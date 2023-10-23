/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "wave_ipc_client.h"
#include "work_serializer.h"
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
#include <time.h>

#define CMD_TIMEOUT_SECS	(20)
#define CMD_TIMEOUT_MSECS	(0)

typedef enum _cmd_response_status {
	RESP_NONE,
	RESP_SUCCESS,
	RESP_FAILURE,
} cmd_response_status;

typedef struct _cmd_response {
	uint8_t seq_num;
	cmd_response_status status;
	wv_ipc_msg *response;
} cmd_response;

typedef struct _listener_thread_data {
	pthread_t thread_id;
	int terminate;

	wv_ipc_event event_clb;
	wv_ipc_cmd cmd_clb;
	wv_ipc_reconnect reconnect_clb;
	void *clb_arg;

	work_serializer *events_serializer;

	l_list *resp_list;
	pthread_cond_t resp_cond;
	pthread_mutex_t resp_cond_lock;
	pthread_mutex_t resp_list_lock;
} listener_thread_data;

struct _wv_ipclient {
	int socket;
	int is_connected;
	int is_after_reconnect;

	uint8_t cmd_seq_num;
	struct sockaddr_un server_sockaddr;
	struct sockaddr_un our_sockaddr;

	wv_ipc_disconnect disconnect_clb;
	void *clb_arg;

	listener_thread_data *listener;
	obj_pool *cmd_resp_pool;

	l_list *queued_msgs;
};

static wv_ipc_ret wave_ipcc_socket_deinit(wv_ipclient *ipclient)
{
	int res;
	wv_ipc_ret ret = WAVE_IPC_SUCCESS;

	if (ipclient->socket == -1)
		goto unlink;

	res = close(ipclient->socket);
	if (res == -1) {
		ELOG("error closing socket, errno = %d", errno);
		ret = WAVE_IPC_ERROR;
	}
	ipclient->socket = -1;

unlink:
	res = unlink(ipclient->our_sockaddr.sun_path + 1);
	if (res == -1 && errno != ENOENT) {
		ELOG("error unlinking from %s, errno = %d",
		     ipclient->our_sockaddr.sun_path, errno);
		ret = WAVE_IPC_ERROR;
	}

	return ret;
}

static wv_ipc_ret wave_ipcc_socket_init(wv_ipclient *ipclient)
{
	int res;
	wv_ipc_ret ret;

	ret = wave_ipcc_socket_deinit(ipclient);
	if (ret != WAVE_IPC_SUCCESS)
		return WAVE_IPC_ERROR;

	ipclient->socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (ipclient->socket == -1) {
		ELOG("error creating socket, errno = %d", errno);
		return WAVE_IPC_ERROR;
	}

	res = bind(ipclient->socket, (struct sockaddr*)&ipclient->our_sockaddr,
		   sizeof(ipclient->our_sockaddr));
	if (res == -1) {
		ELOG("error binding socket %d to %s, errno = %d",
		     ipclient->socket, ipclient->our_sockaddr.sun_path + 1, errno);
		close(ipclient->socket);
		ipclient->socket = -1;
		return WAVE_IPC_ERROR;
	}

	return WAVE_IPC_SUCCESS;
}

static wv_ipc_ret wave_ipcc_connection_attempt(wv_ipclient *ipclient)
{
	int res = connect(ipclient->socket,
		      (struct sockaddr*)&ipclient->server_sockaddr,
		      sizeof(ipclient->server_sockaddr));
	if (res == 0) {
		ipclient->is_connected = 1;
		return WAVE_IPC_SUCCESS;
	} else if (errno == ENOENT) {
		return WAVE_IPC_DISCONNECTED;
	} else if (errno == ECONNREFUSED) {
		return WAVE_IPC_DISCONNECTED;
	} else {
		ELOG("error connecting to socket %d addr: %s, errno = %d",
		     ipclient->socket, ipclient->server_sockaddr.sun_path, errno);
		return WAVE_IPC_ERROR;
	}
}

