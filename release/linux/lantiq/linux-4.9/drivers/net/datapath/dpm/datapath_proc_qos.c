/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <net/datapath_api.h>
#include <net/datapath_proc_api.h>
#include "datapath.h"

#define PROC_MAX_BOX_LVL (DP_MAX_SCH_LVL + 1) /* Sched/Port both map to a box */
/* max direct child per scheduler/port */
#define PROC_DP_MAX_CHILD_PER_SCH_PORT DP_MAX_CHILD_PER_NODE
#define PROC_MAX_Q_PER_PORT 16 /* max queues per port */
#define PROC_DP_MAX_SCH_PER_PORT 4 /* max schedulers per port */
#define PROC_DP_MAX_LEAF 8 /* max leaf per scheduler */
struct location {
	int x1, y1; /* start axis */
	int x2, y2; /* end axis */
};

struct box_info;

struct child_node {
	int filled;
	enum dp_node_type type;
	union {
		int qid;
		struct box_info *box;
		} box_qid;
	struct location l;
};

struct box_info {
	int filled;
	int id;  /* physical id if available, otherwise -1 */
	int node; /* node id if available, otherwise -1 */
	int pir, cir, pbs, cbs;
	int prn_leaf;  /* since PPV4 not support leaf,
			* here generate leaf for layout printing
			*/

	int box_x, box_y, box_height; /* axis (x,y) for box */
	int size;
	int n_q, n_sch; /* direct child queue/scheduler */
	struct box_info *p_box; /* parent box */
	struct child_node child[PROC_DP_MAX_CHILD_PER_SCH_PORT];
	struct location l; /* location */
};

struct q_print_info {
	int q_num;
	int q_id[PROC_MAX_Q_PER_PORT]; /* physical queue id if available,
					*   otherwise -1
					*/
	int q_node_id[PROC_MAX_Q_PER_PORT]; /* queue node id if available,
					     * otherwise -1
					     */
	int q_prn_leaf[PROC_MAX_Q_PER_PORT];  /* since PPV4 not support leaf,
					       * here generate leaf for layout
					       * printing
					       */
	int sch_lvl[PROC_MAX_Q_PER_PORT];

	/* point to one of box entry */
	struct box_info *sch_box[PROC_MAX_Q_PER_PORT][PROC_MAX_BOX_LVL];
	struct box_info port_box;
	int box_num;
	struct box_info box[PROC_DP_MAX_SCH_PER_PORT]; /* need kmalloc/kfree */
};

static char *get_node_stat(int node_id, int type);
char *get_node_pri(int node_id, int type);
char *dp_port_flag_str(int cqm_deq_port, int flag);
char *dp_port_dma_tx_str(int cqm_deq_port, int flag);
static void conv_limit_to_str(int shaper_limit, char *buf, int size);

struct box_info *find_box_via_nodeid(struct box_info *box,
				     int box_num, int sch_node_id)
{
	int i;

	for (i = 0; i < box_num; i++) {
		if (!box[i].filled)
			continue;
		if (box[i].node != sch_node_id)
			continue;
		return &box[i];
	}
	return NULL;
}

struct box_info *find_child_box(
	struct child_node child_list[PROC_DP_MAX_CHILD_PER_SCH_PORT],
	struct box_info *curr_box)
{
	int i = 0;

	while (child_list[i].filled) {
		if (child_list[i].type != DP_NODE_SCH) {
			i++;
			continue;
		}
		if (child_list[i].box_qid.box == curr_box)
			return curr_box;
		i++;
	}
	return NULL;
}

void set_child_per_box(struct q_print_info *q_info)
{
	int i, j, idx;
	struct box_info *p_box, *c_box;

	/* Get child number and info */
	for (i = 0; i < q_info->q_num; i++) { /* queue */
		if (!q_info->sch_box[i][0]) {/* queue to port case */
			p_box = &q_info->port_box;
			idx = p_box->n_q + p_box->n_sch;

			if (idx >= PROC_DP_MAX_CHILD_PER_SCH_PORT) {
				pr_err("too many child: should <%d\n",
				       PROC_DP_MAX_CHILD_PER_SCH_PORT);
				return;
			}
			p_box->child[idx].filled = 1;
			p_box->child[idx].box_qid.qid = q_info->q_id[i];
			p_box->child[idx].type = DP_NODE_QUEUE;
			q_info->q_prn_leaf[i] = idx;
			p_box->n_q++;
			continue;
		}
		/* queue to 1st scheduler */
		p_box = q_info->sch_box[i][0];
		idx = p_box->n_q + p_box->n_sch;

		if (idx >= PROC_DP_MAX_CHILD_PER_SCH_PORT) {
			pr_err("too many child: should <%d\n",
			       PROC_DP_MAX_CHILD_PER_SCH_PORT);
			return;
		}
		if (idx < 0) {
			pr_err("wrong: idx(%d) should >= 0\n", idx);
			return;
		}
		p_box->child[idx].filled = 1;
		p_box->child[idx].box_qid.qid = q_info->q_id[i];
		p_box->child[idx].type = DP_NODE_QUEUE;
		q_info->q_prn_leaf[i] = idx;
		p_box->n_q++;

		/* scheduler to schduer/port */
		for (j = 0; j < q_info->sch_lvl[i]; j++) {
			if (j < (q_info->sch_lvl[i] - 1))
				p_box = q_info->sch_box[i][j + 1];
			else
				p_box = &q_info->port_box;
			c_box = q_info->sch_box[i][j];
			idx = p_box->n_q + p_box->n_sch;
			if (idx < 0) {
				pr_err("wrong: idx(%d) should >= 0\n", idx);
				return;
			}
			c_box->p_box = p_box;
			if (find_child_box(p_box->child, c_box))
				continue;
			if (idx >= PROC_DP_MAX_CHILD_PER_SCH_PORT - 1) {
				pr_err("too many child: should <%d\n",
				       PROC_DP_MAX_CHILD_PER_SCH_PORT);
				return;
			}
			p_box->child[idx].filled = 1;
			p_box->child[idx].box_qid.box = c_box;
			p_box->child[idx].type = DP_NODE_SCH;
			c_box->prn_leaf = idx;
			p_box->n_sch++;
		}
	}
}

#define PREFIX_SIZE_PER_BOX 1 /* for opening ----- */
#define INFO_SIZE_PER_BOX   2 /* for box info, like node id and others */
#define SUFIX_SIZE_PER_BOX  1 /* for closing ---- */
#define EXTRA_SIZE_PER_BOX (PREFIX_SIZE_PER_BOX + INFO_SIZE_PER_BOX + \
				SUFIX_SIZE_PER_BOX)
#define PADDING_BETWEEN_BOX_X 2 /* axis x */
#define PADDING_BETWEEN_BOX_Y 1 /* axis y */
#define SIZE_PER_QUEUE      3
#define BOX_WIDTH           20
#define Q_WIDTH             18
#define PORT_OTHER_INFO     20
#define PORT_BOX_SUFFIX     22

