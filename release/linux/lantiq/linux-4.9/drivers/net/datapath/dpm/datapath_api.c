// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 * Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 * Copyright 2012 - 2014 Lantiq Deutschland GmbH
 *
 *****************************************************************************/

#include <net/datapath_api.h>
#include "datapath.h"
#include "datapath_tx.h"
#include "datapath_instance.h"
#include "datapath_swdev_api.h"

#if defined(CONFIG_INTEL_DATAPATH_DBG) && CONFIG_INTEL_DATAPATH_DBG
u32 dp_max_print_num = -1, dp_print_num_en = 0;
#endif

u32 dp_rx_test_mode = DP_RX_MODE_NORMAL;
u32 rx_desc_mask[4];
u32 tx_desc_mask[4];
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
struct dma_rx_desc_1 dma_rx_desc_mask1;
struct dma_rx_desc_2 dma_rx_desc_mask2;
struct dma_rx_desc_3 dma_rx_desc_mask3;
struct dma_rx_desc_0 dma_tx_desc_mask0;
struct dma_rx_desc_1 dma_tx_desc_mask1;
#endif

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

int dp_init_ok;
int dp_cpu_init_ok;
atomic_t dp_status = ATOMIC_INIT(0);
DP_DEFINE_LOCK(dp_lock);
u32 dp_dbg_err = 1; /*print error */
EXPORT_SYMBOL(dp_dbg_err);

/* saving Globally, reinsert cqm deqport, to retrieve info later */
u32 reinsert_deq_port = 0;

static int dp_register_dc(int inst, u32 port_id,
			  struct dp_dev_data *data, u32 flags);

static int dp_build_cqm_data(int inst, u32 port_id,
			     struct cbm_dp_alloc_complete_data *cbm_data,
			     struct dp_dev_data *data);

struct platform_device *g_dp_dev;
/*port 0 is reserved and never assigned to any one */
int dp_inst_num;
/* Keep per DP instance information here */
struct inst_property dp_port_prop[DP_MAX_INST];
/* Keep all subif information per instance/LPID/subif */
struct pmac_port_info *dp_port_info[DP_MAX_INST];

/* dp_bp_tbl[] is mainly for PON case.
 * Only if multiple gem port are attached to same bridge port,
 * ie, both dev and data->ctp_dev are provided when calling dp_register_subif_ext API
 * This bridge port device will be recorded into this dp_bp_tbl[].
 * later other information, like pmapper ID/mapping table will be put here also
 */
struct bp_pmapper dp_bp_tbl[DP_MAX_INST][DP_MAX_BP_NUM];
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

/* Per CPU gobal Rx and Tx counters for DPM */
DEFINE_PER_CPU_SHARED_ALIGNED(struct mib_global_stats, mib_g_stats);

void (*dp_dev_get_ethtool_stats_fn)(struct net_device *dev,
				    struct ethtool_stats *stats,
				    u64 *data) = NULL;
EXPORT_SYMBOL(dp_dev_get_ethtool_stats_fn);

void dp_set_ethtool_stats_fn(int inst, void (*cb)(struct net_device *dev,
			     struct ethtool_stats *stats, u64 *data))
{
	dp_dev_get_ethtool_stats_fn = cb;
}
EXPORT_SYMBOL(dp_set_ethtool_stats_fn);

int (*dp_get_dev_stat_strings_count_fn)(struct net_device *dev) = NULL;
EXPORT_SYMBOL(dp_get_dev_stat_strings_count_fn);

void dp_set_ethtool_stats_strings_cnt_fn(int inst,
					 int (*cb)(struct net_device *dev))
{
	dp_get_dev_stat_strings_count_fn = cb;
}
EXPORT_SYMBOL(dp_set_ethtool_stats_strings_cnt_fn);

void (*dp_get_dev_ss_stat_strings_fn)(struct net_device *dev,
				      u8 *data) = NULL;
EXPORT_SYMBOL(dp_get_dev_ss_stat_strings_fn);

void dp_set_ethtool_stats_strings_fn(int inst, void (*cb)(struct net_device *dev,
				     u8 *data))
{
	dp_get_dev_ss_stat_strings_fn = cb;
}
EXPORT_SYMBOL(dp_set_ethtool_stats_strings_fn);

int dp_register_ops(int inst, enum DP_OPS_TYPE type, void *ops)
{
	if (is_invalid_inst(inst) || type >= DP_OPS_CNT) {
		DP_DEBUG(DP_DBG_FLAG_REG, "wrong index\n");
		return DP_FAILURE;
	}
	dp_ops[inst][type] = ops;

	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_register_ops);

void *dp_get_ops(int inst, enum DP_OPS_TYPE type)
{
	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return NULL;
	}

	if (is_invalid_inst(inst) || type >= DP_OPS_CNT) {
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

void dp_print_err_info(int res)
{
	switch (res) {
	case DP_ERR_SUBIF_NOT_FOUND:
		pr_err("DPM subif not found\n");
		break;
	case DP_ERR_INIT_FAIL:
		pr_err("DPM init not done\n");
		break;
	case DP_ERR_INVALID_PORT_ID:
		pr_err("DPM invalid port id\n");
		break;
	case DP_ERR_MEM:
		pr_err("DPM memory allocation failure\n");
		break;
	case DP_ERR_NULL_DATA:
		pr_err("DPM exp data info is NULL\n");
		break;
	case DP_ERR_INVALID_SUBIF:
		pr_err("DPM invalid subif\n");
		break;
	case DP_ERR_DEFAULT:
		pr_err("DPM other generic error\n");
		break;
	default:
		pr_err("DPM why come to here??(%s)\n", __func__);
		break;
	}
}

u32 *get_port_flag(int inst, int index)
{
	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return NULL;
	}

	if (is_invalid_inst(inst))
		return NULL;

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
		    port->port_id == dp_port)
			return port;
	}

	return NULL;
}

int8_t parser_size(int8_t v)
{
	int ret;

	switch (v) {
	case DP_PARSER_F_DISABLE:
		ret = 0;
		break;
	case DP_PARSER_F_HDR_ENABLE:
		ret = PASAR_OFFSETS_NUM;
		break;
	case DP_PARSER_F_HDR_OFFSETS_ENABLE:
		ret = PASAR_OFFSETS_NUM + PASAR_FLAGS_NUM;
		break;
	default:
		pr_err("Wrong parser setting: %d\n", v);
		ret = -1;
		break;
	}

	return ret;
}

/*Only for SOC side, not for peripheral device side */
int dp_set_gsw_parser(u8 flag, u8 cpu, u8 mpe1, u8 mpe2, u8 mpe3)
{
	int inst = 0;
	struct inst_info *dp_info;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	dp_info = get_dp_prop_info(inst);
	if (!dp_info->dp_set_gsw_parser)
		return -1;

	return dp_info->dp_set_gsw_parser(flag, cpu, mpe1, mpe2, mpe3);
}
EXPORT_SYMBOL(dp_set_gsw_parser);

