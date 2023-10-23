/*
 * pp_qos_api.h
 * Description: PP QoS APIs and definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only or BSD-3-Clause
 * Copyright (C) 2018-2019 Intel Corporation
 */
#ifndef __PP_QOS_API_H__
#define __PP_QOS_API_H__

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

/**
 * @file
 * @brief QoS API
 */

/******************************************************************************/
/*                                 GENERAL                                    */
/******************************************************************************/

/**
 * @define Instance id for ppv4 qos
 */
#define PP_QOS_INSTANCE_ID              (0)

/**
 * @define Invalid logical queue id
 */
#define PP_QOS_INVALID_ID               (U16_MAX)

/**
 * @define Maximum ports support by HW
 */
#define PP_QOS_MAX_PORTS                (256)

/**
 * @page Calling context
 *        The API exported by this driver, must be called only
 *        from a thread context. Calling them from either
 *        interrupt/tasklet/buttom half is not allowed and will
 *        result in unexpected behaviour.
 */

/**
 * @page Scheduling model
 *
 * Ports, schedulers and queues are modeled as a scheduling tree's nodes.
 * Each such node properties are classified to the following categories:
 * - Common properties - relevant to all nodes
 * - Parent properties - relevant only to parent nodes e.g. arbitration scheme
 *                       between children
 * - Children properties - relevant only to child nodes e.g. node's parent
 * some nodes can be both parents and children e.g. schedulers
 */

/**
 * @page Arbitration
 *
 * The way a parent arbitrates between its children can be either strict
 * priority or round robin.
 *
 * A WRR parent round robin its children - each child gets its turn - as if all
 * children have the same priority. The number of children for this parent type
 * is limited only by the number of nodes (> 1024)
 *
 * A WSP parent employs strict priority arbitration on its children - a child
 * will get scheduled only if there is no other sibling with higher priority
 * that can be scheduled. A WSP parent can have 8 direct children at most, each
 * child must have a different priority. To configure more that 8 children for
 * a WSP parent, or to have several children with the same priority,
 * intermediate schedulers should be use.
 *
 * Example of 5 queues q0, q1, q2, q3, q4 with priorities 0, 1, 2, 3, 3
 *
 *                     Port arbitrating WSP
 *                     /   |    |     \
 *                    q0   q1  q2  Sched arbitrating WRR
 *                                         |	      |
 *                                        q3          q4
 *
 * Example of 16 queues q0 to q15, each with a different priority
 *
 *		      Port arbitrating WSP
 *		         |	       |
 *	    Sched0 arbitrating WSP   Sched1 arbitrating WSP
 *	       /               \       /                   \
 *	   q0 q1 q2 q3 q4 q5 q6 q7  q8 q9 q10 q11 q12 q13 q4 q15
 */

/**
 * @enum pp_qos_arbitration
 * @brief parent method for arbitrating its children on tree
 */
enum pp_qos_arbitration {
	/*!< Strict priority      */
	PP_QOS_ARBITRATION_WSP,

	/*!< Weighted round robin */
	PP_QOS_ARBITRATION_WRR,

	/*!< Weighted fair queue  */
	PP_QOS_ARBITRATION_WFQ
};

/**
 * @struct pp_qos_common_node_properties
 * @brief Properties common to all nodes
 */
struct pp_qos_common_node_properties {
#define QOS_NO_BANDWIDTH_LIMIT          0
#define QOS_MAX_BANDWIDTH_LIMIT         0xA00000 /* 10Gbps */
	/*! bandwidth limit in Kbps */
	u32 bandwidth_limit;

#define QOS_NO_SHARED_BANDWIDTH_GROUP   0x0
#define QOS_MAX_SHARED_BANDWIDTH_GROUP  511
	/*! shared bandwidth group */
	u32 shared_bw_group;

#define QOS_DEAFULT_MAX_BURST           2 /* 4 Quantas */
#define QOS_MAX_MAX_BURST               7
	/*! Defines the max quantas that can be accumulated
	 * num quantas = 1 << (max_burst)
	 */
	u32 max_burst;
};

/**
 * @struct pp_qos_parent_node_properties
 * @brief Properties relevant for parent node
 */
struct pp_qos_parent_node_properties {
	/*! how parent node arbitrates between its children */
	enum pp_qos_arbitration arbitration;

