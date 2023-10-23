/*
 * hostapd / IEEE 802.11 Management
 * Copyright (c) 2002-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "common/ocv.h"
#include "common/wpa_ctrl.h"
#include "hostapd.h"
#include "sta_info.h"
#include "ap_config.h"
#include "ap_drv_ops.h"
#include "wpa_auth.h"
#include "wpa_auth_glue.h"
#include "wpa_auth_i.h"
#include "wpa_auth_ie.h"
#include "ieee802_11.h"


u8 * hostapd_eid_assoc_comeback_time(struct hostapd_data *hapd,
				     struct sta_info *sta, u8 *eid)
{
	u8 *pos = eid;
	u32 timeout, tu;
	struct os_reltime now, passed;

	*pos++ = WLAN_EID_TIMEOUT_INTERVAL;
	*pos++ = 5;
	*pos++ = WLAN_TIMEOUT_ASSOC_COMEBACK;
	os_get_reltime(&now);
	os_reltime_sub(&now, &sta->sa_query_start, &passed);
	tu = (passed.sec * 1000000 + passed.usec) / 1024;
	if (hapd->conf->assoc_sa_query_max_timeout > tu)
		timeout = hapd->conf->assoc_sa_query_max_timeout - tu;
	else
		timeout = 0;
	if (timeout < hapd->conf->assoc_sa_query_max_timeout)
		timeout++; /* add some extra time for local timers */
	WPA_PUT_LE32(pos, timeout);
	pos += 4;

	return pos;
}


/* MLME-SAQuery.request */
void ieee802_11_send_sa_query_req(struct hostapd_data *hapd,
				  const u8 *addr, const u8 *trans_id)
{
#ifdef CONFIG_OCV
	struct sta_info *sta;
#endif /* CONFIG_OCV */
	struct ieee80211_mgmt *mgmt;
	u8 *oci_ie = NULL;
	u8 oci_ie_len = 0;
	u8 *end;

	wpa_printf(MSG_DEBUG, "IEEE 802.11: Sending SA Query Request to "
		   MACSTR, MAC2STR(addr));
	wpa_hexdump(MSG_DEBUG, "IEEE 802.11: SA Query Transaction ID",
		    trans_id, WLAN_SA_QUERY_TR_ID_LEN);

#ifdef CONFIG_OCV
	sta = ap_get_sta(hapd, addr);
	if (sta && wpa_auth_uses_ocv(sta->wpa_sm)) {
		struct wpa_channel_info ci;

		if (hostapd_drv_channel_info(hapd, &ci) != 0) {
			wpa_printf(MSG_WARNING,
				   "Failed to get channel info for OCI element in SA Query Request");
			return;
		}
#ifdef CONFIG_TESTING_OPTIONS
		if (hapd->conf->oci_freq_override_saquery_req) {
			wpa_printf(MSG_INFO,
				   "TEST: Override OCI frequency %d -> %u MHz",
				   ci.frequency,
				   hapd->conf->oci_freq_override_saquery_req);
			ci.frequency =
				hapd->conf->oci_freq_override_saquery_req;
		}
#endif /* CONFIG_TESTING_OPTIONS */

		oci_ie_len = OCV_OCI_EXTENDED_LEN;
		oci_ie = os_zalloc(oci_ie_len);
		if (!oci_ie) {
			wpa_printf(MSG_WARNING,
				   "Failed to allocate buffer for OCI element in SA Query Request");
			return;
		}

		if (ocv_insert_extended_oci(&ci, oci_ie) < 0) {
			os_free(oci_ie);
			return;
		}
	}
#endif /* CONFIG_OCV */

	mgmt = os_zalloc(sizeof(*mgmt) + oci_ie_len);
	if (!mgmt) {
		wpa_printf(MSG_DEBUG,
			   "Failed to allocate buffer for SA Query Response frame");
		os_free(oci_ie);
		return;
	}

	mgmt->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
					   WLAN_FC_STYPE_ACTION);
	os_memcpy(mgmt->da, addr, ETH_ALEN);
	os_memcpy(mgmt->sa, hapd->own_addr, ETH_ALEN);
	os_memcpy(mgmt->bssid, hapd->own_addr, ETH_ALEN);
	mgmt->u.action.category = WLAN_ACTION_SA_QUERY;
	mgmt->u.action.u.sa_query_req.action = WLAN_SA_QUERY_REQUEST;
	os_memcpy(mgmt->u.action.u.sa_query_req.trans_id, trans_id,
		  WLAN_SA_QUERY_TR_ID_LEN);
	end = mgmt->u.action.u.sa_query_req.variable;
#ifdef CONFIG_OCV
	if (oci_ie_len > 0) {
		os_memcpy(end, oci_ie, oci_ie_len);
		end += oci_ie_len;
	}
#endif /* CONFIG_OCV */
	if (hostapd_drv_send_mlme(hapd, mgmt, end - (u8 *) mgmt, 0, NULL, 0, 0)
	    < 0)
		wpa_printf(MSG_INFO, "ieee802_11_send_sa_query_req: send failed");

	os_free(mgmt);
	os_free(oci_ie);
}


static void ieee802_11_send_sa_query_resp(struct hostapd_data *hapd,
					  const u8 *sa, const u8 *trans_id)
{
	struct sta_info *sta;
	struct ieee80211_mgmt *resp;
	u8 *oci_ie = NULL;
	u8 oci_ie_len = 0;
	u8 *end;

	wpa_printf(MSG_DEBUG, "IEEE 802.11: Received SA Query Request from "
		   MACSTR, MAC2STR(sa));
	wpa_hexdump(MSG_DEBUG, "IEEE 802.11: SA Query Transaction ID",
		    trans_id, WLAN_SA_QUERY_TR_ID_LEN);

	sta = ap_get_sta(hapd, sa);
	if (sta == NULL || !(sta->flags & WLAN_STA_ASSOC)) {
		wpa_printf(MSG_DEBUG, "IEEE 802.11: Ignore SA Query Request "
			   "from unassociated STA " MACSTR, MAC2STR(sa));
		return;
	}

#ifdef CONFIG_OCV
	if (wpa_auth_uses_ocv(sta->wpa_sm)) {
		struct wpa_channel_info ci;

		if (hostapd_drv_channel_info(hapd, &ci) != 0) {
			wpa_printf(MSG_WARNING,
				   "Failed to get channel info for OCI element in SA Query Response");
			return;
		}
#ifdef CONFIG_TESTING_OPTIONS
		if (hapd->conf->oci_freq_override_saquery_resp) {
			wpa_printf(MSG_INFO,
				   "TEST: Override OCI frequency %d -> %u MHz",
				   ci.frequency,
				   hapd->conf->oci_freq_override_saquery_resp);
			ci.frequency =
				hapd->conf->oci_freq_override_saquery_resp;
		}
#endif /* CONFIG_TESTING_OPTIONS */

		oci_ie_len = OCV_OCI_EXTENDED_LEN;
		oci_ie = os_zalloc(oci_ie_len);
		if (!oci_ie) {
			wpa_printf(MSG_WARNING,
				   "Failed to allocate buffer for for OCI element in SA Query Response");
			return;
		}

		if (ocv_insert_extended_oci(&ci, oci_ie) < 0) {
			os_free(oci_ie);
			return;
		}
	}
#endif /* CONFIG_OCV */

	resp = os_zalloc(sizeof(*resp) + oci_ie_len);
	if (!resp) {
		wpa_printf(MSG_DEBUG,
			   "Failed to allocate buffer for SA Query Response frame");
		os_free(oci_ie);
		return;
	}

	wpa_printf(MSG_DEBUG, "IEEE 802.11: Sending SA Query Response to "
		   MACSTR, MAC2STR(sa));

	resp->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
					   WLAN_FC_STYPE_ACTION);
	os_memcpy(resp->da, sa, ETH_ALEN);
	os_memcpy(resp->sa, hapd->own_addr, ETH_ALEN);
	os_memcpy(resp->bssid, hapd->own_addr, ETH_ALEN);
	resp->u.action.category = WLAN_ACTION_SA_QUERY;
	resp->u.action.u.sa_query_req.action = WLAN_SA_QUERY_RESPONSE;
	os_memcpy(resp->u.action.u.sa_query_req.trans_id, trans_id,
		  WLAN_SA_QUERY_TR_ID_LEN);
	end = resp->u.action.u.sa_query_req.variable;
