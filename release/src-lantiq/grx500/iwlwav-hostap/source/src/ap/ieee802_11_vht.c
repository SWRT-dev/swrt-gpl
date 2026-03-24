/*
 * hostapd / IEEE 802.11ac VHT
 * Copyright (c) 2002-2009, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of BSD license
 *
 * See README and COPYING for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "hostapd.h"
#include "ap_config.h"
#include "sta_info.h"
#include "beacon.h"
#include "ieee802_11.h"
#include "dfs.h"


u8 * hostapd_eid_vht_capabilities(struct hostapd_data *hapd, u8 *eid, u32 nsts)
{
	errno_t err;
	struct ieee80211_vht_capabilities *cap;
	struct hostapd_hw_modes *mode = hapd->iface->current_mode;
	u8 *pos = eid;

	if (!mode || is_6ghz_op_class(hapd->iconf->op_class))
		return eid;

	if (mode->mode == HOSTAPD_MODE_IEEE80211G && hapd->conf->vendor_vht &&
	    mode->vht_capab == 0 && hapd->iface->hw_features) {
		int i;

		for (i = 0; i < hapd->iface->num_hw_features; i++) {
			if (hapd->iface->hw_features[i].mode ==
			    HOSTAPD_MODE_IEEE80211A) {
				mode = &hapd->iface->hw_features[i];
				break;
			}
		}
	}

	*pos++ = WLAN_EID_VHT_CAP;
	*pos++ = sizeof(*cap);

	cap = (struct ieee80211_vht_capabilities *) pos;
	os_memset(cap, 0, sizeof(*cap));
	cap->vht_capabilities_info = host_to_le32(
		hapd->iface->conf->vht_capab);

	if (nsts != 0) {
		u32 hapd_nsts;

		hapd_nsts = le_to_host32(cap->vht_capabilities_info);
		hapd_nsts = (hapd_nsts >> VHT_CAP_BEAMFORMEE_STS_OFFSET) & 7;
		cap->vht_capabilities_info &=
			~(host_to_le32(hapd_nsts <<
				       VHT_CAP_BEAMFORMEE_STS_OFFSET));
		cap->vht_capabilities_info |=
			host_to_le32(nsts << VHT_CAP_BEAMFORMEE_STS_OFFSET);
	}

	if(hapd->iface->conf->vht_mcs_set[0]) {
			/* Taken from configuration */
			err = memcpy_s(&cap->vht_supported_mcs_set,
				 sizeof(cap->vht_supported_mcs_set),
				 &hapd->iface->conf->vht_mcs_set[0],
				 MIN(sizeof(cap->vht_supported_mcs_set), sizeof(hapd->iface->conf->vht_mcs_set)));
			if (EOK != err) {
				wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
				return NULL;
			}
	} else {
		/* Supported MCS set comes from hw */
		err = memcpy_s(&cap->vht_supported_mcs_set, sizeof(cap->vht_supported_mcs_set), mode->vht_mcs_set, 8);
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return NULL;
		}

		if (CHANWIDTH_160MHZ == hostapd_get_oper_chwidth(hapd->iconf)) {
			u16 tx_highest = le_to_host16(cap->vht_supported_mcs_set.tx_highest);
			tx_highest |= VHT_CAP_EXT_NSS_BW_CAPABLE;
			cap->vht_supported_mcs_set.tx_highest = host_to_le16(tx_highest);
		}
	}

	pos += sizeof(*cap);

	return pos;
}


