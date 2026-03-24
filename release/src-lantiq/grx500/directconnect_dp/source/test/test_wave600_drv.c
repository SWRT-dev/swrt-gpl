/*
* ####################################
*              Head File

cd /proc/net/testwlan

#create testwlan devices
echo add > dev

#dynamically register testwlan1 to datapath
echo register dev   testwlan1 1 DIRECTPATH > directpath
echo register subif testwlan1 > directpath

#dynamically register testwlan2 to datapath
echo register dev   testwlan2 2 DIRECTPATH > directpath
echo register subif testwlan2 > directpath

#dynamically register testwlan3 to datapath
echo register dev   testwlan3 3 DIRECTPATH > directpath
echo register subif testwlan3 > directpath

#register testwlan4 to datapath with specified port_id 12
echo register dev   testwlan4 4 DIRECTPATH 12 > directpath
echo register subif testwlan4 > directpath
echo register subif testwlan5 12 > directpath

#unregister
echo unregister subif testwlan5  > directpath
echo unregister subif testwlan4  > directpath
echo unregister subif testwlan4  > directpath
* ####################################
*/

/*  Common Head File*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/atmdev.h>
#include <linux/init.h>
#include <linux/etherdevice.h>	/*  eth_type_trans  */
#include <linux/ethtool.h>	/*  ethtool_cmd     */
#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/inetdevice.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/icmp.h>
#include <net/tcp.h>
#include <linux/uaccess.h>
#include <asm/unistd.h>
#include <asm/irq.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <asm/checksum.h>
#include <linux/errno.h>
#ifdef CONFIG_XFRM
#include <net/xfrm.h>
#endif
#if defined(CONFIG_SOC_GRX500)
#include <linux/dma/lantiq_dmax.h>
#endif

//#define CONFIG_DIRECTCONNECT_DP_API 1 // FIXME
#define CONFIG_WAVE600_USING_DP_API 0
//#define PUMA_DC_MODE1

//#include <net/ppa_stack_al.h>
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
#include <net/directconnect_dp_api.h>
#elif (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
#include <net/datapath_api.h>
#endif
//#include <net/datapath_proc_api.h>
//#include "../datapath/datapath.h"
#include "test_wlan_proc_api.h"
#include "test_wlan_drv.h"

/*####################################
*              Definition
* ####################################
*/

#define ENABLE_DEBUG                            1

#define ENABLE_ASSERT                           1

#define DEBUG_DUMP_SKB                          1

#if defined(ENABLE_DEBUG) && ENABLE_DEBUG
#define ENABLE_DEBUG_PRINT                    1
#define DISABLE_INLINE                        1
#else
#define ENABLE_DEBUG_PRINT                    0
#define DISABLE_INLINE                        0
#endif

#if !defined(DISABLE_INLINE) || !DISABLE_INLINE
#define INLINE                                inline
#else
#define INLINE
#endif

#define err(format, arg...)    do {;\
	PRINTK(KERN_ERR __FILE__ ":%d:%s: " format "\n", \
		__LINE__, __FUNCTION__, ##arg); } \
		while (0)

#if defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT
#undef  dbg
#define dbg(format, arg...) do { if ((g_dbg_enable &\
	DBG_ENABLE_MASK_DEBUG_PRINT)) \
		PRINTK(KERN_WARNING __FILE__ ":%d:%s: " format "\n",\
			__LINE__, __FUNCTION__, ##arg); } \
	while (0)
#else
#if !defined(dbg)
#define dbg(format, arg...)
#endif
#endif

#if defined(ENABLE_ASSERT) && ENABLE_ASSERT
#define ASSERT(cond, format, arg...)      do {                \
	if ((g_dbg_enable & DBG_ENABLE_MASK_ASSERT) && !(cond)) \
		PRINTK(KERN_ERR __FILE__ ":%d:%s: " format "\n",      \
			__LINE__, __FUNCTION__, ##arg); } \
		while (0)
#else
#define ASSERT(cond, format, arg...)
#endif

#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
#define DUMP_SKB_LEN                          ~0
#endif

#if (defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB)                     \
|| (defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT)          \
|| (defined(ENABLE_ASSERT) && ENABLE_ASSERT)
#define ENABLE_DBG_PROC                       1
#else
#define ENABLE_DBG_PROC                       0
#endif

#define ENABLE_TXDBG							0

/* Debug Print Mask*/
#define DBG_ENABLE_MASK_ERR                     0x0001
#define DBG_ENABLE_MASK_DEBUG_PRINT             0x0002
#define DBG_ENABLE_MASK_ASSERT                  0x0004
#define DBG_ENABLE_MASK_DUMP_SKB_RX             0x0008
#define DBG_ENABLE_MASK_DUMP_SKB_TX             0x0010
#define DBG_ENABLE_MASK_ALL     (DBG_ENABLE_MASK_ERR | \
							DBG_ENABLE_MASK_DEBUG_PRINT |\
							DBG_ENABLE_MASK_ASSERT |\
							DBG_ENABLE_MASK_DUMP_SKB_RX |\
							DBG_ENABLE_MASK_DUMP_SKB_TX)

/* Constant Definition*/
#define ETH_WATCHDOG_TIMEOUT                    (10 * HZ)
#define MAX_RX_QUEUE_LENGTH                     100
#define TASKLET_HANDLE_BUDGET                   25

/* Ethernet Frame Definitions*/
#define ETH_CRC_LENGTH                          4
#define ETH_MAX_DATA_LENGTH                     ETH_DATA_LEN
#define ETH_MIN_TX_PACKET_LENGTH                ETH_ZLEN

/* ####################################
*              Data Type
* ####################################
*/

/* Internal Structure of Devices (ETH/ATM)*/
#define TESTWLAN_F_FREE           0
#define TESTWLAN_F_REGISTER_DEV   1
#define TESTWLAN_F_REGISTER_SUBIF 2

struct test_wlan_priv_data {
	int id;
	struct net_device_stats stats;
	unsigned int rx_preprocess_drop;
	struct sk_buff_head rx_queue;
	struct tasklet_struct rx_tasklet;
	int f_tx_queue_stopped;
	unsigned char dev_addr[MAX_ADDR_LEN];
	unsigned int dp_pkts_to_ppe;
	unsigned int dp_pkts_to_ppe_fail;
	unsigned int dp_pkts_from_ppe;
	unsigned int dp_pkts_tx;

#if CONFIG_WAVE600_USING_DP_API
	dp_cb_t dp_cb;
#endif
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
	struct dc_dp_cb wave500_cb;
	struct dc_dp_dccntr dccntr;
	struct dc_dp_dev devspec;
	struct dc_dp_res resources;
#endif

	struct module *owner;
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
	dp_subif_t dp_subif;
#endif
	int32_t dev_port;	/*dev  instance */
	//int32_t flag_index;
	uint8_t flag_str[64];
	int32_t f_dp;		/* status for register to datapath */
};

int32_t g_resvd_port = -1;
int32_t g_wave6x_radio_num = 0;

static const char *const dbg_enable_mask_str[] = {
	"err",			/*DBG_ENABLE_MASK_ERR */
	"dbg",			/*DBG_ENABLE_MASK_DEBUG_PRINT */
	"assert",		/*DBG_ENABLE_MASK_ASSERT */
	"rx",			/*DBG_ENABLE_MASK_DUMP_SKB_RX */
	"tx"			/*DBG_ENABLE_MASK_DUMP_SKB_TX */
};

/*####################################
*            Local Variable
* ####################################
*/

#define MAX_TESTWLAN_NUM ((PMAC_MAX_NUM*MAX_SUBIF_PER_PORT)+2)	/* 16 PMAC and each port support 16 subif */
static struct net_device *g_test_wlan_dev[MAX_TESTWLAN_NUM] = { 0 };

static struct module g_test_wlan_module[MAX_TESTWLAN_NUM];

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
static int g_dbg_enable = DBG_ENABLE_MASK_ERR | DBG_ENABLE_MASK_ASSERT | DBG_ENABLE_MASK_DUMP_SKB_RX;
#endif

/* Network Operations*/
static void eth_setup(struct net_device *);
static struct net_device_stats *eth_get_stats(struct net_device *);
static int eth_open(struct net_device *);
static int eth_stop(struct net_device *);
static int eth_hard_start_xmit(struct sk_buff *, struct net_device *);
static int eth_ioctl(struct net_device *, struct ifreq *, int);
static void eth_tx_timeout(struct net_device *);

/* skb management functions */
static struct sk_buff *skb_break_away_from_protocol(struct sk_buff *);

/* RX path functions*/
static INLINE int eth_rx_preprocess(struct sk_buff *, int);
static INLINE void eth_rx_handler(struct sk_buff *, int);
static void do_test_wlan_rx_tasklet(unsigned long);

/* Datapath directpath functions*/
static int32_t dp_fp_stop_tx(struct net_device *);
static int32_t dp_fp_restart_tx(struct net_device *);
static int32_t dp_fp_rx(struct net_device *, struct net_device *,
			struct dp_subif *subif, struct sk_buff **, int32_t, uint32_t);
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
static int32_t dp_fp_get_staid(char *, uint32_t *);
#ifdef PUMA_DC_MODE1
static int32_t dp_fp_get_desc_info(int32_t, struct sk_buff *,
				   struct dc_dp_fields_value_dw *, uint32_t);
#endif
#endif

static void proc_read_ring(struct seq_file *s);
static ssize_t proc_write_ring(struct file *file, const char *buf,
			    size_t count, loff_t *data);
//#if defined(ENABLE_TXDBG) && ENABLE_TXDBG /* FIXME */
static void proc_read_cputx(struct seq_file *s);
static ssize_t proc_write_cputx(struct file *file, const char *buf,
			    size_t count, loff_t *data);

//#endif
/*Debug functions*/
#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
static INLINE void dump_skb(struct sk_buff *, u32, char *, int, int);
static INLINE void dump_skb(struct sk_buff *skb, u32 len, char *title,
			    int ch, int is_tx)
{
	int i, j, l;
	int line_num = 32;
	const unsigned char *p = skb->data;
	int bytes = line_num * 8 + 100;
	char *s;

	if (!p)
		return;
	s = kzalloc(bytes, GFP_ATOMIC);
	if (!s)
		return;
	sprintf(s, "%s in hex at 0x%px (%d bytes)\n",
		title ? (char *)title : "Data", p, skb->len);
	pr_info("%s", s);

	for (i = 0; i < len; i += line_num) {
		l = sprintf(s, " %06x ", i);
		for (j = 0; (j < line_num) && (i + j < len); j++)
			l += sprintf(s + l, "%02x ", p[i + j]);
		sprintf(s + l, "\n");
		pr_info("%s", s);
	}
	kfree(s);
}
#else
static INLINE void dump_skb(struct sk_buff *skb, u32 len, char *title,
			    int ch, int is_tx)
{
}
#endif

static const struct net_device_ops test_wlan_netdev_ops = {
	.ndo_open = eth_open,
	.ndo_stop = eth_stop,
	.ndo_start_xmit = eth_hard_start_xmit,
	.ndo_do_ioctl = eth_ioctl,
	.ndo_tx_timeout = eth_tx_timeout,
	.ndo_get_stats = eth_get_stats,
	.ndo_set_mac_address = eth_mac_addr,
	.ndo_change_mtu = eth_change_mtu,
};

//#if defined(ENABLE_TXDBG) && ENABLE_TXDBG /* FIXME */

#define MAX_SKB_SIZE		1600
#define MAX_PKT_SIZE		1500
#define MIN_PKT_SIZE		60
#define DEFAULT_PKT_SIZE	128
#define MAX_PAUSE_CNT		50
#define DEFAULT_PAUSE_CNT   20

enum {
	TX_STOP = 0,
	TX_START = 1
};

static uint32_t g_tx_count = 1;
static uint32_t g_tx_start = TX_STOP;
static spinlock_t g_tx_start_lock;
static uint32_t g_tx_mib;
static struct sk_buff *g_tx_skb;
static uint32_t g_pkt_size = DEFAULT_PKT_SIZE;
static char g_tx_dev[IFNAMSIZ] = { 0 };

static struct task_struct *g_tx_ts;
static uint32_t g_tx_pause_cnt = 20;

//#endif

/*###################################
*           Global Variable
* ####################################
*/

/*####################################
*             Declaration
* ####################################
*/

/* Wrapper for Different Kernel Version
*/
static inline struct net_device *ltq_dev_get_by_name(const char *name)
{
	return dev_get_by_name(&init_net, name);
}

/*find the testwlan index via its device name
return -1: not found
>=0: index

*/
static int find_testwlan_index_via_name(char *ifname)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++) {
		if (g_test_wlan_dev[i]
		    && ltq_strcmpi(ifname, g_test_wlan_dev[i]->name) == 0) {
			return i;
		}
	}
	return -1;
}

static inline unsigned long ltq_get_xmit_fn(struct net_device *dev)
{
	return (unsigned long)dev->netdev_ops->ndo_start_xmit;
}

/*####################################
*            Local Function
* ####################################
*/

