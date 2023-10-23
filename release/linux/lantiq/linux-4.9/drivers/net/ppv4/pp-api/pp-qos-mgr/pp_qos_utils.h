/*
 * pp_qos_utils.h
 * Description: Packet Processor QoS Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2017-2020 Intel Corporation
 */
#ifndef PP_QOS_UTILS_H
#define PP_QOS_UTILS_H

#include <linux/pp_qos_api.h>
#include <linux/debugfs.h>      /* struct dentry */
#include <linux/range.h>
#include "pp_qos_uc_defs.h"
#include "pp_qos_common.h"

extern u64 qos_wred_base_addr;
#define QOS_WRED_BASE_ADDR qos_wred_base_addr
#ifdef WRED_REG_BASE
#undef WRED_REG_BASE
#define WRED_REG_BASE qos_wred_base_addr
#endif

/******************************************************************************/
/*                              CONF                                          */
/******************************************************************************/
#define PPV4_QOS_DRV_VER "1.1.0-lgm"

#define NUM_OF_NODES          (2048U)
#define NUM_OF_OCTETS         ((NUM_OF_NODES / 8) - 1)
#define PP_QOS_TMP_NODE       (2047U)
#define QOS_MAX_SHARED_BW_GRP QOS_MAX_SHARED_BANDWIDTH_GROUP

/* for calculating number of ddr pages for qm from resource size */
#define PPV4_QOS_DESC_SIZE    (16U)
#define PPV4_QOS_DESC_IN_PAGE (16U)

#define PPV4_QOS_LOGGER_WRITE_IDX_OFFSET (0)
#define PPV4_QOS_LOGGER_READ_IDX_OFFSET  (4)
#define PPV4_QOS_LOGGER_RING_OFFSET      (8)

#define PPV4_QOS_LOGGER_HEADROOM_SIZE (8U)
#define PPV4_QOS_LOGGER_MSG_SIZE (128U)
#define PPV4_QOS_LOGGER_BUF_SIZE (PPV4_QOS_LOGGER_HEADROOM_SIZE +   \
				 (PPV4_QOS_LOGGER_MSG_SIZE * 4096))

#define PPV4_QOS_DMA_BUFF_SIZE 4096

#define FIRMWARE_FILE "ppv4-qos-fw.elf"
/******************************************************************************/
/*                              DEFINES                                       */
/******************************************************************************/

/**
 * @define non allocated octet is marked as invalid
 */
#define QOS_INVALID_OCTET   0xFFFFU

/**
 * @define non allocated phy is marked as invalid
 */
#define QOS_INVALID_PHY     0xFFFFU

/**
 * @define non allocated rlm is marked as invalid
 */
#define QOS_INVALID_RLM     (QOS_INVALID_PHY)

/**
 * @define allocated logical id is marked as unmapped until actual mapping
 */
#define QOS_UNMAPPED_PHY    0xFFFEU

#define QOS_CHILD_UNLIMITED_WEIGHT 127

#define QOS_OCTET_VALID(octet) ((octet) < NUM_OF_OCTETS)
#define QOS_ID_VALID(id)       ((id) < NUM_OF_NODES)
#define QOS_PHY_VALID(phy)     ((phy) < NUM_OF_NODES)
#define QOS_PHY_UNMAPPED(phy)  ((phy) == QOS_UNMAPPED_PHY)
#define QOS_BW_GRP_VALID(id)   ((id) && ((id) <= QOS_MAX_SHARED_BW_GRP))

#define QOS_MODIFIED_NODE_TYPE                   BIT(0)
#define QOS_MODIFIED_BANDWIDTH_LIMIT             BIT(1)
#define QOS_MODIFIED_SHARED_GROUP_ID             BIT(2)
#define QOS_MODIFIED_ARBITRATION                 BIT(3)
#define QOS_MODIFIED_BEST_EFFORT                 BIT(4)
#define QOS_MODIFIED_PRIORITY                    BIT(5)
#define QOS_MODIFIED_BW_WEIGHT                   BIT(6)
#define QOS_MODIFIED_PARENT                      BIT(7)
#define QOS_MODIFIED_RING_ADDRESS                BIT(8)
#define QOS_MODIFIED_RING_SIZE                   BIT(9)
#define QOS_MODIFIED_CREDIT                      BIT(10)
#define QOS_MODIFIED_DISABLE                     BIT(11)
#define QOS_MODIFIED_MAX_BURST                   BIT(12)
#define QOS_MODIFIED_BLOCKED                     BIT(13)
#define QOS_MODIFIED_WRED_ENABLE                 BIT(14)
#define QOS_MODIFIED_WRED_MIN_GREEN              BIT(15)
#define QOS_MODIFIED_WRED_MAX_GREEN              BIT(16)
#define QOS_MODIFIED_WRED_SLOPE_GREEN            BIT(17)
#define QOS_MODIFIED_WRED_MIN_YELLOW             BIT(18)
#define QOS_MODIFIED_WRED_MAX_YELLOW             BIT(19)
#define QOS_MODIFIED_WRED_SLOPE_YELLOW           BIT(20)
#define QOS_MODIFIED_WRED_MAX_ALLOWED            BIT(21)
#define QOS_MODIFIED_WRED_MIN_GUARANTEED         BIT(22)
#define QOS_MODIFIED_RLM                         BIT(23)
#define QOS_MODIFIED_WRED_FIXED_DROP_PROB_ENABLE BIT(24)
#define QOS_MODIFIED_WRED_FIXED_GREEN_PROB       BIT(25)
#define QOS_MODIFIED_WRED_FIXED_YELLOW_PROB      BIT(26)
#define QOS_MODIFIED_QUEUE_PORT                  BIT(27)
#define QOS_MODIFIED_CODEL                       BIT(28)
#define QOS_MODIFIED_PORT_GREEN_THRESHOLD        BIT(29)
#define QOS_MODIFIED_PORT_YELLOW_THRESHOLD       BIT(30)
#define QOS_MODIFIED_PORT_ENHANCED_WSP           BIT(31)

