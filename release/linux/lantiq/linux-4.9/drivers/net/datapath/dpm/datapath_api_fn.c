/******************************************************************************
 * Copyright (c) 2021, MaxLinear, Inc.
 *
 ******************************************************************************/

#include <net/datapath_api.h>
#include <linux/version.h>

int (*dp_rx_enable_fn)(struct net_device *netif, char *ifname, u32 flags) = NULL;
EXPORT_SYMBOL(dp_rx_enable_fn);

int (*dp_shaper_conf_set_fn)(struct dp_shaper_conf *cfg, int flag) = NULL;
EXPORT_SYMBOL(dp_shaper_conf_set_fn);

int (*dp_shaper_conf_get_fn)(struct dp_shaper_conf *cfg, int flag) = NULL;
EXPORT_SYMBOL(dp_shaper_conf_get_fn);

int (*dp_node_link_add_fn)(struct dp_node_link *info, int flag) = NULL;
EXPORT_SYMBOL(dp_node_link_add_fn);

int (*dp_queue_map_set_fn)(struct dp_queue_map_set *cfg, int flag) = NULL;
EXPORT_SYMBOL(dp_queue_map_set_fn);

int (*dp_queue_map_get_fn)(struct dp_queue_map_get *cfg, int flag) = NULL;
EXPORT_SYMBOL(dp_queue_map_get_fn);

int (*dp_vlan_set_fn)(struct dp_tc_vlan *vlan, int flags) = NULL;
EXPORT_SYMBOL(dp_vlan_set_fn);

int (*dp_qos_port_conf_set_fn)(struct dp_port_cfg_info *info, int flag) = NULL;
EXPORT_SYMBOL(dp_qos_port_conf_set_fn);

int (*dp_qos_link_prio_set_fn)(struct dp_node_prio *info, int flag) = NULL;
EXPORT_SYMBOL(dp_qos_link_prio_set_fn);

int (*dp_qos_link_prio_get_fn)(struct dp_node_prio *info, int flag) = NULL;
EXPORT_SYMBOL(dp_qos_link_prio_get_fn);

int (*dp_queue_conf_get_fn)(struct dp_queue_conf *cfg, int flag) = NULL;
EXPORT_SYMBOL(dp_queue_conf_get_fn);

int (*dp_queue_conf_set_fn)(struct dp_queue_conf *cfg, int flag) = NULL;
EXPORT_SYMBOL(dp_queue_conf_set_fn);

int (*dp_ingress_ctp_tc_map_set_fn)(struct dp_tc_cfg *tc, int flag) = NULL;
EXPORT_SYMBOL(dp_ingress_ctp_tc_map_set_fn);

int (*dp_deq_port_res_get_fn)(struct dp_dequeue_res *res, int flag) = NULL;
EXPORT_SYMBOL(dp_deq_port_res_get_fn);

int (*dp_node_free_fn)(struct dp_node_alloc *node, int flag) = NULL;
EXPORT_SYMBOL(dp_node_free_fn);

int (*dp_node_alloc_fn)(struct dp_node_alloc *node, int flag) = NULL;
EXPORT_SYMBOL(dp_node_alloc_fn);

int (*dp_node_unlink_fn)(struct dp_node_link *info, int flag) = NULL;
EXPORT_SYMBOL(dp_node_unlink_fn);

int (*dp_meter_alloc_fn)(int inst, int *meterid, int flag) = NULL;
EXPORT_SYMBOL(dp_meter_alloc_fn);

int (*dp_meter_add_fn)(struct net_device *dev, struct dp_meter_cfg *meter,
		       int flag) = NULL;
EXPORT_SYMBOL(dp_meter_add_fn);

int (*dp_meter_del_fn)(struct net_device *dev, struct dp_meter_cfg *meter,
		       int flag) = NULL;
EXPORT_SYMBOL(dp_meter_del_fn);

int (*dp_set_bp_attr_fn)(struct dp_bp_attr *conf, u32 flag) = NULL;
EXPORT_SYMBOL(dp_set_bp_attr_fn);

int (*dp_get_mtu_size_fn)(struct net_device *dev, u32 *mtu_size) = NULL;
EXPORT_SYMBOL(dp_get_mtu_size_fn);

