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
#ifndef _PP_QOS_API_H
#define _PP_QOS_API_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif


/******************************************************************************/
/*                                 GENERAL                                    */
/******************************************************************************/

/**
 * DOC: Calling context
 *
 * The API exported by this driver, must be called only from a thread context.
 * Calling them from either interrupt/tasklet/buttom half is not allowed and
 * will result in unexpexted behaviour.
 */

/**
 * DOC: Scheduling model
 *
 * Ports, schedulers and queues are modeled as a scheduling tree's nodes.
 * Each such node properties are classified to the following categories:
 * - Common properties - relevant to all nodes
 * - Parent properties - relevant only to parent nodes e.g. arbitration scheme
 *                       between children
 * - Children properties - relevant only to child nodes e.g. node's parent
 *
 * Note some nodes can be both parents and children e.g. schedulers
 */

/**
 * DOC: Arbitration
 *
 * The way a parent arbitrates between its children can be either strict prioriy
 * or round robin.
 *
 * A WRR parent round robin its children - each child gets its turn - as if all
 * children have the same priority. The number of children for this parent type
 * is limited only by the number of nodes (> 1024)
 *
 * A WSP parent employs strict priorityi arbitration on its children - a child
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
 * enum pp_qos_arbitration - parent method for arbitrating its children on tree.
 * @PP_QOS_ARBITRATION_WSP: strict priority
 * @PP_QOS_ARBITRATION_WRR: round robin
 * @PP_QOS_ARBITRATION_WFQ:
 */
enum pp_qos_arbitration {
	PP_QOS_ARBITRATION_WSP,
	PP_QOS_ARBITRATION_WRR,
	PP_QOS_ARBITRATION_WFQ,
};


/**
 * struct pp_qos_common_node_properties - Properties common to all nodes
 * @bandwidth_limit:	    bandwidth limit in Kbps
 * @shared_bandwidth_group: shared bandwidth group
 */
struct pp_qos_common_node_properties {
	#define QOS_NO_BANDWIDTH_LIMIT          0
	#define QOS_MAX_BANDWIDTH_LIMIT         0xFFFF
	unsigned int bandwidth_limit;

	#define QOS_NO_SHARED_BANDWIDTH_GROUP   0x0
	#define QOS_MAX_SHARED_BANDWIDTH_GROUP  511
	unsigned int shared_bandwidth_group;
};


/**
 * @struct pp_qos_parent_node_properties - Properties relevant for parent node
 * @arbitration:	how parent node arbitrates between its children
 * @best_effort_enable: whether best effort scheduling is enabled for this
 *                      node's children.
 */
struct pp_qos_parent_node_properties {
	enum pp_qos_arbitration arbitration;
	int  best_effort_enable;
};


/**
 * @struct pp_qos_child_node_properties - Properties relevant for child node
 * @parent:		parent's id
 * @priority:		strict priority, relevant only if parent uses wsp
 *                      arbitration
 * @wrr_weight:		For WRR, child's weight.
 *                      For example, in case parent has 3
 *                      children setting weights of 2-1-1 for
 *                      the children means that the first child
 *                      will have double credits to transmit
 *                      comparing to other two children. Max 127
 */
struct pp_qos_child_node_properties {
	unsigned int	parent;
	#define QOS_MAX_CHILD_PRIORITY 7
	unsigned int	priority;
	unsigned int	wrr_weight;
};



struct pp_qos_dev;

/******************************************************************************/
/*                                      PORTS                                 */
/******************************************************************************/

/**
 * @struct pp_qos_port_stat - Statistics per port
 * @total_green_bytes - total green bytes currently in all queues
 *			feeding this port
 * @total_yellow_bytes - total yellow bytes currently in all queues
 *			 feeding this port
 * @debug_back_pressure_status - Port back pressure status
 * @debug_actual_packet_credit - Actual packet credit
 * @debug_actual_byte_credit - Actual byte credit
 */
