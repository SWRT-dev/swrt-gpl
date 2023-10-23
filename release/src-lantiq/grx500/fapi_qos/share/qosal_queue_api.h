/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __QOS_AL_Q_API_H /* [ */
#define __QOS_AL_Q_API_H
/*! \file qosal_queue_api.h
    \brief File contains function defintions and data structures used to perform queuing operations in 
           QoS Queuing FAPI
*/

/** \defgroup FAPI_QOS_QUEUE Queuing
*   @ingroup FAPI_QOS
    \brief Provides a set of API to create queue for an interface,
           configure shaper on a queue/port, etc
*/

/** \addtogroup FAPI_QOS_GENERAL */
/* @{ */
/*!
    \brief QoS general configuration data structure
*/
typedef struct qos_cfg {
	int32_t ena; /*!< Overall QoS enable flag */
	int32_t us_ena; /*!< QoS enable flag for upstream (LAN-WAN) direction */
	int32_t ds_ena; /*!< QoS enable flag for downstream (WAN-LAN) direction */
	//int32_t log_ena; /*!< Log enable flag */
	int32_t log_level; /*!< Log enable flag */
	log_type_t log_type; /*!< Log type */
	char log_file[MAX_NAME_LEN]; /*!< Log file name */
	uint32_t def_wan_dscp; /*!< DSCP value for unclassified traffic in upstream (LAN-WAN) direction */
	int32_t wmm_ena; /*!< WMM QoS enable flag */
	int32_t mgmt_ena; /*!< Management traffic prioritization enable flag */
	uint32_t wan_port_rate_limit; /*!< Limit rate of WAN interface/port */
	char version[64];
} qos_cfg_t;
/* @} */

/** \addtogroup FAPI_QOS_GENERAL */
/* @{ */
/*! \brief  Set QoS general configuration
   \param[out] qoscfg  QoS general configuration
   \param[in] flags Reserved currently
   \return 0 on successful / -1 on failure
*/
int32_t
fapi_qos_cfg_get(qos_cfg_t *qoscfg, uint32_t flags);
/*! \brief  Set QoS general configuration
   \param[in] qoscfg  QoS general configuration
   \param[in] flags Specified type of operation
	- QOS_CFG_MODIFY : modify the configuration
   \return 0 on successful / -1 on failure
*/
int32_t
fapi_qos_cfg_set(qos_cfg_t *qoscfg, uint32_t flags);
/*! \brief  QoS configuration init
   \param[in] qoscfg QoS AL general configuration
   \param[in] flags Specified type of operation
	- QOS_CFG_MODIFY : modify the configuration
   \return 0 on successful / -1 on failure
*/
int32_t
fapi_qos_init(qos_cfg_t *qoscfg, uint32_t flags);
/* @} */

/** \addtogroup FAPI_QOS_QUEUE */
/* @{ */
/*!
  \brief QOS_AL_Q_OK */
#define QOS_AL_Q_OK			0
/*!
  \brief QOS_AL_Q_ERROR */
#define QOS_AL_Q_ERROR			-1

/*!
  \brief CL action traffic class
*/
#define CL_ACTION_TC			0x1
/*!
  \brief Classifier action drop
  \note DROP is mutually exclusive with any other classifier action - all other
  actions could be combined on a packet
*/
#define CL_ACTION_DROP			0x2
/*!
  \brief Classifier action remark DSCP value
*/
#define CL_ACTION_DSCP			0x4
/*!
  \brief Classifier action remark VLAN priority
*/
#define CL_ACTION_VLAN			0x8
/*!
  \brief Classifier action policer
*/
#define CL_ACTION_POLICER		0x10
/*!
  \brief Classifier action policy route
*/
#define CL_ACTION_POLICY_ROUTE		0x20
/*!
  \brief Classifier action for accelerating session or not
*/
#define CL_ACTION_ACCELERATION          0x40
/*!
  \brief Classifier internal flag to indicate required chains in iptables created
*/
#define CL_INT_IPT_CH_CREATED		0x80
/*!
  \brief Classifier TC marking action location (ingress/egress)
*/
#define CL_ACTION_TC_INGRESS		0x100

