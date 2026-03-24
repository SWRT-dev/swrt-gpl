/******************************************************************************
**
** FILE NAME	: lgm_pp_hal.c
** PROJECT	: LGM
** MODULES	: PPA PPv4 HAL
**
** DATE		: 29 Oct 2018
** AUTHOR	: Kamal Eradath
** DESCRIPTION	: PPv4 hardware abstraction layer
** COPYRIGHT	: Copyright (c) 2020, MaxLinear, Inc.
**                Copyright (c) 2014, Intel Corporation.
**
**	 For licensing information, see the file 'LICENSE' in the root folder of
**	 this software module.
**
** HISTORY
** $Date		$Author		 	$Comment
** 29 Oct 2018		Kamal Eradath		Initial Version
*******************************************************************************/
/*
 * ####################################
 *		Head File
 * ####################################
 */
/*
 *	Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/udp.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <net/checksum.h>
#include <linux/clk.h>
#include <net/ip_tunnels.h>
#include <linux/if_arp.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <asm/uaccess.h>
#include <net/ipv6.h>
#include <net/ip6_tunnel.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/hrtimer.h>
#include <linux/workqueue.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/if_pppox.h>
#include <linux/net.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/route.h>
#include <net/inet6_hashtables.h>

/*
 *	Chip Specific Head File
 */
#include <linux/pp_api.h>
#include <linux/pktprs.h>
#include <net/datapath_api.h>
#include <net/intel_np_lro.h>
#include <net/datapath_api_qos.h>
#include <net/datapath_api_vpn.h>
#include <net/datapath_api_gswip32.h>
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_hal_api.h>
#include <net/qos_mgr/qos_hal_api.h>
#include <net/qos_mgr/qos_mgr_hook.h>
#include "../../ppa_api/ppa_api_netif.h"
#include "../../ppa_api/ppa_api_session.h"
#include "lgm_pp_hal.h"

#include <linux/skb_extension.h>
/*
 * ####################################
 *			Version No.
 * ####################################
 */
#define VER_FAMILY	0x10
#define VER_DRTYPE	0x04 /* 4: Stack/System Adaption Layer driver*/
#define VER_INTERFACE	0x1 /*	bit 0: MII 1 */
#define VER_ACCMODE	0x11 /*	bit 0: Routing &Bridging */

#define VER_MAJOR	1
#define VER_MID		0
#define VER_MINOR	0

#define SESSION_RETRY_MAX	 8
#define ETH_MTU	1500
/*
 *	Compilation Switch
 */
/*!
	\brief Turn on/off debugging message and disable inline optimization.
 */
#define ENABLE_DEBUG			1
/*!
	\brief Turn on/off ASSERT feature, print message while condition is not fulfilled.
 */
#define ENABLE_ASSERT			1
/*@}*/

#if defined(ENABLE_DEBUG) && ENABLE_DEBUG
	#define ENABLE_DEBUG_PRINT	1
	#define DISABLE_INLINE		1
#endif

#if defined(DISABLE_INLINE) && DISABLE_INLINE
	#define INLINE
#else
	#define INLINE			inline
#endif

#if defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT
	#undef	dbg
	static unsigned int lgm_pp_hal_dbg_enable = 0;
	#define dbg(format, arg...) do { if ( lgm_pp_hal_dbg_enable ) printk(KERN_WARNING ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
	#if !defined(dbg)
	#define dbg(format, arg...)
	#endif
#endif

#if defined(ENABLE_ASSERT) && ENABLE_ASSERT
	#define ASSERT(cond, format, arg...) do { if ( !(cond) ) printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
	#define ASSERT(cond, format, arg...)
#endif

#ifndef NUM_ENTITY
#define NUM_ENTITY(x)	(sizeof(x) / sizeof(*(x)))
#endif

#define PPA_API_PROC 		1
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
#define LITEPATH_HW_OFFLOAD	1
#endif /*IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)*/

#define	DEFAULT_POLLING_TIME	4 /* 4 seconds */
#define DEFAULT_DB_SLICE		(1024U) /*number of db entries polled per iteration*/
#define DEFAULT_INACT_ARRAY_SIZE	(8192U) /*8K*/

#define ETH_HLEN		14	/* Total octets in header.	 */
#define IPV4_HLEN		20
#define IPV6_HLEN		40
#define VLAN_HLEN		4
#define PPPOE_HLEN		8
#define ESP_HLEN		8
#define ESP_IV			16
#define PPPOE_IPV4_TAG		0x0021
#define PPPOE_IPV6_TAG		0x0057

#define COPY_16BYTES 16
#define CPU_PORT_WLAN_BIT_MODE	9

#if IS_ENABLED(CONFIG_PPA_BBF247_MODE1)
#define BBF247_MAX_SESS_ID		20
#define BBF247_INVALID_SESS_ID	U32_MAX
#endif /*CONFIG_PPA_BBF247_MODE1*/

/*
 * ####################################
 *		Declaration
 * ####################################
 */

#if defined(PPA_API_PROC)
static int proc_read_ppv4_rtstats_seq_open(struct inode *, struct file *);
static ssize_t proc_clear_ppv4_rtstats(struct file *, const char __user *, size_t , loff_t *);

static int proc_read_ppv4_accel_seq_open(struct inode *, struct file *);
static ssize_t proc_set_ppv4_accel(struct file *, const char __user *, size_t , loff_t *);

static int proc_read_ppv4_debug_seq_open(struct inode *, struct file *);
static ssize_t proc_set_ppv4_debug(struct file *, const char __user *, size_t , loff_t *);

#if IS_ENABLED(CONFIG_LGM_TOE)
static int proc_read_ppv4_lro_seq_open(struct inode *, struct file *);
static ssize_t proc_set_ppv4_lro(struct file *, const char __user *, size_t , loff_t *);
#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/

#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
static int proc_read_ppv4_lp_seq_open(struct inode *, struct file *);
static ssize_t proc_set_ppv4_lp(struct file *, const char __user *, size_t , loff_t *);
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/

#if IS_ENABLED(CONFIG_PPA_BBF247_MODE1)
static int ppa_run_cmd(const char *cmd);
static int32_t ppa_add_bbf247_mode1_us_session(PPA_NETIF *rxif, PPA_NETIF *txif,
					       uint32_t *session_id);
static int32_t ppa_add_bbf247_mode1_ds_session(PPA_NETIF *rxif, PPA_NETIF *txif,
					       u32 class, uint32_t *session_id);
static int32_t ppa_pp_update_ports(PPA_NETIF *lan_if, PPA_NETIF *vuni_if);
static void ppa_bbf247_mode1(PPA_IFNAME vani_netif_name[PPA_IF_NAME_SIZE],
			     PPA_IFNAME vuni_netif_name[PPA_IF_NAME_SIZE],
			     PPA_IFNAME lan_netif_name[PPA_IF_NAME_SIZE],
			     bool enable);
static int proc_read_ppv4_bbf247_hgu(struct seq_file *seq, void *v);
static int proc_read_ppv4_bbf247_hgu_seq_open(struct inode *inode,
					      struct file *file);
static ssize_t proc_set_ppv4_bbf247_hgu(struct file *file,
					const char __user *buf,
					size_t count, loff_t *data);
#endif /*CONFIG_PPA_BBF247_MODE1*/
#endif /*defined(PPA_API_PROC)*/
struct client_info{
	uint32_t sess_id;		/* Client session id */
	struct net_device *netdev;	/* tx netdevice */
	struct pp_hash sess_hash;	/* HW hash of the dst */
};

/*Each client will have one session in pp*/
typedef struct mc_db_nod {
	bool		used;	/*index is in use*/
	struct pp_stats	stats;	/*statistics per group*/
	struct pp_hash  grp_sess_hash; /*HW hash of the group session*/
	struct client_info dst[MAX_MC_CLIENT_PER_GRP]; /* mc clients*/
} MC_DB_NODE;


#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
struct lp_info {
	uint8_t l3_offset;		/* ip layer offset*/
	uint8_t l4_offset;		/* transport layer offset*/
	struct dst_entry *dst;		/* route entry */
	PPA_NETIF *netif;		/* rx net device*/
};
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/

/*Each unicast entry in pp will need following information kept in hal*/
typedef struct uc_db_node {
	bool			used;	/*index is in use*/
	struct pp_stats		stats;	/*statistics*/
	void 			*node; 	/*unicast session node pointer*/
#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
	struct lp_info		*lp_rxinfo; /*litepath rx packet info*/
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
	bool			lro;	/*lro session*/
} PP_HAL_DB_NODE;

typedef struct ipsec_tun {
	bool trns_mode;			/* Transport mode */
	bool is_inbound;		/* Inbound dir */
	bool ipv6;			/* ESP packet packet is IPv6*/
	bool pppoe;			/* PPPoE header */
	uint8_t hdr_len;		/* Total header length of the ESP packet */
	uint8_t esp_hdr_len;		/* ESP header + IV length*/
	uint8_t ip_offset;		/* Offset of the IP header in the ESP packet */
	uint32_t iv_sz;			/* tunnel IV size */
	void *hdr;			/* Backed up ESP header */
	struct list_head sessions;	/* PPA related sessions list(p_item) */
} PP_IPSEC_TUN_NODE;

struct mc_dev_priv{
	struct module	*owner;
};

#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
struct lpdev_priv{
	struct module	*owner;
	dp_subif_t	dp_subif;
};
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/

#if IS_ENABLED(CONFIG_PPA_BBF247_MODE1)
struct BBF247_Session_Info {
	uint32_t sess_id[BBF247_MAX_SESS_ID];
	uint32_t sess_cnt;
	char *eth_arr[BBF247_MAX_SESS_ID];
};
#endif /*CONFIG_PPA_BBF247_MODE1*/

#if IS_ENABLED(CONFIG_LGM_TOE)
extern int32_t ppa_bypass_lro(PPA_SESSION *p_session);
#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/
/*
 * ####################################
 *	 Global Variable
 * ####################################
 */

/* gswip port bitmap map */
static uint32_t g_port_map = 0x8FFF; /*{ 1 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 }*/

static uint32_t g_us_accel_enabled = 1;
static uint32_t g_ds_accel_enabled = 1;
#if IS_ENABLED(CONFIG_LGM_TOE)
static uint32_t g_lro_enabled = 1;
#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/
static uint32_t g_max_hw_sessions = MAX_UC_SESSION_ENTRIES;
#if IS_ENABLED(CONFIG_PPA_BBF247_MODE1)
static struct BBF247_Session_Info *bbf247_sess;
#endif /*CONFIG_PPA_BBF247_MODE1*/

/* Gswip logical qid*/
static uint16_t g_gswip_qid;
static struct nf_node g_mcast_nf = {0};
static struct nf_node g_frag_nf = {0};
static struct nf_node g_reas_nf = {0};
#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
/*TBD: Disabled until LRO/TSO stabilizes*/
static uint32_t g_lp_enabled = 0;
static struct nf_node g_lpdev = {0};
static uint32_t g_pp_hw_version = 0;
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
#if IS_ENABLED(CONFIG_INTEL_VPN)
static struct nf_node g_vpna_conn = {0};
static struct nf_node g_vpn_nf = {0};
static spinlock_t g_tun_db_lock;
static PP_IPSEC_TUN_NODE ipsec_tun_db[MAX_TUN_ENTRIES]={0};
#endif

/*multicast group DB*/
static MC_DB_NODE mc_db[MAX_MC_GROUP_ENTRIES]={0};
/*DB to maintain pp status in pp hal */
static PP_HAL_DB_NODE	*pp_hal_db=NULL;
/*DB to maintain IPSec tunnel entries*/

/*Session delete callback */
static void (*del_routing_session_cb)(void *p_item)=NULL;

static spinlock_t		g_hal_db_lock;
static spinlock_t		g_hal_mc_db_lock;
static struct hrtimer		g_gc_timer;
static struct workqueue_struct	*g_workq=0;

#ifndef PP_INACTIVE_SCAN
static uint32_t		g_db_index = 0;
#else
static uint32_t		g_iter_cnt = 0;
#endif
static uint32_t		g_sess_timeout_thr = 0;

/* Global ppv4 hal counters */
static uint64_t nsess_add=0;
static uint64_t nsess_del=0;
static uint64_t nsess_del_fail=0;
static uint64_t nsess_add_succ=0;
static uint64_t nsess_add_fail_rt_tbl_full=0;
static uint64_t nsess_add_fail_coll_full=0;
static uint32_t nsess_add_fail_oth=0;

#if defined(PPA_API_PROC)
static struct dentry *g_ppa_ppv4hal_debugfs_dir = NULL;
static int g_ppa_ppv4hal_debugfs_flag = 0;

static struct file_operations g_proc_file_ppv4_rtstats_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_ppv4_rtstats_seq_open,
	.read		= seq_read,
	.write		= proc_clear_ppv4_rtstats,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};

static struct file_operations g_proc_file_ppv4_accel_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_ppv4_accel_seq_open,
	.read		= seq_read,
	.write		= proc_set_ppv4_accel,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};

static struct file_operations g_proc_file_ppv4_debug_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_ppv4_debug_seq_open,
	.read		= seq_read,
	.write		= proc_set_ppv4_debug,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};

#if IS_ENABLED(CONFIG_LGM_TOE)
static struct file_operations g_proc_file_ppv4_lro_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_ppv4_lro_seq_open,
	.read		= seq_read,
	.write		= proc_set_ppv4_lro,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};
#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/

#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
static struct file_operations g_proc_file_ppv4_lp_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_ppv4_lp_seq_open,
	.read		= seq_read,
	.write		= proc_set_ppv4_lp,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/

#if IS_ENABLED(CONFIG_PPA_BBF247_MODE1)
static struct file_operations g_proc_file_ppv4_bbf247_hgu_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_ppv4_bbf247_hgu_seq_open,
	.read		= seq_read,
	.write		= proc_set_ppv4_bbf247_hgu,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};
#endif /*CONFIG_PPA_BBF247_MODE1*/

#endif /*defined(PPA_API_PROC)*/

/*
 * ####################################
 *		Extern Variable
 * ####################################
 */
#if defined(PPA_API_PROC)
extern int g_ppa_debugfs_flag;
extern struct dentry *g_ppa_debugfs_dir;
#endif /*defined(PPA_API_PROC)*/

/*
 * ####################################
 *		Extern Function
 * ####################################
 */

extern uint32_t ppa_drv_generic_hal_register(uint32_t hal_id, ppa_generic_hook_t generic_hook);
extern void ppa_drv_generic_hal_deregister(uint32_t hal_id);

extern uint32_t ppa_drv_register_cap(PPA_API_CAPS cap, uint8_t wt, PPA_HAL_ID hal_id);
extern uint32_t ppa_drv_deregister_cap(PPA_API_CAPS cap, PPA_HAL_ID hal_id);

/*
 * ####################################
 *			Local Function
 * ####################################
 */
#define PRINT_SKB 1
#if IS_ENABLED(PRINT_SKB)
static int p_flg = 0;
#endif /*IS_ENABLED(PRINT_SKB)*/

static inline void dumpskb(uint8_t *ptr, int len, int flag)
{
#if IS_ENABLED(PRINT_SKB)
	p_flg++;

	if (!lgm_pp_hal_dbg_enable)
		return;

	if (flag || ((p_flg % 10) == 0)) {
		print_hex_dump(KERN_ERR, "", DUMP_PREFIX_NONE, 16, 1,
			       ptr, len, false);
		p_flg = 0;
	}
#endif
}
static inline void flush_hw_sessions(void)
{
	int i;
	void *session_node= NULL;

	if (!pp_hal_db)
		return;

	for ( i=0; i < g_max_hw_sessions; i++) {
		session_node = NULL;
		spin_lock_bh(&g_hal_db_lock);
		if (pp_hal_db[i].used) {
			session_node = pp_hal_db[i].node;
			spin_unlock_bh(&g_hal_db_lock);
			if (session_node) {
				del_routing_session_cb(session_node);
			}
		} else {
			spin_unlock_bh(&g_hal_db_lock);
		}
	}
	dbg("%s %d Session flush Done\n",__FUNCTION__, __LINE__);
}

int32_t queue_delete_cb(uint32_t queue_id)
{
	if (!queue_id)
		return PPA_FAILURE;

	flush_hw_sessions();
	return PPA_SUCCESS;
}

static inline uint32_t is_valid_session(uint32_t session_id)
{
	if (session_id && (session_id <= g_max_hw_sessions)) {
		return 1;
	}
	return 0;
}

static inline uint32_t is_lansession(uint32_t flags)
{
	return ((flags & SESSION_LAN_ENTRY) ? 1 : 0);
}

int32_t is_lgm_special_netif(struct net_device *netif)
{
	/* multicast netif */
	if (ppa_is_netif_equal(netif, g_mcast_nf.dev)) {
		return 1;
#if IS_ENABLED(CONFIG_INTEL_VPN)
	} else if (ppa_is_netif_equal(netif, g_vpn_nf.dev)) {
	/* vpnnf_netif */
		return 1;
#endif /*IS_ENABLED(CONFIG_INTEL_VPN)*/
	} else if (ppa_is_netif_equal(netif, g_lpdev.dev)) {
	 /*litepath netif*/
		return 1;
	} else if (ppa_is_netif_equal(netif, g_vpna_conn.dev)) {
		/* vpn_adapter netif*/
		return 1;
	}
	return 0;
}
EXPORT_SYMBOL(is_lgm_special_netif);

static inline uint16_t get_gswip_qid(struct dp_spl_cfg *dpcon)
{
	struct dp_qos_q_logic q_logic = {0};

	if (!g_gswip_qid) {
		/* Qid to be used from NF to send the backet back to Gswip */
		ppa_memset(&q_logic, 0, sizeof(q_logic));
		q_logic.q_id = dpcon->egp[1].qid;
		/* physical to logical qid */
		if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
			dbg("%s:%d ERROR Failed to Logical Queue Id\n", __func__, __LINE__);
			return PPA_FAILURE;
		}
		g_gswip_qid = q_logic.q_logic_id;
	}
	return g_gswip_qid;
}

static inline uint32_t lgm_get_session_color(struct uc_session_node *p_item)
{
	return PP_COLOR_GREEN;; /*FIXME: 1 = green, 2 = orange, 3 = red */
}

static inline uint16_t get_cpu_portinfo(void)
{
	/*TBD: this api will be implemented by dp later to return all the 8 CPU GPIds and qids*/
	return 16;
}

static inline uint16_t get_cpu_qid(void)
{
	/*TBD: this api shall call dp to retrieve the correct CPU queueid*/
	return 2;
}

static int mcdev_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	consume_skb(skb);
	return NETDEV_TX_OK;
}

static struct net_device_ops mcdev_ops = {
	.ndo_start_xmit	= mcdev_xmit,

};

static void mcdev_setup(struct net_device *dev)
{
	ether_setup(dev);/* assign some members */
	return;
}

#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
/* Litepath HW offload */
static int lpdev_xmit(struct sk_buff *skb, struct net_device* dev)
{
	consume_skb(skb);
	return NETDEV_TX_OK;
}
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
/* This function will be called from the transport layer hook to
send the packet directly to SoC*/
int __lpdev_tx(struct sk_buff *skb)
{
#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
	int ret;
	struct lpdev_priv *priv;
	struct pmac_tx_hdr pmac = {0};
	char dummy_mac_hdr[] = {0xAC, 0x9A, 0x96, 0x00, 0x00, 0x01,
				0xAC, 0x9A, 0x96, 0x00, 0x00, 0x00,
				0x00, 0x00};

	struct ethhdr *ether_hdr= NULL;
	struct sock *sk = skb->sk;

	if (!g_lpdev.dev || !sk || sk->sk_bound_dev_if || !g_lp_enabled) {
	/* If sk is bound to an interface we are not able to lookup
	the sk if the packet comes back to the stack*/
		return PPA_FAILURE;
	}

	if ((sk->sk_type == SOCK_STREAM) && !(sk->sk_state == TCP_ESTABLISHED)) {
		return PPA_FAILURE;
	}

	if (skb->len + sizeof(struct pmac_tx_hdr) > g_lpdev.dev->gso_max_size) {
		return PPA_FAILURE;
	}

	priv = netdev_priv(g_lpdev.dev);

	/*TBD: check whether the MAC headr is correct*/
	/*Insert dummy ethernet header to the packet*/
	skb_push(skb, ETH_HLEN);
	skb_reset_mac_header(skb);
	ppa_memcpy(skb->data, dummy_mac_hdr, sizeof(dummy_mac_hdr));
	ether_hdr = eth_hdr(skb);
	ether_hdr->h_proto = skb->protocol;

	/* Insert the pmac header*/
	if (g_pp_hw_version == 1 )
		pmac.tcp_chksum = 0; /* to bypass TSO driver in A step, No segmentation offload via TSO, PPv4 routing + encapsulation */
	else
		pmac.tcp_chksum = 1;

	switch (skb->protocol) {
	case htons(ETH_P_IP):
		if (ip_hdr(skb)->protocol == IPPROTO_TCP) {
			pmac.tcp_type = TCP_OVER_IPV4;
		} else if (ip_hdr(skb)->protocol == IPPROTO_UDP) {
			pmac.tcp_type = UDP_OVER_IPV4;
		}
		break;
	case htons(ETH_P_IPV6):
		if (ipv6_hdr(skb)->nexthdr == NEXTHDR_TCP) {
			pmac.tcp_type = TCP_OVER_IPV6;
		} else if (ipv6_hdr(skb)->nexthdr == NEXTHDR_UDP) {
			pmac.tcp_type = UDP_OVER_IPV6;
		}
	}
	pmac.ip_offset = ((u32)skb_network_offset(skb)) >> 1;
	pmac.tcp_h_offset = (skb_network_header_len(skb)) >> 2;

	if (skb_headroom(skb) < sizeof(struct pmac_tx_hdr)) {
                return PPA_FAILURE;
	}

	/* Copy the pmac header to the begining og skb->data*/
	memcpy((void*)((unsigned long)skb->data - sizeof(struct pmac_tx_hdr)),
				&pmac, sizeof(struct pmac_tx_hdr));

	/* Set the DMA descriptor fields */
	/* Set EP*/
	((struct dma_tx_desc_1 *)&skb->DW1)->field.ep = priv->dp_subif.gpid;
	/* set EG Flag = 1 */
	((struct dma_tx_desc_1 *)&skb->DW1)->field.redir = 1;
	/* Set pmac and pre_l2 flags*/
	((struct dma_tx_desc_1 *)&skb->DW1)->field.pmac = 1;
	((struct dma_tx_desc_1 *)&skb->DW1)->field.pre_l2 = 1;
	/* Set color = 1*/
	((struct dma_tx_desc_1 *)&skb->DW1)->field.color = 1;
	/* set subifid*/
	((struct dma_tx_desc_0 *)&skb->DW0)->field.dest_sub_if_id
						= priv->dp_subif.subif;

	/*Force checksum offload */
	skb->ip_summed = CHECKSUM_PARTIAL;
	skb->dev = g_lpdev.dev;

	/*dbg("skb=%px len:%d dw0=%x dw1=%x pmac.ipoffset=%d pmac.tcpoffset=%d\n",
		skb, skb->len, skb->DW0, skb->DW1,
		pmac.ip_offset, pmac.tcp_h_offset);
	dumpskb(skb->data, 64, 1);*/

	/* Call dp xmit with BYPASS FLOW enabled*/
	ret = dp_xmit(g_lpdev.dev, &priv->dp_subif, skb, skb->len,
		DP_TX_CAL_CHKSUM | DP_TX_BYPASS_FLOW | DP_TX_WITH_PMAC);

	if (ret) {
                dbg("Failed to dp_xmit\n");
		skb_pull(skb, ETH_HLEN);
                return PPA_FAILURE;
        }

        return PPA_SUCCESS;
#else
	return PPA_FAILURE;
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
}

