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

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
#include <net/switch_api/lantiq_gsw_api.h>
#define SWITCH_DEV "/dev/switch_api/0"
#endif

/** Defines **/
#define GINDX_LOOP_COUNT 1
#define GINDX_MAX_SIZE 64

#define IP_HDR_LEN 20
#define UDP_HDR_LEN 8
#define TOT_HDR_LEN 28
#define IP6_HDR_LEN 40
#define TOT6_HDR_LEN 48
#define MCH_UPDATE_TIMER 10
#define LANSERVER_UPDATE_TIMER 10
#define MCH_ETH_P_IPV6      0x86DD
#define MCH_ETH_P_IP        0x0800
#define MCH_MIN_BUF_SIZE  64

#define MCH_MAX_PROC_SIZE 24

////////////////////////////////
/*! \def MCH_ETH_HLEN
  \brief Macro that specifies the maximum length of an Ethernet MAC header.
 */
#define MCH_ETH_HLEN                            14  /*Total octets in header.   */

//#define ETH_ALEN 6


/*!
  \brief  Packet buffer structure. For Linux OS, this is the sk_buff structure.
 */
typedef struct sk_buff        MCH_BUF;

/*!
  \brief Macro that specifies MCH network interface data structure
 */
typedef struct net_device     MCH_NETIF;



/**mcast helper member  list */
LIST_HEAD(mch_mem_list_g);
LIST_HEAD(mch_mem_list6_g);

/** mcast helper member session list */
LIST_HEAD(mch_mem_list_session_g);
LIST_HEAD(mch_mem_list6_session_g);


/** mcast helper global variables */
static FTUPLE_INFO_t ftuple_info[FTUPLE_ARR_SIZE];
static FTUPLE_INFO_t ftuple_info6[FTUPLE_ARR_SIZE];
static unsigned long long int  g_mcast_grpindex[GINDX_LOOP_COUNT];

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

extern int mch_br_capture_pkt;
extern int mcast_helper_invoke_callback(unsigned int grpidx, struct net_device *netdev, void *mc_stream, unsigned int flag, unsigned int count);
extern int (*mcast_helper_sig_check_update_ptr)(struct sk_buff *skb);

/**mcast helper function prototype */
static int mcast_helper_invoke_return_callback(unsigned int grpidx, struct net_device *netdev,
		MCAST_STREAM_t *mc_stream, unsigned int flag, unsigned int count);
static MCAST_GIMC_t *mcast_helper_search_gimc_record(IP_Addr_t *gaddr,
		IP_Addr_t *saddr, struct list_head *head);
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
 *Function Name: mcast_helper_dev_get_by_name
 *Description	: Function to get the device by name
 *===========================================================================*/

struct net_device *mcast_helper_dev_get_by_name(struct net *net, const char *name)
{
	struct net_device *netif = NULL;
	netif = dev_get_by_name(net, name);
	if (netif != NULL) {
		dev_put(netif);
		return netif;
	} else
		return NULL;
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
		return false;

	if (0) {
	} else if (addr1->ipType == IPV4 && addr2->ipType == IPV4) {
		return addr1->ipA.ipAddr.s_addr == addr2->ipA.ipAddr.s_addr;
	} else if (addr1->ipType == IPV6 && addr2->ipType == IPV6) {
		return IN6_ARE_ADDR_EQUAL(&addr1->ipA.ip6Addr, &addr2->ipA.ip6Addr);
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

	if (0) {
	} else if (type == IPV4) {
		addr->ipType = IPV4;
		if (addrp) {
			addr->ipA.ipAddr.s_addr = *((unsigned int *)addrp);
		} else {
			addr->ipA.ipAddr.s_addr = 0;
		}
	} else if (type == IPV6) {
		struct in6_addr *in6 = (struct in6_addr *)addrp;
		addr->ipType = IPV6;
		if (in6) {
			memcpy(&(addr->ipA.ip6Addr), in6, sizeof(struct in6_addr));
		} else
			memset(&(addr->ipA.ip6Addr), 0, sizeof(struct in6_addr));
	}
}

/*=============================================================================
 *Function Name: mcast_helper_is_addr_unspecified
 *Description	: Wrapper function to ip address is specified
 *===========================================================================*/


int mcast_helper_is_addr_unspecified(IP_Addr_t *addr)
{
	if (addr->ipType == IPV4) {
		return addr->ipA.ipAddr.s_addr == 0;
	} else if (addr->ipType == IPV6) {
		return IN6_IS_ADDR_UNSPECIFIED(&(addr->ipA.ip6Addr.s6_addr16[0]));
	}
	return 0;
}


/*=============================================================================
 *Function Name: mcast_helper_list_p
 *Description	: Wrapper function to get the membership list pointer
 *===========================================================================*/


struct list_head *mcast_helper_list_p(ptype_t type)
{
	if (0) {
	} else if (type == IPV4) {
		return &mch_mem_list_g;
	} else if (type == IPV6) {
		return &mch_mem_list6_g;
	}

	return NULL;
}

struct list_head *mcast_helper_session_p(ptype_t type)
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
 *Description	: Wrapper function to check netif is ppp
 *===========================================================================*/


uint32_t mch_check_is_ppp_netif (MCH_NETIF *netif)
{
	return (netif->type == ARPHRD_PPP && (netif->flags & IFF_POINTOPOINT));
}

/*=============================================================================
 *Function Name: mch_dev_get_by_name
 *Description	: Wrapper function to get the net device from interface name
 *===========================================================================*/

struct net_device *mch_dev_get_by_name(const char *ifname)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 32)
	return dev_get_by_name(ifname);
#else
	return dev_get_by_name(&init_net, ifname);
#endif

}


/*=============================================================================
 *Function Name: mcast_helper_get_pkt_rx_src_mac_addr
 *Description	: Wrapper function to extract the source mac address from skb
 *===========================================================================*/

void mcast_helper_get_pkt_rx_src_mac_addr(MCH_BUF *mch_buf, uint8_t mac[ETH_ALEN])
{
	if ((uint32_t)skb_mac_header(mch_buf) >= KSEG0)
		memcpy(mac, skb_mac_header(mch_buf) + ETH_ALEN, ETH_ALEN);
}

/*=============================================================================
 *Function Name: mch_get_netif
 *Description	: Wrapper function to get netif from interface name
 *===========================================================================*/

MCH_NETIF *mch_get_netif (char  *ifname)
{

	MCH_NETIF *netif;
	netif = mch_dev_get_by_name(ifname);
	if (netif) {
		dev_put(netif);
		return netif;
	} else
		return NULL;
}

/*=============================================================================
 *Function Name: mcast_helper_get_rx_mac_addr
 *Description	: function to read the source mac address from skb based on interface type
 *===========================================================================*/


void  mcast_helper_get_rx_mac_addr(MCH_BUF *mch_buf, char *ifname, char *s_mac)
{
	MCH_NETIF *netif;
	int32_t hdr_offset = 14; // pppoe header length
	netif = mch_get_netif (ifname);
	if (netif != NULL) {
		hdr_offset = MCH_ETH_HLEN;
		if (mch_check_is_ppp_netif (netif)) {
			hdr_offset += PPPOE_SES_HLEN ; // PPPoE
			skb_set_mac_header(mch_buf, -hdr_offset);
			mcast_helper_get_pkt_rx_src_mac_addr(mch_buf, s_mac);
			skb_reset_mac_header(mch_buf);

		} else {

			mcast_helper_get_pkt_rx_src_mac_addr(mch_buf, s_mac);
		}

	}
}

/*=============================================================================
 *Function Name: mcast_helper_five_tuple_info
 *Description	: Function  to retrive 5-tuple info for IPV4 packet
 *===========================================================================*/


static void mcast_helper_five_tuple_info(struct sk_buff *skb, char iface_name[IFSIZE])
{
	int index = 0;
	struct iphdr *iph = ip_hdr(skb);
	struct udphdr *udph = (struct udphdr *)((u8 *)iph +(iph->ihl << 2));

	const unsigned char *dest = eth_hdr(skb)->h_dest;
	if (is_multicast_ether_addr(dest)) {
		if (mch_captured_skb && strncmp(skb->dev->name, "br", 2) && mch_acl_enabled) {
			skb_buff = skb_copy(skb, GFP_ATOMIC);
			mch_captured_skb = 0;
		}


		for (index = 0; index < FTUPLE_ARR_SIZE; index++) {
			if (ftuple_info[index].uflag == 0) {

				strncpy(ftuple_info[index].rxIntrfName, iface_name, strlen(iface_name));
				mcast_helper_init_ipaddr(&ftuple_info[index].groupIP, IPV4, &iph->daddr);
				mcast_helper_init_ipaddr(&ftuple_info[index].srcIP, IPV4, &iph->saddr);
				ftuple_info[index].proto = iph->protocol;
				ftuple_info[index].sPort = udph->source;
				ftuple_info[index].dPort = udph->dest;
				mcast_helper_get_rx_mac_addr(skb, iface_name, ftuple_info[index].src_mac);

				ftuple_info[index].uflag = 1;
				if ((index+1) < FTUPLE_ARR_SIZE)
					ftuple_info[index+1].uflag = 0;
				else
					ftuple_info[0].uflag = 0;

				break;
			} else if (mcast_helper_is_same_ipaddr(&ftuple_info[index].groupIP, (IP_Addr_t *)&iph->daddr)
					&& (mcast_helper_is_same_ipaddr(&ftuple_info[index].srcIP, (IP_Addr_t *)&iph->saddr)
						|| mcast_helper_is_addr_unspecified(&ftuple_info[index].srcIP)))
				break;

		}
	}
	return;
}

/*=============================================================================
 *Function Name: mcast_helper_five_tuple_info6
 *Description	: Function  to retrive 5-tuple info for IPV6 packet
 *===========================================================================*/

static void mcast_helper_five_tuple_info6(struct sk_buff *skb, char iface_name[IFSIZE])
{
	int index;
	struct ipv6hdr *ip6h = ipv6_hdr(skb);
	struct udphdr *udph = udp_hdr(skb);
	const unsigned char *dest = eth_hdr(skb)->h_dest;

	if (is_multicast_ether_addr(dest)) {
		if (mch_captured_skb6 && strncmp(skb->dev->name, "br", 2) && mch_acl_enabled) {
			skb_buff6 = skb_copy(skb, GFP_ATOMIC);
			mch_captured_skb6 = 0;
		}
		for (index = 0; index < FTUPLE_ARR_SIZE; index++) {
			if (ftuple_info6[index].uflag == 0) {
				strncpy(ftuple_info6[index].rxIntrfName, iface_name, strlen(iface_name));
				mcast_helper_init_ipaddr(&ftuple_info6[index].groupIP, IPV6, &ip6h->daddr);
				mcast_helper_init_ipaddr(&ftuple_info6[index].srcIP, IPV6, &ip6h->saddr);
				ftuple_info6[index].proto = ip6h->nexthdr;
				ftuple_info6[index].sPort = udph->source;
				ftuple_info6[index].dPort = udph->dest;
				mcast_helper_get_rx_mac_addr(skb, iface_name, ftuple_info6[index].src_mac);

				ftuple_info6[index].uflag = 1;
				if ((index+1) < FTUPLE_ARR_SIZE)
					ftuple_info6[index+1].uflag = 0;
				else
					ftuple_info6[0].uflag = 0;
				break;
			} else {
				if (mcast_helper_is_same_ipaddr(&ftuple_info6[index].groupIP, (IP_Addr_t *)&ip6h->daddr)
						&& (mcast_helper_is_same_ipaddr(&ftuple_info6[index].srcIP, (IP_Addr_t *)&ip6h->saddr)
							|| mcast_helper_is_addr_unspecified(&ftuple_info6[index].srcIP)))
					break;
			}
		}
	}
	return;
}

