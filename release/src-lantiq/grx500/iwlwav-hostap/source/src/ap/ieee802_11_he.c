/*
 * hostapd / IEEE 802.11ax HE
 * Copyright (c) 2016-2017, Qualcomm Atheros, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "hostapd.h"
#include "ap_config.h"
#include "sta_info.h"
#include "beacon.h"
#include "ieee802_11.h"
#include "dfs.h"

static int get_he_ppe_size(const u8 *he_capab) {

	unsigned nof_sts, nof_ru, nbits, nbytes;
	const u8 *he_ppe_thresholds = he_capab;

	nof_sts = (he_ppe_thresholds[0] & HE_PPE_CAP0_NSS_M1) + 1;
	nof_ru = count_bits_set(he_ppe_thresholds[0] & HE_PPE_CAP0_RU_INDEX_BITMASK);
	nbits   = 3 /* nsts */ + 4 /* ru_index_bitmask */ + (6 * nof_sts * nof_ru);
	nbytes  = BITS_TO_BYTES(nbits);

	return nbytes;
}

u8 * hostapd_eid_he_capab(struct hostapd_data *hapd, u8 *eid)
{
	struct ieee80211_he_capabilities *cap;
	u8 *pos = eid;
	u8 *he_cap, *size_pos;
	u8 he_txrx_mcs_size;
	u8 size;
	u8 chanwidth;
	struct hostapd_hw_modes *mode = hapd->iface->current_mode;
	errno_t err;

	if (!hapd->iface->current_mode)
		return eid;

	/* Minimize HE Channel Width Set against supported by HW and configured values */
	chanwidth = mode->he_capab.he_phy_capab_info[HE_PHYCAP_CAP0_IDX] &
			hapd->iconf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP0_IDX];

	he_txrx_mcs_size = MIN_HE_MCS_NSS_LEN;

	if(chanwidth & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B3)
		he_txrx_mcs_size += SIZE_HE_MCS_MAP_80_80_MHZ;

	if(chanwidth & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2)
		he_txrx_mcs_size += SIZE_HE_MCS_MAP_160_MHZ;

	size = sizeof(cap->he_mac_capab_info) +
			sizeof(cap->he_phy_capab_info) +
			he_txrx_mcs_size;

	*pos++ = WLAN_EID_EXTENSION;
	size_pos = pos;
	*pos++ = 0; /* Final size is not known yet */
	*pos++ = WLAN_EID_EXT_HE_CAPABILITIES;

	he_cap = pos;
	os_memset(he_cap, 0, sizeof(cap->he_mac_capab_info));
	/* HE MAC Capabilities Information field */
	err = memcpy_s(he_cap,
		 sizeof(cap->he_mac_capab_info),
		 &mode->he_capab.he_mac_capab_info,
		 sizeof(cap->he_mac_capab_info));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return NULL;
	}

	he_cap = (u8 *) (he_cap + sizeof(cap->he_mac_capab_info));
	/* HE PHY Capabilities Information field */
	os_memset(he_cap, 0, sizeof(cap->he_phy_capab_info));
	err = memcpy_s(he_cap,
		 sizeof(cap->he_phy_capab_info),
		 &mode->he_capab.he_phy_capab_info,
		 sizeof(cap->he_phy_capab_info));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return NULL;
	}


	he_cap[HE_PHYCAP_CAP0_IDX] = chanwidth;
	if(hapd->iconf->he_cap_punc_pream_rx_support_override == 1){
		he_cap[HE_PHYCAP_CAP1_IDX] &= ~(HE_PHY_CAP1_PUN_PREAM_RX);
		he_cap[HE_PHYCAP_CAP1_IDX] |=
			(hapd->iconf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP1_IDX] & HE_PHY_CAP1_PUN_PREAM_RX);
	}
	/* Update HE PHY MAX NC field with supported value by HW */
	he_cap[HE_PHYCAP_CAP7_IDX] &= ~(HE_PHY_CAP7_MAX_NC);
	he_cap[HE_PHYCAP_CAP7_IDX] |=
		(mode->he_capab.he_phy_capab_info[HE_PHYCAP_CAP7_IDX] & HE_PHY_CAP7_MAX_NC);

	he_cap = (u8 *) (he_cap + sizeof(cap->he_phy_capab_info));
	/* Supported HE-MCS And NSS Set field */
	os_memset(he_cap, 0, he_txrx_mcs_size);
	err = memcpy_s(he_cap,
		 he_txrx_mcs_size,
		 mode->he_capab.optional,
		 he_txrx_mcs_size);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return NULL;
	}

	if(mode->he_capab.he_phy_capab_info[HE_PHYCAP_CAP6_IDX] &
		HE_PHY_CAP6_PPE_THRESHOLD_PRESENT) {
		he_cap = (u8 *) (he_cap + he_txrx_mcs_size);
		u8 ppe_th_size = get_he_ppe_size((u8 *)(mode->he_capab.optional + HE_PPE_THRES_INDEX));
		os_memset(he_cap, 0, ppe_th_size);
		err = memcpy_s(he_cap,
			 ppe_th_size,
			 (u8 *)(mode->he_capab.optional + HE_PPE_THRES_INDEX),
			 ppe_th_size);
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return NULL;
		}
		size += ppe_th_size;
	}

	*size_pos = 1 + size;
	wpa_hexdump(MSG_DEBUG, "hostapd_eid_he_capab:", pos, size);
	pos += size;

	return pos;
}


