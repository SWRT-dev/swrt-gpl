/*
 * hostapd / Callback functions for driver wrappers
 * Copyright (c) 2002-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/eloop.h"
#include "radius/radius.h"
#include "drivers/driver.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "common/wpa_ctrl.h"
#include "common/dpp.h"
#include "common/sae.h"
#include "common/hw_features_common.h"
#include "crypto/random.h"
#include "p2p/p2p.h"
#include "wps/wps.h"
#include "fst/fst.h"
#include "wnm_ap.h"
#include "hostapd.h"
#include "ieee802_11.h"
#include "ieee802_11_auth.h"
#include "sta_info.h"
#include "accounting.h"
#include "tkip_countermeasures.h"
#include "ieee802_1x.h"
#include "wpa_auth.h"
#include "wps_hostapd.h"
#include "ap_drv_ops.h"
#include "ap_config.h"
#include "ap_mlme.h"
#include "hw_features.h"
#include "dfs.h"
#include "beacon.h"
#include "mbo_ap.h"
#include "dpp_hostapd.h"
#include "fils_hlp.h"
#include "neighbor_db.h"
#include "acs.h"
#include "wmm.h"
#include "common/linux_bridge.h"
#include "drivers/linux_ioctl.h"

#ifdef CONFIG_FILS
void hostapd_notify_assoc_fils_finish(struct hostapd_data *hapd,
				      struct sta_info *sta)
{
	u16 reply_res = WLAN_STATUS_SUCCESS;
	struct ieee802_11_elems elems;
	u8 buf[IEEE80211_MAX_MMPDU_SIZE], *p = buf;
	int new_assoc;

	wpa_printf(MSG_DEBUG, "%s FILS: Finish association with " MACSTR,
		   __func__, MAC2STR(sta->addr));
	eloop_cancel_timeout(fils_hlp_timeout, hapd, sta);
	if (!sta->fils_pending_assoc_req)
		return;

	ieee802_11_parse_elems(sta->fils_pending_assoc_req,
			       sta->fils_pending_assoc_req_len, &elems, 0);
	if (!elems.fils_session) {
		wpa_printf(MSG_DEBUG, "%s failed to find FILS Session element",
			   __func__);
		return;
	}

	p = hostapd_eid_assoc_fils_session(sta->wpa_sm, p,
					   elems.fils_session,
					   sta->fils_hlp_resp);

	reply_res = hostapd_sta_assoc(hapd, sta->addr,
				      sta->fils_pending_assoc_is_reassoc,
				      WLAN_STATUS_SUCCESS,
				      buf, p - buf);
	ap_sta_set_authorized(hapd, sta, 1);
	new_assoc = (sta->flags & WLAN_STA_ASSOC) == 0;
	sta->flags |= WLAN_STA_AUTH | WLAN_STA_ASSOC;
	sta->flags &= ~WLAN_STA_WNM_SLEEP_MODE;
	hostapd_set_sta_flags(hapd, sta);
	wpa_auth_sm_event(sta->wpa_sm, WPA_ASSOC_FILS);
	ieee802_1x_notify_port_enabled(sta->eapol_sm, 1);
	hostapd_new_assoc_sta(hapd, sta, !new_assoc);
	os_free(sta->fils_pending_assoc_req);
	sta->fils_pending_assoc_req = NULL;
	sta->fils_pending_assoc_req_len = 0;
	wpabuf_free(sta->fils_hlp_resp);
	sta->fils_hlp_resp = NULL;
	wpabuf_free(sta->hlp_dhcp_discover);
	sta->hlp_dhcp_discover = NULL;
	fils_hlp_deinit(hapd);

	/*
	 * Remove the station in case transmission of a success response fails
	 * (the STA was added associated to the driver) or if the station was
	 * previously added unassociated.
	 */
	if (reply_res != WLAN_STATUS_SUCCESS || sta->added_unassoc) {
		hostapd_drv_sta_remove(hapd, sta->addr);
		sta->added_unassoc = 0;
	}
}
#endif /* CONFIG_FILS */


