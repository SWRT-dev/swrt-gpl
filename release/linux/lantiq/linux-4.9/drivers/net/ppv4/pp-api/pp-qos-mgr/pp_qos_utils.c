/*
 * pp_qos_utils.c
 * Description: Packet Processor QoS Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2017-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_QOS_UTILS]:%s:%d: " fmt, __func__, __LINE__

#include <linux/gcd.h>
#include <linux/seq_file.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include "pp_common.h"
#include "pp_qos_common.h"
#include "pp_qos_fw.h"
#include "pp_regs.h"
#include "pp_desc.h"
#include "pp_qos_utils.h"
#include "infra.h"
#include "qos_wred_regs.h"

#define QOS_INVALID_PRIORITY   0xF

/******************************************************************************/
/*                                 stats                                      */
/******************************************************************************/

s32 qos_queues_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			  u32 num_stats, void *data)
{
	const struct queue_stats *it;
	const struct qos_node    *node;
	struct pp_qos_dev        *qdev = data;
	u32 node_id, node_phy, num;

	if (unlikely(ptr_is_null(buf) || ptr_is_null(n) ||
		     ptr_is_null(stats) || ptr_is_null(data)))
		return -EINVAL;

	pr_buf(buf, sz, *n, "\n");
	pr_buf_cat(buf, sz, *n,
		   "+------------------+-------------+-------------+-------------+-------------+\n");
	pr_buf_cat(buf, sz, *n,
		   "|       Qnum       | Q Occupancy |   Forward   |  WRED Drop  | CoDel Drop  |\n");
	pr_buf_cat(buf, sz, *n,
		   "+------------------+-------------+-------------+-------------+-------------+\n");

	node = &qdev->nodes->nodes[0];
	num  = min_t(u32, ARRAY_SIZE(qdev->nodes->nodes), num_stats);
	for_each_arr_entry(it, stats, num, node++) {
		node_phy = get_phy_from_node(qdev->nodes, node);
		node_id  = get_id_from_phy(qdev->mapping, node_phy);

		if (!it->wred.total_packets_accepted &&
		    !it->wred.total_packets_dropped)
			continue;

		pr_buf_cat(buf, sz, *n,
			   "| %-3u(%-3u)-rlm-%-3u | %-10u  | %-10u  | %-10u  | %-10u  |\n",
			   node_id, node_phy, node->data.queue.rlm,
			   it->wred.queue_packets_occupancy,
			   it->total_fwd_pkts,
			   it->wred.total_packets_dropped,
			   it->codel.packets_dropped);
	}
	pr_buf_cat(buf, sz, *n,
		   "+------------------+-------------+-------------+-------------+-------------+\n");

	return 0;
}

static void __queue_stats_diff(const struct queue_stats *pre,
			       const struct queue_stats *post,
			       struct queue_stats *delta)
{
	U32_STRUCT_DIFF(&pre->codel, &post->codel, &delta->codel);
	delta->wred.queue_packets_occupancy =
	(pre->wred.queue_packets_occupancy + post->wred.queue_packets_occupancy)
	/ 2;
	delta->wred.queue_bytes_occupancy =
	(pre->wred.queue_bytes_occupancy + post->wred.queue_bytes_occupancy)
	/ 2;
	delta->wred.total_packets_accepted = post->wred.total_packets_accepted -
					     pre->wred.total_packets_accepted;
	delta->wred.total_packets_dropped = post->wred.total_packets_dropped -
					    pre->wred.total_packets_dropped;
	delta->wred.total_packets_red_dropped =
		post->wred.total_packets_red_dropped -
		pre->wred.total_packets_red_dropped;
	delta->wred.total_bytes_accepted = post->wred.total_bytes_accepted -
					   pre->wred.total_bytes_accepted;
	delta->wred.total_bytes_dropped =
		post->wred.total_bytes_dropped - pre->wred.total_bytes_dropped;
	delta->total_fwd_pkts = post->total_fwd_pkts - pre->total_fwd_pkts;
}

s32 qos_queues_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			  void *delta, u32 num_delta, void *data)
{
	const struct queue_stats *pre_it, *post_it;
	const struct qos_node *node;
	struct pp_qos_dev     *qdev = data;
	struct queue_stats *delta_it;
	struct queue_stats org_post;
	u32 num;

	if (unlikely(ptr_is_null(pre) || ptr_is_null(post) ||
		     ptr_is_null(delta) || ptr_is_null(data)))
		return -EINVAL;

	if (unlikely(num_pre != num_post && num_pre != num_delta))
		return -EINVAL;

	post_it  = post;
	delta_it = delta;
	node     = &qdev->nodes->nodes[0];
	num      = min_t(u32, ARRAY_SIZE(qdev->nodes->nodes), num_pre);

	for_each_arr_entry(pre_it, pre, num, node++, post_it++, delta_it++) {
		if (!node_queue(node))
			continue;
		memcpy(&org_post, post_it, sizeof(org_post));
		__queue_stats_diff(pre_it, post_it, delta_it);
	}

	return 0;
}

s32 qos_queues_stats_get(void *stats, u32 num_stats, void *data)
{
	const struct qos_node *node;
	struct pp_qos_dev     *qdev = data;
	struct queue_stats    *it;
	u32 node_id, node_phy, num;
	s32 ret;

	if (unlikely(ptr_is_null(stats) || ptr_is_null(data)))
		return -EINVAL;

	node = &qdev->nodes->nodes[0];
	num  = min_t(u32, ARRAY_SIZE(qdev->nodes->nodes), num_stats);
	for_each_arr_entry(it, stats, num, node++) {
		if (!node_queue(node) || (node_rep(node)))
			continue;

		node_phy = get_phy_from_node(qdev->nodes, node);
		node_id  = get_id_from_phy(qdev->mapping, node_phy);

		ret = pp_qos_codel_queue_stat_get(qdev, node_id, &it->codel);
		if (unlikely(ret)) {
			QOS_LOG_ERR("get queue %u codel stats failed, ret %d\n",
				    node_id, ret);
			return ret;
		}
		ret = pp_qos_queue_stat_get(qdev, node_id, &it->wred);
		if (unlikely(ret)) {
			QOS_LOG_ERR("get queue %u stats failed, ret %d\n",
				    node_id, ret);
			return ret;
		}
		it->total_fwd_pkts = it->wred.total_packets_accepted -
		it->wred.queue_packets_occupancy - it->codel.packets_dropped;
	}

	return 0;
}

/******************************************************************************/
/*                                 OCTETS                                     */
/******************************************************************************/
u32 octet_get_use_count(const struct pp_octets *octets, u32 octet)
{
	return octets->info[octet].usage;
}

u32 octet_get_with_at_least_free_entries(struct pp_octets *octets, u32 count)
{
	u32 id;
	struct list_head *list;

	for (list = octets->lists + 8 - count; list >= octets->lists; --list)  {
		if (!list_empty(list)) {
			id = list_entry(list->next, struct octet_info, list) -
					octets->info;
			list_del_init(list->next);
			return id;
		}
	}

	return QOS_INVALID_OCTET;
}

u32 octet_get_least_free_entries(struct pp_octets *octets)
{
	return octet_get_with_at_least_free_entries(octets, 1);
}

static void octet_set_use_count(struct pp_octets *octets, u32 octet, u32 usage)
{
	struct list_head *list;

	QOS_ASSERT(QOS_OCTET_VALID(octet), "Illegal octet %u\n", octet);
	QOS_ASSERT(usage <= 8, "Illegal usage %u\n", usage);
	octets->info[octet].usage = usage;
	if (octet > octets->last_port_octet) {
		list = octets->lists + usage;
		list_move_tail(&octets->info[octet].list, list);
	}
}

static struct pp_octets *octets_init(u32 last_port_octet)
{
	u32 i;
	struct octet_info *info;
	struct pp_octets *octets;

	octets = kzalloc(sizeof(*octets), GFP_KERNEL);
	if (octets) {
		for (i = 0; i < 9; ++i)
			INIT_LIST_HEAD(octets->lists + i);

		info = octets->info;
		for (i = 0; i < NUM_OF_OCTETS; ++i) {
			info->usage = 0;
			if (i > last_port_octet)
				list_add_tail(&info->list, octets->lists);
			else
				INIT_LIST_HEAD(&info->list);

			++info;
		}
		octets->last_port_octet = last_port_octet;
	}
	return octets;
}

static void octets_clean(struct pp_octets *octets)
{
	kfree(octets);
}

/******************************************************************************/
/*                        Ids, Rlms and Port phys pools                       */
/******************************************************************************/
static struct pp_pool *pp_pool_init(u32 capacity, u16 invalid)
{
	struct pp_pool *tmp;

	if (!capacity)
		return NULL;

	tmp = kzalloc(sizeof(*tmp) + ((capacity - 1) * 2),
		      GFP_KERNEL);
	if (tmp) {
		tmp->capacity = capacity;
		tmp->top = 0;
		tmp->invalid = invalid;
	}
	return tmp;
}

u32 pp_pool_capacity_get(struct pp_pool *pool)
{
	return pool->top;
}

u16 pp_pool_get(struct pp_pool *pool)
{
	u16 tmp;
	u32 top;

	top = pool->top;
	if (top > 0) {
		--top;
		tmp = pool->data[top];
		pool->top = top;
		return tmp;
	}
	return pool->invalid;
}

s32 pp_pool_put(struct pp_pool *pool, u16 data)
{
	u32 top;

	top = pool->top;
	if (top < pool->capacity) {
		pool->data[top] = data;
		pool->top = top + 1;
		return 0;
	}
	return -1;
}

static void pp_pool_clean(struct pp_pool *pool)
{
	kfree(pool);
}

static struct pp_pool *free_id_init(void)
{
	u32 i;
	struct pp_pool *tmp;

	tmp  = pp_pool_init(NUM_OF_NODES, PP_QOS_INVALID_ID);
	if (tmp) {
		for (i = NUM_OF_NODES; i > 0; --i)
			pp_pool_put(tmp, i - 1);
	}

	return tmp;
}

static struct pp_pool *free_rlm_init(u32 max_queue_id, u32 queue_cnt)
{
	u32 i;
	u32 last_queue_id = max_queue_id + 1 - queue_cnt;
	struct pp_pool *tmp;

	tmp  = pp_pool_init(queue_cnt, PP_QOS_INVALID_ID);
	if (tmp) {
		for (i = max_queue_id + 1; i > last_queue_id; --i)
			pp_pool_put(tmp, i - 1);
	}

	return tmp;
}

struct pp_pool *free_ports_phys_init(u32 *reserved,
				     u32 max_ports,
				     const u32 *reserved_ports,
				     u32 size,
				     u32 *num_reserved_ports)
{
	u32 i;
	u32 tmp;
	struct pp_pool *pool;

	*num_reserved_ports = 0;
	tmp = min(size, max_ports);
	memcpy(reserved, reserved_ports, tmp * sizeof(u32));

	for (i = 0; i < tmp; ++i) {
		if (reserved[i] == 1)
			++*num_reserved_ports;
	}

	tmp = max_ports - *num_reserved_ports;
	pool  = pp_pool_init(tmp, QOS_INVALID_PHY);
	if (pool) {
		for (i = max_ports; i > 0; --i) {
			if (reserved[i - 1] == 0)
				pp_pool_put(pool, i - 1);
		}
	}
	return pool;
}

/******************************************************************************/
/*                              Sync Queue                                    */
/******************************************************************************/
static struct pp_syncq *syncq_init(u32 num_syncqs)
{
	struct pp_syncq *syncq;

	syncq = kzalloc(sizeof(struct pp_syncq), GFP_KERNEL);
	if (unlikely(!syncq)) {
		pr_err("failed to allocate syncq db\n");
		return NULL;
	}
	if (!num_syncqs)
		goto out;

	syncq->phy = kcalloc(num_syncqs, sizeof(*syncq->phy), GFP_KERNEL);
	if (unlikely(!syncq->phy)) {
		pr_err("failed to allocate syncq phy db\n");
		kfree(syncq);
		return NULL;
	}
	syncq->id = kcalloc(num_syncqs, sizeof(*syncq->id), GFP_KERNEL);
	if (unlikely(!syncq->id)) {
		pr_err("failed to allocate syncq id db\n");
		kfree(syncq->phy);
		kfree(syncq);
		return NULL;
	}

out:
	return syncq;
}

static void syncq_clean(struct pp_syncq *syncq)
{
	if (syncq) {
		kfree(syncq->phy);
		kfree(syncq->id);
		kfree(syncq);
	}
}

/******************************************************************************/
/*                                 Queue                                      */
/******************************************************************************/
u16 pp_queue_dequeue(struct pp_queue *queue)
{
	struct q_item *item;
	s32 rc;

	if (!list_empty(&queue->used)) {
		item = list_entry(queue->used.next, struct q_item, list);
		rc = item->data;
		list_move_tail(&item->list, &queue->free);
	} else {
		rc = PP_QOS_INVALID_ID;
	}
	return rc;
}

s32 pp_queue_enqueue(struct pp_queue *queue, u16 data)
{
	struct q_item *item;

	if (!list_empty(&queue->free)) {
		item = list_entry(queue->free.next, struct q_item, list);
		item->data = data;
		list_move_tail(&item->list, &queue->used);
		return 0;
	}
	QOS_LOG("queue is full\n");
	return -1;
}

static s32 queue_is_reset(struct pp_queue *queue)
{
	u32 i;
	struct list_head *pos;

	if (!list_empty(&queue->used))
		return 0;

	i = 0;
	list_for_each(pos, &queue->free) ++i;

	return (i == queue->capacity);
}

void pp_queue_reset(struct pp_queue *queue)
{
	list_splice_tail_init(&queue->used, &queue->free);
	QOS_ASSERT(queue_is_reset(queue), "Queue is not reset\n");
}

static void pp_queue_clean(struct pp_queue *queue)
{
	kfree(queue);
}

static struct pp_queue *pp_queue_init(u32 capacity)
{
	struct pp_queue *queue;
	u32 i;

	queue = kzalloc(sizeof(*queue) +
			sizeof(struct q_item) * (capacity - 1),
			GFP_KERNEL);

	if (!queue)
		return NULL;

	queue->capacity = capacity;
	INIT_LIST_HEAD(&queue->free);
	INIT_LIST_HEAD(&queue->used);
	for (i = 0; i < queue->capacity; ++i)
		list_add_tail(&queue->items[i].list, &queue->free);

	return queue;
}

/******************************************************************************/
/*                             Cmd Queue                                      */
/******************************************************************************/
#define CMD_QUEUE_SIZE 262144LU
struct cmd_queue {
	size_t read;  /* next index to read */
	size_t write; /* next free index to write */
	size_t count; /* how many bytes are in */
	size_t watermark;
	u8     data[CMD_QUEUE_SIZE];
};

static struct cmd_queue *cmd_queue_init(void)
{
	struct cmd_queue *q;

	q = kzalloc(sizeof(*q), GFP_KERNEL);
	return q;
}

static void cmd_queue_clean(struct cmd_queue *q)
{
	kfree(q);
}

s32 cmd_queue_put(struct cmd_queue *q, void *_cmd, size_t size)
{
	u32 toend;
	u8 *cmd;

	cmd = _cmd;

	if (CMD_QUEUE_SIZE - q->count < size) {
		QOS_LOG_ERR("%lu bytes free, can't accommodate %zu bytes\n",
			    CMD_QUEUE_SIZE - q->count, size);
		return -1;
	}

	toend = CMD_QUEUE_SIZE - q->write;
	if (toend >= size) {
		memcpy(q->data + q->write, cmd, size);
		q->write += size;
	} else {
		memcpy(q->data + q->write, cmd, toend);
		memcpy(q->data, cmd + toend, size - toend);
		q->write = size - toend;
	}

	q->count += size;

	if (q->watermark < q->count)
		q->watermark = q->count;

	return 0;
}

/* Copy size bytes from buffer to command,
 * return amount of bytes read
 */
static s32 cmd_queue_read(struct cmd_queue *q, void *_cmd, size_t size,
			  s32 remove)
{
	u32 toend;
	u8 *cmd;

	cmd = _cmd;

	if (q->count < size)
		return 0;

	toend = CMD_QUEUE_SIZE - q->read;
	if (toend >= size) {
		memcpy(cmd, q->data + q->read, size);
		if (remove)
			q->read += size;
	} else {
		memcpy(cmd, q->data + q->read, toend);
		memcpy(cmd + toend, q->data, size - toend);
		if (remove)
			q->read = size - toend;
	}

	if (remove)
		q->count -= size;

	return size;
}

s32 cmd_queue_get(struct cmd_queue *q, void *_cmd, size_t size)
{
	return cmd_queue_read(q, _cmd, size, 1);
}

s32 cmd_queue_peek(struct cmd_queue *q, void *_cmd, size_t size)
{
	return cmd_queue_read(q, _cmd, size, 0);
}

s32 cmd_queue_is_empty(struct cmd_queue *q)
{
	return (q->count == 0);
}

size_t cmd_queue_watermark_get(struct cmd_queue *q)
{
	return q->watermark;
}

/******************************************************************************/
/*                                 Nodes                                      */
/******************************************************************************/

/**
 * @brief Create a list in form of bitmap with all nodes that
 *        depend on the clock.
 *        Used for getting a list of nodes which needs to be updated
 *        when the clock is changed
 * @param qdev the qos device
 * @param bmap the list (bitmap)
 * @param n number of bits in the bitmap, MUST be at least NUM_OF NODES
 * @return s32 0 on success, error code otherwise
 */
s32 qos_clk_depend_nodes_list_get(struct pp_qos_dev *qdev, ulong *bmap, u32 n)
{
	const struct qos_node *node;

	if (unlikely(ptr_is_null(qdev) || ptr_is_null(bmap)))
		return -EINVAL;

	/* accept only bitmaps which have enough bits for all nodes */
	if (unlikely(n < NUM_OF_NODES)) {
		QOS_LOG_ERR("insufficient bitmap size, at least %u bits\n",
			    NUM_OF_NODES);
		return -EINVAL;
	}

	/* iterate over all nodes and set only those who are busy and
	 * has non infinite bandwidth
	 */
	for_each_qos_node_clk_dep(qdev, node)
		/* mark the node in the used bitmap */
		set_bit(get_phy_from_node(qdev->nodes, node), bmap);

	return 0;
}

static struct pp_nodes *pp_nodes_init(void)
{
	struct pp_nodes *nodes;
	u32 i;

	nodes = kzalloc(sizeof(*nodes), GFP_KERNEL);
	if (nodes) {
		for (i = 0; i < NUM_OF_NODES; ++i)
			nodes->nodes[i].dr_sched_id = PP_QOS_INVALID_ID;
	}