static void eth_setup(struct net_device *dev)
{
	int id = -1;
	struct test_wlan_priv_data *priv = netdev_priv(dev);
	int i;
	for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++)
		if (!g_test_wlan_dev[i]) {
			id = i;
			break;
		}
	if (id < 0)
		return;
	ether_setup(dev);	/*  assign some members */
	dev->netdev_ops = &test_wlan_netdev_ops;
	dev->watchdog_timeo = ETH_WATCHDOG_TIMEOUT;
	dev->dev_addr[0] = 0x00;
	dev->dev_addr[1] = 0x20;
	dev->dev_addr[2] = 0xda;
	dev->dev_addr[3] = 0x86;
	dev->dev_addr[4] = 0x23;
	dev->dev_addr[5] = 0x70 + id;
	priv->id = id;
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
	priv->dp_subif.port_id = -1;
#endif
	skb_queue_head_init(&priv->rx_queue);
	tasklet_init(&priv->rx_tasklet, do_test_wlan_rx_tasklet, 0);
	return;
}

struct net_device_stats *eth_get_stats(struct net_device *dev)
{
	struct test_wlan_priv_data *priv = netdev_priv(dev);
	return &priv->stats;
}

int eth_open(struct net_device *dev)
{
	dbg("open %s", dev->name);
	netif_start_queue(dev);
	return 0;
}

int eth_stop(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}

int eth_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	unsigned long sysflag;
	struct test_wlan_priv_data *priv = netdev_priv(dev);
	int rx_queue_len;
	struct sk_buff *old_skb = skb;
	skb = skb_break_away_from_protocol(skb);
	dev_kfree_skb_any(old_skb);
	if (!skb)
		return 0;
	//dump_skb(skb, skb->len, "test_tx", 0, 1);
	skb->dev = g_test_wlan_dev[priv->id];
	spin_lock_irqsave(&priv->rx_queue.lock, sysflag);
	rx_queue_len = skb_queue_len(&priv->rx_queue);
	if (rx_queue_len < MAX_RX_QUEUE_LENGTH) {
		__skb_queue_tail(&priv->rx_queue, skb);
		if (rx_queue_len == 0)
			tasklet_schedule(&priv->rx_tasklet);
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += skb->len;
	} else if (skb_queue_len(&priv->rx_queue) >= MAX_RX_QUEUE_LENGTH) {
		dev_kfree_skb_any(skb);
		priv->stats.tx_dropped++;
		netif_stop_queue(g_test_wlan_dev[priv->id]);
	//} else {
	//	dev_kfree_skb_any(skb);
	//	priv->stats.tx_dropped++;
	}
	spin_unlock_irqrestore(&priv->rx_queue.lock, sysflag);
	return 0;
}

int eth_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	switch (cmd) {
	default:
		return -EOPNOTSUPP;
	}
	return 0;
}

void eth_tx_timeout(struct net_device *dev)
{
	/*TODO:must restart the TX channels */
	struct test_wlan_priv_data *priv = netdev_priv(dev);
	priv->stats.tx_errors++;
	netif_wake_queue(dev);
	return;
}

struct sk_buff *skb_break_away_from_protocol(struct sk_buff *skb)
{
	struct sk_buff *new_skb;
	if (skb_shared(skb)) {
		new_skb = skb_clone(skb, GFP_ATOMIC);
		if (new_skb == NULL)
			return NULL;
	} else
		new_skb = skb_get(skb);
	skb_dst_drop(new_skb);

#ifdef CONFIG_XFRM
	secpath_put(new_skb->sp);
	new_skb->sp = NULL;
#endif
#if 0 // TODO
#if defined(CONFIG_NETFILTER)
	nf_conntrack_put(new_skb->nfct);
	new_skb->nfct = NULL;
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	nf_conntrack_put_reasm(new_skb->nfct_reasm);
	new_skb->nfct_reasm = NULL;
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	nf_bridge_put(new_skb->nf_bridge);
	new_skb->nf_bridge = NULL;
#endif
#endif
#endif
	return new_skb;
}

/* Ethernet frame types according to RFC 2516 */
#define ETH_PPPOE_DISCOVERY 0x8863
#define ETH_PPPOE_SESSION   0x8864

/* A PPPoE Packet, including Ethernet headers */
typedef struct pppoe_pkt {
#ifdef PACK_BITFIELDS_REVERSED
	unsigned int type:4;	/* PPPoE Type (must be 1) */
	unsigned int ver:4;	/* PPPoE Version (must be 1) */
#else
	unsigned int ver:4;	/* PPPoE Version (must be 1) */
	unsigned int type:4;	/* PPPoE Type (must be 1) */
#endif
	unsigned int code:8;	/* PPPoE code */
	unsigned int session:16;	/* PPPoE session */
	unsigned int length:16;	/* Payload length */
	unsigned char payload[ETH_DATA_LEN];	/* A bit of room to spare, here just for space holder only */
} pppoe_pkt;

/* PPPoE Tag */

static unsigned char ppp_ipv4_proto[2] = {
	0x00, 0x21
};

static unsigned char ppp_ipv6_proto[2] = {
	0x00, 0x57
};

#define VLAN_HEAD_SIZE  4
#define PPPOE_HEAD_SIZE  8

static INLINE int eth_rx_preprocess(struct sk_buff *skb, int id)
{
	unsigned char *p = skb->data;
	unsigned char mac[6];
	unsigned char ip[4];
	unsigned char port[2];
	unsigned char ip_templ[4] = {0};
	struct iphdr *iph;
	struct icmphdr *icmph;
	struct tcphdr *tcph;
	uint32_t t, off_t, *opt;
	int csum;
	pppoe_pkt *pppoe;
	int offset = 0;
	int vlan_num = 0;
	unsigned char *p_new_src_mac;
	static unsigned char zero_mac[4] = {
		0
	};
	struct in_device __rcu *in_dev;
	struct in_ifaddr *if_info;
	__u8 *addr;

	read_lock_bh(&dev_base_lock);
	in_dev = (struct in_device *)skb->dev->ip_ptr;
	if (!in_dev) {
		read_unlock_bh(&dev_base_lock);
		return 0;
	}
	if_info = in_dev->ifa_list;

	if (if_info) {
		memcpy(ip_templ, (char *)&if_info->ifa_address, 4);

		addr = (char *)&if_info->ifa_local;

		PRINTK("Device %s ifa_local: %u.%u.%u.%u\n",
		       skb->dev->name,
		       (__u32) addr[0], (__u32) addr[1],
		       (__u32) addr[2], (__u32) addr[3]);

		addr = (char *)&if_info->ifa_address;
		PRINTK("Device %s ifa_address: %u.%u.%u.%u\n",
		       skb->dev->name,
		       (__u32) addr[0], (__u32) addr[1],
		       (__u32) addr[2], (__u32) addr[3]);

		addr = (char *)&if_info->ifa_mask;
		PRINTK("Device %s ifa_mask: %u.%u.%u.%u\n",
		       skb->dev->name,
		       (__u32) addr[0], (__u32) addr[1],
		       (__u32) addr[2], (__u32) addr[3]);

		addr = (char *)&if_info->ifa_broadcast;
		PRINTK("Device %s ifa_broadcast: %u.%u.%u.%u\n",
		       skb->dev->name, (__u32) addr[0],
		       (__u32) addr[1], (__u32) addr[2], (__u32) addr[3]);
	}
	read_unlock_bh(&dev_base_lock);

	//dump_skb(skb, skb->len, "test_rx", id, 0);
	if (p[offset + 12] == 0x81 && p[offset + 13] == 0x00) {	/*VLAN header */
		offset += VLAN_HEAD_SIZE;
		vlan_num++;
		dbg("Found VLAN%d\n", vlan_num);
	}
	if (p[offset + 12] == 0x88 && p[offset + 13] == 0x63) {
		/*pppoe Discover(0x9)/Offer(0x7)/request(0x19)/Confirm(0x65) */
		return 0;
	}
	if (p[offset + 12] == 0x88 && p[offset + 13] == 0x64) {	/*ppp */
		pppoe = (pppoe_pkt *) (p + offset + 14);
		if ((pppoe->payload[0] == ppp_ipv4_proto[0]
		     && pppoe->payload[1] == ppp_ipv4_proto[1]) /*PPP IPv4 */ ||
		    ((pppoe->payload[0] == ppp_ipv6_proto[0])
		     && (pppoe->payload[1] ==
			 ppp_ipv6_proto[1])) /*  PPP IPv6 */) {
			offset += PPPOE_HEAD_SIZE;	/* skip 8 bytes ppp header */
			dbg("Found PPP IP packet\n");
		} else {
			return 0;
		}
	}
	/*swap dst/src mac address */
	memcpy(mac, p, 6);
	memcpy(p, p + 6, 6);
	memcpy(p + 6, mac, 6);
	p_new_src_mac = p + 6;
	p += offset;		/*Note, now p[12~13] points to protocol */
	if (p[12] == 0x08 && p[13] == 0x06) {
		/* arp */
		if (p[14] == 0x00 && p[15] == 0x01 && p[16] == 0x08
		    && p[17] == 0x00 && p[20] == 0x00 && p[21] == 0x01) {
			dbg("arp request:%d.%d.%d.%d\n", p[38], p[39],
			    p[40], p[41]);
			/*  Ethernet IP - 10.10.xx.xx */
			if ((p[38] == ip_templ[0] && p[39] == ip_templ[1])
			    || memcmp(ip_templ, zero_mac,
				      sizeof(ip_templ)) == 0) {
				/*fill in spoof mac address */
				memcpy(p_new_src_mac,
				       g_test_wlan_dev[id]->dev_addr, 4);
				p[8] = p[38];
				p[9] = p[39];
				p[10] = p[40];
				p[11] = p[41];
				/*  arp reply */
				p[21] = 0x02;
				/*  sender mac */
				memcpy(mac, p + 22, 6);	/*save orignal sender mac */
				memcpy(p + 22, p_new_src_mac, 6);	/*set new sender mac */
				/* sender IP */
				memcpy(ip, p + 28, 4);	/*save original sender ip address */
				memcpy(p + 28, p + 38, 4);	/*set new sender ip address */
				/* target mac */
				memcpy(p + 32, mac, 6);
				/* target IP */
				memcpy(p + 38, ip, 4);
				return 1;
			}
		}
	} else if (((p[12] == 0x08) && (p[13] == 0x00)) /*Normal IPV4 */ ||
		   ((p[12] == ppp_ipv4_proto[0]) &&
		    (p[13] == ppp_ipv4_proto[1])) /*PPP IPV4 */ ||
		   ((p[12] == ppp_ipv6_proto[0]
		     && p[13] == ppp_ipv6_proto[1])) /*PPP IPV6 */) {
		/* IP */
		switch ((int)p[23]) {
		case 0x01:
			/* ICMP - request */
			if (p[34] == 0x08) {
				/* src IP */
				memcpy(ip, p + 26, 4);
				memcpy(p + 26, p + 30, 4);
				/* dest IP */
				memcpy(p + 30, ip, 4);
				/* ICMP reply */
				p[34] = 0x00;
				/* IP checksum */
				iph = (struct iphdr *)(p + 14);
				iph->check = 0;
				iph->check =
				    ip_fast_csum((unsigned char *)iph,
						 iph->ihl);
				/* ICMP checksum */
				icmph = (struct icmphdr *)(p + 34);
				icmph->checksum = 0;
				csum = csum_partial((unsigned char *)
						    icmph, skb->len - 34, 0);
				icmph->checksum = csum_fold(csum);
#if 0
				atomic_inc(&skb->users);
				skb->dev = ltq_dev_get_by_name("eth1");
				dev_put(skb->dev);
				dev_queue_xmit(skb);
				return 0;
#endif
				return 1;
			}
			break;
		case 0x11:
			/* UDP */
		case 0x06:
			/* TCP */
			/*swap src/dst ip */
			/* src IP */
			memcpy(ip, p + 26, 4);
			memcpy(p + 26, p + 30, 4);
			/* dest IP */
			memcpy(p + 30, ip, 4);
			/*shaoguoh remove below checksum item since we
			   just swap ip and port only
			 */
#if 0
			/* IP checksum */
			iph = (struct iphdr *)(p + 14);
			iph->check = 0;
			iph->check =
			    ip_fast_csum((unsigned char *)iph, iph->ihl);
			/* no UDP checksum */
			p[40] = p[41] = 0x00;
#endif
			/*shaoguoh add below to swap src/dst port 34~35 36~37 */
			/*save src port to port array and copy original dest
			   port to new src port
			 */
			memcpy(port, p + 34, 2);
			memcpy(p + 34, p + 36, 2);
			/* copy original src port to dest port */
			memcpy(p + 36, port, 2);
			/*return if UDP */
			if ((int)p[23] == 0x11)
				return 1;
			iph = (struct iphdr *)(p + 14);
			tcph = (struct tcphdr *)(p + 34);
			if (tcph->syn == 1) {	/*set syn & ack, set seq NO same as the incoming syn TCP packet, set ack seq NO as seq NO + 1 */
				tcph->ack = 1;
				tcph->ack_seq = tcph->seq + 1;
			} else if (tcph->fin == 1) {	/*set fin & ack */
				tcph->ack = 1;
				t = tcph->ack_seq;
				tcph->ack_seq = tcph->seq + 1;
				tcph->seq = t;
			} else if (tcph->rst == 1 || (tcph->psh == 0 && tcph->ack == 1)) {	/*rest or only ack, we ignore it. */
				return 0;
			} else if (tcph->psh == 1) {
				t = tcph->ack_seq;
				if (iph->tot_len < 40) {	/*corrupt packet, ignore it. */
					return -1;
				}
				tcph->ack_seq =
				    tcph->seq + iph->tot_len -
				    (iph->ihl * 4) - (tcph->doff * 4);
				tcph->seq = t;
			}
			/*check timestamp */
			off_t = 14 + 20 + 20;	/*mac + ip + tcp */

			while ((tcph->doff << 2) > (off_t - 34)) {	/*tcp option compare tcp header length */

				switch (p[off_t]) {
				case 0x0:	/*Option End */
					break;
				case 0x1:	/* NO Operation */
					off_t += 1;
					continue;
				case 0x2:	/*Max Segment Size */
					off_t += 4;
					continue;
				case 0x3:	/* Window Scale */
					off_t += 3;
					continue;
				case 0x4:	/*TCP Sack permitted */
					off_t += 2;
					continue;
				case 0x8:	/*TCP timestamp */
#if 1
					opt = (uint32_t *) (p + off_t + 2);
					//*(opt + 1) = htons(tcp_time_stamp);
					*(opt + 1) = 0; /*htonl(tcp_time_stamp(sock))*/
					t = *opt;
					*opt = *(opt + 1);
					*(opt + 1) = t;

#else
					for (t = 0; t < 10; t++)
						*(p + off_t + t) = 1;
#endif
					off_t += 10;	/*option max is 64-20 */
					continue;
				default:
					off_t += 64;
					break;
				}
			}
			/* IP checksum */
			iph = (struct iphdr *)(p + 14);
			iph->check = 0;
			iph->check =
			    ip_fast_csum((unsigned char *)iph, iph->ihl);
			/* TCP checksum */
			tcph->check = 0;
			t = iph->tot_len - (iph->ihl * 4);
			/*tcph->check = csum_partial((unsigned char *)tcph, iph->tot_len - 20, 0); */
			tcph->check =
			    csum_tcpudp_magic(iph->saddr, iph->daddr,
					      t, IPPROTO_TCP,
					      csum_partial(tcph, t, 0));
			return 1;
		}
	}
	return 0;
}

