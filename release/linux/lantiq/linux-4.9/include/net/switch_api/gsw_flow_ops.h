/******************************************************************************
                Copyright (c) 2016, 2017 Intel Corporation

For licensing information, see the file 'LICENSE' in the root folder of
this software module.
******************************************************************************/


#ifndef _GSW_FLOW_OPS_H_
#define _GSW_FLOW_OPS_H_

#include "gsw_types.h"
#include "lantiq_gsw_api.h"
#include "gsw_irq.h"


/*RMON operation*/
struct rmon_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_RMON_PORT_GET ; Index: 0x01 */
	GSW_return_t (*RMON_Port_Get)(void *, GSW_RMON_Port_cnt_t *);
	/* Command: GSW_RMON_MODE_SET ; Index: 0x02 */
	GSW_return_t (*RMON_Mode_Set)(void *, GSW_RMON_mode_t *);
	/* Command: GSW_RMON_METER_GET ; Index: 0x03 */
	GSW_return_t (*RMON_Meter_Get)(void *, GSW_RMON_Meter_cnt_t *);
	/* Command: GSW_RMON_REDIRECT_GET ; Index: 0x04 */
	GSW_return_t (*RMON_Redirect_Get)(void *, GSW_RMON_Redirect_cnt_t *);
	/* Command: GSW_RMON_IF_GET ; Index: 0x05 */
	GSW_return_t (*RMON_If_Get)(void *, GSW_RMON_If_cnt_t *);
	/* Command: GSW_RMON_ROUTE_GET ; Index: 0x06 */
	GSW_return_t (*RMON_Route_Get)(void *, GSW_RMON_Route_cnt_t *);
	/* Command: GSW_RMON_CLEAR ; Index: 0x07 */
	GSW_return_t (*RMON_Clear)(void *, GSW_RMON_clear_t *);
	/* Command: GSW_RMON_EXTEND_GET ; Index: 0x08 */
	GSW_return_t (*RMON_ExtendGet)(void *, GSW_RMON_extendGet_t *);
	/* Command: GSW_RMON_FLOW_GET ; Index: 0x09 */
	GSW_return_t (*RMON_TflowGet)(void *, GSW_RMON_flowGet_t *);
	/* Command: GSW_RMON_TFLOW_CLEAR ; Index: 0x0A */
	GSW_return_t (*RMON_TflowClear)(void *, GSW_RMON_flowGet_t *);
	/* Command: GSW_TFLOW_COUNT_MODE_SET ; Index: 0x0B */
	GSW_return_t (*RMON_TflowCountModeSet)(void *, GSW_TflowCmodeConf_t *);
	/* Command: GSW_TFLOW_COUNT_MODE_GET ; Index: 0x0C */
	GSW_return_t (*RMON_TflowCountModeGet)(void *, GSW_TflowCmodeConf_t *);
};

/*Switch MAc operations*/
struct swmac_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_MAC_TABLE_CLEAR ; Index: 0x01 */
	GSW_return_t (*MAC_TableClear)(void *);
	/* Command: GSW_MAC_TABLE_ENTRY_ADD ; Index: 0x02 */
	GSW_return_t (*MAC_TableEntryAdd)(void *, GSW_MAC_tableAdd_t *);
	/* Command: GSW_MAC_TABLE_ENTRY_READ ; Index: 0x03 */
	GSW_return_t (*MAC_TableEntryRead)(void *, GSW_MAC_tableRead_t *);
	/* Command: GSW_MAC_TABLE_ENTRY_QUERY ; Index: 0x04 */
	GSW_return_t (*MAC_TableEntryQuery)(void *, GSW_MAC_tableQuery_t *);
	/* Command: GSW_MAC_TABLE_ENTRY_REMOVE ; Index: 0x05 */
	GSW_return_t (*MAC_TableEntryRemove)(void *, GSW_MAC_tableRemove_t *);
	/* Command: GSW_DEFAUL_MAC_FILTER_SET ; Index: 0x06 */
	GSW_return_t (*MAC_DefaultFilterSet)(void *, GSW_MACFILTER_default_t *);
	/* Command: GSW_DEFAUL_MAC_FILTER_GET ; Index: 0x07 */
	GSW_return_t (*MAC_DefaultFilterGet)(void *, GSW_MACFILTER_default_t *);
};

/*Extended Vlan operations*/
struct extvlan_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_EXTENDEDVLAN_ALLOC ; Index: 0x01 */
	GSW_return_t (*ExtendedVlan_Alloc)(void *, GSW_EXTENDEDVLAN_alloc_t *);
	/* Command: GSW_EXTENDEDVLAN_SET ; Index: 0x02 */
	GSW_return_t (*ExtendedVlan_Set)(void *, GSW_EXTENDEDVLAN_config_t *);
	/* Command: GSW_EXTENDEDVLAN_GET ; Index: 0x03 */
	GSW_return_t (*ExtendedVlan_Get)(void *, GSW_EXTENDEDVLAN_config_t *);
	/* Command: GSW_EXTENDEDVLAN_FREE ; Index: 0x04 */
	GSW_return_t (*ExtendedVlan_Free)(void *, GSW_EXTENDEDVLAN_alloc_t *);
};

/*Vlan Filter operations*/
struct vlanfilter_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_VLANFILTER_ALLOC ; Index: 0x01 */
	GSW_return_t (*VlanFilter_Alloc)(void *, GSW_VLANFILTER_alloc_t *);
	/* Command: GSW_VLANFILTER_SET ; Index: 0x02 */
	GSW_return_t (*VlanFilter_Set)(void *, GSW_VLANFILTER_config_t *);
	/* Command: GSW_VLANFILTER_GET ; Index: 0x03 */
	GSW_return_t (*VlanFilter_Get)(void *, GSW_VLANFILTER_config_t *);
	/* Command: GSW_VLANFILTER_FREE ; Index: 0x04 */
	GSW_return_t (*VlanFilter_Free)(void *, GSW_VLANFILTER_alloc_t *);
};

/*CTP operations*/
struct ctp_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_CTP_PORT_ASSIGNMENT_ALLOC ; Index: 0x01 */
	GSW_return_t (*CTP_PortAssignmentAlloc)(void *, GSW_CTP_portAssignment_t *);
	/* Command: GSW_CTP_PORT_ASSIGNMENT_FREE ; Index: 0x02 */
	GSW_return_t (*CTP_PortAssignmentFree)(void *, GSW_CTP_portAssignment_t *);
	/* Command: GSW_CTP_PORT_ASSIGNMENT_SET ; Index: 0x03 */
	GSW_return_t (*CTP_PortAssignmentSet)(void *, GSW_CTP_portAssignment_t *);
	/* Command: GSW_CTP_PORT_ASSIGNMENT_GET ; Index: 0x04 */
	GSW_return_t (*CTP_PortAssignmentGet)(void *, GSW_CTP_portAssignment_t *);
	/* Command: GSW_CTP_PORT_CONFIG_SET ; Index: 0x05 */
	GSW_return_t (*CTP_PortConfigSet)(void *, GSW_CTP_portConfig_t *);
	/* Command: GSW_CTP_PORT_CONFIG_GET ; Index: 0x06 */
	GSW_return_t (*CTP_PortConfigGet)(void *, GSW_CTP_portConfig_t *);
	/* Command: GSW_CTP_PORT_CONFIG_RESET ; Index: 0x07 */
	GSW_return_t (*CTP_PortConfigReset)(void *, GSW_CTP_portConfig_t *);
};