	/*! whether best effort scheduling is enabled */
	/*! for this node's children */
	s32  best_effort_enable;
};

/**
 * @struct pp_qos_child_node_properties
 * @brief Properties relevant for child node
 */
struct pp_qos_child_node_properties {
	/*! parent's id */
	u32 parent;

#define QOS_MAX_CHILD_PRIORITY 7
	/*! strict priority, relevant only if parent uses wsp arbitration */
	u32 priority;

#define QOS_MIN_CHILD_WRR_WEIGHT 1
#define QOS_MAX_CHILD_WRR_WEIGHT 126
	/*! For WRR, child's weight.
	* For example, in case parent has 3 children
	* setting weights of 2-1-1 for the children means that the first child
	* will have double credits to transmit comparing to other two children.
	* range is 1 - 126
	*/
	u32 wrr_weight;
};

struct pp_qos_dev;

/******************************************************************************/
/*                                      PORTS                                 */
/******************************************************************************/

/**
 * @struct pp_qos_port_stat
 * @brief Statistics per port
 */
struct pp_qos_port_stat {
	/*! Clear statistics after read */
	s32 reset;

	/*! Total green bytes currently in all port queues */
	u32 total_green_bytes;

	/*! Total yellow bytes currently in all port queues */
	u32 total_yellow_bytes;

	/*! Port back pressure status */
	u32 debug_back_pressure_status;

	/*! Actual packet credit */
	u32 debug_actual_packet_credit;

	/*! Actual byte credit */
	u32 debug_actual_byte_credit;
};

/**
 * @struct pp_qos_port_conf
 * @brief Configuration structure for port
 */
struct pp_qos_port_conf {
	/*! common properties */
	struct pp_qos_common_node_properties common_prop;

	/*! properties as a parent */
	struct pp_qos_parent_node_properties port_parent_prop;

	/*! Ring address */
	unsigned long ring_address;

	/*! Ring size */
	u32 ring_size;

	/*! packet credit mode (byte credit disabled) */
	s32 packet_credit_enable;

	/*! amount of credit to add to the port. when packet_credit is */
	/*! enabled this designates packet credit, otherwise byte credit */
	u32 credit;

	/*! disable port transmition */
	s32 disable;

	/*! Egress green bytes threshold */
	u32 green_threshold;

	/*! Egress yellow bytes threshold */
	u32 yellow_threshold;

	/*! Enhanced WSP operation */
	u32 enhanced_wsp;
};

/**
 * @struct pp_qos_port_info
 * @brief Port's information
 */
struct pp_qos_port_info {
	u32 physical_id;   /*! node id on scheduling tree */
	u32 num_of_queues; /*! number of queues feeds this port */
};

#define PP_QOS_MAX_INSTANCES (1)

/**
 * @brief Init port configuration with default values
 * @param conf pointer to client allocated struct. This struct
 *        will be filled with default values.
 *
 * Defaults values:
 *    bandwidth_limit - QOS_NO_BANDWIDTH_LIMIT
 *    shared_bandwidth_limit - QOS_NO_SHARED_BANDWIDTH_GROUP
 *    arbitration - WSP
 *    best_effort_enable - no
 *    packet_credit_enable - yes
 *    byte_credit_enable - no
 *    packet_credit - 0
 *    byte_credit - 0
 *    ring_size - 0
 *    ring_address - 0
 *    green_threshold - 0xFFFFFFFF
 *    yellow_threshold - 0xFFFFFFFF
 */
void pp_qos_port_conf_set_default(struct pp_qos_port_conf *conf);

#define ALLOC_PORT_ID 0xFFFF
/**
 * @brief Allocate a resource for a new port
 * @param qos_dev handle to qos device instance obtained
 *        previously from pp_qos_dev_open
 * @param physical_id if equal ALLOC_PORT_ID then library
 *        allocates a free port id for the new port, otherwise
 *        must be one of the reserved ports ids that were
 *        configured at qos_init
 * @param upon success holds new port's id
 *
 * @return 0 on success.
 */
s32 pp_qos_port_allocate(struct pp_qos_dev *qos_dev, u32 physical_id, u32 *id);

/**
 * @brief Remove port
 * @param qos_dev handle to qos hardware instance.
 * @param id port's id obtained from port_allocate
 *
 * @Return: 0 on success.
 */
