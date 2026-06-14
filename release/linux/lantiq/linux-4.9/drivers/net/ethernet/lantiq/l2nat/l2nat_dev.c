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
#include <linux/wait.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/rtnetlink.h>
#include "l2nat.h"

/* device list */
static struct list_head dev_list;

/* device list lock */
static spinlock_t list_lock;

/*wait queue for rmmod */
static wait_queue_head_t wq;

/* device list size*/
static atomic_t dev_count = ATOMIC_INIT(0);

/* entry aging timeout */
u32 aging_timeout = L2NAT_AGING_TIMEOUT_DEFAULT;

static inline void l2nat_dev_disable_hash(struct l2nat_dev *l2nat)
{
	l2nat->hash_en = false;
}

int l2nat_dev_count(void)
{
	return atomic_read(&dev_count);
}

void l2nat_dev_foreach(void (*fn)(struct l2nat_dev *l2nat, void *data),
		       void *arg)
{
	struct l2nat_dev *l2nat;
	rcu_read_lock();
	list_for_each_entry(l2nat, &dev_list, list)
		fn(l2nat, arg);
	rcu_read_unlock();
}

/* initialize the private data of a l2nat device */
static int l2nat_dev_init(struct l2nat_dev *l2nat)
{
	l2n_dbg_dev("initializing l2nat dev %s\n", l2nat->dev->name);

	spin_lock_init(&l2nat->lock);

	/* init device list and kref */
	INIT_LIST_HEAD(&l2nat->list);
	kref_init(&l2nat->kref);

	hash_init(l2nat->hash);
	l2nat->hash_en = true;

	l2nat->last_arp_sent_ts = jiffies;
	l2nat->default_host = l2nat->dev->dev_addr;
	l2nat->aging_timeout = aging_timeout;

	init_waitqueue_head(&l2nat->wq);
	atomic_set(&l2nat->ent_count, 0);

	atomic_set(&l2nat->arp_info_needed, 0);

	l2n_dbg_dev("l2nat dev initialized for %s\n", l2nat->dev->name);

	return 0;
}

/* free a l2nat device from the list */
void l2nat_dev_release(struct kref *kref)
{
	struct l2nat_dev *l2nat = container_of(kref, struct l2nat_dev, kref);

	l2n_dbg_dev("freeing l2nat dev %s\n", l2nat->dev->name);

	dev_put(l2nat->dev);

	kfree(l2nat);

	atomic_dec(&dev_count);

	/* wake up in case rmmod is waiting for all devices to be freed */
	wake_up_interruptible(&wq);
}

/* put a l2nat device from the list from call_rcu */
static void l2nat_dev_free_rcu(struct rcu_head *rcu)
{
	struct l2nat_dev *l2nat = container_of(rcu, struct l2nat_dev, rcu);
	l2nat_dev_put(l2nat);
}

/* remove all netdevices from the l2nat list */
static void l2nat_dev_list_flush(void)
{
	LIST_HEAD(dying_list);
	struct l2nat_dev *l2nat, *tmp;

	l2n_dbg_dev("flusing l2nat dev list\n");

	spin_lock_bh(&list_lock);
	list_for_each_entry_safe(l2nat, tmp, &dev_list, list) {
		l2nat_dev_hold(l2nat);
		list_del_rcu(&l2nat->list);
		l2nat_dev_disable_hash(l2nat);
		list_add(&l2nat->list, &dying_list);
	}
	spin_unlock_bh(&list_lock);

	list_for_each_entry(l2nat, &dying_list, list) {
		l2nat_hash_flush_sync(l2nat);
		l2nat_dev_put(l2nat);
		call_rcu(&l2nat->rcu, l2nat_dev_free_rcu);
	}
}

/* search for a netdevice in the l2nat list - called under rcu_read_lock */
static struct l2nat_dev *l2nat_dev_get_rcu(const struct net_device *dev)
{
	struct l2nat_dev *l2nat;

	list_for_each_entry_rcu(l2nat, &dev_list, list) {
		if (dev == l2nat->dev)
			return l2nat;
	}
	return NULL;
}

/* search for a netdevice in the l2nat list */
struct l2nat_dev *l2nat_dev_get(const struct net_device *dev)
{
	struct l2nat_dev *l2nat;

	rcu_read_lock();
	l2nat = l2nat_dev_get_rcu(dev);
	if (l2nat)
		l2nat_dev_hold(l2nat);
	rcu_read_unlock();

	return l2nat;
}

/* remove a netdevice from the l2nat list */
int l2nat_dev_del(struct net_device *dev)
{
	struct l2nat_dev *l2nat = l2nat_dev_get(dev);

	if (!l2nat)
		return -ENODEV;

	l2n_info("deleting l2nat dev for %s\n", dev->name);

	spin_lock_bh(&list_lock);
	list_del_rcu(&l2nat->list);
	l2nat_dev_disable_hash(l2nat);
	spin_unlock_bh(&list_lock);

	l2nat_hash_flush_sync(l2nat);

	/* remove the reference that we just took */
	l2nat_dev_put(l2nat);

	call_rcu(&l2nat->rcu, l2nat_dev_free_rcu);

	return 0;
}

/* add a netdevice to the l2nat list */
int l2nat_dev_add(struct net_device *dev)
{

	int ret = 0;
	struct l2nat_dev *l2nat;

	if (!dev)
		return -ENODEV;

	l2n_dbg_dev("trying to add l2nat dev for %s\n", dev->name);

	if (!(dev->priv_flags & IFF_BRIDGE_PORT)) {
		l2n_warn("device %s is not bridged, aborting\n", dev->name);
		return -EINVAL;
	}

	l2nat = l2nat_dev_get(dev);
	if (l2nat) {
		l2n_warn("l2nat dev %s already added\n", dev->name);
		l2nat_dev_put(l2nat);
		return -EEXIST;
	}

	l2nat = kzalloc(sizeof(struct l2nat_dev), GFP_KERNEL);
	if (!l2nat) {
		l2n_err("no memory for adding l2nat dev %s\n", dev->name);
		ret = -ENOMEM;
		goto alloc_fail;
	}

	dev_hold(dev);
	l2nat->dev = dev;

	ret = l2nat_dev_init(l2nat);
	if (ret) {
		l2n_err("l2nat_dev_init failed for dev %s\n", dev->name);
		goto init_fail;
	}

	atomic_inc(&dev_count);

	spin_lock_bh(&list_lock);
	list_add_rcu(&l2nat->list, &dev_list);
	spin_unlock_bh(&list_lock);

	l2n_info("added l2nat dev for %s\n", dev->name);

	return ret;

init_fail:
	dev_put(l2nat->dev);
	kfree(l2nat);
alloc_fail:
	return ret;
}

/* initialize l2net netdev list */
void l2nat_dev_list_init(void)
{
	INIT_LIST_HEAD(&dev_list);
	spin_lock_init(&list_lock);
	init_waitqueue_head(&wq);

	l2n_dbg_dev("initialized l2nat dev list\n");
}

/* uninitialize l2net netdev list */
void l2nat_dev_list_exit(void)
{
	l2nat_dev_list_flush();

	l2n_dbg_dev("exiting l2nat dev list - waiting for mem to be freed\n");
	/* wait for all memory to be freed */
	wait_event_interruptible(wq, atomic_read(&dev_count) == 0);

	l2n_dbg_dev("exited l2nat dev list\n");
}
