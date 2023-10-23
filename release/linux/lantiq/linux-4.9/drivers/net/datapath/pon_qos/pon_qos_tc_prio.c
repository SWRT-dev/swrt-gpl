// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#include <net/pkt_cls.h>
#include <net/datapath_api.h>
#include <net/datapath_api_qos.h>
#include <linux/version.h>
#include "pon_qos_tc_qos.h"
#include "pon_qos_trace.h"

/* create qdisc tree */

static int pon_qos_sched_update(struct net_device *dev,
				struct pon_qos_port *port,
				struct tc_prio_qopt_offload *p)
{
	enum pon_qos_qdisc_type type = PON_QOS_QDISC_PRIO;

	if (!dev || !port || !p)
		return -EINVAL;

	return pon_qos_add_child_qdisc(dev, port, type, p->parent, p->handle);
}

static int pon_qos_queues_update(struct net_device *dev,
				 struct pon_qos_qdisc *sch,
				 struct tc_prio_qopt_offload *parms)
{
	u8 *priomap = parms->replace_params.priomap;
	u8 bands = parms->replace_params.bands;
	int ret, i;

	if (!sch->use_cnt)
		return -EINVAL;

	if (bands == sch->prio.bands)
		/* TODO: nothing to do? */
		return 0;

	if (bands < sch->prio.bands) {
		for (i = 0; i < sch->prio.bands; i++) {
			ret = pon_qos_queue_del(sch, i);
			if (ret < 0) {
				netdev_err(dev, "queue del failed\n");
				return ret;
			}
		}
	}

	sch->prio.bands = bands;
	memcpy(sch->prio.priomap, priomap,
	       sizeof(sch->prio.priomap));

	for (i = 0; i < bands; i++) {
		ret = pon_qos_queue_add(sch, PON_QOS_QDISC_PRIO, priomap[i], i);
		if (ret < 0)
			netdev_err(dev, "queue add failed\n");
	}

	return 0;
}

static int pon_qos_tc_prio_update_tree(struct pon_qos_qdisc *sch,
				       struct tc_prio_qopt_offload *p)
{
	/* tree update requires grafting so skip it for now */

	if (sch->num_children)
		return -EINVAL;

	return 0;
}

static int pon_qos_tc_prio_replace(struct net_device *dev,
				   struct tc_prio_qopt_offload *opt,
				   int port_id,
				   int deq_idx)
{
	struct pon_qos_port *port = NULL;
	struct pon_qos_qdisc *qdisc = NULL;
	int ret = 0;
	bool newp = false;

	port = pon_qos_port_get(dev);
	if (!port) {
		port = pon_qos_port_alloc(dev);
		if (!port) {
			netdev_err(dev, "tc-prio port alloc failed\n");
			return -ENOMEM;
		}
		newp = true;
	}

	if (opt->parent == TC_H_ROOT) {
		port->root_qdisc.port = port_id;
		port->root_qdisc.deq_idx = deq_idx;
		port->root_qdisc.dev = dev;
		ret = pon_qos_get_port_info(&port->root_qdisc);
		if (ret < 0)
			goto err_free_port;
	}

	qdisc = pon_qos_qdisc_find(port, opt->handle);
	if (!qdisc) {
		ret = pon_qos_sched_update(dev, port, opt);
		if (ret < 0) {
			netdev_err(dev, "tc-prio sched config failed\n");
			goto err_free_port;
		}
	}

	qdisc = pon_qos_qdisc_find(port, opt->handle);
	if (!qdisc) {
		netdev_err(dev, "%s: radix tree broken\n", __func__);
		ret = -ENODEV;
		goto err_free_port;
	}

	ret = pon_qos_tc_prio_update_tree(qdisc, opt);
	if (ret < 0) {
		ret = -EOPNOTSUPP;
		goto err_free_qdisc;
	}

	ret = pon_qos_queues_update(dev, qdisc, opt);
	if (ret < 0) {
		netdev_err(dev, "tc-prio queues config failed\n");
		goto err_free_qdisc;
	}

	return 0;

err_free_qdisc:
	ret = pon_qos_sched_del(qdisc);
	if (ret < 0)
		netdev_err(dev, "%s: sched del failed\n", __func__);
	if (qdisc != &port->root_qdisc)
		kfree(qdisc);
err_free_port:
	if (newp)
		pon_qos_port_delete(port);
	return ret;
}

static int pon_qos_tc_prio_destroy(struct net_device *dev,
				   struct tc_prio_qopt_offload *opt)
{
	struct pon_qos_port *port = NULL;
	struct pon_qos_qdisc *sch = NULL;

	port = pon_qos_port_get(dev);
	if (!port) {
		netdev_dbg(dev, "%s:port not found or deallocated\n", __func__);
		return 0;
	}

	sch = pon_qos_qdisc_find(port, opt->handle);
	if (!sch) {
		netdev_dbg(dev, "%s: sch not found or deallocated\n", __func__);
		return 0;
	}

	/*pon_qos_tc_sched_status(port, &port->root_qdisc);*/
	pon_qos_qdisc_tree_del(port, sch);

	return 0;
}

int pon_qos_tc_prio_offload(struct net_device *dev,
			    void *type_data,
			    int port_id,
			    int deq_idx)
{
	int err = 0;

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	struct tc_prio_qopt_offload *opt =
		((struct tc_to_netdev *)type_data)->sch_prio;
#else
	struct tc_prio_qopt_offload *opt =
		(struct tc_prio_qopt_offload *)type_data;
#endif

	ASSERT_RTNL();
	netdev_dbg(dev, "PRIO: offload starting\n");

	trace_pon_qos_tc_prio_enter(dev, opt);

	switch (opt->command) {
	case TC_PRIO_REPLACE:
		if (opt->replace_params.bands > PON_QOS_TC_MAX_Q) {
			netdev_err(dev, "tc-prio - out of range bands %d\n",
				   opt->replace_params.bands);
			return -EINVAL;
		}
		netdev_dbg(dev, "replace pid:%#x class/handle:%#x bands:%#x\n",
			   opt->parent, opt->handle, opt->replace_params.bands);
		err = pon_qos_tc_prio_replace(dev, opt,
					      port_id, deq_idx);
		if (err < 0) {
			netdev_err(dev, "tc-prio replace failed\n");
			return err;
		}
		break;
	case TC_PRIO_DESTROY:
		netdev_dbg(dev, "destroy pid:%#x class/handle:%#x\n",
			   opt->parent, opt->handle);
		err = pon_qos_tc_prio_destroy(dev, opt);
		if (err < 0) {
			netdev_err(dev, "tc-prio destroy failed\n");
			return err;
		}
		break;
	case TC_PRIO_STATS:
		/* TODO */
		netdev_dbg(dev, "stats pid:%#x class/handle:%#x\n",
			   opt->parent, opt->handle);
		return -EOPNOTSUPP;
	case TC_PRIO_GRAFT:
		/* TODO */
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
		netdev_dbg(dev, "graft pid:%#x class/handle:%#x bands:%#x\n",
			   opt->parent, opt->handle, opt->replace_params.bands);
#else
		netdev_dbg(dev, "graft pid:%#x class/handle:%#x band:%#x\n",
			   opt->parent, opt->handle, opt->graft_params.band);
#endif
		return -EOPNOTSUPP;
	default:
		break;
	}

	trace_pon_qos_tc_prio_exit(dev, opt);

	netdev_dbg(dev, "PRIO: offload end: %d\n", err);
	return 0;
}