	return nodes;
}

u32 get_phy_from_node(const struct pp_nodes *nodes,
		      const struct qos_node *node)
{
	QOS_ASSERT(node >= nodes->nodes && node <= nodes->nodes + NUM_OF_NODES,
		   "invalid node pointer %#lx (range %#lx - %#lx)\n",
		   (unsigned long)node, (unsigned long)nodes->nodes,
		   (unsigned long)(nodes->nodes + NUM_OF_NODES));
	return node - nodes->nodes;
}

struct qos_node *get_node_from_phy(struct pp_nodes *nodes, u32 phy)
{
	if (!QOS_PHY_VALID(phy)) {
		QOS_LOG_ERR("invalid phy %u\n", phy);
		return NULL;
	}

	return nodes->nodes + phy;
}

const struct qos_node *get_const_node_from_phy(const struct pp_nodes *nodes,
					       u32 phy)
{
	if (!QOS_PHY_VALID(phy)) {
		QOS_LOG_ERR("invalid phy %u\n", phy);
		return NULL;
	}

	return nodes->nodes + phy;
}

static void pp_nodes_clean(struct pp_nodes *nodes)
{
	kfree(nodes);
}

/******************************************************************************/
/*                                 Mapping                                    */
/******************************************************************************/
u32 get_id_from_phy(const struct pp_mapping *map, u32 phy)
{
	if (!QOS_PHY_VALID(phy)) {
		QOS_LOG_ERR("invalid phy %u\n", phy);
		return NUM_OF_NODES;
	}

	return map->phy2id[phy];
}

u32 get_phy_from_id(const struct pp_mapping *map, u32 id)
{
	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("invalid id %u\n", id);
		return NUM_OF_NODES;
	}

	return map->id2phy[id];
}

void map_id_phy(struct pp_mapping *map, u32 id, u32 phy)
{
	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("invalid id %u\n", id);
		return;
	}

	if (!QOS_PHY_VALID(phy)) {
		QOS_LOG_ERR("invalid phy %u\n", phy);
		return;
	}

	map->id2phy[id] = phy;
	map->phy2id[phy] = id;
}

void map_id_reserved(struct pp_mapping *map, u32 id)
{
	QOS_ASSERT(QOS_ID_VALID(id), "invalid id %u\n", id);
	map->id2phy[id] = QOS_UNMAPPED_PHY;
}

void map_invalidate_id(struct pp_mapping *map, u32 id)
{
	QOS_ASSERT(QOS_ID_VALID(id), "invalid id %u\n", id);
	map->id2phy[id] = QOS_INVALID_PHY;
}

void map_invalidate_phy(struct pp_mapping *map, u32 phy)
{
	QOS_ASSERT(QOS_PHY_VALID(phy), "invalid phy %u\n", phy);
	map->phy2id[phy] = PP_QOS_INVALID_ID;
}

static struct pp_mapping *pp_mapping_init(void)
{
	u32 i;
	struct pp_mapping *map;

	map = kzalloc(sizeof(*map), GFP_KERNEL);
	if (map) {
		for (i = 0; i < NUM_OF_NODES; ++i) {
			map->id2phy[i] = QOS_INVALID_PHY;
			map->phy2id[i] = PP_QOS_INVALID_ID;
			map->id2rep_id[i] = PP_QOS_INVALID_ID;
		}
	}
	return map;
}

static void pp_mapping_clean(struct pp_mapping *map)
{
	kfree(map);
}

void map_invalidate_id_phy(struct pp_mapping *map, u32 id)
{
	u32 phy;

	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("invalid id %u\n", id);
		return;
	}
	phy = get_phy_from_id(map, id);

	/* If phy is not mapped verify it is valid */
	if (!QOS_PHY_UNMAPPED(phy)) {
		if (QOS_PHY_VALID(phy))
			map->phy2id[phy] = PP_QOS_INVALID_ID;
		else
			QOS_LOG_ERR("invalid phy is mapped to id %u\n", id);
	}

	map->id2phy[id] = QOS_INVALID_PHY;
}

u32 get_rep_id_from_id(const struct pp_mapping *map, u32 id)
{
	QOS_ASSERT(QOS_ID_VALID(id), "invalid id %u\n", id);
	return map->id2rep_id[id];
}

void map_id_rep_id(struct pp_mapping *map, u32 id, u32 rep_id)
{
	QOS_ASSERT(QOS_ID_VALID(id), "invalid id %u\n", id);
	QOS_ASSERT(QOS_ID_VALID(rep_id), "invalid rep_id %u\n", rep_id);
	map->id2rep_id[id] = rep_id;
}

void map_invalidate_id_rep_id(struct pp_mapping *map, u32 id)
{
	u32 rep_id;

	QOS_ASSERT(QOS_ID_VALID(id), "invalid id %u\n", id);
	rep_id = get_rep_id_from_id(map, id);

	map->id2rep_id[id] = PP_QOS_INVALID_ID;
	if (QOS_ID_VALID(rep_id))
		map->id2rep_id[rep_id] = PP_QOS_INVALID_ID;
}

/******************************************************************************/
/*                                 Moving                                     */
/******************************************************************************/
/* Return 1 if used status of all count nodes starting from first_phy equals
 * status
 */
static s32 is_all_nodes_used_status(const struct pp_qos_dev *qdev,
				    u32 first_phy, u32 count, u32 status)
{
	const struct qos_node *cur;
	u32 used;

	cur = get_const_node_from_phy(qdev->nodes, first_phy);
	if (unlikely(!cur)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    first_phy);
		return 0;
	}

	for (; count; --count) {
		used = !!QOS_BITS_IS_SET(cur->flags, NODE_FLAGS_USED);
		if (used != status)
			return 0;
		cur++;
	}
	return 1;
}

/* is child_phy a child of parent_phy */
static s32 node_child_of(struct pp_nodes *nodes,
			 u32 child_phy, u32 parent_phy)
{
	const struct qos_node *parent;

	parent = get_const_node_from_phy(nodes, parent_phy);
	if (!parent)
		return 0;

	return (child_phy >= parent->parent_prop.first_child_phy &&
		child_phy <= parent->parent_prop.first_child_phy +
		parent->parent_prop.num_of_children - 1);
}

/**
 * node_update_children_internal() - Update children's parent
 *
 * @nodes:     original nodes (qdev->nodes)
 * @tmp_nodes: a window (buffer) holding a copy of some nodes from
 *	       the original nodes
 * @first:     The phy that the first node of tmp_nodes
 *             represents
 * @count:     How many children should be updated on tmp_nodes
 * @parent:    parent
 * @new_phy:   new phy of parent
 *
 * Does not create update predecessor cmds.
 * The first count children of parent are updated on tmp_nodes
 * the rest on nodes
 */
static void node_update_children_internal(struct pp_nodes *nodes,
					  struct qos_node *tmp_nodes,
					  u32 first, u32 count,
					  struct qos_node *parent, u32 new_phy)

{
	u32 last;
	u32 cur;
	struct qos_node *child;

	cur = parent->parent_prop.first_child_phy;
	last = cur + parent->parent_prop.num_of_children - 1;
	for (; cur <= last; ++cur) {
		if (cur >= first && cur < first + count) {
			child = tmp_nodes + cur - first;
		} else {
			child = get_node_from_phy(nodes, cur);
			if (unlikely(!child)) {
				QOS_LOG_ERR("get_node_from_phy(%u) NULL\n",
					    cur);
				return;
			}
		}

		child->child_prop.parent_phy = new_phy;
	}
}

/* Update the children of a parent which is moving from phy to new_phy
 * Children parent_phy's is updated
 */
void node_update_children(struct pp_qos_dev *qdev, struct qos_node *parent,
			  u32 new_phy)
{
	u32 num;
	struct qos_node *child;

	num = parent->parent_prop.num_of_children;
	if (!num || parent->parent_prop.first_child_phy == QOS_INVALID_PHY)
		return;

	child = get_node_from_phy(qdev->nodes,
				  parent->parent_prop.first_child_phy);
	if (unlikely(!child)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    parent->parent_prop.first_child_phy);
		return;
	}

	num =  parent->parent_prop.num_of_children;
	for (; num; --num)  {
		child->child_prop.parent_phy = new_phy;
		++child;
	}
}

/* Update parent's first child phy and num of children, given that count nodes
 * starting from src_phy are moving to dst_phy
 *
 * tmpnodes - local copy of all moved nodes (not only of this parent)
 * src_first - src phy of first moved node (not only of this parent)
 * size - total number of nodes moving (not only of this parent)
 * src_phy - src phy of child
 * dst_phy - dst phy of child
 * count - how many more nodes have not move yet and should be moved
 *         not only of this parent)
 *
 * Note number of children is calculated as last_child - first_child + 1
 *
 * Returns number of moved nodes for this children
 */
static s32 node_update_parent(struct pp_qos_dev *qdev,
			      struct qos_node *tmpnodes, u32 src_first,
			      u32 size, u32 src_phy, u32 dst_phy, u32 count)
{
	u32 first;
	u32 last;
	u32 num;
	u32 parent_phy;
	u32 moving;
	struct qos_node *parent;
	struct qos_node *child;
	struct pp_nodes *nodes;

	nodes = qdev->nodes;
	child = get_node_from_phy(nodes, src_phy);
	if (unlikely(!child)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    src_phy);
		return 0;
	}

	parent_phy = child->child_prop.parent_phy;

	if (parent_phy >= src_first && parent_phy < src_first + size) {
		parent = tmpnodes + parent_phy - src_first;
	} else {
		parent = get_node_from_phy(nodes, parent_phy);
		if (unlikely(!parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    parent_phy);
			return 0;
		}
	}

	QOS_ASSERT(node_parent(parent),
		   "Node %u suppose to be a parent but is not\n",
		   get_phy_from_node(nodes, parent));

	QOS_ASSERT(node_child_of(nodes, src_phy, parent_phy),
		   "%u is not child of %u\n",
		   src_phy, parent_phy);

	first = parent->parent_prop.first_child_phy;
	num = parent->parent_prop.num_of_children;
	last = first + num - 1;

	QOS_ASSERT(same_octet(first, last),
		   "source nodes %u and %u are not in the same octet\n",
		   first, last);

	/* Number of children of this parent going to move */
	moving = min(count, num - (src_phy - first));
	QOS_ASSERT(same_octet(dst_phy, dst_phy + moving - 1),
		   "destination nodes %u and %u are not in the same octet\n",
		   dst_phy, dst_phy + moving - 1);

	if (moving == num) { /* all children are moving */
		parent->parent_prop.first_child_phy = dst_phy;
	} else {
		QOS_ASSERT(same_octet(dst_phy, src_phy),
			   "src phy %u and dst phy %u are not in same octet\n",
			   src_phy, dst_phy);

		/* first is not moving, but its sibling may left skip him */
			first = min(first, dst_phy);
		if (first != src_phy)
			first = min(first, dst_phy);

		/* first is moving, optionally skip right its sibling */
		else
			first = min(first + moving, dst_phy);

		/* last is not moving, but it sibling may right skip it */
		if (last != src_phy + moving - 1)
			last = max(last, dst_phy + moving - 1);

		/* last is moving, optionally right skip it */
		else
			last = max(last - moving, dst_phy + moving - 1);

		QOS_ASSERT(same_octet(first, last),
			   "%u and %u are in different octets\n",
			   first, last);

		parent->parent_prop.first_child_phy = first;
		parent->parent_prop.num_of_children = last - first + 1;
	}

	return moving;
}

/* Given count nodes starting at src_phy [src_phy..src_phy + count - 1],
 * that are going to move to [dst_phy..dst_phy + count - 1]:
 *
 * Update id<->phy mapping for each node that is moved
 *
 * For each node that is a parent node
 *   Update parent_phy of each of its children
 *
 * For each group of children that have the same parent
 *   Update parent's first_child_phy and num of children
 *
 */
static void nodes_update_stake_holders(struct pp_qos_dev *qdev,
				       u32 src_phy, u32 dst_phy, u32 count)
{
	u32 i;
	u32 tmp;
	struct qos_node *next_updated_parent;
	struct qos_node *node;
	struct pp_mapping *map;
	struct qos_node tmp_nodes[MAX_MOVING_NODES];
	u32 tmp_map[MAX_MOVING_NODES];
	u32 updated_parents[MAX_MOVING_NODES];
	u32 index;

	QOS_ASSERT(count < MAX_MOVING_NODES,
		   "Trying to update %u nodes but the maximum is %u\n",
		   count, MAX_MOVING_NODES);

	map = qdev->mapping;
	index = 0;

	/* Copy of all nodes that are going to move moved
	 * Changes on these nodes will be done on it and then copied
	 * to database
	 */
	memcpy(&tmp_nodes, qdev->nodes->nodes + src_phy,
	       sizeof(struct qos_node) * count);

	/* Invalidate current mapping, update children and parents of
	 * moved nodes
	 */
	node = get_node_from_phy(qdev->nodes, src_phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    src_phy);
		return;
	}

	next_updated_parent = node;
	for (i = src_phy; i < src_phy + count; ++i) {
		tmp = get_id_from_phy(map, i);
		tmp_map[i - src_phy] = tmp;
		map_invalidate_id_phy(map, tmp);
		if (node_parent(node)) {
			node_update_children_internal(qdev->nodes, tmp_nodes,
						      src_phy, count, node,
						      dst_phy + i - src_phy);
		}

		if (node_child(node) && (node >= next_updated_parent)) {
			next_updated_parent =
				node +
				node_update_parent(qdev, tmp_nodes,
						   src_phy, count,
						   i, dst_phy + i - src_phy,
						   count - (i - src_phy));

			updated_parents[index++] = node->child_prop.parent_phy;
		}

		node++;
	}

	/* map all previous invalidated ids to their new phy */
	for (i = dst_phy; i < dst_phy + count; ++i) {
		tmp = tmp_map[i - dst_phy];

		QOS_ASSERT(QOS_ID_VALID(tmp), "Invalid id %u on phy %u\n",
			   tmp, i);

		map_id_phy(map, tmp, i);
	}

	memcpy(qdev->nodes->nodes + src_phy, tmp_nodes,
	       sizeof(struct qos_node) * count);

	for (i = 0; i < index; ++i)
		update_parent(qdev, updated_parents[i]);
}

/* For small arrays this is probably quicker than
 * the heap sort kernel is using
 */
static u32 remove_duplicates(u32 *data, u32 size)
{
	u32 i;
	u32 j;

	QOS_ASSERT(size <= 2 * MAX_MOVING_NODES,
		   "This function intends for array with size up to %d\n",
		   2 * MAX_MOVING_NODES);

	for (i = 0; i < size; ++i) {
		for (j = i + 1; j < size; ++j) {
			if (data[i] == data[j]) {
				--size;
				data[j] = data[size];
				--j;
			}
		}
	}

	return size;
}

/* Given a group A of count nodes starting at start, find a group B which
 * satisfies:
 *
 * - Each element in B is a direct parent of an element from A
 * - B has no duplicates
 * - If (external)
 *	- Each element in B does not belong to A
 *	- Every element on A is a decendant of some element on B
 *   Else
 *	- Each element in B belongs to A
 *	- The parent of each element is B does not belong to A
 *	- Every element on A is either on B or has ancestor on B
 *
 *   B is stored on data and the function returns the size of B
 *
 *   Return number of elements on B
 */
static u32 parents_span(struct pp_nodes *nodes, u32 external,
			u32 start, u32 count, u32 *data, u32 size)
{
	u32 i;
	u32 parent_phy;
	u32 cur_phy;
	const struct qos_node *cur;

	QOS_ASSERT(size >= count,
		   "Array size %u is smaller than number of moved nodes %u\n",
		   size, count);
	i = 0;
	cur = get_const_node_from_phy(nodes, start);
	if (unlikely(!cur)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    start);
		return 0;
	}

	for (cur_phy = start; cur_phy < start + count; ++cur_phy) {
		QOS_ASSERT((cur->type == TYPE_UNKNOWN) || node_child(cur),
			   "Node is not a child\n");

		parent_phy = cur->child_prop.parent_phy;

		/* parent not moving */
		if ((parent_phy < start) || (parent_phy >= start + count)) {
			if (external)
				data[i] = parent_phy;
			else
				data[i] = cur_phy;
			++i;
		}
		++cur;
	}

	return remove_duplicates(data, i);
}

/* If the src and dst region don't overlap then we can use
 * either strategy of "move last src first" or "move first src first"
 * If they overlap and the move is forward (src < dst) we will use
 * "move last src first".
 * If they overlap and the move is backward (src > dst) we will use
 * "move last src first"
 */
static void create_multiple_move_cmds(struct pp_qos_dev *qdev,
				      u32 dst_phy,
				      u32 src_phy,
				      u32 count)
{
	u32 dst_port;
	u32 i;
	u32 dst;
	u32 src;
	u32 delta;

	if (src_phy < dst_phy) {
		src = src_phy + count - 1;
		dst = dst_phy + count - 1;
		delta = ~0;
	} else {
		src = src_phy;
		dst = dst_phy;
		delta = 1;
	}

	for (i = 0; i < count; ++i) {
		dst_port = get_port(qdev->nodes, dst);
		create_move_cmd(qdev, dst, src, dst_port);
		src += delta;
		dst += delta;
	}
}

/**
 * nodes_move() - Move nodes
 *
 * Implement the core functionality of moving nodes to different phy.
 * This involves updating the stake holders nodes e.g. parents and children
 * of the moved nodes, and updating the id<->phy mapping
 *
 * @qdev:
 * @dst_phy: Dest phy of first node
 * @src_phy: Source phy of first node
 * @count: Number of nodes to move
 *
 * Notes:
 * - All src nodes must be used, all dst nodes which don't overlap src
 *   nodes must be unused
 * - Number of moving nodes should be smaller than 8
 * - If not all children of parent are moving then the move
 *   must be within octet boundries - e.g. a shift
 *
 */
