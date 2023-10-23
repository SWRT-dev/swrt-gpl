/******************************************************************************
 **
 ** FILE NAME	: qos_mgr_api.c
 ** PROJECT	: UGW
 ** MODULES	: QoS Manager API
 **
 ** DATE	: 08 Jan 2020
 ** AUTHOR	: Mohammed Aarif
 ** DESCRIPTION	: QoS Manager API Implementation
 ** COPYRIGHT  : Copyright (c) 2019 - 2020 Intel Corporation
 **
 ** HISTORY
 ** $Date		$Author			$Comment
 ** 08 JAN 2020		Mohammed Aarif		Initiate Version
 *******************************************************************************/

#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/list.h>
#include <generated/autoconf.h>
#include <net/qos_mgr/qos_hal_api.h>
#if IS_ENABLED(CONFIG_QOS_MGR_TC_SUPPORT)
#include <net/qos_mgr/qos_mgr_tc_hook.h>
#endif
#include "qos_mgr_stack_al.h"
#include "qos_mgr_api.h"
#include <uapi/net/qos_mgr_common.h>
#include <net/qos_mgr/qos_mgr_hook.h>

#include "qos_mgr_ioctl.h"
#include "qos_mgr_api.h"
#include "qos_hal.h"

#define QOS_MGR_MAX_Q 40
#define QOS_MGR_MAX_Q_LEN 100
#define QOS_MGR_Q_CAP 1
#define MAX_QOS_Q_CAPS 6
#define QOS_HAL 0

#define ETHWAN_PORT 15
int8_t FLEXI_PORT[]	= {7, 8, 9, 10, 11, 12, 13, 14};	/* Flexible Ports for DSL (ATM/PTM) WAN and WLAN. */
int8_t LAN_PORT[] 	= {2,3,4,5,6};
int8_t LAN_PORT_Q[][4] 	= { {8,9,10,11}, {12,13,14,15}, {16,17,18,19},{20,21,22,23},{24,25,26,27} };
int32_t WT_Q[] 		= {20480,20480,65535,65535};
int32_t SCH_Q[] 	= {1,1,0,0};
#define MAX_NUMBER_OF_LAN_PORTS 4

#define TMU_GREEN_DEFAULT_THRESHOLD	0x24

static QOS_SHAPER_LIST_ITEM *g_qos_shaper = NULL;
static QOS_MGR_LOCK g_qos_shaper_lock;

static QOS_INGGRP_LIST_ITEM *g_qos_inggrp_list = NULL;
static QOS_MGR_LOCK g_qos_inggrp_list_lock;

QOS_QUEUE_LIST_ITEM *g_qos_queue;
QOS_MGR_LOCK g_qos_queue_lock;

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

#if defined(WMM_QOS_CONFIG) && WMM_QOS_CONFIG
int g_eth_class_prio_map[MAX_WLAN_DEV][MAX_TC_NUM] = {
	{0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7},
	{0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7},
	{0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7},
	{0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7}
};
#endif /*WMM_QOS_CONFIG*/

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
QOS_SHAPER_LIST_ITEM * qos_mgr_shaper_alloc_item(void)	/* alloc_netif_info */
{
	QOS_SHAPER_LIST_ITEM *obj;

	obj = (QOS_SHAPER_LIST_ITEM *)qos_mgr_malloc(sizeof(*obj));
	if ( obj ) {
		qos_mgr_memset(obj, 0, sizeof(*obj));
		qos_mgr_atomic_set(&obj->count, 1);
	}
	return obj;
}
EXPORT_SYMBOL(qos_mgr_shaper_alloc_item);

void qos_mgr_shaper_free_item(QOS_SHAPER_LIST_ITEM *obj)	/* free_netif_info*/
{
	if ( qos_mgr_atomic_dec(&obj->count) == 0 ) {
		qos_mgr_free(obj);
	}
}
EXPORT_SYMBOL(qos_mgr_shaper_free_item);

void qos_mgr_shaper_lock_list(void)	/* lock_netif_info_list*/
{
	qos_mgr_lock_get(&g_qos_shaper_lock);
}
EXPORT_SYMBOL(qos_mgr_shaper_lock_list);

void qos_mgr_shaper_unlock_list(void)	/* unlock_netif_info_list */
{
	qos_mgr_lock_release(&g_qos_shaper_lock);
}
EXPORT_SYMBOL(qos_mgr_shaper_unlock_list);

void __qos_mgr_shaper_add_item(QOS_SHAPER_LIST_ITEM *obj)	/* add_netif_info */
{
	qos_mgr_atomic_inc(&obj->count);
	obj->next = g_qos_shaper;
	g_qos_shaper = obj;
}
EXPORT_SYMBOL(__qos_mgr_shaper_add_item);

void qos_mgr_shaper_remove_item(int32_t s_num, QOS_MGR_IFNAME ifname[16],QOS_SHAPER_LIST_ITEM **pp_info)	/* remove_netif_info*/
{
	QOS_SHAPER_LIST_ITEM *p_prev, *p_cur;

	if ( pp_info )
		*pp_info = NULL;
	p_prev = NULL;
	qos_mgr_shaper_lock_list();
	for ( p_cur = g_qos_shaper; p_cur; p_prev = p_cur, p_cur = p_cur->next )
		if ( (p_cur->shaperid == s_num) && (strcmp(p_cur->ifname,ifname) == 0)) {
			if ( !p_prev )
				g_qos_shaper = p_cur->next;
			else
				p_prev->next = p_cur->next;
			if ( pp_info )
				*pp_info = p_cur;
			else
				qos_mgr_shaper_free_item(p_cur);
			break;
		}
	qos_mgr_shaper_unlock_list();
}
EXPORT_SYMBOL(qos_mgr_shaper_remove_item);

void qos_mgr_shaper_free_list(void)	/* free_netif_info_list*/
{
	QOS_SHAPER_LIST_ITEM *obj;

	qos_mgr_shaper_lock_list();
	while ( g_qos_shaper ) {
		obj = g_qos_shaper;
		g_qos_shaper = g_qos_shaper->next;

		qos_mgr_shaper_free_item(obj);
		obj = NULL;
	}
	qos_mgr_shaper_unlock_list();
}
EXPORT_SYMBOL(qos_mgr_shaper_free_list);

/* QoS Queue*/
int32_t __qos_mgr_shaper_lookup(int32_t s_num, QOS_MGR_IFNAME ifname[16],QOS_SHAPER_LIST_ITEM **pp_info)	/* netif_info_is_added*/
{
	int32_t ret = QOS_MGR_ENOTAVAIL;
	QOS_SHAPER_LIST_ITEM *p;

	for (p = g_qos_shaper; p; p = p->next ) {
		if ((p->shaperid == s_num) && (strcmp(p->ifname, ifname) == 0)) {
			ret = QOS_MGR_SUCCESS;
			if (pp_info) {
				qos_mgr_atomic_inc(&p->count);
				*pp_info = p;
			}
			break;
		}
	}

	return ret;
}
EXPORT_SYMBOL(__qos_mgr_shaper_lookup);

int32_t qos_mgr_shaper_lookup(int32_t s_num, QOS_MGR_IFNAME ifname[16],QOS_SHAPER_LIST_ITEM **pp_info)	/* netif_info_is_added*/
{
	int32_t ret;
	qos_mgr_shaper_lock_list();
	ret = __qos_mgr_shaper_lookup(s_num, ifname, pp_info);
	qos_mgr_shaper_unlock_list();

	return ret;
}
EXPORT_SYMBOL(qos_mgr_shaper_lookup);

int32_t qosal_add_shaper(QOS_MGR_CMD_RATE_INFO *rate_info, QOS_SHAPER_LIST_ITEM **pp_item)
{
	int32_t ret = QOS_MGR_SUCCESS;
	int32_t res;
	QOS_SHAPER_LIST_ITEM *p_item;
	QOS_SHAPER_LIST_ITEM *p_item1;
	QOS_MGR_ADD_SHAPER_CFG shaper_cfg;
	memset(&shaper_cfg,0x00,sizeof(QOS_MGR_ADD_SHAPER_CFG)); 
	p_item = qos_mgr_shaper_alloc_item();
	if ( !p_item ) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"alloc shaper list item failed \n" ); 
		return QOS_MGR_ENOMEM;
	}
	qos_mgr_strncpy(p_item->ifname, rate_info->ifname, QOS_MGR_IF_NAME_SIZE);
	qos_mgr_strncpy(p_item->dev_name, rate_info->ifname, QOS_MGR_IF_NAME_SIZE);
	p_item->portid = rate_info->portid;
	p_item->shaperid = rate_info->shaperid;
	p_item->shaper.enable = rate_info->shaper.enable;
	p_item->shaper.pir = rate_info->shaper.pir;
	p_item->shaper.pbs = rate_info->shaper.pbs;
	p_item->shaper.cir = rate_info->shaper.cir;
	p_item->shaper.cbs = rate_info->shaper.cbs;
	p_item->shaper.flags = rate_info->shaper.flags;

	p_item1 = *pp_item;
	res = qos_mgr_shaper_lookup(rate_info->shaperid,rate_info->ifname,&p_item1);
	if( res == QOS_MGR_SUCCESS ) {
		p_item->p_entry = p_item1->p_entry;
	
		qos_mgr_shaper_remove_item(p_item1->shaperid,p_item1->ifname,NULL);
		qos_mgr_shaper_free_item(p_item1);
	}

	__qos_mgr_shaper_add_item(p_item);
	
	shaper_cfg.enable = rate_info->shaper.enable;
	shaper_cfg.mode = rate_info->shaper.mode;
	shaper_cfg.pir = rate_info->shaper.pir;
	shaper_cfg.pbs = rate_info->shaper.pbs;
	shaper_cfg.cir = rate_info->shaper.cir;
	shaper_cfg.cbs = rate_info->shaper.cbs;
	shaper_cfg.flags = rate_info->shaper.flags;
	ret = qos_mgr_set_qos_shaper(rate_info->shaperid,rate_info->rate,
			rate_info->burst,&shaper_cfg,rate_info->shaper.flags,QOS_HAL);

	if (ret == QOS_MGR_SUCCESS) {
		p_item->p_entry = (shaper_cfg.phys_shaperid);
	} else
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"qos_mgr_set_qos_shaper failed \n" );

	*pp_item = p_item;
	return ret; 
}

/*
############# SUPPORTING SHAPER FUNCTION DEFINITIONS : END################
*/

/*
############# SUPPORTING METER FUNCTION DEFINITIONS : START################
*/

/*
############# SUPPORTING METER FUNCTION DEFINITIONS : END################
*/
/*
############# SUPPORTING INGRESS FUNCTION DEFINITIONS : START################
*/
QOS_INGGRP_LIST_ITEM * qos_mgr_inggrp_alloc_item(void)	/*	alloc_netif_info*/
{
	QOS_INGGRP_LIST_ITEM *obj;

	obj = (QOS_INGGRP_LIST_ITEM *)qos_mgr_malloc(sizeof(*obj));
	if ( obj ) {
		qos_mgr_memset(obj, 0, sizeof(*obj));
		qos_mgr_atomic_set(&obj->count, 1);
	}
	return obj;
}
EXPORT_SYMBOL(qos_mgr_inggrp_alloc_item);

void qos_mgr_inggrp_free_item(QOS_INGGRP_LIST_ITEM *obj)	/* free_netif_info*/
{
	if ( qos_mgr_atomic_dec(&obj->count) == 0 ) {
		qos_mgr_free(obj);
	}
}
EXPORT_SYMBOL(qos_mgr_inggrp_free_item);

