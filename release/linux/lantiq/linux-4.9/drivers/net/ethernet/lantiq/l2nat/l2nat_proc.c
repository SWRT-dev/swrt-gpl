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
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <net/net_namespace.h>
#include "l2nat.h"

#define L2NAT_DIR_NAME "l2nat"

static struct proc_dir_entry *l2nat_dir;

unsigned int dbg_flags = (L2N_ERR | L2N_WARN | L2N_DBG_INFO);

unsigned long dbg_val[] = {
	L2N_DBG_HASH,
	L2N_DBG_TX,
	L2N_DBG_RX,
	L2N_DBG_TMR,
	L2N_DBG_DHCP,
	L2N_DBG_DEV,
	L2N_DBG_DBG,
	L2N_DBG_INFO,
	L2N_WARN,
	L2N_ERR,
};

char *dbg_str[] = {
	"hash",		/* L2N_DBG_HASH */
	"tx",		/* L2N_DBG_TX */
	"rx",		/* L2N_DBG_RX */
	"timer",	/* L2N_DBG_TMR */
	"dhcp",		/* L2N_DBG_DHCP */
	"devices",	/* L2N_DBG_DEV */
	"genral_dbg",	/* L2N_DBG_DBG */
	"info",		/* L2N_DBG_INFO */
	"warning",	/* L2N_WARN */
	"error",	/* L2N_ERR */
};

/************************************************************************/
/* global_stats:							*/
/* read: show l2nat module stats					*/
/* write 0: clear l2nat module stats					*/
/************************************************************************/
static ssize_t gstats_seq_write(struct file *fp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	unsigned char tmp[4];
	int op;

	if (count > 4)
		return -EINVAL;

	if (copy_from_user(tmp, buf, count))
		return -EFAULT;

	sscanf(tmp, "%d", &op);

	if (op)
		goto done;

	memset(&glbl_stats, 0, sizeof(struct l2nat_global_stats));

done:
	return count;
}

static int gstats_seq_read(struct seq_file *m, void *v)
{

	seq_printf(m, "hooks_registered  %12d\n", hooks_registered);

	seq_printf(m, "\n");

	seq_printf(m, "preroute         [%12llu]\n", glbl_stats.preroute);
	seq_printf(m, "postroute        [%12llu]\n", glbl_stats.postroute);

	return 0;
}

static int gstats_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, gstats_seq_read, NULL);
}

static const struct file_operations gstats_fops = {
	.open		= gstats_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= gstats_seq_write
};

/************************************************************************/
/* dev:									*/
/* read: show all l2nat-enabled devices					*/
/* write:								*/
/*	add <dev name>: enable l2nat on specified device		*/
/*	del <dev name>: disable l2nat on specified device		*/
/************************************************************************/
static ssize_t devs_seq_write(struct file *fp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	unsigned char tmp[64];
	unsigned char cmd[64];
	unsigned char devname[64];
	struct net *net;
	struct net_device *dev;
	enum {DEV_ADD, DEV_DEL} op;

	if (count > 64)
		return -EINVAL;

	if (copy_from_user(tmp, buf, count))
		return -EFAULT;

	sscanf(tmp, "%s %s", cmd, devname);

	if (!strncmp(cmd, "add", strlen("add")))
		op = DEV_ADD;
	else if (!strncmp(cmd, "del", strlen("del")))
		op = DEV_DEL;
	else
		goto done;

	for_each_net(net) {
		dev = dev_get_by_name(net, devname);
		if (dev)
			break;
	}

	if (dev && op == DEV_ADD)
		l2nat_add_device(dev);
	else if (dev && op == DEV_DEL)
		l2nat_del_device(dev);
	else if (!dev)
		return -ENODEV;

	dev_put(dev);
done:
	return count;
}

static void l2nat_dev_show(struct l2nat_dev *l2nat, void *arg)
{
	struct seq_file *m = (struct seq_file *)arg;

	seq_printf(m,
		"%-16s %-23pM ref:%-6d aging_timeout:%-6u entries:%-10d\n",
		l2nat->dev->name,
		l2nat->dev->dev_addr,
		atomic_read(&l2nat->kref.refcount),
		l2nat->aging_timeout / HZ,
		atomic_read(&l2nat->ent_count));
}

static int devs_seq_read(struct seq_file *m, void *v)
{
	l2nat_dev_foreach(&l2nat_dev_show, (void *)m);
	return 0;
}

static int devs_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, devs_seq_read, NULL);
}

static const struct file_operations devs_fops = {
	.open		= devs_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= devs_seq_write
};

/************************************************************************/
/* dbg:									*/
/* read: show current enabled debug print flags in module		*/
/* write <dbg flag>: toggle specified debug print flag			*/
/************************************************************************/
static ssize_t dbg_seq_write(struct file *fp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	int i;
	unsigned char tmp[64];
	unsigned char dbg_op[64];

	if (count > 64)
		return -EINVAL;

	if (copy_from_user(tmp, buf, count))
		return -EFAULT;

	sscanf(tmp, "%s", dbg_op);

	for (i = 0; i < ARRAY_SIZE(dbg_str); i++) {
		if (!strcmp(dbg_op, dbg_str[i])) {
			dbg_flags ^= dbg_val[i];
			break;
		}
	}

	if (i == ARRAY_SIZE(dbg_str))
		return -EINVAL;

	return count;
}

