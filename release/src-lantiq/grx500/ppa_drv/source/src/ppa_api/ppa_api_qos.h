#ifndef __PPA_API_MFE_H__20100427_1703
#define __PPA_API_MFE_H__20100427_1703
/*******************************************************************************
**
** FILE NAME    : ppa_api_qos.h
** PROJECT      : PPA
** MODULES      : PPA API (PPA QOS  APIs)
**
** DATE         : 27 April 2010
** AUTHOR       : Shao Guohua
** DESCRIPTION  : PPA QOS APIs
**                File
** COPYRIGHT    : Copyright (c) 2017 Intel Corporation
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date                $Author         $Comment
** 16 Dec 2009  Shao Guohua             Initiate Version
*******************************************************************************/

/*! \file ppa_api_qos.h
\brief This file contains es.
provide PPA power management API.
*/

/** \addtogroup  PPA_API_QOS */
/*@{*/

#ifdef CONFIG_PPA_QOS
/*
 * ####################################
 *              Definition
 * ####################################
 */
#define PPA_QOS_QUEUE_EXISTS 1
#define PPA_QOS_QUEUE_NOT_FOUND 0
#define PPA_QOS_SHAPER_EXISTS 1
#define PPA_QOS_SHAPER_NOT_FOUND 0
#define MAX_QOS_CAPS 2

int32_t ppa_api_qos_manager_create(void);
void ppa_api_qos_manager_destroy(void);

