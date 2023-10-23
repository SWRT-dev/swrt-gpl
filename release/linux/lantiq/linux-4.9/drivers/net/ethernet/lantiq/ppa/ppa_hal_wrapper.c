/******************************************************************************
 **
 ** FILE NAME	: ppa_hal_wrapper.c
 ** PROJECT	: PPA
 ** MODULES	: PPA Wrapper for HAL Selector
 **
 ** DATE	: 18 Feb 2014
 ** AUTHOR	: Kamal Eradath
 ** DESCRIPTION : PPA Wrapper for HAL Selector layer
 ** COPYRIGHT	: Copyright (c) 2017 Intel Corporation
 ** Copyright (c) 2014 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 ** HISTORY
 ** $Date		$Author				$Comment
 ** 18 FEB 2014  	Kamal Eradath		  Initiate Version
 *******************************************************************************/

/*
 * ####################################
 *			  Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/atmdev.h>
#include <net/sock.h>
/*
 *  Chip Specific Head File
 */
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_hal_api.h>
#include <net/ppa/ppa_hal_wrapper.h>

ppa_generic_hook_t		ppa_drv_hal_hook[MAX_HAL]  =  {NULL};

static PPA_HLIST_HEAD		g_hsel_caps_list[MAX_CAPS];
static PPE_LOCK			g_hsel_caps_lock;

ppa_tunnel_entry		*g_tunnel_table[MAX_TUNNEL_ENTRIES]  =  {NULL};
uint32_t			g_tunnel_counter[MAX_TUNNEL_ENTRIES];
PPE_LOCK			g_tunnel_table_lock;

static uint8_t g_num_registred_hals = 0;

extern int32_t (*ppa_drv_hal_generic_hook)(PPA_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag);

uint8_t ppa_drv_get_num_registred_hals(void)
{
	return g_num_registred_hals;
}
/*****************************************************************************************/
/* HAL registration functions*/
/*****************************************************************************************/
uint32_t ppa_drv_generic_hal_register(uint32_t hal_id, ppa_generic_hook_t generic_hook)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (generic_hook) {
		if (hal_id  ==  PPE_HAL) {/*ppe hal*/
			ppa_drv_hal_generic_hook  =  generic_hook;
		}
		ppa_drv_hal_hook[hal_id]  =  generic_hook;
		g_num_registred_hals++;
		return PPA_SUCCESS;
	}
	return PPA_FAILURE;
}

void ppa_drv_generic_hal_deregister(uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return;

	if (hal_id  ==  PPE_HAL) {/*ppe hal*/
		ppa_drv_hal_generic_hook  =  NULL;
	}
	ppa_drv_hal_hook[hal_id]  =  NULL;
	g_num_registred_hals--;
}

int32_t ppa_select_hals_from_caplist(uint8_t start, uint8_t num_entries, PPA_HSEL_CAP_NODE *caps_list)
{
	PPA_HSEL_CAP_NODE *t_node = NULL;
	PPA_HLIST_NODE *tmp;

	uint32_t i;

	for (i  =  start; i < (start+num_entries); i++) {
		t_node = NULL;
		ppa_hlist_for_each_entry_safe(t_node, tmp, &g_hsel_caps_list[caps_list[i].cap], cap_list) {
			if ((caps_list[i].wt  ==  0) ||  (t_node->wt > caps_list[i].wt)) {
				caps_list[i].hal_id  =  t_node->hal_id;
				caps_list[i].wt  =  t_node->wt;
				break;
			}
		}
		if (t_node  ==  NULL) {
			return PPA_FAILURE;
		}
	}
	return PPA_SUCCESS;
}

int32_t ppa_group_hals_in_capslist(uint8_t start, uint8_t num_entries, PPA_HSEL_CAP_NODE *caps_list)
{
	uint32_t j = 1;
	if ((start+j) < num_entries) {
		while (caps_list[start].hal_id  ==  caps_list[start+j].hal_id) {
			j++;
		}
	}
	return j;
}

static void ppa_insert_hsel_cap_node(PPA_HSEL_CAP_NODE *cap_node)
{
	PPA_HSEL_CAP_NODE *t_node = NULL, *t1_node = NULL;
	PPA_HLIST_NODE *tmp;

	/* check whether it is first entry in the list*/
	if (ppa_hlist_empty(&g_hsel_caps_list[cap_node->cap])) {
		ppa_hlist_add_head(&cap_node->cap_list, &g_hsel_caps_list[cap_node->cap]);
	} else {
		/* not the first entry so find the location based on wt*/
		ppa_hlist_for_each_entry_safe(t_node, tmp,  &g_hsel_caps_list[cap_node->cap], cap_list) {
			if (t_node->wt > cap_node->wt) {
				ppa_hlist_add_before(&cap_node->cap_list, &t_node->cap_list);
				return;
			}
			t1_node = t_node;
		}
		/* reached the end of the list*/
		if (likely(t1_node)) {
			ppa_hlist_add_after(&cap_node->cap_list, &t1_node->cap_list);
		}
	}
}

uint32_t ppa_drv_register_cap(PPA_API_CAPS cap, uint8_t wt, PPA_HAL_ID hal_id)
{
	PPA_HSEL_CAP_NODE *cap_node;

	if (cap >= MAX_CAPS)
		return PPA_FAILURE;

	/* allocate the node*/
	cap_node  =  (PPA_HSEL_CAP_NODE*) kmalloc (sizeof(PPA_HSEL_CAP_NODE), GFP_KERNEL);
	if (!cap_node) {
		return PPA_FAILURE;
	}
	memset(cap_node, 0, sizeof(PPA_HSEL_CAP_NODE));
	PPA_INIT_HLIST_NODE(&cap_node->cap_list);
	cap_node->wt  =  wt;
	cap_node->cap  =  cap;
	cap_node->hal_id  =  hal_id;

	/* find the location to insert in the list and insert it. based on the ascending order of weight*/
	ppa_insert_hsel_cap_node(cap_node);

	return PPA_SUCCESS;
}