/* TODO : anath */
static INLINE void eth_rx_handler(struct sk_buff *skb, int id)
{
	struct test_wlan_priv_data *priv = netdev_priv(g_test_wlan_dev[id]);
	int pktlen;
	dump_skb(skb, skb->len, "test_tx", 0, 0);
	if (!netif_running(g_test_wlan_dev[id])) {
		dev_kfree_skb_any(skb);
		priv->stats.rx_dropped++;
		return;
	}
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
	if (priv->dp_subif.port_id > 0) {
#if CONFIG_WAVE600_USING_DP_API
		if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
			dc_dp_set_ep_pkt(skb, priv->dp_subif.port_id, 0);
			dc_dp_set_subifid_pkt(priv->dp_subif.port_id, skb, priv->dp_subif.subif, 0);
			if (dp_xmit(skb->dev, &priv->dp_subif, skb, skb->len, 0) == DP_SUCCESS) {
				priv->dp_pkts_to_ppe++;
				return;
			} else {
				priv->dp_pkts_to_ppe_fail++;
				return;
			}
		} else
#endif
		if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x") ||
	    	    !ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
			uint32_t sta_id = 0;
			dp_subif_t dp_subif = priv->dp_subif;

			dp_fp_get_staid(eth_hdr(skb)->h_dest, &sta_id);
			dp_subif.subif |= (sta_id & 0xFF); 
			/* TODO : Unicast/broadcast */
#define dc_dp_set_portid_pkt(desc_dw, port_id, flag) { \
    struct dma_tx_desc_1 *desc_1 = (struct dma_tx_desc_1 *) desc_dw; \
    desc_1->field.ep = port_id; \
}
			dc_dp_set_portid_pkt(&skb->DW1, priv->dp_subif.port_id, 0);
			dc_dp_set_subifid_pkt(priv->dp_subif.port_id, skb, dp_subif.subif, 0);

			if (dc_dp_xmit
			    (NULL /*g_test_wlan_dev[id]*/, NULL, &priv->dp_subif, skb, skb->len,
			     0) == DP_SUCCESS) {
				priv->dp_pkts_to_ppe++;
				return;
			} else {
				priv->dp_pkts_to_ppe_fail++;
				return;
			}
#endif
		}
		else if (!ltq_strcmpi(priv->flag_str, "DIRECTPATH")) {
		}
	}
#endif
	pktlen = skb->len;
	skb->dev = g_test_wlan_dev[id];
	skb->protocol = eth_type_trans(skb, g_test_wlan_dev[id]);
	if (netif_rx(skb) == NET_RX_DROP)
		priv->stats.rx_dropped++;
	else {
		priv->stats.rx_packets++;
		priv->stats.rx_bytes += pktlen;
	}
}

static void do_test_wlan_rx_tasklet(unsigned long id)
{
	struct test_wlan_priv_data *priv = NULL;
	struct sk_buff *skb = NULL;
	int i = 0;

	if (!g_test_wlan_dev[id])
		return;
	priv = netdev_priv(g_test_wlan_dev[id]);
	if (!priv)
		return;

	while (1) {
		if (i >= TASKLET_HANDLE_BUDGET) {
			tasklet_schedule(&priv->rx_tasklet);
			break;
		}
		skb = skb_dequeue(&priv->rx_queue);
		if (!skb)
			break;
		netif_wake_queue(g_test_wlan_dev[id]);
		if (eth_rx_preprocess(skb, (int)id))
			eth_rx_handler(skb, (int)id);
		else {
			priv->rx_preprocess_drop++;
			dev_kfree_skb_any(skb);
		}
		i++;
	}
}

static int32_t dp_fp_stop_tx(struct net_device *netif)
{
	return 0;
}

static int32_t dp_fp_restart_tx(struct net_device *netif)
{
	return 0;
}

static int32_t dp_fp_rx(struct net_device *rxif, struct net_device *txif, struct dp_subif *subif,
			struct sk_buff **pskb, int32_t len, uint32_t flags)
{
	struct test_wlan_priv_data *priv;
	int pktlen;
	struct sk_buff *skb;

	if (!pskb)
		return -1;

	skb = *pskb;
	if (rxif) {
		dump_skb(skb, skb->len, "callback: rx", 0, 0);
		if (netif_running(rxif)) {
			priv = netdev_priv(rxif);
			pktlen = skb->len;
			skb->dev = rxif;
			skb->protocol = eth_type_trans(skb, rxif);
			if (netif_rx(skb) == NET_RX_DROP)
				priv->stats.rx_dropped++;
			else {
				priv->stats.rx_packets++;
				priv->stats.rx_bytes += pktlen;
			}
			priv->dp_pkts_from_ppe++;
			*pskb = NULL;
			return 0;
		}
	} else if (txif) {
		priv = netdev_priv(txif);
		skb->dev = txif;
		dev_queue_xmit(skb);
		priv->dp_pkts_tx++;
		*pskb = NULL;
		return 0;
	}
	dev_kfree_skb_any(skb);
	*pskb = NULL;
	return 0;
}

#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
static int32_t dp_fp_get_staid(char *mac_addr, uint32_t *sta_id)
{
    /* TODO : convert staid */

    *sta_id = 0;

    return 0;
}   


#ifdef PUMA_DC_MODE1
static int32_t dp_fp_get_desc_info(int32_t port_id, struct sk_buff *skb,
				   struct dc_dp_fields_value_dw *desc_fields, uint32_t flags)
{
	struct test_wlan_priv_data *priv = NULL;

	if (!skb || (skb && !skb->dev) || !desc_fields || (desc_fields && !desc_fields->dw))
		return -1;

	priv = netdev_priv(skb->dev);

	desc_fields->num_dw = 2;
	desc_fields->dw[0].dw = 0;
	desc_fields->dw[0].desc_val = (uint32_t)priv->dp_subif.subif;
	desc_fields->dw[0].desc_mask = 0xFFFF;

	desc_fields->dw[1].dw = 3;
	desc_fields->dw[1].desc_val = 0;
	desc_fields->dw[1].desc_mask = (0xF << 24);

	return 0;
}
#endif
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
static void proc_read_dbg(struct seq_file *s)
{
	int i;

	SEQ_PRINTF(s, "g_dbg_enable=0x%08x\n. \tEnabled Flags:",
		   g_dbg_enable);
	for (i = 0; i < ARRAY_SIZE(dbg_enable_mask_str); i++)
		if ((g_dbg_enable & (1 << i)))
			SEQ_PRINTF(s, "%s ", dbg_enable_mask_str[i]);
	SEQ_PRINTF(s, "\n");
}

static ssize_t proc_write_dbg(struct file *file, const char *buf,
			  size_t count, loff_t *ppos)
{
	char str[100];
	int len, rlen;
	int f_enable = 0;
	int i, j;
	int num;
	char *param_list[30];

	len = count < sizeof(str) ? count : sizeof(str) - 1;
	rlen = len - copy_from_user(str, buf, len);
	str[rlen] = 0;

	num = ltq_split_buffer(str, param_list, ARRAY_SIZE(param_list));
	if (num < 1)
		goto help;

	if (ltq_strcmpi(param_list[0], "enable") == 0)
		f_enable = 1;
	else if (ltq_strcmpi(param_list[0], "disable") == 0)
		f_enable = -1;
	else
		goto help;

	if (!param_list[1])
		set_ltq_dbg_flag(g_dbg_enable, f_enable, -1);
	else {
		for (i = 1; i < num; i++) {
			for (j = 0; j < ARRAY_SIZE(dbg_enable_mask_str); j++) {
				if (ltq_strcmpi
				    (param_list[i],
				     dbg_enable_mask_str[j]) == 0) {
					set_ltq_dbg_flag(g_dbg_enable,
							 f_enable, (1 << j));

					break;
				}
			}
		}
	}

	return count;
 help:
	PRINTK("echo <enable/disable> [");
	for (i = 0; i < ARRAY_SIZE(dbg_enable_mask_str); i++) {
		if (i == 0)
			PRINTK("%s", dbg_enable_mask_str[i]);
		else
			PRINTK("/%s", dbg_enable_mask_str[i]);
	}
	PRINTK("] > /proc/net/testwlan/dbg\n");
	return count;
}
#endif

static int proc_read_dev(struct seq_file *s, int pos)
{
	if (g_test_wlan_dev[pos])
		SEQ_PRINTF(s, "  %s\n", g_test_wlan_dev[pos]->name);
	pos++;

	if (pos >= MAX_TESTWLAN_NUM)
		pos = -1;
	return pos;
}

static int32_t get_port_info_via_dev_port(struct test_wlan_priv_data *priv_dev, int dev_port)
{
	int i;
	struct test_wlan_priv_data *priv = NULL;

	for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++) {
		if (!g_test_wlan_dev[i]) /* Skip NULL devices */
			continue;
		priv = netdev_priv(g_test_wlan_dev[i]);
		if (priv && priv->f_dp != TESTWLAN_F_FREE && (priv->dev_port == dev_port)) {
			if (!ltq_strcmpi(priv->flag_str, "DIRECTPATH")) {
				/* Determine the registered intf and return the func ptr and etc */
				priv_dev->dev_port = dev_port;
				priv_dev->owner = priv->owner;
#if (defined(CONFIG_LTQ_PPA_GRX500) && CONFIG_LTQ_PPA_GRX500) && \
	(defined(CONFIG_LTQ_PPA_API_DIRECTPATH) && CONFIG_LTQ_PPA_API_DIRECTPATH)
				//memcpy(&priv_dev->dp_cb, &priv->dp_cb, sizeof(dp_cb_t));
#endif
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
				priv_dev->dp_subif.port_id = priv->dp_subif.port_id;
#endif
				//priv_dev->flag_index = priv->flag_index;
				strncpy(priv_dev->flag_str, priv->flag_str, (sizeof(priv_dev->flag_str) - 1));
				priv_dev->flag_str[(sizeof(priv_dev->flag_str) - 1)] = '\0';
			}
			else if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x") ||
	    	    		 !ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
				/* Determine the registered intf and return the func ptr and etc */
				priv_dev->dev_port = dev_port;
				priv_dev->owner = priv->owner;
#if CONFIG_WAVE600_USING_DP_API
				memcpy(&priv_dev->dp_cb, &priv->dp_cb, sizeof(struct dp_cb));
#endif
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
				memcpy(&priv_dev->wave500_cb, &priv->wave500_cb, sizeof(struct dc_dp_cb));
#endif
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
				priv_dev->dp_subif.port_id = priv->dp_subif.port_id;
#endif
				//priv_dev->flag_index = priv->flag_index;
				strncpy(priv_dev->flag_str, priv->flag_str, (sizeof(priv_dev->flag_str) - 1));
				priv_dev->flag_str[(sizeof(priv_dev->flag_str) - 1)] = '\0';
			}
			return 0;
		}
	}
	return -1;
}

/* Get number of subif */
static int get_subif_cnt_via_dev_port(int32_t dev_port)
{
	struct test_wlan_priv_data *priv;
	int num_subif = 0, i;

	for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++) {
		if (!g_test_wlan_dev[i])
			continue;
		priv = netdev_priv(g_test_wlan_dev[i]);
		if (priv && (priv->f_dp == TESTWLAN_F_REGISTER_SUBIF) && (dev_port == priv->dev_port))
			num_subif ++;
	}
	return num_subif;
}

