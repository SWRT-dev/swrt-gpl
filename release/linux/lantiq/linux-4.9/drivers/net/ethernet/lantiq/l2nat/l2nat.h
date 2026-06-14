/*
  GPL LICENSE SUMMARY

  Copyright(c) 2016 Intel Corporation.

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
  The full GNU General Public License is included in this distribution
  in the file called LICENSE.GPL.

  Contact Information:
    Intel Corporation
    2200 Mission College Blvd.
    Santa Clara, CA  97052
*/

#ifndef __L2NAT_H__
#define __L2NAT_H__

#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/kref.h>
#include <linux/hashtable.h>

#define L2NAT_NUM_ENTRIES_BITS	8	/*256 entries*/

#define L2NAT_AGING_WAIT_INFO	10
#define L2NAT_AGING_WAIT_ARP	10

#define L2NAT_SEND_ARP_LIMIT	(HZ/3)

#define L2NAT_AGING_TIMEOUT_DEFAULT	(600 * HZ)

#define L2N_DBG_HASH	0x00000001	/* debug hash operations */
#define L2N_DBG_TX	0x00000002	/* debug tx flow */
#define L2N_DBG_RX	0x00000004	/* debug rx flow */
#define L2N_DBG_TMR	0x00000008	/* debug timer callbacks */
#define L2N_DBG_DHCP	0x00000010	/* debug dhcp modifications */
#define L2N_DBG_DEV	0x00000020	/* debug l2nat device list ops */
#define L2N_DBG_DBG	0x00000040	/* general module debugging */
#define L2N_DBG_INFO	0x00000080	/* general module debugging */
#define L2N_WARN	0x00000100	/* module warnings */
#define L2N_ERR		0x00000200	/* module errors */

extern bool hooks_registered;
extern u32 aging_timeout;
extern struct l2nat_global_stats glbl_stats;

extern unsigned int dbg_flags;