/*Bridge Port operations*/
struct brdgport_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_BRIDGE_PORT_ALLOC ; Index: 0x01 */
	GSW_return_t (*BridgePort_Alloc)(void *, GSW_BRIDGE_portAlloc_t *);
	/* Command: GSW_BRIDGE_PORT_CONFIG_SET ; Index: 0x02 */
	GSW_return_t (*BridgePort_ConfigSet)(void *, GSW_BRIDGE_portConfig_t *);
	/* Command: GSW_BRIDGE_PORT_CONFIG_GET ; Index: 0x03 */
	GSW_return_t (*BridgePort_ConfigGet)(void *, GSW_BRIDGE_portConfig_t *);
	/* Command: GSW_BRIDGE_PORT_FREE ; Index: 0x04 */
	GSW_return_t (*BridgePort_Free)(void *, GSW_BRIDGE_portAlloc_t *);
};

/*Bridge Operations*/
struct brdg_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_BRIDGE_ALLOC ; Index: 0x01 */
	GSW_return_t (*Bridge_Alloc)(void *, GSW_BRIDGE_alloc_t *);
	/* Command: GSW_BRIDGE_CONFIG_SET ; Index: 0x02 */
	GSW_return_t (*Bridge_ConfigSet)(void *, GSW_BRIDGE_config_t *);
	/* Command: GSW_BRIDGE_CONFIG_GET ; Index: 0x03 */
	GSW_return_t (*Bridge_ConfigGet)(void *, GSW_BRIDGE_config_t *);
	/* Command: GSW_BRIDGE_FREE ; Index: 0x04 */
	GSW_return_t (*Bridge_Free)(void *, GSW_BRIDGE_alloc_t *);
};

/*TFlow operations*/
struct tflow_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_PCE_RULE_DELETE ; Index: 0x01 */
	GSW_return_t (*TFLOW_PceRuleDelete)(void *, GSW_PCE_ruleDelete_t *);
	/* Command: GSW_PCE_RULE_READ ; Index: 0x02 */
	GSW_return_t (*TFLOW_PceRuleRead)(void *, GSW_PCE_rule_t *);
	/* Command: GSW_PCE_RULE_WRITE ; Index: 0x03 */
	GSW_return_t (*TFLOW_PceRuleWrite)(void *, GSW_PCE_rule_t *);
	/* Command: GSW_TFLOW_ALLOC ; Index: 0x04 */
	GSW_return_t (*tflow_pcealloc)(void *, gsw_tflow_alloc_t *);
	/* Command: GSW_TFLOW_FREE ; Index: 0x05 */
	GSW_return_t (*tflow_pcefree)(void *, gsw_tflow_alloc_t *);
};

