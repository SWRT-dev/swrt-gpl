// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

#include <linux/types.h>
#include <net/datapath_api.h>
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
#include <net/intel_np_lro.h>
#endif
#include <net/pon_qos_tc.h>
#include "datapath.h"
#include "datapath_instance.h"
#include "datapath_swdev_api.h"
#include "datapath_ioctl.h"

#if IS_ENABLED(CONFIG_INTEL_VPN)
#include <net/datapath_api_vpn.h>
#include <net/xfrm.h>
#endif

int dp_cap_num;
struct dp_hw_cap hw_cap_list[DP_MAX_HW_CAP];

/*Subif DEV hash list */
struct hlist_head dp_dev_list[DP_DEV_HASH_SIZE];

/*Module hash list */
struct hlist_head dp_mod_list[DP_MOD_HASH_SIZE];

static struct kmem_cache *cache_dev_list;
static struct kmem_cache *cache_mod_list;

struct hlist_head *get_dp_dev_list(void)
{
	return dp_dev_list;
}
EXPORT_SYMBOL(get_dp_dev_list);

int register_dp_hw_cap(struct dp_hw_cap *info, u32 flag)
{
	int i;

	if (!info) {
		pr_err("%s: NULL info\n", __func__);
		return -1;
	}
	for (i = 0; i < DP_MAX_HW_CAP; i++) {
		if (hw_cap_list[i].valid)
			continue;
		hw_cap_list[i].valid = 1;
		hw_cap_list[i].info = info->info;
		dp_cap_num++;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
		pr_err("Succeed to %s HAL[%d]: type=%d ver=%d dp_cap_num=%d\n",
		       "Register",
		       i,
		       info->info.type,
		       info->info.ver,
		       dp_cap_num);
#endif
		return 0;
	}
	pr_err("Failed to %s HAL: type=%d ver=%d\n",
	       "Register",
	       info->info.type,
	       info->info.ver);
	return -1;
}

/*return value:
 *succeed: return 0 with info->inst updated
 *fail: -1
 */
int dp_request_inst(struct dp_inst_info *info, u32 flag)
{
	int i, k, j;
	struct inst_property *dp_prop;

	if (!info)
		return -1;

	if (flag & DP_F_DEREGISTER) {
		/*do de-register */

		return 0;
	}
	/*register a dp instance */

	/*to check whether any such matched HW cap */
	for (k = 0; k < DP_MAX_HW_CAP; k++) {
		if (!hw_cap_list[k].valid)
			continue;
		if (hw_cap_list[k].info.type == info->type &&
		    hw_cap_list[k].info.ver == info->ver) {
			break;
		}
	}
	if (k == DP_MAX_HW_CAP) {
		pr_err("%s fail to math cap type=%d/ver=%d\n",
		       __func__, info->type, info->ver);
		return -1;
	}

	/* to find a free instance */
	for (i = 0; i < DP_MAX_INST; i++) {
		if (!dp_port_prop[i].valid)
			break;
	}
	if (i == DP_MAX_INST) {
		pr_err("%s fail for dp inst full arealdy\n", __func__);
		return -1;
	}
	if (alloc_dma_chan_tbl(i)) {
		pr_err("FAIL to alloc dma chan tbl\n");
		return -1;
	}

	dp_prop = get_dp_port_prop(i);

	dp_prop->ops[0] = info->ops[0];
	dp_prop->ops[1] = info->ops[1];

	for (j = 0; j < DP_MAX_MAC_HANDLE; j++) {
		if (info->mac_ops[j])
			dp_prop->mac_ops[j] = info->mac_ops[j];
	}

	dp_prop->info = hw_cap_list[k].info;
	dp_prop->cbm_inst = info->cbm_inst;
	dp_prop->qos_inst = info->qos_inst;
	dp_prop->valid = 1;
#ifdef CONFIG_LTQ_DATAPATH_CPUFREQ
	dp_cpufreq_notify_init(i);
	DP_DEBUG(DP_DBG_FLAG_COC, "DP registered CPUFREQ notifier\n");
#endif
	if (alloc_dp_port_subif_info(i)) {
		pr_err("alloc_dp_port_subif_info fail..\n");
		return DP_FAILURE;
	}
	if (dp_prop->info.dp_platform_set(i, DP_PLATFORM_INIT) < 0) {
		dp_prop->valid = 0;
		pr_err("%s failed for inst=%d\n", __func__, i);
		return -1;
	}
	info->inst = i;
	dp_inst_num++;
	DP_DEBUG(DP_DBG_FLAG_INST,
		 "%s ok: inst=%d, dp_inst_num=%d\n", __func__, i, dp_inst_num);
	return 0;
}
EXPORT_SYMBOL(dp_request_inst);

