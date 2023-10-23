// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/version.h>
#include <net/pkt_cls.h>
#include <net/tc_act/tc_colmark.h>
#include <net/tc_act/tc_police.h>
#include <linux/list.h>
#include <net/datapath_api.h>
#include <net/datapath_api_qos.h>
#include "pon_qos_tc_flower.h"
#include "pon_qos_tc_vlan_prepare.h"
#include "pon_qos_tc_police.h"

#define PON_QOS_COLMARK_ACTION 0x1
#define PON_QOS_POLICE_ACTION 0x2

struct policer_data {
	struct dp_meter_cfg meter_cfg;
	struct net_device *dev;
	int id;
	int dp_inst;
	int flags;
	int acts;
	struct {
		bool flow_based;
	};
	struct list_head list;
};

static LIST_HEAD(policer_list);

static bool pon_qos_police_match(struct policer_data *p1,
				 struct policer_data *p2)
{
	if (p1->dev != p2->dev)
		return false;

	if (p1->meter_cfg.dir != p2->meter_cfg.dir)
		return false;

	/* 1. Meters cannot be attached on same CTP/BP
	 * 2. Color marking and policer can be set independently
	 */

	/* Nor policer nor colmark can be set on device with policer */
	if ((p1->acts & PON_QOS_POLICE_ACTION) &&
	    (p2->acts & PON_QOS_POLICE_ACTION))
		return true;

	if ((p1->acts & PON_QOS_COLMARK_ACTION) &&
	    (p2->acts & PON_QOS_COLMARK_ACTION))
		return true;

	return false;
}

static struct net_device *pon_qos_get_indev(struct net_device *dev,
					    struct tc_cls_flower_offload *f)
{
	int ifi = FL_FLOW_KEY_IFINDEX(f);

	if (ifi)
		return dev_get_by_index(dev_net(dev), ifi);
	return NULL;
}

static bool pon_qos_police_parse_actions(struct tc_cls_flower_offload *f,
					 struct policer_data *pd,
					 dp_subif_t *subif, bool ingress)
{
	struct dp_meter_cfg *meter_cfg = &pd->meter_cfg;
	struct policer_data *p = NULL;
	struct net_device *indev;
	const struct tc_action *a;
#if (KERNEL_VERSION(4, 15, 0) > LINUX_VERSION_CODE)
	LIST_HEAD(actions);
#else
	int i;
#endif
	u64 burst;

	pd->acts = 0;
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	if (tc_no_actions(f->exts))
#else
	if (!tcf_exts_has_actions(f->exts))
#endif
		return false;

#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list) {
#else
	tcf_exts_for_each_action(i, a, f->exts) {
#endif
		if (!(pd->acts & PON_QOS_COLMARK_ACTION) && is_tcf_colmark(a)) {
			/* The enumerations in the TC colmark are matching
			 * enumerations in the dp manager.
			 */
			meter_cfg->col_mode = tcf_colmark_mode(a);
			meter_cfg->mode = tcf_colmark_precedence(a);
			meter_cfg->type = tcf_colmark_mtype(a);

			netdev_dbg(pd->dev,
				   "%s: col_mode %d mode %d type %d\n",
				   __func__,
				   meter_cfg->col_mode,
				   meter_cfg->mode,
				   meter_cfg->type);

			/* Only mode is used for color marking */
			if (tcf_colmark_flags(a) & COLMARK_F_DROP_PRECEDENCE)
				pd->acts |= PON_QOS_COLMARK_ACTION;
		}

		if (!(pd->acts & PON_QOS_POLICE_ACTION) && is_tcf_police(a)) {
			burst = tcf_police_rate_bytes_ps(a);
			burst *= PSCHED_NS2TICKS(tcf_police_tcfp_burst(a));
			burst = div_u64(burst, PSCHED_TICKS_PER_SEC);
			/* Convert rates from bytes/s to kbit/s */
			meter_cfg->cir = tcf_police_rate_bytes_ps(a) * 8;
			meter_cfg->pir = tcf_police_peak_bytes_ps(a) * 8;
			meter_cfg->cbs = burst;
			meter_cfg->pbs = tcf_police_tcfp_mtu(a);

			netdev_dbg(pd->dev,
				   "%s: cir %llu pir %llu cbs %u pbs %u\n",
				   __func__,
				   meter_cfg->cir,
				   meter_cfg->pir,
				   meter_cfg->cbs,
				   meter_cfg->pbs);

			pd->acts |= PON_QOS_POLICE_ACTION;
		}
	}
	netdev_dbg(pd->dev, "%s: acts: %d\n", __func__, pd->acts);

	meter_cfg->dir = ingress ? DP_DIR_INGRESS : DP_DIR_EGRESS;

	indev = pon_qos_get_indev(pd->dev, f);

	/* On the pmapper and when the indev is the same as the normal dev
	 * we should use the bridge port.
	 */
	if (subif->flag_pmapper || (indev && indev == pd->dev)) {
		pd->flags |= DP_METER_ATTACH_BRPORT;
		pd->flow_based = ingress ? false : true;
	} else {
		pd->flags |= DP_METER_ATTACH_CTP;
	}

	netdev_dbg(pd->dev, "%s: flags: %d\n", __func__, pd->flags);

	/* The indev is only used when it is the same as the normal dev, it
	 * should be fine it decrese the ref counter here already.
	 */
	if (indev)
		dev_put(indev);

	/* BP config get failed which should never happen!*/
	if (meter_cfg->dp_pce.flow < 0)
		return false;

	list_for_each_entry(p, &policer_list, list) {
		if (pon_qos_police_match(p, pd)) {
			netdev_err(pd->dev,
				   "%s: policer already on this device\n",
				   __func__);
			return false;
		}
	}

	if (pd->acts && pd->acts <= 3)
		return true;

	return false;
}

