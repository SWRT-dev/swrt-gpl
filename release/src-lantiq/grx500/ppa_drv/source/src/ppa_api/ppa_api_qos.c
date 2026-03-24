/******************************************************************************
 **
 ** FILE NAME	: ppa_api_qos.c
 ** PROJECT	: UEIP
 ** MODULES	: PPA API (Routing/Bridging Acceleration APIs)
 **
 ** DATE	: 11 DEC 2009
 ** AUTHOR	: Shao Guohua
 ** DESCRIPTION	: PPA Protocol Stack QOS API Implementation
 ** COPYRIGHT	: Copyright (c) 2020, MaxLinear, Inc.
 **               Copyright (c) 2009, Lantiq Deutschland GmbH
 **	              Am Campeon 3; 85579 Neubiberg, Germany
 **
 **	 For licensing information, see the file 'LICENSE' in the root folder of
 **	 this software module.
 **
 ** HISTORY
 ** $Date		$Author			$Comment
 ** 11 DEC 2009		Shao Guohua		Initiate Version
 *******************************************************************************/

#include <linux/version.h>
#include <generated/autoconf.h>
/*
 *	PPA Specific Head File
 */
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_drv_wrapper.h>
#include <net/ppa/ppa_hal_wrapper.h>

#include "ppa_api_misc.h"
#include "ppa_api_netif.h"
#include "ppa_api_qos.h"
#include "ppa_api_session.h"
#include "ppa_api_mib.h"
#include "ppa_api_debug.h"

#if IS_BUILTIN(CONFIG_LTQ_ETHSW_API)
#include <net/switch_api/lantiq_gsw_api.h>
#include <net/switch_api/lantiq_gsw_flow.h>
#endif

#if IS_ENABLED(WMM_QOS_CONFIG)
int g_eth_class_prio_map[MAX_WLAN_DEV][MAX_TC_NUM] = {
	{0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7},
	{0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7},
	{0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7},
	{0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7}
};
#endif /*WMM_QOS_CONFIG*/

#define PPA_MAX_Q 40
#define PPA_MAX_Q_LEN 100
#define PPA_Q_CAP 1
#define MAX_QOS_Q_CAPS 6

#define ETHWAN_PORT 15
int8_t FLEXI_PORT[]	= {7, 8, 9, 10, 11, 12, 13, 14};	/* Flexible Ports for DSL (ATM/PTM) WAN and WLAN. */
int8_t LAN_PORT[] 	= {2,3,4,5,6};
int8_t LAN_PORT_Q[][4] 	= { {8,9,10,11}, {12,13,14,15}, {16,17,18,19},{20,21,22,23},{24,25,26,27} };
int32_t WT_Q[] 		= {20480,20480,65535,65535};
int32_t SCH_Q[] 	= {1,1,0,0};
#define MAX_NUMBER_OF_LAN_PORTS 4

#define TMU_GREEN_DEFAULT_THRESHOLD	0x24

static QOS_SHAPER_LIST_ITEM *g_qos_shaper = NULL;
static PPA_LOCK g_qos_shaper_lock;

#if IS_ENABLED(CONFIG_SOC_GRX500)
static QOS_METER_LIST_ITEM *g_qos_meter;
static PPA_LOCK g_qos_meter_lock;
#endif /* CONFIG_SOC_GRX500 */

#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
QOS_LOGICAL_IF_LIST_ITEM *g_qos_logical_if;
PPA_LOCK g_qos_logical_if_lock;
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/

#if IS_ENABLED(CONFIG_SOC_GRX500)
static QOS_INGGRP_LIST_ITEM *g_qos_inggrp_list = NULL;
static PPA_LOCK g_qos_inggrp_list_lock;
#endif/*CONFIG_SOC_GRX500*/

QOS_QUEUE_LIST_ITEM *g_qos_queue;
PPA_LOCK g_qos_queue_lock;

typedef enum
{
    WMM_TC_BE_1 = 0,
    WMM_TC_BK_1,
    WMM_TC_BK_2,
    WMM_TC_BE_2,
    WMM_TC_VI_1,
    WMM_TC_VI_2,
    WMM_TC_VO_1,
    WMM_TC_VO_2
} WMM_TC;

extern int32_t ppa_drv_mpe_set_checksum_queue_map(uint32_t pmac_port, uint32_t flags);

/*
#############STRUCTURE DEFINITIONS : START################
*/

/*
############# STRUCTURE DEFINITIONS : END################
*/
/*
############# SUPPORTING SHAPER FUNCTION DEFINITIONS : START################
*/

/* Shaper info data structures */
QOS_SHAPER_LIST_ITEM * ppa_qos_shaper_alloc_item(void)	/* alloc_netif_info */
{
	QOS_SHAPER_LIST_ITEM *obj;

	obj = (QOS_SHAPER_LIST_ITEM *)ppa_malloc(sizeof(*obj));
	if ( obj ) {
		ppa_memset(obj, 0, sizeof(*obj));
		ppa_atomic_set(&obj->count, 1);
	}
	return obj;
}
EXPORT_SYMBOL(ppa_qos_shaper_alloc_item);

void ppa_qos_shaper_free_item(QOS_SHAPER_LIST_ITEM *obj)	/* free_netif_info*/
{
	if ( ppa_atomic_dec(&obj->count) == 0 ) {
		ppa_free(obj);
	}
}
EXPORT_SYMBOL(ppa_qos_shaper_free_item);

void ppa_qos_shaper_lock_list(void)	/* lock_netif_info_list*/
{
	ppa_lock_get(&g_qos_shaper_lock);
}
EXPORT_SYMBOL(ppa_qos_shaper_lock_list);

void ppa_qos_shaper_unlock_list(void)	/* unlock_netif_info_list */
{
	ppa_lock_release(&g_qos_shaper_lock);
}
EXPORT_SYMBOL(ppa_qos_shaper_unlock_list);

void __ppa_qos_shaper_add_item(QOS_SHAPER_LIST_ITEM *obj)	/* add_netif_info */
{
	ppa_atomic_inc(&obj->count);
	obj->next = g_qos_shaper;
	g_qos_shaper = obj;
}
EXPORT_SYMBOL(__ppa_qos_shaper_add_item);

void ppa_qos_shaper_remove_item(int32_t s_num, PPA_IFNAME ifname[16],QOS_SHAPER_LIST_ITEM **pp_info)	/* remove_netif_info*/
{
	QOS_SHAPER_LIST_ITEM *p_prev, *p_cur;

	if ( pp_info )
		*pp_info = NULL;
	p_prev = NULL;
	ppa_qos_shaper_lock_list();
	for ( p_cur = g_qos_shaper; p_cur; p_prev = p_cur, p_cur = p_cur->next )
		if ( (p_cur->shaperid == s_num) && (strcmp(p_cur->ifname,ifname) == 0)) {
			if ( !p_prev )
				g_qos_shaper = p_cur->next;
			else
				p_prev->next = p_cur->next;
			if ( pp_info )
				*pp_info = p_cur;
			else
				ppa_qos_shaper_free_item(p_cur);
			break;
		}
	ppa_qos_shaper_unlock_list();
}
EXPORT_SYMBOL(ppa_qos_shaper_remove_item);

void ppa_qos_shaper_free_list(void)	/* free_netif_info_list*/
{
	QOS_SHAPER_LIST_ITEM *obj;

	ppa_qos_shaper_lock_list();
	while ( g_qos_shaper ) {
		obj = g_qos_shaper;
		g_qos_shaper = g_qos_shaper->next;

		ppa_qos_shaper_free_item(obj);
		obj = NULL;
	}
	ppa_qos_shaper_unlock_list();
}
EXPORT_SYMBOL(ppa_qos_shaper_free_list);

/* QoS Queue*/
int32_t __ppa_qos_shaper_lookup(int32_t s_num, PPA_IFNAME ifname[16],QOS_SHAPER_LIST_ITEM **pp_info)	/* netif_info_is_added*/
{
	int32_t ret = PPA_ENOTAVAIL;
	QOS_SHAPER_LIST_ITEM *p;

	for (p = g_qos_shaper; p; p = p->next) {
		if ((p->shaperid == s_num) && (strcmp(p->ifname, ifname) == 0)) {
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
EXPORT_SYMBOL(__ppa_qos_shaper_lookup);

int32_t ppa_qos_shaper_lookup(int32_t s_num, PPA_IFNAME ifname[16],QOS_SHAPER_LIST_ITEM **pp_info)	/* netif_info_is_added*/
{
	int32_t ret;
	ppa_qos_shaper_lock_list();
	ret = __ppa_qos_shaper_lookup(s_num, ifname, pp_info);
	ppa_qos_shaper_unlock_list();

	return ret;
}
EXPORT_SYMBOL(ppa_qos_shaper_lookup);

int32_t qosal_add_shaper(PPA_CMD_RATE_INFO *rate_info, QOS_SHAPER_LIST_ITEM **pp_item)
{
	int32_t ret = PPA_SUCCESS;
	int32_t res;
	QOS_SHAPER_LIST_ITEM *p_item;
	QOS_SHAPER_LIST_ITEM *p_item1;
	PPA_QOS_ADD_SHAPER_CFG shaper_cfg;
	PPA_HSEL_CAP_NODE *caps_list=NULL;

	uint32_t num_caps = 0, i, j;
	uint8_t f_more_hals = 0;
	
	memset(&shaper_cfg,0x00,sizeof(PPA_QOS_ADD_SHAPER_CFG)); 
	p_item = ppa_qos_shaper_alloc_item();
	if ( !p_item ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"alloc shaper list item failed \n" ); 
		return PPA_ENOMEM;
	}
	ppa_strncpy(p_item->ifname, rate_info->ifname, PPA_IF_NAME_SIZE);
	ppa_strncpy(p_item->dev_name, rate_info->ifname, PPA_IF_NAME_SIZE);
	p_item->portid = rate_info->portid;
	p_item->shaperid = rate_info->shaperid;
	p_item->shaper.enable = rate_info->shaper.enable;
	p_item->shaper.pir = rate_info->shaper.pir;
	p_item->shaper.pbs = rate_info->shaper.pbs;
	p_item->shaper.cir = rate_info->shaper.cir;
	p_item->shaper.cbs = rate_info->shaper.cbs;
	p_item->shaper.flags = rate_info->shaper.flags;

	p_item1 = *pp_item;
	res = ppa_qos_shaper_lookup(rate_info->shaperid,rate_info->ifname,&p_item1);
	if( res == PPA_SUCCESS ) {
		p_item->p_entry = p_item1->p_entry;
	
		ppa_qos_shaper_remove_item(p_item1->shaperid,p_item1->ifname,NULL);
		ppa_qos_shaper_free_item(p_item1);
	}

	__ppa_qos_shaper_add_item(p_item);
	
	caps_list = (PPA_HSEL_CAP_NODE*) ppa_malloc (MAX_QOS_Q_CAPS*sizeof(PPA_HSEL_CAP_NODE));
	if(!caps_list) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"alloc caps_list item failed \n" ); 
		return PPA_FAILURE;
	}

	ppa_memset(caps_list, 0, (MAX_QOS_Q_CAPS*sizeof(PPA_HSEL_CAP_NODE)));
	if( rate_info->shaper.enable ) {
		if(rate_info->shaperid == -1)
			caps_list[num_caps++].cap = PORT_SHAPER;
		else
			caps_list[num_caps++].cap = Q_SHAPER;
	}

	if(ppa_select_hals_from_caplist(0, num_caps, caps_list) != PPA_SUCCESS) {
		ppa_free(caps_list);
		return PPA_FAILURE;
	}

	for(i = 0; i < num_caps;)  { 
		f_more_hals = 0;
		j = ppa_group_hals_in_capslist(i, num_caps, caps_list);
	
		/* Based on the capability of first entry in the list we can decide the action*/
		switch(caps_list[i].cap)  {
			case Q_SHAPER:
			case PORT_SHAPER: {
				if( caps_list[i].hal_id == PPE_HAL )
					break;
				else {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Hal selected !PPE \n" ); 
					shaper_cfg.enable = rate_info->shaper.enable;
					shaper_cfg.mode = rate_info->shaper.mode;
					shaper_cfg.pir = rate_info->shaper.pir;
					shaper_cfg.pbs = rate_info->shaper.pbs;
					shaper_cfg.cir = rate_info->shaper.cir;
					shaper_cfg.cbs = rate_info->shaper.cbs;
					shaper_cfg.flags = rate_info->shaper.flags;
#if IS_ENABLED(CONFIG_PPA_PUMA7)
					ppa_strncpy(shaper_cfg.ifname, rate_info->ifname, PPA_IF_NAME_SIZE);
#endif 
					ret = ppa_set_qos_shaper(rate_info->shaperid,rate_info->rate,
							rate_info->burst,&shaper_cfg,rate_info->shaper.flags,caps_list[i].hal_id);

					if(ret == PPA_SUCCESS) {
						p_item->p_entry = (shaper_cfg.phys_shaperid);
					} else
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_set_qos_shaper failed \n" ); 
				}
				break;
			}	
			default:
				break;
		}
		i+=j;
	}

	p_item->caps_list = caps_list;
	p_item->num_caps = num_caps;
	*pp_item = p_item;
	return ret; 
}

#if IS_ENABLED(CONFIG_SOC_GRX500)
void ppa_qos_meter_lock_list(void)
{
	ppa_lock_get(&g_qos_meter_lock);
}
EXPORT_SYMBOL(ppa_qos_meter_lock_list);

void ppa_qos_meter_unlock_list(void)
{
	ppa_lock_release(&g_qos_meter_lock);
}
EXPORT_SYMBOL(ppa_qos_meter_unlock_list);

void ppa_qos_meter_free_item(QOS_METER_LIST_ITEM *obj)
{
	if (ppa_atomic_dec(&obj->count) == 0)
		ppa_free(obj);
}
EXPORT_SYMBOL(ppa_qos_meter_free_item);

void ppa_qos_meter_free_list(void)
{
	QOS_METER_LIST_ITEM *obj;

	ppa_qos_meter_lock_list();
	while (g_qos_meter) {
		obj = g_qos_meter;
		g_qos_meter = g_qos_meter->next;
		ppa_qos_meter_free_item(obj);
		obj = NULL;
	}
	ppa_qos_meter_unlock_list();
}
EXPORT_SYMBOL(ppa_qos_meter_free_list);

int32_t __ppa_qos_meter_lookup(PPA_IFNAME *ifname, QOS_METER_LIST_ITEM **pp_info)
{
	int32_t ret = PPA_ENOTAVAIL;
	QOS_METER_LIST_ITEM *p;

	for (p = g_qos_meter; p; p = p->next) {
		if ((strcmp(p->ifname, ifname) == 0)) {
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
EXPORT_SYMBOL(__ppa_qos_meter_lookup);

int32_t ppa_qos_meter_lookup(PPA_IFNAME *ifname, QOS_METER_LIST_ITEM **pp_info)
{
	int32_t ret;
	ppa_qos_meter_lock_list();
	ret = __ppa_qos_meter_lookup(ifname, pp_info);
	ppa_qos_meter_unlock_list();

	return ret;
}
EXPORT_SYMBOL(ppa_qos_meter_lookup);

QOS_METER_LIST_ITEM *ppa_qos_meter_alloc_item(void)
{
	QOS_METER_LIST_ITEM *obj;

	obj = (QOS_METER_LIST_ITEM *)ppa_malloc(sizeof(*obj));
	if (obj) {
		ppa_memset(obj, 0, sizeof(*obj));
		ppa_atomic_set(&obj->count, 1);
	}
	return obj;
}
EXPORT_SYMBOL(ppa_qos_meter_alloc_item);

void __ppa_qos_meter_add_item(QOS_METER_LIST_ITEM *obj)
{
	ppa_atomic_inc(&obj->count);
	obj->next = g_qos_meter;
	g_qos_meter = obj;
}
EXPORT_SYMBOL(__ppa_qos_meter_add_item);

int32_t qosal_add_meter(PPA_QOS_METER_CFG *meter_cfg)
{
	int32_t ret = PPA_SUCCESS;
	QOS_METER_LIST_ITEM *p_item = NULL;
	PPA_NETIF *netif = NULL;
	dp_subif_t *dp_subif = NULL;
	ppa_debug(DBG_ENABLE_MASK_QOS, "meter id : meter name : ifname: %d: %d %s %s\n",
		meter_cfg->phys_meterid,  meter_cfg->port_id, meter_cfg->meter_name, meter_cfg->ifname);

	PPA_ALLOC(dp_subif_t, dp_subif);
	ppa_memset(dp_subif, 0, sizeof(dp_subif_t));
	netif = ppa_get_netif(meter_cfg->ifname);
	ret = dp_get_netif_subifid(netif, NULL, NULL, NULL, dp_subif, 0);
	if (ret != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "Meter dp_get_netif_subifid failed\n");
		ppa_free(dp_subif);
		return PPA_FAILURE;
	}
	meter_cfg->port_id = dp_subif->port_id;
	ppa_free(dp_subif);

	if (meter_cfg->flags & PPA_QOS_OP_F_ADD) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "Qosal Add meter\n");
		p_item = ppa_qos_meter_alloc_item();
		if (!p_item) {
			ppa_debug(DBG_ENABLE_MASK_QOS, "Qosal meter allocation failed\n");
			return PPA_ENOMEM;
		}
		__ppa_qos_meter_add_item(p_item);
	}
	if ((meter_cfg->flags & PPA_QOS_OP_F_MODIFY) ||
			(meter_cfg->flags & PPA_QOS_OP_F_DELETE)) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "Qosal Modify meter\n");
		ret = ppa_qos_meter_lookup(meter_cfg->ifname, &p_item);
		if (ret != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_QOS, "Qosal Meter lookup Failed\n");
			return ret;
		}
	}
	if (p_item == NULL) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "Invalid flags passed\n");
		return PPA_FAILURE;
	}
	ppa_strncpy(p_item->ifname, meter_cfg->ifname, PPA_IF_NAME_SIZE);
	ppa_strncpy(p_item->meter.meter_name, meter_cfg->meter_name, PPA_IF_NAME_SIZE);
	p_item->meter.type = meter_cfg->type;
	p_item->meter.enable = meter_cfg->enable;
	p_item->meter.cir = meter_cfg->cir;
	p_item->meter.cbs = meter_cfg->cbs;
	p_item->meter.pir = meter_cfg->pir;
	p_item->meter.pbs = meter_cfg->pbs;
	p_item->meter.port_id = meter_cfg->port_id;
	p_item->meter.flags = meter_cfg->flags;

	meter_cfg->phys_meterid = p_item->meter.phys_meterid;
	meter_cfg->pce_id = p_item->meter.pce_id;

	ret = ppa_set_qos_meter(meter_cfg, PAE_HAL);
	if (ret == PPA_FAILURE || (meter_cfg->flags & PPA_QOS_OP_F_DELETE)) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "Qosal Delete meter\n");
		ppa_qos_meter_free_item(p_item);
	} else {
		p_item->meter.phys_meterid = meter_cfg->phys_meterid;
		p_item->meter.pce_id = meter_cfg->pce_id;
	}

	return ret;
}