/*=============================================================================
 *Function Name: mcast_helper_five_tuple_br_info
 *Description	: Function  to retrive 5-tuple info for bridge IPV4 and IPV6 packet
 *===========================================================================*/

static void mcast_helper_five_tuple_br_info(struct sk_buff *skb)
{

	MCAST_GIMC_t *gimc_rec;
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	struct list_head *gimc_list;
	IP_Addr_t saddr;
	IP_Addr_t daddr;
	const unsigned char *dest = eth_hdr(skb)->h_dest;


	if ((skb->protocol == htons(ETH_P_IP)) && (is_multicast_ether_addr(dest))) {
		struct iphdr *iph = ip_hdr(skb);
		struct udphdr *udph = (struct udphdr *)((u8 *)iph +(iph->ihl << 2));
		mcast_helper_init_ipaddr(&saddr, IPV4, &iph->saddr);
		mcast_helper_init_ipaddr(&daddr, IPV4, &iph->daddr);
		gimc_list = mcast_helper_list_p(IPV4) ;


		if (mch_captured_skb && mch_acl_enabled) {
			skb_buff = skb_copy(skb, GFP_ATOMIC);
			mch_captured_skb = 0;
		}


		gimc_rec = mcast_helper_search_gimc_record(&daddr, &saddr, gimc_list);
		if (gimc_rec != NULL && gimc_rec->br_callback_flag == 0) {
			mch_br_capture_pkt = 0;
			if (!list_empty(&gimc_rec->mc_mem_list)) {
				list_for_each_safe(liter, tliter, &gimc_rec->mc_mem_list) {
					gitxmc_rec = list_entry(liter, MCAST_MEMBER_t, list);
					if (gitxmc_rec != NULL) {
						gimc_rec->br_callback_flag = 1;
						mcast_helper_init_ipaddr(&gimc_rec->mc_stream.sIP, IPV4, &iph->saddr);
						gimc_rec->mc_stream.proto = iph->protocol;
						gimc_rec->mc_stream.sPort = udph->source;
						gimc_rec->mc_stream.dPort = udph->dest;
						mcast_helper_get_pkt_rx_src_mac_addr(skb, gimc_rec->mc_stream.src_mac);
						mcast_helper_invoke_return_callback(gimc_rec->grpIdx, gitxmc_rec->memDev, (MCAST_STREAM_t *)&(gimc_rec->mc_stream), MC_F_ADD, gitxmc_rec->macaddr_count);
					}
				}
			}

		} else {
			mch_br_capture_pkt = 0;
		}
	} else if (skb->protocol == htons(ETH_P_IPV6)) {
		struct ipv6hdr *iph6 = ipv6_hdr(skb);
		struct udphdr *udph6 = udp_hdr(skb);
		gimc_list = mcast_helper_list_p(IPV6) ;

		mcast_helper_init_ipaddr(&saddr, IPV6, &iph6->saddr);
		mcast_helper_init_ipaddr(&daddr, IPV6, &iph6->daddr);

		if (mch_captured_skb6 && mch_acl_enabled) {
			skb_buff6 = skb_copy(skb, GFP_ATOMIC);
			mch_captured_skb6 = 0;
		}


		gimc_rec = mcast_helper_search_gimc_record(&daddr, &saddr, gimc_list);
		if (gimc_rec != NULL && gimc_rec->br_callback_flag == 0) {
			mch_br_capture_pkt = 0;
			if (!list_empty(&gimc_rec->mc_mem_list)) {
				list_for_each_safe(liter, tliter, &gimc_rec->mc_mem_list) {
					gitxmc_rec = list_entry(liter, MCAST_MEMBER_t, list);
					if (gitxmc_rec != NULL) {
						gimc_rec->br_callback_flag = 1;
						mcast_helper_init_ipaddr(&gimc_rec->mc_stream.sIP, IPV6, &iph6->saddr);
						gimc_rec->mc_stream.proto = iph6->nexthdr;
						gimc_rec->mc_stream.sPort = udph6->source;
						gimc_rec->mc_stream.dPort = udph6->dest;
						mcast_helper_get_pkt_rx_src_mac_addr(skb, gimc_rec->mc_stream.src_mac);
						mcast_helper_invoke_return_callback(gimc_rec->grpIdx, gitxmc_rec->memDev, (MCAST_STREAM_t *)&(gimc_rec->mc_stream), MC_F_ADD, gitxmc_rec->macaddr_count);
					}
				}
			}

		} else {
			mch_br_capture_pkt = 0;
		}
	} else {
		mch_br_capture_pkt = 0;
	}

	return;
}

/*=============================================================================
 * Function Name : mcast_helper_five_tuple_br_hook
 * Description	 : Function  to retrive 5-tuple info for bridge IPV4 and IPV6 packet
 *===========================================================================*/

static void mcast_helper_five_tuple_br_hook(struct sk_buff *skb)
{
	MCAST_GIMC_t *gimc_rec;
	MCAST_GIMC_t *gimc_rec_h;
	struct list_head *gimc_list;
	struct list_head *gimc_list_h;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	IP_Addr_t saddr;
	IP_Addr_t daddr;
	const unsigned char *dest = eth_hdr(skb)->h_dest;

	if (skb->protocol == htons(ETH_P_IP)) {
		uint8_t src_mac[ETH_ALEN] = { 0 };
		struct iphdr *iph = ip_hdr(skb);
		struct udphdr *udph = (struct udphdr *)((u8 *)iph +(iph->ihl << 2));
		mcast_helper_init_ipaddr(&saddr, IPV4, &iph->saddr);
		mcast_helper_init_ipaddr(&daddr, IPV4, &iph->daddr);
		mcast_helper_get_pkt_rx_src_mac_addr(skb, src_mac);
		gimc_list = mcast_helper_session_p(IPV4) ;

		gimc_rec = mcast_helper_search_gimc_record(&daddr, &saddr, gimc_list);
		if (gimc_rec == NULL) {
			gimc_rec = mcast_helper_add_gimc_record(skb->dev,&daddr,&saddr,iph->protocol,udph->source,udph->dest, src_mac,gimc_list);
			if (gimc_rec == NULL) {
			  return;
			}

			gimc_list_h = mcast_helper_list_p(IPV4);
			gimc_rec_h = mcast_helper_search_gimc_record(&daddr, &saddr, gimc_list_h);
			if (gimc_rec_h != NULL && gimc_rec_h->br_callback_flag == 0) {
				if (!list_empty(&gimc_rec_h->mc_mem_list)) {
					list_for_each_safe(liter,tliter, &gimc_rec_h->mc_mem_list) {
						gitxmc_rec = list_entry(liter, MCAST_MEMBER_t, list);
						if (gitxmc_rec != NULL) {
							gimc_rec_h->br_callback_flag = 1;
							mcast_helper_init_ipaddr(&gimc_rec_h->mc_stream.sIP, IPV4, &iph->saddr);
							gimc_rec_h->mc_stream.proto = iph->protocol;
							gimc_rec_h->mc_stream.sPort = udph->source;
							gimc_rec_h->mc_stream.dPort = udph->dest;
							gimc_rec_h->mc_stream.rxDev = skb->dev;
							mcast_helper_get_pkt_rx_src_mac_addr(skb,gimc_rec_h->mc_stream.src_mac);
							mcast_helper_invoke_return_callback(gimc_rec_h->grpIdx,gitxmc_rec->memDev,(MCAST_STREAM_t *)&(gimc_rec_h->mc_stream),MC_F_ADD, gitxmc_rec->macaddr_count);
						}
					}
				}
			}
		} else { //update the information
			gimc_rec->mc_stream.rxDev = skb->dev;
			gimc_rec->mc_stream.proto = iph->protocol;
			gimc_rec->mc_stream.sPort = udph->source;
			gimc_rec->mc_stream.dPort = udph->dest;
			memcpy(gimc_rec->mc_stream.src_mac, src_mac, ETH_ALEN);
		}

	}
	else if (skb->protocol == htons(ETH_P_IPV6)) {
		uint8_t src_mac[ETH_ALEN] = { 0 };
		struct ipv6hdr *iph6 = ipv6_hdr(skb);
		struct udphdr *udph6 = udp_hdr(skb);
		gimc_list = mcast_helper_session_p(IPV6);

		mcast_helper_init_ipaddr(&saddr, IPV6, &iph6->saddr);
		mcast_helper_init_ipaddr(&daddr, IPV6, &iph6->daddr);
		mcast_helper_get_pkt_rx_src_mac_addr(skb, src_mac);

		gimc_rec = mcast_helper_search_gimc_record(&daddr, &saddr, gimc_list);
		if (gimc_rec == NULL) {
			gimc_rec = mcast_helper_add_gimc_record(skb->dev,&daddr,&saddr,iph6->nexthdr,udph6->source,udph6->dest, src_mac,gimc_list);
			if (gimc_rec == NULL) {
			  return;
			}

			gimc_list_h = mcast_helper_list_p(IPV6);
			gimc_rec_h = mcast_helper_search_gimc_record(&daddr, &saddr, gimc_list_h);
			if (gimc_rec_h != NULL && gimc_rec_h->br_callback_flag == 0) {
				if (!list_empty(&gimc_rec_h->mc_mem_list)) {
					list_for_each_safe(liter,tliter, &gimc_rec_h->mc_mem_list) {
						gitxmc_rec = list_entry(liter, MCAST_MEMBER_t, list);
						if (gitxmc_rec != NULL) {
							gimc_rec_h->br_callback_flag = 1;
							mcast_helper_init_ipaddr(&gimc_rec_h->mc_stream.sIP, IPV6, &iph6->saddr);
							gimc_rec_h->mc_stream.proto = iph6->nexthdr;
							gimc_rec_h->mc_stream.sPort = udph6->source;
							gimc_rec_h->mc_stream.dPort = udph6->dest;
							gimc_rec_h->mc_stream.rxDev = skb->dev;
							mcast_helper_get_pkt_rx_src_mac_addr(skb,gimc_rec_h->mc_stream.src_mac);
							mcast_helper_invoke_return_callback(gimc_rec_h->grpIdx,gitxmc_rec->memDev,(MCAST_STREAM_t *)&(gimc_rec_h->mc_stream),MC_F_ADD, gitxmc_rec->macaddr_count);
						}
					}
				}
			}
		} else { //update the information
			gimc_rec->mc_stream.rxDev = skb->dev;
			gimc_rec->mc_stream.proto = iph6->nexthdr;
			gimc_rec->mc_stream.sPort = udph6->source;
			gimc_rec->mc_stream.dPort = udph6->dest;

			memcpy(gimc_rec->mc_stream.src_mac, src_mac, ETH_ALEN);
		}
	}

	return;
}
/*=============================================================================
 * Function Name : mcast_helper_get_grpidx
 * Description	 : Function  to allocate group index
 *===========================================================================*/


