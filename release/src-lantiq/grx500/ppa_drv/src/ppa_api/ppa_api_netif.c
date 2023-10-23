/*******************************************************************************
 **
 ** FILE NAME	: ppa_api_netif.c
 ** PROJECT	  : PPA
 ** MODULES	  : PPA API (Routing/Bridging Acceleration APIs)
 **
 ** DATE		 : 3 NOV 2008
 ** AUTHOR	   : Xu Liang
 ** DESCRIPTION  : PPA Protocol Stack Hook API Network Interface Functions
 ** COPYRIGHT	: Copyright (c) 2020, MaxLinear, Inc.
 **               Copyright (c) 2009, Lantiq Deutschland GmbH
 **               Am Campeon 3; 85579 Neubiberg, Germany
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date		$Author		 $Comment
 ** 03 NOV 2008  Xu Liang		Initiate Version
 *******************************************************************************
 */

/* ####################################
 *			  Head File
 * ####################################
 */

/*  Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>

#ifdef CONFIG_LTQ_ETHSW_API
#include <net/switch_api/lantiq_gsw_api.h>
#endif

#if IS_ENABLED(CONFIG_PPA_EXT_PKT_LEARNING)
#include <linux/pktprs.h>
#endif /*CONFIG_PPA_EXT_PKT_LEARNING*/

/*  PPA Specific Head File
 */
#include <net/ppa/ppa_api.h>
#if defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH
#include <net/datapath_api.h>
#endif
#include <net/ppa/ppa_stack_al.h>
#include <net/ppa/ppa_drv_wrapper.h>
#include <net/ppa/ppa_hal_wrapper.h>
#include <linux/if_macvlan.h>

#include "ppa_api_netif.h"
#include "ppa_api_misc.h"
#include "ppa_api_session.h"
#include "ppa_api_tools.h"
#include "ppa_api_core.h"
#include "ppa_stack_tnl_al.h"


/* ####################################
 *			  Definition
 * ####################################
 */

#define DEFAULT_OUTER_VLAN_ID 0x81000000

/* ####################################
 *			  Data Type
 * ####################################
 */


/* ####################################
 *			 Declaration
 * ####################################
 */

/*  implemented in PPA PPE Low Level Driver (Data Path)
 */


/* ####################################
 *		   Global Variable
 * ####################################
 */

uint32_t g_phys_port_cpu = ~0;
EXPORT_SYMBOL(g_phys_port_cpu);
uint32_t g_phys_port_atm_wan = ~0;
EXPORT_SYMBOL(g_phys_port_atm_wan);
uint32_t g_phys_port_atm_wan_vlan;
EXPORT_SYMBOL(g_phys_port_atm_wan_vlan);

PPA_LIST_HEAD manual_del_iface_list;
EXPORT_SYMBOL(manual_del_iface_list);
static PPA_LOCK g_manual_if_lock;

static struct phys_port_info *g_phys_port_info;
static PPA_LOCK g_phys_port_lock;

static struct netif_info *g_netif;
static PPA_LOCK g_netif_lock;

/* ####################################
 *		   Extern Variable
 * ####################################
 */
#if IS_ENABLED(CONFIG_SOC_GRX500)
extern int32_t ppa_get_fid(PPA_IFNAME *ifname, uint16_t *fid);
extern int32_t ppa_drv_tmu_set_checksum_queue_map(uint32_t pmac_port);
extern int32_t ppa_drv_mpe_set_checksum_queue_map(uint32_t pmac_port, uint32_t flags);
extern int32_t ppa_drv_tmu_set_lro_queue_map(uint32_t pmac_port);
#ifdef PPA_IF_STAT_HELPER
extern struct rtnl_link_stats64* ppa_stat_logical_intf_get_extended_stats64(struct net_device *dev, struct rtnl_link_stats64 *storage);
#endif
#endif

/* ####################################
 *			Local Function
 * ####################################
 */

/*  Physical Network Interface Operation Function */
static struct phys_port_info *ppa_phys_port_alloc_item(void)
{
	struct phys_port_info *obj;

	obj = (struct phys_port_info *)ppa_malloc(sizeof(*obj));
	if (obj)
		ppa_memset(obj, 0, sizeof(*obj));
	return obj;
}

static void ppa_phys_port_free_item(struct phys_port_info *obj)
{
	ppa_free(obj);
}

static void ppa_phys_port_free_list(void)
{
	struct phys_port_info *obj;

	ppa_lock_get(&g_phys_port_lock);
	while (g_phys_port_info) {
		obj = g_phys_port_info;
		g_phys_port_info = g_phys_port_info->next;

		ppa_phys_port_free_item(obj);
	}
	g_phys_port_cpu = ~0;
	g_phys_port_atm_wan = ~0;
	ppa_lock_release(&g_phys_port_lock);
}

static int32_t ppa_phys_port_lookup(PPA_IFNAME ifname[PPA_IF_NAME_SIZE],
					struct phys_port_info **pp_info)
{
	int32_t ret = PPA_ENOTAVAIL;
	struct phys_port_info *obj;

	ppa_lock_get(&g_phys_port_lock);
	for (obj = g_phys_port_info; obj; obj = obj->next) {
		if (strncmp(obj->ifname, ifname, PPA_IF_NAME_SIZE) == 0) {
			ret = PPA_SUCCESS;
			if (pp_info)
				*pp_info = obj;
			break;
		}
	}
	ppa_lock_release(&g_phys_port_lock);

	return ret;
}

void ppa_manual_if_lock_list(void)
{
	ppa_lock_get(&g_manual_if_lock);
}
EXPORT_SYMBOL(ppa_manual_if_lock_list);

void ppa_manual_if_unlock_list(void)
{
	ppa_lock_release(&g_manual_if_lock);
}
EXPORT_SYMBOL(ppa_manual_if_unlock_list);

/*  Network Interface Operation Functions */
static struct netif_info *ppa_netif_alloc_item(void)
{
	struct netif_info *obj;

	obj = (struct netif_info *)ppa_malloc(sizeof(*obj));
	if (obj) {
		ppa_memset(obj, 0, sizeof(*obj));
		obj->mac_entry = ~0;
		ppa_atomic_set(&obj->count, 1);
	}

	return obj;
}

static void ppa_netif_free_item(struct netif_info *obj)
{
	if (ppa_atomic_dec(&obj->count) == 0) {

	/* resotre old wanitf if necessary */
		if (obj->f_wanitf.flag_already_wanitf) {
			PPA_WANITF_CFG wanitf_cfg = {0};
			if (obj->f_wanitf.old_lan_flag)
				wanitf_cfg.lan_flag = 1;
			else
				wanitf_cfg.lan_flag = 0;
			wanitf_cfg.physical_port = obj->phys_port;
			ppa_set_wan_itf(&wanitf_cfg, 0);
		}
		if ((obj->netdev_ops)){
			ppa_free(obj->netdev_ops);
			if (dp_set_net_dev_ops(obj->netif, NULL,
				offsetof(struct net_device_ops, ndo_get_stats64), DP_OPS_NETDEV | DP_OPS_RESET) != DP_SUCCESS) {
				ppa_debug(DBG_ENABLE_MASK_ERR,
					"Reset of netdev_ops failled for :[%s]\n", obj->name);
			}
		}
		ppa_free(obj);
	}
}

void ppa_netif_lock_list(void)
{
	ppa_lock_get(&g_netif_lock);
}
EXPORT_SYMBOL(ppa_netif_lock_list);

void ppa_netif_unlock_list(void)
{
	ppa_lock_release(&g_netif_lock);
}
EXPORT_SYMBOL(ppa_netif_unlock_list);

static void __ppa_netif_add_item(struct netif_info *obj)
{
	ppa_atomic_inc(&obj->count);
	ppa_netif_lock_list();
	obj->next = g_netif;
	g_netif = obj;
	ppa_netif_unlock_list();
}

static void ppa_netif_remove_item(PPA_IFNAME ifname[PPA_IF_NAME_SIZE],
				  struct netif_info **pp_info)
{
	struct netif_info *p_prev, *p_cur;

	if (pp_info)
		*pp_info = NULL;
	p_prev = NULL;
	ppa_netif_lock_list();
	for (p_cur = g_netif; p_cur; p_prev = p_cur, p_cur = p_cur->next)
		if (strncmp(p_cur->name, ifname, PPA_IF_NAME_SIZE) == 0) {
			if (!p_prev)
				g_netif = p_cur->next;
			else
				p_prev->next = p_cur->next;
			if (pp_info)
				*pp_info = p_cur;
			else
				ppa_netif_free_item(p_cur);
			break;
		}
	ppa_netif_unlock_list();
}

static void ppa_netif_free_list(void)
{
	struct netif_info *obj;

	ppa_netif_lock_list();
	while (g_netif) {
		obj = g_netif;
		g_netif = g_netif->next;

#if IS_ENABLED(CONFIG_MCAST_HELPER)
#ifndef CONFIG_PPA_PUMA7
		if ( (obj->flags & NETIF_LAN_IF) ) {
			mcast_helper_register_module(obj->netif,
							THIS_MODULE,
							NULL,
							mcast_module_config,
							NULL,
							MC_F_DEREGISTER | MC_F_DIRECTPATH);
		}
#endif
#endif
		ppa_netif_free_item(obj);
		obj = NULL;
	}
	ppa_netif_unlock_list();
}

/* ####################################
 *		   Global Function
 * ####################################
 */


int32_t ppa_phys_port_add(PPA_IFNAME *ifname, uint32_t port)
{
	struct phys_port_info *obj;
	uint32_t mode = 0;
	uint32_t type = 0;
	uint32_t vlan = 0;
	PPE_IFINFO if_info;
	uint32_t irq_flag = 0;

	if (!ifname)
		return PPA_EINVAL;

	ppa_memset(&if_info, 0, sizeof(if_info));
	if_info.port = port;

	ppa_drv_get_phys_port_info(&if_info, 0);
	if ((if_info.if_flags & PPA_PHYS_PORT_FLAGS_VALID)) {

		switch (if_info.if_flags & PPA_PHYS_PORT_FLAGS_MODE_MASK) {
		case PPA_PHYS_PORT_FLAGS_MODE_LAN:
			mode = 1;
			break;
		case PPA_PHYS_PORT_FLAGS_MODE_WAN:
			mode = 2;
			break;
		case PPA_PHYS_PORT_FLAGS_MODE_MIX:
			mode = 3;
		}

		switch (if_info.if_flags & PPA_PHYS_PORT_FLAGS_TYPE_MASK) {
		case PPA_PHYS_PORT_FLAGS_TYPE_CPU:
			type = 0;
			break;
		case PPA_PHYS_PORT_FLAGS_TYPE_ATM:
			type = 1;
			break;
		case PPA_PHYS_PORT_FLAGS_TYPE_ETH:
			type = 2;
			break;
		case PPA_PHYS_PORT_FLAGS_TYPE_EXT:
			type = 3;
		}
		vlan = (if_info.if_flags & PPA_PHYS_PORT_FLAGS_OUTER_VLAN) ? 2 : 1;
	}
	/*trick here with ppa_lock_get2 since it will be called in irqs_disabled
	 * mode by directpath wlan registering
	 */
	irq_flag = ppa_lock_get2(&g_phys_port_lock);
	for (obj = g_phys_port_info; obj; obj = obj->next)
		if (obj->port == port) {
			snprintf(obj->ifname, sizeof(obj->ifname), "%s", ifname);
			obj->mode = mode;
			obj->type = type;
			obj->vlan   = vlan;
			break;
		}
	ppa_lock_release2(&g_phys_port_lock, irq_flag);

	if (!obj) {
		obj = ppa_phys_port_alloc_item();
		if (!obj)
			return PPA_ENOMEM;
		snprintf(obj->ifname, sizeof(obj->ifname), "%s", ifname);
		obj->mode   = mode;
		obj->type   = type;
		obj->port   = port;
		obj->vlan   = vlan;
		irq_flag = ppa_lock_get2(&g_phys_port_lock);
		obj->next = g_phys_port_info;
		g_phys_port_info = obj;
		ppa_lock_release2(&g_phys_port_lock, irq_flag);
	}
	return PPA_SUCCESS;
}

int32_t ppa_phys_port_remove(uint32_t port)
{
	struct phys_port_info *p_prev, *p_cur;
	uint32_t irq_flags = 0;

	p_prev = NULL;

	/*trick here with ppa_lock_get2 since it will be called in irqs_disabled
	 * mode by directpath wlan registering
	 */
	irq_flags = ppa_lock_get2(&g_phys_port_lock);
	p_cur = g_phys_port_info;
	for (; p_cur; p_prev = p_cur, p_cur = p_cur->next)
		if (p_cur->port == port) {
			if (!p_prev)
				g_phys_port_info = p_cur->next;
			else
				p_prev->next = p_cur->next;
			ppa_lock_release2(&g_phys_port_lock, irq_flags);
			ppa_phys_port_free_item(p_cur);
			return 0;
		}
	ppa_lock_release2(&g_phys_port_lock, irq_flags);
	return 0;
}

int32_t ppa_phys_port_get_first_eth_lan_port(uint32_t *ifid,
						 PPA_IFNAME **ifname)
{
	int32_t ret;
	struct phys_port_info *obj;

	if (!ifid || !ifname)
		return PPA_EINVAL;

	ret = PPA_ENOTAVAIL;

	ppa_lock_get(&g_phys_port_lock);
	for (obj = g_phys_port_info; obj; obj = obj->next)
		if (obj->mode == 1 && obj->type == 2) {
			*ifid = obj->port;
			*ifname = &(obj->ifname[0]);
			ret = PPA_SUCCESS;
			break;
		}
	ppa_lock_release(&g_phys_port_lock);

	return ret;
}

int32_t ppa_phys_port_start_iteration(uint32_t *ppos,
					  struct phys_port_info **ifinfo)
{
	uint32_t l;
	struct phys_port_info *p;

	ppa_lock_get(&g_phys_port_lock);

	p = g_phys_port_info;
	l = *ppos;
	while (p && l) {
		p = p->next;
		l--;
	}

	if (l == 0 && p) {
		++*ppos;
		*ifinfo = p;
		return PPA_SUCCESS;
	} else {
		*ifinfo = NULL;
		return PPA_FAILURE;
	}
}
EXPORT_SYMBOL(ppa_phys_port_start_iteration);

int32_t ppa_phys_port_iterate_next(uint32_t *ppos,
				   struct phys_port_info **ifinfo)
{
	if (*ifinfo) {
		++*ppos;
		*ifinfo = (*ifinfo)->next;
		return *ifinfo ? PPA_SUCCESS : PPA_FAILURE;
	} else {
		return PPA_FAILURE;
	}
}
EXPORT_SYMBOL(ppa_phys_port_iterate_next);

