/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include<linux/init.h>
#include<linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/if_ether.h>
#include <linux/ethtool.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/if_ether.h>
#include <linux/clk.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <net/datapath_api.h>
#include "datapath.h"
#include "datapath_tx.h"
#include "datapath_instance.h"
#include "datapath_swdev_api.h"

#if defined(CONFIG_INTEL_DATAPATH_DBG) && CONFIG_INTEL_DATAPATH_DBG
unsigned int dp_max_print_num = -1, dp_print_num_en = 0;
#endif

GSW_API_HANDLE gswr_r;
u32    dp_rx_test_mode = DP_RX_MODE_NORMAL;
struct dma_rx_desc_1 dma_rx_desc_mask1;
struct dma_rx_desc_2 dma_rx_desc_mask2;
struct dma_rx_desc_3 dma_rx_desc_mask3;
struct dma_rx_desc_0 dma_tx_desc_mask0;
struct dma_rx_desc_1 dma_tx_desc_mask1;
u32 dp_drop_all_tcp_err;
u32 dp_pkt_size_check;

u32 dp_dbg_flag;
EXPORT_SYMBOL(dp_dbg_flag);

#undef DP_DBG_ENUM_OR_STRING
#define DP_DBG_ENUM_OR_STRING(name, short_name) short_name
char *dp_dbg_flag_str[] = DP_DBG_FLAG_LIST;

#undef DP_DBG_ENUM_OR_STRING
#define DP_DBG_ENUM_OR_STRING(name, short_name) name
u32 dp_dbg_flag_list[] = DP_DBG_FLAG_LIST;

#undef DP_F_ENUM_OR_STRING
#define DP_F_ENUM_OR_STRING(name, short_name) short_name
char *dp_port_type_str[] = DP_F_FLAG_LIST;

#undef DP_F_ENUM_OR_STRING
#define DP_F_ENUM_OR_STRING(name, short_name) name
u32 dp_port_flag[] = DP_F_FLAG_LIST;

char *dp_port_status_str[] = {
	"PORT_FREE",
	"PORT_ALLOCATED",
	"PORT_DEV_REGISTERED",
	"PORT_SUBIF_REGISTERED",
	"Invalid"
};

static int try_walkaround;
int dp_init_ok;
DP_DEFINE_LOCK(dp_lock);
unsigned int dp_dbg_err = 1; /*print error */
EXPORT_SYMBOL(dp_dbg_err);
static int dp_register_dc(int inst, uint32_t port_id,
			  struct cbm_dp_alloc_complete_data *cbm_data,
			  struct dp_dev_data *data, uint32_t flags);

static int dp_build_cqm_data(int inst, uint32_t port_id,
			     struct cbm_dp_alloc_complete_data *cbm_data,
			     struct dp_dev_data *data);

/*port 0 is reserved and never assigned to any one */
int dp_inst_num;
/* Keep per DP instance information here */
struct inst_property dp_port_prop[DP_MAX_INST];
/* Keep all subif information per instance/LPID/subif */
struct pmac_port_info *dp_port_info[DP_MAX_INST];

/* bp_mapper_dev[] is mainly for PON case
 * Only if multiple gem port are attached to same bridge port,
 * This bridge port device will be recorded into this bp_mapper_dev[].
 * later other information, like pmapper ID/mapping table will be put here also
 */
struct bp_pmapper_dev dp_bp_dev_tbl[DP_MAX_INST][DP_MAX_BP_NUM];
/* q_tbl[] is mainly for the queue created/used during dp_register_subif_ext
 */
struct q_info dp_q_tbl[DP_MAX_INST][DP_MAX_QUEUE_NUM];
/* sched_tbl[] is mainly for the sched created/used during dp_register_subif_ext
 */
struct dp_sched_info dp_sched_tbl[DP_MAX_INST][DP_MAX_SCHED_NUM];
/* dp_deq_port_tbl[] is to record cqm dequeue port info
 */
struct cqm_port_info dp_deq_port_tbl[DP_MAX_INST][DP_MAX_CQM_DEQ];

/* DMA TX CH info*/
struct dma_chan_info *dp_dma_chan_tbl[DP_MAX_INST];

struct parser_info pinfo[4];
int dp_print_len;

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_ACA_CSUM_WORKAROUND)
static struct module aca_owner;
static struct net_device aca_dev;
int aca_portid = -1;
#endif

static void *dp_ops[DP_MAX_INST][DP_OPS_CNT];

int dp_register_ops(int inst, enum DP_OPS_TYPE type, void *ops)
{
	if (inst < 0 || inst >= DP_MAX_INST || type >= DP_OPS_CNT) {
		DP_DEBUG(DP_DBG_FLAG_REG, "wrong index\n");
		return DP_FAILURE;
	}
	dp_ops[inst][type] = ops;

	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_register_ops);

void *dp_get_ops(int inst, enum DP_OPS_TYPE type)
{
	if (inst < 0 || inst >= DP_MAX_INST || type >= DP_OPS_CNT) {
		DP_DEBUG(DP_DBG_FLAG_REG, "wrong index\n");
		return NULL;
	}
	return dp_ops[inst][type];
}
EXPORT_SYMBOL(dp_get_ops);

struct inst_property *dp_port_prop_dbg(int inst)
{
	return get_dp_port_prop(inst);
}
EXPORT_SYMBOL(dp_port_prop_dbg);

struct pmac_port_info *dp_port_info_dbg(int inst, int index)
{
	return get_dp_port_info(inst, index);
}
EXPORT_SYMBOL(dp_port_info_dbg);

char *get_dp_port_type_str(int k)
{
	return dp_port_type_str[k];
}
EXPORT_SYMBOL(get_dp_port_type_str);

u32 get_dp_port_flag(int k)
{
	return dp_port_flag[k];
}
EXPORT_SYMBOL(get_dp_port_flag);

int get_dp_port_type_str_size(void)
{
	return ARRAY_SIZE(dp_port_type_str);
}
EXPORT_SYMBOL(get_dp_port_type_str_size);

int get_dp_dbg_flag_str_size(void)
{
	return ARRAY_SIZE(dp_dbg_flag_str);
}

int get_dp_port_status_str_size(void)
{
	return ARRAY_SIZE(dp_port_status_str);
}

int parser_size_via_index(u8 index)
{
	if (index >= ARRAY_SIZE(pinfo)) {
		pr_err("Wrong index=%d, it should less than %zu\n", index,
		       ARRAY_SIZE(pinfo));
		return 0;
	}

	return pinfo[index].size;
}

int parser_enabled(int ep, struct dma_rx_desc_1 *desc_1)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
	if (!desc_1) {
		pr_err("NULL desc_1 is not allowed\n");
		return 0;
	}
#endif

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32) || \
	IS_ENABLED(CONFIG_X86_INTEL_LGM)
#else
	if (!ep)
		return pinfo[(desc_1->field.mpe2 << 1) +
			desc_1->field.mpe1].size;
#endif
	return 0;
}

u32 *get_port_flag(int inst, int index)
{
	if (index < dp_port_prop[inst].info.cap.max_num_dp_ports)
		return &get_dp_port_info(inst, index)->alloc_flags;

	return NULL;
}
EXPORT_SYMBOL(get_port_flag);

struct pmac_port_info *get_port_info_via_dp_port(int inst, int dp_port)
{
	int i;

	for (i = 0; i < dp_port_prop[inst].info.cap.max_num_dp_ports; i++) {
		struct pmac_port_info *port = get_dp_port_info(inst, i);

		if ((port->status & PORT_DEV_REGISTERED) &&
		    (port->port_id == dp_port))
			return port;
	}

	return NULL;
}

struct pmac_port_info *get_port_info_via_dev(struct net_device *dev)
{
	int i;
	int inst = dp_get_inst_via_dev(dev, NULL, 0);

	for (i = 0; i < dp_port_prop[inst].info.cap.max_num_dp_ports; i++) {
		struct pmac_port_info *port = get_dp_port_info(inst, i);

		if ((port->status & PORT_DEV_REGISTERED) && (port->dev == dev))
			return port;
	}
	return NULL;
}

int8_t parser_size(int8_t v)
{
	if (v == DP_PARSER_F_DISABLE)
		return 0;

	if (v == DP_PARSER_F_HDR_ENABLE)
		return PASAR_OFFSETS_NUM;

	if (v == DP_PARSER_F_HDR_OFFSETS_ENABLE)
		return PASAR_OFFSETS_NUM + PASAR_FLAGS_NUM;

	pr_err("Wrong parser setting: %d\n", v);
	/*error */
	return -1;
}

/*Only for SOC side, not for peripheral device side */
int dp_set_gsw_parser(u8 flag, u8 cpu, u8 mpe1, u8 mpe2, u8 mpe3)
{
	int inst = 0;

	if (!dp_port_prop[inst].info.dp_set_gsw_parser)
		return -1;

	return dp_port_prop[inst].info.dp_set_gsw_parser(flag, cpu, mpe1,
							 mpe2, mpe3);
}
EXPORT_SYMBOL(dp_set_gsw_parser);

int dp_get_gsw_parser(u8 *cpu, u8 *mpe1, u8 *mpe2, u8 *mpe3)
{
	int inst = 0;

	if (!dp_port_prop[inst].info.dp_get_gsw_parser)
		return -1;

	return dp_port_prop[inst].info.dp_get_gsw_parser(cpu, mpe1,
							 mpe2, mpe3);
}
EXPORT_SYMBOL(dp_get_gsw_parser);

char *parser_str(int index)
{
	if (index == 0)
		return "cpu";

	if (index == 1)
		return "mpe1";

	if (index == 2)
		return "mpe2";

	if (index == 3)
		return "mpe3";

	pr_err("Wrong index:%d\n", index);
	return "Wrong index";
}

/* some module may have reconfigure parser configuration in FMDA_PASER.
 * It is necessary to refresh the pinfo
 */
void dp_parser_info_refresh(u32 cpu, u32 mpe1, u32 mpe2,
			    u32 mpe3, u32 verify)
{
	int i;

	pinfo[0].v = cpu;
	pinfo[1].v = mpe1;
	pinfo[2].v = mpe2;
	pinfo[3].v = mpe3;

	for (i = 0; i < ARRAY_SIZE(pinfo); i++) {
		if (verify && (pinfo[i].size != parser_size(pinfo[i].v)))
			pr_err("%s[%d](%d) != %s(%d)??\n",
			       "Lcal parser pinfo", i, pinfo[i].size,
			       "register cfg", parser_size(pinfo[i].v));

		/*force to update */
		pinfo[i].size = parser_size(pinfo[i].v);

		if ((pinfo[i].size < 0) || (pinfo[i].size > PKT_PMAC_OFFSET)) {
			pr_err("Wrong parser setting for %s: %d\n",
			       parser_str(i), pinfo[i].v);
		}
	}
}
EXPORT_SYMBOL(dp_parser_info_refresh);

void print_parser_status(struct seq_file *s)
{
	if (!s)
		return;

	seq_printf(s, "REG.cpu  value=%u size=%u\n", pinfo[0].v, pinfo[0].size);
	seq_printf(s, "REG.MPE1 value=%u size=%u\n", pinfo[1].v, pinfo[1].size);
	seq_printf(s, "REG.MPE2 value=%u size=%u\n", pinfo[2].v, pinfo[2].size);
	seq_printf(s, "REG.MPE3 value=%u size=%u\n", pinfo[3].v, pinfo[3].size);
}