/*!
  \brief Classifier type representation for MPTCP Classifier
*/
#define CL_ACTION_CL_MPTCP		0x200

/*!
  \brief Classifier type representation for Classification in Software Engine
*/
#define CL_ACTION_CL_SW_ENGINE		0x400

/*!
  \brief Classifier type representation for Classification in Hardware Engine
*/
#define CL_ACTION_CL_HW_ENGINE		0x800

/*!
  \brief Classifier TC marking action ingress as well as egress
  */
#define CL_ACTION_TC_INGRESS_EGRESS		0x1000

/*!
  \brief Classifier Set Ingress  TC
  */
#define CL_ACTION_INGRESS_TC_SET		0x2000

/*!
  \brief Classifier Set Egress  TC
  */
#define CL_ACTION_EGRESS_TC_SET		0x4000
/*!
  \brief Classifier action for meter
  */
#define CL_ACTION_METER		0x8000
/*!
  \brief Classifier action for only CPU packet
  */
#define CL_ACTION_ONLY_CPU		0x10000
/*!
  \brief MAX_Q_NAME_LEN */
#define MAX_Q_NAME_LEN 128
/*!
  \brief MAX_TC_NUM */
#define MAX_TC_NUM 16
#define MAX_PRIO	16
/*!
  \brief MAX_METER_NUM */
#define MAX_METER_NUM	31

/*!
    \brief Default queue identifier; Queues with name as QOS_DEFAULT_QUEUE_NAME are considered default queues
*/
#define QOS_DEFAULT_QUEUE_NAME "def_queue"

/*!
    \brief QoS Shaper Mode Configuration
*/
typedef enum {
	QOS_SCHED_SP=0, /*!< Queue scheduling algorithm is strict priority */
	QOS_SCHED_WFQ=1, /*!< Queue scheduling algorithm is weighted fair queuing */
}qos_sched_mode_t;

#if 0
/*!
    \brief QoS Queue Location[Ingress/Egress] Configuration
*/
typedef enum {
	QOS_Q_EGRESS=0, /*!< Queue scheduling algorithm is strict priority */
	QOS_Q_INGRESS=1, /*!< Queue scheduling algorithm is weighted fair queuing */
}qos_queue_loc_t;
#endif
/*!
    \brief QoS Ingress Group Configuration
*/
typedef enum {
	QOS_INGGRP_1=0, /*!< Queue scheduling algorithm is strict priority */
	QOS_INGGRP_2=1, /*!< Queue scheduling algorithm is weighted fair queuing */
	QOS_INGGRP_3=2, /*!< Queue scheduling algorithm is weighted fair queuing */
	QOS_INGGRP_4=3, /*!< Queue scheduling algorithm is weighted fair queuing */
}qos_inggrp_t;

/*!
    \brief QoS Shaper Mode Configuration
*/
typedef enum {
	QOS_SHAPER_NONE=0, /*!< No shaper assigned */
	QOS_SHAPER_COLOR_BLIND=1, /*!< Color blind */
	QOS_SHAPER_TR_TCM=2, /*!< Three-color marking */
	QOS_SHAPER_TR_TCM_RFC4115=3, /*!< Three-color marking as in RFC4115 */
	QOS_SHAPER_LOOSE_COLOR_BLIND=4 /*!< Loose color blind */
}qos_shaper_mode_t;

/*!
    \brief QoS Queue drop Mode Configuration
*/
typedef enum {
	QOS_DROP_TAIL=0, /*!< Tail-drop */
	QOS_DROP_RED=1, /*!< Random early detection */
	QOS_DROP_WRED=2, /*!< Weighted random early detection */
	QOS_DROP_CODEL=3 /*!< Controlled delay */
}qos_drop_mode_t;

/*!
    \brief Peak shaper info configured
*/
#define QOS_SHAPER_F_PIR 0x00000002
/*!
    \brief Committed shaper info configured
*/
#define QOS_SHAPER_F_CIR 0x00000004
/*!
    \brief Internally used by ppa engine
*/
#define QOS_SHAPER_F_CLST_RATE 0x80000000