static int mcast_helper_get_grpidx(void)
{
	unsigned int index = 0, index1 = 0;
	int grpIdx = -1;
	for (index = 0; index < GINDX_LOOP_COUNT; index++) {
		for (index1 = 0 ; index1 < GINDX_MAX_SIZE ; index1++) {
			if ((g_mcast_grpindex[index] & (0x01 << index1)) == 0) {
				g_mcast_grpindex[index] |= (0x01 << index1) ;
				grpIdx = (index1+1)+(GINDX_MAX_SIZE*index);

				return grpIdx;
			}
		}
	}


	return grpIdx;
}

/*=============================================================================
 *Function Name: mcast_helper_release_grpidx
 *Description	: Function  to release allocated  group index
 *===========================================================================*/


static void  mcast_helper_release_grpidx(int grpidx)
{
	unsigned int index = 0, index1 = 0;
	if (grpidx > GINDX_MAX_SIZE)
		index = 1;
	for (index1 = 0 ; index1 < GINDX_MAX_SIZE ; index1++) {

		if (grpidx == index1) {
			g_mcast_grpindex[index] &= ~(0x01 << (index1-1)) ;
			break;
		}

	}
}

/*=============================================================================
 *Function Name: mcast_helper_search_gimc_record
 *Description	: Function  to search gaddr and saddr in the gimc record list
 *===========================================================================*/


static MCAST_GIMC_t *mcast_helper_search_gimc_record(IP_Addr_t *gaddr,
		IP_Addr_t *saddr, struct list_head *head)
{
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	MCAST_GIMC_t *gimc_rec = NULL;

	list_for_each_safe(liter, tliter, head) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		if (gimc_rec != NULL) {
			if (mcast_helper_is_same_ipaddr((IP_Addr_t *)&gimc_rec->mc_stream.dIP, gaddr)) {
				if (!mcast_helper_is_addr_unspecified(saddr)) {
					if (!mcast_helper_is_addr_unspecified(&gimc_rec->mc_stream.sIP)) {
						if (mcast_helper_is_same_ipaddr((IP_Addr_t *)&gimc_rec->mc_stream.sIP, saddr)) {
							return gimc_rec;
						} else {
							continue;
						}
					}
				}
				return gimc_rec;
			}
		}
	}

	return NULL;
}

#if 0 /*Debug code */
static void mcast_helper_show_gimc_entry(void)
{
	struct list_head *liter = NULL;
	struct list_head *gliter = NULL;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	struct list_head *gimc_list = mcast_helper_list_p(IPV4) ;
	printk(KERN_INFO
			"%3s %10s "
			"%10s %10s %6s %6s %6s %8s\n", "GIdx",
			"RxIntrf", "GA",
			"SA", "proto", "sPort", "dPort", "memIntrf");
	list_for_each(liter, gimc_list) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		if (gimc_rec) {
			printk(KERN_INFO
					"%3d %10s %10x "
					"%10x %6d %6d %6d",
					gimc_rec->grpIdx, gimc_rec->mc_stream.rxDev->name, gimc_rec->mc_stream.sIP.ipA.ipAddr.s_addr, gimc_rec->mc_stream.dIP.ipA.ipAddr.s_addr,
					gimc_rec->mc_stream.proto,
					gimc_rec->mc_stream.sPort,
					gimc_rec->mc_stream.dPort);
		}
		list_for_each(gliter, &gimc_rec->mc_mem_list) {
			gitxmc_rec = list_entry(gliter, MCAST_MEMBER_t, list);
			if (gitxmc_rec) {
				printk(KERN_INFO "%8s", gitxmc_rec->memDev->name);

			}
		}
		printk(KERN_INFO "\n");

	}

}
static void mcast_helper_show_mcstream(unsigned int grpidx,MCAST_STREAM_t *mc_stream,unsigned int flag)
{
	unsigned int index = 0;
	printk(KERN_INFO   " mc_stream->num_joined_macs: %d flag%d\n",mc_stream->num_joined_macs,flag);
	printk(KERN_INFO   "%6s %6s %8s\n",
			   "sPort", "dPort","mac addr");
	printk(KERN_INFO  "%3d %3d ",
			   mc_stream->sPort,
		           mc_stream->dPort);
	for(index =0 ; index < mc_stream->num_joined_macs;index++ ) {
		printk(KERN_INFO "%02x:%02x:%02x:%02x:%02x:%02x",
			   mc_stream->macaddr[index][0],mc_stream->macaddr[index][1],mc_stream->macaddr[index][2],mc_stream->macaddr[index][3],mc_stream->macaddr[index][4],mc_stream->macaddr[index][5]);
		printk(KERN_INFO "::");
	}

}


}
#endif


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
	int grpidx = -1;
	grpidx = mcast_helper_get_grpidx();

	if (grpidx == -1)
		return NULL;

	gimc_rec = kmalloc(sizeof(MCAST_GIMC_t), GFP_KERNEL);
	if (gimc_rec == NULL)
		return NULL;

	gimc_rec->mc_stream.rxDev = netdev ;
	mcast_helper_copy_ipaddr(&(gimc_rec->mc_stream.dIP), gaddr);
	mcast_helper_copy_ipaddr(&(gimc_rec->mc_stream.sIP), saddr);
	gimc_rec->mc_stream.proto = proto ;
	gimc_rec->mc_stream.sPort = sport;
	gimc_rec->mc_stream.dPort = dport;

	memcpy(gimc_rec->mc_stream.src_mac, src_mac, ETH_ALEN);

	gimc_rec->grpIdx = grpidx;
	gimc_rec->br_callback_flag = 0;
#ifdef CONFIG_MCAST_HELPER_ACL
	gimc_rec->oifbitmap = 0;
	gimc_rec->probeFlag = 0;
#endif

	memset(gimc_rec->mc_stream.macaddr,0, MAX_MAC*(sizeof(char)*ETH_ALEN));
        gimc_rec->mc_stream.num_joined_macs =0 ;
	INIT_LIST_HEAD(&gimc_rec->list);
	INIT_LIST_HEAD(&gimc_rec->mc_mem_list);
	list_add_tail(&gimc_rec->list, head);

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
	mcast_helper_release_grpidx(gimc_rec->grpIdx);
	list_del(&gimc_rec->list);
	kfree(gimc_rec);
}


/*=============================================================================
 *Function Name: mcast_helper_invoke_return_callback
 *Description	: Function  which invoke the registerd callback from PPA/WLAN ..
 *===========================================================================*/

static int mcast_helper_invoke_return_callback(unsigned int grpidx, struct net_device *netdev,
		MCAST_STREAM_t *mc_stream, unsigned int flag, unsigned int count)
{

	if (mch_accl_enabled)
		mcast_helper_invoke_callback(grpidx, netdev, mc_stream, flag, count);
	return SUCCESS;
}

/*=============================================================================
 *function name: mcast_helper_search_mac_record
 *description  : function to search and get mac record based on gitxmc_rec
 *===========================================================================*/

static MCAST_MAC_t *mcast_helper_search_mac_record(MCAST_MEMBER_t *gitxmc_rec, unsigned char *macaddr)
{
	MCAST_MAC_t *mac_rec = NULL;
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;

	list_for_each_safe(liter, tliter, &gitxmc_rec->macaddr_list) {
		mac_rec = list_entry(liter, MCAST_MAC_t, list);
		if (memcmp(mac_rec->macaddr, macaddr, sizeof(char)*ETH_ALEN) == 0) {
			return mac_rec;
		}
	}
	return NULL;
}

/*=============================================================================
 *function name: mcast_helper_update_macaddr_record
 *description  : function to update the mac address in gitxmc record
 *===========================================================================*/

static MCAST_MAC_t *mcast_helper_update_macaddr_record(MCAST_MEMBER_t *gitxmc_rec, unsigned char *macaddr)
{
	MCAST_MAC_t *mac;

	if (gitxmc_rec == NULL || macaddr == NULL)
		return NULL;

	mac = kmalloc(sizeof(MCAST_MAC_t), GFP_KERNEL);
	if (mac == NULL)
		return NULL;

	memcpy(mac->macaddr, macaddr, sizeof(char)*ETH_ALEN);
	gitxmc_rec->macaddr_count++;

	INIT_LIST_HEAD(&mac->list);
	list_add_tail(&mac->list, &gitxmc_rec->macaddr_list);

	return mac;
}

/*=============================================================================
 *Function Name: mcast_helper_add_gitxmc_record
 *Description	: Function  which add the entry in gitxmc record
 *===========================================================================*/

static MCAST_MEMBER_t *mcast_helper_add_gitxmc_record(unsigned int grpidx,
		struct net_device *netdev, unsigned char *macaddr,
		struct list_head *head)
{
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MAC_t *mac_rec = NULL;
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;

	list_for_each_safe(liter, tliter, head) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		if (gimc_rec != NULL || liter != NULL) {
			if (gimc_rec->grpIdx == grpidx) {
				gitxmc_rec = kmalloc(sizeof(MCAST_MEMBER_t), GFP_KERNEL);
				if (gitxmc_rec == NULL)
					return NULL;
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
		}

	}

	return NULL;
}


/*=============================================================================
 * function name : mcast_helper_update_mac_list
 * description   : function updates the mac list which will be passed to registered call back
 *===========================================================================*/

static unsigned int mcast_helper_update_mac_list(MCAST_MEMBER_t *gitxmc_rec,MCAST_GIMC_t *gimc_rec,unsigned char *macaddr,unsigned int action)
{
	MCAST_MAC_t *mac_rec = NULL;
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	unsigned int flag = 0;
	unsigned int index = 0;

	memset(gimc_rec->mc_stream.macaddr,0, MAX_MAC*(sizeof(char)*ETH_ALEN));
        gimc_rec->mc_stream.num_joined_macs =0 ;

	if ((gitxmc_rec->macaddr_count == 1) && (action == MC_F_DEL)) {
		if (macaddr != NULL) {
			memcpy(&(gimc_rec->mc_stream.macaddr[0][0]),macaddr,sizeof(char)*ETH_ALEN);
			gimc_rec->mc_stream.num_joined_macs = 1;
		}
		flag = MC_F_DEL ;

	}
	else {
		list_for_each_safe(liter, tliter, &gitxmc_rec->macaddr_list) {
			mac_rec = list_entry(liter, MCAST_MAC_t, list);
              		if (mac_rec) {
				if (gimc_rec->mc_stream.num_joined_macs < 64 ) {
					memcpy(&(gimc_rec->mc_stream.macaddr[index][0]),mac_rec->macaddr,sizeof(char)*ETH_ALEN);
					index ++;
				}
			}

		}

		gimc_rec->mc_stream.num_joined_macs = gitxmc_rec->macaddr_count;
		flag = action ;
	}
	return flag;
}



/*=============================================================================
 *Function Name: mcast_helper_delete_gitxmc_record
 *Description	: Function  which delete the entry in gitxmc record
 *===========================================================================*/