#define __l2n_pr_unlikely(level, flag, fmt, ...)			\
	do {								\
		if (unlikely((flag) & dbg_flags))			\
			printk(level pr_fmt(fmt), ##__VA_ARGS__);	\
	} while (0);

#define __l2n_pr_likely(level, flag, fmt, ...)				\
	do {								\
		if (likely((flag) & dbg_flags))				\
			printk(level pr_fmt(fmt) , ##__VA_ARGS__);	\
	} while (0);


#define l2n_dbg_hash(fmt, ...) \
	__l2n_pr_unlikely(KERN_INFO, L2N_DBG_HASH, fmt, ##__VA_ARGS__)

#define l2n_dbg_tx(fmt, ...) \
	__l2n_pr_unlikely(KERN_INFO, L2N_DBG_TX, fmt, ##__VA_ARGS__)

#define l2n_dbg_rx(fmt, ...) \
	__l2n_pr_unlikely(KERN_INFO, L2N_DBG_RX, fmt, ##__VA_ARGS__)

#define l2n_dbg_tmr(fmt, ...) \
	__l2n_pr_unlikely(KERN_INFO, L2N_DBG_TMR, fmt, ##__VA_ARGS__)

#define l2n_dbg_dhcp(fmt, ...) \
	__l2n_pr_unlikely(KERN_INFO, L2N_DBG_DHCP, fmt, ##__VA_ARGS__)

#define l2n_dbg_dev(fmt, ...) \
	__l2n_pr_unlikely(KERN_INFO, L2N_DBG_DEV, fmt, ##__VA_ARGS__)

#define l2n_dbg(fmt, ...) \
	__l2n_pr_unlikely(KERN_INFO, L2N_DBG_DBG, fmt, ##__VA_ARGS__)

#define l2n_info(fmt, ...) \
	__l2n_pr_likely(KERN_INFO, L2N_DBG_INFO, fmt, ##__VA_ARGS__)

#define l2n_warn(fmt, ...) \
	__l2n_pr_likely(KERN_WARNING, L2N_WARN, fmt, ##__VA_ARGS__)

#define l2n_err(fmt, ...) \
	__l2n_pr_likely(KERN_ERR, L2N_ERR, fmt, ##__VA_ARGS__)

struct snap_hdr {
	/* out1 + out2 = organization code */
	u8 out1;
	u16 out2;
	/* ethernet type */
	u16 ether_type;
};

struct hash_entry {
	__be32			ip;
	unsigned char		mac[ETH_ALEN];

	u64			tx_packets;
	u64			rx_packets;

	atomic_long_t		last_pkt_timestamp;
	unsigned long		first_pkt_timestamp;
	struct timer_list	timer;
	struct l2nat_dev	*l2nat;

	struct hlist_node	hlist;
	struct rcu_head		rcu;
	struct kref		kref;

	struct list_head	seqlist;
};

struct l2nat_global_stats {
	u64 preroute;
	u64 postroute;
};

struct l2nat_dev_stats {
	u64 rx_packets;
	u64 rx_bytes;
	u64 rx_dropped;
	u64 rx_arp;

	u64 tx_packets;
	u64 tx_bytes;
	u64 tx_dropped;
	u64 tx_arp;

	u64 arp_gen;
	u64 dhcp;

	u64 entries_add;
	u64 entries_del;
};

struct l2nat_dev {

	unsigned char *default_host;

	spinlock_t lock;

	DECLARE_HASHTABLE(hash, L2NAT_NUM_ENTRIES_BITS);
	u32 aging_timeout;

	wait_queue_head_t wq;
	atomic_t ent_count;

	unsigned char mac_for_arp[ETH_ALEN];
	__be32 ip_for_arp;
	unsigned long last_arp_sent_ts;

	atomic_t arp_info_needed;
	unsigned long last_arp_info_ts;

	struct l2nat_dev_stats stats;

	bool hash_en;

	struct net_device *dev;
	struct rcu_head rcu;
	struct list_head list;
	struct kref kref;
};

/* kernel doesn't have structure of the BOOTP/DHCP header
 * so here it is defined according to rfc2131
 */

struct dhcphdr {
	u8	op;
#define BOOTREQUEST   1
#define BOOTREPLY     2
	u8	htype;
	u8	hlen;
	u8	hops;
	u32	xid;
	u16	secs;
	u16	flags;
#define BOOTP_BRD_FLAG 0x8000
	u32	ciaddr;
	u32	yiaddr;
	u32	siaddr;
	u32	giaddr;
	u8	chaddr[16];
	u8	sname[64];
	u8	file[128];
	u32	magic; /* NB: actually magic is a part of options */
	u8	options[0];
} __packed __aligned(1);


void entry_aging_timer_function(unsigned long data);

void l2nat_proc_init(void);
void l2nat_proc_exit(void);

struct hash_entry *l2nat_entry_get(struct l2nat_dev *l2nat, __be32 ip);
void l2nat_entry_release(struct kref *kref);
void l2nat_hash_flush(struct l2nat_dev *l2nat);
void l2nat_hash_flush_sync(struct l2nat_dev *l2nat);
int l2nat_entry_check_add(struct l2nat_dev *l2nat, __be32 ip,
			  unsigned char *mac);
void entry_aging_timer_fn(unsigned long data);
void __gen_fake_arp_req(struct l2nat_dev *l2nat, __be32 sip,
			unsigned char *smac, __be32 dip);
void gen_fake_arp_req(struct l2nat_dev *l2nat, __be32 sip,
			     unsigned char *smac, __be32 dip);

void l2nat_del_device(struct net_device *dev);
int l2nat_add_device(struct net_device *dev);

void l2nat_dev_foreach(void (*fn)(struct l2nat_dev *l2nat, void *data),
		       void *arg);
int l2nat_dev_count(void);
void l2nat_dev_release(struct kref *kref);
struct l2nat_dev *l2nat_dev_get(const struct net_device *dev);
int l2nat_dev_del(struct net_device *dev);
int l2nat_dev_add(struct net_device *dev);
void l2nat_dev_list_init(void);
void l2nat_dev_list_exit(void);

static inline unsigned long last_packet_delta(struct hash_entry *ent)
{
	return jiffies -
		(unsigned long)atomic_long_read(&ent->last_pkt_timestamp);
}

static inline void l2nat_dev_hold(struct l2nat_dev *l2nat)
{
	kref_get(&l2nat->kref);
}

static inline void l2nat_dev_put(struct l2nat_dev *l2nat)
{
	kref_put(&l2nat->kref, l2nat_dev_release);
}

static inline void l2nat_entry_hold(struct hash_entry *ent)
{
	kref_get(&ent->kref);
}

static inline void l2nat_entry_put(struct hash_entry *ent)
{
	kref_put(&ent->kref, l2nat_entry_release);
}

#endif /* __L2NAT_H__ */