void nodes_move(struct pp_qos_dev *qdev, u32 dst_phy, u32 src_phy, u32 count)
{
	u32 i;
	u32 j;
	u32 ances_size;
	u32 ancestors[MAX_MOVING_NODES];
	u32 ports[MAX_MOVING_NODES * 2];
	struct qos_node *src_node, *dst_node;

	QOS_LOG_DEBUG("Moving phy %d => %d to phy %d => %d\n",
		      src_phy, src_phy + count - 1,
		      dst_phy, dst_phy + count - 1);

	QOS_ASSERT(src_phy != dst_phy,
		   "src and dst are the same %u\n",
		   src_phy);

	QOS_ASSERT(count < MAX_MOVING_NODES,
		   "Can't move %u nodes, max is %u\n",
		   count, MAX_MOVING_NODES);

	if (!count)
		return;

	j = 0;
	for (i = src_phy; i < src_phy + count; ++i) {
		ports[j] = get_port(qdev->nodes, i);
		j++;
	}

	nodes_update_stake_holders(qdev, src_phy, dst_phy, count);
	nodes_modify_used_status(qdev, src_phy, count, 0);

	QOS_ASSERT(is_all_nodes_used_status(qdev, dst_phy, count, 0),
		   "Some of the destination nodes are in used\n");

	dst_node = get_node_from_phy(qdev->nodes, dst_phy);
	if (unlikely(!dst_node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    dst_phy);
		return;
	}

	src_node = get_node_from_phy(qdev->nodes, src_phy);
	if (!src_node) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    src_phy);
		return;
	}

	memmove(dst_node, src_node, count * sizeof(struct qos_node));
	nodes_modify_used_status(qdev, dst_phy, count, 1);

	for (i = dst_phy; i < dst_phy + count; ++i) {
		ports[j] = get_port(qdev->nodes, i);
		j++;
	}

	QOS_ASSERT(j <= 2 * MAX_MOVING_NODES,
		   "Suspended ports buffer was filled\n");

	i = remove_duplicates(ports, j);
	for (j = 0; j < i; ++j)
		add_suspend_port(qdev, ports[j]);

	create_multiple_move_cmds(qdev, dst_phy, src_phy, count);

	/* Issue update preds each node whose ancestor moved */
	ances_size = parents_span(qdev->nodes, 0, dst_phy, count,
				  ancestors, MAX_MOVING_NODES);

	for (i = 0; i < ances_size; ++i)
		tree_update_predecessors(qdev, ancestors[i]);
}

/******************************************************************************/
/*                                 Generic                                    */
/******************************************************************************/

struct qos_node
*octet_get_min_sibling_group(const struct pp_qos_dev *qdev, u32 octet,
			     const struct qos_node *spc_parent,
			     u32 *num_of_children)
{
	u32 min_size;
	u32 phy;
	u32 last_phy;
	u32 child_cnt;
	const struct qos_node *cur;
	struct qos_node *cur_parent;
	struct qos_node *min_parent;

	min_size = 9;
	min_parent = NULL;

	phy = octet * 8;
	last_phy = phy + octet_get_use_count(qdev->octets, octet);
	while (phy < last_phy) {
		cur = get_const_node_from_phy(qdev->nodes, phy);
		if (unlikely(!cur)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    phy);
			return NULL;
		}

		cur_parent = get_node_from_phy(qdev->nodes,
					       cur->child_prop.parent_phy);
		if (unlikely(!cur_parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    cur->child_prop.parent_phy);
			return NULL;
		}

		child_cnt = cur_parent->parent_prop.num_of_children;

		if ((cur_parent != spc_parent) && (child_cnt < min_size)) {
			min_size = child_cnt;
			min_parent = cur_parent;
		} else if ((cur_parent == spc_parent) &&
			   (child_cnt + 1 < min_size)) {
			min_size = child_cnt + 1;
			min_parent = cur_parent;
		}

		phy += cur_parent->parent_prop.num_of_children;
	}

	*num_of_children = min_size;
	return min_parent;
}

static void update_octet_usage(struct pp_octets *octets, u32 phy, u32 delta,
			       u32 status)
{
	u32 octet;
	u32 usage;

	octet = octet_of_phy(phy);
	usage = octet_get_use_count(octets, octet);
	if (status == 0)
		usage -= delta;
	else
		usage += delta;
	octet_set_use_count(octets, octet, usage);
}

/* Set used status of count nodes starting from first_phy to status
 * Verify that current used status is different from the value to be set
 */
void nodes_modify_used_status(struct pp_qos_dev *qdev, u32 first_phy,
			      u32 count, u32 status)
{
	struct qos_node *cur;
	u32 phy, delta;

	QOS_ASSERT(is_all_nodes_used_status(qdev, first_phy, count, !status),
		   "some nodes used status is already %u\n", status);
	delta = 0;
	for (phy = first_phy; phy < first_phy + count; ++phy) {
		cur = get_node_from_phy(qdev->nodes, phy);
		if (unlikely(!cur)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    phy);
			return;
		}

		QOS_BITS_TOGGLE(cur->flags, NODE_FLAGS_USED);
		++delta;
		if (octet_phy_offset(phy) == 7) {
			update_octet_usage(qdev->octets, phy, delta, status);
			delta = 0;
		}
	}

	if (delta)
		update_octet_usage(qdev->octets, phy - 1, delta, status);
}

/**
 * octet_nodes_shift() - Shift nodes whithin octet
 * @qdev:
 * @first_phy: Phy of first node to be shifted
 * @count: Number of nodes to shift
 * @shift: Number of places to shift Negative indicates shift left,
 *	   positive shift right
 */
void octet_nodes_shift(struct pp_qos_dev *qdev, u32 first_phy, u32 count,
		       s32 shift)
{
	if (count == 0 || shift == 0)
		return;

	QOS_ASSERT(same_octet(first_phy, first_phy + count - 1) &&
		   same_octet(first_phy, first_phy + shift) &&
		   same_octet(first_phy, first_phy + shift + count - 1),
		   "shift %d places of %u nodes from %u crosses octet boundaries\n",
		   shift, count, first_phy);

	nodes_move(qdev, first_phy + shift, first_phy, count);
}

void node_phy_delete(struct pp_qos_dev *qdev, u32 phy)
{
	u32 usage;
	u32 reminder;
	struct qos_node *node;
	struct qos_node *parent;
	u32 id;
	u32 parent_phy;
	u32 update_bw;

	update_bw = 0;
	node = get_node_from_phy(qdev->nodes, phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    phy);
		return;
	}

	if (node_syncq(node))
		return;

	QOS_ASSERT(!node_parent(node) || !(node->parent_prop.num_of_children),
		   "Try to delete node %u that has children\n", phy);

	if (node_child(node)) {
		parent_phy = node->child_prop.parent_phy;
		parent = get_node_from_phy(qdev->nodes, parent_phy);
		if (unlikely(!parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    parent_phy);
			return;
		}

		if (node->child_prop.virt_bw_share) {
			update_bw = 1;
			id = get_id_from_phy(qdev->mapping, parent_phy);
		}

		/* If this is the last child of a parent than shift
		 * operation will not update the parent, since its children
		 * are not moving - we have to update it manually
		 */
		if (phy == parent->parent_prop.first_child_phy +
		    parent->parent_prop.num_of_children - 1) {
			--parent->parent_prop.num_of_children;
			update_parent(qdev, parent_phy);
		}
	}

	usage = octet_get_use_count(qdev->octets, octet_of_phy(phy));
	nodes_modify_used_status(qdev, phy, 1, 0);
	reminder = usage - (octet_phy_offset(phy) + 1);

	if (octet_of_phy(phy) > octet_of_phy(qdev->init_params.max_ports - 1))
		octet_nodes_shift(qdev, phy + 1, reminder, -1);

	if (update_bw) {
		phy = get_phy_from_id(qdev->mapping, id);
		parent = get_node_from_phy(qdev->nodes, phy);
		if (unlikely(!parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    phy);
			return;
		}

		update_internal_bandwidth(qdev, parent);
	}
}

u16 allocate_rlm(struct pp_qos_dev *qdev, u32 id, enum rlm_range range)
{
	u16 rlm;

	switch (range) {
	case RLM_RANGES_RESERVED:
		if (qdev->reserved_rlms) {
			rlm = pp_pool_get(qdev->reserved_rlms);
		} else {
			QOS_LOG_ERR("Cannot allocate reserve\n");
			rlm = pp_pool_get(qdev->rlms);
		}

	break;
	case RLM_RANGES_SYNCQ:
		if (qdev->syncq_rlms) {
			rlm = pp_pool_get(qdev->syncq_rlms);
		} else {
			QOS_LOG_ERR("Cannot allocate syncq\n");
			rlm = pp_pool_get(qdev->rlms);
		}

	break;
	case RLM_RANGES_NORMAL:
		rlm = pp_pool_get(qdev->rlms);
	break;
	default:
		QOS_LOG_ERR("Unknown RLM range\n");
		rlm = pp_pool_get(qdev->rlms);
	}

	if (rlm == QOS_INVALID_RLM) {
		QOS_LOG_ERR("Can't get free rlm\n");
		return QOS_INVALID_RLM;
	}

	QOS_LOG_DEBUG("Allocate new rlm %d (id %d)\n", rlm, id);
	map_id_phy(qdev->rlm_mapping, id, rlm);

	return rlm;
}

/**
 * @brief Get the range type to which this rlm id belongs
 */
enum rlm_range rlm_get_range_type(struct pp_qos_dev *qdev, u32 rlm)
{
	u32 range_idx;
	struct range *rng;

	for (range_idx = 0; range_idx < RLM_RANGES_MAX; range_idx++) {
		rng = &qdev->rlm_ranges[range_idx];
		if (rlm >= rng->start && rlm <= rng->end)
			return range_idx;
	}

	return RLM_RANGES_MAX;
}

void release_rlm(struct pp_qos_dev *qdev, u32 rlm)
{
	u32 id;
	enum rlm_range range;

	id = get_id_from_phy(qdev->rlm_mapping, rlm);
	QOS_ASSERT(QOS_ID_VALID(id), "Invalid id for rlm %u\n", rlm);
	map_invalidate_id_phy(qdev->rlm_mapping, id);

	range = rlm_get_range_type(qdev, rlm);
	switch (range) {
	case RLM_RANGES_RESERVED:
		if (qdev->reserved_rlms) {
			pp_pool_put(qdev->reserved_rlms, rlm);
		} else {
			QOS_LOG_ERR("Cannot push to reserved pool\n");
			pp_pool_put(qdev->rlms, rlm);
		}
	break;
	case RLM_RANGES_SYNCQ:
		if (qdev->syncq_rlms) {
			pp_pool_put(qdev->syncq_rlms, rlm);
		} else {
			QOS_LOG_ERR("Cannot push to syncq pool\n");
			pp_pool_put(qdev->rlms, rlm);
		}
	break;
	case RLM_RANGES_NORMAL:
		pp_pool_put(qdev->rlms, rlm);
	break;
	default:
		QOS_LOG_ERR("Unknown RLM range\n");
		pp_pool_put(qdev->rlms, rlm);
	}
}

void release_id(struct pp_qos_dev *qdev, u32 id)
{
	map_invalidate_id_phy(qdev->mapping, id);
	map_invalidate_id_rep_id(qdev->mapping, id);
	pp_pool_put(qdev->ids, id);
}

static void release_node_id(struct pp_qos_dev *qdev, u32 phy)
{
	u32 id;

	id = get_id_from_phy(qdev->mapping, phy);
	QOS_LOG_DEBUG("Deleting id %u phy %u\n", id, phy);
	QOS_ASSERT(QOS_ID_VALID(id), "Invalid id for phy %u\n", phy);
	release_id(qdev, id);
}

struct qos_node *get_conform_node(const struct pp_qos_dev *qdev,
				  u32 id,
				  bool (*conform)(const void *node))
{
	struct qos_node *node;
	u32 phy;

	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("Illegal id %u\n", id);
		return NULL;
	}

	phy = get_phy_from_id(qdev->mapping, id);
	if (!QOS_PHY_VALID(phy)) {
		QOS_LOG_ERR("Illegal phy (id %u)\n", id);
		return NULL;
	}

	node = get_node_from_phy(qdev->nodes, phy);
	if (conform && !conform(node)) {
		QOS_LOG_ERR("Node conform fail for id %u (type %d, used %d)\n",
			    id, node->type, node_used(node));
		return NULL;
	}

	return node;
}

s32 node_remove(struct pp_qos_dev *qdev, struct qos_node *node)
{
	u32 phy, rlm, id, mapped_rlm;
	enum node_type type;

	phy = get_phy_from_node(qdev->nodes, node);
	type = node->type;
	QOS_ASSERT(type != TYPE_UNKNOWN, "Unknown node type of phy %u\n", phy);

	if (node_queue(node)) {
		id = get_id_from_phy(qdev->mapping, phy);
		mapped_rlm = get_phy_from_id(qdev->rlm_mapping, id);
		rlm = node->data.queue.rlm;

		if (QOS_PHY_VALID(mapped_rlm)) {
			if (node->data.queue.is_alias) {
				QOS_ASSERT(0, "valid aliased mapped rlm %u\n",
					   phy);
				return -EINVAL;
			}

			QOS_ASSERT(mapped_rlm == rlm,
				   "mapped_rlm %u != node's rlm %u\n",
				   mapped_rlm, rlm);
			create_remove_node_cmd(qdev, type, phy, rlm);
			release_rlm(qdev, rlm);
		} else if (QOS_PHY_UNMAPPED(mapped_rlm)) {
			if (node->data.queue.is_alias)
				create_remove_node_cmd(qdev, type, phy, rlm);
			map_invalidate_id(qdev->rlm_mapping, id);
		} else {
			QOS_ASSERT(0, "invalid mapped rlm %u mapped %u\n",
				   phy, mapped_rlm);
			return -EINVAL;
		}
	} else { /* Port/Scheduler */
		rlm = 0;
		create_remove_node_cmd(qdev, type, phy, rlm);
	}

	release_node_id(qdev, phy);

	node_phy_delete(qdev, get_phy_from_node(qdev->nodes, node));

	return 0;
}

s32 node_flush(struct pp_qos_dev *qdev, struct qos_node *node)
{
	u32 phy;
	u32 id;
	u32 rlm;
	s32 rc;

	phy = get_phy_from_node(qdev->nodes, node);
	id = get_id_from_phy(qdev->mapping, phy);
	QOS_ASSERT(QOS_ID_VALID(id), "Invalid id for phy %u\n", phy);

	if (node->type != TYPE_QUEUE) {
		QOS_LOG_ERR("Node %u(%u) is not a queue can't flushed\n",
			    id, phy);
		return -EINVAL;
	}

	rlm  = node->data.queue.rlm;
	create_flush_queue_cmd(qdev, rlm);
	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);

	return rc;
}

s32 qos_queue_flush(struct pp_qos_dev *qdev, u32 id)
{
	struct qos_node *node;

	node = get_conform_node(qdev, id, node_queue);
	if (!node)
		return -EINVAL;
	return node_flush(qdev, node);
}

static s32 calc_gcd_for_n_numbers(s32 arr[], s32 count)
{
	s32 res = arr[0];
	s32 ind;

	for (ind = 1; ind < count; ind++)
		res = gcd(arr[ind], res);

	if (res == 0) {
		QOS_LOG_DEBUG("GCD calc over %d arguments resulted with 0\n",
			      count);
		res++;
	}

	return res;
}

static void send_bw_weight_command(struct pp_qos_dev *qdev,
				   const struct qos_node *child, s32 bw_weight)
{
	struct pp_qos_sched_conf s_conf;
	struct pp_qos_queue_conf q_conf;
	u32    modified = 0;

	QOS_LOG_DEBUG("child %u: Sending new BW weight %d\n",
		      get_phy_from_node(qdev->nodes, child),
		      bw_weight);

	QOS_BITS_SET(modified, QOS_MODIFIED_BW_WEIGHT);

	if (node_sched(child)) {
		get_node_prop(qdev, child, &s_conf.common_prop,
			      &s_conf.sched_parent_prop,
			      &s_conf.sched_child_prop);

		s_conf.sched_child_prop.wrr_weight = bw_weight;

		create_set_sched_cmd(qdev, &s_conf, child, modified);
	} else if (node_queue(child)) {
		get_node_prop(qdev, child, &q_conf.common_prop,
			      NULL,
			      &q_conf.queue_child_prop);

		q_conf.queue_child_prop.wrr_weight = bw_weight;

		create_set_queue_cmd(qdev, &q_conf,
				     get_phy_from_node(qdev->nodes, child),
				     child, modified,
				     node_new_syncq(child));
	}
}

static void get_children_bw_weight(const struct pp_qos_dev *qdev,
				   const struct qos_node *parent,
				   s32 weights[], s32 *num_weights)
{
	struct qos_node *child;
	u32 cnt;

	*num_weights = 0;

	if (!parent->parent_prop.num_of_children)
		return;

	child = get_node_from_phy(qdev->nodes,
				  parent->parent_prop.first_child_phy);
	if (unlikely(!child)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    parent->parent_prop.first_child_phy);
		return;
	}

	cnt = parent->parent_prop.num_of_children;

	for (; cnt; --cnt) {
		QOS_ASSERT(node_child(child), "Node phy %d is not a child\n",
			   get_phy_from_node(qdev->nodes, child));
		if (child->child_prop.virt_bw_share) {
			weights[(*num_weights)++] =
				child->child_prop.virt_bw_share;
		}
		++child;
	}
}

void set_children_wsp_weight(struct pp_qos_dev *qdev,
			     const struct qos_node *parent)
{
	struct qos_node *child;
	u32 cnt;

	/* Update weights only if parent has children */
	if (!parent->parent_prop.num_of_children)
		return;

	child = get_node_from_phy(qdev->nodes,
				  parent->parent_prop.first_child_phy);
	cnt = parent->parent_prop.num_of_children;

	for (; cnt; --cnt) {
		QOS_ASSERT(node_child(child), "Node is not a child\n");
		child->child_prop.bw_weight = QOS_CHILD_UNLIMITED_WEIGHT;
		send_bw_weight_command(qdev, child, QOS_CHILD_UNLIMITED_WEIGHT);
		++child;
	}
}

static void set_children_bw_weight(struct pp_qos_dev *qdev,
				   const struct qos_node *parent, s32 gcd)
{
	struct qos_node *child;
	u32 bw_weight, cnt;

	if (!parent->parent_prop.num_of_children)
		return;

	child = get_node_from_phy(qdev->nodes,
				  parent->parent_prop.first_child_phy);
	if (unlikely(!child)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    parent->parent_prop.first_child_phy);
		return;
	}

	cnt = parent->parent_prop.num_of_children;

	for (; cnt; --cnt) {
		QOS_ASSERT(node_child(child), "Node is not a child\n");
		bw_weight = child->child_prop.virt_bw_share / gcd;
		/* make sure bw_weight will not exceed the maximum */
		bw_weight = min_t(u32, bw_weight, QOS_MAX_CHILD_WRR_WEIGHT);
		child->child_prop.bw_weight = bw_weight;
		send_bw_weight_command(qdev, child, bw_weight);
		++child;
	}
}

