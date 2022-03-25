// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2009~2015 Lantiq Deutschland GmbH
 * Copyright (C) 2016~2018 Intel Corporation.
 */
#define CONFIG_USERSPACE_LINK_NOTIFICATION

#include <linux/version.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/kernel.h> /* printk() */
#include <linux/types.h>  /* size_t */
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/proc_fs.h>
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/thermal.h>

#ifdef CONFIG_USERSPACE_LINK_NOTIFICATION
#include <net/genetlink.h>
#endif

#include <lantiq.h>
#include <net/datapath_api.h>
#include <net/lantiq_cbm_api.h>
#include <net/switch_api/lantiq_gsw_api.h>
#include <net/switch_api/gsw_dev.h>
#include "xrx500_phy_fw.h"
#include "ltq_eth_drv_xrx500.h"
#include "xpcs/xpcs.h"
/* 9600 - ethernet overhead of 18 bytes */
#define LTQ_ETH_MAX_DATA_LEN 9582
/* Min IPv4 MTU per RFC791 */
#define LTQ_ETH_MIN_MTU 68

#define DRV_MODULE_NAME		"lantiq_eth_drv_xrx500"
#define DRV_MODULE_VERSION	 "1.1"

/* length of time before we decide the hardware is borked,
 * and dev->eth_tx_timeout() should be called to fix the problem
 */
#define LTQ_TX_TIMEOUT		(10 * HZ)
#define MDIO_PHY_ADDR_MASK	0x001f
#define MDIO_PHY_LINK_DOWN	0x4000
#define MDIO_PHY_LINK_UP	0x2000

#define MDIO_PHY_FDUP_EN	0x0600
#define MDIO_PHY_FDUP_DIS	0x0200

#define LTQ_ETH_NUM_INTERRUPTS 5

#define NUM_IF 6

/* Init of the network device */
static int ltq_eth_init(struct net_device *dev);
/* Start the network device interface queue */
static int ltq_eth_open(struct net_device *dev);
/* Stop the network device interface queue */
static int ltq_eth_stop(struct net_device *dev);
/* Uninit the network device interface queue */
static void ltq_eth_uninit(struct net_device *dev);
/* Transmit packet from Tx Queue to MAC */
static int ltq_start_xmit(struct sk_buff *skb, struct net_device *dev);
/* Hardware specific IOCTL's  */
static int ltq_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
/* Get the network device statistics */
static
struct rtnl_link_stats64 *ltq_get_stats(struct net_device *dev,
					struct rtnl_link_stats64 *storage);
/* change MTU values */
static int ltq_change_mtu(struct net_device *dev, int new_mtu);
/*  Set mac address*/
static int ltq_set_mac_address(struct net_device *dev, void *p);
/* Transmit timeout*/
static void ltq_tx_timeout(struct net_device *dev);

/* interface change event handler */
static int phy_netdevice_event(struct notifier_block *nb, unsigned long action,
			       void *ptr);
static int xrx500_mdio_probe(struct net_device *dev, struct xrx500_port *port);
static int ltq_gsw_pmac_init(void);

/**
 *  Datapath directpath functions
 */
static int32_t dp_fp_stop_tx(struct net_device *);
static int32_t dp_fp_restart_tx(struct net_device *);
static int32_t dp_fp_rx(struct net_device *, struct net_device *,
			struct sk_buff *, int32_t);

static struct xrx500_hw xrx500_hw;

static char wan_iface[IFNAMSIZ] = "eth1";
static int dev_num;

static struct module g_ltq_eth_module[NUM_ETH_INF];
static u32 g_rx_csum_offload;
static u32 g_eth_switch_mode;
static struct ltq_net_soc_data g_soc_data;
static struct net_device *eth_dev[NUM_IF];

static const struct net_device_ops ltq_eth_drv_ops = {
	.ndo_init		= ltq_eth_init,
	.ndo_open		= ltq_eth_open,
	.ndo_stop		= ltq_eth_stop,
	.ndo_uninit		= ltq_eth_uninit,
	.ndo_start_xmit		= ltq_start_xmit,
	.ndo_set_mac_address	= ltq_set_mac_address,
	.ndo_change_mtu		= ltq_change_mtu,
	.ndo_get_stats64	= ltq_get_stats,
	.ndo_do_ioctl		= ltq_ioctl,
	.ndo_tx_timeout		= ltq_tx_timeout,
};

static struct notifier_block netdevice_notifier = {
	.notifier_call = phy_netdevice_event
};

#undef DUMP_PACKET

#ifdef DUMP_PACKET
/**
 * \brief dump skb data
 * \param[in] len length of the data buffer
 * \param[in] p_data Pointer to data to dump
 *
 * \return void No Value
 */
static inline void dump_skb(u32 len, char *p_data)
{
	int i;

	for (i = 0; i < len; i++) {
		pr_info("%2.2x ", (u8)(p_data[i]));

		if (i % 16 == 15)
			pr_info("\n");
	}

	pr_info("\n");
}
#endif

/* Get the driver information, used by ethtool_ops  */
static void get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	/* driver driver short name (Max 32 characters) */
	strcpy(info->driver, DRV_MODULE_NAME);
	/* driver version(Max 32 characters) */
	strcpy(info->version, DRV_MODULE_VERSION);
}

/* Get the network device settings  */
static int get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct ltq_eth_priv *priv;
	int i;

	priv = netdev_priv(dev);

	pr_debug("in ethtool get setting for num %d\n", priv->num_port);
	for (i = 0; i < priv->num_port; i++)
		if (priv->port[i].phydev)
			return phy_ethtool_gset(priv->port[i].phydev, cmd);

	return -ENODEV;
}

/* Set the network device settings */
static int set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct ltq_eth_priv *priv;
	int i;

	priv = netdev_priv(dev);
	pr_info("%s: called\n", __func__);

	for (i = 0; i < priv->num_port; i++)
		if (priv->port[i].phydev)
			return phy_ethtool_sset(priv->port[i].phydev, cmd);

	return -ENODEV;
}

static int get_link_ksettings(struct net_device *dev,
			      struct ethtool_link_ksettings *cmd)
{
	int ret;

	ret = phy_ethtool_get_link_ksettings(dev, cmd);
	if (ret)
		return ret;

	/* Linux PHY framework does not yet support 2500baseT,
	 * so we need to configure manually here.
	 */
	if (test_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
		     dev->phydev->extended_supported))
		__set_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			  cmd->link_modes.supported);
	else
		__clear_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			    cmd->link_modes.supported);

	if (test_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
		     dev->phydev->extended_advertising))
		__set_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			  cmd->link_modes.advertising);
	else
		__clear_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			    cmd->link_modes.advertising);

	if (test_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
		     dev->phydev->extended_lp_advertising))
		__set_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			  cmd->link_modes.lp_advertising);
	else
		__clear_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			    cmd->link_modes.lp_advertising);

	return 0;
}

static int set_link_ksettings(struct net_device *dev,
			      const struct ethtool_link_ksettings *cmd)
{
	/* Linux PHY framework does not yet support 2500baseT,
	 * so we need to configure manually here.
	 */
	if (test_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
		     cmd->link_modes.advertising))
		__set_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			  dev->phydev->extended_advertising);
	else
		__clear_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			    dev->phydev->extended_advertising);

	return phy_ethtool_set_link_ksettings(dev, cmd);
}

/* Reset the device */
static int nway_reset(struct net_device *dev)
{
	/*TODO*/
	return 0;
}

static int  ethtool_eee_get(struct net_device *dev,
			    struct ethtool_eee *eee_data)
{
	struct core_ops *ops = gsw_get_swcore_ops(0);
	GSW_portLinkCfg_t	port_link_cfg = {0};
	int retval = -EOPNOTSUPP;

	if (!ops)
		return -ENODEV;
	port_link_cfg.nPortId = dev->dev_id;
	retval = ops->gsw_common_ops.PortLinkCfgGet(ops, &port_link_cfg);

	if (retval != 0) {
		pr_err("%s: gsw PortLinkCfgGet failed\n", __func__);
		return retval;
	}

	if (port_link_cfg.bLPI) {
		phy_ethtool_get_eee(dev->phydev, eee_data);
		eee_data->eee_active = port_link_cfg.bLPI;
		eee_data->eee_enabled = port_link_cfg.bLPI;
		eee_data->tx_lpi_enabled = port_link_cfg.bLPI;
	}

	return retval;
}

static int  ethtool_eee_set(struct net_device *dev,
			    struct ethtool_eee *eee_data)
{
	struct core_ops *ops = gsw_get_swcore_ops(0);
	GSW_portLinkCfg_t	port_link_cfg = {0};
	int retval = -EOPNOTSUPP;

	if (!ops)
		return -ENODEV;
	port_link_cfg.nPortId = dev->dev_id;
	retval = ops->gsw_common_ops.PortLinkCfgGet(ops, &port_link_cfg);

	if (retval != 0) {
		pr_err("%s: gsw PortLinkCfgGet failed\n", __func__);
		return retval;
	}

	/* xmac */
	port_link_cfg.bLPI ^= 1;
	retval =  ops->gsw_common_ops.PortLinkCfgSet(ops, &port_link_cfg);

	if (retval != 0) {
		pr_err("%s: gsw PortLinkCfgset failed\n", __func__);
		return retval;
	}

	/* phy */
	if (!eee_data->eee_active) {
		if (phy_init_eee(dev->phydev, 0)) {
			pr_err("%s: phy_init_eee failed\n", __func__);
			return retval;
		}
	}

	return retval;
}

int serdes_ethtool_get_link_ksettings(struct net_device *dev,
				      struct ethtool_link_ksettings *cmd)
{
	struct ltq_eth_priv *priv = netdev_priv(dev);
	struct platform_device *pdev;
	int ret = 0;

	if (!priv->xpcs_node)
		return -1;

	pdev = of_find_device_by_node(priv->xpcs_node);
	if (pdev) {
#ifdef CONFIG_INTEL_XPCS
		/* Speed Get in Ethtool */
		ret = xpcs_ethtool_ksettings_get(&pdev->dev, cmd);
#endif
	} else {
		pr_err("Cannot get Xpcs pdev for %s\n", dev->name);
		ret = -1;
	}
	return ret;
}

int serdes_ethtool_set_link_ksettings(struct net_device *dev,
				      const struct ethtool_link_ksettings *cmd)
{
	struct ltq_eth_priv *priv = netdev_priv(dev);
	int ret = 0;
	struct platform_device *pdev;

	if (!priv->xpcs_node)
		return -1;

	/* Speed Set in Ethtool */
	pdev = of_find_device_by_node(priv->xpcs_node);
	if (pdev) {
#ifdef CONFIG_INTEL_XPCS
		/* Speed Get in Ethtool */
		ret = xpcs_ethtool_ksettings_set(&pdev->dev, cmd);
#endif
	} else {
		 pr_err("Cannot get Xpcs pdev for %s\n", dev->name);
		ret = -1;
	}

	return ret;
}