u8 * hostapd_eid_he_operation(struct hostapd_data *hapd, u8 *eid)
{
#define HE_6GHZ_OPER_INFO_PRI_CHAN_OFFSET                       0
#define HE_6GHZ_OPER_INFO_CTRL_OFFSET                           1
#define HE_6GHZ_OPER_INFO_SEG0_OFFSET                           2
#define HE_6GHZ_OPER_INFO_SEG1_OFFSET                           3
#define HE_6GHZ_OPER_INFO_MIN_RATE_OFFSET                       4
#define HE_6GHZ_OPER_INFO_CTRL_OFFSET_REG_INFO_SHIFT            3
#define US_STANDARD_POWER_ACCESS_POINT                          1

	struct ieee80211_he_operation *oper;
	u8 *pos = eid;
	u8 *he_oper;
	errno_t err;
	const char country_ref[] = {'U','S'};

	if (!hapd->iface->current_mode)
		return eid;

	u8 size = sizeof(oper->he_oper_params) +
				sizeof(oper->bss_color_info) +
				sizeof(oper->he_mcs_nss_set);

	if(hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX] &
		HE_OPERATION_CAP1_VHT_OPER_INFO_PRESENT) {
		size += sizeof(oper->vht_op_info_chwidth) +
				sizeof(oper->vht_op_info_chan_center_freq_seg0_idx) +
				sizeof(oper->vht_op_info_chan_center_freq_seg1_idx);

		hapd->iface->conf->he_oper.vht_op_info_chwidth =
						hostapd_get_oper_chwidth(hapd->iface->conf);
		hapd->iface->conf->he_oper.vht_op_info_chan_center_freq_seg0_idx =
						hostapd_get_oper_centr_freq_seg0_idx(hapd->iface->conf);
		hapd->iface->conf->he_oper.vht_op_info_chan_center_freq_seg1_idx =
						hostapd_get_oper_centr_freq_seg1_idx(hapd->iface->conf);
	}

	/* For 6GHz co-hosted bss subfiled should be set to 0 */
	if(is_6ghz_op_class(hapd->iconf->op_class)) {
		clr_set_he_cap(&hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX],
					   0, HE_OPERATION_CAP1_CO_LOCATED_BSS);
	}

	if(hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX] &
		HE_OPERATION_CAP1_CO_LOCATED_BSS)
		size += sizeof(oper->max_co_located_bssid_ind);

	if(is_6ghz_op_class(hapd->iconf->op_class)) {
		hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP2_IDX] |=
				set_he_cap(1, HE_OPERATION_CAP2_6GHZ_OPERATION_INFO_PRESENT);
                size += sizeof(oper->he_6gh_operation_information);

	}

	*pos++ = WLAN_EID_EXTENSION;
	*pos++ = 1 + size;
	*pos++ = WLAN_EID_EXT_HE_OPERATION;

	he_oper = pos;
	os_memset(he_oper, 0, size);
	err = memcpy_s(he_oper,
		 sizeof(oper->he_oper_params),
		 &hapd->iface->conf->he_oper.he_oper_params,
		 sizeof(oper->he_oper_params));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return NULL;
	}

	he_oper = (u8 *) (he_oper + sizeof(oper->he_oper_params));
	*he_oper = hapd->iface->conf->he_oper.bss_color_info;

	he_oper = (u8 *) (he_oper + sizeof(oper->bss_color_info));
	err = memcpy_s(he_oper,
		 sizeof(oper->he_mcs_nss_set),
		 &hapd->iface->conf->he_oper.he_mcs_nss_set,
		 sizeof(oper->he_mcs_nss_set));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return NULL;
	}

	he_oper = (u8 *) (he_oper + sizeof(oper->he_mcs_nss_set));
	if(hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX] &
		HE_OPERATION_CAP1_VHT_OPER_INFO_PRESENT) {
		*he_oper++ = hapd->iface->conf->he_oper.vht_op_info_chwidth;
		*he_oper++ =
			hapd->iface->conf->he_oper.vht_op_info_chan_center_freq_seg0_idx;
		*he_oper++ =
			hapd->iface->conf->he_oper.vht_op_info_chan_center_freq_seg1_idx;
	}

	if(hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX] &
		HE_OPERATION_CAP1_CO_LOCATED_BSS)
		*he_oper++ = hapd->iface->conf->he_oper.max_co_located_bssid_ind;

	if(hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP2_IDX] &
		HE_OPERATION_CAP2_6GHZ_OPERATION_INFO_PRESENT) {

		hapd->iface->conf->he_oper.he_6gh_operation_information[HE_6GHZ_OPER_INFO_PRI_CHAN_OFFSET] = hapd->iconf->channel; /* Primary Channel */
		/* Control - Channel width */
		/* seg1 is valid only for 80+80 MHz and 160 MHz */
		if(hapd->iface->conf->he_oper.vht_op_info_chan_center_freq_seg1_idx)
			hapd->iface->conf->he_oper.he_6gh_operation_information[HE_6GHZ_OPER_INFO_CTRL_OFFSET] =
				center_idx_to_bw_6ghz(hapd->iface->conf->he_oper.vht_op_info_chan_center_freq_seg1_idx);
		else
			hapd->iface->conf->he_oper.he_6gh_operation_information[HE_6GHZ_OPER_INFO_CTRL_OFFSET] =
				center_idx_to_bw_6ghz(hapd->iface->conf->he_oper.vht_op_info_chan_center_freq_seg0_idx);
		/* Update the Regulatory Info field only for the US operation */
		if ((hapd->iface->conf->country[0] == country_ref[0]) &&
		    (hapd->iface->conf->country[1] == country_ref[1]) &&
		    (hapd->iface->conf->country[2] != INDOOR_ENVIRONMENT_ONLY))
			hapd->iface->conf->he_oper.he_6gh_operation_information[HE_6GHZ_OPER_INFO_CTRL_OFFSET] |=
				(US_STANDARD_POWER_ACCESS_POINT << HE_6GHZ_OPER_INFO_CTRL_OFFSET_REG_INFO_SHIFT);
			
		hapd->iface->conf->he_oper.he_6gh_operation_information[HE_6GHZ_OPER_INFO_SEG0_OFFSET] =
			hapd->iface->conf->he_oper.vht_op_info_chan_center_freq_seg0_idx; /* seg0 */
		hapd->iface->conf->he_oper.he_6gh_operation_information[HE_6GHZ_OPER_INFO_SEG1_OFFSET] =
			hapd->iface->conf->he_oper.vht_op_info_chan_center_freq_seg1_idx; /* seg1 */
		hapd->iface->conf->he_oper.he_6gh_operation_information[HE_6GHZ_OPER_INFO_MIN_RATE_OFFSET] =
			HE_6G_OPER_INFO_MINIMUM_RATE_NSS; /* Minimum rate with NSS*/
		os_memcpy(he_oper,
		&hapd->iface->conf->he_oper.he_6gh_operation_information,
		sizeof(hapd->iface->conf->he_oper.he_6gh_operation_information));
	}

	wpa_hexdump(MSG_DEBUG, "hostapd_eid_he_operation:", pos, size);

	pos += size;

	return pos;
}