static void mcast_helper_delete_gitxmc_record(MCAST_MEMBER_t * gitxmc_rec,MCAST_GIMC_t *gimc_rec,struct net_device *netdev,unsigned char *macaddr, unsigned int action) 
{
	unsigned int flag = 0;

	if ((gitxmc_rec == NULL) || (gimc_rec == NULL))
		return;

#ifdef CONFIG_MCAST_HELPER_ACL
	if ((mch_acl_enabled == 1) && (gitxmc_rec->aclBlocked == 1)) {
		return;
	}
#endif

	flag = mcast_helper_update_mac_list(gitxmc_rec,gimc_rec,macaddr,action);
	mcast_helper_invoke_return_callback(gimc_rec->grpIdx, netdev, (MCAST_STREAM_t *)&(gimc_rec->mc_stream), flag, gitxmc_rec->macaddr_count);
}

/*=============================================================================
 *Function Name: mcast_helper_search_gitxmc_record
 *Description	: Function  to search and get the gitxmc recod based on netdev name
 *===========================================================================*/

static MCAST_MEMBER_t *mcast_helper_search_gitxmc_record(int grpidx,
		struct net_device *netdev, struct list_head *head)
{
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;

	if (!list_empty(head)) {
		list_for_each_safe(liter, tliter, head) {
			gitxmc_rec = list_entry(liter, MCAST_MEMBER_t, list);
			if (gitxmc_rec != NULL)
				if (strncmp (netdev->name, gitxmc_rec->memDev->name, IFNAMSIZ) == 0) {
					return gitxmc_rec;
				}
		}
	}

	return NULL;
}

#if 0 /*Debug code */
static void mcast_helper_show_gitxmc_entry(struct list_head *head)

{
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	struct list_head *gliter = NULL;
	MCAST_GIMC_t *gimc_rec = NULL;

	MCAST_MEMBER_t *gitxmc_rec = NULL;


	printk(KERN_INFO "\n######## Group Index interface info #########");
	list_for_each_safe(liter, tliter, head) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		//if (!list_empty(&gimc_rec->mc_mem_list))
		list_for_each(gliter, &gimc_rec->mc_mem_list) {
			gitxmc_rec = list_entry(gliter, MCAST_MEMBER_t, list);
			if (gitxmc_rec) {
				printk(KERN_INFO "\n intrf  index -> %d \n", gitxmc_rec->memDev->ifindex);
				printk(KERN_INFO "\n intrf name -> %s \n", gitxmc_rec->memDev->name);
			}
		}
	}
	printk(KERN_INFO "######################################\n");
}
#endif

#if 0
void mcast_helper_inet_proto_csum_replace2_u(void *hdr, void *skb, __be32 from, __be32 to)
{
	struct udphdr *uhdr;
	uhdr = (struct udphdr *)hdr;
	inet_proto_csum_replace2(&uhdr->check, (struct sk_buff *)skb, from, to, 0);
}

void mcast_helper_inet_proto_csum_replace4_u(void *hdr, void *skb, __be32 from, __be32 to)
{
	struct udphdr *uhdr;
	uhdr = (struct udphdr *)hdr;
	inet_proto_csum_replace4(&uhdr->check, (struct sk_buff *)skb, from, to, 1);
}
#endif


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
		unsigned int addr[4], unsigned int  new_addr[4],
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

static int mcast_helper_set_ipv6(struct sk_buff *skb, IP_Addr_t *new_saddr, IP_Addr_t *new_daddr)
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

static int mcast_helper_set_ipv4(struct sk_buff *skb, IP_Addr_t *saddr, IP_Addr_t *daddr)
{
	struct iphdr *nh;
	int err;

	err = mcast_helper_make_skb_writeable(skb, skb_network_offset(skb) +
			sizeof(struct iphdr));
	if (unlikely(err))
		return err;

	nh = ip_hdr(skb);

	if (saddr->ipA.ipAddr.s_addr != nh->saddr)
		mcast_helper_set_ip_addr(skb, nh, &nh->saddr, saddr->ipA.ipAddr.s_addr);

	if (daddr->ipA.ipAddr.s_addr != nh->daddr)
		mcast_helper_set_ip_addr(skb, nh, &nh->daddr, daddr->ipA.ipAddr.s_addr);

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

static void mcast_helper_set_udp_port(struct sk_buff *skb, unsigned short *port, unsigned short new_port)
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

static void mcast_helper_set_udp(struct sk_buff *skb, unsigned short udp_src, unsigned short udp_dst)
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

/*=============================================================================
 *function name: mcast_helper_ether_addr_copy
 *description  : function  to copy the ether address
 *===========================================================================*/

static inline void mcast_helper_ether_addr_copy(u8 *dst, const u8 *src)
{
#if defined(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS)
	*(u32 *)dst = *(const u32 *)src;
	*(u16 *)(dst + 4) = *(const u16 *)(src + 4);
#else
	u16 *a = (u16 *)dst;
	const u16 *b = (const u16 *)src;

	a[0] = b[0];
	a[1] = b[1];
	a[2] = b[2];
#endif
}

/*
   static int mcast_helper_set_eth_addr(struct sk_buff *skb,
   unsigned char *eth_src, unsigned char *eth_dst)
   {
   int err;
   err = mcast_helper_make_skb_writeable(skb, ETH_HLEN);
   if (unlikely(err))
   return err;


   mcast_helper_ether_addr_copy(eth_hdr(skb)->h_source, eth_src);
   mcast_helper_ether_addr_copy(eth_hdr(skb)->h_dest, eth_dst);

   skb_postpull_rcsum(skb, eth_hdr(skb), ETH_ALEN *2);

   return 0;
   }
 */

/*=============================================================================
 *function name: mcast_helper_set_sig
 *description  : function  to insert the signature in skb
 *===========================================================================*/


static void mcast_helper_set_sig(struct sk_buff *skb, struct net_device *netdev, int grpidx, int flag)
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
	data = (unsigned char *)udp_hdr(skb)+UDP_HDR_LEN;
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


static unsigned int mcast_helper_acl_probe_pckt_send (struct net_device *inetdev,
		struct net_device *onetdev,
		int grpidx, IP_Addr_t *gaddr, IP_Addr_t *saddr,
		unsigned int proto, unsigned int sport,
		unsigned int dport)
{
	struct iphdr *iph = NULL;
	struct sk_buff *newskb = NULL;
	if (skb_buff) {
		if (ip_hdr(skb_buff)->protocol == IPPROTO_UDP) {
			newskb = skb_copy(skb_buff, GFP_ATOMIC);
			if (newskb != NULL) {
				iph = (struct iphdr *) skb_network_header(newskb);

				mcast_helper_set_ipv4(newskb, saddr, gaddr);
				mcast_helper_set_udp(newskb, sport, dport);
				//				mcast_helper_set_eth_addr(newskb, newskb->dev->dev_addr, eth_hdr(newskb)->h_dest);
				newskb->dev = inetdev;
				mcast_helper_set_sig(newskb, onetdev, grpidx, IPV4);
				netif_receive_skb(newskb); // insert the skb in to  input queue
				return 1;
			}
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
				//mcast_helper_set_eth_addr(newskb, newskb->dev->dev_addr, eth_hdr(newskb)->h_dest);
				newskb->dev = inetdev;
				mcast_helper_set_sig(newskb, onetdev, grpidx, IPV6);

				netif_receive_skb(newskb); // insert the skb in to  input queue
			}
		}
	}


	return NF_ACCEPT;
}


/*=============================================================================
 *function name: mcast_helper_update_entry
 *description  : function searches the gimc record and then updated the gitx record and then
 *		send the probe packets by starting  the proble pckt expiry timer

 *===========================================================================*/

/*call the function to check if GI exist for this group in GIMc table */
static int mcast_helper_update_entry(struct net_device *netdev, struct net_device *rxnetdev, MCAST_REC_t *mc_rec)
{
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	MCAST_MAC_t *mac_rec = NULL;
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	MCAST_MAC_t *mac = NULL;
	int ret=0;
	unsigned int flag = MC_F_UPD ;

	struct list_head *gimc_list = mcast_helper_list_p(mc_rec->groupIP.ipType) ;
	gimc_rec = mcast_helper_search_gimc_record(&(mc_rec->groupIP), &(mc_rec->srcIP), gimc_list);
	if (gimc_rec == NULL) {
		return FAILURE;
	}

	// update the GIMcTx table to add the new interface into the list
	gitxmc_rec = mcast_helper_search_gitxmc_record(gimc_rec->grpIdx, netdev, &gimc_rec->mc_mem_list);
	if (gitxmc_rec == NULL) {
		gitxmc_rec = mcast_helper_add_gitxmc_record(gimc_rec->grpIdx, netdev, mc_rec->macaddr, mcast_helper_list_p(mc_rec->groupIP.ipType)) ;
		if (gitxmc_rec == NULL)
			return FAILURE;
	} else {
		if (mcast_helper_search_mac_record(gitxmc_rec, mc_rec->macaddr) == NULL) {
			mac_rec = mcast_helper_update_macaddr_record(gitxmc_rec, mc_rec->macaddr);
			if (mac_rec == NULL)
				return FAILURE;
		}
	}

	if (mch_acl_enabled) {
		// start the timer here
		if (!mch_timerstarted) {
			mcast_helper_exp_timer.expires = jiffies + (MCH_UPDATE_TIMER *HZ);
			add_timer(&mcast_helper_exp_timer);
			mch_timerstarted  = 1;
		} else {
			mch_timermod  = 1;
			mod_timer(&mcast_helper_exp_timer, jiffies + MCH_UPDATE_TIMER *HZ);
			mch_timermod = 0;
		}

		/*Send the Skb probe packet on interfaces */
		if (gimc_rec->mc_stream.sIP.ipType == IPV6) {
#ifdef CONFIG_MCAST_HELPER_ACL
			gimc_rec->probeFlag = 1;
#endif
			mch_iptype = IPV6;
			mcast_helper_acl_probe_pckt_send6(gimc_rec->mc_stream.rxDev, netdev, gimc_rec->grpIdx, &(gimc_rec->mc_stream.dIP), &(gimc_rec->mc_stream.sIP), gimc_rec->mc_stream.proto, gimc_rec->mc_stream.sPort, gimc_rec->mc_stream.dPort);

		} else {
#ifdef CONFIG_MCAST_HELPER_ACL
			gimc_rec->probeFlag = 1;
#endif
			mch_iptype = IPV4;
			ret = mcast_helper_acl_probe_pckt_send(gimc_rec->mc_stream.rxDev, netdev, gimc_rec->grpIdx, &(gimc_rec->mc_stream.dIP), &(gimc_rec->mc_stream.sIP), gimc_rec->mc_stream.proto, gimc_rec->mc_stream.sPort, gimc_rec->mc_stream.dPort);
			if (ret == 0)
				return FAILURE;
		}
      } else {
		flag = mcast_helper_update_mac_list(gitxmc_rec,gimc_rec,mc_rec->macaddr,flag);
		mcast_helper_invoke_return_callback(gimc_rec->grpIdx,netdev,(MCAST_STREAM_t *)&(gimc_rec->mc_stream),flag, gitxmc_rec->macaddr_count);

	}

	if (!lanserver_timerstarted)
	{
		mcast_lanserver_timer.expires = jiffies + (LANSERVER_UPDATE_TIMER * HZ);
		add_timer(&mcast_lanserver_timer);
		lanserver_timerstarted =1;
	} else {
		lanserver_timermod =1;
		mod_timer(&mcast_lanserver_timer,jiffies + LANSERVER_UPDATE_TIMER * HZ);
		lanserver_timermod =0;
	}

	return SUCCESS;
}


