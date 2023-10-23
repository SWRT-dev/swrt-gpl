/*
 * hostapd / Station table
 * Copyright (c) 2002-2017, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/eloop.h"
#include "common/ieee802_11_defs.h"
#include "common/wpa_ctrl.h"
#include "common/sae.h"
#include "common/dpp.h"
#include "radius/radius.h"
#include "radius/radius_client.h"
#include "p2p/p2p.h"
#include "fst/fst.h"
#include "crypto/crypto.h"
#include "hostapd.h"
#include "accounting.h"
#include "ieee802_1x.h"
#include "ieee802_11.h"
#include "ieee802_11_auth.h"
#include "wpa_auth.h"
#include "preauth_auth.h"
#include "ap_config.h"
#include "beacon.h"
#include "ap_mlme.h"
#include "vlan_init.h"
#include "p2p_hostapd.h"
#include "ap_drv_ops.h"
#include "gas_serv.h"
#include "wnm_ap.h"
#include "mbo_ap.h"
#include "ndisc_snoop.h"
#include "sta_info.h"
#ifdef CONFIG_WDS_WPA
#include "rsn_supp/wpa.h"
#endif
#include "vlan.h"
#include "wps_hostapd.h"

#define STA_EVENT_MAX_BUF_LEN	1024
#define STA_EXTRA_CAP_LEN	300
#define UPPER_24G_FREQ_VALUE	2500
#define FIRST_5G_CHAN	36

#define DO_RELATE_OPER(return_value, first, second, operation) { \
		return_value = (first operation second) ? first : second; \
	}

static void ap_sta_remove_in_other_bss(struct hostapd_data *hapd,
				       struct sta_info *sta);
static void ap_handle_session_timer(void *eloop_ctx, void *timeout_ctx);
static void ap_handle_session_warning_timer(void *eloop_ctx, void *timeout_ctx);
static void ap_sta_deauth_cb_timeout(void *eloop_ctx, void *timeout_ctx);
static void ap_sta_disassoc_cb_timeout(void *eloop_ctx, void *timeout_ctx);
static void ap_sa_query_timer(void *eloop_ctx, void *timeout_ctx);
static int ap_sta_remove(struct hostapd_data *hapd, struct sta_info *sta);
static void ap_sta_delayed_1x_auth_fail_cb(void *eloop_ctx, void *timeout_ctx);
static void sta_get_phy_mode_ch_width(struct hostapd_data *hapd, struct sta_info *sta,
	unsigned int *phyMode, unsigned int *max_ch_width);
static void sta_get_he_mcs_nss(u8 *sta_he_mcs_nss, u8 *mcs, u8 *nss);
static void sta_get_mcs_nss(struct hostapd_data *hapd, struct sta_info *sta,
        unsigned int *max_mcs, unsigned int *max_streams);

int ap_for_each_sta(struct hostapd_data *hapd,
		    int (*cb)(struct hostapd_data *hapd, struct sta_info *sta,
			      void *ctx),
		    void *ctx)
{
	struct sta_info *sta;

	for (sta = hapd->sta_list; sta; sta = sta->next) {
		if (cb(hapd, sta, ctx))
			return 1;
	}

	return 0;
}


struct sta_info * ap_get_sta(struct hostapd_data *hapd, const u8 *sta)
{
	struct sta_info *s;

	s = hapd->sta_hash[STA_HASH(sta)];
	while (s != NULL && os_memcmp(s->addr, sta, 6) != 0)
		s = s->hnext;
	return s;
}


#ifdef CONFIG_P2P
struct sta_info * ap_get_sta_p2p(struct hostapd_data *hapd, const u8 *addr)
{
	struct sta_info *sta;
	int i;

	for (sta = hapd->sta_list; sta; sta = sta->next) {
		const u8 *p2p_dev_addr;

		if (sta->p2p_ie == NULL)
			continue;

		p2p_dev_addr = p2p_get_go_dev_addr(sta->p2p_ie);
		if (p2p_dev_addr == NULL)
			continue;

		if (os_memcmp(p2p_dev_addr, addr, ETH_ALEN) == 0)
			return sta;
	}

	return NULL;
}
#endif /* CONFIG_P2P */


static void ap_sta_list_del(struct hostapd_data *hapd, struct sta_info *sta)
{
	struct sta_info *tmp;

	if (hapd->sta_list == sta) {
		hapd->sta_list = sta->next;
		return;
	}

	tmp = hapd->sta_list;
	while (tmp != NULL && tmp->next != sta)
		tmp = tmp->next;
	if (tmp == NULL) {
		wpa_printf(MSG_DEBUG, "Could not remove STA " MACSTR " from "
			   "list.", MAC2STR(sta->addr));
	} else
		tmp->next = sta->next;
}


void ap_sta_hash_add(struct hostapd_data *hapd, struct sta_info *sta)
{
	sta->hnext = hapd->sta_hash[STA_HASH(sta->addr)];
	hapd->sta_hash[STA_HASH(sta->addr)] = sta;
}


static void ap_sta_hash_del(struct hostapd_data *hapd, struct sta_info *sta)
{
	struct sta_info *s;

	s = hapd->sta_hash[STA_HASH(sta->addr)];
	if (s == NULL) return;
	if (os_memcmp(s->addr, sta->addr, 6) == 0) {
		hapd->sta_hash[STA_HASH(sta->addr)] = s->hnext;
		return;
	}

	while (s->hnext != NULL &&
	       os_memcmp(s->hnext->addr, sta->addr, ETH_ALEN) != 0)
		s = s->hnext;
	if (s->hnext != NULL)
		s->hnext = s->hnext->hnext;
	else
		wpa_printf(MSG_DEBUG, "AP: could not remove STA " MACSTR
			   " from hash table", MAC2STR(sta->addr));
}


void ap_sta_ip6addr_del(struct hostapd_data *hapd, struct sta_info *sta)
{
	sta_ip6addr_del(hapd, sta);
}


