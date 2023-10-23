// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2019~2020 Intel Corporation.
 */
#ifndef __QOS_MGR_COMMON_H_
#define __QOS_MGR_COMMON_H
/*! \file qos_mgr_common.h
  \brief This file contains: QOS Manager Commonly used Structure Definitions
  and helper MACROs
*/
#include <uapi/net/qos_mgr_common.h>
#include <stdbool.h>
/*
 * ####################################
 * Definition
 * ####################################
 */

#define VLAN_ID_SPLIT(full_id, pri, cfi, vid) \
	do { \
		pri = ((full_id) >> 13) & 7; \
		cfi = ((full_id) >> 12) & 1; \
		vid = (full_id) & 0xFFF \
	} while (0)

#define VLAN_ID_CONBINE(full_id, pri, cfi, vid) \
	full_id	= (((uint16_t)(pri) & 7) << 13) | \
				(((uint16_t)(cfi) & 1) << 12) | \
				((uint16_t) (vid) & 0xFFF)

#define WRAPROUND_32BITS ((uint64_t)0xFFFFFFFF)
#define WRAPROUND_64BITS ((uint64_t)0xFFFFFFFFFFFFFFFF)
#if !(IS_ENABLED(CONFIG_SOC_GRX500))
#define WRAPROUND_SESSION_MIB ((uint64_t)0x1FFFFFE0) /*note, 0xFFFFFF * 0x20. In PPE FW, 1 means 32 bytes, ie 0x20 this value will be different with GRX500 platform */
#else
#define WRAPROUND_SESSION_MIB WRAPROUND_32BITS
#endif

/*!
	\brief QoS HAL Rate Shaping configuration structure
 */
typedef struct {
	QOS_MGR_IFNAME ifname[QOS_MGR_IF_NAME_SIZE];/*!< ifname of the interface on which the Queues & its shapers exist */
	char *dev_name;/*!<	dev name of the base interface for interface like pppoatm which do not have base netif */
	uint32_t portid;/*!< Port Id corresponding the interface/netif on which Queues/shapers exist */
	uint32_t queueid;/*!< Logical queue id while creating queue/physical queue id passed to TMU while shaper->queue assign */
	int32_t shaperid;/*!< Logical shaper id while creating shaper/physical shaper id passed to TMU while shaper->queue assign */
	int32_t phys_shaperid;/*!< Physical shaper ID returned by TMU when a shaper is created*/
	QOS_MGR_SHAPER_CFG shaper;/*!< Shaper Info*/
	uint32_t rate_in_kbps;/*!< Peak rate in kbps*/
	uint32_t burst;/*!< Peak burst in kbps*/
	uint32_t flag;/*!< Flags for further use */
} QOS_RATE_SHAPING_CFG;
/*!
	\brief QoS Queue HAL DEL configuration structure
 */
typedef struct qos_q_del_cfg{
	char		*ifname;/*!< Interface name on which the Queue is modified*/
	char		*dev_name;/*!<	dev name of the base interface for interface like pppoatm which do not have base netif */
	int32_t		priority; /*!< Queue Priority or Precedence. Start from 1-16, with 1 as highest priority */
	int32_t		q_id; /*!< Original Queue id of the queue to be deleted */
	uint16_t 	intfId;
	uint32_t	portid;/*!< Portid*/
	uint32_t	flags;/*!< Flags for future use*/
} QOS_Q_DEL_CFG;
/*!
	\brief QoS Queue HAL ADD configuration structure
 */