int set_location_size(struct box_info *box, int y)
{
	int i, y2 = 0, size = 0;

	box->l.x1 = Q_WIDTH + box->box_x * (BOX_WIDTH + PADDING_BETWEEN_BOX_X);
	box->l.x2 = box->l.x1 + BOX_WIDTH;
	box->l.y1 = y;
	y2 = box->l.y1 + PREFIX_SIZE_PER_BOX + INFO_SIZE_PER_BOX;
	for (i = 0;
	     (box->child[i].filled && (i < (box->n_q + box->n_sch)));
		 i++) {
		if (box->child[i].type == DP_NODE_QUEUE) {
			box->child[i].l.x2 = box->l.x1;
			box->child[i].l.y2 = y2;
			size += SIZE_PER_QUEUE;
			y2 += SIZE_PER_QUEUE;
		} else if (box->child[i].type == DP_NODE_SCH) {
			set_location_size(box->child[i].box_qid.box, y2);
			box->child[i].l.x2 = box->l.x1;
			box->child[i].l.y2 = y2;
			size += box->child[i].box_qid.box->size;
			y2 += box->child[i].box_qid.box->size;
		}
	}
	y2 += SUFIX_SIZE_PER_BOX;
	size += EXTRA_SIZE_PER_BOX;
	box->l.y2 = y2;
	box->size = size;
	return 0;
}

int check_location(struct q_print_info *q_info)
{
	int i;

	for (i = 0; i < q_info->box_num; i++) {
		if ((q_info->box[i].l.x2 - q_info->box[i].l.x1) != BOX_WIDTH) {
			pr_err("sched[%d] x1/x2: %d - %d should equal%d\n",
			       q_info->box[i].node,
			       q_info->box[i].l.x2,
			       q_info->box[i].l.x1,
			       q_info->box[i].l.x2 - q_info->box[i].l.x1);
			return -1;
		}
		if (!q_info->box[i].p_box)
			continue;
		if ((q_info->box[i].p_box->l.x1 - q_info->box[i].l.x2) !=
			PADDING_BETWEEN_BOX_X) {
			pr_err("sched[%d]<->sched[%d]: %d - %d %s%d\n",
			       q_info->box[i].node,
			       q_info->box[i].p_box->node,
			       q_info->box[i].p_box->l.x2,
			       q_info->box[i].l.x1,
			       "should equal",
			       q_info->box[i].p_box->l.x2 -
			       q_info->box[i].l.x1);
			return -1;
		}
	}
	return 0;
}

void virtual_print_box(struct box_info *box,
		       struct box_info *p_box,
		       char *buf, int rows, int cols)
{
	char *p;
	int i, len;
	char *stat = NULL;
	char *info = NULL, *p_flag = NULL, *p_dma_tx = NULL;
	struct dp_shaper_conf shaper = {0};
	char buf_cir[6] = {0};
	char buf_pir[6] = {0};

	/* The format like below
	 *         -----------------------
	 *        |sched[%03d]            |
	 *        |                       |
	 *        |leaf[%2d]:kbps         |
	 *        | cir/pir:%5d/%5d       |
	 *        | cbs/pbs:%5d/%5d       |
	 *        | ....                  |
	 *        |                       |
	 *         -----------------------
	 */
	p = &buf[cols * box->l.y1];
	for (i = box->l.x1; i < box->l.x2; i++)
		p[i] = '-';
	p = &buf[cols * (box->l.y2 - 1)];
	for (i = box->l.x1; i < box->l.x2; i++)
		p[i] = '-';

	for (i = 0; i < INFO_SIZE_PER_BOX; i++) {
		p = &buf[cols * (box->l.y1 + 1 + i)];
		p += box->l.x1 + 1;
		if (i == 0) { /* print 1st info of box */
			if (!p_box) { /* port box */
				len = snprintf(p, BOX_WIDTH - 3,
					       "port[%d/%d]",
					       box->id, box->node);
				p[len] = ' ';
				stat = get_node_stat(box->id, DP_NODE_PORT);
				shaper.id.cqm_deq_port = box->id;
				shaper.type = DP_NODE_PORT;
				dp_shaper_conf_get(&shaper, 0);
				if ((shaper.cir == DP_MAX_SHAPER_LIMIT) ||
				    (shaper.cir == DP_NO_SHAPER_LIMIT)) {
					conv_limit_to_str(shaper.cir, buf_cir,
							  sizeof(buf_cir));
					DP_DEBUG(DP_DBG_FLAG_QOS,
						 "port[%d] shaper=%d(%s)\n",
						 shaper.id.cqm_deq_port,
						 shaper.cir,
						 buf_cir);
				} else {
					DP_DEBUG(DP_DBG_FLAG_QOS,
						 "port[%d] shaper=%d\n",
						 shaper.id.cqm_deq_port,
						 shaper.cir);
				}
				p_flag = dp_port_flag_str(box->id, 0);
				p_dma_tx = dp_port_dma_tx_str(box->id, 0);
				DP_DEBUG(DP_DBG_FLAG_QOS,
					 "port[%d] p_flag=%s\n",
					 box->id, p_flag);
			} else { /* sched box */
				len = snprintf(p, BOX_WIDTH - 3,
					       "sched[/%d]",  box->node);
				stat = get_node_stat(box->node, DP_NODE_SCH);
				info = get_node_pri(box->node, DP_NODE_SCH);
				shaper.id.sch_id = box->node;
				shaper.type = DP_NODE_SCH;
				dp_shaper_conf_get(&shaper, 0);
				if ((shaper.cir == DP_MAX_SHAPER_LIMIT) ||
				    (shaper.cir == DP_NO_SHAPER_LIMIT)) {
					conv_limit_to_str(shaper.cir, buf_cir,
							  sizeof(buf_cir));
					DP_DEBUG(DP_DBG_FLAG_QOS,
						 "sched[%d] shaper=%d(%s)\n",
						 shaper.id.sch_id, shaper.cir,
						 buf_cir);
				} else {
					DP_DEBUG(DP_DBG_FLAG_QOS,
						 "sched[%d] shaper=%d\n",
						 shaper.id.sch_id, shaper.cir);
				}
				p[len] = ' ';
			}
		} else if (i == 1) {
			len = snprintf(p, BOX_WIDTH - 3, " stat:%s",
				       stat ? stat : "NULL");
			p[len] = ' ';
		}
	}

	for (i = box->l.y1 + 1; i < box->l.y2 - 1; i++) {
		p = &buf[cols * i];
		p[box->l.x1] = '|';
	}
	for (i = box->l.y1 + 1; i < box->l.y2 - 1; i++) {
		p = &buf[cols * i];
		p[box->l.x2 - 1] = '|';
	}
	if (!p_box) { /* port information */
		p = &buf[cols * ((box->l.y1 + box->l.y2) / 2 - 1)];
		p += box->l.x2;
		len = snprintf(p, cols - box->l.x2 - 1,
			       "--%s", p_flag ? p_flag : "");
		if (len >= 0)
			p[len] = ' ';

		p += cols;
		len = snprintf(p, cols - box->l.x2 - 1,
			       "  %s", p_dma_tx ? p_dma_tx : "");
		if (len >= 0)
			p[len] = ' ';

		p += cols;
		conv_limit_to_str(shaper.cir, buf_cir, sizeof(buf_cir));
		conv_limit_to_str(shaper.pir, buf_pir, sizeof(buf_pir));
		len = snprintf(p, cols - box->l.x2 - 1,
			       "  C/P:%5s/%5s", buf_cir, buf_pir);
		p[len] = ' '; /* remove \0' added by snprintf */
		p += cols;
		len = snprintf(p, cols - box->l.x2 - 1,
			       "  c/p:%5d/%5d", shaper.cbs, shaper.pbs);
		p[len] = ' ';
		return;
	}

	/* print link to the parent box */
	p = &buf[cols * ((box->l.y1 + box->l.y2) / 2)];
	for (i = box->l.x2; i < p_box->l.x1; i++)
		p[i] = '-';

	/* print leaf info in the parent box:sched/port */
	p += p_box->l.x1 + 1; /* skip '|' */
	for (i = 0; i < SIZE_PER_QUEUE; i++) {
		if (i == 0) {
			len = snprintf(p, BOX_WIDTH - 3,/* skip: | & | & null */
				       "child[%d] %s", box->prn_leaf,
				       info ? info : "");
			p[len] = ' ';
		} else if (i == 1) {
			conv_limit_to_str(shaper.cir, buf_cir, sizeof(buf_cir));
			conv_limit_to_str(shaper.pir, buf_pir, sizeof(buf_pir));
			len = snprintf(p, BOX_WIDTH - 3,
				       " C/P:%5s/%5s", buf_cir, buf_pir);
			p[len] = ' ';
		} else if (i == 2) {
			len = snprintf(p, BOX_WIDTH - 3,
				       " c/p:%5d/%5d", shaper.cbs, shaper.pbs);
			p[len] = ' ';
		}
		/* move to next row */
		p += cols;
	}
}

