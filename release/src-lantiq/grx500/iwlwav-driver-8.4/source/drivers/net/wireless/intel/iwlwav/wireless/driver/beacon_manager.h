/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 * $Id$
 *
 *
 *
 * Beacon manager interface
 *
 */
#ifndef __BEACON_MANAGER_H__
#define __BEACON_MANAGER_H__

#include "mac80211.h"

#define MTLK_BEACON_MAN_RCVRY_TMPL_IDX 2
#define MTLK_BEACON_MAN_RCVRY_TMPL_NUM (MTLK_BEACON_MAN_RCVRY_TMPL_IDX + 1)

/* beacon template */
typedef struct
{
  uint8          *ptr;
  uint32     dma_addr;
  uint16 alloced_size;
  uint16     head_len;
  uint16     tail_len;
} beacon_template_t;

/* beacon manager private structure per radio module */
typedef struct {
  beacon_template_t                   tmpl;   /* emergency buffer */
  BOOL                          fw_is_busy;   /* indicates if fw is busy with previous buffer */
  uint8                        for_vap_idx;   /* vap index for emergency buffer */
  mtlk_osal_event_t rcvry_beacon_ind_event;   /* rcvry case: event to wait beacon template set finish ind from FW */
} beacon_manager_priv_t;

/* beacon manager structure per VAP */
typedef struct {
  beacon_template_t     tmpl[MTLK_BEACON_MAN_RCVRY_TMPL_NUM];   /* ping-pong buffer + last saved template for recovery*/
  int                                                drv_idx;   /* index for driver's owned buffer */
  BOOL                                        switch_pending;   /* indicates buffers switch in fw */
  BOOL                                               updated;   /* indicates updated template in driver */
} beacon_manager_t;

int  wave_beacon_man_private_init(beacon_manager_priv_t *bmgr_priv_ctx);
void wave_beacon_man_private_cleanup(beacon_manager_priv_t *bmgr_priv_ctx, void *hw);
int  wave_beacon_man_init(beacon_manager_t *bmgr, mtlk_vap_handle_t vap_handle);
void wave_beacon_man_cleanup(beacon_manager_t *bmgr, mtlk_vap_handle_t vap_handle);
int  wave_beacon_man_beacon_set(mtlk_handle_t hcore, const void *data, uint32 data_size);
int  wave_beacon_man_template_ind_handle(mtlk_handle_t object, const void *data, unsigned data_size);
int  wave_beacon_man_rcvry_reset(mtlk_core_t *core);
int  wave_beacon_man_rcvry_beacon_set(mtlk_core_t *core);
void wave_beacon_man_rcvry_template_ind_handle(mtlk_core_t *core);
int wave_beacon_template_get_beacon_data(struct sk_buff *beacon, struct sk_buff *probe_resp,
    struct cfg80211_beacon_data *beacon_data, struct ieee80211_mutable_offsets *offs);
int  wave_beacon_man_set_unsolicited_frame_tx(mtlk_handle_t hcore, const void *data, uint32 data_size);

#endif /*__BEACON_MANAGER_H__*/