static u32 get_priv_flags(struct net_device *dev)
{
	struct ltq_eth_priv *np = netdev_priv(dev);

	return np->ethtool_flags;
}

static int set_priv_flags(struct net_device *dev, u32 flags)
{
	struct ltq_eth_priv *np = netdev_priv(dev);
	struct dp_bp_attr bp_conf = {0};
	int ret = 0;

	bp_conf.dev = dev;

	if (flags & ETHTOOL_FLAG_BP_CPU_ENABLE) {
		bp_conf.en = true;
		ret = dp_set_bp_attr(&bp_conf, 0);
		if (ret != 0)
			return ret;
		np->ethtool_flags |= ETHTOOL_FLAG_BP_CPU_ENABLE;
	} else {
		bp_conf.en = false;
		ret = dp_set_bp_attr(&bp_conf, 0);
		if (ret != 0)
			return ret;
		np->ethtool_flags &= ~ETHTOOL_FLAG_BP_CPU_ENABLE;
	}

	return ret;
}

static const char priv_flags_strings[][ETH_GSTRING_LEN] = {
	"bp_to_cpu_enable",
};

#define PRIV_FLAGS_STR_LEN ARRAY_SIZE(priv_flags_strings)

static void get_strings(struct net_device *netdev, u32 stringset, u8 *data)
{
	int i;

	switch (stringset) {
	case ETH_SS_PRIV_FLAGS:
		for (i = 0; i < PRIV_FLAGS_STR_LEN; i++) {
			memcpy(data, priv_flags_strings[i],
			       ETH_GSTRING_LEN);
			data += ETH_GSTRING_LEN;
		}
		break;
	default:
		break;
	}
}

static int get_stringset_count(struct net_device *netdev, int sset)
{
	switch (sset) {
	case ETH_SS_PRIV_FLAGS:
		return PRIV_FLAGS_STR_LEN;
	default:
		return -EOPNOTSUPP;
	}
}

/* Structure of the ether tool operation in Phy case  */
static const struct ethtool_ops ethtool_ops = {
	.get_drvinfo		= get_drvinfo,
	.get_settings		= get_settings,
	.set_settings		= set_settings,
	.nway_reset		= nway_reset,
	.get_link		= ethtool_op_get_link,
	.get_link_ksettings	= get_link_ksettings,
	.set_link_ksettings	= set_link_ksettings,
	.get_eee		= ethtool_eee_get,
	.set_eee		= ethtool_eee_set,
	.get_strings		= get_strings,
	.get_sset_count		= get_stringset_count,
	.get_priv_flags		= get_priv_flags,
	.set_priv_flags		= set_priv_flags,
};

/* Structure of the ether tool operation in No-Phy case */
static const struct ethtool_ops serdes_ethtool_ops = {
	.get_drvinfo		= get_drvinfo,
	.get_link_ksettings	= serdes_ethtool_get_link_ksettings,
	.set_link_ksettings	= serdes_ethtool_set_link_ksettings,
	.get_strings		= get_strings,
	.get_sset_count		= get_stringset_count,
	.get_priv_flags		= get_priv_flags,
	.set_priv_flags		= set_priv_flags,
};

/* open the network device interface*/
static int ltq_eth_open(struct net_device *dev)
{
	int ret;
	int flags = 1;  /* flag 1 for enable */

	pr_debug("%s called for device %s\n", __func__, dev->name);
#ifdef CONFIG_GRX500_CBM
	/* Enable the p2p channel at CBM */
	if (!cbm_turn_on_DMA_p2p())
		pr_info("p2p channel turned ON !\n");
	else
		pr_info("p2p channel already ON !\n");
#endif

	ret = dp_rx_enable(dev, dev->name, flags);

	if (ret != DP_SUCCESS) {
		pr_err("%s: failed to enable for device: %s ret %d\n",
		       __func__, dev->name, ret);
		return -1;
	}

	return 0;
}

/* Stop the network device interface*/
static int ltq_eth_stop(struct net_device *dev)
{
	int ret;
	int flags = 0;  /* flag 0 for disable */

	pr_debug("%s called for device %s\n", __func__, dev->name);
	ret = dp_rx_enable(dev, dev->name, flags);

	if (ret != DP_SUCCESS) {
		pr_err("%s: failed to disable for device: %s ret %d\n",
		       __func__, dev->name, ret);
		return -1;
	}
	/* netif_stop_queue(dev); */

	return 0;
}

/* Uninit the network device interface*/
static void ltq_eth_uninit(struct net_device *dev)
{
	struct ltq_eth_priv *priv;
	struct dp_subif_data data = {0};
	int ret;

	pr_debug("%s called for device %s\n", __func__, dev->name);
	priv = netdev_priv(dev);

	if (priv->dp_port_id == DP_FAILURE) {
		pr_debug("dp port id (%d) is invalid. ignore the deregister.\n",
			 priv->dp_port_id);
		return;
	}

	if (priv->lct_en == 1) {
		priv->dp_subif.subif = -1;
		priv->dp_subif.port_id = priv->dp_port_id;
		data.flag_ops = DP_F_DATA_LCT_SUBIF;
		ret = dp_register_subif_ext(0, priv->owner,
					    dev, dev->name,
					    &priv->dp_subif,
					    &data, DP_F_DEREGISTER);
	} else {
		priv->dp_subif.subif = -1;
		priv->dp_subif.port_id = priv->dp_port_id;
		ret = dp_register_subif_ext(0, priv->owner,
					    dev, dev->name,
					    &priv->dp_subif,
					    NULL, DP_F_DEREGISTER);
	}
	if (ret != DP_SUCCESS)
		pr_err("%s: failed to close for device: %s ret %d\n",
		       __func__, dev->name, ret);
}

/* Send the packet to netwrok rx queue */
static void eth_rx(struct net_device *dev, int len, struct sk_buff *skb)
{
	struct ltq_eth_priv *priv;

	priv = netdev_priv(dev);
	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);

	if (dev->features & NETIF_F_RXCSUM)
		skb->ip_summed = CHECKSUM_UNNECESSARY;

	pr_debug("passing to stack: protocol: %x\n", skb->protocol);
	netif_rx(skb);
	priv->stats.rx_packets++;
	priv->stats.rx_bytes += len;
}

static int32_t dp_fp_stop_tx(struct net_device *netif)
{
	return 0;
}

static int32_t dp_fp_restart_tx(struct net_device *netif)
{
	return 0;
}

static int32_t dp_fp_rx(struct net_device *rxif, struct net_device *txif,
			struct sk_buff *skb, int32_t len)
{
	/*skb_put(skb,len);*/
#ifdef DUMP_PACKET
	if (skb->data) {
		pr_ifo("raw data len:%d\n", len);
		dump_skb(len, (char *)skb->data);
	}

#endif

	if (skb) {
		/*Remove PMAC to DMA header */
		len -= (sizeof(ltq_pmac_header_t));
		skb_pull(skb, (sizeof(ltq_pmac_header_t)));
	} else {
		pr_err("%s: skb from DP is null !\n", __func__);
		goto rx_err_exit;
	}

	/* Pass it to the stack */
#ifdef DUMP_PACKET

	if (skb->data) {
		pr_info("data sent to stack.\n");
		dump_skb(len, (char *)skb->data);
	}

#endif

	if (rxif) {
		pr_debug("%s: rxed a packet from DP lib on interface %s\n",
			 __func__, rxif->name);
		eth_rx(rxif, len, skb);
	} else {
		pr_err("%s: error: rxed a packet from DP lib on interface %x\n",
		       __func__, (unsigned int)rxif);
		goto rx_err_exit;
	}

	return 0;
rx_err_exit:

	if (skb)
		dev_kfree_skb_any(skb);

	return -1;
}

/* Get the network device stats information */
static struct
rtnl_link_stats64 *ltq_get_stats(struct net_device *dev,
				 struct rtnl_link_stats64 *storage)
{
#ifndef CONFIG_INTEL_DATAPATH_MIB
	struct ltq_eth_priv *priv = netdev_priv(dev);
	*storage = priv->stats;
#else
	dp_get_netif_stats(dev, NULL, storage, 0);
#endif
	return storage;
}

/* Trasmit timeout */
static void ltq_tx_timeout(struct net_device *dev)
{
}

/* Set the MAC address */
static int ltq_set_mac_address(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;
	struct ltq_eth_priv *priv = netdev_priv(dev);

	pr_debug("set_mac_addr called\n");
	if (netif_running(dev))
		return -EBUSY;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EINVAL;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	if (priv->xgmac_id >= 0) {
		struct mac_ops *ops;

		ops = gsw_get_mac_ops(0, priv->xgmac_id);
		if (ops)
			ops->set_macaddr(ops, dev->dev_addr);
	}
	return 0;
}

void gsw_reg_set_bit(struct core_ops *ops,  u16 nRegAddr, u16	nData)
{
	GSW_register_t reg_cfg;

	memset((void *)&reg_cfg, 0x00, sizeof(reg_cfg));
	reg_cfg.nRegAddr = nRegAddr;
	ops->gsw_common_ops.RegisterGet(ops, &reg_cfg);
	reg_cfg.nData |= nData;
	ops->gsw_common_ops.RegisterSet(ops, &reg_cfg);
}

void gsw_reg_clr_bit(struct core_ops *ops, u16 nRegAddr, u16 nData)
{
	GSW_register_t reg_cfg;

	memset((void *)&reg_cfg, 0x00, sizeof(reg_cfg));
	reg_cfg.nRegAddr = nRegAddr;
	ops->gsw_common_ops.RegisterGet(ops, &reg_cfg);
	reg_cfg.nData &= ~nData;
	ops->gsw_common_ops.RegisterSet(ops, &reg_cfg);
}

void gsw_reg_set_val(struct core_ops *ops, u16 nRegAddr, u16 nData)
{
	GSW_register_t reg_cfg;

	memset((void *)&reg_cfg, 0x00, sizeof(reg_cfg));
	reg_cfg.nRegAddr = nRegAddr;
	reg_cfg.nData = nData;
	ops->gsw_common_ops.RegisterSet(ops, &reg_cfg);
}

u16 gsw_reg_get_val(struct core_ops *ops, u16 nRegAddr)
{
	GSW_register_t reg_cfg;

	memset((void *)&reg_cfg, 0x00, sizeof(reg_cfg));
	reg_cfg.nRegAddr = nRegAddr;
	ops->gsw_common_ops.RegisterGet(ops, &reg_cfg);

	return reg_cfg.nData;
}

