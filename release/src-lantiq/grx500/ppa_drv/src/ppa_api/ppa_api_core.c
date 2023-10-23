/*******************************************************************************
 **
 ** FILE NAME		: ppa_api_core.c
 ** PROJECT		: PPA
 ** MODULES		: PPA API (Routing/Bridging Acceleration APIs)
 **
 ** DATE		: 3 NOV 2008
 ** AUTHOR		: Xu Liang
 ** DESCRIPTION		: PPA Protocol Stack Hook API Implementation
 ** COPYRIGHT		: Copyright (c) 2017 Intel Corporation
 **				Lantiq Deutschland GmbH
 **				Am Campeon 3; 85579 Neubiberg, Germany
 **data
 **	 For licensing information, see the file 'LICENSE' in the root folder of
 **	 this software module.
 **
 ** HISTORY
 ** $Date	$Author			$Comment
 ** 03 NOV 2008	Xu Liang		Initiate Version
 ** 10 DEC 2012	Manamohan Shetty	Added the support for RTP,MIB mode 
 **									 Features
 *******************************************************************************/
/*
 * ####################################
 *		Version No.
 * ####################################
 */
#define VER_FAMILY		0x60		/*	bit 0: res*/
/*		1: Danube
//		2: Twinpass
//		3: Amazon-SE
//		4: res
//		5: AR9
//		6: GR9*/
#define VER_DRTYPE		0x20		/*	bit 0: Normal Data Path driver
//		1: Indirect-Fast Path driver
//		2: HAL driver
//		3: Hook driver
//		4: Stack/System Adaption Layer driver
//		5: PPA API driver*/
#define VER_INTERFACE	 0x07		/*	bit 0: MII 0
//		1: MII 1
//		2: ATM WAN
//		3: PTM WAN*/
#define VER_ACCMODE	 0x03		/*	bit 0: Routing
//		1: Bridging */
#define VER_MAJOR	0
#define VER_MID		0
#define VER_MINOR	4

#define ETHSW_INVALID_PORT 0xFF
/*
 * ####################################
 *		Head File
 * ####################################
 */
#include <linux/string.h>
#include <linux/kernel.h>
/*
 *	Common Head File
 */
/*
 *	PPA Specific Head File
 */
#include <net/ppa/ppa_api_common.h>
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_hook.h>
#include <net/ppa/ppa_drv_wrapper.h>
#include <net/ppa/ppa_hal_wrapper.h>

#include "ppa_api_misc.h"
#include "ppa_api_netif.h"
#include "ppa_api_sess_helper.h"
#include "ppa_api_session.h"
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
#include "ppa_api_session_limit.h"
#endif
#include "ppa_api_hal_selector.h"
#include "ppa_api_core.h"
#include "ppa_api_tools.h"
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
#include "ppa_sae_hal.h"
#endif
#if IS_ENABLED(CONFIG_SOC_GRX500)
#if IS_ENABLED(CONFIG_LTQ_PPA_COC_SUPPORT)
#include "ppa_api_cpu_freq.h"
#endif
#else
#if IS_ENABLED(CONFIG_LTQ_CPU_FREQ) || IS_ENABLED(CONFIG_LTQ_PMCU)
#include "ppa_api_pwm.h"
#endif
#endif
#include "ppa_api_debug.h"

#ifdef CONFIG_PPA_PUMA7
#include <linux/inetdevice.h>
#endif

#if IS_ENABLED(CONFIG_PPA_QOS)
#include "ppa_api_qos.h"
#endif
#include "ppa_api_mib.h"
#include "ppa_stack_tnl_al.h"

/*
 * ####################################
 *		Definition
 * ####################################
 */
#define MIN_HITS	10

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
struct ipsec_tunnel_intf ipsec_tnl_info;
#endif
/*
 * ####################################
 *		Data Type
 * ####################################
 */
#if IS_ENABLED(CONFIG_SOC_GRX500)
typedef struct fidinfo {
	PPA_IFNAME ifname[PPA_IF_NAME_SIZE];
	uint32_t fid;
	struct fidinfo *next;
} fid_info;

static fid_info		*fid_list=NULL;
static uint32_t		g_next_fid=0;

#endif
static FILTER_INFO axglobal_filters[]={
	{{0}, PPA_IPPROTO_UDP, 0, 137, (PPA_SESSION_FILTER_PROTO | PPA_SESSION_FILTER_DPORT)},
	{{0}, PPA_IPPROTO_TCP, 0, 137, (PPA_SESSION_FILTER_PROTO | PPA_SESSION_FILTER_DPORT)},
	{{0}, PPA_IPPROTO_UDP, 0, 138, (PPA_SESSION_FILTER_PROTO | PPA_SESSION_FILTER_DPORT)},
	{{0}, PPA_IPPROTO_UDP, 0, 67, (PPA_SESSION_FILTER_PROTO | PPA_SESSION_FILTER_DPORT)},
	{{0}, PPA_IPPROTO_UDP, 0, 68, (PPA_SESSION_FILTER_PROTO | PPA_SESSION_FILTER_DPORT)},
	{{0}, PPA_IPPROTO_UDP, 0, 53, (PPA_SESSION_FILTER_PROTO | PPA_SESSION_FILTER_DPORT)},
	{{0}, PPA_IPPROTO_TCP, 0, 53, (PPA_SESSION_FILTER_PROTO | PPA_SESSION_FILTER_DPORT)},
};
static FILTER_INFO axsae_filters[]={
	{{0}, PPA_IPPROTO_TCP, 0, 443, (PPA_SESSION_FILTER_PROTO | PPA_SESSION_FILTER_DPORT)},
	{{0}, PPA_IPPROTO_UDP, 0, 68, (PPA_SESSION_FILTER_PROTO | PPA_SESSION_FILTER_DPORT)},
	{{0}, PPA_IPPROTO_TCP, 0, 23, (PPA_SESSION_FILTER_PROTO | PPA_SESSION_FILTER_DPORT)},
};
struct list_head *g_session_filter_list[MAX_HAL+1];

/*
 * ####################################
 *	Global Variable
 * ####################################
 */
PPA_FILTER_STATS ppa_filter = {0};
PPA_PORT_QUEUE_NUM_STATUS ppa_port_qnum[PPA_MAX_PORT_NUM] = {0};
uint32_t g_ppa_min_hits = MIN_HITS;
#ifndef CONFIG_PPA_PUMA7
volatile uint8_t g_ppe_fastpath_enabled = 1;
#else
volatile uint8_t g_ppe_fastpath_enabled = 0;
#endif
/*maximum frame size from ip header to end of the data payload, not including MAC header/pppoe header/vlan */
uint32_t g_ppa_ppa_mtu=DEFAULT_MTU;

/* PPA_SS_VERSION is taken from feed Makefile */
#define PPA_SS_DEFAULT_VERSION	"0.0.0.0"
#ifndef PPA_SS_VERSION
#define PPA_SS_VERSION PPA_SS_DEFAULT_VERSION
#endif
const char g_ppa_ss_version[VERSION_STR_LEN] = PPA_SS_VERSION;
/*
 * ####################################
 *			 Extern Variable
 * ####################################
 */
/*
 * ####################################
 *			Local Function
 * ####################################
 */
static uint8_t g_bridging_mac_learning = 1;
#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
#if IS_ENABLED(CONFIG_SOC_GRX500)
static uint8_t g_bridged_flow_learning = 0;
#else
static uint8_t g_bridged_flow_learning = 1;
#endif
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/

#if IS_ENABLED(CONFIG_SOC_GRX500)
void free_fid_list(void)
{
	fid_info *tmp;
	while(fid_list) {
		tmp=fid_list;
		fid_list = fid_list->next;
		ppa_free(tmp);
	}
}

int32_t ppa_get_fid(PPA_IFNAME *ifname, uint16_t *fid)
{
	int32_t ret = PPA_SUCCESS;
	fid_info *tmp;

	if(ifname) {
		if(fid_list) {
			/*searching in the fid list*/
			tmp=fid_list;
			while(tmp) {
				if(!strncmp(tmp->ifname,ifname, PPA_IF_NAME_SIZE)){
					*fid = tmp->fid;
					break;
				}
				tmp = tmp->next;
			}
			/*search returned failure*/
			if(!tmp) {
				tmp = (fid_info*) ppa_malloc (sizeof(fid_info));
				if(tmp) {
					tmp->next = fid_list;
					ppa_memcpy(tmp->ifname, ifname, PPA_IF_NAME_SIZE);
					tmp->fid = g_next_fid++;
					*fid = tmp->fid;
					fid_list = tmp;
				} else {
					ret = PPA_FAILURE;
				}
			}
		} else { /* first entry in the fid list*/
			fid_list = (fid_info*) ppa_malloc (sizeof(fid_info));
			if(fid_list) {
				fid_list->next = NULL;
				ppa_memcpy(fid_list->ifname, ifname, PPA_IF_NAME_SIZE);
				fid_list->fid= g_next_fid++;
				*fid = fid_list->fid;
			} else {
				ret = PPA_FAILURE;
			}
		}
	}
	return ret;
}
#endif
FILTER_INFO* get_matched_entry (PPA_HAL_ID hal_id, FILTER_INFO *entry)
{
	FILTER_INFO *filter_node;
	struct list_head *list_tmp = NULL;

	if (g_session_filter_list[hal_id] == NULL){
		return NULL;
	}

	list_for_each(list_tmp, g_session_filter_list[hal_id]){
		filter_node = list_entry(list_tmp, FILTER_INFO, list);

		if (!((filter_node->flags & PPA_SESSION_FILTER_PROTO) && (filter_node->ip_proto != entry->ip_proto))
			&& !((filter_node->flags & PPA_SESSION_FILTER_SPORT) && (filter_node->src_port != entry->src_port))
			&& !((filter_node->flags & PPA_SESSION_FILTER_DPORT) && (filter_node->dst_port != entry->dst_port))
			&& ((filter_node->flags & PPA_SESSION_FILTER_DPORT)
				|| (filter_node->flags & PPA_SESSION_FILTER_PROTO)
				|| (filter_node->flags & PPA_SESSION_FILTER_SPORT)))

			return filter_node;
	}

	return NULL;
}

bool add_filter_entry (PPA_HAL_ID hal_id, FILTER_INFO *entry)
{
	FILTER_INFO *filter_node;

	filter_node = get_matched_entry(hal_id, entry);

	if (filter_node) {
		printk("Filter already exists with these params\n");
		return true;
	}

	if (g_session_filter_list[hal_id] == NULL) {
		g_session_filter_list[hal_id] = (struct list_head *)ppa_malloc(sizeof(struct list_head));
		if (g_session_filter_list[hal_id] == NULL) {
			printk("Memory allocation failed\n");
			return false;
		}
		INIT_LIST_HEAD(g_session_filter_list[hal_id]);
	}

	filter_node = (FILTER_INFO *)ppa_malloc(sizeof(FILTER_INFO));
	if (filter_node == NULL) {
		/* Dont free g_session_filter_list head on return */
		printk("Memory allocation failed\n");
		return false;
	}
	list_add_tail( &(filter_node->list), g_session_filter_list[hal_id]);

	filter_node->ip_proto = entry->ip_proto;
	filter_node->src_port = entry->src_port;
	filter_node->dst_port = entry->dst_port;
	filter_node->flags = entry->flags;

	return true;
}

bool del_filter_entry (PPA_HAL_ID hal_id, FILTER_INFO *entry)
{
	FILTER_INFO *filter_node;

	filter_node = get_matched_entry(hal_id, entry);

	if (!filter_node){
		printk("Entry not found in the list\n");
		return false;
	}

	list_del_init(&filter_node->list);
	ppa_free(filter_node);

	/* Free the head if list becomes empty*/
	if(g_session_filter_list[hal_id] != NULL) {
		if (list_empty(g_session_filter_list[hal_id])) {
			ppa_free(g_session_filter_list[hal_id]);
			g_session_filter_list[hal_id] = NULL;
		}
	}

	return true;
}

bool del_filter_list (PPA_HAL_ID hal_id)
{
	FILTER_INFO *filter_node;
	struct list_head *list_tmp = NULL;
	struct list_head *list_node = NULL;

	if (g_session_filter_list[hal_id] == NULL){
		printk("No Entries in filter list for HAL: %d\n",hal_id);
		return true;
	}

	list_for_each_safe(list_node, list_tmp, g_session_filter_list[hal_id]){
		filter_node = list_entry(list_node, FILTER_INFO, list);
		list_del(list_node);
		ppa_free(filter_node);
	}

	/* Free the head */
	if(g_session_filter_list[hal_id] != NULL) {
		if (list_empty(g_session_filter_list[hal_id])) {
			ppa_free(g_session_filter_list[hal_id]);
			g_session_filter_list[hal_id] = NULL;
		}
	}

	return true;
}