/*!
    \brief QoS Shaper configuration structure
*/
typedef struct qos_shaper {
	char 			name[MAX_Q_NAME_LEN];
	qos_shaper_mode_t	mode; /*!< Mode of Token Bucket shaper */
	int32_t			enable; /*!< Whether shaper is enabled */
	uint32_t		cir; /*!< Committed Information Rate in bytes/s */
	uint32_t		cbs; /*!< Committed Burst Size in bytes */
	uint32_t		pir; /*!< Peak Information Rate in bytes/s */
	uint32_t		pbs; /*!< Peak Burst Size */
	uint32_t		flags; /*!< Flags define which shapers are enabled, as follows:
						 - QOS_Q_F_INGRESS
						 - QOS_SHAPER_F_PIR
						 - QOS_SHAPER_F_CIR */
	qos_inggrp_t	inggrp; /*!< QoS Ingress Group - Group1,Group2,Group3,Group4 */
}qos_shaper_t;

/*!
    \brief QoS meter mode configuration
*/
typedef enum {
	QOS_METER_SR_TCM=0, /*!< Two-color marking */
	QOS_METER_TR_TCM=1, /*!< Three-color marking as in RFC4115 */
}qos_meter_mode_t;

/*!
    \brief QoS meter configuration structure
*/
typedef struct qos_meter {
	int32_t				enable; /*!< Whether meter is enabled */
	qos_meter_mode_t		mode; /*!< Mode of color marking in meter */
	uint32_t			cir; /*!< Committed Information Rate in kilobytes/s */
	uint32_t			cbs; /*!< Committed Burst Size */
	uint32_t			pir; /*!< Peak Information Rate in kilobytes/s */
	uint32_t			pbs; /*!< Peak Burst Size */
	uint32_t			meterid; /*!< Meter ID configured in system */
	uint32_t			flags; /*!< Flags */
}qos_meter_t;

/*!
    \brief QoS WRED Drop configuration structure
    \remark If RED, then this is same as WRED Curve 0 only configuration 
*/
typedef struct qos_wred {
	uint32_t	weight; /*!< Weighting factor for QAVG calculation */
	uint32_t	min_th0; /*!< Minimum Threshold for WRED Curve 0 in % of qlen */
	uint32_t	max_th0; /*!< Maximum Threshold for WRED Curve 0 in % of qlen */
	uint32_t	max_p0; /*!< Maximum Drop Probability % at max_th0 for WRED Curve 0 */
	uint32_t	min_th1; /*!< Minimum Threshold for WRED Curve 1 in % of
				  qlen*/
	uint32_t	max_th1; /*!< Maximum Threshold for WRED Curve 1 in % of
				  qlen*/
	uint32_t	max_p1; /*!< Maximum Drop Probability % at max_th1 for WRED Curve 1 */
	uint32_t	drop_th1; /*!< Maximum Queue Threshold value for red color packet */
}qos_wred_t;

/*!
    \brief Drop tail and wred related drop flags
*/
#define QOS_DT_F_GREEN_THRESHOLD 0x00000001 /*!< Green color threshold drop tail flag */
#define QOS_DT_F_YELLOW_THRESHOLD 0x00000002 /*!< Yellow color threshold drop tail flag*/
#define QOS_DT_F_RED_THRESHOLD 0x00000004 /*!< Red color threshold drop tail flag*/
#define QOS_MAX_Q_LENGTH_THRESHOLD 2304 /*!< Maximum value for Green, Yellow and Red color threshold drop*/
/*!
    \brief QoS Queue Drop configuration structure
*/
typedef struct qos_drop_cfg {
	int32_t			enable; /*!< Whether shaper is enabled */
	qos_drop_mode_t		mode; /*!< Mode of Queue Drop - Taildrop, WRED
				       */
	qos_wred_t		wred; /*!< WRED configuration of the queue */
	int32_t			flags; /*!< Drop flags values are:
                                        - 0x00000001 - Green Threshold
                                        - 0x00000002 - Yellow Threshold
                                        - 0x00000004 - Red Threshold */

}qos_drop_cfg_t;

