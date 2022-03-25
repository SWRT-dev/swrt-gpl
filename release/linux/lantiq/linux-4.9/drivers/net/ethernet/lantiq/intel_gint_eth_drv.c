/*
 * Copyright (C) Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/kernel.h> /* printk() */
#include <linux/types.h>  /* size_t */
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/init.h>
#include <linux/of_net.h>
#include <linux/of_device.h>

#include <net/datapath_api.h>
#include <net/switch_api/lantiq_gsw_api.h>
#include <net/switch_api/gsw_dev.h>
#include <net/switch_api/mac_ops.h>
#include "intel_gint_eth_drv.h"

static struct net_device *gint_eth_dev[NUM_GINT_INF];
static struct module gint_eth_module[NUM_ETH_INF];
static char wan_iface[IFNAMSIZ] = "eth1";
static struct gint_hw gint_hw;
static u32 encap_mode;

/* Init of the network device */
static int gint_eth_init(struct net_device *dev);
/* Uninit the network device interface queue */
static void gint_eth_uninit(struct net_device *dev);
/* Start the network device interface queue */
static int gint_eth_open(struct net_device *dev);
/* Stop the network device interface queue */
static int gint_eth_stop(struct net_device *dev);
/* Transmit packet from Tx Queue to MAC */
static int gint_start_xmit(struct sk_buff *skb, struct net_device *dev);
/* transmit timeout */
static void gint_tx_timeout(struct net_device *);
/*  Set mac address*/
static int gint_set_mac_address(struct net_device *dev, void *p);

/**
 *  Datapath directpath functions
 **/
static int32_t dp_fp_stop_tx(struct net_device *);
static int32_t dp_fp_restart_tx(struct net_device *);
static int32_t dp_fp_rx(struct net_device *, struct net_device *,
			struct sk_buff *, int32_t);

static const struct net_device_ops gint_eth_drv_ops = {
	.ndo_open = gint_eth_open,
	.ndo_stop = gint_eth_stop,
	.ndo_start_xmit = gint_start_xmit,
	.ndo_tx_timeout = gint_tx_timeout,
	.ndo_set_mac_address = gint_set_mac_address,
	.ndo_init		= gint_eth_init,
	.ndo_uninit		= gint_eth_uninit,
};

/* open the network device interface*/
static int gint_eth_open(struct net_device *dev)
{
	netif_start_queue(dev);
	return 0;
}

/* Stop the network device interface*/
static int gint_eth_stop(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}

/* Trasmit timeout */
static void gint_tx_timeout(struct net_device *dev)
{
	netif_wake_queue(dev);
}

/*  Set mac address*/
static int gint_set_mac_address(struct net_device *dev,
				void *mac_addr)
{
	struct sockaddr *addr = mac_addr;

	if (netif_running(dev))
		return -EBUSY;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EINVAL;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	return 0;
}

/* Send the packet to netwrok rx queue */
static void eth_rx(struct net_device *dev, int len, struct sk_buff *skb)
{
	struct gint_eth_priv *priv;

	priv = netdev_priv(dev);
	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);
	netdev_info(dev, "passing to stack: protocol: %x\n", skb->protocol);
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
	/*Remove MAC header */
	skb_pull(skb, 8);

	if (rxif) {
		netdev_info(rxif, "%s: rxed a packet from DP lib on interface\n",
			    __func__);
		eth_rx(rxif, len, skb);
	}

	return 0;
}

static int gint_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct gint_eth_priv *priv = netdev_priv(dev);
	int ret;
	int len;

	/* Call the Datapath Library's TX function */
	((struct dma_tx_desc_1 *)&skb->DW1)->field.ep = priv->dp_subif.port_id;
	((struct dma_tx_desc_0 *)&skb->DW0)->field.dest_sub_if_id =
		priv->dp_subif.subif;
	len = skb->len;
	ret = dp_xmit(dev, &priv->dp_subif, skb, skb->len, 0);
	if (!ret) {
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += len;
	} else {
		priv->stats.tx_dropped++;
	}
	return 0;
}