static int register_dev(int i)
{
#if defined(CONFIG_SOC_GRX500)
	struct dma_rx_desc *rx_desc_p;
	dma_addr_t dbg_buf_phys;
	void *dbg_buf = NULL;
	struct dc_dp_buf_pool *buflist;
#endif
	struct test_wlan_priv_data *priv;
	int32_t dp_port_id;
	int32_t port_id;
	uint32_t alloc_flags = 0;

	if (!g_test_wlan_dev[i])
		return -1;

	priv = netdev_priv(g_test_wlan_dev[i]);

	/* TODO : Check for unique priv->dev_port */

#if 0
	if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x")) {
		/* TODO : Check for maximum 3 devices (using priv->dev_port) */
		port_id = 0;
		dp_port_id = dp_alloc_port_ext(0, priv->owner, g_test_wlan_dev[i], priv->dev_port,
					   port_id, NULL, NULL, DP_F_FAST_WLAN);
		if (dp_port_id <= 0) {
			PRINTK
			    ("failed in register fastpath for %s\n",
			     g_test_wlan_dev[i]->name);
			return -1;
		}
		PRINTK("dp_alloc_port_ext get port %d for %s\n",
		       dp_port_id, g_test_wlan_dev[i]->name);
		priv->dp_cb.stop_fn = (dp_stop_tx_fn_t) dp_fp_stop_tx;
		priv->dp_cb.restart_fn = (dp_restart_tx_fn_t) dp_fp_restart_tx;
		priv->dp_cb.rx_fn = (dp_rx_fn_t) dp_fp_rx;
		priv->dp_subif.port_id = dp_port_id;
		if (dp_register_dev(priv->owner, dp_port_id, &priv->dp_cb, 0) != DP_SUCCESS) {
			PRINTK
			    ("dp_register_dev failed for %s\n and port_id %d",
			     g_test_wlan_dev[i]->name, dp_port_id);
			dp_alloc_port_ext(0, priv->owner, g_test_wlan_dev[i], priv->dev_port,
				      dp_port_id, NULL, NULL, DP_F_DEREGISTER);
			return -1;
		}
	} else 
#endif
#if CONFIG_WAVE600_USING_DP_API
	if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
		// 1st radio
		if (g_resvd_port < 0) {
			PRINTK
			    ("Port reservation failed g_resvd_port=%d, not allowed to reg dev=%s as FAST_WAVE6x?\n",
			     g_resvd_port, g_test_wlan_dev[i]->name);
			return -1;
		}
		if (g_wave6x_radio_num > 2) {
			PRINTK
			    ("Port reservation failed g_wave6x_radio_num=%d, not allowed to reg dev=%s as FAST_WAVE6x?\n",
			     g_wave6x_radio_num, g_test_wlan_dev[i]->name);
			return -1;
		}

		// 1st radio
		if (g_wave6x_radio_num == 0) {
			port_id = g_resvd_port;
			/* TODO : Check for maximum 3 devices (using priv->dev_port) */
			dp_port_id = dp_alloc_port_ext(0, priv->owner, g_test_wlan_dev[i], priv->dev_port,
						   port_id, NULL, NULL, (DP_F_FAST_WLAN | DP_F_SHARED_RES));

		// 2nd radio
		} else {
			struct dp_port_data dp_p_data = {0};
			dp_p_data.flag_ops = DP_F_DATA_ALLOC;
			dp_p_data.start_port_no = g_resvd_port;
			port_id = g_resvd_port + 1;
			dp_port_id = dp_alloc_port_ext(0, priv->owner, g_test_wlan_dev[i], priv->dev_port,
						   port_id, NULL, &dp_p_data, (DP_F_FAST_WLAN | DP_F_SHARED_RES));
		}
		if (dp_port_id <= 0) {
			PRINTK
			    ("failed in register fastpath for %s\n",
			     g_test_wlan_dev[i]->name);
			return -1;
		}
		PRINTK("dp_alloc_port_ext get port %d for %s\n",
		       dp_port_id, g_test_wlan_dev[i]->name);
		priv->dp_cb.stop_fn = (dp_stop_tx_fn_t) dp_fp_stop_tx;
		priv->dp_cb.restart_fn = (dp_restart_tx_fn_t) dp_fp_restart_tx;
		priv->dp_cb.rx_fn = (dp_rx_fn_t) dp_fp_rx;
		priv->dp_subif.port_id = dp_port_id;
		if (dp_register_dev(priv->owner, dp_port_id, &priv->dp_cb, 0) != DP_SUCCESS) {
			PRINTK
			    ("dp_register_dev failed for %s\n and port_id %d",
			     g_test_wlan_dev[i]->name, dp_port_id);
			dp_alloc_port_ext(0, priv->owner, g_test_wlan_dev[i], priv->dev_port,
				      dp_port_id, NULL, NULL, DP_F_DEREGISTER);
			return -1;
		}
		g_wave6x_radio_num ++;
	} else
#endif
	if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x")) { // How to know it is allready allocated
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
		dma_addr_t dccntr_phys_base;

		/* TODO : Check for maximum 3 devices (using priv->dev_port) */
		dp_port_id = dc_dp_alloc_port(priv->owner, priv->dev_port, g_test_wlan_dev[i],
					   0, DC_DP_F_FASTPATH);
		if (dp_port_id <= 0) {
			PRINTK
			    ("failed in register fastpath for %s\n",
			     g_test_wlan_dev[i]->name);
			return -1;
		}
		PRINTK("dc_dp_alloc_port get port %d for %s\n",
		       dp_port_id, g_test_wlan_dev[i]->name);
		priv->wave500_cb.stop_fn = (dp_stop_tx_fn_t) dp_fp_stop_tx;
		priv->wave500_cb.restart_fn = (dp_restart_tx_fn_t) dp_fp_restart_tx;
		priv->wave500_cb.rx_fn = (dc_dp_rx_fn_t) dp_fp_rx;
		priv->wave500_cb.get_subif_fn = (dc_dp_get_netif_subinterface_fn_t) dp_fp_get_staid;
#ifdef PUMA_DC_MODE1
		priv->wave500_cb.get_desc_info_fn = (dc_dp_get_dev_specific_desc_info_t) dp_fp_get_desc_info;
#endif
		priv->dp_subif.port_id = dp_port_id;
#ifdef PUMA_DC_MODE1
		priv->dccntr.soc_write_dccntr_mode = (DC_DP_F_DCCNTR_MODE_INCREMENTAL | DC_DP_F_DCCNTR_MODE_BIG_ENDIAN);
#else
		priv->dccntr.soc_write_dccntr_mode = (DC_DP_F_DCCNTR_MODE_INCREMENTAL | DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN);
#endif
		//priv->dccntr.dev2soc_ret_enq_base = (void *)dma_alloc_coherent(NULL, 8, &dccntr_phys_base, GFP_DMA);
		//priv->dccntr.dev2soc_ret_enq_phys_base = (void *)dccntr_phys_base;
#ifdef PUMA_DC_MODE1
		priv->dccntr.dev2soc_ret_enq_base = (void *)kmalloc(16, GFP_DMA);
#else
		priv->dccntr.dev2soc_ret_enq_base = (void *)kmalloc(8, GFP_DMA);
#endif
		priv->dccntr.dev2soc_ret_enq_phys_base = (void *)virt_to_phys(priv->dccntr.dev2soc_ret_enq_base);
		priv->dccntr.dev2soc_ret_enq_dccntr_len = 4;
		priv->dccntr.soc2dev_enq_base = (void *)(priv->dccntr.dev2soc_ret_enq_base + 4);
		priv->dccntr.soc2dev_enq_phys_base = (void *)(priv->dccntr.dev2soc_ret_enq_phys_base + 4);
		priv->dccntr.soc2dev_enq_dccntr_len = 4;
#ifdef PUMA_DC_MODE1
		priv->dccntr.dev2soc_deq_base = (void *)((uint32_t)priv->dccntr.dev2soc_ret_enq_base + 8);
		priv->dccntr.dev2soc_deq_phys_base = (void *)((uint32_t)priv->dccntr.dev2soc_ret_enq_phys_base + 8);
		priv->dccntr.dev2soc_deq_dccntr_len = 4;
		priv->dccntr.soc2dev_ret_deq_base = (void *)((uint32_t)priv->dccntr.dev2soc_ret_enq_base + 12);
		priv->dccntr.soc2dev_ret_deq_phys_base = (void *)((uint32_t)priv->dccntr.dev2soc_ret_enq_phys_base + 12);
		priv->dccntr.soc2dev_ret_deq_dccntr_len = 4;
#endif
#ifdef CONFIG_X86_INTEL_LGM
		priv->resources.num_bufs_req = 64;
		priv->resources.rings.soc2dev.size = 64;
		priv->resources.rings.soc2dev_ret.size = 64;
		priv->resources.rings.dev2soc.size = 64;
		priv->resources.rings.dev2soc_ret.size = 64;
#else
		priv->resources.num_bufs_req = 32;
		priv->resources.rings.soc2dev.size = 32;
		priv->resources.rings.soc2dev_ret.size = 32;
		priv->resources.rings.dev2soc.size = 32;
		priv->resources.rings.dev2soc_ret.size = 32;
#endif

		/* Exception Ring */
		priv->resources.rings.dev2soc_except.base = (void *)dma_alloc_coherent(NULL, 256 + 8, &dccntr_phys_base, GFP_DMA);
		priv->resources.rings.dev2soc_except.phys_base = (void *)dccntr_phys_base;
		priv->resources.rings.dev2soc_except.size = 64;
		priv->resources.rings.dev2soc_except.desc_dwsz = 1;

		/* DC counter ENQ */
		priv->dccntr.dev2soc_except_enq_base = (void *)((uint8_t *)priv->resources.rings.dev2soc_except.base + 256);
		priv->dccntr.dev2soc_except_enq_phys_base = (void *)((uint8_t *)priv->resources.rings.dev2soc_except.phys_base + 256);
		priv->dccntr.dev2soc_except_enq_dccntr_len = 4;

		/* DC counter DEQ */
		priv->dccntr.dev2soc_except_deq_base = (void *)((uint8_t *)priv->resources.rings.dev2soc_except.base + 260);
		priv->dccntr.dev2soc_except_deq_phys_base = (void *)((uint8_t *)priv->resources.rings.dev2soc_except.phys_base + 260);
		priv->dccntr.dev2soc_except_deq_dccntr_len = 4;

		priv->resources.rings.soc2dev.desc_dwsz = 4;
		priv->resources.rings.soc2dev.flags = DC_DP_F_PKTDESC_ENQ;
		priv->resources.rings.soc2dev_ret.desc_dwsz = 4;
		priv->resources.rings.soc2dev.flags = DC_DP_F_PKTDESC_RET;
		priv->resources.rings.dev2soc.desc_dwsz = 4;
		priv->resources.rings.dev2soc.flags = DC_DP_F_PKTDESC_ENQ;
		priv->resources.rings.dev2soc_ret.desc_dwsz = 4;
		priv->resources.rings.dev2soc_ret.flags = DC_DP_F_RET_RING_SAME_AS_ENQ;
		priv->resources.num_dccntr = 1;
		priv->resources.dccntr = &priv->dccntr;
		if (dc_dp_register_dev(priv->owner, dp_port_id, g_test_wlan_dev[i],
			&priv->wave500_cb, &priv->resources, &priv->devspec, 0) != DC_DP_SUCCESS) {
			PRINTK
			    ("dc_dp_register_dev failed for %s\n and port_id %d",
			     g_test_wlan_dev[i]->name, dp_port_id);
			//dma_free_coherent(NULL, 8, priv->dccntr.dev2soc_ret_enq_base, (dma_addr_t)priv->dccntr.dev2soc_ret_enq_phys_base);
			kfree(priv->dccntr.dev2soc_ret_enq_base);
			dc_dp_alloc_port(priv->owner, priv->dev_port, g_test_wlan_dev[i],
				      dp_port_id, DC_DP_F_DEREGISTER);
			return -1;
		}

#if defined(CONFIG_SOC_GRX500)
		/* Allocate memory for recv packets */
		buflist = (void *)priv->resources.buflist;
		if (buflist) {
			dbg_buf = buflist[0].pool;
			if (dbg_buf) {
				dbg_buf_phys = dma_map_single(NULL, dbg_buf, priv->resources.buflist[0].size, DMA_TO_DEVICE);
				rx_desc_p = (struct dma_rx_desc *)priv->resources.rings.dev2soc.base;
				for (i = 0; i < priv->resources.rings.dev2soc.size; i++) {
					rx_desc_p->data_pointer = dbg_buf_phys;
					rx_desc_p++;
				}
			}
		}
