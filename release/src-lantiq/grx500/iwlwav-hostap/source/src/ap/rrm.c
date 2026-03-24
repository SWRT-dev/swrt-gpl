/*
 * hostapd / Radio Measurement (RRM)
 * Copyright(c) 2013 - 2016 Intel Mobile Communications GmbH.
 * Copyright(c) 2011 - 2016 Intel Corporation. All rights reserved.
 * Copyright (c) 2016-2017, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/wpa_ctrl.h"
#include "wpa_debug.h"
#include "hostapd.h"
#include "ap_drv_ops.h"
#include "sta_info.h"
#include "eloop.h"
#include "neighbor_db.h"
#include "rrm.h"
#include "acs.h"

#define HOSTAPD_RRM_REQUEST_TIMEOUT 5
/**
 * "wide_band_ch_switch=%d,%d,%d" string length between 27 and 33 bytes
 * WIDE_BAND_CH_SWICTH_DEBUG_STR_LEN = max-length + 1 (33 + 1);
 */
#define WIDE_BAND_CH_SWICTH_DEBUG_STR_LEN 34

static void hostapd_build_beacon_report_resp(struct wpabuf *buf,
		u8 measurement_token, u8 measurement_rep_mode, u8 op_class, u8 channel,
		u64 start_time, u16 measurement_duration, u8 frame_info, u8 rcpi,
		u8 rsni, const u8 *bssid, u8 antenna_id, u32 parent_tsf, u8 *ch_width,
		u8 *ch_center_freq0, u8 *ch_center_freq1, u8 *rep_frame_body,
		u8 *rep_frame_body_length);
static int hostapd_send_beacon_report_with_header(struct hostapd_data *hapd,
		const u8 *addr, const u8* data, int len, u8 dialog_token);

static void hostapd_lci_rep_timeout_handler(void *eloop_data, void *user_ctx)
{
	struct hostapd_data *hapd = eloop_data;

	wpa_printf(MSG_DEBUG, "RRM: LCI request (token %u) timed out",
		   hapd->lci_req_token);
	hapd->lci_req_active = 0;
}

static void hostapd_memory_as_hex(char *buf, size_t buf_size, const u8 *address, int len)
{
	int curr_byte_index, ret;

	for (curr_byte_index = 0; curr_byte_index < len; curr_byte_index++) {
		ret = sprintf_s(&buf[curr_byte_index * 3], buf_size - curr_byte_index * 3, "%02x ",
			  address[curr_byte_index]);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
	}
}

static void hostapd_handle_lci_report(struct hostapd_data *hapd, u8 token,
				      const u8 *pos, size_t len)
{
	if (!hapd->lci_req_active || hapd->lci_req_token != token) {
		wpa_printf(MSG_DEBUG, "Unexpected LCI report, token %u", token);
		return;
	}

	hapd->lci_req_active = 0;
	eloop_cancel_timeout(hostapd_lci_rep_timeout_handler, hapd, NULL);
	wpa_printf(MSG_DEBUG, "LCI report token %u len %zu", token, len);
}


static void hostapd_range_rep_timeout_handler(void *eloop_data, void *user_ctx)
{
	struct hostapd_data *hapd = eloop_data;

	wpa_printf(MSG_DEBUG, "RRM: Range request (token %u) timed out",
		   hapd->range_req_token);
	hapd->range_req_active = 0;
}


static void hostapd_handle_range_report(struct hostapd_data *hapd, u8 token,
					const u8 *pos, size_t len)
{
	if (!hapd->range_req_active || hapd->range_req_token != token) {
		wpa_printf(MSG_DEBUG, "Unexpected range report, token %u",
			   token);
		return;
	}

	hapd->range_req_active = 0;
	eloop_cancel_timeout(hostapd_range_rep_timeout_handler, hapd, NULL);
	wpa_printf(MSG_DEBUG, "Range report token %u len %zu", token, len);
}

static void hostapd_send_general_rrm_report(struct hostapd_data *hapd,
		const u8 *sta_addr,u8 dialog_token, u8 measurement_token,
		u8 measurement_rep_mode, enum measure_type measure_type)
{
	struct wpabuf *buf;

	/* Measurement report (3) + Measurement element(5) */
	buf = wpabuf_alloc(3 + 5);
	if (!buf) {
		wpa_printf(MSG_ERROR, "send general rrm report - can't allocate buffer");
		return;
	}

	/* IEEE P802.11-REVmc/D6.0, 9.6.7.3 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_RADIO_MEASUREMENT_REPORT);
	wpabuf_put_u8(buf, dialog_token);

	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REPORT);
	wpabuf_put_u8(buf, 3); /* Length */

	wpabuf_put_u8(buf, measurement_token);
	wpabuf_put_u8(buf, measurement_rep_mode);
	wpabuf_put_u8(buf, measure_type);

	hostapd_drv_send_action(hapd, hapd->iface->freq, 0, sta_addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
}

int hostapd_handle_self_beacon_report_scan_results(struct hostapd_data *hapd)
{
	struct wpa_scan_results *scan_res = NULL;
	int i, found_result = 0;
	int ht = hapd->iconf->ieee80211n && !hapd->conf->disable_11n;
	int vht = hapd->iconf->ieee80211ac && !hapd->conf->disable_11ac;
	u8 op_class, channel, phy_type;
	const u8 bssid[ETH_ALEN] = {};

	phy_type = ieee80211_get_phy_type(hapd->iface->freq, ht, vht);
	ieee80211_freq_to_channel_ext(hapd->iface->freq,
			hapd->iconf->secondary_channel, hostapd_get_oper_chwidth(hapd->iconf),
			&op_class, &channel);

	scan_res = hostapd_driver_get_scan_results(hapd);
	if (!scan_res) {
		wpa_printf(MSG_ERROR, "failed to get scan results");
		return -1;
	}

	if (scan_res->num == 0) {
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_SELF_BEACON_REP_RXED MACSTR
					" measurement_rep_mode=%d op_class=%d channel=%d start_time=%d duration=%d frame_info=%d rcpi=%d rsni=%d bssid=" MACSTR " antenna_id=%d parent_tsf=%d",
					MAC2STR(hapd->own_addr), 0, op_class, channel,
					0, 0, phy_type, 0, 0, MAC2STR(bssid), 0, 0);
		wpa_scan_results_free(scan_res);
		return 0;
	}

	for (i = 0; i < scan_res->num; i++) {
		struct wpa_scan_res *bss = scan_res->res[i];
		if (bss->freq == hapd->iface->freq) {
			u8 rcpi = rssi_to_rcpi(bss->level);
			found_result = 1;
			wpa_msg(hapd->msg_ctx, MSG_INFO,
					RRM_EVENT_SELF_BEACON_REP_RXED MACSTR
					" measurement_rep_mode=%d op_class=%d channel=%d start_time=%llu duration=%d frame_info=%d rcpi=%d rsni=%d bssid=" MACSTR " antenna_id=%d parent_tsf=%d",
					MAC2STR(hapd->own_addr), 0, op_class, channel, bss->tsf, 0,
					phy_type, rcpi, 255, MAC2STR(bss->bssid), 0, 0);
		}
	}

	if (!found_result)
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_SELF_BEACON_REP_RXED MACSTR
					" measurement_rep_mode=%d op_class=%d channel=%d start_time=%d duration=%d frame_info=%d rcpi=%d rsni=%d bssid=" MACSTR " antenna_id=%d parent_tsf=%d",
					MAC2STR(hapd->own_addr), 0, op_class, channel,
					0, 0, phy_type, 0, 0, MAC2STR(bssid), 0, 0);

	wpa_scan_results_free(scan_res);
	return 0;
}

void hostapd_handle_sta_beacon_report_scan_results(struct hostapd_iface *iface)
{
	struct wpa_scan_results *scan_res = NULL;
	struct wpabuf *buf;
	const u8 *pos;
	int i, len;
	int ht = iface->bss[0]->iconf->ieee80211n
			&& !iface->bss[0]->conf->disable_11n;
	int vht = iface->bss[0]->iconf->ieee80211ac
			&& !iface->bss[0]->conf->disable_11ac;
	u8 op_class, channel, phy_type;
	const u8 bssid[ETH_ALEN] = {};

	phy_type = ieee80211_get_phy_type(iface->freq, ht, vht);
	ieee80211_freq_to_channel_ext(iface->freq,
			iface->bss[0]->iconf->secondary_channel,
			hostapd_get_oper_chwidth(iface->bss[0]->iconf),
			&op_class, &channel);

	buf = wpabuf_alloc(3 + IEEE80211_MAX_MMPDU_SIZE);
	if (!buf) {
		wpa_printf(MSG_ERROR, "buffer allocation failed");
		return;
	}

	scan_res = hostapd_driver_get_scan_results(iface->bss[0]);
	if (!scan_res) {
		wpa_printf(MSG_ERROR, "failed to get scan results");
		wpabuf_free(buf);
		return;
	}

	for (i = 0; i < scan_res->num; i++) {
		struct wpa_scan_res *bss = scan_res->res[i];
		if (bss->freq == iface->freq) {
			u8 rcpi = rssi_to_rcpi(bss->level);
			if (!iface->bss[0]->conf->rrm_manipulate_measurement) {
				hostapd_build_beacon_report_resp(buf,
						iface->last_beacon_req.measurement_token, 0,
						op_class, channel, bss->tsf, 0, phy_type, rcpi, 255,
						bss->bssid, 0, 0, NULL, NULL, NULL, NULL, NULL);
			} else {
				wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO,
						RRM_EVENT_BEACON_REQUEST_RXED MACSTR
						" dialog_token=%d measurement_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%llu duration=%d frame_info=%d rcpi=%d rsni=%d bssid=" MACSTR " antenna_id=%d parent_tsf=%d",
						MAC2STR(iface->last_beacon_req.src_addr),
						iface->last_beacon_req.dialog_token,
						iface->last_beacon_req.measurement_token,
						0, op_class, channel, bss->tsf,
						0, phy_type, rcpi, 255, MAC2STR(bss->bssid), 0, 0);
			}
		}
	}

	if (!iface->bss[0]->conf->rrm_manipulate_measurement) {
		len = wpabuf_len(buf);
		pos = wpabuf_head_u8(buf);
		hostapd_send_beacon_report_with_header(iface->bss[0],
				iface->last_beacon_req.src_addr, pos, len,
				iface->last_beacon_req.dialog_token);
	} else if (scan_res->num == 0) {
		/* no results notifications */
		wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO,
				RRM_EVENT_BEACON_REQUEST_RXED MACSTR
				" dialog_token=%d measurement_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%d duration=%d frame_info=%d rcpi=%d rsni=%d bssid=" MACSTR " antenna_id=%d parent_tsf=%d",
				MAC2STR(iface->last_beacon_req.src_addr),
				iface->last_beacon_req.dialog_token,
				iface->last_beacon_req.measurement_token,
				0, op_class, channel, 0, 0, phy_type,
				0, 0, MAC2STR(bssid), 0, 0);
	}

	wpa_scan_results_free(scan_res);
	wpabuf_free(buf);
}

static void rrm_reset_scan_callback(struct hostapd_iface *iface)
{
#ifdef CONFIG_ACS
	acs_set_bg_scan_callback(iface);
#else
	iface->scan_cb = NULL;
#endif
}

static void rrm_current_channel_scan_complete(struct hostapd_iface *iface)
{
	if (iface->self_beacon_req) {
		hostapd_handle_self_beacon_report_scan_results(iface->bss[0]);
	} else {
		hostapd_handle_sta_beacon_report_scan_results(iface);
	}
	rrm_reset_scan_callback(iface);
}

static int rrm_current_channel_request_scan(struct hostapd_iface *iface)
{
	struct wpa_driver_scan_params params;

	os_memset(&params, 0, sizeof(params));
	params.freqs = os_calloc(1 + 1, sizeof(params.freqs[0]));
	if (params.freqs == NULL)
		return -1;

	params.freqs[0] = iface->freq;
	iface->scan_cb = rrm_current_channel_scan_complete;

	if (hostapd_driver_scan(iface->bss[0], &params) < 0) {
		wpa_printf(MSG_ERROR, "RRM: Failed to request scan");
		os_free(params.freqs);
		return -1;
	}

	os_free(params.freqs);
	return 0;
}

