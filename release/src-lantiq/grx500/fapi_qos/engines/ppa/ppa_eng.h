#ifndef _SW_HDR_H
#define _SW_HDR_H

#define PPA_Q_CAP_RED			SET_PARAM(Q_CAP_RED, 1)
#define PPA_Q_CAP_WRED			SET_PARAM(Q_CAP_WRED, 1)
#define PPA_Q_CAP_DT			SET_PARAM(Q_CAP_DT, 1)
#define PPA_Q_CAP_CODEL			SET_PARAM(Q_CAP_CODEL, 0)
#define PPA_Q_CAP_SP			SET_PARAM(Q_CAP_SP, 1)
#define PPA_Q_CAP_WFQ			SET_PARAM(Q_CAP_WFQ, 1)
#define PPA_Q_CAP_WRR			SET_PARAM(Q_CAP_WRR, 0)
#define PPA_Q_CAP_LEN			SET_PARAM(Q_CAP_LEN, 1)
#define PPA_Q_CAP_SUBQ			SET_PARAM(Q_CAP_SUBQ, 0)
#define PPA_Q_CAP_SHAPE			SET_PARAM(Q_CAP_SHAPE, 1)
#define PPA_Q_CAP_COL_BLIND		SET_PARAM(Q_CAP_COL_BLIND, 1)
#define PPA_Q_CAP_TR_TCM		SET_PARAM(Q_CAP_TR_TCM, 1)
#define PPA_Q_CAP_TR_TCM_RFC4115	SET_PARAM(Q_CAP_TR_TCM_RFC4115, 0)
#define PPA_Q_CAP_LOOSE_COL_BLIND	SET_PARAM(Q_CAP_LOOSE_COL_BLIND, 0)
#define PPA_PORT_CAP_SHAPE		SET_PARAM(PORT_CAP_SHAPE, 1)
#define PPA_Q_CAP_ATM_IF		SET_PARAM(Q_CAP_ATM_IF, 1)
#define PPA_Q_CAP_PTM_IF		SET_PARAM(Q_CAP_PTM_IF, 1)
#define PPA_Q_CAP_ETHL_IF		SET_PARAM(Q_CAP_ETHL_IF, 1)
#define PPA_Q_CAP_ETHW_IF		SET_PARAM(Q_CAP_ETHW_IF, 1)
#define PPA_Q_CAP_LTE_IF		SET_PARAM(Q_CAP_LTE_IF, 1)
#define PPA_Q_CAP_INGRESS		SET_PARAM(Q_CAP_INGRESS, 1)
#define PPA_Q_CAP_WMM			SET_PARAM(Q_CAP_WMM, 1)
#define PPA_Q_CAP_DOS			SET_PARAM(Q_CAP_DOS, 1)