/*QOS operations*/
struct qos_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_QOS_METER_CFG_GET ; Index: 0x01 */
	GSW_return_t (*QoS_MeterCfgGet)(void *, GSW_QoS_meterCfg_t *);
	/* Command: GSW_QOS_METER_CFG_SET ; Index: 0x02 */
	GSW_return_t (*QoS_MeterCfgSet)(void *, GSW_QoS_meterCfg_t *);
	/* Command: GSW_QOS_METER_PORT_ASSIGN ; Index: 0x03 */
	GSW_return_t (*QoS_MeterPortAssign)(void *, GSW_QoS_meterPort_t *);
	/* Command: GSW_QOS_METER_PORT_DEASSIGN ; Index: 0x04 */
	GSW_return_t (*QoS_MeterPortDeassign)(void *, GSW_QoS_meterPort_t *);
	/* Command: GSW_QOS_METER_PORT_GET ; Index: 0x05 */
	GSW_return_t (*QoS_MeterPortGet)(void *, GSW_QoS_meterPortGet_t *);
	/* Command: GSW_QOS_DSCP_CLASS_GET ; Index: 0x06 */
	GSW_return_t (*QoS_DSCP_ClassGet)(void *, GSW_QoS_DSCP_ClassCfg_t *);
	/* Command: GSW_QOS_DSCP_CLASS_SET ; Index: 0x07 */
	GSW_return_t (*QoS_DSCP_ClassSet)(void *, GSW_QoS_DSCP_ClassCfg_t *);
	/* Command: GSW_QOS_CLASS_DSCP_GET ; Index: 0x08 */
	GSW_return_t (*QoS_ClassDSCP_Get)(void *, GSW_QoS_ClassDSCP_Cfg_t *);
	/* Command: GSW_QOS_CLASS_DSCP_SET ; Index: 0x09 */
	GSW_return_t (*QoS_ClassDSCP_Set)(void *, GSW_QoS_ClassDSCP_Cfg_t *);
	/* Command: GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_GET ; Index: 0x0A */
	GSW_return_t (*QoS_DSCP_DropPrecedenceCfgGet)(void *, GSW_QoS_DSCP_DropPrecedenceCfg_t *);
	/* Command: GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_SET ; Index: 0x0B */
	GSW_return_t (*QoS_DSCP_DropPrecedenceCfgSet)(void *, GSW_QoS_DSCP_DropPrecedenceCfg_t *);
	/* Command: GSW_QOS_PORT_REMARKING_CFG_GET ; Index: 0x0C */
	GSW_return_t (*QoS_PortRemarkingCfgGet)(void *, GSW_QoS_portRemarkingCfg_t *);
	/* Command: GSW_QOS_PORT_REMARKING_CFG_SET ; Index: 0x0D */
	GSW_return_t (*QoS_PortRemarkingCfgSet)(void *, GSW_QoS_portRemarkingCfg_t *);
	/* Command: GSW_QOS_CLASS_PCP_GET ; Index: 0x0E*/
	GSW_return_t (*QoS_ClassPCP_Get)(void *, GSW_QoS_ClassPCP_Cfg_t *);
	/* Command: GSW_QOS_CLASS_PCP_SET ; Index: 0x0F */
	GSW_return_t (*QoS_ClassPCP_Set)(void *, GSW_QoS_ClassPCP_Cfg_t *);
	/* Command: GSW_QOS_PCP_CLASS_GET ; Index: 0x10 */
	GSW_return_t (*QoS_PCP_ClassGet)(void *, GSW_QoS_PCP_ClassCfg_t *);
	/* Command: GSW_QOS_PCP_CLASS_SET ; Index: 0x11 */
	GSW_return_t (*QoS_PCP_ClassSet)(void *, GSW_QoS_PCP_ClassCfg_t *);
	/* Command: GSW_QOS_PORT_CFG_GET ; Index: 0x12 */
	GSW_return_t (*QoS_PortCfgGet)(void *, GSW_QoS_portCfg_t *);
	/* Command: GSW_QOS_PORT_CFG_SET ; Index: 0x13 */
	GSW_return_t (*QoS_PortCfgSet)(void *, GSW_QoS_portCfg_t *);
	/* Command: GSW_QOS_QUEUE_PORT_GET ; Index: 0x14 */
	GSW_return_t (*QoS_QueuePortGet)(void *, GSW_QoS_queuePort_t *);
	/* Command: GSW_QOS_QUEUE_PORT_SET ; Index: 0x15 */
	GSW_return_t (*QoS_QueuePortSet)(void *, GSW_QoS_queuePort_t *);
	/* Command: GSW_QOS_SCHEDULER_CFG_GET ; Index: 0x16 */
	GSW_return_t (*QoS_SchedulerCfgGet)(void *, GSW_QoS_schedulerCfg_t *);
	/* Command: GSW_QOS_SCHEDULER_CFG_SET ; Index: 0x17 */
	GSW_return_t (*QoS_SchedulerCfgSet)(void *, GSW_QoS_schedulerCfg_t *);
	/* Command: GSW_QOS_SHAPER_CFG_GET ; Index: 0x18 */
	GSW_return_t (*QoS_ShaperCfgGet)(void *, GSW_QoS_ShaperCfg_t *);
	/* Command: GSW_QOS_SHAPER_CFG_SET ; Index: 0x19 */
	GSW_return_t (*QoS_ShaperCfgSet)(void *, GSW_QoS_ShaperCfg_t *);
	/* Command: GSW_QOS_SHAPER_QUEUE_ASSIGN ; Index: 0x1A */
	GSW_return_t (*QoS_ShaperQueueAssign)(void *, GSW_QoS_ShaperQueue_t *);
	/* Command: GSW_QOS_SHAPER_QUEUE_DEASSIGN ; Index: 0x1B */
	GSW_return_t (*QoS_ShaperQueueDeassign)(void *, GSW_QoS_ShaperQueue_t *);
	/* Command: GSW_QOS_SHAPER_QUEUE_GET ; Index: 0x1C */
	GSW_return_t (*QoS_ShaperQueueGet)(void *, GSW_QoS_ShaperQueueGet_t *);
	/* Command: GSW_QOS_STORM_CFG_SET ; Index: 0x1D */
	GSW_return_t (*QoS_StormCfgSet)(void *, GSW_QoS_stormCfg_t *);
	/* Command: GSW_QOS_STORM_CFG_GET ; Index: 0x1E */
	GSW_return_t (*QoS_StormCfgGet)(void *, GSW_QoS_stormCfg_t *);
	/* Command: GSW_QOS_WRED_CFG_GET ; Index: 0x1F */
	GSW_return_t (*QoS_WredCfgGet)(void *, GSW_QoS_WRED_Cfg_t *);
	/* Command: GSW_QOS_WRED_CFG_SET ; Index: 0x20 */
	GSW_return_t (*QoS_WredCfgSet)(void *, GSW_QoS_WRED_Cfg_t *);
	/* Command: GSW_QOS_WRED_QUEUE_CFG_GET ; Index: 0x21 */
	GSW_return_t (*QoS_WredQueueCfgGet)(void *, GSW_QoS_WRED_QueueCfg_t *);
	/* Command: GSW_QOS_WRED_QUEUE_CFG_SET ; Index: 0x22 */
	GSW_return_t (*QoS_WredQueueCfgSet)(void *, GSW_QoS_WRED_QueueCfg_t *);
	/* Command: GSW_QOS_WRED_PORT_CFG_GET ; Index: 0x23 */
	GSW_return_t (*QoS_WredPortCfgGet)(void *, GSW_QoS_WRED_PortCfg_t *);
	/* Command: GSW_QOS_WRED_PORT_CFG_SET ; Index: 0x24 */
	GSW_return_t (*QoS_WredPortCfgSet)(void *, GSW_QoS_WRED_PortCfg_t *);
	/* Command: GSW_QOS_FLOWCTRL_CFG_GET ; Index: 0x25 */
	GSW_return_t (*QoS_FlowctrlCfgGet)(void *, GSW_QoS_FlowCtrlCfg_t *);
	/* Command: GSW_QOS_FLOWCTRL_CFG_SET ; Index: 0x26 */
	GSW_return_t (*QoS_FlowctrlCfgSet)(void *, GSW_QoS_FlowCtrlCfg_t *);
	/* Command: GSW_QOS_FLOWCTRL_PORT_CFG_GET ; Index: 0x27 */
	GSW_return_t (*QoS_FlowctrlPortCfgGet)(void *, GSW_QoS_FlowCtrlPortCfg_t *);
	/* Command: GSW_QOS_FLOWCTRL_PORT_CFG_SET ; Index: 0x28 */
	GSW_return_t (*QoS_FlowctrlPortCfgSet)(void *, GSW_QoS_FlowCtrlPortCfg_t *);
	/* Command: GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_GET ; Index: 0x29 */
	GSW_return_t (*QoS_QueueBufferReserveCfgGet)(void *, GSW_QoS_QueueBufferReserveCfg_t *);
	/* Command: GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_SET ; Index: 0x2A */
	GSW_return_t (*QoS_QueueBufferReserveCfgSet)(void *, GSW_QoS_QueueBufferReserveCfg_t *);
	/* Command: GSW_QOS_METER_ACT ; Index: 0x2B */
	GSW_return_t (*QoS_Meter_Act)(void *, GSW_QoS_mtrAction_t *);
	/* Command: GSW_QOS_COLOR_MARKING_TABLE_SET ; Index: 0x2C */
	GSW_return_t (*QOS_ColorMarkingTableSet)(void *, GSW_QoS_colorMarkingEntry_t *);
	/* Command: GSW_QOS_COLOR_MARKING_TABLE_GET ; Index: 0x2D */
	GSW_return_t (*QOS_ColorMarkingTableGet)(void *, GSW_QoS_colorMarkingEntry_t *);
	/* Command: GSW_QOS_COLOR_REMARKING_TABLE_SET ; Index: 0x2E */
	GSW_return_t (*QOS_ColorReMarkingTableSet)(void *, GSW_QoS_colorRemarkingEntry_t *);
	/* Command: GSW_QOS_COLOR_REMARKING_TABLE_GET ; Index: 0x2F */
	GSW_return_t (*QOS_ColorReMarkingTableGet)(void *, GSW_QoS_colorRemarkingEntry_t *);
	/* Command: GSW_QOS_METER_ALLOC ; Index: 0x30 */
	GSW_return_t (*QOS_MeterAlloc)(void *, GSW_QoS_meterCfg_t *param);
	/* Command: GSW_QOS_METER_FREE ; Index: 0x31 */
	GSW_return_t (*QOS_MeterFree)(void *, GSW_QoS_meterCfg_t *param);
	/* Command: GSW_DSCP2PCP_MAP_SET ; Index: 0x32 */
	GSW_return_t (*QOS_Dscp2PcpTableSet)(void *, GSW_DSCP2PCP_map_t *);
	/* Command: GSW_DSCP2PCP_MAP_GET ; Index: 0x33 */
	GSW_return_t (*QOS_Dscp2PcpTableGet)(void *, GSW_DSCP2PCP_map_t *);
	/* Command: GSW_PMAPPER_SET ; Index: 0x34 */
	GSW_return_t (*QOS_PmapperTableSet)(void *, GSW_PMAPPER_t *);
	/* Command: GSW_PMAPPER_GET ; Index: 0x35 */
	GSW_return_t (*QOS_PmapperTableGet)(void *, GSW_PMAPPER_t *);
	/* Command: GSW_QOS_SVLAN_CLASS_PCP_PORT_GET ; Index: 0x36 */
	GSW_return_t (*QoS_SVLAN_ClassPCP_PortGet)(void *, GSW_QoS_SVLAN_ClassPCP_PortCfg_t *);
	/* Command: GSW_QOS_SVLAN_CLASS_PCP_PORT_SET ; Index: 0x37 */
	GSW_return_t (*QoS_SVLAN_ClassPCP_PortSet)(void *, GSW_QoS_SVLAN_ClassPCP_PortCfg_t *);
	/* Command: GSW_QOS_SVLAN_PCP_CLASS_GET ; Index: 0x38 */
	GSW_return_t (*QoS_SVLAN_PCP_ClassGet)(void *, GSW_QoS_SVLAN_PCP_ClassCfg_t *);
	/* Command: GSW_QOS_SVLAN_PCP_CLASS_SET ; Index: 0x39 */
	GSW_return_t (*QoS_SVLAN_PCP_ClassSet)(void *, GSW_QoS_SVLAN_PCP_ClassCfg_t *);
};

