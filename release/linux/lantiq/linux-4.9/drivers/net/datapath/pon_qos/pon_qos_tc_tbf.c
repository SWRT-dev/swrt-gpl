// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#include <net/pkt_cls.h>
#include <linux/version.h>
#include "pon_qos_tc_qos.h"
#include "pon_qos_trace.h"

static int pon_tbf_remove(struct net_device *dev, u32 handle, u32 parent)
{
	struct pon_qos_qdisc *qdisc = NULL;
	struct pon_qos_q_data *qid = NULL;
	struct pon_qos_port *port = NULL;
	int ret;

	port = pon_qos_port_get(dev);
	if (!port) {
		netdev_err(dev, "tc-tbf port get failed\n");
		return -ENODEV;
	}

	qdisc = pon_qos_qdisc_find(port, parent);
	if (!qdisc) {
		netdev_err(dev, "tc-tbf qdisc get failed\n");
		return -ENODEV;
	}

	qid = pon_qos_qdata_qid_get(dev, qdisc, parent);
	if (!qid) {
		ret = pon_qos_get_queue_by_handle(dev, parent, &qid);
		if (ret < 0 || !qid) {
			netdev_err(dev, "%s: handle not found err %d\n",
				   __func__, ret);
			return -EINVAL;
		}
	}

	ret = pon_qos_shaper_remove(qdisc, qid);
	if (ret < 0)
		return ret;

	radix_tree_delete(&port->qdiscs, TC_H_MAJ(handle));

	return 0;
}

static int pon_qos_tc_tbf_replace(struct net_device *dev,
				  struct tc_tbf_qopt_offload *opt)
{
	struct pon_qos_qdisc *qdisc = NULL;
	struct pon_qos_q_data *qid = NULL;
	struct pon_qos_port *port = NULL;
	int ret;

	port = pon_qos_port_get(dev);
	if (!port) {
		netdev_err(dev, "tc-tbf port get failed\n");
		return -ENODEV;
	}

	qdisc = pon_qos_qdisc_find(port, opt->parent);
	if (!qdisc) {
		netdev_err(dev, "tc-tbf qdisc get failed\n");
		return -ENODEV;
	}

	qid = pon_qos_qdata_qid_get(dev, qdisc, opt->parent);
	if (!qid) {
		ret = pon_qos_get_queue_by_handle(dev, opt->parent, &qid);
		if (ret < 0 || !qid) {
			netdev_err(dev, "%s: handle not found err %d\n",
				   __func__, ret);
			return -EINVAL;
		}
	}

	ret = pon_qos_shaper_add(qdisc, qid, &opt->set_params);
	if (ret < 0)
		return ret;

	ret = pon_qos_qdata_add(dev, qid, opt->handle, opt->parent,
				PON_QOS_QDATA_TBF, pon_tbf_remove);
	if (ret < 0)
		return ret;

	ret = radix_tree_insert(&port->qdiscs, TC_H_MAJ(opt->handle), qdisc);
	if (ret < 0) {
		netdev_err(dev, "%s: qdisc insertion to radix tree failed: %d\n",
			   __func__, ret);
		return ret;
	}

	return 0;
}

static int pon_qos_tc_tbf_destroy(struct net_device *dev,
				  struct tc_tbf_qopt_offload *opt)
{
	struct pon_qos_qdisc *qdisc = NULL;
	struct pon_qos_q_data *qid = NULL;
	struct pon_qos_port *port = NULL;
	int ret;

	port = pon_qos_port_get(dev);
	if (!port) {
		/* Linux deletes some qdiscs from root to the leaf node
		 * which may cause that the tbf offload is already removed
		 * by its parent i.e. deleted by pon_qos_qdisc_tree_del */
		netdev_dbg(dev, "tc-tbf port get failed\n");
		return -ENODEV;
	}

	qdisc = pon_qos_qdisc_find(port, opt->parent);
	if (!qdisc) {
		netdev_err(dev, "tc-tbf qdisc get failed\n");
		return -ENODEV;
	}

	qid = pon_qos_qdata_qid_get(dev, qdisc, opt->parent);
	if (!qid) {
		ret = pon_qos_get_queue_by_handle(dev, opt->parent, &qid);
		if (ret < 0 || !qid) {
			netdev_err(dev, "%s: handle not found err %d\n",
				   __func__, ret);
			return -EINVAL;
		}
	}

	return pon_qos_qdata_remove(dev, qid, opt->handle, opt->parent);
}

int pon_qos_tc_tbf_offload(struct net_device *dev,
			   void *type_data)
{
	int err = 0;
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	struct tc_tbf_qopt_offload *opt =
		((struct tc_to_netdev *)type_data)->sch_tbf;
#else
	struct tc_tbf_qopt_offload *opt =
		(struct tc_tbf_qopt_offload *)type_data;
#endif

	trace_pon_qos_tc_tbf_enter(dev, opt);

	switch (opt->command) {
	case TC_TBF_REPLACE:
		err = pon_qos_tc_tbf_replace(dev, opt);
		if (err < 0) {
			netdev_err(dev, "tc-tbf replace failed\n");
			return err;
		}
		break;
	case TC_TBF_DESTROY:
		err = pon_qos_tc_tbf_destroy(dev, opt);
		if (err < 0) {
			/* Expected to fail if root was previously removed */
			netdev_dbg(dev, "tc-tbf destroy failed\n");
			return err;
		}
		break;
	case TC_TBF_STATS:
		return -EOPNOTSUPP;
	default:
		break;
	}

	trace_pon_qos_tc_tbf_exit(dev, opt);

	return 0;
}