void virtual_print_queues(struct q_print_info *q_info,
			  char *buf, int rows, int cols)
{
	int i, j;
	struct box_info *box;
	int len, idx;
	char *p;
	char *stat = NULL;
	char *info = NULL;
	struct dp_shaper_conf shaper = {0};
	char buf_cir[6] = {0};
	char buf_pir[6] = {0};

	for (i = 0; i < q_info->q_num; i++) {
		if (q_info->sch_box[i][0])
			box = q_info->sch_box[i][0];
		else
			box = &q_info->port_box;
		idx = q_info->q_prn_leaf[i];
		DP_DEBUG(DP_DBG_FLAG_QOS, "get_node_stat:queue=%d\n",
			 q_info->q_id[i]);
		stat = get_node_stat(q_info->q_id[i], DP_NODE_QUEUE);
		info = get_node_pri(q_info->q_id[i], DP_NODE_QUEUE);
		shaper.id.q_id = q_info->q_id[i];
		shaper.type = DP_NODE_QUEUE;
		dp_shaper_conf_get(&shaper, 0);
		if ((shaper.cir == DP_MAX_SHAPER_LIMIT) ||
		    (shaper.cir == DP_NO_SHAPER_LIMIT)) {
			conv_limit_to_str(shaper.cir, buf_cir, sizeof(buf_cir));
			DP_DEBUG(DP_DBG_FLAG_QOS, "q[%d] shaper=%d(%s)\n",
				 shaper.id.q_id, shaper.cir, buf_cir);
		} else {
			DP_DEBUG(DP_DBG_FLAG_QOS, "q[%d] shaper=%d\n",
				 shaper.id.q_id, shaper.cir);
		}
		p = &buf[cols * box->child[idx].l.y2];
		len = snprintf(p, Q_WIDTH - 1, "q[%4d/%4d]",
			       q_info->q_id[i], q_info->q_node_id[i]);
		for (j = len; j < box->l.x1; j++)
			p[j] = '-';

		p = &buf[cols * (box->child[idx].l.y2 + 1)];
		len = snprintf(p, Q_WIDTH - 1, "  stat:%s",
			       stat ? stat : "");
		p[len] = ' ';

		/* print leaf info in the parent box:sched/port */
		p = &buf[cols * box->child[idx].l.y2];
		p += box->l.x1 + 1; /* skip '|' */
		for (j = 0; j < SIZE_PER_QUEUE; j++) {
			if (j == 0) {
				len = snprintf(p, BOX_WIDTH - 3, /* skip:| and |
								  * and null
								  */
					       "child[%d]:%s",
					       q_info->q_prn_leaf[i],
					       info ? info : "");
				p[len] = ' ';
			} else if (j == 1) {
				conv_limit_to_str(shaper.cir, buf_cir,
						  sizeof(buf_cir));
				conv_limit_to_str(shaper.pir, buf_pir,
						  sizeof(buf_pir));
				len = snprintf(p, BOX_WIDTH - 3,
					       " C/P:%5s/%5s",
					       buf_cir, buf_pir);
				p[len] = ' ';
			} else if (j == 2) {
				len = snprintf(p, BOX_WIDTH - 3,
					       " c/p:%5d/%5d",
					       shaper.cbs, shaper.pbs);
				p[len] = ' ';
			}
			/* move to next row */
			p += cols;
		}
	}
}

#define PRINT_QOS_DETAIL  0
void print_all(struct seq_file *s, struct q_print_info *q_info)
{
	int cols = q_info->port_box.l.x2 + PORT_BOX_SUFFIX;
	int rows = q_info->port_box.l.y2 + 1;
	int i;
	char *buf;
	char *p;

	buf = kmalloc(cols * rows + 1, GFP_KERNEL);
	if (!buf) {
		pr_err("fail to alloc %d bytes\n", cols * rows + 1);
		return;
	}
	memset(buf, ' ', cols * rows);
	buf[cols * rows] = 0;
#if PRINT_QOS_DETAIL
	seq_printf(s, "allocate buffer: %d bytes(%d * %d)\n",
		   cols * rows, cols, rows);
#endif

	p = buf;
	for (i = 0; i < rows; i++) {
		p += cols;
		*(p - 1) = 0;
	}

	/* print port box */
	virtual_print_box(&q_info->port_box, NULL, buf, rows, cols);
	for (i = 0; i < q_info->box_num; i++)
		virtual_print_box(&q_info->box[i], q_info->box[i].p_box, buf,
				  rows, cols);
	/* print queue */
	virtual_print_queues(q_info, buf, rows, cols);
	p = buf;
	for (i = 0; i < rows; i++) {
		seq_printf(s, "%s\n", p);
		p += cols;
	}
	kfree(buf);
}