static void hostapd_handle_beacon_report_request(struct hostapd_data *hapd,
		u8 token, const u8 *pos, size_t len, const u8 *sta_addr,
		u16 num_of_repetition)
{
	u8 channel, measurement_token, beacon_measurement_mode;
	int ret;
	errno_t err;

	measurement_token = pos[0];
	channel = pos[4];
	beacon_measurement_mode = pos[9];

	if ((beacon_measurement_mode == BEACON_REPORT_MODE_PASSIVE &&
		!(hapd->conf->radio_measurements[0] & WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE))
		|| (beacon_measurement_mode == BEACON_REPORT_MODE_TABLE &&
		!(hapd->conf->radio_measurements[0] & WLAN_RRM_CAPS_BEACON_REPORT_TABLE))
		|| (beacon_measurement_mode == BEACON_REPORT_MODE_ACTIVE))
	{
		wpa_printf(MSG_ERROR, "beacon report in RRM is not supported");
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE,
				MEASURE_TYPE_BEACON);
		return;
	}

	if ((channel != hapd->iface->conf->channel)
			&& (channel != 0) && (channel != 255)) {
		wpa_printf(MSG_DEBUG, "requested channel (%d) is not supported, only the current AP channel (%d) is supported",
				channel, hapd->iface->conf->channel);
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_REFUSED,
				MEASURE_TYPE_BEACON);
		return;
	}

	err = memcpy_s(hapd->iface->last_beacon_req.src_addr,
		 sizeof(hapd->iface->last_beacon_req.src_addr),
		 sta_addr, ETH_ALEN);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return;
	}
	hapd->iface->last_beacon_req.dialog_token = token;
	hapd->iface->last_beacon_req.measurement_token = measurement_token;

	if (beacon_measurement_mode == BEACON_REPORT_MODE_TABLE) {
		hostapd_handle_sta_beacon_report_scan_results(hapd->iface);
	} else if (beacon_measurement_mode == BEACON_REPORT_MODE_PASSIVE) {
		hapd->iface->self_beacon_req = 0;
		ret = rrm_current_channel_request_scan(hapd->iface);
		if (ret)
			wpa_printf(MSG_ERROR, "Failed to request scan");
	}
}

static void hostapd_handle_beacon_report_response(struct hostapd_data *hapd,
		u8 token, const u8 *pos, size_t len, const u8 *sta_addr)
{
	int ret;
	const u8 *end, *attr;
	errno_t err;
	u8 op_class, channel, frame_info, rcpi, rsni, ant_id;
	u8 measurement_rep_mode = 0;
	u16 measurement_duration;
	u32 parent_tsf;
	unsigned long long start_time;
	u8 bssid[ETH_ALEN] = {0};

	int report_id = -1, fragment_id = -1, more_fragment_bit = 0;
	int first_sub_elem_idx;

	struct beacon_body frame_body;
	char o_ssid[128] = "";
	char o_rm_capa[128] = "";
	char o_vendor_specific[255 * 3 + 1] = "";
	char o_rsni[255 * 3 + 1] = "";

	char frame_body_fragment_id[64] = "";
	char last_beacon_rep[32] = "";

	os_memset(&frame_body, 0, sizeof(frame_body));

	wpa_printf(MSG_DEBUG, "Beacon report token %u len %zu",	token, len);

	measurement_rep_mode = pos[1];
	if (measurement_rep_mode != 0 || (len < 29)) {
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_BEACON_REP_RXED MACSTR
				" dialog_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%d duration=%d frame_info=%d rcpi=%d rsni=%d bssid=" MACSTR " antenna_id=%d parent_tsf=%d more_fragments=0",
				MAC2STR(sta_addr), token, measurement_rep_mode, 0, 0, 0, 0, 0,
				0, 0, MAC2STR(bssid), 0, 0);
		return;
	}

	end = pos + len;

	op_class = pos[3];
	channel = pos[4];
	start_time = WPA_GET_LE64(&pos[5]);
	measurement_duration = WPA_GET_LE16(&pos[13]);
	frame_info = pos[15];
	rcpi = pos[16];
	rsni = pos[17];
	err = memcpy_s(bssid, sizeof(bssid), &pos[18], ETH_ALEN);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return;
	}
	ant_id = pos[24];
	parent_tsf = WPA_GET_LE32(&pos[25]);

	if ((pos[29] != WLAN_BEACON_REPORT_SUBELEM_FRAME_BODY)) { /* No sub-elements */
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_BEACON_REP_RXED MACSTR
				" dialog_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%llu duration=%d frame_info=%d rcpi=%d rsni=%d bssid=" MACSTR " antenna_id=%d parent_tsf=%d more_fragments=0",
				MAC2STR(sta_addr), token, measurement_rep_mode, op_class,
				channel, start_time, measurement_duration, frame_info, rcpi,
				rsni, MAC2STR(bssid), ant_id, parent_tsf);
		return;
	}

	if (len < 30 || (&pos[30]) + pos[30] >= end) {
		wpa_printf(MSG_ERROR, "%s: beacon report frame is corrupted", __func__);
		return;
	}
	frame_body.length = pos[30];

	attr = get_ie(&pos[29], len - 29, WLAN_BEACON_REPORT_SUBELEM_FRAME_BODY_FRAGMENT_ID);
	if (attr && attr + 1 < end && attr[1] == 2 && attr + 1 + attr[1] < end) {
		report_id = attr[2];
		fragment_id = attr[3] & 0x7f; /* Bits 0 - 6 */
		more_fragment_bit = attr[3] >> 7; /* Bit 7 */
		ret = sprintf_s(frame_body_fragment_id, 64,
			  "report_id=%d fragment_id=%d more_fragment_bit=%d",
			  report_id, fragment_id, more_fragment_bit);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
	}

	/* initial fragment include additional 12 bytes */
	if (fragment_id == 0 || fragment_id == -1) {
		if (frame_body.length < 12) {
			wpa_printf(MSG_ERROR, "%s: beacon report frame is corrupted, frame_body.length=%d",
				   __func__, frame_body.length);
			return;
		}
		frame_body.length -= 12;
		frame_body.timestamp = WPA_GET_LE64(&pos[31]);
		frame_body.beacon_int = WPA_GET_LE16(&pos[39]);
		frame_body.capab_info = WPA_GET_LE16(&pos[41]);
		first_sub_elem_idx = 43;
	} else {
		first_sub_elem_idx = 31;
	}

	attr = get_ie(&pos[first_sub_elem_idx], frame_body.length, WLAN_EID_SSID);
	if (attr && attr + 1 < end && attr[1] > 0 && attr[1] <= 32 && attr + 1 + attr[1] < end)
		hostapd_memory_as_hex(o_ssid, sizeof(o_ssid), &attr[2], attr[1]);
	attr = get_ie(&pos[first_sub_elem_idx], frame_body.length, WLAN_EID_RRM_ENABLED_CAPABILITIES);
	if (attr && attr + 1 < end && attr[1] == 5 && attr + 1 + attr[1] < end)
		hostapd_memory_as_hex(o_rm_capa, sizeof(o_rm_capa), &attr[2], attr[1]);
	attr = get_ie(&pos[first_sub_elem_idx], frame_body.length, WLAN_EID_RSN);
	if (attr && attr + 1 < end && attr[1] >= 2 && attr + 1 + attr[1] < end)
		hostapd_memory_as_hex(o_rsni, sizeof(o_rsni), &attr[2], attr[1]);
	attr = get_ie(&pos[first_sub_elem_idx], frame_body.length, WLAN_EID_VENDOR_SPECIFIC);
	if (attr && attr + 1 < end && attr[1] > 0 && attr + 1 + attr[1] < end)
		hostapd_memory_as_hex(o_vendor_specific, sizeof(o_vendor_specific), &attr[2], attr[1]);

	attr = get_ie(&pos[29], len - 29, WLAN_BEACON_REPORT_SUBELEM_LAST_INDICATION);
	if (attr && attr + 1 < end && attr[1] == 1 && attr + 1 + attr[1] < end) {
		ret = sprintf_s(last_beacon_rep, 32, "last_beacon_rep=%d", attr[2]);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
	}

	wpa_printf(MSG_DEBUG, "%s %s", frame_body_fragment_id, last_beacon_rep);

	wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_BEACON_REP_RXED MACSTR
			" dialog_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%llu duration=%d frame_info=%d rcpi=%d rsni=%d bssid=" MACSTR " antenna_id=%d parent_tsf=%d timestamp=%llu beacon_int=%d capab_info=%02x ssid=%s rm_capa=%s vendor_specific=%s rsn_info=%s more_fragments=%d",
			MAC2STR(sta_addr), token, measurement_rep_mode, op_class, channel,
			start_time, measurement_duration, frame_info, rcpi, rsni,
			MAC2STR(bssid), ant_id, parent_tsf, frame_body.timestamp,
			frame_body.beacon_int, frame_body.capab_info,
			o_ssid, o_rm_capa, o_vendor_specific, o_rsni, more_fragment_bit);
}

int hostapd_handle_self_beacon_req(struct hostapd_data *hapd,
		u16 random_interval, u16 measurement_duration, u8 mode)
{
	int ret = 0;
	const u8 bssid[ETH_ALEN] = {};

	if ((mode == BEACON_REPORT_MODE_PASSIVE &&
		!(hapd->conf->radio_measurements[0] & WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE))
		|| (mode == BEACON_REPORT_MODE_TABLE &&
		!(hapd->conf->radio_measurements[0] & WLAN_RRM_CAPS_BEACON_REPORT_TABLE))
		|| (mode == BEACON_REPORT_MODE_ACTIVE))
	{
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_SELF_BEACON_REP_RXED MACSTR
				" measurement_rep_mode=%d op_class=%d channel=%d start_time=%d duration=%d frame_info=%d rcpi=%d rsni=%d bssid=" MACSTR " antenna_id=%d parent_tsf=%d",
				MAC2STR(hapd->own_addr),
				MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE, 0, 0, 0, 0, 0, 0, 0,
				MAC2STR(bssid), 0, 0);
		return -1;
	}

	if (mode == BEACON_REPORT_MODE_TABLE) {
		ret = hostapd_handle_self_beacon_report_scan_results(hapd);
	} else if (mode == BEACON_REPORT_MODE_PASSIVE) {
		hapd->iface->self_beacon_req = 1;
		ret = rrm_current_channel_request_scan(hapd->iface);
	}

	return ret;
}

static void hostapd_handle_channel_load_request(struct hostapd_data *hapd,
		u8 token, const u8 *pos, size_t len, const u8 *sta_addr,
		u16 num_of_repetition)
{
	const u8 *ie, *end;
	u8 op_class, channel, measurement_token;
	u8 ch_load_rep_cond = 0, ch_load_ref_val = 0;
	u8 channel_load = 0;
	u16 measurement_duration_report = 0;
	u64 start_time = 0;
	struct intel_vendor_radio_info radio_info;

	end = pos + len;
	measurement_token = pos[0];
	op_class = pos[3];
	channel = pos[4];

	if (!(hapd->conf->radio_measurements[1] & WLAN_RRM_CAPS_CHANNEL_LOAD)) {
		wpa_printf(MSG_ERROR, "channel load in RRM is not supported");
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE,
				MEASURE_TYPE_CHANNEL_LOAD);
		return;
	}

	if (channel != hapd->iface->conf->channel) {
		wpa_printf(MSG_DEBUG, "requested channel (%d) is not supported, only the current AP channel (%d) is supported",
				channel, hapd->iface->conf->channel);
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_REFUSED,
				MEASURE_TYPE_CHANNEL_LOAD);
		return;
	}

	if (hostapd_drv_get_radio_info(hapd, &radio_info)) {
		wpa_printf(MSG_ERROR, "failed to get radio measurements");
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE,
				MEASURE_TYPE_CHANNEL_LOAD);
		return;
	}

	if ((ie = get_ie(&pos[9], end - &pos[9], 1)) && (ie[1] == 2)) {
		ch_load_rep_cond = ie[2];
		ch_load_ref_val = ie[3];
	}

	measurement_duration_report = 255;
	start_time = radio_info.tsf_start_time;
	channel_load = radio_info.load;

	if ((ch_load_rep_cond == 1) && (ch_load_ref_val > channel_load)) {
		wpa_printf(MSG_DEBUG, "ch_load is less than ch_load ref val");
		return;
	}

	if ((ch_load_rep_cond == 2) && (ch_load_ref_val < channel_load)) {
		wpa_printf(MSG_DEBUG, "ch_load is greater than ch_load ref val");
		return;
	}

	if (!hapd->conf->rrm_manipulate_measurement) {
		hostapd_send_channel_load_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_ACCEPT, op_class,
				channel, start_time, measurement_duration_report, channel_load,
				NULL, NULL, NULL);
	} else {
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_CHANNEL_LOAD_REQUEST_RXED MACSTR
			" dialog_token=%d measurement_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%llu duration=%d channel_load=%d",
			MAC2STR(sta_addr), token, measurement_token,
			MEASUREMENT_REPORT_MODE_ACCEPT, op_class, channel, start_time,
			measurement_duration_report, channel_load);
	}
}