struct pp_qos_port_stat {
	int reset;
	unsigned int total_green_bytes;
	unsigned int total_yellow_bytes;
	unsigned int debug_back_pressure_status;
	unsigned int debug_actual_packet_credit;
	unsigned int debug_actual_byte_credit;
};

/**
 * @struct pp_qos_port_conf - Configuration structure for port
 * @common_prop:		common properties
 * @port_parent_prop:		properties as a parent
 * @ring_address:		address of ring.
 * @ring_size:			size of ring.
 * @packet_credit_enable:	packet credit mode (byte credit disabled).
 * @credit:		        amount of credit to add to the port.
 *                              when packet_credit is enabled this designates
 *                              packet credit, otherwise byte credit
 * @disable			disable port transmission
 * @green_threshold		Egress green bytes threshold
 * @yellow_threshold		Egress yellow bytes threshold
 */
struct pp_qos_port_conf {
	struct pp_qos_common_node_properties common_prop;
	struct pp_qos_parent_node_properties port_parent_prop;
	void     *ring_address;
	unsigned int ring_size;
	int      packet_credit_enable;
	unsigned int credit;
	int	disable;
	unsigned int green_threshold;
	unsigned int yellow_threshold;
};


/**
 * @struct pp_qos_port_info -  Port's information
 * @physical_id:   node id on scheduling tree.
 * @num_of_queues: number of queues feeds this port.
 */
struct pp_qos_port_info {
	unsigned int physical_id;
	unsigned int num_of_queues;
};


/**
 * pp_qos_port_conf_set_default() - Init port configuration with default values
 * @conf: pointer to client allocated struct. This struct will be filled with
 * default values.
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
 * pp_qos_port_allocate() - Allocate a resource for a new port
 * @qos_dev:     handle to qos device instance obtained previously
 *               from pp_qos_dev_open
 * @physical_id: if equal ALLOC_PORT_ID then library allocates
 *               a free port id for the new port, otherwise must
 *               be one of the reserved ports ids that were
 *               configured at qos_init
 * @id:          upon success holds new port's id
 *
 * @Return:  0 on success.
 */
int pp_qos_port_allocate(struct pp_qos_dev *qos_dev, unsigned int physical_id,
			 unsigned int *id);


/**
 * pp_qos_port_remove() - Remove port
 * @qos_dev: handle to qos hardware instance.
 * @id: port's id obtained from port_allocate
 *
 * @Return: 0 on success.
 */
int pp_qos_port_remove(struct pp_qos_dev *qos_dev, unsigned int id);


/**
 * pp_qos_port_set() - Set port configuration
 * @qos_dev:   handle to qos device instance obtained previously from
 *         qos_dev_init
 * @id:    port's id obtained from port_allocate
 * @conf:  port configuration.
 *
 * @Return: 0 on success
 */
int pp_qos_port_set(struct pp_qos_dev *qos_dev, unsigned int id,
		    const struct pp_qos_port_conf *conf);


/**
 * pp_qos_port_disable() - Remove port desendants from scheduling
 * @qos_dev: handle to qos hardware instance.
 * @id:  port's id obtained from port_allocate
 *
 * @Return: 0 on success
 */
int pp_qos_port_disable(struct pp_qos_dev *qos_dev, unsigned int id);


/**
 * pp_qos_port_enable() - Resume port and its decendants scheduling
 * @qos_dev: handle to qos hardware instance.
 * @id:  port's id obtained from port_allocate
 *
 * @Return: 0 on success
 */
int pp_qos_port_enable(struct pp_qos_dev *qos_dev, unsigned int id);

/**
 * pp_qos_port_block() - All new packets enqueued to queues feeding this port
 *                    will be dropped
 * @qos_dev: handle to qos hardware instance.
 * @id:  port's id obtained from port_allocate
 *
 * Note	- already enportd descriptors will be transmitted
 *
 * Return: 0 on success
 */