struct dp_hw_cap *match_hw_cap(struct dp_inst_info *info, u32 flag)
{
	int k;

	for (k = 0; k < DP_MAX_HW_CAP; k++) {
		if (!hw_cap_list[k].valid)
			continue;
		if (hw_cap_list[k].info.type == info->type &&
		    hw_cap_list[k].info.ver == info->ver) {
			return &hw_cap_list[k];
		}
	}
	return NULL;
}

/*Note: like pon one device can have multiple ctp,
 *ie, it may register multiple times
 */
u32 dp_dev_hash(void *dev)
{
	return hash_ptr(dev, DP_DEV_HASH_BIT_LENGTH);
}

struct dp_dev *dp_dev_lookup(struct net_device *dev)
{
	struct hlist_head *head;
	struct dp_dev *item;
	u32 idx;

	idx = dp_dev_hash(dev);
	head = &dp_dev_list[idx];

	hlist_for_each_entry(item, head, hlist) {
		if (item->dev == dev)
			return item;
	}
	return NULL;
}

#if IS_ENABLED(CONFIG_PON_QOS)
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
int (*pon_qos_setup_tc_fn)(struct net_device *dev,
			   u32 handle,
			   __be16 protocol,
			   struct tc_to_netdev *tc,
			   int port_id,
			   int deq_idx);
#else
int (*pon_qos_setup_tc_fn)(struct net_device *dev,
			   enum tc_setup_type type,
			   void *type_data,
			   int port_id,
			   int deq_idx);
#endif
EXPORT_SYMBOL(pon_qos_setup_tc_fn);
#define DP_ENABLE_TC_OFFLOADS
#endif

#if defined(DP_ENABLE_TC_OFFLOADS)
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
static int dp_ndo_setup_tc(struct net_device *dev, u32 handle,
			   __be16 protocol, struct tc_to_netdev *tc)
{
#if IS_ENABLED(CONFIG_PON_QOS)
	if (pon_qos_setup_tc_fn)
		return pon_qos_setup_tc_fn(dev, handle, protocol, tc, -1, -1);
#endif

#if IS_ENABLED(CONFIG_QOS_MGR)
	if (qos_mgr_hook_setup_tc)
		return qos_mgr_hook_setup_tc(dev, handle, protocol, tc);
#endif
	if (dev->netdev_ops->ndo_setup_tc) {
		pr_err("Cannot support ndo_setup_tc\n");
		return -1;
	}
	return -1;
}
#else
static int dp_ndo_setup_tc(struct net_device *dev,
			   enum tc_setup_type type,
			   void *type_data)
{
#if IS_ENABLED(CONFIG_PON_QOS)
	if (pon_qos_setup_tc_fn)
		return pon_qos_setup_tc_fn(dev, type, type_data, -1, -1);
#endif

	return -1;
}
#endif /* LINUX_VERSION_CODE */
#endif /* DP_ENABLE_TC_OFFLOADS */

