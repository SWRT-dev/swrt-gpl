/*
 * hostapd / IEEE 802.11 Management: Beacon and Probe Request/Response
 * Copyright (c) 2002-2004, Instant802 Networks, Inc.
 * Copyright (c) 2005-2006, Devicescape Software, Inc.
 * Copyright (c) 2008-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#ifndef CONFIG_NATIVE_WINDOWS

#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "common/hw_features_common.h"
#include "common/wpa_ctrl.h"
#include "wps/wps_defs.h"
#include "p2p/p2p.h"
#include "hostapd.h"
#include "ieee802_11.h"
#include "wpa_auth.h"
#include "wmm.h"
#include "ap_config.h"
#include "sta_info.h"
#include "p2p_hostapd.h"
#include "ap_drv_ops.h"
#include "beacon.h"
#include "hs20.h"
#include "dfs.h"
#include "taxonomy.h"
#include "ieee802_11_auth.h"
#include "crc32.h"
#include "ap/hw_features.h"

#ifdef NEED_AP_MLME

static u8 * hostapd_eid_bss_load(struct hostapd_data *hapd, u8 *eid, size_t len)
{
	if (len < 2 + 5)
		return eid;

#ifdef CONFIG_TESTING_OPTIONS
	if (hapd->conf->bss_load_test_set) {
		*eid++ = WLAN_EID_BSS_LOAD;
		*eid++ = 5;
		os_memcpy(eid, hapd->conf->bss_load_test, 5);
		eid += 5;
		return eid;
	}
#endif /* CONFIG_TESTING_OPTIONS */

	if (!hapd->conf->hs20 && !hapd->conf->enable_bss_load_ie)
		return eid;

	/* generated BSS Load IE, will be updated by driver */
	*eid++ = WLAN_EID_BSS_LOAD;
	*eid++ = 5;
	eid += 5;
	return eid;
}


static u8 ieee802_11_erp_info(struct hostapd_data *hapd)
{
	u8 erp = 0;

	if (hapd->iface->current_mode == NULL ||
	    hapd->iface->current_mode->mode != HOSTAPD_MODE_IEEE80211G)
		return 0;

	if (hapd->iface->olbc)
		erp |= ERP_INFO_USE_PROTECTION;
	if (hapd->iface->num_sta_non_erp > 0) {
		erp |= ERP_INFO_NON_ERP_PRESENT |
			ERP_INFO_USE_PROTECTION;
	}
	if (hapd->iface->num_sta_no_short_preamble > 0 ||
	    hapd->iconf->preamble == LONG_PREAMBLE)
		erp |= ERP_INFO_BARKER_PREAMBLE_MODE;

	return erp;
}


static u8 * hostapd_eid_ds_params(struct hostapd_data *hapd, u8 *eid)
{
	*eid++ = WLAN_EID_DS_PARAMS;
	*eid++ = 1;
	*eid++ = hapd->iconf->channel;
	return eid;
}


static u8 * hostapd_eid_erp_info(struct hostapd_data *hapd, u8 *eid)
{
	if (hapd->iface->current_mode == NULL ||
	    hapd->iface->current_mode->mode != HOSTAPD_MODE_IEEE80211G)
		return eid;

	/* Set NonERP_present and use_protection bits if there
	 * are any associated NonERP stations. */
	/* TODO: use_protection bit can be set to zero even if
	 * there are NonERP stations present. This optimization
	 * might be useful if NonERP stations are "quiet".
	 * See 802.11g/D6 E-1 for recommended practice.
	 * In addition, Non ERP present might be set, if AP detects Non ERP
	 * operation on other APs. */

	/* Add ERP Information element */
	*eid++ = WLAN_EID_ERP_INFO;
	*eid++ = 1;
	*eid++ = ieee802_11_erp_info(hapd);

	return eid;
}


static u8 * hostapd_eid_pwr_constraint(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;
	u8 local_pwr_constraint = 0;
	int dfs;

	if (hapd->iface->current_mode == NULL ||
	    hapd->iface->current_mode->mode != HOSTAPD_MODE_IEEE80211A)
		return eid;

	/* Let host drivers add this IE if DFS support is offloaded */
	if (hapd->iface->drv_flags & WPA_DRIVER_FLAGS_DFS_OFFLOAD)
		return eid;

	/*
	 * There is no DFS support and power constraint was not directly
	 * requested by config option.
	 */
	if (!hapd->iconf->ieee80211h &&
	    hapd->iconf->local_pwr_constraint == -1)
		return eid;

	/* Check if DFS is required by regulatory. */
	dfs = hostapd_is_dfs_required(hapd->iface);
	if (dfs < 0) {
		wpa_printf(MSG_WARNING, "Failed to check if DFS is required; ret=%d",
			   dfs);
		dfs = 0;
	}

	if (dfs == 0 && hapd->iconf->local_pwr_constraint == -1)
		return eid;

	/*
	 * ieee80211h (DFS) is enabled so Power Constraint element shall
	 * be added when running on DFS channel whenever local_pwr_constraint
	 * is configured or not. In order to meet regulations when TPC is not
	 * implemented using a transmit power that is below the legal maximum
	 * (including any mitigation factor) should help. In this case,
	 * indicate 3 dB below maximum allowed transmit power.
	 */
	if (hapd->iconf->local_pwr_constraint == -1)
		local_pwr_constraint = 3;

	/*
	 * A STA that is not an AP shall use a transmit power less than or
	 * equal to the local maximum transmit power level for the channel.
	 * The local maximum transmit power can be calculated from the formula:
	 * local max TX pwr = max TX pwr - local pwr constraint
	 * Where max TX pwr is maximum transmit power level specified for
	 * channel in Country element and local pwr constraint is specified
	 * for channel in this Power Constraint element.
	 */

	/* Element ID */
	*pos++ = WLAN_EID_PWR_CONSTRAINT;
	/* Length */
	*pos++ = 1;
	/* Local Power Constraint */
	if (local_pwr_constraint)
		*pos++ = local_pwr_constraint;
	else
		*pos++ = hapd->iconf->local_pwr_constraint;

	return pos;
}


static u8 * hostapd_eid_country_add(u8 *pos, u8 *end, int chan_spacing,
				    struct hostapd_channel_data *start,
				    struct hostapd_channel_data *prev,
					bool op_class_6g)
{
	if (end - pos < 3)
		return pos;

	/* first channel number */
	*pos++ = start->chan;
	/* number of channels */
	*pos++ = (prev->chan - start->chan) / chan_spacing + 1;
	/* maximum transmit power level */
	if (!op_class_6g)
		*pos++ = start->max_tx_power;
	else
		*pos++ = 0; /*Reserved for 6GHz band */

	return pos;
}


static u8 * hostapd_eid_country(struct hostapd_data *hapd, u8 *eid,
				int max_len)
{
#define COUNTRY_STRING_OCTET_3                       2
#define TABLE_E4_GLOBAL_OP_CLASSES                   4
#define COUNTRY_OPERATING_TRIPLET_EXT_ID             234
#define COUNTRY_OPERATING_TRIPLET_COVERAGE_CLASS_0   0
#define OPERATING_CLASS_6GHZ_20MHZ_BW                131

	u8 *pos = eid;
	u8 *end = eid + max_len;
	int i;
	struct hostapd_hw_modes *mode;
	struct hostapd_channel_data *start, *prev;
	int chan_spacing = 1;
	bool op_class_6g = false;

	if (!hapd->iconf->ieee80211d || max_len < 6 ||
	    hapd->iface->current_mode == NULL)
		return eid;

	*pos++ = WLAN_EID_COUNTRY;
	pos++; /* length will be set later */
	os_memcpy(pos, hapd->iconf->country, 3); /* e.g., 'US ' */
	pos += 3;

	mode = hapd->iface->current_mode;
	if (mode->mode == HOSTAPD_MODE_IEEE80211A)
		chan_spacing = 4;

	op_class_6g = is_6ghz_op_class(hapd->iconf->op_class);
	if (op_class_6g) {
		hapd->iconf->country[COUNTRY_STRING_OCTET_3] = TABLE_E4_GLOBAL_OP_CLASSES;
		*pos++ = COUNTRY_OPERATING_TRIPLET_EXT_ID; /* Operating Extn ID */
		*pos++ = hapd->iconf->op_class; /* Operating class */
		*pos++ = COUNTRY_OPERATING_TRIPLET_COVERAGE_CLASS_0; /*Coverage Class */
	}
	if (!op_class_6g || hapd->iconf->op_class == OPERATING_CLASS_6GHZ_20MHZ_BW) {
		start = prev = NULL;
		for (i = 0; i < mode->num_channels; i++) {
			struct hostapd_channel_data *chan = &mode->channels[i];
			if (chan->flag & HOSTAPD_CHAN_DISABLED)
				continue;
			if (start && prev &&
				prev->chan + chan_spacing == chan->chan &&
				start->max_tx_power == chan->max_tx_power) {
				prev = chan;
				continue; /* can use same entry */
			}

			if (start && prev) {
				pos = hostapd_eid_country_add(pos, end, chan_spacing,
								  start, prev, op_class_6g);
				start = NULL;
			}

			/* Start new group */
			start = prev = chan;
		}

		if (start) {
			pos = hostapd_eid_country_add(pos, end, chan_spacing,
							  start, prev, op_class_6g);
		}
	}
	if ((pos - eid) & 1) {
		if (end - pos < 1)
			return eid;
		*pos++ = 0; /* pad for 16-bit alignment */
	}

	eid[1] = (pos - eid) - 2;

	return pos;
}


static const u8 * hostapd_wpa_ie(struct hostapd_data *hapd, u8 eid)
{
	const u8 *ies;
	size_t ies_len;

	ies = wpa_auth_get_wpa_ie(hapd->wpa_auth, &ies_len);
	if (!ies)
		return NULL;

	return get_ie(ies, ies_len, eid);
}


static const u8 * hostapd_vendor_wpa_ie(struct hostapd_data *hapd,
					u32 vendor_type)
{
	const u8 *ies;
	size_t ies_len;

	ies = wpa_auth_get_wpa_ie(hapd->wpa_auth, &ies_len);
	if (!ies)
		return NULL;

	return get_vendor_ie(ies, ies_len, vendor_type);
}


static u8 * hostapd_get_rsne(struct hostapd_data *hapd, u8 *pos, size_t len)
{
	const u8 *ie;

	ie = hostapd_wpa_ie(hapd, WLAN_EID_RSN);
	if (!ie || 2U + ie[1] > len)
		return pos;

	os_memcpy(pos, ie, 2 + ie[1]);
	return pos + 2 + ie[1];
}


static u8 * hostapd_get_mde(struct hostapd_data *hapd, u8 *pos, size_t len)
{
	const u8 *ie;

	ie = hostapd_wpa_ie(hapd, WLAN_EID_MOBILITY_DOMAIN);
	if (!ie || 2U + ie[1] > len)
		return pos;

	os_memcpy(pos, ie, 2 + ie[1]);
	return pos + 2 + ie[1];
}


static u8 * hostapd_get_rsnxe(struct hostapd_data *hapd, u8 *pos, size_t len)
{
	const u8 *ie;

#ifdef CONFIG_TESTING_OPTIONS
	if (hapd->conf->no_beacon_rsnxe) {
		wpa_printf(MSG_INFO, "TESTING: Do not add RSNXE into Beacon");
		return pos;
	}
#endif /* CONFIG_TESTING_OPTIONS */
	ie = hostapd_wpa_ie(hapd, WLAN_EID_RSNX);
	if (!ie || 2U + ie[1] > len)
		return pos;

	os_memcpy(pos, ie, 2 + ie[1]);
	return pos + 2 + ie[1];
}


static u8 * hostapd_get_wpa_ie(struct hostapd_data *hapd, u8 *pos, size_t len)
{
	const u8 *ie;

	ie = hostapd_vendor_wpa_ie(hapd, WPA_IE_VENDOR_TYPE);
	if (!ie || 2U + ie[1] > len)
		return pos;

	os_memcpy(pos, ie, 2 + ie[1]);
	return pos + 2 + ie[1];
}


