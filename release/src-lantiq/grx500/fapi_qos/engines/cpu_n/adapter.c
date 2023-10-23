/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>

#include "qosal_inc.h"
#include "cpu_n_eng.h"
#include "qosal_utils.h"
#include "qosal_engine.h"

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

typedef enum {
	CLI_PROTO_ICMP = 1,
	CLI_PROTO_TCP,
	CLI_PROTO_UDP,
	CLI_PROTO_IGMP,
	CLI_PROTO_ALL,
} classcfg_cli_protocol;

name_value_t axprototype[]={
	{"icmp",CLI_PROTO_ICMP},
	{"tcp",CLI_PROTO_TCP},
	{"udp",CLI_PROTO_UDP},
	{"udp",CLI_PROTO_IGMP},
	{"all",CLI_PROTO_ALL},
	{"\0",0}
};

extern char *IPT;
if_police_cfg_t police_cfg[CPU_N_MAX_POLICE_IF];
int32_t iptAdapt(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags);
int32_t ebtAdaptPart(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags);
int32_t cpu_ebtAdapt(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags);

extern int32_t vlanPrioUpdate(char *sIface, int32_t inPrio, int32_t out8021p, uint32_t flags);
extern int32_t nMapInUse, nDefVlanPrioCnt;
extern vlanMap xVlanMap[DEFAULT_TC];

#define HA_MARK_VALUE	0x80000
#define HA_MARK_MASK	0x80000
#define HA_CL       	"PHA"

/* =============================================================================
 *  Function Name : iptebtAdapt
 *  Description   : wrapper API that abstracts iptables/ebtables command/API.
 * ============================================================================*/
int32_t iptebtAdapt(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags)
{
	int32_t iRet = LTQ_SUCCESS;

	ENTRY
	if(CL_VID_SEL(clcfg) || CL_VPRIO_SEL(clcfg) || CL_DMAC_SEL(clcfg) || (CL_ACT(clcfg) & CL_ACTION_ACCELERATION) || (CL_ETH_SEL(clcfg))) { /* || CL_RXIF_SEL(clcfg)) { */
		iRet = ebtAdaptPart(iftype, order, clcfg, flags);
		if(iRet != LTQ_SUCCESS) {
			iRet = QOS_CPU_ENG_CFG_FAIL;
			LOGF_LOG_ERROR("Failed to configure ebtables rule for VLAN header fields.\n");
			goto lbl;
		}
	}
	iRet = iptAdapt(iftype, order, clcfg, flags);
	if(iRet != LTQ_SUCCESS) {
		iRet = QOS_CPU_ENG_CFG_FAIL;
		LOGF_LOG_ERROR("Failed to configure iptables rule for requested fields.\n");
		goto lbl;
	}

lbl:
	EXIT
	return iRet;
}

int32_t tcAdapt(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags)
{
	uint32_t uiLength = 0, uiLengthTmp = 0, i = 0, proto = 0, pbs = 102400;
	uint32_t retval = 0;
	int32_t ret = LTQ_SUCCESS;
	char cmd_buf[MAX_DATA_LEN] = {0};
	char *op = NULL;
	if_police_cfg_t *policer = NULL;

	ENTRY

	UNUSED(iftype);

	for (i=0; i<CPU_N_MAX_POLICE_IF; i++) {
		if ((strnlen_s(police_cfg[i].ifname, MAX_IF_NAME_LEN) == 0) || (strcmp(police_cfg[i].ifname, CL_RXIF(clcfg)) == 0 )) {
			policer = &police_cfg[i];
			break;
		}
	}

	if (policer == NULL) {
		LOGF_LOG_ERROR("Configuring tc filter rule failed. policer structure not found\n");
		ret = LTQ_FAIL;
		goto exit;
	}

	if (flags & CL_ADD) {
		op = "add";
	}
	else if (flags & CL_DEL || flags & CL_MOD) {
		if (policer->count == 0) goto exit;
		op = "del";
	}
	else {
		LOGF_LOG_ERROR("Configuring tc filter rule failed. Operation not specified\n");
		ret = LTQ_FAIL;
		goto exit;
	}

	if (clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_INNER_IP
		|| clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_TX_IF
		|| clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_TCP_HDR
		|| clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_L7_PROTO
		|| clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_SSAP
		|| clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_DSAP
		|| clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_VLANID
		|| clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_INNER_VLANID
		|| clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_PRIO
		|| clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_INNER_PRIO
		|| clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_VLAN_DEI
		|| clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_INNER_VLAN_DEI
		|| clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_LLC_CONTROL
		|| clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_OUI
		|| clcfg->filter.iphdr.param_in_ex.param_in & CLSCFG_PARAM_IP_HDR_LEN_MIN
		|| clcfg->filter.iphdr.param_in_ex.param_in & CLSCFG_PARAM_IP_HDR_LEN_MAX
		|| clcfg->filter.param_in_ex.param_ex & CLSCFG_PARAM_SRC_MAC
		|| clcfg->filter.param_in_ex.param_ex & CLSCFG_PARAM_DST_MAC
		|| clcfg->filter.param_in_ex.param_ex & CLSCFG_PARAM_OUTER_IP
		|| clcfg->filter.outer_ip.param_in_ex.param_ex & CLSCFG_PARAM_OUTER_IP_PROTO
		|| clcfg->filter.outer_ip.param_in_ex.param_ex & CLSCFG_PARAM_OUTER_IP_SRC_PORT
		|| clcfg->filter.outer_ip.param_in_ex.param_ex & CLSCFG_PARAM_OUTER_IP_DST_PORT
		|| clcfg->filter.iphdr.param_in_ex.param_ex & CLSCFG_PARAM_IP_HDR_DSCP ) {
		LOGF_LOG_ERROR("Classifier configuration [%s] failed. Some/all of the configured filters are not supported with policer action\n",
			clcfg->filter.name);
		return QOS_CL_POLICER_UNSUP_PARAM;
	}

	if ((policer->count == 0) && (flags & CL_ADD)) {
		snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc add dev %s handle ffff: ingress;\n",CL_RXIF(clcfg));
		RUN_CMD3(cmd_buf, retval);
		strncpy_s(policer->ifname, MAX_IF_NAME_LEN, CL_RXIF(clcfg), MAX_IF_NAME_LEN);
	}

	uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "tc filter %s dev %s parent ffff: prio %d ",op,CL_RXIF(clcfg),order);

	if (flags & CL_DEL || flags & CL_MOD){
		(policer->count)--;
		LOGF_LOG_DEBUG("Deleting filter with prio: %d, filter count for interface %s after op: %d\n",order,CL_RXIF(clcfg),policer->count);
		goto apply_filter;
	}

add_filter:
	if (flags & CL_MOD)
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "tc filter %s dev %s parent ffff: prio %d ",op,CL_RXIF(clcfg),order);
	if((CL_O_SIP_SEL(clcfg) > 0 && util_get_ipaddr_family(clcfg->filter.outer_ip.src_ip.ipaddr) == AF_INET6)
			|| (CL_O_DIP_SEL(clcfg) > 0 && util_get_ipaddr_family(clcfg->filter.outer_ip.dst_ip.ipaddr) == AF_INET6))
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "protocol ip6 ");
	else
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "protocol ip ");
		
	uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "u32 ");
	uiLengthTmp = uiLength;

	/* filter section */
	if(CL_O_SIP_SEL(clcfg) > 0) {
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "match ip src %s", CL_SIP_OUT(clcfg));
		if (strnlen_s(CL_SMASK_OUT(clcfg), MAX_MACADDR_LEN) != 0)
			uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "/%s ", CL_SMASK_OUT(clcfg));
	}

	if(CL_O_DIP_SEL(clcfg) > 0) {
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "match ip dst %s", CL_DIP_OUT(clcfg));
		if (strnlen_s(CL_DMASK_OUT(clcfg), MAX_MACADDR_LEN) != 0)
			uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "/%s ", CL_DMASK_OUT(clcfg));
	}

	if(CL_O_SPORT_SEL(clcfg) > 0) {
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "match ip sport %d 0xffff ", CL_SPORT_ST(clcfg));
	}

	if(CL_O_DPORT_SEL(clcfg) > 0) {
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "match ip dport %d 0xffff ", CL_DPORT_ST(clcfg));
	}

	if(CL_SMAC_SEL(clcfg) > 0) {
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "match ether src %s ",CL_SMAC(clcfg));
	}

	if(CL_DMAC_SEL(clcfg) > 0) {
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "match ether dst %s ",CL_DMAC(clcfg));
	}

	if(CL_O_PROTO_SEL(clcfg) > 0) {
		if (CL_PROTO(clcfg) == CLI_PROTO_TCP)
			proto = 6;
		else if (CL_PROTO(clcfg) == CLI_PROTO_UDP)
			proto = 17;
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "match ip protocol %d 0xff ",proto);
	}

	if(CL_DSCP_SEL(clcfg) > 0) {
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "match ip tos %d 0xfc ",CL_DSCP(clcfg)<<2);
	}

	if (uiLengthTmp == uiLength) {
		LOGF_LOG_ERROR("None of the filters configured\n");
		ret = LTQ_FAIL;
		goto exit;
	}

	(policer->count)++;

	/* action section */
	if (clcfg->action.policer.pbs > 0)
		pbs = clcfg->action.policer.pbs;

	if ((CL_ACT(clcfg) & CL_ACTION_POLICER) && clcfg->action.policer.enable == 1) {
		uiLength += snprintf(cmd_buf + uiLength, sizeof(cmd_buf), "police rate %dkbit burst %d drop flowid :1", clcfg->action.policer.pr, pbs);
	}