void update_internal_bandwidth(struct pp_qos_dev *qdev,
			       struct qos_node *parent)
{
	u32 share;
	u16 cur_parent_phy;
	struct qos_node *cur_parent;
	struct qos_node *internals[10];
	s32 index; // Should not be unsigned
	s32 weights[10] = { 0 };
	s32 num_weights = 0;
	s32 gcd = 0;

	/* Update weights only in WRR. Currently WSP acts as SP */
	if (parent->parent_prop.arbitration != PP_QOS_ARBITRATION_WRR)
		return;

	cur_parent = parent;
	index = 0;

	while (node_internal(cur_parent)) {
		QOS_ASSERT(node_internal(cur_parent), "Node is not internal");
		share = get_children_bandwidth_share(qdev, cur_parent);
		cur_parent->child_prop.virt_bw_share = share;
		internals[index++] = cur_parent;
		cur_parent_phy = cur_parent->child_prop.parent_phy;
		cur_parent = get_node_from_phy(qdev->nodes, cur_parent_phy);
		if (unlikely(!cur_parent)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    cur_parent_phy);
			return;
		}
	}

	// We arrive here with all internal schedulers added to internals[]
	// array. In addition, the uper level concrete parent (not IS) is held
	// in cur_parent variable.

	// Even if no IS, we still want to enter this "for" once
	for (; index >= 0; index--) {
		num_weights = 0;
		get_children_bw_weight(qdev, cur_parent, weights, &num_weights);
		gcd = calc_gcd_for_n_numbers(weights, num_weights + 1);
		set_children_bw_weight(qdev, cur_parent, gcd);

		if (index > 0)
			cur_parent = internals[index - 1];
	}
}

/**
 * link_with_parent() - Link a new node to a parent
 * @qdev:
 * @phy:	The phy of the node
 * @parent_id:  Id of a parent to linked with
 *
 * Note node is marked as used
 */
static void link_with_parent(struct pp_qos_dev *qdev,
			     u32 phy, u32 parent_id)
{
	struct qos_node *node;
	struct qos_node *parent;
	u32 first;
	u32 last;
	u32 parent_phy;
	u32 num;

	parent_phy = get_phy_from_id(qdev->mapping, parent_id);
	parent = get_node_from_phy(qdev->nodes, parent_phy);
	if (unlikely(!parent)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    parent_phy);
		return;
	}

	node = get_node_from_phy(qdev->nodes, phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    phy);
		return;
	}

	node->child_prop.parent_phy = parent_phy;
	num = parent->parent_prop.num_of_children;
	first = parent->parent_prop.first_child_phy;
	last = first + num - 1;

	if (num == 0 || phy < first)
		first = phy;

	if (num == 0 || phy > last)
		last = phy;

	parent->parent_prop.num_of_children = last - first + 1;
	parent->parent_prop.first_child_phy = first;
	update_parent(qdev, parent_phy);
}

s32 qos_pools_init(struct pp_qos_dev *qdev, u32 max_ports, u32 max_queues,
		   u32 reserved_queues, u32 synq_queues)
{
	u32 max_q_idx;
	u32 start_q_idx;

	qdev->octets = octets_init(octet_of_phy(max_ports - 1));
	if (!qdev->octets)
		goto fail;

	qdev->nodes = pp_nodes_init();
	if (!qdev->nodes)
		goto fail;

	qdev->ids = free_id_init();
	if (!qdev->ids)
		goto fail;

	if (reserved_queues) {
		start_q_idx = max_queues - reserved_queues;
		max_q_idx = max_queues - 1;
		qdev->reserved_rlms = free_rlm_init(max_q_idx, reserved_queues);
		qdev->rlm_ranges[RLM_RANGES_RESERVED].start = start_q_idx;
		qdev->rlm_ranges[RLM_RANGES_RESERVED].end = max_q_idx;

		if (!qdev->reserved_rlms)
			goto fail;
	} else {
		qdev->reserved_rlms = NULL;
		qdev->rlm_ranges[RLM_RANGES_RESERVED].start = max_queues;
		qdev->rlm_ranges[RLM_RANGES_RESERVED].end = max_queues;
	}

	if (synq_queues) {
		start_q_idx = max_queues - reserved_queues - synq_queues;
		max_q_idx = max_queues - 1 - reserved_queues;
		qdev->syncq_rlms = free_rlm_init(max_q_idx, synq_queues);
		qdev->rlm_ranges[RLM_RANGES_SYNCQ].start = start_q_idx;
		qdev->rlm_ranges[RLM_RANGES_SYNCQ].end = max_q_idx;

		if (!qdev->syncq_rlms)
			goto fail;
	} else {
		qdev->syncq_rlms = NULL;
		qdev->rlm_ranges[RLM_RANGES_SYNCQ].start = max_queues;
		qdev->rlm_ranges[RLM_RANGES_SYNCQ].end = max_queues;
	}

	start_q_idx = 0;
	max_q_idx = max_queues - 1 - reserved_queues - synq_queues;
	qdev->rlms = free_rlm_init(max_q_idx,
				   max_queues - reserved_queues - synq_queues);
	qdev->rlm_ranges[RLM_RANGES_NORMAL].start = start_q_idx;
	qdev->rlm_ranges[RLM_RANGES_NORMAL].end = max_q_idx;

	if (!qdev->rlms)
		goto fail;

	qdev->mapping = pp_mapping_init();
	if (!qdev->mapping)
		goto fail;

	qdev->rlm_mapping = pp_mapping_init();
	if (!qdev->rlm_mapping)
		goto fail;

	qdev->queue = pp_queue_init(1024);
	if (!qdev->queue)
		goto fail;

	return 0;

fail:
	return -ENOMEM;
}

void qos_pools_clean(struct pp_qos_dev *qdev)
{
	pp_nodes_clean(qdev->nodes);
	qdev->nodes = NULL;

	pp_pool_clean(qdev->ids);
	qdev->ids = NULL;

	pp_pool_clean(qdev->rlms);
	qdev->rlms = NULL;

	pp_pool_clean(qdev->reserved_rlms);
	qdev->rlms = NULL;

	octets_clean(qdev->octets);
	qdev->octets = NULL;

	pp_mapping_clean(qdev->mapping);
	qdev->mapping = NULL;

	pp_mapping_clean(qdev->rlm_mapping);
	qdev->rlm_mapping = NULL;

	pp_queue_clean(qdev->queue);
	qdev->queue = NULL;
}

s32 qos_dev_init_db(struct pp_qos_dev *qdev, u32 max_ports, u32 max_queues,
		    u32 reserved_queues, u32 num_syncqs)
{
	u32 i, id;

	QOS_LOG_DEBUG("\n");

	id = qdev->id;
	memset(qdev, 0, sizeof(struct pp_qos_dev));
	qdev->id = id;
	qdev->init_params.max_ports = max_ports;

	QOS_LOCK_INIT(qdev);

	if (qos_pools_init(qdev, max_ports, max_queues, reserved_queues,
			   num_syncqs))
		goto fail;

	qdev->syncq = syncq_init(num_syncqs);
	if (!qdev->syncq)
		goto fail;
	qdev->init_params.num_syncqs = num_syncqs;

	qdev->drvcmds.cmdq = cmd_queue_init();
	if (!qdev->drvcmds.cmdq)
		goto fail;

	qdev->drvcmds.pendq = cmd_queue_init();
	if (!qdev->drvcmds.pendq)
		goto fail;

	for (i = 0; i <= QOS_MAX_SHARED_BANDWIDTH_GROUP; ++i)
		qdev->groups[i].used = 0;

	return 0;
fail:
	return -ENOMEM;
}

void qos_dev_clear_db(struct pp_qos_dev *qdev)
{
	QOS_LOG_DEBUG("\n");

	pp_pool_clean(qdev->portsphys);
	qdev->portsphys = NULL;

	clean_fwdata_internals(qdev);
	cmd_queue_clean(qdev->drvcmds.pendq);
	qdev->drvcmds.pendq = NULL;

	cmd_queue_clean(qdev->drvcmds.cmdq);
	qdev->drvcmds.cmdq = NULL;

	qos_pools_clean(qdev);

	syncq_clean(qdev->syncq);
	qdev->syncq = NULL;
}

/******************************************************************************/
/*                           Tree traversal                                   */
/******************************************************************************/
/**
 * post_order_travers_tree() - Traverse subtree post order - children first
 * followed by parents and apply operation on each conformed node
 *
 * @qdev:
 * @root:	Phy of sub tree's root node
 * @conform:    Function to see if the node we reach is conform
 * @cdata:	Data to supply to conform
 * @operation:  Operation to apply upon each conformed node
 * @odata:      Data to supply to operation
 * Return:	Sum of the returned values on all operations applied at the
 * subtree
 *
 * Note - though operation is allowed to modify qdev, it is important that it
 * won't do operation that modifies tree topology (e.g. move/shift/delete/add
 * node), since this will mess the iteration process
 */
static int
post_order_travers_tree(struct pp_qos_dev *qdev, s32 root,
			s32 (*conform)(const struct pp_qos_dev *qdev,
				       const struct qos_node *node,
				       void *cdata),
			void *cdata,
			s32 (*operation)(struct pp_qos_dev *qdev,
					 struct qos_node *node,
					 void *odata),
			void *odata)
{
	u32 i;
	u32 phy;
	u32 total;
	struct qos_node *node;

	total = 0;
	node = get_node_from_phy(qdev->nodes, root);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    root);
		return 0;
	}

	QOS_ASSERT(node_used(node), "Unused node phy %d\n", root);

	if (node_parent(node)) {
		for (i = 0; i < node->parent_prop.num_of_children; ++i) {
			phy = node->parent_prop.first_child_phy + i;

			/* Do not try to update the about to be added child */
			if (node_sched(node) &&
			    (node->data.sched.new_child_phy == phy)) {
				node->data.sched.new_child_skipped = 1;
				QOS_LOG_DEBUG("Skipping child %u (Sched %u)\n",
					      phy, root);
				continue;
			}

			total += post_order_travers_tree(qdev, phy, conform,
							 cdata, operation,
							 odata);
		}
	}

	if (conform(qdev, node, cdata))
		total += operation(qdev, node, odata);

	return total;
}

struct ids_container_metadata {
	u32 next;
	u16 *ids;
	u32 size;
};

struct wsp_container_metadata {
	u32 next;
	struct wsp_queue_t *info;
	u32 size;
};

static s32 update_ids_container(struct pp_qos_dev *qdev,
				struct qos_node *node, void *data)
{
	struct ids_container_metadata *ids;
	u32 phy;
	u16 id;

	ids = (struct ids_container_metadata *)data;
	phy = get_phy_from_node(qdev->nodes, node);
	id = get_id_from_phy(qdev->mapping, phy);

	if (likely(ids->next < ids->size)) {
		ids->ids[ids->next] = id;
		ids->next++;
	}

	return 1;
}

bool is_q_ewsp(struct pp_qos_dev *qdev, u32 id)
{
	struct wsp_queue_t *wsp_info;
	u32 num_queues, idx, rlm;

	num_queues = *(u32 *)qdev->hwconf.wsp_queues_ddr_virt;
	wsp_info = (struct wsp_queue_t *)(qdev->hwconf.wsp_queues_ddr_virt + 4);
	rlm = get_phy_from_id(qdev->rlm_mapping, id);

	for (idx = 0; idx < num_queues; idx++, wsp_info++) {
		if (rlm == wsp_info->rlm)
			return true;
	}

	return false;
}

static s32 update_wsp_container(struct pp_qos_dev *qdev, struct qos_node *node,
				void *data)
{
	struct wsp_container_metadata *wsp_info;
	u32 phy, rlm;

	wsp_info = (struct wsp_container_metadata *)data;
	phy = get_phy_from_node(qdev->nodes, node);
	rlm = node->data.queue.rlm;

	if (wsp_info->next < wsp_info->size) {
		wsp_info->info[wsp_info->next].node_id = phy;
		wsp_info->info[wsp_info->next].rlm = rlm;
		wsp_info->next++;
		return 1;
	} else {
		return 0;
	}
}

static s32 update_bwl_buffer_node(struct pp_qos_dev *qdev,
				  struct qos_node *node, void *data)
{
	u32 phy;
	unsigned long bwl_entry_addr;
	unsigned long sbwl_entry_addr;
	struct bwl_ddr_info_t *bwl_info;

	phy = get_phy_from_node(qdev->nodes, node);
	bwl_entry_addr = (ulong)qdev->hwconf.bwl_ddr_virt +
			 (phy * sizeof(struct bwl_ddr_info_t));
	bwl_info = (struct bwl_ddr_info_t *)bwl_entry_addr;
	bwl_info->bw_limit = node->bandwidth_limit;
	bwl_info->max_burst = node->max_burst;

	sbwl_entry_addr = (ulong)qdev->hwconf.sbwl_ddr_virt + (phy * 4);
	*(u32 *)(sbwl_entry_addr) = (u32)node->shared_bw_group;

	return 1;
}

struct rlm_container_metadata {
	u32 next;
	u16 *rlm;
	u32 size;
};

static s32 update_rlm_container(struct pp_qos_dev *qdev,
				struct qos_node *node, void *data)
{
	struct rlm_container_metadata *rlms;
	u16 rlm;

	rlms = (struct rlm_container_metadata *)data;
	rlm = node->data.queue.rlm;

	if (likely(rlms->next < rlms->size)) {
		rlms->rlm[rlms->next] = rlm;
		rlms->next++;
	}

	return 1;
}

static s32 node_queue_wrapper(const struct pp_qos_dev *qdev,
			      const struct qos_node *node, void *data)
{
	return node_used(node) && (node->type == TYPE_QUEUE);
}

/**
 * @brief Conforms user visible queue nodes
 */
static s32 node_user_queue_wrapper(const struct pp_qos_dev *qdev,
				   const struct qos_node *node, void *data)
{
	return node_queue(node) && !node_rep(node);
}

static int node_active_queue_wrapper(const struct pp_qos_dev *qdev,
				     const struct qos_node *node, void *data)
{
	return node_used(node) && (node->type == TYPE_QUEUE) &&
		(!QOS_BITS_IS_SET(node->flags, NODE_FLAGS_QUEUE_BLOCKED));
}

void get_user_queues(struct pp_qos_dev *qdev, u32 phy, u16 *queue_ids, u32 size,
		     u32 *queues_num)
{
	struct ids_container_metadata data = {0, queue_ids, size};

	if (unlikely(!queue_ids))
		data.size = 0;

	*queues_num =
		post_order_travers_tree(qdev, phy, node_user_queue_wrapper,
					NULL, update_ids_container, &data);
}

void get_node_queues(struct pp_qos_dev *qdev, u32 phy, u16 *queue_ids, u32 size,
		     u32 *queues_num)
{
	struct ids_container_metadata data = {0, queue_ids, size};

	if (unlikely(!queue_ids))
		data.size = 0;

	*queues_num =
		post_order_travers_tree(qdev, phy, node_queue_wrapper,
					NULL, update_ids_container, &data);
}

u32 get_dr_sched_id(const struct pp_qos_dev *qdev, u32 id)
{
	struct qos_node *node;
	u32 phy = get_phy_from_id(qdev->mapping, id);
	u32 port;

	port = get_port(qdev->nodes, phy);

	node = get_node_from_phy(qdev->nodes, port);

	return node->dr_sched_id;
}

/**
 * get_node_direct_queues() - Retrieves the node's direct queues
 * @qdev: handle to qos device instance obtained previously from qos_dev_init
 * @node: Parent node id
 * @queue: Physical queue node id list
 * @queues_num: Number of direct queus under node
 */
static void get_node_direct_queues(const struct pp_qos_dev *qdev,
				   struct qos_node *node, u32 *queue,
				   u32 *queues_num)
{
	u32 child_idx, child_phy, child_id;
	u32 dr_sched_id, dr_sched_child_phy;
	struct qos_node *child, *dr_child;
	struct parent_node_properties *pprop;

	*queues_num = 0;

	pprop = &node->parent_prop;
	for (child_idx = 0; child_idx < pprop->num_of_children; child_idx++) {
		child_phy = pprop->first_child_phy + child_idx;
		child = get_node_from_phy(qdev->nodes, child_phy);

		/* Count direct queues + priority 7 queue under dr sced */
		if (node_queue(child)) {
			*queue = child_phy;
			*queues_num = *queues_num + 1;
			queue++;
		} else if (node_rep(child)) {
			/* Verify if this is dr sched */
			child_id = get_id_from_phy(qdev->mapping, child_phy);
			dr_sched_id = get_dr_sched_id(qdev, child_id);

			/* Do not check non dr sched replica */
			if (child_id != dr_sched_id)
				continue;

			/* Fetch queue under DR Sched if exist */
			dr_sched_child_phy = child->parent_prop.first_child_phy;
			dr_child = get_node_from_phy(qdev->nodes,
						     dr_sched_child_phy);

			if (node_queue(dr_child)) {
				*queue = dr_sched_child_phy;
				*queues_num = *queues_num + 1;
				queue++;
			}
		}
	}
}

static s32 node_bw_limited_wrapper(const struct pp_qos_dev *qdev,
				   const struct qos_node *node, void *data)
{
	return node_used(node) &&
		(node->bandwidth_limit || node->shared_bw_group);
}

static s32 queue_node_nonblocked_wrapper(const struct pp_qos_dev *qdev,
					 const struct qos_node *node,
					 void *data)
{
	return node_queue(node) &&
		!QOS_BITS_IS_SET(node->flags, NODE_FLAGS_QUEUE_BLOCKED);
}

static u32 get_wsp_position(u32 *wsp_queue_list, u32 wsp_queue_phy)
{
	u32 idx;

	for (idx = 0; idx < MAX_WSP_CHILDREN; idx++) {
		if (wsp_queue_list[idx] == wsp_queue_phy)
			return idx;
	}

	return MAX_WSP_CHILDREN;
}

static bool node_dr_sched(const struct pp_qos_dev *qdev,
			  const struct qos_node *node)
{
	struct qos_node *parent;

	if (node_port(node))
		return false;

	parent = get_node_from_phy(qdev->nodes, node->child_prop.parent_phy);
	if (node_rep(node) && node_port(parent))
		return true;

	return false;
}

/**
 * @brief Check whether node should be considered as ewsp
 */