#ifdef PLATFORM_LGM
//Flags can be put for all parameters provided in 181 data model
#define PPA_CL_CAP_MAC_ADDR 		SET_PARAM(CL_CAP_MAC_ADDR, 0)
#define PPA_CL_CAP_TX_IF		SET_PARAM(CL_CAP_TX_IF, 0)
#define PPA_CL_CAP_VLAN_ID		SET_PARAM(CL_CAP_ETH_VLAN_ID, 0)
#define PPA_CL_CAP_VLAN_PRIO		SET_PARAM(CL_CAP_ETH_VLAN_PRIO, 0)
#define PPA_CL_CAP_L3_PROTO		SET_PARAM(CL_CAP_L3_PROTO, 0)
#define PPA_CL_CAP_IPv4_ADDR		SET_PARAM(CL_CAP_L3_IPv4_ADDR, 0)
#define PPA_CL_CAP_IPv6_ADDR		SET_PARAM(CL_CAP_L3_IPv6_ADDR, 0)
#define PPA_CL_CAP_IP_DSCP		SET_PARAM(CL_CAP_L3_DSCP, 0)
#define PPA_CL_CAP_L4_PROTO_N_PORT	SET_PARAM(CL_CAP_L4_PROTO_N_PORT, 0)
#define PPA_CL_CAP_L7_PROTO		SET_PARAM(CL_CAP_L7_PROTO, 0)
#define PPA_CL_CAP_DHCP_VEND_EXT	SET_PARAM(CL_CAP_DHCP_VEND_EXT, 0)
#define PPA_CL_CAP_EXCL			SET_PARAM(CL_CAP_EXCL, 0)
#define PPA_CL_CAP_ACT_TC		SET_PARAM(CL_CAP_ACT_TC, 0)
#define PPA_CL_CAP_ACT_DROP		SET_PARAM(CL_CAP_ACT_DROP, 0)
#define PPA_CL_CAP_ACT_DSCP_REMARK	SET_PARAM(CL_CAP_ACT_DSCP_REMARK, 0)
#define PPA_CL_CAP_ACT_VLAN_REMARK	SET_PARAM(CL_CAP_ACT_VLAN_REMARK, 0)
#define PPA_CL_CAP_ACT_FWD		SET_PARAM(CL_CAP_ACT_FWD, 0)
#define PPA_CL_CAP_ACT_POLICE		SET_PARAM(CL_CAP_ACT_POLICE, 0)
#define PPA_CL_CAP_ACT_ACCL		SET_PARAM(CL_CAP_ACT_ACCL, 0)
#define PPA_CL_CAP_ACT_APP		SET_PARAM(CL_CAP_ACT_APP, 0)
#define PPA_CL_CAP_ETHL_IF		SET_PARAM(CL_CAP_ETHL_IF, 0)
#define PPA_CL_CAP_ATM_IF		SET_PARAM(CL_CAP_ATM_IF, 0)
#define PPA_CL_CAP_PTM_IF		SET_PARAM(CL_CAP_PTM_IF, 0)
#define PPA_CL_CAP_ETHW_IF		SET_PARAM(CL_CAP_ETHW_IF, 0)
#define PPA_CL_CAP_LTE_IF		SET_PARAM(CL_CAP_LTE_IF, 0)
#define PPA_CL_CAP_INS			SET_PARAM(CL_CAP_INS, 0)
#define PPA_CL_CAP_ETH_TYPE		SET_PARAM(CL_CAP_ETH_TYPE,0)

