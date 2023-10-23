/*************************************************************************
 **
 ** FILE NAME    : qos_mgr_tc.c
 ** PROJECT      : QOS MGR
 ** MODULES      : QOS MGR (TC APIs)
 **
 ** DATE         : 18 APR 2018
 ** AUTHOR       : Purnendu Ghosh
 ** DESCRIPTION  : QOS MGR TC API Implementation
 ** COPYRIGHT : Copyright (c) 2017 - 2018 Intel Corporation
 ** Copyright (c) 2010 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 **
 *************************************************************************/
#include "qos_mgr_tc.h"
#include "qos_mgr_tc_lct.h"
#include <net/switch_api/lantiq_gsw_api.h>
#include <net/switch_api/lantiq_gsw_flow.h>

extern int g_ppa_proc_dir_flag;
extern struct proc_dir_entry *g_ppa_proc_dir ;
static struct proc_dir_entry *g_ppa_qos_mgr_proc_dir;
static int g_ppa_qos_mgr_proc_dir_flag;
static struct proc_dir_entry *g_ppa_qos_mgr_tc_proc_dir;
static int g_ppa_qos_mgr_tc_proc_dir_flag;
PPA_LOCK g_qos_mgr_list_lock;

#if IS_ENABLED(CONFIG_NET_SCH_DRR)
extern int32_t qos_mgr_tc_drr_offload(struct net_device *dev,
				u32 handle,
				struct tc_to_netdev *tc);
#endif
#if IS_ENABLED(CONFIG_NET_SCH_PRIO)
extern int32_t qos_mgr_tc_qdisc_prio_offload(struct net_device *dev,
                                u32 handle,
                                struct tc_to_netdev *tc);
#endif
/** Right now maximum of 16 CTP/bridge port are considered */
struct _tc_qos_mgr_db tc_qos_mgr_db[QOS_MGR_MAX_IFACE];

static int32_t qos_mgr_set_ext_vlan_to_dp(
				struct net_device *dev,
				uint32_t dir,
				struct qos_mgr_tc_flow_rule_list_item  *fl_list);

void qos_mgr_tc_lock_list(void)
{
	ppa_lock_get(&g_qos_mgr_list_lock);
}

void qos_mgr_tc_unlock_list(void)
{
	ppa_lock_release(&g_qos_mgr_list_lock);
}

struct qos_mgr_tc_flow_rule_list_item *tc_qos_mgr_alloc_list_item(void)
{
	struct qos_mgr_tc_flow_rule_list_item  *obj;

	obj = (struct qos_mgr_tc_flow_rule_list_item  *)ppa_malloc(sizeof(*obj));
	if (obj) {
		ppa_memset(obj, 0, sizeof(*obj));
		ppa_atomic_set(&obj->count, 1);
	}
	return obj;
}

/* Add 'obj' to the 'obj_head' list.
 * The 'obj_head' list is sorted by tc filter 'pref' (a.k.a. priority) value.
 * This function will maintain the entries in a list in a sorted order. */
static void
__qos_mgr_tc_add_list_item(struct qos_mgr_tc_flow_rule_list_item *obj,
			   struct qos_mgr_tc_flow_rule_list_item **obj_head)
{
	struct qos_mgr_tc_flow_rule_list_item **insertion_place = NULL;
	struct qos_mgr_tc_flow_rule_list_item *item = NULL;

	ppa_atomic_inc(&obj->count);

	/* We start looking for a place
	 * to insert from the beginning */
	insertion_place = obj_head;

	while (true) {
		item = *insertion_place;

		/* This is the end of the list
		 * we will insert at this place */
		if (item == NULL)
			break;

		/* Current item at this place has a higher priority value so
		 * we will insert at this place. This will maintain the
		 * list sorted by priorities. */
		if (item->priority > obj->priority)
			break;

		/* Check the next place */
		insertion_place = &item->next;
	}

	/* Place items with higher prio after the obj */
	obj->next = *insertion_place;

	/* Insert obj at the place, that we've just found */
	*insertion_place = obj;
}

void qos_mgr_tc_free_list_item(struct qos_mgr_tc_flow_rule_list_item *obj)
{
	if (ppa_atomic_dec(&obj->count) == 0)
		ppa_free(obj);
}

void tc_qos_mgr_remove_list_item(uint32_t prio,
		struct qos_mgr_tc_flow_rule_list_item **obj_head)
{
	struct qos_mgr_tc_flow_rule_list_item *p_prev, *p_cur;
	p_prev = NULL;

	qos_mgr_tc_lock_list();
	for (p_cur = *obj_head; p_cur; p_prev = p_cur, p_cur = p_cur->next)
		if ((p_cur->priority == prio)) {
			if (!p_prev)
				*obj_head = p_cur->next;
			else
				p_prev->next = p_cur->next;

			qos_mgr_tc_free_list_item(p_cur);
			break;
		}
	qos_mgr_tc_unlock_list();
}

void qos_mgr_tc_list_item_put(struct qos_mgr_tc_flow_rule_list_item *obj)
{
	if (obj)
		qos_mgr_tc_free_list_item(obj);
}

char *get_pattern_match_type(uint32_t pattern)
{

	switch (pattern) {
	case QOS_MGR_MATCH_UNKNOWN: return "Unknown";
	case QOS_MGR_MATCH_ETH_ADDRS: return "Ethernet MAC";
	case QOS_MGR_MATCH_PROTO: return "Protocol";
	case QOS_MGR_MATCH_VLAN: return "VLAN";
	}
	return "No Match";
}

char *get_action_type(int8_t act_type)
{
	switch (act_type) {
	case TC_ACT_OK: return "OK";
	case TC_ACT_SHOT: return "DROP";
	case TC_ACT_UNSPEC: return "CONTINUE";
	case TCA_ACT_VLAN: return "VLAN";
	case TCA_ACT_MIRRED: return "REDIRECT";
	default: return "UNKNOWN";
	}
}

char *get_dir(uint8_t dir)
{
	switch (dir) {
	case 0: return "EGRESS";
	case 1: return "INGRESS";
	default: return "INVALID";
	}
}
/**
	This function returns the matched class id of the given priority
	of the flow rule. It will return success if any valid mqprio
	handler is cretaed before.
*/
int32_t qos_mgr_tc_flow_find_parentid(
			int32_t prio,
			struct qos_mgr_tc_flow_rule_list_item  *fl_list,
			struct net_device **indev,
			uint32_t *classid)
{
	int32_t ret = PPA_ENOTAVAIL;
	struct qos_mgr_tc_flow_rule_list_item *p;

	for (p = fl_list; p; p = p->next) {
		if (p->priority == prio) {
			if (p->parentid) {
				if (classid)
					*classid = p->parentid;
				if (indev)
					*indev = p->indev;
				ret = PPA_SUCCESS;
			}
			break;
		}
	}
	return ret;
}

static int32_t qos_mgr_is_parent_handler_configured(int32_t dev_index)
{
	int32_t i = 0;

	for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
		if (tc_qos_mgr_db[dev_index].qos_info.q_info[i].p_handle == 0) {
			pr_debug("Handler is not present\n");
			return PPA_FAILURE;
		}
	}

	return PPA_SUCCESS;
}

static void qos_mgr_get_no_of_flow_rules(uint32_t index, uint32_t *ingress, uint32_t *egress)
{
	uint32_t ig_cnt = 0, eg_cnt = 0;
	struct qos_mgr_tc_flow_rule_list_item *p;
	pr_debug("<%s><%d> Device node index: [%d]\n", __func__, __LINE__, index);
	if (tc_qos_mgr_db[index].flow_item_ig != NULL) {
		for (p = tc_qos_mgr_db[index].flow_item_ig; p; p = p->next) {
			if (p != NULL)
				ig_cnt++;
		}
	}
	if (tc_qos_mgr_db[index].flow_item_eg != NULL) {
		for (p = tc_qos_mgr_db[index].flow_item_eg; p; p = p->next) {
			if (p != NULL)
				eg_cnt++;
		}
	}
	*ingress = ig_cnt;
	*egress = eg_cnt;
	pr_debug("<%s><%d> No Of Rules -> Ingress:[%d] Egress:[%d]\n", __func__, __LINE__, ig_cnt, eg_cnt);
}

int32_t __qos_mgr_tc_flow_lookup(struct qos_mgr_tc_flow_rule_list_item  *fl_list)
{
	int32_t ret = PPA_ENOTAVAIL;
	struct qos_mgr_tc_flow_rule_list_item *p;

	for (p = fl_list; p; p = p->next) {
		if (p != NULL) {
			/*ppa_atomic_inc(&p->count); */
			pr_info("=====================================================\n");
			pr_info("Direction: %s  Priority: %d\n",
					get_dir(p->dir), p->priority);
			pr_info("PATTERN Used Keys: %d\n",
					p->flow_pattern.used_keys);
			pr_info("Pattern: %s\n",
					get_pattern_match_type(p->flow_pattern.match_to));
			if (p->flow_pattern.match_to == QOS_MGR_MATCH_VLAN) {
				int32_t i = 0;
				pr_info("\t Number Of Tag:%d\n", p->flow_pattern.vlan_match.no_of_tag);
				for (i = 0; i < p->flow_pattern.vlan_match.no_of_tag ; i++) {
					pr_info("\t vlan_mask:%x\n",
						p->flow_pattern.vlan_match.entry[i].vlan_mask);
					pr_info("\t VID:[%x] Priority:[%d] Protocol:[%x] TPID:[%x] Dei:[%d]\n",
						p->flow_pattern.vlan_match.entry[i].vid,
						p->flow_pattern.vlan_match.entry[i].prio,
						p->flow_pattern.vlan_match.entry[i].proto,
						p->flow_pattern.vlan_match.entry[i].tpid,
						p->flow_pattern.vlan_match.entry[i].dei);
				}
			}
			if (p->flow_pattern.match_to == QOS_MGR_MATCH_ETH_ADDRS) {
				if (p->flow_pattern.eth_match.which_mac == (QOS_MGR_ETH_SRC_MAC | QOS_MGR_ETH_DST_MAC)) {
					pr_info("\t SRC MAC Addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
							p->flow_pattern.eth_match.src[0],
							p->flow_pattern.eth_match.src[1],
							p->flow_pattern.eth_match.src[2],
							p->flow_pattern.eth_match.src[3],
							p->flow_pattern.eth_match.src[4],
							p->flow_pattern.eth_match.src[5]);
					pr_info("\t DST MAC Addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
							p->flow_pattern.eth_match.dst[0],
							p->flow_pattern.eth_match.dst[1],
							p->flow_pattern.eth_match.dst[2],
							p->flow_pattern.eth_match.dst[3],
							p->flow_pattern.eth_match.dst[4],
							p->flow_pattern.eth_match.dst[5]);
				} else if (p->flow_pattern.eth_match.which_mac == QOS_MGR_ETH_SRC_MAC) {
					pr_info("\t SRC MAC Addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
							p->flow_pattern.eth_match.src[0],
							p->flow_pattern.eth_match.src[1],
							p->flow_pattern.eth_match.src[2],
							p->flow_pattern.eth_match.src[3],
							p->flow_pattern.eth_match.src[4],
							p->flow_pattern.eth_match.src[5]);
				} else if (p->flow_pattern.eth_match.which_mac == QOS_MGR_ETH_DST_MAC) {
					pr_info("\t DST MAC Addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
							p->flow_pattern.eth_match.dst[0],
							p->flow_pattern.eth_match.dst[1],
							p->flow_pattern.eth_match.dst[2],
							p->flow_pattern.eth_match.dst[3],
							p->flow_pattern.eth_match.dst[4],
							p->flow_pattern.eth_match.dst[5]);
				}

			}
			pr_info("ACTION Type: %s\n", get_action_type(p->flow_action.act_type));
			if (p->flow_action.act_type == TCA_ACT_VLAN) {
				if (p->flow_action.action.act == QOS_MGR_VLAN_ACT_POP) {
					pr_info("\t POP\n");
					pr_info("\t No Of POP: %d\n", p->flow_action.action.pop_n);
				} else if (p->flow_action.action.act == QOS_MGR_VLAN_ACT_PUSH) {
					pr_info("\t PUSH\n");
					pr_info("\t No Of PUSH: %d\n", p->flow_action.action.push_n);
					pr_info("\t VID: %02x Prio: %d Proto: %02x\n",
							p->flow_action.action.vid[p->flow_action.action.push_n - 1],
							p->flow_action.action.prio[p->flow_action.action.push_n - 1],
							p->flow_action.action.tpid[p->flow_action.action.push_n - 1]);
				} else if (p->flow_action.action.act == QOS_MGR_VLAN_ACT_MODIFY) {
					pr_info("\t MODIFY\n");
					pr_info("\t VID: %d Prio: %d Proto: %02x\n",
							p->flow_action.action.vid[0],
							p->flow_action.action.prio[0],
							p->flow_action.action.tpid[0]);
				}
			}
			if (p->flow_action.act_type == TC_ACT_SHOT) {
				pr_info("\t DROP\n");
			}
#if IS_ENABLED(CONFIG_NET_ACT_COLMARK)
			if (p->meter_id != -1)
				pr_info("Meter id: %d\n", p->meter_id);
#endif
			ret = PPA_SUCCESS;
		}
	}

	return ret;
}