uint32_t ppa_drv_deregister_cap(PPA_API_CAPS cap, PPA_HAL_ID hal_id)
{
	PPA_HSEL_CAP_NODE *t_node = NULL;
	PPA_HLIST_NODE *tmp;

	if (cap >= MAX_CAPS)
		return PPA_FAILURE;

	ppa_hlist_for_each_entry_safe(t_node, tmp, &g_hsel_caps_list[cap], cap_list) {
		if (t_node->hal_id  ==  hal_id) {
			ppa_hlist_del(&t_node->cap_list);
			kfree(t_node);
			return PPA_SUCCESS;
		}
	}
	return PPA_FAILURE;
}

/*****************************************************************************************/
/* wrappers for various hal API*/
/*****************************************************************************************/
uint32_t ppa_hsel_hal_init(PPA_HAL_INIT_CFG *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_INIT, cfg, flag);
}

uint32_t ppa_hsel_hal_exit(uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_EXIT, (void *)NULL, flag);
}

uint32_t ppa_hsel_get_hal_id(PPA_VERSION *v, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_HAL_VERSION, (void *)v, flag);
}

uint32_t ppa_hsel_get_firmware_id(PPA_VERSION *v, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_PPE_FW_VERSION, (void *)v, flag);
}

uint32_t ppa_hsel_get_number_of_phys_port(PPA_COUNT_CFG *count, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_PHYS_PORT_NUM, (void *)count, flag);
}

uint32_t ppa_hsel_get_phys_port_info(PPE_IFINFO *info, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_PHYS_PORT_INFO, (void *)info, flag);
}

uint32_t ppa_hsel_get_max_entries(PPA_MAX_ENTRY_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_MAX_ENTRIES, (void *)entry, flag);
}

uint32_t ppa_hsel_set_route_cfg(PPA_ROUTING_CFG *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_ROUT_CFG, (void *)cfg, flag);
}


uint32_t ppa_hsel_set_bridging_cfg(PPA_BRDG_CFG *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_BRDG_CFG, (void *)cfg, flag);
}

uint32_t ppa_hsel_get_acc_mode(PPA_ACC_ENABLE *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_ACC_ENABLE, (void *)cfg, flag);
}

uint32_t ppa_hsel_set_acc_mode(PPA_ACC_ENABLE *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_ACC_ENABLE, (void *)cfg, flag);
}

uint32_t ppa_hsel_is_ipv6_enabled(uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_IPV6_FLAG, (void *)NULL, flag);
}


uint32_t ppa_hsel_init_qos_cfg(uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_QOS_INIT_CFG, (void *)NULL, flag);
}
uint32_t ppa_hsel_uninit_qos_cfg(uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_QOS_UNINIT_CFG, (void *)NULL, flag);
}
uint32_t ppa_hsel_dscp_class_set(uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_QOS_DSCP_CLASS_MAP_SET, (void *)NULL, flag);
}
EXPORT_SYMBOL(ppa_hsel_dscp_class_set);
uint32_t ppa_hsel_dscp_class_reset(uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_QOS_DSCP_CLASS_MAP_RESET, (void *)NULL, flag);
}
EXPORT_SYMBOL(ppa_hsel_dscp_class_reset);
uint32_t ppa_hsel_add_qos_queue_entry(QOS_Q_ADD_CFG *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_QOS_ADDQUE_CFG, (void *)entry, flag);
}
uint32_t ppa_hsel_modify_qos_queue_entry(QOS_Q_MOD_CFG *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_QOS_MODQUE_CFG, (void *)entry, flag);
}
uint32_t ppa_hsel_delete_qos_queue_entry(QOS_Q_DEL_CFG *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_QOS_DELQUE_CFG, (void *)entry, flag);
}
uint32_t ppa_hsel_set_qos_rate_entry(QOS_RATE_SHAPING_CFG *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_QOS_RATE_SHAPING_CFG, (void *)entry, flag);
}
uint32_t ppa_hsel_reset_qos_rate_entry(QOS_RATE_SHAPING_CFG *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_RESET_QOS_RATE_SHAPING_CFG, (void *)entry, flag);
}
uint32_t ppa_hsel_set_qos_shaper_entry(QOS_RATE_SHAPING_CFG *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_QOS_SHAPER_CFG, (void *)entry, flag);
}

uint32_t ppa_hsel_set_qos_meter_entry(PPA_QOS_METER_CFG *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_QOS_METER_CFG,(void *)entry, flag);
}
EXPORT_SYMBOL(ppa_hsel_set_qos_meter_entry);

uint32_t ppa_hsel_mod_subif_port_cfg(QOS_MOD_SUBIF_PORT_CFG *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_MOD_SUBIF_PORT_CFG, (void *)entry, flag);
}

uint32_t ppa_hsel_add_complement(PPA_ROUTING_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_COMPLEMENT_ENTRY, (void *)entry, flag);
}
EXPORT_SYMBOL(ppa_hsel_add_complement);

uint32_t ppa_hsel_del_complement(PPA_ROUTING_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_COMPLEMENT_ENTRY, (void *)entry, flag);
}
EXPORT_SYMBOL(ppa_hsel_del_complement);

#if IS_ENABLED(CONFIG_SOC_GRX500)
uint32_t ppa_hsel_add_class_rule(PPA_CLASS_RULE *rule, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_CLASS_RULE, (void*) rule, flag);
}

uint32_t ppa_hsel_mod_class_rule(PPA_CLASS_RULE *rule, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_MOD_CLASS_RULE, (void*) rule, flag);
}

uint32_t ppa_hsel_del_class_rule(PPA_CLASS_RULE *rule, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_CLASS_RULE, (void*) rule, flag);
}

uint32_t ppa_hsel_get_class_rule(PPA_CLASS_RULE *rule, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_CLASS_RULE, (void*) rule, flag);
}
#endif

