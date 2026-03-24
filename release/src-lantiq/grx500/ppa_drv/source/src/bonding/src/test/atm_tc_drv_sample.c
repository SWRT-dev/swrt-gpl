
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/skbuff.h>
#include <net/datapath_api.h>
#include <net/lantiq_cbm_api.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include "common/atm_bond.h"

#define DEVICE_LOOPBACK	0

#define TC_SAMPLE_DBGFS_NAME "tc_sample"
#define CONFIG_BUFF_LEN 32

enum ring_type {
	TXIN = 0,
	TXOUT,
	RXOUT,
	RXIN
};

struct ring {
	enum ring_type type;
	u32 start;
	dma_addr_t start_phys;
	u32 size;
	u32 read_idx;
	u32 write_idx;
	u32 total;
	u32 delta;

	u32 *read_umt;
	u32 *write_umt;
};

struct conn_params {
	struct ring txin;	/* TXIN ring params */
	struct ring txout;	/* TXOUT ring params */
	struct ring rxout;	/* RXOUT ring params */
	struct ring rxin;	/* RXIN ring params */
};

struct cfg_params {
	u32 *soc_umt_dst;		/* SoC UMT dst vaddr */
	u32 aca_umt_dst[2];		/* Per EP device ACA UMT dst paddr */
	struct conn_params soc_conn;	/* SoC conn-params*/
	struct conn_params ep_conn[2];	/* Per EP device conn-params*/
};

struct tc_priv_data {
	struct module *owner;
	struct net_device *dev;
	struct net_device_stats stats;
	u8 qid;
	int dp_port_id;
	int dp_subif_id;
	int cbm_pid;
	bool umt_enabled;
	struct cfg_params cfg;
	struct task_struct *poll_thread;
};

struct st_conf {
	bool showtime[2]; /* idx 0 for link 0 and idx 1 for link 1 */
};

/* Linux network operations */
static int tc_open(struct net_device *dev);
static int tc_stop(struct net_device *dev);
static int tc_start_xmit(struct sk_buff *skb, struct net_device *dev);
static struct net_device_stats *tc_get_stats(struct net_device *dev);
static int tc_change_mtu(struct net_device *dev, int new_mtu);
static int tc_set_mac_address(struct net_device *dev, void *addr);
/* Platform datapath operations */
static int plat_dp_init(struct tc_priv_data *priv);
static void plat_dp_uninit(struct tc_priv_data *priv);
static int plat_dp_open(struct net_device *dev);
static void plat_dp_stop(struct net_device *dev);
static int plat_dp_xmit(struct sk_buff *skb, struct net_device *dev);
static void plat_dp_reset(s32 port_id);
/* Bonding interface operations */
static int bonding_device_init(struct net_device *dev);
static void bonding_device_uninit(struct net_device *dev);
static void bonding_showtime_enter(int ep_id);
static void bonding_showtime_exit(int ep_id);
static int bonding_rx_cb(struct atm_aal5_t *atm_rx_pkt);
static void bonding_cbm_buf_free_cb(void *phys);
static int bonding_cbm_dqptr_read_cb(u32 *p);
static void bonding_rx_asm_ind_cb(const unsigned char ep_id);
static int bonding_cfg_init(struct tc_priv_data *priv);
static void bonding_cfg_uninit(struct tc_priv_data *priv);
/* Device loopback operations */
static int loopback_poll_handler(void *data);

void cleanup_debug_interface(void);
int init_debug_interface(void);
ssize_t config_read(struct file *file, char __user *buff, size_t count, loff_t *offset);
ssize_t config_write(struct file *file, const char __user *buff, size_t count, loff_t *offset);

static const struct net_device_ops tc_netdev_ops = {
	.ndo_open = tc_open,
	.ndo_stop = tc_stop,
	.ndo_start_xmit = tc_start_xmit,
	.ndo_get_stats = tc_get_stats,
	.ndo_set_mac_address = tc_set_mac_address,
	.ndo_change_mtu = tc_change_mtu
};

static const struct file_operations config_ops = {
	.owner		= THIS_MODULE,
	.read		= config_read,
	.write		= config_write,
};

struct tc_sample_dbgfs_list {
	char *name;
	umode_t mode;
	const struct file_operations *fops;
};

static struct tc_sample_dbgfs_list tc_sample_dbgfs_entries[] = {
	{"showtime",	0644,	&config_ops},
};

static struct dentry *g_dbgfs_node;
struct st_conf g_conf = {.showtime[0] = 0, .showtime[1] = 0};

static struct tc_priv_data *g_tc_priv;

/*
 * ####################################
 * Linux network operations
 * ####################################
 */
static void tc_setup(struct net_device *dev)
{
	ether_setup(dev);
	dev->netdev_ops = &tc_netdev_ops;
	dev->dev_addr[0] = 0x00;
	dev->dev_addr[1] = 0x11;
	dev->dev_addr[2] = 0x22;
	dev->dev_addr[3] = 0x33;
	dev->dev_addr[4] = 0x44;
	dev->dev_addr[5] = 0x55;
}

static struct net_device_stats *tc_get_stats(struct net_device *dev)
{
	struct tc_priv_data *priv = netdev_priv(dev);

	return &priv->stats;
}

static int tc_open(struct net_device *dev)
{
	bonding_device_init(dev);
	plat_dp_open(dev);
	netif_start_queue(dev);
	return 0;
}

