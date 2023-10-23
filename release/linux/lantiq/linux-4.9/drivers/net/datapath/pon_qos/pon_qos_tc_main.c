// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#include <linux/module.h>
#include <linux/version.h>
#include <net/pkt_cls.h>
#include <net/pon_qos_tc.h>
#include <linux/jiffies.h>
#include "pon_qos_tc_main.h"
#include "pon_qos_tc_qos.h"
#include "pon_qos_tc_debugfs.h"
#include "pon_qos_trace.h"
#if IS_ENABLED(CONFIG_QOS_MGR)
#include <net/qos_mgr/qos_mgr_tc_hook.h>
#endif

#define DRV_NAME	"mod_pon_qos"

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
int pon_qos_setup_tc(struct net_device *dev,
		     u32 handle,
		     __be16 protocol,
		     struct tc_to_netdev *tc,
		     int port_id,
		     int deq_idx)
{
	int ret = 0;

	ASSERT_RTNL();
	netdev_dbg(dev, "%s: start\n", __func__);

	trace_pon_qos_setup_tc_enter(dev, tc->type);

	switch (tc->type) {
	case TC_SETUP_CLSFLOWER:
		ret = pon_qos_tc_flower_offload(dev,
						handle,
						tc);
		break;
	case TC_SETUP_MQPRIO:
		ret = pon_qos_tc_mqprio_offload(dev, handle, tc,
						port_id, deq_idx);
		break;
	case TC_SETUP_QDISC_PRIO:
		ret = pon_qos_tc_prio_offload(dev, tc,
					      port_id, deq_idx);
		break;
	case TC_SETUP_DRR:
		ret = pon_qos_tc_drr_offload(dev, tc,
					     port_id, deq_idx);
		break;
	case TC_SETUP_QDISC_RED:
		ret = pon_qos_tc_red_offload(dev, tc);
		break;
	case TC_SETUP_TBF:
		ret = pon_qos_tc_tbf_offload(dev, tc);
		break;
	default:
		netdev_err(dev, "offload type:%d not supported\n", tc->type);
		ret = -EOPNOTSUPP;
		break;
	}

	trace_pon_qos_setup_tc_exit(dev, tc->type);

	return ret;
}
#else
int pon_qos_setup_tc(struct net_device *dev,
		     enum tc_setup_type type,
		     void *type_data,
		     int port_id,
		     int deq_idx)
{
	int ret = 0;

	ASSERT_RTNL();
	netdev_dbg(dev, "%s: start type %d\n", __func__, type);

	trace_pon_qos_setup_tc_enter(dev, type);

	switch (type) {
	case TC_SETUP_QDISC_MQPRIO:
		ret = pon_qos_tc_mqprio_offload(dev,
		      ((struct tc_mqprio_qopt_offload *)type_data)->handle,
		      type_data, port_id, deq_idx);
		break;
	case TC_SETUP_QDISC_PRIO:
		ret = pon_qos_tc_prio_offload(dev, type_data,
					      port_id, deq_idx);
		break;
	case TC_SETUP_QDISC_DRR:
		ret = pon_qos_tc_drr_offload(dev, type_data,
					     port_id, deq_idx);
		break;
	case TC_SETUP_QDISC_RED:
		ret = pon_qos_tc_red_offload(dev, type_data);
		break;
	case TC_SETUP_QDISC_TBF:
		ret = pon_qos_tc_tbf_offload(dev, type_data);
		break;
	case TC_SETUP_BLOCK:
		ret = pon_qos_tc_block_offload(dev, type_data);
		break;
	case TC_SETUP_QDISC_MQ:
		/* ToDo: Support to be added */
		ret = -EOPNOTSUPP;
		break;
	default:
		netdev_err(dev, "offload type:%d not supported\n", type);
		ret = -EOPNOTSUPP;
		break;
	}

	trace_pon_qos_setup_tc_exit(dev, type);

	return ret;
}
#endif
EXPORT_SYMBOL(pon_qos_setup_tc);

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
int pon_qos_setup_tc_gen(struct net_device *dev,
			 u32 handle,
			 __be16 protocol,
			 struct tc_to_netdev *tc)
{
	int ret;

	ret = pon_qos_setup_tc(dev, handle, protocol, tc, -1, -1);
	if (!ret)
		return ret;
/* TODO: this has to be removed when all features are implemented in this
 * driver
 */
#if IS_ENABLED(CONFIG_QOS_MGR)
	if (qos_mgr_hook_setup_tc)
		return qos_mgr_hook_setup_tc(dev, handle, protocol, tc);
#endif
	return ret;
}
#else
int pon_qos_setup_tc_gen(struct net_device *dev,
			 enum tc_setup_type type,
			 void *type_data)
{
	int ret;

	ret = pon_qos_setup_tc(dev, type, type_data, -1, -1);
	if (!ret)
		return ret;
/* TODO: this has to be removed when all features are implemented in this
 * driver
 */
#if IS_ENABLED(CONFIG_QOS_MGR)
	if (qos_mgr_hook_setup_tc)
		return qos_mgr_hook_setup_tc(dev, type, type_data);
#endif
	return ret;
}
#endif

static int __init pon_qos_init(void)
{
	int ret = 0;

	ret = pon_qos_tc_debugfs_init();
	if (ret)
		return ret;

	rtnl_lock();
	pon_qos_setup_tc_fn = pon_qos_setup_tc;
	ret = pon_qos_tc_mappings_init();
	rtnl_unlock();

	if (ret) {
		pon_qos_tc_debugfs_exit();
		return ret;
	}

	return ret;
}

static void __exit pon_qos_destroy(void)
{
	pon_qos_tc_debugfs_exit();
	rtnl_lock();
	pon_qos_ports_cleanup();
	pon_qos_setup_tc_fn = NULL;
	rtnl_unlock();
}

module_init(pon_qos_init);
module_exit(pon_qos_destroy);
MODULE_LICENSE("GPL");
MODULE_ALIAS_RTNL_LINK(DRV_NAME);