int hostapd_notif_assoc(struct hostapd_data *hapd, const u8 *addr,
			const u8 *req_ies, size_t req_ies_len, int reassoc)
{
	struct sta_info *sta;
	int new_assoc, res;
	struct ieee802_11_elems elems;
	const u8 *ie;
	size_t ielen;
	u8 buf[sizeof(struct ieee80211_mgmt) + 1024];
	u8 *p = buf;
	u16 reason = WLAN_REASON_UNSPECIFIED;
	u16 status = WLAN_STATUS_SUCCESS;
	const u8 *p2p_dev_addr = NULL;

	if (addr == NULL) {
		/*
		 * This could potentially happen with unexpected event from the
		 * driver wrapper. This was seen at least in one case where the
		 * driver ended up being set to station mode while hostapd was
		 * running, so better make sure we stop processing such an
		 * event here.
		 */
		wpa_printf(MSG_DEBUG,
			   "hostapd_notif_assoc: Skip event with no address");
		return -1;
	}

	if (is_multicast_ether_addr(addr) ||
	    is_zero_ether_addr(addr) ||
	    os_memcmp(addr, hapd->own_addr, ETH_ALEN) == 0) {
		/* Do not process any frames with unexpected/invalid SA so that
		 * we do not add any state for unexpected STA addresses or end
		 * up sending out frames to unexpected destination. */
		wpa_printf(MSG_DEBUG, "%s: Invalid SA=" MACSTR
			   " in received indication - ignore this indication silently",
			   __func__, MAC2STR(addr));
		return 0;
	}

	random_add_randomness(addr, ETH_ALEN);

	hostapd_logger(hapd, addr, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_INFO, "associated");

	ieee802_11_parse_elems(req_ies, req_ies_len, &elems, 0);
	if (elems.wps_ie) {
		ie = elems.wps_ie - 2;
		ielen = elems.wps_ie_len + 2;
		wpa_printf(MSG_DEBUG, "STA included WPS IE in (Re)AssocReq");
	} else if (elems.rsn_ie) {
		ie = elems.rsn_ie - 2;
		ielen = elems.rsn_ie_len + 2;
		wpa_printf(MSG_DEBUG, "STA included RSN IE in (Re)AssocReq");
	} else if (elems.wpa_ie) {
		ie = elems.wpa_ie - 2;
		ielen = elems.wpa_ie_len + 2;
		wpa_printf(MSG_DEBUG, "STA included WPA IE in (Re)AssocReq");
#ifdef CONFIG_HS20
	} else if (elems.osen) {
		ie = elems.osen - 2;
		ielen = elems.osen_len + 2;
		wpa_printf(MSG_DEBUG, "STA included OSEN IE in (Re)AssocReq");
#endif /* CONFIG_HS20 */
	} else {
		ie = NULL;
		ielen = 0;
		wpa_printf(MSG_DEBUG,
			   "STA did not include WPS/RSN/WPA IE in (Re)AssocReq");
	}

	sta = ap_get_sta(hapd, addr);
	if (sta) {
		ap_sta_no_session_timeout(hapd, sta);
		accounting_sta_stop(hapd, sta);

		/*
		 * Make sure that the previously registered inactivity timer
		 * will not remove the STA immediately.
		 */
		sta->timeout_next = STA_NULLFUNC;
	} else {
		sta = ap_sta_add(hapd, addr);
		if (sta == NULL) {
			hostapd_drv_sta_disassoc(hapd, addr,
						 WLAN_REASON_DISASSOC_AP_BUSY);
			return -1;
		}
	}
	sta->flags &= ~(WLAN_STA_WPS | WLAN_STA_MAYBE_WPS | WLAN_STA_WPS2);

	/*
	 * ACL configurations to the drivers (implementing AP SME and ACL
	 * offload) without hostapd's knowledge, can result in a disconnection
	 * though the driver accepts the connection. Skip the hostapd check for
	 * ACL if the driver supports ACL offload to avoid potentially
	 * conflicting ACL rules.
	 */
	if (hapd->iface->drv_max_acl_mac_addrs == 0 &&
	    hostapd_check_acl(hapd, addr, NULL) != HOSTAPD_ACL_ACCEPT) {
		wpa_printf(MSG_INFO, "STA " MACSTR " not allowed to connect",
			   MAC2STR(addr));
		reason = WLAN_REASON_UNSPECIFIED;
		goto fail;
	}

#ifdef CONFIG_P2P
	if (elems.p2p) {
		wpabuf_free(sta->p2p_ie);
		sta->p2p_ie = ieee802_11_vendor_ie_concat(req_ies, req_ies_len,
							  P2P_IE_VENDOR_TYPE);
		if (sta->p2p_ie)
			p2p_dev_addr = p2p_get_go_dev_addr(sta->p2p_ie);
	}
#endif /* CONFIG_P2P */

#ifdef NEED_AP_MLME
	if (elems.ht_capabilities &&
	    (hapd->iface->conf->ht_capab &
	     HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET)) {
		struct ieee80211_ht_capabilities *ht_cap =
			(struct ieee80211_ht_capabilities *)
			elems.ht_capabilities;

		if (le_to_host16(ht_cap->ht_capabilities_info) &
		    HT_CAP_INFO_40MHZ_INTOLERANT)
			ht40_intolerant_add(hapd->iface, sta);
	}
#endif /* NEED_AP_MLME */

#ifdef CONFIG_INTERWORKING
	if (elems.ext_capab && elems.ext_capab_len > 4) {
		if (elems.ext_capab[4] & 0x01)
			sta->qos_map_enabled = 1;
	}
#endif /* CONFIG_INTERWORKING */

#ifdef CONFIG_HS20
	wpabuf_free(sta->hs20_ie);
	if (elems.hs20 && elems.hs20_len > 4) {
		sta->hs20_ie = wpabuf_alloc_copy(elems.hs20 + 4,
						 elems.hs20_len - 4);
	} else
		sta->hs20_ie = NULL;

	wpabuf_free(sta->roaming_consortium);
	if (elems.roaming_cons_sel)
		sta->roaming_consortium = wpabuf_alloc_copy(
			elems.roaming_cons_sel + 4,
			elems.roaming_cons_sel_len - 4);
	else
		sta->roaming_consortium = NULL;
#endif /* CONFIG_HS20 */

#ifdef CONFIG_FST
	wpabuf_free(sta->mb_ies);
	if (hapd->iface->fst)
		sta->mb_ies = mb_ies_by_info(&elems.mb_ies);
	else
		sta->mb_ies = NULL;
#endif /* CONFIG_FST */

	if (elems.ext_capab && elems.ext_capab_len > 2) {
		if (elems.ext_capab[2] & 0x08)
			sta->btm_supported = 1;
	}

	ap_copy_sta_supp_op_classes(sta, elems.supp_op_classes,
				    elems.supp_op_classes_len);

	if (hapd->conf->wpa) {
		if (ie == NULL || ielen == 0) {
#ifdef CONFIG_WPS
			if (hapd->conf->wps_state) {
				wpa_printf(MSG_DEBUG,
					   "STA did not include WPA/RSN IE in (Re)Association Request - possible WPS use");
				sta->flags |= WLAN_STA_MAYBE_WPS;
				goto skip_wpa_check;
			}
#endif /* CONFIG_WPS */

			wpa_printf(MSG_DEBUG, "No WPA/RSN IE from STA");
			reason = WLAN_REASON_INVALID_IE;
			status = WLAN_STATUS_INVALID_IE;
			goto fail;
		}
#ifdef CONFIG_WPS
		if (hapd->conf->wps_state && ie[0] == 0xdd && ie[1] >= 4 &&
		    os_memcmp(ie + 2, "\x00\x50\xf2\x04", 4) == 0) {
			struct wpabuf *wps;

			sta->flags |= WLAN_STA_WPS;
			wps = ieee802_11_vendor_ie_concat(ie, ielen,
							  WPS_IE_VENDOR_TYPE);
			if (wps) {
				if (wps_is_20(wps)) {
					wpa_printf(MSG_DEBUG,
						   "WPS: STA supports WPS 2.0");
					sta->flags |= WLAN_STA_WPS2;
				}
				wpabuf_free(wps);
			}
			goto skip_wpa_check;
		}
#endif /* CONFIG_WPS */

		if (sta->wpa_sm == NULL)
			sta->wpa_sm = wpa_auth_sta_init(hapd->wpa_auth,
							sta->addr,
							p2p_dev_addr);
		if (sta->wpa_sm == NULL) {
			wpa_printf(MSG_ERROR,
				   "Failed to initialize WPA state machine");
			return -1;
		}
		res = wpa_validate_wpa_ie(hapd->wpa_auth, sta->wpa_sm,
					  hapd->iface->freq,
					  ie, ielen,
					  elems.rsnxe ? elems.rsnxe - 2 : NULL,
					  elems.rsnxe ? elems.rsnxe_len + 2 : 0,
					  elems.mdie, elems.mdie_len,
					  elems.owe_dh, elems.owe_dh_len);
		reason = WLAN_REASON_INVALID_IE;
		status = WLAN_STATUS_INVALID_IE;
		switch (res) {
		case WPA_IE_OK:
			reason = WLAN_REASON_UNSPECIFIED;
			status = WLAN_STATUS_SUCCESS;
			break;
		case WPA_INVALID_IE:
			reason = WLAN_REASON_INVALID_IE;
			status = WLAN_STATUS_INVALID_IE;
			break;
		case WPA_INVALID_GROUP:
			reason = WLAN_REASON_GROUP_CIPHER_NOT_VALID;
			status = WLAN_STATUS_GROUP_CIPHER_NOT_VALID;
			break;
		case WPA_INVALID_PAIRWISE:
			reason = WLAN_REASON_PAIRWISE_CIPHER_NOT_VALID;
			status = WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID;
			break;
		case WPA_INVALID_AKMP:
			reason = WLAN_REASON_AKMP_NOT_VALID;
			status = WLAN_STATUS_AKMP_NOT_VALID;
			break;
		case WPA_NOT_ENABLED:
			reason = WLAN_REASON_INVALID_IE;
			status = WLAN_STATUS_INVALID_IE;
			break;
		case WPA_ALLOC_FAIL:
			reason = WLAN_REASON_UNSPECIFIED;
			status = WLAN_STATUS_UNSPECIFIED_FAILURE;
			break;
		case WPA_MGMT_FRAME_PROTECTION_VIOLATION:
			reason = WLAN_REASON_INVALID_IE;
			status = WLAN_STATUS_INVALID_IE;
			break;
		case WPA_INVALID_MGMT_GROUP_CIPHER:
			reason = WLAN_REASON_CIPHER_SUITE_REJECTED;
			status = WLAN_STATUS_CIPHER_REJECTED_PER_POLICY;
			break;
		case WPA_INVALID_MDIE:
			reason = WLAN_REASON_INVALID_MDE;
			status = WLAN_STATUS_INVALID_MDIE;
			break;
		case WPA_INVALID_PROTO:
			reason = WLAN_REASON_INVALID_IE;
			status = WLAN_STATUS_INVALID_IE;
			break;
		case WPA_INVALID_PMKID:
			reason = WLAN_REASON_INVALID_PMKID;
			status = WLAN_STATUS_INVALID_PMKID;
			break;
		case WPA_DENIED_OTHER_REASON:
			reason = WLAN_REASON_UNSPECIFIED;
			status = WLAN_STATUS_ASSOC_DENIED_UNSPEC;
			break;
		}
		if (status != WLAN_STATUS_SUCCESS) {
			wpa_printf(MSG_DEBUG,
				   "WPA/RSN information element rejected? (res %u)",
				   res);
			wpa_hexdump(MSG_DEBUG, "IE", ie, ielen);
			if (res == WPA_INVALID_GROUP) {
				reason = WLAN_REASON_GROUP_CIPHER_NOT_VALID;
				status = WLAN_STATUS_GROUP_CIPHER_NOT_VALID;
			} else if (res == WPA_INVALID_PAIRWISE) {
				reason = WLAN_REASON_PAIRWISE_CIPHER_NOT_VALID;
				status = WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID;
			} else if (res == WPA_INVALID_AKMP) {
				reason = WLAN_REASON_AKMP_NOT_VALID;
				status = WLAN_STATUS_AKMP_NOT_VALID;
			} else if (res == WPA_MGMT_FRAME_PROTECTION_VIOLATION) {
				reason = WLAN_REASON_INVALID_IE;
				status = WLAN_STATUS_INVALID_IE;
			} else if (res == WPA_INVALID_MGMT_GROUP_CIPHER) {
				reason = WLAN_REASON_CIPHER_SUITE_REJECTED;
				status = WLAN_STATUS_CIPHER_REJECTED_PER_POLICY;
			}
			else if (res == WPA_INVALID_PMKID) {
				reason = WLAN_REASON_INVALID_PMKID;
				status = WLAN_STATUS_INVALID_PMKID;
			} else {
				reason = WLAN_REASON_INVALID_IE;
				status = WLAN_STATUS_INVALID_IE;
			}
			goto fail;
		}

		if ((sta->flags & (WLAN_STA_ASSOC | WLAN_STA_MFP)) ==
		    (WLAN_STA_ASSOC | WLAN_STA_MFP) &&
		    !sta->sa_query_timed_out &&
		    sta->sa_query_count > 0)
			ap_check_sa_query_timeout(hapd, sta);
		if ((sta->flags & (WLAN_STA_ASSOC | WLAN_STA_MFP)) ==
		    (WLAN_STA_ASSOC | WLAN_STA_MFP) &&
		    !sta->sa_query_timed_out &&
		    (sta->auth_alg != WLAN_AUTH_FT)) {
			/*
			 * STA has already been associated with MFP and SA
			 * Query timeout has not been reached. Reject the
			 * association attempt temporarily and start SA Query,
			 * if one is not pending.
			 */

			if (sta->sa_query_count == 0)
				ap_sta_start_sa_query(hapd, sta);

			status = WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY;

			p = hostapd_eid_assoc_comeback_time(hapd, sta, p);

			hostapd_sta_assoc(hapd, addr, reassoc, status, buf,
					  p - buf);
			return 0;
		}

		if (wpa_auth_uses_mfp(sta->wpa_sm))
			sta->flags |= WLAN_STA_MFP;
		else
			sta->flags &= ~WLAN_STA_MFP;

#ifdef CONFIG_IEEE80211R_AP
		if (sta->auth_alg == WLAN_AUTH_FT) {
			status = wpa_ft_validate_reassoc(sta->wpa_sm, req_ies,
							 req_ies_len);
			if (status != WLAN_STATUS_SUCCESS) {
				if (status == WLAN_STATUS_INVALID_PMKID)
					reason = WLAN_REASON_INVALID_IE;
				if (status == WLAN_STATUS_INVALID_MDIE)
					reason = WLAN_REASON_INVALID_IE;
				if (status == WLAN_STATUS_INVALID_FTIE)
					reason = WLAN_REASON_INVALID_IE;
				goto fail;
			}
		}
#endif /* CONFIG_IEEE80211R_AP */
#ifdef CONFIG_SAE
		if (hapd->conf->sae_pwe == 2 &&
		    sta->auth_alg == WLAN_AUTH_SAE &&
		    sta->sae && !sta->sae->h2e &&
		    elems.rsnxe && elems.rsnxe_len >= 1 &&
		    (elems.rsnxe[0] & BIT(WLAN_RSNX_CAPAB_SAE_H2E))) {
			wpa_printf(MSG_INFO, "SAE: " MACSTR
				   " indicates support for SAE H2E, but did not use it",
				   MAC2STR(sta->addr));
			status = WLAN_STATUS_UNSPECIFIED_FAILURE;
			reason = WLAN_REASON_UNSPECIFIED;
			goto fail;
		}
#endif /* CONFIG_SAE */
	} else if (hapd->conf->wps_state) {
#ifdef CONFIG_WPS
		struct wpabuf *wps;

		if (req_ies)
			wps = ieee802_11_vendor_ie_concat(req_ies, req_ies_len,
							  WPS_IE_VENDOR_TYPE);
		else
			wps = NULL;
#ifdef CONFIG_WPS_STRICT
		if (wps && wps_validate_assoc_req(wps) < 0) {
			reason = WLAN_REASON_INVALID_IE;
			status = WLAN_STATUS_INVALID_IE;
			wpabuf_free(wps);
			goto fail;
		}
#endif /* CONFIG_WPS_STRICT */
		if (wps) {
			sta->flags |= WLAN_STA_WPS;
			if (wps_is_20(wps)) {
				wpa_printf(MSG_DEBUG,
					   "WPS: STA supports WPS 2.0");
				sta->flags |= WLAN_STA_WPS2;
			}
		} else
			sta->flags |= WLAN_STA_MAYBE_WPS;
		wpabuf_free(wps);
#endif /* CONFIG_WPS */
#ifdef CONFIG_HS20
	} else if (hapd->conf->osen) {
		if (elems.osen == NULL) {
			hostapd_logger(
				hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
				HOSTAPD_LEVEL_INFO,
				"No HS 2.0 OSEN element in association request");
			return WLAN_STATUS_INVALID_IE;
		}

		wpa_printf(MSG_DEBUG, "HS 2.0: OSEN association");
		if (sta->wpa_sm == NULL)
			sta->wpa_sm = wpa_auth_sta_init(hapd->wpa_auth,
							sta->addr, NULL);
		if (sta->wpa_sm == NULL) {
			wpa_printf(MSG_WARNING,
				   "Failed to initialize WPA state machine");
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
		}
		if (wpa_validate_osen(hapd->wpa_auth, sta->wpa_sm,
				      elems.osen - 2, elems.osen_len + 2) < 0)
			return WLAN_STATUS_INVALID_IE;
#endif /* CONFIG_HS20 */
	}

#ifdef CONFIG_MBO
	mbo_ap_check_sta_assoc(hapd, sta, &elems);

	if (hapd->conf->mbo_enabled && hapd->mbo_bss_assoc_disallow) {
		wpa_printf(MSG_INFO, "MBO: AP configure to reject associations");
		return WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA;
	}

	if (!hapd->conf->mbo_pmf_bypass)
		if (hapd->conf->mbo_enabled && (hapd->conf->wpa & 2) &&
			elems.mbo && sta->cell_capa && !(sta->flags & WLAN_STA_MFP) &&
			hapd->conf->ieee80211w != NO_MGMT_FRAME_PROTECTION) {
			wpa_printf(MSG_INFO,
				   "MBO: Reject WPA2 association without PMF");
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
		}
#endif /* CONFIG_MBO */

#ifdef CONFIG_WPS
skip_wpa_check:
#endif /* CONFIG_WPS */
	if (elems.multi_ap)
		sta->multi_ap_supported = 1;

	if ((elems.multi_ap) ||
		(elems.intel_ie_4addr_mode &&
			(elems.intel_ie_4addr_mode[VENDOR_INTEL_ATTRIBUTES_OFFSET] == VENDOR_INTEL_4ADDR_MODE_STA)) ||
		(hapd->conf->vendor_4addr && elems.brcom_ie &&
			(elems.brcom_ie[VENDOR_BRCOM_FLAGS1_OFFSET] & VENDOR_BRCOM_DWDS_CAPABLE))
		)
		sta->four_addr_mode_sta = 1;

	if (hapd->conf->mesh_mode == MESH_MODE_EXT_HYBRID ||
			hapd->conf->mesh_mode == MESH_MODE_BACKHAUL_AP)
		if (elems.brcom_ie && (elems.brcom_ie[VENDOR_BRCOM_FLAGS1_OFFSET] & VENDOR_BRCOM_DWDS_CAPABLE))
			sta->four_addr_mode_sta = 1;

#ifdef CONFIG_IEEE80211R_AP
	p = wpa_sm_write_assoc_resp_ies(sta->wpa_sm, buf, sizeof(buf),
					sta->auth_alg, req_ies, req_ies_len);
	if (!p) {
		wpa_printf(MSG_DEBUG, "FT: Failed to write AssocResp IEs");
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}
#endif /* CONFIG_IEEE80211R_AP */

#ifdef CONFIG_FILS
	if (sta->auth_alg == WLAN_AUTH_FILS_SK ||
	    sta->auth_alg == WLAN_AUTH_FILS_SK_PFS ||
	    sta->auth_alg == WLAN_AUTH_FILS_PK) {
		int delay_assoc = 0;

		if (!req_ies)
			return WLAN_STATUS_UNSPECIFIED_FAILURE;

		if (!wpa_fils_validate_fils_session(sta->wpa_sm, req_ies,
						    req_ies_len,
						    sta->fils_session)) {
			wpa_printf(MSG_DEBUG,
				   "FILS: Session validation failed");
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
		}

		res = wpa_fils_validate_key_confirm(sta->wpa_sm, req_ies,
						    req_ies_len);
		if (res < 0) {
			wpa_printf(MSG_DEBUG,
				   "FILS: Key Confirm validation failed");
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
		}

		if (fils_process_hlp(hapd, sta, req_ies, req_ies_len) > 0) {
			wpa_printf(MSG_DEBUG,
				   "FILS: Delaying Assoc Response (HLP)");
			delay_assoc = 1;
		} else {
			wpa_printf(MSG_DEBUG,
				   "FILS: Going ahead with Assoc Response (no HLP)");
		}

		if (sta) {
			wpa_printf(MSG_DEBUG, "FILS: HLP callback cleanup");
			eloop_cancel_timeout(fils_hlp_timeout, hapd, sta);
			os_free(sta->fils_pending_assoc_req);
			sta->fils_pending_assoc_req = NULL;
			sta->fils_pending_assoc_req_len = 0;
			wpabuf_free(sta->fils_hlp_resp);
			sta->fils_hlp_resp = NULL;
			sta->fils_drv_assoc_finish = 0;
		}

		if (sta && delay_assoc && status == WLAN_STATUS_SUCCESS) {
			u8 *req_tmp;

			req_tmp = os_malloc(req_ies_len);
			if (!req_tmp) {
				wpa_printf(MSG_DEBUG,
					   "FILS: buffer allocation failed for assoc req");
				goto fail;
			}
			os_memcpy(req_tmp, req_ies, req_ies_len);
			sta->fils_pending_assoc_req = req_tmp;
			sta->fils_pending_assoc_req_len = req_ies_len;
			sta->fils_pending_assoc_is_reassoc = reassoc;
			sta->fils_drv_assoc_finish = 1;
			wpa_printf(MSG_DEBUG,
				   "FILS: Waiting for HLP processing before sending (Re)Association Response frame to "
				   MACSTR, MAC2STR(sta->addr));
			eloop_register_timeout(
				0, hapd->conf->fils_hlp_wait_time * 1024,
				fils_hlp_timeout, hapd, sta);
			return 0;
		}
		p = hostapd_eid_assoc_fils_session(sta->wpa_sm, p,
						   elems.fils_session,
						   sta->fils_hlp_resp);
		wpa_hexdump(MSG_DEBUG, "FILS Assoc Resp BUF (IEs)",
			    buf, p - buf);
	}
#endif /* CONFIG_FILS */

#ifdef CONFIG_OWE
	if ((hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_OWE) &&
	    wpa_auth_sta_key_mgmt(sta->wpa_sm) == WPA_KEY_MGMT_OWE &&
	    elems.owe_dh) {
		u8 *npos;

		npos = owe_assoc_req_process(hapd, sta,
					     elems.owe_dh, elems.owe_dh_len,
					     p, sizeof(buf) - (p - buf),
					     &status);
		if (npos)
			p = npos;

		if (!npos &&
		    status == WLAN_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED) {
			hostapd_sta_assoc(hapd, addr, reassoc, status, buf,
					  p - buf);
			return 0;
		}

		if (!npos || status != WLAN_STATUS_SUCCESS)
			goto fail;
	}
#endif /* CONFIG_OWE */

#ifdef CONFIG_DPP2
		dpp_pfs_free(sta->dpp_pfs);
		sta->dpp_pfs = NULL;

		if ((hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_DPP) &&
		    hapd->conf->dpp_netaccesskey && sta->wpa_sm &&
		    wpa_auth_sta_key_mgmt(sta->wpa_sm) == WPA_KEY_MGMT_DPP &&
		    elems.owe_dh) {
			sta->dpp_pfs = dpp_pfs_init(
				wpabuf_head(hapd->conf->dpp_netaccesskey),
				wpabuf_len(hapd->conf->dpp_netaccesskey));
			if (!sta->dpp_pfs) {
				wpa_printf(MSG_DEBUG,
					   "DPP: Could not initialize PFS");
				/* Try to continue without PFS */
				goto pfs_fail;
			}

			if (dpp_pfs_process(sta->dpp_pfs, elems.owe_dh,
					    elems.owe_dh_len) < 0) {
				dpp_pfs_free(sta->dpp_pfs);
				sta->dpp_pfs = NULL;
				reason = WLAN_REASON_UNSPECIFIED;
				goto fail;
			}
		}

		wpa_auth_set_dpp_z(sta->wpa_sm, sta->dpp_pfs ?
				   sta->dpp_pfs->secret : NULL);
	pfs_fail:
#endif /* CONFIG_DPP2 */

	if (elems.rrm_enabled &&
	    elems.rrm_enabled_len >= sizeof(sta->rrm_enabled_capa))
	    os_memcpy(sta->rrm_enabled_capa, elems.rrm_enabled,
		      sizeof(sta->rrm_enabled_capa));

#if defined(CONFIG_IEEE80211R_AP) || defined(CONFIG_FILS) || defined(CONFIG_OWE)
	hostapd_sta_assoc(hapd, addr, reassoc, status, buf, p - buf);

	if (sta->auth_alg == WLAN_AUTH_FT ||
	    sta->auth_alg == WLAN_AUTH_FILS_SK ||
	    sta->auth_alg == WLAN_AUTH_FILS_SK_PFS ||
	    sta->auth_alg == WLAN_AUTH_FILS_PK)
		ap_sta_set_authorized(hapd, sta, 1);
#else /* CONFIG_IEEE80211R_AP || CONFIG_FILS */
	/* Keep compiler silent about unused variables */
	if (status) {
	}
#endif /* CONFIG_IEEE80211R_AP || CONFIG_FILS */

	new_assoc = (sta->flags & WLAN_STA_ASSOC) == 0;
	sta->flags |= WLAN_STA_AUTH | WLAN_STA_ASSOC;
	sta->flags &= ~WLAN_STA_WNM_SLEEP_MODE;

	hostapd_set_sta_flags(hapd, sta);

	if (reassoc && (sta->auth_alg == WLAN_AUTH_FT))
		wpa_auth_sm_event(sta->wpa_sm, WPA_ASSOC_FT);
#ifdef CONFIG_FILS
	else if (sta->auth_alg == WLAN_AUTH_FILS_SK ||
		 sta->auth_alg == WLAN_AUTH_FILS_SK_PFS ||
		 sta->auth_alg == WLAN_AUTH_FILS_PK)
		wpa_auth_sm_event(sta->wpa_sm, WPA_ASSOC_FILS);
#endif /* CONFIG_FILS */
	else
		wpa_auth_sm_event(sta->wpa_sm, WPA_ASSOC);

	hostapd_new_assoc_sta(hapd, sta, !new_assoc);

	ieee802_1x_notify_port_enabled(sta->eapol_sm, 1);

#ifdef CONFIG_P2P
	if (req_ies) {
		p2p_group_notif_assoc(hapd->p2p_group, sta->addr,
				      req_ies, req_ies_len);
	}
#endif /* CONFIG_P2P */

	return 0;

fail:
#ifdef CONFIG_IEEE80211R_AP
	hostapd_sta_assoc(hapd, addr, reassoc, status, buf, p - buf);
#endif /* CONFIG_IEEE80211R_AP */
	hostapd_drv_sta_disassoc(hapd, sta->addr, reason);
	ap_free_sta(hapd, sta);
	return -1;
}