static int tc_stop(struct net_device *dev)
{
	netif_stop_queue(dev);
	plat_dp_stop(dev);
	bonding_device_uninit(dev);
	return 0;
}

static int tc_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	int ret;
	struct tc_priv_data *priv = netdev_priv(dev);

	ret = plat_dp_xmit(skb, dev);
	if (!ret) {
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += skb->len;
	} else
		priv->stats.tx_dropped++;

	return 0;
}

static int tc_set_mac_address(struct net_device *dev, void *addr)
{
	return 0;
}

static int tc_change_mtu(struct net_device *dev, int new_mtu)
{
	return 0;
}

static int tc_device_init(void)
{
	int ret;
	struct tc_priv_data *priv;
	struct net_device *dev;

	dev = alloc_netdev(sizeof(struct tc_priv_data), "testnas0",
			 NET_NAME_ENUM, tc_setup);
	if (!dev) {
		pr_err("alloc_netdev() failed!\n");
		ret = -ENODEV;
		goto err_out_alloc;
	}
	ret = register_netdev(dev);
	if (ret) {
		pr_err("register_netdev() failed!\n");
		goto err_out_register;
	}

	priv = netdev_priv(dev);
	priv->owner = THIS_MODULE;
	priv->dev = dev;
	priv->qid = 1;

	ret = plat_dp_init(priv);
	if (ret) {
		pr_err("plat_dp_init() failed!\n");
		goto err_out_dp;
	}
	ret = bonding_cfg_init(priv);
	if (ret) {
		pr_err("bonding_cfg_init() failed!\n");
		goto err_out_cfg;
	}
	g_tc_priv = priv;

	return 0;

err_out_cfg:
	plat_dp_uninit(priv);
err_out_dp:
	unregister_netdev(dev);
err_out_register:
	free_netdev(dev);
err_out_alloc:
	return ret;
}

static void tc_device_uninit(void)
{
	if (g_tc_priv) {
		if (netif_running(g_tc_priv->dev)) {
			plat_dp_stop(g_tc_priv->dev);
			bonding_device_uninit(g_tc_priv->dev);
		}
		bonding_cfg_uninit(g_tc_priv);
		plat_dp_uninit(g_tc_priv);
		unregister_netdev(g_tc_priv->dev);
		free_netdev(g_tc_priv->dev);
	}
	g_tc_priv = NULL;
}

/*
 * ####################################
 * Platform datapath operations
 * ####################################
 */
#define set_mask_bit(val, set, mask, bits)		\
	(val = (((val) & (~((mask) << (bits))))	\
	| (((set) & (mask)) << (bits))))
/* skb port configuration */
#define SKB_PORT_ID		0xF
#define SKB_PORT_ID_S		8
#define SKB_PORT_SUBID		0x7FFF
#define SKB_PORT_SUBID_S	0

static int plat_dp_init(struct tc_priv_data *priv)
{
	int ret;
	int dp_pid;
	int flags;

	flags = DP_F_FAST_WLAN | DP_F_FAST_DSL | DP_F_DSL_BONDING;
	dp_pid = dp_alloc_port(priv->owner, priv->dev, 0, 0, NULL, flags);
	if (dp_pid == DP_FAILURE) {
		pr_err("dp_alloc_port() failed!\n");
		ret = -ENOMEM;
		goto err_out_alloc;
	}
	ret = dp_register_dev(priv->owner, dp_pid, NULL, 0);
	if (ret != DP_SUCCESS) {
		pr_err("dp_register_dev() failed for port id %d", dp_pid);
		ret = -ENODEV;
		goto err_out_register;
	}
	priv->dp_port_id = dp_pid;

	return 0;

err_out_register:
	dp_alloc_port(priv->owner, NULL, 0, dp_pid, NULL, DP_F_DEREGISTER);
err_out_alloc:
	return ret;
}

static void plat_dp_uninit(struct tc_priv_data *priv)
{
	dp_register_dev(priv->owner, priv->dp_port_id, NULL, DP_F_DEREGISTER);
	dp_alloc_port(priv->owner, NULL, 0,
		priv->dp_port_id, NULL, DP_F_DEREGISTER);
}

static void plat_dp_reset(s32 port_id)
{
	s32 ret;
	u32 num_tmu_ports = 0;
	cbm_tmu_res_t *cbm_tmu_res = NULL;
	u32 tmu_port;
	s32 cbm_port_id;

	ret = cbm_dp_port_resources_get((u32 *)&port_id, &num_tmu_ports, &cbm_tmu_res, 0);
	if (ret < 0)
		return;

	if (!cbm_tmu_res)
		return;

	tmu_port = cbm_tmu_res[0].tmu_port;
	cbm_port_id = cbm_tmu_res[0].cbm_deq_port;
	kfree(cbm_tmu_res);

	/*
	 * Flush the CBM/TMU port
	 * Reset the CBM dequeue port desc index
	 */
	pr_info("[%s:%d] Flush CBM/TMU queues and reset CBM deq port - "
		"port_id=%d, tmu_port=%d, cbm_port_id=%d.\n",
		__func__, __LINE__, port_id, tmu_port, cbm_port_id);
	cbm_port_quick_reset(cbm_port_id, CBM_PORT_F_DEQUEUE_PORT);
	cbm_dp_q_enable(0, port_id, -1, tmu_port, -1, 200000, 0,
		(/*CBM_Q_F_DISABLE | */CBM_Q_F_FLUSH | CBM_Q_F_FORCE_FLUSH));
	cbm_port_quick_reset(cbm_port_id, CBM_PORT_F_DEQUEUE_PORT);
}