typedef struct qos_q_add_cfg{
	char		*ifname;/*!< Interface name on which the Queue is modified*/
	char		*dev_name;/*!<	dev name of the base interface for interface like pppoatm which do not have base netif */
	char		tc_map[MAX_TC_NUM];/*!< Which all Traffic Class(es) map to this Queue */
	uint8_t		tc_no; /*!< Number of Traffic Class(es) map to this Queue */
	uint8_t		intfId_en;/*!<Enable/Disable for flow Id + tc bits used for VAP's & Vlan interfaces*/
	int32_t		weight; /*!< WFQ Weight */
	int32_t		priority; /*!< Queue Priority or Precedence. Start from 1-16, with 1 as highest priority */
	int32_t		qlen; /*!< Length of Queue in bytes */
	int32_t		q_id; /*!< Original Queue id of the Queue added */
	uint32_t	portid;/*!< Portid*/
	uint16_t	intfId;/*!< flow Id + tc bits used for VAP's & Vlan interfaces*/
	/* QOS_MGR_QSCHED_MODE may not be required as tmu hal can internaly find this out: Need to remove later*/
	QOS_MGR_QSCHED_MODE	q_type;/*!< Scheduler type */
	QOS_MGR_DROP_CFG	drop; /*!< Queue Drop Properties */
	uint32_t		flags;/*!< Flags for future use*/
} QOS_Q_ADD_CFG;
/*!
	\brief QoS Queue HAL MOD configuration structure
 */
typedef struct qos_q_mod_cfg{
	char		*ifname;/*!< Interface name on which the Queue is modified*/
	uint32_t	enable; /*!< Whether Queue is enabled */
	int32_t		weight; /*!< WFQ Weight */
	int32_t		priority; /*!< Queue Priority or Precedence. Start from 1-16, with 1 as highest priority */
	int32_t		qlen; /*!< Length of Queue in bytes */
	int32_t		q_id; /*!< Original Queue id of the Queue modified */
	uint32_t	portid;/*!< Portid*/
	QOS_MGR_QSCHED_MODE	q_type;/*!< Scheduler type */
	QOS_MGR_DROP_CFG	drop; /*!< Queue Drop Properties */
	uint32_t		flags;/*!< Flags for future use*/
} QOS_Q_MOD_CFG;
/*!
	\brief QoS Modify QOS Sub interface to Port configuration structure
 */
typedef struct qos_mod_subif_port_cfg{
	char		ifname[QOS_MGR_IF_NAME_SIZE];/*!< Interface name on which the Queue is modified*/
	uint32_t	port_id;/*!< Portid*/
	int32_t		priority_level; /*!< Queue Priority or Precedence. Start from 1-16, with 1 as highest priority */
	int32_t		weight; /*!< WFQ Weight */
	uint32_t	flags;/*!< Flags for future use*/
} QOS_MOD_SUBIF_PORT_CFG;
/*!
	\brief QoS Add Queue Configuration structure
 */
typedef struct qos_mgr_add_queue_cfg {
	char		tc_map[MAX_TC_NUM];/*!< Which all Traffic Class(es) map to this Queue */
	uint8_t		tc_no; /*!< Number of Traffic Class(es) map to this Queue */
	uint8_t		intfId_en;/*!<Enable/Disable for flow Id + tc bits used for VAP's & Vlan interfaces*/
	uint16_t	intfId;/*!< flow Id + tc bits used for VAP's & Vlan interfaces */
	uint32_t	portid;/*!< PORT ID */
	int32_t		weight; /*!< WFQ Weight */
	int32_t		priority; /*!< Queue Priority or Precedence. Start from 1-16, with 1 as highest priority */
	int32_t		qlen; /*!< Length of Queue in bytes */
	QOS_MGR_QSCHED_MODE q_type; /*!< QoS scheduler mode - Priority, WFQ */
	uint32_t	queue_id;/*!< Physical Queue id of Queue that is added*/
	QOS_MGR_DROP_CFG	drop; /*!< Queue Drop Properties */
} QOS_MGR_ADD_QUEUE_CFG;
/*!
  \brief QoS Meter configuration structure
 */
typedef struct {
	QOS_MGR_METER_TYPE	type; /*!< Mode of Meter*/
	uint32_t		enable; /*!< Enable for meter */
	uint32_t		cir; /*!< Committed Information Rate in bytes/s */
	uint32_t		cbs; /*!< Committed Burst Size in bytes */
	uint32_t		pir; /*!< Peak Information Rate in bytes/s */
	uint32_t		pbs; /*!< Peak Burst Size */
	uint32_t		meterid; /*!< Meter ID Configured on the system*/
	uint32_t		flags; /*!< Flags define operations on meters enbled*/
}QOS_MGR_METER_CFG;
#endif /* __QOS_MGR_COMMON_H_*/