/* print_box_lvl must bigger 1 than sch_lvl */
struct q_print_info *collect_info(struct seq_file *s,
				  struct dp_dequeue_res *res,
				  int print_box_lvl)
{
	int i, j, size, curr_box_y = 0, curr_box_x;
	struct q_print_info *q_info = NULL;
	struct box_info *box;
	char f_new_box;

	if (!res || !res->num_q || (res->num_deq_ports < 1))
		goto ERR_EXIT;

	if (res->num_q >= PROC_MAX_Q_PER_PORT) {
		seq_printf(s, "too many queues in one port:%d should <%d\n",
			   res->num_q, PROC_MAX_Q_PER_PORT);
		return NULL;
	}

	size = sizeof(*q_info) + 1;
	q_info = kmalloc(size, GFP_KERNEL);
	if (!q_info) {
		pr_err("fail to alloc %d bytes\n", size);
		return NULL;
	}
	memset(q_info, 0, size);

	q_info->port_box.filled = 1;
	q_info->port_box.id = res->q_res[0].cqm_deq_port;
	q_info->port_box.node = res->q_res[0].qos_deq_port;
	q_info->port_box.box_x = print_box_lvl - 1;
	q_info->port_box.box_y = 0;

	for (i = 0; i < res->num_q; i++) { /* q loop */
		q_info->q_id[i] = res->q_res[i].q_id;
		q_info->q_node_id[i] = res->q_res[i].q_node;
		if (res->q_res[i].sch_lvl <= 0)
			continue;
		if (res->q_res[i].sch_lvl > PROC_MAX_BOX_LVL - 1) {
			pr_err("Too many sched lvl(%d): expect<=%d\n",
			       res->q_res[i].sch_lvl,
			       PROC_MAX_BOX_LVL - 1);
				goto ERR_EXIT;
		}
		f_new_box = 0;
		curr_box_x = print_box_lvl - res->q_res[i].sch_lvl - 1;
		for (j = 0; j < res->q_res[i].sch_lvl; j++) { /* each sched */
			box = find_box_via_nodeid(q_info->box,
						  q_info->box_num,
						  res->q_res[i].sch_id[j]);
			if (box) {
				q_info->sch_box[i][j] = box;
				continue;
			}
			/* create a new box */
			memset(&q_info->box[q_info->box_num], 0,
			       sizeof(q_info->box[q_info->box_num]));
			q_info->box[q_info->box_num].filled = 1;
			q_info->box[q_info->box_num].node =
							res->q_res[i].sch_id[j];
			q_info->box[q_info->box_num].id = -1; /* not valid */
			q_info->box[q_info->box_num].box_x = curr_box_x + j;
			q_info->box[q_info->box_num].box_y = curr_box_y;
			q_info->sch_box[i][j] = &q_info->box[q_info->box_num];
			q_info->box_num++;
			if (q_info->box_num == ARRAY_SIZE(q_info->box)) {
				pr_err("sched+port (%d) in one node: %s<%zd\n",
				       q_info->box_num,
				       "expect",
				       ARRAY_SIZE(q_info->box));
				goto ERR_EXIT;
			}
			f_new_box = 1;
		}
		q_info->sch_lvl[i] = res->q_res[i].sch_lvl;
		if (f_new_box)
			curr_box_y++;
	}
	q_info->q_num = res->num_q;

	/* sanity check */
	for (i = 0; i < res->num_q; i++) {
		if (!q_info->sch_lvl[i])
			continue;
		for (j = 0; j < q_info->sch_lvl[i]; j++) {
			if (!q_info->sch_box[i][j]->filled) {
				pr_err("sch_box[%d][%d].fill should 1:%d\n",
				       i, j, q_info->sch_box[i][j]->filled);
				goto ERR_EXIT;
			}
			if (q_info->sch_box[i][j]->n_q < 0) {
				pr_err("sch_box[%d][%d].n_q should >=0:%d\n",
				       i, j, q_info->sch_box[i][j]->n_q);
				goto ERR_EXIT;
			}
			if (q_info->sch_box[i][j]->n_sch < 0) {
				pr_err("sch_box[%d][%d].n_sch should >=0:%d\n",
				       i, j, q_info->sch_box[i][j]->n_sch);
				goto ERR_EXIT;
			}
		}
	}
	return q_info;
ERR_EXIT:
	kfree(q_info);
	return NULL;
}

//static struct dp_dequeue_res res;
static struct dp_queue_res q_res[PROC_MAX_Q_PER_PORT * 4];
static int qos_layout_inst;
static int qos_layout_max_lvl = PROC_MAX_BOX_LVL; /* sched/port box */
static int max_tconf_idx;
static int curr_tconf_idx;
struct dp_dequeue_res tmp_res;
struct dp_queue_res tmp_q_res[PROC_MAX_Q_PER_PORT] = {0};
#define DP_PROC_BUF_LEN 20
static char dma_flag[DP_PROC_BUF_LEN];
static char port_flag[DP_PROC_BUF_LEN];

static char *port_flag_str(int inst, int dp_port)
{
	int i;
	struct pmac_port_info *port_info;

	port_info = get_dp_port_info(inst, dp_port);
	if (!port_info)
		return "";
	for (i = 0; i < get_dp_port_type_str_size(); i++) {
		if (port_info->alloc_flags & dp_port_flag[i])
			return dp_port_type_str[i];
	}
	return "";
}

char *dp_port_flag_str(int cqm_deq_port, int flag)
{
	int i;
	int inst = qos_layout_inst;

	port_flag[0] = 0;
	for (i = 0; i < dp_port_prop[inst].info.cap.max_num_dp_ports; i++) {
		struct pmac_port_info *pi = get_dp_port_info(inst, i);

		if ((cqm_deq_port >= pi->deq_port_base) &&
		    (cqm_deq_port < (pi->deq_port_base +
		     pi->deq_port_num)))  {
			if (i == 0) {
				snprintf(port_flag, sizeof(port_flag),
					 "CPU:%d", i);
				return port_flag;
			}
			if (!pi->alloc_flags)
				continue;

			snprintf(port_flag, sizeof(port_flag),
				 "%s:%d",
				 port_flag_str(inst, i),
				 i);
			return port_flag;
		}
	}
	snprintf(port_flag, sizeof(port_flag), "?:%d", cqm_deq_port);
	return port_flag;
}

char *dp_port_dma_tx_str(int cqm_deq_port, int flag)
{
	int i;
	int inst = qos_layout_inst;

	dma_flag[0] = 0;
	for (i = 0; i < dp_port_prop[inst].info.cap.max_num_dp_ports; i++) {
		struct pmac_port_info *pi = get_dp_port_info(inst, i);

		if ((cqm_deq_port >= pi->deq_port_base) &&
		    (cqm_deq_port < (pi->deq_port_base +
		     pi->deq_port_num))) {
			if (i == 0) {
				snprintf(dma_flag, sizeof(dma_flag), "-");
				return dma_flag;
			}
			if (!pi->alloc_flags)
				continue;
			snprintf(dma_flag, sizeof(dma_flag), "CH%x",
				 dp_deq_port_tbl[inst][cqm_deq_port].dma_chan);
			return dma_flag;
		}
	}
	snprintf(dma_flag, sizeof(dma_flag), "?:%d", cqm_deq_port);
	return dma_flag;
}

char *get_node_stat(int node_id, int type)
{
	struct dp_node_link_enable node_en = {0};
	static char stat[20];

	node_en.inst = qos_layout_inst;
	node_en.id.q_id = node_id;
	node_en.type = (enum dp_node_type)type;
	stat[0] = 0;
	if (dp_node_link_en_get(&node_en, 0)) {
		strcpy(stat, "?");
	} else {
		if (node_en.en & DP_NODE_DIS)
			strcat(stat, "Blk ");
		if (node_en.en & DP_NODE_SUSPEND)
			strcat(stat, "Susp");
		if (node_en.en & DP_NODE_EN)
			strcat(stat, "Normal");
		if (strlen(stat) == 0)
			strcat(stat, "??");
	}

	return stat;
}