static int dbg_seq_read(struct seq_file *m, void *v)
{

	int i;

	for (i = 0; i < ARRAY_SIZE(dbg_val); i++) {
		if (dbg_flags & dbg_val[i])
			seq_printf(m, "%-20s on\n", dbg_str[i]);
		else
			seq_printf(m, "%-20s off\n", dbg_str[i]);
	}

	return 0;
}

static int dbg_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, dbg_seq_read, NULL);
}

static const struct file_operations dbg_fops = {
	.open		= dbg_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= dbg_seq_write
};

/************************************************************************/
/* stats:								*/
/* read: show per device statistics					*/
/* write 0: clear per device statistics					*/
/************************************************************************/
static void l2nat_dev_stats_clear(struct l2nat_dev *l2nat, void *arg)
{
	memset(&l2nat->stats, 0, sizeof(l2nat->stats));
}

static ssize_t stats_seq_write(struct file *fp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	unsigned char tmp[4];
	int cmd;

	if (count > 4)
		return -EINVAL;

	if (copy_from_user(tmp, buf, count))
		return -EFAULT;

	sscanf(tmp, "%d", &cmd);

	if (cmd)
		goto done;

	l2nat_dev_foreach(&l2nat_dev_stats_clear, NULL);

done:
	return count;
}

static void l2nat_dev_stats_show(struct l2nat_dev *l2nat, void *arg)
{
	struct seq_file *m = (struct seq_file *)arg;
	struct l2nat_dev_stats *stats = &l2nat->stats;

	seq_printf(m, "%s:\n", l2nat->dev->name);
	seq_printf(m, "rx_packets       [%12llu]\n", stats->rx_packets);
	seq_printf(m, "rx_bytes         [%12llu]\n", stats->rx_bytes);
	seq_printf(m, "rx_dropped       [%12llu]\n", stats->rx_dropped);
	seq_printf(m, "rx_arp           [%12llu]\n", stats->rx_arp);

	seq_printf(m, "\n");

	seq_printf(m, "tx_packets       [%12llu]\n", stats->tx_packets);
	seq_printf(m, "tx_bytes         [%12llu]\n", stats->tx_bytes);
	seq_printf(m, "tx_dropped       [%12llu]\n", stats->tx_dropped);
	seq_printf(m, "tx_arp           [%12llu]\n", stats->tx_arp);

	seq_printf(m, "\n");

	seq_printf(m, "arp_gen          [%12llu]\n", stats->arp_gen);
	seq_printf(m, "dhcp             [%12llu]\n", stats->dhcp);
	seq_printf(m, "entries_add      [%12llu]\n", stats->entries_add);
	seq_printf(m, "entries_del      [%12llu]\n", stats->entries_del);
	seq_printf(m, "\n");
	seq_printf(m, "\n");
}

static int stats_seq_read(struct seq_file *m, void *v)
{
	l2nat_dev_foreach(&l2nat_dev_stats_show, (void *)m);

	return 0;
}

static int stats_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, stats_seq_read, NULL);
}

static const struct file_operations stats_fops = {
	.open		= stats_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= stats_seq_write
};

/************************************************************************/
/* aging_timeout:							*/
/* read: show ip-MAC entry aging timeout				*/
/* write: clear ip-MAC entry aging timeout				*/
/************************************************************************/
static void l2nat_set_aging_to(struct l2nat_dev *l2nat, void *arg)
{
	l2nat->aging_timeout = *(u32 *)arg;
}

static ssize_t aging_to_seq_write(struct file *fp, const char __user *buf,
					size_t count, loff_t *ppos)
{
	unsigned char tmp[16];
	u32 new_aging_timeout = 0;
	if (count > 16)
		return -EINVAL;

	if (copy_from_user(tmp, buf, count))
		return -EFAULT;

	sscanf(tmp, "%u", &new_aging_timeout);

	aging_timeout = new_aging_timeout * HZ;

	l2nat_dev_foreach(&l2nat_set_aging_to, (void *)&aging_timeout);

	return count;
}

static int aging_to_seq_read(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", aging_timeout / HZ);
	return 0;
}

static int aging_to_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, aging_to_seq_read, NULL);
}

static const struct file_operations aging_to_fops = {
	.open		= aging_to_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= aging_to_seq_write
};

/************************************************************************/
/* hash:								*/
/* read: show ip-MAC hashtable for all devices				*/
/* write 0: clear ip-MAC hashtable for all devices			*/
/************************************************************************/
struct hash_seq_priv {
	struct list_head ent_list;
	int counter;
};

static void l2nat_dev_hash_clear(struct l2nat_dev *l2nat, void *arg)
{
	l2nat_hash_flush(l2nat);
}

