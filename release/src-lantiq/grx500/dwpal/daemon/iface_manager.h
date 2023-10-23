/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __WAVE_IFACE_MANAGER__H__
#define __WAVE_IFACE_MANAGER__H__

#include "wave_ipc_server.h"
#include "wave_ipc_core.h"
#include "stadb.h"

#include <stdint.h>

typedef struct _iface_manager iface_manager;

typedef struct _manager_apis {
  int (*execute_command)(wv_ipserver *ipserv, wv_ipc_msg *cmd, wv_ipstation *ipsta, uint8_t seq_num);
  int (*iface_attach)(iface_manager *manager, char *ifname, uint8_t *state);
  int (*iface_detach)(char *ifname);
  int (*register_sta_to_events)(l_list *events, wv_ipstation *ipsta, const char *reg_str, size_t len);
  int (*unregister_sta_from_events)(l_list *events, wv_ipstation *ipsta);
  int (*send_event)(wv_ipserver *ipserv, char *ifname, wv_ipc_msg *event, void *info, l_list *events, uint8_t *state);
} manager_apis;

iface_manager * iface_manager_init(wv_ipserver *ipserver, manager_apis *man_apis,
				   l_list *seed_ifaces, uint8_t iftype,
				   unsigned int detach_time);

int iface_manager_deinit(iface_manager *manager);

int iface_manager_sta_cmd_async(iface_manager *manager, wv_ipstation *ipsta,
				uint8_t seq_num, wv_ipc_msg *cmd);

int iface_manager_event_received(iface_manager *manager, wv_ipc_msg *event,
				 const char *ifname, size_t ifnamsiz, void *info);

int iface_manager_sta_disconnected(iface_manager *manager, wv_ipstation *ipsta);

#endif /* __WAVE_IFACE_MANAGER__H__ */