u8 * hostapd_eid_vht_operation(struct hostapd_data *hapd, u8 *eid)
{
	struct ieee80211_vht_operation *oper;
	u8 *pos = eid;

	if (is_6ghz_op_class(hapd->iconf->op_class))
		return eid;

	*pos++ = WLAN_EID_VHT_OPERATION;
	*pos++ = sizeof(*oper);

	oper = (struct ieee80211_vht_operation *) pos;
	os_memset(oper, 0, sizeof(*oper));

	/*
	 * center freq = 5 GHz + (5 * index)
	 * So index 42 gives center freq 5.210 GHz
	 * which is channel 42 in 5G band
	 */
	oper->vht_op_info_chan_center_freq_seg0_idx =
		hostapd_get_oper_centr_freq_seg0_idx(hapd->iconf);
	oper->vht_op_info_chan_center_freq_seg1_idx =
		hostapd_get_oper_centr_freq_seg1_idx(hapd->iconf);;

	oper->vht_op_info_chwidth = hostapd_get_oper_chwidth(hapd->iconf);
	if (hostapd_get_oper_chwidth(hapd->iconf) == 2) {
		/*
		 * Convert 160 MHz channel width to new style as interop
		 * workaround.
		 */
		oper->vht_op_info_chwidth = 1;
		oper->vht_op_info_chan_center_freq_seg1_idx =
			oper->vht_op_info_chan_center_freq_seg0_idx;
		if (hapd->iconf->channel <
		    hostapd_get_oper_centr_freq_seg0_idx(hapd->iconf))
			oper->vht_op_info_chan_center_freq_seg0_idx -= 8;
		else
			oper->vht_op_info_chan_center_freq_seg0_idx += 8;
	} else if (hostapd_get_oper_chwidth(hapd->iconf) == 3) {
		/*
		 * Convert 80+80 MHz channel width to new style as interop
		 * workaround.
		 */
		oper->vht_op_info_chwidth = 1;
	}

	/* For a STA with dot11VHTExtendedNSSBWCapable equal to true: See Table 9-250, otherwise this field is set to 0. */
	hapd->iface->ht_op_mode &= ~HT_OPER_OP_MODE_CHANNEL_CENTER_FREQUENCY_SEGMENT_TWO;
	if (hapd->iconf->vht_capab & VHT_CAP_EXT_NSS_BW_3)
		hapd->iface->ht_op_mode |= oper->vht_op_info_chan_center_freq_seg1_idx << 5;

	/* For a STA with dot11VHTExtendedNSSBWCapable equal to true: See Table 9-250, otherwise this field is set to 0. */
	hapd->iface->ht_op_mode &= ~HT_OPER_OP_MODE_CHANNEL_CENTER_FREQUENCY_SEGMENT_TWO;
	if (hapd->iconf->vht_capab & VHT_CAP_EXT_NSS_BW_3)
		hapd->iface->ht_op_mode |= oper->vht_op_info_chan_center_freq_seg1_idx << 5;


	/* VHT Basic MCS set comes from hw */
	/* Hard code 1 stream, MCS0-7 is a min Basic VHT MCS rates */
	oper->vht_basic_mcs_set = host_to_le16(0xfffc);
	pos += sizeof(*oper);

	return pos;
}


static int check_valid_vht_mcs(struct hostapd_hw_modes *mode,
			       const u8 *sta_vht_capab)
{
	const struct ieee80211_vht_capabilities *vht_cap;
	struct ieee80211_vht_capabilities ap_vht_cap;
	u16 sta_rx_mcs_set, ap_tx_mcs_set;
	int i;

	if (!mode)
		return 1;

	/*
	 * Disable VHT caps for STAs for which there is not even a single
	 * allowed MCS in any supported number of streams, i.e., STA is
	 * advertising 3 (not supported) as VHT MCS rates for all supported
	 * stream cases.
	 */
	os_memcpy(&ap_vht_cap.vht_supported_mcs_set, mode->vht_mcs_set,
		  sizeof(ap_vht_cap.vht_supported_mcs_set));
	vht_cap = (const struct ieee80211_vht_capabilities *) sta_vht_capab;

	/* AP Tx MCS map vs. STA Rx MCS map */
	sta_rx_mcs_set = le_to_host16(vht_cap->vht_supported_mcs_set.rx_map);
	ap_tx_mcs_set = le_to_host16(ap_vht_cap.vht_supported_mcs_set.tx_map);

	for (i = 0; i < VHT_RX_NSS_MAX_STREAMS; i++) {
		if ((ap_tx_mcs_set & (0x3 << (i * 2))) == 3)
			continue;

		if ((sta_rx_mcs_set & (0x3 << (i * 2))) == 3)
			continue;

		return 1;
	}

	wpa_printf(MSG_DEBUG,
		   "No matching VHT MCS found between AP TX and STA RX");
	return 0;
}

