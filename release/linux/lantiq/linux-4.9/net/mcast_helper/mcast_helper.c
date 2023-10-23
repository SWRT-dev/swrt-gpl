/******************************************************************************
 **
 **FILE NAME   : mcast_helper.c
 **AUTHOR      :
 **DESCRIPTION : Multicast Helper module
 **COPYRIGHT   :      Copyright (c) 2014 2015
 **               Lantiq Beteiligungs-GmbH & Co. KG
 **
 **   This program is free software; you can redistribute it and/or modify
 **   it under the terms of the GNU General Public License as published by
 **   the Free Software Foundation; either version 2 of the License, or
 **   (at your option) any later version.
 **
 **HISTORY
 **$Date         $Author                $Comment
 **26 AUG 2014                 	      Initial Version
 **
 *******************************************************************************/

/**Header files */
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/skbuff.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/ipv6.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/inet.h>
#include "mcast_helper.h"
#include <linux/in.h>
#include <linux/in6.h>
#include <net/checksum.h>

#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#endif

#include <linux/spinlock.h>
#include <linux/if_arp.h>
#include <linux/if_pppox.h>

/** Defines **/
#define GINDX_LOOP_COUNT        1
#define GINDX_MAX_SIZE          64

#define UDP_HDR_LEN             (sizeof(struct udphdr))
#define TOT_HDR_LEN             (sizeof(struct iphdr) + UDP_HDR_LEN)
#define IP6_HDR_LEN             (sizeof(struct ipv6hdr))
#define TOT6_HDR_LEN            (IP6_HDR_LEN + UDP_HDR_LEN)

#define MCH_UPDATE_TIMER        10
#define LANSERVER_UPDATE_TIMER  10

/* Log level for debug prints */
static int mcast_debug;

#define mch_debug(fmt, ...)					\
do {								\
	if (mcast_debug)					\
		printk(KERN_DEBUG "MCH: "fmt, ##__VA_ARGS__);	\
} while (0)

/*!
  \brief  Packet buffer structure. For Linux OS, this is the sk_buff structure.
 */
typedef struct sk_buff MCH_BUF;

/*!
  \brief Macro that specifies MCH network interface data structure
 */
typedef struct net_device MCH_NETIF;

/**mcast helper member  list */
LIST_HEAD(mch_mem_list_g);
LIST_HEAD(mch_mem_list6_g);

/** mcast helper member session list */
LIST_HEAD(mch_mem_list_session_g);
LIST_HEAD(mch_mem_list6_session_g);

/** mcast helper global variables */
static FTUPLE_INFO_t ftuple_info[FTUPLE_ARR_SIZE];
static FTUPLE_INFO_t ftuple_info6[FTUPLE_ARR_SIZE];
static unsigned long long int g_mcast_grpindex[GINDX_LOOP_COUNT];

#if defined(CONFIG_PROC_FS)
struct mcast_helperf_iter_state {
	struct seq_net_private p;
	struct net_device *dev;
	struct in_device *in_dev;
};
#endif

static struct sk_buff *skb_buff = NULL;
static struct sk_buff *skb_buff6 = NULL;
struct timer_list mcast_helper_exp_timer;
struct timer_list mcast_lanserver_timer;
static char mch_captured_skb = 1;
static char mch_captured_skb6 = 1;
static char mch_signature[] = "mcast1234";
int mch_timerstarted = 0;
int lanserver_timerstarted = 0;
int mch_timermod = 0;
int lanserver_timermod = 0;

static void mcast_helper_start_helper_timer(void);
static void mcast_helper_start_lanserver_timer(void);

int mch_iptype = 0;

int mch_acl_enabled = 0;
int mch_accl_enabled = 1;
#ifdef CONFIG_MCAST_HELPER_ACL
mch_acl_enabled	= 1;
#endif

static int mch_major = -1;
static struct cdev mcast_cdev;
static struct class *mcast_class = NULL;
static bool device_created;

#ifdef CONFIG_SYSCTL
static struct ctl_table_header *mcast_acl_sysctl_header;
static struct ctl_table_header *mcast_accl_sysctl_header;
#endif

extern void (*five_tuple_info_ptr)(struct sk_buff *, char);
#if IS_ENABLED(CONFIG_IPV6_MROUTE)
extern void (*five_tuple_info6_ptr)(struct sk_buff *, char);
#endif
extern void (*five_tuple_br_info_ptr)(struct sk_buff *);
extern void (*five_tuple_br_info_hook)(struct sk_buff *);

extern atomic_t mch_br_capture_pkt;
extern int mcast_helper_invoke_callback(unsigned int grpidx,
		struct net_device *netdev, void *mc_stream,
		unsigned int flag, unsigned int count);
extern int (*mcast_helper_sig_check_update_ptr)(struct sk_buff *skb);
extern int (*mcast_helper_get_skb_gid_ptr)(struct sk_buff *skb);

/**mcast helper function prototype */
static int mcast_helper_invoke_return_callback(unsigned int grpidx,
		struct net_device *netdev,
		MCAST_STREAM_t *mc_stream, unsigned int flag, unsigned int count);
static MCAST_GIMC_t *mcast_helper_search_gimc_record(IP_Addr_t *gaddr,
		IP_Addr_t *saddr, struct list_head *head);
static void mcast_helper_release_grpidx(int grpidx);
static MCAST_GIMC_t * mcast_helper_add_gimc_record(struct net_device *netdev,
                          IP_Addr_t * gaddr, IP_Addr_t * saddr,
                          unsigned int proto,unsigned int sport,
                          unsigned int dport, unsigned char *src_mac,
                          struct list_head *head);

static int mcast_helper_open(struct inode *i, struct file *f)
{
	return 0;
}

static int mcast_helper_close(struct inode *i, struct file *f)
{
	return 0;
}

/*=============================================================================
 *Function Name: mcast_helper_copy_ipaddr
 *Description	: Wrapper function to copy ip address
 *===========================================================================*/
static void mcast_helper_copy_ipaddr(IP_Addr_t *to, IP_Addr_t *from)
{
	if (to == NULL || from == NULL)
		return;

	memcpy(to, from, sizeof(IP_Addr_t));
}

/*=============================================================================
 *Function Name: mcast_helper_is_same_ipaddr
 *Description	: Wrapper function to compare IP address
 *===========================================================================*/
static int mcast_helper_is_same_ipaddr(IP_Addr_t *addr1, IP_Addr_t *addr2)
{
	if (addr1 == NULL || addr2 == NULL)
		return 0;

	if (addr1->ipType == IPV4 && addr2->ipType == IPV4) {
		return addr1->ipA.ipAddr.s_addr == addr2->ipA.ipAddr.s_addr;
	} else if (addr1->ipType == IPV6 && addr2->ipType == IPV6) {
		return IN6_ARE_ADDR_EQUAL(&addr1->ipA.ip6Addr,
				&addr2->ipA.ip6Addr);
	}

	return 0;
}

/*=============================================================================
 *Function Name: mcast_helper_init_ipaddr
 *Description	: Wrapper function to initialize IP address
 *===========================================================================*/
static void mcast_helper_init_ipaddr(IP_Addr_t *addr, ptype_t type, void *addrp)
{
	if (addr == NULL)
		return;

	addr->ipType = type;
	if (type == IPV4) {
		if (addrp)
			addr->ipA.ipAddr.s_addr = *((unsigned int *)addrp);
		else
			addr->ipA.ipAddr.s_addr = 0;
	} else if (type == IPV6) {
		struct in6_addr *in6 = (struct in6_addr *)addrp;
		if (addrp)
			memcpy(&(addr->ipA.ip6Addr), in6, sizeof(struct in6_addr));
		else
			memset(&(addr->ipA.ip6Addr), 0, sizeof(struct in6_addr));
	}
}

/*=============================================================================
 *Function Name: mcast_helper_is_addr_unspecified
 *Description	: Wrapper function to ip address is specified
 *===========================================================================*/
static int mcast_helper_is_addr_unspecified(IP_Addr_t *addr)
{
	if (addr->ipType == IPV4) {
		return addr->ipA.ipAddr.s_addr == 0;
	} else if (addr->ipType == IPV6) {
		return IN6_IS_ADDR_UNSPECIFIED(addr->ipA.ip6Addr.s6_addr);
	}
	return 0;
}

/*=============================================================================
 *Function Name: mcast_helper_list_p
 *Description	: Wrapper function to get the membership list pointer
 *===========================================================================*/
static struct list_head *mcast_helper_list_p(ptype_t type)
{
	if (type == IPV4) {
		return &mch_mem_list_g;
	} else if (type == IPV6) {
		return &mch_mem_list6_g;
	}

	return NULL;
}

static struct list_head *mcast_helper_session_p(ptype_t type)
{
	if (type == IPV4) {
		return &mch_mem_list_session_g;
	}
	else if (type == IPV6) {
		return &mch_mem_list6_session_g;
	}

	return NULL;
}

/*=============================================================================
 *Function Name: mch_check_is_ppp_netif
 *Description  : Wrapper function to check netif is ppp
 *===========================================================================*/
static inline uint32_t mch_check_is_ppp_netif(MCH_NETIF *netif)
{
	return ((netif->type == ARPHRD_PPP) && (netif->flags & IFF_POINTOPOINT));
}

/*=============================================================================
 *Function Name: mcast_helper_get_pkt_rx_src_mac_addr
 *Description  : Wrapper function to extract the source mac address from skb
 *===========================================================================*/
static inline void mcast_helper_get_pkt_rx_src_mac_addr(MCH_BUF *mch_buf,
						 uint8_t mac[ETH_ALEN])
{
	if ((uint32_t)skb_mac_header(mch_buf) >= KSEG0)
		memcpy(mac, skb_mac_header(mch_buf) + ETH_ALEN, ETH_ALEN);
}

/*=============================================================================
 *Function Name: mch_get_netif
 *Description  : Wrapper function to get netif from interface name
 *===========================================================================*/
static MCH_NETIF *mch_get_netif(char *ifname)
{
	MCH_NETIF *netif;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 23)
	netif = dev_get_by_name(ifname);
#else
	netif = dev_get_by_name(&init_net, ifname);
#endif
	if (netif) {
		dev_put(netif);
		return netif;
	} else
		return NULL;
}

/*=============================================================================
 *Function Name: mch_get_netif_by_idx
 *Description  : Wrapper function to get netif from interface index
 *===========================================================================*/
static MCH_NETIF *mch_get_netif_by_idx(int ifindex)
{
	MCH_NETIF *netif;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 23)
	netif = dev_get_by_index(ifindex);
#else
	netif = dev_get_by_index(&init_net, ifindex);
#endif
	if (netif) {
		dev_put(netif);
		return netif;
	} else
		return NULL;
}

/*=============================================================================
 *Function Name: mcast_helper_get_rx_mac_addr
 *Description	: function to read the source mac address from skb
 *                based on interface type
 *===========================================================================*/
static void mcast_helper_get_rx_mac_addr(MCH_BUF *mch_buf,
					 char *ifname, char *s_mac)
{
	MCH_NETIF *netif;
	int32_t hdr_offset = ETH_HLEN;

	netif = mch_get_netif(ifname);
	if (netif == NULL)
		return;

	if (mch_check_is_ppp_netif(netif)) {
		hdr_offset += PPPOE_SES_HLEN;
		skb_set_mac_header(mch_buf, -hdr_offset);
		mcast_helper_get_pkt_rx_src_mac_addr(mch_buf, s_mac);
		skb_reset_mac_header(mch_buf);
	} else {
		mcast_helper_get_pkt_rx_src_mac_addr(mch_buf, s_mac);
	}
}

/*=============================================================================
 *Function Name: mcast_helper_five_tuple_info
 *Description  : Function  to retrive 5-tuple info for IPV4 packet
 *===========================================================================*/
static void
mcast_helper_five_tuple_info(struct sk_buff *skb, char iface_name[IFSIZE])
{
	int index = 0;
	struct iphdr *iph = ip_hdr(skb);
	struct udphdr *udph = (struct udphdr *)((u8 *)iph +(iph->ihl << 2));
	const unsigned char *dest = eth_hdr(skb)->h_dest;

	if (is_multicast_ether_addr(dest)) {
		if (mch_captured_skb && mch_acl_enabled &&
		    strncmp(skb->dev->name, "br", 2)) {
			skb_buff = skb_copy(skb, GFP_ATOMIC);
			mch_captured_skb = 0;
		}

		for (index = 0; index < FTUPLE_ARR_SIZE; index++) {
			if (ftuple_info[index].uflag == 0) {
				strncpy(ftuple_info[index].rxIntrfName,
						iface_name, strlen(iface_name));
				mcast_helper_init_ipaddr(
						&ftuple_info[index].groupIP,
						IPV4, &iph->daddr);
				mcast_helper_init_ipaddr(
						&ftuple_info[index].srcIP,
						IPV4, &iph->saddr);
				ftuple_info[index].proto = iph->protocol;
				ftuple_info[index].sPort = udph->source;
				ftuple_info[index].dPort = udph->dest;
				mcast_helper_get_rx_mac_addr(skb, iface_name,
						ftuple_info[index].src_mac);

				ftuple_info[index].uflag = 1;
				if ((index + 1) < FTUPLE_ARR_SIZE)
					ftuple_info[index + 1].uflag = 0;
				else
					ftuple_info[0].uflag = 0;
				break;
			} else if (mcast_helper_is_same_ipaddr(
				   &ftuple_info[index].groupIP,
				   (IP_Addr_t *)&iph->daddr) &&
				   (mcast_helper_is_same_ipaddr(
				   &ftuple_info[index].srcIP,
				   (IP_Addr_t *)&iph->saddr) ||
				   mcast_helper_is_addr_unspecified(
				   &ftuple_info[index].srcIP))) {
				break;
			}
		}
	}
	return;
}

