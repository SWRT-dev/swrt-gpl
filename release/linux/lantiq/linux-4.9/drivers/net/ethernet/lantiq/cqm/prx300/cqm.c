#include "cqm.h"
#include "cqm_debugfs.h"
#include "../cqm_dev.h"
#include <linux/of_reserved_mem.h>
#include <net/datapath_proc_api.h>
#include <net/switch_api/gsw_flow_ops.h>
#include <net/switch_api/gsw_dev.h>

#define CQM_KB(x) ((x) * 1024UL)
#define LAN_0_PORT 7
#define LAN_1_PORT 15
#define LAN_PORT_NUM ((cqm_ctrl->gint_mode <  0) ? 1 : \
		      ((LAN_1_PORT - LAN_0_PORT) << cqm_ctrl->gint_mode))
#define MAX_PORT(FLAG) (((FLAG) & FLAG_LAN) ? LAN_PORT_NUM : 1)
#define OWN_BIT  BIT(31)
#define COMPLETE_BIT  BIT(30)
#define CQM_PON_IP_BASE_ADDR 0x181003FC
#define CQM_PON_IP_PKT_LEN_ADJ_BYTES 9
#define CQEM_PON_IP_IF_PIB_OVERSHOOT_BYTES 9260
#define PRX300_CQM_DROP_INIT ((PRX300_CQM_DROP_Q << 24) | \
			   (PRX300_CQM_DROP_Q << 16) | \
			   (PRX300_CQM_DROP_Q << 8) | \
			   PRX300_CQM_DROP_Q)
#define IS_CPU_PORT_TYPE(TYPE)((TYPE == DP_F_DEQ_CPU) ||\
			   (TYPE == DP_F_DEQ_CPU1) ||\
			   (TYPE == DP_F_DEQ_MPE) ||\
			   (TYPE == DP_F_DEQ_DL))
#define FSQM_FRM_NUM (CQM_SRAM_SIZE / CQM_SRAM_FRM_SIZE)
#define NUM_RX_RING 1
static const char cqm_name[] = "cqm";
static void __iomem *bufreq[CQM_PRX300_NUM_POOLS];
static void __iomem *eqmdesc[4];
static struct cbm_cntr_mode cbm_cntr_func[2];
static struct cqm_dqm_port_info dqm_port_info[CQM_PORT_MAX] = { {0} };
static struct cqm_eqm_port_info eqm_port_info[CQM_ENQ_PORT_MAX] = { {0} };
static struct cbm_desc_list g_cqm_dlist[4][CQM_DEQ_BUF_SIZE];
static bool touched[FSQM_FRM_NUM];
/*Maintain a shadow queue lookup table for fast
 *processing of queue map get API
 */
static struct cbm_qidt_shadow g_cbm_qidt_mirror[CQM_QIDT_DW_NUM];
static u32 g_qidt_help[0x4000];
static spinlock_t cqm_qidt_lock;
static spinlock_t cqm_port_map;
static spinlock_t cpu_pool_enq;
static struct pp_bmgr_policy_params p_param[CQM_PRX300_NUM_BM_POOLS];

LIST_HEAD(pmac_mapping_list);
static struct cqm_ctrl *cqm_ctrl;

static s32 cqm_dma_port_enable(s32 cqm_port_id, u32 flags, s32 type);
static s32 cqm_dq_dma_chan_init(s32 cqm_port_id, u32 flags);
static void init_cqm_deq_cpu_port(int idx, u32 tx_ring_size);
static s32 cqm_dequeue_dma_port_uninit(s32 cqm_port_id, u32 flags);
static void fill_dp_alloc_data(struct cbm_dp_alloc_data *data, int dp,
			       int port, u32 flags);
static void cqm_deq_port_flush(int port);

#ifdef ENABLE_LL_DEBUG
static void do_cbm_debug_tasklet(unsigned long cpu);
static struct tasklet_struct cbm_debug_tasklet;
#endif

#define FLAG_WAN (DP_F_FAST_ETH_WAN | DP_F_GPON | DP_F_EPON)
#define FLAG_LAN (DP_F_FAST_ETH_LAN | DP_F_GINT)
#define FLAG_ACA (DP_F_FAST_WLAN | DP_F_FAST_DSL)
#define FLAG_LSB_DONT_CARE 0x80000000
#define FLAG_VUNI (DP_F_VUNI)
#define Q_FLUSH_NO_PACKET_CNT 5

struct cqm_egp_map epg_lookup_table[] = {
	{0,              0,                0},
	{26,             2,                FLAG_WAN},
	{LAN_0_PORT,     3,                FLAG_LAN},
	{LAN_1_PORT,     4,                FLAG_LAN},
	{4,              CBM_PMAC_DYNAMIC, FLAG_ACA},
	{5,              CBM_PMAC_DYNAMIC, FLAG_ACA},
	{6,              CBM_PMAC_DYNAMIC, FLAG_ACA},
	{24,             CBM_PMAC_DYNAMIC, FLAG_VUNI},
	{LAN_1_PORT + 1, CBM_PMAC_DYNAMIC, DP_F_DIRECT},
};

struct cqm_buf_dbg_cnt cqm_dbg_cntrs[CQM_MAX_POLICY_NUM][CQM_MAX_POOL_NUM];

static int get_buff_resv_bytes(int cbm_inst, int size)
{
	int bsl_thr_val = 0x2800;

	if (size < cqm_ctrl->num_pools)
		bsl_thr_val = cqm_ctrl->prx300_pool_size[size] - BSL_THRES;
	else
		dev_dbg(cqm_ctrl->dev, "%s: unsupported size %d\n", __func__,
			size);
	return bsl_thr_val;
}

int find_dqm_port_type(int port)
{
	if ((port >= DQM_CPU_START_ID) && (port <= DQM_CPU_END_ID))
		return DQM_CPU_TYPE;
	if ((port >= DQM_ACA_START_ID) && (port <= DQM_ACA_END_ID))
		return DQM_ACA_TYPE;
	else if ((port >= DQM_DMA_START_ID) && (port <= DQM_DMA_END_ID))
		return DQM_DMA_TYPE;
	else if ((port >= DQM_PON_START_ID) && (port <= DQM_PON_END_ID))
		return DQM_PON_TYPE;
	else if ((port >= DQM_VUNI_START_ID) && (port <= DQM_VUNI_END_ID))
		return DQM_VUNI_TYPE;
	else
		return CBM_FAILURE;
}

void set_wlan_ep(int cbm_port)
{
	u32 *addr = 0;

	addr = cqm_ctrl->cqm + CBM_CTRL;
	switch (cbm_port) {
	case 4:
		set_val(addr, 1, BIT(18), 18);
		break;
	case 5:
		set_val(addr, 1, BIT(20), 20);
		break;
	case 6:
		set_val(addr, 1, BIT(22), 22);
		break;
	default:
		break;
	}
}

static inline void cqm_populate_entry(struct cqm_pmac_port_map *local_entry,
				      int *phys_port, u32 cbm_port,
				      u32 flags, int *found)
{
	int index, idx;
	u32 port_bit;
	*phys_port = cbm_port;

	for (idx = 0; idx < MAX_PORT(flags); idx++) {
		index = ((cbm_port + idx) / BITS_PER_LONG);
		port_bit = (cbm_port + idx) % BITS_PER_LONG;
		local_entry->egp_port_map[index] |= BIT(port_bit);
		dev_dbg(cqm_ctrl->dev, "egp_port_map[%d]->%u idx %d\n",
			index, local_entry->egp_port_map[index], idx);
	}

	local_entry->egp_type = flags;
	*found = 1;
}

static int is_excluded(u32 flags, u32 exclude_flags)
{
	if (flags & exclude_flags)
		return 1;
	else
		return 0;
}

static struct cqm_pmac_port_map *is_cbm_allocated(s32 cbm, u32 flags)
{
	struct cqm_pmac_port_map *ptr = NULL;
	unsigned long lock_flags;
	int num_deq_ports, i, j;
	u32 port_map, index;
	u32 exclude_flags = DP_F_DONTCARE;
	int skip = 0;

	if (!is_excluded(flags, exclude_flags))
		flags = DP_F_DONTCARE;
	spin_lock_irqsave(&cqm_port_map, lock_flags);
	list_for_each_entry(ptr, &pmac_mapping_list, list) {
		j = 0;
		if ((flags != DP_F_DONTCARE) && (ptr->egp_type & flags)) {
			spin_unlock_irqrestore(&cqm_port_map, lock_flags);
			return ptr;
		}
		if ((flags == DP_F_DONTCARE) && (j < PRX300_MAX_PORT_MAP)) {
			num_deq_ports = hweight_long(ptr->egp_port_map[j]);
			port_map = ptr->egp_port_map[j];
			skip = is_excluded(ptr->egp_type, exclude_flags);
			for (i = 0; i < num_deq_ports; i++) {
				index = get_is_bit_set(port_map);
				if ((index == cbm) && !skip) {
					spin_unlock_irqrestore(&cqm_port_map,
							       lock_flags);
					return ptr;
				}
				clear_bit(index, (unsigned long *)&port_map);
			}
			j++;
		}
	}
	spin_unlock_irqrestore(&cqm_port_map, lock_flags);
	return NULL;
}

static struct cqm_pmac_port_map *is_dp_allocated(s32 pmac, u32 flags)
{
	struct cqm_pmac_port_map *ptr = NULL;
	unsigned long lock_flags;
	u32 exclude_flags = DP_F_DONTCARE;

	if (!is_excluded(flags, exclude_flags))
		flags = DP_F_DONTCARE;
	spin_lock_irqsave(&cqm_port_map, lock_flags);
	if (flags & DP_F_DONTCARE) {
		list_for_each_entry(ptr, &pmac_mapping_list, list) {
			dev_dbg(cqm_ctrl->dev,
				"11:pmac %d type %d  input %d input %d\r\n",
				ptr->pmac, ptr->egp_type, pmac, flags);
			if ((ptr->pmac == pmac) &&
			    (!is_excluded(ptr->egp_type, exclude_flags))) {
				spin_unlock_irqrestore(&cqm_port_map,
						       lock_flags);
				return ptr;
			}
		}
	} else {
		list_for_each_entry(ptr, &pmac_mapping_list, list) {
			dev_dbg(cqm_ctrl->dev, "22:pmac %d type %d \r\n",
				ptr->pmac, ptr->egp_type);
			if (ptr->egp_type & flags) {
				spin_unlock_irqrestore(&cqm_port_map,
						       lock_flags);
				return ptr;
			}
		}
	}
	spin_unlock_irqrestore(&cqm_port_map, lock_flags);
	return NULL;
}

static u32 get_matching_pmac(u32 *ep, u32 flags, u32 *cbm_port, int pmac,
			     int pmac_present)
{
	int i, j = 0, result = CBM_NOTFOUND;

	for (i = 0; i < PRX300_MAX_PORT_PER_EP; i++) {
		ep[i] = CBM_PORT_INVALID;
		cbm_port[i] = CBM_PORT_INVALID;
	}
	for (i = 0; i < ARRAY_SIZE(epg_lookup_table); i++) {
		if (!(epg_lookup_table[i].port_type & flags))
			continue;

		if (j < PRX300_MAX_PORT_PER_EP) {
			if (pmac_present &&
			    (epg_lookup_table[i].pmac == pmac)) {
				ep[j] = epg_lookup_table[i].pmac;
				cbm_port[j] = epg_lookup_table[i].epg;
				if (cbm_port[j] == DMA_PORT_FOR_FLUSH)
					dev_err(cqm_ctrl->dev, "DMA port %d in use pmac %d\n",
						cbm_port[j], pmac);
				j++;
			}
			if (!pmac_present) {
				if (!is_cbm_allocated(
					epg_lookup_table[i].epg, flags)) {
					ep[j] = epg_lookup_table[i].pmac;
					cbm_port[j] = epg_lookup_table[i].epg;
					if (cbm_port[j] ==
					    DMA_PORT_FOR_FLUSH) {
						dev_err(cqm_ctrl->dev,
							"DMA port %d in use pmac %d\n",
							cbm_port[j], pmac);
					}
					j++;
				}
			}
			result = CBM_SUCCESS;
		} else {
			break;
		}
	}
	return result;
}

static void set_intr_to_line(int index, int line)
{
	cqm_ctrl->cbm_line[index] = line;
}

static inline int get_intr_to_line(int intr, int *line)
{
	int i;

	for (i = 0; i < PRX300_MAX_INTR_LINE; i++) {
		if (cqm_ctrl->cbm_irq[i] == intr) {
			*line = cqm_ctrl->cbm_line[i];
			return CBM_SUCCESS;
		}
	}
	return CBM_FAILURE;
}

static int cqm_delete_from_list(s32 pmac, u32 flags)
{
	struct cqm_pmac_port_map *ptr = NULL;
	struct cqm_pmac_port_map *next = NULL;
	int found = 0;
	unsigned long lock_flags;
	u32 exclude_flags = DP_F_DONTCARE;

	if (!is_excluded(flags, exclude_flags))
		flags = DP_F_DONTCARE;
	dev_dbg(cqm_ctrl->dev, "%s: flags 0x%x\r\n", __func__, flags);

	spin_lock_irqsave(&cqm_port_map, lock_flags);

	list_for_each_entry_safe(ptr, next, &pmac_mapping_list, list) {
		if (ptr->pmac == pmac) {
			if (flags == DP_F_DONTCARE) {
				found = 1;
				break;
			} else if (ptr->egp_type & flags) {
				found = 1;
				break;
			}
		}
	}
	if (found) {
		list_del(&ptr->list);
		devm_kfree(cqm_ctrl->dev, ptr);
		spin_unlock_irqrestore(&cqm_port_map, lock_flags);
		return 1;
	}
	spin_unlock_irqrestore(&cqm_port_map, lock_flags);
	return 0;
}

static struct cqm_pmac_port_map *cqm_add_to_list(struct cqm_pmac_port_map
						     *val)
{
	struct cqm_pmac_port_map *ptr = NULL;
	int i = 0;
	unsigned long flags;

	dev_dbg(cqm_ctrl->dev, "\n new linked list\n");
	ptr = devm_kzalloc(cqm_ctrl->dev, sizeof(*ptr), GFP_ATOMIC);
	if (!ptr)
		return NULL;
	dev_dbg(cqm_ctrl->dev, "\n %s : 1\n", __func__);
	ptr->pmac = val->pmac;
	for (i = 0; i < PRX300_MAX_PORT_MAP; i++)
		ptr->egp_port_map[i] = val->egp_port_map[i];
	ptr->egp_type = val->egp_type;
	ptr->owner = 0;
	ptr->dev = 0;
	ptr->dev_port = 0;
	ptr->flags = P_ALLOCATED;
	dev_dbg(cqm_ctrl->dev, "\n %s : 2\n", __func__);
	spin_lock_irqsave(&cqm_port_map, flags);
	/* Init the list within the struct. */
	INIT_LIST_HEAD(&ptr->list);
	/* Add this struct to the tail of the list. */
	list_add_tail(&ptr->list, &pmac_mapping_list);
	spin_unlock_irqrestore(&cqm_port_map, flags);
	return ptr;
}

static u32 get_matching_flag(u32 *flags, u32 cqm_port)
{
	int i, result = CBM_NOTFOUND;

	for (i = 0; i < ARRAY_SIZE(epg_lookup_table); i++) {
		if (epg_lookup_table[i].epg == cqm_port) {
			*flags = epg_lookup_table[i].port_type;
			result = CBM_SUCCESS;
			break;
		}
	}
	return result;
}

static s32 dp_port_resources_get(u32 *dp_port, u32 *num_cbm_ports,
				 struct cqm_res_t **res_pp,
				 u32 flags)
{
	int i = 0, j = 0;
	u32 port_map;
	struct cqm_res_t *res;
	struct cqm_pmac_port_map *local_entry = NULL;

	dev_dbg(cqm_ctrl->dev, "%s: flags 0x%x dp %d\r\n", __func__, flags,
		*dp_port);
	if (dp_port) {
		dev_dbg(cqm_ctrl->dev, "%s: dp %d\r\n", __func__, *dp_port);
		local_entry = is_dp_allocated(*dp_port, flags);
	} else {
		local_entry = is_dp_allocated(0, flags);
	}
	*num_cbm_ports = 0;
	if (!local_entry)
		return CBM_FAILURE;
	for (i = 0; i < PRX300_MAX_PORT_MAP; i++)
		*num_cbm_ports += hweight_long(local_entry->egp_port_map[i]);
	if ((*num_cbm_ports > PRX300_MAX_PON_PORTS) || (*num_cbm_ports == 0))
		return CBM_FAILURE;
	res = devm_kzalloc(cqm_ctrl->dev, sizeof(*res) * (*num_cbm_ports),
			   GFP_ATOMIC);
	if (!res)
		return CBM_FAILURE;
	*res_pp = res;
	for (i = 0; i < PRX300_MAX_PORT_MAP; i++) {
		port_map = local_entry->egp_port_map[i];
		while ((port_map) && (j < *num_cbm_ports)) {
			res[j].cqm_deq_port = get_is_bit_set(port_map);
			/*clear the flag for the current bitpos*/
			clear_bit(res[j].cqm_deq_port
					, (unsigned long *)&port_map);
			j++;
		}
	}
	return CBM_SUCCESS;
}

#define IS_CPU_POOL(pool, policy) ((!(pool)) && ((policy) == 7))

static void get_bm_info(struct cqm_bm_pool_config *tmp_bm, s32 type)
{
	u32 i;

	for (i = 0; i < ARRAY_SIZE(bm_pool_conf); i++) {
		if (bm_pool_conf[i].buf_type != type)
			continue;
		tmp_bm->buf_type = type;
		tmp_bm->offset_mask = bm_pool_conf[i].offset_mask;
		tmp_bm->segment_mask = bm_pool_conf[i].segment_mask;
		tmp_bm->pool = bm_pool_conf[i].pool;
		tmp_bm->policy = bm_pool_conf[i].policy;
		tmp_bm->buf_frm_size = bm_pool_conf[i].buf_frm_size;
		return;
	}
}

static int get_buffer_base_index(u32 v_buf, u32 *buff_base,
				 u32 *buff_size, int *index)
{
	int i = 0, var_offset;
	u32 temp_buf_start, temp_buf_end;

	while (i < CQM_PRX300_NUM_POOLS) {
		temp_buf_start = bm_pool_conf[i].pool_start_low;
		temp_buf_end = bm_pool_conf[i].pool_end_low;
		dev_dbg(cqm_ctrl->dev, "0x%x 0x%x 0x%x\n",
			v_buf, temp_buf_start, temp_buf_end);
		if ((v_buf >= temp_buf_start) && (v_buf < temp_buf_end)) {
			dev_dbg(cqm_ctrl->dev, "0x%x 0x%x 0x%x\n",
				v_buf, temp_buf_start, temp_buf_end);
			*buff_size = bm_pool_conf[i].buf_frm_size;
			var_offset = v_buf - bm_pool_conf[i].pool_start_low;
			var_offset %= *buff_size;
			*buff_base =  (v_buf - var_offset);
			*index = i;
			dev_dbg(cqm_ctrl->dev, "0x%x 0x%x 0x%x\n",
				*buff_size, *buff_base, *index);
			return CBM_SUCCESS;
		}
		i++;
	}

	dev_err(cqm_ctrl->dev, "Buffer doesn't belong to the pool\n");
	return CBM_FAILURE;
}

static void store_bufreq_baseaddr(void)
{
	void *enq = cqm_ctrl->enq;

	bufreq[0] = enq + EQ_CPU_PORT(0, new_s0ptr);
	bufreq[1] = enq + EQ_CPU_PORT(0, new_s1ptr);
	bufreq[2] = enq + EQ_CPU_PORT(0, new_s2ptr);
	bufreq[3] = enq + EQ_CPU_PORT(0, new_s3ptr);
	bufreq[4] = enq + EQ_CPU_PORT(0, new_pbptr);
#ifdef CONFIG_CBM_LS_ENABLE
	dqmdesc[0] = enq + DQ_CPU_PORT(2, desc0.desc0);
	dqmdesc[1] = enq + DQ_CPU_PORT(2, desc1.desc0);
#endif
	eqmdesc[0] = enq + EQ_CPU_PORT(0, desc0.desc0);
	eqmdesc[1] = enq + EQ_CPU_PORT(1, desc0.desc0);
	eqmdesc[2] = enq + EQ_CPU_PORT(2, desc0.desc0);
	eqmdesc[3] = enq + EQ_CPU_PORT(3, desc0.desc0);
	cbm_cntr_func[0].reg = enq + CBM_EQM_CTRL;
	cbm_cntr_func[1].reg = enq + CBM_DQM_CTRL;
}

static s32 pib_status_get(struct pib_stat *ctrl)
{
	u32 reg = cbm_r32(cqm_ctrl->pib + PIB_STATUS);

	ctrl->en_stat = (reg & PIB_STATUS_PIB_EN_STS_MASK) >>
			 PIB_STATUS_PIB_EN_STS_POS;
	ctrl->pause_stat = (reg & PIB_STATUS_PIB_PAUSE_STS_MASK) >>
			    PIB_STATUS_PIB_PAUSE_STS_POS;
	ctrl->fifo_size = (reg & PIB_STATUS_FIFO_SIZE_MASK) >>
			   PIB_STATUS_FIFO_SIZE_POS;
	ctrl->fifo_full = (reg & PIB_STATUS_FIFO_FULL_MASK) >>
			   PIB_STATUS_FIFO_FULL_POS;
	ctrl->fifo_empty = (reg & PIB_STATUS_FIFO_EMPTY_MASK) >>
			    PIB_STATUS_FIFO_EMPTY_POS;
	ctrl->cmd_ovflw = (reg & PIB_STATUS_FIFO_OVFL_MASK) >>
			   PIB_STATUS_FIFO_OVFL_POS;
	ctrl->cmd_illegal_port = (reg & PIB_STATUS_ILLEGAL_PORT_MASK) >>
				  PIB_STATUS_ILLEGAL_PORT_POS;
	ctrl->wakeup_intr = (reg & PIB_STATUS_WAKEUP_INT_MASK) >>
			     PIB_STATUS_WAKEUP_INT_POS;
	return CBM_SUCCESS;
}

static s32 pib_ovflw_cmd_get(u32 *cmd)
{
	*cmd = cbm_r32(cqm_ctrl->pib + PIB_FIFO_OVFL_CMD_REG);
	return CBM_SUCCESS;
}

static s32 pib_illegal_cmd_get(u32 *cmd)
{
	*cmd = cbm_r32(cqm_ctrl->pib + PIB_ILLEGAL_CMD_REG);
	return CBM_SUCCESS;
}

static s32 config_pib_ctrl(struct pib_ctrl *ctrl, u32 flags)
{
	void *base = (cqm_ctrl->pib + PIB_CTRL);

	if (flags & FLAG_PIB_ENABLE)
		set_val(base, ctrl->pib_en, PIB_CTRL_PIB_EN_MASK,
			PIB_CTRL_PIB_EN_POS);
	if (flags & FLAG_PIB_PAUSE)
		set_val(base, ctrl->pib_pause, PIB_CTRL_PIB_PAUSE_MASK,
			PIB_CTRL_PIB_PAUSE_POS);
	if (flags & FLAG_PIB_OVFLW_INTR)
		set_val(base, ctrl->cmd_ovflw_intr_en,
			PIB_CTRL_OVRFLW_INT_EN_MASK,
			PIB_CTRL_OVRFLW_INT_EN_POS);
	if (flags & FLAG_PIB_ILLEGAL_INTR)
		set_val(base, ctrl->cmd_illegal_port_intr_en,
			PIB_CTRL_ILLEGAL_PORT_INT_EN_MASK,
			PIB_CTRL_ILLEGAL_PORT_INT_EN_POS);
	if (flags & FLAG_PIB_WAKEUP_INTR)
		set_val(base, ctrl->wakeup_intr_en,
			PIB_CTRL_WAKEUP_INT_EN_MASK,
			PIB_CTRL_WAKEUP_INT_EN_POS);
	if (flags & FLAG_PIB_BYPASS)
		set_val(base, ctrl->pib_bypass, PIB_CTRL_PIB_BYPASS_MASK,
			PIB_CTRL_PIB_BYPASS_POS);
	if (flags & FLAG_PIB_DELAY)
		set_val(base, ctrl->deq_delay, PIB_CTRL_DQ_DLY_MASK,
			PIB_CTRL_DQ_DLY_POS);
	if (flags & FLAG_PIB_PKT_LEN_ADJ)
		set_val(base, ctrl->pkt_len_adj, PIB_CTRL_PKT_LEN_ADJ_EN_MASK,
			PIB_CTRL_PKT_LEN_ADJ_EN_POS);
	return CBM_SUCCESS;
}

static s32 pib_port_enable(u32 deq_port, int enable)
{
	int port = (deq_port - DQM_PON_START_ID);

	if (port < 32)
		set_val((cqm_ctrl->pib + PIB_PORT_EN_31_0),
			enable, (PIB_PORT_EN_31_0_EN_PORT0_MASK << port), port);
	else
		set_val((cqm_ctrl->pib + PIB_PORT_EN_64_32),
			enable, (PIB_PORT_EN_64_32_EN_PORT0_MASK << port),
		port);
	return CBM_SUCCESS;
}

static s32 pon_deq_cntr_get(int port, u32 *count)
{
	if (port < PRX300_MAX_PON_PORTS) {
		cbm_r32((cqm_ctrl->pon_dqm_cntr + (port * 4)));
		return CBM_SUCCESS;
	} else {
		return CBM_FAILURE;
	}
}

static s32 pib_program_overshoot(u32 overshoot_bytes)
{
	cbm_w32((cqm_ctrl->pib + PIB_OVERSHOOT_BYTES),
		(overshoot_bytes & PIB_OVERSHOOT_BYTES_OVERSHOOT_MASK));
	return CBM_SUCCESS;
}

static s32 enable_backpressure(s32 port_id, bool flag)
{
	void *deq = cqm_ctrl->deq;
	int retval = CBM_SUCCESS;
	u32 reg_off, config;

	dev_dbg(cqm_ctrl->dev, "%s enter port_id %d enable: %d\n",
		__func__, port_id, flag);

	switch (find_dqm_port_type(port_id)) {
	case DQM_PON_TYPE:
		reg_off = (((port_id - DQM_PON_START_ID) >= BITS_PER_LONG)
				? EPON_PORTBP_REG_1 : EPON_PORTBP_REG_0);
		cbm_w32(cqm_ctrl->enq + reg_off, flag);
		break;
	case DQM_ACA_TYPE:
		config = cbm_r32(deq + DQ_CPU_PORT(port_id, cfg));
		if (flag)
			config |= CFG_ACA_EGP_4_DQBPEN_MASK;
		else
			config &= ~(CFG_ACA_EGP_4_DQBPEN_MASK);
		cbm_w32((deq + DQ_CPU_PORT(port_id, cfg)), config);
		dev_dbg(cqm_ctrl->dev, "0x%x\n",
			(u32)(deq + DQ_CPU_PORT(port_id, cfg)));
		break;
	default:
		dev_err(cqm_ctrl->dev, "Invalid port_id: %u", port_id);
		retval = CBM_FAILURE;
		break;
	}

	dev_dbg(cqm_ctrl->dev, "%s exit retval %d\n", __func__, retval);
	return retval;
}

static inline void handle_state(int *state, int nxt_state, int done_state,
				u32 limit, u8 *done, u32 value, u32 mask,
				u32 *qidt_value, u32 *idx)
{
	*state = nxt_state;
	if (*done) {
		*state = done_state;
		*done = 0;
		*idx = 0;
		return;
	}
	if ((mask) && (!(*done))) {
		*qidt_value = *idx;
		*idx += 1;
		if (*idx >= limit)
			*done = 1;
	} else {
		*qidt_value = value;
		*done = 1;
	}
}

static s32 qid2ep_map_set(int qid, int port)
{
	cbm_w32(cqm_ctrl->qid2ep + (qid * 4), port);
	return CBM_SUCCESS;
}

static s32 qid2ep_map_get(int qid, int *port)
{
	*port = cbm_r32(cqm_ctrl->qid2ep + (qid * 4));
	return CBM_SUCCESS;
}