u8 * hostapd_eid_he_mu_edca_parameter_set(struct hostapd_data *hapd, u8 *eid)
{
	struct ieee80211_he_mu_edca_parameter_set *edca;
	u8 *pos;
	size_t i;

	pos = (u8 *) &hapd->iface->conf->he_mu_edca;
	for (i = 0; i < sizeof(*edca); i++) {
		if (pos[i])
			break;
	}
	if (i == sizeof(*edca))
		return eid; /* no MU EDCA Parameters configured */

	pos = eid;
	*pos++ = WLAN_EID_EXTENSION;
	*pos++ = 1 + sizeof(*edca);
	*pos++ = WLAN_EID_EXT_HE_MU_EDCA_PARAMS;

	edca = (struct ieee80211_he_mu_edca_parameter_set *) pos;
	os_memcpy(edca, &hapd->iface->conf->he_mu_edca, sizeof(*edca));

	wpa_hexdump(MSG_DEBUG, "HE: MU EDCA Parameter Set element",
		    pos, sizeof(*edca));

	pos += sizeof(*edca);

	return pos;
}

u8 * hostapd_eid_he_spatial_reuse_parameter_set(struct hostapd_data *hapd, u8 *eid)
{
	struct ieee80211_he_spatial_reuse_parameter_set *spatial_reuse;
	u8 *pos = eid;
	u8 *he_spatial;
	errno_t err;

	if (!hapd->iface->current_mode)
		return eid;

	u8 size = sizeof(spatial_reuse->he_sr_control);

	if(hapd->iface->conf->he_spatial_reuse.he_sr_control & HE_SRP_NON_SRG_OFFSET_PRESENT)
		size += sizeof(spatial_reuse->he_non_srg_obss_pd_max_offset);

	if(hapd->iface->conf->he_spatial_reuse.he_sr_control & HE_SRP_SRG_INFO_PRESENT)
		size += sizeof(spatial_reuse->he_srg_obss_pd_min_offset) +
				sizeof(spatial_reuse->he_srg_obss_pd_max_offset) +
				sizeof(spatial_reuse->he_srg_bss_color_bitmap) +
				sizeof(spatial_reuse->he_srg_partial_bssid_bitmap);

	*pos++ = WLAN_EID_EXTENSION;
	*pos++ = 1 + size;
	*pos++ = WLAN_EID_EXT_HE_SPATIAL_REUSE_SET;

	he_spatial = pos;
	os_memset(he_spatial, 0, size);
	*he_spatial++ = hapd->iface->conf->he_spatial_reuse.he_sr_control;

	if(hapd->iface->conf->he_spatial_reuse.he_sr_control & HE_SRP_NON_SRG_OFFSET_PRESENT)
		*he_spatial++ = hapd->iface->conf->he_spatial_reuse.he_non_srg_obss_pd_max_offset;

	if(hapd->iface->conf->he_spatial_reuse.he_sr_control & HE_SRP_SRG_INFO_PRESENT) {
		*he_spatial++ = hapd->iface->conf->he_spatial_reuse.he_srg_obss_pd_min_offset;
		*he_spatial++ = hapd->iface->conf->he_spatial_reuse.he_srg_obss_pd_max_offset;
		err = memcpy_s(he_spatial,
			 sizeof(spatial_reuse->he_srg_bss_color_bitmap),
			 &hapd->iface->conf->he_spatial_reuse.he_srg_bss_color_bitmap,
			 sizeof(spatial_reuse->he_srg_bss_color_bitmap));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return NULL;	
		}
		he_spatial += sizeof(spatial_reuse->he_srg_bss_color_bitmap);
		err = memcpy_s(he_spatial,
			 sizeof(spatial_reuse->he_srg_partial_bssid_bitmap),
			 &hapd->iface->conf->he_spatial_reuse.he_srg_partial_bssid_bitmap,
			 sizeof(spatial_reuse->he_srg_partial_bssid_bitmap));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return NULL;
		}
	}
	wpa_hexdump(MSG_DEBUG, "HE: Spatial Reuse Parameter Set:", pos, size);

	pos += size;

	return pos;

}