void ppa_phys_port_stop_iteration(void)
{
	ppa_lock_release(&g_phys_port_lock);
}
EXPORT_SYMBOL(ppa_phys_port_stop_iteration);

#if defined(PPA_IF_MIB) && PPA_IF_MIB
static void ppa_add_subif(struct netif_info *pnet_if, const char *ifname)
{
	int32_t idxIter = 0;

	/* Return if subif already added */
	for (idxIter = 0; idxIter < pnet_if->sub_if_index; idxIter++) {
		if (!strcmp(pnet_if->sub_if_name[idxIter], ifname)) {
			return;
		}
	}
	if (pnet_if->sub_if_index < PPA_IF_SUB_NAME_MAX_NUM) {
		ppa_strncpy(pnet_if->sub_if_name[pnet_if->sub_if_index],
				ifname, PPA_IF_NAME_SIZE);
		pnet_if->sub_if_index++;
	}
}

static void ppa_remove_subif(struct netif_info *pnet_if, const char *ifname)
{
	int32_t idxIter = 0;
	int32_t subif_index = -1;

	for (idxIter = 0; idxIter < pnet_if->sub_if_index; idxIter++) {
		if (subif_index < 0) {
			if (!strcmp(pnet_if->sub_if_name[idxIter], ifname)) {
				ppa_memset(&pnet_if->sub_if_name[idxIter], '\0',
						PPA_IF_NAME_SIZE);
				subif_index = idxIter;
				continue;
			}
		} else {
			ppa_strncpy(pnet_if->sub_if_name[idxIter - 1],
					pnet_if->sub_if_name[idxIter],
					PPA_IF_NAME_SIZE);
			ppa_memset(&pnet_if->sub_if_name[idxIter], '\0',
						PPA_IF_NAME_SIZE);
		}
	}

	if (subif_index >= 0)
		pnet_if->sub_if_index--;
}
#else
#define ppa_add_subif(pnet_if, ifname)
#define ppa_remove_subif(pnet_if, ifname)
#endif

#if defined(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500
/* Add pce rule to avoid local traffic from hitting the acceleration lookup */
static void ppa_add_bridge_class_rule(struct netif_info *p_ifinfo,
					  PPA_NETIF *netif)
{
	uint32_t brip = 0;
	PPA_CLASS_RULE class_rule;

	if (ppa_get_netif_ip(&brip, netif) == PPA_SUCCESS) {
		ppa_memset(&class_rule, 0, sizeof(PPA_CLASS_RULE));

		class_rule.in_dev = GSWR_INGRESS;
		class_rule.category = CAT_MGMT;

		class_rule.pattern.bEnable = 1;
		class_rule.pattern.eDstIP_Select = 1;
		class_rule.pattern.nDstIP.nIPv4 = brip;
		class_rule.pattern.nDstIP_Mask = 0xFF00;

		class_rule.action.fwd_action.rtaccelenable = 0;
		class_rule.action.fwd_action.rtctrlenable = 1;

		class_rule.action.fwd_action.processpath = 4;

		class_rule.action.rmon_action = 1;
		class_rule.action.rmon_id = 23;

		if (ppa_add_class_rule(&class_rule) == PPA_SUCCESS)
			p_ifinfo->fid_index = class_rule.uidx;
	}
}

static bool ppa_phys_port_is_fcs_required(int32_t port_id)
{
	int32_t ret;
	GSW_API_HANDLE gswr = 0;
	GSW_PMAC_Eg_Cfg_t egCfg = {0};
	bool is_fcs_required = false;

	if ((port_id < 7) || (port_id == 15))
		return false;

	gswr = gsw_api_kopen("/dev/switch_api/1");
	if (gswr == 0)
		return false;

	egCfg.nDestPortId = port_id;
	egCfg.nTrafficClass = 0;
	egCfg.nFlowIDMsb = 0;
	ret = gsw_api_kioctl(gswr, GSW_PMAC_EG_CFG_GET, &egCfg);
	if (ret >= GSW_statusOk)
		is_fcs_required = egCfg.bFcsEna;

	gsw_api_kclose(gswr);

	return is_fcs_required;
}
#endif

static void ppa_update_netif_name(PPA_NETIF *netif)
{
	struct netif_info *p;
	bool found = false;
	PPA_IFNAME ifname[PPA_IF_NAME_SIZE];

	ifname[0] = 0;

	ppa_netif_lock_list();
	for (p = g_netif; p; p = p->next)
		if (p->netif == netif) {
			found = true;
			ppa_strncpy(ifname, p->name, PPA_IF_NAME_SIZE);
			break;
		}

	if (!found || ifname[0] == 0) {
		ppa_netif_unlock_list();
		return;
	}

	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
		  "updating netif name change [%s:%s]\n",
		  ifname, ppa_get_netif_name(netif));

	for (p = g_netif; p; p = p->next) {
		if (ppa_str_cmp(p->name, ifname))
			ppa_strncpy(p->name, netif->name, PPA_IF_NAME_SIZE);

		if (ppa_str_cmp(p->lower_ifname, ifname))
			ppa_strncpy(p->lower_ifname, netif->name,
					PPA_IF_NAME_SIZE);

		if (ppa_str_cmp(p->phys_netif_name, ifname))
			ppa_strncpy(p->phys_netif_name, netif->name,
					PPA_IF_NAME_SIZE);

	}
	ppa_netif_unlock_list();
}

static void ppa_update_netif_mac(PPA_NETIF *netif)
{
	PPA_IFNAME *ifname;
	struct netif_info *p_ifinfo = NULL;

	ifname = ppa_get_netif_name(netif);
	if (ifname == NULL)
		return;

#if IS_ENABLED(CONFIG_PPA_ACCEL)
	ppa_remove_sessions_on_netif(netif);
#endif /*def CONFIG_PPA_ACCEL*/

	if (ppa_get_netif_info(ifname, &p_ifinfo) == PPA_SUCCESS &&
		p_ifinfo != NULL) {
		if (ppa_get_netif_hwaddr(p_ifinfo->netif, p_ifinfo->mac,
					 1) == PPA_SUCCESS &&
				is_valid_ether_addr(p_ifinfo->mac)) {
			p_ifinfo->flags |= NETIF_MAC_AVAILABLE;
		}
		ppa_netif_put(p_ifinfo);
	}
}

static void ppa_register_new_netif(PPA_NETIF *netif)
{
	PPA_IFNAME phy_ifname[PPA_IF_NAME_SIZE] = {0};
	PPA_IFNAME *ifname;
	struct netif_info *p_ifinfo = NULL;
	int f_is_lan = 0;
	int force_wanitf = 0;
	struct iface_list *local;

	ifname = ppa_get_netif_name(netif);
	if (ifname == NULL)
		return;

	/* Maually deleted ifname from PPA should not be added until it is
	 * explicitaly added.
	 */

	ppa_manual_if_lock_list();
	ppa_list_for_each_entry(local, &manual_del_iface_list, node_ptr)
		if (ppa_str_cmp(local->name, ifname)) {
			ppa_manual_if_unlock_list();
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				  "Manually deleted[%s] cannot be auto added\n",
				  ifname);
			return;
		}
	ppa_manual_if_unlock_list();

	/* Iface was already added into PPA durig init or before
	 * creation of netif, and needs relearning after netif creation
	 */
	if (ppa_get_netif_info(ifname, &p_ifinfo) == PPA_SUCCESS &&
			p_ifinfo != NULL) {
		if (p_ifinfo->flags & NETIF_CONTAINER_VETH_UP) {
			p_ifinfo->flags &= ~NETIF_CONTAINER_VETH_UP;
			p_ifinfo->flags |= NETIF_CONTAINER_VETH_UPDATE;
		}
		ppa_netif_put(p_ifinfo);
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			  "Update already exsiting iface[%s]\n", ifname);
		ppa_netif_update(netif, NULL);
		return;
	}

	/* New netif will automatically be added into PPA if its physcial
	 * is already present in PPA. If netif is bridge, it will be added
	 * to LAN group. All handling done after ppacmd init.
	 */
	if ((ppa_get_physical_if(netif, NULL, phy_ifname) == PPA_SUCCESS) &&
		(strnlen(phy_ifname, PPA_IF_NAME_SIZE) > 0)) {
		if ((ppa_get_netif_info(phy_ifname, &p_ifinfo) == PPA_SUCCESS)
			&& (p_ifinfo != NULL)) {
			f_is_lan = p_ifinfo->flags & NETIF_LAN_IF;
			force_wanitf = p_ifinfo->f_wanitf.flag_force_wanitf;
			ppa_netif_put(p_ifinfo);

			if (ppa_netif_add(ifname, f_is_lan, NULL, NULL,
					  force_wanitf) != PPA_SUCCESS)
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
					  "Adding iface[%s] to PPA failed\n",
					  ifname);
		} else if (ppa_if_is_br_if(netif, NULL)) {
			ppa_netif_add(ifname, NETIF_LAN_IF, NULL, NULL, 0);
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				  "dynamically adding bridge iface[%s]\n",
				  ifname);
		} else if (ppa_if_is_br2684(netif, NULL) ||
				   ppa_if_is_pppoa(netif, NULL) ||
				   ppa_if_is_ipoa(netif, NULL)) {
			ppa_netif_add(ifname, f_is_lan, NULL, NULL, 0);
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				  "dynamically adding ATM iface[%s]\n", ifname);
		}
	} else {
		if (ppa_if_is_veth_if(netif, NULL)) {
			ppa_netif_add(ifname, NETIF_LAN_IF, NULL, NULL, 0);
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				"dynamically adding veth iface[%s]\n", ifname);
		}
	}
}

static void ppa_unregister_new_netif(PPA_NETIF *netif)
{
	PPA_IFNAME phy_ifname[PPA_IF_NAME_SIZE];
	PPA_IFNAME *ifname = NULL;
	struct netif_info *p_ifinfo = NULL;
	int f_is_lan = 0;

	phy_ifname[0] = 0;

	ifname = ppa_get_netif_name(netif);

	if (ifname == NULL)
		return;

	/* Logical netif will automatically be removed from PPA after
	 * ppacmd init. All handling is done after ppacmd init
	 */
 
	if (ppa_get_netif_info(ifname, &p_ifinfo) != PPA_SUCCESS
		|| p_ifinfo == NULL)
		return;

	f_is_lan = p_ifinfo->flags & NETIF_LAN_IF;

	if (p_ifinfo->flags & NETIF_VETH) {
		if (p_ifinfo->flags & NETIF_CONTAINER_VETH_MOVE || p_ifinfo->flags & NETIF_CONTAINER_VETH_UPDATE)
			ppa_netif_remove(ifname, f_is_lan);
		ppa_netif_put(p_ifinfo);
		return;
	}

	if (ppa_memcmp(p_ifinfo->name, p_ifinfo->phys_netif_name,
			   strnlen(p_ifinfo->name, PPA_IF_NAME_SIZE)) != 0) {
		ppa_netif_put(p_ifinfo);
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			  "dynamically removing netif[%s]\n", ifname);
		ppa_netif_remove(ifname, f_is_lan);
		return;
	} else {
		/* Physical netif removed from system.
		 * So assigning NULL to PPA netif entry.
		 */
		p_ifinfo->netif = NULL;
		ppa_netif_put(p_ifinfo);
	}
	if (ppa_if_is_br_if(netif, NULL)) {
		ppa_netif_remove(ifname, f_is_lan);
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			  "dynamically removing bridge[%s]\n", ifname);
	} else if (ppa_if_is_br2684(netif, NULL) ||
			   ppa_if_is_pppoa(netif, NULL) ||
			   ppa_if_is_ipoa(netif, NULL)) {
		ppa_netif_remove(ifname, f_is_lan);
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			  "dynamically removing ATM iface[%s]\n", ifname);
	}
}

#ifdef PPA_IF_STAT_HELPER
static void ppa_netif_up_stats(struct netif_info *p_ifinfo, PPA_NETIF *netif)
{
	if (p_ifinfo == NULL || netif == NULL){
		ppa_debug(DBG_ENABLE_MASK_ERR,
				"ppa_netif_down_stats: netif is NULL\n");
		return;
	}

	if((strncmp(p_ifinfo->name, p_ifinfo->phys_netif_name, PPA_IF_NAME_SIZE) != 0)
#if IS_ENABLED(CONFIG_SOC_GRX500)
		&& (netif->netdev_ops->ndo_get_stats64 != ppa_stat_logical_intf_get_extended_stats64)
#endif
	){
		if (p_ifinfo->netdev_ops == NULL) {
			p_ifinfo->netdev_ops = (struct net_device_ops *)ppa_malloc(sizeof(struct net_device_ops));
			if (p_ifinfo->netdev_ops == NULL) {
				ppa_debug(DBG_ENABLE_MASK_ERR,
						"Could not allocate memory for net dev ops backup\n");
				return;
			}
		}
		ppa_memcpy(p_ifinfo->netdev_ops, netif->netdev_ops, sizeof(struct net_device_ops));
		if (dp_set_net_dev_ops(netif, (void *)&ppa_stat_logical_intf_get_extended_stats64,
			offsetof(struct net_device_ops, ndo_get_stats64), DP_OPS_NETDEV) != DP_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_ERR,
				"Set of netdev_ops failled for :[%s]\n", p_ifinfo->name);
		}
	}
}

static void ppa_netif_down_stats(struct netif_info *p_ifinfo, PPA_NETIF *netif)
{
	if (p_ifinfo == NULL || netif == NULL){
		ppa_debug(DBG_ENABLE_MASK_ERR,
				"ppa_netif_down_stats: netif is NULL\n");
		return;
	}
	if((strncmp(p_ifinfo->name, p_ifinfo->phys_netif_name, PPA_IF_NAME_SIZE) != 0)
#if IS_ENABLED(CONFIG_SOC_GRX500)
		&& (netif->netdev_ops->ndo_get_stats64 == ppa_stat_logical_intf_get_extended_stats64)
#endif
	){
		if (p_ifinfo->netdev_ops) {
			if (dp_set_net_dev_ops(netif, NULL,
				offsetof(struct net_device_ops, ndo_get_stats64), DP_OPS_NETDEV | DP_OPS_RESET) != DP_SUCCESS) {
				ppa_debug(DBG_ENABLE_MASK_ERR,
					"Reset of netdev_ops failled for :[%s]\n", p_ifinfo->name);
			}
			ppa_free(p_ifinfo->netdev_ops);
			p_ifinfo->netdev_ops = NULL;
		}
	}

}
#else
#define ppa_netif_down_stats(ifinfo, netif)
#define ppa_netif_up_stats(ifinfo, netif)
#endif /* PPA_IF_STAT_HELPER */

