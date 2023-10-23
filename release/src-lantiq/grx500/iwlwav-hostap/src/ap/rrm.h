/*
 * hostapd / Radio Measurement (RRM)
 * Copyright(c) 2013 - 2016 Intel Mobile Communications GmbH.
 * Copyright(c) 2011 - 2016 Intel Corporation. All rights reserved.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef RRM_H
#define RRM_H

/*
 * Max measure request length is 255, -6 of the body we have 249 for the
 * neighbor report elements. Each neighbor report element is at least 2 + 13
 * bytes, so we can't have more than 16 responders in the request.
 */
#define RRM_RANGE_REQ_MAX_RESPONDERS 16

#define RRM_NOISE_HISTO_IPI_DENSITY_SIZE   11
#define RRM_STA_STATISTICS_GROUP_DATA_SIZE 13

void hostapd_handle_radio_measurement(struct hostapd_data *hapd,
				      const u8 *buf, size_t len, int ssi_signal);
int hostapd_send_lci_req(struct hostapd_data *hapd, const u8 *addr);
int hostapd_send_range_req(struct hostapd_data *hapd, const u8 *addr,
			   u16 random_interval, u8 min_ap,
			   const u8 *responders, unsigned int n_responders);
void hostapd_clean_rrm(struct hostapd_data *hapd);
int hostapd_send_beacon_req(struct hostapd_data *hapd, const u8 *addr,
		u16 num_of_repetitions, u8 measurement_request_mode,
		u8 oper_class, u8 channel, u16 random_interval,
		u16 measurement_duration, u8 mode, const u8* bssid,
		struct wpa_ssid_value* ssid, u8* rep_cond, u8* rep_cond_threshold,
		u8* rep_detail,	const u8* ap_ch_rep, unsigned int ap_ch_rep_len,
		const u8* req_elem, unsigned int req_elem_len, u8 *ch_width,
		u8 *ch_center_freq0, u8 *ch_center_freq1, u8 last_indication);
int hostapd_send_beacon_report_resp(struct hostapd_data *hapd, const u8 *addr,
		u8 dialog_token, u8 measurement_token, u8 measurement_rep_mode,
		u8 op_class, u8 channel, u64 start_time, u16 measurement_duration,
		u8 frame_info, u8 rcpi, u8 rsni, const u8 *bssid, u8 antenna_id,
		u32 parent_tsf, u8 *ch_width, u8 *ch_center_freq0, u8 *ch_center_freq1,
		u8 *rep_frame_body, u8 *rep_frame_body_length);
int hostapd_handle_self_beacon_req(struct hostapd_data *hapd,
		u16 random_interval, u16 measurement_duration, u8 mode);
void hostapd_rrm_beacon_req_tx_status(struct hostapd_data *hapd,
				      const struct ieee80211_mgmt *mgmt,
				      size_t len, int ok);
int hostapd_send_link_measurement_req(struct hostapd_data *hapd,
		const u8 *addr);
int hostapd_send_link_measurement_report(struct hostapd_data *hapd,
		const u8 *addr, struct rrm_link_measurement_report link_msmt_report,
		struct dmg_link_margin *dmg_margin,
		struct dmg_link_adaptation_ack *dmg_ack);
int hostapd_get_statistics_group_data_len(u8 group_identity);
int hostapd_send_sta_statistics_req(struct hostapd_data *hapd, const u8 *addr,
		u16 num_of_repetitions, u8 measurement_request_mode,
		const u8 *peer_addr, u16 random_interval, u16 measurement_duration,
		u8 group_identity, struct rrm_trig_rep_counters *trig_rep_sta_counters,
		struct rrm_trig_rep_counters *trig_rep_qos_sta_counters,
		struct rrm_trig_rep_counters *trig_rep_rsna_counters);
int hostapd_send_sta_statistics_report(struct hostapd_data *hapd, const u8 *addr,
		u8 dialog_token, u8 measurement_token, u8 measurement_rep_mode,
		u16 measurement_duration, u8 group_identity,
		const u32 *statistics_group_data, u8 statistics_group_data_len,
		u8 *rep_reason);
int hostapd_send_channel_load_req(struct hostapd_data *hapd, const u8 *addr,
		u16 num_of_repetitions, u8 measurement_request_mode, u8 oper_class,
		u8 channel, u16 random_interval, u16 measurement_duration, u8 *rep_cond,
		u8 *ch_load_ref_val, u8 *ch_width, u8 *ch_center_freq0,
		u8 *ch_center_freq1);
int hostapd_send_channel_load_report(struct hostapd_data *hapd, const u8 *addr,
		u8 dialog_token, u8 measurement_token, u8 measurement_rep_mode,
		u8 op_class, u8 channel, u64 start_time, u16 measurement_duration,
		u8 channel_load, u8 *ch_width, u8 *ch_center_freq0,
		u8 *ch_center_freq1);
int hostapd_send_noise_histogram_req(struct hostapd_data *hapd, const u8 *addr,
		u16 num_of_repetitions, u8 measurement_request_mode, u8 oper_class,
		u8 channel, u16 random_interval, u16 measurement_duration, u8 *rep_cond,
		u8 *anpi_ref_val, u8 *ch_width, u8 *ch_center_freq0,
		u8 *ch_center_freq1);
int hostapd_send_noise_histogram_report(struct hostapd_data *hapd, const u8 *addr,
		u8 dialog_token, u8 measurement_token, u8 measurement_rep_mode,
		u8 op_class, u8 channel, u64 start_time, u16 measurement_duration,
		u8 ant_id, u8 anpi, u8 *ipi_density, u8 *ch_width, u8 *ch_center_freq0,
		u8 *ch_center_freq1);

#endif /* RRM_H */
