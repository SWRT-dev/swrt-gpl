#ifndef __CQM_COMMON_H
#define __CQM_COMMON_H
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/dma/lantiq_dmax.h>
#include <linux/netdevice.h>
#include <net/lantiq_cbm_api.h>

#define CBM_PMAC_DYNAMIC 1000
#define CBM_PORT_INVALID 2000
#define CBM_PMAC_NOT_APPL 3000
#define CBM_PORT_NOT_APPL 255
#define LS_PORT_NUM             4
#define CBM_MAX_INT_LINES       8
#define CBM_DMA_DESC_OWN        1U/*belong to DMA*/
 /* DMA offset to be 128 byte aligned
  * CBM_DMA_DATA_OFFSET + NET_IP_ALIGN + NET_SKB_PAD +
  * CQM_POOL_METADATA = 128 + 6(byte offset for IP ALIGN)
  */
#define CBM_DMA_ALIGN 128
#define CBM_BYTE_OFFSET 6
#define CBM_DMA_DATA_OFFSET     \
(CBM_DMA_ALIGN + CBM_BYTE_OFFSET - NET_IP_ALIGN  - \
NET_SKB_PAD - CQM_POOL_METADATA)
/* TCP lite and LRO expects it to be 128 */
#define CBM_GRX550_DMA_DATA_OFFSET     128
#define DEFAULT_WAIT_CYCLES     20
#define DEFAULT_LS_QTHH         7
#define DEFAULT_LS_OVFL_CNT     0x2000
#define DEFAULT_LS_PORT_WEIGHT  2
#define CBM_SUCCESS             0
#define CBM_FAILURE             -1
#define CBM_EQM_DELAY_ENQ 0x10
#define CBM_PDU_TYPE 26

#define CBM_LS_PORT(idx, reg) \
(LS_DESC_DW0_PORT0 + ((idx) * 0x100) + offsetof(struct cbm_ls_reg, reg))