static s32 queue_node_wsp_wrapper(const struct pp_qos_dev *qdev,
				  const struct qos_node *node, void *data)
{
	u32 phy, wsp_priority, num_high_prio_queues;
	u32 num_wsp_children, wsp_queue_list[MAX_WSP_CHILDREN];
	bool wsp_queue, high_prio_q = false;
	struct qos_node *parent;

	if (node_port(node))
		return 0;

	/* If replica, Accept only DR Sched since it might contain a valid q */
	if (node_rep(node) && !node_dr_sched(qdev, node))
		return 0;

	parent = get_node_from_phy(qdev->nodes, node->child_prop.parent_phy);
	wsp_queue = node_queue(node) &&
		    (parent->parent_prop.arbitration == PP_QOS_ARBITRATION_WSP);

	if (wsp_queue) {
		get_node_direct_queues(qdev, parent, wsp_queue_list,
				       &num_wsp_children);

		/* Do not consider 2 lowest WSP queues as high priority since
		 * EWSP algorithm can perform well without monitoring them
		 */
		if (num_wsp_children > 2) {
			phy = get_phy_from_node(qdev->nodes, node);
			num_high_prio_queues = num_wsp_children - 2;
			wsp_priority = get_wsp_position(wsp_queue_list, phy);

			if ((wsp_priority != MAX_WSP_CHILDREN) &&
			    (wsp_priority < num_high_prio_queues))
				high_prio_q = true;
		}
	}

	return wsp_queue && high_prio_q;
}

void update_bwl_buffer(struct pp_qos_dev *qdev, u32 phy)
{
	volatile u32 bwl_val, sbwl_val;

	post_order_travers_tree(qdev, phy,
				node_bw_limited_wrapper, NULL,
				update_bwl_buffer_node, NULL);

	pp_cache_writeback(qdev->hwconf.bwl_ddr_virt, qdev->hwconf.bwl_sz);
	pp_cache_writeback(qdev->hwconf.sbwl_ddr_virt, qdev->hwconf.sbwl_sz);

	/* Read last address to make sure data is written */
	bwl_val = *(u32 *)(qdev->hwconf.bwl_ddr_virt +
			   qdev->hwconf.bwl_sz - 4);

	sbwl_val = *(u32 *)(qdev->hwconf.sbwl_ddr_virt +
			    qdev->hwconf.sbwl_sz - 4);
}

void get_active_port_rlms(struct pp_qos_dev *qdev, u32 phy,
			  u16 *rlms, u32 size, u32 *queues_num)
{
	struct rlm_container_metadata data = {0, rlms, size};

	if (unlikely(!rlms))
		data.size = 0;

	*queues_num = post_order_travers_tree(qdev, phy,
					      node_active_queue_wrapper, NULL,
					      update_rlm_container, &data);
}

/**
 * @define Use only 6 high priority queues
 */
#define QOS_ENHANCED_WSP_MAX_QUEUES_PER_PORT 6

/**
 * @brief Traverse all enhanced WSP ports and compose a WSP queue list
 */
static void get_wsp_info(struct pp_qos_dev *qdev, struct wsp_queue_t *wsp_info,
			 u32 size, u32 *queues_num)
{
	struct wsp_container_metadata data;
	struct qos_node *node;
	u32 phy;

	if (unlikely(!wsp_info))
		data.size = 0;

	*queues_num = 0;

	/* Iterate all ports */
	for (phy = 0; phy < qdev->init_params.max_ports; phy++) {
		node = get_node_from_phy(qdev->nodes, phy);
		if (!node_enhanced_wsp_port(node))
			continue;

		if (*queues_num == size)
			return;

		data.next = 0;
		data.info = wsp_info + *queues_num;
		data.size = min_t(u32, QOS_ENHANCED_WSP_MAX_QUEUES_PER_PORT,
				  size - *queues_num);
		*queues_num +=
			post_order_travers_tree(qdev, phy,
						queue_node_wsp_wrapper, NULL,
						update_wsp_container, &data);
	}
}

void update_wsp_info(struct pp_qos_dev *qdev)
{
	struct wsp_queue_t *wsp_info;
	u32 num_enahnced_wsp_queues;

	wsp_info = qdev->hwconf.wsp_queues_ddr_virt + 4;
	memset(wsp_info, 0, qdev->hwconf.wsp_queues_sz);
	get_wsp_info(qdev, wsp_info, QOS_ENHANCED_WSP_MAX_QUEUES,
		     &num_enahnced_wsp_queues);

	qdev->ewsp_cfg.num_queues = num_enahnced_wsp_queues;
	*(u32 *)qdev->hwconf.wsp_queues_ddr_virt = num_enahnced_wsp_queues;

	pp_cache_writeback(qdev->hwconf.wsp_queues_ddr_virt,
			   qdev->hwconf.wsp_queues_sz);
}

static s32 update_nonblocked_count(struct pp_qos_dev *qdev,
				   struct qos_node *node, void *data)
{
	return 1;
}

void get_num_nonblocked_queues(struct pp_qos_dev *qdev,
			       u32 phy, u32 *queues_num)
{
	*queues_num = post_order_travers_tree(qdev, phy,
					      queue_node_nonblocked_wrapper,
					      NULL, update_nonblocked_count,
					      NULL);
}

static s32 node_in_grp(const struct pp_qos_dev *qdev,
		       const struct qos_node *node, void *data)
{
	return node_used(node) &&
		(node->shared_bw_group == (uintptr_t)data);
}

void get_bw_grp_members_under_node(struct pp_qos_dev *qdev, u32 id, u32 phy,
				   u16 *ids, u32 size, u32 *ids_num)
{
	struct ids_container_metadata data = {0, ids, size};

	if (!ids)
		data.size = 0;

	*ids_num = post_order_travers_tree(qdev, phy, node_in_grp,
					   (void *)(uintptr_t)id,
					   update_ids_container, &data);
}

s32 update_predecessors(struct pp_qos_dev *qdev,
			struct qos_node *node, void *data)
{
	bool queue_port_changed = false;
	u32  queue_port = QOS_INVALID_PHY;
	u32  queue_id = PP_QOS_INVALID_ID;
	u32  phy = get_phy_from_node(qdev->nodes, node);

	if (node_queue(node)) {
		queue_id  = get_id_from_phy(qdev->mapping, phy);
		queue_port = get_port(qdev->nodes, phy);
		if (queue_port != node->data.queue.port_phy) {
			QOS_LOG_DEBUG("Queue %u port changed from %u to %u\n",
				      queue_id,
				      node->data.queue.port_phy,
				      queue_port);
			node->data.queue.port_phy = queue_port;
			queue_port_changed = true;
		}
	}

	update_preds(qdev, phy, queue_port_changed);
	return 1;
}

static s32 node_child_wrapper(const struct pp_qos_dev *qdev,
			      const struct qos_node *node, void *data)
{
	return node_child(node);
}

void tree_update_predecessors(struct pp_qos_dev *qdev, u32 phy)
{
	post_order_travers_tree(qdev, phy, node_child_wrapper,
				NULL, update_predecessors, NULL);
}

/* Return the sum of bandwidth share of all direct children of parent */
u32 get_children_bandwidth_share(const struct pp_qos_dev *qdev,
				 const struct qos_node *parent)
{
	u32 total;
	u32 num;
	u16 first_child;
	const struct qos_node *child;

	total = 0;
	if (!node_parent(parent))
		return 0;

	num = parent->parent_prop.num_of_children;
	first_child = parent->parent_prop.first_child_phy;

	if (num > 0) {
		child = get_const_node_from_phy(qdev->nodes, first_child);
		if (unlikely(!child)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    first_child);
			return 0;
		}
	}

	for (; num; --num) {
		total += child->child_prop.virt_bw_share;
		++child;
	}

	return total;
}

static s32 node_remove_wrapper(struct pp_qos_dev *qdev,
			       struct qos_node *node, void *data)
{
	u16 phy;
	u16 id;
	s32 rc;

	phy = get_phy_from_node(qdev->nodes, node);
	id = get_id_from_phy(qdev->mapping, phy);
	rc = pp_queue_enqueue(qdev->queue, id);
	QOS_ASSERT(rc == 0, "Could not enqueue\n");
	return rc;
}

static s32 node_flush_wrapper(struct pp_qos_dev *qdev,
			      struct qos_node *node, void *data)
{
	u16 phy;
	u16 id;
	s32 rc;

	phy = get_phy_from_node(qdev->nodes, node);
	id = get_id_from_phy(qdev->mapping, phy);
	rc = qos_queue_flush(qdev, id);
	QOS_ASSERT(rc == 0, "Could not enqueue\n");
	return rc;
}

static s32 node_modify_blocked_status(struct pp_qos_dev *qdev,
				      struct qos_node *node, void *data)
{
	u16 phy;
	u16 id;
	u32 status;
	s32 rc;

	status = (u32)(uintptr_t)data;
	phy = get_phy_from_node(qdev->nodes, node);
	id = get_id_from_phy(qdev->mapping, phy);
	if (status)
		rc = _pp_qos_queue_block(qdev, id);
	else
		rc = _pp_qos_queue_unblock(qdev, id);
	QOS_ASSERT(rc == 0, "Could not block queue\n");
	return rc;
}

static s32 node_used_wrapper(const struct pp_qos_dev *qdev,
			     const struct qos_node *node, void *data)
{
	return node_used(node);
}

s32 tree_remove(struct pp_qos_dev *qdev, u32 phy)
{
	struct qos_node *node;
	struct qos_node *parent = NULL;
	u32 id;
	u32 tmp;
	s32 rc;

	pp_queue_reset(qdev->queue);
	rc = post_order_travers_tree(qdev, phy, node_used_wrapper, NULL,
				     node_remove_wrapper, NULL);
	if (rc) {
		QOS_LOG("Error while trying to delete subtree with root %u\n",
			phy);
		return -EBUSY;
	}

	/* Save parent of the sub-tree */
	node = get_node_from_phy(qdev->nodes, phy);
	if (node_child(node)) {
		tmp = node->child_prop.parent_phy;
		parent = get_node_from_phy(qdev->nodes, tmp);
	}

	id = pp_queue_dequeue(qdev->queue);
	while (QOS_ID_VALID(id)) {
		tmp = get_phy_from_id(qdev->mapping, id);
		node = get_node_from_phy(qdev->nodes, tmp);
		if (unlikely(!node)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    tmp);
			return 0;
		}

		node_remove(qdev, node);
		id = pp_queue_dequeue(qdev->queue);
	}

	/* If parent is empty IS, remove it */
	if (parent &&
	    node_internal(parent) &&
	    !parent->parent_prop.num_of_children)
		return node_remove(qdev, parent);

	return 0;
}

s32 tree_flush(struct pp_qos_dev *qdev, u32 phy)
{
	s32 rc;

	rc = post_order_travers_tree(qdev, phy, node_queue_wrapper,
				     NULL, node_flush_wrapper, NULL);
	if (rc) {
		QOS_LOG("Unexpected error while flush subtree root is %u\n",
			phy);
		return -EBUSY;
	}
	return 0;
}

s32 tree_modify_blocked_status(struct pp_qos_dev *qdev,
			       u32 phy,
			       u32 status)
{
	s32 rc;

	rc = post_order_travers_tree(qdev,
				     phy, node_queue_wrapper,
				     NULL, node_modify_blocked_status,
				     (void *)(uintptr_t)status);
	if (rc) {
		QOS_LOG_ERR("Error when change blocked status to %u (root %u)",
			    status, phy);
		rc =  -EBUSY;
	}
	return rc;
}

/******************************************************************************/
/*                           Configuration                                    */
/******************************************************************************/

static s32 common_cfg_valid(const struct pp_qos_dev *qdev,
			    const struct qos_node *node)
{
	u32 shared;

	if (!((node->bandwidth_limit == QOS_NO_BANDWIDTH_LIMIT) ||
	      (node->bandwidth_limit <= qdev->hwconf.max_bw_limit_kbps))) {
		QOS_LOG_ERR("Invalid bandwidth limit %u. maximum is %u Kbps\n",
			    node->bandwidth_limit,
			    qdev->hwconf.max_bw_limit_kbps);
		return 0;
	}

	shared = node->shared_bw_group;
	if (!((shared == QOS_NO_SHARED_BANDWIDTH_GROUP) ||
	      (shared <= QOS_MAX_SHARED_BANDWIDTH_GROUP &&
	       qdev->groups[shared].used))) {
		QOS_LOG_ERR("Invalid shared bandwidth group %u\n",
			    node->shared_bw_group);
		return 0;
	}

	if (node->max_burst > QOS_MAX_MAX_BURST) {
		QOS_LOG_ERR("Invalid max burst %u\n", node->max_burst);
		return 0;
	}

	return 1;
}

/* If new node
 *     node.child.parent.phy == virtual parent phy
 * Else
 *     node.child.parent.phy is either virtual parent phy (if user is changing
 *     parent) or actual parent (if user is not changing parent)
 */
static s32 child_cfg_valid(const struct pp_qos_dev *qdev,
			   const struct qos_node *node,
			   const struct qos_node *orig_node,
			   u32 prev_virt_parent_phy)
{
	const struct qos_node *parent;
	struct qos_node *virt_parent_node;
	u32 parent_phy;
	u32 cur_virt_parent_phy;

	QOS_ASSERT(node_child(node), "node is not a child\n");

	if (node_syncq(node))
		return 1;

	if (node->child_prop.priority > QOS_MAX_CHILD_PRIORITY) {
		QOS_LOG_ERR("Illegal priority %u\n",
			    node->child_prop.priority);
		return 0;
	}

	parent_phy = node->child_prop.parent_phy;
	if (parent_phy > NUM_OF_NODES - 1) {
		QOS_LOG_ERR("Illegal parent %u\n", parent_phy);
		return 0;
	}

	parent = get_const_node_from_phy(qdev->nodes, parent_phy);
	if (!node_parent(parent)) {
		QOS_LOG_ERR("Node's parent %u is not a parent\n", parent_phy);
		return 0;
	}

	/* Find current virtual parent and bandwidth of its direct children */
	if (!node_internal(parent)) {
		cur_virt_parent_phy = parent_phy;
	} else {
		cur_virt_parent_phy = get_virtual_parent_phy(qdev->nodes,
							     parent);
	}

	virt_parent_node = get_node_from_phy(qdev->nodes, cur_virt_parent_phy);
	if (unlikely(!virt_parent_node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    cur_virt_parent_phy);
		return 0;
	}

	if (node->child_prop.virt_bw_share  > 0x7F) {
		QOS_LOG_ERR("bw_share (%u) can't exceed 0x7F\n",
			    node->child_prop.virt_bw_share);
		return 0;
	}

	return 1;
}

static s32 parent_cfg_valid(const struct pp_qos_dev *qdev,
			    const struct qos_node *node)
{
	u32 first;
	u32 num;

	QOS_ASSERT(node_parent(node), "node is not a parent\n");

	if (node->parent_prop.arbitration != PP_QOS_ARBITRATION_WSP &&
	    node->parent_prop.arbitration != PP_QOS_ARBITRATION_WRR &&
	    node->parent_prop.arbitration != PP_QOS_ARBITRATION_WFQ) {
		QOS_LOG_ERR("Unsupported arbitration\n");
		return 0;
	}

	if (node->parent_prop.num_of_children > 8) {
		QOS_LOG_ERR("node has %u children but max allowed is 8\n",
			    node->parent_prop.num_of_children);
		return 0;
	}

	first = node->parent_prop.first_child_phy;
	num = node->parent_prop.num_of_children;
	if (num  && ((first < qdev->init_params.max_ports) ||
		     (first > NUM_OF_NODES - 1))) {
		QOS_LOG_ERR("node has %u children but first (%u) is illegal\n",
			    node->parent_prop.num_of_children, first);
		return 0;
	}

	return 1;
}

u32 reserve_port(struct pp_qos_dev *qdev, u32 phy_port_id)
{
	if (phy_port_id >= PP_QOS_MAX_PORTS) {
		pr_err("Invalid port id %u\n", phy_port_id);
		return -EINVAL;
	}

	if (qdev->init_params.reserved_ports[phy_port_id]) {
		pr_err("Port id %u already reserved\n", phy_port_id);
		return -EINVAL;
	}

	qdev->init_params.reserved_ports[phy_port_id] = 1;

	return 0;
}

u32 unreserve_port(struct pp_qos_dev *qdev, u32 phy_port_id)
{
	if (phy_port_id >= PP_QOS_MAX_PORTS) {
		pr_err("Invalid port id %u\n", phy_port_id);
		return -EINVAL;
	}

	if (!(qdev->init_params.reserved_ports[phy_port_id])) {
		pr_err("Port id %u is not reserved\n", phy_port_id);
		return -EINVAL;
	}

	qdev->init_params.reserved_ports[phy_port_id] = 0;

	return 0;
}

u32 get_port(const struct pp_nodes *nodes, u32 phy)
{
	const struct qos_node *node;

	node = get_const_node_from_phy(nodes, phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    phy);
		return QOS_INVALID_PHY;
	}

	while (node_child(node)) {
		node = get_const_node_from_phy(nodes,
					       node->child_prop.parent_phy);
	}

	QOS_ASSERT(node_port(node), "Did not reach port node for phy %u\n",
		   phy);
	return get_phy_from_node(nodes, node);
}

s32 port_cfg_valid(const struct pp_qos_dev *qdev,
		   const struct qos_node *node,
		   const struct qos_node *orig_node)
{
	if (!node_port(node)) {
		QOS_LOG_ERR("Node is not a port\n");
		return 0;
	}

	if ((node->data.port.ring_address == 0) ||
	    (node->data.port.ring_size == 0)) {
		QOS_LOG_ERR("Invalid ring address 0x%lx or ring size %zu\n",
			    node->data.port.ring_address,
			    node->data.port.ring_size);
		return 0;
	}

	return node_cfg_valid(qdev, node, orig_node, QOS_INVALID_PHY);
}

s32 node_cfg_valid(const struct pp_qos_dev *qdev,
		   const struct qos_node *node,
		   const struct qos_node *orig_node,
		   u32 prev_virt_parent_phy)
{
	if (!common_cfg_valid(qdev, node)) {
		QOS_LOG_ERR("Common cfg not valid\n");
		return 0;
	}

	if (node_parent(node) && !parent_cfg_valid(qdev, node)) {
		QOS_LOG_ERR("Parent cfg not valid\n");
		return 0;
	}

	if (node_child(node) && !child_cfg_valid(qdev, node, orig_node,
						 prev_virt_parent_phy)) {
		QOS_LOG_ERR("Child cfg not valid\n");
		return 0;
	}

	return 1;
}

u32 get_virtual_parent_phy(const struct pp_nodes *nodes,
			   const struct qos_node *child)
{
	const struct qos_node *tmp;
	u32 phy;

	tmp = child;
	do {
		QOS_ASSERT(node_child(tmp), "Node is not a child\n");
		phy = tmp->child_prop.parent_phy;
		tmp = get_const_node_from_phy(nodes, phy);
		if (unlikely(!tmp)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    phy);
			return QOS_INVALID_PHY;
		}
	} while (node_internal(tmp));

	return phy;
}

