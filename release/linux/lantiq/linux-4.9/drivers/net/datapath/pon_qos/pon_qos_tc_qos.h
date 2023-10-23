/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#ifndef _PON_QOS_TC_QOS_
#define _PON_QOS_TC_QOS_

#include <net/pkt_cls.h>
#include <linux/list.h>
#include <linux/radix-tree.h>
#include <linux/types.h>

#define PON_QOS_TC_MAX_Q 8
#define PON_QOS_DOT1P_SZ 8
#define PON_QOS_SCH_MAX_LEVEL 3
#define PON_QOS_8021P_HIGHEST_PRIO 7
#define PON_QOS_UNUSED -1
#define PON_QOS_CPU_PORT 0

#define PON_QOS_TC_COOKIE_EMPTY -1

enum pon_qos_qdisc_type {
	PON_QOS_QDISC_NONE,
	PON_QOS_QDISC_PRIO,
	PON_QOS_QDISC_DRR,
	PON_QOS_QDISC_TBF,
	PON_QOS_QDISC_RED,
};

struct pon_qos_qdisc_ops {
	int (*offload)(struct net_device *dev, void *params);

	int (*unoffload)(struct net_device *dev, void *params);
};

enum pon_qos_qdata_type {
	PON_QOS_QDATA_GREEN,
	PON_QOS_QDATA_YELLOW,
	PON_QOS_QDATA_TBF,
};

struct pon_qos_qdata_params {
	u32 handle;
	u32 parent;
	enum pon_qos_qdata_type type;
	struct pon_qos_q_data *qid;
	int (*destroy)(struct net_device *dev, u32 handle, u32 parent);
	struct list_head list;
};

struct pon_qos_q_data {
	u32 qid;
	int tc;
	atomic_t ref_cnt;
	int arbi;
	int p_w;
	struct list_head params;
};

struct pon_qos_dot1p {
	bool en;
	unsigned int tcid;
};

struct pon_qos_qdisc {
	struct net_device *dev;
	enum pon_qos_qdisc_type type;
	u32 parent;
	u32 handle;
	int use_cnt;

	/* HW specific settings */
	u32 sch_id;
	int inst;
	s32 port;
	s32 deq_idx;
	int epn;
	u16 def_q;
	bool ds;
	struct pon_qos_dot1p dot1p[PON_QOS_DOT1P_SZ];

	unsigned int num_q;
	struct pon_qos_q_data qids[PON_QOS_TC_MAX_Q];

	/* TODO: for multi-stage schedulers */
	unsigned int num_children;
	struct pon_qos_qdisc *children[PON_QOS_TC_MAX_Q];
	/* Used for sibling qdiscs */
	int p_w;

	union {
		struct {
			u8 bands;
			u8 priomap[TC_PRIO_MAX + 1];
		} prio;

		struct {
			u8 num_tc;
			u8 priomap[TC_PRIO_MAX + 1];
		} mqprio;

		struct {
			int classes;
		} drr;
	};

	/* TODO: abstrac the hw specific part */
	struct pon_qos_qdisc_ops *ops;
};

struct pon_qos_port {
	struct net_device *dev;
	struct pon_qos_qdisc root_qdisc;
	unsigned int sch_num;
	struct radix_tree_root qdiscs;
	struct list_head list;
};

struct pon_qos_qmap_tc {
	int flags;
	u32 handle;
	struct net_device *indev;
	int tc;
	char tc_cookie;
	bool ingress;
};

int pon_qos_get_queue_by_handle(struct net_device *dev,
				u32 handle,
				struct pon_qos_q_data **qid);

struct pon_qos_port *pon_qos_port_get(struct net_device *dev);
struct pon_qos_port *pon_qos_port_alloc(struct net_device *dev);
int pon_qos_port_delete(struct pon_qos_port *port);
int pon_qos_alloc_qdisc(struct pon_qos_qdisc **qdisc);
void pon_qos_free_qdisc(struct pon_qos_qdisc *qdisc);
struct pon_qos_qdisc *pon_qos_qdisc_find(struct pon_qos_port *port, u32 handle);

int pon_qos_queue_add(struct pon_qos_qdisc *sch, int arbi, int prio_w, int idx);
int pon_qos_queue_del(struct pon_qos_qdisc *sch, int idx);

int pon_qos_qos_init(void);
int pon_qos_qos_destroy(void);

int pon_qos_add_sched(struct pon_qos_qdisc *sch, int prio);
int pon_qos_add_staged_sched(struct pon_qos_qdisc *psch,
			     struct pon_qos_qdisc *csch,
			     int prio);
int pon_qos_sched_del(struct pon_qos_qdisc *sch);
int pon_qos_add_child_qdisc(struct net_device *dev,
			    struct pon_qos_port *port,
			    enum pon_qos_qdisc_type type,
			    u32 parent,
			    u32 hanle);

int pon_qos_fill_port_data(struct pon_qos_qdisc *sch);
int pon_qos_get_port_info(struct pon_qos_qdisc *sch);

int pon_qos_tc_qdisc_unlink(struct pon_qos_port *p, struct pon_qos_qdisc *sch);
int pon_qos_qdisc_tree_del(struct pon_qos_port *p, struct pon_qos_qdisc *root);
int pon_qos_tc_sched_status(struct pon_qos_port *p, struct pon_qos_qdisc *root);

int pon_qos_update_qmap(struct net_device *dev,
			struct pon_qos_qmap_tc *q_tc,
			bool en);
int pon_qos_ev_tc_assign(struct pon_qos_qdisc *sch, u8 tc);
int pon_qos_ev_tc_unassign(struct pon_qos_qdisc *sch, u8 tc);
int pon_qos_tc_mappings_init(void);

int pon_qos_ports_cleanup(void);

int pon_qos_qdata_get_by_type(struct net_device *dev,
			      struct pon_qos_q_data *qid,
			      enum pon_qos_qdata_type type,
			      u32 *handle, u32 *parent);
struct pon_qos_q_data *pon_qos_qdata_qid_get(struct net_device *dev,
					     struct pon_qos_qdisc *qdisc,
					     u32 parent);
int pon_qos_qdata_add(struct net_device *dev, struct pon_qos_q_data *qid,
		      u32 handle, u32 parent, enum pon_qos_qdata_type type,
		      int (*destroy)(struct net_device *dev, u32 handle,
				     u32 parent));
int pon_qos_qdata_remove(struct net_device *dev, struct pon_qos_q_data *qid,
			 u32 handle, u32 parent);

int pon_qos_shaper_add(struct pon_qos_qdisc *sch,
		       struct pon_qos_q_data *qid,
		       struct tc_tbf_qopt_offload_params *params);
int pon_qos_shaper_remove(struct pon_qos_qdisc *sch,
			  struct pon_qos_q_data *qid);

bool pon_qos_is_cpu_port(int port);

bool pon_qos_is_netdev_cpu_port(struct net_device *dev);

#endif