s32 pp_qos_port_remove(struct pp_qos_dev *qos_dev, u32 id);

/**
 * @brief Set port configuration
 * @param qos_dev handle to qos device instance obtained
 *         previously from qos_dev_init
 * @param id port's id obtained from port_allocate
 * @param conf port configuration.
 * @return 0 on success
 */
s32 pp_qos_port_set(struct pp_qos_dev *qos_dev, u32 id,
		    const struct pp_qos_port_conf *conf);

/**
 * @brief Remove port desendants from scheduling
 * @param qos_dev handle to qos hardware instance.
 * @param id port's id obtained from port_allocate
 * @return 0 on success
 */
s32 pp_qos_port_disable(struct pp_qos_dev *qos_dev, u32 id);

/**
 * @brief Resume port and its decendants scheduling
 * @param qos_dev handle to qos hardware instance.
 * @param id port's id obtained from port_allocate
 * @return 0 on success
 */
s32 pp_qos_port_enable(struct pp_qos_dev *qos_dev, u32 id);

/**
 * @brief All new packets enqueued to queues feeding this port
 *        will be dropped
 * @param qos_dev handle to qos hardware instance.
 * @param id port's id obtained from port_allocate
 * @note already enportd descriptors will be transmitted
 * @return 0 on success
 */
s32 pp_qos_port_block(struct pp_qos_dev *qos_dev, u32 id);

/**
 * @brief Unblock enqueuing of new packets to queues that feeds
 *        this port
 * @param qos_dev handle to qos hardware instance.
 * @param id port's id obtained from port_allocate
 * @return 0 on success
 */
s32 pp_qos_port_unblock(struct pp_qos_dev *qos_dev, u32 id);

/**
 * @brief Get port configuration
 * @param qos_dev handle to qos device instance obtained from
 * @param id port's id obtained from port_allocate
 * @param conf pointer to struct to be filled with port's
 *        configuration
 * @return 0 on success
 */

s32 pp_qos_port_conf_get(struct pp_qos_dev *qos_dev, u32 id,
			 struct pp_qos_port_conf *conf);

/**
 * @brief Get extra information about port
 * @param id port's id obtained from port_allocate
 * @param info pointer to struct to be filled with port's info
 * @return 0 on success
 */
s32 pp_qos_port_info_get(struct pp_qos_dev *qos_dev, u32 id,
			 struct pp_qos_port_info *info);

/**
 * @brief Get all port's queues
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id scheduler's id obtained from sched_allocate
 * @param queue_ids client allocated array that will hold queues
 *        ids
 * @param size size of queue_ids
 * @param queues_num holds the number of queues
 * @note queues_num can be bigger than size
 * @return 0 on success
 */
s32 pp_qos_port_get_queues(struct pp_qos_dev *qos_dev, u32 id,
			   uint16_t *queue_ids, u32 size, u32 *queues_num);

/**
 * @brief Get port's statistics
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id port's id obtained from port_allocate
 * @param stat pointer to struct to be filled with port's
 *        statistics
 * @return 0 on success
 */
s32 pp_qos_port_stat_get(struct pp_qos_dev *qos_dev, u32 id,
			 struct pp_qos_port_stat *stat);

/**
 * @brief Get logical port id
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param physical_id port's physical id allocated at
 *        pp_qos_port_set
 * @return queue logical id or PP_QOS_INVALID_ID on failure
 */
u32 pp_qos_port_id_get(struct pp_qos_dev *qos_dev, u32 physical_id);

/******************************************************************************/
/*                                    QUEUES                                  */
/******************************************************************************/

/**
 * @struct pp_qos_queue_stat
 * @brief Statistics per queue
 */
struct pp_qos_queue_stat {
	/*! Reset after reading */
	s32 reset;

	/*! Packets currently in queue */
	u32 queue_packets_occupancy;

	/*! Bytes currently in queue */
	u32 queue_bytes_occupancy;

	/*! WRED packets accepted */
	u32 total_packets_accepted;

	/*! WRED (any reason) Packet dropped */
	u32 total_packets_dropped;

	/*! WRED Packet dropped (red) */
	u32 total_packets_red_dropped;

	/*! WRED bytes accepted */
	u64 total_bytes_accepted;