void hostapd_notif_disassoc(struct hostapd_data *hapd, const u8 *addr)
{
	struct sta_info *sta;

	if (addr == NULL) {
		/*
		 * This could potentially happen with unexpected event from the
		 * driver wrapper. This was seen at least in one case where the
		 * driver ended up reporting a station mode event while hostapd
		 * was running, so better make sure we stop processing such an
		 * event here.
		 */
		wpa_printf(MSG_DEBUG,
			   "hostapd_notif_disassoc: Skip event with no address");
		return;
	}

	hostapd_logger(hapd, addr, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_INFO, "disassociated");

	sta = ap_get_sta(hapd, addr);
	if (sta == NULL) {
		wpa_printf(MSG_DEBUG,
			   "Disassociation notification for unknown STA "
			   MACSTR, MAC2STR(addr));
		return;
	}

	sta->flags &= ~WLAN_STA_BSS_TRANS_MGMT_REQ_TIMEOUT;
	/* until required details is made available driver,
	 * set reason and source values to unspecified and local respectively */
	sta->disassoc_reason = WLAN_REASON_UNSPECIFIED;
	sta->disconnect_source = DISCONNECT_SOURCE_LOCAL;
	ap_sta_set_authorized(hapd, sta, 0);
	sta->flags &= ~(WLAN_STA_AUTH | WLAN_STA_ASSOC);
	hostapd_set_sta_flags(hapd, sta);
	wpa_auth_sm_event(sta->wpa_sm, WPA_DISASSOC);
	sta->acct_terminate_cause = RADIUS_ACCT_TERMINATE_CAUSE_USER_REQUEST;
	ieee802_1x_notify_port_enabled(sta->eapol_sm, 0);
	ap_free_sta(hapd, sta);
}

void hostapd_event_sta_low_ack(struct hostapd_data *hapd, const u8 *addr)
{
	struct sta_info *sta;
	struct hostapd_iface *iface = hapd->iface;
	size_t i;

	for (i = 0; i < iface->num_bss; i++) {
		sta = ap_get_sta(iface->bss[i], addr);
		if (sta) {
			hapd = iface->bss[i];
			break;
		}
	}

	if (!sta || !hapd->conf->disassoc_low_ack || sta->agreed_to_steer)
		return;

	hostapd_logger(hapd, addr, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_INFO,
		       "disconnected due to excessive missing ACKs");
	hostapd_drv_sta_disassoc(hapd, addr, WLAN_REASON_DISASSOC_LOW_ACK);
	ap_sta_disassociate(hapd, sta, WLAN_REASON_DISASSOC_LOW_ACK);
}


void hostapd_event_sta_opmode_changed(struct hostapd_data *hapd, const u8 *addr,
				      enum smps_mode smps_mode,
				      enum chan_width chan_width, u8 rx_nss)
{
	struct sta_info *sta = ap_get_sta(hapd, addr);
	const char *txt;

	if (!sta)
		return;

	switch (smps_mode) {
	case SMPS_AUTOMATIC:
		txt = "automatic";
		break;
	case SMPS_OFF:
		txt = "off";
		break;
	case SMPS_DYNAMIC:
		txt = "dynamic";
		break;
	case SMPS_STATIC:
		txt = "static";
		break;
	default:
		txt = NULL;
		break;
	}
	if (txt) {
		wpa_msg(hapd->msg_ctx, MSG_INFO, STA_OPMODE_SMPS_MODE_CHANGED
			MACSTR " %s", MAC2STR(addr), txt);
	}

	switch (chan_width) {
	case CHAN_WIDTH_20_NOHT:
		txt = "20(no-HT)";
		break;
	case CHAN_WIDTH_20:
		txt = "20";
		break;
	case CHAN_WIDTH_40:
		txt = "40";
		break;
	case CHAN_WIDTH_80:
		txt = "80";
		break;
	case CHAN_WIDTH_80P80:
		txt = "80+80";
		break;
	case CHAN_WIDTH_160:
		txt = "160";
		break;
	default:
		txt = NULL;
		break;
	}
	if (txt) {
		wpa_msg(hapd->msg_ctx, MSG_INFO, STA_OPMODE_MAX_BW_CHANGED
			MACSTR " %s", MAC2STR(addr), txt);
	}

	if (rx_nss != 0xff) {
		wpa_msg(hapd->msg_ctx, MSG_INFO, STA_OPMODE_N_SS_CHANGED
			MACSTR " %d", MAC2STR(addr), rx_nss);
	}
}

static char *width2str(int width)
{
  switch(width) {
  case CHAN_WIDTH_80:
    return "80";
  case CHAN_WIDTH_80P80:
    return "80+80";
  case CHAN_WIDTH_160:
    return "160";
  case CHAN_WIDTH_20_NOHT:
  case CHAN_WIDTH_20:
    return "20";
  case CHAN_WIDTH_40:
  default:
    return "40";
  }
}


void hostapd_event_ch_switch(struct hostapd_data *hapd, int freq, int ht,
			     int offset, int width, int cf1, int cf2,
			     int finished)
{
	/* TODO: If OCV is enabled deauth STAs that don't perform a SA Query */

#ifdef NEED_AP_MLME
	int channel, chwidth, is_dfs;
	u8 seg0_idx = 0, seg1_idx = 0;
	size_t i;

	hostapd_logger(hapd, NULL, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_INFO,
		       "driver %s channel switch: freq=%d, ht=%d, vht_ch=0x%x, offset=%d, width=%d (%s), cf1=%d, cf2=%d",
		       finished ? "had" : "starting",
		       freq, ht, hapd->iconf->ch_switch_vht_config, offset,
		       width, channel_width_to_string(width), cf1, cf2);

	if (!hapd->iface->current_mode) {
		hostapd_logger(hapd, NULL, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_WARNING,
			       "ignore channel switch since the interface is not yet ready");
		return;
	}

	hapd->iface->freq = freq;

	channel = hostapd_hw_get_channel(hapd, freq);
	if (!channel) {
		hostapd_logger(hapd, NULL, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_WARNING,
			       "driver switched to bad channel!");
		return;
	}

	switch (width) {
	case CHAN_WIDTH_80:
		chwidth = CHANWIDTH_80MHZ;
		break;
	case CHAN_WIDTH_80P80:
		chwidth = CHANWIDTH_80P80MHZ;
		break;
	case CHAN_WIDTH_160:
		chwidth = CHANWIDTH_160MHZ;
		break;
	case CHAN_WIDTH_20_NOHT:
	case CHAN_WIDTH_20:
	case CHAN_WIDTH_40:
	default:
		chwidth = CHANWIDTH_USE_HT;
		break;
	}

	switch (hapd->iface->current_mode->mode) {
	case HOSTAPD_MODE_IEEE80211A:
		if (cf1 > 5000)
			seg0_idx = (cf1 - 5000) / 5;
		if (cf2 > 5000)
			seg1_idx = (cf2 - 5000) / 5;
		break;
	default:
		ieee80211_freq_to_chan(cf1, &seg0_idx);
		ieee80211_freq_to_chan(cf2, &seg1_idx);
		break;
	}

	hapd->iconf->channel = channel;
	hapd->iconf->ieee80211n = ht;
	if (!ht)
		hapd->iconf->ieee80211ac = 0;
	else if (ht && hapd->iface->conf->orig_ieee80211ac)
		hapd->iconf->ieee80211ac = 1;
	else if (hapd->iconf->ch_switch_vht_config) {
		/* CHAN_SWITCH VHT config */
		if (hapd->iconf->ch_switch_vht_config &
		    CH_SWITCH_VHT_ENABLED)
			hapd->iconf->ieee80211ac = 1;
		else if (hapd->iconf->ch_switch_vht_config &
			 CH_SWITCH_VHT_DISABLED)
			hapd->iconf->ieee80211ac = 0;
	}
	hapd->iconf->ch_switch_vht_config = 0;
	hapd->iconf->secondary_channel = offset;
	hostapd_set_oper_chwidth(hapd->iconf,chwidth);
	hostapd_set_oper_centr_freq_seg0_idx(hapd->iconf,seg0_idx);
	hostapd_set_oper_centr_freq_seg1_idx(hapd->iconf,seg1_idx);

	is_dfs = ieee80211_is_dfs(freq, hapd->iface->hw_features,
				  hapd->iface->num_hw_features);

	/* For radar simulation */
	if (hapd->iface->conf->dfs_debug_chan) {
		int i;
		struct hostapd_channel_data *ch = NULL;
		for (i = 0; i < hapd->iface->current_mode->num_channels; i++) {
			ch = &hapd->iface->current_mode->channels[i];
			if (ch->freq == freq)
				break;
		}

		/* Need to Do CAC if driver switched to DFS channel and it is not available*/
		if (ch && hapd->iface->conf->dfs_dbg_need_cac) {
			int width_mhz = channel_width_to_int(width);
			int n_chans = width_mhz / 20;
			int low_freq = cf1 - (width_mhz / 2) + 10;
			wpa_printf(MSG_INFO, "DFS_DEBUG: Starting CAC on DFS channel %d", channel);
			hostapd_cleanup_cs_params(hapd);
			hostapd_dfs_debug_set_chan_dfs_state(hapd->iface, HOSTAPD_CHAN_DFS_USABLE, low_freq, n_chans);
			hostapd_setup_interface_complete(hapd->iface, 0);
			hapd->iface->conf->dfs_dbg_need_cac = false;
			return;
		}
	}

	wpa_msg(hapd->msg_ctx, MSG_INFO,
		"%sfreq=%d ht_enabled=%d ch_offset=%d ch_width=%s cf1=%d cf2=%d dfs=%d",
		finished ? WPA_EVENT_CHANNEL_SWITCH :
		WPA_EVENT_CHANNEL_SWITCH_STARTED,
		freq, ht, offset, channel_width_to_string(width),
		cf1, cf2, is_dfs);
	if (!finished)
		return;

	if ((hapd->csa_in_progress && freq == hapd->cs_freq_params.freq) ||
	    (hapd->iface->drv_flags & WPA_DRIVER_FLAGS_DFS_OFFLOAD)) {

		if (hapd->csa_in_progress && freq == hapd->cs_freq_params.freq)
			hostapd_cleanup_cs_params(hapd);

		wpa_msg(hapd->msg_ctx, MSG_INFO, AP_CSA_FINISHED "freq=%d Channel=%d "
				"OperatingChannelBandwidt=%s ExtensionChannel=%d cf1=%d "
				"cf2=%d reason=%s dfs_chan=%d",
				freq, channel, width2str(width), offset, cf1, cf2,
				hostapd_channel_switch_text(hapd->chan_switch_reason),
				is_dfs);

		hapd->chan_switch_reason = HAPD_CHAN_SWITCH_OTHER;
	}

	for (i = 0; i < hapd->iface->num_bss; i++) {
		hostapd_neighbor_set_own_report(hapd->iface->bss[i]);
		hostapd_neighbor_set_colocated_6ghz_report(hapd->iface->bss[i]);
	}
	/* set_beacon needs to be called when AP csa is in progress, and also when STA role
	 * interface initiated the ch switch following Peer AP CSA (the AP the sta is
	 * connected to is switching channel and AP beacons need to be changed */
	ieee802_11_set_beacon(hapd);

#endif /* NEED_AP_MLME */
}


void hostapd_event_connect_failed_reason(struct hostapd_data *hapd,
					 const u8 *addr, int reason_code)
{
	switch (reason_code) {
	case MAX_CLIENT_REACHED:
		wpa_msg(hapd->msg_ctx, MSG_INFO, AP_REJECTED_MAX_STA MACSTR,
			MAC2STR(addr));
		break;
	case BLOCKED_CLIENT:
		wpa_msg(hapd->msg_ctx, MSG_INFO, AP_REJECTED_BLOCKED_STA MACSTR,
			MAC2STR(addr));
		break;
	}
}


#ifdef CONFIG_ACS
void hostapd_acs_channel_selected(struct hostapd_data *hapd,
				  struct acs_selected_channels *acs_res)
{
	int ret, i;
	int err = 0;
	struct hostapd_channel_data *pri_chan;

	if (hapd->iconf->channel) {
		wpa_printf(MSG_INFO, "ACS: Channel was already set to %d",
			   hapd->iconf->channel);
		return;
	}

	hapd->iface->freq = acs_res->pri_freq;

