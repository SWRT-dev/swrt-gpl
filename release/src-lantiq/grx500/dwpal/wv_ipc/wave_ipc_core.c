/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "wave_ipc_core.h"
#include "obj_pool.h"
#include "logs.h"

#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

#if defined YOCTO
#include <slibc/string.h>
#else
#include <stddef.h>
#include "libsafec/safe_str_lib.h"
#include "libsafec/safe_mem_lib.h"
#endif

#define IPC_MSG_HDR_SIZE	32

typedef struct __attribute__((__packed__)) {
	uint16_t data_len;
	uint8_t hdr[IPC_MSG_HDR_SIZE];
} wv_ipc_msg_info;

typedef struct __attribute__((__packed__)) {
	wv_ipc_msg_info info;
	char data[WAVE_IPC_BUFF_SIZE];
} wv_ipc_msg_transmitted_data;

struct __attribute__((__packed__)) _wv_ipc_msg {
	/* for multi msg response support */
	wv_ipc_msg *prev, *next;
	uint8_t is_head;

	/* actual data sent over socket */
	wv_ipc_msg_transmitted_data packet;
};

static obj_pool *ipc_msg_pool = NULL;
static pthread_mutex_t ipc_msg_pool_lock = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_IPC_MSG_POOL() pthread_mutex_lock(&ipc_msg_pool_lock);
#define UNLOCK_IPC_MSG_POOL() pthread_mutex_unlock(&ipc_msg_pool_lock);

static void ipc_msg_pool_cleanup(void)
{
	size_t unfreed = obj_pool_destroy(ipc_msg_pool);
	if (unfreed)
		BUG("obj pool has %zu unreturned objects", unfreed);
}

wv_ipc_msg* wave_ipc_msg_alloc(void)
{
	wv_ipc_msg *msg = NULL;

	LOCK_IPC_MSG_POOL()
	if (ipc_msg_pool == NULL) {
		ipc_msg_pool = obj_pool_init("ipc msg", sizeof(wv_ipc_msg), 8, 0, 0);
		if (ipc_msg_pool == NULL) {
			UNLOCK_IPC_MSG_POOL()
			BUG("Failed to initialize obj pool");
			return NULL;
		}
		atexit(ipc_msg_pool_cleanup);
	}

	msg = (wv_ipc_msg*)obj_pool_alloc_object(ipc_msg_pool);
	UNLOCK_IPC_MSG_POOL()

	if (msg) {
		msg->prev = NULL;
		msg->next = NULL;
		msg->is_head = 0;
		msg->packet.info.data_len = 0;
		memset(msg->packet.info.hdr, 0, sizeof(msg->packet.info.hdr));
	} else
		BUG("alloc object returned NULL");

	return msg;
}

wv_ipc_ret wave_ipc_multi_msg_append(wv_ipc_msg *head, wv_ipc_msg *msg)
{
	if (head == NULL || msg == NULL)
		return WAVE_IPC_ERROR;

	if (head->next && !head->is_head) {
		BUG("incorrect head message");
		return WAVE_IPC_ERROR;
	}

	if (msg->next || msg->is_head) {
		BUG("incorrect msg to append");
		return WAVE_IPC_ERROR;
	}

	if (head->next == NULL) {
		head->is_head = 1;

		head->next = msg;
		msg->prev = head;
	} else {
		msg->prev = head->prev;
		head->prev->next = msg;
	}
	head->prev = msg;
	msg->next = head;

	return WAVE_IPC_SUCCESS;
}

int wave_ipc_msg_is_multi_msg(wv_ipc_msg *msg)
{
	if (msg == NULL || !msg->is_head)
		return 0;
	return (msg->next != NULL);
}

wv_ipc_msg * wave_ipc_multi_msg_get_next(wv_ipc_msg *msg)
{
	if (msg == NULL || msg->next == NULL)
		return NULL;

	return (msg->next->is_head) ? NULL : msg->next;
}

wv_ipc_msg* wave_ipc_msg_dup(wv_ipc_msg *orig)
{
	wv_ipc_msg *clone = NULL;

	if (orig == NULL) {
		BUG("orig is NULL!");
		return NULL;
	}

	if (orig->prev || orig->next) {
		/* TODO: fix this? */
		ELOG("dup not supported for multimsg");
		return NULL;
	}

	clone = wave_ipc_msg_alloc();
	if (clone == NULL) {
		ELOG("alloc object returned NULL");
		return NULL;
	}

	memcpy_s(clone, sizeof(wv_ipc_msg), orig, sizeof(wv_ipc_msg));
	return clone;
}