	/*! WRED bytes dropped */
	u64 total_bytes_dropped;
};

/**
 * @struct pp_qos_queue_conf
 * @brief Configuration structure for queue
 */
struct pp_qos_queue_conf {
	/*! common properties */
	struct pp_qos_common_node_properties common_prop;

	/*! properties as a child node */
	struct pp_qos_child_node_properties  queue_child_prop;

	/*! queue will drop new enqueued packets */
	s32 blocked;

	/*! WRED is applied on this queue, */
	/*! otherwise only min and max values are used */
	s32 wred_enable;

	/*! use fixed drop probability for WRED instead of slope */
	s32 wred_fixed_drop_prob_enable;

	/*! Start of the slope area (Below that no drops) */
	u32 wred_min_avg_green;

	/*! End of the slope area (Above that 100% drops) */
	u32 wred_max_avg_green;

	/*! 0-100 scale */
	u32 wred_slope_green;

	/*! Fixed drop rate in between min & max */
	u32 wred_fixed_drop_prob_green;

	/*! Start of the slope area (Below that no drops) */
	u32 wred_min_avg_yellow;

	/*! End of the slope area (Above that 100% drops) */
	u32 wred_max_avg_yellow;

	/*! 0-100 scale */
	u32 wred_slope_yellow;

	/*! Fixed drop rate in between min & max */
	u32 wred_fixed_drop_prob_yellow;

	/*! Minimum descriptors guaranteed */
	u32 wred_min_guaranteed;

	/*! Maximum descriptors allowed in this queue */
	u32 wred_max_allowed;

	/*! codel enable */
	s32 codel_en;

	/*! Excess inforation rate
	 * User can set an extra bw per queue, which will be applied only
	 * after serving all "regular" queues in tree
	 */
	u32 eir;

	/*! Excess burst size
	 * Defines the max quantas that can be accumulated
	 * for the excess node (eir)
	 * max num quantas = 1 << (max_excess_burst)
	 */
	u32 ebs;
};

/**
 * @struct pp_qos_queue_info
 * @brief Queue information struct
 */
struct pp_qos_queue_info {
	/*! port fed by this queue */
	u32 port_id;

	/*! queue (ready list) number */
	u32 physical_id;
};

#define PP_QOS_AQM_CONTEXT_MAX_QUEUES (8)
#define PP_QOS_AQM_MAX_SERVICE_FLOWS  (16)

/**
 * @struct pp_qos_aqm_sf_config
 * @brief AQM service flow configuration
 */
struct pp_qos_aqm_sf_config {
	/*! AQM Latency Target for this Service Flow (milliseconds) */
	u32 latency_target_ms;

	/*! Service Flow configured Peak Rate, expressed in Bytes/sec */
	u32 peak_rate;

	/*! Service Flow configured MaxSustained Rate expressed in Bytes/sec */
	u32 msr;

	/*! The size (in bytes) of the buffer for this Service Flow */
	u32 buffer_size;

	/*! number of queues attached to this context */
	u32 num_queues;

	/*! array of logical queue id's attached to this context */
	u32 queue_id[PP_QOS_AQM_CONTEXT_MAX_QUEUES];
};

#define PP_QOSCODEL_MAX_CONSECUTIVE_DROP_LIMIT (0x7FFF)

/**
 * @struct pp_qos_codel_cfg
 * @brief CoDel configuration
 */
struct pp_qos_codel_cfg {
	/*! Maximum consecutive drops allowed */
	u32 consecutive_drop_limit;

	/*! Target delay in mSec */
	u32 target_delay_msec;

	/*! Interval time mSec */
	u32 interval_time_msec;
};

/**
 * @struct pp_qos_codel_stat
 * @brief Statistics per CoDel queue
 */
struct pp_qos_codel_stat {
	/*! Reset after reading */
	s32 reset;

	/*! Packets dropped by CoDel */
	u32 packets_dropped;

	/*! Bytes dropped by CoDel */
	u32 bytes_dropped;

	/*! Minimum sojourn time */
	u32 min_sojourn_time;

	/*! Maximum sojourn time */
	u32 max_sojourn_time;

	/*! Total sojourn time */
	u32 total_sojourn_time;

	/*! Maximum packet size */
	u32 max_packet_size;

