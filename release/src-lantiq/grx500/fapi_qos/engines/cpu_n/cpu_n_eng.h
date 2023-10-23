/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef _SW_HDR_H
#define _SW_HDR_H

#include <libubus.h>
#include <libubox/blobmsg.h>

#define CPU_N_Q_CAP_RED			SET_PARAM(Q_CAP_RED, 1)
#define CPU_N_Q_CAP_WRED		SET_PARAM(Q_CAP_WRED, 0)
#define CPU_N_Q_CAP_DT			SET_PARAM(Q_CAP_DT, 1)
#define CPU_N_Q_CAP_CODEL		SET_PARAM(Q_CAP_CODEL, 1)
#define CPU_N_Q_CAP_SP			SET_PARAM(Q_CAP_SP, 1)
#define CPU_N_Q_CAP_WFQ			SET_PARAM(Q_CAP_WFQ, 1)
#define CPU_N_Q_CAP_WRR			SET_PARAM(Q_CAP_WRR, 0)
#define CPU_N_Q_CAP_LEN			SET_PARAM(Q_CAP_LEN, 1)
#define CPU_N_Q_CAP_SUBQ		SET_PARAM(Q_CAP_SUBQ, 0)
#define CPU_N_Q_CAP_SHAPE		SET_PARAM(Q_CAP_SHAPE, 1)
#define CPU_N_Q_CAP_COL_BLIND		SET_PARAM(Q_CAP_COL_BLIND, 1)
#define CPU_N_Q_CAP_TR_TCM		SET_PARAM(Q_CAP_TR_TCM, 0)
#define CPU_N_Q_CAP_TR_TCM_RFC4115	SET_PARAM(Q_CAP_TR_TCM_RFC4115, 0)
#define CPU_N_Q_CAP_LOOSE_COL_BLIND	SET_PARAM(Q_CAP_LOOSE_COL_BLIND, 0)
#define CPU_N_PORT_CAP_SHAPE		SET_PARAM(PORT_CAP_SHAPE, 1)
#define CPU_N_Q_CAP_ATM_IF		SET_PARAM(Q_CAP_ATM_IF, 1)
#define CPU_N_Q_CAP_PTM_IF		SET_PARAM(Q_CAP_PTM_IF, 1)
#define CPU_N_Q_CAP_ETHL_IF		SET_PARAM(Q_CAP_ETHL_IF, 1)
#define CPU_N_Q_CAP_ETHW_IF		SET_PARAM(Q_CAP_ETHW_IF, 1)
#define CPU_N_Q_CAP_LTE_IF		SET_PARAM(Q_CAP_LTE_IF, 0)
#define CPU_N_Q_CAP_INGRESS		SET_PARAM(Q_CAP_INGRESS, 0)