static int ltq_enable_gsw_l_jumbo(struct net_device *dev)
{
	struct ltq_eth_priv *priv = netdev_priv(dev);
	struct core_ops *ops;

	pr_info("%s called for id: %d\n", __func__, priv->id);

	/* Do the GSW-L configuration */
	pr_info("doing the GSW-L configuration for jumbo\n");
	ops = gsw_get_swcore_ops(0);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	/* Set the MAC control register 2 to enable Jumbo frames */
	gsw_reg_set_bit(ops, (0x8f9 + (0xc * (priv->id + 1))), 0x8);

	/* PMAC control register 2 to enable jumbo frames */
	pr_info("doing the PMAC configuration for jumbo at GSW-L\n");
	gsw_reg_set_bit(ops, 0xd05, 0x8);

	/* Set the jumbo frame length threshold */
	gsw_reg_set_val(ops, 0xd06, 9216);

	return 0;
}

static int ltq_disable_gsw_l_jumbo(struct net_device *dev)
{
	struct ltq_eth_priv *priv = netdev_priv(dev);
	struct core_ops *ops;

	pr_info("%s called for id: %d\n", __func__, priv->id);

	/* Do the GSW-L configuration */
	pr_info("doing the GSW-L configuration for jumbo\n");
	ops = gsw_get_swcore_ops(0);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	/* Set the MAC control register 2 to disable Jumbo frames */
	gsw_reg_clr_bit(ops, (0x8f9 + (0xc * (priv->id + 1))), 0x8);

	return 0;
}

static int ltq_enable_gsw_r_pmac_jumbo(struct net_device *dev)
{
	/* Do the GSW-R configuration */
	struct core_ops *ops = gsw_get_swcore_ops(1);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	/* PMAC control register 2 to enable jumbo frames */
	pr_info("doing the PMAC configuration for jumbo at GSW-R\n");
	gsw_reg_set_bit(ops, 0xd05, 0x8);

	/* Set the jumbo frame length threshold */
	gsw_reg_set_val(ops, 0xd06, 1662);

	return 0;
}

static int ltq_enable_gsw_r_jumbo(struct net_device *dev)
{
	/* Do the GSW-R configuration */
	struct core_ops *ops = gsw_get_swcore_ops(1);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	/* Set the MAC control register 2 to enable Jumbo frames */
	pr_info("doing the GSW-R configuration for jumbo\n");
	gsw_reg_set_bit(ops, 0x905, 0x8);

	return 0;
}

static int ltq_disable_gsw_r_jumbo(struct net_device *dev)
{
	/* Do the GSW-R configuration */
	struct core_ops *ops = gsw_get_swcore_ops(1);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	/* Set the MAC control register 2 to enable Jumbo frames */
	pr_info("doing the GSW-R configuration for jumbo\n");
	gsw_reg_clr_bit(ops, 0x905, 0x8);

	return 0;
}

static void ltq_set_max_pkt_len (struct ltq_eth_priv *priv, u16 pkt_len)
{
	struct core_ops *ops = NULL;
	pkt_len += ETH_HLEN + ETH_FCS_LEN;
	if (priv != NULL) {
		/* Set the GSW-L MAC_FLEN register for max packet length */
		if (!priv->wan) {
			ops = gsw_get_swcore_ops(0);
			if (ops == NULL)
				return;

			pr_info("doing the GSW-L MAC_FLEN configuration\n");
			gsw_reg_set_val(ops, 0x8C5, max(pkt_len, gsw_reg_get_val(ops, 0x8C5)));
		}

		/* Set the GSW-R MAC_FLEN register for max packet length */
		ops = gsw_get_swcore_ops(1);
		if (ops == NULL)
			return;

		pr_info("doing the GSW-R MAC_FLEN configuration\n");
		gsw_reg_set_val(ops, 0x8C5, max(pkt_len, gsw_reg_get_val(ops, 0x8C5)));
	}
}

/* Change the MTU value of the netwrok device interfaces */
static int ltq_change_mtu(struct net_device *dev, int new_mtu)
{
	struct ltq_eth_priv *priv = netdev_priv(dev);

	if (new_mtu < LTQ_ETH_MIN_MTU || new_mtu > g_soc_data.mtu_limit) {
		pr_err("%s: Invalid MTU size %u\n", __func__, new_mtu);
		return -EINVAL;
	}

	if (g_eth_switch_mode == 0) {
		if (priv->xgmac_id >= 0) {
			struct mac_ops *ops;

			ops = gsw_get_mac_ops(0, priv->xgmac_id);

			/* In OMCI daemon, need to use ifconfig to set mtu to
			 * [L2 Maximum Frame Size - 18] ex: (1518 - 18 = 1500)
			 * 18 bytes is the ETH header and FCS
			 */
			if (ops)
				ops->set_mtu(ops,
					    (new_mtu + ETH_HLEN + ETH_FCS_LEN));
		}
	} else {

		/* set the max pkt length on the switch */
		ltq_set_max_pkt_len(priv,(u16)new_mtu);

		/* if the MTU > 1500, do the jumbo config in switch */
		if (new_mtu > ETH_DATA_LEN && !(priv->jumbo_enabled)) {
			if (priv->wan) {
				if (ltq_enable_gsw_r_jumbo(dev) < 0)
					return -EIO;
			} else {
				if (ltq_enable_gsw_l_jumbo(dev) < 0)
					return -EIO;
			}

			ltq_enable_gsw_r_pmac_jumbo(dev);
			priv->jumbo_enabled = 1;
		} else if (new_mtu <= ETH_DATA_LEN && priv->jumbo_enabled) {
			if (priv->wan)
				ltq_disable_gsw_r_jumbo(dev);
			else
				ltq_disable_gsw_l_jumbo(dev);

			priv->jumbo_enabled = 0;
		}
	}

	dev->mtu = new_mtu;
	return 0;
}

static int ltq_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ltq_eth_priv *priv = netdev_priv(dev);
	int ret;
	int len;

	/* pad packet to avoid stuck in traffic queues
	   as ethernet expects min of ETH_ZLEN bytes */
	if (skb_put_padto(skb,ETH_ZLEN)) {
		priv->stats.tx_dropped++;
		return 0;
	}

	/* Call the Datapath Library's TX function */
	((struct dma_tx_desc_1 *)&skb->DW1)->field.ep = priv->dp_subif.port_id;
	((struct dma_tx_desc_0 *)&skb->DW0)->field.dest_sub_if_id =
		priv->dp_subif.subif;

	len = skb->len;

	if (dev->features & NETIF_F_HW_CSUM) {
		ret = dp_xmit(dev, &priv->dp_subif, skb, skb->len,
			      DP_TX_CAL_CHKSUM);
	} else {
		ret = dp_xmit(dev, &priv->dp_subif, skb, skb->len, 0);
	}
	if (!ret) {
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += len;
	} else {
		priv->stats.tx_dropped++;
	}
	return 0;
}

/* Platform specific IOCTL's handler */
static int ltq_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	return -EOPNOTSUPP;
}

/* init of the network device */
static int ltq_eth_init(struct net_device *dev)
{
	int i;
	struct ltq_eth_priv *priv;
	struct dp_subif_data data = {0};
	int ret;

	priv = netdev_priv(dev);
	pr_debug("probing for number of ports = %d\n", priv->num_port);
	pr_debug("%s called for device %s\n", __func__, dev->name);

	for (i = 0; i < priv->num_port; i++) {
		if (of_phy_is_fixed_link(priv->port[i].phy_node))
			pr_debug("phy is fixed-link\n");
		else if (g_soc_data.phy_connect_func(dev, &priv->port[i]))
			pr_warn("connect phy of port %d failed\n",
				priv->port[i].num);

		if (priv->port[i].phy_node)
			dev->ethtool_ops = &ethtool_ops;
		else if (IS_ENABLED(CONFIG_INTEL_XPCS))
			/* without xpcs node the num_port would be 0 */
			dev->ethtool_ops = &serdes_ethtool_ops;
	}

	if (priv->lct_en == 1) {
		priv->dp_subif.subif = -1;
		priv->dp_subif.port_id = priv->dp_port_id;
		data.flag_ops = DP_F_DATA_LCT_SUBIF;
		ret = dp_register_subif_ext(0, priv->owner,
					    dev, dev->name, &priv->dp_subif,
					    &data, 0);
	} else {
		priv->dp_subif.subif = -1;
		priv->dp_subif.port_id = priv->dp_port_id;
		ret = dp_register_subif_ext(0, priv->owner,
					    dev, dev->name, &priv->dp_subif,
					    NULL, 0);
	}
	if (ret != DP_SUCCESS) {
		pr_err("%s: failed to open for device: %s ret %d\n",
		       __func__, dev->name, ret);
		return -1;
	}
	pr_debug("\n subif reg for owner = %s portid = %d subifid = %d dev= %s\n",
		 priv->owner->name, priv->dp_port_id,
		 priv->dp_subif.subif, dev->name);

	/* get the minimum MTU and call the change mtu */
	dp_get_mtu_size(dev, &g_soc_data.mtu_limit);

	if (!(dev->mtu > LTQ_ETH_MIN_MTU && dev->mtu < g_soc_data.mtu_limit))
		dev->mtu = g_soc_data.mtu_limit;

	if (g_eth_switch_mode != 0)
		ltq_change_mtu(dev, dev->mtu);

	return 0;
}