int32_t (*dp_alloc_port_fn)(struct module *owner, struct net_device *dev,
			 u32 dev_port, int32_t port_id,
			 dp_pmac_cfg_t *pmac_cfg, u32 flags) = NULL;
EXPORT_SYMBOL(dp_alloc_port_fn);

int32_t (*dp_alloc_port_ext_fn)(int inst, struct module *owner,
				struct net_device *dev,
				u32 dev_port, int32_t port_id,
				dp_pmac_cfg_t *pmac_cfg,
				struct dp_port_data *data, u32 flags) = NULL;
EXPORT_SYMBOL(dp_alloc_port_ext_fn);

int32_t (*dp_register_dev_fn)(struct module *owner, u32 port_id,
			      dp_cb_t *dp_cb, u32 flags) = NULL;
EXPORT_SYMBOL(dp_register_dev_fn);

int32_t (*dp_register_dev_ext_fn)(int inst, struct module *owner, u32 port_id,
			       dp_cb_t *dp_cb, struct dp_dev_data *data,
			       u32 flags) = NULL;
EXPORT_SYMBOL(dp_register_dev_ext_fn);

int32_t (*dp_register_subif_fn)(struct module *owner, struct net_device *dev,
				char *subif_name, dp_subif_t *subif_id,
				u32 flags) = NULL;
EXPORT_SYMBOL(dp_register_subif_fn);

int32_t (*dp_register_subif_ext_fn)(int inst, struct module *owner,
				    struct net_device *dev,
				    char *subif_name, dp_subif_t *subif_id,
				struct dp_subif_data *data, u32 flags) = NULL;
EXPORT_SYMBOL(dp_register_subif_ext_fn);

int (*dp_pce_rule_add_fn)(struct dp_pce_blk_info *pce_blk_info,
			  GSW_PCE_rule_t *pce) = NULL;
EXPORT_SYMBOL(dp_pce_rule_add_fn);

int (*dp_pce_rule_del_fn)(struct dp_pce_blk_info *pce_blk_info,
			  GSW_PCE_ruleDelete_t *pce) = NULL;
EXPORT_SYMBOL(dp_pce_rule_del_fn);

int (*dp_register_event_cb_fn)(struct dp_event *info, u32 flag);
EXPORT_SYMBOL(dp_register_event_cb_fn);

int32_t (*dp_xmit_fn)(struct net_device *rx_if, dp_subif_t *rx_subif,
		struct sk_buff *skb, int32_t len, u32 flags) = NULL;
EXPORT_SYMBOL(dp_xmit_fn);

int32_t (*dp_get_netif_subifid_fn)(struct net_device *netif, struct sk_buff *skb,
				   void *subif_data, u8 dst_mac[DP_MAX_ETH_ALEN],
				   dp_subif_t *subif, u32 flags) = NULL;
EXPORT_SYMBOL(dp_get_netif_subifid_fn);

bool (*dp_is_pmapper_check_fn)(struct net_device *dev) = NULL;
EXPORT_SYMBOL(dp_is_pmapper_check_fn);

u32 (*dp_get_tx_cbm_pkt_fn)(int inst, int port_id, int subif_id_grp) = NULL;
EXPORT_SYMBOL(dp_get_tx_cbm_pkt_fn);

int (*dp_register_ops_fn)(int inst, enum DP_OPS_TYPE type, void *ops) = NULL;
EXPORT_SYMBOL(dp_register_ops_fn);

int32_t (*dp_rx_fn)(struct sk_buff *skb, u32 flags) = NULL;
EXPORT_SYMBOL(dp_rx_fn);

int (*dp_qos_get_q_qocc_fn)(struct dp_qos_queue_info *info, int flag) = NULL;
EXPORT_SYMBOL(dp_qos_get_q_qocc_fn);

int (*dp_split_buffer_fn)(char *buffer, char *array[], int max_param_num) = NULL;
EXPORT_SYMBOL(dp_split_buffer_fn);

void (*dp_replace_ch_fn)(char *p, int len, char orig_ch, char new_ch) = NULL;
EXPORT_SYMBOL(dp_replace_ch_fn);

int (*dp_atoi_fn)(unsigned char *str) = NULL;
EXPORT_SYMBOL(dp_atoi_fn);