/* init of the network device */
static int gint_eth_init(struct net_device *dev)
{
	struct gint_eth_priv *priv;
	int ret;

	priv = netdev_priv(dev);
	netdev_dbg(dev, "%s called for device\n", __func__);
	priv->dp_subif.subif = priv->subif;
	priv->dp_subif.port_id = priv->dp_port_id;
	ret = dp_register_subif_ext(0, priv->owner,
				    dev, dev->name, &priv->dp_subif,
				    NULL, 0);
	if (ret != DP_SUCCESS) {
		pr_err("%s: failed to open for device: %s ret %d\n",
		       __func__, dev->name, ret);
		return -1;
	}
	pr_debug("\n subif reg for owner = %s portid = %d subifid = %d dev= %s\n",
		 priv->owner->name, priv->dp_port_id,
		 priv->dp_subif.subif, dev->name);

	return 0;
}

/* Uninit the network device interface*/
static void gint_eth_uninit(struct net_device *dev)
{
	struct gint_eth_priv *priv;
	int ret;

	pr_debug("%s called for device %s\n", __func__, dev->name);
	priv = netdev_priv(dev);

	if (priv->dp_port_id == DP_FAILURE) {
		pr_debug("dp port id (%d) is invalid. ignore the deregister.\n",
			 priv->dp_port_id);
		return;
	}
		priv->dp_subif.subif = priv->subif;
		priv->dp_subif.port_id = priv->dp_port_id;
		ret = dp_register_subif_ext(0, priv->owner,
					    dev, dev->name,
					    &priv->dp_subif,
					    NULL, DP_F_DEREGISTER);
	if (ret != DP_SUCCESS)
		pr_err("%s: failed to close for device: %s ret %d\n",
		       __func__, dev->name, ret);
}

static int enable_gint_xgmac(u32 xgmac_id, u32 port_id,
				struct device_node *iface)
{
	GSW_CPU_PortCfg_t cpu_port_cfg;
	GSW_return_t ret;
	struct mac_ops *lan_mac_ops = gsw_get_mac_ops(0, xgmac_id);
	struct core_ops *core_ops = gsw_get_swcore_ops(0);
	struct adap_ops *adap_ops = gsw_get_adap_ops(0);
	u32 gint_hd = 0, gint_hd_type = 0, rd_val = 0;

	ret = of_property_read_u32(iface, "intel,gint-hd",
				   &gint_hd);
	if (ret < 0) {
		pr_info("ERROR : Property intel,gint-hd not read from DT for if %d\n",
			port_id);

		return ret;
	}

	ret = of_property_read_u32(iface, "intel,gint-hd-type",
				   &gint_hd_type);
	if (ret < 0) {
		pr_info("ERROR : Property intel,gint-hd-type not read from DT for if %d\n",
			port_id);

		return ret;
	}

	/* For LAN RX */
		cpu_port_cfg.nPortId = port_id;
		cpu_port_cfg.bSpecialTagIngress = 1;
		cpu_port_cfg.bSpecialTagEgress = 1;
		ret = core_ops->gsw_common_ops.CPU_PortCfgSet(core_ops,
							     &cpu_port_cfg);
	if (ret != GSW_statusOk) {
		pr_err("Fail in configuring CPU port\n");
		return -1;
	}
	/*TODO: Replace register modification by proper API */
	lan_mac_ops->mac_op_cfg(lan_mac_ops, TX_SPTAG_REMOVE);
	lan_mac_ops->mac_op_cfg(lan_mac_ops, RX_SPTAG_INSERT);

	if (port_id == 3) {
		rd_val |= 1 << 28;
		/*TODO: Replace register modification by proper API */
		lan_mac_ops->xgmac_reg_wr(lan_mac_ops, 0, rd_val);

		/* Encapsulation mode, Enbaling G.INT and Length Mode */
		if (encap_mode == 1) {
			rd_val |= 7 << 0;
			/*TODO: Replace register modification by proper API */
			adap_ops->ss_rg_wr(lan_mac_ops, MAC3_GINT_CFG, rd_val);
		} else {
			rd_val |= 5 << 0;
			/*TODO: Replace register modification by proper API */
			adap_ops->ss_rg_wr(lan_mac_ops, MAC3_GINT_CFG, rd_val);
		}
		/*TODO: Replace register modification by proper API */
		/* 14 bytes G.INT Header Configuration, it cab be anything */
		adap_ops->ss_rg_wr(lan_mac_ops, MAC3_GINT_HD0_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC3_GINT_HD1_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC3_GINT_HD2_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC3_GINT_HD3_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC3_GINT_HD4_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC3_GINT_HD5_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC3_GINT_HD6_CFG,
							gint_hd_type);
	}

