/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "wave_ipc_server.h"
#include "linked_list.h"
#include "logs.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/un.h>

#if defined YOCTO
#include <slibc/string.h>
#else
#include <stddef.h>
#include "libsafec/safe_str_lib.h"
#include "libsafec/safe_mem_lib.h"
#endif

#define IPC_CLIENT_ARR_SIZE	(36)
#define IPC_CLIENT_NAME_SIZE	(48)
#define SERV_SOCKET_LISTEN	(16)

typedef enum _cmd_response_status {
	RESP_NONE,
	RESP_SUCCESS,
	RESP_FAILURE,
} cmd_response_status;

typedef struct _cmd_response {
	wv_ipstation *ipsta;
	uint8_t seq_num;
	cmd_response_status status;
	wv_ipc_msg *response;
} cmd_response;

struct _wv_ipstation {
	int refcnt;
        int socket;
	int is_connected;
	void *data;  /**< user-defined data;*/
	char name[IPC_CLIENT_NAME_SIZE];

	int has_pending_msgs;
	int resend_attempts;
	l_list *pending_msgs;
	l_list *pending_replies;
};

#define MAX_NUM_PENDING_IPC_MSGS		(1000)
#define STA_MAX_NUM_PENDING_ON_SERV_LIMIT	(200)

struct _wv_ipserver {
	int listener_socket;
	char buf[WAVE_IPC_BUFF_SIZE];
	struct sockaddr_un sockaddr;

	wv_ipstation *stations[IPC_CLIENT_ARR_SIZE];
	unsigned int num_stas;
	pthread_mutex_t pending_lock;

	wv_ipserver_callbacks clbacks;

	int station_has_pending_msgs;

	/* Thread id of the server's wave_ipcs_run() */
	pthread_t thread_id;

	uint8_t cmd_seq_num;
	l_list *resp_list;
	pthread_mutex_t resp_list_lock;
	pthread_mutex_t resp_cond_lock;
	pthread_cond_t resp_cond;
};

#define PEND_MSGS_LOCK(srv) pthread_mutex_lock(&srv->pending_lock)
#define PEND_MSGS_UNLOCK(srv) pthread_mutex_unlock(&srv->pending_lock)

static int wave_ipcs_cmd_async(wv_ipserver *ipserv, wv_ipstation *ipsta,
				uint8_t seq_num, wv_ipc_msg *cmd)
{
	if (ipserv->clbacks.cmd_async)
		return ipserv->clbacks.cmd_async(ipserv, ipsta, seq_num, cmd);

	wave_ipc_msg_put(cmd);
	return 1;
}

static int wave_ipcs_stop_cond(wv_ipserver *ipserv)
{
	if (ipserv->clbacks.stop_cond)
		return ipserv->clbacks.stop_cond(ipserv);

	return 1;
}

static int wave_ipcs_adding_client(wv_ipserver *ipserv, wv_ipstation *ipsta)
{
	if (ipserv->clbacks.adding_client)
		return ipserv->clbacks.adding_client(ipserv, ipsta);

	return 1;
}

static int wave_ipcs_removing_client(wv_ipserver *ipserv, wv_ipstation *ipsta)
{
	if (ipserv->clbacks.removing_client)
		return ipserv->clbacks.removing_client(ipserv, ipsta);

	return 1;
}