/*=============================================================================
 *Function Name: mcast_helper_five_tuple_info6
 *Description  : Function  to retrive 5-tuple info for IPV6 packet
 *===========================================================================*/
static void
mcast_helper_five_tuple_info6(struct sk_buff *skb, char iface_name[IFSIZE])
{
	int index;
	struct ipv6hdr *ip6h = ipv6_hdr(skb);
	/*
	 * IP packet received from ip6mr.c hook,
	 * cannot use udp_hdr() directly to get udp details
	 */
	struct udphdr *udph = (struct udphdr *)((u8 *)ip6h + IP6_HDR_LEN);
	const unsigned char *dest = eth_hdr(skb)->h_dest;

	if (is_multicast_ether_addr(dest)) {
		if (mch_captured_skb6 && mch_acl_enabled &&
		    strncmp(skb->dev->name, "br", 2)) {
			skb_buff6 = skb_copy(skb, GFP_ATOMIC);
			mch_captured_skb6 = 0;
		}
		for (index = 0; index < FTUPLE_ARR_SIZE; index++) {
			if (ftuple_info6[index].uflag == 0) {
				strncpy(ftuple_info6[index].rxIntrfName,
					iface_name, strlen(iface_name));
				mcast_helper_init_ipaddr(
						&ftuple_info6[index].groupIP,
						IPV6, &ip6h->daddr);
				mcast_helper_init_ipaddr(
						&ftuple_info6[index].srcIP,
						IPV6, &ip6h->saddr);
				ftuple_info6[index].proto = ip6h->nexthdr;
				ftuple_info6[index].sPort = udph->source;
				ftuple_info6[index].dPort = udph->dest;
				mcast_helper_get_rx_mac_addr(skb, iface_name,
						ftuple_info6[index].src_mac);

				ftuple_info6[index].uflag = 1;
				if ((index + 1) < FTUPLE_ARR_SIZE)
					ftuple_info6[index + 1].uflag = 0;
				else
					ftuple_info6[0].uflag = 0;
				break;
			} else if (mcast_helper_is_same_ipaddr(
				   &ftuple_info6[index].groupIP,
				   (IP_Addr_t *)&ip6h->daddr) &&
				   (mcast_helper_is_same_ipaddr(
				   &ftuple_info6[index].srcIP,
				   (IP_Addr_t *)&ip6h->saddr) ||
				   mcast_helper_is_addr_unspecified(
				   &ftuple_info6[index].srcIP))) {
				break;
			}
		}
	}
	return;
}

/*=============================================================================
 *Function Name: mcast_helper_update_five_tuple
 *Description  : Function to update 5-tuple info for stream and inform callback
 *===========================================================================*/
static MCAST_GIMC_t *mcast_helper_update_five_tuple(struct sk_buff *skb,
		IP_Addr_t *daddr, IP_Addr_t *saddr, uint32_t proto,
		uint32_t sPort, uint32_t dPort, bool update)
{
	MCAST_GIMC_t *gimc_rec;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	struct list_head *gimc_list;
	unsigned int flag = MCH_CB_F_ADD;
	const unsigned char *src_mac = eth_hdr(skb)->h_source;

	if (skb->protocol == htons(ETH_P_IP))
		gimc_list = mcast_helper_list_p(IPV4);
	else
		gimc_list = mcast_helper_list_p(IPV6);
	gimc_rec = mcast_helper_search_gimc_record(daddr, saddr, gimc_list);
	if (gimc_rec == NULL)
		goto done;

	/*
	 * FIXME: Updated flag indicates the touple change.
	 * Think about the callback in LAN server case, when
	 * same stream coming but port got changed.
	 */
	if (update) {
		memcpy(gimc_rec->mc_stream.src_mac, src_mac, ETH_ALEN);
		gimc_rec->mc_stream.rxDev = mch_get_netif_by_idx(skb->skb_iif);
		flag = MCH_CB_F_UPD;
	}

	/* If not asked to update always, depends on br_callback_flag */
	if (!update && gimc_rec->br_callback_flag)
		goto done;

	/* GA/daddr & dev updated from MCH add entry */
	gimc_rec->br_callback_flag = 1;
	atomic_dec(&mch_br_capture_pkt);
	memcpy(&gimc_rec->mc_stream.sIP, saddr, sizeof(IP_Addr_t));
	gimc_rec->mc_stream.proto = proto;
	gimc_rec->mc_stream.sPort = sPort;
	gimc_rec->mc_stream.dPort = dPort;

	list_for_each_safe(liter, tliter, &gimc_rec->mc_mem_list) {
		gitxmc_rec = list_entry(liter, MCAST_MEMBER_t, list);
		mcast_helper_invoke_return_callback(gimc_rec->grpIdx,
				gitxmc_rec->memDev, &gimc_rec->mc_stream,
				flag, gitxmc_rec->macaddr_count);
	}

done:
	return gimc_rec;
}

/*=============================================================================
 *Function Name: mcast_helper_five_tuple_br_info
 *Description  : Function to retrive 5-tuple info for bridge IPV4 and IPV6 packet
 *===========================================================================*/
static void mcast_helper_five_tuple_br_info(struct sk_buff *skb)
{
	IP_Addr_t saddr;
	IP_Addr_t daddr;

	if (skb->protocol == htons(ETH_P_IP)) {
		struct iphdr *iph = ip_hdr(skb);
		struct udphdr *udph = (struct udphdr *)((u8 *)iph +
				(iph->ihl << 2));

		if (mch_captured_skb && mch_acl_enabled) {
			skb_buff = skb_copy(skb, GFP_ATOMIC);
			mch_captured_skb = 0;
		}

		mcast_helper_init_ipaddr(&saddr, IPV4, &iph->saddr);
		mcast_helper_init_ipaddr(&daddr, IPV4, &iph->daddr);

		/* Update helper and inform callback if not done */
		mcast_helper_update_five_tuple(skb, &daddr, &saddr,
				iph->protocol, udph->source, udph->dest, false);
	} else if (skb->protocol == htons(ETH_P_IPV6)) {
		struct ipv6hdr *iph6 = ipv6_hdr(skb);
		/*
		 * XXX: IP packet received from bridge hook,
		 * cannot use udp_hdr() directly to get udp details
		 */
		struct udphdr *udph6 = (struct udphdr *)((u8 *)iph6 +
				IP6_HDR_LEN);

		if (mch_captured_skb6 && mch_acl_enabled) {
			skb_buff6 = skb_copy(skb, GFP_ATOMIC);
			mch_captured_skb6 = 0;
		}

		mcast_helper_init_ipaddr(&saddr, IPV6, &iph6->saddr);
		mcast_helper_init_ipaddr(&daddr, IPV6, &iph6->daddr);

		/* Update helper and inform callback if not done */
		mcast_helper_update_five_tuple(skb, &daddr, &saddr,
				iph6->nexthdr, udph6->source, udph6->dest, false);
	}

	return;
}

/*=============================================================================
 *Function Name: mcast_helper_update_lanserver_list
 *Description  : Function to add IPv4 5-tuple info and inform callback
 *===========================================================================*/
static void mcast_helper_update_lanserver_list(struct sk_buff *skb)
{
	bool changed = false;
	IP_Addr_t saddr;
	IP_Addr_t daddr;
	MCAST_GIMC_t *gimc_rec;
	MCAST_GIMC_t *gimc_rec_h;
	MCH_NETIF *rxDev = NULL;
	struct list_head *gimc_list;
	unsigned char *src_mac = eth_hdr(skb)->h_source;
	struct iphdr *iph = ip_hdr(skb);
	struct udphdr *udph = (struct udphdr *)((u8 *)iph + (iph->ihl << 2));

	/*
	 * Ignore Multicast control packets(224.0.0.x) &
	 * UPnP packets(239.255.255.250)
	 */
	if (((ntohl(iph->daddr) & 0xffffff00U) == 0xe0000000U) ||
	    ((ntohl(iph->daddr) & 0xffffffffU) == 0xeffffffaU))
		return;

	rxDev = mch_get_netif_by_idx(skb->skb_iif);
	mcast_helper_init_ipaddr(&saddr, IPV4, &iph->saddr);
	mcast_helper_init_ipaddr(&daddr, IPV4, &iph->daddr);

	/* Search stream in lanserver list */
	gimc_list = mcast_helper_session_p(IPV4) ;
	gimc_rec = mcast_helper_search_gimc_record(&daddr, &saddr, gimc_list);
	if (gimc_rec == NULL) {
		gimc_rec = mcast_helper_add_gimc_record(rxDev, &daddr,
				&saddr, iph->protocol, udph->source,
				udph->dest, src_mac, gimc_list);
		if (gimc_rec == NULL)
			return;

		/* Update helper and inform callback if not done */
		gimc_rec_h = mcast_helper_update_five_tuple(skb, &daddr, &saddr,
				iph->protocol, udph->source, udph->dest, false);
		if (gimc_rec_h) {
			/* Use same GID from helper list */
			mcast_helper_release_grpidx(gimc_rec->grpIdx);
			gimc_rec->grpIdx = gimc_rec_h->grpIdx;
		}
	} else {
		/* Updating lanserver list entry */
		if (gimc_rec->mc_stream.rxDev != rxDev) {
			gimc_rec->mc_stream.rxDev = rxDev;
			changed = true;
		}
		if (gimc_rec->mc_stream.proto != iph->protocol) {
			gimc_rec->mc_stream.proto = iph->protocol;
			changed = true;
		}
		if (gimc_rec->mc_stream.sPort != udph->source) {
			gimc_rec->mc_stream.sPort = udph->source;
			changed = true;
		}
		if (gimc_rec->mc_stream.dPort != udph->dest) {
			gimc_rec->mc_stream.dPort = udph->dest;
			changed = true;
		}
		if (!ether_addr_equal(gimc_rec->mc_stream.src_mac, src_mac)) {
			memcpy(gimc_rec->mc_stream.src_mac, src_mac, ETH_ALEN);
			changed = true;
		}
		if (changed) {
			/*
			 * Some change in source, update helper list entry
			 * and inform callback */
			mcast_helper_update_five_tuple(skb, &daddr, &saddr,
					iph->protocol, udph->source,
					/* TODO: touple updated - callback? */
					udph->dest, false);
		}
	}
}

/*=============================================================================
 *Function Name: mcast_helper_update_lanserver_list6
 *Description  : Function to add IPv6 5-tuple info and inform callback
 *===========================================================================*/
static void mcast_helper_update_lanserver_list6(struct sk_buff *skb)
{
	bool changed = false;
	IP_Addr_t saddr;
	IP_Addr_t daddr;
	MCAST_GIMC_t *gimc_rec;
	MCAST_GIMC_t *gimc_rec_h;
	MCH_NETIF *rxDev = NULL;
	struct list_head *gimc_list;
	unsigned char *src_mac = eth_hdr(skb)->h_source;
	struct ipv6hdr *iph6 = ipv6_hdr(skb);
	/*
	 * IP packet received from bridge hook, cannot
	 * use udp_hdr() directly to get udp details
	 */
	struct udphdr *udph6 = (struct udphdr *)((u8 *)iph6 +
			IP6_HDR_LEN);

	rxDev = mch_get_netif_by_idx(skb->skb_iif);
	mcast_helper_init_ipaddr(&saddr, IPV6, &iph6->saddr);
	mcast_helper_init_ipaddr(&daddr, IPV6, &iph6->daddr);

	/* Search stream in lanserver list */
	gimc_list = mcast_helper_session_p(IPV6);
	gimc_rec = mcast_helper_search_gimc_record(&daddr, &saddr, gimc_list);
	if (gimc_rec == NULL) {
		gimc_rec = mcast_helper_add_gimc_record(rxDev, &daddr,
				&saddr, iph6->nexthdr, udph6->source,
				udph6->dest, src_mac, gimc_list);
		if (gimc_rec == NULL)
			return;

		/* Update helper and inform callback if not done */
		gimc_rec_h = mcast_helper_update_five_tuple(skb, &daddr, &saddr,
				iph6->nexthdr, udph6->source, udph6->dest, false);
		if (gimc_rec_h) {
			/* Use same GID from helper list */
			mcast_helper_release_grpidx(gimc_rec->grpIdx);
			gimc_rec->grpIdx = gimc_rec_h->grpIdx;
		}
	} else {
		/* Updating lanserver list entry */
		if (gimc_rec->mc_stream.rxDev != rxDev) {
			gimc_rec->mc_stream.rxDev = rxDev;
			changed = true;
		}
		if (gimc_rec->mc_stream.proto != iph6->nexthdr) {
			gimc_rec->mc_stream.proto = iph6->nexthdr;
			changed = true;
		}
		if (gimc_rec->mc_stream.sPort != udph6->source) {
			gimc_rec->mc_stream.sPort = udph6->source;
			changed = true;
		}
		if (gimc_rec->mc_stream.dPort != udph6->dest) {
			gimc_rec->mc_stream.dPort = udph6->dest;
			changed = true;
		}
		if (!ether_addr_equal(gimc_rec->mc_stream.src_mac, src_mac)) {
			memcpy(gimc_rec->mc_stream.src_mac, src_mac, ETH_ALEN);
			changed = true;
		}
		if (changed) {
			/*
			 * Some change in source, update helper list entry
			 * and inform callback */
			mcast_helper_update_five_tuple(skb, &daddr, &saddr,
					iph6->nexthdr, udph6->source,
					/* TODO: touple updated - callback? */
					udph6->dest, false);
		}
	}
}