extern int ppa_api_register_hooks(void);
extern void ppa_api_unregister_hooks(void);

#if IS_ENABLED(CONFIG_PPA_IPTABLE_EVENT_HANDLING)
PPA_NOTIFIER_BLOCK ppa_iptable_notifier = {
	.notifier_call = ppa_iptable_event
};
#endif

void del_routing_session_cb(void *p_item)
{
	if(p_item) {
		ppa_hsel_del_routing_session((struct uc_session_node*)p_item);
	}
}

static int32_t ppa_add_lite0(void)
{
#if IS_ENABLED(CONFIG_SOC_GRX500)
	PPA_IFINFO ifinfo = {0};
	ifinfo.ifname = "lite0";
	ifinfo.if_flags = PPA_F_LAN_IF;

	if (ppa_add_if(&ifinfo, 0) != PPA_SUCCESS)
		ppa_debug(DBG_ENABLE_MASK_ERR, "lite0 addif failed!\n");
#endif
	return PPA_SUCCESS;
}

static int32_t ppa_init_new(PPA_INIT_INFO *p_info, uint32_t flags)
{
	int32_t ret = PPA_FAILURE;
	uint32_t i;
	PPA_HAL_INIT_CFG cfg;
#if IS_ENABLED(CONFIG_PPA_ACCEL)
	PPA_MAX_ENTRY_INFO entry={0};
#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
	PPA_ACC_ENABLE acc_cfg={0};
#endif /*IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)*/
#endif

	cfg.del_cb = del_routing_session_cb; /*Callback if the HAL wants to delete a HW entry*/
	if (ppa_drv_hal_init(&cfg, 0) != PPA_SUCCESS) {
		ret = PPA_EIO;
		goto HAL_INIT_ERROR;
	}
#if IS_ENABLED(CONFIG_PPA_ACCEL)

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
#if IS_ENABLED(CONFIG_NET_IPGRE)
	init_gre_hooks();
#endif/* IS_ENABLED(CONFIG_NET_IPGRE)*/
#endif /*LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)*/
	ppa_drv_get_max_entries(&entry, 0);
#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)

	ret = PPA_EINVAL;
	if (p_info->max_lan_source_entries + p_info->max_wan_source_entries > 
			(entry.max_lan_entries + entry.max_wan_entries)) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Too many entries:%d > %d\n",
			p_info->max_lan_source_entries + p_info->max_wan_source_entries,
			(entry.max_lan_entries + entry.max_wan_entries));
		goto MAX_SOURCE_ENTRIES_ERROR;
	}
#endif /*IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)*/

	if (p_info->max_mc_entries > entry.max_mc_entries) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Too many multicast entries:%d > %d\n",
				p_info->max_mc_entries , entry.max_mc_entries);
		goto MAX_MC_ENTRIES_ERROR;
	}

#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
	if (p_info->max_bridging_entries > entry.max_bridging_entries) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Too many bridge entries:%d > %d\n", 
				p_info->max_bridging_entries , entry.max_bridging_entries);
		goto MAX_BRG_ENTRIES_ERROR;
	}
#endif /*IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)*/

#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
	/*disable accelation mode by default*/
	acc_cfg.f_is_lan = 1;
	acc_cfg.f_enable = PPA_ACC_MODE_NONE;
	if(ppa_drv_set_acc_mode(&acc_cfg, 0) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_drv_set_acc_mode lan fail\n");
	}
	acc_cfg.f_is_lan = 0;
	acc_cfg.f_enable = PPA_ACC_MODE_NONE;
	if(ppa_drv_set_acc_mode(&acc_cfg, 0) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_drv_set_acc_mode	wan fail\n");
	}

	if ((entry.max_lan_entries + entry.max_wan_entries) || entry.max_mc_entries) {
		PPA_ROUTING_CFG cfg;

		/*set LAN acceleration*/
		ppa_memset(&cfg, 0, sizeof(cfg));
		cfg.f_is_lan = 1;
		cfg.entry_num = p_info->max_lan_source_entries;
		cfg.mc_entry_num = 0;
		cfg.flags = PPA_SET_ROUTE_CFG_ENTRY_NUM;
		if(ppa_drv_set_route_cfg(&cfg, 0) != PPA_SUCCESS)
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_drv_set_route_cfg lan fail\n");

		/*set WAN acceleration*/
		ppa_memset(&cfg, 0, sizeof(cfg));
		cfg.f_is_lan = 0;
		cfg.entry_num = p_info->max_wan_source_entries;
		cfg.mc_entry_num = p_info->max_mc_entries;
		cfg.flags = PPA_SET_ROUTE_CFG_ENTRY_NUM | PPA_SET_ROUTE_CFG_MC_ENTRY_NUM;
		if(ppa_drv_set_route_cfg(&cfg, 0) != PPA_SUCCESS)
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_drv_set_route_cfg wan fail\n");
	}
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/
#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
	if (entry.max_bridging_entries) {
		PPA_BRDG_CFG br_cfg;
		PPA_COUNT_CFG count={0};
		if (ppa_drv_get_number_of_phys_port(&count, 0) != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_drv_get_number_of_phys_port fail\n");
		}

		ppa_memset(&br_cfg, 0, sizeof(br_cfg));
		br_cfg.entry_num = p_info->max_bridging_entries;
		br_cfg.br_to_src_port_mask = (1 << count.num) - 1; /*br_to_src_port_mask*/
		br_cfg.flags = PPA_SET_BRIDGING_CFG_ENTRY_NUM | PPA_SET_BRIDGING_CFG_BR_TO_SRC_PORT_EN |
				PPA_SET_BRIDGING_CFG_DEST_VLAN_EN | PPA_SET_BRIDGING_CFG_SRC_VLAN_EN |
				PPA_SET_BRIDGING_CFG_MAC_CHANGE_DROP;

		if (ppa_drv_set_bridging_cfg(&br_cfg, 0) != PPA_SUCCESS)
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_drv_set_bridging_cfg	fail\n");
	}
#endif /*CONFIG_PPA_BR_SESS_LEARNING*/

	if ((ret = ppa_api_session_manager_init()) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_api_session_manager_init fail\n");
		goto PPA_API_SESSION_MANAGER_INIT_FAIL;
	}
	if(p_info->add_requires_min_hits)
		g_ppa_min_hits = p_info->add_requires_min_hits;

#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
	ppa_init_session_limit_params(p_info,0);
#endif
#endif /*def CONFIG_PPA_ACCEL*/

//*not able to find/map physical ports for PUMA and failing to init*/
#if defined(CONFIG_PPA_PUMA7) && CONFIG_PPA_PUMA7
	if ((ret = ppa_api_netif_manager_init()) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_api_netif_manager_init fail\n");
		goto PPA_API_NETIF_CREATE_INIT_FAIL;
	}
#else
	if ((ret = ppa_api_netif_manager_init()) != PPA_SUCCESS || !PPA_IS_PORT_CPU0_AVAILABLE()) {
		if(ret != PPA_SUCCESS)
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_api_netif_manager_init fail\n");
		else
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"CPU0 not available\n");
		goto PPA_API_NETIF_CREATE_INIT_FAIL;
	}
#endif

	for ( i = 0; i < p_info->num_lanifs; i++ )
		if ( p_info->p_lanifs[i].ifname != NULL && 
			ppa_netif_add(p_info->p_lanifs[i].ifname, 1, NULL, NULL,0) != PPA_SUCCESS )
			ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in adding LAN side network interface - %s,\
				reason could be no sufficient memory or LAN/WAN rule violation with \
				physical network interface.\n", p_info->p_lanifs[i].ifname);
	for (i = 0; i < p_info->num_wanifs; i++)
		if (p_info->p_wanifs[i].ifname != NULL &&
			ppa_netif_add(p_info->p_wanifs[i].ifname, 0, NULL, NULL,0) != PPA_SUCCESS)
			ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in adding WAN side network interface - %s,\
				reason could be no sufficient memory or LAN/WAN rule violation with \
				physical network interface.\n", p_info->p_wanifs[i].ifname);

#if IS_ENABLED(CONFIG_PPA_QOS_RATE_SHAPING)
	if(ppa_drv_init_qos_rate(0) != PPA_SUCCESS){
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_drv_init_qos_rate fail\n");
	}
#endif /* CONFIG_QOS_RATE_SHAPING */

#if IS_ENABLED(CONFIG_PPA_QOS_WFQ)
	if(ppa_drv_init_qos_wfq(0) != PPA_SUCCESS){
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_drv_init_qos_wfq fail\n");
	}
#endif /* CONFIG_PPA_QOS_WFQ */

#if IS_ENABLED(CONFIG_SOC_GRX500)
#if IS_ENABLED(CONFIG_LTQ_PPA_COC_SUPPORT)
	ppa_api_cpufreq_init();
#endif
#else
#if IS_ENABLED(CONFIG_LTQ_CPU_FREQ) || IS_ENABLED(CONFIG_LTQ_PMCU)
	ppa_pwm_init();
#endif
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	ppa_hook_session_ipsec_del_fn = ppa_session_ipsec_delete;
	ppa_hook_session_ipsec_add_fn = ppa_session_ipsec_add;
#endif
#endif
	ppa_set_init_status(PPA_INIT_STATE);
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
	ppa_sw_fastpath_enable(PPA_INIT_STATE);
#endif
#if !IS_ENABLED(CONFIG_PPA_PUMA7)
	/* enable the ppa learning netfileter hooks*/
	ppa_api_register_hooks();	
#endif /*!IS_ENABLED(CONFIG_PPA_PUMA7)*/
#if IS_ENABLED(CONFIG_PPA_IPTABLE_EVENT_HANDLING)
	ppa_register_iptable_notifier(&ppa_iptable_notifier);
#endif

	ppa_add_lite0();

	return PPA_SUCCESS;

#if IS_ENABLED(CONFIG_PPA_ACCEL)
PPA_API_SESSION_MANAGER_INIT_FAIL:
#endif
	ppa_api_session_manager_exit();
PPA_API_NETIF_CREATE_INIT_FAIL:
	ppa_api_netif_manager_exit();
#if IS_ENABLED(CONFIG_PPA_ACCEL)
#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
MAX_BRG_ENTRIES_ERROR:
#endif /*IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)*/
MAX_MC_ENTRIES_ERROR:
#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
MAX_SOURCE_ENTRIES_ERROR:
#endif /*IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)*/
#endif
	ppa_drv_hal_exit(0);
HAL_INIT_ERROR:
	ppa_debug(DBG_ENABLE_MASK_ERR,"failed in PPA init\n");
	return ret;
}

void ppa_api_exit(void)
{
#if !IS_ENABLED(CONFIG_PPA_PUMA7)
	/* un register the ppa learning netfilter hooks */
	ppa_api_unregister_hooks();
#endif /*!IS_ENABLED(CONFIG_PPA_PUMA7)*/
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
#if IS_ENABLED(CONFIG_NET_IPGRE)
	uninit_gre_hooks();
#endif/* IS_ENABLED(CONFIG_NET_IPGRE)*/
#endif /*LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)*/

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	ppa_hook_session_ipsec_del_fn = NULL;
	ppa_hook_session_ipsec_add_fn = NULL;
#endif
	ppa_set_init_status(PPA_UNINIT_STATE);
	ppa_synchronize_rcu();
#if IS_ENABLED(CONFIG_SOC_GRX500)
#if IS_ENABLED(CONFIG_LTQ_PPA_COC_SUPPORT)
	ppa_api_cpufreq_exit();
#endif
#else
#if IS_ENABLED(CONFIG_LTQ_CPU_FREQ) || IS_ENABLED(CONFIG_LTQ_PMCU)
	ppa_pwm_exit();
#endif
#endif
	ppa_api_session_manager_exit();
	ppa_api_netif_manager_exit();
	ppa_drv_hal_exit(0);
#if IS_ENABLED(CONFIG_SOC_GRX500)
	free_fid_list();
#endif
#if IS_ENABLED(CONFIG_PPA_IPTABLE_EVENT_HANDLING)
	ppa_unregister_iptable_notifier(&ppa_iptable_notifier);
#endif
	printk(KERN_INFO "Acceleration module exited!!!\n");
}

/*
 * ####################################
 *			 Global Function
 * ####################################
 */
/*
 *	PPA Initialization Functions
 */