static u8 * hostapd_get_osen_ie(struct hostapd_data *hapd, u8 *pos, size_t len)
{
	const u8 *ie;

	ie = hostapd_vendor_wpa_ie(hapd, OSEN_IE_VENDOR_TYPE);
	if (!ie || 2U + ie[1] > len)
		return pos;

	os_memcpy(pos, ie, 2 + ie[1]);
	return pos + 2 + ie[1];
}


static u8 * hostapd_eid_csa(struct hostapd_data *hapd, u8 *eid)
{
#ifdef CONFIG_TESTING_OPTIONS
	if (hapd->iface->cs_oper_class && hapd->iconf->ecsa_ie_only)
		return eid;
#endif /* CONFIG_TESTING_OPTIONS */

	if (!hapd->cs_freq_params.channel)
		return eid;

	*eid++ = WLAN_EID_CHANNEL_SWITCH;
	*eid++ = 3;
	*eid++ = hapd->cs_block_tx;
	*eid++ = hapd->cs_freq_params.channel;
	*eid++ = hapd->cs_count;

	return eid;
}


static u8 * hostapd_eid_ecsa(struct hostapd_data *hapd, u8 *eid)
{
	if (!hapd->cs_freq_params.channel || !hapd->iface->cs_oper_class)
		return eid;

	*eid++ = WLAN_EID_EXT_CHANSWITCH_ANN;
	*eid++ = 4;
	*eid++ = hapd->cs_block_tx;
	*eid++ = hapd->iface->cs_oper_class;
	*eid++ = hapd->cs_freq_params.channel;
	*eid++ = hapd->cs_count;

	return eid;
}


static u8 * hostapd_eid_supported_op_classes(struct hostapd_data *hapd, u8 *eid)
{
	u8 op_class, channel;

	if (!(hapd->iface->drv_flags & WPA_DRIVER_FLAGS_AP_CSA) ||
	    !hapd->iface->freq)
		return eid;

	if (ieee80211_freq_to_channel_ext(hapd->iface->freq,
					  hapd->iconf->secondary_channel,
					  hostapd_get_oper_chwidth(hapd->iconf),
					  &op_class, &channel) ==
	    NUM_HOSTAPD_MODES)
		return eid;

	*eid++ = WLAN_EID_SUPPORTED_OPERATING_CLASSES;
	*eid++ = 2;

	/* Current Operating Class */
    if (is_6ghz_op_class(hapd->iface->conf->op_class))
		*eid++ = hapd->iface->conf->op_class;
	else
		*eid++ = op_class;

	/* TODO: Advertise all the supported operating classes */
	*eid++ = 0;

	return eid;
}

static u8 count_same_ssid_vap(int radio, struct hostapd_data *hapd)
{
	int j=0, count=0;
	u8 empty_bssid[ETH_ALEN] = {'\0'};
	for (  j = 0; j < sizeof(hapd->iconf->coloc_6g_ap_info[radio].bss_info)/sizeof(neigh_ap_info_field_t); j++ ) 
	{
		if( !os_memcmp( hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].bssid, empty_bssid, ETH_ALEN)) 
			continue; // not valid entry, skip 
		if( RNR_TBTT_INFO_BSS_PARAM_SAME_SSID & hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].BssParams )
			count++;
	}
	return count;
}

static u8 count_different_ssid_vap(int radio, struct hostapd_data *hapd)
{
	int j=0, count=0;
	u8 empty_bssid[ETH_ALEN] = {'\0'};
	for (  j = 0; j < sizeof(hapd->iconf->coloc_6g_ap_info[radio].bss_info)/sizeof(neigh_ap_info_field_t); j++ ) 
	{
		if( !os_memcmp( hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].bssid, empty_bssid, ETH_ALEN)) 
			continue; // not valid entry, skip 
		if( ! ( RNR_TBTT_INFO_BSS_PARAM_SAME_SSID & hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].BssParams ) )
			count++;
	}
	return count;
}

static void create_rnr_vap(u8 **pos, int radio, struct hostapd_data *hapd, bool is_same_ssid)
{
	u8 empty_bssid[ETH_ALEN] = {'\0'};
	size_t j;
	struct ieee80211_neighbor_vap_info *rnr_vap;

	for (  j = 0; j < sizeof(hapd->iconf->coloc_6g_ap_info[radio].bss_info)/sizeof(neigh_ap_info_field_t); j++ ) 
	{
		if( !os_memcmp( hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].bssid, empty_bssid, ETH_ALEN)) 
			continue; // not valid entry, skip 

		if (  !is_same_ssid  && 
			( RNR_TBTT_INFO_BSS_PARAM_SAME_SSID & hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].BssParams) )
		{
			continue;	
		}
	
		if ( is_same_ssid  
			&& !( RNR_TBTT_INFO_BSS_PARAM_SAME_SSID & hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].BssParams) )
		{
			continue;
		}

		rnr_vap = (struct ieee80211_neighbor_vap_info *)*pos;
		rnr_vap->neighbor_ap_tbtt_offset |= hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].NeighApTbttOffset; 
		os_memcpy(rnr_vap->neighbor_ap_bssid,hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].bssid, ETH_ALEN);
		*pos += sizeof(*rnr_vap);
	
		if ( !is_same_ssid )
		{
			os_memcpy(*pos, hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].ShortSsid,
			sizeof(hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].ShortSsid));
			*pos += sizeof(hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].ShortSsid);
		}
		os_memcpy(*pos, &hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].BssParams, sizeof(u8)); 
		*pos += sizeof(u8);

		os_memcpy(*pos, &hapd->iconf->coloc_6g_ap_info[radio].bss_info[j].psd_20MHz, sizeof(u8)); 
		*pos += sizeof(u8);
	}
}

static void create_rnr(u8 **pos, int radio, struct hostapd_data *hapd, int count, bool is_same_ssid)
{
	struct ieee80211_neighbor_ap_info *rnr;
	rnr = (struct ieee80211_neighbor_ap_info *)*pos;
	rnr->tbtt_info_hdr = 0; /* Initialize */
	rnr->tbtt_info_hdr |=  hapd->iconf->coloc_6g_ap_info[radio].Reserved;
	rnr->tbtt_info_hdr |=  hapd->iconf->coloc_6g_ap_info[radio].TbttInfoFieldType;
	rnr->channel = hapd->iconf->coloc_6g_ap_info[radio].ChanNum;
	rnr->op_class = hapd->iconf->coloc_6g_ap_info[radio].OperatingClass;
	rnr->tbtt_info_hdr |= ( ( count - 1 ) << RNR_TBTT_INFO_COUNT_OFFSET );
	if ( is_same_ssid )
	{ 
		/* TBTT offset + BSSID + BSS param */
		rnr->tbtt_info_len = RNR_TBTT_INFO_LEN_SAME_SSID;
	}
	else {
		/* TBTT offset + BSSID + Short SSID + BSS param */
		rnr->tbtt_info_len = RNR_TBTT_INFO_LEN_DIFFERENT_SSID;
	}
	*pos += sizeof(*rnr);

	create_rnr_vap(pos,radio,hapd,is_same_ssid);
}

/*
 * Add Reduced Neighbor Report element to Beacons and Probe Responses
 */
static u8 * hostapd_eid_rnr(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;
	u8 *len;
	size_t i,same_ssid_vap, different_ssid_vap;

	*pos++ = WLAN_EID_REDUCED_NEIGHBOR_REPORT;
	len = pos++;

	for( i = 0;  i < sizeof(hapd->iconf->coloc_6g_ap_info)/sizeof(colocated_6g_ap_info_t) ; i++ )
	{
		same_ssid_vap = count_same_ssid_vap(i, hapd);
		different_ssid_vap = count_different_ssid_vap(i, hapd);
		if ( !same_ssid_vap && !different_ssid_vap )
			continue;
		if ( different_ssid_vap )
			create_rnr(&pos, i, hapd, different_ssid_vap, false);	
		if ( same_ssid_vap )
			create_rnr(&pos, i, hapd, same_ssid_vap, true);		
	}
	*len = pos - eid - 2;

	/* don't add empty IE */
	if (*len == 0)
		return eid;

	return pos;
}



static u8 * hostapd_gen_probe_resp(struct hostapd_data *hapd,
				   const struct ieee80211_mgmt *req,
				   int is_p2p, int is_he_in_req,
				   size_t *resp_len)
{
	struct ieee80211_mgmt *resp;
	u8 *pos, *epos, *csa_pos;
	size_t buflen;
	bool is_req_non_transmit_vap = false;
	int multiple_bssid_ie_len = -1;

#define DEFAULT_PROBERESP_LEN 768
    buflen = DEFAULT_PROBERESP_LEN;

	/* if multibss is enabled, probe response is always sent from transmit bss */
	if (hapd->iconf->multibss_enable) {
		if (req || (MULTIBSS_REFERENCE_BSS_IDX == hapd->conf->bss_idx)) {
			if (MULTIBSS_REFERENCE_BSS_IDX != hapd->conf->bss_idx) {
				is_req_non_transmit_vap = true;
				hapd = hapd->iface->bss[MULTIBSS_REFERENCE_BSS_IDX];
			}
			multiple_bssid_ie_len = multiple_bssid_ie_get_len(hapd);
			if (multiple_bssid_ie_len < 0)
				return NULL;
			buflen += multiple_bssid_ie_len;
		}
	}

#ifdef CONFIG_WPS
	if (hapd->wps_probe_resp_ie)
		buflen += wpabuf_len(hapd->wps_probe_resp_ie);
#endif /* CONFIG_WPS */
#ifdef CONFIG_P2P
	if (hapd->p2p_probe_resp_ie)
		buflen += wpabuf_len(hapd->p2p_probe_resp_ie);
#endif /* CONFIG_P2P */
#ifdef CONFIG_FST
	if (hapd->iface->fst_ies)
		buflen += wpabuf_len(hapd->iface->fst_ies);
#endif /* CONFIG_FST */
	if (hapd->conf->vendor_elements)
		buflen += wpabuf_len(hapd->conf->vendor_elements);
	if (hapd->conf->vendor_vht && !hapd->conf->disable_11ac) {
		buflen += 5 + 2 + sizeof(struct ieee80211_vht_capabilities) +
			2 + sizeof(struct ieee80211_vht_operation);
	}

	if (hapd->iconf->ieee80211ax && !hapd->conf->disable_11ax) {
		if (is_he_in_req || !hapd->iconf->ieee80211n_acax_compat) {
			buflen += (3 + sizeof(struct ieee80211_he_capabilities) +
				  3 + sizeof(struct ieee80211_he_operation) +
				  3 + sizeof(struct ieee80211_he_mu_edca_parameter_set));
		}
	}


	buflen += hostapd_mbo_ie_len(hapd);
	buflen += hostapd_eid_owe_trans_len(hapd);
	buflen += hostapd_eid_dpp_cc_len(hapd);

	resp = os_zalloc(buflen);
	if (resp == NULL)
		return NULL;

	epos = ((u8 *) resp) + buflen;

	resp->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
					   WLAN_FC_STYPE_PROBE_RESP);
	if (req)
	{
		/*  6GHz probe response must have broadcast destination unless the BSS is hidden */
		if ( is_6ghz_op_class(hapd->iconf->op_class) && !hapd->conf->ignore_broadcast_ssid )
		{
			os_memset(resp->da, 0xff, ETH_ALEN);
		}
		else
			os_memcpy(resp->da, req->sa, ETH_ALEN);
	}
	os_memcpy(resp->sa, hapd->own_addr, ETH_ALEN);

	os_memcpy(resp->bssid, hapd->own_addr, ETH_ALEN);
	resp->u.probe_resp.beacon_int =
		host_to_le16(hostapd_get_beacon_int(hapd));

	/* hardware or low-level driver will setup seq_ctrl and timestamp */
	resp->u.probe_resp.capab_info =
		host_to_le16(hostapd_own_capab_info(hapd));

	pos = resp->u.probe_resp.variable;
	*pos++ = WLAN_EID_SSID;

	/* For multibss case, if the transmitted VAP is hidden, it must not reveal its own ssid
	 * while transmiting probe resp frame on behalf of a non-transmited vap
	 */
	if( hapd->iconf->multibss_enable && hapd->conf->ignore_broadcast_ssid && is_req_non_transmit_vap )
	{
		if ( hapd->conf->ignore_broadcast_ssid == IGNORE_BROADCAST_SSID_CLEAR_SSID ) {
			/* clear the data, but keep the correct length of the SSID */
			*pos++ = hapd->conf->ssid.ssid_len;
			os_memset(pos, 0, hapd->conf->ssid.ssid_len);
			pos += hapd->conf->ssid.ssid_len;
		} else {
			*pos++ = 0; /* empty SSID */
		}
	} else {
		*pos++ = hapd->conf->ssid.ssid_len;
		os_memcpy(pos, hapd->conf->ssid.ssid, hapd->conf->ssid.ssid_len);
		pos += hapd->conf->ssid.ssid_len;
	}

	/* Supported rates */
	pos = hostapd_eid_supp_rates(hapd, pos);

	/* DS Params */
	pos = hostapd_eid_ds_params(hapd, pos);

	pos = hostapd_eid_country(hapd, pos, epos - pos);

	/* Power Constraint element */
	pos = hostapd_eid_pwr_constraint(hapd, pos);

	/* CSA IE */
	csa_pos = hostapd_eid_csa(hapd, pos);
	if (csa_pos != pos)
		hapd->cs_c_off_proberesp = csa_pos - (u8 *) resp - 1;
	pos = csa_pos;

	/* ERP Information element */
	pos = hostapd_eid_erp_info(hapd, pos);

	/* Extended supported rates */
	pos = hostapd_eid_ext_supp_rates(hapd, pos);

	pos = hostapd_get_rsne(hapd, pos, epos - pos);
	pos = hostapd_eid_bss_load(hapd, pos, epos - pos);
	pos = hostapd_eid_rm_enabled_capab(hapd, pos, epos - pos);
	if (!pos) {
		wpa_printf(MSG_ERROR, "hostapd_eid_rm_enabled_capab failed");
		os_free(resp);
		return NULL;
	}

	/* Multiple BSSID */
	if (hapd->iconf->multibss_enable && (MULTIBSS_REFERENCE_BSS_IDX == hapd->conf->bss_idx)) {
		if (multiple_bssid_ie_len > (epos - pos)) {
			wpa_printf(MSG_ERROR, "hostapd_eid_multiple_bssid failed for probe response, no space left !!!"
				"required length %d", multiple_bssid_ie_len);
			os_free(resp);
			return NULL;
		}
		pos = hostapd_eid_multiple_bssid(hapd, pos);
		if (!pos) {
			wpa_printf(MSG_ERROR, "hostapd_eid_multiple_bssid failed while probe resp template creation");
			os_free(resp);
			return NULL;
		}
	}
	pos = hostapd_get_mde(hapd, pos, epos - pos);

	/* eCSA IE */
	csa_pos = hostapd_eid_ecsa(hapd, pos);
	if (csa_pos != pos)
		hapd->cs_c_off_ecsa_proberesp = csa_pos - (u8 *) resp - 1;
	pos = csa_pos;

	pos = hostapd_eid_supported_op_classes(hapd, pos);
	pos = hostapd_eid_ht_capabilities(hapd, pos);
	if (!pos) {
		wpa_printf(MSG_ERROR, "hostapd_eid_ht_capabilities");
		os_free(resp);
		return NULL;
	}
	pos = hostapd_eid_ht_operation(hapd, pos);
	pos = hostapd_eid_overlapping_bss_scan_params(hapd, pos);

	pos = hostapd_eid_ext_capab(hapd, pos);

	pos = hostapd_eid_time_adv(hapd, pos);
	pos = hostapd_eid_time_zone(hapd, pos);

	pos = hostapd_eid_interworking(hapd, pos);
	pos = hostapd_eid_adv_proto(hapd, pos);
	pos = hostapd_eid_roaming_consortium(hapd, pos);

