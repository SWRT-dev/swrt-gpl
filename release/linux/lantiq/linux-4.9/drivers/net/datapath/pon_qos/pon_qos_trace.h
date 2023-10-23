/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 *
 *****************************************************************************/
#undef TRACE_SYSTEM
#define TRACE_SYSTEM pon_qos_tc

#if !defined(_PON_QOS_TRACE_H_) || defined(TRACE_HEADER_MULTI_READ)
#define _PON_QOS_TRACE_H_

#include <linux/tracepoint.h>
#include <net/pkt_cls.h>

#include "pon_qos_tc_qos.h"
#include "pon_qos_tc_flower.h"

/* create empty functions when tracing is disabled */
#if !defined(CONFIG_PON_QOS_TRACING)
#undef TRACE_EVENT
#define TRACE_EVENT(name, proto, ...) \
static inline void trace_ ## name(proto) {}
#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(...)
#undef DEFINE_EVENT
#define DEFINE_EVENT(evt_class, name, proto, ...) \
static inline void trace_ ## name(proto) {}
#endif

DECLARE_EVENT_CLASS(pon_qos_tc_class,
	TP_PROTO(const struct net_device *dev, const unsigned int type),

	TP_ARGS(dev, type),

	TP_STRUCT__entry(
		__string(name, dev->name)
		__field(unsigned int, type)
	),

	TP_fast_assign(
		__assign_str(name, dev->name);
		__entry->type = type;
	),

	TP_printk("[%s] -> type: %u", __get_str(name), __entry->type)
);

DEFINE_EVENT(pon_qos_tc_class, pon_qos_setup_tc_enter,
	TP_PROTO(const struct net_device *dev, const unsigned int type),
	TP_ARGS(dev, type)
);

DEFINE_EVENT(pon_qos_tc_class, pon_qos_setup_tc_exit,
	TP_PROTO(const struct net_device *dev, const unsigned int type),
	TP_ARGS(dev, type)
);

#define pon_qos_tc_offload_type				\
	{ TC_TYPE_EXT_VLAN, "TC_TYPE_EXT_VLAN" },	\
	{ TC_TYPE_VLAN_FILTER, "TC_TYPE_VLAN_FILTER" }, \
	{ TC_TYPE_QUEUE, "TC_TYPE_QUEUE" },		\
	{ TC_TYPE_MIRRED, "TC_TYPE_MIRRED" },		\
	{ TC_TYPE_POLICE, "TC_TYPE_POLICE" },		\
	{ TC_TYPE_COLMARK, "TC_TYPE_COLMARK" },		\
	{ TC_TYPE_TRAP, "TC_TYPE_TRAP" },		\
	{ TC_TYPE_IP_DROP, "TC_TYPE_DROP" },		\
	{ -EOPNOTSUPP, "TC_TYPE_UNKNOWN"  }

DECLARE_EVENT_CLASS(pon_qos_tc_flower_class,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_cls_flower_offload *f,
		 int type),

	TP_ARGS(dev, f, type),

	TP_STRUCT__entry(
		__string(name, dev->name)
		__field(u8, command)
		__field(unsigned long, cookie)
		__field(unsigned int, classid)
		__field(int, type)
	),

	TP_fast_assign(
		__assign_str(name, dev->name);
		__entry->command = f->command;
		__entry->cookie = f->cookie;
		__entry->classid = f->classid;
		__entry->type = type;
	),

	TP_printk("[%s] -> cmd: %#x cookie: %#lx classid: %#x type: %s",
		  __get_str(name), __entry->command,
		  __entry->cookie, __entry->classid,
		  __print_symbolic(__entry->type, pon_qos_tc_offload_type))
);

DEFINE_EVENT(pon_qos_tc_flower_class, pon_qos_tc_flower_enter,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_cls_flower_offload *f,
		 int type),
	TP_ARGS(dev, f, type)
);

DEFINE_EVENT(pon_qos_tc_flower_class, pon_qos_tc_flower_exit,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_cls_flower_offload *f,
		 int type),
	TP_ARGS(dev, f, type)
);