int dp_get_gsw_parser(u8 *cpu, u8 *mpe1, u8 *mpe2, u8 *mpe3)
{
	int inst = 0;
	struct inst_info *dp_info;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	dp_info = get_dp_prop_info(inst);
	if (!dp_info->dp_get_gsw_parser)
		return -1;

	return dp_info->dp_get_gsw_parser(cpu, mpe1, mpe2, mpe3);
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
		if (verify && pinfo[i].size != parser_size(pinfo[i].v))
			pr_err("%s[%d](%d) != %s(%d)??\n",
			       "Lcal parser pinfo", i, pinfo[i].size,
			       "register cfg", parser_size(pinfo[i].v));

		/*force to update */
		pinfo[i].size = parser_size(pinfo[i].v);

		if (pinfo[i].size < 0 || pinfo[i].size > PKT_PMAC_OFFSET) {
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
	struct inst_info *dp_info;
	if (!owner) {
		pr_err("Allocate port failed for owner NULL\n");
		return DP_FAILURE;
	}

	if (is_invalid_port(port_id) || is_invalid_inst(inst))
		return DP_FAILURE;

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

	dp_info = get_dp_prop_info(inst);
	cbm_data.dp_inst = inst;
	cbm_data.cbm_inst = dp_port_prop[inst].cbm_inst;

	if (flags & DP_F_DEREGISTER) {	/*De-register */
		port = get_dp_port_info(inst, port_id);
		if (port->status != PORT_ALLOCATED) {
			pr_err("No Deallocate for module %s w/o deregistered\n",
			       owner->name);
			return DP_FAILURE;
		}

		dp_notifier_invoke(inst, dev, port_id, 0,
				   DP_EVENT_DE_ALLOC_PORT);

		cbm_data.deq_port = port->deq_port_base;
		cbm_data.dma_chan = port->dma_chan;

		dp_dealloc_cqm_port(owner, dev_port, port, &cbm_data,
				    port->alloc_flags | flags);
		dp_inst_del_mod(owner, port_id, 0);
		DP_CB(inst, port_platform_set)(inst, port_id, data, flags);
		/* Only clear those fields we need to clear */
		memset(port, 0, offsetof(struct pmac_port_info, tail));
		return DP_SUCCESS;
	}
	if (port_id) { /*with specified port_id */
		port = get_dp_port_info(inst, port_id);
		if (port->status != PORT_FREE) {
			pr_err("%s %s(%s %d) fail: port %d used by %s %d\n",
			       "module", owner->name,
			       "dev_port", dev_port, port_id,
			       port->owner->name,
			       port->dev_port);
			return DP_FAILURE;
		}
	}
	if (dp_alloc_cqm_port(owner, dev, dev_port, port_id, &cbm_data, flags))
		return DP_FAILURE;

	port_id = cbm_data.dp_port;
	/* Only clear those fields we need to clear */
	port = get_dp_port_info(inst, port_id);
	memset(port, 0, offsetof(struct pmac_port_info, tail));
	port->type = DP_DATA_PORT_LINUX;
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

	port->num_dma_chan = cbm_data.num_dma_chan;

	if (cbm_data.num_dma_chan) {
		int dma_ch_base;

		dma_ch_base = get_dma_chan_idx(inst, cbm_data.num_dma_chan);
		if (dma_ch_base == DP_FAILURE) {
			pr_err("Failed get_dma_chan_idx!!\n");
			dp_dealloc_cqm_port(owner, dev_port, port, &cbm_data,
					    flags);
			/* Only clear those fields we need to clear */
			memset(port, 0, offsetof(struct pmac_port_info, tail));
			return DP_FAILURE;
		}
		port->dma_chan_tbl_idx = dma_ch_base;
	}
	/*save info to port data*/
	data->deq_port_base = port->deq_port_base;
	data->deq_num = port->deq_port_num;
	if (cbm_data.flags & CBM_PORT_DMA_CHAN_SET)
		port->dma_chan = cbm_data.dma_chan;
	if (cbm_data.flags & CBM_PORT_PKT_CRDT_SET)
		port->tx_pkt_credit = cbm_data.tx_pkt_credit;
	if (cbm_data.flags & CBM_PORT_BYTE_CRDT_SET)
		port->tx_b_credit = cbm_data.tx_b_credit;
	if (cbm_data.flags & CBM_PORT_RING_ADDR_SET) {
		port->txpush_addr = (void *)cbm_data.txpush_addr;
		port->txpush_addr_qos =	(void *)cbm_data.txpush_addr_qos;
	}
	if (cbm_data.flags & CBM_PORT_RING_SIZE_SET)
		port->tx_ring_size = cbm_data.tx_ring_size;
	if (cbm_data.flags & CBM_PORT_RING_OFFSET_SET)
		port->tx_ring_offset = cbm_data.tx_ring_offset;
	if (cbm_data.num_dma_chan > 1 &&
	    cbm_data.deq_port_num != cbm_data.num_dma_chan) {
		pr_err("ERROR:deq_port_num=%d not equal to num_dma_chan=%d\n",
		       cbm_data.deq_port_num, cbm_data.num_dma_chan);
		return DP_FAILURE;
	}

	if (dp_info->port_platform_set(inst, port_id, data, flags)) {
		pr_err("Failed port_platform_set for port_id=%d(%s)\n",
		       port_id, owner ? owner->name : "");
		dp_dealloc_cqm_port(owner, dev_port, port, &cbm_data, flags);
		/* Only clear those fields we need to clear */
		memset(port, 0, offsetof(struct pmac_port_info, tail));
		return DP_FAILURE;
	}
	if (pmac_cfg)
		dp_pmac_set(inst, port_id, pmac_cfg);
	/*only 1st dp instance support real CPU path traffic */
	if (!inst && dp_info->init_dma_pmac_template)
		dp_info->init_dma_pmac_template(port_id, flags);
	for (i = 0; i < dp_info->cap.max_num_subif_per_port; i++)
		INIT_LIST_HEAD(&get_dp_port_subif(port, i)->logic_dev);
	dp_inst_insert_mod(owner, port_id, inst, 0);

	DP_DEBUG(DP_DBG_FLAG_REG,
		 "Port %d allocation succeed for module %s with dev_port %d\n",
		 port_id, owner->name, dev_port);

	dp_notifier_invoke(inst, dev, port_id, 0, DP_EVENT_ALLOC_PORT);

	return port_id;
}

int dp_cbm_deq_port_enable(struct module *owner, int inst, int port_id,
			   int deq_port, int num_deq_port, int flags,
			   u32 dma_ch_offset)
{
	struct cbm_dp_en_data cbm_data = {0};
	struct pmac_port_info *port_info;
	u32 cqm_deq_port, dma_ch_ref;

	port_info = get_dp_port_info(inst, port_id);
	cbm_data.dp_inst = inst;
	cbm_data.num_dma_chan = port_info->num_dma_chan;
	cbm_data.cbm_inst = dp_port_prop[inst].cbm_inst;

	cqm_deq_port = deq_port;
	dma_ch_ref = atomic_read(&(dp_dma_chan_tbl[inst] +
				   dma_ch_offset)->ref_cnt);
	cbm_data.deq_port = cqm_deq_port;
	/* Enhance below condition later to support
	 * both dp_register_subif & deq_update_info API
	 */
	/* to enable DMA */
	if (flags & CBM_PORT_F_DISABLE) {
		if (!is_directpath(port_info) && !dma_ch_ref)
			cbm_data.dma_chnl_init = 1;
	} else {
		if (dma_ch_ref == 1 && !(port_info->alloc_flags & DP_F_DIRECT))
			cbm_data.dma_chnl_init = 1;
	}

	DP_DEBUG(DP_DBG_FLAG_REG, "%s%d%s%d%s%d%s%d\n",
		 "cbm_dp_enable: dp_port=", port_id,
		 " deq_port=", cbm_data.deq_port,
		 " dma_chnl_init=", cbm_data.dma_chnl_init,
		 " tx_dma_chan ref=", dma_ch_ref);
	if (cbm_dp_enable(owner, port_id, &cbm_data, flags,
			  port_info->alloc_flags)) {
		DP_DEBUG(DP_DBG_FLAG_REG, "cbm_dp_enable fail\n");
		return DP_FAILURE;
	}
	return DP_SUCCESS;
}

int32_t dp_register_subif_private(int inst, struct module *owner,
				  struct net_device *dev,
				  char *subif_name, dp_subif_t *subif_id,
				  struct dp_subif_data *data, u32 flags)
{
	int i, port_id, start, end, j;
	struct pmac_port_info *port_info;
	struct cbm_dp_en_data cbm_data = {0};
	struct subif_platform_data platfrm_data = {0};
	struct dp_subif_info *sif;
	u32 cqm_deq_port;
	u32 dma_ch_offset;
	u32 dma_ch_ref_curr;
	struct inst_info *dp_info = get_dp_prop_info(inst);

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
		return DP_FAILURE;
	}
	if (!dp_dma_chan_tbl[inst]) {
		pr_err("dp_dma_chan_tbl[%d] NULL\n", inst);
		return DP_FAILURE;
	}
	if (subif_id->subif < 0) {/*dynamic mode */
		if (flags & DP_F_SUBIF_LOGICAL) {
			if (!(dp_info->supported_logic_dev(inst, dev,
							   subif_name))) {
				DP_DEBUG(DP_DBG_FLAG_REG,
					 "reg subif fail:%s not support dev\n",
					 subif_name);
				return DP_FAILURE;
			}
			if (!(flags & DP_F_ALLOC_EXPLICIT_SUBIFID)) {
				/*Share same subif with its base device
				 *For GRX350: nothing need except save it
				 *For PRX300: it need to allocate BP for it
				 */
				return add_logic_dev(inst, port_id, dev,
						     subif_id, data, flags);
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

	/*allocate a free subif */
	for (i = start; i < end; i++) {
		sif = get_dp_port_subif(port_info, i);
		if (!sif->flags)
			break;
		if ((subif_id->subif > 0) && (start == i))
			pr_err("subifid(%d) is duplicated! vap(%d)\n",
			       subif_id->subif, start);
	}
	if (i >= end) {
		pr_err("register subif failed for no matched vap(%s)\n",
		       dev->name ? dev->name : "NULL");
		return DP_FAILURE;
	}
	if (data->num_deq_port == 0)
		data->num_deq_port = 1;

	cqm_deq_port = port_info->deq_port_base + data->deq_port_idx;
	dma_ch_offset =	DP_DEQ(inst, cqm_deq_port).dma_ch_offset;
	dma_ch_ref_curr = atomic_read(&(dp_dma_chan_tbl[inst] +
					dma_ch_offset)->ref_cnt);

	if (data->flag_ops & DP_SUBIF_PREL2)
		sif->prel2_len = 1;

	cbm_data.dp_inst = inst;
	cbm_data.num_dma_chan = dp_get_dma_ch_num(inst, port_id,
				data->num_deq_port);
	cbm_data.cbm_inst = dp_port_prop[inst].cbm_inst;
	cbm_data.f_policy = data->f_policy;
	cbm_data.bm_policy_res_id = data->bm_policy_res_id;

	for (j = 0; j < data->num_deq_port; j++) {
		cbm_data.deq_port = port_info->deq_port_base +
				    data->deq_port_idx + j;
		if (get_dp_deqport_info(inst, cbm_data.deq_port)->ref_cnt)
			continue;
		/* No need to enable DMA if multiple DEQ->single DMA */
		if (j != 0)
			cbm_data.dma_chnl_init = 0;
		else
			/* PPA Directpath/LitePath don't have DMA CH */
			if (dma_ch_ref_curr == 0 && !is_directpath(port_info))
				cbm_data.dma_chnl_init = 1;
		if (dp_enable_cqm_port(owner, port_info, &cbm_data, 0))
			return DP_FAILURE;
	}

	if (data->f_policy) {
		sif->tx_policy_num = cbm_data.tx_policy_num;
		sif->tx_policy_base = cbm_data.tx_policy_base;
		sif->rx_policy_num = cbm_data.rx_policy_num;
		sif->rx_policy_base = cbm_data.rx_policy_base;
	} else {
		sif->tx_policy_num = port_info->tx_policy_num;
		sif->tx_policy_base = port_info->tx_policy_base;
		sif->rx_policy_num = port_info->rx_policy_num;
		sif->rx_policy_base = port_info->rx_policy_base;
	}

	if (dp_info->subif_platform_set(inst, port_id, i, &platfrm_data,
					flags)) {
		pr_err("subif_platform_set fail\n");
		return DP_FAILURE;
	}
	DP_DEBUG(DP_DBG_FLAG_REG, "subif_platform_set succeed\n");

	sif->flags = 1;
	sif->netif = dev;
	port_info->port_id = port_id;

	/* currently this field is used for EPON case. Later can enhance */
	sif->num_qid = data->num_deq_port;
	sif->deq_port_idx = data->deq_port_idx;

	if (subif_id->subif < 0)
		sif->subif = SET_VAP(i, port_info->vap_offset,
				     port_info->vap_mask);
	else /* provided by caller since it is alerady shifted properly */
		sif->subif = subif_id->subif;
	strncpy(sif->device_name, subif_name, sizeof(sif->device_name) - 1);
	sif->subif_flag = flags;
	sif->spl_conn_type = DP_SPL_INVAL;
	sif->data_flag_ops = data->flag_ops;
	if (data->flag_ops & DP_SUBIF_RX_FLAG)
		STATS_SET(sif->rx_flag, !!data->rx_en_flag);
	else
		STATS_SET(sif->rx_flag, 1);

	if (!data->rx_fn)
		sif->rx_fn = port_info->cb.rx_fn;
	else
		sif->rx_fn = data->rx_fn;

	sif->get_subifid_fn = data->get_subifid_fn;
	if (!sif->get_subifid_fn)
		sif->get_subifid_fn = port_info->cb.get_subifid_fn;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	if (data->flag_ops & DP_SUBIF_SWDEV)
		sif->swdev_en = data->swdev_en_flag;
	else
		sif->swdev_en = port_info->swdev_en;
#endif
	port_info->status = PORT_SUBIF_REGISTERED;
	/* to disable CPU bridge port from bp member list */
	if (data->flag_ops & DP_SUBIF_BP_CPU_DISABLE)
		sif->cpu_port_en = 0;
	else
		sif->cpu_port_en = 1;
	subif_id->port_id = port_id;
	subif_id->subif = sif->subif;
	sif->subif_groupid = i;
	/* set port as LCT port */
	if (data->flag_ops & DP_F_DATA_LCT_SUBIF) {
		port_info->lct_idx = i;
		/* ignore a failure here, this will only result in missing
		 * counter corrections, but is not a critical problem
		 * we allocate two buffers for unicast/multicast counters
		 */
		port_info->lct_rx_cnt =
			kzalloc(2 * sizeof(*port_info->lct_rx_cnt), GFP_ATOMIC);
	}
	port_info->num_subif++;

	if (get_dp_bp_info(inst, sif->bp)->ref_cnt > 1)
		return DP_SUCCESS;
	dp_inst_add_dev(dev, subif_id->inst, subif_id->port_id,
			sif->bp, subif_id->subif, flags);
	return DP_SUCCESS;
}

int32_t dp_deregister_subif_private(int inst, struct module *owner,
				    struct net_device *dev,
				    char *subif_name, dp_subif_t *subif_id,
				    struct dp_subif_data *data,
				    u32 flags)
{
	int res = DP_FAILURE;
	int i, j, port_id, cqm_port = 0, bp;
	u8 find = 0;
	struct pmac_port_info *port_info;
	struct cbm_dp_en_data cbm_data = {0};
	struct subif_platform_data platfrm_data = {0};
	struct dp_subif_info *sif;
	u32 dma_ch_offset, dma_ch_ref;
	struct inst_info *dp_info = get_dp_prop_info(inst);

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

	/* device not match. Maybe it is unexplicit logical dev */
	if (sif->netif != dev)
		return del_logic_dev(inst, &sif->logic_dev, dev, flags);

	/* reset LCT port and free counter corrections */
	if (data->flag_ops & DP_F_DATA_LCT_SUBIF) {
		port_info->lct_idx = 0;
		/* take mib_cnt_lock to avoid access to freed memory */
		spin_lock_bh(&port_info->mib_cnt_lock);
		kfree(port_info->lct_rx_cnt);
		port_info->lct_rx_cnt = NULL;
		spin_unlock_bh(&port_info->mib_cnt_lock);
	}
	if (!list_empty(&sif->logic_dev)) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "Unregister fail: logic_dev of %s not empty yet!\n",
			 subif_name);
		return res;
	}
	bp = sif->bp;

	if (sif->ctp_dev)
		dp_notifier_invoke(inst, sif->ctp_dev, port_id, subif_id->subif,
				   DP_EVENT_DE_REGISTER_SUBIF);

	/* subif_hw_reset */
	if (dp_info->subif_platform_set(inst, port_id, i, &platfrm_data,
					flags)) {
		pr_err("subif_hw_reset fail\n");
	}

	/* for pmapper and non-pmapper both
	 *  1)for PRX300, dev is managed at its HAL level
	 *  2)for GRX350, bp/dev should be always zero/NULL at present
	 *        before adapting to new datapath framework
	 */
	if (!get_dp_bp_info(inst, bp)->dev) {
		DP_DEBUG(DP_DBG_FLAG_REG, "%s for %s inst=%d bp=%d\n",
			 "dp_inst_del_dev", dev->name, inst, bp);

		dp_inst_del_dev(dev, inst, port_id, subif_id->subif, 0);

		dp_notifier_invoke(inst, dev, port_id, subif_id->subif,
				   DP_EVENT_DE_REGISTER_SUBIF);
	}
	/* reset mib, flag, and others */
	memset(&sif->mib, 0, sizeof(sif->mib));
	sif->flags = 0;
	sif->netif = NULL;
	port_info->num_subif--;

	if (!port_info->num_subif)
		port_info->status = PORT_DEV_REGISTERED;
	for (j = 0; j < sif->num_qid; j++) {
		cqm_port = sif->cqm_deq_port[j];

		if (get_dp_deqport_info(inst, cqm_port)->ref_cnt)
			continue;

		/*disable cqm port */
		cbm_data.dp_inst = inst;
		cbm_data.cbm_inst = dp_port_prop[inst].cbm_inst;
		cbm_data.deq_port = cqm_port;
		cbm_data.num_dma_chan =
			dp_get_dma_ch_num(inst, port_id, sif->num_qid);
		dma_ch_offset =	DP_DEQ(inst, cqm_port).dma_ch_offset;
		dma_ch_ref = atomic_read(&(dp_dma_chan_tbl[inst] +
					   dma_ch_offset)->ref_cnt);

		/* PPA Directpath/LitePath don't have DMA CH */
		if (!is_directpath(port_info) && !dma_ch_ref)
			cbm_data.dma_chnl_init = 1;

		if (dp_enable_cqm_port(owner, port_info, &cbm_data,
				       CBM_PORT_F_DISABLE))
			return DP_FAILURE;

		DP_DEBUG(DP_DBG_FLAG_REG,
			 "cbm_dp_enable ok:port=%d subix=%d cqm_port=%d\n",
			 port_id, i, cqm_port);
	}

	DP_DEBUG(DP_DBG_FLAG_REG, "  dp_port=%d subif=%d cqm_port=%d\n",
		 subif_id->port_id, subif_id->subif, cqm_port);
	res = DP_SUCCESS;

	return res;
}

static int dp_config_ctp(int inst, int vap, int dp_port, int bp,
			 struct subif_platform_data *data)
{
	struct inst_info *inst_info;

	inst_info = get_dp_prop_info(inst);
	if (!inst_info->dp_set_ctp_bp) {
		pr_err("dp set ctp config not supported\n");
		return DP_FAILURE;
	}
	return inst_info->dp_set_ctp_bp(inst, vap, dp_port, bp, data);
}

static int dp_cbm_deq_update(int inst, struct dp_subif_upd_info *info)
{
	struct inst_info *inst_info;

	inst_info = get_dp_prop_info(inst);
	if (!inst_info->dp_deq_update_info) {
		pr_err("dp_deq_update_info not supported\n");
		return DP_FAILURE;
	}
	return inst_info->dp_deq_update_info(info);
}

static int dp_alloc_bp_priv(int inst, int port_id, int subif_ix,
			    int fid, int bp_member, int flags)
{
	struct inst_info *inst_info;

	inst_info = get_dp_prop_info(inst);
	if (!inst_info->dp_alloc_bridge_port) {
		pr_err("dp_alloc_bridge_port not supported\n");
		return DP_FAILURE;
	}
	return inst_info->dp_alloc_bridge_port(inst, port_id, subif_ix,
					       fid, bp_member, flags);
}

static int dp_free_bp_priv(int inst, int bp)
{
	struct inst_info *inst_info;

	inst_info = get_dp_prop_info(inst);
	if (!inst_info->dp_free_bridge_port) {
		pr_err("dp_free_bridge_port not supported\n");
		return DP_FAILURE;
	}
	return inst_info->dp_free_bridge_port(inst, bp);
}

int32_t dp_update_subif_info(struct dp_subif_upd_info *info)
{
	struct pmac_port_info *port_info;
	struct dp_subif_info *sif;
	struct dp_dev *dp_dev;
	int vap, bp = 0, fid = 0, res = DP_FAILURE;
	int f_new_subif = -1, f_old_subif = -1, f_rem_old_dev = 0;
	int f_rem_old_ctp_dev = 0, f_notif = 1;
	struct net_device *old_dev, *old_ctp_dev;
	struct bp_pmapper *bp_info;
	dp_subif_t *new_subif_id_sync, *old_subif_id_sync;
	dp_get_netif_subifid_fn_t subifid_fn = NULL;
	struct dp_subif_data old_data = {0}, new_data = {0};
	int flags = 0; /*to set CPU BP member */
	struct subif_platform_data platfrm_data = {0};
	struct dp_subif_data data = {0};

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (!info || is_invalid_inst(info->inst) ||
	    is_invalid_port(info->dp_port)) {
		pr_err("dp_update_subif failed, info not valid\n");
		return res;
	}
	if (!info->new_dev) {
		pr_err("dp_update_subif failed, info->new_dev NULL\n");
		return res;
	}

	port_info = get_dp_port_info(info->inst, info->dp_port);
	vap = GET_VAP(info->subif, port_info->vap_offset,
		      port_info->vap_mask);
	sif = get_dp_port_subif(port_info, vap);
	/* please note for LGM we have subif level callback, not port level */
	subifid_fn = port_info->cb.get_subifid_fn;
	old_dev = sif->netif;
	old_ctp_dev = sif->ctp_dev;
	DP_DEBUG(DP_DBG_FLAG_REG, "%s:%s%s %s%s %s=0x%x(%d)\n",
		 "update_subif", "new_dev=", info->new_dev->name,
		 "new_ctp_dev=", info->new_ctp_dev->name, "subif",
		 info->subif, vap);

	DP_LIB_LOCK(&dp_lock);
	/* update the master device info to subif */
	if (info->new_dev != old_dev) {
		/* check if exists in DP dev_list */
		dp_dev = dp_dev_lookup(info->new_dev);
		if (!dp_dev) {
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "new master dev(%s) not exists\n",
				 info->new_dev->name);
			if (!(sif->cpu_port_en))
				flags = DP_SUBIF_BP_CPU_DISABLE;
			/* Allocate new bp */
			bp = dp_alloc_bp_priv(info->inst, info->dp_port, vap,
					      0, 0, flags);
			if (bp < 0) {
				pr_err("Fail to alloc bridge port\n");
				goto exit;
			}
			if (info->new_ctp_dev) {
				DP_DEBUG(DP_DBG_FLAG_REG,
					 "master dev with new bp(%d)\n", bp);
				/* update bridge port table reference counter */
				bp_info = get_dp_bp_info(info->inst, bp);
				bp_info->dev = info->new_dev;
				bp_info->ref_cnt = 1;
				bp_info->flag = 1;
				INIT_LIST_HEAD(&bp_info->ctp_dev);
				dp_ctp_dev_list_add(&bp_info->ctp_dev,
						    info->new_ctp_dev, bp, vap);
			}
			dp_inst_add_dev(info->new_dev, info->inst,
					info->dp_port, bp, vap, 0);
		} else {
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "new master dev(%s) exists\n",
				 info->new_dev->name);
			bp = dp_dev->bp;
			fid = dp_dev->fid;
			bp_info = get_dp_bp_info(info->inst, bp);
			if (!bp_info->flag) {
				pr_err("why bp_info->flag:%d(%s)?\n",
				       bp_info->flag, "expect 1");
				goto exit;
			}
			if (info->new_ctp_dev) {
				bp_info->ref_cnt++;

				DP_DEBUG(DP_DBG_FLAG_REG,
					 "update bp refcnt:%d\n",
					 bp_info->ref_cnt);
				dp_ctp_dev_list_add(&bp_info->ctp_dev,
						    info->new_ctp_dev, bp, vap);
			}
		}
		/* flag set to clear old dp dev information */
		f_rem_old_dev = 1;
		DP_DEBUG(DP_DBG_FLAG_REG, "update bp(%d) into GSWIP CTP tbl\n",
			 bp);
		/* update GSWIP CtpPortconfig table */
		platfrm_data.dev = info->new_dev;
		data.ctp_dev = info->new_ctp_dev;
		platfrm_data.subif_data = &data;
		res = dp_config_ctp(info->inst, vap, info->dp_port, bp,
				    &platfrm_data);
		if (res) {
			if (info->new_ctp_dev)
				dp_ctp_dev_list_del(&bp_info->ctp_dev,
						    info->new_ctp_dev);
			goto exit;
		}
	}
	bp_info = get_dp_bp_info(info->inst, sif->bp);
	if (old_ctp_dev) {
		if (old_ctp_dev != info->new_ctp_dev) {
			/* remove the CTP dev from pmapper */
			dp_ctp_dev_list_del(&bp_info->ctp_dev, old_ctp_dev);
			bp_info->ref_cnt--;
			if (!bp_info->ref_cnt) {
				bp_info->dev = NULL;
				bp_info->flag = 0;
			}
			DP_DEBUG(DP_DBG_FLAG_REG,
				 "reduce old bp refcnt:%d\n", bp_info->ref_cnt);
			dp_notifier_invoke(info->inst,
					   old_ctp_dev, info->dp_port,
					   info->subif,
					   DP_EVENT_DE_REGISTER_SUBIF);
			f_rem_old_ctp_dev = 1;
		}
		old_data.ctp_dev = old_ctp_dev;
	}
	if ((!f_rem_old_dev) && (!f_rem_old_ctp_dev))
		f_notif = 0;
	/* remove old dev */
	if ((!bp_info->ref_cnt) && (f_rem_old_dev)) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "del old master dev(%s) from dp_dev list\n",
			 old_dev->name);
		dp_inst_del_dev(old_dev, info->inst, info->dp_port, vap, 0);

		dp_notifier_invoke(info->inst, old_dev, info->dp_port,
				   info->subif, DP_EVENT_DE_REGISTER_SUBIF);
		dp_free_bp_priv(info->inst, sif->bp);
	}

	/* added to make the subif_sync info similar to de-register
	 * subif to force update rcu link list for old_dev/old_ctp_dev
	 * Later will set this flag correctly
	 */
	sif->flags = 0;
	/* collect old subif info to update rcu list */
	old_subif_id_sync = devm_kzalloc(&g_dp_dev->dev,
					 sizeof(*old_subif_id_sync) * 2,
					 GFP_ATOMIC);
	if (!old_subif_id_sync) {
		res = DP_ERR_MEM;
		goto exit;
	}
	old_subif_id_sync->port_id = info->dp_port;
	old_subif_id_sync->inst = info->inst;
	res = dp_sync_subifid(old_dev, NULL, old_subif_id_sync, &old_data,
			      DP_F_DEREGISTER, &f_old_subif);
	/* no need any DQ port modification if prev & new tcont are same */
	if (sif->cqm_port_idx == info->new_cqm_deq_idx) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "same cbm deq index, no change in queue config\n");
	} else {
		/* new tcont, update DP deq info */
		res = dp_cbm_deq_update(info->inst, info);
		if (res)
			goto exit;
	}
	/* update new subif info to current subif struct */
	if (f_rem_old_dev) {
		DP_DEBUG(DP_DBG_FLAG_REG, "%s%s bp=%d fid=%d into DP subif\n",
			 "update master_dev=", info->new_dev->name, bp, fid);
		sif->netif = info->new_dev;
		sif->bp = bp;
		sif->fid = fid;
		strncpy(sif->device_name, info->new_dev->name,
			sizeof(sif->device_name) - 1);
	}
	sif->ctp_dev = info->new_ctp_dev;
	sif->flags = 1;
	new_data.ctp_dev = sif->ctp_dev;
	new_subif_id_sync = devm_kzalloc(&g_dp_dev->dev,
					 sizeof(*new_subif_id_sync) * 2,
					 GFP_ATOMIC);
	if (!new_subif_id_sync) {
		devm_kfree(&g_dp_dev->dev, old_subif_id_sync);
		res = DP_ERR_MEM;
		goto exit;
	}
	new_subif_id_sync->port_id = info->dp_port;
	new_subif_id_sync->inst = info->inst;
	res = dp_sync_subifid(sif->netif, NULL, new_subif_id_sync, &new_data, 0,
			      &f_new_subif);
	if (res) {
		devm_kfree(&g_dp_dev->dev, old_subif_id_sync);
		devm_kfree(&g_dp_dev->dev, new_subif_id_sync);
		goto exit;
	}
	DP_LIB_UNLOCK(&dp_lock);

	/* remove old rcu info based on collected subif info */
	res = dp_sync_subifid_priv(old_dev, NULL, old_subif_id_sync, &old_data,
				   DP_F_DEREGISTER, subifid_fn, &f_old_subif,
				   f_notif);
	if (res) {
		pr_err("remove old dev from rcu fail(%s)\n",
		       old_dev->name ? old_dev->name : "NULL");
		devm_kfree(&g_dp_dev->dev, old_subif_id_sync);
		devm_kfree(&g_dp_dev->dev, new_subif_id_sync);
		return DP_FAILURE;
	}
	/* update rcu info based on new subif info */
	res = dp_sync_subifid_priv(info->new_dev, NULL, new_subif_id_sync,
				   &new_data, 0, subifid_fn, &f_new_subif,
				   f_notif);
	if (res) {
		pr_err("update new dev into rcu fail(%s)\n",
		       info->new_dev->name ? info->new_dev->name : "NULL");
		devm_kfree(&g_dp_dev->dev, old_subif_id_sync);
		devm_kfree(&g_dp_dev->dev, new_subif_id_sync);
		return DP_FAILURE;
	}
	devm_kfree(&g_dp_dev->dev, new_subif_id_sync);
	devm_kfree(&g_dp_dev->dev, old_subif_id_sync);
	return DP_SUCCESS;