int32_t ppa_ioctl_add_qos_meter(unsigned int cmd,
				unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int32_t ret;

	ppa_memset(&cmd_info->meter_cfg, 0, sizeof(cmd_info->meter_cfg));

	if (ppa_copy_from_user(&cmd_info->meter_cfg, (void *)arg,
			sizeof(cmd_info->meter_cfg)) != 0)
		return PPA_FAILURE;

	ret = qosal_add_meter(&cmd_info->meter_cfg);
	if (ret != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "qosal_add_meter returned failure\n");
		return PPA_FAILURE;
	}
	return ret;
}
EXPORT_SYMBOL(ppa_ioctl_add_qos_meter);
#endif /* CONFIG_SOC_GRX500 */

/*
############# SUPPORTING SHAPER FUNCTION DEFINITIONS : END################
*/

/*
############# SUPPORTING METER FUNCTION DEFINITIONS : START################
*/

#if IS_ENABLED(CONFIG_SOC_GRX500)
int32_t ppa_ioctl_set_qos_meter(unsigned int cmd,
				unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int32_t ret;

	ppa_memset(&cmd_info->meter_info, 0, sizeof(cmd_info->meter_info));

	if (ppa_copy_from_user(&cmd_info->meter_info, (void *)arg,
			sizeof(cmd_info->meter_info)) != PPA_SUCCESS)
		return PPA_FAILURE;

	ret = qosal_set_qos_meter(&cmd_info->meter_info);
	if (ret != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR, "<%s:%d> qosal_set_qos_meter returned error\n",
				__FUNCTION__, __LINE__);
		return PPA_FAILURE;
	}
	return ret;
}
EXPORT_SYMBOL(ppa_ioctl_set_qos_meter);

int32_t qosal_set_qos_meter(PPA_QOS_METER_INFO *meter_info)
{
	int32_t ret;
	PPA_QOS_METER_CFG meter_cfg;

	memset(&meter_cfg, 0x00, sizeof(PPA_QOS_METER_CFG));

	meter_cfg.type = meter_info->type;
	meter_cfg.enable = meter_info->enable;
	meter_cfg.cir = meter_info->cir;
	meter_cfg.pir = meter_info->pir;
	meter_cfg.cbs = meter_info->cbs;
	meter_cfg.pbs = meter_info->pbs;
	meter_cfg.flags = meter_info->flags;
	meter_cfg.phys_meterid = meter_info->meterid;

	ret = ppa_set_qos_meter(&meter_cfg, TMU_HAL);

	return ret;
}
/*
############# SUPPORTING METER FUNCTION DEFINITIONS : END################
*/
/*
############# SUPPORTING INGRESS FUNCTION DEFINITIONS : START################
*/
QOS_INGGRP_LIST_ITEM * ppa_qos_inggrp_alloc_item(void)	/*	alloc_netif_info*/
{
	QOS_INGGRP_LIST_ITEM *obj;

	obj = (QOS_INGGRP_LIST_ITEM *)ppa_malloc(sizeof(*obj));
	if ( obj ) {
		ppa_memset(obj, 0, sizeof(*obj));
		ppa_atomic_set(&obj->count, 1);
	}
	return obj;
}
EXPORT_SYMBOL(ppa_qos_inggrp_alloc_item);

void ppa_qos_inggrp_free_item(QOS_INGGRP_LIST_ITEM *obj)	/* free_netif_info*/
{
	if ( ppa_atomic_dec(&obj->count) == 0 ) {
		ppa_free(obj);
	}
}
EXPORT_SYMBOL(ppa_qos_inggrp_free_item);

void ppa_qos_inggrp_lock_list(void)	/*	lock_netif_info_list*/
{
	ppa_lock_get(&g_qos_inggrp_list_lock);
}
EXPORT_SYMBOL(ppa_qos_inggrp_lock_list);

void ppa_qos_inggrp_unlock_list(void)	/*	unlock_netif_info_list*/
{
	ppa_lock_release(&g_qos_inggrp_list_lock);
}
EXPORT_SYMBOL(ppa_qos_inggrp_unlock_list);

void __ppa_qos_inggrp_add_item(QOS_INGGRP_LIST_ITEM *obj)	 /*	add_netif_info*/
{
	ppa_atomic_inc(&obj->count);
	obj->next = (struct qos_inggrp_list_item *)g_qos_inggrp_list;
	g_qos_inggrp_list = obj;
}
EXPORT_SYMBOL(__ppa_qos_inggrp_add_item);

void ppa_qos_inggrp_remove_item(PPA_IFNAME ifname[16], QOS_INGGRP_LIST_ITEM **pp_info)	/*	remove_netif_info*/
{
	QOS_INGGRP_LIST_ITEM *p_prev, *p_cur;

	if ( pp_info )
		*pp_info = NULL;
	p_prev = NULL;
	ppa_qos_inggrp_lock_list();
	for ( p_cur = g_qos_inggrp_list; p_cur; p_prev = p_cur, p_cur = (QOS_INGGRP_LIST_ITEM *)p_cur->next )
		if ((strcmp(p_cur->ifname,ifname) == 0)) {
			if ( !p_prev )
				g_qos_inggrp_list = (QOS_INGGRP_LIST_ITEM *)p_cur->next;
			else
				p_prev->next = p_cur->next;
			if ( pp_info )
				*pp_info = p_cur;
			else
				ppa_qos_inggrp_free_item(p_cur);
			break;
		}
	ppa_qos_inggrp_unlock_list();
}
EXPORT_SYMBOL(ppa_qos_inggrp_remove_item);

void ppa_qos_inggrp_free_list(void)
{
	 QOS_INGGRP_LIST_ITEM *obj;

	ppa_qos_inggrp_lock_list();
	while ( g_qos_inggrp_list ) {
		obj = g_qos_inggrp_list;
		g_qos_inggrp_list = (QOS_INGGRP_LIST_ITEM *)g_qos_inggrp_list->next;

		ppa_qos_inggrp_free_item(obj);
		obj = NULL;
	}
	ppa_qos_inggrp_unlock_list();
}
EXPORT_SYMBOL(ppa_qos_inggrp_free_list);

int32_t __ppa_qos_inggrp_lookup(const PPA_IFNAME ifname[16],QOS_INGGRP_LIST_ITEM **pp_info)	/* netif_info_is_added*/
{
	int32_t ret = PPA_ENOTAVAIL;
	QOS_INGGRP_LIST_ITEM *p;
	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: lookup ifname=%s\n", __func__, __LINE__, ifname);
	for ( p = g_qos_inggrp_list; p; p = (QOS_INGGRP_LIST_ITEM *)p->next ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: entry %p: ingress group=%d, ifname=%s\n", __func__,
				__LINE__, p, p->ingress_group, p->ifname); 
		if (strcmp(p->ifname,ifname) == 0) {
			ret = PPA_SUCCESS;
			if ( pp_info ) {
				ppa_atomic_inc(&p->count);
				*pp_info = p;
			}
			break;
		}
	}

	return ret;
}
EXPORT_SYMBOL(__ppa_qos_inggrp_lookup);

int32_t ppa_qos_inggrp_lookup(const PPA_IFNAME ifname[16],QOS_INGGRP_LIST_ITEM **pp_info)	/* netif_info_is_added*/
{
	int32_t ret;
	ppa_qos_inggrp_lock_list();
	ret = __ppa_qos_inggrp_lookup(ifname, pp_info);
	ppa_qos_inggrp_unlock_list();

	return ret;
}
EXPORT_SYMBOL(ppa_qos_inggrp_lookup);

int32_t ppa_set_qos_inggrp(PPA_QOS_INGGRP_CFG *inggrp_info, uint32_t hal_id)
{
	/* For future use - presently the HAL doesn't need the grouping information,
	 * it is maintained only in PPA.
	 */
	return PPA_SUCCESS;
}

int32_t qosal_get_qos_inggrp(PPA_QOS_INGGRP inggrp, PPA_IFNAME ifnames[PPA_QOS_MAX_IF_PER_INGGRP][PPA_IF_NAME_SIZE])
{
	QOS_INGGRP_LIST_ITEM *p;
	int32_t count = 0;

	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s;%d: find all interfaces in inggrp%d", __func__, __LINE__, inggrp);
	ppa_qos_inggrp_lock_list();
	for ( p = g_qos_inggrp_list; p && (count < PPA_QOS_MAX_IF_PER_INGGRP); p = (QOS_INGGRP_LIST_ITEM *)p->next ) {
		if (p->ingress_group == inggrp) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: found interface %s in inggrp %d", __func__, __LINE__, p->ifname, inggrp); 
			strncpy(ifnames[count++], p->ifname, PPA_IF_NAME_SIZE);
		}
	}
	ppa_qos_inggrp_unlock_list();
	return count;
}
EXPORT_SYMBOL(qosal_get_qos_inggrp);

int32_t qosal_set_qos_inggrp(PPA_CMD_QOS_INGGRP_INFO *inggrp_info, QOS_INGGRP_LIST_ITEM **pp_item)
{
	QOS_INGGRP_LIST_ITEM *p_item = NULL;
	PPA_HSEL_CAP_NODE hsel = {.cap = QOS_INGGRP};
	PPA_QOS_INGGRP_CFG hcfg;
	uint32_t ret=PPA_SUCCESS;

	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: add %s to inggrp %d\n", __func__, __LINE__,
			inggrp_info->ifname, inggrp_info->ingress_group);

	/* First, check if interface is already added */
	if (ppa_qos_inggrp_lookup(inggrp_info->ifname, &p_item) == PPA_SUCCESS) {
		/* interface found in inggrp list - modify ingress group */
		if (PPA_INGGRP_VALID(inggrp_info->ingress_group) && inggrp_info->ingress_group != p_item->ingress_group) {
			ppa_debug(DBG_ENABLE_MASK_ERR, "%s:%d: WARNING: modify %s ingress group %d -> %d (won't affect already active queues!)\n",
							__func__, __LINE__, inggrp_info->ifname, p_item->ingress_group, inggrp_info->ingress_group);
			p_item->ingress_group = inggrp_info->ingress_group;
		}
	} else if (PPA_INGGRP_INVALID(inggrp_info->ingress_group)) {
			ppa_debug(DBG_ENABLE_MASK_ERR, "%s:%d: ERROR: invalid ingress group %d (%s)\n",
								__func__, __LINE__, inggrp_info->ingress_group, inggrp_info->ifname);
			return PPA_FAILURE;
	}

	/* Second, select HAL which supports Ingress Grouping amd call it */
	if (ppa_select_hals_from_caplist(0, 1, &hsel) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR, "%s:%d: failed to find HAL with Ingress Grouping support\n", __func__, __LINE__);
		return PPA_FAILURE;
	}

	memset(&hcfg, 0, sizeof(hcfg));
	hcfg.ingress_group = inggrp_info->ingress_group;
	ppa_strncpy(hcfg.ifname, inggrp_info->ifname, PPA_IF_NAME_SIZE);
	if (ppa_set_qos_inggrp(&hcfg, hsel.hal_id) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR, "%s:%d: failed to set Ingress Grouping for HAL id %d\n", __func__, __LINE__, hsel.hal_id);
		return PPA_FAILURE;
	}

	if (!p_item) {
		/* Allocate a new node and add to the inggrp list */
		p_item = ppa_qos_inggrp_alloc_item();
		if (!p_item) {
			ppa_debug(DBG_ENABLE_MASK_ERR, "%s:%d: alloc qos inggrp list item failed \n", __func__, __LINE__);
			return PPA_ENOMEM;
		}

		ppa_strncpy(p_item->ifname, inggrp_info->ifname, PPA_IF_NAME_SIZE);
		p_item->ingress_group = inggrp_info->ingress_group;
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: Add item %p ifname=%s, inggrp=%d\n", __func__, __LINE__,
					p_item, p_item->ifname, p_item->ingress_group);
		__ppa_qos_inggrp_add_item(p_item);
	}

	/* success */
	*pp_item = p_item;
	return ret;
}

static uint32_t ppa_set_ingress_qos_generic(const char *ifname, uint32_t *flags)
{
	QOS_INGGRP_LIST_ITEM *p_item;
	uint32_t ret=PPA_SUCCESS;

	if ((*flags & PPA_QOS_Q_F_INGRESS) || (*flags & PPA_QOS_OP_F_INGRESS))	{
		ret = ppa_qos_inggrp_lookup(ifname, &p_item);
		if (ret) {
			ppa_debug(DBG_ENABLE_MASK_ERR, "%s:%d: ifname %s does not exist in qos inggrp list\n", __func__, __LINE__, ifname);
			return ret;
		}
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: set ifname %s to ingress_group %d\n", __func__, __LINE__,
				ifname, p_item->ingress_group);
		*flags |= inggrp2flags(p_item->ingress_group);
		ppa_qos_inggrp_free_item(p_item); /* decrement reference counter */
	}

	return PPA_SUCCESS;
}

static uint32_t ppa_set_ingress_qos_classifier(PPA_CLASS_RULE *rule)
{
	QOS_INGGRP_LIST_ITEM *p_item;

	if ((rule->action.flags & PPA_QOS_Q_F_INGRESS) != PPA_QOS_Q_F_INGRESS)
		return PPA_SUCCESS;

	if (ppa_qos_inggrp_lookup(rule->tx_if, &p_item) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR, "ifname %s does not exist in qos inggrp list\n", rule->rx_if);
		return PPA_FAILURE;
	}

	rule->action.qos_action.flowid_enabled = 1;

	/* TODO: verify groups 1-5 (see tmu_hal_set_ingress_grp_qmap logic) */
	switch (p_item->ingress_group) {
		case PPA_QOS_INGGRP0:
			rule->action.fwd_action.processpath = 1; /* [MPE1|MPE2]=10 */
			rule->action.qos_action.flowid = 0x0;	/* flowid[7:6]=00 */
			break;
		case PPA_QOS_INGGRP1:
			rule->action.fwd_action.processpath = 3; /* [MPE1|MPE2]=11 */
			rule->action.qos_action.flowid = 0x0;	/* flowid[7:6]=00 */
			break;
		case PPA_QOS_INGGRP2:
			rule->action.fwd_action.processpath = 1; /* [MPE1|MPE2]=10 */
			rule->action.qos_action.flowid = 0x40;	 /* flowid[7:6]=01 */
			break;
		case PPA_QOS_INGGRP3:
			rule->action.fwd_action.processpath = 3; /* [MPE1|MPE2]=11 */
			rule->action.qos_action.flowid = 0x40;	 /* flowid[7:6]=01 */
			break;
		case PPA_QOS_INGGRP4:
		case PPA_QOS_INGGRP5:
		default:
			ppa_debug(DBG_ENABLE_MASK_ERR,"%s:%d: ingress group %d NOT supported!\n", __func__, __LINE__,
					p_item->ingress_group);
			rule->action.qos_action.flowid_enabled = 0;
			return PPA_FAILURE;
	}

#if IS_ENABLED(CONFIG_SOC_GRX500)
	if ((rule->action.flags & PPA_QOS_CL_CPU) == PPA_QOS_CL_CPU) {
		/* Reset MPE1 bit to bypass MPE */
		rule->action.fwd_action.processpath = rule->action.fwd_action.processpath & 2;
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: MPE is bypassed for classifier!\n",
				__func__, __LINE__);
	}
#endif /* CONFIG_SOC_GRX500 */

	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: p_item->ingress_group=%d\n"
			"rule->action.qos_action.alt_trafficclass=%d\n"
			"rule->action.fwd_action.processpath=%d\n"
			"rule->action.qos_action.flowid=%d\n", __func__, __LINE__,
			p_item->ingress_group, rule->action.qos_action.alt_trafficclass,
			rule->action.fwd_action.processpath,
			rule->action.qos_action.flowid);
	ppa_qos_inggrp_free_item(p_item); /* decrement reference counter */

	return PPA_SUCCESS;
}

int32_t ppa_ioctl_set_qos_ingress_group(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	QOS_INGGRP_LIST_ITEM *p_item;
	int res = PPA_FAILURE;

	res = copy_from_user(&cmd_info->qos_inggrp_info, (void *)arg, sizeof(cmd_info->qos_inggrp_info));
	if (res != PPA_SUCCESS)
		return PPA_FAILURE;

	if (qosal_set_qos_inggrp(&cmd_info->qos_inggrp_info, &p_item) != PPA_SUCCESS)
		return PPA_FAILURE;

	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_ioctl_set_qos_ingress_group);

int32_t ppa_ioctl_get_qos_ingress_group(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	/* Place Holder */
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_ioctl_get_qos_ingress_group);

#endif/*CONFIG_SOC_GRX500*/

/*
############# SUPPORTING INGRESS FUNCTION DEFINITIONS : END################
*/

/*
############# SUPPORTING WMM FUNCTION DEFINITIONS : START################
*/
#if IS_ENABLED(WMM_QOS_CONFIG)

int32_t ppa_qos_create_c2p_map_for_wmm(PPA_IFNAME ifname[16],uint8_t c2p[])
{
	int32_t ret = PPA_ENOTAVAIL,i,j=0;
	QOS_QUEUE_LIST_ITEM *p;

	ppa_memset(&c2p[0],0,16);
	ppa_qos_queue_lock_list();
	for ( p = g_qos_queue; p; p = p->next ) {
		if ( strcmp(p->ifname,ifname) == 0) {
			ret = PPA_SUCCESS;
			for(i=0;i< p->tc_no;i++) {
				c2p[(int)(p->tc_map[i])] = 8 - p->priority;
			}
			j++;
		}
	}
	ppa_qos_queue_unlock_list();
	if(j <= 1) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"%s %s %d num. Queues <= 1: Return PPA_ENOTAVAIL to set default Map !!!\n", __FILE__,__FUNCTION__,__LINE__);
		ret = PPA_ENOTAVAIL;
	}
	
	return ret;
}
EXPORT_SYMBOL(ppa_qos_create_c2p_map_for_wmm);

