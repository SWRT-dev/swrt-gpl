
/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2017 Intel Corporation.
 * *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *  The full GNU General Public License is included in this distribution
 *  in the file called LICENSE.GPL.
 *
 *  Contact Information:
 *  Intel Corporation
 *  2200 Mission College Blvd.
 *  Santa Clara, CA  97052
 */

#include <linux/gcd.h>
#include <linux/slab.h>
#include "pp_qos_common.h"
#include "pp_qos_fw.h"
#include "pp_qos_utils.h"


#define QOS_INVALID_PRIORITY   0xF

/******************************************************************************/
/*                                 OCTETS                                     */
/******************************************************************************/
struct octet_info {
	struct list_head list;
	uint8_t usage;
};

struct pp_octets {
	unsigned int last_port_octet;
	struct octet_info info[NUM_OF_OCTETS];
	struct list_head lists[9];
};

static unsigned int octet_get_use_count(const struct pp_octets *octets,
					unsigned int octet)
{
	return octets->info[octet].usage;
}

unsigned int octet_get_with_at_least_free_entries(struct pp_octets *octets,
						  unsigned int count)
{
	unsigned int id;
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

unsigned int  octet_get_least_free_entries(struct pp_octets *octets)
{
	return octet_get_with_at_least_free_entries(octets, 1);
}

static void octet_set_use_count(struct pp_octets *octets,
				unsigned int octet,
				unsigned int usage)
{
	struct list_head *list;

	QOS_ASSERT(QOS_OCTET_VALID(octet), "Illegal octet %u\n", octet);
	QOS_ASSERT(usage <= 8, "Illegal usage %u\n", usage);
	octets->info[octet].usage = usage;
	if (octet > octets->last_port_octet) {
		list = octets->lists + usage;
		list_move_tail(&(octets->info[octet].list), list);
	}
}

STATIC_UNLESS_TEST struct pp_octets *octets_init(unsigned int last_port_octet)
{
	unsigned int i;
	struct octet_info *info;
	struct pp_octets *octets;

	octets = QOS_MALLOC(sizeof(struct pp_octets));
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

STATIC_UNLESS_TEST void octets_clean(struct pp_octets *octets)
{
	if (octets)
		QOS_FREE(octets);
}

#ifdef PP_QOS_TEST
STATIC_UNLESS_TEST void debug_verify_octet_usage(struct pp_octets *octets,
						 unsigned int octet,
						 unsigned int usage)
{
	struct list_head *list;
	struct list_head *pos;
	unsigned int tmp;
	int found;

	tmp = octet_get_use_count(octets, octet);
	QOS_ASSERT(tmp == usage, "Octet %u count is %u and not %u\n", octet,
		   tmp, usage);

	list = &(octets->info[octet].list);
	if (octet > octets->last_port_octet) {
		found = 0;
		for (pos = list->next; pos != list; pos = pos->next) {
			if (pos >= octets->lists && pos <= octets->lists + 8) {
				tmp = (pos - octets->lists);
				QOS_ASSERT(tmp == usage,
					   "Octet is on list %u but should be on %u\n",
					   tmp, usage);
				found = 1;
				break;
			}
		}
		QOS_ASSERT(found, "Did not find list of octet %u\n", octet);
	} else {
		QOS_ASSERT(list_empty(list),
			"Octet %u is on a list though its a port octet\n",
			octet);
	}
}
#endif

/******************************************************************************/
/*                        Ids, Rlms and Port phys pools                       */
/******************************************************************************/
struct pp_pool {
	unsigned int capacity;
	unsigned int top;
	uint16_t invalid;
	uint16_t data[1];
};

static struct pp_pool *pp_pool_init(unsigned int capacity, uint16_t invalid)
{
	struct pp_pool *tmp;

	tmp = QOS_MALLOC(sizeof(struct pp_pool) +
			(capacity - 1) * sizeof(uint16_t));
	if (tmp) {
		tmp->capacity = capacity;
		tmp->top = 0;
		tmp->invalid = invalid;
	}
	return tmp;
}

uint16_t pp_pool_get(struct pp_pool *pool)
{
	uint16_t tmp;
	unsigned int top;

	top = pool->top;
	if (top > 0) {
		--top;
		tmp = pool->data[top];
		pool->top = top;
		return tmp;
	}
	return pool->invalid;
}

int pp_pool_put(struct pp_pool *pool, uint16_t data)
{
	unsigned int top;

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
	if (pool)
		QOS_FREE(pool);
}

static struct pp_pool *free_id_init(void)
{
	unsigned int i;
	struct pp_pool *tmp;

	tmp  = pp_pool_init(NUM_OF_NODES, QOS_INVALID_ID);
	if (tmp)
		for (i = NUM_OF_NODES; i > 0; --i)
			pp_pool_put(tmp, i - 1);

	return tmp;
}

static struct pp_pool *free_rlm_init(void)
{
	unsigned int i;
	struct pp_pool *tmp;

	tmp  = pp_pool_init(NUM_OF_QUEUES, QOS_INVALID_ID);
	if (tmp)
		for (i = NUM_OF_QUEUES; i > 0; --i)
			pp_pool_put(tmp, i - 1);

	return tmp;
}


struct pp_pool *free_ports_phys_init(unsigned int *reserved,
				     unsigned int max_port,
				     const unsigned int *reserved_ports,
				     unsigned int size)
{
	unsigned int i;
	unsigned int res;
	unsigned int tmp;
	struct pp_pool *pool;

	res = 0;
	tmp = min(size, max_port + 1);
	memcpy(reserved, reserved_ports, tmp * sizeof(unsigned int));

	for (i = 0; i < tmp; ++i)
		if (reserved[i] == 1)
			++res;

	tmp = max_port + 1 - res;
	pool  = pp_pool_init(tmp, QOS_INVALID_PHY);
	if (pool)
		for (i = max_port + 1; i > 0; --i)
			if (reserved[i - 1] == 0)
				pp_pool_put(pool, i - 1);
	return pool;
}

/******************************************************************************/
/*                                 Queue                                      */
/******************************************************************************/
struct q_item {
	struct list_head list;
	uint16_t data;
};

struct pp_queue {
	unsigned int capacity;
	struct list_head used;
	struct list_head free;
	struct q_item items[1];
};

uint16_t pp_queue_dequeue(struct pp_queue *queue)
{
	struct q_item *item;
	int rc;

	if (!list_empty(&queue->used)) {
		item = list_entry(queue->used.next, struct q_item, list);
		rc = item->data;
		list_move_tail(&item->list, &queue->free);
	} else {
		QOS_LOG("queue is empty\n");
		rc = QOS_INVALID_ID;
	}
	return rc;
}

int pp_queue_enqueue(struct pp_queue *queue, uint16_t data)
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

static int queue_is_reset(struct pp_queue *queue)
{
	unsigned int i;
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
	if (queue)
		QOS_FREE(queue);
}

static struct pp_queue *pp_queue_init(unsigned int capacity)
{
	struct pp_queue *queue;
	unsigned int i;

	queue = QOS_MALLOC(sizeof(struct pp_queue) +
			   sizeof(struct q_item) * (capacity - 1));
	if (queue) {
		queue->capacity = capacity;
		INIT_LIST_HEAD(&(queue->free));
		INIT_LIST_HEAD(&(queue->used));
		for (i = 0; i < queue->capacity; ++i)
			list_add_tail(&(queue->items[i].list), &(queue->free));
	}
	return queue;
}

/******************************************************************************/
/*                             Cmd Queue                                      */
/******************************************************************************/
#define CMD_QUEUE_SIZE 16384LU
struct cmd_queue {
	size_t read;  /* next index to read */
	size_t write; /* next free index to write */
	size_t count; /* how many bytes are in */
	uint8_t data[CMD_QUEUE_SIZE];
};

static struct cmd_queue *cmd_queue_init(void)
{
	struct cmd_queue *q;

	q = QOS_MALLOC(sizeof(struct cmd_queue));
	if (q) {
		q->read = 0;
		q->write = 0;
		q->count = 0;
	}
	return q;
}

static void cmd_queue_clean(struct cmd_queue *q)
{
	if (q)
		QOS_FREE(q);
}

int cmd_queue_put(struct cmd_queue *q, void *_cmd, size_t size)
{
	unsigned int toend;
	uint8_t *cmd;

	cmd = _cmd;

	QOS_ASSERT(CMD_QUEUE_SIZE - q->count >= size,
		   "%lu bytes free, can't accommodate %zu bytes\n",
		   CMD_QUEUE_SIZE - q->count, size);

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

	return 0;
}

static int cmd_queue_read(struct cmd_queue *q,
		void *_cmd,
		size_t size,
		int remove)
{
	unsigned int toend;
	uint8_t *cmd;

	cmd = _cmd;

	if (q->count < size) {
		QOS_LOG_DEBUG("has only %zu bytes, can't retrieve %zu bytes\n",
			      q->count, size);
		return -1;
	}

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

	return 0;
}

int cmd_queue_get(struct cmd_queue *q, void *_cmd, size_t size)
{
	return cmd_queue_read(q, _cmd, size, 1);
}

int cmd_queue_peek(struct cmd_queue *q, void *_cmd, size_t size)
{
	return cmd_queue_read(q, _cmd, size, 0);
}

int cmd_queue_is_empty(struct cmd_queue *q)
{
	return (q->count == 0);
}

/******************************************************************************/
/*                                 Nodes                                      */
/******************************************************************************/
struct pp_nodes {
	struct qos_node nodes[NUM_OF_NODES];
};

static struct pp_nodes *pp_nodes_init(void)
{
	struct pp_nodes *nodes;

	nodes = QOS_MALLOC(sizeof(struct pp_nodes));
	if (nodes)
		memset(nodes, 0, sizeof(struct pp_nodes));

	return nodes;
}

unsigned int get_phy_from_node(const struct pp_nodes *nodes,
		const struct qos_node *node)
{
	QOS_ASSERT(node >= nodes->nodes && node <= nodes->nodes + NUM_OF_NODES,
		   "invalid node pointer\n");
	return node - nodes->nodes;
}

struct qos_node *get_node_from_phy(struct pp_nodes *nodes, unsigned int phy)
{
	QOS_ASSERT(QOS_PHY_VALID(phy), "invalid phy %u\n", phy);
	return nodes->nodes + phy;
}

const struct qos_node *get_const_node_from_phy(const struct pp_nodes *nodes,
					       unsigned int phy)
{
	QOS_ASSERT(QOS_PHY_VALID(phy), "invalid phy %u\n", phy);
	return nodes->nodes + phy;
}

static void pp_nodes_clean(struct pp_nodes *nodes)
{
	if (nodes)
		QOS_FREE(nodes);
}
#if 0
static unsigned int get_child_node_order(
		struct pp_nodes *nodes,
		const struct qos_node *node)
{
	unsigned int phy;
	const struct qos_node *parent_node;

	QOS_ASSERT(node_child(node), "Node is not a child\n");
	phy = get_phy_from_node(nodes, node);
	parent_node = get_const_node_from_phy(nodes,
			node->child_prop.parent_phy);
	return (phy - parent_node->parent_prop.first_child_phy);
}
#endif
/******************************************************************************/
/*                                 Mapping                                    */
/******************************************************************************/
struct pp_mapping {
	uint16_t id2phy[NUM_OF_NODES];
	uint16_t phy2id[NUM_OF_NODES];
};

unsigned int get_id_from_phy(const struct pp_mapping *map, unsigned int phy)
{
	QOS_ASSERT(QOS_PHY_VALID(phy), "invalid phy %u\n", phy);
	return map->phy2id[phy];
}

unsigned int get_phy_from_id(const struct pp_mapping *map, unsigned int id)
{
	QOS_ASSERT(QOS_ID_VALID(id), "invalid id %u\n", id);
	return map->id2phy[id];
}

void map_id_phy(struct pp_mapping *map, unsigned int id, unsigned int phy)
{
	QOS_ASSERT(QOS_ID_VALID(id), "invalid id %u\n", id);
	QOS_ASSERT(QOS_PHY_VALID(phy), "invalid phy %u\n", phy);
	map->id2phy[id] = phy;
	map->phy2id[phy] = id;
}

void map_id_reserved(struct pp_mapping *map, unsigned int id)
{
	QOS_ASSERT(QOS_ID_VALID(id), "invalid id %u\n", id);
	map->id2phy[id] = QOS_UNKNOWN_PHY;
}

static struct pp_mapping *pp_mapping_init(void)
{
	unsigned int i;
	struct pp_mapping *map;

	map = QOS_MALLOC(sizeof(struct pp_mapping));
	if (map) {
		for (i = 0; i < NUM_OF_NODES; ++i) {
			map->id2phy[i] = QOS_INVALID_PHY;
			map->phy2id[i] = QOS_INVALID_ID;
		}
	}
	return map;
}

static void pp_mapping_clean(struct pp_mapping *map)
{
	if (map)
		QOS_FREE(map);
}

void map_invalidate_id(struct pp_mapping *map, unsigned int id)
{
	unsigned int phy;

	if (!QOS_ID_VALID(id)) {
		QOS_LOG_ERR("invalid id %u\n", id);
		return;
	}
	phy = get_phy_from_id(map, id);

	/* If phy is not unknown verify it is valid */
	if (!QOS_PHY_UNKNOWN(phy)) {
		if (QOS_PHY_VALID(phy)) {
			map->phy2id[phy] = PP_QOS_INVALID_ID;
		} else {
			QOS_LOG_ERR("invalid phy is mapped to id %u\n", id);
		}
	}

	map->id2phy[id] = QOS_INVALID_PHY;
}

/******************************************************************************/
/*                                 Moving                                     */
/******************************************************************************/
/*
 * Return 1 if used status of all count nodes starting from first_phy equals
 * status
 */
static int is_all_nodes_used_status(const struct pp_qos_dev *qdev,
		unsigned int first_phy, unsigned int count, unsigned int status)
{
	const struct qos_node *cur;
	unsigned int used;

	cur = get_const_node_from_phy(qdev->nodes, first_phy);
	for (; count; --count) {
		used = !!QOS_BITS_IS_SET(cur->flags, QOS_NODE_FLAGS_USED);
		if (used != status)
			return 0;
		cur++;
	}
	return 1;
}

/* is child_phy a child of parent_phy */
static int node_child_of(struct pp_nodes *nodes,
		unsigned int child_phy, unsigned int parent_phy)
{
	const struct qos_node *parent;

	parent = get_const_node_from_phy(nodes, parent_phy);

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
static void node_update_children_internal(
				   struct pp_nodes *nodes,
				   struct qos_node *tmp_nodes,
				   unsigned int first,
				   unsigned int count,
				   struct qos_node *parent,
				   unsigned int new_phy)

{
	unsigned int last;
	unsigned int cur;
	struct qos_node *child;

	cur = parent->parent_prop.first_child_phy;
	last = cur + parent->parent_prop.num_of_children - 1;
	for (; cur <= last; ++cur) {
		if (cur >= first && cur < first + count)
			child = tmp_nodes + cur - first;
		else
			child = get_node_from_phy(nodes, cur);
		child->child_prop.parent_phy = new_phy;
	}
}

/*
 * Update the children of a parent which is moving from phy to new_phy
 * Children parent_phy's is updated
 */
void node_update_children(
			  struct pp_qos_dev *qdev,
			  struct qos_node *parent,
			  unsigned int new_phy)
{
	unsigned int num;
	struct qos_node *child;

	num = parent->parent_prop.num_of_children;
	if (!num)
		return;

	child = get_node_from_phy(qdev->nodes,
				  parent->parent_prop.first_child_phy);

	for (; num; --num)  {
		child->child_prop.parent_phy = new_phy;
		++child;
	}
}


/*
 * Update parent's first child phy and num of children, given that count nodes
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
static int node_update_parent(struct pp_qos_dev *qdev,
		struct qos_node *tmpnodes, unsigned int src_first,
		unsigned int size, unsigned int src_phy, unsigned int dst_phy,
		unsigned int count)
{
	unsigned int first;
	unsigned int last;
	unsigned int num;
	unsigned int parent_phy;
	unsigned int moving;
	struct qos_node *parent;
	struct qos_node *child;
	struct pp_nodes *nodes;

	nodes = qdev->nodes;
	child = get_node_from_phy(nodes, src_phy);
	parent_phy = child->child_prop.parent_phy;

	if (parent_phy >= src_first && parent_phy < src_first + size)
		parent = tmpnodes + parent_phy - src_first;
	else
		parent = get_node_from_phy(nodes, parent_phy);

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

/*
 * Given count nodes starting at src_phy [src_phy..src_phy + count - 1],
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
		unsigned int src_phy, unsigned int dst_phy, unsigned int count)
{
	unsigned int i;
	unsigned int tmp;
	struct qos_node *next_updated_parent;
	struct qos_node *node;
	struct pp_mapping *map;
	struct qos_node tmp_nodes[MAX_MOVING_NODES];
	unsigned int tmp_map[MAX_MOVING_NODES];
	unsigned int updated_parents[MAX_MOVING_NODES];
	unsigned int index;

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

	/*
	 * Invalidate current mapping, update children and parents of
	 * moved nodes
	 */
	node = get_node_from_phy(qdev->nodes, src_phy);
	next_updated_parent = node;
	for (i = src_phy; i < src_phy + count; ++i) {
		tmp = get_id_from_phy(map, i);
		tmp_map[i - src_phy] = tmp;
		map_invalidate_id(map, tmp);
		if (node_parent(node))
			node_update_children_internal(
					qdev->nodes,
					tmp_nodes,
					src_phy,
					count,
					node,
					dst_phy + i - src_phy);

		if (node_child(node) && (node >= next_updated_parent)) {
			next_updated_parent =  node +
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
		create_parent_change_cmd(qdev, updated_parents[i]);
}

/*
 * For small arrays this is probably quicker than
 * the heap sort kernel is using
 */
static unsigned int remove_duplicates(unsigned int *data, unsigned int size)
{
	unsigned int i;
	unsigned int j;

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

/*
 * Given a group A of count nodes starting at start, find a group B which
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
static unsigned int parents_span(struct pp_nodes *nodes, unsigned int external,
		unsigned int start, unsigned int count,
		unsigned int *data, unsigned int size)
{
	unsigned int i;
	unsigned int parent_phy;
	unsigned int cur_phy;
	const struct qos_node *cur;

	QOS_ASSERT(size >= count,
			"Array size %u is smaller than number of moved nodes %u\n",
			size, count);
	i = 0;
	cur = get_const_node_from_phy(nodes, start);
	for (cur_phy = start; cur_phy < start + count; ++cur_phy) {
		QOS_ASSERT((cur->type == TYPE_UNKNOWN) ||
				node_child(cur), "Node is not a child\n");
		parent_phy = cur->child_prop.parent_phy;
		/* parent not moving */
		if ((parent_phy < start) ||
				(parent_phy >= start + count)) {
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


/*
 * If the src and dst region don't overlap then we can use
 * either strategy of "move last src first" or "move first src first"
 * If they overlap and the move is forward (src < dst) we will use
 * "move last src first".
 * If they overlap and the move is backward (src > dst) we will use
 * "move last src first"
 */
static void create_multiple_move_cmds(struct pp_qos_dev *qdev,
		unsigned int dst_phy,
		unsigned int src_phy,
		unsigned int count)
{
	unsigned int dst_port;
	unsigned int i;
	unsigned int dst;
	unsigned int src;
	unsigned int delta;

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
STATIC_UNLESS_TEST void nodes_move(struct pp_qos_dev *qdev,
		unsigned int dst_phy,
		unsigned int src_phy,
		unsigned int count)
{
	unsigned int i;
	unsigned int j;
#if 0
	unsigned int suspend_size;
	unsigned int suspend[MAX_MOVING_NODES];
#endif
	unsigned int ances_size;
	unsigned int ancestors[MAX_MOVING_NODES];
	unsigned int ports[MAX_MOVING_NODES * 2];
#if 0
	const struct qos_node *node;
#endif

	QOS_LOG_DEBUG("src_phy %u. dst_phy %u. count %u\n",
		      src_phy, dst_phy, count);

	QOS_ASSERT(src_phy != dst_phy,
			"src and dst are the same %u\n",
			src_phy);
	QOS_ASSERT(count < MAX_MOVING_NODES, "Can't move %u nodes, max is %u\n",
			count, MAX_MOVING_NODES);
	if (count) {
	#if 0
		suspend_size = parents_span(qdev->nodes, 1,
				src_phy, count, suspend, MAX_MOVING_NODES);
		for (i = 0; i < suspend_size; ++i)
			create_suspend_cmd(qdev, suspend[i]);
	#endif

		j = 0;
		for (i = src_phy; i < src_phy + count; ++i) {
			ports[j] = get_port(qdev->nodes, i);
			j++;
		}

		nodes_update_stake_holders(qdev, src_phy, dst_phy, count);
		nodes_modify_used_status(qdev, src_phy, count, 0);
		QOS_ASSERT(is_all_nodes_used_status(qdev, dst_phy, count, 0),
			   "Some of the destination nodes are in used\n");
		memmove(get_node_from_phy(qdev->nodes, dst_phy),
				get_node_from_phy(qdev->nodes, src_phy),
				count * sizeof(struct qos_node));
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

#if 0
		/* FW will delete the src node on a move command */
		/* Issue delete node cmd to each node */
		node = get_const_node_from_phy(qdev->nodes, src_phy);
		for (i = 0; i < count; ++i) {
			if (!node_used(node))
				create_remove_node_cmd(qdev,
						TYPE_UNKNOWN,
						i + src_phy, 0);
			++node;
		}
#endif

		/* Issue update preds each node whose ancestor moved */
		ances_size = parents_span(qdev->nodes, 0, dst_phy, count,
				ancestors, MAX_MOVING_NODES);

		for (i = 0; i < ances_size; ++i)
			tree_update_predecessors(qdev, ancestors[i]);

	#if 0
		for (i = 0; i < suspend_size; ++i)
			create_resume_cmd(qdev, suspend[i]);
	#endif
	}
}

/******************************************************************************/
/*                                 Generic                                    */
/******************************************************************************/

struct qos_node *octet_get_min_sibling_group(
		const struct pp_qos_dev *qdev,
		unsigned int octet,
		const struct qos_node *special_parent,
		unsigned int *num_of_children)
{
	unsigned int min_size;
	unsigned int phy;
	unsigned int last_phy;
	unsigned int num_children;
	const struct qos_node *cur;
	struct qos_node *cur_parent;
	struct qos_node *min_parent;

	min_size = 9;
	min_parent = NULL;

	phy = octet * 8;
	last_phy = phy + octet_get_use_count(qdev->octets, octet);
	while (phy < last_phy) {
		cur = get_const_node_from_phy(qdev->nodes, phy);
		cur_parent = get_node_from_phy(qdev->nodes,
				cur->child_prop.parent_phy);
		num_children = cur_parent->parent_prop.num_of_children;
		if ((cur_parent != special_parent) &&
				(num_children < min_size)) {
			min_size = num_children;
			min_parent = cur_parent;
		} else if ((cur_parent == special_parent) &&
				(num_children + 1 < min_size)) {
			min_size = num_children + 1;
			min_parent = cur_parent;
		}
		phy += cur_parent->parent_prop.num_of_children;
	}
	*num_of_children = min_size;
	return min_parent;
}

static void update_octet_usage(struct pp_octets *octets, unsigned int phy,
		unsigned int delta, unsigned int status)
{
	unsigned int octet;
	unsigned int usage;

	octet = octet_of_phy(phy);
	usage = octet_get_use_count(octets, octet);
	if (status == 0)
		usage -= delta;
	else
		usage += delta;
	octet_set_use_count(octets, octet, usage);
}

/*
 * Set used status of count nodes starting from first_phy to status
 * Verify that current used status is different from the value to be set
 */
void nodes_modify_used_status(struct pp_qos_dev *qdev, unsigned int first_phy,
		unsigned int count, unsigned int status)
{
	struct qos_node *cur;
	unsigned int phy;
	unsigned int delta;

	QOS_ASSERT(is_all_nodes_used_status(qdev, first_phy, count, !status),
		   "some nodes used status is already %u\n", status);
	delta = 0;
	for (phy = first_phy; phy < first_phy + count; ++phy) {
		cur = get_node_from_phy(qdev->nodes, phy);
		QOS_BITS_TOGGLE(cur->flags, QOS_NODE_FLAGS_USED);
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
STATIC_UNLESS_TEST void octet_nodes_shift(struct pp_qos_dev *qdev,
		unsigned int first_phy,
		unsigned int count,
		int shift)
{
	if (count != 0 && shift != 0) {
		QOS_ASSERT(same_octet(first_phy, first_phy + count - 1) &&
			   same_octet(first_phy, first_phy + shift) &&
			   same_octet(first_phy, first_phy + shift + count - 1)
			   ,
			   "shift %d places of %u nodes from %u crosses octet boundaries\n",
			   shift, count, first_phy);
		nodes_move(qdev, first_phy + shift, first_phy, count);
	}
}

/*
 *
 */
void node_phy_delete(struct pp_qos_dev *qdev, unsigned int phy)
{
	unsigned int usage;
	unsigned int reminder;
	struct qos_node *node;
	struct qos_node *parent;
	unsigned int id;
	unsigned int parent_phy;
	unsigned int update_bw;

	update_bw = 0;
	usage = octet_get_use_count(qdev->octets, octet_of_phy(phy));
	node = get_node_from_phy(qdev->nodes, phy);

	QOS_ASSERT(!node_parent(node) || node->parent_prop.num_of_children == 0,
			"Try to delete node %u that has children\n", phy);

	if (node_child(node)) {
		parent_phy = node->child_prop.parent_phy;
		parent = get_node_from_phy(qdev->nodes, parent_phy);
		if (node->child_prop.virt_bw_share) {
			update_bw = 1;
			id = get_id_from_phy(qdev->mapping, parent_phy);
		}

		/*
		 * If this is the last child of a parent than shift
		 * operation will not update the parent, since its children
		 * are not moving - we have to update it manually
		 */
		if (phy == parent->parent_prop.first_child_phy +
				parent->parent_prop.num_of_children - 1) {
			--parent->parent_prop.num_of_children;
			create_parent_change_cmd(qdev, parent_phy);
		}
	}
	nodes_modify_used_status(qdev, phy, 1, 0);
	reminder = usage - (octet_phy_offset(phy) + 1);
	if (octet_of_phy(phy) > octet_of_phy(qdev->max_port))
		octet_nodes_shift(qdev, phy + 1, reminder, -1);

	if (update_bw) {
		phy = get_phy_from_id(qdev->mapping, id);
		parent = get_node_from_phy(qdev->nodes, phy);
		update_internal_bandwidth(qdev, parent);
	}
}

void release_rlm(struct pp_pool *rlms, unsigned int rlm)
{
	pp_pool_put(rlms, rlm);
}

void release_id(struct pp_qos_dev *qdev, unsigned int id)
{
	map_invalidate_id(qdev->mapping, id);
	pp_pool_put(qdev->ids, id);
}

static void release_node_id(struct pp_qos_dev *qdev, unsigned int phy)
{
	unsigned int id;

	id = get_id_from_phy(qdev->mapping, phy);
	QOS_LOG_DEBUG("Deleting id %u phy %u\n", id, phy);
	QOS_ASSERT(QOS_ID_VALID(id), "Invalid id for phy %u\n", phy);
	release_id(qdev, id);
}

int node_remove(struct pp_qos_dev *qdev, struct qos_node *node)
{
	unsigned int phy;
	enum node_type type;
	unsigned int rlm;

	phy = get_phy_from_node(qdev->nodes, node);
	type = node->type;
	QOS_ASSERT(type != TYPE_UNKNOWN, "Unknown node type of phy %u\n",
			phy);

	if (type == TYPE_QUEUE)
		rlm  = node->data.queue.rlm;
	else
		rlm = 0;
	create_remove_node_cmd(qdev, type, phy, rlm);
	release_node_id(qdev, phy);
	if (node_queue(node))
		release_rlm(qdev->rlms, node->data.queue.rlm);

	node_phy_delete(qdev, get_phy_from_node(qdev->nodes, node));
	return 0;
}

int node_flush(struct pp_qos_dev *qdev, struct qos_node *node)
{
	unsigned int phy;
	unsigned int id;
	unsigned int rlm;

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
	return 0;
}

#if 0 // Testing purposes
// Calc greatest_common_divisor
static int calc_gcd(int num1, int num2)
{
	// If num1 is 0, then num2 is the result
	if (num1 == 0)
		return num2;

	/// otherwise we are dividing the number and check for the reminder
	return calc_gcd(num2 % num1, num1);
}
#endif

static int calc_gcd_for_n_numbers(int arr[], int count)
{
	int res = arr[0];
	int ind;

	for (ind = 1; ind < count; ind++)
		res = gcd(arr[ind], res); // res = calc_gcd(arr[ind], res);

	return res;
}

static void send_bw_weight_command(struct pp_qos_dev *qdev,
				   const struct qos_node *child,
				   int bw_weight)
{
	struct pp_qos_sched_conf s_conf;
	struct pp_qos_queue_conf q_conf;
	uint32_t modified = 0;

	QOS_LOG_DEBUG("child %u: Sending new BW weight %d\n",
		      get_phy_from_node(qdev->nodes, child),
		      bw_weight);

	QOS_BITS_SET(modified, QOS_MODIFIED_BW_WEIGHT);

	if (node_sched(child)) {
		get_node_prop(qdev, child, &s_conf.common_prop,
			&s_conf.sched_parent_prop,
			&s_conf.sched_child_prop);

		s_conf.sched_child_prop.wrr_weight = bw_weight;

		create_set_sched_cmd(qdev,
			&s_conf,
			get_phy_from_node(qdev->nodes, child),
			child->child_prop.parent_phy,
			modified);
	} else if (node_queue(child)) {
		get_node_prop(qdev, child, &q_conf.common_prop,
				NULL,
				&q_conf.queue_child_prop);

		q_conf.queue_child_prop.wrr_weight = bw_weight;

		create_set_queue_cmd(qdev,
			&q_conf,
			get_phy_from_node(qdev->nodes, child),
			child->child_prop.parent_phy,
			child->data.queue.rlm,
			modified);
	}
}

static void get_children_bw_weight(const struct pp_qos_dev *qdev,
				   const struct qos_node *parent,
				   int weights[],
				   int *num_weights)
{
	struct qos_node *child;
	unsigned int cnt;

	child = get_node_from_phy(qdev->nodes,
			parent->parent_prop.first_child_phy);
	cnt = parent->parent_prop.num_of_children;
	for (; cnt; --cnt) {
		QOS_ASSERT(node_child(child), "Node is not a child\n");
		if (child->child_prop.virt_bw_share) {
			weights[(*num_weights)++] =
					child->child_prop.virt_bw_share;
		}
		++child;
	}
}

static void set_children_bw_weight(struct pp_qos_dev *qdev,
				   const struct qos_node *parent,
				   int gcd)
{
	struct qos_node *child;
	unsigned int cnt;
	int bw_weight = 0;

	child = get_node_from_phy(qdev->nodes,
			parent->parent_prop.first_child_phy);
	cnt = parent->parent_prop.num_of_children;

	for (; cnt; --cnt) {
		QOS_ASSERT(node_child(child), "Node is not a child\n");

		bw_weight = child->child_prop.virt_bw_share / gcd;

		if (child->child_prop.bw_weight != bw_weight) {
			child->child_prop.bw_weight = bw_weight;
			send_bw_weight_command(qdev, child, bw_weight);
		}

		++child;
	}
}

STATIC_UNLESS_TEST unsigned int get_children_bandwidth_share(
		const struct pp_qos_dev *qdev,
		const struct qos_node *parent);

void update_internal_bandwidth(struct pp_qos_dev *qdev,
		struct qos_node *parent)
{
	unsigned int share;
	struct qos_node *cur_parent;
	struct qos_node *internals[10];
	int index; // Should not be unsigned
	int weights[10] = { 0 };
	int num_weights = 0;
	int gcd = 0;

	// Update weights only in WRR. Currently WSP acts as SP
	if (parent->parent_prop.arbitration != PP_QOS_ARBITRATION_WRR)
		return;

	cur_parent = parent;
	index = 0;

	while (node_internal(cur_parent)) {
		QOS_ASSERT(node_internal(cur_parent), "Node is not internal");
		share = get_children_bandwidth_share(qdev, cur_parent);
		cur_parent->child_prop.virt_bw_share = share;
		internals[index++] = cur_parent;
		cur_parent = get_node_from_phy(qdev->nodes,
				cur_parent->child_prop.parent_phy);
	}

	// We arrive here with all internal schedulers added to internals[]
	// array. In addition, the uper level concrete parent (not IS) is held
	// in cur_parent variable.

	// Even if no IS, we still want to enter this "for" once
	for (; index >= 0; index--) {
		get_children_bw_weight(qdev, cur_parent, weights, &num_weights);
		gcd = calc_gcd_for_n_numbers(weights, num_weights + 1);
		set_children_bw_weight(qdev, cur_parent, gcd);

		if (index > 0)
			cur_parent = internals[index-1];
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
static void link_with_parent(
		struct pp_qos_dev *qdev,
		unsigned int phy,
		unsigned int parent_id)
{
	struct qos_node *node;
	struct qos_node *parent;
	unsigned int first;
	unsigned int last;
	unsigned int parent_phy;
	unsigned int num;

	parent_phy = get_phy_from_id(qdev->mapping, parent_id);
	parent = get_node_from_phy(qdev->nodes, parent_phy);
	node = get_node_from_phy(qdev->nodes, phy);
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
	nodes_modify_used_status(qdev, phy, 1, 1);
	create_parent_change_cmd(qdev, parent_phy);

}

int qos_pools_init(struct pp_qos_dev *qdev, unsigned int max_port)
{
	qdev->nodes = pp_nodes_init();
	if (!qdev->nodes)
		goto fail;

	qdev->ids = free_id_init();
	if (!qdev->ids)
		goto fail;

	qdev->rlms = free_rlm_init();
	if (!qdev->rlms)
		goto fail;

	qdev->octets = octets_init(octet_of_phy(max_port));
	if (!qdev->octets)
		goto fail;

	qdev->mapping = pp_mapping_init();
	if (!qdev->mapping)
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
	pp_pool_clean(qdev->ids);
	pp_pool_clean(qdev->rlms);
	octets_clean(qdev->octets);
	pp_mapping_clean(qdev->mapping);
	pp_queue_clean(qdev->queue);
}

void _qos_init(unsigned int max_port, struct pp_qos_dev **qdev)
{
	unsigned int i;

	*qdev = QOS_MALLOC(sizeof(struct pp_qos_dev));
	if (*qdev) {
		memset(*qdev, 0, sizeof(struct pp_qos_dev));
		(*qdev)->max_port = max_port;

		if (qos_pools_init(*qdev, max_port))
			goto fail;

		(*qdev)->drvcmds.cmdq = cmd_queue_init();
		if ((*qdev)->drvcmds.cmdq == NULL)
			goto fail;

		(*qdev)->drvcmds.pendq = cmd_queue_init();
		if ((*qdev)->drvcmds.pendq == NULL)
			goto fail;

		for (i = 0; i <= QOS_MAX_SHARED_BANDWIDTH_GROUP; ++i)
			(*qdev)->groups[i].used = 0;
		QOS_LOCK_INIT((*qdev));
	}
	return;
fail:
	_qos_clean(*qdev);
}

void _qos_clean(struct pp_qos_dev *qdev)
{
	if (qdev) {
		qos_pools_clean(qdev);
		pp_pool_clean(qdev->portsphys);
		clean_fwdata_internals(qdev);
		cmd_queue_clean(qdev->drvcmds.pendq);
		cmd_queue_clean(qdev->drvcmds.cmdq);
		QOS_FREE(qdev);
	}
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
static int post_order_travers_tree(struct pp_qos_dev *qdev,
				    int root,
				    int (*conform)(
					    const struct pp_qos_dev *qdev,
					    const struct qos_node *node,
					    void *cdata),
				    void *cdata,
				    int (*operation)(
					    struct pp_qos_dev *qdev,
					    struct qos_node *node,
					    void *odata),
				    void *odata)
{
	unsigned int i;
	unsigned int phy;
	unsigned int total;
	struct qos_node *node;

	total = 0;
	node = get_node_from_phy(qdev->nodes, root);
	QOS_ASSERT(node_used(node), "Unused node with phy %u\n", root);

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
					cdata, operation, odata);
		}
	}
	if (conform(qdev, node, cdata))
		total += operation(qdev, node, odata);

	return total;
}

struct ids_container_metadata {
	unsigned int next;
	uint16_t *ids;
	unsigned int size;
};

static int update_ids_container(struct pp_qos_dev *qdev,
		struct qos_node *node, void *data)
{
	struct ids_container_metadata *ids;
	unsigned int phy;
	uint16_t id;

	ids = (struct ids_container_metadata *)data;
	phy = get_phy_from_node(qdev->nodes, node);
	id = get_id_from_phy(qdev->mapping, phy);

	if (ids->next < ids->size) {
		ids->ids[ids->next] = id;
		ids->next++;
	}

	return 1;
}

static int update_bwl_buffer_node(struct pp_qos_dev *qdev,
				  struct qos_node *node, void *data)
{
	unsigned int phy;

	phy = get_phy_from_node(qdev->nodes, node);
	qdev->hwconf.bwl_ddr_virt[phy] = node->bandwidth_limit;
	qdev->hwconf.sbwl_ddr_virt[phy] = node->shared_bandwidth_group;

	return 1;
}

struct rlm_container_metadata {
	u32 next;
	u16 *rlm;
	u32 size;
};

static int update_rlm_container(struct pp_qos_dev *qdev,
				struct qos_node *node, void *data)
{
	struct rlm_container_metadata *rlms;
	u16 rlm;

	rlms = (struct rlm_container_metadata *)data;
	rlm = node->data.queue.rlm;

	if (rlms->next < rlms->size) {
		rlms->rlm[rlms->next] = rlm;
		rlms->next++;
	}

	return 1;
}

static int node_queue_wrapper(const struct pp_qos_dev *qdev,
		const struct qos_node *node, void *data)
{
	return node_used(node) && (node->type == TYPE_QUEUE);
}

static int node_active_queue_wrapper(const struct pp_qos_dev *qdev,
				     const struct qos_node *node, void *data)
{
	return node_used(node) && (node->type == TYPE_QUEUE) &&
		(!QOS_BITS_IS_SET(node->flags, QOS_NODE_FLAGS_QUEUE_BLOCKED));
}

void get_node_queues(struct pp_qos_dev *qdev,
		unsigned int phy, uint16_t *queue_ids,
		unsigned int size, unsigned int *queues_num)
{
	struct ids_container_metadata data = {0, queue_ids, size};

	if (queue_ids == NULL)
		data.size = 0;
	*queues_num = post_order_travers_tree(qdev, phy,
			node_queue_wrapper, NULL,
			update_ids_container, &data);
}

static int node_bw_limited_wrapper(const struct pp_qos_dev *qdev,
				   const struct qos_node *node, void *data)
{
	return node_used(node) &&
		(node->bandwidth_limit || node->shared_bandwidth_group);
}

void update_bwl_buffer(struct pp_qos_dev *qdev, unsigned int phy)
{
	post_order_travers_tree(qdev, phy,
				node_bw_limited_wrapper, NULL,
				update_bwl_buffer_node, NULL);
}

void get_active_node_queues(struct pp_qos_dev *qdev,
			    unsigned int phy, uint16_t *queue_ids,
			    unsigned int size, unsigned int *queues_num)
{
	struct ids_container_metadata data = {0, queue_ids, size};

	if (queue_ids == NULL)
		data.size = 0;
	*queues_num = post_order_travers_tree(qdev, phy,
			node_active_queue_wrapper, NULL,
			update_ids_container, &data);
}

void get_active_port_rlms(struct pp_qos_dev *qdev, u32 phy,
			  u16 *rlms, u32 size, u32 *queues_num)
{
	struct rlm_container_metadata data = {0, rlms, size};

	if (!rlms)
		data.size = 0;

	*queues_num = post_order_travers_tree(qdev, phy,
					      node_active_queue_wrapper, NULL,
					      update_rlm_container, &data);
}

static int node_in_grp(const struct pp_qos_dev *qdev,
		const struct qos_node *node, void *data)
{
	return node_used(node) &&
		(node->shared_bandwidth_group == (uintptr_t)data);
}

void get_bw_grp_members_under_node(struct pp_qos_dev *qdev, unsigned int id,
		unsigned int phy, uint16_t *ids,
		unsigned int size, unsigned int *ids_num)
{
	struct ids_container_metadata data = {0, ids, size};

	if (ids == NULL)
		data.size = 0;
	*ids_num = post_order_travers_tree(qdev, phy, node_in_grp,
			(void *)(uintptr_t)id, update_ids_container, &data);
}

int update_predecessors(struct pp_qos_dev *qdev,
		struct qos_node *node, void *data)
{
	bool queue_port_changed = false;
	u32  queue_port = QOS_INVALID_PHY;
	u32  queue_id = PP_QOS_INVALID_ID;
	u32  phy = get_phy_from_node(qdev->nodes, node);

	if (node_queue(node)) {
		queue_id = get_id_from_phy(qdev->mapping, phy);
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

	create_update_preds_cmd(qdev, phy, queue_port_changed);
	return 1;
}

static int node_child_wrapper(const struct pp_qos_dev *qdev,
		const struct qos_node *node, void *data)
{
	return node_child(node);
}

void tree_update_predecessors(struct pp_qos_dev *qdev, unsigned int phy)
{
	post_order_travers_tree(qdev, phy, node_child_wrapper,
			NULL, update_predecessors, NULL);
}

#if 0
static int node_virtual_child_of(const struct pp_qos_dev *qdev,
		const struct qos_node *node, void *data)
{
	return node_child(node) && !node_internal(node) &&
		((uintptr_t)data == get_virtual_parent_phy(qdev->nodes, node));
}

static int add_bandwidth_share(struct pp_qos_dev *qdev,
		const struct qos_node *node,
		void *data)
{
	QOS_ASSERT(node_child(node), "Node is not a child\n");
	return node->child_prop.virt_bw_share;
}


/*
 * Return the sum of bandwidth share of all desendants
 * whose virtual parent is parent
 * Return 0 if parent is internal
 */
STATIC_UNLESS_TEST unsigned int get_virt_children_bandwidth_share(
		const struct pp_qos_dev *qdev,
		const struct qos_node *parent)
{
	int phy;
	int total;
	struct pp_qos_dev *_qdev;

	total = 0;
	if (node_parent(parent) && !node_internal(parent)) {
		_qdev = (struct pp_qos_dev *)qdev;
		phy = get_phy_from_node(qdev->nodes, parent);
		total = post_order_travers_tree(_qdev, phy,
				node_virtual_child_of,
				(void *)(uintptr_t)phy,
				add_bandwidth_share, NULL);
	}
	return total;
}
#endif

/*
 * Return the sum of bandwidth share of all direct children of parent
 */
STATIC_UNLESS_TEST unsigned int get_children_bandwidth_share(
		const struct pp_qos_dev *qdev,
		const struct qos_node *parent)
{
	unsigned int total;
	unsigned int num;
	const struct qos_node *child;

	total = 0;
	if (node_parent(parent)) {
		num = parent->parent_prop.num_of_children;
		if (num > 0)
			child = get_const_node_from_phy(qdev->nodes,
					parent->parent_prop.first_child_phy);
		for (; num; --num) {
			total += child->child_prop.virt_bw_share;
			++child;
		}
	}
	return total;
}

static int node_remove_wrapper(
		struct pp_qos_dev *qdev,
		struct qos_node *node, void *data)
{
	uint16_t phy;
	uint16_t id;
	int rc;

	phy = get_phy_from_node(qdev->nodes, node);
	id = get_id_from_phy(qdev->mapping, phy);
	rc = pp_queue_enqueue(qdev->queue, id);
	QOS_ASSERT(rc == 0, "Could not enqueue\n");
	return rc;
}

static int node_flush_wrapper(
		struct pp_qos_dev *qdev,
		struct qos_node *node, void *data)
{
	uint16_t phy;
	uint16_t id;
	int rc;

	phy = get_phy_from_node(qdev->nodes, node);
	id = get_id_from_phy(qdev->mapping, phy);
	rc = _pp_qos_queue_flush(qdev, id);
	QOS_ASSERT(rc == 0, "Could not enqueue\n");
	return rc;
}

static int node_modify_blocked_status(
		struct pp_qos_dev *qdev,
		struct qos_node *node, void *data)
{
	uint16_t phy;
	uint16_t id;
	unsigned int status;
	int rc;

	status = (unsigned int)(uintptr_t)data;
	phy = get_phy_from_node(qdev->nodes, node);
	id = get_id_from_phy(qdev->mapping, phy);
	if (status)
		rc = _pp_qos_queue_block(qdev, id);
	else
		rc = _pp_qos_queue_unblock(qdev, id);
	QOS_ASSERT(rc == 0, "Could not block queue\n");
	return rc;
}

static int node_used_wrapper(
		const struct pp_qos_dev *qdev,
		const struct qos_node *node, void *data)
{
	return node_used(node);
}

int tree_remove(struct pp_qos_dev *qdev, unsigned int phy)
{
	struct qos_node *node;
	unsigned int id;
	unsigned int tmp;
	int rc;

	pp_queue_reset(qdev->queue);
	rc = post_order_travers_tree(qdev, phy, node_used_wrapper, NULL,
			node_remove_wrapper, NULL);
	if (rc) {
		QOS_LOG("Error while trying to delete subtree with root %u\n",
				phy);
		return -EBUSY;
	}
	id = pp_queue_dequeue(qdev->queue);
	while (QOS_ID_VALID(id)) {
		tmp = get_phy_from_id(qdev->mapping, id);
		node =	get_node_from_phy(qdev->nodes, tmp);
		node_remove(qdev, node);
		id = pp_queue_dequeue(qdev->queue);
	}

	return 0;
}

int tree_flush(struct pp_qos_dev *qdev, unsigned int phy)
{
	int rc;

	rc = post_order_travers_tree(qdev, phy, node_queue_wrapper,
			NULL, node_flush_wrapper, NULL);
	if (rc) {
		QOS_LOG("Unexpected error while flush subtree root is %u\n",
				phy);
		return -EBUSY;
	}
	return 0;
}

int tree_modify_blocked_status(
		struct pp_qos_dev *qdev,
		unsigned int phy,
		unsigned int status)
{
	int rc;

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

#ifdef MAY_USE_IN_THE_FUTURE
static int shared_group_defined(
		const struct pp_qos_dev *qdev,
		unsigned int group_id)
{
	QOS_ASSERT(group_id <= QOS_MAX_SHARED_BANDWIDTH_GROUP,
			"illegal shared group %u\n",
		   group_id);
	return (qdev->groups[group_id].limit != QOS_NO_BANDWIDTH_LIMIT);
}
#endif

static int common_cfg_valid(
		const struct pp_qos_dev *qdev,
		const struct qos_node *node)
{
	unsigned int shared;

	if (!(node->bandwidth_limit == QOS_NO_BANDWIDTH_LIMIT ||
				node->bandwidth_limit <=
				QOS_MAX_BANDWIDTH_LIMIT)) {
		QOS_LOG("Invalid bandwidth limit %u\n", node->bandwidth_limit);
		return 0;
	}

	shared = node->shared_bandwidth_group;
	if (!(
				(shared == QOS_NO_SHARED_BANDWIDTH_GROUP) ||
				(shared <= QOS_MAX_SHARED_BANDWIDTH_GROUP &&
				qdev->groups[shared].used)
	     )) {
		QOS_LOG("Invalid shared bandwidth group %u\n",
				node->shared_bandwidth_group);
		return 0;
	}
	return 1;
}

/*
 * If new node
 *     node.child.parent.phy == virtual parent phy
 * Else
 *     node.child.parent.phy is either virtual parent phy (if user is changing
 *     parent) or actual parent (if user is not changing parent)
 */
static int child_cfg_valid(
		const struct pp_qos_dev *qdev,
		const struct qos_node *node,
		const struct qos_node *orig_node,
		unsigned int prev_virt_parent_phy)
{
	const struct qos_node *parent;
	unsigned int parent_phy;
	unsigned int cur_virt_parent_phy;

	QOS_ASSERT(node_child(node), "node is not a child\n");
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

	parent =  get_const_node_from_phy(qdev->nodes, parent_phy);
	if (!node_parent(parent)) {
		QOS_LOG_ERR("Node's parent %u is not a parent\n", parent_phy);
		return 0;
	}

	/* Find current virtual parent and bandwidth of its direct children */
	if (!node_internal(parent))
		cur_virt_parent_phy = parent_phy;
	else
		cur_virt_parent_phy = get_virtual_parent_phy(
				qdev->nodes,
				parent);

	if (node->child_prop.virt_bw_share  > 0x7F) {
		QOS_LOG_ERR("bw_share (%u) can't exceed 0x7F\n",
			    node->child_prop.virt_bw_share);
		return 0;
	}

	return 1;
}

static int parent_cfg_valid(
		const struct pp_qos_dev *qdev,
		const struct qos_node *node)
{
	unsigned int first;
	unsigned int num;

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
	if ((num > 0) &&
	    (first <= qdev->max_port || first > NUM_OF_NODES - 1)) {
		QOS_LOG_ERR("node has %u children but first (%u) is illegal\n",
			node->parent_prop.num_of_children, first);
		return 0;
	}

	return 1;
}

int node_cfg_valid(
		const struct pp_qos_dev *qdev,
		const struct qos_node *node,
		const struct qos_node *orig_node,
		unsigned int prev_virt_parent_phy)
{
	if (!common_cfg_valid(qdev, node))
		return 0;
	if (node_parent(node) && !parent_cfg_valid(qdev, node))
		return 0;
	if (node_child(node) &&
			!child_cfg_valid(qdev, node,
				orig_node, prev_virt_parent_phy))
		return 0;
	return 1;
}

unsigned int get_virtual_parent_phy(
		const struct pp_nodes *nodes,
		const struct qos_node *child)
{
	const struct qos_node *tmp;
	unsigned int phy;

	tmp = child;
	do {
		QOS_ASSERT(node_child(tmp), "Node is not a child\n");
		phy = tmp->child_prop.parent_phy;
		tmp = get_const_node_from_phy(nodes, phy);
	} while (node_internal(tmp));
	return phy;
}

int get_node_prop(const struct pp_qos_dev *qdev,
		  const struct qos_node *node,
		  struct pp_qos_common_node_properties *common,
		  struct pp_qos_parent_node_properties *parent,
		  struct pp_qos_child_node_properties *child)
{

	QOS_ASSERT(node_used(node), "Node is not used\n");

	if (common) {
		common->bandwidth_limit = node->bandwidth_limit;
		common->shared_bandwidth_group = node->shared_bandwidth_group;
	}

	if (parent) {
		parent->arbitration = node->parent_prop.arbitration;
		parent->best_effort_enable =
			!!QOS_BITS_IS_SET(node->flags,
					QOS_NODE_FLAGS_PARENT_BEST_EFFORT_ENABLE
					);
	}

	if (child) {
		/*
		 * Internal schedulers are transparent to clients. Clients see
		 * only virtual parent - the first parent in hierarchy which is
		 * not internal scheduler
		 */
		child->parent = get_id_from_phy(qdev->mapping,
				get_virtual_parent_phy(qdev->nodes, node));
		child->priority = node->child_prop.priority;
		child->wrr_weight = node->child_prop.virt_bw_share;
	}

	return 0;
}

static int set_common(struct pp_qos_dev *qdev,
		      struct qos_node *node,
		      const struct pp_qos_common_node_properties *common,
		      uint32_t *modified)
{
	unsigned int grp_id;

	if (node->bandwidth_limit != common->bandwidth_limit) {
		node->bandwidth_limit = common->bandwidth_limit;
		QOS_BITS_SET(*modified, QOS_MODIFIED_BANDWIDTH_LIMIT);
	}

	grp_id = common->shared_bandwidth_group;
	if (grp_id != node->shared_bandwidth_group) {
		node->shared_bandwidth_group = grp_id;
		QOS_BITS_SET(*modified, QOS_MODIFIED_SHARED_GROUP_ID);
	}
	return 0;
}

static int set_parent(struct pp_qos_dev *qdev,
		      struct qos_node *node,
		      const struct pp_qos_parent_node_properties *parent,
		      uint32_t *modified)
{
	if (node->parent_prop.arbitration != parent->arbitration) {
		node->parent_prop.arbitration = parent->arbitration;
		QOS_BITS_SET(*modified, QOS_MODIFIED_ARBITRATION);
	}

	if (!!(parent->best_effort_enable) !=
			!!QOS_BITS_IS_SET(node->flags,
				QOS_NODE_FLAGS_PARENT_BEST_EFFORT_ENABLE)) {
		QOS_BITS_TOGGLE(node->flags,
				QOS_NODE_FLAGS_PARENT_BEST_EFFORT_ENABLE);
		QOS_BITS_SET(*modified, QOS_MODIFIED_BEST_EFFORT);
	}
	return 0;
}


static int set_child(struct pp_qos_dev *qdev,
		     struct qos_node *node,
		     const struct pp_qos_child_node_properties *child,
		     uint32_t *modified)
{
	unsigned int conf_parent_phy;
	unsigned int virt_parent_phy;
	struct qos_node *parent;

	/* Equals to virtual parent phy since client is not aware of internal
	 * schedulers, they are transparent to him
	 */
	conf_parent_phy = get_phy_from_id(qdev->mapping, child->parent);
	virt_parent_phy = node->child_prop.parent_phy;

	if (QOS_PHY_VALID(virt_parent_phy))
		virt_parent_phy = get_virtual_parent_phy(qdev->nodes, node);

	if (virt_parent_phy != conf_parent_phy) {
		node->child_prop.parent_phy = conf_parent_phy;
		QOS_BITS_SET(*modified, QOS_MODIFIED_PARENT);
	}

	if (node->child_prop.virt_bw_share != child->wrr_weight) {
		node->child_prop.virt_bw_share = child->wrr_weight;
		QOS_BITS_SET(*modified, QOS_MODIFIED_BW_WEIGHT);
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
			QOS_LOG_ERR("Trying to set priority, INVALID parent phy\n");
		}
	}

	return 0;
}

int set_node_prop(struct pp_qos_dev *qdev,
		  struct qos_node *node,
		  const struct pp_qos_common_node_properties *common,
		  const struct pp_qos_parent_node_properties *parent,
		  const struct pp_qos_child_node_properties *child,
		  uint32_t *modified)
{
	int rc;

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
	if (child)
		return set_child(qdev, node, child, modified);

	return 0;
}

static void node_parent_init(struct qos_node *node)
{
	node->parent_prop.first_child_phy = QOS_INVALID_PHY;
	node->parent_prop.num_of_children = 0;
	node->parent_prop.arbitration = PP_QOS_ARBITRATION_WRR;
}

static void node_child_init(struct qos_node *node)
{
	node->child_prop.parent_phy = QOS_INVALID_PHY;
	node->child_prop.virt_bw_share = 0;
	node->child_prop.priority = 0;
}

static void node_common_init(struct qos_node *node)
{
	node->flags = 0;
	node->bandwidth_limit = QOS_NO_BANDWIDTH_LIMIT;
	node->shared_bandwidth_group = QOS_NO_SHARED_BANDWIDTH_GROUP;
}

void node_init(const struct pp_qos_dev *qdev,
	       struct qos_node *node,
	       unsigned int common,
	       unsigned int parent,
	       unsigned int child)
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
static unsigned int calculate_new_child_location(
		struct pp_qos_dev *qdev,
		struct qos_node *parent,
		unsigned int priority)
{
	unsigned int phy;
	unsigned int num;
	struct qos_node *child;

	QOS_ASSERT(node_parent(parent), "node is not a parent\n");
	if (parent->parent_prop.arbitration == PP_QOS_ARBITRATION_WRR) {
		phy = parent->parent_prop.first_child_phy +
			parent->parent_prop.num_of_children;
	} else {
		num = parent->parent_prop.num_of_children;
		phy = parent->parent_prop.first_child_phy;
		child = get_node_from_phy(qdev->nodes, phy);
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
static unsigned int children_on_non_full_octet(
		struct pp_qos_dev *qdev,
		struct qos_node *parent,
		unsigned int usage,
		unsigned int child_priority)
{
	unsigned int phy;

	phy = calculate_new_child_location(qdev, parent, child_priority);

	if (node_sched(parent)) {
		parent->data.sched.new_child_phy = phy;
		parent->data.sched.new_child_skipped = 0;
	}

	octet_nodes_shift(qdev, phy, usage - octet_phy_offset(phy), 1);
	return phy;
}

static unsigned int children_on_non_full_octet_wrapper(
		struct pp_qos_dev *qdev,
		struct qos_node *parent,
		unsigned int child_priority)
{
	unsigned int usage;
	unsigned int octet;

	QOS_ASSERT(node_parent(parent),
			"Node %u is not a parent\n",
			get_phy_from_node(qdev->nodes, parent));
	octet = octet_of_phy(parent->parent_prop.first_child_phy);
	usage = octet_get_use_count(qdev->octets, octet);
	QOS_ASSERT(usage < 8, "Octet is full\n");
	return children_on_non_full_octet(qdev, parent, usage, child_priority);
}

/**
 * has_less_than_8_children_on_full_octet() - Allocate a phy for a new child
 * are held on a full octet.
 * @qdev:
 * @parent:
 * @children_octet:
 * @child_priority:
 * Return: New allocated phy
 */
static unsigned int has_less_than_8_children_on_full_octet(
		struct pp_qos_dev *qdev,
		struct qos_node *parent,
		uint8_t children_octet,
		unsigned int child_priority)
{
	struct qos_node *min_parent;
	unsigned int octet;
	unsigned int phy;
	unsigned int num_of_required_entries;
	unsigned int num_of_nodes_to_move;
	unsigned int dst_phy;
	unsigned int src_phy;
	unsigned int parent_id;

	parent_id = get_id_from_phy(qdev->mapping,
			get_phy_from_node(qdev->nodes, parent));
	QOS_ASSERT(octet_get_use_count(qdev->octets, children_octet) == 8,
			"Octet %d is not full\n", children_octet);
	min_parent = octet_get_min_sibling_group(qdev, children_octet,
			parent, &num_of_required_entries);
	QOS_ASSERT(min_parent != NULL, "Can't find min_parent for octet %d\n",
			children_octet);
	if (min_parent == NULL)
		return QOS_INVALID_PHY;

	octet = octet_get_with_at_least_free_entries(
			qdev->octets,
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

	/*
	 * shift original octet if necessary i.e. if last moved
	 * node was not last node on octet
	 */
	src_phy += num_of_nodes_to_move;
	if (octet_phy_offset(src_phy - 1) != 7)
		octet_nodes_shift(qdev, src_phy,
				8 - octet_phy_offset(src_phy),
				-num_of_nodes_to_move);

	//min_parent->parent_prop.first_child_phy = dst_phy;

	parent = get_node_from_phy(qdev->nodes,
			get_phy_from_id(qdev->mapping, parent_id));
	phy = children_on_non_full_octet_wrapper(qdev, parent, child_priority);
	return phy;
}

/**
 * phy_alloc_parent_has_less_than_8_children() - Allocate a phy
 * for a new child whose parent has less than 8 children
 * @qdev:
 * @parent:
 * @child_priority:
 * Return:
 */
static unsigned int phy_alloc_parent_has_less_than_8_children(
		struct pp_qos_dev *qdev,
		struct qos_node *parent,
		unsigned int child_priority)
{
	unsigned int phy;
	unsigned int octet;
	unsigned int usage;
	unsigned int parent_id;

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
		if (usage < 8)
			phy = children_on_non_full_octet(
					qdev,
					parent,
					usage,
					child_priority);
		else
			phy = has_less_than_8_children_on_full_octet(
					qdev,
					parent,
					octet,
					child_priority);
	}

	parent = get_node_from_phy(qdev->nodes,
				   get_phy_from_id(qdev->mapping, parent_id));

	if (QOS_PHY_VALID(phy)) {
		link_with_parent(qdev, phy, parent_id);
		if (node_sched(parent) &&
		    QOS_PHY_VALID(parent->data.sched.new_child_phy)) {
			QOS_LOG_DEBUG("Sched %u(%u). phy %u. skipped %u\n",
				      parent_id,
				      get_phy_from_node(qdev->nodes, parent),
				      phy,
				      parent->data.sched.new_child_skipped);

			/* We must update preds of the skipped child */
			if (parent->data.sched.new_child_skipped)
				tree_update_predecessors(qdev, phy);

			/* Remove new child listing from sched db */
			parent->data.sched.new_child_phy = QOS_UNKNOWN_PHY;
			parent->data.sched.new_child_skipped = 0;
		}
	}
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
static unsigned int create_internal_scheduler_on_node(
		struct pp_qos_dev *qdev,
		struct qos_node *node)
{
	struct qos_node *new_node;
	unsigned int octet;
	unsigned int new_phy;
	unsigned int phy;
	unsigned int id;
	uint32_t modified;
	struct pp_qos_sched_conf conf;

	octet = octet_get_with_at_least_free_entries(qdev->octets, 8);
	if (!QOS_OCTET_VALID(octet)) {
		octet = octet_get_with_at_least_free_entries(qdev->octets, 2);
		if (!QOS_OCTET_VALID(octet)) {
			QOS_LOG("could not find free octet\n");
			return  QOS_INVALID_PHY;
		}
	}

	/*
	 * The assertion on nodes_move especially the one that states
	 * that if a child is moved to another octet than its siblings must
	 * move also, prevents using move in this clone scenario
	 */

	/* find a place for a new node */
	new_phy = octet * 8 + octet_get_use_count(qdev->octets, octet);
	QOS_ASSERT(QOS_PHY_VALID(new_phy + 1),
			"%u is not a valid phy\n", new_phy + 1);
	new_node = get_node_from_phy(qdev->nodes, new_phy);
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
	QOS_BITS_SET(node->flags,
			QOS_NODE_FLAGS_USED | QOS_NODE_FLAGS_INTERNAL);
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
			QOS_MODIFIED_PARENT | QOS_MODIFIED_ARBITRATION |
			QOS_MODIFIED_BEST_EFFORT);

	create_set_sched_cmd(qdev, &conf, phy,
			node->child_prop.parent_phy, modified);

	/* The new node will be put right after the moved node */
	++new_phy;
	link_with_parent(qdev, new_phy, id);

	tree_update_predecessors(qdev, phy);

	return new_phy;
}

/**
 * phy_alloc_for_new_child_parent_has_8_children() - Allocate a phy for a new
 * child whose parent has 8 children
 * @qdev:
 * @parent:
 * Return:
 */
static unsigned int phy_alloc_for_new_child_parent_has_8_children(
		struct pp_qos_dev *qdev,
		struct qos_node *parent)
{
	unsigned int first_phy;
	struct qos_node *child;
	struct qos_node *last_child;
	unsigned int phy;
	unsigned int parent_phy;
	unsigned int i;

	phy = QOS_INVALID_PHY;

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
			return
				phy_alloc_parent_has_less_than_8_children(
					qdev,
					child,
					QOS_INVALID_PRIORITY);
		}
	}

	child = get_node_from_phy(qdev->nodes, first_phy);
	while (child < last_child && node_internal(child))
		++child;
	if (!node_internal(child))
		return create_internal_scheduler_on_node(qdev, child);

	/*
	 * If we reach this point all children are full internal schedulers
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
		phy = phy_alloc_for_new_child_parent_has_8_children(
				qdev,
				get_node_from_phy(qdev->nodes, parent_phy));
		if (QOS_PHY_VALID(phy))
			return phy;
	}

	return phy;
}

unsigned int phy_alloc_by_parent(
		struct pp_qos_dev *qdev,
		struct qos_node *parent,
		unsigned int child_priority)
{
	unsigned int phy;

	if (parent->parent_prop.num_of_children < 8) {
		phy =  phy_alloc_parent_has_less_than_8_children(
				qdev,
				parent,
				child_priority);
	} else if (parent->parent_prop.arbitration == PP_QOS_ARBITRATION_WSP) {
		QOS_LOG_ERR("WSP parent %u already has 8 children\n",
				get_phy_from_node(qdev->nodes, parent));
		phy = QOS_INVALID_PHY;
	} else {
		pp_queue_reset(qdev->queue);
		phy =  phy_alloc_for_new_child_parent_has_8_children(qdev,
				parent);
	}
	return phy;
}

int check_sync_with_fw(struct pp_qos_dev *qdev)
{
	unsigned int i;
	unsigned int used;
	struct qos_hw_info hw_info = {0};
	unsigned int id;
	int rc;
	const struct qos_node *node;
	struct pp_pool *pool;
	struct pp_qos_node_info info;

	rc = 0;
	pool = pp_pool_init(NUM_OF_NODES, QOS_INVALID_ID);
	if (pool == NULL) {
		QOS_LOG_ERR("Can't create pool for firmware sync check\n");
		return -1;
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

	create_get_sys_info_cmd(qdev, qdev->hwconf.fw_stat, &hw_info);
	update_cmd_id(&qdev->drvcmds);
	transmit_cmds(qdev);

	if (hw_info.num_used != used) {
		QOS_LOG_ERR("Driver's DB has %u used nodes, "
			    "while firmware reports %u\n",
			    used, hw_info.num_used);
		rc = -1;
	}

	id = pp_pool_get(pool);
	while (QOS_ID_VALID(id)) {
		if (pp_qos_get_node_info(qdev, id, &info)) {
			QOS_LOG_ERR("Error info on %u\n", id);
			rc = -1;
			break;
		}
		id = pp_pool_get(pool);
	}

	pp_pool_clean(pool);

	return rc;
}

/* Return 1 if device is not in assert and was initialized */
int qos_device_ready(const struct pp_qos_dev *qdev)
{
	if (PP_QOS_DEVICE_IS_ASSERT(qdev)) {
		QOS_LOG_CRIT("Qos fatal event occurred\n");
		return 0;
	}

	if (!qdev->initialized) {
		QOS_LOG_ERR("Device was not initialized\n");
		return 0;
	}

	return 1;
}

/*
 * Return new phy of a node whose original node was
 * orig_node and now has a new priority
 */
static unsigned int find_child_position_by_priority(
			struct pp_qos_dev *qdev,
			struct qos_node *parent,
			struct qos_node *orig_node,
			unsigned int priority)
{
	struct qos_node *first;
	struct qos_node *child;
	struct qos_node *last;
	unsigned int pos;
	unsigned int num;

	QOS_ASSERT(parent->parent_prop.arbitration == PP_QOS_ARBITRATION_WSP,
			"Parent is not wsp\n");

	num = parent->parent_prop.num_of_children;
	QOS_ASSERT(num, "Function can't be used on parent with no children\n");

	pos = parent->parent_prop.first_child_phy;

	first = get_node_from_phy(qdev->nodes, pos);
	last = get_node_from_phy(qdev->nodes, pos + num - 1);

	for (child = first; child <= last; ++child) {
		if (child == orig_node)
			continue;
		if (priority <= child->child_prop.priority)
			break;
		++pos;
	}
	return pos;
}

/*
 * Update wsp child position based on its position
 * child - original child node before moving (stored at db)
 * parent - parent node of child
 * node_src - copy of child node (stored at stack)
 */
void update_children_position(
			struct pp_qos_dev *qdev,
			struct qos_node *child,
			struct qos_node *parent,
			struct qos_node *node_src)
{
	unsigned int old_phy;
	unsigned int new_phy;
	unsigned int id;
	unsigned int dst_port;
	struct qos_node *node;
	unsigned int first;
	unsigned int cnt;

	old_phy = get_phy_from_node(qdev->nodes, child);

	new_phy = find_child_position_by_priority(qdev,
			parent,
			child,
			node_src->child_prop.priority);

	if (new_phy == old_phy) {
		node = get_node_from_phy(qdev->nodes, new_phy);
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
	memcpy(node, node_src, sizeof(struct qos_node));
	QOS_BITS_CLEAR(node->flags, QOS_NODE_FLAGS_USED);
	nodes_modify_used_status(qdev, new_phy, 1, 1);
	parent->parent_prop.first_child_phy = first;
	parent->parent_prop.num_of_children = cnt;

	create_parent_change_cmd(qdev,
			get_phy_from_node(qdev->nodes, parent));

	if (node_parent(node))
		tree_update_predecessors(qdev, new_phy);
}

#define ARB_STR(a)                             \
	((a) == PP_QOS_ARBITRATION_WSP ? "WSP" : \
	 (a) == PP_QOS_ARBITRATION_WRR ? "WRR" :  \
	 (a) == PP_QOS_ARBITRATION_WFQ ? "WFQ" :  \
	 "Unknown")

#define QOS_DBG_PRINT(s, msg, ...) (s ? seq_printf(s, msg, ##__VA_ARGS__) : \
				    pr_info(msg, ##__VA_ARGS__))

void __dbg_dump_subtree(struct pp_qos_dev *qdev,
			struct qos_node *node,
			u32 depth,
			struct seq_file *s)
{
	u32 idx, tab_idx, n = 0;
	u32 child_phy, node_id;
	char *tabs_str;
	char *indent_str;
	bool last_child;
	struct qos_node *child;

	if (depth > 6) {
		pr_err("Maximum depth of 6 exceeded\n");
		return;
	}

	tabs_str = kzalloc(PP_QOS_DBG_MAX_INPUT, GFP_KERNEL);
	if (!tabs_str) {
		QOS_LOG_ERR("Cannot allocate temp string\n");
		return;
	}

	indent_str = kzalloc(PP_QOS_DBG_MAX_INPUT, GFP_KERNEL);
	if (!indent_str) {
		QOS_LOG_ERR("Cannot allocate temp string\n");
		kfree(tabs_str);
		return;
	}

	tabs_str[0] = '\0';
	for (tab_idx = 0 ; tab_idx < depth ; tab_idx++)
		n += snprintf(tabs_str + n, PP_QOS_DBG_MAX_INPUT - n, "|\t");

	for (idx = 0; idx < node->parent_prop.num_of_children ; ++idx) {
		last_child = (idx == (node->parent_prop.num_of_children - 1));
		child_phy = node->parent_prop.first_child_phy + idx;
		node_id = get_id_from_phy(qdev->mapping, child_phy);
		child = get_node_from_phy(qdev->nodes, child_phy);

		if (last_child)
			snprintf(indent_str, PP_QOS_DBG_MAX_INPUT, "%s'-- ",
				 tabs_str);
		else
			snprintf(indent_str, PP_QOS_DBG_MAX_INPUT, "%s|-- ",
				 tabs_str);

		if (node_sched(child)) {
			QOS_DBG_PRINT(s, "%sSched-%u(%u)-%s\n",
				   indent_str, node_id, child_phy,
				   ARB_STR(child->parent_prop.arbitration));
			__dbg_dump_subtree(qdev, child, depth + 1, s);
		} else if (node_queue(child)) {
			QOS_DBG_PRINT(s, "%sQueue-%u(%u)-rlm-%u\n",
				   indent_str, node_id, child_phy,
				   child->data.queue.rlm);
		}
	}

	kfree(indent_str);
	kfree(tabs_str);
}

void qos_dbg_tree_show(struct pp_qos_dev *qdev, struct seq_file *s)
{
	struct qos_node *node;
	u32 node_id, node_phy;

	/* Iterate through all port nodes */
	for (node_phy = 0; node_phy < NUM_OF_NODES; ++node_phy) {
		node = get_node_from_phy(qdev->nodes, node_phy);
		node_id = get_id_from_phy(qdev->mapping, node_phy);
		if (node_port(node)) {
			QOS_DBG_PRINT(s, "|-- Port-%u(%u)-%s\n",
				      node_id,
				      get_phy_from_id(qdev->mapping, node_id),
				      ARB_STR(node->parent_prop.arbitration));
			__dbg_dump_subtree(qdev, node, 1, s);
		}
	}
}

#define NUM_QUEUES_ON_QUERY (32U)
#define NUM_OF_TRIES (20U)
struct queue_stat_info {
	uint32_t qid;
	struct queue_stats_s qstat;
};

int qos_dbg_qstat_show(struct pp_qos_dev *qdev, struct seq_file *s)
{
	unsigned int i;
	struct queue_stat_info *stat;
	unsigned int tries;
	uint32_t *dst;
	unsigned int j;
	uint32_t val;
	uint32_t num;
	volatile uint32_t *pos;

	if (!qos_device_ready(qdev)) {
		seq_puts(s, "Device is not ready\n");
		return 0;
	}
	QOS_DBG_PRINT(s, "Queue\t\tQocc(p)\t\tAccept(p)\tDrop(p)"
			 "\t\tRed dropped(p)\n");

	dst = (uint32_t *)(qdev->fwcom.cmdbuf);
	*dst++ = qos_u32_to_uc(
			UC_QOS_CMD_GET_ACTIVE_QUEUES_STATS);
	pos = dst;
	*dst++ = qos_u32_to_uc(UC_CMD_FLAG_IMMEDIATE);
	*dst++ = qos_u32_to_uc(3);

	for (i = 0; i < NUM_OF_QUEUES; i += NUM_QUEUES_ON_QUERY) {
		*pos = qos_u32_to_uc(UC_CMD_FLAG_IMMEDIATE);
		dst = (uint32_t *)(qdev->fwcom.cmdbuf) + 3;
		*dst++ = qos_u32_to_uc(i);
		*dst++ = qos_u32_to_uc(i + NUM_QUEUES_ON_QUERY - 1);
		*dst++ = qos_u32_to_uc(qdev->hwconf.fw_stat);
		signal_uc(qdev);
		val = qos_u32_from_uc(*pos);
		tries = 0;
		while ((
				val &
				(UC_CMD_FLAG_UC_DONE |
				 UC_CMD_FLAG_UC_ERROR))
				== 0) {
			qos_sleep(10);
			tries++;
			if (tries == NUM_OF_TRIES) {
				QOS_DBG_PRINT(s, "firmware not responding\n");
				return 0;
			}
			val = qos_u32_from_uc(*pos);
		}
		if (val & UC_CMD_FLAG_UC_ERROR) {
			QOS_DBG_PRINT(s, "firmware signaled error !!!!\n");
			return 0;
		}
		stat = (struct queue_stat_info *)(qdev->stat + 4);
		num =   *((uint32_t *)(qdev->stat));
		for (j = 0; j < num; ++j) {
			QOS_DBG_PRINT(s, "%u\t\t%u\t\t%u\t\t%u\t\t%u\n",
				      stat->qid,
				      stat->qstat.queue_size_entries,
				      stat->qstat.total_accepts,
				      stat->qstat.total_drops,
				      stat->qstat.total_red_dropped);
			++stat;
		}
	}
	return 0;
}
