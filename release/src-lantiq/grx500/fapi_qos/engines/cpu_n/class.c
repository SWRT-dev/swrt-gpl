/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "qosal_inc.h"
#include <qosal_utils.h>
#include "cpu_n_eng.h"

extern cl_adapt_t cl_adapt;
int32_t cpu_classAdapt(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags);
static char *sDefLanIf = "br-lan";

/* =============================================================================
 *  Function Name : cpu_classAdd
 *  Description   : Classifier add callback of CPU-N engine.
		    This API will add classifier for upstream in software.
		    Assumes that rx_if is present. in case user has given * or NULL in this field, 
		    then SL/scripts in turn have to call FAPI for each available interface in system.
		    Both ipt and ebt are under same engine. If any vlan header fields are specified
		    then call ebtables adapter first for matching those fields (partial cl).
		    And then call ip/ip6tables adapter. TODO
 * ============================================================================*/
int32_t cpu_classAdd(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags)
{
	UNUSED(ifinggrp);
	/* decide which adapter to use.
	   if bridged then ebtables, otherwise see below.
	   if (src/dst) IP address is IPv6 then ip6tables.
	   else if (src/dst) IP address is IPv4 then iptables.
	   else both iptables and ip6tables.
	   assumption is, if src and dst will be of same address
	   family if specified.
	*/
#if 0 /* TODO ip type param not available in CL struct */
	if(CL_SIP_OUT_EX(clcfg) != 1 && CL_SIP_OUT(clcfg) > 0) {
		nAdapter |= clcfg->filter.outer_ip.src_ip.type;
	}
	else if(CL_DIP_OUT_EX(clcfg) != 1 && CL_DIP_OUT(clcfg) > 0) {
		nAdapter |= clcfg->filter.outer_ip.dst_ip.type;
	}
	else {
		/* no IP address (v4/v6) in criteria. use both
		   ipv4 and ipv6 adapters for configuration. */
		nAdapter |= IPV4_ADDR;
		nAdapter |= IPV6_ADDR;
	}

	switch(nAdapter) {
		case IPV4_ADDR:
			cpu_classAdapt(clcfg, flags);
			break;
		case IPV6_ADDR:
			cpu_ipv6Adapt(clcfg, flags);
			break;
		default:
			/* invoke all supported adapters. */
			cpu_classAdapt(clcfg, flags);
			cpu_ipv6Adapt(clcfg, flags);
			break;
	}
#else
	int32_t iRet = LTQ_SUCCESS;
	flags |= CL_ADD;
	iRet = cpu_classAdapt(iftype, order, clcfg, flags);
	if(iRet != LTQ_SUCCESS) {
		if(iRet != QOS_CL_POLICER_UNSUP_PARAM) {
			iRet = QOS_CPU_ENG_CFG_FAIL;
		}
		LOGF_LOG_ERROR("Failed to configure iptables rule for classifier [%s].\n", clcfg->filter.name);
		goto lbl;
	}

lbl:
	return iRet;
#endif
}

/* =============================================================================
 *  Function Name : cpu_classDel
 *  Description   : Classifier delete callback of CPU-N engine.
		    This API will delete classifier for upstream in software.
		    Assumes that rx_if is present. in case user has given * or NULL in this field, 
		    then SL/scripts in turn have to call FAPI for each available interface in system.
		    Both ipt and ebt are under same engine. If any vlan header fields are specified
		    then call ebtables adapter first for matching those fields (partial cl).
		    And then call ip/ip6tables adapter. TODO
 * ============================================================================*/