static int32_t ppa_set_wlan_wmm_prio(PPA_IFNAME *ifname,int32_t port_id,int8_t caller_flag)
{
	PPA_NETIF *netif = NULL;
	dp_subif_t *dp_subif = NULL;
	uint8_t *class2prio;
	uint8_t c2p[16],cl2p[16] = {0};
	int32_t i;
	int8_t port = 0;

	if(ifname == NULL) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ifname value is NULL \n");
		return PPA_FAILURE;
	}

	netif = ppa_get_netif(ifname);
	ppa_memset(&c2p[0],0,16);
	switch (caller_flag) {
		case 1:
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d caller_case is %d!!!\n", __FILE__,
					__FUNCTION__,__LINE__,caller_flag);
			if(!strcmp(ifname,"wlan0"))
				port=0;
			else if(!strcmp(ifname,"wlan1"))
				port=1;
			else if(!strcmp(ifname,"wlan2"))
				port=2;
			else if(!strcmp(ifname,"wlan3"))
				port=3;
			if(port >= 0) {
				for(i=0;i<MAX_TC_NUM;i++) {
					c2p[i] = g_eth_class_prio_map[port][i];
				}
			}
			class2prio = &c2p[0];
			ppa_call_class2prio_notifiers(PPA_CLASS2PRIO_DEFAULT,
						      port_id, netif,
						      class2prio);
			break;
		case 2:
		case 3:
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d caller_case is %d!!!\n", __FILE__,
					__FUNCTION__,__LINE__,caller_flag);

			if (ppa_if_is_br2684(netif, NULL)) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d called for ATM interface!!!\n",
						__FILE__,__FUNCTION__,__LINE__);
				return PPA_FAILURE;
            }

			PPA_ALLOC(dp_subif_t, dp_subif);
			ppa_memset(dp_subif, 0, sizeof(dp_subif_t));
			if (dp_get_netif_subifid(netif, NULL, NULL, NULL,
						dp_subif, 0) != DP_SUCCESS) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d dp_get_netif_subifid failed!!!\n",
					__FILE__,__FUNCTION__,__LINE__);
				ppa_free(dp_subif);
				return PPA_FAILURE;
			}

			if (!(dp_subif->alloc_flag & DP_F_FAST_WLAN) || (dp_subif->alloc_flag & DP_F_FAST_DSL)) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d not a fastpath wave interface!!!\n",
					__FILE__,__FUNCTION__,__LINE__);
				ppa_free(dp_subif);
				return PPA_FAILURE;
			}

			if(ppa_qos_create_c2p_map_for_wmm(ifname,cl2p) == PPA_ENOTAVAIL) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d Setting default Map!!!\n",
					__FILE__,__FUNCTION__,__LINE__);
				if(!strcmp(ifname,"wlan0"))
					port=0;
				else if(!strcmp(ifname,"wlan1"))
					port=1;
				else if(!strcmp(ifname,"wlan2"))
					port=2;
				else if(!strcmp(ifname,"wlan3"))
					port=3;
				if(port >= 0) {
					for(i=0;i<MAX_TC_NUM;i++) {
						cl2p[i] = g_eth_class_prio_map[port][i];
					}
				}
			}
			class2prio = cl2p;
			ppa_call_class2prio_notifiers(PPA_CLASS2PRIO_CHANGE,
						      dp_subif->port_id, netif,
						      class2prio);
			ppa_free(dp_subif);
			break;
		default:
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Invalid Wmm caller case \n");
			break;
	}

	return PPA_SUCCESS;
}
#endif/*WMM_QOS_CONFIG*/

/*
############# SUPPORTING WMM FUNCTION DEFINITIONS : END################
*/

/*
############# INIT FUNCTION DEFINITIONS : START################
*/

int32_t ppa_qos_init_cfg( uint32_t flags, uint32_t hal_id)
{
	int32_t ret=PPA_SUCCESS;
	if ( (ret = ppa_hsel_init_qos_cfg( 0, hal_id) ) == PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d qos init success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}

int32_t ppa_qos_uninit_cfg( uint32_t flags, uint32_t hal_id)
{
	int32_t ret=PPA_SUCCESS;
	if ( (ret = ppa_hsel_uninit_qos_cfg( 0, hal_id) ) == PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d qos uninit success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}

int32_t qosal_eng_init_cfg(void)
{
	int32_t ret = PPA_SUCCESS;
	PPA_HSEL_CAP_NODE *caps_list=NULL;

	uint32_t num_caps = 0, i, j;
	uint8_t f_more_hals = 0;
	
	caps_list = (PPA_HSEL_CAP_NODE*) ppa_malloc (sizeof(PPA_HSEL_CAP_NODE));
	if(!caps_list) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"alloc caps_list item failed \n" ); 
	return PPA_FAILURE;
	}
	ppa_memset(caps_list, 0 , (sizeof(PPA_HSEL_CAP_NODE)));
	caps_list[num_caps++].cap = QOS_INIT;
	
	if(ppa_select_hals_from_caplist(0, num_caps, caps_list) != PPA_SUCCESS) {
		ppa_free(caps_list);
		return PPA_FAILURE;
	}
	for(i = 0; i < num_caps;) { 
		f_more_hals = 0;
		j = ppa_group_hals_in_capslist(i, num_caps, caps_list);
	
		//* Based on the capability of first entry in the list we can decide the action*/
		switch(caps_list[i].cap)  {
			case QOS_INIT: {
				if( caps_list[i].hal_id == PPE_HAL )
					break;
				else {
					ret = ppa_qos_init_cfg(0,caps_list[i].hal_id);
				}
				break;
			}
			default:
				break;
		}
		i+=j;
	}

	ppa_free(caps_list);
	return ret;		
}

int32_t qosal_eng_uninit_cfg(void)
{
	int32_t ret = PPA_SUCCESS;
	PPA_HSEL_CAP_NODE *caps_list=NULL;

	uint32_t num_caps = 0, i, j;
	uint8_t f_more_hals = 0;
	
	caps_list = (PPA_HSEL_CAP_NODE*) ppa_malloc (sizeof(PPA_HSEL_CAP_NODE));
	if(!caps_list) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"alloc caps_list item failed \n" ); 
	return PPA_FAILURE;
	}
	ppa_memset(caps_list , 0, (sizeof(PPA_HSEL_CAP_NODE)));
	caps_list[num_caps++].cap = QOS_UNINIT;
	
	if(ppa_select_hals_from_caplist(0, num_caps, caps_list) != PPA_SUCCESS) {
		ppa_free(caps_list);
		return PPA_FAILURE;
	}
	for(i = 0; i < num_caps;) { 
		f_more_hals = 0;
		j = ppa_group_hals_in_capslist(i, num_caps, caps_list);
	
		/* Based on the capability of first entry in the list we can decide the action*/
		switch(caps_list[i].cap) {
			case QOS_UNINIT: {
				if( caps_list[i].hal_id == PPE_HAL )
					break;
				else {
					ret = ppa_qos_uninit_cfg(0,caps_list[i].hal_id);
				}
				break;
			}
			default:
				break;
		}
		i+=j;
	}
	ppa_free(caps_list);
	return ret;		
}

#if IS_ENABLED(CONFIG_SOC_GRX500)
int32_t ppa_qos_dscp_class_set( uint32_t flags, uint32_t hal_id)
{
	int32_t ret=PPA_SUCCESS;
	if ( (ret = ppa_hsel_dscp_class_set( 0, hal_id) ) == PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d qos dscp class map set success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}

int32_t ppa_qos_dscp_class_reset( uint32_t flags, uint32_t hal_id)
{
	int32_t ret=PPA_SUCCESS;
	if ( (ret = ppa_hsel_dscp_class_reset( 0, hal_id) ) == PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d qos dscp class map reset success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}

int32_t qosal_dscp_class_set(uint32_t flags)
{
	int32_t ret = PPA_SUCCESS;
	PPA_HSEL_CAP_NODE *caps_list = NULL;

	uint32_t num_caps = 0, i, j;
	uint8_t f_more_hals = 0;

	caps_list = (PPA_HSEL_CAP_NODE*) ppa_malloc (sizeof(PPA_HSEL_CAP_NODE));
	if (!caps_list) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"alloc caps_list item failed!\n" );
		return PPA_FAILURE;
	}
	ppa_memset(caps_list , 0, (sizeof(PPA_HSEL_CAP_NODE)));
	caps_list[num_caps++].cap = WMM_QOS_DSCP_MAP;

	if (ppa_select_hals_from_caplist(0, num_caps, caps_list) != PPA_SUCCESS) {
		ppa_free(caps_list);
		return PPA_FAILURE;
	}
	for (i = 0; i < num_caps;) {
		f_more_hals = 0;
		j = ppa_group_hals_in_capslist(i, num_caps, caps_list);

		/* Based on the capability of first entry in the list we can decide the action*/
		switch(caps_list[i].cap) {
			case WMM_QOS_DSCP_MAP: {
				if ( caps_list[i].hal_id == PPE_HAL )
					break;
				else {
					if (flags)
						ret = ppa_qos_dscp_class_set(0,caps_list[i].hal_id);
					else
						ret = ppa_qos_dscp_class_reset(0,caps_list[i].hal_id);
				}
				break;
			}
			default:
				break;
		}
		i+=j;
	}
	ppa_free(caps_list);
	return ret;
}
#endif

#if IS_ENABLED(CONFIG_SOC_GRX500)
int32_t qosal_set_gswr(uint32_t cmd,void *cfg)
{
	GSW_API_HANDLE gswr_handle = 0;
	GSW_QoS_queuePort_t *qos_queueport_set;
	GSW_QoS_SVLAN_ClassPCP_PortCfg_t *qos_classpcp;
	GSW_QoS_portRemarkingCfg_t *qos_remark_cfg;
	int32_t ret = 0;

	gswr_handle = gsw_api_kopen("/dev/switch_api/1");

	switch(cmd) {
		case GSW_QOS_QUEUE_PORT_SET: {
			qos_queueport_set = (GSW_QoS_queuePort_t *)cfg;
			ret =	gsw_api_kioctl(gswr_handle, GSW_QOS_QUEUE_PORT_SET, qos_queueport_set);
			break;
		}
		case GSW_QOS_PORT_REMARKING_CFG_SET: {
			qos_remark_cfg = (GSW_QoS_portRemarkingCfg_t *)cfg;
			ret = gsw_api_kioctl(gswr_handle, GSW_QOS_PORT_REMARKING_CFG_SET, qos_remark_cfg);
			break;
		}
		case GSW_QOS_SVLAN_CLASS_PCP_PORT_SET: {
			qos_classpcp = (GSW_QoS_SVLAN_ClassPCP_PortCfg_t *)cfg;
			ret = gsw_api_kioctl(gswr_handle, GSW_QOS_SVLAN_CLASS_PCP_PORT_SET, qos_classpcp);
			break;
		}
		case GSW_QOS_SVLAN_CLASS_PCP_PORT_GET: {
			qos_classpcp = (GSW_QoS_SVLAN_ClassPCP_PortCfg_t *)cfg;
			ret = gsw_api_kioctl(gswr_handle, GSW_QOS_SVLAN_CLASS_PCP_PORT_GET, qos_classpcp);
			break;
		}
		default:
			break;
	}
	if(ret < GSW_statusOk)
		pr_err(" gsw_api_kioctl returned failure \n");
	gsw_api_kclose(gswr_handle);

	return ret;
}

int32_t qosal_set_gswl(uint32_t cmd,void *cfg)
{
	GSW_API_HANDLE gswl_handle = 0;
	GSW_QoS_schedulerCfg_t *qos_schcfg_set;
	GSW_QoS_DSCP_ClassCfg_t *qos_dscp_class_set;
	GSW_QoS_portCfg_t *qos_portcfg_set;
	GSW_portCfg_t *portcfg_set;
	GSW_QoS_queuePort_t *qos_queueport_set;
	GSW_QoS_WRED_QueueCfg_t *qos_wred_set;
	GSW_QoS_FlowCtrlPortCfg_t *qos_flowctrl;
	GSW_register_t *regCfg;
	int32_t ret = 0;

	gswl_handle = gsw_api_kopen("/dev/switch_api/0");

	switch(cmd) {
		case GSW_QOS_SCHEDULER_CFG_SET: {
			qos_schcfg_set = (GSW_QoS_schedulerCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_SCHEDULER_CFG_SET, qos_schcfg_set);
			break;
		}
		case GSW_QOS_DSCP_CLASS_SET: {
			qos_dscp_class_set = (GSW_QoS_DSCP_ClassCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_DSCP_CLASS_SET, qos_dscp_class_set);
			break;
		}
		case GSW_QOS_PORT_CFG_SET: {
			qos_portcfg_set = (GSW_QoS_portCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_PORT_CFG_SET, qos_portcfg_set);
			break;
		}
		case GSW_QOS_QUEUE_PORT_SET: {
			qos_queueport_set = (GSW_QoS_queuePort_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_QUEUE_PORT_SET, qos_queueport_set);
			break;
		}
		case GSW_PORT_CFG_SET: {
			portcfg_set = (GSW_portCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_PORT_CFG_SET, portcfg_set);
			break;
		}
		case GSW_QOS_WRED_QUEUE_CFG_SET: {
			qos_wred_set = (GSW_QoS_WRED_QueueCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_WRED_QUEUE_CFG_SET, qos_wred_set);
			break;
		}
		case GSW_QOS_FLOWCTRL_PORT_CFG_SET: {
			qos_flowctrl = (GSW_QoS_FlowCtrlPortCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_FLOWCTRL_PORT_CFG_SET, qos_flowctrl);
			break;
		}
		case GSW_REGISTER_SET: {
			regCfg = (GSW_register_t *)cfg;
			ret = gsw_api_kioctl(gswl_handle, GSW_REGISTER_SET, regCfg);
			break;
		}
		default:
			break;
	}

	if(ret < GSW_statusOk)
		pr_err(" gsw_api_kioctl returned failure \n");
	gsw_api_kclose(gswl_handle);
	return ret;
}
#endif/* CONFIG_SOC_GRX500 */
int32_t ppa_ioctl_qos_init_cfg(unsigned int cmd)
{
	int32_t ret = PPA_SUCCESS;

	switch ( cmd ) {
		case PPA_CMD_ENG_QUEUE_INIT: {
			ret = qosal_eng_init_cfg();
			break;
		}
		case PPA_CMD_ENG_QUEUE_UNINIT: {
			ret = qosal_eng_uninit_cfg();
			break;
		}
#if IS_ENABLED(CONFIG_SOC_GRX500)
		case PPA_CMD_QOS_DSCP_CLASS_SET: {
			ret = qosal_dscp_class_set(1);
			break;
		}
		case PPA_CMD_QOS_DSCP_CLASS_RESET: {
			ret = qosal_dscp_class_set(0);
			break;
		}
#endif
		default: {
			break;
		}
	}

	return ret;
}
/*
############# INIT FUNCTION DEFINITIONS : END################
*/

/*
############# QUEUE FUNCTION DEFINITIONS : START################
*/
int32_t ppa_modify_qos_queue( char *ifname, PPA_QOS_MOD_QUEUE_CFG *q, uint32_t flags, uint32_t hal_id)
{
	int32_t ret=PPA_SUCCESS;
	QOS_Q_MOD_CFG tmu_q;
	memset(&tmu_q,0x00,sizeof(QOS_Q_MOD_CFG));
	tmu_q.ifname = ifname;
	tmu_q.portid = q->portid;
	tmu_q.priority = q->priority;
	tmu_q.qlen = q->qlen;
	tmu_q.q_type = q->q_type;
	tmu_q.weight = q->weight;
	if(q->drop.mode == PPA_QOS_DROP_RED) {
		tmu_q.drop.mode = PPA_QOS_DROP_RED;
		tmu_q.drop.wred.min_th0 = q->drop.wred.min_th0;
		tmu_q.drop.wred.max_th0 = q->drop.wred.max_th0;
		tmu_q.drop.wred.max_p0 = q->drop.wred.max_p0;
	} else if(q->drop.mode == PPA_QOS_DROP_WRED) {
		tmu_q.drop.mode = PPA_QOS_DROP_WRED;
		tmu_q.drop.wred.weight = q->drop.wred.weight;
		tmu_q.drop.wred.min_th0 = q->drop.wred.min_th0;
		tmu_q.drop.wred.max_th0 = q->drop.wred.max_th0;
		tmu_q.drop.wred.max_p0 = q->drop.wred.max_p0;
		tmu_q.drop.wred.min_th1 = q->drop.wred.min_th1;
		tmu_q.drop.wred.max_th1 = q->drop.wred.max_th1;
		tmu_q.drop.wred.max_p1 = q->drop.wred.max_p1;
	} else {
		tmu_q.drop.mode = PPA_QOS_DROP_TAIL;
#if IS_ENABLED(CONFIG_SOC_GRX500)
		tmu_q.drop.wred.max_th0 = (q->drop.flags & PPA_QOS_DT_F_GREEN_THRESHOLD) ? q->drop.wred.max_th0:
					((flags & PPA_QOS_Q_F_DEFAULT) ? 3 * TMU_GREEN_DEFAULT_THRESHOLD : TMU_GREEN_DEFAULT_THRESHOLD);
		tmu_q.drop.wred.max_th1 = (q->drop.flags & PPA_QOS_DT_F_YELLOW_THRESHOLD) ? q->drop.wred.max_th1: TMU_GREEN_DEFAULT_THRESHOLD;
		tmu_q.drop.wred.drop_th1 = (q->drop.flags & PPA_QOS_DT_F_RED_THRESHOLD) ? q->drop.wred.drop_th1: 0;
		tmu_q.drop.flags |= q->drop.flags;
#endif /* CONFIG_SOC_GRX500*/
	}

	tmu_q.flags = q->flags;
	tmu_q.q_id = q->queue_id;
	
	if ( (ret = ppa_hsel_modify_qos_queue_entry( &tmu_q, 0, hal_id) ) == PPA_SUCCESS ){
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d modify success!!!\n", __FILE__,__FUNCTION__,__LINE__);
		q->queue_id = tmu_q.q_id;
	} else
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d modify failure!!!\n", __FILE__,__FUNCTION__,__LINE__);
	return ret;
}

QOS_QUEUE_LIST_ITEM *ppa_qos_queue_alloc_item(void)
{
	QOS_QUEUE_LIST_ITEM *obj;

	obj = (QOS_QUEUE_LIST_ITEM *)ppa_malloc(sizeof(*obj));
	if (obj) {
		ppa_memset(obj, 0, sizeof(*obj));
		ppa_atomic_set(&obj->count, 1);
	}
	return obj;
}

void ppa_qos_queue_free_item(QOS_QUEUE_LIST_ITEM *obj)
{
	if (ppa_atomic_dec(&obj->count) == 0)
		ppa_free(obj);
}

void ppa_qos_queue_lock_list(void)
{
	ppa_lock_get(&g_qos_queue_lock);
}

void ppa_qos_queue_unlock_list(void)
{
	ppa_lock_release(&g_qos_queue_lock);
}