	/*! Total number of packets */
	u32 total_packets;
};

/**
 * @brief Init configuration with default values
 * @param conf pointer to client allocated struct. This struct
 *        will be filled with default values.
 *
 * Defaults values:
 *    bandwidth_limit - 0
 *    shared_bandwidth_limit - 0
 *    parent - 0
 *    priority - 0
 *    bandwidth_share - 0
 *    wred_enable - no
 *    ALL wred params - 0
 *    codel_en - 0
 */
void pp_qos_queue_conf_set_default(struct pp_qos_queue_conf *conf);

/**
 * @brief Allocate resources for a new queue
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id upon success hold's new queue id
 * @return 0 on success
 */
s32 pp_qos_queue_allocate(struct pp_qos_dev *qos_dev, u32 *id);

/**
 * @brief Allocate resources for a new queue
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id upon success hold's new queue id
 * @param phy upon success hold's new physical queue id
 * @return 0 on success
 */
s32 pp_qos_queue_allocate_id_phy(struct pp_qos_dev *qdev, u32 *id, u32 *phy);

/**
 * @brief Remove a queue
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id queue's id obtained from queue_allocate
 *
 * @note client should make sure that queue is empty and that
 *        new packets are not enqueued, by calling
 *        pp_qos_queue_disable
 * @return 0 on success
 */
s32 pp_qos_queue_remove(struct pp_qos_dev *qos_dev, u32 id);

/**
 * @brief Allocate resources for new contiguous queues
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param ids upon success hold's new queue id's list
 * @param phy_ids upon success hold's new physical queue id's list
 * @param count Amount of contiguous queues to allocate
 * @note Amount of queues must be as defined in dts
 * @return 0 on success
 */
s32 pp_qos_contiguous_queue_allocate(struct pp_qos_dev *qos_dev, u32 *ids,
				     u32 *phy_ids, u32 count);

/**
 * @brief Remove all contiguous queues
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id holds the first queue id obtained by alloc function
 * @param count Amount of contiguous queues to remove
 * @note Amount of queues must be as defined in dts
 * @return 0 on success
 */
s32 pp_qos_contiguous_queue_remove(struct pp_qos_dev *qos_dev, u32 id,
				   u32 count);

/**
 * @brief Set queue configuration
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id queue's id obtained from queue_allocate
 * @param conf new configuration for the queue
 * @return 0 on success
 */
s32 pp_qos_queue_set(struct pp_qos_dev *qos_dev, u32 id,
		     const struct pp_qos_queue_conf *conf);

/**
 * @brief All new packets enqueue to this queue will be dropped
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id queue's id obtained from queue_allocate
 * @note already enqueued descriptors will be transmitted
 * @return 0 on success
 */
s32 pp_qos_queue_block(struct pp_qos_dev *qos_dev, u32 id);

/**
 * @brief Unblock enqueuing of new packets
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id queue's id obtained from queue_allocate
 * @return 0 on success
 */
s32 pp_qos_queue_unblock(struct pp_qos_dev *qos_dev, u32 id);

/**
 * @brief Get queue current configuration
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open id: port's id obtained from
 *        queue_allocate
 * @param conf pointer to struct to be filled with queue's
 *        configuration
 * @return 0 on success
 */
s32 pp_qos_queue_conf_get(struct pp_qos_dev *qos_dev, u32 id,
			  struct pp_qos_queue_conf *conf);

/**
 * @brief Get information about queue
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id queue's id obtained from queue_allocate
 * @param info pointer to struct to be filled with queue's info
 * @return 0 on success
 */
s32 pp_qos_queue_info_get(struct pp_qos_dev *qos_dev, u32 id,
			  struct pp_qos_queue_info *info);

/**
 * @brief Get logical queue id
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param physical_id queue's physical id allocated at
 *        pp_qos_queue_set
 * @return queue logical id or PP_QOS_INVALID_ID on failure
 */
u32 pp_qos_queue_id_get(struct pp_qos_dev *qos_dev, u32 physical_id);

/**
 * @brief Get queue's statistics
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id queue's id obtained from queue_allocate
 * @param stat pointer to struct to be filled with queue's
 *        statistics
 * @return 0 on success
 */