static void hostapd_handle_channel_load_report(struct hostapd_data *hapd,
		u8 token, const u8 *pos, size_t len, const u8 *sta_addr)
{
	int ret;
	u8 op_class, channel, channel_load, measurement_rep_mode;
	u16 measurement_duration;
	u64 start_time;
	char wide_band_ch_switch[WIDE_BAND_CH_SWICTH_DEBUG_STR_LEN];

	wpa_printf(MSG_DEBUG, "channel load token %u len %zu",	token, len);

	measurement_rep_mode = pos[1];
	if (measurement_rep_mode != 0) {
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_CHANNEL_LOAD_RXED MACSTR
				" dialog_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%d duration=%d channel_load=%d",
				MAC2STR(sta_addr), token, measurement_rep_mode, 0, 0, 0, 0, 0);
		return;
	}

	op_class = pos[3];
	channel = pos[4];
	start_time = WPA_GET_LE64(&pos[5]);
	measurement_duration = WPA_GET_LE16(&pos[13]);
	channel_load = pos[15];

	wide_band_ch_switch[0] = '\0';
	if (pos[16] == 163 && pos[17] == 5 && pos[18] == 194 && pos[19] == 3) {
		ret = sprintf_s(wide_band_ch_switch, WIDE_BAND_CH_SWICTH_DEBUG_STR_LEN,
			" wide_band_ch_switch=%d,%d,%d", pos[20], pos[21], pos[22]);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
	}

	wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_CHANNEL_LOAD_RXED MACSTR
			" dialog_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%llu duration=%d channel_load=%d%s",
			MAC2STR(sta_addr), token, measurement_rep_mode, op_class, channel,
			start_time, measurement_duration, channel_load,
			wide_band_ch_switch);
}

static void hostapd_handle_noise_histogram_request(struct hostapd_data *hapd,
		u8 token, const u8 *pos, size_t len, const u8 *sta_addr,
		u16 num_of_repetition)
{
	const u8 *ie, *end;
	u8 op_class, channel, measurement_token, ant_id = 0, anpi = 0;
	u8 noise_histogram_rep_cond = 0, noise_histogram_ref_val = 0;
	u8 ipi_density[RRM_NOISE_HISTO_IPI_DENSITY_SIZE] = {};
	u16 measurement_duration_report = 0;
	u64 start_time = 0;
	char ipi_density_str[64], *cur, *end_str;
	int ret, i;
	struct intel_vendor_radio_info radio_info;

	end = pos + len;
	measurement_token = pos[0];
	op_class = pos[3];
	channel = pos[4];

	if (!(hapd->conf->radio_measurements[1] & WLAN_RRM_CAPS_NOISE_HISTOGRAM)) {
		wpa_printf(MSG_ERROR, "noise histogram in RRM is not supported");
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE,
				MEASURE_TYPE_NOISE_HIST);
		return;
	}

	if (channel != hapd->iface->conf->channel) {
		wpa_printf(MSG_DEBUG, "requested channel (%d) is not supported, only the current AP channel (%d) is supported",
				channel, hapd->iface->conf->channel);
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_REFUSED,
				MEASURE_TYPE_NOISE_HIST);
		return;
	}

	if (hostapd_drv_get_radio_info(hapd, &radio_info)) {
		wpa_printf(MSG_ERROR, "failed to get radio measurements");
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE,
				MEASURE_TYPE_NOISE_HIST);
		return;
	}

	if ((ie = get_ie(&pos[9], end - &pos[9], 1)) && (ie[1] == 2)) {
		noise_histogram_rep_cond = ie[2];
		noise_histogram_ref_val = ie[3];
	}

	anpi = radio_info.Noise;
	start_time = radio_info.tsf_start_time;

	/* the rx antenna id is 1 in case of 1 antenna exist
	 * or unique configuration of multiple antennas */
	ant_id = radio_info.num_rx_antennas;

	if ((noise_histogram_rep_cond == 1) && (noise_histogram_ref_val > anpi)) {
		wpa_printf(MSG_DEBUG, "anpi is less than anpi ref val");
		return;
	}

	if ((noise_histogram_rep_cond == 2) && (noise_histogram_ref_val < anpi)) {
		wpa_printf(MSG_DEBUG, "anpi is greater than anpi ref val");
		return;
	}

	cur = ipi_density_str;
	end_str = ipi_density_str + 64;
	ret = sprintf_s(cur, end_str - cur, " ipi=");
	if (ret <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
		return;
	}
	cur += ret;

	for (i = 0; i < RRM_NOISE_HISTO_IPI_DENSITY_SIZE; i++) {
		ret = sprintf_s(cur, end_str - cur, "%d%s", ipi_density[i],
				i + 1 < RRM_NOISE_HISTO_IPI_DENSITY_SIZE ? "," : " ");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
		cur += ret;
	}

	if (!hapd->conf->rrm_manipulate_measurement) {
		hostapd_send_noise_histogram_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_ACCEPT, op_class,
				channel, start_time, measurement_duration_report, ant_id, anpi,
				ipi_density, NULL, NULL, NULL);
	} else {
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_NOISE_HISTOGRAM_REQUEST_RXED MACSTR
			" dialog_token=%d measurement_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%llu duration=%d antenna_id=%d anpi=%d%s",
			MAC2STR(sta_addr), token, measurement_token,
			MEASUREMENT_REPORT_MODE_ACCEPT, op_class, channel, start_time,
			measurement_duration_report, ant_id, anpi, ipi_density_str);
	}
}

static void hostapd_handle_noise_histogram_report(struct hostapd_data *hapd,
		u8 token, const u8 *pos, size_t len, const u8 *sta_addr)
{
	u8 op_class, channel, measurement_rep_mode, ant_id, anpi, i;
	u16 measurement_duration;
	u64 start_time;
	char ipi_density[64] = {};
	char *cur, *end;
	char wide_band_ch_switch[WIDE_BAND_CH_SWICTH_DEBUG_STR_LEN];
	int ret;

	wpa_printf(MSG_DEBUG, "noise histogram token %u len %zu", token, len);

	measurement_rep_mode = pos[1];
	if (measurement_rep_mode != 0) {
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_NOISE_HISTOGRAM_RXED MACSTR
				" dialog_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%d duration=%d antenna_id=%d anpi=%d ipi=0,0,0,0,0,0,0,0,0,0,0",
				MAC2STR(sta_addr), token, measurement_rep_mode,
				0, 0, 0, 0, 0, 0);
		return;
	}

	op_class = pos[3];
	channel = pos[4];
	start_time = WPA_GET_LE64(&pos[5]);
	measurement_duration = WPA_GET_LE16(&pos[13]);
	ant_id = pos[15];
	anpi = pos[16];

	cur = ipi_density;
	end = ipi_density + 64;
	ret = sprintf_s(cur, end - cur, " ipi=");
	if (ret <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
		return;
	}
	cur += ret;

	for (i = 0; i < RRM_NOISE_HISTO_IPI_DENSITY_SIZE; i++) {
		ret = sprintf_s(cur, end - cur, "%d%s", pos[17 + i],
				i + 1 < RRM_NOISE_HISTO_IPI_DENSITY_SIZE ? "," : " ");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
		cur += ret;
	}

	wide_band_ch_switch[0] = '\0';
	if (pos[28] == 163 && pos[29] == 5 && pos[30] == 194 && pos[31] == 3) {
		ret = sprintf_s(wide_band_ch_switch, WIDE_BAND_CH_SWICTH_DEBUG_STR_LEN,
			" wide_band_ch_switch=%d,%d,%d", pos[32], pos[33], pos[34]);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
	}

	wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_NOISE_HISTOGRAM_RXED MACSTR
			" dialog_token=%d measurement_rep_mode=%d op_class=%d channel=%d start_time=%llu duration=%d antenna_id=%d anpi=%d%s%s",
			MAC2STR(sta_addr), token, measurement_rep_mode, op_class, channel,
			start_time, measurement_duration, ant_id, anpi, ipi_density,
			wide_band_ch_switch);
}

static void build_statistics_group_str(char *statistics_group_data_str,
		u32 *statistics_group_data, int statistics_group_data_len)
{
	char *cur, *end;
	int ret, i;
	cur = statistics_group_data_str;
	end = statistics_group_data_str + 128;
	ret = sprintf_s(cur, end - cur, " statistics_group_data=");
	if (ret <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
		return;
	}
	cur += ret;

	for (i = 0; i < statistics_group_data_len; i++) {
		ret = sprintf_s(cur, end - cur, "%d%s", statistics_group_data[i],
				i + 1 < statistics_group_data_len ? "," : " ");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
		cur += ret;
	}
}

static void hostapd_handle_sta_statistics_request(struct hostapd_data *hapd,
		u8 token, const u8 *pos, size_t len, const u8 *sta_addr,
		u16 num_of_repetition)
{
	errno_t err;
	u8 peer_addr[ETH_ALEN];
	u8 measurement_token, group_identity;
	u16 measurement_duration_report = 0;
	u32 statistics_group_data[RRM_STA_STATISTICS_GROUP_DATA_SIZE] = {};
	u32 statistics_group_data_len = 0;
	char statistics_group_data_str[128] = {};
	struct intel_vendor_vap_info vap_info;

	measurement_token = pos[0];
	err = memcpy_s(peer_addr, sizeof(peer_addr), &pos[3], ETH_ALEN);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return;
	}
	group_identity = pos[12];

	if (!(hapd->conf->radio_measurements[1] & WLAN_RRM_CAPS_STATISTICS_MEASUREMENT)) {
		wpa_printf(MSG_ERROR, "sta statistics in RRM is not supported");
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE,
				MEASURE_TYPE_STA_STATISTICS);
		return;
	}

	if (hostapd_drv_get_vap_measurements(hapd, &vap_info)) {
		wpa_printf(MSG_ERROR, "failed to get vap measurements");
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE,
				MEASURE_TYPE_STA_STATISTICS);
		return;
	}

	statistics_group_data_len =
			hostapd_get_statistics_group_data_len(group_identity);
	if (group_identity == 1) {
		statistics_group_data[0] = vap_info.RetryCount;
		statistics_group_data[1] = vap_info.MultipleRetryCount;
		statistics_group_data[2] = vap_info.FrameDuplicateCount;
		statistics_group_data[3] = vap_info.RTSSuccessCount;
		statistics_group_data[4] = vap_info.RTSFailureCount;
		statistics_group_data[5] = vap_info.ACKFailureCount;
		build_statistics_group_str(statistics_group_data_str,
				statistics_group_data, statistics_group_data_len);
	} else if (group_identity == 11) {
		statistics_group_data[0] = vap_info.TransmittedAMSDUCount;
		statistics_group_data[1] = vap_info.FailedAMSDUCount;
		statistics_group_data[2] = vap_info.RetryCount;
		statistics_group_data[3] = vap_info.MultipleRetryCount;
		err = memcpy_s(&statistics_group_data[4],
			 (sizeof(statistics_group_data) - (4 * sizeof(statistics_group_data[0]))),
			 &vap_info.TransmittedOctetsInAMSDUCount,
			 sizeof(vap_info.TransmittedOctetsInAMSDUCount));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}
		statistics_group_data[6] = vap_info.AMSDUAckFailureCount;
		statistics_group_data[7] = vap_info.ReceivedAMSDUCount;
		err = memcpy_s(&statistics_group_data[8],
			 (sizeof(statistics_group_data) - (8 * sizeof(statistics_group_data[0]))),
			 &vap_info.ReceivedOctetsInAMSDUCount,
			 sizeof(vap_info.ReceivedOctetsInAMSDUCount));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}
		build_statistics_group_str(statistics_group_data_str,
						statistics_group_data, statistics_group_data_len);
	} else if (group_identity == 12) {
		statistics_group_data[0] = vap_info.TransmittedAMPDUCount;
		statistics_group_data[1] = vap_info.TransmittedMPDUsInAMPDUCount;
		err = memcpy_s(&statistics_group_data[2],
			 (sizeof(statistics_group_data) - (2 * sizeof(statistics_group_data[0]))),
			 &vap_info.TransmittedOctetsInAMPDUCount,
			 sizeof(vap_info.TransmittedOctetsInAMPDUCount));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}
		statistics_group_data[4] = vap_info.AMPDUReceivedCount;
		statistics_group_data[5] = vap_info.MPDUInReceivedAMPDUCount;
		err = memcpy_s(&statistics_group_data[6],
			 (sizeof(statistics_group_data) - (6 * sizeof(statistics_group_data[0]))),
			 &vap_info.ReceivedOctetsInAMPDUCount,
			 sizeof(vap_info.ReceivedOctetsInAMPDUCount));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}
		statistics_group_data[8] = 0; /* AMPDUDelimiterCRCErrorCount unknown */
		build_statistics_group_str(statistics_group_data_str,
						statistics_group_data, statistics_group_data_len);
	} else if (group_identity == 13) {
		statistics_group_data[0] = vap_info.ImplicitBARFailureCount;
		statistics_group_data[1] = vap_info.ExplicitBARFailureCount;
		statistics_group_data[2] = vap_info.SwitchChannel20To40
								 + vap_info.SwitchChannel40To20;
		statistics_group_data[3] = vap_info.TwentyMHzFrameTransmittedCount;
		statistics_group_data[4] = vap_info.FortyMHzFrameTransmittedCount;
		statistics_group_data[5] = 0; /* TwentyMHzFrameReceivedCount unknown */
		statistics_group_data[6] = 0; /* FortyMHzFrameReceivedCount unknown */
		statistics_group_data[7] = 0; /* PSMPUTTGrantDuration unknown */
		statistics_group_data[8] = 0; /* PSMPUTTUsedDuration unknown */
		build_statistics_group_str(statistics_group_data_str,
						statistics_group_data, statistics_group_data_len);
	} else {
		wpa_printf(MSG_ERROR, "group_identity number %d is not supported",
				group_identity);
		hostapd_send_general_rrm_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_REJECT_REFUSED,
				MEASURE_TYPE_STA_STATISTICS);
		return;
	}

	if (!hapd->conf->rrm_manipulate_measurement) {
		hostapd_send_sta_statistics_report(hapd, sta_addr, token,
				measurement_token, MEASUREMENT_REPORT_MODE_ACCEPT,
				measurement_duration_report, group_identity,
				statistics_group_data, statistics_group_data_len, NULL);
	} else {
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_STA_STATISTICS_REQUEST_RXED MACSTR
			" dialog_token=%d measurement_token=%d measurement_rep_mode=%d duration=%d group_identity=%d%s",
			MAC2STR(sta_addr), token, measurement_token,
			MEASUREMENT_REPORT_MODE_ACCEPT, measurement_duration_report,
			group_identity, statistics_group_data_str);
	}
}