void __ppa_qos_queue_add_item(QOS_QUEUE_LIST_ITEM *obj)
{
	ppa_atomic_inc(&obj->count);
	obj->next = g_qos_queue;
	g_qos_queue = obj;
}

void ppa_qos_queue_remove_item(int32_t q_num, PPA_IFNAME ifname[16], QOS_QUEUE_LIST_ITEM **pp_info)
{
	QOS_QUEUE_LIST_ITEM *p_prev, *p_cur;

	if (pp_info)
		*pp_info = NULL;
	p_prev = NULL;
	ppa_qos_queue_lock_list();
	for (p_cur = g_qos_queue; p_cur; p_prev = p_cur, p_cur = p_cur->next)
		if ((p_cur->queue_num == q_num) && (strcmp(p_cur->ifname, ifname) == 0)) {
			if (!p_prev)
				g_qos_queue = p_cur->next;
			else
				p_prev->next = p_cur->next;
			if (pp_info)
				*pp_info = p_cur;
			else
				ppa_qos_queue_free_item(p_cur);
			break;
		}
	ppa_qos_queue_unlock_list();
}

void ppa_qos_queue_free_list(void)
{
	QOS_QUEUE_LIST_ITEM *obj;

	ppa_qos_queue_lock_list();
	while (g_qos_queue) {
		obj = g_qos_queue;
		g_qos_queue = g_qos_queue->next;

		ppa_qos_queue_free_item(obj);
		obj = NULL;
	}
	ppa_qos_queue_unlock_list();
}

int32_t __ppa_qos_intfid_lookup(char *ifname, uint32_t *intfid)
{
	int32_t ret = PPA_ENOTAVAIL;
	QOS_QUEUE_LIST_ITEM *p;

	for (p = g_qos_queue; p; p = p->next)
		if ((strncmp(p->ifname, ifname, PPA_IF_NAME_SIZE) == 0)) {
			ret = PPA_SUCCESS;
			*intfid = p->intfId;
			break;
		}

	return ret;
}

int32_t ppa_qos_intfid_lookup(char *ifname, uint32_t *intfid)
{
	int32_t ret;
	ppa_qos_queue_lock_list();
	ret = __ppa_qos_intfid_lookup(ifname, intfid);
	ppa_qos_queue_unlock_list();

	return ret;
}

int32_t __ppa_qos_queue_lookup(int32_t q_num, PPA_IFNAME ifname[16], QOS_QUEUE_LIST_ITEM **pp_info)
{
	int32_t ret = PPA_ENOTAVAIL;
	QOS_QUEUE_LIST_ITEM *p;

	for (p = g_qos_queue; p; p = p->next) {
		if (p->ifname)
			ppa_debug(DBG_ENABLE_MASK_QOS,
				"queue_num:%d ifname : %s\n",
				p->queue_num, p->ifname);
		if ((p->queue_num == q_num) && (strcmp(p->ifname, ifname) == 0)) {
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

int32_t ppa_qos_queue_lookup(int32_t qnum, PPA_IFNAME ifname[16], QOS_QUEUE_LIST_ITEM **pp_info)
{
	int32_t ret;
	ppa_qos_queue_lock_list();
	ret = __ppa_qos_queue_lookup(qnum, ifname, pp_info);
	ppa_qos_queue_unlock_list();

	return ret;
}

int32_t ppa_ioctl_add_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
	QOS_QUEUE_LIST_ITEM *p_item;
	int res = PPA_FAILURE;

	res = copy_from_user(&cmd_info->qos_queue_info, (void *)arg, sizeof(cmd_info->qos_queue_info));
	if (res != PPA_SUCCESS)
		return PPA_FAILURE;

	if (qosal_add_qos_queue(&cmd_info->qos_queue_info,&p_item) != PPA_SUCCESS)
		return PPA_FAILURE;
#if IS_ENABLED(WMM_QOS_CONFIG)
	else
		ppa_set_wlan_wmm_prio(cmd_info->qos_queue_info.ifname,-1,2);
#endif/*WMM_QOS_CONFIG*/
	return PPA_SUCCESS;
}

int32_t ppa_ioctl_modify_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
	int32_t ret = PPA_SUCCESS;
	return ret;
}

int32_t ppa_ioctl_delete_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{

	QOS_QUEUE_LIST_ITEM *p_item;
	QOS_SHAPER_LIST_ITEM *p_s_item;
	int32_t ret = PPA_FAILURE;

	ret = copy_from_user( &cmd_info->qos_queue_info, (void *)arg, sizeof(cmd_info->qos_queue_info));
	if (ret != PPA_SUCCESS)
		return PPA_FAILURE;

#if IS_ENABLED(CONFIG_SOC_GRX500)
	/*
	 * Skip deleting the Queue when adjusting WFQ weights and modify weight
	 * parameters during ADD queue operation.
	 */
	if ((cmd_info->qos_queue_info.flags & PPA_QOS_Q_F_WFQ_UPDATE) == PPA_QOS_Q_F_WFQ_UPDATE)
		return PPA_SUCCESS;
#endif/*CONFIG_SOC_GRX500*/
	ret = ppa_qos_queue_lookup(cmd_info->qos_queue_info.queue_num,cmd_info->qos_queue_info.ifname,&p_item);
	if( ret == PPA_ENOTAVAIL ) {
		return PPA_FAILURE;
	} else {
#if IS_ENABLED(CONFIG_SOC_GRX500)
		if (ppa_set_ingress_qos_generic(cmd_info->qos_queue_info.ifname, &cmd_info->qos_queue_info.flags))
			return PPA_FAILURE;
#endif/*CONFIG_SOC_GRX500*/
		ret = qosal_delete_qos_queue(&cmd_info->qos_queue_info,p_item);
	}
	/* Delete Shaper assigned to the Queue when the Queue is deleted */
	if(ret == PPA_SUCCESS) {
		if(ppa_qos_shaper_lookup(cmd_info->qos_queue_info.shaper_num,
				cmd_info->qos_queue_info.ifname,&p_s_item) == PPA_SUCCESS) {
			ppa_qos_shaper_remove_item(p_s_item->shaperid,p_s_item->ifname,NULL);
			ppa_qos_shaper_free_item(p_s_item);
		}
	}
	ppa_qos_queue_remove_item(p_item->queue_num,p_item->ifname,NULL);
	ppa_qos_queue_free_item(p_item);
	if(ret == PPA_SUCCESS)
	{
#if IS_ENABLED(WMM_QOS_CONFIG)
		ppa_set_wlan_wmm_prio(cmd_info->qos_queue_info.ifname,-1,3);
#endif/*WMM_QOS_CONFIG*/
	}

	return ret;
}

int32_t ppa_get_pppoa_base_if( char *ifname, PPA_IFNAME base_ifname[PPA_IF_NAME_SIZE])
{
	uint32_t if_flags = 0;
	int32_t ret = PPA_SUCCESS;
	short vpi = 0;
	int vci = 0;
	struct netif_info *if_info;
	if ( ppa_netif_lookup(ifname, &if_info) == PPA_SUCCESS ) {
		if_flags = if_info->flags;
		if((if_flags & NETIF_PPPOATM) == NETIF_PPPOATM) {
			vpi = if_info->vcc->vpi;
			vci = if_info->vcc->vci;
		}
		ppa_netif_put(if_info);
	}
	if((if_flags & NETIF_PPPOATM) == NETIF_PPPOATM) {
		snprintf(base_ifname, PPA_IF_NAME_SIZE, "atm_%d%d",vpi,vci);
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT," %s:%d Pseudo PPPOA dev_name = %s\n",__FUNCTION__,__LINE__,base_ifname);
		ret = PPA_SUCCESS;
	}
	else
	ret = PPA_FAILURE;

	return ret;
}

int32_t ppa_modify_qos_subif_to_port(PPA_CMD_SUBIF_PORT_INFO *subif_port_info)
{
	int32_t ret=PPA_SUCCESS;
	QOS_MOD_SUBIF_PORT_CFG SubifPort_info;

	memset(&SubifPort_info, 0, sizeof(QOS_MOD_SUBIF_PORT_CFG));

	ppa_strncpy(SubifPort_info.ifname, subif_port_info->ifname, PPA_IF_NAME_SIZE);
	SubifPort_info.port_id = subif_port_info->port_id;
	SubifPort_info.priority_level = subif_port_info->priority_level;
	SubifPort_info.weight = subif_port_info->weight;
	SubifPort_info.flags = subif_port_info->flags;

	if ( (ret = ppa_hsel_mod_subif_port_cfg( &SubifPort_info, 0, TMU_HAL) ) != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d hal select Mod subif to port failed!!!\n",
				__FILE__,__FUNCTION__,__LINE__);
		return PPA_FAILURE;
	}
	return ret;
}

int32_t ppa_ioctl_mod_subif_port_config(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
	int res = PPA_FAILURE;

	res = copy_from_user( &cmd_info->subif_port_info, (void *)arg, sizeof(cmd_info->subif_port_info));
	if (res != PPA_SUCCESS)
		return PPA_FAILURE;

	if( ppa_modify_qos_subif_to_port(&cmd_info->subif_port_info)!= PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d Mod subif to port failed!!!\n", __FILE__,__FUNCTION__,__LINE__);
		return PPA_FAILURE;
	}

	return PPA_SUCCESS;
}

int32_t ppa_add_qos_queue(char *ifname, PPA_QOS_ADD_QUEUE_CFG *q, uint32_t flags, uint32_t hal_id)
{
	int32_t ret = PPA_SUCCESS;
	int32_t k;
	char dev_name[PPA_IF_NAME_SIZE];
	QOS_Q_ADD_CFG tmu_q;

	memset(&tmu_q, 0x00, sizeof(QOS_Q_ADD_CFG));

	tmu_q.intfId_en = q->intfId_en;
	if (q->intfId_en)
		tmu_q.intfId = q->intfId;

	if (ppa_get_pppoa_base_if(ifname, dev_name) == PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, " %s:%d Pseudo PPPOA interface = %s\n", __FUNCTION__, __LINE__, dev_name);
		tmu_q.dev_name = dev_name;
		tmu_q.ifname = ifname;
		tmu_q.flags |= PPA_F_PPPOATM;
	} else {
		tmu_q.ifname = ifname;
		tmu_q.dev_name = ifname;
	}

	tmu_q.portid = q->portid;
	tmu_q.priority = q->priority;
	tmu_q.q_type = q->q_type;
	tmu_q.weight = q->weight;
	tmu_q.flags |= flags;
	tmu_q.drop.mode = q->drop.mode;
	switch (q->drop.mode) {
		case PPA_QOS_DROP_TAIL:
			tmu_q.qlen = q->qlen;
#if IS_ENABLED(CONFIG_SOC_GRX500)
			tmu_q.drop.wred.max_th0 = (q->drop.flags & PPA_QOS_DT_F_GREEN_THRESHOLD) ? q->drop.wred.max_th0:
						((flags & PPA_QOS_Q_F_DEFAULT) ? 3 * TMU_GREEN_DEFAULT_THRESHOLD : TMU_GREEN_DEFAULT_THRESHOLD);
			tmu_q.drop.wred.max_th1 = (q->drop.flags & PPA_QOS_DT_F_YELLOW_THRESHOLD) ? q->drop.wred.max_th1: TMU_GREEN_DEFAULT_THRESHOLD;
			tmu_q.drop.wred.drop_th1 = (q->drop.flags & PPA_QOS_DT_F_RED_THRESHOLD) ? q->drop.wred.drop_th1: 0;
			tmu_q.drop.flags |= q->drop.flags;
#endif /* CONFIG_SOC_GRX500*/
			break;
		case PPA_QOS_DROP_RED:
			break;
		case PPA_QOS_DROP_WRED:
			tmu_q.drop.wred.weight = q->drop.wred.weight;
			tmu_q.drop.wred.min_th0 = q->drop.wred.min_th0;
			tmu_q.drop.wred.min_th1 = q->drop.wred.min_th1;
			tmu_q.drop.wred.max_th0 = q->drop.wred.max_th0;
			tmu_q.drop.wred.max_th1 = q->drop.wred.max_th1;
			tmu_q.drop.wred.max_p0 = q->drop.wred.max_p0;
			tmu_q.drop.wred.max_p1 = q->drop.wred.max_p1;
			break;
		case PPA_QOS_DROP_CODEL:
			break;
		default:
			tmu_q.qlen = q->qlen;
			break;
	}

	for (k = 0; k < MAX_TC_NUM; k++)
		tmu_q.tc_map[k] = q->tc_map[k];

	tmu_q.tc_no = q->tc_no;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	if ((flags & PPA_QOS_Q_F_WFQ_UPDATE) == PPA_QOS_Q_F_WFQ_UPDATE)
		tmu_q.q_id = q->queue_id;
	if ((flags & PPA_QOS_Q_F_VAP_QOS) == PPA_QOS_Q_F_VAP_QOS) {
		PPA_NETIF *netif = NULL;
		dp_subif_t *dp_subif = NULL;
		PPA_ALLOC(dp_subif_t, dp_subif);
		ppa_memset(dp_subif, 0, sizeof(dp_subif_t));
		netif = ppa_get_netif(ifname);
		if (dp_get_netif_subifid(netif, NULL, NULL, NULL, dp_subif, 0) == PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%d vap qos dp port %d\n",__LINE__, dp_subif->port_id);
			ppa_drv_mpe_set_checksum_queue_map(dp_subif->port_id, NETIF_VAP_QOS);
		}
		ppa_free(dp_subif);
	}
#endif/*CONFIG_SOC_GRX500*/

	ret = ppa_hsel_add_qos_queue_entry(&tmu_q, 0, hal_id);
	if (ret == PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %s %d add success!!!\n", __FILE__, __FUNCTION__, __LINE__);
		q->queue_id = tmu_q.q_id;
	}
	return ret;
}

int32_t ppa_delete_qos_queue(char *ifname, char *dev_name, int32_t priority, uint32_t *queue_id, uint32_t portid,
			uint32_t hal_id, uint32_t flags)
{
	int32_t ret = PPA_SUCCESS;
#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	QOS_LOGICAL_IF_LIST_ITEM *p_logical_if_item;
	PPA_NETIF *netif = NULL;
	dp_subif_t *dp_subif = NULL;
	uint8_t	flowId_en=0; /*!<Enable/Disable for flow Id + tc bits used for VAP's & Vlan interfaces*/
	uint16_t flowId=0; /*!<flow Id + tc bits used for VAP's & Vlan interfaces*/
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/
	uint32_t infId;
	QOS_Q_DEL_CFG tmu_q;
	memset(&tmu_q, 0x00, sizeof(QOS_Q_DEL_CFG));

	/** For those netdevice which is not registered to DP, intfId is used 
	to store the cqm deq index which is passed by the caller while queue
	is added.
	*/
	if(ppa_qos_intfid_lookup(dev_name, &infId) == PPA_SUCCESS) {
		tmu_q.intfId = infId;
	}
#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	dp_subif = ppa_malloc(sizeof(dp_subif_t));
	if (!dp_subif)
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"[%s:%d] DP subif allocation failed\n",
			__func__, __LINE__);
	else {
		ppa_memset(dp_subif, 0, sizeof(dp_subif_t));
		netif = ppa_get_netif(ifname);
		if (dp_get_netif_subifid(netif, NULL, NULL, NULL, dp_subif, 0) == PPA_SUCCESS) {
			if (dp_subif->subif != 0) {
				/* lookup for the logical interface name */
				if (ppa_qos_logical_if_lookup(0, ifname, &p_logical_if_item) == PPA_SUCCESS) {
					flowId_en = p_logical_if_item->flowId_en;
					flowId = p_logical_if_item->flowId;
				}
			}
		}
		ppa_free(dp_subif);
	}
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/
	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, " %s:%d Pseudo PPPOA interface = %s, ifname = %s, flags = %d \n",
			__FUNCTION__, __LINE__, dev_name, ifname, flags);
	tmu_q.dev_name = dev_name;
	tmu_q.ifname = ifname;
	tmu_q.flags = flags;
	tmu_q.portid = portid;
	tmu_q.q_id = *queue_id;
	tmu_q.priority = priority;
	tmu_q.intfId = infId;
#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	tmu_q.intfId = flowId;
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/

	ret = ppa_hsel_delete_qos_queue_entry(&tmu_q, 0, hal_id);
	if (ret == PPA_SUCCESS)
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %s %d delete success!!!\n", __FILE__, __FUNCTION__, __LINE__);
	return ret;
}


static void qosal_dump_qinfo(PPA_CMD_QOS_QUEUE_INFO *q_info)
{
	pr_debug("================ q_info dump =======================\n");
	pr_debug("ifname:[%s]\n", q_info->ifname);
	pr_debug("tc_no:[0x%x]\n", q_info->tc_no);
	pr_debug("flowId_en:[0x%x]\n", q_info->flowId_en);
	pr_debug("flowId:[0x%x]\n", q_info->flowId);
	pr_debug("enable:[0x%x]\n", q_info->enable);
	pr_debug("weight:[0x%x]\n", q_info->weight);
	pr_debug("priority:[0x%x]\n", q_info->priority);
	pr_debug("qlen:[0x%x]\n", q_info->qlen);
	pr_debug("queue_num:[0x%x]\n", q_info->queue_num);
	pr_debug("shaper_num:[0x%x]\n", q_info->shaper_num);
	pr_debug("portid:[0x%x]\n", q_info->portid);
	pr_debug("sched:[0x%x]\n", q_info->sched);
	pr_debug("====================================================\n");
}