/*=============================================================================
 * Function Name : mcast_helper_five_tuple_br_hook
 * Description   : Function to retrive 5-tuple info for bridge
 *                 IPV4 and IPV6 packet (lanserver)
 *===========================================================================*/
static void mcast_helper_five_tuple_br_hook(struct sk_buff *skb)
{
	if (skb->protocol == htons(ETH_P_IP)) {
		mcast_helper_update_lanserver_list(skb);
	} else if (skb->protocol == htons(ETH_P_IPV6)) {
		mcast_helper_update_lanserver_list6(skb);
	}
}

/*=============================================================================
 * Function Name : mcast_helper_allocate_grpidx
 * Description	 : Function to allocate group index
 *===========================================================================*/
static int mcast_helper_allocate_grpidx(void)
{
	unsigned int index = 0, i = 0;
	int grpIdx = -1;

	for (index = 0; index < GINDX_LOOP_COUNT; index++) {
		for (i = 0 ; i < GINDX_MAX_SIZE ; i++) {
			if ((g_mcast_grpindex[index] & BIT_ULL(i)) == 0) {
				g_mcast_grpindex[index] |= BIT_ULL(i);
				grpIdx = i + 1 + (GINDX_MAX_SIZE * index);
				return grpIdx;
			}
		}
	}

	return grpIdx;
}

/*=============================================================================
 *Function Name: mcast_helper_release_grpidx
 *Description	: Function to release allocated group index
 *===========================================================================*/
static void mcast_helper_release_grpidx(int grpidx)
{
	unsigned int index = 0, index1 = 0;

	if (grpidx > GINDX_MAX_SIZE)
		index = 1; /* index 1 not possible GINDX_MAX_SIZE=1 */

	for (index1 = 0; index1 < GINDX_MAX_SIZE; index1++) {
		if (grpidx == index1) {
			g_mcast_grpindex[index] &= ~BIT_ULL(grpidx - 1);
			break;
		}
	}
}

/*=============================================================================
 *function name: mcast_helper_get_gimc_record
 *description  : function to get the gimc record
 *===========================================================================*/
static MCAST_GIMC_t *
mcast_helper_get_gimc_record(struct list_head *head, int grpidx)
{
	MCAST_GIMC_t *gimc_rec = NULL;
	struct list_head *liter = NULL, *tliter = NULL;

	list_for_each_safe(liter, tliter, head) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		if (gimc_rec->grpIdx == grpidx) {
			return gimc_rec;
		}
	}

	return NULL;
}

/*=============================================================================
 *Function Name: mcast_helper_search_gimc_record
 *Description	: Function to search gaddr and saddr in the gimc record list
 *===========================================================================*/
static MCAST_GIMC_t *
mcast_helper_search_gimc_record(IP_Addr_t *gaddr,
				IP_Addr_t *saddr,
				struct list_head *head)
{
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_STREAM_t *stream;

	list_for_each_safe(liter, tliter, head) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		stream = &gimc_rec->mc_stream;
		if (!mcast_helper_is_same_ipaddr(&stream->dIP, gaddr))
			continue;
		/* ASM: group IP matched */
		if (!mcast_helper_is_addr_unspecified(saddr) &&
		    !mcast_helper_is_addr_unspecified(&stream->sIP)) {
			if (!mcast_helper_is_same_ipaddr(&stream->sIP, saddr))
				continue;
			/* SSM: source IP matched */
		}
		/* TODO: If VLAN present and not matching continue */
		return gimc_rec;
	}

	return NULL;
}

/**
 * mcast_helper_get_skb_gid_internal - get MCGID from socket buffer
 *
 * @skb: pointer to the socket buffer
 *
 * Return: MCGID (>=0) on success else negative in case of failure
 */
static int mcast_helper_get_skb_gid_internal(struct sk_buff *skb)
{
	int mc_gid = MCH_GID_BCAST;
	MCAST_GIMC_t *gimc_rec;
	struct list_head *gimc_list;
	IP_Addr_t saddr;
	IP_Addr_t daddr;
	struct iphdr *iph;
	struct ipv6hdr *iph6;
	const unsigned char *dest;
	unsigned short network_header_offset;
	unsigned short protocol;
	ptype_t ptype;

	if (!skb_mac_header_was_set(skb)) {
		struct ethhdr *eth_hdr = (struct ethhdr *)skb->data;

		dest = eth_hdr->h_dest;
		protocol = eth_hdr->h_proto;
		network_header_offset = skb->data - skb->head + ETH_HLEN;
	} else {
		dest = eth_hdr(skb)->h_dest;
		protocol = skb->protocol;
		network_header_offset = skb->network_header;
	}

	if (!is_multicast_ether_addr(dest))
		return MCH_GID_ERR;

	if (is_broadcast_ether_addr(dest))
		return MCH_GID_BCAST;

	/* TODO: Handle one or more vlan tag if present */

	switch (ntohs(protocol)) {
	case ETH_P_IP:
		iph = (struct iphdr *)(skb->head + network_header_offset);
		ptype = IPV4;

		mcast_helper_init_ipaddr(&saddr, ptype, &iph->saddr);
		mcast_helper_init_ipaddr(&daddr, ptype, &iph->daddr);
		/* Check link-local multicast space 224.0.0.0/24
		 * and IPv4 Service Discovery Protocol address 239.255.255.250
		 */
		if (ipv4_is_local_multicast(iph->daddr) ||
		    (ntohl(iph->daddr) == 0xEFFFFFFAU)) {
			mch_debug("{%pI4, %pI4} => %d\n",
				  &saddr.ipA, &daddr.ipA, MCH_GID_BCAST);
			return MCH_GID_BCAST;
		}
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case ETH_P_IPV6:
		iph6 = (struct ipv6hdr *)(skb->head + network_header_offset);
		ptype = IPV6;

		mcast_helper_init_ipaddr(&saddr, ptype, &iph6->saddr);
		mcast_helper_init_ipaddr(&daddr, ptype, &iph6->daddr);
		if (ipv6_addr_is_ll_all_nodes(&iph6->daddr) ||
		    IPV6_ADDR_MC_SCOPE(&iph6->daddr) < IPV6_ADDR_SCOPE_LINKLOCAL) {
			mch_debug("{%pI6c, %pI6c} => %d\n",
				  &saddr.ipA, &daddr.ipA, MCH_GID_BCAST);
			return MCH_GID_BCAST;
		}
		break;
#endif /* CONFIG_IPV6 */
	default:
		mch_debug("{*, %pM} => %d\n", dest, MCH_GID_BCAST);
		return MCH_GID_BCAST;
	}

	gimc_list = mcast_helper_list_p(ptype);
	gimc_rec = mcast_helper_search_gimc_record(&daddr, &saddr, gimc_list);
	if (gimc_rec)
		mc_gid = gimc_rec->grpIdx;

	if (ptype == IPV4)
		mch_debug("{%pI4, %pI4} => %d\n", &saddr.ipA, &daddr.ipA, mc_gid);
	else if (ptype == IPV6)
		mch_debug("{%pI6c, %pI6c} => %d\n", &saddr.ipA, &daddr.ipA, mc_gid);

	return mc_gid;
}

/*=============================================================================
 *Function Name: mcast_helper_add_gimc_record
 *Description	: Function  to create and add 5 tuple entry into gimc record list
 *===========================================================================*/
static MCAST_GIMC_t *mcast_helper_add_gimc_record(struct net_device *netdev,
		IP_Addr_t *gaddr, IP_Addr_t *saddr,
		unsigned int proto, unsigned int sport,
		unsigned int dport, unsigned char *src_mac,
		struct list_head *head)
{
	MCAST_GIMC_t *gimc_rec = NULL;
	struct list_head *ses_list = mcast_helper_session_p(saddr->ipType);
	int grpidx = -1;

	grpidx = mcast_helper_allocate_grpidx();
	if (grpidx == -1)
		return NULL;

	gimc_rec = kmalloc(sizeof(MCAST_GIMC_t), GFP_ATOMIC);
	if (gimc_rec == NULL) {
		mch_debug("failed to add GIMC\n");
		return NULL;
	}

	gimc_rec->mc_stream.rxDev = netdev;
	mcast_helper_copy_ipaddr(&(gimc_rec->mc_stream.dIP), gaddr);
	mcast_helper_copy_ipaddr(&(gimc_rec->mc_stream.sIP), saddr);
	gimc_rec->mc_stream.proto = proto;
	gimc_rec->mc_stream.sPort = sport;
	gimc_rec->mc_stream.dPort = dport;

	memcpy(gimc_rec->mc_stream.src_mac, src_mac, ETH_ALEN);

	gimc_rec->grpIdx = grpidx;
	gimc_rec->br_callback_flag = 0;
#ifdef CONFIG_MCAST_HELPER_ACL
	gimc_rec->oifbitmap = 0;
	gimc_rec->probeFlag = 0;
#endif

	memset(gimc_rec->mc_stream.macaddr, 0, MAX_MAC * ETH_ALEN);
	gimc_rec->mc_stream.num_joined_macs = 0;
	INIT_LIST_HEAD(&gimc_rec->list);
	INIT_LIST_HEAD(&gimc_rec->mc_mem_list);
	list_add_tail(&gimc_rec->list, head);
	mch_debug("adding GIMC record=%p RX(%pM, %s) with GID=%d into '%s' list\n",
		  gimc_rec, src_mac, netdev->name, grpidx,
		  (head == ses_list) ? "session" : "member");

	return gimc_rec;
}

/*=============================================================================
 *Function Name: mcast_helper_delete_gimc_record
 *Description	: Function  to delete a gimc record list entry
 *===========================================================================*/

static void mcast_helper_delete_gimc_record(MCAST_GIMC_t *gimc_rec)
{
	if (gimc_rec == NULL)
		return;

	mch_debug("deleting GIMC record=%p for GID=%d\n",
		  gimc_rec, gimc_rec->grpIdx);

	mcast_helper_release_grpidx(gimc_rec->grpIdx);
	list_del(&gimc_rec->list);
	kfree(gimc_rec);
}


/*=============================================================================
 *Function Name: mcast_helper_invoke_return_callback
 *Description	: Function  which invoke the registerd callback from PPA/WLAN ..
 *===========================================================================*/
static int
mcast_helper_invoke_return_callback(unsigned int grpidx,
				    struct net_device *netdev,
				    MCAST_STREAM_t *stream,
				    unsigned int flag, unsigned int count)
{
	if (mch_accl_enabled == 0)
		return SUCCESS;

	if (stream->sIP.ipType == IPV4) {
		mch_debug("callback for(%s): stream source(%pM, %s)# sIP=%pI4,"
			  " dIP=%pI4, proto=%#x, sPort=%d, dPort=%d\n",
			  netdev->name, stream->src_mac, stream->rxDev->name,
			  &stream->sIP.ipA, &stream->dIP.ipA,
			  stream->proto, stream->sPort, stream->dPort);
	} else if (stream->sIP.ipType == IPV6) {
		mch_debug("callback for: stream src(%pM, %s)# sIP=%pI6,"
			  " dIP=%pI6, proto=%#x, sPort=%d, dPort=%d\n",
			  stream->src_mac, stream->rxDev->name,
			  &stream->sIP.ipA, &stream->dIP.ipA,
			  stream->proto, stream->sPort, stream->dPort);
	}

	return mcast_helper_invoke_callback(grpidx, netdev, stream, flag, count);
}

/*=============================================================================
 *function name: mcast_helper_search_mac_record
 *description  : function to search and get mac record based on gitxmc_rec
 *===========================================================================*/
static MCAST_MAC_t *
mcast_helper_search_mac_record(MCAST_MEMBER_t *gitxmc_rec,
			       unsigned char *macaddr)
{
	MCAST_MAC_t *mac_rec = NULL;
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;

	list_for_each_safe(liter, tliter, &gitxmc_rec->macaddr_list) {
		mac_rec = list_entry(liter, MCAST_MAC_t, list);
		if (memcmp(mac_rec->macaddr, macaddr, ETH_ALEN) == 0) {
			return mac_rec;
		}
	}
	return NULL;
}

/*=============================================================================
 *function name: mcast_helper_update_macaddr_record
 *description  : function to update the mac address in gitxmc record
 *===========================================================================*/
static MCAST_MAC_t *
mcast_helper_update_macaddr_record(MCAST_MEMBER_t *gitxmc_rec,
				   unsigned char *macaddr)
{
	MCAST_MAC_t *mac;

	if (gitxmc_rec == NULL || macaddr == NULL)
		return NULL;

	mac = kmalloc(sizeof(MCAST_MAC_t), GFP_ATOMIC);
	if (mac == NULL)
		return NULL;

	memcpy(mac->macaddr, macaddr, sizeof(char)*ETH_ALEN);
	gitxmc_rec->macaddr_count++;

	INIT_LIST_HEAD(&mac->list);
	list_add_tail(&mac->list, &gitxmc_rec->macaddr_list);

	mch_debug("adding MAC=%pM into member=%s, total MAC=%d\n",
		  macaddr, gitxmc_rec->memDev->name, gitxmc_rec->macaddr_count);

	return mac;
}

/*=============================================================================
 *Function Name: mcast_helper_add_gitxmc_record
 *Description	: Function  which add the entry in gitxmc record
 *===========================================================================*/