apply_filter:
	RUN_CMD3(cmd_buf, retval);

	if ((flags & CL_MOD) && (strcmp(op, "del") == 0)) {
		op = "add";
		uiLength = 0;
		LOGF_LOG_DEBUG("Adding back filter with prio: %d for interface %s, count before op: %d\n",order,CL_RXIF(clcfg),policer->count);
		goto add_filter;
	}

	if ((policer->count == 0) && (flags & CL_DEL)) {
		snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc del dev %s handle ffff: ingress;\n",CL_RXIF(clcfg));
		RUN_CMD3(cmd_buf, retval);
		memset_s(policer->ifname, MAX_IF_NAME_LEN, 0);
	}

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : iptAdapt
 *  Description   : API to add classifier for routed traffic using iptables command/API.
		    structure of iptables chains for qos is,
	   	    mangle -L PREROUTING
			    LAN_CHAINS
			    	-m EXTMARK ! --match-mark <bits_to_identify_LAN_IF> -j RETURN
			    	LAN_1
			    	LAN_2
			    	LAN_default
			    WAN_CHAINS
			    	-m EXTMARK ! --match-mark <bits_to_identify_WAN_IF> -j RETURN
			    	WAN_1
			    	WAN_2
			    	WAN_default
	   	    mangle -L LAN_X/WAN_X
			    <rule to match criteria and mark>
			    <match mark and take decistion>

		    mangle -L LAN_default/WAN_default
			    <rule to mark with default LAN/WAN TC>
			    <match default LAN/WAN TC in mark and accept>
 * ============================================================================*/