exit:
	if (unlikely(res))
		dp_print_err_info(res);
	res = DP_FAILURE;
	DP_LIB_UNLOCK(&dp_lock);
	return res;
}
EXPORT_SYMBOL(dp_update_subif_info);

/*Note: For same owner, it should be in the same HW instance
 *          since dp_register_dev/subif no dev_port information at all,
 *          at the same time, dev is optional and can be NULL
 */

int32_t dp_alloc_port(struct module *owner, struct net_device *dev,
		      u32 dev_port, int32_t port_id,
		      dp_pmac_cfg_t *pmac_cfg, u32 flags)
{
	struct dp_port_data data = {0};

	return dp_alloc_port_ext(0, owner, dev, dev_port, port_id, pmac_cfg,
				 &data, flags);
}
EXPORT_SYMBOL(dp_alloc_port);

static bool dp_late_init(void)
{
	if (atomic_cmpxchg(&dp_status, 0, 1) == 0)
		dp_init_module();
	if (!dp_init_ok)
		pr_err("dp_alloc_port fail: datapath can't init\n");
	return dp_init_ok;
}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_ACA_CSUM_WORKAROUND)
static int32_t aca_csum_workaround(int inst, u32 flags)
{
	int res = DP_FAILURE;

	if (inst) /* only inst zero need ACA workaround */
		return res;

	/*For VRX518, it will always carry DP_F_FAST_WLAN flag for
	 * ACA HW resource purpose in CBM
	 */
	if ((flags & DP_F_FAST_WLAN) && aca_portid < 0) {
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

	return DP_SUCCESS;
}
#endif

int32_t dp_alloc_port_ext(int inst, struct module *owner,
			  struct net_device *dev,
			  u32 dev_port, int32_t port_id,
			  dp_pmac_cfg_t *pmac_cfg,
			  struct dp_port_data *data, u32 flags)
{
	int res;
	struct dp_port_data tmp_data = {0};

	if (!dp_late_init())
		return DP_FAILURE;
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
	if (res > 0)
		res = aca_csum_workaround(inst, flags);
#endif
	return res;
}
EXPORT_SYMBOL(dp_alloc_port_ext);

int32_t dp_register_dev(struct module *owner, u32 port_id,
			dp_cb_t *dp_cb, u32 flags)
{
	int inst;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (!owner || is_invalid_port(port_id))
		return DP_FAILURE;

	inst = dp_get_inst_via_module(owner, port_id, 0);

	if (inst < 0) {
		pr_err("%s not valid module %s\n", __func__, owner->name);
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

	/* Disable UMT port */
	if (ops->umt_enable(ops->umt_dev, umt->ctl.id, 0))
		return DP_FAILURE;

	/* Release UMT port */
	return ops->umt_release(ops->umt_dev, umt->ctl.id);
}
#endif

static int32_t dp_deregister_dev(int inst, struct module *owner,
				 u32 port_id, struct dp_dev_data *data,
				 u32 flags)
{
	struct pmac_port_info *port_info = get_dp_port_info(inst, port_id);
	struct cbm_dp_alloc_complete_data *cbm_data;
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	struct cqm_port_info *deq;
#endif
	int i, ret;

	if (port_info->status != PORT_DEV_REGISTERED) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "%s de-register dev failed due to wrong state %d\n",
			 owner->name, port_info->status);
		return DP_FAILURE;
	}
	cbm_data = kzalloc(sizeof(*cbm_data), GFP_ATOMIC);
	if (!cbm_data)
		return DP_FAILURE;

	dp_notifier_invoke(inst, port_info->dev, port_id, 0,
			   DP_EVENT_DE_REGISTER_DEV);

	port_info->status = PORT_ALLOCATED;
	cbm_data->deq_port = port_info->deq_port_base;
	cbm_data->num_rx_ring = port_info->num_rx_ring;
	for (i = 0; i < port_info->num_rx_ring; i++) {
		cbm_data->rx_ring[i] = &data->rx_ring[i];
		cbm_data->rx_ring[i]->out_enq_port_id =
			port_info->rx_ring[i].out_enq_port_id;
		cbm_data->rx_ring[i]->out_dma_ch_to_gswip =
			port_info->rx_ring[i].out_dma_ch_to_gswip;
		cbm_data->rx_ring[i]->out_cqm_deq_port_id =
			port_info->rx_ring[i].out_cqm_deq_port_id;
		cbm_data->rx_ring[i]->rx_policy_base =
			port_info->rx_ring[i].rx_policy_base;
		cbm_data->rx_ring[i]->policy_num =
			port_info->rx_ring[i].policy_num;
	}
	cbm_data->num_tx_ring = port_info->num_tx_ring;
	for (i = 0; i < port_info->num_tx_ring; i++) {
		cbm_data->tx_ring[i] = &data->tx_ring[i];
		cbm_data->tx_ring[i]->txout_policy_base =
			port_info->tx_ring[i].txout_policy_base;
		cbm_data->tx_ring[i]->policy_num =
			port_info->tx_ring[i].policy_num;
	}
	/* Free CQM resources allocated during dev register */
	ret = dp_cqm_port_alloc_complete(owner, port_info, port_id,
					 cbm_data, DP_F_DEREGISTER);
	kfree(cbm_data);
	if (ret == DP_FAILURE)
		return ret;

	DP_CB(inst, dev_platform_set)(inst, port_id, data, flags);

#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	for (i = 0; i < port_info->num_umt_port; i++) {
		deq = get_dp_deqport_info(inst, port_info->deq_port_base + i);
		if (deq->ref_cnt_umt && !(--deq->ref_cnt_umt))
			remove_umt(inst, &port_info->umt[i]);
	}