s32 get_node_prop(const struct pp_qos_dev *qdev,
		  const struct qos_node *node,
		  struct pp_qos_common_node_properties *common,
		  struct pp_qos_parent_node_properties *parent,
		  struct pp_qos_child_node_properties *child)
{
	QOS_ASSERT(node_used(node), "Node is not used\n");

	if (common) {
		common->bandwidth_limit = node->bandwidth_limit;
		common->shared_bw_group = node->shared_bw_group;
		common->max_burst = node->max_burst;
	}

	if (parent) {
		parent->arbitration = node->parent_prop.arbitration;
		parent->best_effort_enable =
			!!QOS_BITS_IS_SET(node->flags,
					  NODE_FLAGS_PARENT_BEST_EFFORT_EN);
	}

	if (child) {
		/* Internal schedulers are transparent to clients. Clients see
		 * only virtual parent - the first parent in hierarchy which is
		 * not internal scheduler
		 */
		child->parent =
			get_id_from_phy(qdev->mapping,
					get_virtual_parent_phy(qdev->nodes,
							       node));
		child->priority = node->child_prop.priority;
		child->wrr_weight = node->child_prop.virt_bw_share;
	}

	return 0;
}

static s32 set_common(struct pp_qos_dev *qdev,
		      struct qos_node *node,
		      const struct pp_qos_common_node_properties *common,
		      uint32_t *modified)
{
	u32 grp_id;
	u32 burst;

	if (node->bandwidth_limit != common->bandwidth_limit) {
		node->bandwidth_limit = common->bandwidth_limit;
		QOS_BITS_SET(*modified, QOS_MODIFIED_BANDWIDTH_LIMIT);
	}

	grp_id = common->shared_bw_group;
	if (grp_id != node->shared_bw_group) {
		node->shared_bw_group = grp_id;
		QOS_BITS_SET(*modified, QOS_MODIFIED_SHARED_GROUP_ID);
	}

	burst = common->max_burst;
	if (burst != node->max_burst) {
		node->max_burst = burst;
		QOS_BITS_SET(*modified, QOS_MODIFIED_MAX_BURST);
	}

	return 0;
}

static s32 set_parent(struct pp_qos_dev *qdev,
		      struct qos_node *node,
		      const struct pp_qos_parent_node_properties *parent,
		      uint32_t *modified)
{
	if (node->parent_prop.arbitration != parent->arbitration) {
		if (node_sched(node) ||
		    QOS_BITS_IS_SET(*modified, QOS_MODIFIED_NODE_TYPE)) {
			node->parent_prop.arbitration = parent->arbitration;
			QOS_BITS_SET(*modified, QOS_MODIFIED_ARBITRATION);
		} else {
			QOS_LOG_ERR("Changing port arb is not supported\n");
			return 1;
		}
	}

	if (!!(parent->best_effort_enable) !=
	    !!QOS_BITS_IS_SET(node->flags, NODE_FLAGS_PARENT_BEST_EFFORT_EN)) {
		QOS_BITS_TOGGLE(node->flags, NODE_FLAGS_PARENT_BEST_EFFORT_EN);
		QOS_BITS_SET(*modified, QOS_MODIFIED_BEST_EFFORT);
	}
	return 0;
}

static s32 set_child(struct pp_qos_dev *qdev,
		     struct qos_node *node,
		     const struct pp_qos_child_node_properties *child,
		     uint32_t *modified)
{
	u32 conf_parent_phy;
	u32 virt_parent_phy;
	struct qos_node *parent;
	enum pp_qos_arbitration parent_arb;

	/* Do not set node properties for a disconnected queue */
	if (node_new_syncq(node)) {
		node->child_prop.parent_phy = child->parent;
		return 0;
	}

	/* Equals to virtual parent phy since client is not aware of internal
	 * schedulers, they are transparent to him
	 */
	conf_parent_phy = get_phy_from_id(qdev->mapping, child->parent);
	virt_parent_phy = node->child_prop.parent_phy;
	parent = get_node_from_phy(qdev->nodes, conf_parent_phy);
	parent_arb = parent->parent_prop.arbitration;

	if (QOS_PHY_VALID(virt_parent_phy))
		virt_parent_phy = get_virtual_parent_phy(qdev->nodes, node);

	if (virt_parent_phy != conf_parent_phy) {
		node->child_prop.parent_phy = conf_parent_phy;
		QOS_BITS_SET(*modified, QOS_MODIFIED_PARENT);

		/* Set unlimited weight if new parent is WSP */
		if (parent_arb == PP_QOS_ARBITRATION_WSP) {
			node->child_prop.bw_weight =
				QOS_CHILD_UNLIMITED_WEIGHT;
			QOS_BITS_SET(*modified, QOS_MODIFIED_BW_WEIGHT);
		}

		/* Set default weight if new parent is WRR and old is WSP */
		if ((parent_arb == PP_QOS_ARBITRATION_WRR) &&
		    (node->child_prop.bw_weight ==
		     QOS_CHILD_UNLIMITED_WEIGHT)) {
			node->child_prop.bw_weight =
				node->child_prop.virt_bw_share;
			QOS_BITS_SET(*modified, QOS_MODIFIED_BW_WEIGHT);
		}
	}

	if (node->child_prop.virt_bw_share != child->wrr_weight) {
		if (child->wrr_weight > QOS_MAX_CHILD_WRR_WEIGHT) {
			QOS_LOG_ERR("Wrr weight %u out of range (%u - %u)\n",
				    child->wrr_weight,
				    QOS_MIN_CHILD_WRR_WEIGHT,
				    QOS_MAX_CHILD_WRR_WEIGHT);
			return -EINVAL;
		}

		/* Modify weight only on WRR */
		if (parent_arb == PP_QOS_ARBITRATION_WRR) {
			/* If weight was set to 0, set it to 1 */
			node->child_prop.virt_bw_share =
				child->wrr_weight ? child->wrr_weight : 1;

			node->child_prop.bw_weight =
				node->child_prop.virt_bw_share;
			QOS_BITS_SET(*modified, QOS_MODIFIED_BW_WEIGHT);
		}
	}

	parent = get_node_from_phy(qdev->nodes, node->child_prop.parent_phy);
	if (unlikely(!parent)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    node->child_prop.parent_phy);
		return 0;
	}

	if (node->child_prop.priority != child->priority) {
		if (QOS_PHY_VALID(node->child_prop.parent_phy)) {
			parent = get_node_from_phy(qdev->nodes,
						   node->child_prop.parent_phy);
			if (parent->parent_prop.arbitration ==
			    PP_QOS_ARBITRATION_WSP) {
				node->child_prop.priority = child->priority;
				QOS_BITS_SET(*modified, QOS_MODIFIED_PRIORITY);
			}
		} else {
			pr_err("Trying to set priority, INVALID parent phy\n");
		}
	}

	return 0;
}

s32 set_node_prop(struct pp_qos_dev *qdev,
		  struct qos_node *node,
		  const struct pp_qos_common_node_properties *common,
		  const struct pp_qos_parent_node_properties *parent,
		  const struct pp_qos_child_node_properties *child,
		  uint32_t *modified)
{
	s32 rc;

	if (common) {
		rc = set_common(qdev, node, common, modified);
		if (rc)
			return rc;
	}
	if (parent) {
		rc = set_parent(qdev, node, parent, modified);
		if (rc)
			return rc;
	}
	if (child) {
		rc = set_child(qdev, node, child, modified);
		if (rc)
			return rc;
	}

	return 0;
}

static void node_parent_init(struct qos_node *node)
{
	node->parent_prop.first_child_phy = QOS_INVALID_PHY;
	node->parent_prop.num_of_children = 0;
	node->parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
}

static void node_child_init(struct qos_node *node)
{
	node->child_prop.parent_phy = QOS_INVALID_PHY;
	node->child_prop.virt_bw_share = 1;
	node->child_prop.priority = 0;
}

static void node_common_init(struct qos_node *node)
{
	node->flags = 0;
	node->bandwidth_limit = QOS_NO_BANDWIDTH_LIMIT;
	node->shared_bw_group = QOS_NO_SHARED_BANDWIDTH_GROUP;
	node->max_burst = QOS_DEAFULT_MAX_BURST;
}

void node_init(const struct pp_qos_dev *qdev, struct qos_node *node,
	       u32 common, u32 parent, u32 child)
{
	memset(node, 0, sizeof(struct qos_node));

	node->type = TYPE_UNKNOWN;
	if (common)
		node_common_init(node);

	if (parent)
		node_parent_init(node);

	if (child)
		node_child_init(node);
}

/******************************************************************************/
/*                              ALLOC PHY                                     */
/******************************************************************************/

/**
 * Return phy where a child whose priority is child_priority should be placed.
 * If parent is WRR than priority is ignored and the new child will
 * be given a phy after the current last child
 *
 * Assumption: parent's children are held on a non full octet
 */
static u32 calculate_new_child_location(struct pp_qos_dev *qdev,
					struct qos_node *parent,
					u32 priority)
{
	u32 phy;
	u32 num;
	struct qos_node *child;

	QOS_ASSERT(node_parent(parent), "node is not a parent\n");
	if (parent->parent_prop.arbitration == PP_QOS_ARBITRATION_WRR) {
		phy = parent->parent_prop.first_child_phy +
			parent->parent_prop.num_of_children;
	} else {
		num = parent->parent_prop.num_of_children;
		phy = parent->parent_prop.first_child_phy;
		child = get_node_from_phy(qdev->nodes, phy);
		if (unlikely(!child)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    phy);
			return QOS_INVALID_PHY;
		}

		while ((priority > child->child_prop.priority) && num) {
			++phy;
			++child;
			--num;
		}
	}

	return phy;
}

/**
 * children_on_non_full_octet() - Allocate a phy for a new child whose siblings
 * are held on a non full octet.
 * @qdev:
 * @parent:
 * @usage:	    Number of used nodes on children's octet
 * @child_priority: New child priority (relevant only for WSP parent)
 * Return:	    New allocated phy
 */
static u32 children_on_non_full_octet(struct pp_qos_dev *qdev,
				      struct qos_node *parent,
				      u32 usage, u32 child_priority)
{
	u32 phy;

	phy = calculate_new_child_location(qdev, parent, child_priority);

	if (node_sched(parent)) {
		parent->data.sched.new_child_phy = phy;
		parent->data.sched.new_child_skipped = 0;
	}

	octet_nodes_shift(qdev, phy, usage - octet_phy_offset(phy), 1);
	return phy;
}

static u32
children_on_non_full_octet_wrapper(struct pp_qos_dev *qdev,
				   struct qos_node *parent,
				   u32 child_priority)
{
	u32 usage;
	u32 octet;

	QOS_ASSERT(node_parent(parent),
		   "Node %u is not a parent\n",
		   get_phy_from_node(qdev->nodes, parent));
	octet = octet_of_phy(parent->parent_prop.first_child_phy);
	usage = octet_get_use_count(qdev->octets, octet);
	QOS_ASSERT(usage < 8, "Octet is full\n");
	return children_on_non_full_octet(qdev, parent, usage, child_priority);
}

/**
 * less_than_8_children_full_octet() - Allocate a phy for a new child
 * are held on a full octet.
 * @qdev:
 * @parent:
 * @children_octet:
 * @child_priority:
 * Return: New allocated phy
 */
static u32 less_than_8_children_full_octet(struct pp_qos_dev *qdev,
					   struct qos_node *parent,
					   u8 children_octet,
					   u32 child_priority)
{
	struct qos_node *min_parent;
	u32 octet;
	u32 phy;
	u32 num_of_required_entries;
	u32 num_of_nodes_to_move;
	u32 dst_phy;
	u32 src_phy;
	u32 parent_id;

	parent_id = get_id_from_phy(qdev->mapping,
				    get_phy_from_node(qdev->nodes, parent));
	QOS_ASSERT(octet_get_use_count(qdev->octets, children_octet) == 8,
		   "Octet %d is not full\n", children_octet);

	min_parent = octet_get_min_sibling_group(qdev, children_octet,
						 parent,
						 &num_of_required_entries);
	QOS_ASSERT(min_parent, "Can't find min_parent for octet %d\n",
		   children_octet);

	if (!min_parent)
		return QOS_INVALID_PHY;

	octet = octet_get_with_at_least_free_entries(qdev->octets,
						     num_of_required_entries);
	if (!QOS_OCTET_VALID(octet)) {
		QOS_LOG("could not find free octet\n");
		return QOS_INVALID_PHY;
	}

	if (parent == min_parent)
		num_of_nodes_to_move = num_of_required_entries - 1;
	else
		num_of_nodes_to_move = num_of_required_entries;

	/* move children of min_parent to new octet */
	dst_phy = 8 * octet + octet_get_use_count(qdev->octets, octet);
	src_phy =  min_parent->parent_prop.first_child_phy;
	nodes_move(qdev, dst_phy, src_phy, num_of_nodes_to_move);

	/* shift original octet if necessary i.e. if last moved
	 * node was not last node on octet
	 */
	src_phy += num_of_nodes_to_move;
	if (octet_phy_offset(src_phy - 1) != 7) {
		octet_nodes_shift(qdev, src_phy, 8 - octet_phy_offset(src_phy),
				  -num_of_nodes_to_move);
	}

	parent = get_node_from_phy(qdev->nodes,
				   get_phy_from_id(qdev->mapping, parent_id));
	if (unlikely(!parent)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    get_phy_from_id(qdev->mapping, parent_id));
		return QOS_INVALID_PHY;
	}

	phy = children_on_non_full_octet_wrapper(qdev, parent, child_priority);
	return phy;
}

/**
 * phy_alloc_parent_less_than_8_children() - Allocate a phy
 * for a new child whose parent has less than 8 children
 * @qdev:
 * @parent:
 * @child_priority:
 * Return:
 */
static u32 phy_alloc_1_to_7_child(struct pp_qos_dev *qdev,
				  struct qos_node *parent,
				  u32 child_priority)
{
	u32 phy, octet, usage, parent_id;

	QOS_ASSERT(node_parent(parent) &&
		   (parent->parent_prop.num_of_children < 8),
		   "Node %u is not a parent with less than 8 children\n",
		   get_phy_from_node(qdev->nodes, parent));

	parent_id = get_id_from_phy(qdev->mapping,
				    get_phy_from_node(qdev->nodes, parent));

	if (parent->parent_prop.num_of_children == 0) {
		octet = octet_get_with_at_least_free_entries(qdev->octets, 8);
		if (!QOS_OCTET_VALID(octet)) {
			octet = octet_get_with_at_least_free_entries(
				qdev->octets, 1);
			if (!QOS_OCTET_VALID(octet)) {
				QOS_LOG("could not find free octet\n");
				return QOS_INVALID_PHY;
			}
		}
		phy = octet * 8 + octet_get_use_count(qdev->octets, octet);
	} else {
		octet = octet_of_phy(parent->parent_prop.first_child_phy);
		usage = octet_get_use_count(qdev->octets, octet);
		if (usage < 8) {
			phy = children_on_non_full_octet(qdev,
							 parent,
							 usage,
							 child_priority);
		} else {
			phy = less_than_8_children_full_octet(qdev,
							      parent,
							      octet,
							      child_priority);
		}
	}
	parent = get_node_from_phy(qdev->nodes,
				   get_phy_from_id(qdev->mapping, parent_id));

	if (unlikely(!QOS_PHY_VALID(phy)))
		goto done;

	link_with_parent(qdev, phy, parent_id);
	nodes_modify_used_status(qdev, phy, 1, 1);
	if (node_sched(parent) &&
	    QOS_PHY_VALID(parent->data.sched.new_child_phy)) {
		QOS_LOG_DEBUG("Sched %u(%u). phy %u. skipped %u\n",
			      parent_id, get_phy_from_node(qdev->nodes, parent),
			      phy, parent->data.sched.new_child_skipped);

		/* We must update preds of the skipped child */
		if (parent->data.sched.new_child_skipped)
			tree_update_predecessors(qdev, phy);

		/* Remove new child listing from sched db */
		parent->data.sched.new_child_phy = QOS_UNMAPPED_PHY;
		parent->data.sched.new_child_skipped = 0;
	}
done:
	return phy;
}

/**
 * create_internal_scheduler_on_node() - Create internal scheduler
 * The original node is moved to another octet (which has at least 2 free slots
 * and can accommodate the original node and a new child) and becomes a child of
 * the created internal scheduler
 * @qdev:
 * @node: The node where the internal scheduler should be placed
 * Return:
 */
static u32 create_internal_scheduler_on_node(struct pp_qos_dev *qdev,
					     struct qos_node *node)
{
	struct qos_node *new_node;
	u32 octet;
	u32 new_phy;
	u32 phy;
	u32 id;
	u32 modified;
	struct pp_qos_sched_conf conf;

	octet = octet_get_with_at_least_free_entries(qdev->octets, 8);
	if (!QOS_OCTET_VALID(octet)) {
		octet = octet_get_with_at_least_free_entries(qdev->octets, 2);
		if (!QOS_OCTET_VALID(octet)) {
			QOS_LOG("could not find free octet\n");
			return  QOS_INVALID_PHY;
		}
	}

	/* The assertion on nodes_move especially the one that states
	 * that if a child is moved to another octet than its siblings must
	 * move also, prevents using move in this clone scenario
	 */