#define MAX_MOVING_NODES 8
#define MAX_WSP_CHILDREN 8

/******************************************************************************/
/*                              TYPES                                         */
/******************************************************************************/

/**
 * @struct fw_com - Communication to/from FW
 * @cmdbuf: Command buffer offset
 * @cmdbuf_sz: Command buffer size
 * @mbx_to_uc: Base address for signaling uC
 */
struct fw_com {
	u64         cmdbuf;
	size_t      cmdbuf_sz;
	u64         mbx_to_uc;
};

struct fw_sec_info {
	u32         is_in_dccm;
	u32         dccm_offset;
	u32         max_size;
};

/**
 * @struct pp_qos_init_param - qos subsystem initialization struct
 * @valid                 params valid
 * @qos_uc_base           UC base address
 * @qos_wred_base         WRED base address
 * @wakeuc                wkae uC register base address
 * @max_ports             Maximum supported ports
 * @max_queues            Maximum supported queues
 * @num_syncqs            Maximum supported sync queues (user)
 * @enhanced_wsp          When set enhanced WSP feature is enabled
 * @wred_prioritize_pop   set to 1 to prioritize pop
 * @wred_total_avail_resources upper bound for the sum of bytes on all queues
 * @wred_p_const          WRED algorithm p, used for calculating
 *                        avg queue depth avg = (old_avg*(1-p))
 *                        + (curr_avg*p) p is taken to b
 *                        wred_p_const / 1023 wred_p_const
 *                        should be in the range 0..1023
 * @wred_max_q_size       maximum size on bytes of any queue
 * @hw_clk                HW Clock in MHz
 * @fw_com                FW Communication configuration
 * @fw_sec_data_stack     Describes FW internal data and stack
 *                        location
 * @reserved_ports        These ports are reserved for client to explicity
 *                        select the physical_port ithat will be used when
 *                        adding a new port. All other non reserved ports are
 *                        managed by the system, i.e. when client adds a new
 *                        port system will select a new free port from the
 *                        unreserved ports.
 *                        To reserve a port its value on the array should be
 *                        not 0
 * @reserved_queues       These queues are reserved for contiguous queue
 *                        allocation
 * @bm_base               bm base address
 * @dbgfs                 debugfs parent folder
 * @sysfs                 sysfs parent folder
 */
struct pp_qos_init_param {
	bool               valid;
	u64                qos_uc_base;
	u64                qos_wred_base;
	u64                wakeuc;
	u32                max_ports;
	u32                max_queues;
	u32                num_syncqs;
	u32                enhanced_wsp;
	u32                wred_prioritize_pop;
	u32                wred_total_avail_resources;
	u32                wred_p_const;
	u32                wred_max_q_size;
	u32                hw_clk;
	struct fw_com      fwcom;
	struct fw_sec_info fw_sec_data_stack;
	u32                reserved_ports[PP_QOS_MAX_PORTS];
	u32                reserved_queues;
	phys_addr_t        bm_base;
	struct dentry     *dbgfs;
	struct kobject    *sysfs;
};

/**
 * @struct pp_qos_node_info
 * @note Since 0 is a valid value for some of the fields.
 *        PPV4_QOS_INVALID will be used to mark field value as
 *        irrelevant
 * @note All above values except from type, is_internal and
 *        logical id are taken from HW and not from driver DB
 */
struct pp_qos_node_info {
	/*! Node type */
	enum pp_qos_node_type type;

	/*! Ansectors of node (irrelevant to PORT) */
	struct pp_qos_node_num preds[QOS_MAX_PREDECESSORS];

	/*! Number of direct children (irrelevant to QUEUE) */
	struct pp_qos_node_num children[8];

	/*! Is it internal node */
	s32  is_internal;

	/*! Relevant only for QUEUE */
	u32 queue_physical_id;

	/*! Bw limit In Kbps */
	u32 bw_limit;

	/*! Port (phy) of this queue (relevant only for QUEUE) */
	u32 port;
};

/**
 * @brief All queue stats definition
 */
struct queue_stats {
	/*! wred stats */
	struct pp_qos_queue_stat wred;
	/*! codel stats */
	struct pp_qos_codel_stat codel;
	/*! total forward packets */
	u32 total_fwd_pkts;
};

enum node_type {
	TYPE_UNKNOWN,
	TYPE_PORT,
	TYPE_SCHED,
	TYPE_QUEUE
};

struct pp_pool {
	u32 capacity;
	u32 top;
	u16 invalid;
	u16 data[1];
};

/* Additional 4 bytes for the validation signature */
#define PPV4_QOS_STAT_SIZE           \
	(max(sizeof(struct qm_info), \
	sizeof(struct system_stats_s)) + 4)

/**
 * @struct pp_mapping
 * @brief Holds mapping between a logical id and a physical id. Used for:
 * 	  1. Logical id to physical node id mapping
 * 	  2. Logical id to physical queue id (rlm) mapping
 */
struct pp_mapping {
	/* phyical id mapped to a logical id */
	u16 id2phy[NUM_OF_NODES];
	/* logical id mapped to a physical id */
	u16 phy2id[NUM_OF_NODES];
	/* replication tree id's */
	u16 id2rep_id[NUM_OF_NODES];
};

struct octet_info {
	struct list_head list;
	u8 usage;
};

struct pp_octets {
	s32 last_port_octet;
	struct octet_info info[NUM_OF_OCTETS];
	struct list_head lists[9];
};

struct q_item {
	struct list_head list;
	u16 data;
};

struct pp_syncq {
	u32 capacity;
	u32 *phy;
	u32 *id;
};