//Flags can be put for all parameters provided in 181 data model
#define CPU_N_CL_CAP_MAC_ADDR 		SET_PARAM(CL_CAP_MAC_ADDR, 1)
#define CPU_N_CL_CAP_TX_IF		SET_PARAM(CL_CAP_TX_IF, 1)
#define CPU_N_CL_CAP_VLAN_ID		SET_PARAM(CL_CAP_ETH_VLAN_ID, 1)
#define CPU_N_CL_CAP_VLAN_PRIO		SET_PARAM(CL_CAP_ETH_VLAN_PRIO, 1)
#define CPU_N_CL_CAP_VLAN_DEI		SET_PARAM(CL_CAP_ETH_VLAN_DEI, 0)
#define CPU_N_CL_CAP_INNER_VLAN		SET_PARAM(CL_CAP_ETH_INNER_VLAN, 0)
#define CPU_N_CL_CAP_L3_PROTO		SET_PARAM(CL_CAP_L3_PROTO, 1)
#define CPU_N_CL_CAP_IPv4_ADDR		SET_PARAM(CL_CAP_L3_IPv4_ADDR, 1)
#define CPU_N_CL_CAP_IPv6_ADDR		SET_PARAM(CL_CAP_L3_IPv6_ADDR, 1)
#define CPU_N_CL_CAP_IP_DSCP		SET_PARAM(CL_CAP_L3_DSCP, 1)
#define CPU_N_CL_CAP_INNER_IP		SET_PARAM(CL_CAP_L3_INNER_IP, 0)
#define CPU_N_CL_CAP_L4_PROTO_N_PORT	SET_PARAM(CL_CAP_L4_PROTO_N_PORT, 1)
#define CPU_N_CL_CAP_L7_PROTO		SET_PARAM(CL_CAP_L7_PROTO, 1)
#define CPU_N_CL_CAP_DHCP_VEND_EXT	SET_PARAM(CL_CAP_DHCP_VEND_EXT, 0)
#define CPU_N_CL_CAP_PKTLEN		SET_PARAM(CL_CAP_L3_PKTLEN, 1)
#define CPU_N_CL_CAP_TCP_FL		SET_PARAM(CL_CAP_L4_TCP_FL, 1)
#define CPU_N_CL_CAP_EXCL		SET_PARAM(CL_CAP_EXCL, 1)
#define CPU_N_CL_CAP_ACT_TC		SET_PARAM(CL_CAP_ACT_TC, 1)
#define CPU_N_CL_CAP_ACT_DROP		SET_PARAM(CL_CAP_ACT_DROP, 0)
#define CPU_N_CL_CAP_ACT_DSCP_REMARK	SET_PARAM(CL_CAP_ACT_DSCP_REMARK, 1)
#define CPU_N_CL_CAP_ACT_VLAN_REMARK	SET_PARAM(CL_CAP_ACT_VLAN_REMARK, 1)
#define CPU_N_CL_CAP_ACT_FWD		SET_PARAM(CL_CAP_ACT_FWD, 0)
#define CPU_N_CL_CAP_ACT_POLICE		SET_PARAM(CL_CAP_ACT_POLICE, 1)
#define CPU_N_CL_CAP_ACT_ACCL           SET_PARAM(CL_CAP_ACT_ACCL, 1)
#define CPU_N_CL_CAP_ACT_APP		SET_PARAM(CL_CAP_ACT_APP, 0)
#define CPU_N_CL_CAP_INS		SET_PARAM(CL_CAP_INS, 0)
#define CPU_N_CL_CAP_L2IF		SET_PARAM(CL_CAP_L2IF, 1)
#define CPU_N_CL_CAP_ETHL_IF		SET_PARAM(CL_CAP_ETHL_IF, 1)
#define CPU_N_CL_CAP_ATM_IF		SET_PARAM(CL_CAP_ATM_IF, 1)
#define CPU_N_CL_CAP_PTM_IF		SET_PARAM(CL_CAP_PTM_IF, 1)
#define CPU_N_CL_CAP_ETHW_IF		SET_PARAM(CL_CAP_ETHW_IF, 1)
#define CPU_N_CL_CAP_LTE_IF		SET_PARAM(CL_CAP_LTE_IF, 0)
#define CPU_N_CL_CAP_MPTCP		SET_PARAM(CL_CAP_MPTCP, 1)
#define CPU_N_CL_CAP_ETH_TYPE   	SET_PARAM(CL_CAP_ETH_TYPE, 1)

#define CPU_N_CL_CAP_ACT_INGRS_SW       SET_PARAM(CL_CAP_ACT_INGRS_SW, 1)
#define CPU_N_CL_CAP_ACT_INGRS_HW       SET_PARAM(CL_CAP_ACT_INGRS_HW, 1)

#define CPU_N_Q_CAP (CPU_N_Q_CAP_RED) | (CPU_N_Q_CAP_WRED) | (CPU_N_Q_CAP_DT) | \
	(CPU_N_Q_CAP_CODEL) | (CPU_N_Q_CAP_SP) | (CPU_N_Q_CAP_WFQ) | \
	(CPU_N_Q_CAP_WRR) | (CPU_N_Q_CAP_LEN) | (CPU_N_Q_CAP_SUBQ) | \
	(CPU_N_Q_CAP_SHAPE) | (CPU_N_Q_CAP_COL_BLIND) | (CPU_N_Q_CAP_TR_TCM) | \
	(CPU_N_Q_CAP_TR_TCM_RFC4115) | (CPU_N_Q_CAP_LOOSE_COL_BLIND) | (CPU_N_PORT_CAP_SHAPE) | \
	(CPU_N_Q_CAP_ATM_IF) | (CPU_N_Q_CAP_PTM_IF) | (CPU_N_Q_CAP_ETHL_IF) | (CPU_N_Q_CAP_ETHW_IF) | \
	(CPU_N_Q_CAP_LTE_IF) | (CPU_N_Q_CAP_INGRESS)

