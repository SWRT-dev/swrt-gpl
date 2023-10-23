/******************************************************************************
 **
 ** FILE NAME    : litepath_hwacc.c
 ** PROJECT      : PPA
 ** MODULES      : Litepath HW Acceleration
 **
 ** DATE         : 21st August 2019 
 ** AUTHOR       : Moinak Debnath
 ** DESCRIPTION  : Litepath HW Acceleration driver implementation
 ** COPYRIGHT    : Copyright (c) 2019 Intel Corporation
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date        	$Author         $Comment
 ** 21st August 2019	Moinak Debnath	Initiate Version
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
#include <net/datapath_api_gswip30.h>

#include <net/ppa/ppa_hook.h>
#include <net/ppa/ppa_api_directpath.h>
#include "../ppa_api/ppa_sae_hal.h"
#include "../platform/xrx500/ltq_tmu_hal_api.h"

/* proc counters and func declarations*/

uint64_t lp_tx, lp_tx_b; /* Litepath tx counter */
uint64_t lp_rx, lp_rx_b; /* Litepath rx counter*/
uint64_t lblp_rx, lblp_rx_b; /* Litepath loopback counter */

extern struct proc_dir_entry *g_ppa_proc_dir;
static void hw_litepath_proc_file_remove(void);
static void hw_litepath_proc_file_create(void);
static int proc_read_hwlitepath_pkt_ctrs_seq_open(struct inode *inode, struct file *file);
static ssize_t proc_reset_hwlitepath_pkt_ctrs(struct file *file, const char __user *buf, size_t count, loff_t *data);
static int proc_read_hwlitepath_pkt_ctrs(struct seq_file *seq, void *v);
uint32_t ctr = 0;
#define use_Directpath 0

#if use_Directpath
PPA_DIRECTPATH_CB litepath_cb;
#endif

#define DBG_DUMP_SKB 0
static inline void dps_dump_skb(uint8_t *ptr, int len) 
{
#if DBG_DUMP_SKB
	char *tmp_buf = NULL;
	int i, offset;
	
	tmp_buf = kmalloc(512, GFP_KERNEL);

	if (!tmp_buf)
		return;
	
	for (i = 0, offset = 0; i < len; ++i) {
		if ((i % 16) == 0)
			offset += sprintf(tmp_buf + offset, "\n");

		offset += sprintf(tmp_buf + offset, "%02x ", *(ptr+i));
	}

	if (offset) {
		offset += sprintf(tmp_buf + offset, "\n");
		printk(KERN_INFO "%s", tmp_buf);
	}
	kfree(tmp_buf);
#endif
}
struct module *module_litepath = NULL;
struct hw_litepath_priv {
	struct module		*owner;
	int32_t			id;
	dp_subif_t		subif_id; //chnge this name
	struct net_device	*dev; 
#if !use_Directpath
	int32_t			dp_port;
	dp_cb_t 		cb;
#endif
};

/*Definitions */
static int32_t hw_litepath_rx_fn(struct net_device *rxif, struct net_device *txif, struct sk_buff *skb, int32_t len)
{
	int32_t ret = -1;

	if(!skb) {
		printk("skb is null\n");
		return -1;
	}

#if !use_Directpath
	len -= sizeof(struct pmac_rx_hdr);   /*Remove PMAC to DMA header */
	skb_pull(skb, sizeof(struct pmac_rx_hdr));
#endif

	if (rxif) { /* rxif is lite0, will come here if directpath_send fails! */

		skb->dev = rxif;
		lblp_rx++;
		lblp_rx_b += len;
		dev_kfree_skb(skb);

	} else if (txif) {

		skb->dev = txif;
		skb->protocol = eth_type_trans(skb, txif);
		ret = ppa_sw_fastpath_send_hook(skb);
		lp_rx++;
		lp_rx_b += len;

		return ret;

	} else {

		printk("rxif and txif are NULL, free skb\n");
		kfree(skb);

		return PPA_FAILURE;
	}	

	return PPA_SUCCESS;
}

static int32_t hw_litepath_stop_fn(struct net_device *dev)
{
	return PPA_SUCCESS;
}

static int32_t hw_litepath_restart_fn(struct net_device *dev)
{
	return PPA_SUCCESS;
}

/* Init of the network device */
/* Start the network device interface queue */
static int hw_litepath_eth_open(struct net_device *dev);
/* Stop the network device interface queue */
static int hw_litepath_eth_stop(struct net_device *dev);
/* Uninit the network device interface queue */
static void hw_litepath_eth_uninit(struct net_device *dev);
/* Transmit packet from Tx Queue to MAC */
static int hw_litepath_start_xmit(struct sk_buff *skb, struct net_device *dev);
/*  Set mac address*/
static int hw_litepath_set_mac_address(struct net_device *dev, void *p);
/* Get the network device statistics */