static void ppa_netif_up(PPA_NETIF *netif)
{
	struct netif_info *p_ifinfo = NULL;
	PPA_IFNAME *ifname = NULL;
	/* ppa_get_physical_if() fails for pppoe net_dev because call to
	 * ppa_check_is_pppoe_netif() also validated only if pppoe_addr
	 * is assigned to fetch the physical interface.
	 * So, adding pppoe into ppa on up event
	 */
	if (ppa_check_is_ppp_netif(netif) || ppa_is_macvlan_if(netif, ifname))
		ppa_register_new_netif(netif);

	ifname = ppa_get_netif_name(netif);
	if (ifname == NULL)
		return;

	if (ppa_get_netif_info(ifname, &p_ifinfo) == PPA_SUCCESS) {
		p_ifinfo->enable = true;
		if (p_ifinfo->flags & NETIF_VETH) {
			p_ifinfo->flags |= NETIF_CONTAINER_VETH_UP;
			p_ifinfo->flags &= ~NETIF_CONTAINER_VETH_UPDATE;
			p_ifinfo->flags &= ~NETIF_CONTAINER_VETH_MOVE;
		}
		/* replace get stats function for logical interface */
		ppa_netif_up_stats(p_ifinfo, netif);
		ppa_netif_put(p_ifinfo);
	}
}

static void ppa_netif_down(PPA_NETIF *netif)
{
	struct netif_info *p_ifinfo = NULL;
	PPA_IFNAME *ifname;

	ifname = ppa_get_netif_name(netif);
	if (ifname == NULL)
		return;

	if (ppa_get_netif_info(ifname, &p_ifinfo) == PPA_SUCCESS) {
		p_ifinfo->enable = false;
		if (p_ifinfo->flags & NETIF_VETH) {
			p_ifinfo->flags &= ~NETIF_CONTAINER_VETH_UP;
			if (ppa_is_netif_bridged(p_ifinfo->netif))
				p_ifinfo->flags |= NETIF_CONTAINER_VETH_UPDATE;
			else
				p_ifinfo->flags |= NETIF_CONTAINER_VETH_MOVE;
		}

		/* reset interface get stats function for logical interface */
		ppa_netif_down_stats(p_ifinfo, netif);
		ppa_netif_put(p_ifinfo);
	}
#if IS_ENABLED(CONFIG_PPA_ACCEL)
	ppa_remove_sessions_on_netif(netif);
#endif /*def CONFIG_PPA_ACCEL*/

	/* deleting pppoe from ppa on down event, as added it on up event */
	if (ppa_check_is_ppp_netif(netif) || ppa_is_macvlan_if(netif, ifname))
		ppa_unregister_new_netif(netif);
}

#if IS_ENABLED(CONFIG_SOC_GRX500)
/*
 * Update VLAN settings for LAN ports in bridge to reset cvid_remove action.
 * This updation of VLAN setting is done for Q-in-Q feature.
 */
static void ppa_reset_lan_cvlan_remove_action(struct netif_info *p_ifinfo) {
	PPA_OUT_VLAN_INFO vlan_info = {0};
	struct netif_info *ifinfo;
	uint32_t pos = 0;

	/* VLAN settings update is not needed for PRX platforms */
	if (IS_ENABLED(CONFIG_PRX300_CQM))
		return;

	if (p_ifinfo == NULL)
		return;

	if (p_ifinfo->brif == NULL || !(ppa_is_netif_bridged(p_ifinfo->netif)))
		return;

	if ((p_ifinfo->flags & NETIF_VLAN_INNER) &&
		(p_ifinfo->in_vlan_netif)) {
		vlan_info.port_id = p_ifinfo->phys_port;
		vlan_info.subif_id = ((p_ifinfo->subif_id & 0xF00) >> 8);
		if (ppa_hsel_get_outer_vlan_entry (&vlan_info, 0,
					PAE_HAL) != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_ERR,
					"vlan entry not found :[%s]\n", p_ifinfo->name);
			return;
		}
		vlan_info.vlan_type = ppa_get_vlan_type(p_ifinfo->in_vlan_netif);
	}

	if (vlan_info.vlan_type == PPA_VLAN_PROTO_8021AD &&
			vlan_info.ctag_rem == 0)
	{
		/*
		 * Loop through interfaces in bridge and do following for each:
		 * 1. Get current vlan info
		 * 2. Reset the ctag_rem bit
		 * 3, Save the new vlan info
		 */
		if (ppa_netif_start_iteration(&pos, &ifinfo) == PPA_SUCCESS) {
			do {
				/*
				 * Need to reset only for LAN interfaces in bridge.
				 * Skip for rest.
				 */
				if (!(ppa_is_netif_bridged(ifinfo->netif)) ||
					(strcmp(ifinfo->brif->name, p_ifinfo->brif->name) != 0) ||
					!(ifinfo->flags & NETIF_LAN_IF)) {
					continue;
				}
				ppa_memset(&vlan_info, 0, sizeof(PPA_OUT_VLAN_INFO));
				vlan_info.port_id = ifinfo->phys_port;
				vlan_info.subif_id = ((ifinfo->subif_id & 0xF00) >> 8);
				if (ppa_hsel_get_outer_vlan_entry (&vlan_info, 0,
							PAE_HAL) == PPA_SUCCESS) {
					vlan_info.ctag_rem = 0;
					vlan_info.vlan_type = PPA_VLAN_PROTO_8021AD;
					if (ppa_hsel_add_outer_vlan_entry (&vlan_info, 0,
							PAE_HAL) != PPA_SUCCESS) {
						ppa_debug(DBG_ENABLE_MASK_ERR,
							"Failed to set vlan entry\n");
					}
				}
			} while (ppa_netif_iterate_next(&pos, &ifinfo) == PPA_SUCCESS);
		}
		ppa_netif_stop_iteration();
	}
	else {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				"No need to update vlan settings of LAN ports\n");
	}
}

static void ppa_handle_bridge_update(PPA_NETIF *netif)
{
	struct netif_info *p_ifinfo = NULL;
	PPA_BR_PORT_INFO port_info;

	if (ppa_netif_lookup(ppa_get_netif_name(netif), &p_ifinfo) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR,
					"bridge info not updated for :[%s]\n", p_ifinfo->name);
		return;
	}

	ppa_memset(&port_info, 0, sizeof(PPA_BR_PORT_INFO));
	port_info.brid = p_ifinfo->fid;
	port_info.port = p_ifinfo->phys_port;
	port_info.index =  p_ifinfo->fid_index;

	if (p_ifinfo->brif != NULL) {
#if IS_ENABLED(CONFIG_PPA_ACCEL)
		ppa_remove_sessions_on_netif(netif);
#endif /*def CONFIG_PPA_ACCEL*/
		ppa_drv_del_br_port(&port_info, 0);
		p_ifinfo->brif = NULL;
	}
	/* Checking always to avoid mis-config due missing or duble event */
	ppa_rcu_read_lock();
	p_ifinfo->brif = ppa_netdev_master_upper_dev_get(p_ifinfo->netif);
	ppa_rcu_read_unlock();

	if (p_ifinfo->brif != NULL) {
		ppa_update_bridge_info(p_ifinfo, p_ifinfo->netif);
		/*
		 * Update cvlan settings of LAN interfaces for the bridge WAN interface
		 * with VLAN type 0x88A8
		 */
		ppa_reset_lan_cvlan_remove_action(p_ifinfo);
	} else {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
					"bridge info not found for :[%s]\n", p_ifinfo->name);
	}
}
#endif /* CONFIG_SOC_GRX500 */

static int ppa_phy_netdevice_event(PPA_NOTIFIER_BLOCK *nb,
				unsigned long action, void *ptr)
{
	PPA_NETIF *netif = ppa_netdev_notifier_info_to_dev(ptr);

	switch (action) {
	case NETDEV_REGISTER:
		/* do register only for non PPP interface; 
		for PPP interface it is done in NETDEV_UP*/
		if (!(ppa_check_is_ppp_netif(netif) || ppa_is_macvlan_if(netif, netif->name)))
			ppa_register_new_netif(netif);
		break;
	case NETDEV_UNREGISTER:
		ppa_unregister_new_netif(netif);
		break;
	case NETDEV_CHANGEADDR:
		ppa_update_netif_mac(netif);
	case NETDEV_CHANGENAME:
		ppa_update_netif_name(netif);
		break;
	case NETDEV_UP:
		ppa_netif_up(netif);
		break;
	case NETDEV_DOWN:
		ppa_netif_down(netif);
		break;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	case NETDEV_CHANGEUPPER:
		if (ppa_is_netif_bridged(netif)) {
			ppa_handle_bridge_update(netif);
		}
		break;
#endif /* CONFIG_SOC_GRX500 */
	case NETDEV_PRE_UP:
	case NETDEV_CHANGE:
	case NETDEV_POST_INIT:
	case NETDEV_POST_TYPE_CHANGE:
		ppa_netif_update(netif, NULL);
		break;
	}
	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"ppa_netdevice_event[%s:%lu]\n",
			ppa_get_netif_name(netif), action);
	return PPA_NOTIFY_OK;
}

PPA_NOTIFIER_BLOCK ppa_netdevice_notifier = {
	.notifier_call = ppa_phy_netdevice_event
};

#if defined(CONFIG_PPA_PUMA7) && defined(CONFIG_TI_HIL_PROFILE_INTRUSIVE_P7)
static void ppa_netdev_pp_prepare_vpid(struct netif_info *p_ifinfo,
					   uint32_t is_lan)
{
	PPA_NETIF *netif, *lnetif;
	uint16_t setup_flag = 0;
	uint32_t ip = 0;

	PPA_IFNAME underlying_ifname[PPA_IF_NAME_SIZE];

	lnetif = netif = NULL;
	underlying_ifname[0] = '\0';

	netif = ppa_get_netif(p_ifinfo->name);
	if (netif != NULL) {
		/* check if this is under WAN interface */
		if (! is_lan)
			setup_flag = 1;
		}
		if (p_ifinfo->flags & NETIF_PPPOE)
			setup_flag = 0;
		/* check if this is a bonded slave interface */
		if (ppa_is_bond_slave(NULL, netif))
			setup_flag = 1;
		/* check if interface is under bridge */
		if (ppa_is_netif_bridged(netif))
			setup_flag = 1;
		/* check if interface is LAN and not bridge */
		if (is_lan && !(ppa_if_is_br_if(netif, NULL)))
			setup_flag = 1;

		if ((p_ifinfo->flags & NETIF_PHY_TUNNEL) ||
				(p_ifinfo->flags & NETIF_PPPOL2TP))
			setup_flag = 0;
		if (setup_flag) {
			/*for virtual interface copy pid_handle and do netdev cloning*/
			lnetif = ppa_get_netif(p_ifinfo->phys_netif_name);
			if (lnetif != NULL && netif != NULL) {
				netif->vpid_block.parent_pid_handle = lnetif->pid_handle;
				netif->pid_handle = lnetif->pid_handle;
				ti_hil_clone_netdev_pp_info(netif, lnetif);
			}

			/* for VLAN interface set vpid block type as
			 * AVALANCHE_PP_VPID_VLAN
			 */
			if (p_ifinfo->flags & NETIF_VLAN) {
				netif->vpid_block.type = AVALANCHE_PP_VPID_VLAN;
				if (p_ifinfo->flags & NETIF_VLAN_INNER)
					netif->vpid_block.vlan_identifier = p_ifinfo->inner_vid;
			} else {
				netif->vpid_block.type = AVALANCHE_PP_VPID_ETHERNET;
			}
			ti_hil_pp_event (TI_PP_ADD_VPID, netif);
		}
	}
	return;
}