#if defined(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500
static inline
uint32_t inggrp2flags(PPA_QOS_INGGRP inggrp)
{
	/* TODO: verify groups 1-5 (see tmu_hal_set_ingress_grp_qmap logic) */
	switch (inggrp) {
	case PPA_QOS_INGGRP0: return PPA_QOS_Q_F_INGGRP1;
	case PPA_QOS_INGGRP1: return PPA_QOS_Q_F_INGGRP2;
	case PPA_QOS_INGGRP2: return PPA_QOS_Q_F_INGGRP3;
	case PPA_QOS_INGGRP3: return PPA_QOS_Q_F_INGGRP4;
	case PPA_QOS_INGGRP4:
	case PPA_QOS_INGGRP5:
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
	PPA_ATOMIC                          count;
	char                                ifname[PPA_IF_NAME_SIZE];/*!< Ingress Interface name corresponding to a Ingress QoS group*/
	PPA_QOS_INGGRP                      ingress_group;/*!< Ingress QoS Group*/
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
#endif/*CONFIG_SOC_GRX500*/


/*!
  \brief QoS Shaper list item structure
 */
typedef struct qos_shaper_list_item {
	struct qos_shaper_list_item            	*next;
	PPA_ATOMIC                              	count;
	char                        		ifname[PPA_IF_NAME_SIZE];/*!< Interface name on which the Shaper is set*/
	char  		                        dev_name[PPA_IF_NAME_SIZE];
	int32_t                       		shaperid;/*!< Logical shaper Id*/
	PPA_QOS_SHAPER_CFG                 		shaper;/*!< Shaper Properties */
	uint32_t                     		portid;/*!< Portid*/
	uint32_t                     		flags;/*!< Flags for future use*/
	uint32_t                     		p_entry;/*!< Physical shaper Entry*/
	void                        		*caps_list;/*!< List of capabilities for the specific Shaper*/
	uint16_t                    		num_caps;/*!< Maximum number of capabilities set for specific Shaper*/

}QOS_SHAPER_LIST_ITEM;

/*!
  \brief QoS Meter list item structure
 */
typedef struct qos_meter_list_item {
	struct qos_meter_list_item             *next;
	PPA_ATOMIC                              count;
	char                                    ifname[PPA_IF_NAME_SIZE];/*!< Interface name on which the Meter is set*/
	PPA_QOS_METER_CFG                       meter;/*!< Meter Properties */
}QOS_METER_LIST_ITEM;

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
int32_t __ppa_qos_shaper_lookup(int32_t s_num, PPA_IFNAME ifname[16], QOS_SHAPER_LIST_ITEM **pp_info);
int32_t ppa_qos_init_cfg(uint32_t flags, uint32_t hal_id);
int32_t ppa_qos_uninit_cfg(uint32_t flags, uint32_t hal_id);
int32_t ppa_modify_qos_queue(char *ifname, PPA_QOS_MOD_QUEUE_CFG *q, uint32_t flags, uint32_t hal_id);
int32_t ppa_set_qos_inggrp(PPA_QOS_INGGRP_CFG *inggrp_info, uint32_t hal_id);
int32_t qosal_eng_init_cfg(void);
int32_t qosal_eng_uninit_cfg(void);
int32_t qosal_add_shaper(PPA_CMD_RATE_INFO *rate_info, QOS_SHAPER_LIST_ITEM **pp_item);
#if defined(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500
int32_t qosal_get_qos_inggrp(PPA_QOS_INGGRP inggrp, PPA_IFNAME ifnames[PPA_QOS_MAX_IF_PER_INGGRP][PPA_IF_NAME_SIZE]);
int32_t qosal_set_qos_inggrp(PPA_CMD_QOS_INGGRP_INFO *inggrp_info, QOS_INGGRP_LIST_ITEM **pp_item);
QOS_INGGRP_LIST_ITEM * ppa_qos_inggrp_alloc_item(void);
void __ppa_qos_inggrp_add_item(QOS_INGGRP_LIST_ITEM *obj);
void ppa_qos_inggrp_free_item(QOS_INGGRP_LIST_ITEM *obj);
void ppa_qos_inggrp_lock_list(void);
void ppa_qos_inggrp_unlock_list(void);
void ppa_qos_inggrp_remove_item(PPA_IFNAME ifname[16], QOS_INGGRP_LIST_ITEM **pp_info);
void ppa_qos_inggrp_free_list(void);
int32_t __ppa_qos_inggrp_lookup(const PPA_IFNAME ifname[16],
				QOS_INGGRP_LIST_ITEM **pp_info);
int32_t ppa_qos_inggrp_lookup(const PPA_IFNAME ifname[16],
			      QOS_INGGRP_LIST_ITEM **pp_info);
#endif/*CONFIG_SOC_GRX500*/

int32_t qosal_add_shaper(PPA_CMD_RATE_INFO *, QOS_SHAPER_LIST_ITEM **);
int32_t qosal_eng_init_cfg(void);
int32_t qosal_eng_uninit_cfg(void);
#if IS_ENABLED(CONFIG_SOC_GRX500)
int32_t ppa_qos_dscp_class_set(uint32_t flags, uint32_t hal_id);
int32_t ppa_qos_dscp_class_reset(uint32_t flags, uint32_t hal_id);
int32_t qosal_dscp_class_set(uint32_t flags);
#endif

QOS_SHAPER_LIST_ITEM * ppa_qos_shaper_alloc_item(void);
void ppa_qos_shaper_free_item(QOS_SHAPER_LIST_ITEM *obj);
void ppa_qos_shaper_lock_list(void);
void ppa_qos_shaper_unlock_list(void);
void __ppa_qos_shaper_add_item(QOS_SHAPER_LIST_ITEM *obj);
void ppa_qos_shaper_remove_item(int32_t s_num, PPA_IFNAME ifname [16],QOS_SHAPER_LIST_ITEM **pp_info);
void ppa_qos_shaper_free_list(void);
int32_t ppa_qos_shaper_lookup(int32_t s_num, PPA_IFNAME ifname [16],QOS_SHAPER_LIST_ITEM **pp_info);


#if defined(WMM_QOS_CONFIG) && WMM_QOS_CONFIG
int32_t ppa_qos_create_c2p_map_for_wmm(PPA_IFNAME ifname[16],uint8_t c2p[]);
//int32_t __ppa_qos_create_c2p_map_for_wmm(PPA_IFNAME ifname[16],uint8_t *class2prio);
#endif /* WMM_QOS_CONFIG */


//typedef int (*PPA_QOS_CLASS2PRIO_CB)(int32_t port_id, struct net_device *netif, uint8_t *class2prio);
typedef struct qos_queue_list_item {
        struct qos_queue_list_item                      *next;
        PPA_ATOMIC                                      count;
        char                                    ifname[PPA_IF_NAME_SIZE];/*!< Interface name on which the Queue is modified*/
        char                                    dev_name[PPA_IF_NAME_SIZE];/*!< Interface name on which the Queue is modified*/
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
        PPA_QOS_DROP_CFG                                drop; /*!< Queue Drop Properties */
        uint32_t                                flags;/*!< Flags for future use*/
        int32_t                                 p_entry;/*!< Physical index of the Queue created returned by the Hal*/
        void                                    *caps_list;/*!< List of capabilities for the specific Queue*/
        uint16_t                                num_caps;/*!< Maximum number of capabilities set for specific Queue*/

} QOS_QUEUE_LIST_ITEM;

#if defined(CONFIG_PPA_VAP_QOS_SUPPORT) && defined(CONFIG_SOC_GRX500)
typedef struct qos_logical_if_list_item {
        struct qos_logical_if_list_item *next;
        char logical_ifname[16];
        int32_t port_id;
        int32_t subif_id;
        uint8_t flowId_en;
        int32_t flowId;

} QOS_LOGICAL_IF_LIST_ITEM;
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/

extern int32_t ppa_ioctl_get_qos_status(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_get_qos_qnum(uint32_t portid, uint32_t flag);
extern int32_t ppa_ioctl_get_qos_qnum(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_get_qos_mib(uint32_t portid, uint32_t queueid, PPA_QOS_MIB *mib, uint32_t flag);
extern int32_t ppa_ioctl_get_qos_mib(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);

extern int32_t ppa_set_qos_wfq(uint32_t portid, uint32_t queueid, uint32_t weight_level, uint32_t flag);
extern int32_t ppa_get_qos_wfq(uint32_t portid, uint32_t queueid, uint32_t *weight_level, uint32_t flag);
extern int32_t ppa_reset_qos_wfq(uint32_t portid, uint32_t queueid, uint32_t flag);
extern int32_t ppa_set_ctrl_qos_wfq(uint32_t portid,  uint32_t f_enable, uint32_t flag);
extern int32_t ppa_get_ctrl_qos_wfq(uint32_t portid,  uint32_t *f_enable, uint32_t flag);

extern int32_t ppa_ioctl_add_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_modify_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_delete_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_qos_init_cfg(unsigned int cmd);
#if defined(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500
extern int32_t ppa_ioctl_add_class_rule(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_mod_class_rule(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_del_class_rule(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_get_class_rule(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_set_qos_ingress_group(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_get_qos_ingress_group(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t qosal_set_qos_meter(PPA_QOS_METER_INFO *meter_info);
int32_t ppa_ioctl_set_qos_meter(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_add_qos_meter(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
#endif

extern int32_t ppa_ioctl_mod_subif_port_config(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_add_wmm_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_delete_wmm_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_set_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_get_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_set_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_get_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
extern int32_t ppa_ioctl_reset_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);

int32_t qosal_add_qos_queue(PPA_CMD_QOS_QUEUE_INFO *, QOS_QUEUE_LIST_ITEM **);
int32_t qosal_modify_qos_queue(PPA_CMD_QOS_QUEUE_INFO *, QOS_QUEUE_LIST_ITEM **);
int32_t qosal_delete_qos_queue(PPA_CMD_QOS_QUEUE_INFO *, QOS_QUEUE_LIST_ITEM *);
int32_t qosal_set_qos_rate(PPA_CMD_RATE_INFO *, QOS_QUEUE_LIST_ITEM *, QOS_SHAPER_LIST_ITEM *);
int32_t qosal_reset_qos_rate(PPA_CMD_RATE_INFO *, QOS_QUEUE_LIST_ITEM *, QOS_SHAPER_LIST_ITEM *);

int32_t ppa_ioctl_get_qos_status(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_get_qos_qnum( uint32_t portid, uint32_t flag);
int32_t ppa_ioctl_get_qos_qnum(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_get_qos_mib( uint32_t portid, uint32_t queueid, PPA_QOS_MIB *mib, uint32_t flag);
int32_t ppa_ioctl_get_qos_mib(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);

int32_t ppa_set_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t weight_level, uint32_t flag );
int32_t ppa_get_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t *weight_level, uint32_t flag);
int32_t ppa_reset_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t flag);
int32_t ppa_set_ctrl_qos_wfq(uint32_t portid,  uint32_t f_enable, uint32_t flag);
int32_t ppa_get_ctrl_qos_wfq(uint32_t portid,  uint32_t *f_enable, uint32_t flag);

int32_t ppa_ioctl_add_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_modify_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_delete_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_qos_init_cfg(unsigned int cmd);
#if defined(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500
int32_t ppa_ioctl_add_class_rule(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_mod_class_rule(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_del_class_rule(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_class_rule(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_set_qos_ingress_group(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t ppa_ioctl_get_qos_ingress_group(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t ppa_ioctl_set_qos_meter(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
#endif/*CONFIG_SOC_GRX500*/

int32_t ppa_ioctl_mod_subif_port_config(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_add_wmm_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_delete_wmm_qos_queue(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_set_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_set_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_reset_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);

int32_t ppa_set_ctrl_qos_rate(uint32_t portid,  uint32_t f_enable, uint32_t flag);
int32_t ppa_get_ctrl_qos_rate(uint32_t portid,  uint32_t *f_enable, uint32_t flag);
int32_t ppa_set_qos_rate( char *ifname, uint32_t portid, uint32_t queueid, int32_t shaperid, uint32_t rate, uint32_t burst, uint32_t flag, int32_t hal_id );
int32_t ppa_get_qos_rate( uint32_t portid, uint32_t queueid, int32_t* shaperid, uint32_t *rate, uint32_t *burst, uint32_t flag);
int32_t ppa_reset_qos_rate( char *ifname, char *dev_name, uint32_t portid, int32_t queueid, int32_t shaperid, uint32_t flag, int32_t hal_id);
int32_t ppa_set_qos_shaper( int32_t shaperid, uint32_t rate, uint32_t burst,PPA_QOS_ADD_SHAPER_CFG *, uint32_t flags, int32_t hal_id );
int32_t ppa_get_qos_shaper( int32_t shaperid, uint32_t *rate, uint32_t *burst, uint32_t flag);
int32_t ppa_ioctl_set_qos_shaper(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_qos_shaper(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_set_ctrl_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_ctrl_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_set_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_reset_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);

#if defined(CONFIG_PPA_VAP_QOS_SUPPORT) && defined(CONFIG_SOC_GRX500)
extern QOS_LOGICAL_IF_LIST_ITEM *g_qos_logical_if;
extern PPA_LOCK g_qos_logical_if_lock;

#ifdef CONFIG_PPA_QOS_8_VAP_SUPPORT
#define MAX_VAP_QOS_SUPPORT 8
#else
#define MAX_VAP_QOS_SUPPORT 4
#endif/*CONFIG_PPA_QOS_8_VAP_SUPPORT*/
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/

extern QOS_QUEUE_LIST_ITEM *g_qos_queue;
extern PPA_LOCK g_qos_queue_lock;




QOS_QUEUE_LIST_ITEM *ppa_qos_queue_alloc_item(void);
void ppa_qos_queue_free_item(QOS_QUEUE_LIST_ITEM *obj);
void ppa_qos_queue_lock_list(void);
void ppa_qos_queue_unlock_list(void);
void __ppa_qos_queue_add_item(QOS_QUEUE_LIST_ITEM *obj);
void ppa_qos_queue_remove_item(int32_t q_num, PPA_IFNAME ifname[16], QOS_QUEUE_LIST_ITEM **pp_info);
void ppa_qos_queue_free_list(void);
int32_t ppa_qos_queue_lookup(int32_t q_num, PPA_IFNAME ifname[16], QOS_QUEUE_LIST_ITEM **pp_info);

#if defined(CONFIG_PPA_VAP_QOS_SUPPORT) && defined(CONFIG_SOC_GRX500)
int32_t get_avl_flow_id(int32_t port_id);
int32_t get_avl_queues_via_ifname(char *ifname);
QOS_LOGICAL_IF_LIST_ITEM *ppa_qos_logical_if_alloc_item(void);
void ppa_qos_logical_if_free_item(QOS_LOGICAL_IF_LIST_ITEM *obj);
void ppa_qos_logical_if_lock_list(void);
void ppa_qos_logical_if_unlock_list(void);
void __ppa_qos_logical_if_add_item(QOS_LOGICAL_IF_LIST_ITEM *obj);
void ppa_qos_logical_if_remove_item(PPA_IFNAME ifname[16], QOS_LOGICAL_IF_LIST_ITEM **pp_info);
void ppa_qos_logical_if_free_list(void);
int32_t ppa_qos_logical_if_lookup(int32_t subifId, PPA_IFNAME ifname[16], QOS_LOGICAL_IF_LIST_ITEM **pp_info);
int32_t ppa_set_logical_if_qos_classifier(PPA_CLASS_RULE *rule);
#endif/*CONFIG_PPA_VAP_QOS_SUPPORT*/

int32_t qos_hal_wmm_add(struct dp_event_info *info);
int32_t qos_hal_wmm_del(struct dp_event_info *info);

#endif //END OF CONFIG_PPA_QOS
#endif  //end of __PPA_API_MFE_H__20100427_1703
