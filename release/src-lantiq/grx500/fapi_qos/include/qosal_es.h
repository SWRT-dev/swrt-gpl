/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __QOS_AL_ES_H
#define __QOS_AL_ES_H
#include <commondefs.h>
#include <libubox/list.h>
#include <qosal_engine.h>
#include <qosal_cl_api.h>
#include <qosal_queue_api.h>
#define MAX_RULES 20
#ifndef HOST_ENV
#define PLATFORM_CONFIG  PLATFORM_XML"/usr/sbin/qosal_plat_cfg.xml"
#endif
#define ENG_LIB_PATH  PLATFORM_XML"/usr/lib/qosal/"
#define ENG_REG_CB_POSTFIX "eng_reg"
#define ENG_FINI_CB_POSTFIX "eng_fini"

#define MAX_ENGINES 10
#define MAX_SYS_DATAPATHS 50

#define DEF_CL_WT 100
#define DEF_Q_WT 100

#define ENGINE_INIT_PTR(_eng_info) _eng_info.engine.init
#define ENGINE_FINI_PTR(_eng_info) _eng_info.engine.fini
#define ENGINE_Q_ADD_PTR(_eng_info) _eng_info.engine.queue_add
#define ENGINE_Q_MOD_PTR(_eng_info) _eng_info.engine.queue_modify
#define ENGINE_Q_DEL_PTR(_eng_info) _eng_info.engine.queue_delete
#define ENGINE_CL_CAP(_eng_info) _eng_info.engine.engine_capab.cl_cap
#define ENGINE_CL_MAX(_eng_info) _eng_info.engine.engine_capab.cl_max
#define ENGINE_Q_CAP(_eng_info) _eng_info.engine.engine_capab.q_cap
#define ENGINE_Q_MAX(_eng_info) _eng_info.engine.engine_capab.q_max
#define ENGINE_QLEN_MAX(_eng_info) _eng_info.engine.engine_capab.q_len_max

#define CL_PROTO(_clcfg) (_clcfg->filter.outer_ip.ip_p)
#define CL_APP(_clcfg) (_clcfg->filter.app.app_id)
#define CL_L7_PROTO(_clcfg) (_clcfg->filter.l7_proto)
#define CL_DSCP(_clcfg) (_clcfg->filter.iphdr.dscp)
#define CL_PCP(_clcfg) (_clcfg->filter.ethhdr.prio)
#define CL_SPORT_ST(_clcfg) (_clcfg->filter.outer_ip.src_port.start_port)
#define CL_SPORT_END(_clcfg) (_clcfg->filter.outer_ip.src_port.end_port)
#define CL_DPORT_ST(_clcfg) (_clcfg->filter.outer_ip.dst_port.start_port)
#define CL_DPORT_END(_clcfg) (_clcfg->filter.outer_ip.dst_port.end_port)
#define CL_SIP_OUT(_clcfg) (_clcfg->filter.outer_ip.src_ip.ipaddr)
#define CL_SMASK_OUT(_clcfg) (_clcfg->filter.outer_ip.src_ip.mask)
#define CL_DIP_OUT(_clcfg) (_clcfg->filter.outer_ip.dst_ip.ipaddr)
#define CL_DMASK_OUT(_clcfg) (_clcfg->filter.outer_ip.dst_ip.mask)
#define CL_SMAC(_clcfg) (_clcfg->filter.src_mac.mac_addr)
#define CL_SMAC_MASK(_clcfg) (_clcfg->filter.src_mac.mac_addr_mask)
#define CL_DMAC(_clcfg) (_clcfg->filter.dst_mac.mac_addr)
#define CL_DMAC_MASK(_clcfg) (_clcfg->filter.dst_mac.mac_addr_mask)
#define CL_VLAN(_clcfg) (_clcfg->filter.ethhdr.vlanid)
#define CL_RXIF(_clcfg) (_clcfg->filter.rx_if)
#define CL_TXIF(_clcfg) (_clcfg->filter.tx_if)
#define CL_ACT(_clcfg) (_clcfg->action.flags)
#define CL_NAME(_clcfg) (_clcfg->filter.name)
#define CL_MPTCP(_clcfg) (_clcfg->flags & CL_ACTION_CL_MPTCP)