void ap_free_sta(struct hostapd_data *hapd, struct sta_info *sta)
{
	int set_beacon = 0;
	bool is_recovery = (hapd->iface->stas_free_reason == WLAN_STA_FREE_REASON_RECOVERY);

	accounting_sta_stop(hapd, sta);

	/* just in case */
	sta->flags &= ~WLAN_STA_BSS_TRANS_MGMT_REQ_TIMEOUT;
	sta->disassoc_reason = (hapd->iface->stas_free_reason < WLAN_REASON_UNSPECIFIED) ? WLAN_REASON_UNSPECIFIED :
					hapd->iface->stas_free_reason;
	sta->disconnect_source = DISCONNECT_SOURCE_LOCAL;
	ap_sta_set_authorized(hapd, sta, 0);
	hostapd_set_sta_flags(hapd, sta);

	if (hapd->conf->multi_ap_vlan_per_bsta &&
	    (sta->flags & (WLAN_STA_WDS | WLAN_STA_MULTI_AP)))
		hostapd_set_wds_sta(hapd, NULL, sta->addr, sta->aid, 0);

	if (sta->ipaddr)
		hostapd_drv_br_delete_ip_neigh(hapd, 4, (u8 *) &sta->ipaddr);
	ap_sta_ip6addr_del(hapd, sta);

#if 0
	if (!hapd->iface->driver_ap_teardown &&
	    !(sta->flags & WLAN_STA_PREAUTH)) {
#else
	if (!(sta->flags & WLAN_STA_PREAUTH)) {
#endif
		if (!is_recovery)
			hostapd_drv_sta_remove(hapd, sta->addr);
		sta->added_unassoc = 0;
	}

	ap_sta_hash_del(hapd, sta);
	ap_sta_list_del(hapd, sta);

	if (sta->aid > 0) {
		if (hapd->driver->free_aid) {
			if (!is_recovery) {
				sta->aid -= hapd->iconf->mbssid_aid_offset;
				(*hapd->driver->free_aid)(hapd->drv_priv, &sta->aid);
			}
		}
		else
			hapd->sta_aid[(sta->aid - 1) / 32]
				&= ~BIT((sta->aid - 1) % 32);
	}

	hapd->num_sta--;
	if (sta->nonerp_set) {
		sta->nonerp_set = 0;
		hapd->iface->num_sta_non_erp--;
		if (hapd->iface->num_sta_non_erp == 0)
			set_beacon++;
	}

	if (sta->no_short_slot_time_set) {
		sta->no_short_slot_time_set = 0;
		hapd->iface->num_sta_no_short_slot_time--;
		if (hapd->iface->current_mode &&
		    hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211G
		    && hapd->iface->num_sta_no_short_slot_time == 0)
			set_beacon++;
	}

	if (sta->no_short_preamble_set) {
		sta->no_short_preamble_set = 0;
		hapd->iface->num_sta_no_short_preamble--;
		if (hapd->iface->current_mode &&
		    hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211G
		    && hapd->iface->num_sta_no_short_preamble == 0)
			set_beacon++;
	}

	if (sta->no_ht_gf_set) {
		sta->no_ht_gf_set = 0;
		hapd->iface->num_sta_ht_no_gf--;
	}

	if (sta->no_ht_set) {
		sta->no_ht_set = 0;
		hapd->iface->num_sta_no_ht--;
	}

	if (sta->ht_20mhz_set) {
		sta->ht_20mhz_set = 0;
		hapd->iface->num_sta_ht_20mhz--;
	}

#ifdef CONFIG_TAXONOMY
	wpabuf_free(sta->probe_ie_taxonomy);
	sta->probe_ie_taxonomy = NULL;
	wpabuf_free(sta->assoc_ie_taxonomy);
	sta->assoc_ie_taxonomy = NULL;
#endif /* CONFIG_TAXONOMY */

	ht40_intolerant_remove(hapd->iface, sta);

#ifdef CONFIG_P2P
	if (sta->no_p2p_set) {
		sta->no_p2p_set = 0;
		hapd->num_sta_no_p2p--;
		if (hapd->num_sta_no_p2p == 0)
			hostapd_p2p_non_p2p_sta_disconnected(hapd);
	}
#endif /* CONFIG_P2P */

#ifdef NEED_AP_MLME
	if (hostapd_ht_operation_update(hapd->iface) > 0)
		set_beacon++;
#endif /* NEED_AP_MLME */

#ifdef CONFIG_MESH
	if (hapd->mesh_sta_free_cb)
		hapd->mesh_sta_free_cb(hapd, sta);
#endif /* CONFIG_MESH */

	if (set_beacon)
		ieee802_11_update_beacons(hapd->iface);

	wpa_printf(MSG_DEBUG, "%s: cancel ap_handle_timer for " MACSTR,
		   __func__, MAC2STR(sta->addr));
	eloop_cancel_timeout(ap_handle_timer, hapd, sta);
	eloop_cancel_timeout(ap_handle_session_timer, hapd, sta);
	eloop_cancel_timeout(ap_handle_session_warning_timer, hapd, sta);
	ap_sta_clear_disconnect_timeouts(hapd, sta);
	sae_clear_retransmit_timer(hapd, sta);

	ieee802_1x_free_station(hapd, sta);
	wpa_auth_sta_deinit(sta->wpa_sm);
#ifdef CONFIG_WDS_WPA
	wpa_sm_deinit(sta->wpa);
#endif
	rsn_preauth_free_station(hapd, sta);
#ifndef CONFIG_NO_RADIUS
	if (hapd->radius)
		radius_client_flush_auth(hapd->radius, sta->addr);
#endif /* CONFIG_NO_RADIUS */

#ifndef CONFIG_NO_VLAN
	/*
	 * sta->wpa_sm->group needs to be released before so that
	 * vlan_remove_dynamic() can check that no stations are left on the
	 * AP_VLAN netdev.
	 */
	if (sta->vlan_id)
		vlan_remove_dynamic(hapd, sta->vlan_id);
	if (sta->vlan_id_bound) {
		/*
		 * Need to remove the STA entry before potentially removing the
		 * VLAN.
		 */
		if (hapd->iface->driver_ap_teardown &&
		    !(sta->flags & WLAN_STA_PREAUTH)) {
			if (!is_recovery)
				hostapd_drv_sta_remove(hapd, sta->addr);
			sta->added_unassoc = 0;
		}
		vlan_remove_dynamic(hapd, sta->vlan_id_bound);
	}
#endif /* CONFIG_NO_VLAN */

  os_free(sta->last_assoc_req);
  os_free(sta->challenge);

	os_free(sta->sa_query_trans_id);
	eloop_cancel_timeout(ap_sa_query_timer, hapd, sta);

#ifdef CONFIG_P2P
	p2p_group_notif_disassoc(hapd->p2p_group, sta->addr);
#endif /* CONFIG_P2P */

#ifdef CONFIG_INTERWORKING
	if (sta->gas_dialog) {
		int i;
		for (i = 0; i < GAS_DIALOG_MAX; i++)
			gas_serv_dialog_clear(&sta->gas_dialog[i]);
		os_free(sta->gas_dialog);
	}
#endif /* CONFIG_INTERWORKING */

	wpabuf_free(sta->wps_ie);
	wpabuf_free(sta->p2p_ie);
	wpabuf_free(sta->hs20_ie);
	wpabuf_free(sta->roaming_consortium);
#ifdef CONFIG_FST
	wpabuf_free(sta->mb_ies);
#endif /* CONFIG_FST */

	os_free(sta->ht_capabilities);
	os_free(sta->ht_operation);
	os_free(sta->vht_capabilities);
	os_free(sta->vht_operation);
	os_free(sta->he_capabilities);
	os_free(sta->he_operation);
	os_free(sta->he_6ghz_capab);
	hostapd_free_psk_list(sta->psk);
	os_free(sta->identity);
	os_free(sta->radius_cui);
	os_free(sta->remediation_url);
	os_free(sta->t_c_url);
	wpabuf_free(sta->hs20_deauth_req);
	os_free(sta->hs20_session_info_url);

#ifdef CONFIG_SAE
	sae_clear_data(sta->sae);
	os_free(sta->sae);
#endif /* CONFIG_SAE */

	mbo_ap_sta_free(sta);
	os_free(sta->supp_op_classes);

#ifdef CONFIG_FILS
	os_free(sta->fils_pending_assoc_req);
	wpabuf_free(sta->fils_hlp_resp);
	wpabuf_free(sta->hlp_dhcp_discover);
	eloop_cancel_timeout(fils_hlp_timeout, hapd, sta);
#ifdef CONFIG_FILS_SK_PFS
	crypto_ecdh_deinit(sta->fils_ecdh);
	wpabuf_clear_free(sta->fils_dh_ss);
	wpabuf_free(sta->fils_g_sta);
#endif /* CONFIG_FILS_SK_PFS */
#endif /* CONFIG_FILS */

#ifdef CONFIG_OWE
	bin_clear_free(sta->owe_pmk, sta->owe_pmk_len);
	crypto_ecdh_deinit(sta->owe_ecdh);
#endif /* CONFIG_OWE */

#ifdef CONFIG_DPP2
	dpp_pfs_free(sta->dpp_pfs);
	sta->dpp_pfs = NULL;
#endif /* CONFIG_DPP2 */

	os_free(sta->ext_capability);

#ifdef CONFIG_WNM_AP
	eloop_cancel_timeout(ap_sta_reset_steer_flag_timer, hapd, sta);
#endif /* CONFIG_WNM_AP */

	os_free(sta->ifname_wds);

#ifdef CONFIG_TESTING_OPTIONS
	os_free(sta->sae_postponed_commit);
#endif /* CONFIG_TESTING_OPTIONS */

	os_free(sta);
}


void hostapd_free_stas(struct hostapd_data *hapd)
{
	struct sta_info *sta, *prev;

	sta = hapd->sta_list;

	while (sta) {
		prev = sta;
		if (sta->flags & WLAN_STA_AUTH) {
			mlme_deauthenticate_indication(
				hapd, sta, WLAN_REASON_UNSPECIFIED);
		}
		sta = sta->next;
		wpa_printf(MSG_DEBUG, "Removing station " MACSTR,
			   MAC2STR(prev->addr));
		ap_free_sta(hapd, prev);
	}
}


/**
 * ap_handle_timer - Per STA timer handler
 * @eloop_ctx: struct hostapd_data *
 * @timeout_ctx: struct sta_info *
 *
 * This function is called to check station activity and to remove inactive
 * stations.
 */
void ap_handle_timer(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	struct sta_info *sta = timeout_ctx;
	unsigned long next_time = 0;
	u16 reason;

	wpa_printf(MSG_DEBUG, "%s: %s: " MACSTR " flags=0x%x timeout_next=%d",
		   hapd->conf->iface, __func__, MAC2STR(sta->addr), sta->flags,
		   sta->timeout_next);
	if (sta->timeout_next == STA_REMOVE) {
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_INFO, "deauthenticated due to "
			       "local deauth request");
		ap_free_sta(hapd, sta);
		return;
	}

	if ((sta->flags & WLAN_STA_ASSOC) &&
	    (sta->timeout_next == STA_NULLFUNC ||
	     sta->timeout_next == STA_DISASSOC)) {
		int inactive_sec;
		/*
		 * Add random value to timeout so that we don't end up bouncing
		 * all stations at the same time if we have lots of associated
		 * stations that are idle (but keep re-associating).
		 */
		int fuzz = os_random() % 20;
		inactive_sec = hostapd_drv_get_inact_sec(hapd, sta->addr);
		if (inactive_sec == -1) {
			wpa_msg(hapd->msg_ctx, MSG_DEBUG,
				"Check inactivity: Could not "
				"get station info from kernel driver for "
				MACSTR, MAC2STR(sta->addr));
			/*
			 * The driver may not support this functionality.
			 * Anyway, try again after the next inactivity timeout,
			 * but do not disconnect the station now.
			 */
			next_time = hapd->conf->ap_max_inactivity + fuzz;
		} else if (inactive_sec == -ENOENT) {
			wpa_msg(hapd->msg_ctx, MSG_DEBUG,
				"Station " MACSTR " has lost its driver entry",
				MAC2STR(sta->addr));

			/* Avoid sending client probe on removed client */
			sta->timeout_next = STA_DISASSOC;
			goto skip_poll;
		} else if (inactive_sec < hapd->conf->ap_max_inactivity) {
			/* station activity detected; reset timeout state */
			wpa_msg(hapd->msg_ctx, MSG_DEBUG,
				"Station " MACSTR " has been active %is ago",
				MAC2STR(sta->addr), inactive_sec);
			sta->timeout_next = STA_NULLFUNC;
			next_time = hapd->conf->ap_max_inactivity + fuzz -
				inactive_sec;
		} else {
			wpa_msg(hapd->msg_ctx, MSG_DEBUG,
				"Station " MACSTR " has been "
				"inactive too long: %d sec, max allowed: %d",
				MAC2STR(sta->addr), inactive_sec,
				hapd->conf->ap_max_inactivity);

			if (hapd->conf->skip_inactivity_poll)
				sta->timeout_next = STA_DISASSOC;
		}
	}

	if ((sta->flags & WLAN_STA_ASSOC) &&
	    sta->timeout_next == STA_DISASSOC &&
	    !(sta->flags & WLAN_STA_PENDING_POLL) &&
	    !hapd->conf->skip_inactivity_poll) {
		wpa_msg(hapd->msg_ctx, MSG_DEBUG, "Station " MACSTR
			" has ACKed data poll", MAC2STR(sta->addr));
		/* data nullfunc frame poll did not produce TX errors; assume
		 * station ACKed it */
		sta->timeout_next = STA_NULLFUNC;
		next_time = hapd->conf->ap_max_inactivity;
	}

skip_poll:
	if (next_time) {
		wpa_printf(MSG_DEBUG, "%s: register ap_handle_timer timeout "
			   "for " MACSTR " (%lu seconds)",
			   __func__, MAC2STR(sta->addr), next_time);
		eloop_register_timeout(next_time, 0, ap_handle_timer, hapd,
				       sta);
		return;
	}

	if (sta->timeout_next == STA_NULLFUNC &&
	    (sta->flags & WLAN_STA_ASSOC)) {
		wpa_printf(MSG_DEBUG, "  Polling STA");
		sta->flags |= WLAN_STA_PENDING_POLL;
		hostapd_drv_poll_client(hapd, hapd->own_addr, sta->addr,
					sta->flags & WLAN_STA_WMM);
	} else if (sta->timeout_next != STA_REMOVE) {
		int deauth = sta->timeout_next == STA_DEAUTH;

		if (!deauth && !(sta->flags & WLAN_STA_ASSOC)) {
			/* Cannot disassociate not-associated STA, so move
			 * directly to deauthentication. */
			sta->timeout_next = STA_DEAUTH;
			deauth = 1;
		}

		wpa_dbg(hapd->msg_ctx, MSG_DEBUG,
			"Timeout, sending %s info to STA " MACSTR,
			deauth ? "deauthentication" : "disassociation",
			MAC2STR(sta->addr));

		if (deauth) {
			hostapd_drv_sta_deauth(
				hapd, sta->addr,
				WLAN_REASON_PREV_AUTH_NOT_VALID);
		} else {
			reason = (sta->timeout_next == STA_DISASSOC) ?
				WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY :
				WLAN_REASON_PREV_AUTH_NOT_VALID;

			hostapd_drv_sta_disassoc(hapd, sta->addr, reason);
		}
	}

	switch (sta->timeout_next) {
	case STA_NULLFUNC:
		sta->timeout_next = STA_DISASSOC;
		wpa_printf(MSG_DEBUG, "%s: register ap_handle_timer timeout "
			   "for " MACSTR " (%d seconds - AP_DISASSOC_DELAY)",
			   __func__, MAC2STR(sta->addr), AP_DISASSOC_DELAY);
		eloop_register_timeout(AP_DISASSOC_DELAY, 0, ap_handle_timer,
				       hapd, sta);
		break;
	case STA_DISASSOC:
	case STA_DISASSOC_FROM_CLI:
		sta->flags &= ~WLAN_STA_BSS_TRANS_MGMT_REQ_TIMEOUT;
		reason = sta->disassoc_reason = (sta->timeout_next == STA_DISASSOC) ?
						WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY :
						WLAN_REASON_PREV_AUTH_NOT_VALID;
		sta->disconnect_source = DISCONNECT_SOURCE_LOCAL;
		ap_sta_set_authorized(hapd, sta, 0);
		sta->flags &= ~WLAN_STA_ASSOC;
		hostapd_set_sta_flags(hapd, sta);
		ieee802_1x_notify_port_enabled(sta->eapol_sm, 0);
		if (!sta->acct_terminate_cause)
			sta->acct_terminate_cause =
				RADIUS_ACCT_TERMINATE_CAUSE_IDLE_TIMEOUT;
		accounting_sta_stop(hapd, sta);
		ieee802_1x_free_station(hapd, sta);
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_INFO, "disassociated due to "
			       "inactivity");
		sta->timeout_next = STA_DEAUTH;
		wpa_printf(MSG_DEBUG, "%s: register ap_handle_timer timeout "
			   "for " MACSTR " (%d seconds - AP_DEAUTH_DELAY)",
			   __func__, MAC2STR(sta->addr), AP_DEAUTH_DELAY);
		eloop_register_timeout(AP_DEAUTH_DELAY, 0, ap_handle_timer,
				       hapd, sta);
		mlme_disassociate_indication(hapd, sta, reason);
		break;
	case STA_DEAUTH:
	case STA_REMOVE:
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_INFO, "deauthenticated due to "
			       "inactivity (timer DEAUTH/REMOVE)");
		if (!sta->acct_terminate_cause)
			sta->acct_terminate_cause =
				RADIUS_ACCT_TERMINATE_CAUSE_IDLE_TIMEOUT;
		mlme_deauthenticate_indication(
			hapd, sta,
			WLAN_REASON_PREV_AUTH_NOT_VALID);
		ap_free_sta(hapd, sta);
		break;
	}
}