u32 epon_mode_reg_get(void)
{
	return cbm_r32(cqm_ctrl->enq + EPON_EPON_MODE_REG);
}

static s32 mode_table_get(int cbm_inst, int *mode,
			  cbm_queue_map_entry_t *entry, u32 flags)
{
	u32 mpe, ep, reg_value;
	void *c_base = cqm_ctrl->cqm;

	mpe = (entry->mpe1 & 0x1) | ((entry->mpe2 & 0x1) << 1);
	ep = entry->ep;
	reg_value = cbm_r32(c_base + CBM_QID_MODE_SEL_REG_0 + (mpe * 4));
	*mode = (reg_value >> (ep * 2)) & 0x3;
	return CBM_SUCCESS;
}

static s32 mode_table_set(int cbm_inst, cbm_queue_map_entry_t *entry,
			  u32 mode, u32 flags)
{
	u32 mpe, ep;
	struct cqm_qidt_elm qidt_idx = {0};
	struct cqm_qidt_elm qidt_elm = {0};
	int state;
	struct qidt_flag_done flag_done = {0};
	void *c_base = cqm_ctrl->cqm;

	mpe = (entry->mpe1 & 0x1) | ((entry->mpe2 & 0x1) << 1);
	ep = entry->ep;
	state = STATE_EP;
	while (state != STATE_NONE) {
		switch (state) {
		case STATE_EP:
			handle_state(&state, STATE_MPE1, STATE_NONE, 16,
				     &flag_done.ep_done, entry->ep,
				     (flags & CBM_QUEUE_MAP_F_EP_DONTCARE),
				     &qidt_elm.ep, &qidt_idx.ep);
			break;
		case STATE_MPE1:
			handle_state(&state, STATE_MPE2, STATE_EP, 2,
				     &flag_done.mpe1_done, entry->mpe1,
				     (flags & CBM_QUEUE_MAP_F_MPE1_DONTCARE),
				     &qidt_elm.mpe1, &qidt_idx.mpe1);
			break;
		case STATE_MPE2:
			handle_state(&state, STATE_MPE2, STATE_MPE1, 2,
				     &flag_done.mpe2_done, entry->mpe2,
				     (flags & CBM_QUEUE_MAP_F_MPE2_DONTCARE),
				     &qidt_elm.mpe2, &qidt_idx.mpe2);
			mpe = (qidt_elm.mpe1 & 0x1) |
			      ((qidt_elm.mpe2 & 0x1) << 1);
			ep = qidt_elm.ep;
			set_val(c_base + CBM_QID_MODE_SEL_REG_0 + (mpe * 4),
				mode, (0x3 << (ep * 2)),
			(ep * 2));
		case STATE_NONE:
			break;
		};
	}
	return CBM_SUCCESS;
}

static void cqm_qid_program(u8 qid_val, u32 qidt)
{
	u32 qidt_idx;
	u32 qidt_offset;
	u32 offset_factor, value_mask;
	u32 value, shadow;
	void *qidt_base = cqm_ctrl->qidt;

	qidt_idx = qidt >> 2;
	qidt_offset = qidt % 4;
	offset_factor = qidt_offset << 3;
	value_mask = PRX300_CQM_Q_MASK << offset_factor;

	shadow = g_cbm_qidt_mirror[qidt_idx].qidt_shadow;
	value = (shadow & ~value_mask) |
		((qid_val & PRX300_CQM_Q_MASK) << offset_factor);
	g_cbm_qidt_mirror[qidt_idx].qidt_shadow = value;
	cbm_w32((qidt_base + qidt_idx * 4), value);
}

static void cqm_qid_reg_set(struct cqm_qidt_elm *qidt_elm, u8 qid_val,
			    u8 sel_field)
{
	u32 qidt;

	qidt = ((((sel_field & 0xf0) >> 0x4) << PRX300_SEL7TO4_POS)
		| ((qidt_elm->mpe2 & 0x1) << PRX300_MPE2_POS)
		| ((qidt_elm->mpe1 & 0x1) << PRX300_MPE1_POS)
		| ((qidt_elm->ep & 0xf) << PRX300_EP_POS)
		| ((sel_field & 0x0f) << PRX300_SEL3TO0_POS));
	cqm_qid_program(qid_val, qidt);
}

static void set_lookup_qid_via_index_prx300(struct cbm_lookup *info)
{
	unsigned long sys_flag;

	spin_lock_irqsave(&cqm_qidt_lock, sys_flag);
	cqm_qid_program(info->qid, info->index);
	spin_unlock_irqrestore(&cqm_qidt_lock, sys_flag);
}

static u8 get_lookup_qid_via_idx_prx300(struct cbm_lookup *info)
{
	u32 index = info->index;
	u32 offset = (index / 4) * 4;
	u32 shift = (index % 4) * 8;
	unsigned long sys_flag;
	u8 value = 0;
	void *qidt = cqm_ctrl->qidt;

	spin_lock_irqsave(&cqm_qidt_lock, sys_flag);
	value = ((cbm_r32(qidt + offset)) >> shift) & PRX300_CQM_Q_MASK;
	spin_unlock_irqrestore(&cqm_qidt_lock, sys_flag);
	return value;
}

static void map_to_drop_q(struct cbm_lookup_entry *lu_entry)
{
	unsigned long sys_flag;
	u8 value = 0;
	int i = 0;
	u32 offset, shift;
	void *qidt = cqm_ctrl->qidt;

	spin_lock_irqsave(&cqm_qidt_lock, sys_flag);

	for (i = 0; i < MAX_LOOKUP_TBL_SIZE; i++) {
		offset = (i / 4) * 4;
		shift = (i % 4) * 8;
		value = ((cbm_r32(qidt + offset)) >> shift) & PRX300_CQM_Q_MASK;

		/* if match, set the index to drop q */
		if (lu_entry->qid == value) {
			lu_entry->entry[lu_entry->num[0]++] = i;
			cqm_qid_program(lu_entry->ppv4_drop_q, i);
		}
	}

	spin_unlock_irqrestore(&cqm_qidt_lock, sys_flag);
}

static void restore_orig_q(const struct cbm_lookup_entry *lu_entry)
{
	unsigned long sys_flag;
	int i = 0;

	spin_lock_irqsave(&cqm_qidt_lock, sys_flag);

	for (i = 0; i < lu_entry->num[0]; i++)
		cqm_qid_program(lu_entry->qid, lu_entry->entry[i]);

	spin_unlock_irqrestore(&cqm_qidt_lock, sys_flag);
}

static inline void find_next_sub_if_id(u32 *idx_p, u32 mask, u32 mask_all,
				       u32 limit_v, u32 start_v)
{
	if (*idx_p < start_v)
		*idx_p = start_v;
	if (mask && (*idx_p) && mask_all) {
		while ((start_v != ((*idx_p) & (~mask))) &&
		       ((*idx_p) < limit_v)) {
			*idx_p =  *idx_p + 1;
		}
	}
}

static inline void sub_if_id_masking(u32 sub_if_id, u32 *st, u32 *lim,
				     u32 in_mask, u32 *bit_mask)
{
	if (in_mask & 1) {
		*bit_mask = in_mask >> SUB_IF_ID_MASK_SHF;
		if (*bit_mask) {
			*st = sub_if_id & (~(*bit_mask));
			*lim = ((*st) | (*bit_mask)) + 1;
		}
	}
}

static void cqm_qidt_set_mode0(const struct cqm_qidt_elm *qid_set,
			       const struct cqm_qidt_mask *qid_mask,
			       u8 qid_val)
{
	struct cqm_qidt_elm qidt_idx = {0};
	struct cqm_qidt_elm qidt_elm = {0};
	int state;
	struct qidt_flag_done flag_done = {0};
	u8 sel_field = 0;

	dev_dbg(cqm_ctrl->dev, "%u\n", qid_val);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_set->clsid,
		qid_set->ep, qid_set->mpe1, qid_set->mpe2);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_set->enc,
		qid_set->dec, qid_set->flowidh, qid_set->flowidl);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_mask->classid_mask,
		qid_mask->ep_mask, qid_mask->mpe1_mask, qid_mask->mpe2_mask);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_mask->enc_mask,
		qid_mask->dec_mask, qid_mask->flowid_hmask,
		qid_mask->flowid_lmask);
	state = STATE_CLASS;
	while (state != STATE_NONE) {
		switch (state) {
		case STATE_CLASS:
			handle_state(&state, STATE_ENC, STATE_NONE, 16,
				     &flag_done.cls_done, qid_set->clsid,
				     qid_mask->classid_mask, &qidt_elm.clsid,
				     &qidt_idx.clsid);
			break;
		case STATE_ENC:
			handle_state(&state, STATE_DEC, STATE_CLASS, 2,
				     &flag_done.enc_done, qid_set->enc,
				     qid_mask->enc_mask, &qidt_elm.enc,
				     &qidt_idx.enc);
			break;
		case STATE_DEC:
			handle_state(&state, STATE_FLOWID_LOW, STATE_ENC, 2,
				     &flag_done.dec_done, qid_set->dec,
				     qid_mask->dec_mask, &qidt_elm.dec,
				     &qidt_idx.dec);
			break;
		case STATE_FLOWID_LOW:
			handle_state(&state, STATE_FLOWID_HIGH, STATE_DEC, 2,
				     &flag_done.fl_done, qid_set->flowidl,
				     qid_mask->flowid_lmask, &qidt_elm.flowidl,
				     &qidt_idx.flowidl);
			break;
		case STATE_FLOWID_HIGH:
			handle_state(&state, STATE_EP, STATE_FLOWID_LOW, 2,
				     &flag_done.fh_done, qid_set->flowidh,
				     qid_mask->flowid_hmask, &qidt_elm.flowidh,
				     &qidt_idx.flowidh);
			sel_field = ((qidt_elm.flowidh & 0x1) << 7)
			| ((qidt_elm.flowidl & 0x1) << 6)
			| ((qidt_elm.dec & 0x1) << 5)
			| ((qidt_elm.enc & 0x1) << 4)
			| ((qidt_elm.clsid & 0xf));
			break;
		case STATE_EP:
			handle_state(&state, STATE_MPE1, STATE_FLOWID_HIGH, 16,
				     &flag_done.ep_done, qid_set->ep,
				     qid_mask->ep_mask, &qidt_elm.ep,
				     &qidt_idx.ep);
			break;
		case STATE_MPE1:
			handle_state(&state, STATE_MPE2, STATE_EP, 2,
				     &flag_done.mpe1_done, qid_set->mpe1,
				     qid_mask->mpe1_mask, &qidt_elm.mpe1,
				     &qidt_idx.mpe1);
			break;
		case STATE_MPE2:
			handle_state(&state, STATE_MPE2, STATE_MPE1, 2,
				     &flag_done.mpe2_done, qid_set->mpe2,
				     qid_mask->mpe2_mask, &qidt_elm.mpe2,
				     &qidt_idx.mpe2);
			if (!flag_done.mpe2_done)
				cqm_qid_reg_set(&qidt_elm, qid_val, sel_field);
			break;
		case STATE_NONE:
			break;
		};
	}
}

static void cqm_qidt_set_mode1(const struct cqm_qidt_elm *qid_set,
			       const struct cqm_qidt_mask *qid_mask,
			       u8 qid_val)
{
	struct cqm_qidt_elm qidt_idx = {0};
	struct cqm_qidt_elm qidt_elm = {0};
	int state;
	struct qidt_flag_done flag_done = {0};
	u32 limit = MODE1_SUB_IF_ID_LIMIT, masking_bits = 0, start = 0;
	u32 sub_if_id_bit_mask =
		((qid_mask->sub_if_id_mask >> SUB_IF_ID_MODE_1_POS) &
		(SUB_IF_ID_MODE_1_MASK << SUB_IF_ID_MASK_SHF)) |
		(qid_mask->sub_if_id_mask & 0x01);
	u8 sel_field = 0;

	dev_dbg(cqm_ctrl->dev, "%u\n", qid_val);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_set->clsid,
		qid_set->ep, qid_set->mpe1, qid_set->mpe2);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_set->enc,
		qid_set->dec, qid_set->flowidh, qid_set->flowidl);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_mask->classid_mask,
		qid_mask->ep_mask, qid_mask->mpe1_mask, qid_mask->mpe2_mask);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_mask->enc_mask,
		qid_mask->dec_mask, qid_mask->flowid_hmask,
		qid_mask->flowid_lmask);
	state = STATE_SUBIF_ID;
	sub_if_id_masking(((qid_set->sub_if_id >> SUB_IF_ID_MODE_1_POS) &
			  SUB_IF_ID_MODE_1_MASK), &start, &limit,
			  sub_if_id_bit_mask, &masking_bits);
	qidt_elm.sub_if_id = start;
	qidt_idx.sub_if_id = start;
	while (state != STATE_NONE) {
		switch (state) {
		case STATE_SUBIF_ID:
			handle_state(&state, STATE_EP, STATE_NONE, limit,
				     &flag_done.sub_if_id_done,
				     (qid_set->sub_if_id & 0xff),
				     qid_mask->sub_if_id_mask,
				     &qidt_elm.sub_if_id, &qidt_idx.sub_if_id);
			sel_field = qidt_elm.sub_if_id;
			find_next_sub_if_id(&qidt_idx.sub_if_id, masking_bits,
					    (qid_mask->sub_if_id_mask & 1),
					    limit, start);
			break;
		case STATE_EP:
			handle_state(&state, STATE_MPE1, STATE_SUBIF_ID, 16,
				     &flag_done.ep_done, qid_set->ep,
				     qid_mask->ep_mask, &qidt_elm.ep,
				     &qidt_idx.ep);
			break;
		case STATE_MPE1:
			handle_state(&state, STATE_MPE2, STATE_EP, 2,
				     &flag_done.mpe1_done, qid_set->mpe1,
				     qid_mask->mpe1_mask, &qidt_elm.mpe1,
				     &qidt_idx.mpe1);
			break;
		case STATE_MPE2:
			handle_state(&state, STATE_MPE2, STATE_MPE1, 2,
				     &flag_done.mpe2_done, qid_set->mpe2,
				     qid_mask->mpe2_mask, &qidt_elm.mpe2,
				     &qidt_idx.mpe2);
			if (!flag_done.mpe2_done)
				cqm_qid_reg_set(&qidt_elm, qid_val, sel_field);
			break;
		case STATE_NONE:
			break;
		};
	}
}

static void cqm_qidt_set_mode2(const struct cqm_qidt_elm *qid_set,
			       const struct cqm_qidt_mask *qid_mask,
			       u8 qid_val)
{
	struct cqm_qidt_elm qidt_idx = {0};
	struct cqm_qidt_elm qidt_elm = {0};
	int state;
	struct qidt_flag_done flag_done = {0};
	u32 limit = MODE2_SUB_IF_ID_LIMIT, masking_bits = 0, start = 0;
	u32 sub_if_id_bit_mask =
		((qid_mask->sub_if_id_mask >> SUB_IF_ID_MODE_2_POS) &
		(SUB_IF_ID_MODE_2_MASK << SUB_IF_ID_MASK_SHF)) |
		(qid_mask->sub_if_id_mask & 0x01);
	u8 sel_field = 0;

	dev_dbg(cqm_ctrl->dev, "%u\n", qid_val);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_set->clsid,
		qid_set->ep, qid_set->mpe1, qid_set->mpe2);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_set->enc,
		qid_set->dec, qid_set->flowidh, qid_set->flowidl);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_mask->classid_mask,
		qid_mask->ep_mask, qid_mask->mpe1_mask, qid_mask->mpe2_mask);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_mask->enc_mask,
		qid_mask->dec_mask, qid_mask->flowid_hmask,
		qid_mask->flowid_lmask);
	dev_dbg(cqm_ctrl->dev, "%u %u\n", qid_set->sub_if_id,
		qid_mask->sub_if_id_mask);

	state = STATE_CLASS;
	sub_if_id_masking(((qid_set->sub_if_id >> SUB_IF_ID_MODE_2_POS) &
			  SUB_IF_ID_MODE_2_MASK), &start, &limit,
			  sub_if_id_bit_mask, &masking_bits);
	qidt_elm.sub_if_id = start;
	qidt_idx.sub_if_id = start;
	while (state != STATE_NONE) {
		switch (state) {
		case STATE_CLASS:
			handle_state(&state, STATE_SUBIF_ID, STATE_NONE, 16,
				     &flag_done.cls_done, qid_set->clsid,
				     qid_mask->classid_mask, &qidt_elm.clsid,
				     &qidt_idx.clsid);
			break;
		case STATE_SUBIF_ID:
			handle_state(&state, STATE_EP, STATE_CLASS,
				     limit, &flag_done.sub_if_id_done,
				     ((qid_set->sub_if_id >> 8) & 0xf),
				     qid_mask->sub_if_id_mask & 1,
				     &qidt_elm.sub_if_id,
				     &qidt_idx.sub_if_id);
			sel_field = (qidt_elm.sub_if_id << 4) |
				(qidt_elm.clsid & 0xf);

			find_next_sub_if_id(&qidt_idx.sub_if_id, masking_bits,
					    (qid_mask->sub_if_id_mask & 1),
					    limit, start);

			break;
		case STATE_EP:
			handle_state(&state, STATE_MPE1, STATE_SUBIF_ID, 16,
				     &flag_done.ep_done, qid_set->ep,
				     qid_mask->ep_mask, &qidt_elm.ep,
				     &qidt_idx.ep);
			break;
		case STATE_MPE1:
			handle_state(&state, STATE_MPE2, STATE_EP, 2,
				     &flag_done.mpe1_done, qid_set->mpe1,
				     qid_mask->mpe1_mask, &qidt_elm.mpe1,
				     &qidt_idx.mpe1);
			break;
		case STATE_MPE2:
			handle_state(&state, STATE_MPE2, STATE_MPE1, 2,
				     &flag_done.mpe2_done, qid_set->mpe2,
				     qid_mask->mpe2_mask, &qidt_elm.mpe2,
				     &qidt_idx.mpe2);
			if (!flag_done.mpe2_done)
				cqm_qid_reg_set(&qidt_elm, qid_val, sel_field);
			break;
		case STATE_NONE:
			break;
		};
	}
}

static void cqm_qidt_set_mode3(const struct cqm_qidt_elm *qid_set,
			       const struct cqm_qidt_mask *qid_mask,
			       u8 qid_val)
{
	struct cqm_qidt_elm qidt_idx = {0};
	struct cqm_qidt_elm qidt_elm = {0};
	int state;
	struct qidt_flag_done flag_done = {0};
	u8 sel_field = 0;
	u32 limit = MODE3_SUB_IF_ID_LIMIT, masking_bits = 0, start = 0;
	u32 sub_if_id_bit_mask =
		((qid_mask->sub_if_id_mask >> SUB_IF_ID_MODE_3_POS) &
		(SUB_IF_ID_MODE_3_MASK << SUB_IF_ID_MASK_SHF)) |
		(qid_mask->sub_if_id_mask & 0x01);

	dev_dbg(cqm_ctrl->dev, "%u\n", qid_val);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_set->clsid,
		qid_set->ep, qid_set->mpe1, qid_set->mpe2);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_set->enc,
		qid_set->dec, qid_set->flowidh, qid_set->flowidl);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_mask->classid_mask,
		qid_mask->ep_mask, qid_mask->mpe1_mask, qid_mask->mpe2_mask);
	dev_dbg(cqm_ctrl->dev, "%u %u %u %u\n", qid_mask->enc_mask,
		qid_mask->dec_mask, qid_mask->flowid_hmask,
		qid_mask->flowid_lmask);
	state = STATE_CLASS;
	sub_if_id_masking(((qid_set->sub_if_id >> SUB_IF_ID_MODE_3_POS) &
			  SUB_IF_ID_MODE_3_MASK), &start, &limit,
			  sub_if_id_bit_mask, &masking_bits);
	qidt_elm.sub_if_id = start;
	qidt_idx.sub_if_id = start;
	while (state != STATE_NONE) {
		switch (state) {
		case STATE_CLASS:
			handle_state(&state, STATE_SUBIF_ID, STATE_NONE, 8,
				     &flag_done.cls_done, (qid_set->clsid & 0x7)
				     , qid_mask->classid_mask, &qidt_elm.clsid,
				     &qidt_idx.clsid);
			break;
		case STATE_SUBIF_ID:
			handle_state(&state, STATE_EP, STATE_CLASS, limit,
				     &flag_done.sub_if_id_done,
				     (qid_set->sub_if_id & 0x1f),
				      qid_mask->sub_if_id_mask,
				      &qidt_elm.sub_if_id,
				      &qidt_idx.sub_if_id);
			sel_field = (qidt_elm.sub_if_id << 3) |
				     (qidt_elm.clsid & 0x7);
			find_next_sub_if_id(&qidt_idx.sub_if_id, masking_bits,
					    (qid_mask->sub_if_id_mask & 1),
					    limit, start);
			break;
		case STATE_EP:
			handle_state(&state, STATE_MPE1, STATE_SUBIF_ID, 16,
				     &flag_done.ep_done, qid_set->ep,
				     qid_mask->ep_mask, &qidt_elm.ep,
				     &qidt_idx.ep);
			break;
		case STATE_MPE1:
			handle_state(&state, STATE_MPE2, STATE_EP, 2,
				     &flag_done.mpe1_done, qid_set->mpe1,
				     qid_mask->mpe1_mask, &qidt_elm.mpe1,
				     &qidt_idx.mpe1);
			break;
		case STATE_MPE2:
			handle_state(&state, STATE_MPE2, STATE_MPE1, 2,
				     &flag_done.mpe2_done, qid_set->mpe2,
				     qid_mask->mpe2_mask, &qidt_elm.mpe2,
				     &qidt_idx.mpe2);
			if (!flag_done.mpe2_done)
				cqm_qid_reg_set(&qidt_elm, qid_val, sel_field);
			break;
		case STATE_NONE:
			break;
		};
	}
}

static void cqm_qidt_set(const struct cqm_qidt_elm *qid_set,
			 const struct cqm_qidt_mask *qid_mask,
			 u8 qid_val)
{
	u32 mode;
	u32 offset = (qid_set->mpe1 & 0x1) | ((qid_set->mpe2 & 0x1) << 1);
	void *c_base = cqm_ctrl->cqm;

	mode = cbm_r32(c_base + CBM_QID_MODE_SEL_REG_0 + (offset << 2));
	mode >>= (qid_set->ep << 1);
	mode = mode & 0x3;
	switch (mode) {
	case 0:
		dev_dbg(cqm_ctrl->dev, "mode0\n");
		cqm_qidt_set_mode0(qid_set, qid_mask, qid_val);
		break;
	case 1:
		dev_dbg(cqm_ctrl->dev, "mode1\n");
		cqm_qidt_set_mode1(qid_set, qid_mask, qid_val);
		break;
	case 2:
		dev_dbg(cqm_ctrl->dev, "mode2\n");
		cqm_qidt_set_mode2(qid_set, qid_mask, qid_val);
		break;
	case 3:
		dev_dbg(cqm_ctrl->dev, "mode3\n");
		cqm_qidt_set_mode3(qid_set, qid_mask, qid_val);
		break;
	default:
		dev_err(cqm_ctrl->dev, "unknown mode\n");
		break;
	}
}

static s32 queue_map_get(int cbm_inst, s32 queue_id, s32 *num_entries,
			 cbm_queue_map_entry_t **entries, u32 flags)
{
	int i = 0, j = 0, k = 0;
	u32 offset, index, offset_1, mode, sel_field;
	unsigned long sys_flag;
	cbm_queue_map_entry_t *temp_entry;
	u8 *ptr;
	void *c_base = cqm_ctrl->cqm;

	spin_lock_irqsave(&cqm_qidt_lock, sys_flag);
	if ((queue_id >= 0) && (queue_id <= 255)) {
		for (i = 0, k = i; i < 0x1000; i += 1, k += 4) {
			/*unrolling the loop*/
			ptr = (u8 *)&g_cbm_qidt_mirror[i].qidt_shadow;
			if (ptr[3] == queue_id) {
				g_qidt_help[j] = k + 3;
				j++;
			}
			if (ptr[2] == queue_id) {
				g_qidt_help[j] = k + 2;
				j++;
			}
			if (ptr[1] == queue_id) {
				g_qidt_help[j] = k + 1;
				j++;
			}
			if (ptr[0] == queue_id) {
				g_qidt_help[j] = k;
				j++;
			}
		}
	} else {
		spin_unlock_irqrestore(&cqm_qidt_lock, sys_flag);
		return CBM_FAILURE;
	}
	temp_entry = kzalloc(sizeof(cbm_queue_map_entry_t) * (j),
			     GFP_ATOMIC);
	if (!temp_entry) {
		spin_unlock_irqrestore(&cqm_qidt_lock, sys_flag);
		return CBM_FAILURE;
	}
	*entries = temp_entry;
	*num_entries = j;
	for (i = 0; i < j; i++) {
		index = (g_qidt_help[i] / 4) << 2;
		offset = (g_qidt_help[i] % 4);
		index = index + 3 - offset;
		temp_entry[i].mpe1 = (index & 0x100) >> PRX300_MPE1_POS;
		temp_entry[i].mpe2 = (index & 0x200) >> PRX300_MPE2_POS;
		temp_entry[i].ep = (index & 0xF0) >> PRX300_EP_POS;
		offset_1 = (temp_entry[i].mpe1 & 0x1) |
		((temp_entry[i].mpe2 & 0x1) << 1);
		mode = cbm_r32(c_base + CBM_QID_MODE_SEL_REG_0 +
			       (offset_1 << 2));
		mode >>= (temp_entry[i].ep << 1);
		mode = mode & 0x3;
		sel_field = (index & 0xf) |
			     (((index & 0x3c00) >> 6) & 0xf0);
		switch (mode) {
		case 0:
			dev_dbg(cqm_ctrl->dev, "mode0\n");
			temp_entry[i].flowid = ((index & 0x2000) |
						(index & 0x1000)) >> 12;
			temp_entry[i].dec = (index & 0x800) >> 11;
			temp_entry[i].enc = (index & 0x400) >> 10;
			temp_entry[i].tc = (index & 0x0F);
			break;
		case 1:
			dev_dbg(cqm_ctrl->dev, "mode1\n");
			temp_entry[i].sub_if_id = sel_field;
			break;
		case 2:
			dev_dbg(cqm_ctrl->dev, "mode2\n");
			temp_entry[i].tc = (sel_field & 0x0F);
			temp_entry[i].sub_if_id = ((sel_field & 0xf0) << 4);
			break;
		case 3:
			dev_dbg(cqm_ctrl->dev, "mode3\n");
			temp_entry[i].tc = (sel_field & 0x07);
			temp_entry[i].sub_if_id = ((sel_field & 0xf8) >> 3);
			break;
		default:
			dev_err(cqm_ctrl->dev, "unknown mode\n");
			break;
		}
	}
	spin_unlock_irqrestore(&cqm_qidt_lock, sys_flag);
	return CBM_SUCCESS;
}

