#include "cqm_common.h"
static const struct cbm_ops *g_cbm_ops;
void cbm_setup_DMA_p2p(void)
{
	if (g_cbm_ops->cbm_setup_DMA_p2p)
		g_cbm_ops->cbm_setup_DMA_p2p();
}
EXPORT_SYMBOL(cbm_setup_DMA_p2p);

int cbm_turn_on_DMA_p2p(void)
{
	if (g_cbm_ops->cbm_turn_on_DMA_p2p)
		return g_cbm_ops->cbm_turn_on_DMA_p2p();
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_turn_on_DMA_p2p);

s32 cbm_enable_backpressure(s32 port_id, bool flag)
{
	if (g_cbm_ops->cbm_enable_backpressure)
		return g_cbm_ops->cbm_enable_backpressure(port_id, flag);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_enable_backpressure);

s32 cbm_get_mtu_size(u32 *mtu_size)
{
	if (g_cbm_ops->cbm_get_mtu_size)
		return g_cbm_ops->cbm_get_mtu_size(mtu_size);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_get_mtu_size);

s32 cbm_queue_map_get(int cbm_inst, s32 queue_id, s32 *num_entries,
		      cbm_queue_map_entry_t **entries, u32 flags)
{
	if (g_cbm_ops->cbm_queue_map_get)
		return g_cbm_ops->cbm_queue_map_get(
		cbm_inst, queue_id, num_entries, entries, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_queue_map_get);

s32 cbm_queue_map_set(int cbm_inst, s32 queue_id,
		      cbm_queue_map_entry_t *entry,
		      u32 flags)
{
	if (g_cbm_ops->cbm_queue_map_set)
		return g_cbm_ops->cbm_queue_map_set(
		cbm_inst, queue_id, entry, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_queue_map_set);

s32 cqm_qid2ep_map_set(int qid, int port)
{
	if (g_cbm_ops->cqm_qid2ep_map_set)
		return g_cbm_ops->cqm_qid2ep_map_set(qid, port);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cqm_qid2ep_map_set);

s32 cqm_qid2ep_map_get(int qid, int *port)
{
	if (g_cbm_ops->cqm_qid2ep_map_get)
		return g_cbm_ops->cqm_qid2ep_map_get(qid, port);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cqm_qid2ep_map_get);

s32
cqm_mode_table_set(int cbm_inst, cbm_queue_map_entry_t *entry, u32 mode,
		   u32 flags)
{
	if (g_cbm_ops->cqm_mode_table_set)
		return g_cbm_ops->cqm_mode_table_set(cbm_inst, entry, mode,
						     flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cqm_mode_table_set);

s32
cqm_mode_table_get(int cbm_inst, int *mode,
		   cbm_queue_map_entry_t *entry,
		   u32 flags)
{
	if (g_cbm_ops->cqm_mode_table_get)
		return g_cbm_ops->cqm_mode_table_get(
		cbm_inst, mode, entry, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cqm_mode_table_get);

struct sk_buff *cbm_build_skb(void *data, unsigned int frag_size,
			      gfp_t priority)
{
	if (g_cbm_ops->cbm_build_skb)
		return g_cbm_ops->cbm_build_skb(data, frag_size, priority);
	else
		return NULL;
}
EXPORT_SYMBOL(cbm_build_skb);

int cbm_setup_desc(struct cbm_desc *desc, u32 data_ptr, u32 data_len,
		   struct sk_buff *skb)
{
	if (g_cbm_ops->cbm_setup_desc)
		return g_cbm_ops->cbm_setup_desc(
		desc, data_ptr, data_len, skb->DW0, skb->DW1);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_setup_desc);

int cbm_cpu_enqueue_hw(u32 pid, struct cbm_desc *desc, void *data_pointer,
		       int flags)
{
	if (g_cbm_ops->cbm_cpu_enqueue_hw)
		return g_cbm_ops->cbm_cpu_enqueue_hw(
		pid, desc, data_pointer, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_cpu_enqueue_hw);

void *cbm_buffer_alloc(u32 pid, u32 flag, u32 size)
{
	if (g_cbm_ops->cbm_buffer_alloc)
		return g_cbm_ops->cbm_buffer_alloc(pid, flag, size);
	else
		return NULL;
}
EXPORT_SYMBOL(cbm_buffer_alloc);

void *cqm_buffer_alloc_by_policy(u32 pid, u32 flag, u32 policy)
{
	if (g_cbm_ops->cqm_buffer_alloc_by_policy)
		return g_cbm_ops->cqm_buffer_alloc_by_policy(pid, flag, policy);
	else
		return NULL;
}
EXPORT_SYMBOL(cqm_buffer_alloc_by_policy);

int cqm_buffer_free_by_policy(struct cqm_bm_free *free_info)
{
	return CBM_FAILURE;
}
EXPORT_SYMBOL(cqm_buffer_free_by_policy);

struct sk_buff *cbm_copy_skb(const struct sk_buff *skb, gfp_t gfp_mask)
{
	if (g_cbm_ops->cbm_copy_skb)
		return g_cbm_ops->cbm_copy_skb(skb, gfp_mask);
	else
		return NULL;
}
EXPORT_SYMBOL(cbm_copy_skb);

struct sk_buff *cbm_alloc_skb(unsigned int size, gfp_t priority)
{
	if (g_cbm_ops->cbm_alloc_skb)
		return g_cbm_ops->cbm_alloc_skb(size, priority);
	else
		return NULL;
}
EXPORT_SYMBOL(cbm_alloc_skb);

int cbm_buffer_free(u32 pid, void *v_buf, u32 flag)
{
	if (g_cbm_ops->cbm_buffer_free)
		return g_cbm_ops->cbm_buffer_free(pid, v_buf, flag);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_buffer_free);

inline int check_ptr_validation(u32 buf)
{
	if (g_cbm_ops->check_ptr_validation)
		return g_cbm_ops->check_ptr_validation(buf);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(check_ptr_validation);

s32 cbm_cpu_pkt_tx(struct sk_buff *skb, struct cbm_tx_data *data, u32 flags)
{
	if (g_cbm_ops->cbm_cpu_pkt_tx)
		return g_cbm_ops->cbm_cpu_pkt_tx(skb, data, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_cpu_pkt_tx);

s32 cbm_port_quick_reset(s32 cbm_port_id, u32 flags)
{
	if (g_cbm_ops->cbm_port_quick_reset)
		return g_cbm_ops->cbm_port_quick_reset(cbm_port_id, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_port_quick_reset);

s32 cbm_dp_port_alloc(struct module *owner, struct net_device *dev,
		      u32 dev_port, s32 dp_port, struct cbm_dp_alloc_data *data,
		      u32 flags)
{
	if (g_cbm_ops->cbm_dp_port_alloc)
		return g_cbm_ops->cbm_dp_port_alloc(
		owner, dev, dev_port, dp_port, data, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dp_port_alloc);

s32 cbm_dp_port_alloc_complete(struct module *owner, struct net_device *dev,
			       u32 dev_port, s32 dp_port,
			       struct cbm_dp_alloc_complete_data *data,
			       u32 flags)
{
	if (g_cbm_ops && g_cbm_ops->cbm_dp_port_alloc_complete)
		return g_cbm_ops->cbm_dp_port_alloc_complete(
		owner, dev, dev_port, dp_port, data, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dp_port_alloc_complete);

int cbm_get_wlan_umt_pid(u32 ep_id, u32 *cbm_pid)
{
	if (g_cbm_ops->cbm_get_wlan_umt_pid)
		return g_cbm_ops->cbm_get_wlan_umt_pid(ep_id, cbm_pid);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_get_wlan_umt_pid);

s32 cbm_dp_enable(struct module *owner, u32 dp_port,
		  struct cbm_dp_en_data *data, u32 flags, u32 alloc_flags)
{
	if (g_cbm_ops->cbm_dp_enable)
		return g_cbm_ops->cbm_dp_enable(owner, dp_port,
						data, flags, alloc_flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dp_enable);

s32 cqm_qos_queue_flush(int cqm_inst, int cqm_drop_port, int qid)
{
	if (g_cbm_ops->cqm_qos_queue_flush)
		return g_cbm_ops->cqm_qos_queue_flush(cqm_inst, cqm_drop_port,
					qid);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cqm_qos_queue_flush);

s32 cbm_queue_flush(s32 cbm_port_id, s32 queue_id, u32 timeout, u32 flags)
{
	if (g_cbm_ops->cbm_queue_flush)
		return g_cbm_ops->cbm_queue_flush(cbm_port_id, queue_id,
						  timeout, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_queue_flush);

s32 cbm_dp_q_enable(
	int cbm_inst,
	s32 dp_port_id,
	s32 qnum,
	s32 tmu_port_id,
	s32 remap_to_qid,
	u32 timeout,
	s32 qidt_valid,
	u32 flags
	)
{
	if (g_cbm_ops->cbm_dp_q_enable)
		return g_cbm_ops->cbm_dp_q_enable(
		cbm_inst, dp_port_id, qnum, tmu_port_id,
		remap_to_qid, timeout,
		qidt_valid, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dp_q_enable);

s32 cbm_enqueue_port_resources_get(cbm_eq_port_res_t *res, u32 flags)
{
	if (g_cbm_ops->cbm_enqueue_port_resources_get)
		return g_cbm_ops->cbm_enqueue_port_resources_get(res, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_enqueue_port_resources_get);

s32 cbm_dequeue_port_resources_get(u32 dp_port, cbm_dq_port_res_t *res,
				   u32 flags)
{
	if (g_cbm_ops->cbm_dequeue_port_resources_get)
		return g_cbm_ops->cbm_dequeue_port_resources_get(dp_port, res,
								 flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dequeue_port_resources_get);

s32 cbm_dp_port_resources_get(u32 *dp_port, u32 *num_tmu_ports,
			      cbm_tmu_res_t **res_pp, u32 flags)
{
	if (g_cbm_ops->cbm_dp_port_resources_get)
		return g_cbm_ops->cbm_dp_port_resources_get(
		dp_port,
		num_tmu_ports, res_pp, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dp_port_resources_get);

s32 cbm_reserved_dp_resources_get(u32 *tmu_port, u32 *tmu_sched, u32 *tmu_q)
{
	if (g_cbm_ops->cbm_reserved_dp_resources_get)
		return g_cbm_ops->cbm_reserved_dp_resources_get(
		tmu_port, tmu_sched, tmu_q);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_reserved_dp_resources_get);

s32 cbm_get_egress_port_info(u32 cbm_port, u32 *tx_ch, u32 *flags)
{
	if (g_cbm_ops->cbm_get_egress_port_info)
		return g_cbm_ops->cbm_get_egress_port_info(cbm_port, tx_ch,
							   flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_get_egress_port_info);

s32 cbm_enqueue_port_overhead_set(s32 port_id, int8_t ovh)
{
	if (g_cbm_ops->cbm_enqueue_port_overhead_set)
		return g_cbm_ops->cbm_enqueue_port_overhead_set(port_id, ovh);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_enqueue_port_overhead_set);

s32 cbm_enqueue_port_overhead_get(s32 port_id, int8_t *ovh)
{
	if (g_cbm_ops->cbm_enqueue_port_overhead_get)
		return g_cbm_ops->cbm_enqueue_port_overhead_get(port_id, ovh);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_enqueue_port_overhead_get);

s32 cbm_enqueue_port_thresh_get(s32 cbm_port_id,
				cbm_port_thresh_t *thresh, u32 flags)
{
	if (g_cbm_ops->cbm_enqueue_port_thresh_get)
		return g_cbm_ops->cbm_enqueue_port_thresh_get(cbm_port_id,
							      thresh, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_enqueue_port_thresh_get);

s32 cbm_enqueue_port_thresh_set(s32 cbm_port_id,
				cbm_port_thresh_t *thresh, u32 flags)
{
	if (g_cbm_ops->cbm_enqueue_port_thresh_set)
		return g_cbm_ops->cbm_enqueue_port_thresh_set(cbm_port_id,
							      thresh, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_enqueue_port_thresh_set);

s32 cbm_dequeue_cpu_port_stats_get(s32 cbm_port_id, u32 *deq_ctr, u32 flags)
{
	if (g_cbm_ops->cbm_dequeue_cpu_port_stats_get)
		return g_cbm_ops->cbm_dequeue_cpu_port_stats_get(
		cbm_port_id, deq_ctr, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dequeue_cpu_port_stats_get);

s32 cbm_enqueue_cpu_port_stats_get(s32 cbm_port_id, u32 *occupancy_ctr,
				   u32 *enq_ctr, u32 flags)
{
	if (g_cbm_ops->cbm_enqueue_cpu_port_stats_get)
		return g_cbm_ops->cbm_enqueue_cpu_port_stats_get(
		cbm_port_id, occupancy_ctr, enq_ctr, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_enqueue_cpu_port_stats_get);

s32 cbm_dequeue_dma_port_stats_get(s32 cbm_port_id, u32 *deq_ctr, u32 flags)
{
	if (g_cbm_ops->cbm_dequeue_dma_port_stats_get)
		return g_cbm_ops->cbm_dequeue_dma_port_stats_get(
		cbm_port_id, deq_ctr, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dequeue_dma_port_stats_get);

s32 cbm_enqueue_dma_port_stats_get(s32 cbm_port_id, u32 *occupancy_ctr,
				   u32 *enq_ctr, u32 flags)
{
	if (g_cbm_ops->cbm_enqueue_dma_port_stats_get)
		return g_cbm_ops->cbm_enqueue_dma_port_stats_get(
		cbm_port_id,
		occupancy_ctr,
		enq_ctr,
		flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_enqueue_dma_port_stats_get);

s32 cbm_dp_port_dealloc(struct module *owner, u32 dev_port, s32 cbm_port_id,
			struct cbm_dp_alloc_data *data, u32 flags)
{
	if (g_cbm_ops->cbm_dp_port_dealloc)
		return g_cbm_ops->cbm_dp_port_dealloc(owner, dev_port,
						      cbm_port_id, data, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dp_port_dealloc);

void set_lookup_qid_via_index(u32 index, u32 qid)
{
	if (g_cbm_ops->set_lookup_qid_via_index)
		g_cbm_ops->set_lookup_qid_via_index(index, qid);
}
EXPORT_SYMBOL(set_lookup_qid_via_index);

uint8_t get_lookup_qid_via_index(u32 index)
{
	if (g_cbm_ops->get_lookup_qid_via_index)
		return g_cbm_ops->get_lookup_qid_via_index(index);
	else
		return 0;
}
EXPORT_SYMBOL(get_lookup_qid_via_index);

int cbm_q_thres_get(u32 *length)
{
	if (g_cbm_ops->cbm_q_thres_get)
		return g_cbm_ops->cbm_q_thres_get(length);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_q_thres_get);

int cbm_q_thres_set(u32 length)
{
	if (g_cbm_ops->cbm_q_thres_set)
		return g_cbm_ops->cbm_q_thres_set(length);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_q_thres_set);

s32 cbm_enqueue_mgr_ctrl_get(cbm_eqm_ctrl_t *ctrl, u32 flags)
{
	if (g_cbm_ops->cbm_enqueue_mgr_ctrl_get)
		return g_cbm_ops->cbm_enqueue_mgr_ctrl_get(ctrl, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_enqueue_mgr_ctrl_get);

s32 cbm_enqueue_mgr_ctrl_set(cbm_eqm_ctrl_t *ctrl, u32 flags)
{
	if (g_cbm_ops->cbm_enqueue_mgr_ctrl_set)
		return g_cbm_ops->cbm_enqueue_mgr_ctrl_set(ctrl, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_enqueue_mgr_ctrl_set);

s32 cbm_dequeue_mgr_ctrl_get(cbm_dqm_ctrl_t *ctrl, u32 flags)
{
	if (g_cbm_ops->cbm_dequeue_mgr_ctrl_get)
		return g_cbm_ops->cbm_dequeue_mgr_ctrl_get(ctrl, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dequeue_mgr_ctrl_get);

s32 cbm_dequeue_mgr_ctrl_set(cbm_dqm_ctrl_t *ctrl, u32 flags)
{
	if (g_cbm_ops->cbm_dequeue_mgr_ctrl_set)
		return g_cbm_ops->cbm_dequeue_mgr_ctrl_set(ctrl, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_dequeue_mgr_ctrl_set);

s32 cbm_igp_delay_set(s32 cbm_port_id, s32 delay)
{
	if (g_cbm_ops->cbm_igp_delay_set)
		return g_cbm_ops->cbm_igp_delay_set(cbm_port_id, delay);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_igp_delay_set);

s32
cbm_igp_delay_get(s32 cbm_port_id, s32 *delay)
{
	if (g_cbm_ops->cbm_igp_delay_get)
		return g_cbm_ops->cbm_igp_delay_get(cbm_port_id, delay);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_igp_delay_get);

s32
cbm_queue_delay_enable_set(s32 enable, s32 queue)
{
	if (g_cbm_ops->cbm_queue_delay_enable_set)
		return g_cbm_ops->cbm_queue_delay_enable_set(enable, queue);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_queue_delay_enable_set);

int cbm_counter_mode_set(int enq, int mode)
{
	if (g_cbm_ops->cbm_counter_mode_set)
		return g_cbm_ops->cbm_counter_mode_set(enq, mode);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_counter_mode_set);

int cbm_counter_mode_get(int enq, int *mode)
{
	if (g_cbm_ops->cbm_counter_mode_get)
		return g_cbm_ops->cbm_counter_mode_get(enq, mode);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_counter_mode_get);

u8 get_lookup_qid_via_bits(u32 ep, u32 classid, u32 mpe1, u32 mpe2, u32 enc,
			   u32 dec, u8 flow_id, u32 dic)
{
	if (g_cbm_ops->get_lookup_qid_via_bits)
		return g_cbm_ops->get_lookup_qid_via_bits(
		ep, classid, mpe1, mpe2, enc, dec, flow_id, dic);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(get_lookup_qid_via_bits);

s32 cbm_cpu_port_get(struct cbm_cpu_port_data *data, u32 flags)
{
	if (g_cbm_ops->cbm_cpu_port_get)
		return g_cbm_ops->cbm_cpu_port_get(
		data, flags);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(cbm_cpu_port_get);

s32 pib_program_overshoot(u32 overshoot_bytes)
{
	if (g_cbm_ops->pib_program_overshoot)
		return g_cbm_ops->pib_program_overshoot(
		overshoot_bytes);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(pib_program_overshoot);

s32 pib_status_get(struct pib_stat *ctrl)
{
	if (g_cbm_ops->pib_status_get)
		return g_cbm_ops->pib_status_get(
		ctrl);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(pib_status_get);

s32 pib_ovflw_cmd_get(u32 *cmd)
{
	if (g_cbm_ops->pib_ovflw_cmd_get)
		return g_cbm_ops->pib_ovflw_cmd_get(
		cmd);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(pib_ovflw_cmd_get);

s32 pib_illegal_cmd_get(u32 *cmd)
{
	if (g_cbm_ops->pib_illegal_cmd_get)
		return g_cbm_ops->pib_illegal_cmd_get(
		cmd);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(pib_illegal_cmd_get);

s32 pon_deq_cntr_get(int port, u32 *count)
{
	if (g_cbm_ops->pon_deq_cntr_get)
		return g_cbm_ops->pon_deq_cntr_get(
		port, count);
	else
		return CBM_FAILURE;
}
EXPORT_SYMBOL(pon_deq_cntr_get);

void register_cbm(const struct cbm_ops *cbm_cb)
{
	g_cbm_ops = cbm_cb;
}