int pp_qos_port_block(struct pp_qos_dev *qos_dev, unsigned int id);


/**
 * pp_qos_port_unblock() - Unblock enqueuing of new packets to queues that feeds
 *                      this port
 * @qos_dev: handle to qos hardware instance.
 * @id:	 port's id obtained from port_allocate
 *
 * Return: 0 on success
 */
int pp_qos_port_unblock(struct pp_qos_dev *qos_dev, unsigned int id);


/**
 * pp_qos_port_flush() - Drop all enqueued packets on queues under this port
 * @qos_dev: handle to qos hardware instance.
 * @id:	 port's id obtained from port_allocate
 *
 * Return: 0 on success
 */
int pp_qos_port_flush(struct pp_qos_dev *qos_dev, unsigned int id);


/**
 * pp_qos_port_conf_get() - Get port configuration
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * id:	  port's id obtained from port_allocate
 * @conf: pointer to struct to be filled with port's configuration
 *
 * Return: 0 on success
 */

int pp_qos_port_conf_get(struct pp_qos_dev *qos_dev, unsigned int id,
			 struct pp_qos_port_conf *conf);


/**
 * pp_qos_port_info_get() - Get extra information about port
 * id:	  port's id obtained from port_allocate
 * @info: pointer to struct to be filled with port's info
 *
 * Return: 0 on success
 */
int pp_qos_port_info_get(struct pp_qos_dev *qos_dev, unsigned int id,
			 struct pp_qos_port_info *info);


/**
 * pp_qos_port_get_queues() - Get all port's queues
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	        scheduler's id obtained from sched_allocate
 * @queue_ids:  client allocated array that will hold queues ids
 * @size:       size of queue_ids
 * @queues_num: holds the number of queues
 *
 * Note queues_num can be bigger than size
 *
 * Return: 0 on success
 */
int pp_qos_port_get_queues(struct pp_qos_dev *qos_dev, unsigned int id,
			   uint16_t *queue_ids, unsigned int size,
			   unsigned int *queues_num);


/**
 * pp_qos_port_stat_get() - Get port's statistics
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	  port's id obtained from port_allocate
 * @stat: pointer to struct to be filled with port's statistics
 *
 * Return: 0 on success
 */
int pp_qos_port_stat_get(struct pp_qos_dev *qos_dev, unsigned int id,
			 struct pp_qos_port_stat *stat);


/******************************************************************************/
/*                                    QUEUES                                  */
/******************************************************************************/

/**
 * @struct pp_qos_queue_stat - Statistics per queue
 * @reset:		       Should statistics be reset after reading
 * @queue_packets_occupancy:   Packets currently in queue
 * @queue_bytes_occupancy:     Bytes currently in queue
 * @total_packets_accepted:    Packets accepted by WRED
 * @total_packets_dropped:     Packets dropped by WRED due any reason
 * @total_packets_red_dropped: Packets dropped by WRED due to being red
 * @total_bytes_accepted:      Bytes accepted by WRED
 * @total_bytes_dropped:       Bytes dropped by WRED
 */
struct pp_qos_queue_stat {
	int reset;
	unsigned int	queue_packets_occupancy;
	unsigned int	queue_bytes_occupancy;
	unsigned int	total_packets_accepted;
	unsigned int	total_packets_dropped;
	unsigned int	total_packets_red_dropped;
	uint64_t	total_bytes_accepted;
	uint64_t	total_bytes_dropped;
};