#ifdef CONFIG_OCV
	if (oci_ie_len > 0) {
		os_memcpy(end, oci_ie, oci_ie_len);
		end += oci_ie_len;
	}
#endif /* CONFIG_OCV */
	if (hostapd_drv_send_mlme(hapd, resp, end - (u8 *) resp, 0, NULL, 0, 0)
	    < 0)
		wpa_printf(MSG_INFO, "ieee80211_mgmt_sa_query_request: send failed");

	os_free(resp);
	os_free(oci_ie);
}


void ieee802_11_sa_query_action(struct hostapd_data *hapd,
				const struct ieee80211_mgmt *mgmt,
				size_t len)
{
	struct sta_info *sta;
	int i;
	const u8 *sa = mgmt->sa;
	const u8 action_type = mgmt->u.action.u.sa_query_resp.action;
	const u8 *trans_id = mgmt->u.action.u.sa_query_resp.trans_id;

	if (((const u8 *) mgmt) + len <
	    mgmt->u.action.u.sa_query_resp.variable) {
		wpa_printf(MSG_DEBUG,
			   "IEEE 802.11: Too short SA Query Action frame (len=%lu)",
			   (unsigned long) len);
		return;
	}

	sta = ap_get_sta(hapd, sa);

#ifdef CONFIG_OCV
	if (sta && wpa_auth_uses_ocv(sta->wpa_sm)) {
		struct ieee802_11_elems elems;
		struct wpa_channel_info ci;
		int tx_chanwidth;
		int tx_seg1_idx;
		size_t ies_len;
		const u8 *ies;

		ies = mgmt->u.action.u.sa_query_resp.variable;
		ies_len = len - (ies - (u8 *) mgmt);
		if (ieee802_11_parse_elems(ies, ies_len, &elems, 1) ==
		    ParseFailed) {
			wpa_printf(MSG_DEBUG,
				   "SA Query: Failed to parse elements");
			return;
		}

		if (hostapd_drv_channel_info(hapd, &ci) != 0) {
			wpa_printf(MSG_WARNING,
				   "Failed to get channel info to validate received OCI in SA Query Action frame");
			return;
		}

		if (get_sta_tx_parameters(sta->wpa_sm,
					  channel_width_to_int(ci.chanwidth),
					  ci.seg1_idx, &tx_chanwidth,
					  &tx_seg1_idx) < 0)
			return;

		if (ocv_verify_tx_params(elems.oci, elems.oci_len, &ci,
					 tx_chanwidth, tx_seg1_idx) != 0) {
			wpa_msg(hapd->msg_ctx, MSG_INFO, OCV_FAILURE "addr="
				MACSTR " frame=saquery%s error=%s",
				MAC2STR(sa),
				action_type == WLAN_SA_QUERY_REQUEST ?
				"req" : "resp", ocv_errorstr);
			return;
		}
	}
#endif /* CONFIG_OCV */

	if (action_type == WLAN_SA_QUERY_REQUEST) {
		ieee802_11_send_sa_query_resp(hapd, sa, trans_id);
		return;
	}

	if (action_type != WLAN_SA_QUERY_RESPONSE) {
		wpa_printf(MSG_DEBUG, "IEEE 802.11: Unexpected SA Query "
			   "Action %d", action_type);
		return;
	}

	wpa_printf(MSG_DEBUG, "IEEE 802.11: Received SA Query Response from "
		   MACSTR, MAC2STR(sa));
	wpa_hexdump(MSG_DEBUG, "IEEE 802.11: SA Query Transaction ID",
		    trans_id, WLAN_SA_QUERY_TR_ID_LEN);

	/* MLME-SAQuery.confirm */

	if (sta == NULL || sta->sa_query_trans_id == NULL) {
		wpa_printf(MSG_DEBUG, "IEEE 802.11: No matching STA with "
			   "pending SA Query request found");
		return;
	}

	for (i = 0; i < sta->sa_query_count; i++) {
		if (os_memcmp(sta->sa_query_trans_id +
			      i * WLAN_SA_QUERY_TR_ID_LEN,
			      trans_id, WLAN_SA_QUERY_TR_ID_LEN) == 0)
			break;
	}

	if (i >= sta->sa_query_count) {
		wpa_printf(MSG_DEBUG, "IEEE 802.11: No matching SA Query "
			   "transaction identifier found");
		return;
	}

	hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_DEBUG,
		       "Reply to pending SA Query received");
	ap_sta_stop_sa_query(hapd, sta);
}


static int ieee80211_ext_cap_check(u8 ext_cap, u8 mask, char *ext_cap_name)
{
	if (!(ext_cap & mask)) {
		wpa_printf(MSG_DEBUG, "Driver does not support configured EXT Cap: %s", ext_cap_name);
		return 0;
	}

	return 1;
}