static void ppa_netdev_pp_remove_vpid(struct netif_info *p_ifinfo)
{
	PPA_NETIF *lnetif, *netif;
	PPA_IFNAME underlying_ifname[PPA_IF_NAME_SIZE];
	netif = ppa_get_netif(p_ifinfo->name);
	if(netif != NULL) {
		/* for NETIF_PPPOE, delete VPID of actual interafce */
		if (p_ifinfo->flags & NETIF_PPPOE &&
				(ppa_pppoe_get_physical_if(netif, NULL,
							   underlying_ifname) == PPA_SUCCESS)) {
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
	return;
}
#endif

PPA_NETIF* ppa_get_lower_dev(PPA_NETIF *dev, struct net *net)
{
	PPA_IFNAME lowIfName[PPA_IF_NAME_SIZE] = {0};
	PPA_NETIF *lowerIf = NULL;
	int32_t ret = PPA_FAILURE;
	PPA_NETIF *curr_dev;
	struct list_head *iter;

	if (dev == NULL)
		return dev;

	/* Assumption: netif should be valid */
	netdev_for_each_lower_dev(dev, curr_dev, iter) {
		return curr_dev;
	}

	if (ppa_check_is_ppp_netif(dev)) {
		if (ppa_if_is_pppoa(dev, NULL)) {
			goto lowerIfFound;
		}
		else if (ppa_check_is_pppol2tp_netif(dev)) {
			ret = ppa_pppol2tp_get_base_netif(dev, lowIfName);
		}
		else if (ppa_check_is_pppoe_netif(dev)) {
			ret = ppa_pppoe_get_eth_netif(dev, lowIfName);
		}

		if (ret == PPA_SUCCESS)
			lowerIf = ppa_get_netif_by_net(net, lowIfName);
		goto lowerIfFound;
	}

#if IS_ENABLED(CONFIG_MACVLAN)
	if (netif_is_macvlan(dev)) {
		lowerIf = macvlan_dev_real_dev(dev);
		goto lowerIfFound;
	}
#endif
	if (ppa_is_gre_netif(dev)) {
		lowerIf = ppa_get_gre_phyif(dev);
		goto lowerIfFound;
	}

#if IS_ENABLED(CONFIG_SOC_GRX500)
	if (ppa_is_vxlan_netif(dev)) {
		lowerIf = ppa_get_vxlan_phyif(dev);
		goto lowerIfFound;
	}
#endif

#if IS_ENABLED(CONFIG_VLAN_8021Q)
	if ( (dev->priv_flags & IFF_802_1Q_VLAN) ) {
		lowerIf = vlan_dev_priv(dev)->real_dev;
		goto lowerIfFound;
	}
#endif
#if IS_ENABLED(CONFIG_IPV6_SIT)
	if (dev->type == ARPHRD_SIT) {
		lowerIf = ppa_get_6rd_phyif(dev);
		goto lowerIfFound;
	}
#endif
	if (dev->type == ARPHRD_TUNNEL) {
		lowerIf = ppa_get_6rd_phyif(dev);
		goto lowerIfFound;
	}
#if IS_ENABLED(CONFIG_IPV6_TUNNEL)
	if (dev->type == ARPHRD_TUNNEL6) {
		lowerIf = ppa_get_ip4ip6_phyif(dev);
		goto lowerIfFound;
	}
#endif

lowerIfFound:
	return lowerIf;
}

struct netif_info* ppa_iface_init_alloc(PPA_NETIF *dev,
			  PPA_IFNAME *ifname_lower,
			  PPA_IFNAME *ifname,
			  struct net *net)
{
	struct netif_info *ifInfo = NULL;
	PPA_NETIF *lowerifInfo = NULL;
	PPA_NETIF *brif = NULL;

	if ( (ifInfo = ppa_netif_alloc_item() ) ) {
		ppa_strncpy(ifInfo->name, ifname, sizeof(ifInfo->name));
		if (ifname_lower && strnlen(ifname_lower, PPA_IF_NAME_SIZE)) {
			/* Copies lower interface name to netif_info's lower_ifname */
			ppa_strncpy(ifInfo->lower_ifname, ifname_lower, sizeof(ifInfo->lower_ifname)-1);
		} else if (dev && ppa_if_is_br_if(dev, NULL)) {
			/* Not updating lower interface for bridge */
			ifInfo->flags |= NETIF_BRIDGE;
		} else {
			/* If the lower interface name doesn't exists, set lower_ifname
			 * as 0 and search for lower interface */
			lowerifInfo = ppa_get_lower_dev(dev, net);
			if (lowerifInfo != NULL)
				ppa_strncpy(ifInfo->lower_ifname, lowerifInfo->name, sizeof(ifInfo->lower_ifname)-1);
		}

		if (dev && ppa_is_netif_bridged(dev)) {
			/* Checks if netif is a bridge interface */
			ppa_rcu_read_lock();
			brif = ppa_netdev_master_upper_dev_get(dev);
			ppa_rcu_read_unlock();
		}
		ifInfo->hw_accel_stats.rx_bytes = 0;
		ifInfo->hw_accel_stats.tx_bytes = 0;
		ifInfo->sw_accel_stats.rx_bytes = 0;
		ifInfo->sw_accel_stats.tx_bytes = 0;
#if IS_ENABLED(CONFIG_IPV4_IPV6_COUNTER_SUPPORT)
		ifInfo->hw_accel_stats.rx_pkts = 0;
		ifInfo->hw_accel_stats.tx_pkts = 0;
		ifInfo->hw_accel_stats.rx_pkts_ipv4 = 0;
		ifInfo->hw_accel_stats.tx_pkts_ipv4 = 0;
		ifInfo->hw_accel_stats.rx_pkts_ipv6 = 0;
		ifInfo->hw_accel_stats.tx_pkts_ipv6 = 0;

		ifInfo->sw_accel_stats.rx_pkts = 0;
		ifInfo->sw_accel_stats.tx_pkts = 0;
		ifInfo->sw_accel_stats.rx_pkts_ipv4 = 0;
		ifInfo->sw_accel_stats.tx_pkts_ipv4 = 0;
		ifInfo->sw_accel_stats.rx_pkts_ipv6 = 0;
		ifInfo->sw_accel_stats.tx_pkts_ipv6 = 0;
#endif /* CONFIG_IPV4_IPV6_COUNTER_SUPPORT */

		if (dev == NULL) {
			ifInfo->netif = NULL;
			return ifInfo;
		}

		ifInfo->netif = dev;
		ifInfo->net = net;
		ifInfo->mtu = dev->mtu;
		ifInfo->brif = brif;
	}
	return ifInfo;
}

int32_t ppa_update_ppp_if(PPA_NETIF *dev, struct netif_info *ifInfo)
{
	PPA_VCC *vcc;
	int32_t dslwan_qid;
	struct netif_info *lower_if_info = NULL;

	if ((ppa_if_is_ipoa(dev, NULL) &&
		(ppa_br2684_get_vcc(dev, &vcc) == PPA_SUCCESS)) ||
		(ppa_if_is_pppoa(dev, NULL) &&
		(ppa_pppoa_get_vcc(dev, &vcc) == PPA_SUCCESS))) {

		dslwan_qid = ppa_drv_get_dslwan_qid_with_vcc(vcc);
		if ( dslwan_qid >= 0 ) {
			ifInfo->flags &= ~NETIF_PHY_ETH;
			if(ppa_if_is_ipoa(dev, NULL))
				ifInfo->flags |= NETIF_PHY_ATM | NETIF_BR2684 | NETIF_IPOA;
			else
				ifInfo->flags |= NETIF_PHY_ATM | NETIF_PPPOATM;

			ifInfo->vcc = vcc;
			ifInfo->dslwan_qid = dslwan_qid;
			return PPA_SUCCESS;
		}
	}
	else if (ppa_check_is_ppp_netif(dev)) {
		if (ppa_if_is_ipoa(dev, NULL)) {
			/* Allow IPoA interface, although ppa_br2684_get_vcc() failed above.
			 * NOTE: VCC information is to be updated later. */
			return PPA_SUCCESS;
		} else if (ppa_check_is_pppoe_netif(dev)) {
			ifInfo->pppoe_session_id = ppa_pppoe_get_pppoe_session_id(dev);
			ifInfo->flags |= NETIF_PPPOE;
			return PPA_SUCCESS;
		}
		else if (ppa_check_is_pppol2tp_netif(dev)) {
			ifInfo->pppol2tp_session_id = ppa_pppol2tp_get_l2tp_session_id(dev);
			ifInfo->pppol2tp_tunnel_id = ppa_pppol2tp_get_l2tp_tunnel_id(dev);
			ifInfo->flags |= NETIF_PPPOL2TP;
			if (ppa_get_netif_info(ifInfo->lower_ifname, &lower_if_info) == PPA_SUCCESS) {
				ifInfo->pppoe_session_id = lower_if_info->pppoe_session_id;
				ppa_netif_put(lower_if_info);
			}

			return PPA_SUCCESS;
		}
	}
	return PPA_FAILURE;
}

int32_t ppa_add_iface_on_lower_exists(PPA_NETIF *netif,
		struct netif_info *ifInfo)
{
	struct netif_info *prev_info = NULL;
	int index = 0;

	if (ppa_get_netif_info(netif->name, &prev_info) == PPA_SUCCESS) {
		if ( !(ifInfo->flags & NETIF_MAC_AVAILABLE) ) {
			ppa_strncpy(ifInfo->mac, prev_info->mac,
					sizeof(ifInfo->mac));
		}
		ifInfo->flags |= prev_info->flags;
		if (ifInfo->flags & NETIF_PHY_ATM)
			ifInfo->flags &= ~NETIF_PHY_ETH;

		ppa_strncpy(ifInfo->phys_netif_name, prev_info->phys_netif_name,
				sizeof(ifInfo->phys_netif_name));

		while (index < prev_info->vlan_layer) {
			ifInfo->vlan_stack[ifInfo->vlan_layer++] = prev_info->
				vlan_stack[index++];
			if (ifInfo->vlan_layer >= PPA_MAX_VLANS)
				break;
		}

		ifInfo->phys_port = prev_info->phys_port;

		if (!ifInfo->inner_vid)
			ifInfo->inner_vid = prev_info->inner_vid;
		else if(ifInfo->vlan_layer > 1)
			ifInfo->outer_vid = prev_info->inner_vid;
		ifInfo->in_vlan_netif = prev_info->in_vlan_netif;

		ppa_netif_put(prev_info);
		return PPA_SUCCESS;
	}

	return PPA_FAILURE;
}


#if IS_ENABLED(CONFIG_SOC_GRX500)
int32_t ppa_update_bridge_info(struct netif_info *ifInfo, PPA_NETIF *dev)
{
	PPA_BR_PORT_INFO port_info = {0};

	if (dev == NULL || ifInfo == NULL)
		return PPA_FAILURE;

	if ((ifInfo->flags & NETIF_BRIDGE))
		ppa_add_bridge_class_rule(ifInfo, dev);
	else if (ppa_get_fid(ppa_get_netif_name(ifInfo->brif),
				&ifInfo->fid) == PPA_SUCCESS) {

		port_info.brid = ifInfo->fid;
		port_info.port = ifInfo->phys_port;
		if ((ifInfo->flags & NETIF_DIRECTPATH)
			|| (ifInfo->flags & NETIF_DIRECTCONNECT_WIFI)) {
			port_info.subif_en = 1;
			port_info.subif = ifInfo->subif_id;
			if (ppa_drv_add_br_port(&port_info, 0) == PPA_SUCCESS)
				ifInfo->fid_index = port_info.index;
		} else if (ifInfo->fid || ifInfo->inner_vid) {
			port_info.vid = ifInfo->inner_vid;
			if (ppa_drv_add_br_port(&port_info, 0) == PPA_SUCCESS)
				ifInfo->fid_index = port_info.index;
		}
	}
	return PPA_SUCCESS;
}
#endif /* CONFIG_SOC_GRX500 */

int32_t ppa_update_iface_info(PPA_NETIF *dev,
		struct netif_info *ifInfo)
{
	PPA_VCC *vcc;
	uint8_t is_IPv4Gre = 0, is_greTap = 0;
	struct netif_info *lower_if_info = NULL;
	int32_t dslwan_qid;
	int32_t ret = PPA_SUCCESS;

	if (dev == NULL || ifInfo == NULL)
		return PPA_FAILURE;

	switch(dev->type)
	{
		case ARPHRD_SIT:
		case ARPHRD_TUNNEL:
		case ARPHRD_TUNNEL6:
			ifInfo->flags |= NETIF_PHY_TUNNEL;
			break;
		case ARPHRD_ETHER:
			ifInfo->flags |= NETIF_PHY_ETH;
			break;
		case ARPHRD_IPGRE:
			is_IPv4Gre = 1;
		case ARPHRD_IP6GRE:
			ifInfo->flags |= NETIF_PHY_TUNNEL | NETIF_GRE_TUNNEL;
			break;
		case ARPHRD_PPP:
			if (dev->flags & IFF_POINTOPOINT)
				return ppa_update_ppp_if(dev, ifInfo);
			else
				return PPA_FAILURE;
	}

#if IS_ENABLED(CONFIG_L2NAT)
	if (ppa_check_if_netif_l2nat_fn &&
			ppa_check_if_netif_l2nat_fn(dev, NULL, 0))
		ifInfo->flags |= NETIF_L2NAT;
#endif
#if IS_ENABLED(CONFIG_PPA_API_DIRECTPATH)
	if (ppa_check_if_netif_directpath_fn &&
			ppa_check_if_netif_directpath_fn(dev, 0) == PPA_SUCCESS)
		ifInfo->flags |= NETIF_DIRECTPATH;
#endif

	if (ppa_if_is_br_if(dev, NULL)) {
		ifInfo->flags |= NETIF_BRIDGE;
		ifInfo->flags &= ~NETIF_PHY_ETH;
#if IS_ENABLED(CONFIG_SOC_GRX500)
		ppa_update_bridge_info(ifInfo, dev);
#endif
	}
	else if (ppa_if_is_br2684(dev, ifInfo->name) &&
			ppa_br2684_get_vcc(dev, &vcc) == PPA_SUCCESS ) {

		dslwan_qid = ppa_drv_get_dslwan_qid_with_vcc(vcc);
		if (dslwan_qid >= 0) {
			ifInfo->flags &= ~NETIF_PHY_ETH;
			ifInfo->flags |= NETIF_PHY_ATM | NETIF_BR2684 | NETIF_EOA;
			ifInfo->vcc = vcc;
			ifInfo->dslwan_qid = dslwan_qid;
		}
	}
	else if (ppa_if_is_vlan_if(dev, NULL)) {
		ifInfo->flags |= NETIF_VLAN;
		if (ifInfo->vlan_layer < PPA_MAX_VLANS ) {
			ifInfo->vlan_stack[ifInfo->vlan_layer] = dev;
		}
		ifInfo->vlan_layer++;
	}
	else if( ppa_is_ipv4_gretap_fn && ppa_is_ipv4_gretap_fn(dev) ) {
		ifInfo->flags |= NETIF_GRE_TUNNEL;
		is_IPv4Gre = 1;
		is_greTap =1;
	}
	else if( (ppa_is_ipv6_gretap_fn && ppa_is_ipv6_gretap_fn(dev)) ) {
		ifInfo->flags |= NETIF_GRE_TUNNEL;
		is_greTap =1;
	}
	else if (ppa_if_is_veth_if(dev, NULL)) {
		ifInfo->flags |= NETIF_VETH;
		if(!(ifInfo->flags & NETIF_CONTAINER_VETH_UP))
			ifInfo->flags |= NETIF_CONTAINER_VETH_UP;
	}
#if IS_ENABLED(CONFIG_SOC_GRX500)
	else if (ppa_is_vxlan_netif(dev)) {
		ifInfo->flags |= NETIF_VXLAN_TUNNEL | NETIF_PHY_TUNNEL;
	}
#endif
	if (ifInfo->flags & NETIF_GRE_TUNNEL) {
		/* << SOC specific */
		if (is_IPv4Gre)
			ifInfo->greInfo.flowId = (is_greTap) ?
				FLOWID_IPV4_EoGRE : FLOWID_IPV4GRE;
		else
			ifInfo->greInfo.flowId = (is_greTap) ?
				FLOWID_IPV6_EoGRE : FLOWID_IPV6GRE;
		/* >> END of SOC specific */
		ppa_get_gre_hdrlen(ifInfo->netif, &ifInfo->greInfo.tnl_hdrlen);
		ifInfo->flags |= NETIF_PHY_TUNNEL;
	}

	if (ifInfo->flags & NETIF_PHY_TUNNEL) {
		if (ppa_get_netif_info(ifInfo->lower_ifname, &lower_if_info) == PPA_SUCCESS) {
			ifInfo->pppoe_session_id = lower_if_info->pppoe_session_id;
			ppa_netif_put(lower_if_info);
		}
	}
	return ret;
}

int32_t ppa_update_vlan(struct netif_info *ifInfo, dp_subif_t *dp_subif)
{
	struct phys_port_info *phys_pinfo = NULL;
	int maxVlans = 0;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	struct module *owner = NULL;
	PPA_OUT_VLAN_INFO vlan_info = {0};
	PPA_NETIF *dev = NULL;
#endif

	if (ifInfo->vlan_layer == 0) {
#if IS_ENABLED(CONFIG_SOC_GRX500) && !IS_ENABLED(CONFIG_PRX300_CQM)
		/* Egress VLAN settings for WiFi VAP (non-zero subif) */
		if ((ifInfo->flags & NETIF_DIRECTCONNECT_WIFI) &&
		    (ifInfo->flags & NETIF_PHYS_PORT_GOT) &&
		    (((ifInfo->subif_id & 0xF00) >> 8) > 0)) {
			vlan_info.port_id = ifInfo->phys_port;
			vlan_info.subif_id = ((ifInfo->subif_id & 0xF00) >> 8);
			vlan_info.vlan_type = PPA_VLAN_PROTO_8021Q;
			vlan_info.ctag_rem = 1;
			vlan_info.stag_rem = 1;

			if (ppa_hsel_add_outer_vlan_entry (&vlan_info, 0,
						PAE_HAL) != PPA_SUCCESS)
				printk(KERN_INFO"HSEL Vlan entry failed\n");
		}
#endif
		return PPA_SUCCESS; /* No VLAN */
	}

	if (ppa_phys_port_lookup(ifInfo->phys_netif_name, &phys_pinfo) == PPA_SUCCESS) {
		maxVlans = phys_pinfo->vlan;
	} else
		maxVlans = PPA_MAX_VLANS;

	if (ifInfo->flags & NETIF_PHY_ATM) {
		/* overwrite maxVlans if phys_port type detected as 1:ATM */
		if (phys_pinfo && (phys_pinfo->type == 1))
			maxVlans = g_phys_port_atm_wan_vlan;
		ifInfo->flags &= ~NETIF_PHY_ETH;
	}

	if (ifInfo->vlan_layer > maxVlans ) {
		ifInfo->flags |= NETIF_VLAN_CANT_SUPPORT;
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				  "vlan layers:[%d] greater than maximum vlan layers:[%d]\n",
				  ifInfo->vlan_layer, maxVlans);
		return PPA_FAILURE;
	}

	if (ifInfo->vlan_layer == 2) {
		ifInfo->inner_vid = ppa_get_vlan_id(ifInfo->vlan_stack[0]);
		ifInfo->outer_vid = DEFAULT_OUTER_VLAN_ID | ppa_get_vlan_id(ifInfo->vlan_stack[1]);
		ifInfo->flags |= NETIF_VLAN_INNER | NETIF_VLAN_OUTER;
		ifInfo->in_vlan_netif = ifInfo->vlan_stack[0];
		ifInfo->out_vlan_netif = ifInfo->vlan_stack[1];
	} else if (ifInfo->vlan_layer == 1 /*&& maxVlans == 2 */) {
		ifInfo->inner_vid = ppa_get_vlan_id(ifInfo->vlan_stack[0]);
		ifInfo->flags |= NETIF_VLAN_INNER;
		ifInfo->in_vlan_netif = ifInfo->vlan_stack[0];
	}

#if IS_ENABLED(CONFIG_SOC_GRX500)
	dp_subif->port_id = ifInfo->phys_port;

	if ((ifInfo->flags & NETIF_VLAN_INNER) &&
		(ifInfo->in_vlan_netif)) {
		dev = ifInfo->in_vlan_netif;
	}
	else if ((ifInfo->flags & NETIF_VLAN_OUTER) &&
		 (ifInfo->out_vlan_netif)) {
		dev = ifInfo->out_vlan_netif;
	}
	else
		return PPA_SUCCESS;

	if (ifInfo->flags & NETIF_DIRECTCONNECT_WIFI)
		return PPA_SUCCESS;

#if IS_ENABLED(CONFIG_PRX300_CQM)
	/* dp registration is not needed for PRX300 */
	return PPA_SUCCESS;
#endif /* CONFIG_PRX300_CQM */

	if ( (dp_get_netif_subifid(dev, NULL, NULL, NULL, dp_subif,
					DP_F_SUBIF_LOGICAL)) == PPA_SUCCESS) {
		ifInfo->subif_id |= (dp_subif->subif) & 0xF00;
	}
	else {
		dp_subif->subif = -1;

		owner = dp_get_module_owner(ifInfo->phys_port);
		if (dp_register_subif(owner, dev,
					ppa_get_netif_name(dev), dp_subif,
					DP_F_ALLOC_EXPLICIT_SUBIFID | DP_F_SUBIF_LOGICAL
					) != PPA_SUCCESS) {
			ifInfo->flags &= ~NETIF_PHYS_PORT_GOT;
			ifInfo->phys_port = 0;

			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				  "dp_register_subif failed for : [%s]\n",
				  ppa_get_netif_name(dev));
			return PPA_FAILURE;
		}
		ifInfo->subif_id |= (dp_subif->subif) & 0xF00;
		vlan_info.port_id = ifInfo->phys_port;
		vlan_info.subif_id = ((ifInfo->subif_id & 0xF00) >> 8);
		vlan_info.vlan_type = ppa_get_vlan_type(ifInfo->in_vlan_netif);

		if (ifInfo->flags & NETIF_VLAN_OUTER) {
			vlan_info.stag_vlan_id = ifInfo->outer_vid;
			vlan_info.stag_ins = 1;
			vlan_info.stag_rem = 1;
		}
		/* in case of single VLAN tagged interface we need to
		 * check the VLAN type and
		 * 1. IF the VLAN type is 0x8100; remove the incoming
		 * ctag vlan and add the new vlan in ctag
		 * 2. IF the VLAN type is 0x88A8; preserve the incoming
		 * ctag and add stag
		 */
		if (ifInfo->flags & NETIF_VLAN_INNER) {
			if (vlan_info.vlan_type == PPA_VLAN_PROTO_8021Q) {
				vlan_info.vlan_id = ifInfo->inner_vid;
				vlan_info.ctag_ins = 1;
				vlan_info.ctag_rem = 1;
				vlan_info.stag_rem = 1;
			}
			else if (vlan_info.vlan_type == PPA_VLAN_PROTO_8021AD) {
				vlan_info.stag_vlan_id = ifInfo->inner_vid;
				vlan_info.stag_ins = 1;
			}
		}

		if (ppa_hsel_add_outer_vlan_entry (&vlan_info, 0,
					PAE_HAL) != PPA_SUCCESS)
			printk(KERN_INFO"HSEL Vlan entry failed\n");

		/*
		 * Update cvlan settings of LAN interfaces for the bridge WAN interface
		 * with VLAN type 0x88A8
		 */
		ppa_reset_lan_cvlan_remove_action(ifInfo);
	}
#endif /* CONFIG_SOC_GRX500 */

	return PPA_SUCCESS;
}