int32_t cpu_classDel(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags)
{
	UNUSED(ifinggrp);
	/* decide which adapter to use.
	   if bridged then ebtables, otherwise see below.
	   if (src/dst) IP address is IPv6 then ip6tables.
	   else if (src/dst) IP address is IPv4 then iptables.
	   else both iptables and ip6tables.
	   assumption is, if src and dst will be of same address
	   family if specified.
	*/
#if 0 /* TODO ip type param not available in CL struct */
	if(CL_SIP_OUT_EX(clcfg) != 1 && CL_SIP_OUT(clcfg) > 0) {
		nAdapter |= clcfg->filter.outer_ip.src_ip.type;
	}
	else if(CL_DIP_OUT_EX(clcfg) != 1 && CL_DIP_OUT(clcfg) > 0) {
		nAdapter |= clcfg->filter.outer_ip.dst_ip.type;
	}
	else {
		/* no IP address (v4/v6) in criteria. use both
		   ipv4 and ipv6 adapters for configuration. */
		nAdapter |= IPV4_ADDR;
		nAdapter |= IPV6_ADDR;
	}

	switch(nAdapter) {
		case IPV4_ADDR:
			cpu_classAdapt(clcfg, flags);
			break;
		case IPV6_ADDR:
			cpu_ipv6Adapt(clcfg, flags);
			break;
		default:
			/* invoke all supported adapters. */
			cpu_classAdapt(clcfg, flags);
			cpu_ipv6Adapt(clcfg, flags);
			break;
	}
#else
	 if(iftype == QOS_IF_CATEGORY_ETHLAN && (clcfg->action.iflags & QOS_Q_F_LINK_STATE_CHANGE)) {
        /* check if all LAN ports are down */
        if (strncmp(sDefLanIf, clcfg->filter.rx_if, strnlen_s(clcfg->filter.rx_if, MAX_IF_NAME_LEN)) == 0) {
                LOGF_LOG_INFO("Classifier Deletion is not Possible, Since One or Other LAN ports are UP & individual deletion is not supported..!!");
                return QOS_CFG_DENY_FROM_LINK_CHK;
        }
    }

	int32_t iRet = LTQ_SUCCESS;
	flags |= CL_DEL;
	iRet = cpu_classAdapt(iftype, order, clcfg, flags);
	if(iRet != LTQ_SUCCESS) {
		iRet = QOS_CPU_ENG_CFG_FAIL;
		LOGF_LOG_ERROR("Failed to configure iptables rule for classifier [%s].\n", clcfg->filter.name);
		goto lbl;
	}

lbl:
	return iRet;
#endif
}

/* =============================================================================
 *  Function Name : cpu_classModify
 *  Description   : Classifier modify callback of CPU-N engine.
		    This API calls Delete and Add APIs to implement modify functionality.
 * ============================================================================*/
int32_t cpu_classModify(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t old_order, qos_class_cfg_t *old_clcfg, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags)
{
	int32_t iRet = LTQ_SUCCESS;
	
	iRet = cpu_classDel(iftype, ifinggrp, old_order, old_clcfg, flags);

	if (iRet == LTQ_SUCCESS)
		iRet = cpu_classAdd(iftype, ifinggrp, order, clcfg, flags);

	return iRet;
}
/* =============================================================================
 *  Function Name : cpu_classAdapt
 *  Description   : wrapper API to abstract various adapters to configure classifier.
 *  		    adapters (like iptables command, iptables API, tc command, ..)
 *  		    need to be hooked up to this API, and statically specify which
 *  		    adapter to be used.
 * ============================================================================*/
int32_t cpu_classAdapt(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags)
{
	int32_t iRet = LTQ_SUCCESS;
	bool revert = false;

	if (CL_ACT(clcfg) & CL_ACTION_POLICER) {
		cl_adapt = CL_ADAPT_TC;
	}

again:
	switch(cl_adapt) {
		case CL_ADAPT_IPTEBT:
			/* call iptables/ebtables adapter. */
			if ((flags & CL_FILTER_ORDER_UPDATE) == 0)
				iRet = iptebtAdapt(iftype, order, clcfg, flags);
			break;
		case CL_ADAPT_TC:
			/* place holder to call 'tc' adapter. */
			iRet = tcAdapt(iftype, order, clcfg, flags);
			cl_adapt = CL_ADAPT_IPTEBT;
			if (iRet != LTQ_SUCCESS) {
				break;
			}
			/* Disable Acceleration for this filter. */
			if((CL_ACT(clcfg) & CL_ACTION_ACCELERATION) == 0) {
				clcfg->action.flags |= CL_ACTION_ACCELERATION;
				clcfg->action.accl = 1;
				revert = true;
			}
			goto again;

		default:
			break;
	}

	if (revert == true) {
		clcfg->action.flags &= (~CL_ACTION_ACCELERATION);
		clcfg->action.accl = 0;
	}

	return iRet;
}