/**
 * @struct pp_qos_queue_conf - Configuration structure for queue
 * @common_prop:		common properties
 * @queue_child_prop:           properties as a child node
 * @max_burst:		        max burst in Kbps
 * @blocked:			queue will drop new enqueued packets
 * @aliased:			queue will be aliased by another queue
 * @wred_enable:		WRED is applied on this queue, otherwise only
 *                              min and max values are used
 * @wred_fixed_drop_prob_enable:use fixed drop probability for WRED instead of
 *                              slope
 * @queue_wred_min_avg_green:	Start of the slope area (Below that no drops)
 * @queue_wred_max_avg_green:	End of the slope area (Above that 100% drops)
 * @queue_wred_slope_green:	0-90 degrees scale
 * @queue_wred_fixed_drop_prob_green:	Fixed drop rate in between min & max
 * @queue_wred_min_avg_yellow:	Start of the slope area (Below that no drops)
 * @queue_wred_max_avg_yellow:	End of the slope area (Above that 100% drops)
 * @queue_wred_slope_yellow:	0-90 degrees scale
 * @queue_wred_fixed_drop_prob_yellow:	Fixed drop rate in between min & max
 * @queue_wred_min_guaranteed:	Minimum descriptors guaranteed
 * @queue_wred_max_allowed:	Maximum descriptors allowed in this queue
 */
struct pp_qos_queue_conf {
	struct pp_qos_common_node_properties common_prop;
	struct pp_qos_child_node_properties  queue_child_prop;
	unsigned int max_burst;
	int      blocked;
	int      aliased;
	int	 wred_enable;
	int      wred_fixed_drop_prob_enable;
	unsigned int queue_wred_min_avg_green;
	unsigned int queue_wred_max_avg_green;
	unsigned int queue_wred_slope_green;
	unsigned int queue_wred_fixed_drop_prob_green;
	unsigned int queue_wred_min_avg_yellow;
	unsigned int queue_wred_max_avg_yellow;
	unsigned int queue_wred_slope_yellow;
	unsigned int queue_wred_fixed_drop_prob_yellow;
	unsigned int queue_wred_min_guaranteed;
	unsigned int queue_wred_max_allowed;
};


/**
 * @struct pp_qos_queue_info - Queue information struct
 * @port_id:	 port fed by this queue
 * @physical_id: queue (ready list) number
 * @alias_master_id: Id of the queue which is aliased by this queue,
 *                   PP_QOS_INVALID_ID if this queue does not alias any queue
 * @alias_slave_id: Id of the queue aliasing this queue, PP_QOS_INVALID_ID
 *                  if this queue is not aliased by any queue.
 *
 */
#define PP_QOS_INVALID_ID 0xFFFF
struct pp_qos_queue_info {
	unsigned int port_id;
	unsigned int physical_id;
	unsigned int alias_slave_id;
	unsigned int alias_master_id;
};


/**
 * pp_qos_queue_conf_set_default() - Init configuration with default values
 * @conf: pointer to client allocated struct. This struct will be filled with
 * default values.
 *
 * Defaults values:
 *    max_burst - 0
 *    bandwidth_limit - 0
 *    shared_bandwidth_limit - 0
 *    parent - 0
 *    priority - 0
 *    bandwidth_share - 0
 *    wred_enable - no
 *    ALL wred params - 0
 */
void pp_qos_queue_conf_set_default(struct pp_qos_queue_conf *conf);


/**
 * pp_qos_queue_allocate() - Allocate aresouirce for a new queue
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:   upon success hold's new queue id
 *
 * Return:  0 on success.
 */
int pp_qos_queue_allocate(struct pp_qos_dev *qos_dev, unsigned int *id);


/**
 * pp_qos_queue_remove() -	Remove a queue
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:   queue's id obtained from queue_allocate
 *
 * Note: client should make sure that queue is empty and
 *       that new packets are not enqueued, by calling
 *       pp_qos_queue_disable and pp_qos_queue_flush
 *
 * Return: 0 on success
 */
int pp_qos_queue_remove(struct pp_qos_dev *qos_dev, unsigned int id);


/**
 * pp_qos_queue_set() - Set queue configuration
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	  queue's id obtained from queue_allocate
 * @conf: new configuration for the queue
 *
 * Return: 0 on success
 */
int pp_qos_queue_set(struct pp_qos_dev *qos_dev, unsigned int id,
		     const struct pp_qos_queue_conf *conf);