#if IS_ENABLED(CONFIG_INTEL_VPN)
static int dp_inst_register_xfrm_ops(struct net_device *dev, int reset)
{
	struct intel_vpn_ops *vpn;
	struct dp_dev *dp_dev;
	int i;
	u32 flag = DP_OPS_XFRMDEV;
	int offset[] = {offsetof(struct xfrmdev_ops, xdo_dev_state_add),
			offsetof(struct xfrmdev_ops, xdo_dev_state_delete),
			offsetof(struct xfrmdev_ops, xdo_dev_offload_ok)};
	void *cb[ARRAY_SIZE(offset)] = {NULL};
	u32 pflag = (DP_F_FAST_ETH_WAN | DP_F_FAST_DSL | DP_F_VUNI |
		     DP_F_DOCSIS | DP_F_GPON | DP_F_EPON);
	struct pmac_port_info *port_info;

	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev)
		return DP_FAILURE;

	port_info = get_dp_port_info(dp_dev->inst, dp_dev->ep);
	if (!(port_info->alloc_flags & pflag))
		return DP_SUCCESS;

	DP_DEBUG(DP_DBG_FLAG_OPS, "xfrm_ops %s for %s\n",
		 reset ? "reset" : "update", dev->name);
	vpn = dp_get_vpn_ops(0);
	if (!vpn) {
		netdev_err(dev, "Invalid vpn ops\n");
		return DP_FAILURE;
	}

	if (reset) {
		flag |= DP_OPS_RESET;
		dev->features &= ~NETIF_F_HW_ESP;
		dev->hw_enc_features &= ~NETIF_F_HW_ESP;
		dev->vlan_features &= ~NETIF_F_HW_ESP;
	} else {
		dev->features |= NETIF_F_HW_ESP;
		dev->hw_enc_features |= NETIF_F_HW_ESP;
		dev->vlan_features |= NETIF_F_HW_ESP;
		cb[0] =	vpn->add_xfrm_sa;
		cb[1] = vpn->delete_xfrm_sa;
		cb[2] = vpn->xfrm_offload_ok;
	}

	for (i = 0; i < ARRAY_SIZE(offset); i++) {
		if (dp_set_net_dev_ops_priv(dp_dev->dev, cb[i], offset[i],
					    flag)) {
			pr_err("%s failed to set ops %d\n", __func__, i);
			return DP_FAILURE;
		}
	}

	return DP_SUCCESS;
}
#endif

static void dp_set_gso_size(struct net_device *dev)
{
#if IS_ENABLED(CONFIG_LGM_TOE)
	struct dp_dev *dp_dev;
	struct pmac_port_info *port_info;
	struct lro_ops *ops;

	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev)
		return;
	port_info = get_dp_port_info(dp_dev->inst, dp_dev->ep);
	if (!(port_info->alloc_flags & (DP_F_FAST_ETH_LAN | DP_F_FAST_ETH_WAN)))
		return;

	/* remove TOE features if TOE is not loaded at the moment */
	ops = dp_get_lro_ops();
	if (!ops)
		return;

	netif_set_gso_max_size(dp_dev->dev, ops->get_gso_max_size(ops->toe));
#endif
}

static void dp_check_and_free_dp_dev(struct net_device *dev)
{
	struct dp_dev *dp_dev;

	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev)
		return;

	/* If failed to set ops, and no other owner */
	if (!dp_dev->cb_cnt && !(dp_dev->owner & DP_OPS_OWNER_DPM)) {
		hlist_del(&dp_dev->hlist);
		dp_ops_reset(dp_dev, dev);
		kmem_cache_free(cache_dev_list, dp_dev);
		DP_DEBUG(DP_DBG_FLAG_OPS, "Free ops memory for %s\n",
			 dev->name);
	}
}