int32_t qos_mgr_tc_flow_lookup(struct qos_mgr_tc_flow_rule_list_item  *fl_list)
{
	int32_t ret;

	qos_mgr_tc_lock_list();
	ret = __qos_mgr_tc_flow_lookup(fl_list);
	qos_mgr_tc_unlock_list();

	return ret;
}

static void tc_flower_store_eth_addr_match(
			struct flow_dissector_key_eth_addrs *key,
			struct flow_dissector_key_eth_addrs *mask,
			struct qos_mgr_tc_flow_rule_list_item *tc_fl_item)
{
	tc_fl_item->flow_pattern.match_to = QOS_MGR_MATCH_ETH_ADDRS;

	if (mask->dst[0] == 0) {
		tc_fl_item->flow_pattern.eth_match.which_mac = QOS_MGR_ETH_SRC_MAC;
		memcpy(tc_fl_item->flow_pattern.eth_match.src, key->src, ETH_ALEN);
	} else if (mask->src[0] == 0) {
		tc_fl_item->flow_pattern.eth_match.which_mac = QOS_MGR_ETH_DST_MAC;
		memcpy(tc_fl_item->flow_pattern.eth_match.dst, key->dst, ETH_ALEN);
	} else {
		tc_fl_item->flow_pattern.eth_match.which_mac = QOS_MGR_ETH_SRC_MAC | QOS_MGR_ETH_DST_MAC;
		memcpy(tc_fl_item->flow_pattern.eth_match.src, key->src, ETH_ALEN);
		memcpy(tc_fl_item->flow_pattern.eth_match.dst, key->dst, ETH_ALEN);
	}
}

static void tc_flower_store_vlan_match(
				uint32_t index,
				struct flow_dissector_key_vlan *key,
				struct flow_dissector_key_vlan *mask,
				struct qos_mgr_tc_flow_rule_list_item *tc_fl_item)
{
	tc_fl_item->flow_pattern.match_to = QOS_MGR_MATCH_VLAN;

	tc_fl_item->flow_pattern.vlan_match.no_of_tag = index + 1;

	if (mask != NULL)
		pr_debug("Match Mask vid: %#x pcp: %#x\n", mask->vlan_id, mask->vlan_priority);

	if ((mask != NULL) && (key != NULL)) {
		if (mask->vlan_id == 0xfff) {
			pr_debug("<%s><%d> VLAN ID Matched\n",  __func__, __LINE__);
			tc_fl_item->flow_pattern.vlan_match.entry[index].vlan_mask |= QOS_MGR_VLAN_ID;
			tc_fl_item->flow_pattern.vlan_match.entry[index].vid = key->vlan_id;
		}
		if (mask->vlan_priority == 0x7) {
			pr_debug("<%s><%d> VLAN PRIO Matched\n",  __func__, __LINE__);
			tc_fl_item->flow_pattern.vlan_match.entry[index].vlan_mask |= QOS_MGR_VLAN_PRIO;
			tc_fl_item->flow_pattern.vlan_match.entry[index].prio = key->vlan_priority;
		}
		pr_debug("<%s><%d> index = %d VLAN EthType Mask [%x]\n",  __func__, __LINE__, index, mask->vlan_tpid);
		pr_debug("<%s><%d> VLAN EthType [%x]\n",  __func__, __LINE__, key->vlan_tpid);
		tc_fl_item->flow_pattern.vlan_match.entry[index].tpid = key->vlan_tpid;
		tc_fl_item->flow_pattern.vlan_match.entry[index].vlan_mask |= QOS_MGR_VLAN_TPID;
	}
	/*#ifdef ANYVLAN
	  if (tc_fl_item->proto != ETH_P_ANYVLAN) {
	  pr_debug("<%s><%d> VLAN TPID Matched \n", __func__, __LINE__);
	  tc_fl_item->flow_pattern.vlan_match.entry[index].vlan_mask |= QOS_MGR_VLAN_TPID;
	  tc_fl_item->flow_pattern.vlan_match.entry[index].tpid = tc_fl_item->proto;
	  }
#endif*/
}

static void tc_flower_store_vlan_proto_match(
				uint32_t index,
				struct flow_dissector_key_basic *key,
				struct flow_dissector_key_basic *mask,
				struct qos_mgr_tc_flow_rule_list_item *tc_fl_item)
{
	/*tc_fl_item->flow_pattern.match_to = QOS_MGR_MATCH_VLAN;*/

	pr_debug("Match Mask Proto: %#x\n", mask->n_proto);
	if (mask->n_proto == 0xffff) {
		pr_debug("VLAN Proto Matched\n");
		tc_fl_item->flow_pattern.vlan_match.entry[index].vlan_mask |= QOS_MGR_VLAN_PROTO;
		tc_fl_item->flow_pattern.vlan_match.entry[index].proto = key->n_proto;
	}
}

#define IP_TOS_TO_DSCP 0xFC
static void tc_flower_store_ip_match(
			struct flow_dissector_key_ip *key,
			struct flow_dissector_key_ip *mask,
			struct qos_mgr_tc_flow_rule_list_item *tc_fl_item)
{
	if (key != NULL) {
		tc_fl_item->dscp.value = (key->tos & IP_TOS_TO_DSCP) >> 2;
		tc_fl_item->dscp.enable = true;
		pr_debug("DSCP value: %#x\n", tc_fl_item->dscp.value);
	}
	/* TODO: ip_tos mask handling */
}

#if 0
static void __qos_mgr_tc_fill_vlan_from_pending_flow(
			struct qos_mgr_tc_flow_rule_list_item  *cur_flow,
			struct qos_mgr_tc_flow_rule_list_item *pending_flow)
{
	if (pending_flow->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_ID) {
		cur_flow->flow_pattern.vlan_match.entry[1].vid = pending_flow->flow_pattern.vlan_match.entry[0].vid;
	}
	if (pending_flow->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_PRIO) {
		cur_flow->flow_pattern.vlan_match.entry[1].prio = pending_flow->flow_pattern.vlan_match.entry[0].prio;
	}
	if (pending_flow->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_PROTO) {
		cur_flow->flow_pattern.vlan_match.entry[1].proto = pending_flow->flow_pattern.vlan_match.entry[0].proto;
	}
	if (pending_flow->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_TPID) {
		cur_flow->flow_pattern.vlan_match.entry[1].tpid = pending_flow->flow_pattern.vlan_match.entry[0].tpid;
	}
	cur_flow->flow_pattern.vlan_match.no_of_tag++;

}
#endif

int32_t __qos_mgr_tc_find_pending_flow(
				struct qos_mgr_tc_flow_rule_list_item  *fl_list,
				struct qos_mgr_tc_flow_rule_list_item **fl_info)
{
	struct qos_mgr_tc_flow_rule_list_item *p;

	for (p = fl_list; p; p = p->next) {
		if (p->status == 1) {
			*fl_info = p;
			pr_debug("Match Found %p\n", p);
			break;
		}
	}
	return 0;

}

#if 0
static void qos_mgr_get_device_node_list(void)
{
	int32_t i = 0;

	for (i = 0; i < QOS_MGR_MAX_IFACE; i++) {
		if (tc_qos_mgr_db[i].dev != NULL) {
			pr_debug("Index: %d  Device Name: %s\n", i, tc_qos_mgr_db[i].dev->name);
		}
	}
}
#endif

static void qos_mgr_free_dev_node_index(uint32_t index)
{
	uint32_t ingress = 0, egress = 0;

	pr_debug("<%s><%d> Free Device node index: [%d]\n",
			__func__, __LINE__, index);
	qos_mgr_get_no_of_flow_rules(index, &ingress, &egress);

	pr_debug("<%s><%d> No of Queue: [%d]\n",
			__func__, __LINE__,
			tc_qos_mgr_db[index].qos_info.no_of_queue);
	if ((ingress == 0) &&
		(egress == 0) &&
		(tc_qos_mgr_db[index].qos_info.no_of_queue == 0) &&
		(qos_mgr_is_parent_handler_configured(index) == PPA_FAILURE)) {
		tc_qos_mgr_db[index].dev = NULL;
		}
}

static int32_t qos_mgr_get_free_dev_node_index(void)
{
	int32_t i = 0;

	for (i = 0; i < QOS_MGR_MAX_IFACE; i++) {
		if (tc_qos_mgr_db[i].dev == NULL) {
			pr_debug("Dev not in the DB. New DB index is %d\n", i);
			break;
		}
	}
	if ((i == (QOS_MGR_MAX_IFACE)) || (i > (QOS_MGR_MAX_IFACE-1))) {
		pr_err("DB is FULL\n");
		return PPA_FAILURE;
	}

	return i;
}

static int32_t qos_mgr_get_dev_node_match_index(struct net_device *dev)
{
	int32_t j = 0;

	for (j = 0; j < QOS_MGR_MAX_IFACE; j++) {
		if (tc_qos_mgr_db[j].dev == dev) {
			pr_debug("Matched Device Index is %d\n", j);
			break;
		}
	}
	if ((j == (QOS_MGR_MAX_IFACE)) || (j > (QOS_MGR_MAX_IFACE-1)))
		return PPA_FAILURE;

	return j;
}

int32_t qos_mgr_get_db_handle(struct net_device *dev,
				uint32_t oper,
				struct _tc_qos_mgr_db **handle)
{
	int32_t dev_index;
	dev_index = qos_mgr_get_dev_node_match_index(dev);
	if (dev_index == PPA_FAILURE) {
		if (oper == 0)
			return PPA_FAILURE;
		else {
			pr_debug("No match found !!!\n");
			dev_index = 0;
			dev_index = qos_mgr_get_free_dev_node_index();
			if (dev_index == PPA_FAILURE)
				return -1;
		}
	}
	pr_debug("<%s>DB index is %d\n", __func__, dev_index);
	tc_qos_mgr_db[dev_index].dev = dev;
	*handle = &tc_qos_mgr_db[dev_index];
	return 0;
}

void qos_mgr_free_db_handle(struct _tc_qos_mgr_db *db_handle)
{
	uint32_t ingress = 0, egress = 0, index = 0;
	
	for (index = 0; index < QOS_MGR_MAX_IFACE; index++) {
		if (tc_qos_mgr_db[index].dev == db_handle->dev) {
			pr_debug("Matched Device Index is %d\n", index);
			break;
		}
	}
	pr_debug("<%s><%d> Free Device node index: [%d]\n", __func__, __LINE__, index);
	qos_mgr_get_no_of_flow_rules(index, &ingress, &egress);

	pr_debug("<%s><%d> No of Queue: [%d]\n", __func__, __LINE__, tc_qos_mgr_db[index].qos_info.no_of_queue);
	if ((ingress == 0) && (egress == 0) && (tc_qos_mgr_db[index].qos_info.no_of_queue == 0))
		tc_qos_mgr_db[index].dev = NULL;
}

#if 0
static int32_t qos_mgr_get_dev_node_index(struct net_device *dev)
{
	int32_t i = 0, index = 0;

	for (i = 0; i < QOS_MGR_MAX_IFACE; i++) {
		if (tc_qos_mgr_db[i].dev == dev) {
			index = i;
			break;
		}
	}
	pr_debug("i= %d\n", i);
	if (i < QOS_MGR_MAX_IFACE)
		goto DEV_FOUND;

	for (i = 0; i < QOS_MGR_MAX_IFACE; i++) {
		if (tc_qos_mgr_db[i].dev == NULL) {
			pr_debug("Dev not found!!! New DB index is %d\n", i);
			break;
		}
	}
	pr_debug("i= %d\n", i);
	index = i;
	if ((i == (QOS_MGR_MAX_IFACE-1)) || (i > (QOS_MGR_MAX_IFACE-1))) {
		pr_err("DB is FULL\n");
		return PPA_FAILURE;
	}
DEV_FOUND:
	return index;

}
#endif

#if IS_ENABLED(CONFIG_NET_SCH_MQPRIO)
static int32_t qos_mgr_get_subif_info_from_indev(struct net_device *indev,
						int32_t *tc_index, int32_t flags)
{
	dp_subif_t dp_subif = {0};

	if (dp_get_netif_subifid(indev, NULL, NULL, 0, &dp_subif, flags) != PPA_SUCCESS) {
		pr_debug("dp_get_netif_subifid failed for %s\n", indev->name);
		return PPA_FAILURE;
	}
	if (indev->name != NULL)
		pr_debug("Device Name; %s\n", indev->name);
	pr_debug("Port Id; %d\n", dp_subif.port_id);
	pr_debug("subif_num; %d\n", dp_subif.subif_num);
	pr_debug("subif; %d\n", dp_subif.subif);
	pr_debug("subif_list; %d\n", dp_subif.subif_list[0]);

	*tc_index = dp_subif.subif;
	return 0;
}