u8 * hostapd_eid_he_ndp_feedback_report_parameters_set(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;
	*pos++ = WLAN_EID_EXTENSION;
	*pos++ = sizeof(u8) * 2;
	*pos++ = WLAN_EID_EXT_HE_NDP_FEEDBACK_REPORT_PARAMETER_SET;
	*pos++ = hapd->iface->conf->he_nfr_buffer_threshold;
	wpa_printf(MSG_DEBUG, "hostapd_eid_he_ndp_feedback_report_parameters_set: 0x%02x",
																					 hapd->iface->conf->he_nfr_buffer_threshold);
	return pos;
}

u8 * hostapd_eid_he_6ghz_band_cap(struct hostapd_data *hapd, u8 *eid)
{
	struct hostapd_hw_modes *mode = hapd->iface->current_mode;
	struct ieee80211_he_6ghz_band_cap *cap;
	u16 capab;
	u8 *pos;

	if (!mode || !is_6ghz_op_class(hapd->iconf->op_class) ||
	    !is_6ghz_freq(hapd->iface->freq))
		return eid;

	capab = (mode->he_6ghz_capab).capab;
	capab |= HE_6GHZ_BAND_CAP_SMPS_DISABLED;

	pos = eid;
	*pos++ = WLAN_EID_EXTENSION;
	*pos++ = 1 + sizeof(*cap);
	*pos++ = WLAN_EID_EXT_HE_6GHZ_BAND_CAPABILITIES;

	cap = (struct ieee80211_he_6ghz_band_cap *) pos;
	cap->capab = host_to_le16(capab);
	pos += sizeof(*cap);

	return pos;
}


