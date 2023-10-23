// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#include <net/pkt_cls.h>
#include <net/flow_dissector.h>
#include <net/datapath_api.h>
#include <net/datapath_api_qos.h>
#include <linux/version.h>
#include "pon_qos_tc_qos.h"
#include "pon_qos_trace.h"

#define WRED_GREEN_MAX_TH (500 * 1024)
#define WRED_GREEN_MIN_TH ((WRED_GREEN_MAX_TH * 90) / 100)
#define WRED_GREEN_MAX_P 100
#define WRED_YELLOW_MAX_TH (250 * 1024)
#define WRED_YELLOW_MIN_TH ((WRED_YELLOW_MAX_TH * 90) / 100)
#define WRED_YELLOW_MAX_P 100

static int wred_params_set(struct pon_qos_qdisc *sch,
			   struct pon_qos_q_data *qid,
			   enum pon_qos_qdata_type type,
			   bool def,
			   struct tc_red_qopt_offload_params *params)
{
	u64 prob_tmp;
	int id;
	struct dp_queue_conf q_cfg = {
		.q_id = qid->qid,
		.inst = sch->inst
	};

	if (!def && !params) {
		netdev_err(sch->dev, "%s: non-defaults params missing\n",
			   __func__);
		return -EINVAL;
	}

	switch (type) {
	case PON_QOS_QDATA_GREEN:
		id = 0;
		break;
	case PON_QOS_QDATA_YELLOW:
		id = 1;
		break;
	default:
		netdev_err(sch->dev, "%s: wrong type\n",
			   __func__);
		return -EINVAL;
	}

	/** Read */
	if (dp_queue_conf_get(&q_cfg, 0) == DP_FAILURE) {
		netdev_err(sch->dev, "%s: dp_queue_conf_get() failed\n",
			   __func__);
		return -EINVAL;
	}

	/** Modify */
	if (def) {
		q_cfg.min_size[id] = WRED_GREEN_MIN_TH;
		q_cfg.max_size[id] = WRED_GREEN_MAX_TH;
		q_cfg.wred_slope[id] = WRED_GREEN_MAX_P;
	} else {
		q_cfg.min_size[id] = params->min;
		q_cfg.max_size[id] = params->max;
		/* Calculate percent probability */
		prob_tmp = params->probability;
		prob_tmp = prob_tmp * 100;
		prob_tmp = DIV_ROUND_UP(prob_tmp, 1 << 16);
		prob_tmp = DIV_ROUND_UP(prob_tmp, 1 << 16);
		q_cfg.wred_slope[id] = prob_tmp;
	}

	netdev_dbg(sch->dev, "%s: min_size: %u, max_size: %u, probability: %u\n",
		   __func__, q_cfg.min_size[type], q_cfg.max_size[type],
		   q_cfg.wred_slope[type]);

	/** Write */
	if (dp_queue_conf_set(&q_cfg, 1) == DP_FAILURE) {
		netdev_err(sch->dev, "%s: dp_queue_conf_set() failed\n",
			   __func__);
		return -EINVAL;
	}

	return 0;
}

static int __wred_destroy(struct net_device *dev, u32 handle, u32 parent,
			  enum pon_qos_qdata_type type)
{
	struct pon_qos_port *port = NULL;
	struct pon_qos_qdisc *qdisc = NULL;
	struct pon_qos_q_data *qid = NULL;
	int ret;

	port = pon_qos_port_get(dev);
	if (!port) {
		netdev_err(dev, "tc-red port get failed\n");
		return -ENODEV;
	}

	qdisc = pon_qos_qdisc_find(port, parent);
	if (!qdisc) {
		/* Return success if parent was already removed */
		netdev_dbg(dev, "tc-red qdisc get failed\n");
		return 0;
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

	ret = wred_params_set(qdisc, qid, type, true, NULL);
	if (ret < 0)
		return ret;

	radix_tree_delete(&port->qdiscs, TC_H_MAJ(handle));

	return 0;
}

static int wred_yellow_destroy(struct net_device *dev, u32 handle, u32 parent)
{
	return __wred_destroy(dev, handle, parent, PON_QOS_QDATA_YELLOW);
}

static int wred_green_destroy(struct net_device *dev, u32 handle, u32 parent)
{
	return __wred_destroy(dev, handle, parent, PON_QOS_QDATA_GREEN);
}

static int pon_qos_tc_red_replace(struct net_device *dev,
				  struct tc_red_qopt_offload *opt)
{
	struct pon_qos_port *port = NULL;
	struct pon_qos_qdisc *qdisc = NULL;
	struct pon_qos_q_data *qid = NULL;
	enum pon_qos_qdata_type type;
	u32 tmp_h, tmp_p;
	void *destroy;
	int ret;

