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
 * Interface for FAPI to send / receive messages via Netlink socket
 *
 *
 */

#include "mtlkinc.h"
#include "nlmsgs.h"
#include "wave_fapi_nl_if.h"
#include "wave_fapi_if.h"
#include "mtlk_coreui.h"
#include "fw_recovery.h"

#define LOG_LOCAL_GID   GID_WAVE_FAPI
#define LOG_LOCAL_FID   1

int wave_fapi_msg_send (const uint8 subcmd_id, const uint8 card_idx,
                        const void *data, size_t data_size)
{
  int res;
  size_t buffer_len = sizeof(wave_fapi_msghdr_t) + data_size;
  void *buffer = mtlk_osal_mem_alloc(buffer_len, WAVE_MEM_TAG_FAPI_IF);

  if (!buffer)
    return MTLK_ERR_NO_MEM;

  /* Prepare message header */
  WAVE_FAPI_MSG_HDR_PUT(buffer, subcmd_id, card_idx);

  /* Copy message payload if any */
  if (data && data_size) {
    wave_memcpy((uint8*)buffer + sizeof(wave_fapi_msghdr_t), data_size, data, data_size);
  }

  /* Send message */
  res = mtlk_nl_send_brd_msg(buffer, buffer_len, GFP_ATOMIC, NETLINK_FAPI_GROUP, NL_DRV_CMD_FAPI_NOTIFY);

  if (MTLK_ERR_OK == res)
    ILOG1_DD("CID-%02x: Message sent to FAPI (subcmd_id=%02X)", card_idx, subcmd_id);
  else
    WLOG_DDD("CID-%02x: Unable to send message to FAPI (subcmd_id=%02X), res=%d", card_idx, subcmd_id, res);

  mtlk_osal_mem_free(buffer);

  return res;
}

int wave_prod_nl_send_msg_cal_file_evt (const uint8 card_idx)
{
  int res;

  /* Send message */
  res = wave_fapi_msg_send(WAVE_FAPI_SUBCMDID_PROD_CAL_FILE_EVT, card_idx, NULL, 0);

  if (MTLK_ERR_OK == res)
    ILOG1_D("CID-%02x: Calibration File event sent to FAPI", card_idx);
  else
    WLOG_DD("CID-%02x: Unable to send Calibration File event to FAPI (res=%d)", card_idx, res);

  return res;
}
