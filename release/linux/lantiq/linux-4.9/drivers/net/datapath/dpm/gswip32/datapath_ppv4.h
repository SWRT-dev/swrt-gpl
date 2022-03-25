/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef DATAPATH_PPV4_H
#define DATAPATH_PPV4_H

#define MAX_PPV4_PORT 256
#define MAX_CQM_DEQ   138
#define MAX_QUEUE   512
#define MAX_PP_CHILD_PER_NODE  8 /* Maximum queue per scheduler */
#define MAX_Q_PER_PORT 32 /* Maximum queue per port */
#define QOS_MAX_NODES  2048 /* Maximum PPV4 nodes. ?? need further check */
#define MAX_SCHD 128 /* Need further check ?? need further check*/
#define INV_RESV_IDX 0xFFFF  /* Invalid reserved resource index */
#define MAX_LOOKUP_TBL_SIZE (8 * 1024) /* lookup table size:12bits + 1bit eg */
#define DEF_QRED_MAX_ALLOW 0x400  /* max qocc in queue */
#define DEF_QRED_MIN_ALLOW 0x40 /* minqocc in queue */
#define DEF_QRED_SLOP_GREEN 30 /* green slop in queue */
#define DEF_QRED_SLOP_YELLOW 70 /* yellow slop in queue */
#define DEF_WRED_RATIO       5

#define HAL(inst) ((struct hal_priv *)dp_port_prop[inst].priv_hal)
#define PARENT(x) (x.queue_child_prop.parent)
#define PARENT_S(x) (x.sched_child_prop.parent)
#define CHILD(x, idx) (priv->qos_sch_stat[x].child[idx])
#define DP_PORT(p) (dp_deq_port_tbl[p->inst][p->cqm_deq_port.cqm_deq_port])

enum flag {
	DP_NODE_DEC = BIT(0), /* flag to reduce node counter */
	DP_NODE_INC = BIT(1), /* flag to increase node counter */
	DP_NODE_RST = BIT(2), /* flag to reset node counter */
	C_FLAG = BIT(8), /* scheduler flag linked to node */
	P_FLAG = BIT(9) /* scheduler flag linked to parent */
};

struct ppv4_queue {
	int inst;  /* dp instance */
	u16 qid;  /* -1 means dynammic, otherwise already configured */
	u16 node_id; /*output */
	u16 sch;  /* -1 means dynammic, otherwise already configured */
	u16 parent; /* -1 means no parent.
		     * it is used for shared dropping queueu purpose
		     */
};

struct ppv4_scheduler {
	u16 sch;  /* -1 means dynammic, otherwise already configured */
	u16 parent; /* input */
	u16 node_id; /* output */
};

struct ppv4_port {
	int inst;
	u16 dp_port;
	u16 qos_deq_port; /* -1 means dynammic, otherwise already specified.
			   * Remove in new datapath lib
			   */
	u16 cqm_deq_port;  /* rename in new datapath lib */
	u16 node_id; /* output */

	u32 tx_pkt_credit;  /* PP port tx bytes credit */
	void *tx_ring_addr;  /* PP port ring address */
	void *tx_ring_addr_push;  /* PP port ring address */
	u32 tx_ring_size; /* PP ring size */
};

struct ppv4_q_sch_port {
	/* input */
	int inst;
	int dp_port; /* for storing q/scheduler */
	int ctp; /* for storing q/scheduler: masked subifid. */
	u32 cqe_deq; /* CQE dequeue port */
	u32 tx_pkt_credit;  /* PP port tx bytes credit */
	void *tx_ring_addr;  /* PP port ring address. */
	void *tx_ring_addr_push;  /* PP port ring address. */
	u32 tx_ring_size; /* PP ring size */

	/* output of PP */
	u32 qid;
	u32 q_node;
	u32 schd_node;
	u32 port_node; /* qos port node id */
	u32 f_deq_port_en:1; /* flag to trigger cbm_dp_enable */
};

struct pp_sch_list {
	u32 flag:1; /* 0: valid 1-used 2-reserved */
	u16 node;
	u16 parent_type;  /* scheduler/port */
	u16 parent;
};

enum PP_NODE_STAT {
	PP_NODE_FREE = 0, /* Free and not allocated yet */
	PP_NODE_ALLOC = BIT(0), /* allocated */
	PP_NODE_ACTIVE = BIT(1), /* linked */
	PP_NODE_RESERVE = BIT(2) /* reserved */
};

struct pp_node {
	enum PP_NODE_STAT flag; /* 0: FREE 1-used/alloc */
	u16 type; /* node type */
	u16 node_id;  /* node id */
};

struct pp_queue_stat {
	enum PP_NODE_STAT flag; /* 0: valid 1-used 2-reserved */
	u16 deq_port; /* cqm dequeue port id */
	u16 node_id;  /* queue node id */
	u16 resv_idx; /* index of reserve table */
	u16 dp_port; /* datapath port id */
};