static int ltq_gsw_pmac_init(void)
{
	u8 i = 0, j = 0, k = 0;
	GSW_PMAC_Eg_Cfg_t eg_cfg;
	GSW_PMAC_Ig_Cfg_t ig_cfg;
	GSW_CPU_PortCfg_t cpu_port_cfg;
	GSW_portCfg_t gsw_port_cfg;

	/* Do the GSW-L configuration */
	struct core_ops *ops = gsw_get_swcore_ops(0);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	memset((void *)&eg_cfg, 0x00, sizeof(eg_cfg));
	memset((void *)&ig_cfg, 0x00, sizeof(ig_cfg));
	memset((void *)&cpu_port_cfg, 0x00, sizeof(cpu_port_cfg));

	/**
	 * 1. GSWIP-L PMAC Egress Configuration Table
	 * Entry:
	 * Address: (i from 0 to 15, j from 0 to 3)
	 * Destination Port = 0, Traffic Class = i, Others field = j
	 * Value:
	 * DMA Channel = if i < 3? i: 3
	 * PMAC_Flag = 1
	 * Other fields = 0
	 */
	pr_info("PMAC_EG_CFG_SET for GSW-L\n");

	for (i = 0; i <= 15; i++) {
		for (j = 0; j <= 3; j++) {
			memset((void *)&eg_cfg, 0x00, sizeof(eg_cfg));
#if defined(SINGLE_RX_CH0_ONLY) && SINGLE_RX_CH0_ONLY
			eg_cfg.nRxDmaChanId  = 0;
#else
			eg_cfg.nRxDmaChanId  = (i < 3) ? i : 3;
#endif
			eg_cfg.bRemL2Hdr	 = 0;
			eg_cfg.numBytesRem   = 0;
			eg_cfg.bFcsEna	   = 0;
			eg_cfg.bPmacEna	  = 1;
			eg_cfg.nResDW1	   = 0;
			eg_cfg.nRes1DW0	  = 0;
			eg_cfg.nRes2DW0	  = 0;
			eg_cfg.nDestPortId   = 0;
			eg_cfg.nTrafficClass = i;
			eg_cfg.nFlowIDMsb	= j;
			eg_cfg.bDecFlag	  = 0;
			eg_cfg.bEncFlag	  = 0;
			eg_cfg.bMpe1Flag	 = 0;
			eg_cfg.bMpe2Flag	 = 0;
			eg_cfg.bTCEnable	 = 1;
			ops->gsw_pmac_ops.Pmac_Eg_CfgSet(ops, &eg_cfg);
		}
	}

	/**
	 * 2. GSWIP-L PMAC Ingress Configuration Table
	 * Entry:
	 * Address: (i from 0 to 15)
	 * DMA Channel = i
	 * Value:
	 * PMAC_Flag = 1
	 * SPPID_Mode = 1
	 * SUBID_Mode = 1
	 * CLASSEN_Mode = 1
	 * CLASS_Mode = 1
	 * PMAPEN_Mode = 1
	 * PMAP_Mode = 1
	 * TCPERR_DP = 1
	 * DF_PMAC_HD.PMAP_EN = 1, DF_PMAC_HD.PMAP = 1 << i[2:0],
	 * DF_PMAC_HD.CLASS_EN = 1, DF_PMAC_HD.CLASS = i[3]*2 + 1
	 * Other fields = 0
	 */
	pr_info("PMAC_IG_CFG_SET for GSW-L\n");

	for (i = 0; i <= 15; i++) {
		memset((void *)&ig_cfg, 0x00, sizeof(ig_cfg));

		ig_cfg.nTxDmaChanId = i;
		ig_cfg.bErrPktsDisc = 1;
		ig_cfg.bPmapDefault = 1;
		ig_cfg.bPmapEna	= 1;
		ig_cfg.bClassDefault = 1;
		ig_cfg.bClassEna	 = 1;
		ig_cfg.eSubId = 1;
		ig_cfg.bSpIdDefault  = 1;
		ig_cfg.bPmacPresent  = 1;
		ig_cfg.defPmacHdr[0] = 0;
		ig_cfg.defPmacHdr[1] = 0;
		ig_cfg.defPmacHdr[2] = ((((i & 8) >> 3) * 2) + 1);/* 1/3 */
		/* (1 << 7) | (1 << 4); */
		ig_cfg.defPmacHdr[3] = 0x90;
		ig_cfg.defPmacHdr[4] = 0;
		ig_cfg.defPmacHdr[5] = 0;
		/*if ( i >= 8)
		 *ig_cfg.defPmacHdr[6] = 1 << (i & 0x7);
		 *else
		 */
		ig_cfg.defPmacHdr[7] = 1 << (i & 0x7);
		ops->gsw_pmac_ops.Pmac_Ig_CfgSet(ops, &ig_cfg);
	}

	/* Enable the Ingress Special Tag */
	cpu_port_cfg.nPortId = 0;
	cpu_port_cfg.bCPU_PortValid = 0;
	cpu_port_cfg.bSpecialTagIngress = 1;
	cpu_port_cfg.bSpecialTagEgress = 0;
	cpu_port_cfg.bFcsCheck = 0;
	cpu_port_cfg.bFcsGenerate = 1;
	ops->gsw_common_ops.CPU_PortCfgSet(ops, &cpu_port_cfg);

#ifdef CONFIG_USE_EMULATOR
	/* Add some extra register writes for the 1.4.1 */
	gsw_reg_set_val(ops, 0x90f, 0x182);

	gsw_reg_set_val(ops, 0x91b, 0x182);

	gsw_reg_set_val(ops, 0x927, 0x182);

	gsw_reg_set_val(ops, 0x933, 0x182);

	gsw_reg_set_val(ops, 0x93f, 0x182);

#endif

	/* Enable the CPU port MAC address spoofing detection */
	gsw_reg_set_bit(ops, 0x480, 0x4000);

	/*PMAC control register 2 to disable LCHKS*/
	gsw_reg_clr_bit(ops, 0xd05, 0x3);

	/* Do the GSW-R configuration */
	ops = gsw_get_swcore_ops(1);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	/**
	 * 3. GSWIP-R PMAC Egress Configuration Table
	 * Entry:
	 * Address: (k i from 0 to 15,j from 0 to 63)
	 * Destination Port = k, Others field = j
	 * Value:
	 * DMA Channel = if k <= 13? 0: k
	 * PMAC_Flag = if k ==15 or k < 7? 1: 0
	 * FCS_Mode = if k ==13? PTM_Bonding_Mode:0
	 * L2_HD_RM = if k == 12 or k == 14? 1:0
	 * L2_HD_Bytes = if k == 14? CAPWAP_HD_BYTES:0
	 * Other fields = 0
	 */
	pr_info("PMAC_EG_CFG_SET for GSW-R\n");

	for (k = 0; k <= 15; k++) {
		for (i = 0; i <= 15; i++) {
			for (j = 0; j <= 3; j++) {
				memset((void *)&eg_cfg, 0x00, sizeof(eg_cfg));
#ifdef CONFIG_USE_EMULATOR
				eg_cfg.nRxDmaChanId  = (k <= 14) ? 0 : 5;
#else

				if (ltq_get_soc_rev() == 2)
					eg_cfg.nRxDmaChanId = (k <= 13) ? 0 : 5;
				else
					eg_cfg.nRxDmaChanId = (k <= 13) ? 0 : k;

#endif
				/*((k == 15) || (k < 7)) ? 1 : 0; */
				eg_cfg.bPmacEna = 1;
				/*PTM_Bonding_Mode*/
				eg_cfg.bFcsEna = (k == 13) ? 1 : 0;
				eg_cfg.bRemL2Hdr = 0;
				/*CAPWAP_HD_BYTES*/
				eg_cfg.numBytesRem = (k == 14) ? 1 : 0;
				eg_cfg.nResDW1	   = 0;
				eg_cfg.nRes1DW0	  = 0;
				eg_cfg.nRes2DW0	  = 0;
				eg_cfg.nDestPortId   = k;
				eg_cfg.nTrafficClass = i;
				eg_cfg.bMpe1Flag	 = 0;
				eg_cfg.bMpe2Flag	 = 0;
				eg_cfg.bEncFlag	  = 0;
				eg_cfg.bDecFlag	  = 0;
				eg_cfg.nFlowIDMsb	= j;
				eg_cfg.bTCEnable		= 1;

				ops->gsw_pmac_ops.Pmac_Eg_CfgSet(ops, &eg_cfg);
			}
		}
	}

	/**
	 * 4. GSWIP-R PMAC Ingress Configuration Table
	 * Entry:
	 * Address: (i from 0 to 15)
	 * DMA Channel  = i
	 * Value:
	 * PMAC_Flag = if 5 < i < 15? 0: 1
	 * SPPID_Mode = 0
	 * SUBID_Mode = If 7 <= i <=8? 0: 1
	 * CLASSEN_Mode = 0
	 * CLASS_Mode = 0
	 * PMAPEN_Mode = if i < 4 or i == 15? 1:0
	 * PMAP_Mode = if i < 4 or i == 15? 1:0
	 * TCPERR_DP = 1
	 * DF_PMAC_HD.SPPID = i, DF_PMAC_HD.PMAP = if i == 15? 0x8000:0xFFFF
	 * DF_PMAC_HD.PMAC_EN = if i==15?1:0
	 * Other fields = 0
	 */
	pr_info("PMAC_IG_CFG_SET for GSW-R\n");

	for (i = 0; i <= 15; i++) {
		memset((void *)&ig_cfg, 0x00, sizeof(ig_cfg));

		ig_cfg.nTxDmaChanId  = i;
		ig_cfg.bPmacPresent  = 1; /*((i > 5) && (i < 15)) ? 0 : 1; */
		/*ig_cfg.bSpIdDefault  = 0; */
		/* For channel 15, use source port ID from default PMAC header*/
		ig_cfg.bSpIdDefault = (i == 15) ? 1 : 0;
		ig_cfg.eSubId = 0; /*((i == 6) || (i == 13)) ? 0 : 1;*/
		ig_cfg.bClassDefault = 0;
		ig_cfg.bClassEna	 = 0;
		ig_cfg.bErrPktsDisc  = 1;

		ig_cfg.bPmapDefault  = ((i < 4) || (i == 15)) ? 1 : 0;
		ig_cfg.bPmapEna	  = ((i < 4) || (i == 15)) ? 1 : 0;

		ig_cfg.defPmacHdr[0] = 0;
		ig_cfg.defPmacHdr[1] = 0;
		/*ig_cfg.defPmacHdr[2] = (i << 4); // Byte 2 (Bits 7:4)*/
		/* For channel 15, source port is 0. */
		ig_cfg.defPmacHdr[2] = (i == 15) ? 0 : (i << 4);
		ig_cfg.defPmacHdr[3] = (i == 15) ? 0x80 : 0; /*Byte 3 (Bit 7) */
		ig_cfg.defPmacHdr[4] = 0;
		ig_cfg.defPmacHdr[5] = 0;
		ig_cfg.defPmacHdr[6] = (i == 15) ? 0x80 : 0xFF;
		ig_cfg.defPmacHdr[7] = (i == 15) ? 0x00 : 0xFF;

		ops->gsw_pmac_ops.Pmac_Ig_CfgSet(ops, &ig_cfg);
	}

	/* Enable the Ingress Special Tag */
	for (k = 0; k < 15; k++) {
		memset((void *)&cpu_port_cfg, 0x00, sizeof(cpu_port_cfg));

		cpu_port_cfg.nPortId = k;

		/* get the current values first */
		ops->gsw_common_ops.CPU_PortCfgGet(ops, &cpu_port_cfg);

		/* change the required values and set it back */
		cpu_port_cfg.bCPU_PortValid = 0;
		cpu_port_cfg.bSpecialTagIngress = 1;
		cpu_port_cfg.bSpecialTagEgress = 0;
		cpu_port_cfg.bFcsCheck = 0;
		cpu_port_cfg.bFcsGenerate = 1;
		ops->gsw_common_ops.CPU_PortCfgSet(ops, &cpu_port_cfg);
	}

	/* Disable the Learning in the GSWIP-R */
	for (k = 0; k < 15; k++) {
		memset((void *)&gsw_port_cfg, 0x00, sizeof(gsw_port_cfg));

		gsw_port_cfg.nPortId = k;
		ops->gsw_common_ops.PortCfgGet(ops, &gsw_port_cfg);

		gsw_port_cfg.bLearning = 1;
		ops->gsw_common_ops.PortCfgSet(ops, &gsw_port_cfg);
	}

#ifdef CONFIG_USE_EMULATOR
	/* Add some extra register writes for the 1.4.1 */
	gsw_reg_set_val(ops, 0x903, 0x182);
#endif

	gsw_reg_set_val(ops, 0x454, 0x1);

	gsw_reg_set_val(ops, 0x455, 0x1);

	/*PMAC control register 2 to disable LCHKS*/
	gsw_reg_clr_bit(ops, 0xd05, 0x3);

	pr_info("\n\t GSW PMAC Init Done!!!\n");
	return 0;
}