#ifdef CONFIG_FST
	if (hapd->iface->fst_ies) {
		os_memcpy(pos, wpabuf_head(hapd->iface->fst_ies),
			  wpabuf_len(hapd->iface->fst_ies));
		pos += wpabuf_len(hapd->iface->fst_ies);
	}
#endif /* CONFIG_FST */

#ifdef CONFIG_IEEE80211AC
	if ((hapd->conf->vendor_vht || hapd->iconf->ieee80211ac) &&
		!hapd->conf->disable_11ac && !is_6ghz_op_class(hapd->iconf->op_class)) { /* IOP with STA transmitting directly VHT IE */
		pos = hostapd_eid_vht_capabilities(hapd, pos, 0);
		if (!pos) {
			wpa_printf(MSG_ERROR, "hostapd_eid_vht_capabilities failed");
			os_free(resp);
			return NULL;
		}
		pos = hostapd_eid_vht_operation(hapd, pos);
	}
	if ((hapd->iconf->ieee80211ac && !hapd->conf->disable_11ac) ||
	    (hapd->iconf->ieee80211ax && !hapd->conf->disable_11ax)) {
		pos = hostapd_eid_txpower_envelope(hapd, pos);
	}
#endif /* CONFIG_IEEE80211AC */

	pos = hostapd_eid_rnr(hapd, pos);
	
	if ((hapd->iconf->ieee80211ac && !hapd->conf->disable_11ac) ||
	    hapd->iconf->ieee80211ax)
		pos = hostapd_eid_wb_chsw_wrapper(hapd, pos);

	pos = hostapd_eid_fils_indic(hapd, pos, 0);
	pos = hostapd_get_rsnxe(hapd, pos, epos - pos);

#ifdef CONFIG_IEEE80211AX
	if (hapd->iconf->ieee80211ax && !hapd->conf->disable_11ax) {
		if (is_he_in_req || !hapd->iconf->ieee80211n_acax_compat) {
		pos = hostapd_eid_he_capab(hapd, pos);
		if (!pos) {
			wpa_printf(MSG_ERROR, "hostapd_eid_vht_capabilities failed");
			os_free(resp);
			return NULL;
		}
		pos = hostapd_eid_he_operation(hapd, pos);
		if (!pos) {
			wpa_printf(MSG_ERROR, "hostapd_eid_he_operation failed");
			os_free(resp);
			return NULL;
		}

		if(hapd->iconf->he_spatial_reuse_ie_present_in_probe_response) {
			pos = hostapd_eid_he_spatial_reuse_parameter_set(hapd, pos);
			if (!pos) {
				wpa_printf(MSG_ERROR, "hostapd_eid_he_spatial_reuse_parameter_set");
				os_free(resp);
				return NULL;
			}
		}

		if(hapd->iconf->he_mu_edca_ie_present)
			pos = hostapd_eid_he_mu_edca_parameter_set(hapd, pos);

		if(get_he_cap(hapd->iconf->he_capab.he_mac_capab_info[HE_MACCAP_CAP4_IDX],
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 HE_MAC_CAP4_NDP_FEEDBACK_REPORT_SUPPORT))
			pos = hostapd_eid_he_ndp_feedback_report_parameters_set(hapd, pos);
		if(get_he_cap(hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP2_IDX],
							HE_OPERATION_CAP2_6GHZ_OPERATION_INFO_PRESENT))
			pos = hostapd_eid_he_6ghz_band_cap(hapd,pos);
	}
	}
#endif /* CONFIG_IEEE80211AX */

#ifdef CONFIG_IEEE80211AC
	if (hapd->conf->vendor_vht && !hapd->conf->disable_11ac) {
		pos = hostapd_eid_vendor_vht(hapd, pos);
		if (!pos) {
			wpa_printf(MSG_ERROR, "hostapd_eid_vendor_vht failed");
			os_free(resp);
			return NULL;
		}
	}
#endif /* CONFIG_IEEE80211AC */

	/* WPA / OSEN */
	pos = hostapd_get_wpa_ie(hapd, pos, epos - pos);
	pos = hostapd_get_osen_ie(hapd, pos, epos - pos);

	/* Wi-Fi Alliance WMM */
	pos = hostapd_eid_wmm(hapd, pos);

#ifdef CONFIG_WPS
	if (hapd->conf->wps_state && hapd->wps_probe_resp_ie) {
		os_memcpy(pos, wpabuf_head(hapd->wps_probe_resp_ie),
			  wpabuf_len(hapd->wps_probe_resp_ie));
		pos += wpabuf_len(hapd->wps_probe_resp_ie);
	}
#endif /* CONFIG_WPS */

#ifdef CONFIG_P2P
	if ((hapd->conf->p2p & P2P_ENABLED) && is_p2p &&
	    hapd->p2p_probe_resp_ie) {
		os_memcpy(pos, wpabuf_head(hapd->p2p_probe_resp_ie),
			  wpabuf_len(hapd->p2p_probe_resp_ie));
		pos += wpabuf_len(hapd->p2p_probe_resp_ie);
	}
#endif /* CONFIG_P2P */
#ifdef CONFIG_P2P_MANAGER
	if ((hapd->conf->p2p & (P2P_MANAGE | P2P_ENABLED | P2P_GROUP_OWNER)) ==
	    P2P_MANAGE)
		pos = hostapd_eid_p2p_manage(hapd, pos);
#endif /* CONFIG_P2P_MANAGER */

#ifdef CONFIG_HS20
	pos = hostapd_eid_hs20_indication(hapd, pos);
#endif /* CONFIG_HS20 */

	pos = hostapd_eid_mbo(hapd, pos, (u8 *) resp + buflen - pos);
	pos = hostapd_eid_owe_trans(hapd, pos, (u8 *) resp + buflen - pos);
	pos = hostapd_eid_dpp_cc(hapd, pos, (u8 *) resp + buflen - pos);

	if (hapd->conf->vendor_elements) {
		os_memcpy(pos, wpabuf_head(hapd->conf->vendor_elements),
			  wpabuf_len(hapd->conf->vendor_elements));
		pos += wpabuf_len(hapd->conf->vendor_elements);
	}

	*resp_len = pos - (u8 *) resp;
	return (u8 *) resp;
}


enum ssid_match_result {
	NO_SSID_MATCH,
	EXACT_SSID_MATCH,
	WILDCARD_SSID_MATCH
};

#define MAX_NUM_COLOC_RADIO sizeof(hapd->iconf->coloc_6g_ap_info)/\
				sizeof(colocated_6g_ap_info_t)

#define MAX_NUM_COLOC_VAP_PER_RADIO \
			sizeof(hapd->iconf->coloc_6g_ap_info[i].bss_info)/\
					sizeof(neigh_ap_info_field_t)

static enum ssid_match_result ssid_list_match(const u8 *ssid,
                                         const u8 *ssid_list,
                                         size_t ssid_list_len)
{
	const u8 *pos, *end;
	int wildcard = 0;
	if (ssid_list) {
		pos = ssid_list;
		end = ssid_list + ssid_list_len;
		while (end - pos >= 2) {
			if (2 + pos[1] > end - pos)
				break;
			if (pos[1] == 0)
				wildcard = 1;
			
			if( !os_memcmp(pos + 2,
			 ssid,
				pos[1]) )  
				return EXACT_SSID_MATCH; 
			pos += 2 + pos[1];
		}
	}
	return wildcard ? WILDCARD_SSID_MATCH : NO_SSID_MATCH;
}

static enum ssid_match_result short_ssid_match(u8 short_ssid[],
					 const u8 *short_ssid_list,
                                         size_t short_ssid_list_len)
{
	const u8 *pos, *end;
	if (short_ssid_list) {
		pos = short_ssid_list;
		end = short_ssid_list + short_ssid_list_len;
		while (end - pos >= 4) {
			int short_ssid_in_list = WPA_GET_LE32(pos);
			if ( !os_memcmp(short_ssid, &short_ssid_in_list, 4) )
				return EXACT_SSID_MATCH;
			pos += 4;
		}
	}
	return NO_SSID_MATCH;
}
	