int32_t ppa_update_lower_if(PPA_NETIF *dev, PPA_NETIF *lowerdev)
{
	struct netif_info *lowerIf;
	if (lowerdev == NULL)
		return PPA_FAILURE;

	if( ppa_get_netif_info(lowerdev->name, &lowerIf) == PPA_SUCCESS ) {
		ppa_netif_remove_item(lowerdev->name, NULL);
		ppa_add_subif(lowerIf, dev->name);
		__ppa_netif_add_item(lowerIf);
		ppa_netif_put(lowerIf);
	}

	return PPA_SUCCESS;
}

int32_t ppa_netif_add(PPA_IFNAME *ifname, int f_is_lan,
		struct netif_info **pp_ifinfo, PPA_IFNAME *ifname_lower,
		int force_wanitf)
{
	struct netif_info *ifInfo = NULL;
	PPA_NETIF *dev, *lowerIf = NULL, *prev_lowerIf = NULL;
	PPA_NETIF *dp_netif;
	dp_subif_t *dp_subif = NULL;
	struct net *net = NULL;
	struct phys_port_info *p_phys_port = NULL;
	uint8_t newIf = 0;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	uint8_t eth_lan_wan = 0;
#endif

	PPA_ALLOC(dp_subif_t, dp_subif);
	ppa_memset(dp_subif, 0, sizeof(dp_subif_t));
	net = ppa_get_current_net_ns();
	dev = ppa_get_netif_by_net(net, ifname);

	if (ppa_get_netif_info(ifname, &ifInfo) == PPA_SUCCESS) {
		/*
		 * Cases where this block hit. All cases this ifname exist in
		 * PPA database.
		 *  case-1: Interface was DOWN and now UP
		 *  case-2: Interface was added ad startup and now it is created
		 *  	    or UP
		 *  Update this current interface if not updated previously or
		 *  lower interface that is passed is not same as stored in
		 *  database.
		 */
		if(ifInfo->flags & NETIF_UPDATED) {
			goto DEV_UPDATE_DONE;
		}
		if (ifname_lower && strnlen(ifname_lower, PPA_IF_NAME_SIZE)) {
			ppa_strncpy(ifInfo->lower_ifname,
					ifname_lower,
					sizeof(ifInfo->lower_ifname)-1);
		}
		if (dev == NULL)
			goto DEV_UPDATE_DONE;
	} else {
		ifInfo = ppa_iface_init_alloc(dev, ifname_lower, ifname, net);
		if (ifInfo == NULL)
			goto DEV_ADD_ERROR;
		newIf = 1;

		if (dev == NULL) {
			ifInfo->flags &= ~(NETIF_LAN_IF | NETIF_WAN_IF);
			ifInfo->flags |= f_is_lan ? NETIF_LAN_IF : NETIF_WAN_IF;
			ifInfo->flags |= NETIF_PHY_IF_GOT;
			ifInfo->f_wanitf.flag_root_itf = 1;
			ppa_strncpy(ifInfo->phys_netif_name, ifname,
					PPA_IF_NAME_SIZE);
			goto DEV_UPDATE_DONE;
		}
	}
	if (ifInfo->flags & NETIF_DIRECTCONNECT_WIFI) {
		goto DEV_UPDATE_DONE;
	}

	if (dev && ppa_get_netif_hwaddr(dev, ifInfo->mac, 1) == PPA_SUCCESS &&
			is_valid_ether_addr(ifInfo->mac)) {
		ifInfo->flags |= NETIF_MAC_AVAILABLE;
	}

	if (ppa_update_iface_info(dev, ifInfo) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR,
			"Updation of interface info failed for:[%s]\n", ifname);
		goto DEV_ADD_ERROR;
	}

	if (strnlen(ifInfo->lower_ifname, PPA_IF_NAME_SIZE) > 0) {
		lowerIf = ppa_get_netif_by_net(net, ifInfo->lower_ifname);
	}

	prev_lowerIf = dev;

	if (!(ifInfo->flags & NETIF_BRIDGE)) {
		if (lowerIf == NULL) {
			lowerIf = ppa_get_lower_dev(dev, net);
		}

		while (lowerIf != NULL) {
			ppa_update_lower_if(prev_lowerIf, lowerIf);
			if (ppa_add_iface_on_lower_exists(lowerIf, ifInfo) == PPA_SUCCESS) {
				goto DEV_ADDED;
			}

			if (ppa_update_iface_info(lowerIf, ifInfo) != PPA_SUCCESS) {
				ppa_debug(DBG_ENABLE_MASK_ERR,
					"Updation of interface info failed for:[%s]\n", ifname);
				goto DEV_ADD_ERROR;
			}

			prev_lowerIf = lowerIf;
			lowerIf = ppa_get_lower_dev(lowerIf, net);
		}
	}

	ppa_strncpy(ifInfo->phys_netif_name, prev_lowerIf->name,
				sizeof(prev_lowerIf->name));

	ifInfo->flags |= NETIF_PHY_IF_GOT;

	/*Is it a root interface */
	if (ppa_memcmp(ifInfo->phys_netif_name, ifname, strnlen(ifname, PPA_IF_NAME_SIZE)) == 0)
		ifInfo->f_wanitf.flag_root_itf = 1;

	dp_netif = prev_lowerIf;
	if (ifInfo->flags & NETIF_PPPOATM)
		dp_netif = NULL;
	if ((dp_get_netif_subifid(dp_netif, NULL, ifInfo->vcc, NULL,
					dp_subif, 0)) == PPA_SUCCESS) {

		/* ATM bonding interface, for which dp_subif.peripheral_pvt to be set
		 * don't update physport and thus disable hardware acceleration
		 */
		if (ppa_if_is_br2684(prev_lowerIf, NULL) &&
		    (dp_subif->peripheral_pvt == 1))
			goto DEV_ADDED;

		ifInfo->flags |= NETIF_PHYS_PORT_GOT;
		ifInfo->phys_port = dp_subif->port_id;
		ifInfo->subif_id = dp_subif->subif;
		if ((ifInfo->flags & NETIF_PHY_IF_GOT)
			&& !(ifInfo->flags & NETIF_BRIDGE)) {

			/* Check whether the netif is directconnect wifi
			 * interface or not */
			if ((dp_subif->alloc_flag & DP_F_FAST_WLAN) &&
					!(dp_subif->alloc_flag & DP_F_FAST_DSL)) {
				ifInfo->flags |= NETIF_DIRECTCONNECT_WIFI;
			}
			else if ((dp_subif->alloc_flag & DP_F_VUNI) &&
					(dp_subif->data_flag & DP_SUBIF_VANI)) {
				ifInfo->flags |= NETIF_VANI;
			}
#if IS_ENABLED(CONFIG_SOC_GRX500)
			else if ((dp_subif->alloc_flag & DP_F_FAST_ETH_LAN) ||
					(dp_subif->alloc_flag & DP_F_FAST_ETH_WAN)) {
				ppa_drv_mpe_set_checksum_queue_map(dp_subif->port_id, NETIF_PMAC_NEEDED | NETIF_LAN_WAN);
				eth_lan_wan = 1;
			}
#endif

			if (ppa_phys_port_lookup(ifInfo->phys_netif_name,
					&p_phys_port) != PPA_SUCCESS) {
				/* If the interface is not a part of physical interface
				 * list. Check if it is a dynamic interface
				 */
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
					  "netif=%s portid=%d subifid=%d\n",
					  dev->name,
					  dp_subif->port_id, dp_subif->subif);

				if (ppa_phys_port_add(dev->name,
							dp_subif->port_id) != PPA_SUCCESS) {
					ppa_debug(DBG_ENABLE_MASK_ERR,
						"Physical Port add failed for:[%s]\n", ifname);
					goto DEV_ADD_ERROR;
				}
			}
		}

#if IS_ENABLED(CONFIG_SOC_GRX500)
		if (ifInfo->brif)
			ppa_update_bridge_info(ifInfo, dev);
#endif
	}

DEV_ADDED:
	/* TODO: If PHY_IF_GOT & PHYS_PORT_GOT are redundant,
	 * we need to remove one of these.
	 * Only one check is required - PHYS_PORT_GOT
	 */

	/* If interface is Veth or Bridge, there will be no physical interface
	 * and it should continue.
	 */
	if ( !(ifInfo->flags & NETIF_PHYS_PORT_GOT) &&
			((ifInfo->flags & NETIF_DIRECTPATH) ||
			(ifInfo->flags & NETIF_DIRECTCONNECT_WIFI))) {
		ppa_debug(DBG_ENABLE_MASK_ERR,
				"No subif found for : %s", ifInfo->name);
		goto DEV_ADD_ERROR;
	}

#if IS_ENABLED(CONFIG_PPA_EXT_PKT_LEARNING)
	if (ifInfo->f_wanitf.flag_root_itf && !(ifInfo->flags & NETIF_PHY_TUNNEL) &&
			!(ifInfo->flags & NETIF_BRIDGE)) {
		if (ppa_get_netif(ifInfo->phys_netif_name)) {
			if (pktprs_dev_add(ppa_get_netif(ifInfo->phys_netif_name))) {
				ppa_debug(DBG_ENABLE_MASK_ERR,
					"Adding [%s] to parsing driver failed\n",
					ifInfo->phys_netif_name);
				goto DEV_ADD_ERROR;
			}
		}
		else {
			ppa_debug(DBG_ENABLE_MASK_ERR,
				"Parsng add failed\n");
			goto DEV_ADD_ERROR;
		}
	}