#define CL_SRC_DHCP_SEL(_clcfg) (_clcfg->filter.src_dhcp.param_in_ex.param_in)
#define CL_DST_DHCP_SEL(_clcfg) (_clcfg->filter.dst_dhcp.param_in_ex.param_in)
#define CL_O_SIP_SEL(_clcfg) (_clcfg->filter.outer_ip.param_in_ex.param_in & CLSCFG_PARAM_OUTER_IP_SRC_IP)
#define CL_O_DIP_SEL(_clcfg) (_clcfg->filter.outer_ip.param_in_ex.param_in & CLSCFG_PARAM_OUTER_IP_DST_IP)
#define CL_O_SPORT_SEL(_clcfg) (_clcfg->filter.outer_ip.param_in_ex.param_in & CLSCFG_PARAM_OUTER_IP_SRC_PORT)
#define CL_O_DPORT_SEL(_clcfg) (_clcfg->filter.outer_ip.param_in_ex.param_in & CLSCFG_PARAM_OUTER_IP_DST_PORT)
#define CL_O_PROTO_SEL(_clcfg) (_clcfg->filter.outer_ip.param_in_ex.param_in & CLSCFG_PARAM_OUTER_IP_PROTO)
#define CL_DMAC_SEL(_clcfg) (_clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_DST_MAC)
#define CL_SMAC_SEL(_clcfg) (_clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_SRC_MAC)
#define CL_ETH_SEL(_clcfg) (_clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_TYPE)
#define CL_VID_SEL(_clcfg) (_clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_VLANID)
#define CL_VPRIO_SEL(_clcfg) (_clcfg->filter.ethhdr.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR_PRIO)
#define CL_IPHDR_SEL(_clcfg) (_clcfg->filter.iphdr.param_in_ex.param_in)
#define CL_DSCP_SEL(_clcfg) (_clcfg->filter.iphdr.param_in_ex.param_in & CLSCFG_PARAM_IP_HDR_DSCP)
#define CL_TCPHDR_SEL(_clcfg) (_clcfg->filter.tcphdr.param_in_ex.param_in)
#define CL_APP_SEL(_clcfg) (_clcfg->filter.app.param_in_ex.param_in & CLSCFG_PARAM_APP_ID)
#define CL_L7_PROTO_SEL(_clcfg) (_clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_L7_PROTO)
#define CL_RXIF_SEL(_clcfg) (_clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_RX_IF)
#define CL_TXIF_SEL(_clcfg) (_clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_TX_IF)
#define CL_ETHHDR_SEL(_clcfg) (_clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_ETH_HDR)
#define CL_PPP_PROTO_SEL(_clcfg) (_clcfg->filter.param_in_ex.param_in & CLSCFG_PARAM_PPP_PROTO)