static MCAST_MEMBER_t *mcast_helper_add_gitxmc_record(MCAST_GIMC_t *gimc_rec,
		struct net_device *netdev, unsigned char *macaddr)
{
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	MCAST_MAC_t *mac_rec = NULL;

	if (gimc_rec == NULL)
		return NULL;

	gitxmc_rec = kmalloc(sizeof(MCAST_MEMBER_t), GFP_ATOMIC);
	if (gitxmc_rec == NULL)
		return NULL;

	mch_debug("adding member=%s into GIMC record=%p for GID=%d\n",
		  netdev->name, gimc_rec, gimc_rec->grpIdx);

	gitxmc_rec->memDev = netdev;
	gitxmc_rec->macaddr_count = 0;

	INIT_LIST_HEAD(&gitxmc_rec->macaddr_list);
	mac_rec = mcast_helper_update_macaddr_record(gitxmc_rec, macaddr);
	if (mac_rec == NULL) {
		kfree(gitxmc_rec);
		return NULL;
	}

	if (mch_acl_enabled) {
#ifdef CONFIG_MCAST_HELPER_ACL
		gitxmc_rec->aclBlocked = 0;
#endif
	}
	INIT_LIST_HEAD(&gitxmc_rec->list);
	list_add_tail(&gitxmc_rec->list, &gimc_rec->mc_mem_list);

	return gitxmc_rec;
}

/*=============================================================================
 * function name : mcast_helper_update_mac_list
 * description   : Function updates the mac list which will be passed
 *                 to registered call back
 *===========================================================================*/
static unsigned int 
mcast_helper_update_mac_list(MCAST_MEMBER_t *gitxmc_rec,
			     MCAST_GIMC_t *gimc_rec,
			     unsigned char *macaddr,
			     unsigned int action)
{
	MCAST_STREAM_t *stream = &gimc_rec->mc_stream;
	MCAST_MAC_t *mac_rec = NULL;
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	unsigned int flag = 0;

	memset(stream->macaddr, 0, MAX_MAC * ETH_ALEN);
	stream->num_joined_macs = 0;

	if ((gitxmc_rec->macaddr_count == 1) && (action == MCH_CB_F_DEL)) {
		if (macaddr != NULL) {
			memcpy(stream->macaddr[0], macaddr, ETH_ALEN);
			stream->num_joined_macs = 1;
		}
		flag = MCH_CB_F_DEL;
	} else {
		list_for_each_safe(liter, tliter, &gitxmc_rec->macaddr_list) {
			mac_rec = list_entry(liter, MCAST_MAC_t, list);
			if (stream->num_joined_macs < MAX_MAC) {
				memcpy(stream->macaddr[stream->num_joined_macs],
				       mac_rec->macaddr, ETH_ALEN);
				stream->num_joined_macs++;
			}
		}
		flag = action;
	}

	return flag;
}

/*=============================================================================
 *Function Name: mcast_helper_delete_gitxmc_record
 *Description  : Function which delete the entry in gitxmc record
 *===========================================================================*/
static void
mcast_helper_delete_gitxmc_record(MCAST_MEMBER_t *gitxmc_rec,
				  MCAST_GIMC_t *gimc_rec,
				  struct net_device *netdev,
				  unsigned char *macaddr, unsigned int action) 
{
	unsigned int flag = 0;
	char mac_str[18] = { 0 };

	if ((gitxmc_rec == NULL) || (gimc_rec == NULL))
		return;

#ifdef CONFIG_MCAST_HELPER_ACL
	if ((mch_acl_enabled == 1) && (gitxmc_rec->aclBlocked == 1)) {
		return;
	}
#endif

	if (macaddr)
		snprintf(mac_str, sizeof(mac_str), "%pM", macaddr);
	mch_debug("deleting %d MAC(%s) from member=%s GID=%d & callback\n",
		  gitxmc_rec->macaddr_count, (macaddr) ? mac_str : "all",
		  gitxmc_rec->memDev->name, gimc_rec->grpIdx);

	flag = mcast_helper_update_mac_list(gitxmc_rec, gimc_rec,
					    macaddr, action);
	mcast_helper_invoke_return_callback(gimc_rec->grpIdx,
					    netdev, &gimc_rec->mc_stream,
					    flag, gitxmc_rec->macaddr_count);
}

/*=============================================================================
 *Function Name: mcast_helper_search_gitxmc_record
 *Description	: Function  to search and get the gitxmc recod based on netdev name
 *===========================================================================*/
static MCAST_MEMBER_t *mcast_helper_search_gitxmc_record(struct list_head *head,
		struct net_device *netdev)
{
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;

	list_for_each_safe(liter, tliter, head) {
		gitxmc_rec = list_entry(liter, MCAST_MEMBER_t, list);
		if (gitxmc_rec->memDev == netdev)
			return gitxmc_rec;
	}

	return NULL;
}

/*=============================================================================
 *Function Name: mcast_helper_make_skb_writeable
 *Description	: Function  to make skb writeable
 *===========================================================================*/
static int mcast_helper_make_skb_writeable(struct sk_buff *skb, int write_len)
{
	if (!pskb_may_pull(skb, write_len))
		return -ENOMEM;

	if (!skb_cloned(skb) || skb_clone_writable(skb, write_len))
		return 0;

	return pskb_expand_head(skb, 0, 0, GFP_ATOMIC);
}

/*=============================================================================
 *Function Name: mcast_helper_set_ip_addr
 *Description	: Function  to set ip address in the skb
 *===========================================================================*/
static void mcast_helper_set_ip_addr(struct sk_buff *skb, struct iphdr *nh,
		unsigned int  *addr, unsigned int  new_addr)
{
	int transport_len = skb->len - skb_transport_offset(skb);

	if (nh->protocol == IPPROTO_TCP) {
		if (likely(transport_len >= sizeof(struct tcphdr)))
			inet_proto_csum_replace4(&tcp_hdr(skb)->check, skb,
					*addr, new_addr, 1);
	} else if (nh->protocol == IPPROTO_UDP) {
		if (likely(transport_len >= sizeof(struct udphdr))) {
			struct udphdr *uh = udp_hdr(skb);

			if (uh->check || skb->ip_summed == CHECKSUM_PARTIAL) {
				inet_proto_csum_replace4(&uh->check, skb,
						*addr, new_addr, 1);
				if (!uh->check)
					uh->check = CSUM_MANGLED_0;
			}
		}
	}

	csum_replace4(&nh->check, *addr, new_addr);

	*addr = new_addr;
}

/*=============================================================================
 *function name: mcast_helper_update_ipv6_checksum
 *description	: function  to update ipv6 checksum in the passed skb
 *===========================================================================*/
static void mcast_helper_update_ipv6_checksum(struct sk_buff *skb,
		unsigned int  addr[4], unsigned int  new_addr[4])
{
	int transport_len = skb->len - skb_transport_offset(skb);
	if (likely(transport_len >= sizeof(struct udphdr))) {
		struct udphdr *uh = udp_hdr(skb);

		if (uh->check || skb->ip_summed == CHECKSUM_PARTIAL) {
			inet_proto_csum_replace16(&uh->check, skb,
					addr, new_addr, 1);
			if (!uh->check)
				uh->check = CSUM_MANGLED_0;
		}
	}
}

/*=============================================================================
 *function name: mcast_helper_set_ipv6_addr
 *description	: function  to set the ipaddress in the skb
 *===========================================================================*/
static void mcast_helper_set_ipv6_addr(struct sk_buff *skb,
		unsigned int addr[4], unsigned int new_addr[4],
		bool recalculate_csum)
{
	if (recalculate_csum)
		mcast_helper_update_ipv6_checksum(skb, addr, new_addr);

	memcpy(addr, new_addr, sizeof(__be32[4]));
}

/*=============================================================================
 *function name: mcast_helper_set_ipv6
 *description  : function  to update the ipv6 address in skb
 *===========================================================================*/
static int mcast_helper_set_ipv6(struct sk_buff *skb,
				 IP_Addr_t *new_saddr, IP_Addr_t *new_daddr)
{
	struct ipv6hdr *nh;
	int err;
	unsigned int  *saddr;
	unsigned int *daddr;

	err = mcast_helper_make_skb_writeable(skb, skb_network_offset(skb) +
			sizeof(struct ipv6hdr));
	if (unlikely(err))
		return err;

	nh = ipv6_hdr(skb);
	saddr = (unsigned int *)&nh->saddr;
	daddr = (unsigned int *)&nh->daddr;

	if (memcmp(&(new_saddr->ipA.ip6Addr), saddr, sizeof(struct in6_addr)))
		mcast_helper_set_ipv6_addr(skb, saddr,
				(unsigned int *)&(new_saddr->ipA.ip6Addr), true);
	if (memcmp(&(new_daddr->ipA.ip6Addr), daddr, sizeof(struct in6_addr)))
		mcast_helper_set_ipv6_addr(skb, daddr,
				(unsigned int *)&(new_daddr->ipA.ip6Addr), true);


	return 0;
}

/*=============================================================================
 *function name: mcast_helper_set_ipv4
 *description  : function  to update the ipv4 address in skb
 *===========================================================================*/
static int mcast_helper_set_ipv4(struct sk_buff *skb,
				 IP_Addr_t *saddr, IP_Addr_t *daddr)
{
	struct iphdr *nh;
	int err;

	err = mcast_helper_make_skb_writeable(skb, skb_network_offset(skb) +
					      sizeof(struct iphdr));
	if (unlikely(err))
		return err;

	nh = ip_hdr(skb);

	if (saddr->ipA.ipAddr.s_addr != nh->saddr)
		mcast_helper_set_ip_addr(skb, nh, &nh->saddr,
					 saddr->ipA.ipAddr.s_addr);

	if (daddr->ipA.ipAddr.s_addr != nh->daddr)
		mcast_helper_set_ip_addr(skb, nh, &nh->daddr,
					 daddr->ipA.ipAddr.s_addr);

	return 0;
}

/*=============================================================================
 *function name: mcast_helper_set_port
 *description  : function  to update the port info in skb
 *===========================================================================*/
static void mcast_helper_set_port(struct sk_buff *skb, unsigned short *port,
		unsigned short new_port, unsigned short *check)
{
	inet_proto_csum_replace2(check, skb, *port, new_port, 0);
	*port = new_port;
}

/*=============================================================================
 *function name: mcast_helper_set_udp_port
 *description  : function  to update the udp port in skb
 *===========================================================================*/
static void mcast_helper_set_udp_port(struct sk_buff *skb, unsigned short *port,
				      unsigned short new_port)
{
	struct udphdr *uh = udp_hdr(skb);

	if (uh->check && skb->ip_summed != CHECKSUM_PARTIAL) {
		mcast_helper_set_port(skb, port, new_port, &uh->check);

		if (!uh->check)
			uh->check = CSUM_MANGLED_0;
	} else {
		*port = new_port;
	}
}

/*=============================================================================
 *function name: mcast_helper_set_udp
 *description  : function  to update the udp header in skb
 *===========================================================================*/
static void mcast_helper_set_udp(struct sk_buff *skb, unsigned short udp_src,
				 unsigned short udp_dst)
{
	struct udphdr *uh;
	int err;

	err = mcast_helper_make_skb_writeable(skb, skb_transport_offset(skb) +
					      sizeof(struct udphdr));
	if (unlikely(err))
		return;

	uh = udp_hdr(skb);
	if (udp_src != uh->source)
		mcast_helper_set_udp_port(skb, &uh->source, udp_src);

	if (udp_dst != uh->dest)
		mcast_helper_set_udp_port(skb, &uh->dest, udp_dst);

	return;
}

/*
   static int mcast_helper_set_eth_addr(struct sk_buff *skb,
   unsigned char *eth_src, unsigned char *eth_dst)
   {
   int err;
   err = mcast_helper_make_skb_writeable(skb, ETH_HLEN);
   if (unlikely(err))
   return err;


   ether_addr_copy(eth_hdr(skb)->h_source, eth_src);
   ether_addr_copy(eth_hdr(skb)->h_dest, eth_dst);

   skb_postpull_rcsum(skb, eth_hdr(skb), ETH_ALEN *2);

   return 0;
   }
 */

/*=============================================================================
 *function name: mcast_helper_set_sig
 *description  : function  to insert the signature in skb
 *===========================================================================*/
static void mcast_helper_set_sig(struct sk_buff *skb, struct net_device *netdev,
				 int grpidx, int flag)
{
	unsigned char *data = NULL;
	unsigned int data_len = 0;
	unsigned int extra_data_len = -1;
	int index = 0;
	if (flag == IPV6)
		data_len = skb->len - TOT6_HDR_LEN;
	else
		data_len = skb->len - TOT_HDR_LEN;
	extra_data_len =  sizeof(mch_signature)+8;
	index = sizeof(mch_signature) -1;
	data = (unsigned char *)udp_hdr(skb) + UDP_HDR_LEN;
	if (data_len > extra_data_len) {
		memcpy(data, mch_signature, sizeof(mch_signature));
		data[index] = (unsigned char)(grpidx & 0xFF);
		data[index+1] = (unsigned char)(netdev->ifindex & 0xFF);
	}

}

/*=============================================================================
 *function name: mcast_helper_acl_probe_pckt_send
 *description  : function  to send the IPV4 probe packet
 *===========================================================================*/