#endif
#endif
	}
	else if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) { // How to know it is allready allocated
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
		//dma_addr_t dccntr_phys_base;
		if (g_wave6x_radio_num > 2) {
			PRINTK
			    ("Port reservation failed g_wave6x_radio_num=%d, not allowed to reg dev=%s as FAST_WAVE6x?\n",
			     g_wave6x_radio_num, g_test_wlan_dev[i]->name);
			return -1;
		}

		// 1st radio
		if (g_wave6x_radio_num == 0) {
			port_id = 0;
			alloc_flags |= DC_DP_F_MULTI_PORT;
			
		} else {
			port_id = g_resvd_port;
			alloc_flags |= DC_DP_F_MULTI_PORT | DC_DP_F_SHARED_RES;
		}

		/* TODO : Check for maximum 3 devices (using priv->dev_port) */
		dp_port_id = dc_dp_alloc_port(THIS_MODULE, priv->dev_port, g_test_wlan_dev[i],
					   port_id, (alloc_flags | DC_DP_F_FASTPATH));
		if (dp_port_id <= 0) {
			PRINTK
			    ("failed in register fastpath for %s\n",
			     g_test_wlan_dev[i]->name);
			return -1;
		}
		if (g_wave6x_radio_num == 0)
			g_resvd_port = dp_port_id;
		g_wave6x_radio_num++;
		PRINTK("dc_dp_alloc_port get port %d for %s\n",
		       dp_port_id, g_test_wlan_dev[i]->name);
		priv->wave500_cb.stop_fn = (dp_stop_tx_fn_t) dp_fp_stop_tx;
		priv->wave500_cb.restart_fn = (dp_restart_tx_fn_t) dp_fp_restart_tx;
		priv->wave500_cb.rx_fn = (dc_dp_rx_fn_t) dp_fp_rx;
		priv->wave500_cb.get_subif_fn = (dc_dp_get_netif_subinterface_fn_t) dp_fp_get_staid;
#ifdef PUMA_DC_MODE1
		priv->wave500_cb.get_desc_info_fn = (dc_dp_get_dev_specific_desc_info_t) dp_fp_get_desc_info;
#endif
		priv->dp_subif.port_id = dp_port_id;
#ifdef PUMA_DC_MODE1
		priv->dccntr.soc_write_dccntr_mode = (DC_DP_F_DCCNTR_MODE_INCREMENTAL | DC_DP_F_DCCNTR_MODE_BIG_ENDIAN);
#else
		priv->dccntr.soc_write_dccntr_mode = (DC_DP_F_DCCNTR_MODE_INCREMENTAL | DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN);
#endif
		//priv->dccntr.dev2soc_ret_enq_base = (void *)dma_alloc_coherent(NULL, 8, &dccntr_phys_base, GFP_DMA);
		//priv->dccntr.dev2soc_ret_enq_phys_base = (void *)dccntr_phys_base;
#ifdef PUMA_DC_MODE1
		priv->dccntr.dev2soc_ret_enq_base = (void *)kmalloc(16, GFP_DMA);
#else
		priv->dccntr.dev2soc_ret_enq_base = (void *)kmalloc(8, GFP_DMA);
#endif
		priv->dccntr.dev2soc_ret_enq_phys_base = (void *)virt_to_phys(priv->dccntr.dev2soc_ret_enq_base);
		priv->dccntr.dev2soc_ret_enq_dccntr_len = 4;
		priv->dccntr.soc2dev_enq_base = (void *)(priv->dccntr.dev2soc_ret_enq_base + 4);
		priv->dccntr.soc2dev_enq_phys_base = (void *)(priv->dccntr.dev2soc_ret_enq_phys_base + 4);
		priv->dccntr.soc2dev_enq_dccntr_len = 4;
#ifdef PUMA_DC_MODE1
		priv->dccntr.dev2soc_deq_base = (void *)((uint32_t)priv->dccntr.dev2soc_ret_enq_base + 8);
		priv->dccntr.dev2soc_deq_phys_base = (void *)((uint32_t)priv->dccntr.dev2soc_ret_enq_phys_base + 8);
		priv->dccntr.dev2soc_deq_dccntr_len = 4;
		priv->dccntr.soc2dev_ret_deq_base = (void *)((uint32_t)priv->dccntr.dev2soc_ret_enq_base + 12);
		priv->dccntr.soc2dev_ret_deq_phys_base = (void *)((uint32_t)priv->dccntr.dev2soc_ret_enq_phys_base + 12);
		priv->dccntr.soc2dev_ret_deq_dccntr_len = 4;
#endif
		priv->resources.num_bufs_req = 64;
		priv->resources.rings.soc2dev.size = 64;
		priv->resources.rings.soc2dev.desc_dwsz = 4;
		priv->resources.rings.soc2dev.flags = DC_DP_F_PKTDESC_ENQ;
		priv->resources.rings.soc2dev_ret.size = 64;
		priv->resources.rings.soc2dev_ret.desc_dwsz = 4;
		priv->resources.rings.soc2dev.flags = DC_DP_F_PKTDESC_RET;
		priv->resources.rings.dev2soc.size = 64;
		priv->resources.rings.dev2soc.desc_dwsz = 4;
		priv->resources.rings.dev2soc.flags = DC_DP_F_PKTDESC_ENQ;
		priv->resources.rings.dev2soc_ret.size = 64;
		priv->resources.rings.dev2soc_ret.desc_dwsz = 4;
		priv->resources.rings.dev2soc_ret.flags = DC_DP_F_RET_RING_SAME_AS_ENQ;
		priv->resources.num_dccntr = 1;
		priv->resources.dccntr = &priv->dccntr;
		if (dc_dp_register_dev(THIS_MODULE, dp_port_id, g_test_wlan_dev[i],
			&priv->wave500_cb, &priv->resources, &priv->devspec, 0) != DC_DP_SUCCESS) {
			PRINTK
			    ("dc_dp_register_dev failed for %s\n and port_id %d",
			     g_test_wlan_dev[i]->name, dp_port_id);
			//dma_free_coherent(NULL, 8, priv->dccntr.dev2soc_ret_enq_base, (dma_addr_t)priv->dccntr.dev2soc_ret_enq_phys_base);
			kfree(priv->dccntr.dev2soc_ret_enq_base);
			dc_dp_alloc_port(THIS_MODULE, priv->dev_port, g_test_wlan_dev[i],
				      dp_port_id, DC_DP_F_DEREGISTER);
		        if (g_wave6x_radio_num == 1)
		        	g_resvd_port = -1;
		        g_wave6x_radio_num--;
			return -1;
		}
#endif
	}
	else if (!ltq_strcmpi(priv->flag_str, "DIRECTPATH")) {
	}

	priv->f_dp = TESTWLAN_F_REGISTER_DEV;

	return 0;
}

static int unregister_dev(int i)
{
	struct test_wlan_priv_data *priv;
	int k;
	int32_t dev_port = -1;	/*dev  instance */

	if (!g_test_wlan_dev[i])
		return -1;

	priv = netdev_priv(g_test_wlan_dev[i]);
	dev_port = priv->dev_port;
	if (priv->f_dp == TESTWLAN_F_REGISTER_SUBIF) {
#if CONFIG_WAVE600_USING_DP_API
		if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
			dp_register_subif(priv->owner, g_test_wlan_dev[i], g_test_wlan_dev[i]->name,
					  &priv->dp_subif,
					  DP_F_DEREGISTER);
		} else
#endif
		if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x")) {
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
			dc_dp_register_subif(priv->owner, g_test_wlan_dev[i], NULL,
					  &priv->dp_subif,
					  DC_DP_F_DEREGISTER);
#endif
		} else if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
			dc_dp_register_subif(THIS_MODULE, g_test_wlan_dev[i], NULL,
					  &priv->dp_subif,
					  DC_DP_F_DEREGISTER);
#endif
		} else if (!ltq_strcmpi(priv->flag_str, "DIRECTPATH")) {
		}
		priv->f_dp = TESTWLAN_F_REGISTER_DEV;
	}
	if(!get_subif_cnt_via_dev_port(priv->dev_port)) {
		if (priv->f_dp == TESTWLAN_F_REGISTER_DEV) {
#if CONFIG_WAVE600_USING_DP_API
			if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
				if (dp_register_dev(priv->owner, priv->dp_subif.port_id, &priv->dp_cb, DP_F_DEREGISTER) != DP_SUCCESS)
					PRINTK("dp_register_dev failed \
						for %s with port_id/subif %d/%d\n", g_test_wlan_dev[i]->name, priv->dp_subif.port_id, priv->dp_subif.subif);
				if (dp_alloc_port_ext(0, priv->owner, g_test_wlan_dev[i], priv->dev_port,
						  priv->dp_subif.port_id, NULL, NULL, DP_F_DEREGISTER) != DP_SUCCESS) {
					PRINTK("dp_alloc_port_ext failed \
						for %s with port_id/subif %d/%d\n", g_test_wlan_dev[i]->name, priv->dp_subif.port_id, priv->dp_subif.subif);
				}
				g_wave6x_radio_num--;
				if (g_wave6x_radio_num < 0)
					g_wave6x_radio_num = 0;
			} else
#endif
			if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x")) {
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
				if (dc_dp_register_dev
				    (priv->owner, priv->dp_subif.port_id, g_test_wlan_dev[i], &priv->wave500_cb,
					&priv->resources, &priv->devspec, DC_DP_F_DEREGISTER) != DP_SUCCESS)
					PRINTK("dc_dp_register_dev failed \
						for %s with port_id/subif %d/%d\n", g_test_wlan_dev[i]->name, priv->dp_subif.port_id, priv->dp_subif.subif);
				//dma_free_coherent(NULL, 8, priv->dccntr.dev2soc_ret_enq_base, (dma_addr_t)priv->dccntr.dev2soc_ret_enq_phys_base);
				kfree(priv->dccntr.dev2soc_ret_enq_base);
				if (dc_dp_alloc_port(priv->owner, priv->dev_port, g_test_wlan_dev[i],
						  priv->dp_subif.port_id, DC_DP_F_DEREGISTER) != DP_SUCCESS) {
					PRINTK("dc_dp_alloc_port failed \
						for %s with port_id/subif %d/%d\n", g_test_wlan_dev[i]->name, priv->dp_subif.port_id, priv->dp_subif.subif);
				}
#endif
			} else if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
				if (dc_dp_register_dev
				    (THIS_MODULE, priv->dp_subif.port_id, g_test_wlan_dev[i], &priv->wave500_cb,
					&priv->resources, &priv->devspec, DC_DP_F_DEREGISTER) != DP_SUCCESS)
					PRINTK("dc_dp_register_dev failed \
						for %s with port_id/subif %d/%d\n", g_test_wlan_dev[i]->name, priv->dp_subif.port_id, priv->dp_subif.subif);
				//dma_free_coherent(NULL, 8, priv->dccntr.dev2soc_ret_enq_base, (dma_addr_t)priv->dccntr.dev2soc_ret_enq_phys_base);
				kfree(priv->dccntr.dev2soc_ret_enq_base);
				if (dc_dp_alloc_port(THIS_MODULE, priv->dev_port, g_test_wlan_dev[i],
						  priv->dp_subif.port_id, DC_DP_F_DEREGISTER) != DP_SUCCESS) {
					PRINTK("dc_dp_alloc_port failed \
						for %s with port_id/subif %d/%d\n", g_test_wlan_dev[i]->name, priv->dp_subif.port_id, priv->dp_subif.subif);
				}
#endif
			} else if (!ltq_strcmpi(priv->flag_str, "DIRECTPATH")) {
			}
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
			priv->dp_subif.port_id = -1;
#endif
		}
		for (k = 0; k < ARRAY_SIZE(g_test_wlan_dev); k++) {
			if (!g_test_wlan_dev[k])
				continue;
			priv = netdev_priv(g_test_wlan_dev[k]);
			if (priv && (dev_port == priv->dev_port)) {
				priv->dev_port = -1;
#if CONFIG_WAVE600_USING_DP_API
				if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
					memset(&priv->dp_cb, 0, sizeof(struct dp_cb));
				} else
#endif
				if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x") ||
				    !ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
					memset(&priv->wave500_cb, 0, sizeof(struct dc_dp_cb));
#endif
				}
				else if (!ltq_strcmpi(priv->flag_str, "DIRECTPATH")) {
				}
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
				priv->dp_subif.port_id = -1;
#endif
				//priv->flag_index = -1;
				priv->f_dp = TESTWLAN_F_FREE;
			}
		}
	}
	return 0;
}

static int delete_testwlan_dev(int i)
{
	struct test_wlan_priv_data *priv;

	if (!g_test_wlan_dev[i])
		return -1;

	priv = netdev_priv(g_test_wlan_dev[i]);

	unregister_dev(i);

	/*unregister testwlan dev itself */
	unregister_netdev(g_test_wlan_dev[i]);
	free_netdev(g_test_wlan_dev[i]);
	g_test_wlan_dev[i] = NULL;
	return 0;
}

static int create_testwlan_dev(int i)
{
	char ifname[32];
	struct test_wlan_priv_data *priv;

	sprintf(ifname, "testwlan%d", i);
	g_test_wlan_dev[i] =
	    alloc_netdev(sizeof(struct test_wlan_priv_data), ifname,
			 NET_NAME_ENUM, eth_setup);
	if (g_test_wlan_dev[i] == NULL) {
		PRINTK("alloc_netdev fail\n");
		return -1;
	}
	if (register_netdev(g_test_wlan_dev[i])) {
		free_netdev(g_test_wlan_dev[i]);
		g_test_wlan_dev[i] = NULL;
		PRINTK("register device \"%s\" fail ??\n", ifname);
	} else {
		PRINTK("add \"%s\" successfully\n", ifname);
		priv = netdev_priv(g_test_wlan_dev[i]);
		priv->f_dp = TESTWLAN_F_FREE;
		priv->id = i; /* Set the priv ID */
		priv->rx_tasklet.data = i; /* Set the tasklet ID */
	}
	return 0;
}

