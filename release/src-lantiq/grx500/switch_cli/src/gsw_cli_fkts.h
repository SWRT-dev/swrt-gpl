/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#ifndef _CMD_DECLARE_H
#define _CMD_DECLARE_H
int gsw_8021x_eapol_rule_set(int argc, char *argv[], void *fd, int numPar);
int gsw_8021x_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_cpu_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_cpu_port_extend_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_disable(int argc, char *argv[], void *fd, int numPar);
int gsw_enable(int argc, char *argv[], void *fd, int numPar);
int gsw_hw_init(int argc, char *argv[], void *fd, int numPar);
int gsw_mac_table_clear(int argc, char *argv[], void *fd, int numPar);
int gsw_mac_table_entry_add(int argc, char *argv[], void *fd, int numPar);
int gsw_mac_table_entry_remove(int argc, char *argv[], void *fd, int numPar);
int gsw_mdio_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_mdio_data_write(int argc, char *argv[], void *fd, int numPar);
int gsw_mmd_data_write(int argc, char *argv[], void *fd, int numPar);
int gsw_monitor_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_multicast_router_port_add(int argc, char *argv[], void *fd, int numPar);
int gsw_multicast_router_port_remove(int argc, char *argv[], void *fd, int numPar);
int gsw_multicast_snoop_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_multicast_table_entry_add(int argc, char *argv[], void *fd, int numPar);
int gsw_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_port_link_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_port_redirect_set(int argc, char *argv[], void *fd, int numPar);
int gsw_port_rgmii_clk_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_class_dscp_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_dscp_class_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_dscp_drop_precedence_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_meter_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_meter_port_assign(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_meter_port_deassign(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_pcp_class_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_port_remarking_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_queue_port_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_scheduler_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_shaper_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_shaper_queue_assign(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_shaper_queue_deassign(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_storm_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_wred_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_wred_queue_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_rmon_clear(int argc, char *argv[], void *fd, int numPar);
int gsw_stp_bpdu_rule_set(int argc, char *argv[], void *fd, int numPar);
int gsw_stp_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_id_create(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_id_delete(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_port_member_add(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_port_member_remove(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_member_init(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_reserved_add(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_reserved_remove(int argc, char *argv[], void *fd, int numPar);
int gsw_wol_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_wol_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_irq_mask_set(int argc, char *argv[], void *fd, int numPar);
int gsw_irq_status_clear(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_rule_delete(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_rule_write(int argc, char *argv[], void *fd, int numPar);
int gsw_register_set(int argc, char *argv[], void *fd, int numPar);
int gsw_reset(int argc, char *argv[], void *fd, int numPar);
int gsw_8021x_eapol_rule_get(int argc, char *argv[], void *fd, int numPar);
int gsw_8021x_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_cap_get(int argc, char *argv[], void *fd, int numPar);
int gsw_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_cpu_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_cpu_port_extend_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_mac_table_entry_query(int argc, char *argv[], void *fd, int numPar);
int gsw_mac_table_entry_read(int argc, char *argv[], void *fd, int numPar);
int gsw_mdio_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_mdio_data_read(int argc, char *argv[], void *fd, int numPar);
int gsw_mmd_data_read(int argc, char *argv[], void *fd, int numPar);
int gsw_monitor_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_multicast_router_port_read(int argc, char *argv[], void *fd, int numPar);
int gsw_multicast_snoop_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_multicast_table_entry_read(int argc, char *argv[], void *fd, int numPar);
int gsw_multicast_table_entry_remove(int argc, char *argv[], void *fd, int numPar);
int gsw_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_port_link_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_port_phy_addr_get(int argc, char *argv[], void *fd, int numPar);
int gsw_port_phy_query(int argc, char *argv[], void *fd, int numPar);
int gsw_port_redirect_get(int argc, char *argv[], void *fd, int numPar);
int gsw_port_rgmii_clk_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_class_dscp_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_class_pcp_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_class_pcp_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_dscp_class_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_dscp_drop_precedence_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_meter_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_meter_port_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_pcp_class_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_port_remarking_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_queue_port_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_scheduler_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_shaper_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_shaper_queue_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_storm_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_wred_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_wred_queue_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_rmon_port_get(int argc, char *argv[], void *fd, int numPar);
int gsw_stp_bpdu_rule_get(int argc, char *argv[], void *fd, int numPar);
int gsw_stp_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_version_get(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_id_get(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_vlan_port_member_read(int argc, char *argv[], void *fd, int numPar);
int gsw_wol_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_wol_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_irq_get(int argc, char *argv[], void *fd, int numPar);
int gsw_irq_mask_get(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_rule_read(int argc, char *argv[], void *fd, int numPar);
int gsw_register_get(int argc, char *argv[], void *fd, int numPar);
int gsw_rmon_extend_get(int argc, char *argv[], void *fd, int numPar);

int gsw_timestamp_timer_get(int argc, char *argv[], void *fd, int numPar);
int gsw_timestamp_timer_set(int argc, char *argv[], void *fd, int numPar);

int gsw_timestamp_port_read(int argc, char *argv[], void *fd, int numPar);
int gsw_trunking_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_trunking_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_trunking_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_trunking_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_wred_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_wred_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_flowctrl_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_flowctrl_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_flowctrl_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_flowctrl_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_queue_buffer_reserve_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_queue_buffer_reserve_cfg_get(int argc, char *argv[], void *fd, int numPar);
//#ifdef SWAPI_ETC_CHIP
int gsw_svlan_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_svlan_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_svlan_port_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_svlan_port_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_svlan_class_pcp_port_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_svlan_class_pcp_port_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_svlan_pcp_class_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_svlan_pcp_class_get(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_eg_vlan_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_eg_vlan_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_eg_vlan_entry_write(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_eg_vlan_entry_read(int argc, char *argv[], void *fd, int numPar);

int gsw_pmac_bm_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_bm_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_eg_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_eg_cfg_set(int argc, char *argv[], void *fd, int numPar);
//int gsw_pmac_eg_count_get(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_ig_cfg_get(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_ig_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_count_get(int argc, char *argv[], void *fd, int numPar);

int gsw_rmon_mode_set(int argc, char *argv[], void *fd, int numPar);
int gsw_rmon_if_get(int argc, char *argv[], void *fd, int numPar);
int gsw_rmon_redirect_get(int argc, char *argv[], void *fd, int numPar);
int gsw_rmon_route_get(int argc, char *argv[], void *fd, int numPar);
int gsw_rmon_meter_get(int argc, char *argv[], void *fd, int numPar);
int gsw_rmon_flow_get(int argc, char *argv[], void *fd, int numPar);
int gsw_rmon_tflow_clear(int argc, char *argv[], void *fd, int numPar);



int gsw_qos_meter_act(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_glbl_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_glbl_cfg_get(int argc, char *argv[], void *fd, int numPar);

/*GSWIP 3.1*/
int gsw_ctp_port_assigment_set(int argc, char *argv[], void *fd, int numPar);
int gsw_ctp_port_assigment_get(int argc, char *argv[], void *fd, int numPar);
int gsw_extendedvlan_config_set(int argc, char *argv[], void *fd, int numPar);
int gsw_extendedvlan_config_get(int argc, char *argv[], void *fd, int numPar);
int gsw_extendedvlan_free(int argc, char *argv[], void *fd, int numPar);
int gsw_vlanfilter_config_set(int argc, char *argv[], void *fd, int numPar);
int gsw_vlanfilter_config_get(int argc, char *argv[], void *fd, int numPar);
int gsw_vlanfilter_free(int argc, char *argv[], void *fd, int numPar);
int gsw_bridge_config_set(int argc, char *argv[], void *fd, int numPar);
int gsw_bridge_config_get(int argc, char *argv[], void *fd, int numPar);
int gsw_bridge_free(int argc, char *argv[], void *fd, int numPar);
int gsw_ctp_port_config_set(int argc, char *argv[], void *fd, int numPar);
int gsw_ctp_port_config_get(int argc, char *argv[], void *fd, int numPar);
int gsw_ctp_port_config_reset(int argc, char *argv[], void *fd, int numPar);
int gsw_bridge_port_config_set(int argc, char *argv[], void *fd, int numPar);
int gsw_bridge_port_config_get(int argc, char *argv[], void *fd, int numPar);
int gsw_bridge_port_alloc(int argc, char *argv[], void *fd, int numPar);
int gsw_bridge_port_free(int argc, char *argv[], void *fd, int numPar);
int gsw_extendedvlan_alloc(int argc, char *argv[], void *fd, int numPar);
int gsw_vlanfilter_alloc(int argc, char *argv[], void *fd, int numPar);
int gsw_bridge_alloc(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_rmon_get(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_ctptablestatus(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_bridgeporttablestatus(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_bridgetablestatus(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_Exvlantablestatus(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_VlanFiltertablestatus(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_Metertablestatus(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_Dscp2Pcptablestatus(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_Pmappertablestatus(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_pmac_eg(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_pmac_ig(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_pmac_bp(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_def_pce_qmap(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_def_byp_qmap(int argc, char *argv[], void *fd, int numPar);
int gsw_xgmac_cli(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_ctpstatistics(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_lpstatistics(int argc, char *argv[], void *fd, int numPar);
int gsw_gswss_cli(int argc, char *argv[], void *fd, int numPar);
int gsw_lmac_cli(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_cli(int argc, char *argv[], void *fd, int numPar);
int gsw_pmacbr_cli(int argc, char *argv[], void *fd, int numPar);
int gsw_dump_mem(int argc, char *argv[], void *fd, int numPar);
int gsw_ctp_port_alloc(int argc, char *argv[], void *fd, int numPar);
int gsw_ctp_port_free(int argc, char *argv[], void *fd, int numPar);


int gsw_irq_register(int argc, char *argv[], void *fd, int numPar);
int gsw_irq_unregister(int argc, char *argv[], void *fd, int numPar);
int gsw_irq_enable(int argc, char *argv[], void *fd, int numPar);
int gsw_irq_disable(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_PrintPceIrqList(int argc, char *argv[], void *fd, int numPar);

int gsw_dscp2pcp_map_get(int argc, char *argv[], void *fd, int numPar);
int gsw_defaul_mac_filter_get(int argc, char *argv[], void *fd, int numPar);
int gsw_defaul_mac_filter_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_color_marking_table_set(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_color_marking_table_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_color_remarking_table_get(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_color_remarking_table_set(int argc, char *argv[], void *fd, int numPar);


int gsw_debug_ctp_rmon_port_get(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_bridge_rmon_port_get(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_ctpbypass_rmon_port_get(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_rmon_port_get(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_rmon_port_get_all(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_tunneltemptablestatus(int argc, char *argv[], void *fd, int numPar);


int gsw_pbb_tunneltemplate_config_set(int argc, char *argv[], void *fd, int numPar);
int gsw_pbb_tunneltemplate_config_get(int argc, char *argv[], void *fd, int numPar);
int gsw_pbb_tunneltemplate_alloc(int argc, char *argv[], void *fd, int numPar);
int gsw_pbb_tunneltemplate_free(int argc, char *argv[], void *fd, int numPar);

int gsw_lpidTogpid_assignmentset(int argc, char *argv[], void *fd, int numPar);
int gsw_lpidTogpid_assignmentget(int argc, char *argv[], void *fd, int numPar);
int gsw_gpidTolpid_assignmentset(int argc, char *argv[], void *fd, int numPar);
int gsw_gpidTolpid_assignmentget(int argc, char *argv[], void *fd, int numPar);

int gsw_defaul_pce_qmap_get(int argc, char *argv[], void *fd, int numPar);
int gsw_defaul_bypass_qmap_get(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_get_ig_cfg(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_pmac_rmon_get_all(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_rule_alloc(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_rule_free(int argc, char *argv[], void *fd, int numPar);
int gsw_debug_pceruletablestatus(int argc, char *argv[], void *fd, int numPar);
int gsw_vxlan_cfg_uget(int argc, char *argv[], void *fd, int numPar);
int gsw_vxlan_cfg_uset(int argc, char *argv[], void *fd, int numPar);
int gsw_read_all_enabled_pce_rule(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_meter_alloc(int argc, char *argv[], void *fd, int numPar);
int gsw_qos_meter_free(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_rule_enable(int argc, char *argv[], void *fd, int numPar);
int gsw_pce_rule_disable(int argc, char *argv[], void *fd, int numPar);


#if defined(CONFIG_LTQ_TEST) && CONFIG_LTQ_TEST
int gsw_pmac_bm_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_eg_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_pmac_ig_cfg_set(int argc, char *argv[], void *fd, int numPar);
int gsw_route_entry_read(int argc, char *argv[], void *fd, int numPar);
int gsw_route_entry_add(int argc, char *argv[], void *fd, int numPar);
int gsw_route_entry_delete(int argc, char *argv[], void *fd, int numPar);
int gsw_route_tunnel_entry_add(int argc, char *argv[], void *fd, int numPar);
int gsw_route_tunnel_entry_delete(int argc, char *argv[], void *fd, int numPar);
int gsw_route_tunnel_entry_read(int argc, char *argv[], void *fd, int numPar);
int gsw_route_l2nat_cfg_write(int argc, char *argv[], void *fd, int numPar);
int gsw_route_l2nat_cfg_read(int argc, char *argv[], void *fd, int numPar);
int gsw_route_session_hit_op(int argc, char *argv[], void *fd, int numPar);
int gsw_route_session_dest_mod(int argc, char *argv[], void *fd, int numPar);
#endif

//#endif /* SWAPI_ETC_CHIP */
#endif /* _CMD_DECLARE_H*/