int lpdev_tx(struct sk_buff *skb)
{
#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
	if (g_pp_hw_version == 1) {
		struct iphdr *iph = NULL;
		int queued = 0;
		int error = 0;
		struct sock *sk = skb->sk;

		if (!g_lpdev.dev || !sk || sk->sk_bound_dev_if || !g_lp_enabled) {
			/* If sk is bound to an interface we are not able to lookup
			   the sk if the packet comes back to the stack*/
			return PPA_FAILURE;
		}
		if ((sk->sk_type == SOCK_STREAM) && !(sk->sk_state == TCP_ESTABLISHED)) {
			return PPA_FAILURE;
		}

		iph = ip_hdr(skb);
		iph->tot_len =  htons(skb->len);
		if (skb_shinfo(skb)->gso_size) {
			struct sk_buff *segs;
			segs = skb_gso_segment(skb, 0);
			do {
				struct sk_buff *nskb = segs->next;
				if (segs) {
					if (__lpdev_tx(segs) == PPA_SUCCESS) {
						queued++;
					} else {
						error++;
						kfree_skb(segs);
					}
				}
				segs = nskb;
			} while (segs);
			if (queued && !error) {
				kfree_skb(skb);
				return PPA_SUCCESS;
			} else {
				return PPA_FAILURE;
			}
		} else {
			skb_checksum_help(skb);
			ip_send_check(iph);
			/*Call the hardware offload before lookup*/
			if (__lpdev_tx(skb) == PPA_SUCCESS) {
				return PPA_SUCCESS;
			}
		}
	} else {
		if (__lpdev_tx(skb) == PPA_SUCCESS) {
			return PPA_SUCCESS;
		}
	}
	return PPA_SUCCESS;
#else
	return PPA_FAILURE;
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
}
EXPORT_SYMBOL(lpdev_tx);

int32_t is_lpdev(struct net_device *dev)
{
#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
	if (g_lp_enabled)
		return ppa_is_netif_equal( dev, g_lpdev.dev);
	else
		return PPA_FAILURE;
#else
	return PPA_FAILURE;
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
}
EXPORT_SYMBOL(is_lpdev);

#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
extern int32_t ppa_sw_litepath_local_deliver(struct sk_buff *skb);

static struct net_device_ops lpdev_ops = {
	.ndo_start_xmit = lpdev_xmit,
};

static void lpdev_setup(struct net_device *dev)
{
        dev->features = 0;
        dev->netdev_ops = &lpdev_ops;
}

static int32_t lpdev_rx_udp_loopback(struct sk_buff *skb, int32_t len, bool is_ipv6)
{
	struct sock *sk = NULL;
	struct inet_sock *inet = NULL;
        struct dst_entry *dst = NULL;
        struct flowi6 fl6;
	struct udphdr *uh = udp_hdr(skb);

	if (!uh) {
		return PPA_FAILURE;
	}

	if (!is_ipv6) {
		/* in case of ipv4 UDP*/
		/* lookup sk*/
		sk = udp4_lib_lookup_skb(skb, uh->source, uh->dest);
		if (!sk) {
			return PPA_FAILURE;
		}

		inet = inet_sk(sk);
		if (inet == NULL ) {
			return PPA_FAILURE;
		}

		/* set skb sk */
		skb->sk = sk;
		skb->destructor = NULL;
		skb_set_hash_from_sk(skb, sk);

		/* Send to the ip layer */
		return ip_queue_xmit(sk, skb, &inet->cork.fl);
	} else {
		/* in case of ipv6*/
		/* lookup sk */
		sk = udp6_lib_lookup_skb( skb, uh->source, uh->dest);
		if (!sk) {
			return PPA_FAILURE;
		}
		/* get the flow from sk */
		fl6 = inet_sk(sk)->cork.fl.u.ip6;
		/* get the dst info */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,158)
		dst = ip6_dst_lookup_flow(sock_net(sk), sk, &fl6, NULL);
#else
		dst = ip6_dst_lookup_flow(sk, &fl6, NULL);
#endif
		if (dst ) {
			/* set skb dst*/
			skb_dst_set_noref(skb, dst);
		} else {
			/*TBD find the dst for sk */
		}

		/* set skb sk */
		skb->sk = sk;
		skb->destructor = NULL;
		skb_set_hash_from_sk(skb, sk);

		/*packet is reaching here as we hijacked the packet from
		"udp_v6_send_skb()" just before calling ip6_send_skb()
		send it back to ip layer*/
		return ip6_local_out( sock_net(sk), sk, skb);

	}
	return PPA_FAILURE;
}

static int32_t lpdev_rx_tcp_loopback(struct sk_buff *skb, int32_t len, bool is_ipv6)
{
	struct ipv6hdr *ip6h = NULL;
	struct sock *sk = NULL;
	struct inet_connection_sock *icsk = NULL;
	struct inet_sock *inet = NULL;
	struct dst_entry *dst = NULL;
	struct flowi6 fl6;
	struct tcphdr *th = tcp_hdr(skb);

	if (!th) {
		return PPA_FAILURE;
	}

	if (!is_ipv6) {
		/* TBD: skb->iif must be set to the actual egress if by the caller*/
		sk = inet_lookup_established(dev_net(g_lpdev.dev), &tcp_hashinfo,
			ip_hdr(skb)->daddr, th->dest,
			ip_hdr(skb)->saddr, th->source, skb->skb_iif);

		dbg("%s %d DST IP: %pI4 PORT:%d SRC IP: %pI4 PORT:%d\n",
		__FUNCTION__, __LINE__,
		&ip_hdr(skb)->daddr, ntohs(th->dest),
		&ip_hdr(skb)->saddr, ntohs(th->source));

		if (!sk) {
			return PPA_FAILURE;
		}

		icsk = inet_csk(sk);
		inet = inet_sk(sk);
		if (inet == NULL || icsk == NULL) {
			return PPA_FAILURE;
		}

		skb->pfmemalloc = 0;
		skb->sk = sk;
		skb->destructor = NULL;
		skb_set_hash_from_sk(skb, sk);

		skb_set_dst_pending_confirm(skb, sk->sk_dst_pending_confirm);
		skb->tstamp = 0;
		memset(skb->cb, 0, max(sizeof(struct inet_skb_parm),
			sizeof(struct inet6_skb_parm)));

		/*call the ip layer */
		return icsk->icsk_af_ops->queue_xmit(sk, skb, &inet->cork.fl);
	} else {
		ip6h = ipv6_hdr(skb);
		if (!ip6h) {
			return PPA_FAILURE;
		}
		sk =  __inet6_lookup_established(dev_net(skb->dev), &tcp_hashinfo,
					&ip6h->daddr, th->dest,
					&ip6h->saddr, th->source,
					inet6_iif(skb), inet6_sdif(skb));
		if (!sk) {
			return PPA_FAILURE;
		}

		 /* get the flow from sk */
                fl6 = inet_sk(sk)->cork.fl.u.ip6;

		skb->sk = sk;
		skb->destructor = NULL;
		skb_set_hash_from_sk(skb, sk);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,158)
		dst = ip6_dst_lookup_flow(sock_net(sk), sk, &fl6, NULL);
#else
		dst = ip6_dst_lookup_flow(sk, &fl6, NULL);
#endif
		if (!IS_ERR(dst)) {
			skb_dst_set(skb, dst);
			return ip6_xmit(sk, skb, &fl6, fl6.flowi6_mark, NULL, 0);
		}
	}

	return PPA_FAILURE;
}

static int32_t lpdev_rx_loopback(struct sk_buff *skb, int32_t len)
{
	struct iphdr *iph = NULL;
	struct ipv6hdr *ip6h = NULL;
	struct ethhdr *ether_hdr = NULL;
	uint16_t l3proto;
	uint8_t l4proto;

	/*Reset the skb MAC header pointer to skb->data i.e current position*/
	skb_reset_mac_header(skb);
	ether_hdr = eth_hdr(skb);

	/*Move skb->data pointer to the ip header */
	/*We donot expect VLAN packets here as
	while sending to the Litepath driver we dont add vlan header*/
	skb_pull(skb, ETH_HLEN);

	/*Reset network header*/
	skb_reset_network_header(skb);
	l3proto = ntohs(ether_hdr->h_proto);
	skb->ip_summed = CHECKSUM_UNNECESSARY;

	switch(l3proto) {
	case ETH_P_IP:
	/*if protocol is ipv4*/
		iph = ip_hdr(skb);
		if (!iph) {
			return PPA_FAILURE;
		}

		/*Move skb->data to the transport header*/
		skb_pull(skb, iph->ihl * 4);

		/*Reset the transport header*/
		skb_reset_transport_header(skb);

		/*Call the parsing RX hook for the skb*/
		pktprs_do_parse(skb, g_lpdev.dev, PKTPRS_ETH_RX);

		/* Set incoming device & index */
		skb->dev = g_lpdev.dev;
		skb->skb_iif = g_lpdev.dev->ifindex;
		skb->protocol = htons(ETH_P_IP);

		l4proto = iph->protocol;
		/* Call the TCP/UDP layer loopack fn*/
		if (l4proto == IPPROTO_TCP)
			return lpdev_rx_tcp_loopback(skb, len, 0);
		else if (l4proto == IPPROTO_UDP)
			return lpdev_rx_udp_loopback(skb, len, 0);
		break;
	case ETH_P_IPV6:
		ip6h = ipv6_hdr(skb);
		if (!ip6h) {
			return PPA_FAILURE;
		}

		skb->protocol = htons(ETH_P_IPV6);
		/*packets with ipv6 options header is not expected to be sent out by lpdev*/
		/*Move skb->data to the transport header*/
		skb_pull(skb, IPV6_HLEN);
		/*Reset the transport header*/
		skb_reset_transport_header(skb);

		/*Call the parsing RX hook for the skb*/
		pktprs_do_parse(skb, g_lpdev.dev, PKTPRS_ETH_RX);

		l4proto = ntohs(ip6h->nexthdr);
		if (l4proto == IPPROTO_TCP)
			return lpdev_rx_tcp_loopback(skb, len, 1);
		else if (l4proto == IPPROTO_UDP)
			return lpdev_rx_udp_loopback(skb, len, 1);
		break;
	}

	return PPA_FAILURE;
}

static int32_t lpdev_handle_rx( struct sk_buff *skb, int32_t session_id)
{
	struct dst_entry *dst = NULL;
	uint8_t l3_off = 0, l4_off = 0;
	struct net_device *netif = NULL;
	struct iphdr *iph = NULL;

	if ((session_id >= 0) && (session_id < g_max_hw_sessions)) {
		spin_lock_bh(&g_hal_db_lock);
		if (pp_hal_db[session_id].lp_rxinfo) {
			dst = pp_hal_db[session_id].lp_rxinfo->dst;
			netif = pp_hal_db[session_id].lp_rxinfo->netif;
			l3_off = pp_hal_db[session_id].lp_rxinfo->l3_offset;
			l4_off = pp_hal_db[session_id].lp_rxinfo->l4_offset;
		}
		spin_unlock_bh(&g_hal_db_lock);
	}

	if (netif) {
		skb->dev = netif;
		skb->skb_iif = netif->ifindex;
	} else {
		return PPA_FAILURE;
	}

	/*Set acceleration disabled so that this skb will not hit PPA learning again*/
#if IS_ENABLED(CONFIG_NETFILTER_XT_EXTMARK)
	skb_extmark_set_hook(skb, ACCELSEL_MASK);
#else
	skb->mark &= ACCELSEL_MASK;
#endif /*IS_ENABLED(CONFIG_NETFILTER_XT_EXTMARK)*/

	skb->protocol = eth_type_trans(skb, netif);
	skb->ip_summed = CHECKSUM_UNNECESSARY;

	if ( (l3_off != 0) && (l4_off != 0)) {

		skb_set_network_header(skb, 0);
		iph = (struct iphdr *)skb_network_header(skb);

		/*Set packet length and packet type = PACKET_HOST*/
		skb->len = ntohs(iph->tot_len);
		skb->pkt_type = PACKET_HOST;

		/* Remove any debris in the socket control block */
		memset(IPCB(skb), 0, sizeof(struct inet_skb_parm));
		IPCB(skb)->iif = skb->skb_iif;

		/*Set skb dst*/
		if (dst && !(dst->flags & DST_METADATA)) {
			dst_hold_safe(dst);
			skb_dst_set(skb, dst);
		} else {
			return netif_rx(skb);
		}

	/*FIXME: We are now calling ip layer to process the packet.
	Although we are calling ip layer the routing will be skipped as we already set dst
	This needs to be removed after stabilizing the below code*/
	return netif_rx(skb);

		/*Set transport header */
		skb_set_transport_header(skb, (l4_off - l3_off));
		/*set the skb->data point to the transport header*/
		skb_pull(skb, skb_network_header_len(skb));

		/*Call local deliver*/
		return ppa_sw_litepath_local_deliver(skb);
	}
	return PPA_FAILURE;
}

/* Calback invoked by dp when packets are received on g_litepath_gpid*/
int32_t lp_dp_rx_handler(struct net_device *rxif, struct net_device *txif,
        struct sk_buff *skb, int32_t len)
{
	int ret;
	struct pp_desc *ppdesc = NULL;
	int32_t sess_id = -1;

	/*1. Read the ud parameters from the descriptor*/
	ppdesc = pp_pkt_desc_get(skb);
	if (ppdesc) {
		/* If the packet is an exception packet */
		/* It is coming from the tx path as there is no HW session match */
		/* We need to insert this packet back to the stack for learning */
		if ( ppdesc->ud.is_exc_sess) {
			ret = lpdev_rx_loopback(skb, len);
			if (ret == PPA_FAILURE) {
				consume_skb(skb);
			}
		} else {
		/* Session match exists*/
		/* This is an rx path packet to be terminated in DUT */
		/* Extract the session id */
			sess_id = ppdesc->ud.sess_id;

			dbg("skb =%px, len=%d truesize=%d session_id=%d\n",
				skb, skb->len, skb->truesize, sess_id);
			dumpskb(skb->data,64,1);

			if (sess_id >= 0) {
			/* Get the sock info from the db*/
			/* Move the skb pointers to end of ip layer */
			/* Pass the packet to registred handler (tcp/udp receive) */
				if (lpdev_handle_rx(skb, sess_id) != PPA_SUCCESS) {
					consume_skb(skb);
				}
			}
		}
	} else {
		consume_skb(skb);
	}

	return PPA_SUCCESS;
}

static dp_cb_t lp_dp_cb = {
	.rx_fn = lp_dp_rx_handler,
};

/* Application litepath offload */
static int32_t init_app_lp(void)
{
	int32_t ret = PPA_SUCCESS;
	struct dp_spl_cfg dp_con = {0};
#if IS_ENABLED(CONFIG_LGM_TOE)
	struct dp_spl_cfg lro_dp_con = {0};
	struct lro_ops *ops = NULL;
	struct dp_queue_map_set q_map = {0};
#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/
	struct dp_qos_q_logic q_logic = {0};
	struct lpdev_priv *priv = NULL;
	struct pp_version ver;

	dbg("%s %d\n",__FUNCTION__,__LINE__);

	ppa_memset(&g_lpdev, 0, sizeof(g_lpdev));
	/*Allocate netdevice */
	g_lpdev.dev = alloc_netdev(sizeof(struct lpdev_priv),
			"lpdev0", NET_NAME_UNKNOWN, lpdev_setup);
	if (!g_lpdev.dev) {
		pr_err("alloc_netdev failed for lpdev0\n");
		return PPA_FAILURE;
	}

	priv = netdev_priv(g_lpdev.dev);
	if (!priv) {
		pr_err("priv is NULL\n");
		return PPA_FAILURE;
	}
	priv->owner = THIS_MODULE;

	/*Register netdevice*/
	if (register_netdev(g_lpdev.dev)) {
		free_netdev(g_lpdev.dev);
		g_lpdev.dev = NULL;
		pr_err("register device \"lpdev0\" failed\n");
		return PPA_FAILURE;
	}

	/*call the dp to allocate special connection */
	/*******************************************************/
	dp_con.flag = 0;/*DP_F_REGISTER;*/
	dp_con.type = DP_SPL_APP_LITEPATH;
	dp_con.f_subif = 1;
	dp_con.f_gpid = 1;
	dp_con.f_hostif = 1;

	/*assign the netdevice */
	dp_con.dev = g_lpdev.dev;

	/*callback to be invoked by dp when packet is received for this GPID*/
	dp_con.dp_cb = &lp_dp_cb;

	if ((ret = dp_spl_conn(0, &dp_con))) {
		pr_err("Regsiter spl conn for mc failed\n");
		return PPA_FAILURE;
	}

	/*Store the gpid and uc_id*/
	g_lpdev.uc_id = dp_con.spl_id;
	g_lpdev.gpid = dp_con.gpid;
	g_lpdev.subif = dp_con.subif;

	priv->dp_subif.port_id = dp_con.dp_port;
	priv->dp_subif.subif = dp_con.subif;
	priv->dp_subif.gpid = dp_con.gpid;

	/* Enable dp_rx*/
	if ((ret = dp_rx_enable(g_lpdev.dev, g_lpdev.dev->name, 1))) {
		pr_err("Enable rx_fn for litepath failed\n");
		return PPA_FAILURE;
	}

	/*******************************************************/
	dbg("%s %d dp_spl_conn success dp_con.gpid=%d, dp_con.subif=%d dp_con.spl_id=%d, dp_con.egp[0].qid=%d, dp_con.egp[1].qid=%d\n",
		__FUNCTION__, __LINE__,dp_con.gpid, dp_con.subif, dp_con.spl_id, dp_con.egp[0].qid, dp_con.egp[1].qid);

#if IS_ENABLED(CONFIG_LGM_TOE)
	/* Setup Queue map table to lpdev_tx */
	q_map.map.dp_port = 0; 			/* CPU portid*/
	q_map.map.subif = dp_con.subif;		/* Subif id */

	/* If LRO is enabled we need to setup the queue map table*/
	dp_spl_conn_get(0, DP_SPL_TOE, &lro_dp_con, 1);

	/* Verify that the toe driver succeeds to load */
	if (lro_dp_con.igp && lro_dp_con.igp->egp) {
		q_map.q_id = lro_dp_con.igp->egp->qid;

		q_map.mask.class = 1;			/* TC dont care*/
		q_map.mask.flowid = 1;			/* Flowid dont care*/
		q_map.mask.egflag = 1;			/* Flowid dont care*/

		if (dp_queue_map_set(&q_map, 0) == DP_FAILURE)
			dbg("dp_queue_map_set failed for Queue [%d]\n",
			    dp_con.egp[1].qid);

		ops = dp_get_lro_ops();
		if (ops)
			netif_set_gso_max_size(g_lpdev.dev,
					       ops->get_gso_max_size(ops->toe));
	} else
		pr_err("TOE driver is not ready\n");
#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/

	/*Egress port qid in this case CPU queueid*/;
	q_logic.q_id = get_cpu_qid();

	/* physical to logical qid */
	if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
		pr_err("%s:%d ERROR Failed to Logical Queue Id\n", __func__, __LINE__);
		return PPA_FAILURE;
	}

	/*Store the logical qid */
	g_lpdev.qid = q_logic.q_logic_id;

	if (unlikely(pp_version_get(&ver, PP_VER_TYPE_HW))) {
		pr_err("Failed to get PP version\n");
		return PPA_FAILURE;
	} else {
		g_pp_hw_version = ver.major;
	}

	return ret;
}
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/

/*Calback invoked by dp when packets are received on g_mc_gpid */
int32_t mc_dp_rx_handler(struct net_device *rxif, struct net_device *txif,
	struct sk_buff *skb, int32_t len)
{
	struct pp_desc *ppdesc = NULL;
	int16_t groupid = -1, dev_idx = -1;
	PPA_NETIF *tx_netif = NULL;

	skb_reset_mac_header(skb);
	skb_set_network_header(skb, ETH_HLEN);

	/*1. Call the parsing RX hook for the skb*/
	pktprs_do_parse(skb, NULL, PKTPRS_ETH_RX);

	/*2. Read the ud parameters from the descriptor*/
	/* Based on the mc groupid and dev_idx we need to find the eg_netdev from the mc_db*/
	ppdesc = pp_pkt_desc_get(skb);
	if (ppdesc) {
		groupid = ppdesc->ps & MC_GRP_MASK; /*BITS: [8:0] */
		dev_idx = (ppdesc->ps & MC_DST_MASK) >> 18; /* BITS: [18:21] */
	}

	spin_lock_bh(&g_hal_mc_db_lock);
	if ( mc_db[groupid].used && ((groupid > 0) && (groupid < MAX_MC_GROUP_ENTRIES))
		&& ((dev_idx >= 0) && (dev_idx < MAX_MC_CLIENT_PER_GRP))) {
			tx_netif = mc_db[groupid].dst[dev_idx].netdev;
	}
	spin_unlock_bh(&g_hal_mc_db_lock);

	/*3. set skb->dev as the eg_netdev and call dev_queue_xmit*/
	/* parsing driver's TX hook will be invoked automatically*/
	if (tx_netif) {
		skb->dev = tx_netif;
		skb->pkt_type = PACKET_OUTGOING;
		dev_queue_xmit(skb);
	} else {
		consume_skb(skb);
	}

	return PPA_SUCCESS;
}

static dp_cb_t mc_dp_cb = {
	.rx_fn = mc_dp_rx_handler,
};