#define cbm_r32(m_reg)		readl(m_reg)
#define cbm_w32(m_reg, val)	writel(val, m_reg)
#define cbm_assert(cond, fmt, arg...) \
do { \
	if (!(cond)) \
		pr_err("%d:%s:" fmt "\n", __LINE__, __func__, ##arg); \
} while (0)
#define cbm_err(fmt, arg...) \
pr_err("%d:%s:"fmt "\n", __LINE__, __func__, ##arg)

#ifdef CBM_DEBUG
#define cbm_debug(fmt, arg...) \
pr_info(fmt, ##arg)
#else
#define cbm_debug(fmt, arg...)
#endif

#ifdef CBM_DEBUG_LVL_1
#define cbm_debug_1(fmt, arg...) \
pr_info(fmt, ##arg)
#else
#define cbm_debug_1(fmt, arg...)
#endif

#define get_val(val, mask, offset) (((val) & (mask)) >> (offset))

enum {
	SPREAD_WRR = 0,
	SPREAD_FLOWID = 1,
	SPREAD_MAX,
};

struct cbm_ls_reg {
	struct cbm_desc desc;
	u32 ctrl;
	u32 status;
	u32 resv0[2];
	struct cbm_desc qdesc[7];
};

struct qidt_flag_done {
	u8 cls_done;
	u8 ep_done;
	u8 fl_done;
	u8 fh_done;
	u8 dec_done;
	u8 enc_done;
	u8 mpe1_done;
	u8 mpe2_done;
	u8 sub_if_id_done;
	u8 sub_if_dc_done;
};

struct cbm_q_info {
u16 refcnt; /* No of Queue Map table entries pointing to this q */
u16 qmap_idx_start; /* First index of Queue Map table pointing to q */
u16 qmap_idx_end; /* last index of Queue Map table pointing to q */
};

struct cbm_qidt_shadow {
u32 qidt_shadow;
u32 qidt_drop_flag;
};

struct cbm_ops {
	s32 (*cbm_queue_delay_enable_set)(s32 enable, s32 queue);
	s32 (*cbm_igp_delay_set)(s32 cbm_port_id, s32 delay);
	s32 (*cbm_igp_delay_get)(s32 cbm_port_id, s32 *delay);
	struct sk_buff *(*cbm_build_skb)(void *data, unsigned int frag_size,
					 gfp_t priority);
	s32 (*cbm_queue_map_get)(int cbm_inst, s32 queue_id, s32 *num_entries,
				 cbm_queue_map_entry_t **entries,
				 u32 flags);
	s32 (*cbm_queue_map_set)(int cbm_inst, s32 queue_id,
				 cbm_queue_map_entry_t *entry, u32 flags);
	s32 (*cqm_qid2ep_map_get)(int qid, int *port);
	s32 (*cqm_qid2ep_map_set)(int qid, int port);
	s32 (*cqm_mode_table_get)(int cbm_inst, int *mode,
				  cbm_queue_map_entry_t *entry, u32 flags);
	s32 (*cqm_mode_table_set)(int cbm_inst,
				  cbm_queue_map_entry_t *entry, u32 mode,
				  u32 flags);
	int (*cbm_setup_desc)(struct cbm_desc *desc, u32 data_ptr, u32 data_len,
			      u32 DW0, u32 DW1);
	int (*cbm_cpu_enqueue_hw)(u32 pid, struct cbm_desc *desc,
				  void *data_pointer,  int flags);
	void *(*cbm_buffer_alloc)(u32 pid, u32 flag, u32 size);
	void *(*cqm_buffer_alloc_by_policy)(u32 pid, u32 flag, u32 policy);
	struct sk_buff *(*cbm_copy_skb)(const struct sk_buff *skb,
					gfp_t gfp_mask);
	struct sk_buff *(*cbm_alloc_skb)(unsigned int size, gfp_t priority);
	int (*cbm_buffer_free)(u32 pid, void *v_buf, u32 flag);
	int (*check_ptr_validation)(u32 buf);
	s32 (*cbm_cpu_pkt_tx)(struct sk_buff *skb, struct cbm_tx_data *data,
			      u32 flags);
	s32 (*cbm_port_quick_reset)(s32 cbm_port_id, u32 flags);
	u32 (*cbm_get_dptr_scpu_egp_count)(u32 cbm_port_id, u32 flags);
	s32 (*cbm_dp_port_alloc)(struct module *owner, struct net_device *dev,
				 u32 dev_port, s32 dp_port,
				 struct cbm_dp_alloc_data *data, u32 flags);
	s32 (*cbm_dp_port_alloc_complete)(struct module *owner,
					  struct net_device *dev,
					  u32 dev_port, s32 dp_port,
				  struct cbm_dp_alloc_complete_data *data,
					  u32 flags);
	int (*cbm_get_wlan_umt_pid)(u32 ep_id, u32 *cbm_pid);
	s32 (*cbm_dp_enable)(struct module *owner, u32 dp_port,
			     struct cbm_dp_en_data *data, u32 flags,
			     u32 alloc_flags);
	s32 (*cqm_qos_queue_flush)(s32 cqm_inst, s32 cqm_drop_port, int qid);
	s32 (*cbm_queue_flush)(s32 cbm_port_id, s32 queue_id, u32 timeout,
			       u32 flags);
	s32 (*cbm_dp_q_enable)(int cbm_inst, s32 dp_port_id, s32 qnum,
			       s32 tmu_port_id, s32 remap_to_qid, u32 timeout,
			       s32 qidt_valid, u32 flags);
	s32 (*cbm_enqueue_port_resources_get)(cbm_eq_port_res_t *res,
					      u32 flags);
	s32 (*cbm_dequeue_port_resources_get)(u32 dp_port,
					      cbm_dq_port_res_t *res,
					      u32 flags);
	s32 (*cbm_dp_port_resources_get)(u32 *dp_port, u32 *num_tmu_ports,
					 cbm_tmu_res_t **res_pp,
					 u32 flags);
	s32 (*cbm_reserved_dp_resources_get)(u32 *tmu_port, u32 *tmu_sched,
					     u32 *tmu_q);
	s32 (*cbm_get_egress_port_info)(u32 cbm_port, u32 *tx_ch, u32 *flags);
	s32 (*cbm_enqueue_port_overhead_set)(s32 port_id, int8_t ovh);
	s32 (*cbm_enqueue_port_overhead_get)(s32 port_id, int8_t *ovh);
	s32 (*cbm_enqueue_port_thresh_get)(s32 cbm_port_id,
					   cbm_port_thresh_t *thresh,
					   u32 flags);
	s32 (*cbm_enqueue_port_thresh_set)(s32 cbm_port_id,
					   cbm_port_thresh_t *thresh,
					   u32 flags);
	s32 (*cbm_dequeue_cpu_port_stats_get)(s32 cbm_port_id, u32 *deq_ctr,
					      u32 flags);
	s32 (*cbm_enqueue_cpu_port_stats_get)(s32 cbm_port_id,
					      u32 *occupancy_ctr, u32 *enq_ctr,
					      u32 flags);
	s32 (*cbm_dequeue_dma_port_stats_get)(s32 cbm_port_id, u32 *deq_ctr,
					      u32 flags);
	s32 (*cbm_enqueue_dma_port_stats_get)(s32 cbm_port_id,
					      u32 *occupancy_ctr,
					      u32 *enq_ctr,
					      u32 flags);
	void (*set_lookup_qid_via_index)(u32 index, u32 qid);
	uint8_t (*get_lookup_qid_via_index)(u32 index);
	u8 (*get_lookup_qid_via_bits)(
	u32 ep,
	u32 classid,
	u32 mpe1,
	u32 mpe2,
	u32 enc,
	u32 dec,
	u8 flow_id,
	u32 dic);
	int (*cbm_q_thres_get)(u32 *length);
	int (*cbm_q_thres_set)(u32 length);
	s32 (*cbm_dp_port_dealloc)(struct module *owner, u32 dev_port,
				   s32 cbm_port_id,
				   struct cbm_dp_alloc_data *data, u32 flags);
	s32 (*cbm_enqueue_mgr_ctrl_get)(cbm_eqm_ctrl_t *ctrl, u32 flags);
	s32 (*cbm_enqueue_mgr_ctrl_set)(cbm_eqm_ctrl_t *ctrl, u32 flags);
	s32 (*cbm_dequeue_mgr_ctrl_get)(cbm_dqm_ctrl_t *ctrl, u32 flags);
	s32 (*cbm_dequeue_mgr_ctrl_set)(cbm_dqm_ctrl_t *ctrl, u32 flags);
	int (*cbm_counter_mode_set)(int enq, int mode);
	int (*cbm_counter_mode_get)(int enq, int *mode);
	s32 (*cbm_cpu_port_get)(struct cbm_cpu_port_data *data, u32 flags);
	s32 (*pib_program_overshoot)(u32 overshoot_bytes);
	s32 (*pib_status_get)(struct pib_stat *ctrl);
	s32 (*pib_ovflw_cmd_get)(u32 *cmd);
	s32 (*pib_illegal_cmd_get)(u32 *cmd);
	s32 (*pon_deq_cntr_get)(int port, u32 *count);
	void (*cbm_setup_DMA_p2p)(void);
	int (*cbm_turn_on_DMA_p2p)(void);
	s32 (*cbm_enable_backpressure)(s32 port_id, bool flag);
	s32 (*cbm_get_mtu_size)(u32 *mtu_size);
};

static inline void set_val(void __iomem *reg, u32 val, u32 mask, u32 offset)
{
	u32 temp_val = cbm_r32(reg);

	temp_val &= ~(mask);
	temp_val |= (((val) << (offset)) & (mask));
	cbm_w32(reg, temp_val);
}

static inline int cqm_desc_data_len(u32 dw)
{
	return dw & 0x0000FFFF;
}

static inline int cqm_desc_data_off(u32 dw)
{
	return (dw & 0x3800000) >> 23;
}

static inline int cqm_desc_data_pool(u32 dw)
{
	return (dw & 0x70000) >> 16;
}

static inline int cqm_desc_data_policy(u32 dw)
{
	return (dw & 0x700000) >> 20;
}

static inline int get_is_bit_set(u32 flags)
{
	return ffs(flags) - 1;
}

void buf_addr_adjust(unsigned int buf_base_addr, unsigned int buf_size,
		     unsigned int *adjusted_buf_base,
		     unsigned int *adjusted_buf_size,
		     unsigned int align);
int cbm_linearise_buf(struct sk_buff *skb, struct cbm_tx_data *data,
		      int buf_size, u32 new_buf);
void init_cbm_ls(void __iomem *cqm_ls_addr_base);
int cqm_dma_get_controller(char *ctrl);
void ls_intr_ctrl(u32 val, void __iomem *cqm_ls_addr_base);
void cbm_dw_memset(u32 *base, int val, u32 size);
void register_cbm(const struct cbm_ops *cbm_cb);
#endif

