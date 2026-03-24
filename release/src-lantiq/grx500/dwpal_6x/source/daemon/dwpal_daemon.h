/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/


#ifndef __WAVE_DWPAL_DAEMON__H__
#define __WAVE_DWPAL_DAEMON__H__

#include "wave_ipc_core.h"
#include "logs.h"
#include "dwpal.h"

#define DWPALD_SERVER_NAME "dwpald"
#ifdef CONFIG_DWPALD_DEBUG_TOOLS
#define DWPALD_SERVER_UTEST_NAME "dwpald_unitest"
#endif

#define DWPALD_NL_DRV_IFNAME "nl_drv"

#define DWPALD_CMD			(1)
#define DWPALD_CMD_RESP			(2)
#define DWPALD_EVENT			(3)
#define DWPALD_DETACH_REQ		(4)
#define DWPALD_DETACH_RESP		(5)
#define DWPALD_ATTACH_REQ		(6)
#define DWPALD_ATTACH_RESP		(7)
#define DWPALD_UPDATE_EVENT_REQ		(8)
#define DWPALD_UPDATE_EVENT_RESP		(9)

#ifdef CONFIG_DWPALD_DEBUG_TOOLS
#define DWPALD_TERMINATE_REQ		(10)
#define DWPALD_TERMINATE_RESP		(11)
#define DWPALD_CONNECTED_CLIENTS_REQ	(12)
#define DWPALD_CONNECTED_CLIENTS_RESP	(13)
#endif

#define DWPALD_IF_TYPE_HOSTAP		(1)
#define DWPALD_IF_TYPE_DRIVER		(2)
#define DWPALD_IF_TYPE_KERNEL		(3)

/* dwpald Message headers:
 *
 * under ipc command:
 * [DWPALD_REG_EVENTS] [DWPALD_IF_TYPE_HOSTAP]
 * [DWPALD_REG_EVENTS] [DWPALD_IF_TYPE_DRIVER]
 * [DWPALD_CMD] [DWPALD_IF_TYPE_HOSTAP] [ifname_len]
 * [DWPALD_CMD] [DWPALD_IF_TYPE_DRIVER] [ifname_len] [has_response]
 * [DWPALD_CMD] [DWPALD_IF_TYPE_KERNEL] [ifname_len] [has_response]
 *
 * under ipc event:
 * [DWPALD_EVENT] [DWPALD_IF_TYPE_HOSTAP] [ifname_len] [op_code_len]
 * [DWPALD_EVENT] [DWPALD_IF_TYPE_DRIVER] [ifname_len]
 * [DWPALD_EVENT] [DWPALD_IF_TYPE_KERNEL]
 *
 * under ipc response:
 * [DWPALD_REG_EVENTS_STATUS] [ failed_flag ]
 * [DWPALD_CMD_RESP] [DWPALD_IF_TYPE_HOSTAP] [dwpal_ext_ret]
 * [DWPALD_CMD_RESP] [DWPALD_IF_TYPE_DRIVER] [dwpal_ext_ret]
 * [DWPALD_CMD_RESP] [DWPALD_IF_TYPE_KERNEL] [dwpal_ext_ret]
 *
 *
 */

#define INTERFACE_DWPAL_STATE_UNKNOWN		(0)
#define INTERFACE_DWPAL_STATE_CONNECTED		(1)
#define INTERFACE_DWPAL_STATE_DISCONNECTED	(2)

typedef struct __attribute__((__packed__)) {
	uint8_t magic;
	uint8_t header[6];
} dwpald_header;

#define DWPALD_HDR_MAGIC	(0x55)

static inline int dwpald_header_push(wv_ipc_msg *ipc_msg, dwpald_header *hdr)
{
	hdr->magic = DWPALD_HDR_MAGIC;

	if (WAVE_IPC_SUCCESS !=
	    wave_ipc_msg_push_hdr(ipc_msg, (uint8_t*)hdr, sizeof(dwpald_header))) {
		BUG("ipc msg push header failed");
		return 1;
	}

	return 0;
}

static inline int dwpald_header_pop(wv_ipc_msg *ipc_msg, dwpald_header *hdr)
{
	uint8_t size = sizeof(dwpald_header);

	if (WAVE_IPC_SUCCESS !=
	    wave_ipc_msg_pop_hdr(ipc_msg, (uint8_t*)hdr, &size)) {
		BUG("ipc msg pop header failed");
		return 1;
	}

	if (size != sizeof(dwpald_header) || hdr->magic != DWPALD_HDR_MAGIC) {
		BUG("size = %hhu, magic = %hhu", size, hdr->magic);
		return 1;
	}

	return 0;
}

static inline int dwpald_header_peek(wv_ipc_msg *ipc_msg, dwpald_header *hdr)
{
	if (dwpald_header_pop(ipc_msg, hdr)) {
		BUG("ipc msg pop header failed");
		return 1;
	}

	if (dwpald_header_push(ipc_msg, hdr)) {
		BUG("ipc msg push header failed");
		return 1;
	}

	return 0;
}

static inline struct nl_msg * dwpald_nl_msg_from_ipc_msg(wv_ipc_msg *ipc_msg)
{
	struct nlmsghdr *hdr = (struct nlmsghdr *)wave_ipc_msg_get_data(ipc_msg);
	struct nl_msg *nlmsg;

	if (hdr == NULL) {
		BUG("data is NULL in side the ipc msg");
		return NULL;
	}

	nlmsg = nlmsg_convert(hdr);
	return nlmsg;
}

static inline wv_ipc_msg * dwpald_ipc_msg_from_nl_msg(struct nl_msg *nlmsg)
{
	struct nlmsghdr *hdr = nlmsg_hdr(nlmsg);
	wv_ipc_msg *ipc_msg = wave_ipc_msg_alloc();

	wave_ipc_msg_fill_data(ipc_msg, (char *)hdr, nlmsg_total_size(nlmsg_datalen(hdr)));
	return ipc_msg;
}

#endif /* __WAVE_DWPAL_DAEMON__H__ */
