/******************************************************************************
**
** FILE NAME	: lgm_lro_hal.c
** PROJECT	: LGM
** MODULES	: PPA LRO HAL
**
** DATE		: 18 July 2019
** AUTHOR	: Kamal Eradath
** DESCRIPTION	: PPv4 hardware abstraction layer
** COPYRIGHT	:	Copyright (c) 2014
**			Intel Corporation.
**
**	 For licensing information, see the file 'LICENSE' in the root folder of
**	 this software module.
**
** HISTORY
** $Date		$Author		 	$Comment
** 18 July 2018		Kamal Eradath		Initial Version
*******************************************************************************/
/*
 * ####################################
 *		Head File
 * ####################################
 */
/*
 *	Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>

#include <net/datapath_api.h>
#include <net/intel_np_lro.h>
#include <net/datapath_api_qos.h>
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_hal_api.h>
#include "../../ppa_api/ppa_api_session.h"
#include "lgm_pp_hal.h"

/*LRO table */
static ppa_lro_entry g_lro_table[MAX_LRO_ENTRIES];
static struct lro_ops *g_lro_ops;	/*lro_ops returned by the lro driver*/
static uint16_t g_lro_qid;
static int is_lro_init=0;
static spinlock_t port_lock;

void init_lro_table(void)
{
	struct dp_spl_cfg conn={0};
	struct dp_qos_q_logic q_logic = {0};

	/*Clear the lro table */
	ppa_memset(g_lro_table, 0, sizeof(ppa_lro_entry) * MAX_LRO_ENTRIES);

	/*Get spl conn parameteres 0 means failure*/
	if (dp_spl_conn_get(0, DP_SPL_TOE, &conn, 1) != 0) {
		/*Egress port qid*/;
		q_logic.q_id = conn.egp[0].qid;

		/* physical to logical qid */
		if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
			printk("%s:%d ERROR Failed to Logical Queue Id\n", __func__, __LINE__);
			return;
		}

		/*Store the logical qid */
		g_lro_qid = q_logic.q_logic_id;
		printk(KERN_INFO"%s %d q_logic.q_id=%d g_lro_qid =%d\n", __FUNCTION__, __LINE__, q_logic.q_id, g_lro_qid);
		is_lro_init = 1;

	} else {
		is_lro_init = 0;
		printk(KERN_INFO"LRO init failed\n");
	}
}

/*!
	\fn int32_t add_lro_entry( )
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief add a LRO HW session
	\param PPA_LRO_INFO * lro_entry
	\return >= 0: lro session id, otherwise: fail
 */
int32_t add_lro_entry(PPA_LRO_INFO *lro_entry)
{
	int32_t i = 0, ret = PPA_SUCCESS;

	if (!is_lro_init) {
		init_lro_table();
		if (!is_lro_init) {
			return PPA_FAILURE;
		}
	}

	spin_lock_bh(&port_lock);
	g_lro_ops = (struct lro_ops *)dp_get_ops(0, DP_OPS_LRO);
	if (g_lro_ops)
		i = g_lro_ops->find_free_lro_port(g_lro_ops->toe);
	spin_unlock_bh(&port_lock);

	if (i == -1) {
		pr_err("[%s] Error: No LRO ports available!!\n",__FUNCTION__);
		return PPA_FAILURE;
	}

	printk("=> add_lro_entry port allocated port i = %d\n", i);
	if (i < MAX_LRO_ENTRIES) {
		/* flow empty entry is i*/
		ppa_memset(&(g_lro_table[i]), 0, sizeof(ppa_lro_entry));

		/*Get the lro ops*/

		/*call the lro driver*/
		if (g_lro_ops) {
			if((ret = g_lro_ops->lro_start(g_lro_ops->toe, i ,lro_entry->lro_type)) < 0) {
				pr_err("LRO start flow returned failure %d port = %d\n",ret, i);
				return ret;
			} else {
				g_lro_table[i].enabled = 1;
				g_lro_table[i].session_id = i;
				lro_entry->session_id = g_lro_table[i].session_id;
				lro_entry->dst_q = g_lro_qid;
				printk(KERN_INFO"%s %d lro_entry->session_id = %d lro_entry->dst_q =%d \n",
					__FUNCTION__,__LINE__, lro_entry->session_id, lro_entry->dst_q);
			}
		}
	}

	printk("add_lro_entry succeeded...\n");
	return PPA_SUCCESS;
}

/*!
	\fn int32_t del_lro_entry( )
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief delete a LRO HW session
	\param uint8_t sessionid
	\return >= 0: success, otherwise: fail
 */

int32_t	del_lro_entry(uint8_t sessionid)
{
	uint32_t ret = PPA_SUCCESS;
	if(!is_lro_init)
		return PPA_FAILURE;

	printk("del_lro_entry requested for port %d\n", sessionid);

	if (sessionid >= MAX_LRO_ENTRIES) {
		printk("Invalid LRO rule index\n");
		return PPA_FAILURE;
	}

	if (!g_lro_table[sessionid].enabled) {
		printk("No LRO entry at the index %d\n", sessionid);
		return PPA_FAILURE;
	}

	spin_lock_bh(&port_lock);
	/*Get the lro ops*/
	g_lro_ops = (struct lro_ops *)dp_get_ops(0, DP_OPS_LRO);

	/*TBD: call the to delete the flow*/
	if(g_lro_ops) {
		g_lro_ops->lro_stop(g_lro_ops->toe, sessionid);
	}
	spin_unlock_bh(&port_lock);

	/* free the flow table */
	ppa_memset(&(g_lro_table[sessionid]), 0, sizeof(ppa_lro_entry));
	return ret;
}

MODULE_LICENSE("GPL");