	if (!hapd->iface->current_mode) {
		for (i = 0; i < hapd->iface->num_hw_features; i++) {
			struct hostapd_hw_modes *mode =
				&hapd->iface->hw_features[i];

			if (mode->mode == acs_res->hw_mode) {
				if (hapd->iface->freq > 0 &&
				    !hw_get_chan(mode->mode,
						 hapd->iface->freq,
						 hapd->iface->hw_features,
						 hapd->iface->num_hw_features))
					continue;
				hapd->iface->current_mode = mode;
				break;
			}
		}
		if (!hapd->iface->current_mode) {
			hostapd_logger(hapd, NULL, HOSTAPD_MODULE_IEEE80211,
				       HOSTAPD_LEVEL_WARNING,
				       "driver selected to bad hw_mode");
			err = 1;
			goto out;
		}
	}

	if (!acs_res->pri_freq) {
		hostapd_logger(hapd, NULL, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_WARNING,
			       "driver switched to bad channel");
		err = 1;
		goto out;
	}
	pri_chan = hw_get_channel_freq(hapd->iface->current_mode->mode,
				       acs_res->pri_freq, NULL,
				       hapd->iface->hw_features,
				       hapd->iface->num_hw_features);
	if (!pri_chan) {
		wpa_printf(MSG_ERROR,
			   "ACS: Could not determine primary channel number from pri_freq %u",
			   acs_res->pri_freq);
		err = 1;
		goto out;
	}

	hapd->iconf->channel = pri_chan->chan;
	hapd->iconf->acs = 1;

	if (acs_res->sec_freq == 0)
		hapd->iconf->secondary_channel = 0;
	else if (acs_res->sec_freq < acs_res->pri_freq)
		hapd->iconf->secondary_channel = -1;
	else if (acs_res->sec_freq > acs_res->pri_freq)
		hapd->iconf->secondary_channel = 1;
	else {
		wpa_printf(MSG_ERROR, "Invalid secondary channel!");
		err = 1;
		goto out;
	}

	if (hapd->iface->conf->ieee80211ac || hapd->iface->conf->ieee80211ax) {
		/* set defaults for backwards compatibility */
		hostapd_set_oper_centr_freq_seg1_idx(hapd->iconf, 0);
		hostapd_set_oper_centr_freq_seg0_idx(hapd->iconf, 0);
		hostapd_set_oper_chwidth(hapd->iconf, CHANWIDTH_USE_HT);
		if (acs_res->ch_width == 80) {
			hostapd_set_oper_centr_freq_seg0_idx(hapd->iconf, acs_res->vht_seg0_center_ch);
			hostapd_set_oper_chwidth(hapd->iconf, CHANWIDTH_80MHZ);

		} else if (acs_res->ch_width == 160) {
			if (acs_res->vht_seg1_center_ch == 0) {
				hostapd_set_oper_centr_freq_seg0_idx(hapd->iconf, acs_res->vht_seg0_center_ch);
				hostapd_set_oper_chwidth(hapd->iconf, CHANWIDTH_160MHZ);
			} else {
				hostapd_set_oper_centr_freq_seg0_idx(hapd->iconf, acs_res->vht_seg0_center_ch);
				hostapd_set_oper_centr_freq_seg1_idx(hapd->iconf, acs_res->vht_seg1_center_ch);
				hostapd_set_oper_chwidth(hapd->iconf, CHANWIDTH_80P80MHZ);
			}
		}
	}

out:
	ret = hostapd_acs_completed(hapd->iface, err, 1);
	if (ret) {
		wpa_printf(MSG_ERROR,
			   "ACS: Possibly channel configuration is invalid");
	}
}
#endif /* CONFIG_ACS */


int hostapd_probe_req_rx(struct hostapd_data *hapd, const u8 *sa, const u8 *da,
			 const u8 *bssid, const u8 *ie, size_t ie_len,
			 int ssi_signal)
{
	size_t i;
	int ret = 0;

	if (sa == NULL || ie == NULL)
		return -1;

	random_add_randomness(sa, ETH_ALEN);
	for (i = 0; hapd->probereq_cb && i < hapd->num_probereq_cb; i++) {
		if (hapd->probereq_cb[i].cb(hapd->probereq_cb[i].ctx,
					    sa, da, bssid, ie, ie_len,
					    ssi_signal) > 0) {
			ret = 1;
			break;
		}
	}
	return ret;
}


#ifdef HOSTAPD

#ifdef CONFIG_IEEE80211R_AP
static void hostapd_notify_auth_ft_finish(void *ctx, const u8 *dst,
					  const u8 *bssid,
					  u16 auth_transaction, u16 status,
					  const u8 *ies, size_t ies_len)
{
	struct hostapd_data *hapd = ctx;
	struct sta_info *sta;

	sta = ap_get_sta(hapd, dst);
	if (sta == NULL)
		return;

	hostapd_logger(hapd, dst, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_DEBUG, "authentication OK (FT)");
	sta->flags |= WLAN_STA_AUTH;

	hostapd_sta_auth(hapd, dst, auth_transaction, status, ies, ies_len);
}
#endif /* CONFIG_IEEE80211R_AP */


#ifdef CONFIG_FILS
static void hostapd_notify_auth_fils_finish(struct hostapd_data *hapd,
					    struct sta_info *sta, u16 resp,
					    struct wpabuf *data, int pub)
{
	if (resp == WLAN_STATUS_SUCCESS) {
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_DEBUG, "authentication OK (FILS)");
		sta->flags |= WLAN_STA_AUTH;
		wpa_auth_sm_event(sta->wpa_sm, WPA_AUTH);
		sta->auth_alg = WLAN_AUTH_FILS_SK;
		mlme_authenticate_indication(hapd, sta);
	} else {
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_DEBUG,
			       "authentication failed (FILS)");
	}

	hostapd_sta_auth(hapd, sta->addr, 2, resp,
			 data ? wpabuf_head(data) : NULL,
			 data ? wpabuf_len(data) : 0);
	wpabuf_free(data);
}
#endif /* CONFIG_FILS */


static void hostapd_notif_auth(struct hostapd_data *hapd,
			       struct auth_info *rx_auth)
{
	struct sta_info *sta;
	u16 status = WLAN_STATUS_SUCCESS;
	u8 resp_ies[2 + WLAN_AUTH_CHALLENGE_LEN];
	size_t resp_ies_len = 0;

	sta = ap_get_sta(hapd, rx_auth->peer);
	if (!sta) {
		sta = ap_sta_add(hapd, rx_auth->peer);
		if (sta == NULL) {
			status = WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA;
			goto fail;
		}
	}
	sta->flags &= ~WLAN_STA_PREAUTH;
	ieee802_1x_notify_pre_auth(sta->eapol_sm, 0);
#ifdef CONFIG_IEEE80211R_AP
	if (rx_auth->auth_type == WLAN_AUTH_FT && hapd->wpa_auth) {
		sta->auth_alg = WLAN_AUTH_FT;
		if (sta->wpa_sm == NULL)
			sta->wpa_sm = wpa_auth_sta_init(hapd->wpa_auth,
							sta->addr, NULL);
		if (sta->wpa_sm == NULL) {
			wpa_printf(MSG_DEBUG,
				   "FT: Failed to initialize WPA state machine");
			status = WLAN_STATUS_UNSPECIFIED_FAILURE;
			goto fail;
		}
		wpa_ft_process_auth(sta->wpa_sm, rx_auth->bssid,
				    rx_auth->auth_transaction, rx_auth->ies,
				    rx_auth->ies_len,
				    hostapd_notify_auth_ft_finish, hapd);
		return;
	}
#endif /* CONFIG_IEEE80211R_AP */

#ifdef CONFIG_FILS
	if (rx_auth->auth_type == WLAN_AUTH_FILS_SK) {
		sta->auth_alg = WLAN_AUTH_FILS_SK;
		handle_auth_fils(hapd, sta, rx_auth->ies, rx_auth->ies_len,
				 rx_auth->auth_type, rx_auth->auth_transaction,
				 rx_auth->status_code,
				 hostapd_notify_auth_fils_finish);
		return;
	}
#endif /* CONFIG_FILS */

fail:
	hostapd_sta_auth(hapd, rx_auth->peer, rx_auth->auth_transaction + 1,
			 status, resp_ies, resp_ies_len);
}


#ifndef NEED_AP_MLME
static void hostapd_action_rx(struct hostapd_data *hapd,
			      struct rx_mgmt *drv_mgmt)
{
	struct ieee80211_mgmt *mgmt;
	struct sta_info *sta;
	size_t plen __maybe_unused;
	u16 fc;
	u8 *action __maybe_unused;

	if (drv_mgmt->frame_len < IEEE80211_HDRLEN + 2 + 1)
		return;

	plen = drv_mgmt->frame_len - IEEE80211_HDRLEN;

	mgmt = (struct ieee80211_mgmt *) drv_mgmt->frame;
	fc = le_to_host16(mgmt->frame_control);
	if (WLAN_FC_GET_STYPE(fc) != WLAN_FC_STYPE_ACTION)
		return; /* handled by the driver */

	action = (u8 *) &mgmt->u.action.u;
	wpa_printf(MSG_DEBUG, "RX_ACTION category %u action %u sa " MACSTR
		   " da " MACSTR " plen %d",
		   mgmt->u.action.category, *action,
		   MAC2STR(mgmt->sa), MAC2STR(mgmt->da), (int) plen);

	sta = ap_get_sta(hapd, mgmt->sa);
	if (sta == NULL) {
		wpa_printf(MSG_DEBUG, "%s: station not found", __func__);
		return;
	}
#ifdef CONFIG_IEEE80211R_AP
	if (mgmt->u.action.category == WLAN_ACTION_FT) {
		wpa_ft_action_rx(sta->wpa_sm, (u8 *) &mgmt->u.action, plen);
		return;
	}
#endif /* CONFIG_IEEE80211R_AP */
	if (mgmt->u.action.category == WLAN_ACTION_SA_QUERY) {
		ieee802_11_sa_query_action(hapd, mgmt, drv_mgmt->frame_len);
		return;
	}
#ifdef CONFIG_WNM_AP
	if (mgmt->u.action.category == WLAN_ACTION_WNM) {
		ieee802_11_rx_wnm_action_ap(hapd, mgmt, drv_mgmt->frame_len);
		return;
	}
#endif /* CONFIG_WNM_AP */
#ifdef CONFIG_FST
	if (mgmt->u.action.category == WLAN_ACTION_FST && hapd->iface->fst) {
		fst_rx_action(hapd->iface->fst, mgmt, drv_mgmt->frame_len);
		return;
	}
#endif /* CONFIG_FST */
#ifdef CONFIG_DPP
	if (plen >= 2 + 4 &&
	    mgmt->u.action.u.vs_public_action.action ==
	    WLAN_PA_VENDOR_SPECIFIC &&
	    WPA_GET_BE24(mgmt->u.action.u.vs_public_action.oui) ==
	    OUI_WFA &&
	    mgmt->u.action.u.vs_public_action.variable[0] ==
	    DPP_OUI_TYPE) {
		const u8 *pos, *end;

		pos = mgmt->u.action.u.vs_public_action.oui;
		end = drv_mgmt->frame + drv_mgmt->frame_len;
		hostapd_dpp_rx_action(hapd, mgmt->sa, pos, end - pos,
				      drv_mgmt->freq);
		return;
	}
#endif /* CONFIG_DPP */
}
#endif /* NEED_AP_MLME */


#ifdef NEED_AP_MLME

#define HAPD_BROADCAST ((struct hostapd_data *) -1)

static struct hostapd_data * get_hapd_bssid(struct hostapd_iface *iface,
					    const u8 *bssid)
{
	size_t i;

	if (bssid == NULL)
		return NULL;
	if (bssid[0] == 0xff && bssid[1] == 0xff && bssid[2] == 0xff &&
	    bssid[3] == 0xff && bssid[4] == 0xff && bssid[5] == 0xff)
		return HAPD_BROADCAST;

	for (i = 0; i < iface->num_bss; i++) {
		if (os_memcmp(bssid, iface->bss[i]->own_addr, ETH_ALEN) == 0)
			return iface->bss[i];
	}

	return NULL;
}


static void hostapd_rx_from_unknown_sta(struct hostapd_data *hapd,
					const u8 *bssid, const u8 *addr,
					int wds)
{
	hapd = get_hapd_bssid(hapd->iface, bssid);
	if (hapd == NULL || hapd == HAPD_BROADCAST)
		return;

	ieee802_11_rx_from_unknown(hapd, addr, wds);
}


static int hostapd_mgmt_rx(struct hostapd_data *hapd, struct rx_mgmt *rx_mgmt)
{
	struct hostapd_iface *iface = hapd->iface;
	const struct ieee80211_hdr *hdr;
	const u8 *bssid;
	struct hostapd_frame_info fi;
	int ret;
	errno_t err;

#ifdef CONFIG_TESTING_OPTIONS
	if (hapd->ext_mgmt_frame_handling) {
		size_t hex_len = 2 * rx_mgmt->frame_len + 1;
		char *hex = os_malloc(hex_len);

		if (hex) {
			wpa_snprintf_hex(hex, hex_len, rx_mgmt->frame,
					 rx_mgmt->frame_len);
			wpa_msg(hapd->msg_ctx, MSG_INFO, "MGMT-RX %s", hex);
			os_free(hex);
		}
		return 1;
	}
#endif /* CONFIG_TESTING_OPTIONS */

	hdr = (const struct ieee80211_hdr *) rx_mgmt->frame;
	bssid = get_hdr_bssid(hdr, rx_mgmt->frame_len);
	if (bssid == NULL)
		return 0;

	hapd = get_hapd_bssid(iface, bssid);
	if (hapd == NULL) {
		u16 fc = le_to_host16(hdr->frame_control);

		/*
		 * Drop frames to unknown BSSIDs except for Beacon frames which
		 * could be used to update neighbor information.
		 */
		if (WLAN_FC_GET_TYPE(fc) == WLAN_FC_TYPE_MGMT &&
		    WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_BEACON)
			hapd = iface->bss[0];
		else
#ifdef CONFIG_WDS_WPA
		{
			/* set BSSID from destination address */
			struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *) rx_mgmt->frame;

			bssid = mgmt->da;
			hapd = get_hapd_bssid(iface, bssid);

			if ((hapd == NULL) || (hapd == HAPD_BROADCAST)) {
				return 0;
			}

			if (!hostapd_maclist_found(hapd->conf->wds_wpa_sta,
							hapd->conf->num_wds_wpa_sta, mgmt->sa, NULL))
			return 0;

			/* Replace BSSID for WDS mode */
			err = memcpy_s(mgmt->bssid, sizeof(mgmt->bssid), mgmt->da, ETH_ALEN);
			if (EOK != err) {
				wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
				return 0;
			}
		}
#else
		return 0;
#endif
	}

	os_memset(&fi, 0, sizeof(fi));
	fi.freq = rx_mgmt->freq;
	fi.datarate = rx_mgmt->datarate;
	fi.ssi_signal = rx_mgmt->ssi_signal;
	fi.snr_db = rx_mgmt->snr_db; 

	if (hapd == HAPD_BROADCAST) {
		size_t i;

		ret = 0;
		for (i = 0; i < iface->num_bss; i++) {
			/* if bss is set, driver will call this function for
			 * each bss individually. */
			if (rx_mgmt->drv_priv &&
			    (iface->bss[i]->drv_priv != rx_mgmt->drv_priv))
				continue;

			if (ieee802_11_mgmt(iface->bss[i], rx_mgmt->frame,
					    rx_mgmt->frame_len, &fi) > 0)
				ret = 1;
		}
	} else
		ret = ieee802_11_mgmt(hapd, rx_mgmt->frame, rx_mgmt->frame_len,
				      &fi);

	random_add_randomness(&fi, sizeof(fi));

	return ret;
}


