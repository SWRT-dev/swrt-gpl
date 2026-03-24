#ifndef QOS_MGR_API_H_20200123_1910
#define QOS_MGR_API_H_20200123_1910
/******************************************************************************
 **
 ** FILE NAME	: qos_mgr_api.h
 ** PROJECT	: UGW
 ** MODULES	: QoS Manager API
 **
 ** DATE	: 08 Jan 2020
 ** AUTHOR	: Mohammed Aarif
 ** DESCRIPTION	: QoS Manager API Implementation
 ** COPYRIGHT  : Copyright (c) 2019 - 2020 Intel Corporation
 **
 ** HISTORY
 ** $Date		$Author			$Comment
 ** 08 JAN 2020		Mohammed Aarif		Initiate Version
 *******************************************************************************/

/*! \file qos_mgr_api.h
\brief This file contains QoS Manager API
*/

//#include "qos_mgr_hal_api.h"
#include "qos_mgr_stack_al.h"
#include <uapi/net/qos_mgr_common.h>

/*
 * ####################################
 *              Definition
 * ####################################
 */
#define QOS_MGR_QUEUE_EXISTS 1
#define QOS_MGR_QUEUE_NOT_FOUND 0
#define QOS_MGR_SHAPER_EXISTS 1
#define QOS_MGR_SHAPER_NOT_FOUND 0
#define MAX_QOS_CAPS 2
#define QOS_MGR_IFNAME_SIZE 16



#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
#define DP_EVENT_CB 1
#else
#define DP_EVENT_CB 0
#endif

int32_t qos_mgr_api_create(void);
void qos_mgr_api_destroy(void);

#define QOS_MGR_MAX_IF_PER_INGGRP 20
#define QOS_MGR_NUM_INGRESS_GROUPS		QOS_MGR_INGGRP_MAX
#define QOS_MGR_INGGRP_INVALID(g)		((g < QOS_MGR_INGGRP0) || (g >= QOS_MGR_INGGRP_MAX))
#define QOS_MGR_INGGRP_VALID(g)		(!QOS_MGR_INGGRP_INVALID(g))
static inline
uint32_t inggrp2flags(QOS_MGR_INGGRP inggrp)
{
	/* TODO: verify groups 1-5 (see tmu_hal_set_ingress_grp_qmap logic) */
	switch (inggrp) {
	case QOS_MGR_INGGRP0: return QOS_MGR_Q_F_INGGRP1;
	case QOS_MGR_INGGRP1: return QOS_MGR_Q_F_INGGRP2;
	case QOS_MGR_INGGRP2: return QOS_MGR_Q_F_INGGRP3;
	case QOS_MGR_INGGRP3: return QOS_MGR_Q_F_INGGRP4;
	case QOS_MGR_INGGRP4:
	case QOS_MGR_INGGRP5:
	default: /* not supported in MPE FW (4 groups only) */
			      pr_err("%s:%d: INGGRP%d not supported!!!\n", __func__, __LINE__, inggrp);
	}

	return 0; /* ERROR */
}

/*!
  \brief QoS Queue list item structure
 */
/*!
  \brief QoS Ingress Group list item structure
 */
typedef struct qos_ingggrp_list_item {
	struct qos_inggrp_list_item         *next;
	QOS_MGR_ATOMIC                          count;
	char                                ifname[QOS_MGR_IFNAME_SIZE];/*!< Ingress Interface name corresponding to a Ingress QoS group*/
	QOS_MGR_INGGRP                      ingress_group;/*!< Ingress QoS Group*/
	uint32_t                            flowId;/*!< FlowId value for a particular ingress group*/
	uint32_t                            flowId_en;/*!< FlowId enable/disable*/
	uint32_t                            enc;/*!< Encrytption bit used to select particular ingress group*/
	uint32_t                            dec;/*!< Decrytption bit used to select particular ingress group*/
	uint32_t                            mpe1;/*!< MPE1 bit used to select particular ingress group*/
	uint32_t                            mpe2;/*!< MPe2 bit used to select particular ingress group*/
	uint32_t                            tc;/*!< Traffic class used to select queue in a particular ingress group*/
	uint32_t                            ep;/*!< Egress port corresponding to a particular ingress flow*/
	uint32_t                            flags;/*!< Flags for Future use*/

}QOS_INGGRP_LIST_ITEM;