static inline int32_t init_mc_nf(void)
{
	int32_t ret = PPA_SUCCESS;
	struct pp_nf_info nf_info = {0};
	struct dp_spl_cfg dp_con = {0};
	struct dp_qos_q_logic q_logic = {0};
	struct pp_port_cfg pcfg = {0};
	struct mc_dev_priv *priv = NULL;

	dbg("%s %d\n",__FUNCTION__,__LINE__);

	ppa_memset(&g_mcast_nf, 0, sizeof(g_mcast_nf));
	/*Allocate netdevice */

	g_mcast_nf.dev = alloc_netdev(sizeof(struct mc_dev_priv),
				"mcdev0", NET_NAME_UNKNOWN, mcdev_setup);
	if (!g_mcast_nf.dev) {
		dbg("alloc_netdev failed for mcdev0\n");
		return PPA_FAILURE;
	}

	priv = netdev_priv(g_mcast_nf.dev);
	priv->owner = THIS_MODULE;

	g_mcast_nf.dev->netdev_ops = &mcdev_ops;
	/*Register netdevice*/
	if (register_netdev(g_mcast_nf.dev)) {
		free_netdev(g_mcast_nf.dev);
		g_mcast_nf.dev = NULL;
		dbg("register device \"mcdev0\" failed\n");
		return PPA_FAILURE;
	}
	/*call the dp to allocate special connection */
	/*******************************************************/

	/*dp connection for multicast uC */
	dp_con.flag = 0;/*DP_F_REGISTER;*/
	dp_con.type = DP_SPL_PP_NF;
	dp_con.f_subif = 1;
	dp_con.f_gpid = 1;

	/*assign the netdevice */
	dp_con.dev = g_mcast_nf.dev;

	/*callback to be invoked by dp when packet is received for this GPID*/
	dp_con.dp_cb = &mc_dp_cb;

	if ((ret = dp_spl_conn(0, &dp_con))) {
		dbg("Regsiter spl conn for mc failed\n");
		return PPA_FAILURE;
	}

	/*enable rp_rx*/
	if ((ret = dp_rx_enable(g_mcast_nf.dev, g_mcast_nf.dev->name, 1))) {
		dbg("Enable rx_fn for mc failed\n");
		return PPA_FAILURE;
	}

	/*******************************************************/
	dbg("%s %d dp_spl_conn success dp_con.gpid=%d, dp_con.subif=%d dp_con.spl_id=%d, dp_con.egp[0].qid=%d, dp_con.egp[1].qid=%d\n",
		__FUNCTION__, __LINE__,dp_con.gpid, dp_con.subif, dp_con.spl_id, dp_con.egp[0].qid, dp_con.egp[1].qid);

	/*Store the gpid and uc_id*/
	g_mcast_nf.uc_id = dp_con.spl_id;
	g_mcast_nf.gpid = dp_con.gpid;
	g_mcast_nf.subif = (dp_con.subif >> CPU_PORT_WLAN_BIT_MODE) & 0X0F; /*remove 9 bits of mc groupid*/

	/*Get the port settings and change the ingress classification parameters*/
	if ((ret=pp_port_get(g_mcast_nf.gpid, &pcfg))) {
		dbg("pp_port_get failed in %s %d\n", __FUNCTION__, __LINE__);
		return PPA_FAILURE;
	}

	/*Fields to copy from STW*/
	pcfg.rx.cls.n_flds = 2;
	pcfg.rx.cls.cp[0].stw_off = 0;/* multicast group index in ps0 form bit 0*/
	pcfg.rx.cls.cp[0].copy_size = 9;/* 9 bits field */
	pcfg.rx.cls.cp[1].stw_off = 18;/* multicast dst bitmap in ps0 from bit 18 */
	pcfg.rx.cls.cp[1].copy_size = 4;/* 4 bits field */

	/*Set the modified port configuration */
	if ((ret=pp_port_update(g_mcast_nf.gpid, &pcfg))) {
		dbg("pp_port_update failed in %s %d\n", __FUNCTION__, __LINE__);
		return PPA_FAILURE;
	}

	/*Egress port qid*/;
	q_logic.q_id = dp_con.egp[0].qid;

	/* physical to logical qid */
	if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
		dbg("%s:%d ERROR Failed to Logical Queue Id\n", __func__, __LINE__);
		return PPA_FAILURE;
	}

	/*Store the logical qid */
	g_mcast_nf.qid = q_logic.q_logic_id;

	nf_info.cycl2_q = get_gswip_qid(&dp_con);
	nf_info.pid = g_mcast_nf.gpid;
	nf_info.q = g_mcast_nf.qid;

	dbg("%s %d calling pp_nf_set gpid=%d qid=%d cycl2_qid=%d\n", __FUNCTION__, __LINE__, nf_info.pid, nf_info.q, nf_info.cycl2_q);

	/*Setup the uC path */
	if ((ret = pp_nf_set(PP_NF_MULTICAST, &nf_info))) {
		dbg("pp_nf_set failed for PP_NF_MULTICAST\n");
	}

	dbg("%s %d g_mcast_nf.gpid=%d, dp_con.subif=%d, dp_con.spl_id=%d, g_mcast_nf.qid=%d, g_gswip_qid=%d\n",
		__FUNCTION__, __LINE__, g_mcast_nf.gpid, dp_con.subif, dp_con.spl_id, g_mcast_nf.qid, g_gswip_qid);

	return ret;
}

static int init_frag_nf(void)
{
	int32_t ret = PPA_SUCCESS;
	struct pp_nf_info nf_info = {0};
	struct dp_spl_cfg dp_con = {0};
	struct dp_qos_q_logic q_logic = {0};

	/*TBD: remove next line when fragmentor in implemented in pp*/
	ppa_memset(&g_frag_nf, 0, sizeof(g_frag_nf));

	/*dp connection for fragmenter uC */
	dp_con.flag = 0;/*DP_F_REGISTER;*/
	dp_con.type = DP_SPL_PP_NF;
	dp_con.f_subif = 1;
	dp_con.f_gpid = 0;

	if ((ret = dp_spl_conn(0, &dp_con))) {
		dbg("Regsiter spl conn for fragmenter failed\n");
		return PPA_FAILURE;
	}
	g_frag_nf.uc_id = dp_con.spl_id;

	dbg("%s %d dp_spl_conn success dp_con.gpid=%d, dp_con.spl_id=%d, dp_con.egp[0].qid=%d, dp_con.igp[0].egp.qid=%d\n",
		__FUNCTION__, __LINE__,dp_con.gpid, dp_con.spl_id, dp_con.egp[0].qid, dp_con.egp[1].qid);
	/*******************************************************/

	g_frag_nf.gpid = dp_con.gpid;

	/*Egress port qid*/
	q_logic.q_id = dp_con.egp[0].qid;

	/* physical to logical qid */
	if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
		dbg("%s:%d ERROR Failed to Logical Queue Id\n", __func__, __LINE__);
		return PPA_FAILURE;
	}

	nf_info.pid = g_frag_nf.gpid;
	nf_info.q = g_frag_nf.qid = q_logic.q_logic_id;

	nf_info.cycl2_q = get_gswip_qid(&dp_con);
	dbg("%s %d calling pp_nf_set gpid=%d qid=%d cycl2_qid=%d\n", __FUNCTION__, __LINE__, nf_info.pid, nf_info.q, nf_info.cycl2_q);

	/*Setup the uC path */
	if ((ret = pp_nf_set(PP_NF_FRAGMENTER, &nf_info))) {
		dbg("pp_nf_set failed for PP_NF_FRAGMENTER\n");
	}

	ppa_memset(&g_reas_nf, 0, sizeof(g_reas_nf));
	ppa_memset(&dp_con, 0, sizeof(dp_con));
	ppa_memset(&q_logic, 0, sizeof(q_logic));
	ppa_memset(&nf_info, 0, sizeof(nf_info));

	/*dp connection for reassembler uC */
	dp_con.flag = 0;/*DP_F_REGISTER;*/
	dp_con.type = DP_SPL_PP_NF;
	dp_con.f_subif = 1;
	dp_con.f_gpid = 0;

	if ((ret = dp_spl_conn(0, &dp_con))) {
		dbg("Regsiter spl conn for reassmbler failed\n");
		return PPA_FAILURE;
	}
	g_reas_nf.uc_id = dp_con.spl_id;

	dbg("%s %d dp_spl_conn success dp_con.gpid=%d, dp_con.spl_id=%d, dp_con.egp[0].qid=%d, dp_con.igp[0].egp.qid=%d\n",
		__FUNCTION__, __LINE__,dp_con.gpid, dp_con.spl_id, dp_con.egp[0].qid, dp_con.egp[1].qid);
	/*******************************************************/

	g_reas_nf.gpid = dp_con.gpid;

	/*Egress port qid*/
	q_logic.q_id = dp_con.egp[0].qid;

	/* physical to logical qid */
	if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
		dbg("%s:%d ERROR Failed to Logical Queue Id\n", __func__, __LINE__);
		return PPA_FAILURE;
	}

	nf_info.pid = g_reas_nf.gpid;
	nf_info.q = g_reas_nf.qid = q_logic.q_logic_id;

	nf_info.cycl2_q = get_gswip_qid(&dp_con);
	dbg("%s %d calling pp_nf_set gpid=%d qid=%d cycl2_qid=%d\n", __FUNCTION__, __LINE__, nf_info.pid, nf_info.q, nf_info.cycl2_q);

	/*Setup the uC path */
	if ((ret = pp_nf_set(PP_NF_REASSEMBLY, &nf_info))) {
		dbg("pp_nf_set failed for PP_NF_REASSEMBLY\n");
	}

	return ret;
}

#if IS_ENABLED(CONFIG_INTEL_VPN)
static inline int32_t ppa_vpn_parse_rx(struct sk_buff *skb, struct net_device *dev, uint16_t tunnel_id)
{
	PP_IPSEC_TUN_NODE *tunn_info;
	int32_t ret = PPA_SUCCESS;
	struct iphdr *iph = NULL;
	struct ipv6hdr *ip6h = NULL;
	struct pppoe_hdr *ppphdr = NULL;
	struct pktprs_hdr *pktprs_hdr;
	uint16_t offset = 0;
	uint16_t totlen = 0;
	uint8_t ip_offset = 0;

	dbg("received skb====> dev=%s",dev?dev->name:"null");
	dumpskb(skb->data, 64, 1);

	spin_lock_bh(&g_tun_db_lock);
	tunn_info = &ipsec_tun_db[tunnel_id];
	if (!list_empty(&tunn_info->sessions) && tunn_info->is_inbound) {
		skb_reset_mac_header(skb);
		skb->dev = dev;

		/* Call learning ingress */
		pktprs_do_parse(skb, dev, PKTPRS_ETH_RX);
		pktprs_hdr = pktprs_skb_hdr_get(skb);
		if (!pktprs_hdr) {
			pr_err("Failed to get RX pktprs header\n");
			ret = PPA_FAILURE;
			goto exit;
		}

		if (PKTPRS_IS_ESP(pktprs_hdr, PKTPRS_HDR_LEVEL0))
			goto exit;

		/* Get the ingress ip length */
		if (PKTPRS_IS_IPV6(pktprs_hdr, PKTPRS_HDR_LEVEL0)) {
			ip6h = pktprs_ipv6_hdr(pktprs_hdr, PKTPRS_HDR_LEVEL0);
			totlen = ntohs(ip6h->payload_len);
			ip_offset = pktprs_hdr_off(pktprs_hdr,
						   PKTPRS_PROTO_IPV6,
						   PKTPRS_HDR_LEVEL0);
		} else if (PKTPRS_IS_IPV4(pktprs_hdr, PKTPRS_HDR_LEVEL0)) {
			iph = pktprs_ipv4_hdr(pktprs_hdr, PKTPRS_HDR_LEVEL0);
			totlen = ntohs(iph->tot_len);
			ip_offset = pktprs_hdr_off(pktprs_hdr,
						   PKTPRS_PROTO_IPV4,
						   PKTPRS_HDR_LEVEL0);
		} else {
			pr_err("%s:%u: Invalid IP Layer\n", __func__, __LINE__);
			ret = PPA_FAILURE;
			goto exit;
		}

		/*2. Insert the original ESP header */
		/* The skb->data is currently pointing to the begining of Ethernet header
		Set the skb->data to the point where the ESP header can be copied */

		offset = tunn_info->hdr_len - ip_offset + tunn_info->iv_sz;
		if (tunn_info->trns_mode) {
			/* In case of transport mode payload starts immidiately after ESP header*/
			/* IP header is before the ESP header (already present in the backed up header)*/
			if (PKTPRS_IS_IPV6(pktprs_hdr, PKTPRS_HDR_LEVEL0))
				offset -= IPV6_HLEN;
			else
				offset -= IPV4_HLEN;
		}

		/*Skb has enough headroom */
		if (offset > 0) {
			if (skb_headroom(skb) > offset) {
				skb_push(skb, offset);
			} else {
				pr_err("%s:%d: Not enough headroom %d!!\n",
				       __FUNCTION__,__LINE__, offset);
				ret = PPA_FAILURE;
				goto exit;
			}
		}

		/*Copy the stored ESP header */
		ppa_memcpy(skb->data, tunn_info->hdr, tunn_info->hdr_len);
		/*Set the IV field to 0*/
		ppa_memset(skb->data + tunn_info->hdr_len, 0, tunn_info->iv_sz);

		skb_set_mac_header(skb, 0);
		skb_set_network_header(skb, tunn_info->ip_offset);

		if (tunn_info->ipv6) {
			ip6h = (struct ipv6hdr *)skb_network_header(skb);
			totlen += tunn_info->esp_hdr_len + tunn_info->iv_sz;
			if (!tunn_info->trns_mode)
				totlen += IPV6_HLEN;
			ip6h->payload_len = htons(totlen);
		} else {
			iph = (struct iphdr *)skb_network_header(skb);
			totlen += tunn_info->esp_hdr_len + tunn_info->iv_sz;
			if (!tunn_info->trns_mode)
				totlen += IPV4_HLEN;
			iph->tot_len = htons(totlen);

			/* calculate header checksum*/
			iph->check = 0;
			iph->check = ip_fast_csum((const void *)iph, iph->ihl);
		}

		if (tunn_info->pppoe) {
			/* update ppp header with length of the ppp payload */
			ppphdr = (struct pppoe_hdr *)(skb->data + (tunn_info->ip_offset - PPPOE_HLEN));
			/* add the outer ipv6 header length as it is not
			 * included in the totlen
			 */
			if (tunn_info->ipv6)
				totlen += IPV6_HLEN;
			/* ip payload length + ppp header length */
			ppphdr->length = htons(totlen + 2);
		}

		/* Set flags in the skb to make sure that stack understand
		 * the packet is already decrypted
		 */
		secpath_set(skb);
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	}

exit:
	spin_unlock_bh(&g_tun_db_lock);
	return ret;
}

static inline int32_t ppa_vpn_tunn_del(uint16_t tunnel_id)
{
	struct uc_session_node *p_item, *p;

	/*check if the tunnel table entry is free */
	spin_lock_bh(&g_tun_db_lock);
	/* delete all related sessions */
	list_for_each_entry_safe(p_item, p, &ipsec_tun_db[tunnel_id].sessions, tun_node) {
		list_del_init(&p_item->tun_node);
		del_routing_session_cb(p_item);
	}
	INIT_LIST_HEAD(&ipsec_tun_db[tunnel_id].sessions);
	spin_unlock_bh(&g_tun_db_lock);

	return PPA_SUCCESS;
}

static int init_vpn_offload(void)
{
	struct pp_nf_info nf_info = {0};
	struct dp_spl_cfg dp_con = {0};
	struct dp_spl_cfg vpn_con = {0};
	struct dp_qos_q_logic q_logic = {0};
	int i, ret = PPA_SUCCESS;
	struct mc_dev_priv *priv = NULL;
	struct dp_queue_map_set q_map = {0};

	struct intel_vpn_ops *vpn_ops = dp_get_vpn_ops(0);;

	if (vpn_ops) {
		ret = PPA_SUCCESS;
	} else{
		return PPA_FAILURE;
	}

	/* initialize the VPN assist */
	ppa_memset(&g_vpn_nf, 0, sizeof(g_vpn_nf));

	/*Allocate netdevice */
	/*using lpdev structures as this is a dummy net device */
	g_vpn_nf.dev = alloc_netdev(sizeof(struct mc_dev_priv),
			"vpnnf_dev0", NET_NAME_UNKNOWN, mcdev_setup);
	if (!g_vpn_nf.dev) {
		dbg("alloc_netdev failed for vpnnf_dev0\n");
		return PPA_FAILURE;
	}

	priv = netdev_priv(g_vpn_nf.dev);
	priv->owner = THIS_MODULE;

	g_vpn_nf.dev->netdev_ops = &mcdev_ops;
	/*Register netdevice*/
	if (register_netdev(g_vpn_nf.dev)) {
		free_netdev(g_vpn_nf.dev);
		g_vpn_nf.dev = NULL;
		dbg("register device \"vpnnf_dev0\" failed\n");
		return PPA_FAILURE;
	}

	/*Get the VPNA con parameters*/
	dp_spl_conn_get(0, DP_SPL_VPNA, &vpn_con, 1);
	g_vpna_conn.dev = vpn_con.dev;

	/*assign the netdevice */
	dp_con.dev = g_vpn_nf.dev;

	/*callback to be invoked by dp when packet is received for this GPID*/
	/*register the callback registered by the VPN adaptor driver */
	dp_con.dp_cb = vpn_con.dp_cb;

	/*dp connection for vpn assist uC */
	dp_con.flag = 0;/*DP_F_REGISTER;*/
	dp_con.type = DP_SPL_PP_NF;
	dp_con.f_subif = 1;
	dp_con.f_gpid = 1;

	if ((ret = dp_spl_conn(0, &dp_con))) {
		dbg("Regsiter spl conn for mc failed\n");
		return PPA_FAILURE;
	}
	g_vpn_nf.uc_id = dp_con.spl_id;

	dbg("%s %d dp_spl_conn success dp_con.gpid=%d, dp_con.spl_id=%d, dp_con.egp[0].qid=%d, dp_con.igp[0].egp.qid=%d\n",
		__FUNCTION__, __LINE__,dp_con.gpid, dp_con.spl_id, dp_con.egp[0].qid, dp_con.egp[1].qid);
	/*******************************************************/

	g_vpn_nf.gpid = dp_con.gpid;

	/* Setup Queue map table for vpn assist tx */
	q_map.q_id =  dp_con.egp[1].qid; 	/* Gswip Qid */
	q_map.map.dp_port = 0; 			/*CPU portid*/
	q_map.map.subif = dp_con.subif;		/*Subif id */
	q_map.mask.flowid = 1;			/* Flowid dont care*/
	q_map.mask.egflag = 1;			/* Egress flag dont care*/

	if (dp_queue_map_set(&q_map, 0) == DP_FAILURE) {
		dbg("dp_queue_map_set failed for Queue [%d]\n", dp_con.egp[1].qid);
        }

	/*Egress port qid*/
	q_logic.q_id = dp_con.egp[0].qid;

	/* physical to logical qid */
	if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
		dbg("%s:%d ERROR Failed to Logical Queue Id\n", __func__, __LINE__);
		return PPA_FAILURE;
	}

	nf_info.pid = g_vpn_nf.gpid;
	nf_info.q = g_vpn_nf.qid = q_logic.q_logic_id;

	nf_info.cycl2_q = get_gswip_qid(&dp_con);
	dbg("%s %d calling pp_nf_set gpid=%d qid=%d cycl2_qid=%d\n", __FUNCTION__, __LINE__, nf_info.pid, nf_info.q, nf_info.cycl2_q);

	/*Setup the uC path */
	if ((ret = pp_nf_set(PP_NF_IPSEC, &nf_info))) {
		dbg("pp_nf_set failed for PP_NF_FRAGMENTER\n");
	}

	/*initalize the tunnel_db*/
	spin_lock_init(&g_tun_db_lock);
	ppa_memset(&ipsec_tun_db, 0, sizeof(ipsec_tun_db));

	ppa_vpn_ig_lrn_hook   = ppa_vpn_parse_rx;
	ppa_vpn_tunn_del_hook = ppa_vpn_tunn_del;

	for (i = 0; i < ARRAY_SIZE(ipsec_tun_db); i++)
		INIT_LIST_HEAD(&ipsec_tun_db[i].sessions);

	return ret;
}

static inline void uninit_vpn_offload(void)
{
	struct dp_spl_cfg dp_con={0};
	dp_con.flag = DP_F_DEREGISTER;
	dp_con.type = DP_SPL_PP_NF;
	dp_con.spl_id = g_vpn_nf.uc_id;

	if (dp_spl_conn(0, &dp_con)) {
		dbg("Deregister of dp spl conn for vpn failed\n");
	}

	ppa_vpn_ig_lrn_hook   = NULL;
	ppa_vpn_tunn_del_hook = NULL;
}
#endif /*IS_ENABLED(CONFIG_INTEL_VPN)*/

static inline void uninit_mc_nf(void)
{
	/*******************************************************/
	struct dp_spl_cfg dp_con={0};
	dp_con.flag = DP_F_DEREGISTER;
	dp_con.type = DP_SPL_PP_NF;
	dp_con.spl_id = g_mcast_nf.uc_id;

	if (dp_spl_conn(0, &dp_con)) {
		dbg("Deregister of dp spl conn for mc failed\n");
	}
	/*******************************************************/
}

static inline void uninit_frag_nf(void)
{
	/*******************************************************/
	struct dp_spl_cfg dp_con={0};
	dp_con.flag = DP_F_DEREGISTER;
	dp_con.type = DP_SPL_PP_NF;
	dp_con.spl_id = g_frag_nf.uc_id;

	/*Workaround to disable the fragmenter and reassembler
	until the performance issue is fixed*/
	return;

	if (dp_spl_conn(0, &dp_con)) {
		dbg("Deregister of dp spl conn for fragmenter failed\n");
	}

	dp_con.spl_id = g_reas_nf.uc_id;

	if (dp_spl_conn(0, &dp_con)) {
		dbg("Deregister of dp spl conn for reassembly failed\n");
	}
/*******************************************************/
}

#if IS_ENABLED(CONFIG_INTEL_VPN)
static inline int32_t is_vpn_assist(struct pktprs_desc *desc,
			struct intel_vpn_tunnel_info *tun_info)
{

	if (tun_info->mode == VPN_MODE_TRANSPORT) {
		return 1;
	}else {
	/*tunnel mode needs vpn assist only in case of eth type update*/
	/*it is can be handled by VPN session itself*/
		return 0;
	}
}

/**
 * @brief adding the ipsec tunnel to the local database
 *        1. update the ref count if entry already exist
 *        2. free the old header if exist
 *        3. reset the entry
 *        4. increment the ref count
 *        5. save the related p_item
 * @param p_item session ndoe
 */
static void ipsec_tunn_db_add(struct uc_session_node *p_item)
{

	/* if the tunnel already created, do nothing */
	if (!list_empty(&ipsec_tun_db[p_item->tunnel_idx].sessions))
		return;

	/*if this index was in use earlier free it*/
	if (ipsec_tun_db[p_item->tunnel_idx].hdr)
		ppa_free(ipsec_tun_db[p_item->tunnel_idx].hdr);

	ppa_memset(&ipsec_tun_db[p_item->tunnel_idx], 0,
		   sizeof(PP_IPSEC_TUN_NODE));

	INIT_LIST_HEAD(&ipsec_tun_db[p_item->tunnel_idx].sessions);
}

static void store_esp_tunnel_header(struct uc_session_node *p_item,
				    struct pktprs_desc *desc,
				    struct intel_vpn_tunnel_info *tunn_info)
{
	PP_IPSEC_TUN_NODE *db_tinfo; /* db tunnel info */
	char *ptr = NULL;
	uint16_t tunnel_id = p_item->tunnel_idx;
	struct iphdr *ipv4;
	struct ipv6hdr *ipv6;
	struct ipv6_opt_hdr *ehdr;
	uint8_t nexthdr;
	uint8_t *last_nexthdr_ptr;
	uint16_t off;

	db_tinfo = &ipsec_tun_db[tunnel_id];

	db_tinfo->is_inbound = true;
	/* Update header len */
	db_tinfo->hdr_len =
		pktprs_hdr_sz(desc->rx, PKTPRS_PROTO_ESP, PKTPRS_HDR_LEVEL0);
	db_tinfo->iv_sz = tunn_info->iv_sz;
	/* Save ESP header len */
	db_tinfo->esp_hdr_len = db_tinfo->hdr_len -
				pktprs_hdr_off(desc->rx,
					       PKTPRS_PROTO_ESP,
					       PKTPRS_HDR_LEVEL0);

	/* Allocate memory for the tunnel header */
	db_tinfo->hdr = ppa_malloc(db_tinfo->hdr_len);
	if (!db_tinfo->hdr) {
		dbg("Header allocation failed!!");
		return;
	}

	if (tunn_info->mode == VPN_MODE_TRANSPORT)
		db_tinfo->trns_mode = 1;

	/*Copy the ingress tunnel header to the buffer*/
	ptr = (char *)db_tinfo->hdr;
	ppa_memcpy(ptr, desc->rx->buf, db_tinfo->hdr_len);

