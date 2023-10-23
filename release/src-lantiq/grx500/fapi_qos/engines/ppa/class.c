/******************************************************************************

                               Copyright (c) 2014
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
//#include <netdb.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ezxml.h>

#include "sysapi.h"
#include "qosal_inc.h"
#include "qosal_engine.h"
#include <qosal_utils.h>
#include "ppa_eng.h"
#include <sys/ioctl.h>
#include <net/ppa_api.h>

#define MAX_QUEUES_IN_US 8
#define MAX_QUEUES_IN_DS 4
#define PPA_DEVICE   "/dev/ifx_ppa"

#define INGRESS_PCE_LIST "INGRESS_HW_CL_LIST"

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

extern ifinggrp_t xml_lookup_ingress_group(const char *ifname);

extern int32_t vlanPrioUpdate(char *sIface, int32_t inPrio, int32_t out8021p, uint32_t flags);
extern int32_t nMapInUse, nDefVlanPrioCnt;
extern vlanMap xVlanMap[DEFAULT_TC];

static int8_t pceRuleAdd(uint8_t node, uint8_t nORDER,qos_class_cfg_t *clcfg,iftype_t iftype,ifinggrp_t ifinggrp,uint32_t flags);
static int8_t pceRuleDel(uint8_t node, uint8_t nORDER,qos_class_cfg_t *clcfg,iftype_t iftype,ifinggrp_t ifinggrp,uint32_t flags);

int32_t ppa_del_def_wmm_dscp_class(void)
{
	ENTRY
#ifdef PLATFORM_XRX500
	return qos_call_ioctl(PPA_DEVICE, PPA_CMD_QOS_DSCP_CLASS_RESET, NULL);
#else
	return LTQ_SUCCESS;
#endif
}
int32_t ppa_add_def_wmm_dscp_class(void)
{
	ENTRY
#ifdef PLATFORM_XRX500
	return qos_call_ioctl(PPA_DEVICE, PPA_CMD_QOS_DSCP_CLASS_SET, NULL);
#else
	return LTQ_SUCCESS;
#endif
}

static int32_t sw_convStrIPmask( char *ipmask_adr_str, unsigned char *ipmask_adr_ptr)
{
   char *str_ptr=ipmask_adr_str;
   char *endptr;
   int32_t i;
   unsigned long int val;

   uint32_t mask=0xff00;

   if (strnlen_s(ipmask_adr_str, IP_ADDRSTRLEN) == 0)
   {  
      mask|=0xff00;
      printf("No Valid Ip mask! \n");
      return mask;
   }

   for (i=0; i<4; i++)
   {
      val = strtoul(str_ptr, &endptr, 10);
      if ((*endptr != 0) && (*endptr != '.') && (*endptr != '-'))
            return 0;
      *(ipmask_adr_ptr+i)= (val & 0xFF);
      str_ptr = endptr+1;
	if(*(ipmask_adr_ptr+i) == 0)
	{
		int j=3-i;
		mask|=(3<<(j*2));
		mask|=0xff00;
	}
   }
	
   return mask;
}

static int32_t sw_convStrMacmask( char *macmask_adr_str, unsigned char *macmask_adr_ptr)
{
   char *str_ptr=macmask_adr_str;
   char *endptr;
   int32_t i;
   unsigned long int val;

   uint32_t mask=0;
   if (strnlen_s(macmask_adr_str, MAX_MACADDR_LEN) == 0)
   {  
      mask|=0xff00;
      printf("No Valid Mac mask! \n");
      return mask;
   }
   if (strnlen_s(macmask_adr_str, MAX_MACADDR_LEN) != (12+5))
   {  
      printf("ERROR: Invalid length of address string!\n");
      return 0;
   }

   for (i=0; i<6; i++)
   {
      val = strtoul(str_ptr, &endptr, 16);
      if ((*endptr != 0) && (*endptr != ':') && (*endptr != '-'))
            return 0;
      *(macmask_adr_ptr+i)= (val & 0xFF);
      str_ptr = endptr+1;
	if(*(macmask_adr_ptr+i) == 0)
	{
		int j=5-i;
		mask|=(3<<(j*2));
		mask|=0xff00;
	}
   }
	
   return mask;
}

static int32_t sw_convStrMac( char *mac_adr_str, unsigned char *mac_adr_ptr )
{
   char *str_ptr=mac_adr_str;
   char *endptr;
   int32_t i;
   unsigned long int val;

   if (strnlen_s(mac_adr_str, MAX_MACADDR_LEN) != (12+5))
   {  
      printf("ERROR: Invalid length of address string!\n");
      return 0;
   }

   for (i=0; i<6; i++)
   {
      val = strtoul(str_ptr, &endptr, 16);
      if ((*endptr != 0) && (*endptr != ':') && (*endptr != '-'))
            return 0;
      *(mac_adr_ptr+i)= (unsigned char) (val & 0xFF);
      str_ptr = endptr+1;
   }

   return 1;
}

/* ===================================================================================
 *  Function Name : clcfg_setParamIncExc
 *  Description   : API to set corresponding bit in bitmap including all parameters
 *  			indicating specified parameter has proper value.
 * ===================================================================================*/