static void
ltq_eth_drv_eth_addr_setup(struct net_device *dev, int port)
{
	struct ltq_eth_priv *priv = netdev_priv(dev);

	if (is_valid_ether_addr(dev->dev_addr))
		return;

	eth_hw_addr_random(dev);
	pr_debug("using random mac for port %d.\n", port);

	if (priv->xgmac_id >= 0) {
		struct mac_ops *ops;

		ops = gsw_get_mac_ops(0, priv->xgmac_id);
		if (ops)
			ops->set_macaddr(ops, dev->dev_addr);
	}
}

#ifdef CONFIG_USERSPACE_LINK_NOTIFICATION
enum eth_cmd {
	ETH_C_UNSPEC,
	ETH_C_NOTIFY,
	__ETH_C_MAX,
};

#define ETH_C_MAX (__ETH_C_MAX - 1)

enum eth_attr {
	ETH_A_UNSPEC,
	ETH_A_LINK_NAME,
	ETH_A_LINK_STATUS,
	ETH_A_LINK_SPEED,
	__ETH_A_MAX,
};

#define ETH_LINK_A_MAX (__ETH_A_MAX - 1)

enum eth_multicast_groups {
	ETH_MCGRP,
};

/* VRX318 TC message multicast group */
static struct genl_multicast_group eth_grps[] = {
	[ETH_MCGRP] = {.name = "eth_mcgrp",},
};

/* VRX318 TC message genelink family */
static struct genl_family eth_gnl_family = {
	.id = GENL_ID_GENERATE, /* To generate an id for the family*/
	.hdrsize = 0,
	.name = "eth_drv_notify", /*family name, used by userspace application*/
	.version = 1, /*version number  */
	.maxattr = ETH_LINK_A_MAX,
	.mcgrps = eth_grps,
	.n_mcgrps = ARRAY_SIZE(eth_grps),
};

/* API definition for the driver to send TC notify messages to user application
 * using genetlink method.
 * pid: process id
 * tc_msg: 1-link is up,0- link is down
 * ln_no: interface name
 */
static int
ltq_eth_nl_msg_send(int pid, int link_status, int speed, char *if_name)
{
	struct sk_buff *skb;
	int ret;
	void *msg_head;

	skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);

	if (!skb)
		return -ENOMEM;

	/* create the message headers */
	msg_head = genlmsg_put(skb, 0, 0,
			       &eth_gnl_family, 0, ETH_C_NOTIFY);

	if (!msg_head) {
		ret = -ENOMEM;
		kfree_skb(skb);
		pr_err("create message header fail!\n");
		goto out;
	}

	nla_put_string(skb, ETH_A_LINK_NAME, if_name);

	if (link_status) {
		nla_put_string(skb, ETH_A_LINK_STATUS, "up");
		nla_put_u16(skb, ETH_A_LINK_SPEED, speed);
	} else {
		nla_put_string(skb, ETH_A_LINK_STATUS, "down");
	}
	genlmsg_end(skb, msg_head);
	ret = genlmsg_multicast(&eth_gnl_family, skb, pid,
				ETH_MCGRP, GFP_KERNEL);

	if (ret != 0 && ret != -ESRCH) {
		pr_err("failed to send out the multicast message:ret = %d\n",
		       ret);
		goto out;
	}

	return 0;
out:
	return ret;
}

static int ltq_eth_genetlink_init(void)
{
	int ret;

	/*register new family*/
	ret = genl_register_family(&eth_gnl_family);

	if (ret != 0) {
		pr_err("Family registeration fail:%s\n",
		       eth_gnl_family.name);
	}

	return ret;
}

static void ltq_eth_genetlink_exit(void)
{
	int ret;

	/*unregister the family*/
	ret = genl_unregister_family(&eth_gnl_family);

	if (ret != 0)
		pr_err("unregister Genetlink family %i\n", ret);
}
#endif

/* This function is event handler for net_device change notify */
static int phy_netdevice_event(struct notifier_block *nb, unsigned long action,
			       void *ptr)
{
	struct ltq_eth_priv *priv = NULL;
	struct net_device *dev = NULL;

	if (!ptr)
		return 0;

	dev = netdev_notifier_info_to_dev(ptr);

	switch (action) {
	case NETDEV_CHANGENAME: {
		priv = netdev_priv(dev);

		if (priv && priv->wan) {
			pr_debug("\nUpdate WAN iface from [%s] to [%s]\n",
				 wan_iface, dev->name);
			memcpy(wan_iface, dev->name, IFNAMSIZ);
		}
	}
	}

	return NOTIFY_OK;
}

#ifdef CONFIG_XRX500_ETH_DRV_THERMAL_SUPPORT

static int ltq_eth_thermal_get_max_state(struct thermal_cooling_device *cdev,
					 unsigned long *state)
{
	struct xrx500_hw *hw = cdev->devdata;
	struct xrx500_thermal *thermal = &hw->thermal;

	*state = thermal->max_state;

	return 0;
}

static int ltq_eth_thermal_get_cur_state(struct thermal_cooling_device *cdev,
					 unsigned long *state)
{
	struct xrx500_hw *hw = (struct xrx500_hw *)cdev->devdata;
	struct xrx500_thermal *thermal = &hw->thermal;

	*state = thermal->cur_state;

	return 0;
}

static int advertising_min = ADVERTISED_10baseT_Half  |
			    ADVERTISED_10baseT_Full;

static int ltq_eth_thermal_recover_state(struct net_device *dev)
{
	struct ltq_eth_priv *priv = netdev_priv(dev);
	int ret = 0;

	if (priv && priv->needs_recovery) {
		ret = set_settings(dev, &priv->bkup_cmd);
		if (ret)
			return ret;

		priv->needs_recovery = false;
	}

	return ret;
}

static int ltq_eth_thermal_set_cur_state(struct thermal_cooling_device *cdev,
					 unsigned long state)
{
	struct ethtool_cmd cmd;
	int ret, i, j;
	struct xrx500_hw *hw = cdev->devdata;
	struct xrx500_thermal *thermal = &hw->thermal;

	if (WARN_ON(state > thermal->max_state))
		return -EINVAL;

	if (thermal->cur_state == state)
		return 0;

	for (i = 0, j = 0; i < hw->num_devs; i++) {
		struct net_device *dev = hw->devs[i];
		struct ltq_eth_priv *priv = netdev_priv(dev);

		if (state == 0) { /* cooling disabled */
			if (ltq_eth_thermal_recover_state(dev))
				pr_err("%s: failed to recover settings for %s\n",
				       __func__, dev->name);
			continue;
		}

		if (priv->wan)
			continue;

		if (!netif_carrier_ok(dev))
			continue;  /* phy puts hw in sleep if no link */

		ret = get_settings(dev, &cmd);
		if (ret) {
			pr_err("Failed to get settings for %s\n", dev->name);
			continue;
		}

		if (j < state) { /* reduce speed */
			if (cmd.advertising == advertising_min)
				continue;

			priv->bkup_cmd.phy_address = cmd.phy_address;
			priv->bkup_cmd.advertising = cmd.advertising;
			priv->bkup_cmd.autoneg = cmd.autoneg;

			cmd.cmd = ETHTOOL_SLINKSETTINGS;
			cmd.advertising = advertising_min;
			cmd.autoneg = AUTONEG_ENABLE;

			ret = set_settings(dev, &cmd);
			if (ret) {
				pr_err("%s: Failed to set settings for %s (reducing)\n",
				       __func__, dev->name);
				continue;
			}

			j++;
			priv->needs_recovery = true;

		} else { /* recover to previous settings */
			if (ltq_eth_thermal_recover_state(dev)) {
				pr_err("%s: Failed to set settings for %s (recovery)\n",
				       __func__, dev->name);
				continue;
			}
		}
	}

	thermal->cur_state = state;

	return ret;
}

static struct thermal_cooling_device_ops ltq_eth_thermal_ops = {
	.get_max_state = ltq_eth_thermal_get_max_state,
	.get_cur_state = ltq_eth_thermal_get_cur_state,
	.set_cur_state = ltq_eth_thermal_set_cur_state,
};

static ssize_t show_advrt_min(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	return sprintf(buf, "%d\n", advertising_min);
}

static ssize_t store_advrt_min(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	int advrt_min;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (kstrtoint(buf, 10, &advrt_min))
		return -EINVAL;

	advertising_min = advrt_min;

	return 0;
}
static DEVICE_ATTR(advrt_min, 0600, show_advrt_min, store_advrt_min);

static int ltq_eth_thermal_init(struct device_node *np, struct xrx500_hw *hw)
{
	struct thermal_cooling_device *cdev;

	hw->thermal.max_state = hw->num_devs;

	cdev = thermal_of_cooling_device_register(np, "eth_thermal", hw,
						  &ltq_eth_thermal_ops);
	if (IS_ERR(cdev)) {
		pr_err("Failed to setup thermal device (err %ld)\n",
		       PTR_ERR(cdev));
		return PTR_ERR(cdev);
	}

	hw->thermal.cdev = cdev;

	if (device_create_file(&cdev->device, &dev_attr_advrt_min)) {
		dev_err(&cdev->device,
			"couldn't create device file for advrt_min\n");
	}

	return 0;
}

