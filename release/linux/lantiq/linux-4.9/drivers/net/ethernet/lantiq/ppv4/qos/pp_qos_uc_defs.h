/*
 * pp_qos_uc_defs.h
 * Description: Packet Processor QoS Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2017-2019 Intel Corporation
 */

#ifndef SRC_UC_HOST_DEFS_H_
#define SRC_UC_HOST_DEFS_H_

// UC version
#define UC_VERSION_MAJOR (1)
#define UC_VERSION_MINOR (6)
#define UC_VERSION_BUILD (1)

/**************************************************************************
 *! @enum UC_STATUS
 **************************************************************************
 *
 * @brief UC general status enum
 *
 **************************************************************************/
enum uc_status {
	//!< Status OK
	UC_STATUS_OK,

	//!< General failure
	UC_STATUS_GENERAL_FAILURE,

	//!< Invalid user input
	UC_STATUS_INVALID_INPUT,
};

/**************************************************************************
 *! @enum UC_LOGGER_LEVEL
 **************************************************************************
 *
 * @brief UC Logger level enum. It is recommended to use the defines below
 * for presets
 *
 **************************************************************************/
enum uc_logger_level {
	//!< No log level
	UC_LOGGER_LEVEL_NONE          = 0,

	//!< FATAL error occurred. SW will probably fail to proceed
	UC_LOGGER_LEVEL_FATAL_ONLY    = BIT(0),

	//!< General ERROR occurred.
	UC_LOGGER_LEVEL_ERROR_ONLY    = BIT(1),

	//!< WARNING
	UC_LOGGER_LEVEL_WARNING_ONLY  = BIT(2),

	//!< Information print to the user
	UC_LOGGER_LEVEL_INFO_ONLY     = BIT(3),

	//!< Debug purposes level
	UC_LOGGER_LEVEL_DEBUG_ONLY    = BIT(4),

	//!< Dump all writings to registers
	UC_LOGGER_LEVEL_DUMP_REG_ONLY = BIT(5),

	//!< Dump all commands
	UC_LOGGER_LEVEL_COMMANDS_ONLY = BIT(6),
};

/* Below levels will be normally used from host. */
/* Each level includes all higher priorities levels messages */

//!< FATAL/ERROR level messages
#define UC_LOGGER_LEVEL_DEFAULT         \
	(UC_LOGGER_LEVEL_FATAL_ONLY |   \
	 UC_LOGGER_LEVEL_ERROR_ONLY)

//!< FATAL level messages
#define UC_LOGGER_LEVEL_FATAL           \
	(UC_LOGGER_LEVEL_FATAL_ONLY |   \
	 UC_LOGGER_LEVEL_COMMANDS_ONLY)

//!< ERRORS level messages
#define UC_LOGGER_LEVEL_ERROR           \
	(UC_LOGGER_LEVEL_FATAL_ONLY |   \
	 UC_LOGGER_LEVEL_ERROR_ONLY |   \
	 UC_LOGGER_LEVEL_COMMANDS_ONLY)

//!< WARNING level messages
#define UC_LOGGER_LEVEL_WARNING         \
	(UC_LOGGER_LEVEL_FATAL_ONLY   | \
	 UC_LOGGER_LEVEL_ERROR_ONLY   | \
	 UC_LOGGER_LEVEL_WARNING_ONLY | \
	 UC_LOGGER_LEVEL_COMMANDS_ONLY)

//!< INFO level messages
#define UC_LOGGER_LEVEL_INFO            \
	(UC_LOGGER_LEVEL_FATAL_ONLY   | \
	 UC_LOGGER_LEVEL_ERROR_ONLY   | \
	 UC_LOGGER_LEVEL_WARNING_ONLY | \
	 UC_LOGGER_LEVEL_INFO_ONLY    | \
	 UC_LOGGER_LEVEL_COMMANDS_ONLY)