uint32_t ppa_hsel_add_sess_meta(PPA_ROUTING_INFO *entry, PPA_BUF *skb, void *txifinfo, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	PPA_SESSMETA_INFO metainfo={0};

	if(!entry || !skb) return PPA_FAILURE;
	
	if (!ppa_drv_hal_hook[hal_id]) return PPA_FAILURE;

	metainfo.p_item = entry->p_item;
	metainfo.skb 	= skb;
	metainfo.txif_info = txifinfo;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_UPDATE_SESS_META, (void *)&metainfo, 0);
}

uint32_t ppa_hsel_del_sess_meta(PPA_ROUTING_INFO *entry, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_CLEAR_SESS_META, (void *)entry->p_item, 0);
}

uint32_t ppa_hsel_add_routing_entry(PPA_ROUTING_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_ROUTE_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_del_routing_entry(PPA_ROUTING_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_ROUTE_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_update_routing_entry(PPA_ROUTING_INFO *entry , uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_UPDATE_ROUTE_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_add_wan_mc_entry(PPA_MC_INFO *entry , uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_MC_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_del_wan_mc_entry(PPA_MC_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_MC_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_update_wan_mc_entry(PPA_MC_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_UPDATE_MC_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_add_bridging_entry(PPA_BR_MAC_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_BR_MAC_BRIDGING_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_del_bridging_entry(PPA_BR_MAC_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_BR_MAC_BRIDGING_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_test_and_clear_bridging_hit_stat(PPA_BR_MAC_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_TEST_CLEAR_BR_HIT_STAT, (void *)entry, flag);
}

uint32_t ppa_hsel_add_tunnel_entry(PPA_TUNNEL_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	if (entry->tunnel_type  ==  TUNNEL_TYPE_6RD) {
		/*Extract details from the netif and store in the tunnel table*/
		return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_6RD_TUNNEL_ENTRY, (void *)entry, flag);
	}else if (entry->tunnel_type  ==  TUNNEL_TYPE_DSLITE) {
#if defined(PPA_DSLITE) && PPA_DSLITE
		/*disable upsteam acce*/
		return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_DSLITE_TUNNEL_ENTRY, (void *)entry, flag);
#endif
	}
	return PPA_FAILURE;
}

/*****************************************************************************************/
/* debug enable for all the registered hals*/
/*****************************************************************************************/
uint32_t ppa_drv_set_hal_dbg(PPA_CMD_GENERAL_ENABLE_INFO *cfg, uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;

	for (; hal_id < MAX_HAL; hal_id++) {
		if (ppa_drv_hal_hook[hal_id] != NULL) {
			ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_DEBUG, (void *)cfg, flag);
		}
	}

	return PPA_SUCCESS;
}

uint32_t ppa_hsel_del_tunnel_entry(PPA_TUNNEL_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	if (entry->tunnel_type  ==  TUNNEL_TYPE_6RD) {
		return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_6RD_TUNNEL_ENTRY, (void *)entry, flag);
	}else if (entry->tunnel_type  ==  TUNNEL_TYPE_DSLITE) {
		return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_DSLITE_TUNNEL_ENTRY, (void *)entry, flag);
	}
	return PPA_FAILURE;
}

#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
uint32_t ppa_hsel_del_lro_entry(PPA_LRO_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_LRO_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_add_lro_entry(PPA_LRO_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_LRO_ENTRY, (void *)entry, flag);
}
#endif /*IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)*/

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
uint32_t ppa_hsel_get_ipsec_tunnel_mib(IPSEC_TUNNEL_MIB_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_IPSEC_TUNNEL_MIB, (void *)entry, flag);
}
#endif

uint32_t ppa_hsel_get_routing_entry_bytes(PPA_ROUTING_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_ROUTE_ACC_BYTES, (void *)entry, flag);
}

uint32_t ppa_hsel_get_mc_entry_bytes(PPA_MC_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_MC_ACC_BYTES, (void *)entry, flag);
}

uint32_t ppa_hsel_add_outer_vlan_entry(PPA_OUT_VLAN_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_OUT_VLAN_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_del_outer_vlan_entry(PPA_OUT_VLAN_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_OUT_VLAN_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_get_outer_vlan_entry(PPA_OUT_VLAN_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_OUT_VLAN_ENTRY, (void *)entry, flag);
}

uint32_t ppa_hsel_get_itf_mib(PPE_ITF_MIB_INFO *mib, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_ITF_MIB, (void *)mib, flag);
}

uint32_t ppa_hsel_get_generic_itf_mib( PPA_ITF_MIB_INFO *mib, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if( !ppa_drv_hal_hook[hal_id] ) return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_NEW_ITF_MIB,(void *)mib, flag );
}

uint32_t ppa_hsel_get_dsl_mib(PPA_DSL_QUEUE_MIB *mib, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_DSL_MIB, (void *)mib, flag);
}

uint32_t ppa_hsel_get_ports_mib(PPA_PORT_MIB *mib, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_PORT_MIB, (void *)mib, flag);
}

uint32_t ppa_hsel_test_and_clear_hit_stat(PPA_ROUTING_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_TEST_CLEAR_ROUTE_HIT_STAT, (void *)entry, flag);
}

uint32_t ppa_hsel_test_and_clear_mc_hit_stat(PPA_MC_INFO *entry, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_TEST_CLEAR_MC_HIT_STAT, (void *)entry, flag);
}

uint32_t ppa_hsel_get_qos_qnum(PPA_QOS_COUNT_CFG *count, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_QOS_QUEUE_NUM, (void *)count, flag);
}

uint32_t ppa_hsel_get_qos_status(PPA_QOS_STATUS *status, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_QOS_STATUS, (void *)status, flag);
}


uint32_t ppa_hsel_get_qos_mib(PPA_QOS_MIB_INFO *mib, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_QOS_MIB, (void *)mib, flag);
}