struct pp_queue {
	u32 capacity;
	struct list_head used;
	struct list_head free;
	struct q_item items[1];
};

struct child_node_properties {
	u16 parent_phy;
	u8  priority;
	u8  virt_bw_share;
	u8  bw_weight;
};

struct parent_node_properties {
	enum pp_qos_arbitration arbitration;
	u16 first_child_phy;
	u8  num_of_children;
};

#define WRED_MAX_ALLOWED_DEFAULT (0x400)

struct qos_node {
	/* Port / Sched / Queue */
	enum node_type                type;
	/* Configured BW limit by user */
	u32                           bandwidth_limit;
	/* Actual configured BW limit */
	u32                           actual_bandwidth_limit;
	u16                           shared_bw_group;
	/* Max quantas that can be accumulated
	 * num quantas = 1 << (max_burst)
	 * Max value is 7
	 */
	u16                           max_burst;
	/* child properties, relevant for sched and queue */
	struct child_node_properties  child_prop;
	/* parent properties, relevant for port and sched */
	struct parent_node_properties parent_prop;
	/* Dual rate shaper scheduler logical id */
	u16                           dr_sched_id;
	union _data {
		struct {
			unsigned long ring_address;
			size_t ring_size;
			u32    credit;
			s32    disable;
			u32    green_threshold;
			u32    yellow_threshold;
		} port;
		struct {
			u16 new_child_phy;
			u8  new_child_skipped;
		} sched;
		struct _queue {
			u32 green_min;
			u32 green_max;
			u32 yellow_min;
			u32 yellow_max;
			u32 max_allowed;
			u16 port_phy;
			u32 min_guaranteed;
			u32 fixed_green_prob;
			u32 fixed_yellow_prob;
			u16 rlm;
			u8  green_slope;
			u8  yellow_slope;
			u8  aqm_context;
			u8  codel_en;
			u16 is_alias;
			u32 eir;
			u16 ebs;
			u16 last_active_port_phy;
		} queue;
	} data;

#define NODE_FLAGS_USED                  BIT(0) /* Active node */
#define NODE_FLAGS_INTERNAL              BIT(1) /* Internal node */
#define NODE_FLAGS_QUEUE_NEW_SYNCQ       BIT(2) /* SQ allocated */
#define NODE_FLAGS_QUEUE_CONNECTED_SYNCQ BIT(3) /* SQ connected to tree */
#define NODE_FLAGS_PARENT_BEST_EFFORT_EN BIT(4) /* Best effort */
#define NODE_FLAGS_REPLICATION_NODE      BIT(5) /* Dual rate shaper replica */
#define NODE_FLAGS_SCHED_SYNCQ           BIT(6) /* Sched for SQ */
#define NODE_FLAGS_PORT_PACKET_CREDIT_EN BIT(7) /* Packet credit mode */
#define NODE_FLAGS_PORT_BYTE_CREDIT_EN   BIT(8) /* Byte credit - DEPRECATED */
#define NODE_FLAGS_QUEUE_WRED_EN         BIT(10) /* WRED algorithm enabled */
#define NODE_FLAGS_QUEUE_BLOCKED         BIT(11) /* Queue is blocked */
#define NODE_FLAGS_QUEUE_WRED_FIXED_DROP_PROB_EN BIT(12) /* Fixed drop prob */
#define NODE_FLAGS_QUEUE_AQM             BIT(13) /* Connected to AQM context */
#define NODE_FLAGS_ENHANCED_WSP          BIT(14) /* Enhanced WSP Port */
	u16 flags;
};

struct pp_nodes {
	struct qos_node nodes[NUM_OF_NODES];
};

struct shared_bw_group {
	u32 max_burst;
	u8  used;
	u8  reserved;
};

struct driver_cmds {
	struct cmd_queue *cmdq;
	struct cmd_queue *pendq;
	u32              cmd_id;
	u32              cmdbuf_watermark;
};

/* HW configuration */
struct hw_conf {
	void         *dma_buff_virt;
	dma_addr_t   dma_buff_phys;
	size_t       dma_buff_sz;
	void         *qm_ddr_virt;
	dma_addr_t   qm_ddr_phys;
	u32          qm_num_pages;
	size_t       qm_sz;
	void         *fw_logger_ddr_virt;
	dma_addr_t   fw_logger_ddr_phys;
	size_t       fw_logger_sz;
	void         *fw_stats_ddr_virt;
	dma_addr_t   fw_stats_ddr_phys;
	size_t       fw_stats_sz;
	void         *max_allow_ddr_virt;
	dma_addr_t   max_allow_ddr_phys;
	size_t       max_allow_sz;
	void         *bwl_ddr_virt;
	dma_addr_t   bwl_ddr_phys;
	size_t       bwl_sz;
	void         *sbwl_ddr_virt;
	dma_addr_t   sbwl_ddr_phys;
	size_t       sbwl_sz;
	void         *wsp_queues_ddr_virt;
	dma_addr_t   wsp_queues_ddr_phys;
	size_t       wsp_queues_sz;
	void         *api_str_buf;
	u16          *rlms_container;
	u32          max_bw_limit_kbps; /* @todo remove this workaround */
};

struct fw_ver {
	u32 major;
	u32 minor;
	u32 build;
};

struct qos_aqm_sf_entry {
	struct pp_qos_aqm_sf_config sf_cfg;
	bool                        enabled;
};

struct qos_drv_stats {
	u32 active_nodes;
	u32 active_queues;
	u32 active_reserved_queues;
	u32 active_ports;
	u32 active_sched;
	u32 active_internals;
};

enum rlm_range {
	RLM_RANGES_RESERVED,
	RLM_RANGES_SYNCQ,
	RLM_RANGES_NORMAL,
	RLM_RANGES_MAX,
};