static void ap_handle_session_timer(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	struct sta_info *sta = timeout_ctx;

	wpa_printf(MSG_DEBUG, "%s: Session timer for STA " MACSTR,
		   hapd->conf->iface, MAC2STR(sta->addr));
	if (!(sta->flags & (WLAN_STA_AUTH | WLAN_STA_ASSOC |
			    WLAN_STA_AUTHORIZED))) {
		if (sta->flags & WLAN_STA_GAS) {
			wpa_printf(MSG_DEBUG, "GAS: Remove temporary STA "
				   "entry " MACSTR, MAC2STR(sta->addr));
			ap_free_sta(hapd, sta);
		}
		return;
	}

	hostapd_drv_sta_deauth(hapd, sta->addr,
			       WLAN_REASON_PREV_AUTH_NOT_VALID);
	mlme_deauthenticate_indication(hapd, sta,
				       WLAN_REASON_PREV_AUTH_NOT_VALID);
	hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_INFO, "deauthenticated due to "
		       "session timeout");
	sta->acct_terminate_cause =
		RADIUS_ACCT_TERMINATE_CAUSE_SESSION_TIMEOUT;
	ap_free_sta(hapd, sta);
}


void ap_sta_replenish_timeout(struct hostapd_data *hapd, struct sta_info *sta,
			      u32 session_timeout)
{
	if (eloop_replenish_timeout(session_timeout, 0,
				    ap_handle_session_timer, hapd, sta) == 1) {
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_DEBUG, "setting session timeout "
			       "to %d seconds", session_timeout);
	}
}


void ap_sta_session_timeout(struct hostapd_data *hapd, struct sta_info *sta,
			    u32 session_timeout)
{
	hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_DEBUG, "setting session timeout to %d "
		       "seconds", session_timeout);
	eloop_cancel_timeout(ap_handle_session_timer, hapd, sta);
	eloop_register_timeout(session_timeout, 0, ap_handle_session_timer,
			       hapd, sta);
}


void ap_sta_no_session_timeout(struct hostapd_data *hapd, struct sta_info *sta)
{
	eloop_cancel_timeout(ap_handle_session_timer, hapd, sta);
}


static void ap_handle_session_warning_timer(void *eloop_ctx, void *timeout_ctx)
{
#ifdef CONFIG_WNM_AP
	struct hostapd_data *hapd = eloop_ctx;
	struct sta_info *sta = timeout_ctx;

	wpa_printf(MSG_DEBUG, "%s: WNM: Session warning time reached for "
		   MACSTR, hapd->conf->iface, MAC2STR(sta->addr));
	if (sta->hs20_session_info_url == NULL)
		return;

	wnm_send_ess_disassoc_imminent(hapd, sta, sta->hs20_session_info_url,
				       sta->hs20_disassoc_timer);
#endif /* CONFIG_WNM_AP */
}


void ap_sta_session_warning_timeout(struct hostapd_data *hapd,
				    struct sta_info *sta, int warning_time)
{
	eloop_cancel_timeout(ap_handle_session_warning_timer, hapd, sta);
	eloop_register_timeout(warning_time, 0, ap_handle_session_warning_timer,
			       hapd, sta);
}

static int num_sta_get_total(struct hostapd_data *hapd)
{
	struct hostapd_iface *iface = hapd->iface;
	size_t i;
	int num_of_sta;

	for (i = 0, num_of_sta = 0; i < iface->num_bss; i++) {
		struct hostapd_data *bss = iface->bss[i];
		num_of_sta += bss->num_sta;
	}
	return num_of_sta;
}

static int num_res_sta_get_total(struct hostapd_data *hapd)
{
	struct hostapd_iface *iface = hapd->iface;
	size_t i;
	int num_res_sta;

	for (i = 0, num_res_sta = 0; i < iface->num_bss; i++) {
		struct hostapd_data *bss = iface->bss[i];
		num_res_sta += bss->conf->num_res_sta;
	}
	return num_res_sta;
}

struct sta_info * ap_sta_add_ex(struct hostapd_data *hapd, const u8 *addr, u8 *out_limit_reached)
{
	struct sta_info *sta;
	int i = 0;
	int num_sta_total = num_sta_get_total(hapd);
	int num_res_sta_total = num_res_sta_get_total(hapd);
	*out_limit_reached = 0;

	sta = ap_get_sta(hapd, addr);
	if (sta)
		return sta;

	wpa_printf(MSG_DEBUG, "  New STA");

	if (hapd->conf->num_res_sta) { /* Number of reserved STAs is set for this BSS */
		if (hapd->num_sta >= hapd->conf->max_num_sta) {
			/* FIX: might try to remove some old STAs first? */
			wpa_printf(MSG_ERROR, "no more room for new STAs (%d/%d)",
				   hapd->num_sta, hapd->conf->max_num_sta);
			*out_limit_reached = 1;
			return NULL;
		}
	} else { /* Number of reserved STAs is not set for this BSS */
		if (hapd->num_sta >= (hapd->conf->max_num_sta - num_res_sta_total)) {
			wpa_printf(MSG_ERROR, "no more room for new STAs, "
				   "reserved STAs limit is reached for BSS(%d/%d)",
				   hapd->num_sta, hapd->conf->max_num_sta - num_res_sta_total);
			*out_limit_reached = 1;
			return NULL;
		}
	}
	if (num_sta_total >= hapd->iconf->ap_max_num_sta) {
		wpa_printf(MSG_ERROR, "no more room for new STAs, Radio limit reached (%d/%d)",
			   num_sta_total, hapd->iconf->ap_max_num_sta);
		*out_limit_reached = 1;
		return NULL;
	}
	if (hapd->iconf->atf_cfg.distr_type && hostapd_atf_is_sta_allowed(hapd, addr) == 0) {
		wpa_printf(MSG_INFO, "ATF: Configuration/capacity doesn't allow station "
				MACSTR " to connect", MAC2STR(addr));
		return NULL;
	}
	sta = os_zalloc(sizeof(struct sta_info));
	if (sta == NULL) {
		wpa_printf(MSG_ERROR, "malloc failed");
		return NULL;
	}
	sta->acct_interim_interval = hapd->conf->acct_interim_interval;
	if (accounting_sta_get_id(hapd, sta) < 0) {
		os_free(sta);
		return NULL;
	}

	for (i = 0; i < WLAN_SUPP_RATES_MAX; i++) {
		if (!hapd->iface->basic_rates)
			break;
		if (hapd->iface->basic_rates[i] < 0)
			break;
		sta->supported_rates[i] = hapd->iface->basic_rates[i] / 5;
	}
	sta->supported_rates_len = i;

	if (!(hapd->iface->drv_flags & WPA_DRIVER_FLAGS_INACTIVITY_TIMER)) {
		wpa_printf(MSG_DEBUG, "%s: register ap_handle_timer timeout "
			   "for " MACSTR " (%d seconds - ap_max_inactivity)",
			   __func__, MAC2STR(addr),
			   hapd->conf->ap_max_inactivity);
		eloop_register_timeout(hapd->conf->ap_max_inactivity, 0,
				       ap_handle_timer, hapd, sta);
	}

	/* initialize STA info data */
	os_memcpy(sta->addr, addr, ETH_ALEN);
	sta->next = hapd->sta_list;
	hapd->sta_list = sta;
	hapd->num_sta++;
	ap_sta_hash_add(hapd, sta);
	ap_sta_remove_in_other_bss(hapd, sta);
	sta->last_seq_ctrl = WLAN_INVALID_MGMT_SEQ;
	dl_list_init(&sta->ip6addr);

#ifdef CONFIG_TAXONOMY
	sta_track_claim_taxonomy_info(hapd->iface, addr,
				      &sta->probe_ie_taxonomy);
#endif /* CONFIG_TAXONOMY */

	return sta;
}

struct sta_info * ap_sta_add(struct hostapd_data *hapd, const u8 *addr)
{
	u8 out_limit_reached;
	return ap_sta_add_ex(hapd, addr, &out_limit_reached);
}

static int ap_sta_remove(struct hostapd_data *hapd, struct sta_info *sta)
{
	ieee802_1x_notify_port_enabled(sta->eapol_sm, 0);

	if (sta->ipaddr)
		hostapd_drv_br_delete_ip_neigh(hapd, 4, (u8 *) &sta->ipaddr);
	ap_sta_ip6addr_del(hapd, sta);

	wpa_printf(MSG_DEBUG, "%s: Removing STA " MACSTR " from kernel driver",
		   hapd->conf->iface, MAC2STR(sta->addr));
	if (hostapd_drv_sta_remove(hapd, sta->addr) &&
	    sta->flags & WLAN_STA_ASSOC) {
		wpa_printf(MSG_DEBUG, "%s: Could not remove station " MACSTR
			   " from kernel driver",
			   hapd->conf->iface, MAC2STR(sta->addr));
		return -1;
	}
	sta->added_unassoc = 0;
	return 0;
}


static void ap_sta_remove_in_other_bss(struct hostapd_data *hapd,
				       struct sta_info *sta)
{
	struct hostapd_iface *iface = hapd->iface;
	size_t i;

	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *bss = iface->bss[i];
		struct sta_info *sta2;
		/* bss should always be set during operation, but it may be
		 * NULL during reconfiguration. Assume the STA is not
		 * associated to another BSS in that case to avoid NULL pointer
		 * dereferences. */
		if (bss == hapd || bss == NULL)
			continue;
		sta2 = ap_get_sta(bss, sta->addr);
		if (!sta2)
			continue;

		wpa_printf(MSG_DEBUG, "%s: disconnect old STA " MACSTR
			   " association from another BSS %s",
			   hapd->conf->iface, MAC2STR(sta2->addr),
			   bss->conf->iface);
		ap_sta_disconnect(bss, sta2, sta2->addr,
				  WLAN_REASON_PREV_AUTH_NOT_VALID);
	}
}


static void ap_sta_disassoc_cb_timeout(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	struct sta_info *sta = timeout_ctx;

	wpa_printf(MSG_DEBUG, "%s: Disassociation callback for STA " MACSTR,
		   hapd->conf->iface, MAC2STR(sta->addr));
	ap_sta_remove(hapd, sta);
	mlme_disassociate_indication(hapd, sta, sta->disassoc_reason);
}


void ap_sta_disassociate(struct hostapd_data *hapd, struct sta_info *sta,
			 u16 reason)
{
	wpa_printf(MSG_DEBUG, "%s: disassociate STA " MACSTR,
		   hapd->conf->iface, MAC2STR(sta->addr));
	sta->last_seq_ctrl = WLAN_INVALID_MGMT_SEQ;
	if (hapd->iface->current_mode &&
	    hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211AD) {
		/* Skip deauthentication in DMG/IEEE 802.11ad */
		sta->flags &= ~(WLAN_STA_AUTH | WLAN_STA_ASSOC |
				WLAN_STA_ASSOC_REQ_OK);
		sta->timeout_next = STA_REMOVE;
	} else {
		sta->flags &= ~(WLAN_STA_ASSOC | WLAN_STA_ASSOC_REQ_OK);
		sta->timeout_next = STA_DEAUTH;
	}
        sta->flags &= ~WLAN_STA_BSS_TRANS_MGMT_REQ_TIMEOUT;
	sta->disassoc_reason = reason;
	sta->disconnect_source = DISCONNECT_SOURCE_LOCAL;
	ap_sta_set_authorized(hapd, sta, 0);
	hostapd_set_sta_flags(hapd, sta);
	wpa_printf(MSG_DEBUG, "%s: reschedule ap_handle_timer timeout "
		   "for " MACSTR " (%d seconds - "
		   "AP_MAX_INACTIVITY_AFTER_DISASSOC)",
		   __func__, MAC2STR(sta->addr),
		   AP_MAX_INACTIVITY_AFTER_DISASSOC);
	eloop_cancel_timeout(ap_handle_timer, hapd, sta);
	eloop_register_timeout(AP_MAX_INACTIVITY_AFTER_DISASSOC, 0,
			       ap_handle_timer, hapd, sta);
	accounting_sta_stop(hapd, sta);
	ieee802_1x_free_station(hapd, sta);
	wpa_auth_sta_deinit(sta->wpa_sm);
	sta->wpa_sm = NULL;

	sta->flags |= WLAN_STA_PENDING_DISASSOC_CB;
	eloop_cancel_timeout(ap_sta_disassoc_cb_timeout, hapd, sta);
	eloop_register_timeout(hapd->iface->drv_flags &
			       WPA_DRIVER_FLAGS_DEAUTH_TX_STATUS ? 2 : 0, 0,
			       ap_sta_disassoc_cb_timeout, hapd, sta);
}