static void hostapd_ext_capab_byte(struct hostapd_data *hapd, u8 *pos, int idx)
{

#define hw_support_ext_cap(cap, name) \
		ieee80211_ext_cap_check(ext_cap, WLAN_EXT_CAPAB_##cap, name)

	u8 ext_cap = 0;
	*pos = 0x00;

	if (idx < hapd->iface->extended_capa_len)
		ext_cap = hapd->iface->extended_capa[idx];

	switch (idx) {
	case 0: /* Bits 0-7 */
		if (hapd->iconf->obss_interval)
			*pos |= 0x01; /* Bit 0 - Coexistence management */
		if (hw_support_ext_cap(EXT_CSA_MASK, "[EXT_CSA]"))
			*pos |= 0x04; /* Bit 2 - Extended Channel Switching */
		break;
	case 1: /* Bits 8-15 */
		if (hapd->conf->proxy_arp)
			*pos |= 0x10; /* Bit 12 - Proxy ARP */
		if (hapd->conf->coloc_intf_reporting) {
			/* Bit 13 - Collocated Interference Reporting */
			*pos |= 0x20;
		}
		break;
	case 2: /* Bits 16-23 */
		if (hapd->conf->wnm_sleep_mode)
			*pos |= 0x02; /* Bit 17 - WNM-Sleep Mode */
		if (hapd->conf->bss_transition)
			*pos |= 0x08; /* Bit 19 - BSS Transition */
		if (hapd->iconf->multibss_enable)
			*pos |= 0x40; /* Bit 22 - Multiple BSSID */
		break;
	case 3: /* Bits 24-31 */
#ifdef CONFIG_WNM_AP
		if (hw_support_ext_cap(SSID_LIST_MASK, "[SSID-LIST]"))
			*pos |= 0x02; /* Bit 25 - SSID List */
#endif /* CONFIG_WNM_AP */
		if (hapd->conf->time_advertisement == 2)
			*pos |= 0x08; /* Bit 27 - UTC TSF Offset */
		if (hapd->conf->interworking)
			*pos |= 0x80; /* Bit 31 - Interworking */
		break;
	case 4: /* Bits 32-39 */
		if (hapd->conf->qos_map_set_len)
			*pos |= 0x01; /* Bit 32 - QoS Map */
		if (hapd->conf->tdls & TDLS_PROHIBIT)
			*pos |= 0x40; /* Bit 38 - TDLS Prohibited */
		if (hapd->conf->tdls & TDLS_PROHIBIT_CHAN_SWITCH) {
			/* Bit 39 - TDLS Channel Switching Prohibited */
			*pos |= 0x80;
		}
		break;
	case 5: /* Bits 40-47 */
#ifdef CONFIG_HS20
		if (hapd->conf->hs20) {
			if (hw_support_ext_cap(WNM_NOTIF_MASK, "[WNM-NOTIFICATION]"))
				*pos |= 0x40; /* Bit 46 - WNM-Notification */
		}
#endif /* CONFIG_HS20 */
#ifdef CONFIG_MBO
		if (hapd->conf->mbo_enabled) {
			if (hw_support_ext_cap(WNM_NOTIF_MASK, "[WNM-NOTIFICATION]"))
				*pos |= 0x40; /* Bit 46 - WNM-Notification */
		}
#endif /* CONFIG_MBO */
		break;
	case 6: /* Bits 48-55 */
		if (hapd->conf->ssid.utf8_ssid)
			*pos |= 0x01; /* Bit 48 - UTF-8 SSID */
		break;
	case 7: /* Bits 56-63 */
		if (hapd->conf->opmode_notif) {
			if (hw_support_ext_cap(OP_MODE_NOTIF_MASK, "[OPERATING-MODE-NOTIFICATION]"))
				*pos |= 0x40; /* Bit 62 - Operating Mode Notification */
		}
		if (hapd->iconf->ext_cap_max_num_msdus_in_amsdu & 0x01)
			*pos |= 0x80;	/* Bit 63 - LSB of Max Number of MSDUs in A-MSDU */
		break;
	case 8: /* Bits 64-71 */
		if (hapd->iconf->ext_cap_max_num_msdus_in_amsdu & 0x02)
			*pos |= 0x01;	/* Bit 64 - MSB of Max Number of MSDUs in A-MSDU */
		if (hapd->conf->ftm_responder)
			*pos |= 0x40; /* Bit 70 - FTM responder */
		if (hapd->conf->ftm_initiator)
			*pos |= 0x80; /* Bit 71 - FTM initiator */
		break;
	case 9: /* Bits 72-79 */
#ifdef CONFIG_FILS
		if ((hapd->conf->wpa & WPA_PROTO_RSN) &&
		    wpa_key_mgmt_fils(hapd->conf->wpa_key_mgmt))
			*pos |= 0x01;
#endif /* CONFIG_FILS */
		if(hapd->conf->twt_responder_support)
			*pos |= 0x40; /* Bit 78 */
		break;
	case 10: /* Bits 80-87 */
		if(hapd->iconf->multibss_enable)
			*pos |= 0x01; /* Bit 80 */
#ifdef CONFIG_SAE
		if (hapd->conf->wpa &&
		    wpa_key_mgmt_sae(hapd->conf->wpa_key_mgmt)) {
			int in_use = hostapd_sae_pw_id_in_use(hapd->conf);

			if (in_use)
				*pos |= 0x02; /* Bit 81 - SAE Password
					       * Identifiers In Use */
			if (in_use == 2)
				*pos |= 0x04; /* Bit 82 - SAE Password
					       * Identifiers Used Exclusively */
		}
#endif /* CONFIG_SAE */
		if (hapd->conf->beacon_protection_enabled)
			*pos |= 0x10; /* Bit 84 - Beacon Protection Enabled */
		break;
	case 11: /* Bits 88-95 */
#ifdef CONFIG_SAE_PK
		if (hapd->conf->wpa &&
		    wpa_key_mgmt_sae(hapd->conf->wpa_key_mgmt) &&
		    hostapd_sae_pk_exclusively(hapd->conf))
			*pos |= 0x01; /* Bit 88 - SAE PK Exclusively */
#endif /* CONFIG_SAE_PK */
		break;
	}
}


u8 * hostapd_eid_ext_capab(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;
	u8 len = 0, i;

	if (hapd->conf->tdls & (TDLS_PROHIBIT | TDLS_PROHIBIT_CHAN_SWITCH))
		len = 5;
	if (len < 4 && hapd->conf->interworking)
		len = 4;
	if (len < 3 && hapd->conf->wnm_sleep_mode)
		len = 3;
        if (len < 2 && hapd->conf->proxy_arp)
                len = 2;
	if (len < 1 && hapd->iconf->obss_interval)
		len = 1;
	if (len < 7 && hapd->conf->ssid.utf8_ssid)
		len = 7;
	if (len < 8 && hapd->conf->opmode_notif)
		len = 8;
	if ((len < 9) && (hapd->conf->ftm_initiator || hapd->conf->ftm_responder || hapd->iconf->ext_cap_max_num_msdus_in_amsdu))
		len = 9;
	if (len < 10 && (hapd->conf->twt_responder_support))
		len = 10;
	if (len < 11 && (hapd->iconf->multibss_enable))
		len = 11;
	if (len < 11 && (hapd->conf->beacon_protection_enabled))
		len = 11;

#ifdef CONFIG_WNM_AP
	if (len < 4)
		len = 4;
#endif /* CONFIG_WNM_AP */
#ifdef CONFIG_HS20
	if (hapd->conf->hs20 && len < 6)
		len = 6;
#endif /* CONFIG_HS20 */
#ifdef CONFIG_MBO
	if (hapd->conf->mbo_enabled && len < 6)
		len = 6;
#endif /* CONFIG_MBO */
#ifdef CONFIG_FILS
	if ((!(hapd->conf->wpa & WPA_PROTO_RSN) ||
	     !wpa_key_mgmt_fils(hapd->conf->wpa_key_mgmt)) && len < 10)
		len = 10;
#endif /* CONFIG_FILS */
#ifdef CONFIG_SAE
	if (len < 11 && hapd->conf->wpa &&
	    wpa_key_mgmt_sae(hapd->conf->wpa_key_mgmt) &&
	    hostapd_sae_pw_id_in_use(hapd->conf))
		len = 11;
#endif /* CONFIG_SAE */
#ifdef CONFIG_SAE_PK
	if (len < 12 && hapd->conf->wpa &&
	    wpa_key_mgmt_sae(hapd->conf->wpa_key_mgmt) &&
	    hostapd_sae_pk_exclusively(hapd->conf))
		len = 12;
#endif /* CONFIG_SAE_PK */
	if (len < hapd->iface->extended_capa_len)
		len = hapd->iface->extended_capa_len;
	if (len == 0)
		return eid;

	*pos++ = WLAN_EID_EXT_CAPAB;
	*pos++ = len;
	for (i = 0; i < len; i++, pos++) {
		hostapd_ext_capab_byte(hapd, pos, i);

		if (i < hapd->iface->extended_capa_len) {
			*pos &= ~hapd->iface->extended_capa_mask[i];
			*pos |= hapd->iface->extended_capa[i];
		}
	}

	while (len > 0 && eid[1 + len] == 0) {
		len--;
		eid[1] = len;
	}
	if (len == 0)
		return eid;

	return eid + 2 + len;
}

u8 * hostapd_eid_rm_enabled_capab(struct hostapd_data *hapd, u8 *eid,
					 size_t len)
{
	errno_t err;
	size_t i;

	for (i = 0; i < RRM_CAPABILITIES_IE_LEN; i++) {
		if (hapd->conf->radio_measurements[i])
			break;
	}

	if (i == RRM_CAPABILITIES_IE_LEN || len < 2 + RRM_CAPABILITIES_IE_LEN)
		return eid;

	*eid++ = WLAN_EID_RRM_ENABLED_CAPABILITIES;
	*eid++ = RRM_CAPABILITIES_IE_LEN;
	err = memcpy_s(eid, RRM_CAPABILITIES_IE_LEN, hapd->conf->radio_measurements, RRM_CAPABILITIES_IE_LEN);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return NULL;
	}

	return eid + RRM_CAPABILITIES_IE_LEN;
}