static enum ssid_match_result ssid_match(struct hostapd_data *hapd,
					 const u8 *ssid, size_t ssid_len,
					 const u8 *ssid_list,
					 size_t ssid_list_len,
					 const u8 *short_ssid_list,
					 size_t short_ssid_list_len)
{
	int wildcard = 0, i = 0, j = 0, res, tmp;
	errno_t err = EOK;
	u8 short_ssid[4];
	u8 empty_bssid[ETH_ALEN] = {'\0'};
	colocated_6g_ap_info_t *c = hapd->iconf->coloc_6g_ap_info;

	if (ssid_len == 0)
		wildcard = 1;
	if (ssid_len == hapd->conf->ssid.ssid_len &&
	    os_memcmp(ssid, hapd->conf->ssid.ssid, ssid_len) == 0)
		return EXACT_SSID_MATCH;

	/* check self ssid in ssid_list */
	if ( NO_SSID_MATCH != (res = ssid_list_match(hapd->conf->ssid.ssid, ssid_list
		, ssid_list_len)) )
		return res;

	if ( hapd->conf->ignore_broadcast_ssid )
		return wildcard ? WILDCARD_SSID_MATCH : NO_SSID_MATCH;

	/* Compare short ssid and co-located 6G ssid/short_ssid only if the network is non-hidden */

	/* calculate short_ssid */
	tmp = crc32(hapd->conf->ssid.ssid,
		hapd->conf->ssid.ssid_len);

	err = memcpy_s(short_ssid, 4, &tmp, 4);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return NO_SSID_MATCH;
	}

	/* check self short_ssid in short_ssid_list */
	if ( NO_SSID_MATCH != (res = short_ssid_match(short_ssid, short_ssid_list,
		 short_ssid_list_len) ) )
		return res; 


	for( i = 0; i < MAX_NUM_COLOC_RADIO; i++) 
	{ 
		for (  j = 0; j < MAX_NUM_COLOC_VAP_PER_RADIO;
					 j++) 
		{
			if( !os_memcmp( c[i].bss_info[j].bssid
				, empty_bssid, ETH_ALEN))
				continue;

			/* check if 6GHz SSID received on 2.4/5GHz */ 
			if( !os_memcmp(ssid,
			 c[i].bss_info[j].Ssid,
				ssid_len) )  
				return EXACT_SSID_MATCH;

			/* check 6GHz ssid in ssid_list */
			if ( NO_SSID_MATCH != (res = ssid_list_match(c[i].bss_info[j].Ssid,
				 ssid_list, ssid_list_len)) )
				return res;

			/* check 6GHz short_ssid in short_ssid_list */
			if ( NO_SSID_MATCH != (res = short_ssid_match(c[i].bss_info[j].ShortSsid,
				 short_ssid_list, short_ssid_list_len)) )
				return res;

		}
	}
	
	return wildcard ? WILDCARD_SSID_MATCH : NO_SSID_MATCH;
}


void sta_track_expire(struct hostapd_iface *iface, int force)
{
	struct os_reltime now;
	struct hostapd_sta_info *info;

	if (!iface->num_sta_seen)
		return;

	os_get_reltime(&now);
	while ((info = dl_list_first(&iface->sta_seen, struct hostapd_sta_info,
				     list))) {
		if (!force &&
		    !os_reltime_expired(&now, &info->last_seen,
					iface->conf->track_sta_max_age))
			break;
		force = 0;

		wpa_printf(MSG_MSGDUMP, "%s: Expire STA tracking entry for "
			   MACSTR, iface->bss[0]->conf->iface,
			   MAC2STR(info->addr));
		dl_list_del(&info->list);
		iface->num_sta_seen--;
		sta_track_del(info);
	}
}


static struct hostapd_sta_info * sta_track_get(struct hostapd_iface *iface,
					       const u8 *addr)
{
	struct hostapd_sta_info *info;

	dl_list_for_each(info, &iface->sta_seen, struct hostapd_sta_info, list)
		if (os_memcmp(addr, info->addr, ETH_ALEN) == 0)
			return info;

	return NULL;
}


void sta_track_add(struct hostapd_iface *iface, const u8 *addr, int ssi_signal)
{
	struct hostapd_sta_info *info;

	info = sta_track_get(iface, addr);
	if (info) {
		/* Move the most recent entry to the end of the list */
		dl_list_del(&info->list);
		DL_LIST_ADD_TAIL(&iface->sta_seen, info, list);
		os_get_reltime(&info->last_seen);
		info->ssi_signal = ssi_signal;
		return;
	}

	/* Add a new entry */
	info = os_zalloc(sizeof(*info));
	if (info == NULL)
		return;
	os_memcpy(info->addr, addr, ETH_ALEN);
	os_get_reltime(&info->last_seen);
	info->ssi_signal = ssi_signal;

	if (iface->num_sta_seen >= iface->conf->track_sta_max_num) {
		/* Expire oldest entry to make room for a new one */
		sta_track_expire(iface, 1);
	}

	wpa_printf(MSG_MSGDUMP, "%s: Add STA tracking entry for "
		   MACSTR, iface->bss[0]->conf->iface, MAC2STR(addr));
	DL_LIST_ADD_TAIL(&iface->sta_seen, info, list);
	iface->num_sta_seen++;
}


struct hostapd_data *
sta_track_seen_on(struct hostapd_iface *iface, const u8 *addr,
		  const char *ifname)
{
	struct hapd_interfaces *interfaces = iface->interfaces;
	size_t i, j;

	for (i = 0; i < interfaces->count; i++) {
		struct hostapd_data *hapd = NULL;

		iface = interfaces->iface[i];
		for (j = 0; j < iface->num_bss; j++) {
			hapd = iface->bss[j];
			if (os_strcmp(ifname, hapd->conf->iface) == 0)
				break;
			hapd = NULL;
		}

		if (hapd && sta_track_get(iface, addr))
			return hapd;
	}

	return NULL;
}


#ifdef CONFIG_TAXONOMY
void sta_track_claim_taxonomy_info(struct hostapd_iface *iface, const u8 *addr,
				   struct wpabuf **probe_ie_taxonomy)
{
	struct hostapd_sta_info *info;

	info = sta_track_get(iface, addr);
	if (!info)
		return;

	wpabuf_free(*probe_ie_taxonomy);
	*probe_ie_taxonomy = info->probe_ie_taxonomy;
	info->probe_ie_taxonomy = NULL;
}
#endif /* CONFIG_TAXONOMY */

static void _notify_probe_req(struct hostapd_data *hapd, const u8 *probe_req,
			      size_t length)
{
	int ret, str_len = length * 2 + 1;
	char *buf = os_malloc(str_len);

	if (buf == NULL) {
		wpa_printf(MSG_ERROR, "Malloc for probe req failed");
		return;
	}

	ret = wpa_snprintf_hex_uppercase(buf, str_len, probe_req, length);

	/* '\0' is not counted in return value of wpa_snprintf_hex_uppercase() */
	if (ret != (str_len - 1)) {
		wpa_printf(MSG_ERROR,
			   "Failed to copy all probe req bytes: copied %d, expected %d",
			   ret, str_len);
		os_free(buf);
		return;
	}

	wpa_msg_ctrl(hapd->msg_ctx, MSG_INFO, AP_PROBE_REQ_RECEIVED "%s", buf);
	os_free(buf);
}

