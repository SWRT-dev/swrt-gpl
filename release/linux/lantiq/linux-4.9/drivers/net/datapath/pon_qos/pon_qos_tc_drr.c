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

#define MAX_QUANTUM (255 * 4096)
#define KB 1024
#define MIN_WEIGHT 1
#define MAX_WEIGHT 255

static int pon_qos_scale_quantum(u32 q_quantum, u32 sch_quanta)
{
	u32 weight;

	if (q_quantum < (KB * sch_quanta))
		weight = MIN_WEIGHT;
	else if (q_quantum > MAX_QUANTUM)
		weight = MAX_WEIGHT;
	else
		weight = q_quantum / (KB * sch_quanta);

	return weight;
}

/* Two cases of adding qdisc:
 * 1. qdisc is root
 * 2. qdisc is child
 */
static int pon_qos_drr_add_child_qdisc(struct net_device *dev,
				       struct pon_qos_port *port,
				       struct tc_drr_qopt_offload *p)
{
	enum pon_qos_qdisc_type type = PON_QOS_QDISC_DRR;

	if (!dev || !port || !p)
		return -EINVAL;

	return pon_qos_add_child_qdisc(dev, port, type, p->parent, p->handle);
}

static int pon_qos_drr_sched_update(struct net_device *dev,
				    struct pon_qos_port *port,
				    struct tc_drr_qopt_offload *parms)
{
	int ret;

	if (!dev || !port || !parms)
		return -EINVAL;

	if (port->root_qdisc.use_cnt && parms->parent == TC_H_ROOT)
		return 0;

	ret = pon_qos_drr_add_child_qdisc(dev, port, parms);
	if (ret < 0) {
		netdev_err(dev, "drr child qdisc create failed\n");
		return ret;
	}

	port->sch_num++;

	return 0;
}

static int pon_qos_drr_queue_update(struct net_device *dev,
				    struct pon_qos_qdisc *sch,
				    struct tc_drr_qopt_offload *parms)
{
	u32 handle = parms->handle;
	int ret, idx = TC_H_MIN(handle) - 1;
	int weight, quantum;

	if (idx < 0 || idx > PON_QOS_TC_MAX_Q - 1) {
		netdev_err(dev, "invalid child minor -> should be in [1-8]\n");
		return -EINVAL;
	}

	if (!sch->use_cnt || TC_H_MAJ(handle) != sch->handle) {
		netdev_err(dev, "invalid sched configuration\n");
		return -EINVAL;
	}

	if (!parms->set_params.quantum) {
		netdev_err(dev, "invalid quantum\n");
		return -EINVAL;
	}

	quantum = parms->set_params.quantum;
	weight = pon_qos_scale_quantum(quantum, 4);
	/* Range provided from OMCI via quantum must be adapted to
	 * hardware range.
	 * PPv4 has range 1..126
	 * OMCI has range 0..255
	 * Calculation for the adaptation:
	 * ppv4_w= round_up ((126-1)/(255-0)* omci_w + 1 )
	 * Normally even values are used, then this will produce
	 * better accuracy in lower range:
	 * ppv4_w= round_up ((126-1)/(255-0)* omci_w + 0 )
	 */
	weight = (125 * weight + 254) / 255;

	ret = pon_qos_queue_add(sch, PON_QOS_QDISC_DRR, weight, idx);

	return ret;
}

static int pon_qos_tc_drr_replace(struct net_device *dev,
				  struct tc_drr_qopt_offload *opt,
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
			netdev_err(dev, "tc-drr port alloc failed\n");
			return -ENOMEM;
		}
		newp = true;
	}

	if (!TC_H_MIN(opt->handle)) {
		netdev_dbg(dev, "%s: Config drr sched\n", __func__);
		port->root_qdisc.port = port_id;
		port->root_qdisc.deq_idx = deq_idx;
		port->root_qdisc.dev = dev;
		ret = pon_qos_get_port_info(&port->root_qdisc);
		if (ret < 0) {
			netdev_err(dev, "tc-drr port info get failed\n");
			goto err_free_port;
		}
		ret = pon_qos_drr_sched_update(dev, port, opt);
		if (ret < 0) {
			netdev_err(dev, "tc-drr sched config failed\n");
			goto err_free_port;
		}
	}

	/* This is a drr class so queue has to be configured on
	 * an existing port.
	 */
	if (TC_H_MIN(opt->handle) && !newp) {
		netdev_dbg(dev, "%s: Config queue\n", __func__);
		qdisc = pon_qos_qdisc_find(port, opt->handle);
		if (!qdisc) {
			netdev_err(dev, "%s: radix tree broken\n", __func__);
			return -ENODEV;
		}
		ret = pon_qos_drr_queue_update(dev, qdisc, opt);
		if (ret < 0) {
			netdev_err(dev, "%s: drr queue config err\n", __func__);
			return ret;
		}
	}

	return 0;

err_free_port:
	if (newp)
		pon_qos_port_delete(port);
	return ret;
}

