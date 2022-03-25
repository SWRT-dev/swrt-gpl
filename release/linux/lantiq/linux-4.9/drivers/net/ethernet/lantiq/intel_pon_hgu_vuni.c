// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2019 Intel Corporation.
 */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/kernel.h> /* printk() */
#include <linux/types.h>  /* size_t */
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/delay.h>
#include <linux/init.h>

#include <linux/of_net.h>

#include <linux/of_device.h>

#include <net/datapath_api.h>
#include "intel_pon_hgu_vuni.h"

#define MAX_SUB_IF 4
#define DRV_MODULE_NAME		"intel_pon_hgu_vuni"

/* length of time before we decide the hardware is borked,
 * and dev->eth_tx_timeout() should be called to fix the problem
 */
#define INTEL_VUNI_TX_TIMEOUT	 (10 * HZ)

/* Init of the network device */
static int vuni_init(struct net_device *dev);
/* Start the network device interface queue */
static int vuni_open(struct net_device *dev);
/* Stop the network device interface queue */
static int vuni_stop(struct net_device *dev);
/* Uninit the network device interface queue */
static void vuni_uninit(struct net_device *dev);
/* Transmit packet from Tx Queue to MAC */
static int vuni_xmit(struct sk_buff *skb, struct net_device *dev);
/*  Set mac address*/
static int vuni_set_mac_address(struct net_device *dev, void *p);
/* Hardware specific IOCTL's  */
static int vuni_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
/* Get the network device statistics */
static
struct rtnl_link_stats64 *vuni_get_stats(struct net_device *dev,
					 struct rtnl_link_stats64 *storage);
/* Transmit timeout*/
static void vuni_tx_timeout(struct net_device *dev);

/**
 *  Datapath directpath functions
 */
static int32_t dp_fp_stop_tx(struct net_device *);
static int32_t dp_fp_restart_tx(struct net_device *);
static int32_t dp_fp_rx(struct net_device *, struct net_device *,
			struct sk_buff *, int32_t);

static struct vuni_hw vuni_hw;

static struct module g_intel_vuni_module[NUM_IF];
static u32 g_rx_csum_offload;
static const struct intel_net_soc_data *g_soc_data;
static struct net_device *eth_dev[NUM_IF][MAX_SUB_IF];

static const struct net_device_ops intel_vuni_drv_ops = {
	.ndo_init		= vuni_init,
	.ndo_open		= vuni_open,
	.ndo_stop		= vuni_stop,
	.ndo_uninit		= vuni_uninit,
	.ndo_start_xmit		= vuni_xmit,
	.ndo_set_mac_address	= vuni_set_mac_address,
	.ndo_get_stats64	= vuni_get_stats,
	.ndo_do_ioctl		= vuni_ioctl,
	.ndo_tx_timeout		= vuni_tx_timeout,
};

/* Set the MAC address */
static int vuni_set_mac_address(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;

	if (netif_running(dev))
		return -EBUSY;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EINVAL;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	return 0;
}

/* open the network device interface*/
static int vuni_open(struct net_device *dev)
{
	int ret;
	/* flag 1 for enable */
	int flags = 1;

	pr_info("%s called for device %s\n", __func__, dev->name);

	ret = dp_rx_enable(dev, dev->name, flags);

	if (ret != DP_SUCCESS) {
		pr_err("%s: failed to enable for device: %s ret %d\n",
		       __func__, dev->name, ret);
		return -1;
	}

	return 0;
}