extern int32_t (*hw_litepath_xmit_fn)(PPA_SKBUF *);
struct net_device *hw_lite0_netdev, *hw_lite0_netdev_phy;
/* init and exit func */
static int32_t ppa_set_up_hw_litepath_netif(void);
static int32_t ppa_delete_hw_litepath_netif(void);



static void hw_litepath_setup(struct net_device *dev)
{
	ether_setup(dev);

	return;
}
static const struct net_device_ops lite0_netif_drv_ops = {
	.ndo_open		= hw_litepath_eth_open,
	.ndo_stop		= hw_litepath_eth_stop,
	.ndo_uninit		= hw_litepath_eth_uninit,
	.ndo_start_xmit		= hw_litepath_start_xmit,
	.ndo_set_mac_address	= hw_litepath_set_mac_address,
};

int32_t hw_litepath_xmit(PPA_SKBUF *skb)
{
	struct ethhdr *mac = eth_hdr(skb);
	struct hw_litepath_priv *priv = (struct hw_litepath_priv *)module_litepath;
#if use_Directpath
	PPA_SUBIF lite_subif;
	lite_subif.port_id = priv->subif_id.port_id;
	lite_subif.subif = 0;
#else
	struct dma_tx_desc_0 *desc_0 = (struct dma_tx_desc_0 *)&skb->DW0;
	struct dma_tx_desc_1 *desc_1 = (struct dma_tx_desc_1 *)&skb->DW1;
	uint32_t dp_flags = 0;
#endif
	/* lite0's MAC*/
	memcpy(mac->h_source, priv->dev->dev_addr, ETH_ALEN);
	/* some dummy MAC*/
	mac->h_dest[0] = 0x02;
	mac->h_dest[1] = 0x9A;
	mac->h_dest[2] = 0x96;
	mac->h_dest[3] = 0xF3;
	mac->h_dest[4] = 0x7A;
	mac->h_dest[5] = 0xB1;

	lp_tx_b += skb->len;
	lp_tx++;

#if use_Directpath
	return ppa_hook_directpath_ex_send_fn(&lite_subif, skb, skb->len, 0);
#else
	desc_1->field.ep = priv->subif_id.port_id;  
	desc_0->field.dest_sub_if_id = 0;

	if (skb->ip_summed == CHECKSUM_PARTIAL)
		dp_flags |= DP_TX_CAL_CHKSUM;

	if (dp_xmit(priv->dev, &(priv->subif_id), skb, skb->len, dp_flags) != 0) {
		pr_err("dp_xmit fails for dp port: %d\n", priv->dp_port);
		return PPA_EPERM;
	}	
	return PPA_SUCCESS;
#endif
}
static int hw_litepath_eth_open(struct net_device *dev)
{
	printk("lite0 open called!\n");
	netif_start_queue(dev);

	return 0;
}

static int hw_litepath_eth_stop(struct net_device *dev)
{
	printk("lite0 stop called!\n");
	netif_stop_queue(dev);

	return 0;
}

static void hw_litepath_eth_uninit(struct net_device *dev)
{
	return;
}

static int hw_litepath_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	printk("Lite0 xmit function called, free/dropping skb\n");
	dev_kfree_skb(skb);

	return 0;
}