/*STP operations*/
struct stp_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_STP_BPDU_RULE_GET ; Index: 0x01 */
	GSW_return_t (*STP_BPDU_RuleGet)(void *, GSW_STP_BPDU_Rule_t *);
	/* Command: GSW_STP_BPDU_RULE_SET ; Index: 0x02 */
	GSW_return_t (*STP_BPDU_RuleSet)(void *, GSW_STP_BPDU_Rule_t *);
	/* Command: GSW_STP_PORT_CFG_GET ; Index: 0x03 */
	GSW_return_t (*STP_PortCfgGet)(void *, GSW_STP_portCfg_t *);
	/* Command: GSW_STP_PORT_CFG_SET ; Index: 0x04 */
	GSW_return_t (*STP_PortCfgSet)(void *, GSW_STP_portCfg_t *);
};

/*8021x operations*/
struct eapol_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_8021X_EAPOL_RULE_GET ; Index: 0x01 */
	GSW_return_t (*EAPOL_RuleGet)(void *, GSW_8021X_EAPOL_Rule_t *);
	/* Command: GSW_8021X_EAPOL_RULE_SET ; Index: 0x02 */
	GSW_return_t (*EAPOL_RuleGet_RuleSet)(void *, GSW_8021X_EAPOL_Rule_t *);
	/* Command: GSW_8021X_PORT_CFG_GET ; Index: 0x03 */
	GSW_return_t (*EAPOL_RuleGet_PortCfgGet)(void *, GSW_8021X_portCfg_t *);
	/* Command: GSW_8021X_PORT_CFG_SET ; Index: 0x04 */
	GSW_return_t (*EAPOL_RuleGet_PortCfgSet)(void *, GSW_8021X_portCfg_t *);
};

/*multicast operations*/
struct multicast_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_MULTICAST_ROUTER_PORT_ADD ; Index: 0x01 */
	GSW_return_t (*Multicast_RouterPortAdd)(void *, GSW_multicastRouter_t *);
	/* Command: GSW_MULTICAST_ROUTER_PORT_READ ; Index: 0x02 */
	GSW_return_t (*Multicast_RouterPortRead)(void *, GSW_multicastRouterRead_t *);
	/* Command: GSW_MULTICAST_ROUTER_PORT_REMOVE ; Index: 0x03 */
	GSW_return_t (*Multicast_RouterPortRemove)(void *, GSW_multicastRouter_t *);
	/* Command: GSW_MULTICAST_SNOOP_CFG_GET ; Index: 0x04 */
	GSW_return_t (*Multicast_SnoopCfgGet)(void *, GSW_multicastSnoopCfg_t *);
	/* Command: GSW_MULTICAST_SNOOP_CFG_SET ; Index: 0x05 */
	GSW_return_t (*Multicast_SnoopCfgSet)(void *, GSW_multicastSnoopCfg_t *);
	/* Command: GSW_MULTICAST_TABLE_ENTRY_ADD ; Index: 0x06 */
	GSW_return_t (*Multicast_TableEntryAdd)(void *, GSW_multicastTable_t *);
	/* Command: GSW_MULTICAST_TABLE_ENTRY_READ ; Index: 0x07 */
	GSW_return_t (*Multicast_TableEntryRead)(void *, GSW_multicastTableRead_t *);
	/* Command: GSW_MULTICAST_TABLE_ENTRY_REMOVE ; Index: 0x08 */
	GSW_return_t (*Multicast_TableEntryRemove)(void *, GSW_multicastTable_t *);
};

/*Trunking operations*/
struct trunking_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_TRUNKING_CFG_GET ; Index: 0x01 */
	GSW_return_t (*Trunking_CfgGet)(void *, GSW_trunkingCfg_t *);
	/* Command: GSW_TRUNKING_CFG_SET ; Index: 0x02 */
	GSW_return_t (*Trunking_CfgSet)(void *, GSW_trunkingCfg_t *);
	/* Command: GSW_TRUNKING_PORT_CFG_GET ; Index: 0x03 */
	GSW_return_t (*Trunking_PortCfgGet)(void *, GSW_trunkingPortCfg_t *);
	/* Command: GSW_TRUNKING_PORT_CFG_SET ; Index: 0x04 */
	GSW_return_t (*Trunking_PortCfgSet)(void *, GSW_trunkingPortCfg_t *);
};