void wave_ipc_msg_put(wv_ipc_msg *msg)
{
	if (msg == NULL) {
		BUG("msg is NULL!");
		return;
	}

	if (msg->next && !msg->is_head) {
		BUG("msg is from a multimsg response but not the head");
		return;
	}

	LOCK_IPC_MSG_POOL()
	if (wave_ipc_msg_is_multi_msg(msg)) {
		wv_ipc_msg *next = msg->next;
		while (next != msg && next != NULL) {
			wv_ipc_msg *next_next = next->next;
			obj_pool_put_object(ipc_msg_pool, (void*)next);
			next = next_next;
		}
	}
	obj_pool_put_object(ipc_msg_pool, (void*)msg);
	UNLOCK_IPC_MSG_POOL()
}

wv_ipc_ret wave_ipc_msg_fill_data(wv_ipc_msg *msg, const char *data, size_t len)
{
	if (msg == NULL || (data == NULL && len != 0))
		return WAVE_IPC_ERROR;

	if (len > WAVE_IPC_BUFF_SIZE)
		return WAVE_IPC_ERROR;

	if (data == NULL) {
		msg->packet.info.data_len = 0;
		return WAVE_IPC_SUCCESS;
	}

	if (msg->packet.info.data_len)
		return WAVE_IPC_ERROR;

	memcpy_s(msg->packet.data, sizeof(msg->packet.data), data, len);
	msg->packet.info.data_len = len;
	return WAVE_IPC_SUCCESS;
}

wv_ipc_ret wave_ipc_msg_append_data(wv_ipc_msg *msg, const char *data, size_t len)
{
	if (msg == NULL || data == NULL || len == 0)
		return WAVE_IPC_ERROR;

	if (len + msg->packet.info.data_len > WAVE_IPC_BUFF_SIZE)
		return WAVE_IPC_ERROR;

	memcpy_s(msg->packet.data + msg->packet.info.data_len,
		 sizeof(msg->packet.data) - msg->packet.info.data_len, data, len);
	msg->packet.info.data_len += len;
	return WAVE_IPC_SUCCESS;
}

wv_ipc_ret wave_ipc_msg_reserve_data(wv_ipc_msg *msg, size_t len)
{
	if (msg == NULL || len == 0)
		return WAVE_IPC_ERROR;

	if (len > WAVE_IPC_BUFF_SIZE)
		return WAVE_IPC_ERROR;

	if (msg->packet.info.data_len)
		return WAVE_IPC_ERROR;

	msg->packet.info.data_len = len;
	return WAVE_IPC_SUCCESS;
}

wv_ipc_ret wave_ipc_msg_shrink_data(wv_ipc_msg *msg, size_t len)
{
	if (msg == NULL)
		return WAVE_IPC_ERROR;

	if (msg->packet.info.data_len < len)
		return WAVE_IPC_ERROR;

	msg->packet.info.data_len = len;
	return WAVE_IPC_SUCCESS;
}

char* wave_ipc_msg_get_data(wv_ipc_msg *msg)
{
	if (msg == NULL ||
	    msg->packet.info.data_len == 0 ||
	    msg->packet.info.data_len > sizeof(msg->packet.data))
		return NULL;

	return msg->packet.data;
}

size_t wave_ipc_msg_get_size(wv_ipc_msg *msg)
{
	if (msg == NULL || msg->packet.info.data_len > sizeof(msg->packet.data))
		return 0;

	return msg->packet.info.data_len;
}

wv_ipc_ret wave_ipc_msg_push_hdr(wv_ipc_msg *msg, uint8_t* hdr, uint8_t len)
{
	size_t i = 0;

	if (msg == NULL || hdr == NULL || len == 0)
		return WAVE_IPC_ERROR;

	while (i < sizeof(msg->packet.info.hdr)) {
		if (msg->packet.info.hdr[i])
			i += msg->packet.info.hdr[i] + 1;
		else
			break;
	}

	if (i >= sizeof(msg->packet.info.hdr))
		return WAVE_IPC_ERROR;

	if (i + len + 1 > sizeof(msg->packet.info.hdr))
		return WAVE_IPC_ERROR;

	msg->packet.info.hdr[i++] = len;
	memcpy_s(&msg->packet.info.hdr[i], sizeof(msg->packet.info.hdr) - i, hdr, len);
	return WAVE_IPC_SUCCESS;
}