/*!
  \brief QoS Shaper list item structure
 */
typedef struct qos_shaper_list_item {
	struct qos_shaper_list_item            	*next;
	QOS_MGR_ATOMIC                              	count;
	char                        		ifname[QOS_MGR_IFNAME_SIZE];/*!< Interface name on which the Shaper is set*/
	char  		                        dev_name[QOS_MGR_IFNAME_SIZE];
	int32_t                       		shaperid;/*!< Logical shaper Id*/
	QOS_MGR_SHAPER_CFG                 		shaper;/*!< Shaper Properties */
	uint32_t                     		portid;/*!< Portid*/
	uint32_t                     		flags;/*!< Flags for future use*/
	uint32_t                     		p_entry;/*!< Physical shaper Entry*/
	void                        		*caps_list;/*!< List of capabilities for the specific Shaper*/
	uint16_t                    		num_caps;/*!< Maximum number of capabilities set for specific Shaper*/

}QOS_SHAPER_LIST_ITEM;

/*!
	\brief QoS Mgr Modify Queue Configuration structure
 */
typedef struct qos_mgr_mod_queue_cfg {
	uint32_t		enable; /*!< Whether Queue is enabled */
	int32_t			weight; /*!< WFQ Weight */
	int32_t			priority; /*!< Queue Priority or Precedence. Start from 1-16, with 1 as highest priority */
	uint32_t		portid;/*!< PORT ID */
	QOS_MGR_QSCHED_MODE 	q_type; /*!< QoS scheduler mode - Priority, WFQ */
	int32_t			qlen; /*!< Length of Queue in bytes */
	uint32_t		queue_id; /*!< Physical Queue id of Queue being modified*/
	uint32_t		flags; /*!< Flags for future use*/
	QOS_MGR_DROP_CFG	drop; /*!< Queue Drop Properties */
} QOS_MGR_MOD_QUEUE_CFG;

/*!
	\brief QoS Mgr Ingress Group structure
 */
typedef struct qos_mgr_inggrp_cfg {
	QOS_MGR_IFNAME 	ifname[QOS_MGR_IF_NAME_SIZE];/*!< Ingress Interface name corresponding to a Ingress QoS group*/
	QOS_MGR_INGGRP 	ingress_group;/*!< Ingress QoS Group*/
	uint16_t 	flowId;/*!< FlowId value for a particular ingress group*/
	uint8_t 	flowId_en;/*!< FlowId enable/disable*/
	uint8_t 	enc;/*!< Encrytption bit used to select particular ingress group*/
	uint8_t 	dec;/*!< Decrytption bit used to select particular ingress group*/
	uint8_t 	mpe1;/*!< MPE1 bit used to select particular ingress group*/
	uint8_t 	mpe2;/*!< MPe2 bit used to select particular ingress group*/
	uint16_t 	tc;/*!< Traffic class used to select queue in a particular ingress group*/
	uint32_t 	ep;/*!< Egress port corresponding to a particular ingress flow*/
	uint32_t 	flags;/*!< Flags for Future use*/
} QOS_MGR_INGGRP_CFG;

/*!
	\brief QoS Mgr Add Shaper Configuration structure
 */
typedef struct qos_mgr_add_shaper_cfg {
	QOS_MGR_SHAPER_MODE 	mode; /*!< Mode of Token Bucket shaper */
	uint32_t		enable; /*!< Enable for Shaper */
	uint32_t 		cir; /*!< Committed Information Rate in bytes/s */
	uint32_t 		cbs; /*!< Committed Burst Size in bytes */
	uint32_t 		pir; /*!< Peak Information Rate in bytes/s */
	uint32_t 		pbs; /*!< Peak Burst Size */
	uint32_t 		flags; /*!< Flags define which shapers are enabled
						- QOS_SHAPER_F_PIR
						- QOS_SHAPER_F_CIR */
	uint32_t		phys_shaperid;/*!< Physical Queue id of Queue that is added*/
} QOS_MGR_ADD_SHAPER_CFG;
/*
* ####################################
*              Data Type
* ####################################
*/