void qos_mgr_inggrp_lock_list(void)	/*	lock_netif_info_list*/
{
	qos_mgr_lock_get(&g_qos_inggrp_list_lock);
}
EXPORT_SYMBOL(qos_mgr_inggrp_lock_list);

void qos_mgr_inggrp_unlock_list(void)	/*	unlock_netif_info_list*/
{
	qos_mgr_lock_release(&g_qos_inggrp_list_lock);
}
EXPORT_SYMBOL(qos_mgr_inggrp_unlock_list);

void __qos_mgr_inggrp_add_item(QOS_INGGRP_LIST_ITEM *obj)	 /*	add_netif_info*/
{
	qos_mgr_atomic_inc(&obj->count);
	obj->next = (struct qos_inggrp_list_item *)g_qos_inggrp_list;
	g_qos_inggrp_list = obj;
}
EXPORT_SYMBOL(__qos_mgr_inggrp_add_item);

void qos_mgr_inggrp_remove_item(QOS_MGR_IFNAME ifname[16], QOS_INGGRP_LIST_ITEM **pp_info)	/*	remove_netif_info*/
{
	QOS_INGGRP_LIST_ITEM *p_prev, *p_cur;

	if ( pp_info )
		*pp_info = NULL;
	p_prev = NULL;
	qos_mgr_inggrp_lock_list();
	for ( p_cur = g_qos_inggrp_list; p_cur; p_prev = p_cur, p_cur = (QOS_INGGRP_LIST_ITEM *)p_cur->next )
		if ((strcmp(p_cur->ifname,ifname) == 0)) {
			if ( !p_prev )
				g_qos_inggrp_list = (QOS_INGGRP_LIST_ITEM *)p_cur->next;
			else
				p_prev->next = p_cur->next;
			if ( pp_info )
				*pp_info = p_cur;
			else
				qos_mgr_inggrp_free_item(p_cur);
			break;
		}
	qos_mgr_inggrp_unlock_list();
}
EXPORT_SYMBOL(qos_mgr_inggrp_remove_item);

void qos_mgr_inggrp_free_list(void)
{
	 QOS_INGGRP_LIST_ITEM *obj;

	qos_mgr_inggrp_lock_list();
	while ( g_qos_inggrp_list ) {
		obj = g_qos_inggrp_list;
		g_qos_inggrp_list = (QOS_INGGRP_LIST_ITEM *)g_qos_inggrp_list->next;

		qos_mgr_inggrp_free_item(obj);
		obj = NULL;
	}
	qos_mgr_inggrp_unlock_list();
}
EXPORT_SYMBOL(qos_mgr_inggrp_free_list);

int32_t __qos_mgr_inggrp_lookup(const QOS_MGR_IFNAME ifname[16],QOS_INGGRP_LIST_ITEM **pp_info)	/* netif_info_is_added*/
{
	int32_t ret = QOS_MGR_ENOTAVAIL;
	QOS_INGGRP_LIST_ITEM *p;
	qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: lookup ifname=%s\n", __func__, __LINE__, ifname);
	for ( p = g_qos_inggrp_list; p; p = (QOS_INGGRP_LIST_ITEM *)p->next ) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: entry %p: ingress group=%d, ifname=%s\n", __func__,
				__LINE__, p, p->ingress_group, p->ifname); 
		if (strcmp(p->ifname,ifname) == 0) {
			ret = QOS_MGR_SUCCESS;
			if ( pp_info ) {
				qos_mgr_atomic_inc(&p->count);
				*pp_info = p;
			}
			break;
		}
	}

	return ret;
}
EXPORT_SYMBOL(__qos_mgr_inggrp_lookup);

int32_t qos_mgr_inggrp_lookup(const QOS_MGR_IFNAME ifname[16],QOS_INGGRP_LIST_ITEM **pp_info)	/* netif_info_is_added*/
{
	int32_t ret;
	qos_mgr_inggrp_lock_list();
	ret = __qos_mgr_inggrp_lookup(ifname, pp_info);
	qos_mgr_inggrp_unlock_list();

	return ret;
}
EXPORT_SYMBOL(qos_mgr_inggrp_lookup);

int32_t qos_mgr_set_qos_inggrp(QOS_MGR_INGGRP_CFG *inggrp_info, uint32_t hal_id)
{
	return QOS_MGR_SUCCESS;
}

int32_t qosal_get_qos_inggrp(QOS_MGR_INGGRP inggrp, QOS_MGR_IFNAME ifnames[QOS_MGR_MAX_IF_PER_INGGRP][QOS_MGR_IF_NAME_SIZE])
{
	QOS_INGGRP_LIST_ITEM *p;
	int32_t count = 0;

	qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s;%d: find all interfaces in inggrp%d", __func__, __LINE__, inggrp);
	qos_mgr_inggrp_lock_list();
	for ( p = g_qos_inggrp_list; p && (count < QOS_MGR_MAX_IF_PER_INGGRP); p = (QOS_INGGRP_LIST_ITEM *)p->next ) {
		if (p->ingress_group == inggrp) {
			qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: found interface %s in inggrp %d", __func__, __LINE__, p->ifname, inggrp); 
			strncpy(ifnames[count++], p->ifname, QOS_MGR_IF_NAME_SIZE);
		}
	}
	qos_mgr_inggrp_unlock_list();
	return count;
}
EXPORT_SYMBOL(qosal_get_qos_inggrp);

int32_t qosal_set_qos_inggrp(QOS_MGR_CMD_INGGRP_INFO *inggrp_info, QOS_INGGRP_LIST_ITEM **pp_item)
{
	QOS_INGGRP_LIST_ITEM *p_item = NULL;
	QOS_MGR_INGGRP_CFG hcfg;
	uint32_t ret=QOS_MGR_SUCCESS;

	qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: add %s to inggrp %d\n", __func__, __LINE__,
			inggrp_info->ifname, inggrp_info->ingress_group);

	/* First, check if interface is already added */
	if (qos_mgr_inggrp_lookup(inggrp_info->ifname, &p_item) == QOS_MGR_SUCCESS) {
		/* interface found in inggrp list - modify ingress group */
		if (QOS_MGR_INGGRP_VALID(inggrp_info->ingress_group) && inggrp_info->ingress_group != p_item->ingress_group) {
			qos_mgr_debug(DBG_ENABLE_MASK_ERR, "%s:%d: WARNING: modify %s ingress group %d -> %d (won't affect already active queues!)\n",
							__func__, __LINE__, inggrp_info->ifname, p_item->ingress_group, inggrp_info->ingress_group);
			p_item->ingress_group = inggrp_info->ingress_group;
		}
	} else if (QOS_MGR_INGGRP_INVALID(inggrp_info->ingress_group)) {
			qos_mgr_debug(DBG_ENABLE_MASK_ERR, "%s:%d: ERROR: invalid ingress group %d (%s)\n",
								__func__, __LINE__, inggrp_info->ingress_group, inggrp_info->ifname);
			return QOS_MGR_FAILURE;
	}


	memset(&hcfg, 0, sizeof(hcfg));
	hcfg.ingress_group = inggrp_info->ingress_group;
	qos_mgr_strncpy(hcfg.ifname, inggrp_info->ifname, QOS_MGR_IF_NAME_SIZE);
	if (qos_mgr_set_qos_inggrp(&hcfg, QOS_HAL) != QOS_MGR_SUCCESS) {
		qos_mgr_debug(DBG_ENABLE_MASK_ERR, "%s:%d: failed to set Ingress Grouping for HAL id %d\n", __func__, __LINE__, QOS_HAL);
		return QOS_MGR_FAILURE;
	}

	if (!p_item) {
		/* Allocate a new node and add to the inggrp list */
		p_item = qos_mgr_inggrp_alloc_item();
		if (!p_item) {
			qos_mgr_debug(DBG_ENABLE_MASK_ERR, "%s:%d: alloc qos inggrp list item failed \n", __func__, __LINE__);
			return QOS_MGR_ENOMEM;
		}

		qos_mgr_strncpy(p_item->ifname, inggrp_info->ifname, QOS_MGR_IF_NAME_SIZE);
		p_item->ingress_group = inggrp_info->ingress_group;
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: Add item %p ifname=%s, inggrp=%d\n", __func__, __LINE__,
					p_item, p_item->ifname, p_item->ingress_group);
		__qos_mgr_inggrp_add_item(p_item);
	}

	/* success */
	*pp_item = p_item;
	return ret;
}

static uint32_t qos_mgr_set_ingress_qos_generic(const char *ifname, uint32_t *flags)
{
	QOS_INGGRP_LIST_ITEM *p_item;
	uint32_t ret=QOS_MGR_SUCCESS;

	if ((*flags & QOS_MGR_Q_F_INGRESS) || (*flags & QOS_MGR_OP_F_INGRESS))	{
		ret = qos_mgr_inggrp_lookup(ifname, &p_item);
		if (ret) {
			qos_mgr_debug(DBG_ENABLE_MASK_ERR, "%s:%d: ifname %s does not exist in qos inggrp list\n", __func__, __LINE__, ifname);
			return ret;
		}
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s:%d: set ifname %s to ingress_group %d\n", __func__, __LINE__,
				ifname, p_item->ingress_group);
		*flags |= inggrp2flags(p_item->ingress_group);
		qos_mgr_inggrp_free_item(p_item); /* decrement reference counter */
	}

	return QOS_MGR_SUCCESS;
}

int32_t qos_mgr_ioctl_set_qos_ingress_group(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info)
{
	QOS_INGGRP_LIST_ITEM *p_item;
	int res = QOS_MGR_FAILURE;

	res = copy_from_user(&cmd_info->qos_inggrp_info, (void *)arg, sizeof(cmd_info->qos_inggrp_info));
	if (res != QOS_MGR_SUCCESS)
		return QOS_MGR_FAILURE;

	if (qosal_set_qos_inggrp(&cmd_info->qos_inggrp_info, &p_item) != QOS_MGR_SUCCESS)
		return QOS_MGR_FAILURE;

	return QOS_MGR_SUCCESS;
}
EXPORT_SYMBOL(qos_mgr_ioctl_set_qos_ingress_group);

int32_t qos_mgr_ioctl_get_qos_ingress_group(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info)
{
	/* Place Holder */
	return QOS_MGR_SUCCESS;
}
EXPORT_SYMBOL(qos_mgr_ioctl_get_qos_ingress_group);


/*
############# SUPPORTING INGRESS FUNCTION DEFINITIONS : END################
*/

/*
############# SUPPORTING WMM FUNCTION DEFINITIONS : START################
*/
#if DP_EVENT_CB
/* Notify callback registration ID (pointer) - used for deregister CB */
static void *g_notify_cb_id;
int32_t g_wmm_1;

#define WMM_ADD 1
#define WMM_ENABLED 1

bool g_qos_mgr_wmm_enable_flag = WMM_ENABLED;

struct wlan_iface_wmm_info {
	char ifname[QOS_MGR_IF_NAME_SIZE];
	bool iface_reg_status;
	bool queues_present;
	struct list_head list;
};

static LIST_HEAD(g_wlan_iface_wmm_info_list);

void qos_hal_global_wmm_status()
{
	printk("WMM Functionality Status: [%s]\n", g_qos_mgr_wmm_enable_flag ? "Enabled":"Disabled");
	return;
}