static void hostapd_mgmt_tx_cb(struct hostapd_data *hapd, const u8 *buf,
			       size_t len, u16 stype, int ok)
{
	struct ieee80211_hdr *hdr;
	struct hostapd_data *orig_hapd = hapd;

	hdr = (struct ieee80211_hdr *) buf;
	hapd = get_hapd_bssid(hapd->iface, get_hdr_bssid(hdr, len));
	if (!hapd)
		return;
	if (hapd == HAPD_BROADCAST) {
		if (stype != WLAN_FC_STYPE_ACTION || len <= 25 ||
		    buf[24] != WLAN_ACTION_PUBLIC)
			return;
		hapd = get_hapd_bssid(orig_hapd->iface, hdr->addr2);
		if (!hapd || hapd == HAPD_BROADCAST)
			return;
		/*
		 * Allow processing of TX status for a Public Action frame that
		 * used wildcard BBSID.
		 */
	}
	ieee802_11_mgmt_cb(hapd, buf, len, stype, ok);
}

#endif /* NEED_AP_MLME */

static void hostapd_event_ltq_flush_stations_rx(struct hostapd_data *hapd,
               const u8 *data, size_t data_len)
{
  struct hostapd_iface *iface = hapd->iface;
  s32 flush;

  /* Discard event if interface didn't finished initialization.
   * This may happen when driver sends irrelevant events due to station mode actions
   */
  if (iface->state != HAPD_IFACE_ENABLED) {
    wpa_printf(MSG_DEBUG, "discard driver event (%s) - interface not initialized yet", __FUNCTION__);
    return;
  }

  if (!data || (data_len != sizeof(s32)))
    flush = LTQ_FLUSH_RADIO;
  else
    flush = *((s32*)data);
  hostapd_ltq_clear_old(iface, flush);
}

static int hostapd_event_new_sta(struct hostapd_data *hapd, const u8 *addr)
{
	struct sta_info *sta = ap_get_sta(hapd, addr);

	if (sta)
		return 0;

	wpa_printf(MSG_DEBUG, "Data frame from unknown STA " MACSTR
		   " - adding a new STA", MAC2STR(addr));
	sta = ap_sta_add(hapd, addr);
	if (sta) {
		hostapd_new_assoc_sta(hapd, sta, 0);
	} else {
		wpa_printf(MSG_DEBUG, "Failed to add STA entry for " MACSTR,
			   MAC2STR(addr));
		return -1;
	}

	return 0;
}


static void hostapd_event_eapol_rx(struct hostapd_data *hapd, const u8 *src,
				   const u8 *data, size_t data_len)
{
	struct hostapd_iface *iface = hapd->iface;
	struct sta_info *sta;
	size_t j;

	for (j = 0; j < iface->num_bss; j++) {
		sta = ap_get_sta(iface->bss[j], src);
		if (sta && sta->flags & WLAN_STA_ASSOC) {
			hapd = iface->bss[j];
			break;
		}
	}

	ieee802_1x_receive(hapd, src, data, data_len);
}

#endif /* HOSTAPD */

static void hostapd_event_ltq_coc(struct hostapd_data *hapd,
		struct intel_vendor_event_coc *data, size_t data_len)
{
	struct hostapd_iface *iface = hapd->iface;

	/* Discard event if interface didn't finished initialization.
	 * This may happen when driver sends irrelevant events due to station mode actions
	 */
	if (iface->state == HAPD_IFACE_UNINITIALIZED ||
			iface->state == HAPD_IFACE_DISABLED ||
			iface->state == HAPD_IFACE_COUNTRY_UPDATE ) {
		wpa_printf(MSG_DEBUG, "discard driver event (%s) - interface not ready yet", __FUNCTION__);
		return;
	}

	hostapd_ltq_update_coc(iface, data, data_len);
}

#ifdef CONFIG_ACS
static void hostapd_event_ltq_chan_data(struct hostapd_data *hapd,
					const u8 *data, size_t data_len)
{
	struct hostapd_iface *iface = hapd->iface;

	/* Discard event if interface didn't finished initialization.
	 * This may happen when driver sends irrelevant events due to station mode actions
	 */
	if (iface->state == HAPD_IFACE_UNINITIALIZED ||
	    iface->state == HAPD_IFACE_DISABLED ||
	    iface->state == HAPD_IFACE_COUNTRY_UPDATE ) {
		wpa_printf(MSG_DEBUG, "discard driver event (%s) - interface not ready yet", __FUNCTION__);
		return;
	}

	hostapd_ltq_update_channel_data(iface, data, data_len);
}
#endif

static void hostapd_event_ltq_unconnected_sta_rx(struct hostapd_data *hapd,
  struct intel_vendor_unconnected_sta *unc_sta)
{
  int ret;
  char buf[30];

  /* Discard event if interface didn't finished initialization.
   * This may happen when driver sends irrelevant events due to station mode actions
   */
  if (hapd->iface->state != HAPD_IFACE_ENABLED) {
    wpa_printf(MSG_DEBUG, "discard driver event (%s) - interface not initialized yet", __FUNCTION__);
    return;
  }

  ret = sprintf_s(buf, sizeof(buf), MACSTR, MAC2STR(unc_sta->addr));
  if (ret <= 0) {
    wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
    return;
  }
  wpa_msg(hapd->msg_ctx, MSG_INFO,
    UNCONNECTED_STA_RSSI "%s rx_bytes=%llu rx_packets=%u "
    "rssi=%d %d %d %d SNR=%d %d %d %d rate=%d",
    buf, unc_sta->rx_bytes, unc_sta->rx_packets,
    unc_sta->rssi[0], unc_sta->rssi[1], unc_sta->rssi[2], unc_sta->rssi[3],
    unc_sta->noise[0] ? unc_sta->rssi[0] - unc_sta->noise[0] : 0, /* RSSI and noise to SNR */
    unc_sta->noise[1] ? unc_sta->rssi[1] - unc_sta->noise[1] : 0, /* RSSI and noise to SNR */
    unc_sta->noise[2] ? unc_sta->rssi[2] - unc_sta->noise[2] : 0, /* RSSI and noise to SNR */
    unc_sta->noise[3] ? unc_sta->rssi[3] - unc_sta->noise[3] : 0, /* RSSI and noise to SNR */
    unc_sta->rate);
}

static u16 check_wmm_wds(struct hostapd_data *hapd, struct sta_info *sta,
		const u8 *wmm_ie, size_t wmm_ie_len)
{
	sta->flags &= ~WLAN_STA_WMM;
	sta->qosinfo = 0;
	if (wmm_ie && hapd->conf->wmm_enabled) {
		struct wmm_information_element *wmm;

		if (!hostapd_eid_wmm_valid_wds(hapd, wmm_ie, wmm_ie_len)) {
			hostapd_logger(hapd, sta->addr,
			HOSTAPD_MODULE_WPA, HOSTAPD_LEVEL_DEBUG,
					"invalid WMM element in association "
							"request");
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
		}

		sta->flags |= WLAN_STA_WMM;
		wmm = (struct wmm_information_element *) wmm_ie;
		sta->qosinfo = wmm->qos_info;
	}
	return WLAN_STATUS_SUCCESS;
}

static u16 check_assoc_ies_wds(struct hostapd_data *hapd, struct sta_info *sta,
		const u8 *ies, size_t ies_len, int reassoc)
{
	struct ieee802_11_elems elems;
	u16 resp;

	if (ieee802_11_parse_elems(ies, ies_len, &elems, 1) == ParseFailed) {
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
				HOSTAPD_LEVEL_INFO, "Station sent an invalid "
						"association request");
		return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}
#if 1 /*FIXME: open question if wmm parameters should be kept */
	resp = check_wmm_wds(hapd, sta, elems.wmm, elems.wmm_len);
	if (resp != WLAN_STATUS_SUCCESS)
		return resp;
#endif
	resp = check_ext_capab(hapd, sta, elems.ext_capab, elems.ext_capab_len);
	if (resp != WLAN_STATUS_SUCCESS)
		return resp;
	resp = copy_supp_rates(hapd, sta, &elems);
	if (resp != WLAN_STATUS_SUCCESS)
		return resp;
	resp = copy_sta_ht_capab(hapd, sta, elems.ht_capabilities);
	if (resp != WLAN_STATUS_SUCCESS)
	return resp;
	if (hapd->iconf->ieee80211n && hapd->iconf->require_ht &&
			!(sta->flags & WLAN_STA_HT)) {
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
				HOSTAPD_LEVEL_INFO, "Station does not support "
				"mandatory HT PHY - reject association");
		return WLAN_STATUS_ASSOC_DENIED_NO_HT;
	}
#ifdef CONFIG_IEEE80211AC
	if (hapd->iconf->channel > 14) {
		resp = copy_sta_vht_capab(hapd, sta, elems.vht_capabilities);
		if (resp != WLAN_STATUS_SUCCESS)
		return resp;
		resp = set_sta_vht_opmode(hapd, sta, elems.vht_opmode_notif);
		if (resp != WLAN_STATUS_SUCCESS)
		return resp;
		if (hapd->iconf->ieee80211ac && hapd->iconf->require_vht &&
				!(sta->flags & WLAN_STA_VHT)) {
			hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
					HOSTAPD_LEVEL_INFO, "Station does not support "
					"mandatory VHT PHY - reject association");
			return WLAN_STATUS_ASSOC_DENIED_NO_VHT;
		}
	}
#endif /* CONFIG_IEEE80211AC */

#ifdef CONFIG_IEEE80211AX
		resp = copy_sta_he_capab(hapd, sta, elems.pc_he_capabilities, elems.he_capabilities_len_from_sta);
		if (resp != WLAN_STATUS_SUCCESS)
						return resp;
		resp = copy_sta_he_operation(hapd, sta, elems.pc_he_operation, elems.he_operation_len_from_sta);
		if (resp != WLAN_STATUS_SUCCESS)
						return resp;
		if (is_6ghz_op_class(hapd->iconf->op_class)) {
			resp = copy_sta_he_6ghz_capab(hapd, sta, elems.he_6ghz_band_cap);
			if (resp != WLAN_STATUS_SUCCESS)
				return resp;
			resp = check_6ghz_assoc_valid(sta);
			if (resp != WLAN_STATUS_SUCCESS)
				return resp;
		}
		if (hapd->iconf->ieee80211ax && hapd->iconf->require_he &&
				!(sta->flags & WLAN_STA_HE)) {
			hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
					HOSTAPD_LEVEL_INFO, "Station does not support "
					"mandatory HE PHY - reject association");
			return WLAN_STATUS_DENIED_HE_NOT_SUPPORTED;
		}
#endif /* CONFIG_IEEE80211AX */
	wpa_auth_sta_no_wpa(sta->wpa_sm);

	return WLAN_STATUS_SUCCESS;
}

static void hostapd_event_ltq_wds_connect_rx(struct hostapd_data *hapd,
		const u8 *data, size_t data_len)
{
	struct sta_info *sta;
	struct intel_vendor_wds_sta_info *wds_sta = (struct intel_vendor_wds_sta_info *) data;
	struct ieee80211_ht_capabilities ht_cap;
	struct ieee80211_vht_capabilities vht_cap;
	struct ieee80211_he_capabilities he_cap;
	u16 resp, i, capab_info = 0;
	int new_assoc = 1;
	u8 *ext_capab = NULL;
	errno_t err;

	/* Discard event if interface didn't finished initialization.
	 * This may happen when driver sends irrelevant events due to station mode actions
	 */
	if (hapd->iface->state != HAPD_IFACE_ENABLED) {
	  wpa_printf(MSG_DEBUG, "discard driver event (%s) - interface not initialized yet", __FUNCTION__);
	  return;
	}

	if (!data) {
		wpa_printf(MSG_ERROR, "NULL hostapd_wds_sta_connect data received");
		return;
	}

	if (data_len != sizeof(*wds_sta) + wds_sta->assoc_req_ies_len) {
		wpa_printf(MSG_ERROR, "Wrong hostapd_wds_sta_connect data length");
		return;
	}

	hapd = get_bss_index(wds_sta->ifname, hapd->iface);
	if (hapd == NULL) {
		wpa_printf(MSG_ERROR, "Hostapd BSS with name %s not found", wds_sta->ifname);
		return;
	}

	wpa_printf(MSG_DEBUG, "hostapd_wds_sta_connect " MACSTR,
			MAC2STR(wds_sta->mac_addr));

	sta = ap_sta_add(hapd, wds_sta->mac_addr);
	if (!sta) {
		wpa_printf(MSG_ERROR, "Unable to handle new sta");
		hostapd_drv_sta_remove(hapd, wds_sta->mac_addr);
		return;
	}
	sta->flags |= WLAN_STA_AUTH;
	wpa_printf(MSG_INFO, "RX max_rssi 0x%08x(%d)", wds_sta->max_rssi,
			wds_sta->max_rssi);
	sta->ssi_signal = wds_sta->max_rssi;

	resp = check_assoc_ies_wds(hapd, sta, wds_sta->assoc_req_ies,
			wds_sta->assoc_req_ies_len, 0);
	if (resp != WLAN_STATUS_SUCCESS) {
		wpa_printf(MSG_ERROR, "check_assoc_ies_wds() failed");
		hostapd_drv_sta_remove(hapd, sta->addr);
		return;
	}

	if (hostapd_get_aid(hapd, sta) < 0) {
		hostapd_logger(hapd, wds_sta->mac_addr, HOSTAPD_MODULE_IEEE80211,
				HOSTAPD_LEVEL_INFO, "No room for more AIDs");
		hostapd_drv_sta_remove(hapd, sta->addr);
		return;
	}

	if (wds_sta->short_preamble)
		capab_info |= WLAN_CAPABILITY_SHORT_PREAMBLE;
	if (wds_sta->short_slot_time)
		capab_info |= WLAN_CAPABILITY_SHORT_SLOT_TIME;
	sta->capability = capab_info;

	if (hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211G)
		sta->flags |= WLAN_STA_NONERP;
	for (i = 0; i < sta->supported_rates_len; i++) {
		if ((sta->supported_rates[i] & 0x7f) > 22) {
			sta->flags &= ~WLAN_STA_NONERP;
			break;
		}
	}
	if (sta->flags & WLAN_STA_NONERP && !sta->nonerp_set) {
		sta->nonerp_set = 1;
		hapd->iface->num_sta_non_erp++;
		if (hapd->iface->num_sta_non_erp == 1)
			ieee802_11_update_beacons(hapd->iface);
	}

	if (!(sta->capability & WLAN_CAPABILITY_SHORT_SLOT_TIME)
			&& !sta->no_short_slot_time_set) {
		sta->no_short_slot_time_set = 1;
		hapd->iface->num_sta_no_short_slot_time++;
		if (hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211G
				&& hapd->iface->num_sta_no_short_slot_time == 1)
			ieee802_11_update_beacons(hapd->iface);
	}

	if (sta->capability & WLAN_CAPABILITY_SHORT_PREAMBLE)
		sta->flags |= WLAN_STA_SHORT_PREAMBLE;
	else
		sta->flags &= ~WLAN_STA_SHORT_PREAMBLE;

	if (!(sta->capability & WLAN_CAPABILITY_SHORT_PREAMBLE)
			&& !sta->no_short_preamble_set) {
		sta->no_short_preamble_set = 1;
		hapd->iface->num_sta_no_short_preamble++;
		if (hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211G
				&& hapd->iface->num_sta_no_short_preamble == 1)
			ieee802_11_update_beacons(hapd->iface);
	}

	if (update_ht_state(hapd, sta) > 0)
	    ieee802_11_update_beacons(hapd->iface);

	hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			HOSTAPD_LEVEL_DEBUG, "association OK (aid %d)", sta->aid);

	/* Stop previous accounting session, if one is started, and allocate
	 * new session id for the new session. */
	accounting_sta_stop(hapd, sta);

	hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			HOSTAPD_LEVEL_INFO, "associated (aid %d)", sta->aid);

	sta->flags |= WLAN_STA_ASSOC_REQ_OK;
	if (sta->flags & WLAN_STA_ASSOC)
		new_assoc = 0;
	sta->flags |= WLAN_STA_ASSOC;
	sta->flags &= ~WLAN_STA_WNM_SLEEP_MODE;