char *get_node_pri(int node_id, int type)
{
	struct dp_node_prio node_prio = {0};
	static char stat[20];
	int len;

	stat[0] = 0;
	node_prio.inst = qos_layout_inst;
	node_prio.id.q_id = node_id;
	node_prio.type = (enum dp_node_type)type;
	if (dp_qos_link_prio_get(&node_prio, 0)) {
		strcpy(stat, "?");
	} else {
		if (node_prio.arbi == ARBITRATION_WRR)
			strcpy(stat, "WRR");
		else if (node_prio.arbi == ARBITRATION_SP)
			strcpy(stat, "SP");
		else if (node_prio.arbi == ARBITRATION_WSP)
			strcpy(stat, "WSP");
		else if (node_prio.arbi == ARBITRATION_WFQ)
			strcpy(stat, "WFQ");
		else
			snprintf(stat, sizeof(stat), "?%d:", node_prio.arbi);

		len = strlen(stat);
		snprintf(stat + len, sizeof(stat) - len - 1, ":%d",
			 node_prio.prio_wfq);
	}
	if (node_prio.type == DP_NODE_QUEUE)
		DP_DEBUG(DP_DBG_FLAG_QOS, "q[%d] arbi=%d prio=%d\n",
			 node_prio.id.q_id, node_prio.arbi, node_prio.prio_wfq);
	else if (node_prio.type == DP_NODE_SCH)
		DP_DEBUG(DP_DBG_FLAG_QOS, "sch[%d] arbi=%d prio=%d\n",
			 node_prio.id.sch_id, node_prio.arbi,
			 node_prio.prio_wfq);
	else if (node_prio.type == DP_NODE_PORT)
		DP_DEBUG(DP_DBG_FLAG_QOS, "port[%d] arbi=%d prio=%d\n",
			 node_prio.id.cqm_deq_port, node_prio.arbi,
			 node_prio.prio_wfq);
	else
		DP_DEBUG(DP_DBG_FLAG_QOS, "unknown type\n");
	return stat;
}

void conv_limit_to_str(int shaper_limit, char *buf, int size)
{
	if (shaper_limit == DP_NO_SHAPER_LIMIT)
		snprintf(buf, size, "NoLim");
	else if (shaper_limit == DP_MAX_SHAPER_LIMIT)
		snprintf(buf, size, "MaxLm");
	else
		snprintf(buf, size, "%d", shaper_limit);
}

int get_num_deq_ports(int inst, int dp_port)
{
	memset(&tmp_res, 0, sizeof(tmp_res));
	tmp_res.inst = inst;
	tmp_res.dp_port = dp_port;
	tmp_res.q_res = NULL;
	tmp_res.cqm_deq_idx = DEQ_PORT_OFFSET_ALL;
	if (dp_deq_port_res_get(&tmp_res, 0))
		return -1;
	return tmp_res.num_deq_ports;
}

int get_res(int inst, int dp_port, int tconf_idx)
{
	memset(&tmp_res, 0, sizeof(tmp_res));
	tmp_res.inst = inst;
	tmp_res.dp_port = dp_port;
	tmp_res.cqm_deq_idx = tconf_idx;
	tmp_res.q_res = q_res;
	tmp_res.q_res_size = ARRAY_SIZE(q_res);
	DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
		 "get_res: dp_port=%d tconf_idx=%d\n",
		 tmp_res.dp_port, tmp_res.cqm_deq_idx);
	if (dp_deq_port_res_get(&tmp_res, 0)) {
		pr_err("dp_deq_port_res_get fail: inst=%d dp_port=%d, %s=%d\n",
		       qos_layout_inst, tmp_res.dp_port,
		       "tconf_idx", curr_tconf_idx);
		return -1;
	}
	return 0;
}

int dump_q_info_dbg(struct seq_file *s, struct dp_dequeue_res *res)
{
#if PRINT_QOS_DETAIL
	struct dp_queue_res *q_res = res->q_res;
	int i, j;

	for (i = 0; i < res->num_q; i++) {
		seq_printf(s, "q[%d]-", q_res[i].q_id);
		for (j = 0; j < q_res[i].sch_lvl; j++)
			seq_printf(s, "sched[%d]-", q_res[i].sch_id[j]);
		seq_printf(s, "p[%d/%d]\n", q_res[i].cqm_deq_port,
			   q_res[i].qos_deq_port);
	}
#endif
	return 0;
}

int qos_dump(struct seq_file *s, int pos)
{
	struct q_print_info *q_info;
	int i;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	if (pos == 0) {
		seq_puts(s, "Note:\n");
		seq_puts(s, "  x/y :physical node/logical node\n");
		seq_puts(s, "      :cqm dequeue port/ppv4 logical node\n");
		seq_puts(s, "  Blk :blocked, ie new incoming packet will be dropped\n");
		seq_puts(s, "  Susp:suspended, ie, packet in queue will not be scheduled\n");
		seq_puts(s, "  C   :CIR in kbps\n");
		seq_puts(s, "  P   :PIR in kbps\n");
		seq_puts(s, "  c   :cbs in bytes\n");
		seq_puts(s, "  p   :pbs in bytes\n");
		seq_puts(s, "\n");
	}
	max_tconf_idx = get_num_deq_ports(qos_layout_inst, pos);
	DP_DEBUG(DP_DBG_FLAG_QOS, "dp_port=%d  max_tconf=%d\n",
		 pos, max_tconf_idx);
	if (max_tconf_idx <= 0)
		goto NEXT_DP_PORT;
	curr_tconf_idx = 0;
	for (i = 0; i < max_tconf_idx; i++) {
		/* get qos resource and store into tmp_res */
		if (get_res(qos_layout_inst, pos, i))
			continue;
		dump_q_info_dbg(s, &tmp_res);
		if (!tmp_res.num_q)
			continue;
		q_info = collect_info(s, &tmp_res, qos_layout_max_lvl);
		if (!q_info)
			goto NEXT_DP_PORT;
		set_child_per_box(q_info);
		set_location_size(&q_info->port_box, 0);
		if (check_location(q_info)) {
			kfree(q_info);
			continue;
		}
		print_all(s, q_info);
		kfree(q_info);
	}
NEXT_DP_PORT:
	if (!seq_has_overflowed(s))
		pos++;
	if (pos >= 16)
		pos = -1;

	return pos;
}

int qos_dump_start(void)
{
	struct dp_qos_level dp = {0};

	qos_layout_inst = 0;
	/* current just use maximum one. Later should check real QOS
	 * configuration and get its maxim hirachy layers
	 * qos_layout_max_lvl = PROC_MAX_BOX_LVL;
	 */
	dp_qos_level_get(&dp, 0);
	qos_layout_max_lvl = dp.max_sch_lvl + 1;
	max_tconf_idx = 0;
	return 0;
}