u8 round_pow2_up(u8 x)
{
	int i;
	u8  y = x;

	if (x == 0) return 0;

	for (i = 0; i < 8; i++) {
		x = x >> 1u;
		if (x == 0)
			break;
	}

	if (y ^ (1 << i))
		i++;

	return i;
}

static bool is_security_ie_required(struct hostapd_data *nontx_bss, struct hostapd_data *tx_bss)
{
	if (nontx_bss->conf->wpa_ie_required_for_mbss)
		return true;
	if ((nontx_bss->conf->wpa != 0) &&
		((nontx_bss->conf->wpa_key_mgmt != tx_bss->conf->wpa_key_mgmt) ||
		(nontx_bss->conf->sae_pwe != tx_bss->conf->sae_pwe) ||
		(nontx_bss->conf->wpa_pairwise != tx_bss->conf->wpa_pairwise) ||
		(nontx_bss->conf->rsn_pairwise != tx_bss->conf->rsn_pairwise) ||
		(nontx_bss->conf->group_cipher != tx_bss->conf->group_cipher) ||
		(nontx_bss->conf->rsn_preauth != tx_bss->conf->rsn_preauth) ||
		(nontx_bss->conf->group_mgmt_cipher != tx_bss->conf->group_mgmt_cipher) ||
		(nontx_bss->conf->ieee80211w != tx_bss->conf->ieee80211w))) {
		nontx_bss->conf->wpa_ie_required_for_mbss = 1;
		return true;
	}
	return false;
}


/* IE ID + Len */
#define IE_HDR_LEN 2
/* Minimum MBSSID IE length (includes Max BSSID Indicator) */
#define MIN_MBSSID_IE_ELEM_LEN (IE_HDR_LEN + 1)
/* Non Transmit BSSID element */
/* NON_TX_BSSID + LEN + NON_TX_BSS_CAPAB(4)+ NON_TX_MBSS_IND(3) + NON_TX_SSID(2) */
#define MIN_MBSSID_IE_NON_TX_BSS_PROF_HDR_LEN 2
#define MIN_MBSSID_IE_NON_TX_BSS_ELEM_LEN (MIN_MBSSID_IE_NON_TX_BSS_PROF_HDR_LEN + 9)
#define MAX_MBSSID_IE_TAG_LEN 255
#define MAX_MBSSID_IE_ELEM_LEN (IE_HDR_LEN + MAX_MBSSID_IE_TAG_LEN)
static int get_non_tx_bss_len(struct hostapd_data *bss)
{
	int len = MIN_MBSSID_IE_NON_TX_BSS_ELEM_LEN;
	/* ssid length check */
	if (!bss->conf->ignore_broadcast_ssid) {
		len = len + bss->conf->ssid.ssid_len;
	}

	if (is_security_ie_required(bss, bss->iface->bss[MULTIBSS_REFERENCE_BSS_IDX])) {
		len = len + WPA_IE_MAX_LEN;
	}
	return len;
}

int multiple_bssid_ie_get_len(struct hostapd_data *hapd)
{
	int i = 0;
	int num_ies = 1;
	int tot_len = MIN_MBSSID_IE_ELEM_LEN;
    /* Iterate over each BSS */
    for (i = MULTIBSS_REFERENCE_BSS_IDX + 1; i < hapd->iface->num_bss; i++) {
        struct hostapd_data *bss = hapd->iface->bss[i];
		int non_tx_bss_len = get_non_tx_bss_len(bss);
		if(non_tx_bss_len >= MAX_MBSSID_IE_TAG_LEN) {
			wpa_printf(MSG_ERROR,"Non-tx-bss size %d crossed max-size(254)",non_tx_bss_len);
			return -1;
		}
		tot_len += non_tx_bss_len;

		if (tot_len > (num_ies * MAX_MBSSID_IE_ELEM_LEN)) {
			num_ies++;
			tot_len += MIN_MBSSID_IE_ELEM_LEN;
		}
    }

	return tot_len;
}