int32_t qos_hal_global_wmm_enable_disable(bool oper)
{
	int32_t ret = QOS_MGR_SUCCESS;
	struct wlan_iface_wmm_info *p, *n;

	printk("WMM Functionality Status: Existing:[%s] New:[%s]\n",
			g_qos_mgr_wmm_enable_flag ? "Enable":"Disable",
			oper ? "Enable":"Disable");

	if (oper == g_qos_mgr_wmm_enable_flag) {
		printk("No change in WMM Functionality Status.\n");
		return ret;
	}

	g_qos_mgr_wmm_enable_flag = oper;

	list_for_each_entry_safe(p, n, &g_wlan_iface_wmm_info_list, list) {
		printk("<%s> Ifname:[%s] Reg Status:[%s] Queues Present:[%s]\n", __func__,
				p->ifname,
				p->iface_reg_status ? "Registered" : "Un-Registered",
				p->queues_present ? "Yes" : "No");

		if (p->iface_reg_status == 1) {
			if (oper == WMM_ADD) {
				if (qos_hal_wmm_add(p->ifname) == QOS_MGR_SUCCESS)
					p->queues_present = 1;
			} else {
				if (qos_hal_wmm_del(p->ifname) == QOS_MGR_SUCCESS)
					p->queues_present = 0;
			}
		}
	}

	return ret;
}

static void qos_manager_update_wmm_info (
	struct wlan_iface_wmm_info *wlan_iface_wmm_info,
	char *ifname,
	bool iface_reg_status,
	bool queues_present)
{
	wlan_iface_wmm_info->queues_present = 0;
	wlan_iface_wmm_info->iface_reg_status = 1;
	snprintf(wlan_iface_wmm_info->ifname, QOS_MGR_IF_NAME_SIZE, ifname);

	return;
}

int32_t qos_manager_dp_event_handler(struct dp_event_info *info)
{
	int32_t ret = QOS_MGR_SUCCESS;
	dp_subif_t *dp_subif = NULL;
	struct wlan_iface_wmm_info *p, *n;
	struct wlan_iface_wmm_info *wlan_iface_wmm_info = NULL;

	switch (info->type) {
	case DP_EVENT_REGISTER_SUBIF:
	{
		pr_info("Register subif:=> netdevice:[%s] subif info:[%d:%d]\n",
				info->reg_subif_info.dev->name,
				info->reg_subif_info.dpid,
				info->reg_subif_info.subif);
#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
		dp_subif = kzalloc(sizeof(dp_subif_t), GFP_ATOMIC);
		if (!dp_subif) {
			pr_err("<%s> DP subif allocation failed\n", __func__);
			return QOS_MGR_FAILURE;
		}

		if (dp_get_netif_subifid(info->reg_subif_info.dev, NULL, NULL, 0, dp_subif, 0) != QOS_MGR_SUCCESS) {
			pr_err("dp_get_netif_subifid failed\n");
		}

		pr_debug("<%s>Alloc Flag: 0x%x  dp_subif->alloc_flag & DP_F_FAST_WLAN:0x%x DP_F_FAST_WLAN:0x%x \n",
				__func__, dp_subif->alloc_flag, dp_subif->alloc_flag & DP_F_FAST_WLAN, DP_F_FAST_WLAN);

		if (((dp_subif->alloc_flag & DP_F_FAST_WLAN) == DP_F_FAST_WLAN) &&
				(g_qos_mgr_wmm_enable_flag == WMM_ENABLED)) {

			list_for_each_entry_safe(p, n, &g_wlan_iface_wmm_info_list, list) {
				if (strcmp(p->ifname, info->reg_subif_info.dev->name) == 0) {
					printk("Interface [%s] exists in list.\n", info->reg_subif_info.dev->name);
					wlan_iface_wmm_info = p;
					break;
				}
			}

			if (wlan_iface_wmm_info == NULL) {
				printk("New interface [%s] registered.\n", info->reg_subif_info.dev->name);
				wlan_iface_wmm_info = kmalloc(sizeof(wlan_iface_wmm_info), GFP_ATOMIC);
				if (wlan_iface_wmm_info == NULL) {
					pr_err("%s:%d kmalloc failed.\n", __func__, __LINE__);
					kfree(dp_subif);
					return QOS_MGR_FAILURE;
				}
				list_add(&wlan_iface_wmm_info->list, &g_wlan_iface_wmm_info_list);
			}

			if (qos_hal_wmm_add(info->reg_subif_info.dev->name) == QOS_MGR_SUCCESS)
				qos_manager_update_wmm_info(wlan_iface_wmm_info, info->reg_subif_info.dev->name, 1, 1);
			else
				qos_manager_update_wmm_info(wlan_iface_wmm_info, info->reg_subif_info.dev->name, 1, 0);
		}
		kfree(dp_subif);
#endif
		break;
	}
	case DP_EVENT_DE_REGISTER_SUBIF:
	{

		pr_info("De-Register subif:=> netdevice:[%s] subif info:[%d:%d]\n",
				info->de_reg_subif_info.dev->name,
				info->de_reg_subif_info.dpid,
				info->de_reg_subif_info.subif);

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
		dp_subif = kzalloc(sizeof(dp_subif_t), GFP_ATOMIC);
		if (!dp_subif) {
			pr_err("<%s> DP subif allocation failed\n", __func__);
			return QOS_MGR_FAILURE;
		}
		if (dp_get_netif_subifid(info->reg_subif_info.dev, NULL, NULL, 0, dp_subif, 0) != QOS_MGR_SUCCESS) {
			pr_err("dp_get_netif_subifid failed\n");
		}

		pr_debug("<%s>Alloc Flag: 0x%x  dp_subif->alloc_flag & DP_F_FAST_WLAN:0x%x DP_F_FAST_WLAN:0x%x \n",
				__func__, dp_subif->alloc_flag, dp_subif->alloc_flag & DP_F_FAST_WLAN, DP_F_FAST_WLAN);

		if (((dp_subif->alloc_flag & DP_F_FAST_WLAN) == DP_F_FAST_WLAN) &&
				(g_qos_mgr_wmm_enable_flag == WMM_ENABLED)) {

			list_for_each_entry_safe(p, n, &g_wlan_iface_wmm_info_list, list) {
				if (strcmp(p->ifname, info->reg_subif_info.dev->name) == 0) {
					printk("Interface [%s] exists in list.\n", info->reg_subif_info.dev->name);
					wlan_iface_wmm_info = p;
					break;
				}
			}

			if (!wlan_iface_wmm_info) {
				printk("Interface [%s] is not registered.\n", info->reg_subif_info.dev->name);
			} else {
				if (qos_hal_wmm_del(info->reg_subif_info.dev->name) == QOS_MGR_SUCCESS) {
					qos_manager_update_wmm_info(wlan_iface_wmm_info, info->reg_subif_info.dev->name, 0, 0);
				} else {
					printk("WMM Queue deletion for interface [%s] is failed.\n", info->reg_subif_info.dev->name);
					qos_manager_update_wmm_info(wlan_iface_wmm_info, info->reg_subif_info.dev->name, 0, 1);
				}
			}
		}
		kfree(dp_subif);
#endif
		break;
	}
	case DP_EVENT_REGISTER_DEV:
	{
		pr_info("Register device\n");
		break;
	}
	case DP_EVENT_DE_REGISTER_DEV:
	{
		pr_info("De- Register device\n");
		break;
	}
	case DP_EVENT_ALLOC_PORT:
	{
		pr_info("Alloc Port\n");
		break;	
	}
	default:
	return 0;
	}
	return ret;
}

int32_t qos_manager_register_event_dp(void)
{
	int32_t ret = QOS_MGR_SUCCESS;
	struct dp_event dp_event = {0};

	dp_event.owner = DP_EVENT_OWNER_PPA;
	dp_event.type = DP_EVENT_REGISTER_SUBIF | DP_EVENT_DE_REGISTER_SUBIF;
	dp_event.dp_event_cb = qos_manager_dp_event_handler;

	ret = dp_register_event_cb(&dp_event, 0);
	if (ret != DP_SUCCESS) {
		pr_err("Can't register DP_EVENT callback\n");
		return ret;
	}

	/* Save callback ID for deregistration purpose */
	g_notify_cb_id = dp_event.id;

	return ret;
}

int32_t qos_manager_deregister_event_dp(void)
{
	int32_t ret = QOS_MGR_SUCCESS;
	struct dp_event dp_event = {0};

	dp_event.owner = DP_EVENT_OWNER_PPA;
	dp_event.type = DP_EVENT_REGISTER_SUBIF | DP_EVENT_DE_REGISTER_SUBIF;
	dp_event.id = g_notify_cb_id;
	dp_event.dp_event_cb = qos_manager_dp_event_handler;

	ret = dp_register_event_cb(&dp_event, DP_F_DEREGISTER);
	if (ret != DP_SUCCESS) {
		pr_err("Can't de-register DP_EVENT callback\n");
		return ret;
	}

	return ret;
}
#endif

int32_t qos_hal_spl_conn_mod(uint8_t conn_type, uint32_t queue_len)
{
	__qos_hal_spl_conn_init(conn_type, queue_len);
	return 0;
}

#if defined(WMM_QOS_CONFIG) && WMM_QOS_CONFIG
int32_t qos_mgr_create_c2p_map_for_wmm(QOS_MGR_IFNAME ifname[16],uint8_t c2p[])
{
	int32_t ret = QOS_MGR_ENOTAVAIL;
#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
	int32_t i,j,c=0;
	QOS_QUEUE_LIST_ITEM *p;

	j = 0;
	qos_mgr_memset(&c2p[0],0,16);
	qos_mgr_queue_lock_list();
	for ( p = g_qos_queue; p; p = p->next ) {
		if ( strcmp(p->ifname,ifname) == 0) {
			ret = QOS_MGR_SUCCESS;
			for(i=0;i< p->tc_no;i++) {
				c2p[c] = (int)(p->tc_map[i]);
				c++;
			}
			j++;
		}
	}

	qos_mgr_queue_unlock_list();

	j = 0; /* USING default MAP. */
	if(j <= 1) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"%s %s %d num. Queues <= 1: Return QOS_MGR_ENOTAVAIL to set default Map !!!\n", __FILE__,__FUNCTION__,__LINE__);
		ret = QOS_MGR_ENOTAVAIL;
	}
#endif /* !CONFIG_X86_INTEL_LGM */

	return ret;
}
EXPORT_SYMBOL(qos_mgr_create_c2p_map_for_wmm);

