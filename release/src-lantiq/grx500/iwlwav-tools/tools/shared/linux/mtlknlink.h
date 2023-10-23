/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_NLINK_H__
#define __MTLK_NLINK_H__

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"
#include "nl.h"

typedef void (*mtlk_netlink_callback_t)(void* ctx, void* data);

#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#ifdef CPTCFG_IWLWAV_USE_LIBNL_NG
#define nl_handle                 nl_sock
#define nl_handle_alloc           nl_socket_alloc
#define nl_handle_destroy         nl_socket_free
#define nl_disable_sequence_check nl_socket_disable_seq_check
#endif

typedef struct _mtlk_nlink_socket_t
{
  int family;
  struct nl_handle *sock;
  mtlk_netlink_callback_t receive_callback;
  void* receive_callback_ctx;
} mtlk_nlink_socket_t;

int __MTLK_IFUNC
mtlk_nlink_create(mtlk_nlink_socket_t* nlink_socket, const char* group_name,
                  mtlk_netlink_callback_t receive_callback, void* callback_ctx);

int wave_nlink_send_brd_msg(mtlk_nlink_socket_t *nlink_socket, const void *data, uint16 data_len, uint8 cmd);

int __MTLK_IFUNC
mtlk_nlink_receive_loop(mtlk_nlink_socket_t* nlink_socket, int stop_fd);

void __MTLK_IFUNC
mtlk_nlink_cleanup(mtlk_nlink_socket_t* nlink_socket);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_NLINK_H__ */