#endif

	return DP_SUCCESS;
}

int32_t dp_register_dev_ext(int inst, struct module *owner, u32 port_id,
			    dp_cb_t *dp_cb, struct dp_dev_data *data,
			    u32 flags)
{
	int res = DP_FAILURE;
	struct pmac_port_info *port_info;
	struct dp_dev_data tmp_data = {0};
	struct cbm_dp_alloc_complete_data *cbm_data = NULL;

	if (unlikely(!dp_init_ok)) {
		pr_err("dp_register_dev failed for datapath not init yet\n");
		return res;
	}

	if (!data)
		data = &tmp_data;

	if (is_invalid_port(port_id) || is_invalid_inst(inst))
		return res;

	if (!owner) {
		pr_err("owner NULL\n");
		return res;
	}

	port_info = get_dp_port_info(inst, port_id);

	DP_LIB_LOCK(&dp_lock);
	if (flags & DP_F_DEREGISTER) {	/*de-register */
		res = dp_deregister_dev(inst, owner, port_id, data,
					flags);
		goto exit;
	}

	/*register a device */
	if (port_info->status != PORT_ALLOCATED) {
		pr_err("register dev fail for %s for unknown status:%d\n",
		       owner->name, port_info->status);
		goto exit;
	}

	if (port_info->owner != owner) {
		pr_err("No matched owner(%s):%px->%px\n",
		       owner->name, owner, port_info->owner);
		goto exit;
	}

	port_info->res_qid_base = data->qos_resv_q_base;
	port_info->num_resv_q = data->num_resv_q;

	cbm_data = kzalloc(sizeof(*cbm_data), GFP_ATOMIC);
	if (!cbm_data)
		goto exit;

	if (dp_build_cqm_data(inst, port_id, cbm_data, data))
		goto exit;

	/* Register device to CQM */
	if (dp_cqm_port_alloc_complete(owner, port_info, port_id, cbm_data,
				       flags))
		goto exit;

	/* For Streaming port save the policy base/num from CQM
	 * For DC port, dp_register_dc will overwrite from ring info
	 */
	port_info->tx_policy_base = cbm_data->tx_policy_base;
	port_info->tx_policy_num = cbm_data->tx_policy_num;

	res = dp_register_dc(inst, port_id, data, flags);
	if (res)
		goto exit;

	DP_CB(inst, dev_platform_set)(inst, port_id, data, flags);

	/* TODO: Need a HAL layer API for CQM and DMA Setup for CQM QoS path
	 * especially for LGM 4 Ring case
	 */
	port_info->status = PORT_DEV_REGISTERED;
	if (dp_cb)
		port_info->cb = *dp_cb;

	dp_notifier_invoke(inst, port_info->dev, port_id, 0,
			   DP_EVENT_REGISTER_DEV);

	res = DP_SUCCESS;

exit:
	kfree(cbm_data);
	DP_LIB_UNLOCK(&dp_lock);
	return res;
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
			      struct dp_subif_data *data, u32 flags)
{
	int res = DP_FAILURE;
	int n, port_id, f_subif = -1;
	struct pmac_port_info *port_info;
	struct dp_subif_data tmp_data = {0};
	dp_subif_t *subif_id_sync;
	dp_get_netif_subifid_fn_t subifid_fn = NULL;