static unsigned int mcast_helper_acl_probe_pckt_send(struct net_device *inetdev,
		struct net_device *onetdev,
		int grpidx, IP_Addr_t *gaddr, IP_Addr_t *saddr,
		unsigned int proto, unsigned int sport,
		unsigned int dport)
{
	struct iphdr *iph = NULL;
	struct sk_buff *newskb = NULL;
	if (skb_buff == NULL)
		return 0;
	if (ip_hdr(skb_buff)->protocol == IPPROTO_UDP) {
		newskb = skb_copy(skb_buff, GFP_ATOMIC);
		if (newskb != NULL) {
			iph = (struct iphdr *) skb_network_header(newskb);

			mcast_helper_set_ipv4(newskb, saddr, gaddr);
			mcast_helper_set_udp(newskb, sport, dport);
			//mcast_helper_set_eth_addr(newskb, newskb->dev->dev_addr, eth_hdr(newskb)->h_dest);
			newskb->dev = inetdev;
			mcast_helper_set_sig(newskb, onetdev, grpidx, IPV4);
			netif_receive_skb(newskb); // insert the skb in to  input queue
			return 1;
		}
	}

	return 0;
}

/*=============================================================================
 *function name: mcast_helper_acl_probe_pckt_send6
 *description  : function  to send the IPV6 probe packet
 *===========================================================================*/
static unsigned int mcast_helper_acl_probe_pckt_send6(struct net_device *inetdev,
		struct net_device *onetdev,
		int grpidx, IP_Addr_t *gaddr, IP_Addr_t *saddr,
		unsigned int proto, unsigned int sport,
		unsigned int dport)

{
	struct sk_buff *newskb = NULL;

	if (skb_buff6) {
		if (ipv6_hdr(skb_buff6)->nexthdr == IPPROTO_UDP) {
			newskb = skb_copy(skb_buff6, GFP_ATOMIC);
			if (newskb != NULL) {
				mcast_helper_set_ipv6(newskb, saddr, gaddr);
				mcast_helper_set_udp(newskb, sport, dport);
#if 0
				mcast_helper_set_eth_addr(newskb,
						newskb->dev->dev_addr,
						eth_hdr(newskb)->h_dest);
#endif
				newskb->dev = inetdev;
				mcast_helper_set_sig(newskb, onetdev,
						     grpidx, IPV6);
				/* insert the skb in to input queue */
				netif_receive_skb(newskb);
			}
		}
	}


	return NF_ACCEPT;
}


/*=============================================================================
 *function name: mcast_helper_update_entry
 *description  : function searches the gimc record and then updated the gitx
 *               record and then send the probe packets by starting the
 *               proble pckt expiry timer
 *===========================================================================*/

/*call the function to check if GI exist for this group in GIMc table */
static int
mcast_helper_update_entry(struct net_device *netdev, MCAST_REC_t *mc_rec)
{
	int ret = 0;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	MCAST_MAC_t *mac_rec = NULL;
	unsigned int flag = MCH_CB_F_UPD;
	struct list_head *gimc_list = NULL;

	gimc_list = mcast_helper_list_p(mc_rec->groupIP.ipType);
	gimc_rec = mcast_helper_search_gimc_record(&mc_rec->groupIP,
			&mc_rec->srcIP, gimc_list);
	if (gimc_rec == NULL) {
		return FAILURE;
	}

	/* Update the GIMcTx table to add the new interface into the list */
	gitxmc_rec = mcast_helper_search_gitxmc_record(&gimc_rec->mc_mem_list,
			netdev);
	if (gitxmc_rec == NULL) {
		gitxmc_rec = mcast_helper_add_gitxmc_record(gimc_rec, netdev,
				mc_rec->macaddr);
		if (gitxmc_rec == NULL)
			return FAILURE;
	} else {
		if (mcast_helper_search_mac_record(gitxmc_rec,
		    mc_rec->macaddr) == NULL) {
			mac_rec = mcast_helper_update_macaddr_record(gitxmc_rec,
					mc_rec->macaddr);
			if (mac_rec == NULL)
				return FAILURE;
		}
	}

	if (mch_acl_enabled) {
		/* Start the timer here */
		mcast_helper_start_helper_timer();

#ifdef CONFIG_MCAST_HELPER_ACL
		gimc_rec->probeFlag = 1;
#endif
		/*Send the Skb probe packet on interfaces */
		if (gimc_rec->mc_stream.sIP.ipType == IPV6) {
			mch_iptype = IPV6;
			mcast_helper_acl_probe_pckt_send6(
					gimc_rec->mc_stream.rxDev, netdev,
					gimc_rec->grpIdx,
					&gimc_rec->mc_stream.dIP,
					&gimc_rec->mc_stream.sIP,
					gimc_rec->mc_stream.proto,
					gimc_rec->mc_stream.sPort,
					gimc_rec->mc_stream.dPort);

		} else {
			mch_iptype = IPV4;
			ret = mcast_helper_acl_probe_pckt_send(
					gimc_rec->mc_stream.rxDev,
					netdev, gimc_rec->grpIdx,
					&gimc_rec->mc_stream.dIP,
					&gimc_rec->mc_stream.sIP,
					gimc_rec->mc_stream.proto,
					gimc_rec->mc_stream.sPort,
					gimc_rec->mc_stream.dPort);
			if (ret == 0)
				return FAILURE;
		}
	} else {
		flag = mcast_helper_update_mac_list(gitxmc_rec,
				gimc_rec, mc_rec->macaddr, flag);
		mcast_helper_invoke_return_callback(gimc_rec->grpIdx, netdev,
				&gimc_rec->mc_stream, flag,
				gitxmc_rec->macaddr_count);

	}

	mcast_helper_start_lanserver_timer();

	return SUCCESS;
}


/*=============================================================================
 *function name: mcast_helper_add_entry
 *description  : function searches and adds the entry in gimc/gitcmc record
 *               based on the prameters received  from user space
 *===========================================================================*/
static int mcast_helper_add_entry(struct net_device *netdev,
				  struct net_device *rxnetdev,
				  bool routed, MCAST_REC_t *mc_rec,
				  unsigned char *src_mac)
{
	int ret=0;
	unsigned int flag = 0;
	MCAST_GIMC_t *gimc_rec = NULL, *gimc_rec_l = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	struct list_head *gimc_list = NULL;
	struct list_head *gimc_list_l = NULL;
	__be32 s_addr = ntohl(mc_rec->groupIP.ipA.ipAddr.s_addr);

	/*
	 * Ignore Multicast control packets(224.0.0.x)
	 * and UPnP packets(239.255.255.250)
	 */
	if (((s_addr & 0xffffff00U) == 0xe0000000U) ||
	    ((s_addr & 0xffffffffU) == 0xeffffffaU)) {
		return FAILURE;
	}

	gimc_list = mcast_helper_list_p(mc_rec->groupIP.ipType);
	gimc_list_l = mcast_helper_session_p(mc_rec->groupIP.ipType);
	gimc_rec = mcast_helper_search_gimc_record(&mc_rec->groupIP,
			&mc_rec->srcIP, gimc_list);
	if (gimc_rec == NULL) {
		gimc_rec = mcast_helper_add_gimc_record(rxnetdev,
				&mc_rec->groupIP, &mc_rec->srcIP, mc_rec->proto,
				mc_rec->sPort, mc_rec->dPort, src_mac, gimc_list);
		if (gimc_rec == NULL) {
			return FAILURE;
		}
		/* For LAN server case stream will be in session list */
		gimc_rec_l = mcast_helper_search_gimc_record(&mc_rec->groupIP,
				&mc_rec->srcIP, gimc_list_l);
		if (gimc_rec_l) {
			mch_debug("found GIMC record=%p GID=%d in lanserver"
				  " list, updating into helper GIMC record=%p\n",
				  gimc_rec_l, gimc_rec_l->grpIdx, gimc_rec);
			/* Use same GID from session list */
			mcast_helper_release_grpidx(gimc_rec->grpIdx);
			gimc_rec->grpIdx = gimc_rec_l->grpIdx;
			gimc_rec->br_callback_flag = gimc_rec_l->br_callback_flag;
#ifdef CONFIG_MCAST_HELPER_ACL
			gimc_rec->oifbitmap = gimc_rec_l->oifbitmap;
			gimc_rec->probeFlag = gimc_rec_l->probeFlag;
#endif
			memcpy(&gimc_rec->mc_stream, &gimc_rec_l->mc_stream,
			       sizeof(MCAST_STREAM_t));
		}
	}

	/* Update the GIMcTx table to add the new interface into the list */
	gitxmc_rec = mcast_helper_search_gitxmc_record(&gimc_rec->mc_mem_list,
			netdev);
	if (gitxmc_rec)
		return SUCCESS;

	gitxmc_rec = mcast_helper_add_gitxmc_record(gimc_rec, netdev,
			mc_rec->macaddr);
	if (gitxmc_rec == NULL)
		return FAILURE;

	/* Just now member created adding single MAC */
	memcpy(gimc_rec->mc_stream.macaddr, mc_rec->macaddr, ETH_ALEN);
	if (routed) {
		flag = mcast_helper_update_mac_list(gitxmc_rec, gimc_rec,
				mc_rec->macaddr, MCH_CB_F_ADD);
		mch_debug("added member for routed path into GID=%d\n",
			  gimc_rec->grpIdx);
		atomic_inc(&mch_br_capture_pkt);
	} else {
		if (mch_acl_enabled) {
			/* Start the timer here */
			mcast_helper_start_helper_timer();

#ifdef CONFIG_MCAST_HELPER_ACL
			gimc_rec->probeFlag = 1;
#endif
			/* Send the Skb probe packet on interfaces */
			if (gimc_rec->mc_stream.sIP.ipType == IPV6) {
				mch_iptype = IPV6;
				mcast_helper_acl_probe_pckt_send6(
						gimc_rec->mc_stream.rxDev,
						netdev, gimc_rec->grpIdx,
						&(gimc_rec->mc_stream.dIP),
						&(gimc_rec->mc_stream.sIP),
						gimc_rec->mc_stream.proto,
						gimc_rec->mc_stream.sPort,
						gimc_rec->mc_stream.dPort);

			} else {
				mch_iptype = IPV4;
				ret = mcast_helper_acl_probe_pckt_send(
						gimc_rec->mc_stream.rxDev,
						netdev, gimc_rec->grpIdx,
						&(gimc_rec->mc_stream.dIP),
						&(gimc_rec->mc_stream.sIP),
						gimc_rec->mc_stream.proto,
						gimc_rec->mc_stream.sPort,
						gimc_rec->mc_stream.dPort);
				if (ret == 0)
					return FAILURE;
			}
		} else {
			mch_debug("added member for LAN server path into"
				  " GID=%d & callback\n", gimc_rec->grpIdx);
			if (gimc_rec_l)
				gimc_rec_l->br_callback_flag = 1;
			gimc_rec->br_callback_flag = 1;
			flag = mcast_helper_update_mac_list(gitxmc_rec,
					gimc_rec, mc_rec->macaddr, MCH_CB_F_ADD);
			mcast_helper_invoke_return_callback(gimc_rec->grpIdx,
					netdev, &gimc_rec->mc_stream, MCH_CB_F_ADD,
					gitxmc_rec->macaddr_count);
	        }
	}

	mcast_helper_start_lanserver_timer();

	return SUCCESS;
}

/*=============================================================================
 *function name: mcast_helper_delete_entry
 *description  : function searches and delted the entry in gimc/gitcmc record
 *               based on the prameters received  from user space
 *===========================================================================*/

/*call the function to check if GI exist for this group in GIMc table */
static int
mcast_helper_delete_entry(struct net_device *netdev, MCAST_REC_t *mc_mem)
{
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	struct list_head *gimc_list = NULL;
	MCAST_MAC_t *mac = NULL;

	gimc_list = mcast_helper_list_p(mc_mem->groupIP.ipType);
	gimc_rec = mcast_helper_search_gimc_record(&mc_mem->groupIP,
			&mc_mem->srcIP, gimc_list);
	if (gimc_rec == NULL)
		return FAILURE;

	/* Update the GIMcTx table to del mcast member mapping table */
	gitxmc_rec = mcast_helper_search_gitxmc_record(&gimc_rec->mc_mem_list,
			netdev);
	if (gitxmc_rec == NULL)
		return FAILURE;

	mac = mcast_helper_search_mac_record(gitxmc_rec, mc_mem->macaddr); 
	mch_debug("deleting MAC=%pM from member=%s GID=%d total MAC=%d\n",
		  (mac) ? mac->macaddr : NULL, gitxmc_rec->memDev->name,
		  gimc_rec->grpIdx, gitxmc_rec->macaddr_count);
	if (mac && gitxmc_rec->macaddr_count <= 1) {
		/* MAC count 1 and matching, inform and remove */
		mcast_helper_delete_gitxmc_record(gitxmc_rec, gimc_rec,
				netdev, mc_mem->macaddr, MCH_CB_F_DEL);
		list_del(&mac->list);
		kfree(mac);
		mch_debug("deleting member=%s from GIMC=%p GID=%d\n",
			  gitxmc_rec->memDev->name, gimc_rec, gimc_rec->grpIdx);
		list_del(&gitxmc_rec->list);
		kfree(gitxmc_rec);
	} else if (mac) {
		list_del(&mac->list);
		kfree(mac);
		gitxmc_rec->macaddr_count--;
		mcast_helper_delete_gitxmc_record(gitxmc_rec, gimc_rec,
				netdev, mc_mem->macaddr, MCH_CB_F_DEL_UPD);
	}

	if (list_empty(&gimc_rec->mc_mem_list))
		mcast_helper_delete_gimc_record(gimc_rec);

	mcast_helper_start_lanserver_timer();

	return SUCCESS;
}