#define PPA_CL_CAP_ACT_INGRS_SW		SET_PARAM(CL_CAP_ACT_INGRS_SW,0)
#define PPA_CL_CAP_ACT_INGRS_HW		SET_PARAM(CL_CAP_ACT_INGRS_HW, 0)
#else
//Flags can be put for all parameters provided in 181 data model
#define PPA_CL_CAP_MAC_ADDR 		SET_PARAM(CL_CAP_MAC_ADDR, 1)
#define PPA_CL_CAP_TX_IF		SET_PARAM(CL_CAP_TX_IF, 1)
#define PPA_CL_CAP_VLAN_ID		SET_PARAM(CL_CAP_ETH_VLAN_ID, 1)
#define PPA_CL_CAP_VLAN_PRIO		SET_PARAM(CL_CAP_ETH_VLAN_PRIO, 1)
#define PPA_CL_CAP_L3_PROTO		SET_PARAM(CL_CAP_L3_PROTO, 1)
#define PPA_CL_CAP_IPv4_ADDR		SET_PARAM(CL_CAP_L3_IPv4_ADDR, 1)
#define PPA_CL_CAP_IPv6_ADDR		SET_PARAM(CL_CAP_L3_IPv6_ADDR, 1)
#define PPA_CL_CAP_IP_DSCP		SET_PARAM(CL_CAP_L3_DSCP, 1)
#define PPA_CL_CAP_L4_PROTO_N_PORT	SET_PARAM(CL_CAP_L4_PROTO_N_PORT, 1)
#define PPA_CL_CAP_L7_PROTO		SET_PARAM(CL_CAP_L7_PROTO, 0)
#define PPA_CL_CAP_DHCP_VEND_EXT	SET_PARAM(CL_CAP_DHCP_VEND_EXT, 1)
#define PPA_CL_CAP_EXCL			SET_PARAM(CL_CAP_EXCL, 1)
#define PPA_CL_CAP_ACT_TC		SET_PARAM(CL_CAP_ACT_TC, 1)
#define PPA_CL_CAP_ACT_DROP		SET_PARAM(CL_CAP_ACT_DROP, 1)
#define PPA_CL_CAP_ACT_DSCP_REMARK	SET_PARAM(CL_CAP_ACT_DSCP_REMARK, 0)
#define PPA_CL_CAP_ACT_VLAN_REMARK	SET_PARAM(CL_CAP_ACT_VLAN_REMARK, 1)
#define PPA_CL_CAP_ACT_FWD		SET_PARAM(CL_CAP_ACT_FWD, 1)
#define PPA_CL_CAP_ACT_POLICE		SET_PARAM(CL_CAP_ACT_POLICE, 0)
#define PPA_CL_CAP_ACT_ACCL		SET_PARAM(CL_CAP_ACT_ACCL, 0)
#define PPA_CL_CAP_ACT_APP		SET_PARAM(CL_CAP_ACT_APP, 1)
#define PPA_CL_CAP_ETHL_IF		SET_PARAM(CL_CAP_ETHL_IF, 1)
#define PPA_CL_CAP_ATM_IF		SET_PARAM(CL_CAP_ATM_IF, 1)
#define PPA_CL_CAP_PTM_IF		SET_PARAM(CL_CAP_PTM_IF, 1)
#define PPA_CL_CAP_ETHW_IF		SET_PARAM(CL_CAP_ETHW_IF, 1)
#define PPA_CL_CAP_LTE_IF		SET_PARAM(CL_CAP_LTE_IF, 1)
#define PPA_CL_CAP_INS			SET_PARAM(CL_CAP_INS, 1)
#define PPA_CL_CAP_ETH_TYPE     	SET_PARAM(CL_CAP_ETH_TYPE,1)

#define PPA_CL_CAP_ACT_INGRS_SW     	SET_PARAM(CL_CAP_ACT_INGRS_SW,0)
#define PPA_CL_CAP_ACT_INGRS_HW     	SET_PARAM(CL_CAP_ACT_INGRS_HW,1)
#endif

#define PPA_CL_CAP_PPP_PROTO     	SET_PARAM(CL_CAP_PPP_PROTO, 1)

#define PPA_Q_CAP  (PPA_Q_CAP_RED) | (PPA_Q_CAP_WRED) | (PPA_Q_CAP_DT) | \
	(PPA_Q_CAP_CODEL) | (PPA_Q_CAP_SP) | (PPA_Q_CAP_WFQ) | \
	(PPA_Q_CAP_WRR) | (PPA_Q_CAP_LEN) | (PPA_Q_CAP_SUBQ) | \
	(PPA_Q_CAP_SHAPE) | (PPA_Q_CAP_COL_BLIND) | (PPA_Q_CAP_TR_TCM) | \
	(PPA_Q_CAP_TR_TCM_RFC4115) | (PPA_Q_CAP_LOOSE_COL_BLIND) | (PPA_PORT_CAP_SHAPE) | \
	(PPA_Q_CAP_ATM_IF) | (PPA_Q_CAP_PTM_IF) | (PPA_Q_CAP_ETHL_IF) | (Q_CAP_ETHW_IF) | (PPA_Q_CAP_LTE_IF) | \
	(PPA_Q_CAP_INGRESS) | (PPA_Q_CAP_WMM) | (PPA_Q_CAP_DOS)