	if (unlikely(!dp_init_ok)) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "dp_register_subif fail for datapath not init yet\n");
		res = DP_ERR_INIT_FAIL;
		goto EXIT;
	}

	if (!subif_id || !owner) {
		DP_DEBUG(DP_DBG_FLAG_REG, "%s Failed subif_id %px owner %px\n",
			 __func__, subif_id, owner);
		res = DP_ERR_INVALID_SUBIF;
		goto EXIT;
	}

	DP_DEBUG(DP_DBG_FLAG_REG,
		 "%s:owner=%s dev=%s(px=%px p=%p) %s=%s port_id=%d subif=%d(%s) %s%s\n",
		 (flags & DP_F_DEREGISTER) ?
		 "unregister subif:" : "register subif",
		 owner ? owner->name : "NULL",
		 dev ? dev->name : "NULL",
		 dev, dev,
		 "subif_name",
		 subif_name,
		 subif_id->port_id,
		 subif_id->subif,
		 (subif_id->subif < 0) ? "dynamic" : "fixed",
		 (flags & DP_F_SUBIF_LOGICAL) ? "Logical" : "",
		 (flags & DP_F_ALLOC_EXPLICIT_SUBIFID) ?
		 "Explicit" : "Non-Explicit");

	if (is_invalid_port(subif_id->port_id) || is_invalid_inst(inst)) {
		res = DP_ERR_INVALID_PORT_ID;
		goto EXIT;
	}

	port_id = subif_id->port_id;
	port_info = get_dp_port_info(inst, port_id);

	if ((!dev && !is_dsl(port_info)) || !subif_name) {
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
		res = dp_deregister_subif_private(inst, owner, dev, subif_name,
						  subif_id, data, flags);
	else /*register */
		res = dp_register_subif_private(inst, owner, dev, subif_name,
						subif_id, data, flags);
	if (res) {
		DP_LIB_UNLOCK(&dp_lock);
		goto EXIT;
	}

	if (!(flags & DP_F_SUBIF_LOGICAL)) {
		n = GET_VAP(subif_id->subif, port_info->vap_offset,
			    port_info->vap_mask);
		subifid_fn = get_dp_port_subif(port_info, n)->get_subifid_fn;
	}
	subif_id_sync = devm_kzalloc(&g_dp_dev->dev,
				     sizeof(*subif_id_sync) * 2, GFP_ATOMIC);

	if (!subif_id_sync) {
		DP_LIB_UNLOCK(&dp_lock);
		res = DP_ERR_MEM;
		goto EXIT;
	}
	subif_id_sync->port_id = port_id;
	subif_id_sync->inst = inst;
	res = dp_sync_subifid(dev, subif_name, subif_id_sync, data, flags,
			      &f_subif);
	if (res) {
		DP_LIB_UNLOCK(&dp_lock);
		devm_kfree(&g_dp_dev->dev, subif_id_sync);
		goto EXIT;
	}
	DP_LIB_UNLOCK(&dp_lock);
	if (!res)
		res = dp_sync_subifid_priv(dev, subif_name, subif_id_sync, data,
					   flags, subifid_fn, &f_subif, 1);
	devm_kfree(&g_dp_dev->dev, subif_id_sync);
EXIT:
	if (unlikely(res)) {
		dp_print_err_info(res);
		res = DP_FAILURE;
	}
	return res;
}
EXPORT_SYMBOL(dp_register_subif_ext);

int32_t dp_register_subif(struct module *owner, struct net_device *dev,
			  char *subif_name, dp_subif_t *subif_id,
			  u32 flags)
{
	int inst;
	struct dp_subif_data data = {0};

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (!subif_id || !owner || is_invalid_port(subif_id->port_id)) {
		DP_DEBUG(DP_DBG_FLAG_REG, "%s fail owner %px subif_id %px\n",
			 __func__, subif_id, owner);
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
			     dp_subif_t *subif, u32 flags)
{
	struct dp_subif_cache *dp_subif;
	struct dp_subif_info *sif;
	struct pmac_port_info *port_info;
	u32 idx;
	dp_get_netif_subifid_fn_t subifid_fn_t;
	int res = DP_FAILURE;

	if (unlikely(!dp_init_ok)) {
		DP_DEBUG(DP_DBG_FLAG_DBG,
			 "%s failed: datapath not initialized yet\n",
			 __func__);
		return res;
	}

	idx = dp_subif_hash(netif);
	//TODO handle DSL case in future
	rcu_read_lock_bh();
	dp_subif = dp_subif_lookup_safe(&dp_subif_list[idx], netif, subif_data);
	if (!dp_subif) {
		DP_DEBUG(DP_DBG_FLAG_DBG, "Failed dp_subif_lookup: %s\n",
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
		if (sif->netif && sif->flags)
			subif->associate_netif = sif->netif;
	}
	rcu_read_unlock_bh();
	if (subifid_fn_t) {
		/*subif->subif will be set by callback api itself */
		res = subifid_fn_t(netif, skb, subif_data, dst_mac, subif,
				   flags);
		if (res != 0)
			DP_DEBUG(DP_DBG_FLAG_DBG,
				 "get_netif_subifid callback function fail\n");
	} else {
		res = DP_SUCCESS;
	}
	return res;
}
EXPORT_SYMBOL(dp_get_netif_subifid);

bool dp_is_pmapper_check(struct net_device *dev)
{
	dp_subif_t subif = {0};
	int ret;

	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0);
	if (ret != DP_SUCCESS) {
		netdev_err(dev, "can not get subif\n");
		return false;
	}

	if (subif.flag_pmapper)
		return true;

	return false;
}
EXPORT_SYMBOL(dp_is_pmapper_check);

/*Note:
 * get subif according to netif.
 * Use subif->port_id passed by caller to get port_info
 */
int32_t dp_get_subifid_for_update(int inst, struct net_device *netif,
				  dp_subif_t *subif, u32 flags)
{
	int res = DP_FAILURE;
	int i;
	int port_id;
	u8 num = 0;
	u16 *subifs = NULL;
	u32 *subif_flag = NULL;
	struct logic_dev *tmp = NULL;
	struct pmac_port_info *p_info;

	port_id = subif->port_id;
	if (port_id < 0) {
		DP_DEBUG(DP_DBG_FLAG_DBG,
			 "%s failed: %s\n", __func__,
			 netif ? netif->name : "NULL");
		res = DP_ERR_SUBIF_NOT_FOUND;
		goto EXIT;
	}

	p_info = get_dp_port_info(inst, port_id);
	if (p_info->status != PORT_SUBIF_REGISTERED)
		goto EXIT;

	/* For DSL ATM case netif will be NULL with valid port id */
	if ((!netif) && (!is_dsl(p_info))) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "%s failed: netif null\n", __func__);
		return DP_ERR_NULL_DATA;
	}
	if (!subif) {
		DP_DEBUG(DP_DBG_FLAG_REG,
			 "%s failed:subif is NULL\n", __func__);
		return DP_ERR_NULL_DATA;
	}

	subifs = kzalloc(sizeof(*subifs) * DP_MAX_CTP_PER_DEV,
			 GFP_ATOMIC);
	if (!subifs)
		return DP_ERR_MEM;

	subif_flag = kzalloc(sizeof(*subif_flag) * DP_MAX_CTP_PER_DEV,
			     GFP_ATOMIC);
	if (!subif_flag) {
		res = DP_ERR_MEM;
		goto EXIT;
	}

	subif->flag_pmapper = 0;

	/*search sub-interfaces/VAP */
	for (i = 0; i < p_info->subif_max; i++) {
		struct dp_subif_info *sif = get_dp_port_subif(p_info, i);

		/* FOR DSL ATM case when netif is NULL, no need subif info */
		if (is_dsl(p_info) && (!netif))
			break;

		if (!sif->flags)
			continue;

#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
		if (sif->ctp_dev == netif) { /*for PON pmapper case*/
			if (num > 0) {
				pr_err("Multiple same ctp_dev exist\n");
				goto EXIT;
			}
			subifs[num] = sif->subif;
			subif_flag[num] = sif->subif_flag;
			subif->flag_bp = 0;
			memcpy(&subif->subif_common, &sif->subif_common,
			       sizeof(struct dp_subif_common));
			res = DP_SUCCESS;
			num++;
			break;
		}
#endif
		if (sif->netif == netif) {
			subif->flag_bp = 1;
			if (num >= DP_MAX_CTP_PER_DEV) {
				pr_err("%s: Why CTP over %d\n",
				       netif->name,
				       DP_MAX_CTP_PER_DEV);
				goto EXIT;
			}
			/* some dev may have multiple
			 * subif,like pon
			 */
			subifs[num] = sif->subif;
			memcpy(&subif->subif_common, &sif->subif_common,
			       sizeof(struct dp_subif_common));
			subif_flag[num] = sif->subif_flag;
			if (sif->ctp_dev)
				subif->flag_pmapper = 1;
			if (num && subif->bport != sif->bp) {
				pr_err("%s:Why many bp:%d %d\n",
				       netif->name, sif->bp,
				       subif->bport);
				goto EXIT;
			}
			res = DP_SUCCESS;
			num++;
		}
		/*continue search non-explicate logical device */
		list_for_each_entry(tmp, &sif->logic_dev, list) {
			if (tmp->dev == netif) {
				memcpy(&subif->subif_common,
				       &sif->subif_common,
				       sizeof(struct dp_subif_common));
				subif->subif_num = 1;
				subif->inst = inst;
				subif->subif_list[0] = tmp->ctp;
				subif->bport = tmp->bp;
				subif->port_id = port_id;
				subif_flag[num] = sif->subif_flag;
				res = DP_SUCCESS;
				/*note: logical device no callback */
				goto EXIT;
			}
		}
	}

	if (flags & DP_F_DEREGISTER)
		goto EXIT;

	subif->inst = inst;
	memcpy(&subif->subif_port_cmn, &p_info->subif_port_cmn,
	       sizeof(struct dp_subif_port_common));
	subif->subif_num = num;
	for (i = 0; i < num; i++) {
		subif->subif_list[i] = subifs[i];
		subif->subif_flag[i] = subif_flag[i];
	}
	res = DP_SUCCESS;