static int hw_litepath_set_mac_address(struct net_device *dev, void *p)
{
	return 0;
}
static int32_t ppa_set_up_hw_litepath_netif(void)
{
#if !use_Directpath	
	uint32_t dp_port_id = 0;
#endif
	char ifname[IFNAMSIZ], ifnamephy[IFNAMSIZ];
	struct hw_litepath_priv *priv;
#if use_Directpath
	uint32_t status = -1;
	PPA_SUBIF if_id = {0};

#endif
	snprintf(ifnamephy, sizeof(ifnamephy), "litephy%d", 0);
	snprintf(ifname, sizeof(ifname), "lite%d", 0);

	hw_lite0_netdev = alloc_netdev(sizeof(struct hw_litepath_priv), ifname, NET_NAME_UNKNOWN, hw_litepath_setup);
	hw_lite0_netdev_phy = alloc_netdev(sizeof(struct hw_litepath_priv), ifnamephy, NET_NAME_UNKNOWN, hw_litepath_setup);
#define dbg printk
	if (hw_lite0_netdev_phy == NULL) {
		dbg("alloc_netdev fail\n");
		return -1;
	}
	hw_lite0_netdev_phy->netdev_ops = NULL;

	if (hw_lite0_netdev_phy == NULL) {
		dbg("alloc_netdev fail\n");
		return -1;
	}
	/* assign MAC for lite0 */ 
	hw_lite0_netdev->dev_addr[0] = 0x02;
	hw_lite0_netdev->dev_addr[1] = 0x9A;
	hw_lite0_netdev->dev_addr[2] = 0x96;
	hw_lite0_netdev->dev_addr[3] = 0xF3;
	hw_lite0_netdev->dev_addr[4] = 0x7A;
	hw_lite0_netdev->dev_addr[5] = 0xA3;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	hw_lite0_netdev->netdev_ops = &lite0_netif_drv_ops;
	if (register_netdev(hw_lite0_netdev)) {
		free_netdev(hw_lite0_netdev);
		hw_lite0_netdev = NULL;
		dbg("register device \"%s\" fail ??\n", ifname);
	} else {
		dbg("add \"%s\" successfully\n", ifname);
	}
#undef dbg	
	priv->owner = THIS_MODULE; 
	priv->id = 1;
	snprintf(priv->owner->name, sizeof(priv->owner->name), "litepath_hw%d", 0);
	priv->dev = hw_lite0_netdev;
#if use_Directpath
	litepath_cb.stop_tx_fn = hw_litepath_stop_fn;
	litepath_cb.start_tx_fn = hw_litepath_restart_fn;
	litepath_cb.rx_fn = hw_litepath_rx_fn;

	if_id.port_id = -1; // dev register

	if (ppa_hook_directpath_ex_register_dev_fn) {
		status = ppa_hook_directpath_ex_register_dev_fn(
				&if_id, hw_lite0_netdev_phy,
				&litepath_cb,
				PPA_F_DIRECTPATH_REGISTER
				| PPA_F_DIRECTPATH_ETH_IF);
	}
	if (status != PPA_SUCCESS) {
		pr_err("%s: direcpath register failed \n", __func__);
		goto FINISH;
	}

	priv->subif_id.port_id = if_id.port_id;

	if_id.subif = -1; // regiter subif

	if (ppa_hook_directpath_ex_register_dev_fn) {
		status = ppa_hook_directpath_ex_register_dev_fn(
				&if_id, hw_lite0_netdev,
				&litepath_cb,
				PPA_F_DIRECTPATH_REGISTER
				| PPA_F_DIRECTPATH_ETH_IF);
	}
	if (status != PPA_SUCCESS) {
		pr_err("%s: direcpath subif register failed \n", __func__);
		goto FINISH;
	}

	priv->subif_id.subif = if_id.subif;
	
#else
	memset(&(priv->cb), 0 , sizeof(priv->cb));
	dp_port_id = dp_alloc_port(priv->owner, priv->dev, 0, 0, NULL, DP_F_DIRECT);
	if (dp_port_id == DP_FAILURE) {
		pr_err("%s: dp_alloc_port failed \n", __func__);
		goto FINISH;
	}

	/* Datapath Library callback registration */
	priv->cb.rx_fn = hw_litepath_rx_fn;
	priv->cb.stop_fn = hw_litepath_stop_fn;
	priv->cb.restart_fn = hw_litepath_restart_fn;
	priv->dp_port = dp_port_id;

	if (dp_register_dev(priv->owner, priv->dp_port, &(priv->cb), 0) != DP_SUCCESS) {
		pr_err("%s: dp_register_dev failed \n", __func__);
		goto FINISH;
	}

	priv->subif_id.port_id = dp_port_id;
	priv->subif_id.subif = -1;

	if (dp_register_subif(priv->owner, priv->dev, priv->dev->name, &(priv->subif_id), DP_F_DIRECT) != DP_SUCCESS) {
		pr_err("dp_register_subif fail for %s\n", "Liteath_HWAcc_Port");
		goto FINISH;
	}
#endif
	module_litepath = (struct module *)priv;
	hw_litepath_xmit_fn = hw_litepath_xmit;

	rtnl_lock();
	dev_open(priv->dev);
	rtnl_unlock();
	
	return 0;
FINISH:
	unregister_netdev(priv->dev);
	free_netdev(priv->dev);
	kfree(priv);
	priv = NULL;

	return PPA_FAILURE;
}