u8 * hostapd_eid_txpower_envelope_6g(struct hostapd_data *hapd, u8 *eid, bool only_one_psd_limit)
{
#define MAX_TRANSMIT_PWR_INTERPRET_LOC_EIRP     0
#define MAX_TRANSMIT_PWR_INTERPRET_LOC_EIRP_PSD 1
#define MAX_TRANSMIT_PWR_INTERPRET_REG_EIRP     2
#define MAX_TRANSMIT_PWR_INTERPRET_REG_EIRP_PSD 3
#define MAX_TRANSMIT_PWR_INTERPRET_OFFSET       3

#define MAX_TX_POWER_COUNT_ALL_BW_PSD           0
#define MAX_TX_POWER_COUNT_20MHZ_PSD            1
#define MAX_TX_POWER_COUNT_40MHZ_PSD            2
#define MAX_TX_POWER_COUNT_80MHZ_PSD            3
#define MAX_TX_POWER_COUNT_160MHZ_PSD           4
#define MAX_TX_POWER_COUNT_RESERVED_PSD         7

#define MAX_TX_POWER_RANGE_MIN                  (-127)
#define MAX_TX_POWER_RANGE_MAX                  126
#define MAX_TX_POWER_NO_MAX_PSD_LIMIT           127

#define MAX_TX_POWER_PSD_MIN_NUM_OCTETS         1
#define TEN_LOG_BW_20MHZ                        13
#define FACTOR_2_POWER_8                        256

	struct hostapd_iface *iface = hapd->iface;
	struct hostapd_hw_modes *mode = iface->current_mode;
	struct hostapd_channel_data *chan;
	int max_tx_power = 0;
	u8 tx_pwr = 0;
	u8 tx_pwr_count = MAX_TX_POWER_COUNT_RESERVED_PSD;
	u8 num_octets_psd = 0;
	int i = 0;

	if (only_one_psd_limit) {
		for (i = 0; i < mode->num_channels; i++) {
			if (mode->channels[i].freq == iface->freq)
				break;
		}
		if (i == mode->num_channels)
			return eid;
		/* Compute/advertise only one Max Transmit PSD for a PPDU of any BW within the BSS BW */
		chan = &mode->channels[i]; 
		max_tx_power = (chan->max_tx_power - TEN_LOG_BW_20MHZ) << 1; /* 0.5dB steps */

		if (max_tx_power > MAX_TX_POWER_RANGE_MAX) {
			max_tx_power = MAX_TX_POWER_RANGE_MAX;
		}
		if (max_tx_power < MAX_TX_POWER_RANGE_MIN)
			max_tx_power = MAX_TX_POWER_RANGE_MIN;

		if (max_tx_power < 0)
			tx_pwr = (u8)(max_tx_power + FACTOR_2_POWER_8); /*2's complement form*/
		else
			tx_pwr = (u8)max_tx_power;

		tx_pwr_count = MAX_TX_POWER_COUNT_ALL_BW_PSD;
	} else {
		 /* TODO: To compute PSD values based on the BW configured and advertise multiple PSD,	*
		  * 	  one for each of the 20MHz channels contained within the configured BW			*/
		return eid;
	}

	/* Compute the number of octets representing the PSD of 20MHz channels */
	switch (tx_pwr_count) {
		case MAX_TX_POWER_COUNT_ALL_BW_PSD :
			num_octets_psd = MAX_TX_POWER_PSD_MIN_NUM_OCTETS;
			break;
		case MAX_TX_POWER_COUNT_20MHZ_PSD	:
		case MAX_TX_POWER_COUNT_40MHZ_PSD	:
		case MAX_TX_POWER_COUNT_80MHZ_PSD	:
		case MAX_TX_POWER_COUNT_160MHZ_PSD:
			num_octets_psd = (1 << (tx_pwr_count - 1));
			break;
		default :
			tx_pwr = MAX_TX_POWER_RANGE_MAX;
			num_octets_psd = MAX_TX_POWER_PSD_MIN_NUM_OCTETS;
			break;
	}

	*eid++ = WLAN_EID_VHT_TRANSMIT_POWER_ENVELOPE;
	*eid++ = 1 + num_octets_psd;
	/* Max Transmit Power interpretation = 3 (Regulatory client EIRP-PSD), Category = Default(0) for 6GHz band */
	*eid++ = tx_pwr_count | (MAX_TRANSMIT_PWR_INTERPRET_REG_EIRP_PSD << MAX_TRANSMIT_PWR_INTERPRET_OFFSET);
	for (i = 0; i < num_octets_psd; i++)
		*eid++ = tx_pwr;

	return eid;
}