EXIT:
	kfree(subifs);
	kfree(subif_flag);
	return res;
}

static int dp_build_cqm_data(int inst, u32 port_id,
			     struct cbm_dp_alloc_complete_data *cbm_data,
			     struct dp_dev_data *data)
{
	int i = 0;
	struct pmac_port_info *port = get_dp_port_info(inst, port_id);

	if (data->num_rx_ring > DP_RX_RING_NUM ||
	    data->num_tx_ring > DP_TX_RING_NUM) {
		pr_err("Error RxRing = %d TxRing = %d\n",
		       data->num_rx_ring, data->num_tx_ring);
		return DP_FAILURE;
	}

	/* HOST -> ACA */
	/* For PRX300 No: of Tx Ring is 1 */
	cbm_data->num_tx_ring = data->num_tx_ring;

	/* Pass TX ring info to CQM */
	for (i = 0; i < data->num_tx_ring; i++)
		cbm_data->tx_ring[i] = &data->tx_ring[i];

	cbm_data->deq_port = port->deq_port_base;

	/* ACA -> HOST */
	/* For PRX300 No: of Rx Ring is 1 */
	cbm_data->num_rx_ring = data->num_rx_ring;

	/* Pass RX ring info to CQM */
	for (i = 0; i < data->num_rx_ring; i++)
		cbm_data->rx_ring[i] = &data->rx_ring[i];

	cbm_data->num_qid = data->num_resv_q;
	cbm_data->qid_base = data->qos_resv_q_base;
	cbm_data->alloc_flags = port->alloc_flags;
	cbm_data->bm_policy_res_id = data->bm_policy_res_id;
	cbm_data->opt_param = data->opt_param;

	return 0;
}

static int dp_register_dc(int inst, u32 port_id,
			  struct dp_dev_data *data, u32 flags)
{
	return DP_SUCCESS;
}

int dp_get_port_subitf_via_dev(struct net_device *dev, dp_subif_t *subif)
{
	int res;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	res = dp_get_netif_subifid(dev, NULL, NULL, NULL, subif, 0);
	return res;
}
EXPORT_SYMBOL(dp_get_port_subitf_via_dev);

int dp_get_port_subitf_via_ifname_private(char *ifname, dp_subif_t *subif)
{
	int i, j;
	int inst;
	struct inst_info *dp_info;

	inst = dp_get_inst_via_dev(NULL, ifname, 0);
	dp_info = get_dp_prop_info(inst);

	for (i = 0; i < dp_info->cap.max_num_dp_ports; i++) {
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

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

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
				      u32 flags)
{
	int res = 0;
	dp_subif_t tmp_subif = { 0 };
	struct pmac_port_info *p_info;
	int max_dp_ports;
	int dp_flags;

	DP_LIB_LOCK(&dp_lock);
	if (unlikely(!dp_init_ok)) {
		pr_err("%s fail: dp not ready\n", __func__);
		return DP_FAILURE;
	}
	if (subif) {
		tmp_subif = *subif;
		tmp_subif.inst = 0;
	} else if (netif) {
		dp_get_netif_subifid(netif, NULL, NULL, NULL, &tmp_subif, 0);
	} else if (ifname) {
		dp_get_port_subitf_via_ifname_private(ifname, &tmp_subif);
	}

	max_dp_ports = get_dp_prop_info(tmp_subif.inst)->cap.max_num_dp_ports;
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
		pr_err("%s failed for dp not init yet\n", __func__);
		return NULL;
	}

	if (ep >= 0 && ep < get_dp_prop_info(inst)->cap.max_num_dp_ports)
		return get_dp_port_info(inst, ep)->owner;

	return NULL;
}
EXPORT_SYMBOL(dp_get_module_owner);