static s32 queue_map_set(int cbm_inst, s32 queue_id,
			 cbm_queue_map_entry_t *entry, u32 flags)
{
	unsigned long sys_flag;
	struct cqm_qidt_elm qidt_elm;
	struct cqm_qidt_mask qidt_mask = {0};
	u32 i;

	if (!entry)
		return CBM_FAILURE;
	qidt_elm.clsid = entry->tc;
	qidt_elm.ep = entry->ep;
	qidt_elm.mpe1 = entry->mpe1;
	qidt_elm.mpe2 = entry->mpe2;
	qidt_elm.enc = entry->enc;
	qidt_elm.dec = entry->dec;
	qidt_elm.flowidl = entry->flowid & 0x1;
	qidt_elm.flowidh = entry->flowid >> 1;
	qidt_elm.sub_if_id = entry->sub_if_id;

	if (flags & CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE)
		qidt_mask.flowid_lmask = 1;
	if (flags & CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE)
		qidt_mask.flowid_hmask = 1;
	if (flags & CBM_QUEUE_MAP_F_EN_DONTCARE)
		qidt_mask.enc_mask = 1;
	if (flags & CBM_QUEUE_MAP_F_DE_DONTCARE)
		qidt_mask.dec_mask = 1;
	if (flags & CBM_QUEUE_MAP_F_MPE1_DONTCARE)
		qidt_mask.mpe1_mask = 1;
	if (flags & CBM_QUEUE_MAP_F_MPE2_DONTCARE)
		qidt_mask.mpe2_mask = 1;
	if (flags & CBM_QUEUE_MAP_F_TC_DONTCARE)
		qidt_mask.classid_mask = 1;
	if (flags & CBM_QUEUE_MAP_F_SUBIF_DONTCARE) {
		qidt_mask.sub_if_id_mask = 1;
		qidt_mask.sub_if_id_mask |= (entry->sub_if_id_mask_bits
			<< SUB_IF_ID_MASK_SHF);
	}

	spin_lock_irqsave(&cqm_qidt_lock, sys_flag);
	if (flags & CBM_QUEUE_MAP_F_EP_DONTCARE) {
		for (i = 0; i < MAX_QID_MAP_EP_NUM; i++) {
			qidt_elm.ep = i;
			cqm_qidt_set(&qidt_elm, &qidt_mask, queue_id);
		}
	} else {
		cqm_qidt_set(&qidt_elm, &qidt_mask, queue_id);
	}
	spin_unlock_irqrestore(&cqm_qidt_lock, sys_flag);
	return CBM_SUCCESS;
}

void queue_map_buf_free(cbm_queue_map_entry_t *entries)
{
	kfree(entries);
}

void *cqm_get_enq_base(void)
{
	return cqm_ctrl->enq;
}

void *cqm_get_deq_base(void)
{
	return cqm_ctrl->deq;
}

void *cqm_get_dma_desc_base(void)
{
	return cqm_ctrl->dmadesc;
}

void *cqm_get_ctrl_base(void)
{
	return cqm_ctrl->cqm;
}

void *cqm_get_ls_base(void)
{
	return cqm_ctrl->ls;
}

void *cqm_get_pib_base(void)
{
	return cqm_ctrl->pib;
}

void cqm_read_dma_desc_prep(int port, void **base_addr, int *num_desc)
{
	int chan;

	/*Switch off the corresponding DMA TX channel (DMA register)*/
	chan = dqm_port_info[port].dma_ch;
	/* Disable DMA channels associated with CBM DQ Port*/
	if (chan && dqm_port_info[port].dma_ch_in_use)
		ltq_dma_chan_off(chan);
	usleep_range(20, 25);
	/*Disable read pointer auto-increment mode
	 *for that DMA channel(CQEM register)
	 */
	if (port < DQM_PON_START_ID) {
		set_val((cqm_ctrl->deq + DQ_DMA_PORT((port), cfg)), 1, 0x2, 1);
		*base_addr = cqm_ctrl->dmadesc + CQM_DEQ_DMA_DESC(port, 0);
	} else {
		set_val(cqm_ctrl->pib + PIB_CTRL, 1, 0x4, 2);
		*base_addr = cqm_ctrl->dmadesc +
			      CQM_DEQ_PON_DMA_DESC(0, 0);
	}
	pr_info("base addr 0x%p\n", *base_addr);
	*num_desc =  dqm_port_info[port].deq_info.num_desc;
}

void cqm_read_dma_desc_end(int port)
{
	int chan;

	/*Disable read pointer auto-increment mode
	 *for that DMA channel(CQEM register)
	 */
	if (port < DQM_PON_START_ID)
		set_val((cqm_ctrl->deq + DQ_DMA_PORT((port), cfg)), 0, 0x2, 1);
	else
		set_val(cqm_ctrl->pib + PIB_CTRL, 0, 0x4, 2);
	chan = dqm_port_info[port].dma_ch;
	/* Disable DMA channels associated with CBM DQ Port*/
	if (chan && dqm_port_info[port].dma_ch_in_use)
		ltq_dma_chan_on(chan);
}

static inline void add_metadata(u8 *v_buf_addr, u32 pool,
				u32 policy)
{
	(v_buf_addr)[0] = 0xde;
	(v_buf_addr)[1] = pool;
	(v_buf_addr)[2] = policy;
	(v_buf_addr)[3] = 0xad;
}

static int get_metadata(u8 *v_buf_addr, int *pool, int *policy)
{
	if (((v_buf_addr)[0] == 0xde) &&
	    ((v_buf_addr)[3] == 0xad)) {
		*pool = (v_buf_addr)[1];
		*policy = (v_buf_addr)[2];
		return CBM_SUCCESS;
	}
	dev_err(cqm_ctrl->dev, "Invalid METADATA\n");
	return CBM_FAILURE;
}

/* flags : STD Buffer or Jumbo buffer using existing defines
 * Caller must have checked the parameter validity etc before calling this API
 */
static int cpu_pool_enqueue(u32 pid, struct cbm_desc *desc, void *data_pointer,
			    int flags)
{
	int i = 0;
	unsigned long sys_flag;
	u32 reg;
	void *c_base = cqm_ctrl->cqm;

	dev_dbg(cqm_ctrl->dev, "%s\n", __func__);
	spin_lock_irqsave(&cpu_pool_enq, sys_flag);
	do {
		reg = cbm_r32(c_base + CBM_CPU_POOL_ENQ_CNT);
		i++;
		if (i == 10) {
			spin_unlock_irqrestore(&cpu_pool_enq, sys_flag);
			dev_err(cqm_ctrl->dev, "wait for time i %d\n", i);
			dev_err(cqm_ctrl->dev, "Error Enq CPU pool buff pkt reg 0x%x\n",
				reg);
		return CBM_FAILURE;
	}
	} while (((reg & CBM_CPU_POOL_ENQ_CNT_STS_MASK) >>
		 CBM_CPU_POOL_ENQ_CNT_STS_POS) == 0);
	cbm_w32(eqmdesc[pid], desc->desc0);
	cbm_w32(eqmdesc[pid] + 4, desc->desc1);
	cbm_w32(eqmdesc[pid] + 8, (u32)data_pointer);
	dev_dbg(cqm_ctrl->dev, "dw2 0x%p\n", data_pointer);
	/*write barrier*/
	wmb();
	cbm_w32(eqmdesc[pid] + 12, desc->desc3);
	/*write barrier*/
	wmb();
	spin_unlock_irqrestore(&cpu_pool_enq, sys_flag);
	dev_dbg(cqm_ctrl->dev, "exiting cpu enq\n");
	return CBM_SUCCESS;
}

static int cpu_enqueue_hw(u32 pid, struct cbm_desc *desc, void *data_pointer,
			  int flags)
{
	unsigned long sys_flag;
	u32 pool, policy;

	pool = (desc->desc3 & PTR_RTN_CPU_DW3_EGP_0_POOL_MASK) >>
		PTR_RTN_CPU_DW3_EGP_0_POOL_POS;
	policy = (desc->desc3 & PTR_RTN_CPU_DW3_EGP_0_POLICY_MASK) >>
		  PTR_RTN_CPU_DW3_EGP_0_POLICY_POS;
	if (IS_CPU_POOL(pool, policy))
		return cpu_pool_enqueue(pid, desc, data_pointer, flags);
	local_irq_save(sys_flag);
	cbm_w32(eqmdesc[pid], desc->desc0);
	cbm_w32(eqmdesc[pid] + 4, desc->desc1);
	cbm_w32(eqmdesc[pid] + 8, (u32)data_pointer);
	/*make sure desc0, desc1, desc2 are written*/
	wmb();
	cbm_w32(eqmdesc[pid] + 12, desc->desc3);
	/*make sure desc3 own bit is written*/
	wmb();
	local_irq_restore(sys_flag);
	return CBM_SUCCESS;
}

static int cqm_cpu_enqueue(u32 pid, struct cbm_desc *desc)
{
	u32 data_pointer, size_to_wb, pointer_to_wb;
	int idx;

	if (!desc || pid >= CPU_EQM_PORT_NUM)
		return CBM_FAILURE;
	dev_dbg(cqm_ctrl->dev, "%s %d %d\n", __func__,
		(cqm_desc_data_pool(desc->desc3)),
		(cqm_desc_data_policy(desc->desc3)));

	if (IS_CPU_POOL((cqm_desc_data_pool(desc->desc3)),
			(cqm_desc_data_policy(desc->desc3)))) {
		pointer_to_wb = desc->desc2 + cqm_desc_data_off((desc->desc3));
		size_to_wb  = cqm_desc_data_len(desc->desc3);
	} else {
		if (get_buffer_base_index(desc->desc2, &pointer_to_wb,
					  &size_to_wb, &idx)) {
			dev_err(cqm_ctrl->dev, "Error locating the BUFF Pool\n");
			return CBM_FAILURE;
		}
	}

	data_pointer = dma_map_single(cqm_ctrl->dev, (void *)pointer_to_wb,
				      size_to_wb, DMA_TO_DEVICE);
	if (dma_mapping_error(cqm_ctrl->dev, data_pointer)) {
		dev_err(cqm_ctrl->dev, "%s DMA map failed\n", __func__);
		return CBM_FAILURE;
	}
	data_pointer += desc->desc2 - pointer_to_wb;
	return cpu_enqueue_hw(pid, desc, (void *)data_pointer, 0);
}

/*return 1: pointer is valid
 *0: pointer is invalid
 */
static inline int check_ptr_validation_prx300(void *buf)
{
	int i = 0;
	u32 temp_buf_start, temp_buf_end;
	u32 v_buf = (u32)buf;

	while (i < CQM_PRX300_NUM_POOLS) {
		temp_buf_start = bm_pool_conf[i].pool_start_low;
		temp_buf_end = bm_pool_conf[i].pool_end_low;
		if ((v_buf >= temp_buf_start) && (v_buf < temp_buf_end))
			return 1;
		i++;
	}
	return 0;
}

#ifdef MODULE_TEST
void *cqm_fsqm_buff_alloc(u32 pid, u32 policy)
#else
static void *cqm_fsqm_buff_alloc(u32 pid, u32 policy)
#endif
{
	u32 buf_addr = 0;
	u32 i = 0, j = 0;
	unsigned long sys_flag;
	u32 segment_mask = 0;
	u32 pool_t;
	u32 policy_t;

	if (pid >= CPU_EQM_PORT_NUM) {
		dev_err(cqm_ctrl->dev, "illegal pid: %d\n", pid);
		return NULL;
	}
	while (j < CQM_PRX300_NUM_POOLS) {
		if (policy == bm_pool_conf[j].policy) {
			segment_mask = bm_pool_conf[j].segment_mask;
			break;
		}
		j++;
	}
	if (j >= CQM_PRX300_NUM_POOLS) {
		dev_err(cqm_ctrl->dev, "No matching policy is available\n");
		return NULL;
	}
	dev_dbg(cqm_ctrl->dev, "0x%x\n", (int)bufreq[j] + (pid * 0x1000));
	dev_dbg(cqm_ctrl->dev, "segment mask 0x%x\n", segment_mask);
	local_irq_save(sys_flag);
	do {
		buf_addr = cbm_r32(bufreq[j] + (pid * 0x1000));
		dev_dbg(cqm_ctrl->dev, "alloc 0x%x i %d\n", buf_addr, i);
	} while (((buf_addr & segment_mask) == segment_mask) &&
		 ((i++) < DEFAULT_WAIT_CYCLES));
	dev_dbg(cqm_ctrl->dev, "alloc 0x%x i %d\n", (int)buf_addr, i);
	if ((buf_addr & segment_mask) == segment_mask) {
		dev_err(cqm_ctrl->dev, "alloc fsqm buff fail for portid: %d",
			pid);
		local_irq_restore(sys_flag);
		return NULL;
	}
	local_irq_restore(sys_flag);
	pool_t = bm_pool_conf[j].pool;
	policy_t = bm_pool_conf[j].policy;
	UP_STATS(cqm_dbg_cntrs[policy_t][pool_t].alloc_cnt);
	return (void *)buf_addr;
}

static void *cqm_buff_alloc_by_policy_prx300(u32 pid, u32 flag, u32 policy)
{
	u32 buf_addr = 0;
	u32 i = 0, j = 0;
	unsigned long sys_flag;
	u32 segment_mask = 0;
	u8 *v_buf;
	u32 pool_t;
	u32 policy_t;
	u32 buff_base, buff_size, index;

	if (pid >= CPU_EQM_PORT_NUM) {
		dev_err(cqm_ctrl->dev, "illegal pid: %d\n", pid);
		return NULL;
	}
	while (j < CQM_PRX300_NUM_POOLS) {
		if (policy == bm_pool_conf[j].policy) {
			segment_mask = bm_pool_conf[j].segment_mask;
			break;
		}
		j++;
	}
	if (j >= CQM_PRX300_NUM_POOLS) {
		dev_err(cqm_ctrl->dev, "No matching policy is available\n");
		return NULL;
	}
	dev_dbg(cqm_ctrl->dev, "pool %d\n", j);
	dev_dbg(cqm_ctrl->dev, "0x%x\n", (int)bufreq[j] + (pid * 0x1000));
	dev_dbg(cqm_ctrl->dev, "segment mask 0x%x\n", segment_mask);
	local_irq_save(sys_flag);
	do {
		buf_addr = cbm_r32(bufreq[j] + (pid * 0x1000));
		dev_dbg(cqm_ctrl->dev, "alloc 0x%x i %d\n", buf_addr, i);
	} while (((buf_addr & segment_mask) == segment_mask) &&
		 ((i++) < DEFAULT_WAIT_CYCLES));
	dev_dbg(cqm_ctrl->dev, "alloc 0x%x i %d\n", (int)buf_addr, i);
	if ((buf_addr & segment_mask) == segment_mask) {
		dev_err(cqm_ctrl->dev, "buff alloc fail portid: %d type %d",
			pid, flag);
		local_irq_restore(sys_flag);
		return NULL;
	}
	local_irq_restore(sys_flag);
	v_buf = __va(buf_addr);
	get_buffer_base_index((u32)v_buf, &buff_base, &buff_size, &index);
	pool_t = bm_pool_conf[j].pool;
	policy_t = bm_pool_conf[j].policy;
	add_metadata((u8 *)buff_base, pool_t, policy_t);
	UP_STATS(cqm_dbg_cntrs[policy_t][pool_t].alloc_cnt);
	return (void *)buff_base + CQM_POOL_METADATA;
}

static int cqm_buffer_free_by_policy_prx300(struct cqm_bm_free *free_info)
{
	u32 buf, size, pool, policy, pid = smp_processor_id();
	unsigned long sys_flag;
	int ret, idx;

	ret = get_buffer_base_index((u32)__va(free_info->buf),
				    &buf, &size, &idx);
	if (!ret) {
		pool = bm_pool_conf[idx].pool;
		policy = free_info->policy_base + pool;

		if ((pool != CQM_FSQM_POOL) &&
		    (policy != CQM_FSQM_POLICY)) {
			memset((u8 *)buf, 0, CQM_PRX300_POOL_POL_HDR_SIZE);
			buf = dma_map_single(cqm_ctrl->dev, (void *)buf,
					     size, DMA_FROM_DEVICE);
			if (dma_mapping_error(cqm_ctrl->dev, buf)) {
				dev_err(cqm_ctrl->dev, "%s DMA map failed\n",
					__func__);
				return CBM_FAILURE;
			}
		}
		UP_STATS(cqm_dbg_cntrs[policy][pool].free_cnt);
		local_irq_save(sys_flag);
		cbm_w32(cqm_ctrl->deq + DQ_CPU_PORT(pid, ptr_rtn_dw2), buf);
		cbm_w32(cqm_ctrl->deq + DQ_CPU_PORT(pid, ptr_rtn_dw3),
			(pool << PTR_RTN_CPU_DW3_EGP_0_POOL_POS)
			| (policy << PTR_RTN_CPU_DW3_EGP_0_POLICY_POS));
		local_irq_restore(sys_flag);
	}

	return ret;
}

static void *cqm_buffer_alloc(u32 pid, u32 flag, u32 size, u32 *buf_size)
{
	u32 buf_addr = 0;
	u8 *v_buf_addr;
	u32 i = 0, j = 0;
	unsigned long sys_flag;
	u32 segment_mask = 0;
	u32 pool_t;
	u32 policy_t;
	u32 buff_base, buff_size, index;

	if (pid >= CPU_EQM_PORT_NUM) {
		dev_err(cqm_ctrl->dev, "illegal pid: %d\n", pid);
		return NULL;
	}
	if (size > bm_pool_conf[cqm_ctrl->num_pools - 1].buf_frm_size) {
		dev_err(cqm_ctrl->dev, "Invalid size requested: %d\n", size);
		return NULL;
	}
	while (j < cqm_ctrl->num_pools) {
		if (size <= bm_pool_conf[j].buf_frm_size) {
			*buf_size = bm_pool_conf[j].buf_frm_size;
			segment_mask = bm_pool_conf[j].segment_mask;
			break;
		}
		j++;
	}
	if (j >= cqm_ctrl->num_pools) {
		dev_err(cqm_ctrl->dev,
			"Req Buff size exceeds the max available\n");
		return NULL;
	}
	dev_dbg(cqm_ctrl->dev, "pool %d\n", j);
	dev_dbg(cqm_ctrl->dev, "0x%x\n", (int)bufreq[j] + (pid * 0x1000));
	dev_dbg(cqm_ctrl->dev, "segment mask 0x%x\n", segment_mask);
	local_irq_save(sys_flag);
	do {
		buf_addr = cbm_r32(bufreq[j] + (pid * 0x1000));
		dev_dbg(cqm_ctrl->dev, "alloc 0x%x i %d\n", buf_addr, i);
	} while (((buf_addr & segment_mask) == segment_mask) &&
		 ((i++) < DEFAULT_WAIT_CYCLES));
	dev_dbg(cqm_ctrl->dev, "alloc 0x%x i %d\n", (int)buf_addr, i);
	if ((buf_addr & segment_mask) == segment_mask) {
		dev_err(cqm_ctrl->dev,
			"alloc buffer fail for portid: %d type %d", pid, flag);
		local_irq_restore(sys_flag);
		return NULL;
	}
	pool_t = bm_pool_conf[j].pool;
	policy_t = bm_pool_conf[j].policy;

	UP_STATS(cqm_dbg_cntrs[policy_t][pool_t].alloc_cnt);
	local_irq_restore(sys_flag);
	v_buf_addr = (uint8_t *)__va(buf_addr);
	get_buffer_base_index((u32)v_buf_addr, &buff_base, &buff_size, &index);
	add_metadata((u8 *)buff_base, pool_t, policy_t);
	return (void *)buff_base + CQM_POOL_METADATA;
}

static void *cqm_buffer_alloc_by_size(u32 pid, u32 flag, u32 size)
{
	u32 buf_size;

	dev_dbg(cqm_ctrl->dev, "%s\n", __func__);
	return cqm_buffer_alloc(pid, flag, size, &buf_size);
}

static int cqm_buffer_free(u32 pid, void *v_buf_free, u32 flag)
{
	u32 buf, buf_addr;
	unsigned long sys_flag;
	u32 size_to_wb, pool, policy;
	int t_pool = 0, t_policy = 0;
	u32 pointer_to_wb;
	int idx = 0, res, i;
	u32 v_buf = (u32)v_buf_free;
	void *deq = cqm_ctrl->deq;

	if (pid >= CPU_DQM_PORT_NUM) {
		cbm_err("illegal pid: %d", pid);
		return CBM_FAILURE;
	}
	if (flag)
		v_buf = (u32)__va(v_buf);
	dev_dbg(cqm_ctrl->dev, "%s %x\n", __func__, v_buf);
	res = get_buffer_base_index(v_buf, &pointer_to_wb, &size_to_wb, &idx);
	if (!res) {
		pool = bm_pool_conf[idx].pool;
		policy = bm_pool_conf[idx].policy;
		dev_dbg(cqm_ctrl->dev, "pool %d policy %d\n", pool, policy);
		if ((bm_pool_conf[idx].pool == CQM_FSQM_POOL) &&
		    (bm_pool_conf[idx].policy == CQM_FSQM_POLICY)) {
			buf = pointer_to_wb;
			dev_info(cqm_ctrl->dev, "fsqm buff free 0x%x\n", buf);
			goto BUFF_FREE;
		}

		if (!get_metadata((u8 *)pointer_to_wb, &t_pool, &t_policy)) {
			pool = t_pool;
			policy = t_policy;
			dev_dbg(cqm_ctrl->dev, "valid metadata found\n");
			dev_dbg(cqm_ctrl->dev, "pool %d policy %d\n", pool,
				policy);
		} else {
			dev_err(cqm_ctrl->dev, "getting metadata failed\n");
		}

		/*clear metadata*/
		memset((u8 *)pointer_to_wb, 0, CQM_PRX300_POOL_POL_HDR_SIZE);
		dev_dbg(cqm_ctrl->dev, "Erased 0x%x 0x%x\n", pointer_to_wb,
			size_to_wb);
		buf = dma_map_single(cqm_ctrl->dev, (void *)pointer_to_wb,
				     size_to_wb, DMA_FROM_DEVICE);
		if (dma_mapping_error(cqm_ctrl->dev, buf)) {
			dev_err(cqm_ctrl->dev, "%s DMA map failed\n",
				__func__);
			return CBM_FAILURE;
		}
	} else {
		return CBM_FAILURE;
	}
BUFF_FREE:
	local_irq_save(sys_flag);
	/*Mahua's fix for buffer free issue.To be tuned for real silicon*/
	i = 0;
	do {
		buf_addr = cbm_r32(deq + DQ_CPU_PORT(pid, ptr_rtn_dw2));
		i++;
		if ((i % 1000) == 0) {
			dev_err(cqm_ctrl->dev, "wait for time i %d, buf_addr is 0x%x\n",
				i, buf_addr);
		}
		if (i == 10000)
			break;
	} while (((buf_addr & 0xFFFFFF80) != 0xFFFFFF80));

	if ((buf_addr & 0xFFFFFF80) != 0xFFFFFF80) {
		local_irq_restore(sys_flag);
		dev_err(cqm_ctrl->dev, "can not free for portid: %d", pid);
		return CBM_FAILURE;
	}
	UP_STATS(cqm_dbg_cntrs[policy][pool].free_cnt);
	cbm_w32(deq + DQ_CPU_PORT(pid, ptr_rtn_dw2), buf);
	cbm_w32(deq + DQ_CPU_PORT(pid, ptr_rtn_dw3),
		(pool << PTR_RTN_CPU_DW3_EGP_0_POOL_POS)
		| (policy << PTR_RTN_CPU_DW3_EGP_0_POLICY_POS));
	local_irq_restore(sys_flag);
	return CBM_SUCCESS;
}

static int setup_desc(struct cbm_desc *desc, u32 data_ptr, u32 data_len,
		      u32 DW0, u32 DW1, u32 DW3, int pool, int policy)
{
	struct dma_desc temp_desc;
	struct cbm_desc *ptr;

	if (!desc)
		return CBM_FAILURE;
	memset(&temp_desc, 0, sizeof(struct dma_desc));
	temp_desc.data_ptr		= data_ptr & (~0x7);
	temp_desc.data_len		= data_len;
	temp_desc.byte_offset	= data_ptr & 0x7;
	temp_desc.sop			= 1;
	temp_desc.eop			= 1;
	temp_desc.own			= 0;
	if (DW3 & BIT(27))  /*keep dic original value set by caller */
		temp_desc.dic = 1;
	ptr = (struct cbm_desc *)&temp_desc;
	ptr->desc0 = DW0;
	ptr->desc1 = DW1;
	/*copy the pdu type bit for DSL*/
	if (DW3 & BIT(CBM_PDU_TYPE))
		ptr->desc2 |= BIT(CBM_PDU_TYPE);
	dev_dbg(cqm_ctrl->dev, "desc20x%x pool %d policy %d\n", ptr->desc2,
		pool, policy);
	ptr->desc2 = (ptr->desc2 & (~PTR_RTN_CPU_DW3_EGP_0_POOL_MASK)) |
		     (pool << PTR_RTN_CPU_DW3_EGP_0_POOL_POS);
	dev_dbg(cqm_ctrl->dev, "desc3 0x%x\n", ptr->desc2);
	ptr->desc2 = (ptr->desc2 & (~PTR_RTN_CPU_DW3_EGP_0_POLICY_MASK)) |
		     (policy << PTR_RTN_CPU_DW3_EGP_0_POLICY_POS);
	dev_dbg(cqm_ctrl->dev, "desc2 0x%x\n", ptr->desc2);
	#ifdef CONFIG_LITTLE_ENDIAN
	desc->desc0 = ptr->desc0;
	desc->desc1 = ptr->desc1;
	desc->desc2 = ptr->desc2;
	desc->desc3 = ptr->desc3;
	#else
	desc->desc0 = ptr->desc1;
	desc->desc1 = ptr->desc0;
	desc->desc2 = ptr->desc3;
	desc->desc3 = ptr->desc2;
	#endif
	dev_dbg(cqm_ctrl->dev, "0x%x 0x%x 0x%x 0x%x\n", desc->desc0,
		desc->desc1, desc->desc2, desc->desc3);
	return CBM_SUCCESS;
}

static s32 cqm_cpu_pkt_tx(struct sk_buff *skb, struct cbm_tx_data *data,
			  u32 flags)
{
	struct cbm_desc desc = {0};
	u32 tmp_data_ptr;
	struct dma_tx_desc_2 *desc_2 = (struct dma_tx_desc_2 *)&skb->DW2;
	u32 new_buf = 0;
	s32 clone_f, shared_f, no_hdr_room_f = 0;
	int pool = 0, policy = 0, tot_len, buf_size, cpu_buf;

	dev_dbg(cqm_ctrl->dev, "%s\n", __func__);
	tot_len = skb->len;
	clone_f = skb_cloned(skb);
	shared_f = skb_shared(skb);
	cpu_buf = !check_ptr_validation_prx300((void *)(skb->head));

	if (data && data->pmac) {
		no_hdr_room_f = skb_headroom(skb) < data->pmac_len ? 1 : 0;
		tot_len = skb->len + data->pmac_len;
	}
	if (cpu_buf || clone_f || no_hdr_room_f || shared_f) {
		dev_dbg(cqm_ctrl->dev, "linearising\n");
		new_buf = (u32)cqm_buffer_alloc(smp_processor_id(), 0,
						tot_len, &buf_size);
		if (!new_buf) {
			dev_err(cqm_ctrl->dev,
				"%s: cbm buffer alloc failed ..\n", __func__);
			goto ERR_CASE_2;
		} else {
			new_buf -= CQM_POOL_METADATA;
			if (get_metadata((u8 *)new_buf, &pool, &policy)) {
				dev_err(cqm_ctrl->dev, "Error getting pool policy\n");
				goto ERR_CASE_1;
			}
			tmp_data_ptr = new_buf;
			dev_dbg(cqm_ctrl->dev, "tmp_data_ptr 0x%x\n",
				tmp_data_ptr);
			cbm_linearise_buf(skb, data, buf_size, new_buf);
		}
	} else {
		/*Insert PMAC header if present*/
		if (data && data->pmac) {
			skb_push(skb, data->pmac_len);
			memcpy(skb->data, data->pmac, data->pmac_len);
			desc_2->field.data_ptr = (u32)skb->data;
		}
		tmp_data_ptr = desc_2->field.data_ptr;
		dev_dbg(cqm_ctrl->dev, "tmp_data_ptr 0x%x\n", tmp_data_ptr);
		if (get_metadata((u8 *)(skb->head - CQM_POOL_METADATA),
				 &pool, &policy)) {
			dev_err(cqm_ctrl->dev, "Error getting pool policy\n");
			goto ERR_CASE_2;
		}
		/* Detach the skb */
		skb->head = NULL;
	}
	setup_desc((struct cbm_desc *)&desc, tmp_data_ptr, tot_len, skb->DW1,
		   skb->DW0, skb->DW3, pool, policy);
	dev_kfree_skb_any(skb);

	cqm_cpu_enqueue(smp_processor_id(), &desc);
	UP_STATS(cqm_dbg_cntrs[policy][pool].tx_cnt);
		return CBM_SUCCESS;
ERR_CASE_1:
	cqm_buffer_free(smp_processor_id(), (void *)new_buf, 0);
ERR_CASE_2:
	dev_kfree_skb_any(skb);
	return CBM_FAILURE;
}