static void ap_sta_deauth_cb_timeout(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	struct sta_info *sta = timeout_ctx;

	wpa_printf(MSG_DEBUG, "%s: Deauthentication callback for STA " MACSTR,
		   hapd->conf->iface, MAC2STR(sta->addr));
	ap_sta_remove(hapd, sta);
	mlme_deauthenticate_indication(hapd, sta, sta->deauth_reason);
}


void ap_sta_deauthenticate(struct hostapd_data *hapd, struct sta_info *sta,
			   u16 reason)
{
	if (hapd->iface->current_mode &&
	    hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211AD) {
		/* Deauthentication is not used in DMG/IEEE 802.11ad;
		 * disassociate the STA instead. */
		ap_sta_disassociate(hapd, sta, reason);
		return;
	}

	wpa_printf(MSG_DEBUG, "%s: deauthenticate STA " MACSTR,
		   hapd->conf->iface, MAC2STR(sta->addr));
	sta->last_seq_ctrl = WLAN_INVALID_MGMT_SEQ;
	sta->flags &= ~(WLAN_STA_AUTH | WLAN_STA_ASSOC | WLAN_STA_ASSOC_REQ_OK);
	sta->flags &= ~WLAN_STA_BSS_TRANS_MGMT_REQ_TIMEOUT;
	sta->deauth_reason = reason;
	sta->disconnect_source = DISCONNECT_SOURCE_LOCAL;
	ap_sta_set_authorized(hapd, sta, 0);
	hostapd_set_sta_flags(hapd, sta);
	sta->timeout_next = STA_REMOVE;
	wpa_printf(MSG_DEBUG, "%s: reschedule ap_handle_timer timeout "
		   "for " MACSTR " (%d seconds - "
		   "AP_MAX_INACTIVITY_AFTER_DEAUTH)",
		   __func__, MAC2STR(sta->addr),
		   AP_MAX_INACTIVITY_AFTER_DEAUTH);
	eloop_cancel_timeout(ap_handle_timer, hapd, sta);
	eloop_register_timeout(AP_MAX_INACTIVITY_AFTER_DEAUTH, 0,
			       ap_handle_timer, hapd, sta);
	accounting_sta_stop(hapd, sta);
	ieee802_1x_free_station(hapd, sta);

	sta->flags |= WLAN_STA_PENDING_DEAUTH_CB;
	eloop_cancel_timeout(ap_sta_deauth_cb_timeout, hapd, sta);
	eloop_register_timeout(hapd->iface->drv_flags &
			       WPA_DRIVER_FLAGS_DEAUTH_TX_STATUS ? 2 : 0, 0,
			       ap_sta_deauth_cb_timeout, hapd, sta);
}


#ifdef CONFIG_WPS
int ap_sta_wps_cancel(struct hostapd_data *hapd,
		      struct sta_info *sta, void *ctx)
{
	if (sta && (sta->flags & WLAN_STA_WPS)) {
		ap_sta_deauthenticate(hapd, sta,
				      WLAN_REASON_PREV_AUTH_NOT_VALID);
		wpa_printf(MSG_DEBUG, "WPS: %s: Deauth sta=" MACSTR,
			   __func__, MAC2STR(sta->addr));
		return 1;
	}

	return 0;
}
#endif /* CONFIG_WPS */


static int ap_sta_get_free_vlan_id(struct hostapd_data *hapd)
{
	struct hostapd_vlan *vlan;
	int vlan_id = MAX_VLAN_ID + 2;

retry:
	for (vlan = hapd->conf->vlan; vlan; vlan = vlan->next) {
		if (vlan->vlan_id == vlan_id) {
			vlan_id++;
			goto retry;
		}
	}
	return vlan_id;
}


int ap_sta_set_vlan(struct hostapd_data *hapd, struct sta_info *sta,
		    struct vlan_description *vlan_desc)
{
	struct hostapd_vlan *vlan = NULL, *wildcard_vlan = NULL;
	int old_vlan_id, vlan_id = 0, ret = 0;

	/* Check if there is something to do */
	if (hapd->conf->ssid.per_sta_vif && !sta->vlan_id) {
		/* This sta is lacking its own vif */
	} else if (hapd->conf->ssid.dynamic_vlan == DYNAMIC_VLAN_DISABLED &&
		   !hapd->conf->ssid.per_sta_vif && sta->vlan_id) {
		/* sta->vlan_id needs to be reset */
	} else if (!vlan_compare(vlan_desc, sta->vlan_desc)) {
		return 0; /* nothing to change */
	}

	/* Now the real VLAN changed or the STA just needs its own vif */
	if (hapd->conf->ssid.per_sta_vif) {
		/* Assign a new vif, always */
		/* find a free vlan_id sufficiently big */
		vlan_id = ap_sta_get_free_vlan_id(hapd);
		/* Get wildcard VLAN */
		for (vlan = hapd->conf->vlan; vlan; vlan = vlan->next) {
			if (vlan->vlan_id == VLAN_ID_WILDCARD)
				break;
		}
		if (!vlan) {
			hostapd_logger(hapd, sta->addr,
				       HOSTAPD_MODULE_IEEE80211,
				       HOSTAPD_LEVEL_DEBUG,
				       "per_sta_vif missing wildcard");
			vlan_id = 0;
			ret = -1;
			goto done;
		}
	} else if (vlan_desc && vlan_desc->notempty) {
		for (vlan = hapd->conf->vlan; vlan; vlan = vlan->next) {
			if (!vlan_compare(&vlan->vlan_desc, vlan_desc))
				break;
			if (vlan->vlan_id == VLAN_ID_WILDCARD)
				wildcard_vlan = vlan;
		}
		if (vlan) {
			vlan_id = vlan->vlan_id;
		} else if (wildcard_vlan) {
			vlan = wildcard_vlan;
			vlan_id = vlan_desc->untagged;
			if (vlan_desc->tagged[0]) {
				/* Tagged VLAN configuration */
				vlan_id = ap_sta_get_free_vlan_id(hapd);
			}
		} else {
			hostapd_logger(hapd, sta->addr,
				       HOSTAPD_MODULE_IEEE80211,
				       HOSTAPD_LEVEL_DEBUG,
				       "missing vlan and wildcard for vlan=%d%s",
				       vlan_desc->untagged,
				       vlan_desc->tagged[0] ? "+" : "");
			vlan_id = 0;
			ret = -1;
			goto done;
		}
	}

	if (vlan && vlan->vlan_id == VLAN_ID_WILDCARD) {
		vlan = vlan_add_dynamic(hapd, vlan, vlan_id, vlan_desc);
		if (vlan == NULL) {
			hostapd_logger(hapd, sta->addr,
				       HOSTAPD_MODULE_IEEE80211,
				       HOSTAPD_LEVEL_DEBUG,
				       "could not add dynamic VLAN interface for vlan=%d%s",
				       vlan_desc ? vlan_desc->untagged : -1,
				       (vlan_desc && vlan_desc->tagged[0]) ?
				       "+" : "");
			vlan_id = 0;
			ret = -1;
			goto done;
		}

		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_DEBUG,
			       "added new dynamic VLAN interface '%s'",
			       vlan->ifname);
	} else if (vlan && vlan->dynamic_vlan > 0) {
		vlan->dynamic_vlan++;
		hostapd_logger(hapd, sta->addr,
			       HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_DEBUG,
			       "updated existing dynamic VLAN interface '%s'",
			       vlan->ifname);
	}
done:
	old_vlan_id = sta->vlan_id;
	sta->vlan_id = vlan_id;
	sta->vlan_desc = vlan ? &vlan->vlan_desc : NULL;

	if (vlan_id != old_vlan_id && old_vlan_id)
		vlan_remove_dynamic(hapd, old_vlan_id);

	return ret;
}


int ap_sta_bind_vlan(struct hostapd_data *hapd, struct sta_info *sta)
{
#ifndef CONFIG_NO_VLAN
	const char *iface;
	struct hostapd_vlan *vlan = NULL;
	int ret;
	int old_vlanid = sta->vlan_id_bound;

	if ((sta->flags & WLAN_STA_WDS) && sta->vlan_id == 0) {
		wpa_printf(MSG_DEBUG,
			   "Do not override WDS VLAN assignment for STA "
			   MACSTR, MAC2STR(sta->addr));
		return 0;
	}

	iface = hapd->conf->iface;
	if (hapd->conf->ssid.vlan[0])
		iface = hapd->conf->ssid.vlan;

	if (sta->vlan_id > 0) {
		for (vlan = hapd->conf->vlan; vlan; vlan = vlan->next) {
			if (vlan->vlan_id == sta->vlan_id)
				break;
		}
		if (vlan)
			iface = vlan->ifname;
	}

	/*
	 * Do not increment ref counters if the VLAN ID remains same, but do
	 * not skip hostapd_drv_set_sta_vlan() as hostapd_drv_sta_remove() might
	 * have been called before.
	 */
	if (sta->vlan_id == old_vlanid)
		goto skip_counting;

	if (sta->vlan_id > 0 && vlan == NULL) {
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_DEBUG, "could not find VLAN for "
			       "binding station to (vlan_id=%d)",
			       sta->vlan_id);
		ret = -1;
		goto done;
	} else if (vlan && vlan->dynamic_vlan > 0) {
		vlan->dynamic_vlan++;
		hostapd_logger(hapd, sta->addr,
			       HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_DEBUG,
			       "updated existing dynamic VLAN interface '%s'",
			       iface);
	}

	/* ref counters have been increased, so mark the station */
	sta->vlan_id_bound = sta->vlan_id;

skip_counting:
	hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_DEBUG, "binding station to interface "
		       "'%s'", iface);

	if (wpa_auth_sta_set_vlan(sta->wpa_sm, sta->vlan_id) < 0)
		wpa_printf(MSG_INFO, "Failed to update VLAN-ID for WPA");

	ret = hostapd_drv_set_sta_vlan(iface, hapd, sta->addr, sta->vlan_id);
	if (ret < 0) {
		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_DEBUG, "could not bind the STA "
			       "entry to vlan_id=%d", sta->vlan_id);
	}

	/* During 1x reauth, if the vlan id changes, then remove the old id. */
	if (old_vlanid > 0 && old_vlanid != sta->vlan_id)
		vlan_remove_dynamic(hapd, old_vlanid);
done:

	return ret;
#else /* CONFIG_NO_VLAN */
	return 0;
#endif /* CONFIG_NO_VLAN */
}


int ap_check_sa_query_timeout(struct hostapd_data *hapd, struct sta_info *sta)
{
	u32 tu;
	struct os_reltime now, passed;
	os_get_reltime(&now);
	os_reltime_sub(&now, &sta->sa_query_start, &passed);
	tu = (passed.sec * 1000000 + passed.usec) / 1024;
	if (hapd->conf->assoc_sa_query_max_timeout < tu) {
		hostapd_logger(hapd, sta->addr,
			       HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_DEBUG,
			       "association SA Query timed out");
		sta->sa_query_timed_out = 1;
		os_free(sta->sa_query_trans_id);
		sta->sa_query_trans_id = NULL;
		sta->sa_query_count = 0;
		eloop_cancel_timeout(ap_sa_query_timer, hapd, sta);
		return 1;
	}

	return 0;
}