static int32_t qos_mgr_delete_mqprio_handle(int32_t dev_index)
{
	int32_t i = 0;

	if (tc_qos_mgr_db[dev_index].qos_info.no_of_queue != 0) {
		pr_debug("All the queues are not yet deleted.\n");
		return PPA_FAILURE;
	}

	for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
		tc_qos_mgr_db[dev_index].qos_info.q_info[i].p_handle = 0;
		tc_qos_mgr_db[dev_index].qos_info.q_info[i].priority = 0;
	}
	memset(&tc_qos_mgr_db[dev_index].qos_info.mqprio, 0, sizeof(struct tc_mqprio_qopt));

	tc_qos_mgr_db[dev_index].qos_info.port_id = -1;
	tc_qos_mgr_db[dev_index].qos_info.deq_idx = 0;

	return 0;
}

static int32_t qos_mgr_find_and_store_mqprio_handle(int32_t dev_index,
						uint32_t handle,
						struct tc_mqprio_qopt *mqprio)
{
	int32_t i = 0;

	for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
		tc_qos_mgr_db[dev_index].qos_info.q_info[i].p_handle = (handle | (i + 1));
		tc_qos_mgr_db[dev_index].qos_info.q_info[i].priority = mqprio->offset[i];
	}
	tc_qos_mgr_db[dev_index].qos_info.sched_type = QOS_MGR_TC_MQPRIO;
	memcpy(&tc_qos_mgr_db[dev_index].qos_info.mqprio, mqprio, sizeof(struct tc_mqprio_qopt));

	return 0;
}

static int32_t qos_mgr_flow_get_valid_queue_handle(struct net_device *dev,
						   uint32_t parent_id)
{
	int32_t i = PPA_FAILURE, dev_index;

	dev_index = qos_mgr_get_dev_node_match_index(dev);
	if (dev_index == PPA_FAILURE)
		return PPA_FAILURE;

	pr_debug("Find classid: %x for device index %d\n", parent_id, dev_index);
	if (parent_id) {
		for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
			if (parent_id == tc_qos_mgr_db[dev_index].qos_info.q_info[i].p_handle) {
				pr_debug("Found Q handler for index %d\n", i);
				break;
			}
		}
	}
	if (i == QOS_MGR_MAX_QUEUE_IFACE) {
		pr_debug("<%s> Valid Queue handler not found.\n", __func__);
		return PPA_FAILURE;
	}
	return i;
}

static void qos_mgr_add_default_queue_settings(PPA_CMD_QOS_QUEUE_INFO *q_info)
{
	q_info->drop.mode = PPA_QOS_DROP_WRED;
	q_info->drop.wred.max_th0 = (500 * 1024); /* 500 Kbytes max green */
	q_info->drop.wred.min_th0 = (q_info->drop.wred.max_th0 * 90)/100;
	q_info->drop.wred.max_p0 = 100; /* slope green */
	q_info->drop.wred.max_th1 = (250 * 1024); /* 250 Kbytes max yellow*/
	q_info->drop.wred.min_th1 = (q_info->drop.wred.max_th1 * 90)/100;
	q_info->drop.wred.max_p1 = 100; /* slope yellow */
	pr_debug("Green: Min: [%d bytes] Max: [%d bytes] Prob: [%d]\n",
			q_info->drop.wred.min_th0,
			q_info->drop.wred.max_th0,
			q_info->drop.wred.max_p0);
	pr_debug("Yellow: Min: [%d bytes] Max: [%d bytes] Prob: [%d]\n",
			q_info->drop.wred.min_th1,
			q_info->drop.wred.max_th1,
			q_info->drop.wred.max_p1);
}

static int32_t
qos_mgr_del_queue(
		struct net_device *dev,
		struct net_device *in_dev,
		uint32_t parentid)
{
	int32_t ret = PPA_SUCCESS;
	int32_t dev_index, q_index;
	int32_t tcIndex, ctp_index;
	PPA_CMD_QOS_QUEUE_INFO q_info = {0};
	struct qos_mgr_tc_queue_info	*mqinfo = NULL;
	struct dp_tc_cfg tc_cfg = {0};

	dev_index = qos_mgr_get_dev_node_match_index(dev);
	if (dev_index == PPA_FAILURE)
		return PPA_FAILURE;

	q_index = qos_mgr_flow_get_valid_queue_handle(dev, parentid);
	if (q_index == PPA_FAILURE)
		return PPA_FAILURE;

	mqinfo = &tc_qos_mgr_db[dev_index].qos_info;

	strncpy(q_info.ifname, dev->name, strlen(dev->name));
	q_info.queue_num = mqinfo->q_info[q_index].queue_id;
	pr_debug("Delete Queue Id: [%d] for Port: [%d] CQM DEQ Idx: [%d]\n",
			q_info.queue_num, mqinfo->port_id, mqinfo->deq_idx);
	q_info.priority = mqinfo->mqprio.offset[q_index];
	q_info.enable = 1;
	q_info.portid = mqinfo->port_id;
	q_info.flowId = mqinfo->deq_idx;
	if (mqinfo->deq_idx != 0)
		q_info.flags = NETDEV_TYPE_TCONT;

	/* More than 1 GEM is mapped to this queue. Find the subif/class of the
	 * GEM. If deq_idx is more than 0 then the queue is for T-CONT in
	 * upstream direction. Call qos_mgr_set_mapped_queue() to update the
	 * queue map of the queue. It can be either default queue or drop
	 * queue.
	 */
	if (mqinfo->q_info[q_index].ref_cnt > 0) {
		pr_debug("Ref count is %d > 0\n",
				mqinfo->q_info[q_index].ref_cnt);
		if (in_dev != NULL) {
			ret = qos_mgr_get_subif_info_from_indev(in_dev, &ctp_index, 0);
			if (ret != PPA_SUCCESS) {
				pr_debug("%s: qos_mgr_get_subif_from_indev failed for %s\n",
					__func__, in_dev->name);
				return ret;
			}
			if (mqinfo->deq_idx != 0) {
				tcIndex = ctp_index;
				pr_debug("CTP index:---> %d\n", tcIndex);
			} else {
				tc_cfg.dev = in_dev;
				tc_cfg.tc = 0;
				tc_cfg.force = 0;
				pr_debug("Force TC: %d for device: %s\n",
					tc_cfg.tc, in_dev->name);
				if (dp_ingress_ctp_tc_map_set(&tc_cfg, 0) == DP_FAILURE) {
					pr_debug("ctp to tc map failed for device %s\n",
						in_dev->name);
					return PPA_FAILURE;
				}
				mqinfo->q_info[q_index].ref_cnt--;
				return PPA_SUCCESS;
			}
		}
		if (mqinfo->deq_idx != 0) {
			ret = qos_mgr_set_mapped_queue(
					dev,
					mqinfo->port_id,
					mqinfo->deq_idx,
					0, /* Set to Default Queue */
					0, /* 0 - Egress 1 - Ingress */
					tcIndex, /* Traffic class */
					q_info.flags);
		}

		if (ret == PPA_SUCCESS)
			mqinfo->q_info[q_index].ref_cnt--;
		return ret;
	}

	q_info.flowId_en = 1;
	if (qosal_delete_qos_queue(
			&q_info,
			mqinfo->q_info[q_index].p_item) != PPA_SUCCESS) {
		return PPA_FAILURE;
	}
	/* Update local database */
	mqinfo->q_info[q_index].p_item = NULL;
	mqinfo->q_info[q_index].queue_id = -1;
	mqinfo->no_of_queue--;
	return PPA_SUCCESS;
}
/** qos_mgr_add_queue is used to add the queue to the hardware.
  * dev: netdev used for the filter configuration e.g. with clsact qdisc attached
  * in_dev: netdev used for filtering the traffic pass usually in cls_flower for
  * ingress traffic classification.
  * parentid: queue handler for queue creation. This is usually passed as classid
  * in cls_flower
  * tc: traffic class
  *
  * In PON system this in_dev is used to specify the GEM port on which the traffic
  * is ingressing
*/
static int32_t
qos_mgr_add_queue(
		struct net_device *dev,
		struct net_device *in_dev,
		uint32_t parentid,
		int32_t tc)
{
	int32_t ret = PPA_FAILURE;
	int32_t dev_index, q_index;
	int32_t tcIndex, ctp_index;
	PPA_CMD_QOS_QUEUE_INFO q_info = {0};
	QOS_QUEUE_LIST_ITEM *p_item;
	struct qos_mgr_tc_queue_info	*mqinfo = NULL;
	struct dp_tc_cfg tc_cfg = {0};

	dev_index = qos_mgr_get_dev_node_match_index(dev);
	if (dev_index == PPA_FAILURE)
		return PPA_FAILURE;

	q_index = qos_mgr_flow_get_valid_queue_handle(dev, parentid);
	if (q_index == PPA_FAILURE)
		return PPA_FAILURE;

	pr_debug("dev_index: %d queue_index: %d\n", dev_index, q_index);
	mqinfo = &tc_qos_mgr_db[dev_index].qos_info;

	pr_debug("Priority:%d\n", mqinfo->q_info[q_index].priority);

	/* If the tc is not provided then by default tc will be
	 * priority of the queue.
	 */
	if (tc == -1)
		tcIndex = mqinfo->q_info[q_index].priority;
	else
		tcIndex = tc;

	/* For the upstream the queue will be created to a t-cont
	 * netdevice. The ctp index of the in_dev netdevice is considered
	 * as the tc.
	 * For the downstream UNI port we need to configure force ctp
	 * to tc.
	 */
	if (in_dev != NULL) {
		ret = qos_mgr_get_subif_info_from_indev(in_dev, &ctp_index, 0);
		if (ret != PPA_SUCCESS) {
			pr_debug("%s: qos_mgr_get_subif_from_indev failed for %s\n",
				__func__, in_dev->name);
			return ret;
		}

		/* tcont netdevice is not registered to DP. So dp_get_netif_subifid()
		 * will return an error. For tcont netdevice deq_idx is passed by
		 * pon ethernet datapath driver. deq_idx should be 0 for any other
		 * net device. So right now this is used to determine whether it
		 * is US or DS.
		 */
		if (mqinfo->deq_idx != 0) {
			tcIndex = ctp_index;
			pr_debug("CTP index:[%d]\n", tcIndex);
		} else {
		/* For Downstream Gem to Queue assignment traffic class 7
		 * should be configured to highest priority queue accroding 802.1p.
		 * So force the correct TC to this CTP port
		 */
			tc_cfg.dev = in_dev;
			tc_cfg.tc = (QOS_MGR_8021P_HIGHEST_PRIO - tcIndex);
			tc_cfg.force = 1;
			pr_debug("Force TC: %d for device: %s\n",
				tc_cfg.tc, in_dev->name);
			if (dp_ingress_ctp_tc_map_set(&tc_cfg, 0) == DP_FAILURE) {
				pr_debug("ctp to tc map failed for device %s\n",
						in_dev->name);
				return PPA_FAILURE;
			}
			if (mqinfo->q_info[q_index].queue_id != -1) {
				pr_debug("Already Q is created,no lookup update\n");
				mqinfo->q_info[q_index].ref_cnt++;
				return PPA_SUCCESS;
			}
		}
	}
	pr_debug("TC:%d\n", tcIndex);

	strncpy(q_info.ifname, dev->name, strlen(dev->name));
	q_info.priority = mqinfo->q_info[q_index].priority;
	q_info.enable = 1;
	q_info.tc_no = 1;
	q_info.tc_map[0] = tcIndex;
	q_info.portid = mqinfo->port_id;
	q_info.flowId = mqinfo->deq_idx;

	if (mqinfo->sched_type == QOS_MGR_TC_MQPRIO)
		q_info.sched = PPA_QOS_SCHED_SP;
#if IS_ENABLED(CONFIG_NET_SCH_PRIO)
	if (mqinfo->sched_type == QOS_MGR_TC_PRIO)
		q_info.sched = PPA_QOS_SCHED_SP;
#endif
#if IS_ENABLED(CONFIG_NET_SCH_DRR)
	if (mqinfo->sched_type == QOS_MGR_TC_DRR) {
		q_info.sched = PPA_QOS_SCHED_WFQ;
		q_info.weight =
			mqinfo->q_info[q_index].quantum/(mqinfo->quanta * 1024);
	}
#endif

	/* For tcont netdevice deq_idx is passed by pon ethernet datapath driver.
	 * deq_idx should be 0 for any other net device. So right now this
	 * is used to determine whether it is US or DS.
	 */
	if (mqinfo->deq_idx != 0)
		q_info.flags = NETDEV_TYPE_TCONT;

	/* When there is more than 1 GEM share the same queue (class id) then
	 * just update the qmap table with the gem subif. If queue map
	 * set passed then increase the ref_cnt for that queue.
	 */
	if (mqinfo->q_info[q_index].queue_id != -1) {
		pr_debug("Queue handler is already used for Queue Id:[%d]\n",
				mqinfo->q_info[q_index].queue_id);

		ret = qos_mgr_set_mapped_queue(
				dev,
				mqinfo->port_id,
				mqinfo->deq_idx,
				mqinfo->q_info[q_index].queue_id, /** Queue Id*/
				0, /* 0 - Egress 1 - Ingress */
				tcIndex, /* Traffic class */
				q_info.flags);

		if (ret == PPA_SUCCESS) {
			mqinfo->q_info[q_index].ref_cnt++;
			pr_debug("Queue Ref count: [%d]\n",
				mqinfo->q_info[q_index].ref_cnt);
		}
		return ret;
	}

	/* update default queue settings */
	qos_mgr_add_default_queue_settings(&q_info);

	q_info.flowId_en = 1;
	if (qosal_add_qos_queue(
			&q_info,
			&p_item) != PPA_SUCCESS)
		return PPA_FAILURE;
	if (p_item->p_entry < 0)
		return PPA_FAILURE;

	pr_debug("<%s> Returned Queue Id: [%d]\n",
		__func__, p_item->p_entry);

	/* Update local database */
	mqinfo->q_info[q_index].p_item = p_item;
	mqinfo->q_info[q_index].queue_id = p_item->p_entry;
	mqinfo->no_of_queue++;
	return PPA_SUCCESS;
}
#endif

