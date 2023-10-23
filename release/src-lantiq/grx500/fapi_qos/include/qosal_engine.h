/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __QOS_AL_ENGINE_IF_H
#define __QOS_AL_ENGINE_IF_H
#include <qosal_queue_api.h>
#include <qosal_cl_api.h>

#define MAX_FLOW_TYPES 10

#define Q_CAP_RED		(1<<0)
#define Q_CAP_WRED		(1<<1)
#define Q_CAP_DT		(1<<2)
#define Q_CAP_CODEL		(1<<3)
#define Q_CAP_SP		(1<<4)
#define Q_CAP_WFQ		(1<<5)
#define Q_CAP_WRR		(1<<6)
#define Q_CAP_LEN		(1<<7)
#define Q_CAP_SUBQ		(1<<8)
#define Q_CAP_SHAPE		(1<<9)
#define Q_CAP_COL_BLIND		(1<<10)
#define Q_CAP_TR_TCM		(1<<11)
#define Q_CAP_TR_TCM_RFC4115	(1<<12)
#define Q_CAP_LOOSE_COL_BLIND	(1<<13)
#define PORT_CAP_SHAPE		(1<<14)
#define Q_CAP_ATM_IF		(1<<15)
#define Q_CAP_PTM_IF		(1<<16)
#define Q_CAP_ETHL_IF		(1<<17)
#define Q_CAP_ETHW_IF		(1<<18)
#define Q_CAP_LTE_IF		(1<<19)
#define Q_CAP_INGRESS		(1<<20)
#define Q_CAP_WMM		(1<<21)
#define Q_CAP_DOS		(1<<22)

//Flags can be put for all parameters provided in 181 data model
#define CL_CAP_MAC_ADDR		(1<<0)
#define CL_CAP_TX_IF		(1<<1)
#define CL_CAP_ETH_VLAN_PRIO	(1<<2)
#define CL_CAP_ETH_VLAN_ID	(1<<3)
#define CL_CAP_ETH_VLAN_DEI	(1<<4)
#define CL_CAP_ETH_INNER_VLAN	(1<<5)
#define CL_CAP_L3_IPv4_ADDR	(1<<6)
#define CL_CAP_L3_IPv6_ADDR	(1<<7)
#define CL_CAP_L3_PROTO		(1<<8)
#define CL_CAP_L3_INNER_IP	(1<<9)
#define CL_CAP_L3_PKTLEN	(1<<10)
#define CL_CAP_L3_DSCP		(1<<11)
#define CL_CAP_L4_PROTO_N_PORT	(1<<12)
#define CL_CAP_L4_TCP_FL	(1<<13)
#define CL_CAP_L7_PROTO		(1<<14)
#define CL_CAP_DHCP_VEND_EXT	(1<<15)
#define CL_CAP_EXCL		(1<<16) /* engine either supports exclude option for all or no params. */
#define CL_CAP_L2IF		(1<<17)
#define CL_CAP_ACT_TC		(1<<18)
#define CL_CAP_ACT_DROP		(1<<19)
#define CL_CAP_ACT_DSCP_REMARK	(1<<20)
#define CL_CAP_ACT_VLAN_REMARK	(1<<21)
#define CL_CAP_ACT_FWD		(1<<22)
#define CL_CAP_ACT_POLICE	(1<<23)
#define CL_CAP_ACT_POLICY_RT	(1<<24)
#define CL_CAP_ACT_APP		(1<<25)
#define CL_CAP_INS		(1<<26)
#define CL_CAP_ETHL_IF		(1<<27)
#define CL_CAP_ATM_IF		(1<<28)
#define CL_CAP_PTM_IF		(1<<29)
#define CL_CAP_ETHW_IF		(1<<30)
#define CL_CAP_LTE_IF		(1<<31)

#define CL_CAP_ACT_ACCL		(1<<2)
#define CL_CAP_MPTCP		(1<<3)
#define CL_CAP_ETH_TYPE     (1<<4) /*Added for Ethertype*/
#define CL_CAP_ACT_INGRS_SW	(1<<5) /*Added for Ingress iptables classification*/
#define CL_CAP_ACT_INGRS_HW	(1<<6) /*Added for Ingress Harware Classification*/
#define CL_CAP_PPP_PROTO	(1<<7) /* Added for PPP Protocol Classification */

#define SET_PARAM(param, value) (value == 1)?param:0

/* Internal flags for classifier callbacks */
#define CL_FILTER_ORDER_UPDATE 0x80000000

#define DEFAULT_TC		8

#define CL_ADD 			1
#define CL_DEL 			2
#define CL_MOD 			4

#define CPU_CALLER 		(1 << 5)
#define PPA_CALLER 		(1 << 6)

#define DEF_VLAN_PRIO 		-1

typedef struct {
        char sIf[MAX_IF_NAME_LEN];
        int32_t defIfVlanPrio;
        int32_t nVlanPrioMap[DEFAULT_TC];
        int32_t nVlanPrioPPAMap[MAX_TC_NUM];
} vlanMap;

typedef enum qos_dir {
	QOS_DIR_NONE,
	QOS_DIR_US,
	QOS_DIR_DS,
	QOS_DIR_BOTH
} qos_dir_t;

typedef enum qos_resource {
	QOS_RESOURCE_CPU,
	QOS_RESOURCE_FW,
	QOS_RESOURCE_HW,
	QOS_RESOURCE_PPA,
	QOS_RESOURCE_MAX
} qos_resource_t;

typedef enum qos_module_type {
	QOS_MODULE_CLASSIFIER,
	QOS_MODULE_QUEUE,
	QOS_MODULE_BOTH,
	QOS_MODULE_WMM,
	QOS_MODULE_DOS
} qos_module_type_t;