static int plat_dp_open(struct net_device *dev)
{
	int ret;
	struct tc_priv_data *priv = netdev_priv(dev);
	struct dp_subif dp_subif = {0};

	plat_dp_reset(priv->dp_port_id);

	/* Enable UMT counter update */
	priv->umt_enabled = true;

	dp_subif.port_id = priv->dp_port_id;
	dp_subif.subif = -1;
	ret = dp_register_subif_ext(0, priv->owner, dev, dev->name,
			&dp_subif, NULL, 0);
	if (ret < 0) {
		pr_err("dp_register_subif_ext() failed!\n");
		goto err_out_register;
	}
	priv->dp_subif_id = dp_subif.subif;
	return 0;

err_out_register:
	return ret;
}

static void plat_dp_stop(struct net_device *dev)
{
	int ret;
	struct tc_priv_data *priv = netdev_priv(dev);
	struct dp_subif dp_subif = {0};

	dp_subif.port_id = priv->dp_port_id;
	dp_subif.subif = priv->dp_subif_id;
	ret = dp_register_subif_ext(0, priv->owner, dev, dev->name,
			&dp_subif, NULL, DP_F_DEREGISTER);
	if (ret < 0) {
		pr_err("Unregister pid(%d) subif(%d) fail!\n",
			priv->dp_port_id, priv->dp_subif_id);
	}

	/* Disable UMT counter update */
	priv->umt_enabled = false;
}

/* Thanks Venkatesh Bolla */
static void swap_mac(struct sk_buff *skb)
{
	u8 *data = (u8 *)skb_mac_header(skb);
	u8 tmp[6];

	ether_addr_copy(tmp, data);
	ether_addr_copy(data, data+6);
	ether_addr_copy(data+6, tmp);
}

/* Thanks Venkatesh Bolla */
static void swap_ip(struct sk_buff *skb)
{
	u32 *data = (u32 *)skb_network_header(skb);
	u32 tmp = 0;

	data += 3;
	tmp = data[0];
	data[0] = data[1];
	data[1] = tmp;
}

static int plat_dp_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct tc_priv_data *priv = netdev_priv(dev);
	struct dp_subif dp_subif = {0};

	dp_subif.port_id = priv->dp_port_id;
	dp_subif.subif = (priv->dp_subif_id | (priv->qid << 3));

	set_mask_bit(skb->DW1, dp_subif.port_id, SKB_PORT_ID, SKB_PORT_ID_S);
	set_mask_bit(skb->DW0, dp_subif.subif, SKB_PORT_SUBID, SKB_PORT_SUBID_S);

	/* swap MACs and IP addrs, then IP works: thanks Venkatesh Bolla */
	swap_mac(skb);
	swap_ip(skb);

	return dp_xmit(dev, &dp_subif, skb, skb->len, 0);
}

/*
 * ####################################
 * Bonding interface operations
 * ####################################
 */
#ifndef MAX_EP
#define MAX_EP 2
#endif
#ifndef TXIN_SOC_DES_NUM
#define TXIN_SOC_DES_NUM	256
#endif
#ifndef TXOUT_SOC_DES_NUM
#define TXOUT_SOC_DES_NUM	256
#endif
#ifndef RXOUT_SOC_DES_NUM
#define RXOUT_SOC_DES_NUM	256
#endif
#ifndef RXIN_SOC_DES_NUM
#define RXIN_SOC_DES_NUM	RXOUT_SOC_DES_NUM
#endif

static struct bonding_cb_ops *g_bonding_ops;
static struct tc_cb_ops g_tc_ops = {
	.rx_atm_frame_cb = bonding_rx_cb,
	.tc_drv_cbm_buf_free = bonding_cbm_buf_free_cb,
	.tc_drv_cbm_dqptr_read = bonding_cbm_dqptr_read_cb,
	.cb_tc_receive_asm_ind = bonding_rx_asm_ind_cb
};
static struct bond_drv_cfg_params g_bonding_cfg = {0};

static int bonding_rx_cb(struct atm_aal5_t *atm_rx_pkt)
{
	int ret;
	struct tc_priv_data *priv = g_tc_priv;
	struct sk_buff *lcl_skb;

	if (!atm_rx_pkt || !atm_rx_pkt->skb) {
		pr_err("[%s:%d] receive failed!\n", __func__, __LINE__);
		return -1;
	}

	if (atm_rx_pkt->conn != priv->qid) {
		dev_kfree_skb_any(atm_rx_pkt->skb);
		pr_err("[%s:%d] rx qid (%d) does not match with %d!\n",
			__func__, __LINE__, atm_rx_pkt->conn, priv->qid);
		return -1;
	}

	lcl_skb = atm_rx_pkt->skb;
	lcl_skb->dev = priv->dev;
	lcl_skb->protocol = eth_type_trans(lcl_skb, lcl_skb->dev);
	ret = netif_rx(lcl_skb);
	if (ret == NET_RX_SUCCESS) {
		priv->stats.rx_packets++;
		priv->stats.rx_bytes += lcl_skb->len;
	}

	return 0;
}