static int32_t qos_mgr_set_wlan_wmm_prio(QOS_MGR_IFNAME *ifname,int32_t port_id,int8_t caller_flag)
{
	QOS_MGR_NETIF *netif = NULL;
	dp_subif_t *dp_subif = NULL;
	uint8_t *class2prio;
	uint8_t c2p[MAX_TC_NUM] = {0};
	uint8_t cl2p[MAX_TC_NUM] = {0};
	int32_t i;
	int8_t port = 0;

	if(ifname == NULL) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ifname value is NULL \n");
		return QOS_MGR_FAILURE;
	}

	netif = qos_mgr_get_netif(ifname);
	switch (caller_flag) {
		case 1:
			qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d caller_case is %d!!!\n", __FILE__,
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
			qos_mgr_call_class2prio_notifiers(QOS_MGR_CLASS2PRIO_DEFAULT,
						      port_id, netif,
						      class2prio);
			break;
		case 2:
		case 3:
			qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d caller_case is %d!!!\n", __FILE__,
					__FUNCTION__,__LINE__,caller_flag);

			dp_subif = kzalloc(sizeof(dp_subif_t), GFP_ATOMIC);
			if (!dp_subif) {
				qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
					"<%s> DP subif allocation failed\n",
					__func__);
				return QOS_MGR_FAILURE;
			}

			if (dp_get_netif_subifid(netif, NULL, NULL, NULL,
						 dp_subif, 0) != DP_SUCCESS) {
				qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d dp_get_netif_subifid failed!!!\n",
					__FILE__,__FUNCTION__,__LINE__);
				//return QOS_MGR_FAILURE;
			}

			if (!(dp_subif->alloc_flag & DP_F_FAST_WLAN) || (dp_subif->alloc_flag & DP_F_FAST_DSL)) {
				qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d not a fastpath wave interface!!!\n",
					__FILE__,__FUNCTION__,__LINE__);
				kfree(dp_subif);
				return QOS_MGR_FAILURE;
			}

			if(qos_mgr_create_c2p_map_for_wmm(ifname,cl2p) == QOS_MGR_ENOTAVAIL) {
				qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d Setting default Map!!!\n",
					__FILE__,__FUNCTION__,__LINE__);
				if(!strncmp(ifname,"wlan0", 5))
					port=0;
				else if(!strcmp(ifname,"wlan1"))
					port=1;
				else if(!strncmp(ifname,"wlan2", 5))
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
			qos_mgr_call_class2prio_notifiers(QOS_MGR_CLASS2PRIO_CHANGE,
						      dp_subif->port_id, netif,
						      class2prio);
			kfree(dp_subif);
			break;
		default:
			qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Invalid Wmm caller case \n");
			break;
	}

	return QOS_MGR_SUCCESS;
}
#endif /* WMM_QOS_CONFIG */



#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
int32_t qos_hal_wmm_add(char *ifname)
{
	int32_t ret = QOS_MGR_SUCCESS;
	QOS_MGR_CMD_QUEUE_INFO wmm_q_info = {0};
	QOS_QUEUE_LIST_ITEM *p_item;
	uint32_t q_index = 0;

	pr_debug("<%s> Add WMM queue for wlan netdevice %s\n", __func__, ifname);
	snprintf(wmm_q_info.ifname, QOS_MGR_IF_NAME_SIZE, ifname);

	/*

	Traffic Type	Traffic Class(TC)   Priority
	------------    -----------------   ---------
	Voice (VO)	    6,7	    	    Highest
	Video (VI)	    4,5               -
	BestEffort (BE)	    0,3	              -
	Background (BK)	    1,2	    	    Lowest

	*/

	for (q_index=1; q_index<=4; q_index++) {
		wmm_q_info.priority = q_index;
		wmm_q_info.enable = 1;
		wmm_q_info.tc_no = 2;
		if (q_index == 1) { /* Voice (VO) */
			wmm_q_info.tc_map[0] = WMM_TC_VO_1;
			wmm_q_info.tc_map[1] = WMM_TC_VO_2;
		} if (q_index == 2) { /* Video (VI) */
			wmm_q_info.tc_map[0] = WMM_TC_VI_1;
			wmm_q_info.tc_map[1] = WMM_TC_VI_2;
		} if (q_index == 3) { /* Best Effort (BE) */
			wmm_q_info.tc_map[0] = WMM_TC_BE_1;
			wmm_q_info.tc_map[1] = WMM_TC_BE_2;
		} if (q_index == 4) { /* Background (BK) */
			wmm_q_info.tc_map[0] = WMM_TC_BK_1;
			wmm_q_info.tc_map[1] = WMM_TC_BK_2;
		}
		wmm_q_info.portid = 0; 
		wmm_q_info.sched = QOS_MGR_SCHED_SP;
		wmm_q_info.queue_num = q_index;

		if (qosal_add_qos_queue(
					&wmm_q_info,
					&p_item) != QOS_MGR_SUCCESS) {
			pr_err("Failed to create WMM Queues with q_prio:0x%x for %s\n", q_index, wmm_q_info.ifname);
			return QOS_MGR_FAILURE;
		}

#if defined(WMM_QOS_CONFIG) && WMM_QOS_CONFIG
	qos_mgr_set_wlan_wmm_prio(wmm_q_info.ifname,-1,2);
#endif /* WMM_QOS_CONFIG */

		if (p_item->p_entry < 0) {
			pr_err("Add Queue failed\n");
			return QOS_MGR_FAILURE;
		}

		pr_info("<%s> Returned Queue Id: [%d]\n",
				__func__, p_item->p_entry);
	}

	return ret;
}

int32_t qos_hal_wmm_del(char *ifname)
{
	int32_t ret = QOS_MGR_SUCCESS;
	QOS_MGR_CMD_QUEUE_INFO wmm_q_info = {0};
	QOS_QUEUE_LIST_ITEM *p_item;
	uint32_t q_index = 0;

	pr_debug("<%s> Del WMM queue for wlan netdevice %s\n", __func__, ifname);

	snprintf(wmm_q_info.ifname, QOS_MGR_IF_NAME_SIZE, ifname);
	wmm_q_info.portid = 0; 

	for (q_index=1; q_index<=4; q_index++) {
		wmm_q_info.queue_num = q_index;

		pr_debug("<%s> Deleting WMM: [ifname:%s Q Num:%d]\n", __func__, wmm_q_info.ifname, wmm_q_info.queue_num);

		ret = qos_mgr_queue_lookup(wmm_q_info.queue_num, wmm_q_info.ifname, &p_item);
		if( ret == QOS_MGR_ENOTAVAIL ) {
			pr_err("<%s> lookup failed for wlan netdevice %s\n", __func__, wmm_q_info.ifname);
			return QOS_MGR_FAILURE;
		} else {
			if (qosal_delete_qos_queue(&wmm_q_info, p_item) != QOS_MGR_SUCCESS) {
				pr_err("Failed to create WMM Queues with q_prio:0x%x for %s\n", q_index, wmm_q_info.ifname);
				return QOS_MGR_FAILURE;
			}

		}
		qos_mgr_queue_remove_item(wmm_q_info.queue_num, wmm_q_info.ifname, NULL);
		qos_mgr_queue_free_item(p_item);
	}

	return ret;
}
#endif /* CONFIG_X86_INTEL_LGM */

/*
############# SUPPORTING WMM FUNCTION DEFINITIONS : END################
*/

/*
############# INIT FUNCTION DEFINITIONS : START################
*/

int32_t qos_mgr_init_cfg( uint32_t flags, uint32_t hal_id)
{
	int32_t ret=QOS_MGR_SUCCESS;
	if ( (ret = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_QOS_INIT_CFG, NULL, 0)) == QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d qos init success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}

int32_t qos_mgr_uninit_cfg( uint32_t flags, uint32_t hal_id)
{
	int32_t ret=QOS_MGR_SUCCESS;
	if ( (ret = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_QOS_UNINIT_CFG, NULL, 0)) == QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d qos uninit success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}

int32_t qosal_eng_init_cfg(void)
{
	int32_t ret = QOS_MGR_SUCCESS;

	ret = qos_mgr_init_cfg(0,QOS_HAL);

	return ret;		
}

int32_t qosal_eng_uninit_cfg(void)
{
	int32_t ret = QOS_MGR_SUCCESS;
	ret = qos_mgr_uninit_cfg(0,QOS_HAL);
	return ret;		
}