	if (port_id == 4) {
		rd_val = 0;
		rd_val |= 1 << 28;
		/*TODO: Replace register modification by proper API */
		lan_mac_ops->xgmac_reg_wr(lan_mac_ops, 0, rd_val);

		/* Encapsulation mode, Enbaling G.INT and Length Mode */
		if (encap_mode == 1) {
			rd_val |= 7 << 0;
			/*TODO: Replace register modification by proper API */
			adap_ops->ss_rg_wr(lan_mac_ops, MAC4_GINT_CFG, rd_val);
		} else {
			rd_val |= 5 << 0;
			/*TODO: Replace register modification by proper API */
			adap_ops->ss_rg_wr(lan_mac_ops, MAC4_GINT_CFG, rd_val);
		}
		/*TODO: Replace register modification by proper API */
		/* 14 bytes G.INT Header Configuration, it cab be anything */
		adap_ops->ss_rg_wr(lan_mac_ops, MAC4_GINT_HD0_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC4_GINT_HD1_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC4_GINT_HD2_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC4_GINT_HD3_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC4_GINT_HD4_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC4_GINT_HD5_CFG, gint_hd);
		adap_ops->ss_rg_wr(lan_mac_ops, MAC4_GINT_HD6_CFG,
							gint_hd_type);
	}

	return 0;
}

static int  gint_eth_drv_reg(struct gint_hw *hw, u32 dp_port_id_param,
			     u32 xgmac_id_param, u32 subif_start,
			     u32 subif_end, struct device_node *iface)
{
	int i, err;

	for (i = subif_start; i <= subif_end; i++) {
		char name[16];
		struct gint_eth_priv *priv;

		gint_eth_dev[i] = alloc_etherdev_mq
					(sizeof(struct gint_eth_priv), 8);
		if (!gint_eth_dev[i]) {
			pr_debug("allocated failed for interface %d\n", i);
			return -ENOMEM;
		}
		priv = netdev_priv(gint_eth_dev[i]);
		priv->dp_port_id = dp_port_id_param;

		/* If WAN interface subif value is 0 */
		if (hw->wan)
			priv->subif = 0;
		else
			priv->subif = i;
		priv->xgmac_id = xgmac_id_param;
		priv->hw = hw;
		priv->id = hw->num_devs;
		priv->owner = &gint_eth_module[hw->num_devs];
		sprintf(priv->owner->name, "mod%02d", priv->id);
		/* setup the network device */
		if (hw->wan)
			snprintf(name, sizeof(name), wan_iface);
		else
			sprintf(name, "gint_eth%d", i);
		strcpy(gint_eth_dev[i]->name, name);
		if (is_zero_ether_addr(gint_eth_dev[i]->dev_addr))
			eth_hw_addr_random(gint_eth_dev[i]);
		gint_eth_dev[i]->netdev_ops = &gint_eth_drv_ops;
		spin_lock_init(&priv->lock);
		err = register_netdev(gint_eth_dev[i]);
		if (err != 0) {
			pr_err("%s: failed to register netdevice: %d\n",
			       __func__, err);
		free_netdev(gint_eth_dev[i]);
		gint_eth_dev[i] = NULL;
		return -1;
		}
	}
		/* If WAN interface no need enable G.INT in XGMAC */
	if (hw->wan)
		return 0;
	/* Enable G.INT in XGMAC */
	enable_gint_xgmac(xgmac_id_param, dp_port_id_param, iface);
	return 0;
}

static int gint_of_iface(struct gint_hw *hw, struct device_node *iface,
			 struct platform_device *pdev)

{
	struct gint_eth_priv *priv;
	struct device_node *mac_np;
	struct dp_port_data port_data = {0};
	struct dp_dev_data dev_data = {0};
	dp_cb_t cb = {0};
	u32 dp_dev_port_param, dp_port_id_param, xgmac_id_param;
	u32 subif_start = 0, subif_end = 0;
	const __be32 *wan;
	u32 dp_port_id = 0;
	int ret;

	/* alloc the network device */
	hw->devs[hw->num_devs] = alloc_etherdev_mq(sizeof(struct gint_eth_priv),
						   8);
	if (!hw->devs[hw->num_devs]) {
		pr_debug("allocated failed for interface %d\n",
			 hw->num_devs);
		return -ENOMEM;
	}

	priv = netdev_priv(hw->devs[hw->num_devs]);