static ssize_t proc_write_dev(struct file *file, const char *buf, size_t count,
			  loff_t *ppos)
{
	char str[100];
	int len, rlen;
	char *param_list[10];
	int num;
	int i;

	len = count < sizeof(str) ? count : sizeof(str) - 1;
	rlen = len - copy_from_user(str, buf, len);
	str[rlen] = 0;

	num = ltq_split_buffer(str, param_list, ARRAY_SIZE(param_list));
	if (num < 1)
		goto help;

	if (ltq_strcmpi(param_list[0], "add") == 0) {
		if (param_list[1]) {
			i = ltq_atoi(param_list[1]);
			if ((i < 0) || (i >= ARRAY_SIZE(g_test_wlan_dev))) {
				PRINTK("Wrong index value: %d\n", i);
				return count;
			}
			if (g_test_wlan_dev[i]) {
				PRINTK
				    ("interface index %d already exist and no need to create it\n",
				     i);
				return count;
			}

			/*create one dev */
			create_testwlan_dev(i);
		} else {
#if 0
			for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++) {
				/*create all dev if not created yet */
				if (g_test_wlan_dev[i])
					continue;
				create_testwlan_dev(i);
			}
#endif
			PRINTK("Wrong arguments: %s\n", param_list[0]);
			goto help;
		}
	} else if (ltq_strcmpi(param_list[0], "del") == 0) {
		if (param_list[1]) {
			for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++) {
				if (g_test_wlan_dev[i] &&
				    (ltq_strcmpi
				     (g_test_wlan_dev[i]->name,
				      param_list[1]) == 0)) {
					delete_testwlan_dev(i);
					break;
				}
			}
		} else {
			for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++)
				delete_testwlan_dev(i);
		}
	} else {
		PRINTK("Wrong command: %s\n", param_list[0]);
		goto help;
	}
	return count;

 help:
	PRINTK("echo add <index> /proc/net/testwlan/dev\n");
	PRINTK("   example: echo add 0 > /proc/net/testwlan/dev\n");
	PRINTK("        Note, the maximum index is %d\n",
	       ARRAY_SIZE(g_test_wlan_dev));
	PRINTK("echo <del> [device name] > /proc/net/testwlan/dev\n");
	PRINTK("   example: echo del testwlan1 > /proc/net/testwlan/dev\n");
	PRINTK("   example: echo del          > /proc/net/testwlan/dev\n");
	return count;
}

static int proc_read_mib(struct seq_file *s, int pos)
{
	struct test_wlan_priv_data *priv;

	if (g_test_wlan_dev[pos]) {
		priv = netdev_priv(g_test_wlan_dev[pos]);
		SEQ_PRINTF(s, "  %s:\n", g_test_wlan_dev[pos]->name);
		SEQ_PRINTF(s, "    rx_packets: %lu\n",
			   priv->stats.rx_packets);
		SEQ_PRINTF(s, "    rx_bytes:   %lu\n", priv->stats.rx_bytes);
		SEQ_PRINTF(s, "    rx_errors:  %lu\n", priv->stats.rx_errors);
		SEQ_PRINTF(s, "    rx_dropped: %lu\n",
			   priv->stats.rx_dropped);
		SEQ_PRINTF(s, "    tx_packets: %lu\n",
			   priv->stats.tx_packets);
		SEQ_PRINTF(s, "    tx_bytes:   %lu\n", priv->stats.tx_bytes);
		SEQ_PRINTF(s, "    tx_errors:  %lu\n", priv->stats.tx_errors);
		SEQ_PRINTF(s, "    tx_dropped: %lu\n",
			   priv->stats.tx_dropped);
		SEQ_PRINTF(s, "    rx_preprocess_drop:  %u\n",
			   priv->rx_preprocess_drop);
		SEQ_PRINTF(s, "    dp_pkts_to_ppe:      %u\n",
			   priv->dp_pkts_to_ppe);
		SEQ_PRINTF(s, "    dp_pkts_to_ppe_fail: %u\n",
			   priv->dp_pkts_to_ppe_fail);
		SEQ_PRINTF(s, "    dp_pkts_from_ppe:    %u\n",
			   priv->dp_pkts_from_ppe);
		SEQ_PRINTF(s, "    dp_pkts_tx:          %u\n",
			   priv->dp_pkts_tx);
	}

	pos++;
	if (pos >= MAX_TESTWLAN_NUM)
		pos = -1;

	return pos;
}

static void clear_mib(int i)
{
	struct test_wlan_priv_data *priv;

	priv = netdev_priv(g_test_wlan_dev[i]);
	if (!priv)
		return;
	memset(&priv->stats, 0, sizeof(priv->stats));
	priv->rx_preprocess_drop = 0;
	priv->dp_pkts_to_ppe = 0;
	priv->dp_pkts_to_ppe_fail = 0;
	priv->dp_pkts_from_ppe = 0;
	priv->dp_pkts_tx = 0;
}

static ssize_t proc_write_mib(struct file *file, const char *buf,
			  size_t count, loff_t *ppos)
{
	char str[100];
	int len, rlen;
	int i;
	int num;
	char *param_list[4];

	len = count < sizeof(str) ? count : sizeof(str) - 1;
	rlen = len - copy_from_user(str, buf, len);
	str[rlen] = 0;
	num = ltq_split_buffer(str, param_list, ARRAY_SIZE(param_list));
	if (num < 2)
		goto help;

	if (ltq_strcmpi(param_list[0], "clear") != 0) {
		PRINTK("Wrong command:%s\n", param_list[0]);
		goto help;
	}

	if ((ltq_strcmpi(param_list[1], "all") != 0)) {
		for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++) {
			if (g_test_wlan_dev[i] &&
			    (ltq_strcmpi(g_test_wlan_dev[i]->name,
					 param_list[1]) == 0)) {
				clear_mib(i);
				break;
			}
		}
		if (i >= ARRAY_SIZE(g_test_wlan_dev))
			PRINTK("not found device %s\n", param_list[1]);
	} else {
		for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++)
			clear_mib(i);
	}
	return count;
 help:
	PRINTK("echo <clear> [all/device name] > /proc/net/testwlan/mib\n");
	return count;
}

static int proc_read_datapath(struct seq_file *s, int pos)
{
	struct test_wlan_priv_data *priv;

	if (g_test_wlan_dev[pos]) {
		priv = netdev_priv(g_test_wlan_dev[pos]);
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
		if (priv->dp_subif.port_id >= 0) {
			SEQ_PRINTF(s,
				   "%s - directpath on (ifid %d subif %d)\n",
				   g_test_wlan_dev[pos]->name,
				   priv->dp_subif.port_id,
				   priv->dp_subif.subif);
		}
#else
		if (priv->dev_port >= 0) {
			SEQ_PRINTF(s,
				   "%s - directpath on (ifid %d subif %d)\n",
				   g_test_wlan_dev[pos]->name,
				   priv->dev_port, 0);
		}
#endif
		else
			SEQ_PRINTF(s, "%s - directpath off\n",
				   g_test_wlan_dev[pos]->name);
	}

	pos++;
	if (pos >= MAX_TESTWLAN_NUM)
		pos = -1;

	return pos;
}

#if 0 /* TODO : anath */
void unregister_from_dp(struct net_device *dev)
{
	struct test_wlan_priv_data *priv;
	int32_t dp_port_id = -1;
	int32_t dev_port = -1;
	int i;

	priv = netdev_priv(dev);
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
	dp_port_id = priv->dp_subif.port_id;
#else	
	dev_port = priv->dev_port;
#endif
	if (dp_port_id <= 0) {
		PRINTK
		    ("Cannot undregister %s since it is not reigstered yet\n",
		     dev->name);
		return;
	}

	/*unregister all subif with same port_id first */
	for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++) {
		if (g_test_wlan_dev[i]) {
			struct test_wlan_priv_data *priv =
			    netdev_priv(g_test_wlan_dev[i]);
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
			if (priv->dp_subif.port_id != dp_port_id)
#else
			if (priv->dev_port != dev_port)
#endif
				continue;
			if (priv->f_dp != TESTWLAN_F_REGISTER_SUBIF)
				continue;

#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
			if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x") ||
			    !ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
				if (dc_dp_register_subif
				    (priv->owner, g_test_wlan_dev[i], NULL,
				     &priv->dp_subif,
				     0) != DP_SUCCESS) {
					PRINTK("dc_dp_register_subif failed for %s \
						with port_id/subif %d/%d ?\n", dev->name, priv->dp_subif.port_id, priv->dp_subif.subif);
				}
			}
#endif
#if (defined(CONFIG_LTQ_PPA_GRX500) && CONFIG_LTQ_PPA_GRX500) && \
	(defined(CONFIG_LTQ_PPA_API_DIRECTPATH) && CONFIG_LTQ_PPA_API_DIRECTPATH)
			if (!ltq_strcmpi(priv->flag_str, "DIRECTPATH")) {
			}
#endif
			priv->f_dp = TESTWLAN_F_REGISTER_DEV;
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
			priv->dp_subif.subif = 0;
#endif
		}
	}

	/*unregister/deallocate devices and reset all devices with same port_id */
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
	if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x") ||
	    !ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
		if (dc_dp_register_dev(priv->owner, dp_port_id, NULL, DC_DP_F_DEREGISTER)
		    != DP_SUCCESS) {
			PRINTK("dp_unregister_dev failed for %s \
				with port_id %d ??? \n", dev->name, dp_port_id);
		}
		//dma_free_coherent(NULL, 8, priv->dccntr.dev2soc_ret_enq_base, (dma_addr_t)priv->dccntr.dev2soc_ret_enq_phys_base);
		kfree(priv->dccntr.dev2soc_ret_enq_base);
		if (dc_dp_alloc_port(priv->owner, priv->dev_port, dev, dp_port_id,
				  DC_DP_F_DEREGISTER) != DP_SUCCESS) {
			PRINTK
			    ("dp_dealloc_port failed for %s with port_id %d \n",
			     dev->name, dp_port_id);
		}
	}
#endif
#if (defined(CONFIG_LTQ_PPA_GRX500) && CONFIG_LTQ_PPA_GRX500) && \
	(defined(CONFIG_LTQ_PPA_API_DIRECTPATH) && CONFIG_LTQ_PPA_API_DIRECTPATH)
	if (!ltq_strcmpi(priv->flag_str, "DIRECTPATH")) {
	}
#endif
	for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++) {
		if (g_test_wlan_dev[i]) {
			struct test_wlan_priv_data *priv =
			    netdev_priv(g_test_wlan_dev[i]);
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
			if (priv->dp_subif.port_id != dp_port_id)
#else			
			if (priv->dev_port != dev_port)
#endif			
				continue;

			priv->f_dp = TESTWLAN_F_FREE;
			priv->dp_subif.port_id = 0;
		}
	}
}
#endif

static int get_dp_port_flag_str_size(void)
{
	return ARRAY_SIZE(dp_port_flag_str);
}

#if 0
/*to find out the device type index via its flag name. for example eth_lan, eth_wan,....
return value:
-1: not found
>=0: type index
*/
static int get_dev_type_index(char *flag_name)
{
	int i;

	for (i = 1; i < get_dp_port_flag_str_size(); i++) {	/*skip i = 0 */
		if (ltq_strcmpi(flag_name, dp_port_flag_str[i]) == 0) {
			return i;
			;
		}
	}
	return -1;
}
#endif