s32 cqm_cpu_port_get(struct cbm_cpu_port_data *data, u32 flags)
{
	int i;
	u32 type;
	struct cbm_tx_push *ptr;

	if (!data) {
		dev_err(cqm_ctrl->dev, "port data empty\n");
		return CBM_FAILURE;
	}

	data->dq_tx_flush_info.deq_port = DMA_PORT_FOR_FLUSH;
	data->dq_tx_flush_info.tx_ring_size = 1;
	data->dq_tx_flush_info.tx_b_credit = 0;
	data->dq_tx_flush_info.txpush_addr_qos =
		(void *)((u32)(cqm_ctrl->txpush +
		(TXPUSH_CMD_RX_EGP_1 * DMA_PORT_FOR_FLUSH)) &
		0x7FFFFF);
	data->dq_tx_flush_info.txpush_addr = (void *)((u32)(cqm_ctrl->txpush +
				  (TXPUSH_CMD_RX_EGP_1 * DMA_PORT_FOR_FLUSH)));
	data->dq_tx_flush_info.tx_ring_offset = TXPUSH_CMD_RX_EGP_1;
	data->dq_tx_flush_info.tx_pkt_credit =
				dqm_port_info[DMA_PORT_FOR_FLUSH].dq_txpush_num;
	if (!(dqm_port_info[DMA_PORT_FOR_FLUSH].ref_cnt)) {
		cqm_dma_port_enable(DMA_PORT_FOR_FLUSH,
				    CBM_PORT_F_DEQUEUE_PORT, 0);
		dqm_port_info[DMA_PORT_FOR_FLUSH].ref_cnt = 1;
	}

	dev_dbg(cqm_ctrl->dev,
		"%d %d %d 0x%x 0x%x %d\n",
		data->dq_tx_flush_info.deq_port,
		data->dq_tx_flush_info.tx_ring_size,
		data->dq_tx_flush_info.tx_b_credit,
		data->dq_tx_flush_info.txpush_addr_qos,
		data->dq_tx_flush_info.tx_ring_offset,
		data->dq_tx_flush_info.tx_pkt_credit);

	for (i = 0; i < CQM_MAX_CPU; i++) {
		type = dqm_port_info[i].cpu_port_type;
		data->policy_num[i][0] = cqm_ctrl->num_pools;
		data->policy_base[i][0] = 0;
		if (!((dqm_port_info[i].valid) && IS_CPU_PORT_TYPE(type))) {
			data->dq_tx_push_info[i][0].deq_port = -1;
			continue;
		}
		ptr = &data->dq_tx_push_info[i][0];
		ptr->deq_port = i;
		ptr->type = type;
		ptr->tx_ring_size = 1;
		ptr->tx_b_credit = 0;
		ptr->txpush_addr_qos = (void *)((u32)(cqm_ctrl->txpush +
					  (TXPUSH_CMD_RX_EGP_1 * i)) &
					  0x7FFFFF);
		ptr->txpush_addr = (void *)((u32)(cqm_ctrl->txpush +
					  (TXPUSH_CMD_RX_EGP_1 * i)));
		ptr->tx_ring_offset = TXPUSH_CMD_RX_EGP_1;
		ptr->tx_pkt_credit = dqm_port_info[i].dq_txpush_num;
	}

	if (cqm_ctrl->re_insertion > 0)
		fill_dp_alloc_data(&data->re_insertion, 0,
				   DMA_PORT_RE_INSERTION, 0);

	return CBM_SUCCESS;
}

void print_reg(char *name, void *addr)
{
	dev_dbg(cqm_ctrl->dev, "%s addr: 0x%8x, val: 0x%8x\n",
		name, (u32)addr, cbm_r32(addr));
}

static void fill_dp_alloc_data(struct cbm_dp_alloc_data *data, int dp,
			       int port, u32 flags)
{
	struct cqm_dqm_port_info *p_info;
	int dma_controller, chan, idx;
	unsigned char dma_ctrl[DMA_CH_STR_LEN];

	/*Write only valid, else skip*/
	data->flags |= CBM_PORT_DP_SET | CBM_PORT_DQ_SET;
	data->dp_port = dp;
	data->deq_port = port;
	for (idx = 0; idx < MAX_PORT(flags); idx++) {
		p_info = &dqm_port_info[port + idx];
		if (p_info->dma_dt_init_type == DEQ_DMA_CHNL) {
			data->flags |= CBM_PORT_DMA_CHAN_SET |
					CBM_PORT_PKT_CRDT_SET;
			dev_dbg(cqm_ctrl->dev, "%d %d %d\n", (port + idx),
				p_info->dma_dt_ctrl,  p_info->dma_dt_ch);
			if (p_info->dma_dt_ctrl == 3)
				snprintf(dma_ctrl, sizeof(dma_ctrl), "DMA%d"
				, p_info->dma_dt_ctrl);
			else
				snprintf(dma_ctrl, sizeof(dma_ctrl), "DMA%dTX"
				, p_info->dma_dt_ctrl);
			dma_controller = cqm_dma_get_controller(dma_ctrl);
			dev_dbg(cqm_ctrl->dev, "dma %d \r\n", dma_controller);
			chan = _DMA_C(dma_controller, 0, p_info->dma_dt_ch);
			dev_dbg(cqm_ctrl->dev, "chan %d \r\n", chan);
			p_info->dma_ch = chan;
			snprintf(p_info->dma_chan_str, DMA_CH_STR_LEN, "port%d",
				 (port + idx));
			data->dma_chan = _DMA_C(p_info->dma_dt_ctrl, 0,
						p_info->dma_dt_ch);
			data->tx_pkt_credit = p_info->dq_txpush_num;
		} else {
			data->flags |= CBM_PORT_PKT_CRDT_SET;
			data->tx_pkt_credit = p_info->dq_txpush_num;
		}
	}
	data->flags |= CBM_PORT_RING_ADDR_SET |
		       CBM_PORT_RING_SIZE_SET |
		       CBM_PORT_RING_OFFSET_SET;
	data->tx_ring_size = 1;
	data->tx_b_credit = 0;
	data->num_dma_chan = idx;
	data->deq_port_num = (data->deq_port == DQM_PON_TYPE) ? 64 : idx;
	/*Lower 22 bits*/
	data->txpush_addr_qos =
		(void *)((u32)(cqm_ctrl->txpush +
		(TXPUSH_CMD_RX_EGP_1 * port)) & 0x7FFFFF);
	data->txpush_addr = (void *)((u32)(cqm_ctrl->txpush +
		(TXPUSH_CMD_RX_EGP_1 * port)));
	data->tx_ring_offset = TXPUSH_CMD_RX_EGP_1;
}

static void set_ifmux(int mode)
{
	u32 val;

	if (!cqm_ctrl->syscfg) {
		dev_err(cqm_ctrl->dev, "chiptop wanmux not set\n");
		return;
	}

	regmap_read(cqm_ctrl->syscfg, cqm_ctrl->ifmux_reg_offset, &val);
	if (mode == PRX300_WAN_AON_MODE)
		val |= PRX300_WAN_AON_MODE;
	else
		val &= ~PRX300_WAN_AON_MODE;
	regmap_write(cqm_ctrl->syscfg, cqm_ctrl->ifmux_reg_offset, val);
	regmap_read(cqm_ctrl->syscfg, cqm_ctrl->ifmux_reg_offset, &val);
	dev_dbg(cqm_ctrl->dev, "wan mode ifmux_cfg 0x%08x\n", val);
}

static s32 do_port_setting(u32 *pmac, u32 flags, u32 *cbm_port,
			   struct cqm_pmac_port_map *local_entry,
			   int *pmac_found, int *loop)
{
	int i = *loop;
	u32 temp_flags;
	int phys_port = 0;
	struct cqm_pmac_port_map *test_ptr = NULL;
	struct pib_ctrl ctrl = {0};
	int populate = 0;
	void *deq = cqm_ctrl->deq;
	u32 config;

	temp_flags = (flags & DP_F_FAST_WLAN) ? DP_F_FAST_WLAN : flags;
	temp_flags = (flags & DP_F_FAST_DSL) ? DP_F_FAST_DSL : flags;
	if (cbm_port[i] != CBM_PORT_NOT_APPL) {
		dev_dbg(cqm_ctrl->dev, "0x%x 0x%x\n", cbm_port[i], flags);
		populate = 1;
	}
	switch (temp_flags) {
	case DP_F_FAST_ETH_WAN:
		/*set PIB bypass*/
		ctrl.pib_bypass = 1;
		ctrl.pib_en = 1;
		config_pib_ctrl(&ctrl, FLAG_PIB_BYPASS | FLAG_PIB_ENABLE);
		set_ifmux(PRX300_WAN_AON_MODE);
		break;
	case DP_F_GPON:
		pib_program_overshoot(CQEM_PON_IP_IF_PIB_OVERSHOOT_BYTES);
		/*clear PIB bypass*/
		ctrl.pib_bypass = 0;
		ctrl.pib_en = 1;
		ctrl.wakeup_intr_en = 1;
		ctrl.deq_delay = 7;
		config_pib_ctrl(&ctrl,
				FLAG_PIB_BYPASS |
				FLAG_PIB_ENABLE |
				FLAG_PIB_DELAY |
				FLAG_PIB_WAKEUP_INTR);
		if (cqm_ctrl->force_xpcs)
			set_ifmux(PRX300_WAN_AON_MODE);
		else
			set_ifmux(PRX300_WAN_PON_MODE);
		break;
	case DP_F_EPON:
		cbm_w32(cqm_ctrl->cqm + CBM_EPON_BASE,
			CQM_PON_IP_BASE_ADDR);
		pib_program_overshoot(CQM_PON_IP_PKT_LEN_ADJ_BYTES);
		/*clear PIB bypass*/
		ctrl.pib_bypass = 0;
		ctrl.pib_en = 1;
		ctrl.wakeup_intr_en = 1;
		ctrl.pkt_len_adj = 1;
		ctrl.deq_delay = 0xf;
		config_pib_ctrl(&ctrl,
				FLAG_PIB_BYPASS |
				FLAG_PIB_ENABLE |
				FLAG_PIB_DELAY |
				FLAG_PIB_WAKEUP_INTR |
				FLAG_PIB_PKT_LEN_ADJ);
		if (cqm_ctrl->force_xpcs)
			set_ifmux(PRX300_WAN_AON_MODE);
		else
			set_ifmux(PRX300_WAN_PON_MODE);
		break;
	case DP_F_FAST_WLAN:
	case DP_F_FAST_DSL:
		populate = 0;
		if (cbm_port[i] != CBM_PORT_NOT_APPL) {
			test_ptr = is_cbm_allocated(cbm_port[i], flags);
			if (!test_ptr) {
				populate = 1;
				set_wlan_ep(phys_port);
				*loop = CBM_FAILURE;
			}
		}
		config = cbm_r32(deq + DQ_CPU_PORT(cbm_port[i], cfg));
		config |= ((cbm_port[i] << CFG_CPU_EGP_0_EPMAP_POS) &
			   CFG_CPU_EGP_0_EPMAP_MASK);
		cbm_w32((deq + DQ_CPU_PORT(cbm_port[i], cfg)), config);
		break;
	default:
		break;
	}
	if (populate)
		cqm_populate_entry(local_entry, &phys_port, cbm_port[i], flags,
				   pmac_found);
	return CBM_SUCCESS;
}

static s32 cqm_dp_port_dealloc(struct module *owner, u32 dev_port,
			       s32 port_id, struct cbm_dp_alloc_data *data,
			       u32 flags)
{
	u32 num;
	struct cqm_res_t *res = NULL;
	int port, shared_p;
	struct cqm_dqm_port_info *p_info;

	if ((dp_port_resources_get(&port_id, &num, &res,
				   flags) != 0) && (!res)) {
		dev_err(cqm_ctrl->dev,
			"Error getting resources for port_id %d\n", port_id);
		return CBM_FAILURE;
	}
	if (port_id >= PMAC_MAX_NUM || port_id < 0)
		return DP_FAILURE;

	dev_dbg(cqm_ctrl->dev,
		"cqm_delete_from_list port_id:%d, flags:0x%x\n",
		port_id, flags);
	cqm_delete_from_list(port_id, flags);

/*First Subif*/
	port = res[0].cqm_deq_port;
	p_info = &dqm_port_info[port];
	if (p_info->ref_cnt != 0)
		return DP_SUCCESS;

	if (flags & DP_F_SHARE_RES) {
		shared_p = port_id ^ 1;
		if (is_dp_allocated(shared_p, flags)) {
			dev_dbg(cqm_ctrl->dev,
				"shared port_id %d exists\n",
				shared_p);
			return DP_SUCCESS;
		}
	}
	dev_dbg(cqm_ctrl->dev, "%s free deq port = %d flag:%x ptk_base :%x\n",
		__func__, port, flags & FLAG_ACA,
		(unsigned int)p_info->deq_info.pkt_base);

	/* Free ACA port */
	if ((flags & FLAG_ACA) && (p_info->deq_info.pkt_base)) {
		dmam_free_coherent(cqm_ctrl->dev, p_info->deq_info.dma_size,
				   p_info->deq_info.pkt_base,
				   (dma_addr_t)p_info->deq_info.pkt_base_paddr);
		ltq_dma_chan_desc_free(p_info->dma_ch);
	}

	if (p_info->dma_dt_init_type == DEQ_DMA_CHNL) {
		cqm_dequeue_dma_port_uninit(port, 0);
		cqm_dma_port_enable(port, CBM_PORT_F_DEQUEUE_PORT |
				    CBM_PORT_F_DISABLE, 0);
		p_info->dma_ch_in_use = false;
	} else {
		/*ACA port*/
		cbm_w32((cqm_ctrl->deq + DQ_CPU_PORT(port, cfg)), 0x0);
		if (find_dqm_port_type(port) == DQM_ACA_TYPE) {
			dev_dbg(cqm_ctrl->dev, "%s DQM_ACA: reset dptr and dqpc\n",
				__func__);
			cbm_w32((cqm_ctrl->deq + DQ_SCPU_PORT(port, dqpc)), 0);
			cbm_w32((cqm_ctrl->deq + DQ_SCPU_PORT(port, dptr)), 0);
		}
	}
	devm_kfree(cqm_ctrl->dev, res);

	return DP_SUCCESS;
}

static s32
dp_port_alloc(struct module *owner, struct net_device *dev, u32 dev_port,
	      s32 port_id, struct cbm_dp_alloc_data *data, u32 flags)
{
	u32 port_start, port_resv_st = 0;
	u32 port_end, port_resv_end = 0;
	int i, result, cnt;
	int param_pmac = 0, pmac_present = 0, pmac_found = 0;
	u32 cbm_port[PRX300_MAX_PORT_PER_EP],
	pmac[PRX300_MAX_PORT_PER_EP];
	struct cqm_pmac_port_map local_entry = {0};
	struct cqm_pmac_port_map *dp_local_entry = NULL;
	u32 ch_dp_port, ch_num;
	struct cqm_res_t *res = NULL;
	u32 shared_port = 0;
	u32 shared_port_exist = 0;

	data->flags = 0;
	if (!owner)
		return CBM_FAILURE;
	dev_dbg(cqm_ctrl->dev, "%s: port_id %d, flags 0x%x\n",
		__func__, port_id, flags);
	/*to allocate port*/

	if (cqm_ctrl->radio_num) {
		port_resv_st = PMAC_ALLOC_START_ID % 2 + PMAC_ALLOC_START_ID;
		port_resv_end = port_resv_st + 2 * cqm_ctrl->radio_num - 1;
		if (port_resv_end > PMAC_ALLOC_END_ID)
			return DP_FAILURE;
	}

	if (flags & FLAG_LAN) {
		port_start = PMAC_ETH_LAN_START_ID;
		port_end = PMAC_ETH_LAN_END_ID;
		if ((cqm_ctrl->gint_mode > 0) &&
		    (port_id == PMAC_ETH_LAN_END_ID)) {
			dev_err(cqm_ctrl->dev,
				"Use ginit mode flag to alloc LAN port %d\n",
				port_id);
			return DP_FAILURE;
		}
	} else if (flags & (DP_F_FAST_ETH_WAN | DP_F_GPON | DP_F_EPON)) {
		port_start = PMAC_ETH_WAN_ID;
		port_end = PMAC_ETH_WAN_ID;
	} else {
		port_start = PMAC_ALLOC_START_ID;
		port_end = PMAC_ALLOC_END_ID;

		if (flags & DP_F_SHARE_RES) {
			port_start = port_resv_st;
			port_end = port_resv_end;
		}
	}
	/*alloc port */
	if (port_id) {	/*allocate with specified port id */
		if ((port_id > port_end) || (port_id < port_start)) {
			dev_err(cqm_ctrl->dev,
				"pord_id %i out of range [%i..%i]\n", port_id,
				port_start, port_end);
			return DP_FAILURE;
		}

		if ((cqm_ctrl->radio_num) && !(flags & DP_F_SHARE_RES) &&
		    (port_id > port_resv_st) &&
		    (port_id < port_resv_end)) {
			dev_err(cqm_ctrl->dev,
				"pord_id %i is reserved [%i..%i]\n", port_id,
				port_resv_st, port_resv_end);
			return DP_FAILURE;
		}

		if (is_dp_allocated(port_id, flags)) {
			dev_err(cqm_ctrl->dev,
				"already llocated pmac port %d\n", port_id);
			return DP_FAILURE; /*not free*/
		}

		if ((flags & DP_F_SHARE_RES) && (cqm_ctrl->radio_num)) {
			shared_port = port_id ^ 1;
			if (is_dp_allocated(shared_port, flags))
				shared_port_exist = 1;
			else
				shared_port_exist = 0;
		}
		param_pmac = port_id;
		pmac_present = 1;
		goto ALLOC_OK;
	} else { /* dynamic alloc a free port */
		if ((flags & DP_F_SHARE_RES) &&
		    (cqm_ctrl->radio_num)) {
			port_start++; /*start from odd number*/
		}
		for (i = port_start; i <= port_end; i++) {
			if ((cqm_ctrl->radio_num) &&
			    !(flags & DP_F_SHARE_RES) &&
			    (i >= port_resv_st) && (i <= port_resv_end)) {
				i = port_resv_end;
				continue;
			}
			dp_local_entry = is_dp_allocated(i, flags);
			if (!dp_local_entry) {
				port_id = i;
				if (flags & DP_F_SHARE_RES) {
					shared_port = port_id ^ 1;
					dp_local_entry =
						is_dp_allocated(shared_port,
								flags);
					if (dp_local_entry) {
						i++;
						continue;
					}
				}
				goto ALLOC_OK;
			}
			/*to get next even port id for SHARE_RES*/
			if (flags & DP_F_SHARE_RES)
				i++;
		}
	}
	dev_err(cqm_ctrl->dev,
		"Failed to get a free port for module %p\n", owner);
	return DP_FAILURE;
ALLOC_OK:
	if (!shared_port_exist) {
		result = get_matching_pmac(pmac, flags, cbm_port, param_pmac,
					   pmac_present);
	} else {
		pmac[0] = CBM_PMAC_DYNAMIC;
		result = CBM_SUCCESS;
	}
	if (result == CBM_FAILURE) {
		dev_err(cqm_ctrl->dev, "get_matching pmac error\n");
		return DP_FAILURE;
	} else if (result == CBM_NOTFOUND) {
		/*Physical port allocation failure,
		 *means this is just virtual pmac required
		 */
		dev_err(cqm_ctrl->dev, "phys port alloc failure\n");
	} else {
		if ((pmac[0] != CBM_PMAC_DYNAMIC) && (pmac[0] != port_id)) {
			dev_err(cqm_ctrl->dev,
				"%s Error in the mapping table\r\n", __func__);
			return DP_FAILURE;
		}
		local_entry.pmac = port_id;
		local_entry.owner = owner;
		local_entry.dev = dev;
		local_entry.dev_port = dev_port;
		local_entry.flags = P_ALLOCATED;

		if (!shared_port_exist) {
			for (i = 0; i < PRX300_MAX_PORT_PER_EP; i++) {
				if (pmac[i] == CBM_PORT_INVALID)
					continue;
				cnt = i;
				do_port_setting(pmac, flags, cbm_port,
						&local_entry,
						&pmac_found, &cnt);
				if (cnt == CBM_FAILURE)
					break;
			}
			if (!pmac_found)
				return DP_FAILURE;
		}
	}

	if (shared_port_exist) {
			/*get cbm_port[0] accordingly*/
		ch_dp_port = port_id ^ 1;
		local_entry.egp_type = flags;
		result = dp_port_resources_get(&ch_dp_port, &ch_num,
					       &res, flags);

		if (result == CBM_FAILURE)
			return CBM_FAILURE;

		cbm_port[0] = res[0].cqm_deq_port;
		devm_kfree(cqm_ctrl->dev, res);
		cqm_populate_entry(&local_entry,
				   &ch_dp_port, cbm_port[0],
				   flags, &pmac_found);
	}
	cqm_add_to_list(&local_entry);
	fill_dp_alloc_data(data, port_id, cbm_port[0], flags);
	dev_dbg(cqm_ctrl->dev, "%s: port_id:%d, cbm_port:%d\n",
		__func__, port_id, cbm_port[0]);
	return CBM_SUCCESS;
}

static void fill_tx_ring_data(struct cbm_dp_alloc_complete_data *dp_data)
{
	struct cqm_dqm_port_info *p_info;
	void *deq = cqm_ctrl->deq_phy + DESC0_0_CPU_EGP_0;
	void *free = cqm_ctrl->deq_phy + PTR_RTN_CPU_DW2_EGP_0;
	void *deq_v = cqm_ctrl->deq + DESC0_0_CPU_EGP_0;
	void *free_v = cqm_ctrl->deq + PTR_RTN_CPU_DW2_EGP_0;

	u8 ring_idx;

	p_info = &dqm_port_info[dp_data->deq_port];

	for (ring_idx = 0; ring_idx < dp_data->num_tx_ring; ring_idx++) {
		dp_data->tx_ring[ring_idx]->in_deq_ring_size =
				p_info->deq_info.num_desc;
		dp_data->tx_ring[ring_idx]->in_deq_paddr	=
			(deq + (dp_data->deq_port * (DESC0_0_CPU_EGP_1 -
			 DESC0_0_CPU_EGP_0)));
		dp_data->tx_ring[ring_idx]->in_deq_vaddr	=
			(deq_v + (dp_data->deq_port * (DESC0_0_CPU_EGP_1 -
			 DESC0_0_CPU_EGP_0)));
		dp_data->tx_ring[ring_idx]->out_free_paddr =
			(free + (dp_data->deq_port * (PTR_RTN_CPU_DW2_EGP_1 -
			 PTR_RTN_CPU_DW2_EGP_0)));
		dp_data->tx_ring[ring_idx]->out_free_vaddr =
			(free_v + (dp_data->deq_port * (PTR_RTN_CPU_DW2_EGP_1 -
			 PTR_RTN_CPU_DW2_EGP_0)));

		dp_data->tx_ring[ring_idx]->out_free_ring_size =
			p_info->deq_info.num_free_burst;

		/* Need to be disccused and modified later base on policy */
		dp_data->tx_ring[ring_idx]->num_tx_pkt = 0;
		dp_data->tx_ring[ring_idx]->txout_policy_base = 0;
		dp_data->tx_ring[ring_idx]->policy_num = 1;
		dp_data->tx_ring[ring_idx]->tx_pkt_size = 0;
		dp_data->tx_ring[ring_idx]->tx_poolid = 0;
		dp_data->tx_ring[ring_idx]->tx_deq_port = dp_data->deq_port;
	}
}

static int fill_rx_ring_data(struct cbm_dp_alloc_complete_data *dp_data)
{
	u32 buf,  cpu = smp_processor_id();
	struct cqm_dqm_port_info *p_info;
	int ring_idx, idx, ret = CBM_FAILURE;
	size_t tbl_size;
	size_t dma_size;
	void *dma_vaddr;
	dma_addr_t dma_paddr;
	u32 *vbase;

	/* As of now, we are supporting only one rx ring for prx300 */
	if (dp_data->num_rx_ring != NUM_RX_RING) {
		dev_err(cqm_ctrl->dev, "%s: Invalid num_rx_ring %u\r\n",
			__func__, dp_data->num_rx_ring);
		return ret;
	}

	p_info = &dqm_port_info[dp_data->deq_port];

	for (ring_idx = 0; ring_idx < dp_data->num_rx_ring; ring_idx++) {
		tbl_size = dp_data->rx_ring[ring_idx]->prefill_pkt_num
			 * sizeof(u32);
		dma_size = tbl_size * 2;
		/* rx_pkt_size fixed to 2048 */
		if (dp_data->rx_ring[ring_idx]->rx_pkt_size != CQM_KB(2)) {
			dev_err(cqm_ctrl->dev, "%s rx_pkt_size %u\n",
				__func__,
				dp_data->rx_ring[ring_idx]->rx_pkt_size);
			break;
		}

		/* out_enq_ring_size fixed to 4096 */
		if (dp_data->rx_ring[ring_idx]->out_enq_ring_size >=
			CQM_KB(4)) {
			dev_err(cqm_ctrl->dev, "%s out_enq_ring_size %u\n",
				__func__,
				dp_data->rx_ring[ring_idx]->out_enq_ring_size);
			break;
		}

		if (!p_info->dma_ch_in_use) {
			if (p_info->dma_dt_init_type != DEQ_DMA_CHNL) {
				dev_err(cqm_ctrl->dev, "%s Inv DMA type %u dqport %u\n",
					__func__, p_info->dma_dt_init_type,
					dp_data->deq_port);
				break;
			}

			if (ltq_request_dma(p_info->dma_ch,
					    p_info->dma_chan_str)) {
				dev_err(cqm_ctrl->dev, "%s: dma open failed\r\n",
					__func__);
				break;
			}

			if (ltq_dma_chan_desc_alloc(
				p_info->dma_ch,
				dp_data->rx_ring[ring_idx]->out_enq_ring_size)) {
				ltq_free_dma(p_info->dma_ch);
				dev_err(cqm_ctrl->dev, "%s: dma alloc failed\r\n",
					__func__);
				break;
			}
			ltq_dma_chan_polling_cfg(p_info->dma_ch, 6, 6);

			p_info->deq_info.desc_phy_base =
				(void *)ltq_dma_chan_get_desc_phys_base(p_info->
									dma_ch);
			p_info->deq_info.desc_vir_base =
				(void *)ltq_dma_chan_get_desc_vir_base(p_info->
								       dma_ch);
			dma_vaddr = dmam_alloc_coherent(cqm_ctrl->dev, dma_size,
							&dma_paddr, GFP_DMA);
			if (!dma_vaddr) {
				dev_err(cqm_ctrl->dev,
					"%s: dmam_alloc_coherent failed\n",
					__func__);
				ltq_free_dma(p_info->dma_ch);
				break;
			}
			p_info->deq_info.pkt_base_paddr = (void *)dma_paddr;
			p_info->deq_info.pkt_base = dma_vaddr;
			p_info->deq_info.dma_size = dma_size;
		}

		for (idx = 0;
		     (idx < dp_data->rx_ring[ring_idx]->prefill_pkt_num) &&
		     (!p_info->deq_info.pkt_base[idx]);
		     idx++) {
		     /* depends on the dc port policy right now
		      * its fixed to policy 0
		      */
			buf = (u32)cqm_buff_alloc_by_policy_prx300(cpu, 0, 0);
			if (!buf) {
				dev_err(cqm_ctrl->dev, "%s: BM alloc failed\n",
					__func__);

				while (--idx >= 0) {
					buf = p_info->deq_info.pkt_base[idx];
					cqm_buffer_free(cpu, (void *)buf, 1);
				}
				dmam_free_coherent(cqm_ctrl->dev, dma_size,
						   dma_vaddr, dma_paddr);
				ltq_free_dma(p_info->dma_ch);
				return CBM_FAILURE;
			}
			p_info->deq_info.pkt_base[idx] =
				__pa(buf - CQM_POOL_METADATA);
			vbase = p_info->deq_info.pkt_base + tbl_size;
			vbase[idx] = buf - CQM_POOL_METADATA;
		}

		dp_data->rx_ring[ring_idx]->out_dma_ch_to_gswip =
			p_info->dma_ch;
		dp_data->rx_ring[ring_idx]->num_out_tx_dma_ch =
			dp_data->num_rx_ring;
		dp_data->rx_ring[ring_idx]->out_enq_paddr =
			p_info->deq_info.desc_phy_base;
		dp_data->rx_ring[ring_idx]->out_enq_vaddr =
			p_info->deq_info.desc_vir_base;
		dp_data->rx_ring[ring_idx]->pkt_base_paddr =
			p_info->deq_info.pkt_base_paddr;
		dp_data->rx_ring[ring_idx]->pkt_base_vaddr =
			p_info->deq_info.pkt_base;
		dp_data->rx_ring[ring_idx]->pkt_list_vaddr =
			p_info->deq_info.pkt_base + tbl_size;
		dp_data->rx_ring[ring_idx]->num_pkt =
				dp_data->rx_ring[ring_idx]->prefill_pkt_num;
		p_info->deq_info.prefill_pkt_num =
				dp_data->rx_ring[ring_idx]->prefill_pkt_num;
		/* Need to be disccused and modified later base on policy */
		dp_data->rx_ring[ring_idx]->rx_policy_base = 0;
		dp_data->rx_ring[ring_idx]->policy_num = 1;
		p_info->dma_ch_in_use = true;
		ret = CBM_SUCCESS;
	}

	return ret;
}