/*!
    \brief QoS queue operation related flags
*/
#define	QOS_Q_F_INGRESS 0x10000000 /*!< Ingress operation flag */
#define	QOS_Q_F_MODIFIED 0x00000002 /*!< Modify operation flag */
#define	QOS_Q_F_MODIFIED_OTHER 0x00000004 /*!< Modify other operation flag */
#define	QOS_Q_F_HW 0x00000008 /*!< Hardware operation flag */
#define	QOS_Q_F_CPU 0x00000010 /*!< Software operation flag */
#define	QOS_Q_F_DEFAULT 0x00000020 /*!< Default Queue operation flag */
#define	QOS_Q_F_SKIP_DEF_Q_CHK 0x00000040 /*!< Default Queue check skip flag */
#define QOS_Q_F_INT_Q 0x00000080 /*!< Internal queue identification flag */
#define QOS_Q_F_LINK_STATE_CHANGE 0x00004000 /*!< Configuration triggered from link state change */
#define QOS_Q_F_WFQ_UPDATE 0x00008000 /*!< WFQ Weight change identification flag */
#define QOS_Q_F_DS_QOS 0x00000100 /*INGRESS DS QOS Option*/

/*!
    \brief QoS Queue Configuration structure
*/
typedef struct qos_queue_cfg {
	uint32_t	enable; /*!< Whether Queue is enabled */
	uint32_t	status; /*!< Operational status of the queue. Only
				  valid on GET */
	char		name[MAX_Q_NAME_LEN]; /*!< Name or Alias of the
						  queue, if any */
	uint32_t	owner; /*!<  Enumerated value of who is doing the operation.
					Possible owners can be: TR-069, Web GUI, CLI, System, BaseRules */
	uint32_t	flags; /*!< Following flags can be kept per
				 filter structure: 
		 - QOS_Q_F_MODIFIED : Modified after being originally added 
		 - QOS_Q_F_MODIFIED_OTHER : Modified by an entity other than the owner 
		 - QOS_Q_F_HW : Set if this Q is in HW 
		 - QOS_Q_F_INGRESS  : This queue is applied to ingress from the interface */
	uint32_t	queue_id; /*!< Queue Id for other operations */
	int32_t		qlen; /*!< Length of Queue in bytes */
	int32_t		weight; /*!< WFQ Weight */
	int32_t		priority; /*!< Queue Priority or Precedence. Start from 1-16, with 1 as highest priority */
	char		tc_map[MAX_TC_NUM]; /*!< Which all Traffic Class(es) map to this Queue */
	/* ?? What about additional qselectors possible in GRX350 */
	qos_drop_cfg_t	drop; /*!< Queue Drop Properties */
	qos_sched_mode_t	sched; /*!< QoS scheduler mode - Priority, WFQ */
	qos_inggrp_t	inggrp; /*!< QoS Ingress Group - Group1,Group2,Group3,Group4 */
	qos_shaper_t	shaper; /*!< Shaper Configuration */
} qos_queue_cfg_t;

/*!
    \brief QoS queue statistics related flags
*/
#define	QOS_Q_STAT_RX_PKT 0x00000001 /*!< Rx Packets support flag */
#define	QOS_Q_STAT_RX_BYTE 0x00000002 /*!< Rx Bytes support flag */
#define	QOS_Q_STAT_TX_PKT 0x00000004 /*!< Tx Packets support flag */
#define	QOS_Q_STAT_TX_BYTE 0x00000008 /*!< Tx Bytes support flag */
#define	QOS_Q_STAT_DROP_PKT 0x00000010 /*!< Drop Packets support flag */
#define	QOS_Q_STAT_DROP_BYTE 0x00000020 /*!< Drop Bytes support flag */
#define	QOS_Q_STAT_OCC_PKT 0x00000040 /*!< Queue Occupacny Packets support flag */
#define	QOS_Q_STAT_OCC_PCT 0x00000080 /*!< Queue Occupacny Percentage support flag */

