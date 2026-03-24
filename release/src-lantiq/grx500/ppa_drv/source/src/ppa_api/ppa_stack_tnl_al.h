
/*******************************************************************************
**
** FILE NAME	: ppa_stack_tnl_al.h
** PROJECT	: PPA
** MODULES	: PPA Stack Adaptation Layer
**
** DATE		: 12 May 2014
** AUTHOR	: Mahipati Deshpande
** DESCRIPTION	: Stack Adaptation for tunneled interfaces and sessions.
** NOTE		: This file is internal to PPA Stack Adaptation layer, so
**			file should be included(used) only in PPA stack adaptation
**			layer
** COPYRIGHT	: Copyright (c) 2017 Intel Corporation
**	 For licensing information, see the file 'LICENSE' in the root folder of
**	 this software module.
**
** HISTORY
** $Date		$Author				$Comment
** 12 May 2014	 	Mahipati Deshpande	Moved tunneled routines to this file
**								Added support for GRE
** 14 May 2020	  	Moinak Debnath		Added Support for VxLAN
*******************************************************************************/
#ifndef __PPA_STACK_TNL_AL__
#define __PPA_STACK_TNL_AL__

#define PPA_NETIF_FAIL	0 /*!< netif passed is NULL */

#define IPV4_HDR_LEN		20
#define IPV6_HDR_LEN		40
#define UDP_HDR_LEN		8
#define VXLAN_HDR_LEN		8

uint32_t ppa_is_vxlan_netif_type(struct net_device* dev, uint8_t* isIPv4VxLAN);

uint32_t ppa_is_gre_netif_type(struct net_device* dev, uint8_t* isIPv4Gre,
			       uint8_t* isGreTap);

/* Returns base interface of GRE tunnel */
struct net_device* ppa_get_gre_phyif(struct net_device* dev);

/* Returns destination MAC address of the GRE tunnel */
int32_t ppa_get_gre_dmac(uint8_t *mac,
struct net_device* dev,
struct sk_buff *skb);
/* Returns destination MAX for the 6RD tunnel */
int32_t ppa_get_6rd_dst_mac(struct net_device *dev,
PPA_BUF *ppa_buf,
uint8_t *mac,
uint32_t daddr);

/* Returns destination MAC of DS-Lite tunnel */
int32_t ppa_get_dslite_dst_mac(struct net_device *dev,PPA_BUF* buf, uint8_t *mac);

/*
* Supporting routines that are used by other modules or inside PPA
* Stack Adaptation Layer
*/
int32_t ppa_get_dmac_from_dst_entry(uint8_t* mac,
PPA_BUF* skb,
struct dst_entry *dst);

void ppa_neigh_update_hhs(struct neighbour *neigh);

void ppa_neigh_hh_init(struct neighbour *n);

uint32_t ppa_pkt_from_tunnelv6_netdev( PPA_NETIF *netdev, struct in6_addr saddr, struct in6_addr daddr);
uint32_t ppa_pkt_from_tunnelv4_netdev( PPA_NETIF *netdev, u32 saddr, u32 daddr);
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
#if IS_ENABLED(CONFIG_NET_IPGRE)
void init_gre_hooks(void);
void uninit_gre_hooks(void);
#endif/* IS_ENABLED(CONFIG_NET_IPGRE)*/
#endif /*LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)*/

static inline int is_zero_mac(uint8_t *mac)
{
	return !(mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]);
}

#endif