//!< DEBUG level messages
#define UC_LOGGER_LEVEL_DEBUG           \
	(UC_LOGGER_LEVEL_FATAL_ONLY   | \
	 UC_LOGGER_LEVEL_ERROR_ONLY   | \
	 UC_LOGGER_LEVEL_WARNING_ONLY | \
	 UC_LOGGER_LEVEL_INFO_ONLY    | \
	 UC_LOGGER_LEVEL_DEBUG_ONLY   | \
	 UC_LOGGER_LEVEL_COMMANDS_ONLY)

//!< DUMP to registers level messages
#define UC_LOGGER_LEVEL_DUMP_REGS        \
	(UC_LOGGER_LEVEL_FATAL_ONLY    | \
	 UC_LOGGER_LEVEL_ERROR_ONLY    | \
	 UC_LOGGER_LEVEL_WARNING_ONLY  | \
	 UC_LOGGER_LEVEL_DUMP_REG_ONLY | \
	 UC_LOGGER_LEVEL_COMMANDS_ONLY)

/**************************************************************************
 *! @enum UC_LOGGER_MODE
 **************************************************************************
 *
 * @brief UC Logger operation mode
 *
 **************************************************************************/
enum uc_logger_mode {
	//!< Logger is disabled
	UC_LOGGER_MODE_NONE,

	//!< Messages are written to the standard output
	UC_LOGGER_MODE_STDOUT,

	//!< Local file. N/A
//	UC_LOGGER_MODE_LOCAL_FILE,

	//!< Messages are written to the host allocated memory
	UC_LOGGER_MODE_WRITE_HOST_MEM,
};

/**************************************************************************
 *! \enum TSCD_NODE_CONF
 **************************************************************************
 *
 * \brief TSCD node configuration valid bits. Used in modify existing node
 *
 **************************************************************************/
enum tscd_node_conf {
	//!< None
	TSCD_NODE_CONF_NONE               = 0,

	//!< Suspend/Resume node
	TSCD_NODE_CONF_SUSPEND_RESUME     = BIT(0),

	//!< first child (Not relevant for queue)
	TSCD_NODE_CONF_FIRST_CHILD        = BIT(1),

	//!< last child (Not relevant for queue)
	TSCD_NODE_CONF_LAST_CHILD         = BIT(2),

	//!< 0 - BW Limit disabled >0 - define BW
	TSCD_NODE_CONF_BW_LIMIT           = BIT(3),

	//!< Best Effort enable
	TSCD_NODE_CONF_BEST_EFFORT_ENABLE = BIT(4),

	//!< First Weighted-Round-Robin node (Not relevant for queue)
	TSCD_NODE_CONF_FIRST_WRR_NODE     = BIT(5),

	//!< Node Weight (Not relevant for ports)
	TSCD_NODE_CONF_NODE_WEIGHT        = BIT(6),

	//!< Update predecessor 0 (Not relevant for port)
	TSCD_NODE_CONF_PREDECESSOR_0      = BIT(7),

	//!< Update predecessor 1 (Not relevant for port)
	TSCD_NODE_CONF_PREDECESSOR_1      = BIT(8),

	//!< Update predecessor 2 (Not relevant for port)
	TSCD_NODE_CONF_PREDECESSOR_2      = BIT(9),

	//!< Update predecessor 3 (Not relevant for port)
	TSCD_NODE_CONF_PREDECESSOR_3      = BIT(10),

	//!< Update predecessor 4 (Not relevant for port)
	TSCD_NODE_CONF_PREDECESSOR_4      = BIT(11),

	//!< Update predecessor 5 (Not relevant for port)
	TSCD_NODE_CONF_PREDECESSOR_5      = BIT(12),

	//!< Shared BW limit group (0: no shared BW limit, 1-511: group ID)
	TSCD_NODE_CONF_SHARED_BWL_GROUP   = BIT(13),

	//!< Set if Queue's port was changed (Relevant only for queue)
	TSCD_NODE_CONF_SET_PORT_TO_QUEUE  = BIT(14),

	//!< All flags are set
	TSCD_NODE_CONF_ALL                = 0xFFFF
};

/**************************************************************************
 *! \enum QUEUE_CONF
 **************************************************************************
 *
 * \brief queue configuration valid bits. Used in modify existing queue
 *
 **************************************************************************/