static void ap_sa_query_timer(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	struct sta_info *sta = timeout_ctx;
	unsigned int timeout, sec, usec;
	u8 *trans_id, *nbuf;

	wpa_printf(MSG_DEBUG, "%s: SA Query timer for STA " MACSTR
		   " (count=%d)",
		   hapd->conf->iface, MAC2STR(sta->addr), sta->sa_query_count);

	if (sta->sa_query_count > 0 &&
	    ap_check_sa_query_timeout(hapd, sta))
		return;
	if (sta->sa_query_count >= 1000)
		return;

	nbuf = os_realloc_array(sta->sa_query_trans_id,
				sta->sa_query_count + 1,
				WLAN_SA_QUERY_TR_ID_LEN);
	if (nbuf == NULL)
		return;
	if (sta->sa_query_count == 0) {
		/* Starting a new SA Query procedure */
		os_get_reltime(&sta->sa_query_start);
	}
	trans_id = nbuf + sta->sa_query_count * WLAN_SA_QUERY_TR_ID_LEN;
	sta->sa_query_trans_id = nbuf;
	sta->sa_query_count++;

	if (os_get_random(trans_id, WLAN_SA_QUERY_TR_ID_LEN) < 0) {
		/*
		 * We don't really care which ID is used here, so simply
		 * hardcode this if the mostly theoretical os_get_random()
		 * failure happens.
		 */
		trans_id[0] = 0x12;
		trans_id[1] = 0x34;
	}

	timeout = hapd->conf->assoc_sa_query_retry_timeout;
	sec = ((timeout / 1000) * 1024) / 1000;
	usec = (timeout % 1000) * 1024;
	eloop_register_timeout(sec, usec, ap_sa_query_timer, hapd, sta);

	hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_DEBUG,
		       "association SA Query attempt %d", sta->sa_query_count);

	ieee802_11_send_sa_query_req(hapd, sta->addr, trans_id);
}


void ap_sta_start_sa_query(struct hostapd_data *hapd, struct sta_info *sta)
{
	ap_sa_query_timer(hapd, sta);
}


void ap_sta_stop_sa_query(struct hostapd_data *hapd, struct sta_info *sta)
{
	eloop_cancel_timeout(ap_sa_query_timer, hapd, sta);
	os_free(sta->sa_query_trans_id);
	sta->sa_query_trans_id = NULL;
	sta->sa_query_count = 0;
}

const char * ap_sta_wpa_get_passphrase(struct hostapd_data *hapd,
				       struct sta_info *sta)
{
	struct hostapd_wpa_psk *psk;
	struct hostapd_ssid *ssid;
	const u8 *pmk;
	int pmk_len;

	ssid = &hapd->conf->ssid;

	pmk = wpa_auth_get_pmk(sta->wpa_sm, &pmk_len);
	if (!pmk || pmk_len != PMK_LEN)
		return NULL;

	for (psk = ssid->wpa_psk; psk; psk = psk->next)
		if (os_memcmp(pmk, psk->psk, PMK_LEN) == 0)
			break;
	if (!psk)
		return NULL;
	if (!psk || !psk->wpa_passphrase[0])
		return NULL;

	return psk->wpa_passphrase;
}

const char * ap_sta_wpa_get_keyid(struct hostapd_data *hapd,
				  struct sta_info *sta)
{
	struct hostapd_wpa_psk *psk;
	struct hostapd_ssid *ssid;
	const u8 *pmk;
	int pmk_len;

	ssid = &hapd->conf->ssid;

	pmk = wpa_auth_get_pmk(sta->wpa_sm, &pmk_len);
	if (!pmk || pmk_len != PMK_LEN)
		return NULL;

	for (psk = ssid->wpa_psk; psk; psk = psk->next)
		if (os_memcmp(pmk, psk->psk, PMK_LEN) == 0)
			break;
	if (!psk)
		return NULL;
	if (!psk || !psk->keyid[0])
		return NULL;

	return psk->keyid;
}