/*note: dev can be NULL */
static int32_t dp_alloc_port_private(int inst,
				     struct module *owner,
				     struct net_device *dev,
				     u32 dev_port, s32 port_id,
				     dp_pmac_cfg_t *pmac_cfg,
				     struct dp_port_data *data,
				     u32 flags)
{
	int i;
	struct cbm_dp_alloc_data cbm_data = {0};
	struct pmac_port_info *port;

	if (!owner) {
		pr_err("Allocate port failed for owner NULL\n");
		return DP_FAILURE;
	}

	if (port_id >= MAX_DP_PORTS || port_id < 0) {
		DP_DEBUG_ASSERT((port_id >= MAX_DP_PORTS),
				"port_id(%d) >= MAX_DP_PORTS(%d)", port_id,
				MAX_DP_PORTS);
		DP_DEBUG_ASSERT((port_id < 0), "port_id(%d) < 0", port_id);
		return DP_FAILURE;
	}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (unlikely(dp_dbg_flag & DP_DBG_FLAG_REG)) {
		DP_DEBUG(DP_DBG_FLAG_REG, "Flags=");
		for (i = 0; i < ARRAY_SIZE(dp_port_type_str); i++)
			if (flags & dp_port_flag[i])
				DP_DEBUG(DP_DBG_FLAG_REG, "%s ",
					 dp_port_type_str[i]);
		DP_DEBUG(DP_DBG_FLAG_REG, "\n");
	}
#endif
	cbm_data.dp_inst = inst;
	cbm_data.cbm_inst = dp_port_prop[inst].cbm_inst;
	port = get_dp_port_info(inst, port_id);

	if (flags & DP_F_DEREGISTER) {	/*De-register */
		if (port->status != PORT_ALLOCATED) {
			pr_err("No Deallocate for module %s w/o deregistered\n",
			       owner->name);
			return DP_FAILURE;
		}
		cbm_data.deq_port = port->deq_port_base;
		cbm_data.dma_chan = port->dma_chan;
		cbm_dp_port_dealloc(owner, dev_port, port_id, &cbm_data, port->alloc_flags | flags);
		dp_inst_insert_mod(owner, port_id, inst, 0);
		DP_DEBUG(DP_DBG_FLAG_REG, "de-alloc port %d\n", port_id);
		DP_CB(inst, port_platform_set)(inst, port_id, data, flags);
		/* Only clear those fields we need to clear */
		memset(port, 0, offsetof(struct pmac_port_info, tail));
		return DP_SUCCESS;
	}
	if (port_id) { /*with specified port_id */
		if (port->status != PORT_FREE) {
			pr_err("%s %s(%s %d) fail: port %d used by %s %d\n",
			       "module", owner->name,
			       "dev_port", dev_port, port_id,
			       port->owner->name,
			       port->dev_port);
			return DP_FAILURE;
		}
	}
	if (cbm_dp_port_alloc(owner, dev, dev_port, port_id,
			      &cbm_data, flags)) {
		pr_err("cbm_dp_port_alloc fail for %s/dev_port %d: %d\n",
		       owner->name, dev_port, port_id);
		return DP_FAILURE;
	} else if (!(cbm_data.flags & CBM_PORT_DP_SET) &&
		   !(cbm_data.flags & CBM_PORT_DQ_SET)) {
		pr_err("%s NO DP_SET/DQ_SET(%x):%s/dev_port %d\n",
		       "cbm_dp_port_alloc",
		       cbm_data.flags,
		       owner->name, dev_port);
		return DP_FAILURE;
	}
	DP_DEBUG(DP_DBG_FLAG_REG,
		 "cbm alloc dpport:%d deq:%d dmachan=0x%x deq_num:%d\n",
		 cbm_data.dp_port, cbm_data.deq_port, cbm_data.dma_chan,
		 cbm_data.deq_port_num);

	port_id = cbm_data.dp_port;
	/* Only clear those fields we need to clear */
	port = get_dp_port_info(inst, port_id);
	memset(port, 0, offsetof(struct pmac_port_info, tail));
	/*save info from caller */
	port->owner = owner;
	port->dev = dev;
	port->dev_port = dev_port;
	port->alloc_flags = flags;
	port->status = PORT_ALLOCATED;
	/*save info from cbm_dp_port_alloc*/
	port->flag_other = cbm_data.flags;
	port->port_id = cbm_data.dp_port;
	port->deq_port_base = cbm_data.deq_port;
	port->deq_port_num = cbm_data.deq_port_num;
	DP_DEBUG(DP_DBG_FLAG_REG,
		 "cbm alloc dp_port:%d deq:%d deq_num:%d\n",
		 cbm_data.dp_port, cbm_data.deq_port, cbm_data.deq_port_num);

	port->num_dma_chan = cbm_data.num_dma_chan;
#if 1  /* TODO: Hardcorded currently, later need to align with CQM */
	port->policy_base = 0;
	port->policy_num = 4;
#endif
	if (cbm_data.num_dma_chan) {
		u16 dma_ch_base;

		dma_ch_base = get_dma_chan_idx(inst, cbm_data.num_dma_chan);
		if (dma_ch_base == DP_FAILURE) {
			pr_err("Failed get_dma_chan_idx!!\n");
			cbm_dp_port_dealloc(owner, dev_port, port_id, &cbm_data,
					    flags | DP_F_DEREGISTER);
			/* Only clear those fields we need to clear */
			memset(port, 0,
			       offsetof(struct pmac_port_info, tail));
			return DP_FAILURE;
		}
		port->dma_ch_base = dma_ch_base;
	}
	/*save info to port data*/
	data->deq_port_base = port->deq_port_base;
	data->deq_num = port->deq_port_num;
	if (cbm_data.flags & CBM_PORT_DMA_CHAN_SET)
		port->dma_chan = cbm_data.dma_chan;
	if (cbm_data.flags & CBM_PORT_PKT_CRDT_SET)
		port->tx_pkt_credit =
				cbm_data.tx_pkt_credit;
	if (cbm_data.flags & CBM_PORT_BYTE_CRDT_SET)
		port->tx_b_credit = cbm_data.tx_b_credit;
	if (cbm_data.flags & CBM_PORT_RING_ADDR_SET) {
		port->txpush_addr = (void *)cbm_data.txpush_addr;
		port->txpush_addr_qos =	(void *)cbm_data.txpush_addr_qos;
	}
	if (cbm_data.flags & CBM_PORT_RING_SIZE_SET)
	port->tx_ring_size = cbm_data.tx_ring_size;
	if (cbm_data.flags & CBM_PORT_RING_OFFSET_SET)
		port->tx_ring_offset =
				cbm_data.tx_ring_offset;
	if ((cbm_data.num_dma_chan > 1) && (cbm_data.deq_port_num !=
	    cbm_data.num_dma_chan)) {
		pr_err("ERROR:deq_port_num=%d not equal to num_dma_chan=%d\n",
		       cbm_data.deq_port_num, cbm_data.num_dma_chan);
		return DP_FAILURE;
	}

	if (dp_port_prop[inst].info.port_platform_set(inst, port_id,
						      data, flags)) {
		pr_err("Failed port_platform_set for port_id=%d(%s)\n",
		       port_id, owner ? owner->name : "");
		cbm_dp_port_dealloc(owner, dev_port, port_id, &cbm_data,
				    flags | DP_F_DEREGISTER);
		/* Only clear those fields we need to clear */
		memset(port, 0,
		       offsetof(struct pmac_port_info, tail));
		return DP_FAILURE;
	}
	if (pmac_cfg)
		dp_pmac_set(inst, port_id, pmac_cfg);
	/*only 1st dp instance support real CPU path traffic */
	if (!inst && dp_port_prop[inst].info.init_dma_pmac_template)
		dp_port_prop[inst].info.init_dma_pmac_template(port_id, flags);
	for (i = 0; i < dp_port_prop[inst].info.cap.max_num_subif_per_port; i++)
		INIT_LIST_HEAD(&get_dp_port_subif(port, i)->logic_dev);
	dp_inst_insert_mod(owner, port_id, inst, 0);

	DP_DEBUG(DP_DBG_FLAG_REG,
		 "Port %d allocation succeed for module %s with dev_port %d\n",
		 port_id, owner->name, dev_port);
	return port_id;
}

int32_t dp_register_subif_private(int inst, struct module *owner,
				  struct net_device *dev,
				  char *subif_name, dp_subif_t *subif_id,
				  /*device related info*/
				  struct dp_subif_data *data, u32 flags)
{
	int res = DP_FAILURE;

	int i, port_id, start, end, j;
	struct pmac_port_info *port_info;
	struct cbm_dp_en_data cbm_data = {0};
	struct subif_platform_data platfrm_data = {0};
	struct dp_subif_info *sif;
	u32 cqm_deq_port;
	u32 dma_ch_offset;
	u32 dma_ch_ref, dma_ch_ref_curr;