void ppa_subsystem_id(uint32_t *p_family,
		uint32_t *p_type,
		uint32_t *p_if,
		uint32_t *p_mode,
		uint32_t *p_major,
		uint32_t *p_mid,
		uint32_t *p_minor,
		uint32_t *p_tag)
{
	char *s_version = NULL, *token = NULL;
	unsigned long u_version[VERSION_MAX] = {0, 0, 0, 0};
	int version_index = 0, ret = PPA_SUCCESS;

	if (p_family)
		*p_family = 0;

	if (p_type)
		*p_type = 0;

	if (p_if)
		*p_if = 0;

	if (p_mode)
		*p_mode = 0;

	s_version = (char *)ppa_malloc(VERSION_STR_LEN * sizeof(char *));
	if (s_version == NULL)
		goto exit_lbl;

	ppa_memset(s_version, '\0', VERSION_STR_LEN);
	ppa_strncpy(s_version, g_ppa_ss_version, strnlen(g_ppa_ss_version, VERSION_STR_LEN) + 1);

	token = strsep(&s_version, ".");
	while (token != NULL) {
		ret = kstrtoul(token, 10, &u_version[version_index]);
		if (ret != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_ERR, "<%s> Invalid version number!\n", __FUNCTION__);
		}
		version_index++;
		token = strsep(&s_version, ".");
	}

	if (p_major)
		*p_major = u_version[VERSION_MAJOR];

	if (p_mid)
		*p_mid = u_version[VERSION_MID];

	if (p_minor)
		*p_minor = u_version[VERSION_MINOR];

	if (p_tag)
		*p_tag = u_version[VERSION_TAG];

	if (!strcmp(g_ppa_ss_version, PPA_SS_DEFAULT_VERSION))
		ppa_debug(DBG_ENABLE_MASK_ERR, "<%s> PPA Subsystem Version is NOT defined !\n", __FUNCTION__);

exit_lbl:
	if (s_version != NULL)
		ppa_free(s_version);
}

void ppa_get_api_id(uint32_t *p_family,
		uint32_t *p_type,
		uint32_t *p_if,
		uint32_t *p_mode,
		uint32_t *p_major,
		uint32_t *p_mid,
		uint32_t *p_minor)
{
	if (p_family)
		*p_family = VER_FAMILY;

	if (p_type)
		*p_type = VER_DRTYPE;

	if (p_if)
		*p_if = VER_INTERFACE;

	if (p_mode)
		*p_mode = VER_ACCMODE;

	if (p_major)
		*p_major = VER_MAJOR;

	if (p_mid)
		*p_mid = VER_MID;

	if (p_minor)
		*p_minor = VER_MINOR;
}

int32_t ppa_init(PPA_INIT_INFO *p_info, uint32_t flags)
{
	int32_t ret = PPA_FAILURE, i;

	if (!p_info)
		return PPA_EINVAL;

	if (ppa_is_init())
		ppa_exit();

	if ((ret = ppa_init_new(p_info, flags)) == PPA_SUCCESS)
		printk("ppa_init - init succeeded\n");
	else
		printk("ppa_init - init failed (%d)\n", ret);

	/* Add default Session filters */
	for (i=0; i<(sizeof(axglobal_filters)/sizeof(FILTER_INFO)); i++) {
		add_filter_entry (MAX_HAL, &axglobal_filters[i]);
	}
	for (i=0; i<(sizeof(axsae_filters)/sizeof(FILTER_INFO)); i++) {
		add_filter_entry (SWAC_HAL, &axsae_filters[i]);
	}

	return ret;
}

void ppa_exit(void)
{
	PPA_HAL_ID i;
	if (ppa_is_init()) {
		/* Delete all Session filter tables */
		for (i=0; i<=MAX_HAL; i++) {
			del_filter_list (i);
		}
		ppa_api_exit();
	}
}

/*
 *	PPA Enable/Disable and Status Functions
 */
int32_t ppa_enable(uint32_t lan_rx_ppa_enable, uint32_t wan_rx_ppa_enable, uint32_t flags)
{
	u32 sys_flag;
	PPA_ACC_ENABLE acc_cfg;

	if (ppa_is_init()) {
		lan_rx_ppa_enable = lan_rx_ppa_enable ? PPA_ACC_MODE_ROUTING : PPA_ACC_MODE_NONE;
		wan_rx_ppa_enable = wan_rx_ppa_enable ? PPA_ACC_MODE_ROUTING : PPA_ACC_MODE_NONE;
		sys_flag = ppa_disable_int();
		acc_cfg.f_is_lan =1;
		acc_cfg.f_enable = lan_rx_ppa_enable;
		ppa_drv_set_acc_mode(&acc_cfg, 0);

		acc_cfg.f_is_lan =0;
		acc_cfg.f_enable = wan_rx_ppa_enable;
		ppa_drv_set_acc_mode(&acc_cfg, 0);
		ppa_enable_int(sys_flag);
		return PPA_SUCCESS;
	}
	return PPA_FAILURE;
}

#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
/*
 *	PPA Set Unicast/multicast session mib mode configuration
 */
int32_t ppa_set_mib_mode(uint8_t mib_mode)
{
	PPA_MIB_MODE_ENABLE mib_cfg;

	mib_cfg.session_mib_unit = mib_mode;

	ppa_drv_set_mib_mode(&mib_cfg,0);

	return PPA_SUCCESS;
}
/*
 *	PPA Get Unicast/multicast session mib mode configuration
 */
int32_t ppa_get_mib_mode(uint8_t *mib_mode)
{
	PPA_MIB_MODE_ENABLE mib_cfg;

	ppa_drv_get_mib_mode(&mib_cfg);

	*mib_mode =mib_cfg.session_mib_unit;

	return PPA_SUCCESS;
}
#endif
int32_t ppa_get_status(uint32_t *lan_rx_ppa_enable, uint32_t *wan_rx_ppa_enable, uint32_t flags)
{
	if (ppa_is_init()) {
		PPA_ACC_ENABLE cfg;

		cfg.f_is_lan = 1;
		ppa_drv_get_acc_mode(&cfg, 0);
		if(lan_rx_ppa_enable) *lan_rx_ppa_enable = cfg.f_enable;

		cfg.f_is_lan = 0;
		ppa_drv_get_acc_mode(&cfg, 0);
		if(wan_rx_ppa_enable) *wan_rx_ppa_enable = cfg.f_enable;
		return PPA_SUCCESS;
	}
	return PPA_FAILURE;
}

int32_t ppa_hook_set_ppe_fastpath_enable(uint32_t f_enable, uint32_t flags)
{
	g_ppe_fastpath_enabled = f_enable;
	return PPA_SUCCESS;
}

int32_t ppa_hook_get_ppe_fastpath_enable(uint32_t *f_enable, uint32_t flags)
{
	if(f_enable)
		*f_enable = g_ppe_fastpath_enabled;
	return PPA_SUCCESS;
}

void ppa_pitem_refcnt_inc(PPA_BUF *ppa_buf)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	struct uc_session_node *p_item = NULL;
	if (ppa_buf->ptr_ppa_pitem) {
		p_item = (struct uc_session_node *)(ppa_buf->ptr_ppa_pitem);
		ppa_atomic_inc_not_zero(&p_item->used);
	}
#endif
}

void ppa_pitem_refcnt_dec(PPA_BUF *ppa_buf)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	struct uc_session_node *p_item = NULL;
	if (ppa_buf->ptr_ppa_pitem) {
		p_item = (struct uc_session_node *)(ppa_buf->ptr_ppa_pitem);
		ppa_session_put(p_item);
	}
	ppa_buf->ptr_ppa_pitem = NULL;
#endif
}

#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
static int32_t ppa_pkt_filter(PPA_BUF *ppa_buf, PPA_SESSION *session, uint32_t flags)
{
	/*basic pkt filter*/
	PPA_NETIF *tx_if;
	PPA_IFNAME *ifname;
	struct netif_info *p_info;
	uint32_t flag=0;
	uint16_t sport,dport;
	uint8_t proto;

	/*ignore packets output by the device*/
	if (ppa_is_pkt_host_output(ppa_buf)) {
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH) && CONFIG_PPA_TCP_LITEPATH
		if( ( ppa_get_pkt_ip_proto(ppa_buf) != PPA_IPPROTO_TCP ) ) {
			ppa_filter.ppa_is_pkt_host_output++;
			goto __PPA_SESSION_FILTED;
		}
#else
		ppa_filter.ppa_is_pkt_host_output++;
		goto __PPA_SESSION_FILTED;
#endif
	}

	/* ignore incoming broadcast*/
	if (ppa_is_pkt_broadcast(ppa_buf)) {
		ppa_filter.ppa_is_pkt_broadcast++;
		goto __PPA_SESSION_FILTED;
	}

	/* ignore loopback packet*/
	if (ppa_is_pkt_loopback(ppa_buf)) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG2_PRINT,"ppa_is_pkt_loopback\n");
		ppa_filter.ppa_is_pkt_loopback++;
		return PPA_SESSION_FILTED;
	}

	/* ignore multicast packet in unitcast routing but learn multicast source interface automatically*/
	if ( ppa_is_pkt_multicast(ppa_buf)) {

		if(ppa_get_pkt_ip_proto(ppa_buf) != PPA_IPPROTO_UDP) {
			ppa_filter.ppa_is_pkt_protocol_invalid++;
			goto __PPA_SESSION_FILTED;
		}
 
		ppa_debug(DBG_ENABLE_MASK_DEBUG2_PRINT, "ppa_is_pkt_multicast\n");

		/*auto learn multicast source interface*/
		if (flags & PPA_F_BEFORE_NAT_TRANSFORM)
			ppa_multicast_pkt_srcif_add(ppa_buf, NULL);
		goto __PPA_SESSION_FILTED;
	}



	/* ignore protocols other than TCP/UDP, since some of them (e.g. ICMP) can't be handled safe in this arch*/
	proto = ppa_get_pkt_ip_proto(ppa_buf);
	if ((proto != PPA_IPPROTO_UDP) && (proto != PPA_IPPROTO_TCP)
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	    && (proto != IP_PROTO_ESP)
#endif
	   ) {
		ppa_filter.ppa_is_pkt_protocol_invalid++;
		goto __PPA_SESSION_FILTED;
	}

	sport = ppa_get_pkt_src_port(ppa_buf);
	dport = ppa_get_pkt_dst_port(ppa_buf);
	if (sport == 1701 || dport == 1701 ||
		dport == 8472 || dport == 4789 || dport == 4790) {
			ppa_filter.ppa_is_pkt_ignore_special_session++;
			goto __PPA_SESSION_FILTED;
	}

	/* ignore fragment packet*/
	if (ppa_is_pkt_fragment(ppa_buf)) {

		ppa_filter.ppa_is_pkt_fragment++;
		goto __PPA_SESSION_FILTED;
	}

	tx_if = ppa_get_pkt_dst_if(ppa_buf);
	if (tx_if) {
		ifname = ppa_get_netif_name(tx_if);
	} else {
		goto skip_lookup;
	}

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	if(sport == 500) {
		ppa_get_pkt_src_ip(&ipsec_tnl_info.src_ip, ppa_buf);
		ipsec_tnl_info.tx_if		 = tx_if;
	}
#endif

	if(ppa_netif_lookup(ifname, &p_info) != PPA_SUCCESS) {
		return PPA_ENOTAVAIL;
	}

	flag = p_info->flags;
	ppa_netif_put(p_info);

skip_lookup:
	if(!(flags & PPA_F_BEFORE_NAT_TRANSFORM)){
		/* handle routed packet only*/ 
		if(!(flag & NETIF_PPPOL2TP)) {
			if (!ppa_is_pkt_routing(ppa_buf)) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"not routing packet\n");
				return PPA_SESSION_FILTED;
			}
		}
	}

	return PPA_SESSION_NOT_FILTED;

__PPA_SESSION_FILTED:

	ppa_filter.ppa_is_filtered++;
	return PPA_SESSION_FILTED;

}
/*
 *	PPA Routing Session Operation Functions
 */