static void hostapd_handle_sta_statistics_report(struct hostapd_data *hapd,
		u8 token, const u8 *pos, size_t len, const u8 *sta_addr)
{
	u8 group_identity, measurement_rep_mode;
	u16 measurement_duration;
	char statistics_group_data[128], rep_reason[30];
	char *cur, *end;
	int i, ret, statistics_group_data_len;

	wpa_printf(MSG_DEBUG, "sta statistics token %u len %zu", token, len);

	measurement_rep_mode = pos[1];
	if (measurement_rep_mode != 0) {
		wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_STA_STATISTICS_RXED MACSTR
				" dialog_token=%d measurement_rep_mode=%d duration=%d group_identity=%d statistics_group_data=0,0,0,0,0,0,0",
				MAC2STR(sta_addr), token, measurement_rep_mode, 0, 0);
		return;
	}

	measurement_duration = WPA_GET_LE16(&pos[3]);
	group_identity = pos[5];

	statistics_group_data_len =
			hostapd_get_statistics_group_data_len(group_identity);

	cur = statistics_group_data;
	end = statistics_group_data + 128;
	ret = sprintf_s(cur, end - cur, " statistics_group_data=");
	if (ret <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
		return;
	}
	cur += ret;

	for (i = 0; i < statistics_group_data_len; i++) {
		ret = sprintf_s(cur, end - cur, "%d%s", pos[6 + i],
				i + 1 < statistics_group_data_len ? "," : " ");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
		cur += ret;
	}

	rep_reason[0] = '\0';
	if ((pos[6 + statistics_group_data_len] == 1)
			&& (pos[7 + statistics_group_data_len] == 1)) {
		ret = sprintf_s(rep_reason, sizeof(rep_reason),
			  " rep_reason=%d", pos[8 + statistics_group_data_len]);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
	}

	wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_STA_STATISTICS_RXED MACSTR
			" dialog_token=%d measurement_rep_mode=%d duration=%d group_identity=%d%s%s",
			MAC2STR(sta_addr), token, measurement_rep_mode,
			measurement_duration, group_identity, statistics_group_data,
			rep_reason);
}
static void hostapd_handle_radio_msmt_request(struct hostapd_data *hapd,
					     const u8 *buf, size_t len)
{
	const struct ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *) buf;
	const u8 *pos, *ie, *end;
	u8 token;
	u16 num_of_repetition;

	end = buf + len;
	token = mgmt->u.action.u.rrm.dialog_token;
	num_of_repetition = WPA_GET_LE16(mgmt->u.action.u.rrm.variable);
	pos = mgmt->u.action.u.rrm.variable;

	while ((ie = get_ie(pos, end - pos, WLAN_EID_MEASURE_REQUEST))) {
		if (ie[1] < 3) {
			wpa_printf(MSG_DEBUG, "Bad Measurement Request element");
			break;
		}

		wpa_printf(MSG_DEBUG, "Measurement request type %u", ie[4]);

		switch (ie[4]) {
		case MEASURE_TYPE_CHANNEL_LOAD:
			hostapd_handle_channel_load_request(hapd, token, ie + 2, ie[1],
					mgmt->sa, num_of_repetition);
			break;
		case MEASURE_TYPE_NOISE_HIST:
			hostapd_handle_noise_histogram_request(hapd, token, ie + 2, ie[1],
					mgmt->sa, num_of_repetition);
			break;
		case MEASURE_TYPE_BEACON:
			hostapd_handle_beacon_report_request(hapd, token, ie + 2, ie[1],
					mgmt->sa, num_of_repetition);
			break;
		case MEASURE_TYPE_STA_STATISTICS:
			hostapd_handle_sta_statistics_request(hapd, token, ie + 2, ie[1],
					mgmt->sa, num_of_repetition);
			break;
		default:
			wpa_printf(MSG_DEBUG,
				   "Measurement request type %u is not supported", ie[4]);
			break;
		}

		pos = ie + ie[1] + 2;
	}
}
static void hostapd_handle_radio_msmt_report(struct hostapd_data *hapd,
					     const u8 *buf, size_t len)
{
	const struct ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *) buf;
	const u8 *pos, *ie, *end;
	u8 token, rep_mode;

	end = buf + len;
	token = mgmt->u.action.u.rrm.dialog_token;
	pos = mgmt->u.action.u.rrm.variable;

	while ((ie = get_ie(pos, end - pos, WLAN_EID_MEASURE_REPORT))) {
		if (ie[1] < 3) {
			wpa_printf(MSG_DEBUG, "Bad Measurement Report element");
			break;
		}

		UNUSED_VAR(rep_mode);
		rep_mode = ie[3];
		wpa_printf(MSG_DEBUG, "Measurement report mode 0x%x type %u", rep_mode,ie[4]);

		switch (ie[4]) {
		case MEASURE_TYPE_LCI:
			hostapd_handle_lci_report(hapd, token, ie + 2, ie[1]);
			break;
		case MEASURE_TYPE_FTM_RANGE:
			hostapd_handle_range_report(hapd, token, ie + 2, ie[1]);
			break;
		case MEASURE_TYPE_STA_STATISTICS:
			hostapd_handle_sta_statistics_report(hapd, token, ie + 2, ie[1],
					mgmt->sa);
			break;
		case MEASURE_TYPE_BEACON:
			hostapd_handle_beacon_report_response(hapd, token, ie + 2, ie[1],
					mgmt->sa);
			break;
		case MEASURE_TYPE_CHANNEL_LOAD:
			hostapd_handle_channel_load_report(hapd, token, ie + 2, ie[1],
					mgmt->sa);
			break;
		case MEASURE_TYPE_NOISE_HIST:
			hostapd_handle_noise_histogram_report(hapd, token, ie + 2, ie[1],
					mgmt->sa);
			break;
		default:
			wpa_printf(MSG_DEBUG,
				   "Measurement report type %u is not supported", ie[4]);
			break;
		}

		pos = ie + ie[1] + 2;
	}
}

static void hostapd_handle_link_msmt_report(struct hostapd_data *hapd,
					     const u8 *buf, size_t len)
{
	errno_t err;
	int ret;
	const struct ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *) buf;
	const u8 *pos, *ie, *end;
	u8 token;
	struct rrm_link_measurement_report link_msmt_report;
	struct dmg_link_margin dmg_margin;
	struct dmg_link_adaptation_ack dmg_adapt;
	char dmg_margin_str[50], dmg_adapt_str[50];

	end = buf + len;
	token = mgmt->u.action.u.rrm.dialog_token;
	pos = mgmt->u.action.u.rrm.variable;

	wpa_printf(MSG_DEBUG, "link measurement report token %u len %zu",
			token, len);

	err = memcpy_s(&link_msmt_report.tpc, sizeof(link_msmt_report.tpc), pos, 4);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return;
	}
	link_msmt_report.rx_ant_id = pos[4];
	link_msmt_report.tx_ant_id = pos[5];
	link_msmt_report.rcpi = pos[6];
	link_msmt_report.rsni = pos[7];

	dmg_margin_str[0] = '\0';
	if ((ie = get_ie(pos, end - pos, WLAN_EID_DMG_LINK_MARGIN))
			&& (ie[1] == 8)) {
		err = memcpy_s(&dmg_margin, sizeof(dmg_margin), ie, 10);
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}
		ret = sprintf_s(dmg_margin_str, sizeof(dmg_margin_str),
			  " dmg_link_margin=%d,%d,%d,%d,%d", dmg_margin.activity,
			  dmg_margin.mcs, dmg_margin.link_margin, dmg_margin.snr,
			  dmg_margin.ref_timestamp);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
	}

	dmg_adapt_str[0] = '\0';
	if ((ie = get_ie(pos, end - pos, WLAN_EID_DMG_LINK_ADAPTATION_ACK))
			&& (ie[1] == 5)) {
		err = memcpy_s(&dmg_adapt, sizeof(dmg_adapt), ie, 7);
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}
		ret = sprintf_s(dmg_adapt_str, sizeof(dmg_adapt_str),
			  " dmg_link_adapt_ack=%d,%d", dmg_adapt.activity,
			  dmg_adapt.ref_timestamp);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return;
		}
	}

	wpa_msg(hapd->msg_ctx, MSG_INFO, RRM_EVENT_LINK_MEASUREMENT_RXED MACSTR
			" dialog_token=%d tpc_report=%d,%d rx_ant_id=%d tx_ant_id=%d rcpi=%d rsni=%d%s%s",
			MAC2STR(mgmt->sa), token, link_msmt_report.tpc.tx_power,
			link_msmt_report.tpc.link_margin, link_msmt_report.rx_ant_id,
			link_msmt_report.tx_ant_id, link_msmt_report.rcpi,
			link_msmt_report.rsni, dmg_margin_str, dmg_adapt_str);
}

static void hostapd_handle_link_msmt_request(struct hostapd_data *hapd,
					     const u8 *buf, size_t len, int ssi_signal)
{
	struct hostapd_iface *iface = hapd->iface;
	struct hostapd_hw_modes *mode = iface->current_mode;
	const struct ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *) buf;
	int i;
	u8 token;

	struct rrm_link_measurement_report link_msmt_report;
	struct intel_vendor_radio_info radio_info;

	os_memset(&link_msmt_report, 0, sizeof(link_msmt_report));

	token = mgmt->u.action.u.rrm.dialog_token;

	if (!(hapd->conf->radio_measurements[0] & WLAN_RRM_CAPS_LINK_MEASUREMENT)) {
		wpa_printf(MSG_DEBUG, "AP not support link measurement, token %u",
			   token);
		return;
	}

	if (!(hapd->iface->drv_flags & WPA_DRIVER_FLAGS_TX_POWER_INSERTION)) {
		wpa_printf(MSG_DEBUG, "TX power insertion not supported, token %u",
			   token);
		return;
	}

	wpa_printf(MSG_DEBUG, "link measurement request token %u len %zu",
			token, len);

	link_msmt_report.dialog_token = token;
	link_msmt_report.tpc.eid = WLAN_EID_TPC_REPORT;
	link_msmt_report.tpc.len = 2;
	link_msmt_report.tpc.link_margin = 0; /* unknown */

	for (i = 0; i < mode->num_channels; i++) {
		if (mode->channels[i].chan == iface->conf->channel) {
			link_msmt_report.tpc.tx_power = mode->channels[i].max_tx_power;
			break;
		}
	}

	if (hostapd_drv_get_radio_info(hapd, &radio_info)) {
		wpa_printf(MSG_ERROR, "failed to get radio measurements");
		return;
	}

	if (radio_info.num_tx_antennas == 1)
		link_msmt_report.tx_ant_id = 1;
	else if (radio_info.num_tx_antennas > 1)
		link_msmt_report.tx_ant_id = 255; /* multiple antennas */
	/* the rx antenna id is 1 in case of 1 antenna exist
	 * or unique configuration of multiple antennas */
	link_msmt_report.rx_ant_id = radio_info.num_rx_antennas;

	link_msmt_report.rcpi = rssi_to_rcpi(ssi_signal);
	link_msmt_report.rsni = 255; /* 255 indicates that RSNI is not available */

	if (!hapd->conf->rrm_manipulate_measurement) {
		hostapd_send_link_measurement_report(hapd, mgmt->sa, link_msmt_report,
				NULL, NULL);
	} else {
		wpa_msg(hapd->msg_ctx, MSG_INFO,
				RRM_EVENT_LINK_MEASUREMENT_REQUEST_RXED MACSTR
				" dialog_token=%d rx_ant_id=%d tx_ant_id=%d rcpi=%d rsni=%d tpc_report=%d,%d",
				MAC2STR(mgmt->sa), link_msmt_report.dialog_token,
				link_msmt_report.rx_ant_id, link_msmt_report.tx_ant_id,
				link_msmt_report.rcpi, link_msmt_report.rsni,
				link_msmt_report.tpc.tx_power,
				link_msmt_report.tpc.link_margin);
	}
}