int dp_inst_add_dev(struct net_device *dev, int inst,
		    int ep, int bp, int ctp, u32 flag)
{
	struct dp_dev *dp_dev;
	int new_f = 0;
	u32 idx;
#if defined(DP_ENABLE_TC_OFFLOADS)
	int offset;
#endif

	if (!dev) {
		pr_err("%s dev is NULL\n", __func__);
		return DP_FAILURE;
	}

	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev) {
		dp_dev = kmem_cache_zalloc(cache_dev_list, GFP_ATOMIC);
		if (dp_dev)
			new_f = 1;
		else
			return DP_FAILURE;
	}

	if (new_f || !(dp_dev->owner & DP_OPS_OWNER_DPM)) {
		dp_dev->inst = inst;
		dp_dev->dev = dev;
		dp_dev->ctp = ctp;
		dp_dev->ep = ep;
		dp_dev->bp = bp;
		if (new_f) {
			idx = dp_dev_hash(dev);
			hlist_add_head(&dp_dev->hlist, &dp_dev_list[idx]);
		}

#if defined(DP_ENABLE_TC_OFFLOADS)
		DP_DEBUG(DP_DBG_FLAG_OPS, "ndo_setup_tc update for %s\n",
			 dev->name);
		dev->features |= NETIF_F_HW_TC;
		offset = offsetof(const struct net_device_ops, ndo_setup_tc);
		dp_set_net_dev_ops_priv(dev, &dp_ndo_setup_tc, offset,
					DP_OPS_NETDEV);
#endif
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
		if (!(flag & DP_F_SUBIF_LOGICAL))
			dp_register_switchdev_ops(dev, 0);
#endif
#if IS_ENABLED(CONFIG_PTP_1588_CLOCK)
		dp_register_ptp_ioctl(dev, 0);
#endif
		dp_set_gso_size(dev);
#if IS_ENABLED(CONFIG_INTEL_VPN)
		dp_inst_register_xfrm_ops(dev, 0);
#endif
		/* need update dp_dev since it may be freed during ops update */
		dp_dev = dp_dev_lookup(dev);
		if (!dp_dev) {
			pr_err("why dp_dev not found for %s\n", dev->name);
			return DP_FAILURE;
		}
		dp_dev->owner |= DP_OPS_OWNER_DPM;

	}

	dp_dev->count++;

	DP_DEBUG(DP_DBG_FLAG_DBG, "%s:add new ctp=%d\n", __func__, ctp);

	return DP_SUCCESS;
}

int dp_inst_del_dev(struct net_device *dev, int inst,
		    int ep, u16 ctp, u32 flag)
{
	struct dp_dev *dp_dev;
	struct net_device *tmp_dev;
#if defined(DP_ENABLE_TC_OFFLOADS)
	int offset;
#endif

	if (!dev)
		return DP_FAILURE;

	/* sanity check */
	tmp_dev = dev_get_by_name(&init_net, dev->name);
	if (!tmp_dev) {
		DP_DEBUG(DP_DBG_FLAG_REG, "device %s(%px) is already de-registered from os\n",
		       dev->name, dev);
	} else {
		dev_put(tmp_dev);
		if (tmp_dev != dev) {
			pr_err("dev %s(%px) to dpm not match kernel's dev(%px)\n",
				dev->name, dev, tmp_dev);
			return DP_FAILURE;
		}
	}

	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev) {
		pr_err("Failed to dp_dev_lookup: %s\n",
		       dev ? dev->name : "NULL");
		return -1;
	}
	if (dp_dev->count <= 0) {
		pr_err("Count(%d) should > 0(%s)\n", dp_dev->count,
		       dev ? dev->name : "NULL");
		return -1;
	}
	if (dp_dev->inst != inst) {
		pr_err("Why inst not same:%d_%d(%s)\n", dp_dev->inst, inst,
		       dev ? dev->name : "NULL");
		return -1;
	}
	if (dp_dev->ep != ep) {
		pr_err("Why ep not same:%d_%d(%s)\n", dp_dev->ep, ep,
		       dev ? dev->name : "NULL");
		return -1;
	}
	/* Check added to remove pmapper device from dp_dev list
	 * if pmapper device is unregistered with last gem
	 */
	if (dp_dev->dev == dev) {
		dp_dev->count--;

		if (!dp_dev->count) {
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
			if (netif_is_bridge_port(dev) &&
			    !(flag & DP_F_SUBIF_LOGICAL)) {
				struct net_device *br_dev;
				struct inst_info *dp_info;
				int inst;
				u8 *addr;

				inst = dp_dev->inst;
				addr = (u8 *)dev->dev_addr;
				dp_info = get_dp_prop_info(inst);
				dp_info->dp_mac_reset(0, dp_dev->fid,
						      inst, addr);
				rcu_read_lock();
				br_dev =
				   netdev_master_upper_dev_get_rcu(dev);
				if (br_dev)
					dp_del_br_if(dev, br_dev, inst,
						     dp_dev->bp);
				rcu_read_unlock();
			}
#endif
			dp_dev->owner &= ~DP_OPS_OWNER_DPM;

#if defined(DP_ENABLE_TC_OFFLOADS)
			DP_DEBUG(DP_DBG_FLAG_OPS, "ndo_setup_tc reset for %s\n",
				 dev->name);
			offset = offsetof(const struct net_device_ops,
					  ndo_setup_tc);
			dp_set_net_dev_ops_priv(dev, NULL,
						offset,
					DP_OPS_NETDEV | DP_OPS_RESET);
#endif
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
			dp_register_switchdev_ops(dev, 1);
#endif
#if IS_ENABLED(CONFIG_PTP_1588_CLOCK)
			dp_register_ptp_ioctl(dev, 1);
#endif
#if IS_ENABLED(CONFIG_INTEL_VPN)
			dp_inst_register_xfrm_ops(dev, 1);
#endif
			/* Since all the ops are enable by CONFIG,
			 * there is a possibility that no ops are set.
			 * Check again to avoid not freeing dp_dev.
			 */
			dp_check_and_free_dp_dev(dev);
		}
		return 0;
	}

	pr_err("dp_del_dev fail(%s): not found ctp=%d\n",
	       dev ? dev->name : "NULL", ctp);
	return -1;
}