int32_t ppa_session_add(PPA_BUF *ppa_buf, PPA_SESSION *p_session, uint32_t flags)
{
	int32_t ret = PPA_SESSION_NOT_ADDED;
	struct uc_session_node* p_item = NULL;
	uint32_t idx;

	if(!ppa_buf){
		return PPA_SESSION_NOT_ADDED;
	}

 	if(ppa_is_pkt_loopback(ppa_buf))
		return PPA_SESSION_NOT_ADDED;

	if((flags & PPA_F_BRIDGED_SESSION) && !g_bridged_flow_learning)
		return PPA_SESSION_NOT_ADDED; /* No bridged flow learning */

	if (p_session) {
		ret = ppa_session_find_by_tuple(p_session,
				flags & PPA_F_SESSION_REPLY_DIR,
				&p_item);
	} else {
		ret = ppa_find_session_from_skb(ppa_buf, 0, &p_item);
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
/* Store the pointer to p_item in skb and increment the reference counter */
	if (!ppa_buf->ptr_ppa_pitem && PPA_SESSION_EXISTS == ret && p_item) {
		ppa_buf->ptr_ppa_pitem = (void *)(p_item);
		ppa_atomic_inc_not_zero(&p_item->used);
	}

	if (ppa_buf->ptr_ppa_pitem && PPA_SESSION_EXISTS != ret) {
		p_item = (struct uc_session_node *)(ppa_buf->ptr_ppa_pitem);
		ppa_atomic_inc_not_zero(&p_item->used);
		ret = PPA_SESSION_EXISTS;
 	}
#endif
	if (PPA_SESSION_EXISTS != ret) {

		if(unlikely(NULL == p_session && !(flags & PPA_F_BRIDGED_SESSION))) {
			return PPA_SESSION_NOT_ADDED;
		}

		if(likely(flags & PPA_F_BEFORE_NAT_TRANSFORM)) {
			/* Filter packet */
			if((ret = ppa_pkt_filter(ppa_buf, p_session, flags)) == PPA_SESSION_FILTED) {
				return ret;
			}

			if (ppa_alloc_session(ppa_buf, p_session, flags, &p_item) != PPA_SUCCESS) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_alloc_session failed\n");
				return PPA_SESSION_NOT_ADDED;
			}
		} else {
			return PPA_SESSION_NOT_ADDED;
		}
	}

	if (p_item == NULL) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Could not allocate the session\n");
		return PPA_SESSION_NOT_ADDED;
	}

	idx = ppa_session_get_index(p_item->hash);
	ppa_session_bucket_lock(idx);

	if(p_session && p_item && ppa_is_BrSession(p_item)) {
		/* When control comes here, it must be from PRE routing hook
			 from routed path */
		/* NOTE : Don't consider LOCAL_OUT session, possibly a locally terminated Layer-3 tunnel. */
		if ((flags & PPA_F_BEFORE_NAT_TRANSFORM) && !(flags & PPA_F_SESSION_LOCAL_OUT))
			ppa_session_not_bridged(p_item,p_session);
#ifdef CONFIG_PPA_PP_LEARNING
		p_item->rx_if = ppa_get_pkt_src_if(ppa_buf);
#endif
		goto done;/* Packet is already seen in bridged path */
	}

	if(p_item && ! ppa_is_BrSession(p_item) && (flags & PPA_F_BRIDGED_SESSION) &&
			(flags & PPA_F_BEFORE_NAT_TRANSFORM))
		goto done; /*Routed session...but seen in bridged path*/
	/* Note: ppa_speed_handle_frame returns PPA_SESSION_NOT_FILTED only in POST
		 routing */
	if (p_item)
	{
		if (flags & PPA_F_BEFORE_NAT_TRANSFORM)
			ret = ppa_prerouting_speed_handle_frame(ppa_buf, p_item, flags);
		else
			ret = ppa_postrouting_speed_handle_frame(ppa_buf, p_item, flags);
		if (ret == PPA_SESSION_NOT_FILTED) {
			/* in case compiler optimization problem*/
			PPA_SYNC();
			ret = ppa_update_session(ppa_buf, p_item, flags);
		}
	}

done:
	__ppa_session_put(p_item);
	ppa_session_bucket_unlock(idx);
	return ret;
}

int32_t ppa_session_modify(PPA_SESSION *p_session, PPA_SESSION_EXTRA *p_extra, uint32_t flags)
{
	struct uc_session_node *p_item=NULL;
	int32_t ret = PPA_FAILURE;
	uint32_t idx;

	/* Since session parameters being updated, so take spin lock */
	if (PPA_SESSION_EXISTS == __ppa_session_find_by_ct(p_session,
				flags & PPA_F_SESSION_REPLY_DIR, &p_item)) {

		ppa_update_session_extra(p_extra, p_item, flags);
		idx = ppa_session_get_index(p_item->hash);
		ppa_session_bucket_lock(idx);
		if ((p_item->flags & SESSION_ADDED_IN_HW) && (flags != 0) ) {

			if(!(p_item->flags & SESSION_NOT_ACCEL_FOR_MGM)
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
					&& !(p_item->flags & SESSION_ADDED_IN_SW)
#endif
			 ) {

				if (ppa_hw_update_session_extra(p_item, flags) != PPA_SUCCESS) {
					/* update failed*/
					ppa_hsel_del_routing_session(p_item);
#ifdef CONFIG_PPA_API_SW_FASTPATH
					/* session was in hardware an the modification failed; so the 
						 session is moved out of HW and put in SW fastpath.
						 software session management is handled by SAE */
					ppa_sw_session_enable(p_item, 1, p_item->flags);
#endif
					ppa_session_bucket_unlock(idx);
					goto __MODIFY_DONE;
				}
			} else {
				/*just remove the accelerated session from PPE FW, no need to update
				other flags since PPA hook will rewrite them.*/
				ppa_hsel_del_routing_session(p_item);
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH) && defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
				update_session_mgmt_stats(p_item, ADD);
#endif
			}
		}
		ppa_session_bucket_unlock(idx);
		ret = PPA_SUCCESS;
	} else{
		ret = PPA_FAILURE;
	}
__MODIFY_DONE:
	if(p_item) ppa_session_put(p_item);
	return ret;
}

int32_t ppa_session_get(PPA_SESSION ***pp_sessions, PPA_SESSION_EXTRA **pp_extra, int32_t *p_num_entries, uint32_t flags)
{
	/*#warning ppa_session_get is not implemented*/
	return PPA_ENOTIMPL;
}
/*
 *	PPA IPSec Session Operation Functions
 */
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
int32_t ppa_session_ipsec_add(PPA_XFRM_STATE *ppa_x, sa_direction dir)
{
	int32_t ret = PPA_SESSION_NOT_ADDED;
	uint32_t tunnel_index =0;

	if(!ppa_x){
		return PPA_SESSION_NOT_ADDED;
	}
	if(ppa_add_ipsec_tunnel_tbl_entry(ppa_x,dir,&tunnel_index) != PPA_SUCCESS)
		return PPA_SESSION_NOT_ADDED;

	ppa_ipsec_get_session_lock();

	if(dir == INBOUND) {
		ret =	ppa_ipsec_add_entry(tunnel_index);

	} else {
		ret =	ppa_ipsec_add_entry_outbound(tunnel_index);
	}

	ppa_ipsec_release_session_lock();

	return ret;
}

int32_t ppa_session_ipsec_delete(PPA_XFRM_STATE *ppa_x)
{
	int32_t ret = PPA_SESSION_NOT_DELETED;
	struct uc_session_node *p_item;
	uint32_t tunnel_index =0;
	sa_direction dir;

	if(!ppa_x){
		return PPA_SESSION_NOT_DELETED;
	}
	if(ppa_get_ipsec_tunnel_tbl_entry(ppa_x,&dir,&tunnel_index) != PPA_SUCCESS)
		return PPA_SESSION_NOT_DELETED;

	ppa_ipsec_get_session_lock();

	if(dir == INBOUND) {
		ret = __ppa_lookup_ipsec_group(ppa_x, &p_item);
		if (ret	== PPA_IPSEC_EXISTS)
			ret =	ppa_ipsec_del_entry(p_item);
		else
			ret = PPA_SESSION_NOT_DELETED;
	} else
		ret =	ppa_ipsec_del_entry_outbound(tunnel_index);

	ppa_add_ipsec_tunnel_tbl_update(dir, tunnel_index);
	ppa_ipsec_release_session_lock();

	return ret;
}
#endif
#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
uint8_t ppa_session_bridged_flow_status(void)
{
	return g_bridged_flow_learning;
}
EXPORT_SYMBOL(ppa_session_bridged_flow_status);

void ppa_session_bridged_flow_set_status(uint8_t fEnable)
{
	if(fEnable) {
		ppa_reg_export_fn(PPA_SESSION_BRADD_FN,
				ppa_session_add,
				"ppa_session_bradd",
				(void *)&ppa_hook_session_bradd_fn,
				ppa_session_bradd_rpfn);
	} else {
		/* Un-register the hooks */
		ppa_unreg_export_fn(PPA_SESSION_BRADD_FN,
				(void *)&ppa_hook_session_bradd_fn);
	}
	g_bridged_flow_learning = fEnable;
}
EXPORT_SYMBOL(ppa_session_bridged_flow_set_status);
#endif /*CONFIG_PPA_BR_SESS_LEARNING*/
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/

int32_t ppa_mc_check_ip(PPA_MC_GROUP *ppa_mc_entry)
{
	if(is_ip_zero(&ppa_mc_entry->ip_mc_group)) {
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_mc_group_update not support zero ip address\n");
		return PPA_FAILURE;
	}

	if(is_ip_allbit1(&ppa_mc_entry->source_ip)){
		if(g_ppa_dbg_enable & DBG_ENABLE_MASK_DUMP_MC_GROUP){
			ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP,
				"source ip not support all bit 1 ip address, except dbg enabled\n");
		}else{
			return PPA_FAILURE;
		}
	}
	if(is_ip_zero(&ppa_mc_entry->source_ip)){
		if (ppa_mc_entry->SSM_flag == 1){/*Must provide src ip if SSM_flag is set*/
			ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "SMM flag set but no souce ip provided\n");
			return PPA_FAILURE;
		}
	}else if(ppa_mc_entry->ip_mc_group.f_ipv6 !=
			ppa_mc_entry->source_ip.f_ipv6){ /*mc group ip & source ip must both be ipv4 or ipv6*/
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "MC group IP and source ip not in same IPv4/IPv6 type\n");
		return PPA_FAILURE;
	}

	return PPA_SUCCESS;
}

int32_t ppa_mc_add_entry(PPA_MC_GROUP *ppa_mc_entry, uint32_t flags)
{
	struct mc_session_node *p_item;

	/*Add new mc groups*/
	if (ppa_add_mc_group(ppa_mc_entry, &p_item, flags) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_add_mc_group fail\n");
		return PPA_SESSION_NOT_ADDED;
	}

	p_item->num_adds = 0;
	if (p_item->grp.src_netif == NULL) {
	/* only added in PPA level, not PPE FW level since source interface not get yet.*/
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "IGMP request no src_netif. No acceleration !\n");
		return PPA_SESSION_ADDED;
	}

	/*Add HW entry */
	if (ppa_hsel_add_wan_mc_group(p_item, &ppa_mc_entry->cop) != PPA_SUCCESS) {
		p_item->num_adds ++;
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_hw_add_mc_group(%d.%d.%d.%d): fail",
			ppa_mc_entry->ip_mc_group.ip.ip >> 24, (ppa_mc_entry->ip_mc_group.ip.ip >> 16) & 0xFF,
			(ppa_mc_entry->ip_mc_group.ip.ip >> 8) & 0xFF, ppa_mc_entry->ip_mc_group.ip.ip & 0xFF);
			/*keep p_item added in PPA level*/
			return PPA_SESSION_ADDED;
	}
	ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "hardware add Success \n");

	return PPA_SESSION_ADDED;
}

/*
 *	PPA Multicast Routing Session Operation Functions
 */

int32_t ppa_mc_group_update(PPA_MC_GROUP *ppa_mc_entry, uint32_t flags)
{
	struct mc_session_node *p_item;
	int32_t ret;

	if(ppa_mc_entry->ip_mc_group.f_ipv6 == 0) {
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, 
			"ppa_mc_group_update for group: %d.%d.%d.%d \n", NIPQUAD(ppa_mc_entry->ip_mc_group.ip.ip));
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP,
			"source ip : %d.%d.%d.%d \n", NIPQUAD(ppa_mc_entry->source_ip.ip.ip));
	} else {
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP,
			"group ip: "NIP6_FMT"\n", NIP6(ppa_mc_entry->ip_mc_group.ip.ip6));
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP,
			"source ip: "NIP6_FMT"\n", NIP6(ppa_mc_entry->source_ip.ip.ip6));
	}
	ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "from %s ", ppa_mc_entry->src_ifname ? ppa_mc_entry->src_ifname: "NULL");

	ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "to ");
	if(ppa_mc_entry->num_ifs ==0)
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "NULL");
	else {
		int i, bit;
		for (i = 0, bit = 1; i < PPA_MAX_MC_IFS_NUM; i++) {
			if (ppa_mc_entry->if_mask & bit)
				ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "%s ",
					ppa_mc_entry->array_mem_ifs[i].ifname? ppa_mc_entry->array_mem_ifs[i].ifname:"NULL");
				bit = bit<<1;
		}
	}
	ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "with ssm=%d flags=%x\n",	ppa_mc_entry->SSM_flag, flags);
	ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "lan itf num:%d, mask:%x\n",	ppa_mc_entry->num_ifs, ppa_mc_entry->if_mask);

	if (ppa_mc_check_ip(ppa_mc_entry) != PPA_SUCCESS)
		return PPA_FAILURE;

	/*If the current operation is IF_DEL and the num_if = 0;
		it is the last client leaving group */
	if ((ppa_mc_entry->num_ifs == 0) && (ppa_mc_entry->cop.flag == PPA_IF_DEL)) {
		/*Delete the group: if SMM flag == 0, don't care src ip*/
		ppa_delete_mc_group(ppa_mc_entry);
		return PPA_SUCCESS;
	}

	ppa_mc_get_htable_lock();
	ret = __ppa_lookup_mc_group(&ppa_mc_entry->ip_mc_group, &ppa_mc_entry->source_ip, &p_item);
	if(ret == PPA_MC_SESSION_VIOLATION) {/*Cannot add or update*/
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "IGMP violation, cannot be added or updated\n");
		ret = PPA_FAILURE;
	} else if (ret	!= PPA_SESSION_EXISTS){
		ret =	ppa_mc_add_entry(ppa_mc_entry, flags);
	} else {
		ret = __ppa_mc_group_update(ppa_mc_entry, p_item, flags);
	}
	ppa_mc_release_htable_lock();

	return ret;
}

