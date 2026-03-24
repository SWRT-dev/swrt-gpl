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
#include "hostapd.h"
#include "common/hw_features_common.h"
#include "ap/hw_features.h"
#include "../hostapd/ctrl_iface.h"
#include "ap_drv_ops.h"
#include "dfs.h"
#include "sta_info.h"


static int hostapd_csa_update_current_channel(struct hostapd_data *hapd)
{
	/* Update DFS state */
	int ret = 0, i;
	struct hostapd_iface *iface = hapd->iface;
	struct hostapd_hw_modes *mode = mode = iface->current_mode;
	int n_chans, n_chans1, start_chan_idx, start_chan_idx1;
	struct hostapd_channel_data *channel;

	start_chan_idx = dfs_get_start_chan_idx(iface, &start_chan_idx1);
	if (start_chan_idx == -1)
		return -1;

	n_chans = dfs_get_used_n_chans(iface, &n_chans1);


	wpa_printf(MSG_DEBUG, "Update DFS state start channel: %dMHz, n_chans: %d",
		   mode->channels[start_chan_idx].freq, n_chans);
	for (i = 0; i < n_chans; i++) {
		channel = &mode->channels[start_chan_idx + i];
		if (channel->flag & HOSTAPD_CHAN_RADAR) {
			if (set_dfs_state_freq(iface, channel->freq, HOSTAPD_CHAN_DFS_UNAVAILABLE) != 1) {
				wpa_printf(MSG_DEBUG, "Update hostapd DFS state channel:%d fail", channel->chan);
				ret = -1;
			}
			if (hostapd_set_chan_dfs_state(iface, NL80211_DFS_UNAVAILABLE, channel->freq, 0, 0)) {
				wpa_printf(MSG_DEBUG, "Update kernel DFS state channel:%d fail", channel->chan);
				ret = -1;
			}
		}
	}

	return ret;
}

static int hostapd_csa_validate_channel(struct hostapd_data *hapd,
					int start_ch, int bw)
{
	int i, j;
	int n_chans = bw/20;
	int chan = start_ch;
	int pass = 0;
	struct hostapd_iface *iface = hapd->iface;
	struct hostapd_hw_modes *mode = iface->current_mode;

	for (j = 0; j < n_chans; j++) {
		for (i = 0; i < mode->num_channels; i++) {
			if (mode->channels[i].chan == chan) {
				int flag = mode->channels[i].flag;
				wpa_printf(MSG_DEBUG, "validate channel:%d", chan);
				if (flag & HOSTAPD_CHAN_RADAR) {
					/* new channel can be DFS/Available only for ETSI */
					if (iface->dfs_domain != HOSTAPD_DFS_REGION_ETSI) {
						wpa_printf(MSG_DEBUG, "CSA DFS channel can be used only in ETSI:%d", chan);
						return -1;
					}
					if ((flag & HOSTAPD_CHAN_DFS_MASK) != HOSTAPD_CHAN_DFS_AVAILABLE) {
						wpa_printf(MSG_DEBUG, "CSA DFS channel not available:%d", chan);
						return -1;
					}
				}
				pass++;
				break;
			}
		}
		chan += 4;
	}

	if (n_chans == pass)
		return 0;

	return -1;
}