s32 pp_qos_queue_stat_get(struct pp_qos_dev *qos_dev, u32 id,
			  struct pp_qos_queue_stat *stat);

/**
 * @brief Set Aqm service flow configuration
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param sf_id service flow id
 * @param sf_cfg service flow configuration
 * @return 0 on success
 */
s32 pp_qos_aqm_sf_set(struct pp_qos_dev *qos_dev, u8 sf_id,
		      struct pp_qos_aqm_sf_config *sf_cfg);

/**
 * @brief Set CoDel configuration
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param cfg CoDel configuration
 * @return 0 on success
 */
s32 pp_qos_codel_cfg_set(struct pp_qos_dev *qdev, struct pp_qos_codel_cfg *cfg);

/**
 * @brief Get CoDel queue statistics
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id queue's id obtained from queue_allocate
 * @param stat pointer to struct to be filled with queue's
 *        CoDel statistics
 * @return 0 on success
 */
s32 pp_qos_codel_queue_stat_get(struct pp_qos_dev *qdev, u32 id,
				struct pp_qos_codel_stat *stat);

/**
 * @brief Remove Aqm service flow
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param sf_id service flow id
 * @return 0 on success
 */
s32 pp_qos_aqm_sf_remove(struct pp_qos_dev *qos_dev, u8 sf_id);

/******************************************************************************/
/*                                 SCHEDULER                                  */
/******************************************************************************/

/**
 * @struct pp_qos_sched_conf
 * @brief Configuration structure for scheduler
 */
struct pp_qos_sched_conf {
	/*! common properties as a node */
	struct pp_qos_common_node_properties common_prop;

	/*! properties as a parent node */
	struct pp_qos_parent_node_properties sched_parent_prop;

	/*! properties as a child node */
	struct pp_qos_child_node_properties  sched_child_prop;
};

/**
 * @struct pp_qos_sched_info
 * @brief Scheduler information struct
 */
struct pp_qos_sched_info {
	u32 port_id;       /*! port fed by this scheduler */
	u32 num_of_queues; /*! number of queues feed this scheduler */
};

/**
 * @brief Init scheduler config with defaults
 * @param conf pointer to client allocated struct which will be
 *        filled with default values
 *
 * Defaults values:
 *    bandwidth_limit - 0
 *    shared_bandwidth_limit - 0
 *    arbitration - WSP
 *    best_effort_enable - no
 *    parent - 0
 *    priority - 0
 *    bandwidth_share - 0
 */
void pp_qos_sched_conf_set_default(struct pp_qos_sched_conf *conf);

/**
 * @brief Allocate a resource for a new scheduler
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id upon success - holds new scheduler's id
 * @return  0 on success
 */
s32 pp_qos_sched_allocate(struct pp_qos_dev *qos_dev, u32 *id);

/**
 * @brief Remove scheduler
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id scheduler's id obtained from sched_allocate
 * @return 0 on success
 */
s32 pp_qos_sched_remove(struct pp_qos_dev *qos_dev, u32 id);

/**
 * @brief Set scheduler configuration
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id scheduler's id obtained from sched_allocate
 * @param conf new configuration for the scheduler
 * @note must init conf by pp_qos_sched_conf_init before calling
 *        this function.
 * @return 0 on success
 */
s32 pp_qos_sched_set(struct pp_qos_dev *qos_dev, u32 id,
		     const struct pp_qos_sched_conf *conf);

/**
 * @brief Get sched current configuration
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open id: port's id obtained from
 *        port_allocate
 * @param conf pointer to struct to be filled with sched's
 *        configuration
 * @return 0 on success
 */
s32 pp_qos_sched_conf_get(struct pp_qos_dev *qos_dev, u32 id,
			  struct pp_qos_sched_conf *conf);

/**
 * @brief Get information about scheduler
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id scheduler's id obtained from sched_allocate
 * @param info pointer to struct to be filled with scheduler's
 *        info
 * @return 0 on success
 */
s32 pp_qos_sched_info_get(struct pp_qos_dev *qos_dev, u32 id,
			  struct pp_qos_sched_info *info);

/**
 * @brief Get all scheduler's queues
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id scheduler's id obtained from sched_allocate
 * @param queue_ids client allocated array that will hold
 *        queues ids
 * @param size size of queue_ids
 * @param queues_num holds the number of queues
 * @note queues_num can be bigger than size
 * @return 0 on success
 */