wv_ipc_ret wave_ipc_msg_pop_hdr(wv_ipc_msg *msg, uint8_t* hdr, uint8_t *len)
{
	uint8_t i = 0, j = sizeof(msg->packet.info.hdr);

	if (msg == NULL || hdr == NULL || len == NULL || *len == 0)
		return WAVE_IPC_ERROR;

	while (i < sizeof(msg->packet.info.hdr)) {
		if (msg->packet.info.hdr[i]) {
			j = i;
			i += msg->packet.info.hdr[i] + 1;
		} else
			break;
	}

	if (i == 0)
		return WAVE_IPC_ERROR;

	if (j >= sizeof(msg->packet.info.hdr) ||
	    j + msg->packet.info.hdr[j] >= sizeof(msg->packet.info.hdr)) {
		BUG("j=%d, i=%d msg->hdr[j]=%d", j, i, msg->packet.info.hdr[j]);
		return WAVE_IPC_ERROR;
	}

	if (msg->packet.info.hdr[j] > *len) {
		BUG("msg->hdr[j]=%d > *len=%d", msg->packet.info.hdr[j], *len);
		return WAVE_IPC_ERROR;
	}

	memcpy_s(hdr, *len, &msg->packet.info.hdr[j + 1], msg->packet.info.hdr[j]);
	*len = msg->packet.info.hdr[j];

	memset(&msg->packet.info.hdr[j], 0, *len + 1);
	return WAVE_IPC_SUCCESS;
}

wv_ipc_ret wave_ipc_send_msg(int socket, wv_ipc_msg *msg, int flags)
{
	ssize_t res;

	if (msg == NULL)
		return WAVE_IPC_ERROR;

	if (socket == -1)
		goto err;

	if (msg->packet.info.data_len > sizeof(msg->packet.data))
		goto err;

	res = send(socket, &msg->packet,
		   sizeof(msg->packet.info) + msg->packet.info.data_len,
		   MSG_NOSIGNAL | flags);
	if (res == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return WAVE_IPC_CMD_WOULD_BLOCK;

		ELOG("send() retuned error, errno = %d", errno);
		goto err;
	}

	return WAVE_IPC_SUCCESS;
err:
	return WAVE_IPC_ERROR;
}

wv_ipc_ret wave_ipc_recv_msg(int socket, wv_ipc_msg **out_msg)
{
	ssize_t recv_res;
	wv_ipc_msg *msg;
	wv_ipc_ret ret;

	if (socket == -1)
		return WAVE_IPC_DISCONNECTED;

	if (out_msg == NULL)
		return WAVE_IPC_ERROR;

	if ((msg = wave_ipc_msg_alloc()) == NULL)
		return WAVE_IPC_ERROR;

	ret = WAVE_IPC_ERROR;
	recv_res = recv(socket, &msg->packet.info, sizeof(msg->packet.info), 0);
	if (recv_res == -1) {
		ELOG("recv error, errno = %d", errno);
		goto err;
	} else if (recv_res == 0) {
		LOG(1, "disconnected gracefuly");
		ret = WAVE_IPC_DISCONNECTED;
		goto err;
	} else if ((size_t)recv_res < sizeof(msg->packet.info)) {
		ELOG("disconnecting client");
		goto err;
	} else if (msg->packet.info.data_len > sizeof(msg->packet.data)) {
		BUG("data len (%d) > max data size", msg->packet.info.data_len);
		goto err;
	}

	if (msg->packet.info.data_len) {
		recv_res = recv(socket, msg->packet.data, msg->packet.info.data_len, 0);
		if (recv_res == -1) {
			ELOG("recv error, errno = %d", errno);
			goto err;
		} else if (recv_res != msg->packet.info.data_len) {
			BUG("disconnecting client");
			goto err;
		}
	}

	*out_msg = msg;
	return WAVE_IPC_SUCCESS;

err:
	wave_ipc_msg_put(msg);
	return ret;
}