u8 * hostapd_eid_multiple_bssid(struct hostapd_data *hapd, u8 *eid)
{
	int i;
	u8 *data_pos, *size_pos, *subelem_data_pos, *subelem_size_pos;
	struct wpa_auth_config *wpa_auth_conf = NULL;
	u8 *wpa_ie = NULL;
	size_t wpa_ie_len = 0;
	u8 max_idx = 0;
	int ie_len = MIN_MBSSID_IE_ELEM_LEN;


	wpa_ie = os_malloc(WPA_IE_MAX_LEN);
	if (wpa_ie == NULL) {
		wpa_printf(MSG_ERROR,"%s memory allocation failed for wpa_ie",__func__);
		return NULL;
	}
	wpa_auth_conf = os_malloc(sizeof(struct wpa_auth_config));
	if (wpa_auth_conf == NULL) {
		wpa_printf(MSG_ERROR,"%s memory allocation failed for wpa_auth_conf",__func__);
		eid = NULL;
		goto end;
	}

	*eid++ = WLAN_EID_MULTIPLE_BSSID;
	size_pos = eid;
	*eid++ = 0; /* Final size is not known yet */
	data_pos = eid;
	/* MaxBSSID Indicator computed excluding the Dummy VAP */
	max_idx = round_pow2_up(hapd->iface->num_bss - 1);
	*eid++ = max_idx;
	/* Iterate over each BSS */
	for (i = MULTIBSS_REFERENCE_BSS_IDX + 1; i < hapd->iface->num_bss; i++) {
		struct hostapd_data *bss = hapd->iface->bss[i];
		/* Separte MBSSID IE will be created if size is more than MAX_MBSSID_IE_ELEM_LEN */
		if ((get_non_tx_bss_len(bss) + ie_len) > MAX_MBSSID_IE_ELEM_LEN) {
			*size_pos = (eid - data_pos);
			ie_len = MIN_MBSSID_IE_ELEM_LEN;
			*eid++ = WLAN_EID_MULTIPLE_BSSID;
			size_pos = eid;
			*eid++ = 0;
			data_pos = eid;
			/* MaxBSSID Indicator computed excluding the Dummy VAP */
			*eid++ = max_idx;
		}
		wpa_ie_len = 0;

		os_memset(wpa_ie, 0, WPA_IE_MAX_LEN);

		*eid++ = WLAN_EID_SUBELEMENT_NONTRANSMITTED_BSSID_PROFILE;
		subelem_size_pos = eid;
		*eid++ = 0; /* Final subelement size is not known yet */
		subelem_data_pos = eid;
		/* Nontransmitted BSSID capability element */
		eid = hostapd_eid_nontransmitted_bssid_capability(bss, eid);
		if (!eid) {
			wpa_printf(MSG_ERROR, "hostapd_eid_nontransmitted_bssid_capability failed");
			goto end;
		}
		/* SSID element */
		eid = hostapd_eid_ssid(bss, eid);
		if (!eid) {
			wpa_printf(MSG_ERROR, "hostapd_eid_multiple_bssid ssid failed");
			goto end;
		}
		/* Multiple BSSID-Index */
		eid = hostapd_eid_multiple_bssid_idx(bss, eid, i - 1);
		if (!eid)
			goto end;

		/* Below IEs will be advertised if Non-Transmit BSS's security method
		 * is not same as Transmited BSS.
		 */
		if (bss->conf->wpa_ie_required_for_mbss) {
			/* create wpa_auth_conf from bss configuration */
			hostapd_wpa_auth_conf(bss->conf, bss->iconf, wpa_auth_conf);
			if (wpa_auth_conf_gen_multibss_wpa_ie(wpa_auth_conf, &wpa_ie, &wpa_ie_len) < 0) {
				wpa_printf(MSG_ERROR,"Multibss WPA-IE create failed !!!");
				eid = NULL;
				goto end;
			}

			if (wpa_ie && wpa_ie_len) {
				wpa_hexdump(MSG_DEBUG," Non-transmit BSS (MultiBSSID) wpa_ie dump", wpa_ie, wpa_ie_len);
				os_memcpy(eid, wpa_ie, wpa_ie_len);
				eid += wpa_ie_len;
			}
		}

		*subelem_size_pos = (eid - subelem_data_pos);
		ie_len += (*subelem_size_pos + MIN_MBSSID_IE_NON_TX_BSS_PROF_HDR_LEN);
	}

	if (size_pos && (*size_pos == 0))
		*size_pos = (eid - data_pos);

end:
	if(wpa_ie)
		os_free(wpa_ie);
	if(wpa_auth_conf)
		os_free(wpa_auth_conf);

	return eid;
}

u8 * hostapd_eid_ssid(struct hostapd_data *hapd, u8 *eid)
{
	errno_t err;
	*eid++ = WLAN_EID_SSID;
	if ( hapd->conf->ignore_broadcast_ssid == IGNORE_BROADCAST_SSID_CLEAR_SSID ) {
		/* clear the data, but keep the correct length of the SSID */
		*eid++ = hapd->conf->ssid.ssid_len;
		os_memset(eid, 0, hapd->conf->ssid.ssid_len);
		eid += hapd->conf->ssid.ssid_len;
	} else if ( hapd->conf->ignore_broadcast_ssid ) {
		*eid++ = 0; /* empty SSID */
	} else {
		*eid++ = hapd->conf->ssid.ssid_len;
		err = memcpy_s(eid, hapd->conf->ssid.ssid_len, hapd->conf->ssid.ssid, hapd->conf->ssid.ssid_len);
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return NULL;
		}
		eid += hapd->conf->ssid.ssid_len;
	}
	return eid;
}

u8 * hostapd_eid_multiple_bssid_idx(struct hostapd_data *hapd, u8 *eid, u8 idx)
{
	*eid++ = WLAN_EID_MLTIPLE_BSSID_INDEX;
	*eid++ = sizeof(u8);
	*eid++ = idx;
	return eid;
}

u8 * hostapd_eid_nontransmitted_bssid_capability(struct hostapd_data *hapd, u8 *eid)
{
	errno_t err;
	u16 capab_info;
	*eid++ = WLAN_EID_NONTRANSMITTED_BSSID_CAPA;
	*eid++ = sizeof(capab_info);
	capab_info = host_to_le16(hostapd_own_capab_info(hapd));
	err = memcpy_s(eid, sizeof(capab_info), (const void*)&capab_info, sizeof(capab_info));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return NULL;
	}
	eid += sizeof(capab_info);
	return eid;
}

u8 * hostapd_eid_qos_map_set(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;
	u8 len = hapd->conf->qos_map_set_len;

	if (!len)
		return eid;

	*pos++ = WLAN_EID_QOS_MAP_SET;
	*pos++ = len;
	os_memcpy(pos, hapd->conf->qos_map_set, len);
	pos += len;

	return pos;
}


u8 * hostapd_eid_interworking(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;
#ifdef CONFIG_INTERWORKING
	u8 *len;

	if (!hapd->conf->interworking)
		return eid;

	*pos++ = WLAN_EID_INTERWORKING;
	len = pos++;

	*pos = hapd->conf->access_network_type;
	if (hapd->conf->internet)
		*pos |= INTERWORKING_ANO_INTERNET;
	if (hapd->conf->asra)
		*pos |= INTERWORKING_ANO_ASRA;
	if (hapd->conf->esr)
		*pos |= INTERWORKING_ANO_ESR;
	if (hapd->conf->uesa)
		*pos |= INTERWORKING_ANO_UESA;
	pos++;

	if (hapd->conf->venue_info_set) {
		*pos++ = hapd->conf->venue_group;
		*pos++ = hapd->conf->venue_type;
	}

	if (!is_zero_ether_addr(hapd->conf->hessid)) {
		os_memcpy(pos, hapd->conf->hessid, ETH_ALEN);
		pos += ETH_ALEN;
	}

	*len = pos - len - 1;
#endif /* CONFIG_INTERWORKING */

	return pos;
}


u8 * hostapd_eid_adv_proto(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;
#ifdef CONFIG_INTERWORKING

	/* TODO: Separate configuration for ANQP? */
	if (!hapd->conf->interworking)
		return eid;

	*pos++ = WLAN_EID_ADV_PROTO;
	*pos++ = 2;
	 /* Query Response Length Limit | PAME-BI */
	*pos++ = (hapd->iconf->adv_proto_query_resp_len & ADV_PROTO_QUERY_RESP_LEN_MASK);
	*pos++ = ACCESS_NETWORK_QUERY_PROTOCOL;
#endif /* CONFIG_INTERWORKING */

	return pos;
}