int (*dp_strncmpi_fn)(const char *s1, const char *s2, size_t n) = NULL;
EXPORT_SYMBOL(dp_strncmpi_fn);

int (*dp_get_reinsert_cnt_fn)(int inst, int dp_port, int vap, int flag,
			      struct dp_reinsert_count *dp_reins_count) = NULL;
EXPORT_SYMBOL(dp_get_reinsert_cnt_fn);

int (*dp_get_lct_cnt_fn)(int inst, int dp_port, int flag,
                      u32 *lct_idx, struct dp_lct_rx_cnt *dp_lct_rx_count);
EXPORT_SYMBOL(dp_get_lct_cnt_fn);

bool (*dp_is_ready_fn)(void) = NULL;
EXPORT_SYMBOL(dp_is_ready_fn);

int (*dp_get_port_prop_fn)(int inst, int port_id, struct dp_port_prop *prop) = NULL;
EXPORT_SYMBOL(dp_get_port_prop_fn);

int (*dp_get_subif_prop_fn)(int inst, int port_id, int vap, struct dp_subif_prop *prop) = NULL;
EXPORT_SYMBOL(dp_get_subif_prop_fn);

void * (*dp_get_ops_fn)(int inst, enum DP_OPS_TYPE type) = NULL;
EXPORT_SYMBOL(dp_get_ops_fn);

int (*dp_set_mtu_size_fn)(struct net_device *dev, u32 mtu_size) = NULL;
EXPORT_SYMBOL(dp_set_mtu_size_fn);

int (*dp_spl_conn_fn)(int inst, struct dp_spl_cfg *conn) = NULL;
EXPORT_SYMBOL(dp_spl_conn_fn);

int (*dp_register_tx_fn)(enum DP_TX_PRIORITY priority, tx_fn fn, void *priv) = NULL;
EXPORT_SYMBOL(dp_register_tx_fn);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
int (*pon_qos_setup_tc_fn)(struct net_device *dev,
			   u32 handle,
			   __be16 protocol,
			   struct tc_to_netdev *tc,
			   int port_id,
			   int deq_idx) = NULL;
#else
int (*pon_qos_setup_tc_fn)(struct net_device *dev,
			   enum tc_setup_type type,
			   void *type_data,
			   int port_id,
			   int deq_idx) = NULL;
#endif
EXPORT_SYMBOL(pon_qos_setup_tc_fn);

#define MAX_EVENTS 10
static int num_event;
struct event_late_init_list {
	struct dp_event info;
	u32 flag; /*callback 2nd parameter */
};
struct event_late_init_list late_event_list[MAX_EVENTS];
int dp_register_event_cb(struct dp_event *info, u32 flag)
{
	if (!dp_is_ready()) {
		if (flag & DP_F_DEREGISTER) {
			pr_err("Not support de-register before DPM ready\n");
			return DP_FAILURE;
		}
		if (!info) {
			pr_err("info NULL:%s\n", __func__);
			return DP_FAILURE;
		}

		/* workaround in case some module build-in kernel but DPM in ko */
		if (num_event >= MAX_EVENTS) {
			pr_err("dpm event > capability of %d\n", MAX_EVENTS);
			return DP_FAILURE;
		}
		late_event_list[num_event].info = *info;
		late_event_list[num_event].flag = flag;
		num_event++;
		/* return one dummy id since DPM not ready yet */
		*(unsigned long *)&info->id = (unsigned long)num_event;
		return DP_SUCCESS;
	} else if (dp_register_event_cb_fn) {
		if (flag & DP_F_DEREGISTER) { /*retrieve real info->id */
			unsigned long id = (unsigned long) info->id;
			if (id == 0) {
				pr_err("wrong zero ID:de-register_event_cb\n");
				return DP_FAILURE;
			} 
			if ((id > 0) && (id < MAX_EVENTS)) {
				info->id = late_event_list[id - 1].info.id;
				late_event_list[id - 1].info.id = NULL;
			}
		}
		return dp_register_event_cb_fn(info, flag);
	}
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_register_event_cb);