/*
  Given bit pairs a0a1 and b0b1:
  If a0a1 == 3 or b0b1 == 3:
    c0c1 = 3
  else c0c1 = min(a0a1, b0b1)
  (c0c1 being the output)
 */
void hostapd_get_he_mcs_nss(const u8 *our_he_mcs_nss,
								const u8 *sta_he_mcs_nss, u8 *out_he_mcs_nss)
{
	u8 i = 0;
	while(i < 2) {
		u8 j, k, a, b, c;
		for(j = 0, k = 3; j < 4; j++, k = k<<2) {
			a = (our_he_mcs_nss[i] & k)>>(j*2);
			b = (sta_he_mcs_nss[i] & k)>>(j*2);
			c = MIN(a,b);
			if((a == 3) || (b == 3))
				out_he_mcs_nss[i] |= k;
			else
				out_he_mcs_nss[i] |= (k & c<<(j*2));
		}
		i++;
	}
}

static int get_he_mac_phy_size() {
	struct ieee80211_he_capabilities he_cap;
	return (sizeof(he_cap.he_mac_capab_info) + sizeof(he_cap.he_phy_capab_info));
}

int get_he_mcs_nss_size(const u8 *he_capab) {
	const struct ieee80211_he_capabilities *sta_he_capab
		= (struct ieee80211_he_capabilities*)he_capab;
	u8 he_txrx_mcs_size = MIN_HE_MCS_NSS_LEN;

	if(sta_he_capab->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] &
                HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2)
		he_txrx_mcs_size += SIZE_HE_MCS_MAP_160_MHZ;

	if(sta_he_capab->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] &
                HE_PHY_CAP0_CHANNEL_WIDTH_SET_B3)
		he_txrx_mcs_size += SIZE_HE_MCS_MAP_80_80_MHZ;

	return he_txrx_mcs_size;
}