int32_t qosal_add_qos_queue(PPA_CMD_QOS_QUEUE_INFO *q_info, QOS_QUEUE_LIST_ITEM **pp_item)
{

	QOS_QUEUE_LIST_ITEM *p_item;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	QOS_QUEUE_LIST_ITEM *p_item_tmp;
#endif/*CONFIG_SOC_GRX500*/
	PPA_QOS_ADD_QUEUE_CFG add_q_cfg;
#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	QOS_LOGICAL_IF_LIST_ITEM *p_logical_if_item;
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/
	PPA_HSEL_CAP_NODE *caps_list = NULL;
	uint32_t num_caps = 0, i, j, ret = PPA_SUCCESS;
	uint32_t k;
	uint8_t f_more_hals = 0;
	char dev_name[PPA_IF_NAME_SIZE];

#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	PPA_NETIF *netif = NULL;
	dp_subif_t *dp_subif = NULL;
	int32_t flow_id_ret;
#endif

	qosal_dump_qinfo(q_info);

#if IS_ENABLED(CONFIG_SOC_GRX500)
	if (ppa_set_ingress_qos_generic(q_info->ifname, &q_info->flags))
		return PPA_FAILURE;
#endif

	memset(&add_q_cfg, 0x00, sizeof(PPA_QOS_ADD_QUEUE_CFG));
	p_item = ppa_qos_queue_alloc_item();
	if (!p_item) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "alloc queue list item failed \n");
		return PPA_ENOMEM;
	}
	ppa_strncpy(p_item->ifname, q_info->ifname, PPA_IF_NAME_SIZE);
	ppa_strncpy(p_item->dev_name, q_info->ifname, PPA_IF_NAME_SIZE);
	p_item->weight = q_info->weight;
	p_item->priority = q_info->priority;
	p_item->portid = q_info->portid;
	p_item->queue_num = q_info->queue_num;
	p_item->shaper_num = q_info->queue_num;
	p_item->drop.mode = q_info->drop.mode;
	switch (q_info->drop.mode) {
	case PPA_QOS_DROP_TAIL:
		p_item->qlen = q_info->qlen;
#if IS_ENABLED(CONFIG_SOC_GRX500)
		p_item->drop.wred.max_th0 = (q_info->drop.flags & PPA_QOS_DT_F_GREEN_THRESHOLD) ? q_info->drop.wred.max_th0:
			((q_info->flags & PPA_QOS_Q_F_DEFAULT) ? 3 * TMU_GREEN_DEFAULT_THRESHOLD : TMU_GREEN_DEFAULT_THRESHOLD);
		p_item->drop.wred.max_th1 = (q_info->drop.flags & PPA_QOS_DT_F_YELLOW_THRESHOLD) ? q_info->drop.wred.max_th1: TMU_GREEN_DEFAULT_THRESHOLD;
		p_item->drop.wred.drop_th1 = (q_info->drop.flags & PPA_QOS_DT_F_RED_THRESHOLD) ? q_info->drop.wred.drop_th1: 0;
		p_item->drop.flags |= q_info->drop.flags;
#endif /* CONFIG_SOC_GRX500*/
		break;
	case PPA_QOS_DROP_RED:
		break;
	case PPA_QOS_DROP_WRED:
		p_item->drop.wred.weight = q_info->drop.wred.weight;
		p_item->drop.wred.min_th0 = q_info->drop.wred.min_th0;
		p_item->drop.wred.min_th1 = q_info->drop.wred.min_th1;
		p_item->drop.wred.max_th0 = q_info->drop.wred.max_th0;
		p_item->drop.wred.max_th1 = q_info->drop.wred.max_th1;
		p_item->drop.wred.max_p0 = q_info->drop.wred.max_p0;
		p_item->drop.wred.max_p1 = q_info->drop.wred.max_p1;
		break;
	case PPA_QOS_DROP_CODEL:
		break;
	default:
		p_item->qlen = q_info->qlen;
		break;
	}
	for (k = 0; k < MAX_TC_NUM; k++)
		p_item->tc_map[k] = q_info->tc_map[k];

	p_item->tc_no = q_info->tc_no;
	p_item->intfId_en = q_info->flowId_en;
	p_item->intfId = q_info->flowId;

#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	dp_subif = ppa_malloc(sizeof(dp_subif_t));
	if (!dp_subif) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"[%s:%d] DP subif allocation failed\n",
			__func__, __LINE__);
		ppa_qos_queue_free_item(p_item);
		*pp_item = NULL;
		return PPA_ENOMEM;
	}
	ppa_memset(dp_subif, 0, sizeof(dp_subif_t));
	netif = ppa_get_netif(q_info->ifname);
	if (dp_get_netif_subifid(netif, NULL, NULL, NULL, dp_subif, 0) == PPA_SUCCESS) {
		if (dp_subif->subif != 0) {
			/* lookup for the logical interface name */
			if (ppa_qos_logical_if_lookup(0, q_info->ifname, &p_logical_if_item) == PPA_SUCCESS) {
				q_info->flowId_en = p_logical_if_item->flowId_en;
				q_info->flowId = p_logical_if_item->flowId;
			} else {
				p_logical_if_item = ppa_qos_logical_if_alloc_item();
				if (!p_logical_if_item) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "alloc logical interface list item failed \n");
					ppa_qos_queue_free_item(p_item);
					*pp_item = NULL;
					ppa_free(dp_subif);
					return PPA_ENOMEM;
				}
				snprintf(p_logical_if_item->logical_ifname, IFNAMSIZ, "%s", q_info->ifname);
				p_logical_if_item->port_id = dp_subif->port_id;
				p_logical_if_item->subif_id = dp_subif->subif;
				p_logical_if_item->flowId_en = 1;
				flow_id_ret = get_avl_flow_id(dp_subif->port_id);
				if (flow_id_ret == PPA_ENOTAVAIL) {
					ppa_qos_logical_if_free_item(p_logical_if_item);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "Available logical interfaces exceeds the limit \n");
					ppa_qos_queue_free_item(p_item);
					*pp_item = NULL;
					ppa_free(dp_subif);
					return PPA_FAILURE;
				} else {
					p_logical_if_item->flowId = flow_id_ret;
					q_info->flowId_en = p_logical_if_item->flowId_en;
					q_info->flowId = p_logical_if_item->flowId;
					__ppa_qos_logical_if_add_item(p_logical_if_item);
					p_item->intfId_en = q_info->flowId_en;
					p_item->intfId = q_info->flowId;
				}
			}
		}
	}
	ppa_free(dp_subif);
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/

	if (ppa_get_pppoa_base_if(p_item->ifname, dev_name) == PPA_SUCCESS) {
		ppa_strncpy(p_item->dev_name, dev_name, PPA_IF_NAME_SIZE);
		p_item->flags |= PPA_F_PPPOATM;
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, " %s:%d Pseudo PPPOA interface = %s : flags = %d\n", __FUNCTION__,
			__LINE__, dev_name, p_item->flags);
	}

	caps_list = (PPA_HSEL_CAP_NODE *) ppa_malloc (MAX_QOS_Q_CAPS*sizeof(PPA_HSEL_CAP_NODE));

	if (!caps_list) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "alloc caps_list item failed \n");
		ppa_qos_queue_free_item(p_item);
		*pp_item = NULL;
		return PPA_FAILURE;
	}

	ppa_memset(caps_list, 0, (MAX_QOS_Q_CAPS*sizeof(PPA_HSEL_CAP_NODE)));

	if (q_info->weight != 0)
		caps_list[num_caps++].cap = Q_SCH_WFQ;
	else
		caps_list[num_caps++].cap = Q_SCH_SP;

	switch (q_info->drop.mode) {
		case PPA_QOS_DROP_TAIL:
			caps_list[num_caps++].cap = Q_DROP_DT;
			break;
		case PPA_QOS_DROP_RED:
			caps_list[num_caps++].cap = Q_DROP_RED;
			break;
		case PPA_QOS_DROP_WRED:
			caps_list[num_caps++].cap = Q_DROP_WRED;
			break;
		case PPA_QOS_DROP_CODEL:
			break;
		default:
			caps_list[num_caps++].cap = Q_DROP_DT;
			break;
	}

	if (q_info->shaper.enable == 1)
		caps_list[num_caps++].cap = Q_SHAPER;

	if (q_info->flags & PPA_QOS_Q_F_INGRESS)
		caps_list[num_caps++].cap = QOS_INGGRP;

	if (ppa_select_hals_from_caplist(0, num_caps, caps_list) != PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_ERR, "%s:%d: ppa_select_hals_from_caplist failed\n", __func__, __LINE__);
		ppa_free(caps_list);
		ppa_qos_queue_free_item(p_item);
		*pp_item = NULL;
		return PPA_FAILURE;
	}
	for (i = 0; i < num_caps;) {
		f_more_hals = 0;
		j = ppa_group_hals_in_capslist(i, num_caps, caps_list);

		/* Based on the capability of first entry in the list we can decide the action */
		switch (caps_list[i].cap) {
			case Q_SCH_WFQ:
			case Q_SCH_SP:
			case Q_DROP_DT:
			case Q_DROP_WRED:
			case Q_DROP_RED:
			case Q_SHAPER:
			case QOS_QUEUE: {
				if (caps_list[i].hal_id == PPE_HAL)
					break;
				else {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "Hal selected !PPE \n");
					add_q_cfg.portid = q_info->portid;
					add_q_cfg.priority = q_info->priority;
					add_q_cfg.weight = q_info->weight;
					add_q_cfg.q_type = q_info->sched;
					for (k = 0; k < MAX_TC_NUM; k++)
						add_q_cfg.tc_map[k] = q_info->tc_map[k];

					add_q_cfg.tc_no = q_info->tc_no;
					add_q_cfg.drop.mode = q_info->drop.mode;
					switch (q_info->drop.mode) {
						case PPA_QOS_DROP_TAIL:
							add_q_cfg.qlen = q_info->qlen;
#if IS_ENABLED(CONFIG_SOC_GRX500)
							add_q_cfg.drop.wred.max_th0 = (q_info->drop.flags & PPA_QOS_DT_F_GREEN_THRESHOLD) ? q_info->drop.wred.max_th0:
										((q_info->flags & PPA_QOS_Q_F_DEFAULT) ? 3 * TMU_GREEN_DEFAULT_THRESHOLD : TMU_GREEN_DEFAULT_THRESHOLD);
							add_q_cfg.drop.wred.max_th1 = (q_info->drop.flags & PPA_QOS_DT_F_YELLOW_THRESHOLD) ? q_info->drop.wred.max_th1: TMU_GREEN_DEFAULT_THRESHOLD;
							add_q_cfg.drop.wred.drop_th1 = (q_info->drop.flags & PPA_QOS_DT_F_RED_THRESHOLD) ? q_info->drop.wred.drop_th1: 0;
							add_q_cfg.drop.flags |= q_info->drop.flags;
#endif /* CONFIG_SOC_GRX500*/
							break;
						case PPA_QOS_DROP_RED:
							break;
						case PPA_QOS_DROP_WRED:
							add_q_cfg.drop.wred.weight = q_info->drop.wred.weight;
							add_q_cfg.drop.wred.min_th0 = q_info->drop.wred.min_th0;
							add_q_cfg.drop.wred.min_th1 = q_info->drop.wred.min_th1;
							add_q_cfg.drop.wred.max_th0 = q_info->drop.wred.max_th0;
							add_q_cfg.drop.wred.max_th1 = q_info->drop.wred.max_th1;
							add_q_cfg.drop.wred.max_p0 = q_info->drop.wred.max_p0;
							add_q_cfg.drop.wred.max_p1 = q_info->drop.wred.max_p1;
							break;
						case PPA_QOS_DROP_CODEL:
							break;
						default:
							add_q_cfg.qlen = q_info->qlen;
							break;
					}
					if (q_info->flowId_en) {
						add_q_cfg.intfId_en = 1;
						add_q_cfg.intfId = q_info->flowId;
					}

#if IS_ENABLED(CONFIG_SOC_GRX500)
						if ((q_info->flags & PPA_QOS_Q_F_WFQ_UPDATE) == PPA_QOS_Q_F_WFQ_UPDATE) {
							ret = ppa_qos_queue_lookup(q_info->queue_num, q_info->ifname, &p_item_tmp);
							if (ret == PPA_ENOTAVAIL) {
								ppa_free(caps_list);
								ppa_qos_queue_free_item(p_item);
								*pp_item = NULL;
								return PPA_FAILURE;
							} else {
								add_q_cfg.queue_id = p_item_tmp->p_entry;
								ppa_qos_queue_remove_item(p_item_tmp->queue_num, p_item_tmp->ifname, NULL);
								ppa_qos_queue_free_item(p_item_tmp);
							}
						}
#endif/*CONFIG_SOC_GRX500*/
						ret = ppa_add_qos_queue(q_info->ifname, &add_q_cfg, q_info->flags, caps_list[i].hal_id);

				}
			}
			default:
				break;
		}
		i += j;
	}

	if (ret == PPA_SUCCESS) {
		p_item->p_entry = (add_q_cfg.queue_id);
		__ppa_qos_queue_add_item(p_item);
	} else {
		ppa_debug(DBG_ENABLE_MASK_ERR, "%s:%d: ppa_add_qos_queue failed\n", __func__, __LINE__);
		ppa_free(caps_list);
		ppa_qos_queue_free_item(p_item);
		*pp_item = NULL;
		return PPA_FAILURE;
	}

	p_item->caps_list = caps_list;
	p_item->num_caps = num_caps;
	*pp_item = p_item;
	return PPA_SUCCESS;
}

int32_t qosal_modify_qos_queue(PPA_CMD_QOS_QUEUE_INFO *q_info, QOS_QUEUE_LIST_ITEM **pp_item)
{
	int32_t ret = PPA_SUCCESS;
	QOS_QUEUE_LIST_ITEM *p_q_item;
	QOS_QUEUE_LIST_ITEM *p_q_item1;
	PPA_QOS_MOD_QUEUE_CFG mod_q_cfg;
	PPA_HSEL_CAP_NODE *caps_list = NULL;
	uint32_t *tmp;
	uint32_t num_caps = 0, i, j = 0;
	uint16_t f_more_hals;

	p_q_item1 = *pp_item;
	memset(&mod_q_cfg, 0x00, sizeof(PPA_QOS_MOD_QUEUE_CFG));
	/* Start of list del and add operation*/
	p_q_item = ppa_qos_queue_alloc_item();
	if (!p_q_item)
		goto UPDATE_FAILED;

	/* Update all info into p_q_item */
	ppa_strncpy(p_q_item->ifname, q_info->ifname, PPA_IF_NAME_SIZE);
	p_q_item->weight = q_info->weight;
	p_q_item->priority = q_info->priority;
	p_q_item->qlen = q_info->qlen;
	p_q_item->drop.enable = q_info->drop.enable;
	p_q_item->drop.mode = q_info->drop.mode;
	p_q_item->drop.wred.weight = q_info->drop.wred.weight;
	p_q_item->drop.wred.min_th0 = q_info->drop.wred.min_th0;
	p_q_item->drop.wred.max_th0 = q_info->drop.wred.max_th0;
	p_q_item->drop.wred.max_p0 = q_info->drop.wred.max_p0;
	p_q_item->drop.wred.min_th1 = q_info->drop.wred.min_th1;
	p_q_item->drop.wred.max_th1 = q_info->drop.wred.max_th1;
	p_q_item->drop.wred.max_p1 = q_info->drop.wred.max_p1;
	p_q_item->num_caps = p_q_item1->num_caps;
	p_q_item->caps_list = p_q_item1->caps_list;
	p_q_item->p_entry = p_q_item1->p_entry;
	p_q_item->queue_num = p_q_item1->queue_num;

	ppa_qos_queue_remove_item(p_q_item1->queue_num, p_q_item1->ifname, NULL);
	ppa_qos_queue_free_item(p_q_item1);

	__ppa_qos_queue_add_item(p_q_item);
	/* End of list del and add operation*/

	caps_list = (PPA_HSEL_CAP_NODE *)p_q_item->caps_list;
	num_caps = p_q_item->num_caps;
	if (!caps_list)
		return PPA_FAILURE;

	for (i = 0; i < num_caps;) {
		f_more_hals = 0;
		j = ppa_group_hals_in_capslist(i, num_caps, caps_list);

		/* Based on the capability of first entry in the list we can decide the action */
		switch (caps_list[i].cap) {
			case Q_SCH_WFQ:
			case Q_SCH_SP:
			case Q_DROP_DT:
			case Q_DROP_RED:
			case Q_DROP_WRED:
			case Q_SHAPER:
			case QOS_QUEUE: {
				if (caps_list[i].hal_id == PPE_HAL)
					break;
				else {
					mod_q_cfg.portid = q_info->portid;
					mod_q_cfg.priority = q_info->priority;
					mod_q_cfg.qlen = q_info->qlen;
					mod_q_cfg.q_type = q_info->sched;
					mod_q_cfg.weight = q_info->weight;
					if (q_info->drop.mode == PPA_QOS_DROP_RED) {
						mod_q_cfg.drop.mode = PPA_QOS_DROP_RED;
						mod_q_cfg.drop.wred.min_th0 = q_info->drop.wred.min_th0;
						mod_q_cfg.drop.wred.max_th0 = q_info->drop.wred.max_th0;
						mod_q_cfg.drop.wred.max_p0 = q_info->drop.wred.max_p0;
					} else if (q_info->drop.mode == PPA_QOS_DROP_WRED) {
						mod_q_cfg.drop.mode = PPA_QOS_DROP_WRED;
						mod_q_cfg.drop.wred.weight = q_info->drop.wred.weight;
						mod_q_cfg.drop.wred.min_th0 = q_info->drop.wred.min_th0;
						mod_q_cfg.drop.wred.max_th0 = q_info->drop.wred.max_th0;
						mod_q_cfg.drop.wred.max_p0 = q_info->drop.wred.max_p0;
						mod_q_cfg.drop.wred.min_th1 = q_info->drop.wred.min_th1;
						mod_q_cfg.drop.wred.max_th1 = q_info->drop.wred.max_th1;
						mod_q_cfg.drop.wred.max_p1 = q_info->drop.wred.max_p1;
					} else {
						mod_q_cfg.drop.mode = PPA_QOS_DROP_TAIL;
#if IS_ENABLED(CONFIG_SOC_GRX500)
						mod_q_cfg.drop.wred.max_th0 = (q_info->drop.flags & PPA_QOS_DT_F_GREEN_THRESHOLD) ? q_info->drop.wred.max_th0:
								((q_info->flags & PPA_QOS_Q_F_DEFAULT) ? 3 * TMU_GREEN_DEFAULT_THRESHOLD : TMU_GREEN_DEFAULT_THRESHOLD);
						mod_q_cfg.drop.wred.max_th1 = (q_info->drop.flags & PPA_QOS_DT_F_YELLOW_THRESHOLD) ? q_info->drop.wred.max_th1: TMU_GREEN_DEFAULT_THRESHOLD;
						mod_q_cfg.drop.wred.drop_th1 = (q_info->drop.flags & PPA_QOS_DT_F_RED_THRESHOLD) ? q_info->drop.wred.drop_th1: 0;
						mod_q_cfg.drop.flags |= q_info->drop.flags;
#endif /* CONFIG_SOC_GRX500*/
					}

						mod_q_cfg.flags = q_info->flags;
						mod_q_cfg.queue_id = p_q_item->p_entry;

						ret = ppa_modify_qos_queue(q_info->ifname, &mod_q_cfg, q_info->flags, caps_list[i].hal_id);
						if (ret != PPA_SUCCESS)
							ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_modify_qos_queue failed \n");

						if (ret == PPA_SUCCESS) {
							tmp = &mod_q_cfg.queue_id;
							p_q_item->p_entry = *tmp;
						}
				}
				break;
			}
			default:
				break;
		}
		i += j;
	}
	/* returned success in step 1 proceed */
	return ret;

UPDATE_FAILED:
	return PPA_FAILURE;

}