int32_t iptAdapt(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags)
{
	/* decide to use iptables API / command. TODO */

	uint32_t retval = 0;
	char cl_chName[MAX_NAME_LEN] = "", rootChName[MAX_NAME_LEN] = "";
	bool_t bLanIng = 0, bWanIng = 0, bCfgRule = 0, bLocalIng = 0;
	uint32_t MOD_MARK = 0;
	char DEFINTF[MAX_IF_NAME_LEN]={};
	int32_t cl_in8021p = -1;
	char QOS_RULE[MAX_DATA_LEN] = "\0";
	char QOS_SUB_RULE[MAX_DATA_LEN] = "\0";
	char cl_srcIp[IP_ADDRSTRLEN], cl_srcIpMask[IP_ADDRSTRLEN], cl_dstIp[IP_ADDRSTRLEN], cl_dstIpMask[IP_ADDRSTRLEN];
	char cl_srcMac[MAX_MACADDR_LEN], cl_srcMacMask[MAX_MACADDR_LEN], acc_crit[MAX_DATA_LEN] = "";
	int32_t cl_inDscp, cl_srcPortSt, cl_srcPortEnd, cl_dstPortSt, cl_dstPortEnd, qTC, cl_outDscp, nf_Mark = 0, cl_out1p, cl_accl, qPrio;
	char* retvalue = NULL, cl_act[MAX_DATA_LEN];
	int32_t nCnt = 0, nCnt2 = 0;
	bool bFiltSet = false;
	uint32_t paramInLocal = 0;
	char cl_ingress[MAX_DATA_LEN] = "\0";
	int32_t ds_mark = 0;
	int32_t ds_tc = 0;
	int32_t ing_group = 0;
	char IngrsDefaultchName[MAX_NAME_LEN] = "";
	if_inggrp_map_t* ing_map;
	iptebt_ops_t ipt_oper = CH_READ;
	struct blob_buf b = {};
	uint32_t rule_idx = 0;

	retval = blob_buf_init(&b, 0);
	if (retval != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message init failed\n");
		goto lbl;
	}

	ENTRY
	UNUSED(order);
	/* IPv4 iptables is default table*/
	IPT = IPV4T;
	/* determine classifier as LAN/WAN based on interface category. */
	switch(iftype) {
		case QOS_IF_CATEGORY_ETHLAN:
		case QOS_IF_CATEGORY_WLANDP:
		case QOS_IF_CATEGORY_WLANNDP:
			bLanIng = 1;
			break;
		case QOS_IF_CATEGORY_ETHWAN:
		case QOS_IF_CATEGORY_PTMWAN:
		case QOS_IF_CATEGORY_ATMWAN:
		case QOS_IF_CATEGORY_LTEWAN:
			bWanIng = 1;
			break;
		case QOS_IF_CATEGORY_LOCAL:
			bLocalIng = 1;
			break;
		default:
			break;
	}

	//Forming Classification Chain Macro based on LAN / WAN
	if(bLanIng) {
		if (CL_MPTCP((&clcfg->filter))) {
			snprintf(rootChName, MAX_NAME_LEN, "%s", HA_CL);
			retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_CREAT, rootChName, -1, NULL, &b, rule_idx++);
			snprintf(cl_act, sizeof(cl_act), "-p tcp -j %s", HA_CL);
			retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_INS, IPT_QOS_STD_HOOK, -1, cl_act, &b, rule_idx++);
			snprintf(cl_chName, sizeof(cl_chName), HA_CL"_%s", CL_NAME(clcfg));
		}
		else {
			snprintf(rootChName, MAX_NAME_LEN, "%s", LAN_CL);
			snprintf(cl_chName, sizeof(cl_chName), "LAN_%s", CL_NAME(clcfg));
		}
	}
	else if(bWanIng) {
		snprintf(rootChName, sizeof(rootChName), "%s", WAN_CL);
		if((clcfg->action.flags & CL_ACTION_TC_INGRESS_EGRESS) || (clcfg->action.flags & CL_ACTION_INGRESS_TC_SET))
			snprintf(cl_chName, sizeof(cl_chName), "WAN_Ing_%s", CL_NAME(clcfg));
		else 
			snprintf(cl_chName, sizeof(cl_chName), "WAN_%s", CL_NAME(clcfg));
	}
	else if(bLocalIng) {
		snprintf(rootChName, sizeof(rootChName), "%s", OUTPUT);
		snprintf(cl_chName, sizeof(cl_chName), "LO_%s", CL_NAME(clcfg));
	}

	qTC = clcfg->action.tc;
	qPrio = clcfg->action.priority;
	cl_in8021p = CL_PCP(clcfg);
	cl_out1p = clcfg->action.vlan.pcp;
	paramInLocal = clcfg->filter.param_in_ex.param_in;
	paramInLocal = paramInLocal & (~CLSCFG_PARAM_FILTER_NAME);
	paramInLocal = paramInLocal & (~CLSCFG_PARAM_ORDER_NUM);
	paramInLocal = paramInLocal & (~CLSCFG_PARAM_RX_IF);
	paramInLocal = paramInLocal & (~CLSCFG_PARAM_TX_IF);
	paramInLocal = paramInLocal & (~CLSCFG_PARAM_ACT);
	if(paramInLocal > 0) {
		bFiltSet = true;
	}

	if (CL_ETH_SEL(clcfg) > 0) {
		LOGF_LOG_ERROR("NOT adding iptables rule for Ethertype\n");
		return  LTQ_SUCCESS;
	}

	/* TODO : set fields based on flags set at l2/l3/l4 levels. */
	snprintf(cl_srcIp, sizeof(cl_srcIp), "%s", CL_SIP_OUT(clcfg));
	snprintf(cl_srcIpMask, sizeof(cl_srcIpMask), "%s", CL_SMASK_OUT(clcfg));
	snprintf(cl_dstIp, sizeof(cl_dstIp), "%s", CL_DIP_OUT(clcfg));
	snprintf(cl_dstIpMask, sizeof(cl_dstIpMask), "%s", CL_DMASK_OUT(clcfg));
	cl_srcPortSt =  CL_SPORT_ST(clcfg);
	cl_srcPortEnd =  CL_SPORT_END(clcfg);
	cl_dstPortSt =  CL_DPORT_ST(clcfg);
	cl_dstPortEnd =  CL_DPORT_END(clcfg);
	snprintf(cl_srcMac, sizeof(cl_srcMac), "%s", CL_SMAC(clcfg));
	snprintf(cl_srcMacMask, sizeof(cl_srcMacMask), "%s", CL_SMAC_MASK(clcfg));
	cl_inDscp = CL_DSCP(clcfg);
	cl_outDscp = clcfg->action.dscp;
	cl_accl = clcfg->action.accl;
       
	retvalue = util_get_name_from_value(CL_PROTO(clcfg), axprototype);
       
	//Forming IPTABLE Rule for L4 Protocol
	if((CL_O_PROTO_SEL(clcfg) > 0) && (retvalue != NULL)) {
		if ((strcmp(retvalue, "udp") == 0) || (strcmp(retvalue, "tcp") == 0))
			snprintf(QOS_RULE, sizeof(QOS_RULE), "%s -p %s", CL_O_PROTO_EX(clcfg)?"!":"", retvalue);
		else {
			/* Protocol "all" will come only from PPA engine call */
			LOGF_LOG_WARNING("Protocol not supported in iptables\n");
			return LTQ_SUCCESS;
		}
	}

	//Forming IPTABLE Rule for Source IP
	if(CL_O_SIP_SEL(clcfg) > 0) {
		if(util_get_ipaddr_family(clcfg->filter.outer_ip.src_ip.ipaddr) == AF_INET)
		{
			IPT = IPV4T;
		}
		else
		{
			IPT = IPV6T;
		}
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s -s %s", CL_O_SIP_EX(clcfg)?"!":"", cl_srcIp);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		if(strnlen_s(cl_srcIpMask, IP_ADDRSTRLEN) > 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), "/%s", cl_srcIpMask);
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	//Forming IPTABLE Rule for Destination IP
	if(CL_O_DIP_SEL(clcfg) > 0) {
		if(util_get_ipaddr_family(clcfg->filter.outer_ip.dst_ip.ipaddr) == AF_INET)
		{
			IPT = IPV4T;
		}
		else
		{
			IPT = IPV6T;
		}
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s -d %s", CL_O_DIP_EX(clcfg)?"!":"", cl_dstIp);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		if(strnlen_s(cl_dstIpMask, IP_ADDRSTRLEN) > 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), "/%s", cl_dstIpMask);
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	//Forming IPTABLE Rule for Source Port
	if(CL_O_SPORT_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s --sport %d", CL_O_SPORT_EX(clcfg)?"!":"", cl_srcPortSt);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		if(cl_srcPortEnd != 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), ":%d", cl_srcPortEnd);
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	//Forming IPTABLE Rule for Destination Port
	if(CL_O_DPORT_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s --dport %d", CL_O_DPORT_EX(clcfg)?"!":"", cl_dstPortSt);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		if(cl_dstPortEnd != 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), ":%d", cl_dstPortEnd);
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	//Forming IPTABLE Rule for Source MAC
	if(CL_SMAC_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " -m mac %s --mac-source %s", CL_SMAC_EX(clcfg)?"!":"", cl_srcMac);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
	}

	//Forming IPTABLE Rule for Incoming DSCP
	if(CL_DSCP_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " -m dscp %s --dscp %d", CL_DSCP_EX(clcfg)?"!":"", cl_inDscp);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
	}

	//Forming IPTABLE Rule for App/L7
	if(CL_L7_PROTO_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " -m layer7 %s --l7proto %s", CL_APP_EX(clcfg)?"!":"", CL_L7_PROTO(clcfg));
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
	}

	if(CL_TCPHDR_SEL(clcfg) > 0) {
		if((CL_TCPHDR_SEL(clcfg) & CLSCFG_PARAM_TCP_HDR_ACK) > 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " -p tcp -m length %s --length :80",
				(CL_TCPHDR_EX(clcfg) & CLSCFG_PARAM_TCP_HDR_ACK)?"!":"");
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	/* ebtAdaptPart might have created required chains. do check for same. */
	if((clcfg->filter.flags & CL_INT_IPT_CH_CREATED) == 0) {
		/* create individual classifier chain - LAN_X / WAN_X */
		ipt_oper = (flags & CL_DEL) ? CH_REM: CH_CREAT;
		retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ipt_oper, cl_chName, -1, NULL, &b, rule_idx++);

		/* hook classifier chain to parent chain at given order.
		 * parent chain: LAN_CL/WAN_CL is created from engine init. */
		if((clcfg->action.flags & CL_ACTION_TC_INGRESS_EGRESS) || (clcfg->action.flags & CL_ACTION_INGRESS_TC_SET)  || bLocalIng )
			snprintf(cl_act, sizeof(cl_act), "-j %s",cl_chName);
		
		else 
			snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d/%d -j %s", mark, CLASSIFICATION_NOT_DONE_MASK, CLASSIFICATION_NOT_DONE_MASK, cl_chName);
		ipt_oper = (flags & CL_DEL) ? RULE_DEL: RULE_INS;
		retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ipt_oper, rootChName, -1, cl_act, &b, rule_idx++); /* TODO: Update order while inserting */
	}

	/* depending on action, configure rules in cl_chName. */
	if(CL_ACT(clcfg) & CL_ACTION_ACCELERATION) {
		MOD_MARK = (cl_accl << ACCEL_DIS_BIT_POS); // value of macro is 13
		snprintf(acc_crit, sizeof(acc_crit), "-m %s --mark %d/%d", mark, MOD_MARK, MOD_MARK);
		bCfgRule = 1;
	}
	if(CL_ACT(clcfg) & CL_ACTION_TC) {
		/* shift TC in mark field which would be checked by stack during ip_output */
		if (qTC != 0) {  
		if(qTC <= DEFAULT_TC)
			MOD_MARK |= (DEFAULT_TC + 1 - qTC) << TC_START_BIT_POS;
		else
			MOD_MARK |= (MAX_TC_NUM + DEFAULT_TC + 1 - qTC) << TC_START_BIT_POS;
		}
		/* enable PPA bit always. */
		PPA_BIT_ENABLE(MOD_MARK)
		snprintf(acc_crit, sizeof(acc_crit), "-m %s --mark %d/%d", mark, MOD_MARK, CLASSIFICATION_BIT_MASK);
		/* if it is Ingress then add below stuffs, Egress is handled above */
		if((clcfg->action.flags & CL_ACTION_TC_INGRESS_EGRESS) || (clcfg->action.flags & CL_ACTION_INGRESS_TC_SET)) {
			ds_tc = clcfg->action.tc_ing;
			ing_map = inggrp_map_get_by_ifname(clcfg->filter.tx_if);
			if(!ing_map) {
				LOGF_LOG_ERROR("DS-QOS Ingress Group lookup failed for interface %s\n",
						clcfg->filter.tx_if);
				return QOS_IFMAP_LOOKUP_FAIL;
			}
			ing_group = ing_map->inggrp;
		/* if both ingress and egress TC set*/
		if(clcfg->action.flags & CL_ACTION_TC_INGRESS_EGRESS) {
			MOD_MARK |= (3 << INGRESS_EGRESS_TC_SET);
			MOD_MARK |= ((DEFAULT_TC + 1 - ds_tc) << INGRESS_TC_BIT);
			MOD_MARK |= ((DEFAULT_TC + 1 - qTC) << QUEPRIO_START_BIT_POS);
		} else {
			/*only ingress tc is set, in this case set egress tc and prio to default TC*/	
			MOD_MARK |= (1 << INGRESS_EGRESS_TC_SET);
			MOD_MARK |= ((DEFAULT_TC + 1 - ds_tc) << INGRESS_TC_BIT);
			MOD_MARK |= (1 << TC_START_BIT_POS);
			MOD_MARK |= (1 << QUEPRIO_START_BIT_POS);
		} 
		MOD_MARK = MOD_MARK | (ing_group << INGRESS_GROUP_BIT);
		if (clcfg->action.flags & CL_ACTION_CL_HW_ENGINE)
			MOD_MARK |= (1 << INGRESS_SW_HW_BIT);
		MOD_MARK |= (1 << INGRESS_EGRESS_QOS_BIT);
		
		snprintf(cl_ingress, sizeof(cl_ingress), "%s -j %s --set-mark %d/%d", QOS_RULE, MARK, MOD_MARK, CLASSIFICATION_BIT_MASK);
		ipt_oper = (flags & CL_DEL) ? RULE_DEL: RULE_APP;
		retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ipt_oper, cl_chName, -1, cl_ingress, &b, rule_idx++);
		snprintf(acc_crit, sizeof(acc_crit), "-m %s --mark %d/%d", mark, MOD_MARK, CLASSIFICATION_BIT_MASK);
		
		/*create Ingress Default chain*/
		snprintf(IngrsDefaultchName, sizeof(IngrsDefaultchName), "%s", DEF_INGRESS_CL);
		LOGF_LOG_DEBUG("DS QOS Ingress Creating Default chain\n");
		ds_mark = 0;
		ds_mark |= (3 << INGRESS_EGRESS_TC_SET);
		ds_mark |= (1 << INGRESS_TC_BIT);
		ds_mark |= (1 << TC_START_BIT_POS);
		ds_mark |= (1 << QUEPRIO_START_BIT_POS);
		ds_mark |= (ing_group << INGRESS_GROUP_BIT);
		ds_mark |= (1 << INGRESS_EGRESS_QOS_BIT);
		PPA_BIT_ENABLE(ds_mark);

		/* create default classifier chain for WAN. */
		set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_CREAT, IngrsDefaultchName, -1, NULL, &b, rule_idx++);

		/*mark all packets in this chain with default Ingress Values*/
		snprintf(cl_ingress, sizeof(cl_ingress), "-j %s --set-mark %d/%d", MARK, ds_mark, CLASSIFICATION_BIT_MASK);
		retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_INS, DEF_INGRESS_CL, -1, cl_ingress, &b, rule_idx++);

		/* and accept the packets. */
		snprintf(cl_ingress, sizeof(cl_ingress), "-m %s --mark %d/%d -j ACCEPT", mark, ds_mark, CLASSIFICATION_BIT_MASK);
		retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, DEF_INGRESS_CL, -1, cl_ingress, &b, rule_idx++);

		/* link this chain to end of WAN classifier list just above the Egress Default*/
		snprintf(cl_ingress, sizeof(cl_ingress), "-j %s", DEF_INGRESS_CL);
		retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, WAN_CL, -1, cl_ingress, &b, rule_idx++);

		/* Ingress Default chain should always be on top of Egress Default chain
		 * if Egress default chain is der, first delete it add ingress default chain then append
		 * egress default chain
		 */

		/*Flush all the rules and delete the chain*/
		snprintf(cl_ingress, sizeof(cl_ingress), "-m %s --mark %d/%d -j %s", mark, CLASSIFICATION_NOT_DONE_MASK, CLASSIFICATION_NOT_DONE_MASK, DEF_WAN_CL);
		set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_DEL, WAN_CL, -1, cl_ingress, &b, rule_idx++);

		/* mark all packets in this chain with default queue priority. */
		/* link this chain to end of LAN classifier list. */
		snprintf(cl_ingress, sizeof(cl_ingress), "-m %s --mark %d/%d -j %s", mark, CLASSIFICATION_NOT_DONE_MASK, CLASSIFICATION_NOT_DONE_MASK, DEF_WAN_CL);
		ipt_oper = RULE_APP;
		retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ipt_oper, WAN_CL, -1, cl_ingress, &b, rule_idx++);
			
		bCfgRule = 0;
			
	} else
		bCfgRule = 1;
	}
	if(bCfgRule == 1) {
		/* Creating Classifier Rules and Marking with given Queue ID.
		 * If classifier has only fileds such as VLAN Id, VLAN priority, destination MAC
		 * then QOS_RULE would be NULL here. In this case skip configuring wild card
		 * rule to mark packets with MOD_MARK.
		 */
		if(( (strnlen_s(QOS_RULE, MAX_DATA_LEN) == 0) && (CL_VID_SEL(clcfg) || CL_VPRIO_SEL(clcfg) || CL_DMAC_SEL(clcfg)) ) == 0) {
			if (CL_MPTCP((&clcfg->filter))) {
				snprintf(cl_act, sizeof(cl_act), "%s -j %s --set-mark %d/%d", QOS_RULE, SKB_MARK,
					HA_MARK_VALUE, HA_MARK_MASK);
			}
			else {
				snprintf(cl_act, sizeof(cl_act), "%s -j %s --set-mark %d/%d", QOS_RULE, MARK, MOD_MARK, CLASSIFICATION_BIT_MASK);
			}
			ipt_oper = (flags & CL_DEL) ? RULE_DEL : RULE_APP;
			retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ipt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
		}
	}

	if(CL_ACT(clcfg) & CL_ACTION_DSCP) {
		// Setting Outgoing DSCP Value 
		if(cl_outDscp >= 0) {
			snprintf(cl_act, sizeof(cl_act), " %s -m %s --mark %d/%d -j DSCP --set-dscp %d", QOS_RULE, mark, MOD_MARK, MOD_MARK, cl_outDscp);
			ipt_oper = (flags & CL_DEL) ? RULE_DEL : RULE_APP;
			retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ipt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
			snprintf(acc_crit, sizeof(acc_crit), "-m %s --mark %d/%d -m dscp --dscp %d", mark, MOD_MARK, MOD_MARK, cl_outDscp);
		}
	}

	if((!CL_MPTCP((&clcfg->filter))) && ( (CL_ACT(clcfg) & CL_ACTION_DSCP) || (CL_ACT(clcfg) & CL_ACTION_TC) || (CL_ACT(clcfg) & CL_ACTION_ACCELERATION) )) {
		/* accept packets if they are processed for DSCP or TC or acceleration disable actions. */
		snprintf(cl_act, sizeof(cl_act), "%s -j ACCEPT", acc_crit);
		ipt_oper = (flags & CL_DEL) ? RULE_DEL : RULE_APP;
		retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ipt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
	}

	if (flags & CL_DEL) {
		ipt_oper = CH_REM;
		retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ipt_oper, cl_chName, -1, NULL, &b, rule_idx++);
	}

	if(CL_ACT(clcfg) & CL_ACTION_VLAN) {
		/* convert prio of format 1-8 into 8-1 because software prio descending order is 8-1. */
		if(CL_VPRIO_SEL(clcfg) > 0) {
			nf_Mark = cl_in8021p; /* incoming 1p is 1-8. */
		}
		else {
			nf_Mark = qPrio;
		}

		flags |= CPU_CALLER;
		/* Setting Outgoing 8021p value using vconfig. */
		if((strnlen_s(CL_TXIF(clcfg), MAX_IF_NAME_LEN) > 0) && (strnlen_s(CL_TXIF(clcfg), MAX_IF_NAME_LEN) < (sizeof(DEFINTF) - 1))) {
			strncpy_s(DEFINTF, MAX_IF_NAME_LEN, CL_TXIF(clcfg), strnlen_s(CL_TXIF(clcfg), MAX_IF_NAME_LEN) + 1);
		}
		else {
			/* no VLAN interfaces on LAN as of now.
			   use default interface on WAN, if it's VLAN based.
				 TODO: If TX interface is not provided, how to apply this rule?
			*/
			for(nCnt=0; nCnt < nMapInUse; nCnt++) {
				retval = vlanPrioUpdate(xVlanMap[nCnt].sIf, qPrio, cl_out1p, flags);
			}
		}

		if(bFiltSet == false) {
			/* VLANTODO : need to add/update entire prio map for tx_if in global array */
			retval = vlanPrioUpdate(DEFINTF, 0, cl_out1p, flags);
		}
		else {
			/* VLANTODO : need to add/update specific prio map for tx_if in global array */
			retval = vlanPrioUpdate(DEFINTF, nf_Mark, cl_out1p, flags);
		}

		LOGF_LOG_DEBUG("----------VLAN priority map----------\n");
		for(nCnt=0; nCnt < nMapInUse; nCnt++) {
			LOGF_LOG_DEBUG("\nInterface[%s]\n\t", xVlanMap[nCnt].sIf);
			for(nCnt2=0; nCnt2 < DEFAULT_TC; nCnt2++) {
				if(flags & CPU_CALLER) {
					LOGF_LOG_DEBUG("%d -> %d \n", nCnt2, xVlanMap[nCnt].nVlanPrioMap[nCnt2]);
				}
				else {
					LOGF_LOG_DEBUG("%d -< %d \n", nCnt, xVlanMap[nCnt].nVlanPrioPPAMap[nCnt2]);
				}
			}
			LOGF_LOG_DEBUG("\nDefault VLAN priority [%d]\t", xVlanMap[nCnt].defIfVlanPrio);
		}
		LOGF_LOG_DEBUG("\nNumber of interfaces with default VLAN priority [%d]\t", nDefVlanPrioCnt);
	}

	send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_MODIFY);