DECLARE_EVENT_CLASS(pon_qos_tc_drr_class,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_drr_qopt_offload *opt),

	TP_ARGS(dev, opt),

	TP_STRUCT__entry(
		__string(name, dev->name)
		__field(u8, command)
		__field(u32, handle)
		__field(u32, parent)
		__field(int, quantum)
	),

	TP_fast_assign(
		__assign_str(name, dev->name);
		__entry->command = opt->command;
		__entry->handle = opt->handle;
		__entry->parent = opt->parent;
		__entry->quantum = (opt->command == TC_DRR_STATS) ?
				    0 : opt->set_params.quantum;
	),

	TP_printk("[%s] -> cmd: %#x parent: %#x handle: %#x quantum: %d",
		  __get_str(name), __entry->command,
		  __entry->parent, __entry->handle,
		  __entry->quantum)
);

DEFINE_EVENT(pon_qos_tc_drr_class, pon_qos_tc_drr_enter,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_drr_qopt_offload *opt),
	TP_ARGS(dev, opt)
);

DEFINE_EVENT(pon_qos_tc_drr_class, pon_qos_tc_drr_exit,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_drr_qopt_offload *opt),
	TP_ARGS(dev, opt)
);

DECLARE_EVENT_CLASS(pon_qos_tc_prio_class,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_prio_qopt_offload *opt),

	TP_ARGS(dev, opt),

	TP_STRUCT__entry(
		__string(name, dev->name)
		__field(u8, command)
		__field(u32, handle)
		__field(u32, parent)
		__field(int, bands)
	),

	TP_fast_assign(
		__assign_str(name, dev->name);
		__entry->command = opt->command;
		__entry->handle = opt->handle;
		__entry->parent = opt->parent;
		__entry->bands = (opt->command == TC_PRIO_REPLACE) ?
				  opt->replace_params.bands :
				 (opt->command != TC_PRIO_GRAFT) ?
				  0 : opt->graft_params.band;
	),

	TP_printk("[%s] -> cmd:%#x parent:%#x handle:%#x band(s):%#x",
		  __get_str(name), __entry->command,
		  __entry->parent, __entry->handle,
		  __entry->bands)
);

DEFINE_EVENT(pon_qos_tc_prio_class, pon_qos_tc_prio_enter,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_prio_qopt_offload *opt),
	TP_ARGS(dev, opt)
);

DEFINE_EVENT(pon_qos_tc_prio_class, pon_qos_tc_prio_exit,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_prio_qopt_offload *opt),
	TP_ARGS(dev, opt)
);

DECLARE_EVENT_CLASS(pon_qos_tc_tbf_class,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_tbf_qopt_offload *opt),

	TP_ARGS(dev, opt),

	TP_STRUCT__entry(
		__string(name, dev->name)
		__field(u8, command)
		__field(u32, handle)
		__field(u32, parent)
		__field(u64, rate)
		__field(u64, prate)
		__field(u32, burst)
		__field(u32, pburst)
	),

	TP_fast_assign(
		__assign_str(name, dev->name);
		__entry->command = opt->command;
		__entry->handle = opt->handle;
		__entry->parent = opt->parent;
		__entry->rate = (opt->command == TC_TBF_REPLACE) ?
				 opt->set_params.rate : 0;
		__entry->prate = (opt->command == TC_TBF_REPLACE) ?
				     opt->set_params.prate : 0;
		__entry->burst = (opt->command == TC_TBF_REPLACE) ?
				  opt->set_params.burst : 0;
		__entry->pburst = (opt->command == TC_TBF_REPLACE) ?
				   opt->set_params.pburst : 0;
	),

	TP_printk("[%s] -> cmd:%#x parent:%#x handle:%#x rate:%llu peakrate:%llu burst:%u pburst:%u",
		  __get_str(name), __entry->command,
		  __entry->parent, __entry->handle,
		  __entry->rate, __entry->prate,
		  __entry->burst, __entry->pburst)
);

DEFINE_EVENT(pon_qos_tc_tbf_class, pon_qos_tc_tbf_enter,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_tbf_qopt_offload *opt),
	TP_ARGS(dev, opt)
);

DEFINE_EVENT(pon_qos_tc_tbf_class, pon_qos_tc_tbf_exit,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_tbf_qopt_offload *opt),
	TP_ARGS(dev, opt)
);

DECLARE_EVENT_CLASS(pon_qos_tc_red_class,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_red_qopt_offload *opt),

	TP_ARGS(dev, opt),

	TP_STRUCT__entry(
		__string(name, dev->name)
		__field(u8, command)
		__field(u32, handle)
		__field(u32, parent)
	),

	TP_fast_assign(
		__assign_str(name, dev->name);
		__entry->command = opt->command;
		__entry->handle = opt->handle;
		__entry->parent = opt->parent;
	),

	TP_printk("[%s] -> cmd:%#x parent:%#x handle:%#x",
		  __get_str(name), __entry->command,
		  __entry->parent, __entry->handle)
);

