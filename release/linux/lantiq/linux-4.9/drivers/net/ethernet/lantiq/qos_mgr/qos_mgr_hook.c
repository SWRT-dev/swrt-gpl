/******************************************************************************
 **
 ** FILE NAME   : qos_mgr_hook.c
 ** PROJECT   : UGW
 ** MODULES   : QoS Manager Hooks
 **
 ** DATE   : 2 Jan 2020
 ** AUTHOR   : Mohammed Aarif
 ** DESCRIPTION : QoS Manager Hook Pointers
 ** COPYRIGHT  : Copyright (c) 2019 - 2020 Intel Corporation
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date $Author $Comment
 ******************************************************************************/

/*
 * Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>
#include <asm/atomic.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/atmdev.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/protocol.h>
#include <net/xfrm.h>
#include <linux/once.h>
#include <linux/jhash.h>
#include <linux/skbuff.h>
#include <net/netns/hash.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/netfilter_bridge.h>
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nfnetlink_conntrack.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_conntrack_acct.h>
/*
 * Chip Specific Head File
 */
#include <net/qos_mgr/qos_mgr_common.h>

int32_t (*qos_mgr_hook_setup_tc)(struct net_device *dev, u32 handle,
		__be16 protocol, struct tc_to_netdev *tc) = NULL;
EXPORT_SYMBOL(qos_mgr_hook_setup_tc);

int32_t (*qos_mgr_hook_setup_tc_ext)(struct net_device *dev, u32 handle,
		__be16 protocol, struct tc_to_netdev *tc, int32_t deq_idx,
		int32_t port_id) = NULL;
EXPORT_SYMBOL(qos_mgr_hook_setup_tc_ext);

int32_t (*qos_mgr_br2684_get_vcc_fn)(struct net_device *netdev,
		struct atm_vcc **pvcc) = NULL;
EXPORT_SYMBOL(qos_mgr_br2684_get_vcc_fn);