int32_t qos_mgr_ioctl_qos_init_cfg(unsigned int cmd)
{
	int32_t ret = QOS_MGR_SUCCESS;

	switch ( cmd ) {
		case QOS_MGR_CMD_ENG_QUEUE_INIT: {
			ret = qosal_eng_init_cfg();
			break;
		}
		case QOS_MGR_CMD_ENG_QUEUE_UNINIT: {
			ret = qosal_eng_uninit_cfg();
			break;
		}
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
int32_t qos_mgr_modify_qos_queue( char *ifname, QOS_MGR_MOD_QUEUE_CFG *q, uint32_t flags, uint32_t hal_id)
{
	int32_t ret=QOS_MGR_SUCCESS;
	QOS_Q_MOD_CFG tmu_q;
	memset(&tmu_q,0x00,sizeof(QOS_Q_MOD_CFG));
	tmu_q.ifname = ifname;
	tmu_q.portid = q->portid;
	tmu_q.priority = q->priority;
	tmu_q.qlen = q->qlen;
	tmu_q.q_type = q->q_type;
	tmu_q.weight = q->weight;
	if(q->drop.mode == QOS_MGR_DROP_RED) {
		tmu_q.drop.mode = QOS_MGR_DROP_RED;
		tmu_q.drop.wred.min_th0 = q->drop.wred.min_th0;
		tmu_q.drop.wred.max_th0 = q->drop.wred.max_th0;
		tmu_q.drop.wred.max_p0 = q->drop.wred.max_p0;
	} else if(q->drop.mode == QOS_MGR_DROP_WRED) {
		tmu_q.drop.mode = QOS_MGR_DROP_WRED;
		tmu_q.drop.wred.weight = q->drop.wred.weight;
		tmu_q.drop.wred.min_th0 = q->drop.wred.min_th0;
		tmu_q.drop.wred.max_th0 = q->drop.wred.max_th0;
		tmu_q.drop.wred.max_p0 = q->drop.wred.max_p0;
		tmu_q.drop.wred.min_th1 = q->drop.wred.min_th1;
		tmu_q.drop.wred.max_th1 = q->drop.wred.max_th1;
		tmu_q.drop.wred.max_p1 = q->drop.wred.max_p1;
	} else {
		tmu_q.drop.mode = QOS_MGR_DROP_TAIL;
	}

	tmu_q.flags = q->flags;
	tmu_q.q_id = q->queue_id;
	
	if ( (ret = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_QOS_MODQUE_CFG, &tmu_q, 0)) == QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d modify success!!!\n", __FILE__,__FUNCTION__,__LINE__);
		q->queue_id = tmu_q.q_id;
	} else
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d modify failure!!!\n", __FILE__,__FUNCTION__,__LINE__);
	return ret;
}

QOS_QUEUE_LIST_ITEM *qos_mgr_queue_alloc_item(void)
{
	QOS_QUEUE_LIST_ITEM *obj;

	obj = (QOS_QUEUE_LIST_ITEM *)qos_mgr_malloc(sizeof(*obj));
	if (obj) {
		qos_mgr_memset(obj, 0, sizeof(*obj));
		qos_mgr_atomic_set(&obj->count, 1);
	}
	return obj;
}

void qos_mgr_queue_free_item(QOS_QUEUE_LIST_ITEM *obj)
{
	if (qos_mgr_atomic_dec(&obj->count) == 0)
		qos_mgr_free(obj);
}

void qos_mgr_queue_lock_list(void)
{
	qos_mgr_lock_get(&g_qos_queue_lock);
}

void qos_mgr_queue_unlock_list(void)
{
	qos_mgr_lock_release(&g_qos_queue_lock);
}

void __qos_mgr_queue_add_item(QOS_QUEUE_LIST_ITEM *obj)
{
	qos_mgr_atomic_inc(&obj->count);
	obj->next = g_qos_queue;
	g_qos_queue = obj;
}

void qos_mgr_queue_remove_item(int32_t q_num, QOS_MGR_IFNAME ifname[16], QOS_QUEUE_LIST_ITEM **pp_info)
{
	QOS_QUEUE_LIST_ITEM *p_prev, *p_cur;

	if (pp_info)
		*pp_info = NULL;
	p_prev = NULL;
	qos_mgr_queue_lock_list();
	for (p_cur = g_qos_queue; p_cur; p_prev = p_cur, p_cur = p_cur->next)
		if ((p_cur->queue_num == q_num) && (strcmp(p_cur->ifname, ifname) == 0)) {
			if (!p_prev)
				g_qos_queue = p_cur->next;
			else
				p_prev->next = p_cur->next;
			if (pp_info)
				*pp_info = p_cur;
			else
				qos_mgr_queue_free_item(p_cur);
			break;
		}
	qos_mgr_queue_unlock_list();
}

void qos_mgr_queue_free_list(void)
{
	QOS_QUEUE_LIST_ITEM *obj;

	qos_mgr_queue_lock_list();
	while (g_qos_queue) {
		obj = g_qos_queue;
		g_qos_queue = g_qos_queue->next;

		qos_mgr_queue_free_item(obj);
		obj = NULL;
	}
	qos_mgr_queue_unlock_list();
}

int32_t __qos_mgr_intfid_lookup(char *ifname, uint32_t *intfid)
{
	int32_t ret = QOS_MGR_ENOTAVAIL;
	QOS_QUEUE_LIST_ITEM *p;

	for (p = g_qos_queue; p; p = p->next)
		if ((strncmp(p->ifname, ifname, strlen(ifname)) == 0)) {
			ret = QOS_MGR_SUCCESS;
			*intfid = p->intfId;
			break;
		}

	return ret;
}

int32_t qos_mgr_intfid_lookup(char *ifname, uint32_t *intfid)
{
	int32_t ret;
	qos_mgr_queue_lock_list();
	ret = __qos_mgr_intfid_lookup(ifname, intfid);
	qos_mgr_queue_unlock_list();

	return ret;
}

int32_t __qos_mgr_queue_lookup(int32_t q_num, QOS_MGR_IFNAME ifname[16], QOS_QUEUE_LIST_ITEM **pp_info)
{
	int32_t ret = QOS_MGR_ENOTAVAIL;
	QOS_QUEUE_LIST_ITEM *p;

	for (p = g_qos_queue; p; p = p->next)
		if ((p->queue_num == q_num) && (strcmp(p->ifname, ifname) == 0)) {
			ret = QOS_MGR_SUCCESS;
			if (pp_info) {
				qos_mgr_atomic_inc(&p->count);
				*pp_info = p;
			}
			break;
		}

	return ret;
}

int32_t qos_mgr_queue_lookup(int32_t qnum, QOS_MGR_IFNAME ifname[16], QOS_QUEUE_LIST_ITEM **pp_info)
{
	int32_t ret;
	qos_mgr_queue_lock_list();
	ret = __qos_mgr_queue_lookup(qnum, ifname, pp_info);
	qos_mgr_queue_unlock_list();

	return ret;
}

int32_t qos_mgr_ioctl_add_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{
	QOS_QUEUE_LIST_ITEM *p_item;
	int res = QOS_MGR_FAILURE;

	res = copy_from_user(&cmd_info->qos_queue_info, (void *)arg, sizeof(cmd_info->qos_queue_info));
	if (res != QOS_MGR_SUCCESS)
		return QOS_MGR_FAILURE;

	if (cmd_info->qos_queue_info.tc_no > MAX_TC_NUM) {
		pr_err("Ivalid tc no..\n");
		res = QOS_MGR_FAILURE;
		return QOS_MGR_FAILURE;
	}

	if (qosal_add_qos_queue(&cmd_info->qos_queue_info,&p_item) != QOS_MGR_SUCCESS)
		return QOS_MGR_FAILURE;

#if defined(WMM_QOS_CONFIG) && WMM_QOS_CONFIG
	qos_mgr_set_wlan_wmm_prio(cmd_info->qos_queue_info.ifname,-1,2);
#endif /* WMM_QOS_CONFIG */

	return QOS_MGR_SUCCESS;
}

int32_t qos_mgr_ioctl_modify_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{
	int32_t ret = QOS_MGR_SUCCESS;
	return ret;
}

int32_t qos_mgr_ioctl_delete_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{

	QOS_QUEUE_LIST_ITEM *p_item;
	QOS_SHAPER_LIST_ITEM *p_s_item;
	int32_t ret = QOS_MGR_FAILURE;

	ret = copy_from_user( &cmd_info->qos_queue_info, (void *)arg, sizeof(cmd_info->qos_queue_info));
	if (ret != QOS_MGR_SUCCESS)
		return QOS_MGR_FAILURE;

	ret = qos_mgr_queue_lookup(cmd_info->qos_queue_info.queue_num,cmd_info->qos_queue_info.ifname,&p_item);
	if( ret == QOS_MGR_ENOTAVAIL ) {
		return QOS_MGR_FAILURE;
	} else {
		if (qos_mgr_set_ingress_qos_generic(cmd_info->qos_queue_info.ifname, &cmd_info->qos_queue_info.flags))
			return QOS_MGR_FAILURE;
		ret = qosal_delete_qos_queue(&cmd_info->qos_queue_info,p_item);
	}
	/* Delete Shaper assigned to the Queue when the Queue is deleted */
	if(ret == QOS_MGR_SUCCESS) {
		if(qos_mgr_shaper_lookup(cmd_info->qos_queue_info.shaper_num,
				cmd_info->qos_queue_info.ifname,&p_s_item) == QOS_MGR_SUCCESS) {
			qos_mgr_shaper_remove_item(p_s_item->shaperid,p_s_item->ifname,NULL);
			qos_mgr_shaper_free_item(p_s_item);
		}
	}
	qos_mgr_queue_remove_item(p_item->queue_num,p_item->ifname,NULL);
	qos_mgr_queue_free_item(p_item);

	return ret;
}

int32_t qos_mgr_get_pppoa_base_if( char *ifname, QOS_MGR_IFNAME base_ifname[QOS_MGR_IF_NAME_SIZE])
{
	return QOS_MGR_FAILURE;
}

int32_t qos_mgr_modify_qos_subif_to_port(QOS_MGR_CMD_SUBIF_PORT_INFO *subif_port_info)
{
	int32_t ret=QOS_MGR_SUCCESS;
	QOS_MOD_SUBIF_PORT_CFG SubifPort_info;

	memset(&SubifPort_info, 0, sizeof(QOS_MOD_SUBIF_PORT_CFG));

	qos_mgr_strncpy(SubifPort_info.ifname, subif_port_info->ifname, QOS_MGR_IF_NAME_SIZE);
	SubifPort_info.port_id = subif_port_info->port_id;
	SubifPort_info.priority_level = subif_port_info->priority_level;
	SubifPort_info.weight = subif_port_info->weight;
	SubifPort_info.flags = subif_port_info->flags;

	//if ( (ret = qos_mgr_hsel_mod_subif_port_cfg( &SubifPort_info, 0, TMU_HAL) ) != QOS_MGR_SUCCESS ) {
	if ( (ret = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_MOD_SUBIF_PORT_CFG, &SubifPort_info, 0)) == QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d hal select Mod subif to port failed!!!\n",
				__FILE__,__FUNCTION__,__LINE__);
		return QOS_MGR_FAILURE;
	}
	return ret;
}

int32_t qos_mgr_ioctl_mod_subif_port_config(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{
	int res = QOS_MGR_FAILURE;

	res = copy_from_user( &cmd_info->subif_port_info, (void *)arg, sizeof(cmd_info->subif_port_info));
	if (res != QOS_MGR_SUCCESS)
		return QOS_MGR_FAILURE;

	if( qos_mgr_modify_qos_subif_to_port(&cmd_info->subif_port_info)!= QOS_MGR_SUCCESS) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d Mod subif to port failed!!!\n", __FILE__,__FUNCTION__,__LINE__);
		return QOS_MGR_FAILURE;
	}

	return QOS_MGR_SUCCESS;
}

int32_t qos_mgr_add_qos_queue(char *ifname, QOS_MGR_ADD_QUEUE_CFG *q, uint32_t flags, uint32_t hal_id)
{
	int32_t ret = QOS_MGR_SUCCESS;
	int32_t k;
	char dev_name[QOS_MGR_IF_NAME_SIZE];
	QOS_Q_ADD_CFG tmu_q;
	memset(&tmu_q, 0x00, sizeof(QOS_Q_ADD_CFG));

	tmu_q.intfId_en = q->intfId_en;
	if (q->intfId_en)
		tmu_q.intfId = q->intfId;

	if (qos_mgr_get_pppoa_base_if(ifname, dev_name) == QOS_MGR_SUCCESS) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, " %s:%d Pseudo PPPOA interface = %s\n", __FUNCTION__, __LINE__, dev_name);
		tmu_q.dev_name = dev_name;
		tmu_q.ifname = ifname;
		tmu_q.flags |= QOS_MGR_F_PPPOATM;
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
		case QOS_MGR_DROP_TAIL:
			tmu_q.qlen = q->qlen;
			break;
		case QOS_MGR_DROP_RED:
			break;
		case QOS_MGR_DROP_WRED:
			tmu_q.drop.wred.weight = q->drop.wred.weight;
			tmu_q.drop.wred.min_th0 = q->drop.wred.min_th0;
			tmu_q.drop.wred.min_th1 = q->drop.wred.min_th1;
			tmu_q.drop.wred.max_th0 = q->drop.wred.max_th0;
			tmu_q.drop.wred.max_th1 = q->drop.wred.max_th1;
			tmu_q.drop.wred.max_p0 = q->drop.wred.max_p0;
			tmu_q.drop.wred.max_p1 = q->drop.wred.max_p1;
			break;
		case QOS_MGR_DROP_CODEL:
			break;
		default:
			tmu_q.qlen = q->qlen;
			break;
	}

	for (k = 0; k < MAX_TC_NUM; k++)
		tmu_q.tc_map[k] = q->tc_map[k];

	tmu_q.tc_no = q->tc_no;

	ret = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_QOS_ADDQUE_CFG, &tmu_q, 0);
	if (ret == QOS_MGR_SUCCESS) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %s %d add success!!!\n", __FILE__, __FUNCTION__, __LINE__);
		q->queue_id = tmu_q.q_id;
	}
	return ret;
}

int32_t qos_mgr_delete_qos_queue(char *ifname, char *dev_name, int32_t priority, uint32_t *queue_id, uint32_t portid,
			uint32_t hal_id, uint32_t flags)
{
	int32_t ret = QOS_MGR_SUCCESS;
	uint32_t infId;
	QOS_Q_DEL_CFG tmu_q;
	memset(&tmu_q, 0x00, sizeof(QOS_Q_DEL_CFG));

	/** For those netdevice which is not registered to DP, intfId is used 
	to store the cqm deq index which is passed by the caller while queue
	is added.
	*/
	if(qos_mgr_intfid_lookup(dev_name, &infId) == QOS_MGR_SUCCESS) {
		tmu_q.intfId = infId;
	}
	qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, " %s:%d Pseudo PPPOA interface = %s, ifname = %s, flags = %d \n",
			__FUNCTION__, __LINE__, dev_name, ifname, flags);
	tmu_q.dev_name = dev_name;
	tmu_q.ifname = ifname;
	tmu_q.flags = flags;
	tmu_q.portid = portid;
	tmu_q.q_id = *queue_id;
	tmu_q.priority = priority;
	tmu_q.intfId = infId;

	ret = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_QOS_DELQUE_CFG, &tmu_q, 0);
	if (ret == QOS_MGR_SUCCESS)
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %s %d delete success!!!\n", __FILE__, __FUNCTION__, __LINE__);
	return ret;
}

