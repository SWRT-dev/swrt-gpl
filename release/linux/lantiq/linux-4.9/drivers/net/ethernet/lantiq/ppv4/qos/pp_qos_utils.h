/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2017 Intel Corporation.
 *
 *  This program is free software; you can redistribute it and/or modify
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
#ifndef PP_QOS_UTILS_H
#define PP_QOS_UTILS_H

#include "pp_qos_common.h"
#include "pp_qos_drv.h"

/******************************************************************************/
/*                              CONF                                          */
/******************************************************************************/
#define PPV4_QOS_DRV_VER "1.0.4-fl"

#define NUM_OF_NODES	2048U
#define NUM_OF_QUEUES	256U
#define NUM_OF_OCTETS ((NUM_OF_NODES / 8) - 1)
#define PP_QOS_TMP_NODE 2047U
#define QOS_MAX_SHARED_BW_GRP QOS_MAX_SHARED_BANDWIDTH_GROUP

#define MAX_QOS_INSTANCES 1U
#define PP_QOS_DBG_MAX_INPUT (1024)

/* for calculating number of ddr pages for qm from resource size */
#define PPV4_QOS_DESC_SIZE (16U)
#define PPV4_QOS_DESC_IN_PAGE (16U)

#define PPV4_QOS_LOGGER_MSG_SIZE (128U)
#define PPV4_QOS_LOGGER_BUF_SIZE (PPV4_QOS_LOGGER_MSG_SIZE * 4096)
#define PPV4_QOS_STAT_SIZE	(1024U)

#define PPV4_QOS_IVT_START (0x48000U)

#define FIRMWARE_FILE "ppv4-qos-fw.elf"
/******************************************************************************/
/*                              DEFINES                                       */
/******************************************************************************/

#define QOS_INVALID_OCTET   0xFFFFU
#define QOS_INVALID_ID      0xFFFFU
#define QOS_INVALID_RLM     0xFFFFU
#define QOS_INVALID_PHY     0xFFFFU
#define QOS_UNKNOWN_PHY     0xFFFEU

#define QOS_OCTET_VALID(octet)	((octet) < NUM_OF_OCTETS)
#define QOS_ID_VALID(id)	((id) < NUM_OF_NODES)
#define QOS_PHY_VALID(phy)	((phy) < NUM_OF_NODES)
#define QOS_PHY_UNKNOWN(phy)	((phy) == QOS_UNKNOWN_PHY)
#define QOS_BW_GRP_VALID(id)	((id) && ((id) <= QOS_MAX_SHARED_BW_GRP))

#define QOS_MODIFIED_NODE_TYPE			BIT(0)
#define QOS_MODIFIED_BANDWIDTH_LIMIT		BIT(2)
#define QOS_MODIFIED_SHARED_GROUP_ID		BIT(3)
#define QOS_MODIFIED_ARBITRATION		BIT(5)
#define QOS_MODIFIED_BEST_EFFORT		BIT(6)
#define QOS_MODIFIED_PRIORITY			BIT(7)
#define QOS_MODIFIED_BW_WEIGHT			BIT(8)
#define QOS_MODIFIED_PARENT			BIT(9)
#define QOS_MODIFIED_RING_ADDRESS		BIT(10)
#define QOS_MODIFIED_RING_SIZE			BIT(11)
#define QOS_MODIFIED_CREDIT			BIT(12)
#define QOS_MODIFIED_DISABLE			BIT(13)
#define QOS_MODIFIED_MAX_BURST			BIT(15)
#define QOS_MODIFIED_BLOCKED			BIT(16)
#define QOS_MODIFIED_WRED_ENABLE		BIT(17)
#define QOS_MODIFIED_WRED_MIN_GREEN		BIT(18)
#define QOS_MODIFIED_WRED_MAX_GREEN		BIT(19)
#define QOS_MODIFIED_WRED_SLOPE_GREEN		BIT(20)
#define QOS_MODIFIED_WRED_MIN_YELLOW		BIT(21)
#define QOS_MODIFIED_WRED_MAX_YELLOW		BIT(22)
#define QOS_MODIFIED_WRED_SLOPE_YELLOW		BIT(23)
#define QOS_MODIFIED_WRED_MAX_ALLOWED		BIT(24)
#define QOS_MODIFIED_WRED_MIN_GUARANTEED	BIT(25)
#define QOS_MODIFIED_RLM			BIT(26)
#define QOS_MODIFIED_WRED_FIXED_DROP_PROB_ENABLE	BIT(27)
#define QOS_MODIFIED_WRED_FIXED_GREEN_PROB	BIT(28)
#define QOS_MODIFIED_WRED_FIXED_YELLOW_PROB	BIT(29)
#define QOS_MODIFIED_PORT_GREEN_THRESHOLD	BIT(30)
#define QOS_MODIFIED_PORT_YELLOW_THRESHOLD	BIT(31)