#if 0
	/*
	 * Remove the STA entry in order to make sure the STA PS state gets
	 * cleared and configuration gets updated in case of reassociation back
	 * to the same AP.
	 */
	hostapd_drv_sta_remove(hapd, sta->addr);
#endif
	if (sta->flags & WLAN_STA_HT)
	hostapd_get_ht_capab(hapd, sta->ht_capabilities, &ht_cap);
#ifdef CONFIG_IEEE80211AC
	if (sta->flags & WLAN_STA_VHT)
	hostapd_get_vht_capab(hapd, sta->vht_capabilities, &vht_cap);
#endif /* CONFIG_IEEE80211AC */

#ifdef CONFIG_IEEE80211AX
	if (sta->flags & WLAN_STA_HE) {
		hostapd_get_he_capab(hapd, sta->he_capabilities, &he_cap, sta->he_capabilities_len_from_sta);
	}
#endif /* CONFIG_IEEE80211AX */
	if (sta->ext_capability) {
		ext_capab = os_malloc(1 + sta->ext_capability[0]);
		if (ext_capab) {
			err = memcpy_s(ext_capab, 1 + sta->ext_capability[0],
				 sta->ext_capability, 1 + sta->ext_capability[0]);
			if (EOK != err) {
				wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
				os_free(ext_capab);
				return;
			}
		}
	}
	if (hostapd_sta_add(hapd, sta->addr, sta->aid, sta->capability,
			sta->supported_rates, sta->supported_rates_len,
			    sta->listen_interval,
			    sta->flags & WLAN_STA_HT ? &ht_cap : NULL,
			sta->flags & WLAN_STA_VHT ? &vht_cap : NULL,
#ifdef CONFIG_IEEE80211AX
			    sta->flags & WLAN_STA_HE ? &he_cap : NULL,
#endif /* CONFIG_IEEE80211AX */
			sta->flags | WLAN_STA_AUTHORIZED, sta->qosinfo, sta->vht_opmode,
#ifdef CONFIG_IEEE80211AX
			    sta->he_operation,
#endif /* CONFIG_IEEE80211AX */
			sta->p2p_ie ? 1 : 0, sta->added_unassoc, sta->last_assoc_req,
			sta->last_assoc_req_len, sta->ssi_signal, ext_capab, sta->he_6ghz_capab)) {
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_NOTICE,
			       "Could not add STA to kernel driver");
		ap_free_sta(hapd, sta);
		os_free(ext_capab);
		return;
	}

	hostapd_set_sta_flags(hapd, sta);
	hapd->new_assoc_sta_cb(hapd, sta, !new_assoc);
	ieee802_1x_notify_port_enabled(sta->eapol_sm, 1);
	hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			HOSTAPD_LEVEL_INFO, "hostapd_wds_sta_connect: WDS STA connected");
	os_free(ext_capab);

}

static void hostapd_event_ltq_wds_disconnect_rx(struct hostapd_data *hapd,
		const u8 *data, size_t data_len)
{
	struct intel_vendor_wds_sta_info *wds_sta = (struct intel_vendor_wds_sta_info *) data;

	/* Discard event if interface didn't finished initialization.
	 * This may happen when driver sends irrelevant events due to station mode actions
	 */
	if (hapd->iface->state != HAPD_IFACE_ENABLED) {
	  wpa_printf(MSG_DEBUG, "discard driver event (%s) - interface not initialized yet", __FUNCTION__);
	  return;
	}

	if (!data) {
		wpa_printf(MSG_ERROR, "NULL hostapd_wds_sta_disconnect data received");
		return;
	}

	if (data_len != sizeof(*wds_sta) + wds_sta->assoc_req_ies_len) {
		wpa_printf(MSG_ERROR, "Wrong hostapd_wds_sta_disconnect data length");
		return;
	}

	hapd = get_bss_index(wds_sta->ifname, hapd->iface);
	if (hapd == NULL) {
		wpa_printf(MSG_ERROR, "Hostapd BSS with name %s not found", wds_sta->ifname);
		return;
	}

	hostapd_notif_disassoc(hapd, wds_sta->mac_addr);
}

static void hostapd_event_ltq_radar(struct hostapd_data *hapd,
	struct intel_vendor_radar *radar)
{
	/* Discard event if interface didn't finished initialization.
	 * This may happen when driver sends irrelevant events due to station mode actions
	 */
	if (hapd->iface->state != HAPD_IFACE_ENABLED) {
		wpa_printf(MSG_DEBUG, "discard driver event (%s) - interface not initialized yet", __FUNCTION__);
		return;
	}
	wpa_msg(hapd->msg_ctx, MSG_INFO, LTQ_RADAR
		"freq=%d chan_width=%d cf1=%d cf2=%d radar_bit_map=0x%02X",
		radar->center_freq, radar->width, radar->center_freq1, radar->center_freq2,
		radar->radar_bit_map);
}

static void hostapd_event_iface_bridge_config(struct hostapd_data *hapd,
	struct iface_bridge_config *br_conf)
{
	int i;
	struct backhaul_virtual_iface * bkh_vrt_iface;
	char in_br[IFNAMSIZ];
	int fd;

	if (hapd->conf->mesh_mode != MESH_MODE_BACKHAUL_AP &&
	    hapd->conf->mesh_mode != MESH_MODE_EXT_HYBRID)
		return;

	if (!hapd->conf->bap_vifs_follow_bridge)
		return;

	if (NULL == hapd->bkh_vrt_ifaces)
		return;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		wpa_printf(MSG_ERROR, "%s: socket(AF_INET,SOCK_STREAM) failed: %s", __func__, strerror(errno));
		return;
	}

	wpa_printf(MSG_DEBUG, "main iface:%s bridge:%s mode is changed to:%d", hapd->conf->iface, br_conf->bridge_name, br_conf->enabled);

	for (i = 0; i < hapd->num_bkh_vrt_ifaces; i++) {
		bkh_vrt_iface = hapd->bkh_vrt_ifaces[i];
		if (NULL == bkh_vrt_iface) {
			wpa_printf(MSG_ERROR, "Virtual interface is null when it should be set");
			goto out;
		}

		if (br_conf->enabled) {
			/* Remove from bridge if it is already in the another bridge */
			if (linux_br_get(in_br, bkh_vrt_iface->ifname) == 0) {
				if (os_strcmp(in_br, br_conf->bridge_name) == 0) {
					bkh_vrt_iface->added_to_bridge = 1;
					wpa_printf(MSG_DEBUG, "iface:%s already in bridge:%s", bkh_vrt_iface->ifname, br_conf->bridge_name);
					continue; /* already in the targed bridge */
				}

				if (linux_br_del_if(fd, in_br, bkh_vrt_iface->ifname) < 0) {
					wpa_printf(MSG_ERROR, "nl80211: Failed to "
							"remove interface %s from bridge "
							"%s: %s", bkh_vrt_iface->ifname, br_conf->bridge_name, strerror(errno));
					continue;
				}
				bkh_vrt_iface->added_to_bridge = 0;
				wpa_printf(MSG_DEBUG, "iface:%s removed from bridge:%s", bkh_vrt_iface->ifname, in_br);
			}

			if (linux_br_add_if(fd, br_conf->bridge_name, bkh_vrt_iface->ifname) < 0) {
				wpa_printf(MSG_ERROR, "nl80211: Failed to add interface %s "
					"into bridge %s: %s", bkh_vrt_iface->ifname, br_conf->bridge_name,
					strerror(errno));
				continue;
			}
			bkh_vrt_iface->added_to_bridge = 1;
			wpa_printf(MSG_DEBUG, "iface:%s added to bridge:%s", bkh_vrt_iface->ifname, br_conf->bridge_name);
		} else {
			/* remove interface from bridge */
			if (linux_br_get(in_br, bkh_vrt_iface->ifname) == 0) {
				if (os_strcmp(in_br, br_conf->bridge_name) == 0) {
					if (linux_br_del_if(fd, br_conf->bridge_name, bkh_vrt_iface->ifname) < 0) {
						wpa_printf(MSG_ERROR, "nl80211: Failed to remove interface %s from bridge "
							"%s: %s", bkh_vrt_iface->ifname, br_conf->bridge_name, strerror(errno));
						continue;
					}
					bkh_vrt_iface->added_to_bridge = 0;
					wpa_printf(MSG_DEBUG, "iface:%s removed from bridge:%s", bkh_vrt_iface->ifname, br_conf->bridge_name);
					continue;
				}
			}
			wpa_printf(MSG_DEBUG, "iface:%s already not in the bridge:%s", bkh_vrt_iface->ifname, br_conf->bridge_name);
		}
	}
out:
	close(fd);
	return;
}

void hostapd_event_ltq_report_event(struct hostapd_data *hapd,
		u8 msgtype, u8 event, u8 *macaddr, u8 fail)
{
	struct hostapd_iface *iface = hapd->iface;
	u8 report_event = event;

	/* Discard event if interface didn't finished initialization.
	 * This may happen when driver sends irrelevant events due to station mode actions
	 */
	if (iface->state != HAPD_IFACE_ENABLED) {
		wpa_printf(MSG_DEBUG, "discard driver event (%s) - interface not initialized yet", __FUNCTION__);
		return;
	}

	switch(msgtype) {
		case WLAN_FC_STYPE_AUTH:
			if (fail) {
				if (event == WLAN_STATUS_UNSPECIFIED_FAILURE) {
					report_event = HOSTAPD_EVENT_AUTH_FAIL_UNKNOWN_REASON;
				} else if (event == WLAN_STATUS_UNKNOWN_PASSWORD_IDENTIFIER) {
					report_event = HOSTAPD_EVENT_AUTH_FAIL_WRONG_PASSWORD;
				}
			}
			break;
		case WLAN_MSG_TYPE_EAPOL:
			if (event == WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT) {
				report_event = HOSTAPD_EVENT_AUTH_FAIL_TIMEOUT;
			}
			break;
		default:
			break;
	}

	wpa_msg(hapd->msg_ctx, MSG_INFO, LTQ_REPORT_EVENT
		" VAP=%s VAP_BSSID="MACSTR" msg_type=%d addr="MACSTR" fail=%d event=%d",
		hapd->conf->iface, MAC2STR(hapd->conf->bssid), msgtype,
		MAC2STR(macaddr), fail, report_event);
}

static void hostapd_drv_event_ltq_softblock_drop(struct hostapd_data *hapd,
		struct intel_vendor_event_msg_drop *msg_dropped)
{
	if (hapd->iface->state != HAPD_IFACE_ENABLED) {
		wpa_printf(MSG_DEBUG, "discard driver event (%s) - interface not initialized yet", __FUNCTION__);
		return;
	}

	hapd = get_hapd_bssid(hapd->iface, msg_dropped->bssid);
	if (hapd == NULL || hapd == HAPD_BROADCAST) {
		wpa_printf(MSG_ERROR, "SoftBlock event from unknown BSSID="MACSTR, MAC2STR(msg_dropped->bssid));
		return;
	}

	hostapd_event_ltq_softblock_drop(hapd, msg_dropped);
}

void ap_event_hash_add(struct hostapd_data *hapd, struct event_info *e)
{
	e->hnext = hapd->event_hash[STA_HASH(e->addr)];
	hapd->event_hash[STA_HASH(e->addr)] = e;
}

static bool is_same_softblock_event(char const *msg_str1, char const *msg_str2)
{
	char *pos1, *pos2;

	/* compare event strings excluding ' snr=%d' */
	pos1 = os_strstr(msg_str1, " snr=");
	pos2 = os_strstr(msg_str2, " snr=");
	if (!pos1 || !pos2){
		wpa_printf(MSG_ERROR, "%s event comparison error", __FUNCTION__);
		return false;
	}
	if ((pos1 - msg_str1) != (pos2 - msg_str2))
		return false;

	if (os_strncmp(msg_str1, msg_str2, pos1 - msg_str1) != 0)
		return false;

	pos1 = os_strstr(pos1 + 5, " ");
	pos2 = os_strstr(pos2 + 5, " ");

	if (!pos1 || !pos2){
		wpa_printf(MSG_ERROR, "%s event format error", __FUNCTION__);
		return false;
	}

	if (os_strncmp(pos1, pos2, MAX_EVENT_MSG_LEN) != 0)
		return false;

	return true;
}