	ret = of_property_read_u32(iface, "intel,dp-dev-port",
				   &dp_dev_port_param);
	if (ret < 0) {
		pr_info("ERROR : Property intel,dp-dev-port not read from DT for if %d\n",
			dp_dev_port_param);
		return ret;
	}

	ret = of_property_read_u32(iface, "intel,dp-port-id",
				   &dp_port_id_param);
	if (ret < 0) {
		pr_info("ERROR : Property intel,dp-port-id not read from DT for if %d\n",
			dp_dev_port_param);
		return ret;
	}

	priv->xgmac_id = -1;

	mac_np = of_parse_phandle(iface, "mac", 0);
	if (mac_np) {
		ret = of_property_read_u32(mac_np, "mac_idx", &xgmac_id_param);
		if (ret < 0) {
			pr_info("ERROR : Property mac_idx not read from DT for if %d\n",
				dp_dev_port_param);
			return ret;
		}

		priv->xgmac_id = xgmac_id_param;
	}

	ret = of_property_read_u32(iface, "intel,subif-start",
				   &subif_start);
	if (ret < 0) {
		pr_info("ERROR : Property intel,subif-start not read from DT for if %d\n",
			dp_dev_port_param);
	}

	priv->subif_min = subif_start;
	ret = of_property_read_u32(iface, "intel,subif-end",
				   &subif_end);
	if (ret < 0) {
		pr_info("ERROR : Property intel,subif-end not read from DT for if %d\n",
			dp_dev_port_param);
	}

	priv->subif_max = subif_end;

	/* setup our private data */
	priv->hw = hw;
	priv->id = hw->num_devs;
	spin_lock_init(&priv->lock);

	priv->owner = &gint_eth_module[hw->num_devs];
	sprintf(priv->owner->name, "mod%02d", priv->id);

	/* is this the wan interface ?, these values are fixed */
	wan = of_get_property(iface, "intel,wan", NULL);
	if (wan && (*wan == 1)) {
		hw->wan = 1;
		subif_start = 16;
		subif_end = 16;
		priv->subif_min = subif_start;
		priv->subif_max = subif_end;
	} else {
			hw->wan = 0;
	}

	/* If max subif is zero no need register */
	if (!subif_end)
		return 0;

	if (hw->wan)
		dp_port_id  = dp_alloc_port_ext(0, priv->owner,
						hw->devs[hw->num_devs],
						dp_dev_port_param,
						dp_port_id_param, NULL,
						NULL, DP_F_FAST_ETH_WAN);
	else

		dp_port_id  = dp_alloc_port_ext(0, priv->owner,
						hw->devs[hw->num_devs],
						dp_dev_port_param,
						dp_port_id_param, NULL,
						&port_data, DP_F_GINT);

	if (dp_port_id == DP_FAILURE) {
		pr_err("dp_alloc_port failed for %s with port_id %d\n",
		       hw->devs[hw->num_devs]->name, priv->id + 1);
		return -ENODEV;
	}

	priv->dp_port_id = dp_port_id;
	cb.stop_fn = (dp_stop_tx_fn_t)dp_fp_stop_tx;
	cb.restart_fn  = (dp_restart_tx_fn_t)dp_fp_restart_tx;
	cb.rx_fn = (dp_rx_fn_t)dp_fp_rx;

	if (dp_register_dev_ext(0, priv->owner,
				dp_port_id, &cb,
				&dev_data, 0) != DP_SUCCESS) {
		pr_err("dp_register_dev failed for %s\n and port_id %d",
		       hw->devs[hw->num_devs]->name, dp_port_id);
		dp_alloc_port_ext(0, priv->owner, hw->devs[hw->num_devs],
				  dp_dev_port_param, dp_port_id_param,
				  NULL, NULL, DP_F_DEREGISTER);
		return -ENODEV;
	}

	gint_eth_drv_reg(&gint_hw, dp_port_id_param, xgmac_id_param,
			 subif_start, subif_end, iface);

	hw->num_devs++;
	return 0;
}