#define MAX_MOVING_NODES 8

/******************************************************************************/
/*                              TYPES                                         */
/******************************************************************************/

#define NODE_TYPE(OP)		\
	OP(TYPE_UNKNOWN)	\
	OP(TYPE_PORT)		\
	OP(TYPE_SCHED)		\
	OP(TYPE_QUEUE)

enum node_type {
	NODE_TYPE(GEN_ENUM)
};

struct child_node_properties {
	uint16_t	parent_phy;
	uint8_t		priority;
	uint8_t		virt_bw_share;
	uint8_t		bw_weight;
};

struct parent_node_properties {
	enum pp_qos_arbitration		arbitration;
	uint16_t			first_child_phy;
	uint8_t				num_of_children;
};

struct qos_node {
	enum node_type			type;
	uint32_t			bandwidth_limit;
	uint16_t			shared_bandwidth_group;
	struct child_node_properties	child_prop;
	struct parent_node_properties	parent_prop;
	union _data {
		struct {
			void		*ring_address;
			size_t		ring_size;
			unsigned int	credit;
			int		disable;
			unsigned int	green_threshold;
			unsigned int	yellow_threshold;
		} port;
		struct {
			uint16_t	new_child_phy;
			uint8_t		new_child_skipped;
		} sched;
		struct _queue {
			uint32_t	green_min;
			uint32_t	green_max;
			uint32_t	yellow_min;
			uint32_t	yellow_max;
			uint32_t	max_allowed;
			uint32_t	min_guaranteed;
			uint32_t	fixed_green_prob;
			uint32_t	fixed_yellow_prob;
			uint16_t	max_burst;
			uint16_t	rlm;
			uint8_t		green_slope;
			uint8_t		yellow_slope;
			uint16_t	alias_slave_id;
			uint16_t	alias_master_id;
			uint16_t	port_phy;
		} queue;
	} data;

#define QOS_NODE_FLAGS_USED					BIT(0)
#define QOS_NODE_FLAGS_INTERNAL					BIT(1)
#define QOS_NODE_FLAGS_PARENT_BEST_EFFORT_ENABLE		BIT(4)
#define QOS_NODE_FLAGS_PORT_PACKET_CREDIT_ENABLE		BIT(7)
#define QOS_NODE_FLAGS_PORT_BYTE_CREDIT_ENABLE			BIT(8)
#define QOS_NODE_FLAGS_QUEUE_WRED_ENABLE			BIT(10)
#define QOS_NODE_FLAGS_QUEUE_BLOCKED				BIT(11)
#define QOS_NODE_FLAGS_QUEUE_WRED_FIXED_DROP_PROB_ENABLE	BIT(12)
	uint16_t	flags;
};


struct shared_bandwidth_group {
	uint32_t	limit;
	uint8_t		used;
	uint8_t		reserved;
};

struct qos_hw_info {
	uint32_t	num_used;
	uint32_t	infinite_loop_error_occurred;
	uint32_t	bwl_update_error_occurred;
	uint32_t	quanta;
};

struct driver_cmds {
	struct cmd_queue	*cmdq;
	struct cmd_queue	*pendq;
	unsigned int		cmd_id;
	unsigned int		cmd_fw_id;
};

/* Communication to/from FW */
struct fw_com {
	void		*cmdbuf;
	size_t		cmdbuf_sz;
	uint32_t	*mems;
	size_t		mems_size;
	void __iomem	*mbx_to_uc;
	void __iomem	*mbx_from_uc;
	unsigned int	irqline;
};