#endif

	if (ppa_update_vlan(ifInfo, dp_subif) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"Updation of VLAN info failed for: [%s]\n", ifInfo->name);
		goto DEV_UPDATE_DONE;
	}

	/* Interface is success therefore, this point onwards
	 * no failure should be returned.
	 */
	if (force_wanitf && ifInfo->f_wanitf.flag_root_itf) {
		/* since it is physical root interface and the
		 * force_wanitf flag is set, then try to set wantif
		 * value accordingly
		 */
		PPA_WANITF_CFG wanitf_cfg = {0};
		ifInfo->f_wanitf.flag_force_wanitf = force_wanitf;
		wanitf_cfg.lan_flag = f_is_lan ? 1 : 0;
		wanitf_cfg.physical_port = ifInfo->phys_port;
		if (ppa_set_wan_itf(&wanitf_cfg, 0) == PPA_SUCCESS &&
			(!ifInfo->f_wanitf.flag_already_wanitf)) {
			ifInfo->f_wanitf.flag_already_wanitf = 1;
			ifInfo->f_wanitf.old_lan_flag = wanitf_cfg.old_lan_flag;
		} else {
			ppa_debug(DBG_ENABLE_MASK_ERR,
				  "ppa_set_wan_itf fail\n");
		}
	}
	ifInfo->flags &= ~(NETIF_LAN_IF | NETIF_WAN_IF);
	ifInfo->flags |= f_is_lan ? NETIF_LAN_IF : NETIF_WAN_IF;

	if (ppa_check_is_ifup(dev)) {
		ifInfo->enable = true;
		ppa_netif_up_stats(ifInfo, dev);
	} else {
		ifInfo->enable = false;
		ppa_netif_down_stats(ifInfo, dev);
	}

#if defined(CONFIG_PPA_PUMA7) && defined(CONFIG_TI_HIL_PROFILE_INTRUSIVE_P7)
	if (!(ifInfo->flags & NETIF_PHY_ATM) &&
			!(ppa_if_is_br2684(dev, ifInfo->name))) {
		ppa_netdev_pp_prepare_vpid(ifInfo, f_is_lan);
	}
#elif IS_ENABLED(CONFIG_MCAST_HELPER)
	if (f_is_lan) {
		mcast_helper_register_module(dev,
			THIS_MODULE,
			NULL,
			mcast_module_config,
			NULL,
			MC_F_REGISTER | MC_F_DIRECTPATH);
	}
#endif

#if IS_ENABLED(CONFIG_SOC_GRX500)
	if ( (ifInfo->flags & NETIF_WAN_IF)
			&& (ifInfo->f_wanitf.flag_root_itf == 1 && eth_lan_wan == 0)
			&& !(ifInfo->flags & NETIF_VANI) ) {
		ppa_drv_tmu_set_checksum_queue_map(ifInfo->phys_port);
		if ( ppa_phys_port_is_fcs_required(ifInfo->phys_port) ) {
			ppa_drv_mpe_set_checksum_queue_map(ifInfo->phys_port, NETIF_PMAC_NEEDED);
			ifInfo->flags |= NETIF_FCS;
		} else {
			ppa_drv_mpe_set_checksum_queue_map(ifInfo->phys_port, 0);
		}
	}
	else if (ifInfo->flags & NETIF_DIRECTCONNECT_WIFI) {
		printk("Setting genconf port infor wifi port:%d\n",ifInfo->phys_port);
		ppa_drv_mpe_set_checksum_queue_map(ifInfo->phys_port, NETIF_VAP_QOS);
	}

	if (!strcmp(ifname, "lite0") && (ifInfo->phys_port != PPA_PORT_CPU0)) {
		ppa_drv_tmu_set_lro_queue_map(ifInfo->phys_port);
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				"Set LRO queuemap for ifname=[%s]\n", ifname);
	}

	if (strncmp(ifInfo->name, ifInfo->phys_netif_name, PPA_IF_NAME_SIZE) != 0) {
		struct netif_info *p_phys_ifinfo = NULL;
		if (ppa_get_netif_info(ifInfo->phys_netif_name, &p_phys_ifinfo) == PPA_SUCCESS) {
			if (p_phys_ifinfo->flags & NETIF_VANI) {
				ifInfo->flags |= NETIF_VANI;
				ifInfo->subif_id = p_phys_ifinfo->subif_id;
			}
			ppa_netif_put(p_phys_ifinfo);
		}
	}
	else if (ifInfo->flags & NETIF_VANI) {
		dp_subif_t *p_dp_subif = NULL;
		p_dp_subif = (dp_subif_t *)ppa_malloc(sizeof(dp_subif_t));
		if (p_dp_subif != NULL) {
			ppa_memset(p_dp_subif, 0, sizeof(dp_subif_t));
			if ((dp_get_netif_subifid(dp_subif->associate_netif, NULL, NULL, NULL,
							p_dp_subif, 0)) == PPA_SUCCESS) {
				ifInfo->subif_id = p_dp_subif->subif;
				ppa_drv_mpe_set_checksum_queue_map(ifInfo->phys_port, NETIF_PMAC_NEEDED | NETIF_VANI_INTERFACE);
			}
			ppa_free(p_dp_subif);
		}
		else {
			ppa_debug(DBG_ENABLE_MASK_ERR,
					"PON subif not updated - Memory allocation Failed\n");
		}
	}
#endif /* CONFIG_SOC_GRX500 */

	ifInfo->flags |= NETIF_UPDATED;
DEV_UPDATE_DONE:
	if (newIf)
		__ppa_netif_add_item(ifInfo);

	printk(KERN_INFO"%s Success for ifname=%s (netif=%px)\n",
				__FUNCTION__, ifname, ifInfo->netif);
	ppa_netif_put(ifInfo);
	ppa_free(dp_subif);
	return PPA_SUCCESS;

DEV_ADD_ERROR:
	printk(KERN_INFO"%s Error adding Interface:[%s]\n",
			__FUNCTION__, ifname);
	if (ifInfo != NULL)
		ppa_netif_free_item(ifInfo);
	ppa_free(dp_subif);
	return PPA_FAILURE;
}

void ppa_netif_remove(PPA_IFNAME *ifname, int f_is_lan)
{
	struct netif_info *p_ifinfo;
#if IS_ENABLED(CONFIG_SOC_GRX500)
#if !IS_ENABLED(CONFIG_PRX300_CQM)
	PPA_OUT_VLAN_INFO vlan_info = {0};
#endif /* CONFIG_PRX300_CQM */
	PPA_CLASS_RULE *class_rule = NULL;
	PPA_BR_PORT_INFO port_info = {0};
#endif /* CONFIG_SOC_GRX500 */

#if IS_ENABLED(CONFIG_PPA_EXT_PKT_LEARNING)
	PPA_NETIF *netif_tmp = NULL;
#endif /*CONFIG_PPA_EXT_PKT_LEARNING*/

#if defined(PPA_IF_MIB) && PPA_IF_MIB
	struct netif_info *lower_if_info = NULL;
#endif /* PPA_IF_MIB */

	if (ppa_get_netif_info(ifname, &p_ifinfo) == PPA_SUCCESS) {
		p_ifinfo->flags &= f_is_lan ? ~NETIF_LAN_IF : ~NETIF_WAN_IF;
		if (p_ifinfo->flags & (NETIF_LAN_IF | NETIF_WAN_IF)) {
			ppa_netif_free_item(p_ifinfo);
			return;
		}
#if IS_ENABLED(CONFIG_SOC_GRX500)
		if (p_ifinfo->fid || p_ifinfo->inner_vid ||
			(p_ifinfo->flags & NETIF_DIRECTPATH) ||
			(p_ifinfo->flags & NETIF_DIRECTCONNECT_WIFI)) {
			port_info.brid = p_ifinfo->fid;
			port_info.port = p_ifinfo->phys_port;
			port_info.index =  p_ifinfo->fid_index;
			ppa_drv_del_br_port(&port_info, 0);
		}

		if (p_ifinfo->flags & NETIF_BRIDGE) {
			class_rule = (PPA_CLASS_RULE  *)ppa_malloc(sizeof(PPA_CLASS_RULE));
			if (class_rule == NULL){
				ppa_debug(DBG_ENABLE_MASK_ERR,
					"ppa_netif_remove : Memory allocation Failed\n");
				return;
			}
			ppa_memset(class_rule, 0, sizeof(PPA_CLASS_RULE));

			class_rule->in_dev = GSWR_INGRESS;
			class_rule->category = CAT_MGMT;
			class_rule->uidx = p_ifinfo->fid_index;

			ppa_del_class_rule(class_rule);
			ppa_free(class_rule);
		}

#if !IS_ENABLED(CONFIG_PRX300_CQM)
	/* dp de-registration is not needed for PRX300 */
		/* Specially, skip any subif de-registration for WiFi VLAN interface.*/
		if (((p_ifinfo->flags & NETIF_VLAN_INNER) ||
		    (p_ifinfo->flags & NETIF_VLAN_OUTER)) &&
		    !(p_ifinfo->flags & NETIF_DIRECTCONNECT_WIFI)) {
			if ((p_ifinfo->netif &&
			     ppa_if_is_vlan_if(p_ifinfo->netif, NULL)) &&
			    (p_ifinfo->flags & NETIF_PHYS_PORT_GOT)) {
				struct module *owner = NULL;
				dp_subif_t *dp_subif;

				vlan_info.port_id = p_ifinfo->phys_port;
				vlan_info.subif_id = ((p_ifinfo->subif_id & 0xF00) >> 8);
				if (ppa_hsel_del_outer_vlan_entry(&vlan_info, 0, PAE_HAL) != PPA_SUCCESS)
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "hsel_del_outer_vlan_entry fail\n");

				/* Not the mac vlan interfaces */
				owner = dp_get_module_owner(p_ifinfo->phys_port);

				dp_subif = ppa_malloc(sizeof(dp_subif_t));
				if (!dp_subif)
					ppa_debug(DBG_ENABLE_MASK_ERR,
						"[%s:%d] DP subif allocation failed\n",
						__FUNCTION__, __LINE__);
				else {
					ppa_memset(dp_subif, 0, sizeof(dp_subif_t));
					dp_subif->port_id = p_ifinfo->phys_port;
					dp_subif->subif = p_ifinfo->subif_id;
					if (dp_register_subif(owner, p_ifinfo->netif,
					    p_ifinfo->name, dp_subif,
					    DP_F_DEREGISTER) != DP_SUCCESS)
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
							"dp_register_subif deregister failed\n");
					else
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
							"%s unregistered!!\n",
							p_ifinfo->name);
					ppa_free(dp_subif);
				}
			}
		}
		else if (!(p_ifinfo->flags & NETIF_VLAN)) {
			/* Egress VLAN settings cleanup for WiFi VAP (non-zero subif) */
			if ((p_ifinfo->flags & NETIF_DIRECTCONNECT_WIFI) &&
			    (p_ifinfo->flags & NETIF_PHYS_PORT_GOT) &&
			    (((p_ifinfo->subif_id & 0xF00) >> 8) > 0)) {
				vlan_info.port_id = p_ifinfo->phys_port;
				vlan_info.subif_id = ((p_ifinfo->subif_id & 0xF00) >> 8);

				if (ppa_hsel_del_outer_vlan_entry(&vlan_info, 0, PAE_HAL) != PPA_SUCCESS)
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "hsel_del_outer_vlan_entry fail\n");
			}
		}
#endif /* CONFIG_PRX300_CQM */
#endif /* CONFIG_SOC_GRX500 */

#if IS_ENABLED(CONFIG_PPA_EXT_PKT_LEARNING)
		if (p_ifinfo->f_wanitf.flag_root_itf && !(p_ifinfo->flags & NETIF_PHY_TUNNEL) &&
			!(p_ifinfo->flags & NETIF_BRIDGE)) {
			printk(KERN_INFO"Removing %s from parsing driver\n",p_ifinfo->phys_netif_name);
			netif_tmp = ppa_get_netif(p_ifinfo->phys_netif_name);
			if (netif_tmp) {
				if (pktprs_dev_remove(netif_tmp)) {
					printk(KERN_INFO"Removing %s from parsing driver failed!!\n",
						p_ifinfo->phys_netif_name);
				}
			} else {
				printk(KERN_INFO"Get %s netif failed!!\n",
					p_ifinfo->phys_netif_name);
			}
		}
#endif /* CONFIG_PPA_EXT_PKT_LEARNING */

#if defined(CONFIG_PPA_PUMA7) && defined(CONFIG_TI_HIL_PROFILE_INTRUSIVE_P7)
		if (!(p_ifinfo->flags & NETIF_PHY_ATM) &&
				!(ppa_if_is_br2684(p_ifinfo->netif, p_ifinfo->name))) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
					" %s:%d : removing vpid for the interface %s\n",
					__func__, __LINE__, p_ifinfo->name);
			ppa_netdev_pp_remove_vpid(p_ifinfo);
		}
#endif

		/* Add upper iface mib to lower iface */
#if defined(PPA_IF_MIB) && PPA_IF_MIB
		ppa_get_netif_info(p_ifinfo->lower_ifname, &lower_if_info);
		if (lower_if_info != NULL) {
			lower_if_info->hw_accel_stats.rx_bytes += p_ifinfo->hw_accel_stats.rx_bytes;
			lower_if_info->hw_accel_stats.tx_bytes += p_ifinfo->hw_accel_stats.tx_bytes;
#if IS_ENABLED(CONFIG_IPV4_IPV6_COUNTER_SUPPORT)
			/* Update only HW accel statistics here */
			lower_if_info->hw_accel_stats.rx_pkts += p_ifinfo->hw_accel_stats.rx_pkts;
			lower_if_info->hw_accel_stats.tx_pkts += p_ifinfo->hw_accel_stats.tx_pkts;
			lower_if_info->hw_accel_stats.rx_pkts_ipv4 += p_ifinfo->hw_accel_stats.rx_pkts_ipv4;
			lower_if_info->hw_accel_stats.tx_pkts_ipv4 += p_ifinfo->hw_accel_stats.tx_pkts_ipv4;
			lower_if_info->hw_accel_stats.rx_pkts_ipv6 += p_ifinfo->hw_accel_stats.rx_pkts_ipv6;
			lower_if_info->hw_accel_stats.tx_pkts_ipv6 += p_ifinfo->hw_accel_stats.tx_pkts_ipv6;
#endif /* CONFIG_IPV4_IPV6_COUNTER_SUPPORT */
			ppa_netif_remove_item(lower_if_info->name, NULL);
			ppa_remove_subif(lower_if_info, ifname);
			__ppa_netif_add_item(lower_if_info);
			ppa_netif_put(lower_if_info);
		}