void handle_probe_req(struct hostapd_data *hapd,
		      const struct ieee80211_mgmt *mgmt, size_t len,
		      int ssi_signal, int snr_db)
{
	u8 *resp;
	struct ieee802_11_elems elems;
	const u8 *ie;
	size_t ie_len;
	size_t i, resp_len;
	int noack;
	enum ssid_match_result res;
	int ret, softblock_res, sb_acl;
	u16 csa_offs[2];
	size_t csa_offs_len;
	u32 session_timeout, acct_interim_interval;
	struct vlan_description vlan_id;
	struct hostapd_sta_wpa_psk_short *psk = NULL;
	char *identity = NULL;
	char *radius_cui = NULL;
	struct intel_vendor_event_msg_drop msg_dropped;
	u16 status;

	if (len < IEEE80211_HDRLEN)
		return;

	/* Check for softblock thresholds now */
	softblock_res = hostapd_check_softblock(hapd, mgmt->sa, &status, WLAN_FC_STYPE_PROBE_REQ,
				snr_db, &msg_dropped);

	if (softblock_res == HOSTAPD_SOFTBLOCK_DROP) {
		if (is_multicast_ether_addr(mgmt->da)) {
			msg_dropped.broadcast = true; /* Its a bcast probe req */
		}
		hostapd_event_ltq_softblock_drop(hapd, &msg_dropped);
		return; /* should be harmless. no allocs done to free */
	} else if (softblock_res == HOSTAPD_SOFTBLOCK_ALLOWED) {
		/* Update the broadcast flag and notify the app once the Prb resp is sent */
		if (is_multicast_ether_addr(mgmt->da)) {
			msg_dropped.broadcast = true; /* Its a bcast probe req */
		}
	} else if (softblock_res == HOSTAPD_MULTI_AP_BLACKLIST_FOUND) {
		return; /* Case when the STA is in blacklist not by softblock */
	}

	if (hapd->iconf->owl)
		_notify_probe_req(hapd, (const u8 *)mgmt, len);

	ie = ((const u8 *) mgmt) + IEEE80211_HDRLEN;
	if (hapd->iconf->track_sta_max_num)
		sta_track_add(hapd->iface, mgmt->sa, ssi_signal);
	ie_len = len - IEEE80211_HDRLEN;

	ret = ieee802_11_allowed_address(hapd, mgmt->sa, (const u8 *) mgmt, len,
					 &session_timeout,
					 &acct_interim_interval, &vlan_id,
					 &psk, &identity, &radius_cui, 1);
	if (ret == HOSTAPD_ACL_REJECT) {
		wpa_msg(hapd->msg_ctx, MSG_DEBUG,
			"Ignore Probe Request frame from " MACSTR
			" due to ACL reject ", MAC2STR(mgmt->sa));
		return;
	}

	for (i = 0; hapd->probereq_cb && i < hapd->num_probereq_cb; i++)
		if (hapd->probereq_cb[i].cb(hapd->probereq_cb[i].ctx,
					    mgmt->sa, mgmt->da, mgmt->bssid,
					    ie, ie_len, ssi_signal) > 0)
			return;

	if (!hapd->conf->send_probe_response)
		return;

	if (ieee802_11_parse_elems(ie, ie_len, &elems, 0) == ParseFailed) {
		wpa_printf(MSG_DEBUG, "Could not parse ProbeReq from " MACSTR,
			   MAC2STR(mgmt->sa));
		return;
	}

	if ((!elems.ssid || !elems.supp_rates)) {
		wpa_printf(MSG_DEBUG, "STA " MACSTR " sent probe request "
			   "without SSID or supported rates element",
			   MAC2STR(mgmt->sa));
		return;
	}

	/*
	 * No need to reply if the Probe Request frame was sent on an adjacent
	 * channel. IEEE Std 802.11-2012 describes this as a requirement for an
	 * AP with dot11RadioMeasurementActivated set to true, but strictly
	 * speaking does not allow such ignoring of Probe Request frames if
	 * dot11RadioMeasurementActivated is false. Anyway, this can help reduce
	 * number of unnecessary Probe Response frames for cases where the STA
	 * is less likely to see them (Probe Request frame sent on a
	 * neighboring, but partially overlapping, channel).
	 */
	if (elems.ds_params &&
	    hapd->iface->current_mode &&
	    (hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211G ||
	     hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211B) &&
	    hapd->iconf->channel != elems.ds_params[0]) {
		wpa_printf(MSG_DEBUG,
			   "Ignore Probe Request due to DS Params mismatch: chan=%u != ds.chan=%u",
			   hapd->iconf->channel, elems.ds_params[0]);
		return;
	}

#ifdef CONFIG_P2P
	if (hapd->p2p && hapd->p2p_group && elems.wps_ie) {
		struct wpabuf *wps;
		wps = ieee802_11_vendor_ie_concat(ie, ie_len, WPS_DEV_OUI_WFA);
		if (wps && !p2p_group_match_dev_type(hapd->p2p_group, wps)) {
			wpa_printf(MSG_MSGDUMP, "P2P: Ignore Probe Request "
				   "due to mismatch with Requested Device "
				   "Type");
			wpabuf_free(wps);
			return;
		}
		wpabuf_free(wps);
	}

	if (hapd->p2p && hapd->p2p_group && elems.p2p) {
		struct wpabuf *p2p;
		p2p = ieee802_11_vendor_ie_concat(ie, ie_len, P2P_IE_VENDOR_TYPE);
		if (p2p && !p2p_group_match_dev_id(hapd->p2p_group, p2p)) {
			wpa_printf(MSG_MSGDUMP, "P2P: Ignore Probe Request "
				   "due to mismatch with Device ID");
			wpabuf_free(p2p);
			return;
		}
		wpabuf_free(p2p);
	}
#endif /* CONFIG_P2P */

	if (hapd->conf->ignore_broadcast_ssid && elems.ssid_len == 0 &&
	    elems.ssid_list_len == 0) {
		wpa_printf(MSG_MSGDUMP, "Probe Request from " MACSTR " for "
			   "broadcast SSID ignored", MAC2STR(mgmt->sa));
		return;
	}

#ifdef CONFIG_P2P
	if ((hapd->conf->p2p & P2P_GROUP_OWNER) &&
	    elems.ssid_len == P2P_WILDCARD_SSID_LEN &&
	    os_memcmp(elems.ssid, P2P_WILDCARD_SSID,
		      P2P_WILDCARD_SSID_LEN) == 0) {
		/* Process P2P Wildcard SSID like Wildcard SSID */
		elems.ssid_len = 0;
	}
#endif /* CONFIG_P2P */

#ifdef CONFIG_TAXONOMY
	{
		struct sta_info *sta;
		struct hostapd_sta_info *info;

		if ((sta = ap_get_sta(hapd, mgmt->sa)) != NULL) {
			taxonomy_sta_info_probe_req(hapd, sta, ie, ie_len);
		} else if ((info = sta_track_get(hapd->iface,
						 mgmt->sa)) != NULL) {
			taxonomy_hostapd_sta_info_probe_req(hapd, info,
							    ie, ie_len);
		}
	}
#endif /* CONFIG_TAXONOMY */

	res = ssid_match(hapd, elems.ssid, elems.ssid_len,
 			 elems.ssid_list, elems.ssid_list_len,
			 elems.short_ssid_list, elems.short_ssid_list_len);

	if (res == NO_SSID_MATCH) {
		if (!(mgmt->da[0] & 0x01)) {
			wpa_printf(MSG_MSGDUMP, "Probe Request from " MACSTR
				   " for foreign SSID '%s' (DA " MACSTR ")%s",
				   MAC2STR(mgmt->sa),
				   wpa_ssid_txt(elems.ssid, elems.ssid_len),
				   MAC2STR(mgmt->da),
				   elems.ssid_list ? " (SSID list)" : "");
		}
		return;
	}

	/* Soft Block ACL */
	if (res == EXACT_SSID_MATCH) {
		sb_acl = hostapd_soft_block_acl_client_status(hapd, mgmt->sa, WLAN_FC_STYPE_PROBE_REQ, NULL);
		if (SOFT_BLOCK_ACL_DROP == sb_acl) {
			wpa_printf(MSG_DEBUG, "SofBLock ACL: STA " MACSTR " PROBE dropped",
			MAC2STR(mgmt->sa));
			return;
		} else {
			wpa_printf(MSG_DEBUG, "SofBLock ACL: STA " MACSTR " PROBE accepted",
			MAC2STR(mgmt->sa));
		}
	}

	/* Probe request with wildcard ssid on 6ghz and both da and bssid broadcast is not expected     *
         *                                   IEEE802.11 Draft 6.0                                       *
         * The non-AP STA shall not transmit a Probe Request frame to the broadcast destination address *
         * with the Address 3 field set to the wildcard BSSID, and the SSID set to the wildcard SSID    */
	if ((res == WILDCARD_SSID_MATCH) && is_6ghz_op_class(hapd->iconf->op_class)
            && is_broadcast_ether_addr(mgmt->da) && is_broadcast_ether_addr(mgmt->bssid))  
	{
		wpa_printf(MSG_MSGDUMP, "wildcard Probe Request dropped on 6GHz due to "
	 				"broadcast destination and wildcard bssid");
		return;
	}

#ifdef CONFIG_INTERWORKING
	if (hapd->conf->interworking &&
	    elems.interworking && elems.interworking_len >= 1) {
		u8 ant = elems.interworking[0] & 0x0f;
		if (ant != INTERWORKING_ANT_WILDCARD &&
		    ant != hapd->conf->access_network_type) {
			wpa_printf(MSG_MSGDUMP, "Probe Request from " MACSTR
				   " for mismatching ANT %u ignored",
				   MAC2STR(mgmt->sa), ant);
			return;
		}
	}

	if (hapd->conf->interworking && elems.interworking &&
	    (elems.interworking_len == 7 || elems.interworking_len == 9)) {
		const u8 *hessid;
		if (elems.interworking_len == 7)
			hessid = elems.interworking + 1;
		else
			hessid = elems.interworking + 1 + 2;
		if (!is_broadcast_ether_addr(hessid) &&
		    os_memcmp(hessid, hapd->conf->hessid, ETH_ALEN) != 0) {
			wpa_printf(MSG_MSGDUMP, "Probe Request from " MACSTR
				   " for mismatching HESSID " MACSTR
				   " ignored",
				   MAC2STR(mgmt->sa), MAC2STR(hessid));
			return;
		}
	}
#endif /* CONFIG_INTERWORKING */

#ifdef CONFIG_P2P
	if ((hapd->conf->p2p & P2P_GROUP_OWNER) &&
	    supp_rates_11b_only(&elems)) {
		/* Indicates support for 11b rates only */
		wpa_printf(MSG_EXCESSIVE, "P2P: Ignore Probe Request from "
			   MACSTR " with only 802.11b rates",
			   MAC2STR(mgmt->sa));
		return;
	}
#endif /* CONFIG_P2P */

	/* TODO: verify that supp_rates contains at least one matching rate
	 * with AP configuration */

	if (hapd->conf->no_probe_resp_if_seen_on &&
	    is_multicast_ether_addr(mgmt->da) &&
	    is_multicast_ether_addr(mgmt->bssid) &&
	    sta_track_seen_on(hapd->iface, mgmt->sa,
			      hapd->conf->no_probe_resp_if_seen_on)) {
		wpa_printf(MSG_MSGDUMP, "%s: Ignore Probe Request from " MACSTR
			   " since STA has been seen on %s",
			   hapd->conf->iface, MAC2STR(mgmt->sa),
			   hapd->conf->no_probe_resp_if_seen_on);
		return;
	}

	if (hapd->conf->no_probe_resp_if_max_sta &&
	    is_multicast_ether_addr(mgmt->da) &&
	    is_multicast_ether_addr(mgmt->bssid) &&
	    hapd->num_sta >= hapd->conf->max_num_sta &&
	    !ap_get_sta(hapd, mgmt->sa)) {
		wpa_printf(MSG_MSGDUMP, "%s: Ignore Probe Request from " MACSTR
			   " since no room for additional STA",
			   hapd->conf->iface, MAC2STR(mgmt->sa));
		return;
	}

#ifdef CONFIG_TESTING_OPTIONS
	if (hapd->iconf->ignore_probe_probability > 0.0 &&
	    drand48() < hapd->iconf->ignore_probe_probability) {
		wpa_printf(MSG_INFO,
			   "TESTING: ignoring probe request from " MACSTR,
			   MAC2STR(mgmt->sa));
		return;
	}
#endif /* CONFIG_TESTING_OPTIONS */

	wpa_msg_ctrl(hapd->msg_ctx, MSG_INFO, RX_PROBE_REQUEST "sa=" MACSTR
		     " signal=%d", MAC2STR(mgmt->sa), ssi_signal);
	resp = hostapd_gen_probe_resp(hapd, mgmt, elems.p2p != NULL,
                                              elems.pc_he_capabilities != NULL,
                                              &resp_len);

	if (resp == NULL)
		return;

	/*
	 * If this is a broadcast probe request, apply no ack policy to avoid
	 * excessive retries.
	 */
	noack = !!(res == WILDCARD_SSID_MATCH &&
		   is_broadcast_ether_addr(mgmt->da));

	csa_offs_len = 0;
	if (hapd->csa_in_progress) {
		if (hapd->cs_c_off_proberesp)
			csa_offs[csa_offs_len++] =
				hapd->cs_c_off_proberesp;

		if (hapd->cs_c_off_ecsa_proberesp)
			csa_offs[csa_offs_len++] =
				hapd->cs_c_off_ecsa_proberesp;
	}

	if(hapd->iconf->multibss_enable) {
		ret = hostapd_drv_send_mlme(hapd->iface->bss[MULTIBSS_REFERENCE_BSS_IDX], resp, resp_len, noack,
						csa_offs_len ? csa_offs : NULL,
						csa_offs_len,0);
	} else {
		ret = hostapd_drv_send_mlme(hapd, resp, resp_len, noack,
						csa_offs_len ? csa_offs : NULL,
						csa_offs_len,0);
	}

	if (ret < 0) {
		wpa_printf(MSG_INFO, "handle_probe_req: send failed");
	} else if (softblock_res == HOSTAPD_SOFTBLOCK_ALLOWED) {
		hostapd_event_ltq_softblock_drop(hapd, &msg_dropped);
	}

	os_free(resp);

	wpa_printf(MSG_EXCESSIVE, "STA " MACSTR " sent probe request for %s "
		   "SSID", MAC2STR(mgmt->sa),
		   elems.ssid_len == 0 ? "broadcast" : "our");
}


static u8 * hostapd_probe_resp_offloads(struct hostapd_data *hapd,
					size_t *resp_len)
{
	/* check probe response offloading caps and print warnings */
	if (!(hapd->iface->drv_flags & WPA_DRIVER_FLAGS_PROBE_RESP_OFFLOAD))
		return NULL;

#ifdef CONFIG_WPS
	if (hapd->conf->wps_state && hapd->wps_probe_resp_ie &&
	    (!(hapd->iface->probe_resp_offloads &
	       (WPA_DRIVER_PROBE_RESP_OFFLOAD_WPS |
		WPA_DRIVER_PROBE_RESP_OFFLOAD_WPS2))))
		wpa_printf(MSG_DEBUG, "Device is trying to offload WPS "
			   "Probe Response while not supporting this");
#endif /* CONFIG_WPS */

#ifdef CONFIG_P2P
	if ((hapd->conf->p2p & P2P_ENABLED) && hapd->p2p_probe_resp_ie &&
	    !(hapd->iface->probe_resp_offloads &
	      WPA_DRIVER_PROBE_RESP_OFFLOAD_P2P))
		wpa_printf(MSG_DEBUG, "Device is trying to offload P2P "
			   "Probe Response while not supporting this");
#endif  /* CONFIG_P2P */

	if (hapd->conf->interworking &&
	    !(hapd->iface->probe_resp_offloads &
	      WPA_DRIVER_PROBE_RESP_OFFLOAD_INTERWORKING))
		wpa_printf(MSG_DEBUG, "Device is trying to offload "
			   "Interworking Probe Response while not supporting "
			   "this");

	/* Generate a Probe Response template for the non-P2P case */
	return hostapd_gen_probe_resp(hapd, NULL, 0, 1, resp_len);
}

#endif /* NEED_AP_MLME */


void sta_track_del(struct hostapd_sta_info *info)
{
#ifdef CONFIG_TAXONOMY
	wpabuf_free(info->probe_ie_taxonomy);
	info->probe_ie_taxonomy = NULL;
#endif /* CONFIG_TAXONOMY */
	os_free(info);
}