#define CL_SRC_DHCP_EX(_clcfg) (_clcfg->filter.src_dhcp.param_in_ex.param_ex)
#define CL_DST_DHCP_EX(_clcfg) (_clcfg->filter.dst_dhcp.param_in_ex.param_ex)
#define CL_O_SIP_EX(_clcfg) (_clcfg->filter.outer_ip.param_in_ex.param_ex & CLSCFG_PARAM_OUTER_IP_SRC_IP)
#define CL_O_DIP_EX(_clcfg) (_clcfg->filter.outer_ip.param_in_ex.param_ex & CLSCFG_PARAM_OUTER_IP_DST_IP)
#define CL_O_SPORT_EX(_clcfg) (_clcfg->filter.outer_ip.param_in_ex.param_ex & CLSCFG_PARAM_OUTER_IP_SRC_PORT)
#define CL_O_DPORT_EX(_clcfg) (_clcfg->filter.outer_ip.param_in_ex.param_ex & CLSCFG_PARAM_OUTER_IP_DST_PORT)
#define CL_O_PROTO_EX(_clcfg) (_clcfg->filter.outer_ip.param_in_ex.param_ex & CLSCFG_PARAM_OUTER_IP_PROTO)
#define CL_DMAC_EX(_clcfg) (_clcfg->filter.param_in_ex.param_ex & CLSCFG_PARAM_DST_MAC)
#define CL_SMAC_EX(_clcfg) (_clcfg->filter.param_in_ex.param_ex & CLSCFG_PARAM_SRC_MAC)
#define CL_ETH_EX(_clcfg) (_clcfg->filter.ethhdr.param_in_ex.param_ex & CLSCFG_PARAM_ETH_HDR_TYPE)
#define CL_VID_EX(_clcfg) (_clcfg->filter.ethhdr.param_in_ex.param_ex & CLSCFG_PARAM_ETH_HDR_VLANID)
#define CL_VPRIO_EX(_clcfg) (_clcfg->filter.ethhdr.param_in_ex.param_ex & CLSCFG_PARAM_ETH_HDR_PRIO)
#define CL_IPHDR_EX(_clcfg) (_clcfg->filter.iphdr.param_in_ex.param_ex)
#define CL_DSCP_EX(_clcfg) (_clcfg->filter.iphdr.param_in_ex.param_ex & CLSCFG_PARAM_IP_HDR_DSCP)
#define CL_TCPHDR_EX(_clcfg) (_clcfg->filter.tcphdr.param_in_ex.param_ex)
#define CL_APP_EX(_clcfg) (_clcfg->filter.app.param_in_ex.param_ex & CLSCFG_PARAM_APP_ID)
#define CL_L7_PROTO_EX(_clcfg) (_clcfg->filter.param_in_ex.param_ex & CLSCFG_PARAM_L7_PROTO)
#define CL_RXIF_EX(_clcfg) (_clcfg->filter.param_in_ex.param_ex & CLSCFG_PARAM_RX_IF)
#define CL_TXIF_EX(_clcfg) (_clcfg->filter.param_in_ex.param_ex & CLSCFG_PARAM_TX_IF)
#define CL_PPP_PROTO_EX(_clcfg) (_clcfg->filter.param_in_ex.param_ex & CLSCFG_PARAM_PPP_PROTO)

#define CL_OVERALL_EX(_clcfg) { \
		CL_SRC_DHCP_EX(_clcfg) | CL_DST_DHCP_EX(_clcfg) | CL_O_SIP_EX(_clcfg) | \
		CL_O_DIP_EX(_clcfg) | CL_O_SPORT_EX(_clcfg) | CL_O_DPORT_EX(_clcfg) | \
		CL_O_PROTO_EX(_clcfg) | CL_DMAC_EX(_clcfg) | CL_SMAC_EX(_clcfg) | \
		CL_ETH_EX(_clcfg) | CL_IPHDR_EX(_clcfg) | CL_TCPHDR_EX(_clcfg) | \
		CL_APP_EX(_clcfg) \
	}