#endif /* PPA_IF_MIB */

		ppa_netif_remove_item(ifname, NULL);

#if IS_ENABLED(CONFIG_MCAST_HELPER)
#ifndef CONFIG_PPA_PUMA7
		if (f_is_lan && p_ifinfo->netif) {
			mcast_helper_register_module(p_ifinfo->netif,
							 THIS_MODULE,
							 NULL,
							 mcast_module_config,
							 NULL,
							 MC_F_DEREGISTER | MC_F_DIRECTPATH);
		}
#endif
#endif /* CONFIG_MCAST_HELPER */
		printk(KERN_INFO"%s Success for ifname=%s (netif=%px)\n",
				__FUNCTION__, ifname, p_ifinfo->netif);
		ppa_netif_put(p_ifinfo);
	}
}

/* Returns netif when it is available in lookup table
 * Should be used in the configuration path to get netif_info.
 */
int32_t ppa_get_netif_info(const PPA_IFNAME *ifname,
			   struct netif_info **pp_info)
{
	int32_t ret = PPA_ENOTAVAIL;
	struct netif_info *p;

	ppa_netif_lock_list();
	for (p = g_netif; p; p = p->next) {
		if (strncmp(p->name, ifname, PPA_IF_NAME_SIZE) == 0) {
			ret = PPA_SUCCESS;
			if (pp_info) {
				ppa_atomic_inc(&p->count);
				*pp_info = p;
			}
			break;
		}
	}
	ppa_netif_unlock_list();

	return ret;
}

int32_t __ppa_netif_lookup(PPA_IFNAME *ifname, struct netif_info **pp_info)
{
	int32_t ret = PPA_ENOTAVAIL;
	struct netif_info *p;

	for (p = g_netif; p; p = p->next) {
		if (strncmp(p->name, ifname, PPA_IF_NAME_SIZE) == 0 && p->enable == true) {
			ret = PPA_SUCCESS;
			if (pp_info) {
				ppa_atomic_inc(&p->count);
				*pp_info = p;
			}
			break;
		}
	}
	return ret;
}

#if IS_ENABLED(CONFIG_PPA_EXT_PKT_LEARNING)
PPA_NETIF *ppa_logical_netif_from_pktprs(struct pktprs_hdr *h, uint32_t dir,
					 PPA_NETIF *base_if,
					 struct netif_info **ifinfo)
{
	PPA_NETIF *netif=base_if;
	struct netif_info *p = NULL;
	uint8_t pkt_vlan, vlan_match, pkt_pppoe, pppoe_match;
	uint8_t pkt_dslite, dslite_match, pkt_6rd, sixrd_match;
	uint8_t pkt_l2tpv2, l2tp_match, pkt_gre, gre_match;
	uint8_t if_macvlan, macvlan_match;
	__be16 sid, tid;

	pkt_vlan = PKTPRS_IS_VLAN0(h, PKTPRS_HDR_LEVEL0) ? 1:0;
	pkt_pppoe = PKTPRS_IS_PPPOE(h, PKTPRS_HDR_LEVEL0) ? 1:0;
	pkt_l2tpv2 = pktprs_is_l2tpv2(h, PKTPRS_HDR_LEVEL0) ? 1:0;
	pkt_dslite = PKTPRS_IS_DSLITE(h) ? 1:0;
	pkt_6rd = PKTPRS_IS_SIXRD(h) ? 1:0;
	pkt_gre = PKTPRS_IS_GRE(h, PKTPRS_HDR_LEVEL0) ? 1:0;

	ppa_netif_lock_list();
	/*for each entry in the netif list*/
	for (p = g_netif; p; p = p->next) {
		vlan_match=pppoe_match=dslite_match=sixrd_match=0;
		l2tp_match=gre_match=if_macvlan=macvlan_match=0;
		/*compare physical netif name*/
		if (strncmp(p->phys_netif_name, base_if->name,
				PPA_IF_NAME_SIZE) == 0 && p->enable == true) {
			if(strncmp(p->name, base_if->name, PPA_IF_NAME_SIZE) == 0) {
				/* skip the base netdevice */
				continue;
			}
			/* if the interfae is a vlan interface */
			if( (p->flags & NETIF_VLAN) && (pkt_vlan)) {
				if((p->flags & NETIF_VLAN_OUTER)) {
					/* stacked vlan */
					if(PKTPRS_IS_VLAN1(h, PKTPRS_HDR_LEVEL0)) {
						if(!((pktprs_vlan_hdr(h, PKTPRS_HDR_LEVEL0, 1)->h_vlan_TCI
							== p->inner_vid) &&
							(pktprs_vlan_hdr(h, PKTPRS_HDR_LEVEL0, 0)->h_vlan_TCI
							== p->outer_vid))) {
							/* vlans not matching */
							continue;
						} else {
							vlan_match=1;
						}
					} else {
						/* no stacked vlan in the packet */
						continue;
					}
				} else if (p->flags & NETIF_VLAN_INNER) {
					if(pkt_vlan) {
						if(!(pktprs_vlan_hdr(h, PKTPRS_HDR_LEVEL0, 0)->h_vlan_TCI
							== p->inner_vid)) {
							/* vlans not matching */
							continue;
						} else {
							vlan_match=1;
						}
					} else {
						/* no vlan in the packet */
						continue;
					}
				}
			}

			/* if the interface is pppoe */
			if( (p->flags & NETIF_PPPOE) && (pkt_pppoe)) {
				if(!(pktprs_pppoe_hdr(h, PKTPRS_HDR_LEVEL0)->sid == p->pppoe_session_id)){
					/*pppoe session not matching */
					continue;
				} else {
					pppoe_match=1;
				}
			}
#if defined(L2TP_CONFIG) && L2TP_CONFIG
			/* if the interface is l2tp */
			if(( p->flags & NETIF_PPPOL2TP) && (pkt_l2tpv2)) {
				if (pktprs_l2tp_oudp_hdr(h, PKTPRS_HDR_LEVEL0)->v2.flags & L2TP_HDRFLAG_L) {
					sid = pktprs_l2tp_oudp_hdr(h, PKTPRS_HDR_LEVEL0)->v2_len.sess_id;
					tid = pktprs_l2tp_oudp_hdr(h, PKTPRS_HDR_LEVEL0)->v2_len.tunnel;
				} else {
					sid = pktprs_l2tp_oudp_hdr(h, PKTPRS_HDR_LEVEL0)->v2.sess_id;
					tid = pktprs_l2tp_oudp_hdr(h, PKTPRS_HDR_LEVEL0)->v2.tunnel;
				}
				if (p->pppol2tp_session_id != sid ||
				    p->pppol2tp_tunnel_id != tid) {
					/*l2tp session not matching */
					continue;
				} else {
					l2tp_match = 1;
				}
			}
#endif /*defined(L2TP_CONFIG) && L2TP_CONFIG*/

#if IS_ENABLED(CONFIG_IPV6_TUNNEL)
			/* if the interface is dslite */
			if((ppa_netif_type(p->netif) == ARPHRD_TUNNEL6) && (pkt_dslite)) {
				if(!ppa_pkt_from_tunnelv6_netdev(p->netif,
					((dir==PKT_TX)? pktprs_ipv6_hdr(h, PKTPRS_HDR_LEVEL0)->saddr:pktprs_ipv6_hdr(h, PKTPRS_HDR_LEVEL0)->daddr),
					((dir==PKT_TX)? pktprs_ipv6_hdr(h, PKTPRS_HDR_LEVEL0)->daddr:pktprs_ipv6_hdr(h, PKTPRS_HDR_LEVEL0)->saddr))) {
					continue;
				} else {
					dslite_match = 1;
				}
			}

#endif /*IS_ENABLED(CONFIG_IPV6_TUNNEL)*/
			/* if the interface is 6rd */
			if((ppa_netif_type(p->netif) == ARPHRD_SIT) && (pkt_6rd)) {
				if(!ppa_pkt_from_tunnelv4_netdev(p->netif,
					((dir==PKT_TX)? pktprs_ipv4_hdr(h, PKTPRS_HDR_LEVEL0)->saddr:pktprs_ipv4_hdr(h, PKTPRS_HDR_LEVEL0)->daddr),
					((dir==PKT_TX)? pktprs_ipv4_hdr(h, PKTPRS_HDR_LEVEL0)->daddr:pktprs_ipv4_hdr(h, PKTPRS_HDR_LEVEL0)->saddr))) {
					continue;
				} else {
					sixrd_match = 1;
				}
			}

			/* if the interface is gre */
			if((p->flags & NETIF_GRE_TUNNEL) && (pkt_gre))  {
				if( p->netif->type == ARPHRD_IP6GRE || (ppa_is_ipv6_gretap_fn &&
						ppa_is_ipv6_gretap_fn(p->netif)) ) {
					if(!ppa_pkt_from_tunnelv6_netdev(p->netif,
						((dir==PKT_TX)? pktprs_ipv6_hdr(h, PKTPRS_HDR_LEVEL0)->saddr:pktprs_ipv6_hdr(h, PKTPRS_HDR_LEVEL0)->daddr),
						((dir==PKT_TX)? pktprs_ipv6_hdr(h, PKTPRS_HDR_LEVEL0)->daddr:pktprs_ipv6_hdr(h, PKTPRS_HDR_LEVEL0)->saddr))) {
						continue;
					} else {
						gre_match=1;
					}
				} else {
					if(!ppa_pkt_from_tunnelv4_netdev(p->netif,
						((dir==PKT_TX)? pktprs_ipv4_hdr(h, PKTPRS_HDR_LEVEL0)->saddr:pktprs_ipv4_hdr(h, PKTPRS_HDR_LEVEL0)->daddr),
						((dir==PKT_TX)? pktprs_ipv4_hdr(h, PKTPRS_HDR_LEVEL0)->daddr:pktprs_ipv4_hdr(h, PKTPRS_HDR_LEVEL0)->saddr))) {
						continue;
					} else {
						gre_match=1;
					}
				}
			}

			/* if mac vlan compare the mac address */
			if(ppa_is_macvlan_if(p->netif, NULL)) {
				if_macvlan=1;
				if(memcmp(p->mac,
					  pktprs_eth_hdr(h, PKTPRS_HDR_LEVEL0)->h_source,
					  ETH_ALEN)!=0) {
					continue;
				} else {
					macvlan_match=1;
				}
			}
			
			if(((if_macvlan==macvlan_match) && (pkt_gre==gre_match) && (pkt_l2tpv2==l2tp_match) &&
				(pkt_6rd==sixrd_match) && (pkt_dslite==dslite_match) && (pkt_vlan ==vlan_match)) || 
				((pkt_pppoe==pppoe_match) && (pkt_gre==gre_match) && (pkt_6rd==sixrd_match) && 
				(pkt_dslite==dslite_match))) {
					netif = p->netif;
					ppa_atomic_inc(&p->count);
					*ifinfo = p;
					break;
			}
		}
	}
	ppa_netif_unlock_list();
	return netif;
} 
#endif /*IS_ENABLED(CONFIG_PPA_EXT_PKT_LEARNING)*/

/* Returns netif_info only when it is up and available in lookup table.
 * Should be used in the learning path to get netif_info.
 */
int32_t ppa_netif_lookup(PPA_IFNAME *ifname, struct netif_info **pp_info)
{
	int32_t ret = PPA_FAILURE;
	if(ifname) {
		ppa_netif_lock_list();
		ret = __ppa_netif_lookup(ifname, pp_info);
		ppa_netif_unlock_list();
	}

	return ret;
}

void ppa_netif_put(struct netif_info *p_info)
{
	if (p_info)
		ppa_netif_free_item(p_info);
}

/* This is called only from netif_update to check tunnel or any other interface
 * on top of tunnel.
 */
uint32_t ppa_is_tunnel_if(PPA_NETIF *netif)
{

	if ((netif->type == ARPHRD_SIT) ||
		(netif->type == ARPHRD_TUNNEL6) ||
		(ppa_is_gre_netif(netif)) ||
#if IS_ENABLED(CONFIG_SOC_GRX500)
		(ppa_is_vxlan_netif(netif)) ||
#endif
		ppa_if_is_vlan_if(netif, NULL))
		return 1;
	else
		return 0;
}