s32 pp_qos_sched_get_queues(struct pp_qos_dev *qos_dev, u32 id,
			    uint16_t *queue_ids, u32 size, u32 *queues_num);

/******************************************************************************/
/*                                 SHARED_LIMIT                               */
/******************************************************************************/

/**
 * @brief Add new shared bandwidth group
 * @param qos_dev handle to qos device instance obtained from pp_qos_dev_open
 * @param max_burst num quantas = 1 << (max_burst)
 * @param id upon success, new group's id
 * @return 0 on success
 */
s32 pp_qos_shared_limit_group_add(struct pp_qos_dev *qos_dev, u32 max_burst,
				  u32 *id);

/**
 * @brief Remove shared bandwidth group
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id group's id previously obtained when adding group
 * @return 0 on success
 */
s32 pp_qos_shared_limit_group_remove(struct pp_qos_dev *qos_dev, u32 id);

/**
 * @brief Modify shared bandwidth group
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id group's id previously obtained when adding group
 * @param max_burst num quantas = 1 << (max_burst)
 * @return 0 on success
 */
s32 pp_qos_shared_limit_group_modify(struct pp_qos_dev *qos_dev, u32 id,
				     u32 max_burst);

/**
 * @brief Get group's members
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param id shared bandwidth group id
 * @param members client allocated array that will hold ids
 *        members
 * @param size size of ids
 * @param members_num holds the number of members
 * @note members_num can be bigger than size function iterates
 *        over tree so it is rather slow
 * @return 0 on success
 */
s32 pp_qos_shared_limit_group_get_members(struct pp_qos_dev *qos_dev,
					  u32 id, uint16_t *members,
					  u32 size, u32 *members_num);

/******************************************************************************/
/*                                  INIT	                              */
/******************************************************************************/

/**
 * @brief Initialize of a qos device
 * @param id Identify the requested qos instance (same id that
 *        designates the instance in the device tree)
 * @return on success handle to qos device
 *        NULL otherwise
 */
struct pp_qos_dev *pp_qos_dev_open(u32 id);

void pp_qos_mgr_init(void);
void pp_qos_mgr_exit(void);

/**
 * @brief get fw version
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param major storage for major part of version
 * @param minor storage for minor part of version
 * @param build storage for build part of version
 * @return 0 on success
 */
s32 pp_qos_get_fw_version(struct pp_qos_dev *qos_dev, u32 *major, u32 *minor,
			  u32 *build);

enum pp_qos_node_type {
	PPV4_QOS_NODE_TYPE_PORT,
	PPV4_QOS_NODE_TYPE_SCHED,
	PPV4_QOS_NODE_TYPE_QUEUE,
};

#define PPV4_QOS_INVALID (-1U)

/**
 * @struct pp_qos_node_num
 */
struct pp_qos_node_num {
	/*! Physical node on scheduler's node table */
	u32 phy;

	/*! Logical id seen by client */
	u32 id;
};

struct pp_qos_stats {
	u64 pkts_rcvd;
	u64 pkts_dropped;
	u64 wred_pkts_dropped;
	u64 codel_pkts_dropped;
	u64 pkts_transmit;
	u64 bytes_rcvd;
	u64 bytes_dropped;
	u64 bytes_transmit;
};

/**
 * @brief Get the packet statistics
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param stats
 * @return s32 0 on success, error code otherwise
 */
s32 pp_qos_stats_get(struct pp_qos_dev *qos_dev, struct pp_qos_stats *stats);

/**
 * @brief Reset all Qos queues statistics
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @return s32 0 on success, error code otherwise
 */
s32 pp_qos_stats_reset(struct pp_qos_dev *qos_dev);

/**
 * @brief Gets QoS quanta (In MB). QoS credits calculations are
 *        done in Quanta scale. QoS will transmit a
 *        multiplication of a quanta in one iteration
 * @param qos_dev handle to qos device instance obtained from
 *        pp_qos_dev_open
 * @param quanta pointer which the quanta value will be written
 *        to
 * @return s32 0 on success, error code otherwise
 */
s32 pp_qos_get_quanta(struct pp_qos_dev *qdev, unsigned int *quanta);

#endif /* __PP_QOS_API_H__ */