uint32_t ppa_hsel_set_ctrl_qos_rate(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_QOS_RATE_SHAPING_CTRL, (void *)enable_cfg, flag);
}
uint32_t ppa_hsel_get_ctrl_qos_rate(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_QOS_RATE_SHAPING_CTRL, (void *)enable_cfg, flag);
}

uint32_t ppa_hsel_set_qos_rate(PPA_QOS_RATE_SHAPING_CFG *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_QOS_RATE_SHAPING_CFG, (void *)cfg, flag);
}

uint32_t ppa_hsel_get_qos_rate(PPA_QOS_RATE_SHAPING_CFG *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_QOS_RATE_SHAPING_CFG, (void *)cfg, flag);
}

uint32_t ppa_hsel_reset_qos_rate(PPA_QOS_RATE_SHAPING_CFG *cfg , uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_RESET_QOS_RATE_SHAPING_CFG, (void *)cfg, flag);
}

uint32_t ppa_hsel_init_qos_rate(uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_INIT_QOS_RATE_SHAPING, (void *)NULL, flag);
}

uint32_t ppa_hsel_set_ctrl_qos_wfq(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_QOS_WFQ_CTRL, (void *)enable_cfg, flag);
}

uint32_t ppa_hsel_get_ctrl_qos_wfq(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_QOS_WFQ_CTRL, (void *)enable_cfg, flag);
}

uint32_t ppa_hsel_set_qos_wfq(PPA_QOS_WFQ_CFG *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_QOS_WFQ_CFG, (void *)cfg, flag);
}

uint32_t ppa_hsel_get_qos_wfq(PPA_QOS_WFQ_CFG *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_QOS_WFQ_CFG, (void *)cfg, flag);
}

uint32_t ppa_hsel_reset_qos_wfq(PPA_QOS_WFQ_CFG *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_RESET_QOS_WFQ_CFG, (void *)cfg, flag);
}

uint32_t ppa_hsel_init_qos_wfq(uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_INIT_QOS_WFQ, (void *)NULL, flag);
}
/*****************************************************************************************/
/* HAL layer APIs*/
/*****************************************************************************************/
static inline int32_t get_platform_hal(int api_type) /* 0 = routing, 1 = qos */
{	
	uint32_t hal_id = PPE_HAL;
#ifdef CONFIG_PPA_PUMA7
	hal_id = PUMA_HAL;
#elif IS_ENABLED(CONFIG_SOC_GRX500)
	switch(api_type) {
	case 0: 
		hal_id  =  PAE_HAL;
		break;
	case 1:
		hal_id  =  TMU_HAL;
		break;
	default:
		hal_id	= PAE_HAL;
	}
#elif  IS_ENABLED(CONFIG_SOC_LGM)
 	hal_id = PPV4_HAL;
#endif
	return hal_id;
}
/*****************************************************************************************/
/*  Initialize all registred HAL layers							 */
/*****************************************************************************************/
uint32_t ppa_drv_hal_init(PPA_HAL_INIT_CFG *cfg, uint32_t flag)
{
	int i = 0;

	/* Initialize the capability list head*/
	for (i = 0; i < MAX_CAPS; i++) {
		PPA_INIT_HLIST_HEAD(&g_hsel_caps_list[i]);
	}
	/* Initialize all the registred HAL layers*/
	for (i = 0; i < MAX_HAL; i++) {
		if (ppa_drv_hal_hook[i] && (ppa_hsel_hal_init(cfg, flag, i) !=  PPA_SUCCESS))
			return PPA_FAILURE;
	}

	for (i = 0; i<MAX_TUNNEL_ENTRIES; i++) {
		g_tunnel_table[i]  =  NULL;
		g_tunnel_counter[i]  =  0;
	}

	ppe_lock_init(&g_hsel_caps_lock);
	ppe_lock_init(&g_tunnel_table_lock);

	return PPA_SUCCESS;
}

/*****************************************************************************************/
/*  Un-Initialize all registred HAL layers						 */
/*****************************************************************************************/
uint32_t ppa_drv_hal_exit(uint32_t flag)
{
	int i = 0;

	for (i = 0; i<MAX_TUNNEL_ENTRIES; i++) {
		kfree(g_tunnel_table[i]);
	}

	for (i = 0; i < MAX_HAL; i++) {
		if (ppa_drv_hal_hook[i] && (ppa_hsel_hal_exit(flag, i) !=  PPA_SUCCESS))
			return PPA_FAILURE;
	}

	return PPA_SUCCESS;
}

/*****************************************************************************************/
/*  return the id of all registred HAL layers						 */
/*****************************************************************************************/
uint32_t ppa_drv_get_hal_id(PPA_VERSION *v, uint32_t flag)
{
	return ppa_hsel_get_hal_id(v, flag, get_platform_hal(0));
}

/*****************************************************************************************/
/*  return the firmware id of all registred HAL layers					 */
/*****************************************************************************************/
uint32_t ppa_drv_get_firmware_id(PPA_VERSION *v, uint32_t flag)
{
uint32_t hal_id = PPE_HAL;
#ifndef CONFIG_PPA_PUMA7
#if IS_ENABLED(CONFIG_SOC_GRX500)
	if (v->index  ==  0)
		hal_id  =  PAE_HAL;
	else
		hal_id  =  MPE_HAL;
#endif
	return ppa_hsel_get_firmware_id(v, flag, hal_id);
#endif
	return PPA_FAILURE;
}

/*****************************************************************************************/
/* This function returns the number of physical port at the system level*/
/* Since PAE has the physical ports in the iRX500 system connected to it, we need to query only PAE*/
/* in case of legacy platforms we need to call PPE HAL*/
/*****************************************************************************************/
uint32_t ppa_drv_get_number_of_phys_port(PPA_COUNT_CFG *count, uint32_t flag)
{
	return ppa_hsel_get_number_of_phys_port(count, flag, get_platform_hal(0));
}