#define TC_INGRESS 0
#define TC_EGRESS 1
#define TC_INVALID 2
static int32_t get_clsact_ingress_egress(u32 classid, bool *ingress)
{
	/* This also returns true for ingress qdisc */
	if (TC_H_MAJ(classid) == TC_H_MAJ(TC_H_CLSACT) &&
		TC_H_MIN(classid) != TC_H_MIN(TC_H_MIN_EGRESS))
		*ingress = TC_INGRESS;
	else if (TC_H_MAJ(classid) == TC_H_MAJ(TC_H_CLSACT) &&
		TC_H_MIN(classid) == TC_H_MIN(TC_H_MIN_EGRESS))
		*ingress = TC_EGRESS;
	return 0;
}

/** The qos_mgr_tc_flower_action_parse function is the main function for
 *  parsing and storing flower actions.
 */
static void qos_mgr_tc_flower_action_parse(const struct tc_action *a,
			struct qos_mgr_tc_flow_rule_list_item *tc_fl_item)
{
	struct qos_mgr_act_vlan *vlan_act;

	if (!tc_fl_item)
		return;

	vlan_act = &tc_fl_item->flow_action.action;

	if (tcf_vlan_action(a) == TCA_VLAN_ACT_POP) {
		pr_debug("<%s><%d> TCA_VLAN_ACT_POP\n",
				__func__, __LINE__);
		vlan_act->act |= QOS_MGR_VLAN_ACT_POP;
		vlan_act->pop_n++;
		return;
	}

	if (tcf_vlan_action(a) != TCA_VLAN_ACT_PUSH &&
	    tcf_vlan_action(a) != TCA_VLAN_ACT_MODIFY) {
		pr_debug("<%s><%d> TCA_VLAN_ACT_UNKNOWN\n",
				__func__, __LINE__);
		return;
	}

	if (tcf_vlan_action(a) == TCA_VLAN_ACT_PUSH) {
		pr_debug("<%s><%d> TCA_VLAN_ACT_PUSH\n",
				__func__, __LINE__);
		vlan_act->act |= QOS_MGR_VLAN_ACT_PUSH;
	} else if (tcf_vlan_action(a) == TCA_VLAN_ACT_MODIFY) {
		pr_debug("<%s><%d> TCA_VLAN_ACT_MODIFY\n",
				__func__, __LINE__);
		pr_debug("Modify: vid:%x prio: %x tpid: %x\n",
			tcf_vlan_push_vid(a),
			((struct tcf_vlan *)a)->tcfv_push_prio,
			tcf_vlan_push_proto(a));
		vlan_act->act |= QOS_MGR_VLAN_ACT_MODIFY;
		vlan_act->pop_n++;
	}

	if (tc_fl_item->dscp.enable) {
		tc_fl_item->dscp.map[tc_fl_item->dscp.value] =
			((struct tcf_vlan *)a)->tcfv_push_prio;
		vlan_act->prio[vlan_act->push_n] = DERIVE_FROM_DSCP;
	} else {
		vlan_act->prio[vlan_act->push_n] =
			((struct tcf_vlan *)a)->tcfv_push_prio;
	}

	vlan_act->vid[vlan_act->push_n] = tcf_vlan_push_vid(a);
	vlan_act->tpid[vlan_act->push_n] = tcf_vlan_push_proto(a);
	vlan_act->push_n++;
}

/** The qos_mgr_tc_flower_parse function is the main function for parsing the flower rule
	- match the rule based on the flower key and mask
	- store the matched rule in tc_fl_item
	- store the flower actions
*/
static int32_t qos_mgr_tc_flower_parse(
				struct net_device *dev,
				struct tc_cls_flower_offload *cls_flower,
				bool ingress,
				struct qos_mgr_tc_flow_rule_list_item *tc_fl_item)
{
	struct tc_cls_flower_offload *f = cls_flower;
	const struct tc_action *a;
	int32_t indev = (int) *(int *)f->key;
	int32_t vlanprio = -1;
#if IS_ENABLED(CONFIG_NET_ACT_COLMARK)
	uint32_t flags;
	struct dp_meter_cfg meter_cfg = {0};
#endif
	LIST_HEAD(actions);

	struct net_device *in_dev = dev_get_by_index(&init_net, indev);
	pr_debug("indev: %#x type: %s\n",
			indev, ingress ? "egress" : "ingress");
	/* pr_debug("classid: %#x\n", f->classid); */
	if (in_dev != NULL) {
		pr_debug("Dev: name: %s - ifindex: %d\n",
				in_dev->name, in_dev->ifindex);
	}
	pr_debug("Prio:%d\n", cls_flower->common.prio >> 16);
	pr_debug("Protocol:%02x\n", cls_flower->common.protocol);
	pr_debug("Class Id:%02x\n", cls_flower->common.classid);

	if (tc_fl_item != NULL) {
		tc_fl_item->dir = ingress;
		tc_fl_item->priority = cls_flower->common.prio >> 16;
		tc_fl_item->proto = cls_flower->common.protocol;
		tc_fl_item->parentid = f->classid;
		tc_fl_item->indev = in_dev;
	}
	if (in_dev)
		dev_put(in_dev);

	if (f->dissector->used_keys &
			~(BIT(FLOW_DISSECTOR_KEY_CONTROL) |
				BIT(FLOW_DISSECTOR_KEY_BASIC) |
				BIT(FLOW_DISSECTOR_KEY_ETH_ADDRS) |
				BIT(FLOW_DISSECTOR_KEY_VLAN) |
				BIT(FLOW_DISSECTOR_KEY_IPV4_ADDRS) |
				BIT(FLOW_DISSECTOR_KEY_IPV6_ADDRS) |
				BIT(FLOW_DISSECTOR_KEY_IP) |
				BIT(FLOW_DISSECTOR_KEY_PORTS))) {
		pr_debug("Unsupported key used: 0x%x\n",
				f->dissector->used_keys);
	}
	pr_debug("Supported key used: 0x%x\n",
			f->dissector->used_keys);

	if (tc_fl_item != NULL) {
		tc_fl_item->flow_pattern.match_to = QOS_MGR_MATCH_UNKNOWN;
		tc_fl_item->flow_pattern.used_keys = f->dissector->used_keys;
	}
	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_CONTROL)) {
		struct flow_dissector_key_control *key =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_CONTROL,
					f->key);
		pr_debug("Key Addr Type:%d\n", key->addr_type);
	}

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_BASIC)) {
		struct flow_dissector_key_basic *key =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_BASIC,
					f->key);
		struct flow_dissector_key_basic *mask =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_BASIC,
					f->mask);
		pr_debug("IP Proto:%d mask:%d\n",
				key->ip_proto, mask->ip_proto);
		pr_debug("Proto:%02x mask:%02x\n",
				key->n_proto, mask->n_proto);
		if (tc_fl_item != NULL) {
			tc_fl_item->flow_pattern.match_to = QOS_MGR_MATCH_PROTO;
			if (mask->n_proto == 0xffff)
				tc_flower_store_vlan_proto_match(0, key, mask, tc_fl_item);
		}
	}

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_ETH_ADDRS)) {
		struct flow_dissector_key_eth_addrs *key =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_ETH_ADDRS,
					f->key);
		struct flow_dissector_key_eth_addrs *mask =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_ETH_ADDRS,
					f->mask);

		pr_debug("MASK src:%s dst:%s\n", mask->src, mask->dst);
		pr_debug("SRC MAC MASK: %02x:%02x:%02x:%02x:%02x:%02x\n",
				mask->src[0], mask->src[1],
				mask->src[2], mask->src[3],
				mask->src[4], mask->src[5]);
		pr_debug("DST MAC MASK: %02x:%02x:%02x:%02x:%02x:%02x\n",
				mask->dst[0], mask->dst[1],
				mask->dst[2], mask->dst[3],
				mask->dst[4], mask->dst[5]);
		pr_debug("SRC MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
				key->src[0], key->src[1],
				key->src[2], key->src[3],
				key->src[4], key->src[5]);
		pr_debug("DST MAC : %02x:%02x:%02x:%02x:%02x:%02x\n",
				key->dst[0], key->dst[1],
				key->dst[2], key->dst[3],
				key->dst[4], key->dst[5]);
		if (tc_fl_item != NULL)
			tc_flower_store_eth_addr_match(key, mask, tc_fl_item);
	}

	/* Classification/Matching arguments parsing */
	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_VLAN)) {
		struct flow_dissector_key_vlan *key =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_VLAN,
					f->key);
		struct flow_dissector_key_vlan *mask =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_VLAN,
					f->mask);
		pr_debug("Match vid: %#x pcp: %#x\n",
				key->vlan_id, key->vlan_priority);
		vlanprio = key->vlan_priority;

		if (tc_fl_item != NULL) {
			tc_flower_store_vlan_match(0, key, mask, tc_fl_item);
		}
	}

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_IP)) {
		struct flow_dissector_key_ip *ip_key =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_IP,
					f->key);
		struct flow_dissector_key_ip *ip_mask =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_IP,
					f->mask);
		pr_debug("IP Match tos: %#x ttl: %#x\n",
				ip_key->tos, ip_key->ttl);

		if (tc_fl_item != NULL)
			tc_flower_store_ip_match(ip_key, ip_mask, tc_fl_item);
	}

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_CVLAN)) {
		struct flow_dissector_key_vlan *cvlan_key =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_CVLAN,
					f->key);
		struct flow_dissector_key_vlan *cvlan_mask =
			skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_CVLAN,
					f->mask);
		pr_debug("CVLAN Match vid: %#x pcp: %#x\n",
				cvlan_key->vlan_id, cvlan_key->vlan_priority);
		if (tc_fl_item != NULL)
			tc_flower_store_vlan_match(1, cvlan_key, cvlan_mask, tc_fl_item);
	}
	if (tc_fl_item != NULL)
		tc_fl_item->status = 0;

	if (tc_no_actions(f->exts)) {
		pr_debug("TC No Action\n");
		return -EINVAL;
	} else {
		pr_debug("No Of Action:%d\n", (f->exts)->nr_actions);
	}
	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list) {
		if (is_tcf_gact_shot(a)) {
			pr_debug("GACT SHOT\n");
			if (tc_fl_item != NULL) {
				tc_fl_item->flow_action.act_type = TC_ACT_SHOT;
				tc_fl_item->flow_action.action.act = QOS_MGR_ACT_DROP;
			}
			continue;
		} else if (((struct tcf_gact *)a)->tcf_action == TC_ACT_OK) {
			pr_debug("ACTION OK\n");
			tc_fl_item->flow_action.action.act = QOS_MGR_ACT_FWD;
#if IS_ENABLED(CONFIG_NET_SCH_MQPRIO)
			if (tc_fl_item != NULL)
				tc_fl_item->flow_action.act_type = TC_ACT_OK;
			if (f->command == TC_CLSFLOWER_REPLACE) {
				if (tc_fl_item->parentid) {
					/* vlanprio will be set to (0 .. 7) for downstream p-bit
					 * priotarization otherwise -1 (means the priority will be used
					 * as traffic class). This traffic class will be used for 
					 * queue map configuration.
					 */
					if (qos_mgr_add_queue(dev, tc_fl_item->indev, tc_fl_item->parentid, vlanprio) == PPA_FAILURE) {
						pr_debug("<%s><%d> Queue Add Failed\n", __func__, __LINE__);
						return PPA_FAILURE;
					}
					tc_fl_item->flow_pattern.match_to = QOS_MGR_MATCH_UNKNOWN;
				}
			} else if (f->command == TC_CLSFLOWER_DESTROY)
				qos_mgr_del_queue(dev, NULL, f->classid);
#endif
			continue;
		} else if (((struct tcf_gact *)a)->tcf_action == TC_ACT_UNSPEC) {
			pr_debug("ACTION UNSPEC\n");
			if (tc_fl_item != NULL) {
				tc_fl_item->flow_action.act_type = TC_ACT_UNSPEC;
				tc_fl_item->status = 1;
			}
			continue;
		}

		if (is_tcf_mirred_redirect(a)) {
			int ifindex = tcf_mirred_ifindex(a);
			struct net_device *out_dev;

			pr_debug("<%s><%d> Redirect to ifindex:%d\n",
					__func__, __LINE__, ifindex);

			out_dev = __dev_get_by_index(&init_net, ifindex);
			continue;
		}

#if IS_ENABLED(CONFIG_NET_ACT_COLMARK)
		if (is_tcf_colmark(a)) {
			pr_debug("<%s><%d> TCA_COLMARK\n",
					__func__, __LINE__);
			tc_fl_item->flow_pattern.match_to = QOS_MGR_MATCH_METER;
			if (tcf_colmark_mode(a))
				meter_cfg.col_mode = tcf_colmark_mode(a);
			if (tcf_colmark_precedence(a))
				meter_cfg.mode = tcf_colmark_precedence(a);
			meter_cfg.type = tcf_colmark_mtype(a);
			pr_debug("<%s><%d> Mode:[%d] Precedence: [%d] Mtype: [%d] \n",
					__func__, __LINE__, meter_cfg.col_mode, meter_cfg.mode, meter_cfg.type);
			meter_cfg.dir = tc_fl_item->dir;

			flags = DP_METER_COL_MARKING_ONLY;
			if (tc_fl_item->indev != NULL) {
				if (!strncmp(dev->name, tc_fl_item->indev->name, strlen(dev->name)))
					flags |= DP_METER_ATTACH_BRPORT;
			} else
				flags |= DP_METER_ATTACH_CTP;

			tc_fl_item->meter_id = qos_mgr_cfg_meter_to_dp(dev, &meter_cfg, flags);
			if (tc_fl_item->meter_id == PPA_FAILURE)
				return -1;
			pr_debug("Meter Id: [%d]\n", tc_fl_item->meter_id);
			continue;
		}
#endif
		if (is_tcf_vlan(a)) {
			qos_mgr_tc_flower_action_parse(a, tc_fl_item);
			continue;
		}
	}
	return 0;
}