	port_id = subif_id->port_id;
	port_info = get_dp_port_info(inst, port_id);
	subif_id->inst = inst;
	subif_id->subif_num = 1;
	platfrm_data.subif_data = data;
	platfrm_data.dev = dev;
	/*Sanity Check*/
	if (port_info->status < PORT_DEV_REGISTERED) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "register subif failed:%s is not a registered dev!\n",
			 subif_name);
		return res;
	}

	if (subif_id->subif < 0) {/*dynamic mode */
		if (flags & DP_F_SUBIF_LOGICAL) {
			if (!(DP_CB(inst, supported_logic_dev)(inst,
							       dev,
							       subif_name))) {
				DP_DEBUG(DP_DBG_FLAG_REG,
					 "reg subif fail:%s not support dev\n",
				 subif_name);
				return res;
			}
			if (!(flags & DP_F_ALLOC_EXPLICIT_SUBIFID)) {
				/*Share same subif with its base device
				 *For GRX350: nothing need except save it
				 *For PRX300: it need to allocate BP for it
				 */
				res = add_logic_dev(inst, port_id, dev,
						    subif_id, flags);
				return res;
			}
		}
		start = 0;
		end = port_info->subif_max;
	} else {
		/*caller provided subif. Try to get its vap value as start */
		start = GET_VAP(subif_id->subif, port_info->vap_offset,
				port_info->vap_mask);
		end = start + 1;
	}

	/*pr_info("search range: start=%d end=%d\n",start, end);*/
    /*allocate a free subif */
	for (i = start; i < end; i++) {
		sif = get_dp_port_subif(port_info, i);
		if (sif->flags) /*used already & not free*/
			continue;

		if (data->num_deq_port == 0)
			data->num_deq_port = 1;
		cqm_deq_port = port_info->deq_port_base + data->deq_port_idx;
		dma_ch_offset =	DP_DEQ(inst, cqm_deq_port).dma_ch_offset;
		dma_ch_ref_curr = atomic_read(&(dp_dma_chan_tbl[inst] +
					      dma_ch_offset)->ref_cnt);
		/*now find a free subif or valid subif
		 *need to do configuration HW
		 */
		if (port_info->status) {
			if (dp_port_prop[inst].info.subif_platform_set(
				inst, port_id, i, &platfrm_data, flags)) {
				pr_err("subif_platform_set fail\n");
				goto EXIT;
			} else {
				DP_DEBUG(DP_DBG_FLAG_REG,
					 "subif_platform_set succeed\n");
			}
		} else {
			pr_err("port info status fail for 0\n");
			return res;
		}
		sif->flags = 1;
		sif->netif = dev;
		port_info->port_id = port_id;
		/*currently this field is used for EPON case. Later can further
		 * enhance
		 */
		sif->num_qid = data->num_deq_port;
		sif->deq_port_idx = data->deq_port_idx;

		if (subif_id->subif < 0) /*dynamic:shift bits as HW defined*/
			sif->subif =
				SET_VAP(i, port_info->vap_offset,
					port_info->vap_mask);
		else /*provided by caller since it is alerady shifted properly*/
			sif->subif =
			    subif_id->subif;
		strncpy(sif->device_name,
			subif_name, sizeof(sif->device_name) - 1);
		sif->subif_flag = flags;
		sif->data_flag_ops = data->flag_ops;
		if (data->flag_ops & DP_SUBIF_RX_FLAG)
			STATS_SET(sif->rx_flag, !!data->rx_en_flag);
		else
			STATS_SET(sif->rx_flag, 1);
		port_info->status = PORT_SUBIF_REGISTERED;
		subif_id->port_id = port_id;
		subif_id->subif = sif->subif;
		/* set port as LCT port */
		if (data->flag_ops & DP_F_DATA_LCT_SUBIF)
			port_info->lct_idx = i;
		port_info->num_subif++;
		if ((port_info->num_subif == 1) ||
		    (platfrm_data.act & TRIGGER_CQE_DP_ENABLE)) {
			cbm_data.dp_inst = inst;
			cbm_data.num_dma_chan = port_info->num_dma_chan;
			cbm_data.cbm_inst = dp_port_prop[inst].cbm_inst;
			cbm_data.deq_port = port_info->deq_port_base +
				(data ? data->deq_port_idx : 0);
			if ((cbm_data.deq_port == 0) ||
			    (cbm_data.deq_port >= DP_MAX_CQM_DEQ)) {
				pr_err("Wrong deq_port: %d\n",
				       cbm_data.deq_port);
				return res;
			}
			if (!dp_dma_chan_tbl[inst]) {
				pr_err("dp_dma_chan_tbl[%d] NULL\n", inst);
				return res;
			}
			for (j = 0; j < data->num_deq_port; j++) {
				cqm_deq_port = sif->cqm_deq_port[j];
				dma_ch_offset =
					DP_DEQ(inst, cqm_deq_port).dma_ch_offset;
				dma_ch_ref =
					atomic_read(&(dp_dma_chan_tbl[inst] +
						 dma_ch_offset)->ref_cnt);
				cbm_data.deq_port = port_info->deq_port_base +
							data->deq_port_idx + j;
				/* No need to enable DMA if multiple DEQ->single
				 * DMA
				 */
				if (j != 0)
					cbm_data.dma_chnl_init = 0;
				else
				/* PPA Directpath/LitePath don't have DMA CH */
					if (dma_ch_ref_curr == 0 &&
					    !(port_info->alloc_flags &
					    DP_F_DIRECT))
						/*to enable DMA*/
						cbm_data.dma_chnl_init = 1;
				DP_DEBUG(DP_DBG_FLAG_REG, "%s%d%s%d%s%d%s%d%d\n",
					 "cbm_dp_enable: dp_port=", port_id,
					 " deq_port=", cbm_data.deq_port,
					 " dma_chnl_init=",
					 cbm_data.dma_chnl_init,
					 " tx_dma_chan ref=", dma_ch_ref, dma_ch_ref_curr);
				if (cbm_dp_enable(owner, port_id, &cbm_data, 0,
						  port_info->alloc_flags)) {
					DP_DEBUG(DP_DBG_FLAG_REG,
						 "cbm_dp_enable fail\n");
					return res;
				}
			}
			DP_DEBUG(DP_DBG_FLAG_REG, "cbm_dp_enable ok\n");
		} else {
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "No need cbm_dp_enable:dp_port=%d subix=%d\n",
				 port_id, i);
		}
		break;
	}

	if (i < end) {
		res = DP_SUCCESS;
		if (dp_bp_dev_tbl[inst][sif->bp].ref_cnt > 1)
			return res;
		dp_inst_add_dev(dev, subif_name,
				subif_id->inst, subif_id->port_id,
				sif->bp,
				subif_id->subif, flags);
	} else {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "register subif failed for no matched vap\n");
	}
EXIT:
	return res;
}

int32_t dp_deregister_subif_private(int inst, struct module *owner,
				    struct net_device *dev,
				    char *subif_name, dp_subif_t *subif_id,
				    struct dp_subif_data *data,
				    uint32_t flags)
{
	int res = DP_FAILURE;
	int i, j, port_id, cqm_port, bp;
	u8 find = 0;
	struct pmac_port_info *port_info;
	struct cbm_dp_en_data cbm_data = {0};
	struct subif_platform_data platfrm_data = {0};
	struct dp_subif_info *sif;
	u32 dma_ch_offset, dma_ch_ref;

	port_id = subif_id->port_id;
	port_info = get_dp_port_info(inst, port_id);
	platfrm_data.subif_data = data;
	platfrm_data.dev = dev;

	DP_DEBUG(DP_DBG_FLAG_REG,
		 "Try to unregister subif=%s with dp_port=%d subif=%d\n",
		 subif_name, subif_id->port_id, subif_id->subif);

	if (port_info->status != PORT_SUBIF_REGISTERED) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "Unregister failed:%s not registered subif!\n",
			 subif_name);
		return res;
	}
	if (!dp_dma_chan_tbl[inst]) {
		pr_err("dp_dma_chan_tbl[%d] NULL\n", inst);
		return res;
	}

	for (i = 0; i < port_info->subif_max; i++) {
		sif = get_dp_port_subif(port_info, i);
		if (sif->subif == subif_id->subif) {
			find = 1;
			break;
		}
	}
	if (!find)
		return res;

	DP_DEBUG(DP_DBG_FLAG_REG,
		 "Found matched subif: port_id=%d subif=%x vap=%d\n",
		 subif_id->port_id, subif_id->subif, i);
	if (sif->netif != dev) {
		/* device not match. Maybe it is unexplicit logical dev */
		res = del_logic_dev(inst, &sif->logic_dev,
				    dev, flags);
		return res;
	}
	/* reset LCT port */
	if (data->flag_ops & DP_F_DATA_LCT_SUBIF)
		port_info->lct_idx = 0;
	if (!list_empty(&sif->logic_dev)) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "Unregister fail: logic_dev of %s not empty yet!\n",
			 subif_name);
		return res;
	}
	bp = sif->bp;
	/* reset mib, flag, and others */
	memset(&sif->mib, 0, sizeof(sif->mib));
	sif->flags = 0;
	sif->netif = NULL;
	port_info->num_subif--;
	if (dp_port_prop[inst].info.subif_platform_set(inst, port_id, i,
						       &platfrm_data, flags)) {
		pr_err("subif_platform_set fail\n");
		/*return res;*/
	}
	if (!port_info->num_subif)
		port_info->status = PORT_DEV_REGISTERED;
	for (j = 0; j < sif->num_qid; j++) {
		cqm_port = sif->cqm_deq_port[j];
		if (!dp_deq_port_tbl[inst][cqm_port].ref_cnt) {
			/*delete all queues which may created
			 * by PPA or other apps
			 */
			struct dp_node_alloc port_node;

			port_node.inst = inst;
			port_node.dp_port = port_id;
			port_node.type = DP_NODE_PORT;
			port_node.id.cqm_deq_port = cqm_port;
			dp_node_children_free(&port_node, 0);
			/*disable cqm port */
			cbm_data.dp_inst = inst;
			cbm_data.cbm_inst = dp_port_prop[inst].cbm_inst;
			cbm_data.deq_port = cqm_port;
			dma_ch_offset =	DP_DEQ(inst, cqm_port).dma_ch_offset;
			dma_ch_ref = atomic_read(&(dp_dma_chan_tbl[inst] +
						 dma_ch_offset)->ref_cnt);
			/* PPA Directpath/LitePath don't have DMA CH */
			if ((!(port_info->alloc_flags & DP_F_DIRECT)) &&
			    (!dma_ch_ref))
				cbm_data.dma_chnl_init = 1; /*to disable DMA */
			if (cbm_dp_enable(owner, port_id, &cbm_data,
					  CBM_PORT_F_DISABLE,
					  port_info->alloc_flags)) {
				DP_DEBUG(DP_DBG_FLAG_REG,
					 "cbm_dp_disable fail:port=%d subix=%d %s=%d\n",
					 port_id, i, "dma_chnl_init",
					 cbm_data.dma_chnl_init);
				return res;
			}
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "cbm_dp_disable ok:port=%d subix=%d cqm_port=%d\n",
				 port_id, i, cqm_port);
		}
	}
	/* for pmapper and non-pmapper both
	 *  1)for PRX300, dev is managed at its HAL level
	 *  2)for GRX350, bp/dev should be always zero/NULL at present
	 *        before adapting to new datapath framework
	 */
	if (!dp_bp_dev_tbl[inst][bp].dev) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "dp_inst_del_dev for %s inst=%d bp=%d\n",
			 dev->name, inst, bp);
		dp_inst_del_dev(dev, subif_name, inst, port_id,
				subif_id->subif, 0);
	}

	DP_DEBUG(DP_DBG_FLAG_REG, "  dp_port=%d subif=%d cqm_port=%d\n",
		 subif_id->port_id, subif_id->subif, cqm_port);
	res = DP_SUCCESS;

	return res;
}

/*Note: For same owner, it should be in the same HW instance
 *          since dp_register_dev/subif no dev_port information at all,
 *          at the same time, dev is optional and can be NULL
 */

int32_t dp_alloc_port(struct module *owner, struct net_device *dev,
		      u32 dev_port, int32_t port_id,
		      dp_pmac_cfg_t *pmac_cfg, uint32_t flags)
{
	struct dp_port_data data = {0};

	return dp_alloc_port_ext(0, owner, dev, dev_port, port_id, pmac_cfg,
				 &data, flags);
}
EXPORT_SYMBOL(dp_alloc_port);

int32_t dp_alloc_port_ext(int inst, struct module *owner,
			  struct net_device *dev,
			  u32 dev_port, int32_t port_id,
			  dp_pmac_cfg_t *pmac_cfg,
			  struct dp_port_data *data, uint32_t flags)
{
	int res;
	struct dp_port_data tmp_data = {0};
	int first = 0;