/*=============================================================================
 *function name: mcast_helper_add_entry
 *description  : function searches and adds the entry in gimc/gitcmc record based on
 *		the prameters received  from user space
 *===========================================================================*/


static int mcast_helper_add_entry(struct net_device *netdev, struct net_device *rxnetdev, MCAST_REC_t *mc_rec, unsigned char *src_mac)
{
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	struct net_device *upper_dev = NULL;
	struct list_head *gimc_list = mcast_helper_list_p(mc_rec->groupIP.ipType) ;
	int ret=0;
	unsigned int flag = 0;

	gimc_rec = mcast_helper_search_gimc_record(&(mc_rec->groupIP),&(mc_rec->srcIP),gimc_list);
	if (gimc_rec == NULL) {
		gimc_rec = mcast_helper_add_gimc_record(rxnetdev, &(mc_rec->groupIP), &(mc_rec->srcIP), mc_rec->proto, mc_rec->sPort, mc_rec->dPort, src_mac, gimc_list);
		if (gimc_rec == NULL) {
			return FAILURE;
		}
	}
	/*update the GIMcTx table to add the new interface into the list */
	gitxmc_rec = mcast_helper_search_gitxmc_record(gimc_rec->grpIdx, netdev, &gimc_rec->mc_mem_list);
	if (gitxmc_rec == NULL) {
		gitxmc_rec = mcast_helper_add_gitxmc_record(gimc_rec->grpIdx, netdev, mc_rec->macaddr, mcast_helper_list_p(mc_rec->groupIP.ipType)) ;

		if (gitxmc_rec == NULL)
			return FAILURE;
		memcpy(gimc_rec->mc_stream.macaddr, mc_rec->macaddr, sizeof(char)*ETH_ALEN);

		rtnl_lock();
		upper_dev = netdev_master_upper_dev_get(rxnetdev);
		rtnl_unlock();
		if (upper_dev && (upper_dev->priv_flags & IFF_EBRIDGE)) {
			flag = mcast_helper_update_mac_list(gitxmc_rec, gimc_rec, mc_rec->macaddr, MC_F_ADD);
			mch_br_capture_pkt = 1;
		} else {

			if (mch_acl_enabled) {
				// start the timer here
				if (!mch_timerstarted) {
					mcast_helper_exp_timer.expires = jiffies + (MCH_UPDATE_TIMER *HZ);
					add_timer(&mcast_helper_exp_timer);
					mch_timerstarted  = 1;
				} else {
					mch_timermod  = 1;
					mod_timer(&mcast_helper_exp_timer, jiffies + MCH_UPDATE_TIMER *HZ);
					mch_timermod = 0;
				}

				/*Send the Skb probe packet on interfaces */
				if (gimc_rec->mc_stream.sIP.ipType == IPV6) {
#ifdef CONFIG_MCAST_HELPER_ACL
					gimc_rec->probeFlag = 1;
#endif
					mch_iptype = IPV6;
					mcast_helper_acl_probe_pckt_send6(gimc_rec->mc_stream.rxDev, netdev, gimc_rec->grpIdx, &(gimc_rec->mc_stream.dIP), &(gimc_rec->mc_stream.sIP), gimc_rec->mc_stream.proto, gimc_rec->mc_stream.sPort, gimc_rec->mc_stream.dPort);

				} else {

#ifdef CONFIG_MCAST_HELPER_ACL
					gimc_rec->probeFlag = 1;
#endif
					mch_iptype = IPV4;
					ret = mcast_helper_acl_probe_pckt_send(gimc_rec->mc_stream.rxDev, netdev, gimc_rec->grpIdx, &(gimc_rec->mc_stream.dIP), &(gimc_rec->mc_stream.sIP), gimc_rec->mc_stream.proto, gimc_rec->mc_stream.sPort, gimc_rec->mc_stream.dPort);
					if (ret == 0)
						return FAILURE;
				}
			}else {
				flag = mcast_helper_update_mac_list(gitxmc_rec,gimc_rec,mc_rec->macaddr,MC_F_ADD);
	        		mcast_helper_invoke_return_callback(gimc_rec->grpIdx,netdev,(MCAST_STREAM_t *)&(gimc_rec->mc_stream),MC_F_ADD, gitxmc_rec->macaddr_count);
		        }
		}

		if (!lanserver_timerstarted)
		{
			mcast_lanserver_timer.expires = jiffies + (LANSERVER_UPDATE_TIMER * HZ);
			add_timer(&mcast_lanserver_timer);
			lanserver_timerstarted =1;
		} else {
			lanserver_timermod =1;
			mod_timer(&mcast_lanserver_timer,jiffies + LANSERVER_UPDATE_TIMER * HZ);
			lanserver_timermod =0;
		}
	}
	return SUCCESS;
}

/*=============================================================================
 *function name: mcast_helper_delete_entry
 *description  : function searches and delted the entry in gimc/gitcmc record based on
 *		the prameters received  from user space
 *===========================================================================*/

/*call the function to check if GI exist for this group in GIMc table */
static int mcast_helper_delete_entry(struct net_device *netdev, struct net_device *rxnetdev, MCAST_REC_t *mc_mem)
{
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	struct list_head *gimc_list = mcast_helper_list_p(mc_mem->groupIP.ipType);
	MCAST_MAC_t *mac = NULL;
	struct list_head *liter = NULL;
        struct list_head *tliter = NULL;
	unsigned int flag = 0;

	gimc_rec = mcast_helper_search_gimc_record(&(mc_mem->groupIP), &(mc_mem->srcIP), gimc_list);
	if (gimc_rec == NULL)
		return FAILURE;

	// update the GIMcTx table to del  mcast member mapping table
	gitxmc_rec = mcast_helper_search_gitxmc_record(gimc_rec->grpIdx, netdev, &gimc_rec->mc_mem_list);
	if (gitxmc_rec == NULL)
		return FAILURE;

	if (mc_mem->macaddr != NULL) {
		mac = mcast_helper_search_mac_record(gitxmc_rec,mc_mem-> macaddr); 
	}

	if (mac && gitxmc_rec->macaddr_count <= 1) {
		mcast_helper_delete_gitxmc_record(gitxmc_rec,gimc_rec,netdev,mc_mem->macaddr,MC_F_DEL);
		list_for_each_safe(liter, tliter, &gitxmc_rec->macaddr_list) {
                        MCAST_MAC_t *mac_all = list_entry(liter, MCAST_MAC_t, list);
                        if (mac_all) {
                                list_del(&mac_all->list);
                                kfree(mac_all);
				gitxmc_rec->macaddr_count--;
                        }
                }
		list_del(&gitxmc_rec->list);
		kfree(gitxmc_rec);
	} else if (mac) {
		list_del(&mac->list);
		kfree(mac);
		gitxmc_rec->macaddr_count--;
		flag = MC_F_DEL_UPD ;
		mcast_helper_delete_gitxmc_record(gitxmc_rec,gimc_rec,netdev,mc_mem->macaddr,flag);
	}

	if (list_empty(&gimc_rec->mc_mem_list))
		mcast_helper_delete_gimc_record(gimc_rec);

	if (!lanserver_timerstarted)
	{
		mcast_lanserver_timer.expires = jiffies + (LANSERVER_UPDATE_TIMER * HZ);
		add_timer(&mcast_lanserver_timer);
		lanserver_timerstarted =1;
	} else {
		lanserver_timermod =1;
		mod_timer(&mcast_lanserver_timer,jiffies + LANSERVER_UPDATE_TIMER * HZ);
		lanserver_timermod =0;
	}

	return SUCCESS;
}


/*=============================================================================
 *function name: mcast_helper_update_ftuple_info
 *description  : function to update the five tuple info
 *===========================================================================*/