	/* fix the IP header and remove fragment info if exist */
	if (PKTPRS_IS_IPV4(desc->rx, PKTPRS_HDR_LEVEL0)) {
		db_tinfo->ip_offset = pktprs_hdr_off(desc->rx,
							 PKTPRS_PROTO_IPV4,
							 PKTPRS_HDR_LEVEL0);
		ipv4 = (struct iphdr *)&ptr[db_tinfo->ip_offset];

		/*
		 * - Reset ipv4 totlen and checksum - will be updated
		 *   in dp.
		 * - Remove the frag offset indication since we want to apply
		 *   this information on the reassembled packet
		 */
		ipv4->tot_len = 0;
		ipv4->frag_off &= htons(IP_DF);
		ipv4->check = 0;
	} else if (PKTPRS_IS_IPV6(desc->rx, PKTPRS_HDR_LEVEL0)) {
		db_tinfo->ipv6 = true;
		db_tinfo->ip_offset = pktprs_hdr_off(desc->rx,
						     PKTPRS_PROTO_IPV6,
						     PKTPRS_HDR_LEVEL0);
		ipv6 = (struct ipv6hdr *)&ptr[db_tinfo->ip_offset];

		/* Reset ipv6 payload len - will be updated in dp */
		ipv6->payload_len = 0;

		if (!PKTPRS_IS_FRAG_OPT(desc->rx, PKTPRS_HDR_LEVEL0))
			goto check_pppoe;

		/* Remove the frag extention header */
		db_tinfo->hdr_len -= sizeof(struct frag_hdr);
		last_nexthdr_ptr = &ipv6->nexthdr;
		nexthdr = *last_nexthdr_ptr;
		off = db_tinfo->ip_offset + sizeof(struct ipv6hdr);

		while (ipv6_ext_hdr(nexthdr) && nexthdr != NEXTHDR_NONE) {
			ehdr = (struct ipv6_opt_hdr *)(ptr + off);
			if (nexthdr == NEXTHDR_FRAGMENT) {
				*last_nexthdr_ptr = ehdr->nexthdr;
				memmove(ehdr,
					ptr + off + sizeof(struct frag_hdr),
					db_tinfo->hdr_len - off);
				break;
			} else if (nexthdr == NEXTHDR_AUTH) {
				off += ipv6_authlen(ehdr);
			} else {
				off += ipv6_optlen(ehdr);
			}
			last_nexthdr_ptr = &ehdr->nexthdr;
			nexthdr = *last_nexthdr_ptr;
		}
	}

check_pppoe:
	if (PKTPRS_IS_PPPOE(desc->rx, PKTPRS_HDR_LEVEL0))
		db_tinfo->pppoe = 1;

	dbg("ip_offset=%d hdr_len=%d esp_hdr_len=%d, iv_sz=%u, ipv6=%u\n",
	    db_tinfo->ip_offset, db_tinfo->hdr_len,
	    db_tinfo->esp_hdr_len, db_tinfo->iv_sz,
	    db_tinfo->ipv6);
	dumpskb(ptr, db_tinfo->hdr_len, 1);
}

static inline bool is_valid_pitem_tunidx(uint32_t tunidx)
{
	return (tunidx < MAX_TUN_ENTRIES);
}

static void get_vpn_tun_params(uint32_t spi,
			       struct intel_vpn_tunnel_info *tun_info)
{
	struct dp_qos_q_logic q_logic = {0};
	struct intel_vpn_ops *vpn_ops = dp_get_vpn_ops(0);

	if (!vpn_ops)
		return;

	vpn_ops->get_tunnel_info(vpn_ops->dev, spi, tun_info);

	if (!g_vpna_conn.gpid) {
		g_vpna_conn.gpid = tun_info->gpid;
		g_vpna_conn.subif =
			(tun_info->subif >> CPU_PORT_WLAN_BIT_MODE) & 0X0F;

		/*Egress port qid*/
		q_logic.q_id = tun_info->qid;

		/* physical to logical qid */
		if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
			dbg("%s:%d ERROR Failed to Logical Queue Id\n", __func__, __LINE__);
		}

		g_vpna_conn.qid = q_logic.q_logic_id;
	}
}

static int32_t
set_vpn_outbound_session_params(struct pp_sess_create_args *rt_entry,
				struct pktprs_desc *desc,
				struct uc_session_node *p_item,
				struct intel_vpn_tunnel_info *tun_info)
{
	/*Upstream session needs to first go to the VPN-A for encryption*/
	struct intel_vpn_ipsec_act vpn_act = {0};
	struct intel_vpn_ops *vpn_ops = dp_get_vpn_ops(0);
	u8 next_header = 0;

	if (!vpn_ops) {
		dbg("VPNA not initialized\n");
		return PPA_FAILURE;
	}

	/* 1. Set the egress port = VPNA.gpid */
	rt_entry->eg_port = g_vpna_conn.gpid;

	/* 2. Set the egress qid = VPNA.qid*/
	rt_entry->dst_q = g_vpna_conn.qid;

	/* 3. Set the dw0 and dw1 to be added egress in the VPNA */
	vpn_act.dw0_mask = ~(0x1FFF);  /* subif/tunnelid field dw0[12:0] */
	vpn_act.dw1_mask = ~(0xFF0000);/* port field dw1[23:16] */

	/* Egress GPID in dw1[23:16] */
	vpn_act.dw1_val = g_vpna_conn.gpid << 16;

	/* VPNA subif field in dw0[12:9] tunnel_id dw0[7:0] */
	vpn_act.dw0_val = (g_vpna_conn.subif << 9) | tun_info->tunnel_id;
	vpn_act.enq_qos = 1; /*enqueue with qos*/

	/*Add session in vpn FW */
	vpn_ops->add_session(vpn_ops->dev, tun_info->tunnel_id,
			     0 /* session_id */, &vpn_act);

	/* 4. set the dectyption parameters in the dw0 of session PSB */
	rt_entry->ud_sz = 0;
	rt_entry->ps = 0;
	/*<EIP_offset> dw0[7:0]*/
	if (PKTPRS_IS_IPV4(desc->tx, PKTPRS_HDR_LEVEL0))
		rt_entry->ps = pktprs_hdr_sz(desc->tx, PKTPRS_PROTO_IPV4, PKTPRS_HDR_LEVEL0);
	else if (PKTPRS_IS_IPV6(desc->tx, 0))
		rt_entry->ps = pktprs_hdr_sz(desc->tx, PKTPRS_PROTO_IPV6, PKTPRS_HDR_LEVEL0);
	rt_entry->ps |= BIT(8); /*<ENCRYPT> dw0[8]*/

	/* <NextHdr> dw0[16:13] */
	if (tun_info->mode == VPN_MODE_TUNNEL) {
		if (PKTPRS_IS_IPV4(desc->tx, PKTPRS_HDR_LEVEL1))
			next_header = IPPROTO_IPIP;
		else if (PKTPRS_IS_IPV6(desc->tx, PKTPRS_HDR_LEVEL1))
			next_header = IPPROTO_IPV6;

	} else {
		next_header = p_item->pkt.ip_proto;
	}

	if (vpn_ops->proto_to_next_header(vpn_ops->dev,
					  next_header, &next_header))
		return PPA_FAILURE;

	rt_entry->ps |= next_header << 13;

	/* TBD: rt_entry->ps third byte =
	 * (TBD:needed only in single pass << 1 ) & 0x0E;
	 */
	/* <VPN_Sess_off> dw0[19:17] */
	rt_entry->ps |= (0 /* session-id */ & 0x7) << 17;
	/* dw0[27:20] */
	rt_entry->ps |= (tun_info->tunnel_id & 0xFF) << 20;
	/* dw0[31:28] <TC> */
	rt_entry->ps |= (p_item->pkt.priority & 0xF) << 28;

	/* Set the session flags */
	/* Backup ingress desc to the UD#1 */
	rt_entry->flags |= PP_SESS_FLAG_PS_VALID_MSK;

	dbg("rt_entry->eg_port= %d tunnel_id=%d, dw0=%#x\n", rt_entry->eg_port,
		tun_info->tunnel_id, rt_entry->ps);

	return PPA_SUCCESS;
}

static inline void set_eg_ethtype(struct pktprs_desc *desc, uint16_t proto)
{
	if (!desc->tx)
		return;

	if (PKTPRS_IS_VLAN1(desc->tx, PKTPRS_HDR_LEVEL0)) {
		desc->tx->buf[pktprs_hdr_off(desc->tx, PKTPRS_PROTO_VLAN1, PKTPRS_HDR_LEVEL0)
			+ offsetof(struct vlan_hdr, h_vlan_encapsulated_proto)]
				= htons(proto);
	} else if (PKTPRS_IS_VLAN0(desc->tx, PKTPRS_HDR_LEVEL0)) {
                 desc->tx->buf[pktprs_hdr_off(desc->tx, PKTPRS_PROTO_VLAN0, PKTPRS_HDR_LEVEL0)
			+ offsetof(struct vlan_hdr, h_vlan_encapsulated_proto)]
				= htons(proto);
	} else if (PKTPRS_IS_MAC(desc->tx, PKTPRS_HDR_LEVEL0)) {
		desc->tx->buf[pktprs_hdr_off(desc->tx, PKTPRS_PROTO_MAC, PKTPRS_HDR_LEVEL0)
			+ offsetof(struct ethhdr, h_proto)]
				= htons(proto);
	}
}
static int32_t
set_vpn_inbound_session_params(struct pp_sess_create_args *rt_entry,
			struct pktprs_desc *desc,
			struct uc_session_node *p_item,
			struct intel_vpn_tunnel_info *tun_info)
{
	struct intel_vpn_ipsec_act vpn_act = {0};
	struct intel_vpn_ops *vpn_ops = dp_get_vpn_ops(0);
	uint16_t orig_eg_ethertype = 0;

	if (!vpn_ops) {
		dbg("VPNA not initialized\n");
		return PPA_FAILURE;
	}

	vpn_ops->update_tunnel_in_netdev(vpn_ops->dev, tun_info->tunnel_id,
					 p_item->rx_if);

	/* 1. Set the egress port = VPNA.gpid */
	rt_entry->eg_port = g_vpna_conn.gpid;

	/* 2. Set the egress qid = VPNA.qid*/
	rt_entry->dst_q = g_vpna_conn.qid;

	/* 3. Set the dw0 and dw1 to be added egress in the VPNA */
	vpn_act.dw0_mask = ~(0x1E00);  /* subif field dw0[12:9] */
	vpn_act.dw1_mask = ~(0xFF0000);/* port field dw1[23:16] */

	if (is_vpn_assist(desc, tun_info)) {
		/*IPSec assist uC subif field in dw0[12:9]*/
		vpn_act.dw0_val = g_vpn_nf.subif << 9;
		vpn_act.dw1_val = g_vpn_nf.gpid << 16;
		vpn_act.enq_qos = 1; /*enqueue with qos to send to uC*/
	} else {
		/*VPNA subif field in dw0[12:9] */
		vpn_act.dw0_val = g_vpna_conn.subif << 9;
		vpn_act.dw1_val = g_vpna_conn.gpid << 16;
		vpn_act.enq_qos = 1; /*enqueue with qos*/
	}
	vpn_ops->update_tunnel_in_act(vpn_ops->dev, tun_info->tunnel_id, &vpn_act);

	/*Get the original egress ethernet type*/
	if (PKTPRS_IS_IPV4(desc->tx, PKTPRS_HDR_LEVEL0)) {
		orig_eg_ethertype = ETH_P_IP;
	} else if (PKTPRS_IS_IPV6(desc->tx, PKTPRS_HDR_LEVEL0)) {
		orig_eg_ethertype = ETH_P_IPV6;
	}

	/* copy ingress fv to egress fv */
	ppa_memcpy(desc->tx, desc->rx, sizeof(*desc->rx));

	/* In case of PPPoE we need to remove the
	 * PPPoE header in the egress FV
	 */
	if (PKTPRS_IS_PPPOE(desc->tx, PKTPRS_HDR_LEVEL0))
		pktprs_proto_remove(desc->tx, PKTPRS_PROTO_PPPOE, PKTPRS_HDR_LEVEL0);

	/* 4. set the dectyption parameters in the dw0 of session PSB */
	rt_entry->ps = 0;
	rt_entry->ps &= ~BIT(8); /* <DECRYPT> dw0[8] */

	if (PKTPRS_IS_IPV4(desc->tx, PKTPRS_HDR_LEVEL0)) {
		if (tun_info->mode == VPN_MODE_TUNNEL) {
		/*In case of tunnel mode we need to remove the tunnel ip header*/
			pktprs_proto_remove(desc->tx, PKTPRS_PROTO_IPV4, PKTPRS_HDR_LEVEL0);
			/* <EIP_offset> dw0[7:0] */
			rt_entry->ps = pktprs_hdr_off(desc->tx, PKTPRS_PROTO_ESP, PKTPRS_HDR_LEVEL0) & 0xFF;

			/*If orig_eg_ethertype is ipv6 we need to update the ethernet header*/
			if (orig_eg_ethertype == ETH_P_IPV6) {
				set_eg_ethtype(desc, orig_eg_ethertype);
			}
		} else {
		/*In transport mode after VPN decryption the ip header needs to be updated by VPN-Assist*/
			/* <EIP_offset> dw0[7:0] */
			rt_entry->ps = pktprs_hdr_sz(desc->tx, PKTPRS_PROTO_IPV4, PKTPRS_HDR_LEVEL0) & 0xFF;
			/* <uC_IP_Offset> dw0[16:9] */
			rt_entry->ps |= (pktprs_hdr_off(desc->tx, PKTPRS_PROTO_IPV4,
								PKTPRS_HDR_LEVEL0) & 0xFF) << 9;
		}

	} else if (PKTPRS_IS_IPV6(desc->tx, PKTPRS_HDR_LEVEL0)) {
		if (tun_info->mode == VPN_MODE_TUNNEL) {
			/** In case of tunnel mode we need to remove the
			 *  tunnel ipv6 header including any extensions
			 */
			pktprs_ipv6_remove(desc->tx, PKTPRS_HDR_LEVEL0);

			/* <EIP_offset> dw0[7:0] */
			rt_entry->ps = pktprs_hdr_off(desc->tx, PKTPRS_PROTO_ESP, PKTPRS_HDR_LEVEL0) & 0xFF;

			/*If orig_eg_ethertype is ipv4 we need to update the ethernet header*/
			if (orig_eg_ethertype == ETH_P_IP) {
				set_eg_ethtype(desc, orig_eg_ethertype);
			}
		} else {
			/*In transport mode after VPN decryption the ip header needs to be updated by VPN-Assist*/
			/* <EIP_offset> dw0[7:0] */
			rt_entry->ps = pktprs_hdr_sz(desc->tx, PKTPRS_PROTO_IPV6, PKTPRS_HDR_LEVEL0) & 0xFF;
			/* <uC_IP_Offset> dw0[16:9] */
			rt_entry->ps |= (pktprs_hdr_off(desc->tx, PKTPRS_PROTO_IPV6,
								PKTPRS_HDR_LEVEL0) & 0xFF) << 9;
		}
	}

	rt_entry->ps |= (tun_info->tunnel_id & 0xFF) << 20; /* dw0[27:20] */
	rt_entry->ps |= (0 << 28); /*dw0[31:28] <TC>*/
	rt_entry->flags |= PP_SESS_FLAG_PS_VALID_MSK;

	/* ESP header to be inserted if the decrypted packet from VPNA comes back to CPU*/
	/* It happens when the decrypted packet hits exception session */
	spin_lock_bh(&g_tun_db_lock);
	if (!ipsec_tun_db[tun_info->tunnel_id].hdr)
		store_esp_tunnel_header(p_item, desc, tun_info);
	spin_unlock_bh(&g_tun_db_lock);

	dbg("rt_entry->eg_port= %d tunnel_id=%d, dw0=%#x\n",
		rt_entry->eg_port,
		tun_info->tunnel_id,
		rt_entry->ps);

	return PPA_SUCCESS;
}
#endif /*IS_ENABLED(CONFIG_INTEL_VPN)*/

#if defined(PPA_API_PROC)
static int proc_read_ppv4_rtstats(struct seq_file *seq, void *v)
{
	if (!capable(CAP_SYSLOG)) {
		printk ("Read Permission denied");
		return 0;
	}

	seq_printf(seq,	"=====================================================================\n");
	seq_printf(seq,	"Total Number of Routing session entrys			: %llu\n", nsess_add_succ - nsess_del + nsess_del_fail);
	seq_printf(seq,	"Total Number of Routing session add requests		: %llu\n", nsess_add);
	seq_printf(seq,	"Total Number of Routing session delete			: %llu\n", nsess_del);
	seq_printf(seq,	"Total Number of Routing session delete fail		: %llu\n", nsess_del_fail);
	seq_printf(seq,	"Total Number of Routing session add fails		: %llu\n", nsess_add_fail_rt_tbl_full + nsess_add_fail_coll_full + nsess_add_fail_oth);
	seq_printf(seq,	"Total Number of Routing session add fail rt tbl full	: %llu\n", nsess_add_fail_rt_tbl_full);
	seq_printf(seq,	"Total Number of Routing session add fail coll full	: %llu\n", nsess_add_fail_coll_full);
	seq_printf(seq,	"Total Number of Routing session add fail others	: %u\n", nsess_add_fail_oth);
	seq_printf(seq,	"=====================================================================\n");
	return 0;
}

static int proc_read_ppv4_rtstats_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_ppv4_rtstats, NULL);
}

static ssize_t proc_clear_ppv4_rtstats(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[40];
	char *p;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
	while ( len && str[len - 1] <= ' ' )
		len--;
	str[len] = 0;
	for ( p = str; *p && *p <= ' '; p++, len-- );
	if ( !*p )
		return count;

	if (strncmp(p, "clear", 5) == 0) {
		nsess_add = 0;
		nsess_del = 0;
		nsess_del_fail = 0;
		nsess_add_succ = 0;
		nsess_add_fail_rt_tbl_full = 0;
		nsess_add_fail_coll_full = 0;
		nsess_add_fail_oth = 0;
		printk(KERN_ERR "PPv4 HAL stats cleared!!!\n");
	} else {
		printk(KERN_ERR "usage : echo clear > /sys/kernel/debug/ppa/pp_hal/rtstats\n");
	}

	return len;
}

static int proc_read_ppv4_accel(struct seq_file *seq, void *v)
{
	if (!capable(CAP_SYSLOG)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq,	"PPv4 Upstream Acceleration	: %s\n", g_us_accel_enabled ? "enabled" : "disabled");
	seq_printf(seq,	"PPv4 Downstream Acceleration	: %s\n", g_ds_accel_enabled ? "enabled" : "disabled");
	return 0;
}

static int proc_read_ppv4_accel_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_ppv4_accel, NULL);
}

static ssize_t proc_set_ppv4_accel(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[40];
	char *p;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
	while ( len && str[len - 1] <= ' ' )
		len--;
	str[len] = 0;
	for ( p = str; *p && *p <= ' '; p++, len-- );
	if ( !*p )
		return count;

	if (strncmp(p, "enable", 6) == 0) {
		if (len > 6) {
			if (strncmp(p + 7, "us", 2) == 0) {
				g_us_accel_enabled = 3;
			} else if (strncmp(p + 7, "ds", 2) == 0) {
				g_ds_accel_enabled = 3;
			}
		} else {
			g_us_accel_enabled = 3;
			g_ds_accel_enabled = 3;
		}
		printk(KERN_ERR "Acceleration Enabled!!!\n");
	} else if (strncmp(p, "disable", 7) == 0) {
		if (len > 7) {
			if (strncmp(p + 8, "us", 2) == 0) {
				g_us_accel_enabled=0;
			} else if (strncmp(p + 8, "ds", 2) == 0) {
				g_ds_accel_enabled=0;
			}
		} else {
			g_us_accel_enabled = 0;
			g_ds_accel_enabled = 0;
		}
		printk(KERN_ERR "Acceleration Disabled!!!\n");
	} else {
		printk(KERN_ERR "usage : echo <enable/disable> [us/ds] > /sys/kernel/debug/ppa/pp_hal/accel\n");
	}

	return len;
}

static int proc_read_ppv4_debug(struct seq_file *seq, void *v)
{
	if (!capable(CAP_SYSLOG)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq,	"PPv4 Debug	: %s\n", lgm_pp_hal_dbg_enable ? "enabled" : "disabled");
	return 0;
}

static int proc_read_ppv4_debug_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_ppv4_debug, NULL);
}

static ssize_t proc_set_ppv4_debug(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[40];
	char *p;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
	while ( len && str[len - 1] <= ' ' )
		len--;
	str[len] = 0;
	for ( p = str; *p && *p <= ' '; p++, len-- );
	if ( !*p )
		return count;

	if (strncmp(p, "enable", 6) == 0) {
		lgm_pp_hal_dbg_enable = 1;
		printk(KERN_ERR"Debug Enabled!!!\n");
	} else if (strncmp(p, "disable", 7) == 0) {
		lgm_pp_hal_dbg_enable = 0;
		printk(KERN_ERR"Debug Disbled!!!\n");
	} else {
		printk(KERN_ERR "usage : echo <enable/disable> > /sys/kernel/debug/ppa/pp_hal/dbg\n");
	}

	return len;
}

#if IS_ENABLED(CONFIG_LGM_TOE)
static int proc_read_ppv4_lro(struct seq_file *seq, void *v)
{
	if (!capable(CAP_SYSLOG)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq,	"PPv4 lro session learning	: %s\n", g_lro_enabled ? "enabled" : "disabled");
	return 0;
}

static int proc_read_ppv4_lro_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_ppv4_lro, NULL);
}

static ssize_t proc_set_ppv4_lro(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[40];
	char *p;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
	while ( len && str[len - 1] <= ' ' )
		len--;
	str[len] = 0;
	for ( p = str; *p && *p <= ' '; p++, len-- );
	if ( !*p )
		return count;

	if (strncmp(p, "enable", 6) == 0) {
		g_lro_enabled = 1;
		printk(KERN_ERR"LRO session learning enabled!!!\n");
	} else if (strncmp(p, "disable", 7) == 0) {
		g_lro_enabled = 0;
		printk(KERN_ERR"LRO session learning disabled!!!\n");
	} else {
		printk(KERN_ERR "usage : echo <enable/disable> > /sys/kernel/debug/ppa/pp_hal/lro\n");
	}

	return len;
}
#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/

#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
static int proc_read_ppv4_lp(struct seq_file *seq, void *v)
{
	if (!capable(CAP_SYSLOG)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq,	"Litepath HW offload	: %s\n", g_lp_enabled ? "enabled" : "disabled");
	return 0;
}

static int proc_read_ppv4_lp_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_ppv4_lp, NULL);
}

static ssize_t proc_set_ppv4_lp(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[40];
	char *p;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
	while ( len && str[len - 1] <= ' ' )
		len--;
	str[len] = 0;
	for ( p = str; *p && *p <= ' '; p++, len-- );
	if ( !*p )
		return count;

	if (strncmp(p, "enable", 6) == 0) {
		g_lp_enabled = 1;
		printk(KERN_ERR"Litepath HW offload enabled!!!\n");
	} else if (strncmp(p, "disable", 7) == 0) {
		g_lp_enabled = 0;
		printk(KERN_ERR"Litepath HW offload disabled!!!\n");
	} else {
		printk(KERN_ERR "usage : echo <enable/disable> > /sys/kernel/debug/ppa/pp_hal/litepath\n");
	}

	return len;
}
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/