	if (unlikely(!dp_init_ok)) {
		DP_LIB_LOCK(&dp_lock);
		if (!try_walkaround) {
			try_walkaround++;
			first = try_walkaround;
		}
		DP_LIB_UNLOCK(&dp_lock);
		if (first == 1)
			dp_init_module();
		if (!dp_init_ok) {
			pr_err("dp_alloc_port fail: datapath can't init\n");
			return DP_FAILURE;
		}
	}
	if (!dp_port_prop[0].valid) {
		pr_err("No Valid datapath instance yet?\n");
		return DP_FAILURE;
	}
	if (!data)
		data = &tmp_data;
	DP_LIB_LOCK(&dp_lock);
	res = dp_alloc_port_private(inst, owner, dev, dev_port,
				    port_id, pmac_cfg, data, flags);
	DP_LIB_UNLOCK(&dp_lock);
	if (inst) /* only inst zero need ACA workaround */
		return res;

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_ACA_CSUM_WORKAROUND)
	/*For VRX518, it will always carry DP_F_FAST_WLAN flag for
	 * ACA HW resource purpose in CBM
	 */
	if ((res > 0) &&
	    (flags & DP_F_FAST_WLAN) &&
	    (aca_portid < 0)) {
		dp_subif_t subif_id;
		#define ACA_CSUM_NAME "aca_csum"
		strcpy(aca_owner.name, ACA_CSUM_NAME);
		strcpy(aca_dev.name, ACA_CSUM_NAME);
		aca_portid = dp_alloc_port(&aca_owner, &aca_dev,
					   0, 0, NULL, DP_F_CHECKSUM);
		if (aca_portid <= 0) {
			pr_err("dp_alloc_port failed for %s\n", ACA_CSUM_NAME);
			return res;
		}
		if (dp_register_dev(&aca_owner, aca_portid,
				    NULL, DP_F_CHECKSUM)) {
			pr_err("dp_register_dev fail for %s\n", ACA_CSUM_NAME);
			return res;
		}
		subif_id.port_id = aca_portid;
		subif_id.subif = -1;
		if (dp_register_subif(&aca_owner, &aca_dev,
				      ACA_CSUM_NAME, &subif_id,
				      DP_F_CHECKSUM)) {
			pr_err("dp_register_subif fail for %s\n",
			       ACA_CSUM_NAME);
			return res;
		}
	}
#endif
	return res;
}
EXPORT_SYMBOL(dp_alloc_port_ext);

int32_t dp_register_dev(struct module *owner, uint32_t port_id,
			dp_cb_t *dp_cb, uint32_t flags)
{
	int inst = dp_get_inst_via_module(owner, port_id, 0);

	if (inst < 0) {
		pr_err("dp_register_dev not valid module %s\n", owner->name);
		return -1;
	}

	return dp_register_dev_ext(inst, owner, port_id, dp_cb, NULL, flags);
}
EXPORT_SYMBOL(dp_register_dev);

#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
static int remove_umt(int inst, const struct dp_umt_port *umt)
{
	struct umt_ops *ops = dp_get_umt_ops(inst);

	if (!ops)
		return -ENODEV;

	return ops->umt_release(ops->umt_dev, umt->ctl.id);
}
#endif

int32_t dp_register_dev_ext(int inst, struct module *owner, uint32_t port_id,
			    dp_cb_t *dp_cb, struct dp_dev_data *data,
			    uint32_t flags)
{
	int res = DP_FAILURE;
	struct pmac_port_info *port_info;
	struct dp_dev_data tmp_data = {0};
	struct cbm_dp_alloc_complete_data *cbm_data = NULL;

	if (unlikely(!dp_init_ok)) {
		pr_err("dp_register_dev failed for datapath not init yet\n");
		return DP_FAILURE;
	}
	if (!data)
		data = &tmp_data;
	if (!port_id || !owner || (port_id >= MAX_DP_PORTS)) {
		if ((inst < 0) || (inst >= DP_MAX_INST))
			DP_DEBUG(DP_DBG_FLAG_REG, "wrong inst=%d\n", inst);
		else if (!owner)
			DP_DEBUG(DP_DBG_FLAG_REG, "owner NULL\n");
		else
			DP_DEBUG(DP_DBG_FLAG_REG, "Wrong port_id:%d\n",
				 port_id);

		return DP_FAILURE;
	}
	port_info = get_dp_port_info(inst, port_id);

	DP_LIB_LOCK(&dp_lock);
	if (flags & DP_F_DEREGISTER) {	/*de-register */
		if (port_info->status != PORT_DEV_REGISTERED) {
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "No or %s to de-register for num_subif=%d\n",
				 owner->name,
				 port_info->num_subif);
		} else if (port_info->status ==
			   PORT_DEV_REGISTERED) {
			port_info->status = PORT_ALLOCATED;
			DP_CB(inst, dev_platform_set)(inst, port_id, data,
						      flags);
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
			remove_umt(inst, &port_info->umt);
#endif
			res = DP_SUCCESS;
		} else {
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "No for %s to de-register for unknown status:%d\n",
				 owner->name, port_info->status);
		}

		DP_LIB_UNLOCK(&dp_lock);
		return res;
	}

	if (port_info->status != PORT_ALLOCATED) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "No de-register for %s for unknown status:%d\n",
			 owner->name, port_info->status);
		DP_LIB_UNLOCK(&dp_lock);
		return DP_FAILURE;
	}

	if (port_info->owner != owner) {
		DP_DEBUG(DP_DBG_FLAG_REG, "No matched owner(%s):%px->%px\n",
			 owner->name, owner, port_info->owner);
		DP_LIB_UNLOCK(&dp_lock);
		return res;
	}

	DP_CB(inst, dev_platform_set)(inst, port_id, data, flags);
	port_info->res_qid_base = data->qos_resv_q_base;
	port_info->num_resv_q = data->num_resv_q;

	cbm_data = kzalloc(sizeof(*cbm_data), GFP_ATOMIC);
	if (!cbm_data) {
		DP_LIB_UNLOCK(&dp_lock);
		return res;
	}

	res = dp_build_cqm_data(inst, port_id, cbm_data, data);
	if (res == DP_FAILURE) {
		kfree(cbm_data);
		DP_LIB_UNLOCK(&dp_lock);
		return res;
	}

	/*register a device */
	if (cbm_dp_port_alloc_complete(owner,
				       port_info->dev,
				       port_info->dev_port,
				       port_id,
				       cbm_data,
				       port_info->alloc_flags)) {
		DP_DEBUG(DP_DBG_FLAG_REG, "CBM port alloc failed\n");
		kfree(cbm_data);
		DP_LIB_UNLOCK(&dp_lock);
		return DP_FAILURE;
	}

	if (data->num_rx_ring || data->num_tx_ring) {
		res = dp_register_dc(inst, port_id, cbm_data, data, flags);
		if (res == DP_FAILURE) {
			kfree(cbm_data);
			DP_LIB_UNLOCK(&dp_lock);
			return res;
		}
	}

	/* TODO: Need a HAL layer API for CQM and DMA Setup for CQM QoS path
	 * especially for LGM 4 Ring case
	 */
	port_info->status = PORT_DEV_REGISTERED;
	if (dp_cb)
		port_info->cb = *dp_cb;
	DP_LIB_UNLOCK(&dp_lock);
	kfree(cbm_data);

	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_register_dev_ext);

/* if subif_id->subif < 0: Dynamic mode
 * else subif is provided by caller itself
 * Note: 1) for register logical device, if DP_F_ALLOC_EXPLICIT_SUBIFID is not
 *       specified, subif will take its base dev's subif.
 *       2) for IPOA/PPPOA, dev is NULL and subif_name is dummy string.
 *          in this case, dev->name may not be subif_name
 */
int32_t dp_register_subif_ext(int inst, struct module *owner,
			      struct net_device *dev,
			      char *subif_name, dp_subif_t *subif_id,
			      /*device related info*/
			      struct dp_subif_data *data, uint32_t flags)
{
	int res = DP_FAILURE;
	int port_id, f_subif = -1;
	struct pmac_port_info *port_info;
	struct dp_subif_data tmp_data = {0};
	dp_subif_t *subif_id_sync;
	dp_get_netif_subifid_fn_t subifid_fn_t = NULL;

	if (unlikely(!dp_init_ok)) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "dp_register_subif fail for datapath not init yet\n");
		return DP_FAILURE;
	}
	DP_DEBUG(DP_DBG_FLAG_REG,
		 "%s:owner=%s dev=%s %s=%s port_id=%d subif=%d(%s) %s%s\n",
		 (flags & DP_F_DEREGISTER) ?
			"unregister subif:" : "register subif",
		 owner ? owner->name : "NULL",
		 dev ? dev->name : "NULL",
		 "subif_name",
		 subif_name,
		 subif_id->port_id,
		 subif_id->subif,
		 (subif_id->subif < 0) ? "dynamic" : "fixed",
		 (flags & DP_F_SUBIF_LOGICAL) ? "Logical" : "",
		 (flags & DP_F_ALLOC_EXPLICIT_SUBIFID) ?
			"Explicit" : "Non-Explicit");

	if ((!subif_id) || (!subif_id->port_id) || (!owner) ||
	    (subif_id->port_id >= MAX_DP_PORTS) ||
	    (subif_id->port_id <= 0) ||
	    ((inst < 0) || (inst >= DP_MAX_INST))) {
		if (!owner)
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "register subif failed for owner NULL\n");
		else if (!subif_id)
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "register subif failed for NULL subif_id\n");
		else if ((inst < 0) || (inst >= DP_MAX_INST))
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "register subif failed for wrong inst=%d\n",
				 inst);
		else
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "register subif failed port_id=%d or others\n",
				 subif_id->port_id);

		return DP_FAILURE;
	}
	port_id = subif_id->port_id;
	port_info = get_dp_port_info(inst, port_id);

	if (((!dev) && !(port_info->alloc_flags & DP_F_FAST_DSL)) ||
	    !subif_name) {
		DP_DEBUG(DP_DBG_FLAG_REG, "Wrong dev=%px, subif_name=%px\n",
			 dev, subif_name);
		return DP_FAILURE;
	}
	if (!data)
		data = &tmp_data;
	DP_LIB_LOCK(&dp_lock);
	if (port_info->owner != owner) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "Unregister subif fail:Not matching:%px(%s)->%px(%s)\n",
			 owner, owner->name, port_info->owner,
			 port_info->owner->name);
		DP_LIB_UNLOCK(&dp_lock);
		return res;
	}

	if (flags & DP_F_DEREGISTER) /*de-register */
		res =
		dp_deregister_subif_private(inst, owner, dev,
					    subif_name,
					    subif_id, data, flags);
	else /*register */
		res =
		dp_register_subif_private(inst, owner, dev,
					  subif_name,
					  subif_id, data, flags);
	if (!(flags & DP_F_SUBIF_LOGICAL))
		subifid_fn_t = port_info->cb.get_subifid_fn;

	subif_id_sync = kmalloc(sizeof(*subif_id_sync) * 2, GFP_ATOMIC);
	if (!subif_id_sync) {
		pr_err("Failed to alloc %zu bytes\n",
		       sizeof(*subif_id_sync) * 2);
		return DP_FAILURE;
	}
	memcpy(&subif_id_sync[0], subif_id, sizeof(dp_subif_t));
	memcpy(&subif_id_sync[1], subif_id, sizeof(dp_subif_t));
	dp_sync_subifid(dev, subif_name, subif_id_sync, data, flags, &f_subif);
	DP_LIB_UNLOCK(&dp_lock);
	if (!res)
		dp_sync_subifid_priv(dev, subif_name, subif_id_sync, data,
				     flags, subifid_fn_t, &f_subif);
	kfree(subif_id_sync);
	return res;
}
EXPORT_SYMBOL(dp_register_subif_ext);