/*****************************************************************************************/
/* Re-write the hal layer implementation in case of GRX500*/
/* Since PAE has the physical ports in the system connected to it, we need to query only PAE*/
/*****************************************************************************************/
uint32_t ppa_drv_get_phys_port_info(PPE_IFINFO *info, uint32_t flag)
{
#if IS_ENABLED(CONFIG_PRX300_CQM)
	return ppa_hsel_get_phys_port_info(info, flag, MPE_HAL);
#else
	return ppa_hsel_get_phys_port_info(info, flag, get_platform_hal(0));
#endif
}

/*****************************************************************************************/
/* The paramters returned by this function is too tightly coupled to PPE implementation*/
/* some of those parameters are not valid for GRX500*/
/* some new parametrs are added to the original datastructure to support GRX500 routing*/
/*****************************************************************************************/
uint32_t ppa_drv_get_max_entries(PPA_MAX_ENTRY_INFO *entry, uint32_t flag)
{
	uint32_t hal_id = get_platform_hal(0);
#if IS_ENABLED(CONFIG_SOC_GRX500)
	entry->ppe_hal_disabled  =  1;
#endif
	if ((ppa_drv_hal_hook[hal_id] != NULL) && (ppa_hsel_get_max_entries(entry, flag, hal_id) !=  PPA_SUCCESS))
		return PPA_FAILURE;

	return PPA_SUCCESS;
}

/*****************************************************************************************/
/* Initial routing configuration*/
/* this will include some changes in the parameters based on PAE and MPE initialization parameters*/
/*******************re-visit**********************************/
uint32_t ppa_drv_set_route_cfg(PPA_ROUTING_CFG *cfg, uint32_t flag)
{
	uint32_t hal_id = get_platform_hal(0);
	
	if ((ppa_drv_hal_hook[hal_id] != NULL) && (ppa_hsel_set_route_cfg(cfg, flag, hal_id)!=  PPA_SUCCESS))
		return PPA_FAILURE;

	return PPA_SUCCESS;
}

/*****************************************************************************************/
/* Initial bridging configuration*/
/* Bridge intital configuration*/
/* Only PAE will do the bridging in case of GRX500*/
/* In legacy platforms bridge acceleration is not supported*/
/*****************************************************************************************/
uint32_t ppa_drv_set_bridging_cfg(PPA_BRDG_CFG *cfg, uint32_t flag)
{
	return ppa_hsel_set_bridging_cfg(cfg, flag, get_platform_hal(0));
}

/*****************************************************************************************/
/* enable or disable acceleration*/
/* This is achived in PAE by writing a flow rule to send all the packets to CPU port*/
/*****************************************************************************************/
uint32_t ppa_drv_get_acc_mode(PPA_ACC_ENABLE *cfg, uint32_t flag)
{
	return ppa_hsel_get_acc_mode(cfg, flag, get_platform_hal(0));
}

/*****************************************************************************************/
/* enable or disable acceleration*/
/* This is achived in PAE by writing a flow rule to send all the packets to CPU port*/
/*****************************************************************************************/
uint32_t ppa_drv_set_acc_mode(PPA_ACC_ENABLE *cfg, uint32_t flag)
{
	return ppa_hsel_set_acc_mode(cfg, flag, get_platform_hal(0));
}

/*****************************************************************************************/
/* Add bridge session entry */
/* Supported only by PAE in GRX500 so no need to run the HAL selection Algorithm */
/*****************************************************************************************/
uint32_t ppa_drv_add_bridging_entry(PPA_BR_MAC_INFO *entry, uint32_t flag)
{
	return ppa_hsel_add_bridging_entry(entry, flag, get_platform_hal(0));
}

/*****************************************************************************************/
/* Delete bridge session entry */
/* Supported only by PAE in GRX500 so no need to run the HAL selection Algorithm */
/*****************************************************************************************/
uint32_t ppa_drv_del_bridging_entry(PPA_BR_MAC_INFO *entry, uint32_t flag)
{
	return ppa_hsel_del_bridging_entry(entry, flag, get_platform_hal(0));
}

/*****************************************************************************************/
/* Multi bridge support*/
/* Adding a port under a bridge */
/* Supported only by PAE in GRX500 so no need to run the HAL selection Algorithm */
/*****************************************************************************************/
uint32_t ppa_drv_add_br_port(PPA_BR_PORT_INFO *entry, uint32_t flag)
{
	uint32_t hal_id = get_platform_hal(0);

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_BR_PORT, (void *)entry, flag);
}

/*****************************************************************************************/
/* Multi bridge support*/
/* deleting a port under a bridge */
/* Supported only by PAE in GRX500 so no need to run the HAL selection Algorithm */
/*****************************************************************************************/
uint32_t ppa_drv_del_br_port(PPA_BR_PORT_INFO *entry, uint32_t flag)
{
	uint32_t hal_id = get_platform_hal(0);

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_BR_PORT, (void *)entry, flag);
}


/*****************************************************************************************/
/* Bridge entry timeout */
/* Supported only by PAE in GRX500 so no need to run the HAL selection Algorithm */
/*****************************************************************************************/
uint32_t ppa_drv_test_and_clear_bridging_hit_stat(PPA_BR_MAC_INFO *entry, uint32_t flag)
{
	return ppa_hsel_test_and_clear_bridging_hit_stat(entry, flag, get_platform_hal(0));
}

#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
uint32_t ppa_drv_set_mib_mode(PPA_MIB_MODE_ENABLE *cfg, uint32_t flag)
{
	uint32_t hal_id = get_platform_hal(0);

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_MIB_MODE_ENABLE, (void *)cfg, flag);
}

uint32_t ppa_drv_get_mib_mode(PPA_MIB_MODE_ENABLE *cfg)
{
	uint32_t hal_id = get_platform_hal(0);

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_MIB_MODE_ENABLE, (void *)cfg, 0);
}
#endif /* CONFIG_PPA_MIB_MODE */