	port = pon_qos_port_get(dev);
	if (!port) {
		netdev_err(dev, "tc-red port get failed\n");
		return -ENODEV;
	}

	qdisc = pon_qos_qdisc_find(port, opt->parent);
	if (!qdisc) {
		netdev_err(dev, "tc-red qdisc get failed\n");
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

	/* Set GREEN for first RED, YELLOW for second */
	if (pon_qos_qdata_get_by_type(dev, qid, PON_QOS_QDATA_GREEN,
				      &tmp_h, &tmp_p)) {
		destroy = wred_green_destroy;
		type = PON_QOS_QDATA_GREEN;
		netdev_dbg(dev, "tc-red qdisc configuring green\n");
	} else {
		destroy = wred_yellow_destroy;
		type = PON_QOS_QDATA_YELLOW;
		netdev_dbg(dev, "tc-red qdisc configuring yellow\n");
	}

	ret = pon_qos_qdata_add(dev, qid, opt->handle, opt->parent,
				type, destroy);
	if (ret < 0)
		return ret;

	ret = wred_params_set(qdisc, qid, type, false, &opt->set);
	if (ret < 0)
		return ret;

	ret = radix_tree_insert(&port->qdiscs, TC_H_MAJ(opt->handle), qdisc);
	if (ret) {
		netdev_err(dev, "%s: qdisc insertion to radix tree failed: %d\n",
			   __func__, ret);
		return ret;
	}

	trace_pon_qos_tc_red_replace(dev, opt, qdisc, qid, type);

	return 0;
}

static int pon_qos_tc_red_destroy(struct net_device *dev,
				  struct tc_red_qopt_offload *opt)
{
	struct pon_qos_port *port = NULL;
	struct pon_qos_qdisc *qdisc = NULL;
	struct pon_qos_q_data *qid = NULL;
	int ret;

	port = pon_qos_port_get(dev);
	if (!port) {
		/* Return success if port was already removed */
		netdev_dbg(dev, "tc-red port get failed\n");
		return 0;
	}

	qdisc = pon_qos_qdisc_find(port, opt->parent);
	if (!qdisc) {
		/* Return success if parent was already removed */
		netdev_dbg(dev, "tc-red qdisc get failed\n");
		return 0;
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

	ret = pon_qos_qdata_remove(dev, qid, opt->handle, opt->parent);

	trace_pon_qos_tc_red_destroy(dev, opt, qdisc, qid);

	return ret;
}

int pon_qos_tc_red_offload(struct net_device *dev,
			   void *type_data)
{
	int err = 0;

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	struct tc_red_qopt_offload *opt =
		((struct tc_to_netdev *)type_data)->sch_red;
#else
	struct tc_red_qopt_offload *opt =
		(struct tc_red_qopt_offload *)type_data;
#endif
	ASSERT_RTNL();
	netdev_dbg(dev, "RED: offload starting\n");

	trace_pon_qos_tc_red_enter(dev, opt);

	switch (opt->command) {
	case TC_RED_REPLACE:
		netdev_dbg(dev, "replace pid:%#x class/handle:%#x\n",
			   opt->parent, opt->handle);
		err = pon_qos_tc_red_replace(dev, opt);
		if (err < 0) {
			netdev_err(dev, "tc-red replace failed\n");
			return err;
		}
		break;
	case TC_RED_DESTROY:
		netdev_dbg(dev, "destroy pid:%#x class/handle:%#x\n",
			   opt->parent, opt->handle);
		err = pon_qos_tc_red_destroy(dev, opt);
		if (err < 0) {
			netdev_err(dev, "tc-red destroy failed\n");
			return err;
		}
		break;
	case TC_RED_STATS:
	case TC_RED_XSTATS:
		return -EOPNOTSUPP;
	default:
		break;
	}

	trace_pon_qos_tc_red_exit(dev, opt);

	netdev_dbg(dev, "RED: offload end: %d\n", err);
	return 0;
}