int32_t dp_register_subif(struct module *owner, struct net_device *dev,
			  char *subif_name, dp_subif_t *subif_id,
			  uint32_t flags)
{
	int inst;
	struct dp_subif_data data = {0};

	if ((!subif_id) || (!subif_id->port_id) || (!owner) ||
	    (subif_id->port_id >= MAX_DP_PORTS) ||
	    (subif_id->port_id <= 0)) {
		if (!owner)
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "register subif fail for owner NULL\n");
		else if (!subif_id)
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "register subif fail for NULL subif_id\n");
		else
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "register subif fail port_id=%d or others\n",
				 subif_id->port_id);

		return DP_FAILURE;
	}
	inst = dp_get_inst_via_module(owner, subif_id->port_id, 0);
	if (inst < 0) {
		pr_err("wrong inst for owner=%s with ep=%d\n", owner->name,
		       subif_id->port_id);
		return DP_FAILURE;
	}
	return dp_register_subif_ext(inst, owner, dev, subif_name,
				     subif_id, &data, flags);
}
EXPORT_SYMBOL(dp_register_subif);

int32_t dp_get_netif_subifid(struct net_device *netif, struct sk_buff *skb,
			     void *subif_data, u8 dst_mac[DP_MAX_ETH_ALEN],
			     dp_subif_t *subif, uint32_t flags)
{
	struct dp_subif_cache *dp_subif;
	struct dp_subif_info *sif;
	struct pmac_port_info *port_info;
	u32 idx;
	dp_get_netif_subifid_fn_t subifid_fn_t;
	int res = DP_FAILURE;

	idx = dp_subif_hash(netif);
	//TODO handle DSL case in future
	rcu_read_lock_bh();
	dp_subif = dp_subif_lookup_safe(&dp_subif_list[idx], netif, subif_data);
	if (!dp_subif) {
		pr_err("Failed dp_subif_lookup: %s\n",
		       netif ? netif->name : "NULL");
		rcu_read_unlock_bh();
		return res;
	}
	memcpy(subif, &dp_subif->subif, sizeof(*subif));
	subifid_fn_t = dp_subif->subif_fn;
	/* To return associate VUNI device if subif is VANI
	 */
	if (subif->data_flag & DP_SUBIF_VANI) {
		port_info = get_dp_port_info(subif->inst, subif->port_id);
		sif = get_dp_port_subif(port_info, !subif->subif);
		if ((sif->netif) && (sif->flags))
			subif->associate_netif = sif->netif;
	}
	rcu_read_unlock_bh();
	if (subifid_fn_t) {
		/*subif->subif will be set by callback api itself */
		res = subifid_fn_t(netif, skb, subif_data, dst_mac, subif,
				   flags);
		if (res != 0)
			pr_err("get_netif_subifid callback function failed\n");
	} else {
		res = DP_SUCCESS;
	}
	return res;
}
EXPORT_SYMBOL(dp_get_netif_subifid);

/*Note:
 * try to get subif according to netif, skb,vcc,dst_mac.
 * For DLS nas interface, must provide valid subif_data, otherwise set to NULL.
 * Note: subif_data is mainly used for DSL WAN mode, esp ATM.
 * If subif->port_id valid, take it, otherwise search all to get the port_id
 */
int32_t dp_get_netif_subifid_priv(struct net_device *netif, struct sk_buff *skb,
				  void *subif_data,
				  u8 dst_mac[DP_MAX_ETH_ALEN],
				  dp_subif_t *subif, uint32_t flags)
{
	int res = -1;
	int i, k;
	int port_id = -1;
	u16 bport = 0;
	int inst, start, end;
	u8 match = 0;
	u8 num = 0;
	u16 *subifs = NULL;
	u32 *subif_flag = NULL;
	struct logic_dev *tmp = NULL;
	u16 gpid = 0;

	subifs = kmalloc(sizeof(*subifs) * DP_MAX_CTP_PER_DEV,
			 GFP_ATOMIC);
	if (!subifs) {
		pr_err("Failed to alloc %zu bytes\n",
		       sizeof(*subifs) * DP_MAX_CTP_PER_DEV);
		return res;
	}
	subif_flag = kmalloc(sizeof(*subif_flag) * DP_MAX_CTP_PER_DEV,
			     GFP_ATOMIC);
	if (!subif_flag) {
		pr_err("Failed to alloc %zu bytes\n",
		       sizeof(*subif_flag) * DP_MAX_CTP_PER_DEV);
		kfree(subifs);
		return res;
	}
	if (!netif && !subif_data) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "dp_get_netif_subifid failed: netif=%px subif_data=%px\n",
			 netif, subif_data);
		goto EXIT;
	}
	if (!subif) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "dp_get_netif_subifid failed:subif=%px\n", subif);
		goto EXIT;
	}
	if (!netif && subif_data)
		inst = 0;
	else
		inst = dp_get_inst_via_dev(netif, NULL, 0);
	start = 0;
	end = dp_port_prop[inst].info.cap.max_num_dp_ports;
#ifdef DP_FAST_SEARCH /* Don't enable since need to be back-compatible */
	if ((subif->port_id < MAX_DP_PORTS) && (subif->port_id > 0)) {
		start = subif->port_id;
		end = start + 1;
	}
#endif
	subif->flag_pmapper = 0;
	for (k = start; k < end; k++) {
		struct pmac_port_info *port = get_dp_port_info(inst, k);

		if (port->status != PORT_SUBIF_REGISTERED)
			continue;

		/*Workaround for VRX318 */
		if (subif_data && (port->alloc_flags & DP_F_FAST_DSL)) {
			/*VRX318 should overwritten them later if necessary */
			port_id = k;
			break;
		}

		/*search sub-interfaces/VAP */
		for (i = 0; i < port->subif_max; i++) {
			struct dp_subif_info *sif = get_dp_port_subif(port, i);

			if (!sif->flags)
				continue;
			if (sif->ctp_dev == netif) { /*for PON pmapper case*/
				match = 1;
				port_id = k;
				if (num > 0) {
					pr_err("Multiple same ctp_dev exist\n");
					goto EXIT;
				}
				subifs[num] = sif->subif;
				subif_flag[num] = sif->subif_flag;
				subif->data_flag = sif->data_flag_ops;
				bport = sif->bp;
				subif->flag_bp = 0;
				gpid = sif->gpid;
				subif->num_q = sif->num_qid;
				memcpy(subif->def_qlist, sif->qid_list,
				       sizeof(sif->qid_list));
				num++;
				break;
			}

			if (sif->netif == netif) {
				if ((subif->port_id > 0) &&
				    (subif->port_id != k)) {
					DP_DEBUG(DP_DBG_FLAG_REG,
						 "dp_get_netif_subifid portid not match:%d expect %d\n",
						 subif->port_id, k);
				} else {
					match = 1;
					subif->flag_bp = 1;
					port_id = k;
					if (num >= DP_MAX_CTP_PER_DEV) {
						pr_err("%s: Why CTP over %d\n",
						       netif ? netif->name : "",
						       DP_MAX_CTP_PER_DEV);
						goto EXIT;
					}
					/* some dev may have multiple
					 * subif,like pon
					 */
					subifs[num] = sif->subif;
					gpid = sif->gpid;
					subif->num_q = sif->num_qid;
					memcpy(subif->def_qlist, sif->qid_list,
					       sizeof(sif->qid_list));
					subif_flag[num] = sif->subif_flag;
					subif->data_flag = sif->data_flag_ops;
					if (sif->ctp_dev)
						subif->flag_pmapper = 1;
					bport = sif->bp;
					if (num && (bport != sif->bp)) {
						pr_err("%s:Why many bp:%d %d\n",
						       netif ? netif->name : "",
						       sif->bp, bport);
						goto EXIT;
					}
					num++;
				}
			}
			/*continue search non-explicate logical device */
			list_for_each_entry(tmp, &sif->logic_dev, list) {
				if (tmp->dev == netif) {
					subif->subif_num = 1;
					subif->subif_list[0] = tmp->ctp;
					subif->inst = inst;
					subif->port_id = k;
					subif->bport = tmp->bp;
					subif->gpid = sif->gpid;
					subif->num_q = sif->num_qid;
					subif->data_flag = sif->data_flag_ops;
					memcpy(subif->def_qlist, sif->qid_list,
					       sizeof(sif->qid_list));
					res = 0;
					/*note: logical device no callback */
					goto EXIT;
				}
			}
		}
		if (match)
			break;
	}

	if (port_id < 0) {
		if (subif_data)
			DP_DEBUG(DP_DBG_FLAG_DBG,
				 "dp_get_netif_subifid failed with subif_data %px\n",
				 subif_data);
		else /*netif must should be valid */
			DP_DEBUG(DP_DBG_FLAG_DBG,
				 "dp_get_netif_subifid failed: %s\n",
				 netif->name);

		goto EXIT;
	}
	subif->inst = inst;
	subif->port_id = port_id;
	subif->bport = bport;
	subif->gpid = gpid;
	subif->alloc_flag = get_dp_port_info(inst, port_id)->alloc_flags;
	subif->subif_num = num;
	for (i = 0; i < num; i++) {
		subif->subif_list[i] = subifs[i];
		subif->subif_flag[i] = subif_flag[i];
	}
	res = 0;
EXIT:
	kfree(subifs);
	kfree(subif_flag);
	return res;
}

static int dp_build_cqm_data(int inst, uint32_t port_id,
			     struct cbm_dp_alloc_complete_data *cbm_data,
			     struct dp_dev_data *data)
{
	int i = 0;
	struct pmac_port_info *port = get_dp_port_info(inst, port_id);

	if ((data->num_rx_ring > DP_RX_RING_NUM) ||
	    (data->num_tx_ring > DP_TX_RING_NUM)) {
		pr_err("Error RxRing = %d TxRing = %d\n",
		       data->num_rx_ring, data->num_tx_ring);
		return DP_FAILURE;
	}

	/* HOST -> ACA */
	/* For PRX300 No: of Tx Ring is 1 */
	cbm_data->num_tx_ring = data->num_tx_ring;

	/* TX Ring */
	for (i = 0; i < data->num_tx_ring; i++) {
		memcpy(&cbm_data->tx_ring[i], &data->tx_ring[i],
		       sizeof(struct dp_tx_ring));
	}

	cbm_data->deq_port = port->deq_port_base;

	/* ACA -> HOST */
	/* For PRX300 No: of Rx Ring is 1 */
	cbm_data->num_rx_ring = data->num_rx_ring;

	/* RX ring */
	for (i = 0; i < data->num_rx_ring; i++) {
		memcpy(&cbm_data->rx_ring[i], &data->rx_ring[i],
		       sizeof(struct dp_rx_ring));
	}

	cbm_data->num_qid = data->num_resv_q;
	cbm_data->qid_base = data->qos_resv_q_base;

	return 0;
}

static int dp_register_dc(int inst, uint32_t port_id,
			  struct cbm_dp_alloc_complete_data *cbm_data,
			  struct dp_dev_data *data, uint32_t flags)
{
	struct pmac_port_info *port = get_dp_port_info(inst, port_id);
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	struct umt_port_res *res;
	struct umt_ops *ops = dp_get_umt_ops(inst);
#endif
	int i, ret = DP_SUCCESS;