struct wsp_helper_cfg {
	bool enable;
	u32 timeout_microseconds;
	u32 byte_threshold;
	u32 num_queues;
};
struct pp_qos_dev {
	LOCK                           lock;
	u32                            id;
	bool                           initialized;
	struct qos_aqm_sf_entry        sf_entry[PP_QOS_AQM_MAX_SERVICE_FLOWS];
	struct wsp_helper_cfg          ewsp_cfg;
	struct pp_qos_init_param       init_params;
	struct shared_bw_group         groups[QOS_MAX_SHARED_BW_GRP + 1];
	struct pp_pool                 *ids;
	struct pp_pool                 *rlms;
	struct pp_pool                 *reserved_rlms;
	struct pp_pool                 *syncq_rlms;
	struct range                   rlm_ranges[RLM_RANGES_MAX];
	struct pp_pool                 *portsphys;
	struct pp_nodes                *nodes;
	struct pp_mapping              *mapping; /* logical id/phy id mapping */
	struct pp_mapping              *rlm_mapping; /* logical id/rlm mapping*/
	struct pp_octets               *octets;
	struct pp_queue                *queue;
	struct pp_syncq                *syncq;
	struct driver_cmds             drvcmds;
	struct fw_ver                  fwver;
	void                           *fwbuf;
	struct hw_conf                 hwconf;
#define PP_QOS_FLAGS_ASSERT            (0x1U)
	u32                            dbg_flags;
	struct dentry                  *dbgfs;
	struct kobject                 *sysfs;
};

#define PP_QOS_DEVICE_IS_ASSERT(qdev)                             \
	(QOS_BITS_IS_SET((qdev)->dbg_flags, PP_QOS_FLAGS_ASSERT))

/******************************************************************************/
/*                              UTILS                                         */
/******************************************************************************/
#define QOS_LOG(format, arg...) QOS_LOG_DEBUG(format, ##arg)
void stop_run(void);
#define QOS_ASSERT(condition, format, arg...)                      \
do {                                                               \
	if (!(condition)) {                                        \
		QOS_LOG_CRIT("Assertion. " format, ##arg);         \
		stop_run();                                        \
	}                                                          \
} while (0)

#define QOS_BITS_SET(flags, bits)    ((flags) |= (bits))
#define QOS_BITS_CLEAR(flags, bits)  ((flags) &= ~(bits))
#define QOS_BITS_TOGGLE(flags, bits) ((flags) ^= (bits))
#define QOS_BITS_IS_SET(flags, bits) ((flags) & (bits))

#ifndef PP_QOS_DISABLE_CMDQ
s32 check_sync_with_fw(struct pp_qos_dev *qdev, bool dump_entries);
#endif

/**
 * @brief qos sysfs init
 * @return s32 0 for success, non-zero otherwise
 */
s32 qos_sysfs_init(struct pp_qos_dev *qos_dev);

/**
 * @brief qos sysfs cleanup
 */
void qos_sysfs_clean(struct pp_qos_dev *qdev);

/**
 * @brief Print stats of all queues into a buffer
 * @param buf buffer to print to
 * @param sz buffer size
 * @param n pointer to return number of bytes written
 * @param stats the stats
 * @param num_stats number of stats
 * @param data qdev reference
 * @return s32 0 on success, error code otherwise
 */
s32 qos_queues_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			  u32 num_stats, void *data);

/**
 * @brief Calculates the difference between queues stats
 * @param pre pre stats
 * @param num_pre number of pre stats
 * @param post post stats
 * @param num_post number of post stats
 * @param delta stats to save the delta
 * @param num_delta number of delta stats
 * @note all stats array MUST be with same size
 * @return s32 0 on success, error code otherwise
 */
s32 qos_queues_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			  void *delta, u32 num_delta, void *data);

/**
 * @brief Get all queues wred and codel stats
 * @param stats pointer to save the stats
 * @param num_stats number of stats entries
 * @param data user data
 * @return s32 0 on success, error code otherwise
 */
s32 qos_queues_stats_get(void *stats, u32 num_stats, void *data);

/**
 * @brief Get bitmap of nodes which needs to be updated due
 *        to clock frequency change,
 *        Each bit represent node physical id, set bit means the node
 *        needs to be update
 * @param qdev qos device
 * @param bmap bitmap
 * @param n number of bits if the bitmap
 * @return s32 0 on success, error code otherwise
 */
s32 qos_clk_depend_nodes_list_get(struct pp_qos_dev *qdev, ulong *bmap, u32 n);
/**
 * pp_qos_dev_init() - set qos system wide configuration
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @conf: qos init parameters
 *
 * @note
 * All other functions except pp_qos_dev_open will failed if called
 * before this function
 *
 * Return: 0 on success
 */
s32 pp_qos_dev_init(struct pp_qos_dev *qos_dev, struct pp_qos_init_param *conf);

/**
 * @brief Init WRED HW registers to prevent packets
 *        to queued into the queues before the uC
 *        was initialized
 * @param qdev
 * @return int 0 on success, error code otherwise
 */
int qos_init_wred_hw(struct pp_qos_dev *qdev);

/**
 * pp_qos_dev_clean() - clean resources.
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 *
 * @note
 *   qos_dev is invalid after this call
 */
void pp_qos_dev_clean(struct pp_qos_dev *qos_dev);

/**
 * pp_qos_config_get() - get qos system wide configuration
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @conf: qos init parameters
 *
 * @note
 * All other functions except pp_qos_dev_open will failed if called
 * before this function
 *
 * Return: 0 on success
 */
s32 pp_qos_config_get(struct pp_qos_dev *qos_dev,
		      struct pp_qos_init_param *conf);

/* Dump the complete qos tree */
void qos_dbg_tree_show(struct pp_qos_dev *qdev, bool user_tree,
		       struct seq_file *s);
void qos_dbg_tree_show_locked(struct pp_qos_dev *qdev, bool user_tree,
			      struct seq_file *s);
