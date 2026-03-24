/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __WAVE_IPC_SERVER__H__
#define __WAVE_IPC_SERVER__H__

#include "wave_ipc.h"
#include "wave_ipc_core.h"
#include <stddef.h>

typedef struct _wv_ipserver wv_ipserver;
typedef struct _wv_ipstation wv_ipstation;

typedef struct _wv_ipserver_callbacks
{
	int (*cmd_async)(wv_ipserver *ipserv, wv_ipstation *ipsta,
			 uint8_t seq_num, wv_ipc_msg *cmd);
	int (*stop_cond)(wv_ipserver *ipserv);
	int (*adding_client)(wv_ipserver *ipserv, wv_ipstation *ipsta);
	int (*removing_client)(wv_ipserver *ipserv, wv_ipstation *ipsta);
} wv_ipserver_callbacks;

wv_ipc_ret wave_ipcs_create(wv_ipserver **handle_p, const char *server_name);
wv_ipc_ret wave_ipcs_delete(wv_ipserver **handle_p);

wv_ipc_ret wave_ipcs_run(wv_ipserver *handle, wv_ipserver_callbacks *callbacks);

wv_ipc_ret wave_ipcs_send_response_to(wv_ipserver *handle, wv_ipstation *ipsta,
				      uint8_t seq_num, wv_ipc_msg *reply,
				      uint8_t has_more);
wv_ipc_ret wave_ipcs_send_req_failed_to(wv_ipserver *handle, wv_ipstation *ipsta,
					uint8_t seq_num);

wv_ipc_ret wave_ipcs_send_cmd_to(wv_ipserver *handle, wv_ipstation *ipsta,
				 wv_ipc_msg *cmd, wv_ipc_msg **reply);

wv_ipc_ret wave_ipcs_send_event_all(wv_ipserver *handle, wv_ipc_msg *event);
wv_ipc_ret wave_ipcs_send_event_to(wv_ipserver *handle, wv_ipc_msg *event,
				   wv_ipstation *ipsta);

/* Send an event to a station. Unlike the wave_ipcs_send_event_to() and wave_ipcs_send_event_all(),
 * this function does't push message header by itself, and requires call of wave_ipcs_push_event_header()
 * before the call.
 * This function can be used if the same event should be sent to several stations. The example scenario:
 *
 * wave_ipcs_push_event_header(event);
 * wave_ipcs_send_to(server, event, sta1);
 * wave_ipcs_send_to(server, event, sta2);
 * ...
 */
wv_ipc_ret wave_ipcs_send_to(wv_ipserver *handle, wv_ipc_msg *event, wv_ipstation *ipsta);
wv_ipc_ret wave_ipcs_push_event_header(wv_ipc_msg *event);

wv_ipc_ret wave_ipcs_sta_incref(wv_ipstation *ipclient);
wv_ipc_ret wave_ipcs_sta_decref(wv_ipstation *ipclient);
const char* wave_ipcs_sta_name(wv_ipstation *ipsta);
void wave_ipcs_sta_set_data(wv_ipstation *ipsta, void *data);
void* wave_ipcs_sta_get_data(wv_ipstation *ipsta);

#endif /* __WAVE_IPC_SERVER__H__ */
