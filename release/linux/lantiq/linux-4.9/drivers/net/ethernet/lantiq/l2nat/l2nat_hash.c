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

#define pr_fmt(fmt) "L2NAT:%s:%d " fmt, __func__, __LINE__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kref.h>
#include <linux/netdevice.h>
#include "l2nat.h"

/* search for an ip-MAC mapping entry, should be called with rcu_read_lock() */
static struct hash_entry *l2nat_entry_get_rcu(struct l2nat_dev *l2nat,
					       __be32 ip)
{
	struct hash_entry *ent;
	hash_for_each_possible_rcu(l2nat->hash, ent, hlist, ntohl(ip)) {
		if (ent->ip == ip)
			return ent;
	}

	return NULL;
}

/* search for an ip-MAC mapping entry in the hash table */
struct hash_entry *l2nat_entry_get(struct l2nat_dev *l2nat, __be32 ip)
{
	struct hash_entry *ent;

	rcu_read_lock();
	ent = l2nat_entry_get_rcu(l2nat, ip);
	if (ent)
		l2nat_entry_hold(ent);
	rcu_read_unlock();

	return ent;
}

/* free an ip-MAC mapping entry */
void l2nat_entry_release(struct kref *kref)
{
	struct hash_entry *ent = container_of(kref, struct hash_entry, kref);
	struct l2nat_dev *l2nat = ent->l2nat;
	del_timer(&ent->timer);

	l2n_dbg_hash("%s: freeing entry %pI4 %pM\n", l2nat->dev->name,
		     &ent->ip, &ent->mac);

	l2nat->stats.entries_del++;

	kfree(ent);
	atomic_dec(&l2nat->ent_count);

	/* wake up anyone waiting for all entries to release */
	wake_up_interruptible(&l2nat->wq);
	l2nat_dev_put(l2nat);
}

/* put an ip-MAC mapping entry for the hash, called from call_rcu */
static void l2nat_entry_free_rcu(struct rcu_head *rcu)
{
	struct hash_entry *ent = container_of(rcu, struct hash_entry, rcu);
	l2nat_entry_put(ent);
}

/* delete ip-MAC mapping entry, should be locked by l2nat->lock */
static void __l2nat_entry_del(struct hash_entry *ent)
{
	l2n_dbg_hash("%s: deleting entry %pI4 %pM\n", ent->l2nat->dev->name,
	    &ent->ip, &ent->mac);

	hash_del_rcu(&ent->hlist);
	call_rcu(&ent->rcu, l2nat_entry_free_rcu);

	/* entry has a pending timer, force it to expire to relase the
	 * counter reference.
	 * note - a running timer is not pending */
	if (timer_pending(&ent->timer))
		mod_timer(&ent->timer, jiffies);
}

/* flush all ip-MAC mapping entries */
void l2nat_hash_flush(struct l2nat_dev *l2nat)
{
	int i;
	struct hash_entry *ent;
	struct hlist_node *tmp;

	l2n_dbg_hash("flushing entries for %s\n", l2nat->dev->name);

	spin_lock_bh(&l2nat->lock);
	hash_for_each_safe(l2nat->hash, i, tmp, ent, hlist)
		__l2nat_entry_del(ent);
	spin_unlock_bh(&l2nat->lock);
}

/* flush all ip-MAC mapping entries, and wait for them to be freed */
void l2nat_hash_flush_sync(struct l2nat_dev *l2nat)
{
	l2nat_hash_flush(l2nat);

	l2n_dbg_hash("waiting for entries to be freed\n");
	wait_event_interruptible(l2nat->wq,
				 atomic_read(&l2nat->ent_count) == 0);
}

/* initialize a new ip-MAC mapping entry */
static inline void l2nat_entry_init(struct l2nat_dev *l2nat,
				    struct hash_entry *ent, __be32 ip, u8 *mac)
{
	ent->ip = ip;
	memcpy(&ent->mac, mac, ETH_ALEN);
	atomic_long_set(&ent->last_pkt_timestamp, jiffies);
	ent->first_pkt_timestamp = jiffies;
	ent->tx_packets++;
	ent->l2nat = l2nat;
	l2nat_dev_hold(l2nat);
	kref_init(&ent->kref);
	init_timer(&ent->timer);
	ent->timer.function = entry_aging_timer_fn;
	ent->timer.data = (unsigned long)ent;
	INIT_HLIST_NODE(&ent->hlist);
	atomic_inc(&l2nat->ent_count);

	l2n_dbg_hash("%s: initialized entry %pI4 %pM\n", l2nat->dev->name, &ip,
		     mac);
}

/*
 * check for an ip-MAC mapping entry,
 * if it is new - add it to the hash table
 * if it exists - update timestamp
 * if it exists but mac address changed - replace with new entry
 */