s32 qos_dbg_qm_stat_show(struct pp_qos_dev *qdev, struct seq_file *s);

/* Phy octet mapping */
static inline s32 octet_of_phy(u32 phy)
{
	QOS_ASSERT(QOS_PHY_VALID(phy), "Invalid phy %u\n", phy);
	return (phy >> 3);
}

/* Offset of phy on octet (0..7) */
static inline s32 octet_phy_offset(u32 phy)
{
	QOS_ASSERT(QOS_PHY_VALID(phy), "Invalid phy %u\n", phy);
	return (phy & 0x7);
}

/* Return true iff both phys are on same octet */
static inline s32 same_octet(u32 first_phy, u32 second_phy)
{
	return octet_of_phy(first_phy) == octet_of_phy(second_phy);
}

void octet_nodes_shift(struct pp_qos_dev *qdev, u32 first_phy, u32 count,
		       s32 shift);

void nodes_move(struct pp_qos_dev *qdev, u32 dst_phy, u32 src_phy, u32 count);

int qos_get_node_info(struct pp_qos_dev *qdev, u32 id,
		      struct pp_qos_node_info *info);

/* Predicates on nodes */
static inline bool node_used(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	if (!node) {
		QOS_LOG_ERR("NULL Node\n");
		return 0;
	}

	return QOS_BITS_IS_SET(node->flags, NODE_FLAGS_USED);
}

static inline bool node_internal(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return node_used(node) && QOS_BITS_IS_SET(node->flags,
						  NODE_FLAGS_INTERNAL);
}

static inline bool node_child(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return node_used(node) && (node->type == TYPE_SCHED ||
				   node->type == TYPE_QUEUE);
}

static inline bool node_parent(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return node_used(node) && (node->type == TYPE_SCHED ||
				   node->type == TYPE_PORT);
}

static inline bool node_port(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return node_used(node) && (node->type == TYPE_PORT);
}

static inline bool node_enhanced_wsp_port(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return node_used(node) && (node->type == TYPE_PORT) &&
	       QOS_BITS_IS_SET(node->flags, NODE_FLAGS_ENHANCED_WSP);
}

static inline bool node_sched(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return node_used(node) && (node->type == TYPE_SCHED);
}

static inline bool node_queue(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return node_used(node) && (node->type == TYPE_QUEUE);
}

static inline bool node_rep(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return (node_used(node) &&
		QOS_BITS_IS_SET(node->flags, NODE_FLAGS_REPLICATION_NODE));
}

static inline bool node_new_syncq(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return (node_used(node) &&
		QOS_BITS_IS_SET(node->flags, NODE_FLAGS_QUEUE_NEW_SYNCQ));
}

static inline bool node_connected_syncq(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return (node_used(node) &&
		QOS_BITS_IS_SET(node->flags, NODE_FLAGS_QUEUE_CONNECTED_SYNCQ));
}

static inline bool node_syncq(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return (node_new_syncq(node) || node_connected_syncq(node));
}

static inline bool node_sched_syncq(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	return (node_used(node) &&
		QOS_BITS_IS_SET(node->flags, NODE_FLAGS_SCHED_SYNCQ));
}

/**
 * @brief Test whether a node depend on hw clock rate and needs
 *        to be updated upon clock change
 * @param _node the node
 * @return bool true if it is, false otherwise
 */
static inline bool node_clk_depend(const void *_node)
{
	const struct qos_node *node = (const struct qos_node *)_node;

	/* if the node is used and has a bandwidth limit he needs
	 * needs to be updated when the clock is changed
	 */
	return node_used(node) && (node->bandwidth_limit);
}

/**
 * @brief Iterate over all nodes which 'cond'
 */
#define for_each_qos_node_cond(qdev, node, cond)                               \
	for_each_arr_entry_cond(node, (qdev)->nodes->nodes,                    \
				ARRAY_SIZE((qdev)->nodes->nodes), cond)

/**
 * @brief Iterate over all used nodes
 */
#define for_each_qos_used_node(qdev, node, ...)                                \
	for_each_qos_node_cond(qdev, node, node_used)

/**
 * @brief Iterate over all queues
 */
#define for_each_qos_queue(qdev, node, ...)                                    \
	for_each_qos_node_cond(qdev, node, node_queue)

/**
 * @brief Iterate over all nodes that should be updated
 *        upon clock frequency change
 *
 */
#define for_each_qos_node_clk_dep(qdev, node, ...)                             \
	for_each_qos_node_cond(qdev, node, node_clk_depend)

/* Id and phy mapping */
u32 get_id_from_phy(const struct pp_mapping *map, u32 phy);
u32 get_phy_from_id(const struct pp_mapping *map, u32 id);

/* Map id <==> phy */
void map_id_phy(struct pp_mapping *map, u32 id, u32 phy);

/* Map id ==>  QOS_UNMAPPED_PHY */
void map_id_reserved(struct pp_mapping *map, u32 id);

/* Invalidate id */
void map_invalidate_id(struct pp_mapping *map, u32 id);

/* Id and rep_id mapping */
u32 get_id_from_rep_id(const struct pp_mapping *map, u32 rep_id);
u32 get_rep_id_from_id(const struct pp_mapping *map, u32 id);

/* Map id <==> replication id */
void map_id_rep_id(struct pp_mapping *map, u32 id, u32 rep_id);

void map_invalidate_id_rep_id(struct pp_mapping *map, u32 id);

/* Invalidate phy */
void map_invalidate_phy(struct pp_mapping *map, u32 phy);

/* Invalidate both the id and the phy that is currently mapped to it */
void map_invalidate_id_phy(struct pp_mapping *map, u32 id);

/* Get db node structure from physical node id */
struct qos_node *get_node_from_phy(struct pp_nodes *nodes, u32 phy);
const struct qos_node *get_const_node_from_phy(const struct pp_nodes *nodes,
					       u32 phy);