static void mcast_helper_update_ftuple_info(MCAST_REC_t *mcast_rec, unsigned char *src_mac)
{
	int index;
	if (mcast_rec->groupIP.ipType == IPV4) {
		for (index = 0; index < FTUPLE_ARR_SIZE; index++) {
			if (!strcmp(mcast_rec->rxIntrfName, ftuple_info[index].rxIntrfName)
					&& mcast_helper_is_same_ipaddr((IP_Addr_t *)&mcast_rec->groupIP, (IP_Addr_t *)&ftuple_info[index].groupIP)
					&& mcast_helper_is_same_ipaddr((IP_Addr_t *)&mcast_rec->srcIP, (IP_Addr_t *)&ftuple_info[index].srcIP)) {
				mcast_rec->proto = ftuple_info[index].proto;
				mcast_rec->sPort = ftuple_info[index].sPort;
				mcast_rec->dPort = ftuple_info[index].dPort;
				memcpy(src_mac, ftuple_info[index].src_mac, ETH_ALEN);
				break;
			}
		}
	} else if (mcast_rec->groupIP.ipType == IPV6) {
		for (index = 0; index < FTUPLE_ARR_SIZE; index++) {
			if (!strcmp(mcast_rec->rxIntrfName, ftuple_info6[index].rxIntrfName)
					&& mcast_helper_is_same_ipaddr((IP_Addr_t *)&mcast_rec->groupIP, (IP_Addr_t *)&ftuple_info6[index].groupIP)
					&& mcast_helper_is_same_ipaddr((IP_Addr_t *)&mcast_rec->srcIP, (IP_Addr_t *)&ftuple_info6[index].srcIP)) {
				mcast_rec->proto = ftuple_info6[index].proto;
				mcast_rec->sPort = ftuple_info6[index].sPort;
				mcast_rec->dPort = ftuple_info6[index].dPort;
				memcpy(src_mac, ftuple_info6[index].src_mac, ETH_ALEN);
				break;
			}
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

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	switch (cmd) {
	case MCH_MEMBER_ENTRY_ADD:
			if (copy_from_user(&mcast_mem, (MCAST_REC_t *)arg, sizeof(MCAST_REC_t))) {
				return -EACCES;
			}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 32)
			netdev = dev_get_by_name(mcast_mem.memIntrfName);
			rxnetdev = dev_get_by_name(mcast_mem.rxIntrfName);
#else
			netdev = mcast_helper_dev_get_by_name(&init_net, mcast_mem.memIntrfName);
			rxnetdev = mcast_helper_dev_get_by_name(&init_net, mcast_mem.rxIntrfName);
#endif
			if (rxnetdev == NULL || netdev == NULL)
				return -ENXIO;

			rtnl_lock();
			upper_dev = netdev_master_upper_dev_get(rxnetdev);
			rtnl_unlock();
			if (upper_dev) {
				if (!(upper_dev->priv_flags & IFF_EBRIDGE)) {
					mcast_helper_update_ftuple_info(&mcast_mem, s_mac);
				}
			} else {
				mcast_helper_update_ftuple_info(&mcast_mem, s_mac);
			}

			mcast_helper_add_entry(netdev, rxnetdev, &mcast_mem, s_mac);
			break;
		case MCH_MEMBER_ENTRY_UPDATE:
			if (copy_from_user(&mcast_mem, (MCAST_REC_t *)arg, sizeof(MCAST_REC_t))) {
				return -EACCES;
			}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 32)
			netdev = dev_get_by_name(mcast_mem.memIntrfName);
#else
			netdev = mcast_helper_dev_get_by_name(&init_net, mcast_mem.memIntrfName);
#endif
			if (netdev == NULL)
				return -ENXIO;

			mch_br_capture_pkt = 0;
			mcast_helper_update_entry(netdev, NULL, &mcast_mem);
			break;
		case  MCH_MEMBER_ENTRY_REMOVE:
			if (copy_from_user(&mcast_mem, (MCAST_REC_t *)arg, sizeof(MCAST_REC_t))) {
				return -EACCES;
			}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 32)
			netdev = dev_get_by_name(mcast_mem.memIntrfName);
#else
			netdev = mcast_helper_dev_get_by_name(&init_net, mcast_mem.memIntrfName);
#endif
			if (netdev == NULL)
				return -ENXIO;

			mch_br_capture_pkt = 0;
			mcast_helper_delete_entry(netdev, NULL, &mcast_mem);
			break;
		case MCH_SEVER_ENTRY_GET:
			if (copy_from_user(&mcast_mem, (MCAST_REC_t *)arg, sizeof(MCAST_REC_t))) {
				return -EACCES;
			}

			gimc_list = mcast_helper_session_p(mcast_mem.groupIP.ipType);
			gimc_rec = mcast_helper_search_gimc_record(&(mcast_mem.groupIP), &(mcast_mem.srcIP), gimc_list);
			if (gimc_rec == NULL) {
				return -EACCES;

			} else {
				memcpy(mcast_mem.rxIntrfName, gimc_rec->mc_stream.rxDev->name, strlen(gimc_rec->mc_stream.rxDev->name));
			}

			if (copy_to_user((MCAST_REC_t *)arg, &mcast_mem, sizeof(MCAST_REC_t))) {
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
 *function name: mcast_helper_seq_show
 *description  : proc support to read and output  the mcast helper IPV4 table entries
 *===========================================================================*/

int mcast_helper_seq_show(struct seq_file *seq, void *v)
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
	struct list_head *gimc_list = mcast_helper_list_p(IPV4) ;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (mch_acl_enabled) {
		seq_printf(seq,
				"%3s %10s "
				"%10s %10s %6s %6s %6s %6s %12s\n", "GIdx",
				"RxIntrf", "SA",
				"GA", "proto", "sPort", "dPort", "sMac", "memIntrf(MacAddr)(AclFlag)");
	} else {
		seq_printf(seq,
				"%3s %10s "
				"%10s %10s %6s %6s %6s %6s %12s\n", "GIdx",
				"RxIntrf", "SA",
				"GA", "proto", "sPort", "dPort", "sMac", "memIntrf(MacAddr)");
	}
	list_for_each_safe(liter, tliter, gimc_list) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		if (gimc_rec != NULL) {
			if (gimc_rec->mc_stream.dIP.ipType == IPV4) {
				seq_printf(seq,
						"%3d %10s %10x "
						"%10x %6d %6d %6d  (%02x:%02x:%02x:%02x:%02x:%02x)",
						gimc_rec->grpIdx, gimc_rec->mc_stream.rxDev->name,
						gimc_rec->mc_stream.sIP.ipA.ipAddr.s_addr,
						gimc_rec->mc_stream.dIP.ipA.ipAddr.s_addr,
						gimc_rec->mc_stream.proto,
						gimc_rec->mc_stream.sPort,
						gimc_rec->mc_stream.dPort,
						gimc_rec->mc_stream.src_mac[0], gimc_rec->mc_stream.src_mac[1],
						gimc_rec->mc_stream.src_mac[2], gimc_rec->mc_stream.src_mac[3],
						gimc_rec->mc_stream.src_mac[4], gimc_rec->mc_stream.src_mac[5]);
			}
			if (mch_acl_enabled) {
				list_for_each_safe(gliter, iter, &gimc_rec->mc_mem_list) {
					gitxmc_rec = list_entry(gliter, MCAST_MEMBER_t, list);
					if (gitxmc_rec) {
						list_for_each_safe(gliter_mac, iter_mac, &gitxmc_rec->macaddr_list) {
							mac_rec = list_entry(gliter_mac, MCAST_MAC_t, list);
						}
					}
				}
			} else {
				list_for_each_safe(gliter, iter, &gimc_rec->mc_mem_list) {
					gitxmc_rec = list_entry(gliter, MCAST_MEMBER_t, list);
					if (gitxmc_rec) {
						list_for_each_safe(gliter_mac, iter_mac, &gitxmc_rec->macaddr_list) {
							mac_rec = list_entry(gliter_mac, MCAST_MAC_t, list);
							if (mac_rec) {
								seq_printf(seq, "%8s(%02x:%02x:%02x:%02x:%02x:%02x)", gitxmc_rec->memDev->name, mac_rec->macaddr[0],
										mac_rec->macaddr[1],
										mac_rec->macaddr[2],
										mac_rec->macaddr[3],
										mac_rec->macaddr[4],
										mac_rec->macaddr[5]);
							}
						}
					}
				}
			}
		}
		seq_printf(seq, "\n");
	}
	return 0;
}

int mcast_lanserver_seq_show(struct seq_file *seq, void *v)
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
	struct list_head *gimc_list = mcast_helper_session_p(IPV4) ;

	seq_printf(seq,
                           "%3s %10s "
                           "%10s %10s %6s %6s %6s %6s %12s\n", "GIdx",
                           "RxIntrf", "SA",
                           "GA", "proto" ,"sPort", "dPort", "sMac", "memIntrf(MacAddr)");

	list_for_each_safe(liter,tliter,gimc_list) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		if (gimc_rec != NULL) {
			if (gimc_rec->mc_stream.dIP.ipType == IPV4) {
				seq_printf(seq,
						"%3d %10s %10x "
						"%10x %6d %6d %6d  (%02x:%02x:%02x:%02x:%02x:%02x)",
						gimc_rec->grpIdx, gimc_rec->mc_stream.rxDev->name,
						gimc_rec->mc_stream.sIP.ipA.ipAddr.s_addr,
						gimc_rec->mc_stream.dIP.ipA.ipAddr.s_addr,
						gimc_rec->mc_stream.proto,
						gimc_rec->mc_stream.sPort,
						gimc_rec->mc_stream.dPort,
						gimc_rec->mc_stream.src_mac[0],gimc_rec->mc_stream.src_mac[1],
						gimc_rec->mc_stream.src_mac[2],gimc_rec->mc_stream.src_mac[3],
						gimc_rec->mc_stream.src_mac[4],gimc_rec->mc_stream.src_mac[5]);
			}

			list_for_each_safe(gliter,iter,&gimc_rec->mc_mem_list) {
				gitxmc_rec = list_entry(gliter, MCAST_MEMBER_t, list);
				if (gitxmc_rec) {
					list_for_each_safe(gliter_mac,iter_mac,&gitxmc_rec->macaddr_list) {
						mac_rec = list_entry(gliter_mac, MCAST_MAC_t, list);
						if(mac_rec) {
							seq_printf(seq,"%8s(%02x:%02x:%02x:%02x:%02x:%02x)",gitxmc_rec->memDev->name,mac_rec->macaddr[0],
									mac_rec->macaddr[1],
									mac_rec->macaddr[2],
									mac_rec->macaddr[3],
									mac_rec->macaddr[4],
									mac_rec->macaddr[5]);
						}
					}
				}
			}
		}
		seq_printf(seq,"\n");
	}
	return 0;
}


/*=============================================================================
 *function name: mcast_helper_seq_show6
 *description  : proc support to read and output  the mcast helper IPV6 table entries
 *===========================================================================*/


int mcast_helper_seq_show6(struct seq_file *seq, void *v)
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
	struct list_head *gimc_list = mcast_helper_list_p(IPV6) ;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (mch_acl_enabled) {
		seq_printf(seq,
				"%3s %10s "
				"%32s %32s\t\t\t %6s %6s %6s %12s\n", "GIdx",
				"RxIntrf", "SA",
				"GA", "proto", "sPort", "dPort", "memIntrf(MacAddr)(AclFlag)");
	} else {
		seq_printf(seq,
				"%3s %10s "
				"%32s %32s\t\t\t %6s %6s %6s %12s\n", "GIdx",
				"RxIntrf", "SA",
				"GA", "proto", "sPort", "dPort", "memIntrf(MacAddr)");
	}

	list_for_each_safe(liter, tliter, gimc_list) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		if (gimc_rec != NULL) {
			if (gimc_rec->mc_stream.dIP.ipType == IPV6) {
				seq_printf(seq,
						"%3d %15s %04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X "
						"%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X %6d %6d %6d",
						gimc_rec->grpIdx, gimc_rec->mc_stream.rxDev->name,
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[0],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[1],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[2],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[3],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[4],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[5],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[6],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[7],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[0],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[1],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[2],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[3],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[4],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[5],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[6],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[7],
						gimc_rec->mc_stream.proto,
						gimc_rec->mc_stream.sPort,
						gimc_rec->mc_stream.dPort);
			}

			if (mch_acl_enabled) {
				list_for_each_safe(gliter, iter, &gimc_rec->mc_mem_list) {
					gitxmc_rec = list_entry(gliter, MCAST_MEMBER_t, list);
					if (gitxmc_rec) {
						list_for_each_safe(gliter_mac, iter_mac, &gitxmc_rec->macaddr_list) {
							mac_rec = list_entry(gliter_mac, MCAST_MAC_t, list);
							if(mac_rec) {
#ifdef CONFIG_MCAST_HELPER_ACL
								seq_printf(seq,"%8s(%02x:%02x:%02x:%02x:%02x:%02x)(%d)",gitxmc_rec->memDev->name,
										mac_rec->macaddr[0],
										mac_rec->macaddr[1],
										mac_rec->macaddr[2],
										mac_rec->macaddr[3],
										mac_rec->macaddr[4],
										mac_rec->macaddr[5],
										gitxmc_rec->aclBlocked);
#endif
							}
						}
					}
				}
			} else {
				list_for_each_safe(gliter, iter, &gimc_rec->mc_mem_list) {
					gitxmc_rec = list_entry(gliter, MCAST_MEMBER_t, list);
					if (gitxmc_rec) {
						list_for_each_safe(gliter_mac, iter_mac, &gitxmc_rec->macaddr_list) {
							mac_rec = list_entry(gliter_mac, MCAST_MAC_t, list);
							if (mac_rec) {
								seq_printf(seq, "%8s(%02x:%02x:%02x:%02x:%02x:%02x)", gitxmc_rec->memDev->name,
										mac_rec->macaddr[0],
										mac_rec->macaddr[1],
										mac_rec->macaddr[2],
										mac_rec->macaddr[3],
										mac_rec->macaddr[4],
										mac_rec->macaddr[5]);
							}
						}
					}
				}
			}
		}
		seq_printf(seq, "\n");

	}
	return 0;
}

