/******************************************************************************
 * Copyright (c) 2021, MaxLinear, Inc.
 *
 ******************************************************************************/

#ifndef DATAPATH_API_FN_H
#define DATAPATH_API_FN_H

#include <linux/version.h>

extern int (*dp_rx_enable_fn)(struct net_device *netif, char *ifname, u32 flags);
extern int (*dp_shaper_conf_set_fn)(struct dp_shaper_conf *cfg, int flag);
extern int (*dp_shaper_conf_get_fn)(struct dp_shaper_conf *cfg, int flag);
extern int (*dp_node_link_add_fn)(struct dp_node_link *info, int flag);
extern int (*dp_queue_map_set_fn)(struct dp_queue_map_set *cfg, int flag);
extern int (*dp_queue_map_get_fn)(struct dp_queue_map_get *cfg, int flag);
extern int (*dp_vlan_set_fn)(struct dp_tc_vlan *vlan, int flags);
extern int (*dp_qos_port_conf_set_fn)(struct dp_port_cfg_info *info, int flag);
extern int (*dp_qos_link_prio_set_fn)(struct dp_node_prio *info, int flag);
extern int (*dp_qos_link_prio_get_fn)(struct dp_node_prio *info, int flag);
extern int (*dp_queue_conf_get_fn)(struct dp_queue_conf *cfg, int flag);
extern int (*dp_queue_conf_set_fn)(struct dp_queue_conf *cfg, int flag);
extern int (*dp_ingress_ctp_tc_map_set_fn)(struct dp_tc_cfg *tc, int flag);
extern int (*dp_deq_port_res_get_fn)(struct dp_dequeue_res *res, int flag);
extern int (*dp_node_free_fn)(struct dp_node_alloc *node, int flag);
extern int (*dp_node_alloc_fn)(struct dp_node_alloc *node, int flag);
extern int (*dp_node_unlink_fn)(struct dp_node_link *info, int flag);
extern int (*dp_meter_alloc_fn)(int inst, int *meterid, int flag);
extern int (*dp_meter_add_fn)(struct net_device *dev, struct dp_meter_cfg *meter,
			      int flag);
extern int (*dp_meter_del_fn)(struct net_device *dev, struct dp_meter_cfg *meter,
			      int flag);
extern int (*dp_set_bp_attr_fn)(struct dp_bp_attr *conf, u32 flag);
extern void (*dp_net_dev_get_ss_stat_strings_fn)(struct net_device *dev, u8 *data);
extern int (*dp_net_dev_get_ss_stat_strings_count_fn)(struct net_device *dev);
extern int (*dp_get_mtu_size_fn)(struct net_device *dev, u32 *mtu_size);
extern int32_t (*dp_alloc_port_fn)(struct module *owner, struct net_device *dev,
				   u32 dev_port, int32_t port_id,
				   dp_pmac_cfg_t *pmac_cfg, u32 flags);
extern int32_t (*dp_alloc_port_ext_fn)(int inst, struct module *owner,
				       struct net_device *dev,
				       u32 dev_port, int32_t port_id,
				       dp_pmac_cfg_t *pmac_cfg,
				       struct dp_port_data *data, u32 flags);
extern int32_t (*dp_register_dev_fn)(struct module *owner, u32 port_id,
				     dp_cb_t *dp_cb, u32 flags);
extern int32_t (*dp_register_dev_ext_fn)(int inst, struct module *owner, u32 port_id,
					 dp_cb_t *dp_cb, 
					 struct dp_dev_data *data,
					 u32 flags);
extern int32_t (*dp_register_subif_fn)(struct module *owner, struct net_device *dev,
				       char *subif_name, dp_subif_t *subif_id,
				       u32 flags);
extern int32_t (*dp_register_subif_ext_fn)(int inst, struct module *owner,
					   struct net_device *dev,
					   char *subif_name,
					   dp_subif_t *subif_id,
					   struct dp_subif_data *data,
					   u32 flags);
extern int (*dp_pce_rule_add_fn)(struct dp_pce_blk_info *pce_blk_info,
				 GSW_PCE_rule_t *pce);
extern int (*dp_pce_rule_del_fn)(struct dp_pce_blk_info *pce_blk_info,
				 GSW_PCE_ruleDelete_t *pce);
extern void (*dp_get_dev_ss_stat_strings_fn)(struct net_device *dev, u8 *data);
extern int (*dp_get_dev_stat_strings_count_fn)(struct net_device *dev);
extern int (*dp_register_event_cb_fn)(struct dp_event *info, u32 flag);
extern int32_t (*dp_xmit_fn)(struct net_device *rx_if, dp_subif_t *rx_subif,
			     struct sk_buff *skb, int32_t len, u32 flags);
extern int32_t (*dp_get_netif_subifid_fn)(struct net_device *netif, struct sk_buff *skb,
					  void *subif_data, 
					  u8 dst_mac[DP_MAX_ETH_ALEN],
					  dp_subif_t *subif, u32 flags);
extern bool (*dp_is_pmapper_check_fn)(struct net_device *dev);
extern u32 (*dp_get_tx_cbm_pkt_fn)(int inst, int port_id, int subif_id_grp);
extern int (*dp_register_ops_fn)(int inst, enum DP_OPS_TYPE type, void *ops);
extern int32_t (*dp_rx_fn)(struct sk_buff *skb, u32 flags);
extern int (*dp_qos_get_q_qocc_fn)(struct dp_qos_queue_info *info, int flag);
extern int (*dp_split_buffer_fn)(char *buffer, char *array[], int max_param_num);
extern void (*dp_replace_ch_fn)(char *p, int len, char orig_ch, char new_ch);
extern int (*dp_atoi_fn)(unsigned char *str);
extern int (*dp_strncmpi_fn)(const char *s1, const char *s2, size_t n);
extern int (*dp_get_reinsert_cnt_fn)(int inst, int dp_port, int vap, int flag,
				     struct dp_reinsert_count *dp_reins_count);
extern int (*dp_get_lct_cnt_fn)(int inst, int dp_port, int flag,
		   		u32 *lct_idx,
				struct dp_lct_rx_cnt *dp_lct_rx_count);
extern bool (*dp_is_ready_fn)(void);
extern int (*dp_get_port_prop_fn)(int inst, int port_id, struct dp_port_prop *prop);
extern int (*dp_get_subif_prop_fn)(int inst, int port_id, int vap, struct dp_subif_prop *prop);
extern void *(*dp_get_ops_fn)(int inst, enum DP_OPS_TYPE type);
extern int (*dp_set_mtu_size_fn)(struct net_device *dev, u32 mtu_size);
extern int (*dp_spl_conn_fn)(int inst, struct dp_spl_cfg *conn);
extern int (*dp_register_tx_fn)(enum DP_TX_PRIORITY priority, tx_fn fn, void *priv);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
extern int32_t (*qos_mgr_hook_setup_tc)(struct net_device *dev, u32 handle,
					__be16 protocol,
					struct tc_to_netdev *tc);
#else
extern int32_t (*qos_mgr_hook_setup_tc)(struct net_device *dev,
					enum tc_setup_type type,
					void *type_data);
#endif

int dp_late_register_ops(void);
int dp_late_register_event_cb(void);

#endif