typedef struct cl_cap {
	uint32_t cap; /* Bitmap to indicate various classifying capabilities supported by the engine */
	uint32_t cap_exclude; /* Bitmap to indicate various classifying capabilities exclude supported by the engine */
	uint32_t ext_cap; /* Extra space if capabilities list increase to more than 32 */
	uint32_t ext_cap_exclude; /* Bitmap to indicate various classifying capabilities exclude supported by the engine */
} cl_cap_t;

typedef uint32_t q_cap_t;

/* Engine uses this structure to indicate its capabilities to Engine Selector */
typedef struct engine_capab {
	uint32_t q_max; /* Max number of queues supported by the engine */
	q_cap_t q_cap; /* Bitmap to indicate various queueing capabilities supported by the engine */
	uint32_t q_len_max; /* Max queue length in Bytes supported by the engine */
	uint32_t cl_max; /* Max number of classifiers supported by the engine */
	cl_cap_t cl_cap; /* Bitmap to indicate various classifying capabilities supported by the engine */
} engine_capab_t;

/* Engine uses this structure to indicate its capabilities and callback functions to Engine Selector */
typedef struct qos_engine {
	uint32_t engine_id; /* to be alloted by engine selector, es can create and pass this id to engine on first invoke, Id can be allotted from enum engines_t */
	int32_t (*init) (qos_module_type_t); /* Callback function to initialize the engine*/
	int32_t (*fini) (qos_module_type_t); /* Callback function to un-initialize the engine*/
	int32_t (*queue_add) (char *, iftype_t, ifinggrp_t, qos_queue_cfg_t *); /* Callback function to add a queue for the given interface in the engine*/
	int32_t (*queue_modify) (char *, iftype_t, ifinggrp_t, qos_queue_cfg_t *); /* Callback function to modify a queue for the given interface in the engine*/
	int32_t (*queue_delete) (char *, iftype_t, ifinggrp_t, qos_queue_cfg_t *); /* Callback function to delete a queue for the given interface in the engine*/
	int32_t (*queue_stats_get) (char *, iftype_t, ifinggrp_t, char*, qos_queue_stats_t *); /* Callback function to get a queue statistics for the given interface in the engine*/
	int32_t (*port_set) (char *, iftype_t, ifinggrp_t, qos_shaper_t *, uint32_t, uint32_t, uint32_t); /* Callback function to set port config for the given interface in the engine, iftype can be used to get port rate to validate shaper pir */
	//int32_t (*class_add) (qos_class_cfg_t *, uint32_t); /* Callback function to add a classifier for the given interface in the engine*/
	//int32_t (*class_modify) (qos_class_cfg_t *, uint32_t); /* Callback function to modify a classifier for the given interface in the engine*/
	//int32_t (*class_delete) (qos_class_cfg_t *, uint32_t); /* Callback function to delete a classifier for the given interface in the engine*/
	int32_t (*class_add) (iftype_t, ifinggrp_t, uint32_t, qos_class_cfg_t *, uint32_t); /* Callback function to add a classifier for the given interface in the engine*/
	int32_t (*class_modify) (iftype_t, ifinggrp_t, uint32_t, qos_class_cfg_t *, uint32_t, qos_class_cfg_t *, uint32_t); /* Callback function to modify a classifier for the given interface in the engine*/
	int32_t (*class_delete) (iftype_t, ifinggrp_t, uint32_t, qos_class_cfg_t *, uint32_t); /* Callback function to delete a classifier for the given interface in the engine*/
	int32_t (*ifabs_set) (char *, iftype_t, int32_t); /* Callback function to handle interface abstraction*/
	//Callback for port shape
	engine_capab_t engine_capab; /* engine's capabilities */
	qos_resource_t used_resource; /* Used resource to achieve the functionality. //each engine shall provide its capabilities and resources needed for that, so that engine selector can use this as well as some other info like number of sessions handeled by a particular resource at present. eg switch can do queueing in hardware and sw engine can do the same using CPU. */
} qos_engine_t;


	//uint32_t register_engine(Engine*); NOT needed

	//uint32_t register_engine_<engine_name>(qos_engine_t); This API has to be provided by each engine and fed to Engine Selector statically, ES can use this API to invoke engine so that engine can register itself with ES
#define MAX_IF_RATE		1000000
extern int32_t sysapi_iface_rate_get(char *ifname, iftype_t, uint32_t *rate);
extern int32_t sysapi_set_ppefp(char *ifname, iftype_t iftype,  uint32_t enable);
extern int32_t sysapi_set_pkts_redirection(char *from, char *to, uint32_t enable);
extern int32_t sysapi_no_of_lan_ports(void);
extern int32_t
qosd_queue_get(char *ifname,       
                char* queue_name,
                uint32_t *num_queues,
                qos_queue_cfg_t **q,
                uint32_t flags);

/* Below are the cpu_n engine implemented API's those can be used other engines as well */
uint32_t isQueueDefault(qos_queue_cfg_t* q);
int32_t ingress_ipt_config(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags);
int32_t tc_q_map_add(char *ifname, iftype_t iftype, qos_queue_cfg_t *q);
int32_t tc_q_map_delete(char *ifname, iftype_t iftype, qos_queue_cfg_t *q);
int32_t tc_q_map_init(void);
int32_t tc_q_map_fini(void);
int32_t def_dscp_map_init(void);
int32_t def_dscp_map_fini(void);
uint32_t is_wan_interface(iftype_t iftype);

#endif /* __QOS_AL_ENGINE_IF_H */