#if IS_ENABLED(CONFIG_PPA_BBF247_MODE1)
static int ppa_run_cmd(const char *cmd)
{
	int ret;
	char **argv;
	static char *envp[] = {
		"HOME=/",
		"PATH=/sbin:/bin:/usr/sbin:/usr/bin",
		NULL
	};

	argv = argv_split(GFP_KERNEL, cmd, NULL);
	if (argv) {
		ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
		argv_free(argv);
	} else {
		ret = -ENOMEM;
	}

	if (ret)
		printk(KERN_ERR "%s: {%s} Failed %d\n", __func__, cmd, ret);

	return ret;
}

/* PON DS Session */
int32_t ppa_add_bbf247_mode1_ds_session(PPA_NETIF *rxif, PPA_NETIF *txif,
					u32 class, uint32_t *session_id)
{
	struct pp_sess_create_args args = {0};
	struct qos_mgr_match_qid eg_tc = {0};
	PPA_SUBIF *vuni_subif;
	PPA_SUBIF *lan_subif;
	int ret = PPA_FAILURE;

	vuni_subif = kzalloc(sizeof(PPA_SUBIF), GFP_KERNEL);
	lan_subif = kzalloc(sizeof(PPA_SUBIF), GFP_KERNEL);

	if (!vuni_subif || !lan_subif) {
		printk(KERN_ERR "%s: subifid kmalloc failed\n", __func__);
		goto __out_free;
	}

	if (dp_get_netif_subifid(rxif, NULL, NULL, NULL, vuni_subif, 0)) {
		printk(KERN_ERR "%s: lan dp_get_netif_subif failed\n", __func__);
		goto __out_free;
	}

	if (dp_get_netif_subifid(txif, NULL, NULL, NULL, lan_subif, 0)) {
		printk(KERN_ERR "%s: vuni dp_get_netif_subif failed\n", __func__);
		goto __out_free;
	}

	eg_tc.class = class;

	args.in_port = vuni_subif->gpid;
	args.eg_port = lan_subif->gpid;
	args.dst_q = qos_mgr_get_mapped_queue(txif, args.eg_port, 0, &eg_tc, queue_delete_cb, 0);
	args.color = PP_COLOR_GREEN;
	args.flags = BIT(PP_SESS_FLAG_INTERNAL_HASH_CALC_BIT);
	args.ps = lan_subif->subif & 0xFFFF;
	args.flags |= PP_SESS_FLAG_PS_VALID_MSK;
	args.ps |= 0x8000000;
	args.cls.n_flds = 2;
	args.cls.fld_data[0] = class;
	args.cls.fld_data[1] = 0;  /* Must check b16=0 for DS sessions */

	memset(&args.sgc, U8_MAX, sizeof(args.sgc));
	memset(&args.tbm, U8_MAX, sizeof(args.tbm));

	printk("%s %d args: dst_q=%d in_port %u eg_port %u ps 0x%x f 0x%lx\n",
	       __FUNCTION__, __LINE__, args.dst_q, args.in_port, args.eg_port,
	       args.ps, args.flags);

	if ((ret = pp_session_create(&args, session_id, NULL))) {
		printk(KERN_ERR "%s: BBF.247 session_create failed!!! ret=%d\n",
		       __FUNCTION__, ret);
		goto __out_free;
	}
	ret = PPA_SUCCESS;

__out_free:
	kfree(vuni_subif);
	kfree(lan_subif);
	return ret;
}

/* PON US Session */
int32_t ppa_add_bbf247_mode1_us_session(PPA_NETIF *rxif, PPA_NETIF *txif,
					uint32_t *session_id)
{
	struct pp_sess_create_args args = {0};
	struct qos_mgr_match_qid eg_tc = {0};
	PPA_SUBIF *vuni_subif;
	PPA_SUBIF *lan_subif;
	int ret = PPA_FAILURE;

	vuni_subif = kzalloc(sizeof(PPA_SUBIF), GFP_KERNEL);
	lan_subif = kzalloc(sizeof(PPA_SUBIF), GFP_KERNEL);

	if (!vuni_subif || !lan_subif) {
		printk(KERN_ERR "%s: subifid kmalloc failed\n", __func__);
		goto __out_free;
	}

	if (dp_get_netif_subifid(rxif, NULL, NULL, NULL, lan_subif, 0)) {
		printk(KERN_ERR "%s: lan dp_get_netif_subifid failed\n", __func__);
		goto __out_free;
	}

	if (dp_get_netif_subifid(txif, NULL, NULL, NULL, vuni_subif, 0)) {
		printk(KERN_ERR "%s: vuni dp_get_netif_subifid failed\n", __func__);
		goto __out_free;
	}

	eg_tc.class = 0;

	args.in_port = lan_subif->gpid;
	args.eg_port = vuni_subif->gpid;
	args.dst_q = qos_mgr_get_mapped_queue(txif, args.eg_port, 0, &eg_tc, queue_delete_cb, 0);
	args.color = PP_COLOR_GREEN;
	args.flags = BIT(PP_SESS_FLAG_INTERNAL_HASH_CALC_BIT);
	args.ps = vuni_subif->subif & 0xFFFF;
	args.ps |= BIT(16); /* Set the US flag bit - for multicast control packets marking */
	args.flags |= PP_SESS_FLAG_PS_VALID_MSK;

	memset(&args.sgc, U8_MAX, sizeof(args.sgc));
	memset(&args.tbm, U8_MAX, sizeof(args.tbm));

	printk("%s %d args: dst_q=%d in_port %u eg_port %u ps 0x%x f 0x%lx\n",
		__FUNCTION__, __LINE__, args.dst_q, args.in_port, args.eg_port,
		args.ps, args.flags);

	if ((ret = pp_session_create(&args, session_id, NULL))) {
		printk(KERN_ERR "%s: BBF.247 session_create failed!!! ret=%d\n",
		       __FUNCTION__, ret);
		goto __out_free;
	}
	ret = PPA_SUCCESS;

__out_free:
	kfree(vuni_subif);
	kfree(lan_subif);
	return ret;
}

int32_t ppa_pp_update_ports(PPA_NETIF *lan_if, PPA_NETIF *vuni_if)
{
	struct pp_port_cfg vuni_cfg = {0};
	struct pp_port_cfg lan_cfg = {0};
	PPA_SUBIF *vuni_subif;
	PPA_SUBIF *lan_subif;
	int ret = PPA_FAILURE;

	vuni_subif = kzalloc(sizeof(PPA_SUBIF), GFP_KERNEL);
	lan_subif = kzalloc(sizeof(PPA_SUBIF), GFP_KERNEL);

	if (!vuni_subif || !lan_subif) {
		printk(KERN_ERR "%s: subifid kmalloc failed\n", __func__);
		goto __out_free;
	}

	if (dp_get_netif_subifid(lan_if, NULL, NULL, NULL, lan_subif, 0)) {
		printk(KERN_ERR "%s: lan dp_get_netif_subifid failed\n", __func__);
		goto __out_free;
	}

	if (dp_get_netif_subifid(vuni_if, NULL, NULL, NULL, vuni_subif, 0)) {
		printk(KERN_ERR "%s: vuni dp_get_netif_subifid failed\n", __func__);
		goto __out_free;
	}

	if (unlikely(pp_port_get(lan_subif->gpid, &lan_cfg))) {
		printk(KERN_ERR "%s: pp_port_get failed gpid: %d\n", __func__, lan_subif->gpid);
		goto __out_free;
	}

	lan_cfg.rx.parse_type = NO_PARSE;
	if (unlikely(pp_port_update(lan_subif->gpid, &lan_cfg))) {
		printk(KERN_ERR "%s: pp_port_update failed gpid: %d\n", __func__, lan_subif->gpid);
		goto __out_free;
	}

	if (unlikely(pp_port_get(vuni_subif->gpid, &vuni_cfg))) {
		printk(KERN_ERR "%s: pp_port_get failed gpid: %d\n", __func__, vuni_subif->gpid);
		goto __out_free;
	}

	/* Additionally set classfication based on class + b16 */
	vuni_cfg.rx.parse_type = NO_PARSE;
	vuni_cfg.rx.cls.n_flds = 2;
	vuni_cfg.rx.cls.cp[0].stw_off = 28;
	vuni_cfg.rx.cls.cp[0].copy_size = 4;
	vuni_cfg.rx.cls.cp[1].stw_off = 16;
	vuni_cfg.rx.cls.cp[1].copy_size = 1;

	if (unlikely(pp_port_update(vuni_subif->gpid, &vuni_cfg))) {
		printk(KERN_ERR "%s: pp_port_update failed gpid: %d\n", __func__, vuni_subif->gpid);
		goto __out_free;
	}
	ret = PPA_SUCCESS;

__out_free:
	kfree(vuni_subif);
	kfree(lan_subif);
	return ret;
}

void ppa_bbf247_mode1(PPA_IFNAME vani_netif_name[PPA_IF_NAME_SIZE],
		      PPA_IFNAME vuni_netif_name[PPA_IF_NAME_SIZE],
		      PPA_IFNAME lan_netif_name[PPA_IF_NAME_SIZE],
		      bool enable)
{
	struct pp_port_cfg vuni_cfg = {0};
	struct pp_port_cfg lan_cfg = {0};
	PPA_NETIF *vuni_if;
	PPA_NETIF *lan_if;
	PPA_SUBIF *vuni_subif;
	PPA_SUBIF *lan_subif;
	u32  session_id = 0, idx;
	char buffer[100];

	vuni_subif = kzalloc(sizeof(PPA_SUBIF), GFP_KERNEL);
	lan_subif = kzalloc(sizeof(PPA_SUBIF), GFP_KERNEL);

	if (!vuni_subif || !lan_subif) {
		printk(KERN_ERR "%s: subifid kmalloc failed\n", __func__);
		goto __out_free;
	}

	if (enable) {
		printk("Enable->vani: %s vuni: %s lan: %s\n", vani_netif_name, vuni_netif_name, lan_netif_name);
		if ((lan_if = ppa_get_netif(lan_netif_name)) == NULL) {
			printk(KERN_ERR "%s: get lan_if ppa_get_netif failed\n", __func__);
			goto __out_free;
		}

		if ((vuni_if = ppa_get_netif(vuni_netif_name)) == NULL) {
			printk(KERN_ERR "%s: get vuni_if ppa_get_netif failed\n", __func__);
			goto __out_free;
		}

		/* Re-create the br-lan bridge with user given interfaces */
		if (bbf247_sess->sess_cnt == 0) {
			/* Add the VANI0 interface always for mode1 */
			ppa_run_cmd("/usr/sbin/brctl addif br-lan VANI0");

			if (!(strcmp(vani_netif_name, "VANI0") == 0)) {
				snprintf(buffer, sizeof(buffer), "/usr/sbin/brctl addif br-lan %s", vani_netif_name);
				ppa_run_cmd(buffer);
			}

			snprintf(buffer, sizeof(buffer), "/usr/sbin/brctl addif br-lan %s", lan_netif_name);
			ppa_run_cmd(buffer);
		} else { /* ADD LAN interface with existing br-lan */
			snprintf(buffer, sizeof(buffer), "/usr/sbin/brctl addif br-lan %s", lan_netif_name);
			ppa_run_cmd(buffer);
		}

		/* Change Port configuration for VUNI and LAN */
		if (ppa_pp_update_ports(lan_if, vuni_if) != PPA_SUCCESS) {
			printk(KERN_ERR "%s: ppa_pp_update_ports failed\n", __func__);
			goto __br_lan_def_conf;
		}

		/* Create US session */
		if (ppa_add_bbf247_mode1_us_session(lan_if, vuni_if, &session_id) != PPA_SUCCESS) {
			printk(KERN_ERR "%s: ppa_add_bbf247_mode1_us_session failed\n", __func__);
			goto __br_lan_def_conf;
		}
		bbf247_sess->sess_id[bbf247_sess->sess_cnt++] = session_id;

		/* Create DS session */
		if (ppa_add_bbf247_mode1_ds_session(vuni_if, lan_if, (bbf247_sess->sess_cnt / 2), &session_id) != PPA_SUCCESS) {
			printk(KERN_ERR "%s: ppa_add_bbf247_mode1_ds_session failed\n", __func__);
			goto __br_lan_def_conf;
		}
		bbf247_sess->sess_id[bbf247_sess->sess_cnt++] = session_id;
	} else {
		printk("Disable->vuni_netif: %s\n", vuni_netif_name);

		if ((vuni_if = ppa_get_netif(vuni_netif_name)) == NULL) {
			printk(KERN_ERR "%s: get vuni_if ppa_get_netif failed\n", __func__);
			goto __out_free;
		}

		if (dp_get_netif_subifid(vuni_if, NULL, NULL, NULL, vuni_subif, 0)) {
			printk(KERN_ERR "%s: vuni dp_get_netif_subifid failed\n", __func__);
			goto __out_free;
		}

		if (unlikely(pp_port_get(vuni_subif->gpid, &vuni_cfg))) {
			printk(KERN_ERR "%s: vuni_if failed to get cfg of gpid: %d\n", __func__, vuni_subif->gpid);
			goto __out_free;
		}

		/* Revert the vUNI port config */
		vuni_cfg.rx.parse_type = L2_PARSE;
		vuni_cfg.rx.cls.n_flds = 0;

		if (unlikely(pp_port_update(vuni_subif->gpid, &vuni_cfg))) {
			printk(KERN_ERR "%s: pp_port_update failed  gpid: %d\n", __func__, vuni_subif->gpid);
			goto __out_free;
		}

		for (idx = 0; idx < (bbf247_sess->sess_cnt / 2); idx++) {
			if ((lan_if = ppa_get_netif(bbf247_sess->eth_arr[idx + 2])) == NULL) {
				printk(KERN_ERR "%s: get vuni_if ppa_get_netif failed\n", __func__);
				goto __br_lan_def_conf;
			}

			if (dp_get_netif_subifid(lan_if, NULL, NULL, NULL, lan_subif, 0)) {
				printk(KERN_ERR "%s: vuni dp_get_netif_subifid failed\n", __func__);
				goto __br_lan_def_conf;
			}

			if (unlikely(pp_port_get(lan_subif->gpid, &lan_cfg))) {
				printk(KERN_ERR "%s: lan_if failed to get cfg of gpid: %d\n", __func__, lan_subif->gpid);
				goto __br_lan_def_conf;
			}

			/* Revert the LAN port config */
			lan_cfg.rx.parse_type = L2_PARSE;
			lan_cfg.rx.cls.n_flds = 0;

			if (unlikely(pp_port_update(lan_subif->gpid, &lan_cfg))) {
				printk(KERN_ERR "%s: pp_port_update failed	gpid: %d\n", __func__, lan_subif->gpid);
				goto __br_lan_def_conf;
			}
		}
		goto __br_lan_def_conf;
	}

	goto __out_free;

__br_lan_def_conf:
	for (idx = 0; idx < bbf247_sess->sess_cnt; idx++) {
		if (bbf247_sess->sess_id[idx] != BBF247_INVALID_SESS_ID) {
			if (pp_session_delete(bbf247_sess->sess_id[idx], NULL)) {
				printk(KERN_ERR "%s: %d pp_session_delete failed\n",
				       __func__, bbf247_sess->sess_id[idx]);
			}
		}
		bbf247_sess->sess_id[idx] = BBF247_INVALID_SESS_ID;
	}

	/* Remove the VANI0 interface */
	ppa_run_cmd("/usr/sbin/brctl delif br-lan VANI0");
	if (!(strcmp(bbf247_sess->eth_arr[0], "VANI0") == 0)) {
		snprintf(buffer, sizeof(buffer), "/usr/sbin/brctl delif br-lan %s", bbf247_sess->eth_arr[0]);
		ppa_run_cmd(buffer);
	}

	bbf247_sess->sess_cnt = 0;
	memset(&bbf247_sess->eth_arr, 0, sizeof(char *) * ARRAY_SIZE(bbf247_sess->eth_arr));

__out_free:
	kfree(vuni_subif);
	kfree(lan_subif);
	return;
}

static int proc_read_ppv4_bbf247_hgu(struct seq_file *seq, void *v)
{
	int idx;

	if (!capable(CAP_SYSLOG)) {
		printk("Read Permission denied");
		return 0;
	}

	if (bbf247_sess->sess_cnt > 0) {
		seq_printf(seq,	"Interfaces\n");
		for (idx = 2; idx < BBF247_MAX_SESS_ID; idx++) {
			if (bbf247_sess->eth_arr[idx] != NULL)
				seq_printf(seq,	"\t %s->%s->%s\n",
						   bbf247_sess->eth_arr[idx],
						   bbf247_sess->eth_arr[0],
						   bbf247_sess->eth_arr[1]);
		}
	} else {
		seq_printf(seq,	"BBF247 Session Empty !!!\n");
	}

	return 0;
}

static int proc_read_ppv4_bbf247_hgu_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_ppv4_bbf247_hgu, NULL);
}

static ssize_t proc_set_ppv4_bbf247_hgu(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len, cnt = 0, idx, offset = 0;
	char *eth_arr[BBF247_MAX_SESS_ID];
	const char *s;
	char *p;
	char buffer[100], str[100];

	if (!capable(CAP_NET_ADMIN)) {
		printk("Write Permission denied");
		return 0;
	}

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
	while (len && str[len - 1] <= ' ')
		len--;
	str[len] = 0;

	for (p = str; *p && *p <= ' '; p++, len--);

	if (!*p)
		return count;

	if (strcmp(buf, "help") == 0)
		goto __write_help;

	s = str;
	while (sscanf(s, "%s%n", buffer, &offset) == 1) {
		s += offset;
		eth_arr[cnt] = kzalloc((strlen(buffer) + 1), GFP_KERNEL);
		if (!eth_arr[cnt]) {
			for (idx = 0; idx < cnt; idx++)
				kfree(eth_arr[idx]);
			printk(KERN_ERR "%s: kmalloc failed\n", __func__);
			return len;
		}
		strcpy(eth_arr[cnt++], buffer);
	}

	if (cnt == 0)
		goto __write_help;

	if ((strcmp(eth_arr[cnt - 1], "enable") == 0) && cnt > 3) {
		if ((strstr(eth_arr[0], "ANI") == NULL) ||
			(strstr(eth_arr[1], "VUNI0_") == NULL) ||
			(strstr(eth_arr[2], "eth0_") == NULL))
			goto __write_help;

		if (bbf247_sess->sess_cnt > 0) {
			printk("!!! BBF247 Session Already Created\n");
			goto __out_free;
		}

		memcpy(&bbf247_sess->eth_arr, &eth_arr, sizeof(char *) * (cnt - 1));
		for (idx = 2; idx < cnt - 1; idx++)
			ppa_bbf247_mode1(eth_arr[0], eth_arr[1], eth_arr[idx], true);
	} else if ((strcmp(eth_arr[cnt - 1], "disable") == 0) && (cnt == 1)) {
		if (bbf247_sess->sess_cnt <= 0) {
			printk("!!! BBF247 Session Not Created\n");
			goto __out_free;
		}
		ppa_bbf247_mode1(NULL, bbf247_sess->eth_arr[1], NULL, false);

		/* Free the interface name mem */
		for (idx = 0; idx < BBF247_MAX_SESS_ID; idx++)
			kfree(bbf247_sess->eth_arr[idx]);
	} else {
		goto __write_help;
	}

	return len;

__write_help:
	printk("Mode1: enable\n");
	printk("\t arg1 - VANI interface ( VANI0 or VANI0.xx or ANI0_wanxx_.. )\n");
	printk("\t arg2 - VUNI interface ( VUNI0_0 )\n");
	printk("\t arg3 - LAN  interfaces ( eth0_1 -> eth0_5)\n");
	printk("\t arg4 - enable\n");
	printk("\t Example# echo VANI0 VUNI0_0 eth0_1 enable> /sys/kernel/debug/ppa/pp_hal/bbf247_hgu_mode1\n\n");

	printk("Mode1: disable\n");
	printk("\t arg1 - disable\n");
	printk("\t Example# echo disable> /sys/kernel/debug/ppa/pp_hal/bbf247_hgu_mode1\n");
	goto __out_free;

__out_free:
	/* Free the interfaces memory */
	for (idx = 0; idx < cnt; idx++)
		kfree(eth_arr[idx]);

	return len;

}
#endif /*CONFIG_PPA_BBF247_MODE1*/

void ppv4_proc_file_create(void)
{
	struct dentry *res;

	/*TBD: replace this with necessary data to be passed to the fn*/
	void *dummy_data = NULL;

	if (!g_ppa_debugfs_flag) {
		g_ppa_debugfs_dir = debugfs_create_dir("ppa", NULL);
		g_ppa_debugfs_flag = 1;
	}

	g_ppa_ppv4hal_debugfs_dir = debugfs_create_dir("pp_hal", g_ppa_debugfs_dir);
	g_ppa_ppv4hal_debugfs_flag = 1;

	res = debugfs_create_file("accel", 0600,
			g_ppa_ppv4hal_debugfs_dir,
			dummy_data,
			&g_proc_file_ppv4_accel_seq_fops);
	if (!res)
		goto err;

	res = debugfs_create_file("dbg", 0600,
			g_ppa_ppv4hal_debugfs_dir,
			dummy_data,
			&g_proc_file_ppv4_debug_seq_fops);

	if (!res)
		goto err;

	res = debugfs_create_file("rtstats", 0600,
			g_ppa_ppv4hal_debugfs_dir,
			dummy_data,
			&g_proc_file_ppv4_rtstats_seq_fops);
	if (!res)
		goto err;
#if IS_ENABLED(CONFIG_LGM_TOE)
	res = debugfs_create_file("lro", 0600,
			g_ppa_ppv4hal_debugfs_dir,
			dummy_data,
			&g_proc_file_ppv4_lro_seq_fops);
	if (!res)
		goto err;

#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/

#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
	res = debugfs_create_file("litepath", 0600,
			g_ppa_ppv4hal_debugfs_dir,
			dummy_data,
			&g_proc_file_ppv4_lp_seq_fops);
	if (!res)
		goto err;

#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/

	res = debugfs_create_u32("sess_timeout_thr", 0600,
				 g_ppa_ppv4hal_debugfs_dir, &g_sess_timeout_thr);
	if (!res)
		goto err;

#if IS_ENABLED(CONFIG_PPA_BBF247_MODE1)
	bbf247_sess = kzalloc(sizeof(*bbf247_sess), GFP_KERNEL);

	if (!bbf247_sess)
		goto err;

	memset(&bbf247_sess->sess_id, BBF247_INVALID_SESS_ID,
			sizeof(int) * ARRAY_SIZE(bbf247_sess->sess_id));

	memset(&bbf247_sess->eth_arr, 0,
			sizeof(char *) * ARRAY_SIZE(bbf247_sess->eth_arr));

	res = debugfs_create_file("bbf247_hgu_mode1", 0600,
			g_ppa_ppv4hal_debugfs_dir,
			dummy_data,
			&g_proc_file_ppv4_bbf247_hgu_seq_fops);
	if (!res)
		goto err;
#endif /*CONFIG_PPA_BBF247_MODE1*/

	return;

err:
	debugfs_remove_recursive(g_ppa_ppv4hal_debugfs_dir);
	g_ppa_ppv4hal_debugfs_flag = 0;
#if IS_ENABLED(CONFIG_PPA_BBF247_MODE1)
	kfree(bbf247_sess);
#endif /*CONFIG_PPA_BBF247_MODE1*/
}