/*
 * ####################################
 *             Declaration
 * ####################################
 */
int32_t __qos_mgr_shaper_lookup(int32_t s_num, QOS_MGR_IFNAME ifname[QOS_MGR_IFNAME_SIZE], QOS_SHAPER_LIST_ITEM **pp_info);
int32_t qos_mgr_init_cfg(uint32_t flags, uint32_t hal_id);
int32_t qos_mgr_uninit_cfg(uint32_t flags, uint32_t hal_id);
int32_t qos_mgr_modify_qos_queue(char *ifname, QOS_MGR_MOD_QUEUE_CFG *q, uint32_t flags, uint32_t hal_id);
int32_t qos_mgr_set_qos_inggrp(QOS_MGR_INGGRP_CFG *inggrp_info, uint32_t hal_id);
int32_t qosal_eng_init_cfg(void);
int32_t qosal_eng_uninit_cfg(void);
int32_t qosal_add_shaper(QOS_MGR_CMD_RATE_INFO *rate_info, QOS_SHAPER_LIST_ITEM **pp_item);
int32_t qosal_get_qos_inggrp(QOS_MGR_INGGRP inggrp, QOS_MGR_IFNAME ifnames[QOS_MGR_MAX_IF_PER_INGGRP][QOS_MGR_IF_NAME_SIZE]);
int32_t qosal_set_qos_inggrp(QOS_MGR_CMD_INGGRP_INFO *inggrp_info, QOS_INGGRP_LIST_ITEM **pp_item);
QOS_INGGRP_LIST_ITEM * qos_mgr_inggrp_alloc_item(void);
void __qos_mgr_inggrp_add_item(QOS_INGGRP_LIST_ITEM *obj);
void qos_mgr_inggrp_free_item(QOS_INGGRP_LIST_ITEM *obj);
void qos_mgr_inggrp_lock_list(void);
void qos_mgr_inggrp_unlock_list(void);
void qos_mgr_inggrp_remove_item(QOS_MGR_IFNAME ifname[QOS_MGR_IFNAME_SIZE], QOS_INGGRP_LIST_ITEM **pp_info);
void qos_mgr_inggrp_free_list(void);
int32_t __qos_mgr_inggrp_lookup(const QOS_MGR_IFNAME ifname[QOS_MGR_IFNAME_SIZE],
				QOS_INGGRP_LIST_ITEM **pp_info);
int32_t qos_mgr_inggrp_lookup(const QOS_MGR_IFNAME ifname[QOS_MGR_IFNAME_SIZE],
			      QOS_INGGRP_LIST_ITEM **pp_info);

int32_t qosal_add_shaper(QOS_MGR_CMD_RATE_INFO *, QOS_SHAPER_LIST_ITEM **);
int32_t qosal_eng_init_cfg(void);
int32_t qosal_eng_uninit_cfg(void);

QOS_SHAPER_LIST_ITEM * qos_mgr_shaper_alloc_item(void);
void qos_mgr_shaper_free_item(QOS_SHAPER_LIST_ITEM *obj);
void qos_mgr_shaper_lock_list(void);
void qos_mgr_shaper_unlock_list(void);
void __qos_mgr_shaper_add_item(QOS_SHAPER_LIST_ITEM *obj);
void qos_mgr_shaper_remove_item(int32_t s_num, QOS_MGR_IFNAME ifname [QOS_MGR_IFNAME_SIZE],QOS_SHAPER_LIST_ITEM **pp_info);
void qos_mgr_shaper_free_list(void);
int32_t qos_mgr_shaper_lookup(int32_t s_num, QOS_MGR_IFNAME ifname [QOS_MGR_IFNAME_SIZE],QOS_SHAPER_LIST_ITEM **pp_info);