/*!
    \brief QoS Queue statistics structure
*/
typedef struct {
	char            name[MAX_Q_NAME_LEN]; /*!< Name or Alias of the
                                                  queue, if any, which maps to unique instance of qos_queue_cfg_t object */
	uint64_t        rx_pkt;   /*!<  All packets received by this queue */
	uint64_t        rx_bytes; /*!<  All bytes received by this queue */
	uint64_t        tx_pkt;   /*!<  All packets trasmitted by this queue */
	uint64_t        tx_bytes; /*!<  All bytes trasmitted by this queue */
	uint64_t        drop_pkt;   /*!<  All packets dropped by this queue */
	uint64_t        drop_bytes; /*!<  All bytes dropped by this queue */
	uint64_t        occ_pkt;   /*!<  Queue occupancy packets */
	uint32_t        occ_pct;   /*!<  Queue occupancy percentage */
	uint32_t	      flags; /*!< Flags to indicate supported stats */
} qos_queue_stats_t;

/*defined for FAPI flags used in FAPI calls, Only 16 Least Significant Bits can be used, 16 MSB are reserved for internal use */
/*!
  \brief Queue add operation flag
*/
#define QOS_OP_F_ADD 0x00000001
/*!
  \brief Queue modify operation flag
*/
#define QOS_OP_F_MODIFY 0x00000002
/*!
  \brief Queue delete operation flag
*/
#define QOS_OP_F_DELETE 0x00000004
/*!
  \brief Queue drop algorithm set operation flag
*/
#define QOS_OP_F_QUEUE_DROP 0x00000008
/*!
  \brief Queue scheduler algorithm set operation flag
*/
#define QOS_OP_F_QUEUE_SCHED 0x00000010
/*!
  \brief Queue shaper set operation flag
*/
#define QOS_OP_F_QUEUE_SHAPER 0x00000020
/*!
  \brief Dump QoS configuration
*/
#define QOS_OP_CFG_DUMP 0x00000040
/*!
  \brief Ingress operation flag
*/
#define QOS_OP_F_INGRESS 0x00000080
/*!
  \brief Flag to indicate not to use default general configuration during init
*/
#define QOS_DONT_USE_DEF_CFG 0x00000100
/*!
  \brief Queue length threshold operation flag
*/
#define QOS_OP_F_QUEUE_LENGTH 0x00000400
/*!
  \brief Flag to indicate not to add line rate shaper
*/
#define QOS_NOT_ADD_DEF_SHAPER 0x00000800

#define MAX_IFNAME_LEN 32

/*!
    \brief QoS Queue Configuration structure used for inter process communication
*/
struct x_qapi_t
{
	char	acifname[MAX_IFNAME_LEN]; /*!< Inteface name */
	uint32_t uiflags; /*!< Operational flags */
	int32_t inumqs; /*!< Number of queues - used in GET operation */
	/* Parameters from this will be used for other APIs as applicable */
	qos_queue_cfg_t xqcfg; /*!< QOS configuration structure */
};

/*!
    \brief QoS Queue General configuration structure used for inter process communication
*/
struct x_qoscfg_t {
	uint32_t uiflags; /*!< Operational flags */
	qos_cfg_t qoscfg; /*!< QOS general configuration structure */
};

/*!
    \brief QoS interface mapping structure for interface process communication
*/
struct x_ifcfg_t {
	uint32_t uiFlags; /*!< Operational flags */
	char ifnames[20][MAX_IF_NAME_LEN]; /*!< List of interface names */
	char base_ifnames[20][MAX_IF_NAME_LEN]; /*!< List of base interface names */
	char logical_base[20][MAX_IF_NAME_LEN]; /*!< List of logical base interface names */
	iftype_t ifgroup; /*!< Interface category */
	ifinggrp_t ifinggrp; /*!< Interface ingress group */
	int32_t num_entries; /*!< Number of interfaces under given interface category */
};

/* @} */