static u16 hostapd_parse_location_lci_req_age(const u8 *buf, size_t len)
{
	const u8 *subelem;

	/* Range Request element + Location Subject + Maximum Age subelement */
	if (len < 3 + 1 + 4)
		return 0;

	/* Subelements are arranged as IEs */
	subelem = get_ie(buf + 4, len - 4, LCI_REQ_SUBELEM_MAX_AGE);
	if (subelem && subelem[1] == 2)
		return WPA_GET_LE16(subelem + 2);

	return 0;
}


static int hostapd_check_lci_age(struct hostapd_neighbor_entry *nr, u16 max_age)
{
	struct os_time curr, diff;
	unsigned long diff_l;

	if (nr->stationary || max_age == 0xffff)
		return 1;

	if (!max_age)
		return 0;

	if (os_get_time(&curr))
		return 0;

	os_time_sub(&curr, &nr->lci_date, &diff);

	/* avoid overflow */
	if (diff.sec > 0xffff)
		return 0;

	/* LCI age is calculated in 10th of a second units. */
	diff_l = diff.sec * 10 + diff.usec / 100000;

	return max_age > diff_l;
}


static size_t hostapd_neighbor_report_len(struct wpabuf *buf,
					  struct hostapd_neighbor_entry *nr,
					  int send_lci, int send_civic)
{
	size_t len = 2 + wpabuf_len(nr->nr);

	if (send_lci && nr->lci)
		len += 2 + wpabuf_len(nr->lci);

	if (send_civic && nr->civic)
		len += 2 + wpabuf_len(nr->civic);

	return len;
}


static void hostapd_send_nei_report_resp(struct hostapd_data *hapd,
					 const u8 *addr, u8 dialog_token,
					 struct wpa_ssid_value *ssid, u8 lci,
					 u8 civic, u16 lci_max_age)
{
	struct hostapd_neighbor_entry *nr;
	struct wpabuf *buf;
	u8 *msmt_token;

	/*
	 * The number and length of the Neighbor Report elements in a Neighbor
	 * Report frame is limited by the maximum allowed MMPDU size; + 3 bytes
	 * of RRM header.
	 */
	buf = wpabuf_alloc(3 + IEEE80211_MAX_MMPDU_SIZE);
	if (!buf)
		return;

	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_NEIGHBOR_REPORT_RESPONSE);
	wpabuf_put_u8(buf, dialog_token);

	dl_list_for_each(nr, &hapd->nr_db, struct hostapd_neighbor_entry,
			 list) {
		int send_lci;
		size_t len;

		if ((ssid->ssid_len != 0)
		    && (ssid->ssid_len != nr->ssid.ssid_len ||
		    os_memcmp(ssid->ssid, nr->ssid.ssid, ssid->ssid_len) != 0))
			continue;

		send_lci = (lci != 0) && hostapd_check_lci_age(nr, lci_max_age);
		len = hostapd_neighbor_report_len(buf, nr, send_lci, civic);

		if (len - 2 > 0xff) {
			wpa_printf(MSG_DEBUG,
				   "NR entry for " MACSTR " exceeds 0xFF bytes",
				   MAC2STR(nr->bssid));
			continue;
		}

		if (len > wpabuf_tailroom(buf))
			break;

		wpabuf_put_u8(buf, WLAN_EID_NEIGHBOR_REPORT);
		wpabuf_put_u8(buf, len - 2);
		wpabuf_put_buf(buf, nr->nr);

		if (send_lci && nr->lci) {
			wpabuf_put_u8(buf, WLAN_EID_MEASURE_REPORT);
			wpabuf_put_u8(buf, wpabuf_len(nr->lci));
			/*
			 * Override measurement token - the first byte of the
			 * Measurement Report element.
			 */
			msmt_token = wpabuf_put(buf, 0);
			wpabuf_put_buf(buf, nr->lci);
			*msmt_token = lci;
		}

		if (civic && nr->civic) {
			wpabuf_put_u8(buf, WLAN_EID_MEASURE_REPORT);
			wpabuf_put_u8(buf, wpabuf_len(nr->civic));
			/*
			 * Override measurement token - the first byte of the
			 * Measurement Report element.
			 */
			msmt_token = wpabuf_put(buf, 0);
			wpabuf_put_buf(buf, nr->civic);
			*msmt_token = civic;
		}
	}

	hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
}


static void hostapd_handle_nei_report_req(struct hostapd_data *hapd,
					  const u8 *buf, size_t len)
{
	const struct ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *) buf;
	const u8 *pos, *ie, *end;
	struct wpa_ssid_value ssid = {
		.ssid_len = 0
	};
	u8 token;
	u8 lci = 0, civic = 0; /* Measurement tokens */
	u16 lci_max_age = 0;

	if (!(hapd->conf->radio_measurements[0] &
	      WLAN_RRM_CAPS_NEIGHBOR_REPORT))
		return;

	end = buf + len;

	token = mgmt->u.action.u.rrm.dialog_token;
	pos = mgmt->u.action.u.rrm.variable;
	len = end - pos;

	ie = get_ie(pos, len, WLAN_EID_SSID);
	if (ie && ie[1] && ie[1] <= SSID_MAX_LEN) {
		ssid.ssid_len = ie[1];
		os_memcpy(ssid.ssid, ie + 2, ssid.ssid_len);
	}
	/*
	    See 9.4.2.36 (Neighbor Report element))
	    that include at least the SSID information
	    of all the co-located APs operating in the 6 GHz band.
	*/

	while ((ie = get_ie(pos, len, WLAN_EID_MEASURE_REQUEST))) {
		if (ie[1] < 3)
			break;

		wpa_printf(MSG_DEBUG,
			   "Neighbor report request, measure type %u",
			   ie[4]);

		switch (ie[4]) { /* Measurement Type */
		case MEASURE_TYPE_LCI:
			lci = ie[2]; /* Measurement Token */
			lci_max_age = hostapd_parse_location_lci_req_age(ie + 2,
									 ie[1]);
			break;
		case MEASURE_TYPE_LOCATION_CIVIC:
			civic = ie[2]; /* Measurement token */
			break;
		}

		pos = ie + ie[1] + 2;
		len = end - pos;
	}

	hostapd_send_nei_report_resp(hapd, mgmt->sa, token, &ssid, lci, civic,
				     lci_max_age);
}


void hostapd_handle_radio_measurement(struct hostapd_data *hapd,
				      const u8 *buf, size_t len, int ssi_signal)
{
	const struct ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *) buf;

	/*
	 * Check for enough bytes: header + (1B)Category + (1B)Action +
	 * (1B)Dialog Token.
	 */
	if (len < IEEE80211_HDRLEN + 3)
		return;

	wpa_printf(MSG_DEBUG, "Radio measurement frame, action %u from " MACSTR,
		   mgmt->u.action.u.rrm.action, MAC2STR(mgmt->sa));

	switch (mgmt->u.action.u.rrm.action) {
	case WLAN_RRM_RADIO_MEASUREMENT_REQUEST:
		hostapd_handle_radio_msmt_request(hapd, buf, len);
		break;
	case WLAN_RRM_RADIO_MEASUREMENT_REPORT:
		hostapd_handle_radio_msmt_report(hapd, buf, len);
		break;
	case WLAN_RRM_LINK_MEASUREMENT_REQUEST:
		hostapd_handle_link_msmt_request(hapd, buf, len, ssi_signal);
		break;
	case WLAN_RRM_LINK_MEASUREMENT_REPORT:
		hostapd_handle_link_msmt_report(hapd, buf, len);
		break;
	case WLAN_RRM_NEIGHBOR_REPORT_REQUEST:
		hostapd_handle_nei_report_req(hapd, buf, len);
		break;
	default:
		wpa_printf(MSG_DEBUG, "RRM action %u is not supported",
			   mgmt->u.action.u.rrm.action);
		break;
	}
}


int hostapd_send_lci_req(struct hostapd_data *hapd, const u8 *addr)
{
	struct wpabuf *buf;
	struct sta_info *sta = ap_get_sta(hapd, addr);
	int ret;

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_INFO,
			   "Request LCI: Destination address is not connected");
		return -1;
	}

	if (!(sta->rrm_enabled_capa[1] & WLAN_RRM_CAPS_LCI_MEASUREMENT)) {
		wpa_printf(MSG_INFO,
			   "Request LCI: Station does not support LCI in RRM");
		return -1;
	}

	if (hapd->lci_req_active) {
		wpa_printf(MSG_DEBUG,
			   "Request LCI: LCI request is already in process, overriding");
		hapd->lci_req_active = 0;
		eloop_cancel_timeout(hostapd_lci_rep_timeout_handler, hapd,
				     NULL);
	}

	/* Measurement request (5) + Measurement element with LCI (10) */
	buf = wpabuf_alloc(5 + 10);
	if (!buf)
		return -1;

	hapd->lci_req_token++;
	/* For wraparounds - the token must be nonzero */
	if (!hapd->lci_req_token)
		hapd->lci_req_token++;

	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_RADIO_MEASUREMENT_REQUEST);
	wpabuf_put_u8(buf, hapd->lci_req_token);
	wpabuf_put_le16(buf, 0); /* Number of repetitions */

	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REQUEST);
	wpabuf_put_u8(buf, 3 + 1 + 4);

	wpabuf_put_u8(buf, 1); /* Measurement Token */
	/*
	 * Parallel and Enable bits are 0, Duration, Request, and Report are
	 * reserved.
	 */
	wpabuf_put_u8(buf, 0);
	wpabuf_put_u8(buf, MEASURE_TYPE_LCI);

	wpabuf_put_u8(buf, LOCATION_SUBJECT_REMOTE);

	wpabuf_put_u8(buf, LCI_REQ_SUBELEM_MAX_AGE);
	wpabuf_put_u8(buf, 2);
	wpabuf_put_le16(buf, 0xffff);

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
	if (ret)
		return ret;

	hapd->lci_req_active = 1;

	eloop_register_timeout(HOSTAPD_RRM_REQUEST_TIMEOUT, 0,
			       hostapd_lci_rep_timeout_handler, hapd, NULL);

	return 0;
}


int hostapd_send_range_req(struct hostapd_data *hapd, const u8 *addr,
			   u16 random_interval, u8 min_ap,
			   const u8 *responders, unsigned int n_responders)
{
	struct wpabuf *buf;
	struct sta_info *sta;
	u8 *len;
	unsigned int i;
	int ret;

	wpa_printf(MSG_DEBUG, "Request range: dest addr " MACSTR
		   " rand interval %u min AP %u n_responders %u", MAC2STR(addr),
		   random_interval, min_ap, n_responders);

	if (min_ap == 0 || min_ap > n_responders) {
		wpa_printf(MSG_INFO, "Request range: Wrong min AP count");
		return -1;
	}

	sta = ap_get_sta(hapd, addr);
	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_INFO,
			   "Request range: Destination address is not connected");
		return -1;
	}

	if (!(sta->rrm_enabled_capa[4] & WLAN_RRM_CAPS_FTM_RANGE_REPORT)) {
		wpa_printf(MSG_ERROR,
			   "Request range: Destination station does not support FTM range report in RRM");
		return -1;
	}

	if (hapd->range_req_active) {
		wpa_printf(MSG_DEBUG,
			   "Request range: Range request is already in process; overriding");
		hapd->range_req_active = 0;
		eloop_cancel_timeout(hostapd_range_rep_timeout_handler, hapd,
				     NULL);
	}

	/* Action + measurement type + token + reps + EID + len = 7 */
	buf = wpabuf_alloc(7 + 255);
	if (!buf)
		return -1;

	hapd->range_req_token++;
	if (!hapd->range_req_token) /* For wraparounds */
		hapd->range_req_token++;

	/* IEEE P802.11-REVmc/D5.0, 9.6.7.2 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_RADIO_MEASUREMENT_REQUEST);
	wpabuf_put_u8(buf, hapd->range_req_token); /* Dialog Token */
	wpabuf_put_le16(buf, 0); /* Number of Repetitions */

	/* IEEE P802.11-REVmc/D5.0, 9.4.2.21 */
	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REQUEST);
	len = wpabuf_put(buf, 1); /* Length will be set later */

	wpabuf_put_u8(buf, 1); /* Measurement Token */
	/*
	 * Parallel and Enable bits are 0; Duration, Request, and Report are
	 * reserved.
	 */
	wpabuf_put_u8(buf, 0); /* Measurement Request Mode */
	wpabuf_put_u8(buf, MEASURE_TYPE_FTM_RANGE); /* Measurement Type */

	/* IEEE P802.11-REVmc/D5.0, 9.4.2.21.19 */
	wpabuf_put_le16(buf, random_interval); /* Randomization Interval */
	wpabuf_put_u8(buf, min_ap); /* Minimum AP Count */

	/* FTM Range Subelements */

	/*
	 * Taking the neighbor report part of the range request from neighbor
	 * database instead of requesting the separate bits of data from the
	 * user.
	 */
	for (i = 0; i < n_responders; i++) {
		struct hostapd_neighbor_entry *nr;

		nr = hostapd_neighbor_get(hapd, responders + ETH_ALEN * i,
					  NULL);
		if (!nr) {
			wpa_printf(MSG_INFO, "Missing neighbor report for "
				   MACSTR, MAC2STR(responders + ETH_ALEN * i));
			wpabuf_free(buf);
			return -1;
		}

		if (wpabuf_tailroom(buf) < 2 + wpabuf_len(nr->nr)) {
			wpa_printf(MSG_ERROR, "Too long range request");
			wpabuf_free(buf);
			return -1;
		}

		wpabuf_put_u8(buf, WLAN_EID_NEIGHBOR_REPORT);
		wpabuf_put_u8(buf, wpabuf_len(nr->nr));
		wpabuf_put_buf(buf, nr->nr);
	}

	/* Action + measurement type + token + reps + EID + len = 7 */
	*len = wpabuf_len(buf) - 7;

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
	if (ret)
		return ret;

	hapd->range_req_active = 1;

	eloop_register_timeout(HOSTAPD_RRM_REQUEST_TIMEOUT, 0,
			       hostapd_range_rep_timeout_handler, hapd, NULL);

	return 0;
}