u8 * hostapd_eid_roaming_consortium(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;
#ifdef CONFIG_INTERWORKING
	u8 *len;
	unsigned int i, count;

	if (!hapd->conf->interworking ||
	    hapd->conf->roaming_consortium == NULL ||
	    hapd->conf->roaming_consortium_count == 0)
		return eid;

	*pos++ = WLAN_EID_ROAMING_CONSORTIUM;
	len = pos++;

	/* Number of ANQP OIs (in addition to the max 3 listed here) */
	if (hapd->conf->roaming_anqp_ois_count_set) {
		*pos++ = (u8)hapd->conf->roaming_anqp_ois_count;
	} else {
		if (hapd->conf->roaming_consortium_count > 3 + 255)
			*pos++ = 255;
		else if (hapd->conf->roaming_consortium_count > 3)
			*pos++ = hapd->conf->roaming_consortium_count - 3;
		else
			*pos++ = 0;
	}

	/* OU #1 and #2 Lengths */
	*pos = hapd->conf->roaming_consortium[0].len;
	if (hapd->conf->roaming_consortium_count > 1)
		*pos |= hapd->conf->roaming_consortium[1].len << 4;
	pos++;

	if (hapd->conf->roaming_consortium_count > 3)
		count = 3;
	else
		count = hapd->conf->roaming_consortium_count;

	for (i = 0; i < count; i++) {
		os_memcpy(pos, hapd->conf->roaming_consortium[i].oi,
			  hapd->conf->roaming_consortium[i].len);
		pos += hapd->conf->roaming_consortium[i].len;
	}

	*len = pos - len - 1;
#endif /* CONFIG_INTERWORKING */

	return pos;
}


u8 * hostapd_eid_time_adv(struct hostapd_data *hapd, u8 *eid)
{
	if (hapd->conf->time_advertisement != 2)
		return eid;

	if (hapd->time_adv == NULL &&
	    hostapd_update_time_adv(hapd) < 0)
		return eid;

	if (hapd->time_adv == NULL)
		return eid;

	os_memcpy(eid, wpabuf_head(hapd->time_adv),
		  wpabuf_len(hapd->time_adv));
	eid += wpabuf_len(hapd->time_adv);

	return eid;
}


u8 * hostapd_eid_time_zone(struct hostapd_data *hapd, u8 *eid)
{
	size_t len;

	if (hapd->conf->time_advertisement != 2 || !hapd->conf->time_zone)
		return eid;

	len = os_strlen(hapd->conf->time_zone);

	*eid++ = WLAN_EID_TIME_ZONE;
	*eid++ = len;
	os_memcpy(eid, hapd->conf->time_zone, len);
	eid += len;

	return eid;
}


int hostapd_update_time_adv(struct hostapd_data *hapd)
{
	const int elen = 2 + 1 + 10 + 5 + 1;
	struct os_time t;
	struct os_tm tm;
	u8 *pos;

	if (hapd->conf->time_advertisement != 2)
		return 0;

	if (os_get_time(&t) < 0 || os_gmtime(t.sec, &tm) < 0)
		return -1;

	if (!hapd->time_adv) {
		hapd->time_adv = wpabuf_alloc(elen);
		if (hapd->time_adv == NULL)
			return -1;
		pos = wpabuf_put(hapd->time_adv, elen);
	} else
		pos = wpabuf_mhead_u8(hapd->time_adv);

	*pos++ = WLAN_EID_TIME_ADVERTISEMENT;
	*pos++ = 1 + 10 + 5 + 1;

	*pos++ = 2; /* UTC time at which the TSF timer is 0 */

	/* Time Value at TSF 0 */
	/* FIX: need to calculate this based on the current TSF value */
	WPA_PUT_LE16(pos, tm.year); /* Year */
	pos += 2;
	*pos++ = tm.month; /* Month */
	*pos++ = tm.day; /* Day of month */
	*pos++ = tm.hour; /* Hours */
	*pos++ = tm.min; /* Minutes */
	*pos++ = tm.sec; /* Seconds */
	WPA_PUT_LE16(pos, 0); /* Milliseconds (not used) */
	pos += 2;
	*pos++ = 0; /* Reserved */

	/* Time Error */
	/* TODO: fill in an estimate on the error */
	*pos++ = 0;
	*pos++ = 0;
	*pos++ = 0;
	*pos++ = 0;
	*pos++ = 0;

	*pos++ = hapd->time_update_counter++;

	return 0;
}


u8 * hostapd_eid_bss_max_idle_period(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;

#ifdef CONFIG_WNM_AP
	if (hapd->conf->ap_max_inactivity > 0) {
		unsigned int val;
		*pos++ = WLAN_EID_BSS_MAX_IDLE_PERIOD;
		*pos++ = 3;
		val = hapd->conf->ap_max_inactivity;
		if (val > 68000)
			val = 68000;
		val *= 1000;
		val /= 1024;
		if (val == 0)
			val = 1;
		if (val > 65535)
			val = 65535;
		WPA_PUT_LE16(pos, val);
		pos += 2;
		*pos++ = hapd->conf->ap_protected_keep_alive_required;
	}
#endif /* CONFIG_WNM_AP */

	return pos;
}


#ifdef CONFIG_MBO

u8 * hostapd_eid_mbo_rssi_assoc_rej(struct hostapd_data *hapd, u8 *eid,
				    size_t len, int delta)
{
	u8 mbo[4];

	mbo[0] = OCE_ATTR_ID_RSSI_BASED_ASSOC_REJECT;
	mbo[1] = 2;
	/* Delta RSSI */
	mbo[2] = delta;
	/* Retry delay */
	mbo[3] = hapd->iconf->rssi_reject_assoc_timeout;

	return eid + mbo_add_ie(eid, len, mbo, 4);
}


u8 * hostapd_eid_mbo(struct hostapd_data *hapd, u8 *eid, size_t len)
{
	u8 mbo[9], *mbo_pos = mbo;
	u8 *pos = eid;

	if (!hapd->conf->mbo_enabled &&
	    !OCE_STA_CFON_ENABLED(hapd) && !OCE_AP_ENABLED(hapd))
		return eid;

	if (hapd->conf->mbo_enabled) {
		*mbo_pos++ = MBO_ATTR_ID_AP_CAPA_IND;
		*mbo_pos++ = 1;
		if (hapd->conf->mbo_cell_aware)
			*mbo_pos++ = MBO_AP_CAPA_CELL_AWARE;
		else
			*mbo_pos++ = 0; /* Not Cellular aware */

		if (hapd->mbo_assoc_disallow) {
			*mbo_pos++ = MBO_ATTR_ID_ASSOC_DISALLOW;
			*mbo_pos++ = 1;
			*mbo_pos++ = hapd->mbo_assoc_disallow;
		} else if (hapd->mbo_bss_assoc_disallow) {
			*mbo_pos++ = MBO_ATTR_ID_ASSOC_DISALLOW;
			*mbo_pos++ = 1;
			*mbo_pos++ = hapd->mbo_bss_assoc_disallow;
		}
	}

	if (OCE_STA_CFON_ENABLED(hapd) || OCE_AP_ENABLED(hapd)) {
		u8 ctrl;

		ctrl = OCE_RELEASE;
		if (OCE_STA_CFON_ENABLED(hapd) && !OCE_AP_ENABLED(hapd))
			ctrl |= OCE_IS_STA_CFON;

		*mbo_pos++ = OCE_ATTR_ID_CAPA_IND;
		*mbo_pos++ = 1;
		*mbo_pos++ = ctrl;
	}

	pos += mbo_add_ie(pos, len, mbo, mbo_pos - mbo);

	return pos;
}