int dp_set_net_dev_ops_priv(struct net_device *dev, void *ops_cb,
			    int ops_offset, u32 flag)
{
	struct dp_dev *dp_dev;
	void **dev_ops;
	void **org_ops;
	void *new_ops;
	int ops_sz;
	unsigned long org_cb = 0, new_cb;
	int sz = sizeof(unsigned long);
	int offset = ops_offset / sz;
	int idx, err = DP_FAILURE;

	if (!dev) {
		pr_err("%s invalid dev\n", __func__);
		return err;
	}

	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev && !(flag & DP_OPS_RESET)) {
		/* New net_device, create and add to dp_dev_list */
		idx = dp_dev_hash(dev);
		dp_dev = kmem_cache_zalloc(cache_dev_list, GFP_ATOMIC);
		if (dp_dev) {
			dp_dev->dev = dev;
			dp_dev->owner = flag & DP_OPS_OWNER_EXT;
			hlist_add_head(&dp_dev->hlist, &dp_dev_list[idx]);
		} else {
			pr_err("%s kmem_cache_zalloc failed\n", __func__);
		}
	}

	if (!dp_dev) {
		pr_err("%s failed to create dp_dev %s\n", __func__,
		       dev ? dev->name : "NULL");
		err = DP_FAILURE;
		goto EXIT;
	}

	switch (flag & DP_OPS_MASK) {
	case DP_OPS_NETDEV:
		DP_DEBUG(DP_DBG_FLAG_OPS, "before DP_OPS_NETDEV %s %d %s\n",
			 flag & DP_OPS_RESET ? "reset" : "update",
			 dp_dev->cb_cnt,
			 dev->name);
		dev_ops = (void **)&dev->netdev_ops;
		org_ops = (void **)&dp_dev->old_dev_ops;
		new_ops = &dp_dev->new_dev_ops;
		ops_sz = sizeof(*dev->netdev_ops);
		break;

	case DP_OPS_ETHTOOL:
		DP_DEBUG(DP_DBG_FLAG_OPS, "before DP_OPS_ETHTOOL %s %d %s\n",
			 flag & DP_OPS_RESET ? "reset" : "update",
			 dp_dev->cb_cnt,
			 dev->name);
		dev_ops = (void **)&dev->ethtool_ops;
		org_ops = (void **)&dp_dev->old_ethtool_ops;
		new_ops = &dp_dev->new_ethtool_ops;
		ops_sz = sizeof(*dev->ethtool_ops);
		break;

#if IS_ENABLED(CONFIG_NET_SWITCHDEV)
	case DP_OPS_SWITCHDEV:
		DP_DEBUG(DP_DBG_FLAG_OPS, "before DP_OPS_SWITCHDEV %s %d %s\n",
			 flag & DP_OPS_RESET ? "reset" : "update",
			 dp_dev->cb_cnt,
			 dev->name);
		dev_ops = (void **)&dev->switchdev_ops;
		org_ops = (void **)&dp_dev->old_swdev_ops;
		new_ops = &dp_dev->new_swdev_ops;
		ops_sz = sizeof(*dev->switchdev_ops);
		break;
#endif
#if IS_ENABLED(CONFIG_INTEL_VPN)
	case DP_OPS_XFRMDEV:
		DP_DEBUG(DP_DBG_FLAG_OPS, "before DP_OPS_XFRMDEV %s %d %s\n",
			 flag & DP_OPS_RESET ? "reset" : "update",
			 dp_dev->cb_cnt,
			 dev->name);
		dev_ops = (void **)&dev->xfrmdev_ops;
		org_ops = (void **)&dp_dev->old_xfrm_ops;
		new_ops = &dp_dev->new_xfrm_ops;
		ops_sz = sizeof(*dev->xfrmdev_ops);
		break;
#endif
	default:
		pr_err("%s ops not supported\n", __func__);
		goto EXIT;
	}

	if (offset >= ops_sz / sz) {
		pr_err("%s wrong ops offset\n", __func__);
		goto ERR;
	}

	new_cb = *((unsigned long *)new_ops + offset);
	if (*org_ops)
		org_cb = *((unsigned long *)(*org_ops) + offset);

	if (flag & DP_OPS_RESET) {
		/* Set ops with original callback */
		if (org_cb == new_cb) {
			pr_warn("%s ops already reset\n", __func__);
			goto EXIT;
		}

		err = dp_ops_set(dev_ops, ops_offset, ops_sz,
				 org_ops, new_ops, (void *)org_cb);
		if (!err)
			dp_dev->cb_cnt--;
	} else {
		/* Set ops with new callback */
		if (new_cb == (unsigned long)ops_cb || new_cb != org_cb) {
			pr_warn("%s ops already set\n", __func__);
			goto EXIT;
		}

		err = dp_ops_set(dev_ops, ops_offset, ops_sz,
				 org_ops, new_ops, ops_cb);
		if (!err)
			dp_dev->cb_cnt++;
	}
	DP_DEBUG(DP_DBG_FLAG_OPS, "after %s %d %s\n",
			 flag & DP_OPS_RESET ? "reset" : "update",
			 dp_dev->cb_cnt,
			 dev->name);