static void sta_get_phy_mode_ch_width(struct hostapd_data *hapd, struct sta_info *sta,
	unsigned int *phy_mode, unsigned int *max_ch_width)
{
	*phy_mode = PHY_TYPE_UNSPECIFIED;
	*max_ch_width = NR_CHAN_WIDTH_20;

	if (sta->he_capabilities) {
		u8 *he_phy = (u8*) &(sta->he_capabilities->he_phy_capab_info);
		*phy_mode = PHY_TYPE_HE;
		if (he_phy[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B0) /* 40 MHz */
			*max_ch_width = NR_CHAN_WIDTH_40;
		if (he_phy[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B1) /* 40 and 80 MHz */
			*max_ch_width = NR_CHAN_WIDTH_80;
		else {
			/* Non-AP STA with 20 MHz channel width (B1 = 0) */
			/* If 20 MHz In 40 MHz HE PPDU In 2.4 GHz subfield is 1 and B4 = 1,
			max channel width will be 40 MHz
			*/
			if ((he_phy[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B4) &&
				(he_phy[HE_PHYCAP_CAP8_IDX] & HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_2_4_GHZ_BAND))
				*max_ch_width = NR_CHAN_WIDTH_40;

			/* If 20 MHz In 160/80+80 MHz HE PPDU subfield is set to 0 and B5 = 1,
			max channel width will be 160 MHz otherwise 80 MHz
			*/
			if (he_phy[HE_PHYCAP_CAP8_IDX] & HE_PHY_CAP8_20MHZ_IN_160MHZ_HE_PPDU) {
				if (he_phy[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B5)
					*max_ch_width = NR_CHAN_WIDTH_160;
				else
					*max_ch_width = NR_CHAN_WIDTH_80;
			}
		}
		if ((he_phy[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2) || /* 160 MHz */
			(he_phy[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B3)) /* 160/80+80 MHz */
			*max_ch_width = NR_CHAN_WIDTH_160;

		return;
	}

	if (sta->vht_capabilities) {
		u8 seg0 = 0, seg1 = 0;
		*phy_mode = PHY_TYPE_VHT;
		if (sta->vht_operation) {
			switch (sta->vht_operation->vht_op_info_chwidth) {
			case 0:
				if (sta->ht_operation) { /* identify channel width from HT operation */
					if (sta->ht_operation->ht_param & HT_INFO_HT_PARAM_STA_CHNL_WIDTH)
						*max_ch_width = NR_CHAN_WIDTH_40;
					else
						*max_ch_width = NR_CHAN_WIDTH_20;
				}
				break;
			case 1:
				seg0 = sta->vht_operation->vht_op_info_chan_center_freq_seg0_idx;
				seg1 = sta->vht_operation->vht_op_info_chan_center_freq_seg1_idx;
				if (seg1 && abs(seg1 - seg0) == 8) {
					*max_ch_width = NR_CHAN_WIDTH_160;
				}
				else if (seg1) {
					/* 80P80 : not supported */
				}
				else {
					*max_ch_width = NR_CHAN_WIDTH_80;
				}
				break;
			case 2:
				*max_ch_width = NR_CHAN_WIDTH_160;
				break;
			case 3:
				/* 80P80 : not supported */
				break;
			default:
				break;
			}
		}
		return;
	}

	if (sta->ht_capabilities) {
		*phy_mode = PHY_TYPE_HT;
		int ht_ch_width = (le_to_host16(sta->ht_capabilities->ht_capabilities_info) & HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET);
		*max_ch_width = (ht_ch_width != 0) ? NR_CHAN_WIDTH_40 : NR_CHAN_WIDTH_20;
		return;
	}

	if (*phy_mode == PHY_TYPE_UNSPECIFIED) { /* is not set yet by vht/ht */
		if (hapd->iface->freq > UPPER_24G_FREQ_VALUE)
			*phy_mode = PHY_TYPE_OFDM;
		else {
			u8 rate1_set = 0, rate6_set = 0;
			int supp_rate = 0, i = 0;
			for (i = 0; i < sta->supported_rates_len; i++) {
				supp_rate = sta->supported_rates[i] & 0x7f;
				if(supp_rate > 22) {
					*phy_mode = PHY_TYPE_ERP;
					rate1_set = 1;
				}
				else {
					*phy_mode = PHY_TYPE_HRDSSS;
					rate6_set = 1;
				}
				if(rate1_set && rate6_set) {
					/* skip for now, enum doesn't cover BG mode */
					*phy_mode = PHY_TYPE_UNSPECIFIED;
					break;
				}
			}
		}
		*max_ch_width = NR_CHAN_WIDTH_20;
	}

	return;
}

/*
  Identify max MCS and corresponding NSS value
  for given HE MCS_NSS struct
*/
static void sta_get_he_mcs_nss(u8 *sta_he_mcs_nss, u8 *mcs, u8 *nss)
{
#define GET_HE_MCS_N_SS(n, m, sta_he_mcs_nss, mask, mcs, nss)	\
		do {													\
			u8 val = get_he_cap(sta_he_mcs_nss[m], mask);		\
			if (val != IEEE80211_HE_MCS_NOT_SUPPORTED) { 		\
				nss = n;										\
				if (val <= mcs) {								\
					mcs = val;									\
				}												\
			}													\
		} while(0)

	*mcs = 0;
	*nss = 0;

	GET_HE_MCS_N_SS(1, 0, sta_he_mcs_nss, HE_MCS_NSS_FOR_1SS, *mcs, *nss);
	GET_HE_MCS_N_SS(2, 0, sta_he_mcs_nss, HE_MCS_NSS_FOR_2SS, *mcs, *nss);
	GET_HE_MCS_N_SS(3, 0, sta_he_mcs_nss, HE_MCS_NSS_FOR_3SS, *mcs, *nss);
	GET_HE_MCS_N_SS(4, 0, sta_he_mcs_nss, HE_MCS_NSS_FOR_4SS, *mcs, *nss);
	GET_HE_MCS_N_SS(5, 1, sta_he_mcs_nss, HE_MCS_NSS_FOR_5SS, *mcs, *nss);
	GET_HE_MCS_N_SS(6, 1, sta_he_mcs_nss, HE_MCS_NSS_FOR_6SS, *mcs, *nss);
	GET_HE_MCS_N_SS(7, 1, sta_he_mcs_nss, HE_MCS_NSS_FOR_7SS, *mcs, *nss);
	GET_HE_MCS_N_SS(8, 1, sta_he_mcs_nss, HE_MCS_NSS_FOR_8SS, *mcs, *nss);
}

static void sta_get_mcs_nss(struct hostapd_data *hapd, struct sta_info *sta,
        unsigned int *max_mcs, unsigned int *max_streams)
{
	int i = 0;
	*max_mcs = *max_streams = 0;

	if (sta->he_capabilities) {
		/* array[0,1,2] = [TX,RX,max(TX,RX)] */
		u8 he_80_mcs_array[3] = {0}, he_160_mcs_array[3] = {0};
		u8 he_80_nss_array[3] = {0}, he_160_nss_array[3] = {0};
		u8 *sta_he_mcs_nss = NULL;

		/* STA TX/RX 80 */
		sta_he_mcs_nss = (u8 *)&sta->he_capabilities->optional[HE_MCS_NSS_TX_MCS_MAP_LESS_EQ_80_MHZ_PART1_IDX];
		sta_get_he_mcs_nss(sta_he_mcs_nss, &he_80_mcs_array[0], &he_80_nss_array[0]);
		sta_he_mcs_nss = (u8 *)&sta->he_capabilities->optional[HE_MCS_NSS_RX_MCS_MAP_LESS_EQ_80_MHZ_PART1_IDX];
		sta_get_he_mcs_nss(sta_he_mcs_nss, &he_80_mcs_array[1], &he_80_nss_array[1]);
		/* find out min MCS and NSS between TX/RX */
		DO_RELATE_OPER(he_80_mcs_array[2], he_80_mcs_array[0], he_80_mcs_array[1], <=)
		DO_RELATE_OPER(he_80_nss_array[2], he_80_nss_array[0], he_80_nss_array[1], <=)

		if (sta->he_capabilities->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2) {
			/* STA TX/RX 160 */
			sta_he_mcs_nss = (u8 *)&sta->he_capabilities->optional[HE_MCS_NSS_TX_MCS_MAP_160_MHZ_PART1_IDX];
			sta_get_he_mcs_nss(sta_he_mcs_nss, &he_160_mcs_array[0], &he_160_nss_array[0]);
			sta_he_mcs_nss = (u8 *)&sta->he_capabilities->optional[HE_MCS_NSS_RX_MCS_MAP_160_MHZ_PART1_IDX];
			sta_get_he_mcs_nss(sta_he_mcs_nss, &he_160_mcs_array[1], &he_160_nss_array[1]);
			/* find out min MCS and NSS between TX/RX */
			DO_RELATE_OPER(he_160_mcs_array[2], he_160_mcs_array[0], he_160_mcs_array[1], <=)
			DO_RELATE_OPER(he_160_nss_array[2], he_160_nss_array[0], he_160_nss_array[1], <=)

			/* it's not yet clear which of RX/TX MCS and NSS values needed by caller, so return min of the two for now */
			DO_RELATE_OPER(he_80_mcs_array[2], he_80_mcs_array[2], he_160_mcs_array[2], <)
			DO_RELATE_OPER(he_80_nss_array[2], he_80_nss_array[2], he_160_nss_array[2], <)
		}

		if (sta->he_capabilities->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B3) {
			/* 80P80 : not supported */
		}

		/* min MCS and NSS values after comparsion between different channel widths is stored in he_80_mcs/nss_array[2] */
		*max_mcs = he_80_mcs_array[2];
		*max_streams = he_80_nss_array[2];
		switch(*max_mcs) {
			case 0:
				*max_mcs = 7;
				break;
			case 1:
				*max_mcs = 9;
				break;
			case 2:
				*max_mcs = 11;
				break;
			default:
				break;
		}
		return;
	}
	if (sta->vht_capabilities) {
		int mcs_per_nss = 0, rx_map = 0, tx_map = 0;
		int mcs_array[2] = {0}, nss_array[2] = {0};
		tx_map = sta->vht_capabilities->vht_supported_mcs_set.tx_map;
		for (i = 0; i < VHT_RX_NSS_MAX_STREAMS; i++) { /* max streams same for RX/TX , ref : check_valid_vht_mcs */
			mcs_per_nss = (((1 << 2) - 1) & (tx_map >> ((i * 2) - 1))); /* extract 2-bits at a time (MCS per SS) */
			if (mcs_per_nss != 3) { /* SS not supported */
				nss_array[0] = i + 1;
				if (mcs_array[0] <= mcs_per_nss) {
					mcs_array[0] = mcs_per_nss;
				}
			}
		}
		rx_map = sta->vht_capabilities->vht_supported_mcs_set.rx_map;
		for (i = 0; i < VHT_RX_NSS_MAX_STREAMS; i++) {
			mcs_per_nss = (((1 << 2) - 1) & (rx_map >> ((i * 2) - 1))); /* extract 2-bits at a time (MCS per SS) */
			if (mcs_per_nss != 3) { /* SS not supported */
				nss_array[1] = i + 1;
				if (mcs_array[1] <= mcs_per_nss) {
					mcs_array[1] = mcs_per_nss;
				}
			}
		}
		DO_RELATE_OPER(*max_mcs, mcs_array[0], mcs_array[1], <)
		DO_RELATE_OPER(*max_streams, nss_array[0], nss_array[1], <)
		switch(*max_mcs) {
			case 0:
				*max_mcs = 7;
				break;
			case 1:
				*max_mcs = 8;
				break;
			case 2:
				*max_mcs = 9;
				break;
			default:
				break;
		}
		return;
	}
	if (sta->ht_capabilities) {
		int rx_nss = 0, tx_nss = 0, rx_mcs = 0;
		u8 tx_mcs_byte = 0, tx_mcs_set = 0, tx_rx_mcs_not_equal = 0;
		u8 rx_mcs_byte = 0;
		tx_mcs_byte = sta->ht_capabilities->supported_mcs_set[HT_CAP_SUPP_MCS_SET_TX_OCTET]; /* fetch 13th octet */
		tx_mcs_set = (tx_mcs_byte & HT_CAP_SUPP_MCS_SET_TX_MCS_SET) ? 1 : 0; /* TX MCS Set Defined */
		tx_rx_mcs_not_equal = (tx_mcs_byte & HT_CAP_SUPP_MCS_SET_TX_RX_MCS_SET_NOT_EQUAL) ? 1 : 0; /* TX RX MCS Set Not Equal*/
		tx_nss = 0;
		if (tx_mcs_set && tx_rx_mcs_not_equal) {
			/* extract TX NSS from bit 98 and 99 */
			tx_nss = ((tx_mcs_byte & HT_CAP_SUPP_MCS_SET_TX_MAX_NSS_SUPP) >> 4) + 1;
		}
		for (i = 0; i < HT_CAP_SUPP_MCS_SET_RX_OCTETS; i++) { /* parse first 4 octets for RX NSS */
			rx_mcs_byte = sta->ht_capabilities->supported_mcs_set[i];
			if (rx_mcs_byte & HT_CAP_SUPP_MCS_SET_RX_MASK) {
				rx_mcs = 0;
				rx_nss++;
				while (rx_mcs_byte >>= 1) /* identify highest order MCS */
					rx_mcs++;
				if (rx_mcs > *max_mcs)
					*max_mcs = rx_mcs;
			}
		}
		DO_RELATE_OPER(*max_streams, tx_nss, rx_nss, <) /* take min of TX/RX MCS */
	}

	return;
}

static const char *
_sta_mode_to_str (struct hostapd_iface *iface, struct sta_info *sta)
{
	int i;
	bool mode_b = false;

	if (sta->flags & (WLAN_STA_HE | WLAN_STA_HE_OPMODE_ENABLED))
		return "802.11ax";
	if (sta->flags & (WLAN_STA_VHT | WLAN_STA_VHT_OPMODE_ENABLED | WLAN_STA_VENDOR_VHT | WLAN_STA_VENDOR2_VHT))
		return "802.11ac";
	if (sta->flags & WLAN_STA_HT)
		return "802.11n";
	if (iface->current_mode->mode == HOSTAPD_MODE_IEEE80211A)
		return "802.11a";

	/* g or b */
	for (i = 0; i < sta->supported_rates_len; i++) {
		u8 rate_mbps, rate = sta->supported_rates[i];
		rate_mbps = (rate & 0x7f) >> 1u;
		switch (rate_mbps) {
			case 6u:
			case 9u:
			case 12u:
			case 18u:
			case 24u:
			case 36u:
			case 48u:
			case 54u:
				return "802.11g";
			case 1u:
			case 2u:
			case 11u:
				mode_b = true;
				break;
			case 5u:
				if(rate & 1) /* 0.5 */
					mode_b = true;
				break;
		}
	}
	return mode_b ? "802.11b" : "UNKNOWN";
}

void ap_sta_set_authorized(struct hostapd_data *hapd, struct sta_info *sta,
			   int authorized)
{
	const u8 *dev_addr = NULL;
	char *buf, *pos, *end;
	int buflen = STA_EVENT_MAX_BUF_LEN;
	char *sta_extra_cap = NULL;
	int ret = 0;
	int i;
	int len = 0;
	unsigned int max_ch_width = 0, phy_mode = 0;
	unsigned int mumimo = 0, max_mcs = 0, max_streams = 0;
	unsigned int rrm_cap = 0, band_2g_capable = 0, band_5g_capable = 0, band_6g_capable = 0;
#ifdef CONFIG_P2P
	u8 addr[ETH_ALEN];
	u8 ip_addr_buf[4];
#endif /* CONFIG_P2P */

	if (!!authorized == !!(sta->flags & WLAN_STA_AUTHORIZED))
		return;

	buf = os_malloc(buflen);
	if (!buf)
		return;
	pos = buf;
	end = buf + buflen;

	if (authorized)
		sta->flags |= WLAN_STA_AUTHORIZED;
	else
		sta->flags &= ~WLAN_STA_AUTHORIZED;

#ifdef CONFIG_P2P
	if (hapd->p2p_group == NULL) {
		if (sta->p2p_ie != NULL &&
		    p2p_parse_dev_addr_in_p2p_ie(sta->p2p_ie, addr) == 0)
			dev_addr = addr;
	} else
		dev_addr = p2p_group_get_dev_addr(hapd->p2p_group, sta->addr);

	if (dev_addr)
		ret = os_snprintf(pos, end - pos, MACSTR " p2p_dev_addr=" MACSTR,
				MAC2STR(sta->addr), MAC2STR(dev_addr));
		if (os_snprintf_error(end - pos, ret))
			goto free_alloc;
		pos += ret;
	else
#endif /* CONFIG_P2P */
	{
		/* Report disassociation following expired BSS Transition Management Request timeout
                 * in case STA didn't dis-associate on its own */
		if (!authorized && (sta->timeout_next == STA_DISASSOC_FROM_CLI) &&
			(sta->flags & WLAN_STA_BSS_TRANS_MGMT_REQ_TIMEOUT))
			ret = sprintf_s(pos, end - pos, MACSTR " DISASSOC_TIMER_EXPIRED", MAC2STR(sta->addr));
		else
			ret = sprintf_s(pos, end - pos, MACSTR, MAC2STR(sta->addr));
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			goto free_alloc;
		}
		pos += ret;
	}

	if (hapd->sta_authorized_cb)
		hapd->sta_authorized_cb(hapd->sta_authorized_cb_ctx,
					sta->addr, authorized, dev_addr);

	sta_extra_cap = os_malloc(STA_EXTRA_CAP_LEN);
	if (sta_extra_cap == NULL) {
		wpa_printf(MSG_ERROR, "malloc failed");
		goto free_alloc;
	}
	sta_extra_cap[0] = '\0';

	if (authorized) {
		const char *keyid;
		char keyid_buf[100];
		char ip_addr[100];
		keyid_buf[0] = '\0';
		char signal_strength[30];
		char *rates, *capabilities, *assoc_req;
		int assoc_req_len;
		int ret;
#ifdef CONFIG_MBO
		struct mbo_non_pref_chan_info *info;
#endif /* CONFIG_MBO */

		/* 4 symbols per rate */
		rates = os_malloc(WLAN_SUPP_RATES_MAX * 4);
		if (rates == NULL) {
			wpa_printf(MSG_ERROR, "malloc failed");
			goto free_alloc;
		}
		rates[0] = '\0';

		capabilities = os_malloc(120);
		if (capabilities == NULL) {
			wpa_printf(MSG_ERROR, "malloc failed");
			os_free(rates);
			goto free_alloc;
		}
		capabilities[0] = '\0';

		assoc_req_len = sizeof(" assoc_req=") + sta->last_assoc_req_len * 2;
		assoc_req = os_malloc(assoc_req_len);
		if (assoc_req == NULL) {
			wpa_printf(MSG_ERROR, "malloc failed");
			os_free(rates);
			os_free(capabilities);
			goto free_alloc;
		}
		assoc_req[0] = '\0';

		ip_addr[0] = '\0';
		signal_strength[0] = '\0';
#ifdef CONFIG_P2P
		if (wpa_auth_get_ip_addr(sta->wpa_sm, ip_addr_buf) == 0) {
			os_snprintf(ip_addr, sizeof(ip_addr),
				    " ip_addr=%u.%u.%u.%u",
				    ip_addr_buf[0], ip_addr_buf[1],
				    ip_addr_buf[2], ip_addr_buf[3]);
		}
#endif /* CONFIG_P2P */
		keyid = ap_sta_wpa_get_keyid(hapd, sta);
		if (keyid) {
			os_snprintf(keyid_buf, sizeof(keyid_buf),
				    " keyid=%s", keyid);
		}
		ret = sprintf_s(signal_strength, sizeof(signal_strength),
			  " SignalStrength=%d", (s32)sta->ssi_signal);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			os_free(rates);
			os_free(capabilities);
			os_free(assoc_req);
			goto free_alloc;
		}

		len = 0;
		for (i = 0; i < sta->supported_rates_len; i++) {
			if (i > 0)
				ret = sprintf_s(rates + len, WLAN_SUPP_RATES_MAX * 4 - len,
						" %d", sta->supported_rates[i]);
			 else
				ret = sprintf_s(rates + len, WLAN_SUPP_RATES_MAX * 4 - len,
						" SupportedRates=%d", sta->supported_rates[i]);
			if (ret <= 0) {
				wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
				os_free(rates);
				os_free(capabilities);
				os_free(assoc_req);
				goto free_alloc;
			}
			len += ret;
		}

		len = 0;
		if (sta->ht_capabilities) {
			ret = sprintf_s(capabilities + len, 120 - len, " HT_CAP=%04X",
					le_to_host16(sta->ht_capabilities->ht_capabilities_info));
			if (ret <= 0) {
				wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
				os_free(rates);
				os_free(capabilities);
				os_free(assoc_req);
				goto free_alloc;
			}
		len += ret;
			for (i = 0; i < sizeof(sta->ht_capabilities->supported_mcs_set) /
				sizeof(sta->ht_capabilities->supported_mcs_set[0]); i++) {
				if (i > 0)
					ret = sprintf_s(capabilities + len, 120 - len, " %02X",
							sta->ht_capabilities->supported_mcs_set[i]);
				else
					ret = sprintf_s(capabilities + len, 120 - len, " HT_MCS=%02X",
							sta->ht_capabilities->supported_mcs_set[i]);

				if (ret <= 0) {
					wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
					os_free(rates);
					os_free(capabilities);
					os_free(assoc_req);
					goto free_alloc;
				}
				len += ret;
			}
		}
		if (sta->vht_capabilities) {
			ret = sprintf_s(capabilities + len, 120 - len, " VHT_CAP=%08X",
					le_to_host32(sta->vht_capabilities->vht_capabilities_info));
			if (ret <= 0) {
				wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
				os_free(rates);
				os_free(capabilities);
				os_free(assoc_req);
				goto free_alloc;
			}
			len += ret;
			ret = sprintf_s(capabilities + len, 120 - len, " VHT_MCS=%04X %04X %04X %04X",
					le_to_host16(sta->vht_capabilities->vht_supported_mcs_set.rx_map),
					le_to_host16(sta->vht_capabilities->vht_supported_mcs_set.rx_highest),
					le_to_host16(sta->vht_capabilities->vht_supported_mcs_set.tx_map),
					le_to_host16(sta->vht_capabilities->vht_supported_mcs_set.tx_highest));
			if (ret <= 0) {
				wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
				os_free(rates);
				os_free(capabilities);
				os_free(assoc_req);
				goto free_alloc;
			}
			len += ret;
		}

		if (sta->last_assoc_req) {
			len = 0;
			ret = sprintf_s(assoc_req + len, assoc_req_len - len, " assoc_req=");
			if (ret <= 0) {
				wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
				os_free(rates);
				os_free(capabilities);
				os_free(assoc_req);
				goto free_alloc;
			}
			len += ret;
			for (i = 0; i < sta->last_assoc_req_len; i++) {
				ret = sprintf_s(assoc_req + len, assoc_req_len - len, "%02X",
						((u8*)sta->last_assoc_req)[i]);
				if (ret <= 0) {
					wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
					os_free(rates);
					os_free(capabilities);
					os_free(assoc_req);
					goto free_alloc;
				}
				len += ret;
			}
		}

		ret = sprintf_s(pos, end - pos,
				" nr_enabled=%u btm_supported=%u ",
				sta->rrm_enabled_capa[0] & WLAN_RRM_CAPS_NEIGHBOR_REPORT ? 1 : 0,
				sta->btm_supported);
		if (os_snprintf_error(end - pos, ret)) {
			os_free(rates);
			os_free(capabilities);
			os_free(assoc_req);
			goto free_alloc;
		}
		pos += ret;

#ifdef CONFIG_MBO
		ret = sprintf_s(pos, end - pos, "cell_capa=%d", sta->cell_capa);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			os_free(rates);
			os_free(capabilities);
			os_free(assoc_req);
			goto free_alloc;
		}
		pos += ret;

		for (info = sta->non_pref_chan; info; info = info->next) {
			ret = sprintf_s(pos, end - pos, " non_pref_chan=%u:",
					info->op_class);
			if (ret <= 0)
				break;
			pos += ret;
			for (i = 0; i < info->num_channels; i++) {
				ret = sprintf_s(pos, end - pos, "%u%s", info->channels[i],
						i + 1 < info->num_channels ? "," : ":");
				if (ret <= 0)
					break;
				pos += ret;
			}
		  ret = sprintf_s(pos, end - pos, "%u:%u ",
				  info->pref, info->reason_code);
		  if (ret <= 0)
			  break;
		  pos += ret;
		}
#endif

		len = 0;
		rrm_cap = (sta->capability & WLAN_CAPABILITY_RADIO_MEASUREMENT) ? 1 : 0;
		band_2g_capable = band_5g_capable = band_6g_capable = 0;

		if(is_6ghz_freq(hapd->iface->freq))
			band_6g_capable = 1;
		else if (hapd->iface->freq > UPPER_24G_FREQ_VALUE)
			band_5g_capable = 1;
		else
			band_2g_capable = 1;

		ret = os_snprintf(sta_extra_cap + len, STA_EXTRA_CAP_LEN - len, " rrm_supported=%d band_2g_capable=%d band_5g_capable=%d  band_6g_capable=%d ",
			rrm_cap, band_2g_capable, band_5g_capable, band_6g_capable);
		if (os_snprintf_error(STA_EXTRA_CAP_LEN - len, ret)) {
			os_free(rates);
			os_free(capabilities);
			os_free(assoc_req);
			goto free_alloc;
		}
		len += ret;
		sta_get_phy_mode_ch_width(hapd, sta, &phy_mode, &max_ch_width);
		if (sta->he_capabilities) {
			mumimo = (get_he_cap(sta->he_capabilities->he_phy_capab_info[HE_PHYCAP_CAP2_IDX], HE_PHY_CAP2_PARTIAL_BANDWIDTH_UL_MU_MIMO) ||
				get_he_cap(sta->he_capabilities->he_phy_capab_info[HE_PHYCAP_CAP2_IDX], HE_PHY_CAP2_FULL_BANDWIDTH_UL_MU_MIMO) ||
				get_he_cap(sta->he_capabilities->he_phy_capab_info[HE_PHYCAP_CAP6_IDX], HE_PHY_CAP6_PARTIAL_BANDWIDTH_DL_MU_MIMO));
		}
		sta_get_mcs_nss(hapd, sta, &max_mcs, &max_streams);
		ret = sprintf_s(sta_extra_cap + len, STA_EXTRA_CAP_LEN - len,
				"max_ch_width=%d max_streams=%d phy_mode=%d max_mcs=%d max_tx_power=%d mu_mimo=%d ",
				max_ch_width, max_streams, phy_mode, max_mcs, sta->max_tx_power, mumimo);
		if (ret <= 0) {
			os_free(rates);
			os_free(capabilities);
			os_free(assoc_req);
			goto free_alloc;
		}
		len += ret;
		ret = sprintf_s(sta_extra_cap + len, STA_EXTRA_CAP_LEN - len, "rrm_cap=%02x %02x %02x %02x %02x ",
				sta->rrm_enabled_capa[0], sta->rrm_enabled_capa[1], sta->rrm_enabled_capa[2],
				sta->rrm_enabled_capa[3], sta->rrm_enabled_capa[4]);
		if (ret <= 0) {
			os_free(rates);
			os_free(capabilities);
			os_free(assoc_req);
			goto free_alloc;
		}
		len += ret;
		if (sta->flags & WLAN_STA_MULTI_AP){
			ret = sprintf_s(sta_extra_cap + len, STA_EXTRA_CAP_LEN - len, "multi_ap_profile=%d ",
				sta->multi_ap_profile);
			if (ret <= 0) {
				os_free(rates);
				os_free(capabilities);
				os_free(assoc_req);
				goto free_alloc;
			}
                }
		len += ret;

		wpa_msg(hapd->msg_ctx, MSG_INFO, AP_STA_CONNECTED "%s%s%s%s%s%s%s%s",
			buf, ip_addr, signal_strength, rates, capabilities, assoc_req, sta_extra_cap, keyid_buf);

		if (hapd->msg_ctx_parent &&
		    hapd->msg_ctx_parent != hapd->msg_ctx)
			wpa_msg_no_global(hapd->msg_ctx_parent, MSG_INFO,
					  AP_STA_CONNECTED "%s%s%s%s%s%s",
					  buf, ip_addr, signal_strength, rates, capabilities, sta_extra_cap);

		os_free(rates);
		os_free(capabilities);
		os_free(assoc_req);
	} else {
		u16 reason = 0;
		enum sta_disconnect_type type = DISCONNECT_TYPE_UNKNOWN;

		if (hapd->iface->current_mode &&
		    hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211AD) {
			type = DISCONNECT_TYPE_DISASSOC;
		}

		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
				HOSTAPD_LEVEL_DEBUG, "STA disconnect invoked with deauth reason %d, disconnect reason %d, source %d",
				sta->deauth_reason, sta->disassoc_reason, sta->disconnect_source);

		/* values for deauth and diassoc reason parameters not set together */
		if (sta->deauth_reason) {
			reason = sta->deauth_reason;
			type = DISCONNECT_TYPE_DEAUTH;
		}
		if (sta->disassoc_reason) {
			reason = sta->disassoc_reason;
			type = DISCONNECT_TYPE_DISASSOC;
		}
		if (!sta->deauth_reason && !sta->disassoc_reason) {
			reason = WLAN_REASON_PREV_AUTH_NOT_VALID;
			type = DISCONNECT_TYPE_RECONNECT;
		}

		ret = sprintf_s(sta_extra_cap, STA_EXTRA_CAP_LEN,
				" reason=%d source=%d type=%d", reason, sta->disconnect_source, type);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			goto free_alloc;
		}

		{ /* Additional STA disconnect event parameters */
			struct intel_vendor_sta_info sta_info = {0};
			int encrypt_mode = hapd->conf->wpa_pairwise;
			int auth_mode = hapd->conf->wpa_key_mgmt;
			int proto = 0;
			char wireles_mode[32]  = "UNKNOWN";
			u32 last_tx_phy_rate   = 0;
			u32 last_rx_phy_rate   = 0;
			u32 max_supported_rate = 0;
			u32 sta_bandwidth      = 0;
			u32 tx_fail_pkts       = 0;
			u32 retrans_pkts       = 0;
			s32 signal_strength    = -128;

			if (!hostapd_drv_get_sta_measurements(hapd, sta->addr, &sta_info)) {
				last_tx_phy_rate   = sta_info.LastDataDownlinkRate / 1000;
				last_rx_phy_rate   = sta_info.LastDataUplinkRate / 1000;
				max_supported_rate = sta_info.MaxRate / 1000;
				tx_fail_pkts       = sta_info.ErrorsSent;
				retrans_pkts       = sta_info.RetryCount;
				signal_strength    = sta_info.SignalStrength;

				if (sta_info.RateInfoFlag) {
					sta_bandwidth = sta_info.RateCbwMHz;
				}
			}

			os_strlcpy(wireles_mode, _sta_mode_to_str(hapd->iface, sta), sizeof(wireles_mode));

			if (sta->wpa_sm) {
				encrypt_mode = wpa_auth_get_pairwise(sta->wpa_sm);
				auth_mode    = wpa_auth_sta_key_mgmt(sta->wpa_sm);
				proto        = wpa_auth_sta_wpa_version(sta->wpa_sm);
			}

			ret = sprintf_s(sta_extra_cap + ret, STA_EXTRA_CAP_LEN - ret,
					" wireles_mode=%s"
					" last_tx_phy_rate=%uMbps"
					" last_rx_phy_rate=%uMbps"
					" max_supported_rate=%uMbps"
					" encrypt_mode=%s"
					" auth_mode=%s"
					" RSSI=%d"
					" channel=%u"
					" bandwidth=%u"
					" tx_fail_pkts=%u"
					" retrans_pkts=%u",
					wireles_mode,
					last_tx_phy_rate,
					last_rx_phy_rate,
					max_supported_rate,
					wpa_cipher_txt(encrypt_mode),
					wpa_key_mgmt_txt(auth_mode, proto),
					signal_strength,
					hapd->iface->conf->channel,
					sta_bandwidth,
					tx_fail_pkts,
					retrans_pkts);

			if (ret <= 0) {
				wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
				goto free_alloc;
			}
		}

		hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
				HOSTAPD_LEVEL_INFO, "Generating AP_STA_DISCONNECTED event with %s%s", buf, sta_extra_cap);
		wpa_msg(hapd->msg_ctx, MSG_INFO, AP_STA_DISCONNECTED "%s%s", buf, sta_extra_cap);

		if (hapd->msg_ctx_parent &&
		    hapd->msg_ctx_parent != hapd->msg_ctx)
			wpa_msg_no_global(hapd->msg_ctx_parent, MSG_INFO,
					  AP_STA_DISCONNECTED "%s%s", buf, sta_extra_cap);

		sta->deauth_reason = sta->disassoc_reason = 0;
		sta->disconnect_source = DISCONNECT_SOURCE_UNKNOWN;
	}

	/* Update ATF quotas if station is already in driver */
	hostapd_drv_send_atf_quotas(hapd, sta->addr, 0, 0);

#ifdef CONFIG_FST
	if (hapd->iface->fst) {
		if (authorized)
			fst_notify_peer_connected(hapd->iface->fst, sta->addr);
		else
			fst_notify_peer_disconnected(hapd->iface->fst,
						     sta->addr);
	}
#endif /* CONFIG_FST */

free_alloc:
	os_free(buf);
	os_free(sta_extra_cap);
}