u8 hostapd_mbo_ie_len(struct hostapd_data *hapd)
{
	u8 len;

	if (!hapd->conf->mbo_enabled &&
	    !OCE_STA_CFON_ENABLED(hapd) && !OCE_AP_ENABLED(hapd))
		return 0;

	/*
	 * MBO IE header (6) + Capability Indication attribute (3) +
	 * Association Disallowed attribute (3) = 12
	 */
	len = 6;
	if (hapd->conf->mbo_enabled)
		len += 3 + (hapd->mbo_assoc_disallow ? 3 : 0);

	/* OCE capability indication attribute (3) */
	if (OCE_STA_CFON_ENABLED(hapd) || OCE_AP_ENABLED(hapd))
		len += 3;

	return len;
}

#endif /* CONFIG_MBO */


#ifdef CONFIG_OWE
static int hostapd_eid_owe_trans_enabled(struct hostapd_data *hapd)
{
	return hapd->conf->owe_transition_ssid_len > 0 &&
		!is_zero_ether_addr(hapd->conf->owe_transition_bssid);
}
#endif /* CONFIG_OWE */


size_t hostapd_eid_owe_trans_len(struct hostapd_data *hapd)
{
#ifdef CONFIG_OWE
	if (!hostapd_eid_owe_trans_enabled(hapd))
		return 0;
	return 6 + ETH_ALEN + 1 + hapd->conf->owe_transition_ssid_len;
#else /* CONFIG_OWE */
	return 0;
#endif /* CONFIG_OWE */
}


u8 * hostapd_eid_owe_trans(struct hostapd_data *hapd, u8 *eid,
				  size_t len)
{
#ifdef CONFIG_OWE
	u8 *pos = eid;
	size_t elen;

	if (hapd->conf->owe_transition_ifname[0] &&
	    !hostapd_eid_owe_trans_enabled(hapd))
		hostapd_owe_trans_get_info(hapd);

	if (!hostapd_eid_owe_trans_enabled(hapd))
		return pos;

	elen = hostapd_eid_owe_trans_len(hapd);
	if (len < elen) {
		wpa_printf(MSG_DEBUG,
			   "OWE: Not enough room in the buffer for OWE IE");
		return pos;
	}

	*pos++ = WLAN_EID_VENDOR_SPECIFIC;
	*pos++ = elen - 2;
	WPA_PUT_BE24(pos, OUI_WFA);
	pos += 3;
	*pos++ = OWE_OUI_TYPE;
	os_memcpy(pos, hapd->conf->owe_transition_bssid, ETH_ALEN);
	pos += ETH_ALEN;
	*pos++ = hapd->conf->owe_transition_ssid_len;
	os_memcpy(pos, hapd->conf->owe_transition_ssid,
		  hapd->conf->owe_transition_ssid_len);
	pos += hapd->conf->owe_transition_ssid_len;

	return pos;
#else /* CONFIG_OWE */
	return eid;
#endif /* CONFIG_OWE */
}

u8 * hostapd_eid_multi_ap(struct hostapd_data *hapd, u8 *eid, size_t len)
{
	u8 multi_ap[10], *multi_ap_pos = multi_ap;
	u8 *pos = eid;
	u8 bss_type;

	switch (hapd->conf->mesh_mode) {
	case MESH_MODE_FRONTHAUL_AP:
		bss_type = MULTI_AP_SUBELEMENT_FAP;
		if (hapd->conf->mesh_mode_set && hapd->conf->multi_ap_backhaul_ssid.ssid_len)
			bss_type |= MULTI_AP_SUBELEMENT_BAP;
		break;
	case MESH_MODE_BACKHAUL_AP:
		bss_type = MULTI_AP_SUBELEMENT_BAP;
		break;
	default:
		bss_type = MULTI_AP_SUBELEMENT_BAP | MULTI_AP_SUBELEMENT_FAP;
		break;
	}

	if (bss_type & MULTI_AP_SUBELEMENT_BAP) {
		if (hapd->conf->multi_ap_profile1_disallow) bss_type |= MULTI_AP_PROFILE1_DISALLOWED;
		if (hapd->conf->multi_ap_profile2_disallow) bss_type |= MULTI_AP_PROFILE2_DISALLOWED;
	}

	*multi_ap_pos++ = MULTI_AP_SUBELEMENT_ID;
	*multi_ap_pos++ = MULTI_AP_SUBELEMENT_LEN;
	*multi_ap_pos++ = bss_type;

	/* Multi-AP Specification v2.0
	 * If a Multi-AP Agent that implements Profile-2 sends a Multi-AP IE in
	 * a (Re)Association Request or Response frame, it shall include a
	 * Multi-AP Profile subelement with the Multi-AP Profile field set to 0x02.
	 */
	if ((bss_type & MULTI_AP_SUBELEMENT_BAP) &&
	    (hapd->iconf->multi_ap_profile == MULTI_AP_PROFILE2)) {
		*multi_ap_pos++ = MULTI_AP_PROFILE_SUBELEMENT_ID;
		*multi_ap_pos++ = MULTI_AP_PROFILE_SUBELEMENT_LEN;
		*multi_ap_pos++ = MULTI_AP_PROFILE2;

		/* Multi-AP Default 802.1Q Setting subelement.*/
		if (hapd->iconf->multi_ap_primary_vlanid != VLAN_ID_DISABLED){
			*multi_ap_pos++ = MULTI_AP_8021Q_SUBELEMENT_ID;
			*multi_ap_pos++ = MULTI_AP_8021Q_SUBELEMENT_LEN;
			WPA_PUT_LE16(multi_ap_pos, hapd->iconf->multi_ap_primary_vlanid);
			multi_ap_pos += 2;
		}
	}

	pos += multi_ap_add_ie(pos, len, multi_ap, multi_ap_pos - multi_ap);

	return pos;
}


size_t hostapd_eid_dpp_cc_len(struct hostapd_data *hapd)
{
#ifdef CONFIG_DPP2
	if (hapd->conf->dpp_configurator_connectivity)
		return 6;
#endif /* CONFIG_DPP2 */
	return 0;
}


u8 * hostapd_eid_dpp_cc(struct hostapd_data *hapd, u8 *eid, size_t len)
{
	u8 *pos = eid;

#ifdef CONFIG_DPP2
	if (!hapd->conf->dpp_configurator_connectivity || len < 6)
		return pos;

	*pos++ = WLAN_EID_VENDOR_SPECIFIC;
	*pos++ = 4;
	WPA_PUT_BE24(pos, OUI_WFA);
	pos += 3;
	*pos++ = DPP_CC_OUI_TYPE;
#endif /* CONFIG_DPP2 */

	return pos;
}


void ap_copy_sta_supp_op_classes(struct sta_info *sta,
				 const u8 *supp_op_classes,
				 size_t supp_op_classes_len)
{
	if (!supp_op_classes)
		return;
	os_free(sta->supp_op_classes);
	sta->supp_op_classes = os_malloc(1 + supp_op_classes_len);
	if (!sta->supp_op_classes)
		return;

	sta->supp_op_classes[0] = supp_op_classes_len;
	os_memcpy(sta->supp_op_classes + 1, supp_op_classes,
		  supp_op_classes_len);
}


