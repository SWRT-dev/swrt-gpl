/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2015 Zhu YiXin<yixin.zhu@lantiq.com>
 * Copyright (C) 2016 Intel Corporation.
 */

#ifndef __LTQ_HWMCPY_H__
#define __LTQ_HWMCPY_H__

enum {
	HWMCPY_F_PRIO_LOW	= 0x0,
	HWMCPY_F_PRIO_HIGH	= 0x1,
	HWMCPY_F_IPC		= 0x2,
	HWMCPY_F_CHKSZ_1	= 0x4,
	HWMCPY_F_CHKSZ_2	= 0x8,
	HWMCPY_F_CHKSZ_3	= 0xC,
	HWMCPY_F_CHKSZ_4	= 0x10,
/*
 * Trunk size support from 0 to 0x7(512B,1KB to 64KB),
 * start from bit 2, len 3 bits, max 0x1C
 */
	HWMCPY_F_RESERVED	= 0x20,
	HWMCPY_F_CHKSZ_SET	= 0x40,
	HWMCPY_F_LAST		= 0x80,

};

enum mcpy_type {
	MCPY_PHY_TO_PHY = 0,
	MCPY_PHY_TO_IOCU,
	MCPY_IOCU_TO_PHY,
	MCPY_IOCU_TO_IOCU,
	MCPY_SW_CPY,
};

struct mcpy_frag {
	void *ptr;
	u16 offset;
	u16 size;
};

enum umt_mode {
	UMT_SELFCNT_MODE = 0,
	UMT_USER_MODE    = 1,
	UMT_MODE_MAX,
};

enum umt_msg_mode {
	UMT_NO_MSG    = 0,
	UMT_MSG0_ONLY = 1,
	UMT_MSG1_ONLY = 2,
	UMT_MSG0_MSG1 = 3,
	UMT_MSG_MAX,
};

enum umt_status {
	UMT_DISABLE = 0,
	UMT_ENABLE  = 1,
	UMT_STATUS_MAX,
	UMT_BROKEN,
};

struct umt_set_mode {
	enum umt_mode umt_mode;
	enum umt_msg_mode msg_mode;
	u32 phy_dst;
	u32 period;
	u32 msg1_period;
	enum umt_status enable;
	u32 umt_ep_dst;
};

extern void *ltq_hwmemcpy(void *dst, const void *src, u32 len,
			  u32 portid, enum mcpy_type mode, u32 flags);
extern int ltq_hwmcpy_sg(void *dst, const struct mcpy_frag *src, u32 frag_num,
			 u32 portid, enum mcpy_type mode, u32 flags);
extern int ltq_mcpy_reserve(void);
extern void ltq_mcpy_release(u32 pid);

int ltq_umt_set_period(u32 umt_id, u32 ep_id,
		       enum umt_msg_mode msg_mode, u32 period);
int ltq_umt_set_mode(u32 umt_id, u32 ep_id, struct umt_set_mode *umt_mode);
int ltq_umt_enable(u32 umt_id, u32 ep_id, u32 enable);
int ltq_umt_request(u32 ep_id, u32 cbm_pid,
		    u32 *dma_ctrlid, u32 *dma_cid, u32 *umt_id);
int ltq_umt_release(u32 umt_id, u32 ep_id);
int ltq_umt_suspend(u32 umt_id, u32 ep_id, u32 enable);
#ifdef CONFIG_LTQ_UMT_SW_MODE
int umt_reset_port_dq_idx(u32 cbm_id);
#endif

#endif /* __LTQ_HWMCPY_H__ */