int ieee802_11_build_ap_params(struct hostapd_data *hapd,
			       struct wpa_driver_ap_params *params)
{
	struct ieee80211_mgmt *head = NULL;
	u8 *tail = NULL;
	size_t head_len = 0, tail_len = 0;
	u8 *resp = NULL;
	size_t resp_len = 0;
	int multiple_bssid_ie_len = -1;
#ifdef NEED_AP_MLME
	u16 capab_info;
	u8 *pos, *tailpos, *tailend, *csa_pos;

#define BEACON_HEAD_BUF_SIZE 256
#define BEACON_TAIL_BUF_SIZE 512
	head = os_zalloc(BEACON_HEAD_BUF_SIZE);
	tail_len = BEACON_TAIL_BUF_SIZE;
#ifdef CONFIG_WPS
	if (hapd->conf->wps_state && hapd->wps_beacon_ie)
		tail_len += wpabuf_len(hapd->wps_beacon_ie);
#endif /* CONFIG_WPS */
#ifdef CONFIG_P2P
	if (hapd->p2p_beacon_ie)
		tail_len += wpabuf_len(hapd->p2p_beacon_ie);
#endif /* CONFIG_P2P */
#ifdef CONFIG_FST
	if (hapd->iface->fst_ies)
		tail_len += wpabuf_len(hapd->iface->fst_ies);
#endif /* CONFIG_FST */
	if (hapd->conf->vendor_elements)
		tail_len += wpabuf_len(hapd->conf->vendor_elements);

#ifdef CONFIG_IEEE80211AC
	if (hapd->conf->vendor_vht && !hapd->conf->disable_11ac) {
		tail_len += 5 + 2 + sizeof(struct ieee80211_vht_capabilities) +
			2 + sizeof(struct ieee80211_vht_operation);
	}
#endif /* CONFIG_IEEE80211AC */

#ifdef CONFIG_IEEE80211AX
	if (hapd->iconf->ieee80211ax) {
        tail_len += 3 + sizeof(struct ieee80211_he_capabilities) +
            3 + sizeof(struct ieee80211_he_operation) +
            3 + sizeof(struct ieee80211_he_mu_edca_parameter_set);
	}
#endif /* CONFIG_IEEE80211AX */

	tail_len += hostapd_mbo_ie_len(hapd);
	tail_len += hostapd_eid_owe_trans_len(hapd);
	tail_len += hostapd_eid_dpp_cc_len(hapd);
	if (hapd->iconf->multibss_enable && (MULTIBSS_REFERENCE_BSS_IDX == hapd->conf->bss_idx)) {
		multiple_bssid_ie_len = multiple_bssid_ie_get_len(hapd);
		if (multiple_bssid_ie_len < 0)
                goto fail;
		tail_len += multiple_bssid_ie_len;
	}
	tailpos = tail = os_malloc(tail_len);
	if (head == NULL || tail == NULL) {
		wpa_printf(MSG_ERROR, "Failed to set beacon data");
		goto fail;
	}
	tailend = tail + tail_len;

	head->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
					   WLAN_FC_STYPE_BEACON);
	head->duration = host_to_le16(0);
	os_memset(head->da, 0xff, ETH_ALEN);

	os_memcpy(head->sa, hapd->own_addr, ETH_ALEN);
	os_memcpy(head->bssid, hapd->own_addr, ETH_ALEN);
	head->u.beacon.beacon_int =
		host_to_le16(hostapd_get_beacon_int(hapd));

	/* hardware or low-level driver will setup seq_ctrl and timestamp */
	capab_info = hostapd_own_capab_info(hapd);
	head->u.beacon.capab_info = host_to_le16(capab_info);
	pos = &head->u.beacon.variable[0];

	/* SSID */
	*pos++ = WLAN_EID_SSID;
	if (hapd->conf->ignore_broadcast_ssid == 2) {
		/* clear the data, but keep the correct length of the SSID */
		*pos++ = hapd->conf->ssid.ssid_len;
		os_memset(pos, 0, hapd->conf->ssid.ssid_len);
		pos += hapd->conf->ssid.ssid_len;
	} else if (hapd->conf->ignore_broadcast_ssid) {
		*pos++ = 0; /* empty SSID */
	} else {
		*pos++ = hapd->conf->ssid.ssid_len;
		os_memcpy(pos, hapd->conf->ssid.ssid,
			  hapd->conf->ssid.ssid_len);
		pos += hapd->conf->ssid.ssid_len;
	}

	/* Supported rates */
	pos = hostapd_eid_supp_rates(hapd, pos);

	/* DS Params */
	pos = hostapd_eid_ds_params(hapd, pos);

	head_len = pos - (u8 *) head;

	tailpos = hostapd_eid_country(hapd, tailpos, tailend - tailpos);

	/* Power Constraint element */
	tailpos = hostapd_eid_pwr_constraint(hapd, tailpos);

	/* CSA IE */
	csa_pos = hostapd_eid_csa(hapd, tailpos);
	if (csa_pos != tailpos)
		hapd->cs_c_off_beacon = csa_pos - tail - 1;
	tailpos = csa_pos;

	/* ERP Information element */
	tailpos = hostapd_eid_erp_info(hapd, tailpos);

	/* Extended supported rates */
	tailpos = hostapd_eid_ext_supp_rates(hapd, tailpos);

	tailpos = hostapd_get_rsne(hapd, tailpos, tailend - tailpos);
	tailpos = hostapd_eid_rm_enabled_capab(hapd, tailpos,
					       tail + BEACON_TAIL_BUF_SIZE -
					       tailpos);
	if (!tailpos) {
		wpa_printf(MSG_ERROR, "hostapd_eid_rm_enabled_capab failed");
		goto fail;
	}

	/* Multiple BSSID */
	if (hapd->iconf->multibss_enable && MULTIBSS_REFERENCE_BSS_IDX == hapd->conf->bss_idx) {
		if (multiple_bssid_ie_len > (tailend - tailpos)) {
			wpa_printf(MSG_ERROR, "hostapd_eid_multiple_bssid failed for beacon, no space left !!!"
				"required length %d", multiple_bssid_ie_len);
			goto fail;
		}
		tailpos = hostapd_eid_multiple_bssid(hapd, tailpos);
		if (!tailpos) {
			wpa_printf(MSG_ERROR, "hostapd_eid_multiple_bssid failed for beacon template creation");
			goto fail;
		}
	}

	tailpos = hostapd_get_mde(hapd, tailpos, tailend - tailpos);

	/* eCSA IE */
	csa_pos = hostapd_eid_ecsa(hapd, tailpos);
	if (csa_pos != tailpos)
		hapd->cs_c_off_ecsa_beacon = csa_pos - tail - 1;
	tailpos = csa_pos;

	tailpos = hostapd_eid_supported_op_classes(hapd, tailpos);
	tailpos = hostapd_eid_ht_capabilities(hapd, tailpos);
	if (!tailpos) {
		wpa_printf(MSG_ERROR, "hostapd_eid_ht_capabilities");
		goto fail;
	}
	tailpos = hostapd_eid_ht_operation(hapd, tailpos);
	tailpos = hostapd_eid_overlapping_bss_scan_params(hapd, tailpos);

	tailpos = hostapd_eid_ext_capab(hapd, tailpos);

	/*
	 * TODO: Time Advertisement element should only be included in some
	 * DTIM Beacon frames.
	 */
	tailpos = hostapd_eid_time_adv(hapd, tailpos);

	tailpos = hostapd_eid_interworking(hapd, tailpos);
	tailpos = hostapd_eid_adv_proto(hapd, tailpos);
	tailpos = hostapd_eid_roaming_consortium(hapd, tailpos);

#ifdef CONFIG_FST
	if (hapd->iface->fst_ies) {
		os_memcpy(tailpos, wpabuf_head(hapd->iface->fst_ies),
			  wpabuf_len(hapd->iface->fst_ies));
		tailpos += wpabuf_len(hapd->iface->fst_ies);
	}
#endif /* CONFIG_FST */

#ifdef CONFIG_IEEE80211AC
	if ((hapd->conf->vendor_vht || hapd->iconf->ieee80211ac) &&
		!hapd->conf->disable_11ac) { /* IOP with STAs transmitting direct VHT-IEs */
		tailpos = hostapd_eid_vht_capabilities(hapd, tailpos, 0);
		if (!tailpos) {
			wpa_printf(MSG_ERROR, "hostapd_eid_vht_capabilities failed");
			goto fail;
		}
		tailpos = hostapd_eid_vht_operation(hapd, tailpos);
	}
	if ((hapd->iconf->ieee80211ac && !hapd->conf->disable_11ac) ||
	    (hapd->iconf->ieee80211ax && !hapd->conf->disable_11ax)) {
		tailpos = hostapd_eid_txpower_envelope(hapd, tailpos);
	}
#endif /* CONFIG_IEEE80211AC */

	tailpos = hostapd_eid_opmodenotif(hapd, tailpos);

	tailpos = hostapd_eid_rnr(hapd, tailpos);
	if ((hapd->iconf->ieee80211ac && !hapd->conf->disable_11ac) ||
	     hapd->iconf->ieee80211ax)
		tailpos = hostapd_eid_wb_chsw_wrapper(hapd, tailpos);

	tailpos = hostapd_eid_fils_indic(hapd, tailpos, 0);
	tailpos = hostapd_get_rsnxe(hapd, tailpos, tailend - tailpos);

#ifdef CONFIG_IEEE80211AX
    if (hapd->iconf->ieee80211ax && !hapd->conf->disable_11ax) {
			tailpos = hostapd_eid_he_capab(hapd, tailpos);
			if (!tailpos) {
				wpa_printf(MSG_ERROR, "hostapd_eid_he_capab failed");
				goto fail;
			}
			tailpos = hostapd_eid_he_operation(hapd, tailpos);
			if (!tailpos) {
				wpa_printf(MSG_ERROR, "hostapd_eid_he_operation failed");
				goto fail;
			}

			if(hapd->iconf->he_mu_edca_ie_present)
				tailpos = hostapd_eid_he_mu_edca_parameter_set(hapd, tailpos);

			if(hapd->iconf->he_spatial_reuse_ie_present_in_beacon) {
				tailpos = hostapd_eid_he_spatial_reuse_parameter_set(hapd, tailpos);
				if (!tailpos) {
					wpa_printf(MSG_ERROR, "hostapd_eid_he_spatial_reuse_parameter_set failed");
					goto fail;
				}
			}
  		if(get_he_cap(hapd->iconf->he_capab.he_mac_capab_info[HE_MACCAP_CAP4_IDX],
   	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 HE_MAC_CAP4_NDP_FEEDBACK_REPORT_SUPPORT))
				tailpos = hostapd_eid_he_ndp_feedback_report_parameters_set(hapd, tailpos);
			if(get_he_cap(hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP2_IDX],
                                                        HE_OPERATION_CAP2_6GHZ_OPERATION_INFO_PRESENT))
				tailpos = hostapd_eid_he_6ghz_band_cap(hapd,tailpos);
    }
#endif /* CONFIG_IEEE80211AX */

#ifdef CONFIG_IEEE80211AC
	if (hapd->conf->vendor_vht && !hapd->conf->disable_11ac) {
		tailpos = hostapd_eid_vendor_vht(hapd, tailpos);
		if (!tailpos) {
			wpa_printf(MSG_ERROR, "hostapd_eid_vendor_vht");
			goto fail;
		}
	}
#endif /* CONFIG_IEEE80211AC */

	/* WPA / OSEN */
	tailpos = hostapd_get_wpa_ie(hapd, tailpos, tailend - tailpos);
	tailpos = hostapd_get_osen_ie(hapd, tailpos, tailend - tailpos);

	/* Wi-Fi Alliance WMM */
	tailpos = hostapd_eid_wmm(hapd, tailpos);

#ifdef CONFIG_WPS
	if (hapd->conf->wps_state && hapd->wps_beacon_ie) {
		os_memcpy(tailpos, wpabuf_head(hapd->wps_beacon_ie),
			  wpabuf_len(hapd->wps_beacon_ie));
		tailpos += wpabuf_len(hapd->wps_beacon_ie);
	}
#endif /* CONFIG_WPS */

#ifdef CONFIG_P2P
	if ((hapd->conf->p2p & P2P_ENABLED) && hapd->p2p_beacon_ie) {
		os_memcpy(tailpos, wpabuf_head(hapd->p2p_beacon_ie),
			  wpabuf_len(hapd->p2p_beacon_ie));
		tailpos += wpabuf_len(hapd->p2p_beacon_ie);
	}
