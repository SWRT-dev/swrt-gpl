/*
 ***************************************************************************
 * MediaTek Inc.
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************

	Module Name: wifi_offload
	warp.h
*/

#ifndef _WARP_BM_H
#define _WARP_BM_H

#include "warp_wifi.h"

struct warp_bm_rxdmad {
	__le32 sdp0;
	__le32 token;
} __packed __aligned(4);

enum thrd_operation {
	THRD_INC_ALL = 0,
	THRD_DEC_ALL,
	THRD_INC_L,
	THRD_INC_H,
	THRD_DEC_L,
	THRD_DEC_H,
	THRD_OP_MAX
};

int regist_dl_dybm_task(struct wed_entry *wed);
int unregist_dl_dybm_task(struct wed_entry *wed);
int regist_ul_dybm_task(struct wed_entry *wed);
int unregist_ul_dybm_task(struct wed_entry *wed);

void buf_free_task(unsigned long data);
void buf_alloc_task(unsigned long data);

void dump_pkt_info(struct wed_buf_res *res);

int wed_txbm_init(struct wed_entry *wed, struct wifi_hw *hw);
void wed_txbm_exit(struct wed_entry *wed);

int dybm_dl_int_disp(struct wed_entry *wed, u32 status);

void rxbm_recycle_handler(unsigned long data);
void rxbm_alloc_handler(unsigned long data);

int wed_rx_bm_init(struct wed_entry *wed, struct wifi_hw *hw);
void wed_rx_bm_exit(struct wed_entry *wed);

int dybm_ul_int_disp(struct wed_entry *wed, u32 status);

#endif /*_WARP_BM_H*/