wv_ipc_ret wave_ipcc_connect(wv_ipclient **handle_p,
			     const char *prog_name, const char *server_name)
{
	wv_ipc_ret ret;
	wv_ipclient *ipclient;

	if (handle_p == NULL || *handle_p != NULL)
		return WAVE_IPC_ERROR;

	if (prog_name == NULL || server_name == NULL)
		return WAVE_IPC_ERROR;

	ipclient = (wv_ipclient*)malloc(sizeof(wv_ipclient));
	if (ipclient == NULL)
		return WAVE_IPC_ERROR;

	memset(ipclient, 0, sizeof(wv_ipclient));
	ipclient->cmd_resp_pool = obj_pool_init("cmd resp", sizeof(cmd_response), 2, 0, 1);
	if (ipclient->cmd_resp_pool == NULL)
		goto free;

	if ((ipclient->queued_msgs = list_init()) == NULL)
		goto free;

	ipclient->our_sockaddr.sun_family = AF_UNIX;
	ipclient->our_sockaddr.sun_path[0] = '\0';
	snprintf(ipclient->our_sockaddr.sun_path + 1,
		 sizeof(ipclient->our_sockaddr.sun_path) - 1,
		 "/tmp/_%s_%d", prog_name, getpid());

	ipclient->server_sockaddr.sun_family = AF_UNIX;
	snprintf(ipclient->server_sockaddr.sun_path,
		 sizeof(ipclient->server_sockaddr.sun_path),
		 "/tmp/_%s", server_name);

	ipclient->socket = -1;
	ret = wave_ipcc_socket_init(ipclient);
	if (ret != WAVE_IPC_SUCCESS) {
		ELOG("ipcc socket init returned err");
		goto free;
	}

	ret = wave_ipcc_connection_attempt(ipclient);
	if (ret == WAVE_IPC_ERROR) {
		ELOG("ipcc connection attempt returned err");
		wave_ipcc_socket_deinit(ipclient);
		goto free;
	} else if (ret == WAVE_IPC_SUCCESS)
		ipclient->is_after_reconnect = 1;

	*handle_p = ipclient;
	return ret;

free:
	if (ipclient->cmd_resp_pool)
		obj_pool_destroy(ipclient->cmd_resp_pool);
	if (ipclient->queued_msgs)
		list_free(ipclient->queued_msgs);
	free(ipclient);
	return WAVE_IPC_ERROR;
}

wv_ipc_ret wave_ipcc_disconnect(wv_ipclient **handle_p)
{
	wv_ipclient *ipclient;
	wv_ipc_ret ret;
	size_t unfreed;

	if (handle_p == NULL || *handle_p == NULL)
		return WAVE_IPC_ERROR;

	ipclient = *handle_p;

	ret = wave_ipcc_socket_deinit(ipclient);
	list_foreach_start(ipclient->queued_msgs, event, wv_ipc_msg)
		wave_ipc_msg_put(event);
		list_foreach_remove_current_entry();
	list_foreach_end
	list_free(ipclient->queued_msgs);
	unfreed = obj_pool_destroy(ipclient->cmd_resp_pool);
	if (unfreed)
		BUG("obj pool has %zu unreturned objects", unfreed);
	free(ipclient);
	*handle_p = NULL;

	return ret;
}

