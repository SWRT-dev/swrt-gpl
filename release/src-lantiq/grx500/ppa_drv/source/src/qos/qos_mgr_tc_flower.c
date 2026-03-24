/*************************************************************************
 **
 ** FILE NAME    : qos_mgr_tc_flower.c
 ** PROJECT      : QOS MGR
 ** MODULES      : QOS MGR (TC APIs)
 **
 ** DESCRIPTION  : TC Flower Offload Implementation
 ** COPYRIGHT 	 : Copyright (c) 2019 - 2020 Intel Corporation
 **
 *************************************************************************/
#include "qos_mgr_tc.h"
#include "qos_mgr_tc_mirred.h"

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

	if (!tc_fl_item) {
		if (in_dev)
			dev_put(in_dev);
		return QOS_MGR_FAILURE;
	}

	tc_fl_item->dir = ingress;
	tc_fl_item->priority = cls_flower->common.prio >> 16;
	tc_fl_item->proto = cls_flower->common.protocol;
	tc_fl_item->parentid = f->classid;
	tc_fl_item->indev = in_dev;

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

	tc_fl_item->flow_pattern.match_to = QOS_MGR_MATCH_UNKNOWN;
	tc_fl_item->flow_pattern.used_keys = f->dissector->used_keys;

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
		tc_fl_item->flow_pattern.match_to = QOS_MGR_MATCH_PROTO;
		if (mask->n_proto == 0xffff)
			tc_flower_store_vlan_proto_match(0, key, mask, tc_fl_item);
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

		tc_flower_store_vlan_match(0, key, mask, tc_fl_item);
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
		tc_flower_store_vlan_match(1, cvlan_key, cvlan_mask, tc_fl_item);
	}
	tc_fl_item->status = 0;

	if (tc_no_actions(f->exts)) {
		pr_debug("TC No Action\n");
		return -EINVAL;
	} else {
		pr_debug("No Of Action:%d\n", (f->exts)->nr_actions);
	}
	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list) {
		if (tc_fl_item != NULL) {
			/* Handle cookie from action */
			tc_fl_item->flow_action.cookie_prio = 0;
			if (a->act_cookie) {
				uint8_t *p = a->act_cookie->data;
				int i;
				/* Save first two words (64 bits) from cookie
				 * data as cookie priority
				 */
				for (i = 7; i > -1; i--) {
					uint64_t cookie_byte = *p;
					tc_fl_item->flow_action.cookie_prio |=
						(cookie_byte << i * 8);
					p++;
				}
			}
		}
		if (is_tcf_gact_shot(a)) {
			pr_debug("GACT SHOT\n");
			tc_fl_item->flow_action.act_type = TC_ACT_SHOT;
			tc_fl_item->flow_action.action.act = QOS_MGR_ACT_DROP;
			continue;
		} else if (((struct tcf_gact *)a)->tcf_action == TC_ACT_OK) {
			pr_debug("ACTION OK\n");
			tc_fl_item->flow_action.action.act = QOS_MGR_ACT_FWD;
#if IS_ENABLED(CONFIG_NET_SCH_MQPRIO)
			tc_fl_item->flow_action.act_type = TC_ACT_OK;
			if (f->command == TC_CLSFLOWER_REPLACE) {
				if (tc_fl_item->parentid) {
					/* vlanprio will be set to (0 .. 7) for downstream p-bit
					 * priotarization otherwise -1 (means the priority will be used
					 * as traffic class). This traffic class will be used for
					 * queue map configuration.
					 */
					if (qos_mgr_add_queue(dev, tc_fl_item->indev,
								tc_fl_item->parentid,
								vlanprio) == QOS_MGR_FAILURE) {
						pr_debug("<%s> Queue Add Failed\n", __func__);
						return QOS_MGR_FAILURE;
					}
					tc_fl_item->flow_pattern.match_to = QOS_MGR_MATCH_UNKNOWN;
				}
			} else if (f->command == TC_CLSFLOWER_DESTROY)
				qos_mgr_del_queue(dev, NULL, f->classid);
#endif
			continue;
		} else if (((struct tcf_gact *)a)->tcf_action == TC_ACT_UNSPEC) {
			pr_debug("ACTION UNSPEC\n");
			tc_fl_item->flow_action.act_type = TC_ACT_UNSPEC;
			tc_fl_item->status = 1;
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

			flags = DP_COL_MARKING;
			if (tc_fl_item->indev != NULL) {
				if (!strncmp(dev->name, tc_fl_item->indev->name, strlen(dev->name)))
					flags |= DP_METER_ATTACH_BRPORT;
			} else
				flags |= DP_METER_ATTACH_CTP;

			tc_fl_item->meter_id = qos_mgr_cfg_meter_to_dp(dev, &meter_cfg, flags);
			if (tc_fl_item->meter_id == QOS_MGR_FAILURE)
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
	return QOS_MGR_SUCCESS;
}

static int32_t get_clsact_ingress_egress(u32 classid, bool *ingress)
{
	/* This also returns true for ingress qdisc */
	if (TC_H_MAJ(classid) == TC_H_MAJ(TC_H_CLSACT) &&
			TC_H_MIN(classid) != TC_H_MIN(TC_H_MIN_EGRESS))
		*ingress = QOS_MGR_TC_INGRESS;
	else if (TC_H_MAJ(classid) == TC_H_MAJ(TC_H_CLSACT) &&
			TC_H_MIN(classid) == TC_H_MIN(TC_H_MIN_EGRESS))
		*ingress = QOS_MGR_TC_EGRESS;
	return QOS_MGR_SUCCESS;
}

/** This function is merging DSCP rule into existing one. */
static int32_t qos_mgr_tc_flower_merger_dscp(
	struct qos_mgr_tc_flow_rule_list_item *p_old,
	struct qos_mgr_tc_flow_rule_list_item *p_new)
{
	struct qos_mgr_act_vlan *vold, *vnew;

	if (p_old->indev != p_new->indev)
		return QOS_MGR_FAILURE;

	if (p_old->dir != p_new->dir)
		return QOS_MGR_FAILURE;

	if (p_old->proto != p_new->proto)
		return QOS_MGR_FAILURE;

	vold = &p_old->flow_action.action;
	vnew = &p_new->flow_action.action;

	if (vold->act != vnew->act)
		return QOS_MGR_FAILURE;

	if ((vold->act & QOS_MGR_VLAN_ACT_POP) &&
			vold->pop_n != vnew->pop_n)
		return QOS_MGR_FAILURE;

	if ((vold->act & QOS_MGR_VLAN_ACT_PUSH) &&
			vold->push_n != vnew->push_n &&
			vold->vid[vold->push_n] != vnew->vid[vnew->push_n] &&
			vold->tpid[vold->push_n] != vnew->tpid[vnew->push_n])
		return QOS_MGR_FAILURE;

	p_old->dscp.map[p_new->dscp.value] = p_new->dscp.map[p_new->dscp.value];

	return QOS_MGR_SUCCESS;
}

/** This function is merging chosen rules into existing ones. */
static int32_t qos_mgr_tc_flower_merger(
	struct qos_mgr_tc_flow_rule_list_item *p_new,
	struct qos_mgr_tc_flow_rule_list_item *obj_head)
{
	struct qos_mgr_tc_flow_rule_list_item *p_old;
	int32_t ret = QOS_MGR_FAILURE;

	if (!p_new)
		return ret;

	if (!p_new->dscp.enable)
		return ret;

	qos_mgr_tc_lock_list();
	for (p_old = obj_head; p_old; p_old = p_old->next) {
		if (qos_mgr_tc_flower_merger_dscp(p_old, p_new) != QOS_MGR_SUCCESS)
			continue;

		qos_mgr_tc_free_list_item(p_new);
		ret = QOS_MGR_SUCCESS;
		break;
	}
	qos_mgr_tc_unlock_list();

	return ret;
}

static int32_t tc_add_item_and_send_to_dp(struct net_device *dev, uint32_t dir,
			   struct qos_mgr_tc_flow_rule_list_item **fl_list,
			   struct qos_mgr_tc_flow_rule_list_item *tc_fl_item,
			   uint32_t prio)
{
	int32_t ret = QOS_MGR_SUCCESS;

	/* Attempt to merge a rule into a current rule in the list
	 * This is needed to merge DSCP filters into one rule */
	ret = qos_mgr_tc_flower_merger(tc_fl_item, *fl_list);
	if (ret != QOS_MGR_SUCCESS)
		/* Add a new list item only if we failed to merge
		 * This will happen when a first DSCP filter comes or when
		 * a non DSCP rule is added */
		__qos_mgr_tc_add_list_item(tc_fl_item, fl_list);

	/* Call Ext. Vlan only if we have valid parent id for Ingress */
	ret = qos_mgr_tc_flow_find_parentid(prio, *fl_list, NULL, NULL);
	if (ret != QOS_MGR_SUCCESS) {
		pr_debug("No Valid parent id for Ingress so calling ext vlan\n");
		ret = qos_mgr_set_ext_vlan_to_dp(dev, dir, *fl_list);
		if (ret != QOS_MGR_SUCCESS)
			pr_debug("qos_mgr_set_ext_vlan_to_dp - failure\n");
	}

	return ret;
}

static int32_t qos_mgr_setup_tc_clsflower_replace_common(
	struct net_device *dev,
	uint32_t ingress,
	struct tc_cls_flower_offload *f)
{
	uint32_t prio;
	struct qos_mgr_tc_flow_rule_list_item *tc_fl_item = NULL;
	struct qos_mgr_tc_flow_rule_list_item *tmp_fl_item = NULL;
	int32_t ret;
	struct _tc_qos_mgr_db *db_handle = NULL;

	prio = f->common.prio >> 16;

	ret = qos_mgr_get_db_handle(dev, 1, &db_handle);
	if (ret == QOS_MGR_FAILURE) {
		pr_debug("No match found\n");
		return QOS_MGR_FAILURE;
	}
	db_handle->dev = dev;
	tc_fl_item = tc_qos_mgr_alloc_list_item();
	if (!tc_fl_item) {
		pr_err("Obj error\n");
		qos_mgr_free_db_handle(db_handle);
		return QOS_MGR_FAILURE;
	}

	if (qos_mgr_tc_flower_parse(dev, f, ingress, tc_fl_item)
			== QOS_MGR_FAILURE) {
		pr_debug("<%s><%d> TC Flower parsing failed\n",
				__func__, __LINE__);
		qos_mgr_tc_free_list_item(tc_fl_item);
		qos_mgr_free_db_handle(db_handle);
		return QOS_MGR_FAILURE;
	}

	if (ingress == 0)
		ret = tc_add_item_and_send_to_dp(dev,
				DP_DIR_INGRESS,
				&db_handle->flow_item_ig,
				tc_fl_item,
				prio);
	else
		ret = tc_add_item_and_send_to_dp(dev,
				DP_DIR_EGRESS,
				&db_handle->flow_item_eg,
				tc_fl_item,
				prio);
	if (ret != QOS_MGR_SUCCESS) {
		pr_debug("tc_add_item_and_send_to_dp - failure\n");
		/* adding flow rule in the DP failed, retrieve the rule from
		 * the list DB and free it
		 */
		if (ingress == 0)
			tmp_fl_item = tc_qos_mgr_retrieve_list_item(prio,
					&(db_handle->flow_item_ig));
		else
			tmp_fl_item = tc_qos_mgr_retrieve_list_item(prio,
					&(db_handle->flow_item_eg));
		qos_mgr_tc_list_item_put(tmp_fl_item);
		qos_mgr_free_db_handle(db_handle);
		return ret;
	}

	return ret;
}

static int32_t qos_mgr_setup_tc_clsflower_destroy_common(
	struct net_device *dev,
	uint32_t ingress,
	struct tc_cls_flower_offload *f)
{
	uint32_t prio, pid = 0;
	uint32_t ext_set = 0;
	struct net_device *indev;
	struct qos_mgr_tc_flow_rule_list_item *tmp_fl_item = NULL;
	int32_t ret;
	struct _tc_qos_mgr_db *db_handle = NULL;

	ret = qos_mgr_get_db_handle(dev, 1, &db_handle);
	if (ret == QOS_MGR_FAILURE)
		return QOS_MGR_FAILURE;

	prio = f->common.prio >> 16;
	if (ingress == 0) {
		if (qos_mgr_tc_flow_find_parentid(prio,
					(db_handle->flow_item_ig),
					&indev,
					&pid) == QOS_MGR_SUCCESS) {
			if (qos_mgr_del_queue(dev, indev, pid) != QOS_MGR_SUCCESS) {
				pr_debug("<%s><%d> Queue deletion failed for Ingress\n",
						__func__, __LINE__);
				return QOS_MGR_FAILURE;
			}
			ext_set = 1;
		}
		/* retrieve the flow rule for this prio */
		if (db_handle->flow_item_ig != NULL)
			tmp_fl_item = tc_qos_mgr_retrieve_list_item(prio,
					&(db_handle->flow_item_ig));

		if (!tmp_fl_item) {
			pr_debug("no matching rule for prio %d\n", prio);
			return QOS_MGR_FAILURE;
		}
		if (ext_set == 0) {
			ret = qos_mgr_set_ext_vlan_to_dp(dev,
					DP_DIR_INGRESS,
					db_handle->flow_item_ig);
			if (ret != QOS_MGR_SUCCESS) {
				/* deleting flow rule in DP Failed, add back
				 * the rule to list DB
				 */
				pr_debug("qos_mgr_set_ext_vlan_to_dp - failure\n");
				__qos_mgr_tc_add_list_item(tmp_fl_item,
						&(db_handle->flow_item_ig));
				qos_mgr_free_db_handle(db_handle);
				return ret;
			}
		}
		/* both deleting queue and rule in DP passed
		 * delete the rule from the list DB
		 */
		qos_mgr_tc_list_item_put(tmp_fl_item);
		qos_mgr_free_db_handle(db_handle);
	} else {
		if (qos_mgr_tc_flow_find_parentid(prio,
					(db_handle->flow_item_eg),
					&indev,
					&pid) == QOS_MGR_SUCCESS) {
			if (qos_mgr_del_queue(dev, indev, pid) != QOS_MGR_SUCCESS) {
				pr_debug("<%s><%d> Queue deletion failed for Egress\n",
						__func__, __LINE__);
				return QOS_MGR_FAILURE;
			}
			ext_set = 1;
		}
		/* retrieve the flow rule for this prio */
		if (db_handle->flow_item_eg != NULL)
			tmp_fl_item = tc_qos_mgr_retrieve_list_item(prio,
					&(db_handle->flow_item_eg));
		if (!tmp_fl_item) {
			pr_debug("no matching rule for prio %d\n", prio);
			return QOS_MGR_FAILURE;
		}
		if (ext_set == 0) {
			ret = qos_mgr_set_ext_vlan_to_dp(dev,
					DP_DIR_EGRESS,
					db_handle->flow_item_eg);
			if (ret != QOS_MGR_SUCCESS) {
				/* deleting flow rule in DP Failed, add back
				 * the rule to list DB
				 */
				pr_debug("qos_mgr_set_ext_vlan_to_dp-failure\n");
				__qos_mgr_tc_add_list_item(tmp_fl_item,
						&(db_handle->flow_item_eg));
				qos_mgr_free_db_handle(db_handle);
				return ret;
			}
		}
		/* both deleting queue and rule in DP passed,
		 * delete the rule from the list DB
		 */
		qos_mgr_tc_list_item_put(tmp_fl_item);
		qos_mgr_free_db_handle(db_handle);
	}

	return QOS_MGR_SUCCESS;
}

/* Adds a flow rule to the list and sends the list to dp */
enum tc_type {
	TC_TYPE_COMMON = 0,
	TC_TYPE_MIRRED = 1,
};

static int32_t qos_mgr_tc_type_specifier(struct tc_cls_flower_offload *f,
					 enum tc_type *type)
{
	const struct tc_action *a;
	uint8_t mirr_cnt = 0, drop_cnt = 0;
	LIST_HEAD(actions);

	*type = TC_TYPE_COMMON;

	if (tc_no_actions(f->exts))
		return QOS_MGR_SUCCESS;

	pr_debug("TC: actions count:%d\n", (f->exts)->nr_actions);

	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list) {
		if (is_tcf_mirred_redirect(a))
			mirr_cnt++;
		if (is_tcf_gact_shot(a))
			drop_cnt++;
	}

	if (mirr_cnt) {
		if ((f->exts)->nr_actions != (mirr_cnt + drop_cnt)) {
			pr_err("TC: combining mirred action with non-drop actions is not supported!\n");
			return QOS_MGR_FAILURE;
		}
		*type = TC_TYPE_MIRRED;
		return QOS_MGR_SUCCESS;
	}

	*type = TC_TYPE_COMMON;
	return QOS_MGR_SUCCESS;
}