enum queue_conf {
	//!< None
	QUEUE_CONF_NONE                     = 0,

	//!< Q is active
	WRED_QUEUE_CONF_ACTIVE_Q            = BIT(0),

	//!< Disable flags valid
	WRED_QUEUE_CONF_DISABLE             = BIT(1),

	//!< Use fixed green drop probability
	WRED_QUEUE_CONF_FIXED_GREEN_DROP_P  = BIT(2),

	//!< Use fixed yellow drop probability
	WRED_QUEUE_CONF_FIXED_YELLOW_DROP_P = BIT(3),

	//!< Min average yellow
	WRED_QUEUE_CONF_MIN_AVG_YELLOW      = BIT(4),

	//!< Max average yellow
	WRED_QUEUE_CONF_MAX_AVG_YELLOW      = BIT(5),

	//!< Slope yellow
	WRED_QUEUE_CONF_SLOPE_YELLOW        = BIT(6),

	//!< INTERNAL CONFIGURATION. SHOULD NOT BE SET BY HOST
	WRED_QUEUE_CONF_SHIFT_AVG_YELLOW    = BIT(7),

	//!< Min average green
	WRED_QUEUE_CONF_MIN_AVG_GREEN       = BIT(8),

	//!< Max average green
	WRED_QUEUE_CONF_MAX_AVG_GREEN       = BIT(9),

	//!< Slope green
	WRED_QUEUE_CONF_SLOPE_GREEN         = BIT(10),

	//!< INTERNAL CONFIGURATION. SHOULD NOT BE SET BY HOST
	WRED_QUEUE_CONF_SHIFT_AVG_GREEN     = BIT(11),

	//!< Min guaranteed
	WRED_QUEUE_CONF_MIN_GUARANTEED      = BIT(12),

	//!< max allowed
	WRED_QUEUE_CONF_MAX_ALLOWED         = BIT(13),

	//!< CoDel enable
	TXMGR_QUEUE_CODEL_EN                = BIT(14),

	//!< All flags are set
	QUEUE_CONF_ALL                      = 0xFFFF
};

/**************************************************************************
 *! \enum PORT_CONF
 **************************************************************************
 *
 * \brief Port configuration valid bits. Used in modify existing port
 *
 **************************************************************************/
enum port_conf {
	//!< None
	PORT_CONF_NONE              = 0,

	//!< Ring Size
	PORT_CONF_RING_SIZE         = BIT(0),

	//!< Ring address high
	PORT_CONF_RING_ADDRESS_HIGH = BIT(1),

	//!< Ring address low
	PORT_CONF_RING_ADDRESS_LOW  = BIT(2),

	//!< Enable port
	PORT_CONF_ACTIVE            = BIT(3),

	//!< Green bytes threshold
	PORT_CONF_GREEN_THRESHOLD   = BIT(4),

	//!< Yellow bytes threshold
	PORT_CONF_YELLOW_THRESHOLD  = BIT(5),

	//!< All flags are set
	PORT_CONF_ALL               = 0xFFFF
};

/**************************************************************************
 *! \struct port_stats_s
 **************************************************************************
 *
 * \brief Port stats
 *
 **************************************************************************/
struct port_stats_s {
	u32 total_green_bytes;
	u32 total_yellow_bytes;

	// Following stats can not be reset
	u32 debug_back_pressure_status;
	u32 debug_actual_packet_credit;
	u32 debug_actual_byte_credit;
};

/**************************************************************************
 *! \struct codel_qstats_s
 **************************************************************************
 *
 * \brief Codel queue stats
 *
 **************************************************************************/
struct codel_qstats_s {
	u32 drop_pkts;
	u32 drop_bytes;
	u32 min_packet_sojourn_time;
	u32 max_packet_sojourn_time;
	u32 total_sojourn_time;
	u32 max_pkt_sz;
	u32 num_pkts;
};

/**************************************************************************
 *! \struct codel_qstate_s
 **************************************************************************
 *
 * \brief Codel queue state
 *
 **************************************************************************/