int32_t ppa_netif_update(PPA_NETIF *netif, PPA_IFNAME *ifname)
{
	struct netif_info *p_info;
	int f_need_update = 0;
	uint32_t flags;
	uint32_t force_wantif;
	PPA_IFNAME lower_ifname[PPA_IF_NAME_SIZE];
#if defined(L2TP_CONFIG) && L2TP_CONFIG
	PPA_IFNAME phy_ifname[PPA_IF_NAME_SIZE];
	PPA_NETIF *new_netif;
#endif
	PPA_IF_STATS hw_accel_stats_tmp;
	PPA_IF_STATS sw_accel_stats_tmp;

	if (netif)
		ifname = ppa_get_netif_name(netif);
	else
		netif = ppa_get_netif(ifname);

	if (!netif || !ifname) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			  "fail: cannot find device\n");
		return PPA_EINVAL;
	}

	if (ppa_get_netif_info(ifname, &p_info) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			  "fail: device: %s not accelerated\n", ifname);
		return PPA_ENOTAVAIL;
	}

	flags = p_info->flags;

	if (!ppa_is_netif_equal(netif, p_info->netif)) {
		f_need_update = 1;
		/*If previous netif is null, dont rely on the stats to update
		 as it may result in junk values. Make all 0's in this case*/
		if(p_info->netif == NULL) {
			p_info->hw_accel_stats.rx_bytes = 0;
			p_info->hw_accel_stats.tx_bytes = 0;
			p_info->sw_accel_stats.rx_bytes = 0;
			p_info->sw_accel_stats.tx_bytes = 0;
#if IS_ENABLED(CONFIG_IPV4_IPV6_COUNTER_SUPPORT)
			p_info->hw_accel_stats.rx_pkts = 0;
			p_info->hw_accel_stats.tx_pkts = 0;
			p_info->hw_accel_stats.rx_pkts_ipv4 = 0;
			p_info->hw_accel_stats.tx_pkts_ipv4 = 0;
			p_info->hw_accel_stats.rx_pkts_ipv6 = 0;
			p_info->hw_accel_stats.tx_pkts_ipv6 = 0;

			p_info->sw_accel_stats.rx_pkts = 0;
			p_info->sw_accel_stats.tx_pkts = 0;
			p_info->sw_accel_stats.rx_pkts_ipv4 = 0;
			p_info->sw_accel_stats.tx_pkts_ipv4 = 0;
			p_info->sw_accel_stats.rx_pkts_ipv6 = 0;
			p_info->sw_accel_stats.tx_pkts_ipv6 = 0;
#endif /* CONFIG_IPV4_IPV6_COUNTER_SUPPORT */
		}
	} else if (flags & NETIF_VETH) {
		if (p_info->flags & NETIF_CONTAINER_VETH_UPDATE) {
			p_info->flags &= ~NETIF_CONTAINER_VETH_UPDATE;
			p_info->flags |= NETIF_CONTAINER_VETH_MOVE;
			f_need_update = 0;
		}
		if (p_info->flags & NETIF_CONTAINER_VETH_MOVE) {
			p_info->flags &= ~NETIF_CONTAINER_VETH_MOVE;
			p_info->flags |= NETIF_CONTAINER_VETH_UP;
			f_need_update = 1;
		}
		if (p_info->flags & NETIF_CONTAINER_VETH_UP)
			 f_need_update = 0;
	} else if ((flags & NETIF_PHYS_PORT_GOT) == 0 &&
		   (flags & (NETIF_BRIDGE | NETIF_PHY_IF_GOT)) !=
		   (NETIF_BRIDGE | NETIF_PHY_IF_GOT)) {
		f_need_update = 1;
#if defined(L2TP_CONFIG) && L2TP_CONFIG
	} else if ((flags & NETIF_PPPOL2TP)) {
		if (!ppa_check_is_pppol2tp_netif(netif)) {
			f_need_update = 1;
		} else {
			p_info->pppol2tp_session_id =
				ppa_pppol2tp_get_l2tp_session_id(netif);
			p_info->pppol2tp_tunnel_id =
				ppa_pppol2tp_get_l2tp_tunnel_id(netif);

			if (ppa_pppol2tp_get_base_netif(netif, phy_ifname)
				== PPA_SUCCESS) {
				new_netif = ppa_get_netif(phy_ifname);
				if (ppa_check_is_ppp_netif(new_netif)) {
					if (!ppa_check_is_pppoe_netif(new_netif))
						f_need_update = 1;
					else
						p_info->pppoe_session_id =
							ppa_pppoe_get_pppoe_session_id(new_netif);
				}
			}
		}
#endif
	} else if ((flags & NETIF_PPPOE)) {
		if (!ppa_check_is_pppoe_netif(netif)) {
			if (ppa_is_tunnel_if(netif) == 0)
				f_need_update = 1;
		} else {
			p_info->pppoe_session_id =
				ppa_pppoe_get_pppoe_session_id(netif);
		}
	} else if ((flags & NETIF_PPPOATM)) {
		if (!ppa_if_is_pppoa(netif, NULL)) {
			if (ppa_is_tunnel_if(netif) == 0)
				f_need_update = 1;
		}
	}

	if (!f_need_update &&
		(flags & (NETIF_BRIDGE | NETIF_PHY_ETH | NETIF_EOA)) != 0) {
#if IS_ENABLED(CONFIG_L2NAT)
		/* Check whether the netif is an l2nat interface or not */
		if (ppa_check_if_netif_l2nat_fn) {
			if (ppa_check_if_netif_l2nat_fn(netif, NULL, 0))
				p_info->flags |= NETIF_L2NAT;
			else
				p_info->flags &= ~NETIF_L2NAT;
		}
#endif
	}

	/* save the force_wanitf flag */
	force_wantif = p_info->f_wanitf.flag_force_wanitf;
	if (strnlen(p_info->lower_ifname, PPA_IF_NAME_SIZE))
		ppa_strncpy(lower_ifname, p_info->lower_ifname,
				sizeof(lower_ifname));
	else
		lower_ifname[0] = 0;

	if (f_need_update) {
		hw_accel_stats_tmp.rx_bytes = p_info->hw_accel_stats.rx_bytes;
		hw_accel_stats_tmp.tx_bytes = p_info->hw_accel_stats.tx_bytes;
		sw_accel_stats_tmp.rx_bytes = p_info->sw_accel_stats.rx_bytes;
		sw_accel_stats_tmp.tx_bytes = p_info->sw_accel_stats.tx_bytes;
#if IS_ENABLED(CONFIG_IPV4_IPV6_COUNTER_SUPPORT)
		hw_accel_stats_tmp.rx_pkts = p_info->hw_accel_stats.rx_pkts;
		hw_accel_stats_tmp.tx_pkts = p_info->hw_accel_stats.tx_pkts;
		hw_accel_stats_tmp.rx_pkts_ipv4 = p_info->hw_accel_stats.rx_pkts_ipv4;
		hw_accel_stats_tmp.tx_pkts_ipv4 = p_info->hw_accel_stats.tx_pkts_ipv4;
		hw_accel_stats_tmp.rx_pkts_ipv6 = p_info->hw_accel_stats.rx_pkts_ipv6;
		hw_accel_stats_tmp.tx_pkts_ipv6 = p_info->hw_accel_stats.tx_pkts_ipv6;

		sw_accel_stats_tmp.rx_pkts = p_info->sw_accel_stats.rx_pkts;
		sw_accel_stats_tmp.tx_pkts = p_info->sw_accel_stats.tx_pkts;
		sw_accel_stats_tmp.rx_pkts_ipv4 = p_info->sw_accel_stats.rx_pkts_ipv4;
		sw_accel_stats_tmp.tx_pkts_ipv4 = p_info->sw_accel_stats.tx_pkts_ipv4;
		sw_accel_stats_tmp.rx_pkts_ipv6 = p_info->sw_accel_stats.rx_pkts_ipv6;
		sw_accel_stats_tmp.tx_pkts_ipv6 = p_info->sw_accel_stats.tx_pkts_ipv6;
#endif /* CONFIG_IPV4_IPV6_COUNTER_SUPPORT */
	}

	ppa_netif_put(p_info);

	if (f_need_update) {
		if ((flags & NETIF_LAN_IF))
			ppa_netif_remove(ifname, 1);
		if ((flags & NETIF_WAN_IF))
			ppa_netif_remove(ifname, 0);

		if ((flags & NETIF_LAN_IF) &&
			ppa_netif_add(ifname, 1, NULL, lower_ifname,
				  force_wantif) != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				  "update lan interface %s fail\n", ifname);
			return PPA_FAILURE;
		}

		if ((flags & NETIF_WAN_IF) &&
			ppa_netif_add(ifname, 0, NULL, lower_ifname,
				  force_wantif) != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				  "update wan interface %s fail\n", ifname);
			return PPA_FAILURE;
		}

		if (ppa_get_netif_info(ifname, &p_info) != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_ERR,
				  "fail: device: %s not accelerated\n", ifname);
			return PPA_ENOTAVAIL;
		}

		p_info->hw_accel_stats.rx_bytes = hw_accel_stats_tmp.rx_bytes;
		p_info->hw_accel_stats.tx_bytes = hw_accel_stats_tmp.tx_bytes;
		p_info->sw_accel_stats.rx_bytes = sw_accel_stats_tmp.rx_bytes;
		p_info->sw_accel_stats.tx_bytes = sw_accel_stats_tmp.tx_bytes;
#if IS_ENABLED(CONFIG_IPV4_IPV6_COUNTER_SUPPORT)
		p_info->hw_accel_stats.rx_pkts = hw_accel_stats_tmp.rx_pkts;
		p_info->hw_accel_stats.tx_pkts = hw_accel_stats_tmp.tx_pkts;
		p_info->hw_accel_stats.rx_pkts_ipv4 = hw_accel_stats_tmp.rx_pkts_ipv4;
		p_info->hw_accel_stats.tx_pkts_ipv4 = hw_accel_stats_tmp.tx_pkts_ipv4;
		p_info->hw_accel_stats.rx_pkts_ipv6 = hw_accel_stats_tmp.rx_pkts_ipv6;
		p_info->hw_accel_stats.tx_pkts_ipv6 = hw_accel_stats_tmp.tx_pkts_ipv6;

		p_info->sw_accel_stats.rx_pkts = sw_accel_stats_tmp.rx_pkts;
		p_info->sw_accel_stats.tx_pkts = sw_accel_stats_tmp.tx_pkts;
		p_info->sw_accel_stats.rx_pkts_ipv4 = sw_accel_stats_tmp.rx_pkts_ipv4;
		p_info->sw_accel_stats.tx_pkts_ipv4 = sw_accel_stats_tmp.tx_pkts_ipv4;
		p_info->sw_accel_stats.rx_pkts_ipv6 = sw_accel_stats_tmp.rx_pkts_ipv6;
		p_info->sw_accel_stats.tx_pkts_ipv6 = sw_accel_stats_tmp.tx_pkts_ipv6;
#endif /* CONFIG_IPV4_IPV6_COUNTER_SUPPORT */

		ppa_netif_put(p_info);
	}
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_netif_update);

int32_t ppa_netif_start_iteration(uint32_t *ppos, struct netif_info **ifinfo)
{
	uint32_t l;
	struct netif_info *p;

	ppa_netif_lock_list();

	if (!ppa_is_init()) {
		*ifinfo = NULL;
		return PPA_FAILURE;
	}

	p = g_netif;
	l = *ppos;
	while (p && l) {
		p = p->next;
		l--;
	}

	if (l == 0 && p) {
		++*ppos;
		*ifinfo = p;
		return PPA_SUCCESS;
	} else {
		*ifinfo = NULL;
		return PPA_FAILURE;
	}
}
EXPORT_SYMBOL(ppa_netif_start_iteration);

int32_t ppa_netif_iterate_next(uint32_t *ppos, struct netif_info **ifinfo)
{
	if (*ifinfo) {
		++*ppos;
		*ifinfo = (*ifinfo)->next;
		return *ifinfo ? PPA_SUCCESS : PPA_FAILURE;
	} else {
		return PPA_FAILURE;
	}
}
EXPORT_SYMBOL(ppa_netif_iterate_next);

void ppa_netif_stop_iteration(void)
{
	ppa_netif_unlock_list();
}
EXPORT_SYMBOL(ppa_netif_stop_iteration);

/* ####################################
 *		   Init/Cleanup API
 * ####################################
 */
int32_t ppa_api_netif_manager_init(void)
{
	struct phys_port_info *p_phys_port_info;
	uint32_t pos = 0;
	int i, ret = PPA_SUCCESS;
	PPE_IFINFO if_info;
	PPA_COUNT_CFG count = {0};

	ppa_netif_free_list();
	ppa_phys_port_free_list();

	ppa_drv_get_number_of_phys_port(&count, 0);

	for (i = 0; i <= count.num; i++) {
		ppa_memset(&if_info, 0, sizeof(PPE_IFINFO));
		if_info.port = i;
		ppa_drv_get_phys_port_info(&if_info, 0);
		switch ((if_info.if_flags & (PPA_PHYS_PORT_FLAGS_TYPE_MASK |
						 PPA_PHYS_PORT_FLAGS_MODE_MASK |
						 PPA_PHYS_PORT_FLAGS_VALID))) {
		case PPA_PHYS_PORT_FLAGS_MODE_CPU_VALID:
			if (g_phys_port_cpu == ~0)
				g_phys_port_cpu = i;
			break;
		case PPA_PHYS_PORT_FLAGS_MODE_ATM_WAN_VALID:
			if (g_phys_port_atm_wan == ~0)
				g_phys_port_atm_wan = i;
			if (if_info.if_flags & PPA_PHYS_PORT_FLAGS_OUTER_VLAN)
				g_phys_port_atm_wan_vlan = 2;
			else
				g_phys_port_atm_wan_vlan = 1;
			break;
		case PPA_PHYS_PORT_FLAGS_MODE_ETH_LAN_VALID:
		case PPA_PHYS_PORT_FLAGS_MODE_ETH_WAN_VALID:
		case PPA_PHYS_PORT_FLAGS_MODE_ETH_MIX_VALID:
		case PPA_PHYS_PORT_FLAGS_MODE_EXT_LAN_VALID:
			if (if_info.ifname[0]) {
				p_phys_port_info = ppa_phys_port_alloc_item();
				if (!p_phys_port_info)
					goto PPA_API_NETIF_MANAGER_INIT_FAIL;
				ppa_strncpy(p_phys_port_info->ifname, if_info.ifname, PPA_IF_NAME_SIZE);
				switch ((if_info.if_flags &
					 PPA_PHYS_PORT_FLAGS_MODE_MASK)) {
				case PPA_PHYS_PORT_FLAGS_MODE_LAN:
					p_phys_port_info->mode = 1;
					break;
				case PPA_PHYS_PORT_FLAGS_MODE_WAN:
					p_phys_port_info->mode = 2;
					break;
				case PPA_PHYS_PORT_FLAGS_MODE_MIX:
					p_phys_port_info->mode = 3;
				}
				if ((if_info.if_flags &
					 PPA_PHYS_PORT_FLAGS_TYPE_MASK) ==
					PPA_PHYS_PORT_FLAGS_TYPE_ETH)
					p_phys_port_info->type = 2;
				else
					p_phys_port_info->type = 3;
				if (if_info.if_flags &
					PPA_PHYS_PORT_FLAGS_OUTER_VLAN)
					p_phys_port_info->vlan = 2;
				else
					p_phys_port_info->vlan = 1;
				p_phys_port_info->port = i;
				ppa_lock_get(&g_phys_port_lock);
				p_phys_port_info->next = g_phys_port_info;
				g_phys_port_info = p_phys_port_info;
				ppa_lock_release(&g_phys_port_lock);
			}
		}
	}

	pos = 0;
#if IS_ENABLED(CONFIG_PPA_API_DIRECTPATH)
	if (ppa_is_init() && ppa_directpath_port_add_fn)
		ppa_directpath_port_add_fn();
#endif
	ppa_lock_init(&g_manual_if_lock);
	PPA_LIST_HEAD_INIT(&manual_del_iface_list);
	ppa_register_netdevice_notifier(&ppa_netdevice_notifier);
	return ret;

PPA_API_NETIF_MANAGER_INIT_FAIL:
	ppa_phys_port_free_list();
	return PPA_ENOMEM;
}

void ppa_api_netif_manager_exit(void)
{
	ppa_unregister_netdevice_notifier(&ppa_netdevice_notifier);
	ppa_netif_free_list();
	ppa_phys_port_free_list();
}

int32_t ppa_api_netif_manager_create(void)
{
	if (ppa_lock_init(&g_phys_port_lock)) {
		ppa_debug(DBG_ENABLE_MASK_ERR,
			  "Failed in creating lock for physical network interface list.\n");
		return PPA_EIO;
	}

	if (ppa_lock_init(&g_netif_lock)) {
		ppa_lock_destroy(&g_phys_port_lock);
		ppa_debug(DBG_ENABLE_MASK_ERR,
			  "Failed in creating lock for network interface list.\n");
		return PPA_EIO;
	}
	return PPA_SUCCESS;
}

void ppa_api_netif_manager_destroy(void)
{
	ppa_lock_destroy(&g_netif_lock);
	ppa_lock_destroy(&g_phys_port_lock);
}