u8 * hostapd_eid_fils_indic(struct hostapd_data *hapd, u8 *eid, int hessid)
{
	u8 *pos = eid;
#ifdef CONFIG_FILS
	u8 *len;
	u16 fils_info = 0;
	size_t realms;
	struct fils_realm *realm;

	if (!(hapd->conf->wpa & WPA_PROTO_RSN) ||
	    !wpa_key_mgmt_fils(hapd->conf->wpa_key_mgmt))
		return pos;

	realms = dl_list_len(&hapd->conf->fils_realms);
	if (realms > 7)
		realms = 7; /* 3 bit count field limits this to max 7 */

	*pos++ = WLAN_EID_FILS_INDICATION;
	len = pos++;
	/* TODO: B0..B2: Number of Public Key Identifiers */
	if (hapd->conf->erp_domain) {
		/* B3..B5: Number of Realm Identifiers */
		fils_info |= realms << 3;
	}
	/* TODO: B6: FILS IP Address Configuration */
	if (hapd->conf->fils_cache_id_set)
		fils_info |= BIT(7);
	if (hessid && !is_zero_ether_addr(hapd->conf->hessid))
		fils_info |= BIT(8); /* HESSID Included */
	/* FILS Shared Key Authentication without PFS Supported */
	fils_info |= BIT(9);
	if (hapd->conf->fils_dh_group) {
		/* FILS Shared Key Authentication with PFS Supported */
		fils_info |= BIT(10);
	}
	/* TODO: B11: FILS Public Key Authentication Supported */
	/* B12..B15: Reserved */
	WPA_PUT_LE16(pos, fils_info);
	pos += 2;
	if (hapd->conf->fils_cache_id_set) {
		os_memcpy(pos, hapd->conf->fils_cache_id, FILS_CACHE_ID_LEN);
		pos += FILS_CACHE_ID_LEN;
	}
	if (hessid && !is_zero_ether_addr(hapd->conf->hessid)) {
		os_memcpy(pos, hapd->conf->hessid, ETH_ALEN);
		pos += ETH_ALEN;
	}

	dl_list_for_each(realm, &hapd->conf->fils_realms, struct fils_realm,
			 list) {
		if (realms == 0)
			break;
		realms--;
		os_memcpy(pos, realm->hash, 2);
		pos += 2;
	}
	*len = pos - len - 1;
#endif /* CONFIG_FILS */

	return pos;
}


#ifdef CONFIG_OCV
int get_tx_parameters(struct sta_info *sta, int ap_max_chanwidth,
		      int ap_seg1_idx, int *bandwidth, int *seg1_idx)
{
	int ht_40mhz = 0;
	int vht_80p80 = 0;
	int requested_bw;

	if (sta->ht_capabilities)
		ht_40mhz = !!(sta->ht_capabilities->ht_capabilities_info &
			      HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET);

	if (sta->vht_operation) {
		struct ieee80211_vht_operation *oper = sta->vht_operation;

		/*
		 * If a VHT Operation element was present, use it to determine
		 * the supported channel bandwidth.
		 */
		if (oper->vht_op_info_chwidth == 0) {
			requested_bw = ht_40mhz ? 40 : 20;
		} else if (oper->vht_op_info_chan_center_freq_seg1_idx == 0) {
			requested_bw = 80;
		} else {
			int diff;

			requested_bw = 160;
			diff = abs((int)
				   oper->vht_op_info_chan_center_freq_seg0_idx -
				   (int)
				   oper->vht_op_info_chan_center_freq_seg1_idx);
			vht_80p80 = oper->vht_op_info_chan_center_freq_seg1_idx
				!= 0 &&	diff > 16;
		}
	} else if (sta->vht_capabilities) {
		struct ieee80211_vht_capabilities *capab;
		int vht_chanwidth;

		capab = sta->vht_capabilities;

		/*
		 * If only the VHT Capabilities element is present (e.g., for
		 * normal clients), use it to determine the supported channel
		 * bandwidth.
		 */
		vht_chanwidth = capab->vht_capabilities_info &
			VHT_CAP_SUPP_CHAN_WIDTH_MASK;
		vht_80p80 = capab->vht_capabilities_info &
			VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ;

		/* TODO: Also take into account Extended NSS BW Support field */
		requested_bw = vht_chanwidth ? 160 : 80;
	} else {
		requested_bw = ht_40mhz ? 40 : 20;
	}

	*bandwidth = requested_bw < ap_max_chanwidth ?
		requested_bw : ap_max_chanwidth;

	*seg1_idx = 0;
	if (ap_seg1_idx && vht_80p80)
		*seg1_idx = ap_seg1_idx;

	return 0;
}
#endif /* CONFIG_OCV */


u8 * hostapd_eid_rsnxe(struct hostapd_data *hapd, u8 *eid, size_t len)
{
	u8 *pos = eid;
	bool sae_pk = false;

#ifdef CONFIG_SAE_PK
	sae_pk = hostapd_sae_pk_in_use(hapd->conf);
#endif /* CONFIG_SAE_PK */

	if (!(hapd->conf->wpa & WPA_PROTO_RSN) ||
	    !wpa_key_mgmt_sae(hapd->conf->wpa_key_mgmt) ||
	    (hapd->conf->sae_pwe != 1 && hapd->conf->sae_pwe != 2 &&
	     !hostapd_sae_pw_id_in_use(hapd->conf) && !sae_pk) ||
	    hapd->conf->sae_pwe == 3 ||
	    len < 3)
		return pos;

	*pos++ = WLAN_EID_RSNX;
	*pos++ = 1;
	/* bits 0-3 = 0 since only one octet of Extended RSN Capabilities is
	 * used for now */
	*pos = BIT(WLAN_RSNX_CAPAB_SAE_H2E);
#ifdef CONFIG_SAE_PK
	if (sae_pk)
		*pos |= BIT(WLAN_RSNX_CAPAB_SAE_PK);
#endif /* CONFIG_SAE_PK */
	pos++;

	return pos;
}

u8 * hostapd_eid_opmodenotif(struct hostapd_data *hapd, u8 *eid)
{
	u8 *pos = eid;
	u8 bw = hostapd_get_oper_chwidth(hapd->iface->conf);
	int omn_ie = hapd->iface->require_omn;

	if (!omn_ie)
		return pos;

	*pos++ = WLAN_EID_VHT_OPERATING_MODE_NOTIFICATION;
	*pos++ = 1;

	if (omn_ie == COC_POWER_1x1_STATE) {
		*pos = 0;
	} else if (omn_ie == COC_POWER_2x2_STATE) {
		*pos = BIT(4);
	} else if (omn_ie == COC_POWER_3x3_STATE) {
		*pos = BIT(5);
	} else if (omn_ie == COC_POWER_4x4_STATE) {
		*pos = BIT(4) | BIT (5);
	}

	switch (bw) {
		case CHANWIDTH_USE_HT:
			if (hapd->iface->conf->secondary_channel == 0) {
				*pos &= ~BIT(0);  /* For 20Mhz value should be zero*/
			} else {
				*pos |= BIT(0);
			}
			break;
		case CHANWIDTH_80MHZ:
			*pos |= BIT(1);
			break;
		case CHANWIDTH_160MHZ:
			*pos |=  BIT (1) | BIT(2);
			break;
		default:
			*pos &= ~BIT(0);
			wpa_printf(MSG_INFO,"BW parameter on OMN IE  set to Default 20Mhz \n");
			break;
	}

	pos++;
	return pos;
}