int mcast_lanserver_seq_show6(struct seq_file *seq, void *v)
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
	struct list_head *gimc_list = mcast_helper_session_p(IPV6) ;

	seq_printf(seq,
		   "%3s %10s "
		   "%32s %32s\t\t\t %6s %6s %6s %12s\n", "GIdx",
		   "RxIntrf", "SA",
		   "GA", "proto" ,"sPort", "dPort","memIntrf(MacAddr)");

	list_for_each_safe(liter,tliter,gimc_list) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		if (gimc_rec != NULL) {
			if (gimc_rec->mc_stream.dIP.ipType == IPV6) {
				seq_printf(seq,
						"%3d %15s %04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X "
						"%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X %6d %6d %6d",
						gimc_rec->grpIdx, gimc_rec->mc_stream.rxDev->name,
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[0],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[1],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[2],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[3],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[4],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[5],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[6],
						gimc_rec->mc_stream.sIP.ipA.ip6Addr.s6_addr16[7],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[0],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[1],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[2],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[3],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[4],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[5],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[6],
						gimc_rec->mc_stream.dIP.ipA.ip6Addr.s6_addr16[7],
						gimc_rec->mc_stream.proto,
						gimc_rec->mc_stream.sPort,
						gimc_rec->mc_stream.dPort);
			}


			list_for_each_safe(gliter,iter,&gimc_rec->mc_mem_list) {
				gitxmc_rec = list_entry(gliter, MCAST_MEMBER_t, list);
				if (gitxmc_rec) {
					list_for_each_safe(gliter_mac,iter_mac,&gitxmc_rec->macaddr_list) {
						mac_rec = list_entry(gliter_mac, MCAST_MAC_t, list);
						if(mac_rec) {
							seq_printf(seq,"%8s(%02x:%02x:%02x:%02x:%02x:%02x)",gitxmc_rec->memDev->name,
									mac_rec->macaddr[0],
									mac_rec->macaddr[1],
									mac_rec->macaddr[2],
									mac_rec->macaddr[3],
									mac_rec->macaddr[4],
									mac_rec->macaddr[5]);
						}
					}
				}
			}
		}
		seq_printf(seq,"\n");

	}
	return 0;
}

/*=============================================================================
 *function name: mcast_proc_open
 *description  : function to open proc for ipv6 table entries
 *===========================================================================*/


int mcast_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, mcast_helper_seq_show, NULL);
}

int lanserver_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, mcast_lanserver_seq_show, NULL);
}

/*=============================================================================
 *function name: mcast_proc_open
 *description  : function to open proc for ipv6 table entries
 *===========================================================================*/


int mcast_proc_open6(struct inode *inode, struct file *file)
{
	return single_open(file, mcast_helper_seq_show6, NULL);
}

int lanserver_proc_open6(struct inode *inode, struct file *file)
{
	return single_open(file, mcast_lanserver_seq_show6, NULL);
}


const struct file_operations mcast_helper_seq_fops = {
	.owner		=	THIS_MODULE,
	.open		=	mcast_proc_open,
	.read		=	seq_read,
	.llseek		=	seq_lseek,
	.release	=	single_release,
};

const struct file_operations mcast_helper_seq_fops6 = {
	.owner          =       THIS_MODULE,
	.open           =       mcast_proc_open6,
	.read           =       seq_read,
	.llseek         =       seq_lseek,
	.release        =       single_release,
};

const struct file_operations mcast_lanserver_seq_fops = {
	.owner		=	THIS_MODULE,
	.open		=	lanserver_proc_open,
	.read		=	seq_read,
	.llseek		=	seq_lseek,
	.release	=	single_release,
};

const struct file_operations mcast_lanserver_seq_fops6 = {
        .owner          =       THIS_MODULE,
        .open           =       lanserver_proc_open6,
        .read           =       seq_read,
        .llseek         =       seq_lseek,
        .release        =       single_release,
};


/*=============================================================================
 *function name: mcast_helper_net_init
 *description  : function to create mcast helper proc entry
 *===========================================================================*/


int mcast_helper_net_init(void)
{

	struct proc_dir_entry *pde, *pde6;
	struct proc_dir_entry *pdl, *pdl6;
	pde = proc_create("mcast_helper", 0, NULL, &mcast_helper_seq_fops);
	if (!pde) {
		goto out_mcast;
	}
	pde6 = proc_create("mcast_helper6", 0, NULL, &mcast_helper_seq_fops6);
	if (!pde6) {
		goto out_mcast;
	}

	pdl = proc_create("mcast_lanserver", 0, NULL, &mcast_lanserver_seq_fops);
	if (!pdl) {
		goto out_mcast;
	}
	pdl6 = proc_create("mcast_lanserver6", 0, NULL, &mcast_lanserver_seq_fops6);
    if (!pdl6) {
            goto out_mcast;
    }
	return 0;

out_mcast:
	remove_proc_entry("mcast_helper", NULL);
	remove_proc_entry("mcast_helper6", NULL);
	remove_proc_entry("mcast_lanserver", NULL);
	remove_proc_entry("mcast_lanserver6", NULL);
	return -ENOMEM;

}

/*=============================================================================
 *function name: mcast_helper_net_exit
 *description  : function to remove  mcast helper proc entry
 *===========================================================================*/

#ifdef CONFIG_SYSCTL

/**Functions to create a proc for ACL enable/disbale support **/

	static
int mcast_helper_acl_sysctl_call_tables(struct ctl_table *ctl, int write,
		void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (write && *(int *)(ctl->data))
		*(int *)(ctl->data) = 1;
	return ret;
}

static struct ctl_table mcast_helper_acl_table[] = {
	{

		.procname       = "multicast-acl",
		.data           = &mch_acl_enabled,
		.maxlen         = sizeof(int),
		.mode           = 0644,
		.proc_handler   = mcast_helper_acl_sysctl_call_tables,
	},
	{ }

};

/**Functions to create a proc to  enable/disbale Multicast accleration for WLAN **/

	static
int mcast_helper_accl_sysctl_call_tables(struct ctl_table *ctl, int write,
		void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (write && *(int *)(ctl->data))
		*(int *)(ctl->data) = 1;
	return ret;
}

static struct ctl_table mcast_helper_accl_table[] = {
	{

		.procname       = "multicast-accleration",
		.data           = &mch_accl_enabled,
		.maxlen         = sizeof(int),
		.mode           = 0644,
		.proc_handler   = mcast_helper_accl_sysctl_call_tables,
	},
	{ }

};


static struct proc_dir_entry *proc_write_entry;

int read_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;
	len = sprintf(buf, "\n %s\n ", (char *)data);
	return len;
}

int write_proc(struct file *file, const char *buf, int count, void *data)
{

	if (count > MCH_MAX_PROC_SIZE)
		count = MCH_MAX_PROC_SIZE;
	if (copy_from_user(data, buf, count))
		return -EFAULT;

	return count;
}

/*
void create_new_proc_entry(void)
{
	proc_write_entry = create_proc_entry("proc_entry", 0666, NULL);
	if (!proc_write_entry)
	{
		printk(KERN_INFO "Error creating proc entry");
		return ;
	}
	proc_write_entry->read_proc = read_proc ;
	proc_write_entry->write_proc = write_proc;
	printk(KERN_INFO "proc initialized");

}
*/
#endif
int __init mcast_helper_proc_init(void)
{
	int ret;
#ifdef CONFIG_SYSCTL
	mcast_acl_sysctl_header = register_net_sysctl(&init_net, "net/", mcast_helper_acl_table);
	if (mcast_acl_sysctl_header == NULL) {
		printk(KERN_WARNING "Failed to register mcast acl sysctl table.\n");
		return 0;
	}

	/*Proc to disable multicast accleration for WLAN */
	mcast_accl_sysctl_header = register_net_sysctl(&init_net, "net/", mcast_helper_accl_table);
	if (mcast_accl_sysctl_header == NULL) {
		printk(KERN_WARNING "Failed to register mcast accl sysctl table.\n");
		return 0;
	}

#endif
	/*create_new_proc_entry();*/

	ret = mcast_helper_net_init();
	return ret;
}


#endif

/*=============================================================================
 *function name: mcast_helper_get_gimc_record
 *description  : function to get the gimc record
 *===========================================================================*/


static MCAST_GIMC_t *mcast_helper_get_gimc_record(int grpidx, struct list_head *head)
{
	struct list_head *liter = NULL;
	struct list_head *tliter = NULL;
	MCAST_GIMC_t *gimc_rec = NULL;
	list_for_each_safe(liter, tliter, head) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
		if (gimc_rec != NULL) {
			if (gimc_rec->grpIdx == grpidx) {
				return gimc_rec;
			}
		}
	}

	return NULL;
}

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
 *		invoke registered callbacks for IPv4 packet
 *===========================================================================*/

int mcast_helper_sig_check_update_ip(struct sk_buff *skb)
{
	unsigned char *data;
	int grpidx = 0;
	int intrfid = 0;
	struct list_head *gimc_list = mcast_helper_list_p(IPV4) ;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	unsigned int flag = 0;


	if (ip_hdr(skb)->protocol == IPPROTO_UDP) {
		data = (unsigned char *)udp_hdr(skb)+UDP_HDR_LEN;

		if (mcast_helper_sig_check(data) == 0)
			return 0;


		/*Signature matched now extract the grpindex and the call update gitxmc table */
		grpidx = mcast_helper_extract_grpidx(data, sizeof(mch_signature)-1);
		intrfid = mcast_helper_extract_intrfidx(data, sizeof(mch_signature));
		gimc_rec = mcast_helper_get_gimc_record(grpidx, gimc_list);
		if (gimc_rec) {
			if (skb->dev->ifindex == intrfid) {
				/*update the GIMcTx table to add the new interface into the list */
				gitxmc_rec = mcast_helper_search_gitxmc_record(gimc_rec->grpIdx, skb->dev, &gimc_rec->mc_mem_list);
				if (gitxmc_rec != NULL) {
#ifdef CONFIG_MCAST_HELPER_ACL
					gitxmc_rec->aclBlocked = 0;
#endif
					flag = mcast_helper_update_mac_list(gitxmc_rec,gimc_rec,NULL,MC_F_ADD);
					mcast_helper_invoke_return_callback(gimc_rec->grpIdx,gitxmc_rec->memDev,(MCAST_STREAM_t *)&(gimc_rec->mc_stream),flag, gitxmc_rec->macaddr_count);

				}

			}
			/*update the oifindex bitmap to be used for evaluating after timer expires */
#ifdef CONFIG_MCAST_HELPER_ACL
			gimc_rec->oifbitmap |= 1ULL << skb->dev->ifindex;
#endif
		}

		return 1;
	}
	return 0;
}


/*=============================================================================
 *function name: mcast_helper_sig_check_update_ip6
 *description  : function to check signature and update the gitxmc table and invoke
 *		registered callbacks for the IPv6 packet
 *===========================================================================*/