/** This function is merging DSCP rule into existing one. */
int32_t qos_mgr_tc_flower_merger_dscp(
	struct qos_mgr_tc_flow_rule_list_item *p_old,
	struct qos_mgr_tc_flow_rule_list_item *p_new)
{
	struct qos_mgr_act_vlan *vold, *vnew;

	if (p_old->indev != p_new->indev)
		return PPA_FAILURE;

	if (p_old->dir != p_new->dir)
		return PPA_FAILURE;

	if (p_old->proto != p_new->proto)
		return PPA_FAILURE;

	vold = &p_old->flow_action.action;
	vnew = &p_new->flow_action.action;

	if (vold->act != vnew->act)
		return PPA_FAILURE;

	if ((vold->act & QOS_MGR_VLAN_ACT_POP) &&
		vold->pop_n != vnew->pop_n)
		return PPA_FAILURE;

	if ((vold->act & QOS_MGR_VLAN_ACT_PUSH) &&
	     vold->push_n != vnew->push_n &&
	     vold->vid[vold->push_n] != vnew->vid[vnew->push_n] &&
	     vold->tpid[vold->push_n] != vnew->tpid[vnew->push_n])
		return PPA_FAILURE;

	p_old->dscp.map[p_new->dscp.value] = p_new->dscp.map[p_new->dscp.value];

	return PPA_SUCCESS;
}

/** This function is merging chosen rules into existing ones. */
int32_t qos_mgr_tc_flower_merger(
	struct qos_mgr_tc_flow_rule_list_item *p_new,
	struct qos_mgr_tc_flow_rule_list_item *obj_head)
{
	struct qos_mgr_tc_flow_rule_list_item *p_old;
	int32_t ret = PPA_FAILURE;

	if (!p_new)
		return ret;

	if (!p_new->dscp.enable)
		return ret;

	qos_mgr_tc_lock_list();
	for (p_old = obj_head; p_old; p_old = p_old->next) {
		if (qos_mgr_tc_flower_merger_dscp(p_old, p_new) != PPA_SUCCESS)
			continue;

		qos_mgr_tc_free_list_item(p_new);
		ret = PPA_SUCCESS;
		break;
	}
	qos_mgr_tc_unlock_list();

	return ret;
}

/* Adds a flow rule to the list and sends the list to dp */
static int32_t
tc_add_item_and_send_to_dp(struct net_device *dev, uint32_t dir,
			   struct qos_mgr_tc_flow_rule_list_item **fl_list,
			   struct qos_mgr_tc_flow_rule_list_item *tc_fl_item,
			   uint32_t prio)
{
	struct qos_mgr_tc_flow_rule_list_item *first_item = NULL;
	int32_t ret = PPA_SUCCESS;

	first_item = *fl_list;

	/* Attempt to merge a rule into a current rule in the list
	 * This is needed to merge DSCP filters into one rule */
	ret = qos_mgr_tc_flower_merger(tc_fl_item, first_item);
	if (ret != PPA_SUCCESS)
		/* Add a new list item only if we failed to merge
		 * This will happen when a first DSCP filter comes or when
		 * a non DSCP rule is added */
		__qos_mgr_tc_add_list_item(tc_fl_item, fl_list);

	/* Call Ext. Vlan only if we have valid parent id for Ingress */
	ret = qos_mgr_tc_flow_find_parentid(prio, first_item, NULL, NULL);
	if (ret != PPA_SUCCESS) {
		pr_debug("No Valid parent id for Ingress so calling ext vlan\n");
		/* TODO: Provide error handling */
		qos_mgr_set_ext_vlan_to_dp(dev, dir, first_item);
	}

	return PPA_SUCCESS;
}

/** The qos_mgr_setup_tc function is the main function for TC HW offload
	- this is implemented with support for the tc hooks using the kernel ndo_setup_tc ops
	- extracting direction(ingress/egress), priority, protocol
	- offload the extended VLAN rule of the flower command
	- collect all VLAN information for this CTP or brige port based on
	- specified direction Ingress or Egress
	- for every netdevice two linked lists are maintained per direction
	- Based on the netdevice and direction, rules are added to either ingress/egress linked list
*/
int32_t qos_mgr_setup_tc(struct net_device *dev,
			u32 handle,
			__be16 protocol,
			struct tc_to_netdev *tc)
{
	int32_t ret = PPA_SUCCESS;

	pr_debug("======================================================\n");
	pr_debug("<%s> TC Setup for device %s === Handle: %x Protocol: %x\n",
				__func__, dev->name, handle, protocol);
#if IS_ENABLED(CONFIG_NET_SCH_MQPRIO)
	if (tc->type == TC_SETUP_MQPRIO) {
		int32_t dev_index;
		dev_index = qos_mgr_get_dev_node_match_index(dev);
		if (dev_index == PPA_FAILURE) {
			pr_debug("No match found !!!\n");
			dev_index = 0;
			dev_index = qos_mgr_get_free_dev_node_index();
			if (dev_index == PPA_FAILURE)
				return -1;
		}
		if (tc->mqprio->num_tc != 0) {
			pr_debug("MQPRIO ADD\n");
			tc_qos_mgr_db[dev_index].dev = dev;
			qos_mgr_find_and_store_mqprio_handle(dev_index, handle, tc->mqprio);
		} else {
			pr_debug("MQPRIO DEL\n");
			ret = qos_mgr_delete_mqprio_handle(dev_index);
			if (ret == PPA_SUCCESS)
				qos_mgr_free_dev_node_index(dev_index);
		}
	}
#endif

#if IS_ENABLED(CONFIG_NET_SCH_DRR)
	if (tc->type == TC_SETUP_DRR) {
		ret = qos_mgr_tc_drr_offload(dev, handle, tc);
	}
#endif
#if IS_ENABLED(CONFIG_NET_SCH_PRIO)
	if (tc->type == TC_SETUP_QDISC_PRIO)
		ret = qos_mgr_tc_qdisc_prio_offload(dev, handle, tc);
#endif

	if (tc->type == TC_SETUP_CLSFLOWER) {
		switch (tc->cls_flower->command) {
			bool ingress;
			int32_t dev_index;
			uint32_t prio, pid = 0;
			struct qos_mgr_tc_flow_rule_list_item *tc_fl_item = NULL;
		case TC_CLSFLOWER_REPLACE:
			{
				pr_debug("TC_CLSFLOWER_REPLACE\n");
				prio = tc->cls_flower->common.prio >> 16;
				get_clsact_ingress_egress(
					tc->cls_flower->common.classid,
					&ingress);
				dev_index = qos_mgr_get_dev_node_match_index(dev);
				if (dev_index == PPA_FAILURE) {
					pr_debug("No match found\n");
					dev_index = qos_mgr_get_free_dev_node_index();
					if (dev_index == PPA_FAILURE)
						return -1;
				}
				pr_debug("New device index in DB : %d dev pointer %p name %s\n", dev_index, dev, dev->name);
				tc_qos_mgr_db[dev_index].dev = dev;
				tc_fl_item = tc_qos_mgr_alloc_list_item();
				if (!tc_fl_item)
					pr_err("Obj error\n");

				if (qos_mgr_tc_flower_parse(dev, tc->cls_flower, ingress, tc_fl_item) == PPA_FAILURE) {
					pr_debug("<%s><%d> TC Flower parsing failed\n", __func__, __LINE__);
					qos_mgr_tc_free_list_item(tc_fl_item);
					qos_mgr_free_dev_node_index(dev_index);
					return -1;
				}

				if (ingress == 0)
					ret = tc_add_item_and_send_to_dp(dev,
									 DP_DIR_INGRESS,
									 &tc_qos_mgr_db[dev_index].flow_item_ig,
									 tc_fl_item,
									 prio);
				else
					ret = tc_add_item_and_send_to_dp(dev,
									 DP_DIR_EGRESS,
									 &tc_qos_mgr_db[dev_index].flow_item_eg,
									 tc_fl_item,
									 prio);
				break;
			}
		case TC_CLSFLOWER_DESTROY:
			{
				uint32_t ext_set = 0;
				struct net_device *indev;
				pr_debug("TC_CLSFLOWER_DESTROY\n");
				dev_index = qos_mgr_get_dev_node_match_index(dev);
				if (dev_index == PPA_FAILURE) {
					return 0;
				} else {
					prio = tc->cls_flower->common.prio >> 16;
					get_clsact_ingress_egress(tc->cls_flower->common.classid, &ingress);
					if (ingress == 0) {
						if (qos_mgr_tc_flow_find_parentid(prio,
								(tc_qos_mgr_db[dev_index].flow_item_ig),
								&indev,
								&pid) == PPA_SUCCESS) {
							if (qos_mgr_del_queue(dev, indev, pid) != PPA_SUCCESS) {
								pr_debug("<%s><%d> Queue deletion failed for Ingress\n", __func__, __LINE__);
								return -1;
							}
							ext_set = 1;
						}
						if (tc_qos_mgr_db[dev_index].flow_item_ig != NULL)
							tc_qos_mgr_remove_list_item(prio, &(tc_qos_mgr_db[dev_index].flow_item_ig));

						if (ext_set == 0)
							qos_mgr_set_ext_vlan_to_dp(dev, DP_DIR_INGRESS, tc_qos_mgr_db[dev_index].flow_item_ig);

						qos_mgr_free_dev_node_index(dev_index);
					} else {
						if (qos_mgr_tc_flow_find_parentid(prio,
								(tc_qos_mgr_db[dev_index].flow_item_eg),
								&indev,
								&pid) == PPA_SUCCESS) {
							if (qos_mgr_del_queue(dev, indev, pid) != PPA_SUCCESS) {
								pr_debug("<%s><%d> Queue deletion failed for Egress\n", __func__, __LINE__);
								return -1;
							}
							ext_set = 1;
						}
						if (tc_qos_mgr_db[dev_index].flow_item_eg != NULL)
							tc_qos_mgr_remove_list_item(prio, &(tc_qos_mgr_db[dev_index].flow_item_eg));
						if (ext_set == 0)
							qos_mgr_set_ext_vlan_to_dp(dev, DP_DIR_EGRESS, tc_qos_mgr_db[dev_index].flow_item_eg);

						qos_mgr_free_dev_node_index(dev_index);
					}
				}
				break;
			}
		case TC_CLSFLOWER_STATS:
			{
				pr_debug("TC_CLSFLOWER_STATS\n");
				dev_index = qos_mgr_get_dev_node_match_index(dev);
				if (dev_index == PPA_FAILURE) {
					return -1;
				} else {
					get_clsact_ingress_egress(tc->cls_flower->common.classid, &ingress);
				}
				break;
			}
		default:
			return -EINVAL;
		}

	}
	return ret;
}

int32_t qos_mgr_setup_tc_ext(struct net_device *dev,
			u32 handle,
			__be16 protocol,
			struct tc_to_netdev *tc,
			int32_t deq_idx,
			int32_t port_id)
{
	int32_t dev_index;
	int32_t ret = PPA_SUCCESS;
	pr_debug("TC Setup for device %s === Handle: %x Protocol: %x\n",
				dev->name, handle, protocol);
	pr_debug("deq_idx: [%d] port_id: [%d]\n", deq_idx, port_id);
	if (qos_mgr_setup_tc(dev, handle, protocol, tc) == -1) {
		pr_debug("qos_mgr_setup_tc failed\n");
		return PPA_FAILURE;
	}

	dev_index = qos_mgr_get_dev_node_match_index(dev);
	if (dev_index == PPA_FAILURE)
		return PPA_FAILURE;

	tc_qos_mgr_db[dev_index].qos_info.deq_idx = deq_idx;
	tc_qos_mgr_db[dev_index].qos_info.port_id = port_id;
	return ret;
}