static ssize_t proc_write_datapath(struct file *file, const char *buf,
				 size_t count, loff_t *ppos)
{
	char str[300];
	int len, rlen;
	char *ifname = NULL;
	char *param_list[10] = {
		NULL
	};
	int param_list_num = 0;
	struct test_wlan_priv_data *priv = NULL;
	int i, k;
	uint32_t dev_port = 0;
	//int flag_index = 0;
	char *flag_str = NULL;
	char *dev_port_str = NULL;

	len = count < sizeof(str) ? count : sizeof(str) - 1;
	rlen = len - copy_from_user(str, buf, len);
	str[rlen] = 0;
	param_list_num =
	    ltq_split_buffer(str, param_list, ARRAY_SIZE(param_list));
	if (param_list_num < 3)
		goto help;

	ifname = param_list[2];
	i = find_testwlan_index_via_name(ifname);	/*device must ready before register to Datapath */
	if (i < 0)
		goto help;

	priv = netdev_priv(g_test_wlan_dev[i]);

	if (((ltq_strcmpi(param_list[0], "register") == 0) ||
	     (ltq_strcmpi(param_list[0], "reg") == 0)) &&
	    (ltq_strcmpi(param_list[1], "dev") == 0)) {
		if (param_list_num < 5)
			goto help;
		if (priv->f_dp == TESTWLAN_F_REGISTER_SUBIF) {
			PRINTK("Dev %s already registered as subif\n", ifname);
			goto exit;
		} else if (priv->f_dp == TESTWLAN_F_REGISTER_DEV) {
			PRINTK("Dev %s already registered as dev\n", ifname);
			goto exit;
		}
		dev_port_str = param_list[3];
		flag_str = param_list[4];
		PRINTK("dev_port_str=%s\n",
		       dev_port_str ? dev_port_str : "NULL");
		PRINTK("flag_str=%s\n", flag_str ? flag_str : "NULL");
		PRINTK("Try to register dev %s to datapath\n", ifname);

		dev_port = ltq_atoi(dev_port_str);
		//flag_index = get_dev_type_index(flag_str);
		//if (flag_index <= 0) {
		//	PRINTK("Not vailid device type:%s(%d)\n",
		//	       flag_str, flag_index);
		//	goto help;
		//}
		priv->owner = &g_test_wlan_module[i];
		sprintf(priv->owner->name, "module%02d", i);
		priv->dev_port = dev_port;
		//priv->flag_index = flag_index;
		if (flag_str) {
			strncpy(priv->flag_str, flag_str, (sizeof(priv->flag_str) - 1));
			priv->flag_str[(sizeof(priv->flag_str) - 1)] = '\0';
		}
		if (register_dev(i) != 0) {
			PRINTK("Registration is failed for %s\n", ifname);
			goto exit;
		}
		PRINTK("Succeed to register dev %s dev_port %d to \
			datapath with dp_port %d \n", ifname, dev_port,
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
			priv->dp_subif.port_id
#else
			priv->dev_port
#endif
            );
	} else if (((ltq_strcmpi(param_list[0], "register") == 0)
		    || (ltq_strcmpi(param_list[0], "reg") == 0))
		   && (ltq_strcmpi(param_list[1], "subif") == 0)) {
		if (param_list_num < 4)
			goto help;
		PRINTK("Try to register subif %s to datapath\n", ifname);
		if (priv->f_dp == TESTWLAN_F_REGISTER_DEV) {	/*already alloc a port and registered dev */
#if CONFIG_WAVE600_USING_DP_API
			if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
				if (dp_register_subif
				    (priv->owner, g_test_wlan_dev[i], g_test_wlan_dev[i]->name,
				     &priv->dp_subif,
				     0) != DP_SUCCESS) {
					PRINTK
					    ("dp_register_subif failed for %s\n and port_id %d",
					     g_test_wlan_dev[i]->name, priv->dp_subif.port_id);
					goto exit;
				}
			}
			else
#endif
			if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x")) {
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
				if (dc_dp_register_subif
				    (priv->owner, g_test_wlan_dev[i], NULL,
				     &priv->dp_subif,
				     0) != DC_DP_SUCCESS) {
					PRINTK
					    ("dc_dp_register_subif failed for %s\n and port_id %d",
					     g_test_wlan_dev[i]->name, priv->dp_subif.port_id);
					goto exit;
				}
#endif
			}
	    		else if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
				if (dc_dp_register_subif
				    (THIS_MODULE, g_test_wlan_dev[i], NULL,
				     &priv->dp_subif,
				     0) != DC_DP_SUCCESS) {
					PRINTK
					    ("dc_dp_register_subif failed for %s\n and port_id %d",
					     g_test_wlan_dev[i]->name, priv->dp_subif.port_id);
					goto exit;
				}
#endif
			}
			else if (!ltq_strcmpi(priv->flag_str, "DIRECTPATH")) {
			}
			priv->f_dp = TESTWLAN_F_REGISTER_SUBIF;
			PRINTK("Succeed to register subitf %s dev port %d: "
				"dp_port %d subif %d dp_flag=%d\n", ifname, priv->dev_port,
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
				priv->dp_subif.port_id, priv->dp_subif.subif,
#else
				0, 0,
#endif
				priv->f_dp);
		} else if (priv->f_dp == TESTWLAN_F_FREE) {
			dev_port_str = param_list[3];
			PRINTK("dev_port_str=%s\n", dev_port_str);
			dev_port = ltq_atoi(dev_port_str);
			if (get_port_info_via_dev_port(priv, dev_port) != 0) {
				PRINTK("Failed to register subif %s to dev port %s: Try to register dev first\n", ifname, dev_port_str);
				goto exit;
			}
#if CONFIG_WAVE600_USING_DP_API
			if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
				if (dp_register_subif
				    (priv->owner, g_test_wlan_dev[i], g_test_wlan_dev[i]->name,
				     &priv->dp_subif,
				     0) != DP_SUCCESS) {
					goto exit;
				}
			} else
#endif
			if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE5x")) {
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
				if (dc_dp_register_subif
				    (priv->owner, g_test_wlan_dev[i], NULL,
				     &priv->dp_subif,
				     0) != DP_SUCCESS) {
					goto exit;
				}
#endif
			}
	    		else if (!ltq_strcmpi(priv->flag_str, "FAST_WAVE6x")) {
#if ((defined(CONFIG_DIRECTCONNECT_DP_API) && CONFIG_DIRECTCONNECT_DP_API) || \
	(defined(CONFIG_DIRECTCONNECT_DP_API_MODULE) && CONFIG_DIRECTCONNECT_DP_API_MODULE))
				if (dc_dp_register_subif
				    (THIS_MODULE, g_test_wlan_dev[i], NULL,
				     &priv->dp_subif,
				     0) != DP_SUCCESS) {
					goto exit;
				}
#endif
			}
			else if (!ltq_strcmpi(priv->flag_str, "DIRECTPATH")) {
			}
			priv->f_dp = TESTWLAN_F_REGISTER_SUBIF;
			PRINTK
			    ("Succeed to register subitf %s dev port %d:dp_port %d subif %d\n",
			     ifname, priv->dev_port,
#if (defined(CONFIG_LTQ_DATAPATH) && CONFIG_LTQ_DATAPATH) || (defined(CONFIG_INTEL_DATAPATH) && CONFIG_INTEL_DATAPATH)
				 priv->dp_subif.port_id, priv->dp_subif.subif
#else
				 0, 0
#endif
                 );
		} else if (priv->f_dp == TESTWLAN_F_REGISTER_SUBIF) {
			PRINTK("Subif %s already registered\n", ifname);
			goto exit;
		} else {
			PRINTK("Failed for uknown reason:%d\n", priv->f_dp);
			goto exit;
		}
	} else if (((ltq_strcmpi(param_list[0], "unregister") == 0) ||
		    (ltq_strcmpi(param_list[0], "unreg") == 0)) &&
		   (ltq_strcmpi(param_list[1], "dev") == 0)) {
		PRINTK("Try to unregister dev %s from datapath\n", ifname);
		dev_port = priv->dev_port;
		for (k = 0; k < ARRAY_SIZE(g_test_wlan_dev); k++) {
			if (g_test_wlan_dev[k]) {	/*unregister all devices with same port_id */
				priv = netdev_priv(g_test_wlan_dev[k]);
				if (!priv || (dev_port != priv->dev_port))
					continue;
				unregister_dev(k);
			}
		}
	} else if (((ltq_strcmpi(param_list[0], "unregister") == 0) ||
		    (ltq_strcmpi(param_list[0], "unreg") == 0)) &&
		   (ltq_strcmpi(param_list[1], "subif") == 0)) {
		PRINTK("Try to unregister subif %s from datapath\n", ifname);
		if (priv->f_dp == TESTWLAN_F_REGISTER_SUBIF) {
			unregister_dev(i);
		} else {
			PRINTK
			    ("subif %s cannot be unregistered since it is not a registered subif yet\n",
			     ifname);
		}
	} else {
		PRINTK("Wrong command: %s %s\n", param_list[0],
		       param_list[1]);
		goto help;
	}
 exit:
	return count;
 help:
/*     param_list[0]    [1]    [2]         [3]      [4]     [5]            */
	PRINTK
	    ("echo register   dev   <dev_name> <dev_port> <type> > /proc/net/testwlan/datapath\n");
	PRINTK
	    ("echo unregister dev   <dev_name>                             > /proc/net/testwlan/datapath\n");
	PRINTK
	    ("echo register   subif <dev_name> <dev_port>                   > /proc/net/testwlan/datapath\n");
	PRINTK
	    ("echo unregister subif <dev_name>                             > /proc/net/testwlan/datapath\n");
	PRINTK("Device Type:\n");
	for (i = 1; i < get_dp_port_flag_str_size(); i++)	/*skip i 0 */
		PRINTK("\t%s\n", dp_port_flag_str[i]);
	return count;
}

//#if defined(ENABLE_TXDBG) && ENABLE_TXDBG /* FIXME */

static struct sk_buff *alloc_cputx_skb(int skb_len)
{
	struct sk_buff *new_skb = dev_alloc_skb(skb_len);
	if (!new_skb) {
		return NULL;
		;
	}
	return new_skb;
}

static int init_cputx_skb(void)
{
	struct ethhdr *eth_h;
	struct iphdr *iph;
	struct udphdr *udph;
	uint32_t srcip, dstip, i;
	char *dst_mac[] = {
		"00", "01", "02", "03", "04", "05"
	};
	char *src_mac[] = {
		"00", "10", "E0", "00", "00", "01"
	};
	char *src_ip[] = {
		"192", "168", "1", "100"
	};
	char *dst_ip[] = {
		"192", "168", "1", "200"
	};
	g_tx_skb = alloc_cputx_skb(MAX_SKB_SIZE);
	if (!g_tx_skb) {
		PRINTK("failed to alloc skb. initialization failed\n");
		return -1;
	}
	if (g_pkt_size > MAX_PKT_SIZE) {
		g_pkt_size = MAX_PKT_SIZE;
	}
	skb_reserve(g_tx_skb, 8);	/*reserve for flag header */
	skb_put(g_tx_skb, g_pkt_size);
	eth_h = (struct ethhdr *)g_tx_skb->data;
	for (i = 0; i < 6; i++) {
		eth_h->h_dest[i] = ltq_atoi(dst_mac[i]);
		eth_h->h_source[i] = ltq_atoi(src_mac[i]);
	}
	eth_h->h_proto = htons(ETH_P_IP);
	iph = (struct iphdr *)(g_tx_skb->data + sizeof(struct ethhdr));
	memset(iph, 0, sizeof(struct iphdr));
	iph->version = 4;
	iph->ihl = 5;
	iph->tot_len = g_pkt_size - 14;	/*totoal pkt size - ethernet hdr */
	iph->ttl = 64;
	iph->protocol = IPPROTO_UDP;
	iph->check = 0;
	srcip = (ltq_atoi(src_ip[0]) << 24) |
	    (ltq_atoi(src_ip[1]) << 16) |
	    (ltq_atoi(src_ip[2]) << 8) | (ltq_atoi(src_ip[3]));
	dstip = (ltq_atoi(dst_ip[0]) << 24) |
	    (ltq_atoi(dst_ip[1]) << 16) |
	    (ltq_atoi(dst_ip[2]) << 8) | (ltq_atoi(dst_ip[3]));
	iph->saddr = srcip;
	iph->daddr = dstip;
	iph->check = ip_fast_csum(iph, iph->ihl);
	udph = (struct udphdr *)(g_tx_skb->data + sizeof(struct ethhdr) +
				 sizeof(struct iphdr));
	udph->source = htons(1024);
	udph->dest = htons(1024);
	udph->check = 0;
	udph->len = g_pkt_size - 14 - 20;	/*totol pkt size - CRC - ethernet hdr - ip hdr */
	return 0;
}

static void reset_pkt_size(struct sk_buff *skb, uint32_t new_size)
{
	struct iphdr *iph;
	struct udphdr *udph;
	int len = new_size - skb->len;
	skb_put(skb, len);
	iph = (struct iphdr *)(skb->data + sizeof(struct ethhdr));
	iph->tot_len = iph->tot_len + len;
	iph->check = 0;
	iph->check = ip_fast_csum(iph, iph->ihl);
	udph = (struct udphdr *)(skb->data + sizeof(struct ethhdr) +
				 sizeof(struct iphdr));
	udph->len += len;
	return;
}

static void reset_tx_dev(char *dev_name, struct sk_buff *pskb)
{
	struct net_device *pdev;
	pdev = ltq_dev_get_by_name(dev_name);
	if (pdev) {
		dev_put(pdev);
		;
	}
	pskb->dev = pdev;
	return;
}

static int send_out_pkt(void *arg)
{
	struct sk_buff *skb_tx;
	uint32_t tx_cnt = 1;
	g_tx_mib = 0;
	g_tx_start = TX_START;
	if (!g_tx_skb->dev) {
		goto __THREAD_STOP;
		;
	}
	while (!kthread_should_stop() && tx_cnt <= g_tx_count) {
		if (tx_cnt % g_tx_pause_cnt == 0)
			schedule_timeout_uninterruptible(1);
		skb_tx = skb_clone(g_tx_skb, GFP_ATOMIC);
		if (skb_tx)
			dev_queue_xmit(skb_tx);
		g_tx_mib++;
		tx_cnt++;
	}
 __THREAD_STOP:
	g_tx_start = TX_STOP;
	return 0;
}

static void send_out_manager(char *dev_name)
{
	struct net_device *dev;
	if (!g_tx_count) {
		PRINTK("Warning: tx count is zero, stop sending\n");
		return;
	}
	if (!g_tx_skb) {
		PRINTK("Error: tx skb is not init\n");
		return;
	}
	dev = ltq_dev_get_by_name(dev_name);
	if (dev) {
		dev_put(dev);
		;
	} else {
		PRINTK("Cannot find the dev by name: %s\n", dev_name);
		return;
	}
	if (!ltq_get_xmit_fn(dev)) {
		PRINTK("The dev %s don't support xmit function", dev_name);
		return;
	}
	if (g_tx_ts && g_tx_start == TX_START) {
		kthread_stop(g_tx_ts);
		;
	}
	g_tx_ts = kthread_run(send_out_pkt, NULL, "sending_out");
	if (IS_ERR(g_tx_ts)) {
		PRINTK("Cannot alloc a new thread to send the pkt\n");
		g_tx_ts = NULL;
		g_tx_start = TX_STOP;
	}
	return;
}