ERR:
	dp_check_and_free_dp_dev(dev);

EXIT:
	return err;
}

u32 dp_mod_hash(struct module *owner, u16 ep)
{
	return hash_ptr(owner, DP_MOD_HASH_BIT_LENGTH);
}

struct dp_mod *dp_mod_lookup(struct hlist_head *head, struct module *owner,
			     u16 ep, u32 flag)
{
	struct dp_mod *item;

	hlist_for_each_entry(item, head, hlist) {
		if (item->mod == owner && item->ep == ep)
			return item;
	}
	return NULL;
}

/* tuple: owner + ep
 * act: inst
 */
int dp_inst_insert_mod(struct module *owner, u16 ep, u32 inst, u32 flag)
{
	struct dp_mod *dp_mod;
	u8 new_f = 0;
	u32 idx;

	if (!owner) {
		pr_err("owner NULL?\n");
		return -1;
	}
	idx = dp_mod_hash(owner, ep);
	DP_DEBUG(DP_DBG_FLAG_INST, "%s:idx=%u\n", __func__, idx);
	dp_mod = dp_mod_lookup(&dp_mod_list[idx], owner, ep, flag);
	if (!dp_mod) { /*alloc new */
		dp_mod = kmem_cache_zalloc(cache_mod_list, GFP_ATOMIC);
		if (dp_mod) {
			dp_mod->mod = owner;
			dp_mod->ep = ep;
			dp_mod->inst = inst;
			new_f = 1;
		}
	}
	if (!dp_mod)
		return -1;
	if (new_f)
		hlist_add_head(&dp_mod->hlist, &dp_mod_list[idx]);
	DP_DEBUG(DP_DBG_FLAG_INST, "%s:%s\n", __func__, owner->name);
	return 0;
}