static void bonding_cbm_buf_free_cb(void *phys)
{
	cbm_buffer_free(smp_processor_id(), phys, 1);
}

static int bonding_cbm_dqptr_read_cb(u32 *p)
{
	int ret = -1;

	if (likely(g_tc_priv->umt_enabled))
		ret = cbm_dequeue_dma_port_stats_get(g_tc_priv->cbm_pid, p, 0);

	return ret;
}

static void bonding_rx_asm_ind_cb(const unsigned char ep_id)
{
	pr_info("[%s:%d] ASM received on link %d\n", __func__, __LINE__, ep_id);
}

static int bonding_cfg_init(struct tc_priv_data *priv)
{
	int ret;
	int ep_id;
	struct cbm_dq_port_res cbm_res;
	void *desc_base;
	dma_addr_t dma_handle;

	memset(&priv->cfg, 0, sizeof(struct cfg_params));
	memset(&g_bonding_cfg, 0, sizeof(struct bond_drv_cfg_params));

	/* Host-side <TXIN,TXOUT> ring and umt configurations */
	memset(&cbm_res, 0, sizeof(cbm_res));
	ret = cbm_dequeue_port_resources_get(priv->dp_port_id, &cbm_res, 0);
	if (ret < 0) {
		pr_err("Get CBM DQ port res fail!\n");
		ret = -ENODEV;
		goto err_out;
	}
	if (cbm_res.num_deq_ports < 1) {
		pr_err("CBM port(%d) less than required(1)\n",
			cbm_res.num_deq_ports);
		ret = -ENODEV;
		goto err_out;
	}
	priv->cbm_pid = cbm_res.deq_info[0].port_no;

	priv->cfg.soc_conn.txin.type = TXIN;
	priv->cfg.soc_conn.txin.start = ((u32 __force)cbm_res.deq_info[0].cbm_dq_port_base);
	priv->cfg.soc_conn.txin.size = cbm_res.deq_info[0].num_desc;
	priv->cfg.soc_conn.txin.write_umt = (u32 *)((priv->cfg.soc_conn.txin.start & 0xFFFFF000) + 0x4); /* DQPC */

	priv->cfg.soc_conn.txout.type = TXOUT;
	priv->cfg.soc_conn.txout.start = ((u32 __force)cbm_res.cbm_buf_free_base);
	priv->cfg.soc_conn.txout.size = cbm_res.num_free_entries;

	kfree(cbm_res.deq_info);

	/* Bonding SoC configurations */
	g_bonding_cfg.soc_conn.txin.soc_desc_base = priv->cfg.soc_conn.txin.start;
	g_bonding_cfg.soc_conn.txin.soc_desc_num = priv->cfg.soc_conn.txin.size;
	g_bonding_cfg.soc_conn.txout.soc_desc_base = priv->cfg.soc_conn.txout.start;
	g_bonding_cfg.soc_conn.txout.soc_desc_num = priv->cfg.soc_conn.txout.size;
	/* NOTE: cbm register address, 4bytes offset used for TXIN cumulative counter */
	g_bonding_cfg.soc_umt_dst = priv->cfg.soc_conn.txin.write_umt - 1;
	pr_info("host, txin base: 0x%x, dnum: %d\n",
			g_bonding_cfg.soc_conn.txin.soc_desc_base,
			g_bonding_cfg.soc_conn.txin.soc_desc_num);
	pr_info("host, txout base: 0x%x, dnum: %d\n",
			g_bonding_cfg.soc_conn.txout.soc_desc_base,
			g_bonding_cfg.soc_conn.txout.soc_desc_num);

	/* Device-side <TXIN,TXOUT,RXOUT> ring and umt configuations */
	for (ep_id = 0; ep_id < MAX_EP; ep_id++) {
		desc_base = dma_zalloc_coherent(NULL, (TXIN_SOC_DES_NUM << 4),
				&dma_handle, GFP_DMA);
		if (!desc_base) {
			ret = -ENOMEM;
			goto err_out_cfg;
		}
		priv->cfg.ep_conn[ep_id].txin.type = TXIN;
		priv->cfg.ep_conn[ep_id].txin.start = (u32)desc_base;
		priv->cfg.ep_conn[ep_id].txin.start_phys = dma_handle;
		priv->cfg.ep_conn[ep_id].txin.size = TXIN_SOC_DES_NUM;

		desc_base = dma_zalloc_coherent(NULL, (TXOUT_SOC_DES_NUM << 2),
				&dma_handle, GFP_DMA);
		if (!desc_base) {
			ret = -ENOMEM;
			goto err_out_cfg;
		}
		priv->cfg.ep_conn[ep_id].txout.type = TXOUT;
		priv->cfg.ep_conn[ep_id].txout.start = (u32)desc_base;
		priv->cfg.ep_conn[ep_id].txout.start_phys = dma_handle;
		priv->cfg.ep_conn[ep_id].txout.size = TXOUT_SOC_DES_NUM;

		desc_base = dma_zalloc_coherent(NULL, (RXOUT_SOC_DES_NUM << 4),
				&dma_handle, GFP_DMA);
		if (!desc_base) {
			ret = -ENOMEM;
			goto err_out_cfg;
		}
		priv->cfg.ep_conn[ep_id].rxout.type = RXOUT;
		priv->cfg.ep_conn[ep_id].rxout.start = (u32)desc_base;
		priv->cfg.ep_conn[ep_id].rxout.start_phys = dma_handle;
		priv->cfg.ep_conn[ep_id].rxout.size = RXOUT_SOC_DES_NUM;
		priv->cfg.ep_conn[ep_id].rxin.type = RXIN;
		priv->cfg.ep_conn[ep_id].rxin.start = (u32)desc_base;
		priv->cfg.ep_conn[ep_id].rxin.start_phys = dma_handle;
		priv->cfg.ep_conn[ep_id].rxin.size = RXIN_SOC_DES_NUM;

		desc_base = dma_zalloc_coherent(NULL, 8, &dma_handle, GFP_DMA);
		if (!desc_base) {
			ret = -ENOMEM;
			goto err_out_cfg;
		}
		priv->cfg.ep_conn[ep_id].rxin.read_umt = (u32 *)desc_base;
		priv->cfg.ep_conn[ep_id].txin.read_umt = (u32 *)desc_base + 1;
		priv->cfg.ep_conn[ep_id].rxout.write_umt = &priv->cfg.ep_conn[ep_id].rxin.delta;
		priv->cfg.ep_conn[ep_id].rxin.write_umt = &priv->cfg.ep_conn[ep_id].rxout.delta;

		/* Bonding per EP configurations */
		g_bonding_cfg.ep_conn[ep_id].txin.soc_desc_num = priv->cfg.ep_conn[ep_id].txin.size;
		g_bonding_cfg.ep_conn[ep_id].txin.soc_desc_base = priv->cfg.ep_conn[ep_id].txin.start;
		g_bonding_cfg.ep_conn[ep_id].txin.soc_desc_base_phys = priv->cfg.ep_conn[ep_id].txin.start_phys;
		g_bonding_cfg.ep_conn[ep_id].txout.soc_desc_num = priv->cfg.ep_conn[ep_id].txout.size;
		g_bonding_cfg.ep_conn[ep_id].txout.soc_desc_base = priv->cfg.ep_conn[ep_id].txout.start;
		g_bonding_cfg.ep_conn[ep_id].txout.soc_desc_base_phys = priv->cfg.ep_conn[ep_id].txout.start_phys;
		g_bonding_cfg.ep_conn[ep_id].rxout.soc_desc_num = priv->cfg.ep_conn[ep_id].rxout.size;
		g_bonding_cfg.ep_conn[ep_id].rxout.soc_desc_base = priv->cfg.ep_conn[ep_id].rxout.start;
		g_bonding_cfg.ep_conn[ep_id].rxout.soc_desc_base_phys = priv->cfg.ep_conn[ep_id].rxout.start_phys;
		g_bonding_cfg.ep_conn[ep_id].rxin.soc_desc_num = priv->cfg.ep_conn[ep_id].rxin.size;
		g_bonding_cfg.ep_conn[ep_id].rxin.soc_desc_base = priv->cfg.ep_conn[ep_id].rxin.start;
		g_bonding_cfg.ep_conn[ep_id].rxin.soc_desc_base_phys = priv->cfg.ep_conn[ep_id].rxin.start_phys;
		g_bonding_cfg.aca_umt_dst[ep_id] = (u32)priv->cfg.ep_conn[ep_id].rxin.read_umt;
		g_bonding_cfg.aca_umt_hdparams[ep_id][ACA_RXIN] = (u32)priv->cfg.ep_conn[ep_id].rxin.read_umt;
		g_bonding_cfg.aca_umt_hdparams[ep_id][ACA_TXIN] = (u32)priv->cfg.ep_conn[ep_id].txin.read_umt;
		pr_info("ep%d, txin, base: 0x%x, phys_base: 0x%x, dnum: %d\n", ep_id,
				g_bonding_cfg.ep_conn[ep_id].txin.soc_desc_base,
				priv->cfg.ep_conn[ep_id].txin.start_phys,
				g_bonding_cfg.ep_conn[ep_id].txin.soc_desc_num);
		pr_info("ep%d, txout, base: 0x%x, phys_base: 0x%x, dnum: %d\n", ep_id,
				g_bonding_cfg.ep_conn[ep_id].txout.soc_desc_base,
				priv->cfg.ep_conn[ep_id].txout.start_phys,
				g_bonding_cfg.ep_conn[ep_id].txout.soc_desc_num);
		pr_info("ep%d, rxout, base: 0x%x, phys_base: 0x%x, dnum: %d\n", ep_id,
				g_bonding_cfg.ep_conn[ep_id].rxout.soc_desc_base,
				priv->cfg.ep_conn[ep_id].rxout.start_phys,
				g_bonding_cfg.ep_conn[ep_id].rxout.soc_desc_num);
		pr_info("ep%d, rxin, base: 0x%x, phys_base: 0x%x, dnum: %d\n", ep_id,
				g_bonding_cfg.ep_conn[ep_id].rxin.soc_desc_base,
				priv->cfg.ep_conn[ep_id].rxin.start_phys,
				g_bonding_cfg.ep_conn[ep_id].rxin.soc_desc_num);
	}

	return 0;

err_out_cfg:
	bonding_cfg_uninit(priv);
err_out:
	return ret;
}