struct codel_qstate_s {
	u32 count;
	u32 last_count;
	u32 first_above_time;
	u32 drop_next;
	u32 drop_rec_inv_sqrt;
};

/**************************************************************************
 *! \struct hw_node_info_s
 **************************************************************************
 *
 * \brief HW node info
 *
 **************************************************************************/
struct hw_node_info_s {
	u32 first_child;
	u32 last_child;
	u32 is_suspended;
	u32 bw_limit;
	u32 predecessor0;
	u32 predecessor1;
	u32 predecessor2;
	u32 predecessor3;
	u32 predecessor4;
	u32 predecessor5;
	u32 queue_physical_id;
	u32 queue_port;
};

/**************************************************************************
 *! \enum PORT_STATS_CLEAR_FLAGS
 **************************************************************************
 *
 * \brief port stats clear flags.
 *        Used in get port stats command to set which stats
 *        will be reset after read
 *
 **************************************************************************/
enum port_stats_clear_flags {
	//!< None
	PORT_STATS_CLEAR_NONE               = 0,

	//!< Clear port total green bytes stats
	PORT_STATS_CLEAR_TOTAL_GREEN_BYTES  = BIT(0),

	//!< Clear port total yellow bytes stats
	PORT_STATS_CLEAR_TOTAL_YELLOW_BYTES = BIT(1),

	//!< All above stats will be cleared
	PORT_STATS_CLEAR_ALL                = 0xFFFF,
};

/**************************************************************************
 *! \struct queue_stats_s
 **************************************************************************
 *
 * \brief Queue stats
 *
 **************************************************************************/
struct queue_stats_s {
	/* WRED counters */
	u32 queue_size_bytes;
	u32 queue_average_size_bytes;
	u32 queue_size_entries;
	u32 drop_p_yellow;
	u32 drop_p_green;
	u32 total_bytes_added_low;
	u32 total_bytes_added_high;
	u32 total_accepts;
	u32 total_drops;
	u32 total_dropped_bytes_low;
	u32 total_dropped_bytes_high;
	u32 total_red_dropped;

	/* Following stats can not be reset */
	u32 qmgr_num_queue_entries;
};

/**************************************************************************
 *! \enum QUEUE_STATS_CLEAR_FLAGS
 **************************************************************************
 *
 * \brief queue stats clear flags.
 *        Used in get queue stats command to set which stats
 *        will be reset after read
 *
 **************************************************************************/
enum queue_stats_clear_flags {
	//!< None
	QUEUE_STATS_CLEAR_NONE                = 0,

	//!< Clear queue size bytes stats
	QUEUE_STATS_CLEAR_Q_SIZE_BYTES        = BIT(0),

	//!< Clear queue average size bytes stats
	QUEUE_STATS_CLEAR_Q_AVG_SIZE_BYTES    = BIT(1),

	//!< Clear queue size entries stats
	QUEUE_STATS_CLEAR_Q_SIZE_ENTRIES      = BIT(2),

	//!< Clear drop probability yellow stats
	QUEUE_STATS_CLEAR_DROP_P_YELLOW       = BIT(3),

	//!< Clear drop probability green stats
	QUEUE_STATS_CLEAR_DROP_P_GREEN        = BIT(4),

	//!< Clear total bytes added stats
	QUEUE_STATS_CLEAR_TOTAL_BYTES_ADDED   = BIT(5),

	//!< Clear total accepts stats
	QUEUE_STATS_CLEAR_TOTAL_ACCEPTS       = BIT(6),

	//!< Clear total drops stats
	QUEUE_STATS_CLEAR_TOTAL_DROPS         = BIT(7),

	//!< Clear total dropped bytes stats
	QUEUE_STATS_CLEAR_TOTAL_DROPPED_BYTES = BIT(8),

	//!< Clear total RED drops stats
	QUEUE_STATS_CLEAR_TOTAL_RED_DROPS     = BIT(9),

	//!< All above stats will be cleared
	QUEUE_STATS_CLEAR_ALL                 = 0xFFFF,
};