/* Get physical node id from db node structure */
u32 get_phy_from_node(const struct pp_nodes *nodes,
		      const struct qos_node *node);

/**
 * @brief get the phy from queue is which is a syncq
 * @param qdev qos device
 * @param id syncq id
 * @return u32 phy id
 */
u32 get_phy_from_syncq(struct pp_qos_dev *qdev, u32 id);

/**
 * @brief allocate phy id for syncq id
 * @param qdev qos device
 * @param id syncq id
 * @return u32 phy id
 */
u32 alloc_phy_from_syncq(struct pp_qos_dev *qdev, u32 id);

/**
 * @brief connect a synchronization queue to QoS queue
 * @param qdev qos device
 * @param syncq_phy syncq phy id
 * @param dstq_phy dstq phy id
 * @return s32 0 on success, error code otherwise
 */
s32 connect_sync_queue(struct pp_qos_dev *qdev, u32 syncq_phy, u32 dstq_phy);

/**
 * @brief disconnect a synchronization queue from QoS queue
 * @param qdev qos device
 * @param phy syncq phy id
 * @return s32 0 on success, error code otherwise
 */
s32 disconnect_sync_queue(struct pp_qos_dev *qdev, u32 phy);

/**
 * @brief delete all syncqs
 * @param qdev qos device
 */
void delete_all_syncq(struct pp_qos_dev *qdev);

/* Queue of u16 data.
 * Data is dequeued from queue's head and enqueued into its tail
 * return head of queue or QOS_INVALID_ID if queue is empty
 */
u16 pp_queue_dequeue(struct pp_queue *queue);

/* return 0 on success -1 if queue is full */
s32 pp_queue_enqueue(struct pp_queue *queue, u16 data);

/* empty queue */
void pp_queue_reset(struct pp_queue *queue);

/* Pool (stack) of u16 data */
u16 pp_pool_get(struct pp_pool *pool);
u32 pp_pool_capacity_get(struct pp_pool *pool);
s32 pp_pool_put(struct pp_pool *pool, u16 data);
s32 qos_pools_init(struct pp_qos_dev *qdev, u32 max_port, u32 max_queues,
		   u32 reserved_queues, u32 synq_queues);
void qos_pools_clean(struct pp_qos_dev *qdev);

/* Cyclic buffer
 * Copy size bytes from buffer to command,
 * return amount of bytes read
 * cmd_queue_get removes read bytes from buffer while cmd_queue_peek isn't
 */
s32 cmd_queue_get(struct cmd_queue *q, void *cmd, size_t size);
s32 cmd_queue_peek(struct cmd_queue *q, void *_cmd, size_t size);
s32 cmd_queue_is_empty(struct cmd_queue *q);
size_t cmd_queue_watermark_get(struct cmd_queue *q);

/* Copy size bytes into cyclic buffer
 * return 0 on success, -1 if buffer has no for size bytes
 */
s32 cmd_queue_put(struct cmd_queue *q, void *cmd, size_t size);

/* Octets manipulation */

u32 octet_get_use_count(const struct pp_octets *octets, u32 octet);

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
u32 octet_get_with_at_least_free_entries(struct pp_octets *octets, u32 count);

/**
 * octet_get_least_free_entries() - The octet that has the least free entries
 * @octets:
 * Return:
 *
 * Note octet is removed from container, user should call
 * to octet_set_use_count (or nodes_move/octet_nodes_shift which call it) to
 * return it
 */
u32 octet_get_least_free_entries(struct pp_octets *octets);

/* Initialize qos dev, max port designates phy of highest port */
s32 qos_dev_init_db(struct pp_qos_dev *qdev, u32 max_ports, u32 max_queues,
		    u32 reserved_queues, u32 num_syncqs);
void qos_dev_clear_db(struct pp_qos_dev *qdev);

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
					     u32 octet,
					     const struct qos_node *spc_parent,
					     u32 *num_of_children);

/**
 * nodes_modify_used_status() - Set/Clear node's use status
 * @qdev:
 * @first_phy: First node's phy
 * @count:     Number of nodes
 * @status:    Desired status (0/1)
 */
void nodes_modify_used_status(struct pp_qos_dev *qdev, u32 first_phy, u32 count,
			      u32 status);

struct qos_node *get_conform_node(const struct pp_qos_dev *qdev,
				  u32 id,
				  bool (*conform)(const void *node));

/* Remove node
 * remove node and free its resources, assume node has no children
 */
s32 node_remove(struct pp_qos_dev *qdev, struct qos_node *node);

/* mark the node unused, update parents, id and rlm are not freed.
 * assume node has no children
 */
void node_phy_delete(struct pp_qos_dev *qdev, u32 phy);

u16 allocate_rlm(struct pp_qos_dev *qdev, u32 id, enum rlm_range range);
enum rlm_range rlm_get_range_type(struct pp_qos_dev *qdev, u32 rlm);
void release_rlm(struct pp_qos_dev *qdev, u32 rlm);

/* Return id to pool */
void release_id(struct pp_qos_dev *qdev, u32 id);

s32 node_flush(struct pp_qos_dev *qdev, struct qos_node *node);
s32 qos_queue_flush(struct pp_qos_dev *qdev, u32 id);
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
s32 node_cfg_valid(const struct pp_qos_dev *qdev, const struct qos_node *node,
		   const struct qos_node *orig_node, u32 prev_virt_parent_phy);

s32 get_node_prop(const struct pp_qos_dev *qdev, const struct qos_node *node,
		  struct pp_qos_common_node_properties *common,
		  struct pp_qos_parent_node_properties *parent,
		  struct pp_qos_child_node_properties *child);