wv_ipc_ret wave_ipcs_create(wv_ipserver **handle_p, const char *server_name)
{
	wv_ipserver *serv;
	int res;

	if (handle_p == NULL || *handle_p != NULL)
		return WAVE_IPC_ERROR;

	if (server_name == NULL)
		return WAVE_IPC_ERROR;

	serv = (wv_ipserver*)malloc(sizeof(wv_ipserver));
	if (serv == NULL)
		return WAVE_IPC_ERROR;

	memset(serv, 0, sizeof(wv_ipserver));

	pthread_mutex_init(&serv->pending_lock, NULL);

	serv->listener_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (serv->listener_socket == -1) {
		ELOG("error creating socket, errno = %d", errno);
		goto free;
	}

	serv->sockaddr.sun_family = AF_UNIX;
	snprintf(serv->sockaddr.sun_path, sizeof(serv->sockaddr.sun_path),
		 "/tmp/_%s", server_name);

	res = unlink(serv->sockaddr.sun_path);
	if (res == -1 && errno != ENOENT) {
		ELOG("error unlinking from %s, errno = %d",
		       serv->sockaddr.sun_path, errno);
		goto close;
	}

	res = bind(serv->listener_socket, (struct sockaddr *)&serv->sockaddr,
		   sizeof(serv->sockaddr));
	if (res == -1) {
		ELOG("error binding socket to %s, errno = %d",
		       serv->sockaddr.sun_path, errno);
		goto close;
	}

	res = listen(serv->listener_socket, SERV_SOCKET_LISTEN);
	if (res == -1) {
		ELOG("error listening to socket, errno = %d", errno);
		goto close;
	}

	if ((serv->resp_list = list_init()) == NULL)
		goto close;

	pthread_mutex_init(&serv->resp_cond_lock, NULL);
	pthread_mutex_init(&serv->resp_list_lock, NULL);
	pthread_cond_init(&serv->resp_cond, NULL);

	*handle_p = serv;
	return WAVE_IPC_SUCCESS;
close:
	if (serv->resp_list)
		list_free(serv->resp_list);
	close(serv->listener_socket);
free:
	pthread_mutex_destroy(&serv->pending_lock);
	free(serv);
	return WAVE_IPC_ERROR;
}

wv_ipc_ret wave_ipcs_delete(wv_ipserver **handle_p)
{
	wv_ipserver *ipserver;
	int i;

	if (handle_p == NULL || *handle_p == NULL)
		return WAVE_IPC_ERROR;

	ipserver = *handle_p;

	unlink(ipserver->sockaddr.sun_path);
	close(ipserver->listener_socket);

	for (i = 0; i < IPC_CLIENT_ARR_SIZE; i++) {
		if (ipserver->stations[i] == NULL)
			continue;

		wave_ipcs_removing_client(ipserver, ipserver->stations[i]);
		wave_ipcs_sta_decref(ipserver->stations[i]);
		ipserver->stations[i] = NULL;
	}

	pthread_mutex_destroy(&ipserver->pending_lock);

	pthread_cond_destroy(&ipserver->resp_cond);
	pthread_mutex_destroy(&ipserver->resp_list_lock);
	pthread_mutex_destroy(&ipserver->resp_cond_lock);
	list_free(ipserver->resp_list);

	free(ipserver);

	*handle_p = NULL;
	return WAVE_IPC_SUCCESS;
}

static void wave_ipcs_handle_response_from_client(wv_ipserver *ipserv,
						  wv_ipstation *ipsta,
						  wv_ipc_msg *resp,
						  ipc_header *resp_hdr)
{
	cmd_response *cs = NULL;
	uint8_t response_seq_num = resp_hdr->header[1];
	int notify = 1;

	pthread_mutex_lock(&ipserv->resp_cond_lock);
	pthread_mutex_lock(&ipserv->resp_list_lock);
	list_foreach_start(ipserv->resp_list, tmp, cmd_response)
		if (tmp->seq_num == response_seq_num &&
		    tmp->ipsta == ipsta) {
			cs = tmp;
			break;
		}
	list_foreach_end
	if (!cs) {
		pthread_mutex_unlock(&ipserv->resp_list_lock);
		pthread_mutex_unlock(&ipserv->resp_cond_lock);
		ELOG("response to seq num %d from '%s'was thrown",
		     response_seq_num, wave_ipcs_sta_name(ipsta));
		wave_ipc_msg_put(resp);
		return;
	}

	if (resp_hdr->header[0] == WAVE_IPC_MSG_RESP) {
		if (resp_hdr->header[2] == 0) {
			/* last response */
			cs->status = RESP_SUCCESS;
			if (cs->response == NULL)
				/* single response */
				cs->response = resp;
			else
				/* last response in a multimsg resp */
				wave_ipc_multi_msg_append(cs->response, resp);
		} else {
			/* multi msg response */
			if (cs->response == NULL)
				/* first response in a multimsg resp */
				cs->response = resp;
			else
				/* middle response in a multimsg resp */
				wave_ipc_multi_msg_append(cs->response, resp);

			/* don't notify about response, since it's not complete */
			notify = 0;
		}
	} else {
		cs->status = RESP_FAILURE;
		if (cs->response)
			wave_ipc_msg_put(cs->response);
		cs->response = resp;
	}

	pthread_mutex_unlock(&ipserv->resp_list_lock);
	if (notify)
		pthread_cond_broadcast(&ipserv->resp_cond);
	pthread_mutex_unlock(&ipserv->resp_cond_lock);
}