int32_t ppa_mc_entry_modify(IP_ADDR_C ip_mc_group, IP_ADDR_C src_ip, PPA_MC_GROUP *ppa_mc_entry, PPA_SESSION_EXTRA *p_extra, uint32_t flags)
{
	struct mc_session_node *p_item;
	int32_t ret = PPA_FAILURE;

	ppa_mc_get_htable_lock();
	if (__ppa_lookup_mc_group(&ip_mc_group, &src_ip, &p_item) == PPA_SESSION_EXISTS) {
		ppa_update_mc_group_extra(p_extra, p_item, flags);
		if ((p_item->flags & SESSION_ADDED_IN_HW)) {
			if (ppa_hsel_update_wan_mc_group(p_item, &ppa_mc_entry->cop, flags) != PPA_SUCCESS) {
				ppa_remove_mc_group(p_item);
				ret = PPA_FAILURE;
			}
		}
		ret = PPA_SUCCESS;
	}
	ppa_mc_release_htable_lock();

	return ret;
}

int32_t ppa_mc_entry_get(IP_ADDR_C ip_mc_group, IP_ADDR_C src_ip, PPA_SESSION_EXTRA *p_extra, uint32_t flags)
{
	struct mc_session_node *p_item;
	int32_t ret = PPA_FAILURE;

	if (!p_extra)
		return PPA_EINVAL;

	ppa_mc_get_htable_lock();
	if (__ppa_lookup_mc_group(&ip_mc_group, &src_ip, &p_item) == PPA_SESSION_EXISTS) {

		ppa_memset(p_extra, 0, sizeof(*p_extra));
		p_extra->session_flags = flags;
		if ((flags & PPA_F_SESSION_NEW_DSCP)) {
			if ((p_item->flags & SESSION_VALID_NEW_DSCP)) {
				p_extra->dscp_remark = 1;
				p_extra->new_dscp = p_item->grp.new_dscp;
			}
		}

		if ((flags & PPA_F_SESSION_VLAN)) {
			if ((p_item->flags & SESSION_VALID_VLAN_INS)) {
				p_extra->vlan_insert = 1;
				p_extra->vlan_prio = p_item->grp.new_vci >> 13;
				p_extra->vlan_cfi = (p_item->grp.new_vci >> 12) & 0x01;
				p_extra->vlan_id = p_item->grp.new_vci & ((1 << 12) - 1);
			}
			if ((p_item->flags & SESSION_VALID_VLAN_RM))
				p_extra->vlan_remove = 1;
		}

		if ((flags & PPA_F_SESSION_OUT_VLAN)) {
			if ((p_item->flags & SESSION_VALID_OUT_VLAN_INS)) {
				p_extra->out_vlan_insert = 1;
				p_extra->out_vlan_tag = p_item->grp.out_vlan_tag;
			}

			if ((p_item->flags & SESSION_VALID_OUT_VLAN_RM))
				p_extra->out_vlan_remove = 1;
		}

		p_extra->dslwan_qid_remark = 1;
		p_extra->dslwan_qid = p_item->dslwan_qid;
		p_extra->out_vlan_tag = p_item->grp.out_vlan_tag;

		ret = PPA_SUCCESS;
	}
	ppa_mc_release_htable_lock();

	return ret;
}

#if defined(RTP_SAMPLING_ENABLE) && RTP_SAMPLING_ENABLE
int32_t ppa_mc_entry_rtp_get(IP_ADDR_C ip_mc_group, IP_ADDR_C src_ip, uint8_t* p_RTP_flag)
{
	struct mc_session_node *p_item;
	int32_t ret = PPA_FAILURE;

	if (!p_RTP_flag)
		return PPA_EINVAL;

	ppa_mc_get_htable_lock();
	if (__ppa_lookup_mc_group(&ip_mc_group, &src_ip, &p_item) == PPA_SESSION_EXISTS) {
		*p_RTP_flag = p_item->RTP_flag;
		ret = PPA_SUCCESS;
	}
	ppa_mc_release_htable_lock();

	return ret;
}
#endif
/*
 *	PPA Unicast Session Timeout Functions
 */
int32_t ppa_inactivity_status(PPA_U_SESSION *p_session)
{
	int f_timeout = 1;
#if 0
/***********************************************************
OBSOLETE:  This callback ins not invoked in currentSOCs 
***********************************************************/
	int f_flag = 0;
	int32_t ret;
	struct uc_session_node *p_item;
	uint32_t timeDiff;

	p_item = NULL;
	ret = ppa_session_find_by_ct((PPA_SESSION *)p_session, 0, &p_item);
	if(ret == PPA_SESSION_EXISTS) {

		if(p_item->pkt.ip_proto == PPA_IPPROTO_TCP &&
				! ppa_is_tcp_established(p_session)) {

			ppa_session_put(p_item);
			return PPA_TIMEOUT;
		}
		if(p_item->flags & (SESSION_ADDED_IN_HW | SESSION_ADDED_IN_SW)) {

			f_flag = 1;
			timeDiff = ppa_get_time_in_sec() - p_item->last_hit_time;
			if (p_item->timeout >= timeDiff)
				f_timeout = 0;
			ppa_debug(DBG_ENABLE_MASK_DEBUG2_PRINT,
					"session %px, timeout=%u Time since last hit=%u\n",
					p_session, p_item->timeout, timeDiff);
		}

		ppa_session_put(p_item);
	}
	ret = ppa_session_find_by_ct((PPA_SESSION *)p_session, 1, &p_item);

	if(ret == PPA_SESSION_EXISTS) {
		if(p_item->flags & (SESSION_ADDED_IN_HW | SESSION_ADDED_IN_SW)) {

			f_flag = 1;
			timeDiff = ppa_get_time_in_sec() - p_item->last_hit_time;
			if(p_item->timeout >= timeDiff)
				f_timeout = 0;
			ppa_debug(DBG_ENABLE_MASK_DEBUG2_PRINT,
					"session %px, timeout=%u Time since last hit=%u\n",
					p_session, p_item->timeout, timeDiff);
		}
		ppa_session_put(p_item);
	}

	if(g_ppa_dbg_enable & DBG_ENABLE_MASK_SESSION){
		/*if session dbg enable, keep it from timeout*/
		return PPA_HIT;
	}
	/* not added in hardware*/
	if (!f_flag)
		return PPA_SESSION_NOT_ADDED;
#endif
	return f_timeout ? PPA_TIMEOUT : PPA_HIT;
}

int32_t ppa_set_session_inactivity(PPA_U_SESSION *p_session, int32_t timeout)
{
	return PPA_SUCCESS;
}

/*
 *	PPA Bridge MAC learning Operation Functions
 */
int32_t ppa_bridge_entry_add(uint8_t *mac_addr, PPA_NETIF *brif, PPA_NETIF *netif, uint32_t flags)
{
	struct br_mac_node *p_item;
	int32_t ret = PPA_SESSION_NOT_ADDED;
	uint16_t fid=0;
	struct netif_info *ifinfo = NULL;
	uint8_t br_item_updated = 0;
	uint8_t is_del_routing_session = 0;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	uint32_t cur_time = 0;
	if(ppa_get_fid(ppa_get_netif_name(brif), &fid)!=PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in getting fid of bridge %s\n", ppa_get_netif_name(brif));
	}
#else
	if(flags & PPA_F_BRIDGE_ACCEL_MODE) {
	/* update operation not needed for legacy platforms*/
		return PPA_SUCCESS;
	}
#endif

	if(!g_bridging_mac_learning) return PPA_FAILURE;

	if(ppa_netif_lookup(ppa_get_netif_name(netif), &ifinfo) != PPA_SUCCESS)
		return ret;

	/* - Mahipati -
	 * Exception: Don't add eogre interfaces
	 * - IPoGRE and EoGRE does not terminate on same MAC !!
	 * Exception: Don't add wifi vlan interfaces
	 * - Accelerator/PAE can't handle subif action for wifi vap and vlan sametime.
	 */
	if ((!(flags & PPA_F_STATIC_ENTRY) && (ifinfo->flags & (NETIF_VXLAN_TUNNEL | NETIF_GRE_TUNNEL)))
	    || ((ifinfo->flags & NETIF_DIRECTCONNECT_WIFI) && (ifinfo->flags & NETIF_VLAN))) {
		ppa_netif_put(ifinfo);
		return ret;
	}

	ppa_br_get_htable_lock();
	if ((__ppa_bridging_lookup_mac(mac_addr,
				fid,
				netif,
				&p_item) == PPA_SESSION_EXISTS) && (p_item != NULL)) {

		/* Bridge mac entry learned through new interface? */
		if ((p_item->netif != netif) && !(flags & PPA_F_STATIC_ENTRY)) {
			p_item->netif = netif;
			br_item_updated = 1;
		}

		if (ifinfo->flags & NETIF_PHYS_PORT_GOT) {
			if (ppa_bridging_update_mac(p_item, ifinfo, netif,
			    mac_addr, flags))
				goto __BR_SESSION_ADD_DONE;
		}

		if((p_item->flags & SESSION_ADDED_IN_HW)) {
		/*added in hardware/firmware*/
#if IS_ENABLED(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500
			if(flags & PPA_F_STATIC_ENTRY) {
				p_item->ref_count++;
#if !defined(CONFIG_MAC_VIOLATION_MIRROR)
				if(!(p_item->flags & SESSION_STATIC)) {
					p_item->flags |= SESSION_STATIC;
					p_item->timeout	 = ~0; /* max timeout*/
					goto __UPDATE_HW_SESSION;
				}
#else
				if (!(p_item->flags & SESSION_STATIC)) {
					cur_time = ppa_get_time_in_sec();
					if (cur_time - p_item->last_hit_time > 10) { /*1 hw update per 10 sec */
						p_item->last_hit_time = cur_time;
						goto __UPDATE_HW_SESSION;
					}
				}
#endif
			} else if(p_item->flags & SESSION_LAN_ENTRY) { /* dynamic bridge entry getting updated*/
				cur_time = ppa_get_time_in_sec();
				if(br_item_updated || (cur_time - p_item->last_hit_time > 10)) { /*1 hw update per 10 sec*/
					p_item->last_hit_time = cur_time;
					goto __UPDATE_HW_SESSION;
				}
			} else {
				p_item->ref_count++;
				p_item->flags |= SESSION_LAN_ENTRY; /* dynamic entry learned by bridge learning*/
			}
#endif
			ret = PPA_SESSION_ADDED;
			goto __BR_SESSION_ADD_DONE;
		}
	} else if (ppa_bridging_add_mac(mac_addr, fid, netif, &p_item, flags) != 0)
		goto __BR_SESSION_ADD_DONE;

#if IS_ENABLED(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500
__UPDATE_HW_SESSION:
#endif
	if (ifinfo->flags & NETIF_VETH)
		goto __BR_SESSION_ADD_DONE;

	if (ppa_bridging_hw_add_mac(p_item) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_bridging_hw_add_mac(%02x:%02x:%02x:%02x:%02x:%02x): fail\n",
			(uint32_t)p_item->mac[0], (uint32_t)p_item->mac[1], (uint32_t)p_item->mac[2],
			(uint32_t)p_item->mac[3], (uint32_t)p_item->mac[4], (uint32_t)p_item->mac[5]);
		goto __BR_SESSION_ADD_DONE;
	}
	ret =	PPA_SESSION_ADDED;

	/* Need to delete, if any older routing session */
	if (br_item_updated)
		is_del_routing_session = 1;

	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"hardware added\n");
__BR_SESSION_ADD_DONE:
	ppa_netif_put(ifinfo);
	ppa_br_release_htable_lock();

	if (is_del_routing_session)
		ppa_session_delete_by_macaddr(p_item->mac);

	return ret;
}

int32_t ppa_bridge_entry_delete_all(uint32_t f_enable)
{
	int32_t ret = PPA_FAILURE;

	if(f_enable) return PPA_SUCCESS;

	ppa_br_get_htable_lock();
	ret = ppa_bridging_flush_macs();
	ppa_br_release_htable_lock();

	return ret;
}

