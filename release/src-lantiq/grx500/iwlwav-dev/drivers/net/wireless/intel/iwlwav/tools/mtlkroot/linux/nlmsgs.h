/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * $Id$
 *
 */
/*! \file   nlmsgs.h
 *  \ brief Subsystem providing communication with userspace over
 *          NETLINK_USERSOCK netlink protocol.
 */

#ifndef __NLMSGS_H__
#define __NLMSGS_H__

#include "nl.h"

#define LOG_LOCAL_GID   GID_NLMSGS
#define LOG_LOCAL_FID   0

/*! Prototype of RX callback function */
typedef void (*wave_nl_callback_t)(void* ctx, void* data);

/*! Configuration structure */
typedef struct
{
  BOOL is_active;                                             /*!< if TRUE, then Netlink module is active */
  wave_nl_callback_t receive_callback[WAVE_NL_DRV_CMD_COUNT]; /*!< pointer to RX callback function */
  void* receive_callback_ctx[WAVE_NL_DRV_CMD_COUNT];          /*!< parameters for RX callback function */
} wave_nl_socket_t;

/*! \brief      This function does the following:
 *                - (1) Initializing Netlink module
 *                - (2) Register a generic netlink family
 *                - (3) Register RX callback to handle a command
 *
 *  \param[in]  rx_clb_fn         Pointer to RX callback function
 *  \param[in]  rx_clb_fn_params  Pointer to parameters for the callback
 *  \param[in]  cmd               Command ID to handle
 *
 *  \return     MTLK_ERR_OK on success, an error code on failure
*/
mtlk_error_t mtlk_nl_init(wave_nl_callback_t rx_clb_fn, void *rx_clb_fn_params, u8 cmd);

/*! \brief      Register RX callback to handle a command
 *              This function does the same as point (3) of the \c mtlk_nl_init()
 *
 *  \param[in]  rx_clb_fn         Pointer to RX callback function
 *  \param[in]  rx_clb_fn_params  Pointer to parameters for the callback
 *  \param[in]  cmd               Command ID to handle
 *
 *  \return     MTLK_ERR_OK on success, an error code on failure
*/
mtlk_error_t wave_nl_update_receive_callback(wave_nl_callback_t rx_clb_fn, void *rx_clb_fn_params, u8 cmd);

/*! Unregister a generic netlink family and cleanup Netlink module
 *
 *  \return     none
 */
void mtlk_nl_cleanup(void);

/*! Allocate and send a new generic netlink message
 *
 *  \param[in]  data              data of the message payload
 *  \param[in]  length            size of the message payload in bytes
 *  \param[in]  flags             the type of memory to allocate
 *  \param[in]  dst_group         multicast group
 *  \param[in]  cmd               Command ID received
 *
 *  \return     MTLK_ERR_OK on success, an error code on failure
 */
mtlk_error_t mtlk_nl_send_brd_msg(void *data, int length, gfp_t flags, u32 dst_group, u8 cmd);

/*! Activity status of the Netlink module
 *
 * \return      TRUE if the Netlink module is active, FALSE if not
 */
BOOL wave_nl_is_active(void);


#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif /* __NLMSGS_H__ */