/*=============================================================================
 *function name: mcast_helper_update_ftuple_info
 *description  : function to update the five tuple info
 *===========================================================================*/
static void
mcast_helper_update_ftuple_info(MCAST_REC_t *mcast_rec, unsigned char *src_mac)
{
	int index;
	FTUPLE_INFO_t *tuple = NULL;

	if (mcast_rec->groupIP.ipType == IPV4) {
		tuple = ftuple_info;
	} else if (mcast_rec->groupIP.ipType == IPV6) {
		tuple = ftuple_info6;
	} else {
		return;
	}

	for (index = 0; index < FTUPLE_ARR_SIZE; index++) {
		if (!strcmp(mcast_rec->rxIntrfName, tuple[index].rxIntrfName) &&
		    mcast_helper_is_same_ipaddr(
		    &mcast_rec->groupIP, &tuple[index].groupIP) &&
		    mcast_helper_is_same_ipaddr(
		    &mcast_rec->srcIP, &tuple[index].srcIP)) {
			mcast_rec->proto = tuple[index].proto;
			mcast_rec->sPort = tuple[index].sPort;
			mcast_rec->dPort = tuple[index].dPort;
			memcpy(src_mac, tuple[index].src_mac, ETH_ALEN);
			break;
		}
	}
}


/*=============================================================================
 *function name: mcast_helper_ioctl
 *description  : IOCTL handler functon
 *===========================================================================*/
static long mcast_helper_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	MCAST_REC_t mcast_mem;
	struct net_device *netdev = NULL;
	struct net_device *rxnetdev = NULL;
	struct net_device *upper_dev = NULL;
	MCAST_GIMC_t *gimc_rec = NULL;
	struct list_head *gimc_list = NULL;
	unsigned char s_mac[ETH_ALEN] = {0};
	bool routed = false;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	switch (cmd) {
	case MCH_MEMBER_ENTRY_ADD:
		if (copy_from_user(&mcast_mem, (MCAST_REC_t *)arg,
		    sizeof(MCAST_REC_t))) {
			return -EACCES;
		}
		netdev = mch_get_netif(mcast_mem.memIntrfName);
		rxnetdev = mch_get_netif(mcast_mem.rxIntrfName);
		if (rxnetdev == NULL || netdev == NULL)
			return -ENXIO;

		rtnl_lock();
		upper_dev = netdev_master_upper_dev_get(rxnetdev);
		rtnl_unlock();
		if (upper_dev) {
			if (!(upper_dev->priv_flags & IFF_EBRIDGE)) {
				mcast_helper_update_ftuple_info(&mcast_mem,
								s_mac);
				routed = true;
			}
		} else {
			mcast_helper_update_ftuple_info(&mcast_mem, s_mac);
			routed = true;
		}
		mcast_helper_add_entry(netdev, rxnetdev,
				       routed, &mcast_mem, s_mac);
		break;
	case MCH_MEMBER_ENTRY_UPDATE:
		if (copy_from_user(&mcast_mem, (MCAST_REC_t *)arg,
		    sizeof(MCAST_REC_t))) {
			return -EACCES;
		}
		netdev = mch_get_netif(mcast_mem.memIntrfName);
		if (netdev == NULL)
			return -ENXIO;

		mcast_helper_update_entry(netdev, &mcast_mem);
		break;
	case  MCH_MEMBER_ENTRY_REMOVE:
		if (copy_from_user(&mcast_mem, (MCAST_REC_t *)arg,
		    sizeof(MCAST_REC_t))) {
			return -EACCES;
		}
		netdev = mch_get_netif(mcast_mem.memIntrfName);
		if (netdev == NULL)
			return -ENXIO;

		mcast_helper_delete_entry(netdev, &mcast_mem);
		break;
	case MCH_SERVER_ENTRY_GET:
		if (copy_from_user(&mcast_mem, (MCAST_REC_t *)arg,
		    sizeof(MCAST_REC_t))) {
			return -EACCES;
		}
		gimc_list = mcast_helper_session_p(mcast_mem.groupIP.ipType);
		gimc_rec = mcast_helper_search_gimc_record(&mcast_mem.groupIP,
				&mcast_mem.srcIP, gimc_list);
		if (gimc_rec == NULL) {
			return -EACCES;
		} else {
			memcpy(mcast_mem.rxIntrfName,
			       gimc_rec->mc_stream.rxDev->name, IFNAMSIZ);
		}
		if (copy_to_user((MCAST_REC_t *)arg,
		    &mcast_mem, sizeof(MCAST_REC_t))) {
			return -EACCES;
		}
		break;

	default:
		return -EINVAL;
	}
	return 0;
}

#ifdef CONFIG_PROC_FS
/*=============================================================================
 *function name: mcast_helper_show_ipv4
 *description  : proc support to read and output the IPv4 table entries
 *===========================================================================*/
static int
mcast_helper_show_ipv4(struct seq_file *seq, struct list_head *gimc_list)
{
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	struct list_head *gliter = NULL;
	struct list_head *iter = NULL;
	struct list_head *gliter_mac = NULL;
	struct list_head *iter_mac = NULL;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	MCAST_MAC_t *mac_rec = NULL;

	list_for_each_safe(liter, tliter, gimc_list) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		if (gimc_rec->mc_stream.dIP.ipType == IPV4) {
			seq_printf(seq, "%3d %15s %10x %10x %6d %6d %6d (%pM)",
				   gimc_rec->grpIdx,
				   gimc_rec->mc_stream.rxDev->name,
				   gimc_rec->mc_stream.sIP.ipA.ipAddr.s_addr,
				   gimc_rec->mc_stream.dIP.ipA.ipAddr.s_addr,
				   gimc_rec->mc_stream.proto,
				   gimc_rec->mc_stream.sPort,
				   gimc_rec->mc_stream.dPort,
				   gimc_rec->mc_stream.src_mac);
		}

		list_for_each_safe(gliter, iter, &gimc_rec->mc_mem_list) {
			gitxmc_rec = list_entry(gliter, MCAST_MEMBER_t, list);
			list_for_each_safe(gliter_mac, iter_mac,
					   &gitxmc_rec->macaddr_list) {
				mac_rec = list_entry(gliter_mac,
						     MCAST_MAC_t, list);
				seq_printf(seq, " %s(%pM)",
					   gitxmc_rec->memDev->name,
					   mac_rec->macaddr);
#ifdef CONFIG_MCAST_HELPER_ACL
				if (mch_acl_enabled) {
					seq_printf(seq, "(%d)",
						   gitxmc_rec->aclBlocked);
				}
#endif
			}
		}
		seq_printf(seq, "\n");
	}

	return 0;
}

/*=============================================================================
 *function name: mcast_helper_seq_show
 *description  : proc support to read and output IPV4 mcast helper record
 *===========================================================================*/
static int mcast_helper_seq_show(struct seq_file *seq, void *v)
{
	struct list_head *gimc_list = mcast_helper_list_p(IPV4);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	seq_printf(seq, "%3s %10s %11s %10s %9s %6s %6s %12s %24s%s\n",
		   "GIdx", "RxIntrf", "SA", "GA", "proto", "sPort",
		   "dPort", "sMAC", "memIntrf(MacAddr)",
		   (mch_acl_enabled) ? "(AclFlag)" : "");

	mcast_helper_show_ipv4(seq, gimc_list);

	return 0;
}

/*=============================================================================
 *function name: mcast_lanserver_seq_show
 *description  : proc support to read and output IPV4 mcast lanserver record
 *===========================================================================*/
static int mcast_lanserver_seq_show(struct seq_file *seq, void *v)
{
	struct list_head *gimc_list = mcast_helper_session_p(IPV4) ;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	seq_printf(seq, "%3s %10s %11s %10s %9s %6s %6s %12s\n", "GIdx",
		   "RxIntrf", "SA", "GA", "proto", "sPort", "dPort", "sMAC");

	mcast_helper_show_ipv4(seq, gimc_list);

	return 0;
}

/*=============================================================================
 *function name: mcast_helper_show_ipv6
 *description  : proc support to read and output the IPv6 table entries
 *===========================================================================*/
static int
mcast_helper_show_ipv6(struct seq_file *seq, struct list_head *gimc_list)
{
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	struct list_head *gliter = NULL;
	struct list_head *iter = NULL;
	struct list_head *gliter_mac = NULL;
	struct list_head *iter_mac = NULL;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	MCAST_MAC_t *mac_rec = NULL;

	list_for_each_safe(liter, tliter, gimc_list) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		seq_printf(seq, "%3d %15s %pI6 %pI6 %6d %6d %6d (%pM)",
			   gimc_rec->grpIdx, gimc_rec->mc_stream.rxDev->name,
			   gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr,
			   gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr,
			   gimc_rec->mc_stream.proto, gimc_rec->mc_stream.sPort,
			   gimc_rec->mc_stream.dPort,
			   gimc_rec->mc_stream.src_mac);

		list_for_each_safe(gliter, iter, &gimc_rec->mc_mem_list) {
			gitxmc_rec = list_entry(gliter, MCAST_MEMBER_t, list);
			list_for_each_safe(gliter_mac, iter_mac,
					   &gitxmc_rec->macaddr_list) {
				mac_rec = list_entry(gliter_mac,
						     MCAST_MAC_t, list);
				seq_printf(seq, " %s(%pM)",
					   gitxmc_rec->memDev->name,
					   mac_rec->macaddr);
#ifdef CONFIG_MCAST_HELPER_ACL
				if (mch_acl_enabled) {
					seq_printf(seq, "(%d)",
						   gitxmc_rec->aclBlocked);
				}
#endif
			}
		}
		seq_printf(seq, "\n");

	}

	return 0;
}

/*=============================================================================
 *function name: mcast_helper_seq_show6
 *description  : proc support to read and output IPV6 mcast helper record
 *===========================================================================*/
static int mcast_helper_seq_show6(struct seq_file *seq, void *v)
{
	struct list_head *gimc_list = mcast_helper_list_p(IPV6) ;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	seq_printf(seq, "%3s %10s %26s %38s\t\t %9s %6s %6s %12s %24s%s\n",
		   "GIdx", "RxIntrf", "SA", "GA", "proto", "sPort",
		   "dPort", "sMAC", "memIntrf(MacAddr)",
		   (mch_acl_enabled) ? "(AclFlag)" : "");

	mcast_helper_show_ipv6(seq, gimc_list);

	return 0;
}

/*=============================================================================
 *function name: mcast_lanserver_seq_show6
 *description  : proc support to read and output IPV6 mcast lanserver record
 *===========================================================================*/
static int mcast_lanserver_seq_show6(struct seq_file *seq, void *v)
{
	struct list_head *gimc_list = mcast_helper_session_p(IPV6) ;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	seq_printf(seq, "%3s %10s %26s %38s\t\t %9s %6s %6s %12s\n", "GIdx",
		   "RxIntrf", "SA", "GA", "proto", "sPort", "dPort", "sMAC");

	mcast_helper_show_ipv6(seq, gimc_list);

	return 0;
}

/*=============================================================================
 *function name: mcast_debug_seq_show
 *description  : proc support to read and output debug status
 *===========================================================================*/
static int mcast_debug_seq_show(struct seq_file *m, void *v)
{
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	seq_printf(m, "MCH: debug is %sabled\n", (mcast_debug)? "en" : "dis");

	return 0;
}

/*=============================================================================
 *function name: mcast_debug_proc_write
 *description  : proc support to enable/disable (1/0) debug level
 *===========================================================================*/
static ssize_t mcast_debug_proc_write(struct file *file, const char *buffer,
				size_t length, loff_t *offset)
{
	int ret = 0;
	char *wbuf = NULL;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	wbuf = kmalloc(length, GFP_KERNEL);
	if (wbuf == NULL) {
		printk(KERN_ERR "MCH: Cannot allocate memory!\n");
		return -EFAULT;
	}

	if (copy_from_user(wbuf, buffer, length)) {
		printk(KERN_ERR "MCH: Cannot copy buffer from user space!\n");
		kfree(wbuf);
		return -EFAULT;
	}
	wbuf[length - 1] = '\0';

	if ((wbuf[0] == '0') || (wbuf[0] == '1')) {
		mcast_debug = wbuf[0] - '0';
		printk(KERN_INFO "MCH: Debug level is now %sabled\n",
		       (mcast_debug) ? "en" : "dis");
	} else {
		printk(KERN_ERR "MCH: invalid value - 0/1 (enable/disable)\n");
	}

	ret = length;

	kfree(wbuf);
	*offset += ret;
	return ret;
}

/*=============================================================================
 *function name: helper_proc_open
 *description  : function to open helper proc for ipv4 table entries
 *===========================================================================*/
int helper_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, mcast_helper_seq_show, NULL);
}

/*=============================================================================
 *function name: lanserver_proc_open
 *description  : function to open lanserver proc for ipv4 table entries
 *===========================================================================*/
int lanserver_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, mcast_lanserver_seq_show, NULL);
}

/*=============================================================================
 *function name: helper_proc_open6
 *description  : function to open helper proc for ipv6 table entries
 *===========================================================================*/
int helper_proc_open6(struct inode *inode, struct file *file)
{
	return single_open(file, mcast_helper_seq_show6, NULL);
}

/*=============================================================================
 *function name: lanserver_proc_open6
 *description  : function to open lanserver proc for ipv6 table entries
 *===========================================================================*/