/* HW configuration */
struct hw_conf {
	unsigned int	wred_total_avail_resources;
	unsigned int	wred_prioritize_pop;
	unsigned int	wred_const_p;
	unsigned int	wred_max_q_size;
	unsigned int	qm_ddr_start;
	void*		qm_ddr_start_virt;
	unsigned int	qm_num_pages;
	unsigned int	fw_logger_start;
	unsigned int	fw_stat;
	unsigned int	qos_clock; /* In MHZ. */
	void*		max_allowed_ddr_virt;
	dma_addr_t	max_allowed_ddr_phys;
	unsigned int	*bwl_ddr_virt;
	dma_addr_t	bwl_ddr_phys;
	unsigned int	*sbwl_ddr_virt;
	dma_addr_t	sbwl_ddr_phys;
};

struct fw_ver {
	unsigned int major;
	unsigned int minor;
	unsigned int build;
};

struct pp_qos_dev {
	LOCK				lock;
	int				initialized;
	unsigned int			max_port;
	unsigned int			reserved_ports[QOS_MAX_PORTS];
	struct shared_bandwidth_group	groups[QOS_MAX_SHARED_BW_GRP + 1];
	struct pp_pool			*ids;
	struct pp_pool			*rlms;
	struct pp_pool			*portsphys;
	struct pp_nodes			*nodes;
	struct pp_mapping		*mapping;
	struct pp_octets		*octets;
	struct pp_queue			*queue;
	struct driver_cmds		drvcmds;
	struct fw_com			fwcom;
	struct fw_ver			fwver;
	void				*fwbuf;
	struct hw_conf			hwconf;
	void				*pdev;
	void				*stat;
#define PP_QOS_FLAGS_ASSERT		0x1U
	unsigned int			flags;
};

#define PP_QOS_DEVICE_IS_ASSERT(qdev)\
	(QOS_BITS_IS_SET((qdev)->flags, PP_QOS_FLAGS_ASSERT))

/* Info from platform statically or device tree */
struct ppv4_qos_platform_data  {
	int		id;
	unsigned int	max_port;
	unsigned int	wred_prioritize_pop;
	unsigned int	qm_ddr_start;
	void		*qm_ddr_start_virt;
	unsigned int	qm_num_pages;
	unsigned int	fw_logger_start;
	unsigned int	fw_stat;
	unsigned int	qos_clock; /* In MHZ. */
	void		*max_allowed_ddr_virt;
	dma_addr_t	max_allowed_ddr_phys;
	size_t		max_allowed_sz;
	void		*bwl_ddr_virt;
	dma_addr_t	bwl_ddr_phys;
	size_t		bwl_sz;
	void		*sbwl_ddr_virt;
	dma_addr_t	sbwl_ddr_phys;
	size_t		sbwl_sz;
};

/* Info needed to create descriptor */
struct qos_dev_init_info {
	struct ppv4_qos_platform_data pl_data;
	struct fw_com fwcom;
};


/******************************************************************************/
/*                              UTILS                                         */
/******************************************************************************/
#define QOS_LOG(format, arg...) QOS_LOG_DEBUG(format, ##arg)
void stop_run(void);
void resume_run(void);
void qos_dbg_tree_show(struct pp_qos_dev *qdev, struct seq_file *s);
int qos_dbg_qstat_show(struct pp_qos_dev *qdev, struct seq_file *s);

#ifdef __KERNEL__
#define QOS_ASSERT(condition, format... )				\
if (WARN(!(condition), format))						\
	stop_run();