	/* Fill in the output data to the the DCDP driver for the RX rings
	 * and Save Info for debugging
	 */
	for (i = 0; i < data->num_rx_ring; i++) {
		memcpy(&data->rx_ring[i], &cbm_data->rx_ring[i],
		       sizeof(struct dp_rx_ring));
		memcpy(&port->rx_ring[i], &cbm_data->rx_ring[i],
		       sizeof(struct dp_rx_ring));
	}

	for (i = 0; i < data->num_tx_ring; i++) {
		memcpy(&data->tx_ring[i], &cbm_data->tx_ring[i],
		       sizeof(struct dp_tx_ring));
		memcpy(&port->tx_ring[i], &cbm_data->tx_ring[i],
		       sizeof(struct dp_tx_ring));
	}

	/* Save info for debugging */
	port->num_rx_ring = data->num_rx_ring;
	port->num_tx_ring = data->num_tx_ring;

	/* UMT Interface is not supported for old products */
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
        res = &data->umt->res;
	/* For PRX300, RXOUT is to DMA Channel,
	 * For LGM, RXOUT is to CQEM Deq port
	 */
	res->rx_src     = UMT_RX_SRC_DMA;
	res->dma_id     = data->rx_ring[0].out_dma_ch_to_gswip;
	res->dma_ch_num = data->rx_ring[0].num_out_tx_dma_ch;
	res->cqm_dq_pid = port->deq_port_base;

	if (!ops) {
		pr_err("No UMT driver registered\n");
		return -ENODEV;
	}

	ret = ops->umt_request(ops->umt_dev, data->umt);
	memcpy(&port->umt, data->umt, sizeof(struct dp_umt_port));
	return ret;
#endif
	return ret;
}

/* return DP_SUCCESS -- found
 * return DP_FAILURE -- not found
 */
int dp_get_port_subitf_via_dev_private(struct net_device *dev,
				       dp_subif_t *subif)
{
	int i, j;
	int inst;

	inst = dp_get_inst_via_dev(dev, NULL, 0);
	for (i = 0; i < dp_port_prop[inst].info.cap.max_num_dp_ports; i++) {
		struct pmac_port_info *port = get_dp_port_info(inst, i);

		for (j = 0; j < port->subif_max; j++) {
			struct dp_subif_info *sif = get_dp_port_subif(port, j);

			if (!sif->flags)
				continue;
			if (sif->netif != dev)
				continue;
			subif->port_id = i;
			subif->subif = SET_VAP(j, port->vap_offset,
					       port->vap_mask);
			subif->inst = inst;
			subif->bport = sif->bp;
			return DP_SUCCESS;
		}
	}
	return DP_FAILURE;
}

int dp_get_port_subitf_via_dev(struct net_device *dev, dp_subif_t *subif)
{
	int res;

	DP_LIB_LOCK(&dp_lock);
	res = dp_get_port_subitf_via_dev_private(dev, subif);
	DP_LIB_UNLOCK(&dp_lock);
	return res;
}
EXPORT_SYMBOL(dp_get_port_subitf_via_dev);

int dp_get_port_subitf_via_ifname_private(char *ifname, dp_subif_t *subif)
{
	int i, j;
	int inst;

	inst = dp_get_inst_via_dev(NULL, ifname, 0);

	for (i = 0; i < dp_port_prop[inst].info.cap.max_num_dp_ports; i++) {
		struct pmac_port_info *port = get_dp_port_info(inst, i);

		for (j = 0; j < port->subif_max; j++) {
			struct dp_subif_info *sif = get_dp_port_subif(port, j);

			if (strcmp(sif->device_name, ifname) == 0) {
				subif->port_id = i;
				subif->subif = SET_VAP(j, port->vap_offset,
						       port->vap_mask);
				subif->inst = inst;
				subif->bport = sif->bp;
				return DP_SUCCESS;
			}
		}
	}

	return DP_FAILURE;
}

int dp_get_port_subitf_via_ifname(char *ifname, dp_subif_t *subif)
{
	int res;
	struct net_device *dev;

	if (!ifname)
		return -1;
	dev = dev_get_by_name(&init_net, ifname);
	if (!dev)
		return -1;
	res = dp_get_port_subitf_via_dev(dev, subif);
	dev_put(dev);
	return res;
}
EXPORT_SYMBOL(dp_get_port_subitf_via_ifname);

int32_t dp_check_if_netif_fastpath_fn(struct net_device *netif,
				      dp_subif_t *subif, char *ifname,
				      uint32_t flags)
{
	int res = 0;
	dp_subif_t tmp_subif = { 0 };
	struct pmac_port_info *p_info;
	int max_dp_ports;
	int dp_flags;

	DP_LIB_LOCK(&dp_lock);
	if (unlikely(!dp_init_ok)) {
		pr_err("dp_check_if_netif_fastpath_fn fail: dp not ready\n");
		return DP_FAILURE;
	}
	if (subif) {
		tmp_subif = *subif;
		tmp_subif.inst = 0;
	} else if (netif) {
		dp_get_port_subitf_via_dev_private(netif, &tmp_subif);
	} else if (ifname) {
		dp_get_port_subitf_via_ifname_private(ifname, &tmp_subif);
	}
	max_dp_ports = dp_port_prop[tmp_subif.inst].info.cap.max_num_dp_ports;
	if (tmp_subif.port_id <= 0 || tmp_subif.port_id >= max_dp_ports)
		goto EXIT;
	p_info = get_dp_port_info(tmp_subif.inst, tmp_subif.port_id);
	dp_flags = DP_F_FAST_DSL || DP_F_FAST_ETH_LAN ||
		   DP_F_FAST_ETH_WAN || DP_F_FAST_WLAN;
	if (!(p_info->alloc_flags & dp_flags))
		goto EXIT;
	res = 1;
EXIT:
	DP_LIB_UNLOCK(&dp_lock);
	return res;
}
EXPORT_SYMBOL(dp_check_if_netif_fastpath_fn);

struct module *dp_get_module_owner(int ep)
{
	int inst = 0; /*here hardcode for PPA only */

	if (unlikely(!dp_init_ok)) {
		pr_err("dp_get_module_owner failed for dp not init yet\n");
		return NULL;
	}

	if ((ep >= 0) && (ep < dp_port_prop[inst].info.cap.max_num_dp_ports))
		return get_dp_port_info(inst, ep)->owner;

	return NULL;
}
EXPORT_SYMBOL(dp_get_module_owner);

/*if subif->vap == -1, it means all vap */
void dp_clear_mib(dp_subif_t *subif, uint32_t flag)
{
	int i, j, start_vap, end_vap;
	dp_reset_mib_fn_t reset_mib_fn;
	struct pmac_port_info *port_info;

	if (!subif || (subif->port_id >= MAX_DP_PORTS) ||
	    (subif->port_id < 0)) {
		DP_DEBUG(DP_DBG_FLAG_DBG, "dp_clear_mib Wrong subif\n");
		return;
	}

	i = subif->port_id;
	port_info = get_dp_port_info(subif->inst, i);

	if (subif->subif == -1) {
		start_vap = 0;
		end_vap = port_info->num_subif;
	} else {
		start_vap = GET_VAP(subif->subif, port_info->vap_offset,
				    port_info->vap_mask);
		end_vap = start_vap + 1;
	}

	for (j = start_vap; j < end_vap; j++) {
		struct dp_subif_info *sif = get_dp_port_subif(port_info, i);
		struct dev_mib *mib = get_dp_port_subif_mib(sif);

		STATS_SET(port_info->tx_err_drop, 0);
		STATS_SET(port_info->rx_err_drop, 0);
		memset(mib, 0, sizeof(struct dev_mib));
		reset_mib_fn = port_info->cb.reset_mib_fn;

		if (reset_mib_fn)
			reset_mib_fn(subif, 0);
	}
}

void dp_clear_all_mib_inside(uint32_t flag)
{
	dp_subif_t subif;
	int i;

	memset(&subif, 0, sizeof(subif));
	for (i = 0; i < MAX_DP_PORTS; i++) {
		subif.port_id = i;
		subif.subif = -1;
		dp_clear_mib(&subif, flag);
	}
}

int dp_get_drv_mib(dp_subif_t *subif, dp_drv_mib_t *mib, uint32_t flag)
{
	dp_get_mib_fn_t get_mib_fn;
	dp_drv_mib_t tmp;
	int i, vap;
	struct pmac_port_info *port_info;
	struct dp_subif_info *sif;

	if (unlikely(!dp_init_ok)) {
		DP_DEBUG(DP_DBG_FLAG_DBG,
			 "dp_get_drv_mib failed for datapath not init yet\n");
		return DP_FAILURE;
	}

	if (!subif || !mib)
		return -1;
	memset(mib, 0, sizeof(*mib));
	port_info = get_dp_port_info(subif->inst, subif->port_id);
	vap = GET_VAP(subif->subif, port_info->vap_offset,
		      port_info->vap_mask);
	get_mib_fn = port_info->cb.get_mib_fn;

	if (!get_mib_fn)
		return -1;

	if (!(flag & DP_F_STATS_SUBIF)) {
		/*get all VAP's  mib counters if it is -1 */
		for (i = 0; i < port_info->num_subif; i++) {
			sif = get_dp_port_subif(port_info, i);
			if (!sif->flags)
				continue;

			subif->subif = sif->subif;
			memset(&tmp, 0, sizeof(tmp));
			get_mib_fn(subif, &tmp, flag);
			mib->rx_drop_pkts += tmp.rx_drop_pkts;
			mib->rx_error_pkts += tmp.rx_error_pkts;
			mib->tx_drop_pkts += tmp.tx_drop_pkts;
			mib->tx_error_pkts += tmp.tx_error_pkts;
		}
	} else {
		sif = get_dp_port_subif(port_info, vap);
		if (sif->flags)
			get_mib_fn(subif, mib, flag);
	}

	return 0;
}

int dp_get_netif_stats(struct net_device *dev, dp_subif_t *subif_id,
		       struct rtnl_link_stats64 *stats, uint32_t flags)
{
	dp_subif_t subif;
	int res;
	int (*get_mib)(dp_subif_t *subif_id, void *priv,
		       struct rtnl_link_stats64 *stats,
		       uint32_t flags);

	if (subif_id) {
		subif = *subif_id;
	} else if (dev) {
		res = dp_get_port_subitf_via_dev(dev, &subif);
		if (res) {
			DP_DEBUG(DP_DBG_FLAG_MIB,
				 "dp_get_netif_stats fail:%s not registered yet to datapath\n",
				 dev->name);
			return DP_FAILURE;
		}
	} else {
		DP_DEBUG(DP_DBG_FLAG_MIB,
			 "dp_get_netif_stats: dev/subif_id both NULL\n");
		return DP_FAILURE;
	}
	get_mib = dp_port_prop[subif.inst].info.dp_get_port_vap_mib;
	if (!get_mib)
		return DP_FAILURE;

	return get_mib(&subif, NULL, stats, flags);
}
EXPORT_SYMBOL(dp_get_netif_stats);