#define CPU_N_CL_CAP (CPU_N_CL_CAP_MAC_ADDR) | (CPU_N_CL_CAP_VLAN_PRIO) | \
	(CPU_N_CL_CAP_VLAN_ID) | (CPU_N_CL_CAP_VLAN_DEI) | (CPU_N_CL_CAP_INNER_VLAN) | \
	(CPU_N_CL_CAP_IPv4_ADDR) | (CPU_N_CL_CAP_IPv6_ADDR) | (CPU_N_CL_CAP_L3_PROTO) | \
	(CPU_N_CL_CAP_INNER_IP) | (CPU_N_CL_CAP_PKTLEN) | (CPU_N_CL_CAP_IP_DSCP) | \
	(CPU_N_CL_CAP_L4_PROTO_N_PORT) | (CPU_N_CL_CAP_TCP_FL) | (CPU_N_CL_CAP_L7_PROTO) | \
	(CPU_N_CL_CAP_DHCP_VEND_EXT) | (CPU_N_CL_CAP_EXCL) | \
	(CPU_N_CL_CAP_ACT_TC) |  (CPU_N_CL_CAP_ACT_DROP) | (CPU_N_CL_CAP_ACT_DSCP_REMARK) | \
	(CPU_N_CL_CAP_ACT_VLAN_REMARK) | (CPU_N_CL_CAP_ACT_FWD) | (CPU_N_CL_CAP_ACT_POLICE) | \
	(CPU_N_CL_CAP_ACT_APP) | (CPU_N_CL_CAP_INS) | (CPU_N_CL_CAP_L2IF) | \
	(CPU_N_CL_CAP_ETHL_IF) | (CPU_N_CL_CAP_ATM_IF) | (CPU_N_CL_CAP_PTM_IF) | \
	(CPU_N_CL_CAP_ETHW_IF) | (CPU_N_CL_CAP_LTE_IF) | (CPU_N_CL_CAP_TX_IF)

#define CPU_N_CL_EXT_CAP (CPU_N_CL_CAP_TX_IF) | (CPU_N_CL_CAP_ACT_ACCL) | (CPU_N_CL_CAP_MPTCP) | (CPU_N_CL_CAP_ETH_TYPE) | (CPU_N_CL_CAP_ACT_INGRS_SW) | (CPU_N_CL_CAP_ACT_INGRS_HW)

/* Set the 31 bit position for the mark */
#define PPA_BIT_ENABLE(mark) { \
		mark |= (1 << PPA_START_BIT_POS); \
}

/* bit position of various fields in skb->mark. */
#define PPA_START_BIT_POS	30
#define VLANID_START_BIT_POS	17
#define VPRIO_START_BIT_POS	14
#define ACCEL_DIS_BIT_POS	13
#define SUBQUEUE_START_BIT_POS	10
#define DEF_US_Q_PRIO		8

#define INGRESS_EGRESS_QOS_BIT 		17
#define INGRESS_SW_HW_BIT       	18
#define INGRESS_TC_BIT          	23
#define INGRESS_GROUP_BIT       	21
#define INGRESS_EGRESS_TC_SET   	19
#define INGRESS_MASK            	0X0FFC0000
#define INGRESS_TC_MASK         	0X0F800000
#define INGRESS_DSQOS_BIT		28
#define DS_QOS_MPE_MARK            	0X10000000
#define VLAN_PRIO_BIT_MASK         	0X1FFE0000

#define QUEPRIO_START_BIT_POS	5

#define DEF_PRIO_START_BIT_POS	5
#define DEF_DS_Q_PRIO		8
#define TC_START_BIT_POS	0

#define TC_MASK			0x1f
#define CLASSIFICATION_NOT_DONE_MASK			0x10

#define QUEPRIO_MASK 		0x3e0

/*Queue type bit in extmark is used only in 750
  for SP/WFQ profile Identifier*/
#define QUETYPE_START_BIT_POS	5
#define QUETYPE_MASK		0x20

#define Q_PRIO_TC_MASK		0x3ff
#define Q_PRIO_TC_DEF_DSCP_MASK		0x3ef

#define CLASSIFICATION_BIT_MASK ((1 << PPA_START_BIT_POS) | VLAN_PRIO_BIT_MASK |\
		(1 << ACCEL_DIS_BIT_POS) | Q_PRIO_TC_MASK)

#define DEF_US_MARK ((DEF_US_Q_PRIO - DEFAULT_TC + 1) << TC_START_BIT_POS)
#define DEF_DS_MARK ((DEF_DS_Q_PRIO - DEFAULT_TC + 1) << TC_START_BIT_POS)

#define SMALLER_OF(a,b) ((a)<(b) ?(a) :(b))
#define CPU_N_MAX_POLICE_IF 2
#define CPU_N_MAX_Q 16
#define CPU_N_MAX_Q_LEN 1234
#define CPU_N_MAX_CL 24
#define QQ_AVPKT 1500
#define DEFAULT_RED_THRESHOLD 3000
#define DEFAULT_RED_PROBABILITY 0.2
#define CALCULATE_THRESOLD_RED(qmin_th,limit, qmax_th,qburst) \
{ \
	if ( qmin_th == 0 ) \
		qmin_th = DEFAULT_RED_THRESHOLD; \
	if ( qmax_th == 0 ) \
		qmax_th = SMALLER_OF(qmin_th * 3, limit); \
	qburst  = (qmin_th << 2); \
	qburst  = (qburst + qmax_th); \
	qburst  = (qburst / (QQ_AVPKT * 3)); \
} 