int dp_inst_del_mod(struct module *owner, u16 ep, u32 flag)
{
	struct dp_mod *dp_mod;
	u32 idx;

	if (!owner) {
		pr_err("owner NULL?\n");
		return -1;
	}
	idx = dp_mod_hash(owner, ep);
	dp_mod = dp_mod_lookup(&dp_mod_list[idx], owner, ep, flag);
	if (!dp_mod) {
		pr_err("Failed to dp_mod_lookup: %s\n",
		       owner->name);
		return -1;
	}
	hlist_del(&dp_mod->hlist);
	kmem_cache_free(cache_mod_list, dp_mod);

	DP_DEBUG(DP_DBG_FLAG_INST, "%s ok: %s:\n", __func__, owner->name);
	return 0;
}

int dp_get_inst_via_module(struct module *owner,  u16 ep, u32 flag)
{
	struct dp_mod *dp_mod;
	u32 idx;

	if (!owner) {
		pr_err("owner NULL?\n");
		return -1;
	}
	idx = dp_mod_hash(owner, ep);
	dp_mod = dp_mod_lookup(&dp_mod_list[idx], owner, ep, flag);
	if (!dp_mod) {
		pr_err("Failed to dp_mod_lookup: %s\n",
		       owner->name);
		return -1;
	}

	return dp_mod->inst;
}

/* if dev NULL, use subif_name, otherwise use dev to search */
int dp_get_inst_via_dev(struct net_device *dev, char *subif_name,
			u32 flag)
{
	struct dp_dev *dp_dev;

	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev)
		return 0; /*workaround:otherwise caller need to check value */

	return dp_dev->inst;
}

static u32 dev_hash_index;
static struct dp_dev *dp_dev_proc;
int proc_inst_dev_dump(struct seq_file *s, int pos)
{

	if (!capable(CAP_SYS_PACCT))
		return -1;
	while (!dp_dev_proc) {
		dev_hash_index++;
		pos = 0;
		if (dev_hash_index == DP_DEV_HASH_SIZE)
			return -1;

		dp_dev_proc =
			hlist_entry_safe((&dp_dev_list[dev_hash_index])->first,
					 struct dp_dev, hlist);
	}
	seq_printf(s, "%s=%u %s=%d %s=%s(@%px) %s=%d ep=%d bp=%d %s=%d %s=%d @%px\n",
		   "Hash",
		   dev_hash_index,
		   "pos",
		   pos,
		   "dev",
		   dp_dev_proc->dev ? dp_dev_proc->dev->name : "NULL",
		   dp_dev_proc->dev,
		   "inst",
		   dp_dev_proc->inst,
		   dp_dev_proc->ep,
		   dp_dev_proc->bp,
		   "ctp",
		   dp_dev_proc->ctp,
		   "count",
		   dp_dev_proc->count,
		   dp_dev_proc);
	seq_puts(s, "\n");
	dp_dev_proc = hlist_entry_safe((dp_dev_proc)->hlist.next,
				       struct dp_dev, hlist);

	pos++;
	return pos;
}

int proc_inst_dev_start(void)
{
	dev_hash_index = 0;
	dp_dev_proc = hlist_entry_safe((&dp_dev_list[dev_hash_index])->first,
				       struct dp_dev, hlist);
	return 0;
}

static void dump_cap(struct seq_file *s, struct dp_cap *cap)
{
	if (!s)
		return;
	seq_printf(s, "	HW TX checksum offloading: %s\n",
		   cap->tx_hw_chksum ? "Yes" : "No");
	seq_printf(s, "	HW RX checksum verification: %s\n",
		   cap->rx_hw_chksum ? "Yes" : "No");
	seq_printf(s, "	HW TSO: %s\n",
		   cap->hw_tso ? "Yes" : "No");
	seq_printf(s, "	HW GSO: %s\n",
		   cap->hw_gso ? "Yes" : "No");
	seq_printf(s, "	QOS Engine: %s\n",
		   cap->qos_eng_name);
	seq_printf(s, "	Pkt Engine: %s\n",
		   cap->pkt_eng_name);
	seq_printf(s, "	max_num_queues: %d\n",
		   cap->max_num_queues);
	seq_printf(s, "	max_num_scheds: %d\n",
		   cap->max_num_scheds);
	seq_printf(s, "	max_num_deq_ports: %d\n",
		   cap->max_num_deq_ports);
	seq_printf(s, "	max_num_qos_ports: %d\n",
		   cap->max_num_qos_ports);
	seq_printf(s, "	max_num_dp_ports: %d\n",
		   cap->max_num_dp_ports);
	seq_printf(s, "	max_num_subif_per_port: %d\n",
		   cap->max_num_subif_per_port);
	seq_printf(s, "	max_num_subif: %d\n",
		   cap->max_num_subif);
	seq_printf(s, "	max_num_bridge_port: %d\n",
		   cap->max_num_bridge_port);
}