DEFINE_EVENT(pon_qos_tc_red_class, pon_qos_tc_red_enter,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_red_qopt_offload *opt),
	TP_ARGS(dev, opt)
);

DEFINE_EVENT(pon_qos_tc_red_class, pon_qos_tc_red_exit,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_red_qopt_offload *opt),
	TP_ARGS(dev, opt)
);

DECLARE_EVENT_CLASS(pon_qos_sched_class,
	TP_PROTO(const struct pon_qos_qdisc *sch, u32 pid),

	TP_ARGS(sch, pid),

	TP_STRUCT__entry(
		__string(name, sch->dev->name)
		__field(u32, handle)
		__field(u32, parent)
		__field(u32, sch_id)
		__field(int, p_w)
		__field(s32, port)
		__field(int, epn)
		__field(u16, def_q)
		__field(int, use_cnt)
		__field(unsigned int, num_q)
		__field(unsigned int, num_children)
		__field(u32, pid)
	),

	TP_fast_assign(
		__assign_str(name, sch->dev->name);
		__entry->handle = sch->handle;
		__entry->parent = sch->handle;
		__entry->sch_id = sch->sch_id;
		__entry->p_w = sch->p_w;
		__entry->port = sch->port;
		__entry->epn = sch->epn;
		__entry->def_q = sch->def_q;
		__entry->use_cnt = sch->use_cnt;
		__entry->num_q = sch->num_q;
		__entry->num_children = sch->num_children;
		__entry->pid = pid;
	),

	TP_printk("[%s] hdls:%#x/%#x id:%u p_w:%d port:%i epn:%i def_q:%u psch_id: %u use_cnt: %d num_q: %u children: %u",
		  __get_str(name), __entry->parent, __entry->handle,
		  __entry->sch_id, __entry->p_w, __entry->port,
		  __entry->epn, __entry->def_q, __entry->pid,
		  __entry->use_cnt, __entry->num_q, __entry->num_children)
);

DEFINE_EVENT(pon_qos_sched_class, pon_qos_add_sched_enter,
	TP_PROTO(const struct pon_qos_qdisc *sch, u32 pid),
	TP_ARGS(sch, pid)
);

DEFINE_EVENT(pon_qos_sched_class, pon_qos_add_sched_exit,
	TP_PROTO(const struct pon_qos_qdisc *sch, u32 pid),
	TP_ARGS(sch, pid)
);

DEFINE_EVENT(pon_qos_sched_class, pon_qos_sched_del_enter,
	TP_PROTO(const struct pon_qos_qdisc *sch, u32 pid),
	TP_ARGS(sch, pid)
);

DEFINE_EVENT(pon_qos_sched_class, pon_qos_sched_del_exit,
	TP_PROTO(const struct pon_qos_qdisc *sch, u32 pid),
	TP_ARGS(sch, pid)
);

DECLARE_EVENT_CLASS(pon_qos_queue_class,
	TP_PROTO(const struct pon_qos_qdisc *sch,
		 const struct pon_qos_q_data *q,
		 int idx),

	TP_ARGS(sch, q, idx),

	TP_STRUCT__entry(
		__string(name, sch->dev->name)
		__field(u32, handle)
		__field(u32, parent)
		__field(u32, sch_id)
		__field(u32, qid)
		__field(int, arbi)
		__field(int, p_w)
		__field(int, idx)
	),

	TP_fast_assign(
		__assign_str(name, sch->dev->name);
		__entry->handle = sch->handle;
		__entry->parent = sch->handle;
		__entry->sch_id = sch->sch_id;
		__entry->qid = q->qid;
		__entry->arbi = q->arbi;
		__entry->p_w = q->p_w;
		__entry->idx = idx;
	),

	TP_printk("[%s] hdls:%#x/%#x sch_id:%u qid:%u arbi:%i p_w:%i idx: %i",
		  __get_str(name), __entry->parent, __entry->handle,
		  __entry->sch_id, __entry->qid, __entry->arbi,
		  __entry->p_w, __entry->idx)
);

DEFINE_EVENT(pon_qos_queue_class, pon_qos_queue_add_enter,
	TP_PROTO(const struct pon_qos_qdisc *sch,
		 const struct pon_qos_q_data *q,
		 int idx),
	TP_ARGS(sch, q, idx)
);