lbl:
	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : ebtAdaptPart
 *  Description   : API that configures VLAN header related fields in ebtables
		    and returns configured TC in extmark if fields are matched.
	packets from ebtables would carry spl value to indicate ebt processing status in extmark.
	if ebt processed value says pass then check remaining criteria of iptables. else return.
 * ============================================================================*/
int32_t ebtAdaptPart(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags)
{
	/* decide to use iptables API / command. TODO */

	/* in case criteria is vlan, should IPT rules be configured to accept pkt. so that pkts dont process chains un-necessarily ?
	   what if action is non-tc, then what mark will ebt put for ipt to match with ?
	TODO */
	uint32_t retval = 0;
	char policy[MAX_DATA_LEN] = "";
	char cl_chName[MAX_NAME_LEN] = "", iptRootChName[MAX_NAME_LEN] = "", ebtRootChName[MAX_NAME_LEN] = "";
	bool_t bLanIng = 0, bWanIng = 0, bLocalIng = 0;
	int32_t cl_in8021p = -1, cl_inVlanId = -1;
	char QOS_RULE[MAX_DATA_LEN] = "\0", cl_act[MAX_DATA_LEN];
	int32_t qTC,qPrio;
	uint32_t vid_1p_mark = 0, Incom_802_1p = 0, Incom_VID = 0, X_MARK = 0, mark_1p = 0, vid_mark = 0;
	uint32_t in_1p, in_vid;
	int32_t cl_inDscp,cl_inTos,cl_outTos,cl_outDscp;
	char cl_dstMac[MAX_MACADDR_LEN];
	char QOS_RULE_1P[MAX_DATA_LEN] = "\0";
	char QOS_RULE_VID[MAX_DATA_LEN] = "\0";
	char QOS_SUB_RULE[MAX_DATA_LEN] = "\0";
	struct blob_buf b = {};
	uint32_t rule_idx = 0;
	iptebt_ops_t ebt_oper = CH_READ;
	UNUSED(order);

	retval = blob_buf_init(&b, 0);
	if (retval != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message init Failed\n");
		goto lbl;
	}

	bLanIng = bWanIng = 0;
	memset_s(cl_dstMac, sizeof(cl_dstMac), 0);
	/* determine classifier as LAN/WAN based on interface category. */
	switch(iftype) {
		case QOS_IF_CATEGORY_ETHLAN:
		case QOS_IF_CATEGORY_WLANDP:
		case QOS_IF_CATEGORY_WLANNDP:
		case QOS_IF_CATEGORY_LOCAL:
			bLanIng = 1;
			break;
		case QOS_IF_CATEGORY_ETHWAN:
		case QOS_IF_CATEGORY_PTMWAN:
		case QOS_IF_CATEGORY_ATMWAN:
		case QOS_IF_CATEGORY_LTEWAN:
			bWanIng = 1;
			break;
		default:
			break;
	}

	if (iftype == QOS_IF_CATEGORY_LOCAL)
		bLocalIng = 1;

	/* IPv4 iptables is default table*/
	IPT = IPV4T;

	/* single list for both LAN and WAN. and actual classifiers will have interface name as mandatory match. */
	snprintf(ebtRootChName, sizeof(ebtRootChName), "%s", CL_LIST);

	if(bLanIng) {
		snprintf(cl_chName, sizeof(cl_chName), "LAN_%s", CL_NAME(clcfg));
		snprintf(iptRootChName, sizeof(iptRootChName), "%s", LAN_CL);
	}
	else if(bWanIng) {
		snprintf(cl_chName, sizeof(cl_chName), "WAN_%s", CL_NAME(clcfg));
		snprintf(iptRootChName, sizeof(iptRootChName), "%s", WAN_CL);
	}

	if((flags & CL_DEL) || (flags & CL_MOD)) {
		/* flush classifier chain. */
		retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_FL, cl_chName, -1, NULL, &b, rule_idx++);
		/* remove classifier chain association with parent chain. */
		snprintf(cl_act, sizeof(cl_act), "-j %s", cl_chName);
		retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, RULE_DEL, ebtRootChName, -1, cl_act, &b, rule_idx++);
		/* remove classifier chain. */
		retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_REM, cl_chName, -1, NULL, &b, rule_idx++);
	}

	cl_inDscp = CL_DSCP(clcfg);
	cl_inTos = cl_inDscp<<2;
	cl_outDscp = clcfg->action.dscp;
	cl_outTos = cl_outDscp<<2;
	in_1p = 0;
	in_vid = 0;	
	/* TODO : set fields based on flags set at l2/l3/l4 levels. */
	cl_in8021p = CL_PCP(clcfg);
	cl_inVlanId = CL_VLAN(clcfg);
	qTC = clcfg->action.tc;
	qPrio = clcfg->action.priority;
	snprintf(cl_dstMac, sizeof(cl_dstMac), "%s", CL_DMAC(clcfg));

	if(CL_VPRIO_SEL(clcfg) > 0) {
		// Shift Incoming 1P value so that it matches the designated 1P bit positions in NFMARK
		// This logic only used for Routed case i.e for Iptables
		Incom_802_1p = cl_in8021p << VPRIO_START_BIT_POS;
		in_1p=1;
		// Framing QOS Rule for Bridge scenario i.e for Ebtables
		snprintf(QOS_RULE_1P, sizeof(QOS_RULE_1P), "-p 8021Q %s --vlan-prio %d", CL_VPRIO_EX(clcfg)?"!":"", cl_in8021p);
	}
	
	// Forming EBTABLE Rule for Incoming VlanId
	if(CL_VID_SEL(clcfg) > 0) {
		// Shift Incoming VlanId value so that it matches the designated VlanId bit positions in NFMARK
		// This logic only used for Routed case i.e for Iptables
		Incom_VID = cl_inVlanId << VLANID_START_BIT_POS;
		in_vid=1;
		// Framing QOS Rule for Bridge scenario i.e for Ebtables
		snprintf(QOS_RULE_VID, sizeof(QOS_RULE_VID), "-p 8021Q %s --vlan-id %d", CL_VID_EX(clcfg)?"!":"", cl_inVlanId);
	}

	if(CL_ETH_SEL(clcfg) > 0) {
		LOGF_LOG_DEBUG("ETHTYPE writing ebtable rule with type %d\n",clcfg->filter.ethhdr.eth_type);
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s -p %x", CL_ETH_EX(clcfg)?"!":"", clcfg->filter.ethhdr.eth_type);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
	}
	
	if(CL_DSCP_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s -p ipv4 --ip-tos %d", CL_DSCP_EX(clcfg)?"!":"", cl_inTos);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
	}
	// Forming EBTABLE Rule for Destination MAC address
	if(CL_DMAC_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s -d %s", CL_DMAC_EX(clcfg)?"!":"", cl_dstMac);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
	}

	// Forming EBTABLE Rule for Ingress Interface
	if(CL_RXIF_SEL(clcfg) > 1) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s -i %s", CL_RXIF_EX(clcfg)?"!":"", CL_RXIF(clcfg));
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
	}

	uint32_t MOD_MARK = 0, QOS_802_1P_MASK; //, QOS_802_1P;

	snprintf(policy, MAX_DATA_LEN, "-P %s", "RETURN");
	ebt_oper = (flags & CL_DEL) ? CH_REM : CH_CREAT;
	retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, policy, &b, rule_idx++);

	/* identify iptables/ip6tables to use.. */
	if(CL_O_SIP_SEL(clcfg) > 0 || CL_O_DIP_SEL(clcfg) > 0) {
		if(util_get_ipaddr_family(clcfg->filter.outer_ip.src_ip.ipaddr) == AF_INET ||
			util_get_ipaddr_family(clcfg->filter.outer_ip.dst_ip.ipaddr) == AF_INET) {
			IPT = IPV4T;
		}
		else {
			IPT = IPV6T;
		}
	}

	/* hook classifier chain to parent chain at given order.
	 * parent chain: LAN_CL/WAN_CL is created from engine init. */
	snprintf(cl_act, sizeof(cl_act), "-j %s", cl_chName);
	ebt_oper = (flags & CL_DEL) ? RULE_DEL : RULE_INS;
	retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, ebtRootChName, -1, cl_act, &b, rule_idx++); /* TODO: Insert rule handling */

	ebt_oper = (flags & CL_DEL) ? CH_REM : CH_CREAT;
	retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ebt_oper, cl_chName, -1, NULL, &b, rule_idx++);
	clcfg->filter.flags = CL_INT_IPT_CH_CREATED;

	/* hook classifier chain to parent chain at given order.
	 * parent chain: LAN_CL/WAN_CL is created from engine init. */
	if ((clcfg->action.flags & CL_ACTION_TC_INGRESS_EGRESS) || (clcfg->action.flags & CL_ACTION_INGRESS_TC_SET)  || bLocalIng)
		snprintf(cl_act, sizeof(cl_act), "-j %s", cl_chName);
	else
		snprintf(cl_act, sizeof(cl_act), "-m extmark --mark %d/%d -j %s", CLASSIFICATION_NOT_DONE_MASK, CLASSIFICATION_NOT_DONE_MASK, cl_chName);

	ebt_oper = (flags & CL_DEL) ? RULE_DEL : RULE_INS;
	retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ebt_oper, iptRootChName, -1, cl_act, &b, rule_idx++); /* TODO: Update order */

	if(CL_ACT(clcfg) & CL_ACTION_DSCP) {
		// Setting Outgoing DSCP Value 
		if(cl_outDscp >= 0) {
			snprintf(cl_act, sizeof(cl_act), "%s -j tos --set-tos %d", QOS_RULE, cl_outTos);
			ebt_oper = (flags & CL_DEL) ? RULE_DEL: RULE_APP;
			retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
		}
	}
	if(CL_ACT(clcfg) & CL_ACTION_ACCELERATION) {
		MOD_MARK = (clcfg->action.accl << ACCEL_DIS_BIT_POS);
	}
	/* depending on action, configure rules in cl_chName. */
	if(CL_ACT(clcfg) & CL_ACTION_TC) {
		QOS_802_1P_MASK=0x1c000;
		/* shift TC in mark field which would be checked by stack during ip_output */
		if(qTC <= DEFAULT_TC)
			MOD_MARK |= (((DEFAULT_TC + 1 - qTC) << TC_START_BIT_POS) | ((MAX_PRIO + 1 - qPrio) << QUEPRIO_START_BIT_POS));
		else
			MOD_MARK |= (((MAX_TC_NUM + DEFAULT_TC + 1 - qTC) << TC_START_BIT_POS) | ((MAX_PRIO + 1 - qPrio) << QUEPRIO_START_BIT_POS));

		PPA_BIT_ENABLE(MOD_MARK)
		LOGF_LOG_DEBUG("EBTABLES MOD_MARK is %d\n",MOD_MARK);
		
		/* enable PPA bit always. */
		X_MARK = qPrio << SUBQUEUE_START_BIT_POS;

		ebt_oper = (flags & CL_DEL) ? RULE_DEL: RULE_APP;
		// Creating Classifier Rules and Marking with given Queue ID
		switch(in_1p) {
			case 1:
				switch(in_vid) {
					case 1:
						vid_1p_mark = Incom_802_1p | Incom_VID;
						snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d/%d -j %s --set-mark %d", mark, vid_1p_mark, vid_1p_mark, MARK, X_MARK);
						retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d %s -j %s --set-mark %d", mark, X_MARK, QOS_RULE, MARK, MOD_MARK);
						retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), "%s -j %s --set-%s %d --%s-target CONTINUE", QOS_RULE, mark, mark, X_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), "--%s %d %s -j %s --set-%s %d --%s-target CONTINUE", mark, X_MARK, QOS_RULE_VID, mark, mark, X_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), "--%s %d %s -j %s --set-%s %d --%s-target CONTINUE", mark, X_MARK, QOS_RULE_1P, mark, mark, MOD_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						break;
					case 0:	
						// Check the value of Incoming 802.1P specified by user against the 802.1P value available in NF_MARK 
						mark_1p = Incom_802_1p;
						snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d/%d -j %s --set-mark %d", mark, mark_1p, QOS_802_1P_MASK, MARK, X_MARK);
						retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d %s -j %s --set-mark %d", mark, X_MARK, QOS_RULE, MARK, MOD_MARK);
						retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), "%s -j %s --set-%s %d --%s-target CONTINUE", QOS_RULE, mark, mark, X_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), "--mark %d %s -j %s --set-%s %d --%s-target CONTINUE", X_MARK, QOS_RULE_1P, mark, mark, MOD_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						break;
				}
				break;
			case 0:
				switch(in_vid) {
					case 1:
						// Check the value of Incoming VlanId specified by user against the VlanId value available in NF_MARK 
						vid_mark = Incom_VID;
						snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d/%d -j %s --set-mark %d", mark, vid_mark, vid_mark, MARK, X_MARK);
						retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d %s -j %s --set-mark %d", mark, X_MARK, QOS_RULE, MARK, MOD_MARK);
						retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), "%s -j %s --set-%s %d --%s-target CONTINUE", QOS_RULE, mark, mark, X_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), "--mark %d %s -j %s --set-%s %d --%s-target CONTINUE", X_MARK, QOS_RULE_VID, mark, mark, MOD_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						break;
					case 0: /* this case is used when both VLAN Id and pirority are not set.
						 * such as destination MAC address. */
						snprintf(cl_act, sizeof(cl_act), "%s -j %s --set-%s %d --%s-target CONTINUE", QOS_RULE, mark, mark, MOD_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
				}
				break;
	
		}
	}
	
	send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_MODIFY);