static int pon_qos_tc_drr_queue_del(struct pon_qos_qdisc *sch, u32 handle)
{
	int idx = TC_H_MIN(handle) - 1;
	int ret;

	/* not a queue */
	if (!TC_H_MIN(handle))
		return 0;

	if (idx < 0 && idx >= PON_QOS_TC_MAX_Q)
		return -EINVAL;

	/* check if this is a queue */
	if (sch->qids[idx].qid) {
		netdev_dbg(sch->dev, "%s: qid:%d deleting\n",
			   __func__, sch->qids[idx].qid);
		ret = pon_qos_queue_del(sch, idx);
		if (ret < 0) {
			netdev_err(sch->dev, "%s: queue del err\n", __func__);
			return ret;
		}
		return 1;
	}

	return 0;
}

static int pon_qos_tc_drr_qdisc_del(struct pon_qos_port *port,
				    struct pon_qos_qdisc *sch)
{
	int ret;

	if (!port || !sch)
		return -EINVAL;

	netdev_dbg(sch->dev, "%s:free sch:%d parent:%#x %#x children:%d/%d\n",
		   __func__,
		   sch->sch_id, sch->parent, sch->handle,
		   sch->num_q, sch->num_children);

	if (TC_H_MIN(sch->handle) || sch->num_q || sch->num_children) {
		netdev_dbg(sch->dev, "%s: empty input or busy sched\n",
			   __func__);
		return 0;
	}

	if (sch->use_cnt) {
		ret = pon_qos_sched_del(sch);
		if (ret < 0) {
			netdev_err(sch->dev, "%s: sch:%d del failed\n",
				   __func__, sch->sch_id);
			return ret;
		}
		port->sch_num--;
		if (sch->parent != TC_H_ROOT && TC_H_MIN(sch->parent)) {
			ret = pon_qos_tc_qdisc_unlink(port, sch);
			if (ret < 0)
				netdev_err(sch->dev, "%s:sch:%d unlink fail\n",
					   __func__, sch->sch_id);
		}
	}

	WARN_ON(radix_tree_delete(&port->qdiscs, TC_H_MAJ(sch->handle)) != sch);
	if (sch->parent != TC_H_ROOT)
		pon_qos_free_qdisc(sch);
	else
		pon_qos_port_delete(port);

	return 0;
}

static int pon_qos_tc_drr_destroy(struct net_device *dev,
				  struct tc_drr_qopt_offload *opt)
{
	struct pon_qos_port *port = NULL;
	struct pon_qos_qdisc *sch = NULL;
	int ret;

	port = pon_qos_port_get(dev);
	if (!port) {
		netdev_dbg(dev, "%s:port not found or deallocated\n", __func__);
		return 0;
	}

	sch = pon_qos_qdisc_find(port, opt->handle);
	if (!sch) {
		netdev_dbg(dev, "%s:sch not found or deallocated\n", __func__);
		return 0;
	}

	ret = pon_qos_tc_drr_queue_del(sch, opt->handle);
	if (ret < 0)
		return ret;

	/* this is a queue no need to do anything further */
	if (ret == 1)
		return 0;

	/*pon_qos_tc_sched_status(port, &port->root_qdisc);*/

	ret = pon_qos_tc_drr_qdisc_del(port, sch);
	if (ret < 0) {
		netdev_err(dev, "%s: %#x id: %d failed\n", __func__,
			   sch->handle, sch->sch_id);
		return ret;
	}

	return 0;
}

int pon_qos_tc_drr_offload(struct net_device *dev,
			   void *type_data,
			   int port_id,
			   int deq_idx)
{
	int err = 0;
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	struct tc_drr_qopt_offload *opt =
		((struct tc_to_netdev *)type_data)->sch_drr;
#else
	struct tc_drr_qopt_offload *opt =
		(struct tc_drr_qopt_offload *)type_data;
#endif

	ASSERT_RTNL();
	netdev_dbg(dev, "DRR: offload starting\n");

	trace_pon_qos_tc_drr_enter(dev, opt);
	switch (opt->command) {
	case TC_DRR_REPLACE:
		netdev_dbg(dev, "pid: %#x class/handle: %#x quantum: %#x\n",
			   opt->parent, opt->handle, opt->set_params.quantum);
		err = pon_qos_tc_drr_replace(dev, opt,
					     port_id, deq_idx);
		if (err < 0) {
			netdev_err(dev, "tc-drr replace failed\n");
			return err;
		}
		break;
	case TC_DRR_DESTROY:
		netdev_dbg(dev,
			   "destroy pid: %#x class/handle: %#x quantum: %#x\n",
			   opt->parent, opt->handle, opt->set_params.quantum);
		err = pon_qos_tc_drr_destroy(dev, opt);
		if (err < 0) {
			netdev_err(dev, "tc-drr destroy failed\n");
			return err;
		}

		break;
	case TC_DRR_STATS:
		/* TODO */
		return -EOPNOTSUPP;
	case TC_DRR_GRAFT:
		/* TODO */
		return -EOPNOTSUPP;
	default:
		break;
	}
	trace_pon_qos_tc_drr_exit(dev, opt);

	netdev_dbg(dev, "DRR: offload end: %d\n", err);
	return err;
}