int32_t ppa_bridge_entry_delete(uint8_t *mac_addr, PPA_NETIF *brif, uint32_t flags)
{
	struct br_mac_node *p_item;
	int32_t ret = PPA_FAILURE;
	uint16_t fid=0;
	uint8_t is_del_routing_session = 0;

	if(!g_bridging_mac_learning) return PPA_FAILURE;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	if(ppa_get_fid(ppa_get_netif_name(brif), &fid)!=PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in getting fid of bridge %s\n", ppa_get_netif_name(brif));
	}
#endif
	ppa_br_get_htable_lock();
	if (__ppa_bridging_lookup_mac(mac_addr, fid, NULL, &p_item) != PPA_SESSION_EXISTS){
		goto __BR_SESSION_DELETE_DONE;
	}
/*	dump_bridging_list_item(p_item, "ppa_bridge_entry_delete");*/

	if(!(p_item->flags & SESSION_STATIC)) { /*bridge learned entry only delete*/
		if (!(flags & PPA_F_STATIC_ENTRY)) /*For bridged Sessions */
			is_del_routing_session = 1;

		/* Internally, ppa_bridging_remove_mac() will delete MAC entry from Firmware/Hardware*/
		ppa_bridging_remove_mac(p_item);

#if IS_ENABLED(CONFIG_SOC_GRX500)
	} else {
		if(p_item->ref_count==1) {
			ppa_bridging_remove_mac(p_item);
		} else {
			p_item->ref_count--;
			if(flags & PPA_F_STATIC_ENTRY) { /* routed mac learned entry delete*/
				if(p_item->ref_count==1 && (p_item->flags & SESSION_LAN_ENTRY)) {
					p_item->flags &= ~SESSION_STATIC;
					p_item->timeout = DEFAULT_BRIDGING_TIMEOUT_IN_SEC;
					ppa_bridging_hw_add_mac(p_item); /* modify the session to dynamic*/
				}
			} else {
				p_item->flags &= ~SESSION_LAN_ENTRY;
			}
		}
#endif
	}
	ret = PPA_SUCCESS;

__BR_SESSION_DELETE_DONE:
	ppa_br_release_htable_lock();

	if (is_del_routing_session)
		ppa_session_delete_by_macaddr(p_item->mac);

	return ret;
}

#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING)
int32_t ppa_hook_bridge_enable(uint32_t f_enable, uint32_t flags)
{
	g_bridging_mac_learning = f_enable;
	ppa_bridge_entry_delete_all(f_enable);
	return PPA_SUCCESS;
}

int32_t ppa_hook_get_bridge_status(uint32_t *f_enable, uint32_t flags)
{
	if(f_enable)
		*f_enable = g_bridging_mac_learning;
	return PPA_SUCCESS;
}

int32_t ppa_bridge_entry_hit_time(uint8_t *mac_addr, PPA_NETIF *brif, uint32_t *p_hit_time)
{
	struct br_mac_node *p_item;
	int32_t ret = PPA_SESSION_NOT_ADDED;
	uint16_t fid=0;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	PPA_BR_MAC_INFO br_mac={0};
#endif

	if(!g_bridging_mac_learning) return PPA_SUCCESS;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	if(ppa_get_fid(ppa_get_netif_name(brif), &fid)!=PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in getting fid of bridge %s\n", ppa_get_netif_name(brif));
	}
#endif

	ppa_br_get_htable_lock();
	if (__ppa_bridging_lookup_mac(mac_addr, fid, NULL, &p_item) == PPA_SESSION_EXISTS) {

#if IS_ENABLED(CONFIG_SOC_GRX500)
		/*	If the hardware has the MAC entry then the bridge entry should not timeout */
		/*	If the mac entry is aged out by the hardware then bridge entry can be removed.*/
		ppa_memcpy(br_mac.mac, p_item->mac, PPA_ETH_ALEN);
		br_mac.fid = p_item->fid;
		ppa_drv_test_and_clear_bridging_hit_stat(&br_mac, 0);
		if(br_mac.f_hit) {
			p_item->last_hit_time = ppa_get_time_in_sec() - (p_item->timeout - br_mac.age_timer);
			*p_hit_time = p_item->last_hit_time;
			ret = PPA_HIT;
		}
#else
		*p_hit_time = p_item->last_hit_time;
		ret = PPA_HIT;
#endif
	}

	ppa_br_release_htable_lock();
	return ret;
}

int32_t ppa_bridge_entry_inactivity_status(uint8_t *mac_addr, PPA_NETIF *brif)
{
	struct br_mac_node *p_item;
	int32_t ret = PPA_HIT;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	PPA_BR_MAC_INFO br_mac={0};
#endif
	uint16_t fid=0;

	if(!g_bridging_mac_learning) return PPA_SUCCESS;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	if(ppa_get_fid(ppa_get_netif_name(brif), &fid)!=PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in getting fid of bridge %s\n", ppa_get_netif_name(brif));
	}
#endif
	ppa_br_get_htable_lock();
	if (__ppa_bridging_lookup_mac(mac_addr, fid, NULL, &p_item) != PPA_SESSION_EXISTS){
		ret = PPA_SESSION_NOT_ADDED;
		goto __BR_INACTIVITY_DONE;
	}

	/* not added in hardware*/
	if (!(p_item->flags & SESSION_ADDED_IN_HW)) {
		ret = PPA_SESSION_NOT_ADDED;
		goto __BR_INACTIVITY_DONE;
	}

	if ((p_item->flags & SESSION_STATIC)){
		ret = PPA_HIT;
		goto __BR_INACTIVITY_DONE;
	}
#if IS_ENABLED(CONFIG_SOC_GRX500)
	/*	If the hardware has the MAC entry then the bridge entry should not timeout */
	/*	If the mac entry is aged out by the hardware then bridge entry can be removed.*/
	ppa_memcpy(br_mac.mac, p_item->mac, PPA_ETH_ALEN);
	br_mac.fid = p_item->fid;
	ppa_drv_test_and_clear_bridging_hit_stat(&br_mac, 0);
	ret = br_mac.f_hit ? PPA_HIT : PPA_TIMEOUT;
#else
	if (p_item->timeout < ppa_get_time_in_sec() - p_item->last_hit_time) {
		/* use < other than <= to avoid "false positives"*/
		ret = PPA_TIMEOUT;
	}
#endif

__BR_INACTIVITY_DONE:
	ppa_br_release_htable_lock();
	return ret;
}

int32_t ppa_set_bridge_entry_timeout(uint8_t *mac_addr, PPA_NETIF *brif, uint32_t timeout)
{
	struct br_mac_node *p_item;
	int32_t ret = PPA_SUCCESS;
	uint16_t fid=0;

#if IS_ENABLED(CONFIG_SOC_GRX500)
	if(ppa_get_fid(ppa_get_netif_name(brif), &fid)!=PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in getting fid of bridge %s\n", ppa_get_netif_name(brif));
	}
#endif
	ppa_br_get_htable_lock();
	if (__ppa_bridging_lookup_mac(mac_addr, fid,NULL, &p_item) != PPA_SESSION_EXISTS){
		ret = PPA_FAILURE;
		goto __BR_TIMEOUT_DONE;
	}

	ppa_br_release_htable_lock();
	if (!(p_item->flags & SESSION_STATIC))
		p_item->timeout = timeout;

	ppa_bridging_set_polling_timer(timeout);

	return PPA_SUCCESS;

__BR_TIMEOUT_DONE:
	ppa_br_release_htable_lock();

	return PPA_SUCCESS;
}
#endif /*CONFIG_PPA_BR_MAC_LEARNING*/
/*
 *	PPA MIB Counters Operation Functions
 */
int32_t ppa_get_if_stats(PPA_IFNAME *ifname, PPA_IF_STATS *p_stats, uint32_t flags)
{
	struct netif_info *p_info;
	uint32_t port_flags;
	PPE_ITF_MIB_INFO itf_mib={0};

	if (!ifname || !p_stats)
		return PPA_EINVAL;

	if (ppa_netif_lookup(ifname, &p_info) != PPA_SUCCESS)
		return PPA_EIO;
	itf_mib.itf= p_info->phys_port;
	port_flags = p_info->flags;
	ppa_netif_put(p_info);

	if (!(port_flags & NETIF_PHYS_PORT_GOT))
		return PPA_EIO;

	ppa_drv_get_itf_mib(&itf_mib, 0);

	p_stats->rx_pkts		= itf_mib.mib.ig_cpu_pkts;
	p_stats->tx_discard_pkts	= itf_mib.mib.ig_drop_pkts;
	p_stats->rx_bytes		= itf_mib.mib.ig_cpu_bytes;

	return PPA_SUCCESS;
}

int32_t ppa_get_accel_stats(PPA_IFNAME *ifname, PPA_ACCEL_STATS *p_stats, uint32_t flags)
{
	struct netif_info *p_info;
	uint32_t port;
	uint32_t port_flags;
	PPE_ITF_MIB_INFO mib = {0};

	if (!ifname || !p_stats)
		return PPA_EINVAL;

	if (ppa_netif_lookup(ifname, &p_info) != PPA_SUCCESS)
		return PPA_EIO;
	port = p_info->phys_port;
	port_flags = p_info->flags;
	ppa_netif_put(p_info);

	if (!(port_flags & NETIF_PHYS_PORT_GOT))
		return PPA_EIO;

	mib.itf = p_info->phys_port;
	mib.flag = flags;
	ppa_drv_get_itf_mib(&mib, 0);

	p_stats->fast_routed_tcp_pkts		= mib.mib.ig_fast_rt_ipv4_tcp_pkts + mib.mib.ig_fast_rt_ipv6_tcp_pkts;
	p_stats->fast_routed_udp_pkts		= mib.mib.ig_fast_rt_ipv4_udp_pkts + mib.mib.ig_fast_rt_ipv6_udp_pkts;
	p_stats->fast_routed_udp_mcast_pkts	= mib.mib.ig_fast_rt_ipv4_mc_pkts;
	p_stats->fast_drop_pkts			= mib.mib.ig_drop_pkts;
	p_stats->fast_drop_bytes		= mib.mib.ig_drop_bytes;
	p_stats->fast_ingress_cpu_pkts		= mib.mib.ig_cpu_pkts;
	p_stats->fast_ingress_cpu_bytes	 	= mib.mib.ig_cpu_bytes;
	p_stats->fast_bridged_ucast_pkts	= mib.mib.ig_fast_brg_pkts;
	p_stats->fast_bridged_bytes		= mib.mib.ig_fast_brg_bytes;

	return PPA_SUCCESS;
}

static void ppa_add_accel_stats(PPA_NETIF_ACCEL_STATS *p_stats, struct netif_info *p_info)
{
	p_stats->hw_accel_stats.rx_bytes += p_info->hw_accel_stats.rx_bytes;
	p_stats->hw_accel_stats.tx_bytes += p_info->hw_accel_stats.tx_bytes;
	p_stats->hw_accel_stats.rx_pkts += p_info->hw_accel_stats.rx_pkts;
	p_stats->hw_accel_stats.tx_pkts += p_info->hw_accel_stats.tx_pkts;

	p_stats->sw_accel_stats.rx_bytes += p_info->sw_accel_stats.rx_bytes;
	p_stats->sw_accel_stats.tx_bytes += p_info->sw_accel_stats.tx_bytes;
	p_stats->sw_accel_stats.rx_pkts += p_info->sw_accel_stats.rx_pkts;
	p_stats->sw_accel_stats.tx_pkts += p_info->sw_accel_stats.tx_pkts;

#if IS_ENABLED(CONFIG_IPV4_IPV6_COUNTER_SUPPORT)
	p_stats->hw_accel_stats.rx_pkts_ipv4 += p_info->hw_accel_stats.rx_pkts_ipv4;
	p_stats->hw_accel_stats.tx_pkts_ipv4 += p_info->hw_accel_stats.tx_pkts_ipv4;
	p_stats->hw_accel_stats.rx_pkts_ipv6 += p_info->hw_accel_stats.rx_pkts_ipv6;
	p_stats->hw_accel_stats.tx_pkts_ipv6 += p_info->hw_accel_stats.tx_pkts_ipv6;

	p_stats->sw_accel_stats.rx_pkts_ipv4 += p_info->sw_accel_stats.rx_pkts_ipv4;
	p_stats->sw_accel_stats.tx_pkts_ipv4 += p_info->sw_accel_stats.tx_pkts_ipv4;
	p_stats->sw_accel_stats.rx_pkts_ipv6 += p_info->sw_accel_stats.rx_pkts_ipv6;
	p_stats->sw_accel_stats.tx_pkts_ipv6 += p_info->sw_accel_stats.tx_pkts_ipv6;
#endif /* CONFIG_IPV4_IPV6_COUNTER_SUPPORT */
}