DEFINE_EVENT(pon_qos_queue_class, pon_qos_queue_add_exit,
	TP_PROTO(const struct pon_qos_qdisc *sch,
		 const struct pon_qos_q_data *q,
		 int idx),
	TP_ARGS(sch, q, idx)
);

DEFINE_EVENT(pon_qos_queue_class, pon_qos_queue_del_enter,
	TP_PROTO(const struct pon_qos_qdisc *sch,
		 const struct pon_qos_q_data *q,
		 int idx),
	TP_ARGS(sch, q, idx)
);

DEFINE_EVENT(pon_qos_queue_class, pon_qos_queue_del_exit,
	TP_PROTO(const struct pon_qos_qdisc *sch,
		 const struct pon_qos_q_data *q,
		 int idx),
	TP_ARGS(sch, q, idx)
);

DECLARE_EVENT_CLASS(pon_qos_update_qmap_class,
	TP_PROTO(const struct net_device *dev,
		 const struct pon_qos_qdisc *sch,
		 const struct pon_qos_q_data *q,
		 const struct pon_qos_qmap_tc *tc,
		 bool en),

	TP_ARGS(dev, sch, q, tc, en),

	TP_STRUCT__entry(
		__string(name, dev->name)
		__field(s32, port)
		__field(int, epn)
		__field(u8, ds)
		__field(u8, en)
		__field(u32, qid)
		__field(u32, classid)
		__field(int, tc)
		__field(char, tc_cookie)
		__string(indev, tc->indev->name)
	),

	TP_fast_assign(
		__assign_str(name, dev->name);
		__entry->port = sch->port;
		__entry->epn = sch->epn;
		__entry->ds = sch->ds;
		__entry->en = en;
		__entry->qid = q->qid;
		__entry->classid = tc->handle;
		__entry->tc = tc->tc;
		__entry->tc_cookie = tc->tc_cookie;
		__assign_str(indev, tc->indev ? tc->indev->name : "x");
	),

	TP_printk("[%s] en:%u ds:%u port:%i epn:%i q:%u cls:%#x tc:%i tc_cookie:%#x indev:%s",
		  __get_str(name), __entry->en, __entry->ds,
		  __entry->port, __entry->epn, __entry->qid,
		  __entry->classid, __entry->tc, __entry->tc_cookie,
		  __get_str(indev))
);

DEFINE_EVENT(pon_qos_update_qmap_class, pon_qos_update_qmap_enter,
	TP_PROTO(const struct net_device *dev,
		 const struct pon_qos_qdisc *sch,
		 const struct pon_qos_q_data *q,
		 const struct pon_qos_qmap_tc *tc,
		 bool en),
	TP_ARGS(dev, sch, q, tc, en)
);

DEFINE_EVENT(pon_qos_update_qmap_class, pon_qos_update_qmap_exit,
	TP_PROTO(const struct net_device *dev,
		 const struct pon_qos_qdisc *sch,
		 const struct pon_qos_q_data *q,
		 const struct pon_qos_qmap_tc *tc,
		 bool en),
	TP_ARGS(dev, sch, q, tc, en)
);

TRACE_EVENT(pon_qos_tc_red_replace,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_red_qopt_offload *opt,
		 const struct pon_qos_qdisc *sch,
		 const struct pon_qos_q_data *q,
		 int type),

	TP_ARGS(dev, opt, sch, q, type),

	TP_STRUCT__entry(
		__string(name, dev->name)
		__field(u32, handle)
		__field(u32, parent)
		__field(u32, port)
		__field(u32, sch_id)
		__field(u32, qid)
		__field(int, type)
		__field(u32, min)
		__field(u32, max)
		__field(u64, probability)
	),

	TP_fast_assign(
		__assign_str(name, dev->name);
		__entry->handle = opt->handle;
		__entry->parent = opt->parent;
		__entry->port = sch->port;
		__entry->sch_id = sch->sch_id;
		__entry->qid = q->qid;
		__entry->type = type;
		__entry->min = opt->set.min;
		__entry->max = opt->set.max;
		__entry->probability = opt->set.probability * 100;
	),

	TP_printk("[%s] hdls:%#x/%#x port:%u sch_id:%u qid:%u col:%s min:%u max:%u probability:%llu",
		  __get_str(name), __entry->parent, __entry->handle,
		  __entry->port, __entry->sch_id, __entry->qid,
		  __entry->type == PON_QOS_QDATA_GREEN ? "green" : "yellow",
		  __entry->min, __entry->max,
		  DIV_ROUND_UP(
		  DIV_ROUND_UP(__entry->probability, 1 << 16), 1 << 16))
);