lbl:
	return retval;
}

/* =============================================================================
 *  Function Name : cpu_ebtAdapt
 *  Description   : API to add classifier for bridged traffic using ebtables command.
	Structure of ebtables chains for qos is,
	-t filter -L FORWARD
		LAN_CHAINS
			-m EXTMARK ! --match-mark <bits_to_identify_LAN_IF> -j RETURN
			LAN_1
			LAN_2
			LAN_default
		WAN_CHAINS
			-m EXTMARK ! --match-mark <bits_to_identify_WAN_IF> -j RETURN
			WAN_1
			WAN_2
			WAN_default

	-t filter -L LAN_X/WAN_X
		<rule to match criteria and mark>
		<match mark and take decistion>

	-t filter -L LAN_default/WAN_default
		<rule to mark with default LAN/WAN TC>
		<match default LAN/WAN TC in mark and accept>

	packets from ebtables would carry tc value and spl value to indicate ebt processed in extmark.
	if ebt processed then go to IPQOS_QUEUE_MAP directly.
 * ============================================================================*/
int32_t cpu_ebtAdapt(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags)
{
	/* in case criteria is vlan, should IPT rules be configured to accept pkt. so that pkts dont process chains un-necessarily ?
	   what if action is non-tc, then what mark will ebt put for ipt to match with ?
	TODO */
	uint32_t retval = 0;
	char cmd[MAX_DATA_LEN] = "", cl_chName[MAX_NAME_LEN] = "", rootChName[MAX_NAME_LEN] = "";
	bool_t bLanIng, bWanIng;
	uint32_t MAX_QUEUES = 0;
	char DEFINTF[MAX_IF_NAME_LEN]={0};
	int32_t nf_Mark = 0, cl_out1p;
	char cl_act[MAX_DATA_LEN];

	struct blob_buf b = {};
	uint32_t rule_idx = 0;
	iptebt_ops_t ebt_oper = CH_READ;
	UNUSED(order);

	retval = blob_buf_init(&b, 0);
	if (retval != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message init Failed\n");
		goto lbl;
	}

	bLanIng = bWanIng = 0;

	/* determine classifier as LAN/WAN based on interface category. */
	switch(iftype) {
		case QOS_IF_CATEGORY_ETHLAN:
		case QOS_IF_CATEGORY_WLANDP:
		case QOS_IF_CATEGORY_WLANNDP:
		case QOS_IF_CATEGORY_LOCAL:
			bLanIng = 1;
			break;
		case QOS_IF_CATEGORY_ETHWAN:
		case QOS_IF_CATEGORY_PTMWAN:
		case QOS_IF_CATEGORY_ATMWAN:
		case QOS_IF_CATEGORY_LTEWAN:
			bWanIng = 1;
			break;
		default:
			break;
	}

	//Forming Classification Chain Macro
	snprintf(rootChName, sizeof(rootChName), "%s", "CL_LIST");

	if(bLanIng) {
		snprintf(cl_chName, sizeof(cl_chName), "LAN_%s", CL_NAME(clcfg));
	}
	else if(bWanIng) {
		snprintf(cl_chName, sizeof(cl_chName), "WAN_%s", CL_NAME(clcfg));
	}

	if((flags & CL_DEL) || (flags & CL_MOD)) {
		/* remove classifier chain. */
		retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_FL, cl_chName, -1, NULL, &b, rule_idx++);
		/* remove classifier chain association with parent chain. */
		snprintf(cl_act, sizeof(cl_act), "-j %s", cl_chName);
		retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, RULE_DEL, rootChName, -1, cl_act, &b, rule_idx++);
		/* remove classifier chain. */
		retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_REM, cl_chName, -1, NULL, &b, rule_idx++);
	}

	int32_t cl_in8021p = -1, cl_inVlanId = -1;
	char QOS_RULE[MAX_DATA_LEN] = "\0";
	char QOS_SUB_RULE[MAX_DATA_LEN] = "\0";
	char cl_srcIp[IP_ADDRSTRLEN], cl_srcIpMask[IP_ADDRSTRLEN], cl_dstIp[IP_ADDRSTRLEN], cl_dstIpMask[IP_ADDRSTRLEN];
	char cl_srcMac[MAX_MACADDR_LEN], cl_srcMacMask[MAX_MACADDR_LEN];
	char cl_dstMac[MAX_MACADDR_LEN], cl_dstMacMask[MAX_MACADDR_LEN];
	int32_t cl_inDscp, cl_srcPortSt, cl_srcPortEnd, cl_dstPortSt, cl_dstPortEnd, qId;

	uint32_t X_MARK = 0, QOS_TOS_MAP = 0;
	uint32_t in_1p, in_vid;
	char QOS_RULE_1P[MAX_DATA_LEN] = "\0";
	char QOS_RULE_VID[MAX_DATA_LEN] = "\0";
	char* retvalue = NULL;

	in_1p = 0;
	in_vid = 0;	
	/* TODO : set fields based on flags set at l2/l3/l4 levels. */
	snprintf(cl_srcIp, sizeof(cl_srcIp), "%s", CL_SIP_OUT(clcfg));
	snprintf(cl_srcIpMask, sizeof(cl_srcIpMask), "%s", CL_SMASK_OUT(clcfg));
	snprintf(cl_dstIp, sizeof(cl_dstIp), "%s", CL_DIP_OUT(clcfg));
	snprintf(cl_dstIpMask, sizeof(cl_dstIpMask), "%s", CL_DMASK_OUT(clcfg));
	cl_srcPortSt =  CL_SPORT_ST(clcfg);
	cl_srcPortEnd =  CL_SPORT_END(clcfg);
	cl_dstPortSt =  CL_DPORT_ST(clcfg);
	cl_dstPortEnd =  CL_DPORT_END(clcfg);
	snprintf(cl_srcMac, sizeof(cl_srcMac), "%s", CL_SMAC(clcfg));
	snprintf(cl_srcMacMask, sizeof(cl_srcMacMask), "%s", CL_SMAC_MASK(clcfg));
	snprintf(cl_dstMac, sizeof(cl_dstMac), "%s", CL_DMAC(clcfg));
	snprintf(cl_dstMacMask, sizeof(cl_dstMacMask), "%s", CL_DMAC_MASK(clcfg));
	cl_inDscp = CL_DSCP(clcfg);
	cl_in8021p = CL_PCP(clcfg);
	cl_inVlanId = CL_VLAN(clcfg);
	qId = clcfg->action.tc;
	cl_out1p = clcfg->action.vlan.pcp;

	//Forming EBTABLE Rule for L4 Protocol
	if(CL_O_PROTO_SEL(clcfg) > 0) {
		retvalue = util_get_name_from_value(CL_PROTO(clcfg), axprototype);
		snprintf(QOS_RULE, sizeof(QOS_RULE), "%s --ip-protocol %s", CL_O_PROTO_EX(clcfg)?"!":"", (retvalue != NULL)?retvalue:"");
	}

	//Forming EBTABLE Rule for Source IP
	if(CL_O_SIP_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s --ip-source %s", CL_O_SIP_EX(clcfg)?"!":"", cl_srcIp);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		if(strnlen_s(cl_srcIpMask, IP_ADDRSTRLEN) > 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), "/%s", cl_srcIpMask);
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	//Forming EBTABLE Rule for Destination IP
	if(CL_O_DIP_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s --ip-destination %s", CL_O_DIP_EX(clcfg)?"!":"", cl_dstIp);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		if(strnlen_s(cl_dstIpMask, IP_ADDRSTRLEN) > 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), "/%s", cl_dstIpMask);
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	//Forming EBTABLE Rule for Source Port
	if(CL_O_SPORT_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s --ip-source-port %d", CL_O_SPORT_EX(clcfg)?"!":"", cl_srcPortSt);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		if(cl_srcPortEnd != 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), ":%d", cl_srcPortEnd);
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	//Forming EBTABLE Rule for Destination Port
	if(CL_O_DPORT_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s --ip-destination-port %d", CL_O_DPORT_EX(clcfg)?"!":"", cl_dstPortSt);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		if(cl_dstPortEnd != 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), ":%d", cl_dstPortEnd);
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	//Forming EBTABLE Rule for Source MAC
	if(CL_SMAC_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s -s %s", CL_SMAC_EX(clcfg)?"!":"", cl_srcMac);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		if(strnlen_s(cl_srcMacMask, MAX_MACADDR_LEN) > 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), "/%s", cl_srcMacMask);
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	//Forming EBTABLE Rule for Destination MAC
	if(CL_DMAC_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s -d %s", CL_DMAC_EX(clcfg)?"!":"", cl_dstMac);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		if(strnlen_s(cl_dstMacMask, MAX_MACADDR_LEN) > 0) {
			memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
			snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), "/%s", cl_dstMacMask);
			strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
		}
	}

	//Forming EBTABLE Rule for Incoming DSCP
	if(CL_DSCP_SEL(clcfg) > 0) {
		QOS_TOS_MAP=cl_inDscp<<2;
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s --ip-tos %d", CL_DSCP_EX(clcfg)?"!":"", QOS_TOS_MAP);
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
	}
	
	//Forming EBTABLE Rule for Incoming 8021P
	if(CL_VPRIO_SEL(clcfg) > 0) {
		in_1p=1;
		// Framing QOS Rule for Bridge scenario i.e for Ebtables
		snprintf(QOS_RULE_1P, sizeof(QOS_RULE_1P), "-p 8021Q %s --vlan-prio %d", CL_VPRIO_EX(clcfg)?"!":"", cl_in8021p);
       	}
	
	// Forming EBTABLE Rule for Incoming VlanId
	if(cl_inVlanId != -1) {
		in_vid=1;
		// Framing QOS Rule for Bridge scenario i.e for Ebtables
		snprintf(QOS_RULE_VID, sizeof(QOS_RULE_VID), "-p 8021Q %s --vlan-id %d", CL_VID_EX(clcfg)?"!":"", cl_inVlanId);
	}
	
	// Forming EBTABLE Rule for Ingress Interface
	if(CL_RXIF_SEL(clcfg) > 0) {
		memset_s(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), 0);
		snprintf(QOS_SUB_RULE, sizeof(QOS_SUB_RULE), " %s -i %s", CL_RXIF_EX(clcfg)?"!":"", CL_RXIF(clcfg));
		strncat_s(QOS_RULE, MAX_DATA_LEN, QOS_SUB_RULE, strnlen_s(QOS_SUB_RULE, MAX_DATA_LEN));
	}

	uint32_t MOD_MARK = 0; //, QOS_802_1P;

	/* parent classifier chain - LAN_CL / WAN_CL is created from engine init. */

	/* create individual classifier chain in ebtables->filter table as LAN_X / WAN_X */
	ebt_oper = (flags & CL_DEL) ? CH_REM : CH_CREAT;
	retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, NULL, &b, rule_idx++);

	snprintf(cl_act, sizeof(cl_act), "-j %s", cl_chName);
	ebt_oper = (flags & CL_DEL) ? RULE_DEL : RULE_INS;
	retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, rootChName, -1, cl_act, &b, rule_idx++); /* TODO: Insert rule handling */

	/* depending on action, configure rules in cl_chName. */
	if(CL_ACT(clcfg) & CL_ACTION_TC) {
		/* shift prio in mark field which would be checked by stack during ip_output */
		if(qId <= DEFAULT_TC)
			MOD_MARK |= (DEFAULT_TC + 1 - qId) << TC_START_BIT_POS;
		else
			MOD_MARK |= (MAX_TC_NUM + DEFAULT_TC + 1 - qId) << TC_START_BIT_POS;

		// Creating Classifier Rules and Marking with given Queue ID
		ebt_oper = (flags & CL_DEL) ? RULE_DEL: RULE_APP;
		switch(in_1p) {
			case 1:
				switch(in_vid) {
					case 1:
						snprintf(cl_act, sizeof(cl_act), " %s -j %s --set-%s %d --%s-target CONTINUE", QOS_RULE, mark, mark, X_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), " --%s %d %s -j %s --set-%s %d --%s-target CONTINUE", mark, X_MARK, QOS_RULE_VID, mark, mark, X_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), " --%s %d %s -j %s --set-%s %d --%s-target CONTINUE", mark, X_MARK, QOS_RULE_1P, mark, mark, MOD_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						break;
					case 0:	
						// Check the value of Incoming 802.1P specified by user against the 802.1P value available in NF_MARK 
						snprintf(cl_act, sizeof(cl_act), " %s -j %s --set-%s %d --%s-target CONTINUE", QOS_RULE, mark, mark, X_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), " --mark %d %s -j %s --set-%s %d --%s-target CONTINUE", X_MARK, QOS_RULE_1P, mark, mark, MOD_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						break;
				}
				break;
			case 0:
				switch(in_vid) {
					case 1:
						// Check the value of Incoming VlanId specified by user against the VlanId value available in NF_MARK 
						snprintf(cl_act, sizeof(cl_act), " %s -j %s --set-%s %d --%s-target CONTINUE", QOS_RULE, mark, mark, X_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						snprintf(cl_act, sizeof(cl_act), " --mark %d %s -j %s --set-%s %d --%s-target CONTINUE", X_MARK, QOS_RULE_VID, mark, mark, MOD_MARK, mark);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						break;
					case 0:
						snprintf(cl_act, sizeof(cl_act), " %s -j %s --set-mark %d --mark-target CONTINUE", QOS_RULE, mark, MOD_MARK);
						retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
						break;
				}
				break;
	
		}
		
		snprintf(cl_act, sizeof(cl_act), " --mark %d/%d -j ACCEPT", MOD_MARK, MOD_MARK);
		retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, ebt_oper, cl_chName, -1, cl_act, &b, rule_idx++);
	}

	send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_MODIFY);

	if(CL_ACT(clcfg) & CL_ACTION_VLAN) {
		if(bLanIng) {
			MAX_QUEUES=MAX_QUEUES_IN_US;
		}
		else if(bWanIng) {
			MAX_QUEUES=MAX_QUEUES_IN_DS;
		}

		/* convert prio of format 1-8 into 8-1 because software prio descending order is 8-1. */
		if(CL_ACT(clcfg) & CL_ACTION_TC) {
			nf_Mark = -qId + MAX_QUEUES;
		}
		else {
			nf_Mark = cl_in8021p - 1; /* incoming 1p is 1-8. */
		}

		/* Setting Outgoing 8021p value using vconfig. */
		if((strnlen_s(CL_RXIF(clcfg), MAX_IF_NAME_LEN) > 0) && (strnlen_s(CL_RXIF(clcfg), MAX_IF_NAME_LEN) < (sizeof(DEFINTF) -1 ))) {
			strncpy_s(DEFINTF, MAX_IF_NAME_LEN, CL_RXIF(clcfg), strnlen_s(CL_RXIF(clcfg), MAX_IF_NAME_LEN));
		}
		else {
			/* no VLAN interfaces on LAN as of now.
			   use default interface on WAN, if it's VLAN based.
			*/
		}
		if(nf_Mark != -1 && strnlen_s(DEFINTF, MAX_IF_NAME_LEN)) {
			/* TODO have this as system API since proc entry can change. */
			snprintf(cmd, sizeof(cmd), "/proc/net/vlan/%s", DEFINTF);
			if (access(cmd, F_OK | R_OK) == 0) {
				snprintf(cmd, sizeof(cmd), "vconfig set_egress_map %s %d %d 2> /dev/null", DEFINTF, nf_Mark, cl_out1p); /* nf_Mark is 1-8 and skb->prio is 0-7 */
				RUN_CMD(cmd)
			}
		}
	}
	