void ppv4_proc_file_remove(void)
{
	if (g_ppa_ppv4hal_debugfs_flag) {
		debugfs_remove_recursive(g_ppa_ppv4hal_debugfs_dir);
		g_ppa_ppv4hal_debugfs_flag = 0;
	}
#if IS_ENABLED(CONFIG_PPA_BBF247_MODE1)
	kfree(bbf247_sess);
#endif /*CONFIG_PPA_BBF247_MODE1*/
}
#endif /*defined(PPA_API_PROC)*/
/*
 * ####################################
 *		 Global Function
 * ####################################
 */

/*!
	\fn uint32_t get_hal_id(uint32_t *p_family,
			uint32_t *p_type,
			uint32_t *p_if,
			uint32_t *p_mode,
			uint32_t *p_major,
			uint32_t *p_mid,
			uint32_t *p_minor)
			char *name,
			char *version)
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
	\brief read HAL ID
	\param p_family	get family code
	\param p_type	get driver type
	\param p_if	get interface type
	\param p_mode	get driver mode
	\param p_major	get major number
	\param p_mid	get mid number
	\param p_minor	get minor number
	\return no return value
 */
void get_lgm_pp_hal_id(uint32_t *p_family,
			uint32_t *p_type,
			uint32_t *p_if,
			uint32_t *p_mode,
			uint32_t *p_major,
			uint32_t *p_mid,
			uint32_t *p_minor)
{
	if ( p_family )
		*p_family = VER_FAMILY;

	if ( p_type )
		*p_type = VER_DRTYPE;

	if ( p_if )
		*p_if = VER_INTERFACE;

	if ( p_mode )
		*p_mode = VER_ACCMODE;

	if ( p_major )
		*p_major = VER_MAJOR;

	if ( p_mid )
		*p_mid = VER_MID;

	if ( p_minor )
		*p_minor = VER_MINOR;
}

/*!
	\fn uint32_t get_firmware_id(uint32_t *id,
					 char *name,
					 char *version)
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
	\brief read firmware ID
	\param p_family	 get family code
	\param p_type	 get firmware type
	\return no return value
 */
int32_t get_firmware_id(uint32_t *id,
				char *p_name,
				char *p_version)
{
	/* TBD */
	/* Get the firmware version from PPv4 */

	/*
	*id=sw_version.nId;
	ppa_memcpy(p_name,sw_version.cName, PPA_VERSION_LEN);
	ppa_memcpy(p_version,sw_version.cVersion, PPA_VERSION_LEN);
	*/

	return PPA_SUCCESS;
}

/*!
	\fn uint32_t get_number_of_phys_port(void)
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
	\brief get max number of physical ports
	\return get max number of physical ports
 */
uint32_t get_number_of_phys_port(void)
{
	/* TBD?? needed ? get the port number dynamically from the system*/

	return MAX_LGM_PORTS;
}

/*!
	\fn void get_phys_port_info(uint32_t port, uint32_t *p_flags,
				PPA_IFNAME ifname[PPA_IF_NAME_SIZE])
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
	\brief get physical port information
	\param port	 in port id
	\param p_flags	 get flags
	\param ifname	 get inteface name [ depricated ]
	\return no return value
 */
void get_phys_port_info(uint32_t port, uint32_t *p_flags,
				PPA_IFNAME ifname[PPA_IF_NAME_SIZE])
{
	/* This function can only set the flags based on GSWIP-O configuration
	Interface name needs to be retrieved from the dp */

	if ( port >= MAX_LGM_PORTS) {
		if (p_flags)
			*p_flags = 0;
		if (ifname)
			*ifname = 0;
		return;
	}

	if (p_flags) {
		*p_flags = 0;
		switch(port) {
		case 0: /*CPU port */
			*p_flags = PPA_PHYS_PORT_FLAGS_MODE_CPU_VALID;
			*ifname = 0;
			break;
		case 2: /*LAN side ports */
		case 3:
		case 4:
		case 5:
		case 6:
			if (g_port_map & (1 << port)) {
				*p_flags = PPA_PHYS_PORT_FLAGS_MODE_ETH_LAN_VALID;
			}
			break;
		case 7: /* dynamic ports */
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			if (g_port_map & (1 << port)) {
				*p_flags = PPA_PHYS_PORT_FLAGS_MODE_ETH_MIX_VALID;
			}
			break;
		case 1: /* ethernet wan port*/
			if (g_port_map & (1 << port)) {
				*p_flags = PPA_PHYS_PORT_FLAGS_MODE_ETH_WAN_VALID;
			}
			break;
		default:
			*p_flags = 0;
			break;
		}
	}
}

/*!
	\fn void get_max_route_entries(uint32_t *p_entry,
					uint32_t *p_mc_entry)
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
	\brief get maximum number of routing entries
	\param p_entry	get maximum number of uni-cast routing entries.
	\param p_mc_entry get maximum number of multicast routing entries.
	\return no return value
 */
void get_max_route_entries(uint32_t *p_entry,
				uint32_t *p_mc_entry)
{
	if ( p_entry )
		*p_entry = g_max_hw_sessions;

	if ( p_mc_entry )
		*p_mc_entry = MAX_MC_GROUP_ENTRIES;
}

/*!
	\fn void get_acc_mode(uint32_t f_is_lan, uint32_t *p_acc_mode)
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
	\brief get acceleration mode for interfaces (LAN/WAN)
	\param f_is_lan		 0: WAN interface, 1: LAN interface
	\param p_acc_mode	 a u32 data pointer to get acceleration mode (PPA_ACC_MODE_ROUTING / PPA_ACC_MODE_NONE)
	\return no return value
 */
void get_acc_mode(uint32_t f_is_lan, uint32_t *p_acc_mode)
{
	if (f_is_lan)
		*p_acc_mode = g_us_accel_enabled;
	else
		*p_acc_mode = g_ds_accel_enabled;
}

/*!
	\fn void set_acc_mode(uint32_t f_is_lan,
					uint32_t acc_mode)
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
	\brief set acceleration mode for interfaces (LAN/WAN)
	\param f_is_lan		 0: WAN interface, 1: LAN interface
	\param p_acc_mode	 acceleration mode (PPA_ACC_MODE_ROUTING / PPA_ACC_MODE_NONE/ PPA_ACC_MODE_BRIDGING/ PPA_ACC_MODE_HYBRID)
	\return no return value
*/
void set_acc_mode(uint32_t f_is_lan, uint32_t acc_mode)
{
	if (f_is_lan)
		g_us_accel_enabled = acc_mode;
	else
		g_ds_accel_enabled = acc_mode;
}

static inline int32_t set_egress_port_n_queue(struct uc_session_node *p_item,
					struct pp_sess_create_args *rt_entry,
					PPA_SUBIF *dp_port)
{
	PPA_IFNAME phys_netif_name[PPA_IF_NAME_SIZE];
	struct qos_mgr_match_qid eg_tc = {0};
	PPA_NETIF *txif = p_item->tx_if;
	struct pp_port_cfg pcfg = {0};
	bool len_update = 0;

	if ((p_item->flags & (SESSION_VALID_PPPOE | SESSION_TUNNEL_DSLITE | SESSION_TUNNEL_6RD
		| SESSION_VALID_PPPOL2TP | SESSION_VALID_VLAN_INS | SESSION_VALID_OUT_VLAN_INS))
		|| (p_item->flag2 & SESSION_FLAG2_GRE) || netif_is_macvlan(txif)) {
		if (ppa_get_physical_if(txif, NULL, phys_netif_name) == PPA_SUCCESS) {
			txif = ppa_get_netif(phys_netif_name);
		}
			dbg("txif=%s\n", txif->name);
	}
	ppa_memset(dp_port, 0, sizeof(PPA_SUBIF));
	/* Get the egress gpid from the tx netdevice */
	if (dp_get_netif_subifid(txif, NULL, NULL, NULL, dp_port, 0)) {
		dbg("Unable to get tx netdevice GPID!!!\n");
		return PPA_FAILURE;
	}

	/* Set eg port */
	rt_entry->eg_port = dp_port->gpid;
	dbg("%s %d ig_gpid=%d eg_port=%d eg_gpid=%d\n", __FUNCTION__, __LINE__,
	rt_entry->in_port, dp_port->port_id, rt_entry->eg_port);

	/*Set the dest_q */
	eg_tc.class = p_item->pkt.priority;
	rt_entry->dst_q = qos_mgr_get_mapped_queue(txif, rt_entry->eg_port, 0, &eg_tc, queue_delete_cb, 0);
	dbg("%s %d rt_entry.dst_q=%d\n", __FUNCTION__, __LINE__, rt_entry->dst_q);

	/* Set the egress port max_len */
	if (p_item->flags & (SESSION_VALID_VLAN_INS | SESSION_VALID_OUT_VLAN_INS)) {
		if (pp_port_get(dp_port->gpid, &pcfg)) {
			dbg("pp_port_get failed in %s %d\n", __FUNCTION__, __LINE__);
		}

		if ((p_item->flags & SESSION_VALID_VLAN_INS) &&
			pcfg.tx.max_pkt_size < (ETH_MTU + ETH_HLEN + VLAN_HLEN)) {
			pcfg.tx.max_pkt_size += VLAN_HLEN;
			len_update = 1;
		}
		if ((p_item->flags & SESSION_VALID_OUT_VLAN_INS) &&
			pcfg.tx.max_pkt_size < (ETH_MTU + ETH_HLEN + VLAN_HLEN*2)) {
			pcfg.tx.max_pkt_size += VLAN_HLEN;
			len_update = 1;
		}
		if (len_update) {
			/*Set the modified port configuration */
			if (pp_port_update(dp_port->gpid, &pcfg)) {
				dbg("pp_port_update failed in %s %d\n", __FUNCTION__, __LINE__);
			}
		}
	}

	return PPA_SUCCESS;
}

/*!
	\fn int32_t add_routing_entry(PPA_ROUTING_INFO *route_info)
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
	\brief add one routing entry
 */
int32_t add_routing_entry(PPA_ROUTING_INFO *route)
{
	int32_t ret = 0, i;
	uint32_t session_id = 0;
	struct pp_sess_create_args rt_entry;
	struct pktprs_desc *desc = NULL;
	PPA_SUBIF *dp_port;
	PPA_SUBIF *dp_port_vuni;
#if IS_ENABLED(CONFIG_LGM_TOE)
	bool udp_gso = 1;
	PPA_LRO_INFO lro_entry = {0};
#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/
#if IS_ENABLED(CONFIG_INTEL_VPN)
	struct intel_vpn_tunnel_info tun_info = {0};
#endif /*IS_ENABLED(CONFIG_INTEL_VPN)*/
	bool is_eg_vpna=0;
	struct dp_qos_q_logic q_logic = {0};

	struct uc_session_node *p_item = (struct uc_session_node *)route->p_item;
	if (!p_item) {
		dbg("uc_session_node is null!!!\n");
		return PPA_FAILURE;
	}

	desc = (struct pktprs_desc *)p_item->session_meta;
	if (!desc) {
		dbg("pktprs descriptor is null!!!\n");
		return PPA_FAILURE;
	}

	if ((!p_item->hwhash.h1) && (!p_item->hwhash.h2) && (!p_item->hwhash.sig)) {
		dbg("hardware hash is null!!!\n");
		return PPA_FAILURE;
	}

	ppa_memset(&rt_entry, 0, sizeof(struct pp_sess_create_args));

	nsess_add++;

	rt_entry.in_port = p_item->ig_gpid;
	if (!rt_entry.in_port) {
		if (!(p_item->flag2 |= SESSION_FLAG2_CPU_OUT)) {
			dbg("Ingress port in null!\n");
			return PPA_FAILURE;
		} else {
		/* Local out session */
		/*TBD: ingress port needs to be set as the litepath device gpid when the litepath HW acceleration is enabled*/
#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
			rt_entry.in_port = g_lpdev.gpid;
#else
			/*until the hw acceleration is enabled; we keep this session not accelable*/
			p_item->flags |= SESSION_NOT_ACCELABLE;
			return PPA_FAILURE;
#endif
		}
	}

	dp_port = ppa_malloc(sizeof(PPA_SUBIF));
	if (!dp_port) {
		dbg("[%s:%d] DP subif allocation failed!\n",
			__func__, __LINE__);
		return PPA_ENOMEM;
	}
	ppa_memset(dp_port, 0, sizeof(PPA_SUBIF));

	if (p_item->tx_if) {
	/* get the physical tx netdevice*/
		if (!(p_item->flag2 & (SESSION_FLAG2_VALID_IPSEC_INBOUND|SESSION_FLAG2_VALID_IPSEC_OUTBOUND)) ||
		    !secpath_exists(desc->skb)) {
			/* non-esp or real esp bypass sessions */
			if (set_egress_port_n_queue(p_item, &rt_entry, dp_port) != PPA_SUCCESS) {
				ppa_free(dp_port);
				return PPA_FAILURE;
			}
		} else {
#if IS_ENABLED(CONFIG_INTEL_VPN)
			/*Get the vpn gpid and qid populated */
			/*Get the tunnel information */
			get_vpn_tun_params(p_item->spi, &tun_info);
			spin_lock_bh(&g_tun_db_lock);
			/* add the tunnel to the db */
			if (!is_valid_pitem_tunidx(p_item->tunnel_idx)) {
				p_item->tunnel_idx = tun_info.tunnel_id;
				ipsec_tunn_db_add(p_item);
			}
			/* add the session to the tunnel list for rekeying deletion */
			list_add(&p_item->tun_node,
				 &ipsec_tun_db[tun_info.tunnel_id].sessions);
			spin_unlock_bh(&g_tun_db_lock);

			/* Downstream ESP packets; to be forwarded to the VPNA for decryption*/
			if (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_INBOUND) {
				/* IPSec Inbound session 1:Tunnel session*/
				is_eg_vpna = 1;
				if (set_vpn_inbound_session_params(&rt_entry, desc, p_item, &tun_info) != PPA_SUCCESS) {
					ppa_free(dp_port);
					return PPA_FAILURE;
				}
			} else {
				/*p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND*/
				if (tun_info.vpn_if->ifindex == desc->rx->ifindex) {
					/*
					 * we use the same tx pkt of the 1st
					 * session to open the 2nd one,
					 * so the ing port/gpid are not updated
					 */
					p_item->ig_gpid = g_vpna_conn.gpid;
					rt_entry.in_port = p_item->ig_gpid;
					/* IPSec Outboud session 2: Tunnel session "already encrypted"
					egress port and queue shall be like any other normal session*/
					rt_entry.flags |= PP_SESS_FLAG_INTERNAL_HASH_CALC_MSK;
					if (set_egress_port_n_queue(p_item, &rt_entry, dp_port) != PPA_SUCCESS) {
						ppa_free(dp_port);
						return PPA_FAILURE;
					}
				} else {
					/* In this case ingress packet is plain ethernet and egress
					is un encrypted ESP packet; we need to get the SPI from EGRESS packet*/
					/* IPSec Outbound session 1: Upstream TCP/UDP packets;
					to be forwarded to VPNA for ESP encapsulation and encryption */

					is_eg_vpna = 1;
					if (set_vpn_outbound_session_params(&rt_entry, desc, p_item, &tun_info)
											!= PPA_SUCCESS) {
						ppa_free(dp_port);
						return PPA_FAILURE;
					}
				}
			}
#endif /*IS_ENABLED(CONFIG_INTEL_VPN)*/
		}
	} else {
		/* Locally terminated session we need to get the CPU GPID/queueid or LRO GPID/queueid*/
		if ((p_item->flag2 & SESSION_FLAG2_CPU_IN)) {
#if IS_ENABLED(CONFIG_LGM_TOE)
			if (!ppa_bypass_lro(p_item->session)) {
				/*add lro entry in PP and lro engine */
				ppa_memset(&lro_entry,0,sizeof(lro_entry));

				if (p_item->flags & SESSION_IS_TCP) {
					lro_entry.lro_type = LRO_TYPE_TCP;
					/*check mptcp options and if yes set
					lro_entry.lro_type = LRO_TYPE_MPTCP;
					*/
				} else if (p_item->flags & SESSION_TUNNEL_ESP) {
					dbg("ESP not supported in LRO\n");
				} else {
					struct sock *sk = NULL;

					struct udphdr *uh = udp_hdr(desc->skb);
					if (!uh) {
						ppa_free(dp_port);
						return PPA_FAILURE;
					}
					switch (desc->skb->protocol) {
						case htons(ETH_P_IP):
							sk = udp4_lib_lookup_skb(desc->skb, uh->source, uh->dest);
							break;
						case htons(ETH_P_IPV6):
							sk = udp6_lib_lookup_skb(desc->skb, uh->source, uh->dest);
							break;
					}
					if (g_pp_hw_version != 1 ) {
						if(!sk || !udp_sk(sk)->gro_enabled)
							udp_gso = 0;
					} else {
						/* In A step UDP LRO is not supported in LRO engine */
						udp_gso = 0;
					}
					lro_entry.lro_type = LRO_TYPE_UDP;
				}

				if (udp_gso && g_lro_enabled &&
					(add_lro_entry(&lro_entry) == PPA_SUCCESS)) {
					dbg("lro entry added\n");
					p_item->flag2 |= SESSION_FLAG2_LRO;
					p_item->lro_sessid = lro_entry.session_id;
					/*cpu gpid if litepath offload is not enabled*/
					rt_entry.eg_port = get_cpu_portinfo();
					/*LRO qid as returned from the lro conn*/
					rt_entry.dst_q = lro_entry.dst_q;
					/*set the lro flowid */
					rt_entry.lro_info = lro_entry.session_id;

					/*Set the number of Template UD bytes to copy*/
					rt_entry.tmp_ud_sz = COPY_16BYTES;

					/*Set the session flags */
					rt_entry.flags |= PP_SESS_FLAG_LRO_INFO_MSK;
				} else {
					dbg("lro entry add failed\n");
					rt_entry.eg_port = get_cpu_portinfo();
					rt_entry.tmp_ud_sz = COPY_16BYTES;
					q_logic.q_id = get_cpu_qid();
					if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
						pr_err("%s:%d ERROR Failed to Logical Queue Id\n", __func__, __LINE__);
						ppa_free(dp_port);
						return PPA_FAILURE;
					}
					rt_entry.dst_q = q_logic.q_logic_id;
#if !IS_ENABLED(LITEPATH_HW_OFFLOAD)
					/*HW offload is not enabled session cannot be accelerated*/
					p_item->flags |= SESSION_NOT_ACCELABLE;
					ppa_free(dp_port);
					return PPA_FAILURE;
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
				}
			} else {
#if !IS_ENABLED(CONFIG_LITEPATH_OFFLOAD)
				ppa_free(dp_port);
				return PPA_FAILURE;
#endif
			}
#endif /*CONFIG_LGM_TOE*/

#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
			/*Set the gpid and qid of lpdev device*/
			/* TBD: litepath hw offload supported only for TCP IPV4*/
			if (g_lp_enabled
				&& p_item->pkt.protocol ==  ETH_P_IP &&
					p_item->flags & SESSION_IS_TCP) {
				rt_entry.eg_port = g_lpdev.gpid;
			}
			if (!(p_item->flag2 & SESSION_FLAG2_LRO)) {
				if (g_lp_enabled)
					rt_entry.dst_q = g_lpdev.qid;
			}
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
		} else {
			dbg("Unable to get tx netdevice GPID!!!\n");
			ppa_free(dp_port);
			return PPA_FAILURE;
		}
	}

#if 0 // FIXME: needs more clarity
	/* Set the fsqm priority */
	/* TBD: where to fetch this info */
	rt_entry.fsqm_prio = lgm_get_fsqm_prio(desc->skb, p_item->tx_if);
#endif

	/*Set Color for the session */
	/* TBD: Based on what we are supposed to set color*/
	rt_entry.color = lgm_get_session_color(p_item);

	/*TBD:Set the session group counters */
	for (i = 0; i < ARRAY_SIZE(rt_entry.sgc); i++)
		rt_entry.sgc[i] = PP_SGC_INVALID;

	/*TBD: Set the token bucket metering */
	for (i = 0; i < ARRAY_SIZE(rt_entry.tbm); i++)
		rt_entry.tbm[i] = PP_TBM_INVALID;

	/*Set the UD parameters */
	if (!p_item->is_loopback) {
		if (!is_eg_vpna) {
			rt_entry.ps = 0;

			if ((p_item->flag2 & SESSION_FLAG2_CPU_IN)) {
#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
				/* TBD: litepath hw offload supported only for TCP IPV4*/
				if (g_lp_enabled &&
					(p_item->pkt.protocol ==  ETH_P_IP
						&& p_item->flags & SESSION_IS_TCP)) {
					/* If litepath is enabled packet needs to be received
					   on litepath netdev */
					rt_entry.ps = g_lpdev.subif & 0xFFFF; /* sub_if_id */
				}
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
				if (p_item->flag2 & SESSION_FLAG2_LRO) {
#if IS_ENABLED(CONFIG_LGM_TOE)
					/*Set lro type in UD*/
					/*set the lro type in dw0[25:24]*/
					rt_entry.ps |= (lro_entry.lro_type & 0x03) << 24;
#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/
				}
			} else {
				if ((dp_port->alloc_flag & DP_F_VUNI)
					&& (dp_port->data_flag & DP_SUBIF_VANI)) {

					dp_port_vuni = ppa_malloc(sizeof(PPA_SUBIF));
					if (!dp_port_vuni) {
						dbg("[%s:%d] DP subif allocation failed!\n",
							__func__, __LINE__);
						ppa_free(dp_port);
						return PPA_ENOMEM;
					}
					ppa_memset(dp_port_vuni, 0, sizeof(PPA_SUBIF));

					if (dp_get_netif_subifid(dp_port->associate_netif,
						NULL, NULL, NULL, dp_port_vuni, 0)) {
						dbg("Unable to get tx netdevice GPID!!!\n");
						ppa_free(dp_port_vuni);
						ppa_free(dp_port);
						return PPA_FAILURE;
					}
					/* Set eg port to vuni port*/
					rt_entry.eg_port = dp_port_vuni->gpid;
					rt_entry.ps =  dp_port_vuni->subif & 0xFFFF; /* sub_if_id */
					ppa_free(dp_port_vuni);
				} else {
					rt_entry.ps =  p_item->dest_subifid & 0xFFFF; /* sub_if_id */
					if(!(dp_port->alloc_flag & DP_F_ACA)) {
						/*In case of DC interfaces we dont need Egress flag */
						/* This field is supposed to carry DevQos for wireless*/
						/*set the egress flag in the SI UD bit 27 of PS-B*/
						rt_entry.ps |= BIT(27);
					}
					if (dp_port->alloc_flag & DP_F_FAST_WLAN) {
						uint8_t devqos;
						if (qos_mgr_get_class2devqos_fn) {
							if (qos_mgr_get_class2devqos_fn(
										p_item->tx_if,
										dp_port,
										p_item->pkt.priority,
										&devqos) == DP_SUCCESS) {

								/*In case of DC interfaces we dont need Egress flag */
								/* This field carries DevQos for wireless*/
								/*set the DevQoS bit 24:27 */
								rt_entry.ps |= (devqos & 0xF) << 24;
								/*set the Calss bit 28:31 */
								rt_entry.ps |= (p_item->pkt.priority & 0xF) << 28;
								dbg("rt_entry.ps:%u devqos:%d p_item->pkt.priority:%d\n", rt_entry.ps, devqos, p_item->pkt.priority);
							} else {
								ppa_free(dp_port);
								return PPA_FAILURE;
							}
						}
					}
				}
			}

			/*Set the session flags */
			rt_entry.flags |= PP_SESS_FLAG_PS_VALID_MSK;
		}

	} else {
		/*TBD: Fill in the pp_port_cls_data in case we have a second cycle through PPv4 */
	}

	/* Fill in the fv information */
	rt_entry.rx = desc->rx;
	rt_entry.tx = desc->tx;

	/* Fill in the Hash information */
	rt_entry.hash.h1 = p_item->hwhash.h1;
	rt_entry.hash.h2 = p_item->hwhash.h2;
	rt_entry.hash.sig = p_item->hwhash.sig;

	dbg("ig_gpid=%d eg_port=%d eg_gpid=%d, rt_entry.dst_q=%d\n",
		rt_entry.in_port, dp_port->port_id, rt_entry.eg_port, rt_entry.dst_q);
	ppa_free(dp_port);

	/* Callin the API */
	if ((ret = pp_session_create(&rt_entry, &session_id, NULL))) {
		switch(ret) {
		/* TBD: handle the correct errorcode */
		case GSW_ROUTE_ERROR_RT_SESS_FULL:
			nsess_add_fail_rt_tbl_full++;
			break;
		case GSW_ROUTE_ERROR_RT_COLL_FULL:
			nsess_add_fail_coll_full++;
			break;
		default:
			nsess_add_fail_oth++;
			break;
		}
		dbg("pp_session_create returned failure!! %s %d ret=%d\n", __FUNCTION__, __LINE__,ret);
		if (p_item->flag2 & SESSION_FLAG2_LRO) {
			del_lro_entry(rt_entry.lro_info);
		}
		return PPA_FAILURE;
	}

	dbg("%s %d pp_session_create succeeded id=%d\n", __FUNCTION__, __LINE__,session_id);
	route->entry = p_item->routing_entry = session_id;
	p_item->flags |= SESSION_ADDED_IN_HW;

	/*set used flag in hal db*/
	if ((session_id >= 0) && (session_id < g_max_hw_sessions)) {
		spin_lock_bh(&g_hal_db_lock);
		pp_hal_db[session_id].used = 1;
		if (rt_entry.flags & PP_SESS_FLAG_LRO_INFO_MSK)
			pp_hal_db[session_id].lro = 1;
		pp_hal_db[session_id].node = (void*)p_item;
#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
		if ((p_item->flag2 & SESSION_FLAG2_CPU_IN)) {
			/* TBD: litepath hw offload supported only for IPV4 TCP*/
			if (g_lp_enabled &&
				(p_item->pkt.protocol ==  ETH_P_IP &&
					p_item->flags & SESSION_IS_TCP)) {
				pp_hal_db[session_id].lp_rxinfo =
					(struct lp_info*)ppa_malloc(sizeof(struct lp_info));
				if (pp_hal_db[session_id].lp_rxinfo) {
					pp_hal_db[session_id].lp_rxinfo->dst = skb_dst(desc->skb);
					pp_hal_db[session_id].lp_rxinfo->netif =
						p_item->br_rx_if ? p_item->br_rx_if : p_item->rx_if;
					if (PKTPRS_IS_IPV4(desc->tx, PKTPRS_HDR_LEVEL0)) {
						pp_hal_db[session_id].lp_rxinfo->l3_offset =
							pktprs_hdr_off(desc->tx, PKTPRS_PROTO_IPV4, PKTPRS_HDR_LEVEL0);
						pp_hal_db[session_id].lp_rxinfo->l4_offset =
							pktprs_hdr_sz(desc->tx, PKTPRS_PROTO_IPV4, PKTPRS_HDR_LEVEL0);
					} else if (PKTPRS_IS_IPV6(desc->tx, PKTPRS_HDR_LEVEL0)) {
						pp_hal_db[session_id].lp_rxinfo->l3_offset =
							pktprs_hdr_off(desc->tx, PKTPRS_PROTO_IPV6, PKTPRS_HDR_LEVEL0);
						pp_hal_db[session_id].lp_rxinfo->l4_offset =
							pktprs_hdr_sz(desc->tx, PKTPRS_PROTO_IPV6, PKTPRS_HDR_LEVEL0);
					}
				}
			}
		}
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
		spin_unlock_bh(&g_hal_db_lock);
		dbg("%s %d session id=%d p_item=%px\n", __FUNCTION__, __LINE__, p_item->routing_entry, p_item);
	} else {
		dbg("invalid session_id %d!!\n", session_id);
	}

	return PPA_SUCCESS;
}