static ssize_t hash_seq_write(struct file *fp, const char __user *buf,
				size_t count, loff_t *ppos)
{
	unsigned char tmp[4];
	int cmd;

	if (count > 4)
		return -EINVAL;

	if (copy_from_user(tmp, buf, count))
		return -EFAULT;

	sscanf(tmp, "%d", &cmd);

	if (cmd)
		goto done;

	l2nat_dev_foreach(&l2nat_dev_hash_clear, NULL);

done:
	return count;
}

static void *hash_seq_start(struct seq_file *m, loff_t *pos)
{
	struct hash_seq_priv *priv = (struct hash_seq_priv *)m->private;
	return seq_list_start(&priv->ent_list, *pos);
}

static void *hash_seq_next(struct seq_file *m, void *p, loff_t *pos)
{
	struct hash_seq_priv *priv = (struct hash_seq_priv *)m->private;
	return seq_list_next(p, &priv->ent_list, pos);
}

static void hash_seq_stop(struct seq_file *m, void *p)
{
}

static int hash_seq_show(struct seq_file *m, void *p)
{
	struct hash_seq_priv *priv = (struct hash_seq_priv *)m->private;
	struct hash_entry *ent = list_entry(p, struct hash_entry, seqlist);

	seq_printf(m,
		"%-3d] %-7s %-16pI4 %pM   ref %d idle %-3d tx %llu rx %llu\n",
		priv->counter++,
		ent->l2nat->dev->name,
		&ent->ip,
		&ent->mac,
		/* remove 1 ref since the proc iteration itself takes a ref*/
		atomic_read(&ent->kref.refcount) - 1,
		jiffies_to_msecs(last_packet_delta(ent)) / 1000,
		ent->tx_packets,
		ent->rx_packets);

	return 0;
}

static const struct seq_operations hash_seq_ops = {
	.start = hash_seq_start,
	.next = hash_seq_next,
	.stop = hash_seq_stop,
	.show  = hash_seq_show,
};

static void l2nat_dev_release_entries(struct l2nat_dev *l2nat, void *arg)
{
	struct hash_entry *ent;
	struct list_head *itr, *tmp;
	struct list_head *ent_list = (struct list_head *)arg;

	list_for_each_safe(itr, tmp, ent_list) {
		ent = list_entry(itr, struct hash_entry, seqlist);
		list_del(&ent->seqlist);
		l2nat_entry_put(ent);
	}
}

int hash_seq_release(struct inode *inode, struct file *file)
{
	struct seq_file *seq = file->private_data;
	struct hash_seq_priv *priv = (struct hash_seq_priv *)seq->private;

	l2nat_dev_foreach(&l2nat_dev_release_entries, (void *)&priv->ent_list);

	return seq_release_private(inode, file);
}

static void l2nat_dev_collect_entries(struct l2nat_dev *l2nat, void *arg)
{
	int i;
	struct hash_entry *ent;
	struct list_head *ent_list = (struct list_head *)arg;

	rcu_read_lock();
	hash_for_each_rcu(l2nat->hash, i, ent, hlist) {
		l2nat_entry_hold(ent);
		INIT_LIST_HEAD(&ent->seqlist);
		list_add_tail(&ent->seqlist, ent_list);
	}
	rcu_read_unlock();
}

static int hash_seq_open(struct inode *inode, struct file *file)
{
	struct hash_seq_priv *priv;

	priv = __seq_open_private(file, &hash_seq_ops, sizeof(*priv));
	if (!priv)
		return -ENOMEM;

	priv->counter = 0;
	INIT_LIST_HEAD(&priv->ent_list);

	l2nat_dev_foreach(&l2nat_dev_collect_entries, (void *)&priv->ent_list);

	return 0;
}

static const struct file_operations hash_fops = {
	.open		= hash_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= hash_seq_release,
	.write		= hash_seq_write
};

void l2nat_proc_init(void)
{
	l2nat_dir = proc_mkdir(L2NAT_DIR_NAME, NULL);
	if (!l2nat_dir) {
		l2n_err("proc_mkdir failed\n");
		return;
	}

	if (!proc_create("global_stats" , 0, l2nat_dir, &gstats_fops)) {
		l2n_err("proc_create failed\n");
		return;
	}

	if (!proc_create("dev" , 0, l2nat_dir, &devs_fops)) {
		l2n_err("proc_create failed\n");
		return;
	}

	if (!proc_create("dbg" , 0, l2nat_dir, &dbg_fops)) {
		l2n_err("proc_create failed\n");
		return;
	}

	if (!proc_create("stats" , 0, l2nat_dir, &stats_fops)) {
		l2n_err("proc_create failed\n");
		return;
	}

	if (!proc_create("aging_timeout" , 0, l2nat_dir, &aging_to_fops)) {
		l2n_err("proc_create failed\n");
		return;
	}

	if (!proc_create("hash" , 0, l2nat_dir, &hash_fops)) {
		l2n_err("proc_create failed\n");
		return;
	}
}

void l2nat_proc_exit(void)
{
	proc_remove(l2nat_dir);
}