TRACE_EVENT(pon_qos_tc_red_destroy,
	TP_PROTO(const struct net_device *dev,
		 const struct tc_red_qopt_offload *opt,
		 const struct pon_qos_qdisc *sch,
		 const struct pon_qos_q_data *q),

	TP_ARGS(dev, opt, sch, q),

	TP_STRUCT__entry(
		__string(name, dev->name)
		__field(u32, handle)
		__field(u32, parent)
		__field(u32, port)
		__field(u32, sch_id)
		__field(u32, qid)
	),

	TP_fast_assign(
		__assign_str(name, dev->name);
		__entry->handle = opt->handle;
		__entry->parent = opt->parent;
		__entry->port = sch->port;
		__entry->sch_id = sch->sch_id;
		__entry->qid = q->qid;
	),

	TP_printk("[%s] hdls:%#x/%#x port:%u sch_id:%u qid:%u",
		  __get_str(name), __entry->parent, __entry->handle,
		  __entry->port, __entry->sch_id, __entry->qid)
);

DECLARE_EVENT_CLASS(pon_qos_tc_ext_vlan_class,
	TP_PROTO(const struct net_device *dev,
		 const void *ptr, int tags, int cnt,
		 bool ingress),

	TP_ARGS(dev, ptr, tags, cnt, ingress),

	TP_STRUCT__entry(
		__string(name, dev->name)
		__field(const void *, ptr)
		__field(bool, ingress)
		__field(int, tags)
		__field(int, cnt)
	),

	TP_fast_assign(
		__assign_str(name, dev->name);
		__entry->ptr = ptr;
		__entry->ingress = ingress;
		__entry->tags = tags;
		__entry->cnt = cnt;
	),

	TP_printk("[%s] -> node: %p %s tags:%i rules:%i",
		  __get_str(name), __entry->ptr,
		  __entry->ingress ? "ingress" : "egress",
		  __entry->tags, __entry->cnt)
);

DEFINE_EVENT(pon_qos_tc_ext_vlan_class, ev_add_enter,
	TP_PROTO(const struct net_device *dev,
		 const void *ptr, int tags, int cnt,
		 bool ingress),
	TP_ARGS(dev, ptr, tags, cnt, ingress)
);

DEFINE_EVENT(pon_qos_tc_ext_vlan_class, ev_add_exit,
	TP_PROTO(const struct net_device *dev,
		 const void *ptr, int tags, int cnt,
		 bool ingress),
	TP_ARGS(dev, ptr, tags, cnt, ingress)
);

DEFINE_EVENT(pon_qos_tc_ext_vlan_class, ev_del_enter,
	TP_PROTO(const struct net_device *dev,
		 const void *ptr, int tags, int cnt,
		 bool ingress),
	TP_ARGS(dev, ptr, tags, cnt, ingress)
);

DEFINE_EVENT(pon_qos_tc_ext_vlan_class, ev_del_exit,
	TP_PROTO(const struct net_device *dev,
		 const void *ptr, int tags, int cnt,
		 bool ingress),
	TP_ARGS(dev, ptr, tags, cnt, ingress)
);

DEFINE_EVENT(pon_qos_tc_ext_vlan_class, vf_add_enter,
	TP_PROTO(const struct net_device *dev,
		 const void *ptr, int tags, int cnt,
		 bool ingress),
	TP_ARGS(dev, ptr, tags, cnt, ingress)
);

DEFINE_EVENT(pon_qos_tc_ext_vlan_class, vf_add_exit,
	TP_PROTO(const struct net_device *dev,
		 const void *ptr, int tags, int cnt,
		 bool ingress),
	TP_ARGS(dev, ptr, tags, cnt, ingress)
);

DEFINE_EVENT(pon_qos_tc_ext_vlan_class, vf_del_enter,
	TP_PROTO(const struct net_device *dev,
		 const void *ptr, int tags, int cnt,
		 bool ingress),
	TP_ARGS(dev, ptr, tags, cnt, ingress)
);

DEFINE_EVENT(pon_qos_tc_ext_vlan_class, vf_del_exit,
	TP_PROTO(const struct net_device *dev,
		 const void *ptr, int tags, int cnt,
		 bool ingress),
	TP_ARGS(dev, ptr, tags, cnt, ingress)
);

#endif /* __PON_QOS_DEVICE_TRACE_DATA */

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE pon_qos_trace

#include <trace/define_trace.h>