void qos_create_qos_help(void)
{
	pr_info("QOS Command Help:\n\n");
	pr_info("     Add Queue: echo add_q <qid> <schid>:<leaf> %s%s\n\n",
		"<schid>:<leaf> <schid>:<leaf> <schid>:<leaf> <portid> ",
		"> /sys/kernel/debug/dp/qos");
	pr_info("            id: qid/schid_node/cbm_port\n");
	pr_info("  Delete Queue: echo del_q qid > /sys/kernel/debug/dp/qos\n");
	pr_info("  Delete Sched: echo del_sch schid > %s\n",
		"/sys/kernel/debug/dp/qos");
	pr_info("  SET PRIO: echo prio <id> <type> <arbi> <prio_wfq> %s\n",
		"> /sys/kernel/debug/dp/qos");
	pr_info("            id: phy_queue/sched_node\n");
	pr_info("          type: queue/sched\n");
	pr_info("          arbi: null/sp/wsp/wrr/wrr_wsp/wfq\n");
	pr_info("    CFG SHAPER: echo shaper <cmd> <id> <type> %s\n",
		"<cir> <pir> <cbs> <pbs> > /sys/kernel/debug/dp/qos");
	pr_info("           cmd: add/remove/disable\n");
	pr_info("            id: qid/sched_node/cbm_port\n");
	pr_info("          type: queue/sched/port\n");
	pr_info("           cir: no_limit/max/value\n");
	pr_info("      SET NODE: echo set_node <id> <type> <cmd>... %s\n",
		"> /sys/kernel/debug/dp/qos");
	pr_info("            id: phy_queue/sched_node/cbm_port\n");
	pr_info("          type: queue/sched/port\n");
	pr_info("           cmd: enable(unblock)/disable(block)/%s\n",
		"resume/suspend");
	pr_info("                enable/disable: only for queue/port\n");
	pr_info("                resume/suspend: for all nodes\n");
}