struct event_info * ap_find_event(struct hostapd_data *hapd, const u8 *sta_mac,
				  const char *msg_str)
{
	struct event_info *e;

	e = hapd->event_hash[STA_HASH(sta_mac)];
	while (e != NULL && os_memcmp(e->addr, sta_mac, 6) != 0){
		if (is_same_softblock_event(msg_str, e->msg_string))
			break;
		e = e->hnext;
	}
	return e;
}

static void ap_event_hash_del(struct hostapd_data *hapd, struct event_info *event)
{
	struct event_info *e;

	e = hapd->event_hash[STA_HASH(event->addr)];
	if (e == NULL) return;
	if (os_memcmp(e->addr, event->addr, ETH_ALEN) == 0) {
		if (is_same_softblock_event(event->msg_string, e->msg_string)){
			hapd->event_hash[STA_HASH(event->addr)] = e->hnext;
			return;
		}
	}

	/* Find the mac address related to the event */
	while (e->hnext != NULL &&
	       (os_memcmp(e->hnext->addr, event->addr, ETH_ALEN) != 0))
		e = e->hnext;

	/* Find the message */
	while (e->hnext != NULL &&
	       (is_same_softblock_event(e->hnext->msg_string, event->msg_string)))
		e = e->hnext;

	if (e->hnext != NULL)
		e->hnext = e->hnext->hnext;
	else
		wpa_printf(MSG_DEBUG, "AP: could not remove event for mac " MACSTR
			   " from hash table", MAC2STR(event->addr));
}

static void ap_event_list_del(struct hostapd_data *hapd, struct event_info *event)
{
	struct event_info *tmp;

	if (hapd->event_list == event) {
		hapd->event_list = event->next;
		return;
	}

	tmp = hapd->event_list;
	while (tmp != NULL && tmp->next != event)
		tmp = tmp->next;
	if (tmp == NULL) {
		wpa_printf(MSG_DEBUG, "Could not remove event for mac " MACSTR " from "
			   "list.", MAC2STR(event->addr));
	} else
		tmp->next = event->next;
}

static void remove_event_from_cache(struct hostapd_data *hapd, struct event_info *event)
{

	if (event == NULL){
		wpa_printf(MSG_ERROR, "%s event no longer exists in cache", __FUNCTION__);
		return;
	}

	ap_event_hash_del(hapd, event);
	ap_event_list_del(hapd, event);
	os_free(event);
	hapd->cached_event_count--;
}

static void handle_cached_event_expired(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	struct event_info *event = timeout_ctx;

	remove_event_from_cache(hapd, event);
}

void hostapd_free_cached_events(struct hostapd_data *hapd)
{
	struct event_info *event, *prev;

	event = hapd->event_list;

	while (event) {
		prev = event;
		event = event->next;

		wpa_printf(MSG_DEBUG, "Removing event for mac " MACSTR,
			   MAC2STR(prev->addr));

		if (eloop_is_timeout_registered(handle_cached_event_expired, hapd, prev))
			eloop_cancel_timeout(handle_cached_event_expired, hapd, prev);

		remove_event_from_cache(hapd, prev);
	}
}

void send_event_buffered (struct hostapd_data *hapd, const u8 addr[6],
		const char *msg_str)
{
	struct event_info *event;

	/* Send event and keep it in cache to make sure future
	 * events are not sent again during the set interval.
	 */
	event = ap_find_event(hapd, addr, msg_str);
	if (event)
		wpa_printf(MSG_DEBUG,
		   "Event already exists in cache for " MACSTR
		   " event will not be sent." , MAC2STR(addr));
	else {
		/* new event, send and add to event cache */
		wpa_msg(hapd->msg_ctx, MSG_INFO, "%s", msg_str);

		if (hapd->cached_event_count >= MAX_CACHED_EVENT_COUNT){
			wpa_printf(MSG_ERROR, "Maximum number of cached events reached");
			return;
		}

		event = os_zalloc(sizeof(struct event_info));
		if (event == NULL) {
			wpa_printf(MSG_ERROR, "malloc failed");
			return;
		}

		if (EOK != strncpy_s(event->msg_string, MAX_EVENT_MSG_LEN, msg_str, MAX_EVENT_MSG_LEN)){
			wpa_printf(MSG_ERROR, "%s Adding a new event to cache failed, strncpy_s returned an error", __FUNCTION__);
			os_free(event);
			return;
		}

		if (EOK != memcpy_s(event->addr, ETH_ALEN, addr, ETH_ALEN)){
			wpa_printf(MSG_ERROR, "%s Adding a new event to cache failed, memcpy_s returned an error", __FUNCTION__);
			os_free(event);
			return;
		}

		event->next = hapd->event_list;
		hapd->event_list = event;
		ap_event_hash_add(hapd, event);
		hapd->cached_event_count++;

		eloop_register_timeout(hapd->iconf->event_cache_interval, 0,
				       handle_cached_event_expired, hapd, event);
	}
}


void hostapd_event_ltq_softblock_drop(struct hostapd_data *hapd,
		struct intel_vendor_event_msg_drop *msg_dropped)
{
	struct hostapd_iface *iface = hapd->iface;
	char msg_str[MAX_EVENT_MSG_LEN];
	int ret;

	/* Discard event if interface didn't finished initialization.
	 * This may happen when driver sends irrelevant events due to station mode actions
	 */
	if (iface->state != HAPD_IFACE_ENABLED) {
		wpa_printf(MSG_DEBUG, "discard driver event (%s) - interface not initialized yet", __FUNCTION__);
		return;
	}

	ret = sprintf_s (msg_str, MAX_EVENT_MSG_LEN, LTQ_SOFTBLOCK_DROP
		" VAP=%s VAP_BSSID="MACSTR" msg_type=%d addr="MACSTR" snr=%d blocked=%d rejected=%d "
		"broadcast=%d reason=%d", hapd->conf->iface, MAC2STR(hapd->conf->bssid), msg_dropped->msgtype,
		MAC2STR(msg_dropped->addr), msg_dropped->rx_snr, msg_dropped->blocked,
		msg_dropped->rejected, msg_dropped->broadcast, msg_dropped->reason);
	if (ret <= 0){
		wpa_printf(MSG_ERROR, "%s event message format error", __FUNCTION__);
		return;
	}

	if (hapd->iconf->event_cache_interval == 0)
		wpa_msg(hapd->msg_ctx, MSG_INFO, "%s", msg_str);
	else
		send_event_buffered(hapd, msg_dropped->addr, msg_str);
}

struct hostapd_channel_data * hostapd_get_mode_channel(
	struct hostapd_iface *iface, unsigned int freq)
{
	int i;
	struct hostapd_channel_data *chan;

	for (i = 0; i < iface->current_mode->num_channels; i++) {
		chan = &iface->current_mode->channels[i];
		if ((unsigned int) chan->freq == freq)
			return chan;
	}

	return NULL;
}


u16 hostapd_get_beacon_int(const struct hostapd_data *hapd)
{
	return hapd->conf->beacon_int ? hapd->conf->beacon_int :
					hapd->iconf->beacon_int;
}


static void hostapd_update_nf(struct hostapd_iface *iface,
			      struct hostapd_channel_data *chan,
			      struct freq_survey *survey)
{
	if (!iface->chans_surveyed) {
		chan->min_nf = survey->nf;
		iface->lowest_nf = survey->nf;
	} else {
		if (dl_list_empty(&chan->survey_list))
			chan->min_nf = survey->nf;
		else if (survey->nf < chan->min_nf)
			chan->min_nf = survey->nf;
		if (survey->nf < iface->lowest_nf)
			iface->lowest_nf = survey->nf;
	}
}


static void hostapd_single_channel_get_survey(struct hostapd_iface *iface,
					      struct survey_results *survey_res)
{
	struct hostapd_channel_data *chan;
	struct freq_survey *survey;
	u64 divisor, dividend;

	survey = dl_list_first(&survey_res->survey_list, struct freq_survey,
			       list);
	if (!survey || !survey->freq)
		return;

	chan = hostapd_get_mode_channel(iface, survey->freq);
	if (!chan || chan->flag & HOSTAPD_CHAN_DISABLED)
		return;

	wpa_printf(MSG_DEBUG,
		   "Single Channel Survey: (freq=%d channel_time=%ld channel_time_busy=%ld)",
		   survey->freq,
		   (unsigned long int) survey->channel_time,
		   (unsigned long int) survey->channel_time_busy);

	if (survey->channel_time > iface->last_channel_time &&
	    survey->channel_time > survey->channel_time_busy) {
		dividend = survey->channel_time_busy -
			iface->last_channel_time_busy;
		divisor = survey->channel_time - iface->last_channel_time;

		iface->channel_utilization = dividend * 255 / divisor;
		wpa_printf(MSG_DEBUG, "Channel Utilization: %d",
			   iface->channel_utilization);
	}
	iface->last_channel_time = survey->channel_time;
	iface->last_channel_time_busy = survey->channel_time_busy;
}


void hostapd_event_get_survey(struct hostapd_iface *iface,
			      struct survey_results *survey_results)
{
	struct freq_survey *survey, *tmp;
	struct hostapd_channel_data *chan;

	if (dl_list_empty(&survey_results->survey_list)) {
		wpa_printf(MSG_DEBUG, "No survey data received");
		return;
	}

	if (survey_results->freq_filter) {
		hostapd_single_channel_get_survey(iface, survey_results);
		return;
	}

	dl_list_for_each_safe(survey, tmp, &survey_results->survey_list,
			      struct freq_survey, list) {
		chan = hostapd_get_mode_channel(iface, survey->freq);
		if (!chan)
			continue;
		if (chan->flag & HOSTAPD_CHAN_DISABLED)
			continue;

		dl_list_del(&survey->list);
		DL_LIST_ADD_TAIL(&chan->survey_list, survey, list);

		hostapd_update_nf(iface, chan, survey);

		iface->chans_surveyed++;
	}
}


#ifdef HOSTAPD
#ifdef NEED_AP_MLME

static void hostapd_event_iface_unavailable(struct hostapd_data *hapd)
{
	wpa_printf(MSG_DEBUG, "Interface %s is unavailable -- stopped",
		   hapd->conf->iface);

	if (hapd->csa_in_progress) {
		wpa_printf(MSG_INFO, "CSA failed (%s was stopped)",
			   hapd->conf->iface);
		hostapd_switch_channel_fallback(hapd->iface,
						&hapd->cs_freq_params);
	}
}


static void hostapd_event_dfs_radar_detected(struct hostapd_data *hapd,
					     struct dfs_event *radar)
{
	wpa_printf(MSG_DEBUG, "DFS radar detected on %d MHz", radar->freq);
	hostapd_dfs_radar_detected(hapd->iface, radar->freq, radar->ht_enabled,
				   radar->chan_offset, radar->chan_width,
				   radar->cf1, radar->cf2, radar->rbm);
}


static void hostapd_event_dfs_pre_cac_expired(struct hostapd_data *hapd,
					      struct dfs_event *radar)
{
	wpa_printf(MSG_DEBUG, "DFS Pre-CAC expired on %d MHz", radar->freq);
	hostapd_dfs_pre_cac_expired(hapd->iface, radar->freq, radar->ht_enabled,
				    radar->chan_offset, radar->chan_width,
				    radar->cf1, radar->cf2);
}


static void hostapd_event_dfs_cac_finished(struct hostapd_data *hapd,
					   struct dfs_event *radar)
{
	wpa_printf(MSG_DEBUG, "DFS CAC finished on %d MHz", radar->freq);
	hostapd_dfs_complete_cac(hapd->iface, 1, radar->freq, radar->ht_enabled,
				 radar->chan_offset, radar->chan_width,
				 radar->cf1, radar->cf2);
}


static void hostapd_event_dfs_cac_aborted(struct hostapd_data *hapd,
					  struct dfs_event *radar)
{
	wpa_printf(MSG_DEBUG, "DFS CAC aborted on %d MHz", radar->freq);
	hostapd_dfs_complete_cac(hapd->iface, 0, radar->freq, radar->ht_enabled,
				 radar->chan_offset, radar->chan_width,
				 radar->cf1, radar->cf2);
}

static void hostapd_event_dfs_cac_paused(struct hostapd_data *hapd,
					 struct dfs_event *radar)
{
	wpa_printf(MSG_DEBUG, "DFS CAC paused on %d MHz", radar->freq);
	hostapd_dfs_complete_cac(hapd->iface, 2, radar->freq, radar->ht_enabled,
				 radar->chan_offset, radar->chan_width,
				 radar->cf1, radar->cf2);
}

static void hostapd_event_dfs_nop_finished(struct hostapd_data *hapd,
					   struct dfs_event *radar)
{
	wpa_printf(MSG_DEBUG, "DFS NOP finished on %d MHz", radar->freq);
	hostapd_dfs_nop_finished(hapd->iface, radar->freq, radar->ht_enabled,
				 radar->chan_offset, radar->chan_width,
				 radar->cf1, radar->cf2);
}


static void hostapd_event_dfs_cac_started(struct hostapd_data *hapd,
					  struct dfs_event *radar)
{
	wpa_printf(MSG_DEBUG, "DFS offload CAC started on %d MHz", radar->freq);
	hostapd_dfs_start_cac(hapd->iface, radar->freq, radar->ht_enabled,
			      radar->chan_offset, radar->chan_width,
			      radar->cf1, radar->cf2);
}

#endif /* NEED_AP_MLME */


static void hostapd_event_wds_sta_interface_status(struct hostapd_data *hapd,
						   int istatus,
						   const char *ifname,
						   const u8 *addr)
{
	struct sta_info *sta = ap_get_sta(hapd, addr);

	if (sta) {
		os_free(sta->ifname_wds);
		if (istatus == INTERFACE_ADDED)
			sta->ifname_wds = os_strdup(ifname);
		else
			sta->ifname_wds = NULL;
	}

	wpa_msg(hapd->msg_ctx, MSG_INFO, "%sifname=%s sta_addr=" MACSTR,
		istatus == INTERFACE_ADDED ?
		WDS_STA_INTERFACE_ADDED : WDS_STA_INTERFACE_REMOVED,
		ifname, MAC2STR(addr));
}