#define QOSD_ENGINE_PORT_SET(engine_id, ifname, iftype, ifinggrp, shaper, priority, weight, flags) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for port configuration is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.port_set) \
		ret = es_tables.engine_info[engine_id].engine.port_set(ifname, iftype, ifinggrp, shaper, priority, weight, flags); \
	else \
		LOGF_LOG_ERROR("port_set callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("Engine callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_QUEUE_ADD(engine_id, ifname, iftype, ifinggrp, q) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for queue add operation is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.queue_add) \
		ret = es_tables.engine_info[engine_id].engine.queue_add(ifname, iftype, ifinggrp, q); \
	else \
		LOGF_LOG_ERROR("queue_add callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("Engine callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_QUEUE_MODIFY(engine_id, ifname, iftype, ifinggrp, q) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for queue modify operation is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.queue_modify) \
		ret = es_tables.engine_info[engine_id].engine.queue_modify(ifname, iftype, ifinggrp, q); \
	else \
		LOGF_LOG_ERROR("queue_add callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("Engine callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_QUEUE_DELETE(engine_id, ifname, iftype, ifinggrp, q) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for queue delete operation is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.queue_delete) \
		ret = es_tables.engine_info[engine_id].engine.queue_delete(ifname, iftype, ifinggrp, q); \
	else \
		LOGF_LOG_ERROR("queue_delete callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("Engine callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_QUEUE_STATS_GET(engine_id, ifname, iftype, ifinggrp, qname, qstats) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for queue add operation is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.queue_stats_get) \
		ret = es_tables.engine_info[engine_id].engine.queue_stats_get(ifname, iftype, ifinggrp, qname, qstats); \
	else \
		LOGF_LOG_ERROR("queue_stats_get callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("Engine callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_CLASS_ADD(engine_id, iftype, ifinggrp, order, cl, flags) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for classfier add operation is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.class_add) \
		ret = es_tables.engine_info[engine_id].engine.class_add(iftype, ifinggrp, order, cl, flags); \
	else \
		LOGF_LOG_ERROR("class_add callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("Engine callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_CLASS_DEL(engine_id, iftype, ifinggrp, order, cl, flags) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for classifier delete operation is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.class_delete) \
		ret = es_tables.engine_info[engine_id].engine.class_delete(iftype, ifinggrp, order, cl, flags); \
	else \
		LOGF_LOG_ERROR("class_delete callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("Engine callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_CLASS_MODIFY(engine_id, iftype, ifinggrp, old_order, old_cl, order, cl, flags) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for classfier modify operation is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.class_modify) \
		ret = es_tables.engine_info[engine_id].engine.class_modify(iftype, ifinggrp, old_order, old_cl, order, cl, flags); \
	else \
		LOGF_LOG_ERROR("class_modify callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("Engine callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_INIT(engine_id, module) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for classifier delete operation is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.init) \
		ret = es_tables.engine_info[engine_id].engine.init(module); \
	else \
		LOGF_LOG_ERROR("init callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("init callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_FINI(engine_id, module) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for classifier delete operation is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.fini) \
		ret = es_tables.engine_info[engine_id].engine.fini(module); \
	else \
		LOGF_LOG_ERROR("finish callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("finish callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_REINIT(engine_id, module) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for classifier delete operation is not valid\n") \
		return ret; \
	} \
	if (es_tables.engine_info[engine_id].engine.reinit) \
		ret = es_tables.engine_info[engine_id].engine.reinit(module); \
	else \
		LOGF_LOG_ERROR("Re-init callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("Re-init callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		return ret; \
	}

#define QOSD_ENGINE_IFABS_SET(engine_id, ifgroup, ifname, oper) \
	if(engine_id >= QOS_ENGINE_MAX) { \
		LOGF_LOG_ERROR("Engine selected for classifier delete operation is not valid\n") \
		ret = LTQ_FAIL; \
	} \
	if (es_tables.engine_info[engine_id].engine.ifabs_set) \
		ret = es_tables.engine_info[engine_id].engine.ifabs_set(ifname, ifgroup, oper); \
	else { \
		LOGF_LOG_ERROR("Interface abstraction set callback is not provided by engine %s\n",es_tables.eng_names[engine_id]) \
		ret = LTQ_SUCCESS; \
	} \
	if (ret != LTQ_SUCCESS) { \
		LOGF_LOG_ERROR("Interface abstraction set callback failed for engine %s\n",es_tables.eng_names[engine_id]) \
		ret = LTQ_FAIL; \
	}

#define QOSD_VALIDATE_IF_NODE(if_list_node) \
	if (if_list_node == NULL){ \
		LOGF_LOG_ERROR("Interface node is NULL\n") \
		ret = LTQ_FAIL; \
		goto exit; \
	}

#define QOSD_GET_IFTYPE(ifname, ifmap) \
	iftype = qosd_get_iftype(ifname, ifmap); \
	if (iftype >= QOS_IF_CATEGORY_MAX){ \
		LOGF_LOG_ERROR("Interface %s could net get categorised \n",ifname) \
		ret = QOS_IFMAP_LOOKUP_FAIL; \
		goto exit; \
	}

#define QOSD_GET_IFINGGRP(ifname, inggrpmap) do{\
	ifinggrp = qosd_get_ifinggrp(ifname, inggrpmap); \
	if (ifinggrp >= QOS_IF_CAT_INGGRP_MAX){ \
		LOGF_LOG_ERROR("Interface %s could net get ingress group categorised \n",ifname) \
		ret = QOS_IFMAP_LOOKUP_FAIL; \
	} \
	} while(0)

#define QOSD_SHOW_DATAPATHS(dps, counter) \
	for (counter=0; i<MAX_SYS_DATAPATHS && dps[counter]; counter++){ \
		LOGF_LOG_INFO ("%d\n",dps[counter]); \
	} \

typedef struct if_category_map {
	int8_t ifcat;
	int32_t ifcount;
	char ifnames[20][MAX_IF_NAME_LEN];
	char base_ifnames[20][MAX_IF_NAME_LEN];
	char logical_base[20][MAX_IF_NAME_LEN];
} if_category_map_t;

typedef struct if_inggrp_map {
        int8_t inggrp;
        char ifnames[MAX_IF_PER_INGRESS_GROUP][MAX_IF_NAME_LEN];
} if_inggrp_map_t;

/* node of this type is added on each call of class_add. In case of template based rules where multiple sub-rules might be generated for a single rule, this node would be added for each sub-rules. Rule generator logic shall maintain list of sub-rules. */
typedef struct qos_class_node {
	struct list_head list;
	bool_t internal; /* this flag is set if rule is addded by rule generator logic */
	qos_class_cfg_t class_cfg;
	uint32_t flags; /* indicates parameters to configure in terms of engines' capabilities macros */
	uint32_t sub_rules_count; /* indicates number of subrules if it's app based classiiers, */
	struct list_head *sub_rules_list_head; /* Maintains a list of subrules, node type qos_class_node_t */
	uint32_t engine_rules_count; /* number of actual rules applied in engines for a user configured classifier */
	struct list_head *engine_rules_list_head; /* Maintains a list of rules applied at engines, node type qos_class_rule_t */
	qos_engines_t needed_classification_points[QOS_ENGINE_MAX]; /* There can be multiple classification points in case of partial classification */
	qos_engines_t classification_points[QOS_ENGINE_MAX]; /* There can be multiple classification points in case of partial classification */
	//why do we need to have a flowid here?
} qos_class_node_t;

typedef struct qos_class_rule {
	struct list_head list;
	uint32_t engine; /* engine identifier at which this subrule is applied */
	uint32_t order; /* actual order at engine */
	uint32_t flags; /* to indicate criteria and actions configured in this subrule */
	qos_class_node_t* parent; /* pointer to rule to get classifier criteria and actions based on flag OR we can havea pointer to class_cfg itself*/
} qos_class_rule_t;

/* ES uses this structure to maintain list of registered engines and their capabilities */
typedef struct qos_engine_info {
	uint32_t status; /* indicates engine's rgistration status */
	uint32_t cl_initialized; //to indicate if ClassInit has been called, reset this when this ClassUnInit is called
	uint32_t cl_wt; //can be modified by ES based on factors such as resources, system load and so on..
	uint32_t cl_current; // indicates the number of classifiers currently configured at the engine.
	uint32_t q_initialized; //to indicate if QueueInit has been called, reset this when this QueueUnInit is called
	uint32_t q_wt; //can be modified by ES based on factors such as resources, system load and so on..
	qos_engine_t engine;
	uint32_t rules_count;
	qos_class_node_t* rules[QOS_IF_CATEGORY_MAX][MAX_RULES]; //list of classifers added in this engine, can be useful in debuging. these pointers are to the nodes of rules list inside qos_class_node_t. This list has to be updated on class add/delete operations. instead of 2D array, change to array of struct
	//qos_class_rule_t* rules[MAX_RULES]; //list of classifers added in this engine, can be useful in debuging. these pointers are to the nodes of rules list inside qos_class_node_t. This list has to be updated on class add/delete operations. 
} qos_engine_info_t;

typedef struct qos_engine_node {
	struct list_head list;
	qos_engine_info_t eng_info;
} qos_engine_node_t;

/* should we avoid too many levels of nesting */
typedef struct qos_classess {
	uint32_t no_of_classifiers; //can be usefull to traverse DLL forward/backward based on requested node.
	qos_class_node_t* classifiers;
} qos_classes_t;

typedef struct qos_queue_node {
	struct list_head list;
	qos_queue_cfg_t queue_cfg;
	qos_engines_t q_point;
	uint32_t orig_wt;
	uint32_t	enable; // Whether Queue corresponding to this node is applied in the system
} qos_queue_node_t;

typedef struct qos_port_shaper_node {
  char ifname[MAX_IF_NAME_LEN];
	qos_engines_t shaping_point;
	qos_shaper_t shaper;
	int32_t weight;
	int32_t priority;
	uint32_t flags;
} qos_port_shaper_node_t;

typedef struct qos_if_node {
	struct list_head list;
	char ifname[MAX_IF_NAME_LEN];

	/* Queueing Information for the interface */
	uint32_t sum_wts;
	uint32_t sum_cirs;
	uint32_t wfq_prio;
	uint32_t q_count;
	uint32_t port_rate; // port rate in Kbps
	uint32_t enable; //Whether Queues corresponding to this node are applied in the system
	uint32_t ingress; //Whether this node holds ingress configuration
	qos_engines_t common_q_point;
	struct list_head *q_list_head;

	/* Classification Information for the interface */
	//struct list_head *class_list_head;
	uint32_t class_count;
	qos_class_node_t* classifiers[MAX_RULES]; // TODO use macro for max number of classifiers per interface
	uint32_t common_cl_points_count;
	qos_engines_t common_cl_points[QOS_ENGINE_MAX]; /* There can be multiple engines where classifers are configured for this interface*/


	/* Port Configuration in case interface is used to apply port level configuration */
	qos_port_shaper_node_t *port_shaper;
} qos_if_node_t;

/*
typedef struct qos_queues {
	uint32_t no_of_queues;
	qos_queue_node_t* queues;
} qos_queues_t; //we can maintain queuelist per interface
*/
//port shaper handling missing

typedef struct qos_port_shaper {
	uint32_t no_of_port_shapers;
	qos_port_shaper_node_t* port_shapers;
} qos_port_shaper_t;

typedef struct es_tables {
	char plat_name[MAX_NAME_LEN];
	int8_t qos_enable;
	int8_t wmm_ena;
	uint8_t dos_qos_ena;
	int32_t number_of_engines;
	int32_t number_of_datapaths;
	int32_t number_of_datapaths_bw_ifs;
	int32_t act_if_cnt;
	int32_t act_ifgrp_cnt;
	struct list_head *ingress_if_list_head;
	struct list_head *egress_if_list_head;
	struct list_head *class_list_head;
	char eng_names[MAX_ENGINES][MAX_IF_NAME_LEN];
	int32_t eng_ids[MAX_ENGINES];
	char ifcat[QOS_IF_CATEGORY_MAX][MAX_NAME_LEN]; /* ifcat[0] = ETHLAN, ifcat[1] = ETHWAN, .. */
	if_category_map_t ifmap[QOS_IF_CATEGORY_MAX]; /* ifmap[0] = {.ifname="eth0", .ifcat=0} where ifcat is an index to ifcat array */
	if_inggrp_map_t ifinggrp_map[QOS_IF_CAT_INGGRP_MAX];
	char dp_types [QOS_DATAPATH_MAX][MAX_NAME_LEN];
	qos_dp_info_t sys_datapaths[MAX_SYS_DATAPATHS];
	int32_t datapath_tbl[QOS_IF_CATEGORY_MAX][QOS_IF_CATEGORY_MAX][FLOWS_BW_IFS]; //waste of memory, need to store differently
	int32_t disabled_datapaths[MAX_SYS_DATAPATHS]; /* List of Datapath Ids currently disabled in the system */
	qos_engine_info_t engine_info[QOS_ENGINE_MAX]; //this can be aloocated from heap
	uint32_t q_counter; /* Counter used to derive queue id */
        int32_t log_level; /*!< Log enable flag */
        log_type_t log_type; /*!< Log type */
	char version[MAX_NAME_LEN];
} es_tables_t;

/* Consider renaming above structs name */
typedef struct qos_q_capab {
	uint32_t q_count;
	q_cap_t q_cap;
	qos_engines_t q_point;
} qos_q_capab_t;

typedef struct qos_cl_capab {
	uint32_t cl_count;
	cl_cap_t cl_cap;
	qos_engines_t cl_points[QOS_ENGINE_MAX];
} qos_cl_capab_t;

typedef struct qos_capab_cfg {
	qos_module_type_t module;
	union {
		qos_q_capab_t q_capab;
		qos_cl_capab_t cl_capab;
	} capab_cfg;
} qos_capab_cfg_t;

extern int32_t qosd_populate_tbls(char *file, es_tables_t *tbl);
extern int32_t qosd_show_dps(es_tables_t *tbl);
extern int32_t qosd_show_dps_bw_ifaces(es_tables_t *tbl);
extern int32_t cpu_n_eng_reg(qos_engine_t *eng);
extern int32_t switch_eng_reg(qos_engine_t *eng);
extern int32_t ppe_e_eng_reg(qos_engine_t *eng);
extern int32_t ppa_eng_reg(qos_engine_t *eng);

int32_t cpu_n_eng_fini(qos_module_type_t mtype);
int32_t switch_eng_fini(qos_module_type_t mtype);
int32_t ppe_e_eng_fini(qos_module_type_t mtype);
qos_class_rule_t*
qosd_get_engine_node(struct list_head *engine_list_head, qos_engines_t engine);
int32_t  
qosd_add_engine_rule(struct list_head **engine_rule_head, qos_class_rule_t **engine_rule_node);
int32_t  
qosd_del_engine_rule(struct list_head **engine_rules_list_head, qos_class_rule_t *engine_rule_node);
qos_class_node_t*
qosd_get_class_node(struct list_head *class_list_head, char *name, char *rx_if);
int32_t
qosd_del_class_node(struct list_head **class_list_head, qos_class_node_t *class_list_node);
int32_t
qosd_add_class_node(struct list_head **class_list_head, qos_class_node_t **class_list_node);
qos_if_node_t*
qosd_get_if_node(struct list_head *if_list_head, char *ifname);
qos_queue_node_t*
qosd_get_q_node(struct list_head *q_list_head, char *qname);
int32_t
qosd_add_if_node(struct list_head **if_list_head, char *ifname, qos_if_node_t **if_list_node);
int32_t  
qosd_del_if_node(struct list_head **if_list_head, char *ifname, es_tables_t *es_tables);
qos_queue_node_t*  
qosd_add_q_node(struct list_head **q_list_head);
int32_t
qosd_del_q_node(qos_if_node_t* if_list_node, qos_queue_cfg_t *q);
iftype_t qosd_get_iftype (char *ifname, if_category_map_t *ifmap);
ifinggrp_t qosd_get_ifinggrp (char *ifname, if_inggrp_map_t *if_inggrp_map);
int32_t qosd_str_to_int(char const *param, int32_t *data);
int32_t
qosd_show_dps(es_tables_t *tbl);
int32_t
qosd_show_dps_bw_ifaces(es_tables_t *tbl);
int32_t
qosd_populate_tbls(char *file, es_tables_t *tbl);
int32_t
qosd_get_dps_for_iface(es_tables_t *tbl, iftype_t from, iftype_t to, int32_t dps[], qos_dp_type_t filter);
int32_t
qosd_get_common_engine(int32_t dps[], int32_t pos, int32_t ingress, qos_engines_t *engine1);
int32_t
qosd_get_action_point(int32_t dps[], qos_capab_cfg_t *capab, int32_t forward, uint32_t starting_engine);
int32_t
qosd_disable_datapaths(char *ifname, iftype_t iftype, int32_t dps_to_disable[]);
int32_t
qosd_enable_datapaths(char *ifname, iftype_t iftype,  int32_t dps_to_enable[]);
int32_t
qosd_get_dps_wo_given_engine(int32_t dps[], qos_engines_t engine, IN int32_t ingress, int32_t dps_to_disable[]);
int32_t
qosd_filter_common_dps(int32_t super[], int32_t sub[], int32_t filter[]);
int32_t
qosd_prepare_needed_capab(qos_queue_cfg_t *q, iftype_t iftype, uint32_t *needed_capab);
int32_t
qosd_prepare_needed_cl_capab(qos_class_cfg_t *class_cfg, iftype_t iftype, cl_cap_t *needed_capab);
int32_t
qosd_prepare_needed_capab_port(qos_shaper_t *shaper, iftype_t iftype, uint32_t *needed_capab);
int32_t qosd_cfg_get(qos_cfg_t *qoscfg, uint32_t flags);
int32_t qosd_cfg_set(qos_cfg_t *qoscfg);

int32_t
qosd_classifier_get(
	uint32_t				order,
	int32_t					*num_entries,
	qos_class_cfg_t **class_cfg,
	uint32_t				flags);

int32_t
qosd_classifier_set(
	int32_t			order,
	qos_class_cfg_t *class_cfg,
	uint32_t 		flags);

int32_t
qosd_queue_set(char *ifname,
		qos_queue_cfg_t	*q,
		uint32_t flags);
int32_t
qosd_queue_get(char			*ifname,
							//int32_t     queue_id,
							char*     queue_name,
							uint32_t			*num_queues,
							qos_queue_cfg_t		**q,
							uint32_t 		flags);
int32_t
qosd_queue_stats_get(
		char			*ifname,
		char*			queue_name,
		int32_t			*num_queues,
		qos_queue_stats_t	**qstats,
		uint32_t 		flags);
int32_t
qosd_port_config_set(char			*ifname,
										qos_shaper_t		*shaper,
										int32_t			weight,
										int32_t			priority,
										uint32_t 		flags);
int32_t
qosd_port_config_get(char			*ifname,
										qos_shaper_t		*shaper,
										int32_t			*weight,
										int32_t			*priority,
										uint32_t 		flags);
int32_t
qosd_port_update(char *ifname,
		uint32_t flags);
int32_t
qosd_if_abs_set(
	iftype_t	ifgroup,
	char		*ifname,
	uint32_t 	flags);
int32_t
qosd_if_abs_get(
	iftype_t	ifgroup,
	int32_t		*num_entries,
	char		(*ifnames)[MAX_IF_NAME_LEN],
	uint32_t 	flags);
int32_t
qosd_if_base_set(
	char		*ifname,
	char		*base_ifname,
	char		*logical_base,
	uint32_t 	flags);
int32_t
qosd_if_base_get(
	char		*ifname,
	char		*base_ifname,
	char		*logical_base,
	uint32_t 	flags);
int32_t
qosd_if_inggrp_set(
	ifinggrp_t ifinggrp,
    char *ifname,
    uint32_t flags);
int32_t
qosd_if_inggrp_get(
    ifinggrp_t ifinggrp,
    int32_t *num_entries,
    char (*ifnames)[MAX_IF_NAME_LEN],
    uint32_t flags);

if_inggrp_map_t *inggrp_map_get_by_ifname(const char *ifname);

int32_t q_wt_n_rate_derive(iftype_t iftype, qos_if_node_t *if_list_node, qos_queue_cfg_t *qcfg);
int32_t es_init(qos_cfg_t *);
int32_t es_shut(void);
int32_t qosd_show_cfg(es_tables_t *tbl);
int32_t qosd_show_classifers (void);
int32_t qosd_show_queues(void);
char * qosd_get_str_drop_mode(qos_drop_mode_t drop_mode);
char * qosd_get_str_sched_mode(qos_sched_mode_t sched_mode);
char * qosd_get_str_shaper(qos_shaper_mode_t shaper);

int32_t validate_q_rate(char *ifname, iftype_t new_q_iftype, uint32_t new_commit_rate, uint32_t new_peak_rate);
int32_t is_q_assoc_with_cl(char *ifname, iftype_t new_q_iftype, uint32_t new_commit_rate, uint32_t new_peak_rate);
int32_t is_q_present_for_tc(uint32_t tc);
int32_t is_cl_present_for_q(qos_queue_cfg_t *q);
int32_t uniqueness_of_q_tc(iftype_t new_q_iftype, qos_queue_cfg_t *q );
void es_log_init(qos_cfg_t *qoscfg);
int32_t default_qos_cfg(int32_t nEna);

#endif //__QOS_AL_ES_H