/**************************************************************************
 *! \struct system_stats_s
 **************************************************************************
 *
 * \brief system stats
 *
 **************************************************************************/
struct system_stats_s {
	u32 qmgr_cache_free_pages_counter;
	u32 qmgr_sm_current_state;
	u32 qmgr_cmd_machine_busy;
	u32 qmgr_cmd_machine_pop_busy;
	u32 qmgr_null_pop_counter;
	u32 qmgr_empty_pop_counter;
	u32 qmgr_null_push_counter;
	u32 qmgr_ddr_stop_push_low_threshold;
	u32 qmgr_fifo_error_register;
	u32 qmgr_ocp_error_register;
	u32 qmgr_cmd_machine_sm_current_state_0;
	u32 qmgr_cmd_machine_sm_current_state_1;
	u32 qmgr_cmd_machine_sm_current_state_2;
	u32 qmgr_cmd_machine_sm_current_state_3;
	u32 qmgr_cmd_machine_sm_current_state_4;
	u32 qmgr_cmd_machine_sm_current_state_5;
	u32 qmgr_cmd_machine_sm_current_state_6;
	u32 qmgr_cmd_machine_sm_current_state_7;
	u32 qmgr_cmd_machine_sm_current_state_8;
	u32 qmgr_cmd_machine_sm_current_state_9;
	u32 qmgr_cmd_machine_sm_current_state_10;
	u32 qmgr_cmd_machine_sm_current_state_11;
	u32 qmgr_cmd_machine_sm_current_state_12;
	u32 qmgr_cmd_machine_sm_current_state_13;
	u32 qmgr_cmd_machine_sm_current_state_14;
	u32 qmgr_cmd_machine_sm_current_state_15;
	u32 qmgr_cmd_machine_queue_0;
	u32 qmgr_cmd_machine_queue_1;
	u32 qmgr_cmd_machine_queue_2;
	u32 qmgr_cmd_machine_queue_3;
	u32 qmgr_cmd_machine_queue_4;
	u32 qmgr_cmd_machine_queue_5;
	u32 qmgr_cmd_machine_queue_6;
	u32 qmgr_cmd_machine_queue_7;
	u32 qmgr_cmd_machine_queue_8;
	u32 qmgr_cmd_machine_queue_9;
	u32 qmgr_cmd_machine_queue_10;
	u32 qmgr_cmd_machine_queue_11;
	u32 qmgr_cmd_machine_queue_12;
	u32 qmgr_cmd_machine_queue_13;
	u32 qmgr_cmd_machine_queue_14;
	u32 qmgr_cmd_machine_queue_15;
	u32 tscd_num_of_used_nodes;

	/* Error in Scheduler tree configuration */
	u32 tscd_infinite_loop_error_occurred;

	/* HW failed to complete the bwl credits updates */
	u32 tscd_bwl_update_error_occurred;

	/* Quanta size in KB */
	u32 tscd_quanta;
};

/**************************************************************************
 *! \struct system_stats_s
 **************************************************************************
 *
 * \brief system stats
 *
 **************************************************************************/
struct used_nodes_s {
	u32 tscd_num_of_used_nodes;
	u32 nodes_bmap[64];
};

/**************************************************************************
 *! @enum UC_QOS_CMD
 **************************************************************************
 *
 * @brief UC QOS command enum. Must be synced with the Host definition
 *
 **************************************************************************/