	/* find a place for a new node */
	new_phy = octet * 8 + octet_get_use_count(qdev->octets, octet);
	QOS_ASSERT(QOS_PHY_VALID(new_phy + 1),
		   "%u is not a valid phy\n", new_phy + 1);
	new_node = get_node_from_phy(qdev->nodes, new_phy);
	if (unlikely(!new_node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n", new_phy);
		return QOS_INVALID_PHY;
	}

	QOS_ASSERT(!node_used(new_node), "Node is used\n");
	nodes_modify_used_status(qdev, new_phy, 1, 1);

	/* update children to point to new node */
	phy = get_phy_from_node(qdev->nodes, node);
	id = get_id_from_phy(qdev->mapping, phy);
	map_id_phy(qdev->mapping, id, new_phy);
	if (node_parent(node))
		node_update_children(qdev, node, new_phy);

	memcpy(new_node, node, sizeof(struct qos_node));
	new_node->child_prop.parent_phy = phy;
	create_move_cmd(qdev, new_phy, phy, get_port(qdev->nodes, new_phy));

	/* virtual bw share and parent_phy remain */
	node->flags = 0;
	QOS_BITS_SET(node->flags, NODE_FLAGS_USED | NODE_FLAGS_INTERNAL);
	node->type = TYPE_SCHED;
	node->parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
	node->parent_prop.num_of_children = 1;
	node->parent_prop.first_child_phy = new_phy;
	id = pp_pool_get(qdev->ids);
	QOS_ASSERT(QOS_ID_VALID(id), "Got invalid id\n");
	map_id_phy(qdev->mapping, id, phy);

	get_node_prop(qdev, node, &conf.common_prop,
		      &conf.sched_parent_prop,
		      &conf.sched_child_prop);
	modified = 0;
	QOS_BITS_SET(modified,
		     QOS_MODIFIED_NODE_TYPE |
		     QOS_MODIFIED_BANDWIDTH_LIMIT |
		     QOS_MODIFIED_SHARED_GROUP_ID |
		     QOS_MODIFIED_BW_WEIGHT |
		     QOS_MODIFIED_PARENT |
		     QOS_MODIFIED_ARBITRATION |
		     QOS_MODIFIED_BEST_EFFORT);

	create_set_sched_cmd(qdev, &conf, node, modified);

	/* The new node will be put right after the moved node */
	++new_phy;
	link_with_parent(qdev, new_phy, id);
	nodes_modify_used_status(qdev, new_phy, 1, 1);
	tree_update_predecessors(qdev, phy);

	return new_phy;
}

s32 connect_sync_queue(struct pp_qos_dev *qdev, u32 syncq_phy, u32 dstq_phy)
{
	struct qos_node *new_dstq_node, *new_sched_node;
	struct pp_qos_sched_conf conf;
	u32 sched_id, sched_phy, new_dstq_phy, modified;

	QOS_LOG_DEBUG("connect_sync_queue: syncq_phy %u, dstq_phy %u\n",
		      syncq_phy, dstq_phy);
	/* the original dst queue phy will be
	 * replaced by the new scheduler phy
	 */
	sched_phy = dstq_phy;
	/* new dst queue phy will follow the syncq phy */
	new_dstq_phy = syncq_phy + 1;
	/* get the scheduler node (old dst queue node)*/
	new_sched_node = get_node_from_phy(qdev->nodes, sched_phy);
	if (unlikely(!new_sched_node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n", sched_phy);
		return QOS_INVALID_PHY;
	}
	/* get the new dst queue node */
	new_dstq_node = get_node_from_phy(qdev->nodes, new_dstq_phy);
	if (unlikely(!new_dstq_node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    new_dstq_phy);
		return QOS_INVALID_PHY;
	}
	/* copy the dst queue node to the prev configuration */
	memcpy(new_dstq_node, new_sched_node, sizeof(struct qos_node));
	/* update the dstq parent to be the sched node */
	new_dstq_node->child_prop.parent_phy = sched_phy;
	new_dstq_node->child_prop.priority = 1; // SP
	/* remap the dst queue for the new phy */
	map_id_phy(qdev->mapping, get_id_from_phy(qdev->mapping, dstq_phy),
		   new_dstq_phy);

	create_move_cmd(qdev, new_dstq_phy, dstq_phy,
			get_port(qdev->nodes, dstq_phy));

	/* configure the new scheduler node */
	new_sched_node->flags = 0;
	QOS_BITS_SET(new_sched_node->flags,
		     NODE_FLAGS_USED | NODE_FLAGS_SCHED_SYNCQ);
	new_sched_node->type = TYPE_SCHED;
	new_sched_node->parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
	new_sched_node->parent_prop.num_of_children = 2;
	new_sched_node->parent_prop.first_child_phy = syncq_phy;
	sched_id = pp_pool_get(qdev->ids);
	QOS_ASSERT(QOS_ID_VALID(sched_id), "Got invalid id\n");

	modified = 0;
	QOS_BITS_SET(modified,
		     QOS_MODIFIED_NODE_TYPE |
		     QOS_MODIFIED_BANDWIDTH_LIMIT |
		     QOS_MODIFIED_SHARED_GROUP_ID |
		     QOS_MODIFIED_BW_WEIGHT |
		     QOS_MODIFIED_PARENT |
		     QOS_MODIFIED_ARBITRATION |
		     QOS_MODIFIED_BEST_EFFORT);

	map_id_phy(qdev->mapping, sched_id, sched_phy);
	get_node_prop(qdev, new_sched_node, &conf.common_prop,
		      &conf.sched_parent_prop,
		      &conf.sched_child_prop);
	create_set_sched_cmd(qdev, &conf, new_sched_node, modified);

	/* The new node will be put right after the moved node */
	link_with_parent(qdev, new_dstq_phy, sched_id);
	link_with_parent(qdev, syncq_phy, sched_id);
	tree_update_predecessors(qdev, sched_phy);
	add_suspend_port(qdev, get_port(qdev->nodes, sched_phy));
	suspend_node(qdev, syncq_phy, 0);
	set_children_wsp_weight(qdev, new_sched_node);
	return 0;
}

s32 disconnect_sync_queue(struct pp_qos_dev *qdev, u32 phy)
{
	struct qos_node *dst_node, *sched_node, *parent;
	u32 sched_id, sched_phy, dst_id, dst_phy = phy + 1;

	QOS_LOG_DEBUG("disconnect_sync_queue: phy %u\n", phy);
	dst_node = get_node_from_phy(qdev->nodes, dst_phy);
	if (unlikely(!dst_node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n", dst_phy);
		return QOS_INVALID_PHY;
	}

	sched_phy = dst_node->child_prop.parent_phy;
	sched_id  = get_id_from_phy(qdev->mapping, sched_phy);
	dst_id    = get_id_from_phy(qdev->mapping, dst_phy);

	/* get the scheduler node */
	sched_node = get_node_from_phy(qdev->nodes, sched_phy);
	if (unlikely(!sched_node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n", sched_phy);
		return QOS_INVALID_PHY;
	}

	/* copy the child part from the scheduler to the queue */
	memcpy(&dst_node->child_prop, &sched_node->child_prop,
	       sizeof(struct child_node_properties));
	/* copy the dst queue node to the prev scheduler node */
	memcpy(sched_node, dst_node, sizeof(struct qos_node));
	/* remap the dst queue for the new phy */
	map_id_phy(qdev->mapping, dst_id, sched_phy);
	create_move_cmd(qdev, sched_phy, dst_phy,
			get_port(qdev->nodes, sched_phy));

	if (node_parent(sched_node)) {
		node_update_children(qdev, sched_node, sched_phy);
		update_parent(qdev, sched_phy);
	}

	tree_update_predecessors(qdev, sched_phy);
	map_invalidate_phy(qdev->mapping, dst_phy);
	memset(dst_node, 0, sizeof(struct qos_node));
	map_invalidate_id(qdev->mapping, sched_id);
	pp_pool_put(qdev->ids, sched_id);
	add_suspend_port(qdev, get_port(qdev->nodes, sched_phy));
	parent = get_node_from_phy(qdev->nodes,
				   sched_node->child_prop.parent_phy);
	update_internal_bandwidth(qdev, parent);

	/* Sync queue has no parent now, suspend the queue */
	suspend_node(qdev, phy, 1);

	return 0;
}

u32 get_phy_from_syncq(struct pp_qos_dev *qdev, u32 id)
{
	u32 i;

	for (i = 0; i < qdev->syncq->capacity; i++) {
		if (id == qdev->syncq->id[i]) {
			QOS_LOG_DEBUG("get_phy_from_syncq found phy %u\n",
				      qdev->syncq->phy[i]);
			return qdev->syncq->phy[i];
		}
	}

	QOS_LOG_DEBUG("get_phy_from_syncq failed\n");
	return QOS_INVALID_PHY;
}

u32 alloc_phy_from_syncq(struct pp_qos_dev *qdev, u32 id)
{
	u32 capacity = qdev->syncq->capacity;
	u32 octet, phy = QOS_INVALID_PHY;

	if (unlikely(QOS_PHY_VALID(get_phy_from_id(qdev->mapping, id)))) {
		QOS_LOG_ERR("phy already exist for syncq %u\n", id);
		return QOS_INVALID_PHY;
	}

	if (capacity >= qdev->init_params.num_syncqs) {
		QOS_LOG_DEBUG("can't allocate phy syncq for queue %u\n", id);
		return phy;
	}

	/* each octet contain 4 syncqs (and 4 empty nodes for dstqs)*/
	if (capacity % 4) {
		phy = qdev->syncq->phy[capacity - 1] + 2;
	} else {
		/* Allocate octets */
		octet = octet_get_with_at_least_free_entries(qdev->octets, 8);
		if (!QOS_OCTET_VALID(octet)) {
			QOS_LOG_ERR("could not find free octet\n");
			return phy;
		}
		phy = octet << 3;
		update_octet_usage(qdev->octets, phy, 8, 1);
	}

	map_id_phy(qdev->mapping, id, phy);
	qdev->syncq->phy[capacity] = phy;
	qdev->syncq->id[capacity]  = id;
	qdev->syncq->capacity++;
	QOS_LOG_DEBUG("alloc_phy_from_syncq, allocated phy=%u\n", phy);

	return phy;
}

void delete_all_syncq(struct pp_qos_dev *qdev)
{
	struct qos_node *node;
	u32 i, phy;

	QOS_LOG_DEBUG("delete syncqs, capacity = %u\n", qdev->syncq->capacity);
	while (qdev->syncq->capacity) {
		i    = --qdev->syncq->capacity;
		phy  = qdev->syncq->phy[i];
		node = get_node_from_phy(qdev->nodes, phy);
		if (unlikely(node_remove(qdev, node))) {
			QOS_LOG_ERR("could not remove node (%u)\n", phy);
			return;
		}
		memset(node, 0, sizeof(*node));
		if ((phy % 8) == 0)
			update_octet_usage(qdev->octets, phy, 8, 0);
	}
	QOS_LOG_DEBUG("delete_all_syncq done\n");
}

/**
 * phy_alloc_for_new_child_parent_has_8_children() - Allocate a phy for a new
 * child whose parent has 8 children
 * @qdev:
 * @parent: a parent with 8 children
 * Return: new child phy
 */
static u32 phy_alloc_8_child(struct pp_qos_dev *qdev, struct qos_node *parent)
{
	struct qos_node *child;
	struct qos_node *last_child;
	u32 i, first_phy, parent_phy, phy = QOS_INVALID_PHY;

	QOS_ASSERT(node_parent(parent) &&
		   (parent->parent_prop.num_of_children == 8) &&
		   (parent->parent_prop.arbitration == PP_QOS_ARBITRATION_WRR),
		   "Node %u is not a WRR parent with 8 children\n",
		   get_phy_from_node(qdev->nodes, parent));

	first_phy = parent->parent_prop.first_child_phy;
	child = get_node_from_phy(qdev->nodes, first_phy);
	last_child = child + 7;
	for (; child <= last_child; ++child) {
		if (node_internal(child) &&
		    (child->parent_prop.num_of_children < 8)) {
			return phy_alloc_1_to_7_child(qdev,
						      child,
						      QOS_INVALID_PRIORITY);
		}
	}

	child = get_node_from_phy(qdev->nodes, first_phy);
	if (unlikely(!child)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    first_phy);
		return QOS_INVALID_PHY;
	}

	while (child < last_child && node_internal(child))
		++child;
	if (!node_internal(child))
		return create_internal_scheduler_on_node(qdev, child);

	/* If we reach this point all children are full internal schedulers
	 * We will do a breadth first traversal on the tree i.e. look for a
	 * place for the child under the internal schedulers children of the
	 * parent. For this we will need a queue
	 */
	for (i = 0; i < 8; ++i) {
		if (pp_queue_enqueue(qdev->queue, first_phy + i)) {
			QOS_LOG("Queue is full\n");
			return QOS_INVALID_PHY;
		}
	}

	parent_phy = pp_queue_dequeue(qdev->queue);
	while (QOS_PHY_VALID(parent_phy)) {
		phy = phy_alloc_8_child(qdev, get_node_from_phy(qdev->nodes,
								parent_phy));
		if (QOS_PHY_VALID(phy))
			return phy;
	}

	return phy;
}

u32 phy_alloc_by_parent(struct pp_qos_dev *qdev, struct qos_node *parent,
			u32 child_priority)
{
	u32 phy;

	if (parent->parent_prop.num_of_children < 8) {
		phy =  phy_alloc_1_to_7_child(qdev, parent, child_priority);
	} else if (parent->parent_prop.arbitration == PP_QOS_ARBITRATION_WSP) {
		QOS_LOG_ERR("WSP parent %u already has 8 children\n",
			    get_phy_from_node(qdev->nodes, parent));
		phy = QOS_INVALID_PHY;
	} else {
		pp_queue_reset(qdev->queue);
		phy = phy_alloc_8_child(qdev, parent);
	}

	return phy;
}

s32 qos_get_node_info(struct pp_qos_dev *qdev, u32 id,
		      struct pp_qos_node_info *info)
{
	struct qos_node *node;
	s32 rc = 0;

	node = get_conform_node(qdev, id, node_used);
	if (!node) {
		rc = -EINVAL;
		goto out;
	}

	create_get_node_info_cmd(qdev, get_phy_from_node(qdev->nodes, node),
				 qdev->hwconf.fw_stats_ddr_phys, info);

	update_cmd_id(&qdev->drvcmds);
	rc = transmit_cmds(qdev);
out:
	return rc;
}

#ifndef PP_QOS_DISABLE_CMDQ
s32 check_sync_with_fw(struct pp_qos_dev *qdev, bool dump_entries)
{
	const struct qos_node *node;
	struct system_stats_s sys_stats = {0};
	struct pp_pool *pool;
	struct pp_qos_node_info info;
	u32 i, used, id;
	s32 ret = 0;

	pool = pp_pool_init(NUM_OF_NODES, PP_QOS_INVALID_ID);
	if (!pool) {
		QOS_LOG_ERR("Can't create pool for firmware sync check\n");
		return -ENOMEM;
	}

	used = 0;
	node = get_const_node_from_phy(qdev->nodes, 0);
	for (i = 0; i < NUM_OF_NODES; ++i) {
		if (node_used(node)) {
			id = get_id_from_phy(qdev->mapping, i);
			QOS_ASSERT(QOS_ID_VALID(id),
				   "Invalid id for phy %u\n", i);
			pp_pool_put(pool, id);
			++used;
		}
		++node;
	}

	create_get_sys_info_cmd(qdev, qdev->hwconf.fw_stats_ddr_phys,
				&sys_stats, dump_entries);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);

	if (sys_stats.tscd_num_of_used_nodes != used) {
#ifdef DUMP_LOG_ON_FAILURE
		qos_dbg_tree_show(qdev, false, NULL);
		print_fw_log(qdev);
#endif
		QOS_LOG_ERR("Node Count Assertion: DB: %u, FW: %u\n",
			    used, sys_stats.tscd_num_of_used_nodes);
		ret = -EINVAL;
		goto done;
	}

	id = pp_pool_get(pool);
	while (QOS_ID_VALID(id)) {
		if (qos_get_node_info(qdev, id, &info)) {
			QOS_LOG_ERR("Error info on %u\n", id);
			ret = -EINVAL;
			break;
		}
		id = pp_pool_get(pool);
	}

done:
	pp_pool_clean(pool);
	return ret;
}
#endif

/* Return 1 if device is not in assert and was initialized */
s32 qos_device_ready(const struct pp_qos_dev *qdev)
{
	if (PP_QOS_DEVICE_IS_ASSERT(qdev)) {
		QOS_LOG_CRIT("!!!!! Qos driver in unstable mode !!!!!\n");
		return 0;
	}

	if (!qdev->initialized) {
		QOS_LOG_ERR("Device was not initialized\n");
		return 0;
	}

	return 1;
}

/* Return new phy of a node whose original node was
 * orig_node and now has a new priority
 */
static u32 find_child_position_by_priority(struct pp_qos_dev *qdev,
					   struct qos_node *parent,
					   struct qos_node *orig_node,
					   u32 priority)
{
	struct qos_node *first;
	struct qos_node *child;
	struct qos_node *last;
	u32 pos;
	u32 num;

	QOS_ASSERT(parent->parent_prop.arbitration == PP_QOS_ARBITRATION_WSP,
		   "Parent is not wsp\n");

	num = parent->parent_prop.num_of_children;
	QOS_ASSERT(num, "Function can't be used on parent with no children\n");

	pos = parent->parent_prop.first_child_phy;

	first = get_node_from_phy(qdev->nodes, pos);
	if (unlikely(!first)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n", pos);
		return pos;
	}

	last = get_node_from_phy(qdev->nodes, pos + num - 1);
	if (unlikely(!last)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
			    pos + num - 1);
		return pos;
	}

	for (child = first; child <= last; ++child) {
		if (child == orig_node)
			continue;
		if (priority <= child->child_prop.priority)
			break;
		++pos;
	}
	return pos;
}

/* Update wsp child position based on its position
 * child - original child node before moving (stored at db)
 * parent - parent node of child
 * node_src - copy of child node (stored at stack)
 */
void update_children_position(struct pp_qos_dev *qdev,
			      struct qos_node *child,
			      struct qos_node *parent,
			      struct qos_node *node_src)
{
	u32 old_phy;
	u32 new_phy;
	u32 id;
	u32 dst_port;
	struct qos_node *node;
	u32 first;
	u32 cnt;

	old_phy = get_phy_from_node(qdev->nodes, child);

	new_phy =
		find_child_position_by_priority(qdev,
						parent,
						child,
						node_src->child_prop.priority);

	if (new_phy == old_phy) {
		node = get_node_from_phy(qdev->nodes, new_phy);
		if (unlikely(!node)) {
			QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n",
				    new_phy);
			return;
		}

		memcpy(node, node_src, sizeof(struct qos_node));
		return;
	}

	first = parent->parent_prop.first_child_phy;
	cnt = parent->parent_prop.num_of_children;
	dst_port = get_port(qdev->nodes, old_phy);
	nodes_modify_used_status(qdev, old_phy, 1, 0);

	/* Store old node info in tmp node */
	memcpy(get_node_from_phy(qdev->nodes, PP_QOS_TMP_NODE),
	       get_node_from_phy(qdev->nodes, old_phy),
	       sizeof(struct qos_node));

	create_move_cmd(qdev, PP_QOS_TMP_NODE, old_phy, dst_port);

	id = get_id_from_phy(qdev->mapping, old_phy);
	if (new_phy > old_phy)
		octet_nodes_shift(qdev, old_phy + 1, new_phy - old_phy, -1);
	else if (new_phy < old_phy)
		octet_nodes_shift(qdev, new_phy, old_phy - new_phy, 1);