/**
 * pp_qos_queue_block() - All new packets enqueue to this queue will be dropped
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:   queue's id obtained from queue_allocate
 *
 * Note	- already enqueued descriptors will be transmitted
 *
 * Return: 0 on success
 */
int pp_qos_queue_block(struct pp_qos_dev *qos_dev, unsigned int id);


/**
 * pp_qos_queue_unblock() - Unblock enqueuing of new packets
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	  queue's id obtained from queue_allocate
 *
 * Return: 0 on success
 */
int pp_qos_queue_unblock(struct pp_qos_dev *qos_dev, unsigned int id);


/**
 * pp_qos_queue_flush() - Drop all enqueued packets
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	  queue's id obtained from queue_allocate
 *
 * Return: 0 on success
 */
int pp_qos_queue_flush(struct pp_qos_dev *qos_dev, unsigned int id);


/**
 * pp_qos_queue_conf_get() - Get queue current configuration
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * id:	  port's id obtained from queue_allocate
 * @conf: pointer to struct to be filled with queue's configuration
 *
 * Return: 0 on success
 */
int pp_qos_queue_conf_get(struct pp_qos_dev *qos_dev, unsigned int id,
			  struct pp_qos_queue_conf *conf);


/**
 * pp_qos_queue_info_get() - Get information about queue
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	  queue's id obtained from queue_allocate
 * @info: pointer to struct to be filled with queue's info
 *
 * Return: 0 on success
 */
int pp_qos_queue_info_get(struct pp_qos_dev *qos_dev, unsigned int id,
			  struct pp_qos_queue_info *info);


/**
 * pp_qos_queue_stat_get() - Get queue's statistics
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	  queue's id obtained from queue_allocate
 * @stat: pointer to struct to be filled with queue's statistics
 *
 * Return: 0 on success
 */
int pp_qos_queue_stat_get(struct pp_qos_dev *qos_dev, unsigned int id,
			  struct pp_qos_queue_stat *stat);


/******************************************************************************/
/*                                 SCHEDULER                                  */
/******************************************************************************/

/**
 * @struct pp_qos_sched_conf - Configuration structure for scheduler
 * @qos_dev:               handle to qos device instance obtained
 *                     previously from pp_qos_dev_open
 * @common_prop:       common properties as a node
 * @sched_parent_prop: properties as a parent node
 * @sched_child_node:  properties as a child node
 */
struct pp_qos_sched_conf {
	struct pp_qos_common_node_properties common_prop;
	struct pp_qos_parent_node_properties sched_parent_prop;
	struct pp_qos_child_node_properties  sched_child_prop;
};


/**
 * @struct pp_qos_sched_info - Scheduler information struct
 * @qos_dev:                    handle to qos device instance obtained
 *                          previously from pp_qos_dev_open
 * @port_id:		    port fed by this scheduler
 * @num_of_queues:	    number of queues feed this scheduler.
 */
struct pp_qos_sched_info {
	unsigned int port_id;
	unsigned int num_of_queues;
};


/**
 * pp_qos_sched_conf_set_default() - Init scheduler config with defaults
 * @conf: pointer to client allocated struct which will be filled with
 *        default values
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
 * pp_qos_sched_allocate() - Allocate a resource for a new scheduler
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:   upon success - holds new scheduler's id
 *
 * Return:  0 on success.
 */
int pp_qos_sched_allocate(struct pp_qos_dev *qos_dev, unsigned int *id);


/**
 * pp_qos_sched_remove() -	Remove scheduler
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	  scheduler's id obtained from sched_allocate
 *
 * Return:	0 on success
 */
int pp_qos_sched_remove(struct pp_qos_dev *qos_dev, unsigned int id);


/**
 * pp_qos_sched_set() - Set scheduler configuration
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	  scheduler's id obtained from sched_allocate
 * @conf: new configuration for the scheduler
 *
 * Note: must init conf by pp_qos_sched_conf_init before calling
 *       this function.
 *
 * Return: 0 on success
 */