static s32 dp_port_dealloc_complete(struct cbm_dp_alloc_complete_data *dp,
				    s32 port_id, u32 flags)
{
	u32 num;
	struct cqm_res_t *res = NULL;
	int port, idx, shared_p;
	struct cqm_dqm_port_info *p_info;
	void *buf;
	struct cqm_pmac_port_map *shared_dp = NULL;
	struct cqm_pmac_port_map *local_dp = NULL;

	dev_dbg(cqm_ctrl->dev, "%s: enter port_id:%d, flags:%x\n",
		__func__, port_id, flags);

	if ((dp_port_resources_get(&port_id, &num, &res,
				   flags) != 0) && (!res)) {
		dev_err(cqm_ctrl->dev,
			"Error getting resources for port_id %d\n", port_id);
		return CBM_FAILURE;
	}
	if (port_id >= PMAC_MAX_NUM || port_id < 0)
		return DP_FAILURE;

	port = res[0].cqm_deq_port;
	p_info = &dqm_port_info[port];
	if (p_info->ref_cnt != 0)
		return DP_SUCCESS;

	local_dp = is_dp_allocated(port_id, flags);
	if (!local_dp) {
		dev_err(cqm_ctrl->dev, "%s invalid dp %d\n",
			__func__, port_id);
		return DP_FAILURE;
	}

	if (local_dp->aca_buf_ref_cnt)
		local_dp->aca_buf_ref_cnt = 0;

	if (flags & DP_F_SHARE_RES) {
		shared_p = port_id ^ 1;
		shared_dp = is_dp_allocated(shared_p, flags);
		if (shared_dp) {
			if (shared_dp->aca_buf_ref_cnt) {
				dev_dbg(cqm_ctrl->dev,
					"shared port_id %d aca buf in use\n",
					shared_p);
					return DP_SUCCESS;
			}
		}
	}

	dev_dbg(cqm_ctrl->dev, "%s free deq port = %d flag:%x ptk_base :%lx\n",
		__func__, port, flags & FLAG_ACA,
		(unsigned long)p_info->deq_info.pkt_base);

	/* Free ACA port */
	if ((flags & FLAG_ACA) && (p_info->deq_info.pkt_base)) {
		struct cqm_bm_free free_info;

		free_info.policy_base = 0;
		for (idx = 0; idx < p_info->deq_info.prefill_pkt_num; idx++) {
			if (p_info->deq_info.pkt_base[idx]) {
				buf = (void *)p_info->deq_info.pkt_base[idx];
				free_info.buf = buf;
				cqm_buffer_free_by_policy_prx300(&free_info);
				p_info->deq_info.pkt_base[idx] = 0;
			} else {
				break;
			}
		}
	}

	devm_kfree(cqm_ctrl->dev, res);

	return DP_SUCCESS;
}

static s32 dp_port_alloc_complete(struct module *owner, struct net_device *dev,
				  u32 dev_port, s32 dp_port,
				  struct cbm_dp_alloc_complete_data *data,
				  u32 flags)
{
	u32 reg;
	struct cqm_pmac_port_map *local_dp = NULL;

	dev_dbg(cqm_ctrl->dev, "%s: enter dp_port:%d, flags:0x%x\n",
		__func__, dp_port, flags);

	if (!owner)
		return CBM_FAILURE;

	if (flags & DP_F_DEREGISTER)
		return dp_port_dealloc_complete(data, dp_port, flags);

	if (flags & FLAG_ACA) {
		local_dp = is_dp_allocated(dp_port, flags);
		if (local_dp) {
			if (!local_dp->aca_buf_ref_cnt)
				local_dp->aca_buf_ref_cnt++;
		}

		if (fill_rx_ring_data(data) != CBM_SUCCESS)
			return CBM_FAILURE;
		fill_tx_ring_data(data);
	} else if (flags & DP_F_EPON) {
		reg = (EPON_EPON_MODE_REG_EPONCHKEN_MASK |
		       EPON_EPON_MODE_REG_EPONPKTSIZADJ_MASK |
		       (data->deq_port << EPON_EPON_MODE_REG_EPONBASEPORT_POS) |
		       ((data->qid_base + (data->num_qid - 1)) <<
			 EPON_EPON_MODE_REG_ENDQ_POS)
		       | data->qid_base);
		cbm_w32(cqm_ctrl->enq + EPON_EPON_MODE_REG, reg);
	}
	data->tx_policy_base = 0;
	data->tx_policy_num = cqm_ctrl->num_pools;

	dev_dbg(cqm_ctrl->dev, "%s: exit\n", __func__);
	return CBM_SUCCESS;
}

static s32 handle_dma_chnl_init(int port, u32 flags)
{
	struct cqm_dqm_port_info *p_info;
	int chan;
	int port_type = find_dqm_port_type(port);

	if (port_type == DQM_PON_TYPE)
		p_info = &dqm_port_info[DQM_PON_START_ID];
	else
		p_info = &dqm_port_info[port];

	chan = p_info->dma_ch;

	if (flags & CBM_PORT_F_DISABLE) {
		if (port_type != DQM_PON_TYPE) {
			if (chan)
				ltq_dma_chan_off(chan);
			usleep_range(20, 25);
			/*disable auto read inrcement*/
			set_val((cqm_ctrl->deq + DQ_DMA_PORT((port), cfg)),
				1, 0x2, 1);
			if (flags & CBM_PORT_F_FLUSH)
				cqm_deq_port_flush(port);

			cqm_dma_port_enable(port, CBM_PORT_F_DEQUEUE_PORT |
					    CBM_PORT_F_DISABLE, 0);
		}
	} else {
		if (chan && p_info->dma_ch_in_use) {
			cqm_dma_port_enable(port, CBM_PORT_F_DEQUEUE_PORT, 0);
			ltq_dma_chan_on(chan);
		} else {
			cqm_dma_port_enable(port, CBM_PORT_F_DEQUEUE_PORT, 0);
			cqm_dq_dma_chan_init(port, p_info->dma_dt_init_type);
			p_info->dma_ch_in_use = true;
		}
	}
	return CBM_SUCCESS;
}

static void cqm_free_aca_port(s32 port)
{
	struct cqm_dqm_port_info *p_info = &dqm_port_info[port];
	int id = 0, cnt = p_info->deq_info.num_desc * 3;
	void *deq = cqm_ctrl->deq;
	u32 reg, val;
	u32 flush_cnt = 0;

	dev_dbg(cqm_ctrl->dev, "%s port:%d desc_num:%d, base:%x\n",
		__func__, port, cnt, DQ_SCPU_PORT(port, desc[0].desc0));
	enable_backpressure(port, 0);
	while (cnt--) {
		id = id % p_info->deq_info.num_desc;
		reg = cbm_r32(deq + DQ_SCPU_PORT(port, desc[id].desc0) + 0xC);
		rmb(); /* read before write */
		dev_dbg(cqm_ctrl->dev, "%s id:%d dw3:%x\n",
			__func__, id, reg);
		if (reg & OWN_BIT) {
			val = cbm_r32(deq +
				      DQ_SCPU_PORT(port, desc[id].desc0) + 8);
			cbm_w32((deq +
				 DQ_SCPU_PORT(port, scpu_ptr_rtn[id].ptr_rtn0)),
				 val);
			/* FMX DC port limitation,
			 * only one pool = 0  and policy = 0
			 */
			cbm_w32((deq +
				 DQ_SCPU_PORT(port, scpu_ptr_rtn[id].ptr_rtn1)),
				 0); /*hardcode pool and policy to 0*/
			wmb(); /* write before read */
			flush_cnt++;
			dev_dbg(cqm_ctrl->dev, "%s dw2:%x, addr:%x, reg:%x\n",
				__func__, val, (unsigned int)
				DQ_SCPU_PORT(port, scpu_ptr_rtn[id].ptr_rtn0),
				reg);
		}
		id++;
	}
	/*dqpc and dptr to be set to 0 after port is disabled*/
	dev_info(cqm_ctrl->dev, "%s flush_cnt:%d\n", __func__, flush_cnt);
}

static void cqm_deq_pon_port_disable(u32 port, u32 alloc_flags)
{
	struct cqm_dqm_port_info *p_info;
	void *deq = cqm_ctrl->deq;
	int chan, i;
	u32  val = 0;
	struct pib_stat stat;

	p_info = &dqm_port_info[port];
	chan = p_info->dma_ch;

	if (alloc_flags & (DP_F_EPON | DP_F_GPON)) {
		val = ((port - DQM_PON_START_ID) <<
		       PIB_PON_IP_CMD_PORT_ID_POS) | 0x28000;
		cbm_w32(cqm_ctrl->pib + PIB_PON_IP_CMD, val);
		dev_dbg(cqm_ctrl->dev, "%s: write CQEM_PIB_PIB_PON_IP_CMD %lx , val %x\n",
			__func__,
			(unsigned long)cqm_ctrl->pib + PIB_PON_IP_CMD, val);
		pib_status_get(&stat);
		dev_dbg(cqm_ctrl->dev, "%s waiting for fifo_emtpy %d\n",
			__func__, stat.fifo_empty);
		while (!stat.fifo_empty)
			pib_status_get(&stat);

		dev_dbg(cqm_ctrl->dev, "%s disable all DQ PON PORT %d - %d\n",
			__func__,
			 DQM_PON_START_ID, DQM_PON_END_ID);
		for (i = DQM_PON_START_ID; i < DQM_PON_END_ID; i++)
			cbm_w32((deq + DQ_PON_PORT((i), cfg)), 0);

	} else if (alloc_flags & DP_F_FAST_ETH_WAN) {
		if (chan) {
			dev_dbg(cqm_ctrl->dev, " %s dma_chan off for port %d\n",
				__func__, port);
			ltq_dma_chan_off(chan);
		}
		cqm_deq_port_flush(port);
		cbm_w32((deq + DQ_PON_PORT((port), cfg)), 0);
	}
}

static s32 dp_enable(struct module *owner, u32 port_id,
		     struct cbm_dp_en_data *data, u32 flags, u32 alloc_flags)
{
	int port, type, i, found = 0;
	u32 val, dma_flag = 0;
	struct core_ops *ops;
	GSW_PMAC_Glbl_Cfg_t glbl_cfg;

	static bool set_bsl;
	void *deq = cqm_ctrl->deq;
	struct cqm_dqm_port_info *dqp_info;
	struct cqm_pmac_port_map *entry_ptr = NULL;

	port = data->deq_port;

	dev_dbg(cqm_ctrl->dev, "%s cbm_port:%d , port_id %d flags:%x, alloc_flags:%x\n",
		__func__, port, port_id, flags, alloc_flags);

	if (port == -1) {
		dev_err(cqm_ctrl->dev, "Invalid port number\n");
		return CBM_FAILURE;
	}

	dqp_info = &dqm_port_info[data->deq_port];
	if (flags & CBM_PORT_F_DISABLE) {
		if (dqp_info->ref_cnt) {
			flags |= CBM_PORT_F_FLUSH;
			dqp_info->ref_cnt--;
		} else {
			return CBM_SUCCESS;
		}
	} else {
		if (!dqp_info->ref_cnt)
			dqp_info->ref_cnt++;
		else
			return CBM_SUCCESS;
	}

	list_for_each_entry(entry_ptr, &pmac_mapping_list, list) {
		if (entry_ptr->pmac == port_id) {
			found = 1;
			break;
		}
	}
	if (!found) {
		dev_err(cqm_ctrl->dev, "Invalid port_id:%d\n", port_id);
		return CBM_FAILURE;
	}

	if (flags & CBM_PORT_F_DISABLE) {
		if (entry_ptr->eqp_en_cnt) {
			entry_ptr->eqp_en_cnt--;
			if (!entry_ptr->eqp_en_cnt)
				dma_flag = 1;
		}
	} else {
		if (!entry_ptr->eqp_en_cnt) {
			entry_ptr->dma_chnl_init = data->dma_chnl_init;
			entry_ptr->num_dma_chan = data->num_dma_chan;
			dma_flag = 1;
		}
		entry_ptr->eqp_en_cnt++;
	}

	if ((entry_ptr->dma_chnl_init) && dma_flag) {
		for (i = 0; i < entry_ptr->num_dma_chan; i++)
			handle_dma_chnl_init((port + i), flags);
	}
	type = find_dqm_port_type(port);
	switch (type) {
	case DQM_CPU_TYPE:
	case DQM_ACA_TYPE:
		if (flags & CBM_PORT_F_DISABLE) {
			if (type == DQM_ACA_TYPE)
				cqm_free_aca_port(port);
		} else {
			init_cqm_deq_cpu_port(port, data->tx_ring_size);
			dqp_info->tx_ring_size = data->tx_ring_size;
		}
		break;
	case DQM_PON_TYPE:
		if ((flags & CBM_PORT_F_DISABLE) &&
		    (entry_ptr->dma_chnl_init) && dma_flag)
			cqm_deq_pon_port_disable(port, alloc_flags);

		if ((flags & CBM_PORT_F_DISABLE) &&
		    ((!entry_ptr->dma_chnl_init) || !dma_flag) &&
		    (alloc_flags & (DP_F_EPON | DP_F_GPON))) {
			val = ((port - DQM_PON_START_ID)
				<< PIB_PON_IP_CMD_PORT_ID_POS) | 0x28000;
			cbm_w32(cqm_ctrl->pib + PIB_PON_IP_CMD, val);
			dev_dbg(cqm_ctrl->dev,
				"%s: write CQEM_PIB_PIB_PON_IP_CMD %lx , val %x\n",
				__func__,
				(unsigned long)cqm_ctrl->pib + PIB_PON_IP_CMD,
				val);
		}

		if (!(flags & CBM_PORT_F_DISABLE)) {
			val = CFG_PON_EGP_26_DQREQ_MASK |
				CFG_PON_EGP_26_BUFRTN_MASK |
				CFG_PON_EGP_26_BFBPEN_MASK |
				CFG_PON_EGP_26_DQPCEN_MASK;
			val |= ((data->deq_port << CFG_DMA_EGP_7_EPMAP_POS) &
				CFG_DMA_EGP_7_EPMAP_MASK);
			cbm_w32((deq + DQ_PON_PORT((port), cfg)),
				val);
			pib_port_enable(port, 1);
		}
		break;
	case DQM_DMA_TYPE:
	case DQM_VUNI_TYPE:
		break;
	default:
		dev_err(cqm_ctrl->dev, "Unknown port type %d\n", type);
		return CBM_FAILURE;
	break;
	}
	if (!set_bsl) {
		ops = gsw_get_swcore_ops(0);
		if (cqm_ctrl->split[0] != 2 || cqm_ctrl->split[1] != 2) {
			for (i = 0; i < 2; i++) {
				glbl_cfg.nPmacId = i;
				if (ops)
					ops->gsw_pmac_ops.Pmac_Gbl_CfgGet(ops,
						&glbl_cfg);
				val = get_buff_resv_bytes(0, 3);
				glbl_cfg.nMaxJumboLen = val;
				val = get_buff_resv_bytes(0, 0);
				glbl_cfg.nBslThreshold[0] = val;
				val = get_buff_resv_bytes(0, 1);
				glbl_cfg.nBslThreshold[1] = val;
				val = get_buff_resv_bytes(0, 2);
				glbl_cfg.nBslThreshold[2] = val;

				if (ops)
					ops->gsw_pmac_ops.Pmac_Gbl_CfgSet(ops,
						&glbl_cfg);
			}
		} else {
			val = get_buff_resv_bytes(0, 3);
			glbl_cfg.nMaxJumboLen = val;

			glbl_cfg.nPmacId = 0;
			if (ops)
				ops->gsw_pmac_ops.Pmac_Gbl_CfgGet(ops,
					&glbl_cfg);
			glbl_cfg.nBslThreshold[0] = 0x2ba;
			glbl_cfg.nBslThreshold[1] = 0x3000;
			glbl_cfg.nBslThreshold[2] = 0x3000;
			if (ops)
				ops->gsw_pmac_ops.Pmac_Gbl_CfgSet(ops,
					&glbl_cfg);
			glbl_cfg.nPmacId = 1;
			if (ops)
				ops->gsw_pmac_ops.Pmac_Gbl_CfgGet(ops,
								  &glbl_cfg);
			glbl_cfg.nBslThreshold[0] = 0x10;
			glbl_cfg.nBslThreshold[1] = 0x11;
			glbl_cfg.nBslThreshold[2] = 0x2ba;
			if (ops)
				ops->gsw_pmac_ops.Pmac_Gbl_CfgSet(ops,
						&glbl_cfg);
		}
		set_bsl = true;
	}

	return CBM_SUCCESS;
}

int get_fsqm_ofsc(void)
{
	void *fsqm = cqm_ctrl->fsqm;

	return cbm_r32(fsqm + OFSC);
}

static void init_fsqm(void)
{
	int i;
	int minlsa, maxlsa;
	u32 fsqm_frm_num;
	void *fsqm = cqm_ctrl->fsqm;

	fsqm_frm_num = CQM_SRAM_SIZE / CQM_SRAM_FRM_SIZE;
	dev_dbg(cqm_ctrl->dev, "fsqm frm num %d", fsqm_frm_num);
	for (i = 1; i <= fsqm_frm_num; i++) {
		if (i == fsqm_frm_num - 1) {
			cbm_w32((FSQM_LLT_RAM(fsqm, i - 1)), 0x7FFF);
		} else {
			cbm_w32((FSQM_LLT_RAM(fsqm, i - 1)),
				i % fsqm_frm_num);
		}
#ifdef ENABLE_LL_DEBUG
		cbm_w32((FSQM_RCNT(fsqm, i - 1)), 0);
#else
		cbm_w32((FSQM_RCNT(fsqm, i - 1)), 1);
#endif
	}
	/* minlas is always zero, it's offset counted into the BA */
	minlsa = 0;
	/* The last item is invalid inside the FSQM */
	maxlsa = fsqm_frm_num - 2;
	cbm_w32((fsqm + LSARNG),
		((maxlsa << LSARNG_MAXLSA_POS) & LSARNG_MAXLSA_MASK) |
		((minlsa << LSARNG_MINLSA_POS) & LSARNG_MINLSA_MASK));
	cbm_w32((fsqm + OFSQ),
		((maxlsa << OFSQ_TAIL_POS) & OFSQ_TAIL_MASK) |
		((minlsa << OFSQ_HEAD_POS) & OFSQ_HEAD_MASK));
	cbm_w32((fsqm + OFSC),
		((maxlsa - minlsa + 1) << OFSC_FSC_POS) &
		OFSC_FSC_MASK);

	cbm_w32((fsqm + FSQM_IRNEN), 0x111101F);

	cbm_w32((fsqm + FSQT0), fsqm_frm_num / 5);
	cbm_w32((fsqm + FSQT1), fsqm_frm_num / 8);
	cbm_w32((fsqm + FSQT2), fsqm_frm_num / 12);

	/* FSQT4 should be 2x(Number of 128B segments for the largest packet)
	 * for supporting 9.K Jumbo buffer plus some safety margin, we set it to
	 * 0xa0.FSQT4=FSQT3 will cause back-pressure when the threshold is
	 * reached. So set FSQT3 to 0xa3
	 */
	cbm_w32((fsqm + FSQT3), 0xa3);
	cbm_w32((fsqm + FSQT4), 0xa0);
	cbm_w32(fsqm + IO_BUF_RD, 0);
	cbm_w32(fsqm + IO_BUF_WR, 0);
	cbm_w32(fsqm + FSQM_CTRL, 0x1);
	/* Dump FSQM registers */
	{
#if 1
		print_reg("FSQM_CTRL",	(u32 *)(fsqm + FSQM_CTRL));
		print_reg("IO_BUF_RD",	(u32 *)(fsqm + IO_BUF_RD));
		print_reg("IO_BUF_WR",	(u32 *)(fsqm + IO_BUF_WR));
		print_reg("FSQM_IRNCR", (u32 *)(fsqm + FSQM_IRNCR));
		print_reg("FSQM_IRNEN", (u32 *)(fsqm + FSQM_IRNEN));
		print_reg("FSQM_OFSQ",	(u32 *)(fsqm + OFSQ));
#endif
		print_reg("FSQM_OFSC",	(u32 *)(fsqm + OFSC));
#if 1
		print_reg("FSQM_FSQT0", (u32 *)(fsqm + FSQT0));
		print_reg("FSQM_FSQT1", (u32 *)(fsqm + FSQT1));
		print_reg("FSQM_FSQT2", (u32 *)(fsqm + FSQT2));
		print_reg("FSQM_FSQT3", (u32 *)(fsqm + FSQT3));
		print_reg("FSQM_FSQT4", (u32 *)(fsqm + FSQT4));
		print_reg("FSQM_LSARNG", (u32 *)(fsqm + LSARNG));
#endif
	}
	dev_info(cqm_ctrl->dev, "fsqm init successfully, free buffers %d\n",
		 get_fsqm_ofsc());
}

static void ls_intr_handler(u32 pid)
{
	u32 ls_intr_stat;
	void *ls = cqm_ctrl->ls;

	ls_intr_stat = cbm_r32(ls + IRNICR_LS);
	if (ls_intr_stat & (3 << (pid * 2 + 16)))
		dev_dbg(cqm_ctrl->dev, "pid:%d, interrupt\n", pid);
	ls_intr_stat &= (3 << (pid * 2 + 16));
	cbm_w32(ls + IRNCR_LS, ls_intr_stat);
}

static struct sk_buff *build_skb_cqm(void *data, unsigned int frag_size,
				     gfp_t priority)
{
	u32 buf_size;
	int pool = 0, policy;

	if (get_metadata((u8 *)data, &pool, &policy)) {
		dev_err(cqm_ctrl->dev, "cpu buff for %s\n", __func__);
		return NULL;
	}
	buf_size = bm_pool_conf[pool].buf_frm_size;
	if (frag_size > buf_size) {
		dev_err(cqm_ctrl->dev, "Insufficient Headroom %d %d\n",
			frag_size, buf_size);
		return NULL;
	}
	return __build_skb(data + CQM_POOL_METADATA, frag_size);
}

static struct sk_buff *cqm_alloc_skb(unsigned int size, gfp_t priority)
{
	void *buf = NULL;
	struct sk_buff *skbuf = NULL;

	size = SKB_DATA_ALIGN(size +
			      CBM_DMA_DATA_OFFSET +
			      NET_SKB_PAD + CQM_POOL_METADATA)
			      + SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
	buf = cqm_buffer_alloc_by_size(smp_processor_id(), 0, size);
	if (!buf)
		return NULL;
	buf -= CQM_POOL_METADATA;
	skbuf = build_skb_cqm((void *)buf, size, priority);
	if (skbuf)
		skb_reserve(skbuf, CBM_DMA_DATA_OFFSET +
			    NET_SKB_PAD + CQM_POOL_METADATA);
	else {
		pr_err("build_skb_cqm failed\n");
		cqm_buffer_free(smp_processor_id(), buf, 0);
	}
	return skbuf;
}

static void cqm_deq_port_flush(int port)
{
	int port_type = find_dqm_port_type(port);
	void *desc3_addr_base;
	u32 dw3, no_packet = 0;
	u32 i = 0;
	u32 d_num = dqm_port_info[port].deq_info.num_desc;
	u32 total_free_cnt = 0;
	u32 dqpc_reg;

	if (port_type == DQM_DMA_TYPE)
		desc3_addr_base =
			cqm_ctrl->dmadesc + CQM_DEQ_DMA_DESC(port, 0) + 0xc;
	else if (port_type == DQM_PON_TYPE)
		desc3_addr_base =
			cqm_ctrl->dmadesc + CQM_DEQ_PON_DMA_DESC(port, 0)
			+ 0xc;
	else
		return;

	while (no_packet < 2 * d_num) {
		dw3 = cbm_r32(desc3_addr_base + i * 0x10);
		if (dw3 & OWN_BIT) {
			total_free_cnt++;
			dev_dbg(cqm_ctrl->dev, "%s desc_%d, addr, dw3:%x, cnt:%d\n",
				__func__, i, dw3, total_free_cnt);
			cbm_w32((void *)desc3_addr_base + i * 0x10,
				(dw3 & ~OWN_BIT) | COMPLETE_BIT);
			no_packet = 0;
		} else {
			no_packet++;
			dev_dbg(cqm_ctrl->dev, "no_packet %d desc_%d:0x%x\n",
				no_packet, i, dw3);
		}
		i = (i + 1) % d_num;
	}

	if (port_type == DQM_DMA_TYPE) {
		dqpc_reg = cbm_r32(cqm_ctrl->deq + DQ_DMA_PORT(port, dqpc));
		dev_dbg(cqm_ctrl->dev, "%s: DQPC_EGP_%d = %d total flush %d\n",
			__func__, port, dqpc_reg, total_free_cnt);
	} else {
		dqpc_reg = cbm_r32(cqm_ctrl->pib + CQM_PON_CNTR(port));
		dev_dbg(cqm_ctrl->dev, "%s: DQPC_PON_%d = %d total flush %d\n",
			__func__, port, dqpc_reg, total_free_cnt);
	}
}

s32 qos_q_flush(s32 cqm_inst, s32 cqm_drop_port, s32 qid, s32 nodeid)
{
	void *dma_desc = cqm_ctrl->dmadesc;
	int no_packet = 0;
	u32 port = DMA_PORT_FOR_FLUSH;
	u32 reg, dqpc_reg;
	u32 i = dqm_port_info[port].flush_index;
	u32 desc_num = dqm_port_info[port].deq_info.num_desc;
	u32 total_flush = 0;
	u32 qocc_q = 0;
	struct dp_qos_queue_info qocc_info;

	qocc_info.nodeid = nodeid;
	qocc_info.inst = cqm_inst;

	while ((no_packet < Q_FLUSH_NO_PACKET_CNT) || (qocc_q)) {
		reg = cbm_r32(dma_desc +
				CQM_DEQ_DMA_DESC(port, i) + 0xc);
		if (reg & OWN_BIT) {
			cbm_w32(dma_desc +
				CQM_DEQ_DMA_DESC(port, i) + 0xc,
				(reg & ~OWN_BIT) | COMPLETE_BIT);

			i = (i + 1) % desc_num;
			no_packet = 0;
			total_flush++;
		} else {
			no_packet++;
			dev_dbg(cqm_ctrl->dev, "no_packet %d\n", no_packet);
			if (no_packet >= Q_FLUSH_NO_PACKET_CNT) {
				if (dp_qos_get_q_qocc(&qocc_info,
						      0) == DP_SUCCESS) {
					qocc_q = qocc_info.qocc;
					if (qocc_q)
						no_packet = 0;
				} else {
					qocc_q = 0;
					dev_info(cqm_ctrl->dev, "qocc read error!\n");
				}
				dev_dbg(cqm_ctrl->dev, "%s qid:%d nodeid:%d qocc:%d\n",
					__func__, qid, nodeid, qocc_q);
			}
		}
	}
	dqm_port_info[port].flush_index = i;

	dqpc_reg = cbm_r32(cqm_ctrl->deq + DQ_DMA_PORT(port, dqpc));
	dev_dbg(cqm_ctrl->dev, "%s: DQPC_EGP_%d =  %d, flush cnt:%d\n",
		__func__, port, dqpc_reg, total_flush);

	return CBM_SUCCESS;
}