int32_t __qos_mgr_tc_clsflower_offload(
			struct net_device *dev,
			uint32_t handle,
			uint32_t dir,
			struct tc_cls_flower_offload *cls_flower)
{
	int32_t ret = QOS_MGR_SUCCESS;

	switch (cls_flower->command) {
	enum tc_type type;
	case TC_CLSFLOWER_REPLACE:
	{
		pr_debug("TC_CLSFLOWER_REPLACE\n");
		ret = qos_mgr_tc_type_specifier(cls_flower,
				&type);
		if (ret != QOS_MGR_SUCCESS)
			return ret;

		switch (type) {
		case TC_TYPE_COMMON:
			ret = qos_mgr_setup_tc_clsflower_replace_common(
					dev, dir, cls_flower);
			break;
		case TC_TYPE_MIRRED:
			ret = qos_mgr_mirred_offload(dev,
					cls_flower, handle);
			break;
		}
		break;
	}
	case TC_CLSFLOWER_DESTROY:
	{
		pr_debug("TC_CLSFLOWER_DESTROY\n");
		ret = qos_mgr_mirred_unoffload(dev,
				cls_flower, handle);
		if (ret == QOS_MGR_SUCCESS)
			break;
		ret = qos_mgr_setup_tc_clsflower_destroy_common(dev,
				dir, cls_flower);
		break;
	}
	case TC_CLSFLOWER_STATS:
	{
		pr_debug("TC_CLSFLOWER_STATS\n");
		break;
	}
	default:
	return -EINVAL;
	}

	return ret;
}

int32_t qos_mgr_tc_clsflower_offload(struct net_device *dev, u32 handle,
				struct tc_cls_flower_offload *cls_flower)
{
	bool ingress;
	get_clsact_ingress_egress(
			cls_flower->common.classid,
			&ingress);
	return __qos_mgr_tc_clsflower_offload(dev, handle, ingress, cls_flower);
}
