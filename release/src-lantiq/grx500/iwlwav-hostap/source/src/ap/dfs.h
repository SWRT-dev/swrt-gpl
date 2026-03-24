/*
 * DFS - Dynamic Frequency Selection
 * Copyright (c) 2002-2013, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2013-2017, Qualcomm Atheros, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#ifndef DFS_H
#define DFS_H

/* CAC finished reason code */
enum hostapd_cac_finished_stat {
	HOSTAPD_CAC_STAT_FAILED  = 0,
	HOSTAPD_CAC_STAT_SUCCESS = 1,
	HOSTAPD_CAC_STAT_PAUSED  = 2
};

int hostapd_handle_dfs(struct hostapd_iface *iface);
int hostapd_csa_in_progress(struct hostapd_iface *iface);

void hostapd_dfs_cac_restart(struct hostapd_iface *iface);
int hostapd_dfs_complete_cac(struct hostapd_iface *iface, enum hostapd_cac_finished_stat cac_status,
			     int freq, int ht_enabled, int chan_offset,
			     int chan_width, int cf1, int cf2);
int hostapd_dfs_pre_cac_expired(struct hostapd_iface *iface, int freq,
				int ht_enabled, int chan_offset, int chan_width,
				int cf1, int cf2);
int hostapd_dfs_radar_detected(struct hostapd_iface *iface, int freq,
			       int ht_enabled,
			       int chan_offset, int chan_width,
			       int cf1, int cf2, u8 rbm);
int hostapd_dfs_nop_finished(struct hostapd_iface *iface, int freq,
			     int ht_enabled,
			     int chan_offset, int chan_width, int cf1, int cf2);
int hostapd_is_dfs_required(struct hostapd_iface *iface);
int hostapd_dfs_start_cac(struct hostapd_iface *iface, int freq,
			  int ht_enabled, int chan_offset, int chan_width,
			  int cf1, int cf2);
int set_dfs_state_freq(struct hostapd_iface *iface, int freq, u32 state);
int hostapd_handle_dfs_offload(struct hostapd_iface *iface);
void hostapd_send_dfs_debug_channel_flag(struct hostapd_iface *iface);
int hostapd_send_dfs_channel_state(struct hostapd_iface *iface);
struct hostapd_channel_data *dfs_get_debug_dfs_chan(struct hostapd_iface *iface,
						    int *secondary_channel,
						    u8 *vht_oper_centr_freq_seg0_idx,
						    u8 *vht_oper_centr_freq_seg1_idx);
int dfs_get_start_chan_idx(struct hostapd_iface *iface, int *seg1_start);
int set_first_non_dfs_channel(struct hostapd_iface *iface);
int dfs_get_used_n_chans(struct hostapd_iface *iface, int *seg1);
int dfs_chan_range_available(struct hostapd_hw_modes *mode,
			     int first_chan_idx, int num_chans,
			     int skip_radar);
int hostapd_dfs_start_channel_switch(struct hostapd_iface *iface);
bool dfs_is_failsafe_enabled(struct hostapd_iface *iface);
void hostapd_dfs_debug_set_chan_dfs_state (struct hostapd_iface *iface, int state,
					   int start_freq, int n_chans);

#endif /* DFS_H */