int32_t qosal_delete_qos_queue(PPA_CMD_QOS_QUEUE_INFO *q_info, QOS_QUEUE_LIST_ITEM *p_item)
{
	int32_t ret = PPA_SUCCESS;
	PPA_HSEL_CAP_NODE *caps_list = NULL;
	uint32_t numcap;
	uint32_t i = 0, j = 0;
#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	int32_t avl_q = 0;
	PPA_NETIF *netif = NULL;
	dp_subif_t *dp_subif = NULL;
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/

	if (!p_item->caps_list)
		return PPA_FAILURE;

#if IS_ENABLED(CONFIG_SOC_GRX500)
	if (ppa_set_ingress_qos_generic(q_info->ifname, &q_info->flags))
		return PPA_FAILURE;
#endif

	caps_list = (PPA_HSEL_CAP_NODE *)p_item->caps_list;
	numcap = p_item->num_caps;

	/* when init, these entry values are ~0, the max the number which can be detected by these functions */

	for (i = 0; i < numcap;) {
		if (caps_list[i].hal_id == PPE_HAL)
			break;
		else {
			ret = ppa_delete_qos_queue(q_info->ifname, p_item->dev_name, q_info->priority, &(p_item->p_entry),
					q_info->portid, caps_list[i].hal_id, (q_info->flags|p_item->flags));
			if (ret != PPA_SUCCESS)
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_delete_qos_queue failed \n");
#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
			dp_subif = ppa_malloc(sizeof(dp_subif_t));
			if (!dp_subif)
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
					"[%s:%d] DP subif allocation failed\n",
					__func__, __LINE__);
			else {
				ppa_memset(dp_subif, 0, sizeof(dp_subif_t));
				netif = ppa_get_netif(q_info->ifname);
				if (dp_get_netif_subifid(netif, NULL, NULL, NULL, dp_subif, 0) == PPA_SUCCESS) {
					if (dp_subif->subif != 0) {
						avl_q = get_avl_queues_via_ifname(q_info->ifname);
						if (avl_q == 0)
							ppa_qos_logical_if_remove_item(q_info->ifname, NULL);
					}
				}
				ppa_free(dp_subif);
			}
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/
		}
		j = ppa_group_hals_in_capslist(i, p_item->num_caps, (PPA_HSEL_CAP_NODE *)p_item->caps_list);
		i += j;
	}
	return ret;
}

#if IS_ENABLED(WMM_QOS_CONFIG)
int32_t ppa_ioctl_add_wmm_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
	QOS_QUEUE_LIST_ITEM *p_item;
	PPA_HSEL_CAP_NODE *caps_list=NULL;
	uint32_t num_caps = 0, i, j;
	uint8_t f_more_hals = 0;
	int res = PPA_FAILURE;

	res = copy_from_user( &cmd_info->qos_queue_info, (void *)arg, sizeof(cmd_info->qos_queue_info));
	if (res != PPA_SUCCESS)
		return PPA_FAILURE;

	caps_list = (PPA_HSEL_CAP_NODE*) ppa_malloc (MAX_QOS_Q_CAPS*sizeof(PPA_HSEL_CAP_NODE));

	if(!caps_list) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"alloc caps_list item failed \n" );
		return PPA_FAILURE;
	}

	ppa_memset(caps_list,0,(MAX_QOS_Q_CAPS*sizeof(PPA_HSEL_CAP_NODE)));

	caps_list[num_caps++].cap = QOS_WMM_INIT;
	if(ppa_select_hals_from_caplist(0, num_caps, caps_list) != PPA_SUCCESS) {
		ppa_free(caps_list);
		return PPA_FAILURE;
	}
	for(i = 0; i < num_caps;) {
		f_more_hals = 0;
		j = ppa_group_hals_in_capslist(i, num_caps, caps_list);
		/* Based on the capability of first entry in the list we can decide the action*/
		switch(caps_list[i].cap) {
			case QOS_WMM_INIT: {
				if( caps_list[i].hal_id == PPE_HAL )
					break;
				else {
					if (qosal_add_qos_queue(&cmd_info->qos_queue_info,&p_item) != PPA_SUCCESS) {
						ppa_free(caps_list);
						return PPA_FAILURE;
					}
					else
						ppa_set_wlan_wmm_prio(cmd_info->qos_queue_info.ifname,-1,2);
				}
			}
			break;
			default:
				break;
		}
		i+=j;
	}

	ppa_free(caps_list);
	return PPA_SUCCESS;
}
int32_t ppa_ioctl_delete_wmm_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{

	int32_t ret = PPA_SUCCESS;
	QOS_QUEUE_LIST_ITEM *p_item;
	QOS_SHAPER_LIST_ITEM *p_s_item;
	ret = copy_from_user( &cmd_info->qos_queue_info, (void *)arg, sizeof(cmd_info->qos_queue_info));
	if (ret != PPA_SUCCESS)
		return PPA_FAILURE;
	ret = ppa_qos_queue_lookup(cmd_info->qos_queue_info.queue_num,cmd_info->qos_queue_info.ifname,&p_item);
	if( ret == PPA_ENOTAVAIL )
		return PPA_FAILURE;
	else
		ret = qosal_delete_qos_queue(&cmd_info->qos_queue_info,p_item);
	/* Delete Shaper assigned to the Queue when the Queue is deleted */
	if(ret == PPA_SUCCESS) {
		if(ppa_qos_shaper_lookup(cmd_info->qos_queue_info.shaper_num,cmd_info->qos_queue_info.ifname,&p_s_item) == PPA_SUCCESS) {
			ppa_qos_shaper_remove_item(p_s_item->shaperid,p_s_item->ifname,NULL);
			ppa_qos_shaper_free_item(p_s_item);
		}
	}
	ppa_qos_queue_remove_item(p_item->queue_num,p_item->ifname,NULL);
	ppa_qos_queue_free_item(p_item);
	if(ret == PPA_SUCCESS) {
		ppa_set_wlan_wmm_prio(cmd_info->qos_queue_info.ifname,-1,3);
	}

	return ret;
}
#endif/*WMM_QOS_CONFIG*/

/*
############# QUEUE FUNCTION DEFINITIONS : END################
*/

/*
############# VAPQOS FUNCTION DEFINITIONS : START################
*/
#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
QOS_LOGICAL_IF_LIST_ITEM *ppa_qos_logical_if_alloc_item(void)
{
	QOS_LOGICAL_IF_LIST_ITEM *obj;

	obj = (QOS_LOGICAL_IF_LIST_ITEM *)ppa_malloc(sizeof(*obj));
	if (obj)
		ppa_memset(obj, 0, sizeof(*obj));

	return obj;
}

void ppa_qos_logical_if_free_item(QOS_LOGICAL_IF_LIST_ITEM *obj)
{
	ppa_free(obj);
}

void ppa_qos_logical_if_lock_list(void)
{
	ppa_lock_get(&g_qos_logical_if_lock);
}

void ppa_qos_logical_if_unlock_list(void)
{
	ppa_lock_release(&g_qos_logical_if_lock);
}

void __ppa_qos_logical_if_add_item(QOS_LOGICAL_IF_LIST_ITEM *obj)
{

	obj->next = g_qos_logical_if;
	g_qos_logical_if = obj;

}

void ppa_qos_logical_if_remove_item(PPA_IFNAME ifname[16], QOS_LOGICAL_IF_LIST_ITEM **pp_info)
{
	QOS_LOGICAL_IF_LIST_ITEM *p_prev, *p_cur;

	if (pp_info)
		*pp_info = NULL;
	p_prev = NULL;
	ppa_qos_logical_if_lock_list();
	for (p_cur = g_qos_logical_if; p_cur; p_prev = p_cur, p_cur = p_cur->next)
		if (strcmp(p_cur->logical_ifname, ifname) == 0) {
			if (!p_prev)
				g_qos_logical_if = p_cur->next;
			else
				p_prev->next = p_cur->next;
			if (pp_info)
				*pp_info = p_cur;
			else
				ppa_qos_logical_if_free_item(p_cur);
			break;
		}
	ppa_qos_logical_if_unlock_list();
}

void ppa_qos_logical_if_free_list(void)
{
	QOS_LOGICAL_IF_LIST_ITEM *obj;

	ppa_qos_logical_if_lock_list();
	while (g_qos_logical_if) {
		obj = g_qos_logical_if;
		g_qos_logical_if = g_qos_logical_if->next;

		ppa_qos_logical_if_free_item(obj);
		obj = NULL;
	}
	ppa_qos_logical_if_unlock_list();
}

int32_t __ppa_qos_logical_if_lookup(int32_t subifId, PPA_IFNAME ifname[16], QOS_LOGICAL_IF_LIST_ITEM **pp_info)
{
	int32_t ret = PPA_ENOTAVAIL;
	QOS_LOGICAL_IF_LIST_ITEM *p;

	subifId &= 0xF00;
	for (p = g_qos_logical_if; p; p = p->next) {
		if (!ifname) {
			if (p->subif_id == subifId) {
				ret = PPA_SUCCESS;
				if (pp_info)
					*pp_info = p;
				break;
			}
		} else if (strcmp(p->logical_ifname, ifname) == 0) {
			ret = PPA_SUCCESS;
			if (pp_info)
				*pp_info = p;
			break;	
		}
	}

	return ret;
}

int32_t ppa_qos_logical_if_lookup(int32_t subifId, PPA_IFNAME ifname[16], QOS_LOGICAL_IF_LIST_ITEM **pp_info)
{
	int32_t ret;
	ppa_qos_logical_if_lock_list();
	ret = __ppa_qos_logical_if_lookup(subifId, ifname, pp_info);
	ppa_qos_logical_if_unlock_list();

	return ret;
}

int32_t ppa_set_logical_if_qos_classifier(PPA_CLASS_RULE *rule)
{

	QOS_LOGICAL_IF_LIST_ITEM *p_logical_if_item;

	if (ppa_qos_logical_if_lookup(0, rule->tx_if, &p_logical_if_item) == PPA_SUCCESS) {
		rule->action.qos_action.flowid_enabled = 0;
		rule->action.fwd_action.processpath = 1;
		if (p_logical_if_item->flowId > 3) {
			rule->action.qos_action.alt_trafficclass |= 8;
			rule->action.qos_action.flowid = ((p_logical_if_item->flowId & 0x3) << 6);
		} else
			rule->action.qos_action.flowid = (p_logical_if_item->flowId << 6);
	} else {
		ppa_debug(DBG_ENABLE_MASK_ERR, "%s is NOT present in logical interface list\n", rule->tx_if);
		return PPA_FAILURE;
	}

	return PPA_SUCCESS;
}

int32_t get_avl_flow_id(int32_t port_id)
{
	QOS_LOGICAL_IF_LIST_ITEM *p;
	int32_t flowid_list[MAX_VAP_QOS_SUPPORT] = {0};
	int i;

	for (p = g_qos_logical_if; p; p = p->next) {
		if (p->port_id == port_id)
			flowid_list[p->flowId] = 1;
	}
	/* Reserve Flowid 00 combination for AP */
	for (i = 1; i < MAX_VAP_QOS_SUPPORT; i++) {
		if ((flowid_list[i] == 0) && (i !=4 ))
			return i;
	}

	return PPA_ENOTAVAIL;
}

int32_t get_avl_queues_via_ifname(char *ifname)
{
	int32_t i = 0;
	QOS_QUEUE_LIST_ITEM *p;

	for (p = g_qos_queue; p; p = p->next)
		if (strcmp(p->ifname, ifname) == 0)
			i++;

	return i;
}

#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/
/*
############# VAPQOS FUNCTION DEFINITIONS : END################
*/


/*
############# QOS GET FUNCTION DEFINITIONS : START################
*/


int32_t ppa_ioctl_get_qos_status(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
	int res = PPA_FAILURE;
	/* Try to return PPA_SUCCESS with ioctl, but make sure put real return value
	 * in internal variable.
	 */
	res = copy_from_user( &cmd_info->qos_status_info, (void *)arg, sizeof(cmd_info->qos_status_info));
	if (res != PPA_SUCCESS)
		return PPA_FAILURE;

	if( ppa_update_qos_mib(&cmd_info->qos_status_info.qstat, 1, cmd_info->qos_status_info.flags ) != PPA_SUCCESS ) {
		cmd_info->qos_status_info.qstat.res = PPA_FAILURE;
	}
	else
		cmd_info->qos_status_info.qstat.res = PPA_SUCCESS;

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_status_info, sizeof(cmd_info->qos_status_info)) != 0 )
		return PPA_FAILURE;

	return PPA_SUCCESS;
}

int32_t ppa_get_qos_qnum( uint32_t portid, uint32_t flag)
{
	PPA_QOS_COUNT_CFG	count={0};

	count.portid = portid;
	count.flags = flag;

	ppa_port_qnum[portid].portid = portid;
	if( ppa_drv_get_qos_qnum( &count, 0) != PPA_SUCCESS ) {
		count.num = -1;
		ppa_port_qnum[portid].num = count.num;
		ppa_port_qnum[portid].status = "N/A";
	} else {
		ppa_port_qnum[portid].num = count.num;
		ppa_port_qnum[portid].status = "PORT is active";
	}

	return count.num;
}

int32_t ppa_ioctl_get_qos_qnum(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
	int res = PPA_FAILURE;
	PPA_QOS_COUNT_CFG count={0};

	ppa_memset(&cmd_info->qnum_info, 0, sizeof(cmd_info->qnum_info) );
	res = copy_from_user( &cmd_info->qnum_info, (void *)arg, sizeof(cmd_info->qnum_info));
	if (res != PPA_SUCCESS)
		return PPA_FAILURE;
	count.portid = cmd_info->qnum_info.portid;
	res = ppa_drv_get_qos_qnum( &count, 0);
	cmd_info->qnum_info.queue_num = count.num;
	if ( ppa_copy_to_user( (void *)arg, &cmd_info->qnum_info, sizeof(cmd_info->qnum_info)) != 0 )
		return PPA_FAILURE;

	return res;
}

int32_t ppa_get_qos_mib( uint32_t portid, uint32_t queueid, PPA_QOS_MIB *mib, uint32_t flag)
{
	uint32_t res;
	PPA_QOS_MIB_INFO mib_info={0};

	mib_info.portid = portid;
	mib_info.queueid = queueid;
	mib_info.flag = flag;
	res = ppa_drv_get_qos_mib( &mib_info, 0);

	*mib = mib_info.mib;

	return res;
}

int32_t ppa_ioctl_get_qos_mib(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{

	int res = PPA_FAILURE;
	PPA_QOS_MIB_INFO mib={0};
	QOS_QUEUE_LIST_ITEM *p_item;

	ppa_memset(&cmd_info->qos_mib_info, 0, sizeof(cmd_info->qos_mib_info) );
	res = copy_from_user( &cmd_info->qos_mib_info, (void *)arg, sizeof(cmd_info->qos_mib_info));
	if (res != PPA_SUCCESS)
		return PPA_FAILURE;
	res = ppa_qos_queue_lookup(cmd_info->qos_mib_info.queue_num,cmd_info->qos_queue_info.ifname,&p_item);
	if( res == PPA_ENOTAVAIL ){
		return PPA_FAILURE;
	}

	mib.ifname = cmd_info->qos_mib_info.ifname;
	mib.queueid = p_item->p_entry;
	mib.portid = cmd_info->qos_mib_info.portid;
	mib.flag = cmd_info->qos_mib_info.flags;
	res = ppa_drv_get_qos_mib( &mib, 0);

	cmd_info->qos_mib_info.mib = mib.mib;

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_mib_info, sizeof(cmd_info->qos_mib_info)) != 0 )

		return PPA_FAILURE;

	return res;
}

/*
############# QOS GET FUNCTION DEFINITIONS : END################
*/

/*
############# LEGACY QOS FUNCTION DEFINITIONS : START################
*/
/*#if IS_ENABLED(CONFIG_PPA_QOS_WFQ)*/
int32_t ppa_set_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t weight_level, uint32_t flag )
{
	PPA_QOS_WFQ_CFG wfq={0};

	wfq.portid = portid;
	wfq.queueid = queueid;
	wfq.weight_level = weight_level;
	wfq.flag = flag;

	return ppa_drv_set_qos_wfq( &wfq, 0);
}
EXPORT_SYMBOL(ppa_set_qos_wfq);

int32_t ppa_get_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t *weight_level, uint32_t flag)
{
	PPA_QOS_WFQ_CFG wfq={0};
	int32_t res;

	wfq.portid = portid;
	wfq.queueid = queueid;
	wfq.flag = flag;

	res = ppa_drv_get_qos_wfq (&wfq, 0);

	if( weight_level ) *weight_level = wfq.weight_level;

	return res;
}
EXPORT_SYMBOL(ppa_get_qos_wfq);

int32_t ppa_reset_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t flag)
{
	PPA_QOS_WFQ_CFG cfg={0};

	cfg.portid = portid;
	cfg.queueid = queueid;
	cfg.flag = flag;
	return ppa_drv_reset_qos_wfq(&cfg, 0);
}
EXPORT_SYMBOL(ppa_reset_qos_wfq);

int32_t ppa_set_ctrl_qos_wfq(uint32_t portid,	uint32_t f_enable, uint32_t flag)
{
	int i;
	PPA_QOS_COUNT_CFG count={0};
	PPA_QOS_ENABLE_CFG enable_cfg={0};

	count.portid = portid;
	count.flags = flag;
	ppa_drv_get_qos_qnum( &count, 0);

	if( count.num <= 0 ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_set_ctrl_qos_wfq: count.num not valid (%d) to %s wfq on port %d\n",
			count.num, f_enable?"enable":"disable", portid );
		return PPA_FAILURE;
	}

	enable_cfg.portid = portid;
	enable_cfg.flag = flag;
	enable_cfg.f_enable = f_enable;
	ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_set_ctrl_qos_wfq to %s wfq on port %d\n", f_enable?"enable":"disable", portid );
	ppa_drv_set_ctrl_qos_wfq( &enable_cfg, 0);

	for( i=0; i<count.num; i++ )
		ppa_reset_qos_wfq( portid, i, 0);
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_set_ctrl_qos_wfq);
int32_t ppa_get_ctrl_qos_wfq(uint32_t portid,	uint32_t *f_enable, uint32_t flag)
{
	int32_t res = PPA_FAILURE;

	if( f_enable ) {
		PPA_QOS_ENABLE_CFG enable_cfg={0};

		enable_cfg.portid = portid;
		enable_cfg.flag = flag;

		res = ppa_drv_get_ctrl_qos_wfq(&enable_cfg, 0);

		if( f_enable ) *f_enable = enable_cfg.f_enable;
	}

	return res;
}
EXPORT_SYMBOL(ppa_get_ctrl_qos_wfq);