/* Initialization Ethernet module */
static int gint_eth_drv_init(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	struct device_node *iface_np;
	int ret;

	ret = of_property_read_u32(node, "intel,encap-mode",
				   &encap_mode);
	if (ret < 0) {
		pr_info("couldn't get the eth encapsulation mode from DT.");
		encap_mode = 0;
		ret = 0;
	}
	gint_hw.num_devs = 0;
	for_each_available_child_of_node(node, iface_np) {
		if (of_device_is_compatible(iface_np, "intel,pdi")) {
			pr_debug("adding the interface: %d\n",
				 gint_hw.num_devs);

			if (!of_device_is_available(iface_np)) {
				pr_debug("device not available.\n");
				continue;
			}
			if (gint_hw.num_devs < NUM_ETH_INF) {
				gint_of_iface(&gint_hw, iface_np, pdev);
			} else {
				dev_err(&pdev->dev,
					"only %d interfaces allowed\n",
					NUM_ETH_INF);
			}
		}
	}

	if (!gint_hw.num_devs) {
		dev_err(&pdev->dev, "failed to load interfaces\n");
		return -ENOENT;
	}

	pr_info("Intel G.INT ethernet driver init\n");

	return 0;
}

static int gint_eth_dev_dereg(int start, int end)
{
	int i;

	for (i = start; i <= end; i++) {
		struct gint_eth_priv *priv;
		struct net_device *dev = gint_eth_dev[i];

		netif_stop_queue(dev);
		priv = netdev_priv(dev);
		priv->dp_port_id = DP_FAILURE;
		unregister_netdev(dev);
		free_netdev(dev);
	}
	return 0;
}

static int gint_eth_dev_dereg_subif(int subif_min, int subif_max)
{
	int res, i;
	struct gint_eth_priv *priv;

	for (i = subif_min; i <= subif_max; i++) {
		struct net_device *dev = gint_eth_dev[i];

		priv = netdev_priv(dev);
		priv->dp_subif.subif = priv->dp_subif.subif;
		priv->dp_subif.port_id = priv->dp_subif.port_id;

		res = dp_register_subif_ext(0, priv->owner,
					    dev, dev->name,
					    &priv->dp_subif,
					    NULL, DP_F_DEREGISTER);
		if (res != DP_SUCCESS) {
			pr_err("failed to deregister subif for device\n");
			return -1;
		}
		}
	return 0;
}

static void gint_eth_drv_exit(struct platform_device *pdev)
{
	int i, ret;

	for (i = 0; i < gint_hw.num_devs; i++) {
		int subif_min = 0, subif_max = 0;
		struct gint_eth_priv *priv;
		struct dp_dev_data dev_data = {0};
		struct net_device *dev = gint_hw.devs[i];

		netif_stop_queue(dev);
		priv = netdev_priv(dev);
		subif_min = priv->subif_min;
		subif_max = priv->subif_max;

		/* Deregister the subif */
		gint_eth_dev_dereg_subif(subif_min, subif_max);

		ret = dp_register_dev_ext(0, priv->owner,
					  priv->dp_port_id, NULL,
					  &dev_data,
					  DP_F_DEREGISTER);
		if (ret != DP_SUCCESS) {
			pr_err("%s: failed to call deregister dp_port_id : %d\n",
			       __func__, priv->dp_port_id);
		}
		ret = dp_alloc_port_ext(0, priv->owner, NULL, 0,
					priv->dp_port_id, NULL,
					NULL, DP_F_DEREGISTER);
		if (ret != DP_SUCCESS) {
			pr_err("%s: failed to call dealloc for dp_port_id : %d\n",
			       __func__, priv->dp_port_id);
		}

		priv->dp_port_id = DP_FAILURE;
		free_netdev(dev);
		/* Deregister netdevices */
		gint_eth_dev_dereg(subif_min, subif_max);
	}

	pr_info("Intel G.INT ethernet driver Exit\n");
}

static int gint_eth_drv_probe(struct platform_device *pdev)
{
	/* Just do the init */
	gint_eth_drv_init(pdev);

	return 0;
}

static int gint_eth_drv_remove(struct platform_device *pdev)
{
	/* Just do the exit */
	gint_eth_drv_exit(pdev);

	return 0;
}

static const struct of_device_id gint_eth_drv_match[] = {
	{ .compatible = "intel,gint-eth" },
	{},
};
MODULE_DEVICE_TABLE(of, gint_eth_drv_match);

static struct platform_driver gint_eth_driver = {
	.probe = gint_eth_drv_probe,
	.remove = gint_eth_drv_remove,
	.driver = {
		.name = "gint-eth",
		.of_match_table = gint_eth_drv_match,
		.owner = THIS_MODULE,
	},
};

module_platform_driver(gint_eth_driver);

MODULE_DESCRIPTION("Intel G.INT Ethernet driver");
MODULE_LICENSE("GPL v2");