#endif /* CONFIG_P2P */
#ifdef CONFIG_P2P_MANAGER
	if ((hapd->conf->p2p & (P2P_MANAGE | P2P_ENABLED | P2P_GROUP_OWNER)) ==
	    P2P_MANAGE)
		tailpos = hostapd_eid_p2p_manage(hapd, tailpos);
#endif /* CONFIG_P2P_MANAGER */

#ifdef CONFIG_HS20
	tailpos = hostapd_eid_hs20_indication(hapd, tailpos);
#endif /* CONFIG_HS20 */

	tailpos = hostapd_eid_mbo(hapd, tailpos, tail + tail_len - tailpos);
	tailpos = hostapd_eid_owe_trans(hapd, tailpos,
					tail + tail_len - tailpos);
	tailpos = hostapd_eid_dpp_cc(hapd, tailpos, tail + tail_len - tailpos);

	if (hapd->conf->vendor_elements) {
		os_memcpy(tailpos, wpabuf_head(hapd->conf->vendor_elements),
			  wpabuf_len(hapd->conf->vendor_elements));
		tailpos += wpabuf_len(hapd->conf->vendor_elements);
	}

	tail_len = tailpos > tail ? tailpos - tail : 0;

	resp = hostapd_probe_resp_offloads(hapd, &resp_len);
	if (!resp) {
		wpa_printf(MSG_ERROR, "hostapd_probe_resp_offloads failed");
		goto fail;
	}

#endif /* NEED_AP_MLME */

	os_memset(params, 0, sizeof(*params));
	params->head = (u8 *) head;
	params->head_len = head_len;
	params->tail = tail;
	params->tail_len = tail_len;
	params->proberesp = resp;
	params->proberesp_len = resp_len;
	params->dtim_period = hapd->conf->dtim_period;
	params->beacon_int = hostapd_get_beacon_int(hapd);
	params->basic_rates = hapd->iface->basic_rates;
	params->beacon_rate = hapd->iconf->beacon_rate;
	params->rate_type = hapd->iconf->rate_type;
	params->ssid = hapd->conf->ssid.ssid;
	params->ssid_len = hapd->conf->ssid.ssid_len;
	if ((hapd->conf->wpa & (WPA_PROTO_WPA | WPA_PROTO_RSN)) ==
	    (WPA_PROTO_WPA | WPA_PROTO_RSN))
		params->pairwise_ciphers = hapd->conf->wpa_pairwise |
			hapd->conf->rsn_pairwise;
	else if (hapd->conf->wpa & WPA_PROTO_RSN)
		params->pairwise_ciphers = hapd->conf->rsn_pairwise;
	else if (hapd->conf->wpa & WPA_PROTO_WPA)
		params->pairwise_ciphers = hapd->conf->wpa_pairwise;
	params->group_cipher = hapd->conf->wpa_group;
	params->key_mgmt_suites = hapd->conf->wpa_key_mgmt;
	params->auth_algs = hapd->conf->auth_algs;
	params->wpa_version = hapd->conf->wpa;
	params->privacy = hapd->conf->wpa;
#ifdef CONFIG_WEP
	params->privacy |= hapd->conf->ssid.wep.keys_set ||
		(hapd->conf->ieee802_1x &&
		 (hapd->conf->default_wep_key_len ||
		  hapd->conf->individual_wep_key_len));
#endif /* CONFIG_WEP */
	switch (hapd->conf->ignore_broadcast_ssid) {
	case 0:
		params->hide_ssid = NO_SSID_HIDING;
		break;
	case 1:
		params->hide_ssid = HIDDEN_SSID_ZERO_LEN;
		break;
	case 2:
		params->hide_ssid = HIDDEN_SSID_ZERO_CONTENTS;
		break;
	}
	params->isolate = hapd->conf->isolate;
#ifdef NEED_AP_MLME
	params->cts_protect = !!(ieee802_11_erp_info(hapd) &
				ERP_INFO_USE_PROTECTION);
	params->preamble = hapd->iface->num_sta_no_short_preamble == 0 &&
		hapd->iconf->preamble == SHORT_PREAMBLE;
	if (hapd->iface->current_mode) {
		if (hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211B) {
			params->short_slot_time = 0;
		} else if (hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211G) {
			params->short_slot_time =
				hapd->iface->num_sta_no_short_slot_time > 0 ? 0 : 1;
		} else {
			params->short_slot_time = 1;
		}
	} else {
		params->short_slot_time = -1;
	}
	if (!hapd->iconf->ieee80211n || hapd->conf->disable_11n)
		params->ht_opmode = -1;
	else
		params->ht_opmode = hapd->iface->ht_op_mode;
#endif /* NEED_AP_MLME */
	params->interworking = hapd->conf->interworking;
	if (hapd->conf->interworking &&
	    !is_zero_ether_addr(hapd->conf->hessid))
		params->hessid = hapd->conf->hessid;
	params->access_network_type = hapd->conf->access_network_type;
	params->ap_max_inactivity = hapd->conf->ap_max_inactivity;
#ifdef CONFIG_P2P
	params->p2p_go_ctwindow = hapd->iconf->p2p_go_ctwindow;
#endif /* CONFIG_P2P */
#ifdef CONFIG_HS20
	params->disable_dgaf = hapd->conf->disable_dgaf;
	if (hapd->conf->osen) {
		params->privacy = 1;
		params->osen = 1;
	}
#endif /* CONFIG_HS20 */
	params->multicast_to_unicast = hapd->conf->multicast_to_unicast;
	params->pbss = hapd->conf->pbss;
	if (hapd->conf->ftm_responder) {
		if (hapd->iface->drv_flags & WPA_DRIVER_FLAGS_FTM_RESPONDER) {
			params->ftm_responder = 1;
			params->lci = hapd->iface->conf->lci;
			params->civic = hapd->iface->conf->civic;
		} else {
			wpa_printf(MSG_WARNING,
				   "Not configuring FTM responder as the driver doesn't advertise support for it");
		}
	}
	return 0;
fail:
	os_free(head);
	os_free(tail);
	return -1;
}


void ieee802_11_free_ap_params(struct wpa_driver_ap_params *params)
{
	os_free(params->tail);
	params->tail = NULL;
	os_free(params->head);
	params->head = NULL;
	os_free(params->proberesp);
	params->proberesp = NULL;
}


int update_rnr_in_beacon(struct hostapd_data *hapd, struct update_rnr rnr, int radio_index, int vap_index)
{
	int ret = 0;
	errno_t err = EOK;
	wpa_printf(MSG_DEBUG, "CTRL: entry update_rnr_in_beacon %d radio %d vap",
				radio_index, vap_index);

	hapd->iconf->coloc_6g_ap_info[radio_index].ChanNum = rnr.Channel;
	hapd->iconf->coloc_6g_ap_info[radio_index].OperatingClass = rnr.OperatingClass;
	hapd->iconf->coloc_6g_ap_info[radio_index].Reserved = 0;
	hapd->iconf->coloc_6g_ap_info[radio_index].TbttInfoFieldType = 0; /* TODO (6GHz) Dynamic , possible 0,1,2,3 ( 1,2,3 are reserverd ) */
	hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].NeighApTbttOffset = TBTT_OFFSET_UNKNOWN; /* TODO (6GHz) Dynamic, 255=unknown */


	if( os_memcmp(hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].bssid , rnr.bssid, ETH_ALEN) )
	{
		hapd->iconf->coloc_6g_ap_info[radio_index].TbttInfoCount++;
		wpa_printf(MSG_DEBUG,"CTRL: update_rnr_in_beacon: Increase the tbtt count %d",
			hapd->iconf->coloc_6g_ap_info[radio_index].TbttInfoCount);
	}
	else
		wpa_printf(MSG_DEBUG,"CTRL: update_rnr_in_beacon: Not increase the tbtt count %d",
			hapd->iconf->coloc_6g_ap_info[radio_index].TbttInfoCount);
	/*
	OCT Recommended 0
	Same SSID  x
	Multiple BSSID x
	Transmitted BSSID x
	Member of ESS with 2.4/5GHz Co-located AP(#20024) 0
	20 TU Probe Response Active x
	Co-Located AP 1
	Reserved 0
	0 1 x 0 x x x 0
	*/

	err = memcpy_s(hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].bssid, ETH_ALEN, rnr.bssid, ETH_ALEN);
	if (EOK != err) {
		wpa_printf(MSG_ERROR,"CTRL: %s memcpy_s error %d", __func__, err);
		return -1;
	}
	hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].BssParams = 0; /* clear */
	hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].BssParams
		 |= RNR_TBTT_INFO_BSS_PARAM_COLOC_AP; /*TODO (6GHz) Dynamic */

	if( rnr.unsolicited_frame_support )
	{
		wpa_printf(MSG_DEBUG,"CTRL: update_rnr_in_beacon: 20TU probe response enabled");
		hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].BssParams
			|= RNR_TBTT_INFO_BSS_PARAM_20TU_PROBE_RES;
	}

	if( rnr.multibss_enable )
	{
		hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].BssParams
			|=  RNR_TBTT_INFO_BSS_PARAM_MULTIPLE_BSSID;
		if (rnr.is_transmitted_bssid) {
			hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].BssParams
				|=  RNR_TBTT_INFO_BSS_PARAM_TRANSMITTED_BSSID;
		}
	}

	if( !os_memcmp( hapd->conf->ssid.ssid, rnr.ssid, hapd->conf->ssid.ssid_len ) && !rnr.hidden_mode )
	{
		wpa_printf(MSG_DEBUG, "CTRL: set_beacon_with_snr: Same ssid field set in rnr");
		hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].BssParams |= RNR_TBTT_INFO_BSS_PARAM_SAME_SSID;
	}

	err = memcpy_s(hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].Ssid, SSID_MAX_LEN, rnr.ssid, strnlen_s((const char *)rnr.ssid, SSID_MAX_LEN));
	if (EOK != err) {
		wpa_printf(MSG_ERROR,"CTRL: %s memcpy_s error %d", __func__, err);
		return -1;
	}
	/*
	 * Short SSID calculation is identical to FCS and it is defined in
	 * IEEE802.11-REVmd/D2.2 9.4.2.170.3
	 */
	WPA_PUT_LE32(hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].ShortSsid,
			 crc32(rnr.ssid, strnlen_s((const char *)rnr.ssid, SSID_MAX_LEN)) );

	hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index].psd_20MHz = rnr.psd_20MHz;
	if( ieee802_11_update_beacon(hapd) )
	{
		wpa_printf(MSG_ERROR,"CTRL: SET_BEACON failed");
		ret = -1;
	}

	wpa_printf(MSG_DEBUG, "CTRL: exit update_rnr_in_beacon");

	return ret;
}

int invalidate_rnr_in_beacon(struct hostapd_iface *iface)
{
	int radio_idx;
	struct hostapd_iface *radio;
	struct hapd_interfaces *interfaces = iface->interfaces;

	if(!interfaces) {
		wpa_printf(MSG_ERROR, "%s:%d interface not available", __func__, __LINE__);
		return -1;
	}

	for(radio_idx = 0; radio_idx < interfaces->count; radio_idx++) {
		radio = interfaces->iface[radio_idx];
		wpa_printf(MSG_DEBUG, "%s:%d cleaning for radio:%d", __func__, __LINE__, radio_idx);
		if(!os_memset(radio->conf->coloc_6g_ap_info, 0,
			  sizeof(radio->conf->coloc_6g_ap_info)))
			return -1;
	}
	return 0;
}

static int get_psd_val(struct hostapd_iface *iface, u8 *val)
{
	int psd = 0;
	struct hostapd_channel_data *ch = NULL;
	
	ch = hostapd_hw_get_channel_data_from_ch(iface, iface->conf->channel);
	if ( !ch )
		return -1;

	psd = ( ch->max_tx_power - HOSTAP_6GHZ_10LOG_20MHZ ) * HOSTAP_6GHZ_CONVERT_HALF_DB_UNIT;
	if( ( psd < HOSTAP_6GHZ_PSD_20MHZ_MIN ) ||
	 	( psd > HOSTAP_6GHZ_PSD_20MHZ_MAX ) ) {
		wpa_printf(MSG_ERROR, "Bad %d psd 20MHz", psd);
		return -1;
	}
	if ( psd < 0 )
		psd = psd + HOSTAPD_BYTE_2S_COMPLEMENT; // 2's complement

	*val = (u8) psd;
	return 0;
}