static wv_ipc_ret wave_ipcs_handle_station_msg(wv_ipserver *ipserver, int index)
{
	wv_ipstation *station = ipserver->stations[index];
	int num_pending = 0;
	wv_ipc_msg *msg = NULL;
	wv_ipc_ret ret;
	ipc_header hdr;

	ret = wave_ipc_recv_msg(station->socket, &msg);
	if (ret == WAVE_IPC_DISCONNECTED) {
		LOG(1, "station '%s' disconnected", wave_ipcs_sta_name(station));
		goto disconnect;
	} if (ret != WAVE_IPC_SUCCESS) {
		ELOG("ipc_recv_msg returned err=%d ==> disconnect station '%s'",
		     ret, wave_ipcs_sta_name(station));
		goto disconnect;
	}

	if (ipc_header_pop(msg, &hdr)) {
		wave_ipc_msg_put(msg);
		goto disconnect;
	}

	switch (hdr.header[0]) {
	case WAVE_IPC_MSG_CMD:
		if (wave_ipcs_cmd_async(ipserver, station, hdr.header[1], msg)) {
			ELOG("upper layer returned err on cmd from '%s'",
			     wave_ipcs_sta_name(station));

			if (WAVE_IPC_SUCCESS !=
			    wave_ipcs_send_req_failed_to(ipserver, station, hdr.header[1])) {
				ELOG("send_req_failed_to returned err");
				goto disconnect;
			}
		}
		break;

	case WAVE_IPC_MSG_RESP:
		wave_ipcs_handle_response_from_client(ipserver, station, msg, &hdr);
		break;

	default:
		ELOG("received unsupported msg type : %d from '%s'",
		     hdr.header[0], wave_ipcs_sta_name(station));
		wave_ipc_msg_put(msg);
		goto disconnect;
	}

	return WAVE_IPC_SUCCESS;

disconnect:
	PEND_MSGS_LOCK(ipserver);
	num_pending = list_get_size(station->pending_msgs);
	num_pending += list_get_size(station->pending_replies);
	ipserver->station_has_pending_msgs -= num_pending;
	station->is_connected = 0;
	PEND_MSGS_UNLOCK(ipserver);

	wave_ipcs_removing_client(ipserver, station);
	wave_ipcs_sta_decref(station);
	ipserver->stations[index] = NULL;

	return WAVE_IPC_DISCONNECTED;
}

static void wave_ipcs_sta_resend_pending(wv_ipserver *ipserver, wv_ipstation *sta)
{
	wv_ipc_ret ret = WAVE_IPC_SUCCESS;
	int failed_on_first = 1;
	l_list *pending_list = sta->pending_replies;

again:
	list_foreach_start(pending_list, msg, wv_ipc_msg)
		ret = wave_ipc_send_msg(sta->socket, msg, MSG_DONTWAIT);
		if (ret != WAVE_IPC_SUCCESS) {
			if (failed_on_first) {
				sta->resend_attempts++;
				LOG(1, "station %s is still blocking, resend attempts=%d",
				    wave_ipcs_sta_name(sta), sta->resend_attempts);
			}

			break;
		}

		ipserver->station_has_pending_msgs--;
		sta->resend_attempts = 0;
		failed_on_first = 0;
		wave_ipc_msg_put(msg);
		list_foreach_remove_current_entry()
	list_foreach_end

	if (ret == WAVE_IPC_SUCCESS && pending_list == sta->pending_replies) {
		pending_list = sta->pending_msgs;
		goto again;
	}

	sta->has_pending_msgs = list_get_size(sta->pending_replies) ? 1 : 0;
	if (!sta->has_pending_msgs)
		sta->has_pending_msgs = list_get_size(sta->pending_msgs) ? 1 : 0;
	if (!sta->has_pending_msgs)
		LOG(1, "station %s exited blocking state", wave_ipcs_sta_name(sta));
}

