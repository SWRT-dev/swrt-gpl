/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#ifndef _GSW_DEFCONF_
#define _GSW_DEFCONF_

#define NOT_USED -1
#define X NOT_USED

enum _logical_pid {
	LOG_0 = 0,
	LOG_1,
	LOG_2,
	LOG_3,
	LOG_4,
	LOG_5,
	LOG_6,
	LOG_7,
	LOG_8,
	LOG_9,
	LOG_10,
	LOG_11
};

enum _dpu {
	DPU = 0,
	NON_DPU = 1,
};

struct _gsw_pce_path {
	u32 eg_lpid;	/* Egress Logical PID after Bridge */
	int ext;	/* Extraction */
	u32 tc_from;	/* Traffic Class From */
	u32 tc_to;	/* Traffic Class to */
	u32 qid;	/* Egress QID From */
	u32 redir_lpid;	/* Redirect Logical PID */
};

struct _gsw_bypass_path {
	u32 eg_pid;	/* Egress MAC PID */
	int ext;	/* Extraction */
	u32 dst_subifid;/* Dest subifid */
	u32 qid;	/* QID */
	u32 redir_pid;	/* Redirect Logical PID */
};

int gsw_set_def_pce_qmap(struct core_ops *ops);
int gsw_get_def_pce_qmap(struct core_ops *ops);
int gsw_set_def_bypass_qmap(struct core_ops *ops, GSW_QoS_qMapMode_t q_map_mode);
int gsw_get_def_bypass_qmap(struct core_ops *ops);
int gsw_qos_def_config(struct core_ops *ops);
int gsw_misc_config(struct core_ops *ops);

int gsw_pmac_init_nondpu(void);
int gsw_pmac_init_dpu(void);
int pmac_get_ig_cfg(struct core_ops *ops, u8 pmacid);
int pmac_get_bp_cfg(struct core_ops *ops, u8 pmacid);
int pmac_get_eg_cfg(struct core_ops *ops, u8 pmacid, u8 dst_port);


#endif