static void hostapd_build_beacon_report_resp(struct wpabuf *buf,
		u8 measurement_token, u8 measurement_rep_mode, u8 op_class, u8 channel,
		u64 start_time, u16 measurement_duration, u8 frame_info, u8 rcpi,
		u8 rsni, const u8 *bssid, u8 antenna_id, u32 parent_tsf, u8 *ch_width,
		u8 *ch_center_freq0, u8 *ch_center_freq1, u8 *rep_frame_body,
		u8 *rep_frame_body_length)
{
	u8 *len;

	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REPORT);
	len = wpabuf_put(buf, 1); /* Length will be set later */
	wpabuf_put_u8(buf, measurement_token);
	wpabuf_put_u8(buf, measurement_rep_mode); /* report mode */
	wpabuf_put_u8(buf, MEASURE_TYPE_BEACON); /* report type */

	wpabuf_put_u8(buf, op_class); /* op class */
	wpabuf_put_u8(buf, channel); /* channel number */
	wpabuf_put_data(buf, &start_time, 8); /* start time */
	wpabuf_put_le16(buf, measurement_duration); /* duration */
	wpabuf_put_u8(buf, frame_info); /* frame info */
	wpabuf_put_u8(buf, rcpi); /* rcpi */
	wpabuf_put_u8(buf, rsni); /* rsni */
	wpabuf_put_data(buf, bssid, ETH_ALEN); /* bssid */
	wpabuf_put_u8(buf, antenna_id); /* antenna id */
	wpabuf_put_be32(buf, parent_tsf); /* parent TSF */

	if (ch_width && ch_center_freq0 && ch_center_freq1) {
		wpabuf_put_u8(buf, 163); /* wide bandwidth channel switch sub element id */
		wpabuf_put_u8(buf, 5);   /* sub element length */
		wpabuf_put_u8(buf, 194); /* wide bandwidth channel switch element id */
		wpabuf_put_u8(buf, 3); 	 /* element length */
		wpabuf_put_u8(buf, *ch_width);
		wpabuf_put_u8(buf, *ch_center_freq0);
		wpabuf_put_u8(buf, *ch_center_freq1);
	}

	if (rep_frame_body && rep_frame_body_length) {
		wpabuf_put_u8(buf, 1); /* reported frame body subelemnt id */
		wpabuf_put_u8(buf, *rep_frame_body_length);
		wpabuf_put_data(buf, rep_frame_body, *rep_frame_body_length);
	}

	/* Action + measurement type + token + EID + len = 5 */
	*len = wpabuf_len(buf) - 5;
}

static int hostapd_send_beacon_report_with_header(struct hostapd_data *hapd,
		const u8 *addr, const u8* data, int len, u8 dialog_token)
{
	struct wpabuf *report;
	struct sta_info *sta = NULL;
	int ret, i;

	wpa_printf(MSG_DEBUG, "beacon report response: dest addr: " MACSTR
			,MAC2STR(addr));

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_ERROR,
			   "beacon report response: Destination address is not connected");
		return -1;
	}

	report = wpabuf_alloc(3 + len);
	if (!report)
		return -1;

	wpabuf_put_u8(report, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(report, WLAN_RRM_RADIO_MEASUREMENT_REPORT);
	wpabuf_put_u8(report, dialog_token);

	if (len)
		wpabuf_put_data(report, data, len);

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
					wpabuf_head(report), wpabuf_len(report));

	wpabuf_free(report);
	return ret;
}

int hostapd_send_beacon_report_resp(struct hostapd_data *hapd, const u8 *addr,
		u8 dialog_token, u8 measurement_token, u8 measurement_rep_mode,
		u8 op_class, u8 channel, u64 start_time, u16 measurement_duration,
		u8 frame_info, u8 rcpi, u8 rsni, const u8 *bssid, u8 antenna_id,
		u32 parent_tsf, u8 *ch_width, u8 *ch_center_freq0, u8 *ch_center_freq1,
		u8 *rep_frame_body, u8 *rep_frame_body_length)
{
	struct wpabuf *buf;
	int ret;
	int len;
	const u8 *pos;

	buf = wpabuf_alloc(3 + IEEE80211_MAX_MMPDU_SIZE);
	if (!buf)
		return -1;

	hostapd_build_beacon_report_resp(buf, measurement_token,
			measurement_rep_mode, op_class, channel, start_time,
			measurement_duration, frame_info, rcpi, rsni, bssid, antenna_id,
			parent_tsf, ch_width, ch_center_freq0, ch_center_freq1,
			rep_frame_body, rep_frame_body_length);

	len = wpabuf_len(buf);
	pos = wpabuf_head_u8(buf);

	ret = hostapd_send_beacon_report_with_header(hapd, addr, pos, len,
			dialog_token);

	wpabuf_free(buf);
	return ret;
}

void hostapd_clean_rrm(struct hostapd_data *hapd)
{
	hostapd_free_neighbor_db(hapd);
	eloop_cancel_timeout(hostapd_lci_rep_timeout_handler, hapd, NULL);
	hapd->lci_req_active = 0;
	eloop_cancel_timeout(hostapd_range_rep_timeout_handler, hapd, NULL);
	hapd->range_req_active = 0;
}

int hostapd_send_beacon_req(struct hostapd_data *hapd, const u8 *addr,
		u16 num_of_repetitions, u8 measurement_request_mode,
		u8 oper_class, u8 channel, u16 random_interval,
		u16 measurement_duration, u8 mode, const u8* bssid,
		struct wpa_ssid_value* ssid, u8* rep_cond, u8* rep_cond_threshold,
		u8* rep_detail,	const u8* ap_ch_rep, unsigned int ap_ch_rep_len,
		const u8* req_elem, unsigned int req_elem_len, u8 *ch_width,
		u8 *ch_center_freq0, u8 *ch_center_freq1, u8 last_indication)
{
	struct wpabuf *buf;
	struct sta_info *sta = NULL;
	u8 *len;
	int ret, i;
	const u8 wildcard_bssid[ETH_ALEN] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	};

	wpa_printf(MSG_DEBUG, "Request beacon: dest addr: " MACSTR ", mode: %d",
			MAC2STR(addr), mode);

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_ERROR,
			   "Request beacon: Destination address is not connected");
		return -1;
	}

	if ((mode == BEACON_REPORT_MODE_PASSIVE &&
		!(sta->rrm_enabled_capa[0] & WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE))
		|| (mode == BEACON_REPORT_MODE_ACTIVE &&
		!(sta->rrm_enabled_capa[0] & WLAN_RRM_CAPS_BEACON_REPORT_ACTIVE))
		|| (mode == BEACON_REPORT_MODE_TABLE &&
		!(sta->rrm_enabled_capa[0] & WLAN_RRM_CAPS_BEACON_REPORT_TABLE)))
	{
		wpa_printf(MSG_ERROR,
			   "Request beacon: Destination station does not support BEACON report (mode %d) in RRM",
			   mode);
		return -1;
	}

	if (channel == 255 && !ap_ch_rep) {
		wpa_printf(MSG_ERROR,
			   "Request beacon: channel set to 255, but no ap channel report data provided");
		return -1;
	}

	/* Measurement request (5) + Measurement element with beacon (18) + optional sub-elements (255)*/
	buf = wpabuf_alloc(5 + 18 + 255);
	if (!buf)
		return -1;

	hapd->beacon_req_token++;
	if (!hapd->beacon_req_token) /* For wraparounds */
		hapd->beacon_req_token++;

	/* IEEE P802.11-REVmc/D5.0, 9.6.7.2 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_RADIO_MEASUREMENT_REQUEST);
	wpabuf_put_u8(buf, hapd->beacon_req_token);
	wpabuf_put_le16(buf, num_of_repetitions);

	/* IEEE P802.11-REVmc/D5.0, 9.4.2.21 */
	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REQUEST);
	len = wpabuf_put(buf, 1); /* Length will be set later */

	wpabuf_put_u8(buf, hapd->beacon_req_token); /* Measurement Token */
	wpabuf_put_u8(buf, measurement_request_mode);
	wpabuf_put_u8(buf, MEASURE_TYPE_BEACON);

	/* IEEE P802.11-REVmc/D4.0, 8.4.2.20.7 */
	wpabuf_put_u8(buf, oper_class);
	wpabuf_put_u8(buf, channel);
	wpabuf_put_le16(buf, random_interval);
	wpabuf_put_le16(buf, measurement_duration);
	wpabuf_put_u8(buf, mode); /* Measurement Mode */
	if (!bssid) {
		/* use wildcard BSSID instead of a specific BSSID */
		bssid = wildcard_bssid;
	}
	wpabuf_put_data(buf, bssid, ETH_ALEN);

	/* optional sub-elements should go here */

	if (ssid) {
		wpabuf_put_u8(buf, WLAN_BEACON_REQUEST_SUBELEM_SSID);
		wpabuf_put_u8(buf, ssid->ssid_len);
		wpabuf_put_data(buf, ssid->ssid, ssid->ssid_len);
	}

	/*
	 * Note:
	 * The Beacon Reporting subelement indicates the condition for issuing a
	 * Beacon report. The Beacon Reporting subelement is optionally present in
	 * a Beacon request for repeated measurements; otherwise it is not present.
	 * Mandatory for MBO test plan, redundant according to specifications.
	 */
	if (rep_cond && *rep_cond <= 10 && rep_cond_threshold) {
		wpabuf_put_u8(buf, WLAN_BEACON_REQUEST_SUBELEM_INFO);
		wpabuf_put_u8(buf, 2);
		wpabuf_put_u8(buf, *rep_cond);
		wpabuf_put_u8(buf, *rep_cond_threshold);
	}

	if (rep_detail && (*rep_detail == 0 || *rep_detail == 1 || *rep_detail == 2)) {
		wpabuf_put_u8(buf, WLAN_BEACON_REQUEST_SUBELEM_DETAIL);
		wpabuf_put_u8(buf, 1);
		wpabuf_put_u8(buf, *rep_detail);
	}

	if (req_elem && req_elem_len) {
		wpabuf_put_u8(buf, WLAN_BEACON_REQUEST_SUBELEM_REQUEST);
		wpabuf_put_u8(buf, req_elem_len); /* size */
		wpabuf_put_data(buf, req_elem, req_elem_len); /* data */
	}

	/* in case channel is not 255, this IE is omitted */
	if (ap_ch_rep && ap_ch_rep_len && channel == 255) {
		wpabuf_put_u8(buf, WLAN_BEACON_REQUEST_SUBELEM_AP_CHANNEL);
		wpabuf_put_u8(buf, ap_ch_rep_len + 1);
		wpabuf_put_u8(buf, oper_class);
		wpabuf_put_data(buf, ap_ch_rep, ap_ch_rep_len);
	}

	if (ch_width && ch_center_freq0 && ch_center_freq1) {
		wpabuf_put_u8(buf, WLAN_BEACON_REQUEST_SUBELEM_WIDE_BW_CHSWITCH); /* wide bandwidth channel switch sub element id */
		wpabuf_put_u8(buf, 5);   /* sub element length */
		wpabuf_put_u8(buf, WLAN_EID_VHT_WIDE_BW_CHSWITCH); /* wide bandwidth channel switch element id */
		wpabuf_put_u8(buf, 3); 	 /* element length */
		wpabuf_put_u8(buf, *ch_width);
		wpabuf_put_u8(buf, *ch_center_freq0);
		wpabuf_put_u8(buf, *ch_center_freq1);
	}

	if (last_indication) {
		wpabuf_put_u8(buf, WLAN_BEACON_REQUEST_SUBELEM_LAST_INDICATION);
		wpabuf_put_u8(buf, 1); /* size */
		wpabuf_put_u8(buf, last_indication);
	}

	/* Action + measurement type + token + reps + EID + len = 7 */
	*len = wpabuf_len(buf) - 7;

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
	if (ret)
		return -1;

	return hapd->beacon_req_token;
}