/** \addtogroup FAPI_QOS_QUEUE */
/* @{ */
/*! \brief  Set QoS Queue Configuration - involves adding a new queue or
 * modifying an existing queue, or deleting an existing queue
   \param[in] ifname  Name of interface on which queue is to be added (egress or
   ingress)
   \param[in] q Queue configuration to add, modify or delete
   \param[in] flags Specifies if Filter is to do one of the following:
   	 - QOS_OP_F_ADD
	 - QOS_OP_F_MODIFY
	 - QOS_OP_F_DELETE
	 - QOS_Q_F_QUEUE_GENERIC_MODIFY
	 - QOS_Q_F_QUEUE_DROP_MODIFY
	 - QOS_Q_F_QUEUE_SCHED_MODIFY
	 - QOS_Q_F_QUEUE_SHAPER_MODIFY
	 - QOS_Q_F_QUEUE_SHAPER_DELETE
   \return QueueId on successful / -1 on failure
*/
int32_t
fapi_qos_queue_set(
	char			*ifname,
	qos_queue_cfg_t		*q,
	uint32_t 		flags
	);

/*! \brief  Get one of all QoS Queue Configuration on given interface
   \param[in] ifname  Name of interface on which queue is to be queried (egress or
   ingress)
   \param[in] queue_name  Name of queue which is to be queried
   \param[out] num_queues Number of queues returned
   \param[out] q Array of Queue configuration of num_queues number of queues
   Caller must free the buffer
   \param[in] flags Specifies if the Filter is to do one of the following:
	- QOS_OP_F_INGRESS - if ingress queues are to be retrieved, otherwise
	egress queues are retrieved
   \return 0 on successful / -1 on failure
   \remark if QueueId is -1, then all queues for if_name are returned (number of queues given by num_queues), else only
   queue given by queue_id is returned
*/
int32_t
fapi_qos_queue_get(
	char			*ifname,
	char*			queue_name,
	int32_t			*num_queues,
	qos_queue_cfg_t		**q,
	uint32_t 		flags
	);


/*! \brief  Get statistics of one or all Queues on given interface
   \param[in] ifname  Name of interface on which queue is to be queried (egress or
   ingress)
   \param[in] queue_name  Name of queue for which statistics are to be queried
   \param[out] num_queues Number of queues returned
   \param[out] qstats Array of Queue statistics of num_queues number of queues
   Caller must free the buffer
   \param[in] flags Specifies whether Filter is to do one of the following:
	QOS_OP_F_INGRESS - if ingress queues statistics are to be retrieved, otherwise
	egress queues statistics are retrieved
   \return 0 on successful / -1 on failure
   \remark if queue_name is not specified or NULL, then statistics of all queues for if_name are returned
   (number of queues given by num_queues), else only statistics of queue given by queue_name is returned
*/
int32_t
fapi_qos_queue_stats_get(
	char			*ifname,
	char*			queue_name,
	int32_t			*num_queues,
	qos_queue_stats_t	**qstats,
	uint32_t 		flags
	);

/*! \brief  Set Port QoS characteristics like rate shaper, and/or
 * weight/priority if this port is cascaded into another scheduler
   \param[in] ifname  Name of interface whose port QoS characteristics are to be
   configured
   \param[in] shaper  Port shaper configuration
   \param[in] weight  Port weight when port is cascaded into next stage
   scheduler. -1 means no weight, only priority
   \param[in] priority  Port priority when port is cascaded into next stage
   scheduler. -1 means no priority, only weight
   \param[in] flags Flags are defined as follows:
   	 - QOS_OP_F_ADD
	 - QOS_OP_F_MODIFY
	 - QOS_OP_F_DELETE
   \return 0 on successful / -1 on failure
*/

int32_t
fapi_qos_port_config_set(
	char			*ifname,
	qos_shaper_t		*shaper,
	int32_t			weight,
	int32_t			priority,
	uint32_t 		flags
	);

