/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 *
 *****************************************************************************/

#ifndef _PON_QOS_TC_PARSER_
#define _PON_QOS_TC_PARSER_

void pon_qos_tc2pce_subif_parse(struct net_device *dev,
				GSW_PCE_rule_t *pce_rule,
				int ifindex);

void pon_qos_tc2pce_proto_parse(struct net_device *dev,
				const struct tc_cls_flower_offload *f,
				GSW_PCE_rule_t *pce_rule);

void pon_qos_tc2pce_vlan_parse(struct net_device *dev,
			       const struct tc_cls_flower_offload *f,
			       GSW_PCE_rule_t *pce_rule);

void pon_qos_tc2pce_icmp_parse(struct net_device *dev,
			       const struct tc_cls_flower_offload *f,
			       GSW_PCE_rule_t *pce_rule);

bool pon_qos_tc_parse_is_mcc(struct net_device *dev,
			     const struct tc_cls_flower_offload *f);
#endif