/*WOL operations*/
struct wol_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_WOL_CFG_GET ; Index: 0x01 */
	GSW_return_t (*WoL_CfgGet)(void *, GSW_WoL_Cfg_t *);
	/* Command: GSW_WOL_CFG_SET ; Index: 0x02 */
	GSW_return_t (*WoL_CfgSet)(void *, GSW_WoL_Cfg_t *);
	/* Command: GSW_WOL_PORT_CFG_GET ; Index: 0x03 */
	GSW_return_t (*WoL_PortCfgGet)(void *, GSW_WoL_PortCfg_t *);
	/* Command: GSW_WOL_PORT_CFG_SET ; Index: 0x04 */
	GSW_return_t (*WoL_PortCfgSet)(void *, GSW_WoL_PortCfg_t *);
};

/*Common switch operations*/
struct common_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_REGISTER_GET ; Index: 0x01 */
	GSW_return_t (*RegisterGet)(void *, GSW_register_t *);
	/* Command: GSW_REGISTER_SET ; Index: 0x02 */
	GSW_return_t (*RegisterSet)(void *, GSW_register_t *);
	/* Command: GSW_IRQ_GET ; Index: 0x03 */
	GSW_return_t (*IrqGet)(void *, GSW_irq_t *);
	/* Command: GSW_IRQ_MASK_GET ; Index: 0x04 */
	GSW_return_t (*IrqMaskGet)(void *, GSW_irq_t *);
	/* Command: GSW_IRQ_MASK_SET ; Index: 0x05 */
	GSW_return_t (*IrqMaskSet)(void *, GSW_irq_t *);
	/* Command: GSW_IRQ_STATUS_CLEAR ; Index: 0x06 */
	GSW_return_t (*IrqStatusClear)(void *, GSW_irq_t *);
	/* Command: GSW_ENABLE ; Index: 0x07 */
	GSW_return_t (*Enable)(void *);
	/* Command: GSW_RESET ; Index: 0x08 */
	GSW_return_t (*Reset)(void *, GSW_reset_t *);
	/* Command: GSW_DISABLE ; Index: 0x09 */
	GSW_return_t (*Disable)(void *);
	/* Command: GSW_VERSION_GET ; Index: 0x0A */
	GSW_return_t (*VersionGet)(void *, GSW_version_t *);
	/* Command: GSW_CAP_GET ; Index: 0x0B */
	GSW_return_t (*CapGet)(void *, GSW_cap_t *);
	/* Command: GSW_CFG_GET ; Index: 0x0C */
	GSW_return_t (*CfgGet)(void *, GSW_cfg_t *);
	/* Command: GSW_CFG_SET ; Index: 0x0D */
	GSW_return_t (*CfgSet)(void *, GSW_cfg_t *);
	/* Command: GSW_HW_INIT ; Index: 0x0E */
	GSW_return_t (*HW_Init)(void *, GSW_HW_Init_t *);
	/* Command: GSW_PORT_LINK_CFG_GET ; Index: 0x0F */
	GSW_return_t (*PortLinkCfgGet)(void *, GSW_portLinkCfg_t *);
	/* Command: GSW_PORT_LINK_CFG_SET ; Index: 0x10 */
	GSW_return_t (*PortLinkCfgSet)(void *, GSW_portLinkCfg_t *);
	/* Command: GSW_PORT_PHY_ADDR_GET ; Index: 0x11 */
	GSW_return_t (*PortPHY_AddrGet)(void *, GSW_portPHY_Addr_t *);
	/* Command: GSW_PORT_PHY_QUERY ; Index: 0x12 */
	GSW_return_t (*PortPHY_Query)(void *, GSW_portPHY_Query_t *);
	/* Command: GSW_PORT_RGMII_CLK_CFG_GET ; Index: 0x13 */
	GSW_return_t (*PortRGMII_ClkCfgGet)(void *, GSW_portRGMII_ClkCfg_t *);
	/* Command: GSW_PORT_RGMII_CLK_CFG_SET ; Index: 0x14 */
	GSW_return_t (*PortRGMII_ClkCfgSet)(void *, GSW_portRGMII_ClkCfg_t *);
	/* Command: GSW_PORT_REDIRECT_GET ; Index: 0x15 */
	GSW_return_t (*PortRedirectGet)(void *, GSW_portRedirectCfg_t *);
	/* Command: GSW_PORT_REDIRECT_SET ; Index: 0x16 */
	GSW_return_t (*PortRedirectSet)(void *, GSW_portRedirectCfg_t *);
	/* Command: GSW_CPU_PORT_CFG_GET ; Index: 0x17 */
	GSW_return_t (*CPU_PortCfgGet)(void *, GSW_CPU_PortCfg_t *);
	/* Command: GSW_CPU_PORT_CFG_SET ; Index: 0x18 */
	GSW_return_t (*CPU_PortCfgSet)(void *, GSW_CPU_PortCfg_t *);
	/* Command: GSW_CPU_PORT_EXTEND_CFG_GET ; Index: 0x19 */
	GSW_return_t (*CPU_PortExtendCfgGet)(void *, GSW_CPU_PortExtendCfg_t *);
	/* Command: GSW_CPU_PORT_EXTEND_CFG_SET ; Index: 0x1A */
	GSW_return_t (*CPU_PortExtendCfgSet)(void *, GSW_CPU_PortExtendCfg_t *);
	/* Command: GSW_MONITOR_PORT_CFG_GET ; Index: 0x1B */
	GSW_return_t (*MonitorPortCfgGet)(void *, GSW_monitorPortCfg_t *);
	/* Command: GSW_MONITOR_PORT_CFG_SET ; Index: 0x1C */
	GSW_return_t (*MonitorPortCfgSet)(void *, GSW_monitorPortCfg_t *);
	/* Command: GSW_TIMESTAMP_TIMER_SET ; Index: 0x1D */
	GSW_return_t (*Timestamp_TimerSet)(void *, GSW_TIMESTAMP_Timer_t *);
	/* Command: GSW_TIMESTAMP_TIMER_GET ; Index: 0x1E */
	GSW_return_t (*Timestamp_TimerGet)(void *, GSW_TIMESTAMP_Timer_t *);
	/* Command: GSW_TIMESTAMP_PORT_READ ; Index: 0x1F */
	GSW_return_t (*Timestamp_PortRead)(void *, GSW_TIMESTAMP_PortRead_t *);
	/* Command: GSW_PORT_CFG_GET ; Index: 0x20 */
	GSW_return_t (*PortCfgGet)(void *, GSW_portCfg_t *);
	/* Command: GSW_PORT_CFG_SET ; Index: 0x21 */
	GSW_return_t (*PortCfgSet)(void *, GSW_portCfg_t *);
	/* Command: GSW_MDIO_CFG_GET ; Index: 0x22 */
	GSW_return_t (*MDIO_CfgGet)(void *, GSW_MDIO_cfg_t *);
	/* Command: GSW_MDIO_CFG_SET ; Index: 0x23 */
	GSW_return_t (*MDIO_CfgSet)(void *, GSW_MDIO_cfg_t *);
	/* Command: GSW_MDIO_DATA_READ ; Index: 0x24 */
	GSW_return_t (*MDIO_DataRead)(void *, GSW_MDIO_data_t *);
	/* Command: GSW_MDIO_DATA_WRITE ; Index: 0x25 */
	GSW_return_t (*MDIO_DataWrite)(void *, GSW_MDIO_data_t *);
	/* Command: GSW_MMD_DATA_READ ; Index: 0x26 */
	GSW_return_t (*MmdDataRead)(void *, GSW_MMD_data_t *);
	/* Command: GSW_MMD_DATA_WRITE ; Index: 0x27 */
	GSW_return_t (*MmdDataWrite)(void *, GSW_MMD_data_t *);
};