s32 set_node_prop(struct pp_qos_dev *qdev, struct qos_node *node,
		  const struct pp_qos_common_node_properties *common,
		  const struct pp_qos_parent_node_properties *parent,
		  const struct pp_qos_child_node_properties *child,
		  u32 *modified);

/* Init node with default values
 * common, parent and child dictate which
 * components of node should be initialized
 */
void node_init(const struct pp_qos_dev *qdev, struct qos_node *node, u32 common,
	       u32 parent, u32 child);

/**
 * get_user_queues() - Return all queues on a subtree visible to the user
 * @qdev:
 * @phy:       Phy of subtree's node
 * @queue_ids: Array to store the queues ids - may be NULL
 * @size:      Size of array - may be 0
 * @queues_num: The number of queues on the subtree
 */
void get_user_queues(struct pp_qos_dev *qdev, u32 phy, u16 *queue_ids, u32 size,
		     u32 *queues_num);

/**
 * get_node_queues() - Return all queues on a subtree
 * @qdev:
 * @phy:       Phy of subtree's node
 * @queue_ids: Array to store the queues ids - may be NULL
 * @size:      Size of array - may be 0
 * @queues_num: The number of queues on the subtree
 */
void get_node_queues(struct pp_qos_dev *qdev, u32 phy, u16 *queue_ids, u32 size,
		     u32 *queues_num);

/**
 * get_dr_sched_id() - Retrieves the port's dual rate scheduler.
 * @qdev: handle to qos device instance obtained previously from qos_dev_init
 * @id:   Node id. Can be any id in the tree
 *
 * Return: dual rate scheduler ID
 */
u32 get_dr_sched_id(const struct pp_qos_dev *qdev, u32 id);

/**
 * update_bwl_buffer() - Update all bw limit node values in temp
 *        buffer
 * @qdev:
 * @phy:       Phy of subtree's node
 */
void update_bwl_buffer(struct pp_qos_dev *qdev, u32 phy);

/**
 * get_active_port_rlms() - Return all rlms on a subtree
 * @qdev:
 * @phy:       Phy of subtree's node
 * @rlms:      Array to store the rlms - may be NULL
 * @size:      Size of array - may be 0
 * @queues_num: The number of queues on the subtree
 */
void get_active_port_rlms(struct pp_qos_dev *qdev, u32 phy, u16 *rlms, u32 size,
			  u32 *queues_num);

/**
 * update_wsp_info() - Update enhanced WSP buffer
 * @qdev:
 */
void update_wsp_info(struct pp_qos_dev *qdev);

/**
 * is_q_ewsp() - Return true if queue is ewsp
 */
bool is_q_ewsp(struct pp_qos_dev *qdev, u32 id);

/**
 * get_num_blocked_queues() - Return ammount of non-blocked
 * queue on subtree
 * @qdev:
 * @phy:       Phy of subtree's node
 * @queues_num: The number of non-blocked queues on the subtree
 */
void get_num_nonblocked_queues(struct pp_qos_dev *qdev,
			       unsigned int phy,
			       unsigned int *queues_num);

/**
 * store_queue_list_max_allowed() - receives a queue list of
 *             which writes their max_allowed value in DDR
 * @qdev:
 * @rlms:      Array of physical queues
 * @queues_num: The number of queues on the subtree
 */
s32 store_queue_list_max_allowed(struct pp_qos_dev *qdev,
				 u16 *rlms, u32 queues_num);

/**
 * phy_alloc_by_parent() - Allocate new phy for a node
 * @qdev:
 * @parent:          Node's parent
 * @child_priority:  Relevant only for WSP parent
 * Return:
 */
u32 phy_alloc_by_parent(struct pp_qos_dev *qdev, struct qos_node *parent,
			u32 child_priority);

/* Return the first non internal ancestor of a node */
u32 get_virtual_parent_phy(const struct pp_nodes *nodes,
			   const struct qos_node *child);

void node_update_children(struct pp_qos_dev *qdev, struct qos_node *parent,
			  u32 new_phy);

s32 update_predecessors(struct pp_qos_dev *qdev, struct qos_node *node,
			void *data);
void tree_update_predecessors(struct pp_qos_dev *qdev, u32 phy);

/* Assume that sched is internal scheduler
 * 1. Update all sched's children to WSP scheduling
 */
void set_children_wsp_weight(struct pp_qos_dev *qdev,
			     const struct qos_node *parent);

u32 get_children_bandwidth_share(const struct pp_qos_dev *qdev,
				 const struct qos_node *parent);

/* Assume that sched is internal scheduler
 * 1. Update sched's virtual bandwidth share to be the sum of its
 *    children's share.
 * 2. If sched's parent is also internal scheduler
 *    update its virtual bandwidth share also to be the update
 *    bw share sum of its children. And keep doing that
 *    up the tree hierarchy so long parent is an internal scheduler
 * 3. For each
 */
void update_internal_bandwidth(struct pp_qos_dev *qdev, struct qos_node *sched);

/* Remove all nodes (including root) of a subtree */
s32 tree_remove(struct pp_qos_dev *qdev, u32 phy);

/* Flush all nodes (including root) of a subtree */
s32 tree_flush(struct pp_qos_dev *qdev, u32 phy);

/* Modify blocked status of queues on a subtree */
s32 tree_modify_blocked_status(struct pp_qos_dev *qdev, u32 phy, u32 status);

void get_bw_grp_members_under_node(struct pp_qos_dev *qdev, u32 id, u32 phy,
				   u16 *ids, u32 size, u32 *ids_num);

/* Creates pool of free ports phys */
struct pp_pool *free_ports_phys_init(u32 *reserved, u32 max_port,
				     const u32 *reserved_ports, u32 size,
				     u32 *num_reserved_ports);

/* return the port ancestor of a node */
u32 get_port(const struct pp_nodes *nodes, u32 phy);
u32 reserve_port(struct pp_qos_dev *qdev, u32 phy_port_id);
u32 unreserve_port(struct pp_qos_dev *qdev, u32 phy_port_id);

