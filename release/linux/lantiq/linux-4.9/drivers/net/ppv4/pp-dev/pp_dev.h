/*
 * pp_dev.h
 * Description: PP Device Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#ifndef __PP_DEV_H__
#define __PP_DEV_H__
#include <linux/platform_device.h>
#include "infra.h"
#include "bm.h"
#include "port_dist.h"
#include "pp_session_mgr.h"
#include "pp_port_mgr.h"
#include "pp_buffer_mgr.h"
#include "pp_qos_utils.h"
#include "pp_misc.h"
#include "parser.h"
#include "rpb.h"
#include "classifier.h"
#include "checker.h"
#include "modifier.h"
#include "rx_dma.h"
#include "uc.h"

struct pp_ext_clk {
	bool       valid;
	struct clk *ppv4_freq_clk;
	struct clk *ppv4_gate_clk;
};

/**
 * @brief dts config structure
 */
struct pp_dts_cfg {
	struct pp_pmgr_init_param    pmgr_params;
	struct pp_smgr_init_param    smgr_params;
	struct pp_bmgr_init_param    bmgr_params;
	struct pp_qos_init_param     qos_params;
	struct infra_init_param      infra_params;
	struct prsr_init_param       parser_params;
	struct rpb_init_param        rpb_params;
	struct port_dist_init_param  port_dist_params;
	struct pp_cls_init_param     cls_param;
	struct pp_chk_init_param     chk_param;
	struct pp_mod_init_param     mod_param;
	struct pp_rx_dma_init_params rx_dma_params;
	struct pp_bm_init_param      bm_params;
	struct uc_init_params        uc_params;
	struct pp_misc_init_param    misc_params;
};

s32 pp_dts_cfg_get(struct platform_device *pdev, struct pp_dts_cfg *cfg);
void pp_dts_cfg_dump(struct pp_dts_cfg *cfg);
void pp_fpga_cfg_get(struct pp_dts_cfg *cfg);

#endif /* __PP_DEV_H__ */
