/***************************************************************************
 **
 ** FILE NAME    : qos_hal.h
 ** PROJECT      : QOS DP HAL
 ** MODULES      : QOS (QoS Engine )
 **
 ** AUTHOR       : Purnendu Ghosh
 ** DESCRIPTION  : QOS HAL Layer
 ** COPYRIGHT : Copyright (c) 2017 - 2018 Intel Corporation
 ** Copyright (c) 2010 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 **
 ***************************************************************************/

/************************Below is Common Macro Definition ******************/
/** \addtogroup  QOS_HAL_HEADER_MACRO */
#include "qos_mgr_stack_al.h"
/*@{*/
/**  \brief QOS HAL Common Macro Definition    */

/** Maximum number of queues that can be used
    This does not include the special "drop queue" (index = 255) */
#define QOS_HAL_MAX_QUEUE                  255

/** Number of available scheduler blocks */
#define QOS_HAL_MAX_SCHEDULER              255

/** Maximum scheduling hierarchy level */
#define QOS_HAL_MAX_SCHEDULER_LEVEL        7

/** Maximum number of egress queues,
    the queues are numbered 0 to QOS_HAL_MAX_EGRESS_QUEUES - 1 */
#define QOS_HAL_MAX_EGRESS_QUEUES         256

/** Number of traffic shaper instances */
#define QOS_HAL_MAX_SHAPER                 256

/** Maximum number of egress ports,
    the egress ports are numbered 0 to QOS_HAL_MAX_EGRESS_PORT - 1 */
#define QOS_HAL_MAX_EGRESS_PORT           138

/** Maximum Queue allowed per Egress Port */
#define QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE      16

/** Maximum Queue allowed per Ingress Port */
#define QOS_HAL_MAX_QUEUE_PER_INGRESS_INTERFACE     16

/** Maximum Sub Interface Id supported per Port */
#define QOS_HAL_MAX_SUB_IFID_PER_PORT      16

/** Maximum Priority Level supported  */
#define QOS_HAL_MAX_PRIORITY_LEVEL      16

/** Maximum Directpath Port supported  */
#define QOS_HAL_MAX_DIRECTPATH_PORT      8

/** Maximum number of elements for qos_hal_dp_res */
#define QOS_HAL_MAX_DP_RES              16

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
/** Maximum allowed buffers per Queue. Signifies Max Queue Length **/
#define QOS_HAL_QWRED_MAX_ALLOWED	1024
#define QOS_HAL_QWRED_MAX_ALLOWED_LRO	4096

/** Minimum gauranteed buffers per Queue. Signifies Min Queue Length **/
#define QOS_HAL_QWRED_MIN_GUARANTEED	64
#else
/** Maximum allowed buffers per Queue. Signifies Max Queue Length **/
#define QOS_HAL_QWRED_MAX_ALLOWED	1024
#define QOS_HAL_QWRED_MAX_ALLOWED_LRO	1024

/** Minimum gauranteed buffers per Queue. Signifies Min Queue Length **/
#define QOS_HAL_QWRED_MIN_GUARANTEED	0
#endif

#ifndef ARRAY_SIZE
#   define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#ifndef MIN
#   define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef offsetof
#   define offsetof(STRUCT, MEMBER) ((size_t) &((STRUCT *) 0)->MEMBER)
#endif

#ifdef __GNUC__
#  define QOS_HAL_ASSERT_SCOPE __func__
#else
#  define QOS_HAL_ASSERT_SCOPE __FILE__
#endif