int auto_update_rnr_in_beacon(struct hostapd_iface *iface)
{
	size_t i, radio_idx, vap_idx;
	size_t six_ghz_iface_idx = 0;
	size_t six_ghz_vap_idx;
	int ret = 0;
	u8 psd = 0;
	struct hapd_interfaces *interfaces;

	if(!iface) {
		wpa_printf(MSG_ERROR, "%s:%d iface not available", __func__, __LINE__);
		return -1;
	}

	interfaces = iface->interfaces;
	if(!interfaces) {
		wpa_printf(MSG_ERROR, "%s:%d interface not available", __func__, __LINE__);
		return -1;
	}

	struct update_rnr rnr[MAX_SUPPORTED_6GHZ_RADIO][MAX_SUPPORTED_6GHZ_VAP_PER_RADIO] = {0};

	if(interfaces->count < MIN_COLOCATED_RADIO_COUNT) {
		wpa_printf(MSG_DEBUG, "%s: number of radios %zd is less than minimum %d",
			__func__, interfaces->count, MIN_COLOCATED_RADIO_COUNT);
		return 0;
	}

	/* Prepare an RNR element of 6GHz capable interfaces */
	for (i = 0; i < interfaces->count &&
	(six_ghz_iface_idx < MAX_SUPPORTED_6GHZ_RADIO); i++) {
		struct hostapd_iface *iface = interfaces->iface[i];

		if(!iface->bss[MASTER_VAP_BSS_IDX]->drv_priv ||
			!iface->bss[MASTER_VAP_BSS_IDX]->started) {
			wpa_printf(MSG_DEBUG, "%s:%d - radio:%zd not enabled",
					__func__, __LINE__, i);
			continue;
		}
		if ( get_psd_val(iface, &psd) < 0 )
			return -1;
		
		if(SIX_GHZ_CAPABILITY_INVALID != iface->hw_features->he_6ghz_capab.capab) {
			for (six_ghz_vap_idx = MASTER_VAP_BSS_IDX + 1;
				six_ghz_vap_idx < iface->num_bss; six_ghz_vap_idx++) {

				wpa_printf(MSG_DEBUG, "%s:%d - radio:%zd vap:%zd",
						__func__, __LINE__, i, six_ghz_vap_idx);

				os_memcpy(rnr[six_ghz_iface_idx][six_ghz_vap_idx].bssid,
					iface->bss[six_ghz_vap_idx]->conf->bssid, ETH_ALEN);

				rnr[six_ghz_iface_idx][six_ghz_vap_idx].OperatingClass =
					iface->bss[six_ghz_vap_idx]->iconf->op_class;

				rnr[six_ghz_iface_idx][six_ghz_vap_idx].Channel =
					iface->conf->channel;

				os_memcpy(rnr[six_ghz_iface_idx][six_ghz_vap_idx].ssid,
					iface->bss[six_ghz_vap_idx]->conf->ssid.ssid, SSID_MAX_LEN);

				rnr[six_ghz_iface_idx][six_ghz_vap_idx].multibss_enable =
					iface->bss[six_ghz_vap_idx]->iconf->multibss_enable;

				rnr[six_ghz_iface_idx][six_ghz_vap_idx].is_transmitted_bssid =
					(MULTIBSS_REFERENCE_BSS_IDX ==
						iface->bss[six_ghz_vap_idx]->conf->bss_idx);

				rnr[six_ghz_iface_idx][six_ghz_vap_idx].unsolicited_frame_support =
					iface->bss[six_ghz_vap_idx]->iconf->unsolicited_frame_support &
						FRAME_20TU_PROBE_RESPONSE_ENABLE;
				rnr[six_ghz_iface_idx][six_ghz_vap_idx].hidden_mode =
					!(iface->bss[six_ghz_vap_idx]->conf->ignore_broadcast_ssid == IGNORE_BROADCAST_SSID_DISABLED);

				rnr[six_ghz_iface_idx][six_ghz_vap_idx].psd_20MHz = psd;
			}
			six_ghz_iface_idx++;
		}
	}

	if(0 == six_ghz_iface_idx)
		return 0;

	for (radio_idx = 0; radio_idx < interfaces->count; radio_idx++) {

		/* For all enabled and non-6ghz radios */
		if(!interfaces->iface[radio_idx]->bss[MASTER_VAP_BSS_IDX]->drv_priv ||
			!interfaces->iface[radio_idx]->bss[MASTER_VAP_BSS_IDX]->started) {
			wpa_printf(MSG_DEBUG, "%s:%d - radio:%zd not enabled",
					__func__, __LINE__, radio_idx);
			continue;
		}

		/* For all non-6ghz radios */
		if(SIX_GHZ_CAPABILITY_INVALID != interfaces->iface[radio_idx]->hw_features->he_6ghz_capab.capab)
			continue;

		for (vap_idx = MASTER_VAP_BSS_IDX + 1; vap_idx < interfaces->iface[radio_idx]->num_bss; vap_idx++) {
			/* And every slave VAP */
			for (six_ghz_iface_idx = 0; six_ghz_iface_idx < MAX_SUPPORTED_6GHZ_RADIO; six_ghz_iface_idx++) {
				for (six_ghz_vap_idx = 0; six_ghz_vap_idx < MAX_SUPPORTED_6GHZ_VAP_PER_RADIO; six_ghz_vap_idx++) {
					if( (NULL == interfaces->iface[radio_idx]->bss[MASTER_VAP_BSS_IDX]->drv_priv) ||
					   !interfaces->iface[radio_idx]->bss[vap_idx]->conf->rnr_auto_update ||
					   !interfaces->iface[radio_idx]->bss[vap_idx]->started ) {
						wpa_printf(MSG_DEBUG, "%s:%d - Disabled or not feasible for radio:%zd vap:%zd",
							__func__, __LINE__, radio_idx, vap_idx);
						wpa_printf(MSG_DEBUG, "%s:%d - beacon_set_done:%d started:%d",
							__func__, __LINE__, interfaces->iface[radio_idx]->bss[vap_idx]->beacon_set_done,
							interfaces->iface[radio_idx]->bss[vap_idx]->started);
						continue;
					}

					if( rnr[six_ghz_iface_idx][six_ghz_vap_idx].OperatingClass &&
						rnr[six_ghz_iface_idx][six_ghz_vap_idx].Channel ) {
						wpa_printf(MSG_DEBUG,
						"%s:\n\tUpdating for radio=%s vap=%s\n\tRNR parameters"
						"- Channel=%d, OperatingClass=%d, bssid=" MACSTR 
						" is_transmitted_bssid=%d, multibss_enable=%d, ssid=%s, "
						"unsolicited_frame_support=%d" "hidden_mode=%d",
						__func__, interfaces->iface[radio_idx]->phy,
						interfaces->iface[radio_idx]->bss[vap_idx]->conf->iface,
						rnr[six_ghz_iface_idx][six_ghz_vap_idx].Channel,
						rnr[six_ghz_iface_idx][six_ghz_vap_idx].OperatingClass,
						MAC2STR(rnr[six_ghz_iface_idx][six_ghz_vap_idx].bssid),
						rnr[six_ghz_iface_idx][six_ghz_vap_idx].is_transmitted_bssid,
						rnr[six_ghz_iface_idx][six_ghz_vap_idx].multibss_enable,
						rnr[six_ghz_iface_idx][six_ghz_vap_idx].ssid,
						rnr[six_ghz_iface_idx][six_ghz_vap_idx].unsolicited_frame_support,
						rnr[six_ghz_iface_idx][six_ghz_vap_idx].hidden_mode);
						/* Set the found RNR entry */
						if( (ret = update_rnr_in_beacon(
						     interfaces->iface[radio_idx]->bss[vap_idx],
						     rnr[six_ghz_iface_idx][six_ghz_vap_idx],
						     six_ghz_iface_idx, six_ghz_vap_idx)) )
							return ret;
					}
				}
			}
		}
	}

	return ret;
}

int ieee802_11_set_beacon(struct hostapd_data *hapd)
{
	struct wpa_driver_ap_params params;
	struct hostapd_freq_params freq;
	struct hostapd_iface *iface = hapd->iface;
	struct hostapd_config *iconf = iface->conf;
	struct wpabuf *beacon, *proberesp, *assocresp;
	struct csa_settings csa_settings;
	int res, ret = -1;

	if (hapd->csa_in_progress) {
		wpa_printf(MSG_ERROR, "Cannot set beacons during CSA period");
		return -1;
	}

	hapd->beacon_set_done = 1;

	/* notify the driver to affect the BSS Load in the beacon frame */
	if (hostapd_drv_set_bss_load(hapd, hapd->conf->enable_bss_load_ie) < 0) {
		wpa_printf(MSG_ERROR, "Failed to set bss load in driver");
		return -1;
	}

	if (ieee802_11_build_ap_params(hapd, &params) < 0)
		return -1;

	if (hostapd_build_ap_extra_ies(hapd, &beacon, &proberesp, &assocresp) <
	    0)
		goto fail;

	params.beacon_ies = beacon;
	params.proberesp_ies = proberesp;
	params.assocresp_ies = assocresp;
	params.reenable = hapd->reenable_beacon;
	hapd->reenable_beacon = 0;

	if (iface->current_mode &&
	    hostapd_set_freq_params(&freq, iconf->hw_mode, iface->freq,
				    iconf->channel, iconf->enable_edmg,
				    iconf->edmg_channel, iconf->ieee80211n,
				    iconf->ieee80211ac, iconf->ieee80211ax,
				    iconf->secondary_channel,
				    hostapd_get_oper_chwidth(iconf),
                                    hostapd_get_oper_centr_freq_seg0_idx(iconf),
                                    hostapd_get_oper_centr_freq_seg1_idx(iconf),
				    iface->current_mode->vht_capab,
				    &iface->current_mode->he_capab) == 0)
		params.freq = &freq;


	if (iface->require_csa != 0) {
		iface->require_csa = 0;
		os_memset(&csa_settings, 0, sizeof(csa_settings));
		csa_settings.cs_count = DEFAULT_CS_COUNT;
		csa_settings.block_tx = 0;
		csa_settings.freq_params = freq;
		res = hostapd_switch_channel(hapd, &csa_settings);
		if (res) {
			wpa_printf(MSG_ERROR, " Unable to switch channel ret = %d \n", res);
			return -1;
		}
	}

	res = hostapd_drv_set_ap(hapd, &params);
	hostapd_free_ap_extra_ies(hapd, beacon, proberesp, assocresp);
	if (res) {
		wpa_printf(MSG_ERROR, "Failed to set beacon parameters");
		goto fail;
	}
	else {
		ret = 0;
	}

fail:
	ieee802_11_free_ap_params(&params);
	return ret;
}


/* only update beacon if started */
int ieee802_11_update_beacon(struct hostapd_data *hapd)
{
	if(hapd->started && hapd->beacon_set_done &&
	   ieee802_11_set_beacon(hapd) < 0) {
		return -1;
	}

	return 0;
}


int ieee802_11_set_beacons(struct hostapd_iface *iface)
{
	size_t i;
	int ret = 0;

	for (i = 0; i < iface->num_bss; i++) {
		if (iface->bss[i]->started &&
		    ieee802_11_set_beacon(iface->bss[i]) < 0)
			ret = -1;
	}

	return ret;
}


/* only update beacons if started */
int ieee802_11_update_beacons(struct hostapd_iface *iface)
{
	size_t i;
	int ret = 0;

	for (i = 0; i < iface->num_bss; i++) {
		if (iface->bss[i]->beacon_set_done && iface->bss[i]->started &&
		    ieee802_11_set_beacon(iface->bss[i]) < 0)
			ret = -1;
	}

	return ret;
}

#endif /* CONFIG_NATIVE_WINDOWS */