int lanserver_proc_open6(struct inode *inode, struct file *file)
{
	return single_open(file, mcast_lanserver_seq_show6, NULL);
}

/*=============================================================================
 *function name: mcast_debug_proc_open
 *description  : function to open debug proc for debug prints
 *===========================================================================*/
static int mcast_debug_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, mcast_debug_seq_show, NULL);
}

static const struct file_operations mcast_helper_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= helper_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static const struct file_operations mcast_helper_seq_fops6 = {
	.owner		= THIS_MODULE,
	.open		= helper_proc_open6,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static const struct file_operations mcast_lanserver_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= lanserver_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static const struct file_operations mcast_lanserver_seq_fops6 = {
	.owner		= THIS_MODULE,
	.open		= lanserver_proc_open6,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static const struct file_operations mcast_debug_fops = {
	.owner		= THIS_MODULE,
	.open		= mcast_debug_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= mcast_debug_proc_write,
};

/*=============================================================================
 *function name: mcast_helper_net_init
 *description  : function to create mcast helper proc entry
 *===========================================================================*/
static int mcast_helper_net_init(void)
{
	struct proc_dir_entry *pdh = NULL, *pdh6 = NULL;
	struct proc_dir_entry *pdl = NULL, *pdl6 = NULL;
	struct proc_dir_entry *pdebug = NULL;

	pdh = proc_create("mcast_helper", 0,
			  NULL, &mcast_helper_seq_fops);
	if (!pdh) {
		goto out_mcast;
	}
	pdh6 = proc_create("mcast_helper6", 0,
			   NULL, &mcast_helper_seq_fops6);
	if (!pdh6) {
		goto out_mcast;
	}

	pdl = proc_create("mcast_lanserver", 0,
			  NULL, &mcast_lanserver_seq_fops);
	if (!pdl) {
		goto out_mcast;
	}
	pdl6 = proc_create("mcast_lanserver6", 0,
			   NULL, &mcast_lanserver_seq_fops6);
	if (!pdl6) {
		goto out_mcast;
	}

	pdebug = proc_create("mcast_debug", 0, NULL, &mcast_debug_fops);
	if (!pdebug) {
		goto out_mcast;
	}

	return 0;

out_mcast:
	if (pdh)
		remove_proc_entry("mcast_helper", NULL);
	if (pdh6)
		remove_proc_entry("mcast_helper6", NULL);
	if (pdl)
		remove_proc_entry("mcast_lanserver", NULL);
	if (pdl6)
		remove_proc_entry("mcast_lanserver6", NULL);

	return -ENOMEM;
}

/*=============================================================================
 *function name: mcast_helper_net_exit
 *description  : function to remove  mcast helper proc entry
 *===========================================================================*/

#ifdef CONFIG_SYSCTL
/**Functions to create a proc for ACL enable/disbale support **/
static int
mcast_helper_acl_sysctl_call_tables(struct ctl_table *ctl, int write,
				    void __user *buffer,
				    size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (write && *(int *)(ctl->data))
		*(int *)(ctl->data) = 1;
	return ret;
}

static struct ctl_table mcast_helper_acl_table[] = {
	{

		.procname	= "multicast-acl",
		.data		= &mch_acl_enabled,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= mcast_helper_acl_sysctl_call_tables,
	},
	{ }

};

/*=============================================================================
 *function name: mcast_helper_accl_sysctl_call_tables
 *description  : Functions to create a proc to enable/disbale
 *               Multicast accleration for WLAN
 *===========================================================================*/
static int
mcast_helper_accl_sysctl_call_tables(struct ctl_table *ctl, int write,
				     void __user *buffer, size_t *lenp,
				     loff_t *ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (write && *(int *)(ctl->data))
		*(int *)(ctl->data) = 1;
	return ret;
}

static struct ctl_table mcast_helper_accl_table[] = {
	{

		.procname	= "multicast-accleration",
		.data		= &mch_accl_enabled,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= mcast_helper_accl_sysctl_call_tables,
	},
	{ }

};
#endif /* CONFIG_SYSCTL */

int __init mcast_helper_proc_init(void)
{
	int ret;

#ifdef CONFIG_SYSCTL
	mcast_acl_sysctl_header = register_net_sysctl(&init_net,
			"net/", mcast_helper_acl_table);
	if (mcast_acl_sysctl_header == NULL) {
		printk(KERN_WARNING
		       "Failed to register mcast acl sysctl table.\n");
		return 0;
	}

	/*Proc to disable multicast accleration for WLAN */
	mcast_accl_sysctl_header = register_net_sysctl(&init_net,
			"net/", mcast_helper_accl_table);
	if (mcast_accl_sysctl_header == NULL) {
		printk(KERN_WARNING
		       "Failed to register mcast accl sysctl table.\n");
		return 0;
	}

#endif

	ret = mcast_helper_net_init();

	return ret;
}
#endif

/*=============================================================================
 *function name: mcast_helper_extract_grpidx
 *description  : function to retrieve the group index from the data buffer
 *===========================================================================*/
static unsigned int mcast_helper_extract_grpidx(char *data, int offset)
{
	unsigned int grpidx = 0;
	grpidx = (unsigned int) (data[offset] & 0xFF) ;
	return grpidx;
}

/*=============================================================================
 *function name: mcast_helper_extract_intrfidx
 *description  : function to retrieve the interface index from the data buffer
 *===========================================================================*/

static unsigned int mcast_helper_extract_intrfidx(char *data, int offset)
{
	unsigned int intrfidx = 0;
	intrfidx = (unsigned char) (data[offset] & 0xFF) ;
	return intrfidx;
}

/*=============================================================================
 *function name: mcast_helper_sig_check
 *description  : function to check the signature in the received probe packt
 *===========================================================================*/
static int mcast_helper_sig_check(unsigned char *data)
{
	if (memcmp(data, mch_signature, (sizeof(mch_signature)-1)) == 0)
		return 1;
	else
		return 0;
}

/*=============================================================================
 *function name: mcast_helper_sig_check_update_ip
 *description  : function to check signature and update the gitxmc table and
 *               invoke registered callbacks for IPv4 packet
 *===========================================================================*/
static int mcast_helper_sig_check_update_ip(struct sk_buff *skb)
{
	unsigned char *data;
	int grpidx = 0;
	int intrfid = 0;
	struct list_head *gimc_list = mcast_helper_list_p(IPV4) ;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	unsigned int flag = 0;


	if (ip_hdr(skb)->protocol == IPPROTO_UDP)
		return 0;

	data = (unsigned char *)udp_hdr(skb) + UDP_HDR_LEN;

	if (mcast_helper_sig_check(data) == 0)
		return 0;

	/*
	 * Signature matched now extract the grpindex
	 * and the call update gitxmc table
	 */
	grpidx = mcast_helper_extract_grpidx(data, sizeof(mch_signature) - 1);
	intrfid = mcast_helper_extract_intrfidx(data, sizeof(mch_signature));
	gimc_rec = mcast_helper_get_gimc_record(gimc_list, grpidx);
	if (gimc_rec) {
		if (skb->dev->ifindex == intrfid) {
			/*
			 * Update the GIMcTx table to add the
			 * new interface into the list
			 */
			gitxmc_rec = mcast_helper_search_gitxmc_record(
					&gimc_rec->mc_mem_list, skb->dev);
			if (gitxmc_rec != NULL) {
#ifdef CONFIG_MCAST_HELPER_ACL
				gitxmc_rec->aclBlocked = 0;
#endif
				flag = mcast_helper_update_mac_list(gitxmc_rec,
						gimc_rec, NULL, MCH_CB_F_ADD);
				mcast_helper_invoke_return_callback(
						gimc_rec->grpIdx,
						gitxmc_rec->memDev,
						&gimc_rec->mc_stream, flag,
						gitxmc_rec->macaddr_count);

			}

		}
#ifdef CONFIG_MCAST_HELPER_ACL
		/*
		 * Update the oifindex bitmap to be used
		 * for evaluating after timer expires
		 */
		gimc_rec->oifbitmap |= 1ULL << skb->dev->ifindex;
#endif
	}

	return 1;
}

/*=============================================================================
 *function name: mcast_helper_sig_check_update_ip6
 *description  : function to check signature and update the gitxmc table
 *               and invoke registered callbacks for the IPv6 packet
 *===========================================================================*/
static int mcast_helper_sig_check_update_ip6(struct sk_buff *skb)
{
	unsigned char *data;
	int grpidx = 0;
	int intrfid = 0;
	struct list_head *gimc_list = mcast_helper_list_p(IPV6) ;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	unsigned int flag = 0;

	data = (unsigned char *)udp_hdr(skb) + UDP_HDR_LEN;

	if (mcast_helper_sig_check(data) == 0)
		return 0;

	/*
	 * Signature matched now extract the grpindex
	 * and the call update gitxmc table
	 */
	grpidx = mcast_helper_extract_grpidx(data, sizeof(mch_signature) - 1);
	intrfid = mcast_helper_extract_intrfidx(data, sizeof(mch_signature));
	gimc_rec = mcast_helper_get_gimc_record(gimc_list, grpidx);
	if (gimc_rec) {
		if (skb->dev->ifindex == intrfid) {
			/*
			 * Update the GIMcTx table to add the
			 * new interface into the list
			 */
			gitxmc_rec = mcast_helper_search_gitxmc_record(
					&gimc_rec->mc_mem_list, skb->dev);
			if (gitxmc_rec) {

#ifdef CONFIG_MCAST_HELPER_ACL
				gitxmc_rec->aclBlocked = 0;
#endif
				flag = mcast_helper_update_mac_list(gitxmc_rec,
						gimc_rec, NULL, MCH_CB_F_UPD);
				mcast_helper_invoke_return_callback(
						gimc_rec->grpIdx,
						gitxmc_rec->memDev,
						&gimc_rec->mc_stream, flag,
						gitxmc_rec->macaddr_count);
			}
		}
#ifdef CONFIG_MCAST_HELPER_ACL
		/*
		 * Update the oifindex bitmap to be used
		 * for evaluating after timer expires
		 */
		gimc_rec->oifbitmap |= (1ULL << skb->dev->ifindex);
#endif
	}

	return 1;
}

/*=============================================================================
 *function name: mcast_helper_sig_check_update
 *description  : function to check signature and call corresponding callbacks
 *===========================================================================*/
int mcast_helper_sig_check_update(struct sk_buff *skb)
{
	struct list_head *gimc_list;
	const unsigned char *dest = eth_hdr(skb)->h_dest;

	if (!mch_acl_enabled)
		return 1;

	if (mch_iptype == IPV6)
		gimc_list = mcast_helper_list_p(IPV6) ;
	else
		gimc_list = mcast_helper_list_p(IPV4) ;

	if (mch_timerstarted && is_multicast_ether_addr(dest)) {
		if (eth_hdr(skb)->h_proto == ETH_P_IP) {
			if (ip_hdr(skb)->protocol == IPPROTO_UDP)
				mcast_helper_sig_check_update_ip(skb);
		} else if (eth_hdr(skb)->h_proto == ETH_P_IPV6) {
			if (ipv6_hdr(skb)->nexthdr == IPPROTO_UDP)
				mcast_helper_sig_check_update_ip6(skb);
		}
	}

	return 1;
}
EXPORT_SYMBOL(mcast_helper_sig_check_update);

#ifdef CONFIG_MCAST_HELPER_ACL
static void mcast_helper_timer_handler_probe(MCAST_GIMC_t *gimc_rec)
{
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	struct list_head *tliter = NULL;
	struct list_head *pliter = NULL;
	unsigned int i = 0, flag = 0, delflag = 1;
	unsigned long long int oifbitmap = 0;

	list_for_each_safe(tliter, pliter, &gimc_rec->mc_mem_list) {
		gitxmc_rec = list_entry(tliter, MCAST_MEMBER_t, list);
		oifbitmap = gimc_rec->oifbitmap;
		i = 0;
		delflag = 1;
		do {
			if (!(oifbitmap & 0x1)) {
				i++;
				continue;
			}
			if (gitxmc_rec->memDev->ifindex == i) {
				if (gitxmc_rec->aclBlocked == 1) {
					flag = mcast_helper_update_mac_list(
							gitxmc_rec, gimc_rec,
							NULL, MCH_CB_F_ADD);
					mcast_helper_invoke_return_callback(
							gimc_rec->grpIdx,
							gitxmc_rec->memDev,
							&gimc_rec->mc_stream,
							MCH_CB_F_ADD,
							gitxmc_rec->macaddr_count);
					gitxmc_rec->aclBlocked = 0;
				}
				delflag = 0;
				break;
			}
			i++;
		} while (oifbitmap >>= 1);

		if (delflag == 1) {
			/* Delete this interface from the
			 * gitxmc list and invoke registered
			 * call back for this if any
			 */
			flag = mcast_helper_update_mac_list(gitxmc_rec,
					gimc_rec, NULL, MCH_CB_F_DEL);
			mcast_helper_invoke_return_callback(
					gimc_rec->grpIdx, gitxmc_rec->memDev,
					&gimc_rec->mc_stream, flag,
					gitxmc_rec->macaddr_count);
			gitxmc_rec->aclBlocked = 1;
		}
	}
	gimc_rec->oifbitmap = 0;
	gimc_rec->probeFlag = 0;
}
#endif

/*=============================================================================
 *function name: mcast_helper_timer_handler
 *description  : function handling mcast herlper timer expiry
 *===========================================================================*/
static void mcast_helper_timer_handler(unsigned long data)
{
	MCAST_GIMC_t *gimc_rec = NULL;
	struct list_head *liter = NULL;
	struct list_head *gliter = NULL;
	struct list_head *gimc_list = NULL;

	mch_debug("helper timer invoked\n");
	if (mch_timermod) {
		mch_timermod = 0;
		return;
	}

	if (mch_iptype == IPV6)
		gimc_list = mcast_helper_list_p(IPV6) ;
	else
		gimc_list = mcast_helper_list_p(IPV4) ;

	list_for_each_safe(liter, gliter, gimc_list) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
#ifdef CONFIG_MCAST_HELPER_ACL
		if (gimc_rec->probeFlag == 1)
			mcast_helper_timer_handler_probe(gimc_rec);
#endif
	}

	mch_iptype = 0;
	mch_timerstarted = 0;
}

/*=============================================================================
 * function name : mcast_helper_clear_lanserver
 * description   : function clearing lanserver entries
 *===========================================================================*/
static void mcast_helper_clear_lanserver(ptype_t type)
{
	struct list_head *vliter = NULL;
	struct list_head *vtliter = NULL;
	MCAST_GIMC_t *lan_gimc_rec = NULL, *mcast_gimc_rec = NULL;
	struct list_head *gimc_list = mcast_helper_list_p(type);
	struct list_head *lan_gimc_list = mcast_helper_session_p(type);

	/* Loop the (IPv4/IPv6) lanserver group list */
	list_for_each_safe(vliter, vtliter, lan_gimc_list) {
		lan_gimc_rec = list_entry(vliter, MCAST_GIMC_t, list);
		mcast_gimc_rec = mcast_helper_get_gimc_record(gimc_list,
				lan_gimc_rec->grpIdx);
		if (mcast_gimc_rec != NULL)
			continue;
		/*
		 * If not found in mcast_helper, then indicate
		 * no any client join this GROUP, will delete
		 * from lanserver Group. As, not maintaining
		 * lanserver hook list when mcast_helper entry
		 * update, delete blindly. Will get added again
		 * if streaming continues.
		 */
		mcast_helper_delete_gimc_record(lan_gimc_rec);
	}
}

/*=============================================================================
 * function name : mcast_lanserver_timer_handler
 * description   : function handling lanserver entry timer expiry
 *===========================================================================*/
static void mcast_lanserver_timer_handler(unsigned long data)
{
	mch_debug("LAN server timer invoked\n");
	if (lanserver_timermod) {
		lanserver_timermod = 0;
		return;
	}

	/* Clear lanserver entries if no member present */
	mcast_helper_clear_lanserver(IPV4);
	mcast_helper_clear_lanserver(IPV6);

	lanserver_timerstarted = 0;
}

/*=============================================================================
 *function name: mcast_helper_start_helper_timer
 *description  : function starting/modifying helper timer
 *===========================================================================*/
static void mcast_helper_start_helper_timer(void)
{
	if (!mch_timerstarted) {
		mch_debug("starting helper timer\n");
		mcast_helper_exp_timer.expires = jiffies +
				(MCH_UPDATE_TIMER * HZ);
		add_timer(&mcast_helper_exp_timer);
		mch_timerstarted  = 1;
	} else {
		mch_debug("modifying helper timer\n");
		mch_timermod = 1;
		mod_timer(&mcast_helper_exp_timer, jiffies +
			  MCH_UPDATE_TIMER * HZ);
		mch_timermod = 0;
	}
}

/*=============================================================================
 *function name: mcast_helper_init_timer
 *description  : function handling helper timer Initialization
 *===========================================================================*/
static int mcast_helper_init_timer(int delay)
{

	init_timer(&mcast_helper_exp_timer);
	mcast_helper_exp_timer.expires = jiffies + delay *HZ;
	mcast_helper_exp_timer.data = 0;
	mcast_helper_exp_timer.function = mcast_helper_timer_handler;

	return 0;
}

/*=============================================================================
 *function name: mcast_helper_start_lanserver_timer
 *description  : function starting/modifying lanserver timer
 *===========================================================================*/
static void mcast_helper_start_lanserver_timer(void)
{
	if (!lanserver_timerstarted) {
		mch_debug("starting LAN server timer\n");
		mcast_lanserver_timer.expires = jiffies +
				(LANSERVER_UPDATE_TIMER * HZ);
		add_timer(&mcast_lanserver_timer);
		lanserver_timerstarted = 1;
	} else {
		mch_debug("modifying LAN server timer\n");
		lanserver_timermod = 1;
		mod_timer(&mcast_lanserver_timer,jiffies +
			  LANSERVER_UPDATE_TIMER * HZ);
		lanserver_timermod = 0;
	}
}

/*=============================================================================
 *function name: mcast_lanserver_init_timer
 *description  : function handling lanserver timer Initialization
 *===========================================================================*/
static int mcast_lanserver_init_timer(int delay)
{
	init_timer(&mcast_lanserver_timer);
	mcast_lanserver_timer.expires = jiffies + delay * HZ;
	mcast_lanserver_timer.data = 0;
	mcast_lanserver_timer.function = mcast_lanserver_timer_handler;

	return 0;
}

static struct file_operations mcast_helper_fops = {
	.owner = THIS_MODULE,
	.open = mcast_helper_open,
	.release = mcast_helper_close,
	.unlocked_ioctl = mcast_helper_ioctl
};

/*=============================================================================
 * function name: mcast_helper_delete_all_mac
 * description  : Function deletes all clients and inform callback
 *===========================================================================*/
static void
mcast_helper_delete_all_mac(MCAST_GIMC_t *gimc_rec, MCAST_MEMBER_t *mem_rec)
{
	MCAST_MAC_t *mac_rec = NULL;
	struct list_head *liter = NULL, *tliter = NULL;

	if ((mem_rec == NULL) || (gimc_rec == NULL))
		return;

	if (mem_rec->macaddr_count <= 1) {
		list_for_each_safe(liter, tliter, &mem_rec->macaddr_list) {
			mac_rec = list_entry(liter, MCAST_MAC_t, list);
		}
		if (mac_rec) {
			/* MAC count 1 and matching, inform and remove */
			mcast_helper_delete_gitxmc_record(mem_rec,
					gimc_rec, mem_rec->memDev,
					mac_rec->macaddr, MCH_CB_F_DEL);
			list_del(&mac_rec->list);
			kfree(mac_rec);
		}
	} else {
		/* More than one MAC, delete at once */
		mcast_helper_delete_gitxmc_record(mem_rec, gimc_rec,
				mem_rec->memDev, NULL, MCH_CB_F_DEL);
		list_for_each_safe(liter, tliter, &mem_rec->macaddr_list) {
			mac_rec = list_entry(liter, MCAST_MAC_t, list);
			list_del(&mac_rec->list);
			kfree(mac_rec);
		}
	}
}

/*=============================================================================
 * function name: mcast_helper_delete_gitxmc_and_mac
 * description  : Function searches and deletes all clients and inform callback
 *                Also delete the member and if no member delete GIMC
 *===========================================================================*/
static void
mcast_helper_delete_gitxmc_and_mac(ptype_t type, MCH_NETIF *netif)
{
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *mem_rec = NULL;
	struct list_head *vliter = NULL, *vtliter = NULL;
	struct list_head *gimc_list = mcast_helper_list_p(type);

	/* Loop the GIMC list (IPv4/IPv6) */
	list_for_each_safe(vliter, vtliter, gimc_list) {
		gimc_rec = list_entry(vliter, MCAST_GIMC_t, list);

		/* Search group membership for the device */
		mem_rec = mcast_helper_search_gitxmc_record(
				&gimc_rec->mc_mem_list, netif);
		if (mem_rec == NULL)
			continue;

		/* Delete all of its associated clients */
		mcast_helper_delete_all_mac(gimc_rec, mem_rec);

		list_del(&mem_rec->list);
		kfree(mem_rec);

		if (list_empty(&gimc_rec->mc_mem_list)) {
			/* Delete helper GIMC record */
			mcast_helper_delete_gimc_record(gimc_rec);
		}
		break;
	}
}

/* Lanserver entry deletion for netdev down/unregister events */
static int mcast_netdevice_event(struct notifier_block *nb,
				 unsigned long action, void *ptr)
{
	MCH_NETIF *netif = NULL;

	netif = netdev_notifier_info_to_dev((struct netdev_notifier_info *)ptr);

	switch (action) {
	case NETDEV_UNREGISTER:
	case NETDEV_DOWN:
		/* Search and delete member and all associated clients */
		mcast_helper_delete_gitxmc_and_mac(IPV4, netif);
		mcast_helper_delete_gitxmc_and_mac(IPV6, netif);

		/* Clear lanserver entries if no member present */
		mcast_helper_clear_lanserver(IPV4);
		mcast_helper_clear_lanserver(IPV6);
		break;
	}

	return 0;
}

struct notifier_block mcast_netdevice_notifier = {
	.notifier_call = mcast_netdevice_event
};

/*=============================================================================
 *function name: mcast_helper_init_module
 *description  : Multucast helper module initilization
 *===========================================================================*/
static int __init mcast_helper_init_module(void)
{
	int ret_val;
	int index = 0;

	/*
	 * Alloc the chrdev region for mcast helper
	 */
	ret_val = alloc_chrdev_region(&mch_major , 0 ,1 , DEVICE_NAME);

	/*
	 *Negative values signify an error
	 */
	if (ret_val < 0) {
		printk(KERN_ALERT "%s failed with %d\n",
		       "Sorry, alloc_chrdev_region failed for the mcast device",
		       ret_val);
		return ret_val;
	}

	printk(KERN_INFO "%s The major device number is %d.\n",
	       "Registeration is a success", MAJOR(mch_major));

	/* Create device class (before allocation of the array of devices) */
	mcast_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(mcast_class)) {
		ret_val = PTR_ERR(mcast_class);
		goto fail;
	}
	if (device_create(mcast_class, NULL, mch_major, NULL, "mcast") == NULL)
		goto fail;
	device_created = 1;
	cdev_init(&mcast_cdev, &mcast_helper_fops);
	if (cdev_add(&mcast_cdev, mch_major, 1) == -1)
		goto fail;

	for (index = 0 ; index < GINDX_LOOP_COUNT ;index++) {
		g_mcast_grpindex[index] = 0 ;
	}

	register_netdevice_notifier(&mcast_netdevice_notifier);
#ifdef CONFIG_PROC_FS
	mcast_helper_proc_init();
#endif
	memset(ftuple_info, 0, sizeof(FTUPLE_INFO_t)*FTUPLE_ARR_SIZE);
	memset(ftuple_info6, 0, sizeof(FTUPLE_INFO_t)*FTUPLE_ARR_SIZE);
	five_tuple_info_ptr = (void *)mcast_helper_five_tuple_info;
#if IS_ENABLED(CONFIG_IPV6_MROUTE)
	five_tuple_info6_ptr = (void *)mcast_helper_five_tuple_info6;
#endif
	five_tuple_br_info_ptr = (void *)mcast_helper_five_tuple_br_info;
	five_tuple_br_info_hook = (void *)mcast_helper_five_tuple_br_hook;
	mcast_helper_sig_check_update_ptr = mcast_helper_sig_check_update;
	mcast_helper_get_skb_gid_ptr = mcast_helper_get_skb_gid_internal;
	mcast_helper_init_timer(MCH_UPDATE_TIMER);
	mcast_lanserver_init_timer(LANSERVER_UPDATE_TIMER);

	return 0;

fail:
	if (device_created) {
		device_destroy(mcast_class, mch_major);
		cdev_del(&mcast_cdev);
	}
	if (mcast_class)
		class_destroy(mcast_class);
	if (mch_major != -1)
		unregister_chrdev_region(mch_major, 1);

	return -1;
}

 /*=============================================================================
 * function name : mcast_helper_exit_module
 * description   : Mcast helper module exit handler
 *===========================================================================*/