#ifdef CONFIG_OWE
static int hostapd_notif_update_dh_ie(struct hostapd_data *hapd,
				      const u8 *peer, const u8 *ie,
				      size_t ie_len)
{
	u16 status;
	struct sta_info *sta;
	struct ieee802_11_elems elems;

	if (!hapd || !hapd->wpa_auth) {
		wpa_printf(MSG_DEBUG, "OWE: Invalid hapd context");
		return -1;
	}
	if (!peer) {
		wpa_printf(MSG_DEBUG, "OWE: Peer unknown");
		return -1;
	}
	if (!(hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_OWE)) {
		wpa_printf(MSG_DEBUG, "OWE: No OWE AKM configured");
		status = WLAN_STATUS_AKMP_NOT_VALID;
		goto err;
	}
	if (ieee802_11_parse_elems(ie, ie_len, &elems, 1) == ParseFailed) {
		wpa_printf(MSG_DEBUG, "OWE: Failed to parse OWE IE for "
			   MACSTR, MAC2STR(peer));
		status = WLAN_STATUS_UNSPECIFIED_FAILURE;
		goto err;
	}
	status = owe_validate_request(hapd, peer, elems.rsn_ie,
				      elems.rsn_ie_len,
				      elems.owe_dh, elems.owe_dh_len);
	if (status != WLAN_STATUS_SUCCESS)
		goto err;

	sta = ap_get_sta(hapd, peer);
	if (sta) {
		ap_sta_no_session_timeout(hapd, sta);
		accounting_sta_stop(hapd, sta);

		/*
		 * Make sure that the previously registered inactivity timer
		 * will not remove the STA immediately.
		 */
		sta->timeout_next = STA_NULLFUNC;
	} else {
		sta = ap_sta_add(hapd, peer);
		if (!sta) {
			status = WLAN_STATUS_UNSPECIFIED_FAILURE;
			goto err;
		}
	}
	sta->flags &= ~(WLAN_STA_WPS | WLAN_STA_MAYBE_WPS | WLAN_STA_WPS2);

	status = owe_process_rsn_ie(hapd, sta, elems.rsn_ie,
				    elems.rsn_ie_len, elems.owe_dh,
				    elems.owe_dh_len);
	if (status != WLAN_STATUS_SUCCESS)
		ap_free_sta(hapd, sta);

	return 0;
err:
	hostapd_drv_update_dh_ie(hapd, peer, status, NULL, 0);
	return 0;
}
#endif /* CONFIG_OWE */


void wpa_supplicant_event(void *ctx, enum wpa_event_type event,
			  union wpa_event_data *data)
{
	struct hostapd_data *hapd = ctx;
#ifndef CONFIG_NO_STDOUT_DEBUG
	int level = MSG_DEBUG;

	if (event == EVENT_RX_MGMT && data->rx_mgmt.frame &&
	    data->rx_mgmt.frame_len >= 24) {
		const struct ieee80211_hdr *hdr;
		u16 fc;

		hdr = (const struct ieee80211_hdr *) data->rx_mgmt.frame;
		fc = le_to_host16(hdr->frame_control);
		if (WLAN_FC_GET_TYPE(fc) == WLAN_FC_TYPE_MGMT &&
		    WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_BEACON)
			level = MSG_EXCESSIVE;
		if (WLAN_FC_GET_TYPE(fc) == WLAN_FC_TYPE_MGMT &&
		    WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_PROBE_REQ)
			level = MSG_EXCESSIVE;
	}

	wpa_dbg(hapd->msg_ctx, level, "Event %s (%d) received",
		event_to_string(event), event);
#endif /* CONFIG_NO_STDOUT_DEBUG */

	switch (event) {
	case EVENT_MICHAEL_MIC_FAILURE:
		michael_mic_failure(hapd, data->michael_mic_failure.src, 1);
		break;
	case EVENT_SCAN_RESULTS:
		if (hapd->iface->scan_cb)
			hapd->iface->scan_cb(hapd->iface);

		if (hapd->iface->retrigger_acs) {
			hapd->iface->retrigger_acs = false;
			if (data->scan_info.aborted && !hapd->iface->cac_started) {
				if (acs_init(hapd->iface) != HOSTAPD_CHAN_ACS)
					wpa_printf(MSG_INFO, "Start ACS again failed after reconf");
			}
			break;
		}

#ifdef CONFIG_ACS
		if (!hapd->iface->conf->acs_init_done)
			acs_recalc_ranks_and_set_chan(hapd->iface, SWR_BG_SCAN);
#endif /* CONFIG_ACS */

		if (hapd->iface->cac_required){
			wpa_printf(MSG_DEBUG, "CAC pending, need to restart CAC");
			hapd->iface->cac_required = 0;
			hostapd_dfs_cac_restart(hapd->iface);
		}

		break;
	case EVENT_WPS_BUTTON_PUSHED:
		hostapd_wps_button_pushed(hapd, NULL);
		break;
#ifdef NEED_AP_MLME
	case EVENT_TX_STATUS:
		switch (data->tx_status.type) {
		case WLAN_FC_TYPE_MGMT:
			hostapd_mgmt_tx_cb(hapd, data->tx_status.data,
					   data->tx_status.data_len,
					   data->tx_status.stype,
					   data->tx_status.ack);
			break;
		case WLAN_FC_TYPE_DATA:
			hostapd_tx_status(hapd, data->tx_status.dst,
					  data->tx_status.data,
					  data->tx_status.data_len,
					  data->tx_status.ack);
			break;
		}
		break;
	case EVENT_EAPOL_TX_STATUS:
		hostapd_eapol_tx_status(hapd, data->eapol_tx_status.dst,
					data->eapol_tx_status.data,
					data->eapol_tx_status.data_len,
					data->eapol_tx_status.ack);
		break;
	case EVENT_DRIVER_CLIENT_POLL_OK:
		hostapd_client_poll_ok(hapd, data->client_poll.addr);
		break;
	case EVENT_RX_FROM_UNKNOWN:
		hostapd_rx_from_unknown_sta(hapd, data->rx_from_unknown.bssid,
					    data->rx_from_unknown.addr,
					    data->rx_from_unknown.wds);
		break;
#endif /* NEED_AP_MLME */
	case EVENT_RX_MGMT:
		if (!data->rx_mgmt.frame)
			break;
#ifdef NEED_AP_MLME
		hostapd_mgmt_rx(hapd, &data->rx_mgmt);
#else /* NEED_AP_MLME */
		hostapd_action_rx(hapd, &data->rx_mgmt);
#endif /* NEED_AP_MLME */
		break;
	case EVENT_RX_PROBE_REQ:
		if (data->rx_probe_req.sa == NULL ||
		    data->rx_probe_req.ie == NULL)
			break;
		hostapd_probe_req_rx(hapd, data->rx_probe_req.sa,
				     data->rx_probe_req.da,
				     data->rx_probe_req.bssid,
				     data->rx_probe_req.ie,
				     data->rx_probe_req.ie_len,
				     data->rx_probe_req.ssi_signal);
		break;
	case EVENT_NEW_STA:
		hostapd_event_new_sta(hapd, data->new_sta.addr);
		break;
	case EVENT_EAPOL_RX:
		hostapd_event_eapol_rx(hapd, data->eapol_rx.src,
				       data->eapol_rx.data,
				       data->eapol_rx.data_len);
		break;
	case EVENT_LTQ_FLUSH_STATIONS_RX:
		hostapd_event_ltq_flush_stations_rx(hapd,
				       data->flush_stations.data,
				       data->flush_stations.data_len);
		break;
	case EVENT_LTQ_SOFTBLOCK_DROP:
		hostapd_drv_event_ltq_softblock_drop(hapd,
			&data->msg_dropped);
		break;
	case EVENT_LTQ_COC:
		hostapd_event_ltq_coc(hapd,
				&data->coc,
				sizeof(struct intel_vendor_event_coc));
		break;
#ifdef CONFIG_ACS
	case EVENT_LTQ_CHAN_DATA:
		hostapd_event_ltq_chan_data(hapd,
					    data->chan_data.data,
					    data->chan_data.data_len);
		break;
#endif
	case EVENT_LTQ_UNCONNECTED_STA_RX:
		hostapd_event_ltq_unconnected_sta_rx(hapd,
			&data->unconnected_sta);
		break;
	case EVENT_LTQ_WDS_CONNECT:
		hostapd_event_ltq_wds_connect_rx(hapd,
			data->wds_sta_info_data.data,
			data->wds_sta_info_data.data_len);
		break;
	case EVENT_LTQ_WDS_DISCONNECT:
		hostapd_event_ltq_wds_disconnect_rx(hapd,
			data->wds_sta_info_data.data,
			data->wds_sta_info_data.data_len);
		break;
	case EVENT_LTQ_RADAR:
		hostapd_event_ltq_radar(hapd, &data->radar);
		break;
	case EVENT_LTQ_IFACE_BRIDGE_CONFIG:
		hostapd_event_iface_bridge_config(hapd, &data->iface_bridge_config);
		break;
	case EVENT_ASSOC:
		if (!data)
			return;
		hostapd_notif_assoc(hapd, data->assoc_info.addr,
				    data->assoc_info.req_ies,
				    data->assoc_info.req_ies_len,
				    data->assoc_info.reassoc);
		break;
#ifdef CONFIG_OWE
	case EVENT_UPDATE_DH:
		if (!data)
			return;
		hostapd_notif_update_dh_ie(hapd, data->update_dh.peer,
					   data->update_dh.ie,
					   data->update_dh.ie_len);
		break;
#endif /* CONFIG_OWE */
	case EVENT_DISASSOC:
		if (data)
			hostapd_notif_disassoc(hapd, data->disassoc_info.addr);
		break;
	case EVENT_DEAUTH:
		if (data)
			hostapd_notif_disassoc(hapd, data->deauth_info.addr);
		break;
	case EVENT_STATION_LOW_ACK:
		if (!data)
			break;
		hostapd_event_sta_low_ack(hapd, data->low_ack.addr);
		break;
	case EVENT_AUTH:
		hostapd_notif_auth(hapd, &data->auth);
		break;
	case EVENT_CH_SWITCH_STARTED:
	case EVENT_CH_SWITCH:
		if (!data)
			break;
		hostapd_event_ch_switch(hapd, data->ch_switch.freq,
					data->ch_switch.ht_enabled,
					data->ch_switch.ch_offset,
					data->ch_switch.ch_width,
					data->ch_switch.cf1,
					data->ch_switch.cf2,
					event == EVENT_CH_SWITCH);
		break;
	case EVENT_CONNECT_FAILED_REASON:
		if (!data)
			break;
		hostapd_event_connect_failed_reason(
			hapd, data->connect_failed_reason.addr,
			data->connect_failed_reason.code);
		break;
	case EVENT_SURVEY:
		hostapd_event_get_survey(hapd->iface, &data->survey_results);
		break;
#ifdef NEED_AP_MLME
	case EVENT_INTERFACE_UNAVAILABLE:
		hostapd_event_iface_unavailable(hapd);
		break;
	case EVENT_DFS_RADAR_DETECTED:
		if (!data)
			break;
		hostapd_event_dfs_radar_detected(hapd, &data->dfs_event);
		break;
	case EVENT_DFS_PRE_CAC_EXPIRED:
		if (!data)
			break;
		hostapd_event_dfs_pre_cac_expired(hapd, &data->dfs_event);
		break;
	case EVENT_DFS_CAC_FINISHED:
		if (!data)
			break;
		hostapd_event_dfs_cac_finished(hapd, &data->dfs_event);
		break;
	case EVENT_DFS_CAC_ABORTED:
		if (!data)
			break;
		hostapd_event_dfs_cac_aborted(hapd, &data->dfs_event);
		break;
	case EVENT_DFS_CAC_PAUSED:
		if (!data)
			break;
		hostapd_event_dfs_cac_paused(hapd, &data->dfs_event);
		break;
	case EVENT_DFS_NOP_FINISHED:
		if (!data)
			break;
		hostapd_event_dfs_nop_finished(hapd, &data->dfs_event);
		break;
	case EVENT_CHANNEL_LIST_CHANGED:
		/* channel list changed (regulatory?), update channel list */
		/* TODO: check this. hostapd_get_hw_features() initializes
		 * too much stuff. */
		/* hostapd_get_hw_features(hapd->iface); */
		hostapd_channel_list_updated(hapd->iface, &data->channel_list_changed);
		break;
	case EVENT_DFS_CAC_STARTED:
		if (!data)
			break;
		hostapd_event_dfs_cac_started(hapd, &data->dfs_event);
		break;
#endif /* NEED_AP_MLME */
	case EVENT_INTERFACE_ENABLED:
		wpa_msg(hapd->msg_ctx, MSG_INFO, INTERFACE_ENABLED);
		if (hapd->disabled && hapd->started) {
			hapd->disabled = 0;
			/*
			 * Try to re-enable interface if the driver stopped it
			 * when the interface got disabled.
			 */
			if (hapd->wpa_auth)
				wpa_auth_reconfig_group_keys(hapd->wpa_auth);
			else
				hostapd_reconfig_encryption(hapd);
			hapd->reenable_beacon = 1;
			if (ieee802_11_update_beacon(hapd) < 0) {
				hapd->iface->set_freq_done = 0;
				hapd->beacon_set_done = 0;
				hapd->reenable_beacon = 1;
				hostapd_setup_interface_complete(hapd->iface, 0);
			} else {
				/* Required for FW add VAP bringup sequence */
				hostapd_tx_queue_params(hapd);
			}
		}
		break;
	case EVENT_INTERFACE_DISABLED:
		hostapd_free_stas(hapd);
		hostapd_free_cached_events(hapd);
		wpa_msg(hapd->msg_ctx, MSG_INFO, INTERFACE_DISABLED);
		hapd->disabled = 1;
		break;
#ifdef CONFIG_ACS
	case EVENT_ACS_CHANNEL_SELECTED:
		hostapd_acs_channel_selected(hapd,
					     &data->acs_selected_channels);
		break;
#endif /* CONFIG_ACS */
	case EVENT_STATION_OPMODE_CHANGED:
		hostapd_event_sta_opmode_changed(hapd, data->sta_opmode.addr,
						 data->sta_opmode.smps_mode,
						 data->sta_opmode.chan_width,
						 data->sta_opmode.rx_nss);
		break;
	case EVENT_WDS_STA_INTERFACE_STATUS:
		hostapd_event_wds_sta_interface_status(
			hapd, data->wds_sta_interface.istatus,
			data->wds_sta_interface.ifname,
			data->wds_sta_interface.sta_addr);
		break;
	default:
		wpa_printf(MSG_DEBUG, "Unknown event %d", event);
		break;
	}
}


void wpa_supplicant_event_global(void *ctx, enum wpa_event_type event,
				 union wpa_event_data *data)
{
	struct hapd_interfaces *interfaces = ctx;
	struct hostapd_data *hapd;

	if (event != EVENT_INTERFACE_STATUS)
		return;

	hapd = hostapd_get_iface(interfaces, data->interface_status.ifname);
	if (hapd && hapd->driver && hapd->driver->get_ifindex &&
	    hapd->drv_priv) {
		unsigned int ifindex;

		ifindex = hapd->driver->get_ifindex(hapd->drv_priv);
		if (ifindex != data->interface_status.ifindex) {
			wpa_dbg(hapd->msg_ctx, MSG_DEBUG,
				"interface status ifindex %d mismatch (%d)",
				ifindex, data->interface_status.ifindex);
			return;
		}
	}
	if (hapd)
		wpa_supplicant_event(hapd, event, data);
}

#endif /* HOSTAPD */