void hostapd_rrm_beacon_req_tx_status(struct hostapd_data *hapd,
				      const struct ieee80211_mgmt *mgmt,
				      size_t len, int ok)
{
	if (len < 24 + 3)
		return;
	wpa_msg(hapd->msg_ctx, MSG_INFO, BEACON_REQ_TX_STATUS MACSTR
		" %u ack=%d", MAC2STR(mgmt->da),
		mgmt->u.action.u.rrm.dialog_token, ok);
}

int hostapd_send_link_measurement_req(struct hostapd_data *hapd, const u8 *addr)
{
	struct hostapd_iface *iface = hapd->iface;
	struct hostapd_hw_modes *mode = iface->current_mode;
	struct wpabuf *buf;
	struct sta_info *sta = NULL;
	int ret, i;
	s8 max_tx_power = 0, tx_power_used = 0;
	struct intel_vendor_sta_info sta_info;

	wpa_printf(MSG_DEBUG, "request link measurement: dest addr: " MACSTR
			,MAC2STR(addr));

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_ERROR,
			   "request link measurement: Destination address is not connected");
		return -1;
	}

	if (!(sta->rrm_enabled_capa[0] & WLAN_RRM_CAPS_LINK_MEASUREMENT)) {
		wpa_printf(MSG_ERROR,
			   "request link measurement: Destination station does not support link measurement in RRM");
		return -1;
	}

	if (hostapd_drv_get_sta_measurements(hapd, addr, &sta_info)) {
		wpa_printf(MSG_ERROR, "request link measurement: get tx power used failed");
		return -1;
	}

	for (i = 0; i < mode->num_channels; i++) {
		if (mode->channels[i].chan == iface->conf->channel) {
			max_tx_power = mode->channels[i].max_tx_power;
			break;
		}
	}
	tx_power_used = (s8)(sta_info.TxMgmtPwr / 100);

	/* Link Measurement request (5)*/
	buf = wpabuf_alloc(5);
	if (!buf)
		return -1;

	hapd->link_measurement_token++;
	if (!hapd->link_measurement_token) /* For wraparounds */
		hapd->link_measurement_token++;

	/* IEEE P802.11-REVmc/D6.0, 9.6.7.4 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_LINK_MEASUREMENT_REQUEST);
	wpabuf_put_u8(buf, hapd->link_measurement_token); /* Dialog Token */
	wpabuf_put_u8(buf, tx_power_used); /* tx power used */
	wpabuf_put_u8(buf, max_tx_power); /* tx max power */

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
	if (ret)
		return -1;
	return hapd->link_measurement_token;
}

int hostapd_send_link_measurement_report(struct hostapd_data *hapd,
		const u8 *addr, struct rrm_link_measurement_report link_msmt_report,
		struct dmg_link_margin *dmg_margin,
		struct dmg_link_adaptation_ack *dmg_ack)
{
	struct wpabuf *buf;
	struct sta_info *sta = NULL;
	int ret, i;

	wpa_printf(MSG_DEBUG, "link measurement report: dest addr: " MACSTR
			,MAC2STR(addr));

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_ERROR,
			   "link measurement report: Destination address is not connected");
		return -1;
	}

	if (!(sta->rrm_enabled_capa[0] & WLAN_RRM_CAPS_LINK_MEASUREMENT)) {
		wpa_printf(MSG_ERROR,
			   "link measurement report: Destination station does not support link measurement in RRM");
		return -1;
	}

	/* Link Measurement response (11)
	 * + optional dmg link margin(10)
	 * + optional dmg link adaptation ack(7)*/
	buf = wpabuf_alloc(11 + 10 + 7);
	if (!buf)
		return -1;

	/* IEEE P802.11-REVmc/D6.0, 9.6.7.5 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_LINK_MEASUREMENT_REPORT);
	wpabuf_put_u8(buf, link_msmt_report.dialog_token);
	wpabuf_put_data(buf, (const void *) &link_msmt_report.tpc, 4);
	wpabuf_put_u8(buf, link_msmt_report.rx_ant_id);
	wpabuf_put_u8(buf, link_msmt_report.tx_ant_id);
	wpabuf_put_u8(buf, link_msmt_report.rcpi);
	wpabuf_put_u8(buf, link_msmt_report.rsni);

	if (dmg_margin)
		wpabuf_put_data(buf, (const void *) dmg_margin, 10);

	if (dmg_ack)
		wpabuf_put_data(buf, (const void *) dmg_ack, 7);

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);

	return ret;
}

int hostapd_get_statistics_group_data_len(u8 group_identity)
{
	int statistics_group_data_len = 0;

	if ((group_identity == 0) || (group_identity == 16))
		statistics_group_data_len = 7;
	else if (group_identity == 1)
		statistics_group_data_len = 6;
	else if ((group_identity >= 2) && (group_identity <= 9))
		statistics_group_data_len = 13;
	else if (group_identity == 10)
		statistics_group_data_len = 2;
	else if (group_identity == 11)
		statistics_group_data_len = 10;
	else if ((group_identity >= 12) && (group_identity <= 14))
		statistics_group_data_len = 9;
	else if (group_identity == 15)
		statistics_group_data_len = 5;

	return statistics_group_data_len;
}

static void fill_trig_rep_counters(struct wpabuf *buf,
		struct rrm_trig_rep_counters *trig_rep_counters)
{
	int i;
	if (trig_rep_counters) {
		wpabuf_put_u8(buf, 1);
		wpabuf_put_u8(buf, 8 + (4 * (*trig_rep_counters).num_of_counters));
		wpabuf_put_be32(buf, (*trig_rep_counters).measurement_count);
		wpabuf_put_le16(buf, (*trig_rep_counters).trigger_timeout);
		wpabuf_put_le16(buf, (*trig_rep_counters).counters_trigger_condition);
		for (i = 0; i < 7; i++)
			if ((*trig_rep_counters).counters[i] != 0)
				wpabuf_put_be32(buf, (*trig_rep_counters).counters[i]);
	}
}

int hostapd_send_sta_statistics_req(struct hostapd_data *hapd, const u8 *addr,
		u16 num_of_repetitions, u8 measurement_request_mode,
		const u8 *peer_addr, u16 random_interval, u16 measurement_duration,
		u8 group_identity, struct rrm_trig_rep_counters *trig_rep_sta_counters,
		struct rrm_trig_rep_counters *trig_rep_qos_sta_counters,
		struct rrm_trig_rep_counters *trig_rep_rsna_counters)
{
	struct wpabuf *buf;
	struct sta_info *sta = NULL;
	u8 *len;
	int ret, i;

	wpa_printf(MSG_DEBUG, "request sta statistics: dest addr: " MACSTR
			,MAC2STR(addr));

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_ERROR,
			   "request sta statistics: Destination address is not connected");
		return -1;
	}

	if (!(sta->rrm_enabled_capa[1] & WLAN_RRM_CAPS_STATISTICS_MEASUREMENT)) {
		wpa_printf(MSG_ERROR,
			   "request sta statistics: Destination station does not support sta statistics in RRM");
		return -1;
	}

	/* Measurement request (5) + Measurement element with sta statistics (16) + optional sub-elements (255)*/
	buf = wpabuf_alloc(5 + 16 + 255);
	if (!buf)
		return -1;

	hapd->sta_statistics_token++;
	if (!hapd->sta_statistics_token) /* For wraparounds */
		hapd->sta_statistics_token++;

	/* IEEE P802.11-REVmc/D6.0, 9.6.7.2 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_RADIO_MEASUREMENT_REQUEST);
	wpabuf_put_u8(buf, hapd->sta_statistics_token); /* Dialog Token */
	wpabuf_put_le16(buf, num_of_repetitions); /* Number of Repetitions */

	/* IEEE P802.11-REVmc/D6.0, 9.4.2.21 */
	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REQUEST);
	len = wpabuf_put(buf, 1); /* Length will be set later */

	wpabuf_put_u8(buf, hapd->sta_statistics_token); /* Measurement Token */
	wpabuf_put_u8(buf, measurement_request_mode); /* Measurement Request Mode */
	wpabuf_put_u8(buf, MEASURE_TYPE_STA_STATISTICS); /* Measurement Type */

	/* IEEE P802.11-REVmc/D6.0, 9.4.2.21.9 */
	wpabuf_put_data(buf, (const void *) peer_addr, ETH_ALEN); /* peer mac addr */
	wpabuf_put_le16(buf, random_interval); /* Randomization Interval */
	wpabuf_put_le16(buf, measurement_duration); /* Measurement Duration */
	wpabuf_put_u8(buf, group_identity); /* group identity */

	fill_trig_rep_counters(buf, trig_rep_sta_counters);
	fill_trig_rep_counters(buf, trig_rep_qos_sta_counters);
	fill_trig_rep_counters(buf, trig_rep_rsna_counters);

	/* category + Action + measurement type + token + reps + EID + len = 7 */
	*len = wpabuf_len(buf) - 7;

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
	if (ret)
		return -1;
	return hapd->sta_statistics_token;
}

int hostapd_send_sta_statistics_report(struct hostapd_data *hapd, const u8 *addr,
		u8 dialog_token, u8 measurement_token, u8 measurement_rep_mode,
		u16 measurement_duration, u8 group_identity,
		const u32 *statistics_group_data, u8 statistics_group_data_len,
		u8 *rep_reason)
{
	struct wpabuf *buf;
	struct sta_info *sta = NULL;
	u8 *len;
	int ret, i;

	wpa_printf(MSG_DEBUG, "sta statistics report: dest addr: " MACSTR
			,MAC2STR(addr));

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_ERROR,
			   "sta statistics report: Destination address is not connected");
		return -1;
	}

	if (!(sta->rrm_enabled_capa[1] & WLAN_RRM_CAPS_STATISTICS_MEASUREMENT)) {
		wpa_printf(MSG_ERROR,
			   "sta statistics report: Destination station does not support sta statistics in RRM");
		return -1;
	}

	/* Measurement report (3) + Measurement element with sta statistics (18) + optional sub-elements (255)*/
	buf = wpabuf_alloc(3 + 60 + 255);
	if (!buf)
		return -1;

	/* IEEE P802.11-REVmc/D6.0, 9.6.7.3 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_RADIO_MEASUREMENT_REPORT);
	wpabuf_put_u8(buf, dialog_token); /* Dialog Token */

	/* IEEE P802.11-REVmc/D5.0, 9.4.2.22 */
	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REPORT);
	len = wpabuf_put(buf, 1); /* Length will be set later */

	wpabuf_put_u8(buf, measurement_token); /* Measurement Token */
	wpabuf_put_u8(buf, measurement_rep_mode); /* Measurement Report Mode */
	wpabuf_put_u8(buf, MEASURE_TYPE_STA_STATISTICS); /* Measurement Type */

	/* IEEE P802.11-REVmc/D6.0, 9.4.2.22.9 */
	wpabuf_put_le16(buf, measurement_duration); /* Measurement Duration */
	wpabuf_put_u8(buf, group_identity); /* group_identity */
	for (i = 0; i < statistics_group_data_len; i++)
		if (((group_identity == 11) && ((i == 4) || (i == 8)))
		  ||((group_identity == 12) && ((i == 2) || (i == 6)))) {
			wpabuf_put_data(buf, &statistics_group_data[i], 8);
			i++;
		} else {
			wpabuf_put_be32(buf, statistics_group_data[i]);
		}

	if (rep_reason) {
		wpabuf_put_u8(buf, 1); /* reporting reason sub element id */
		wpabuf_put_u8(buf, 1);   /* sub element length */
		wpabuf_put_u8(buf, *rep_reason);
	}

	/* Action + measurement type + token + EID + len = 5 */
	*len = wpabuf_len(buf) - 5;

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
	return ret;
}

