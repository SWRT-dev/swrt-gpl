/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef DATAPATH_MISC31_H
#define DATAPATH_MISC31_H

#include <linux/notifier.h>
#include <linux/netdevice.h>
#include "datapath_ppv4.h"
#include <linux/debugfs.h>
#include <linux/seq_file.h>

#define PMAC_MAX_NUM  16
#define PAMC_LAN_MAX_NUM 7
#define VAP_OFFSET 8
#define VAP_MASK  0xF
#define VAP_DSL_OFFSET 3
#define NEW_CBM_API 1
#define PMAPPER_DISC_CTP 255
#define MAX_PPV4_PORTS 38
#define PPV4_PORT_BASE 90

#define GSWIP_O_DEV_NAME 1
#define GSWIP_L GSWIP_O_DEV_NAME
#define GSWIP_R GSWIP_O_DEV_NAME
#define MAX_SUBIF_PER_PORT 256
#define MAX_CTP 288
#define MAX_BP_NUM 128
#define CPU_PORT 0
#define CPU_SUBIF 0 /*cpu default subif ID*/
#define CPU_BP 0 /*cpu default bridge port ID */
#define CPU_FID 0 /*cpu default bridge ID */
#define SET_BP_MAP(x, ix) (x[(ix) / 16] |= 1 << ((ix) % 16))
#define GET_BP_MAP(x, ix) ((x[(ix) / 16] >> ((ix) % 16)) & 1)
#define UNSET_BP_MAP(x, ix) (x[(ix) / 16] &= ~(1 << ((ix) % 16)))

enum CQE_LOOKUP_MODE {
	CQE_LU_MODE0 = 0,
	CQE_LU_MODE1,
	CQE_LU_MODE2,
	CQE_LU_MODE3,
	CQE_LU_MODE_NOT_VALID,
};

#define PMAC_SIZE 8
struct gsw_itf {
	u8 ep; /*-1 means no assigned yet for dynamic case */
	u8 fixed; /*fixed (1) or dynamically allocate (0)*/
	u16 start;
	u16 end;
	u16 n;
	u8 mode;
	u8 cqe_mode; /*CQE look up mode */
};

struct cqm_deq_stat;
struct pp_queue_stat;

struct resv_q {
	int flag;
	int id;
	int physical_id;
};

struct resv_sch {
	int flag;
	int id;
};

struct resv_info {
	int num_resv_q; /*!< input:reserve the required number of queues*/
	int num_resv_sched; /*!< input:reserve required number of schedulers*/
	struct resv_q  *resv_q; /*!< reserved queues info*/
	struct resv_sch *resv_sched; /*!< reserved schedulers info */
};

struct ppv4_port_map_table {
	int flags[MAX_PPV4_PORTS];
};

struct pp_qos_dev;
struct hal_priv {
	struct cqm_deq_stat deq_port_stat[MAX_CQM_DEQ];
	struct pp_queue_stat qos_queue_stat[MAX_QUEUE];
	struct pp_sch_stat qos_sch_stat[QOS_MAX_NODES];
	struct resv_info resv[MAX_DP_PORTS];
	int bp_def;
	struct pp_qos_dev *qdev; /* ppv4 qos dev */
	s32 ppv4_drop_q;  /* drop queue: physical id */
	int cqm_drop_p; /* cqm drop/flush port id*/
	u32 ppv4_drop_p;  /* drop qos port(logical node_id):workaround for
			   * PPV4 API issue to get physical queue id
			   * before pp_qos_queue_set
			   */
	u32 ppv4_tmp_p; /* workaround for ppv4 queue allocate to
			 * to get physical queue id
			 */
	struct ppv4_port_map_table ppv4_port_map;/* PPV4 Ports table map */
};

struct datapath_ctrl {
	struct dentry *debugfs;
	const char *name;
};

#define SET_PMAC_IGP_EGP(pmac, port_id) ((pmac)->igp_egp = (port_id) & 0xF)

#define SET_PMAC_SUBIF(pmac, subif) do { \
	(pmac)->src_dst_subif_id_lsb = (subif) & 0xff; \
	(pmac)->src_dst_subif_id_msb =  ((subif) >> 8) & 0x1f; \
} while (0)

int alloc_bridge_port(int inst, int portid, int subif, int fid, int bp_member);
int free_bridge_port(int inst, int bp);
struct gsw_itf *ctp_port_assign(int inst, u8 ep, int bp_default,
				u32 flags, struct dp_dev_data *data);
void dp_sys_mib_reset_31(u32 flag);
int dp_pmac_set_31(int inst, u32 port, dp_pmac_cfg_t *pmac_cfg);
int dp_set_gsw_parser_31(u8 flag, u8 cpu, u8 mpe1, u8 mpe2, u8 mpe3);
int dp_get_gsw_parser_31(u8 *cpu, u8 *mpe1, u8 *mpe2, u8 *mpe3);
int gsw_mib_reset_31(int dev, u32 flag);
int proc_print_ctp_bp_info(struct seq_file *s, int inst,
			   struct pmac_port_info *port,
			   int subif_index, u32 flag);

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
int dp_gswip_mac_entry_add(int bport, int fid, int inst, u8 *addr);
int dp_gswip_mac_entry_del(int bport, int fid, int inst, u8 *addr);
int set_gswip_ext_vlan(struct core_ops *ops, struct ext_vlan_info *vlan,
		       int flag);
