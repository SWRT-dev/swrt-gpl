/*
 * pp_port_mgr.c
 * Description: PP Port Manager definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */

#ifndef __PP_PORT_MGR_H__
#define __PP_PORT_MGR_H__

#include <linux/types.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/pp_api.h>

/**
 * @brief initial configuration parameters
 * @dbgfs debugfs parent folder
 */
struct pp_pmgr_init_param {
	struct dentry  *dbgfs;
	struct kobject *sysfs;
};

#ifdef CONFIG_PPV4_LGM

/**
 * @define host interface number 0 is statically assigned to
 *         default entry
 */
#define PMGR_HIF_DFLT_DP_IDX 0

/**
 * @define host interface number 1 is statically assigned to
 *         protected dp
 */
#define PMGR_HIF_PROTECTED_DP_IDX 1

/**
 * @struct module statistics (for debug)
 */
struct pmgr_stats {
	u32 null_err;
	u32 hal_err;
	u32 port_count;
	u32 port_invalid_err;
	u32 port_invalid_headroom;
	u32 port_act_err;
	u32 port_inact_err;
	u32 port_update_no_changes_err;
	u32 port_cls_param_err;
	u32 port_policy_param_err;
	u32 port_tx_cfg_err;
	u32 port_rx_cfg_err;
	u32 hif_dps_count;
	u32 hif_dp_param_err;
	u32 hif_cls_param_err;
	u32 hif_dp_port_err;
};

/**
 * @brief adding new pp port
 * @param pid port number
 * @param cfg port configuration
 * @param dbg_en if true, create debugfs file
 * @return s32 return 0 for success
 */
s32 pmgr_port_add(u16 pid, struct pp_port_cfg *cfg, bool dbg_en);

/**
 * @brief deleting pp port
 * @param pid port number
 * @param dbg_en if true, delete debugfs file
 * @return s32 return 0 for success
 */
s32 pmgr_port_del(u16 pid, bool dbg_en);

/**
 * @brief adding new pp host interface
 * @param hif host interface configuration
 * @param dbg_en if true, create debugfs file
 * @return s32 return 0 for success
 */
s32 pmgr_hostif_add(struct pp_hostif_cfg *hif, bool dbg_en);

/**
 * @brief update pp host interface
 * @param hif current host interface configuration
 * @param new_dp new datapath to set
 * @param dbg_en if true, delete debugfs file
 * @return s32 return 0 for success
 */
s32 pmgr_hostif_update(struct pp_hostif_cfg *hif,
		       struct pp_hif_datapath *new_dp, bool dbg_en);

/**
 * @brief deleting pp host interface
 * @param hif host interface configuration
 * @param dbg_en if true, delete debugfs file
 * @return s32 return 0 for success
 */
s32 pmgr_hostif_del(struct pp_hostif_cfg *hif, bool dbg_en);

/**
 * @brief set default pp host interface
 * @param dp host interface datapath
 * @param dbg_en if true, create debugfs file
 * @return s32 return 0 for success
 */
s32 pmgr_hostif_dflt_set(struct pp_hif_datapath *dp, bool dbg_en);

/**
 * @brief get port tx policies info
 * @param pid pp port index (GPID)
 * @param base_policy buffer to save base policy
 * @param policies_map bitmap buffer to save the enable policies
 *                     bitmap
 * @return s32 0 on success, error code otherwise
 */
s32 pp_port_tx_policy_info_get(u16 pid, u8 *base_policy, u8 *policies_map);

/**
 * @brief get port's max packet size
 * @param pid pp port index (GPID)
 * @param max_size buffer to save the max packet size
 * @return s32 0 on success, error code otherwise
 */
s32 pp_port_tx_max_pkt_size_get(u16 pid, u16 *max_size);

/**
 * @brief get port rx policies info
 * @param pid pp port index (GPID)
 * @param policies_map bitmap buffer to save the enable policies
 *                     bitmap
 * @return s32 0 on success, error code otherwise
 */
s32 pp_port_rx_policy_info_get(u16 pid, u8 *policies_map);

/**
 * @brief Get port's segmentation enable status
 * @param pid the port
 * @param seg_en pointer to store the status
 * @return s32 0 on success, error code otherwise
 */
s32 pp_port_seg_en_get(u16 pid, bool *seg_en);

/**
 * @brief Get port's prel2 enable status
 * @param pid the port
 * @param prel2_en pointer to store the status
 * @return s32 0 on success, error code otherwise
 */
s32 pp_port_prel2_en_get(u16 pid, bool *prel2_en);

/**
 * @brief get port classification info
 * @param pid pp port id (GPID)
 * @param cls buffer to save classification info
 * @return s32 0 on success, error code otherwise
 */
s32 pp_port_cls_info_get(u16 pid, struct pp_port_cls_info *cls);

/**
 * @brief get port buffers headroom size
 * @param pid pp port id (GPID)
 * @param headroom buffers headroom size
 * @return s32 0 on success, error code otherwise
 */
s32 pp_port_headroom_get(u16 pid, u16 *headroom);

/**
 * @brief Check if port is active
 * @param pid pp port index (GPID)
 * @return bool true if port active, false otherwise
 */
bool pmgr_port_is_active(u16 pid);

/**
 * @brief get the module statistics from database
 * @param stats port manager module statistics
 * @return s32 0 for success, non-zero otherwise
 */
s32 pmgr_stats_get(struct pmgr_stats *stats);

/**
 * @brief Reset port manager stats
 * @return s32 0 for success, non-zero otherwise
 */
s32 pmgr_stats_reset(void);

/**
 * @brief initialized PP port manager driver
 * @param init_param initial parameters
 * @return s32 0 on success
 */
s32 pmgr_init(struct pp_pmgr_init_param *init_param);

/**
 * @brief exit PP port manager driver
 * @param void
 */
void pmgr_exit(void);
#else
static inline s32 pmgr_init(struct pp_pmgr_init_param *init_param)
{
	return 0;
}

static inline void pmgr_exit(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif /* __PP_PORT_MGR_H__ */
