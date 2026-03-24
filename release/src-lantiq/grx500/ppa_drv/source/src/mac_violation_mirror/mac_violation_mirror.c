/******************************************************************************
 **
 ** FILE NAME    : mac_violation_mirror.c
 ** PROJECT      : PPA
 ** MODULES      : mac_violation_mirror (Source MAC violation handling in PAE)
 **
 ** DATE         : 25 JUNE 2018
 ** AUTHOR       : Archisman Maitra
 ** DESCRIPTION  : PPA Source MAC violation handline Functions
 ** COPYRIGHT    : Copyright (c) 2018 Intel Corporation
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date        $Author         	$Comment
 ** 25 JUNE 2018 Archisman MAitra       Initiate Version
 *******************************************************************************
 */

/* ####################################
 *              Head File
 * ####################################
 */
/*
 *  Common Head File
 */
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>

/* PPA specific Header File */
#include <net/ppa/ppa_api.h>
#include "../platform/xrx500/ltq_pae_hal.h"
#include "../ppa_api/ppa_api_session.h"
#include "../ppa_api/ppa_api_sess_helper.h"
#include <net/lantiq_cbm_api.h>
#include <net/datapath_api.h>

/*MACRO definitions*/
#define PMAC_HEADER_LEN 8
#define MAC_VIOLATION_PORT_MIRROR_DEV_NAME "mac_violation_mirror_port"
#define PORT_SET	1
#define PORT_UNSET	0
#define STRBUF_LEN	40

/*Extern Variable Devlarations */
extern int g_ppa_proc_dir_flag;
extern struct proc_dir_entry *g_ppa_proc_dir;
extern int g_mac_threshold_time;

/*Static Function declarations */
static int32_t ppa_set_up_mac_violation_port_mirror(void);
static int32_t ppa_delete_mac_violation_port_mirror(void);
static void mac_violation_mirror_proc_file_remove(void);
static void mac_violation_mirror_proc_file_create(void);
static int proc_read_threshold_time_seq_open(struct inode *inode, struct file *file);
static ssize_t proc_set_threshold_time(struct file *file, const char __user *buf, size_t count, loff_t *data);
static int proc_read_threshold_time(struct seq_file *seq, void *v);
static int proc_read_ports_seq_open(struct inode *inode, struct file *file);
static ssize_t proc_set_ports(struct file *file, const char __user *buf, size_t count, loff_t *data);
static int proc_read_ports(struct seq_file *seq, void *v);

/*Static variables */
static int32_t g_violation_exception_ports[MAX_PAE_PORTS] = {0};

/*Struct definitions */
struct module *ppa_mac_violation_mirror_module;
struct mac_violation_mirror_priv {
	struct module *owner;
	int32_t dp_port;
	dp_cb_t cb;
	dp_subif_t subif_id;
	struct net_device *dev; 
};

/*Definitions */

static int32_t mirror_packet_handling_rx_fn(struct net_device *rxif, struct net_device *txif, struct sk_buff *skb, int32_t len) 
{
	uint8_t src_mac[PPA_ETH_ALEN];
	struct dma_rx_desc_1 *desc_1 = (struct dma_rx_desc_1 *)&skb->DW1;
	short is_drop = 1;

	len -= PMAC_HEADER_LEN;		
	skb_pull(skb, PMAC_HEADER_LEN);

	if (desc_1->field.nat)
		ppa_get_src_mac_pitem_pae(skb, src_mac);
	else
		ppa_get_src_mac_addr(skb, src_mac, 0);

	ppa_buf_free(skb);
	ppa_del_hw_bridging_session_by_mac(src_mac, &is_drop);

	if (is_drop)
		ppa_session_delete_by_macaddr(src_mac);

	return PPA_SUCCESS;
}

static int32_t mirror_packet_stop_fn(struct net_device *dev) 
{
	return PPA_SUCCESS;
}

static int32_t mirror_packet_restart_fn(struct net_device *dev)
{
	return PPA_SUCCESS;
}