/* Stop the network device interface*/
static int vuni_stop(struct net_device *dev)
{
	int ret;
	/* flag 0 for disable */
	int flags = 0;

	pr_info("%s called for device %s\n", __func__, dev->name);
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
static void vuni_uninit(struct net_device *dev)
{
	struct intel_vuni_priv *priv;
	struct dp_subif_data data = {0};
	int ret;

	pr_info("%s called for device %s\n", __func__, dev->name);
	priv = netdev_priv(dev);

	if (priv->dp_port_id == DP_FAILURE) {
		pr_info("dp port id (%d) is invalid. ignore the deregister.\n",
			priv->dp_port_id);
		return;
	}

	if (priv->vani == 1) {
		priv->dp_subif.subif = -1;
		priv->dp_subif.port_id = priv->dp_port_id;
		data.flag_ops = DP_SUBIF_VANI;
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
	struct intel_vuni_priv *priv;

	priv = netdev_priv(dev);
	skb->protocol = eth_type_trans(skb, dev);

	if (dev->features & NETIF_F_RXCSUM)
		skb->ip_summed = CHECKSUM_UNNECESSARY;

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
	struct net_device *vani_dev;
	struct intel_vuni_priv *priv;
#ifdef DUMP_PACKET
	if (skb && skb->data) {
		print_hex_dump(KERN_INFO, "",
			       DUMP_PREFIX_ADDRESS, 16, 1,
			       skb->data, len, true);
	}

#endif
#ifndef CONFIG_INTEL_DATAPATH_HAL_GSWIP32
	if (skb) {
		/*Remove PMAC to DMA header */
		len -= DP_MAX_PMAC_LEN;
		skb_pull(skb, DP_MAX_PMAC_LEN);
	} else {
		pr_err("%s: skb from DP is null !\n", __func__);
		goto rx_err_exit;
	}
#endif
	/* Pass it to the stack */
#ifdef DUMP_PACKET

	if (skb && skb->data) {
		print_hex_dump(KERN_INFO, "",
			       DUMP_PREFIX_ADDRESS, 16, 1,
			       skb->data, len, true);
	}

#endif
	/*convert vUNIx_0 to VANIx*/
	if (rxif) {
		eth_rx(rxif, len, skb);
	} else {
		priv = netdev_priv(txif);
		vani_dev = eth_dev[priv->id][0];
		skb->dev = vani_dev;

		if (vani_dev) {
			eth_rx(vani_dev, len, skb);
		} else {
			pr_err("%s: error: rxed a packet from DP lib on interface %x\n",
			       __func__, (unsigned int)rxif);
			goto rx_err_exit;
		}
	}

	return 0;
rx_err_exit:

	if (skb)
		dev_kfree_skb_any(skb);

	return -1;
}

/* Get the network device stats information */
static struct
rtnl_link_stats64 *vuni_get_stats(struct net_device *dev,
				  struct rtnl_link_stats64 *storage)
{
#ifndef CONFIG_INTEL_DATAPATH_MIB
	struct intel_vuni_priv *priv = netdev_priv(dev);
	*storage = priv->stats;
#else
	dp_get_netif_stats(dev, NULL, storage, 0);
#endif
	return storage;
}

/* Trasmit timeout */
static void vuni_tx_timeout(struct net_device *dev)
{
}

static int vuni_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct intel_vuni_priv *priv = netdev_priv(dev);
	int ret;
	int len;
	struct net_device *vuni_dev;

	if (skb_put_padto(skb, ETH_ZLEN)) {
		priv->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	if (strcmp(dev->name, eth_dev[priv->id][0]->name) != 0) {
		/*drop the packet*/
		priv->stats.tx_dropped++;
		kfree_skb(skb);
		return 0;
	}
	/*convert vANIx to vUNIx_0, use the first subif*/
	vuni_dev = eth_dev[priv->id][1];
	skb->dev = vuni_dev;
	priv = netdev_priv(vuni_dev);
	/* Call the Datapath Library's TX function */
	((struct dma_tx_desc_1 *)&skb->DW1)->field.ep = priv->dp_subif.port_id;
	((struct dma_tx_desc_0 *)&skb->DW0)->field.dest_sub_if_id =
		priv->dp_subif.subif;

	len = skb->len;

	if (dev->features & NETIF_F_HW_CSUM) {
		ret = dp_xmit(vuni_dev, &priv->dp_subif, skb, skb->len,
			      DP_TX_CAL_CHKSUM);
	} else {
		ret = dp_xmit(vuni_dev, &priv->dp_subif, skb, skb->len, 0);
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
static int vuni_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	return -EOPNOTSUPP;
}

/* init of the network device */
static int vuni_init(struct net_device *dev)
{
#if 1
	struct intel_vuni_priv *priv;
	struct dp_subif_data data = {0};
	int ret;

	priv = netdev_priv(dev);
	pr_info("probing for number of ports = %d\n", priv->num_port);
	pr_info("%s called for device %s\n", __func__, dev->name);

	if (priv->vani == 1) {
		priv->dp_subif.subif = -1;
		priv->dp_subif.port_id = priv->dp_port_id;
		data.flag_ops = DP_SUBIF_VANI;
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
	pr_info("\n subif reg for owner = %s portid = %d subifid = %d dev= %s\n",
		priv->owner->name, priv->dp_port_id,
		priv->dp_subif.subif, dev->name);
	return 0;
#endif
}

static void
ltq_eth_drv_eth_addr_setup(struct net_device *dev, int port, int vani)
{
	int i = 0;
	u8 values[ETH_ALEN] = {0};

	if (is_valid_ether_addr(dev->dev_addr))
		return;

	/* read MAC address from the MAC table and put them into device */
	if (is_valid_ether_addr(values)) {
		pr_info("using module param for port %d.\n", port);
		for (i = 0; i < ETH_ALEN; i++)
			dev->dev_addr[i] = values[i];
	} else {
		eth_hw_addr_random(dev);
		pr_info("using random mac for port %d.\n", port);
	}
}

static int intel_vuni_dev_reg(struct net_device *dev_0, struct vuni_hw *hw,
			      u32 dp_port, int start, int end)
{
	int i, err;
	struct intel_vuni_priv *priv;
	struct net_device *dev = dev_0;

	pr_info("start %d end %d\n", start, end);

	for (i = start; i <= end; i++) {
		if (i) {
			dev = alloc_etherdev_mq(sizeof(struct intel_vuni_priv),
						g_soc_data->queue_num);
			if (!dev) {
				pr_err("allocation failed for interface %d\n",
				       i);
				return -ENOMEM;
			}
		}
		eth_dev[hw->num_devs][i] = dev;
		priv = netdev_priv(eth_dev[hw->num_devs][i]);
		priv->dp_port_id = dp_port;
		priv->hw = hw;
		priv->id = hw->num_devs;
		priv->owner = &g_intel_vuni_module[hw->num_devs];
		sprintf(priv->owner->name, "module%02d", priv->id);
		if (start == i) {
			priv->vani = 1;
			snprintf(eth_dev[hw->num_devs][i]->name, IFNAMSIZ,
				 "VANI%d", hw->num_devs);
		} else {
			priv->vani = 0;
			snprintf(eth_dev[hw->num_devs][i]->name, IFNAMSIZ,
				 "VUNI%d_%d",  hw->num_devs, i - 1);
		}

		eth_dev[hw->num_devs][i]->netdev_ops = &intel_vuni_drv_ops;
		ltq_eth_drv_eth_addr_setup(eth_dev[hw->num_devs][i],
					   priv->id, priv->vani);
		err = register_netdev(eth_dev[hw->num_devs][i]);
		if (err) {
			pr_err("%s: failed to register netdevice: %p %d, %d %s\n",
			       __func__, eth_dev[hw->num_devs][i], hw->num_devs,
			       err, eth_dev[hw->num_devs][i]->name);
				return -1;
		}
	}
	return 0;
}

static int vuni_dev_dereg_subif(int dev_num, int start, int end)
{
	int res, i;
	struct intel_vuni_priv *priv;
	struct dp_subif_data data = {0};

	for (i = start; i <= end; i++) {
		struct net_device *dev = eth_dev[dev_num][i];

		priv = netdev_priv(dev);
		priv->dp_subif.subif = priv->dp_subif.subif;
		priv->dp_subif.port_id = priv->dp_subif.port_id;
		if (priv->vani == 1) {
			pr_info("owner = %s portid = %d subifid = %d dev= %s\n",
				priv->owner->name, priv->dp_port_id,
				priv->dp_subif.subif, dev->name);
			data.flag_ops = DP_SUBIF_VANI;
			res = dp_register_subif_ext(0, priv->owner,
						    dev, dev->name,
						    &priv->dp_subif,
						    &data, DP_F_DEREGISTER);
		} else {
			pr_info("owner = %s portid = %d subifid = %d dev= %s\n",
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

static int vuni_dev_dereg(int dev_num, int start, int end)
{
	int i;

	for (i = start; i <= end; i++) {
		struct intel_vuni_priv *priv;
		struct net_device *dev = eth_dev[dev_num][i];

		netif_stop_queue(dev);
		priv = netdev_priv(dev);
		priv->dp_port_id = DP_FAILURE;
		unregister_netdev(dev);
		free_netdev(dev);
	}
	return 0;
}

static int vuni_of_iface(struct vuni_hw *hw, struct device_node *iface,
			 struct platform_device *pdev)
{
	struct intel_vuni_priv *priv;
	struct dp_dev_data dev_data = {0};
	dp_pmac_cfg_t pmac_cfg;
	u32 extra_subif_param = 0;
	int net_start = 0, net_end = 0;
	dp_cb_t cb = {0};
	u32 dp_port_id = 0;
	char name[16];
	int ret;
	struct net_device *dev;

	/* alloc the network device */
	dev = alloc_etherdev_mq(sizeof(struct intel_vuni_priv),
				g_soc_data->queue_num);

	if (!dev) {
		pr_err("allocation failed for interface %d\n",
		       hw->num_devs);
		return -ENOMEM;
	}

	hw->devs[hw->num_devs] = dev;
	priv = netdev_priv(hw->devs[hw->num_devs]);

	/* setup the network device */
	snprintf(name, sizeof(name), "vANI%d", hw->num_devs);

	ret = of_property_read_u32(iface, "intel,extra-subif",
				   &extra_subif_param);
	if (ret < 0) {
		pr_info("Property intel,extra-subif not exist for if %s %d\n",
			name, extra_subif_param);
		priv->extra_subif = 0;
	} else {
		pr_info("Property intel,extra-subif for if %s %d\n",
			name, extra_subif_param);
		priv->extra_subif = extra_subif_param;
	}
	priv->start = 0;
	priv->end = priv->extra_subif + 1;
	if (priv->end >= MAX_SUB_IF) {
		pr_err("Vuni subif exceeds the max\n");
		priv->end = MAX_SUB_IF - 1;
	}

	strcpy(hw->devs[hw->num_devs]->name, name);
	hw->devs[hw->num_devs]->netdev_ops = &intel_vuni_drv_ops;
	hw->devs[hw->num_devs]->watchdog_timeo = INTEL_VUNI_TX_TIMEOUT;
	hw->devs[hw->num_devs]->needed_headroom = DP_MAX_PMAC_LEN;
	SET_NETDEV_DEV(hw->devs[hw->num_devs], &pdev->dev);

	/* setup our private data */
	priv->hw = hw;
	priv->id = hw->num_devs;
	spin_lock_init(&priv->lock);

	priv->owner = &g_intel_vuni_module[hw->num_devs];
	sprintf(priv->owner->name, "module%02d", priv->id);

	memset(&pmac_cfg, 0, sizeof(dp_pmac_cfg_t));
	pmac_cfg.eg_pmac_flags = EG_PMAC_F_PMAC;
	pmac_cfg.eg_pmac.pmac = 1;

	dp_port_id  = dp_alloc_port_ext(0, priv->owner, hw->devs[hw->num_devs],
					0, 0, &pmac_cfg, NULL, DP_F_VUNI);

	if (dp_port_id == DP_FAILURE) {
		pr_err("dp_alloc_port failed for %s with port_id %d\n",
		       hw->devs[hw->num_devs]->name, priv->id + 1);
		return -ENODEV;
	}

	hw->devs[hw->num_devs]->dev_id = dp_port_id;
	priv->dp_port_id = dp_port_id;
	dev_data.max_ctp = 4;
	cb.stop_fn = dp_fp_stop_tx;
	cb.restart_fn  = (dp_restart_tx_fn_t)dp_fp_restart_tx;
	cb.rx_fn = (dp_rx_fn_t)dp_fp_rx;

	if (dp_register_dev_ext(0, priv->owner,
				dp_port_id, &cb,
				&dev_data, 0) != DP_SUCCESS) {
		pr_err("dp_register_dev failed for %s\n and port_id %d",
		       hw->devs[hw->num_devs]->name, dp_port_id);
		dp_alloc_port_ext(0, priv->owner, hw->devs[hw->num_devs],
				  dp_port_id, dp_port_id,
				  NULL, NULL, DP_F_DEREGISTER);
		return -ENODEV;
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
	if (g_soc_data->hw_checksum) {
		hw->devs[hw->num_devs]->features |= NETIF_F_HW_CSUM;
		hw->devs[hw->num_devs]->hw_features |= NETIF_F_HW_CSUM;
		pr_info("%s: hw csum offload is enabled!\n", __func__);
	} else {
		pr_info("%s: hw csum offload is disable!\n", __func__);
	}
#endif

	if (g_rx_csum_offload) {
		pr_info("%s: rx csum offload is enabled !\n", __func__);
		hw->devs[hw->num_devs]->features |= NETIF_F_RXCSUM;
		hw->devs[hw->num_devs]->hw_features |= NETIF_F_RXCSUM;
	} else {
		pr_info("%s: rx csum offload is disabled !\n", __func__);
	}

	net_start = priv->start;
	net_end = priv->end;
	intel_vuni_dev_reg(dev, &vuni_hw, dp_port_id, net_start, net_end);
	hw->num_devs++;
	return 0;
}

/* Initialization Ethernet module */
static int intel_vuni_drv_init(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	struct device_node *iface_np;

	memset(g_intel_vuni_module, 0, sizeof(g_intel_vuni_module));

	/* load the interfaces */
	/* add a dummy interface */
	vuni_hw.num_devs = 0;
	for_each_available_child_of_node(node, iface_np) {
		if (vuni_hw.num_devs < NUM_IF) {
			if (vuni_of_iface(&vuni_hw, iface_np, pdev))
				return -EINVAL;
		} else {
			dev_err(&pdev->dev,
				"only %d interfaces allowed\n",
				NUM_IF);
				return -EINVAL;
		}
	}

	if (!vuni_hw.num_devs) {
		dev_dbg(&pdev->dev, "failed to load interfaces\n");
		return -ENOENT;
	}

	pr_info("Intel vUNI driver init.\n");
	return 0;
}

static void intel_vuni_drv_exit(struct platform_device *pdev)
{
	int i, ret;

	for (i = 0; i < vuni_hw.num_devs; i++) {
		struct intel_vuni_priv *priv;
		struct dp_dev_data dev_data = {0};
		struct net_device *dev = vuni_hw.devs[i];

		if (!dev)
			continue;

		netif_stop_queue(dev);

		priv = netdev_priv(dev);
		vuni_dev_dereg_subif(i, priv->start, priv->end);
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
			/* Deregister netdevices */
		vuni_dev_dereg(i, priv->start, priv->end);
	}
	memset(&vuni_hw, 0, sizeof(vuni_hw));
	pr_info("Intel VUNI driver for remove.\n");
}

static int intel_vuni_drv_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	g_soc_data = of_device_get_match_data(dev);
	if (!g_soc_data) {
		pr_err("No data found for ltq eth drv!\n");
		return -EINVAL;
	}

	/* Just do the init */
	return intel_vuni_drv_init(pdev);
}

static int intel_vuni_drv_remove(struct platform_device *pdev)
{
	/* Just do the exit */
	intel_vuni_drv_exit(pdev);
	return 0;
}

static const struct intel_net_soc_data prx300_net_data = {
	.need_defer = false,
	.hw_checksum = false,
	.queue_num = 8,
};

static const struct of_device_id intel_vuni_drv_match[] = {
	{ .compatible = "intel,hgu-vuni", .data = &prx300_net_data},
	{},
};

MODULE_DEVICE_TABLE(of, intel_vuni_drv_match);

static struct platform_driver intel_vuni_driver = {
	.probe = intel_vuni_drv_probe,
	.remove = intel_vuni_drv_remove,
	.driver = {
		.name = "intel,hgu-vuni",
		.of_match_table = intel_vuni_drv_match,
		.owner = THIS_MODULE,
	},
};

module_platform_driver(intel_vuni_driver);

MODULE_DESCRIPTION("Intel PON HGU vUNI driver");
MODULE_LICENSE("GPL v2");