struct pp_sch_stat {
	enum PP_NODE_STAT c_flag; /* sch flag linked to child */
	enum PP_NODE_STAT p_flag; /* sch flag linked to parent */
	u16 resv_idx; /* index of reserve table */
	struct pp_node child[MAX_PP_CHILD_PER_NODE];
	u16 child_num; /* Number of child */
	int type; /* Node type for queue/sch/port:
		   * sched table is not just for scheduler, also for queue/port
		   * It is table index is based on logical node id,
		   * not just scheduler id
		   */
	struct pp_node parent; /* valid for node type queue/sch */
	u16 dp_port; /* datapath port id */
};

struct cqm_deq_stat {
	enum PP_NODE_STAT flag; /* 0: valid 1-used 2-reserved */
	u16 deq_id; /* qos dequeue port physical id. Maybe no need */
	u16 node_id; /* qos dequeue port's node id */
	u16 child_num; /* Number of child */
};

struct limit_map {
	int pp_limit; /* pp shaper limit */
	int dp_limit; /* dp shaper limit */
};

struct arbi_map {
	int pp_arbi; /* pp arbitrate */
	int dp_arbi; /* dp arbitrate */
};

struct dp_lookup_entry {
	int entry[MAX_LOOKUP_TBL_SIZE];
	int num; /* num of valid lookup entries save in entry[] array */
};

void init_qos_fn_32(void);
extern int (*qos_queue_remove_32)(struct pp_qos_dev *qos_dev, unsigned int id);
extern int (*qos_queue_allocate_32)(struct pp_qos_dev *qos_dev,
				    unsigned int *id);
extern int (*qos_queue_info_get_32)(struct pp_qos_dev *qos_dev, unsigned int id,
				    struct pp_qos_queue_info *info);
extern int (*qos_port_remove_32)(struct pp_qos_dev *qos_dev, unsigned int id);
extern int (*qos_sched_allocate_32)(struct pp_qos_dev *qos_dev,
				    unsigned int *id);
extern int (*qos_sched_remove_32)(struct pp_qos_dev *qos_dev, unsigned int id);
extern int (*qos_port_allocate_32)(struct pp_qos_dev *qos_dev,
				   unsigned int physical_id, unsigned int *id);
extern int (*qos_port_set_32)(struct pp_qos_dev *qos_dev, unsigned int id,
			      const struct pp_qos_port_conf *conf);
extern void (*qos_port_conf_set_default_32)(struct pp_qos_port_conf *conf);
extern void (*qos_queue_conf_set_default_32)(struct pp_qos_queue_conf *conf);
extern int (*qos_queue_set_32)(struct pp_qos_dev *qos_dev, unsigned int id,
			       const struct pp_qos_queue_conf *conf);
extern void (*qos_sched_conf_set_default_32)(struct pp_qos_sched_conf *conf);
extern int (*qos_sched_set_32)(struct pp_qos_dev *qos_dev, unsigned int id,
			       const struct pp_qos_sched_conf *conf);
extern int (*qos_queue_conf_get_32)(struct pp_qos_dev *qos_dev, unsigned int id,
				    struct pp_qos_queue_conf *conf);
extern int (*qos_queue_flush_32)(struct pp_qos_dev *qos_dev, unsigned int id);
extern int (*qos_sched_conf_get_32)(struct pp_qos_dev *qos_dev, unsigned int id,
				    struct pp_qos_sched_conf *conf);
extern int (*qos_sched_get_queues_32)(struct pp_qos_dev *qos_dev,
				      unsigned int id, u16 *queue_ids,
				      unsigned int size,
				      unsigned int *queues_num);
extern int (*qos_port_get_queues_32)(struct pp_qos_dev *qos_dev,
				     unsigned int id, u16 *queue_ids,
				     unsigned int size,
				     unsigned int *queues_num);
extern int (*qos_port_conf_get_32)(struct pp_qos_dev *qdev, unsigned int id,
				   struct pp_qos_port_conf *conf);
extern int (*qos_port_info_get_32)(struct pp_qos_dev *qdev, unsigned int id,
				   struct pp_qos_port_info *info);
extern struct pp_qos_dev *(*qos_dev_open_32)(unsigned int id);
int dp_map_to_drop_q_32(int inst, int q_id, struct dp_lookup_entry *lookup);
int dp_pp_alloc_port_32(struct ppv4_port *info);
int dp_pp_alloc_sched(struct ppv4_scheduler *info);
int dp_pp_alloc_queue_32(struct ppv4_queue *info);
int alloc_q_to_port_32(struct ppv4_q_sch_port *info, u32 flag);
extern struct cqm_deq_stat deq_port_stat[MAX_CQM_DEQ];
extern struct pp_queue_stat qos_queue_stat[MAX_QUEUE];
int init_ppv4_qos_32(int inst, int flag);
int ppv4_alloc_port_32(int inst, int deq_port_num);
int ppv4_port_free_32(int inst, int base, int deq_port_num);
#endif /* DATAPATH_PPV4_H */
