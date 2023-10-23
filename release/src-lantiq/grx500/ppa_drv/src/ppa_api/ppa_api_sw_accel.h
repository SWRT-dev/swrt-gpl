#ifndef __PPA_API_SW_ACCEL_H__20130913
#define __PPA_API_SW_ACCEL_H__20130913
/*******************************************************************************
**
** FILE NAME    : ppa_api_sw_accel.h
** PROJECT      : PPA
** MODULES      : PPA API (Software Fastpath Implementation)
**
** DATE         : 12 Sep 2013
** AUTHOR       : Lantiq
** DESCRIPTION  : Function to bypass the linux stack for packets belonging to the PPA sessions which are not in PPE firmware.
** COPYRIGHT    : Copyright (c) 2020, MaxLinear, Inc.
**                Copyright (c) 2017 Intel Corporation
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author                $Comment
** 12 Sep 2013  Kamal Eradath          Initiate Version
*******************************************************************************/

/*! \file ppa_api_sw_accel.h
\brief This file contains es.
software fastpath function declarations
*/

/*@{*/

/*
* ####################################
*              Definition
* ####################################
*/

/*
* ####################################
*              Data Type
* ####################################
*/
extern volatile u_int8_t g_sw_fastpath_enabled;

typedef enum {
	SW_ACC_TYPE_IPV4,
	SW_ACC_TYPE_IPV6,
	SW_ACC_TYPE_6RD,
	SW_ACC_TYPE_DSLITE,
	SW_ACC_TYPE_BRIDGED,
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
	SW_ACC_TYPE_LTCP,
#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
	SW_ACC_TYPE_LTCP_LRO,
#endif
#endif
#if defined(L2TP_CONFIG) && L2TP_CONFIG
	SW_ACC_TYPE_L2TP,
#endif
	SW_ACC_TYPE_MAX
}sw_acc_type;

typedef struct sw_header {
	uint16_t tot_hdr_len;                /* Total length of outgoing header (for future use) */
	uint16_t transport_offset;   /* Transport header offset - from beginning of MAC header  */
	uint16_t network_offset;     /* Network header offset - from beginning of MAC header  */
#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
	uint32_t extmark;            /* marking */
#else /* CONFIG_NETWORK_EXTMARK */
	uint32_t mark;               /* marking */
#endif /* CONFIG_NETWORK_EXTMARK */
	sw_acc_type type;            /* Software acceleration type */
	PPA_NETIF *tx_if;            /* Out interface */
	int (*tx_handler)(PPA_SKBUF *skb); /* tx handler function */
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
	struct dst_entry *dst;       /* route entry */
#endif
	uint8_t hdr[0];              /* Header to be copied */
} t_sw_hdr;

/*
* ####################################
*             Declaration
* ####################################
*/
/* @} */
extern int32_t (*ppa_sw_fastpath_enable_hook)(uint32_t, uint32_t);
extern int32_t (*ppa_get_sw_fastpath_status_hook)(uint32_t *, uint32_t);
extern int32_t (*ppa_sw_fastpath_send_hook)(PPA_SKBUF *);
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
extern int32_t (*ppa_tcp_litepath_enable_hook)(uint32_t, uint32_t);
extern int32_t (*ppa_get_tcp_litepath_status_hook)(uint32_t *, uint32_t);
extern int32_t (*ppa_sw_litepath_tcp_send_hook)(PPA_SKBUF *);
#endif
#endif  /* __PPA_API_SW_ACCEL_H__20130913*/