static int pon_qos_police_config(struct policer_data *pd, bool en)
{
	int (*cfg)(struct net_device *dev, struct dp_meter_cfg *m, int flag);
	int max = pd->flow_based ? DP_TRAFFIC_TYPE_MAX : 1;
	int ret = 0, type;

	if (en)
		cfg = &dp_meter_add;
	else
		cfg = &dp_meter_del;

	for (type = 0; type < max; type++) {
		pd->meter_cfg.dp_pce.flow = type;
		ret = (cfg)(pd->dev, &pd->meter_cfg, pd->flags);
		if (ret != DP_SUCCESS) {
			netdev_err(pd->dev, "meter config fail\n");
			return ret;
		}
	}

	return 0;
}

int pon_qos_police_offload(struct net_device *dev,
			   struct tc_cls_flower_offload *f,
			   bool ingress)
{
	struct policer_data *pd = NULL;
	struct policer_data tmp = { 0 };
	dp_subif_t subif;
	int ret;

	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0);
	if (ret != DP_SUCCESS) {
		netdev_err(dev, "%s: can not get subif\n", __func__);
		return ret;
	}

	tmp.dev = dev;
	if (!pon_qos_police_parse_actions(f, &tmp, &subif, ingress)) {
		netdev_err(dev, "policer or marker actions parse error.\n");
		return -EOPNOTSUPP;
	}

	pd = kzalloc(sizeof(*pd), GFP_KERNEL);
	if (!pd)
		return -ENOMEM;

	*pd = tmp;
	pd->dp_inst = subif.inst;

	if (pd->acts & PON_QOS_POLICE_ACTION) {
		int id;

		ret = dp_meter_alloc(subif.inst, &id, pd->flags);
		if (ret != DP_SUCCESS) {
			netdev_err(dev, "meter alloc fail\n");
			goto out_free;
		}
		pd->meter_cfg.meter_id = id;
		ret = pon_qos_police_config(pd, true);
		if (ret != DP_SUCCESS) {
			netdev_err(dev, "meter config fail\n");
			goto out_dp_remove;
		}
	}
	if (pd->acts & PON_QOS_COLMARK_ACTION) {
		/* No need to allocate a meter_id, it is not used
		 * for color marking.
		 */
		ret = dp_meter_add(dev, &pd->meter_cfg,
				   pd->flags | DP_COL_MARKING);
		if (ret != DP_SUCCESS) {
			netdev_err(dev, "meter config failed: %i\n", ret);
			goto out_free;
		}
	}

	ret = pon_qos_tc_flower_storage_add(dev, f->cookie,
					    TC_TYPE_COLMARK,
					    &pd->meter_cfg, (void *)pd);
	if (ret < 0) {
		netdev_err(dev, "%s can not add to storage\n", __func__);
		goto out_dp_remove;
	}

	list_add(&pd->list, &policer_list);

	return 0;

out_dp_remove:
	dp_meter_del(dev, &pd->meter_cfg, pd->flags);
out_free:
	kfree(pd);
	return ret;
}

int pon_qos_police_unoffload(struct net_device *dev,
			     void *meter_cfg_ptr, void *flags_ptr)
{
	struct policer_data *pd = (struct policer_data *)flags_ptr;
	struct dp_meter_cfg *meter_cfg = meter_cfg_ptr;
	int ret;

	/* It is not possible to delete the color marking, dp_meter_del() will
	 * only delete the metering in DP manager and not the color marking.
	 * Instead of deleting the setting, just overwrite it with a new one
	 * using the default settings, DP manager does not store any list of
	 * the settings, so this should be save.
	 * Please adapt this to delete the color marking in case
	 * dp_meter_add() or dp_meter_del() change.
	 */
	netdev_dbg(dev, "police action mask: %#x\n", pd->acts);
	if (pd->acts & PON_QOS_COLMARK_ACTION) {
		meter_cfg->col_mode = 0;
		meter_cfg->mode = DP_INTERNAL;
		meter_cfg->type = srTCM;
		ret = dp_meter_add(dev, meter_cfg, pd->flags | DP_COL_MARKING);
		if (ret != DP_SUCCESS) {
			/* We can not do anything when this fails and will
			 * probably leak some memory because we do not know
			 * how much was deleted.
			 */
			netdev_err(dev, "%s: del failed: %i\n", __func__, ret);
		}
	}

	if (pd->acts & PON_QOS_POLICE_ACTION) {
		netdev_dbg(dev, "del meter id: %u\n", meter_cfg->meter_id);
		ret = pon_qos_police_config(pd, false);
		if (ret != DP_SUCCESS)
			netdev_err(dev, "meter del failed: %i\n", ret);
		ret = dp_meter_alloc(pd->dp_inst, &meter_cfg->meter_id, DP_F_DEREGISTER);
		if (ret != DP_SUCCESS)
			netdev_err(dev, "meter dealloc failed: %i\n", ret);
	}

	list_del(&pd->list);
	kfree(pd);
	return 0;
}
