#ifndef __PPA_API_CORE_H__20081103_1920__
#define __PPA_API_CORE_H__20081103_1920__

/*******************************************************************************
 **
 ** FILE NAME    : ppa_api_core.h
 ** PROJECT      : PPA
 ** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
 **
 ** DATE         : 3 NOV 2008
 ** AUTHOR       : Xu Liang
 ** DESCRIPTION  : PPA Protocol Stack Hook API Implementation Header File
 ** COPYRIGHT    : Copyright (c) 2017 Intel Corporation
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date        $Author         $Comment
 ** 03 NOV 2008  Xu Liang        Initiate Version
 *******************************************************************************/
/*! \file ppa_api_core.h
  \brief This file contains es.
  provide PPA API.
 */

/** \addtogroup PPA_CORE_API PPA Core API
  \brief  PPA Core API provide PPA core accleration logic and API
  The API is defined in the following two source files
  - ppa_api_core.h: Header file for PPA API
  - ppa_api_core.c: C Implementation file for PPA API
 */
/* @{ */

/*
 * ####################################
 *              Definition
 * ####################################
 */

#define PPA_SESSION_FILTER_PROTO 0x0001
#define PPA_SESSION_FILTER_SPORT 0x0002
#define PPA_SESSION_FILTER_DPORT 0x0004
typedef struct {
	struct list_head list;
	uint16_t		ip_proto;			/*IP prorocol */
	uint16_t		src_port;			/* Source port */
	uint16_t		dst_port;			/* Destination port */
	uint16_t		flags;			  /* Indicates which parameters to be checked */
} FILTER_INFO;

enum VERSION_INDEX {
	VERSION_MAJOR = 0, /*!< Major Version Index */
	VERSION_MID, /*!< Mid Version Index */
	VERSION_MINOR, /*!< Minor Version Index */
	VERSION_TAG, /*!< Tag Version Index */
	VERSION_MAX /*!< Tag Version Index */
};

#define VERSION_STR_LEN	16

#if defined(CONFIG_MCAST_HELPER_MODULE) || defined(CONFIG_MCAST_HELPER)
#define MC_F_ADD 0x01
#define MC_F_DEL 0x02
#define MC_F_ADD_DROP 0x04
#define MC_F_DEL_DROP 0x08

#define MC_F_REGISTER   0x01
#define MC_F_DEREGISTER 0x02
#define MC_F_DIRECTPATH 0x04
#define PPA_MCAST_DEBUG 1
#define MAX_MAC		64

/* mcast stream data structure */
#ifndef CONFIG_MCAST_SNOOPING
typedef enum {
	IPV4 = 0,
	IPV6 = 1,
	INVALID,
} ptype_t;
#endif

typedef struct _ip_addr_t{
	ptype_t  ipType ;/* IPv4 or IPv6 */
	union {
		struct in_addr ipAddr;
		struct in6_addr ip6Addr;
	} ipA;
} ip_addr_t;

typedef struct _mcast_stream_t{
	struct net_device *rx_dev;/* Rx Netdevice */
	ip_addr_t src_ip;/* Source ip : can be ipv4 or ipv6 */
	ip_addr_t dst_ip;/* Destination ip - GA : can be ipv4 of ipv6 */
	uint32_t proto;/* Protocol type : Mostly UDP for Multicast */
	uint32_t src_port;/* Source port */
	uint32_t dst_port;/* Destination port */
	unsigned char src_mac[ETH_ALEN];/* source Mac address for grx5xx */
	uint32_t num_joined_macs; /* Number of Joined MACs */
	unsigned char macaddr[MAX_MAC][ETH_ALEN]; /* Lan/wlan array of joined Mac Address */
} mcast_stream_t;

/*Same definition shall be present in multicast helper as well */
typedef int32_t (*Mcast_module_callback_t)(unsigned int grpidx, struct net_device *netdev, void *mc_stream, unsigned int flag);

extern int mcast_helper_register_module(PPA_NETIF *mem_dev,
					struct module *mod_name,
					char *addl_name,
					Mcast_module_callback_t fn_cb,
					void *data,
					uint32_t flags);

int32_t mcast_module_config(uint32_t grp_idx,
			    struct net_device *member,
			    void *mc_stream,
			    uint32_t flags);
#endif

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
extern uint32_t ppa_add_ipsec_tunnel_tbl_entry(PPA_XFRM_STATE * entry,
					       sa_direction dir,
					       uint32_t *tunnel_index);
extern uint32_t ppa_get_ipsec_tunnel_tbl_entry(PPA_XFRM_STATE *entry,
					       sa_direction *dir,
					       uint32_t *tunnel_index);
extern uint32_t ppa_add_ipsec_tunnel_tbl_update(sa_direction dir,
						uint32_t tunnel_index);
#endif
extern FILTER_INFO* get_matched_entry (PPA_HAL_ID hal_id, FILTER_INFO *entry);
extern bool add_filter_entry (PPA_HAL_ID hal_id, FILTER_INFO *entry);
extern bool del_filter_entry (PPA_HAL_ID hal_id, FILTER_INFO *entry);

#endif  /*  __PPA_API_CORE_H__20081103_1920__ */
/* @} */