/*!
	\fn int32_t del_routing_entry(PPA_ROUTING_INFO *route_info)
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
	\brief add one routing entry
*/
int32_t del_routing_entry(PPA_ROUTING_INFO *route)
{
	int32_t ret=0;
	struct lp_info *lp_rxinfo = NULL;

	struct uc_session_node *p_item = (struct uc_session_node *)route->p_item;
	if (!p_item)
		return PPA_FAILURE;

	if (p_item->flags & SESSION_ADDED_IN_HW) {

		dbg("%s %d deleting p_item=%px sessionid=%d\n", __FUNCTION__, __LINE__, p_item, p_item->routing_entry);

		if ((ret = pp_session_delete(p_item->routing_entry, NULL))) {
			dbg("pp_session delete returned Error:%d\n",ret);
		} else {
			if (p_item->flag2 & SESSION_FLAG2_LRO)
				del_lro_entry(p_item->lro_sessid);
			/*Reset set used flag in hal db*/
			spin_lock_bh(&g_hal_db_lock);
			lp_rxinfo = pp_hal_db[p_item->routing_entry].lp_rxinfo;
			ppa_memset(&pp_hal_db[p_item->routing_entry],0,sizeof(PP_HAL_DB_NODE));
			spin_unlock_bh(&g_hal_db_lock);
#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
			if (lp_rxinfo) {
				ppa_free(lp_rxinfo);
			}
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/
		}

	}
	p_item->flags &= ~SESSION_ADDED_IN_HW;

#if IS_ENABLED(CONFIG_INTEL_VPN)
	if (!list_empty(&p_item->tun_node)) {
		spin_lock_bh(&g_tun_db_lock);
		if (!list_empty(&p_item->tun_node))
			list_del_init(&p_item->tun_node);
		spin_unlock_bh(&g_tun_db_lock);
	}
#endif

	return ret;
}

/*!
	\fn void del_wan_mc_entry(uint32_t entry)
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
	\brief delete one multicast routing entry
	\param entry	entry number got from function call "add_wan_mc_entry"
	\return no return value
 */
int32_t del_wan_mc_entry(PPA_MC_INFO *mc_route)
{
	int32_t ret = 0, i;

	if (mc_route->p_item) {
		struct mc_session_node *p_item = (struct mc_session_node *)mc_route->p_item;

		spin_lock_bh(&g_hal_mc_db_lock);
		if (mc_db[p_item->grp.group_id].used) {
			/*1. Delete the multicast Mc session */
			for (i = 0; i < MAX_MC_CLIENT_PER_GRP; i++) {
				if (is_valid_session(mc_db[p_item->grp.group_id].dst[i].sess_id)) {
					/*1. Delete all the group member PPv4sessions*/
					/* session_id always stored +1 to handle valid sessionid 0*/
					if ((ret = pp_session_delete(mc_db[p_item->grp.group_id].dst[i].sess_id - 1, NULL))) {
						dbg("pp_session delete multicast client session returned Error:%d\n", ret);
					}
				}
				/*If the interface bitmask is set have to remove the uC entry */
				if (p_item->grp.if_mask & (1 << i)) {
					/* 2.Reset the bit in uC multicast group*/
					if ((ret = pp_mcast_dst_set(p_item->grp.group_id, i, PPA_IF_DEL))) {
						dbg("pp_mcast_dst_set returned Error:%d\n", ret);
					}
				}
			}

			/* 3.Clear the hal db entry */
			ppa_memset(&mc_db[p_item->grp.group_id], 0, sizeof(MC_DB_NODE));
			/* Clear the HW flag */
			p_item->flags &= ~SESSION_ADDED_IN_HW;

			/*If the session is not a DROP session */
			if (!(p_item->flags & SESSION_DROP)) {
				/*4. Delete the multicast group PPv4 session*/
				if (is_valid_session(mc_route->p_entry)) {
					if ((ret = pp_session_delete(mc_route->p_entry - 1, NULL))) {
						dbg("pp_session delete multicast group session returned Error:%d\n", ret);
					}
					p_item->mc_entry = mc_route->p_entry = 0;
				}
			}
		}
		spin_unlock_bh(&g_hal_mc_db_lock);
	}

	return ret;
}

/*!
	\fn int32_t update_wan_mc_entry(PPA_MC_INFO mc_route)
	\ingroup PPA_lgm_pp_hal_GLOBAL_FUNCTIONS
*/
int32_t update_wan_mc_entry(PPA_MC_INFO *mc_route)
{
	struct pp_sess_create_args pp_args = {0};
	struct lgm_mc_args *mc_args = NULL;
	struct qos_mgr_match_qid eg_tc = {0};
	uint32_t session_id = 0, dev_idx = 0;
	PPA_SUBIF *dp_port;
	int32_t ret = PPA_FAILURE, i = 0;

	struct mc_session_node *p_item = (struct mc_session_node *)mc_route->p_item;
	if (!p_item) {
		dbg("mc_session_node is null!!!\n");
		return PPA_FAILURE;
	}

	/*If the call is invoked from the parsing driver p_item->session action will not be NULL*/
	if (!p_item->session_action) {

		/*Call is invoked from multicast daemon
		Following actions needs to be invoked
		1. update the HAL MC DB
		2. based on the current operation mc_route->cop we need to update the pp uC */
		if (mc_route->cop) {
			dev_idx = mc_route->cop->index; /* [valid index 0 -7] */
			if ((dev_idx >= 0) && (dev_idx < MAX_MC_GROUP_ENTRIES)) {
				/*Update the bit in uC multicast group*/
				if ((ret = pp_mcast_dst_set(p_item->grp.group_id, dev_idx, mc_route->cop->flag))) {
					dbg("pp_mcast_dst_set returned Error:%d group = %d index = %d op = %d\n",
						ret, p_item->grp.group_id, dev_idx, mc_route->cop->flag);
				} else {
					dbg("uC mc %s for group=%d index=%d dev=%s\n, "
						, (mc_route->cop->flag ? "set" : "reset"), p_item->grp.group_id, dev_idx,
						p_item->grp.txif[dev_idx].netif->name);
				/*If the operation is delete we need to delete the correspoinding session from the pp*/
					if (mc_route->cop->flag == PPA_IF_DEL) {
						spin_lock_bh(&g_hal_mc_db_lock);
						if (is_valid_session(mc_db[p_item->grp.group_id].dst[dev_idx].sess_id)) {
							/*Delete the group member PPv4sessions*/
							/* session_id always stored +1 to handle valid sessionid 0*/
							if ((ret = pp_session_delete(
								mc_db[p_item->grp.group_id].dst[dev_idx].sess_id - 1, NULL))) {
								dbg("pp_session delete multicast client session %d returned Error:%d\n",
									mc_db[p_item->grp.group_id].dst[dev_idx].sess_id - 1, ret);
							}
							dbg("pp_session delete multicast client session %d succeeded\n",
								mc_db[p_item->grp.group_id].dst[dev_idx].sess_id - 1);
							/*Clear the index in the db*/
							ppa_memset(&mc_db[p_item->grp.group_id].dst[dev_idx],
								0, sizeof(struct client_info));
						}
						spin_unlock_bh(&g_hal_mc_db_lock);
					} else {
						/* Set egress netdevice for the client in the DB */
						spin_lock_bh(&g_hal_mc_db_lock);
						mc_db[p_item->grp.group_id].dst[dev_idx].netdev
							= p_item->grp.txif[dev_idx].netif;
						spin_unlock_bh(&g_hal_mc_db_lock);
						p_item->flags |= SESSION_ADDED_IN_HW;
					}
				}
			}
		} else {
			/*operation not specified nothing needs to be done */
			dbg("Multicast operation not specified !!!\n");
		}
		return ret;
	}

	/*IF the control reaches here the call is invoked from the parsing driver */
	mc_args = (struct lgm_mc_args *) p_item->session_action;
	if (mc_args && !mc_args->desc) {
		dbg("pktprs descriptor is null!!!\n");
		return PPA_FAILURE;
	}

	if (!mc_args->ig_gpid) {
		dbg("Session ingress gpid is not valid\n");
		return PPA_FAILURE;
	}

	memset(&pp_args, 0, sizeof(pp_args));

	pp_args.color = PP_COLOR_GREEN;
	/* Fill in the Hash information */
	pp_args.hash.h1 = mc_args->hwhash.h1;
	pp_args.hash.h2 = mc_args->hwhash.h2;
	pp_args.hash.sig = mc_args->hwhash.sig;

	/*TBD:Set the session group counters */
	for (i = 0; i < ARRAY_SIZE(pp_args.sgc); i++)
		pp_args.sgc[i] = PP_SGC_INVALID;
	/*TBD: Set the token bucket metering */
	for (i = 0; i < ARRAY_SIZE(pp_args.tbm); i++)
		pp_args.tbm[i] = PP_TBM_INVALID;
	/*End TBD Set*/

	/*************Getting ingress and egress ports***************/
	pp_args.in_port = mc_args->ig_gpid;

	/*Compare the ingress gpid with the multicast GPID; which is stored*/
	/*if the gpids match this is session for a specific destination.*/
	if (pp_args.in_port == g_mcast_nf.gpid) {
		/* Find the tx netdevice */
		if (p_item->grp.group_id == mc_args->groupid &&
			((p_item->grp.if_mask >> (mc_args->dst_idx)) & 1)) {

			dev_idx = mc_args->dst_idx; /* [valid value returned is [0-15] */
			if ((dev_idx >= 0) && dev_idx < MAX_MC_CLIENT_PER_GRP) {
				/* if session already exists return */
				spin_lock_bh(&g_hal_mc_db_lock);
				if (is_valid_session(mc_db[p_item->grp.group_id].dst[dev_idx].sess_id)) {
					/*Session exists but Add request is coming again
					we need to compare the HASH and if it is not matching delete the session*/
					if ((mc_db[p_item->grp.group_id].dst[dev_idx].sess_hash.h1
						!= mc_args->hwhash.h1) ||
						(mc_db[p_item->grp.group_id].dst[dev_idx].sess_hash.h2
						!= mc_args->hwhash.h2)) {
						/*Delete the group member PPv4sessions*/
						/* session_id always stored +1 to handle valid sessionid 0*/
						if ((ret = pp_session_delete(
							mc_db[p_item->grp.group_id].dst[dev_idx].sess_id - 1, NULL))) {
							dbg("pp_session delete multicast dst sess%d ret Error:%d\n",
								mc_db[p_item->grp.group_id].dst[dev_idx].sess_id - 1, ret);
						}

						/*Clear the index in the db*/
						mc_db[p_item->grp.group_id].dst[dev_idx].sess_id = 0;
						mc_db[p_item->grp.group_id].dst[dev_idx].sess_hash.h1 = 0;
						mc_db[p_item->grp.group_id].dst[dev_idx].sess_hash.h2 = 0;
						dbg("deleted client session at index =%d\n", dev_idx);
					} else {
						dbg("A client session exists for group=%d at index=%d\n",
							p_item->grp.group_id, dev_idx);
					}
					spin_unlock_bh(&g_hal_mc_db_lock);
					return PPA_SUCCESS;
				}
				spin_unlock_bh(&g_hal_mc_db_lock);

				dp_port = ppa_malloc(sizeof(PPA_SUBIF));
				if (!dp_port) {
					dbg("[%s:%d] DP subif allocation failed!!!\n",
						__func__, __LINE__);
					return PPA_ENOMEM;
				}
				ppa_memset(dp_port, 0, sizeof(PPA_SUBIF));

				if (p_item->grp.txif[dev_idx].netif) {
					/*************Get the egress gpid from the tx netdevice******/
					if (dp_get_netif_subifid(p_item->grp.txif[dev_idx].netif,
						NULL, NULL, NULL, dp_port, 0)){

						dbg("Unable to get tx netdevice GPID!!!\n");
						ppa_free(dp_port);
						return PPA_FAILURE;
					}
					/* egress gpid */
					pp_args.eg_port = dp_port->gpid;
					/*egress qid */
					eg_tc.class = 0;
					pp_args.dst_q = qos_mgr_get_mapped_queue(p_item->grp.txif[dev_idx].netif,
						pp_args.eg_port, 0, &eg_tc, NULL, 0);
					dbg("dst_dev=%s\n", p_item->grp.txif[dev_idx].netif->name);
					/************************************************************/
				}

				/*Setup the classification parameters*/
				/*classification not based on FV: Duplicate packets pass through PPv4*/
				pp_args.rx = mc_args->desc->rx;
				pp_args.tx = mc_args->desc->tx;
				/*End of FV information*/

				/*Classification is based on the Groupid and the dst_index present in the UD0*/
				pp_args.cls.n_flds = 2;
				pp_args.cls.fld_data[0] = mc_args->groupid;
				pp_args.cls.fld_data[1] = mc_args->dst_idx;

				/*Set the egress UD parameters */
				pp_args.ud_sz = 0;
				/*For wlan stations we need to pass the multicast gpid to the fw for reliable multicast*/
				if ((p_item->grp.txif[dev_idx].if_flags & NETIF_DIRECTCONNECT_WIFI)) {
					/* multicast group id bit[0-8] */
					pp_args.ps = p_item->grp.group_id & 0x1FF;
				} else {
					/* sub_if_id */
					pp_args.ps = dp_port->subif & 0xFFFF;
				}
				ppa_free(dp_port);

				/* set the egress flag in the SI UD bit 27 of PS-B */
				pp_args.ps |= BIT(27);

				pp_args.flags |= PP_SESS_FLAG_PS_VALID_MSK;

				pp_args.mcast.grp_idx = p_item->grp.group_id;
				pp_args.mcast.dst_idx = mc_args->dst_idx;
				/*Set the session flags */
				pp_args.flags |= PP_SESS_FLAG_MCAST_DST_MSK;

				/*add hardware session */
				if ((ret = pp_session_create(&pp_args, &session_id, NULL))) {
					dbg("pp_session_create returned failure!! %s %d ret=%d\n", __FUNCTION__, __LINE__, ret);
				} else {
					/* store session_id against the corresponding dev_idx in the hal mc db */
					if (is_valid_session(session_id + 1)) {
						spin_lock_bh(&g_hal_mc_db_lock);
						/*always stored +1 to handle index 0*/
						mc_db[p_item->grp.group_id].dst[dev_idx].sess_id = session_id + 1;
						mc_db[p_item->grp.group_id].dst[dev_idx].sess_hash.h1 = mc_args->hwhash.h1;
						mc_db[p_item->grp.group_id].dst[dev_idx].sess_hash.h2 = mc_args->hwhash.h2;
						spin_unlock_bh(&g_hal_mc_db_lock);
						dbg("Added client session for index=%d session_id=%d\n",dev_idx, session_id + 1);
					}
				}
			}
		} else {
			dbg("Device index %d not valid for multicast group %d\n", dev_idx, mc_args->groupid);
		}
	} else if (!p_item->mc_entry) {
		/*It is multicast group session*/
		/* In case of multicast group session the egress gpid must be set as ingress gpid for the uC to identify correct IGP*/
		/*pp_args.eg_port = mc_args->ig_gpid;*/
		pp_args.eg_port = g_mcast_nf.gpid;

		/*egress qid static qid allocated during mc NF creation*/
		pp_args.dst_q = g_mcast_nf.qid;

		/*Filling in the FV information*/
		pp_args.rx = mc_args->desc->rx;
		pp_args.tx = mc_args->desc->tx;
		/*End of FV information*/

		/*Set the egress UD parameters */
		pp_args.ud_sz = 0;

		/* multicast group id bit[0-8] */
		pp_args.ps = p_item->grp.group_id & 0x1FF;

		/* multicast NF subif id [15:9]*/
		pp_args.ps |= (g_mcast_nf.subif & 0x7F) << 9;

		pp_args.flags |= PP_SESS_FLAG_PS_VALID_MSK;

		pp_args.mcast.grp_idx = p_item->grp.group_id;
		/*Set the session flags */
		pp_args.flags |= PP_SESS_FLAG_MCAST_GRP_MSK;

		/*add hardware session */
		if ((ret = pp_session_create(&pp_args, &session_id, NULL))) {
			dbg("pp_session_create returned failure!! %s %d ret=%d\n", __FUNCTION__, __LINE__, ret);
		} else {
			/* Multicast group session */
			if (is_valid_session(session_id + 1)) {
				/*always stored +1 to handle index 0*/
				mc_route->p_entry = p_item->mc_entry = session_id + 1;
				spin_lock_bh(&g_hal_mc_db_lock);
				mc_db[p_item->grp.group_id].used = 1;
				mc_db[p_item->grp.group_id].grp_sess_hash.h1 = mc_args->hwhash.h1;
				mc_db[p_item->grp.group_id].grp_sess_hash.h2 = mc_args->hwhash.h2;
				spin_unlock_bh(&g_hal_mc_db_lock);
				dbg("Added group session session_id=%d\n", session_id + 1);
			}
		}
	} else {
	/*If the session exists and hw ahsh is not matching.. delete the session*/
		spin_lock_bh(&g_hal_mc_db_lock);
		if (mc_db[p_item->grp.group_id].used
			&& ((mc_db[p_item->grp.group_id].grp_sess_hash.h1
			!= mc_args->hwhash.h1) ||
			(mc_db[p_item->grp.group_id].grp_sess_hash.h2
			!= mc_args->hwhash.h2))) {
			/* delete all the dst sessions */
			for (i = 0; i < MAX_MC_CLIENT_PER_GRP; i++) {
				if (is_valid_session(mc_db[p_item->grp.group_id].dst[i].sess_id)) {
					/*1. Delete all the group member PPv4sessions*/
					/* session_id always stored +1 to handle valid sessionid 0*/
					if ((ret = pp_session_delete(mc_db[p_item->grp.group_id].dst[i].sess_id - 1, NULL))) {
						dbg("pp_session delete multicast client session returned Error:%d\n", ret);
					}
				}
				mc_db[p_item->grp.group_id].dst[i].sess_id = 0;
				mc_db[p_item->grp.group_id].dst[i].sess_hash.h1 = 0;
				mc_db[p_item->grp.group_id].dst[i].sess_hash.h2 = 0;
			}
			/* delete the grp sessions */
			if ((ret = pp_session_delete(mc_route->p_entry - 1, NULL))) {
				dbg("pp_session delete mcast grp sess ret Error:%d\n", ret);
			}
			p_item->mc_entry = mc_route->p_entry = 0;
			/* reset the db entry */
			mc_db[p_item->grp.group_id].used = 0;
			mc_db[p_item->grp.group_id].grp_sess_hash.h1 = 0;
			mc_db[p_item->grp.group_id].grp_sess_hash.h2 = 0;
		} else {
			dbg("MC group session exists\n");
		}
		spin_unlock_bh(&g_hal_mc_db_lock);
	}

	return ret;
}

void get_itf_mib(uint32_t itf, struct ppe_itf_mib *p)
{
}

/*!
	\fn uint32_t get_routing_entry_bytes(uint32_t session_index, uint32_t *f_hit, uint32_t reset_flag)
	\brief get one routing entry's byte counter
	\param entry	entry number got from function call "add_routing_entry"
	\param f_hit hit status
	\param count counter value
	\return error code from switch API
 */