static void hostapd_handle_csa_request(struct hostapd_data *hapd,
					     const u8 *buf, size_t len)
{
	struct hostapd_iface *iface = hapd->iface;
	const struct ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *) buf;
	const u8 *pos, *ie, *end;
	struct csa_settings settings;
	struct sta_info *sta;
	unsigned int i;
	u8 switch_mode = 0;
	u8 switch_count = 0;
	u8 chan_width = 0;
	u8 new_chan = 0;
	u8 seg0_idx = 0;
	u8 seg1_idx = 0;
	int sec_ch = 0;
	u8 start_ch = 0;
	int width = CHANWIDTH_USE_HT;
	int chan_bw = 20;

	if (!hapd->conf->ul_csa)
		return;

	/* Only 5G support */
	if (hapd->iface->current_mode == NULL ||
	    hapd->iface->current_mode->mode != HOSTAPD_MODE_IEEE80211A)
		return;

	/* Only for backhaul VAP */
	if (hapd->conf->mesh_mode != MESH_MODE_BACKHAUL_AP &&
	    hapd->conf->mesh_mode != MESH_MODE_HYBRID &&
	    hapd->conf->mesh_mode != MESH_MODE_EXT_HYBRID)
		return;

	/* Only from associated station */
	sta = ap_get_sta(hapd, mgmt->sa);
	if (sta == NULL || !(sta->flags & WLAN_STA_ASSOC)) {
		wpa_printf(MSG_DEBUG, "IEEE 802.11: Ignored CSA"
			   "frame from unassociated STA " MACSTR, MAC2STR(mgmt->sa));
		return;
	}

	if (!sta->four_addr_mode_sta) {
		wpa_printf(MSG_DEBUG, "IEEE 802.11: Ignored CSA"
			   "frame from non backhaul STA " MACSTR, MAC2STR(mgmt->sa));
		return;
	}

	/* Check if CSA in progress */
	if (hostapd_csa_in_progress(iface))
		return;

	/* Check if active CAC */
	if (iface->cac_started)
		return;

	end = buf + len;
	pos = &mgmt->u.action.u.chan_switch.element_id;
	os_memset(&settings, 0, sizeof(settings));

	if (mgmt->u.action.category == WLAN_ACTION_PUBLIC &&
	    mgmt->u.action.u.ext_chan_switch.action_code == WLAN_PA_EXT_CHANNEL_SWITCH_ANNOUNCE) {

		switch_mode = mgmt->u.action.u.ext_chan_switch.switch_mode;
		new_chan = mgmt->u.action.u.ext_chan_switch.new_chan;
		switch_count = mgmt->u.action.u.ext_chan_switch.switch_count;
		pos = mgmt->u.action.u.ext_chan_switch.variable;
	} else {
		ie = get_ie(pos, end - pos, WLAN_EID_CHANNEL_SWITCH);
		if (ie) {
			if (ie[1] != 3) {
				wpa_printf(MSG_DEBUG, "Bad BW CSA element");
				return;
			}
			switch_mode = ie[2];
			new_chan = ie[3];
			switch_count = ie[4];
		} else {
			wpa_printf(MSG_DEBUG, "No CSA element is found");
			return;
		}
	}

	start_ch = new_chan;

	ie = get_ie(pos, end - pos, WLAN_EID_VHT_WIDE_BW_CHSWITCH);
	if (ie) {
		if (ie[1] != 3) {
			wpa_printf(MSG_DEBUG, "Bad BW switch element");
			return;
		}
		chan_width = ie[2];
		seg0_idx = ie[3];
		seg1_idx = ie[4];
	}

	ie = get_ie(pos, end - pos, WLAN_EID_SECONDARY_CHANNEL_OFFSET);
	if (ie) {
		if (ie[1] != 1) {
			wpa_printf(MSG_DEBUG, "Bad Secondary channel offset element");
			return;
		}
		switch (ie[2]) {
			case 0:
				sec_ch = 0;
				break;
			case 1:
				sec_ch = 1;
				chan_bw = 40;
				break;
			case 3:
				sec_ch = -1;
				chan_bw = 40;
				start_ch -= 4;
				break;
			default:
				wpa_printf(MSG_DEBUG, "Bad Secondary channel offset element");
				return;
		}
	}

	switch (chan_width) {
		case 0:
			break;
		case 1:
			if (seg1_idx) {
				seg0_idx = seg1_idx;
				seg1_idx = 0;
				chan_bw = 160;
				width = CHANWIDTH_160MHZ;
				start_ch = seg0_idx - 14;
			}
			else {
				chan_bw = 80;
				width = CHANWIDTH_80MHZ;
				start_ch = seg0_idx - 6;
			}
			break;
		case 2:
			chan_bw = 160;
			width = CHANWIDTH_160MHZ;
			start_ch = seg0_idx - 14;
			break;
		default:
			wpa_printf(MSG_DEBUG, "Wrong channel width parameter");
			return;
	}

	if (!sec_ch && seg0_idx) {
		wpa_printf(MSG_DEBUG, "Secondary channel offset element is not "
			   "present, try to calculate offset by self");
		sec_ch = hostapd_get_secondary_offset(new_chan, chan_bw, seg0_idx);
	}

	wpa_printf(MSG_DEBUG, "CSA mode:%d channel:%d count:%d freq_seg0_idx:%d "
		   "freq_seg1_idx:%d secondary_channel:%d chan_bw:%d",
		   switch_mode, new_chan, switch_count, seg0_idx,
		   seg1_idx, sec_ch, chan_bw);

	/* Prepare channel switch parameters */
	hostapd_channel_switch_reason_set(iface, HAPD_CHAN_SWITCH_RADAR_DETECTED);
	settings.cs_count = switch_count;
	settings.block_tx = switch_mode;
	if (hostapd_set_freq_params(&settings.freq_params,
				    iface->conf->hw_mode,
				    hostapd_hw_get_freq(hapd, new_chan),
				    new_chan,
				    iface->conf->enable_edmg,
				    iface->conf->edmg_channel,
				    iface->conf->ieee80211n,
				    iface->conf->ieee80211ac,
				    iface->conf->ieee80211ax,
				    sec_ch,
				    width,
				    seg0_idx,
				    0,
				    iface->current_mode->vht_capab,
				    &iface->current_mode->he_capab)) {

		wpa_printf(MSG_ERROR, "failed to calculate CSA freq params");
		return;
	}

	if (hostapd_csa_update_current_channel(hapd)) {
		wpa_printf(MSG_DEBUG, "Can't update current channels, still continue");
	}

	if (hostapd_csa_validate_channel(hapd, start_ch, chan_bw)) {
		wpa_printf(MSG_DEBUG, "CSA channel can't be used,"
			   " try fail-safe channel or reduce bw");
		hostapd_dfs_start_channel_switch(iface);
		return;
	}

	/* Execute channel switch */
	if (hostapd_prepare_and_send_csa_deauth_cfg_to_driver(iface->bss[0])) {
		wpa_printf(MSG_ERROR, "hostapd_prepare_and_send_csa_deauth_cfg_to_driver failed: %s",
			   iface->bss[0]->conf->iface);
		return;
	}
	for (i = 0; i < iface->num_bss; i++) {
		if (hostapd_switch_channel(iface->bss[i], &settings)) {
			wpa_printf(MSG_ERROR, "Unable to switch channel");
			return;
		}
	}
}

void hostapd_handle_spectrum_management(struct hostapd_data *hapd,
					const u8 *buf, size_t len, int ssi_signal)
{
	const struct ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *) buf;

	if (len < IEEE80211_HDRLEN + 3)
		return;

	switch (mgmt->u.action.u.spectrum_action.action) {
	case WLAN_SPEC_MGMT_CHANNEL_SWITCH_ANNOUNCE:
		hostapd_handle_csa_request(hapd, buf, len);
		break;
	default:
		wpa_printf(MSG_DEBUG, "Spectrum management action %u is not supported",
			   mgmt->u.action.u.spectrum_action.action);
		break;
	}
}

void hostapd_handle_action_extcsa(struct hostapd_data *hapd,
				  const u8 *buf, size_t len, int ssi_signal)
{
	const struct ieee80211_mgmt *mgmt = (const struct ieee80211_mgmt *) buf;

	if (len < IEEE80211_HDRLEN + 6)
		return;

	if (mgmt->u.action.u.ext_chan_switch.action_code ==
	    WLAN_PA_EXT_CHANNEL_SWITCH_ANNOUNCE) {
		hostapd_handle_csa_request(hapd, buf, len);
	}
}