#if defined(RTP_SAMPLING_ENABLE) && RTP_SAMPLING_ENABLE
uint32_t ppa_hsel_set_wan_mc_rtp(PPA_MC_INFO *entry, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_MC_RTP, (void *)entry, 0);
}

uint32_t ppa_drv_set_wan_mc_rtp(PPA_MC_INFO *entry)
{
	uint32_t hal_id = PPE_HAL;
	return ppa_hsel_set_wan_mc_rtp(entry, hal_id);
}

uint32_t ppa_hsel_get_mc_rtp_sampling_cnt(PPA_MC_INFO *entry, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_MC_RTP_SAMPLING_CNT, (void *)entry, 0);
}
#endif

/*only neded for ppe hal*/
uint32_t ppa_drv_set_bridge_if_vlan_config(PPA_BRDG_VLAN_CFG *cfg, uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_BRDG_VLAN_CFG, (void *)cfg, flag);
}
uint32_t ppa_drv_get_bridge_if_vlan_config(PPA_BRDG_VLAN_CFG *cfg, uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_BRDG_VLAN_CFG, (void *)cfg, flag);
}
uint32_t ppa_drv_add_vlan_map(PPA_BRDG_VLAN_FILTER_MAP *filter, uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;
	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_ADD_BRDG_VLAN_FITLER, (void *)filter, flag);
}

uint32_t ppa_drv_del_vlan_map(PPA_BRDG_VLAN_FILTER_MAP *filter, uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_BRDG_VLAN_FITLER, (void *)filter, flag);
}
uint32_t ppa_drv_get_vlan_map(PPA_BRDG_VLAN_FILTER_MAP *filter, uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_BRDG_VLAN_FITLER, (void *)filter, flag);
}

uint32_t ppa_drv_del_all_vlan_map(uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_DEL_BRDG_VLAN_ALL_FITLER_MAP, (void *)NULL, flag);
}

uint32_t ppa_drv_get_max_vfilter_entries(PPA_VFILTER_COUNT_CFG *count, uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_MAX_VFILTER_ENTRY_NUM, (void *)count, flag);
}

uint32_t ppa_set_wan_itf(PPA_WANITF_CFG *cfg, uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;

	if (!ppa_drv_hal_hook[hal_id]) return PPA_SUCCESS;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_SET_WANITF, (void *)cfg, flag);
}

uint32_t ppa_get_session_hash(PPA_SESSION_HASH *cfg, uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;

	if (ppa_drv_hal_hook[hal_id]) {
		return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_SESSION_HASH, (void *)cfg, flag);
	} else {
		/* in case of PAE the hash information will be same as the hash stored in uc_session_node.hash_index*/
		/* hash table id '2' indicates that this is PAE hash table*/
		cfg->flag  =  PAE_HAL;
		return PPA_SUCCESS;
	}

	return PPA_FAILURE;
}

/*****************************************************************************************
* HAL selector needs to iterate through all the HALs registered for capability "SESS_IPV6,*
*****************************************************************************************/
uint32_t ppa_drv_is_ipv6_enabled(uint32_t flag)
{
	PPA_HSEL_CAP_NODE *t_node = NULL;
	PPA_HLIST_NODE *tmp;

	/* for each HAL registred for capability IPV6_ROUTING*/
	/* if any of the HAL is having it enabled return success*/
	ppa_hlist_for_each_entry_safe(t_node, tmp,  &g_hsel_caps_list[SESS_IPV6], cap_list) {
		if (ppa_hsel_is_ipv6_enabled(flag, t_node->hal_id)  ==  PPA_SUCCESS) {
			return 1;
		}
	}

	return PPA_FAILURE;
}

/*****************************************************************************************
* interface mib can be read from PAE in case of GRX500
*****************************************************u***********************************/
uint32_t ppa_drv_get_itf_mib(PPE_ITF_MIB_INFO *mib, uint32_t flag)
{
	return ppa_hsel_get_itf_mib(mib, flag, get_platform_hal(0));
}

/*****************************************************************************************
 DSL interface mib can be read from DSL HAL
*****************************************************************************************/
uint32_t ppa_drv_get_dsl_mib(PPA_DSL_QUEUE_MIB *mib, uint32_t flag)
{
	uint32_t hal_id = PPE_HAL;
	if (ppa_drv_hal_hook[hal_id]  ==  NULL) {
		hal_id  =  DSL_HAL;
	}

	return ppa_hsel_get_dsl_mib(mib, flag, hal_id);
}

/*****************************************************************************************
 Port based MIB can be read from PAE HAL*
*****************************************************************************************/
uint32_t ppa_drv_get_ports_mib(PPA_PORT_MIB *mib, uint32_t flag)
{
	uint32_t hal_id = get_platform_hal(0);
	
	mib->flags  =  hal_id;

	return ppa_hsel_get_ports_mib(mib, flag,hal_id);
}

