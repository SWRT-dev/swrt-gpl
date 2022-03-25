/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2015 Zhu YiXin<yixin.zhu@lantiq.com>
 * Copyright (C) 2016~2017 Intel Corporation.
 */

#ifndef __INTEL_UMT_CQEM__
#define __INTEL_UMT_CQEM__

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
	enum umt_status enable;
	u32 umt_ep_dst;
};

int intel_umt_cqem_set_period(u32 umt_id, u32 ep_id, u32 period);
int intel_umt_cqem_set_mode(u32 umt_id, u32 ep_id,
			    struct umt_set_mode *umt_mode);
int intel_umt_cqem_enable(u32 umt_id, u32 ep_id, u32 enable);
int intel_umt_cqem_request(u32 ep_id, u32 cbm_pid, u32 *dma_ctrlid,
			   u32 *dma_cid, u32 *umt_id);
int intel_umt_cqem_release(u32 umt_id, u32 ep_id);
int intel_umt_cqem_suspend(u32 umt_id, u32 ep_id, u32 enable);

#endif /* __INTEL_UMT_CQEM__ */