#if 0 // FIXME
static int print_skb(struct seq_file *s, struct sk_buff *skb)
{
	int len = 0;
	struct ethhdr *eth_h = (struct ethhdr *)skb->data;
	struct iphdr *iph =
	    (struct iphdr *)(skb->data + sizeof(struct ethhdr));
	if (!skb)
		return len;
	SEQ_PRINTF(s, "dst mac: %x%x:%x%x:%x%x:%x%x:%x%x:%x%x\n",
		   eth_h->h_dest[0] >> 4, eth_h->h_dest[0],
		   eth_h->h_dest[1] >> 4, eth_h->h_dest[1],
		   eth_h->h_dest[2] >> 4, eth_h->h_dest[2],
		   eth_h->h_dest[3] >> 4, eth_h->h_dest[3],
		   eth_h->h_dest[4] >> 4, eth_h->h_dest[4],
		   eth_h->h_dest[5] >> 4, eth_h->h_dest[5]);
	SEQ_PRINTF(s, "src mac: %x%x:%x%x:%x%x:%x%x:%x%x:%x%x\n",
		   eth_h->h_source[0] >> 4, eth_h->h_source[0],
		   eth_h->h_source[1] >> 4, eth_h->h_source[1],
		   eth_h->h_source[2] >> 4, eth_h->h_source[2],
		   eth_h->h_source[3] >> 4, eth_h->h_source[3],
		   eth_h->h_source[4] >> 4, eth_h->h_source[4],
		   eth_h->h_source[5] >> 4, eth_h->h_source[5]);
	SEQ_PRINTF(s, "dst ip: %d.%d.%d.%d, src ip: %d.%d.%d.%d\n",
		   (iph->daddr >> 24) & 0xFF, (iph->daddr >> 16) & 0xFF,
		   (iph->daddr >> 8) & 0xFF, iph->daddr & 0xFF,
		   (iph->saddr >> 24) & 0xFF, (iph->saddr >> 16) & 0xFF,
		   (iph->saddr >> 8) & 0xFF, iph->saddr & 0xFF);
	return len;
}
#endif

static ssize_t proc_write_ring(struct file *file, const char *buf,
			    size_t count, loff_t *data)
{
	int i = find_testwlan_index_via_name("testwlan0");
	if( i < 0 ) return count;
	struct test_wlan_priv_data *priv = NULL;

	priv = netdev_priv(g_test_wlan_dev[i]);

	dc_dp_handle_ring_sw(priv->owner, priv->dp_subif.port_id,
		       	g_test_wlan_dev[i], &priv->resources.rings.dev2soc_except,
		       	DC_DP_F_RX_FRAG_EXCEPT);
	return count;
}

static void proc_read_ring(struct seq_file *s)
{
}

static void proc_read_cputx(struct seq_file *s)
{
	SEQ_PRINTF(s, "frame count: 0x%x\n", g_tx_count);
	SEQ_PRINTF(s, "frame mib count: 0x%x\n", g_tx_mib);
	SEQ_PRINTF(s, "TX dev name: %s\n", g_tx_dev);
	SEQ_PRINTF(s, "Tx pause num: %d\n", g_tx_pause_cnt);
	SEQ_PRINTF(s, "frame size: %d(Not including CRC)\n", g_pkt_size);
#if 0 // FIXME
	len += print_skb(page + off + len, g_tx_skb);
	*eof = 1;
#endif
}

static ssize_t proc_write_cputx(struct file *file, const char *buf,
			    size_t count, loff_t *data)
{
	char str[300];
	int len, rlen;
	int param_list_num = 0;
	char *param_list[10];

	len = count < sizeof(str) ? count : sizeof(str) - 1;
	rlen = len - copy_from_user(str, buf, len);
	str[rlen] = 0;
	param_list_num =
	    ltq_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (ltq_strncmpi(param_list[0], "help", 4) == 0) {
		PRINTK
		    ("echo count <num> > /proc/net/testwlan/cputx (0xFFFF would be continues sending)\n");
		PRINTK("echo start/stop > /proc/net/testwlan/cputx\n");
		PRINTK("echo dev <name> > /proc/net/testwlan/cputx\n");
		PRINTK
		    ("echo intcnt <num> /proc/net/testwlan/cputx  (min: 1, max: %d)\n",
		     MAX_PAUSE_CNT);
		PRINTK
		    ("echo pktsize <num> /proc/net/testwlan/cputx (min: %d, max:%d)\n",
		     MIN_PKT_SIZE, MAX_PKT_SIZE);
	} else if (ltq_strncmpi(param_list[0], "count", 5) == 0) {
		g_tx_count = ltq_atoi(param_list[1]);
	} else if (ltq_strncmpi(param_list[0], "start", 5) == 0) {
		if (g_tx_start == TX_START) {
			PRINTK("still running, please stop first\n");
			return count;
		}
		send_out_manager(g_tx_dev);
	} else if (ltq_strncmpi(param_list[0], "stop", 4) == 0) {
		DP_LIB_LOCK(&g_tx_start_lock);
		if (g_tx_start == TX_STOP) {
			goto __EXIT_TX_LOCK;
			;
		}
		kthread_stop(g_tx_ts);
		g_tx_ts = NULL;
		DP_LIB_UNLOCK(&g_tx_start_lock);
	} else if (ltq_strncmpi(param_list[0], "dev", 3) == 0) {

		strncpy(g_tx_dev, param_list[1], sizeof(g_tx_dev) - 1);
		g_tx_dev[IFNAMSIZ - 1] = '\0';
		reset_tx_dev(g_tx_dev, g_tx_skb);
	} else if (ltq_strncmpi(param_list[0], "dumpskb", 7) == 0) {
		if (g_tx_skb) {
			dump_skb(g_tx_skb, g_tx_skb->len, "dump tx skb",
				 -1, 0);
		}
	} else if (ltq_strncmpi(param_list[0], "intcnt", 6) == 0) {
		g_tx_pause_cnt = ltq_atoi(param_list[1]);
		if (g_tx_pause_cnt == 0) {
			PRINTK("Cannot set intermittent number to zero!!!\n");
			g_tx_pause_cnt = DEFAULT_PAUSE_CNT;
		} else if (g_tx_pause_cnt > MAX_PAUSE_CNT) {
			PRINTK("Set to MAX PAUSE COUNT: %d\n", MAX_PAUSE_CNT);
			g_tx_pause_cnt = MAX_PAUSE_CNT;
		}
	} else if (ltq_strncmpi(param_list[0], "pktsize", 7) == 0) {
		DP_LIB_LOCK(&g_tx_start_lock);
		if (g_tx_start == TX_START) {
			PRINTK
			    ("Cannot change the packet size when sending traffic!\n");
			goto __EXIT_TX_LOCK;
		}
		g_pkt_size = ltq_atoi(param_list[1]);
		if (g_pkt_size < MIN_PKT_SIZE || g_pkt_size > MAX_PKT_SIZE) {
			PRINTK
			    ("pkt size cannot be less than %d, or larger than %d\n",
			     MIN_PKT_SIZE, MAX_PKT_SIZE);
			g_pkt_size = DEFAULT_PKT_SIZE;
		}
		reset_pkt_size(g_tx_skb, g_pkt_size);
		DP_LIB_UNLOCK(&g_tx_start_lock);
	} else if (ltq_strncmpi(param_list[0], "verifytx", 8) == 0) {
#if 0 // FIXME
		struct net_device *tx_dev = NULL;
#if (CONFIG_WAVE600_USING_DP_API == 0)
		struct test_wlan_priv_data *priv = NULL;
#endif
		if (g_tx_dev[0] == '\0') {
			PRINTK
			    ("tx_dev is NULL!\n");
			return 0;
		}
		
		tx_dev = ltq_dev_get_by_name(g_tx_dev);
		if(tx_dev){
			dev_put(tx_dev);
		}else{
			printk("Cannot find the dev by name: %s\n", g_tx_dev);
			return 0;
		}

#if (CONFIG_WAVE600_USING_DP_API == 0)
		priv = netdev_priv(tx_dev);
{
		int i;
		uint64_t reg;
		uint32_t reg1, reg2;
		uint32_t addr = (uint32_t)priv->resources.rings.soc2dev.base;

		for (i = 0; i < priv->resources.rings.soc2dev.size; i++) {
			reg = *((uint64_t *)(addr));
    		reg1 = (uint32_t)(reg >> 32);
    		reg2 = (reg & 0xFFFFFFFF);
    		printk("DESC0_%d_EGP_4 : addr:0x%8x, val[32-63]: 0x%8x, val[0-31]: 0x%8x\n", i, addr, reg1, reg2);

			addr += 8;
			reg = *((uint64_t *)(addr));
    		reg1 = (uint32_t)(reg >> 32);
    		reg2 = (reg & 0xFFFFFFFF);
    		printk("DESC1_%d_EGP_4 : addr:0x%8x, val[32-63]: 0x%8x, val[0-31]: 0x%8x\n", i, addr, reg1, reg2);

			addr += 8;
		}
}
#endif
#endif
	}
	return count;
 __EXIT_TX_LOCK:
	DP_LIB_UNLOCK(&g_tx_start_lock);
	return count;
}

//#endif

/*###################################
*           Global Function
* ####################################
*/
#define PROC_NAME		"testwlan"
#define PROC_DBG		"dbg"
#define PROC_READ_DEV		"dev"
#define PROC_READ_MIB		"mib"
#define PROC_READ_DP		"datapath"
#define PROC_READ_CPUTX		"cputx"
#define PROC_WRITE_RING		"ring"
static struct ltq_proc_entry proc_entries[] = {
/* name             single_callback_t    multi_callback_t    multi_callback_start    write_callback_t  */
#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
	{PROC_DBG, proc_read_dbg, NULL, NULL, proc_write_dbg},
#endif
	{PROC_READ_DEV, NULL, proc_read_dev, NULL, proc_write_dev},
	{PROC_READ_MIB, NULL, proc_read_mib, NULL, proc_write_mib},
	{
	 PROC_READ_DP, NULL, proc_read_datapath, NULL,
	 proc_write_datapath},
//#if defined(ENABLE_TXDBG) && ENABLE_TXDBG /* FIXME */
	{PROC_READ_CPUTX, proc_read_cputx, NULL, NULL, proc_write_cputx},
//#endif
	{PROC_WRITE_RING, proc_read_ring, NULL, NULL, proc_write_ring},
};

static struct proc_dir_entry *proc_node;

static struct proc_dir_entry *proc_file_create(void)
{
	proc_node = proc_mkdir(PROC_NAME, init_net.proc_net);
	if (proc_node != NULL) {
		int i;
		for (i = 0; i < ARRAY_SIZE(proc_entries); i++)
			ltq_proc_entry_create(proc_node, &proc_entries[i]);
	} else {
		PRINTK("cannot create proc entry");
		return NULL;
	}
	return proc_node;
}

/*####################################
*           Init/Cleanup API
* ####################################
*/

static __init int test_wlan_drv_init_module(void)
{
	PRINTK("Loading test_wlan_drv driver ...... ");

	memset(g_test_wlan_dev, 0, sizeof(g_test_wlan_dev));
	memset(g_test_wlan_module, 0, sizeof(g_test_wlan_module));

	proc_file_create();
//#if defined(ENABLE_TXDBG) && ENABLE_TXDBG /* FIXME */
	spin_lock_init(&g_tx_start_lock);
	DP_LIB_LOCK(&g_tx_start_lock);
	init_cputx_skb();
#if CONFIG_WAVE600_USING_DP_API
	{
	struct dp_port_data dp_p_data = {0};
	dp_p_data.flag_ops = (DP_F_DATA_RESV_CQM_PORT | DP_F_DATA_EVEN_FIRST);
	dp_p_data.resv_num_port = 2;

	g_resvd_port = dp_alloc_port_ext(0, THIS_MODULE, NULL, 0, 0, NULL, &dp_p_data, 0);
	}
#endif
	DP_LIB_UNLOCK(&g_tx_start_lock);
//#endif
	PRINTK("Succeeded!\n");
	return 0;
}

static __exit void test_wlan_drv_exit_module(void)
{
	int i;

	PRINTK("Unloading test_wlan_drv driver ...... ");
	for (i = 0; i < ARRAY_SIZE(g_test_wlan_dev); i++)
		if (g_test_wlan_dev[i]) {
			delete_testwlan_dev(i);
		}
//#if defined(ENABLE_TXDBG) && ENABLE_TXDBG /* FIXME */
	if (g_tx_ts && g_tx_start == TX_START)
		kthread_stop(g_tx_ts);
	if (g_tx_skb != NULL)
		dev_kfree_skb_any(g_tx_skb);
//#endif
	for (i = 0; i < ARRAY_SIZE(proc_entries); i++)
		remove_proc_entry(proc_entries[i].name, proc_node);
	remove_proc_entry(PROC_NAME, init_net.proc_net);
	PRINTK("Succeeded!\n");

}

module_init(test_wlan_drv_init_module);
module_exit(test_wlan_drv_exit_module);

MODULE_AUTHOR("Anath Bandhu Garai");
MODULE_DESCRIPTION("Intel DC test driver (Supported XRX500)");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.1.1");