static void clcfg_setParamIncExc(int32_t exclude_req, int32_t param , cl_param_in_ex_t *pxparaminex)
{
	pxparaminex->param_in |= param;

	if(exclude_req)
		pxparaminex->param_ex |= param ;

	return;
}

static void clcfg_setPort(char *sPort, int32_t *start_port, int32_t *end_port) {
	if (sPort == NULL)
		return;

	char *token, *next_token;
	rsize_t strmax = strnlen_s(sPort, 128);

	token = strtok_s(sPort, &strmax, ":", &next_token);
	if (token != NULL) {
		*start_port = (int32_t)strtoul(token, NULL, 10);
		*end_port = (int32_t)strtoul(next_token, NULL, 10);
	}
	return;
}

static int32_t get_clcfg_from_xml(qos_class_cfg_t *cl_cfg, uint32_t cfg_flags)
{
	ENTRY

	ezxml_t qos_ctxt, clnode_ctxt, node_ctxt;
	qos_ctxt = clnode_ctxt = node_ctxt = NULL;
	int32_t i, nRet, cl_count, j, k;
	cl_count = nRet = i = j = k = 0;
	const char *tmp_par;
	char *endptr;
	char sName[128] = {0};

	UNUSED(cfg_flags);

	qos_ctxt = ezxml_parse_file(QOS_DEF_Q_FILE);

	clnode_ctxt = ezxml_child(qos_ctxt, "dos-classifiers");

	if (clnode_ctxt == NULL) {
		goto exit_lbl;
	}
	tmp_par = ezxml_attr(clnode_ctxt, "classifier_count");

	if (tmp_par != NULL) {
		cl_count = atoi(tmp_par);
		if (cl_count <= 0 || cl_count > MAX_RULES) {
			goto exit_lbl;
		}
	}

	for (i = 0; i < cl_count; i++) {
		snprintf(sName, sizeof(sName), "dos-cl%d", (i + 1));
		clnode_ctxt = ezxml_child(qos_ctxt, sName);
		if (clnode_ctxt == NULL)
			continue;
		node_ctxt = ezxml_child(clnode_ctxt, "Name");
		if (node_ctxt != NULL) {
			strncpy_s(cl_cfg[i].filter.name, MAX_CL_NAME_LEN, node_ctxt->txt, strnlen_s(node_ctxt->txt, MAX_CL_NAME_LEN) + 1);
		}

		node_ctxt = ezxml_child(clnode_ctxt, "Order");
		if (node_ctxt != NULL) {
			cl_cfg[i].filter.order = (uint32_t)strtoul(node_ctxt->txt, &endptr, 10);
		}

		node_ctxt = ezxml_child(clnode_ctxt, "TrafficClass");
		if (node_ctxt != NULL) {
			cl_cfg[i].action.tc = (uint32_t)strtoul(node_ctxt->txt, &endptr, 10);
			cl_cfg[i].action.flags |= CL_ACTION_TC;
		}

		node_ctxt = ezxml_child(clnode_ctxt, "Prio");
		if (node_ctxt != NULL) {
			cl_cfg[i].action.priority = (uint32_t)strtoul(node_ctxt->txt, &endptr, 10);
		}

		node_ctxt = ezxml_child(clnode_ctxt, "Interface");
		if (node_ctxt != NULL) {
			strncpy_s(cl_cfg[i].filter.rx_if, MAX_IF_NAME_LEN, node_ctxt->txt, strnlen_s(node_ctxt->txt, MAX_IF_NAME_LEN) + 1);
		}

		node_ctxt = ezxml_child(clnode_ctxt, "QInterface");
		if (node_ctxt != NULL) {
			strncpy_s(cl_cfg[i].filter.tx_if, MAX_IF_NAME_LEN, node_ctxt->txt, strnlen_s(node_ctxt->txt, MAX_IF_NAME_LEN) + 1);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_TX_IF, &(cl_cfg[i].filter.param_in_ex));
		}

		node_ctxt = ezxml_child(clnode_ctxt, "SrcIP");
		if (node_ctxt != NULL) {
			strncpy_s(cl_cfg[i].filter.outer_ip.src_ip.ipaddr, IP_ADDRSTRLEN, node_ctxt->txt, strnlen_s(node_ctxt->txt, IP_ADDRSTRLEN) + 1);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_SRC_IP, &(cl_cfg[i].filter.outer_ip.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
		}

		node_ctxt = ezxml_child(clnode_ctxt, "SrcIP_Mask");
		if (node_ctxt != NULL) {
			strncpy_s(cl_cfg[i].filter.outer_ip.src_ip.mask, IP_ADDRSTRLEN, node_ctxt->txt, strnlen_s(node_ctxt->txt, IP_ADDRSTRLEN) + 1);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_SRC_IP, &(cl_cfg[i].filter.outer_ip.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
		}

		node_ctxt = ezxml_child(clnode_ctxt, "SrcIP_Exclude");
		if (node_ctxt != NULL) {
			cl_cfg[i].filter.outer_ip.src_ip.exclude = (uint32_t)strtoul(node_ctxt->txt, &endptr, 10);
			if (cl_cfg[i].filter.outer_ip.src_ip.exclude == QOSAL_ENABLE) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP_SRC_IP, &(cl_cfg[i].filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
			}
		}

		node_ctxt = ezxml_child(clnode_ctxt, "DestIP");
		if (node_ctxt != NULL) {
			strncpy_s(cl_cfg[i].filter.outer_ip.dst_ip.ipaddr, IP_ADDRSTRLEN, node_ctxt->txt, strnlen_s(node_ctxt->txt, IP_ADDRSTRLEN) + 1);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_DST_IP, &(cl_cfg[i].filter.outer_ip.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
		}

		node_ctxt = ezxml_child(clnode_ctxt, "DestIP_Mask");
		if (node_ctxt != NULL) {
			strncpy_s(cl_cfg[i].filter.outer_ip.dst_ip.mask, IP_ADDRSTRLEN, node_ctxt->txt, strnlen_s(node_ctxt->txt, IP_ADDRSTRLEN) + 1);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_DST_IP, &(cl_cfg[i].filter.outer_ip.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
		}

		node_ctxt = ezxml_child(clnode_ctxt, "DestIP_Exclude");
		if (node_ctxt != NULL) {
			cl_cfg[i].filter.outer_ip.dst_ip.exclude = (uint32_t)strtoul(node_ctxt->txt, &endptr, 10);
			if (cl_cfg[i].filter.outer_ip.dst_ip.exclude == QOSAL_ENABLE) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP_DST_IP, &(cl_cfg[i].filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
			}
		}

		node_ctxt = ezxml_child(clnode_ctxt, "Protocol");
		if (node_ctxt != NULL) {
			cl_cfg[i].filter.outer_ip.ip_p = (int32_t)strtoul(node_ctxt->txt, &endptr, 10);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_PROTO, &(cl_cfg[i].filter.outer_ip.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
		}

		node_ctxt = ezxml_child(clnode_ctxt, "Protocol_Exclude");
		if (node_ctxt != NULL) {
			cl_cfg[i].filter.outer_ip.ip_p_exclude = (int32_t)strtoul(node_ctxt->txt, &endptr, 10);
			if (cl_cfg[i].filter.outer_ip.ip_p_exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP_PROTO, &(cl_cfg[i].filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
			}
		}

		node_ctxt = ezxml_child(clnode_ctxt, "SrcPorts");
		if (node_ctxt != NULL) {
			clcfg_setPort(node_ctxt->txt, &(cl_cfg[i].filter.outer_ip.src_port.start_port), &(cl_cfg[i].filter.outer_ip.src_port.end_port));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_SRC_PORT, &(cl_cfg[i].filter.outer_ip.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
		}

		node_ctxt = ezxml_child(clnode_ctxt, "SrcPorts_Exclude");
		if (node_ctxt != NULL) {
			cl_cfg[i].filter.outer_ip.src_port.exclude = (uint32_t)strtoul(node_ctxt->txt, &endptr, 10);
			if (cl_cfg[i].filter.outer_ip.src_port.exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP_SRC_PORT, &(cl_cfg[i].filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
			}
		}

		node_ctxt = ezxml_child(clnode_ctxt, "DstPorts");
		if (node_ctxt != NULL) {
			clcfg_setPort(node_ctxt->txt, &(cl_cfg[i].filter.outer_ip.dst_port.start_port), &(cl_cfg[i].filter.outer_ip.dst_port.end_port));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_DST_PORT, &(cl_cfg[i].filter.outer_ip.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
		}

		node_ctxt = ezxml_child(clnode_ctxt, "DstPorts_Exclude");
		if (node_ctxt != NULL) {
			cl_cfg[i].filter.outer_ip.dst_port.exclude = (uint32_t)strtoul(node_ctxt->txt, &endptr, 10);
			if (cl_cfg[i].filter.outer_ip.dst_port.exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP_DST_PORT, &(cl_cfg[i].filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP, &(cl_cfg[i].filter.param_in_ex));
			}
		}

		node_ctxt = ezxml_child(clnode_ctxt, "PPP_Protocol");
		if (node_ctxt != NULL) {
			if (strcmp(node_ctxt->txt, "LCP") == 0) {
				cl_cfg[i].filter.ppp_proto = 0xC021;
				clcfg_setParamIncExc(0, CLSCFG_PARAM_PPP_PROTO, &(cl_cfg[i].filter.param_in_ex));
			}
		}

		node_ctxt = ezxml_child(clnode_ctxt, "PPP_Protocol_Exclude");
		if (node_ctxt != NULL) {
			uint32_t ppp_exclude = (uint32_t)strtoul(node_ctxt->txt, &endptr, 10);
			if (ppp_exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_PPP_PROTO, &(cl_cfg[i].filter.param_in_ex));
			}
		}

		node_ctxt = ezxml_child(clnode_ctxt, "MeterID");
		if (node_ctxt != NULL) {
			cl_cfg[i].action.meter.id = (uint32_t)strtoul(node_ctxt->txt, &endptr, 10);
			cl_cfg[i].action.meter.action = 2;
			cl_cfg[i].action.flags |= CL_ACTION_METER;
		}

		node_ctxt = ezxml_child(clnode_ctxt, "CPUpacket");
		if (node_ctxt != NULL) {
			uint32_t CPUpacket = (uint32_t)strtoul(node_ctxt->txt, &endptr, 10);
			if (CPUpacket == 1) {
				cl_cfg[i].action.flags |= CL_ACTION_ONLY_CPU;
			}
		}

		node_ctxt = ezxml_child(clnode_ctxt, "Ethertype");
		if (node_ctxt != NULL) {
			cl_cfg[i].filter.ethhdr.eth_type = (int32_t)strtoul(node_ctxt->txt, NULL, 16);
			LOGF_LOG_DEBUG("Ethertype Read is :[%x]\n", cl_cfg[i].filter.ethhdr.eth_type);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR_TYPE, &(cl_cfg[i].filter.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR_TYPE, &(cl_cfg[i].filter.ethhdr.param_in_ex));
		}

		cl_cfg[i].filter.enable = 1;
	}

exit_lbl:
	EXIT
	return nRet;
}

static int8_t pceRuleAdd(uint8_t node, uint8_t nORDER, qos_class_cfg_t *clcfg,iftype_t iftype,ifinggrp_t ifinggrp,uint32_t flags)
{
	int32_t ret = LTQ_SUCCESS, i = 0, nCnt =0, nFound = 0;
	PPA_CMD_CLASSIFIER_INFO c_info;
	PPA_CMD_QOS_INGGRP_INFO g_info;
	unsigned char srcmac_mask[6],dstmac_mask[6],dstip_mask[6],srcip_mask[6];;
	uint32_t sw_mask=0;
	char *psEndptr = NULL, *psIP = NULL, *psIngIntf = NULL, *psEgrIntf = NULL;
	char base_tx_if[PPA_IF_NAME_SIZE], logical_tx_if[PPA_IF_NAME_SIZE];
	unsigned int uiVal=0;
	unsigned short *puIP = NULL;
 
	memset_s(&c_info, sizeof(PPA_CMD_CLASSIFIER_INFO), 0x00);
	memset_s(&g_info, sizeof(PPA_CMD_QOS_INGGRP_INFO), 0x00);
	UNUSED(flags);

	c_info.order = nORDER;
	c_info.in_dev = node; // Select PAE for classification
	c_info.category = (iftype == QOS_IF_CATEGORY_ETHLAN) ? CAT_USQOS : CAT_DSQOS;
	c_info.pattern.bEnable = 1;/* By default Enabled */
	if(strnlen_s(clcfg->filter.tx_if, MAX_IF_NAME_LEN)) {
		ret = qosd_if_base_get(clcfg->filter.tx_if, base_tx_if, logical_tx_if, 0);
		if(ret != LTQ_SUCCESS) {
			LOGF_LOG_ERROR("Not able to get base interface for queuing interface : [%s]\n", clcfg->filter.tx_if);
			return ret;
		}
	}
	psIngIntf = base_tx_if;
	psEgrIntf = clcfg->filter.rx_if;
	strncpy_s(c_info.tx_if, PPA_IF_NAME_SIZE, psIngIntf, PPA_IF_NAME_SIZE);
	strncpy_s(c_info.rx_if, PPA_IF_NAME_SIZE, psEgrIntf, PPA_IF_NAME_SIZE);

	if(clcfg->action.flags & CL_ACTION_TC_INGRESS)
	{
		c_info.action.flags |= PPA_QOS_Q_F_INGRESS;
		if (IFINGGRP_INVALID(ifinggrp)) {
			ifinggrp = xml_lookup_ingress_group(psIngIntf);
			if (IFINGGRP_INVALID(ifinggrp)) {
				LOGF_LOG_ERROR("Invalid XML ingress group\n");
				return -1;
			}
			LOGF_LOG_DEBUG("Using XML setting: iface %s belongs to ingress group %d\n", psIngIntf, ifinggrp);
		}
		c_info.category = CAT_INGQOS;
		strncpy_s(g_info.ifname, PPA_IF_NAME_SIZE, psIngIntf, PPA_IF_NAME_SIZE);
		g_info.ingress_group = (PPA_QOS_INGGRP)ifinggrp;
		ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_INGGRP,&g_info);
		if( ret != LTQ_SUCCESS ) {
			LOGF_LOG_ERROR("Ioctl for set Ingress QoS Failed with [%d]\n", ret);
			return -1;
		}
	}

	/* DSCP */
	if(CL_DSCP_SEL(clcfg) > 0) {
		c_info.pattern.bDSCP_Enable = 1;
		c_info.pattern.nDSCP = clcfg->filter.iphdr.dscp;
		if(CL_DSCP_EX(clcfg) > 0)
		c_info.pattern.bDSCP_Exclude = 1;
	}

	/* PCP */
	if(CL_VPRIO_SEL(clcfg) > 0) {
		c_info.pattern.bPCP_Enable=1;
		c_info.pattern.nPCP=clcfg->filter.ethhdr.prio;
		if(CL_VPRIO_EX(clcfg) > 0)
		c_info.pattern.bCTAG_PCP_DEI_Exclude = 1;	
	}

	/* source port */
	if(CL_O_SPORT_SEL(clcfg) > 0) {
		c_info.pattern.bAppDataMSB_Enable = 1;
		c_info.pattern.nAppDataMSB = clcfg->filter.outer_ip.src_port.start_port;
		if(clcfg->filter.outer_ip.src_port.end_port > 0) {
			c_info.pattern.bAppMaskRangeMSB_Select = 1;
			c_info.pattern.nAppMaskRangeMSB = abs((clcfg->filter.outer_ip.src_port.start_port) - (clcfg->filter.outer_ip.src_port.end_port));
		}
		if(CL_O_SPORT_EX(clcfg) > 0)
		c_info.pattern.bAppMSB_Exclude = 1;
	}

	/* destination port */
	if(CL_O_DPORT_SEL(clcfg) > 0) {
		c_info.pattern.bAppDataLSB_Enable = 1;
		c_info.pattern.nAppDataLSB = clcfg->filter.outer_ip.dst_port.start_port;
		if(clcfg->filter.outer_ip.dst_port.start_port > 0) {
			c_info.pattern.bAppMaskRangeLSB_Select = 1;
			c_info.pattern.nAppMaskRangeLSB = abs((clcfg->filter.outer_ip.dst_port.start_port) - (clcfg->filter.outer_ip.dst_port.end_port));
		}
		if(CL_O_DPORT_EX(clcfg) > 0)
		c_info.pattern.bAppLSB_Exclude = 1;
	}

	/* source IP address of outer header */
	if(CL_O_SIP_SEL(clcfg) > 0) {
		if(util_get_ipaddr_family(clcfg->filter.outer_ip.src_ip.ipaddr) == AF_INET) {
			c_info.pattern.eSrcIP_Select=1;
			c_info.pattern.nSrcIP.nIPv4 = inet_addr(clcfg->filter.outer_ip.src_ip.ipaddr);
			sw_mask = sw_convStrIPmask(clcfg->filter.outer_ip.src_ip.mask, srcip_mask);
			c_info.pattern.nSrcIP_Mask = sw_mask;
		}
		else {
			c_info.pattern.eSrcIP_Select=2;
			if (strnlen_s(clcfg->filter.outer_ip.src_ip.ipaddr, IP_ADDRSTRLEN) > (32+7)) {
				LOGF_LOG_ERROR("Classifier add failed. Invalid length of IP V6 source address [%s]\n", clcfg->filter.outer_ip.src_ip.ipaddr);
				return -1;
			}

			psIP = clcfg->filter.outer_ip.src_ip.ipaddr;
			puIP = c_info.pattern.nSrcIP.nIPv6;
			for (i=0; i<8; i++) {
				uiVal = strtoul(psIP, &psEndptr, 16);
				if (*psEndptr && (*psEndptr != ':'))
					break;

				*puIP++ = uiVal;
				psIP = psEndptr+1;
			}
			c_info.pattern.nSrcIP_Mask = 0xff00;
		}
		if(CL_O_SIP_EX(clcfg) > 0)
		c_info.pattern.bSrcIP_Exclude=1;	
	}

	/* destination IP address of outer header */
	if(CL_O_DIP_SEL(clcfg) > 0) {
		if(util_get_ipaddr_family(clcfg->filter.outer_ip.dst_ip.ipaddr) == AF_INET) {
			c_info.pattern.eDstIP_Select=1;
			c_info.pattern.nDstIP.nIPv4 = inet_addr(clcfg->filter.outer_ip.dst_ip.ipaddr);
			sw_mask = sw_convStrIPmask(clcfg->filter.outer_ip.dst_ip.mask, dstip_mask);
			c_info.pattern.nDstIP_Mask = sw_mask;
		}
		else {
			c_info.pattern.eDstIP_Select=2;
			if (strnlen_s(clcfg->filter.outer_ip.dst_ip.ipaddr, IP_ADDRSTRLEN) > (32+7)) {
				LOGF_LOG_ERROR("Classifier add failed. Invalid length of IP V6 destination address [%s]\n", clcfg->filter.outer_ip.dst_ip.ipaddr);
				return -1;
			}

			psIP = clcfg->filter.outer_ip.dst_ip.ipaddr;
			puIP = c_info.pattern.nDstIP.nIPv6;
			for (i=0; i<8; i++) {
				uiVal = strtoul(psIP, &psEndptr, 16);
				if (*psEndptr && (*psEndptr != ':'))
					break;

				*puIP++ = uiVal;
				psIP = psEndptr+1;
			}
			c_info.pattern.nDstIP_Mask = 0xff00;
		}
		if(CL_O_DIP_EX(clcfg) > 0)
		c_info.pattern.bDstIP_Exclude=1;	
	}

	/* protocol of outer header */
	if(CL_O_PROTO_SEL(clcfg) > 0) {
		c_info.pattern.bProtocolEnable=1;
		switch(clcfg->filter.outer_ip.ip_p)
		{
			case 1:
				c_info.pattern.nProtocol = 1;
				c_info.pattern.nProtocolMask = 0;
				break;
			case 2:
				c_info.pattern.nProtocol = 6;
				c_info.pattern.nProtocolMask = 6;
				break;
			case 3:
				c_info.pattern.nProtocol = 17;
				c_info.pattern.nProtocolMask = 17;
				break;
			case 4:
				c_info.pattern.nProtocol = 2;
				c_info.pattern.nProtocolMask = 0;
				break;
			default:
				/* Allow for all protocols if protocol not mentioned */
				c_info.pattern.bProtocolEnable=0;
				break;
		} 
		if(CL_O_PROTO_EX(clcfg) > 0)
		c_info.pattern.bProtocol_Exclude=1;
		
	}

	/* destination MAC address */
	if(CL_DMAC_SEL(clcfg) > 0) {
		c_info.pattern.bMAC_DstEnable=1;
		sw_convStrMac(clcfg->filter.dst_mac.mac_addr, c_info.pattern.nMAC_Dst);
		sw_mask = sw_convStrMacmask(clcfg->filter.dst_mac.mac_addr_mask, dstmac_mask);
		c_info.pattern.nMAC_DstMask = sw_mask;
		if(CL_DMAC_EX(clcfg) > 0)
		c_info.pattern.bDstMAC_Exclude=1;
	}

	/* source MAC address */
	if(CL_SMAC_SEL(clcfg) > 0) {
		c_info.pattern.bMAC_SrcEnable=1;
		sw_convStrMac(clcfg->filter.src_mac.mac_addr, c_info.pattern.nMAC_Src);
		sw_mask = sw_convStrMacmask(clcfg->filter.src_mac.mac_addr_mask, srcmac_mask);
		c_info.pattern.nMAC_SrcMask = sw_mask;
		if(CL_SMAC_EX(clcfg) > 0)
		c_info.pattern.bSrcMAC_Exclude=1;
	}

	/* Eth Type */
	if(CL_ETH_SEL(clcfg) > 0) {
		c_info.pattern.bEtherTypeEnable=1;
		c_info.pattern.nEtherType = clcfg->filter.ethhdr.eth_type;
		if(CL_ETH_EX(clcfg) > 0)
			c_info.pattern.bEtherType_Exclude=1;
	}
	
	/* VLAN Id */
	if(CL_VID_SEL(clcfg) > 0) {
		/* assumed that VLAN Id exists - either before or caller has created VLAN Id before this. */
		c_info.pattern.bVid=1;
		c_info.pattern.nVid=clcfg->filter.ethhdr.vlanid;
		if(CL_VID_EX(clcfg) > 0)
		c_info.pattern.bVid_Exclude=1;
	}
	
	if((clcfg->action.flags & CL_ACTION_TC) == CL_ACTION_TC) {
		c_info.action.qos_action.trafficclass = 2;
		if((clcfg->action.flags & CL_ACTION_TC_INGRESS_EGRESS) || (clcfg->action.flags & CL_ACTION_INGRESS_TC_SET)) {
			if(clcfg->action.tc_ing <= DEFAULT_TC) 
				c_info.action.qos_action.alt_trafficclass = (DEFAULT_TC - clcfg->action.tc_ing);
			else
				c_info.action.qos_action.alt_trafficclass = (MAX_TC_NUM + DEFAULT_TC - clcfg->action.tc_ing);
		} else {
			if(clcfg->action.tc <= DEFAULT_TC) 
				c_info.action.qos_action.alt_trafficclass = (DEFAULT_TC - clcfg->action.tc);
			else 
				c_info.action.qos_action.alt_trafficclass = (MAX_TC_NUM + DEFAULT_TC - clcfg->action.tc);
		}
#if 0
		if((clcfg->action.flags & CL_ACTION_DSCP) == CL_ACTION_DSCP) {
			/* use class to DSCP mapping.
			   PCE remarking doesn't have option to specify remark DSCP value.
			   so class to DSCP mapping has to be used. that means
			   DSCP remarking can't be supported if TC is not specified.
			*/
			c_info.action.qos_action.remark_dscp = 1;
			c_info.action.qos_action.new_dscp = clcfg->action.dscp;
		}
#endif
#if 1
		if((clcfg->action.flags & CL_ACTION_VLAN) == CL_ACTION_VLAN) {
			for(nCnt=0; nCnt < nMapInUse; nCnt++) {
				if(xVlanMap[nCnt].defIfVlanPrio != DEF_VLAN_PRIO) {
					flags |= CL_ADD;
					flags |= PPA_CALLER;
					ret = vlanPrioUpdate(xVlanMap[nCnt].sIf, clcfg->action.tc, clcfg->action.vlan.pcp, flags);
					nFound = 1;
				}
			}
			if(nFound != 1) {
				c_info.action.qos_action.remarkpcp = 1;
				c_info.action.qos_action.new_pcp = clcfg->action.vlan.pcp;
				c_info.action.qos_action.remark_stagpcp = 1;
				c_info.action.qos_action.new_stagpcp = clcfg->action.vlan.pcp;
				c_info.action.iftype = iftype;
			}
		}

#endif
	}

	/* PPP Protocol */
	if (CL_PPP_PROTO_SEL(clcfg) > 0) {
		c_info.pattern.bPPP_ProtocolEnable = 1;
		c_info.pattern.nPPP_Protocol = clcfg->filter.ppp_proto;
		c_info.pattern.nPPP_ProtocolMask = clcfg->filter.ppp_proto;

		if (CL_PPP_PROTO_EX(clcfg) > 0)
			c_info.pattern.bPPP_Protocol_Exclude = 1;
	}

	/* Meter Action */
	if (clcfg->action.flags & CL_ACTION_METER) {
		c_info.action.qos_action.meterid = clcfg->action.meter.id;
		c_info.action.qos_action.meter = clcfg->action.meter.action;
		c_info.category = CAT_INGQOS;
	}

#ifdef PLATFORM_XRX500
	/* Only CPU packets */
	if (clcfg->action.flags & CL_ACTION_ONLY_CPU) {
		c_info.action.flags |= PPA_QOS_CL_CPU;
	}
#endif

	ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_ADD_CLASSIFIER, &c_info);
	if( ret != LTQ_SUCCESS ) {
		LOGF_LOG_ERROR("Ioctl for add QoS Classifier Failed with [%d]\n", ret);
		ret = QOS_PPA_ENG_CFG_FAIL;
	}

	return ret;

}

/* =============================================================================
 *  Function Name : pceRuleDel
 *  Description   : Removes specfied PCE rule in switch.
 * ============================================================================*/
static int8_t pceRuleDel(uint8_t node, uint8_t nORDER,qos_class_cfg_t *clcfg,iftype_t iftype,ifinggrp_t ifinggrp,uint32_t flags)
{
	int32_t ret=LTQ_SUCCESS, nFound = 0, nCnt = 0;
	PPA_CMD_CLASSIFIER_INFO c_info;
 
	memset_s(&c_info, sizeof(PPA_CMD_CLASSIFIER_INFO), 0x00);

	UNUSED(flags);
	UNUSED(ifinggrp);
	c_info.order = nORDER;
	c_info.in_dev = node; // Select PAE for classification
	if(iftype == QOS_IF_CATEGORY_ETHLAN)
	c_info.category = CAT_USQOS; // Select QoS Category.
	else
	c_info.category = CAT_DSQOS; // Select QoS Category.
	c_info.action.iftype = iftype;
	if((clcfg->action.flags & CL_ACTION_TC) == CL_ACTION_TC) {
		c_info.action.qos_action.trafficclass = 2;

		if((clcfg->action.flags & CL_ACTION_TC_INGRESS_EGRESS) || (clcfg->action.flags & CL_ACTION_INGRESS_TC_SET)) {
			if(clcfg->action.tc_ing <= DEFAULT_TC) 
				c_info.action.qos_action.alt_trafficclass = (DEFAULT_TC - clcfg->action.tc_ing);
			else
				c_info.action.qos_action.alt_trafficclass = (MAX_TC_NUM + DEFAULT_TC - clcfg->action.tc_ing);
		} else {
			if(clcfg->action.tc <= DEFAULT_TC) 
				c_info.action.qos_action.alt_trafficclass = (DEFAULT_TC - clcfg->action.tc);
			else 
				c_info.action.qos_action.alt_trafficclass = (MAX_TC_NUM + DEFAULT_TC - clcfg->action.tc);
		}
#if 0
		if((clcfg->action.flags & CL_ACTION_DSCP) == CL_ACTION_DSCP) {
			/* use class to DSCP mapping.
			   PCE remarking doesn't have option to specify remark DSCP value.
			   so class to DSCP mapping has to be used. that means
			   DSCP remarking can't be supported if TC is not specified.
			*/
			c_info.action.qos_action.remark_dscp = 1;
			c_info.action.qos_action.new_dscp = clcfg->action.dscp;
		}
#endif
#if 1
		if((clcfg->action.flags & CL_ACTION_VLAN) == CL_ACTION_VLAN) {
			for(nCnt=0; nCnt < nMapInUse; nCnt++) {
				flags |= CL_DEL;
				flags |= PPA_CALLER;
				ret = vlanPrioUpdate(xVlanMap[nCnt].sIf, clcfg->action.tc, clcfg->action.vlan.pcp, flags);
				nFound = 1;
			}
			if(nFound != 1) {
				c_info.action.qos_action.remarkpcp = 1;
				c_info.action.qos_action.new_pcp = clcfg->action.vlan.pcp;
				c_info.action.iftype = iftype;
			}
		}
#endif
	}
	if(clcfg->action.flags & CL_ACTION_TC_INGRESS)
	{
		c_info.action.flags |= PPA_QOS_Q_F_INGRESS;
		c_info.category = CAT_INGQOS;
	} 

	ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_DEL_CLASSIFIER,&c_info);
	if( ret != LTQ_SUCCESS ) {
		LOGF_LOG_ERROR("Ioctl for delete QoS Classifier Failed with [%d]\n", ret);
		ret = QOS_PPA_ENG_CFG_FAIL;
	}
	return ret;
}

int32_t ppa_eng_class_add(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags)
{
	int32_t ret=LTQ_SUCCESS;

	//UNUSED(flags);

	ret = pceRuleAdd(0,order,clcfg,iftype,ifinggrp,flags);
	
	if(ret != LTQ_SUCCESS) {
		ret = QOS_PPA_ENG_CFG_FAIL;
		LOGF_LOG_ERROR("PCE rule configuration in PPA failed for classifier [%s]\n", clcfg->filter.name);
		goto sw_lbl;
	}
	if ((clcfg->action.flags & CL_ACTION_TC_INGRESS_EGRESS) || (clcfg->action.flags & CL_ACTION_INGRESS_TC_SET)) {
		ingress_ipt_config(iftype, order, clcfg, flags);
	}

sw_lbl:
	return ret;
}

int32_t ppa_eng_class_delete(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags)
{
	int32_t ret=LTQ_SUCCESS;

	UNUSED(flags);

	ret = pceRuleDel(0,order,clcfg,iftype,ifinggrp,flags);
	
	if(ret != LTQ_SUCCESS) {
		ret = QOS_PPA_ENG_CFG_FAIL;
		LOGF_LOG_ERROR("PCE rule configuration in PPA failed for classifier [%s]\n", clcfg->filter.name);
		goto sw_lbl;
	}
	if ((clcfg->action.flags & CL_ACTION_TC_INGRESS_EGRESS) || (clcfg->action.flags & CL_ACTION_INGRESS_TC_SET)) {
		flags |= CL_DEL;
                ingress_ipt_config(iftype, order, clcfg, flags);
        }

sw_lbl:
	return ret;
}

int32_t ppa_eng_class_modify(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t old_order, qos_class_cfg_t *old_clcfg, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags)
{
	int32_t ret=LTQ_SUCCESS;

	ret = ppa_eng_class_delete(iftype, ifinggrp, old_order, old_clcfg, flags);

	if(ret != LTQ_SUCCESS) {
		goto sw_lbl;
	}

	ret = ppa_eng_class_add(iftype, ifinggrp, order, clcfg, flags);

	if(ret != LTQ_SUCCESS) {
		goto sw_lbl;
	}

sw_lbl:
	if(ret != LTQ_SUCCESS) {
		ret = QOS_PPA_ENG_CFG_FAIL;
		LOGF_LOG_ERROR("PCE rule configuration in PPA failed for classifier [%s]\n", clcfg->filter.name);
	}
	return ret;
}

int32_t dos_cl_cfg(uint32_t oper_flag) {
	ENTRY
	int32_t nRet = LTQ_SUCCESS;
	int i, cl_count;
	ezxml_t qos_ctxt, clnode_ctxt;
	ezxml_t node_ctxt;
	cl_count = i = 0;
	qos_class_cfg_t *cl_cfg = NULL;
	const char *tmp_par;
	char sName[MAX_Q_NAME_LEN] = {0};
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;

	qos_ctxt = ezxml_parse_file(QOS_DEF_Q_FILE);

	clnode_ctxt = ezxml_child(qos_ctxt, "dos-classifiers");
	if(clnode_ctxt == NULL) {
		goto exit_lbl;
	}
	tmp_par = ezxml_attr(clnode_ctxt, "classifier_count");
	if (tmp_par != NULL) {
		cl_count = atoi(tmp_par);
		if (cl_count <= 0 || cl_count > MAX_RULES) {
			goto exit_lbl;
		}
	}

	cl_cfg = (qos_class_cfg_t *)os_calloc(1, (cl_count * sizeof(qos_class_cfg_t)));
	if (cl_cfg == NULL) {
		LOGF_LOG_CRITICAL("Memory Allocation Failed\n");
		nRet = LTQ_FAIL;
		goto exit_lbl;
	}
	get_clcfg_from_xml(cl_cfg, DOS_QOS_CFG);

	for(i = 0; i < cl_count; i++)
	{
		snprintf(sName, sizeof(sName), "dos-cl%d", (i + 1));
		clnode_ctxt = ezxml_child(qos_ctxt, sName);
		if(clnode_ctxt == NULL) {
			continue;
		}

		node_ctxt = ezxml_child(clnode_ctxt, "IfaceCat");
		if (node_ctxt != NULL) {
			iftype = (iftype_t) strtoul(node_ctxt->txt, NULL, 10);
		}

		node_ctxt = ezxml_child(clnode_ctxt, "IngressGrp");
		if (node_ctxt != NULL) {
			ifinggrp = (iftype_t) strtoul(node_ctxt->txt, NULL, 10);
			cl_cfg[i].action.flags |= CL_ACTION_TC_INGRESS;
		}

		if (oper_flag == QOSAL_ENABLE)
			pceRuleAdd(0, cl_cfg[i].filter.order, &cl_cfg[i], iftype, ifinggrp, 0);
//		else if (oper_flag == QOSAL_DISABLE)
//			delete_qos_queue(ifname, iftype, cl_cfg[i].inggrp, &cl_cfg[i], cl_cfg[i].queue_id);
	}
	os_free(cl_cfg);
exit_lbl:

	EXIT
	return nRet;
}