/* is port wsp - phy can be any node in the tree */
static inline bool is_port_wsp(const struct pp_nodes *nodes, u32 phy)
{
	const struct qos_node *node;

	if (!QOS_PHY_VALID(phy))
		return false;

	node = get_const_node_from_phy(nodes, phy);
	while (node_child(node))
		node = get_const_node_from_phy(nodes,
					       node->child_prop.parent_phy);

	QOS_ASSERT(node_port(node), "Did not reach port node for phy %u\n",
		   phy);
	return (node->parent_prop.arbitration == PP_QOS_ARBITRATION_WSP);
}

s32 port_cfg_valid(const struct pp_qos_dev *qdev, const struct qos_node *node,
		   const struct qos_node *orig_node);

s32 qos_device_ready(const struct pp_qos_dev *qdev);

s32 qos_dev_init(struct pp_qos_dev *qdev, struct pp_qos_init_param *conf,
		 bool create_debugfs);

void qos_dev_clean(struct pp_qos_dev *qos_dev, bool delete_debugfs);

s32 _pp_qos_queue_block(struct pp_qos_dev *qdev, u32 id);

s32 _pp_qos_queue_unblock(struct pp_qos_dev *qdev, u32 id);

s32 _pp_qos_queue_flush(struct pp_qos_dev *qdev, u32 id);

void update_cmd_id(struct driver_cmds *drvcmds);

s32 __must_check transmit_cmds(struct pp_qos_dev *qdev);

void update_children_position(struct pp_qos_dev *qdev, struct qos_node *child,
			      struct qos_node *parent,
			      struct qos_node *node_src);
s32 __qos_ddr_allocate(struct pp_qos_dev *qdev);
void __qos_ddr_free(struct pp_qos_dev *qdev);
s32 allocate_ddr_for_qm_on_platform(struct pp_qos_dev *qdev);

/**
 * @brief Copy from pp hw address using qos mcdma engine
 * @param qdev
 * @param src_phy physical source address
 * @param dst_phy destination address
 * @param size number of bytes to copy
 * @return 0 on success, error code otherwise
 */
s32 qos_uc_mcdma_copy_from_hw(struct pp_qos_dev *qdev, u32 src_phy, void *dst,
			      u32 size);

/**
 * @brief Copy to pp hw address using qos uC mcdma
 * @param qdev
 * @param src source address
 * @param dst_phy physical destination address
 * @param size number of bytes to copy
 * @return 0 on success, error code otherwise
 */
s32 qos_uc_mcdma_copy_to_hw(struct pp_qos_dev *qdev, void *src, u32 dst_phy,
			    u32 size);

/**
 * @brief Push qos descriptor to queue
 * @param qdev
 * @param queue_id Logical queue id
 * @param size packet size
 * @param pool buffer pool
 * @param policy buffer policy
 * @param color packet color
 * @param gpid tx port if
 * @param data_off data start offset
 * @param addr packet address
 */
void qos_descriptor_push(struct pp_qos_dev *qdev, u32 queue_id, u32 size,
			 u32 pool, u32 policy, u32 color, u32 gpid,
			 u32 data_off, u32 addr);

/**
 * @brief Pop qos descriptor from queue
 * @param qdev
 * @param queue_id Logical queue id
 */
void qos_descriptor_pop(struct pp_qos_dev *qdev, u32 queue_id, u32 *descriptor);

/**
 * @brief Set a new clock for the QoS driver and FW
 *        This update all necessary SW/FW/HW configuration which
 *        are based on the clock frequency
 * @note This doesn't change the clock, but ONLY update the configuration
 * @param qdev
 * @param clk_MHz new clock in mega hertz
 * @return s32 0 on success, error code otherwise
 */
s32 qos_clock_set(struct pp_qos_dev *qdev, u32 clk_MHz);

/**
 * @brief Get QOS drivers stats
 * @param qdev qos device
 * @param stats the returned stats
 * @return s32 0 on success, error code otherwise
 */
s32 qos_drv_stats_get(struct pp_qos_dev *qdev, struct qos_drv_stats *stats);

/**
 * @brief add a new sync queue
 * @note the sync queue will remain suspended after this API
 * @param qdev qos device
 * @param id sync queue id
 * @param rlm allocated queue phy id for the new sync queue
 * @param conf queue configuration
 * @return s32 0 on success, error code otherwise
 */
s32 qos_sync_queue_add(struct pp_qos_dev *qdev, u32 id, u32 *rlm,
		       const struct pp_qos_queue_conf *conf);

/**
 * @brief connect the sync queue to the destination queue
 * @param qdev qos device
 * @param id sync queue id
 * @param dstq_id destination queue id
 * @return s32 0 on success, error code otherwise
 */
s32 qos_sync_queue_connect(struct pp_qos_dev *qdev, u32 id, u32 dstq_id);

/**
 * @brief disconnect the sync queue from the QoS tree
 * @note the sync queue will remain suspended after this API
 * @param qdev qos device
 * @param id sync queue id
 * @return s32 0 on success, error code otherwise
 */
s32 qos_sync_queue_disconnect(struct pp_qos_dev *qdev, u32 id);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief qos debug init
 * @param qdev db structure
 * @return s32 0 for success, non-zero otherwise
 */
s32 qos_dbg_module_init(struct pp_qos_dev *qdev);

/**
 * @brief qos debug cleanup
 * @param qdev db structure
 * @return s32 0 for success, non-zero otherwise
 */
void qos_dbg_module_clean(struct pp_qos_dev *qdev);
#else
static inline s32 qos_dbg_module_init(struct pp_qos_dev *qdev)
{
	return 0;
}

static inline void qos_dbg_module_clean(struct pp_qos_dev *qdev)
{}
#endif

#endif