ssize_t proc_qos_write(struct file *file, const char *buf, size_t count,
		       loff_t *ppos)
{
	int len, res;
	char str[100];
	char *param_list[16] = { 0 };
	char *temp_list[3] = { 0 };
	int i, idx = 2; /* index for sched_list */
	unsigned int level = 0, num = 0;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if (!len)
		return count;

	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));
	level = num - 3;

	if (num <= 1 || num > ARRAY_SIZE(param_list) ||
	    (dp_strncmpi(param_list[0], "help", strlen("help")) == 0))
		qos_create_qos_help();
	else if (dp_strncmpi(param_list[0], "add_q", strlen("add_q")) == 0) {
		struct dp_qos_link cfg = {0};

		if (num < 3) {
			pr_info("Wrong Parameter(try help):echo help > %s\n",
				"/sys/kernel/debug/dp/qos");
			return count;
		}

		cfg.q_id = dp_atoi(param_list[1]);

		cfg.cqm_deq_port = dp_atoi(param_list[num - 1]);

		cfg.q_leaf = 0;
		cfg.q_arbi = 1;
		cfg.q_prio_wfq = 0;
		cfg.n_sch_lvl = level;

		if (!level)
			pr_info("QID %d->", cfg.q_id);

		for (i = 0; (i < (num - 3) && i < DP_MAX_SCH_LVL); i++) {
			dp_replace_ch(param_list[i + idx],
				      strlen(param_list[i + idx]), ':', ' ');
			dp_split_buffer(param_list[i + idx], temp_list,
					ARRAY_SIZE(temp_list));
			cfg.sch[i].id = dp_atoi(temp_list[0]);
			cfg.sch[i].leaf = dp_atoi(temp_list[1]);
			cfg.sch[i].arbi = 1;
			cfg.sch[i].prio_wfq = 0;

			pr_info("SCH %d:LEAF %d->", cfg.sch[i].id,
				cfg.sch[i].leaf);
		}

		pr_info("PORT %d\n", cfg.cqm_deq_port);
		if (dp_link_add(&cfg, 0)) {
			pr_err("dp_link_add failed\n");
			return count;
		}
	} else if (dp_strncmpi(param_list[0], "del_q", strlen("del_q")) == 0) {
		struct dp_node_alloc node = {0};

		node.id.q_id = dp_atoi(param_list[1]);
		node.type = DP_NODE_QUEUE;

		if (dp_node_free(&node, DP_NODE_SMART_FREE)) {
			pr_err("dp_node_free failed\n");
			return count;
		}
	} else if (dp_strncmpi(param_list[0], "del_p", strlen("del_p")) == 0) {
		struct dp_node_alloc node = {0};

		if (dp_strncmpi(param_list[2], "sched", strlen("sched")) == 0) {
			node.type = DP_NODE_SCH;
			node.id.sch_id = dp_atoi(param_list[1]);
		} else if (dp_strncmpi(param_list[2], "port",
				       strlen("port")) == 0) {
			node.type = DP_NODE_PORT;
			node.id.cqm_deq_port = dp_atoi(param_list[1]);
		} else {
			pr_err("unknown type %s\n", param_list[2]);
		}
	} else if (dp_strncmpi(param_list[0], "del_sch",
			       strlen("del_sch")) == 0) {
		struct dp_node_link node_link = {0};
		struct dp_node_alloc node = {0};

		node_link.node_id.sch_id = dp_atoi(param_list[1]);
		node_link.node_type = DP_NODE_SCH;
		node.id.q_id = dp_atoi(param_list[1]);
		node.type = DP_NODE_SCH;

		if (dp_node_link_get(&node_link, 0))
			pr_err("dp_node_link_get failed\n");

		if (dp_node_free(&node, 0)) {
			pr_err("dp_node_free failed\n");
			return count;
		}

		while (1) {
			node_link.node_id = node_link.p_node_id;
			node_link.node_type = node_link.p_node_type;
			node.id = node_link.p_node_id;
			node.type = node_link.p_node_type;

			res = dp_node_link_get(&node_link, 0);

			if (dp_node_free(&node, 0))
				pr_err("dp_node_free failed\n");

			if (res) {
				pr_err("dp_node_link_get failed\n");
				break;
			}
		}

		pr_info("\nSched %d deleted\n\n", node_link.node_id.sch_id);
	} else if (dp_strncmpi(param_list[0], "prio", strlen("prio")) == 0) {
		struct dp_node_prio node_prio = {0};

		if (num < 3) {
			pr_err("id, type are required!%s%s\n",
			       "\n(try help):echo help > ",
			       "/sys/kernel/debug/dp/qos");
			return count;
		}

		if (dp_strncmpi(param_list[2], "queue", strlen("queue")) == 0) {
			node_prio.type = DP_NODE_QUEUE;
			node_prio.id.q_id = dp_atoi(param_list[1]);
		} else if (dp_strncmpi(param_list[2], "sched",
				       strlen("sched")) == 0) {
			node_prio.type = DP_NODE_SCH;
			node_prio.id.sch_id = dp_atoi(param_list[1]);
		} else if (dp_strncmpi(param_list[2], "port",
				       strlen("port")) == 0) {
			node_prio.type = DP_NODE_PORT;
			node_prio.id.cqm_deq_port = dp_atoi(param_list[1]);
		} else {
			pr_err("unknown type %s\n", param_list[2]);
		}

		if (dp_qos_link_prio_get(&node_prio, 0))
			pr_err("dp_qos_link_prio_get failed\n");

		if (dp_strncmpi(param_list[3], "null", strlen("null")) == 0) {
			node_prio.arbi = ARBITRATION_NULL;
		} else if (dp_strncmpi(param_list[3], "sp",
			   strlen("sp")) == 0) {
			node_prio.arbi = ARBITRATION_SP;
		} else if (dp_strncmpi(param_list[3], "wsp",
			   strlen("wsp")) == 0) {
			node_prio.arbi = ARBITRATION_WSP;
		} else if (dp_strncmpi(param_list[3], "wrr",
			   strlen("wrr")) == 0) {
			node_prio.arbi = ARBITRATION_WRR;
		} else if (dp_strncmpi(param_list[3], "wsp_wrr",
			   strlen("wsp_wrr")) == 0) {
			node_prio.arbi = ARBITRATION_WSP_WRR;
		} else if (dp_strncmpi(param_list[3], "wfq",
			   strlen("wfq")) == 0) {
			node_prio.arbi = ARBITRATION_WFQ;
		} else {
			pr_err("unknown type %s\n", param_list[3]);
			return count;
		}

		if (dp_atoi(param_list[4]))
			node_prio.prio_wfq = dp_atoi(param_list[4]);

		if (dp_qos_link_prio_set(&node_prio, 0)) {
			pr_err("dp_qos_link_prio_set failed\n");
			return count;
		}
	} else if (dp_strncmpi(param_list[0], "shaper",
			       strlen("shaper")) == 0) {
		struct dp_shaper_conf shaper_cfg = {0};

		if (num < 4) {
			pr_err("cmd, id, type are required!%s%s\n",
			       "\n(try help):echo help > ",
			       "/sys/kernel/debug/dp/qos");
			return count;
		}

		if (dp_strncmpi(param_list[1], "add", strlen("add")) == 0)
			shaper_cfg.cmd = DP_SHAPER_CMD_ADD;
		else if (dp_strncmpi(param_list[1], "remove",
				     strlen("remove")) == 0)
			shaper_cfg.cmd = DP_SHAPER_CMD_REMOVE;
		else if (dp_strncmpi(param_list[1], "disable",
				     strlen("disable")) == 0)
			shaper_cfg.cmd = DP_SHAPER_CMD_DISABLE;
		else
			pr_err("unknown cmd try: echo help %s",
			       "> /sys/kernel/debug/dp/qos");

		if (dp_strncmpi(param_list[3], "queue", strlen("queue")) == 0) {
			shaper_cfg.type = DP_NODE_QUEUE;
			shaper_cfg.id.sch_id = dp_atoi(param_list[2]);
		} else if (dp_strncmpi(param_list[3], "sched",
				       strlen("sched")) == 0) {
			shaper_cfg.type = DP_NODE_SCH;
			shaper_cfg.id.sch_id = dp_atoi(param_list[2]);
		} else if (dp_strncmpi(param_list[3], "port",
				       strlen("port")) == 0) {
			shaper_cfg.type = DP_NODE_PORT;
			shaper_cfg.id.sch_id = dp_atoi(param_list[2]);
		} else {
			pr_err("unknown type %s\n", param_list[3]);
			return count;
		}

		if (dp_shaper_conf_get(&shaper_cfg, 0))
			pr_err("dp_shaper_conf_get failed\n");

		if (dp_strncmpi(param_list[4], "no_limit",
				strlen("no_limit")) == 0)
			shaper_cfg.cir = DP_NO_SHAPER_LIMIT;
		else if (dp_strncmpi(param_list[4], "max", strlen("max")) == 0)
			shaper_cfg.cir = DP_MAX_SHAPER_LIMIT;
		else
			shaper_cfg.cir = dp_atoi(param_list[4]);

		if (dp_atoi(param_list[5]))
			shaper_cfg.pir = dp_atoi(param_list[5]);
		if (dp_atoi(param_list[6]))
			shaper_cfg.cbs = dp_atoi(param_list[6]);
		if (dp_atoi(param_list[7]))
			shaper_cfg.pbs = dp_atoi(param_list[7]);

		if (dp_shaper_conf_set(&shaper_cfg, 0)) {
			pr_err("dp_shaper_conf_set failed\n");
			return count;
		}
	} else if (dp_strncmpi(param_list[0], "set_node",
			       strlen("set_node")) == 0) {
		struct dp_node_link_enable en_node = {0};

		if (num < 4 || num > 5) {
			pr_err("id, type, cmd are required!%s%s\n",
			       "\n(try help):echo help > ",
			       "/sys/kernel/debug/dp/qos");
			return count;
		}

		if (dp_strncmpi(param_list[2], "queue", strlen("queue")) == 0) {
			en_node.type = DP_NODE_QUEUE;
			en_node.id.q_id = dp_atoi(param_list[1]);
		} else if (dp_strncmpi(param_list[2], "sched",
				       strlen("sched")) == 0) {
			en_node.type = DP_NODE_SCH;
			en_node.id.sch_id = dp_atoi(param_list[1]);
		} else if (dp_strncmpi(param_list[2], "port",
				       strlen("port")) == 0) {
			en_node.type = DP_NODE_PORT;
			en_node.id.cqm_deq_port = dp_atoi(param_list[1]);
		} else {
			pr_err("Incorrect parameter!%s%s%s\n", param_list[2],
			       "\n(try help):echo help > ",
			       "/sys/kernel/debug/dp/qos");
			return count;
		}

		if (dp_strncmpi(param_list[3], "enable",
				strlen("enable")) == 0) {
			en_node.en |= DP_NODE_EN;
		} else if (dp_strncmpi(param_list[3], "disable",
				       strlen("disable")) == 0) {
			en_node.en |= DP_NODE_DIS;
		} else if (dp_strncmpi(param_list[3], "suspend",
				       strlen("suspend")) == 0) {
			en_node.en |= DP_NODE_SUSPEND;
		} else if (dp_strncmpi(param_list[3], "resume",
				       strlen("resume")) == 0) {
			en_node.en |= DP_NODE_RESUME;
		} else {
			pr_err("Incorrect parameter!%s%s%s\n", param_list[3],
			       "\n(try help):echo help > ",
			       "/sys/kernel/debug/dp/qos");
			return count;
		}

		if (num == 5) {
			if (dp_strncmpi(param_list[4], "enable",
					strlen("enable")) == 0) {
				en_node.en |= DP_NODE_EN;
			} else if (dp_strncmpi(param_list[4], "disable",
					       strlen("disable")) == 0) {
				en_node.en |= DP_NODE_DIS;
			} else if (dp_strncmpi(param_list[4], "suspend",
					       strlen("suspend")) == 0) {
				en_node.en |= DP_NODE_SUSPEND;
			} else if (dp_strncmpi(param_list[4], "resume",
					       strlen("resume")) == 0) {
				en_node.en |= DP_NODE_RESUME;
			} else {
				pr_err("Incorrect parameter!%s%s%s\n",
				       param_list[4],
				       "\n(try help):echo help > ",
				       "/sys/kernel/debug/dp/qos");
				return count;
			}
		}

		if (dp_node_link_en_set(&en_node, 0)) {
			pr_err("dp_node_link_en_set failed\n");
			return count;
		}
	} else if (dp_strncmpi(param_list[0], "qmap_set",
			       strlen("qmap_set")) == 0) {
		struct dp_queue_map_set qmap_set = {0};

		qmap_set.q_id = dp_atoi(param_list[1]);

		if (num < 10) {
			pr_info("Wrong Parameter(try help):%s\n",
				"echo help > /sys/kernel/debug/dp/qos");
			return count;
		}
		qmap_set.map.mpe1 = dp_atoi(param_list[2]);
		qmap_set.map.mpe2 = dp_atoi(param_list[3]);
		qmap_set.map.dp_port = dp_atoi(param_list[4]);
		qmap_set.map.flowid = dp_atoi(param_list[5]);
		qmap_set.map.dec = dp_atoi(param_list[6]);
		qmap_set.map.enc = dp_atoi(param_list[7]);
		qmap_set.map.class = dp_atoi(param_list[8]);
		qmap_set.map.subif = dp_atoi(param_list[9]);
		if (dp_queue_map_set(&qmap_set, 0)) {
			pr_err("dp_queue_map_set failed\n");
			return count;
		}
	} else if (dp_strncmpi(param_list[0], "get_child",
			       strlen("get_child")) == 0) {
		struct dp_node_child node = {0};
		int idx = 0;

		if (dp_strncmpi(param_list[2], "sched", strlen("sched")) == 0) {
			node.type = DP_NODE_SCH;
			node.id.sch_id = dp_atoi(param_list[1]);
		} else if (dp_strncmpi(param_list[2], "port",
				       strlen("port")) == 0) {
			node.type = DP_NODE_PORT;
			node.id.cqm_deq_port = dp_atoi(param_list[1]);
		} else {
			pr_err("unknown type %s\n", param_list[2]);
		}

		if (dp_children_get(&node, 0)) {
			pr_err("dp_children_get failed\n");
			return count;
		}
		if (node.num)
			pr_info("Node[%d] has {%d} Children!!\n",
				node.id.q_id, node.num);
		for (idx = 0; idx < PROC_DP_MAX_LEAF; idx++) {
			if (node.child[idx].id.q_id) {
				if (node.child[idx].type == DP_NODE_SCH)
					pr_info("Child:[%d] is Sched:[%d]\n",
						idx, node.child[idx].id.q_id);
				else if (node.child[idx].type == DP_NODE_QUEUE)
					pr_info("Child:[%d] is Q:[%d]\n",
						idx, node.child[idx].id.q_id);
				else
					pr_info("Child:[%d] is FREE\n", idx);
			}
		}
	} else if (dp_strncmpi(param_list[0], "q_link",
			       strlen("q_link")) == 0) {
		struct dp_qos_link q_link = {0};
		int i = 0;

		q_link.q_id = dp_atoi(param_list[1]);

		if (dp_link_get(&q_link, 0)) {
			pr_err("dp_link_get failed\n");
			return count;
		}
		if (!q_link.n_sch_lvl) {
			pr_info("Q[%d](arbi:%d|prio:%d)->PORT[%d]\n",
				q_link.q_id, q_link.q_arbi, q_link.q_prio_wfq,
				q_link.cqm_deq_port);
		} else {
			pr_info("Q[%d](arbi:%d|prio:%d)\n",
				q_link.q_id, q_link.q_arbi,
				q_link.q_prio_wfq);
			for (i = 0; i < q_link.n_sch_lvl; i++) {
				pr_info("%s(%d):SCH[%d](arbi:%d|prio:%d)\n",
					"Parent level", i, q_link.sch[i].id,
					q_link.sch[i].arbi,
					q_link.sch[i].prio_wfq);
			}
			pr_info("Parent level(%d):PORT[%d] <Final Parent>\n",
				i, q_link.cqm_deq_port);
		}
	} else if (dp_strncmpi(param_list[0], "get_level",
			       strlen("get_level")) == 0) {
		struct dp_qos_level dp = {0};

		dp.inst = 0;

		if (dp_qos_level_get(&dp, 0)) {
			pr_err("dp_qos_level_get failed\n");
			return count;
		}
		if (dp.max_sch_lvl)
			pr_info("Q->SCH(%d)->PORT, level:[%d]\n",
				dp.max_sch_lvl, dp.max_sch_lvl);
		else
			pr_info("Q->PORT, level:[%d]\n", dp.max_sch_lvl);
	} else if (dp_strncmpi(param_list[0], "q_conf",
			       strlen("q_conf")) == 0) {
		struct dp_queue_conf q_conf = {0};

		q_conf.inst = 0;
		q_conf.q_id = dp_atoi(param_list[1]);

		if (dp_queue_conf_get(&q_conf, 0)) {
			pr_err("dp_queue_conf_get failed\n");
			return count;
		}
		if (q_conf.act & DP_NODE_EN)
			pr_info("Q(%d) action is ENABLED:[%d]\n",
				q_conf.q_id, q_conf.act);
		else
			pr_info("Q(%d) action is BLOCKED:[%d]\n",
				q_conf.q_id, q_conf.act);
		if (q_conf.drop == DP_QUEUE_DROP_WRED) {
			pr_info("Q(%d) is in WRED MODE:[%d]\n",
				q_conf.q_id, q_conf.drop);
			pr_info("Q(%d) is min_size_1:[%d]\n",
				q_conf.q_id, q_conf.min_size[0]);
			pr_info("Q(%d) is max_size_1:[%d]\n",
				q_conf.q_id, q_conf.max_size[0]);
			pr_info("Q(%d) is min_size_2:[%d]\n",
				q_conf.q_id, q_conf.min_size[1]);
			pr_info("Q(%d) is max_size_2:[%d]\n",
				q_conf.q_id, q_conf.max_size[1]);
			pr_info("Q(%d) is max_allowed:[%d]\n",
				q_conf.q_id, q_conf.wred_max_allowed);
			pr_info("Q(%d) is min_gauranteed:[%d]\n",
				q_conf.q_id, q_conf.wred_min_guaranteed);
			pr_info("Q(%d) is wred_slope_1:[%d]\n",
				q_conf.q_id, q_conf.wred_slope[0]);
			pr_info("Q(%d) is wred_slope_2:[%d]\n",
				q_conf.q_id, q_conf.wred_slope[1]);
		} else {
			pr_info("Q(%d) is in DROP TAIL MODE:[%d]\n",
				q_conf.q_id, q_conf.drop);
			pr_info("Q(%d) is min_size_1:[%d]\n",
				q_conf.q_id, q_conf.min_size[0]);
			pr_info("Q(%d) is max_size_1:[%d]\n",
				q_conf.q_id, q_conf.max_size[0]);
			pr_info("Q(%d) is min_size_2:[%d]\n",
				q_conf.q_id, q_conf.min_size[1]);
			pr_info("Q(%d) is max_size_2:[%d]\n",
				q_conf.q_id, q_conf.max_size[1]);
		}
	} else {
		pr_info("Wrong Parameter:\n");
		qos_create_qos_help();
	}
	return count;
}