static int32_t ppa_add_upperif_accel_stats(PPA_NETIF_ACCEL_STATS *p_stats, struct netif_info *p_info)
{
	struct netif_info *upif = NULL;
	int32_t i = 0;

	if (p_info->sub_if_index == 0)
		return PPA_FAILURE;
	else {
		for (i = 0; i < p_info->sub_if_index; i++) {
			if (ppa_netif_lookup(p_info->sub_if_name[i], &upif) == PPA_SUCCESS) {
				ppa_add_accel_stats(p_stats, upif);
				ppa_add_upperif_accel_stats(p_stats, upif);
				ppa_netif_put(upif);
			}
		}
	}
	return PPA_SUCCESS;
}

#if defined(PPA_IF_MIB) && PPA_IF_MIB
int32_t ppa_get_netif_accel_stats(PPA_IFNAME *ifname, PPA_NETIF_ACCEL_STATS *p_stats, uint32_t flags, struct rtnl_link_stats64 *storage)
{
	struct netif_info *p_info;
	
	if (!ifname || !p_stats)
		return PPA_EINVAL;

	if (ppa_netif_lookup(ifname, &p_info) != PPA_SUCCESS)
		return PPA_EIO;
	
	if (p_info->netif == NULL) {
		return PPA_EINVAL;
	}
#ifdef PPA_IF_STAT_HELPER
	if(p_info->netdev_ops && p_info->netdev_ops->ndo_get_stats64) {
		memset(storage, 0, sizeof(*storage));
		p_info->netdev_ops->ndo_get_stats64(p_info->netif,storage);
	}
	storage->rx_dropped += (unsigned long)atomic_long_read(&p_info->netif->rx_dropped);
	storage->tx_dropped += (unsigned long)atomic_long_read(&p_info->netif->tx_dropped);
	storage->rx_nohandler += (unsigned long)atomic_long_read(&p_info->netif->rx_nohandler);
#endif

	ppa_add_accel_stats(p_stats, p_info);
	ppa_add_upperif_accel_stats(p_stats, p_info);

	ppa_netif_put(p_info);

	return PPA_SUCCESS;
}
#endif /* PPA_IF_MIB */

/*
 *	PPA Network Interface Operation Functions
 */
int32_t ppa_set_if_mac_address(PPA_IFNAME *ifname, uint8_t *mac_addr, uint32_t flags)
{
	struct netif_info *ifinfo;

	if (!ifname || !mac_addr)
		return PPA_EINVAL;

	if (ppa_netif_lookup(ifname, &ifinfo) != PPA_SUCCESS)
		return PPA_FAILURE;

	if ((ifinfo->flags & NETIF_MAC_ENTRY_CREATED)) {
		ifinfo->mac_entry = ~0;
		ifinfo->flags &= ~NETIF_MAC_ENTRY_CREATED;
	}

	ppa_memcpy(ifinfo->mac, mac_addr, PPA_ETH_ALEN);
	ifinfo->flags |= NETIF_MAC_AVAILABLE;

	ppa_netif_put(ifinfo);

	return PPA_SUCCESS;
}

int32_t ppa_get_if_mac_address(PPA_IFNAME *ifname, uint8_t *mac_addr, uint32_t flags)
{
	int32_t ret;
	struct netif_info *ifinfo;

	if (!ifname || !mac_addr)
		return PPA_EINVAL;

	if (ppa_netif_lookup(ifname, &ifinfo) != PPA_SUCCESS)
		return PPA_FAILURE;

	if ((ifinfo->flags & NETIF_MAC_AVAILABLE)) {
		ppa_memcpy(mac_addr,ifinfo->mac, PPA_ETH_ALEN);
		ret = PPA_SUCCESS;
	} else
		ret = PPA_FAILURE;

	ppa_netif_put(ifinfo);

	return ret;
}

#if IS_ENABLED(CONFIG_MCAST_HELPER)
int32_t ppa_ip_compare(IP_ADDR ip1, IP_ADDR ip2, uint32_t flag)
{
#if IS_ENABLED(CONFIG_IPV6)
	if(flag & SESSION_IS_IPV6) {
		return ppa_memcmp(ip1.ip6, ip2.ip6, sizeof(ip1.ip6));
	} else
#endif
	{
		return ppa_memcmp(&ip1.ip, &ip2.ip, sizeof(ip1.ip));
	}
}

/* Multicast callback function*/
int32_t mcast_module_config(uint32_t grp_idx, struct net_device *member, void *mcs, uint32_t flags)
{
	/* call ppacmd addmc group */
	/* grp_idx for future purpose */
	PPA_MC_GROUP mc_group = {0};
	struct mc_session_node *pp_item = NULL;
	uint32_t pos = 0, itf_num = 0;
	int32_t count = 0, i, mc_group_count = 0, flag = 0, res = PPA_FAILURE;
	uint8_t mcast_match = 0, add_done = 0;
	char *mem_name = NULL;

	mcast_stream_t *mc_stream = (mcast_stream_t *)mcs;
	ppa_memset(&mc_group, 0, sizeof(mc_group));

	/* FILL ALL GIVEN INFO */
	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Received Mcast ADD/DEL Trigger \n");
	if(member != NULL) {
		mem_name = ppa_get_netif_name(member);
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Member name is %s \n",mem_name);
	} else {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Member is NULL \n");
		goto EXIT_EOI;
	}
	if(mc_stream == NULL) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"mc_stream is NULL \n");
		goto EXIT_EOI;
	}

	/* Get total number of Mcast Entries */
	mc_group_count = ppa_get_mc_group_count(flag);
	/* Set SSM flag for Source specific forwarding:
	If not set source ip will be ignored while DEL MC operation*/
	mc_group.SSM_flag = 1;

	/* Get Mcast Group Idx */
	mc_group.group_id = grp_idx;
	ppa_memcpy(mc_group.src_mac, mc_stream->src_mac, PPA_ETH_ALEN);

	/* Get Mcast Group IP */
	if(mc_stream->dst_ip.ipType == 0) {
		mc_group.ip_mc_group.ip.ip = mc_stream->dst_ip.ipA.ipAddr.s_addr;
		mc_group.ip_mc_group.f_ipv6 = 0;
	} else {
		ppa_memcpy(&(mc_group.ip_mc_group.ip.ip6[0]),
			&(mc_stream->dst_ip.ipA.ip6Addr.s6_addr[0]), sizeof(struct in6_addr)) ;
		mc_group.ip_mc_group.f_ipv6 = 1;
	}
	/* Fill rx device name */
	mc_group.src_ifname = mc_stream->rx_dev->name;

	/* Get Mcast Source IP */
	if(mc_stream->src_ip.ipType == 0) {
		mc_group.source_ip.ip.ip = mc_stream->src_ip.ipA.ipAddr.s_addr;
		mc_group.source_ip.f_ipv6 = 0;
	} else {
		ppa_memcpy(&(mc_group.source_ip.ip.ip6[0]),
			&(mc_stream->src_ip.ipA.ip6Addr.s6_addr[0]), sizeof(struct in6_addr));
		mc_group.source_ip.f_ipv6 = 1;
	}

	/* If the member added is bridged, set bridging flag */
	if (ppa_if_is_br_if(mc_stream->rx_dev, NULL)) {
		mc_group.bridging_flag = 1;
	}
	/* DONE FILLING ALL GIVEN INFO */

	if (ppa_mc_group_start_iteration(&pos, &pp_item) == PPA_SUCCESS) {

		count = 0;
		do {
			/* Check if received gaddr & source ip matches the list, and extract the ifname in that 
			gaddr and fill the mc_group data structure*/
			if (((pp_item->grp.ip_mc_group.ip.ip == mc_group.ip_mc_group.ip.ip) &&
					(pp_item->grp.source_ip.ip.ip == mc_group.source_ip.ip.ip) && (!mc_group.ip_mc_group.f_ipv6)) ||
					((!ppa_ip_compare((pp_item->grp.ip_mc_group.ip),(mc_group.ip_mc_group.ip),
					SESSION_IS_IPV6)) && (!ppa_ip_compare((pp_item->grp.source_ip.ip),
					(mc_group.source_ip.ip), SESSION_IS_IPV6)))
					/*compare the source netdevice
					&& (ppa_is_netif_name( mc_group.src_ifname, pp_item->grp.src_netif->name))*/
				) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Grp Ip & Src Ip Match found \n");
				/* itf_num is Number of interface signed in for the matched Group address */
				mcast_match = 1;

				itf_num = pp_item->grp.num_ifs;
				/* for each netdevice in the multicast group node*/
				for (i = 0; i < PPA_MAX_MC_IFS_NUM; i++) {
					/*Initializig mc_group->array_mem_ifs to all string as we need to iterate over all 16 nodes */
					mc_group.array_mem_ifs[i].ifname = "NULL";
					if (pp_item->grp.txif[i].netif) {
						/* if the index is valid compare the netdevice*/
						if (ppa_is_netif_name(pp_item->grp.txif[i].netif, member->name)) {
							/*netdevice matched*/
							if (flags & MC_F_DEL) {
								/* store the index to be deleted and flag*/
								mc_group.cop.index = i;
								mc_group.cop.flag = PPA_IF_DEL;
								itf_num--;
							} else if (flags & MC_F_ADD) {
								/* entry already exist */
								ppa_mc_group_stop_iteration();
								ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "Member %s already exists\n", member->name);
								goto EXIT_EOI;
							}
						} else {
							/*netdevice doesnot match so copy*/
							mc_group.array_mem_ifs[i].ifname = ppa_get_netif_name(pp_item->grp.txif[i].netif);
							mc_group.if_mask |= 1 << i;
						}
					} else {
						/* free index found index add in this location*/
						if ((flags & MC_F_ADD) && !add_done) {
							/* add the new member in this free index */
							mc_group.array_mem_ifs[i].ifname = member->name;
							mc_group.cop.index = i;
							mc_group.cop.flag = PPA_IF_ADD;
							mc_group.if_mask |= 1 << i;
							add_done = 1;
							itf_num++;
						}
					}
				}
				break;
			}
			count++;
		} while ((ppa_mc_group_iterate_next(&pos, &pp_item) == PPA_SUCCESS) && (count < mc_group_count));
	}
	ppa_mc_group_stop_iteration();

	if (flags & MC_F_ADD) {
		if (mcast_match == 0) {
			/*First station in the group */
			/*Intitializing array_mem_ifs for the first time*/
			for (i = 1; i < PPA_MAX_MC_IFS_NUM; i++) {
				mc_group.array_mem_ifs[i].ifname = "NULL";
			}
			/*end*/

			mc_group.array_mem_ifs[0].ifname = member->name;
			mc_group.cop.index = 0;
			mc_group.cop.flag = PPA_IF_ADD;
			mc_group.if_mask |= 1;
			itf_num++;
		} else if (!add_done) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "Maximum station limit reached\n");
			goto EXIT_EOI;
		}
	}

	mc_group.num_ifs = itf_num; /* No of interfaces in the list */

	res = ppa_mc_group_update(&mc_group, flag);

	if (res != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_mc_group_update fail\n");
		goto EXIT_EOI;
	} else {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_mc_group_update Success\n");
		res = 0;
	}

EXIT_EOI:
	return res;

}
EXPORT_SYMBOL(mcast_module_config);
#endif

int32_t ppa_add_if(PPA_IFINFO *ifinfo, uint32_t flags)
{
	uint32_t ret;
	struct iface_list *local, *num;

	if (!ifinfo)
		return PPA_EINVAL;

	ppa_manual_if_lock_list();
	ppa_list_for_each_entry_safe(local, num, &manual_del_iface_list,
					 node_ptr) {
		if (ppa_str_cmp(local->name, ifinfo->ifname)) {
			ppa_list_del(&local->node_ptr);
			ppa_free(local);
		}
	}
	ppa_manual_if_unlock_list();

	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"ppa_add_if with force_wanitf_flag=%d in ppa_add_if\n",
			ifinfo->force_wanitf_flag);
	ret = ppa_netif_add(ifinfo->ifname, ifinfo->if_flags & PPA_F_LAN_IF,
				NULL, ifinfo->ifname_lower,
				ifinfo->force_wanitf_flag);
	return ret;
}