#else
#define QOS_ASSERT(condition, format, arg...)				\
do {									\
	if (!(condition)) {						\
		QOS_LOG_CRIT("!!! Assertion failed !!! on %s:%d: " format,\
		__func__, __LINE__, ##arg);				\
		stop_run();						\
	}								\
} while (0)
#endif

#define QOS_BITS_SET(flags, bits)	((flags) |= (bits))
#define QOS_BITS_CLEAR(flags, bits)	((flags) &= ~(bits))
#define QOS_BITS_TOGGLE(flags, bits)	((flags) ^= (bits))
#define QOS_BITS_IS_SET(flags, bits)	((flags) & (bits))


/*
 * Phy octet mapping
 */
static inline int octet_of_phy(unsigned int phy)
{
	QOS_ASSERT(QOS_PHY_VALID(phy), "Invalid phy %u\n", phy);
	return (phy >> 3);
}

/*
 * Offset of phy on octet (0..7)
 */
static inline int octet_phy_offset(unsigned int phy)
{
	QOS_ASSERT(QOS_PHY_VALID(phy), "Invalid phy %u\n", phy);
	return (phy & 0x7);
}

/*
 * Return true iff both phys are on same octet
 */
static inline int same_octet(unsigned int first_phy, unsigned int second_phy)
{
	return octet_of_phy(first_phy) == octet_of_phy(second_phy);
}

/*
 * Predicates on nodes
 */
static inline int node_used(const struct qos_node *node)
{
	return QOS_BITS_IS_SET(node->flags, QOS_NODE_FLAGS_USED);
}

static inline int node_internal(const struct qos_node *node)
{
	return node_used(node) && QOS_BITS_IS_SET(node->flags,
						  QOS_NODE_FLAGS_INTERNAL);
}

static inline int node_child(const struct qos_node *node)
{
	return node_used(node) && (node->type == TYPE_SCHED ||
				   node->type == TYPE_QUEUE);
}

static inline int node_parent(const struct qos_node *node)
{
	return node_used(node) && (node->type == TYPE_SCHED ||
				   node->type == TYPE_PORT);
}

static inline int node_port(const struct qos_node *node)
{
	return node_used(node) && (node->type == TYPE_PORT);
}

static inline int node_sched(const struct qos_node *node)
{
	return node_used(node) && (node->type == TYPE_SCHED);
}

static inline int node_queue(const struct qos_node *node)
{
	return node_used(node) && (node->type == TYPE_QUEUE);
}

/*
 * Id and phy mapping
 */
unsigned int get_id_from_phy(const struct pp_mapping *map, unsigned int phy);
unsigned int get_phy_from_id(const struct pp_mapping *map, unsigned int id);

/* Map id <==> phy */
void map_id_phy(struct pp_mapping *map, unsigned int id, unsigned int phy);

/* Map id ==>  QOS_UNKNOWN_PHY */
void map_id_reserved(struct pp_mapping *map, unsigned int id);

/* Invalidate both the id and the phy that is currently mapped to it */
void map_invalidate_id(struct pp_mapping *map, unsigned int id);

/*
 * Phy node mapping
 */
struct qos_node *get_node_from_phy(struct pp_nodes *nodes, unsigned int phy);
const struct qos_node *get_const_node_from_phy(const struct pp_nodes *nodes,
					       unsigned int phy);
unsigned int get_phy_from_node(const struct pp_nodes *nodes,
		      const struct qos_node *node);

/*
 * Queue of uint16_t data.
 * Data is dequeued from queue's head and enqueued into its tail
 */
/* return head of queue or QOS_INVALID_ID if queue is empty */
uint16_t pp_queue_dequeue(struct pp_queue *queue);

/* return 0 on success -1 if queue is full */
int pp_queue_enqueue(struct pp_queue *queue, uint16_t data);

/* empty queue */
void pp_queue_reset(struct pp_queue *queue);

/*
 * Pool (stack) of uint16_t data
 */
uint16_t pp_pool_get(struct pp_pool *pool);
int pp_pool_put(struct pp_pool *pool, uint16_t data);
int qos_pools_init(struct pp_qos_dev *qdev, unsigned int max_port);
void qos_pools_clean(struct pp_qos_dev *qdev);
/*
 * Cyclic buffer
 */
/*
 * Copy size bytes from buffer to command,
 * return 0 on success, -1 if there are less than size bytes
 * in buffer.
 * get remove read bytes from buffer while peek not
 */
int cmd_queue_get(struct cmd_queue *q, void *cmd, size_t size);
int cmd_queue_peek(struct cmd_queue *q, void *_cmd, size_t size);
int cmd_queue_is_empty(struct cmd_queue *q);

/*
 * Copy size bytes into cyclic buffer
 * return 0 on success, -1 if buffer has no for size bytes
 */
int cmd_queue_put(struct cmd_queue *q, void *cmd, size_t size);

/*
 * Octets manipulation
 */

/**
 * octet_get_with_at_least_free_entries() - find an octet with free entries.
 * @octets:
 * @count: The minimum number of free entries the octet should have
 * Return: an octet which satisfies the following:
 *  - It has at least count free entries
 *  - No other octet that satisfies the previous condition has less free entries
 *    than this octet
 *
 * If no such octet is found QOS_INVALID_OCTET is returned
 *
 * Note octet is removed from container, user should call
 * to octet_set_use_count (or nodes_move/octet_nodes_shift which call it) to
 * return it
 */
unsigned int octet_get_with_at_least_free_entries(struct pp_octets *octets,
						  unsigned int count);

/**
 * octet_get_least_free_entries() - The octet that has the least free entries
 * @octets:
 * Return:
 *
 * Note octet is removed from container, user should call
 * to octet_set_use_count (or nodes_move/octet_nodes_shift which call it) to
 * return it
 */
unsigned int octet_get_least_free_entries(struct pp_octets *octets);

/*
 * Initialize qos dev, max port designates phy of highest port
 */
void _qos_init(unsigned int max_port, struct pp_qos_dev **qdev);
void _qos_clean(struct pp_qos_dev *qdev);

/*
 * Create qos dev with hardware parameters
 */
struct pp_qos_dev *create_qos_dev_desc(struct qos_dev_init_info *initinfo);

/**
 * octet_get_min_sibling_group() - Find "min parent" - a parent with least
 * children on octet
 * @qdev:
 * @octet:
 * @spc_parent:      The number of children for this parent is increased by one
 *                   only for the sake of finding the "min parent"
 * @num_of_children: Holds the number of children of "min parent"
 * Return:	     The min parent, or NULL if octet is empty
 *
 * Note If several parents hold the same number of children on the octet the
 * first is returned
 *
 */
struct qos_node *octet_get_min_sibling_group(const struct pp_qos_dev *qdev,
					     unsigned int octet,
					     const struct qos_node *spc_parent,
					     unsigned int *num_of_children);

/**
 * nodes_modify_used_status() - Set/Clear node's use status
 * @qdev:
 * @first_phy: First node's phy
 * @count:     Number of nodes
 * @status:    Desired status (0/1)
 */
void nodes_modify_used_status(struct pp_qos_dev *qdev, unsigned int first_phy,
			      unsigned int count, unsigned int status);


/*
 * Remove node
 */
/* remove node and free its resources, assume node has no children */
int node_remove(struct pp_qos_dev *qdev, struct qos_node *node);
/*
 * mark the node unused, update parents, id and rlm are not freed.
 * assume node has no children
 */
void node_phy_delete(struct pp_qos_dev *qdev, unsigned int phy);

void release_rlm(struct pp_pool *rlms, unsigned int rlm);

/* Return id to pool */
void release_id(struct pp_qos_dev *qdev, unsigned int id);

int node_flush(struct pp_qos_dev *qdev, struct qos_node *node);

/**
 * node_cfg_valid() - check if node configuration is valid
 * The process of modifying a node involves cloning orig node to
 * temporary node, making the modification on this temporary node
 * according to new configuration requested by user
 * and check the validity of the result, if its valid than copy temporary node
 * to orig node.
 *
 * @qdev:
 * @node: The temporary node whose configuration should be checked
 * @orig_node: original node from which node was copied from, NULL if new node
 * @prev_virt_parenti_phy: phy of prev virtual parent
 * Return: 1 if configuration is valid, 0 if not
 */
int node_cfg_valid(const struct pp_qos_dev *qdev,
		   const struct qos_node *node,
		   const struct qos_node *orig_node,
		   unsigned int prev_virt_parent_phy);

int get_node_prop(const struct pp_qos_dev *qdev,
		  const struct qos_node *node,
		  struct pp_qos_common_node_properties *common,
		  struct pp_qos_parent_node_properties *parent,
		  struct pp_qos_child_node_properties *child);

int set_node_prop(struct pp_qos_dev *qdev,
		  struct qos_node *node,
		  const struct pp_qos_common_node_properties *common,
		  const struct pp_qos_parent_node_properties *parent,
		  const struct pp_qos_child_node_properties *child,
		  uint32_t *modified);

/*
 * Init node with default values
 * common, parent and child dictate which
 * components of node should be initialized
 */
void node_init(const struct pp_qos_dev *qdev,
	       struct qos_node *node,
	       unsigned int common,
	       unsigned int parent,
	       unsigned int child);

/**
 * get_port_rlms() - Return all active rlms on a subtree
 * @qdev:
 * @phy:       Phy of subtree's node
 * @rlms:      Array to store the rlms - may be NULL
 * @size:      Size of array - may be 0
 * @queues_num: The number of queues on the subtree
 */
void get_active_port_rlms(struct pp_qos_dev *qdev, u32 phy,
			  u16 *rlms,
			  u32 size, u32 *queues_num);

/**
 * get_node_queues() - Return all queues on a subtree
 * @qdev:
 * @phy:       Phy of subtree's node
 * @queue_ids: Array to store the queues ids - may be NULL
 * @size:      Size of array - may be 0
 * @queues_num: The number of queues on the subtree
 */
void get_node_queues(struct pp_qos_dev *qdev, unsigned int phy,
		     uint16_t *queue_ids,
		     unsigned int size, unsigned int *queues_num);

/**
 * update_bwl_buffer() - Update all bw limit node values in temp
 *        buffer
 * @qdev:
 * @phy:       Phy of subtree's node
 */
void update_bwl_buffer(struct pp_qos_dev *qdev, unsigned int phy);

/**
 * get_node_queues() - Return all active queues on a subtree
 * @qdev:
 * @phy:       Phy of subtree's node
 * @queue_ids: Array to store the queues ids - may be NULL
 * @size:      Size of array - may be 0
 * @queues_num: The number of queues on the subtree
 */
void get_active_node_queues(struct pp_qos_dev *qdev, unsigned int phy,
			    uint16_t *queue_ids,
			    unsigned int size, unsigned int *queues_num);

int check_sync_with_fw(struct pp_qos_dev *qdev);

/**
 * phy_alloc_by_parent() - Allocate new phy for a node
 * @qdev:
 * @parent:          Node's parent
 * @child_priority:  Relevant only for WSP parent
 * Return:
 */
unsigned int phy_alloc_by_parent(struct pp_qos_dev *qdev,
				 struct qos_node *parent,
				 unsigned int child_priority);

/* Return the first non internal ancestor of a node */
unsigned int get_virtual_parent_phy(const struct pp_nodes *nodes,
				    const struct qos_node *child);

void node_update_children(struct pp_qos_dev *qdev,
				 struct qos_node *parent,
				 unsigned int new_phy);

int update_predecessors(struct pp_qos_dev *qdev,
			struct qos_node *node, void *data);

void tree_update_predecessors(struct pp_qos_dev *qdev, unsigned int phy);
/*
 * Assume that sched is internal scheduler
 * 1. Update sched's virtual bandwidth share to be the sum of its
 *    children's share.
 * 2. If sched's parent is also internal scheduler
 *    update its virtual bandwidth share also to be the update
 *    bw share sum of its children. And keep doing that
 *    up the tree hierarchy so long parent is an internal scheduler
 * 3. For each
 */
void update_internal_bandwidth(struct pp_qos_dev *qdev,
			       struct qos_node *sched);

/* Remove all nodes (including root) of a subtree */
int tree_remove(struct pp_qos_dev *qdev, unsigned int phy);

/* Flush all nodes (including root) of a subtree */
int tree_flush(struct pp_qos_dev *qdev, unsigned int phy);

/* Modify blocked status of queues on a subtree */
int tree_modify_blocked_status(struct pp_qos_dev *qdev,
			       unsigned int phy,
			       unsigned int status);
void get_bw_grp_members_under_node(struct pp_qos_dev *qdev, unsigned int id,
				   unsigned int phy, uint16_t *ids,
				   unsigned int size,
				   unsigned int *ids_num);

/* Creates pool of free ports phys */
struct pp_pool *free_ports_phys_init(unsigned int *reserved,
				     unsigned int max_port,
				     const unsigned int *reserved_ports,
				     unsigned int size);

/* return the port ancestor of a node */
static inline unsigned int get_port(const struct pp_nodes *nodes,
				    unsigned int phy)
{
	const struct qos_node *node;

	node = get_const_node_from_phy(nodes, phy);
	while (node_child(node))
		node = get_const_node_from_phy(nodes,
					       node->child_prop.parent_phy);

	QOS_ASSERT(node_port(node), "Did not reach port node for phy %u\n",
		   phy);
	return get_phy_from_node(nodes, node);
}

int qos_device_ready(const struct pp_qos_dev *qdev);
void qos_module_init(void);
void remove_qos_instance(unsigned int id);
int _pp_qos_queue_block(struct pp_qos_dev *qdev, unsigned int id);
int _pp_qos_queue_unblock(struct pp_qos_dev *qdev, unsigned int id);
int _pp_qos_queue_flush(struct pp_qos_dev *qdev, unsigned int id);
void wake_uc(void *data);
void update_cmd_id(struct driver_cmds *drvcmds);
void transmit_cmds(struct pp_qos_dev *qdev);
void update_children_position(
			struct pp_qos_dev *qdev,
			struct qos_node *child,
			struct qos_node *parent,
			struct qos_node *node_src);
int allocate_ddr_for_qm(struct pp_qos_dev *qdev);
void free_ddr_for_qm(struct pp_qos_dev *qdev);
int allocate_ddr_for_qm_on_platform(struct pp_qos_dev *qdev);
int check_sync_with_fw(struct pp_qos_dev *qdev);

int get_active_port_phy_queues(struct pp_qos_dev *qdev, u32 port_id,
			       u16 *rlms, u16 *ids, u32 size, u32 *queues_num);
int store_port_queue_max_allowed(struct pp_qos_dev *qdev,
				 u32 port_id, u16 *rlms,
				 u16 *rlms_ids,
				 u32 queues_num);

#ifdef PP_QOS_TEST
void test_cmd_queue(void);
void basic_tests(void);
void advance_tests(void);
void reposition_test(void);
void falcon_test(void);
void simple_test(void);
void stat_test(void);
void info_test(void);
void wsp_test(void);
void wa_min_test(void);
void wa_test(void);
void load_fw_test(void);
void tests(void);
void test_init_instance(struct pp_qos_dev *qdev);
int add_qos_dev(void);
void remove_qos_dev(void);
int port_cfg_valid(const struct pp_qos_dev *qdev, const struct qos_node *node,
		   const struct qos_node *orig_node);
int queue_cfg_valid(const struct pp_qos_dev *qdev, const struct qos_node *node,
		    const struct qos_node *orig_node,
		    unsigned int prev_virt_parent_phy);
int sched_cfg_valid(const struct pp_qos_dev *qdev, const struct qos_node *node,
		    const struct qos_node *orig_node,
		    unsigned int prev_virt_parent_phy);
struct pp_octets *octets_init(unsigned int last_port_octet);
void octets_clean(struct pp_octets *octets);
void debug_verify_octet_usage(struct pp_octets *octets, unsigned int octet,
			      unsigned int usage);
void nodes_move(struct pp_qos_dev *qdev, unsigned int dst_phy,
		unsigned int src_phy, unsigned int count);
void octet_nodes_shift(struct pp_qos_dev *qdev, unsigned int first_phy,
		       unsigned int count, int shift);
unsigned int get_children_bandwidth_share(const struct pp_qos_dev *qdev,
					  const struct qos_node *parent);
unsigned int get_virt_children_bandwidth_share(const struct pp_qos_dev *qdev,
					       const struct qos_node *parent);
unsigned int get_children_bandwidth_share(const struct pp_qos_dev *qdev,
					  const struct qos_node *parent);
void transmit_cmds(struct pp_qos_dev *qdev);
void update_cmd_id(struct driver_cmds *drvcmds);
int check_sync_with_fw(struct pp_qos_dev *qdev);
#define STATIC_UNLESS_TEST
#else
#define STATIC_UNLESS_TEST static
#endif

#endif