static wv_ipc_ret wave_ipcs_accept_new_client(wv_ipserver *ipserver)
{
	struct sockaddr_un sockaddr;
	wv_ipstation *station;
	unsigned int len;
	int i;

	station = (wv_ipstation*)malloc(sizeof(wv_ipstation));
	if (station == NULL)
		return WAVE_IPC_ERROR;

	memset(station, 0, sizeof(wv_ipstation));

	len = sizeof(sockaddr);
	station->socket = accept(ipserver->listener_socket,
				 (struct sockaddr*)&sockaddr, &len);
	if (station->socket == -1)
		goto err;

	station->pending_msgs = list_init();
	if (!station->pending_msgs)
		goto err;

	station->pending_replies = list_init();
	if (!station->pending_replies)
		goto err;

	strncpy_s(station->name, sizeof(station->name),
		  sockaddr.sun_path + 1, sizeof(station->name) - 1);

	if (ipserver->num_stas == IPC_CLIENT_ARR_SIZE) {
		ELOG("can't accept any more clients");
		goto err;
	}

	for (i = 0; i < IPC_CLIENT_ARR_SIZE; i++) {
		if (ipserver->stations[i] == NULL)
			break;
	}

	if (i == IPC_CLIENT_ARR_SIZE)
		goto err;

	station->is_connected = 1;
	ipserver->stations[i] = station;
	wave_ipcs_sta_incref(ipserver->stations[i]);
	wave_ipcs_adding_client(ipserver, ipserver->stations[i]);

	return WAVE_IPC_SUCCESS;

err:
	if (station->socket != -1)
		close(station->socket);
	if (station->pending_msgs)
		list_free(station->pending_msgs);
	if (station->pending_replies)
		list_free(station->pending_replies);
	free(station);
	return WAVE_IPC_ERROR;
}

wv_ipc_ret wave_ipcs_run(wv_ipserver *handle, wv_ipserver_callbacks *callbacks)
{
	wv_ipstation **stations;
	struct timespec next_resend_ts;
	int next_resend_ts_set = 0;

	if (handle == NULL || handle->listener_socket == -1)
		return WAVE_IPC_ERROR;

	if (callbacks == NULL ||
	    callbacks->stop_cond == NULL ||
	    callbacks->cmd_async == NULL) {
		return WAVE_IPC_ERROR;
	}

	memcpy_s(&handle->clbacks, sizeof(wv_ipserver_callbacks),
		 callbacks, sizeof(wv_ipserver_callbacks));
	stations = handle->stations;

	handle->thread_id = pthread_self();

	while (!wave_ipcs_stop_cond(handle)) {
		fd_set rfds;
		struct timeval tv;
		int ret, i, highest_fd;

		FD_ZERO(&rfds);
		FD_SET(handle->listener_socket, &rfds);
		highest_fd = handle->listener_socket;

		for (i = 0; i < IPC_CLIENT_ARR_SIZE; i++) {
			int clinet_fd;

			if (stations[i] == NULL)
				continue;

			clinet_fd = stations[i]->socket;
			if (clinet_fd == -1)
				return WAVE_IPC_ERROR;

			FD_SET(clinet_fd, &rfds);
			if (clinet_fd > highest_fd)
				highest_fd = clinet_fd;
		}

		if (handle->station_has_pending_msgs)
			tv.tv_sec = 1;
		else
			tv.tv_sec = 2;
		tv.tv_usec = 0;

		ret = select(highest_fd + 1, &rfds, NULL, NULL, &tv);
		if (ret == -1 && errno == EINTR) {
			ELOG("select() returned EINTR");
			usleep(1000);
			continue;
		} else if (ret == -1) {
			BUG("select() returned error (errno=%d)", errno);
			return WAVE_IPC_ERROR;
		} else if (ret == 0)
			goto pending;

		if (FD_ISSET(handle->listener_socket, &rfds))
			wave_ipcs_accept_new_client(handle);

		for (i = 0; i < IPC_CLIENT_ARR_SIZE; i++) {
			if (stations[i] == NULL)
				continue;

			if (!FD_ISSET(stations[i]->socket, &rfds))
				continue;

			wave_ipcs_handle_station_msg(handle, i);
		}

pending:
		if (handle->station_has_pending_msgs) {

			if (!next_resend_ts_set) {
				clock_gettime(CLOCK_BOOTTIME, &next_resend_ts);
				next_resend_ts.tv_sec += 1;
				next_resend_ts_set = 1;
			} else {
				struct timespec now_ts;

				clock_gettime(CLOCK_BOOTTIME, &now_ts);
				if (now_ts.tv_sec < next_resend_ts.tv_sec ||
				    (now_ts.tv_sec == next_resend_ts.tv_sec &&
				     now_ts.tv_nsec < next_resend_ts.tv_nsec))
					continue; /* woke up too early */

				clock_gettime(CLOCK_BOOTTIME, &next_resend_ts);
				next_resend_ts.tv_sec += 1;
			}

			PEND_MSGS_LOCK(handle);
			for (i = 0; i < IPC_CLIENT_ARR_SIZE; i++) {
				wv_ipstation *sta = stations[i];

				if (sta == NULL)
					continue;

				if (sta->has_pending_msgs) {
					wave_ipcs_sta_resend_pending(handle, sta);
				} else
					continue;

				if (sta->resend_attempts >= 10) {
					/* client is probably stuck -> remove it */
					int num_pending = 0;

					num_pending = list_get_size(sta->pending_msgs);
					num_pending += list_get_size(sta->pending_replies);
					handle->station_has_pending_msgs -= num_pending;

					ELOG("removing station %s due to not not receiving msgs",
					     wave_ipcs_sta_name(sta));
					sta->is_connected = 0;
					wave_ipcs_removing_client(handle, sta);
					wave_ipcs_sta_decref(sta);
					stations[i] = NULL;
				}
			}

			if (!handle->station_has_pending_msgs)
				next_resend_ts_set = 0;
			PEND_MSGS_UNLOCK(handle);
		}
	}
	handle->thread_id = 0;

	return WAVE_IPC_SUCCESS;
}

