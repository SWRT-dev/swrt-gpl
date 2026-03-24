/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __WAVE_IPC_CORE__H__
#define __WAVE_IPC_CORE__H__

#include "wave_ipc.h"

#include <stddef.h>
#include <stdint.h>

typedef struct _wv_ipc_msg wv_ipc_msg;

wv_ipc_msg* wave_ipc_msg_alloc(void);
wv_ipc_ret wave_ipc_multi_msg_append(wv_ipc_msg *head, wv_ipc_msg *msg);
int wave_ipc_msg_is_multi_msg(wv_ipc_msg *msg);
wv_ipc_msg * wave_ipc_multi_msg_get_next(wv_ipc_msg *msg);
wv_ipc_msg* wave_ipc_msg_dup(wv_ipc_msg *orig);
void wave_ipc_msg_put(wv_ipc_msg *msg);

wv_ipc_ret wave_ipc_msg_fill_data(wv_ipc_msg *msg, const char *data, size_t len);
wv_ipc_ret wave_ipc_msg_append_data(wv_ipc_msg *msg, const char *data, size_t len);
wv_ipc_ret wave_ipc_msg_reserve_data(wv_ipc_msg *msg, size_t len);
wv_ipc_ret wave_ipc_msg_shrink_data(wv_ipc_msg *msg, size_t len);
char* wave_ipc_msg_get_data(wv_ipc_msg *msg);
size_t wave_ipc_msg_get_size(wv_ipc_msg *msg);

wv_ipc_ret wave_ipc_msg_push_hdr(wv_ipc_msg *msg, uint8_t* hdr, uint8_t len);
wv_ipc_ret wave_ipc_msg_pop_hdr(wv_ipc_msg *msg, uint8_t* hdr, uint8_t *len);

wv_ipc_ret wave_ipc_send_msg(int socket, wv_ipc_msg *msg, int flags);
wv_ipc_ret wave_ipc_recv_msg(int socket, wv_ipc_msg **out_msg);

typedef struct __attribute__((__packed__)) {
	uint8_t magic;
	uint8_t header[4];
} ipc_header;

#define IPC_HDR_MAGIC	(0xA1)

static inline int ipc_header_push(wv_ipc_msg *ipc_msg, ipc_header *hdr)
{
	hdr->magic = IPC_HDR_MAGIC;
	if (WAVE_IPC_SUCCESS !=
	    wave_ipc_msg_push_hdr(ipc_msg, (uint8_t*)hdr, sizeof(ipc_header)))
		return 1;
	return 0;
}

static inline int ipc_header_pop(wv_ipc_msg *ipc_msg, ipc_header *hdr)
{
	uint8_t size = sizeof(ipc_header);
	if (WAVE_IPC_SUCCESS !=
	    wave_ipc_msg_pop_hdr(ipc_msg, (uint8_t*)hdr, &size))
		return 1;
	if (size != sizeof(ipc_header) || hdr->magic != IPC_HDR_MAGIC)
		return 1;
	return 0;
}

static inline void wv_aligned_16_bit_assign(uint8_t *buff, uint16_t num)
{
    *buff = num & (uint16_t)0xFF;
    *(buff + 1) = (num >> 8) & (uint16_t)0xFF;
}

static inline uint16_t wv_aligned_16_bit_fetch(uint8_t *buff)
{
    return (uint16_t)*buff | ((uint16_t)*(buff + 1) << 8);
}

#endif /* __WAVE_IPC_CORE__H__ */