int32_t ppa_ioctl_set_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int32_t res;

	ppa_memset(&cmd_info->qos_ctrl_info, 0, sizeof(cmd_info->qos_ctrl_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_ctrl_info, (void *)arg, sizeof(cmd_info->qos_ctrl_info)) != 0 )
		return PPA_FAILURE;

	res = ppa_set_ctrl_qos_wfq(cmd_info->qos_ctrl_info.portid, cmd_info->qos_ctrl_info.enable, cmd_info->qos_ctrl_info.flags);
	if ( res != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_set_ctrl_qos_wfq fail\n");
		res = PPA_FAILURE;
	}

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_set_ctrl_qos_wfq);

int32_t ppa_ioctl_get_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = PPA_FAILURE;

	ppa_memset(&cmd_info->qos_ctrl_info, 0, sizeof(cmd_info->qos_ctrl_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_ctrl_info, (void *)arg, sizeof(cmd_info->qos_ctrl_info)) != 0 )
		return PPA_FAILURE;

	res = ppa_get_ctrl_qos_wfq(cmd_info->qos_ctrl_info.portid, &cmd_info->qos_ctrl_info.enable, cmd_info->qos_ctrl_info.flags);
	if ( res != PPA_SUCCESS )
	{
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_get_ctrl_qos_wfq fail\n");
		res = PPA_FAILURE;
	}

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_ctrl_info, sizeof(cmd_info->qos_ctrl_info)) != 0 )
		return PPA_FAILURE;

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_get_ctrl_qos_wfq);

int32_t ppa_ioctl_set_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int32_t res;

	ppa_memset(&cmd_info->qos_wfq_info, 0, sizeof(cmd_info->qos_wfq_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_wfq_info, (void *)arg, sizeof(cmd_info->qos_wfq_info)) != 0 )
		return PPA_FAILURE;

	res = ppa_set_qos_wfq(cmd_info->qos_wfq_info.portid, cmd_info->qos_wfq_info.queueid, cmd_info->qos_wfq_info.weight, cmd_info->qos_wfq_info.flags);
	if ( res != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_set_qos_wfq fail\n");
		res = PPA_FAILURE;
	}

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_set_qos_wfq);

int32_t ppa_ioctl_reset_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = PPA_FAILURE;

	ppa_memset(&cmd_info->qos_wfq_info, 0, sizeof(cmd_info->qos_wfq_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_wfq_info, (void *)arg, sizeof(cmd_info->qos_wfq_info)) != 0 )
		return PPA_FAILURE;

	res = ppa_reset_qos_wfq(cmd_info->qos_wfq_info.portid, cmd_info->qos_wfq_info.queueid, cmd_info->qos_wfq_info.flags);
	if ( res != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_reset_qos_wfq fail\n");
		res = PPA_FAILURE;
	}

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_reset_qos_wfq);

int32_t ppa_ioctl_get_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = PPA_FAILURE;

	ppa_memset(&cmd_info->qos_wfq_info, 0, sizeof(cmd_info->qos_wfq_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_wfq_info, (void *)arg, sizeof(cmd_info->qos_wfq_info)) != 0 )
		return PPA_FAILURE;

	res = ppa_get_qos_wfq(cmd_info->qos_wfq_info.portid, cmd_info->qos_wfq_info.queueid, &cmd_info->qos_wfq_info.weight, cmd_info->qos_wfq_info.flags);
	if ( res != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_get_qos_wfq fail\n");
		res = PPA_FAILURE;
	}

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_wfq_info, sizeof(cmd_info->qos_wfq_info)) != 0 )
		return PPA_FAILURE;

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_get_qos_wfq);
/*#endif //CONFIG_PPA_QOS_WFQ*/

/*
############# LEGACY QOS FUNCTION DEFINITIONS : END################
*/

/*
############# QOS RATE LIMIT FUNCTION DEFINITIONS : START################
*/

/*#if IS_ENABLED(CONFIG_PPA_QOS_RATE_SHAPING)*/
int32_t ppa_set_ctrl_qos_rate(uint32_t portid,	uint32_t f_enable, uint32_t flag)
{
	uint32_t i;
	PPA_QOS_COUNT_CFG count={0};
	PPA_QOS_ENABLE_CFG enable_cfg={0};
	PPA_QOS_RATE_SHAPING_CFG rate={0};

	count.portid = portid;
	count.flags = flag;
	ppa_drv_get_qos_qnum( &count, 0);

	if( count.num <= 0 )
		return PPA_FAILURE;

	enable_cfg.portid = portid;
	enable_cfg.flag = flag;
	enable_cfg.f_enable = f_enable;
	ppa_drv_set_ctrl_qos_rate( &enable_cfg, 0);

	for( i=0; i<count.num; i++ ) {
		rate.flag = 0;
		rate.portid = portid;
		rate.queueid	= i;
		ppa_drv_reset_qos_rate( &rate, 0);
	}

	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_set_ctrl_qos_rate);

int32_t ppa_get_ctrl_qos_rate(uint32_t portid,	uint32_t *f_enable, uint32_t flag)
{
	PPA_QOS_ENABLE_CFG enable_cfg={0};
	int32_t res;

	enable_cfg.portid = portid;
	enable_cfg.flag = flag;

	res= ppa_drv_get_ctrl_qos_rate( &enable_cfg, 0);

	if( *f_enable ) *f_enable = enable_cfg.f_enable;
	return res;
}
EXPORT_SYMBOL(ppa_get_ctrl_qos_rate);

int32_t ppa_set_qos_rate( char *ifname, uint32_t portid, uint32_t queueid, int32_t shaperid, uint32_t rate, uint32_t burst, uint32_t flag, int32_t hal_id )
{

	int32_t ret=PPA_SUCCESS;

	char dev_name[PPA_IF_NAME_SIZE];

	QOS_RATE_SHAPING_CFG tmu_rate_cfg;
	memset(&tmu_rate_cfg,0x00,sizeof(QOS_RATE_SHAPING_CFG));
	if(ppa_get_pppoa_base_if(ifname,dev_name) == PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT," %s:%d Pseudo PPPOA interface = %s\n",__FUNCTION__,__LINE__,dev_name);
		tmu_rate_cfg.dev_name = dev_name;
		snprintf(tmu_rate_cfg.ifname, sizeof(tmu_rate_cfg.ifname), "%s", ifname);
		tmu_rate_cfg.flag |= PPA_F_PPPOATM;
	} else {
		snprintf(tmu_rate_cfg.ifname, sizeof(tmu_rate_cfg.ifname), "%s", ifname);
		tmu_rate_cfg.dev_name = ifname;
	}
	tmu_rate_cfg.portid = portid;
	tmu_rate_cfg.queueid = queueid;
	tmu_rate_cfg.shaperid = shaperid;
	tmu_rate_cfg.rate_in_kbps = rate;
	tmu_rate_cfg.burst = burst;
	tmu_rate_cfg.flag |= flag;
	if ( (ret = ppa_hsel_set_qos_rate_entry( &tmu_rate_cfg, 0, hal_id) ) == PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}
EXPORT_SYMBOL(ppa_set_qos_rate);

int32_t ppa_get_qos_rate( uint32_t portid, uint32_t queueid, int32_t *shaperid, uint32_t *rate, uint32_t *burst, uint32_t flag)
{
	PPA_QOS_RATE_SHAPING_CFG rate_cfg={0};
	int32_t res = PPA_FAILURE;

	rate_cfg.portid = portid;
	rate_cfg.flag = flag;
	rate_cfg.queueid = queueid;
	rate_cfg.shaperid = -1;
	res = ppa_drv_get_qos_rate( &rate_cfg, 0);

	if( rate ) *rate = rate_cfg.rate_in_kbps;
	if( burst )	*burst = rate_cfg.burst;
	if( shaperid ) *shaperid = rate_cfg.shaperid;
	return res;

}
EXPORT_SYMBOL(ppa_get_qos_rate);

int32_t ppa_reset_qos_rate(char *ifname, char *dev_name, uint32_t portid, int32_t queueid, int32_t shaperid, uint32_t flag, int32_t hal_id)
{
	int32_t ret = PPA_FAILURE;
	QOS_RATE_SHAPING_CFG tmu_rate_cfg;
	memset(&tmu_rate_cfg,0x00,sizeof(QOS_RATE_SHAPING_CFG));

	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT," %s:%d Pseudo PPPOA interface = %s\n",__FUNCTION__,__LINE__,dev_name);
	tmu_rate_cfg.dev_name = dev_name;
	ppa_strncpy(tmu_rate_cfg.ifname,ifname, PPA_IF_NAME_SIZE);
	tmu_rate_cfg.portid = portid;
	tmu_rate_cfg.queueid = queueid;
	tmu_rate_cfg.shaperid = shaperid;
	tmu_rate_cfg.flag |= flag;

	if ( (ret = ppa_hsel_reset_qos_rate_entry( &tmu_rate_cfg, 0, hal_id) ) == PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}
EXPORT_SYMBOL(ppa_reset_qos_rate);


int32_t ppa_ioctl_set_ctrl_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int32_t res;

	ppa_memset(&cmd_info->qos_ctrl_info, 0, sizeof(cmd_info->qos_ctrl_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_ctrl_info, (void *)arg, sizeof(cmd_info->qos_ctrl_info)) != 0 )
		return PPA_FAILURE;

	res = ppa_set_ctrl_qos_rate(cmd_info->qos_ctrl_info.portid, cmd_info->qos_ctrl_info.enable, cmd_info->qos_ctrl_info.flags);
	if ( res != PPA_SUCCESS )
	{
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_set_ctrl_qos_rate fail\n");
		res = PPA_FAILURE;
	}

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_set_ctrl_qos_rate);

int32_t ppa_ioctl_get_ctrl_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = PPA_FAILURE;

	ppa_memset(&cmd_info->qos_ctrl_info, 0, sizeof(cmd_info->qos_ctrl_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_ctrl_info, (void *)arg, sizeof(cmd_info->qos_ctrl_info)) != 0 )
		return PPA_FAILURE;

	res = ppa_get_ctrl_qos_rate(cmd_info->qos_ctrl_info.portid, &cmd_info->qos_ctrl_info.enable, cmd_info->qos_ctrl_info.flags);
	if ( res != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_get_ctrl_qos_rate fail\n");
		res = PPA_FAILURE;
	}

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_ctrl_info, sizeof(cmd_info->qos_ctrl_info)) != 0 )
		return PPA_FAILURE;

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_get_ctrl_qos_rate);

int32_t qosal_reset_qos_rate(PPA_CMD_RATE_INFO *qos_rate_info, QOS_QUEUE_LIST_ITEM *p_item, QOS_SHAPER_LIST_ITEM *p_s_item)
{
	PPA_HSEL_CAP_NODE *caps_list = NULL;
	uint32_t i = 0,ret=PPA_SUCCESS,numcap;

	if(!p_s_item->caps_list) {
		return PPA_FAILURE;
	}
	caps_list = (PPA_HSEL_CAP_NODE *)p_s_item->caps_list;
	numcap = p_s_item->num_caps;

	for( i = 0; i < numcap; ) {
		switch(caps_list[i].cap) {
			case Q_SHAPER: {
				if( caps_list[i].hal_id == PPE_HAL)
					break;
				else {
					ret = ppa_reset_qos_rate(qos_rate_info->ifname,p_s_item->dev_name,qos_rate_info->portid,
						p_item->p_entry, p_s_item->p_entry, qos_rate_info->flags,caps_list[i].hal_id);
					if(ret != PPA_SUCCESS)
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_set_qos_rate failed \n" );
				}
				break;
			}
			case PORT_SHAPER: {
				if( caps_list[i].hal_id == PPE_HAL)
					break;
				else {
					if((qos_rate_info->shaperid == -1) && (qos_rate_info->queueid == -1)) {
						ret = ppa_reset_qos_rate(qos_rate_info->ifname,p_s_item->dev_name,
								qos_rate_info->portid, -1 , p_s_item->p_entry,
								qos_rate_info->flags,caps_list[i].hal_id);
						if(ret != PPA_SUCCESS)
							ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_set_qos_rate failed \n" );
						}
				}
				break;
			}
			default:
				break;
		}
		i++;
	}
	return ret;
}

int32_t qosal_set_qos_rate(PPA_CMD_RATE_INFO *qos_rate_info, QOS_QUEUE_LIST_ITEM *p_item, QOS_SHAPER_LIST_ITEM *p_s_item)
{
	PPA_HSEL_CAP_NODE *caps_list = NULL;
	uint32_t i = 0,ret=PPA_SUCCESS,numcap;

	if(!p_s_item->caps_list) {
		return PPA_FAILURE;
	}
	caps_list = (PPA_HSEL_CAP_NODE *)p_s_item->caps_list;
	numcap = p_s_item->num_caps;

	/* when init, these entry values are ~0, the max the number which can be detected by these functions*/
	for( i = 0; i < numcap; ) {
		switch(caps_list[i].cap) {
			case Q_SHAPER: {
				if( caps_list[i].hal_id == PPE_HAL)
					break;
				else {
					ret = ppa_set_qos_rate(qos_rate_info->ifname,qos_rate_info->portid,
						p_item->p_entry, p_s_item->p_entry, qos_rate_info->rate,qos_rate_info->burst,
						qos_rate_info->flags,caps_list[i].hal_id);
						if(ret != PPA_SUCCESS)
							ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_set_qos_rate failed \n" );
				}
				break;
			}
			case PORT_SHAPER: {
				if( caps_list[i].hal_id == PPE_HAL)
					break;
				else {
					if((qos_rate_info->shaperid == -1) && (qos_rate_info->queueid == -1)) {
						ret = ppa_set_qos_rate(qos_rate_info->ifname,qos_rate_info->portid, -1 ,
								p_s_item->p_entry, qos_rate_info->rate,qos_rate_info->burst,
								qos_rate_info->flags,caps_list[i].hal_id);
						if(ret != PPA_SUCCESS)
							ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_set_qos_rate failed \n" );
					}
				}
				break;
			}
			default:
				break;
		}
		i++;
	}
	return ret;

}
EXPORT_SYMBOL(qosal_set_qos_rate);

int32_t ppa_ioctl_set_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int32_t res;


	uint32_t phy_qid,phy_shaperid;
		char dev_name[PPA_IF_NAME_SIZE];
	QOS_QUEUE_LIST_ITEM *p_item = NULL;
	QOS_SHAPER_LIST_ITEM *p_s_item;

	ppa_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
		return PPA_FAILURE;

	phy_qid = cmd_info->qos_rate_info.queueid;
	phy_shaperid = cmd_info->qos_rate_info.shaperid;

	res = ppa_qos_shaper_lookup(cmd_info->qos_rate_info.shaperid,cmd_info->qos_rate_info.ifname,&p_s_item);
	if( res == PPA_ENOTAVAIL ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, " ppa_ioctl_set_qos_rate: PPA_QOS_SHAPER_NOT_FOUND	\n");
		return PPA_FAILURE;
	}
	else
		phy_shaperid = p_s_item->p_entry;

	if(phy_qid != -1) {
		res = ppa_qos_queue_lookup(cmd_info->qos_rate_info.queueid,cmd_info->qos_queue_info.ifname,&p_item);
		if( res == PPA_ENOTAVAIL ) {
			ppa_debug(DBG_ENABLE_MASK_QOS, " ppa_ioctl_set_qos_rate: PPA_QOS_QUEUE_NOT_FOUND	\n");
			return PPA_FAILURE;
		} else
			phy_qid = p_item->p_entry;
	}
	if(ppa_get_pppoa_base_if(cmd_info->qos_rate_info.ifname,dev_name) == PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT," %s:%d Pseudo PPPOA interface = %s\n",__FUNCTION__,__LINE__,dev_name);
		ppa_strncpy(p_s_item->dev_name,dev_name,PPA_IF_NAME_SIZE);
		p_s_item->flags |= PPA_F_PPPOATM;
	}
#if IS_ENABLED(CONFIG_SOC_GRX500)
	if (ppa_set_ingress_qos_generic(cmd_info->qos_rate_info.ifname, &cmd_info->qos_rate_info.flags))
		return PPA_FAILURE;
#endif/*CONFIG_SOC_GRX500*/

	if ( p_item != NULL )
		res = qosal_set_qos_rate(&cmd_info->qos_rate_info,p_item,p_s_item);
	else
		res = qosal_set_qos_rate(&cmd_info->qos_rate_info,NULL,p_s_item);

	if ( res != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_set_qos_rate fail\n");
		res = PPA_FAILURE;
	}

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_set_qos_rate);

int32_t ppa_ioctl_reset_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = PPA_FAILURE;


	uint32_t phy_qid,phy_shaperid;
	QOS_QUEUE_LIST_ITEM *p_item = NULL;
	QOS_SHAPER_LIST_ITEM *p_s_item = NULL;

	ppa_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
		return PPA_FAILURE;

	phy_qid = cmd_info->qos_rate_info.queueid;
	phy_shaperid = cmd_info->qos_rate_info.shaperid;

	res = ppa_qos_shaper_lookup(cmd_info->qos_rate_info.shaperid,cmd_info->qos_rate_info.ifname,&p_s_item);
	if( res == PPA_ENOTAVAIL || p_s_item == NULL ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, " ppa_ioctl_reset_qos_rate: PPA_QOS_SHAPER_NOT_FOUND	\n");
		return PPA_FAILURE;
	}
	else
		phy_shaperid = p_s_item->p_entry;

	if(phy_qid != -1) {
		res = ppa_qos_queue_lookup(cmd_info->qos_rate_info.queueid,cmd_info->qos_queue_info.ifname,&p_item);
		if( res == PPA_ENOTAVAIL || p_item == NULL) {
			ppa_debug(DBG_ENABLE_MASK_QOS, " ppa_ioctl_set_qos_rate: PPA_QOS_QUEUE_NOT_FOUND	\n");
			return PPA_FAILURE;
		}
		else
			phy_qid = p_item->p_entry;
	}

#if IS_ENABLED(CONFIG_SOC_GRX500)
	if (ppa_set_ingress_qos_generic(cmd_info->qos_rate_info.ifname, &cmd_info->qos_rate_info.flags))
		return PPA_FAILURE;
#endif/*CONFIG_SOC_GRX500*/

	if ( p_item != NULL ) {
		res = qosal_reset_qos_rate(&cmd_info->qos_rate_info,p_item,p_s_item);
	} else {
		res = qosal_reset_qos_rate(&cmd_info->qos_rate_info,NULL,p_s_item);
	}

	if ( res != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_set_qos_rate fail\n");
		res = PPA_FAILURE;
	}

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_reset_qos_rate);

