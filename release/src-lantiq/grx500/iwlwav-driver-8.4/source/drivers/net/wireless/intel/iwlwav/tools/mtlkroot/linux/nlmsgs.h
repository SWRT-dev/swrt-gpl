/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 * $Id$
 *
 * 
 *
 * Subsystem providing communication with userspace over
 * NETLINK_USERSOCK netlink protocol.
 *
 */

#ifndef __NLMSGS_H__
#define __NLMSGS_H__

#include "nl.h"

#define LOG_LOCAL_GID   GID_NLMSGS
#define LOG_LOCAL_FID   0

/* prototype of RX callback function */
typedef void (*wave_nl_callback_t)(void* ctx, void* data);
/* configuration structure */
typedef struct
{
  BOOL is_active;                                             /* if TRUE, then Netlink module is active */
  wave_nl_callback_t receive_callback[WAVE_NL_DRV_CMD_COUNT]; /* pointer to RX callback function */
  void* receive_callback_ctx[WAVE_NL_DRV_CMD_COUNT];          /* parameters for RX callback function */
} wave_nl_socket_t;

int mtlk_nl_init(wave_nl_callback_t rx_clb_fn, void *rx_clb_fn_params, u8 cmd);
typedef void (*mtlk_nl_callback_t)(void* ctx, void* data);

typedef struct _mtlk_nl_socket_t
{
  mtlk_nl_callback_t receive_callback;
  void* receive_callback_ctx;
} mtlk_nl_socket_t;

void mtlk_nl_cleanup(void);
int mtlk_nl_send_brd_msg(void *data, int length, gfp_t flags, u32 dst_group, u8 cmd);
int wave_nl_update_receive_callback(wave_nl_callback_t rx_clb_fn, void *rx_clb_fn_params, u8 cmd);
BOOL wave_nl_is_active(void);

int mtlk_nl_bt_acs_init(void);
void mtlk_nl_bt_acs_cleanup(void);
int mtlk_nl_bt_acs_send_brd_msg(void *data, int length);


#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif /* __NLMSGS_H__ */