static void bonding_cfg_uninit(struct tc_priv_data *priv)
{
	int ep_id;

	/* Host-side <TXIN,TXOUT> ring and umt configuations */

	/* Device-side <TXIN,TXOUT,RXOUT> ring and umt configuations */
	for (ep_id = 0; ep_id < MAX_EP; ep_id++) {
		if (priv->cfg.ep_conn[ep_id].txin.start)
			dma_free_coherent(NULL,
				(priv->cfg.ep_conn[ep_id].txin.size << 4),
				(void *)priv->cfg.ep_conn[ep_id].txin.start, 0);
		if (priv->cfg.ep_conn[ep_id].txout.start)
			dma_free_coherent(NULL,
				(priv->cfg.ep_conn[ep_id].txout.size << 2),
				(void *)priv->cfg.ep_conn[ep_id].txout.start, 0);
		if (priv->cfg.ep_conn[ep_id].rxout.start)
			dma_free_coherent(NULL,
				(priv->cfg.ep_conn[ep_id].rxout.size << 4),
				(void *)priv->cfg.ep_conn[ep_id].rxout.start, 0);
		if (priv->cfg.ep_conn[ep_id].rxin.read_umt)
			dma_free_coherent(NULL, 8,
				(void *)priv->cfg.ep_conn[ep_id].rxin.read_umt, 0);
	}
	memset(&priv->cfg, 0, sizeof(struct cfg_params));
	memset(&g_bonding_cfg, 0, sizeof(struct bond_drv_cfg_params));
}