lbl:
	return retval;
}

/* =============================================================================
 *  Function Name : set_ebt_ipt_cmd_wrap
 *  Description   : Wrapper API to prepare ebtables/iptables rule
 * ============================================================================*/
int32_t set_ebt_ipt_cmd_wrap(ipt_tbl_t tbl, iptebt_ops_t op, char *chName, int32_t order, char *cl_act, struct blob_buf *b, uint32_t rule_idx)
{
	int32_t nRet = LTQ_SUCCESS;
	char8 cmd[MAX_DATA_LEN];
	char8 *tbl_name = NULL;

	switch(tbl) {
		case IPT_QOS_TBL:
			tbl_name = IPT_QOS_TBL_NAME;
			break;
		case EBT_QOS_TBL:
			tbl_name = NULL;
			break;
		default:
			return LTQ_FAIL;
	}

	if ((b == NULL) || (chName == NULL)) {
		LOGF_LOG_ERROR("Configuration command is null!\n");
		return LTQ_FAIL;
	}

	memset_s(cmd, sizeof(cmd), 0);

	switch(op) {
		case CH_READ:
			/* exmaple : iptables -t mangle -L CL_LIST. */
			/* exmaple : ebtables -L CL_LIST. */
			// nRet = set_ebt_ipt_cmd(tbl_name, "-L", chName, NULL, b, rule_idx);
			LOGF_LOG_INFO("Read not supported!\n");
			break;
		case CH_CREAT:
			/* exmaple : iptables -t mangle -N CL_LIST. */
			/* exmaple : ebtables -N CL_LIST. */
			nRet = set_ebt_ipt_cmd(tbl_name, "N", chName, NULL, b, rule_idx);
			break;
		case CH_REM:
			/* exmaple : iptables -t mangle -X CL_LIST. */
			/* exmaple : ebtables -X CL_LIST. */
			nRet = set_ebt_ipt_cmd(tbl_name, "X", chName, NULL, b, rule_idx);
			break;
		case CH_FL:
			/* exmaple : iptables -t mangle -F CL_LIST. */
			/* exmaple : ebtables -F CL_LIST. */
			nRet = set_ebt_ipt_cmd(tbl_name, "F", chName, NULL, b, rule_idx);
			break;
		case RULE_INS:
			/* exmaple : iptables -t mangle -I CL_LIST 2 -j LAN_CL. */
			/* exmaple : ebtables -I CL_LIST 2 -j LAN_CL. */
			if (order == -1)
				snprintf(cmd, sizeof(cmd), "%s", cl_act);
			else
				snprintf(cmd, sizeof(cmd), "%d %s", order, cl_act);
			nRet = set_ebt_ipt_cmd(tbl_name, "I", chName, cmd, b, rule_idx);
			break;
		case RULE_APP:
			/* exmaple : iptables -t mangle -A CL_LIST -j LAN_CL. */
			/* exmaple : ebtables -A CL_LIST -j LAN_CL. */
			nRet = set_ebt_ipt_cmd(tbl_name, "A", chName, cl_act, b, rule_idx);
			break;
		case RULE_DEL:
			/* exmaple : iptables -t mangle -D CL_LIST -j LAN_CL. */
			/* exmaple : ebtables -D CL_LIST -j LAN_CL. */
			nRet = set_ebt_ipt_cmd(tbl_name, "D", chName, cl_act, b, rule_idx);
			break;
	}

	return nRet;
}