int32_t ppa_del_if(PPA_IFINFO *ifinfo, uint32_t flags)
{
	PPA_NETIF *netif;
#if defined(CONFIG_PPA_PUMA7) && defined(CONFIG_TI_HIL_PROFILE_INTRUSIVE_P7)
	PPA_NETIF *lnetif;
	struct netif_info *p_ifinfo;
	PPA_IFNAME underlying_ifname[PPA_IF_NAME_SIZE];
#endif
	struct iface_list *local, *num;
	uint16_t found = 0;

#if defined(CONFIG_PPA_PUMA7) && defined(CONFIG_TI_HIL_PROFILE_INTRUSIVE_P7)
	netif = ppa_get_netif(ifinfo->ifname);
	if (netif == NULL)
	{
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				"ppa_del_if, Invalid interface name\n");
		return PPA_EINVAL;
	}

	if(netif != NULL) {
		if (ppa_netif_lookup(ifinfo->ifname, &p_ifinfo) == PPA_SUCCESS) {
			/* for NETIF_PPPOE, delete VPID of actual interafce*/
			if (p_ifinfo->flags & NETIF_PPPOE &&
				(ppa_pppoe_get_physical_if(netif, NULL,
								 underlying_ifname
							 ) == PPA_SUCCESS)) {
				lnetif = ppa_get_netif(underlying_ifname);
				if (lnetif != NULL) {
					if (lnetif->qos_shutdown_hook != NULL)
						lnetif->qos_shutdown_hook(lnetif);
					ti_hil_pp_event(TI_PP_REMOVE_VPID,
							lnetif);
				}
			} else {
				if (netif->qos_shutdown_hook != NULL)
					netif->qos_shutdown_hook(netif);
				ti_hil_pp_event(TI_PP_REMOVE_VPID, netif);
			}
		}
	}
	/* If hw_disable flag is set then delete the ifname only from HW, SW
	 * acceleration for ifname will be still functional
	 */
	if (ifinfo->hw_disable) {
		printk("ifinfo.hw_disable is set\n");
		return PPA_SUCCESS;
	}
#endif

	ppa_manual_if_lock_list();
	ppa_list_for_each_entry_safe(local, num, &manual_del_iface_list,
					 node_ptr) {
		if (ppa_str_cmp(local->name, ifinfo->ifname)) {
			found = 1;
			break;
		}
	}

	if (!found) {
		local = (struct iface_list *)ppa_malloc(sizeof(struct iface_list));
		if (local != NULL) {
			ppa_strncpy(local->name, ifinfo->ifname,
					PPA_IF_NAME_SIZE);
			ppa_list_add_head(&local->node_ptr,
						&manual_del_iface_list);
		}
	}
	ppa_manual_if_unlock_list();

	ppa_netif_remove(ifinfo->ifname, ifinfo->if_flags & PPA_F_LAN_IF);

	netif = ppa_dev_get_by_name(ifinfo->ifname);
	if (netif) {
		ppa_remove_sessions_on_netif(netif);
		ppa_put_netif(netif);
	}

	return PPA_SUCCESS;
}

int32_t ppa_get_if(int32_t *num_ifs, PPA_IFINFO **ifinfo, uint32_t flags)
{
	uint32_t pos = 0;
	struct netif_info *info;
	int32_t num = 0;
	PPA_IFINFO *p_ifinfo;

	if (!num_ifs || !ifinfo)
		return PPA_EINVAL;

	p_ifinfo = (PPA_IFINFO *)ppa_malloc(100 * sizeof(PPA_IFINFO));	/* assume max 100 netif*/
	if(!p_ifinfo) {
		return PPA_ENOMEM;
	}

	if (ppa_netif_start_iteration(&pos, &info) != PPA_SUCCESS){
		ppa_free(p_ifinfo);
		return PPA_FAILURE;
	}

	do {
		if ((info->flags & NETIF_LAN_IF)) {
			p_ifinfo[num].ifname = info->name;
			p_ifinfo[num].if_flags = PPA_F_LAN_IF;
			num++;
		}
		if ((info->flags & NETIF_WAN_IF)) {
			p_ifinfo[num].ifname = info->name;
			p_ifinfo[num].if_flags = 0;
			num++;
		}
	} while (ppa_netif_iterate_next(&pos, &info) == PPA_SUCCESS);

	ppa_netif_stop_iteration();
	*num_ifs = num;
	*ifinfo = p_ifinfo;

	return PPA_SUCCESS;
}

#if IS_ENABLED(CONFIG_PPA_API_DIRECTCONNECT)
int32_t ppa_disconn_if(PPA_NETIF *netif, PPA_DP_SUBIF *subif, uint8_t *mac, uint32_t flags)
{
	if (!netif && !subif && !mac)
		return PPA_FAILURE;

	if (mac) {
		ppa_remove_sessions_on_macaddr(mac);
	} else if (netif) {
		ppa_remove_sessions_on_netif(netif);
	} else if (subif) {
		ppa_remove_sessions_on_subif(subif);
	}

	return PPA_SUCCESS;
}
#endif

int32_t ppa_multicast_pkt_srcif_add(PPA_BUF *pkt_buf, PPA_NETIF *rx_if)
{
	IP_ADDR_C ip={0};
	IP_ADDR_C src_ip={0};
	struct mc_session_node *p_item;
	struct netif_info *p_netif_info;
	int32_t res = PPA_SESSION_NOT_ADDED;
	int32_t ret;

	if(!rx_if) {
		rx_if = ppa_get_pkt_src_if(pkt_buf);
	}

	if(ppa_get_multicast_pkt_ip(pkt_buf, &ip, &src_ip) != PPA_SUCCESS){
		return res;
	}

	if(is_ip_zero(&ip)) {
		return res;
	}

	if (!g_ppe_fastpath_enabled)
		return res;

	ppa_mc_get_htable_lock();
	ret = __ppa_lookup_mc_group(&ip, &src_ip, &p_item);

	if (ret == PPA_MC_SESSION_VIOLATION){
	/*if violation, there is a item with src ip all zero, so search again with src ip zero*/
		ppa_memset(&src_ip, 0, sizeof(src_ip));
		ret = __ppa_lookup_mc_group(&ip, &src_ip, &p_item);
	}

	if (ret == PPA_SESSION_EXISTS) {
		if(p_item->grp.src_netif && p_item->grp.src_netif != rx_if)
		{ /*at present, we don't allowed to change multicast src_if */
			ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP,
				"Not matched src if: original srcif is %s, but new srcif is %s: %d.%d.%d.%d\n",
				ppa_get_netif_name(p_item->grp.src_netif), ppa_get_netif_name(rx_if),
				ip.ip.ip >> 24, (ip.ip.ip >> 16) & 0xFF, (ip.ip.ip >> 8) & 0xFF, ip.ip.ip & 0xFF);
			goto ENTRY_ADD_EXIT;
		}
		if(p_item->flags & SESSION_ADDED_IN_HW) { /*already added into HW. no change here*/
			res = PPA_SESSION_ADDED;
			goto ENTRY_ADD_EXIT;
		}
               if(p_item->flags & SESSION_CAN_NOT_ACCEL) {
                       ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_hw_add_mc_group(%d.%d.%d.%d): fail ???\n",
                               ip.ip.ip >> 24, (ip.ip.ip >> 16) & 0xFF, (ip.ip.ip >> 8) & 0xFF, ip.ip.ip& 0xFF);
                       goto ENTRY_ADD_EXIT;
               }

		if (ppa_is_netif_bridged(rx_if))
			p_item->bridging_flag =1; /*If the receive interface is in bridge, then regard it as bridge mode*/
		else
			p_item->bridging_flag =0;

		/* add to HW if possible*/
		if (ppa_netif_lookup(ppa_get_netif_name(rx_if), &p_netif_info) != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP,
				"ppa_multicast_pkt_srcif_add cannot get interface %s for multicast session info: %d.%d.%d.%d\n",
					ppa_get_netif_name(rx_if), ip.ip.ip >> 24, (ip.ip.ip >> 16) & 0xFF,
					(ip.ip.ip >> 8) & 0xFF, ip.ip.ip & 0xFF) ;
			res = PPA_SESSION_NOT_ADDED;
			goto ENTRY_ADD_EXIT;
		}
		ppa_netif_put(p_netif_info);

		if (p_netif_info->flags & NETIF_PHYS_PORT_GOT) {
			/* PPPoE and source mac*/
			if (!p_item->bridging_flag) {
				if(p_netif_info->flags & NETIF_PPPOE)
					p_item->flags |= SESSION_VALID_PPPOE;
			}

			/* VLAN*/
			if(p_netif_info->flags & NETIF_VLAN_CANT_SUPPORT)
				ppa_debug(DBG_ENABLE_MASK_ASSERT,"MC processing can support two layers of VLAN only\n");

			if ((p_netif_info->flags & NETIF_VLAN_OUTER))
				p_item->flags |= SESSION_VALID_OUT_VLAN_RM;
			if ((p_netif_info->flags & NETIF_VLAN_INNER))
				p_item->flags |= SESSION_VALID_VLAN_RM;

			p_item->grp.src_netif = p_netif_info->netif;

			if (ppa_hsel_add_wan_mc_group(p_item, NULL) != PPA_SUCCESS) {
				p_item->num_adds ++;
				if (p_item->num_adds > g_ppa_min_hits)
					p_item->flags |= SESSION_CAN_NOT_ACCEL;
				ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_hw_add_mc_group(%d.%d.%d.%d): fail ???\n",
					ip.ip.ip >> 24, (ip.ip.ip >> 16) & 0xFF, (ip.ip.ip >> 8) & 0xFF, ip.ip.ip& 0xFF);
			} else {
				res = PPA_SESSION_ADDED;
				ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_hw_add_mc_group(%d.%d.%d.%d): sucessfully\n",
					ip.ip.ip >> 24, (ip.ip.ip >> 16) & 0xFF, (ip.ip.ip >> 8) & 0xFF, ip.ip.ip & 0xFF);
				ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "%s: update src interface:(%s)\n",
					__FUNCTION__, p_netif_info->netif->name);
			}
		}
	}

ENTRY_ADD_EXIT:
	ppa_mc_release_htable_lock();

	return res;
}

int32_t ppa_get_max_entries(PPA_MAX_ENTRY_INFO *max_entry, uint32_t flags)
{
	if(!max_entry) return PPA_FAILURE;

	ppa_drv_get_max_entries(max_entry, 0);

	return PPA_SUCCESS;
}

int32_t ppa_ip_sprintf(char *buf, PPA_IPADDR ip, uint32_t flag)
{
	int32_t len=0;
	if(buf)	{
#if IS_ENABLED(CONFIG_IPV6)
		if(flag & SESSION_IS_IPV6) {
			len = ppa_sprintf(buf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x", NIP6(ip.ip6));
		} else
#endif
			len = ppa_sprintf(buf, "%u.%u.%u.%u", NIPQUAD(ip.ip));

	}
	return len;
}

int32_t ppa_ip_comare(PPA_IPADDR ip1, PPA_IPADDR ip2, uint32_t flag)
{
#if IS_ENABLED(CONFIG_IPV6)
	if(flag & SESSION_IS_IPV6) {
		return ppa_memcmp(ip1.ip6, ip2.ip6, sizeof(ip1.ip6));
	} else
#endif
	{
		return ppa_memcmp(&ip1.ip, &ip2.ip, sizeof(ip1.ip));
	}
}

int32_t ppa_zero_ip(PPA_IPADDR ip)
{
	PPA_IPADDR zero_ip={0};

	return (ppa_ip_comare(ip, zero_ip, 0)==0) ? 1:0;
}

/* Statistics API */

int32_t ppa_get_ct_stats(PPA_SESSION *p_session, PPA_CT_COUNTER *pCtCounter)
{
	int32_t ret = PPA_SUCCESS;
	struct uc_session_node *p_item;

	if (__ppa_session_find_by_ct(p_session, 0, &p_item) == PPA_SESSION_EXISTS) {

		pCtCounter->lastHitTime = p_item->last_hit_time;
		if (p_item->flags & SESSION_LAN_ENTRY) {
			pCtCounter->txBytes = p_item->acc_bytes;
		} else if (p_item->flags & SESSION_WAN_ENTRY) {
			pCtCounter->rxBytes = p_item->acc_bytes;
		}
		else
			ret = PPA_FAILURE;
		ppa_atomic_dec(&p_item->used);
	}

	if (__ppa_session_find_by_ct(p_session, 1, &p_item) == PPA_SESSION_EXISTS) {

		pCtCounter->lastHitTime = p_item->last_hit_time;
		if (p_item->flags & SESSION_LAN_ENTRY) {
			pCtCounter->txBytes = p_item->acc_bytes;
		} else if (p_item->flags & SESSION_WAN_ENTRY) {
			pCtCounter->rxBytes = p_item->acc_bytes;
		}
		else
			ret = PPA_FAILURE;
		ppa_atomic_dec(&p_item->used);
	}

	return ret;
}

int32_t ppa_update_min_hit(uint32_t val)
{
	g_ppa_min_hits = val;
	return PPA_SUCCESS;
}

EXPORT_SYMBOL(ppa_init);
EXPORT_SYMBOL(ppa_exit);
EXPORT_SYMBOL(ppa_enable);
EXPORT_SYMBOL(ppa_add_if);
EXPORT_SYMBOL(ppa_del_if);
EXPORT_SYMBOL(ppa_get_max_entries);
EXPORT_SYMBOL(ppa_ip_sprintf);
EXPORT_SYMBOL(ppa_ip_comare);
EXPORT_SYMBOL(ppa_update_min_hit);
EXPORT_SYMBOL(g_ppe_fastpath_enabled);
EXPORT_SYMBOL(ppa_zero_ip);