static wv_ipc_ret wave_ipcs_send_would_block(wv_ipserver *ipserver,
					     wv_ipstation *ipsta, wv_ipc_msg *msg,
					     l_list *pending_list)
{
	int print_log = 0;
	wv_ipc_msg *dup_msg = wave_ipc_msg_dup(msg);
	if (dup_msg == NULL)
		return WAVE_IPC_ERROR;

	PEND_MSGS_LOCK(ipserver);
	if (!ipsta->is_connected) {
		PEND_MSGS_UNLOCK(ipserver);
		return WAVE_IPC_DISCONNECTED;
	}

	if (ipserver->station_has_pending_msgs >= MAX_NUM_PENDING_IPC_MSGS &&
	    (list_get_size(ipsta->pending_msgs) +
	     list_get_size(ipsta->pending_replies)) >= STA_MAX_NUM_PENDING_ON_SERV_LIMIT) {
		LOG(2, "can't store more pending messages for '%s', limit reached",
		    wave_ipcs_sta_name(ipsta));
		wave_ipc_msg_put(dup_msg);
		PEND_MSGS_UNLOCK(ipserver);
		return WAVE_IPC_ERROR;
	}

	list_push_back(pending_list, dup_msg);
	if (!ipsta->has_pending_msgs) {
		print_log = 1;
		ipsta->has_pending_msgs = 1;
	}

	ipserver->station_has_pending_msgs++;
	PEND_MSGS_UNLOCK(ipserver);

	if (print_log)
		ELOG("station %s became blocking", wave_ipcs_sta_name(ipsta));

	return WAVE_IPC_SUCCESS;
}

static wv_ipc_ret wave_ipcs_send_reply(wv_ipserver *ipserver, wv_ipstation *ipsta,
				       wv_ipc_msg *reply)
{
	wv_ipc_ret ret = WAVE_IPC_ERROR;
	int socket;

	socket = ipsta->socket;
	if (socket == -1)
		goto err;

	if (!ipsta->has_pending_msgs)
		ret = wave_ipc_send_msg(socket, reply, MSG_DONTWAIT);
	else
		ret = WAVE_IPC_CMD_WOULD_BLOCK;

	if (ret == WAVE_IPC_CMD_WOULD_BLOCK)
		ret = wave_ipcs_send_would_block(ipserver, ipsta, reply,
						 ipsta->pending_replies);

err:
	return ret;
}

wv_ipc_ret wave_ipcs_send_response_to(wv_ipserver *handle, wv_ipstation *ipsta,
				      uint8_t seq_num, wv_ipc_msg *reply,
				      uint8_t has_more)
{
	ipc_header resp_hdr = { 0 };
	int socket;

	if (handle == NULL || ipsta == NULL || reply == NULL)
		goto err;

	socket = ipsta->socket;
	if (socket == -1)
		goto err;

	resp_hdr.header[0] = WAVE_IPC_MSG_RESP;
	resp_hdr.header[1] = seq_num;
	resp_hdr.header[2] = has_more;
	if (ipc_header_push(reply, &resp_hdr))
		goto err;

	return wave_ipcs_send_reply(handle, ipsta, reply);

err:
	return WAVE_IPC_ERROR;
}