int dp_clear_netif_stats(struct net_device *dev, dp_subif_t *subif_id,
			 uint32_t flag)
{
	dp_subif_t subif;
	int (*clear_netif_mib_fn)(dp_subif_t *subif, void *priv, u32 flag);
	int i;

	if (subif_id) {
		clear_netif_mib_fn =
			dp_port_prop[subif_id->inst].info.dp_clear_netif_mib;
		if (!clear_netif_mib_fn)
			return -1;
		return clear_netif_mib_fn(subif_id, NULL, flag);
	}
	if (dev) {
		if (dp_get_port_subitf_via_dev(dev, &subif)) {
			DP_DEBUG(DP_DBG_FLAG_MIB,
				 "%s not register to dp_clear_netif_stats\n",
				 dev->name);
			return -1;
		}
		clear_netif_mib_fn =
			dp_port_prop[subif.inst].info.dp_clear_netif_mib;
		if (!clear_netif_mib_fn)
			return -1;
		return clear_netif_mib_fn(&subif, NULL, flag);
	}
	/*clear all */
	for (i = 0; i < DP_MAX_INST; i++) {
		clear_netif_mib_fn =
			dp_port_prop[i].info.dp_clear_netif_mib;
		if (!clear_netif_mib_fn)
			continue;
		clear_netif_mib_fn(NULL, NULL, flag);
	}
	return 0;
}
EXPORT_SYMBOL(dp_clear_netif_stats);

int dp_pmac_set(int inst, u32 port, dp_pmac_cfg_t *pmac_cfg)
{
	int (*dp_pmac_set_fn)(int inst, u32 port, dp_pmac_cfg_t *pmac_cfg);

	if (inst >= DP_MAX_INST) {
		pr_err("Wrong inst(%d) id: should less than %d\n",
		       inst, DP_MAX_INST);
		return DP_FAILURE;
	}
	dp_pmac_set_fn = dp_port_prop[inst].info.dp_pmac_set;
	if (!dp_pmac_set_fn)
		return DP_FAILURE;
	return dp_pmac_set_fn(inst, port, pmac_cfg);
}
EXPORT_SYMBOL(dp_pmac_set);

/*\brief Datapath Manager Pmapper Configuration Set
 *\param[in] dev: network device point to set pmapper
 *\param[in] mapper: buffer to get pmapper configuration
 *\param[in] flag: reserve for future
 *\return Returns 0 on succeed and -1 on failure
 *\note  for pcp mapper case, all 8 mapping must be configured properly
 *       for dscp mapper case, all 64 mapping must be configured properly
 *       def ctp will match non-vlan and non-ip case
 *	For drop case, assign CTP value == DP_PMAPPER_DISCARD_CTP
 */
int dp_set_pmapper(struct net_device *dev, struct dp_pmapper *mapper, u32 flag)
{
	int inst, ret, bport, i;
	dp_subif_t subif = {0};
	struct dp_pmapper *map = NULL;
	int res = DP_FAILURE;

	if (!dev || !mapper) {
		pr_err("dev or mapper is NULL\n");
		return DP_FAILURE;
	}

	if (unlikely(!dp_init_ok)) {
		pr_err("Failed for datapath not init yet\n");
		return DP_FAILURE;
	}
	if (mapper->mode >= DP_PMAP_MAX) {
		pr_err("mapper->mode(%d) out of range %d\n",
		       mapper->mode, DP_PMAP_MAX);
		return DP_FAILURE;
	}
	/* get the subif from the dev */
	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0);
	if (ret == DP_FAILURE) {
		pr_err("Fail to get subif:dev=%s ret=%d flag_pmap=%d bp=%d\n",
		       dev->name, ret, subif.flag_pmapper, subif.bport);
		return DP_FAILURE;
	}
	inst = subif.inst;
	if (!dp_port_prop[inst].info.dp_set_gsw_pmapper) {
		pr_err("Set pmapper is not supported\n");
		return DP_FAILURE;
	}

	bport = subif.bport;
	if (bport >= DP_MAX_BP_NUM) {
		pr_err("BP port(%d) out of range %d\n", bport, DP_MAX_BP_NUM);
		return DP_FAILURE;
	}
	map = kmalloc(sizeof(*map), GFP_ATOMIC);
	if (!map) {
		pr_err("Failed for kmalloc: %zu bytes\n", sizeof(*map));
		return DP_FAILURE;
	}
	memcpy(map, mapper, sizeof(*map));
	if ((mapper->mode == DP_PMAP_PCP) ||
	    (mapper->mode == DP_PMAP_DSCP)) {
		map->mode = GSW_PMAPPER_MAPPING_PCP;
	} else if (mapper->mode == DP_PMAP_DSCP_ONLY) {
		map->mode = GSW_PMAPPER_MAPPING_DSCP;
	} else {
		pr_err("Unknown mapper mode: %d\n", map->mode);
		goto EXIT;
	}

	/* workaround in case caller forget to set to default ctp */
	if (mapper->mode == DP_PMAP_PCP)
		for (i = 0; i < DP_PMAP_DSCP_NUM; i++)
			map->dscp_map[i] = mapper->def_ctp;

	ret = dp_port_prop[inst].info.dp_set_gsw_pmapper(inst, bport,
							 subif.port_id, map,
							 flag);
	if (ret == DP_FAILURE) {
		pr_err("Failed to set mapper\n");
		goto EXIT;
	}

	/* update local table for pmapper */
	dp_bp_dev_tbl[inst][bport].def_ctp = map->def_ctp;
	dp_bp_dev_tbl[inst][bport].mode = mapper->mode; /* original mode */
	for (i = 0; i < DP_PMAP_PCP_NUM; i++)
		dp_bp_dev_tbl[inst][bport].pcp[i] = map->pcp_map[i];
	for (i = 0; i < DP_PMAP_DSCP_NUM; i++)
		dp_bp_dev_tbl[inst][bport].dscp[i] = map->dscp_map[i];
	res = DP_SUCCESS;
EXIT:
	kfree(map);
	return res;
}
EXPORT_SYMBOL(dp_set_pmapper);

/*\brief Datapath Manager Pmapper Configuration Get
 *\param[in] dev: network device point to set pmapper
 *\param[out] mapper: buffer to get pmapper configuration
 *\param[in] flag: reserve for future
 *\return Returns 0 on succeed and -1 on failure
 *\note  for pcp mapper case, all 8 mapping must be configured properly
 *       for dscp mapper case, all 64 mapping must be configured properly
 *       def ctp will match non-vlan and non-ip case
 *	 For drop case, assign CTP value == DP_PMAPPER_DISCARD_CTP
 */
int dp_get_pmapper(struct net_device *dev, struct dp_pmapper *mapper, u32 flag)
{
	int inst, ret, bport;
	dp_subif_t subif = {0};

	if (!dev || !mapper) {
		pr_err("The parameter dev or mapper can not be NULL\n");
		return DP_FAILURE;
	}

	if (unlikely(!dp_init_ok)) {
		pr_err("Failed for datapath not init yet\n");
		return DP_FAILURE;
	}

	/*get the subif from the dev*/
	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0);
	if (ret == DP_FAILURE) {
		pr_err("Can not get the subif from the dev\n");
		return DP_FAILURE;
	}
	inst = subif.inst;
	if (!dp_port_prop[inst].info.dp_get_gsw_pmapper) {
		pr_err("Get pmapper is not supported\n");
		return DP_FAILURE;
	}

	bport = subif.bport;
	if (bport > DP_MAX_BP_NUM) {
		pr_err("BP port(%d) out of range %d\n", bport, DP_MAX_BP_NUM);
		return DP_FAILURE;
	}
	/* init the subif into the dp_port_info*/
	/* call the switch api to get the HW*/
	ret = dp_port_prop[inst].info.dp_get_gsw_pmapper(inst, bport,
							 subif.port_id, mapper,
							 flag);
	if (ret == DP_FAILURE) {
		pr_err("Failed to get mapper\n");
		return DP_FAILURE;
	}
	return ret;
}
EXPORT_SYMBOL(dp_get_pmapper);

int32_t dp_rx(struct sk_buff *skb, uint32_t flags)
{
	struct sk_buff *next;
	int res = -1;
	int inst = 0;

	if (unlikely(!dp_init_ok)) {
		while (skb) {
			next = skb->next;
			skb->next = 0;
			dev_kfree_skb_any(skb);
			skb = next;
		}
	}

	while (skb) {
		next = skb->next;
		skb->next = 0;
		res = dp_port_prop[inst].info.dp_rx(skb, flags);
		skb = next;
	}

	return res;
}
EXPORT_SYMBOL(dp_rx);

int dp_lan_wan_bridging(int port_id, struct sk_buff *skb)
{
	dp_subif_t subif;
	struct net_device *dev;
	static int lan_port = 4;
	int inst = 0;
	struct dp_subif_info *sif;

	if (!skb)
		return DP_FAILURE;

	skb_pull(skb, 8);	/*remove pmac */

	memset(&subif, 0, sizeof(subif));
	if (port_id == 15) {
		/*recv from WAN and forward to LAN via lan_port */
		subif.port_id = lan_port;	/*send to last lan port */
		subif.subif = 0;
	} else if (port_id <= 6) { /*recv from LAN and forward to WAN */
		subif.port_id = 15;
		subif.subif = 0;
		lan_port = port_id;	/*save lan port id */
	} else {
		dev_kfree_skb_any(skb);
		return DP_FAILURE;
	}

	sif = get_dp_port_subif(get_dp_port_info(inst, subif.port_id), 0);
	dev = sif->netif;

	if (!sif->flags || !dev) {
		dev_kfree_skb_any(skb);
		return DP_FAILURE;
	}

	((struct dma_tx_desc_1 *)&skb->DW1)->field.ep = subif.port_id;
	((struct dma_tx_desc_0 *)&skb->DW0)->field.dest_sub_if_id =
	    subif.subif;

	dp_xmit(dev, &subif, skb, skb->len, 0);
	return DP_SUCCESS;
}

void set_chksum(struct pmac_tx_hdr *pmac, u32 tcp_type,
		u32 ip_offset, int ip_off_hw_adjust,
		u32 tcp_h_offset)
{
	pmac->tcp_type = tcp_type;
	pmac->ip_offset = ip_offset + ip_off_hw_adjust;
	pmac->tcp_h_offset = tcp_h_offset >> 2;
}

int32_t dp_xmit(struct net_device *rx_if, dp_subif_t *rx_subif,
		struct sk_buff *skb, int32_t len, uint32_t flags)
{
	int32_t res;
	int inst = 0;

	res = dp_port_prop[inst].info.dp_tx(rx_if, rx_subif, skb, len, flags);

	return res;
}
EXPORT_SYMBOL(dp_xmit);

void set_dp_dbg_flag(uint32_t flags)
{
	dp_dbg_flag = flags;
}

uint32_t get_dp_dbg_flag(void)
{
	return dp_dbg_flag;
}

/*!
 *@brief  The API is for dp_get_cap
 *@param[in,out] cap dp_cap pointer, caller must provide the buffer
 *@param[in] flag for future
 *@return 0 if OK / -1 if error
 */
int dp_get_cap(struct dp_cap *cap, int flag)
{
	if (!cap)
		return DP_FAILURE;
	if ((cap->inst < 0) || (cap->inst >= DP_MAX_INST))
		return DP_FAILURE;
	if (!hw_cap_list[cap->inst].valid)
		return DP_FAILURE;
	*cap = hw_cap_list[cap->inst].info.cap;

	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_get_cap);