static u32 mod_hash_index;
static struct dp_mod *dp_mod_proc;
int proc_inst_mod_dump(struct seq_file *s, int pos)
{
	if (!capable(CAP_SYS_PACCT))
		return -1;
	while (!dp_mod_proc) {
		mod_hash_index++;
		pos = 0;
		if (mod_hash_index == DP_MOD_HASH_SIZE)
			return -1;

		dp_mod_proc =
			hlist_entry_safe((&dp_mod_list[mod_hash_index])->first,
					 struct dp_mod, hlist);
	}
	seq_printf(s, "Hash=%u pos=%d owner=%s(@%px) ep=%d inst=%d\n",
		   mod_hash_index,
		   pos,
		   dp_mod_proc->mod->name,
		   dp_mod_proc->mod,
		   dp_mod_proc->ep,
		   dp_mod_proc->inst);

	dp_mod_proc = hlist_entry_safe((dp_mod_proc)->hlist.next,
				       struct dp_mod, hlist);
	pos++;
	return pos;
}

int proc_inst_dump(struct seq_file *s, int pos)
{
	struct dp_cap cap;

	if (!capable(CAP_SYS_PACCT))
		return -1;
	if (!dp_port_prop[pos].valid)
		goto NEXT;
	seq_printf(s, "Inst[%d] Type=%u ver=%d\n",
		   pos,
		   dp_port_prop[pos].info.type,
		   dp_port_prop[pos].info.ver);
	/*dump_cap(s, &dp_port_prop[pos].info.cap);*/
	cap.inst = pos;
	dp_get_cap(&cap, 0);
	dump_cap(s, &cap);

NEXT:
	pos++;
	if (pos == DP_MAX_INST)
		return -1;
	return pos;
}

int proc_inst_hal_dump(struct seq_file *s, int pos)
{
	if (!capable(CAP_SYS_PACCT))
		return -1;
	if (!hw_cap_list[pos].valid)
		goto NEXT;

	seq_printf(s, "HAL[%d] Type=%u ver=%d dp_cap_num=%d\n",
		   pos,
		   hw_cap_list[pos].info.type,
		   hw_cap_list[pos].info.ver,
		   dp_cap_num);
	dump_cap(s, &hw_cap_list[pos].info.cap);

NEXT:
	pos++;
	if (pos == DP_MAX_HW_CAP)
		return -1;
	return pos;
}

int proc_inst_mod_start(void)
{
	mod_hash_index = 0;
	dp_mod_proc = hlist_entry_safe((&dp_mod_list[mod_hash_index])->first,
				       struct dp_mod, hlist);
	return 0;
}

int dp_inst_init(u32 flag)
{
	int i;

	dp_cap_num = 0;
	memset(hw_cap_list, 0, sizeof(hw_cap_list));
	for (i = 0; i < DP_DEV_HASH_SIZE; i++)
		INIT_HLIST_HEAD(&dp_dev_list[i]);

	cache_dev_list = kmem_cache_create("dp_dev_list", sizeof(struct dp_dev),
					   0, SLAB_HWCACHE_ALIGN, NULL);
	if (!cache_dev_list)
		return -ENOMEM;
	cache_mod_list = kmem_cache_create("dp_mod_list", sizeof(struct dp_mod),
					   0, SLAB_HWCACHE_ALIGN, NULL);
	if (!cache_mod_list) {
		kmem_cache_destroy(cache_dev_list);
		return -ENOMEM;
	}
	return 0;
}

void dp_inst_free(void)
{
	kmem_cache_destroy(cache_mod_list);
	kmem_cache_destroy(cache_dev_list);
}