void ltq_eth_thermal_exit(struct xrx500_hw *hw)
{
	device_remove_file(&hw->thermal.cdev->device, &dev_attr_advrt_min);
	thermal_cooling_device_unregister(hw->thermal.cdev);
}

#else

static int ltq_eth_thermal_init(struct device_node *np, struct xrx500_hw *hw)
{
	return 0;
}

static void ltq_eth_thermal_exit(struct xrx500_hw *hw)
{
}

#endif

static void
xrx500_of_port(struct net_device *dev, struct device_node *port)
{
	const __be32 *addr, *id = of_get_property(port, "reg", NULL);
	struct resource irqres;
	struct ltq_eth_priv *priv = netdev_priv(dev);
	struct xrx500_port *p = &priv->port[priv->num_port];

	if (!id)
		return;

	memset(p, 0, sizeof(struct xrx500_port));
	p->phy_node = of_parse_phandle(port, "phy-handle", 0);
	p->phy_if = of_get_phy_mode(port);

	if (!p->phy_node) {
		/* check if a fixed-link is defined in device-tree */
		if (of_phy_is_fixed_link(port)) {
			struct phy_device *phy;

			pr_debug("fixed-link detected\n");
			if (of_phy_register_fixed_link(port)) {
				pr_err("cannot register fixed PHY\n");
				return;
			}
			p->phy_node = of_node_get(port);
			phy = of_phy_connect(dev,
					     p->phy_node, NULL, 0,
					     p->phy_if);
			if (!phy)
				pr_err("can not get phy dev for fixed link\n");
			p->phydev = phy;
			netif_carrier_on(dev);
		} else if (priv->xpcs_node) {
			pr_debug("no PHY but connected to xpcs: %s\n",
				 of_node_full_name(priv->xpcs_node));
		} else {
			pr_info("no interface attached to this interface!\n");
			return;
		}
	} else {
		addr = of_get_property(p->phy_node, "reg", NULL);
		if (!addr) {
			pr_info("no real PHY attached to this interface!\n");
			return;
		}
		p->phy_addr = *addr;
		if (of_irq_to_resource_table(port, &irqres, 1) == 1)
			p->irq_num = irqres.start;
		else
			pr_debug("couldn't get irq number for gphy !!\n");
	}

	p->num = *id;
	priv->num_port++;

	/* is this port a wan port ? */
	if (priv->wan)
		priv->hw->wan_map |= BIT(p->num);

	priv->port_map |= BIT(p->num);

	/* store the port id in the hw struct so we can map ports -> devices */
	priv->hw->port_map[p->num] = priv->hw->num_devs;
}

static int ltq_eth_dev_reg(struct xrx500_hw *hw, u32 xgmac_id_param,
			   u32 lct_en, u32 dp_port, int start, int end)
{
	int i, err, num = 1;

	for (i = start; i < end; i++) {
		struct ltq_eth_priv *priv;

		eth_dev[i] = alloc_etherdev_mq(sizeof(struct ltq_eth_priv),
						g_soc_data.queue_num);
		if (!eth_dev[i]) {
			pr_debug("allocated failed for interface %d\n", i);
			return -ENOMEM;
		}
		priv = netdev_priv(eth_dev[i]);
		priv->dp_port_id = dp_port;
		priv->xgmac_id = xgmac_id_param;
		priv->hw = hw;
		priv->id = hw->num_devs;
		priv->owner = &g_ltq_eth_module[hw->num_devs];
		sprintf(priv->owner->name, "module%02d", priv->id);
		if (start == i && lct_en == 1) {
			priv->lct_en = 1;
			snprintf(eth_dev[i]->name, IFNAMSIZ,
				 "eth0_%d_%d_lct", hw->num_devs, num);
		} else {
			priv->lct_en = 0;
			snprintf(eth_dev[i]->name, IFNAMSIZ,
				 "eth0_%d_%d",  hw->num_devs, num);
		}

		eth_dev[i]->netdev_ops = &ltq_eth_drv_ops;
		ltq_eth_drv_eth_addr_setup(eth_dev[i], priv->id);
		err = register_netdev(eth_dev[i]);
		if (err) {
			pr_err("%s: failed to register netdevice: %p %d\n",
			       __func__, eth_dev[i], err);
				return -1;
		}
		num++;
	}
	return 0;
}

static int ltq_eth_dev_dereg_subif(int start, int end)
{
	int i;
	int res = DP_SUCCESS;
	struct ltq_eth_priv *priv;
	struct dp_subif_data data = {0};

	for (i = start; i < end; i++) {
		struct net_device *dev = eth_dev[i];

		priv = netdev_priv(dev);
		priv->dp_subif.subif = priv->dp_subif.subif;
		priv->dp_subif.port_id = priv->dp_subif.port_id;
		if (priv->lct_en == 1) {
			pr_debug("owner = %s portid = %d subifid = %d dev= %s\n",
				 priv->owner->name, priv->dp_port_id,
				 priv->dp_subif.subif, dev->name);
			res = dp_register_subif_ext(0, priv->owner,
						    dev, dev->name,
						    &priv->dp_subif,
						    &data, DP_F_DEREGISTER);
		}
		if (priv->lct_en == 0) {
			pr_debug("owner = %s portid = %d subifid = %d dev= %s\n",
				 priv->owner->name, priv->dp_port_id,
				 priv->dp_subif.subif, dev->name);
			res = dp_register_subif_ext(0, priv->owner,
						    dev, dev->name,
						    &priv->dp_subif, NULL,
						    DP_F_DEREGISTER);
		}
		if (res != DP_SUCCESS) {
			pr_err("%s: failed to deregister subif for device: %s ret %d\n",
			       __func__, dev->name, res);
			return -1;
		}
	}
	return 0;
}

static int ltq_eth_dev_dereg(int start, int end)
{
	int i;

	for (i = start; i < end; i++) {
		struct ltq_eth_priv *priv;
		struct net_device *dev = eth_dev[i];

		netif_stop_queue(dev);
		priv = netdev_priv(dev);
		priv->dp_port_id = DP_FAILURE;
		unregister_netdev(dev);
		free_netdev(dev);
	}
	return 0;
}

static int xrx500_of_iface(struct xrx500_hw *hw, struct device_node *iface,
			   struct platform_device *pdev)
{
	struct ltq_eth_priv *priv;
	struct dp_dev_data dev_data = {0};
	struct device_node *port, *mac_np;
	const __be32 *wan;
	u32 dp_dev_port_param, dp_port_id_param, xgmac_id_param;
	u32 lct_en_param = 0, extra_subif_param = 0;
	int net_start = 0, net_end = 0;
	dp_cb_t cb = {0};
	u32 dp_port_id = 0;
	char name[16];
	int ret;

	/* alloc the network device */
	hw->devs[hw->num_devs] = alloc_etherdev_mq(sizeof(struct ltq_eth_priv),
						   g_soc_data.queue_num);

	if (!hw->devs[hw->num_devs]) {
		pr_debug("allocated failed for interface %d\n",
			 hw->num_devs);
		return -ENOMEM;
	}

	priv = netdev_priv(hw->devs[hw->num_devs]);

	/* is this the wan interface ? */
	wan = of_get_property(iface, "lantiq,wan", NULL);

	if (wan && (*wan == 1))
		priv->wan = 1;
	else
		priv->wan = 0;

	/* setup the network device */
	if (priv->wan)
		snprintf(name, sizeof(name), wan_iface);
	else
		snprintf(name, sizeof(name), "eth0_%d", hw->num_devs);

	ret = of_property_read_u32(iface, "intel,dp-dev-port",
				   &dp_dev_port_param);
	if (ret < 0) {
		pr_info("ERROR : Property intel,dp-dev-port not read from DT for if %s\n",
			name);
		return ret;
	}

	ret = of_property_read_u32(iface, "intel,dp-port-id",
				   &dp_port_id_param);
	if (ret < 0) {
		pr_info("ERROR : Property intel,dp-port-id not read from DT for if %s\n",
			name);
		return ret;
	}

	priv->xgmac_id = -1;

	mac_np = of_parse_phandle(iface, "mac", 0);
	if (mac_np) {
		ret = of_property_read_u32(mac_np, "mac_idx", &xgmac_id_param);
		if (ret < 0) {
			pr_info("ERROR : Property mac_idx not read from DT for if %s\n",
				name);
			return ret;
		}

		priv->xgmac_id = xgmac_id_param;

		priv->xpcs_node = of_parse_phandle(mac_np, "xpcs", 0);
		if (!priv->xpcs_node) {
			pr_info("Cannot get xpcs node\n");
			return -EINVAL;
		}
	}

	ret = of_property_read_u32(iface, "intel,lct-en",
				   &lct_en_param);
	if (ret < 0) {
		pr_debug("Property intel,lct-en not exist for if %s\n",
			 name);
		priv->lct_en = -1;
	}
			pr_debug("Property intel,lct-en for if %s %d\n",
				 name, lct_en_param);
	ret = of_property_read_u32(iface, "intel,extra-subif",
				   &extra_subif_param);
	if (ret < 0) {
		pr_info("Property intel,extra-subif not exist for if %s %d\n",
			name, extra_subif_param);
		priv->extra_subif = -1;
	}	else {
			pr_info("Property intel,extra-subif for if %s %d\n",
				name, extra_subif_param);
			priv->extra_subif = extra_subif_param;
			priv->start = dev_num;
			priv->end = dev_num + priv->extra_subif;
			dev_num = priv->extra_subif;
	}

	strcpy(hw->devs[hw->num_devs]->name, name);
	hw->devs[hw->num_devs]->netdev_ops = &ltq_eth_drv_ops;
	hw->devs[hw->num_devs]->watchdog_timeo = LTQ_TX_TIMEOUT;
	hw->devs[hw->num_devs]->needed_headroom = sizeof(ltq_pmac_header_t);
	hw->devs[hw->num_devs]->dev_id = dp_port_id_param;
	SET_NETDEV_DEV(hw->devs[hw->num_devs], &pdev->dev);

	/* setup our private data */
	priv->hw = hw;
	priv->id = hw->num_devs;
	spin_lock_init(&priv->lock);

	priv->owner = &g_ltq_eth_module[hw->num_devs];
	sprintf(priv->owner->name, "module%02d", priv->id);

	if (priv->wan)
		dp_port_id  = dp_alloc_port(priv->owner, hw->devs[hw->num_devs],
					    dp_dev_port_param, dp_port_id_param,
					    NULL, DP_F_FAST_ETH_WAN);
	else
		dp_port_id  = dp_alloc_port(priv->owner, hw->devs[hw->num_devs],
					    dp_dev_port_param, dp_port_id_param,
					    NULL, DP_F_FAST_ETH_LAN);