int pp_qos_sched_set(struct pp_qos_dev *qos_dev, unsigned int id,
		     const struct pp_qos_sched_conf *conf);


/**
 * pp_qos_sched_conf_get() - Get sched current configuration
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * id:	  port's id obtained from port_allocate
 * @conf: pointer to struct to be filled with sched's configuration
 *
 * Return: 0 on success
 */
int pp_qos_sched_conf_get(struct pp_qos_dev *qos_dev, unsigned int id,
			  struct pp_qos_sched_conf *conf);


/**
 * pp_qos_sched_info_get() - Get information about scheduler
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	  scheduler's id obtained from sched_allocate
 * @info: pointer to struct to be filled with scheduler's info
 *
 * Return: 0 on success
 */
int pp_qos_sched_info_get(struct pp_qos_dev *qos_dev, unsigned int id,
			  struct pp_qos_sched_info *info);


/**
 * pp_qos_sched_get_queues() - Get all scheduler's queues
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	        scheduler's id obtained from sched_allocate
 * @queue_ids:  client allocated array that will hold queues ids
 * @size:       size of queue_ids
 * @queues_num: holds the number of queues
 *
 * Note queues_num can be bigger than size
 *
 * Return: 0 on success
 */
int pp_qos_sched_get_queues(struct pp_qos_dev *qos_dev, unsigned int id,
			    uint16_t *queue_ids, unsigned int size,
			    unsigned int *queues_num);

/******************************************************************************/
/*                                 SHARED_LIMIT                               */
/******************************************************************************/

/**
 * qos_shared_limit_group_add() - Add new shared bandwidth group
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @limit: bandwidth limit in Kbps
 * @id:    upon success, new group's id
 *
 * Return: 0 on success
 */
int pp_qos_shared_limit_group_add(struct pp_qos_dev *qos_dev,
				  unsigned int limit, unsigned int *id);


/**
 * pp_qos_shared_limit_group_remove() - Remove shared bandwidth group
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id: group's id previously obtained when adding group
 *
 * Return: 0 on success
 */
int pp_qos_shared_limit_group_remove(struct pp_qos_dev *qos_dev,
				     unsigned int id);


/**
 * pp_qos_shared_limit_group_modify() - Modify shared bandwidth group
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id: group's id previously obtained when adding group
 * @limit: bandwidth limit in Kbps
 *
 * Return: 0 on success
 */
int pp_qos_shared_limit_group_modify(struct pp_qos_dev *qos_dev,
				     unsigned int id, unsigned int limit);


/**
 * pp_qos_shared_limit_group_get_members() - Get group's members
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @id:	   shared bandwidth group id
 * @members:   client allocated array that will hold ids members
 * @size:        size of ids
 * @members_num: holds the number of members
 *
 * Notes:
 * members_num can be bigger than size
 * function iterates over tree so it is rather slow
 *
 * Return: 0 on success
 */
int pp_qos_shared_limit_group_get_members(struct pp_qos_dev *qos_dev,
					  unsigned int id, uint16_t *members,
					  unsigned int size,
					  unsigned int *members_num);



/******************************************************************************/
/*                                  INIT	                              */
/******************************************************************************/

/**
 * pp_qos_dev_open() -	Initialize of a qos device
 * @id: Identify the requested qos instance (same id that
 *      designates the instance in the device tree)
 *
 * Return: on success handle to qos device
 *         NULL otherwise
 */
struct pp_qos_dev *pp_qos_dev_open(unsigned int id);