#endif
int qos_platform_set(int cmd_id, void *node, int flag);
int dp_node_alloc_31(struct dp_node_alloc *node, int flag);
int dp_node_free_31(struct dp_node_alloc *node, int flag);
int dp_deq_port_res_get_31(struct dp_dequeue_res *res, int flag);
int dp_node_link_en_get_31(struct dp_node_link_enable *en, int flag);
int dp_node_link_en_set_31(struct dp_node_link_enable *en, int flag);
int dp_qos_link_prio_set_31(struct dp_node_prio *info, int flag);
int dp_qos_link_prio_get_31(struct dp_node_prio *info, int flag);
int dp_node_link_add_31(struct dp_node_link *info, int flag);
int dp_link_add_31(struct dp_qos_link *cfg, int flag);
int dp_link_get_31(struct dp_qos_link *cfg, int flag);
int dp_node_unlink_31(struct dp_node_link *info, int flag);
int dp_node_link_get_31(struct dp_node_link *info, int flag);
int dp_queue_conf_set_31(struct dp_queue_conf *cfg, int flag);
int dp_queue_conf_get_31(struct dp_queue_conf *cfg, int flag);
int dp_shaper_conf_set_31(struct dp_shaper_conf *cfg, int flag);
int dp_shaper_conf_get_31(struct dp_shaper_conf *cfg, int flag);
int dp_queue_map_get_31(struct dp_queue_map_get *cfg, int flag);
int dp_queue_map_set_31(struct dp_queue_map_set *cfg, int flag);
int dp_counter_mode_set_31(struct dp_counter_conf *cfg, int flag);
int dp_counter_mode_get_31(struct dp_counter_conf *cfg, int flag);
int dp_set_gsw_pmapper_31(int inst, int bport, int lport,
			  struct dp_pmapper *mapper, u32 flag);
int dp_get_gsw_pmapper_31(int inst, int bport, int lport,
			  struct dp_pmapper *mapper, u32 flag);
int dp_children_get_31(struct dp_node_child *cfg, int flag);
int dp_free_children_via_parent_31(struct dp_node_alloc *node, int flag);
int dp_node_reserve(int inst, int ep, struct dp_dev_data *data, int flags);
int dp_q_reserve_continuos(int inst, int ep, struct dp_dev_data *data,
			   int flags);
int dp_qos_level_get_31(struct dp_qos_level *dp, int flag);
int dp_meter_alloc_31(int inst, int *meterid, int flag);
int dp_meter_add_31(struct net_device *dev,
		    struct dp_meter_cfg *meter,
		    int flag, struct dp_meter_subif *mtr_subif);
int dp_meter_del_31(struct net_device *dev,
		    struct dp_meter_cfg *meter,
		    int flag, struct dp_meter_subif *mtr_subif);
int dp_qos_global_info_get_31(struct dp_qos_cfg_info *info, int flag);
int dp_qos_port_conf_set_31(struct dp_port_cfg_info *info, int flag);
int32_t dp_rx_31(struct sk_buff *skb, u32 flags);
int32_t dp_xmit_31(struct net_device *rx_if, dp_subif_t *rx_subif,
		   struct sk_buff *skb, int32_t len, uint32_t flags);
void set_chksum(struct pmac_tx_hdr *pmac, u32 tcp_type, u32 ip_offset,
		int ip_off_hw_adjust, u32 tcp_h_offset);
int dp_set_bp_attr_31(struct dp_bp_attr *conf, int bport, uint32_t flag);

static inline char *parser_flag_str(u8 f)
{
	if (f == DP_PARSER_F_DISABLE)
		return "No Parser";
	else if (f == DP_PARSER_F_HDR_ENABLE)
		return "Parser Flag only";
	else if (f == DP_PARSER_F_HDR_OFFSETS_ENABLE)
		return "Parser Full";
	else
		return "Reserved";
}

int dp_sub_proc_install_31(void);
char *get_dma_flags_str31(u32 epn, char *buf, int buf_len);
int lookup_dump31(struct seq_file *s, int pos);
int lookup_start31(void);
ssize_t proc_get_qid_via_index31(struct file *file, const char *buf,
				 size_t count, loff_t *ppos);
ssize_t proc_get_qid_via_index(struct file *file, const char *buf,
			       size_t count, loff_t *ppos);
int datapath_debugfs_init(struct datapath_ctrl *pctrl);
int get_q_qocc(int inst, int qid, u32 *c);
int get_q_mib(int inst, int qid,
	      u32 *total_accept,
	      u32 *total_drop,
	      u32 *red_drop);
int get_p_mib(int inst, int pid,
	      u32 *green /* bytes*/,
	      u32 *yellow /*bytes*/);
int cpu_vlan_mod_dis(int inst);
int set_port_lookup_mode_31(int inst, u8 ep, u32 flags);
int tc_vlan_set_31(struct core_ops *ops, struct dp_tc_vlan *vlan,
		   struct dp_tc_vlan_info *info,
		   int flag);
#endif