void ap_sta_disconnect(struct hostapd_data *hapd, struct sta_info *sta,
		       const u8 *addr, u16 reason)
{
	if (sta)
		wpa_printf(MSG_DEBUG, "%s: %s STA " MACSTR " reason=%u",
			   hapd->conf->iface, __func__, MAC2STR(sta->addr),
			   reason);
	else if (addr)
		wpa_printf(MSG_DEBUG, "%s: %s addr " MACSTR " reason=%u",
			   hapd->conf->iface, __func__, MAC2STR(addr),
			   reason);

	if (sta == NULL && addr)
		sta = ap_get_sta(hapd, addr);

	if (addr)
		hostapd_drv_sta_deauth(hapd, addr, reason);

	if (sta == NULL)
		return;
	sta->flags &= ~WLAN_STA_BSS_TRANS_MGMT_REQ_TIMEOUT;
	if (hapd->iface->current_mode &&
	    hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211AD) {
		/* Deauthentication is not used in DMG/IEEE 802.11ad;
		 * disassociate the STA instead. */
		sta->disassoc_reason = reason;
	}
	else {
		sta->deauth_reason = reason;
	}
	sta->disconnect_source = DISCONNECT_SOURCE_LOCAL;
	ap_sta_set_authorized(hapd, sta, 0);
	sta->flags &= ~(WLAN_STA_AUTH | WLAN_STA_ASSOC);
	hostapd_set_sta_flags(hapd, sta);
	wpa_auth_sm_event(sta->wpa_sm, WPA_DEAUTH);
	ieee802_1x_notify_port_enabled(sta->eapol_sm, 0);
	wpa_printf(MSG_DEBUG, "%s: %s: reschedule ap_handle_timer timeout "
		   "for " MACSTR " (%d seconds - "
		   "AP_MAX_INACTIVITY_AFTER_DEAUTH)",
		   hapd->conf->iface, __func__, MAC2STR(sta->addr),
		   AP_MAX_INACTIVITY_AFTER_DEAUTH);
	eloop_cancel_timeout(ap_handle_timer, hapd, sta);
	eloop_register_timeout(AP_MAX_INACTIVITY_AFTER_DEAUTH, 0,
			       ap_handle_timer, hapd, sta);
	sta->timeout_next = STA_REMOVE;

	if (hapd->iface->current_mode &&
	    hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211AD) {
		/* Deauthentication is not used in DMG/IEEE 802.11ad;
		 * disassociate the STA instead. */
		sta->flags |= WLAN_STA_PENDING_DISASSOC_CB;
		eloop_cancel_timeout(ap_sta_disassoc_cb_timeout, hapd, sta);
		eloop_register_timeout(hapd->iface->drv_flags &
				       WPA_DRIVER_FLAGS_DEAUTH_TX_STATUS ?
				       2 : 0, 0, ap_sta_disassoc_cb_timeout,
				       hapd, sta);
		return;
	}

	sta->flags |= WLAN_STA_PENDING_DEAUTH_CB;
	eloop_cancel_timeout(ap_sta_deauth_cb_timeout, hapd, sta);
	eloop_register_timeout(hapd->iface->drv_flags &
			       WPA_DRIVER_FLAGS_DEAUTH_TX_STATUS ? 2 : 0, 0,
			       ap_sta_deauth_cb_timeout, hapd, sta);

	if (reason == WLAN_REASON_IEEE_802_1X_AUTH_FAILED) {
		hostapd_event_ltq_report_event(hapd, WLAN_FC_STYPE_AUTH,
			WLAN_STATUS_UNSPECIFIED_FAILURE, sta->addr, true);
	} else if (reason == WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT) {
		hostapd_event_ltq_report_event(hapd, WLAN_MSG_TYPE_EAPOL,
			WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT, sta->addr, true);
	} else if (reason == WLAN_REASON_MICHAEL_MIC_FAILURE) {
		hostapd_event_ltq_report_event(hapd, WLAN_FC_STYPE_AUTH,
			WLAN_STATUS_UNKNOWN_PASSWORD_IDENTIFIER, sta->addr, true);
	}
}