	if (dp_port_id == DP_FAILURE) {
		pr_err("dp_alloc_port failed for %s with port_id %d\n",
		       hw->devs[hw->num_devs]->name, priv->id + 1);
		return -ENODEV;
	}

	priv->dp_port_id = dp_port_id;
	dev_data.max_ctp = 4;
	cb.stop_fn = (dp_stop_tx_fn_t)dp_fp_stop_tx;
	cb.restart_fn  = (dp_restart_tx_fn_t)dp_fp_restart_tx;
	cb.rx_fn = (dp_rx_fn_t)dp_fp_rx;

	if (dp_register_dev_ext(0, priv->owner,
				dp_port_id, &cb,
				&dev_data, 0) != DP_SUCCESS) {
		pr_err("dp_register_dev failed for %s\n and port_id %d",
		       hw->devs[hw->num_devs]->name, dp_port_id);
		dp_alloc_port(priv->owner, hw->devs[hw->num_devs],
			      dp_dev_port_param, dp_port_id_param,
			      NULL, DP_F_DEREGISTER);
		return -ENODEV;
	}

	/* load the ports that are part of the interface */
	for_each_child_of_node(iface, port) {
		if (of_device_is_compatible(port, "lantiq,xrx500-pdi-port"))
			xrx500_of_port(hw->devs[hw->num_devs], port);
	}

#ifdef CONFIG_LTQ_TOE_DRIVER
	hw->devs[hw->num_devs]->features = NETIF_F_SG | NETIF_F_TSO |
					   NETIF_F_HW_CSUM;
	hw->devs[hw->num_devs]->hw_features = NETIF_F_SG | NETIF_F_TSO |
					      NETIF_F_HW_CSUM;
	hw->devs[hw->num_devs]->vlan_features = NETIF_F_SG | NETIF_F_TSO |
						NETIF_F_HW_CSUM;
	hw->devs[hw->num_devs]->gso_max_size  = GSO_MAX_SIZE;
#else
	hw->devs[hw->num_devs]->features = NETIF_F_SG;
	hw->devs[hw->num_devs]->hw_features = NETIF_F_SG;
	if (g_soc_data.hw_checksum) {
		hw->devs[hw->num_devs]->features |= NETIF_F_HW_CSUM;
		hw->devs[hw->num_devs]->hw_features |= NETIF_F_HW_CSUM;
		pr_debug("%s: hw csum offload is enabled!\n", __func__);
	} else {
		pr_debug("%s: hw csum offload is disable!\n", __func__);
	}
#endif

	if (g_rx_csum_offload) {
		pr_info("%s: rx csum offload is enabled !\n", __func__);
		hw->devs[hw->num_devs]->features |= NETIF_F_RXCSUM;
		hw->devs[hw->num_devs]->hw_features |= NETIF_F_RXCSUM;
	} else {
		pr_info("%s: rx csum offload is disabled !\n", __func__);
	}
	ltq_eth_drv_eth_addr_setup(hw->devs[hw->num_devs], priv->id);

	/* register the actual device */
	if (!register_netdev(hw->devs[hw->num_devs]))
		pr_debug("%s: priv->extra_subif = %d interface %s !\n",
			 __func__, priv->extra_subif, name);
	if (extra_subif_param >= 1) {
		net_start = priv->start;
		net_end = priv->end;
		ltq_eth_dev_reg(&xrx500_hw, xgmac_id_param, lct_en_param,
				dp_port_id_param, net_start, net_end);
	}

	hw->num_devs++;

	return 0;
}

static int xrx500_mdio_pae_wr(struct mii_bus *bus, int addr, int reg, u16 val)
{
	GSW_MDIO_data_t mmd_data;
	struct core_ops *ops;

	pr_debug("%s called with phy addr:%d and reg: %x and val: %x\n",
		 __func__, addr, reg, val);

	ops = gsw_get_swcore_ops(1);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	memset((void *)&mmd_data, 0x00, sizeof(mmd_data));
	mmd_data.nAddressDev = addr;
	mmd_data.nAddressReg = reg;
	mmd_data.nData = val;
	ops->gsw_common_ops.MDIO_DataWrite(ops, &mmd_data);

	return 0;
}

static int xrx500_mdio_pae_rd(struct mii_bus *bus, int addr, int reg)
{
	GSW_MDIO_data_t mmd_data;
	struct core_ops *ops;

	pr_debug("%s called with phy addr:%d and reg: %x\n",
		 __func__, addr, reg);

	ops = gsw_get_swcore_ops(1);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	memset((void *)&mmd_data, 0x00, sizeof(mmd_data));
	mmd_data.nAddressDev = addr;
	mmd_data.nAddressReg = reg;
	ops->gsw_common_ops.MDIO_DataRead(ops, &mmd_data);

	pr_debug("returing: %x\n", mmd_data.nData);
	return mmd_data.nData;
}

static int xrx500_mdio_wr(struct mii_bus *bus, int addr, int reg, u16 val)
{
	GSW_MDIO_data_t mmd_data;
	struct core_ops *ops;

	pr_debug("%s called with phy addr:%d and reg: %x and val: %x\n",
		 __func__, addr, reg, val);

	ops = gsw_get_swcore_ops(0);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	memset((void *)&mmd_data, 0x00, sizeof(mmd_data));
	mmd_data.nAddressDev = addr;
	mmd_data.nAddressReg = reg;
	mmd_data.nData = val;
	ops->gsw_common_ops.MDIO_DataWrite(ops, &mmd_data);

	return 0;
}

static int xrx500_mdio_rd(struct mii_bus *bus, int addr, int reg)
{
	GSW_MDIO_data_t mmd_data;
	struct core_ops *ops;

	pr_debug("%s called with phy addr:%d and reg: %x\n",
		 __func__, addr, reg);

	ops = gsw_get_swcore_ops(0);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	memset((void *)&mmd_data, 0x00, sizeof(mmd_data));
	mmd_data.nAddressDev = addr;
	mmd_data.nAddressReg = reg;
	ops->gsw_common_ops.MDIO_DataRead(ops, &mmd_data);

	pr_debug("returing: %x\n", mmd_data.nData);
	return mmd_data.nData;
}

static int xrx500_of_mdio(struct xrx500_hw *hw, struct device_node *np)
{
	hw->mii_bus = mdiobus_alloc();

	if (!hw->mii_bus)
		return -ENOMEM;

	hw->mii_bus->read = xrx500_mdio_rd;
	hw->mii_bus->write = xrx500_mdio_wr;
	hw->mii_bus->name = "lantiq,xrx500-mdio";
	snprintf(hw->mii_bus->id, MII_BUS_ID_SIZE, "%x", 0);

	pr_info("registering one of MII bus\n");

	if (of_mdiobus_register(hw->mii_bus, np)) {
		mdiobus_free(hw->mii_bus);
		return -ENXIO;
	}

	return 0;
}

static int xrx500_of_mdio_pae(struct xrx500_hw *hw, struct device_node *np)
{
	hw->mii_bus_pae = mdiobus_alloc();

	if (!hw->mii_bus_pae)
		return -ENOMEM;

	hw->mii_bus_pae->read = xrx500_mdio_pae_rd;
	hw->mii_bus_pae->write = xrx500_mdio_pae_wr;
	hw->mii_bus_pae->name = "lantiq,xrx500-mdio-pae";
	snprintf(hw->mii_bus_pae->id, MII_BUS_ID_SIZE, "%x", 1);

	pr_info("registering PAE MII bus\n");

	if (of_mdiobus_register(hw->mii_bus_pae, np)) {
		mdiobus_free(hw->mii_bus_pae);
		return -ENXIO;
	}

	return 0;
}

#ifdef SW_POLLING
static void xrx500_gmac_update(struct xrx500_port *port)
{
	u16 phyaddr = port->phydev->addr & MDIO_PHY_ADDR_MASK;

	if (port->phydev->link)
		phyaddr |= MDIO_PHY_LINK_UP;
	else
		phyaddr |= MDIO_PHY_LINK_DOWN;

	if (port->phydev->duplex == DUPLEX_FULL)
		phyaddr |= MDIO_PHY_FDUP_EN;
	else
		phyaddr |= MDIO_PHY_FDUP_DIS;

	udelay(1);
}
#else
static void xrx500_gmac_update(struct xrx500_port *port)
{
}
#endif
static void xrx500_mdio_link(struct net_device *dev)
{
	struct ltq_eth_priv *priv;
	int i;

	priv = netdev_priv(dev);
	pr_debug("%s called..\n", __func__);

	for (i = 0; i < priv->num_port; i++) {
		if (!priv->port[i].phydev)
			continue;

		if (priv->port[i].link != priv->port[i].phydev->link) {
			xrx500_gmac_update(&priv->port[i]);
			priv->port[i].link = priv->port[i].phydev->link;
			/* netdev_info(dev, "port %d %s link\n",
			 *	priv->port[i].num,
			 *	(priv->port[i].link)?("got"):("lost"));
			 */
#ifdef CONFIG_USERSPACE_LINK_NOTIFICATION
			ltq_eth_nl_msg_send(0, priv->port[i].link,
					    priv->port[i].phydev->speed,
					    dev->name);
#endif
			phy_print_status(priv->port[i].phydev);
		}
	}
}

/* PRX300 MDIO bus interface is owned by xgmac. The PHY device therefore do not
 * need to be probed here, as it is done by xgmac. What we need to do is simply
 * connect the PHY.
 */
static int prx300_phy_connect(struct net_device *dev, struct xrx500_port *port)
{
	struct phy_device *phydev = NULL;
	struct ltq_eth_priv *priv = NULL;
	struct mac_ops *ops;

	if (!port->phy_node)
		return 0;

	priv = netdev_priv(dev);

	phydev = of_phy_find_device(port->phy_node);
	if (!phydev || !phydev->mdio.dev.driver) {
		netdev_err(dev, "Unable to find phydev\n");
		return -ENODEV;
	}

	phydev = of_phy_connect(dev, port->phy_node, &xrx500_mdio_link,
				0, port->phy_if);
	if (!phydev) {
		netdev_err(dev, "Unable to connect phydev\n");
		return -ENODEV;
	}

	phydev->advertising = phydev->supported;
	port->phydev = phydev;

	/* restart xgmac */
	ops = gsw_get_mac_ops(0, priv->xgmac_id);
	if (ops && ops->soft_restart)
		ops->soft_restart(ops);

	pr_info("%s: attached PHY [%s] (phy_addr=%s, irq=%d)\n",
		dev->name, phydev->drv->name,
		phydev_name(phydev), phydev->irq);

	phy_read_status(phydev);
	phy_start_aneg(phydev);

	return 0;
}