int32_t dp_xmit(struct net_device *rx_if, dp_subif_t *rx_subif,
		struct sk_buff *skb, int32_t len, u32 flags)
{
	if (dp_xmit_fn)
		return dp_xmit_fn(rx_if, rx_subif, skb, len, flags);
	pr_err("why %s associated function pointer NULL\n", __func__);
	dev_kfree_skb_any(skb);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_xmit);

int32_t dp_get_netif_subifid(struct net_device *netif, struct sk_buff *skb,
			     void *subif_data, u8 dst_mac[DP_MAX_ETH_ALEN],
			     dp_subif_t *subif, u32 flags)
{
	if (dp_get_netif_subifid_fn)
		return dp_get_netif_subifid_fn(netif, skb, subif_data, dst_mac,
					       subif, flags);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_get_netif_subifid);

bool dp_is_pmapper_check(struct net_device *dev)
{
	if (dp_is_pmapper_check_fn)
		return dp_is_pmapper_check_fn(dev);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return false;
}
EXPORT_SYMBOL(dp_is_pmapper_check);

int dp_get_port_prop(int inst, int port_id, struct dp_port_prop *prop)
{
	if (dp_get_port_prop_fn)
		return dp_get_port_prop_fn(inst, port_id, prop);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_get_port_prop);

int dp_get_subif_prop(int inst, int port_id, int vap,
		      struct dp_subif_prop *prop)
{
	if (dp_get_subif_prop_fn)
		return dp_get_subif_prop_fn(inst, port_id, vap, prop);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_get_subif_prop);

u32 dp_get_tx_cbm_pkt(int inst, int port_id, int subif_id_grp)
{
	if (dp_get_tx_cbm_pkt_fn)
		return dp_get_tx_cbm_pkt_fn(inst, port_id, subif_id_grp);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return 0;
}
EXPORT_SYMBOL(dp_get_tx_cbm_pkt);

int32_t dp_rx(struct sk_buff *skb, u32 flags)
{
	if (dp_rx_fn)
		return dp_rx_fn(skb, flags);
	dev_kfree_skb_any(skb);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_rx);

#define DP_MAX_OPS  10
struct ops_late_init_list {
	int inst;
	enum DP_OPS_TYPE type;
	void *ops;
};
static struct ops_late_init_list ops_late_init[DP_MAX_INST][DP_MAX_OPS];
static int ops_num[DP_MAX_INST];
int dp_register_ops(int inst, enum DP_OPS_TYPE type, void *ops)
{
	int idx;

	if (!dp_is_ready()) {
		idx = ops_num[inst];
		/* workaround in case some module build-in kernel but DPM in ko */
		if (idx >= DP_MAX_OPS) {
			pr_err("ops more than capability of %d\n", DP_MAX_OPS);
			return DP_FAILURE;
		}

		ops_late_init[inst][idx].ops = ops;
		ops_late_init[inst][idx].type = type;
		ops_num[inst]++;
		pr_debug("dp_register_ops num[%d]=%d\n", inst, ops_num[inst]);
		return DP_SUCCESS;
	} else if (dp_register_ops_fn)
		return dp_register_ops_fn(inst, type, ops);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_register_ops);