#define PPA_CL_CAP (PPA_CL_CAP_MAC_ADDR) | (PPA_CL_CAP_VLAN_ID) | (PPA_CL_CAP_VLAN_PRIO) | \
	(PPA_CL_CAP_L3_PROTO) | (PPA_CL_CAP_IPv4_ADDR) | (PPA_CL_CAP_IPv6_ADDR) | \
	(PPA_CL_CAP_IP_DSCP) | (PPA_CL_CAP_L4_PROTO_N_PORT) | (PPA_CL_CAP_L7_PROTO) | \
	(PPA_CL_CAP_DHCP_VEND_EXT) | (PPA_CL_CAP_EXCL) | \
	(PPA_CL_CAP_ACT_TC) |  (PPA_CL_CAP_ACT_DROP) | (PPA_CL_CAP_ACT_DSCP_REMARK) | \
	(PPA_CL_CAP_ACT_VLAN_REMARK) | (PPA_CL_CAP_ACT_FWD) | (PPA_CL_CAP_ACT_POLICE) | (PPA_CL_CAP_ETHL_IF) | (PPA_CL_CAP_ACT_APP) | \
	(PPA_CL_CAP_ACT_APP) | (PPA_CL_CAP_ATM_IF) | (PPA_CL_CAP_PTM_IF) | (PPA_CL_CAP_ETHW_IF) | (PPA_CL_CAP_LTE_IF) | (PPA_CL_CAP_INS) | \
	(PPA_CL_CAP_TX_IF)

#define PPA_CL_EXT_CAP (PPA_CL_CAP_TX_IF) | (PPA_CL_CAP_ETH_TYPE) | \
	(PPA_CL_CAP_ACT_INGRS_SW) | (PPA_CL_CAP_ACT_INGRS_HW) | \
	(PPA_CL_CAP_PPP_PROTO)

#define PPA_MAX_Q 16
#define PPA_MAX_Q_LEN 100
#define PPA_MAX_CL 24

#define WMM_QOS_CFG	0x00000001
#define DOS_QOS_CFG	0x00000002
#define QOS_DEF_Q_FILE	PLATFORM_XML"/usr/sbin/qosal_plat_cfg.xml"
#define ETH0_5_DIR_PATH		"/sys/class/net/eth0_5/address"

uint32_t us_rate;
uint8_t wan_port;

#define gacPrio2QueMap "prio 0 queue 0 prio 1 queue 1 prio 2 queue 2 prio 3 queue 3 prio 4 queue 4 prio 5 queue 5 prio 6 queue 6 prio 7 queue 7"
#define gacPrio2QueMapReverse "prio 0 queue 7 prio 1 queue 6 prio 2 queue 5 prio 3 queue 4 prio 4 queue 3 prio 5 queue 2 prio 6 queue 1 prio 7 queue 0"


int32_t ppa_eng_reg(qos_engine_t *eng);
int32_t ppa_eng_init(qos_module_type_t mtype);
int32_t ppa_eng_fini(qos_module_type_t mtype);
int32_t ppa_eng_queue_modify(char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t *qcfg);
int32_t ppa_eng_queue_add(char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t *qcfg);
int32_t ppa_eng_queue_delete(char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t *qcfg);
int32_t ppa_eng_get_queue_stats (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, char *qname, qos_queue_stats_t* qstats);
int32_t ppa_eng_port_set (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_shaper_t  *shaper, uint32_t priority, uint32_t weight, uint32_t flag);
int32_t ppa_eng_class_add(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags);
int32_t ppa_eng_class_delete(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags);
int32_t ppa_eng_class_modify(iftype_t iftype, ifinggrp_t ifinggrp, uint32_t old_order, qos_class_cfg_t *old_clcfg, uint32_t order, qos_class_cfg_t *clcfg, uint32_t flags);
int32_t init_wmm_queue_cfg(uint32_t oper_flag);
int32_t dos_q_cfg(uint32_t oper_flag);
int32_t dos_qos_cfg(uint32_t oper_flag);
int32_t dos_meter_cfg(uint32_t oper_flag);
int32_t dos_cl_cfg(uint32_t oper_flag);

int32_t ppa_add_def_wmm_dscp_class(void);
int32_t ppa_del_def_wmm_dscp_class(void);
int32_t xml_parse_ingress_groups(void);
int32_t ppa_eng_meter_set(qos_meter_t *meter_cfg);

#endif