/*! \brief  Get Port QoS characteristics like rate shaper, and/or
 * weight/priority if this port is cascaded into another scheduler
   \param[in] ifname  Name of interface whose port QoS characteristics are to be
   configured
   \param[out] shaper  Port shaper configuration
   \param[out] weight  Port weight when port is cascaded into next stage
   scheduler. -1 means no weight, only priority
   \param[out] priority  Port priority when port is cascaded into next stage
   scheduler. -1 means no priority, only weight
   \param[in] flags Flags are defined as follows:
   	- QOS_OP_F_INGRESS : if port egress or ingress configuration is to be
	returned
   \return 0 on successful / -1 on failure
*/
int32_t
fapi_qos_port_config_get(
	char			*ifname,
	qos_shaper_t		*shaper,
	int32_t			*weight,
	int32_t			*priority,
	uint32_t 		flags
	);
/*! \brief  Notification on port add or delete in the system from higher
  layers/system management. This allows qos_al to add/del queues from template
  files to system
   \param[in] ifname  Interface which is to be added or deleted from QoS
   configuration of system
   \param[in] flags Flags are as as follows: 
   	 - QOS_OP_F_ADD : Add QoS Port
	 - QOS_OP_F_DELETE : Delete QoS Port
   \return 0 on successful / -1 on failure
*/
int32_t
fapi_qos_port_update(
	char			*ifname,
	uint32_t 		flags
	);

/*!< Check if All Mandatory config are set for Queue Ingress */
#define QOS_Q_CHECK_INGRESS_MANDATORY_CONFIG \
	((QOS_Q_CHECK_TC_SET) | (QOS_Q_CHECK_PRIO_SET) | (QOS_Q_CHECK_INTERFACE_SET) | \
	(QOS_Q_CHECK_SCHED_ALGO_SET) | (QOS_Q_CHECK_DROP_ALGO_SET) | \
	(QOS_Q_CHECK_QNAME_SET) | (QOS_Q_CHECK_ING_GRP_SET))

/*!< Check if All Mandatory config are set for Queue Egress*/
#define QOS_Q_CHECK_EGRESS_MANDATORY_CONFIG \
	((QOS_Q_CHECK_TC_SET) | (QOS_Q_CHECK_PRIO_SET) | (QOS_Q_CHECK_INTERFACE_SET) | \
	(QOS_Q_CHECK_SCHED_ALGO_SET) | (QOS_Q_CHECK_DROP_ALGO_SET) | \
	(QOS_Q_CHECK_QNAME_SET))

#define QOS_Q_CHECK_TC_SET						0x00000001 /*!< Check if the TC value for the queue is set */
#define QOS_Q_CHECK_PRIO_SET					0x00000002 /*!< Check if the Priority Value for the queue is set*/
#define QOS_Q_CHECK_INTERFACE_SET				0x00000004 /*!< Check if the Interface Name for the queue is set*/
#define QOS_Q_CHECK_SCHED_ALGO_SET				0x00000008 /*!< Check if the Scheduling Algo for the queue is set*/
#define QOS_Q_CHECK_DROP_ALGO_SET				0x00000010 /*!< Check if the Drop Algorithm for the queue is set*/
#define QOS_Q_CHECK_QNAME_SET					0x00000020 /*!< Check if the Queue Name for the queue is set*/
#define QOS_Q_CHECK_ING_GRP_SET					0x00000040 /*!< Check if the Ingress Group for the queue is set*/
#define QOS_Q_CHECK_WEIGHT_SET					0x00000080 /*!< Check if the Weights for WFQ is updated */

#define MAX_Q_WEIGHT			100 /*!< Maximum weigth for the queue if Sched Algo is WFQ*/
#define MIN_Q_WEIGHT			0 /*!< Maximum weigth for the queue if Sched Algo is WFQ*/
#define MAX_Q_PRIO_RANGE		16 /*!< Maximum Queue Priority Range */
#define MIN_Q_PRIO_RANGE		1 /*!< Minimum Queue Priority Range */
#define MAX_SHAPER_RANGE		2500000 /*!< Input provided in Kb, so max shaper range is 2.5G */
#define MAX_QUEUE_NAME_LEN		 19 /*!< Maximum Queue/Classifier Name Length Supported */
#define MAX_RULES				20 /*Maximum Classifiers that can be configured on an interface*/

#define VALIDATE_RANGE(value, min, max) \
	((value >= min && value <= max )? 1:0)
/* @} */
#endif
