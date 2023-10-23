/******************************************************************************
**
** FILE NAME    : ppa_drv_wrapper.h
** PROJECT      : PPA
** MODULES      : PPA Wrapper for PPE Driver API
**
** DATE         : 14 Mar 2011
** AUTHOR       : Shao Guohua
** DESCRIPTION  : PPA Wrapper for PPE Driver API
** COPYRIGHT    : Copyright (c) 2017 Intel Corporation
** Copyright (c) 2011 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
** HISTORY
** $Date        $Author         $Comment
** 14 MAR 2011  Shao Guohua       Initiate Version
** 05 JUL 2017  Kamal eradath    merged ppe_drv_wrapper.h and ppa_datapath_wrapper.h
*******************************************************************************/
#ifndef PPA_DRV_WRAPPER_2011_03_14
#define PPA_DRV_WRAPPER_2011_03_14
extern uint32_t ppa_drv_dp_sb_addr_to_fpi_addr_convert(PPA_FPI_ADDR *a, uint32_t flag);
extern uint32_t ppa_drv_get_firmware_id(PPA_VERSION *v, uint32_t flag);
extern uint32_t ppa_drv_get_number_of_phys_port(PPA_COUNT_CFG *count, uint32_t flag);
extern uint32_t ppa_drv_get_phys_port_info(PPE_IFINFO *info, uint32_t flag);
extern uint32_t ppa_drv_get_max_entries(PPA_MAX_ENTRY_INFO *entry, uint32_t flag);
extern uint32_t ppa_drv_set_route_cfg(PPA_ROUTING_CFG *cfg, uint32_t flag);
extern uint32_t ppa_drv_set_bridging_cfg(PPA_BRDG_CFG *cfg, uint32_t flag);
extern uint32_t ppa_drv_get_acc_mode(PPA_ACC_ENABLE *acc_enable, uint32_t flag);
extern uint32_t ppa_drv_set_acc_mode(PPA_ACC_ENABLE *acc_enable, uint32_t flag);
extern uint32_t ppa_drv_set_bridge_if_vlan_config(PPA_BRDG_VLAN_CFG *cfg, uint32_t flag);
extern uint32_t ppa_drv_get_bridge_if_vlan_config(PPA_BRDG_VLAN_CFG *cfg, uint32_t flag);
extern uint32_t ppa_drv_add_vlan_map(PPA_BRDG_VLAN_FILTER_MAP *filter, uint32_t flag);
extern uint32_t ppa_drv_del_vlan_map(PPA_BRDG_VLAN_FILTER_MAP *filter, uint32_t flag);
extern uint32_t ppa_drv_get_vlan_map(PPA_BRDG_VLAN_FILTER_MAP *filter , uint32_t flag);
extern uint32_t ppa_drv_del_all_vlan_map(uint32_t flag);
extern uint32_t ppa_drv_get_max_vfilter_entries(PPA_VFILTER_COUNT_CFG *count, uint32_t flag);
extern uint32_t ppa_drv_is_ipv6_enabled(uint32_t flag);
#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
extern uint32_t ppa_drv_set_mib_mode(PPA_MIB_MODE_ENABLE *cfg, uint32_t);
extern uint32_t ppa_drv_get_mib_mode(PPA_MIB_MODE_ENABLE *cfg);
#endif
extern uint32_t ppa_drv_add_bridging_entry(PPA_BR_MAC_INFO *entry, uint32_t flag);
extern uint32_t ppa_drv_del_bridging_entry(PPA_BR_MAC_INFO *entry, uint32_t flag);
#ifdef CONFIG_PPA_QOS
extern uint32_t ppa_drv_get_qos_status(PPA_QOS_STATUS *status, uint32_t flag);
extern uint32_t ppa_drv_get_qos_qnum(PPA_QOS_COUNT_CFG *count, uint32_t flag);
extern uint32_t ppa_drv_get_qos_mib(PPA_QOS_MIB_INFO *mib, uint32_t flag);
/*#ifdef CONFIG_PPA_QOS_RATE_SHAPING*/
extern uint32_t ppa_drv_set_ctrl_qos_rate(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag);
extern uint32_t ppa_drv_get_ctrl_qos_rate(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag);
extern uint32_t ppa_drv_set_qos_rate(PPA_QOS_RATE_SHAPING_CFG *cfg , uint32_t flag);
extern uint32_t ppa_drv_get_qos_rate(PPA_QOS_RATE_SHAPING_CFG *cfg , uint32_t flag);
extern uint32_t ppa_drv_reset_qos_rate(PPA_QOS_RATE_SHAPING_CFG *cfg , uint32_t flag);
extern uint32_t ppa_drv_init_qos_rate(uint32_t flag);
/*#endif*/
/*#ifdef CONFIG_PPA_QOS_WFQ*/
extern uint32_t ppa_drv_set_ctrl_qos_wfq(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag);
extern uint32_t ppa_drv_get_ctrl_qos_wfq(PPA_QOS_ENABLE_CFG *enable_cfg, uint32_t flag);
extern uint32_t ppa_drv_set_qos_wfq(PPA_QOS_WFQ_CFG *cfg, uint32_t flag);
extern uint32_t ppa_drv_get_qos_wfq(PPA_QOS_WFQ_CFG *cfg, uint32_t flag);
extern uint32_t ppa_drv_reset_qos_wfq(PPA_QOS_WFQ_CFG *cfg, uint32_t flag);
extern uint32_t ppa_drv_init_qos_wfq(uint32_t flag);
/*#endif*/
#endif
extern uint32_t ppa_drv_get_dsl_mib(PPA_DSL_QUEUE_MIB *mib, uint32_t flag);
extern uint32_t ppa_drv_get_ports_mib(PPA_PORT_MIB *mib, uint32_t flag);
extern uint32_t ppa_drv_get_itf_mib(PPE_ITF_MIB_INFO *mib, uint32_t flag);
extern uint32_t ppa_drv_test_and_clear_bridging_hit_stat(PPA_BR_MAC_INFO *entry, uint32_t flag);
extern uint32_t ppa_set_wan_itf(PPA_WANITF_CFG *cfg, uint32_t flag);
extern uint32_t ppa_drv_hal_init(PPA_HAL_INIT_CFG *cfg, uint32_t flag);
extern uint32_t ppa_drv_hal_exit(uint32_t flag);
extern uint32_t ppa_get_session_hash(PPA_SESSION_HASH *cfg, uint32_t flag);
#if defined(MBR_CONFIG) && MBR_CONFIG
extern uint32_t ppa_drv_set_qos_shaper(PPA_QOS_RATE_SHAPING_CFG *cfg , uint32_t flag);
extern uint32_t ppa_drv_get_qos_shaper(PPA_QOS_RATE_SHAPING_CFG *cfg , uint32_t flag);
#endif
#if IS_ENABLED(CONFIG_SOC_GRX500) 
extern int ppa_drv_directpath_register(PPA_SUBIF *subif, PPA_NETIF *netif, PPA_DIRECTPATH_CB *pDirectpathCb, int32_t *index,  uint32_t flags);
#endif
#if IS_ENABLED(CONFIG_SOC_GRX500) 
extern int ppa_drv_directpath_send(PPA_SUBIF *subif, PPA_SKBUF *skb, int32_t len, uint32_t flags);
#else
extern int ppa_drv_directpath_send(uint32_t if_id, PPA_SKBUF *skb, int32_t len, uint32_t flags);
#endif
extern int ppa_drv_directpath_rx_stop(uint32_t if_id, uint32_t flags);
extern int ppa_drv_directpath_rx_start(uint32_t if_id, uint32_t flags);
extern int ppa_drv_get_dslwan_qid_with_vcc(struct atm_vcc *vcc);
extern int ppa_drv_get_netif_qid_with_pkt(struct sk_buff *skb, void *arg, int is_atm_vcc);
extern int ppa_drv_ppe_clk_change(unsigned int arg, unsigned int flags);
extern int ppa_drv_ppe_pwm_change(unsigned int arg, unsigned int flags);   /*  arg - parameter, flags - 1: clock gating on/off, 2: power gating on/off*/
extern struct ppe_directpath_data *ppa_drv_g_ppe_directpath_data;
#endif /*end of PPA_DRV_WRAPPER_2011_03_14*/