static int32_t qos_mgr_tc_db_init(void)
{
	int32_t i = 0, j = 0;
	int32_t err = PPA_SUCCESS;
	struct dp_qos_cfg_info qos_cfg = {0};

	err = dp_qos_global_info_get(&qos_cfg, 0);
	if (err == DP_FAILURE) {
		pr_debug("<%s> dp_qos_global_info_get failed\n", __func__);
		return PPA_FAILURE;
	}

	pr_debug("Quanta=[%x]\n", qos_cfg.quanta);
	pr_debug("size of DB : {%d} * {%d} = [%d] \n",
		sizeof(struct _tc_qos_mgr_db), QOS_MGR_MAX_IFACE, sizeof(struct _tc_qos_mgr_db) * QOS_MGR_MAX_IFACE);
	for (i = 0; i < QOS_MGR_MAX_IFACE; i++) {
		tc_qos_mgr_db[i].dev = NULL;
		tc_qos_mgr_db[i].flow_item_ig = NULL;
		tc_qos_mgr_db[i].flow_item_eg = NULL;
		for (j = 0; j < QOS_MGR_MAX_IFACE; j++) {
			memset(&tc_qos_mgr_db[i].qos_info, 0, sizeof(struct qos_mgr_tc_queue_info));
		}
		tc_qos_mgr_db[i].qos_info.port_id = -1;
		tc_qos_mgr_db[i].qos_info.deq_idx = 0;
		tc_qos_mgr_db[i].qos_info.no_of_queue = 0;
		tc_qos_mgr_db[i].qos_info.quanta = qos_cfg.quanta;
		tc_qos_mgr_db[i].qos_info.sched_type = 0;
		for (j = 0; j < QOS_MGR_MAX_QUEUE_IFACE; j++) {
			tc_qos_mgr_db[i].qos_info.q_info[j].queue_id = -1;
		}
	}
	pr_debug("<%s> Init Done.\n", __func__);
	return err;
}

static int32_t qos_mgr_find_no_of_vlan_rules(
				struct net_device *dev,
				struct qos_mgr_tc_flow_rule_list_item  *fl_list,
				int32_t *vlan0,
				int32_t *vlan1,
				int32_t *vlan2)
{
	int32_t i = 0, j = 0, k = 0;
	struct qos_mgr_tc_flow_rule_list_item *p;

	for (p = fl_list; p; p = p->next) {
		if (p->flow_pattern.match_to == QOS_MGR_MATCH_PROTO)
			i++;
		else if ((p->flow_pattern.match_to == QOS_MGR_MATCH_VLAN) && (p->dir != TC_INVALID)) {
			if (p->flow_pattern.vlan_match.no_of_tag == 1) {
				j++;
				if (p->indev != NULL) {
					if (!strncmp(dev->name, p->indev->name, strlen(dev->name))) {
						k++;
						pr_debug("Increase the count for double tag %d\n", k);
					}
				}
			} else if (p->flow_pattern.vlan_match.no_of_tag == 2)
				k++;
		}
	}
	*vlan0 = i;
	*vlan1 = j;
	*vlan2 = k;
	return 0;
}

#ifdef QOS_MGR_TC_DBG
static void qos_mgr_show_dp_info(struct dp_tc_vlan *dp_vlan)
{
	int32_t i = 0, j = 0;

	pr_info("<%s> netdevice: %s\n", __func__, dp_vlan->dev->name);
	pr_info("<%s> Direction: %d\n", __func__, dp_vlan->dir);
	pr_info("==== No of No vlan tag:%d Single vlan:%d Double vlan:%d ====\n",
			dp_vlan->n_vlan0,
			dp_vlan->n_vlan1,
			dp_vlan->n_vlan2);
	for (i = 0; i < dp_vlan->n_vlan0; i++) {
		pr_info("Pattern: Proto:%d\n", dp_vlan->vlan0_list->outer.proto);
		pr_info("Default: %d\n", dp_vlan->vlan0_list->def);
		pr_info("Action %d\n", dp_vlan->vlan0_list->act.act);
		for (j = 0; j < dp_vlan->vlan0_list->act.push_n; j++) {
			pr_info("tpid:[%x] vid: [%d] prio: [%d]\n",
					dp_vlan->vlan0_list->act.tpid[j],
					dp_vlan->vlan0_list->act.vid[j],
					dp_vlan->vlan0_list->act.prio[j]);
		}
	}
	for (i = 0; i < dp_vlan->n_vlan1; i++) {
		pr_info("Single VLAN\n");
		pr_info("Default: %d\n", dp_vlan->vlan1_list->def);
		pr_info("Pattern: Proto:%d\n", dp_vlan->vlan1_list->outer.proto);
		pr_info("Pattern: Vid:%d\n", dp_vlan->vlan1_list->outer.vid);
		pr_info("Pattern: Prio:%d\n", dp_vlan->vlan1_list->outer.prio);
		pr_info("Pattern: tpid:%d\n", dp_vlan->vlan1_list->outer.tpid);

		pr_info("Action %d\n", dp_vlan->vlan1_list->act.act);
		if (dp_vlan->vlan1_list->act.act == DP_VLAN_ACT_POP)
			pr_info("No of POP ACTion: [%d]\n", dp_vlan->vlan1_list->act.pop_n);
		else if (dp_vlan->vlan1_list->act.act == DP_VLAN_ACT_PUSH) {
			pr_info("No of PUSH ACTion: [%d]\n", dp_vlan->vlan1_list->act.push_n);
			for (j = 0; j < dp_vlan->vlan1_list->act.push_n; j++) {
				pr_info("tpid:[%x] vid: [%d] prio: [%d]\n",
						dp_vlan->vlan1_list->act.tpid[j],
						dp_vlan->vlan1_list->act.vid[j],
						dp_vlan->vlan1_list->act.prio[j]);
			}
		}
	}
	for (i = 0; i < dp_vlan->n_vlan2; i++) {
		pr_info("Double VLAN\n");
		pr_info("Default: %d\n", dp_vlan->vlan2_list->def);
		pr_info("Pattern Outer: Proto:%d\n", dp_vlan->vlan2_list->outer.proto);
		pr_info("Pattern: Vid:%d\n", dp_vlan->vlan2_list->outer.vid);
		pr_info("Pattern: Prio:%d\n", dp_vlan->vlan2_list->outer.prio);

		pr_info("Pattern: Inner Proto:%d\n", dp_vlan->vlan2_list->inner.proto);
		pr_info("Pattern: Vid:%d\n", dp_vlan->vlan2_list->inner.vid);
		pr_info("Pattern: Prio:%d\n", dp_vlan->vlan2_list->inner.prio);

		pr_info("Action %d\n", dp_vlan->vlan2_list->act.act);
		if (dp_vlan->vlan2_list->act.act == DP_VLAN_ACT_POP)
			pr_info("No of POP ACTion: [%d]\n", dp_vlan->vlan2_list->act.pop_n);
		else if (dp_vlan->vlan2_list->act.act == DP_VLAN_ACT_PUSH) {
			pr_info("No of PUSH ACTion: [%d]\n", dp_vlan->vlan2_list->act.push_n);
			for (j = 0; j < dp_vlan->vlan2_list->act.push_n; j++) {
				pr_info("tpid:[%x] vid: [%d] prio: [%d]\n",
						dp_vlan->vlan2_list->act.tpid[j],
						dp_vlan->vlan2_list->act.vid[j],
						dp_vlan->vlan2_list->act.prio[j]);
			}
		}

	}
}
#endif

static void qos_mgr_set_filter_double_tag(
				int32_t def_rule,
				struct dp_vlan2 *rule2,
				struct qos_mgr_tc_flow_rule_list_item  *p)
{
	pr_debug("Configure Double tag for filter rule\n");
	(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_ID) ?
		(rule2->outer.vid = p->flow_pattern.vlan_match.entry[0].vid) : (rule2->outer.vid = DP_VLAN_PATTERN_NOT_CARE);
	(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_PRIO) ?
		(rule2->outer.prio = p->flow_pattern.vlan_match.entry[0].prio) : (rule2->outer.prio = DP_VLAN_PATTERN_NOT_CARE);
	(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_PROTO) ?
		(rule2->outer.proto = p->flow_pattern.vlan_match.entry[0].proto) : (rule2->outer.proto = DP_VLAN_PATTERN_NOT_CARE);
	rule2->outer.tpid = DP_VLAN_PATTERN_NOT_CARE;
	rule2->outer.dei = DP_VLAN_PATTERN_NOT_CARE;

	rule2->inner.vid = DP_VLAN_PATTERN_NOT_CARE;
	rule2->inner.prio = DP_VLAN_PATTERN_NOT_CARE;
	rule2->inner.proto = DP_VLAN_PATTERN_NOT_CARE;
	rule2->inner.tpid = DP_VLAN_PATTERN_NOT_CARE;
	rule2->inner.dei = DP_VLAN_PATTERN_NOT_CARE;

	rule2->def = def_rule;
	rule2->act.act = DP_VLAN_ACT_FWD;
	if (p->flow_action.action.act == QOS_MGR_ACT_DROP) {
		rule2->act.act = DP_VLAN_ACT_DROP;
	}
}

#if IS_ENABLED(CONFIG_NET_ACT_COLMARK)
static int32_t qos_mgr_cfg_meter_to_dp(
				struct net_device *dev,
				struct dp_meter_cfg  *meter_cfg,
				int32_t flag)
{
	int32_t ret = PPA_SUCCESS;
	int32_t meterid;

	/** TODO: DP instance is configured as 0. Need to check how the
		instance id can be determined.
	*/
	if (dp_meter_alloc(0, &meterid, 0) != DP_SUCCESS) {
		pr_err("<%s> Meter allocation failed.\n", __func__);
		return PPA_FAILURE;
	}

	pr_debug("<%s> Meter Id is : [%d]\n", __func__, meterid);
	ret = meterid;
	meter_cfg->meter_id = meterid;
	if (dp_meter_add(dev, meter_cfg, flag) != DP_SUCCESS) {
		pr_err("<%s> Meter config failed. Delete allocated meter \n", __func__);
		dp_meter_del(dev, meter_cfg, flag);
		ret = PPA_FAILURE;
	}
	return ret;
}
#endif

/** The qos_mgr_dscp_pcp_map_update() function is used to update dscp_pcp_map
 *  in target map of extended vlan rule.
 */
static void qos_mgr_dscp_pcp_map_update(
				struct qos_mgr_tc_flow_rule_list_item *p,
				unsigned char *target_map)
{
	if (!p->dscp.enable)
		return;

	memcpy(target_map, p->dscp.map, QOS_MGR_DSCP_MAX);
}

#define QOS_MGR_TC_FILTER_DEF_PRIO_THRESHOLD 64000
/**
	The qos_mgr_set_ext_vlan_to_dp() function is used to get all the vlan rules from
	the flow rule list of the netdevice of either ingress/egress direction and then
	call dp_vlan_set() API.

	If the netdevice and indev are equal then it is a bridge port. Filter rules
	are configured for those netdevices.
		Ex: tc filter add dev pmapper4354 ingress prio 3 protocol 802.1q flower
			skip_sw indev pmapper4354 vlan_id 74 action ok
	For the filter rules, a default rule is MUST.
	The tc filter rules which has priority more than QOS_MGR_TC_FILTER_DEF_PRIO_THRESHOLD
	is consiered as default rule.
		Ex: tc filter add dev pmapper4354 ingress prio 65521 protocol 802.1q flower
			skip_sw indev pmapper4354 action drop

	VLAN filter only support VID, PCP, or TCI
	For the single tag vlan filter rule, a double tag rule is also configured with the
	outer VID or PCP or TCI is same as single tag vlan rule.
*/
static int32_t qos_mgr_set_ext_vlan_to_dp(
				struct net_device *dev,
				uint32_t dir,
				struct qos_mgr_tc_flow_rule_list_item  *fl_list)
{
	int32_t ret = PPA_SUCCESS;
	struct qos_mgr_tc_flow_rule_list_item *p;

	struct dp_tc_vlan dp_vlan = {0};
	struct dp_vlan0 *rule0_head = NULL;
	struct dp_vlan1 *rule1_head = NULL;
	struct dp_vlan2 *rule2_head = NULL;
	int32_t i = 0, isBrPort, isDefRule, total_rule;
	int32_t vlan0_index = 0, vlan1_index = 0, vlan2_index = 0;

	dp_vlan.dev = dev;
	dp_vlan.dir = dir;
	qos_mgr_lct_lan_ctp(dev, &dp_vlan);