#ifdef CPU_POOL_ALLOWED
static void cqm_cpu_free_tasklet(unsigned long cpu)
{
	int rtn_cnt = 0;
	struct sk_buff *base_addr;
	int i, j = 0, offset, end;
	static int rd_ptr;
	void *base = cqm_ctrl->cqm;
	void *metadata;
	void *data_ptr;
	int pool = CQM_CPU_POOL;
	int policy = CQM_CPU_POLICY;

	offset = sizeof(struct skbuff *);
	while ((rtn_cnt = cbm_r32(base + CBM_CPU_POOL_BUF_RTRN_CNT))) {
		dev_dbg(cqm_ctrl->dev, "rtn_cnt %d rd_ptr %d\n", rtn_cnt,
			rd_ptr);
		end = rd_ptr + rtn_cnt - CQM_CPU_POOL_BUF_ALW_NUM;
		if (end > 0) {
			dev_dbg(cqm_ctrl->dev, "end %d\n", end);
			/*this mean there is wrap around */
			dma_map_single(cqm_ctrl->dev,
				       (void *)(cqm_ctrl->cpu_rtn_aligned +
				       rd_ptr),
				       (CQM_CPU_POOL_BUF_ALW_NUM - rd_ptr) *
				       sizeof(u32), DMA_FROM_DEVICE);
			dma_map_single(cqm_ctrl->dev,
				       (void *)cqm_ctrl->cpu_rtn_aligned,
				       (end) * sizeof(u32), DMA_FROM_DEVICE);
		} else {
			dma_map_single(cqm_ctrl->dev,
				       (void *)(cqm_ctrl->cpu_rtn_aligned +
				       rd_ptr),
				       rtn_cnt * sizeof(u32), DMA_FROM_DEVICE);
		}
		for (i = 0; i < rtn_cnt; i++) {
			data_ptr = (void *)(cqm_ctrl->cpu_rtn_aligned[rd_ptr] -
					    offset);
			data_ptr = __va(data_ptr);
			dev_dbg(cqm_ctrl->dev, "ptr0x%p\n",
				(void *)cqm_ctrl->cpu_rtn_aligned[rd_ptr]);
			dev_dbg(cqm_ctrl->dev, "data_ptr 0x%p\n", data_ptr);
			memcpy(&metadata, data_ptr, offset);
			base_addr = metadata;
			dev_dbg(cqm_ctrl->dev, "base_addr 0x%p\n", base_addr);
			rd_ptr++;
			rd_ptr = rd_ptr % CQM_CPU_POOL_BUF_ALW_NUM;
			UP_STATS(cqm_dbg_cntrs[policy][pool].isr_free_cnt);
			dev_kfree_skb_any(base_addr);
		}
		cbm_w32(base + CBM_CPU_POOL_ENQ_DEC, rtn_cnt);
		/*make sure decrement cnt is written*/
		wmb();
		j++;
		if (j >= (FREE_BUDGET - 1)) {
			tasklet_schedule(&cqm_ctrl->cqm_cpu_free_tasklet);
			return;
		}
	}
	cbm_w32(base + CQM_INT_LINE(4, cbm_irncr), CBM_IRNCR_0_CPUPI_MASK);
	set_val((base + CQM_INT_LINE(4, cbm_irnen)), 1, CBM_IRNCR_0_CPUPI_MASK,
		CBM_IRNCR_0_CPUPI_POS);
	dev_dbg(cqm_ctrl->dev, "tasklet done\n");
}
#endif

int is_fsqm(u32 desc2)
{
	if ((desc2 >= (u32)(CQM_SRAM_BASE)) &&
	    (desc2 < (u32)(CQM_SRAM_BASE + CQM_SRAM_SIZE)))
		return 1;
	return 0;
}

static inline int is_bm(u32 desc2)
{
	int i = 0;

	for (i = 0; i < CQM_PRX300_NUM_BM_POOLS; i++) {
		if ((desc2 >= bm_pool_conf[i].pool_start_low) &&
		    (desc2 < bm_pool_conf[i].pool_end_low))
			return 1;
	}
	return 0;
}

#ifdef ENABLE_LL_DEBUG
/** Trigger when taklet schedule calls*/
static void do_cbm_debug_tasklet(unsigned long cpu)
{
	u32 err_port = 0;
	u32 err_egpid = 0;
	u32 err_igpid = 0;
	u32 err_lsa = 0;
	void *c_base = cqm_ctrl->cqm;

	err_port = get_val(cbm_r32(c_base + CBM_LL_DBG),
			   CBM_LL_DBG_ERR_PORT_MASK,
			   CBM_LL_DBG_ERR_PORT_POS);

	err_egpid = get_val(cbm_r32(c_base + CBM_LL_DBG),
			    CBM_LL_DBG_ERR_EGPID_MASK,
			    CBM_LL_DBG_ERR_EGPID_POS);

	err_igpid = get_val(cbm_r32(c_base + CBM_LL_DBG),
			    CBM_LL_DBG_ERR_IGPID_MASK,
			    CBM_LL_DBG_ERR_IGPID_POS);

	err_lsa = get_val(cbm_r32(c_base + CBM_LL_DBG),
			  CBM_LL_DBG_ERR_LSA_MASK,
			  CBM_LL_DBG_ERR_LSA_POS);
	if (err_port)
		pr_err("***Double free egp %u %u\n", err_egpid, err_lsa);
	else
		pr_err("***Double free igp %u %u\n", err_igpid, err_lsa);

	cbm_w32(c_base + CQM_INT_LINE(5, cbm_irncr), 1 << 16);
	cbm_r32(c_base + CQM_INT_LINE(5, cbm_irnicr));
	/*Dummy read for write flush*/
	cbm_r32(c_base + CQM_INT_LINE(5, cbm_irncr));
	/*finish read*/
	rmb();
	/* Enable all interrupts */
	cbm_w32((c_base + CQM_INT_LINE(5, cbm_irnen)), (1) << 16);
}

#endif

/** Trigger when taklet schedule calls*/
static void do_cqm_tasklet(unsigned long cpu)
{
	struct cbm_desc_list *desc_list = NULL;
	struct sk_buff *skb;
	u32 pkt_recvd = 0;
	int data_len = 0, j = 0;
	u32 data_ptr;
	int real_len = 0, temp_len = 0, new_offset;
	u32 real_buf_start;
	int pool, policy, buf_size;
	int data_offset, var_offset;
	void *c_base = cqm_ctrl->cqm;
	void *ls_base = cqm_ctrl->ls;

/* Dequeue the packet */
	dev_dbg(cqm_ctrl->dev, "am running in cpu %lu\r\n", cpu);
	j = 0;
	while ((pkt_recvd = get_val(cbm_r32(ls_base +
					    CBM_LS_PORT(cpu, status)),
					    LS_STATUS_PORT0_QUEUE_LEN_MASK,
					    LS_STATUS_PORT0_QUEUE_LEN_POS))
				     > 0) {
		dev_dbg(cqm_ctrl->dev, "pkt_recvd %d\n", pkt_recvd);
		j++;

		if ((j >= CQM_DEQ_BUF_SIZE - 1)) {
			tasklet_schedule(&cqm_ctrl->cqm_tasklet[cpu]);
			return;
		}
		g_cqm_dlist[cpu][0].desc.desc0 = cbm_r32(ls_base +
						 CBM_LS_PORT(cpu, desc.desc0));
		g_cqm_dlist[cpu][0].desc.desc1 = cbm_r32(ls_base +
						 CBM_LS_PORT(cpu, desc.desc1));
		g_cqm_dlist[cpu][0].desc.desc2 = cbm_r32(ls_base +
						 CBM_LS_PORT(cpu, desc.desc2));
		g_cqm_dlist[cpu][0].desc.desc3 = cbm_r32(ls_base +
						 CBM_LS_PORT(cpu, desc.desc3));
		desc_list = &g_cqm_dlist[cpu][0];
		/* Build the SKB */
		data_len = cqm_desc_data_len(desc_list->desc.desc3);
		pool = cqm_desc_data_pool(desc_list->desc.desc3);
		policy = cqm_desc_data_policy(desc_list->desc.desc3);
		if (is_fsqm(desc_list->desc.desc2)) {
			dev_err(cqm_ctrl->dev, "FSQM buffer 0x%x %d %d\n",
				desc_list->desc.desc2, pool, policy);
			cqm_buffer_free(cpu, (void *)desc_list->desc.desc2, 0);
			UP_STATS(cqm_dbg_cntrs[policy][pool].rx_cnt);
			goto HANDLE_INTR;
		}
		data_ptr = (u32)__va(desc_list->desc.desc2);
		data_offset = cqm_desc_data_off(desc_list->desc.desc3);
		dev_dbg(cqm_ctrl->dev, "data_len %d data_ptr 0x%x data_offst %d\n",
			data_len, data_ptr, data_offset);
		dma_map_single(cqm_ctrl->dev, (void *)(data_ptr + data_offset),
			       data_len, DMA_FROM_DEVICE);
		if (dma_mapping_error(cqm_ctrl->dev, desc_list->desc.desc2)) {
			dev_err(cqm_ctrl->dev, "%s DMA map failed\n", __func__);
			break;
		}

		buf_size = bm_pool_conf[pool].buf_frm_size;
		data_offset = cqm_desc_data_off(desc_list->desc.desc3);
		var_offset = data_ptr - bm_pool_conf[pool].pool_start_low;
		var_offset %= buf_size;
		real_buf_start =  data_ptr - var_offset;
		new_offset = (data_ptr - real_buf_start - CQM_POOL_METADATA);
		temp_len = data_len + new_offset + data_offset;
		real_len = SKB_DATA_ALIGN(temp_len)
		+ SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
		add_metadata((u8 *)real_buf_start, pool, policy);
		UP_STATS(cqm_dbg_cntrs[policy][pool].rx_cnt);
		skb = build_skb_cqm((u32 *)real_buf_start, real_len,
				    GFP_ATOMIC);
		if (skb) {
			skb_reserve(skb, new_offset + data_offset);
			skb->DW0 = desc_list->desc.desc0;
			skb->DW1 = desc_list->desc.desc1;
			skb->DW2 = data_ptr;
			skb->DW3 = desc_list->desc.desc3;
			skb_put(skb, data_len);
			dev_dbg(cqm_ctrl->dev, "dp_rx:head=%#x\n",
				(u32)skb->head);
			dev_dbg(cqm_ctrl->dev, "data=%#x\n", (u32)skb->data);
			dev_dbg(cqm_ctrl->dev, "tail=%#x\n", (u32)skb->tail);
			dev_dbg(cqm_ctrl->dev, "end=%#x\n", (u32)skb->end);
			if (!is_bm(data_ptr))
				dev_err(cqm_ctrl->dev, "skb 0x%p dptr %u\n"
						, skb, data_ptr);
			dp_rx(skb, 0);
			dev_dbg(cqm_ctrl->dev, "%s: rx to datapath library\n",
				__func__);
		} else {
			dev_err(cqm_ctrl->dev, "%s:failure in allocating skb\n",
				__func__);
			cqm_buffer_free(cpu, (void *)desc_list->desc.desc2, 1);
		}
	}
HANDLE_INTR:
	ls_intr_handler(cpu);
	/*make sure low level intr is handled*/
	wmb();
	cbm_w32(c_base + CQM_INT_LINE(4 + cpu, cbm_irncr), BIT(cpu) << 8);
	cbm_r32(c_base + CQM_INT_LINE(4 + cpu, cbm_irnicr));
	/*Dummy read for write flush*/
	cbm_r32(c_base + CQM_INT_LINE(4 + cpu, cbm_irncr));
	/*make sure high level intr is enabled*/
	rmb();
	dev_dbg(cqm_ctrl->dev, "0x%x\n",
		cbm_r32(c_base + CQM_INT_LINE(4 + cpu, cbm_irncr)));
	/* Enable LS */
	cbm_w32(ls_base + IRNEN_LS, cbm_r32(ls_base + IRNEN_LS) |
		(3 << (cpu * 2 + 16)));
	set_val((c_base + CQM_INT_LINE(4 + cpu, cbm_irnen)), (1 << cpu),
		((1 << cpu) << 8), 8);
}

static irqreturn_t cqm_isr(int irq, void *dev_id)
{
	int line;
	u32 cbm_intr_status = 0;
	int cpu = smp_processor_id();
	void *c_base = cqm_ctrl->cqm;
	void *ls_base = cqm_ctrl->ls;

	if (get_intr_to_line(irq, &line))
		return IRQ_NONE;

	cbm_intr_status = cbm_r32(c_base + CQM_INT_LINE(line, cbm_irncr));
	if (cbm_intr_status & (0x1 << (cpu + 8))) {
		/* Disable LS line interrupts */
		set_val((c_base + CQM_INT_LINE(line, cbm_irnen)), (0 << cpu),
			((1 << cpu) << 8), 8);
		/*Dummy Read, for write flush*/
		cbm_r32((c_base + CQM_INT_LINE(line, cbm_irnen)));
		/* Disable  LS interrupt */
		cbm_w32(ls_base + IRNEN_LS, cbm_r32(ls_base + IRNEN_LS)
			& (~(3 << (cpu * 2 + 16))));
		cbm_r32(ls_base + IRNEN_LS);
		/* Schedule the tasklet */
		tasklet_schedule(&cqm_ctrl->cqm_tasklet[cpu]);
#ifdef CPU_POOL_ALLOWED
	}
	if (cbm_intr_status & CBM_IRNCR_0_CPUPI_MASK) {
		set_val((c_base + CQM_INT_LINE(line, cbm_irnen)), 0,
			CBM_IRNCR_0_CPUPI_MASK, CBM_IRNCR_0_CPUPI_POS);
		tasklet_schedule(&cqm_ctrl->cqm_cpu_free_tasklet);
#endif
#ifdef ENABLE_LL_DEBUG
	} else if (cbm_intr_status & CBM_IRNCR_0_LLI_MASK) {
		/* Disable all interrupts */
		set_val((c_base + CQM_INT_LINE(line, cbm_irnen)), 0,
			CBM_IRNCR_0_LLI_MASK, CBM_IRNCR_0_LLI_POS);
		/*Dummy Read, for write flush*/
		cbm_r32((c_base + CQM_INT_LINE(line, cbm_irnen)));
		/* Schedule the tasklet */
		tasklet_schedule(&cbm_debug_tasklet);
#endif
	} else {
		/*spurious intr is also considered handled, to avoid
		 * kernel from disabling the cqm dqm intr
		 */
		ls_intr_handler(cpu);
		cbm_w32(c_base + CQM_INT_LINE(line, cbm_irncr),
			BIT(cpu) << 8);
	}
	return IRQ_HANDLED;
}

/*	CBM Interrupt init
 *	Register to handle all the 8 lines ,
 *	need to be adapted to work with MPE FW
 */
static int cqm_interrupt_init(void)
{
	int ret = 0;
	struct cpumask cpumask;

	ret = devm_request_irq(cqm_ctrl->dev, cqm_ctrl->cbm_irq[0], cqm_isr, 0,
			       "cbm_eqm", NULL);
	if (ret) {
		LOGF_KLOG_ERROR("Can not get IRQ - %d", ret);
		return ret;
	}
	set_intr_to_line(0, 0);

	if (cqm_ctrl->cbm_irq[1]) {
		ret = devm_request_irq(cqm_ctrl->dev, cqm_ctrl->cbm_irq[1],
				       cqm_isr, 0, "cbm_dqm", NULL);
		if (ret) {
			LOGF_KLOG_ERROR("Can not get IRQ - %d", ret);
			return ret;
		}
		/* Set the Affinity to VPE0 */
		cpumask.bits[0] = 0x1;
		ret = irq_set_affinity(cqm_ctrl->cbm_irq[1], &cpumask);
		if (ret) {
			dev_err(cqm_ctrl->dev, "Can not set affinity for IRQ - %d",
				cqm_ctrl->cbm_irq[1]);
		} else {
			set_intr_to_line(1, 4);
		}
	}

	if (cqm_ctrl->cbm_irq[2]) {
		ret = devm_request_irq(cqm_ctrl->dev, cqm_ctrl->cbm_irq[2],
				       cqm_isr, 0, "cbm_dqm", NULL);
		if (ret) {
			LOGF_KLOG_ERROR("Can not get IRQ - %d", ret);
			return ret;
		}
		/* Set the Affinity to VPE1 */
		cpumask.bits[0] = 0x2;
		ret = irq_set_affinity(cqm_ctrl->cbm_irq[2], &cpumask);
		if (ret) {
			dev_err(cqm_ctrl->dev, "Can not set affinity for IRQ - %d",
				cqm_ctrl->cbm_irq[2]);
		} else {
			set_intr_to_line(2, 5);
		}
	}

	if (cqm_ctrl->cbm_irq[3]) {
		ret = devm_request_irq(cqm_ctrl->dev, cqm_ctrl->cbm_irq[3],
				       cqm_isr, 0, "cbm_dqm", NULL);
		if (ret) {
			dev_err(cqm_ctrl->dev, "Can not get IRQ - %d", ret);
			return ret;
		}
		/* Set the Affinity to VPE2 */
		cpumask.bits[0] = 0x4;
		ret = irq_set_affinity(cqm_ctrl->cbm_irq[3], &cpumask);
		if (ret) {
			dev_err(cqm_ctrl->dev, "Can not set affinity for IRQ - %d",
				cqm_ctrl->cbm_irq[3]);
		} else {
			set_intr_to_line(3, 6);
		}
	}
	if (cqm_ctrl->cbm_irq[4]) {
		ret = devm_request_irq(cqm_ctrl->dev, cqm_ctrl->cbm_irq[4],
				       cqm_isr, 0, "cbm_dqm", NULL);
		if (ret) {
			dev_err(cqm_ctrl->dev, "Can not get IRQ - %d", ret);
			return ret;
		}
		/* Set the Affinity to VPE3 */
		cpumask.bits[0] = 0x8;
		ret = irq_set_affinity(cqm_ctrl->cbm_irq[4], &cpumask);
		if (ret) {
			dev_err(cqm_ctrl->dev,
				"Can not set affinity for IRQ - %d",
				cqm_ctrl->cbm_irq[4]);
		} else {
			set_intr_to_line(4, 7);
		}
	}

	return ret;
}

/*CBM interrupt mapping initialization*/
static void cqm_intr_mapping_init(void)
{
	void *c_base = cqm_ctrl->cqm;
	u32 reg_val = (0x1 << 8);
	/*map LS, PIB intr to line 4 and CPU pool Interrupt*/
#ifdef CPU_POOL_ALLOWED
	reg_val |= BIT(20);
#endif
#ifdef ENABLE_LL_DEBUG
	reg_val |= BIT(16);
#endif
	cbm_w32((c_base + CQM_INT_LINE(4, cbm_irnen)), reg_val);
	/*map LS */
	cbm_w32((c_base + CQM_INT_LINE(5, cbm_irnen)), 0x2 << 8);
	cbm_w32((c_base + CQM_INT_LINE(6, cbm_irnen)), 0x4 << 8);
#ifdef CONFIG_CBM_LS_ENABLE
	cbm_w32((c_base + CQM_INT_LINE(6, egp_irnen)), 0x4);
#endif
	cbm_w32((c_base + CQM_INT_LINE(7, cbm_irnen)), 0x8 << 8);
	/* Disable the EGP and IGP interrupts at top level */
	cbm_w32((c_base + CQM_INT_LINE(0, egp_irnen)), 0x0);
	cbm_w32((c_base + CQM_INT_LINE(4, egp_irnen)), 0x0);
	cbm_w32((c_base + CQM_INT_LINE(5, egp_irnen)), 0x0);
#ifndef CONFIG_CBM_LS_ENABLE
	cbm_w32((c_base + CQM_INT_LINE(6, egp_irnen)), 0x0);
#endif
}

static s32 cqm_igp_delay_set(s32 port_id, s32 delay)
{
	WARN_ON(!((port_id >= 0) && (port_id <= 15)));
	cbm_w32(cqm_ctrl->enq + EQ_CPU_PORT(port_id, dcntr),
		delay & 0x3f);
	return CBM_SUCCESS;
}

static s32 cqm_igp_delay_get(s32 port_id, s32 *delay)
{
	WARN_ON(!((port_id >= 0) && (port_id <= 15)));
	*delay = cbm_r32(cqm_ctrl->enq +
			 EQ_CPU_PORT(port_id, dcntr)) & 0x3f;
	return CBM_SUCCESS;
}

static void eqm_intr_ctrl(u32 val)
{
	int i;

	for (i = 0; i < CPU_EQM_PORT_NUM; i++)
		cbm_w32(cqm_ctrl->enq + EQ_CPU_PORT(i, irnen),
			val);
}

static void dqm_intr_ctrl(u32 val)
{
	int i;

	for (i = 0; i < CPU_DQM_PORT_NUM; i++)
		cbm_w32(cqm_ctrl->deq + DQ_CPU_PORT(i, irnen),
			val);
}

static void dump_cqem_basic_regs(u32 flag)
{
	int i;
	void *c_base = cqm_ctrl->cqm;

	dev_dbg(cqm_ctrl->dev, "dump cqem basic registers\n");
	for (i = 0; i < PRX300_MAX_INTR_LINE; i++) {
		print_reg("CBM_IRNCR\n", c_base + CQM_INT_LINE(i, cbm_irncr));
		print_reg("CBM_IRNEN\n", c_base + CQM_INT_LINE(i, cbm_irnen));
		print_reg("IGP_IRNCR\n", c_base + CQM_INT_LINE(i, igp_irncr));
		print_reg("IGP_IRNEN\n", c_base + CQM_INT_LINE(i, igp_irnen));
		print_reg("EGP_IRNCR\n", c_base + CQM_INT_LINE(i, egp_irncr));
		print_reg("EGP_IRNEN\n", c_base + CQM_INT_LINE(i, egp_irnen));
	}
	print_reg("CQEM SRAM BASE\n", (u32 *)(c_base + CBM_PB_BASE));
	print_reg("CQEM STAT\n", (u32 *)(c_base + CBM_CTRL));
	print_reg("CQEM STAT\n", (u32 *)(c_base + CBM_BUF_SIZE));
	print_reg("CQEM STAT\n", (u32 *)(c_base + CBM_BSL_CTRL));
	print_reg("CQEM STATn", (u32 *)(c_base + CBM_BM_BASE));
	print_reg("CQEM STAT\n", (u32 *)(c_base + CBM_WRED_BASE));
	print_reg("CQEM STAT\n", (u32 *)(c_base + CBM_QPUSH_BASE));
	print_reg("CQEM STAT\n", (u32 *)(c_base + CBM_TX_CREDIT_BASE));
	print_reg("CQEM STAT\n",
		  (u32 *)(c_base + CBM_CPU_POOL_BUF_RTRN_START_ADDR));
	print_reg("CQEM STAT\n",
		  (u32 *)(c_base + CBM_CPU_POOL_BUF_ALW_NUM));
	print_reg("CQEM STAT\n", (u32 *)(c_base + CBM_VERSION_REG));
}

static int init_cqm_basic(struct platform_device *pdev)
{
	int tmp_v;
	u32 version;
	int size;
	void *c_base = cqm_ctrl->cqm;

	dev_dbg(cqm_ctrl->dev, "[%s]Line %d\n", __func__, __LINE__);
	#ifdef ENABLE_LL_DEBUG
	set_val((c_base + CBM_CTRL), 1, CBM_CTRL_LL_DBG_MASK,
		CBM_CTRL_LL_DBG_POS);
	#endif
	/* base address of internal packet buffer managed by the FSQM */
	cbm_w32(c_base + CBM_PB_BASE, CQM_SRAM_BASE);

	/*Program the PBS bit to Select between
	 *Internal Packet Buffer frame size of 128 Bytes or 2 KBytes
	 */
	set_val((c_base + CBM_CTRL), CQM_SRAM_FRM_SIZE_BIT, CBM_CTRL_PBSEL_MASK,
		CBM_CTRL_PBSEL_POS);

	/*Enable trigger for ACA ports from EQM*/
	set_val((c_base + CBM_CTRL), 3, CBM_CTRL_ACA_EP0_EN_MASK,
		CBM_CTRL_ACA_EP0_EN_POS);
	set_val((c_base + CBM_CTRL), 3, CBM_CTRL_ACA_EP1_EN_MASK,
		CBM_CTRL_ACA_EP1_EN_POS);
	set_val((c_base + CBM_CTRL), 3, CBM_CTRL_ACA_EP2_EN_MASK,
		CBM_CTRL_ACA_EP2_EN_POS);

	cqm_intr_mapping_init();
	cqm_interrupt_init();
	/*Config the ex pool buff size*/
	cbm_w32((c_base + CBM_BUF_SIZE),
		(bm_pool_conf[3].buf_frm_size_reg << CBM_BUF_SIZE_SIZE3_POS) |
		(bm_pool_conf[2].buf_frm_size_reg << CBM_BUF_SIZE_SIZE2_POS) |
		(bm_pool_conf[1].buf_frm_size_reg << CBM_BUF_SIZE_SIZE1_POS) |
		(bm_pool_conf[0].buf_frm_size_reg << CBM_BUF_SIZE_SIZE0_POS));

	/*BSL config*/
	cbm_w32((c_base + CBM_BSL_CTRL),
		(1 << CBM_BSL_CTRL_BSL2_EN_POS) |
		(1 << CBM_BSL_CTRL_BSL1_EN_POS));

	cbm_w32((c_base + CBM_BM_BASE), 0x003c0000);
	cbm_w32((c_base + CBM_WRED_BASE), 0x00090000);
	cbm_w32((c_base + CBM_QPUSH_BASE), 0x00260000);
	cbm_w32((c_base + CBM_TX_CREDIT_BASE), 0x00200800);
	size = (CQM_CPU_POOL_BUF_ALW_NUM + 4) * sizeof(u32);
	tmp_v = TOT_DMA_HNDL - 1;
#ifdef CPU_POOL_ALLOWED
	cqm_ctrl->cpu_rtn_ptr = dma_alloc_noncoherent(&pdev->dev, size,
						&cqm_ctrl->dma_hndl_p[tmp_v],
						GFP_KERNEL);
	dev_info(cqm_ctrl->dev, "cpu rtn buf 0x%p\n", cqm_ctrl->cpu_rtn_ptr);
	if (!cqm_ctrl->cpu_rtn_ptr) {
		dev_err(cqm_ctrl->dev, "Error in cpu_rtn_ptr allocation\n");
		return CBM_FAILURE;
	}
	cqm_ctrl->cpu_rtn_aligned = (u32 *)ALIGN((u32)
						  cqm_ctrl->cpu_rtn_ptr,
						  32);
	dev_info(cqm_ctrl->dev, "cpu_rtn_aligned 0x%p\n",
		 cqm_ctrl->cpu_rtn_aligned);
	addr = dma_map_single(cqm_ctrl->dev, (void *)cqm_ctrl->cpu_rtn_aligned,
			      CQM_CPU_POOL_BUF_ALW_NUM * sizeof(u32),
			      DMA_TO_DEVICE);
	if (dma_mapping_error(cqm_ctrl->dev, addr)) {
		dev_err(cqm_ctrl->dev, "%s DMA map failed\n", __func__);
		return CBM_FAILURE;
	}
	cbm_w32((c_base + CBM_CPU_POOL_BUF_RTRN_START_ADDR),
		addr);
	cbm_w32((c_base + CBM_CPU_POOL_BUF_ALW_NUM),
		CQM_CPU_POOL_BUF_ALW_NUM);
#endif
	version = cbm_r32(c_base + CBM_VERSION_REG);
	dev_dbg(cqm_ctrl->dev,
		"Basic init of CQM successful ver Major: %u Minor: %u\n",
		(version & 0xf), ((version >> 8) & 0xf));
	dump_cqem_basic_regs(0);
	return CBM_SUCCESS;
}