int dp_qos_get_q_qocc(struct dp_qos_queue_info *info, int flag)
{
	if (dp_qos_get_q_qocc_fn)
		return dp_qos_get_q_qocc_fn(info, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_qos_get_q_qocc);

int dp_split_buffer(char *buffer, char *array[], int max_param_num)
{
	if (dp_split_buffer_fn)
		return dp_split_buffer_fn(buffer, array, max_param_num);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_split_buffer);

void dp_replace_ch(char *p, int len, char orig_ch, char new_ch)
{
	if (dp_replace_ch_fn)
		return dp_replace_ch_fn(p, len, orig_ch, new_ch);
	pr_err("why %s associated function pointer NULL\n", __func__);
}
EXPORT_SYMBOL(dp_replace_ch);

int dp_atoi(unsigned char *str)
{
	if (dp_atoi_fn)
		return dp_atoi_fn(str);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_atoi);

int dp_strncmpi(const char *s1, const char *s2, size_t n)
{
	if (dp_strncmpi_fn)
		return dp_strncmpi_fn(s1, s2, n);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_strncmpi);

int dp_get_reinsert_cnt(int inst, int dp_port, int vap, int flag,
			struct dp_reinsert_count *dp_reins_count)
{
	if (dp_get_reinsert_cnt_fn)
		return dp_get_reinsert_cnt_fn(inst, dp_port, vap, flag,
					      dp_reins_count);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_get_reinsert_cnt);

int dp_get_lct_cnt(int inst, int dp_port, int flag,
                   u32 *lct_idx, struct dp_lct_rx_cnt *dp_lct_rx_count)
{
	if (dp_get_lct_cnt_fn)
		return dp_get_lct_cnt_fn(inst, dp_port, flag, lct_idx,
					      dp_lct_rx_count);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_get_lct_cnt);

int dp_get_mtu_size(struct net_device *dev, u32 *mtu_size)
{
	if (dp_get_mtu_size_fn)
		return dp_get_mtu_size_fn(dev, mtu_size);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_get_mtu_size);

int32_t dp_alloc_port(struct module *owner, struct net_device *dev,
		      u32 dev_port, int32_t port_id,
		      dp_pmac_cfg_t *pmac_cfg, u32 flags)
{
	if (dp_alloc_port_fn)
		return dp_alloc_port_fn(owner, dev, dev_port, port_id,
					pmac_cfg, flags);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_alloc_port);

int32_t dp_alloc_port_ext(int inst, struct module *owner,
			  struct net_device *dev,
			  u32 dev_port, int32_t port_id,
			  dp_pmac_cfg_t *pmac_cfg,
			  struct dp_port_data *data, u32 flags)
{
	if (dp_alloc_port_ext_fn)
		return dp_alloc_port_ext_fn(inst, owner, dev, dev_port, port_id,
					    pmac_cfg, data, flags);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_alloc_port_ext);

int32_t dp_register_dev(struct module *owner, u32 port_id,
				 dp_cb_t *dp_cb, u32 flags)
{
	if (dp_register_dev_fn)
		return dp_register_dev_fn(owner, port_id, dp_cb, flags);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_register_dev);

int32_t dp_register_dev_ext(int inst, struct module *owner, u32 port_id,
			    dp_cb_t *dp_cb, struct dp_dev_data *data,
			    u32 flags)
{
	if (dp_register_dev_ext_fn)
		return dp_register_dev_ext_fn(inst, owner, port_id, dp_cb, data,
					      flags);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_register_dev_ext);

int32_t dp_register_subif(struct module *owner, struct net_device *dev,
			  char *subif_name, dp_subif_t *subif_id,
			  u32 flags)
{
	if (dp_register_subif_fn)
		return dp_register_subif_fn(owner, dev, subif_name, subif_id,
					    flags);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_register_subif);

int32_t dp_register_subif_ext(int inst, struct module *owner,
			      struct net_device *dev,
			      char *subif_name, dp_subif_t *subif_id,
			      /*device related info*/
			      struct dp_subif_data *data, u32 flags)
{
	if (dp_register_subif_ext_fn)
		return dp_register_subif_ext_fn(inst, owner, dev, subif_name,
						subif_id, data, flags);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_register_subif_ext);

int dp_set_bp_attr(struct dp_bp_attr *conf, u32 flag)
{
	if (dp_set_bp_attr_fn)
		return dp_set_bp_attr_fn(conf, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_set_bp_attr);

int dp_rx_enable(struct net_device *netif, char *ifname, u32 flags)
{
	if (dp_rx_enable_fn)
		return dp_rx_enable_fn(netif, ifname, flags);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_rx_enable);

int dp_shaper_conf_set(struct dp_shaper_conf *cfg, int flag)
{
	if (dp_shaper_conf_set_fn)
		return dp_shaper_conf_set_fn(cfg, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_shaper_conf_set);

int dp_shaper_conf_get(struct dp_shaper_conf *cfg, int flag)
{
	if (dp_shaper_conf_get_fn)
		return dp_shaper_conf_get_fn(cfg, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_shaper_conf_get);

int dp_node_link_add(struct dp_node_link *info, int flag)
{
	if (dp_node_link_add_fn)
		return dp_node_link_add_fn(info, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_node_link_add);

int dp_queue_map_set(struct dp_queue_map_set *cfg, int flag)
{
	if (dp_queue_map_set_fn)
		return dp_queue_map_set_fn(cfg, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_queue_map_set);

int dp_queue_map_get(struct dp_queue_map_get *cfg, int flag)
{
	if (dp_queue_map_get_fn)
		return dp_queue_map_get_fn(cfg, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_queue_map_get);

int dp_vlan_set(struct dp_tc_vlan *vlan, int flags)
{
	if (dp_vlan_set_fn)
		return dp_vlan_set_fn(vlan, flags);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_vlan_set);

int dp_qos_port_conf_set(struct dp_port_cfg_info *info, int flag)
{
	if (dp_qos_port_conf_set_fn)
		return dp_qos_port_conf_set_fn(info, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_qos_port_conf_set);

int dp_qos_link_prio_set(struct dp_node_prio *info, int flag)
{
	if (dp_qos_link_prio_set_fn)
		return dp_qos_link_prio_set_fn(info, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_qos_link_prio_set);

int dp_queue_conf_get(struct dp_queue_conf *cfg, int flag)
{
	if (dp_queue_conf_get_fn)
		return dp_queue_conf_get_fn(cfg, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_queue_conf_get);

int dp_ingress_ctp_tc_map_set(struct dp_tc_cfg *tc, int flag)
{
	if (dp_ingress_ctp_tc_map_set_fn)
		return dp_ingress_ctp_tc_map_set_fn(tc, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_ingress_ctp_tc_map_set);

int dp_deq_port_res_get(struct dp_dequeue_res *res, int flag)
{
	if (dp_deq_port_res_get_fn)
		return dp_deq_port_res_get_fn(res, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_deq_port_res_get);

int dp_qos_link_prio_get(struct dp_node_prio *info, int flag)
{
	if (dp_qos_link_prio_get_fn)
		return dp_qos_link_prio_get_fn(info, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_qos_link_prio_get);

int dp_node_free(struct dp_node_alloc *node, int flag)
{
	if (dp_node_free_fn)
		return dp_node_free_fn(node, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_node_free);

int dp_node_alloc(struct dp_node_alloc *node, int flag)
{
	if (dp_node_alloc_fn)
		return dp_node_alloc_fn(node, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_node_alloc);

int dp_queue_conf_set(struct dp_queue_conf *cfg, int flag)
{
	if (dp_queue_conf_set_fn)
		return dp_queue_conf_set_fn(cfg, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_queue_conf_set);

int dp_node_unlink(struct dp_node_link *info, int flag)
{
	if (dp_node_unlink_fn)
		return dp_node_unlink_fn(info, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_node_unlink);

int dp_meter_alloc(int inst, int *meterid, int flag)
{
	if (dp_meter_alloc_fn)
		return dp_meter_alloc_fn(inst, meterid, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_meter_alloc);

int dp_meter_add(struct net_device *dev, struct dp_meter_cfg *meter, int flag)
{
	if (dp_meter_add_fn)
		return dp_meter_add_fn(dev, meter, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_meter_add);

int dp_meter_del(struct net_device *dev, struct dp_meter_cfg *meter, int flag)
{
	if (dp_meter_del_fn)
		return dp_meter_del_fn(dev, meter, flag);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_meter_del);

int dp_pce_rule_add(struct dp_pce_blk_info *pce_blk_info,
			     GSW_PCE_rule_t *pce)

{
	if (dp_pce_rule_add_fn)
		return dp_pce_rule_add_fn(pce_blk_info, pce);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_pce_rule_add);

int dp_pce_rule_del(struct dp_pce_blk_info *pce_blk_info,
			     GSW_PCE_ruleDelete_t *pce)
{
	if (dp_pce_rule_del_fn)
		return dp_pce_rule_del_fn(pce_blk_info, pce);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_pce_rule_del);

void *dp_get_ops(int inst, enum DP_OPS_TYPE type)
{
	if (dp_get_ops_fn)
		return dp_get_ops_fn(inst, type);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return NULL;
}
EXPORT_SYMBOL(dp_get_ops);

int dp_set_mtu_size(struct net_device *dev, u32 mtu_size)
{
	if (dp_set_mtu_size_fn)
		return dp_set_mtu_size_fn(dev, mtu_size);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return 1500;
}
EXPORT_SYMBOL(dp_set_mtu_size);

int dp_spl_conn(int inst, struct dp_spl_cfg *conn)
{
	if (dp_spl_conn_fn)
		return dp_spl_conn_fn(inst, conn);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_spl_conn);


#define MAX_TX 10
static int num_tx;
struct tx_late_init_list {
	enum DP_TX_PRIORITY priority;
	tx_fn fn;
	void *priv;
};
struct tx_late_init_list tx_late_init_list[MAX_TX];
int dp_register_tx(enum DP_TX_PRIORITY priority, tx_fn fn, void *priv)
{

	if (!dp_is_ready()) {
		/* workaround in case some module build-in kernel but DPM in ko */
		if (num_tx >= MAX_TX) {
			pr_err("tx cb more than capability of %d\n", MAX_TX);
			return DP_FAILURE;
		}

		tx_late_init_list[num_tx].priority = priority;
		tx_late_init_list[num_tx].fn = fn;
		tx_late_init_list[num_tx].priv = priv;
		num_tx++;
		return DP_SUCCESS;
	} else if (dp_register_tx_fn)
		return dp_register_tx_fn(priority, fn, priv);
	pr_err("why %s associated function pointer NULL\n", __func__);
	return DP_FAILURE;
}
EXPORT_SYMBOL(dp_register_tx);

/* This is workaround now: for example MIB counter moudle is build-in kernel
 * but DPM is in module
 */
bool dp_is_ready(void)
{
	if (dp_is_ready_fn)
		return dp_is_ready_fn();
        pr_err("dp not ready yet\n");
	return false;
}

/* This API will be dpm internally triggered if someone call dp_register_ops
 * before DPM moudle ready
 */
int dp_late_register_ops(void)
{
	int i, j, n = 0;

	if (!dp_is_ready()) {
		pr_err("Why DPM still not ready.\n");
		return DP_FAILURE;
	}
	if (!dp_register_ops_fn) {
		pr_err("why %s associated function pointer NULL\n", __func__);
		return DP_FAILURE;
	}
	
	for (i = 0; i < DP_MAX_INST; i++) {
		for (j = 0; j < ops_num[i]; j++) {
			dp_register_ops_fn(i, ops_late_init[i][j].type,
					   ops_late_init[i][j].ops);
			n++;
		}
	}
	pr_info("dp_late_register_ops: %d ops workaround done\n", n);
	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_late_register_ops);

/* This API will be internally triggered if someone call dp_register_event_cb 
 * before DPM module ready
 */
int dp_late_register_event_cb(void)
{
	int i;

	if (!dp_is_ready()) {
		pr_err("Why DPM still not ready.\n");
		return DP_FAILURE;
	}
	if (!dp_register_event_cb_fn) {
		pr_err("why %s associated function pointer NULL\n", __func__);
		return DP_FAILURE;
	}
	
	for (i = 0; i < num_event; i++)
		dp_register_event_cb_fn(&late_event_list[i].info,
					late_event_list[i].flag);
	pr_info("dp_late_register_event_cb: %d event workaround done\n",
		num_event);

	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_late_register_event_cb);


/* This API will be internally triggered if someone call dp_register_event_cb 
 * before DPM module ready
 */
int dp_late_register_tx(void)
{
	int i;

	if (!dp_is_ready()) {
		pr_err("Why DPM still not ready.\n");
		return DP_FAILURE;
	}
	if (!dp_register_tx_fn) {
		pr_err("why %s associated function pointer NULL\n", __func__);
		return DP_FAILURE;
	}
	
	for (i = 0; i < num_tx; i++)
		dp_register_tx_fn(tx_late_init_list[i].priority,
					tx_late_init_list[i].fn,
					tx_late_init_list[i].priv);
	pr_info("dp_late_register_tx: %d tx cb workaround done\n",
		num_tx);

	return DP_SUCCESS;
}
EXPORT_SYMBOL(dp_late_register_tx);

static int __init dp_init(void)
{
	printk("dp manager dummy initi done\n");
	return 0;
}

static void __exit dp_exit(void)
{
	printk("dpm dummy exit\n");
}

module_init(dp_init);
module_exit(dp_exit);

MODULE_LICENSE("GPL");