static wv_ipc_ret ipcc_wait_for_message(wv_ipclient *ipclient,
					wv_ipc_reconnect reconnect_clb,
					wv_ipc_terminate_cond term_cond,
					void *clb_arg,
					wv_ipc_msg **msg)
{
	wv_ipc_ret ret;

	while ((!ipclient->listener && !term_cond(clb_arg)) ||
	       (ipclient->listener && !ipclient->listener->terminate)) {
		struct timeval tv;
		fd_set rfds;
		int res;

		if (!ipclient->is_connected) {
			ret = wave_ipcc_connection_attempt(ipclient);
			if (ret == WAVE_IPC_ERROR) {
				ELOG("reconnect error");
				break;
			} else if (ret == WAVE_IPC_DISCONNECTED)
				usleep(10000);
			else if (ret == WAVE_IPC_SUCCESS) {
				if (reconnect_clb)
					reconnect_clb(clb_arg);
				ipclient->is_after_reconnect = 1;
			}

			continue;
		}

		FD_ZERO(&rfds);
		FD_SET(ipclient->socket, &rfds);

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		res = select(ipclient->socket + 1, &rfds, NULL, NULL, &tv);
		if (res == -1 && errno == EINTR) {
			usleep(1000);
		} else if (res == -1) {
			BUG("select() returned error, errno = %d", errno);
			break;
		} else if (res && FD_ISSET(ipclient->socket, &rfds)) {
			ret = wave_ipc_recv_msg(ipclient->socket, msg);
			if (ret != WAVE_IPC_SUCCESS) {
				if (ret == WAVE_IPC_DISCONNECTED) {
					LOG(1, "server reset ==> try to reconnect");
				} else {
					ELOG("ipc_recv_msg returned %d ==> reconnect", ret);
				}

				ipclient->is_connected = 0;
				ipclient->is_after_reconnect = 0;
				if (wave_ipcc_socket_init(ipclient) != WAVE_IPC_SUCCESS) {
					ELOG("ipcc socket init returned err");
					break;
				}

				if (ipclient->disconnect_clb)
					ipclient->disconnect_clb(ipclient->clb_arg);
			} else
				return WAVE_IPC_SUCCESS;
		}
	}

	return WAVE_IPC_ERROR;
}