/*****************************************************************************************
* QOS get number of queues per port*
*****************************************************************************************/
uint32_t ppa_drv_get_qos_qnum(PPA_QOS_COUNT_CFG *count, uint32_t flag)
{
	return ppa_hsel_get_qos_qnum(count, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS global qos status get*
*****************************************************************************************/
uint32_t ppa_drv_get_qos_status(PPA_QOS_STATUS *status, uint32_t flag)
{
	return ppa_hsel_get_qos_status(status, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS queue level MIB get*
*****************************************************************************************/
uint32_t ppa_drv_get_qos_mib(PPA_QOS_MIB_INFO *mib, uint32_t flag)
{
	return ppa_hsel_get_qos_mib(mib, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS set qos rate shaping enable/disable*
*****************************************************************************************/
uint32_t ppa_drv_set_ctrl_qos_rate(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag)
{
	return ppa_hsel_set_ctrl_qos_rate(enable_cfg, flag, get_platform_hal(1));
}
/*****************************************************************************************
* QOS get rateshaping status*
*****************************************************************************************/
uint32_t ppa_drv_get_ctrl_qos_rate(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag)
{
	return ppa_hsel_get_ctrl_qos_rate(enable_cfg, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS set rate of a queue*
*****************************************************************************************/
uint32_t ppa_drv_set_qos_rate(PPA_QOS_RATE_SHAPING_CFG *cfg, uint32_t flag)
{
	return ppa_hsel_set_qos_rate(cfg, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS get rate of a queue*
*****************************************************************************************/
uint32_t ppa_drv_get_qos_rate(PPA_QOS_RATE_SHAPING_CFG *cfg, uint32_t flag)
{
	return ppa_hsel_get_qos_rate(cfg, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS reset teh rate od a queue to default*
*****************************************************************************************/
uint32_t ppa_drv_reset_qos_rate(PPA_QOS_RATE_SHAPING_CFG *cfg , uint32_t flag)
{
	return ppa_hsel_reset_qos_rate(cfg, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS initialize qos rateshaping*
*****************************************************************************************/
uint32_t ppa_drv_init_qos_rate(uint32_t flag)
{
	return ppa_hsel_init_qos_rate(flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS set enable/disable wfq*
*****************************************************************************************/
uint32_t ppa_drv_set_ctrl_qos_wfq(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag)
{
	return ppa_hsel_set_ctrl_qos_wfq(enable_cfg, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS get wfq status*
*****************************************************************************************/
uint32_t ppa_drv_get_ctrl_qos_wfq(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag)
{
	return ppa_hsel_get_ctrl_qos_wfq(enable_cfg, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS set queue weight*
*****************************************************************************************/
uint32_t ppa_drv_set_qos_wfq(PPA_QOS_WFQ_CFG *cfg, uint32_t flag)
{
	return ppa_hsel_set_qos_wfq(cfg, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS get queue weight*
*****************************************************************************************/
uint32_t ppa_drv_get_qos_wfq(PPA_QOS_WFQ_CFG *cfg, uint32_t flag)
{
	return ppa_hsel_get_qos_wfq(cfg, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS reset queue weight*
*****************************************************************************************/
uint32_t ppa_drv_reset_qos_wfq(PPA_QOS_WFQ_CFG *cfg, uint32_t flag)
{
	return ppa_hsel_reset_qos_wfq(cfg, flag, get_platform_hal(1));
}

/*****************************************************************************************
* QOS initialize wfq*
*****************************************************************************************/
uint32_t ppa_drv_init_qos_wfq(uint32_t flag)
{
	return ppa_hsel_init_qos_wfq(flag, get_platform_hal(1));
}

#if defined(MBR_CONFIG) && MBR_CONFIG
uint32_t ppa_hsel_set_qos_shaper(PPA_QOS_RATE_SHAPING_CFG *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_QOS_RATE_SHAPING_CTRL, (void *)cfg, flag);
}

uint32_t ppa_hsel_get_qos_shaper(PPA_QOS_RATE_SHAPING_CFG *cfg, uint32_t flag, uint32_t hal_id)
{
	if (hal_id >= MAX_HAL)
		return PPA_FAILURE;

	if (!ppa_drv_hal_hook[hal_id])
		return PPA_FAILURE;

	return ppa_drv_hal_hook[hal_id](PPA_GENERIC_HAL_GET_QOS_RATE_SHAPING_CFG, (void *)cfg, flag);
}


uint32_t ppa_drv_set_qos_shaper(PPA_QOS_RATE_SHAPING_CFG *cfg, uint32_t flag)
{
	return ppa_hsel_set_qos_shaper(cfg, flag, get_platform_hal(1));
}

uint32_t ppa_drv_get_qos_shaper(PPA_QOS_RATE_SHAPING_CFG *cfg, uint32_t flag)
{
	return ppa_hsel_get_qos_shaper(cfg, flag, get_platform_hal(1));

}

EXPORT_SYMBOL(ppa_drv_set_qos_shaper);
EXPORT_SYMBOL(ppa_drv_get_qos_shaper);
#endif /*end of MBR_CONFIG*/

EXPORT_SYMBOL(ppa_drv_hal_hook);
EXPORT_SYMBOL(g_tunnel_table);
EXPORT_SYMBOL(g_tunnel_counter);
EXPORT_SYMBOL(g_tunnel_table_lock);

EXPORT_SYMBOL(ppa_drv_get_num_registred_hals);
EXPORT_SYMBOL(ppa_drv_generic_hal_register);
EXPORT_SYMBOL(ppa_drv_generic_hal_deregister);
EXPORT_SYMBOL(ppa_drv_register_cap);
EXPORT_SYMBOL(ppa_drv_deregister_cap);
EXPORT_SYMBOL(ppa_select_hals_from_caplist);
EXPORT_SYMBOL(ppa_group_hals_in_capslist);

EXPORT_SYMBOL(ppa_set_wan_itf);
EXPORT_SYMBOL(ppa_drv_hal_init);
EXPORT_SYMBOL(ppa_drv_hal_exit);
EXPORT_SYMBOL(ppa_drv_get_hal_id);
EXPORT_SYMBOL(ppa_drv_get_max_entries);
EXPORT_SYMBOL(ppa_drv_set_route_cfg);
EXPORT_SYMBOL(ppa_drv_get_ports_mib);
EXPORT_SYMBOL(ppa_hsel_get_generic_itf_mib);
EXPORT_SYMBOL(ppa_drv_get_dsl_mib);
EXPORT_SYMBOL(ppa_drv_get_itf_mib);
EXPORT_SYMBOL(ppa_drv_set_acc_mode);
EXPORT_SYMBOL(ppa_drv_get_acc_mode);
EXPORT_SYMBOL(ppa_drv_get_firmware_id);
EXPORT_SYMBOL(ppa_drv_get_phys_port_info);
EXPORT_SYMBOL(ppa_drv_get_number_of_phys_port);
EXPORT_SYMBOL(ppa_drv_is_ipv6_enabled);

EXPORT_SYMBOL(ppa_drv_set_bridging_cfg);
EXPORT_SYMBOL(ppa_drv_add_bridging_entry);
EXPORT_SYMBOL(ppa_drv_del_bridging_entry);
EXPORT_SYMBOL(ppa_drv_add_br_port);
EXPORT_SYMBOL(ppa_drv_del_br_port);
EXPORT_SYMBOL(ppa_drv_test_and_clear_bridging_hit_stat);

EXPORT_SYMBOL(ppa_drv_set_qos_rate);
EXPORT_SYMBOL(ppa_drv_get_ctrl_qos_wfq);
EXPORT_SYMBOL(ppa_drv_get_ctrl_qos_rate);
EXPORT_SYMBOL(ppa_drv_set_qos_wfq);
EXPORT_SYMBOL(ppa_drv_get_qos_mib);
EXPORT_SYMBOL(ppa_drv_get_qos_rate);
EXPORT_SYMBOL(ppa_drv_reset_qos_rate);
EXPORT_SYMBOL(ppa_drv_set_ctrl_qos_rate);
EXPORT_SYMBOL(ppa_drv_get_qos_status);
EXPORT_SYMBOL(ppa_drv_get_qos_qnum);
EXPORT_SYMBOL(ppa_drv_init_qos_rate);
EXPORT_SYMBOL(ppa_drv_init_qos_wfq);
EXPORT_SYMBOL(ppa_drv_reset_qos_wfq);
EXPORT_SYMBOL(ppa_drv_get_qos_wfq);
EXPORT_SYMBOL(ppa_drv_set_ctrl_qos_wfq);

EXPORT_SYMBOL(ppa_get_session_hash);
EXPORT_SYMBOL(ppa_drv_get_max_vfilter_entries);
EXPORT_SYMBOL(ppa_drv_get_vlan_map);
EXPORT_SYMBOL(ppa_drv_del_vlan_map);
EXPORT_SYMBOL(ppa_drv_add_vlan_map);
EXPORT_SYMBOL(ppa_drv_del_all_vlan_map);
#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
EXPORT_SYMBOL(ppa_drv_set_mib_mode);
EXPORT_SYMBOL(ppa_drv_get_mib_mode);
#endif
#if defined(RTP_SAMPLING_ENABLE) && RTP_SAMPLING_ENABLE
EXPORT_SYMBOL(ppa_drv_set_wan_mc_rtp);
EXPORT_SYMBOL(ppa_hsel_set_wan_mc_rtp);
EXPORT_SYMBOL(ppa_hsel_get_mc_rtp_sampling_cnt);
#endif
EXPORT_SYMBOL(ppa_drv_get_bridge_if_vlan_config);
EXPORT_SYMBOL(ppa_drv_set_bridge_if_vlan_config);

EXPORT_SYMBOL(ppa_drv_set_hal_dbg);
EXPORT_SYMBOL(ppa_hsel_add_sess_meta);
EXPORT_SYMBOL(ppa_hsel_del_sess_meta);
EXPORT_SYMBOL(ppa_hsel_add_routing_entry);
EXPORT_SYMBOL(ppa_hsel_del_routing_entry);
EXPORT_SYMBOL(ppa_hsel_update_routing_entry);
EXPORT_SYMBOL(ppa_hsel_add_wan_mc_entry);
EXPORT_SYMBOL(ppa_hsel_del_wan_mc_entry);
EXPORT_SYMBOL(ppa_hsel_update_wan_mc_entry);

EXPORT_SYMBOL(ppa_hsel_add_tunnel_entry);
EXPORT_SYMBOL(ppa_hsel_del_tunnel_entry);

#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
EXPORT_SYMBOL(ppa_hsel_add_lro_entry);
EXPORT_SYMBOL(ppa_hsel_del_lro_entry);
#endif /* defined(CONFIG_LTQ_TOE_DRIVER) && CONFIG_LTQ_TOE_DRIVER*/

EXPORT_SYMBOL(ppa_hsel_get_routing_entry_bytes);
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
EXPORT_SYMBOL(ppa_hsel_get_ipsec_tunnel_mib);
#endif
EXPORT_SYMBOL(ppa_hsel_get_mc_entry_bytes);
EXPORT_SYMBOL(ppa_hsel_test_and_clear_hit_stat);
EXPORT_SYMBOL(ppa_hsel_test_and_clear_mc_hit_stat);

EXPORT_SYMBOL(ppa_hsel_add_outer_vlan_entry);
EXPORT_SYMBOL(ppa_hsel_del_outer_vlan_entry);
EXPORT_SYMBOL(ppa_hsel_get_outer_vlan_entry);

EXPORT_SYMBOL(ppa_hsel_init_qos_cfg);
EXPORT_SYMBOL(ppa_hsel_uninit_qos_cfg);
EXPORT_SYMBOL(ppa_hsel_add_qos_queue_entry);
EXPORT_SYMBOL(ppa_hsel_modify_qos_queue_entry);
EXPORT_SYMBOL(ppa_hsel_delete_qos_queue_entry);
EXPORT_SYMBOL(ppa_hsel_set_qos_rate_entry);
EXPORT_SYMBOL(ppa_hsel_reset_qos_rate_entry);
EXPORT_SYMBOL(ppa_hsel_set_qos_shaper_entry);
EXPORT_SYMBOL(ppa_hsel_mod_subif_port_cfg);
#if IS_ENABLED(CONFIG_SOC_GRX500)
EXPORT_SYMBOL(ppa_hsel_add_class_rule);
EXPORT_SYMBOL(ppa_hsel_mod_class_rule);
EXPORT_SYMBOL(ppa_hsel_del_class_rule);
EXPORT_SYMBOL(ppa_hsel_get_class_rule);
#endif