/* =============================================================================
 *  Function Name : set_ebt_ipt_cmd
 *  Description   : API to append ebtables/iptables rule in ubus message
 * ============================================================================*/
int32_t set_ebt_ipt_cmd(char *tbl, char *act, char *clname, char *cmd, struct blob_buf *b, uint32_t rule_idx)
{
	char8 cl_cmd[MAX_DATA_LEN];
	char8 rule_name[MAX_DATA_LEN];

	memset_s(cl_cmd, sizeof(cl_cmd), 0);
	memset_s(rule_name, sizeof(rule_name), 0);

	if ((b == NULL) || (act == NULL) || (clname == NULL)) {
		LOGF_LOG_ERROR("Configuration command is null!\n");
		return LTQ_FAIL;
	}

	if (tbl != NULL)
		snprintf(cl_cmd, sizeof(cl_cmd), "%s -%s %s %s -t %s -w", IPT, act, clname, (cmd == NULL)?"":cmd, tbl);
	else
		snprintf(cl_cmd, sizeof(cl_cmd), "%s -%s %s %s", EBT, act, clname, (cmd == NULL)?"":cmd);

	snprintf(rule_name, sizeof(rule_name), "rule[%d]", rule_idx);
	LOGF_LOG_INFO("%s: [%s]\n", rule_name, cl_cmd);

	blobmsg_add_string(b, rule_name, cl_cmd);

	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : send_iptable_msg
 *  Description   : API to send ubus message to firewalld to set iptables rules
 * ============================================================================*/
int32_t send_iptable_msg(struct blob_buf b, char *ubus_header)
{
	ENTRY
	static struct ubus_context *ctx = NULL;
	unsigned int unId = 0;
	ctx = ubus_connect(NULL);
	if (ctx == NULL) {
		LOGF_LOG_ERROR("ubus connect failed\n");
		goto exit_lbl;
	}

	if (ubus_header == NULL) {
		LOGF_LOG_ERROR("ubus header is missing\n");
		goto exit_lbl;
	}

	if (ubus_lookup_id(ctx, "firewalld", &unId)) {
		LOGF_LOG_ERROR("ubus look-up failed\n");
		goto exit_lbl;
	}

	ubus_invoke(ctx, unId, ubus_header, b.head, NULL, 0, 300);
	ubus_free(ctx);

exit_lbl:
	EXIT
	return LTQ_SUCCESS;
}