static void __exit mcast_helper_exit_module(void)
{
	/* Cancel all timers */
	del_timer(&mcast_helper_exp_timer);
	del_timer(&mcast_lanserver_timer);

	mch_acl_enabled = 0;
	five_tuple_info_ptr = NULL;
#if IS_ENABLED(CONFIG_IPV6_MROUTE)
	five_tuple_info6_ptr = NULL;
#endif
	five_tuple_br_info_ptr = NULL;
	five_tuple_br_info_hook = NULL;
	mcast_helper_sig_check_update_ptr = NULL;
	mcast_helper_get_skb_gid_ptr = NULL;

	if (skb_buff)
		kfree_skb(skb_buff);
	if (skb_buff6)
		kfree_skb(skb_buff6);

	unregister_netdevice_notifier(&mcast_netdevice_notifier);
#ifdef CONFIG_SYSCTL
	unregister_net_sysctl_table(mcast_acl_sysctl_header);
	unregister_net_sysctl_table(mcast_accl_sysctl_header);
#endif
	remove_proc_entry("mcast_lanserver", NULL);
	remove_proc_entry("mcast_lanserver6", NULL);
	remove_proc_entry("mcast_helper", NULL);
	remove_proc_entry("mcast_helper6", NULL);
	remove_proc_entry("mcast_debug", NULL);

	if (device_created) {
		device_destroy(mcast_class, mch_major);
		cdev_del(&mcast_cdev);
	}
	if (mcast_class)
		class_destroy(mcast_class);
	if (mch_major != -1)
		unregister_chrdev_region(mch_major, 1);
}

module_init(mcast_helper_init_module);
module_exit(mcast_helper_exit_module);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Multicast helper");
MODULE_AUTHOR("Srikanth");