struct vlan_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_VLAN_MEMBER_INIT ; Index: 0x01 */
	GSW_return_t (*VLAN_Member_Init)(void *, GSW_VLAN_memberInit_t *);
	/* Command: GSW_VLAN_ID_CREATE ; Index: 0x02 */
	GSW_return_t (*VLAN_IdCreate)(void *, GSW_VLAN_IdCreate_t *);
	/* Command: GSW_VLAN_ID_DELETE ; Index: 0x03 */
	GSW_return_t (*VLAN_IdDelete)(void *, GSW_VLAN_IdDelete_t *);
	/* Command: GSW_VLAN_ID_GET ; Index: 0x04 */
	GSW_return_t (*VLAN_IdGet)(void *, GSW_VLAN_IdGet_t *);
	/* Command: GSW_VLAN_PORT_CFG_GET ; Index: 0x05 */
	GSW_return_t (*VLAN_PortCfgGet)(void *, GSW_VLAN_portCfg_t *);
	/* Command: GSW_VLAN_PORT_CFG_SET ; Index: 0x06 */
	GSW_return_t (*VLAN_PortCfgSet)(void *, GSW_VLAN_portCfg_t *);
	/* Command: GSW_VLAN_PORT_MEMBER_ADD ; Index: 0x07 */
	GSW_return_t (*VLAN_PortMemberAdd)(void *, GSW_VLAN_portMemberAdd_t *);
	/* Command: GSW_VLAN_PORT_MEMBER_READ ; Index: 0x08 */
	GSW_return_t (*VLAN_PortMemberRead)(void *, GSW_VLAN_portMemberRead_t *);
	/* Command: GSW_VLAN_PORT_MEMBER_REMOVE ; Index: 0x09 */
	GSW_return_t (*VLAN_PortMemberRemove)(void *, GSW_VLAN_portMemberRemove_t *);
	/* Command: GSW_VLAN_RESERVED_ADD ; Index: 0x0A */
	GSW_return_t (*VLAN_ReservedAdd)(void *, GSW_VLAN_reserved_t *);
	/* Command: GSW_VLAN_RESERVED_REMOVE ; Index: 0x0B */
	GSW_return_t (*VLAN_ReservedRemove)(void *, GSW_VLAN_reserved_t *);
	/* Command: GSW_PCE_EG_VLAN_CFG_SET ; Index: 0x0C */
	GSW_return_t (*VLAN_PCE_EG_CfgSet)(void *, GSW_PCE_EgVLAN_Cfg_t *);
	/* Command: GSW_PCE_EG_VLAN_CFG_GET ; Index: 0x0D */
	GSW_return_t (*VLAN_PCE_EG_CfgGet)(void *, GSW_PCE_EgVLAN_Cfg_t *);
	/* Command: GSW_PCE_EG_VLAN_ENTRY_WRITE ; Index: 0x0E */
	GSW_return_t (*VLAN_PCE_EG_EntryWrite)(void *, GSW_PCE_EgVLAN_Entry_t *);
	/* Command: GSW_PCE_EG_VLAN_ENTRY_READ ; Index: 0x0F */
	GSW_return_t (*VLAN_PCE_EG_EntryRead)(void *, GSW_PCE_EgVLAN_Entry_t *);
	/* Command: GSW_SVLAN_CFG_GET ; Index: 0x10 */
	GSW_return_t (*SVLAN_CfgGet)(void *, GSW_SVLAN_cfg_t *);
	/* Command: GSW_SVLAN_CFG_SET ; Index: 0x11 */
	GSW_return_t (*SVLAN_CfgSet)(void *, GSW_SVLAN_cfg_t *);
	/* Command: GSW_SVLAN_PORT_CFG_GET ; Index: 0x12 */
	GSW_return_t (*SVLAN_PortCfgGet)(void *, GSW_SVLAN_portCfg_t *);
	/* Command: GSW_SVLAN_PORT_CFG_SET ; Index: 0x13 */
	GSW_return_t (*SVLAN_PortCfgSet)(void *, GSW_SVLAN_portCfg_t *);
};

/*PMAC operation*/
struct pmac_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_PMAC_COUNT_GET ; Index: 0x01 */
	int (*Pmac_CountGet)(void *, GSW_PMAC_Cnt_t *);
	/* Command: GSW_PMAC_GLBL_CFG_SET ; Index: 0x02 */
	int (*Pmac_Gbl_CfgSet)(void *, GSW_PMAC_Glbl_Cfg_t *);
	/* Command: GSW_PMAC_GLBL_CFG_GET ; Index: 0x03 */
	int (*Pmac_Gbl_CfgGet)(void *, GSW_PMAC_Glbl_Cfg_t *);
	/* Command: GSW_PMAC_BM_CFG_SET ; Index: 0x04 */
	int (*Pmac_Bm_CfgSet)(void *, GSW_PMAC_BM_Cfg_t *);
	/* Command: GSW_PMAC_BM_CFG_GET ; Index: 0x05 */
	int (*Pmac_Bm_CfgGet)(void *, GSW_PMAC_BM_Cfg_t *);
	/* Command: GSW_PMAC_IG_CFG_SET ; Index: 0x06 */
	int (*Pmac_Ig_CfgSet)(void *, GSW_PMAC_Ig_Cfg_t *);
	/* Command: GSW_PMAC_IG_CFG_GET ; Index: 0x07 */
	int (*Pmac_Ig_CfgGet)(void *, GSW_PMAC_Ig_Cfg_t *);
	/* Command: GSW_PMAC_EG_CFG_SET ; Index: 0x08 */
	int (*Pmac_Eg_CfgSet)(void *, GSW_PMAC_Eg_Cfg_t *);
	/* Command: GSW_PMAC_EG_CFG_GET ; Index: 0x09 */
	int (*Pmac_Eg_CfgGet)(void *, GSW_PMAC_Eg_Cfg_t *);
};