static int get_bufsize(int size)
{
	switch (size) {
	case 128:
		return 0;
	case 256:
		return 1;
	case 512:
		return 2;
	case 1024:
		return 3;
	case 2048:
		return 4;
	case 4096:
		return 5;
	case 8192:
		return 6;
	case 10240:
		return 7;
	};
	return 0;
}

static int cqm_get_dc_config(struct cbm_dc_res *dc_res, int flag)
{
	struct cqm_dqm_port_info *dqp_info = dqm_port_info;
	struct cqm_res_t *res = NULL;
	u32 num, cbm_port;

	if (dc_res->alloc_flags & FLAG_ACA) {
		if ((dp_port_resources_get(&dc_res->dp_port, &num, &res,
					   0) != 0) && (!res)) {
			dev_err(cqm_ctrl->dev,
				"Error getting resources for port_id %d\n",
				dc_res->dp_port);
			return CBM_FAILURE;
		}
		cbm_port = res[0].cqm_deq_port;
		dc_res->rx_res.rx_bufs = 0x40;
		dc_res->tx_res.tx_bufs = 0x40;
		dc_res->tx_res.in_deq_ring_size =
			dqp_info[cbm_port].deq_info.num_desc;
		dc_res->tx_res.out_free_ring_size =
			dqp_info[cbm_port].deq_info.num_free_burst;
	}
	return CBM_SUCCESS;
}

static int pool_config(struct platform_device *pdev, int p, u32 buf_size,
		       u32 buf_num)
{
	struct cqm_bm_pool_config *p_ptr;
	u32 size;
	u32 adj_size;

	size = buf_size * buf_num;
	cqm_ctrl->bm_buf_base[p] = dma_alloc_noncoherent(&pdev->dev, size,
						&cqm_ctrl->dma_hndl_p[p],
						GFP_KERNEL);
	dev_dbg(cqm_ctrl->dev, "qos bm buf 0x%p\n", cqm_ctrl->bm_buf_base[p]);
	if (!cqm_ctrl->bm_buf_base[p]) {
		dev_err(cqm_ctrl->dev, "Error in pool %d buffer allocation\n",
			p);
		return CBM_FAILURE;
	}
	p_ptr = &bm_pool_conf[p];
	p_ptr->buf_frm_size = buf_size;
	p_ptr->buf_frm_num = buf_num;
	buf_addr_adjust((u32)cqm_ctrl->bm_buf_base[p],
			size,
			cqm_ctrl->dma_hndl_p[p],
			&p_ptr->pool_start_low,
			&adj_size,
			&p_ptr->pool_dma,
			p_ptr->buf_frm_size);
	p_ptr->pool_end_low = p_ptr->pool_start_low + adj_size;
	p_ptr->buf_frm_size_reg = get_bufsize(buf_size);
	return CBM_SUCCESS;
}

static int bm_init(struct platform_device *pdev)
{
	struct pp_bmgr_pool_params p_params;
	u8 i, j;
	int result;
	u8  req_pool_id = PP_BM_INVALID_POOL_ID;
	u16 req_policy_id = PP_BM_INVALID_POLICY_ID;
	s32 ret;

	ret = pp_bmgr_set_total_active_pools(cqm_ctrl->num_pools);
	if (ret)
		dev_err(cqm_ctrl->dev, "Set active pools failed\n");

	for (i = 0; i < cqm_ctrl->num_pools; i++) {
		result = pool_config(pdev, i, cqm_ctrl->prx300_pool_size[i],
				     cqm_ctrl->prx300_pool_ptrs[i]);
		if (result)
			panic("pool %d allocation failed\n", i);
	}

	for (i = 0; i < cqm_ctrl->num_pools; i++) {
		p_params.num_buffers = bm_pool_conf[i].buf_frm_num;
		p_params.size_of_buffer = bm_pool_conf[i].buf_frm_size;
		p_params.base_addr_low = bm_pool_conf[i].pool_dma;
		p_params.base_addr_high = 0;
		p_params.flags = POOL_ENABLE_FOR_MIN_GRNT_POLICY_CALC;
		req_pool_id = PP_BM_INVALID_POOL_ID;
		ret = pp_bmgr_pool_configure(&p_params, &req_pool_id);
		if (ret)
			dev_err(cqm_ctrl->dev, "Pool %u conf failed\n", i);
	}

	for (j = 0; j < cqm_ctrl->num_pools; j++) {
		req_policy_id = PP_BM_INVALID_POLICY_ID;
		ret = pp_bmgr_policy_configure(&p_param[j], &req_policy_id);
		if (ret)
			dev_err(cqm_ctrl->dev, "Pool %u conf failed\n", j);
	}

	return CBM_SUCCESS;
}

static int cbm_hw_init(struct platform_device *pdev)
{
	init_fsqm();
	if (init_cqm_basic(pdev))
		return CBM_FAILURE;
	dev_dbg(cqm_ctrl->dev, "CBM HW init\n");
	return CBM_SUCCESS;
}

static int setup_enq_dma_desc(int pid, u32 desc_num, s32 type,
			      u32 data_offset)
{
	struct dma_desc desc;
	u32 buf;
	u32 reg1;
	u32 reg2;
	int i;
	u32 cpu = smp_processor_id();
	struct cqm_bm_pool_config tmp_bm = {0};
	void *dmadesc = cqm_ctrl->dmadesc;

	cbm_assert(pid >= 4, "cbm dma eqm port id is less than 4,pid:%d ", pid);

	dev_dbg(cqm_ctrl->dev, "%s:pid:%d,desc_num:%d, flag: %d, offset:%d\n",
		__func__, pid, desc_num, type, data_offset);
	get_bm_info(&tmp_bm, type);
	for (i = 0; i < desc_num; i++) {
		if (tmp_bm.policy == CQM_FSQM_POLICY) {
			buf = (u32)cqm_fsqm_buff_alloc(cpu, CQM_FSQM_POLICY);
			data_offset = 0;
		} else {
			buf = (u32)cqm_buff_alloc_by_policy_prx300(cpu, type,
								tmp_bm.policy);
			if (!buf) {
				dev_err(cqm_ctrl->dev, "Cannot alloc buffer from CBM!!!\n");
				return CBM_FAILURE;
			}
			buf = __pa(buf - CQM_POOL_METADATA);
		}
		cbm_dw_memset((u32 *)&desc, 0, sizeof(struct dma_desc) /
			      sizeof(u32));
		reg1 = 0;
		reg2 = 0;
		reg2 = tmp_bm.buf_frm_size - data_offset;
		dev_dbg(cqm_ctrl->dev, "data_offset %d %d %d %d\n",
			CBM_DMA_DATA_OFFSET, NET_IP_ALIGN, NET_SKB_PAD,
			CQM_POOL_METADATA);
		dev_dbg(cqm_ctrl->dev, "reg1 0x%x reg20x%x buf 0x%x\n", reg1,
			reg2, buf);
		reg1 = (buf + data_offset) & ~0x7;
		reg2 |= ((buf + data_offset) & 0x7) << 23;
		reg2 |= (CBM_DMA_DESC_OWN << 31);
		reg2 |= tmp_bm.pool << 16;
		reg2 |= tmp_bm.policy << 20;

		dev_dbg(cqm_ctrl->dev, "reg1 0x%x reg2 0x%x buf 0x%x\n", reg1,
			reg2, buf);
		dev_dbg(cqm_ctrl->dev, "addr 0x%x\n",
			(u32)dmadesc + CQM_ENQ_DMA_DESC(pid, i));
		/*adjust the flag*/
		#ifdef CONFIG_CPU_LITTLE_ENDIAN
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0x0, 0);
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0x4, 0);
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0x8,
			reg1);
		/*make sure desc0, desc1, desc2 are written*/
		wmb();
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0xc,
			reg2);
		/*make sure desc3 own written*/
		wmb();
		#else
		#ifdef CONFIG_XBAR_LE
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0x0, 0);
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0x4, 0);
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0x8,
			reg1);
		/*make sure desc0, desc1, desc2 are written*/
		wmb();
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0xc,
			reg2);
		/*make sure desc3 own written*/
		wmb();
		#else
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0x4, 0);
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0x0, 0);
		dev_dbg(cqm_ctrl->dev, "addr 0x%x\n",
			dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0xc);
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0xc,
			reg1);
		/*make sure desc0, desc1, desc2 are written*/
		wmb();
		cbm_w32(dmadesc + CQM_ENQ_DMA_DESC(pid, i) + 0x8,
			reg2);
		/*make sure desc3 own written*/
		wmb();
		#endif
		#endif
		UP_STATS(cqm_dbg_cntrs[tmp_bm.policy]
			 [tmp_bm.pool].dma_ring_buff_cnt);
	}
	return CBM_SUCCESS;
}

static void init_cqm_enq_dma_port(int idx, s32 type)
{
	void *enq = cqm_ctrl->enq;

	cbm_w32(enq + EQ_DMA_PORT(idx, dptr),
		eqm_port_info[idx].eq_info.num_desc - 1);
	cbm_w32(enq + IP_OCC_0 + ((idx) * 4), 0);
	dev_dbg(cqm_ctrl->dev, "0x%x\n", (int)enq + IP_OCC_0 + ((idx) * 4));

	cbm_w32(enq + IP_THRESHOLD_HWM_0 + ((idx) * 4), DEFAULT_DMA_HWM);
	dev_dbg(cqm_ctrl->dev, "0x%x\n",
		(int)enq + IP_THRESHOLD_HWM_0 + ((idx) * 4));

	cbm_w32(enq + IP_THRESHOLD_LWM_0 + ((idx) * 4),
		((DEFAULT_DMA_HWM / 3) * 2));
	dev_dbg(cqm_ctrl->dev, "0x%x\n",
		(int)enq + IP_THRESHOLD_LWM_0 + ((idx) * 4));

	setup_enq_dma_desc(idx, eqm_port_info[idx].eq_info.num_desc, type,
			   CBM_DMA_DATA_OFFSET +
			   NET_IP_ALIGN +
			   NET_SKB_PAD +
			   CQM_POOL_METADATA);
	cbm_w32(enq + EQ_DMA_PORT(idx, cfg),
		CFG_DMA_IGP_4_EQREQ_MASK |
		CFG_DMA_IGP_4_EQPCEN_MASK);

	dev_dbg(cqm_ctrl->dev, "0x%x\n", (int)enq + EQ_DMA_PORT(idx, cfg));

	dev_dbg(cqm_ctrl->dev, "0x%x\n", CFG_DMA_IGP_4_EQREQ_MASK |
		  CFG_DMA_IGP_4_EQPCEN_MASK);
}

static void setup_deq_dma_desc(u32 pid, u32 desc_num)
{
	int i, type;
	void *dma_desc = cqm_ctrl->dmadesc;

	cbm_assert(pid >= DQM_ACA_START_ID,
		   "cbm dma dqm port id less than %d, pid:%d",
		   DQM_ACA_START_ID, pid);
	type = find_dqm_port_type(pid);

	if (type != DQM_PON_TYPE) {
		for (i = 0; i < desc_num; i++) {
			cbm_w32(dma_desc + CQM_DEQ_DMA_DESC(pid, i)
				+ 0x0, 0);
			cbm_w32(dma_desc + CQM_DEQ_DMA_DESC(pid, i)
				+ 0x4, 0);
			cbm_w32(dma_desc + CQM_DEQ_DMA_DESC(pid, i)
				+ 0x8, 0);
			cbm_w32(dma_desc + CQM_DEQ_DMA_DESC(pid, i)
				+ 0xc, 0);
		}
	} else {
		for (i = 0; i < desc_num; i++) {
			cbm_w32(dma_desc + CQM_DEQ_PON_DMA_DESC(pid, i)
				+ 0x0, 0);
			cbm_w32(dma_desc + CQM_DEQ_PON_DMA_DESC(pid, i)
				+ 0x4, 0);
			cbm_w32(dma_desc + CQM_DEQ_PON_DMA_DESC(pid, i)
				+ 0x8, 0);
			cbm_w32(dma_desc + CQM_DEQ_PON_DMA_DESC(pid, i)
				+ 0xc, 0);
		}
	}
}

static void init_cqm_deq_dma_port(int dqp_idx, int dmap_idx)
{
	void *deq = cqm_ctrl->deq;
	int type = find_dqm_port_type(dqp_idx);

	setup_deq_dma_desc(dqp_idx, dqm_port_info[dqp_idx].deq_info.num_desc);
	if (type != DQM_PON_TYPE) {
		/* disable auto read increment, re-en after deq_req */
		cbm_w32((deq + DQ_DMA_PORT(dmap_idx, cfg)),
			0x02 |
			CFG_DMA_EGP_7_DQPCEN_MASK |
			((dqp_idx << CFG_DMA_EGP_7_EPMAP_POS) &
			CFG_DMA_EGP_7_EPMAP_MASK));
		set_val((cqm_ctrl->deq + DQ_DMA_PORT((dmap_idx), cfg)),
			1, 0x1, 0);
		/* re-enable auto read increment here*/
		set_val((cqm_ctrl->deq + DQ_DMA_PORT((dmap_idx), cfg)),
			0, 0x2, 1);
		dev_dbg(cqm_ctrl->dev, " %s 0x%x 0x%x\n dmap_idx %d, dqp_idx %d\n",
			__func__,
			(int)(deq + DQ_DMA_PORT(dmap_idx, cfg)),
			0x02 |
			CFG_DMA_EGP_7_DQPCEN_MASK |
			((dqp_idx << CFG_DMA_EGP_7_EPMAP_POS) &
			CFG_DMA_EGP_7_EPMAP_MASK), dmap_idx, dqp_idx);
	} else if (type == DQM_PON_TYPE) {
		cbm_w32((deq + DQ_PON_PORT(dmap_idx, cfg)),
			CFG_PON_EGP_26_DQPCEN_MASK |
			CFG_PON_EGP_26_DQREQ_MASK |
			((dqp_idx << CFG_PON_EGP_26_EPMAP_POS) &
			CFG_PON_EGP_26_EPMAP_MASK));
		dev_dbg(cqm_ctrl->dev, " %s PON 0x%x 0x%x, dmap_idx %d, dqp_idx %d\n",
			__func__,
			(int)(deq + DQ_PON_PORT(dmap_idx, cfg)),
			CFG_PON_EGP_26_DQPCEN_MASK |
			CFG_PON_EGP_26_DQREQ_MASK |
			((dqp_idx << CFG_PON_EGP_26_EPMAP_POS) &
			CFG_PON_EGP_26_EPMAP_MASK), dmap_idx, dqp_idx);
	}
}

static s32 cqm_dma_port_enable(s32 port_id, u32 flags, s32 type)
{
	int port_type = find_dqm_port_type(port_id);

	dev_dbg(cqm_ctrl->dev, "%s %d 0x%x\n", __func__, port_id, flags);
	if (flags & CBM_PORT_F_DEQUEUE_PORT) {
		/*DMA dequeue port*/
		if (flags & CBM_PORT_F_DISABLE) {
			if (port_type != DQM_PON_TYPE) {
				set_val((cqm_ctrl->deq
					+ DQ_DMA_PORT((port_id), cfg)),
					0, 0x1, 0);
				cbm_w32((cqm_ctrl->deq
					+ DQ_DMA_PORT(port_id, dqpc)), 0);
			} else if (port_type == DQM_PON_TYPE) {
				cbm_w32((cqm_ctrl->deq
					+ DQ_PON_PORT(port_id, cfg)), 0);
			}
		} else {
			init_cqm_deq_dma_port(port_id, port_id);
		}
	} else {
		/*DMA Enqueue port*/
		if (flags & CBM_PORT_F_DISABLE)
			cbm_w32((cqm_ctrl->enq + EQ_DMA_PORT(port_id, cfg)), 0);
		else
			init_cqm_enq_dma_port(port_id, type);
	}
	return CBM_SUCCESS;
}

static void init_cqm_deq_cpu_port(int idx, u32 tx_ring_size)
{
	u32 config = CFG_CPU_EGP_0_DQREQ_MASK |
		     CFG_CPU_EGP_0_BUFRTN_MASK |
		    CFG_CPU_EGP_0_BFBPEN_MASK |
		    CFG_CPU_EGP_0_DQPCEN_MASK;
	void *deq = cqm_ctrl->deq;
	u32 type = find_dqm_port_type(idx);

	if ((tx_ring_size) && (type == DQM_CPU_TYPE)) {
		cbm_w32((deq + DQ_CPU_PORT(idx, dptr)), tx_ring_size - 1);
		dev_dbg(cqm_ctrl->dev, "%s port:%d,val:%d\n",
			__func__, idx, tx_ring_size - 1);
	} else if (type == DQM_ACA_TYPE) {
		cbm_w32((deq + DQ_CPU_PORT(idx, dptr)),
			dqm_port_info[idx].deq_info.num_desc - 1);
		dev_dbg(cqm_ctrl->dev, "%s port:%d,val:%d\n", __func__,
			idx, dqm_port_info[idx].deq_info.num_desc - 1);
	}

	config |= cbm_r32(deq + DQ_CPU_PORT(idx, cfg));
		/*!< preserve complete data changes */
	config |= ((idx << CFG_CPU_EGP_0_EPMAP_POS) & CFG_CPU_EGP_0_EPMAP_MASK);
	dev_dbg(cqm_ctrl->dev, "%d\n", idx);
	cbm_w32((deq + DQ_CPU_PORT(idx, cfg)), config);
	dev_dbg(cqm_ctrl->dev, "%s port:%d,addr:0x%x, cfg:0x%x tx_ring_size:%d\n",
		__func__, idx, (u32)(deq + DQ_CPU_PORT(idx, cfg)),
		config, tx_ring_size);
#ifdef CONFIG_CBM_LS_ENABLE
	cbm_w32((deq + DQ_CPU_PORT(2, irnen)), 0x2);
#endif
}

static void init_cqm_enq_cpu_port(int idx)
{
	void *enq = cqm_ctrl->enq;

	cbm_w32((enq + EQ_CPU_PORT(idx, cfg)),
		CFG_CPU_IGP_0_EQREQ_MASK |
		CFG_CPU_IGP_0_BUFREQ0_MASK |
		CFG_CPU_IGP_0_BUFREQ1_MASK |
		CFG_CPU_IGP_0_EQPCEN_MASK |
		CFG_CPU_IGP_0_BP_EN_MASK |
		CFG_CPU_IGP_0_BUFREQ2_MASK |
		CFG_CPU_IGP_0_BUFREQ3_MASK |
		CFG_CPU_IGP_0_BUFREQ4_MASK);

	cbm_w32((enq + IP_OCC_0 + (idx * 4)), 0);
	dev_dbg(cqm_ctrl->dev, "0x%x\n", (int)(enq + IP_OCC_0 + (idx * 4)));

	cbm_w32((enq + IP_THRESHOLD_HWM_0 + (idx * 4)),
		DEFAULT_CPU_HWM);
	dev_dbg(cqm_ctrl->dev, "0x%x\n",
		(int)(enq + IP_THRESHOLD_HWM_0 + (idx * 4)));

	cbm_w32((enq + IP_THRESHOLD_LWM_0 + (idx * 4)),
		((DEFAULT_CPU_HWM / 3) * 2));

	dev_dbg(cqm_ctrl->dev, "0x%x\n",
		(int)(enq + IP_THRESHOLD_LWM_0 + (idx * 4)));
	cbm_w32((enq + EQ_CPU_PORT(idx, irnen)), 0x3F);

	dev_dbg(cqm_ctrl->dev, "0x%x\n",
		(int)(enq + EQ_CPU_PORT(idx, irnen)));
}

static s32 setup_DMA_channel(int chan, int cqm_port, dma_addr_t desc_base,
			     int buf_type, int desc_num, char *dev_id)
{
	if (ltq_request_dma(chan, dev_id) < 0) {
		dev_err(cqm_ctrl->dev, "%s failed to open chan for chan%d\r\n",
			__func__, chan);
		return CBM_FAILURE;
	}

	if ((ltq_dma_chan_desc_cfg(chan, desc_base, desc_num)) < 0) {
		dev_err(cqm_ctrl->dev, "%s failed to setup chan desc for chan%d\r\n",
			__func__, chan);
		return CBM_FAILURE;
	}
	if (ltq_dma_chan_irq_disable(chan) < 0) {
		dev_err(cqm_ctrl->dev, "%s failed to disable IRQ for chan%d\r\n",
			__func__, chan);
		return CBM_FAILURE;
	}
	ltq_dma_chan_polling_cfg(chan, 6, 6);
	/* Channel on */
	if (ltq_dma_chan_on(chan) < 0) {
		dev_err(cqm_ctrl->dev, " %s failed to ON chan%d\r\n",
			__func__, chan);
		return CBM_FAILURE;
	}
	dev_dbg(cqm_ctrl->dev, "%s executed\n", __func__);
	return CBM_SUCCESS;
}

static s32 cqm_enqueue_dma_port_init(s32 cqm_port_id, s32 dma_hw_num,
				     u32 chan_num, u32 flags)
{
	int dma_controller, chan;
	unsigned char dma_ctrl[DMA_CH_STR_LEN];
	dma_addr_t desc_base;
	struct cqm_eqm_port_info *p_info;

	p_info = &eqm_port_info[cqm_port_id];
	desc_base = (dma_addr_t)cqm_ctrl->dmadesc_phys +
		     CQM_ENQ_DMA_DESC(cqm_port_id, 0);
	dev_dbg(cqm_ctrl->dev, "%d 0x%x %d %d\n", cqm_port_id, flags,
		dma_hw_num, chan_num);
		/*config the dma channel*/
	snprintf(dma_ctrl, DMA_CH_STR_LEN, "DMA%dRX", dma_hw_num);
	dma_controller = cqm_dma_get_controller(dma_ctrl);
	chan = _DMA_C(dma_controller, 0, chan_num);
	p_info->eq_info.dma_rx_chan_std = chan;
	snprintf(p_info->dma_chan_str, DMA_CH_STR_LEN, "port%d", cqm_port_id);
	if (setup_DMA_channel(chan, cqm_port_id, desc_base, flags,
			      p_info->eq_info.num_desc,
			      p_info->dma_chan_str))
		return CBM_FAILURE;
	return CBM_SUCCESS;
}

static s32 cqm_dequeue_dma_port_uninit(s32 cqm_port_id, u32 flags)
{
	int chan;

	chan = dqm_port_info[cqm_port_id].dma_ch;
	if (chan && dqm_port_info[cqm_port_id].dma_ch_in_use) {
		if (ltq_dma_chan_off(chan))
			return CBM_FAILURE;
		if (ltq_free_dma(chan))
			return CBM_FAILURE;
	}
	return CBM_SUCCESS;
}

static s32 cqm_dq_dma_chan_init(s32 cqm_port_id, u32 flags)
{
	s32 ret = CBM_SUCCESS;
	dma_addr_t desc_base;
	struct cqm_dqm_port_info *p_info;

	if (cqm_port_id < DQM_PON_START_ID) {
		desc_base = (dma_addr_t)cqm_ctrl->dmadesc_phys +
			CQM_DEQ_DMA_DESC(cqm_port_id, 0);
	} else {
		/* We have only one DMA for 64 PON ports */
		cqm_port_id = DQM_PON_START_ID;
		desc_base = (dma_addr_t)cqm_ctrl->dmadesc_phys +
			CQM_DEQ_PON_DMA_DESC(cqm_port_id, 0);
	}
	/*config the dma channel*/
	p_info = &dqm_port_info[cqm_port_id];
	snprintf(p_info->dma_chan_str, DMA_CH_STR_LEN, "port%d", cqm_port_id);
	ret = setup_DMA_channel(p_info->dma_ch, cqm_port_id, desc_base, flags,
				p_info->deq_info.num_desc,
				p_info->dma_chan_str);
	return ret;
}

static int conf_deq_aca_port(const struct dqm_aca_port *aca_ptr)
{
	u32 port, flags = 0;
	struct cqm_pmac_port_map local_entry = {0};
	struct cqm_dqm_port_info *p_info;
	void *deq = cqm_ctrl->deq;

	port = aca_ptr->port;
	p_info = &dqm_port_info[port];
	memset(&local_entry, 0, sizeof(local_entry));
	p_info->deq_info.cbm_dq_port_base = deq + DQ_SCPU_PORT(port, desc);
	p_info->deq_info.num_desc = aca_ptr->num_desc;
	p_info->deq_info.num_free_burst = aca_ptr->num_free_burst;
	p_info->deq_info.port_no = port;
	p_info->deq_info.dma_tx_chan = 255;
	p_info->cbm_buf_free_base = deq + DQ_SCPU_PORT(port, scpu_ptr_rtn);
	p_info->num_free_entries = (port > 3) ? 32 : 1;
	p_info->dq_txpush_num = aca_ptr->txpush_desc;
	p_info->dma_dt_ch = aca_ptr->dma_chan;
	p_info->dma_dt_ctrl = aca_ptr->dma_ctrl;
	p_info->dma_dt_init_type = DEQ_DMA_CHNL;
	p_info->dma_ch_in_use = false;
	p_info->valid = 1;

	dev_dbg(cqm_ctrl->dev, "0x%p %d %d %d 0x%p %d %u %u %u\n",
		p_info->deq_info.cbm_dq_port_base,
		p_info->deq_info.num_desc,
		p_info->deq_info.port_no,
		p_info->deq_info.dma_tx_chan,
		p_info->cbm_buf_free_base,
		p_info->num_free_entries,
		p_info->dma_dt_ch,
		p_info->dma_dt_ctrl,
		p_info->dma_dt_init_type);
	/*config cbm/dma port*/
	if (port >= 4) {
		flags = DP_F_FAST_WLAN;
		goto ASSIGN_FLAGS;
	}

	local_entry.egp_port_map[0] |= BIT(port);
	local_entry.owner = 0;
	local_entry.dev = 0;
	local_entry.dev_port = 0;
	local_entry.flags = P_ALLOCATED;
	cqm_add_to_list(&local_entry);
ASSIGN_FLAGS:
	p_info->egp_type = flags;
	aca_ptr = NULL;
	return CBM_SUCCESS;
}

static int conf_deq_cpu_port(const struct dqm_cpu_port *cpu_ptr)
{
	u32 port, flags = 0;
	struct cqm_pmac_port_map local_entry = {0};
	struct cqm_dqm_port_info *p_info;
	void *deq = cqm_ctrl->deq;

	port = cpu_ptr->port;
	p_info = &dqm_port_info[port];
	memset(&local_entry, 0, sizeof(local_entry));
	p_info->deq_info.cbm_dq_port_base = deq + DQ_CPU_PORT(port, desc0);
	p_info->deq_info.num_desc = cpu_ptr->num_desc;
	p_info->deq_info.port_no = port;
	p_info->deq_info.dma_tx_chan = 255;
	p_info->cbm_buf_free_base = deq + DQ_CPU_PORT(port, ptr_rtn_dw2);
	p_info->num_free_entries = (port > 3) ? 32 : 1;
	p_info->cpu_port_type = cpu_ptr->cpu_port_type;
	p_info->dq_txpush_num = cpu_ptr->txpush_desc;
	p_info->valid = 1;

	dev_dbg(cqm_ctrl->dev, "0x%p %d %d %d 0x%p %d\n",
		p_info->deq_info.cbm_dq_port_base,
		p_info->deq_info.num_desc,
		p_info->deq_info.port_no,
		p_info->deq_info.dma_tx_chan,
		p_info->cbm_buf_free_base,
		p_info->num_free_entries);
	/*config cbm/dma port*/
	if (port >= 4) {
		flags = DP_F_FAST_WLAN;
		goto ASSIGN_FLAGS;
	}

	local_entry.egp_port_map[0] |= BIT(port);
	local_entry.owner = 0;
	local_entry.dev = 0;
	local_entry.dev_port = 0;
	local_entry.flags = P_ALLOCATED;
	cqm_add_to_list(&local_entry);
ASSIGN_FLAGS:
	p_info->egp_type = flags;
	cpu_ptr = NULL;
	return CBM_SUCCESS;
}