int get_he_ppe_th_size(const u8 *he_capab) {

	const struct ieee80211_he_capabilities *sta_he_capab
		 = (struct ieee80211_he_capabilities*)he_capab;

	if(sta_he_capab->he_phy_capab_info[HE_PHYCAP_CAP6_IDX] &
		HE_PHY_CAP6_PPE_THRESHOLD_PRESENT) {
		u8 *he_ppe_thresholds =
			(u8*)(he_capab + get_he_mac_phy_size() + get_he_mcs_nss_size(he_capab));

		return get_he_ppe_size(he_ppe_thresholds);
	}
	return 0;
}

static int check_valid_he_ie_length(const u8 *he_capab, u8 he_capab_len)
{
	struct ieee80211_he_capabilities *sta_he_capab =
		(struct ieee80211_he_capabilities*)he_capab;

	u8 min_he_capab_len = sizeof(sta_he_capab->he_mac_capab_info) +
                                                        sizeof(sta_he_capab->he_phy_capab_info);

	min_he_capab_len += get_he_mcs_nss_size((u8 *)sta_he_capab);
	min_he_capab_len += get_he_ppe_th_size((u8 *)sta_he_capab);

	if(he_capab_len != min_he_capab_len) {
		wpa_printf(MSG_DEBUG, "HE IE length (%d)!=(%d)", he_capab_len, min_he_capab_len);
		wpa_hexdump(MSG_DEBUG, "HE IE received from STA:", he_capab, he_capab_len);
	}

	if(he_capab_len < min_he_capab_len)
		return 1;

	return 0;
}