int32_t qosal_add_qos_queue(QOS_MGR_CMD_QUEUE_INFO *q_info, QOS_QUEUE_LIST_ITEM **pp_item)
{
	QOS_QUEUE_LIST_ITEM *p_item;
	QOS_MGR_ADD_QUEUE_CFG add_q_cfg;
	uint32_t k, ret = QOS_MGR_SUCCESS;
	char dev_name[QOS_MGR_IF_NAME_SIZE];

	if (qos_mgr_set_ingress_qos_generic(q_info->ifname, &q_info->flags))
		return QOS_MGR_FAILURE;

	memset(&add_q_cfg, 0x00, sizeof(QOS_MGR_ADD_QUEUE_CFG));
	p_item = qos_mgr_queue_alloc_item();
	if (!p_item) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "alloc queue list item failed \n");
		return QOS_MGR_ENOMEM;
	}
	qos_mgr_strncpy(p_item->ifname, q_info->ifname, QOS_MGR_IF_NAME_SIZE);
	qos_mgr_strncpy(p_item->dev_name, q_info->ifname, QOS_MGR_IF_NAME_SIZE);
	p_item->weight = q_info->weight;
	p_item->priority = q_info->priority;
	p_item->portid = q_info->portid;
	p_item->queue_num = q_info->queue_num;
	p_item->shaper_num = q_info->queue_num;
	p_item->drop.mode = q_info->drop.mode;
	switch (q_info->drop.mode) {
	case QOS_MGR_DROP_TAIL:
		p_item->qlen = q_info->qlen;
		break;
	case QOS_MGR_DROP_RED:
		break;
	case QOS_MGR_DROP_WRED:
		p_item->drop.wred.weight = q_info->drop.wred.weight;
		p_item->drop.wred.min_th0 = q_info->drop.wred.min_th0;
		p_item->drop.wred.min_th1 = q_info->drop.wred.min_th1;
		p_item->drop.wred.max_th0 = q_info->drop.wred.max_th0;
		p_item->drop.wred.max_th1 = q_info->drop.wred.max_th1;
		p_item->drop.wred.max_p0 = q_info->drop.wred.max_p0;
		p_item->drop.wred.max_p1 = q_info->drop.wred.max_p1;
		break;
	case QOS_MGR_DROP_CODEL:
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

	if (qos_mgr_get_pppoa_base_if(p_item->ifname, dev_name) == QOS_MGR_SUCCESS) {
		qos_mgr_strncpy(p_item->dev_name, dev_name, QOS_MGR_IF_NAME_SIZE);
		p_item->flags |= QOS_MGR_F_PPPOATM;
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, " %s:%d Pseudo PPPOA interface = %s : flags = %d\n", __FUNCTION__,
			__LINE__, dev_name, p_item->flags);
	}

	add_q_cfg.portid = q_info->portid;
	add_q_cfg.priority = q_info->priority;
	add_q_cfg.weight = q_info->weight;
	add_q_cfg.q_type = q_info->sched;
	for (k = 0; k < MAX_TC_NUM; k++)
		add_q_cfg.tc_map[k] = q_info->tc_map[k];
	add_q_cfg.tc_no = q_info->tc_no;
	add_q_cfg.drop.mode = q_info->drop.mode;
	switch (q_info->drop.mode) {
		case QOS_MGR_DROP_TAIL:
			add_q_cfg.qlen = q_info->qlen;
				break;
			case QOS_MGR_DROP_RED:
				break;
			case QOS_MGR_DROP_WRED:
				add_q_cfg.drop.wred.weight = q_info->drop.wred.weight;
				add_q_cfg.drop.wred.min_th0 = q_info->drop.wred.min_th0;
				add_q_cfg.drop.wred.min_th1 = q_info->drop.wred.min_th1;
				add_q_cfg.drop.wred.max_th0 = q_info->drop.wred.max_th0;
				add_q_cfg.drop.wred.max_th1 = q_info->drop.wred.max_th1;
				add_q_cfg.drop.wred.max_p0 = q_info->drop.wred.max_p0;
				add_q_cfg.drop.wred.max_p1 = q_info->drop.wred.max_p1;
					break;
			case QOS_MGR_DROP_CODEL:
					break;
			default:
				add_q_cfg.qlen = q_info->qlen;
				break;
	}
	if (q_info->flowId_en) {
		add_q_cfg.intfId_en = 1;
		add_q_cfg.intfId = q_info->flowId;
	}

	ret = qos_mgr_add_qos_queue(q_info->ifname, &add_q_cfg, q_info->flags, QOS_HAL);

	if (ret == QOS_MGR_SUCCESS) {
		p_item->p_entry = (add_q_cfg.queue_id);
		__qos_mgr_queue_add_item(p_item);
	} else {
		qos_mgr_debug(DBG_ENABLE_MASK_ERR, "%s:%d: qos_mgr_add_qos_queue failed\n", __func__, __LINE__);
		qos_mgr_queue_free_item(p_item);
		*pp_item = NULL;
		return QOS_MGR_FAILURE;
	}

	*pp_item = p_item;
	return QOS_MGR_SUCCESS;
}

int32_t qosal_modify_qos_queue(QOS_MGR_CMD_QUEUE_INFO *q_info, QOS_QUEUE_LIST_ITEM **pp_item)
{
	int32_t ret = QOS_MGR_SUCCESS;
	QOS_QUEUE_LIST_ITEM *p_q_item;
	QOS_QUEUE_LIST_ITEM *p_q_item1;
	QOS_MGR_MOD_QUEUE_CFG mod_q_cfg;
	uint32_t *tmp;

	p_q_item1 = *pp_item;
	memset(&mod_q_cfg, 0x00, sizeof(QOS_MGR_MOD_QUEUE_CFG));
	/* Start of list del and add operation*/
	p_q_item = qos_mgr_queue_alloc_item();
	if (!p_q_item)
		goto UPDATE_FAILED;

	/* Update all info into p_q_item */
	qos_mgr_strncpy(p_q_item->ifname, q_info->ifname, QOS_MGR_IF_NAME_SIZE);
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
	p_q_item->p_entry = p_q_item1->p_entry;
	p_q_item->queue_num = p_q_item1->queue_num;

	qos_mgr_queue_remove_item(p_q_item1->queue_num, p_q_item1->ifname, NULL);
	qos_mgr_queue_free_item(p_q_item1);

	__qos_mgr_queue_add_item(p_q_item);
	/* End of list del and add operation*/

					mod_q_cfg.portid = q_info->portid;
					mod_q_cfg.priority = q_info->priority;
					mod_q_cfg.qlen = q_info->qlen;
					mod_q_cfg.q_type = q_info->sched;
					mod_q_cfg.weight = q_info->weight;
					if (q_info->drop.mode == QOS_MGR_DROP_RED) {
						mod_q_cfg.drop.mode = QOS_MGR_DROP_RED;
						mod_q_cfg.drop.wred.min_th0 = q_info->drop.wred.min_th0;
						mod_q_cfg.drop.wred.max_th0 = q_info->drop.wred.max_th0;
						mod_q_cfg.drop.wred.max_p0 = q_info->drop.wred.max_p0;
					} else if (q_info->drop.mode == QOS_MGR_DROP_WRED) {
						mod_q_cfg.drop.mode = QOS_MGR_DROP_WRED;
						mod_q_cfg.drop.wred.weight = q_info->drop.wred.weight;
						mod_q_cfg.drop.wred.min_th0 = q_info->drop.wred.min_th0;
						mod_q_cfg.drop.wred.max_th0 = q_info->drop.wred.max_th0;
						mod_q_cfg.drop.wred.max_p0 = q_info->drop.wred.max_p0;
						mod_q_cfg.drop.wred.min_th1 = q_info->drop.wred.min_th1;
						mod_q_cfg.drop.wred.max_th1 = q_info->drop.wred.max_th1;
						mod_q_cfg.drop.wred.max_p1 = q_info->drop.wred.max_p1;
					} else {
						mod_q_cfg.drop.mode = QOS_MGR_DROP_TAIL;
					}

						mod_q_cfg.flags = q_info->flags;
						mod_q_cfg.queue_id = p_q_item->p_entry;

						ret = qos_mgr_modify_qos_queue(q_info->ifname, &mod_q_cfg, q_info->flags, QOS_HAL);
						if (ret != QOS_MGR_SUCCESS)
							qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "qos_mgr_modify_qos_queue failed \n");

						if (ret == QOS_MGR_SUCCESS) {
							tmp = &mod_q_cfg.queue_id;
							p_q_item->p_entry = *tmp;
						}
	/* returned success in step 1 proceed */
	return ret;

UPDATE_FAILED:
	return QOS_MGR_FAILURE;

}

int32_t qosal_delete_qos_queue(QOS_MGR_CMD_QUEUE_INFO *q_info, QOS_QUEUE_LIST_ITEM *p_item)
{
	int32_t ret = QOS_MGR_SUCCESS;
	if (qos_mgr_set_ingress_qos_generic(q_info->ifname, &q_info->flags))
		return QOS_MGR_FAILURE;

	ret = qos_mgr_delete_qos_queue(q_info->ifname, p_item->dev_name, q_info->priority, &(p_item->p_entry),
			q_info->portid, QOS_HAL, (q_info->flags|p_item->flags));

	if (ret != QOS_MGR_SUCCESS)
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "qos_mgr_delete_qos_queue failed \n");
	return ret;
}

#if IS_ENABLED(WMM_QOS_CONFIG)
#endif/*WMM_QOS_CONFIG*/
/*
############# QUEUE FUNCTION DEFINITIONS : END################
*/

/*
############# VAPQOS FUNCTION DEFINITIONS : START################
*/
/*
############# VAPQOS FUNCTION DEFINITIONS : END################
*/


/*
############# QOS GET FUNCTION DEFINITIONS : START################
*/


/*
############# QOS GET FUNCTION DEFINITIONS : END################
*/

/*
############# LEGACY QOS FUNCTION DEFINITIONS : START################
*/

/*
############# LEGACY QOS FUNCTION DEFINITIONS : END################
*/

/*
############# QOS RATE LIMIT FUNCTION DEFINITIONS : START################
*/

int32_t qos_mgr_set_ctrl_qos_rate(uint32_t portid,	uint32_t f_enable, uint32_t flag)
{
	uint32_t i;
	QOS_MGR_COUNT_CFG count={0};
	QOS_MGR_ENABLE_CFG enable_cfg={0};
	QOS_MGR_RATE_SHAPING_CFG rate={0};

	count.portid = portid;
	count.flags = flag;
	//qos_mgr_drv_get_qos_qnum( &count, 0);
	qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_GET_QOS_QUEUE_NUM, &count, 0);

	if( count.num <= 0 )
		return QOS_MGR_FAILURE;

	enable_cfg.portid = portid;
	enable_cfg.flag = flag;
	enable_cfg.f_enable = f_enable;
	//qos_mgr_drv_set_ctrl_qos_rate( &enable_cfg, 0);
	qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_SET_QOS_RATE_SHAPING_CTRL, &enable_cfg, 0);

	for( i=0; i<count.num; i++ ) {
		rate.flag = 0;
		rate.portid = portid;
		rate.queueid	= i;
		//qos_mgr_drv_reset_qos_rate( &rate, 0);
		qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_RESET_QOS_RATE_SHAPING_CFG, &rate, 0);
	}

	return QOS_MGR_SUCCESS;
}
EXPORT_SYMBOL(qos_mgr_set_ctrl_qos_rate);