static int bonding_device_init(struct net_device *dev)
{
	int ret = 0;
	struct tc_priv_data *priv = netdev_priv(dev);

	/* start loopback polling thread */
	if (!priv->poll_thread && DEVICE_LOOPBACK)
		priv->poll_thread = kthread_run(loopback_poll_handler, priv, "loopback_poll");

	if (g_bonding_ops) {
		ret = g_bonding_ops->cb_bonding_device_init(&g_bonding_cfg);
		if (ret) {
			pr_err("cb_bonding_device_init() failed!");
			goto err_out_init;
		}
	}

	return 0;

err_out_init:
	return ret;
}

static void bonding_device_uninit(struct net_device *dev)
{
	struct tc_priv_data *priv = netdev_priv(dev);
	int ep_id;

	if (g_bonding_ops) {
		for (ep_id = 0; ep_id < MAX_EP; ep_id++)
			bonding_showtime_exit(ep_id);

		g_bonding_ops->cb_bonding_device_deinit();
	}

	/* stop loopback polling thread */
	if (priv->poll_thread) {
		kthread_stop(priv->poll_thread);
		priv->poll_thread = NULL;
	}
}

static void bonding_showtime_enter(int ep_id)
{
	if (g_bonding_ops) {
		g_bonding_ops->cb_showtime_enter(ep_id);
		g_conf.showtime[ep_id] = true;
	}
}

static void bonding_showtime_exit(int ep_id)
{
	if (g_bonding_ops) {
		g_bonding_ops->cb_showtime_exit(ep_id);
		g_conf.showtime[ep_id] = false;
	}
}

int vrx_register_cb_init(struct bonding_cb_ops *bonding_ops, struct tc_cb_ops *tc_ops)
{
	g_bonding_ops = bonding_ops;
	if (tc_ops)
		*tc_ops = g_tc_ops;
	return 0;
}
EXPORT_SYMBOL(vrx_register_cb_init);

void vrx_deregister_cb_uninit(void)
{
	g_bonding_ops = NULL;
}
EXPORT_SYMBOL(vrx_deregister_cb_uninit);

/*
 * ####################################
 * Device loopback operations
 * ####################################
 */
#define MAX_BUDGET	32
#define POLL_TIMEOUT	100 /* in us */

struct desc_1dw {
	u32 data_pointer;
} __packed __aligned(4);

struct desc_4dw {
	union {
		struct {
			u32 resv:17;
			u32 dest_id:15;
		} __packed field;
		u32 all;
	} __packed dw0;
	union {
		struct {
			u32 resv:20;
			u32 ep:4;
			u32 resv1:8;
		} __packed field;
		u32 all;
	} __packed dw1;
	dma_addr_t data_pointer; /* dw2 */
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 resv:2;
			u32 byte_offset:3;
			u32 resv1:7;
			u32 data_len:16;
		} __packed field;
		u32 all;
	} __packed status; /*dw3 */
} __packed __aligned(16);

struct desc_4dw_rx {
	union {
		struct {
			u32 resv:20;
			u32 ep:4;
			u32 resv1:8;
		} __packed field;
		u32 all;
	} __packed dw1;
	union {
		struct {
			u32 resv:17;
			u32 dest_id:15;
		} __packed field;
		u32 all;
	} __packed dw0;
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 resv:2;
			u32 byte_offset:3;
			u32 resv1:7;
			u32 data_len:16;
		} __packed field;
		u32 all;
	} __packed status; /*dw3 */
	dma_addr_t data_pointer; /* dw2 */
} __packed __aligned(16);

static bool is_readable(struct ring *ring)
{
	bool readable = false;
	struct desc_4dw *txin_desc;
	struct desc_4dw_rx *rxin_desc;

	if (ring->type == TXIN) {
		txin_desc = (struct desc_4dw *)ring->start;
		txin_desc += ring->read_idx;

		readable = (txin_desc->status.field.own == 1);
	} else if (ring->type == RXIN) {
		rxin_desc = (struct desc_4dw_rx *)ring->start;
		rxin_desc += ring->read_idx;

		readable = ((rxin_desc->status.field.own == 0) & (ring->delta > 0));
	}

	return readable;
}

