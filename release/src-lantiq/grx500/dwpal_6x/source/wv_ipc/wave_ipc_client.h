/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __WAVE_IPC_CLIENT__H__
#define __WAVE_IPC_CLIENT__H__

#include "wave_ipc.h"
#include "wave_ipc_core.h"
#include <stddef.h>

typedef struct _wv_ipclient wv_ipclient;

#define WAVE_IPC_EVENT_TAKE_OWNERSHIP (0x8000)
#define WAVE_IPC_EVENT_SUCCESS        (0)
#define WAVE_IPC_EVENT_ERROR          (1)

/* Event handler for asynchronus events.
 * Function should return WAVE_IPC_EVENT_TAKE_OWNERSHIP if it takes onwership on event.
 * In a such case function must release event by itself.
 * This feature can be used if event handler is started in a new thread.
 * This feature can not be used if 'use_events_thread' is turned on.
 * Otherwise function may return WAVE_IPC_EVENT_SUCCESS (0) or WAVE_IPC_EVENT_ERROR (1)
 */
typedef int (*wv_ipc_event)(void *arg, wv_ipc_msg *event);
typedef int (*wv_ipc_cmd)(void *arg, uint8_t seq_num, wv_ipc_msg *cmd);
typedef int (*wv_ipc_disconnect)(void *arg);
typedef int (*wv_ipc_reconnect)(void *arg);
typedef int (*wv_ipc_terminate_cond)(void *arg);

wv_ipc_ret wave_ipcc_connect(wv_ipclient **handle_p,
			     const char *prog_name, const char *server_name);

wv_ipc_ret wave_ipcc_disconnect(wv_ipclient **handle_p);

wv_ipc_ret wave_ipcc_start_listener(wv_ipclient *handle,
				    wv_ipc_event event_clb,
				    wv_ipc_cmd command_clb,
				    wv_ipc_disconnect disconnect_clb,
				    wv_ipc_reconnect reconnect_clb,
				    void *clb_arg, int use_events_thread);

wv_ipc_ret wave_ipcc_stop_listener(wv_ipclient *handle);

wv_ipc_ret wave_ipcc_send_cmd(wv_ipclient *handle,
			      wv_ipc_msg *cmd, wv_ipc_msg **reply);

wv_ipc_ret wave_ipcc_send_response(wv_ipclient *handle, uint8_t seq_num,
				   wv_ipc_msg *reply, uint8_t has_more);

wv_ipc_ret wave_ipcc_send_req_failed(wv_ipclient *ipclient, uint8_t seq_num);

wv_ipc_ret wave_ipcc_blocked_event_listener(wv_ipclient *handle,
					    wv_ipc_event event_clb,
					    wv_ipc_cmd command_clb,
					    wv_ipc_disconnect disconnect_clb,
					    wv_ipc_reconnect reconnect_clb,
					    wv_ipc_terminate_cond term_cond,
					    void *clb_arg);

#endif /* __WAVE_IPC_CLIENT__H__ */