/*if subif->vap == -1, it means all vap */
void dp_clear_mib(dp_subif_t *subif, u32 flag)
{
	int i, j, start_vap, end_vap;
	dp_reset_mib_fn_t reset_mib_fn;
	struct pmac_port_info *port_info;

	if (!subif || is_invalid_port(subif->port_id)) {
		DP_DEBUG(DP_DBG_FLAG_DBG, "%s Wrong subif\n", __func__);
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

void dp_clear_all_mib_inside(u32 flag)
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

int dp_get_drv_mib(dp_subif_t *subif, dp_drv_mib_t *mib, u32 flag)
{
	dp_get_mib_fn_t get_mib_fn;
	dp_drv_mib_t tmp;
	int i, vap;
	struct pmac_port_info *port_info;
	struct dp_subif_info *sif;

	if (unlikely(!dp_init_ok)) {
		DP_DEBUG(DP_DBG_FLAG_DBG,
			 "%s failed for datapath not init yet\n", __func__);
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
		       struct rtnl_link_stats64 *stats, u32 flags)
{
	dp_subif_t subif;
	int res;
	int (*get_mib)(dp_subif_t *subif_id, void *priv,
		       struct rtnl_link_stats64 * stats,
		       u32 flags);

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (subif_id) {
		subif = *subif_id;
	} else if (dev) {
		res = dp_get_port_subitf_via_dev(dev, &subif);
		if (res) {
			DP_DEBUG(DP_DBG_FLAG_MIB,
				 "%s fail:%s not registered yet to datapath\n",
				 __func__, dev->name);
			return DP_FAILURE;
		}
	} else {
		DP_DEBUG(DP_DBG_FLAG_MIB,
			 "%s: dev/subif_id both NULL\n", __func__);
		return DP_FAILURE;
	}
	get_mib = get_dp_prop_info(subif.inst)->dp_get_port_vap_mib;
	if (!get_mib)
		return DP_FAILURE;

	return get_mib(&subif, NULL, stats, flags);
}
EXPORT_SYMBOL(dp_get_netif_stats);

int dp_clear_netif_stats(struct net_device *dev, dp_subif_t *subif_id,
			 u32 flag)
{
	dp_subif_t subif;
	int (*clear_netif_mib_fn)(dp_subif_t *subif, void *priv, u32 flag);
	int i;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (subif_id) {
		clear_netif_mib_fn =
			get_dp_prop_info(subif_id->inst)->dp_clear_netif_mib;
		if (!clear_netif_mib_fn)
			return -1;
		return clear_netif_mib_fn(subif_id, NULL, flag);
	}
	if (dev) {
		if (dp_get_port_subitf_via_dev(dev, &subif)) {
			DP_DEBUG(DP_DBG_FLAG_MIB, "%s not register to %s\n",
				 dev->name, __func__);
			return -1;
		}
		clear_netif_mib_fn =
			get_dp_prop_info(subif.inst)->dp_clear_netif_mib;
		if (!clear_netif_mib_fn)
			return -1;
		return clear_netif_mib_fn(&subif, NULL, flag);
	}
	/*clear all */
	for (i = 0; i < DP_MAX_INST; i++) {
		clear_netif_mib_fn = get_dp_prop_info(i)->dp_clear_netif_mib;
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

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (is_invalid_inst(inst))
		return DP_FAILURE;

	dp_pmac_set_fn =  get_dp_prop_info(inst)->dp_pmac_set;
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
	struct bp_pmapper *bp_info;
	struct inst_info *dp_info;

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

	dp_info = get_dp_prop_info(inst);

	if (!dp_info->dp_set_gsw_pmapper) {
		pr_err("Set pmapper is not supported\n");
		return DP_FAILURE;
	}

	bport = subif.bport;
	if (bport >= DP_MAX_BP_NUM) {
		pr_err("BP port(%d) out of range %d\n", bport, DP_MAX_BP_NUM);
		return DP_FAILURE;
	}
	map = devm_kzalloc(&g_dp_dev->dev, sizeof(*map), GFP_ATOMIC);
	if (!map)
		return DP_FAILURE;
	memcpy(map, mapper, sizeof(*map));

	switch (mapper->mode) {
	case DP_PMAP_PCP:
	case DP_PMAP_DSCP:
		map->mode = GSW_PMAPPER_MAPPING_PCP;
		break;
	case DP_PMAP_DSCP_ONLY:
		map->mode = GSW_PMAPPER_MAPPING_DSCP;
		break;
	default:
		pr_err("Unknown mapper mode: %d\n", map->mode);
		goto EXIT;
	}
	/* workaround in case caller forget to set to default ctp */
	if (mapper->mode == DP_PMAP_PCP)
		for (i = 0; i < DP_PMAP_DSCP_NUM; i++)
			map->dscp_map[i] = mapper->def_ctp;

	ret = dp_info->dp_set_gsw_pmapper(inst, bport, subif.port_id, map,
					  flag);
	if (ret == DP_FAILURE) {
		pr_err("Failed to set mapper\n");
		goto EXIT;
	}

	bp_info = get_dp_bp_info(inst, bport);

	/* update local table for pmapper */
	bp_info->def_ctp = map->def_ctp;
	bp_info->mode = mapper->mode; /* original mode */
	for (i = 0; i < DP_PMAP_PCP_NUM; i++)
		bp_info->pcp[i] = map->pcp_map[i];
	for (i = 0; i < DP_PMAP_DSCP_NUM; i++)
		bp_info->dscp[i] = map->dscp_map[i];
	res = DP_SUCCESS;
EXIT:
	devm_kfree(&g_dp_dev->dev, map);
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
	struct inst_info *dp_info;

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

	dp_info = get_dp_prop_info(inst);

	if (!dp_info->dp_get_gsw_pmapper) {
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
	ret = dp_info->dp_get_gsw_pmapper(inst, bport, subif.port_id, mapper,
					  flag);
	if (ret == DP_FAILURE) {
		pr_err("Failed to get mapper\n");
		return DP_FAILURE;
	}
	return ret;
}
EXPORT_SYMBOL(dp_get_pmapper);

int32_t dp_rx(struct sk_buff *skb, u32 flags)
{
	struct sk_buff *next;
	int res = -1;
	int inst = 0;
	struct inst_info *dp_info = get_dp_prop_info(inst);

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
		res = dp_info->dp_rx(skb, flags);
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
	int inst = 0, ret = DP_SUCCESS;
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

	ret = dp_xmit(dev, &subif, skb, skb->len, 0);
	return ret;
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
		struct sk_buff *skb, int32_t len, u32 flags)
{
	int inst = 0;
	struct dp_tx_common_ex ex = {
		.cmn = {
			.inst = 0,
			.flags = flags,
			.subif = rx_subif->subif,
			.tx_portid = rx_subif->port_id,
		},
	};
	struct pmac_port_info *port;
	u32 vap;
	struct inst_info *dp_info = get_dp_prop_info(inst);
	enum DP_TX_FN_RET ret = DP_TX_FN_DROPPED;

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	ex.dev = rx_if;
	ex.rx_subif = rx_subif;
	ret = dp_info->dp_tx(skb, &ex.cmn);

	return ret ? DP_FAILURE : DP_SUCCESS;
#endif

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
	if (unlikely(!dp_init_ok)) {
		printk_ratelimited("%s failed for dp no init yet\n", __func__);
		dev_kfree_skb_any(skb);
		goto exit;
	}

	if (unlikely(!rx_subif)) {
		printk_ratelimited("%s failed for rx_subif null\n", __func__);
		dev_kfree_skb_any(skb);
		goto exit;
	}

	if (unlikely(!skb)) {
		printk_ratelimited("%s skb NULL\n", __func__);
		dev_kfree_skb_any(skb);
		goto exit;
	}

	if (unlikely(in_irq())) {
		printk_ratelimited("%s not allowed in interrupt context\n",
				   __func__);
		dev_kfree_skb_any(skb);
		goto exit;
	}
#endif

	if (unlikely(rx_subif->port_id >=
		     dp_info->cap.max_num_dp_ports)) {
		printk_ratelimited("rx_subif->port_id >= max_ports");
		UP_STATS(get_dp_port_info(0, 0)->tx_err_drop);
		MIB_G_STATS_INC(tx_drop);
		dev_kfree_skb_any(skb);
		goto exit;
	}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MPE_FASTHOOK_TEST)
	if (unlikely(ltq_mpe_fasthook_tx_fn))
		ltq_mpe_fasthook_tx_fn(skb, 0, NULL);
#endif

	port = get_dp_port_info(inst, rx_subif->port_id);
	if (unlikely(!rx_if && !is_dsl(port))) {
		printk_ratelimited("null dev but not DSL\n");
		dev_kfree_skb_any(skb);
		goto exit;
	}

	ex.dev = rx_if;
	ex.rx_subif = rx_subif;
	ex.port = port;
	vap = GET_VAP(ex.cmn.subif, port->vap_offset, port->vap_mask);
	ex.sif = get_dp_port_subif(port, vap);
	ex.mib = get_dp_port_subif_mib(ex.sif);
	ex.cmn.alloc_flags = port->alloc_flags;
	ret = dp_info->dp_tx(skb, &ex.cmn);
	if (likely(ret == DP_TX_FN_CONSUMED)) {
		MIB_G_STATS_INC(tx_pkts);
	} else if (ret != DP_TX_FN_BUSY) {
		if (ret == DP_TX_FN_CONTINUE)
			ret = DP_TX_FN_DROPPED;

		UP_STATS(ex.mib->tx_pkt_dropped);
		MIB_G_STATS_INC(tx_drop);

		/* In LGM Fail case DPM free skb, Success case CQM Free
		 * In PRX Fail and Success case CQM free
		 */
		if (is_soc_lgm(ex.cmn.inst))
			dev_kfree_skb_any(skb);
	}

	if (flags & DP_TX_NEWRET)
		return ret;

	if (ret == DP_TX_FN_BUSY) {
		UP_STATS(ex.mib->tx_pkt_dropped);
		MIB_G_STATS_INC(tx_drop);
		dev_kfree_skb_any(skb);
	}
exit:
	return ret ? DP_FAILURE : DP_SUCCESS;
}
EXPORT_SYMBOL(dp_xmit);

void set_dp_dbg_flag(u32 flags)
{
	dp_dbg_flag = flags;
}

u32 get_dp_dbg_flag(void)
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
	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (!cap || is_invalid_inst(cap->inst))
		return DP_FAILURE;
	if (!hw_cap_list[cap->inst].valid)
		return DP_FAILURE;
	*cap = hw_cap_list[cap->inst].info.cap;

	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_get_cap);

int dp_set_min_frame_len(s32 dp_port,
			 s32 min_frame_len,
			 u32 flags)
{
	pr_info("Dummy %s, need to implement later\n", __func__);
	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_set_min_frame_len);

int dp_rx_enable(struct net_device *netif, char *ifname, u32 flags)
{
	dp_subif_t subif = {0};
	struct pmac_port_info *port_info;
	int vap;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

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

int dp_set_bp_attr(struct dp_bp_attr *conf, u32 flag)
{
	struct pmac_port_info *port_info;
	dp_subif_t subif = {0};
	int ret = DP_SUCCESS;
	struct inst_info *dp_info;
	int vap;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (!conf) {
		DP_DEBUG(DP_DBG_FLAG_DBG, "conf passed is (%s)\n",
			 conf ? conf->dev->name : "NULL");
		return DP_FAILURE;
	}

	DP_LIB_LOCK(&dp_lock);

	if (dp_get_netif_subifid(conf->dev, NULL, NULL, NULL, &subif, 0)) {
		DP_LIB_UNLOCK(&dp_lock);
		return DP_FAILURE;
	}

	port_info = get_dp_port_info(conf->inst, subif.port_id);
	vap = GET_VAP(subif.subif, port_info->vap_offset,
		      port_info->vap_mask);
	get_dp_port_subif(port_info, vap)->cpu_port_en = conf->en;
	DP_LIB_UNLOCK(&dp_lock);

	dp_info = get_dp_prop_info(subif.inst);

	/* Null check is needed since some platforms dont have this API */
	if (!dp_info->dp_set_bp_attr)
		return DP_FAILURE;

	ret = dp_info->dp_set_bp_attr(conf, subif.bport, flag);

	return ret;
}
EXPORT_SYMBOL(dp_set_bp_attr);

int dp_lookup_mode_cfg(int inst, u32 lu_mode, struct dp_q_map *map,
		       u32 flag)
{
	cbm_queue_map_entry_t entry = {0};
	int ret = DP_SUCCESS;
	struct pmac_port_info *port_info;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (is_invalid_port(map->dp_port))
		return DP_FAILURE;

	port_info = get_dp_port_info(inst, map->dp_port);
	if (!port_info)
		return DP_FAILURE;

	DP_LIB_LOCK(&dp_lock);
	entry.ep = map->dp_port;
	if (!is_soc_lgm(inst)) {
		entry.mpe1 = map->mpe1;
		entry.mpe2 = map->mpe2;
	}
	if (flag & DP_CQM_LU_MODE_GET) {
		ret = cqm_mode_table_get(inst, &lu_mode, &entry, 0);
	} else {
		ret = cqm_mode_table_set(inst, &entry, lu_mode,
					 CBM_QUEUE_MAP_F_MPE1_DONTCARE |
					 CBM_QUEUE_MAP_F_MPE2_DONTCARE);

		port_info->cqe_lu_mode = lu_mode;
	}
	DP_LIB_UNLOCK(&dp_lock);

	return ret;
}
EXPORT_SYMBOL(dp_lookup_mode_cfg);

/*Return the table entry index based on dev:
 *success: >=0
 *fail: DP_FAILURE
 */
int bp_pmapper_get(int inst, struct net_device *dev)
{
	int i;
	struct bp_pmapper *bp_info;

	if (!dev)
		return -1;
	for (i = 0; i < ARRAY_SIZE(dp_bp_tbl[inst]); i++) {
		bp_info = get_dp_bp_info(inst, i);
		if (!bp_info->flag)
			continue;
		if (bp_info->dev == dev) {
			DP_DEBUG(DP_DBG_FLAG_PAE, "matched %s\n", dev->name);
			return i;
		}
	}
	return -1;
}

/* ethtool statistics support */
void dp_net_dev_get_ss_stat_strings(struct net_device *dev, u8 *data)
{
	/* This is for the Mib counter wraparound module */
	if (dp_get_dev_ss_stat_strings_fn) {
		DP_DEBUG(DP_DBG_FLAG_MIB,
			 "MIB call dp_get_dev_ss_stat_strings_fn callback\n");
		dp_get_dev_ss_stat_strings_fn(dev, data);
	}

	return;
}
EXPORT_SYMBOL(dp_net_dev_get_ss_stat_strings);

int dp_net_dev_get_ss_stat_strings_count(struct net_device *dev)
{
	/* This is for the Mib counter wraparound module */
	if (dp_get_dev_stat_strings_count_fn) {
		DP_DEBUG(DP_DBG_FLAG_MIB,
			 "MIB call dp_get_dev_stat_strings_count_fn callback\n");
		return dp_get_dev_stat_strings_count_fn(dev);
	}

	return 0;
}
EXPORT_SYMBOL(dp_net_dev_get_ss_stat_strings_count);

void dp_net_dev_get_ethtool_stats(struct net_device *dev,
				  struct ethtool_stats *stats, u64 *data)
{
	/* This is for the Mib counter wraparound module */
	if (dp_dev_get_ethtool_stats_fn) {
		DP_DEBUG(DP_DBG_FLAG_MIB,
			 "MIB call dp_dev_get_ethtool_stats_fn callback\n");
		return dp_dev_get_ethtool_stats_fn(dev, stats, data);
	}

	return;
}
EXPORT_SYMBOL(dp_net_dev_get_ethtool_stats);

int dp_spl_conn(int inst, struct dp_spl_cfg *conn)
{
	struct inst_info *dp_info = get_dp_prop_info(inst);

	if (is_invalid_inst(inst))
		return DP_FAILURE;

	if (!dp_late_init())
		return DP_FAILURE;

	if (!dp_info->dp_spl_conn)
		return DP_FAILURE;

	return dp_info->dp_spl_conn(inst, conn);
}
EXPORT_SYMBOL(dp_spl_conn);

int dp_spl_conn_get(int inst, enum DP_SPL_TYPE type,
		    struct dp_spl_cfg *conns, u8 cnt)
{
	struct inst_info *dp_info;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (is_invalid_inst(inst))
		return DP_FAILURE;

	if (!cnt || !conns) {
		pr_err("Wrong parameter\n");
		return DP_FAILURE;
	}

	dp_info = get_dp_prop_info(inst);

	if (!dp_info->dp_spl_conn_get)
		return DP_FAILURE;

	return dp_info->dp_spl_conn_get(inst, type, conns, cnt);
}
EXPORT_SYMBOL(dp_spl_conn_get);

int dp_get_io_port_info(struct dp_io_port_info *info, u32 flag)
{
	struct dp_dpdk_io_per_core *core_info;
	struct pmac_port_info *port_info;
	struct dp_subif_info *subif_info;
	int i, j, k = 0, vap;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (!info || is_invalid_inst(info->inst))
		return DP_FAILURE;

	DP_LIB_LOCK(&dp_lock);

	port_info = get_dp_port_info(info->inst, PMAC_CPU_ID);

	memset(info, 0, sizeof(*info));
	for (i = 0; i < DP_MAX_CORE; i++) {
		vap = 2 * i;
		subif_info = get_dp_port_subif(port_info, vap);
		if (!subif_info || subif_info->type != DP_DATA_PORT_DPDK)
			continue;

		core_info = &info->info.info.info[k++];
		core_info->core_id = i;
		info->info.info.num++;
		core_info->num_policy = subif_info->tx_policy_num;
		core_info->policy_base = subif_info->tx_policy_base;
		core_info->num_tx_push = subif_info->tx_pkt_credit;
		core_info->num_igp = 2;
		core_info->num_egp = 1;
		core_info->egp_id[0] = subif_info->cqm_deq_port[0];
		core_info->num_subif = 1;
		core_info->f_igp_qos[0] = 1;
		core_info->f_igp_qos[1] = 0;

		for (j = 0; j < 2; j++) {
			core_info->subif[j] = subif_info->subif + j;
			core_info->gpid[j] = subif_info->gpid + j;
			core_info->igp_id[j] = subif_info->igp_id + j;
		}
	}

	DP_LIB_UNLOCK(&dp_lock);

	return 0;
}
EXPORT_SYMBOL(dp_get_io_port_info);

int dp_set_datapath_io_port(struct dp_port_conf *conf, u32 flag)
{
	struct pmac_port_info *port_info;
	struct dp_subif_info *subif_info;
	struct inst_info *info = NULL;
	int i, dpid, max_subif;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (!conf || is_invalid_inst(conf->inst))
		return DP_FAILURE;

	dpid = conf->info.info.dpid;

	if (is_invalid_port(dpid))
		return DP_FAILURE;

	info = get_dp_prop_info(conf->inst);
	max_subif = info->cap.max_num_subif_per_port;

	DP_LIB_LOCK(&dp_lock);

	port_info = get_dp_port_info(conf->inst, dpid);
	if (!port_info) {
		DP_LIB_UNLOCK(&dp_lock);
		pr_err("Wrong dpid %d\n", dpid);
		return DP_FAILURE;
	}

	if (port_info->type == conf->info.info.type) {
		DP_LIB_UNLOCK(&dp_lock);
		return 0;
	}

	for (i = 0; i < max_subif; i++) {
		subif_info = &port_info->subif_info[i];
		if (subif_info->flags) {
			if (info->dp_set_io_port(conf->inst, dpid, i,
						 conf->info.info.type)) {
				DP_LIB_UNLOCK(&dp_lock);
				return DP_FAILURE;
			}
			subif_info->type = conf->info.info.type;
		}
	}

	port_info->type = conf->info.info.type;

	DP_LIB_UNLOCK(&dp_lock);

	return 0;
}
EXPORT_SYMBOL(dp_set_datapath_io_port);

int dp_get_datapath_io_port(struct dp_port_conf *conf, u32 flag)
{
	struct pmac_port_info *port_info;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (!conf || is_invalid_inst(conf->inst))
		return DP_FAILURE;

	DP_LIB_LOCK(&dp_lock);

	port_info = get_dp_port_info(conf->inst, conf->info.info.dpid);
	if (!port_info) {
		DP_LIB_UNLOCK(&dp_lock);
		pr_err("Wrong dpid %d\n", conf->info.info.dpid);
		return DP_FAILURE;
	}

	conf->info.info.type = port_info->type;

	DP_LIB_UNLOCK(&dp_lock);

	return 0;
}
EXPORT_SYMBOL(dp_get_datapath_io_port);

int dp_register_event_cb(struct dp_event *info, u32 flag)
{
	int ret;

	if (!info || is_invalid_inst(info->inst))
		return DP_FAILURE;

	if (flag & DP_F_DEREGISTER)
		ret = unregister_dp_event_notifier(info);
	else
		ret = register_dp_event_notifier(info);

	return ret;
}
EXPORT_SYMBOL(dp_register_event_cb);

int dp_free_buffer_by_policy(struct dp_buffer_info *info, u32 flag)
{
	struct cqm_bm_free data = {0};
	int ret;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

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

int dp_pre_init(void)
{
	/*mask to reset some field as SWAS required  all others try to keep */
	memset(dp_port_prop, 0, sizeof(dp_port_prop));
	memset(dp_port_info, 0, sizeof(dp_port_info));
	g_dp_dev = platform_device_register_simple("dp_plat_dev", 0, NULL, 0);
	if (IS_ERR(g_dp_dev)) {
		pr_err("register DP platform device fail\n");
		return -1;
	}
	dp_proc_install();
	dp_inst_init(0);
	dp_subif_list_init();
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	dp_switchdev_init();
#endif
	if (dp_init_pce()) {
		pr_err("%s: Datapath Init PCE Failed\n", __func__);
		return -1;
	}

	if (dp_tx_ctx_init(0))
		return -ENOMEM;
	return 0;
}

int dp_init_module(void)
{
	int res = 0;

	if (dp_init_ok) /*alredy init */
		return 0;
	register_notifier(0);
	register_dp_cap(0);
	if (request_dp(0)) { /*register 1st dp instance */
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

void dp_cleanup_module(void)
{
	int i;

	if (dp_init_ok) {
		DP_LIB_LOCK(&dp_lock);
		for (i = 0; i < dp_inst_num; i++) {
			DP_CB(i, dp_platform_set)(i, DP_PLATFORM_DE_INIT);
			free_dma_chan_tbl(i);
			free_dp_port_subif_info(i);
		}
		dp_init_ok = 0;

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_CPUFREQ)
		dp_cpufreq_notify_exit();
#endif
		DP_LIB_UNLOCK(&dp_lock);
		unregister_notifier(0);
		platform_device_unregister(g_dp_dev);
	}
}

/*!
 * @brief get network device's MTU
 * @param[in] dev: network device pointer
 * @param[out] mtu_size: return the maximum MTU can be supported
 *                       for this device based on current HW configuration
 * @return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_get_mtu_size(struct net_device *dev, u32 *mtu_size)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
	dp_subif_t subif;
	struct cbm_mtu mtu;
	struct pmac_port_info *port;
	struct dp_subif_info *sif;
#endif

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP31)
	return cbm_get_mtu_size(mtu_size);
#endif
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
	if (unlikely(dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0)))
		return DP_FAILURE;

	port = get_dp_port_info(subif.inst, subif.port_id);
	sif = get_dp_port_subif(port, GET_VAP(subif.subif, port->vap_offset,
					      port->vap_mask));

	mtu.cbm_inst = dp_port_prop[subif.inst].cbm_inst;
	mtu.dp_port = subif.port_id;
	mtu.policy_map = cqm_get_policy_map(subif.inst, sif->tx_policy_base,
					    sif->tx_policy_num,
					    port->alloc_flags,
					    TX_POLICYMAP);
	mtu.alloc_flag = port->alloc_flags;
	mtu.subif_flag = sif->flags;

	if (unlikely(cbm_get_mtu_size(&mtu)))
		return DP_FAILURE;
	mtu.mtu -= ETH_HLEN;
	if (is_stream_port(port->alloc_flags))
		mtu.mtu -= sizeof(struct pmac_tx_hdr);
	if (likely(mtu_size))
		*mtu_size = mtu.mtu;
#endif
	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_get_mtu_size);

int dp_set_mtu_size(struct net_device *dev, u32 mtu_size)
{
	dp_subif_t subif;
	struct inst_info *dp_info;
	struct pmac_port_info *port;
	int vap, rc;

	rc = dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0);
	if (unlikely(rc))
		return rc;
	DP_LIB_LOCK(&dp_lock);
	dp_info = get_dp_prop_info(subif.inst);
	port = get_dp_port_info(subif.inst, subif.port_id);
	vap = GET_VAP(subif.subif, port->vap_offset, port->vap_mask);
	if (unlikely(!dp_info->subif_platform_change_mtu))
		goto EXIT;
	rc = dp_info->subif_platform_change_mtu(subif.inst, subif.port_id,
						vap, mtu_size + ETH_HLEN);
EXIT:
	DP_LIB_UNLOCK(&dp_lock);
	return rc;
}
EXPORT_SYMBOL(dp_set_mtu_size);

int dp_set_net_dev_ops(struct net_device *dev, void *ops_cb, int ops_offset,
		       u32 flag)
{
	int res;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	DP_LIB_LOCK(&dp_lock);

	res = dp_set_net_dev_ops_priv(dev, ops_cb, ops_offset,
				      flag | DP_OPS_OWNER_EXT);

	DP_LIB_UNLOCK(&dp_lock);

	return res;
}
EXPORT_SYMBOL(dp_set_net_dev_ops);

int dp_get_dc_config(struct dp_dc_res *res, int flag)
{
	struct cbm_dc_res r;

	if (!res)
		return DP_FAILURE;

	r.cqm_inst = res->inst;
	r.dp_port = res->dp_port;
	r.res_id = res->res_id;
	r.alloc_flags = get_dp_port_info(res->inst, res->dp_port)->alloc_flags;

	if (cbm_dp_get_dc_config(&r, flag)) {
		pr_err("%s: %s failed, inst=%d, dp_port=%d, res_id=%d\n, alloc_flags=0x%x",
		       __func__, "cbm_dp_get_dc_config",
		       r.cqm_inst, r.dp_port, r.res_id, r.alloc_flags);
		return DP_FAILURE;
	}
	res->rx_res = r.rx_res;
	res->tx_res = r.tx_res;
	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_get_dc_config);

bool dp_is_ready(void)
{
	if (dp_init_ok > 0)
		return true;
	return false;
}
EXPORT_SYMBOL(dp_is_ready);

static int __init dp_init(void)
{
	return dp_pre_init();
}

static void __exit dp_exit(void)
{
	dp_cleanup_module();
}

static int __init dp_dbg_lvl_set(char *str)
{
	pr_info("\n\ndp_dbg=%s\n\n", str);
	dp_dbg_flag = dp_atoi(str);

	return 0;
}

early_param("dp_dbg", dp_dbg_lvl_set);

module_init(dp_init);
module_exit(dp_exit);

MODULE_LICENSE("GPL");