//typedef int (*QOS_MGR_CLASS2PRIO_CB)(int32_t port_id, struct net_device *netif, uint8_t *class2prio);
typedef struct qos_queue_list_item {
        struct qos_queue_list_item                      *next;
        QOS_MGR_ATOMIC                                      count;
        char                                    ifname[QOS_MGR_IFNAME_SIZE];/*!< Interface name on which the Queue is modified*/
        char                                    dev_name[QOS_MGR_IFNAME_SIZE];/*!< Interface name on which the Queue is modified*/
        char                                    tc_map[MAX_TC_NUM];/*!<Which all Traffic Class(es) map to this Queue*/
        uint8_t                                 tc_no;/*!<Nunber of Traffic Class(es) map to this Queue*/
        uint8_t                                 intfId_en;/*!<Enable/Disable for flow Id + tc bits used for VAP's & Vlan interfaces*/
        uint16_t                                        intfId;/*!< flow Id + tc bits used for VAP's & Vlan interfaces*/
        uint32_t                                enable; /*!< Whether Queue is enabled */
        int32_t                                 weight; /*!< WFQ Weight */
        int32_t                                 priority; /*!< Queue Priority or Precedence. Start from 1-16, with 1 as highest priority */
        int32_t                                 qlen; /*!< Length of Queue in bytes */
        int32_t                                 queue_num; /*!< Logical queue number added will be filled up in this field */
        int32_t                                 shaper_num; /*!< Logical shaper number added will be filled up in this field */
        uint32_t                                portid;/*!< Portid*/
        QOS_MGR_DROP_CFG                                drop; /*!< Queue Drop Properties */
        uint32_t                                flags;/*!< Flags for future use*/
        int32_t                                 p_entry;/*!< Physical index of the Queue created returned by the Hal*/
        void                                    *caps_list;/*!< List of capabilities for the specific Queue*/
        uint16_t                                num_caps;/*!< Maximum number of capabilities set for specific Queue*/

} QOS_QUEUE_LIST_ITEM;