#define QOS_HAL_ASSERT(expr) \
	do { \
		if ((expr)) { \
			QOS_HAL_DEBUG_MSG(QOS_HAL_DBG_ERR, "%s [%d] : %s", \
				QOS_HAL_ASSERT_SCOPE, __LINE__, #expr); \
			return QOS_HAL_STATUS_ERR; \
		} \
	} while (0)

/*!
  \brief QOS_HAL_QUEUE_MAP_F_FLOWID1 */
#define QOS_HAL_QUEUE_MAP_F_FLOWID1		0x1 /*!< FlowId bits are don't care for map to QueueId */
/*!
  \brief QOS_HAL_QUEUE_MAP_F_FLOWID2 */
#define QOS_HAL_QUEUE_MAP_F_FLOWID2		0x2 /*!< FlowId bits are don't care for map to QueueId */
/*!
  \brief QOS_HAL_QUEUE_MAP_F_MPE1 */
#define QOS_HAL_QUEUE_MAP_F_MPE1		0x8 /*!< MPE1 bit is don't care for map to QueueId */
/*!
  \brief QOS_HAL_QUEUE_MAP_F_MPE2 */
#define QOS_HAL_QUEUE_MAP_F_MPE2		0x10 /*!< MPE2 bit is don't care for map to QueueId */
/*!
  \brief QOS_HAL_QUEUE_MAP_F_EP */
#define QOS_HAL_QUEUE_MAP_F_EP		0x20 /*!< EP/Egress Port field is don't care for map to QueueId */
/*!
  \brief QOS_HAL_QUEUE_MAP_F_TC1 */
#define QOS_HAL_QUEUE_MAP_F_TC1		0x40 /*!< TC/Class field is don't care for map to QueueId */
/*!
  \brief QOS_HAL_QUEUE_MAP_F_TC2 */
#define QOS_HAL_QUEUE_MAP_F_TC2		0x80 /*!< TC/Class field is don't care for map to QueueId */
/*!
  \brief QOS_HAL_QUEUE_MAP_F_TC3 */
#define QOS_HAL_QUEUE_MAP_F_TC3		0x100 /*!< TC/Class field is don't care for map to QueueId */
/*!
  \brief QOS_HAL_QUEUE_MAP_F_TC4 */
#define QOS_HAL_QUEUE_MAP_F_TC4		0x200 /*!< TC/Class field is don't care for map to QueueId */

/** Enumeration for function status return.
    The upper four bits are reserved for error classification.
*/
enum qos_hal_errorcode {
	/* Common error codes */
	/** Generic or unknown error occurred */
	QOS_HAL_STATUS_ERR = -1,
	/** No error */
	QOS_HAL_STATUS_OK = 0,
	/** Resource not available */
	QOS_HAL_STATUS_NOT_AVAILABLE = 100,
	/** Max Schedulers are used */
	QOS_HAL_STATUS_MAX_SCHEDULERS_REACHED = 101,
	/** Max Queue number for that Port is reached */
	QOS_HAL_STATUS_MAX_QUEUES_FOR_PORT = 102,
	/** Max Schedulers are used */
	QOS_HAL_STATUS_QUEUE_MISMATCH = 103,
	/** Max Schedulers are used */
	QOS_HAL_STATUS_INVALID_QID = 104,
	/** Max Queues reached*/
	QOS_HAL_STATUS_MAX_QUEUES_REACHED = 105,
	/** Not supported by software yet */
	QOS_HAL_STATUS_NOT_IMPLEMENTED = 108,

};

/** Scheduling policy.
    Used by qos_hal_sched_create.
*/
enum qos_hal_policy {
	/** No policy specified, use default.*/
	QOS_HAL_POLICY_NULL = 0,
	/** Weighted Round Robin policy.*/
	QOS_HAL_POLICY_WRR = 1,
	/** Head of Line policy.*/
	QOS_HAL_POLICY_STRICT = 2,
	/** WSP */
	QOS_HAL_POLICY_WSP = 3,
	/** WSP-WRR Mode*/
	QOS_HAL_POLICY_WSP_WRR = 4,
	/** Weighted Fair Queue*/
	QOS_HAL_POLICY_WFQ = 5
};

/** Input type.
    Mainly for the queue. It could be connected to scheduler/port.
*/
enum qos_hal_input_type {
	/** scheduler, use default.*/
	QOS_HAL_INPUT_TO_SCHED = 0,
	/** Port.*/
	QOS_HAL_INPUT_TO_PORT = 1,
};

/*Struct declarion for compilation dependence */

/** Egress queue configuration.
*/
struct qos_hal_equeue_create {
	/** Egress queue index (0 to QOS_HAL_MAX_QUEUE - 1).*/
	uint32_t index;
	/** Queue egress port number (EPN), valid numbers are:*/
	uint32_t egress_port_number;
	/** Queue scheduler block input number (SBIN),
	  this defines the scheduler to be used as well as the input
	  selection of this scheduler,
	  valid from 0 to QOS_HAL_MAX_SCHEDULER * 8 - 1.*/
	uint32_t scheduler_input;
	/** priority. Specially when queue is connected
	  to a port (in case of ppv4) */
	uint32_t prio;
	/** Actual queue weight. */
	uint32_t iwgt;
} __attribute__ ((packed));

struct qos_hal_equeue_cfg {
	/** Egress queue index (0 to QOS_HAL_MAX_QUEUE - 1).*/
	uint32_t index;
	/** Queue enable. Once a queue is enabled, it must not be switched off again.
	  - 0: Enabled
	  - 1: Disabled. */
	uint32_t enable;
	/** Scheduler input enable.
	  - 0: Enabled
	  - 1: Disabled. */
	uint32_t sbin_enable;
	/** Special connection viz. lro  */
	uint32_t spl_conn;
	/** Queue Length */
	uint32_t q_len;
	/** Queue weight. */
	uint32_t weight;
	/** Queue WRED enable.
	  - 0: Enabled
	  - 1: Disabled. */
	uint32_t wred_enable;
	/**  Queue averaging weight */
	uint32_t avg_weight;
	/** Queue tail drop threshold for unassigned packets,
	  given in multiples of 512 byte (eight 64-byte buffer segments). */
	uint32_t drop_threshold_unassigned;
	/** Queue reservation threshold, enabled by setting value > 0,
	  given in multiples of 512 byte (eight 64-byte buffer segments). */
	uint32_t reservation_threshold;
	/** Queue tail drop threshold for red packets,
	  given in multiples of 512 byte (eight 64-byte buffer segments). */
	uint32_t drop_threshold_red;
	/** Maximum drop threshold for "green" data frames,
	  given in multiples of 512 byte (eight 64-byte buffer segments). */
	uint32_t drop_threshold_green_max;
	/** Minimum drop threshold for "green" data frames,
	  given in multiples of 512 byte (eight 64-byte buffer segments). */
	uint32_t drop_threshold_green_min;
	/** Maximum drop threshold for "yellow" data frames,
	  given in multiples of 512 byte (eight 64-byte buffer segments). */
	uint32_t drop_threshold_yellow_max;
	/** Minimum drop threshold for "yellow" data frames,
	  given in multiples of 512 byte (eight 64-byte buffer segments). */
	uint32_t drop_threshold_yellow_min;
	/** Maximum drop probability for "green" data frames,
	  defined by a 12-bit value which represents a fraction of the
	  probability range between 0 (value 0) and 1 (value 4095). */
	uint32_t drop_probability_green;
	/** Maximum drop probability for "yellow" data frames,
	  defined by a 12-bit value which represents a fraction of the
	  probability range between 0 (value 0) and 1 (value 4095). */
	uint32_t drop_probability_yellow;
} __attribute__ ((packed));

/** Egress queue status.
*/
struct qos_hal_equeue_status {
	/** Egress queue index (queue identifier, from 0 to the number of
	  supported egress queues minus 1).*/
	uint32_t index;
	/** Queue configuration option (read-only, fixed value = 1).*/
	uint32_t config_opt;
	/** Maximum queue filling level, in multiples of 64 bytes (segments).*/
	uint32_t fill_max;
	/** Actual queue filling level, in multiples of 64 bytes (segments).*/
	uint32_t fill;
	/** Average queue filling level, in number of segments with 8 fractional bits.
	  Divide by 4 to get the number of bytes. */
	uint32_t fill_avg;
	/** Actual number of frames (packets) in the queue.*/
	uint32_t frames;
	/** Actual queue weight. */
	uint32_t iwgt;
} __attribute__ ((packed));

/** Scheduler information - per level.
    Used by qos_hal_equeue_path.
*/
struct qos_hal_equeue_path_level {
	/** Scheduler block ID.*/
	uint32_t sbid;
	/** Scheduler block input leaf.*/
	uint32_t leaf;
	/** Scheduler input enable.
	  - 0: Disabled
	  - 1: Enabled.*/
	uint32_t sie;
	/** Scheduler input type.
	  - 0: Queue
	  - 1: Scheduler.*/
	uint32_t sit;
	/** Inverse weight.*/
	uint32_t iwgt;
	/** Queue or scheduler ID (depending on sit).*/
	uint32_t qsid;
	/** Scheduler output enable.
	  - 0: Disabled
	  - 1: Enabled.*/
	uint32_t soe;
	/** Scheduler hierarchy level. */
	uint32_t lvl;
} __attribute__ ((packed));

/** Egress queue path.
*/
struct qos_hal_equeue_path {
	/** Scheduler hierarchy level.*/
	uint32_t lvl;
	/** Scheduler block ID.*/
	uint32_t sbid;
	/** Queue enable.
	  - 0: Disabled
	  - 1: Enabled.*/
	uint32_t qe;
	/** Egress port number.*/
	uint32_t epn;
	/** Output mapping identifier, indicates the scheduler's output type.
	  - 0: Egress port
	  - 1: Scheduler input of next scheduler hierarchy.*/
	uint32_t omid;
	/** Egress port enable.
	  - 0: Disabled
	  - 1: Enabled.*/
	uint32_t epe;
	/**  Scheduler hierarchy structural information.*/
	struct qos_hal_equeue_path_level info[9];
} __attribute__ ((packed));

/** Add an entry to the traffic scheduler table.
*/
struct qos_hal_sched_create {
	/** Scheduler index, valid from 0 to QOS_HAL_MAX_SCHEDULER - 1.*/
	uint32_t index;
	/** Level within the scheduling hierarchy. This number is equal to the
	  longest path counted in number of schedulers seen from this
	  scheduler down the hierarchy.
	  Valid from 0 to \ref QOS_HAL_MAX_SCHEDULER_LEVEL*/
	uint32_t level;
	/** Scheduler ID .*/
	uint32_t scheduler_id;
	/** Output port index .*/
	uint32_t port_idx;
	/** Output scheduler index.*/
	uint32_t connected_scheduler_index;
	/** This parameter selects if the scheduler is
	  attached to the egress port identified by port_idx
	  (use_regular == true) or to the input of another scheduler
	  identified by connected_scheduler_index (use_regular == false).*/
	uint32_t use_regular;
	/** Scheduling policy of this scheduler.*/
	enum qos_hal_policy scheduler_policy;
	/** Priority/Weight into the connected scheduler.*/
	uint32_t priority_weight;
} __attribute__ ((packed));

/** Traffic scheduler configuration data.
*/
struct qos_hal_scheduler_cfg {
	/** Scheduler index, valid from 0 to the number of schedulers minus 1.
	 */
	uint32_t index;
	/** Output enable.*/
	uint32_t output_enable;
	/** Weight into connected scheduler.*/
	uint32_t weight;
} __attribute__ ((packed));

/** Traffic scheduler status values.
*/
struct qos_hal_scheduler_status {
	/** Scheduler index, valid from 0 to the number of schedulers minus 1.
	 */
	uint32_t index;
	/** The scheduler output is filled, if true.*/
	uint32_t sof;
	/** Winner leaf to be selected next to transmit from this scheduler.*/
	uint32_t wl;
	/** Winner egress queue to be selected next to transmit from this
	  scheduler.*/
	uint32_t wqid;
} __attribute__ ((packed));

/** Token bucket shaper.
*/
struct qos_hal_token_bucket_shaper {
	/** Token bucket shaper index, must be less than QOS_HAL_MAX_SHAPER - 1.
	 */
	uint32_t index;
	/** Related scheduler block input number
	  (0  to  QOS_HAL_MAX_SCHEDULER * 8).*/
	uint32_t tbs_scheduler_block_input;
} __attribute__ ((packed));

/** Token bucket shaper configuration.
*/
struct qos_hal_token_bucket_shaper_cfg {
	/** Token bucket shaper index.
	  Must be less than QOS_HAL_MAX_SHAPER - 1.*/
	uint32_t index;
	/** Token bucket shaper enable.*/
	uint32_t enable;
	/** Token bucket shaper operation mode (0  to  3).*/
	uint32_t mode;
	/** Committed data rate (bytes/s) */
	uint32_t cir;
	/** Peak data rate (bytes/s) */
	uint32_t pir;
	/** Token bucket shaper committed maximum burst size */
	uint32_t cbs;
	/** Token bucket shaper peak maximum burst size */
	uint32_t pbs;
	/** Token bucket shaper will be atached to
	  queue/scheduler */
	uint32_t attach_to;
	/** Token bucket shaper will be atached to
	  queue/scheduler */
	uint32_t attach_to_id;
} __attribute__ ((packed));

/** queue weights */
extern uint16_t qos_hal_equeue_weight[QOS_HAL_MAX_EGRESS_QUEUES];

/* Qos Port configuration.
 */
struct qos_hal_port_cfg_info {
	/** netdevice */
	struct net_device *netdev;
	/** QoS port green bytes threshold */
	uint32_t green_threshold;
	/** QoS port yellow bytes threshold */
	uint32_t yellow_threshold;
};

struct qos_hal_queue_track_info {
	/** Queue usage indication.
	  - 0: Queue is not in use.
	  - 1: Queue is in use. */
	uint32_t  is_enabled;
	/** Queue Id */
	uint32_t  queue_id;
	/** To which Scheduler it is connected */
	uint32_t  connected_to_sched_id;
	/** To which Scheduler input it is connected */
	uint32_t  sched_input;
	/** To which input is connected to Scheduler/Port */
	enum qos_hal_input_type  attach_to;
	/**  Queue type */
	uint32_t  q_type;
	/**  priority/weight */
	uint32_t  prio_weight;
	/**  Tocken Bucket Index */
	uint32_t  tb_index;
	/** Egress Port Number */
	uint32_t  epn;
	/** User Queue Index - for backtracking */
	uint32_t  user_q_idx;
};

struct qos_hal_dp_egress_res {
	uint32_t qidx;
	uint32_t dp_sched;
	uint32_t dp_port;
};

struct qos_hal_dp_res_info {
	struct net_device *netdev;
	struct qos_hal_dp_egress_res dp_egress_res;
	uint32_t q_ingress;
};

struct qos_hal_sched_track_info {
	/** Scheduler usage indication.
	  - 0: Scheduler is not in use.
	  - 1: Scheduler is in use. */
	uint32_t  in_use;
	/** Scheduler ID. */
	uint32_t  id;
	/** Scheduler leaf mask. */
	uint8_t   leaf_mask;
	/** Scheduler policy. */
	uint8_t   policy;
	/** Scheduler level. */
	uint32_t  level;
	/** Peer Scheduler. */
	uint32_t  peer_sched_id;
	/** Next level Scheduler which is
	  connected to its higher prio leaf. */
	uint32_t  next_sched_id;
	/** Scheduler ouput connectivity type. */
	uint32_t  sched_out_conn;
	/** Scheduler ouput index .
	  -0: Output is connected to a physical port
	  -1: Output is connected to another Scheduler */
	uint32_t  omid;
	/** marker for priority level (0-15) mapping identifier for any interface */
	uint32_t  priority_selector;
	/** tocken bucket shaper attached to it */
	uint32_t tbs;
	/**  priority/weight */
	uint32_t  prio_weight;
};

struct qos_hal_port_track_info {
	/** Port usage indication.
	  - 0: Port is not in use.
	  - 1: Port is in use. */
	uint32_t  is_enabled;
	/** leaf mask. */
	uint8_t   leaf_mask;
	/** Scheduling policy. */
	uint8_t   policy;
	/** Scheduler that is connected by default to the port
	  when configured by CBM driver */
	uint32_t  default_sched_id;
	/** Scheduler which is connected to the port */
	uint32_t  input_sched_id;
	/** Port Scheduler for this port
	  Will be required when the Queue will be added for Sub-interface */
	uint32_t  port_sched_id;
	/** Shaper Scheduler for this port
	  Will be added when the port shaping is required */
	uint32_t  shaper_sched_id;
	/** Number of queues connected to the port */
	uint8_t   no_of_queues;
	/** Number of schedulers connected to the port */
	uint8_t   no_of_schedular;
	/** Max levels of schedulers connected to the port */
	uint32_t  no_of_max_level;
	/** tocken bucket shaper attached to it */
	uint32_t tbs;
};

struct qos_hal_shaper_track_info {
	/** Shaper usage indication.
	  - 0: Shaper is not in use.
	  - 1: Port is in use. */
	uint32_t  is_enabled;
	/** Shaper Index */
	uint32_t  shaper_id;
	/** Scheduler input where the tocken bucket is attached */
	uint32_t  sb_input;
	/** Shaper for queue/scheduler */
	uint32_t  attach_to;
	/** Shaper id for queue/scheduler */
	uint32_t  attach_to_id;
	/** Token bucket shaper configuration.*/
	struct qos_hal_token_bucket_shaper_cfg tb_cfg;
};

struct qos_hal_qos_mib {
	uint32_t enqPkts; /* Enqueued packets Count */
	uint32_t enqBytes; /* Enqueued Bytes Count */
	uint32_t deqPkts; /* Dequeued packets Count */
	uint32_t deqBytes; /* Dequeued Bytes Count */
	uint32_t dropPkts; /* Dropped Packets Count */
	uint32_t dropBytes; /* Dropped Bytes Count - UNUSED for now */
	uint32_t qOccPkts; /* Queue Occupancy Packets Count - Only at Queue level */
};

struct qos_hal_subif_mib {
	uint64_t enqPkts; /* Enqueued packets Count */
	uint64_t enqBytes; /* Enqueued Bytes Count */
	uint64_t deqPkts; /* Dequeued packets Count */
	uint64_t deqBytes; /* Dequeued Bytes Count */
	uint64_t dropPkts; /* Dropped Packets Count */
	uint64_t dropBytes; /* Dropped Bytes Count - UNUSED for now */
	uint64_t qOccPkts; /* Queue Occupancy Packets Count - Only at Queue level */
};

struct qos_hal_user_queue_info {
	/** Actual Queue index */
	uint32_t  queue_index;
	/** scheduler input it is connected to */
	uint32_t  sbin;
	/** Priority/WFQ queue */
	uint32_t  queue_type;
	/** Priority level */
	uint32_t  prio_level;
	/** Weight */
	uint32_t  weight;
	/** QID - unique number */
	uint32_t  qid;
	/** QMap BitMask -  QMap table config for this queue*/
	uint32_t  qmap[MAX_TC_NUM];
	/** No of TC mapped to this queue */
	uint8_t   no_of_tc;
	/** Drop params for this queue */
	struct qos_hal_equeue_cfg ecfg;
	/** Queue MIB */
	struct qos_hal_qos_mib q_mib;
};

struct qos_hal_dp_subif {
	/** PortId */
	int32_t port_id;
	/** SubIf */
	int32_t subif:15;
	/** Ifname */
	char ifname[16];
};

struct qos_hal_user_subif_abstract {
	QOS_MGR_ATOMIC				count;
	struct qos_hal_user_subif_abstract	*head;
	struct qos_hal_user_subif_abstract	*next;
	/** Physical or logical port */
	uint32_t  is_logical;
	/** QOS port index */
	uint32_t  qos_port_idx;
	/** net device of this interface */
	struct net_device *netdev;
	/** Base scheduler id of the interface */
	uint32_t  base_sched_id_egress;
	/** Port scheduler input - reqiured for sub interface */
	uint32_t  port_sched_in_egress;
	/** Port scheduler input user priority level- reqiured for sub interface */
	uint32_t  port_sched_in_user_lvl;
	/** Port Shaper scheduler - reqiured for sub interface */
	uint32_t  port_shaper_sched;
	/** Shaper index - reqiured for sub interface */
	uint32_t  shaper_idx;
	/** Subif weight connected to port  - reqiured for sub interface */
	uint32_t  subif_weight;
	/** Total number of queues for this physical/logical interface */
	uint32_t  queue_cnt;
	/** Last assigned User QID  */
	uint32_t  qid_last_assigned;
	/** Default Qos Queue  */
	uint32_t  default_q;
	/** Default Qos priority */
	uint32_t  default_prio;
	/** lookup mode */
	int32_t lookup_mode;
	/** DP port-subif information */
	struct qos_hal_dp_subif dp_subif;
	/**  All the Queue details of this egress interface */
	struct qos_hal_user_queue_info user_queue[QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE];
	/** MIB */
	struct qos_hal_subif_mib subif_mib;
};

int32_t qos_hal_init(void);
int32_t qos_hal_exit(void);
int32_t __qos_hal_spl_conn_init(uint8_t conn_type, uint32_t queue_len);

/** Buffers to track the Scheduler info */
extern struct qos_hal_sched_track_info  qos_hal_sched_track[QOS_HAL_MAX_SCHEDULER];
/** Buffers to track the Port info */
extern struct qos_hal_port_track_info qos_hal_port_track[QOS_HAL_MAX_EGRESS_PORT];
/** Buffers to track the Queue info */
extern struct qos_hal_queue_track_info qos_hal_queue_track[QOS_HAL_MAX_EGRESS_QUEUES];
/** Buffers to track the Shaper info */
extern struct qos_hal_shaper_track_info qos_hal_shaper_track[QOS_HAL_MAX_SHAPER];
/** Buffers to track the User level request for Egress Queue  */
extern struct qos_hal_user_subif_abstract *qos_hal_user_sub_interface_info ;
/** Buffers to track the User level request for Ingress Queue  */
extern struct qos_hal_user_subif_abstract *qos_hal_user_sub_interface_ingress_info ;

extern struct qos_hal_dp_res_info qos_hal_dp_res[QOS_HAL_MAX_DP_RES];
#define set_val(reg, val, mask, offset) do {(reg) &= ~(mask); (reg) |= (((val) << (offset)) & (mask)); } while (0)
#define get_val(val, mask, offset) (((val) & (mask)) >> (offset))
