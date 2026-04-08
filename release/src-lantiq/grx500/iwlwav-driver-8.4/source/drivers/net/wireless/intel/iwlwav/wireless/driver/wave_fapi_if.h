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
#ifndef __WAVE_FAPI_IF_H__
#define __WAVE_FAPI_IF_H__

/* Payload of any FAPI message */
typedef struct {
  uint8 subcmd_id;
  uint8 card_idx;
  uint8 payload;
} wave_fapi_msgpay_common_t;

/* Sub-command IDs for FAPI messages */
enum {
  WAVE_FAPI_SUBCMDID_RESERVED,                  /* Reserved (not used) */
  WAVE_FAPI_SUBCMDID_PROD_CAL_FILE_EVT,         /* Production Calibration File Event */
  WAVE_FAPI_SUBCMDID_LAST                       /* Last (not used) */
};

int wave_fapi_msg_send(const uint8 subcmd_id, const uint8 card_idx,
                       const void *data, size_t data_size);
void wave_fapi_msg_receive_cb(void *param, void *packet);

int wave_prod_nl_send_msg_cal_file_evt(const uint8 card_idx);

#endif /* __WAVE_FAPI_IF_H__ */