int32_t qos_mgr_get_ctrl_qos_rate(uint32_t portid,	uint32_t *f_enable, uint32_t flag)
{
	QOS_MGR_ENABLE_CFG enable_cfg={0};
	int32_t res;

	enable_cfg.portid = portid;
	enable_cfg.flag = flag;

	//res= qos_mgr_drv_get_ctrl_qos_rate( &enable_cfg, 0);
	res = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_GET_QOS_RATE_SHAPING_CTRL, &enable_cfg, 0);

	if( *f_enable ) *f_enable = enable_cfg.f_enable;
	return res;
}
EXPORT_SYMBOL(qos_mgr_get_ctrl_qos_rate);

int32_t qos_mgr_set_qos_rate( char *ifname, uint32_t portid, uint32_t queueid, int32_t shaperid, uint32_t rate, uint32_t burst, uint32_t flag, int32_t hal_id )
{

	int32_t ret=QOS_MGR_SUCCESS;

	char dev_name[QOS_MGR_IF_NAME_SIZE];

	QOS_RATE_SHAPING_CFG tmu_rate_cfg;
	memset(&tmu_rate_cfg,0x00,sizeof(QOS_RATE_SHAPING_CFG));
	if(qos_mgr_get_pppoa_base_if(ifname,dev_name) == QOS_MGR_SUCCESS) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT," %s:%d Pseudo PPPOA interface = %s\n",__FUNCTION__,__LINE__,dev_name);
		tmu_rate_cfg.dev_name = dev_name;
		snprintf(tmu_rate_cfg.ifname, sizeof(tmu_rate_cfg.ifname), "%s", ifname);
		tmu_rate_cfg.flag |= QOS_MGR_F_PPPOATM;
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
	if ( (ret = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_SET_QOS_RATE_SHAPING_CFG, &tmu_rate_cfg, 0)) == QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}
EXPORT_SYMBOL(qos_mgr_set_qos_rate);

int32_t qos_mgr_get_qos_rate( uint32_t portid, uint32_t queueid, int32_t *shaperid, uint32_t *rate, uint32_t *burst, uint32_t flag)
{
	QOS_MGR_RATE_SHAPING_CFG rate_cfg={0};
	int32_t res = QOS_MGR_FAILURE;

	rate_cfg.portid = portid;
	rate_cfg.flag = flag;
	rate_cfg.queueid = queueid;
	rate_cfg.shaperid = -1;
	//res = qos_mgr_drv_get_qos_rate( &rate_cfg, 0);
	res = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_GET_QOS_RATE_SHAPING_CFG, &rate_cfg, 0);

	if( rate ) *rate = rate_cfg.rate_in_kbps;
	if( burst )	*burst = rate_cfg.burst;
	if( shaperid ) *shaperid = rate_cfg.shaperid;
	return res;

}
EXPORT_SYMBOL(qos_mgr_get_qos_rate);

int32_t qos_mgr_reset_qos_rate(char *ifname, char *dev_name, uint32_t portid, int32_t queueid, int32_t shaperid, uint32_t flag, int32_t hal_id)
{
	int32_t ret = QOS_MGR_FAILURE;
	QOS_RATE_SHAPING_CFG tmu_rate_cfg;
	memset(&tmu_rate_cfg,0x00,sizeof(QOS_RATE_SHAPING_CFG));

	qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT," %s:%d Pseudo PPPOA interface = %s\n",__FUNCTION__,__LINE__,dev_name);
	tmu_rate_cfg.dev_name = dev_name;
	qos_mgr_strncpy(tmu_rate_cfg.ifname,ifname, QOS_MGR_IF_NAME_SIZE);
	tmu_rate_cfg.portid = portid;
	tmu_rate_cfg.queueid = queueid;
	tmu_rate_cfg.shaperid = shaperid;
	tmu_rate_cfg.flag |= flag;

	if ( (ret = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_RESET_QOS_RATE_SHAPING_CFG, &tmu_rate_cfg, 0)) == QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}
EXPORT_SYMBOL(qos_mgr_reset_qos_rate);


int32_t qos_mgr_ioctl_set_ctrl_qos_rate(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int32_t res;

	qos_mgr_memset(&cmd_info->qos_ctrl_info, 0, sizeof(cmd_info->qos_ctrl_info) );

	if ( qos_mgr_copy_from_user( &cmd_info->qos_ctrl_info, (void *)arg, sizeof(cmd_info->qos_ctrl_info)) != 0 )
		return QOS_MGR_FAILURE;

	if (cmd_info->qos_ctrl_info.portid > QOS_HAL_MAX_EGRESS_PORT) {
		pr_err("Ivalid portid.\n");
		return QOS_MGR_FAILURE;
	}

	res = qos_mgr_set_ctrl_qos_rate(cmd_info->qos_ctrl_info.portid, cmd_info->qos_ctrl_info.enable, cmd_info->qos_ctrl_info.flags);
	if ( res != QOS_MGR_SUCCESS )
	{
		qos_mgr_debug(DBG_ENABLE_MASK_QOS, "qos_mgr_ioctl_set_ctrl_qos_rate fail\n");
		res = QOS_MGR_FAILURE;
	}

	return res;
}
EXPORT_SYMBOL(qos_mgr_ioctl_set_ctrl_qos_rate);

int32_t qos_mgr_ioctl_get_ctrl_qos_rate(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = QOS_MGR_FAILURE;

	qos_mgr_memset(&cmd_info->qos_ctrl_info, 0, sizeof(cmd_info->qos_ctrl_info) );

	if ( qos_mgr_copy_from_user( &cmd_info->qos_ctrl_info, (void *)arg, sizeof(cmd_info->qos_ctrl_info)) != 0 )
		return QOS_MGR_FAILURE;

	if (cmd_info->qos_ctrl_info.portid > QOS_HAL_MAX_EGRESS_PORT) {
		pr_err("Ivalid portid.\n");
		return QOS_MGR_FAILURE;
	}

	res = qos_mgr_get_ctrl_qos_rate(cmd_info->qos_ctrl_info.portid, &cmd_info->qos_ctrl_info.enable, cmd_info->qos_ctrl_info.flags);
	if ( res != QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_QOS, "qos_mgr_ioctl_get_ctrl_qos_rate fail\n");
		res = QOS_MGR_FAILURE;
	}

	if ( qos_mgr_copy_to_user( (void *)arg, &cmd_info->qos_ctrl_info, sizeof(cmd_info->qos_ctrl_info)) != 0 )
		return QOS_MGR_FAILURE;

	return res;
}
EXPORT_SYMBOL(qos_mgr_ioctl_get_ctrl_qos_rate);

int32_t qosal_reset_qos_rate(QOS_MGR_CMD_RATE_INFO *qos_rate_info, QOS_QUEUE_LIST_ITEM *p_item, QOS_SHAPER_LIST_ITEM *p_s_item)
{
	uint32_t ret=QOS_MGR_SUCCESS;

	if((qos_rate_info->shaperid == -1) && (qos_rate_info->queueid == -1)) {
		ret = qos_mgr_reset_qos_rate(qos_rate_info->ifname,p_s_item->dev_name,
		qos_rate_info->portid, -1 , p_s_item->p_entry,
		qos_rate_info->flags, QOS_HAL);
	}
	else {
		ret = qos_mgr_reset_qos_rate(qos_rate_info->ifname,p_s_item->dev_name,qos_rate_info->portid,
		p_item->p_entry, p_s_item->p_entry, qos_rate_info->flags, QOS_HAL);
	}

	if(ret != QOS_MGR_SUCCESS)
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"qos_mgr_set_qos_rate failed \n" );

	return ret;
}

int32_t qosal_set_qos_rate(QOS_MGR_CMD_RATE_INFO *qos_rate_info, QOS_QUEUE_LIST_ITEM *p_item, QOS_SHAPER_LIST_ITEM *p_s_item)
{
	uint32_t ret = QOS_MGR_SUCCESS;


	/* when init, these entry values are ~0, the max the number which can be detected by these functions*/
	if((qos_rate_info->shaperid == -1) && (qos_rate_info->queueid == -1)) {
#if IS_ENABLED(CONFIG_X86_INTEL_LGM) || IS_ENABLED(CONFIG_PRX300_HGU)
		pr_debug("---> p_s_item->p_entry:[%d] flags:[%x]\n",
			p_s_item->p_entry,
			qos_rate_info->flags);
		if (qos_rate_info->flags & QOS_MGR_OP_F_MODIFY) {
			QOS_MGR_ADD_SHAPER_CFG shaper_cfg;
			memset(&shaper_cfg,0x00,sizeof(QOS_MGR_ADD_SHAPER_CFG));

			shaper_cfg.enable = qos_rate_info->shaper.enable;
			shaper_cfg.mode = qos_rate_info->shaper.mode;
			shaper_cfg.pir = qos_rate_info->shaper.pir;
			shaper_cfg.pbs = qos_rate_info->shaper.pbs;
			shaper_cfg.cir = qos_rate_info->shaper.cir;
			shaper_cfg.cbs = qos_rate_info->shaper.cbs;
			shaper_cfg.flags = qos_rate_info->shaper.flags;
			qos_mgr_set_qos_shaper(p_s_item->p_entry,
				qos_rate_info->rate,
				qos_rate_info->burst,
				&shaper_cfg,
				qos_rate_info->flags,
				QOS_HAL);
			}
#endif
		ret = qos_mgr_set_qos_rate(qos_rate_info->ifname,qos_rate_info->portid, -1 ,
		p_s_item->p_entry, qos_rate_info->rate,qos_rate_info->burst,
		qos_rate_info->flags, QOS_HAL);
	}
	else {
		ret = qos_mgr_set_qos_rate(qos_rate_info->ifname,qos_rate_info->portid,
		p_item->p_entry, p_s_item->p_entry, qos_rate_info->rate,qos_rate_info->burst,
		qos_rate_info->flags, QOS_HAL);
	}

	if(ret != QOS_MGR_SUCCESS)
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"qos_mgr_set_qos_rate failed \n" );

	return ret;

}
EXPORT_SYMBOL(qosal_set_qos_rate);

int32_t qos_mgr_ioctl_set_qos_rate(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int32_t res;


	uint32_t phy_qid,phy_shaperid;
		char dev_name[QOS_MGR_IF_NAME_SIZE];
	QOS_QUEUE_LIST_ITEM *p_item = NULL;
	QOS_SHAPER_LIST_ITEM *p_s_item;

	qos_mgr_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

	if ( qos_mgr_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
		return QOS_MGR_FAILURE;

	phy_qid = cmd_info->qos_rate_info.queueid;
	phy_shaperid = cmd_info->qos_rate_info.shaperid;

	res = qos_mgr_shaper_lookup(cmd_info->qos_rate_info.shaperid,cmd_info->qos_rate_info.ifname,&p_s_item);
	if( res == QOS_MGR_ENOTAVAIL ) {
		qos_mgr_debug(DBG_ENABLE_MASK_QOS, " qos_mgr_ioctl_set_qos_rate: QOS_MGR_SHAPER_NOT_FOUND	\n");
		return QOS_MGR_FAILURE;
	}
	else
		phy_shaperid = p_s_item->p_entry;

	if(phy_qid != -1) {
		res = qos_mgr_queue_lookup(cmd_info->qos_rate_info.queueid,cmd_info->qos_queue_info.ifname,&p_item);
		if( res == QOS_MGR_ENOTAVAIL ) {
			qos_mgr_debug(DBG_ENABLE_MASK_QOS, " qos_mgr_ioctl_set_qos_rate: QOS_MGR_QUEUE_NOT_FOUND	\n");
			return QOS_MGR_FAILURE;
		} else
			phy_qid = p_item->p_entry;
	}
	if(qos_mgr_get_pppoa_base_if(cmd_info->qos_rate_info.ifname,dev_name) == QOS_MGR_SUCCESS) {
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT," %s:%d Pseudo PPPOA interface = %s\n",__FUNCTION__,__LINE__,dev_name);
		qos_mgr_strncpy(p_s_item->dev_name,dev_name,QOS_MGR_IF_NAME_SIZE);
		p_s_item->flags |= QOS_MGR_F_PPPOATM;
	}
	if (qos_mgr_set_ingress_qos_generic(cmd_info->qos_rate_info.ifname, &cmd_info->qos_rate_info.flags))
		return QOS_MGR_FAILURE;

	if ( p_item != NULL )
		res = qosal_set_qos_rate(&cmd_info->qos_rate_info,p_item,p_s_item);
	else
		res = qosal_set_qos_rate(&cmd_info->qos_rate_info,NULL,p_s_item);

	if ( res != QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_QOS, "qos_mgr_ioctl_set_qos_rate fail\n");
		res = QOS_MGR_FAILURE;
	}

	return res;
}
EXPORT_SYMBOL(qos_mgr_ioctl_set_qos_rate);

