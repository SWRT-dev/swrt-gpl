/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * $Id$
 *
 *
 *
 * Core statistics interface
 *
 */
#ifndef __CORE_STATS_H__
#define __CORE_STATS_H__

int __MTLK_IFUNC mtlk_core_get_associated_dev_rate_info_rx_stats_clb (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_associated_dev_rate_info_tx_stats_clb (mtlk_handle_t hcore, const void *data, uint32 data_size);
void __MTLK_IFUNC mtlk_core_get_associated_dev_rate_info_rx_stats (sta_entry * sta, peerRateInfoRxStats * peer_rx_rate_info);
void __MTLK_IFUNC mtlk_core_get_associated_dev_rate_info_tx_stats (sta_entry * sta, peerRateInfoTxStats * peer_tx_rate_info);

void __MTLK_IFUNC mtlk_core_calc_associated_dev_rate_info_rx_stats (peerRateInfoRxStats * dest, peerRateInfoRxStats * new, peerRateInfoRxStats * old);
void __MTLK_IFUNC mtlk_core_calc_associated_dev_rate_info_tx_stats (peerRateInfoTxStats * dest, peerRateInfoTxStats * new, peerRateInfoTxStats * old);

int __MTLK_IFUNC mtlk_core_stats_poll_period_get (mtlk_handle_t hcore, const void* data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_stats_poll_period_set (mtlk_handle_t hcore, const void* data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_channel_stats (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_associated_dev_tid_stats (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_associated_dev_stats (mtlk_handle_t hcore, const void *data, uint32 data_size);

int __MTLK_IFUNC mtlk_core_get_peer_list (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_peer_flow_status (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_peer_capabilities (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_peer_rate_info (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_recovery_stats (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_hw_flow_status (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_tr181_wlan_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_tr181_hw_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_tr181_peer_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_dev_diag_result2 (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_dev_diag_result3 (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_associated_stations_stats (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC mtlk_core_get_priority_gpio_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_cca_stats_get (mtlk_handle_t hcore, const void *data, uint32 data_size);
void __MTLK_IFUNC wave_core_cca_stats_save (mtlk_core_t *core, int channel, wave_cca_stats_t *cca_stats, wave_cca_stats_user_t *cca_stats_user, BOOL on_chan);
int __MTLK_IFUNC wave_core_probe_req_list_get(mtlk_handle_t hcore, const void* data, uint32 data_size);
int __MTLK_IFUNC wave_core_radio_usage_stats_get (mtlk_handle_t hcore, const void *data, uint32 data_size);

#ifdef CONFIG_WAVE_DEBUG
int __MTLK_IFUNC wave_core_get_peer_host_if_qos_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_peer_host_if_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_peer_rx_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_peer_ul_bsrc_tid_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_peer_baa_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_link_adaption_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_plan_manager_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_peer_twt_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_per_client_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_peer_phy_rx_status (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_peer_info_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_wlan_host_if_qos_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_wlan_host_if_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_wlan_rx_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_wlan_baa_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_radio_rx_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_radio_baa_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_tsman_init_tid_gl_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_tsman_init_sta_gl_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_tsman_rcpt_tid_gl_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_tsman_rcpt_sta_gl_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_radio_link_adapt_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_multicast_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_training_man_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_grp_man_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_general_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_cur_channel_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_radio_phy_rx_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_dynamic_bw_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
int __MTLK_IFUNC wave_core_get_link_adapt_mu_statistics(mtlk_handle_t hcore, const void *data, uint32 data_size);
#endif /* CONFIG_WAVE_DEBUG */
#endif /* __CORE_STATS_H_ */