static int32_t ppa_set_up_mac_violation_port_mirror(void)
{
	uint32_t dp_port_id = 0;
	struct mac_violation_mirror_priv *priv = NULL;
	struct net_device *port_mirror_dev = NULL;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);

	if (!priv) {
		pr_err("Failed to alloc priv buffer\n");
		return PPA_FAILURE;
	}

	port_mirror_dev = kzalloc(sizeof(*port_mirror_dev), GFP_KERNEL);

	if (!port_mirror_dev) {
		pr_err("Failed to alloc Dummy Net device\n");
		kfree(priv);
		return PPA_FAILURE;
	}

	priv->owner = THIS_MODULE;
	memset(&(priv->cb), 0 , sizeof(priv->cb));

	snprintf(priv->owner->name, sizeof(priv->owner->name), "mac_violation_mirror");
	snprintf(port_mirror_dev->name, sizeof(port_mirror_dev->name), "%s",  MAC_VIOLATION_PORT_MIRROR_DEV_NAME);

	priv->dev = port_mirror_dev;

	dp_port_id = dp_alloc_port(priv->owner, priv->dev, 0, 0, NULL, DP_F_DIRECT);

	if (dp_port_id == DP_FAILURE) {
		pr_err("%s: dp_alloc_port failed \n", __func__);
		goto FINISH;
	}

	/* Datapath Library callback registration */
	priv->cb.rx_fn = mirror_packet_handling_rx_fn;
	priv->cb.stop_fn = mirror_packet_stop_fn;
	priv->cb.restart_fn = mirror_packet_restart_fn;
	priv->dp_port = dp_port_id;

	if (dp_register_dev(priv->owner, priv->dp_port, &(priv->cb), 0) != DP_SUCCESS) {
		pr_err("%s: dp_register_dev failed \n", __func__);
		goto FINISH;
	}

	priv->subif_id.port_id = dp_port_id;
	priv->subif_id.subif = -1;

	if (dp_register_subif(priv->owner, priv->dev, priv->dev->name, &(priv->subif_id), DP_F_DIRECT) != DP_SUCCESS) {
		pr_err("dp_register_subif fail for %s\n", MAC_VIOLATION_PORT_MIRROR_DEV_NAME);
		goto FINISH;
	}

	ltq_pae_enable_monitor_port(priv->dp_port);

	pr_info("PPA Port Mirror for Wlan with Port Mirror %d is configured\n", priv->dp_port);

	ppa_mac_violation_mirror_module = (struct module *)priv;
	return PPA_SUCCESS;

FINISH:
	kfree(priv->dev);
	kfree(priv);
	return PPA_FAILURE;
}

static int32_t ppa_delete_mac_violation_port_mirror(void)
{
	int ibase = REG_PCE_CTRL3_BASE, i = 0;

	struct mac_violation_mirror_priv *priv = (struct mac_violation_mirror_priv *)ppa_mac_violation_mirror_module;
	pr_info("Removing Mac violation Mirror Port number :- %d STARTED\n", priv->dp_port);

	ltq_pae_disable_monitor_port(priv->dp_port);

	for (i = 1; i < MAX_PAE_PORTS; i++, ibase += 10) {
		ltq_pae_reset_mac_violation_ports(ibase);
		g_violation_exception_ports[i] = 0;
	}

	if (dp_register_subif(priv->owner, priv->dev, priv->dev->name, &(priv->subif_id), DP_F_DEREGISTER) != DP_SUCCESS) {
		pr_err("dp_register_subif fail for %s\n", MAC_VIOLATION_PORT_MIRROR_DEV_NAME);
		goto FINISH;
	}

	if (dp_register_dev(priv->owner, priv->dp_port, &(priv->cb), DP_F_DEREGISTER) != DP_SUCCESS) {
		pr_err("%s: dp_register_dev failed \n", __func__);
		goto FINISH;
	}

	if (dp_alloc_port(priv->owner, priv->dev, 0, priv->dp_port, NULL, DP_F_DEREGISTER) != DP_SUCCESS) {
		pr_err("dp_unregister_dev failed for port_id %d", priv->dp_port);
		goto FINISH;
	}
	pr_info("Removing Mac violation Mirror Port number :- %d COMPLETED SUCCESSFULLY\n", priv->dp_port);

	ppa_mac_violation_mirror_module = NULL;
	kfree(priv->dev);
	kfree(priv);

	return PPA_SUCCESS;

FINISH:
	pr_info("Removing Mac violation Mirror Port number :- %d UNSUCCESSFUL\n", priv->dp_port);
	ppa_mac_violation_mirror_module = NULL;
	kfree(priv->dev);
	kfree(priv);
	return PPA_FAILURE;
}


static struct file_operations g_proc_file_mac_threshold_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_threshold_time_seq_open,
	.read		= seq_read,
	.write		= proc_set_threshold_time,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static struct file_operations g_proc_file_mac_ports_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_ports_seq_open,
	.read		= seq_read,
	.write		= proc_set_ports,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int proc_read_ports_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_ports, NULL);
}