void ap_sta_deauth_cb(struct hostapd_data *hapd, struct sta_info *sta)
{
	if (!(sta->flags & WLAN_STA_PENDING_DEAUTH_CB)) {
		wpa_printf(MSG_DEBUG, "Ignore deauth cb for test frame");
		return;
	}
	sta->flags &= ~WLAN_STA_PENDING_DEAUTH_CB;
	eloop_cancel_timeout(ap_sta_deauth_cb_timeout, hapd, sta);
	ap_sta_deauth_cb_timeout(hapd, sta);
}


void ap_sta_disassoc_cb(struct hostapd_data *hapd, struct sta_info *sta)
{
	if (!(sta->flags & WLAN_STA_PENDING_DISASSOC_CB)) {
		wpa_printf(MSG_DEBUG, "Ignore disassoc cb for test frame");
		return;
	}
	sta->flags &= ~WLAN_STA_PENDING_DISASSOC_CB;
	eloop_cancel_timeout(ap_sta_disassoc_cb_timeout, hapd, sta);
	ap_sta_disassoc_cb_timeout(hapd, sta);
}


void ap_sta_clear_disconnect_timeouts(struct hostapd_data *hapd,
				      struct sta_info *sta)
{
	if (eloop_cancel_timeout(ap_sta_deauth_cb_timeout, hapd, sta) > 0)
		wpa_printf(MSG_DEBUG,
			   "%s: Removed ap_sta_deauth_cb_timeout timeout for "
			   MACSTR,
			   hapd->conf->iface, MAC2STR(sta->addr));
	if (eloop_cancel_timeout(ap_sta_disassoc_cb_timeout, hapd, sta) > 0)
		wpa_printf(MSG_DEBUG,
			   "%s: Removed ap_sta_disassoc_cb_timeout timeout for "
			   MACSTR,
			   hapd->conf->iface, MAC2STR(sta->addr));
	if (eloop_cancel_timeout(ap_sta_delayed_1x_auth_fail_cb, hapd, sta) > 0)
	{
		wpa_printf(MSG_DEBUG,
			   "%s: Removed ap_sta_delayed_1x_auth_fail_cb timeout for "
			   MACSTR,
			   hapd->conf->iface, MAC2STR(sta->addr));
		if (sta->flags & WLAN_STA_WPS)
			hostapd_wps_eap_completed(hapd);
	}
}


int ap_sta_flags_txt(u32 flags, char *buf, size_t buflen)
{
#define STA_FLAGS_AX_STR "%s%s"
	int res;

	buf[0] = '\0';
	res = os_snprintf(buf, buflen, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s" STA_FLAGS_AX_STR,
			  (flags & WLAN_STA_AUTH ? "[AUTH]" : ""),
			  (flags & WLAN_STA_ASSOC ? "[ASSOC]" : ""),
			  (flags & WLAN_STA_AUTHORIZED ? "[AUTHORIZED]" : ""),
			  (flags & WLAN_STA_PENDING_POLL ? "[PENDING_POLL" :
			   ""),
			  (flags & WLAN_STA_SHORT_PREAMBLE ?
			   "[SHORT_PREAMBLE]" : ""),
			  (flags & WLAN_STA_PREAUTH ? "[PREAUTH]" : ""),
			  (flags & WLAN_STA_WMM ? "[WMM]" : ""),
			  (flags & WLAN_STA_MFP ? "[MFP]" : ""),
			  (flags & WLAN_STA_WPS ? "[WPS]" : ""),
			  (flags & WLAN_STA_MAYBE_WPS ? "[MAYBE_WPS]" : ""),
			  (flags & WLAN_STA_WDS ? "[WDS]" : ""),
			  (flags & WLAN_STA_NONERP ? "[NonERP]" : ""),
			  (flags & WLAN_STA_WPS2 ? "[WPS2]" : ""),
			  (flags & WLAN_STA_GAS ? "[GAS]" : ""),
			  (flags & WLAN_STA_HT ? "[HT]" : ""),
			  (flags & WLAN_STA_VHT ? "[VHT]" : ""),
			  (((flags & WLAN_STA_VENDOR_VHT) || (flags & WLAN_STA_VENDOR2_VHT)) ? "[VENDOR_VHT]" : ""),
			  (flags & WLAN_STA_6GHZ ? "[6GHZ]" : ""),
			  (flags & WLAN_STA_WNM_SLEEP_MODE ?
			   "[WNM_SLEEP_MODE]" : "")
			  ,(flags & WLAN_STA_HE_OPMODE_ENABLED ? "[HE_OPMODE_ENABLED]" : ""),
			  (flags & WLAN_STA_HE ? "[HE]" : ""));
	if (os_snprintf_error(buflen, res))
		res = -1;

	return res;
}


static void ap_sta_delayed_1x_auth_fail_cb(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	struct sta_info *sta = timeout_ctx;
	u16 reason;

	wpa_dbg(hapd->msg_ctx, MSG_DEBUG,
		"IEEE 802.1X: Scheduled disconnection of " MACSTR
		" after EAP-Failure", MAC2STR(sta->addr));

	reason = sta->disconnect_reason_code;
	if (!reason)
		reason = WLAN_REASON_IEEE_802_1X_AUTH_FAILED;
	ap_sta_disconnect(hapd, sta, sta->addr, reason);
	if (sta->flags & WLAN_STA_WPS)
		hostapd_wps_eap_completed(hapd);
}


void ap_sta_delayed_1x_auth_fail_disconnect(struct hostapd_data *hapd,
					    struct sta_info *sta)
{
	/* Update the failure station count in auth fail blacklist DB */
	if(hapd->conf->max_eap_failure && !(sta->flags & WLAN_STA_WPS))
	{
		wpa_dbg(hapd->msg_ctx, MSG_DEBUG,
			"ADD STA entry to auth fail list");
		ieee802_11_auth_fail_entry_add(hapd, sta->addr);
		if (!eloop_is_timeout_registered(ieee802_11_auth_fail_blacklist_flush,
					hapd, NULL)) {
			wpa_dbg(hapd->msg_ctx, MSG_DEBUG,
				"ADD timeout for auth fail list");
			eloop_register_timeout(hapd->conf->auth_fail_blacklist_duration, 0,
					ieee802_11_auth_fail_blacklist_flush, hapd, NULL);
		}
	}

	wpa_dbg(hapd->msg_ctx, MSG_DEBUG,
		"IEEE 802.1X: Force disconnection of " MACSTR
		" after EAP-Failure in 10 ms", MAC2STR(sta->addr));

	/*
	 * Add a small sleep to increase likelihood of previously requested
	 * EAP-Failure TX getting out before this should the driver reorder
	 * operations.
	 */
	eloop_cancel_timeout(ap_sta_delayed_1x_auth_fail_cb, hapd, sta);
	eloop_register_timeout(0, 10000, ap_sta_delayed_1x_auth_fail_cb,
			       hapd, sta);

}


int ap_sta_pending_delayed_1x_auth_fail_disconnect(struct hostapd_data *hapd,
						   struct sta_info *sta)
{
	return eloop_is_timeout_registered(ap_sta_delayed_1x_auth_fail_cb,
					   hapd, sta);
}