enum uc_qos_command {
	UC_QOS_CMD_GET_FW_VERSION,
	UC_QOS_CMD_MULTIPLE_COMMANDS,
	UC_QOS_CMD_INIT_UC_LOGGER,
	UC_QOS_CMD_SET_UC_LOGGER_LEVEL,
	UC_QOS_CMD_INIT_QOS,
	UC_QOS_CMD_ADD_PORT,
	UC_QOS_CMD_REM_PORT,
	UC_QOS_CMD_ADD_SCHEDULER,
	UC_QOS_CMD_REM_SCHEDULER,
	UC_QOS_CMD_ADD_QUEUE,
	UC_QOS_CMD_REM_QUEUE,
	UC_QOS_CMD_FLUSH_QUEUE,
	UC_QOS_CMD_SET_PORT,
	UC_QOS_CMD_SET_SCHEDULER,
	UC_QOS_CMD_SET_QUEUE,
	UC_QOS_CMD_MOVE_SCHEDULER,
	UC_QOS_CMD_MOVE_QUEUE,
	UC_QOS_CMD_GET_PORT_STATS,
	UC_QOS_CMD_GET_QUEUE_STATS,
	UC_QOS_CMD_GET_SYSTEM_STATS,
	UC_QOS_CMD_ADD_SHARED_BW_LIMIT_GROUP,
	UC_QOS_CMD_REM_SHARED_BW_LIMIT_GROUP,
	UC_QOS_CMD_SET_SHARED_BW_LIMIT_GROUP,
	UC_QOS_CMD_GET_NODE_INFO,
	UC_QOS_CMD_DEBUG_READ_NODE,
	UC_QOS_CMD_DEBUG_PUSH_DESC,
	UC_QOS_CMD_DEBUG_ADD_CREDIT_TO_PORT,
	UC_QOS_CMD_GET_ACTIVE_QUEUES_STATS,
	UC_QOS_CMD_UPDATE_PORT_TREE,
	UC_QOS_CMD_SET_AQM_SF,
	UC_QOS_CMD_SET_CODEL_CFG,
	UC_QOS_CMD_GET_CODEL_QUEUE_STATS,
	UC_QOS_CMD_CLK_UPDATE,
	UC_QOS_CMD_NODES_REFRESH,
	UC_QOS_CMD_GET_USED_NODES,
	UC_QOS_CMD_SUSPEND_PORT_TREE,
};

/**************************************************************************
 *! @struct uc_qos_cmd_s
 **************************************************************************
 *
 * @brief UC commands.
 * This structure defines the Host <-->UC interface
 *
 **************************************************************************/
struct uc_qos_cmd_base {
	//!< Type of command (UC_QOS_CMD)
	u32 type;

	//!< Commands flags
	u32 flags;

	//!< Number of 32bit parameters available for this command.
	// must be synced between the host and uc!
	u32 num_params;
};

struct uc_qos_cmd_s {
	//!< Type of command (UC_QOS_CMD)
	u32 type;

	//!< Commands flags
	u32 flags;
#define UC_CMD_FLAG_IMMEDIATE             BIT(0)
#define UC_CMD_FLAG_BATCH_FIRST           BIT(1)
#define UC_CMD_FLAG_BATCH_LAST            BIT(2)
#define UC_CMD_FLAG_MULTIPLE_COMMAND_LAST BIT(3)
#define UC_CMD_FLAG_UC_DONE               BIT(4)
#define UC_CMD_FLAG_UC_ERROR              BIT(5)

	//!< Number of 32bit parameters available for this command.
	// must be synced between the host and uc!
	u32 num_params;

	u32 param0;
	u32 param1;
	u32 param2;
	u32 param3;
	u32 param4;
	u32 param5;
	u32 param6;
	u32 param7;
	u32 param8;
	u32 param9;
	u32 param10;
	u32 param11;
	u32 param12;
	u32 param13;
	u32 param14;
	u32 param15;
	u32 param16;
	u32 param17;
	u32 param18;
	u32 param19;
	u32 param20;
	u32 param21;
	u32 param22;
	u32 param23;
	u32 param24;
	u32 param25;
	u32 param26;
	u32 param27;
	u32 param28;
	u32 param29;
};

struct fw_cmd_clk_update {
	struct uc_qos_cmd_base base;
	u32 clk_MHz;     /*! new clock frequency */
};

struct fw_cmd_refresh_nodes {
	struct uc_qos_cmd_base base;
	u32 base_phy_id;   /*! base physical id */
	u32 nodes_bmap[4]; /*! bitmap specifying which nodes to update,
			    *  bit n represent node (base_phy_id +n)
			    */
};

#endif /* SRC_UC_HOST_DEFS_H_ */
