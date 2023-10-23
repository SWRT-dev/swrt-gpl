/*******************************************************************************
*
* Copyright (c) 2020 - 2021, MaxLinear, Inc.
*
*******************************************************************************/

#ifndef _MCAST_HELPER_API_H
#define _MCAST_HELPER_API_H

#include <net/addrconf.h>
#include <net/ipv6.h>

/* Reg/dereg flag definition */
#define MCH_F_REGISTER		0x01
#define MCH_F_DEREGISTER	0x02
#define MCH_F_DIRECTPATH	0x04
#define MCH_F_UPDATE_MAC_ADDR	0x08
#define MCH_F_FW_RESET		0x10
#define MCH_F_NEW_STA		0x20
#define MCH_F_DISCONN_MAC	0x40

/* Callback flag definition */
#define MCH_CB_F_ADD		0x01
#define MCH_CB_F_DEL		0x02
#define MCH_CB_F_UPD		0x03
#define MCH_CB_F_DEL_UPD	0x08

#define MAX_MAC			64

#define MCH_GID_BCAST		0
#define MCH_GID_ERR		-1

/** Protocol type */
typedef enum {
	IPV4 = 0,
	IPV6 = 1,
	INVALID,
} ptype_t;

typedef struct _ip_addr_t {
	/* Protocol type IPV4 or IPV6 */
	ptype_t ipType;
	union {
		struct in_addr ipAddr;
		struct in6_addr ip6Addr;
	} ipA;
} IP_Addr_t;

typedef struct  _mcast_stream_t {
	/* Rx NetDevice */
	struct net_device *rxDev;
	/* Source IP - v4 or v6 */
	IP_Addr_t sIP;
	/* Destination IP (GA) - v4 or v6 */
	IP_Addr_t dIP;
	/* Protocol Type */
	uint32_t proto;
	/* Source Port */
	uint32_t sPort;
	/* Destination Port */
	uint32_t dPort;
	/* LAN/WLAN source Mac address  */
	unsigned char src_mac[ETH_ALEN];
	/* Number of Joined MACs */
	uint32_t num_joined_macs;
	/* Lan/wlan array of joined Mac Address */
	unsigned char macaddr[MAX_MAC][ETH_ALEN];
} MCAST_STREAM_t;

typedef int32_t (*Mcast_module_callback_t)(unsigned int grpidx,
		struct net_device *netdev, void *mc_stream, unsigned int flag);

extern void  mcast_helper_register_module(
	struct net_device *dev,		/* Registered netdev e.g. wlan0 */
	struct module *modName,		/* Kernel Module Name */
	char *addlName,			/* Optional Additional Name */
	Mcast_module_callback_t *fnCB,	/* Callback Function */
	void *data,			/* Variable input data */
	unsigned int flags);		/* Flag - MCH_F_* */

extern int mcast_helper_get_skb_gid(struct sk_buff *skb);

#endif  /* _MCAST_HELPER_API_H */