wv_ipc_ret wave_ipcs_send_req_failed_to(wv_ipserver *handle, wv_ipstation *ipsta,
					uint8_t seq_num)
{
	wv_ipc_msg *reply = NULL;
	ipc_header resp_hdr = { 0 };
	wv_ipc_ret ret = WAVE_IPC_ERROR;
	int socket;

	if (handle == NULL || ipsta == NULL)
		goto err;

	socket = ipsta->socket;
	if (socket == -1)
		goto err;

	if ((reply = wave_ipc_msg_alloc()) == NULL)
		goto err;

	resp_hdr.header[0] = WAVE_IPC_MSG_REQ_FAIL;
	resp_hdr.header[1] = seq_num;
	if (ipc_header_push(reply, &resp_hdr))
		goto err;

	ret = wave_ipcs_send_reply(handle, ipsta, reply);

err:
	if (reply)
		wave_ipc_msg_put(reply);
	return ret;
}

static wv_ipc_ret _wave_ipcs_send_cmd_to(wv_ipserver *handle, wv_ipstation *ipsta,
					 uint8_t seq_num, wv_ipc_msg *cmd,
					 wv_ipc_msg **reply)
{
	wv_ipc_ret ret;
	int cw_res = 0;
	struct timespec ts;
	cmd_response *resp_handle;

	resp_handle = (cmd_response*)malloc(sizeof(cmd_response));
	if (resp_handle == NULL)
		return WAVE_IPC_ERROR;

	resp_handle->ipsta = ipsta;
	resp_handle->response = NULL;
	resp_handle->status = RESP_NONE;
	resp_handle->seq_num = seq_num;

	pthread_mutex_lock(&handle->resp_list_lock);
	list_push_front(handle->resp_list, resp_handle);
	pthread_mutex_unlock(&handle->resp_list_lock);

	ret = wave_ipc_send_msg(ipsta->socket, cmd, MSG_DONTWAIT);
	if (ret != WAVE_IPC_SUCCESS) {
		ELOG("failed to send command to '%s'", wave_ipcs_sta_name(ipsta));
		goto response;
	}

	pthread_mutex_lock(&handle->resp_cond_lock);
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 10;
	while (resp_handle->status == RESP_NONE && cw_res == 0)
		cw_res = pthread_cond_timedwait(&handle->resp_cond,
						&handle->resp_cond_lock, &ts);

	pthread_mutex_unlock(&handle->resp_cond_lock);

response:
	pthread_mutex_lock(&handle->resp_list_lock);
	list_remove(handle->resp_list, resp_handle);
	pthread_mutex_unlock(&handle->resp_list_lock);

	if (ret != WAVE_IPC_SUCCESS)
		goto free;

	if (resp_handle->status == RESP_NONE) {
		ELOG("timeout while waiting for response from '%s", wave_ipcs_sta_name(ipsta));
		ret = WAVE_IPC_ERROR;
		goto free;
	}

	if (resp_handle->status == RESP_FAILURE) {
		BUG("client '%s' could not handle our command", wave_ipcs_sta_name(ipsta));
		ret = WAVE_IPC_CMD_FAILED;
	}

	*reply = resp_handle->response;
free:
	free(resp_handle);
	return ret;
}

wv_ipc_ret wave_ipcs_send_cmd_to(wv_ipserver *handle, wv_ipstation *ipsta,
				 wv_ipc_msg *cmd, wv_ipc_msg **reply)
{
	ipc_header cmd_hdr = { 0 };
	uint8_t seq_num;

	if (ipsta == NULL || cmd == NULL || handle == NULL || reply == NULL)
		return WAVE_IPC_ERROR;

	if (!ipsta->is_connected || ipsta->socket == -1)
		return WAVE_IPC_DISCONNECTED;

	if (!handle->thread_id) {
		BUG("Cannot send commands while listener thread is down");
		return WAVE_IPC_ERROR;
	}

	if (pthread_self() == handle->thread_id) {
		BUG("Cannot send commands from the commands listener thread");
		return WAVE_IPC_ERROR;
	}

	seq_num = __atomic_add_fetch(&handle->cmd_seq_num, 1, __ATOMIC_SEQ_CST);

	cmd_hdr.header[0] = WAVE_IPC_MSG_CMD;
	cmd_hdr.header[1] = seq_num;
	if (ipc_header_push(cmd, &cmd_hdr))
		return WAVE_IPC_ERROR;

	return _wave_ipcs_send_cmd_to(handle, ipsta, seq_num, cmd, reply);
}