u8 * hostapd_eid_txpower_envelope(struct hostapd_data *hapd, u8 *eid)
{
	struct hostapd_iface *iface = hapd->iface;
	struct hostapd_config *iconf = iface->conf;
	struct hostapd_hw_modes *mode = iface->current_mode;
	struct hostapd_channel_data *chan;
	int dfs, i;
	u8 channel, tx_pwr_count, local_pwr_constraint;
	int max_tx_power;
	u8 tx_pwr;
	bool op_class_6g = is_6ghz_op_class(iface->conf->op_class);

	if (!mode)
		return eid;

	if (ieee80211_freq_to_chan(iface->freq, &channel) == NUM_HOSTAPD_MODES)
		return eid;

	if (op_class_6g)
		return hostapd_eid_txpower_envelope_6g(hapd, eid, true);

	for (i = 0; i < mode->num_channels; i++) {
		if (mode->channels[i].freq == iface->freq)
			break;
	}
	if (i == mode->num_channels)
		return eid;

	switch (hostapd_get_oper_chwidth(iface->conf)) {
	case CHANWIDTH_USE_HT:
		if (iconf->secondary_channel == 0) {
			/* Max Transmit Power count = 0 (20 MHz) */
			tx_pwr_count = 0;
		} else {
			/* Max Transmit Power count = 1 (20, 40 MHz) */
			tx_pwr_count = 1;
		}
		break;
	case CHANWIDTH_80MHZ:
		/* Max Transmit Power count = 2 (20, 40, and 80 MHz) */
		tx_pwr_count = 2;
		break;
	case CHANWIDTH_80P80MHZ:
	case CHANWIDTH_160MHZ:
		/* Max Transmit Power count = 3 (20, 40, 80, 160/80+80 MHz) */
		tx_pwr_count = 3;
		break;
	default:
		return eid;
	}

	/*
	 * Below local_pwr_constraint logic is referred from
	 * hostapd_eid_pwr_constraint.
	 *
	 * Check if DFS is required by regulatory.
	 */
	dfs = hostapd_is_dfs_required(hapd->iface);
	if (dfs < 0)
		dfs = 0;

	/*
	 * In order to meet regulations when TPC is not implemented using
	 * a transmit power that is below the legal maximum (including any
	 * mitigation factor) should help. In this case, indicate 3 dB below
	 * maximum allowed transmit power.
	 */
	if (hapd->iconf->local_pwr_constraint == -1)
		local_pwr_constraint = (dfs == 0) ? 0 : 3;
	else
		local_pwr_constraint = hapd->iconf->local_pwr_constraint;

	/*
	 * A STA that is not an AP shall use a transmit power less than or
	 * equal to the local maximum transmit power level for the channel.
	 * The local maximum transmit power can be calculated from the formula:
	 * local max TX pwr = max TX pwr - local pwr constraint
	 * Where max TX pwr is maximum transmit power level specified for
	 * channel in Country element and local pwr constraint is specified
	 * for channel in this Power Constraint element.
	 */
	chan = &mode->channels[i];
	max_tx_power = chan->max_tx_power - local_pwr_constraint;

	/*
	 * Local Maximum Transmit power is encoded as two's complement
	 * with a 0.5 dB step.
	 */
	max_tx_power *= 2; /* in 0.5 dB steps */
	if (max_tx_power > 127) {
		/* 63.5 has special meaning of 63.5 dBm or higher */
		max_tx_power = 127;
	}
	if (max_tx_power < -128)
		max_tx_power = -128;
	if (max_tx_power < 0)
		tx_pwr = 0x80 + max_tx_power + 128;
	else
		tx_pwr = max_tx_power;

	*eid++ = WLAN_EID_VHT_TRANSMIT_POWER_ENVELOPE;
	*eid++ = 2 + tx_pwr_count;

	/*
	 * Max Transmit Power count and
	 * Max Transmit Power units = 0 (EIRP)
	 */
	*eid++ = tx_pwr_count;

	for (i = 0; i <= tx_pwr_count; i++)
		*eid++ = tx_pwr;

	return eid;
}