u16 copy_sta_he_capab(struct hostapd_data *hapd, struct sta_info *sta,
			const u8 *he_capab, u8 he_capab_len)
{
	errno_t err;
	if (!he_capab || hapd->conf->disable_11ax || !hapd->iconf->ieee80211ax ||
		check_valid_he_ie_length(he_capab, he_capab_len)) {
		sta->flags &= ~WLAN_STA_HE;
		os_free(sta->he_capabilities);
		sta->he_capabilities = NULL;
		wpa_printf(MSG_DEBUG, "copy_sta_he_capab - he_capab: %p", he_capab);
		wpa_printf(MSG_DEBUG, "copy_sta_he_capab - disable_11ax: %d", hapd->conf->disable_11ax);
		return WLAN_STATUS_SUCCESS;
	}


	if (sta->he_capabilities == NULL) {
		sta->he_capabilities =
			os_zalloc(sizeof(struct ieee80211_he_capabilities));
		if (sta->he_capabilities == NULL)
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	sta->flags |= WLAN_STA_HE;
	u8 he_mac_phy_size = get_he_mac_phy_size();
	UNUSED_VAR(he_mac_phy_size);
	wpa_printf(MSG_DEBUG, "copy_sta_he_capab - he_mac_phy_size:%d", he_mac_phy_size);
	u8 mcs_nss_size = get_he_mcs_nss_size(he_capab);
	wpa_printf(MSG_DEBUG, "copy_sta_he_capab - mcs_nss_size:%d", mcs_nss_size);
	u8 ppe_th_size = get_he_ppe_th_size(he_capab);
	wpa_printf(MSG_DEBUG, "copy_sta_he_capab - ppe_th_size:%d", ppe_th_size);

	const u8 *he_capabilities = he_capab;

	err = memcpy_s((void*)&sta->he_capabilities->he_mac_capab_info,
		 sizeof(sta->he_capabilities->he_mac_capab_info),
		 he_capabilities,
		 sizeof(sta->he_capabilities->he_mac_capab_info));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		goto fail;
	}
	he_capabilities += sizeof(sta->he_capabilities->he_mac_capab_info);

	err = memcpy_s((void*)&sta->he_capabilities->he_phy_capab_info,
		 sizeof(sta->he_capabilities->he_phy_capab_info),
		 he_capabilities,
		 sizeof(sta->he_capabilities->he_phy_capab_info));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		goto fail;
	}
	he_capabilities += sizeof(sta->he_capabilities->he_phy_capab_info);

	if(mcs_nss_size) {
		err = memcpy_s((void*)&sta->he_capabilities->optional,
                         mcs_nss_size,
                         he_capabilities,
                         mcs_nss_size);
                if (EOK != err) {
                        wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
                        goto fail;
                }
		he_capabilities += mcs_nss_size;

	}


	err = memcpy_s((void*)&sta->he_capabilities->optional[mcs_nss_size],
                 ppe_th_size,
                 he_capabilities,
                 ppe_th_size);
        if (EOK != err) {
                wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
                goto fail;
        }


	sta->he_capabilities_len_from_sta = he_capab_len;
	wpa_hexdump(MSG_DEBUG, "copy_sta_he_capab - he_capabilities", he_capab, he_capab_len);
	wpa_hexdump(MSG_DEBUG, "copy_sta_he_capab - he_capabilities copied", sta->he_capabilities, sizeof(*sta->he_capabilities));
	return WLAN_STATUS_SUCCESS;

fail:
	os_free(sta->he_capabilities);
	return WLAN_STATUS_UNSPECIFIED_FAILURE;
}

u16 copy_sta_he_operation(struct hostapd_data *hapd, struct sta_info *sta,
			const u8 *he_operation, u8 he_operation_len) {
	errno_t err;

	if (!he_operation || hapd->conf->disable_11ax || !hapd->iconf->ieee80211ax) {
		os_free(sta->he_operation);
		sta->he_operation = NULL;
		return WLAN_STATUS_SUCCESS;
	}

	if (sta->he_operation == NULL) {
		sta->he_operation =
			os_zalloc(sizeof(struct ieee80211_he_operation));
		if (sta->he_operation == NULL)
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	u8 he_operation_copy_size = MIN(sizeof(struct ieee80211_he_operation), he_operation_len);
	err = memcpy_s((void*)sta->he_operation,
		 sizeof(struct ieee80211_he_operation),
		 he_operation,
		 he_operation_copy_size);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		os_free(sta->he_operation);
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}
	sta->he_operation_len_from_sta = he_operation_copy_size;

	if(he_operation_copy_size != he_operation_len)
		wpa_printf(MSG_DEBUG, "copy_sta_he_operation - he_operation_copy_size (%d) !=  he_operation_len (%d)",
							 he_operation_copy_size, he_operation_len);

	wpa_hexdump(MSG_DEBUG, "copy_sta_he_operation - he_operation", he_operation, sta->he_operation_len_from_sta);

	return WLAN_STATUS_SUCCESS;
}