/**
 * @struct pp_qos_init_param - qos subsystem initialization struct
 * @wred_total_avail_resources: upper bound for the sum of bytes on all queues
 * @wred_p_const:         WRED algorithm p, used for calculating avg queue depth
 *                        avg = (old_avg*(1-p)) + (curr_avg*p)
 *                        p is taken to b wred_p_const / 1023
 *                        wred_p_const should be in the range 0..1023
 * @wred_max_q_size:      maximum size on bytes of any queue
 * @reserved_ports:       These ports are reserved for client to explicity
 *                        select the physical_port ithat will be used when
 *                        adding a new port. All other non reserved ports are
 *                        managed by the system, i.e. when client adds a new
 *                        port system will select a new free port from the
 *                        unreserved ports.
 *                        To reserve a port its value on the array should be
 *                        not 0
 */
struct pp_qos_init_param {
	unsigned int wred_total_avail_resources;
	unsigned int wred_p_const;
	unsigned int wred_max_q_size;
	#define QOS_MAX_PORTS    256
	unsigned int reserved_ports[QOS_MAX_PORTS];
};


/**
 * pp_qos_dev_init() - set qos system wide configuration
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @conf:
 *
 * Note
 * All other functions except pp_qos_dev_open will failed if called
 * before this function
 *
 * Return: 0 on success
 */
int pp_qos_dev_init(struct pp_qos_dev *qos_dev, struct pp_qos_init_param *conf);

/**
 * pp_qos_dev_clean() - clean resources.
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 *
 * Note:
 *   qos_dev is invalid after this call
 */
void pp_qos_dev_clean(struct pp_qos_dev *qos_dev);


/**
 * pp_qos_get_fw_version () - get fw version
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @major: storage for major part of version
 * @minor: storage for minor part of version
 * @build: storage for build part of version
 */
int pp_qos_get_fw_version(
		struct pp_qos_dev *qos_dev,
		unsigned int *major,
		unsigned int *minor,
		unsigned int *build);


enum pp_qos_node_type {
	PPV4_QOS_NODE_TYPE_PORT,
	PPV4_QOS_NODE_TYPE_SCHED,
	PPV4_QOS_NODE_TYPE_QUEUE,
};

#define PPV4_QOS_INVALID (-1U)


struct pp_qos_node_num {
	/*
	 * Physical node on scheduler's
	 * node table
	 */
	unsigned int phy;

	/*
	 * Logical id seen by client
	 */
	unsigned int id;
};

/**
 * struct pp_qos_node_info () -
 * @ppv4_qos_node_type:
 * @preds:		Ansectors of node (irrelevant to PORT)
 * @num_of_children:    Number of direct children (irrelevant to QUEUE)
 * @children:		Direct children of node (irrelevant to QUEUE)
 * @is_internal:	Is it internal node
 * @queue_physical_id:	Relevant only for QUEUE
 * @bw_limit:		In Kbps
 * @port:		The port (phy) that is fed from this queue
 *                      (relevant only for QUEUE)
 *
 * Note:
 * Since 0 is a valid value for some of the fields. PPV4_QOS_INVALID will
 * be used to mark field value as irrelevant
 *
 * All above values except from type, is_internal and logical id
 * are taken from HW and not from driver DB
 */
struct pp_qos_node_info {
	enum pp_qos_node_type type;
	struct pp_qos_node_num preds[6];
	struct pp_qos_node_num children[8];
	int is_internal;
	unsigned int queue_physical_id;
	unsigned int bw_limit;
	unsigned int port;
};

/**
 * pp_qos_get_node_info () - Obtain node's info
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @info: Storage for info
 * Return: 0 on success
 */
int pp_qos_get_node_info(
		struct pp_qos_dev *qos_dev,
		unsigned int id,
		struct pp_qos_node_info *info);

/**
 * pp_qos_get_quanta() - Gets QoS quanta (In MB). QoS credits calculations are done
 *			 in Quanta scale. QoS will transmit a multiplication
 *			 of a quanta in one iteration
 * @qos_dev: handle to qos device instance obtained from pp_qos_dev_open
 * @quanta: pointer which the quanta value will be written to
 *
 * Return: 0 on success
 */
int pp_qos_get_quanta(struct pp_qos_dev *qdev, unsigned int *quanta);

#endif