int l2nat_entry_check_add(struct l2nat_dev *l2nat, __be32 ip,
			  unsigned char *mac)
{
	int ret = 0;
	struct hash_entry *old_e, *new_e;
	unsigned long interval;

	if (unlikely(!ip))
		return 0;

	if (unlikely(!l2nat->hash_en))
		return 0;

	old_e = l2nat_entry_get(l2nat, ip);

	/* entry exists and mac did not change */
	if (likely(old_e && !memcmp(old_e->mac, mac, ETH_ALEN)))
		goto update_entry;

	new_e = kzalloc(sizeof(*new_e), GFP_ATOMIC);
	if (!new_e) {
		l2n_err("%s: failed to allocate entry! %pI4 %pM\n",
			l2nat->dev->name, &ip, &mac);
		return -1;
	}

	l2nat_entry_init(l2nat, new_e, ip, mac);

	spin_lock_bh(&l2nat->lock);

	hash_add_rcu(l2nat->hash, &new_e->hlist, ntohl(ip));

	/* mac address changed, replace old entry with new one */
	if (old_e && hash_hashed(&old_e->hlist))
		__l2nat_entry_del(old_e);

	spin_unlock_bh(&l2nat->lock);


	l2n_dbg_hash("%s: added entry %pI4 %pM\n", l2nat->dev->name, &ip, mac);

	if (l2nat->aging_timeout) {
		/* take extra ref for timer */
		l2nat_entry_hold(new_e);
		interval = l2nat->aging_timeout -
			   HZ * (L2NAT_AGING_WAIT_INFO + L2NAT_AGING_WAIT_ARP);
		mod_timer(&new_e->timer, jiffies + interval);
	}

	/* release the entry we just took */
	if (old_e)
		l2nat_entry_put(old_e);

	l2nat->stats.entries_add++;

	return ret;

update_entry:
	atomic_long_set(&old_e->last_pkt_timestamp, jiffies);
	old_e->tx_packets++;
	l2nat_entry_put(old_e);
	return ret;
}

/* ip-MAC mapping entry timer timeout handling */
void entry_aging_timer_fn(unsigned long data)
{
	struct hash_entry *ent = (struct hash_entry *)data;
	unsigned long delta, next_time_offset, send_arp_timeout;
	unsigned long get_info_timeout;
	struct l2nat_dev *l2nat = ent->l2nat;

	spin_lock_bh(&l2nat->lock);

	/* entry was removed by someone, release timer reference */
	if (!hash_hashed(&ent->hlist)) {
		l2nat_entry_put(ent);
		goto done;
	}

	delta = last_packet_delta(ent);
	send_arp_timeout = l2nat->aging_timeout - L2NAT_AGING_WAIT_ARP * HZ;
	get_info_timeout = send_arp_timeout - L2NAT_AGING_WAIT_INFO * HZ;

	/* delete entry if its inactive longer than allowed */
	if (delta >= l2nat->aging_timeout) {
		l2n_dbg_tmr("timer deleting entry %pI4 inactive %u sec\n",
			    &ent->ip,
			    jiffies_to_msecs(last_packet_delta(ent)) / 1000);

		__l2nat_entry_del(ent);
		l2nat_entry_put(ent);
		goto done;
	}

	if (delta < get_info_timeout) {
		/* no need for any actions, reschedule on the moment
		 * of next arp attempt, clear waiting flags if any.
		 */
		next_time_offset = get_info_timeout - delta;
		l2n_dbg_tmr("timer entry %pI4 active, offset %lu, delta %lu\n",
		    &ent->ip, next_time_offset, delta);

		goto modify_out;
	}

	if (delta < send_arp_timeout) {
		atomic_set(&l2nat->arp_info_needed, 1);
		next_time_offset = L2NAT_AGING_WAIT_INFO * HZ;
		l2n_dbg_tmr("timer req arp to %pI4, offset %lu, delta %lu\n",
			    &ent->ip, next_time_offset, delta);

		goto modify_out;
	}

	next_time_offset = L2NAT_AGING_WAIT_ARP * HZ;

	/* only send arp if info is not stale */
	if (jiffies - l2nat->last_arp_info_ts <= L2NAT_AGING_WAIT_INFO * HZ) {
		__gen_fake_arp_req(l2nat, l2nat->ip_for_arp, l2nat->mac_for_arp,
				   ent->ip);

		l2n_dbg_tmr("timer sent arp to %pI4 offset %lu\n", &ent->ip,
			    next_time_offset);
	}

modify_out:
	mod_timer(&ent->timer, jiffies + next_time_offset);
done:
	spin_unlock_bh(&l2nat->lock);
}