extern int32_t qos_mgr_ioctl_get_qos_status(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_get_qos_qnum(uint32_t portid, uint32_t flag);
extern int32_t qos_mgr_ioctl_get_qos_qnum(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_get_qos_mib(uint32_t portid, uint32_t queueid, QOS_MGR_MIB *mib, uint32_t flag);
extern int32_t qos_mgr_ioctl_get_qos_mib(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);

extern int32_t qos_mgr_set_qos_wfq(uint32_t portid, uint32_t queueid, uint32_t weight_level, uint32_t flag);
extern int32_t qos_mgr_get_qos_wfq(uint32_t portid, uint32_t queueid, uint32_t *weight_level, uint32_t flag);
extern int32_t qos_mgr_reset_qos_wfq(uint32_t portid, uint32_t queueid, uint32_t flag);
extern int32_t qos_mgr_set_ctrl_qos_wfq(uint32_t portid,  uint32_t f_enable, uint32_t flag);
extern int32_t qos_mgr_get_ctrl_qos_wfq(uint32_t portid,  uint32_t *f_enable, uint32_t flag);

extern int32_t qos_mgr_ioctl_add_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_modify_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_delete_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_qos_init_cfg(unsigned int cmd);
extern int32_t qos_mgr_ioctl_set_qos_ingress_group(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_get_qos_ingress_group(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);

extern int32_t qos_mgr_ioctl_mod_subif_port_config(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_add_wmm_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_delete_wmm_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_set_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_get_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_set_qos_wfq(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_get_qos_wfq(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
extern int32_t qos_mgr_ioctl_reset_qos_wfq(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);

int32_t qosal_add_qos_queue(QOS_MGR_CMD_QUEUE_INFO *, QOS_QUEUE_LIST_ITEM **);
int32_t qosal_modify_qos_queue(QOS_MGR_CMD_QUEUE_INFO *, QOS_QUEUE_LIST_ITEM **);
int32_t qosal_delete_qos_queue(QOS_MGR_CMD_QUEUE_INFO *, QOS_QUEUE_LIST_ITEM *);
int32_t qosal_set_qos_rate(QOS_MGR_CMD_RATE_INFO *, QOS_QUEUE_LIST_ITEM *, QOS_SHAPER_LIST_ITEM *);
int32_t qosal_reset_qos_rate(QOS_MGR_CMD_RATE_INFO *, QOS_QUEUE_LIST_ITEM *, QOS_SHAPER_LIST_ITEM *);

int32_t qos_mgr_ioctl_get_qos_status(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_get_qos_qnum( uint32_t portid, uint32_t flag);
int32_t qos_mgr_ioctl_get_qos_qnum(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_get_qos_mib( uint32_t portid, uint32_t queueid, QOS_MGR_MIB *mib, uint32_t flag);
int32_t qos_mgr_ioctl_get_qos_mib(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);

int32_t qos_mgr_set_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t weight_level, uint32_t flag );
int32_t qos_mgr_get_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t *weight_level, uint32_t flag);
int32_t qos_mgr_reset_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t flag);
int32_t qos_mgr_set_ctrl_qos_wfq(uint32_t portid,  uint32_t f_enable, uint32_t flag);
int32_t qos_mgr_get_ctrl_qos_wfq(uint32_t portid,  uint32_t *f_enable, uint32_t flag);

int32_t qos_mgr_ioctl_add_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_modify_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_delete_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_qos_init_cfg(unsigned int cmd);
int32_t qos_mgr_ioctl_set_qos_ingress_group(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);
int32_t qos_mgr_ioctl_get_qos_ingress_group(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA *cmd_info);

int32_t qos_mgr_ioctl_mod_subif_port_config(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_add_wmm_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_delete_wmm_qos_queue(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_set_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_get_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_set_qos_wfq(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_get_qos_wfq(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_reset_qos_wfq(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);

int32_t qos_mgr_set_ctrl_qos_rate(uint32_t portid,  uint32_t f_enable, uint32_t flag);
int32_t qos_mgr_get_ctrl_qos_rate(uint32_t portid,  uint32_t *f_enable, uint32_t flag);
int32_t qos_mgr_set_qos_rate( char *ifname, uint32_t portid, uint32_t queueid, int32_t shaperid, uint32_t rate, uint32_t burst, uint32_t flag, int32_t hal_id );
int32_t qos_mgr_get_qos_rate( uint32_t portid, uint32_t queueid, int32_t* shaperid, uint32_t *rate, uint32_t *burst, uint32_t flag);
int32_t qos_mgr_reset_qos_rate( char *ifname, char *dev_name, uint32_t portid, int32_t queueid, int32_t shaperid, uint32_t flag, int32_t hal_id);
int32_t qos_mgr_set_qos_shaper( int32_t shaperid, uint32_t rate, uint32_t burst,QOS_MGR_ADD_SHAPER_CFG *, uint32_t flags, int32_t hal_id );
int32_t qos_mgr_get_qos_shaper( int32_t shaperid, uint32_t *rate, uint32_t *burst, uint32_t flag);
int32_t qos_mgr_ioctl_set_qos_shaper(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_get_qos_shaper(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_set_ctrl_qos_rate(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_get_ctrl_qos_rate(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_set_qos_rate(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_reset_qos_rate(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_mgr_ioctl_get_qos_rate(unsigned int cmd, unsigned long arg, QOS_MGR_CMD_DATA * cmd_info);
int32_t qos_hal_wmm_add(char *ifname);
int32_t qos_hal_wmm_del(char *ifname);

extern QOS_QUEUE_LIST_ITEM *g_qos_queue;
extern QOS_MGR_LOCK g_qos_queue_lock;




QOS_QUEUE_LIST_ITEM *qos_mgr_queue_alloc_item(void);
void qos_mgr_queue_free_item(QOS_QUEUE_LIST_ITEM *obj);
void qos_mgr_queue_lock_list(void);
void qos_mgr_queue_unlock_list(void);
void __qos_mgr_queue_add_item(QOS_QUEUE_LIST_ITEM *obj);
void qos_mgr_queue_remove_item(int32_t q_num, QOS_MGR_IFNAME ifname[QOS_MGR_IFNAME_SIZE], QOS_QUEUE_LIST_ITEM **pp_info);
void qos_mgr_queue_free_list(void);
int32_t qos_mgr_queue_lookup(int32_t q_num, QOS_MGR_IFNAME ifname[QOS_MGR_IFNAME_SIZE], QOS_QUEUE_LIST_ITEM **pp_info);

#if IS_ENABLED(CONFIG_QOS_MGR_TC_SUPPORT)
int32_t qos_mgr_tc_init(void);
void qos_mgr_tc_exit(void);
#endif

typedef enum {
	QOS_MGR_GENERIC_HAL_GET_DSL_MIB = 0,   /*Get dsl mib*/
	QOS_MGR_GENERIC_HAL_CLEAR_DSL_MIB,   /*clear dsl mib*/
	QOS_MGR_GENERIC_HAL_INIT,
	QOS_MGR_GENERIC_HAL_EXIT,
	QOS_MGR_GENERIC_HAL_GET_HAL_VERSION,
	QOS_MGR_GENERIC_HAL_QOS_INIT_CFG,
	QOS_MGR_GENERIC_HAL_QOS_UNINIT_CFG,
	QOS_MGR_GENERIC_HAL_GET_QOS_QUEUE_NUM,  /*get maximum QOS queue number*/
	QOS_MGR_GENERIC_HAL_GET_QOS_MIB,  /*get maximum QOS queue number*/
	QOS_MGR_GENERIC_HAL_SET_QOS_WFQ_CTRL,  /*enable/disable WFQ*/
	QOS_MGR_GENERIC_HAL_GET_QOS_WFQ_CTRL,  /*get  WFQ status: enabeld/disabled*/
	QOS_MGR_GENERIC_HAL_SET_QOS_WFQ_CFG,  /*set WFQ cfg*/
	QOS_MGR_GENERIC_HAL_RESET_QOS_WFQ_CFG,  /*reset WFQ cfg*/
	QOS_MGR_GENERIC_HAL_GET_QOS_WFQ_CFG,  /*get WFQ cfg*/
	QOS_MGR_GENERIC_HAL_INIT_QOS_WFQ, /* init QOS Rateshapping*/
	QOS_MGR_GENERIC_HAL_SET_QOS_RATE_SHAPING_CTRL,  /*enable/disable Rate shaping*/
	QOS_MGR_GENERIC_HAL_GET_QOS_RATE_SHAPING_CTRL,  /*get  Rateshaping status: enabeld/disabled*/
	QOS_MGR_GENERIC_HAL_SET_QOS_RATE_SHAPING_CFG,  /*set rate shaping*/
	QOS_MGR_GENERIC_HAL_GET_QOS_RATE_SHAPING_CFG,  /*get rate shaping cfg*/
	QOS_MGR_GENERIC_HAL_RESET_QOS_RATE_SHAPING_CFG,  /*reset rate shaping cfg*/
	QOS_MGR_GENERIC_HAL_INIT_QOS_RATE_SHAPING, /* init QOS Rateshapping*/
	QOS_MGR_GENERIC_HAL_SET_QOS_SHAPER_CFG,  /*set shaper*/
	QOS_MGR_GENERIC_HAL_GET_QOS_SHAPER_CFG,  /*get shaper*/
	QOS_MGR_GENERIC_HAL_RESET_QOS_SHAPER_CFG, /*reset shaper*/
	QOS_MGR_GENERIC_HAL_GET_QOS_STATUS, /* get QOS tatus*/
	QOS_MGR_GENERIC_HAL_SET_VALUE, /* set value*/
	QOS_MGR_GENERIC_HAL_GET_VALUE, /* get value*/
	QOS_MGR_GENERIC_HAL_QOS_MODQUE_CFG,
	QOS_MGR_GENERIC_HAL_QOS_ADDQUE_CFG,
	QOS_MGR_GENERIC_HAL_QOS_DELQUE_CFG,
	QOS_MGR_GENERIC_HAL_MOD_SUBIF_PORT_CFG,
	/*make sure it is the last one */
	QOS_MGR_GENERIC_HAL_MAX_FLAG
} QOS_MGR_GENERIC_HOOK_CMD;
/*
 * ####################################
 *              Definition
 * ####################################
 */
int32_t qos_hal_generic_hook(QOS_MGR_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag);


int32_t qos_hal_global_wmm_enable_disable(bool oper);
void qos_hal_global_wmm_status(void);
int32_t qos_hal_spl_conn_mod(uint8_t conn_type, uint32_t queue_len);
#endif  /* end of QOS_MGR_API_H_20200123_1910 */