/*PAE operation*/
struct pae_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_ROUTE_ENTRY_ADD ; Index: 0x01 */
	int (*ROUTE_SessionEntryAdd)(void *, GSW_ROUTE_Entry_t *);
	/* Command: GSW_ROUTE_ENTRY_DELETE ; Index: 0x02 */
	int (*ROUTE_SessionEntryDel)(void *, GSW_ROUTE_Entry_t *);
	/* Command: GSW_ROUTE_ENTRY_READ ; Index: 0x03 */
	int (*ROUTE_SessionEntryRead)(void *, GSW_ROUTE_Entry_t *);
	/* Command: GSW_ROUTE_TUNNEL_ENTRY_ADD ; Index: 0x04 */
	int (*ROUTE_TunnelEntryAdd)(void *, GSW_ROUTE_Tunnel_Entry_t *);
	/* Command: GSW_ROUTE_TUNNEL_ENTRY_READ ; Index: 0x05 */
	int (*ROUTE_TunnelEntryRead)(void *, GSW_ROUTE_Tunnel_Entry_t *);
	/* Command: GSW_ROUTE_TUNNEL_ENTRY_DELETE ; Index: 0x06 */
	int (*ROUTE_TunnelEntryDel)(void *, GSW_ROUTE_Tunnel_Entry_t *);
	/* Command: GSW_ROUTE_L2NAT_CFG_WRITE ; Index: 0x07 */
	int (*ROUTE_L2NATCfgWrite)(void *, GSW_ROUTE_EgPort_L2NAT_Cfg_t *);
	/* Command: GSW_ROUTE_L2NAT_CFG_READ ; Index: 0x08 */
	int (*ROUTE_L2NATCfgRead)(void *, GSW_ROUTE_EgPort_L2NAT_Cfg_t *);
	/* Command: GSW_ROUTE_SESSION_HIT_OP ; Index: 0x09 */
	int (*ROUTE_SessHitOp)(void *, GSW_ROUTE_Session_Hit_t *);
	/* Command: GSW_ROUTE_SESSION_DEST_MOD ; Index: 0x0A */
	int (*ROUTE_SessDestModify)(void *, GSW_ROUTE_Session_Dest_t *);
};


struct debug_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_DEBUG_CTPTABLE_STATUS ; Index: 0x01 */
	GSW_return_t (*DEBUG_CtpTableStatus)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_BRDGPORTTABLE_STATUS ; Index: 0x02 */
	GSW_return_t (*DEBUG_BrgPortTableStatus)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_BRDGTABLE_STATUS ; Index: 0x03 */
	GSW_return_t (*DEBUG_BrgTableStatus)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_EXVLANTABLE_STATUS ; Index: 0x04 */
	GSW_return_t (*DEBUG_ExvlanTableStatus)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_VLANFILTERTABLE_STATUS ; Index: 0x05 */
	GSW_return_t (*DEBUG_VlanFilterTableStatus)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_METERTABLE_STATUS ; Index: 0x06 */
	GSW_return_t (*DEBUG_MeterTableStatus)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_DSCP2PCPTABLE_STATUS ; Index: 0x07 */
	GSW_return_t (*DEBUG_Dscp2PcpTableStatus)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_PMAPPER_STATUS ; Index: 0x08 */
	GSW_return_t (*DEBUG_PmapperTableStatus)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_PMAC_EG ; Index: 0x09 */
	GSW_return_t (*DEBUG_PmacEg)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_PMAC_IG ; Index: 0x0A */
	GSW_return_t (*DEBUG_PmacIg)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_DEF_QMAP ; Index: 0x0B */
	GSW_return_t (*DEBUG_Def_PceQmap)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_DEF_BYP_QMAP ; Index: 0x0C */
	GSW_return_t (*DEBUG_Def_PceBypQmap)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_PMAC_BP ; Index: 0x0D */
	GSW_return_t (*DEBUG_PmacBp)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_LP_STATISTICS ; Index: 0x0E */
	GSW_return_t (*DEBUG_GetLpStatistics)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_CTP_STATISTICS ; Index: 0x0F */
	GSW_return_t (*DEBUG_GetCtpStatistics)(void *, GSW_debug_t *);
	/* Command: GSW_XGMAC_CFG ; Index: 0x10 */
	GSW_return_t (*Xgmac)(void *, GSW_MAC_Cli_t *);
	/* Command: GSW_GSWSS_CFG ; Index: 0x11 */
	GSW_return_t (*Gswss)(void *, GSW_MAC_Cli_t *);
	/* Command: GSW_LMAC_CFG ; Index: 0x12 */
	GSW_return_t (*Lmac)(void *, GSW_MAC_Cli_t *);
	/* Command: GSW_MACSEC_CFG ; Index: 0x13 */
	GSW_return_t (*Macsec)(void *, GSW_MAC_cfg_t *);
	/* Command: GSW_PMACBR_CFG ; Index: 0x14 */
	GSW_return_t (*Pmacbr)(void *, GSW_MAC_cfg_t *);
	/* Command: GSW_DUMP_MEM ; Index: 0x15 */
	GSW_return_t (*DumpMem)(void *, GSW_table_t *);
	/* Command: GSW_DEBUG_PRINT_PCEIRQ_LIST ; Index: 0x15 */
	GSW_return_t (*DEBUG_PrintPceIrqList)(void *);
	/* Command: GSW_DEBUG_RMON_PORT_GET ; Index: 0x16 */
	GSW_return_t (*DEBUG_RMON_Port_Get)(void *, GSW_Debug_RMON_Port_cnt_t *);
	/* Command: GSW_DEBUG_TUNNELTEMP_STATUS ; Index: 0x18 */
	GSW_return_t (*DEBUG_TunnelTempTableStatus)(void *, GSW_debug_t *);
	/* Command: GSW_DEBUG_TFLOWTABLE_STATUS ; Index: 0x19 */
	GSW_return_t (*debug_tflowtablestatus)(void *, GSW_debug_t *);
};

struct irq_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_IRQ_REGISTER ; Index: 0x01 */
	GSW_return_t (*IRQ_Register)(void *, GSW_Irq_Op_t *);
	/* Command: GSW_IRQ_UNREGISTER ; Index: 0x02 */
	GSW_return_t (*IRQ_UnRegister)(void *, GSW_Irq_Op_t *);
	/* Command: GSW_IRQ_ENABLE ; Index: 0x03 */
	GSW_return_t (*IRQ_Enable)(void *, GSW_Irq_Op_t *);
	/* Command: GSW_IRQ_DISABLE ; Index: 0x04 */
	GSW_return_t (*IRQ_Disable)(void *, GSW_Irq_Op_t *);
};