static int32_t ppa_delete_hw_litepath_netif(void)
{
	struct hw_litepath_priv *priv = (struct hw_litepath_priv *)module_litepath;
	hw_litepath_xmit_fn = NULL;
#if use_Directpath
	uint32_t status = 0;
	PPA_SUBIF if_id = {0};
	hw_litepath_eth_stop(priv->dev);
 /* subif deregister*/
	if_id.port_id = priv->subif_id.port_id;
	if_id.subif = priv->subif_id.subif;
	
	if (ppa_hook_directpath_ex_register_dev_fn) {
		status = ppa_hook_directpath_ex_register_dev_fn(
				&if_id, priv->dev, NULL, 0); 
	}
 /* dev unregister */
	if_id.subif = -1;
	if (ppa_hook_directpath_ex_register_dev_fn) {
		status = ppa_hook_directpath_ex_register_dev_fn(
				&if_id, priv->dev, NULL, 0);  
	}

	rtnl_lock();
	dev_close(priv->dev);
	rtnl_unlock();

	module_litepath = NULL;
	unregister_netdev(priv->dev);
	free_netdev(priv->dev);
	kfree(priv);
	priv = NULL;

	return PPA_SUCCESS;
#else
	hw_litepath_eth_stop(priv->dev);
	if (dp_register_subif(priv->owner, priv->dev, priv->dev->name, &(priv->subif_id), DP_F_DEREGISTER) != DP_SUCCESS) {
		pr_err("dp_register_subif fail for %s\n", "Liteath_HWAcc_Port");
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

	module_litepath = NULL;
	unregister_netdev(priv->dev);
	free_netdev(priv->dev);
	kfree(priv);
	priv = NULL;

	return PPA_SUCCESS;
FINISH:
	module_litepath = NULL;
	unregister_netdev(priv->dev);
	free_netdev(priv->dev);
	kfree(priv);
	priv = NULL;

	return PPA_FAILURE;
#endif
}
static struct file_operations g_proc_file_hwlitepath_pkt_ctrs_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_hwlitepath_pkt_ctrs_seq_open,
	.read		= seq_read,
	.write		= proc_reset_hwlitepath_pkt_ctrs,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

static int proc_read_hwlitepath_pkt_ctrs_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_hwlitepath_pkt_ctrs, NULL);
}

static int proc_read_hwlitepath_pkt_ctrs(struct seq_file *seq, void *v)
{
	struct hw_litepath_priv *priv;

	if (!capable(CAP_NET_ADMIN)) {
             printk ("Read Permission denied\n");
             return 0;
        }

	priv = (struct hw_litepath_priv *)module_litepath;
	seq_printf(seq,	"=====================================================================\n");
	seq_printf(seq,	"\n\t\tHW LitePath Statistics, port for lite0: %u\n",priv->subif_id.port_id);
	seq_printf(seq, "\n\t\tHW Litepath pkt counters:\n\nrx_fn_tx_if: %llu - DPsend: %llu - rx_fn_rx_if: %llu\n",
			lp_rx, lp_tx, lblp_rx);
	seq_printf(seq, "\n\t\tHW Litepath bytes Counters:\n\nrx_fn_tx_if: %llu - DPsend: %llu - rx_fn_rx_if: %llu\n",
			lp_rx_b, lp_tx_b, lblp_rx_b);
	seq_printf(seq,	"=====================================================================\n");

	return PPA_SUCCESS;
}

static ssize_t proc_reset_hwlitepath_pkt_ctrs(struct file *file,
	const char __user *buf, size_t count, loff_t *data)
{
	int len = 0, tmp = 0;
	char str[23] = {0};
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

	if (tmp == 0) {
		printk("Resetting all the counters to zero\n");
		ctr = lp_tx = lp_rx = lblp_rx = lp_tx_b = lp_rx_b = lblp_rx_b = 0;
	} else {
		printk("Usage: echo 0 > /proc/ppa/pkt_ctrs_lite0 to reset all the counters\n");
	}
	return len;
}

static void hw_litepath_proc_file_create(void)
{
	struct proc_dir_entry *res = NULL;

	res = proc_create("pkt_ctrs_lite0", 0600,
			g_ppa_proc_dir,
			&g_proc_file_hwlitepath_pkt_ctrs_seq_fops);
}

static int __init litepath_netif_init(void)
{
	int f = 0;
	f = ppa_set_up_hw_litepath_netif();

	if (f == PPA_SUCCESS) {
		hw_litepath_proc_file_create();
		pr_info("MODULE HW litepath Init Success!\n");
	} else
		pr_info("MODULE HW litepath Init Failed!\n");

	return PPA_SUCCESS;
}

static void hw_litepath_proc_file_remove(void)
{
	remove_proc_entry("pkt_ctrs_lite0", g_ppa_proc_dir);
}

static void __exit litepath_netif_exit(void)
{
	int f = 0;
	hw_litepath_proc_file_remove();
	f = ppa_delete_hw_litepath_netif();
	if (f == PPA_SUCCESS)
		pr_info("MODULE HW litepath Exit Success!\n");
	else
		pr_info("MODULE HW litepath Exit Failed!\n");
}

module_init(litepath_netif_init);
module_exit(litepath_netif_exit);

MODULE_AUTHOR("Moinak Debnath");
MODULE_DESCRIPTION("lite0 netif driver for tcp litepath hw offload");
MODULE_LICENSE("GPL");
