/******************************************************************************
 **
 ** FILE NAME    : mcast_helper.h
 ** AUTHOR       : 
 ** DESCRIPTION  : Multicast Helper module header file
 ** COPYRIGHT    :      Copyright (c) 2014 2015
 **                Lantiq Beteiligungs-GmbH & Co. KG
 **
 **    This program is free software; you can redistribute it and/or modify
 **    it under the terms of the GNU General Public License as published by
 **    the Free Software Foundation; either version 2 of the License, or
 **    (at your option) any later version.
 **
 ** HISTORY
 ** $Date         $Author                $Comment
 ** 26 AUG 2014                 	      Initial Version
 **                                   
 *******************************************************************************/

/* 
 * mcast_helper.h - the header file with the ioctl definitions.
 *
 */

#ifndef _MCAST_HELPER_H
#define _MCAST_HELPER_H
#include <uapi/linux/in.h> 
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/module.h>
#include <net/mcast_helper_api.h>

#define MCH_MAGIC 'M'
#define MCH_MEMBER_ENTRY_ADD _IOR(MCH_MAGIC, 0, char *)
#define MCH_MEMBER_ENTRY_UPDATE _IOR(MCH_MAGIC, 1, char *)
#define MCH_MEMBER_ENTRY_REMOVE _IOR(MCH_MAGIC, 2, char *)
#define MCH_SERVER_ENTRY_GET _IOR(MCH_MAGIC, 3, char *)

#define SUCCESS					0x1
#define FAILURE					0x0

#define IPSTRLEN				40

#define FIRST_MINOR				0
#define MINOR_CNT				1
#define DEVICE_NAME				"mcast"

#define IN6_ARE_ADDR_EQUAL(a,b) \
	(!memcmp ((const void*)(a), (const void*)(b), sizeof (struct in6_addr)))
#ifndef IN6_IS_ADDR_UNSPECIFIED
# define IN6_IS_ADDR_UNSPECIFIED(a) \
	(((uint32_t *) (a))[0] == 0 && ((uint32_t *) (a))[1] == 0 && \
	 ((uint32_t *) (a))[2] == 0 && ((uint32_t *) (a))[3] == 0)
#endif

#define FTUPLE_ARR_SIZE			10
#define IFSIZE				20

typedef struct _mcast_mac_t {
	unsigned char macaddr[ETH_ALEN];
	struct list_head list;
} MCAST_MAC_t;

typedef struct _mcast_member_t {
	struct net_device *memDev; /* Member NetDevice */
	IP_Addr_t groupIP; /* Destination IP (GA) - v4 or v6 */
	IP_Addr_t srcIP; /* Source IP - v4 or v6 (optional) */
	struct list_head macaddr_list; /* Lan/wlan  Mac Address */
	unsigned int macaddr_count; /** < Interface list count */

#ifdef CONFIG_MCAST_HELPER_ACL
	uint32_t aclBlocked; /* Flag indicates acl status blocked(1)/unblocked(0) */
#endif
	struct list_head list;	/* mcast_member interface map  List  */
} MCAST_MEMBER_t;

typedef struct _mcast_rec_t {
	char memIntrfName[IFSIZE]; /* Member NetDevice */
	char rxIntrfName[IFSIZE]; /* Rx Member NetDevice */
	IP_Addr_t groupIP; /* Destination IP (GA) - v4 or v6 */
	IP_Addr_t srcIP; /* Source IP - v4 or v6 (optional) */
	uint32_t proto;	/* Protocol Number */
	uint32_t sPort; /* Udp Source Port */
	uint32_t dPort; /* Udp Destination Port */
	unsigned char macaddr[ETH_ALEN]; /* Lan/wlan Mac Address */
} MCAST_REC_t;

typedef struct  _mcast_gimc_t {
	unsigned int grpIdx;	/* Group Index */
	MCAST_STREAM_t mc_stream; /* Five tuple info */
#ifdef CONFIG_MCAST_HELPER_ACL
	unsigned long long int oifbitmap; /* Output interface bitmap */
	unsigned int probeFlag; /* Probe Packet generate flag enable(1)/disable(0) */
#endif
	struct list_head list;	/**< Creating Global List  */
	struct list_head mc_mem_list;	/**< Creating Global List  */
	int br_callback_flag;	/* Flag for bridge five tuple info */
} MCAST_GIMC_t;

typedef struct
{
	int status, dignity, ego;
} query_arg_t;

typedef struct _ftuple_info_t {
	char rxIntrfName[IFSIZE]; /* Rx Member NetDevice */
	IP_Addr_t groupIP; /* Destination IP (GA) - v4 or v6 */
	IP_Addr_t srcIP; /* Source IP - v4 or v6 (optional) */
	uint32_t proto; /* Protocol Number */
	uint32_t sPort; /* Udp Source Port */
	uint32_t dPort; /* Udp Destination Port */
	uint32_t uflag; /* update flag for ftuple array */
	unsigned char src_mac[ETH_ALEN];/* source Mac address for grx5xx */
} FTUPLE_INFO_t;

#define QUERY_GET_VARIABLES _IOR('q', 1, query_arg_t *)
#define QUERY_CLR_VARIABLES _IO('q', 2)
#define QUERY_SET_VARIABLES _IOW('q', 3, query_arg_t *)

#endif /* _MCAST_HELPER_H */