	if (fl_list != NULL) {
		dp_vlan.dir = fl_list->dir;

		qos_mgr_find_no_of_vlan_rules(dev, fl_list, &dp_vlan.n_vlan0, &dp_vlan.n_vlan1, &dp_vlan.n_vlan2);

		if (dp_vlan.n_vlan0) {
			rule0_head = (struct dp_vlan0 *)ppa_malloc(sizeof(struct dp_vlan0) * dp_vlan.n_vlan0);
			if (!rule0_head) {
				pr_err("Can't allocate memory\n");
				ret = PPA_FAILURE;
				goto END;
			}
			memset(rule0_head, 0, (sizeof(struct dp_vlan0) * dp_vlan.n_vlan0));
		}
		if (dp_vlan.n_vlan1) {
			rule1_head = (struct dp_vlan1 *)ppa_malloc(sizeof(struct dp_vlan1) * dp_vlan.n_vlan1);
			if (!rule1_head) {
				pr_err("Can't allocate memory\n");
				ret = PPA_FAILURE;
				goto END;
			}
			memset(rule1_head, 0, (sizeof(struct dp_vlan1) * dp_vlan.n_vlan1));
		}
		if (dp_vlan.n_vlan2) {
			rule2_head = (struct dp_vlan2 *)ppa_malloc(sizeof(struct dp_vlan2) * dp_vlan.n_vlan2);
			if (!rule2_head) {
				pr_err("Can't allocate memory\n");
				ret = PPA_FAILURE;
				goto END;
			}
			memset(rule2_head, 0, (sizeof(struct dp_vlan2) * dp_vlan.n_vlan2));
		}

		total_rule = dp_vlan.n_vlan0 + dp_vlan.n_vlan1 + dp_vlan.n_vlan2;
		pr_debug("total no of rules: %d fl_list: %p\n", total_rule, fl_list);
		for (p = fl_list; p; p = p->next) {
			isBrPort = 0;
			isDefRule = 0;
			if (p->indev != NULL) {
				pr_debug("dev: %s indev: %s\n", dev->name, p->indev->name);
				if (!strncmp(dev->name, p->indev->name, strlen(dev->name))) {
					pr_debug("This is a Bridge Port %s\n", dev->name);
					isBrPort = 1;
					if (p->priority > QOS_MGR_TC_FILTER_DEF_PRIO_THRESHOLD) {
						isDefRule = DP_VLAN_DEF_RULE;
						pr_debug("Rule Prio [%d] It is a Default Rule \n", p->priority);
					}
				}
			}
			if ((p->flow_pattern.match_to == QOS_MGR_MATCH_VLAN) && (p->dir != TC_INVALID))
				pr_debug("<%s> No Of VLAN Tag %d\n", __func__,
						p->flow_pattern.vlan_match.no_of_tag);

			pr_debug("<%s><%d> Pattern match: %d Action: %d\n", __func__, __LINE__,
					p->flow_pattern.match_to, p->flow_action.action.act);
			if (p->flow_pattern.match_to == QOS_MGR_MATCH_PROTO) {
				struct dp_vlan0 *rule0 = NULL;
				rule0 = rule0_head + vlan0_index;

				rule0->outer.proto = p->proto;
				if (p->proto == 3)
					rule0->outer.proto = DP_VLAN_PATTERN_NOT_CARE;
				rule0->outer.prio = DP_VLAN_PATTERN_NOT_CARE;
				rule0->outer.vid = DP_VLAN_PATTERN_NOT_CARE;
				rule0->outer.tpid = DP_VLAN_PATTERN_NOT_CARE;
				rule0->outer.dei = DP_VLAN_PATTERN_NOT_CARE;

				rule0->def = isDefRule;
				rule0->act.act = DP_VLAN_ACT_FWD;
				if (p->flow_action.action.act == QOS_MGR_ACT_DROP) {
					if (qos_mgr_lct_configure(dev, rule0) != PPA_SUCCESS)
						rule0->act.act = DP_VLAN_ACT_DROP;
				} else if (p->flow_action.action.act == QOS_MGR_VLAN_ACT_PUSH) {
					rule0->act.act = DP_VLAN_ACT_PUSH;
					for (i = 0; i < p->flow_action.action.push_n; i++) {
						rule0->act.tpid[i] = p->flow_action.action.tpid[i];
						rule0->act.vid[i] = p->flow_action.action.vid[i];
						rule0->act.prio[i] = p->flow_action.action.prio[i];
						rule0->act.dei[i] = p->flow_action.action.dei[i];
					}
					rule0->act.push_n = p->flow_action.action.push_n;
				}
				qos_mgr_dscp_pcp_map_update(p, rule0->act.dscp_pcp_map);
				vlan0_index++;
			} else if ((p->flow_pattern.match_to == QOS_MGR_MATCH_VLAN) && (p->dir != TC_INVALID)) {
				if (p->flow_pattern.vlan_match.no_of_tag == 1) {
					struct dp_vlan1 *rule1 = NULL;
					rule1 = rule1_head + vlan1_index;

					(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_ID) ?
						(rule1->outer.vid = p->flow_pattern.vlan_match.entry[0].vid) : (rule1->outer.vid = DP_VLAN_PATTERN_NOT_CARE);
					(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_PRIO) ?
						(rule1->outer.prio = p->flow_pattern.vlan_match.entry[0].prio) : (rule1->outer.prio = DP_VLAN_PATTERN_NOT_CARE);
					(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_PROTO) ?
						(rule1->outer.proto = p->flow_pattern.vlan_match.entry[0].proto) : (rule1->outer.proto = DP_VLAN_PATTERN_NOT_CARE);
					(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_TPID) ?
						(rule1->outer.tpid = p->flow_pattern.vlan_match.entry[0].tpid) : (rule1->outer.tpid = DP_VLAN_PATTERN_NOT_CARE);
					rule1->outer.dei = DP_VLAN_PATTERN_NOT_CARE;

					rule1->def = isDefRule;
					if (isBrPort == 1)
						rule1->outer.tpid = DP_VLAN_PATTERN_NOT_CARE;

					if (p->flow_action.action.act == QOS_MGR_ACT_FWD)
						rule1->act.act = DP_VLAN_ACT_FWD;

					if (p->flow_action.action.act == QOS_MGR_ACT_DROP)
						rule1->act.act = DP_VLAN_ACT_DROP;

					if ((p->flow_action.action.act & QOS_MGR_VLAN_ACT_POP) == QOS_MGR_VLAN_ACT_POP) {
						rule1->act.act |= DP_VLAN_ACT_POP;
						rule1->act.pop_n = p->flow_action.action.pop_n;
					}
					if (((p->flow_action.action.act & QOS_MGR_VLAN_ACT_PUSH) == QOS_MGR_VLAN_ACT_PUSH)) {
						for (i = 0; i < p->flow_action.action.push_n; i++) {
							rule1->act.tpid[i] = p->flow_action.action.tpid[i];
							rule1->act.vid[i] = p->flow_action.action.vid[i];
							rule1->act.prio[i] = p->flow_action.action.prio[i];
							rule1->act.dei[i] = p->flow_action.action.dei[i];
						}
						rule1->act.act |= DP_VLAN_ACT_PUSH;
						rule1->act.push_n = p->flow_action.action.push_n;
					} else if ((p->flow_action.action.act == QOS_MGR_VLAN_ACT_MODIFY)) {
						for (i = 0; i < p->flow_action.action.push_n; i++) {
							rule1->act.tpid[i] = p->flow_action.action.tpid[i];
							rule1->act.vid[i] = p->flow_action.action.vid[i];
							(p->flow_action.action.prio[i]) ?
								(rule1->act.prio[i] =
							p->flow_action.action.prio[i]) : (rule1->act.prio[i] = CP_FROM_INNER);
							rule1->act.dei[i] = p->flow_action.action.dei[i];
						}
						rule1->act.act = DP_VLAN_ACT_POP | DP_VLAN_ACT_PUSH;
						rule1->act.push_n = p->flow_action.action.push_n;
						rule1->act.pop_n = 1;
					}
					qos_mgr_dscp_pcp_map_update(p, rule1->act.dscp_pcp_map);
					vlan1_index++;

					if (isBrPort == 1) {
						struct dp_vlan2 *rule2_filter = NULL;

						rule2_filter = rule2_head + vlan2_index;
						qos_mgr_set_filter_double_tag(rule1->def, rule2_filter, p);
						qos_mgr_dscp_pcp_map_update(p, rule2_filter->act.dscp_pcp_map);
						vlan2_index++;
					}
				} else if (p->flow_pattern.vlan_match.no_of_tag == 2) {
					struct dp_vlan2 *rule2 = NULL;

					rule2 = rule2_head + vlan2_index;

					(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_ID) ?
						(rule2->outer.vid = p->flow_pattern.vlan_match.entry[0].vid) : (rule2->outer.vid = DP_VLAN_PATTERN_NOT_CARE);
					(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_PRIO) ?
						(rule2->outer.prio = p->flow_pattern.vlan_match.entry[0].prio) : (rule2->outer.prio = DP_VLAN_PATTERN_NOT_CARE);
					(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_PROTO) ?
						(rule2->outer.proto = p->flow_pattern.vlan_match.entry[0].proto) : (rule2->outer.proto = DP_VLAN_PATTERN_NOT_CARE);
					(p->flow_pattern.vlan_match.entry[0].vlan_mask & QOS_MGR_VLAN_TPID) ?
						(rule2->outer.tpid = p->flow_pattern.vlan_match.entry[0].tpid) : (rule2->outer.tpid = DP_VLAN_PATTERN_NOT_CARE);
					rule2->outer.dei = DP_VLAN_PATTERN_NOT_CARE;

					(p->flow_pattern.vlan_match.entry[1].vlan_mask & QOS_MGR_VLAN_ID) ?
						(rule2->inner.vid = p->flow_pattern.vlan_match.entry[1].vid) : (rule2->inner.vid = DP_VLAN_PATTERN_NOT_CARE);
					(p->flow_pattern.vlan_match.entry[1].vlan_mask & QOS_MGR_VLAN_PRIO) ?
						(rule2->inner.prio = p->flow_pattern.vlan_match.entry[1].prio) : (rule2->inner.prio = DP_VLAN_PATTERN_NOT_CARE);
					(p->flow_pattern.vlan_match.entry[1].vlan_mask & QOS_MGR_VLAN_PROTO) ?
						(rule2->inner.proto = p->flow_pattern.vlan_match.entry[1].proto) : (rule2->inner.proto = DP_VLAN_PATTERN_NOT_CARE);
					(p->flow_pattern.vlan_match.entry[1].vlan_mask & QOS_MGR_VLAN_TPID) ?
						(rule2->inner.tpid = p->flow_pattern.vlan_match.entry[1].tpid) : (rule2->inner.tpid = DP_VLAN_PATTERN_NOT_CARE);

					rule2->def = isDefRule;
					if (p->flow_action.action.act == QOS_MGR_ACT_FWD)
						rule2->act.act = DP_VLAN_ACT_FWD;

					if (p->flow_action.action.act == QOS_MGR_ACT_DROP)
						rule2->act.act = DP_VLAN_ACT_DROP;

					if ((p->flow_action.action.act & QOS_MGR_VLAN_ACT_POP) == QOS_MGR_VLAN_ACT_POP) {
						rule2->act.act |= DP_VLAN_ACT_POP;
						rule2->act.pop_n++;
					}
					if (((p->flow_action.action.act & QOS_MGR_VLAN_ACT_PUSH) == QOS_MGR_VLAN_ACT_PUSH)) {
						for (i = 0; i < p->flow_action.action.push_n; i++) {
							rule2->act.tpid[i] = p->flow_action.action.tpid[i];
							rule2->act.vid[i] = p->flow_action.action.vid[i];
							rule2->act.prio[i] = p->flow_action.action.prio[i];
							rule2->act.dei[i] = p->flow_action.action.dei[i];
						}
						rule2->act.act |= DP_VLAN_ACT_PUSH;
						rule2->act.push_n = p->flow_action.action.push_n;
					} else if (((p->flow_action.action.act & QOS_MGR_VLAN_ACT_MODIFY)
									== QOS_MGR_VLAN_ACT_MODIFY)) {
						for (i = 0; i < p->flow_action.action.push_n; i++) {
							rule2->act.tpid[i] = p->flow_action.action.tpid[i];
							rule2->act.vid[i] = p->flow_action.action.vid[i];
							rule2->act.prio[i] = p->flow_action.action.prio[i];
							(p->flow_action.action.prio[i]) ?
								(rule2->act.prio[i] =
							p->flow_action.action.prio[i]) : (rule2->act.prio[i] = CP_FROM_OUTER);
							rule2->act.dei[i] = p->flow_action.action.dei[i];
						}
						rule2->act.act = DP_VLAN_ACT_POP | DP_VLAN_ACT_PUSH;
						rule2->act.push_n = p->flow_action.action.push_n;
						/* Increment the POP if modify action is followed by 
						 * pop before (Rule 2.8).
						 */
						rule2->act.pop_n++;
						pr_debug("POP:%d PUSH:%d\n", rule2->act.pop_n, rule2->act.push_n);
					}
					qos_mgr_dscp_pcp_map_update(p, rule2->act.dscp_pcp_map);
					vlan2_index++;
				}
			}
		}
	}
	dp_vlan.vlan0_list = rule0_head;
	dp_vlan.vlan1_list = rule1_head;
	dp_vlan.vlan2_list = rule2_head;
#ifdef QOS_MGR_TC_DBG
	qos_mgr_show_dp_info(&dp_vlan);
#endif
	if (total_rule != 0) {
		if (dp_vlan_set(&dp_vlan, 0) == DP_FAILURE)
			ret = PPA_FAILURE;
	}
	ret = PPA_SUCCESS;

END:
	if (rule0_head) {
		qos_mgr_lct_cleanup(rule0_head, vlan0_index);
		ppa_free(rule0_head);
	}
	if (rule1_head)
		ppa_free(rule1_head);
	if (rule2_head)
		ppa_free(rule2_head);