int mcast_helper_sig_check_update_ip6(struct sk_buff *skb)
{
	unsigned char *data;
	int grpidx = 0;
	int intrfid = 0;
	struct list_head *gimc_list = mcast_helper_list_p(IPV6) ;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	unsigned int flag = 0;

	data = (unsigned char *)udp_hdr(skb)+UDP_HDR_LEN;

	if (mcast_helper_sig_check(data) == 0)
		return 0;

	/*Signature matched now extract the grpindex and the call update gitxmc table */
	grpidx = mcast_helper_extract_grpidx(data, sizeof(mch_signature)-1);

	intrfid = mcast_helper_extract_intrfidx(data, sizeof(mch_signature));
	gimc_rec = mcast_helper_get_gimc_record(grpidx, gimc_list);
	if (gimc_rec) {
		if (skb->dev->ifindex == intrfid) {
			/*update the GIMcTx table to add the new interface into the list */
			gitxmc_rec = mcast_helper_search_gitxmc_record(gimc_rec->grpIdx, skb->dev, &gimc_rec->mc_mem_list);
			if (gitxmc_rec != NULL) {

#ifdef CONFIG_MCAST_HELPER_ACL
				gitxmc_rec->aclBlocked = 0;
#endif
				flag = mcast_helper_update_mac_list(gitxmc_rec,gimc_rec,NULL,MC_F_UPD);
				mcast_helper_invoke_return_callback(gimc_rec->grpIdx,gitxmc_rec->memDev,(MCAST_STREAM_t *)&(gimc_rec->mc_stream),flag, gitxmc_rec->macaddr_count);
			}

		}
		/*update the oifindex bitmap to be used for evaluating after timer expires */
#ifdef CONFIG_MCAST_HELPER_ACL
		gimc_rec->oifbitmap |= 1ULL << skb->dev->ifindex;
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
	struct list_head *gimc_list ;
	const unsigned char *dest = eth_hdr(skb)->h_dest;

	if (!mch_acl_enabled)
		return 1;
	if (mch_iptype == IPV6)
		gimc_list = mcast_helper_list_p(IPV6) ;
	else
		gimc_list = mcast_helper_list_p(IPV4) ;

	if (mch_timerstarted && is_multicast_ether_addr(dest)) {

		if (eth_hdr(skb)->h_proto == MCH_ETH_P_IP) {
			if (ip_hdr(skb)->protocol == IPPROTO_UDP)
				mcast_helper_sig_check_update_ip(skb);

		} else if (eth_hdr(skb)->h_proto == MCH_ETH_P_IPV6) {
			if (ipv6_hdr(skb)->nexthdr == IPPROTO_UDP)
				mcast_helper_sig_check_update_ip6(skb);

		}
	}
	return 1;
}

EXPORT_SYMBOL(mcast_helper_sig_check_update);

/*=============================================================================
 *function name: mcast_helper_timer_handler
 *description  : function handling mcast herlper timer expiry
 *===========================================================================*/

static void mcast_helper_timer_handler(unsigned long data)
{
	struct list_head *liter = NULL;
	struct list_head *gliter = NULL;
	struct list_head *tliter = NULL;
	struct list_head *pliter = NULL;
	struct list_head *gimc_list = NULL;
	MCAST_GIMC_t *gimc_rec = NULL;
	MCAST_MEMBER_t *gitxmc_rec = NULL;
	unsigned int i=0;
	unsigned int delflag=1;
	unsigned long long int oifbitmap = 0;
	unsigned int flag = 0;

	if (mch_iptype == IPV6)
		gimc_list = mcast_helper_list_p(IPV6) ;
	else
		gimc_list = mcast_helper_list_p(IPV4) ;

	if (mch_timermod) {
		mch_timermod = 0;
		return;
	}
	list_for_each_safe(liter, gliter, gimc_list) {
		gimc_rec = list_entry(liter, MCAST_GIMC_t, list);
#ifdef CONFIG_MCAST_HELPER_ACL
		if (gimc_rec->probeFlag == 1) {
			list_for_each_safe(tliter, pliter, &gimc_rec->mc_mem_list) {
				gitxmc_rec = list_entry(tliter, MCAST_MEMBER_t, list);
				if (gitxmc_rec) {
					oifbitmap = gimc_rec->oifbitmap;
					i = 0;
					delflag = 1;
					do {

						if (oifbitmap & 0x1) {
							if (gitxmc_rec->memDev->ifindex == i) {
								if (gitxmc_rec->aclBlocked == 1) {

									flag = mcast_helper_update_mac_list(gitxmc_rec,gimc_rec,NULL,MC_F_ADD);
									mcast_helper_invoke_return_callback(gimc_rec->grpIdx, gitxmc_rec->memDev, (MCAST_STREAM_t *)&(gimc_rec->mc_stream), MC_F_ADD, gitxmc_rec->macaddr_count);
									gitxmc_rec->aclBlocked = 0;
								}
								delflag = 0;
								break;
							}

						}
						i++;
					} while (oifbitmap >>= 1);

					if (delflag == 1) {
						/* delete this interface from the gitxmc list and invoke registered call back for this if any */

						flag = mcast_helper_update_mac_list(gitxmc_rec,gimc_rec,NULL,MC_F_DEL);
						mcast_helper_invoke_return_callback(gimc_rec->grpIdx,gitxmc_rec->memDev,(MCAST_STREAM_t *)&(gimc_rec->mc_stream),flag, gitxmc_rec->macaddr_count);
						gitxmc_rec->aclBlocked=1;
					}
				}
			}
			gimc_rec->oifbitmap = 0;
			gimc_rec->probeFlag = 0;
		}
#endif
	}

	mch_iptype = 0;
	mch_timerstarted = 0;
}

/*=============================================================================
 * function name : mcast_lanserver_timer_handler
 * description   : function handling lanserver entry timer expiry
 *===========================================================================*/

static void mcast_lanserver_timer_handler(unsigned long data)
{
	/* update the lanserver hook list when mcast_helper entry update */
	struct list_head *vliter = NULL;
	struct list_head *vtliter = NULL;
	MCAST_GIMC_t *lanserver_gimc_rec = NULL;
	struct list_head *gimc_list_v4 = mcast_helper_list_p(IPV4);
	struct list_head *gimc_list_v6 = mcast_helper_list_p(IPV6);
	struct list_head *gimc_lanserver_v4 = mcast_helper_session_p(IPV4);
	struct list_head *gimc_lanserver_v6 = mcast_helper_session_p(IPV6);

	if (lanserver_timermod) {
		lanserver_timermod = 0;
		return;
	}

	/* loop the IPv4 lanserver group list */
	list_for_each_safe(vliter,vtliter,gimc_lanserver_v4) {
		lanserver_gimc_rec = list_entry(vliter, MCAST_GIMC_t, list);

		MCAST_GIMC_t * mcast_gimc_rec = mcast_helper_search_gimc_record(&(lanserver_gimc_rec->mc_stream.dIP),&(lanserver_gimc_rec->mc_stream.sIP),gimc_list_v4);

		/* if not found in mcast_helper, then indicate no any client join this GROUP, will delete the lanserver Group fistly */
		if(mcast_gimc_rec == NULL)
		{
			if (list_empty(&lanserver_gimc_rec->mc_mem_list)){
				mcast_helper_delete_gimc_record(lanserver_gimc_rec);
			}
		}
	}


	/* loop the IPv6 lanserver group list */
	list_for_each_safe(vliter,vtliter,gimc_lanserver_v6) {
		lanserver_gimc_rec = list_entry(vliter, MCAST_GIMC_t, list);
		MCAST_GIMC_t * mcast_gimc_rec = mcast_helper_search_gimc_record(&(lanserver_gimc_rec->mc_stream.dIP),&(lanserver_gimc_rec->mc_stream.sIP),gimc_list_v6);

		/* if not found in mcast_helper, then indicate no any client join this GROUP, will delete the lanserver Group fistly */
		if(mcast_gimc_rec == NULL)
		{
			if (list_empty(&lanserver_gimc_rec->mc_mem_list)){
				mcast_helper_delete_gimc_record(lanserver_gimc_rec);
			}
		}
	}

	lanserver_timerstarted = 0;
}
/*=============================================================================
 *function name: mcast_helper_init_timer
 *description  : function handling timer Initialization
 *===========================================================================*/

static int mcast_helper_init_timer(int delay)
{

	init_timer(&mcast_helper_exp_timer);
	mcast_helper_exp_timer.expires = jiffies + delay *HZ;
	mcast_helper_exp_timer.data = 0;
	mcast_helper_exp_timer.function = mcast_helper_timer_handler;
	return 0;
}

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
 *function name: mcast_helper_init_module
 *description  : Multucast helper module initilization
 *===========================================================================*/


static int __init mcast_helper_init_module(void)
{
	int ret_val ,ret = 0;
	int index = 0;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	GSW_API_HANDLE gswl;
	GSW_PCE_rule_t pcecfg;
#endif
	/*
	 * Alloc the chrdev region for mcast helper
	 */
	ret_val = alloc_chrdev_region(&mch_major , 0 ,1 , DEVICE_NAME);

	/*
	 *Negative values signify an error
	 */
	if (ret_val < 0) {
		printk(KERN_ALERT "%s failed with %d\n",
		       "Sorry, alloc_chrdev_region failed for the mcast  device ", ret_val);
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

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	/* Do GSW_L configuration to avoid control packet flooding */
	gswl = gsw_api_kopen(SWITCH_DEV);
	if (gswl == 0) {
		pr_err (" Mcast helper init- Failed to open swtch dev\n");
	}

	memset(&pcecfg, 0, sizeof(pcecfg));
	pcecfg.pattern.nIndex = 18;
	pcecfg.pattern.bEnable = 1;
	pcecfg.pattern.bParserFlagMSB_Enable = 1;
	pcecfg.pattern.nParserFlagMSB = 0x0040;
	pcecfg.pattern.nParserFlagMSB_Mask = 0xFFBF;
	pcecfg.action.ePortMapAction = GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
	pcecfg.action.nForwardPortMap[0] = 0x01; /*CPU port */

	if ((ret = gsw_api_kioctl(gswl, GSW_PCE_RULE_WRITE, &pcecfg)) < GSW_statusOk) {
		pr_err("GSW_PCE_RULE_WRITE returned failure on GSWIPL %d\n",ret);
	}
	gsw_api_kclose(gswl);
#endif

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

	five_tuple_info_ptr = NULL;
#if IS_ENABLED(CONFIG_IPV6_MROUTE)
	five_tuple_info6_ptr = NULL;
#endif
	five_tuple_br_info_ptr = NULL;
	five_tuple_br_info_hook = NULL;
	mcast_helper_sig_check_update_ptr = NULL;
	mch_acl_enabled = 0;
	if (skb_buff)
		kfree_skb(skb_buff);
	if (skb_buff6)
		kfree_skb(skb_buff6);

#ifdef CONFIG_SYSCTL
	unregister_net_sysctl_table(mcast_acl_sysctl_header);
	unregister_net_sysctl_table(mcast_accl_sysctl_header);
#endif
	remove_proc_entry("mcast_helper", NULL);
	remove_proc_entry("mcast_helper6", NULL);
	remove_proc_entry("proc_entry", NULL);
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
MODULE_ALIAS_RTNL_LINK("mcast_helper");
MODULE_ALIAS_RTNL_LINK("mcast_helper6");
MODULE_DESCRIPTION("Multicast helper ");
MODULE_AUTHOR("Srikanth");