static bool is_writable(struct ring *ring)
{
	bool writable = false;
	struct desc_4dw_rx *rxout_desc;
	struct desc_1dw *txout_desc;

	if (ring->type == RXOUT) {
		rxout_desc = (struct desc_4dw_rx *)ring->start;
		rxout_desc += ring->write_idx;

		writable = ((rxout_desc->status.field.own == 0) & (ring->delta < ring->size));
	} else if (ring->type == TXOUT) {
		txout_desc = (struct desc_1dw *)ring->start;
		txout_desc += ring->write_idx;

		writable = (txout_desc->data_pointer == 0);
	}

	return writable;
}

static int dequeue(struct ring *ring, struct desc_4dw *desc_out)
{
	struct desc_4dw *txin_desc_in;
	struct desc_4dw_rx *rxin_desc_in;

	if (ring->type == TXIN) {
		txin_desc_in = (struct desc_4dw *)ring->start;
		txin_desc_in += ring->read_idx;

		memcpy(desc_out, txin_desc_in, sizeof(struct desc_4dw));
		txin_desc_in->status.field.own = 0;
	} else if (ring->type == RXIN) {
		rxin_desc_in = (struct desc_4dw_rx *)ring->start;
		rxin_desc_in += ring->read_idx;

		desc_out->dw0.all = rxin_desc_in->dw0.all;
		desc_out->dw1.all = rxin_desc_in->dw1.all;
		desc_out->data_pointer = rxin_desc_in->data_pointer;
		desc_out->status.all = rxin_desc_in->status.all;
	}

	//pr_info("DEQ %s[%d], dw0:0x%08x, dw1:0x%08x, dw2:0x%08x, dw3:0x%08x\n",
	//		((ring->type == RXIN) ? "rxin" : "txin"),
	//		ring->read_idx, desc_out->dw0.all, desc_out->dw1.all,
	//		desc_out->data_pointer, desc_out->status.all);

	ring->read_idx++;
	if (ring->read_idx >= ring->size)
		ring->read_idx = 0;

	ring->total++;
	if (ring->type == RXIN) {
		ring->delta--;
		(*ring->write_umt)--;
	}

	return 0;
}

static int enqueue(struct ring *ring, struct desc_4dw *desc_in)
{
	struct desc_4dw_rx *rxout_desc_out;
	struct desc_1dw *txout_desc_out;

	if (ring->type == RXOUT) {
		rxout_desc_out = (struct desc_4dw_rx *)ring->start;
		rxout_desc_out += ring->write_idx;

		rxout_desc_out->dw1.all = desc_in->dw1.all;
		rxout_desc_out->dw0.all = desc_in->dw0.all;
		rxout_desc_out->status.all = desc_in->status.all;
		rxout_desc_out->data_pointer = desc_in->data_pointer;

		wmb();
		rxout_desc_out->status.field.own = 1;
	} else if (ring->type == TXOUT) {
		txout_desc_out = (struct desc_1dw *)ring->start;
		txout_desc_out += ring->write_idx;

		txout_desc_out->data_pointer = desc_in->data_pointer;
		wmb();
	}

	//pr_info("ENQ %s[%d], dw0:0x%08x, dw1:0x%08x, dw2:0x%08x, dw3:0x%08x\n",
	//		((ring->type == RXOUT) ? "rxout" : "txout"),
	//		ring->write_idx, desc_in->dw0.all, desc_in->dw1.all,
	//		desc_in->data_pointer, desc_in->status.all);

	ring->write_idx++;
	if (ring->write_idx >= ring->size)
		ring->write_idx = 0;

	ring->total++;
	if (ring->type == RXOUT) {
		ring->delta++;
		(*ring->write_umt)++;
	}

	return 0;
}

static void reset(struct tc_priv_data *priv)
{
	u8 ep;

	for (ep = 0; ep < MAX_EP; ep++) {
		priv->cfg.ep_conn[ep].txin.read_idx = 0;
		priv->cfg.ep_conn[ep].txin.write_idx = 0;
		priv->cfg.ep_conn[ep].txin.total = 0;
		priv->cfg.ep_conn[ep].txin.delta = 0;

		priv->cfg.ep_conn[ep].txout.read_idx = 0;
		priv->cfg.ep_conn[ep].txout.write_idx = 0;
		priv->cfg.ep_conn[ep].txout.total = 0;
		priv->cfg.ep_conn[ep].txout.delta = 0;

		priv->cfg.ep_conn[ep].rxout.read_idx = 0;
		priv->cfg.ep_conn[ep].rxout.write_idx = 0;
		priv->cfg.ep_conn[ep].rxout.total = 0;
		priv->cfg.ep_conn[ep].rxout.delta = 0;

		priv->cfg.ep_conn[ep].rxin.read_idx = 0;
		priv->cfg.ep_conn[ep].rxin.write_idx = 0;
		priv->cfg.ep_conn[ep].rxin.total = 0;
		priv->cfg.ep_conn[ep].rxin.delta = 0;
	}
}