u16 copy_sta_he_6ghz_capab(struct hostapd_data *hapd, struct sta_info *sta,
			   const u8 *he_6ghz_capab)
{
	if (!he_6ghz_capab || !hapd->iconf->ieee80211ax ||
	    !is_6ghz_op_class(hapd->iconf->op_class)) {
		sta->flags &= ~WLAN_STA_6GHZ;
		os_free(sta->he_6ghz_capab);
		sta->he_6ghz_capab = NULL;
		return WLAN_STATUS_SUCCESS;
	}

	if (!sta->he_6ghz_capab) {
		sta->he_6ghz_capab =
			os_zalloc(sizeof(struct ieee80211_he_6ghz_band_cap));
		if (!sta->he_6ghz_capab)
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	sta->flags |= WLAN_STA_6GHZ;
	os_memcpy(sta->he_6ghz_capab, he_6ghz_capab,
		  sizeof(struct ieee80211_he_6ghz_band_cap));
	sta->he_6ghz_capab->capab = host_to_le16(sta->he_6ghz_capab->capab);

	return WLAN_STATUS_SUCCESS;
}

void hostapd_get_he_capab(struct hostapd_data *hapd,
			  const struct ieee80211_he_capabilities *sta_caps,
			  struct ieee80211_he_capabilities *output_caps,
			  u32 he_capabilities_len_from_sta)
{
	errno_t err;
	if (!sta_caps) {
		wpa_printf(MSG_ERROR, "No matching HE capabilities from driver");
		return;
	}

	u8 chanwidth;
	struct hostapd_hw_modes *mode = hapd->iface->current_mode;

	u8 mcs_nss_size,mcs_nss_size_from_sta,ppe_thresh_size;

	os_memset(output_caps, 0, sizeof(*output_caps));
	chanwidth = mode->he_capab.he_phy_capab_info[HE_PHYCAP_CAP0_IDX] &
		hapd->iconf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP0_IDX];
	err = memcpy_s(output_caps, sizeof(*output_caps), sta_caps, he_capabilities_len_from_sta);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return;
	}

	output_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] = 0;
	output_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] |=
		min_he_cap(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX],
		chanwidth,
		HE_PHY_CAP0_CHANNEL_WIDTH_SET_B0);
	output_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] |=
		min_he_cap(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX],
		chanwidth,
		HE_PHY_CAP0_CHANNEL_WIDTH_SET_B1);
	output_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] |=
		min_he_cap(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX],
		chanwidth,
		HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2);
	output_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] |=
		min_he_cap(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX],
		chanwidth,
		HE_PHY_CAP0_CHANNEL_WIDTH_SET_B3);
	if(~((sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B0) |
			(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B1) |
			(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2) |
			(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B3)) &
			(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP8_IDX] & HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_2_4_GHZ_BAND)) {
			output_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] |=
				min_he_cap(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX],
				chanwidth,
				HE_PHY_CAP0_CHANNEL_WIDTH_SET_B4);
	}
	if(~((sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B0) |
			(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B1) |
			(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2) |
			(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B3)) &
			(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP8_IDX] & HE_PHY_CAP8_20MHZ_IN_160MHZ_HE_PPDU)) {
			output_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] |=
				min_he_cap(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX],
				chanwidth,
				HE_PHY_CAP0_CHANNEL_WIDTH_SET_B5);
	}
	output_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] |=
		min_he_cap(sta_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX],
		chanwidth,
		HE_PHY_CAP0_CHANNEL_WIDTH_SET_B6);

	mcs_nss_size = get_he_mcs_nss_size((u8 *)output_caps);
	mcs_nss_size_from_sta = get_he_mcs_nss_size((u8 *)sta_caps);
	ppe_thresh_size = get_he_ppe_th_size((u8 *)sta_caps);

	os_memset((u8 *)(output_caps->optional + mcs_nss_size),0,HE_PPE_THRES_MAX_LEN);

	err = memcpy_s((u8 *)(output_caps->optional + mcs_nss_size),
				ppe_thresh_size,
				(u8 *)(sta_caps->optional + mcs_nss_size_from_sta), ppe_thresh_size);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return;
	}

	wpa_hexdump(MSG_DEBUG, "STA HE elements received:", sta_caps, he_capabilities_len_from_sta);
	wpa_hexdump(MSG_DEBUG, "STA HE elements after ChannelWidth and MCS-NSS negotiation:", output_caps, sizeof(*output_caps));

}