static int proc_read_ports(struct seq_file *seq, void *v)
{
	int i = 0;

        if (!capable(CAP_NET_ADMIN)) {
             printk ("Read Permission denied");
             return 0;
        }

	seq_printf(seq,	"=====================================================================\n");
	seq_printf(seq,	"The Violation Exception Ports are:-              : \n");
	for (i = 1; i < MAX_PAE_PORTS; i++) {
		if (g_violation_exception_ports[i])
			seq_printf(seq, "%d\n", i);
	}
	seq_printf(seq,	"=====================================================================\n");

	return PPA_SUCCESS;
}

static ssize_t proc_set_ports(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len = count, port = 0 , i = 0;
	char *str = NULL;
	char *p = NULL;

        if (!capable(CAP_NET_ADMIN)) {
             printk ("Write Permission denied");
             return 0;
        }
	str = kmalloc(count, GFP_KERNEL);

	if (!str)
		return PPA_FAILURE;

	if (ppa_copy_from_user(str, buf, len)) {
		kfree(str);
		return PPA_FAILURE;
	}
	
	for (i = 1; i < MAX_PAE_PORTS; i++) {
		ltq_pae_reset_mac_violation_ports(i);
		g_violation_exception_ports[i] = PORT_UNSET;
	}

	str[len] = '\0';
	
	while ((p = strsep(&str, " ")) != NULL) {
		if ((!kstrtol(p, 10, (long *)&port)) && (port < MAX_PAE_PORTS) && (port > 0)) {
			pr_info("Port is %d\n", port);
			ltq_pae_set_mac_violation_ports(port);
			g_violation_exception_ports[port] = PORT_SET;
		}
		else {
			goto FINISH;
		}
	}

	kfree(str);
	return count;

FINISH:
	pr_err("echo \"port_id_1 port_id_2 port_id_n\" > /proc/ppa/violation_hit_ports\n");
	kfree(str);
	return PPA_FAILURE;

}
static int proc_read_threshold_time_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_threshold_time, NULL);
}

static int proc_read_threshold_time(struct seq_file *seq, void *v)
{
        if (!capable(CAP_NET_ADMIN)) {
             printk ("Read Permission denied");
             return 0;
        }
	seq_printf(seq,	"=====================================================================\n");
	seq_printf(seq,	"Mac Violation Threshold Time (ms)              : %u\n", g_mac_threshold_time);
	seq_printf(seq,	"=====================================================================\n");
	return PPA_SUCCESS;
}

static ssize_t proc_set_threshold_time(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len = 0, tmp = 0;
	char str[STRBUF_LEN] = {0};
	char *p = NULL;

        if (!capable(CAP_NET_ADMIN)) {
             printk ("Write Permission denied");
             return 0;
        }

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
	while (len && str[len - 1] <= ' ')
		len--;

	str[len] = 0;

	for (p = str; *p && *p <= ' '; p++, len--)
	;

	if (!*p)
		return count;

	kstrtol(p, 10, (long *)&tmp);

	if (tmp) {
		pr_info("Value of MAC_THRESHOLD_TIME in ms is %d\n", tmp);
		g_mac_threshold_time = tmp;
	} else {
		pr_debug("Usage: echo MAC_THRESHOLD TIME(in ms) > /proc/ppa/mac_violation/violation_threshold_time\n");
	}
	return len;
}

static void mac_violation_mirror_proc_file_create(void)
{
	struct proc_dir_entry *res = NULL;

	res = proc_create("violation_hit_ports", 0600,
			g_ppa_proc_dir,
			&g_proc_file_mac_ports_seq_fops);

	res = proc_create("violation_threshold_time", 0600,
			g_ppa_proc_dir,
			&g_proc_file_mac_threshold_seq_fops);

}

static void mac_violation_mirror_proc_file_remove(void)
{
	remove_proc_entry("violation_hit_ports", g_ppa_proc_dir);
	remove_proc_entry("violation_threshold_time", g_ppa_proc_dir);
}

static int __init mac_violation_port_mirror_init(void)
{
	ppa_set_up_mac_violation_port_mirror();
	mac_violation_mirror_proc_file_create();
	pr_info("MODULE***************Mac Violation Port Mirror Init**********************\n");
	return PPA_SUCCESS;
}


static void __exit mac_violation_port_mirror_exit(void)
{
	mac_violation_mirror_proc_file_remove();
	ppa_delete_mac_violation_port_mirror();
	pr_info("MODULE***************Mac Violation Port Mirror Exit**********************\n");
}

module_init(mac_violation_port_mirror_init);
module_exit(mac_violation_port_mirror_exit);

MODULE_AUTHOR("Archisman Maitra");
MODULE_DESCRIPTION("Mac Violation mirroring implementation");
MODULE_LICENSE("GPL");