	create_move_cmd(qdev, new_phy, PP_QOS_TMP_NODE, dst_port);

	/* Reset tmp node */
	memset(get_node_from_phy(qdev->nodes, PP_QOS_TMP_NODE),
	       0, sizeof(struct qos_node));

	map_id_phy(qdev->mapping, id, new_phy);

	if (node_parent(node_src))
		node_update_children(qdev, node_src, new_phy);

	node = get_node_from_phy(qdev->nodes, new_phy);
	if (unlikely(!node)) {
		QOS_LOG_ERR("get_node_from_phy(%u) returned NULL\n", new_phy);
		return;
	}

	memcpy(node, node_src, sizeof(struct qos_node));
	QOS_BITS_CLEAR(node->flags, NODE_FLAGS_USED);
	nodes_modify_used_status(qdev, new_phy, 1, 1);
	parent->parent_prop.first_child_phy = first;
	parent->parent_prop.num_of_children = cnt;

	update_parent(qdev, get_phy_from_node(qdev->nodes, parent));

	if (node_parent(node))
		tree_update_predecessors(qdev, new_phy);
}

#define ARB_STR(a)                             \
	(a == PP_QOS_ARBITRATION_WSP ? "WSP" : \
	a == PP_QOS_ARBITRATION_WRR ? "WRR" :  \
	a == PP_QOS_ARBITRATION_WFQ ? "WFQ" :  \
	"Unknown")

#define QOS_DBG_PRINT(s, msg, ...) (s ? seq_printf(s, msg, ##__VA_ARGS__) : \
				    pr_info(msg, ##__VA_ARGS__))

static void qos_dbg_dump_subtree(struct pp_qos_dev *qdev, struct qos_node *node,
				 u32 depth, u32 printed_depth, bool user_tree,
				 struct seq_file *s)
{
	u32 idx, tab_idx, n = 0;
	u32 child_phy, node_id;
	char tabs_str[64];
	char indent_str[64];
	char aqm_codel_str[64];
	char weight_str[64] = "";
	char bw_str[64] = "";
	bool last_child;
	bool aqm_queue;
	bool codel_queue;
	struct qos_node *child;

	if (unlikely(depth > QOS_MAX_PREDECESSORS)) {
		pr_err("Maximum depth of %u exceeded\n", QOS_MAX_PREDECESSORS);
		return;
	}

	tabs_str[0] = '\0';
	for (tab_idx = 0 ; tab_idx < printed_depth ; tab_idx++)
		n += snprintf(tabs_str + n, 64 - n, "|\t");

	for (idx = 0; idx < node->parent_prop.num_of_children ; ++idx) {
		last_child = (idx == (node->parent_prop.num_of_children - 1));
		child_phy = node->parent_prop.first_child_phy + idx;
		node_id = get_id_from_phy(qdev->mapping, child_phy);
		child = get_node_from_phy(qdev->nodes, child_phy);

		/* Skip current node print but iterate thorugh its children */
		if (node_rep(child) && user_tree) {
			if (node_sched(child)) {
				qos_dbg_dump_subtree(qdev, child, depth + 1,
						     depth, user_tree, s);
			}
			continue;
		}

		if (last_child)
			snprintf(indent_str, 64, "%s'-- ", tabs_str);
		else
			snprintf(indent_str, 64, "%s|-- ", tabs_str);

		bw_str[0] = '\0';
		weight_str[0] = '\0';
		aqm_codel_str[0] = '\0';
		if (node_sched(child)) {
			if (child->bandwidth_limit)
				snprintf(bw_str, 64, ", bw %u",
					 child->bandwidth_limit);

			if (!user_tree && (node->parent_prop.arbitration ==
			    PP_QOS_ARBITRATION_WRR))
				snprintf(weight_str, sizeof(weight_str),
					 ", w %u, vw %u",
					 child->child_prop.bw_weight,
					 child->child_prop.virt_bw_share);

			QOS_DBG_PRINT(s, "%sSched%s-%u(%u%s%s)-%s\n",
				      indent_str, node_rep(child) ? "'" : "",
				      node_id, child_phy, bw_str, weight_str,
				      ARB_STR(child->parent_prop.arbitration));
			qos_dbg_dump_subtree(qdev, child, depth + 1, depth + 1,
					     user_tree, s);
		} else if (node_queue(child)) {
			aqm_queue = !!QOS_BITS_IS_SET(child->flags,
						      NODE_FLAGS_QUEUE_AQM);
			if (aqm_queue) {
				snprintf(aqm_codel_str, 64, "-aqm(%u)",
					 child->data.queue.aqm_context);
			}

			codel_queue = !!child->data.queue.codel_en;
			if (codel_queue)
				snprintf(aqm_codel_str, 64, "-codel");

			if ((child->bandwidth_limit) ||
			    child->data.queue.eir)
				snprintf(bw_str, 64, ", bw %u/%u",
					 child->bandwidth_limit,
					 child->data.queue.eir);

			if (!user_tree && (node->parent_prop.arbitration ==
			    PP_QOS_ARBITRATION_WRR))
				snprintf(weight_str, sizeof(weight_str),
					 ", w %u, vw %u",
					 child->child_prop.bw_weight,
					 child->child_prop.virt_bw_share);

			QOS_DBG_PRINT(s, "%s%sQueue%s-%u(%u%s%s)-rlm-%u%s%s\n",
				      indent_str,
				      node_syncq(child) ? "Sync-" : "",
				      node_rep(child) ? "'" : "",
				      node_id, child_phy,
				      bw_str, weight_str,
				      child->data.queue.rlm,
				      is_q_ewsp(qdev, node_id) ? "-ewsp" : "",
				      (aqm_queue || codel_queue) ?
				      aqm_codel_str : "");
		}
	}
}

void __qos_dbg_tree_show(struct pp_qos_dev *qdev, bool user_tree,
			 struct seq_file *s)
{
	struct qos_node *node;
	u32 node_id, node_phy;
	char bw_str[64];

	/* Iterate through all port nodes */
	for (node_phy = 0; node_phy < NUM_OF_NODES; ++node_phy) {
		node = get_node_from_phy(qdev->nodes, node_phy);
		node_id = get_id_from_phy(qdev->mapping, node_phy);
		if (!node_port(node))
			continue;

		if (node->bandwidth_limit)
			snprintf(bw_str, 64, ", bw %u)",
				 node->bandwidth_limit);
		else
			snprintf(bw_str, 64, ")");

		QOS_DBG_PRINT(s, "|-- Port-%u(%u%s-%s\n",
			      node_id,
			      get_phy_from_id(qdev->mapping, node_id),
			      bw_str,
			      ARB_STR(node->parent_prop.arbitration));

		qos_dbg_dump_subtree(qdev, node, 1, 1, user_tree, s);
	}
}

void qos_dbg_tree_show(struct pp_qos_dev *qdev, bool user_tree,
		       struct seq_file *s)
{
	__qos_dbg_tree_show(qdev, user_tree, s);
}

void qos_dbg_tree_show_locked(struct pp_qos_dev *qdev, bool user_tree,
			      struct seq_file *s)
{
	QOS_LOCK(qdev);
	__qos_dbg_tree_show(qdev, user_tree, s);
	QOS_UNLOCK(qdev);
}

s32 qos_dbg_qm_stat_show(struct pp_qos_dev *qdev, struct seq_file *s)
{
	struct queue_stat_info *cur_stat;
	struct qm_info *qminfo;
	u32 num_rlms;
	u32 rlm_idx, j;
	s32 ret;

	if (!qos_device_ready(qdev)) {
		seq_puts(s, "Device is not ready\n");
		return 0;
	}

	qminfo = kzalloc(sizeof(*qminfo), GFP_KERNEL);
	if (!qminfo)
		return 0;

	QOS_DBG_PRINT(s, "Queue\t\tQocc(p)\t\tQMGRocc(p)\tAccept(p)\tDrop(p)"
			 "\t\tRed dropped(p)\n");

	num_rlms = qdev->init_params.max_queues;

	QOS_LOCK(qdev);
	for (rlm_idx = 0; rlm_idx < num_rlms; rlm_idx += NUM_Q_ON_QUERY) {
		create_get_qm_stats_cmd(qdev, rlm_idx,
					qdev->hwconf.fw_stats_ddr_phys, qminfo);

		update_cmd_id(&qdev->drvcmds);
		ret = transmit_cmds(qdev);
		if (ret)
			goto unlock;

		if (qminfo->num_queues > NUM_Q_ON_QUERY) {
			QOS_LOG_ERR("Max Q on query is %u (received %u)\n",
				    NUM_Q_ON_QUERY, qminfo->num_queues);
			goto unlock;
		}

		cur_stat = &qminfo->qstat[0];
		for (j = 0; j < qminfo->num_queues; ++j) {
			QOS_DBG_PRINT(s, "%u\t\t%u\t\t%u\t\t%u\t\t%u\t\t%u\n",
				      cur_stat->qid,
				      cur_stat->qstat.queue_size_entries,
				      cur_stat->qstat.qmgr_num_queue_entries,
				      cur_stat->qstat.total_accepts,
				      cur_stat->qstat.total_drops,
				      cur_stat->qstat.total_red_dropped);

			cur_stat++;
		}
	}

unlock:
	QOS_UNLOCK(qdev);

	kfree(qminfo);
	return 0;
}

s32 qos_drv_stats_get(struct pp_qos_dev *qdev, struct qos_drv_stats *stats)
{
	const struct qos_node *node;
	enum rlm_range range;

	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	if (unlikely(!qos_device_ready(qdev)))
		return -EPERM;

	memset(stats, 0, sizeof(*stats));

	for_each_qos_used_node(qdev, node) {
		stats->active_nodes++;
		if (node_port(node)) {
			stats->active_ports++;
		} else if (node_queue(node)) {
			range = rlm_get_range_type(qdev, node->data.queue.rlm);
			switch (range) {
			case RLM_RANGES_RESERVED:
				stats->active_reserved_queues++;
				break;
			case RLM_RANGES_SYNCQ:
			case RLM_RANGES_NORMAL:
				stats->active_queues++;
				break;
			default:
				QOS_LOG_ERR("Unknown queue type %u\n", range);
			}
		} else if (node_internal(node)) {
			stats->active_internals++;
		} else if (node_sched(node)) {
			stats->active_sched++;
		}
	}

	return 0;
}

s32 qos_uc_mcdma_copy_from_hw(struct pp_qos_dev *qdev, u32 src_phy, void *dst,
			      u32 size)
{
	s32 ret;

	if (ptr_is_null(dst))
		return -EINVAL;

	if (!qos_device_ready(qdev))
		return -EPERM;

	if (qdev->hwconf.dma_buff_sz < size) {
		QOS_LOG_ERR("dma buffer too small\n");
		return -ENOMEM;
	}

	QOS_LOCK(qdev);
	create_mcdma_copy_cmd(qdev, src_phy, qdev->hwconf.dma_buff_phys, size);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret))
		QOS_LOG_ERR("dma copy failed (%d)\n", ret);
	pp_cache_invalidate(qdev->hwconf.dma_buff_virt, size);
	/* copy to the user buffer */
	memcpy(dst, qdev->hwconf.dma_buff_virt, size);
	QOS_UNLOCK(qdev);

	return ret;
}

s32 qos_uc_mcdma_copy_to_hw(struct pp_qos_dev *qdev, void *src, u32 dst_phy,
			    u32 size)
{
	s32 ret;

	if (ptr_is_null(src))
		return -EINVAL;

	if (!qos_device_ready(qdev))
		return -EPERM;

	if (qdev->hwconf.dma_buff_sz < size) {
		QOS_LOG_ERR("dma buffer too small\n");
		return -ENOMEM;
	}

	QOS_LOCK(qdev);
	/* copy from the user buffer */
	memcpy(qdev->hwconf.dma_buff_virt, src, size);
	pp_cache_writeback(qdev->hwconf.dma_buff_virt, size);

	create_mcdma_copy_cmd(qdev, qdev->hwconf.dma_buff_phys, dst_phy, size);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret))
		QOS_LOG_ERR("dma copy failed (%d)\n", ret);

	QOS_UNLOCK(qdev);

	return ret;
}

void qos_descriptor_push(struct pp_qos_dev *qdev, u32 queue_id, u32 size,
			 u32 pool, u32 policy, u32 color, u32 gpid,
			 u32 data_off, u32 addr)
{
	struct qos_node *node;
	u32 phy;
	s32 ret;

	QOS_LOCK(qdev);
	phy = get_phy_from_id(qdev->mapping, queue_id);
	node = get_node_from_phy(qdev->nodes, phy);
	if (!node) {
		QOS_LOG_ERR("No node for id %u\n", queue_id);
		goto unlock;
	}

	create_push_desc_cmd(qdev, node->data.queue.rlm, size, pool, policy,
			     color, gpid, data_off,
			     addr >> QOS_DESC_PKT_PTR_SHIFT);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret))
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);

unlock:
	QOS_UNLOCK(qdev);
}

void qos_descriptor_pop(struct pp_qos_dev *qdev, u32 queue_id, u32 *descriptor)
{
	dma_addr_t desc_addr_phys;
	struct qos_node *node;
	void   *desc_addr = NULL;
	u32    phy, sz;
	s32 ret;

	memset(descriptor, 0, 16);
	sz = PAGE_ALIGN(QOS_DESC_WORDS_CNT * sizeof(u32));
	desc_addr = pp_dma_alloc(sz, GFP_KERNEL, &desc_addr_phys, true);
	if (!desc_addr)
		return;

	QOS_LOCK(qdev);
	phy = get_phy_from_id(qdev->mapping, queue_id);
	node = get_node_from_phy(qdev->nodes, phy);
	if (!node) {
		QOS_LOG_ERR("No node for id %u\n", queue_id);
		goto free_mem;
	}

	create_pop_desc_cmd(qdev, node->data.queue.rlm, (u32)desc_addr_phys);
	update_cmd_id(&qdev->drvcmds);
	ret = transmit_cmds(qdev);
	if (unlikely(ret)) {
		QOS_LOG_ERR("transmit_cmds failed (%d)\n", ret);
		goto free_mem;
	}

	memcpy((void *)descriptor, (void *)desc_addr, QOS_DESC_WORDS_CNT * 4);
	pp_cache_invalidate(desc_addr, QOS_DESC_WORDS_CNT * sizeof(u32));

free_mem:
	QOS_UNLOCK(qdev);
	pp_dma_free(sz, desc_addr, &desc_addr_phys, true);
}

static void qos_init_wred_ports_flm(struct pp_qos_dev *qdev)
{
	u32 p;

	for (p = 0; p < qdev->init_params.max_ports; p++) {
		PP_REG_WR32(WRED_PORT_YELLOW_BYTES_COUNTER(p), 0);
		PP_REG_WR32(WRED_PORT_GREEN_BYTES_COUNTER(p), 0);
		PP_REG_WR32(WRED_PORT_YELLOW_THRESHOLD(p), 0);
		PP_REG_WR32(WRED_PORT_GREEN_THRESHOLD(p), 0);
	}
}

static void qos_init_wred_ports_lgm(struct pp_qos_dev *qdev)
{
	u32 p;

	for (p = 0; p < qdev->init_params.max_ports; p++) {
		PP_REG_WR32(WRED_PORT_YELLOW_BYTES_COUNTER_LGM(p), 0);
		PP_REG_WR32(WRED_PORT_GREEN_BYTES_COUNTER_LGM(p), 0);
		PP_REG_WR32(WRED_PORT_YELLOW_THRESHOLD_LGM(p), 0);
		PP_REG_WR32(WRED_PORT_GREEN_THRESHOLD_LGM(p), 0);
	}
}

static void qos_init_wred_ports(struct pp_qos_dev *qdev)
{
	pr_info("Init QoS %u ports\n", qdev->init_params.max_ports);
	if (IS_ENABLED(CONFIG_PPV4_LGM))
		qos_init_wred_ports_lgm(qdev);
	else
		qos_init_wred_ports_flm(qdev);
}

static int qos_init_wred_queues(struct pp_qos_dev *qdev)
{
	u32 q;

	pr_info("Init QoS %u queues\n", qdev->init_params.max_queues);

	for (q = 0; q < qdev->init_params.max_queues; q++) {
		if (IS_ENABLED(CONFIG_PPV4_LGM)) {
			PP_REG_WR32(WRED_QUEUE_TO_PORT_LOOKUP_LGM(q), 0);
			PP_REG_WR32(WRED_Q_SIZE_BYTES_DIRECT_READ_LGM(q), 0);
		} else {
			PP_REG_WR32(WRED_QUEUE_TO_PORT_LOOKUP(q), 0);
			PP_REG_WR32(WRED_Q_SIZE_BYTES_DIRECT_READ(q), 0);
		}

		if (IS_ENABLED(CONFIG_PPV4_LGM) &&
		    pp_silicon_step_get() == PP_SSTEP_B)
			continue;

		/* clear queue counters */
		PP_REG_WR32(PP_QOS_WRED_Q_ADDR_REG, q);
		PP_REG_WR32(PP_QOS_WRED_Q_CNTS_CLR_REG, 0x3FF);
		PP_REG_WR32(PP_QOS_WRED_MEM_CMD_REG,
			    WRED_CMD_OP_READ_CLR_Q_COUNTERS);
		if (pp_reg_poll(PP_QOS_WRED_MEM_ACCESS_BUSY_REG, BIT(0), 0)) {
			pr_err("Q%u reset counters command fails\n", q);
			return -EIO;
		}

		PP_REG_WR32(PP_QOS_WRED_Q_ADDR_REG, q);
		PP_REG_WR32(PP_QOS_WRED_MEM_CMD_REG, WRED_CMD_OP_REMOVE_Q);
		if (pp_reg_poll(PP_QOS_WRED_MEM_ACCESS_BUSY_REG, BIT(0), 0)) {
			pr_err("Q%u remove command fails\n", q);
			return -EIO;
		}
	}

	if (IS_ENABLED(CONFIG_PPV4_LGM) &&
	    pp_silicon_step_get() == PP_SSTEP_B) {
		/* Initializes all queues (Remove all queues) */
		PP_REG_WR32(WRED_INIT_REG, 1);

		// Wait for init done
		if (pp_reg_poll(WRED_INIT_REG, BIT(31), 1)) {
			pr_err("WRED init fails\n");
			return -EIO;
		}
	}

	return 0;
}

int qos_init_wred_hw(struct pp_qos_dev *qdev)
{
	qos_init_wred_ports(qdev);

	return qos_init_wred_queues(qdev);
}