static int xrx500_mdio_probe(struct net_device *dev, struct xrx500_port *port)
{
	struct phy_device *phydev = NULL;
	struct ltq_eth_priv *priv = NULL;
	struct mii_bus *bus = NULL;

	priv = netdev_priv(dev);

	if (priv->wan)
		bus = priv->hw->mii_bus_pae;
	else
		bus = priv->hw->mii_bus;

	if (!bus) {
		pr_info("No mdio bus defined for this port!\n");
		return -ENODEV;
	}

	phydev = mdiobus_get_phy(bus, port->phy_addr);
	if (!phydev) {
		netdev_err(dev, "no PHY found\n");
		return -ENODEV;
	}

	pr_info("trying to connect: %s to device: %s with irq: %d\n",
		dev->name, phydev_name(phydev), port->irq_num);
	phydev->irq = port->irq_num;
	phydev = phy_connect(dev, phydev_name(phydev), &xrx500_mdio_link,
			     port->phy_if);

	if (IS_ERR(phydev)) {
		netdev_err(dev, "Could not attach to PHY\n");
		return PTR_ERR(phydev);
	}

	phydev->supported &= (SUPPORTED_10baseT_Half
			      | SUPPORTED_10baseT_Full
			      | SUPPORTED_100baseT_Half
			      | SUPPORTED_100baseT_Full
			      | SUPPORTED_1000baseT_Half
			      | SUPPORTED_1000baseT_Full
			      | SUPPORTED_Autoneg
			      | SUPPORTED_MII
			      | SUPPORTED_TP);
	phydev->advertising = phydev->supported;
	port->phydev = phydev;

	pr_info("%s: attached PHY [%s] (phy_addr=%s, irq=%d)\n",
		dev->name, phydev->drv->name,
		phydev_name(phydev), phydev->irq);

	phy_read_status(phydev);
	phy_start_aneg(phydev);
	return 0;
}

/* Initialization Ethernet module */
static int ltq_eth_drv_init(struct platform_device *pdev)
{
	int ret = 0;
	struct device_node *node = pdev->dev.of_node;
	struct device_node *mdio_np, *iface_np;
	struct mii_bus *bus;

	memset(g_ltq_eth_module, 0, sizeof(g_ltq_eth_module));

	ret = of_property_read_u32(node, "lantiq,eth-switch-mode",
				   &g_eth_switch_mode);

	if (ret < 0) {
		pr_info("couldn't get the eth switch mode from DT.");
		g_eth_switch_mode = 0;
		ret = 0;
	}

	if (g_eth_switch_mode > 0) {
		/* HW init of the Switch */
		if (ltq_gsw_pmac_init() < 0)
			pr_info("%s[%d]: switch PMAC init failed..\n",
				__func__, __LINE__);
	} else {
		pr_debug("Ethernet driver do not need to config the switch.\n");
	}

	ret = of_property_read_u32(node, "lantiq,eth-rx-csum-offload",
				   &g_rx_csum_offload);

	if (ret < 0) {
		dev_info(&pdev->dev,
			 "couldn't get the RX CSUM offload setting from DT.");
		dev_info(&pdev->dev, " by default its disabled !\n");
		ret = 0;
	}

	g_soc_data.mtu_limit = LTQ_ETH_MAX_DATA_LEN;

	/* bring up the mdio bus */
	mdio_np = of_find_compatible_node(node, NULL,
					  "lantiq,xrx500-mdio");

	if (mdio_np) {
		if (xrx500_of_mdio(&xrx500_hw, mdio_np))
			dev_err(&pdev->dev, "mdio probe failed\n");

		/* bring up the mdio bus for PAE */
		mdio_np = of_find_compatible_node(node, NULL,
						  "lantiq,xrx500-mdio-pae");

		if (mdio_np)
			if (xrx500_of_mdio_pae(&xrx500_hw, mdio_np))
				dev_err(&pdev->dev, "mdio probe of PAE failed\n");
	}

	bus = xrx500_hw.mii_bus;
	if (bus)
		bus->parent = &pdev->dev;

	/* load the interfaces */
	/* add a dummy interface */
	xrx500_hw.num_devs = 0;
	for_each_available_child_of_node(node, iface_np) {
		if (of_device_is_compatible(iface_np, "lantiq,xrx500-pdi")) {
			pr_debug("adding the interface: %d\n",
				 xrx500_hw.num_devs);

			if (!of_device_is_available(iface_np)) {
				pr_debug("device not available.\n");
				continue;
			}
			if (xrx500_hw.num_devs < NUM_ETH_INF) {
				xrx500_of_iface(&xrx500_hw, iface_np, pdev);
			} else {
				dev_err(&pdev->dev,
					"only %d interfaces allowed\n",
					NUM_ETH_INF);
			}
		}
	}

	if (!xrx500_hw.num_devs) {
		dev_err(&pdev->dev, "failed to load interfaces\n");
		return -ENOENT;
	}
#ifdef CONFIG_USERSPACE_LINK_NOTIFICATION
	/* Register the netlink notification */
	ltq_eth_genetlink_init();
#endif

	if (of_find_property(node, "#cooling-cells", NULL)) {
		ret = ltq_eth_thermal_init(node, &xrx500_hw);
		if (ret) {
			pr_err("%s: net cooling device not registered (err %d)\n",
			       __func__, ret);
		}
	}

	pr_info("Lantiq ethernet driver for XRX500 init.\n");
	return 0;
}

static void ltq_eth_drv_exit(struct platform_device *pdev)
{
	int i, ret, j;

	for (i = 0; i < xrx500_hw.num_devs; i++) {
		int net_start = 0, net_end = 0;
		struct ltq_eth_priv *priv;
		struct dp_dev_data dev_data = {0};
		struct net_device *dev = xrx500_hw.devs[i];

		if (!dev)
			continue;

		netif_stop_queue(dev);

		priv = netdev_priv(dev);

		pr_debug("num port %d\n", priv->num_port);

		for (j = 0; j < priv->num_port; j++)
			if (priv->port[j].phydev) {
				struct device_node *device;

				device = priv->port[j].phy_node;
				pr_debug("phy_disconnect phydev(%d) 0x%p\n",
					 j, priv->port[j].phydev);
				phy_disconnect(priv->port[j].phydev);
				if (of_phy_is_fixed_link(device)) {
					of_phy_deregister_fixed_link(device);
					pr_debug("deregister the fixed link\n");
				}
			}

		pr_debug("ltq_eth_drv_exit i %d dev_id %d port_id %d for device %s\n",
			 i, priv->dev_port, priv->dp_port_id, dev->name);
		net_start = priv->start;
		net_end = priv->end;
		priv->dp_subif.subif = 0;
		priv->dp_subif.port_id = priv->dp_port_id;

		ret = dp_register_subif(priv->owner, dev,
					dev->name, &priv->dp_subif,
					DP_F_DEREGISTER);

		if (ret != DP_SUCCESS) {
			pr_err("%s: failed to call deregister subif: %s\n",
			       __func__, dev->name);
		}

		if (priv->extra_subif >= 1)
			ltq_eth_dev_dereg_subif(net_start, net_end);

		ret = dp_register_dev_ext(0, priv->owner,
					  priv->dp_port_id, NULL,
					  &dev_data,
					  DP_F_DEREGISTER);
		if (ret != DP_SUCCESS) {
			pr_err("%s: failed to call deregister device: %s\n",
			       __func__, dev->name);
		}
		ret = dp_alloc_port(priv->owner, dev, priv->dev_port,
				    priv->dp_port_id, NULL, DP_F_DEREGISTER);
		if (ret != DP_SUCCESS) {
			pr_err("%s: failed to call dealloc for device : %s\n",
			       __func__, dev->name);
		}

		priv->dp_port_id = DP_FAILURE;
		unregister_netdev(dev);
		free_netdev(dev);
		if (priv->extra_subif >= 1)
			ltq_eth_dev_dereg(net_start, net_end);
	}

	if (xrx500_hw.mii_bus) {
		mdiobus_unregister(xrx500_hw.mii_bus);
		mdiobus_free(xrx500_hw.mii_bus);
	}

	if (xrx500_hw.mii_bus_pae) {
		mdiobus_unregister(xrx500_hw.mii_bus_pae);
		mdiobus_free(xrx500_hw.mii_bus_pae);
	}

	ltq_eth_thermal_exit(&xrx500_hw);

	memset(&xrx500_hw, 0, sizeof(xrx500_hw));
#ifdef CONFIG_USERSPACE_LINK_NOTIFICATION
	ltq_eth_genetlink_exit();
#endif
	pr_info("Lantiq ethernet driver for XRX500 remove.\n");
}

static int ltq_eth_drv_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ltq_net_soc_data *soc_data = NULL;

	soc_data = (struct ltq_net_soc_data *)of_device_get_match_data(dev);
	if (!soc_data) {
		pr_err("No data found for ltq eth drv!\n");
		return -EINVAL;
	}

	if (soc_data->need_defer && !is_xway_gphy_fw_loaded())
		return -EPROBE_DEFER;

	memcpy(&g_soc_data, soc_data, sizeof(*soc_data));
	/* Just do the init */
	ltq_eth_drv_init(pdev);
	register_netdevice_notifier(&netdevice_notifier);

	return 0;
}

static int ltq_eth_drv_remove(struct platform_device *pdev)
{
	/* Just do the exit */
	unregister_netdevice_notifier(&netdevice_notifier);
	ltq_eth_drv_exit(pdev);
	return 0;
}

static const struct ltq_net_soc_data xrx500_net_data = {
	.need_defer = true,
	.hw_checksum = true,
	.queue_num = 1,
	.phy_connect_func = &xrx500_mdio_probe,
};

static const struct ltq_net_soc_data prx300_net_data = {
	.need_defer = false,
	.hw_checksum = false,
	.queue_num = 8,
	.phy_connect_func = &prx300_phy_connect,
};

static const struct of_device_id ltq_eth_drv_match[] = {
	{ .compatible = "lantiq,xrx500-eth", .data = &xrx500_net_data},
	{ .compatible = "lantiq,prx300-eth", .data = &prx300_net_data},
	{},
};
MODULE_DEVICE_TABLE(of, ltq_eth_drv_match);

static struct platform_driver ltq_eth_driver = {
	.probe = ltq_eth_drv_probe,
	.remove = ltq_eth_drv_remove,
	.driver = {
		.name = "xrx500-eth",
		.of_match_table = ltq_eth_drv_match,
		.owner = THIS_MODULE,
	},
};

module_platform_driver(ltq_eth_driver);
MODULE_DESCRIPTION("Intel ethernet driver");
MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRV_MODULE_VERSION);