int dp_set_min_frame_len(s32 dp_port,
			 s32 min_frame_len,
			 uint32_t flags)
{
	pr_info("Dummy dp_set_min_frame_len, need to implement later\n");
	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_set_min_frame_len);

int dp_rx_enable(struct net_device *netif, char *ifname, uint32_t flags)
{
	dp_subif_t subif = {0};
	struct pmac_port_info *port_info;
	int vap;

	if (dp_get_netif_subifid(netif, NULL, NULL, NULL, &subif, 0)) {
		DP_DEBUG(DP_DBG_FLAG_DBG, "get subifid fail(%s)\n",
			 netif ? netif->name : "NULL");
		return DP_FAILURE;
	}
	port_info = get_dp_port_info(subif.inst, subif.port_id);
	vap = GET_VAP(subif.subif, port_info->vap_offset,
		      port_info->vap_mask);

	STATS_SET(get_dp_port_subif(port_info, vap)->rx_flag, flags ? 1 : 0);

	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_rx_enable);

int dp_vlan_set(struct dp_tc_vlan *vlan, int flags)
{
	dp_subif_t subif = {0};
	struct dp_tc_vlan_info info = {0};
	struct pmac_port_info *port_info;

	if (dp_get_netif_subifid(vlan->dev, NULL, NULL, NULL, &subif, 0))
		return DP_FAILURE;
	port_info = get_dp_port_info(subif.inst, subif.port_id);
	info.subix = GET_VAP(subif.subif, port_info->vap_offset,
			     port_info->vap_mask);
	info.bp = subif.bport;
	info.dp_port = subif.port_id;
	info.inst = subif.inst;

	if ((vlan->def_apply == DP_VLAN_APPLY_CTP) &&
	    (subif.flag_pmapper == 1)) {
		pr_err("cannot apply VLAN rule for pmapper device\n");
		return DP_FAILURE;
	} else if (vlan->def_apply == DP_VLAN_APPLY_CTP) {
		info.dev_type = 0;
	} else {
		info.dev_type |= subif.flag_bp;
	}
	switch (vlan->mcast_flag) {
	case DP_MULTICAST_SESSION:
		info.dev_type |= BIT(1);
		break;
	case DP_NON_MULTICAST_SESSION:
		info.dev_type |= BIT(2);
		break;
	}
	DP_DEBUG(DP_DBG_FLAG_PAE, "dev_type:0x%x\n", info.dev_type);
	if (DP_CB(subif.inst, dp_tc_vlan_set))
		return DP_CB(subif.inst, dp_tc_vlan_set)
			    (dp_port_prop[subif.inst].ops[0],
			     vlan, &info, flags);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_vlan_set);

int dp_set_bp_attr(struct dp_bp_attr *conf, uint32_t flag)
{
	struct pmac_port_info *port_info;
	dp_subif_t subif = {0};
	int ret = DP_SUCCESS;

	if (!conf) {
		DP_DEBUG(DP_DBG_FLAG_DBG, "conf passed is (%s)\n",
			 conf ? conf->dev->name : "NULL");
		return DP_FAILURE;
	}

	DP_LIB_LOCK(&dp_lock);

	if (dp_get_netif_subifid_priv(conf->dev, NULL, NULL, NULL, &subif, 0)) {
		DP_LIB_UNLOCK(&dp_lock);
		return DP_FAILURE;
	}

	port_info = get_dp_port_info(conf->inst, subif.port_id);
	port_info->subif_info->cpu_port_en = conf->en;

	DP_LIB_UNLOCK(&dp_lock);

	/* Null check is needed since some platforms dont have this API */
	if (!dp_port_prop[subif.inst].info.dp_set_bp_attr)
		return DP_FAILURE;

	ret = dp_port_prop[subif.inst].info.dp_set_bp_attr(conf, subif.bport,
			flag);

	return ret;
}
EXPORT_SYMBOL(dp_set_bp_attr);

/*Return the table entry index based on dev:
 *success: >=0
 *fail: DP_FAILURE
 */
int bp_pmapper_dev_get(int inst, struct net_device *dev)
{
	int i;

	if (!dev)
		return -1;
	for (i = 0; i < ARRAY_SIZE(dp_bp_dev_tbl[inst]); i++) {
		if (!dp_bp_dev_tbl[inst][i].flag)
			continue;
		if (dp_bp_dev_tbl[inst][i].dev == dev) {
			DP_DEBUG(DP_DBG_FLAG_PAE, "matched %s\n", dev->name);
			return i;
		}
	}
	return -1;
}

#ifdef DP_TEST_EXAMPLE
void test(void)
{
	/* Base on below example data, it should print like below log
	 *DMA Descripotr:D0=0x00004000 D1=0x00001000 D2=0xa0c02080 D3=0xb0000074
	 *DW0:resv0=0 tunnel_id=00 flow_id=0 eth_type=0 dest_sub_if_id=0x4000
	 *DW1:session_id=0x000 tcp_err=0 nat=0 dec=0 enc=0 mpe2=0 mpe1=0
	 *color=01 ep=00 resv1=0 classid=00
	 *DW2:data_ptr=0xa0c02080
	 *DW3:own=1 c=0 sop=1 eop=1 dic=0 pdu_type=0
	 *byte_offset=0 atm_qid=0 mpoa_pt=0 mpoa_mode=0 data_len= 116
	 *paser flags: 00 00 00 00 80 18 80 00
	 *paser flags: 00 80 18 80 00 00 00 00 (reverse)
	 *flags 15 offset=14: PASER_FLAGS_1IPV4
	 *flags 19 offset=22: PASER_FLAGS_ROUTEXP
	 *flags 20 offset=34: PASER_FLAGS_TCP
	 *flags 31 offset=46: PASER_FLAGS_LROEXP
	 *pmac:0x4e 0x28 0xf0 0x00 0x00 0x00 0x00 0x01
	 *byte 0:res=0 ver_done =1 ip_offset=14
	 *byte 1:tcp_h_offset=5 tcp_type=0
	 *byte 2:ppid=15 class=0
	 *byte 3:res=0 pkt_type=0
	 *byte 4:res=0 redirect=0 res2=0 src_sub_inf_id=0
	 *byte 5:src_sub_inf_id2=0
	 *byte 6:port_map=0
	 *byte 7:port_map2=1
	 */
#if IS_ENABLED(CONFIG_LITTLE_ENDIAN)
	char example_data[] = {
		0x00, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e,
		0x00, 0x00, 0x00, 0x16, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x2e,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x80, 0x18, 0x80, 0x00,
		0x00, 0xf0, 0x28, 0x4e, 0x01, 0x00, 0x00, 0x00, 0xaa, 0x00,
		0x00, 0x00, 0x04, 0x03, 0xbb, 0x00,
		0x00, 0x00, 0x04, 0x02, 0x08, 0x00, 0x45, 0x00, 0x00, 0x2e,
		0x00, 0x00, 0x00, 0x00, 0x01, 0x06,
		0xb9, 0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x04, 0x00, 0xb2, 0x9a, 0x03, 0xde
	};
#else
	char example_data[] = {
		0x00, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e,
		0x00, 0x00, 0x00, 0x16, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x2e,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x80, 0x18, 0x80, 0x00,
		0x4e, 0x28, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xaa, 0x00,
		0x00, 0x00, 0x04, 0x03, 0xbb, 0x00,
		0x00, 0x00, 0x04, 0x02, 0x08, 0x00, 0x45, 0x00, 0x00, 0x2e,
		0x00, 0x00, 0x00, 0x00, 0x01, 0x06,
		0xb9, 0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x04, 0x00, 0xb2, 0x9a, 0x03, 0xde
	};
#endif
	struct sk_buff skb;

	skb.DW0 = 0x4000;
	skb.DW1 = 0x1000;
	skb.DW2 = 0xa0c02080;
	skb.DW3 = 0xb0000074;
	skb.data = example_data;
	skb.len = sizeof(example_data);
	dp_rx(&skb, 0);
}
#endif				/* DP_TEST_EXAMPLE */

int dp_free_buffer_by_policy(struct dp_buffer_info *info, u32 flag)
{
	struct cqm_bm_free data = {0};
	int ret;

	if (!info)
		return DP_FAILURE;

	data.flag = flag;
	data.buf = (void *)info->addr;
	data.policy_base = info->policy_base;
	data.policy_num = info->policy_num;

	ret = cqm_buffer_free_by_policy(&data);
	if (ret != CBM_OK) {
		pr_err("cqm_buffer_free_by_policy failed with %d\n", ret);
		return DP_FAILURE;
	}
	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_free_buffer_by_policy);

int dp_basic_proc(void)
{
	struct dentry *p_node;

	/*mask to reset some field as SWAS required  all others try to keep */
	memset(dp_port_prop, 0, sizeof(dp_port_prop));
	memset(dp_port_info, 0, sizeof(dp_port_info));
	p_node = dp_proc_install();
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_LOOPETH)
	dp_loop_eth_dev_init(p_node);
#endif
	dp_inst_init(0);
	dp_subif_list_init();
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	dp_switchdev_init();
#endif
	if (dp_tx_init(0))
		return -ENOMEM;
	return 0;
}

/*static __init */ int dp_init_module(void)
{
	int res = 0;

	if (dp_init_ok) /*alredy init */
		return 0;
	register_notifier(0);
	register_dp_cap(0);
	if (request_dp(0)) /*register 1st dp instance */ {
		pr_err("register_dp instance fail\n");
		return -1;
	}
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
	pr_info("preempt_count=%x\n", preempt_count());
	if (preempt_count() & HARDIRQ_MASK)
		pr_info("HARDIRQ_MASK\n");
	if (preempt_count() & SOFTIRQ_MASK)
		pr_info("SOFTIRQ_MASK\n");
	if (preempt_count() & NMI_MASK)
		pr_info("NMI_MASK\n");
#endif
	dp_init_ok = 1;

	return res;
}

/*static __exit*/ void dp_cleanup_module(void)
{
	int i;

	if (dp_init_ok) {
		DP_LIB_LOCK(&dp_lock);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB)
		dp_mib_exit();
#endif
		for (i = 0; i < dp_inst_num; i++) {
			DP_CB(i, dp_platform_set)(i, DP_PLATFORM_DE_INIT);
			free_dma_chan_tbl(i);
			free_dp_port_subif_info(i);
		}
		dp_init_ok = 0;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_LOOPETH)
		dp_loop_eth_dev_exit();
#endif
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_CPUFREQ)
		dp_cpufreq_notify_exit();
#endif
		unregister_notifier(0);
	}
}

/*!
*@brief get network device's MTU
*@param[in] dev: network device pointer
*@param[out] mtu_size: return the MTU value in the specified memory if valid.
*@Returns DP_SUCCESS on succeed and DP_FAILURE on failure
*/
/* This API will be enhanced later based on netdevice */
int dp_get_mtu_size(struct net_device *dev, u32 *mtu_size)
{
	return cbm_get_mtu_size(mtu_size);
}
EXPORT_SYMBOL(dp_get_mtu_size);

MODULE_LICENSE("GPL");

static int __init dp_dbg_lvl_set(char *str)
{
	pr_info("\n\ndp_dbg=%s\n\n", str);
	dp_dbg_flag = dp_atoi(str);

	return 0;
}

early_param("dp_dbg", dp_dbg_lvl_set);

