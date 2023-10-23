#ifndef __PPA_API_COMMON_H__20100203__1740__
#define __PPA_API_COMMON_H__20100203__1740__
/*******************************************************************************
**
** FILE NAME	: ppa_api_common.h
** PROJECT	: PPA
** MODULES	: PPA Common header file
**
** DATE		: 3 NOV 2008
** AUTHOR	: Xu Liang
** DESCRIPTION	: PPA Common Header File
** COPYRIGHT	: Copyright (c) 2017 Intel Corporation
** Copyright (c) 2010 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
** HISTORY
** $Date        $Author         $Comment
** 03 NOV 2008  Xu Liang        Initiate Version
*******************************************************************************/
#define NO_DOXY	 		1
#ifndef PPA_DSLITE /*if not defined in kernel's .configure file, then use local's definition*/
#define PPA_DSLITE		1
#endif
#ifndef RTP_SAMPLING_ENABLE	 /*if not defined in kernel's .configure file, then use local's definition*/
#define RTP_SAMPLING_ENABLE	1
#endif
#ifndef CAP_WAP_CONFIG	 /*if not defined in kernel's .configure file, then use local's definition*/
#define CAP_WAP_CONFIG		1
#endif
#ifndef L2TP_CONFIG	 /*if not defined in kernel's .configure file, then use local's definition*/
#define L2TP_CONFIG		1
#endif
#ifndef MBR_CONFIG	 /*if not defined in kernel's .configure file, then use local's definition*/
#define MBR_CONFIG		1
#endif
#ifndef QOS_AL_CONFIG	 /*if not defined in kernel's .configure file, then use local's definition*/
#define QOS_AL_CONFIG		1
#endif
#ifndef PPA_MFE		/*if not defined in kernel's .configure file, then use local's definition*/
#define PPA_MFE			0
#endif
#ifndef WMM_QOS_CONFIG	 /*if not defined in kernel's .configure file, then use local's definition*/
#define WMM_QOS_CONFIG		1
#endif
#ifndef PPA_API_PROC		/*if not defined in kernel's .configure file, then use local's definition*/
#define PPA_API_PROC		1
#endif
/*force dynamic ppe driver's module parameter */
#define PPA_DP_DBG_PARAM_ENABLE	1	 /*for PPA automation purpose. for non-linux os porting, just disable it*/
#define PPA_IF_MIB 		1	 /*Flag to enable/disable PPA software interface based mib counter*/
#define SESSION_STATISTIC_DEBUG 1 /*flag to enable session management statistics support*/
#define MPE_IFMIB		1
#if IS_ENABLED(CONFIG_IPV4_IPV6_COUNTER_SUPPORT)
#undef PPA_INTF_MIB_TIMER	/* Interface packet counters will be updated in other thread */
#else
#define PPA_INTF_MIB_TIMER	1
#endif

#endif