int32_t get_routing_entry_bytes(uint32_t session_id, uint8_t *f_hit, uint64_t *bytes, uint64_t *packets, uint8_t flag)
{
	int ret = PPA_SUCCESS;

	spin_lock_bh(&g_hal_db_lock);
	if ((session_id >= 0) && (session_id < g_max_hw_sessions) && (pp_hal_db[session_id].used)) {
		*f_hit = 1;
		if (flag) {
			/* if the flag is set when the hw needs to be read immidiately */
			/* when the sessions is about to get removed from hardware */
			ret = pp_session_stats_get(session_id, &pp_hal_db[session_id].stats);
		}
		if (ret==PPA_SUCCESS) {
			*bytes = pp_hal_db[session_id].stats.bytes;
			*packets = pp_hal_db[session_id].stats.packets;
		}
	} else {
		ret = PPA_FAILURE;
	}
	spin_unlock_bh(&g_hal_db_lock);
	return ret;
}

static inline uint32_t ppa_drv_get_phys_port_num(void)
{
	return MAX_LGM_PORTS;
}

/* All the capabilities currently supported	are hardcoded
// register all the capabilities supported by PPV4 HAL*/
static int32_t lgm_pp_hal_register_caps(void)
{
	int32_t res = PPA_SUCCESS;

	if ((res = ppa_drv_register_cap(SESS_IPV4, 1, PPV4_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability SESS_IPV4!!!\n");
		goto PP_HAL_FAIL;
	}

	if ((res = ppa_drv_register_cap(SESS_IPV6, 1, PPV4_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability SESS_IPV6!!!\n");
		goto PP_HAL_FAIL;
	}

	if ((res = ppa_drv_register_cap(SESS_MC_DS, 1, PPV4_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability SESS_MC_DS!!!\n");
		goto PP_HAL_FAIL;
	}

	if ((res = ppa_drv_register_cap(TUNNEL_6RD, 1, PPV4_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_6RD!!!\n");
		goto PP_HAL_FAIL;
	}

	if ((res = ppa_drv_register_cap(TUNNEL_DSLITE, 1, PPV4_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_DSLITE!!!\n");
		goto PP_HAL_FAIL;
	}

	if ((res = ppa_drv_register_cap(TUNNEL_L2TP_US, 1, PPV4_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_L2TP_US!!!\n");
		goto PP_HAL_FAIL;
	}

	if ((res = ppa_drv_register_cap(TUNNEL_L2TP_DS, 1, PPV4_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_L2TP_DS!!!\n");
		goto PP_HAL_FAIL;
	}

	if ((res = ppa_drv_register_cap(TUNNEL_GRE_US, 1, PPV4_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_GRE_US!!\n");
		goto PP_HAL_FAIL;
	}

	if ((res = ppa_drv_register_cap(TUNNEL_GRE_DS, 1, PPV4_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_GRE_DS!!\n");
		goto PP_HAL_FAIL;
	}

	if ((res = ppa_drv_register_cap(SESS_LOCAL_IN, 1, PPV4_HAL)) != PPA_SUCCESS) {
		pr_err("ppa_drv_register_cap returned failure for capability SESS_LOCAL_IN!!!\n");
		return res;
	}

	if ((res = ppa_drv_register_cap(SESS_LOCAL_OUT, 1, PPV4_HAL)) != PPA_SUCCESS) {
		pr_err("ppa_drv_register_cap returned failure for capability SESS_LOCAL_OUT!!!\n");
		return res;
	}

#if IS_ENABLED(CONFIG_LGM_TOE)
	/*LRO table init */
	init_lro_table();
#endif /*IS_ENABLED(CONFIG_LGM_TOE)*/

	return res;

PP_HAL_FAIL:
	ppa_drv_deregister_cap(SESS_IPV4,PPV4_HAL);
	ppa_drv_deregister_cap(SESS_IPV6,PPV4_HAL);
	ppa_drv_deregister_cap(SESS_MC_DS,PPV4_HAL);
	ppa_drv_deregister_cap(TUNNEL_6RD,PPV4_HAL);
	ppa_drv_deregister_cap(TUNNEL_DSLITE,PPV4_HAL);
	ppa_drv_deregister_cap(TUNNEL_L2TP_DS,PPV4_HAL);
	ppa_drv_deregister_cap(TUNNEL_GRE_DS,PPV4_HAL);
	return res;
}

static int32_t lgm_pp_hal_deregister_caps(void)
{
	ppa_drv_deregister_cap(SESS_BRIDG,PPV4_HAL);
	ppa_drv_deregister_cap(SESS_IPV4,PPV4_HAL);
	ppa_drv_deregister_cap(SESS_IPV6,PPV4_HAL);
	ppa_drv_deregister_cap(SESS_MC_DS,PPV4_HAL);
	ppa_drv_deregister_cap(TUNNEL_6RD,PPV4_HAL);
	ppa_drv_deregister_cap(TUNNEL_DSLITE,PPV4_HAL);
	ppa_drv_deregister_cap(TUNNEL_L2TP_DS,PPV4_HAL);
	ppa_drv_deregister_cap(QOS_CLASSIFY,PPV4_HAL);
	ppa_drv_deregister_cap(TUNNEL_GRE_DS,PPV4_HAL);

	return PPA_SUCCESS;
}

static int32_t lgm_pp_hal_generic_hook(PPA_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag)
{
	/*dbg("lgm_pp_hal_generic_hook cmd 0x%x_%s\n", cmd, ENUM_STRING(cmd) );*/
	switch (cmd) {
	case PPA_GENERIC_HAL_GET_PORT_MIB: {
			int i=0;
			int num;
			PPA_PORT_MIB *mib = (PPA_PORT_MIB*) buffer;
			num = NUM_ENTITY(mib->mib_info) > ppa_drv_get_phys_port_num() ? ppa_drv_get_phys_port_num() : NUM_ENTITY(mib->mib_info) ;
			for (i = 0; i < num; i++) {
			/* port mib needs to be read from dp library ?? or PP ?? */
			/*
				mib->mib_info[i].ig_fast_rt_ipv4_udp_pkts = pae_port_mib.nRxUCv4UDPPktsCount;
				mib->mib_info[i].ig_fast_rt_ipv4_tcp_pkts = pae_port_mib.nRxUCv4TCPPktsCount;
				mib->mib_info[i].ig_fast_rt_ipv4_mc_pkts = pae_port_mib.nRxMCv4PktsCount;
				mib->mib_info[i].ig_fast_rt_ipv4_bytes = pae_port_mib.nRxIPv4BytesCount;
				mib->mib_info[i].ig_fast_rt_ipv6_udp_pkts = pae_port_mib.nRxUCv6UDPPktsCount;
				mib->mib_info[i].ig_fast_rt_ipv6_tcp_pkts = pae_port_mib.nRxUCv6TCPPktsCount;
				mib->mib_info[i].ig_fast_rt_ipv6_mc_pkts = pae_port_mib.nRxMCv6PktsCount;
				mib->mib_info[i].ig_fast_rt_ipv6_bytes = pae_port_mib.nRxIPv6BytesCount;
				mib->mib_info[i].ig_cpu_pkts = pae_port_mib.nRxCpuPktsCount;
				mib->mib_info[i].ig_cpu_bytes = pae_port_mib.nRxCpuBytesCount;
				mib->mib_info[i].ig_drop_pkts = pae_port_mib.nRxPktsDropCount;
				mib->mib_info[i].ig_drop_bytes = pae_port_mib.nRxBytesDropCount;
				mib->mib_info[i].eg_fast_pkts = pae_port_mib.nTxPktsCount;
				mib->mib_info[i].eg_fast_bytes = pae_port_mib.nTxBytesCount;
			*/
				if ((i >= 1) && (i <= 6))
					mib->mib_info[i].port_flag = PPA_PORT_MODE_ETH;
				else if (i == 13)
					mib->mib_info[i].port_flag = PPA_PORT_MODE_DSL;
				else if (i == 0)	/* 0 is CPU port*/
					mib->mib_info[i].port_flag = PPA_PORT_MODE_CPU;
				else
					mib->mib_info[i].port_flag = PPA_PORT_MODE_EXT;
			}
			mib->port_num = num;
			/*dbg("port_num=%d\n", mib->port_num);*/
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_SET_DEBUG:	{

			lgm_pp_hal_dbg_enable = ((PPA_CMD_GENERAL_ENABLE_INFO*)buffer)->enable;
			dbg("Set lgm_pp_hal_dbg_enable to 0x%x\n", lgm_pp_hal_dbg_enable );

			return PPA_SUCCESS;
		}
	 case PPA_GENERIC_HAL_GET_MAX_ENTRIES: {

			PPA_MAX_ENTRY_INFO *entry = (PPA_MAX_ENTRY_INFO *)buffer;
			entry->max_lan_entries = g_max_hw_sessions;
			entry->max_wan_entries = g_max_hw_sessions;
			entry->max_mc_entries = MAX_MC_GROUP_ENTRIES;

			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_GET_HAL_VERSION: {

			PPA_VERSION *v = (PPA_VERSION *)buffer;
			get_lgm_pp_hal_id(&v->family, &v->type, &v->itf, &v->mode, &v->major, &v->mid, &v->minor);

			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_GET_PPE_FW_VERSION: {

			PPA_VERSION *v = (PPA_VERSION *)buffer;
			get_lgm_pp_hal_id(&v->family, &v->type, &v->itf, &v->mode, &v->major, &v->mid, &v->minor);

			return get_firmware_id(&v->id, v->name, v->version);
		}
	case PPA_GENERIC_HAL_GET_PHYS_PORT_NUM: {

			PPA_COUNT_CFG *count = (PPA_COUNT_CFG *)buffer;
			count->num = get_number_of_phys_port();

			return PPA_SUCCESS;
		 }
	case PPA_GENERIC_HAL_GET_PHYS_PORT_INFO: {

			PPE_IFINFO *info = (PPE_IFINFO *) buffer;
			get_phys_port_info(info->port, &info->if_flags, info->ifname);

			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_SET_ROUT_CFG:
	case PPA_GENERIC_HAL_SET_BRDG_CFG: {

			/* not supported */
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_SET_ACC_ENABLE: {

			/*Enable/disable upstream/downstream acceleration */
			PPA_ACC_ENABLE *cfg = (PPA_ACC_ENABLE *)buffer;
			set_acc_mode(cfg->f_is_lan, cfg->f_enable);

			return PPA_SUCCESS;
		}
	 case PPA_GENERIC_HAL_GET_ACC_ENABLE: {

			PPA_ACC_ENABLE *cfg = (PPA_ACC_ENABLE *)buffer;
			get_acc_mode(cfg->f_is_lan, &cfg->f_enable);

			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_GET_IPV6_FLAG: {

			/*Always returns enabled*/
			return PPA_ENABLED;
		}
	case PPA_GENERIC_HAL_ADD_COMPLEMENT_ENTRY:
	case PPA_GENERIC_HAL_DEL_COMPLEMENT_ENTRY:
	case PPA_GENERIC_HAL_UPDATE_SESS_META:
	case PPA_GENERIC_HAL_CLEAR_SESS_META: {

			/* No special metadate needed for PPv4 HAL*/
			if (g_us_accel_enabled || g_ds_accel_enabled)
				return PPA_SUCCESS;
			else
				return PPA_FAILURE;
		}
	case PPA_GENERIC_HAL_ADD_ROUTE_ENTRY: {

			PPA_ROUTING_INFO *route = (PPA_ROUTING_INFO *)buffer;
			struct uc_session_node *p_item = (struct uc_session_node *)route->p_item;
			if (!p_item)
				return PPA_FAILURE;
			if (is_lansession(p_item->flags)) {
				if (!g_us_accel_enabled) {
					dbg("\n PPv4 HAL US Acceleration is disabled!!! \n");
					return PPA_FAILURE;
				}
			} else {
				if (!g_ds_accel_enabled) {
					dbg("\n PPv4 HAL DS Acceleration is disabled!!! \n");
					return PPA_FAILURE;
				}
			}
			return add_routing_entry(route);
		}
	case PPA_GENERIC_HAL_DEL_ROUTE_ENTRY: {

			PPA_ROUTING_INFO *route = (PPA_ROUTING_INFO *)buffer;
			while ((del_routing_entry(route)) < 0) {
				return PPA_FAILURE;
			}
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_ADD_MC_ENTRY:
	case PPA_GENERIC_HAL_UPDATE_MC_ENTRY: {

			PPA_MC_INFO *mc = (PPA_MC_INFO *)buffer;
			return update_wan_mc_entry(mc);
		}
	case PPA_GENERIC_HAL_DEL_MC_ENTRY: {

			PPA_MC_INFO *mc = (PPA_MC_INFO *)buffer;
			del_wan_mc_entry(mc);
			return PPA_SUCCESS;
		}
	 case PPA_GENERIC_HAL_GET_ROUTE_ACC_BYTES: {

			PPA_ROUTING_INFO *route = (PPA_ROUTING_INFO *)buffer;

			get_routing_entry_bytes(route->entry, &route->f_hit, &route->bytes, &route->packets, flag);
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_GET_MC_ACC_BYTES: {

			PPA_MC_INFO *mc = (PPA_MC_INFO *)buffer;

			get_routing_entry_bytes(mc->p_entry, &mc->f_hit, &mc->bytes, &mc->packets, flag);

			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_GET_ITF_MIB: {

			PPE_ITF_MIB_INFO *mib = (PPE_ITF_MIB_INFO *)buffer;
			get_itf_mib( mib->itf, &mib->mib);
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_INIT: {

			PPA_HAL_INIT_CFG *cfg = (PPA_HAL_INIT_CFG*)buffer;
			del_routing_session_cb = cfg->del_cb;

			return lgm_pp_hal_register_caps();
		}
	case PPA_GENERIC_HAL_EXIT: {

			del_routing_session_cb = NULL;
			return lgm_pp_hal_deregister_caps();
		}
	default:
		return PPA_FAILURE;
	}
	return PPA_FAILURE;
}

/**************************************************************************************************
* Each entry in the inactivity list is being inactive in HW for more than 420 seconds.
* For each entry in the inactivity list
* Call the ppa callback for hw session delete.
* it will internally call the hal session delete entry
***************************************************************************************************/
void pphal_session_inact_cb(struct pp_cb_args *args)
{
	struct pp_inactive_list_cb_args *inact_args;
	uint32_t i, session_id = 0;
	uint32_t num_open_sess = 0;

	dbg("%s invoked\n",__FUNCTION__);

	if (args->ev != PP_INACTIVE_LIST) {
		dbg("%s %d empty list\n", __FUNCTION__, __LINE__);
		return;
	}

	inact_args = container_of(args, struct pp_inactive_list_cb_args, base);
	pp_open_sessions_get(&num_open_sess);

	if (inact_args->base.ret) {
		dbg("failed to get inactive sessions list %d\n", inact_args->base.ret);
		goto done;
	}

	dbg("%s %d inact_args->n_sessions=%d\n", __FUNCTION__, __LINE__, inact_args->n_sessions);

	/*for each entry in the pp hal_db update the inactiviry status*/
	for (i = 0; i < inact_args->n_sessions; i++) {
		session_id = inact_args->inact_sess[i];
		dbg("%s %d inact_args->inact_sess[i]=%d pp_hal_db[%d].used=%d\n", __FUNCTION__, __LINE__,
				session_id, session_id, pp_hal_db[session_id].used);
		if (((session_id >= 0) && (session_id < g_max_hw_sessions)) && pp_hal_db[session_id].used) {
			/* lro sessions are limited to 256 and inactive-scan timeout_thr is 32k,
			   hence lro sessions need early delete to avoid lro table getting exhausted */
			if (!pp_hal_db[session_id].lro && (num_open_sess < g_sess_timeout_thr))
				continue;
			dbg("%s %d deleting %d p_item=%px p_item->sessiond=%d\n",
					__FUNCTION__, __LINE__, session_id, pp_hal_db[session_id].node,
					((struct uc_session_node *)pp_hal_db[session_id].node)->routing_entry);
			del_routing_session_cb(pp_hal_db[session_id].node);
		}
	}

done:
	/*free the inact_list after processing*/
	if (inact_args->inact_sess)
		ppa_free(inact_args->inact_sess);
}

/* work queue handler thread function */
static void pphal_get_stats_and_inactive_sessions(struct work_struct *w)
{
#ifndef PP_INACTIVE_SCAN
	struct pp_stats tmp, *stats;
	struct pp_inactive_list_cb_args args = { 0 };
	uint32_t i, inact_idx = 0, max_idx;
#else
	struct pp_request req;
	int32_t ret=0;
#endif
	uint32_t *inact_list = NULL;
	uint32_t arr_sz = 0;

	arr_sz     = min_t(uint32_t, g_max_hw_sessions, DEFAULT_DB_SLICE);
#ifndef PP_INACTIVE_SCAN
	inact_list = (uint32_t *)ppa_malloc(sizeof(uint32_t) * arr_sz);
	if (!inact_list)
		pr_err("Failed to allocate memory for %u, inactive sessions array\n",
				arr_sz);
	max_idx = min_t(uint32_t, g_db_index + DEFAULT_DB_SLICE, g_max_hw_sessions);

	/*Iterate through maximum 1K entries in the HW */
	for (i = g_db_index; i < max_idx; i++) {
		if (!pp_hal_db[i].used)
			continue;

		/* The entry is valid read the statistics from HW */
		stats = &pp_hal_db[i].stats;

		spin_lock_bh(&g_hal_db_lock);
		ppa_memcpy(&tmp, stats, sizeof(tmp));
		pp_session_stats_get(i, stats);
		if (inact_list && !ppa_memcmp(&tmp, stats, sizeof(tmp)))
			inact_list[inact_idx++] = i;

		spin_unlock_bh(&g_hal_db_lock);
	}

	if (max_idx == g_max_hw_sessions)
		g_db_index = 0;
	else
		g_db_index += DEFAULT_DB_SLICE;

	if (inact_list) {
		if (inact_idx) {
			dbg("deleting %u sessions\n", inact_idx);
			args.inact_sess  = inact_list;
			args.n_sessions  = inact_idx;
			args.base.ret    = 0;
			args.base.req_id = get_jiffies_64();
			args.base.ev     = PP_INACTIVE_LIST;
			pphal_session_inact_cb(&args.base);
		} else {
			ppa_free(inact_list);
		}
	}
#else
	/* the below code should be used when the PP crawler will be enabled again */
	g_iter_cnt++;

	if ((g_iter_cnt * DEFAULT_DB_SLICE) >= MAX_UC_SESSION_ENTRIES) { /*4x64=256 seconds completed */
		dbg("%s %d starting crawler\n",__FUNCTION__, __LINE__);
		/*start the crawler*/
		g_iter_cnt = 0;
		arr_sz = (g_max_hw_sessions < DEFAULT_INACT_ARRAY_SIZE) ? g_max_hw_sessions : DEFAULT_INACT_ARRAY_SIZE;

		/*Call the pp crawler to get all the inactive sessions*/
		req.cb = pphal_session_inact_cb;
		req.req_prio = 0;
		req.req_id = get_jiffies_64();

		inact_list = (uint32_t *)ppa_malloc(sizeof(uint32_t) * arr_sz);
		if (inact_list) {
			dbg("%s %d calling pp_inactive_sessions_get arr_sz=%d\n",__FUNCTION__, __LINE__, arr_sz);
			ret = pp_inactive_sessions_get(&req, inact_list, arr_sz);
			if (ret) {
				ppa_free(inact_list);
			}
		}
	}
#endif
}

/* Declare work queue handler thread */
static DECLARE_DELAYED_WORK(stats_n_gc_thread, pphal_get_stats_and_inactive_sessions);

/**************************************************************************************************
* This function is invoked by the timer at an interval of 4 seconds.
* Each time it goes through a maximum of 1K entries in HW and reads the session statistics.
* It takes 4X64 = 256 Seconds (4.3 minutes to complete one pass through all the 64 k sessions).
* After collecting the statistics of all sessions it triggers HW thread (crawler) for getting incative sessions.
* Crawler will complete its action in less than 500ms and return a set of inactive sessions.
* The crawler will call callback function with a set of inactive sessions.
* One cycle of these operations is expected to complete in 5min in worst case (all hw sessions are full).
***************************************************************************************************/
static enum hrtimer_restart pphal_get_stats_and_inactive_sessions_timer(struct hrtimer *hrtimer)
{

	/* schedule work queue */
	if (g_workq)
		queue_delayed_work(g_workq, &stats_n_gc_thread, 0);

	/*re start timer*/
	ppa_hrt_forward(hrtimer, DEFAULT_POLLING_TIME);
	return HRTIMER_RESTART;
}

/*
 * ####################################
 *		 Init/Cleanup API
 * ####################################
 */
static inline void hal_init(void)
{
	int32_t ret=0;
	u32 flags = WQ_MEM_RECLAIM | WQ_UNBOUND;
	/* register callback with the hal selector*/
	ppa_drv_generic_hal_register(PPV4_HAL, lgm_pp_hal_generic_hook);

	ret = pp_max_sessions_get(&g_max_hw_sessions);
	if (unlikely(ret))
		return;

	/* delete timed out sessions only in case we have at least 50% used */
	g_sess_timeout_thr = g_max_hw_sessions / 2;

	/*initialize the HAL DB*/
	pp_hal_db = (PP_HAL_DB_NODE *) ppa_malloc(sizeof(PP_HAL_DB_NODE) * g_max_hw_sessions);
	if (!pp_hal_db) {
		pr_err("Failed to allocate hal db\n");
		return;
	}
	ppa_memset(pp_hal_db, 0, sizeof(PP_HAL_DB_NODE) * g_max_hw_sessions);

	/*initalize the HAL MC DB*/
	ppa_memset(mc_db, 0, sizeof(MC_DB_NODE) * MAX_MC_GROUP_ENTRIES);

	/*Initialize the MC NF and Register the MC callback*/
	init_mc_nf();

	/*Initialize the Fragmenter NF*/
	init_frag_nf();

#if IS_ENABLED(LITEPATH_HW_OFFLOAD)
	/*Initialize Application litepath offload*/
	init_app_lp();
#endif /*IS_ENABLED(LITEPATH_HW_OFFLOAD)*/

#if IS_ENABLED(CONFIG_INTEL_VPN)
	/*Initialize VPN flow*/
	init_vpn_offload();
#endif /*IS_ENABLED(CONFIG_INTEL_VPN)*/

	/*Init the lock*/
	spin_lock_init(&g_hal_db_lock);
	/*Init the mc db lock*/
	spin_lock_init(&g_hal_mc_db_lock);

	/*initialize workq*/
	g_workq = alloc_workqueue("%s", flags, 2, "PP_HAL wq");
	if (!g_workq) {
		pr_err("Failed to create work queue");
	}

	/*init the timer*/
	ppa_hrt_init(&g_gc_timer, pphal_get_stats_and_inactive_sessions_timer);
	/*start timer*/
	ppa_hrt_start(&g_gc_timer, DEFAULT_POLLING_TIME);
}

static inline void hal_exit(void)
{
	if (g_workq)
		destroy_workqueue(g_workq);

	uninit_frag_nf();
	uninit_mc_nf();

	ppa_drv_generic_hal_deregister(PPV4_HAL);
}

static int __init lgm_pp_hal_init(void)
{
	hal_init();
#if defined(PPA_API_PROC)
	ppv4_proc_file_create();
#endif
	printk(KERN_INFO"lgm_pp_hal loaded successfully MAX_HW_SESSIONS=%d\n", g_max_hw_sessions);
	return 0;
}

static void __exit lgm_pp_hal_exit(void)
{
#if defined(PPA_API_PROC)
	ppv4_proc_file_remove();
#endif
	hal_exit();
}

module_init(lgm_pp_hal_init);
module_exit(lgm_pp_hal_exit);
MODULE_LICENSE("GPL");