	return ret;
}

/* Set OMCI PCE Rule */
static void qos_mgr_gsw_alernate_tc_cfg(uint32_t arg)
{
	struct core_ops *gsw_handle;
	GSW_return_t ret;
	GSW_PCE_rule_t pceRule = {0};

	gsw_handle = gsw_get_swcore_ops(0);
	pceRule.pattern.nIndex = 0;
	pceRule.pattern.bEnable = 1;
	pceRule.pattern.bPortIdEnable = 1;
	pceRule.pattern.nPortId = 2;
	pceRule.pattern.bSubIfIdEnable = 1;
	pceRule.pattern.nSubIfId = 0;

	pceRule.action.eTrafficClassAction = 2;
	pceRule.action.nTrafficClassAlternate = 8;

	ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleWrite(gsw_handle, &pceRule);
	if (ret != GSW_statusOk)
		pr_err("%s: TFLOW_PceRuleWrite for CPU High priority rule failed: %d\n", __func__, ret);

	return;
}

void qos_mgr_remove_leading_whitespace(char **p, int *len)
{
	while (*len && ((**p == ' ') || (**p == '\r') || (**p == '\r'))) {
		(*p)++;
		(*len)--;
	}
}

int qos_mgr_split_buffer(char *buffer, char *array[], int max_param_num)
{
	int i, set_copy = 0;
	int res = 0;
	int len;

	for (i = 0; i < max_param_num; i++)
		array[i] = NULL;
	if (!buffer)
		return 0;
	len = strlen(buffer);
	for (i = 0; i < max_param_num;) {
		qos_mgr_remove_leading_whitespace(&buffer, &len);
		for (;
			 *buffer != ' ' && *buffer != '\0' && *buffer != '\r'
			 && *buffer != '\n' && *buffer != '\t'; buffer++, len--) {
			/*Find first valid charactor */
			set_copy = 1;
			if (!array[i])
				array[i] = buffer;
		}

		if (set_copy == 1) {
			i++;
			if (*buffer == '\0' || *buffer == '\r'
				|| *buffer == '\n') {
				*buffer = 0;
				break;
			}
			*buffer = 0;
			buffer++;
			len--;
			set_copy = 0;

		} else {
			if (*buffer == '\0' || *buffer == '\r'
				|| *buffer == '\n')
				break;
			buffer++;
			len--;
		}
	}
	res = i;

	return res;
}
static int proc_read_tc_db_info(struct seq_file *seq, void *v)
{
	int32_t i = 0;
	uint32_t ingress, egress;

	if (!capable(CAP_SYSLOG)) {
		pr_err ("Read Permission denied");
		return 0;
	}

	seq_printf(seq, "\t========================================================================================\n");
	seq_printf(seq, "\t|     Index        |    Device      |  No Of Queue   |     Ingress    |      Egress    |\n");
	seq_printf(seq, "\t----------------------------------------------------------------------------------------\n");

	for (i = 0; i < QOS_MGR_MAX_IFACE; i++) {
		ingress = 0;
		egress = 0;
		if (tc_qos_mgr_db[i].dev != NULL) {
			seq_printf(seq, "\t|%9d       |", i);
			seq_printf(seq, "%9s       |", tc_qos_mgr_db[i].dev->name);
			seq_printf(seq, "%9d       |", tc_qos_mgr_db[i].qos_info.no_of_queue);
			qos_mgr_get_no_of_flow_rules(i, &ingress, &egress);
			seq_printf(seq, "%9d       |", ingress);
			seq_printf(seq, "%9d       |\n", egress);
			seq_printf(seq, "\t----------------------------------------------------------------------------------------\n");
		}
	}
	return 0;
}

static int proc_read_tc_db_info_seq_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_read_tc_db_info, NULL);
}

#define ARG_NUM 5
static ssize_t proc_write_tc_db_info(struct file *file, const char __user *buf, size_t count, loff_t *data)
{

	uint32_t len, num = 0;
	char str[50];
	char *p ;
	char *param_list[ARG_NUM] = { 0 };
	struct net_device *netdev ;
	int32_t dev_index;

	if (!capable(CAP_NET_ADMIN)) {
		pr_err("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)sizeof(str) - 1);
	len -= ppa_copy_from_user(str, buf, len);
	while (len && str[len - 1] <= ' ')
		len--;
	str[len] = 0;
	/*for (p = str; *p && *p <= ' '; p++, len--); */
	for (p = str; *p && *p <= ' '; p++)
		len--;
	if (!*p)
		return count;

	if (strstr(p, "help")) {
		pr_info("echo <netdev> <dir> > /proc/ppa/qos_mgr/tc/tc_db_info\n");
		pr_info("dir: ingress/ig  egress/eg\n");
		return count;

	} else {
		num = qos_mgr_split_buffer(p, param_list, ARG_NUM);
		pr_debug("%s %s\n", param_list[0], param_list[1]);

		netdev = dev_get_by_name(&init_net, param_list[0]);
		if (netdev == NULL) {
			pr_debug("Invalid interface name\n");
			return PPA_FAILURE;
		}
		pr_debug("Net Device: [%s]\n", netdev->name);
		dev_index = qos_mgr_get_dev_node_match_index(netdev);
		if (dev_index != PPA_FAILURE) {
			if ((strncmp(param_list[1], "ingress", 7) == 0) ||
				(strncmp(param_list[1], "ig", 2) == 0))
				qos_mgr_tc_flow_lookup(tc_qos_mgr_db[dev_index].flow_item_ig);
			if ((strncmp(param_list[1], "egress", 6) == 0) ||
				(strncmp(param_list[1], "eg", 2) == 0))
				qos_mgr_tc_flow_lookup(tc_qos_mgr_db[dev_index].flow_item_eg);
		} else
			pr_debug("Netdevice %s is not in TC DB list\n", netdev->name);

		return count;
	}

}

static struct file_operations g_proc_file_tc_db_info_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_tc_db_info_seq_open,
	.read		= seq_read,
	.write		= proc_write_tc_db_info,
	.llseek		= seq_lseek,
	.release	= single_release,
};

int qos_mgr_tc_proc_create(void)
{
	pr_debug("QOS MGR TC Create Proc entries \n");
	if (!g_ppa_proc_dir_flag) {
		g_ppa_proc_dir = proc_mkdir("ppa", NULL);
		g_ppa_proc_dir_flag = 1;
	}
	if (!g_ppa_qos_mgr_proc_dir_flag) {
		g_ppa_qos_mgr_proc_dir = proc_mkdir("qos_mgr", g_ppa_proc_dir);
		g_ppa_qos_mgr_proc_dir_flag = 1;
	}
	g_ppa_qos_mgr_tc_proc_dir = proc_mkdir("tc", g_ppa_qos_mgr_proc_dir);
	g_ppa_qos_mgr_tc_proc_dir_flag = 1;

	proc_create("tc_db_info", 0600,
			g_ppa_qos_mgr_tc_proc_dir,
			&g_proc_file_tc_db_info_seq_fops);

	return 0;
}

void qos_mgr_tc_proc_destroy(void)
{
	remove_proc_entry("tc_db_info",
			g_ppa_qos_mgr_tc_proc_dir);
	if (g_ppa_qos_mgr_tc_proc_dir_flag) {
		remove_proc_entry("tc",
				g_ppa_qos_mgr_proc_dir);
		g_ppa_qos_mgr_tc_proc_dir = NULL;
		g_ppa_qos_mgr_tc_proc_dir_flag = 0;
	}
	if (g_ppa_qos_mgr_proc_dir_flag) {
		remove_proc_entry("qos_mgr",
				g_ppa_proc_dir);
		g_ppa_qos_mgr_proc_dir = NULL;
		g_ppa_qos_mgr_proc_dir_flag = 0;
	}
}

static void qos_mgr_ds_qassign_cfg(uint32_t arg)
{
#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
	int32_t i;
	struct core_ops *gsw_handle;
	GSW_return_t ret;
	GSW_QoS_DSCP_ClassCfg_t dscp2Class = {0};
	GSW_QoS_SVLAN_PCP_ClassCfg_t spcp2Class = {0};
	GSW_QoS_portCfg_t qosPortCfg = {0};

	gsw_handle = gsw_get_swcore_ops(0);
	if (!gsw_handle) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return;
	}

	/* Program DSCP lookup table */
	ret = gsw_handle->gsw_qos_ops.QoS_DSCP_ClassGet(gsw_handle, &dscp2Class);
	if (ret != GSW_statusOk) {
		pr_err("%s: QoS_DSCP_ClassGet failed: %d\n", __func__, ret);
		return;
	}

	for (i = 0; i < 64; i++) {
		dscp2Class.nTrafficClass[i] = i;
	}

	ret = gsw_handle->gsw_qos_ops.QoS_DSCP_ClassSet(gsw_handle, &dscp2Class);
	if (ret != GSW_statusOk) {
		pr_err("%s: QoS_DSCP_ClassSet failed: %d\n", __func__, ret);
		return;
	}

	ret = gsw_handle->gsw_qos_ops.QoS_DSCP_ClassGet(gsw_handle, &dscp2Class);
	if (ret != GSW_statusOk) {
		pr_err("%s: QoS_DSCP_ClassGet failed: %d\n", __func__, ret);
		return;
	}

	pr_debug("==> DSCP to Traffic class: %d %d %d %d %d",
		dscp2Class.nTrafficClass[0], dscp2Class.nTrafficClass[1],
		dscp2Class.nTrafficClass[2], dscp2Class.nTrafficClass[3],
		dscp2Class.nTrafficClass[4]);

	/* Program SPCP lookup table */
	ret = gsw_handle->gsw_qos_ops.QoS_SVLAN_PCP_ClassGet(gsw_handle, &spcp2Class);
	if (ret != GSW_statusOk) {
		pr_err("%s: QoS_SVLAN_PCP_ClassGet failed: %d\n", __func__, ret);
		return;
	}

	for (i = 0; i < 8; i++) {
		spcp2Class.nTrafficClass[i] = i;
	}

	ret = gsw_handle->gsw_qos_ops.QoS_SVLAN_PCP_ClassSet(gsw_handle, &spcp2Class);
	if (ret != GSW_statusOk) {
		pr_err("%s: QoS_SVLAN_PCP_ClassSet failed: %d\n", __func__, ret);
		return;
	}

	pr_debug("==> SPCP to Traffic class: %d %d %d %d %d",
		spcp2Class.nTrafficClass[0], spcp2Class.nTrafficClass[1],
		spcp2Class.nTrafficClass[2], spcp2Class.nTrafficClass[3],
		spcp2Class.nTrafficClass[4]);

	qosPortCfg.nPortId = 2;
	ret = gsw_handle->gsw_qos_ops.QoS_PortCfgGet(gsw_handle, &qosPortCfg);
	if (ret != GSW_statusOk) {
		pr_err("%s: QoS_PortCfgGet failed: %d\n", __func__, ret);
		return;
	}

	qosPortCfg.eClassMode = GSW_QOS_CLASS_SELECT_SPCP_DSCP;
	ret = gsw_handle->gsw_qos_ops.QoS_PortCfgSet(gsw_handle, &qosPortCfg);
	if (ret != GSW_statusOk) {
		pr_err("%s: QoS_PortCfgSet failed: %d\n", __func__, ret);
		return;
	}

	ret = gsw_handle->gsw_qos_ops.QoS_PortCfgGet(gsw_handle, &qosPortCfg);
	if (ret != GSW_statusOk) {
		pr_err("%s: QoS_PortCfgGet failed: %d\n", __func__, ret);
		return;
	}

	pr_debug("Class Mode:%d TrafficClass:%d\n", qosPortCfg.eClassMode, qosPortCfg.nTrafficClass);
#endif
}


int32_t qos_mgr_tc_init(void)
{
	qos_mgr_tc_db_init();
	qos_mgr_hook_setup_tc = qos_mgr_setup_tc;
	qos_mgr_hook_setup_tc_ext = qos_mgr_setup_tc_ext;
	qos_mgr_tc_proc_create();
	qos_mgr_gsw_alernate_tc_cfg(0);
	qos_mgr_ds_qassign_cfg(0);
	return 0;
}

void qos_mgr_tc_exit(void)
{
	qos_mgr_hook_setup_tc = NULL;
	qos_mgr_hook_setup_tc_ext = NULL;
	qos_mgr_tc_proc_destroy();
}
