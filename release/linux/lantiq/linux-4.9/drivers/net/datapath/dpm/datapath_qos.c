#include <net/datapath_api.h>
#include <net/datapath_api_qos.h>
#include "datapath.h"

int dp_node_link_add(struct dp_node_link *info, int flag)

{
	if (!dp_port_prop[info->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[info->inst].info.dp_qos_platform_set(NODE_LINK_ADD,
								info, flag);
}
EXPORT_SYMBOL(dp_node_link_add);

int dp_node_unlink(struct dp_node_link *info, int flag)
{
	if (!dp_port_prop[info->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[info->inst].info.dp_qos_platform_set(NODE_UNLINK,
								info, flag);
}
EXPORT_SYMBOL(dp_node_unlink);

int dp_node_link_get(struct dp_node_link *info, int flag)
{
	if (!dp_port_prop[info->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[info->inst].info.dp_qos_platform_set(NODE_LINK_GET,
								info, flag);
}
EXPORT_SYMBOL(dp_node_link_get);

int dp_node_link_en_set(struct dp_node_link_enable *en, int flag)
{
	if (!dp_port_prop[en->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[en->inst].info.dp_qos_platform_set(NODE_LINK_EN_SET,
								en, flag);
}
EXPORT_SYMBOL(dp_node_link_en_set);

int dp_node_link_en_get(struct dp_node_link_enable *en, int flag)
{
	if (!dp_port_prop[en->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[en->inst].info.dp_qos_platform_set(NODE_LINK_EN_GET,
								en, flag);
}
EXPORT_SYMBOL(dp_node_link_en_get);

int dp_link_add(struct dp_qos_link *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(LINK_ADD,
								cfg, flag);
}
EXPORT_SYMBOL(dp_link_add);

int dp_link_get(struct dp_qos_link *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(LINK_GET,
								cfg, flag);
}
EXPORT_SYMBOL(dp_link_get);

int dp_qos_link_prio_set(struct dp_node_prio *info, int flag)
{
	if (!dp_port_prop[info->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[info->inst].info.dp_qos_platform_set(LINK_PRIO_SET,
								info, flag);
}
EXPORT_SYMBOL(dp_qos_link_prio_set);

int dp_qos_link_prio_get(struct dp_node_prio *info, int flag)
{
	if (!dp_port_prop[info->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[info->inst].info.dp_qos_platform_set(LINK_PRIO_GET,
								info, flag);
}
EXPORT_SYMBOL(dp_qos_link_prio_get);

int dp_queue_conf_set(struct dp_queue_conf *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(QUEUE_CFG_SET,
								cfg, flag);
}
EXPORT_SYMBOL(dp_queue_conf_set);

int dp_queue_conf_get(struct dp_queue_conf *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(QUEUE_CFG_GET,
								cfg, flag);
}
EXPORT_SYMBOL(dp_queue_conf_get);

int dp_shaper_conf_set(struct dp_shaper_conf *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(SHAPER_SET,
								cfg, flag);
}
EXPORT_SYMBOL(dp_shaper_conf_set);

int dp_shaper_conf_get(struct dp_shaper_conf *cfg, int flag)
{
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(SHAPER_GET,
								cfg, flag);
}
EXPORT_SYMBOL(dp_shaper_conf_get);

int dp_node_alloc(struct dp_node_alloc *node, int flag)
{
	if (!dp_port_prop[node->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[node->inst].info.dp_qos_platform_set(NODE_ALLOC,
								node, flag);
}
EXPORT_SYMBOL(dp_node_alloc);

int dp_node_free(struct dp_node_alloc *node, int flag)
{
	if (!dp_port_prop[node->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[node->inst].info.dp_qos_platform_set(NODE_FREE,
								node, flag);
}
EXPORT_SYMBOL(dp_node_free);

int dp_node_children_free(struct dp_node_alloc *node, int flag)
{
	if (!dp_port_prop[node->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[node->inst].info.dp_qos_platform_set(
					NODE_CHILDREN_FREE, node, flag);
}
EXPORT_SYMBOL(dp_node_children_free);

int dp_deq_port_res_get(struct dp_dequeue_res *res, int flag)
{
	dp_subif_t *subif;
	struct pmac_port_info *port_info;

	if (!res || !dp_port_prop[res->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	if (res->dev) { /*fill dp_port if dev is provided */
		subif = kzalloc(sizeof(*subif), GFP_ATOMIC);
		if (!subif)
			return DP_FAILURE;
		dp_get_netif_subifid(res->dev, NULL, NULL, NULL, subif, 0);
		if (!subif->subif_num) {
			pr_err("Not found dev %s\n", res->dev->name);
			return DP_FAILURE;
		}
		res->dp_port = subif->port_id;
		kfree(subif);
		subif = NULL;
	}
	port_info = get_dp_port_info(res->inst, res->dp_port);
	DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
		 "dp_deq_port_res_get: dp_port=%d tconf_idx=%d\n",
		 res->dp_port, res->cqm_deq_idx);
	return dp_port_prop[res->inst].info.dp_qos_platform_set(
					DEQ_PORT_RES_GET, res, flag);
}
EXPORT_SYMBOL(dp_deq_port_res_get);

int dp_counter_mode_set(struct dp_counter_conf *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(
					COUNTER_MODE_SET, cfg, flag);
}
EXPORT_SYMBOL(dp_counter_mode_set);

int dp_counter_mode_get(struct dp_counter_conf *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(
					COUNTER_MODE_GET, cfg, flag);
}
EXPORT_SYMBOL(dp_counter_mode_get);

int dp_queue_map_set(struct dp_queue_map_set *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(QUEUE_MAP_SET,
								cfg, flag);
}
EXPORT_SYMBOL(dp_queue_map_set);

int dp_queue_map_get(struct dp_queue_map_get *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(QUEUE_MAP_GET,
								cfg, flag);
}
EXPORT_SYMBOL(dp_queue_map_get);

int dp_children_get(struct dp_node_child *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(
					NODE_CHILDREN_GET, cfg, flag);
}
EXPORT_SYMBOL(dp_children_get);

int dp_qos_level_get(struct dp_qos_level *cfg, int flag)
{
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(QOS_LEVEL_GET,
								cfg, flag);
}
EXPORT_SYMBOL(dp_qos_level_get);

int dp_qos_get_q_logic(struct dp_qos_q_logic *cfg, int flag)
{
	if (!cfg)
		return DP_FAILURE;
	if (!dp_port_prop[cfg->inst].info.dp_qos_platform_set) {
		cfg->q_logic_id = cfg->q_id; /* For GRX500 */
		return DP_SUCCESS;
	}
	return dp_port_prop[cfg->inst].info.dp_qos_platform_set(QOS_Q_LOGIC,
								cfg, flag);
}
EXPORT_SYMBOL(dp_qos_get_q_logic);

int dp_qos_global_info_get(struct dp_qos_cfg_info *info, int flag)
{
	if (!dp_port_prop[info->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[info->inst].info.dp_qos_platform_set(
					QOS_GLOBAL_CFG_GET, info, flag);
}
EXPORT_SYMBOL(dp_qos_global_info_get);

int dp_qos_port_conf_set(struct dp_port_cfg_info *info, int flag)
{
	if (!dp_port_prop[info->inst].info.dp_qos_platform_set)
		return DP_FAILURE;
	return dp_port_prop[info->inst].info.dp_qos_platform_set(
					QOS_PORT_CFG_SET, info, flag);
}
EXPORT_SYMBOL(dp_qos_port_conf_set);