int32_t qos_mgr_ioctl_reset_qos_rate(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = QOS_MGR_FAILURE;


	uint32_t phy_qid,phy_shaperid;
	QOS_QUEUE_LIST_ITEM *p_item = NULL;
	QOS_SHAPER_LIST_ITEM *p_s_item = NULL;

	qos_mgr_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

	if ( qos_mgr_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
		return QOS_MGR_FAILURE;

	phy_qid = cmd_info->qos_rate_info.queueid;
	phy_shaperid = cmd_info->qos_rate_info.shaperid;

	res = qos_mgr_shaper_lookup(cmd_info->qos_rate_info.shaperid,cmd_info->qos_rate_info.ifname,&p_s_item);
	if( res == QOS_MGR_ENOTAVAIL || p_s_item == NULL ) {
		qos_mgr_debug(DBG_ENABLE_MASK_QOS, " qos_mgr_ioctl_reset_qos_rate: QOS_MGR_SHAPER_NOT_FOUND	\n");
		return QOS_MGR_FAILURE;
	}
	else
		phy_shaperid = p_s_item->p_entry;

	if(phy_qid != -1) {
		res = qos_mgr_queue_lookup(cmd_info->qos_rate_info.queueid,cmd_info->qos_queue_info.ifname,&p_item);
		if( res == QOS_MGR_ENOTAVAIL || p_item == NULL) {
			qos_mgr_debug(DBG_ENABLE_MASK_QOS, " qos_mgr_ioctl_set_qos_rate: QOS_MGR_QUEUE_NOT_FOUND	\n");
			return QOS_MGR_FAILURE;
		}
		else
			phy_qid = p_item->p_entry;
	}

	if (qos_mgr_set_ingress_qos_generic(cmd_info->qos_rate_info.ifname, &cmd_info->qos_rate_info.flags))
		return QOS_MGR_FAILURE;

	if ( p_item != NULL ) {
		res = qosal_reset_qos_rate(&cmd_info->qos_rate_info,p_item,p_s_item);
	} else {
		res = qosal_reset_qos_rate(&cmd_info->qos_rate_info,NULL,p_s_item);
	}

	if ( res != QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_QOS, "qos_mgr_ioctl_set_qos_rate fail\n");
		res = QOS_MGR_FAILURE;
	}

	return res;
}
EXPORT_SYMBOL(qos_mgr_ioctl_reset_qos_rate);

int32_t qos_mgr_ioctl_get_qos_rate(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = QOS_MGR_FAILURE;

	qos_mgr_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

	if ( qos_mgr_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
		return QOS_MGR_FAILURE;

	if (cmd_info->qos_rate_info.portid > QOS_HAL_MAX_EGRESS_PORT) {
		pr_err("Ivalid portid.\n");
		return QOS_MGR_FAILURE;
	}

	res = qos_mgr_get_qos_rate(cmd_info->qos_rate_info.portid, cmd_info->qos_rate_info.queueid, 
			&cmd_info->qos_rate_info.shaperid, &cmd_info->qos_rate_info.rate,
			&cmd_info->qos_rate_info.burst, cmd_info->qos_rate_info.flags);
	if ( res != QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_QOS, "qos_mgr_ioctl_get_qos_rate fail\n");
		res = QOS_MGR_FAILURE;
	}

	if ( qos_mgr_copy_to_user( (void *)arg, &cmd_info->qos_rate_info, sizeof(cmd_info->qos_rate_info)) != 0 )
		return QOS_MGR_FAILURE;

	return res;
}
EXPORT_SYMBOL(qos_mgr_ioctl_get_qos_rate);

int32_t qos_mgr_set_qos_shaper( int32_t shaperid, uint32_t rate, uint32_t burst, QOS_MGR_ADD_SHAPER_CFG *s, uint32_t flags, int32_t hal_id )
{
	uint32_t ret=QOS_MGR_SUCCESS;
	QOS_RATE_SHAPING_CFG tmu_shape_cfg;

	memset(&tmu_shape_cfg,0x00,sizeof(QOS_RATE_SHAPING_CFG));
#if IS_ENABLED(CONFIG_X86_INTEL_LGM) || IS_ENABLED(CONFIG_PRX300_HGU)
	tmu_shape_cfg.shaperid = shaperid;
#endif
	tmu_shape_cfg.shaper.mode = s->mode;
	tmu_shape_cfg.shaper.enable = s->enable;
	tmu_shape_cfg.shaper.pir = s->pir;
	tmu_shape_cfg.shaper.pbs = s->pbs;
	tmu_shape_cfg.shaper.cir = s->cir;
	tmu_shape_cfg.shaper.cbs = s->cbs;
	tmu_shape_cfg.shaper.flags = s->flags;
	if ( (ret = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_SET_QOS_SHAPER_CFG, &tmu_shape_cfg, 0)) == QOS_MGR_SUCCESS ) {
		s->phys_shaperid = tmu_shape_cfg.phys_shaperid;
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add success!!!\n", __FILE__,__FUNCTION__,__LINE__);
	} else
		qos_mgr_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add failed!!!\n", __FILE__,__FUNCTION__,__LINE__);

	return ret;
}

int32_t qos_mgr_get_qos_shaper( int32_t shaperid, uint32_t *rate, uint32_t *burst, uint32_t flag)
{
	QOS_MGR_RATE_SHAPING_CFG rate_cfg={0};
	int32_t res = QOS_MGR_FAILURE;

	rate_cfg.flag = flag;
	rate_cfg.shaperid = shaperid;

	res = qos_hal_generic_hook(QOS_MGR_GENERIC_HAL_GET_QOS_RATE_SHAPING_CFG, &rate_cfg, 0);


	if( rate ) *rate = rate_cfg.rate_in_kbps;
	if( burst )	*burst = rate_cfg.burst;

	return res;

}

int32_t qos_mgr_ioctl_set_qos_shaper(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/

	QOS_SHAPER_LIST_ITEM *p_item;
	qos_mgr_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

	if ( qos_mgr_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
		return QOS_MGR_FAILURE;

	if (cmd_info->qos_rate_info.shaperid > QOS_HAL_MAX_SHAPER) {
		pr_err("Ivalid shaperid.\n");
		return QOS_MGR_FAILURE;
	}

	if( qosal_add_shaper(&cmd_info->qos_rate_info,&p_item)!= QOS_MGR_SUCCESS) {
		qos_mgr_debug(DBG_ENABLE_MASK_QOS, "qosal_add_shaper returned failure\n");
		return QOS_MGR_FAILURE;
	}
	return QOS_MGR_SUCCESS;
}
EXPORT_SYMBOL(qos_mgr_ioctl_set_qos_shaper);

int32_t qos_mgr_ioctl_get_qos_shaper(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer	*/
	int res = QOS_MGR_FAILURE;

	qos_mgr_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

	if ( qos_mgr_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
		return QOS_MGR_FAILURE;

	if (cmd_info->qos_rate_info.shaperid > QOS_HAL_MAX_SHAPER) {
		pr_err("Ivalid shaperid.\n");
		return QOS_MGR_FAILURE;
	}

	res = qos_mgr_get_qos_shaper(cmd_info->qos_rate_info.shaperid, &cmd_info->qos_rate_info.rate,
			&cmd_info->qos_rate_info.burst, cmd_info->qos_rate_info.flags);
	if ( res != QOS_MGR_SUCCESS ) {
		qos_mgr_debug(DBG_ENABLE_MASK_QOS, "qos_mgr_ioctl_get_qos_shaper fail\n");
		res = QOS_MGR_FAILURE;
	}

	if ( qos_mgr_copy_to_user( (void *)arg, &cmd_info->qos_rate_info, sizeof(cmd_info->qos_rate_info)) != 0 )
		return QOS_MGR_FAILURE;

	return res;
}
EXPORT_SYMBOL(qos_mgr_ioctl_get_qos_shaper);


/*
############# QOS RATE LIMIT FUNCTION DEFINITIONS : END################
*/

int32_t qos_mgr_api_create(void)
{
	if ( qos_mgr_lock_init(&g_qos_queue_lock) ) {
		qos_mgr_debug(DBG_ENABLE_MASK_ERR,"Failed in creating lock for qos queue list.\n");
		goto QOS_MGR_CREATE_FAIL;
	}
	if ( qos_mgr_lock_init(&g_qos_shaper_lock) ) {
		qos_mgr_debug(DBG_ENABLE_MASK_ERR,"Failed in creating lock for qos shaper list.\n");
		goto QOS_MGR_CREATE_FAIL;
	}
	if ( qos_mgr_lock_init(&g_qos_inggrp_list_lock) ) {
		qos_mgr_debug(DBG_ENABLE_MASK_ERR,"Failed in creating lock for qos ingress list.\n");
		goto QOS_MGR_CREATE_FAIL;
	}

#if IS_ENABLED(CONFIG_QOS_MGR_TC_SUPPORT)
	qos_mgr_tc_init();
#endif
#if DP_EVENT_CB
	qos_manager_register_event_dp();
#endif
	return QOS_MGR_SUCCESS;
QOS_MGR_CREATE_FAIL:
	qos_mgr_api_destroy();
	return QOS_MGR_EIO;
}

void qos_mgr_api_destroy(void)
{
#if IS_ENABLED(CONFIG_QOS_MGR_TC_SUPPORT)
	qos_mgr_tc_exit();
#endif
	qos_mgr_queue_free_list();
	qos_mgr_shaper_free_list();
	qos_mgr_inggrp_free_list();
	qos_mgr_lock_destroy(&g_qos_queue_lock);
	qos_mgr_lock_destroy(&g_qos_shaper_lock);
	qos_mgr_lock_destroy(&g_qos_inggrp_list_lock);
#if DP_EVENT_CB
	qos_manager_deregister_event_dp();
#endif
}

EXPORT_SYMBOL(qos_mgr_ioctl_add_qos_queue);
EXPORT_SYMBOL(qos_mgr_ioctl_modify_qos_queue);
EXPORT_SYMBOL(qos_mgr_ioctl_delete_qos_queue);
EXPORT_SYMBOL(qos_mgr_ioctl_qos_init_cfg);