static int loopback_poll_handler(void *data)
{
	int ep;
	int budget;
	struct desc_4dw desc;
	struct tc_priv_data *priv = (struct tc_priv_data *)data;

	reset(priv);

	pr_info("[%s:%d] loopback polling thread started\n", __func__, __LINE__);
	do {
		for (ep = 0; ep < MAX_EP; ep++) {
			/* TXIN->RXOUT transfer */
			for (budget = 0; budget < MAX_BUDGET; budget++) {
				if (!is_readable(&priv->cfg.ep_conn[ep].txin) ||
					!is_writable(&priv->cfg.ep_conn[ep].rxout))
					break;

				//pr_info("[%s:%d] TXIN->RXOUT transfer\n", __func__, __LINE__);
				memset(&desc, 0, sizeof(struct desc_4dw));
				dequeue(&priv->cfg.ep_conn[ep].txin, &desc);
				desc.status.field.own = 0;
				enqueue(&priv->cfg.ep_conn[ep].rxout, &desc);
			}

			/* RXIN->TXOUT transfer */
			for (budget = 0; budget < MAX_BUDGET; budget++) {
				if (!is_readable(&priv->cfg.ep_conn[ep].rxin) ||
					!is_writable(&priv->cfg.ep_conn[ep].txout))
					break;

				//pr_info("[%s:%d] RXIN->TXOUT transfer\n", __func__, __LINE__);
				memset(&desc, 0, sizeof(struct desc_4dw));
				dequeue(&priv->cfg.ep_conn[ep].rxin, &desc);
				desc.status.field.own = 0;
				enqueue(&priv->cfg.ep_conn[ep].txout, &desc);
			}
			//msleep(POLL_TIMEOUT);
			usleep_range(POLL_TIMEOUT-10, POLL_TIMEOUT+10);
		}
	} while (!kthread_should_stop());
	pr_info("[%s:%d] loopback polling thread stopped\n", __func__, __LINE__);

	return 0;
}

ssize_t config_read(struct file *file, char __user *buff, size_t count, loff_t *offset)
{
	unsigned char config_buf[CONFIG_BUFF_LEN] = {0};
	memset(config_buf, 0, sizeof(config_buf));
	sprintf(config_buf, "%d %d\n", g_conf.showtime[0], g_conf.showtime[1]);
	return simple_read_from_buffer(buff, count, offset, config_buf, CONFIG_BUFF_LEN);
}

ssize_t config_write(struct file *file, const char __user *buff, size_t count, loff_t *offset)
{
	unsigned char config_buf[CONFIG_BUFF_LEN] = {0};
	int ret = 0;

	if (count > CONFIG_BUFF_LEN)
		return -EINVAL;

	memset(config_buf, 0, sizeof(config_buf));

	ret = simple_write_to_buffer(config_buf, CONFIG_BUFF_LEN - 1, offset, buff, count);

	if (NULL == g_bonding_ops)
		goto end;

	if (g_conf.showtime[0] != (bool)(config_buf[0] - 48)) {
		g_conf.showtime[0] = (bool)(config_buf[0] - 48);

		if (true == g_conf.showtime[0])
			bonding_showtime_enter(0);
		else
			bonding_showtime_exit(0);
	}

	if (g_conf.showtime[1] != (bool)(config_buf[2] - 48)) {
		g_conf.showtime[1] = (bool)(config_buf[2] - 48);

		if (true == g_conf.showtime[1])
			bonding_showtime_enter(1);
		else
			bonding_showtime_exit(1);
	}

end:
	return ret;
}

int init_debug_interface(void)
{
	int ret = 0;
	int i;
	struct tc_sample_dbgfs_list *p;
	struct dentry *file;
	g_dbgfs_node = debugfs_create_dir(TC_SAMPLE_DBGFS_NAME, NULL);
	if (!g_dbgfs_node) {
		pr_err("failed to create %s", TC_SAMPLE_DBGFS_NAME);
		ret = -ENOMEM;
		goto err_out;
	}
	for (i = 0; i < ARRAY_SIZE(tc_sample_dbgfs_entries); i++) {
		p = &tc_sample_dbgfs_entries[i];
		file = debugfs_create_file(p->name, p->mode,
					   g_dbgfs_node,
					   NULL, p->fops);
		if (!file) {
			pr_err("failed to create %s", p->name);
			ret = -ENOMEM;
			goto err_out_file;
		}
	}

	/**************************************************************
	 *
	 * [link0 showtime entry/exit] [link1 showtime entry/exit]
	 * [1/0]                       [1/0]
	 *
	 * By default both lines are in showtime exit
	 *
	 ***************************************************************/

	return ret;
err_out_file:
	debugfs_remove_recursive(g_dbgfs_node);
err_out:
	return ret;
}

void cleanup_debug_interface(void)
{
	debugfs_remove_recursive(g_dbgfs_node);
}

/*
 * ####################################
 * Init/Cleanup API
 * ####################################
 */

static __init int atm_tc_drv_sample_init_module(void)
{
	pr_info("Loading sample TC driver ...... ");
	tc_device_init();
	init_debug_interface();
	pr_info("Succeeded!\n");
	return 0;
}

static __exit void atm_tc_drv_sample_exit_module(void)
{
	pr_info("Unloading sample TC driver ...... ");
	cleanup_debug_interface();
	tc_device_uninit();
	pr_info("Succeeded!\n");
}

module_init(atm_tc_drv_sample_init_module);
module_exit(atm_tc_drv_sample_exit_module);

MODULE_AUTHOR("Anath Bandhu Garai");
MODULE_DESCRIPTION("Sample TC driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.1");
