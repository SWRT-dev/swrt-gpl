// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2020 Intel Corporation. */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>

#include <net/datapath_api.h>

static struct net_device *rndev;

struct dp_eth_reinsert_priv {
	struct net_device_stats stats;
	int status;
	struct net_device *dev;
};

int dp_eth_reinsert_open(struct net_device *dev)
{
	netif_start_queue(dev);
	return 0;
}

int dp_eth_reinsert_release(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}

int dp_eth_reinsert_config(struct net_device *dev, struct ifmap *map)
{
	return 0;
}

static bool dp_eth_reinsert_skb_mark_ok(struct sk_buff *skb)
{
	if (!skb || !skb->sk || !skb->sk->sk_socket)
		return false;

	if (skb->sk->sk_socket->type == SOCK_RAW && skb->mark > 0)
		return true;

	return false;
}

int dp_eth_reinsert_tx(struct sk_buff *skb, struct net_device *dev)
{
	struct dma_tx_desc_0 *dw0 = (struct dma_tx_desc_0 *)&skb->DW0;
	struct dma_tx_desc_1 *dw1 = (struct dma_tx_desc_1 *)&skb->DW1;
	struct net_device *rdev = NULL;
	dp_subif_t subif = {0};
	int ret = -EINVAL;

	do {
		/* ifindex for reinsertion passed using the SO_MARK */
		if (!dp_eth_reinsert_skb_mark_ok(skb))
			break;

		rdev = dev_get_by_index(&init_net, skb->mark);
		if (!rdev)
			break;

		ret = dp_get_netif_subifid(rdev, NULL, NULL, NULL, &subif, 0);
		if (ret != DP_SUCCESS) {
			netdev_err(skb->dev, "%s: err %d for %s\n",
				   __func__, ret, rdev->name);
			break;
		}

		dw1->field.ep = subif.port_id;
		dw0->field.dest_sub_if_id = subif.subif;
		netdev_dbg(skb->dev, "%s: ifi: %u if: %s EP: %u subif: %u\n",
			   __func__, rdev->ifindex, rdev->name,
			   subif.port_id, subif.subif);
		ret = dp_xmit(rdev, &subif, skb, skb->len, DP_TX_INSERT);
		if (ret)
			netdev_err(skb->dev, "%s: insert fail\n", __func__);

		/* dp_xmit always frees the skb */
		ret = 0;
	} while (0);

	if (rdev)
		dev_put(rdev);

	if (ret)
		dev_kfree_skb(skb);

	return NETDEV_TX_OK;
}

static const struct net_device_ops dp_eth_reinsert_netdev_ops = {
	.ndo_open            = dp_eth_reinsert_open,
	.ndo_stop            = dp_eth_reinsert_release,
	.ndo_start_xmit      = dp_eth_reinsert_tx,
	.ndo_set_config      = dp_eth_reinsert_config,
};

void dp_eth_reinsert_init(struct net_device *dev)
{
	struct dp_eth_reinsert_priv *priv;

	ether_setup(dev); /* assign some of the fields */
	dev->netdev_ops = &dp_eth_reinsert_netdev_ops;
	dev->flags |= IFF_NOARP;
	dev->features |= NETIF_F_HW_CSUM;
	dev->type = ARPHRD_VOID;

	priv = netdev_priv(dev);

	memset(priv, 0, sizeof(struct dp_eth_reinsert_priv));
}

void dp_eth_reinsert_cleanup(void)
{
	if (rndev) {
		unregister_netdev(rndev);
		free_netdev(rndev);
	}
}

int dp_eth_reinsert_init_module(void)
{
	int ret = -ENOMEM;

	rndev = alloc_netdev(sizeof(struct dp_eth_reinsert_priv),
			     "ins%d", NET_NAME_UNKNOWN,
			     dp_eth_reinsert_init);
	if (!rndev)
		return ret;

	ret = register_netdev(rndev);

	if (ret)
		dp_eth_reinsert_cleanup();

	return ret;
}

module_init(dp_eth_reinsert_init_module);
module_exit(dp_eth_reinsert_cleanup);