wv_ipc_ret wave_ipcs_send_event_all(wv_ipserver *handle, wv_ipc_msg *event)
{
	wv_ipc_ret ret = WAVE_IPC_SUCCESS;
	int i;

	if (event == NULL || handle == NULL)
		return WAVE_IPC_ERROR;

	wave_ipcs_push_event_header(event);
	for (i = 0; i < IPC_CLIENT_ARR_SIZE; i++) {
		wv_ipstation *ipsta = handle->stations[i];

		if (ipsta == NULL)
			continue;

		if (wave_ipcs_send_to(handle, event, ipsta) == WAVE_IPC_ERROR)
			ret = WAVE_IPC_ERROR;
	}

	return ret;
}

wv_ipc_ret wave_ipcs_send_event_to(wv_ipserver *handle, wv_ipc_msg *event,
				   wv_ipstation *ipsta)
{
	if (event == NULL || handle == NULL || ipsta == NULL)
		return WAVE_IPC_ERROR;

	if (WAVE_IPC_SUCCESS != wave_ipcs_push_event_header(event))
		return WAVE_IPC_ERROR;

	return wave_ipcs_send_to(handle, event, ipsta);
}

wv_ipc_ret wave_ipcs_push_event_header(wv_ipc_msg *event)
{
	ipc_header event_hdr = { 0 };

	event_hdr.header[0] = WAVE_IPC_MSG_EVENT;
	return ipc_header_push(event, &event_hdr);
}

wv_ipc_ret wave_ipcs_send_to(wv_ipserver *handle, wv_ipc_msg *event, wv_ipstation *ipsta)
{
	int socket;
	wv_ipc_ret ret = WAVE_IPC_ERROR;

	if (event == NULL)
		return WAVE_IPC_ERROR;

	if (handle == NULL || ipsta == NULL)
		goto err;

	socket = ipsta->socket;
	if (socket == -1)
		goto err;

	if (!ipsta->has_pending_msgs)
		ret = wave_ipc_send_msg(socket, event, MSG_DONTWAIT);
	else
		ret = WAVE_IPC_CMD_WOULD_BLOCK;

	if (ret == WAVE_IPC_CMD_WOULD_BLOCK)
		ret = wave_ipcs_send_would_block(handle, ipsta, event,
						 ipsta->pending_msgs);

err:
	return ret;
}

wv_ipc_ret wave_ipcs_sta_incref(wv_ipstation *ipclient)
{
	if (ipclient == NULL)
		return WAVE_IPC_ERROR;

	__atomic_add_fetch(&ipclient->refcnt, 1, __ATOMIC_SEQ_CST);
	return WAVE_IPC_SUCCESS;
}

wv_ipc_ret wave_ipcs_sta_decref(wv_ipstation *ipclient)
{
	int refcnt;

	if (ipclient == NULL)
		return WAVE_IPC_ERROR;

	refcnt = __atomic_sub_fetch(&ipclient->refcnt, 1, __ATOMIC_SEQ_CST);
	if (refcnt == 0) {
		l_list *pending_list;
		close(ipclient->socket);

		pending_list = ipclient->pending_replies;
again:
		list_foreach_start(pending_list, msg, wv_ipc_msg)
			wave_ipc_msg_put(msg);
			list_foreach_remove_current_entry()
		list_foreach_end
		list_free(pending_list);

		if (pending_list == ipclient->pending_replies) {
			pending_list = ipclient->pending_msgs;
			goto again;
		}

		free(ipclient);
	}
	return WAVE_IPC_SUCCESS;
}

const char* wave_ipcs_sta_name(wv_ipstation *ipsta)
{
	return ipsta ? ipsta->name : "<err>";
}

void wave_ipcs_sta_set_data(wv_ipstation *ipsta, void *data)
{
	if (ipsta) ipsta->data = data;
}

void* wave_ipcs_sta_get_data(wv_ipstation *ipsta)
{
	return ipsta ? ipsta->data : NULL;
}