int32_t ppa_ioctl_get_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = PPA_FAILURE;

	ppa_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
		return PPA_FAILURE;
	res = ppa_get_qos_rate(cmd_info->qos_rate_info.portid, cmd_info->qos_rate_info.queueid, 
			&cmd_info->qos_rate_info.shaperid, &cmd_info->qos_rate_info.rate,
			&cmd_info->qos_rate_info.burst, cmd_info->qos_rate_info.flags);
	if ( res != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_get_qos_rate fail\n");
		res = PPA_FAILURE;
	}

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_rate_info, sizeof(cmd_info->qos_rate_info)) != 0 )
		return PPA_FAILURE;

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_get_qos_rate);

int32_t ppa_set_qos_shaper( int32_t shaperid, uint32_t rate, uint32_t burst, PPA_QOS_ADD_SHAPER_CFG *s, uint32_t flags, int32_t hal_id )
{
	uint32_t ret=PPA_SUCCESS;
	QOS_RATE_SHAPING_CFG tmu_shape_cfg;

	memset(&tmu_shape_cfg,0x00,sizeof(QOS_RATE_SHAPING_CFG));
	tmu_shape_cfg.shaper.mode = s->mode;
	tmu_shape_cfg.shaper.enable = s->enable;
	tmu_shape_cfg.shaper.pir = s->pir;
	tmu_shape_cfg.shaper.pbs = s->pbs;
	tmu_shape_cfg.shaper.cir = s->cir;
	tmu_shape_cfg.shaper.cbs = s->cbs;
	tmu_shape_cfg.shaper.flags = s->flags;
#if IS_ENABLED(CONFIG_PPA_PUMA7)
	ppa_strncpy(tmu_shape_cfg.ifname, s->ifname, PPA_IF_NAME_SIZE);
#endif/*CONFIG_PPA_PUMA7*/
	if ( (ret = ppa_hsel_set_qos_shaper_entry( &tmu_shape_cfg, 0, hal_id) ) == PPA_SUCCESS ) {
		s->phys_shaperid = tmu_shape_cfg.phys_shaperid;
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	} else
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add failed!!!\n", __FILE__,__FUNCTION__,__LINE__);

	return ret;
}

int32_t ppa_get_qos_shaper( int32_t shaperid, uint32_t *rate, uint32_t *burst, uint32_t flag)
{
	PPA_QOS_RATE_SHAPING_CFG rate_cfg={0};
	int32_t res = PPA_FAILURE;

	rate_cfg.flag = flag;
	rate_cfg.shaperid = shaperid;

	//res = ifx_ppa_drv_get_qos_rate( &rate_cfg, 0);
	res = ppa_drv_get_qos_shaper( &rate_cfg, 0);


	if( rate ) *rate = rate_cfg.rate_in_kbps;
	if( burst )	*burst = rate_cfg.burst;

	return res;

}

int32_t ppa_ioctl_set_qos_shaper(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/

	QOS_SHAPER_LIST_ITEM *p_item;
	ppa_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
		return PPA_FAILURE;

	if( qosal_add_shaper(&cmd_info->qos_rate_info,&p_item)!= PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "qosal_add_shaper returned failure\n");
		return PPA_FAILURE;
	}
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(ppa_ioctl_set_qos_shaper);

int32_t ppa_ioctl_get_qos_shaper(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = PPA_FAILURE;

	ppa_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

	if ( ppa_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
		return PPA_FAILURE;

	res = ppa_get_qos_shaper(cmd_info->qos_rate_info.shaperid, &cmd_info->qos_rate_info.rate,
			&cmd_info->qos_rate_info.burst, cmd_info->qos_rate_info.flags);
	if ( res != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_get_qos_shaper fail\n");
		res = PPA_FAILURE;
	}

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_rate_info, sizeof(cmd_info->qos_rate_info)) != 0 )
		return PPA_FAILURE;

	return res;
}
EXPORT_SYMBOL(ppa_ioctl_get_qos_shaper);

/*#endif	//CONFIG_PPA_QOS_RATE_SHAPING*/

/*
############# QOS RATE LIMIT FUNCTION DEFINITIONS : END################
*/

/*
############# QOS CLASSIFIER FUNCTION DEFINITIONS : START################
*/

#if IS_ENABLED(CONFIG_SOC_GRX500)
int32_t reset_pcp_remark(PPA_CLASS_RULE *rule)
{
	int32_t ret = PPA_SUCCESS,i = 0;
	GSW_QoS_SVLAN_ClassPCP_PortCfg_t qos_classpcp;

	memset(&qos_classpcp, 0, sizeof(GSW_QoS_SVLAN_ClassPCP_PortCfg_t));

	if ((rule->action.iftype == IF_CATEGORY_ETHWAN) || (rule->action.iftype == IF_CATEGORY_PTMWAN) ||
			(rule->action.iftype == IF_CATEGORY_ATMWAN)) {
		for (i = 0; i < sizeof(LAN_PORT)/sizeof(int8_t); i++) {
			qos_classpcp.nPortId = LAN_PORT[i];
			ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_GET,&qos_classpcp);
			qos_classpcp.nCPCP[rule->action.qos_action.alt_trafficclass] = 0;
			ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_SET,&qos_classpcp);
		}
	} else if ((rule->action.iftype == IF_CATEGORY_ETHLAN)) {
		qos_classpcp.nPortId = ETHWAN_PORT;
		ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_GET, &qos_classpcp);
		qos_classpcp.nCPCP[rule->action.qos_action.alt_trafficclass] = 0;
		ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_SET, &qos_classpcp);

		for (i = 0; i < sizeof(FLEXI_PORT)/sizeof(int8_t); i++) {
			qos_classpcp.nPortId = FLEXI_PORT[i];
			ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_GET, &qos_classpcp);
			qos_classpcp.nCPCP[rule->action.qos_action.alt_trafficclass] = 0;
			ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_SET, &qos_classpcp);
		}
	}

	return ret;
}

int32_t set_pcp_remark(PPA_CLASS_RULE *rule)
{
	int32_t ret = PPA_SUCCESS,i = 0;
	GSW_QoS_portRemarkingCfg_t qos_remark_cfg;
	GSW_QoS_SVLAN_ClassPCP_PortCfg_t qos_classpcp;

	memset(&qos_remark_cfg, 0, sizeof(GSW_QoS_portRemarkingCfg_t));
	memset(&qos_classpcp, 0, sizeof(GSW_QoS_SVLAN_ClassPCP_PortCfg_t));

	for(i=0; i<sizeof(LAN_PORT)/sizeof(int8_t); i++) {
		qos_remark_cfg.nPortId = LAN_PORT[i];
		qos_remark_cfg.bPCP_IngressRemarkingEnable = 1;
		qos_remark_cfg.bPCP_EgressRemarkingEnable = 1;
		qos_remark_cfg.bSTAG_PCP_IngressRemarkingEnable = 1;
		qos_remark_cfg.bSTAG_PCP_DEI_EgressRemarkingEnable = 1;
		ret = qosal_set_gswr(GSW_QOS_PORT_REMARKING_CFG_SET,&qos_remark_cfg);
	}
	qos_remark_cfg.nPortId = ETHWAN_PORT;
	qos_remark_cfg.bPCP_IngressRemarkingEnable = 1;
	qos_remark_cfg.bPCP_EgressRemarkingEnable = 1;
	qos_remark_cfg.bSTAG_PCP_IngressRemarkingEnable = 1;
	qos_remark_cfg.bSTAG_PCP_DEI_EgressRemarkingEnable = 1;
	ret = qosal_set_gswr(GSW_QOS_PORT_REMARKING_CFG_SET,&qos_remark_cfg);

	if ((rule->action.iftype == IF_CATEGORY_ETHWAN) || (rule->action.iftype == IF_CATEGORY_PTMWAN)
			|| (rule->action.iftype == IF_CATEGORY_ATMWAN)) {
		for (i = 0; i < sizeof(LAN_PORT)/sizeof(int8_t); i++) {
			qos_classpcp.nPortId = LAN_PORT[i];
			ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_GET,&qos_classpcp);
			qos_classpcp.nCPCP[rule->action.qos_action.alt_trafficclass] = rule->action.qos_action.new_pcp;
			qos_classpcp.nSPCP[rule->action.qos_action.alt_trafficclass] = rule->action.qos_action.new_pcp;
			ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_SET,&qos_classpcp);
		}
	} else if ((rule->action.iftype == IF_CATEGORY_ETHLAN)) {
		qos_classpcp.nPortId = ETHWAN_PORT;
		ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_GET, &qos_classpcp);
		qos_classpcp.nCPCP[rule->action.qos_action.alt_trafficclass] = rule->action.qos_action.new_pcp;
		qos_classpcp.nSPCP[rule->action.qos_action.alt_trafficclass] = rule->action.qos_action.new_pcp;
		ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_SET, &qos_classpcp);

		for (i = 0; i < sizeof(FLEXI_PORT)/sizeof(int8_t); i++) {
			qos_classpcp.nPortId = FLEXI_PORT[i];
			ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_GET, &qos_classpcp);
			qos_classpcp.nCPCP[rule->action.qos_action.alt_trafficclass] = rule->action.qos_action.new_pcp;
			qos_classpcp.nSPCP[rule->action.qos_action.alt_trafficclass] = rule->action.qos_action.new_pcp;
			ret = qosal_set_gswr(GSW_QOS_SVLAN_CLASS_PCP_PORT_SET, &qos_classpcp);
		}
	}

	return ret;
}

/* Function checks for lower interface until gets a valid dp_subif info.
 * NOTE: Works only on pppoe, vlan, mac_vlan and base (eth, ptm, atm) interface only
 */
int32_t ppa_qos_get_netif_subif(PPA_IFNAME *ifname, dp_subif_t *dp_subif)
{
	int32_t ret;
	PPA_VCC *vcc = NULL;
	PPA_NETIF *lowerIf = NULL;
	struct net *net;

	net = ppa_get_current_net_ns();
	lowerIf = ppa_get_netif_by_net(net, ifname);

	if (!lowerIf)
		return PPA_FAILURE;

	do {
		if (lowerIf->priv_flags & IFF_EBRIDGE)
			return PPA_FAILURE;

		ppa_br2684_get_vcc(lowerIf, &vcc);
		ret = dp_get_netif_subifid(lowerIf, NULL, vcc, NULL, dp_subif, 0);
		lowerIf = ppa_get_lower_dev(lowerIf, net);
	} while (lowerIf && ret != PPA_SUCCESS);

	return ret;
}

int32_t set_port_subifid(PPA_CLASS_RULE *rule)
{
	int32_t ret = PPA_SUCCESS;
	dp_subif_t *dp_subif = NULL;

	PPA_ALLOC(dp_subif_t, dp_subif);
	ppa_memset(dp_subif, 0, sizeof(dp_subif_t));
	if (ppa_qos_get_netif_subif(rule->rx_if, dp_subif)==PPA_SUCCESS) {
		if(dp_subif->subif == 0) {
			ppa_debug(DBG_ENABLE_MASK_QOS," In set_port_subifid : Sub interface Id is 0 \n");
			rule->pattern.bPortIdEnable = 1;
			rule->pattern.nPortId = dp_subif->port_id;

			/*
			For subif = 0,
			a. port specific (e.g. CAT_USQOS or CAT_DSQOS), do not enable subifid pattern
			b. subif specific (e.g. CAT_FWD category or WLAN), enable subifid pattern
			*/
			if (rule->category == CAT_FWD) {
				rule->pattern.bSubIfIdEnable = 1;
				rule->pattern.nSubIfId = 0;
			}
		} else {
			ppa_debug(DBG_ENABLE_MASK_QOS," In set_port_subifid : Sub interface Id is %d \n",rule->pattern.nSubIfId);
			rule->pattern.bPortIdEnable = 1;
			rule->pattern.nPortId = dp_subif->port_id;
			/* Packet does not carry Explicit subifid mark at ingress, so exclude it. */
			if (!(dp_subif->subif_flag[0] & DP_F_ALLOC_EXPLICIT_SUBIFID)) {
				rule->pattern.bSubIfIdEnable = 1;
				rule->pattern.nSubIfId = (dp_subif->subif >> 8) & 0xF; /* subifid is from bits 8-11 */
			}
		}
		ppa_debug(DBG_ENABLE_MASK_QOS,
			" In set_port_subifid: dp_get_netif_subifid: portid enable = %d:portid = %d:subif enable= %d:subif = %d \n",
			rule->pattern.bPortIdEnable,rule->pattern.nPortId,rule->pattern.bSubIfIdEnable,rule->pattern.nSubIfId);
	}
	else
		ppa_debug(DBG_ENABLE_MASK_QOS," In set_port_subifid:: dp_get_netif_subifid failed for the interface %s \n",rule->rx_if);

	ppa_free(dp_subif);
	return ret;
}

int32_t ppa_ioctl_add_class_rule (unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	PPA_CLASS_RULE *class_rule;
	PPA_NETIF *netif = NULL;
	dp_subif_t *dp_subif = NULL;
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/
	int32_t ret=PPA_SUCCESS;

	ret = copy_from_user(&cmd_info->class_info,(void *)arg, sizeof(cmd_info->class_info));
	if (ret != PPA_SUCCESS) {
		return PPA_FAILURE;
	}

	if( cmd_info->class_info.action.qos_action.remarkpcp == 1)
		set_pcp_remark(&cmd_info->class_info);

	set_port_subifid(&cmd_info->class_info);

	ret = ppa_set_ingress_qos_classifier(&cmd_info->class_info);
	if (ret != PPA_SUCCESS)
		return ret;

#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	PPA_ALLOC(dp_subif_t, dp_subif);
	ppa_memset(dp_subif, 0, sizeof(dp_subif_t));
	class_rule = &cmd_info->class_info;
	netif = ppa_get_netif(class_rule->tx_if);
	if (dp_get_netif_subifid(netif, NULL, NULL, NULL, dp_subif, 0) == PPA_SUCCESS) {
		if (dp_subif->subif != 0) {
			ret = ppa_set_logical_if_qos_classifier(class_rule);
			if (ret != PPA_SUCCESS) {
				ppa_free(dp_subif);
				return ret;
			}
		}
	}
	ppa_free(dp_subif);
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/

	ret = ppa_add_class_rule(&cmd_info->class_info);
	if (ret != PPA_SUCCESS)
		ret = PPA_FAILURE;

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->class_info, sizeof(cmd_info->class_info)) != 0 ) 
		ret = PPA_FAILURE;
	
	return ret;

}

int32_t ppa_ioctl_mod_class_rule (unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
	int32_t ret=PPA_SUCCESS;
	ret = copy_from_user(&cmd_info->class_info,(void *)arg, sizeof(cmd_info->class_info));
	if (ret != PPA_SUCCESS) {
		return PPA_FAILURE;
	}
	ret=ppa_mod_class_rule(&cmd_info->class_info);
	if ( ppa_copy_to_user( (void *)arg, &cmd_info->class_info, sizeof(cmd_info->class_info)) != 0 )
		return PPA_FAILURE;

	return ret;

}

int32_t ppa_ioctl_get_class_rule (unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
	int32_t ret=PPA_SUCCESS;
	ret = copy_from_user(&cmd_info->class_info,(void *)arg, sizeof(cmd_info->class_info));
	if (ret != PPA_SUCCESS) {
		return PPA_FAILURE;
	}
	ret=ppa_get_class_rule(&cmd_info->class_info);
	if ( ppa_copy_to_user( (void *)arg, &cmd_info->class_info, sizeof(cmd_info->class_info)) != 0 )
		return PPA_FAILURE;

	return ret;

}

int32_t ppa_ioctl_del_class_rule (unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
	int32_t ret=PPA_SUCCESS;

	ret = copy_from_user(&cmd_info->class_info,(void *)arg, sizeof(cmd_info->class_info));
	if (ret != PPA_SUCCESS) {
		return PPA_FAILURE;
	}
	if( cmd_info->class_info.action.qos_action.remarkpcp == 1)
		reset_pcp_remark(&cmd_info->class_info);
	ret=ppa_del_class_rule(&cmd_info->class_info);
	return ret;

}
#endif/*CONFIG_SOC_GRX500*/

int32_t ppa_api_qos_manager_create(void)
{
	if ( ppa_lock_init(&g_qos_queue_lock) ) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in creating lock for qos queue list.\n");
		goto PPA_API_QOS_MANAGER_CREATE_FAIL;
	}
	if ( ppa_lock_init(&g_qos_shaper_lock) ) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in creating lock for qos shaper list.\n");
		goto PPA_API_QOS_MANAGER_CREATE_FAIL;
	}
#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	if ( ppa_lock_init(&g_qos_logical_if_lock) ) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in creating lock for qos logical if list.\n");
		goto PPA_API_QOS_MANAGER_CREATE_FAIL;
	}
#endif
#if IS_ENABLED(CONFIG_SOC_GRX500)
	if ( ppa_lock_init(&g_qos_inggrp_list_lock) ) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in creating lock for qos ingress list.\n");
		goto PPA_API_QOS_MANAGER_CREATE_FAIL;
	}
	if (ppa_lock_init(&g_qos_meter_lock)) {
		ppa_debug(DBG_ENABLE_MASK_ERR,"Failed in creating lock for qos meter list.\n");
		goto PPA_API_QOS_MANAGER_CREATE_FAIL;
	}
#endif
	return PPA_SUCCESS;
PPA_API_QOS_MANAGER_CREATE_FAIL:
	ppa_api_qos_manager_destroy();
	return PPA_EIO;
}
void ppa_api_qos_manager_destroy(void)
{
	ppa_lock_destroy(&g_qos_queue_lock);
	ppa_lock_destroy(&g_qos_shaper_lock);
#if IS_ENABLED(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
	ppa_lock_destroy(&g_qos_logical_if_lock);
#endif
#if IS_ENABLED(CONFIG_SOC_GRX500)
	ppa_lock_destroy(&g_qos_inggrp_list_lock);
	ppa_lock_destroy(&g_qos_meter_lock);
#endif
}
/*
############# QOS CLASSIFIER FUNCTION DEFINITIONS : START################
*/

EXPORT_SYMBOL(ppa_ioctl_add_qos_queue);
EXPORT_SYMBOL(ppa_ioctl_modify_qos_queue);
EXPORT_SYMBOL(ppa_ioctl_delete_qos_queue);
EXPORT_SYMBOL(ppa_ioctl_qos_init_cfg);
EXPORT_SYMBOL(ppa_ioctl_add_wmm_qos_queue);
EXPORT_SYMBOL(ppa_ioctl_delete_wmm_qos_queue);
EXPORT_SYMBOL(ppa_get_qos_qnum);
EXPORT_SYMBOL(ppa_ioctl_get_qos_qnum);
EXPORT_SYMBOL(ppa_get_qos_mib);
EXPORT_SYMBOL(ppa_ioctl_get_qos_mib);