struct pbb_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_PBB_TUNNEL_TEMPLATE_ALLOC ; Index: 0x01 */
	GSW_return_t (*PBB_TemplateAlloc)(void *, GSW_PBB_Tunnel_Template_Config_t *);
	/* Command: GSW_PBB_TUNNEL_TEMPLATE_FREE ; Index: 0x02 */
	GSW_return_t (*PBB_TemplateFree)(void *, GSW_PBB_Tunnel_Template_Config_t *);
	/* Command: GSW_PBB_TUNNEL_TEMPLATE_SET ; Index: 0x03 */
	GSW_return_t (*PBB_TemplateSet)(void *, GSW_PBB_Tunnel_Template_Config_t *);
	/* Command: GSW_PBB_TUNNEL_TEMPLATE_GET ; Index: 0x04 */
	GSW_return_t (*PBB_TemplateGet)(void *, GSW_PBB_Tunnel_Template_Config_t *);
};

struct gpid_ops {
	/* Command: (NULL); Index: 0x00 */
	GSW_return_t (*null)(void);
	/* Command: GSW_LPID_TO_GPID_ASSIGNMENT_SET ; Index: 0x01 */
	GSW_return_t (*LpidToGpid_AssignmentSet)(void *, GSW_LPID_to_GPID_Assignment_t *);
	/* Command: GSW_LPID_TO_GPID_ASSIGNMENT_GET ; Index: 0x02 */
	GSW_return_t (*LpidToGpid_AssignmentGet)(void *, GSW_LPID_to_GPID_Assignment_t *);
	/* Command: GSW_GPID_TO_LPID_ASSIGNMENT_SET ; Index: 0x03 */
	GSW_return_t (*GpidToLpid_AssignmentSet)(void *, GSW_GPID_to_LPID_Assignment_t *);
	/* Command: GSW_GPID_TO_LPID_ASSIGNMENT_GET ; Index: 0x04 */
	GSW_return_t (*GpidToLpid_AssignmentGet)(void *, GSW_GPID_to_LPID_Assignment_t *);
};

struct core_ops {
	/**Switch Opertations**/
	/*RMON Counters GET opertations for Port/Meter/TFlow/Redirect
	  RMON Counters Mode SET opertations
	  RMON Counters CLEAR operations
	  NOTE: Applicable for GSWIP 3.1 and GSWIP 3.0*/

	struct rmon_ops 			gsw_rmon_ops;

	/*Switch MAC table ADD/READ/QUERY/CLEAR/REMOVE operations
	  NOTE: Applicable for GSWIP 3.1/GSWIP 3.0/GSWIP 2.2

	  Switch Defaul MAC filter SET/GET operations
	  NOTE: Applicable for GSWIP 3.1 only*/

	struct swmac_ops 			gsw_swmac_ops;

	/*Extended VLAN Block ALLOC/SET/GET/FREE operations
	  NOTE: Applicable for GSWIP 3.1 only*/

	struct extvlan_ops		gsw_extvlan_ops;

	/*VLAN Filter Block ALLOC/SET/GET/FREE operations
	  NOTE: Applicable for GSWIP 3.1 only*/

	struct vlanfilter_ops 	gsw_vlanfilter_ops;

	/*CTP Port ALLOC/FREE operations
	  CTP Port Configuration SET/GET/RESET operations
	  NOTE: Applicable for GSWIP 3.1 only*/

	struct ctp_ops			gsw_ctp_ops;

	/*Bridge Port ALLOC/FREE operations
	  Bridge Port Configuration SET/GET operations
	  NOTE: Applicable for GSWIP 3.1 only*/

	struct brdgport_ops		gsw_brdgport_ops;

	/*Bridge ALLOC/FREE operations
	  Bridge Configuration SET/GET operations
	  NOTE: Applicable for GSWIP 3.1 only*/

	struct brdg_ops			gsw_brdg_ops;

	/*TFLOW PCE Rule Read/Write/Delete operations
	  NOTE: Applicable for GSWIP 3.1/GSWIP 3.0/GSWIP 2.2*/

	struct tflow_ops			gsw_tflow_ops;

	/*QOS related operations
	  NOTE: Applicable for GSWIP 3.1/GSWIP 3.0/GSWIP 2.2*/

	struct qos_ops			gsw_qos_ops;

	/*Spanning Tree Protocol opertaions
	  NOTE: Applicable for GSWIP 3.1/GSWIP 3.0*/

	struct stp_ops			gsw_stp_ops;

	/*Extensible Authentication Protocol opertaions
	  NOTE: Applicable for GSWIP 3.1/GSWIP 3.0*/

	struct eapol_ops			gsw_8021x_ops;

	/*Multicast related opertaions
	  NOTE: Applicable for GSWIP 3.1/GSWIP 3.0*/

	struct multicast_ops		gsw_multicast_ops;

	/*Port Trunking opertaions
	  NOTE: Applicable for GSWIP 3.1/GSWIP 3.0*/

	struct trunking_ops		gsw_trunking_ops;

	/*Wake On Lan opertaions
	  NOTE: Applicable for GSWIP 3.1/GSWIP 3.0*/

	struct wol_ops			gsw_wol_ops;

	/*VLAN/SVLAN related opertaions
	 NOTE: Applicable Only for GSWIP 3.0 and below*/

	struct vlan_ops			gsw_vlan_ops;

	/*PMAC related opertaions
	 NOTE: Applicable for GSWIP 3.1/GSWIP 3.0*/

	struct pmac_ops			gsw_pmac_ops;

	/*PAE related opertaions*/
	struct pae_ops			gsw_pae_ops;

	/*Common Purpose Switch operations like
	Switch REG read/write
	Irq mask set/get/status clear
	Switch enable/disable/reset/init
	Port monitor set/get
	Port link get/set
	Port redirect
	Time stamp,version get
	Switch capablity get ..etc
	 NOTE: Applicable for GSWIP 3.1/GSWIP 3.0/GSWIP 2.2*/

	struct common_ops			gsw_common_ops;

	/*Debug Purpose for GSWIP 3.1*/
	struct debug_ops			gsw_debug_ops;

	struct irq_ops			gsw_irq_ops;

	/*PBB Tunnel template configuration ops
	 applicable for GSWIP-3.2*/
	struct pbb_ops gsw_pbb_ops;

	/*logical port to global port assignment
	  global port to logical port assignment
	  configuration ops
	  applicable for GSWIP-3.2*/
	struct gpid_ops gsw_gpid_ops;
};


#endif