u16 copy_sta_vht_capab(struct hostapd_data *hapd, struct sta_info *sta,
		       const u8 *vht_capab)
{
	/* Disable VHT caps for STAs associated to no-VHT BSSes. */
	if (!vht_capab ||
	    !hapd->iconf->ieee80211ac || hapd->conf->disable_11ac ||
	    !check_valid_vht_mcs(hapd->iface->current_mode, vht_capab)) {
		sta->flags &= ~WLAN_STA_VHT;
		os_free(sta->vht_capabilities);
		sta->vht_capabilities = NULL;
		return WLAN_STATUS_SUCCESS;
	}

	if (sta->vht_capabilities == NULL) {
		sta->vht_capabilities =
			os_zalloc(sizeof(struct ieee80211_vht_capabilities));
		if (sta->vht_capabilities == NULL)
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	sta->flags |= WLAN_STA_VHT;
	os_memcpy(sta->vht_capabilities, vht_capab,
		  sizeof(struct ieee80211_vht_capabilities));

	return WLAN_STATUS_SUCCESS;
}


u16 copy_sta_vht_operation(struct hostapd_data *hapd, struct sta_info *sta,
		       const u8 *vht_operation)
{
	errno_t err;
	if (!vht_operation) {
		os_free(sta->vht_operation);
		sta->vht_operation = NULL;
		return WLAN_STATUS_SUCCESS;
	}

	if (sta->vht_operation == NULL) {
		sta->vht_operation =
			os_zalloc(sizeof(struct ieee80211_vht_operation));
		if (sta->vht_operation == NULL)
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	err = memcpy_s(sta->vht_operation, sizeof(struct ieee80211_vht_operation),
		 vht_operation, sizeof(struct ieee80211_vht_operation));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		os_free(sta->vht_operation);
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	return WLAN_STATUS_SUCCESS;
}

u16 copy_sta_vht_oper(struct hostapd_data *hapd, struct sta_info *sta,
                      const u8 *vht_oper)
{
	errno_t err;
        if (!vht_oper) {
                os_free(sta->vht_operation);
                sta->vht_operation = NULL;
                return WLAN_STATUS_SUCCESS;
        }

        if (!sta->vht_operation) {
                sta->vht_operation =
                        os_zalloc(sizeof(struct ieee80211_vht_operation));
                if (!sta->vht_operation)
                        return WLAN_STATUS_UNSPECIFIED_FAILURE;
        }

        err = memcpy_s(sta->vht_operation, sizeof(struct ieee80211_vht_operation),
        	 vht_oper, sizeof(struct ieee80211_vht_operation));
	if (EOK != err) {
		os_free(sta->vht_operation);
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

        return WLAN_STATUS_SUCCESS;
}
u16 copy_sta_vendor_vht(struct hostapd_data *hapd, struct sta_info *sta,
			const u8 *ie, size_t len)
{
	errno_t err;
	const u8 *vht_capab;
	unsigned int vht_capab_len;

	if (!ie || len < 5 + 2 + sizeof(struct ieee80211_vht_capabilities) ||
	    hapd->conf->disable_11ac)
		goto no_capab;

	/* The VHT Capabilities element embedded in vendor VHT */
	vht_capab = ie + 5;
	if (vht_capab[0] != WLAN_EID_VHT_CAP)
		goto no_capab;
	vht_capab_len = vht_capab[1];
	if (vht_capab_len < sizeof(struct ieee80211_vht_capabilities) ||
	    (int) vht_capab_len > ie + len - vht_capab - 2)
		goto no_capab;
	vht_capab += 2;

	if (sta->vht_capabilities == NULL) {
		sta->vht_capabilities =
			os_zalloc(sizeof(struct ieee80211_vht_capabilities));
		if (sta->vht_capabilities == NULL)
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	sta->flags |= WLAN_STA_VHT | WLAN_STA_VENDOR_VHT;
	err = memcpy_s(sta->vht_capabilities, sizeof(struct ieee80211_vht_capabilities),
		 vht_capab, sizeof(struct ieee80211_vht_capabilities));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		os_free(sta->vht_capabilities);
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}
	return WLAN_STATUS_SUCCESS;

no_capab:
	sta->flags &= ~WLAN_STA_VENDOR_VHT;
	return WLAN_STATUS_SUCCESS;
}


u16 copy_sta_vendor2_vht(struct hostapd_data *hapd, struct sta_info *sta,
			const u8 *vht_capab)
{
	errno_t err;
	/* Disable VHT caps for STAs associated to no-VHT BSSes. */
	if (!vht_capab) {
		sta->flags &= ~WLAN_STA_VHT;
		os_free(sta->vht_capabilities);
		sta->vht_capabilities = NULL;
		return WLAN_STATUS_SUCCESS;
	}

	if (sta->vht_capabilities == NULL) {
		sta->vht_capabilities =
			os_zalloc(sizeof(struct ieee80211_vht_capabilities));
		if (sta->vht_capabilities == NULL)
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	sta->flags |= WLAN_STA_VHT | WLAN_STA_VENDOR2_VHT;
	err = memcpy_s(sta->vht_capabilities, sizeof(struct ieee80211_vht_capabilities),
		 vht_capab, sizeof(struct ieee80211_vht_capabilities));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		os_free(sta->vht_capabilities);
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	return WLAN_STATUS_SUCCESS;
}


u8 * hostapd_eid_vendor_vht(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;

	if (!hapd->iface->current_mode || is_6ghz_op_class(hapd->iconf->op_class))
		return eid;

	*pos++ = WLAN_EID_VENDOR_SPECIFIC;
	*pos++ = (5 +		/* The Vendor OUI, type and subtype */
		  2 + sizeof(struct ieee80211_vht_capabilities) +
		  2 + sizeof(struct ieee80211_vht_operation));

	WPA_PUT_BE32(pos, (OUI_BROADCOM << 8) | VENDOR_VHT_TYPE);
	pos += 4;
	*pos++ = VENDOR_VHT_SUBTYPE;
	pos = hostapd_eid_vht_capabilities(hapd, pos, 0);
	if (!pos) {
		wpa_printf(MSG_ERROR, "hostapd_eid_vht_capabilities");
		return NULL;
	}
	pos = hostapd_eid_vht_operation(hapd, pos);

	return pos;
}


u16 set_sta_vht_opmode(struct hostapd_data *hapd, struct sta_info *sta,
		       const u8 *vht_oper_notif)
{
	if (!vht_oper_notif) {
		sta->flags &= ~WLAN_STA_VHT_OPMODE_ENABLED;
		return WLAN_STATUS_SUCCESS;
	}

	sta->flags |= WLAN_STA_VHT_OPMODE_ENABLED;
	sta->vht_opmode = *vht_oper_notif;
	return WLAN_STATUS_SUCCESS;
}

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

static inline u16 limit_rx_tx_highest (u16 x, u16 y) {
	u16 res = 0;
	x = le_to_host16(x);
	y = le_to_host16(y);
	if (!x)
		res = y;
	else if (!y)
		res = x;
	else
		res = MIN(x, y);
	return host_to_le16(res);
}

static inline u16 limit_rx_tx_map (u16 x, u16 y) {
	u16 res = 0;
	int i;
	for (i = 0; i < 16; i+=2) {
		u16 mask = 3 << i;
		u16 r1 = x & mask;
		u16 r2 = y & mask;
		if (r1 == mask || r2 == mask) {
			res |= mask;
		} else {
			res |= MIN(r1, r2);
		}
	}
	return res;
}

void hostapd_get_vht_capab(struct hostapd_data *hapd,
			   struct ieee80211_vht_capabilities *vht_cap,
			   struct ieee80211_vht_capabilities *neg_vht_cap)
{
	errno_t err;
	u32 cap, own_cap, sym_caps;

	if (vht_cap == NULL)
		return;
	os_memcpy(neg_vht_cap, vht_cap, sizeof(*neg_vht_cap));

	cap = le_to_host32(neg_vht_cap->vht_capabilities_info);
	own_cap = hapd->iconf->vht_capab;

	/* mask out symmetric VHT capabilities we don't support */
	sym_caps = VHT_CAP_SHORT_GI_80 | VHT_CAP_SHORT_GI_160;
	cap &= ~sym_caps | (own_cap & sym_caps);

	/* mask out beamformer/beamformee caps if not supported */
	if (!(own_cap & VHT_CAP_SU_BEAMFORMER_CAPABLE))
		cap &= ~(VHT_CAP_SU_BEAMFORMEE_CAPABLE |
			 VHT_CAP_BEAMFORMEE_STS_MAX);

	if (!(own_cap & VHT_CAP_SU_BEAMFORMEE_CAPABLE))
		cap &= ~(VHT_CAP_SU_BEAMFORMER_CAPABLE |
			 VHT_CAP_SOUNDING_DIMENSION_MAX);

	if (!(own_cap & VHT_CAP_MU_BEAMFORMER_CAPABLE))
		cap &= ~VHT_CAP_MU_BEAMFORMEE_CAPABLE;

	if (!(own_cap & VHT_CAP_MU_BEAMFORMEE_CAPABLE))
		cap &= ~VHT_CAP_MU_BEAMFORMER_CAPABLE;

	/* mask channel widths we don't support */
	switch (own_cap & VHT_CAP_SUPP_CHAN_WIDTH_MASK) {
	case VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ:
		break;
	case VHT_CAP_SUPP_CHAN_WIDTH_160MHZ:
		if (cap & VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ) {
			cap &= ~VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ;
			cap |= VHT_CAP_SUPP_CHAN_WIDTH_160MHZ;
		}
		break;
	default:
		cap &= ~VHT_CAP_SUPP_CHAN_WIDTH_MASK;
		break;
	}

	if (!(cap & VHT_CAP_SUPP_CHAN_WIDTH_MASK))
		cap &= ~VHT_CAP_SHORT_GI_160;

	/*
	 * if we don't support RX STBC, mask out TX STBC in the STA's HT caps
	 * if we don't support TX STBC, mask out RX STBC in the STA's HT caps
	 */
	if (!(own_cap & VHT_CAP_RXSTBC_MASK))
		cap &= ~VHT_CAP_TXSTBC;
	if (!(own_cap & VHT_CAP_TXSTBC))
		cap &= ~VHT_CAP_RXSTBC_MASK;

	/* if we don't support RX LDPC, mask out LDPC for TX in the STA's VHT caps */
	if (!(own_cap & VHT_CAP_RXLDPC))
		cap &= ~VHT_CAP_RXLDPC;

	neg_vht_cap->vht_capabilities_info = host_to_le32(cap);

	/* mask out supported MCS set vs our own MCS set */
	{
		u16 our_mcs_set[4];
		struct hostapd_hw_modes *mode = hapd->iface->current_mode;

		if (mode->mode == HOSTAPD_MODE_IEEE80211G && hapd->conf->vendor_vht &&
		    mode->vht_capab == 0 && hapd->iface->hw_features) {
			int i;

			for (i = 0; i < hapd->iface->num_hw_features; i++) {
				if (hapd->iface->hw_features[i].mode ==
				    HOSTAPD_MODE_IEEE80211A) {
					mode = &hapd->iface->hw_features[i];
					break;
				}
			}
		}

		err = memcpy_s(our_mcs_set, sizeof(our_mcs_set), mode->vht_mcs_set, sizeof(mode->vht_mcs_set));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}

		/* RX highest vs our TX highest*/
		neg_vht_cap->vht_supported_mcs_set.rx_highest =
			limit_rx_tx_highest(neg_vht_cap->vht_supported_mcs_set.rx_highest, our_mcs_set[3]);

		/* TX highest vs our RX highest*/
		neg_vht_cap->vht_supported_mcs_set.tx_highest =
			limit_rx_tx_highest(neg_vht_cap->vht_supported_mcs_set.tx_highest, our_mcs_set[1]);

		/* RX map vs our TX map */
		neg_vht_cap->vht_supported_mcs_set.rx_map =
			limit_rx_tx_map(neg_vht_cap->vht_supported_mcs_set.rx_map, our_mcs_set[2]);

		/* TX map vs our RX map */
		neg_vht_cap->vht_supported_mcs_set.tx_map =
			limit_rx_tx_map(neg_vht_cap->vht_supported_mcs_set.tx_map, our_mcs_set[0]);
	}
}