static void ipcc_handle_response_from_listener_ctx(wv_ipclient *ipclient,
						   wv_ipc_msg *resp,
						   ipc_header *resp_hdr)
{
	cmd_response *cs = NULL;
	uint8_t response_seq_num = resp_hdr->header[1];
	listener_thread_data *listener = ipclient->listener;
	int notify = 1;

	pthread_mutex_lock(&listener->resp_cond_lock);
	pthread_mutex_lock(&listener->resp_list_lock);
	list_foreach_start(listener->resp_list, tmp, cmd_response)
		if (tmp->seq_num == response_seq_num) {
			cs = tmp;
			break;
		}
	list_foreach_end
	if (!cs) {
		pthread_mutex_unlock(&listener->resp_list_lock);
		pthread_mutex_unlock(&listener->resp_cond_lock);
		ELOG("response to seq num %d was thrown", response_seq_num);
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

	pthread_mutex_unlock(&listener->resp_list_lock);
	if (notify)
		pthread_cond_broadcast(&listener->resp_cond);
	pthread_mutex_unlock(&listener->resp_cond_lock);
}

static void* ipcc_listener(void *data)
{
	wv_ipc_ret ret;
	wv_ipclient *ipclient = (wv_ipclient*)data;
	listener_thread_data *listener = ipclient->listener;
	wv_ipc_msg *msg;
	ipc_header hdr;

	while (!listener->terminate) {
		if (list_get_size(ipclient->queued_msgs)) {
			msg = list_pop_front(ipclient->queued_msgs);
			if (msg == NULL) {
				BUG("msg is NULL");
				continue;
			}

			goto queued_msg;
		}

		ret = ipcc_wait_for_message(ipclient, listener->reconnect_clb,
					    NULL, listener->clb_arg, &msg);
		if (ret == WAVE_IPC_ERROR) {
			/* TODO: try to perform some sort of recovery? */
			if (!listener->terminate)
				BUG("wait for message returned error");
			break;
		}
queued_msg:
		ret = ipc_header_pop(msg, &hdr);
		if (ret != WAVE_IPC_SUCCESS) {
			BUG("couldn't read header");
			wave_ipc_msg_put(msg);
			continue;
		}

		switch (hdr.header[0]) {
		case WAVE_IPC_MSG_EVENT:
			if (listener->events_serializer) {
				if (serializer_exec_work_async(listener->events_serializer,
							       0, msg, listener)) {
					BUG("failed push event work to serializer");
					wave_ipc_msg_put(msg);
				}
			} else {
				if (listener->event_clb(listener->clb_arg, msg) !=
				    WAVE_IPC_EVENT_TAKE_OWNERSHIP)
					wave_ipc_msg_put(msg);
			}
			break;
		case WAVE_IPC_MSG_REQ_FAIL:
		/* fall through */
		case WAVE_IPC_MSG_RESP:
			ipcc_handle_response_from_listener_ctx(ipclient, msg, &hdr);
			break;
		case WAVE_IPC_MSG_CMD:
			if (!listener->cmd_clb) {
				BUG("received command from server without cmd cb");
				wave_ipc_msg_put(msg);
				break;
			}

			ret = listener->cmd_clb(listener->clb_arg, hdr.header[1], msg);
			if (ret == WAVE_IPC_EVENT_TAKE_OWNERSHIP)
				break;
			else if (ret)
				wave_ipcc_send_req_failed(ipclient, hdr.header[1]);

			wave_ipc_msg_put(msg);
			break;

		default:
			BUG("received unexpected message type (%d)", hdr.header[0]);
			wave_ipc_msg_put(msg);
		}
	}

	if (!listener->terminate)
		BUG("ipc client listener thread shutting down");

	ipclient->disconnect_clb = NULL;

	return NULL;
}

static int _serialized_event_work_free(void *work_obj, void *ctx)
{
	(void)ctx;
	if (!work_obj)
		return 1;

	wave_ipc_msg_put((wv_ipc_msg *)work_obj);
	return 0;
}

static int _serialized_event_work(work_serializer *s, void *work_obj, void *ctx)
{
	listener_thread_data *listener = (listener_thread_data *)ctx;
	wv_ipc_msg *msg = (wv_ipc_msg *)work_obj;

	(void)s;

	if (!listener || !msg)
		return 1;

	listener->event_clb(listener->clb_arg, msg);

	return 0;
}

wv_ipc_ret wave_ipcc_start_listener(wv_ipclient *handle,
				    wv_ipc_event event_clb,
				    wv_ipc_cmd command_clb,
				    wv_ipc_disconnect disconnect_clb,
				    wv_ipc_reconnect reconnect_clb,
				    void *clb_arg, int use_events_thread)
{
	listener_thread_data *listener;

	if (handle == NULL || event_clb == NULL || handle->listener != NULL)
		return WAVE_IPC_ERROR;

	listener = (listener_thread_data*)malloc(sizeof(listener_thread_data));
	if (listener == NULL)
		return WAVE_IPC_ERROR;

	memset(listener, 0, sizeof(listener_thread_data));
	if (use_events_thread) {
		work_ops_t work_op;
		work_op.work_func = _serialized_event_work;
		work_op.free_func = _serialized_event_work_free;
		work_op.cmp_func = NULL;

		listener->events_serializer = serializer_create(&work_op, 1, 1);
		if (!listener->events_serializer) {
			free(listener);
			return WAVE_IPC_ERROR;
		}
	}

	if ((listener->resp_list = list_init()) == NULL) {
		if (listener->events_serializer)
			serializer_destroy(listener->events_serializer);
		free(listener);
		return WAVE_IPC_ERROR;
	}

	listener->event_clb = event_clb;
	listener->cmd_clb = command_clb;
	listener->reconnect_clb = reconnect_clb;
	listener->clb_arg = clb_arg;
	pthread_mutex_init(&listener->resp_cond_lock, NULL);
	pthread_mutex_init(&listener->resp_list_lock, NULL);
	pthread_cond_init(&listener->resp_cond, NULL);

	handle->listener = listener;
	handle->disconnect_clb = disconnect_clb;
	handle->clb_arg = clb_arg;
	if (pthread_create(&listener->thread_id, NULL, ipcc_listener, handle)) {
		ELOG("thread create failed");

		pthread_cond_destroy(&listener->resp_cond);
		pthread_mutex_destroy(&listener->resp_list_lock);
		pthread_mutex_destroy(&listener->resp_cond_lock);
		list_free(listener->resp_list);
		if (listener->events_serializer)
			serializer_destroy(listener->events_serializer);
		free(listener);
		handle->listener = NULL;
		return WAVE_IPC_ERROR;
	}

	return WAVE_IPC_SUCCESS;
}

wv_ipc_ret wave_ipcc_stop_listener(wv_ipclient *handle)
{
	if (handle == NULL || handle->listener == NULL)
		return WAVE_IPC_ERROR;

	handle->listener->terminate = 1;
	if (pthread_join(handle->listener->thread_id, NULL)) {
		/* TODO: deal with Failures here */
		ELOG("join returned error, errno = %d", errno);
		return WAVE_IPC_ERROR;
	}

	pthread_cond_destroy(&handle->listener->resp_cond);
	pthread_mutex_destroy(&handle->listener->resp_list_lock);
	pthread_mutex_destroy(&handle->listener->resp_cond_lock);
	list_free(handle->listener->resp_list);
	if (handle->listener->events_serializer)
		serializer_destroy(handle->listener->events_serializer);
	free(handle->listener);
	handle->listener = NULL;

	return WAVE_IPC_SUCCESS;
}

static wv_ipc_ret wave_ipcc_send_cmd_listener(wv_ipclient *ipclient, uint8_t seq_num,
					      wv_ipc_msg *cmd, wv_ipc_msg **reply)
{
	wv_ipc_ret ret;
	int cw_res = 0;
	struct timespec ts;
	cmd_response *resp_handle;
	int socket;
	listener_thread_data *listener = ipclient->listener;
	int reconnect_attempts = 0;

	while (reconnect_attempts++ < 15) {
		socket = ipclient->socket;
		if (ipclient->is_connected && ipclient->is_after_reconnect && socket != -1)
			break;
		usleep(200 * 1000);
	}

	socket = ipclient->socket;
	if (!ipclient->is_connected || socket == -1)
		return WAVE_IPC_DISCONNECTED;

	resp_handle = (cmd_response*)obj_pool_alloc_object(ipclient->cmd_resp_pool);
	if (resp_handle == NULL)
		return WAVE_IPC_ERROR;

	resp_handle->response = NULL;
	resp_handle->status = RESP_NONE;
	resp_handle->seq_num = seq_num;

	pthread_mutex_lock(&listener->resp_list_lock);
	list_push_front(listener->resp_list, resp_handle);
	pthread_mutex_unlock(&listener->resp_list_lock);

	ret = wave_ipc_send_msg(socket, cmd, 0);
	if (ret != WAVE_IPC_SUCCESS) {
		ELOG("failed to send command to the server");
		goto response;
	}

	pthread_mutex_lock(&listener->resp_cond_lock);
	clock_gettime(CLOCK_REALTIME, &ts);
	/* TODO: add a couple of miliseconds */
	ts.tv_sec += CMD_TIMEOUT_SECS;
	while (resp_handle->status == RESP_NONE && cw_res == 0) {
		cw_res = pthread_cond_timedwait(&listener->resp_cond,
						&listener->resp_cond_lock, &ts);
	}

	pthread_mutex_unlock(&listener->resp_cond_lock);

response:
	pthread_mutex_lock(&listener->resp_list_lock);
	list_remove(listener->resp_list, resp_handle);
	pthread_mutex_unlock(&listener->resp_list_lock);

	if (ret != WAVE_IPC_SUCCESS)
		goto free;

	if (resp_handle->status == RESP_NONE) {
		ELOG("timeout");
		ret = WAVE_IPC_ERROR;
		goto free;
	}

	if (resp_handle->status == RESP_FAILURE) {
		BUG("server could not handle our command");
		ret = WAVE_IPC_CMD_FAILED;
	}

	*reply = resp_handle->response;
free:
	obj_pool_put_object(ipclient->cmd_resp_pool, resp_handle);
	return ret;
}

static wv_ipc_ret wave_ipcc_send_cmd_no_listener(wv_ipclient *ipclient, uint8_t seq_num,
						 wv_ipc_msg *cmd, wv_ipc_msg **reply)
{
	int res;
	fd_set rfds;
	wv_ipc_ret ret;
	wv_ipc_msg *out, *multi_out = NULL;
	ipc_header hdr;
	struct timeval tv;
	int socket = ipclient->socket;

	if (socket == -1)
		return WAVE_IPC_ERROR;

	if (!ipclient->listener && !ipclient->is_connected)
		wave_ipcc_connection_attempt(ipclient);

	if (!ipclient->is_connected)
		return WAVE_IPC_DISCONNECTED;

	ret = wave_ipc_send_msg(socket, cmd, 0);
	if (ret != WAVE_IPC_SUCCESS) {
		ELOG("failed to send command to the server");
		return ret;
	}

again:
	out = NULL;
	FD_ZERO(&rfds);
	FD_SET(socket, &rfds);
	tv.tv_sec = CMD_TIMEOUT_SECS;
	tv.tv_usec = CMD_TIMEOUT_MSECS * 1000;

	res = select(socket + 1, &rfds, NULL, NULL, &tv);
	if (res == -1 && errno == EINTR) {
		usleep(1000);
		goto again;
	} if (res == -1) {
		BUG("select() returned error, errno = %d", errno);
		goto disconnect;
	} else if (res == 0) {
		ELOG("cmd timeout");
		goto timeout;
	}

	ret = wave_ipc_recv_msg(socket, &out);
	if (ret != WAVE_IPC_SUCCESS) {
		ELOG("ipc_recv_msg returned %d ==> disconnect", ret);
		goto disconnect;
	}

	if (ipc_header_pop(out, &hdr)) {
		BUG("error parsing header ==> disconnect");
		wave_ipc_msg_put(out);
		goto disconnect;
	}

	if (hdr.header[0] == WAVE_IPC_MSG_RESP ||
	    hdr.header[0] == WAVE_IPC_MSG_REQ_FAIL) {
		if (hdr.header[1] == seq_num) {
			if (hdr.header[0] == WAVE_IPC_MSG_REQ_FAIL) {
				if (multi_out)
					wave_ipc_msg_put(multi_out);
				*reply = out;
				return WAVE_IPC_CMD_FAILED;
			}

			if (hdr.header[2] == 0 && multi_out) {
				wave_ipc_multi_msg_append(multi_out, out);
				*reply = multi_out;
				return WAVE_IPC_SUCCESS;
			} else if (hdr.header[2] == 0) {
				*reply = out;
				return WAVE_IPC_SUCCESS;
			} else {
				if (multi_out) {
					wave_ipc_multi_msg_append(multi_out, out);
				} else
					multi_out = out;
				/* there are more messages in this cmd-reply */
				goto again;
			}
		} else if (ipclient->listener) {
			ipcc_handle_response_from_listener_ctx(ipclient, out, &hdr);
			goto again;
		} else {
			BUG("received response for different seq num without listener");
			wave_ipc_msg_put(out);
			goto again;
		}
	} else if (hdr.header[0] == WAVE_IPC_MSG_EVENT ||
		   hdr.header[0] == WAVE_IPC_MSG_CMD) {
		ipc_header_push(out, &hdr);
		list_push_back(ipclient->queued_msgs, out);
		goto again;
	} else {
		BUG("received unexpeted message type (%d)", hdr.header[0]);
		wave_ipc_msg_put(out);
		goto again;
	}

disconnect:
	wave_ipcc_socket_init(ipclient);
	ipclient->is_connected = 0;
	if (ipclient->disconnect_clb)
		ipclient->disconnect_clb(ipclient->clb_arg);
	return WAVE_IPC_DISCONNECTED;
timeout:
	return WAVE_IPC_ERROR;
}

wv_ipc_ret wave_ipcc_send_cmd(wv_ipclient *handle,
			      wv_ipc_msg *cmd, wv_ipc_msg **reply)
{
	ipc_header cmd_hdr = { 0 };
	uint8_t seq_num;
	wv_ipc_ret ret = WAVE_IPC_ERROR;

	if (cmd == NULL || handle == NULL || reply == NULL) {
		ELOG("arguments");
		goto err;
	}

	seq_num = __atomic_add_fetch(&handle->cmd_seq_num, 1, __ATOMIC_SEQ_CST);

	cmd_hdr.header[0] = WAVE_IPC_MSG_CMD;
	cmd_hdr.header[1] = seq_num;
	if (ipc_header_push(cmd, &cmd_hdr)) {
		ELOG("failed to push ipc command header");
		ret = WAVE_IPC_ERROR;
		goto err;
	}

	if (handle->listener && pthread_self() != handle->listener->thread_id)
		ret = wave_ipcc_send_cmd_listener(handle, seq_num, cmd, reply);
	else
		ret = wave_ipcc_send_cmd_no_listener(handle, seq_num, cmd, reply);

err:
	return ret;
}

wv_ipc_ret wave_ipcc_send_response(wv_ipclient *handle, uint8_t seq_num,
				   wv_ipc_msg *reply, uint8_t has_more)
{
	ipc_header resp_hdr = { 0 };
	int socket;

	if (handle == NULL || reply == NULL)
		goto err;

	socket = handle->socket;
	if (socket == -1)
		goto err;

	resp_hdr.header[0] = WAVE_IPC_MSG_RESP;
	resp_hdr.header[1] = seq_num;
	resp_hdr.header[2] = has_more;
	if (ipc_header_push(reply, &resp_hdr))
		goto err;

	return wave_ipc_send_msg(socket, reply, 0);

err:
	return WAVE_IPC_ERROR;
}

wv_ipc_ret wave_ipcc_send_req_failed(wv_ipclient *ipclient, uint8_t seq_num)
{
	wv_ipc_msg *reply = NULL;
	ipc_header resp_hdr = { 0 };
	wv_ipc_ret ret = WAVE_IPC_ERROR;
	int socket;

	if (ipclient == NULL)
		goto err;

	socket = ipclient->socket;
	if (socket == -1)
		goto err;

	if ((reply = wave_ipc_msg_alloc()) == NULL)
		goto err;

	resp_hdr.header[0] = WAVE_IPC_MSG_REQ_FAIL;
	resp_hdr.header[1] = seq_num;
	if (ipc_header_push(reply, &resp_hdr))
		goto err;

	ret = wave_ipc_send_msg(socket, reply, 0);

err:
	if (reply)
		wave_ipc_msg_put(reply);
	return ret;
}

wv_ipc_ret wave_ipcc_blocked_event_listener(wv_ipclient *handle,
					    wv_ipc_event event_clb,
					    wv_ipc_cmd command_clb,
					    wv_ipc_disconnect disconnect_clb,
					    wv_ipc_reconnect reconnect_clb,
					    wv_ipc_terminate_cond term_cond,
					    void *clb_arg)
{
	wv_ipc_ret ret;
	wv_ipc_msg *msg;
	ipc_header hdr;

	if (handle == NULL || term_cond == NULL || event_clb == NULL)
		return WAVE_IPC_ERROR;

	handle->disconnect_clb = disconnect_clb;
	handle->clb_arg = clb_arg;
	while (!term_cond(clb_arg)) {
		if (list_get_size(handle->queued_msgs)) {
			msg = list_pop_front(handle->queued_msgs);
			if (msg == NULL) {
				BUG("msg is NULL");
				continue;
			}

			goto queued_msg;
		}

		ret = ipcc_wait_for_message(handle, reconnect_clb, term_cond,
					    clb_arg, &msg);
		if (ret == WAVE_IPC_ERROR)
			/* TODO: try to perform some sort of recovery? */
			return ret;
queued_msg:
		if (ipc_header_pop(msg, &hdr)) {
			BUG("couldn't read header");
			wave_ipc_msg_put(msg);
			continue;
		}

		switch (hdr.header[0]) {
		case WAVE_IPC_MSG_EVENT:
			if (event_clb(clb_arg, msg) != WAVE_IPC_EVENT_TAKE_OWNERSHIP)
				wave_ipc_msg_put(msg);
			break;
		case WAVE_IPC_MSG_CMD:
			if (!command_clb) {
				BUG("received command from server without cmd cb");
				wave_ipc_msg_put(msg);
				break;
			}

			ret = command_clb(clb_arg, hdr.header[1], msg);
			if (ret == WAVE_IPC_EVENT_TAKE_OWNERSHIP)
				break;
			else if (ret)
				wave_ipcc_send_req_failed(handle, hdr.header[1]);

			wave_ipc_msg_put(msg);
			break;

		default:
			BUG("received unexpeted message type (%d)", hdr.header[0]);
			wave_ipc_msg_put(msg);
		}
	}

	handle->disconnect_clb = NULL;
	return WAVE_IPC_SUCCESS;
}