/*#define IPT "iptables"*/
#define IPV4T "iptables"
#define IPV6T "ip6tables"
#define EBT "ebtables"
#ifdef PLATFORM_LGM
#define MARK "MARK"
#define mark "mark"
#else
#define MARK "EXTMARK"
#define mark "extmark"
#endif
#define SKB_MARK "MARK"

#define CPU_N_INTERFACE_LAN "eth0+"
#define CPU_N_INTERFACE_LAN_BRIDGE "br+"
#define CPU_N_INTERFACE_WAN "ppp+"
#define CPU_N_INTERFACE_WAN_ETH "eth1+"
#define CPU_N_INTERFACE_WAN_ATM "nas+"
#define CPU_N_INTERFACE_WAN_PTM "ptm+"

#define MAX_QUEUES_IN_US 8
#define MAX_QUEUES_IN_DS 4

#define IPT_QOS_TBL_NAME "mangle"
#define EBT_QOS_TBL_NAME "filter"
#define IPT_QOS_STD_HOOK "PREROUTING"
#define EBT_QOS_STD_HOOK "FORWARD"

#define LAN_CL		"LAN_CL"
#define WAN_CL		"WAN_CL"
#define CL_LIST		"CL_LIST"
#define DEF_LAN_CL	"LAN_default"
#define DEF_WAN_CL	"WAN_default"
#define DEF_WAN_PRERT	"DEF_WAN_CL"
#define DEF_WAN_POSTRT	"DEF_WAN_PRIO"
#define OUTPUT		"OUTPUT"
#define DEF_INGRESS_CL	"WAN_Ing_default"

#define FIREWALLD_UBUS_METHOD_NOTIFY_QOS_INIT "notify_qos_init"
#define FIREWALLD_UBUS_METHOD_NOTIFY_QOS_MODIFY "notify_qos_modify"
typedef enum {
	CH_READ, /* read chain. */
	CH_FL, /* flush chain. */
	CH_REM, /* remove chain. */
	CH_CREAT, /* create chain. */
	RULE_DEL, /* delete rule. */
	RULE_INS, /* insert rule. */
	RULE_APP /* append rule. */
} iptebt_ops_t;

typedef enum {
	CL_ADAPT_IPTEBT, /* iptables/ebtables adapter. */
	CL_ADAPT_TC /* tc adapter. */
} cl_adapt_t;

typedef enum {
	IPT_QOS_TBL,
	EBT_QOS_TBL
} ipt_tbl_t;

typedef enum {
	QUEUEING_MODE_NONE,
	QUEUEING_MODE_SP,
	QUEUEING_MODE_WFQ,
	QUEUEING_MODE_SP_WFQ,
	QUEUEING_MODE_WFQ_SP,
	QUEUEING_MODE_SP_WFQ_SP
} QueueMode;

typedef struct qdisc_tree {
	uint32_t num_branches;
	uint32_t total_cir;
	qos_shaper_t *port_shaper;
	qos_queue_cfg_t *q_cfg[CPU_N_MAX_Q];
	uint32_t prio[CPU_N_MAX_Q]; /* used to prio of non-leaf classes */
} qdisc_tree_t;

typedef struct q_cfg_info{
	uint32_t uiNoOfQueues;
	qos_queue_cfg_t q_cfg[CPU_N_MAX_Q];
	qos_shaper_t *port_shaper;
}q_cfg_info_t;

typedef struct if_police_cfg {
	char ifname[MAX_IF_NAME_LEN];
	uint32_t count;
}if_police_cfg_t;

int32_t cpu_n_eng_init(qos_module_type_t mtype);
int32_t cpu_n_eng_fini(qos_module_type_t mtype);
int32_t cpu_n_eng_queue_add (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t* pxq_cfg);
int32_t cpu_n_eng_queue_delete (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t* pxq_cfg);
int32_t cpu_n_eng_queue_modify (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t* pxq_cfg);
int32_t cpu_n_eng_stats_get (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, char *qname, qos_queue_stats_t* pxq_stats);
int32_t cpu_classAdd(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags);
int32_t cpu_classModify(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t old_order, qos_class_cfg_t *old_clcfg, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags);
int32_t cpu_classDel(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags);
int32_t cpu_n_eng_port_set (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_shaper_t* shaper, uint32_t priority, uint32_t weight, uint32_t flag);
extern int32_t util_get_ipaddr_family(const char *ipaddr);
int32_t iptebtAdapt(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags);
int32_t tcAdapt(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags);

int32_t set_ebt_ipt_cmd_wrap(ipt_tbl_t tbl, iptebt_ops_t op, char *chName, int32_t order, char *cl_act, struct blob_buf *b, uint32_t rule_idx);
int32_t set_ebt_ipt_cmd(char *tbl, char *act, char *clname, char *cmd, struct blob_buf *b, uint32_t rule_idx);

int32_t send_iptable_msg(struct blob_buf b, char *ubus_header);
#endif
