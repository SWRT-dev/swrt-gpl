/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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
int __MTLK_IFUNC mtlk_core_get_priority_gpio_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size);
#endif /* __CORE_STATS_H_ */