static int conf_deq_dma_port(const struct dqm_dma_port *dma_ptr)
{
	u32 port, range, flags = 0, j, index;
	struct cqm_dqm_port_info *p_info;
	void *dma_base;
	void *dmadesc = cqm_ctrl->dmadesc;

	port = dma_ptr->port;
	range = dma_ptr->port_range + 1;
	j = 0;
	while (j < range) {
		index = port + j;
		dev_dbg(cqm_ctrl->dev, "index %d\n", index);
		p_info = &dqm_port_info[index];
		if (index < DQM_PON_START_ID)
			dma_base = dmadesc + CQM_DEQ_DMA_DESC(index, 0);
		else
			dma_base = dmadesc + CQM_DEQ_PON_DMA_DESC(index, 0);
		p_info->deq_info.cbm_dq_port_base = dma_base;
		p_info->deq_info.num_desc = dma_ptr->num_desc;
		p_info->deq_info.port_no = index;
		p_info->deq_info.dma_tx_chan = dma_ptr->dma_chan + j;
		p_info->cbm_buf_free_base = NULL;
		p_info->num_free_entries = 0;
		p_info->dq_txpush_num = dma_ptr->txpush_desc;
		if (!get_matching_flag(&flags, index))
			p_info->egp_type = flags;
		p_info->dma_dt_ch = dma_ptr->dma_chan + j;
		p_info->dma_dt_ctrl = dma_ptr->dma_ctrl;
		if (index == DMA_PORT_FOR_FLUSH)
			p_info->dma_dt_init_type = DEQ_DMA_CHNL_NOT_APPL;
		else
			p_info->dma_dt_init_type = DEQ_DMA_CHNL;
		p_info->dma_ch_in_use = false;
		p_info->valid = 1;
		dev_dbg(cqm_ctrl->dev, "0x%x %d %d %d 0x%x %d %d %d\n",
			(u32)p_info->deq_info.cbm_dq_port_base,
			p_info->deq_info.num_desc,
			p_info->deq_info.port_no,
			p_info->deq_info.dma_tx_chan,
			p_info->egp_type,
			p_info->dma_dt_ch,
			p_info->dma_dt_ctrl,
			p_info->dma_dt_init_type);
		j++;
	}
	return CBM_SUCCESS;
}

static int conf_enq_dma_port(const struct eqm_dma_port *dma_ptr)
{
	u32 dma_chnl, j, index;
	u32 port = dma_ptr->port;
	u32 range = dma_ptr->port_range + 1;
	struct cqm_eqm_port_info *p_info;
	void *dmadesc = cqm_ctrl->dmadesc;

	j = 0;
	if ((cqm_ctrl->num_pools + 1) < range)
		range = cqm_ctrl->num_pools + 1;
	while (j < range) {
		index = port + j;
		p_info = &eqm_port_info[index];
		dma_chnl = dma_ptr->dma_chnl;
		p_info->num_eq_ports = 1;
		p_info->eq_info.cbm_eq_port_base = dmadesc +
						   CQM_ENQ_DMA_DESC(index, 0);
		p_info->eq_info.port_no = index;
		p_info->port_type = dma_ptr->port_type;
		p_info->eq_info.num_desc = dma_ptr->num_desc;
		p_info->dma_dt_ch = dma_chnl + j;
		p_info->dma_dt_ctrl = dma_ptr->dma_ctrl;
		p_info->dma_dt_init_type = ENQ_DMA_FSQM_CHNL + j;
		p_info->valid = 1;
		dev_dbg(cqm_ctrl->dev, "0x%x %d %d %d %d %d %d\n",
			(u32)p_info->eq_info.cbm_eq_port_base,
			p_info->eq_info.num_desc,
			p_info->eq_info.port_no,
			p_info->port_type,
			p_info->dma_dt_ch,
			p_info->dma_dt_ctrl,
			p_info->dma_dt_init_type);
		cqm_dma_port_enable(index, 0, REQ_FSQM_BUF + j);
		j++;
	}
	return CBM_SUCCESS;
}

static int conf_enq_cpu_port(const struct eqm_cpu_port *cpu_ptr)
{
	u32 port = cpu_ptr->port;
	struct cqm_eqm_port_info *p_info;
	void *enq = cqm_ctrl->enq;

	p_info = &eqm_port_info[port];
	p_info->num_eq_ports = 1;
	p_info->eq_info.cbm_eq_port_base = enq +
					   EQ_CPU_PORT(port, desc0);
	p_info->eq_info.port_no = port;
	p_info->port_type = cpu_ptr->port_type;
	p_info->eq_info.num_desc = cpu_ptr->num_desc;
	p_info->valid = 1;
	dev_dbg(cqm_ctrl->dev, "0x%p %d %d %d\n",
		p_info->eq_info.cbm_eq_port_base,
		p_info->eq_info.num_desc,
		p_info->eq_info.port_no,
		p_info->port_type);
	init_cqm_enq_cpu_port(port);
	return CBM_SUCCESS;
}

static int configure_ports(const struct cqm_config *port_config)
{
	int result = CBM_FAILURE;

	while (port_config->type != NONE_TYPE) {
		switch (port_config->type) {
		case DQM_DMA_TYPE:
		case DQM_PON_TYPE:
		case DQM_VUNI_TYPE:
			result = conf_deq_dma_port(&port_config->data.dqm_dma);
			break;
		case DQM_CPU_TYPE:
			result = conf_deq_cpu_port(&port_config->data.dqm_cpu);
			break;
		case DQM_ACA_TYPE:
			result = conf_deq_aca_port(&port_config->data.dqm_aca);
			break;
		case EQM_DMA_TYPE:
			result = conf_enq_dma_port(&port_config->data.eqm_dma);
			break;
		case EQM_CPU_TYPE:
			result = conf_enq_cpu_port(&port_config->data.eqm_cpu);
			break;
		};
		if (result)
			return CBM_FAILURE;
		port_config++;
	}
	return CBM_SUCCESS;
}

static inline void cqm_rst_deassert(struct cqm_data *lpp)
{
	u32 status = reset_control_status(lpp->rcu_reset);

	pr_debug("0x%x\n", status);
	reset_control_deassert(lpp->rcu_reset);
	pr_debug("poll until its zero\n");
	while (status)
		status = reset_control_status(lpp->rcu_reset);
}

void cqm_rst(struct cqm_data *lpp)
{
	cqm_rst_deassert(lpp);
}

static int cqm_get_mtu_size(struct cbm_mtu *mtu)
{
	/* Always last pool */
	mtu->mtu = (cqm_ctrl->prx300_pool_size[cqm_ctrl->num_pools - 1] -
		BSL_THRES);
	return CBM_SUCCESS;
}

static const struct cbm_ops cqm_ops = {
	.cbm_igp_delay_set = cqm_igp_delay_set,
	.cbm_igp_delay_get = cqm_igp_delay_get,
	.cbm_buffer_alloc = cqm_buffer_alloc_by_size,
	.cqm_buffer_free_by_policy = cqm_buffer_free_by_policy_prx300,
	.cbm_buffer_free = cqm_buffer_free,
	.cbm_cpu_pkt_tx = cqm_cpu_pkt_tx,
	.check_ptr_validation = check_ptr_validation_prx300,
	.cbm_build_skb = build_skb_cqm,
	.cbm_queue_map_get = queue_map_get,
	.cbm_queue_map_set = queue_map_set,
	.cbm_queue_map_buf_free = queue_map_buf_free,
	.cqm_qid2ep_map_get = qid2ep_map_get,
	.cqm_qid2ep_map_set = qid2ep_map_set,
	.cqm_mode_table_get = mode_table_get,
	.cqm_mode_table_set = mode_table_set,
	.cbm_dp_port_alloc = dp_port_alloc,
	.cbm_dp_port_alloc_complete = dp_port_alloc_complete,
	.cbm_dp_port_dealloc = cqm_dp_port_dealloc,
	.cbm_dp_enable = dp_enable,
	.cqm_qos_queue_flush = qos_q_flush,
	.cbm_alloc_skb = cqm_alloc_skb,
	.cbm_cpu_port_get = cqm_cpu_port_get,
	.pib_program_overshoot = pib_program_overshoot,
	.pib_status_get = pib_status_get,
	.pib_ovflw_cmd_get = pib_ovflw_cmd_get,
	.pib_illegal_cmd_get = pib_illegal_cmd_get,
	.pon_deq_cntr_get = pon_deq_cntr_get,
	.set_lookup_qid_via_index = set_lookup_qid_via_index_prx300,
	.get_lookup_qid_via_index = get_lookup_qid_via_idx_prx300,
	.cqm_map_to_drop_q = map_to_drop_q,
	.cqm_restore_orig_q = restore_orig_q,
	.cbm_enable_backpressure = enable_backpressure,
	.cbm_get_mtu_size = cqm_get_mtu_size,
	.cbm_dp_get_dc_config = cqm_get_dc_config,
};

static const struct of_device_id cqm_prx300_match[] = {
	{ .compatible = "intel,prx300-cqm", .data = &g_prx300_ctrl},
	{},
};

static int check_base_addr(void)
{
	if (!cqm_ctrl->txpush ||
	    !cqm_ctrl->dmadesc ||
	    !cqm_ctrl->cqm ||
	    !cqm_ctrl->qidt ||
	    !cqm_ctrl->ls ||
	    !cqm_ctrl->enq ||
	    !cqm_ctrl->deq ||
	    !cqm_ctrl->qid2ep ||
	    !cqm_ctrl->fsqm ||
	    !cqm_ctrl->pib ||
	    !cqm_ctrl->pon_dqm_cntr) {
		return -ENOMEM;
	}
	return CBM_SUCCESS;
}

static int conf_bm(struct cqm_data *pdata)
{
	int i, j, result = CBM_FAILURE;

	/* copy BM pool and policy from dts*/
	memcpy(cqm_ctrl->prx300_pool_ptrs, pdata->pool_ptrs
		, sizeof(cqm_ctrl->prx300_pool_ptrs));
	memcpy(cqm_ctrl->prx300_pool_size, pdata->pool_size
		, sizeof(cqm_ctrl->prx300_pool_size));
	cqm_ctrl->num_pools = pdata->num_pools;

	/*Handle A1*/
	dev_dbg(cqm_ctrl->dev, "soc rev %d\n", ltq_get_soc_rev());
	if (ltq_get_soc_rev() == 0) {
		memcpy(cqm_ctrl->prx300_pool_ptrs, pdata->pool_ptrs_a1,
		       sizeof(cqm_ctrl->prx300_pool_ptrs));
		memcpy(cqm_ctrl->prx300_pool_size, pdata->pool_size_a1,
		       sizeof(cqm_ctrl->prx300_pool_size));
		cqm_ctrl->num_pools = pdata->num_pools_a1;
	}

	/* check prx300 pool and policy */
	if (cqm_ctrl->num_pools > CQM_PRX300_NUM_BM_POOLS) {
		pr_err("prx300 pools %u\n", cqm_ctrl->num_pools);
		return CBM_FAILURE;
	}
	/* Pool Index loop*/
	for (i = 0; i < cqm_ctrl->num_pools; i++) {
		/* Validate pool and policy */
		if ((cqm_ctrl->prx300_pool_ptrs[i] <= 0) ||
		    (cqm_ctrl->prx300_pool_size[i] <= 0)) {
			pr_err("Idx %u 0x%x 0x%x\n"
				, i, cqm_ctrl->prx300_pool_ptrs[i],
				cqm_ctrl->prx300_pool_size[i]);
			return result;
		}

		/* Config pool size */
		p_param[i].max_allowed = cqm_ctrl->prx300_pool_ptrs[i];
		p_param[i].min_guaranteed = 0x40;

		/* Config no of policy */
		p_param[i].num_pools_in_policy = 1;

		 /* Pool size loop*/
		for (j = 0; j < p_param[i].num_pools_in_policy; j++) {
			p_param[i].pools_in_policy[j].pool_id = (i + j);
			p_param[i].pools_in_policy[j].max_allowed =
			cqm_ctrl->prx300_pool_ptrs[i + j];
		}
	}
	return CBM_SUCCESS;
}

void fsqm_ofsq_read(struct fsq *fsq)
{
	u32 reg;

	reg = cbm_r32(cqm_ctrl->fsqm + OFSQ);
	fsq->head = (reg & OFSQ_HEAD_MASK) >> OFSQ_HEAD_POS;
	fsq->tail = (reg & OFSQ_TAIL_MASK) >> OFSQ_TAIL_POS;
#ifdef HEAD_TAIL_SUPERVISION
	if (fsq->head > initial_tail) {
		pr_err("%s %x %s %x\n",
		       "fsqm_ofsq_read: head", fsq->head,
			   "exceeds initial head", initial_tail);
	}
	if (fsq->tail < initial_head) {
		pr_err("%s %x %s %x\n",
		       "fsqm_ofsq_read: tail", fsq->tail,
			   "underrun initial tail", initial_head);
	}
#endif
}

bool fsqm_check(uint16_t len)
{
	bool pass;
	u16 reg;
	u16 cnt;
	struct fsq fsq;
	u16 head, tail, next;
	void *fsqm = cqm_ctrl->fsqm;

	pass = true;

	memset(&touched[0], 0x00, sizeof(touched));

	/* freeze fsqm */
	set_val(fsqm + FSQM_CTRL, 1, FSQM_CTRL_ALLOC_DIS_MASK,
		FSQM_CTRL_ALLOC_DIS_POS);

	/* check: free segment queue is back at its original length */
	reg = cbm_r32(fsqm + OFSC);
	if (len) {
		if (reg == len) {
			dev_info(cqm_ctrl->dev,
				 "pass: OFSC is back at its expected value");
		} else {
			dev_info(cqm_ctrl->dev, "%s %s %d\n",
				 "error: OFSC is NOT back ",
				 "at its original length =", reg);
			pass = false;
		}
	} else {
		len = reg;
	}
	/* check: free segment queue is not corrupt
	 * step through the free segment queue, starting from the head LSA,
	 * and follow the next pointers until the tail LSA is reached.
	 * In an array, mark all LSAs which were touched while stepping through.
	 * If the FSQ touches the same LSA twice, the FSQ is corrupt.
	 */
	for (cnt = 0; cnt < FSQM_FRM_NUM; cnt++)
		touched[cnt] = false;

	fsqm_ofsq_read(&fsq);	/* get head LSA */
	head = fsq.head;
	tail = fsq.tail;
	dev_info(cqm_ctrl->dev, "start with head: 0x%x tail: 0x%x", head, tail);
	if (head < FSQM_FRM_NUM)
		touched[head] = true;
	else
		return false;

	for (cnt = 0; cnt < (len - 1); cnt++) {
		next =  cbm_r32(FSQM_LLT_RAM(fsqm, head));
		dev_info(cqm_ctrl->dev, "[%5d] %s 0x%04x %s 0x%04x\n", cnt,
			 "llt addr:", head, "llt data:", next);

		if (next == 0x7fff) {
			dev_info(cqm_ctrl->dev, "[%5d] %s %s\n", cnt,
				 "error: Unexpected end of FSQ NIL value",
				 "detected before reaching TAIL segment");
			head = next;
			pass = false;
			cnt++;
			break;
		}

		if (next == tail && cnt != (len - 2)) {
			dev_info(cqm_ctrl->dev, "[%5d] %s %s 0x04%x\n", cnt,
				 "error: Correct TAIL segment reached",
				 "but too early (list shortened)",
				  next);
			head = next;
			pass = false;
			cnt++;
			break;
		}

		if (next >= FSQM_FRM_NUM) {
			dev_info(cqm_ctrl->dev, "[%5d] %s 0x04%x\n", cnt,
				 "error: LLT next pointer out ofrange: ",
				 next);
			head = next;
			pass = false;
			cnt++;
			break;
		}

		if (!touched[next]) {
			touched[next] = true;
		} else {
			dev_info(cqm_ctrl->dev, "%s %s\n",
				 "error: FSQ touches the same LSA",
				 " twice the FSQ is corrupt\n");
			pass = false;
		}

		head = next;
	}

	next =  cbm_r32(FSQM_LLT_RAM(fsqm, head));
	dev_info(cqm_ctrl->dev, "[%5d] llt addr: 0x%04x llt data: 0x%04x", cnt,
		 head, next);

	tail = fsq.tail;
	if (tail == head) {
		dev_info(cqm_ctrl->dev, "Arrived at FSQ tail LSA");
	} else {
		dev_info(cqm_ctrl->dev, "[%5d] %s 0x%x %s %s0x%x\n",
			 cnt, "error: FSQ tail LSA:", tail, "does not match",
			 "last next LSA: ", head);
		pass = false;
	}

	/* unfreeze fsqm */
	set_val(fsqm + FSQM_CTRL, 0, FSQM_CTRL_ALLOC_DIS_MASK,
		FSQM_CTRL_ALLOC_DIS_POS);
	return pass;
}

static void cqm_enable_re_insertion_port(s32 portid)
{
	struct cbm_dp_alloc_data data = {0};

	/* Fill the DMA channels and enable */
	fill_dp_alloc_data(&data, 0, DMA_PORT_RE_INSERTION, 0);
	handle_dma_chnl_init(DMA_PORT_RE_INSERTION, 0);
}

static int cqm_prx300_probe(struct platform_device *pdev)
{
	struct resource *res[PRX300_MAX_RESOURCE] = {NULL};
	int i;
	unsigned long sys_flag;
	struct cqm_data *pdata = NULL;
	const struct of_device_id *match;
	u32 port_type;
	int port_no;
	int result, ret;
	struct clk *cqm_clk;

	memset(&cqm_dbg_cntrs, 0, sizeof(cqm_dbg_cntrs));
	pdata = platform_get_drvdata(pdev);
	match = of_match_device(cqm_prx300_match, &pdev->dev);
	if (!match) {
		dev_err(&pdev->dev, "Error: No matching device found\n");
		return -ENODEV;
	}
	cqm_ctrl = (struct cqm_ctrl *)match->data;
	cqm_ctrl->cqm_ops = &cqm_ops;
	register_cbm(cqm_ctrl->cqm_ops);
	dp_register_ops(0, DP_OPS_CQM, (void *)cqm_ctrl->cqm_ops);
	/** init spin lock */
	spin_lock_init(&cqm_qidt_lock);
	spin_lock_init(&cqm_port_map);
	spin_lock_init(&cpu_pool_enq);

	#if 1
	/* load the memory ranges */
	for (i = 0; i < pdata->num_resources; i++) {
		res[i] = platform_get_resource(pdev, IORESOURCE_MEM, i);
		if (!res[i]) {
			dev_err(&pdev->dev, "failed to get resources %d\n", i);
			return -ENOENT;
		}
	}

	ret = of_reserved_mem_device_init_by_idx(&pdev->dev, pdev->dev.of_node,
						 0);
	/* Ignore if this is not available in the device tree, we will use
	 * the normal CMA memory instead.
	 */
	if (ret && ret != -ENODEV) {
		dev_err(&pdev->dev, "Error adding reserved mem: %i\n", ret);
		return ret;
	}

	#endif

	cqm_ctrl->name = cqm_name;
	cqm_ctrl->id = pdev->id;
	cqm_ctrl->dev = &pdev->dev;
	cqm_ctrl->txpush = devm_ioremap_resource(&pdev->dev, res[0]);
	cqm_ctrl->dmadesc = devm_ioremap_resource(&pdev->dev, res[1]);
	cqm_ctrl->dmadesc_phys = (void *)res[1]->start;
	cqm_ctrl->cqm = devm_ioremap_resource(&pdev->dev, res[2]);
	cqm_ctrl->qidt = devm_ioremap_resource(&pdev->dev, res[3]);
	cqm_ctrl->ls = devm_ioremap_resource(&pdev->dev, res[4]);
	cqm_ctrl->enq = devm_ioremap_resource(&pdev->dev, res[5]);
	cqm_ctrl->deq = devm_ioremap_resource(&pdev->dev, res[6]);
	cqm_ctrl->deq_phy = (void *)res[6]->start;
	cqm_ctrl->qid2ep = devm_ioremap_resource(&pdev->dev, res[7]);
	cqm_ctrl->fsqm = devm_ioremap_resource(&pdev->dev, res[8]);
	cqm_ctrl->pib = devm_ioremap_resource(&pdev->dev, res[9]);
	cqm_ctrl->pon_dqm_cntr = devm_ioremap_resource(&pdev->dev, res[10]);

	result = check_base_addr();
	if (result) {
		dev_err(cqm_ctrl->dev, "failed to request and remap io ranges\n");
		return result;
	}

	store_bufreq_baseaddr();
	cqm_ctrl->cbm_irq[0] = pdata->intrs[0];
	for_each_present_cpu(i) {
		if (cpu_online(i))
			cqm_ctrl->cbm_irq[i + 1] = pdata->intrs[i + 1];
		else
			cqm_ctrl->cbm_irq[i + 1] = 0;
	}
#ifndef CONFIG_USE_EMULATOR
	cqm_ctrl->cbm_clk = devm_clk_get(cqm_ctrl->dev, "freq");
	if (IS_ERR(cqm_ctrl->cbm_clk)) {
		ret = PTR_ERR(cqm_ctrl->cbm_clk);
		dev_err(cqm_ctrl->dev, "failed to get cqm_ctrl->cqm:%d\n", ret);
		return ret;
	}
	cqm_clk = devm_clk_get(cqm_ctrl->dev, "cbm");
	if (IS_ERR(cqm_clk)) {
		ret = PTR_ERR(cqm_clk);
		dev_err(cqm_ctrl->dev, "failed to get cqm_clk:%d\n", ret);
		return ret;
	}
	clk_prepare_enable(cqm_clk);
	clk_prepare_enable(cqm_ctrl->cbm_clk);
#endif
	cqm_ctrl->syscfg = pdata->syscfg;
	cqm_ctrl->force_xpcs = pdata->force_xpcs;
	cqm_ctrl->gint_mode = pdata->gint_mode;

	cqm_ctrl->split[0] = pdata->bm_buff_split[0];
	cqm_ctrl->split[1] = pdata->bm_buff_split[1];

	if (conf_bm(pdata) != CBM_SUCCESS) {
		pr_err("conf_BMpool_and_policy failed\n");
		return CBM_FAILURE;
	}

	spin_lock_irqsave(&cqm_qidt_lock, sys_flag);
	for (i = 0; i < CQM_QIDT_DW_NUM; i++) {
		g_cbm_qidt_mirror[i].qidt_shadow = PRX300_CQM_DROP_INIT;
		cbm_w32((cqm_ctrl->qidt + i * 4), PRX300_CQM_DROP_INIT);
	}
	for (i = 0; i < CQM_QID2EP_DW_NUM; i++)
		cbm_w32((cqm_ctrl->qid2ep + i * 4), 0x0);
	for (i = 0; i < PRX300_MAX_PON_PORTS; i++)
		cbm_w32((cqm_ctrl->pon_dqm_cntr + i * 4), 0x0);
	spin_unlock_irqrestore(&cqm_qidt_lock, sys_flag);
	bm_init(pdev);
	if (cbm_hw_init(pdev))
		return -1;
	configure_ports(cqm_ctrl->cqm_cfg);

	/* Fill port info only if entry is in dtsi file */
	if (pdata->num_dq_port) {
		for (i = 0; i < pdata->num_dq_port; i += MAX_CPU_DQ_PORT_ARGS) {
			port_no   = pdata->dq_port[i];
			port_type = pdata->dq_port[i + 1];

			if (!((port_no >= 0) && (port_no < CQM_MAX_CPU))) {
				pr_err("Invalid cpu deq port-no %d\n", port_no);
				continue;
			}

			if (!IS_CPU_PORT_TYPE(port_type)) {
				pr_err("Inv cpu dq port-type %d\n", port_type);
				continue;
			}
			dqm_port_info[port_no].cpu_port_type = port_type;
		}
	}

	cbm_w32(cqm_ctrl->enq + IP_OCC_EN, 0);
	cbm_w32((cqm_ctrl->cqm + CBM_BSL_CTRL), CBM_BSL_CTRL_BSL1_EN_MASK |
		CBM_BSL_CTRL_BSL2_EN_MASK);
	/*Enable the EQM and DQM contollers*/
	cbm_w32((cqm_ctrl->deq + CBM_DQM_CTRL), CBM_DQM_CTRL_DQM_EN_MASK);
	for (i = 0; i < DEFAULT_WAIT_CYCLES; i++) {
		if ((cbm_r32(cqm_ctrl->deq + CBM_DQM_CTRL) &
		    CBM_DQM_CTRL_DQM_ACT_MASK) != 0)
			dev_dbg(cqm_ctrl->dev, "cbm dqm init successfully\n");
	}
	init_cbm_ls(cqm_ctrl->ls);
	cbm_w32((cqm_ctrl->enq + CBM_EQM_CTRL), CBM_EQM_CTRL_EQM_EN_MASK);
	for (i = 0; i < DEFAULT_WAIT_CYCLES; i++) {
		if ((cbm_r32(cqm_ctrl->enq + CBM_EQM_CTRL) &
		    CBM_EQM_CTRL_EQM_ACT_MASK) != 0) {
			dev_dbg(cqm_ctrl->dev, "cbm eqm init successfully\n");
			break;
		}
	}

	cqm_ctrl->radio_num = pdata->radio_dev_num;

	cqm_ctrl->re_insertion = pdata->re_insertion;
	if (cqm_ctrl->re_insertion > 0)
		cqm_enable_re_insertion_port(cqm_ctrl->re_insertion);

#if 1
	for_each_online_cpu(i)
		tasklet_init(&cqm_ctrl->cqm_tasklet[i],
			     do_cqm_tasklet, (unsigned long)i);
#ifdef CPU_POOL_ALLOWED
	tasklet_init(&cqm_ctrl->cqm_cpu_free_tasklet,
		     cqm_cpu_free_tasklet, (unsigned long)i);
#endif
#ifdef ENABLE_LL_DEBUG
	tasklet_init(&cbm_debug_tasklet,
		     do_cbm_debug_tasklet, (unsigned long)i);
#endif
#ifdef CONFIG_CBM_LS_ENABLE
	init_dummy_netdev(&g_cbm_ctrl.dummy_dev);
	netif_napi_add(&g_cbm_ctrl.dummy_dev, &g_cbm_ctrl.napi,
		       do_dq_cbm_poll, 20);
	napi_enable(&g_cbm_ctrl.napi);
#endif
#endif

	/* Disable all the EQM and DQM interrupts */
	eqm_intr_ctrl(0);
#ifndef CONFIG_CBM_LS_ENABLE
	dqm_intr_ctrl(0);
#endif
	/* Enable all the LS interrupts */
	ls_intr_ctrl(0xFF0000, cqm_ctrl->ls);

	/*Override the HW reset values*/
	cbm_w32(cqm_ctrl->enq + DMA_RDY_EN, 0xfff);

	/*setup the DMA channels*/
	for (i = 0; i < CQM_ENQ_PORT_MAX; i++) {
		if (!eqm_port_info[i].valid)
			continue;
		switch (eqm_port_info[i].dma_dt_init_type) {
		case ENQ_DMA_FSQM_CHNL:
		case ENQ_DMA_SIZE0_CHNL:
		case ENQ_DMA_SIZE1_CHNL:
		case ENQ_DMA_SIZE2_CHNL:
		case ENQ_DMA_SIZE3_CHNL:
			cqm_enqueue_dma_port_init(i,
						  eqm_port_info[i].dma_dt_ctrl,
						  eqm_port_info[i].dma_dt_ch,
						  eqm_port_info[i].
						  dma_dt_init_type);
			break;
		default:
			break;
		}
	}
	cqm_debugfs_init(cqm_ctrl);
	dev_info(cqm_ctrl->dev, "CBM: Init Done !!\n");
	for_each_online_cpu(i)
		init_cqm_deq_cpu_port(i, 0);
	return 0;
}

static int cqm_prx300_release(struct platform_device *pdev)
{
	of_reserved_mem_device_release(&pdev->dev);
	return 0;
}

static struct platform_driver cqm_prx300_driver = {
	.probe = cqm_prx300_probe,
	.remove = cqm_prx300_release,
	.driver = {
		.name = PRX300_DEV_NAME,
		.owner = THIS_MODULE,
	},
};

int __init cqm_prx300_init(void)
{
	return platform_driver_register(&cqm_prx300_driver);
}

subsys_initcall(cqm_prx300_init);