int hostapd_send_channel_load_req(struct hostapd_data *hapd, const u8 *addr,
		u16 num_of_repetitions, u8 measurement_request_mode, u8 oper_class,
		u8 channel, u16 random_interval, u16 measurement_duration, u8 *rep_cond,
		u8 *ch_load_ref_val, u8 *ch_width, u8 *ch_center_freq0,
		u8 *ch_center_freq1)
{
	struct wpabuf *buf;
	struct sta_info *sta = NULL;
	u8 *len;
	int ret, i;

	wpa_printf(MSG_DEBUG, "request channel load: dest addr: " MACSTR
			,MAC2STR(addr));

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_ERROR,
			   "request channel load: Destination address is not connected");
		return -1;
	}

	if (!(sta->rrm_enabled_capa[1] & WLAN_RRM_CAPS_CHANNEL_LOAD)) {
		wpa_printf(MSG_ERROR,
			   "request channel load: Destination station does not support channel load in RRM");
		return -1;
	}

	/* Measurement request (5) + Measurement element with channel load (11) + optional sub-elements (255)*/
	buf = wpabuf_alloc(5 + 11 + 255);
	if (!buf)
		return -1;

	hapd->channel_load_token++;
	if (!hapd->channel_load_token) /* For wraparounds */
		hapd->channel_load_token++;

	/* IEEE P802.11-REVmc/D6.0, 9.6.7.2 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_RADIO_MEASUREMENT_REQUEST);
	wpabuf_put_u8(buf, hapd->channel_load_token); /* Dialog Token */
	wpabuf_put_le16(buf, num_of_repetitions); /* Number of Repetitions */

	/* IEEE P802.11-REVmc/D6.0, 9.4.2.21 */
	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REQUEST);
	len = wpabuf_put(buf, 1); /* Length will be set later */

	wpabuf_put_u8(buf, hapd->channel_load_token); /* Measurement Token */
	wpabuf_put_u8(buf, measurement_request_mode); /* Measurement Request Mode */
	wpabuf_put_u8(buf, MEASURE_TYPE_CHANNEL_LOAD); /* Measurement Type */

	/* IEEE P802.11-REVmc/D6.0, 9.4.2.21.5 */
	wpabuf_put_u8(buf, oper_class); /* Operating class */
	wpabuf_put_u8(buf, channel); /* Channel Number */
	wpabuf_put_le16(buf, random_interval); /* Randomization Interval */
	wpabuf_put_le16(buf, measurement_duration); /* Measurement Duration */

	/* optional sub-elements should go here */
	if (rep_cond && ch_load_ref_val) {
		wpabuf_put_u8(buf, 1); /* channel load reporting sub element id */
		wpabuf_put_u8(buf, 2);
		wpabuf_put_u8(buf, *rep_cond);
		wpabuf_put_u8(buf, *ch_load_ref_val);
	}

	if (ch_width && ch_center_freq0 && ch_center_freq1) {
		wpabuf_put_u8(buf, 163); /* wide bandwidth channel switch sub element id */
		wpabuf_put_u8(buf, 5);   /* sub element length */
		wpabuf_put_u8(buf, 194); /* wide bandwidth channel switch element id */
		wpabuf_put_u8(buf, 3); 	 /* element length */
		wpabuf_put_u8(buf, *ch_width);
		wpabuf_put_u8(buf, *ch_center_freq0);
		wpabuf_put_u8(buf, *ch_center_freq1);
	}

	/* category + Action + measurement type + token + reps + EID + len = 7 */
	*len = wpabuf_len(buf) - 7;

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
	if (ret)
		return -1;
	return hapd->channel_load_token;
}

int hostapd_send_channel_load_report(struct hostapd_data *hapd, const u8 *addr,
		u8 dialog_token, u8 measurement_token, u8 measurement_rep_mode,
		u8 op_class, u8 channel, u64 start_time, u16 measurement_duration,
		u8 channel_load, u8 *ch_width, u8 *ch_center_freq0, u8 *ch_center_freq1)
{
	struct wpabuf *buf;
	struct sta_info *sta = NULL;
	u8 *len;
	int ret, i;

	wpa_printf(MSG_DEBUG, "channel load report: dest addr: " MACSTR
			,MAC2STR(addr));

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_ERROR,
			   "channel load report: Destination address is not connected");
		return -1;
	}

	if (!(sta->rrm_enabled_capa[1] & WLAN_RRM_CAPS_CHANNEL_LOAD)) {
		wpa_printf(MSG_ERROR,
			   "channel load report: Destination station does not support channel load in RRM");
		return -1;
	}

	/* Measurement report (3) + Measurement element with channel load (18) + optional sub-elements (255)*/
	buf = wpabuf_alloc(3 + 18 + 255);
	if (!buf)
		return -1;

	/* IEEE P802.11-REVmc/D6.0, 9.6.7.3 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_RADIO_MEASUREMENT_REPORT);
	wpabuf_put_u8(buf, dialog_token); /* Dialog Token */

	/* IEEE P802.11-REVmc/D5.0, 9.4.2.22 */
	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REPORT);
	len = wpabuf_put(buf, 1); /* Length will be set later */

	wpabuf_put_u8(buf, measurement_token); /* Measurement Token */
	wpabuf_put_u8(buf, measurement_rep_mode); /* Measurement Report Mode */
	wpabuf_put_u8(buf, MEASURE_TYPE_CHANNEL_LOAD); /* Measurement Type */

	/* IEEE P802.11-REVmc/D6.0, 9.4.2.22.5 */
	wpabuf_put_u8(buf, op_class); /* Operating class */
	wpabuf_put_u8(buf, channel); /* Channel Number */
	wpabuf_put_data(buf, (const void *) &start_time, 8); /* actual measurement start time */
	wpabuf_put_le16(buf, measurement_duration); /* Measurement Duration */
	wpabuf_put_u8(buf, channel_load); /* channel_load */

	if (ch_width && ch_center_freq0 && ch_center_freq1) {
		wpabuf_put_u8(buf, 163); /* wide bandwidth channel switch sub element id */
		wpabuf_put_u8(buf, 5);   /* sub element length */
		wpabuf_put_u8(buf, 194); /* wide bandwidth channel switch element id */
		wpabuf_put_u8(buf, 3); 	 /* element length */
		wpabuf_put_u8(buf, *ch_width);
		wpabuf_put_u8(buf, *ch_center_freq0);
		wpabuf_put_u8(buf, *ch_center_freq1);
	}

	/* Action + measurement type + token + EID + len = 5 */
	*len = wpabuf_len(buf) - 5;

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
	return ret;
}

int hostapd_send_noise_histogram_req(struct hostapd_data *hapd, const u8 *addr,
		u16 num_of_repetitions, u8 measurement_request_mode, u8 oper_class,
		u8 channel, u16 random_interval, u16 measurement_duration, u8 *rep_cond,
		u8 *anpi_ref_val, u8 *ch_width, u8 *ch_center_freq0,
		u8 *ch_center_freq1)
{
	struct wpabuf *buf;
	struct sta_info *sta = NULL;
	u8 *len;
	int ret, i;

	wpa_printf(MSG_DEBUG, "request noise histogram: dest addr: " MACSTR
			,MAC2STR(addr));

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_ERROR,
			   "request noise histogram: Destination address is not connected");
		return -1;
	}

	if (!(sta->rrm_enabled_capa[1] & WLAN_RRM_CAPS_NOISE_HISTOGRAM)) {
		wpa_printf(MSG_ERROR,
			   "request noise histogram: Destination station does not support noise histogram in RRM");
		return -1;
	}

	/* Measurement request (5) + Measurement element with noise histogram (11) + optional sub-elements (255)*/
	buf = wpabuf_alloc(5 + 11 + 255);
	if (!buf)
		return -1;

	hapd->noise_histogram_token++;
	if (!hapd->noise_histogram_token) /* For wraparounds */
		hapd->noise_histogram_token++;

	/* IEEE P802.11-REVmc/D6.0, 9.6.7.2 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_RADIO_MEASUREMENT_REQUEST);
	wpabuf_put_u8(buf, hapd->noise_histogram_token); /* Dialog Token */
	wpabuf_put_le16(buf, num_of_repetitions); /* Number of Repetitions */

	/* IEEE P802.11-REVmc/D6.0, 9.4.2.21 */
	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REQUEST);
	len = wpabuf_put(buf, 1); /* Length will be set later */

	wpabuf_put_u8(buf, hapd->noise_histogram_token); /* Measurement Token */
	wpabuf_put_u8(buf, measurement_request_mode); /* Measurement Request Mode */
	wpabuf_put_u8(buf, MEASURE_TYPE_NOISE_HIST); /* Measurement Type */

	/* IEEE P802.11-REVmc/D6.0, 9.4.2.21.6 */
	wpabuf_put_u8(buf, oper_class); /* Operating class */
	wpabuf_put_u8(buf, channel); /* Channel Number */
	wpabuf_put_le16(buf, random_interval); /* Randomization Interval */
	wpabuf_put_le16(buf, measurement_duration); /* Measurement Duration */

	/* optional sub-elements should go here */
	if (rep_cond && anpi_ref_val) {
		wpabuf_put_u8(buf, 1); /* channel load reporting sub element id */
		wpabuf_put_u8(buf, 2);
		wpabuf_put_u8(buf, *rep_cond);
		wpabuf_put_u8(buf, *anpi_ref_val);
	}

	if (ch_width && ch_center_freq0 && ch_center_freq1) {
		wpabuf_put_u8(buf, 163); /* wide bandwidth channel switch sub element id */
		wpabuf_put_u8(buf, 5);   /* sub element length */
		wpabuf_put_u8(buf, 194); /* wide bandwidth channel switch element id */
		wpabuf_put_u8(buf, 3); 	 /* element length */
		wpabuf_put_u8(buf, *ch_width);
		wpabuf_put_u8(buf, *ch_center_freq0);
		wpabuf_put_u8(buf, *ch_center_freq1);
	}

	/* category + Action + measurement type + token + reps + EID + len = 7 */
	*len = wpabuf_len(buf) - 7;

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
	if (ret)
		return -1;
	return hapd->noise_histogram_token;
}

int hostapd_send_noise_histogram_report(struct hostapd_data *hapd, const u8 *addr,
		u8 dialog_token, u8 measurement_token, u8 measurement_rep_mode,
		u8 op_class, u8 channel, u64 start_time, u16 measurement_duration,
		u8 ant_id, u8 anpi, u8 *ipi_density, u8 *ch_width, u8 *ch_center_freq0,
		u8 *ch_center_freq1)
{
	struct wpabuf *buf;
	struct sta_info *sta = NULL;
	u8 *len;
	int ret, i;

	wpa_printf(MSG_DEBUG, "noise histogram report: dest addr: " MACSTR
			,MAC2STR(addr));

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
		wpa_printf(MSG_ERROR,
			   "noise histogram report: Destination address is not connected");
		return -1;
	}

	if (!(sta->rrm_enabled_capa[1] & WLAN_RRM_CAPS_NOISE_HISTOGRAM)) {
		wpa_printf(MSG_ERROR,
			   "noise histogram report: Destination station does not support noise histogram in RRM");
		return -1;
	}

	/* Measurement report (3) + Measurement element with noise histogram (25) + optional sub-elements (255)*/
	buf = wpabuf_alloc(3 + 25 + 255);
	if (!buf)
		return -1;

	/* IEEE P802.11-REVmc/D6.0, 9.6.7.3 */
	wpabuf_put_u8(buf, WLAN_ACTION_RADIO_MEASUREMENT);
	wpabuf_put_u8(buf, WLAN_RRM_RADIO_MEASUREMENT_REPORT);
	wpabuf_put_u8(buf, dialog_token); /* Dialog Token */

	/* IEEE P802.11-REVmc/D5.0, 9.4.2.22 */
	wpabuf_put_u8(buf, WLAN_EID_MEASURE_REPORT);
	len = wpabuf_put(buf, 1); /* Length will be set later */

	wpabuf_put_u8(buf, measurement_token); /* Measurement Token */
	wpabuf_put_u8(buf, measurement_rep_mode); /* Measurement Report Mode */
	wpabuf_put_u8(buf, MEASURE_TYPE_NOISE_HIST); /* Measurement Type */

	/* IEEE P802.11-REVmc/D6.0, 9.4.2.22.6 */
	wpabuf_put_u8(buf, op_class); /* Operating class */
	wpabuf_put_u8(buf, channel); /* Channel Number */
	wpabuf_put_data(buf, (const void *) &start_time, 8); /* actual measurement start time */
	wpabuf_put_le16(buf, measurement_duration); /* Measurement Duration */
	wpabuf_put_u8(buf, ant_id); /* antenna id */
	wpabuf_put_u8(buf, anpi); /* anpi */

	for (i = 0; i < 11; i++)
		wpabuf_put_u8(buf, ipi_density[i]);

	if (ch_width && ch_center_freq0 && ch_center_freq1) {
		wpabuf_put_u8(buf, 163); /* wide bandwidth channel switch sub element id */
		wpabuf_put_u8(buf, 5);   /* sub element length */
		wpabuf_put_u8(buf, 194); /* wide bandwidth channel switch element id */
		wpabuf_put_u8(buf, 3); 	 /* element length */
		wpabuf_put_u8(buf, *ch_width);
		wpabuf_put_u8(buf, *ch_center_freq0);
		wpabuf_put_u8(buf, *ch_center_freq1);
	}

	/* Action + measurement type + token + EID + len = 5 */
	*len = wpabuf_len(buf) - 5;

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);
	return ret;
}
