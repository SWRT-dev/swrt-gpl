/*
 * hostapd / Initialization and configuration
 * Copyright (c) 2002-2019, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#ifdef CONFIG_SQLITE
#include <sqlite3.h>
#endif /* CONFIG_SQLITE */

#include "utils/common.h"
#include "utils/eloop.h"
#include "utils/crc32.h"
#include "common/ieee802_11_defs.h"
#include "common/wpa_ctrl.h"
#include "common/hw_features_common.h"
#include "drivers/driver.h"
#include "drivers/nl80211_copy.h"
#include "radius/radius_client.h"
#include "radius/radius_das.h"
#include "eap_server/tncs.h"
#include "eapol_auth/eapol_auth_sm.h"
#include "eapol_auth/eapol_auth_sm_i.h"
#include "fst/fst.h"
#include "hostapd.h"
#include "authsrv.h"
#include "sta_info.h"
#include "accounting.h"
#include "ap_list.h"
#include "beacon.h"
#include "ieee802_1x.h"
#include "ieee802_11_auth.h"
#include "vlan_init.h"
#include "wpa_auth.h"
#include "wps_hostapd.h"
#include "dpp_hostapd.h"
#include "gas_query_ap.h"
#include "hw_features.h"
#include "wpa_auth_glue.h"
#include "ap_drv_ops.h"
#include "ap_config.h"
#include "p2p_hostapd.h"
#include "gas_serv.h"
#include "dfs.h"
#include "ieee802_11.h"
#include "bss_load.h"
#include "x_snoop.h"
#include "dhcp_snoop.h"
#include "ndisc_snoop.h"
#include "neighbor_db.h"
#ifdef CONFIG_WDS_WPA
#include "wds_wpa.h"
#endif
#include "rrm.h"
#include "fils_hlp.h"
#include "acs.h"
#include "hs20.h"
#include "airtime_policy.h"
#include "wpa_auth_kay.h"
#include "drivers/linux_ioctl.h"

#include "utils/crc32.h"
#include "limits.h"

#ifdef CONFIG_DPP2
#include "common/dpp.h"
#endif
channel_pair g_channel_pairs_40_24G[NUM_CHAN_PAIRS_40_24G] =
{
	{  1,  5 },
	{  5,  1 },
	{  2,  6 },
	{  6,  2 },
	{  3,  7 },
	{  7,  3 },
	{  4,  8 },
	{  8,  4 },
	{  5,  9 },
	{  9,  5 },
	{  6, 10 },
	{ 10,  6 },
	{  7, 11 },
	{ 11,  7 },
	{  8, 12 },
	{ 12,  8 },
	{  9, 13 },
	{ 13,  9 }
};

channel_pair g_channel_pairs_40_5G[NUM_CHAN_PAIRS_40_5G] =
{
	{  36, 40 },
	{  40, 36 },
	{  44, 48 },
	{  48, 44 },
	{  52, 56 },
	{  56, 52 },
	{  60, 64 },
	{  64, 60 },
	{ 100,104 },
	{ 104,100 },
	{ 108,112 },
	{ 112,108 },
	{ 116,120 },
	{ 120,116 },
	{ 124,128 },
	{ 128,124 },
	{ 132,136 },
	{ 136,132 },
	{ 140,144 },
	{ 144,140 },
	{ 149,153 },
	{ 153,149 },
	{ 157,161 },
	{ 161,157 }
};

channel_80 g_channels80[NUM_CHAN_PAIRS_80] =
{
	{  42,  36, 40, 5000 + 36 * 5 },
	{  42,  40, 36, 5000 + 36 * 5 },
	{  42,  44, 48, 5000 + 36 * 5 },
	{  42,  48, 44, 5000 + 36 * 5 },

	{  58,  52, 56, 5000 + 52 * 5 },
	{  58,  56, 52, 5000 + 52 * 5 },
	{  58,  60, 64, 5000 + 52 * 5 },
	{  58,  64, 60, 5000 + 52 * 5 },

	{ 106, 100, 104, 5000 + 100 * 5 },
	{ 106, 104, 100, 5000 + 100 * 5 },
	{ 106, 108, 112, 5000 + 100 * 5 },
	{ 106, 112, 108, 5000 + 100 * 5 },

	{ 122, 116, 120, 5000 + 116 * 5 },
	{ 122, 120, 116, 5000 + 116 * 5 },
	{ 122, 124, 128, 5000 + 116 * 5 },
	{ 122, 128, 124, 5000 + 116 * 5 },

	{ 138, 132, 136, 5000 + 132 * 5 },
	{ 138, 136, 132, 5000 + 132 * 5 },
	{ 138, 140, 144, 5000 + 132 * 5 },
	{ 138, 144, 140, 5000 + 132 * 5 },

	{ 155, 149, 153, 5000 + 149 * 5 },
	{ 155, 153, 149, 5000 + 149 * 5 },
	{ 155, 157, 161, 5000 + 149 * 5 },
	{ 155, 161, 157, 5000 + 149 * 5 }
};

channel_160 g_channels160[NUM_CHAN_PAIRS_160] =
{
	{  50,  36,  40, 5000 +  36 * 5 },
	{  50,  40,  36, 5000 +  36 * 5 },
	{  50,  44,  48, 5000 +  36 * 5 },
	{  50,  48,  44, 5000 +  36 * 5 },
	{  50,  52,  56, 5000 +  36 * 5 },
	{  50,  56,  52, 5000 +  36 * 5 },
	{  50,  60,  64, 5000 +  36 * 5 },
	{  50,  64,  60, 5000 +  36 * 5 },

	{ 114, 100, 104, 5000 + 100 * 5 },
	{ 114, 104, 100, 5000 + 100 * 5 },
	{ 114, 108, 112, 5000 + 100 * 5 },
	{ 114, 112, 108, 5000 + 100 * 5 },
	{ 114, 116, 120, 5000 + 100 * 5 },
	{ 114, 120, 116, 5000 + 100 * 5 },
	{ 114, 124, 128, 5000 + 100 * 5 },
	{ 114, 128, 124, 5000 + 100 * 5 }
};

static int hostapd_flush_old_stations(struct hostapd_data *hapd, u16 reason);
#ifdef CONFIG_WEP
static int hostapd_setup_encryption(char *iface, struct hostapd_data *hapd);
static int hostapd_broadcast_wep_clear(struct hostapd_data *hapd);
#endif /* CONFIG_WEP */
static int setup_interface2(struct hostapd_iface *iface);
static void channel_list_update_timeout(void *eloop_ctx, void *timeout_ctx);
static void hostapd_interface_setup_failure_handler(void *eloop_ctx,
						    void *timeout_ctx);
static int hostapd_remove_bss(struct hostapd_iface *iface, unsigned int idx);
static void hostapd_deactivate_backhaul_ifaces(struct hostapd_data *hapd);


int hostapd_for_each_interface(struct hapd_interfaces *interfaces,
			       int (*cb)(struct hostapd_iface *iface,
					 void *ctx), void *ctx)
{
	size_t i;
	int ret;

	for (i = 0; i < interfaces->count; i++) {
		ret = cb(interfaces->iface[i], ctx);
		if (ret)
			return ret;
	}

	return 0;
}


void hostapd_reconfig_encryption(struct hostapd_data *hapd)
{
	if (hapd->wpa_auth)
		return;

	hostapd_set_privacy(hapd, 0);
#ifdef CONFIG_WEP
	hostapd_setup_encryption(hapd->conf->iface, hapd);
#endif /* CONFIG_WEP */
}


static void hostapd_reload_bss(struct hostapd_data *hapd)
{
	struct hostapd_ssid *ssid;

	if (!hapd->started)
		return;

	if (hapd->conf->wmm_enabled < 0)
		hapd->conf->wmm_enabled = hapd->iconf->ieee80211n;

#ifndef CONFIG_NO_RADIUS
	radius_client_reconfig(hapd->radius, hapd->conf->radius);
#endif /* CONFIG_NO_RADIUS */

	ssid = &hapd->conf->ssid;
	if (!ssid->wpa_psk_set && ssid->wpa_psk && !ssid->wpa_psk->next &&
	    ssid->wpa_passphrase_set && ssid->wpa_passphrase) {
		/*
		 * Force PSK to be derived again since SSID or passphrase may
		 * have changed.
		 */
		hostapd_config_clear_wpa_psk(&hapd->conf->ssid.wpa_psk);
	}
	if (hostapd_setup_wpa_psk(hapd->conf)) {
		wpa_printf(MSG_ERROR, "Failed to re-configure WPA PSK "
			   "after reloading configuration");
	}

	if (hapd->conf->ieee802_1x || hapd->conf->wpa)
		hostapd_set_drv_ieee8021x(hapd, hapd->conf->iface, 1);
	else
		hostapd_set_drv_ieee8021x(hapd, hapd->conf->iface, 0);

	if ((hapd->conf->wpa || hapd->conf->osen) && hapd->wpa_auth == NULL) {
		hostapd_setup_wpa(hapd);
		if (hapd->wpa_auth)
			wpa_init_keys(hapd->wpa_auth);
	} else if (hapd->conf->wpa) {
		const u8 *wpa_ie;
		size_t wpa_ie_len;
		hostapd_reconfig_wpa(hapd);
		wpa_ie = wpa_auth_get_wpa_ie(hapd->wpa_auth, &wpa_ie_len);
		if (hostapd_set_generic_elem(hapd, wpa_ie, wpa_ie_len))
			wpa_printf(MSG_ERROR, "Failed to configure WPA IE for "
				   "the kernel driver.");
	} else if (hapd->wpa_auth) {
		wpa_deinit(hapd->wpa_auth);
		hapd->wpa_auth = NULL;
		hostapd_set_privacy(hapd, 0);
#ifdef CONFIG_WEP
		hostapd_setup_encryption(hapd->conf->iface, hapd);
#endif /* CONFIG_WEP */
		hostapd_set_generic_elem(hapd, (u8 *) "", 0);
	}

	if(hapd->conf->rnr_auto_update
	    && auto_update_rnr_in_beacon(hapd->iface))
	  wpa_printf(MSG_ERROR, "Failed to automatically set RNR IE");

	ieee802_11_update_beacon(hapd);
	hostapd_update_wps(hapd);

	if (hapd->conf->ssid.ssid_set &&
	    hostapd_set_ssid(hapd, hapd->conf->ssid.ssid,
			     hapd->conf->ssid.ssid_len)) {
		wpa_printf(MSG_ERROR, "Could not set SSID for kernel driver");
		/* try to continue */
	}
	wpa_printf(MSG_DEBUG, "Reconfigured interface %s", hapd->conf->iface);
}


static void hostapd_clear_old(struct hostapd_iface *iface)
{
	size_t j;

	/*
	 * Deauthenticate all stations since the new configuration may not
	 * allow them to use the BSS anymore.
	 */
	for (j = 0; j < iface->num_bss; j++) {
		hostapd_flush_old_stations(iface->bss[j],
					   WLAN_REASON_PREV_AUTH_NOT_VALID);
#ifdef CONFIG_WEP
		hostapd_broadcast_wep_clear(iface->bss[j]);
#endif /* CONFIG_WEP */

#ifndef CONFIG_NO_RADIUS
		/* TODO: update dynamic data based on changed configuration
		 * items (e.g., open/close sockets, etc.) */
		radius_client_flush(iface->bss[j]->radius, 0);
#endif /* CONFIG_NO_RADIUS */
	}
}


void hostapd_ltq_clear_old(struct hostapd_iface *iface, s32 flush)
{
  struct hostapd_data *hapd = iface->bss[0];

  wpa_printf(MSG_DEBUG, "Clean up STA DB");
  if (flush == LTQ_FLUSH_RADIO) {
	iface->stas_free_reason = (s8)WLAN_STA_FREE_REASON_RECOVERY;
	hostapd_clear_old(iface);
	iface->stas_free_reason = (s8)0;
  } else {
    if (flush >= iface->num_bss) {
      wpa_printf(MSG_ERROR, "Invalid BSS index %d", flush);
      return;
    }
    hapd = iface->bss[flush];
    hostapd_flush_old_stations(hapd,
      WLAN_REASON_PREV_AUTH_NOT_VALID);
#ifdef CONFIG_WEP
    hostapd_broadcast_wep_clear(hapd);
#endif

#ifndef CONFIG_NO_RADIUS
  /* TODO: update dynamic data based on changed configuration
   * items (e.g., open/close sockets, etc.) */
    radius_client_flush(hapd->radius, 0);
#endif /* CONFIG_NO_RADIUS */
  }

  wpa_printf(MSG_DEBUG, "Send sync_done command");
	if (hapd->driver && hapd->driver->sync_done) {
		hapd->driver->sync_done(hapd->drv_priv);
  }
}


static int hostapd_iface_conf_changed(struct hostapd_config *newconf,
				      struct hostapd_config *oldconf)
{
	size_t i;

	if (newconf->num_bss != oldconf->num_bss)
		return 1;

	for (i = 0; i < newconf->num_bss; i++) {
		if (os_strcmp(newconf->bss[i]->iface,
			      oldconf->bss[i]->iface) != 0)
			return 1;
	}

	return 0;
}


int hostapd_reload_config(struct hostapd_iface *iface)
{
	struct hapd_interfaces *interfaces = iface->interfaces;
	struct hostapd_data *hapd = iface->bss[0];
	struct hostapd_config *newconf, *oldconf;
	size_t j;

	if (iface->config_fname == NULL) {
		/* Only in-memory config in use - assume it has been updated */
		hostapd_clear_old(iface);
		for (j = 0; j < iface->num_bss; j++)
			hostapd_reload_bss(iface->bss[j]);
		return 0;
	}

	if (iface->interfaces == NULL ||
	    iface->interfaces->config_read_cb == NULL)
		return -1;
	newconf = iface->interfaces->config_read_cb(iface->config_fname);
	if (newconf == NULL)
		return -1;

	hostapd_clear_old(iface);

	oldconf = hapd->iconf;
	if (hostapd_iface_conf_changed(newconf, oldconf)) {
		char *fname;
		int res;

		wpa_printf(MSG_DEBUG,
			   "Configuration changes include interface/BSS modification - force full disable+enable sequence");
		fname = os_strdup(iface->config_fname);
		if (!fname) {
			hostapd_config_free(newconf);
			return -1;
		}
		hostapd_remove_iface(interfaces, hapd->conf->iface);
		iface = hostapd_init(interfaces, fname);
		os_free(fname);
		hostapd_config_free(newconf);
		if (!iface) {
			wpa_printf(MSG_ERROR,
				   "Failed to initialize interface on config reload");
			return -1;
		}
		iface->interfaces = interfaces;
		interfaces->iface[interfaces->count] = iface;
		interfaces->count++;
		res = hostapd_enable_iface(iface);
		if (res < 0)
			wpa_printf(MSG_ERROR,
				   "Failed to enable interface on config reload");
		return res;
	}
	iface->conf = newconf;

	for (j = 0; j < iface->num_bss; j++) {
		hapd = iface->bss[j];
		hapd->iconf = newconf;
		hapd->iconf->channel = oldconf->channel;
		hapd->iconf->acs = oldconf->acs;
		hapd->iconf->secondary_channel = oldconf->secondary_channel;
		hapd->iconf->ieee80211n = oldconf->ieee80211n;
		hapd->iconf->ieee80211ac = oldconf->ieee80211ac;
		hapd->iconf->ht_capab = oldconf->ht_capab;
		hapd->iconf->ht_tx_bf_capab = oldconf->ht_tx_bf_capab;
		hapd->iconf->vht_capab = oldconf->vht_capab;
		hostapd_set_oper_chwidth(hapd->iconf, hostapd_get_oper_chwidth(oldconf));
		hostapd_set_oper_centr_freq_seg0_idx(hapd->iconf, hostapd_get_oper_centr_freq_seg0_idx(oldconf));
		hostapd_set_oper_centr_freq_seg1_idx(hapd->iconf, hostapd_get_oper_centr_freq_seg1_idx(oldconf));
		hapd->conf = newconf->bss[j];
		hostapd_reload_bss(hapd);
	}

	hostapd_config_free(oldconf);


	return 0;
}


#ifdef CONFIG_WEP

static void hostapd_broadcast_key_clear_iface(struct hostapd_data *hapd,
					      const char *ifname)
{
	int i;

	if (!ifname || !hapd->drv_priv)
		return;
	for (i = 0; i < NUM_WEP_KEYS; i++) {
		if (hostapd_drv_set_key(ifname, hapd, WPA_ALG_NONE, NULL, i, 0,
					0, NULL, 0, NULL, 0, KEY_FLAG_GROUP)) {
			wpa_printf(MSG_DEBUG, "Failed to clear default "
				   "encryption keys (ifname=%s keyidx=%d)",
				   ifname, i);
		}
	}
	if (hapd->conf->ieee80211w) {
		for (i = NUM_WEP_KEYS; i < NUM_WEP_KEYS + 2; i++) {
			if (hostapd_drv_set_key(ifname, hapd, WPA_ALG_NONE,
						NULL, i, 0, 0, NULL,
						0, NULL, 0, KEY_FLAG_GROUP)) {
				wpa_printf(MSG_DEBUG, "Failed to clear "
					   "default mgmt encryption keys "
					   "(ifname=%s keyidx=%d)", ifname, i);
			}
		}
	}
}


static int hostapd_broadcast_wep_clear(struct hostapd_data *hapd)
{
	hostapd_broadcast_key_clear_iface(hapd, hapd->conf->iface);
	return 0;
}


static int hostapd_broadcast_wep_set(struct hostapd_data *hapd)
{
	int errors = 0, idx;
	struct hostapd_ssid *ssid = &hapd->conf->ssid;

	idx = ssid->wep.idx;
	if (ssid->wep.default_len && ssid->wep.key[idx] &&
	    hostapd_drv_set_key(hapd->conf->iface,
				hapd, WPA_ALG_WEP, broadcast_ether_addr, idx, 0,
				1, NULL, 0, ssid->wep.key[idx],
				ssid->wep.len[idx],
				KEY_FLAG_GROUP_RX_TX_DEFAULT)) {
		wpa_printf(MSG_WARNING, "Could not set WEP encryption.");
		errors++;
	}

	return errors;
}

#endif /* CONFIG_WEP */


static void hostapd_free_hapd_data(struct hostapd_data *hapd)
{
	os_free(hapd->probereq_cb);
	hapd->probereq_cb = NULL;
	hapd->num_probereq_cb = 0;

#ifdef CONFIG_P2P
	wpabuf_free(hapd->p2p_beacon_ie);
	hapd->p2p_beacon_ie = NULL;
	wpabuf_free(hapd->p2p_probe_resp_ie);
	hapd->p2p_probe_resp_ie = NULL;
#endif /* CONFIG_P2P */

	if (!hapd->started) {
		wpa_printf(MSG_ERROR, "%s: Interface %s wasn't started",
			   __func__, hapd->conf ? hapd->conf->iface : "N/A");
		return;
	}
	hapd->started = 0;
	hapd->beacon_set_done = 0;

	wpa_printf(MSG_DEBUG, "%s(%s)", __func__, hapd->conf->iface);
#ifdef CONFIG_WDS_WPA
	ltq_wds_wpa_deinit(hapd);
#endif
	accounting_deinit(hapd);
	hostapd_deinit_wpa(hapd);
	vlan_deinit(hapd);
	hostapd_acl_deinit(hapd);
	hostapd_clean_auth_fail_list(hapd);
	if (!hapd->conf->disable_pmksa_caching && hapd->conf->pmksa_interval) {
		if (eloop_is_timeout_registered(ieee802_1x_pmksa_purge_cache,
			hapd, NULL))
			eloop_cancel_timeout(ieee802_1x_pmksa_purge_cache, hapd, NULL);
	}

#ifndef CONFIG_NO_RADIUS
	radius_client_deinit(hapd->radius);
	hapd->radius = NULL;
	radius_das_deinit(hapd->radius_das);
	hapd->radius_das = NULL;
#endif /* CONFIG_NO_RADIUS */

	hostapd_deinit_wps(hapd);
	ieee802_1x_dealloc_kay_sm_hapd(hapd);
#ifdef CONFIG_DPP
	hostapd_dpp_deinit(hapd);
	gas_query_ap_deinit(hapd->gas);
	hapd->gas = NULL;
#endif /* CONFIG_DPP */

	authsrv_deinit(hapd);

	if (hapd->conf->mesh_mode == MESH_MODE_BACKHAUL_AP ||
	    hapd->conf->mesh_mode == MESH_MODE_EXT_HYBRID)
			hostapd_deactivate_backhaul_ifaces(hapd);

	if (hapd->interface_added) {
		hapd->interface_added = 0;
		if (hostapd_if_remove(hapd, WPA_IF_AP_BSS, hapd->conf->iface)) {
			wpa_printf(MSG_WARNING,
				   "Failed to remove BSS interface %s",
				   hapd->conf->iface);
			hapd->interface_added = 1;
		} else {
			/*
			 * Since this was a dynamically added interface, the
			 * driver wrapper may have removed its internal instance
			 * and hapd->drv_priv is not valid anymore.
			 */
			hapd->drv_priv = NULL;
		}
	}

	wpabuf_free(hapd->time_adv);

#ifdef CONFIG_INTERWORKING
	gas_serv_deinit(hapd);
#endif /* CONFIG_INTERWORKING */

	bss_load_update_deinit(hapd);
	ndisc_snoop_deinit(hapd);
	dhcp_snoop_deinit(hapd);
	x_snoop_deinit(hapd);

#ifdef CONFIG_SQLITE
	bin_clear_free(hapd->tmp_eap_user.identity,
		       hapd->tmp_eap_user.identity_len);
	bin_clear_free(hapd->tmp_eap_user.password,
		       hapd->tmp_eap_user.password_len);
#endif /* CONFIG_SQLITE */

#ifdef CONFIG_MESH
	wpabuf_free(hapd->mesh_pending_auth);
	hapd->mesh_pending_auth = NULL;
#endif /* CONFIG_MESH */

	hostapd_clean_rrm(hapd);
	fils_hlp_deinit(hapd);

#ifdef CONFIG_SAE
	{
		struct hostapd_sae_commit_queue *q;

		while ((q = dl_list_first(&hapd->sae_commit_queue,
					  struct hostapd_sae_commit_queue,
					  list))) {
			dl_list_del(&q->list);
			os_free(q);
		}
	}
	eloop_cancel_timeout(auth_sae_process_commit, hapd, NULL);
#endif /* CONFIG_SAE */
}


/**
 * hostapd_cleanup - Per-BSS cleanup (deinitialization)
 * @hapd: Pointer to BSS data
 *
 * This function is used to free all per-BSS data structures and resources.
 * Most of the modules that are initialized in hostapd_setup_bss() are
 * deinitialized here.
 */
static void hostapd_cleanup(struct hostapd_data *hapd)
{
	wpa_printf(MSG_DEBUG, "%s(hapd=%p (%s))", __func__, hapd,
		   hapd->conf->iface);
	if (hapd->iface->interfaces &&
	    hapd->iface->interfaces->ctrl_iface_deinit) {
		wpa_msg(hapd->msg_ctx, MSG_INFO, WPA_EVENT_TERMINATING);
		hapd->iface->interfaces->ctrl_iface_deinit(hapd);
	}
	hostapd_free_hapd_data(hapd);
}


static void sta_track_deinit(struct hostapd_iface *iface)
{
	struct hostapd_sta_info *info;

	if (!iface->num_sta_seen)
		return;

	while ((info = dl_list_first(&iface->sta_seen, struct hostapd_sta_info,
				     list))) {
		dl_list_del(&info->list);
		iface->num_sta_seen--;
		sta_track_del(info);
	}
}

static void dfs_history_deinit (struct hostapd_iface *iface)
{
	hostapd_dfs_history_t *entry;

	if (!iface->dfs_hist_entry_cnt)
		return;

	while ((entry = dl_list_first(&iface->dfs_history, hostapd_dfs_history_t, list))) {
		dl_list_del(&entry->list);
		iface->dfs_hist_entry_cnt--;
		os_free(entry);
	}
}


static void hostapd_cleanup_iface_partial(struct hostapd_iface *iface)
{
	wpa_printf(MSG_DEBUG, "%s(%p)", __func__, iface);
#ifdef NEED_AP_MLME
	hostapd_stop_setup_timers(iface);
#endif /* NEED_AP_MLME */
	if (iface->current_mode)
		acs_cleanup(iface);
	hostapd_free_hw_features(iface->hw_features, iface->num_hw_features);
	iface->hw_features = NULL;
	iface->current_mode = NULL;
	os_free(iface->current_rates);
	iface->current_rates = NULL;
	os_free(iface->basic_rates);
	iface->basic_rates = NULL;
	ap_list_deinit(iface);
	sta_track_deinit(iface);
	airtime_policy_update_deinit(iface);
	iface->set_freq_done = 0;
}


/**
 * hostapd_cleanup_iface - Complete per-interface cleanup
 * @iface: Pointer to interface data
 *
 * This function is called after per-BSS data structures are deinitialized
 * with hostapd_cleanup().
 */
static void hostapd_cleanup_iface(struct hostapd_iface *iface)
{
	wpa_printf(MSG_DEBUG, "%s(%p)", __func__, iface);
	eloop_cancel_timeout(channel_list_update_timeout, iface, NULL);
	eloop_cancel_timeout(hostapd_interface_setup_failure_handler, iface,
			     NULL);

	hostapd_cleanup_iface_partial(iface);
	dfs_history_deinit(iface);
	hostapd_config_free(iface->conf);
	iface->conf = NULL;

	os_free(iface->config_fname);
	os_free(iface->bss);
	wpa_printf(MSG_DEBUG, "%s: free iface=%p", __func__, iface);
	os_free(iface);
}


#ifdef CONFIG_WEP

static void hostapd_clear_wep(struct hostapd_data *hapd)
{
	if (hapd->drv_priv && !hapd->iface->driver_ap_teardown && hapd->conf) {
		hostapd_set_privacy(hapd, 0);
		hostapd_broadcast_wep_clear(hapd);
	}
}


static int hostapd_setup_encryption(char *iface, struct hostapd_data *hapd)
{
	int i;

	hostapd_broadcast_wep_set(hapd);

	if (hapd->conf->ssid.wep.default_len) {
		hostapd_set_privacy(hapd, 1);
		return 0;
	}

	/*
	 * When IEEE 802.1X is not enabled, the driver may need to know how to
	 * set authentication algorithms for static WEP.
	 */
	hostapd_drv_set_authmode(hapd, hapd->conf->auth_algs);

	for (i = 0; i < 4; i++) {
		if (hapd->conf->ssid.wep.key[i] &&
		    hostapd_drv_set_key(iface, hapd, WPA_ALG_WEP, NULL, i, 0,
					i == hapd->conf->ssid.wep.idx, NULL, 0,
					hapd->conf->ssid.wep.key[i],
					hapd->conf->ssid.wep.len[i],
					i == hapd->conf->ssid.wep.idx ?
					KEY_FLAG_GROUP_RX_TX_DEFAULT :
					KEY_FLAG_GROUP_RX_TX)) {
			wpa_printf(MSG_WARNING, "Could not set WEP "
				   "encryption.");
			return -1;
		}
		if (hapd->conf->ssid.wep.key[i] &&
		    i == hapd->conf->ssid.wep.idx)
			hostapd_set_privacy(hapd, 1);
	}

	return 0;
}

#endif /* CONFIG_WEP */


static int hostapd_flush_old_stations(struct hostapd_data *hapd, u16 reason)
{
	int ret = 0;
	u8 addr[ETH_ALEN];

	if (hostapd_drv_none(hapd) || hapd->drv_priv == NULL)
		return 0;

	if (hapd->conf && hapd->conf->broadcast_deauth) {
		wpa_dbg(hapd->msg_ctx, MSG_DEBUG,
			"Deauthenticate all stations");
		hostapd_atf_clean_stations(hapd);
		os_memset(addr, 0xff, ETH_ALEN);
		hostapd_drv_sta_deauth(hapd, addr, reason);
	}
	hostapd_free_stas(hapd);

	return ret;
}


static void hostapd_notify_ltq_goes_down(struct hostapd_data *hapd)
{
	wpa_msg(hapd->msg_ctx, MSG_INFO, LTQ_IFACE_GO_DOWN);
}

static void hostapd_bss_deinit_no_free(struct hostapd_data *hapd)
{
	hostapd_notify_ltq_goes_down(hapd);
	hostapd_free_stas(hapd);
	hostapd_free_cached_events(hapd);
	hostapd_flush_old_stations(hapd, WLAN_REASON_DEAUTH_LEAVING);
#ifdef CONFIG_WEP
	hostapd_clear_wep(hapd);
#endif /* CONFIG_WEP */
}


/**
 * hostapd_validate_bssid_configuration - Validate BSSID configuration
 * @iface: Pointer to interface data
 * Returns: 0 on success, -1 on failure
 *
 * This function is used to validate that the configured BSSIDs are valid.
 */
static int hostapd_validate_bssid_configuration(struct hostapd_iface *iface)
{
	u8 mask[ETH_ALEN] = { 0 };
	struct hostapd_data *hapd = iface->bss[0];
	unsigned int i = iface->conf->num_bss, bits = 0, j;
	int auto_addr = 0;

	if (hostapd_drv_none(hapd))
		return 0;

	if (iface->conf->use_driver_iface_addr)
		return 0;

	/* Generate BSSID mask that is large enough to cover the BSSIDs. */

	/* Determine the bits necessary to cover the number of BSSIDs. */
	for (i--; i; i >>= 1)
		bits++;

	/* Determine the bits necessary to any configured BSSIDs,
	   if they are higher than the number of BSSIDs. */
	for (j = 0; j < iface->conf->num_bss; j++) {
		if (is_zero_ether_addr(iface->conf->bss[j]->bssid)) {
			if (j)
				auto_addr++;
			continue;
		}

		for (i = 0; i < ETH_ALEN; i++) {
			mask[i] |=
				iface->conf->bss[j]->bssid[i] ^
				hapd->own_addr[i];
		}
	}

	if (!auto_addr)
		goto skip_mask_ext;

	for (i = 0; i < ETH_ALEN && mask[i] == 0; i++)
		;
	j = 0;
	if (i < ETH_ALEN) {
		j = (5 - i) * 8;

		while (mask[i] != 0) {
			mask[i] >>= 1;
			j++;
		}
	}

	if (bits < j)
		bits = j;

	if (bits > 40) {
		wpa_printf(MSG_ERROR, "Too many bits in the BSSID mask (%u)",
			   bits);
		return -1;
	}

	os_memset(mask, 0xff, ETH_ALEN);
	j = bits / 8;
	for (i = 5; i > 5 - j; i--)
		mask[i] = 0;
	j = bits % 8;
	while (j) {
		j--;
		mask[i] <<= 1;
	}

skip_mask_ext:
	wpa_printf(MSG_DEBUG, "BSS count %lu, BSSID mask " MACSTR " (%d bits)",
		   (unsigned long) iface->conf->num_bss, MAC2STR(mask), bits);

	if (!auto_addr)
		return 0;

	for (i = 0; i < ETH_ALEN; i++) {
		if ((hapd->own_addr[i] & mask[i]) != hapd->own_addr[i]) {
			wpa_printf(MSG_ERROR, "Invalid BSSID mask " MACSTR
				   " for start address " MACSTR ".",
				   MAC2STR(mask), MAC2STR(hapd->own_addr));
			wpa_printf(MSG_ERROR, "Start address must be the "
				   "first address in the block (i.e., addr "
				   "AND mask == addr).");
			return -1;
		}
	}

	return 0;
}


static int mac_in_conf(struct hostapd_config *conf, const void *a)
{
	size_t i;

	for (i = 0; i < conf->num_bss; i++) {
		if (hostapd_mac_comp(conf->bss[i]->bssid, a) == 0) {
			return 1;
		}
	}

	return 0;
}


#ifndef CONFIG_NO_RADIUS

static int hostapd_das_nas_mismatch(struct hostapd_data *hapd,
				    struct radius_das_attrs *attr)
{
	if (attr->nas_identifier &&
	    (!hapd->conf->nas_identifier ||
	     os_strlen(hapd->conf->nas_identifier) !=
	     attr->nas_identifier_len ||
	     os_memcmp(hapd->conf->nas_identifier, attr->nas_identifier,
		       attr->nas_identifier_len) != 0)) {
		wpa_printf(MSG_DEBUG, "RADIUS DAS: NAS-Identifier mismatch");
		return 1;
	}

	if (attr->nas_ip_addr &&
	    (hapd->conf->own_ip_addr.af != AF_INET ||
	     os_memcmp(&hapd->conf->own_ip_addr.u.v4, attr->nas_ip_addr, 4) !=
	     0)) {
		wpa_printf(MSG_DEBUG, "RADIUS DAS: NAS-IP-Address mismatch");
		return 1;
	}

#ifdef CONFIG_IPV6
	if (attr->nas_ipv6_addr &&
	    (hapd->conf->own_ip_addr.af != AF_INET6 ||
	     os_memcmp(&hapd->conf->own_ip_addr.u.v6, attr->nas_ipv6_addr, 16)
	     != 0)) {
		wpa_printf(MSG_DEBUG, "RADIUS DAS: NAS-IPv6-Address mismatch");
		return 1;
	}
#endif /* CONFIG_IPV6 */

	return 0;
}


static struct sta_info * hostapd_das_find_sta(struct hostapd_data *hapd,
					      struct radius_das_attrs *attr,
					      int *multi)
{
	struct sta_info *selected, *sta;
	char buf[128];
	int num_attr = 0;
	int count;

	*multi = 0;

	for (sta = hapd->sta_list; sta; sta = sta->next)
		sta->radius_das_match = 1;

	if (attr->sta_addr) {
		num_attr++;
		sta = ap_get_sta(hapd, attr->sta_addr);
		if (!sta) {
			wpa_printf(MSG_DEBUG,
				   "RADIUS DAS: No Calling-Station-Id match");
			return NULL;
		}

		selected = sta;
		for (sta = hapd->sta_list; sta; sta = sta->next) {
			if (sta != selected)
				sta->radius_das_match = 0;
		}
		wpa_printf(MSG_DEBUG, "RADIUS DAS: Calling-Station-Id match");
	}

	if (attr->acct_session_id) {
		num_attr++;
		if (attr->acct_session_id_len != 16) {
			wpa_printf(MSG_DEBUG,
				   "RADIUS DAS: Acct-Session-Id cannot match");
			return NULL;
		}
		count = 0;

		for (sta = hapd->sta_list; sta; sta = sta->next) {
			if (!sta->radius_das_match)
				continue;
			os_snprintf(buf, sizeof(buf), "%016llX",
				    (unsigned long long) sta->acct_session_id);
			if (os_memcmp(attr->acct_session_id, buf, 16) != 0)
				sta->radius_das_match = 0;
			else
				count++;
		}

		if (count == 0) {
			wpa_printf(MSG_DEBUG,
				   "RADIUS DAS: No matches remaining after Acct-Session-Id check");
			return NULL;
		}
		wpa_printf(MSG_DEBUG, "RADIUS DAS: Acct-Session-Id match");
	}

	if (attr->acct_multi_session_id) {
		num_attr++;
		if (attr->acct_multi_session_id_len != 16) {
			wpa_printf(MSG_DEBUG,
				   "RADIUS DAS: Acct-Multi-Session-Id cannot match");
			return NULL;
		}
		count = 0;

		for (sta = hapd->sta_list; sta; sta = sta->next) {
			if (!sta->radius_das_match)
				continue;
			if (!sta->eapol_sm ||
			    !sta->eapol_sm->acct_multi_session_id) {
				sta->radius_das_match = 0;
				continue;
			}
			os_snprintf(buf, sizeof(buf), "%016llX",
				    (unsigned long long)
				    sta->eapol_sm->acct_multi_session_id);
			if (os_memcmp(attr->acct_multi_session_id, buf, 16) !=
			    0)
				sta->radius_das_match = 0;
			else
				count++;
		}

		if (count == 0) {
			wpa_printf(MSG_DEBUG,
				   "RADIUS DAS: No matches remaining after Acct-Multi-Session-Id check");
			return NULL;
		}
		wpa_printf(MSG_DEBUG,
			   "RADIUS DAS: Acct-Multi-Session-Id match");
	}

	if (attr->cui) {
		num_attr++;
		count = 0;

		for (sta = hapd->sta_list; sta; sta = sta->next) {
			struct wpabuf *cui;

			if (!sta->radius_das_match)
				continue;
			cui = ieee802_1x_get_radius_cui(sta->eapol_sm);
			if (!cui || wpabuf_len(cui) != attr->cui_len ||
			    os_memcmp(wpabuf_head(cui), attr->cui,
				      attr->cui_len) != 0)
				sta->radius_das_match = 0;
			else
				count++;
		}

		if (count == 0) {
			wpa_printf(MSG_DEBUG,
				   "RADIUS DAS: No matches remaining after Chargeable-User-Identity check");
			return NULL;
		}
		wpa_printf(MSG_DEBUG,
			   "RADIUS DAS: Chargeable-User-Identity match");
	}

	if (attr->user_name) {
		num_attr++;
		count = 0;

		for (sta = hapd->sta_list; sta; sta = sta->next) {
			u8 *identity;
			size_t identity_len;

			if (!sta->radius_das_match)
				continue;
			identity = ieee802_1x_get_identity(sta->eapol_sm,
							   &identity_len);
			if (!identity ||
			    identity_len != attr->user_name_len ||
			    os_memcmp(identity, attr->user_name, identity_len)
			    != 0)
				sta->radius_das_match = 0;
			else
				count++;
		}

		if (count == 0) {
			wpa_printf(MSG_DEBUG,
				   "RADIUS DAS: No matches remaining after User-Name check");
			return NULL;
		}
		wpa_printf(MSG_DEBUG,
			   "RADIUS DAS: User-Name match");
	}

	if (num_attr == 0) {
		/*
		 * In theory, we could match all current associations, but it
		 * seems safer to just reject requests that do not include any
		 * session identification attributes.
		 */
		wpa_printf(MSG_DEBUG,
			   "RADIUS DAS: No session identification attributes included");
		return NULL;
	}

	selected = NULL;
	for (sta = hapd->sta_list; sta; sta = sta->next) {
		if (sta->radius_das_match) {
			if (selected) {
				*multi = 1;
				return NULL;
			}
			selected = sta;
		}
	}

	return selected;
}


static int hostapd_das_disconnect_pmksa(struct hostapd_data *hapd,
					struct radius_das_attrs *attr)
{
	if (!hapd->wpa_auth)
		return -1;
	return wpa_auth_radius_das_disconnect_pmksa(hapd->wpa_auth, attr);
}


static enum radius_das_res
hostapd_das_disconnect(void *ctx, struct radius_das_attrs *attr)
{
	struct hostapd_data *hapd = ctx;
	struct sta_info *sta;
	int multi;

	if (hostapd_das_nas_mismatch(hapd, attr))
		return RADIUS_DAS_NAS_MISMATCH;

	sta = hostapd_das_find_sta(hapd, attr, &multi);
	if (sta == NULL) {
		if (multi) {
			wpa_printf(MSG_DEBUG,
				   "RADIUS DAS: Multiple sessions match - not supported");
			return RADIUS_DAS_MULTI_SESSION_MATCH;
		}
		if (hostapd_das_disconnect_pmksa(hapd, attr) == 0) {
			wpa_printf(MSG_DEBUG,
				   "RADIUS DAS: PMKSA cache entry matched");
			return RADIUS_DAS_SUCCESS;
		}
		wpa_printf(MSG_DEBUG, "RADIUS DAS: No matching session found");
		return RADIUS_DAS_SESSION_NOT_FOUND;
	}

	wpa_printf(MSG_DEBUG, "RADIUS DAS: Found a matching session " MACSTR
		   " - disconnecting", MAC2STR(sta->addr));
	wpa_auth_pmksa_remove(hapd->wpa_auth, sta->addr);

	hostapd_drv_sta_deauth(hapd, sta->addr,
			       WLAN_REASON_PREV_AUTH_NOT_VALID);
	ap_sta_deauthenticate(hapd, sta, WLAN_REASON_PREV_AUTH_NOT_VALID);

	return RADIUS_DAS_SUCCESS;
}


#ifdef CONFIG_HS20
static enum radius_das_res
hostapd_das_coa(void *ctx, struct radius_das_attrs *attr)
{
	struct hostapd_data *hapd = ctx;
	struct sta_info *sta;
	int multi;

	if (hostapd_das_nas_mismatch(hapd, attr))
		return RADIUS_DAS_NAS_MISMATCH;

	sta = hostapd_das_find_sta(hapd, attr, &multi);
	if (!sta) {
		if (multi) {
			wpa_printf(MSG_DEBUG,
				   "RADIUS DAS: Multiple sessions match - not supported");
			return RADIUS_DAS_MULTI_SESSION_MATCH;
		}
		wpa_printf(MSG_DEBUG, "RADIUS DAS: No matching session found");
		return RADIUS_DAS_SESSION_NOT_FOUND;
	}

	wpa_printf(MSG_DEBUG, "RADIUS DAS: Found a matching session " MACSTR
		   " - CoA", MAC2STR(sta->addr));

	if (attr->hs20_t_c_filtering) {
		if (attr->hs20_t_c_filtering[0] & BIT(0)) {
			wpa_printf(MSG_DEBUG,
				   "HS 2.0: Unexpected Terms and Conditions filtering required in CoA-Request");
			return RADIUS_DAS_COA_FAILED;
		}

		hs20_t_c_filtering(hapd, sta, 0);
	}

	return RADIUS_DAS_SUCCESS;
}
#else /* CONFIG_HS20 */
#define hostapd_das_coa NULL
#endif /* CONFIG_HS20 */


#ifdef CONFIG_SQLITE

static int db_table_exists(sqlite3 *db, const char *name)
{
	char cmd[128];

	os_snprintf(cmd, sizeof(cmd), "SELECT 1 FROM %s;", name);
	return sqlite3_exec(db, cmd, NULL, NULL, NULL) == SQLITE_OK;
}


static int db_table_create_radius_attributes(sqlite3 *db)
{
	char *err = NULL;
	const char *sql =
		"CREATE TABLE radius_attributes("
		" id INTEGER PRIMARY KEY,"
		" sta TEXT,"
		" reqtype TEXT,"
		" attr TEXT"
		");"
		"CREATE INDEX idx_sta_reqtype ON radius_attributes(sta,reqtype);";

	wpa_printf(MSG_DEBUG,
		   "Adding database table for RADIUS attribute information");
	if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
		wpa_printf(MSG_ERROR, "SQLite error: %s", err);
		sqlite3_free(err);
		return -1;
	}

	return 0;
}

#endif /* CONFIG_SQLITE */

#endif /* CONFIG_NO_RADIUS */

#ifdef CONFIG_DPP2
static void hostapd_bringup_dpp_configurator(struct hostapd_data *hapd,
					     struct hostapd_bss_config *conf)
{
#define MAX_CONF_LEN 256
#define MAX_PARAM_LEN 72
	int configurator_id = 0;
	char dpp_configurator_params[MAX_CONF_LEN] = {'\0'};
	char hex_ssid[MAX_PARAM_LEN] = {'\0'}, hex_pass[MAX_PARAM_LEN] = {'\0'}, security[MAX_PARAM_LEN] = {'\0'};

	configurator_id = dpp_configurator_add(hapd->iface->interfaces->dpp, " ");
	wpa_snprintf_hex(hex_ssid, MAX_PARAM_LEN, conf->ssid.ssid, conf->ssid.ssid_len);
	sprintf_s(security, 4, "sta");

	if((conf->wpa_key_mgmt & WPA_KEY_MGMT_PSK) || (conf->wpa_key_mgmt & WPA_KEY_MGMT_PSK_SHA256) || (conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_PSK))
	{
		sprintf_s(&security[strlen(security)], 5, "-psk");
		wpa_snprintf_hex(hex_pass, MAX_PARAM_LEN, (u8 *) conf->ssid.wpa_passphrase, os_strlen(conf->ssid.wpa_passphrase));
	}
	if((conf->wpa_key_mgmt & WPA_KEY_MGMT_SAE) || (conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_SAE))
	{
		sprintf_s(&security[os_strlen(security)], 5, "-sae");
		wpa_snprintf_hex(hex_pass, MAX_PARAM_LEN, (u8 *) conf->sae_passwords->password, os_strlen(conf->sae_passwords->password));
	}
	if(conf->wpa_key_mgmt & WPA_KEY_MGMT_DPP)
	{
		sprintf_s(&security[os_strlen(security)], 5, "-dpp");
	}
	if((conf->wpa_key_mgmt & WPA_KEY_MGMT_IEEE8021X) || (conf->wpa_key_mgmt & WPA_KEY_MGMT_IEEE8021X_SHA256))
	{
		sprintf_s(&security[os_strlen(security)], 7, "-dot1x");
	}
	if(os_strlen(security) == (sizeof("sta") - 1)) /*Not supported key-mgmt found ie security="sta" only, expected to be postfix by -psk/sae/dpp/dot1x*/
		return;
	if(hex_pass[0]) {
		sprintf_s(dpp_configurator_params, MAX_CONF_LEN, " configurator=%d conf=%s ssid=%s pass=%s ", configurator_id,
			security, hex_ssid, hex_pass);
	}
	else
		sprintf_s(dpp_configurator_params, MAX_CONF_LEN, " configurator=%d conf=%s ssid=%s ", configurator_id,
			security, hex_ssid);

	//dpp_bootstrap_set(hapd->iface->interfaces->dpp, configurator_id, dpp_configurator_params);

	if (hapd->dpp_configurator_params)
		os_free(hapd->dpp_configurator_params);
	if ( hapd->dpp_configurator_params = strdup(dpp_configurator_params) )
		wpa_printf(MSG_DEBUG, "%s %d", dpp_configurator_params, conf->wpa_key_mgmt);
	else
		wpa_printf(MSG_ERROR, "strdup failed for dpp_configurator_params");
}
#endif /* CONFIG_DPP2 */

static void hostapd_post_up_vendor_vap_cmd(struct hostapd_data *hapd, struct hostapd_bss_config *conf)
{
	errno_t err = EOK;
	bool he_cap_non_adv_override = false;
	int ap_exce_retry_limit;
	/* HE debug */
	if (hapd->iconf->enable_he_debug_mode) {
		struct intel_vendor_he_capa debug_data = {0};
		struct ieee80211_he_capabilities *he_capab = &hapd->iface->current_mode->he_capab;

		/* Sanity check */
		if (sizeof(he_capab->optional) < (sizeof(debug_data.he_txrx_mcs_support) + sizeof(debug_data.he_ppe_thresholds))) {
			wpa_printf(MSG_ERROR, "%s: unsufficient size to copy from", __func__);
			return;
		}

		err = memcpy_s(debug_data.he_mac_capab_info, sizeof(debug_data.he_mac_capab_info),
			       he_capab->he_mac_capab_info, sizeof(he_capab->he_mac_capab_info));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}

		err = memcpy_s(debug_data.he_phy_capab_info, sizeof(debug_data.he_phy_capab_info),
			       he_capab->he_phy_capab_info, sizeof(he_capab->he_phy_capab_info));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}

		err = memcpy_s(debug_data.he_txrx_mcs_support, sizeof(debug_data.he_txrx_mcs_support),
			       he_capab->optional, sizeof(debug_data.he_txrx_mcs_support));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}

		err = memcpy_s(debug_data.he_ppe_thresholds, sizeof(debug_data.he_ppe_thresholds),
			       he_capab->optional + HE_PPE_THRES_INDEX, sizeof(debug_data.he_ppe_thresholds));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}

		hostapd_drv_send_ltq_he_debug_mode_data(hapd, &debug_data);
	}

	if(conf->sAggrConfigSize){
		if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_AGGR_CONFIG,
				(u8*)conf->sAggrConfig, conf->sAggrConfigSize, NULL))
			wpa_printf(MSG_WARNING, "set sAggrConfig failed. sAggrConfigSize:%d", conf->sAggrConfigSize);
	}

	if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_PROTECTION_METHOD,
			(u8*)&conf->s11nProtection, sizeof(conf->s11nProtection), NULL))
		wpa_printf(MSG_WARNING, "set s11nProtection failed");

	if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_UDMA_API,
			(u8*)&conf->sUdmaEnabled, sizeof(conf->sUdmaEnabled), NULL))
		wpa_printf(MSG_WARNING, "set sUdmaEnabled failed");

	if(conf->sUdmaEnabled){
		if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_UDMA_VLAN_ID,
				(u8*)&conf->sUdmaVlanId, sizeof(conf->sUdmaVlanId), NULL))
			wpa_printf(MSG_WARNING, "set sUdmaVlanId failed");
	}

	/* Get non-advertised HE Capabilities from driver */
	if(hostapd_drv_get_he_non_advertised(hapd, &hapd->iface->conf->he_non_advertised_caps) < 0)
		wpa_printf(MSG_ERROR, "Failed to get non-advertised HE capabilities from driver");

	/* Override non-advertised if ones were set in config file */
	if (hapd->iconf->he_cap_punc_pream_rx_support_override) {
		clr_set_he_cap(&hapd->iface->conf->he_non_advertised_caps.he_phy_capab_info[HE_PHYCAP_CAP1_IDX],
			hapd->iconf->he_cap_non_adv_punc_pream_rx_support, HE_PHY_CAP1_PUN_PREAM_RX);
		he_cap_non_adv_override = true;
	}

	if (hapd->iconf->he_cap_non_adv_multi_bss_rx_control_support_override) {
		clr_set_he_cap(&hapd->iface->conf->he_non_advertised_caps.he_mac_capab_info[HE_MACCAP_CAP3_IDX],
			hapd->iconf->he_cap_non_adv_multi_bss_rx_control_support, HE_MAC_CAP3_RX_CONTROL_FRAME_TO_MULTIBSS);
		he_cap_non_adv_override = true;
	}

	/* Set non-advertised HE Capabilities to driver */
	if (he_cap_non_adv_override) {
		if (hostapd_drv_set_he_non_advertised(hapd, &hapd->iface->conf->he_non_advertised_caps) < 0)
			wpa_printf(MSG_ERROR, "Failed to set non-advertised HE capabilities in driver");
	}

	if (is_6ghz_op_class(hapd->iconf->op_class) && 
		((hapd->iconf->num_bss == MIN_NUM_BSS_IN_NON_MBSSID_SET) ||
		(hapd->iconf->multibss_enable))) {
			hostapd_send_fils_discovery_frame(hapd);
			if (hapd->iconf->unsolicited_frame_support) {
				if (hostapd_drv_set_unsolicited_frame(hapd, hapd->iconf->unsolicited_frame_support,
													  hapd->iconf->unsolicited_frame_duration)) {
					wpa_printf(MSG_DEBUG, "Unsolicited frame config failed");
				} else if (hapd->iconf->unsolicited_frame_support == FRAME_20TU_PROBE_RESPONSE_ENABLE) {
					hostapd_send_event_unsolicited_probe_response(hapd);
				}
			}
	}

	if (!conf->bss_idx) {
		if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_MU_OPERATION,
					(u8*)&hapd->iconf->sMuOperation, sizeof(hapd->iconf->sMuOperation), NULL))
			wpa_printf(MSG_WARNING, "set sMuOperation failed");
	}

	if (!conf->bss_idx) {
		if (hostapd_drv_set_whm_config(hapd, hapd->iconf->whm_cfg_flag))
			wpa_printf(MSG_DEBUG, "Set WHM Config failed");
	}

	if (hapd->conf->disassoc_low_ack) {
		ap_exce_retry_limit = conf->ap_retry_limit_exce;
		if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_AP_EXCE_RETRY_LIMIT,
			(u8*)&ap_exce_retry_limit, sizeof(ap_exce_retry_limit), NULL))
			wpa_printf(MSG_WARNING, "set excessive retry cfg failed");
	}
#ifdef CONFIG_DPP2
	hostapd_bringup_dpp_configurator(hapd, conf);
#endif /* CONFIG_DPP2*/

}

static void hostapd_set_debug_mode_he_cap(struct hostapd_data *hapd)
{
	struct ieee80211_he_capabilities *hw = &hapd->iface->current_mode->he_capab;
	struct ieee80211_he_capabilities *conf = &hapd->iconf->he_capab;
	struct he_override_hw_capab *override_cap_idx = &hapd->iconf->override_hw_capab;

	hostapd_set_debug_he_mac_capab_info(hw->he_mac_capab_info,
			conf->he_mac_capab_info, override_cap_idx);
	hostapd_set_debug_he_phy_capab_info(hw->he_phy_capab_info,
			conf->he_phy_capab_info, override_cap_idx);
	hostapd_set_debug_he_txrx_mcs_support(hw->optional,
			conf->optional, override_cap_idx);
	/* PPE thresholds for now in hostapd hw_mode and conf stored with static offset HE_PPE_THRES_INDEX */
	hostapd_set_debug_he_ppe_thresholds(hw->optional + HE_PPE_THRES_INDEX,
			conf->optional + HE_PPE_THRES_INDEX, override_cap_idx);
}

/*
IEEE P802.11ax/D7.0, September 2020
9.4.2.45 Multiple BSSID element

The BSSID(i) value corresponding to the ith BSSID in the multiple BSSID set is derived from a reference BSSID (REF_BSSID) as follows:
BSSID(i) = BSSID_A | BSSID_B
where
BSSID_A is ( REF_BSSID & ZERO[(47-n+1):47] )
BSSID_B is ( ZERO[0:(47-n)] & bin[( ( dec(REF_BSSID[(47-n+1):47]) + i) mod 2n), n] )
and
ZERO[b:c] denotes bits b to c inclusive of a 48-bit address set to 0
REF_BSSID[b:c] denotes bits b to c inclusive of the REF_BSSID address

Example:
Suppose the maximum number of BSSIDs in the multiple BSSID set is 8 (i.e., the MaxBSSID Indicator field (n) is set to 3)
and the transmitted (reference) BSSID is 8c:fd:0f:7f:1e:f5

BSSID(0): 8c:fd:0f:7f:1e:f5, BSSID(1): 8c:fd:0f:7f:1e:f6, BSSID(2) : 8c:fd:0f:7f:1e:f7,
BSSID(3): 8c:fd:0f:7f:1e:f0, BSSID(4): 8c:fd:0f:7f:1e:f1, ..., BSSID(7) : 8c:fd:0f:7f:1e:f4
*/
static bool hostapd_validate_multibssid_mac_addr(struct hostapd_data *hapd)
{
	u8 max_bssid = (1 << round_pow2_up(hapd->iconf->num_bss - 1));
	u8 bss_idx = hapd->conf->bss_idx;
	u8 exp_addr_lsb;
	u8 mask_l = (max_bssid - 1);
	u8 mask_h = ~mask_l;
	u8 exp_addr[ETH_ALEN];

	if (MASTER_VAP_BSS_IDX == hapd->conf->bss_idx)
		return true;
	memcpy_s(exp_addr, ETH_ALEN, hapd->iface->bss[MULTIBSS_REFERENCE_BSS_IDX]->own_addr, ETH_ALEN);
	exp_addr_lsb = ((exp_addr[ETH_ALEN - 1] + (bss_idx - 1)) & mask_l);
	exp_addr[ETH_ALEN - 1] &=  mask_h;
	exp_addr[ETH_ALEN - 1] |= exp_addr_lsb;

	if (!memcmp(exp_addr, hapd->own_addr, ETH_ALEN))
		return true;
	wpa_printf(MSG_ERROR, "Max BSSID = %d, Ref MAC addr is " MACSTR
						   ", Received MAC addr is " MACSTR
						   " but Expected MAC addr is " MACSTR
						   " for bss_idx %d",
						   max_bssid,
						   MAC2STR(hapd->iface->bss[MULTIBSS_REFERENCE_BSS_IDX]->own_addr),
						   MAC2STR(hapd->own_addr), MAC2STR(exp_addr), bss_idx);
	return false;
}

static int hostapd_pre_up_vendor_vap_cmd(struct hostapd_data *hapd)
{
	errno_t err;
	u8 multibss_vap_mode = NON_MULTIBSS_VAP_MODE;
	struct intel_vendor_he_oper he_operation_hw = {0};
	u8 he_oper_bss_color_disable_flag = (hapd->iface->conf->he_oper.bss_color_info & HE_OPERATION_BSS_COLOR_DISABLED);
	u8 he_oper_bss_color = (hapd->iface->conf->he_oper.bss_color_info & HE_OPERATION_BSS_COLOR);
	u8 he_oper_cap0_txop_rts_dur_th = get_he_cap(hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP0_IDX], HE_OPERATION_CAP0_TXOP_DUR_RTS_TH);
	u8 he_oper_cap1_txop_rts_dur_th = get_he_cap(hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX], HE_OPERATION_CAP1_TXOP_DUR_RTS_TH);
	u8 he_oper_cap1_vht_operation_information_present = get_he_cap(hapd->iface->conf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX], HE_OPERATION_CAP1_VHT_OPER_INFO_PRESENT);
	/* Send vendor command before BSS setup is completed */

	/* he_beacon and Multiple BSSID Set for 6GHz band*/
	if (is_6ghz_op_class(hapd->iconf->op_class)) {
		if (hostapd_drv_set_he_beacon(hapd, hapd->iconf->he_beacon) < 0) {
			wpa_printf(MSG_ERROR, "Failed to set he_beacon");
		}
		/* MultiBSSID Set enabled by default unless HE debug mode is enabled */
		if ((hapd->iconf->num_bss >= MIN_NUM_BSS_IN_MBSSID_SET) && (!hapd->iconf->override_6g_mbssid_default_mode)) {
			hapd->iconf->multibss_enable = 1;
			hapd->iconf->mbssid_aid_offset = MULTIBSS_AID_OFFSET;
			wpa_printf(MSG_DEBUG, "mbssid_aid_offset is set to (%d) for 6G interface",
								   hapd->iconf->mbssid_aid_offset);
		}
		/* FILS Discovery frame trannsmission enabled by default in case unsolicited frame tx is disabled */
		if (((hapd->iconf->num_bss == MIN_NUM_BSS_IN_NON_MBSSID_SET) || (hapd->iconf->multibss_enable)) &&
			 (MULTIBSS_REFERENCE_BSS_IDX == hapd->conf->bss_idx) &&
			 (!hapd->iconf->unsolicited_frame_support))
				hapd->iconf->unsolicited_frame_support = FILS_DISCOVERY_ENABLE;
	}

	/* Multiple BSSID */
	if(hapd->iconf->multibss_enable) {
		if (MULTIBSS_REFERENCE_BSS_IDX == hapd->conf->bss_idx) {
					multibss_vap_mode = MULTIBSS_TRANSMITTED_VAP_MODE;
		} else {
					multibss_vap_mode = MULTIBSS_NON_TRANSMITTED_VAP_MODE;
					if (!hostapd_validate_multibssid_mac_addr(hapd)) {
						wpa_printf(MSG_ERROR, "MAC address validation failure for bss_idx %d",
											   hapd->conf->bss_idx);
						return -1;
					}
		}

		if (hostapd_drv_set_mbssid_vap_mode(hapd, multibss_vap_mode) < 0) {
					 wpa_printf(MSG_ERROR, "Failed to set MBSSID VAP");
		}
		if (hostapd_drv_set_mbssid_num_vaps_in_group(hapd, hapd->iconf->num_bss - 1) < 0) {
					 wpa_printf(MSG_ERROR, "Failed to set MBSSID number of VAPs in group");
		}
	} else {
		/* Update to default value otherwise */
		if (hostapd_drv_set_mbssid_vap_mode(hapd, multibss_vap_mode) < 0) {
					 wpa_printf(MSG_ERROR, "Failed to set MBSSID VAP");
		}
		if (hostapd_drv_set_mbssid_num_vaps_in_group(hapd, 0) < 0) {
					 wpa_printf(MSG_ERROR, "Failed to set MBSSID number of VAPs in group");
		}
	}

	/* Management Frames Rate */
	if (hostapd_drv_set_management_frames_rate(hapd, (u8)hapd->conf->management_frames_rate) < 0)
		wpa_printf(MSG_ERROR, "Failed to set Management Frames Rate");

	if (!he_oper_bss_color) {
		if (hapd->iconf->hw_mode == HOSTAPD_MODE_IEEE80211A)
			he_oper_bss_color = HE_OPERATION_BSS_COLOR_5GHZ;
		else if ((hapd->iconf->hw_mode == HOSTAPD_MODE_IEEE80211G) || (hapd->iconf->hw_mode == HOSTAPD_MODE_IEEE80211B))
			he_oper_bss_color = HE_OPERATION_BSS_COLOR_2_4GHZ;
	}

	/* Get HE Operation from driver */
	if (hostapd_drv_get_he_operation(hapd, &he_operation_hw) < 0) {
		 wpa_printf(MSG_ERROR, "Failed to get HE Operation from driver");
	} else {
		/* Combine necessary HE operation got from driver with user configuration */
		clr_set_he_cap(&he_operation_hw.he_oper_params[HE_OPERATION_CAP0_IDX],
				he_oper_cap0_txop_rts_dur_th, HE_OPERATION_CAP0_TXOP_DUR_RTS_TH);
		clr_set_he_cap(&he_operation_hw.he_oper_params[HE_OPERATION_CAP1_IDX],
				he_oper_cap1_txop_rts_dur_th, HE_OPERATION_CAP1_TXOP_DUR_RTS_TH);
		clr_set_he_cap(&he_operation_hw.he_oper_params[HE_OPERATION_CAP1_IDX],
				he_oper_cap1_vht_operation_information_present, HE_OPERATION_CAP1_VHT_OPER_INFO_PRESENT);

		he_operation_hw.he_bss_color_info = 0;
		he_operation_hw.he_bss_color_info |= (he_oper_bss_color & HE_OPERATION_BSS_COLOR);
		if (he_oper_bss_color_disable_flag)
			he_operation_hw.he_bss_color_info |= (HE_OPERATION_BSS_COLOR_DISABLED);
		else
			he_operation_hw.he_bss_color_info &= (~HE_OPERATION_BSS_COLOR_DISABLED);

		/* Write driver's and combined HE operation back to conf */
		hapd->iconf->he_oper.bss_color_info = he_operation_hw.he_bss_color_info;
		err = memcpy_s(hapd->iconf->he_oper.he_oper_params, sizeof(hapd->iconf->he_oper.he_oper_params),
			       he_operation_hw.he_oper_params, sizeof(he_operation_hw.he_oper_params));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return 0;
		}
		err = memcpy_s(hapd->iconf->he_oper.he_mcs_nss_set, sizeof(hapd->iconf->he_oper.he_mcs_nss_set),
			       he_operation_hw.he_mcs_nss_set, sizeof(he_operation_hw.he_mcs_nss_set));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return 0;
		}

		/* Check if Co-Hosted (a.k.a Co-Located) BSS field is present and set its value reported by driver */
		if(hapd->iconf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX] & HE_OPERATION_CAP1_CO_LOCATED_BSS)
			hapd->iconf->he_oper.max_co_located_bssid_ind = he_operation_hw.co_hosted_bss;

		/* Send back combined HE operation to driver */
		if (hostapd_drv_set_he_operation(hapd, &he_operation_hw) < 0)
			wpa_printf(MSG_ERROR, "Failed to set He Operation BSS color disable flag");

		/* In future process 6GHz info field here. */
	}

	/* Dynamic Multicast Rate */
	/* Send NL command to driver */
	if (hostapd_drv_set_dynamic_multicast_mode_rate(hapd,
				hapd->conf->dynamic_multicast_mode,
				hapd->conf->dynamic_multicast_rate) < 0) {
		wpa_printf(MSG_ERROR, "Failed to set Dynamic Multicast Rate");
	}
	/* Add next vendor command here */
	return 0;
}

/**
 * cred_request_timeout - Credential request timeout handler
 * when there are missing credentials for cred_req_timeout period
 * then the hostapd will terminate
 */
void cred_request_timeout(void *eloop_ctx, void *user_data)
{
	struct hostapd_data *hapd = eloop_ctx;
	wpa_msg(hapd->msg_ctx, MSG_ERROR,
		"no response to credentials request. hostapd will terminate\n");
	eloop_cancel_timeout(send_cred_request_handler, hapd, NULL);
	eloop_cancel_timeout(cred_request_timeout, hapd, 0);
	eloop_terminate();
}

/**
 * send_cred_request_handler - periodic timer handler to send credentials
 * request to the control interface for every 1 second If hostapd get of no
 * response to the request with in cred_req_timeout the hostapd will terminate
 * with error
 */
void send_cred_request_handler(void *eloop_ctx, void *user_data)
{
	struct hostapd_data *hapd = eloop_ctx;
	if (!eloop_is_timeout_registered(cred_request_timeout, hapd, NULL))
		return;
	wpa_msg(hapd->msg_ctx, MSG_INFO,
		"CREDENTIALS requested for BSS %s", hapd->conf->iface);
	eloop_cancel_timeout(send_cred_request_handler, hapd, NULL);
	eloop_register_timeout(1, 0, send_cred_request_handler, hapd, 0);
}

/**
 * send_cred_request_to_ui - send credentials request to the control interface
 * after X seconds of no response to the request the hostapd will terminate
 * with error
 */
void send_cred_request_to_ui(struct hostapd_data *hapd)
{
	const int time_to_request = hapd->iconf->cred_req_timeout; /* in seconds */

	if (!eloop_is_timeout_registered(cred_request_timeout, hapd, NULL)) {
		eloop_register_timeout(time_to_request, 0, cred_request_timeout, hapd, 0);
		eloop_register_timeout(1, 0, send_cred_request_handler, hapd, 0);
	}
}


static void wpa_msg_ap_state(struct hostapd_data *hapd, const char* event_str)
{
	if ((hapd->conf->mesh_mode == MESH_MODE_BACKHAUL_AP ||
	     hapd->conf->mesh_mode == MESH_MODE_EXT_HYBRID) &&
	    hapd->bkh_vrt_ifaces != NULL &&
	    hapd->num_bkh_vrt_ifaces > 0) {
		int i, written = 0, has_bkhs = 0;
		char *ifname;
		char *msg;
		rsize_t len;
		int ret;

		len = strnlen_s(event_str, RSIZE_MAX_STR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, RSIZE_MAX_STR - 1)) {
			wpa_printf(MSG_ERROR, "wpa_msg_ap_state -  wrong len");
			return;
		}
		/* AP_EVENT_ENABLED bAP_ifaces=b0_wlan0.1,b1_wlan0.1,b2_wlan0.1 */
		size_t size = (len +
			       ((hapd->num_bkh_vrt_ifaces) * (IFNAMSIZ + 1)) +
			       sizeof("bAP_ifaces=") + 2);

		msg = os_malloc(size * sizeof(char));
		if (NULL == msg) {
			wpa_msg(hapd->msg_ctx, MSG_INFO, "%s", event_str);
			return;
		}

		ret = sprintf_s(msg, size, "%s", event_str);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			os_free(msg);
			return;
		}
		written += ret;

		for (i = 0; i < hapd->num_bkh_vrt_ifaces; i++) {
			struct backhaul_virtual_iface *bkh_vrt_iface
					= hapd->bkh_vrt_ifaces[i];

			if (bkh_vrt_iface == NULL)
				continue;

			if (!has_bkhs) {
				ret = sprintf_s(msg + written,
						       size - written,
						       "bAP_ifaces=");
				if (ret <= 0) {
					wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
					os_free(msg);
					return;
				}
				written += ret;
			}
			has_bkhs = 1;

			ifname = bkh_vrt_iface->ifname;
			ret = sprintf_s(msg + written,
					       size - written, "%s,",
					       ifname);
			if (ret <= 0) {
				wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
				os_free(msg);
				return;
			}
			written += ret;
		}

		if (has_bkhs)
			msg[written - 1] = '\0';

		wpa_msg(hapd->msg_ctx, MSG_INFO, "%s", msg);
		os_free(msg);
	}
	else
		wpa_msg(hapd->msg_ctx, MSG_INFO, "%s", event_str);
}

void hostapd_send_event_unsolicited_probe_response(struct hostapd_data *hapd)
{
	wpa_msg(hapd->msg_ctx, MSG_INFO, AP_EVENT_20TU_PROBE_RESPONSE);
}

static void hostapd_handle_hairpin_mode (struct hostapd_data *hapd)
{
	if (hostapd_drv_br_port_set_attr(hapd, DRV_BR_PORT_ATTR_HAIRPIN_MODE, hapd->conf->enable_hairpin))
		wpa_printf(MSG_INFO, "%s: failed to set hairpin_mode to %d on the bridge port",
			   hapd->conf->iface, hapd->conf->enable_hairpin);

	wpa_msg(hapd->msg_ctx, MSG_INFO, AP_ENABLE_HAIRPIN_MODE "enable=%d", hapd->conf->enable_hairpin);
}

/**
 * hostapd_setup_bss - Per-BSS setup (initialization)
 * @hapd: Pointer to BSS data
 * @first: Whether this BSS is the first BSS of an interface; -1 = not first,
 *	but interface may exist
 *
 * This function is used to initialize all per-BSS data structures and
 * resources. This gets called in a loop for each BSS when an interface is
 * initialized. Most of the modules that are initialized here will be
 * deinitialized in hostapd_cleanup().
 */
int hostapd_setup_bss(struct hostapd_data *hapd, int first)
{
	struct hostapd_bss_config *conf = hapd->conf;
	u8 ssid[SSID_MAX_LEN + 1];
	int ssid_len, set_ssid;
	char force_ifname[IFNAMSIZ];
	u8 if_addr[ETH_ALEN] = {0};
	int flush_old_stations = 1;
	struct intel_vendor_sb_timer_acl_cfg sb_timer_acl_cfg = { 0 };

	/*
	 * when mem_only_cred configuration is set we will halt init of bss and request
	 * credentials from the UI
	 */
	if(conf->ssid.request_credentials)
	{
		send_cred_request_to_ui(hapd);
		return 0;
	}

	wpa_printf(MSG_DEBUG, "%s(hapd=%p (%s), first=%d)",
		   __func__, hapd, conf->iface, first);

#ifdef EAP_SERVER_TNC
	if (conf->tnc && tncs_global_init() < 0) {
		wpa_printf(MSG_ERROR, "Failed to initialize TNCS");
		return -1;
	}
#endif /* EAP_SERVER_TNC */

	if (hapd->started) {
		wpa_printf(MSG_ERROR, "%s: Interface %s was already started",
			   __func__, conf->iface);
		// return -1;  so what ?
	}
	hapd->started = 1;

	if (!first || first == -1) {
		u8 *addr = hapd->own_addr;

		if (!is_zero_ether_addr(conf->bssid)) {
			/* Allocate the configured BSSID. */
			os_memcpy(hapd->own_addr, conf->bssid, ETH_ALEN);

			if (hostapd_mac_comp(hapd->own_addr,
					     hapd->iface->bss[0]->own_addr) ==
			    0) {
				wpa_printf(MSG_ERROR, "BSS '%s' may not have "
					   "BSSID set to the MAC address of "
					   "the radio", conf->iface);
				return -1;
			}
		} else if (hapd->iconf->use_driver_iface_addr) {
			addr = NULL;
		} else {
			/* Allocate the next available BSSID. */
			do {
				inc_byte_array(hapd->own_addr, ETH_ALEN);
			} while (mac_in_conf(hapd->iconf, hapd->own_addr));
		}

		if (!hapd->interface_added && hostapd_if_add(hapd->iface->bss[0], WPA_IF_AP_BSS,
				   conf->iface, addr, hapd,
				   &hapd->drv_priv, force_ifname, if_addr,
				   conf->bridge[0] ? conf->bridge : NULL,
				   first == -1)) {
			wpa_printf(MSG_ERROR, "Failed to add BSS (BSSID="
				   MACSTR ")", MAC2STR(hapd->own_addr));
			return -1;
		}
		hapd->interface_added = 1;

		if (!addr)
			os_memcpy(hapd->own_addr, if_addr, ETH_ALEN);
	}

	/* He Debug mode */
	if (hapd->iconf->enable_he_debug_mode)
		hostapd_set_debug_mode_he_cap(hapd);

	if (hostapd_pre_up_vendor_vap_cmd(hapd))
		return -1;

	if (hostapd_drv_set_mesh_mode(hapd, hapd->conf->mesh_mode)) {
		wpa_printf(MSG_ERROR, "Failed to set mesh_mode");
		return -1;
	}

	/* if needed activate the extra backhaul interfaces and add them to a bridge */
	if ((hapd->conf->mesh_mode == MESH_MODE_BACKHAUL_AP ||
	     hapd->conf->mesh_mode == MESH_MODE_EXT_HYBRID)
	    && hostapd_activate_backhaul_ifaces(hapd)) {
		wpa_printf(MSG_ERROR, "Failed to activate extra backhaul ap interfaces");
		return -1;
	}

	if (hapd->conf->sFwrdUnkwnMcast != -1){
		if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_FORWARD_UNKNOWN_MCAST_FLAG,
					   (u8*)&hapd->conf->sFwrdUnkwnMcast,
					   sizeof(hapd->conf->sFwrdUnkwnMcast), NULL))
			wpa_printf(MSG_WARNING, "set sFwrdUnkwnMcast failed");
	}

	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_SOFTBLOCK_DISABLE,
			(u8*)&hapd->conf->sDisableSoftblock,
			sizeof(hapd->conf->sDisableSoftblock), NULL))
		wpa_printf(MSG_WARNING, "set sDisableSoftblock failed");

	sb_timer_acl_cfg.timer_acl_on = hapd->conf->soft_block_acl_enable;
	sb_timer_acl_cfg.timer_acl_probe_drop =
		(hapd->conf->soft_block_acl_on_probe_req == SOFT_BLOCK_ON_PROBE_REQ_REJECT_ALL) ? 1 : 0;
	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_SB_TIMER_ACL_CFG,
			(u8*)&sb_timer_acl_cfg, sizeof(sb_timer_acl_cfg), NULL))
		wpa_printf(MSG_WARNING, "SET_SB_TIMER_ACL_CFG failed");

	if (hapd->conf->sBridgeMode != -1){
		if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_BRIDGE_MODE,
				(u8*)&hapd->conf->sBridgeMode,
				sizeof(hapd->conf->sBridgeMode), NULL))
			wpa_printf(MSG_WARNING, "set sBridgeMode failed");
	}

	if (hapd->conf->num_sAddPeerAP > 0 && hapd->conf->sAddPeerAP != NULL)
	{
		int i;
		errno_t err;
		uint8_t data8[sizeof(uint16_t) + sizeof(macaddr)];
		uint8_t * data6 = data8 + sizeof(uint16_t);
		uint16_t sa_family = 1;
		err = memcpy_s(data8, sizeof(data8), &sa_family, sizeof(uint16_t));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return -1;
		}

		for (i=0; i < hapd->conf->num_sAddPeerAP; i++)
		{
			err = memcpy_s(data6, sizeof(macaddr), hapd->conf->sAddPeerAP[i], sizeof(macaddr));
			if (EOK != err) {
				wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
				return -1;
			}
			if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_ADD_PEERAP,
					(u8*)data8, sizeof(data8), NULL))
				wpa_printf(MSG_WARNING, "set sAddPeerAP failed");
		}
	}

	if (conf->wmm_enabled < 0)
		conf->wmm_enabled = hapd->iconf->ieee80211n;

#ifdef CONFIG_IEEE80211R_AP
	if (is_zero_ether_addr(conf->r1_key_holder))
		os_memcpy(conf->r1_key_holder, hapd->own_addr, ETH_ALEN);
#endif /* CONFIG_IEEE80211R_AP */

#ifdef CONFIG_MESH
	if ((hapd->conf->mesh & MESH_ENABLED) && hapd->iface->mconf == NULL)
		flush_old_stations = 0;
#endif /* CONFIG_MESH */

	if (flush_old_stations)
		hostapd_flush_old_stations(hapd,
					   WLAN_REASON_PREV_AUTH_NOT_VALID);
	hostapd_set_privacy(hapd, 0);

#ifdef CONFIG_WEP
	hostapd_broadcast_wep_clear(hapd);
	if (hostapd_setup_encryption(conf->iface, hapd))
		return -1;
#endif /* CONFIG_WEP */

	/*
	 * Fetch the SSID from the system and use it or,
	 * if one was specified in the config file, verify they
	 * match.
	 */
	ssid_len = hostapd_get_ssid(hapd, ssid, sizeof(ssid));
	if (ssid_len < 0) {
		wpa_printf(MSG_ERROR, "Could not read SSID from system");
		return -1;
	}
	if (conf->ssid.ssid_set) {
		/*
		 * If SSID is specified in the config file and it differs
		 * from what is being used then force installation of the
		 * new SSID.
		 */
		set_ssid = (conf->ssid.ssid_len != (size_t) ssid_len ||
			    os_memcmp(conf->ssid.ssid, ssid, ssid_len) != 0);
	} else {
		/*
		 * No SSID in the config file; just use the one we got
		 * from the system.
		 */
		set_ssid = 0;
		conf->ssid.ssid_len = ssid_len;
		os_memcpy(conf->ssid.ssid, ssid, conf->ssid.ssid_len);
	}

	/*
	 * Short SSID calculation is identical to FCS and it is defined in
	 * IEEE P802.11-REVmd/D3.0, 9.4.2.170.3 (Calculating the Short-SSID).
	 */
	conf->ssid.short_ssid = crc32(conf->ssid.ssid, conf->ssid.ssid_len);

	if (!hostapd_drv_none(hapd)) {
		wpa_printf(MSG_DEBUG, "Using interface %s with hwaddr " MACSTR
			   " and ssid \"%s\"",
			   conf->iface, MAC2STR(hapd->own_addr),
			   wpa_ssid_txt(conf->ssid.ssid, conf->ssid.ssid_len));
	}

	if (hostapd_setup_wpa_psk(conf)) {
		wpa_printf(MSG_ERROR, "WPA-PSK setup failed.");
		return -1;
	}

	/* Set SSID for the kernel driver (to be used in beacon and probe
	 * response frames) */
	if (set_ssid && hostapd_set_ssid(hapd, conf->ssid.ssid,
					 conf->ssid.ssid_len)) {
		wpa_printf(MSG_ERROR, "Could not set SSID for kernel driver");
		return -1;
	}

	if (wpa_debug_level <= MSG_MSGDUMP)
		conf->radius->msg_dumps = 1;
#ifndef CONFIG_NO_RADIUS

#ifdef CONFIG_SQLITE
	if (conf->radius_req_attr_sqlite) {
		if (sqlite3_open(conf->radius_req_attr_sqlite,
				 &hapd->rad_attr_db)) {
			wpa_printf(MSG_ERROR, "Could not open SQLite file '%s'",
				   conf->radius_req_attr_sqlite);
			return -1;
		}

		wpa_printf(MSG_DEBUG, "Opening RADIUS attribute database: %s",
			   conf->radius_req_attr_sqlite);
		if (!db_table_exists(hapd->rad_attr_db, "radius_attributes") &&
		    db_table_create_radius_attributes(hapd->rad_attr_db) < 0)
			return -1;
	}
#endif /* CONFIG_SQLITE */

	hapd->radius = radius_client_init(hapd, conf->radius);
	if (hapd->radius == NULL) {
		wpa_printf(MSG_ERROR, "RADIUS client initialization failed.");
		return -1;
	}

	if (conf->radius_das_port) {
		struct radius_das_conf das_conf;
		os_memset(&das_conf, 0, sizeof(das_conf));
		das_conf.port = conf->radius_das_port;
		das_conf.shared_secret = conf->radius_das_shared_secret;
		das_conf.shared_secret_len =
			conf->radius_das_shared_secret_len;
		das_conf.client_addr = &conf->radius_das_client_addr;
		das_conf.time_window = conf->radius_das_time_window;
		das_conf.require_event_timestamp =
			conf->radius_das_require_event_timestamp;
		das_conf.require_message_authenticator =
			conf->radius_das_require_message_authenticator;
		das_conf.ctx = hapd;
		das_conf.disconnect = hostapd_das_disconnect;
		das_conf.coa = hostapd_das_coa;
		hapd->radius_das = radius_das_init(&das_conf);
		if (hapd->radius_das == NULL) {
			wpa_printf(MSG_ERROR, "RADIUS DAS initialization "
				   "failed.");
			return -1;
		}
	}
#endif /* CONFIG_NO_RADIUS */

	if (hostapd_acl_init(hapd)) {
		wpa_printf(MSG_ERROR, "ACL initialization failed.");
		return -1;
	}
	if (hostapd_init_wps(hapd, conf))
		return -1;

#ifdef CONFIG_DPP
	hapd->gas = gas_query_ap_init(hapd, hapd->msg_ctx);
	if (!hapd->gas)
		return -1;
	if (hostapd_dpp_init(hapd))
		return -1;
#endif /* CONFIG_DPP */

	if (authsrv_init(hapd) < 0)
		return -1;

	if (ieee802_1x_init(hapd)) {
		wpa_printf(MSG_ERROR, "IEEE 802.1X initialization failed.");
		return -1;
	}

	if ((conf->wpa || conf->osen) && hostapd_setup_wpa(hapd))
		return -1;

	if (accounting_init(hapd)) {
		wpa_printf(MSG_ERROR, "Accounting initialization failed.");
		return -1;
	}

#ifdef CONFIG_INTERWORKING
	if (gas_serv_init(hapd)) {
		wpa_printf(MSG_ERROR, "GAS server initialization failed");
		return -1;
	}

	if (conf->qos_map_set_len &&
	    hostapd_drv_set_qos_map(hapd, conf->qos_map_set,
				    conf->qos_map_set_len)) {
		wpa_printf(MSG_ERROR, "Failed to initialize QoS Map");
		return -1;
	}
#endif /* CONFIG_INTERWORKING */

	if (conf->bss_load_update_period && bss_load_update_init(hapd)) {
		wpa_printf(MSG_ERROR, "BSS Load initialization failed");
		return -1;
	}

	if (conf->proxy_arp && conf->enable_snooping) {
		if (x_snoop_init(hapd)) {
			wpa_printf(MSG_ERROR,
				   "Generic snooping infrastructure initialization failed");
			return -1;
		}

		if (dhcp_snoop_init(hapd)) {
			wpa_printf(MSG_ERROR,
				   "DHCP snooping initialization failed");
			return -1;
		}

		if (ndisc_snoop_init(hapd)) {
			wpa_printf(MSG_ERROR,
				   "Neighbor Discovery snooping initialization failed");
			return -1;
		}
	}

	if (!hostapd_drv_none(hapd) && vlan_init(hapd)) {
		wpa_printf(MSG_ERROR, "VLAN initialization failed.");
		return -1;
	}
	if (conf->beacon_protection_enabled &&
		hapd->wpa_auth && wpa_init_bigtk_keys(hapd->wpa_auth) < 0)
		return -1;

	if(conf->rnr_auto_update && auto_update_rnr_in_beacon(hapd->iface) < 0)
		return -1;

	if (!conf->start_disabled && ieee802_11_set_beacon(hapd) < 0)
		return -1;

	if (hapd->wpa_auth && wpa_init_keys(hapd->wpa_auth) < 0)
		return -1;

	
#ifdef CONFIG_WDS_WPA
  if (ltq_wds_wpa_init(hapd)) {
    wpa_printf(MSG_ERROR, "WDS WPA initialization failed");
    return -1;
  }
#endif

	hostapd_neighbor_set_own_report(hapd);
	hostapd_neighbor_set_colocated_6ghz_report(hapd);

	if (hapd->driver && hapd->driver->set_operstate)
		hapd->driver->set_operstate(hapd->drv_priv, 1);

	hostapd_post_up_vendor_vap_cmd(hapd, conf);


	wpa_msg_ap_state(hapd, AP_EVENT_ENABLED);
	hostapd_handle_hairpin_mode(hapd);

	return 0;
}


int hostapd_activate_backhaul_ifaces(struct hostapd_data *hapd)
{
	char in_br[IFNAMSIZ];
	char *ifname;
	size_t alloc_size;
	int num_vrt_bkh_ifaces = 0;
	int fd, ret;
	uint8_t i;

	if (hapd->conf->mesh_mode != MESH_MODE_BACKHAUL_AP &&
	    hapd->conf->mesh_mode != MESH_MODE_EXT_HYBRID)
		return -1;

	num_vrt_bkh_ifaces = hapd->conf->mesh_mode == MESH_MODE_BACKHAUL_AP ?
                       (hapd->conf->max_num_sta - 1) : hapd->conf->num_vrt_bkh_netdevs;

	if (num_vrt_bkh_ifaces < 1)
		return 0;

	if (!IS_VALID_U8_RANGE(num_vrt_bkh_ifaces)) {
		wpa_printf(MSG_ERROR, "%s: num_vrt_bkh_ifaces is bigger than UINT8_MAX",  __func__);
		return -1;
	}

	if (strnlen_s(hapd->conf->iface, IFNAMSIZ) > TRUNCATE_BKH_VRT_IFACE) {
		wpa_printf(MSG_WARNING, "%s: new interface name will get truncated!", __func__);
	}

	if (NULL != hapd->bkh_vrt_ifaces)
		return 0;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		wpa_printf(MSG_ERROR, "%s: socket(AF_INET,SOCK_STREAM) "
			   "failed: %s", __func__, strerror(errno));
		return -1;
	}

	alloc_size = sizeof(struct backhaul_virtual_iface *) * num_vrt_bkh_ifaces;
	hapd->bkh_vrt_ifaces = os_malloc(alloc_size);
	if (NULL == hapd->bkh_vrt_ifaces)
		goto err;
	memset(hapd->bkh_vrt_ifaces, 0, alloc_size);
	hapd->num_bkh_vrt_ifaces = 0;

	for (i = 0; i < num_vrt_bkh_ifaces; ++i) {
		struct backhaul_virtual_iface * bkh_vrt_iface
				= os_malloc(sizeof(struct backhaul_virtual_iface));
		if (NULL == bkh_vrt_iface)
			goto err;
		hapd->bkh_vrt_ifaces[i] = bkh_vrt_iface;
		hapd->num_bkh_vrt_ifaces++;
		memset(bkh_vrt_iface, 0, sizeof(*bkh_vrt_iface));
		ret = sprintf_s(bkh_vrt_iface->ifname,
			    sizeof(bkh_vrt_iface->ifname),
			    "b%hhu_%.*s", i, TRUNCATE_BKH_VRT_IFACE, hapd->conf->iface);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			os_free(hapd->bkh_vrt_ifaces);
			return -1;
		}
		ifname = bkh_vrt_iface->ifname;

		if (hostapd_drv_add_backhaul_vrt_iface(hapd, ifname) < 0) {
			wpa_printf(MSG_ERROR, "Driver Could not create interface %s",
				   ifname);
			goto err;
		}

		if (linux_set_iface_flags(fd, ifname, 1) < 0) {
			wpa_printf(MSG_ERROR, "Could not set interface %s UP",
				   ifname);
			goto err;
		}
		bkh_vrt_iface->is_up = 1;

		if (!hapd->conf->bridge[0])
			continue; /* no need to add to bridge */

		if (linux_br_get(in_br, ifname) == 0) {
			if (os_strcmp(in_br, hapd->conf->bridge) == 0) {
				bkh_vrt_iface->added_to_bridge = 1;
				continue; /* already in the bridge */
			}

			if (linux_br_del_if(fd, in_br, ifname) < 0) {
				wpa_printf(MSG_ERROR, "nl80211: Failed to "
					   "remove interface %s from bridge "
					   "%s: %s", ifname, in_br, strerror(errno));
				goto err;
			}
		}

		if (linux_br_add_if(fd, hapd->conf->bridge, ifname) < 0) {
			wpa_printf(MSG_ERROR, "nl80211: Failed to add interface %s "
				   "into bridge %s: %s", ifname, hapd->conf->bridge,
				   strerror(errno));
			goto err;
		}
		bkh_vrt_iface->added_to_bridge = 1;
	}

	close(fd);
	return 0;

err:
	close(fd);
	hostapd_deactivate_backhaul_ifaces(hapd);
	return -1;
}


static void hostapd_deactivate_backhaul_ifaces(struct hostapd_data *hapd)
{
	char in_br[IFNAMSIZ];
	char *ifname;
	int fd;
	int i;

	if (NULL == hapd->bkh_vrt_ifaces)
		return;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		wpa_printf(MSG_ERROR, "%s: socket(AF_INET,SOCK_STREAM) "
			   "failed: %s", __func__, strerror(errno));
	}

	for (i = 0; i < hapd->num_bkh_vrt_ifaces; ++i) {
		struct backhaul_virtual_iface * bkh_vrt_iface
				= hapd->bkh_vrt_ifaces[i];

		if (NULL == bkh_vrt_iface)
			continue;

		ifname = bkh_vrt_iface->ifname;

		if (fd < 0)
			goto free;

		if (bkh_vrt_iface->is_up)
			linux_set_iface_flags(fd, ifname, 0);

		if (!hapd->conf->bridge[0] || !bkh_vrt_iface->added_to_bridge)
			goto free; /* no need to remove from bridge */

		if (linux_br_get(in_br, ifname) == 0 &&
		    os_strcmp(in_br, hapd->conf->bridge) == 0)
			/* remove only if in our bridge */
			linux_br_del_if(fd, in_br, ifname);

free:
		os_free(bkh_vrt_iface);
		hapd->bkh_vrt_ifaces[i] = NULL;
	}

	os_free(hapd->bkh_vrt_ifaces);
	hapd->bkh_vrt_ifaces = NULL;
	close(fd);
	return;
}


void hostapd_tx_queue_params(struct hostapd_data *hapd)
{
	struct hostapd_iface *iface = hapd->iface;
	int i;
	struct hostapd_tx_queue_params *p;

#ifdef CONFIG_MESH
	if ((hapd->conf->mesh & MESH_ENABLED) && iface->mconf == NULL)
		return;
#endif /* CONFIG_MESH */

	for (i = 0; i < NUM_TX_QUEUES; i++) {
		p = &iface->conf->tx_queue[i];

		if (hostapd_set_tx_queue_params(hapd, i, p->aifs, p->cwmin,
						p->cwmax, p->burst)) {
			wpa_printf(MSG_DEBUG, "Failed to set TX queue "
				   "parameters for queue %d.", i);
			/* Continue anyway */
		}
	}
}


static int hostapd_set_acl_list(struct hostapd_data *hapd,
				struct mac_acl_entry *mac_acl,
				int n_entries, u8 accept_acl)
{
	struct hostapd_acl_params *acl_params;
	int i, err;

	acl_params = os_zalloc(sizeof(*acl_params) +
			       (n_entries * sizeof(acl_params->mac_acl[0])));
	if (!acl_params)
		return -ENOMEM;

	for (i = 0; i < n_entries; i++)
		os_memcpy(acl_params->mac_acl[i].addr, mac_acl[i].addr,
			  ETH_ALEN);

	acl_params->acl_policy = accept_acl;
	acl_params->num_mac_acl = n_entries;

	err = hostapd_drv_set_acl(hapd, acl_params);

	os_free(acl_params);

	return err;
}


static void hostapd_set_acl(struct hostapd_data *hapd)
{
	struct hostapd_config *conf = hapd->iconf;
	int err;
	u8 accept_acl;

	if (hapd->iface->drv_max_acl_mac_addrs == 0)
		return;

	if (conf->bss[0]->macaddr_acl == DENY_UNLESS_ACCEPTED) {
		accept_acl = 1;
		err = hostapd_set_acl_list(hapd, conf->bss[0]->accept_mac,
					   conf->bss[0]->num_accept_mac,
					   accept_acl);
		if (err) {
			wpa_printf(MSG_DEBUG, "Failed to set accept acl");
			return;
		}
	} else if (conf->bss[0]->macaddr_acl == ACCEPT_UNLESS_DENIED) {
		accept_acl = 0;
		err = hostapd_set_acl_list(hapd, conf->bss[0]->deny_mac,
					   conf->bss[0]->num_deny_mac,
					   accept_acl);
		if (err) {
			wpa_printf(MSG_DEBUG, "Failed to set deny acl");
			return;
		}
	}
}


static int start_ctrl_iface_bss(struct hostapd_data *hapd)
{
	if (!hapd->iface->interfaces ||
	    !hapd->iface->interfaces->ctrl_iface_init)
		return 0;

	if (hapd->iface->interfaces->ctrl_iface_init(hapd)) {
		wpa_printf(MSG_ERROR,
			   "Failed to setup control interface for %s",
			   hapd->conf->iface);
		return -1;
	}

	return 0;
}


static int start_ctrl_iface(struct hostapd_iface *iface)
{
	size_t i;

	if (!iface->interfaces || !iface->interfaces->ctrl_iface_init)
		return 0;

	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *hapd = iface->bss[i];
		if (iface->interfaces->ctrl_iface_init(hapd)) {
			wpa_printf(MSG_ERROR,
				   "Failed to setup control interface for %s",
				   hapd->conf->iface);
			return -1;
		}
	}

	return 0;
}


static void channel_list_update_timeout(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_iface *iface = eloop_ctx;

	if (!iface->wait_channel_update) {
		wpa_printf(MSG_INFO, "Channel list update timeout, but interface was not waiting for it");
		return;
	}

	/*
	 * It is possible that the existing channel list is acceptable, so try
	 * to proceed.
	 */
	wpa_printf(MSG_DEBUG, "Channel list update timeout - try to continue anyway");
	setup_interface2(iface);
}

static int send_initial_params_to_driver (struct hostapd_data *hapd, const char *country)
{
	errno_t err;
	struct intel_vendor_initial_data_cfg data;

	os_memset(&data, 0, sizeof(data));
	err = memcpy_s(data.alpha2, sizeof(data.alpha2), country, 2); /* third char will not be sent */
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return -1;
	}
	data.is_11b = (HOSTAPD_MODE_IEEE80211B == hapd->iconf->hw_mode);
	data.radar_detection = hapd->iconf->ieee80211h;
	data.ieee80211n_acax_compat = hapd->iconf->ieee80211n_acax_compat;

	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_INITIAL_DATA,
						(const u8*)&data, sizeof(data), NULL)) {
		wpa_printf(MSG_ERROR, "Failed to send driver vendor command LTQ_NL80211_VENDOR_SUBCMD_INITIAL_DATA");
		return -1;
	}

	return 0;
}

static void
hostapd_conf_update_country_code (struct hostapd_iface *iface, const char *new_country)
{
	iface->conf->country[0] = new_country[0];
	iface->conf->country[1] = new_country[1];
}

static int hostapd_is_valid_country (const char *country)
{
	if (country[0] == '\0' || country[1] == '\0')
		return 0;

	/*	Special country code cases not supported:
	 *	00 - World regulatory domain
	 *	99 - built by driver but a specific alpha2 cannot be determined
	 *	98 - result of an intersection between two regulatory domains
	 *	97 - regulatory domain has not yet been configured */
	if ((country[0] == '0' && country[1] == '0') ||
	    (country[0] == '9' && country[1] == '9') ||
	    (country[0] == '9' && country[1] == '8') ||
	    (country[0] == '9' && country[1] == '7'))
		return 0;

	return 1;
}

static int
hostapd_change_country(struct hostapd_iface *iface, struct channel_list_changed *info)
{
	int ret;
	int n_chans, n_chans1;
	int start_chan_idx, start_chan_idx1;

	if (!hostapd_is_valid_country(info->alpha2))
		return 0;

	/* If this country already configured, nothing to do */
	if (os_strncmp(iface->conf->country, info->alpha2, 2) == 0)
		return 0;

	if (iface->state != HAPD_IFACE_ENABLED) {
		hostapd_conf_update_country_code(iface, info->alpha2);
		return 0;
	}

#ifdef CONFIG_ACS
	if (iface->conf->acs_algo == ACS_ALGO_SMART && iface->conf->acs_init_done) {
		wpa_printf(MSG_INFO, "Regdom change: will reset candidates and re-trigger ACS");
		iface->conf->acs_init_done = 0;
		iface->conf->channel = 0;

		hostapd_conf_update_country_code(iface, info->alpha2);

		iface->whm_if_fail = false;
		if (hostapd_disable_iface(iface) < 0)
			return -1;

		if (hostapd_enable_iface(iface) < 0)
			return -1;

		return 0;
	}
#endif

	/* Update channels and flags */
	if (hostapd_get_hw_features(iface)) {
		wpa_printf(MSG_ERROR, "Failed to get new regulatory data");
		return -1;
	}

	ret = hostapd_select_hw_mode(iface, 0);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "Could not select hw_mode");
		return -1;
	}

	hostapd_conf_update_country_code(iface, info->alpha2);

	/* Check if current channel definition still valid on new regdom */
	start_chan_idx = dfs_get_start_chan_idx(iface, &start_chan_idx1);
	n_chans = dfs_get_used_n_chans(iface, &n_chans1);

	if (!dfs_chan_range_available(iface->current_mode, start_chan_idx, n_chans, false)) {
		wpa_printf(MSG_INFO, "Regdom change: current chandef not allowed, switching channel");
		return hostapd_dfs_start_channel_switch(iface);
	}

	wpa_printf(MSG_INFO, "Regdom change: current chandef allowed, updating beacons");
	if (ieee802_11_update_beacons(iface))
		return -1;

	return 0;
}

void hostapd_channel_list_updated(struct hostapd_iface *iface, struct channel_list_changed *info)
{
	if (iface->conf->dynamic_country && !iface->wait_channel_update) {
		if (hostapd_change_country(iface, info)) {
			wpa_printf(MSG_ERROR, "Regdom change: country update failed");
			hostapd_disable_iface(iface);
		}

		return;
	}

	if (!iface->wait_channel_update || info->initiator != REGDOM_SET_BY_USER)
		return;

	wpa_printf(MSG_DEBUG, "Channel list updated - continue setup");
	eloop_cancel_timeout(channel_list_update_timeout, iface, NULL);
	setup_interface2(iface);
}

static int setup_interface(struct hostapd_iface *iface)
{
	struct hostapd_data *hapd = iface->bss[0];
	size_t i;

	/*
	 * It is possible that setup_interface() is called after the interface
	 * was disabled etc., in which case driver_ap_teardown is possibly set
	 * to 1. Clear it here so any other key/station deletion, which is not
	 * part of a teardown flow, would also call the relevant driver
	 * callbacks.
	 */
	iface->driver_ap_teardown = 0;

	/* WHM: set whm_if_fail initalized to TRUE to consider default disable
	 * interface call to be treated as failure. Good cases will be marked
	 * false as needed
	 */
	iface->whm_if_fail = true;

	if (!iface->phy[0]) {
		const char *phy = hostapd_drv_get_radio_name(hapd);
		if (phy) {
			wpa_printf(MSG_DEBUG, "phy: %s", phy);
			os_strlcpy(iface->phy, phy, sizeof(iface->phy));
		}
	}

	/*
	 * Make sure that all BSSes get configured with a pointer to the same
	 * driver interface.
	 */
	for (i = 1; i < iface->num_bss; i++) {
		iface->bss[i]->driver = hapd->driver;
		iface->bss[i]->drv_priv = hapd->drv_priv;
	}

	if (hostapd_validate_bssid_configuration(iface))
		return -1;

	/*
	 * Initialize control interfaces early to allow external monitoring of
	 * channel setup operations that may take considerable amount of time
	 * especially for DFS cases.
	 */
	if (start_ctrl_iface(iface))
		return -1;

	if (hostapd_pre_up_vendor_vap_cmd(hapd))
		return -1;

	if (hapd->iconf->country[0] && hapd->iconf->country[1]) {
		char country[4], previous_country[4];

		hostapd_set_state(iface, HAPD_IFACE_COUNTRY_UPDATE);
		if (hostapd_get_country(hapd, previous_country) < 0)
			previous_country[0] = '\0';

		if (hapd->iconf->dynamic_country &&
		    hostapd_is_valid_country(previous_country)) {
			os_memcpy(country, previous_country, 3);
			country[3] = '\0';

			hostapd_conf_update_country_code(iface, previous_country);

			wpa_printf(MSG_DEBUG, "Dynamic country enabled, staying on current country %s", country);
		}
		else {
			os_memcpy(country, hapd->iconf->country, 3);
			country[3] = '\0';

			if (hostapd_set_country(hapd, country) < 0) {
				wpa_printf(MSG_ERROR, "Failed to set country code");
				return -1;
			}

			wpa_printf(MSG_DEBUG, "Previous country code %s, new country code %s",
				   previous_country, country);
		}

		if (send_initial_params_to_driver(hapd, country))
			return -1;

		if (os_strncmp(previous_country, country, 2) != 0) {
			wpa_printf(MSG_DEBUG, "Continue interface setup after channel list update");
			iface->wait_channel_update = 1;
			eloop_register_timeout(5, 0,
					       channel_list_update_timeout,
					       iface, NULL);
			return 0;
		}
	}

	return setup_interface2(iface);
}


static int setup_interface2(struct hostapd_iface *iface)
{
	iface->wait_channel_update = 0;

	if (hostapd_get_hw_features(iface)) {
		/* Not all drivers support this yet, so continue without hw
		 * feature data. */
	} else {
		int ret = hostapd_select_hw_mode(iface, 1);
		if (ret < 0) {
			wpa_printf(MSG_ERROR, "Could not select hw_mode and "
				   "channel. (%d)", ret);
			goto fail;
		}
		hostapd_send_dfs_debug_channel_flag(iface);
		if (ret == 1) {
			wpa_printf(MSG_DEBUG, "Interface initialization will be completed in a callback (ACS)");
			return 0;
		}
		ret = hostapd_check_edmg_capab(iface);
		if (ret < 0)
			goto fail;
		ret = hostapd_check_ht_capab(iface);
		if (ret < 0)
			goto fail;
		if (ret == 1) {
			wpa_printf(MSG_DEBUG, "Interface initialization will "
				   "be completed in a callback");
			return 0;
		}

		if (iface->conf->ieee80211h)
			wpa_printf(MSG_DEBUG, "DFS support is enabled");
	}
	return hostapd_setup_interface_complete(iface, 0);

fail:
	hostapd_set_state(iface, HAPD_IFACE_DISABLED);
	wpa_msg_ap_state(iface->bss[0], AP_EVENT_DISABLED);
	if (iface->interfaces && iface->interfaces->terminate_on_error)
		eloop_terminate();
	return -1;
}


#ifdef CONFIG_FST

static const u8 * fst_hostapd_get_bssid_cb(void *ctx)
{
	struct hostapd_data *hapd = ctx;

	return hapd->own_addr;
}


static void fst_hostapd_get_channel_info_cb(void *ctx,
					    enum hostapd_hw_mode *hw_mode,
					    u8 *channel)
{
	struct hostapd_data *hapd = ctx;

	*hw_mode = ieee80211_freq_to_chan(hapd->iface->freq, channel);
}


static void fst_hostapd_set_ies_cb(void *ctx, const struct wpabuf *fst_ies)
{
	struct hostapd_data *hapd = ctx;

	if (hapd->iface->fst_ies != fst_ies) {
		hapd->iface->fst_ies = fst_ies;
		if (ieee802_11_update_beacon(hapd))
			wpa_printf(MSG_WARNING, "FST: Cannot set beacon");
	}
}


static int fst_hostapd_send_action_cb(void *ctx, const u8 *da,
				      struct wpabuf *buf)
{
	struct hostapd_data *hapd = ctx;

	return hostapd_drv_send_action(hapd, hapd->iface->freq, 0, da,
				       wpabuf_head(buf), wpabuf_len(buf));
}


static const struct wpabuf * fst_hostapd_get_mb_ie_cb(void *ctx, const u8 *addr)
{
	struct hostapd_data *hapd = ctx;
	struct sta_info *sta = ap_get_sta(hapd, addr);

	return sta ? sta->mb_ies : NULL;
}


static void fst_hostapd_update_mb_ie_cb(void *ctx, const u8 *addr,
					const u8 *buf, size_t size)
{
	struct hostapd_data *hapd = ctx;
	struct sta_info *sta = ap_get_sta(hapd, addr);

	if (sta) {
		struct mb_ies_info info;

		if (!mb_ies_info_by_ies(&info, buf, size)) {
			wpabuf_free(sta->mb_ies);
			sta->mb_ies = mb_ies_by_info(&info);
		}
	}
}


static const u8 * fst_hostapd_get_sta(struct fst_get_peer_ctx **get_ctx,
				      bool mb_only)
{
	struct sta_info *s = (struct sta_info *) *get_ctx;

	if (mb_only) {
		for (; s && !s->mb_ies; s = s->next)
			;
	}

	if (s) {
		*get_ctx = (struct fst_get_peer_ctx *) s->next;

		return s->addr;
	}

	*get_ctx = NULL;
	return NULL;
}


static const u8 * fst_hostapd_get_peer_first(void *ctx,
					     struct fst_get_peer_ctx **get_ctx,
					     bool mb_only)
{
	struct hostapd_data *hapd = ctx;

	*get_ctx = (struct fst_get_peer_ctx *) hapd->sta_list;

	return fst_hostapd_get_sta(get_ctx, mb_only);
}


static const u8 * fst_hostapd_get_peer_next(void *ctx,
					    struct fst_get_peer_ctx **get_ctx,
					    bool mb_only)
{
	return fst_hostapd_get_sta(get_ctx, mb_only);
}


void fst_hostapd_fill_iface_obj(struct hostapd_data *hapd,
				struct fst_wpa_obj *iface_obj)
{
	iface_obj->ctx = hapd;
	iface_obj->get_bssid = fst_hostapd_get_bssid_cb;
	iface_obj->get_channel_info = fst_hostapd_get_channel_info_cb;
	iface_obj->set_ies = fst_hostapd_set_ies_cb;
	iface_obj->send_action = fst_hostapd_send_action_cb;
	iface_obj->get_mb_ie = fst_hostapd_get_mb_ie_cb;
	iface_obj->update_mb_ie = fst_hostapd_update_mb_ie_cb;
	iface_obj->get_peer_first = fst_hostapd_get_peer_first;
	iface_obj->get_peer_next = fst_hostapd_get_peer_next;
}

#endif /* CONFIG_FST */

#ifdef CONFIG_OWE

static int hostapd_owe_iface_iter(struct hostapd_iface *iface, void *ctx)
{
	struct hostapd_data *hapd = ctx;
	size_t i;

	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *bss = iface->bss[i];

		if (os_strcmp(hapd->conf->owe_transition_ifname,
			      bss->conf->iface) != 0)
			continue;

		wpa_printf(MSG_DEBUG,
			   "OWE: ifname=%s found transition mode ifname=%s BSSID "
			   MACSTR " SSID %s",
			   hapd->conf->iface, bss->conf->iface,
			   MAC2STR(bss->own_addr),
			   wpa_ssid_txt(bss->conf->ssid.ssid,
					bss->conf->ssid.ssid_len));
		if (!bss->conf->ssid.ssid_set || !bss->conf->ssid.ssid_len ||
		    is_zero_ether_addr(bss->own_addr))
			continue;

		os_memcpy(hapd->conf->owe_transition_bssid, bss->own_addr,
			  ETH_ALEN);
		os_memcpy(hapd->conf->owe_transition_ssid,
			  bss->conf->ssid.ssid, bss->conf->ssid.ssid_len);
		hapd->conf->owe_transition_ssid_len = bss->conf->ssid.ssid_len;
		wpa_printf(MSG_DEBUG,
			   "OWE: Copied transition mode information");
		return 1;
	}

	return 0;
}


int hostapd_owe_trans_get_info(struct hostapd_data *hapd)
{
	if (hapd->conf->owe_transition_ssid_len > 0 &&
	    !is_zero_ether_addr(hapd->conf->owe_transition_bssid))
		return 0;

	/* Find transition mode SSID/BSSID information from a BSS operated by
	 * this hostapd instance. */
	if (!hapd->iface->interfaces ||
	    !hapd->iface->interfaces->for_each_interface)
		return hostapd_owe_iface_iter(hapd->iface, hapd);
	else
		return hapd->iface->interfaces->for_each_interface(
			hapd->iface->interfaces, hostapd_owe_iface_iter, hapd);
}


static int hostapd_owe_iface_iter2(struct hostapd_iface *iface, void *ctx)
{
	size_t i;

	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *bss = iface->bss[i];
		int res;

		if (!bss->conf->owe_transition_ifname[0])
			continue;
		if (bss->iface->state != HAPD_IFACE_ENABLED) {
			wpa_printf(MSG_DEBUG,
				   "OWE: Interface %s state %s - defer beacon update",
				   bss->conf->iface,
				   hostapd_state_text(bss->iface->state));
			continue;
		}
		res = hostapd_owe_trans_get_info(bss);
		if (res == 0)
			continue;
		wpa_printf(MSG_DEBUG,
			   "OWE: Matching transition mode interface enabled - update beacon data for %s",
			   bss->conf->iface);
		ieee802_11_update_beacon(bss);
	}

	return 0;
}

#endif /* CONFIG_OWE */


static void hostapd_owe_update_trans(struct hostapd_iface *iface)
{
#ifdef CONFIG_OWE
	/* Check whether the enabled BSS can complete OWE transition mode
	 * configuration for any pending interface. */
	if (!iface->interfaces ||
	    !iface->interfaces->for_each_interface)
		hostapd_owe_iface_iter2(iface, NULL);
	else
		iface->interfaces->for_each_interface(
			iface->interfaces, hostapd_owe_iface_iter2, NULL);
#endif /* CONFIG_OWE */
}

/* hostapd_post_up_vendor_cmd_mu_type - read modify write for up/down_mu_type */
static void hostapd_post_up_vendor_cmd_mu_type(struct hostapd_data *hapd, struct hostapd_config *conf)
{
	struct wpabuf *rsp = NULL;
	int *dynamicMuConfig = 0;

	rsp = wpabuf_alloc(DYNAMIC_MU_TYPE_SIZE * sizeof(int));
	if(!rsp){
		wpa_printf(MSG_WARNING, "failed to allocate wpabuf");
		return;
	}
	dynamicMuConfig = (int *)rsp->buf;

	if(conf->sDynamicMuTypeDownLink >= 0)
		dynamicMuConfig[0] = conf->sDynamicMuTypeDownLink;
	if(conf->sDynamicMuTypeUpLink >= 0)
		dynamicMuConfig[1] = conf->sDynamicMuTypeUpLink;
	if(conf->sDynamicMuMinStationsInGroup >= 0)
		dynamicMuConfig[2] = conf->sDynamicMuMinStationsInGroup;
	if(conf->sDynamicMuMaxStationsInGroup >= 0)
		dynamicMuConfig[3] = conf->sDynamicMuMaxStationsInGroup;
	if(conf->sDynamicMuCdbConfig >= 0)
		dynamicMuConfig[4] = conf->sDynamicMuCdbConfig;

	if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_DYNAMIC_MU_TYPE,
		(u8*)dynamicMuConfig, DYNAMIC_MU_TYPE_SIZE * sizeof(int), NULL))
		wpa_printf(MSG_WARNING, "set dynamic mu parameter failed");

	wpabuf_free(rsp);
}


/* hostapd_post_up_vendor_cmd_guard_interval - validate guard interval value and write to driver */
static void hostapd_post_up_vendor_cmd_guard_interval(struct hostapd_data *hapd, int sFixedLtfGi, int ieee80211ax, int ieee80211ac)
{
	int iwFormat[2] = {0, GI_ILLEGAL_VALUE}; /*auto, gi value*/

	switch (sFixedLtfGi){
		case GI_ILLEGAL_VALUE:
			return;

		case GUARD_INTERVAL_400:
			if(ieee80211ac && !ieee80211ax)
				iwFormat[1] = GI_CP_MODE_SHORT_CP_SHORT_LTF;
			break;

		case GUARD_INTERVAL_800:
			if(ieee80211ac || ieee80211ax)
				iwFormat[1] = GI_CP_MODE_SHORT_CP_MED_LTF;
			break;

		case GUARD_INTERVAL_1600:
			if(!ieee80211ac && ieee80211ax)
				iwFormat[1] = GI_CP_MODE_MED_CP_MED_LTF;
			break;

		case GUARD_INTERVAL_3200:
			if(!ieee80211ac && ieee80211ax)
				iwFormat[1] = GI_CP_MODE_LONG_CP_LONG_LTF;
			break;

		case GUARD_INTERVAL_AUTO:
			iwFormat[0] = 1;
			iwFormat[1] = 0;
			break;
	}

	if(iwFormat[1] == GI_ILLEGAL_VALUE){
		wpa_printf(MSG_WARNING, "illegal guard interval value %d\n", sFixedLtfGi);
		return;
	}

	if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_FIXED_LTF_AND_GI,
		(u8*)iwFormat, sizeof(iwFormat), NULL))
		wpa_printf(MSG_WARNING, "set guard interval failed");
}

void hostapd_send_fils_discovery_frame(struct hostapd_data *hapd)
{
#define FILS_DISCOVERY_FRAME_CTRL ((BIT(0) | BIT(1) | BIT(6) | BIT(10) | BIT(12))) /* Short_ssid_len(B0 and B1),Short_ssid(B6)       *
                                                                                    * Pri Chan/Op Class (B10), Lenth(B12) indicators */
#define FILS_PAYLOAD_GUARD              2
#define FILS_OP_CLASS_FIELD_SIZE        1
#define FILS_PRI_CHAN_FIELD_SIZE        1
#define FILS_TPE_IE_SIZE                (sizeof(struct intel_vendor_fd_tpe_ie))
#define FILS_OPTIONAL_ELEMENTS_SIZE     (FILS_OP_CLASS_FIELD_SIZE +\
                                         FILS_PRI_CHAN_FIELD_SIZE +\
                                         FILS_TPE_IE_SIZE)

	struct intel_vendor_fils_discovery_info fils_info;
	struct ieee80211_mgmt *frame;
	u8 *pos,*buf;
	size_t buflen = IEEE80211_HDRLEN + sizeof(struct intel_vendor_fils_discovery_info) + FILS_PAYLOAD_GUARD;

	buf = os_zalloc(buflen);
	if (!buf) {
		wpa_printf(MSG_ERROR,"FILS dicovery frame alloc failed");
		return;
	}

	os_memset(&fils_info, 0, sizeof(fils_info));
	fils_info.fils_discovery_frame_control = host_to_le16(FILS_DISCOVERY_FRAME_CTRL);
	fils_info.timestamp = host_to_le64(0); /* FW should fill this */
	fils_info.beacon_int = host_to_le16(hostapd_get_beacon_int(hapd));
	WPA_PUT_LE32(fils_info.short_ssid,crc32(hapd->conf->ssid.ssid,hapd->conf->ssid.ssid_len));
	fils_info.length = FILS_OPTIONAL_ELEMENTS_SIZE;
	fils_info.op_class = hapd->iface->conf->op_class;
	fils_info.primary_chan = hapd->iface->conf->channel;
	/* TPE update - only one PSD advertisement */
	hostapd_eid_txpower_envelope_6g(hapd, &fils_info.tpe_ie.elem_id, true);
	frame = (struct ieee80211_mgmt *) buf;

	frame->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,WLAN_FC_STYPE_ACTION);
	os_memcpy(frame->da, broadcast_ether_addr, ETH_ALEN);
	os_memcpy(frame->sa, hapd->own_addr, ETH_ALEN);
	os_memcpy(frame->bssid, hapd->own_addr, ETH_ALEN);

	frame->u.action.category = WLAN_ACTION_PUBLIC;
	frame->u.action.u.public_action.action = WLAN_PA_FILS_DISCOVERY;

	pos = frame->u.action.u.public_action.variable;
	os_memcpy(pos, &fils_info, sizeof(struct intel_vendor_fils_discovery_info));

	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SEND_FILS_DISCOVERY_FRAME,
						(const u8*)buf, buflen, NULL)) {
		wpa_printf(MSG_ERROR, "send LTQ_NL80211_VENDOR_SUBCMD_SEND_FILS_DISCOVERY_FRAME failed!!!");
		goto done;
	}

done:
	os_free(buf);
}

static inline void hostapd_post_up_vendor_cmd(struct hostapd_iface *iface)
{
	struct hostapd_data *hapd = iface->bss[0];
	int ap_retry_limit_cfg[2];

	if (iface->conf->sErpSet){
		if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_ERP,
					   (u8*)iface->conf->sErpSet, ERP_SET_SIZE * sizeof(int), NULL))
			wpa_printf(MSG_WARNING, "set sErpSet failed");
	}

	if (iface->conf->sRadarRssiTh){
		if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_RADAR_RSSI_TH,
					   (u8*)&iface->conf->sRadarRssiTh, sizeof(iface->conf->sRadarRssiTh), NULL))
			wpa_printf(MSG_WARNING, "set sRadarRssiTh failed");
	}


	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_RTS_MODE,
				   (u8*)iface->conf->sRTSmode, sizeof(iface->conf->sRTSmode), NULL))
		wpa_printf(MSG_WARNING, "set sRTSmode failed");


	if (iface->conf->sFixedRateCfg){
		if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_FIXED_RATE,
					   (u8*)iface->conf->sFixedRateCfg, FIXED_RATE_CONFIG_SIZE  * sizeof(int), NULL))
			wpa_printf(MSG_WARNING, "set sFixedRateCfg failed");
	}

	if(iface->conf->sQAMplus != -1){
		if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_QAMPLUS_MODE,
				(u8*)&iface->conf->sQAMplus, sizeof(iface->conf->sQAMplus), NULL))
			wpa_printf(MSG_WARNING, "set sQAMplus failed");
	}

	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_INTERFER_THRESH,
				   (u8*)&iface->conf->sInterferDetThresh, sizeof(iface->conf->sInterferDetThresh), NULL))
		wpa_printf(MSG_WARNING, "set sInterferDetThresh failed");


	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_CCA_ADAPT,
				   (u8*)iface->conf->sCcaAdapt, sizeof(iface->conf->sCcaAdapt), NULL))
		wpa_printf(MSG_WARNING, "set sCcaAdapt failed");

	if(iface->conf->sFWRecovery){
		if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_FW_RECOVERY,
					  (u8*)iface->conf->sFWRecovery, iface->conf->sFWRecoverySize, NULL))
			wpa_printf(MSG_WARNING, "set sFWRecovery failed");
	}

	u32 nfrpcfg[] = {get_he_cap(iface->conf->he_capab.he_mac_capab_info[HE_MACCAP_CAP4_IDX],
				    HE_MAC_CAP4_NDP_FEEDBACK_REPORT_SUPPORT),
				    iface->conf->he_nfr_buffer_threshold};

	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_NFRP_CFG,
				   (u8*)&nfrpcfg[0], sizeof(nfrpcfg), NULL))
		wpa_printf(MSG_WARNING, "set sNfrpCfg failed");

	ap_retry_limit_cfg[0] = iface->conf->ap_retry_limit;
	ap_retry_limit_cfg[1] = iface->conf->ap_retry_limit_data;

	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_AP_RETRY_LIMIT,
				   (u8*)ap_retry_limit_cfg, sizeof(ap_retry_limit_cfg), NULL))
		wpa_printf(MSG_WARNING, "set ap_retry_limit %d failed", iface->conf->ap_retry_limit);

	hostapd_post_up_vendor_cmd_mu_type(hapd, iface->conf);
	hostapd_post_up_vendor_cmd_guard_interval(hapd, iface->conf->sFixedLtfGi, iface->conf->ieee80211ax, iface->conf->ieee80211ac);
	
}

static void hostapd_interface_setup_failure_handler(void *eloop_ctx,
						    void *timeout_ctx)
{
	struct hostapd_iface *iface = eloop_ctx;
	struct hostapd_data *hapd;

	if (iface->num_bss < 1 || !iface->bss || !iface->bss[0])
		return;
	hapd = iface->bss[0];
	if (hapd->setup_complete_cb)
		hapd->setup_complete_cb(hapd->setup_complete_cb_ctx);
}


static void hostapd_prepare_csa_deauth_frame(struct hostapd_data *hapd, u8 *mgmt_frame_buf, u8 *mgmt_frame_buf_len)
{
	errno_t err;
	size_t len = INTEL_NON_PROTECTED_DEAUTH_FRAME_LEN, plen = len;
	struct ieee80211_mgmt mgmt;
#ifdef BIP_PROTECTION_WORKING
	u8 *mgmt_frame_buf_t = NULL;
#endif

	memset(&mgmt, 0, sizeof(struct ieee80211_mgmt));
	mgmt.frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
                                          WLAN_FC_STYPE_DEAUTH);
	os_memset(mgmt.da, 0xff, ETH_ALEN);
	err = memcpy_s(mgmt.sa, sizeof(mgmt.sa), hapd->own_addr, sizeof(hapd->own_addr));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return;
	}
	err = memcpy_s(mgmt.bssid, sizeof(mgmt.bssid), hapd->own_addr, sizeof(hapd->own_addr));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return;
	}
	mgmt.u.deauth.reason_code = host_to_le16(WLAN_REASON_UNSPECIFIED);

	if (hapd->conf->ieee80211w != NO_MGMT_FRAME_PROTECTION) {
#ifdef BIP_PROTECTION_WORKING
		/* TODO invoke bip_protect or bip_protect_* on mgmt frame based on group_mgmt_cipher */
		mgmt_frame_buf_t = bip_protect(hapd->wpa_auth, (u8 *)&mgmt, len, &plen);
		err = memcpy_s(mgmt_frame_buf, INTEL_MAX_PROTECTED_DEAUTH_FRAME_LEN, mgmt_frame_buf_t, plen);
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}
#else
		err = memcpy_s(mgmt_frame_buf, INTEL_MAX_PROTECTED_DEAUTH_FRAME_LEN, &mgmt, plen);
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}
#endif
	}
	else
	{
		err = memcpy_s(mgmt_frame_buf, INTEL_MAX_PROTECTED_DEAUTH_FRAME_LEN, &mgmt, plen);
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return;
		}
	}
	*mgmt_frame_buf_len = (u8)plen;
}


int hostapd_prepare_and_send_csa_deauth_cfg_to_driver(struct hostapd_data *hapd)
{
	struct hostapd_iface *iface = hapd->iface;
	struct intel_vendor_channel_switch_cfg cs_vendor_cfg;
	int i;

	for (i = 1; i < iface->num_bss; i++) { /* skip master dummy vap (index 0) */
		os_memset(&cs_vendor_cfg, 0, sizeof(cs_vendor_cfg));
		switch (iface->bss[i]->csa_deauth_mode) {
			case CSA_DEAUTH_MODE_DISABLED:
				cs_vendor_cfg.csaDeauthMode = iface->bss[i]->csa_deauth_mode;
				break;
			case CSA_DEAUTH_MODE_BROADCAST:
				hostapd_prepare_csa_deauth_frame(iface->bss[i], cs_vendor_cfg.csaDeauthFrames,
					&cs_vendor_cfg.csaMcDeauthFrameLength); /* TODO replace i in second param with VAP idx equivalent */
				/* fall thru */
			case CSA_DEAUTH_MODE_UNICAST:
				cs_vendor_cfg.csaDeauthMode = iface->bss[i]->csa_deauth_mode;
				if (cs_vendor_cfg.csaDeauthTxTime[INTEL_CSA_DEAUTH_TX_TIME_UC_IDX] == 0 &&
					iface->bss[i]->csa_deauth_tx_time[INTEL_CSA_DEAUTH_TX_TIME_UC_IDX] != 0) {
					cs_vendor_cfg.csaDeauthTxTime[INTEL_CSA_DEAUTH_TX_TIME_UC_IDX] =
						iface->bss[i]->csa_deauth_tx_time[INTEL_CSA_DEAUTH_TX_TIME_UC_IDX];
				}
				if (cs_vendor_cfg.csaDeauthTxTime[INTEL_CSA_DEAUTH_TX_TIME_MC_IDX] == 0 &&
					iface->bss[i]->csa_deauth_tx_time[INTEL_CSA_DEAUTH_TX_TIME_MC_IDX] != 0) {
					cs_vendor_cfg.csaDeauthTxTime[INTEL_CSA_DEAUTH_TX_TIME_MC_IDX] =
						iface->bss[i]->csa_deauth_tx_time[INTEL_CSA_DEAUTH_TX_TIME_MC_IDX];
				}
				break;
			default: /* unreachable code */
				hostapd_logger(iface->bss[i], NULL, HOSTAPD_MODULE_IEEE80211,
							HOSTAPD_LEVEL_INFO, "invalid csa deauth mode [%d]",
							iface->bss[i]->csa_deauth_mode);
				break;
		}
		if (hostapd_drv_vendor_cmd(iface->bss[i], OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_CHANNEL_SWITCH_DEAUTH_CFG,
							(const u8*)&cs_vendor_cfg, sizeof(cs_vendor_cfg), NULL)) {
			wpa_printf(MSG_ERROR, "Failed to send driver vendor command LTQ_NL80211_VENDOR_SUBCMD_CHANNEL_SWITCH_DEAUTH_CFG for %s",
						iface->bss[i]->conf->iface);
			return -1;
		}
	}

	return 0;
}

static int hostapd_setup_interface_complete_sync(struct hostapd_iface *iface,
						 int err)
{
	struct hostapd_data *hapd = iface->bss[0];
	size_t j;
	u8 *prev_addr;
	int res_dfs_offload = 0;
	struct hostapd_channel_data *channel;
	int srxThVal = SRX_TH_VAL;

	if (err)
		goto fail;

	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_TX_POWER_LIMIT_OFFSET,
				   (u8*)&iface->conf->sPowerSelection, sizeof(iface->conf->sPowerSelection), NULL))
		wpa_printf(MSG_WARNING, "set sPowerSelection failed");

	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_RX_THRESHOLD,
				   (u8*)&srxThVal, sizeof(srxThVal), NULL))
		wpa_printf(MSG_WARNING, "set srxThVal failed");

	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_STATIONS_STATISTICS,
				   (u8*)&iface->conf->sStationsStat,
				   sizeof(iface->conf->sStationsStat), NULL))
		wpa_printf(MSG_WARNING, "set sStationsStat failed");

	if (iface->conf->sConfigMRCoex != -1){
		int sConfigMRCoex[4] = {iface->conf->sConfigMRCoex, -1, -1, -1};
		if (iface->conf->sConfigMRCoexActiveTime) {
			sConfigMRCoex[1] = iface->conf->sConfigMRCoexActiveTime;
		}
		if (iface->conf->sConfigMRCoexInactiveTime) {
			sConfigMRCoex[2] = iface->conf->sConfigMRCoexInactiveTime;
		}
		if (iface->conf->sConfigMRCoexCts2SelfActive) {
			sConfigMRCoex[3] = iface->conf->sConfigMRCoexCts2SelfActive;
		}
		if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_COEX_CFG,
			(u8*)sConfigMRCoex, sizeof(sConfigMRCoex), NULL))
			wpa_printf(MSG_WARNING, "set sConfigMRCoex failed");
	}

	if (iface->conf->sCoCPower){
		if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_COC_POWER_MODE,
					   (u8*)iface->conf->sCoCPower, iface->conf->sCoCPowerSize * sizeof(int), NULL))
					   wpa_printf(MSG_WARNING, "set sCoCPower failed");
	}

	if (iface->conf->sCoCAutoCfg){
		if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_COC_AUTO_PARAMS,
					   (u8*)iface->conf->sCoCAutoCfg, COC_AUTO_CONFIG * sizeof(int), NULL))
			wpa_printf(MSG_WARNING, "set sCoCAutoCfg failed");
	}

	if (iface->allow_scan_during_cac == -1){
		if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_ALLOW_SCAN_DURING_CAC,
					   (u8*)&iface->conf->allow_scan_during_cac, sizeof(iface->conf->allow_scan_during_cac), NULL))
			wpa_printf(MSG_WARNING, "set allow_scan_during_cac failed");
		else
			iface->allow_scan_during_cac = iface->conf->allow_scan_during_cac;

	}

	if(iface->conf->sBfMode != -1){
		if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_BF_MODE,
				(u8*)&iface->conf->sBfMode, sizeof(iface->conf->sBfMode), NULL))
			wpa_printf(MSG_WARNING, "set sBfMode failed");
	}

	if(iface->conf->sMaxMpduLen != -1){
		if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_MAX_MPDU_LENGTH,
				(u8*)&iface->conf->sMaxMpduLen, sizeof(iface->conf->sMaxMpduLen), NULL))
			wpa_printf(MSG_WARNING, "set sMaxMpduLen failed");
	}

	if(hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_SET_CLT_PROBE_REQS_MODE,
			(u8*)&iface->conf->sProbeReqCltMode, sizeof(iface->conf->sProbeReqCltMode), NULL))
		wpa_printf(MSG_WARNING, "set sProbeReqCltMode failed");

	iface->sb_dfs_cntr = 0;

	wpa_printf(MSG_DEBUG, "Completing interface initialization");
	if (iface->conf->channel) {
		int res;

		iface->freq = hostapd_hw_get_freq(hapd, iface->conf->channel);
		wpa_printf(MSG_DEBUG, "Mode: %s  Channel: %d  "
			   "Frequency: %d MHz",
			   hostapd_hw_mode_txt(iface->conf->hw_mode),
			   iface->conf->channel, iface->freq);

#ifdef NEED_AP_MLME
		/* Handle DFS only if it is not offloaded to the driver */
		if (!(iface->drv_flags & WPA_DRIVER_FLAGS_DFS_OFFLOAD)) {
			/* Check DFS */
			res = hostapd_handle_dfs(iface);
			if (res <= 0) {
				if (res < 0)
					goto fail;
				return res;
			}
		} else {
			/* If DFS is offloaded to the driver */
			res_dfs_offload = hostapd_handle_dfs_offload(iface);
			if (res_dfs_offload <= 0) {
				if (res_dfs_offload < 0)
					goto fail;
			} else {
				wpa_printf(MSG_DEBUG,
					   "Proceed with AP/channel setup");
				/*
				 * If this is a DFS channel, move to completing
				 * AP setup.
				 */
				if (res_dfs_offload == 1)
					goto dfs_offload;
				/* Otherwise fall through. */
			}
		}
#endif /* NEED_AP_MLME */

#ifdef CONFIG_MESH
		if (iface->mconf != NULL) {
			wpa_printf(MSG_DEBUG,
				   "%s: Mesh configuration will be applied while joining the mesh network",
				   iface->bss[0]->conf->iface);
		}
#endif /* CONFIG_MESH */

		channel = hostapd_hw_get_channel_data(iface, iface->freq);
		if (!channel)
			goto fail;
		if (!iface->set_freq_done) {
			if (hostapd_set_freq(hapd, hapd->iconf->hw_mode, iface->freq,
					hapd->iconf->channel,
					hapd->iconf->enable_edmg,
					hapd->iconf->edmg_channel,
					hapd->iconf->ieee80211n,
					hapd->iconf->ieee80211ac,
					hapd->iconf->ieee80211ax,
					hapd->iconf->secondary_channel,
					hostapd_get_oper_chwidth(hapd->iconf),
					hostapd_get_oper_centr_freq_seg0_idx(hapd->iconf),
					hostapd_get_oper_centr_freq_seg1_idx(hapd->iconf))) {
				wpa_printf(MSG_ERROR, "Could not set channel for "
						"kernel driver");
				goto fail;
			}
			iface->set_freq_done = 1;
		} else {
			struct csa_settings settings;
			memset(&settings, 0, sizeof(settings));
			if (hostapd_set_freq_params(&settings.freq_params,
					hapd->iconf->hw_mode, iface->freq, hapd->iconf->channel,
					hapd->iconf->enable_edmg,
					hapd->iconf->edmg_channel,
					hapd->iconf->ieee80211n, hapd->iconf->ieee80211ac, hapd->iconf->ieee80211ax,
					hapd->iconf->secondary_channel,
					hostapd_get_oper_chwidth(hapd->iconf),
					hostapd_get_oper_centr_freq_seg0_idx(hapd->iconf),
					hostapd_get_oper_centr_freq_seg1_idx(hapd->iconf),
					hapd->iface->current_mode->vht_capab,
					&hapd->iface->current_mode->he_capab)) {
				wpa_printf(MSG_ERROR,
						"hostapd_set_freq_params failed: mode=%i, freq=%i, "
								"channel=%i, ht_enabled=%i, vht_enabled=%i, "
								"sec_channel_offset=%i, vht_open_cwidth=%i, "
								"center_segment0=%i, center_segment1=%i",
						hapd->iconf->hw_mode, iface->freq, hapd->iconf->channel,
						hapd->iconf->ieee80211n, hapd->iconf->ieee80211ac,
						hapd->iconf->secondary_channel,
						hostapd_get_oper_chwidth(hapd->iconf),
						hostapd_get_oper_centr_freq_seg0_idx(hapd->iconf),
						hostapd_get_oper_centr_freq_seg1_idx(hapd->iconf));

				goto fail;
			}

			settings.cs_count = hostapd_get_cs_count(iface);
			res = hostapd_prepare_and_send_csa_deauth_cfg_to_driver(iface->bss[0]);
			if (res) {
				wpa_printf(MSG_ERROR, "hostapd_prepare_and_send_csa_deauth_cfg_to_driver failed: %s",
							iface->bss[0]->conf->iface);
				goto fail;
			}
			for (j = 0; j < iface->num_bss; j++) {
				hapd = iface->bss[j];
				res = hostapd_switch_channel(hapd, &settings);
				if (res) {
					wpa_printf(MSG_ERROR, "hostapd_switch_channel failed: freq=%i, "
							"ht_enabled=%i, vht_enabled=%i, "
							"sec_channel_offset=%i, vht_open_cwidth=%i, "
							"center_freq1=%i, center_freq2=%i",
							settings.freq_params.freq,
							settings.freq_params.ht_enabled,
							settings.freq_params.vht_enabled,
							settings.freq_params.sec_channel_offset,
							settings.freq_params.bandwidth,
							settings.freq_params.center_freq1,
							settings.freq_params.center_freq2);
					goto fail;
				}
			}
			hostapd_set_state(iface, HAPD_IFACE_ENABLED);
			/* After calling hostapd_switch_channel
			 * hostapd_setup_bss/ieee802_11_set_beacon will fail with:
			 * "Cannot set beacons during CSA period", because hostapd_switch_channel
			 * starts CSA, therefore we need to exit here. */
			return 0;
		}
	}

	if (iface->current_mode) {
		if (hostapd_prepare_rates(iface, iface->current_mode)) {
			wpa_printf(MSG_ERROR, "Failed to prepare rates "
				   "table.");
			hostapd_logger(hapd, NULL, HOSTAPD_MODULE_IEEE80211,
				       HOSTAPD_LEVEL_WARNING,
				       "Failed to prepare rates table.");
			goto fail;
		}
	}

	if (hapd->iconf->rts_threshold >= -1 &&
	    hostapd_set_rts(hapd, hapd->iconf->rts_threshold) &&
	    hapd->iconf->rts_threshold >= -1) {
		wpa_printf(MSG_ERROR, "Could not set RTS threshold for "
			   "kernel driver");
		goto fail;
	}

	if (hapd->iconf->fragm_threshold >= -1 &&
	    hostapd_set_frag(hapd, hapd->iconf->fragm_threshold) &&
	    hapd->iconf->fragm_threshold != -1) {
		wpa_printf(MSG_ERROR, "Could not set fragmentation threshold "
			   "for kernel driver");
		goto fail;
	}

	prev_addr = hapd->own_addr;

	for (j = 0; j < iface->num_bss; j++) {
		hapd = iface->bss[j];
		if (j)
			os_memcpy(hapd->own_addr, prev_addr, ETH_ALEN);
		if (hostapd_setup_bss(hapd, j == 0)) {
			for (;;) {
				hapd = iface->bss[j];
				hostapd_bss_deinit_no_free(hapd);
				hostapd_free_hapd_data(hapd);
				if (j == 0)
					break;
				j--;
			}
			goto fail;
		}
		if (is_zero_ether_addr(hapd->conf->bssid))
			prev_addr = hapd->own_addr;
	}
	hapd = iface->bss[0];

	hostapd_tx_queue_params(hapd);

	ap_list_init(iface);

	hostapd_set_acl(hapd);

	if (hostapd_driver_commit(hapd) < 0) {
		wpa_printf(MSG_ERROR, "%s: Failed to commit driver "
			   "configuration", __func__);
		goto fail;
	}

	if ((iface->drv_flags & WPA_DRIVER_FLAGS_DFS_OFFLOAD) &&
	    !res_dfs_offload) {
		/*
		 * If freq is DFS, and DFS is offloaded to the driver, then wait
		 * for CAC to complete.
		 */
		wpa_printf(MSG_DEBUG, "%s: Wait for CAC to complete", __func__);
		return res_dfs_offload;
	}

#ifdef NEED_AP_MLME
dfs_offload:
#endif /* NEED_AP_MLME */

#ifdef CONFIG_FST
	if (hapd->iconf->fst_cfg.group_id[0]) {
		struct fst_wpa_obj iface_obj;

		fst_hostapd_fill_iface_obj(hapd, &iface_obj);
		iface->fst = fst_attach(hapd->conf->iface, hapd->own_addr,
					&iface_obj, &hapd->iconf->fst_cfg);
		if (!iface->fst) {
			wpa_printf(MSG_ERROR, "Could not attach to FST %s",
				   hapd->iconf->fst_cfg.group_id);
			goto fail;
		}
	}
#endif /* CONFIG_FST */

	hostapd_set_state(iface, HAPD_IFACE_ENABLED);
	hostapd_owe_update_trans(iface);
	airtime_policy_update_init(iface);
	if (hapd->setup_complete_cb)
		hapd->setup_complete_cb(hapd->setup_complete_cb_ctx);

	wpa_printf(MSG_DEBUG, "%s: Setup of interface done.",
		   iface->bss[0]->conf->iface);
	if (iface->interfaces && iface->interfaces->terminate_on_error > 0)
		iface->interfaces->terminate_on_error--;

	for (j = 0; j < iface->num_bss; j++) {
		hostapd_flush_old_stations(iface->bss[j], WLAN_REASON_PREV_AUTH_NOT_VALID);
		hostapd_drv_sta_allow(hapd, NULL, 0);
	}

	hostapd_post_up_vendor_cmd(iface);

	/*
	 * WPS UPnP module can be initialized only when the "upnp_iface" is up.
	 * If "interface" and "upnp_iface" are the same (e.g., non-bridge
	 * mode), the interface is up only after driver_commit, so initialize
	 * WPS after driver_commit.
	 */
	for (j = 0; j < iface->num_bss; j++) {
		if (hostapd_init_wps_complete(iface->bss[j]))
			wpa_printf(MSG_ERROR, "init wps may be failed: brlan0 may not have IP");
	}

	return 0;

fail:
	wpa_printf(MSG_ERROR, "Interface initialization failed");
	hostapd_set_state(iface, HAPD_IFACE_DISABLED);
	wpa_msg_ap_state(hapd, AP_EVENT_DISABLED);
#ifdef CONFIG_FST
	if (iface->fst) {
		fst_detach(iface->fst);
		iface->fst = NULL;
	}
#endif /* CONFIG_FST */

	if (iface->interfaces && iface->interfaces->terminate_on_error) {
		eloop_terminate();
	} else if (hapd->setup_complete_cb) {
		/*
		 * Calling hapd->setup_complete_cb directly may cause iface
		 * deinitialization which may be accessed later by the caller.
		 */
		eloop_register_timeout(0, 0,
				       hostapd_interface_setup_failure_handler,
				       iface, NULL);
	}

	return -1;
}

#define DELAYED_START_FILENAME_FMT "/tmp/%s_start_after_%s"
static int delayed_iface_file_create(struct hostapd_iface *hapd_iface) {
	char filename[PATH_MAX + 1];

	int ret = sprintf_s(filename, PATH_MAX,
	DELAYED_START_FILENAME_FMT,
	hapd_iface->bss[MASTER_VAP_BSS_IDX]->conf->iface,
	hapd_iface->conf->start_after);

	if (0 >= ret) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
		return ENOK;
	}

	FILE *file = fopen(filename, "w");
	if(NULL == file)
		return ENOK;
	fclose(file);
	return EOK;
}

static bool delayed_iface_file_present(struct hostapd_iface *hapd_iface) {
	char filename[PATH_MAX + 1];

	int ret = sprintf_s(filename, PATH_MAX, DELAYED_START_FILENAME_FMT,
	hapd_iface->bss[MASTER_VAP_BSS_IDX]->conf->iface,
	hapd_iface->conf->start_after);

	if (0 >= ret) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
		return false;
	}

	return os_file_exists(filename);
}

void delayed_iface_watchdog_cb(void *eloop_ctx, void *user_data) {
	struct hostapd_data *hapd = eloop_ctx;
	eloop_cancel_timeout(delayed_iface_watchdog_cb, hapd, NULL);

	if(HAPD_IFACE_ENABLED != hapd->iface->state) {
		wpa_msg(hapd->msg_ctx, MSG_WARNING,
		"Forcing enablement of interface on %d(s) timeout\n",
		hapd->iface->conf->start_after_watchdog_time);
		if(hostapd_setup_interface_complete_sync(hapd->iface, EOK))
			wpa_msg(hapd->msg_ctx, MSG_ERROR, "Failed to enable interface\n");
		else
		{
			if(delayed_iface_file_create(hapd->iface)) {
				wpa_msg(hapd->msg_ctx, MSG_ERROR, "Failed to open control file for %s after %s started\n",
				hapd->iface->bss[MASTER_VAP_BSS_IDX]->conf->iface, hapd->iface->conf->start_after);
				return;
			}
		}
	}
}

/**
 * hostapd_setup_interface_complete - Complete interface setup
 *
 * This function is called when previous steps in the interface setup has been
 * completed. This can also start operations, e.g., DFS, that will require
 * additional processing before interface is ready to be enabled. Such
 * operations will call this function from eloop callbacks when finished.
 */
int hostapd_setup_interface_complete(struct hostapd_iface *iface, int err)
{
	struct hapd_interfaces *interfaces = iface->interfaces;
	struct hostapd_data *hapd = iface->bss[0];
	unsigned int i;
	int not_ready_in_sync_ifaces = 0;


	/*
	 * when mem_only_cred configuration is set we will halt init of bss and request
	 * credentials from the UI
	 */
	if(iface->bss[0]->conf->ssid.request_credentials)
	{
		send_cred_request_to_ui(hapd);
		return 0;
	}

	if(NULL_CHAR != iface->conf->start_after[0] && !delayed_iface_file_present(iface)) {
		wpa_printf(MSG_DEBUG, "%s:%d start of %s delayed until %s is up",
		__func__, __LINE__, iface->bss[MASTER_VAP_BSS_IDX]->conf->iface, iface->conf->start_after);

		if (!eloop_is_timeout_registered(delayed_iface_watchdog_cb, hapd, NULL)) {
					eloop_register_timeout(iface->conf->start_after_watchdog_time,
					0, delayed_iface_watchdog_cb, hapd, NULL);
		} else {
			wpa_printf(MSG_DEBUG, "%s:%d watchdog already registered for iface:%s",
									__func__, __LINE__, hapd->conf->iface);
		}
		return EOK;
	}

	if (!iface->need_to_start_in_sync)
		return hostapd_setup_interface_complete_sync(iface, err);

	if (err) {
		wpa_printf(MSG_ERROR, "Interface initialization failed");
		hostapd_set_state(iface, HAPD_IFACE_DISABLED);
		iface->need_to_start_in_sync = 0;
		wpa_msg_ap_state(hapd, AP_EVENT_DISABLED);
		if (interfaces && interfaces->terminate_on_error)
			eloop_terminate();
		return -1;
	}

	if (iface->ready_to_start_in_sync) {
		/* Already in ready and waiting. should never happpen */
		return 0;
	}

	for (i = 0; i < interfaces->count; i++) {
		if (interfaces->iface[i]->need_to_start_in_sync &&
		    !interfaces->iface[i]->ready_to_start_in_sync)
			not_ready_in_sync_ifaces++;
	}

	/*
	 * Check if this is the last interface, if yes then start all the other
	 * waiting interfaces. If not, add this interface to the waiting list.
	 */
	if (not_ready_in_sync_ifaces > 1 && iface->state == HAPD_IFACE_DFS) {
		/*
		 * If this interface went through CAC, do not synchronize, just
		 * start immediately.
		 */
		iface->need_to_start_in_sync = 0;
		wpa_printf(MSG_INFO,
			   "%s: Finished CAC - bypass sync and start interface",
			   iface->bss[0]->conf->iface);
		return hostapd_setup_interface_complete_sync(iface, err);
	}

	if (not_ready_in_sync_ifaces > 1) {
		/* need to wait as there are other interfaces still coming up */
		iface->ready_to_start_in_sync = 1;
		wpa_printf(MSG_INFO,
			   "%s: Interface waiting to sync with other interfaces",
			   iface->bss[0]->conf->iface);
		return 0;
	}

	wpa_printf(MSG_INFO,
		   "%s: Last interface to sync - starting all interfaces",
		   iface->bss[0]->conf->iface);
	iface->need_to_start_in_sync = 0;
	hostapd_setup_interface_complete_sync(iface, err);
	for (i = 0; i < interfaces->count; i++) {
		if (interfaces->iface[i]->need_to_start_in_sync &&
		    interfaces->iface[i]->ready_to_start_in_sync) {
			hostapd_setup_interface_complete_sync(
				interfaces->iface[i], 0);
			/* Only once the interfaces are sync started */
			interfaces->iface[i]->need_to_start_in_sync = 0;
		}
	}

	return 0;
}


/**
 * hostapd_setup_interface - Setup of an interface
 * @iface: Pointer to interface data.
 * Returns: 0 on success, -1 on failure
 *
 * Initializes the driver interface, validates the configuration,
 * and sets driver parameters based on the configuration.
 * Flushes old stations, sets the channel, encryption,
 * beacons, and WDS links based on the configuration.
 *
 * If interface setup requires more time, e.g., to perform HT co-ex scans, ACS,
 * or DFS operations, this function returns 0 before such operations have been
 * completed. The pending operations are registered into eloop and will be
 * completed from eloop callbacks. Those callbacks end up calling
 * hostapd_setup_interface_complete() once setup has been completed.
 */
int hostapd_setup_interface(struct hostapd_iface *iface)
{
	int ret;

	ret = setup_interface(iface);
	if (ret) {
		wpa_printf(MSG_ERROR, "%s: Unable to setup interface.",
			   iface->bss[0]->conf->iface);
		return -1;
	}

	return 0;
}


/**
 * hostapd_alloc_bss_data - Allocate and initialize per-BSS data
 * @hapd_iface: Pointer to interface data
 * @conf: Pointer to per-interface configuration
 * @bss: Pointer to per-BSS configuration for this BSS
 * Returns: Pointer to allocated BSS data
 *
 * This function is used to allocate per-BSS data structure. This data will be
 * freed after hostapd_cleanup() is called for it during interface
 * deinitialization.
 */
struct hostapd_data *
hostapd_alloc_bss_data(struct hostapd_iface *hapd_iface,
		       struct hostapd_config *conf,
		       struct hostapd_bss_config *bss)
{
	struct hostapd_data *hapd;

	hapd = os_zalloc(sizeof(*hapd));
	if (hapd == NULL)
		return NULL;

	hapd->new_assoc_sta_cb = hostapd_new_assoc_sta;
	hapd->iconf = conf;
	hapd->conf = bss;
	hapd->iface = hapd_iface;
	if (conf)
		hapd->driver = conf->driver;
	hapd->ctrl_sock = -1;
	dl_list_init(&hapd->ctrl_dst);
	dl_list_init(&hapd->nr_db);
	dl_list_init(&hapd->multi_ap_blacklist);
	dl_list_init(&hapd->auth_fail_list);
	hapd->dhcp_sock = -1;
#ifdef CONFIG_IEEE80211R_AP
	dl_list_init(&hapd->l2_queue);
	dl_list_init(&hapd->l2_oui_queue);
#ifdef CONFIG_SAE
	dl_list_init(&hapd->sae_commit_queue);
#endif /* CONFIG_SAE */
#endif /* CONFIG_IEEE80211R_AP */

	return hapd;
}


static void hostapd_bss_deinit(struct hostapd_data *hapd)
{
	if (!hapd)
		return;
	wpa_printf(MSG_DEBUG, "%s: deinit bss %s", __func__,
		   hapd->conf ? hapd->conf->iface : "N/A");
	hostapd_bss_deinit_no_free(hapd);
	wpa_msg_ap_state(hapd, AP_EVENT_DISABLED);
#ifdef CONFIG_SQLITE
	if (hapd->rad_attr_db) {
		sqlite3_close(hapd->rad_attr_db);
		hapd->rad_attr_db = NULL;
	}
#endif /* CONFIG_SQLITE */
	hostapd_cleanup(hapd);
}


void hostapd_interface_deinit(struct hostapd_iface *iface)
{
	int j;

	wpa_printf(MSG_DEBUG, "%s(%p)", __func__, iface);
	if (iface == NULL)
		return;

	hostapd_set_state(iface, HAPD_IFACE_DISABLED);

	eloop_cancel_timeout(channel_list_update_timeout, iface, NULL);
	iface->wait_channel_update = 0;

#ifdef CONFIG_FST
	if (iface->fst) {
		fst_detach(iface->fst);
		iface->fst = NULL;
	}
#endif /* CONFIG_FST */

	for (j = (int) iface->num_bss - 1; j >= 0; j--) {
		if (!iface->bss)
			break;
		hostapd_bss_deinit(iface->bss[j]);
	}

	eloop_cancel_timeout(ap_max_nss_omn_elem_timeout, iface, NULL);

#ifdef NEED_AP_MLME
	hostapd_stop_setup_timers(iface);
	eloop_cancel_timeout(ap_ht2040_timeout, iface, NULL);
#endif /* NEED_AP_MLME */
}


void hostapd_interface_free(struct hostapd_iface *iface)
{
	size_t j;
	wpa_printf(MSG_DEBUG, "%s(%p)", __func__, iface);
	for (j = 0; j < iface->num_bss; j++) {
		if (!iface->bss)
			break;
		wpa_printf(MSG_DEBUG, "%s: free hapd %p",
			   __func__, iface->bss[j]);
		os_free(iface->bss[j]);
	}
	hostapd_cleanup_iface(iface);
}


struct hostapd_iface * hostapd_alloc_iface(void)
{
	struct hostapd_iface *hapd_iface;

	hapd_iface = os_zalloc(sizeof(*hapd_iface));
	if (!hapd_iface)
		return NULL;

	dl_list_init(&hapd_iface->sta_seen);
	dl_list_init(&hapd_iface->dfs_history);

	return hapd_iface;
}


/**
 * hostapd_init - Allocate and initialize per-interface data
 * @config_file: Path to the configuration file
 * Returns: Pointer to the allocated interface data or %NULL on failure
 *
 * This function is used to allocate main data structures for per-interface
 * data. The allocated data buffer will be freed by calling
 * hostapd_cleanup_iface().
 */
struct hostapd_iface * hostapd_init(struct hapd_interfaces *interfaces,
				    const char *config_file)
{
	struct hostapd_iface *hapd_iface = NULL;
	struct hostapd_config *conf = NULL;
	struct hostapd_data *hapd;
	size_t i;

	hapd_iface = hostapd_alloc_iface();
	if (hapd_iface == NULL)
		goto fail;

	hapd_iface->config_fname = os_strdup(config_file);
	if (hapd_iface->config_fname == NULL)
		goto fail;

	conf = interfaces->config_read_cb(hapd_iface->config_fname);
	if (conf == NULL)
		goto fail;
	hapd_iface->conf = conf;

	hapd_iface->num_bss = conf->num_bss;
	hapd_iface->bss = os_calloc(conf->num_bss,
				    sizeof(struct hostapd_data *));
	if (hapd_iface->bss == NULL)
		goto fail;

	for (i = 0; i < conf->num_bss; i++) {
		hapd = hapd_iface->bss[i] =
			hostapd_alloc_bss_data(hapd_iface, conf,
					       conf->bss[i]);
		if (hapd == NULL)
			goto fail;
		hapd->msg_ctx = hapd;
	}

	return hapd_iface;

fail:
	wpa_printf(MSG_ERROR, "Failed to set up interface with %s",
		   config_file);
	if (conf)
		hostapd_config_free(conf);
	if (hapd_iface) {
		os_free(hapd_iface->config_fname);
		os_free(hapd_iface->bss);
		wpa_printf(MSG_DEBUG, "%s: free iface %p",
			   __func__, hapd_iface);
		os_free(hapd_iface);
	}
	return NULL;
}


static int ifname_in_use(struct hapd_interfaces *interfaces, const char *ifname)
{
	size_t i, j;

	for (i = 0; i < interfaces->count; i++) {
		struct hostapd_iface *iface = interfaces->iface[i];
		for (j = 0; j < iface->num_bss; j++) {
			struct hostapd_data *hapd = iface->bss[j];
			if (os_strcmp(ifname, hapd->conf->iface) == 0)
				return 1;
		}
	}

	return 0;
}


/**
 * hostapd_interface_init_bss - Read configuration file and init BSS data
 *
 * This function is used to parse configuration file for a BSS. This BSS is
 * added to an existing interface sharing the same radio (if any) or a new
 * interface is created if this is the first interface on a radio. This
 * allocate memory for the BSS. No actual driver operations are started.
 *
 * This is similar to hostapd_interface_init(), but for a case where the
 * configuration is used to add a single BSS instead of all BSSes for a radio.
 */
struct hostapd_iface *
hostapd_interface_init_bss(struct hapd_interfaces *interfaces, const char *phy,
			   const char *config_fname, int debug)
{
	struct hostapd_iface *new_iface = NULL, *iface = NULL;
	struct hostapd_data *hapd;
	int k;
	size_t i, bss_idx;

	if (!phy || !*phy)
		return NULL;

	for (i = 0; i < interfaces->count; i++) {
		if (os_strcmp(interfaces->iface[i]->phy, phy) == 0) {
			iface = interfaces->iface[i];
			break;
		}
	}

	wpa_printf(MSG_INFO, "Configuration file: %s (phy %s)%s",
		   config_fname, phy, iface ? "" : " --> new PHY");
	if (iface) {
		struct hostapd_config *conf;
		struct hostapd_bss_config **tmp_conf;
		struct hostapd_data **tmp_bss;
		struct hostapd_bss_config *bss;
		const char *ifname;

		/* Add new BSS to existing iface */
		conf = interfaces->config_read_cb(config_fname);
		if (conf == NULL)
			return NULL;
		if (conf->num_bss > 1) {
			wpa_printf(MSG_ERROR, "Multiple BSSes specified in BSS-config");
			hostapd_config_free(conf);
			return NULL;
		}

		ifname = conf->bss[0]->iface;
		if (ifname[0] != '\0' && ifname_in_use(interfaces, ifname)) {
			wpa_printf(MSG_ERROR,
				   "Interface name %s already in use", ifname);
			hostapd_config_free(conf);
			return NULL;
		}

		tmp_conf = os_realloc_array(
			iface->conf->bss, iface->conf->num_bss + 1,
			sizeof(struct hostapd_bss_config *));
		tmp_bss = os_realloc_array(iface->bss, iface->num_bss + 1,
					   sizeof(struct hostapd_data *));
		if (tmp_bss)
			iface->bss = tmp_bss;
		if (tmp_conf) {
			iface->conf->bss = tmp_conf;
			iface->conf->last_bss = tmp_conf[0];
		}
		if (tmp_bss == NULL || tmp_conf == NULL) {
			hostapd_config_free(conf);
			return NULL;
		}
		bss = iface->conf->bss[iface->conf->num_bss] = conf->bss[0];
		iface->conf->num_bss++;

		hapd = hostapd_alloc_bss_data(iface, iface->conf, bss);
		if (hapd == NULL) {
			iface->conf->num_bss--;
			hostapd_config_free(conf);
			return NULL;
		}
		iface->conf->last_bss = bss;
		iface->bss[iface->num_bss] = hapd;
		hapd->msg_ctx = hapd;

		bss_idx = iface->num_bss++;
		conf->num_bss--;
		conf->bss[0] = NULL;
		hostapd_config_free(conf);
	} else {
		/* Add a new iface with the first BSS */
		new_iface = iface = hostapd_init(interfaces, config_fname);
		if (!iface)
			return NULL;
		os_strlcpy(iface->phy, phy, sizeof(iface->phy));
		iface->interfaces = interfaces;
		bss_idx = 0;
	}

	for (k = 0; k < debug; k++) {
		if (iface->bss[bss_idx]->conf->logger_stdout_level > 0)
			iface->bss[bss_idx]->conf->logger_stdout_level--;
	}

	if (iface->conf->bss[bss_idx]->iface[0] == '\0' &&
	    !hostapd_drv_none(iface->bss[bss_idx])) {
		wpa_printf(MSG_ERROR, "Interface name not specified in %s",
			   config_fname);
		if (new_iface)
			hostapd_interface_deinit_free(new_iface);
		return NULL;
	}

	return iface;
}


void hostapd_interface_deinit_free(struct hostapd_iface *iface)
{
	const struct wpa_driver_ops *driver;
	void *drv_priv;

	wpa_printf(MSG_DEBUG, "%s(%p)", __func__, iface);
	if (iface == NULL)
		return;
	wpa_printf(MSG_DEBUG, "%s: num_bss=%u conf->num_bss=%u",
		   __func__, (unsigned int) iface->num_bss,
		   (unsigned int) iface->conf->num_bss);
	driver = iface->bss[0]->driver;
	drv_priv = iface->bss[0]->drv_priv;
	hostapd_interface_deinit(iface);
	wpa_printf(MSG_DEBUG, "%s: driver=%p drv_priv=%p -> hapd_deinit",
		   __func__, driver, drv_priv);
	if (driver && driver->hapd_deinit && drv_priv) {
		driver->hapd_deinit(drv_priv);
		iface->bss[0]->drv_priv = NULL;
	}
	hostapd_interface_free(iface);
}


static void hostapd_deinit_driver(const struct wpa_driver_ops *driver,
				  void *drv_priv,
				  struct hostapd_iface *hapd_iface)
{
	size_t j;

	wpa_printf(MSG_DEBUG, "%s: driver=%p drv_priv=%p -> hapd_deinit",
		   __func__, driver, drv_priv);
	if (driver && driver->hapd_deinit && drv_priv) {
		driver->hapd_deinit(drv_priv);
		for (j = 0; j < hapd_iface->num_bss; j++) {
			wpa_printf(MSG_DEBUG, "%s:bss[%d]->drv_priv=%p",
				   __func__, (int) j,
				   hapd_iface->bss[j]->drv_priv);
			if (hapd_iface->bss[j]->drv_priv == drv_priv) {
				hapd_iface->bss[j]->drv_priv = NULL;
				hapd_iface->extended_capa = NULL;
				hapd_iface->extended_capa_mask = NULL;
				hapd_iface->extended_capa_len = 0;
			}
		}
	}
}


int hostapd_enable_iface(struct hostapd_iface *hapd_iface)
{
	size_t j;

	if (hapd_iface->bss[0]->drv_priv != NULL) {
		wpa_printf(MSG_ERROR, "Interface %s already enabled",
			   hapd_iface->conf->bss[0]->iface);
		return -1;
	}

	wpa_printf(MSG_DEBUG, "Enable interface %s",
		   hapd_iface->conf->bss[0]->iface);

	for (j = 0; j < hapd_iface->num_bss; j++)
		hostapd_set_security_params(hapd_iface->conf->bss[j], 1);
	if (hostapd_config_check(hapd_iface->conf, 1) < 0) {
		wpa_printf(MSG_INFO, "Invalid configuration - cannot enable");
		return -1;
	}

	if (hapd_iface->interfaces == NULL ||
	    hapd_iface->interfaces->driver_init == NULL ||
	    hapd_iface->interfaces->driver_init(hapd_iface))
		return -1;

	if (hostapd_setup_interface(hapd_iface)) {
		hostapd_deinit_driver(hapd_iface->bss[0]->driver,
				      hapd_iface->bss[0]->drv_priv,
				      hapd_iface);
		return -1;
	}

	return 0;
}


static int hostapd_config_get_missing_bss(struct hostapd_config *conf,
		struct hostapd_config *new_conf)
{
	size_t i, j;
	bool found;

	for (i = 0; i < conf->num_bss; i++) {
		found = false;
		for (j = 0; j < new_conf->num_bss; j++) {
			if (os_strcmp(conf->bss[i]->iface, new_conf->bss[j]->iface) == 0) {
				found = true;
				break;
			}
		}

		if (!found)
			return i;
	}

	return -1;
}


static int hostapd_add_bss(struct hostapd_iface *iface,
		struct hostapd_config *new_conf, int new_bss_idx)
{
	struct hostapd_bss_config **tmp_conf_arr;
	struct hostapd_data **tmp_bss_arr;
	struct hostapd_data *hapd;
	int i, res;
	const char *ifname;

	UNUSED_VAR(ifname);
	ifname = new_conf->bss[new_bss_idx]->iface;
	wpa_printf(MSG_INFO, "%s, ifname=%s", __func__, ifname);

	/* Reallocate conf & bss arrays for new BSS */
	tmp_conf_arr = os_realloc_array(
			iface->conf->bss, iface->conf->num_bss + 1,
			sizeof(struct hostapd_bss_config *));
	if (tmp_conf_arr == NULL) {
		res = -ENOMEM;
		goto fail_conf_arr_realloc;
	}
	iface->conf->bss = tmp_conf_arr;
	iface->conf->num_bss++;

	tmp_bss_arr = os_realloc_array(iface->bss, iface->num_bss + 1,
			sizeof(struct hostapd_data *));

	if (tmp_bss_arr == NULL) {
		res = -ENOMEM;
		goto fail_bss_arr_realloc;
	}
	iface->bss = tmp_bss_arr;
	iface->num_bss++;

	/* Move bss_config from new conf to current conf */
	iface->conf->bss[iface->conf->num_bss - 1] = new_conf->bss[new_bss_idx];

	iface->conf->last_bss = new_conf->bss[new_bss_idx];
	new_conf->num_bss--;
	for (i = new_bss_idx; i < new_conf->num_bss; i++)
		new_conf->bss[i] = new_conf->bss[i + 1];

	for (i = 0; i < NUM_TX_QUEUES; i++){
		iface->conf->tx_queue[i] = new_conf->tx_queue[i];
		iface->conf->wmm_ac_params[i]= new_conf->wmm_ac_params[i];
	}

	/* allocating new bss data */
	hapd = hostapd_alloc_bss_data(iface, iface->conf,
			iface->conf->last_bss);
	if (hapd == NULL){
		res = -ENOMEM;
		goto fail_bss_data_alloc;
	}

	hapd->msg_ctx = hapd;
	iface->bss[iface->num_bss - 1] = hapd;

	/* Set up BSS will be done after interface enabled */
	if (iface->state != HAPD_IFACE_ENABLED) {
		/* In case non-inital ACS is running: do reconf (which will
		 * cancel ACS scan), and after scan aborted restart the ACS again */
		if ((iface->state == HAPD_IFACE_ACS) && iface->conf->acs_init_done)
			iface->retrigger_acs = true;
		else
			return 0;
	}

	/*
	 * when mem_only_cred configuration is set we will halt init of bss and request
	 * credentials from the UI
	 */
	if(hapd->conf->ssid.request_credentials)
	{
		send_cred_request_to_ui(hapd);
 	}
	else {
		if (hostapd_setup_bss(hapd, false)) {
			res = -EINVAL;
			goto fail_setup_bss;
		}

		/* send set WMM to driver for new BSS */
		hostapd_tx_queue_params(iface->bss[0]);
	}

	return 0;

fail_setup_bss:
	os_free(hapd);
fail_bss_data_alloc:
	iface->bss[iface->num_bss - 1] = NULL;
	iface->num_bss--;
fail_bss_arr_realloc:
	iface->conf->bss[iface->conf->num_bss - 1] = NULL;
	iface->conf->num_bss--;
fail_conf_arr_realloc:
	return res;
}

int hostapd_reconf_iface(struct hostapd_iface *hapd_iface, int changed_idx)
{
	struct hostapd_config *new_conf = NULL;
	int idx, res;
	bool found_missing_bss;

	wpa_printf(MSG_DEBUG, "Reconf interface %s", hapd_iface->conf->bss[0]->iface);
	if (hapd_iface->interfaces == NULL ||
		hapd_iface->interfaces->config_read_cb == NULL)
		return -1;
	new_conf = hapd_iface->interfaces->config_read_cb(hapd_iface->config_fname);
	if (new_conf == NULL)
		return -EINVAL;

	if (changed_idx > 0)
		/* changed_idx is a BSS index that needs to be modified.
		 * Instead of really modifying, we will just remove and add the BSS.
		 * This is better because some BSS configurations must be set
		 * to firmware before add VAP is made.
		 */
		hostapd_remove_bss(hapd_iface, changed_idx);

	/* Find BSS needed to be removed */
	do {
		idx = hostapd_config_get_missing_bss(hapd_iface->conf, new_conf);
		/* zero isn't a valid index because we don't support
		 * removing master BSS */
		found_missing_bss = idx > 0;
		if (found_missing_bss)
			hostapd_remove_bss(hapd_iface, idx);
	} while (found_missing_bss);

	/* Find BSS needed to be added */
	do {
		idx = hostapd_config_get_missing_bss(new_conf, hapd_iface->conf);
		/* zero isn't a valid index because there must be at least 1 BSS */
		found_missing_bss = idx > 0;
		if (found_missing_bss) {
			res = hostapd_add_bss(hapd_iface, new_conf, idx);
			if (res) {
				wpa_printf(MSG_ERROR, "Failed adding new BSS (%s), res=%d",
						new_conf->bss[idx]->iface, res);
				hostapd_config_free(new_conf);
				return -1;
			}

			idx = hapd_iface->num_bss - 1;
			hostapd_flush_old_stations(hapd_iface->bss[idx], WLAN_REASON_PREV_AUTH_NOT_VALID);

			if (start_ctrl_iface_bss(hapd_iface->bss[idx])){
				wpa_printf(MSG_ERROR, "Failed adding new BSS (%s) to control interface",
						new_conf->bss[idx]->iface);
				hostapd_config_free(new_conf);
				return -1;
			}
		}
	} while (found_missing_bss);

	hostapd_config_free(new_conf);

	return 0;
}


int hostapd_reload_iface(struct hostapd_iface *hapd_iface)
{
	size_t j;

	wpa_printf(MSG_DEBUG, "Reload interface %s",
		   hapd_iface->conf->bss[0]->iface);
	for (j = 0; j < hapd_iface->num_bss; j++)
		hostapd_set_security_params(hapd_iface->conf->bss[j], 1);
	if (hostapd_config_check(hapd_iface->conf, 1) < 0) {
		wpa_printf(MSG_ERROR, "Updated configuration is invalid");
		return -1;
	}
	hostapd_clear_old(hapd_iface);
	for (j = 0; j < hapd_iface->num_bss; j++)
		hostapd_reload_bss(hapd_iface->bss[j]);

	return 0;
}


int hostapd_disable_iface(struct hostapd_iface *hapd_iface)
{
	size_t j;
	const struct wpa_driver_ops *driver;
	void *drv_priv;

	if (hapd_iface == NULL)
		return -1;

	if (hapd_iface->bss[0]->drv_priv == NULL) {
		wpa_printf(MSG_INFO, "Interface %s already disabled",
			   hapd_iface->conf->bss[0]->iface);
		return -1;
	}

	if (hapd_iface->whm_if_fail) {
		wpa_printf(MSG_WARNING, "WHM-IFDOWN: %s is being disabled,"
					" Trigger WHM warning id - %d",
					hapd_iface->bss[0]->conf->iface, WHM_HOSTAP_IFDOWN);
		hostapd_drv_set_whm_trigger(hapd_iface->bss[0], WHM_HOSTAP_IFDOWN);
	}
	/* reset to default value */
	hapd_iface->whm_if_fail = true;

	if( invalidate_rnr_in_beacon(hapd_iface->interfaces->iface[0]) ) {
		wpa_printf(MSG_ERROR, "%s:%d Failed to invalidate RNR IE",
								__func__, __LINE__);
		return -1;
	}

	for (j = 0; j < hapd_iface->num_bss; j++)
		wpa_msg_ap_state(hapd_iface->bss[j], AP_EVENT_DISABLED);
	driver = hapd_iface->bss[0]->driver;
	drv_priv = hapd_iface->bss[0]->drv_priv;

	hapd_iface->driver_ap_teardown =
		!!(hapd_iface->drv_flags &
		   WPA_DRIVER_FLAGS_AP_TEARDOWN_SUPPORT);

#ifdef NEED_AP_MLME
	for (j = 0; j < hapd_iface->num_bss; j++)
		hostapd_cleanup_cs_params(hapd_iface->bss[j]);
#endif /* NEED_AP_MLME */

	/* same as hostapd_interface_deinit without deinitializing ctrl-iface */
	for (j = 0; j < hapd_iface->num_bss; j++) {
		struct hostapd_data *hapd = hapd_iface->bss[j];
		hostapd_bss_deinit_no_free(hapd);
		hostapd_free_hapd_data(hapd);
	}

	hostapd_deinit_driver(driver, drv_priv, hapd_iface);

	/* From hostapd_cleanup_iface: These were initialized in
	 * hostapd_setup_interface and hostapd_setup_interface_complete
	 */
	hostapd_cleanup_iface_partial(hapd_iface);
	hapd_iface->dbg_dfs_in_cac = 0;

	wpa_printf(MSG_DEBUG, "Interface %s disabled",
		   hapd_iface->bss[0]->conf->iface);
	hostapd_set_state(hapd_iface, HAPD_IFACE_DISABLED);

	if( auto_update_rnr_in_beacon(hapd_iface->interfaces->iface[0]) ) {
		wpa_printf(MSG_ERROR, "%s:%d Failed to update RNR IE",
								__func__, __LINE__);
		return -1;
	}

	return 0;
}


static struct hostapd_iface *
hostapd_iface_alloc(struct hapd_interfaces *interfaces)
{
	struct hostapd_iface **iface, *hapd_iface;

	iface = os_realloc_array(interfaces->iface, interfaces->count + 1,
				 sizeof(struct hostapd_iface *));
	if (iface == NULL)
		return NULL;
	interfaces->iface = iface;
	hapd_iface = interfaces->iface[interfaces->count] =
		hostapd_alloc_iface();
	if (hapd_iface == NULL) {
		wpa_printf(MSG_ERROR, "%s: Failed to allocate memory for "
			   "the interface", __func__);
		return NULL;
	}
	interfaces->count++;
	hapd_iface->interfaces = interfaces;

	return hapd_iface;
}


static struct hostapd_config *
hostapd_config_alloc(struct hapd_interfaces *interfaces, const char *ifname,
		     const char *ctrl_iface, const char *driver)
{
	struct hostapd_bss_config *bss;
	struct hostapd_config *conf;

	/* Allocates memory for bss and conf */
	conf = hostapd_config_defaults();
	if (conf == NULL) {
		 wpa_printf(MSG_ERROR, "%s: Failed to allocate memory for "
				"configuration", __func__);
		 return NULL;
	}

	if (driver) {
		int j;

		for (j = 0; wpa_drivers[j]; j++) {
			if (os_strcmp(driver, wpa_drivers[j]->name) == 0) {
				conf->driver = wpa_drivers[j];
				goto skip;
			}
		}

		wpa_printf(MSG_ERROR,
			   "Invalid/unknown driver '%s' - registering the default driver",
			   driver);
	}

	conf->driver = wpa_drivers[0];
	if (conf->driver == NULL) {
		wpa_printf(MSG_ERROR, "No driver wrappers registered!");
		hostapd_config_free(conf);
		return NULL;
	}

skip:
	bss = conf->last_bss = conf->bss[0];

	os_strlcpy(bss->iface, ifname, sizeof(bss->iface));
	bss->ctrl_interface = os_strdup(ctrl_iface);
	if (bss->ctrl_interface == NULL) {
		hostapd_config_free(conf);
		return NULL;
	}

	/* Reading configuration file skipped, will be done in SET!
	 * From reading the configuration till the end has to be done in
	 * SET
	 */
	return conf;
}


static int hostapd_data_alloc(struct hostapd_iface *hapd_iface,
			      struct hostapd_config *conf)
{
	size_t i;
	struct hostapd_data *hapd;

	hapd_iface->bss = os_calloc(conf->num_bss,
				    sizeof(struct hostapd_data *));
	if (hapd_iface->bss == NULL)
		return -1;

	for (i = 0; i < conf->num_bss; i++) {
		hapd = hapd_iface->bss[i] =
			hostapd_alloc_bss_data(hapd_iface, conf, conf->bss[i]);
		if (hapd == NULL) {
			while (i > 0) {
				i--;
				os_free(hapd_iface->bss[i]);
				hapd_iface->bss[i] = NULL;
			}
			os_free(hapd_iface->bss);
			hapd_iface->bss = NULL;
			return -1;
		}
		hapd->msg_ctx = hapd;
	}

	hapd_iface->conf = conf;
	hapd_iface->num_bss = conf->num_bss;

	return 0;
}


int hostapd_add_iface(struct hapd_interfaces *interfaces, char *buf)
{
	struct hostapd_config *conf = NULL;
	struct hostapd_iface *hapd_iface = NULL, *new_iface = NULL;
	struct hostapd_data *hapd;
	char *ptr;
	size_t i, j;
	const char *conf_file = NULL, *phy_name = NULL;

	if (os_strncmp(buf, "bss_config=", 11) == 0) {
		char *pos;
		phy_name = buf + 11;
		pos = os_strchr(phy_name, ':');
		if (!pos)
			return -1;
		*pos++ = '\0';
		conf_file = pos;
		if (!os_strlen(conf_file))
			return -1;

		hapd_iface = hostapd_interface_init_bss(interfaces, phy_name,
							conf_file, 0);
		if (!hapd_iface)
			return -1;
		for (j = 0; j < interfaces->count; j++) {
			if (interfaces->iface[j] == hapd_iface)
				break;
		}
		if (j == interfaces->count) {
			struct hostapd_iface **tmp;
			tmp = os_realloc_array(interfaces->iface,
					       interfaces->count + 1,
					       sizeof(struct hostapd_iface *));
			if (!tmp) {
				hostapd_interface_deinit_free(hapd_iface);
				return -1;
			}
			interfaces->iface = tmp;
			interfaces->iface[interfaces->count++] = hapd_iface;
			new_iface = hapd_iface;
		}

		if (new_iface) {
			if (interfaces->driver_init(hapd_iface))
				goto fail;

			if (hostapd_setup_interface(hapd_iface)) {
				hostapd_deinit_driver(
					hapd_iface->bss[0]->driver,
					hapd_iface->bss[0]->drv_priv,
					hapd_iface);
				goto fail;
			}
		} else {
			/* Assign new BSS with bss[0]'s driver info */
			hapd = hapd_iface->bss[hapd_iface->num_bss - 1];
			hapd->driver = hapd_iface->bss[0]->driver;
			hapd->drv_priv = hapd_iface->bss[0]->drv_priv;
			os_memcpy(hapd->own_addr, hapd_iface->bss[0]->own_addr,
				  ETH_ALEN);

			if (start_ctrl_iface_bss(hapd) < 0 ||
			    (hapd_iface->state == HAPD_IFACE_ENABLED &&
			     hostapd_setup_bss(hapd, -1))) {
				hostapd_cleanup(hapd);
				hapd_iface->bss[hapd_iface->num_bss - 1] = NULL;
				hapd_iface->conf->num_bss--;
				hapd_iface->num_bss--;
				wpa_printf(MSG_DEBUG, "%s: free hapd %p %s",
					   __func__, hapd, hapd->conf->iface);
				hostapd_config_free_bss(hapd->conf);
				hapd->conf = NULL;
				os_free(hapd);
				return -1;
			}
		}
		hostapd_owe_update_trans(hapd_iface);
		return 0;
	}

	ptr = os_strchr(buf, ' ');
	if (ptr == NULL)
		return -1;
	*ptr++ = '\0';

	if (os_strncmp(ptr, "config=", 7) == 0)
		conf_file = ptr + 7;

	for (i = 0; i < interfaces->count; i++) {
		if (!os_strcmp(interfaces->iface[i]->conf->bss[0]->iface,
			       buf)) {
			wpa_printf(MSG_INFO, "Cannot add interface - it "
				   "already exists");
			return -1;
		}
	}

	hapd_iface = hostapd_iface_alloc(interfaces);
	if (hapd_iface == NULL) {
		wpa_printf(MSG_ERROR, "%s: Failed to allocate memory "
			   "for interface", __func__);
		goto fail;
	}
	new_iface = hapd_iface;

	if (conf_file && interfaces->config_read_cb) {
		conf = interfaces->config_read_cb(conf_file);
		if (conf && conf->bss)
			os_strlcpy(conf->bss[0]->iface, buf,
				   sizeof(conf->bss[0]->iface));
		new_iface->config_fname = os_strdup(conf_file);
		if (new_iface->config_fname == NULL) {
			wpa_printf(MSG_ERROR, "%s: Failed to allocate memory "
				  "for conf file name", __func__);
			goto fail;
		}
	} else {
		char *driver = os_strchr(ptr, ' ');

		if (driver)
			*driver++ = '\0';
		conf = hostapd_config_alloc(interfaces, buf, ptr, driver);
	}

	if (conf == NULL || conf->bss == NULL) {
		wpa_printf(MSG_ERROR, "%s: Failed to allocate memory "
			   "for configuration", __func__);
		goto fail;
	}

	if (hostapd_data_alloc(hapd_iface, conf) < 0) {
		wpa_printf(MSG_ERROR, "%s: Failed to allocate memory "
			   "for hostapd", __func__);
		goto fail;
	}
	conf = NULL;

	if (start_ctrl_iface(hapd_iface) < 0)
		goto fail;

	wpa_printf(MSG_INFO, "Add interface '%s'",
		   hapd_iface->conf->bss[0]->iface);

	return 0;

fail:
	if (conf)
		hostapd_config_free(conf);
	if (hapd_iface) {
		if (hapd_iface->bss) {
			for (i = 0; i < hapd_iface->num_bss; i++) {
				hapd = hapd_iface->bss[i];
				if (!hapd)
					continue;
				if (hapd_iface->interfaces &&
				    hapd_iface->interfaces->ctrl_iface_deinit)
					hapd_iface->interfaces->
						ctrl_iface_deinit(hapd);
				wpa_printf(MSG_DEBUG, "%s: free hapd %p (%s)",
					   __func__, hapd_iface->bss[i],
					   hapd->conf->iface);
				hostapd_cleanup(hapd);
				os_free(hapd);
				hapd_iface->bss[i] = NULL;
			}
			os_free(hapd_iface->bss);
			hapd_iface->bss = NULL;
		}
		if (new_iface) {
			interfaces->count--;
			interfaces->iface[interfaces->count] = NULL;
		}
		hostapd_cleanup_iface(hapd_iface);
	}
	return -1;
}


static int hostapd_remove_bss(struct hostapd_iface *iface, unsigned int idx)
{
	size_t i;

	wpa_printf(MSG_INFO, "Remove BSS '%s'", iface->conf->bss[idx]->iface);

	/* Remove hostapd_data only if it has already been initialized */
	if (idx < iface->num_bss) {
		struct hostapd_data *hapd = iface->bss[idx];

		hostapd_bss_deinit(hapd);

		wpa_printf(MSG_DEBUG, "%s: free hapd %p (%s)",
			   __func__, hapd, hapd->conf->iface);
		hostapd_config_free_bss(hapd->conf);
		hapd->conf = NULL;
		os_free(hapd);

		iface->num_bss--;

		for (i = idx; i < iface->num_bss; i++)
			iface->bss[i] = iface->bss[i + 1];
	} else {
		hostapd_config_free_bss(iface->conf->bss[idx]);
		iface->conf->bss[idx] = NULL;
	}

	iface->conf->num_bss--;
	for (i = idx; i < iface->conf->num_bss; i++)
		iface->conf->bss[i] = iface->conf->bss[i + 1];

	return 0;
}


int hostapd_remove_iface(struct hapd_interfaces *interfaces, char *buf)
{
	struct hostapd_iface *hapd_iface;
	size_t i, j, k = 0;

	for (i = 0; i < interfaces->count; i++) {
		hapd_iface = interfaces->iface[i];
		if (hapd_iface == NULL)
			return -1;
		if (!os_strcmp(hapd_iface->conf->bss[0]->iface, buf)) {

			if( invalidate_rnr_in_beacon(interfaces->iface[0]) ) {
				wpa_printf(MSG_ERROR, "%s:%d Failed to invalidate RNR IE",
									__func__, __LINE__);
				return -1;
			}

			wpa_printf(MSG_INFO, "Remove interface '%s'", buf);
			hapd_iface->driver_ap_teardown =
				!!(hapd_iface->drv_flags &
				   WPA_DRIVER_FLAGS_AP_TEARDOWN_SUPPORT);

			hostapd_interface_deinit_free(hapd_iface);
			k = i;
			while (k < (interfaces->count - 1)) {
				interfaces->iface[k] =
					interfaces->iface[k + 1];
				k++;
			}
			interfaces->count--;

			if(interfaces->count && auto_update_rnr_in_beacon(interfaces->iface[0])) {
				wpa_printf(MSG_ERROR, "%s:%d Failed to update RNR IE",
									__func__, __LINE__);
				return -1;
			}

			return 0;
		}

		for (j = 0; j < hapd_iface->conf->num_bss; j++) {
			if (!os_strcmp(hapd_iface->conf->bss[j]->iface, buf)) {
				hapd_iface->driver_ap_teardown =
					!(hapd_iface->drv_flags &
					  WPA_DRIVER_FLAGS_AP_TEARDOWN_SUPPORT);
				return hostapd_remove_bss(hapd_iface, j);
			}
		}
	}
	return -1;
}


/**
 * hostapd_new_assoc_sta - Notify that a new station associated with the AP
 * @hapd: Pointer to BSS data
 * @sta: Pointer to the associated STA data
 * @reassoc: 1 to indicate this was a re-association; 0 = first association
 *
 * This function will be called whenever a station associates with the AP. It
 * can be called from ieee802_11.c for drivers that export MLME to hostapd and
 * from drv_callbacks.c based on driver events for drivers that take care of
 * management frames (IEEE 802.11 authentication and association) internally.
 */
void hostapd_new_assoc_sta(struct hostapd_data *hapd, struct sta_info *sta,
			   int reassoc)
{
	if (hapd->tkip_countermeasures) {
		hostapd_drv_sta_deauth(hapd, sta->addr,
				       WLAN_REASON_MICHAEL_MIC_FAILURE);
		return;
	}

	hostapd_prune_associations(hapd, sta->addr);
	ap_sta_clear_disconnect_timeouts(hapd, sta);

#ifdef CONFIG_P2P
	if (sta->p2p_ie == NULL && !sta->no_p2p_set) {
		sta->no_p2p_set = 1;
		hapd->num_sta_no_p2p++;
		if (hapd->num_sta_no_p2p == 1)
			hostapd_p2p_non_p2p_sta_connected(hapd);
	}
#endif /* CONFIG_P2P */

	airtime_policy_new_sta(hapd, sta);

	/* Start accounting here, if IEEE 802.1X and WPA are not used.
	 * IEEE 802.1X/WPA code will start accounting after the station has
	 * been authorized. */
	if (!hapd->conf->ieee802_1x && !hapd->conf->wpa && !hapd->conf->osen) {
		ap_sta_set_authorized(hapd, sta, 1);
		os_get_reltime(&sta->connected_time);
		accounting_sta_start(hapd, sta);
	}

	/* Start IEEE 802.1X authentication process for new stations */
	ieee802_1x_new_station(hapd, sta);
	if (reassoc) {
		if (sta->auth_alg != WLAN_AUTH_FT &&
		    sta->auth_alg != WLAN_AUTH_FILS_SK &&
		    sta->auth_alg != WLAN_AUTH_FILS_SK_PFS &&
		    sta->auth_alg != WLAN_AUTH_FILS_PK &&
		    !(sta->flags & (WLAN_STA_WPS | WLAN_STA_MAYBE_WPS)))
			wpa_auth_sm_event(sta->wpa_sm, WPA_REAUTH);
	} else {
		wpa_auth_sta_associated(hapd->wpa_auth, sta->wpa_sm);
		hostapd_event_ltq_report_event(hapd, WLAN_FC_STYPE_ASSOC_RESP,
			HOSTAPD_EVENT_ASSOC_NEW_CONNECTION_COMPLETE, sta->addr, false);
	}

	if (hapd->iface->drv_flags & WPA_DRIVER_FLAGS_WIRED) {
		if (eloop_cancel_timeout(ap_handle_timer, hapd, sta) > 0) {
			wpa_printf(MSG_DEBUG,
				   "%s: %s: canceled wired ap_handle_timer timeout for "
				   MACSTR,
				   hapd->conf->iface, __func__,
				   MAC2STR(sta->addr));
		}
	} else if (!(hapd->iface->drv_flags &
		     WPA_DRIVER_FLAGS_INACTIVITY_TIMER)) {
		wpa_printf(MSG_DEBUG,
			   "%s: %s: reschedule ap_handle_timer timeout for "
			   MACSTR " (%d seconds - ap_max_inactivity)",
			   hapd->conf->iface, __func__, MAC2STR(sta->addr),
			   hapd->conf->ap_max_inactivity);
		eloop_cancel_timeout(ap_handle_timer, hapd, sta);
		eloop_register_timeout(hapd->conf->ap_max_inactivity, 0,
				       ap_handle_timer, hapd, sta);
	}

#ifdef CONFIG_MACSEC
	if (hapd->conf->wpa_key_mgmt == WPA_KEY_MGMT_NONE &&
	    hapd->conf->mka_psk_set)
		ieee802_1x_create_preshared_mka_hapd(hapd, sta);
	else
		ieee802_1x_alloc_kay_sm_hapd(hapd, sta);
#endif /* CONFIG_MACSEC */
}


const char * hostapd_state_text(enum hostapd_iface_state s)
{
	switch (s) {
	case HAPD_IFACE_UNINITIALIZED:
		return "UNINITIALIZED";
	case HAPD_IFACE_DISABLED:
		return "DISABLED";
	case HAPD_IFACE_COUNTRY_UPDATE:
		return "COUNTRY_UPDATE";
	case HAPD_IFACE_ACS:
		return "ACS";
	case HAPD_IFACE_ACS_DONE:
		return "ACS_DONE";
	case HAPD_IFACE_HT_SCAN:
		return "HT_SCAN";
	case HAPD_IFACE_DFS:
		return "DFS";
	case HAPD_IFACE_ENABLED:
		return "ENABLED";
	}

	return "UNKNOWN";
}


const char * hostapd_channel_switch_text(enum hostapd_channel_switch_reason s)
{
  switch (s) {
  case HAPD_CHAN_SWITCH_OTHER:
    return "UNKNOWN";
  case HAPD_CHAN_SWITCH_RADAR_DETECTED:
    return "RADAR";
  }

  return "UNKNOWN";
}

void hostapd_channel_switch_reason_set(struct hostapd_iface *iface,
				       enum hostapd_channel_switch_reason reason)
{
	unsigned int i;

	for (i = 0; i < iface->num_bss; i++)
		iface->bss[i]->chan_switch_reason = reason;
}

void enable_iface_delayed_cb(void *eloop_ctx, void *user_data)
{
	struct hostapd_data *hapd = eloop_ctx;
	struct hostapd_iface *iface = user_data;
	eloop_cancel_timeout(enable_iface_delayed_cb, hapd, iface);

	/* Already in DFS or Enabled state.
	  Might happen due to watchdog timer expired
	  on start_after_watchdog_time
	  and enabled delayed interface already. */
	if(iface->state > HAPD_IFACE_HT_SCAN)
		return;

	if(hostapd_setup_interface_complete_sync(iface, EOK)) {
		wpa_msg(hapd->msg_ctx, MSG_ERROR,
			"Failed to enable iface:%s\n",
			iface->bss[MASTER_VAP_BSS_IDX]->conf->iface);
	} else {
		wpa_msg(hapd->msg_ctx, MSG_INFO, "Enabled iface %s after %s\n",
			iface->bss[MASTER_VAP_BSS_IDX]->conf->iface, iface->conf->start_after);

		if(delayed_iface_file_create(iface)) {
			wpa_msg(hapd->msg_ctx, MSG_ERROR,
					"Failed to open control file for %s after %s started\n",
					iface->bss[MASTER_VAP_BSS_IDX]->conf->iface,
					iface->conf->start_after);
			return;
		}
	}
}

static void hostapd_enable_dependent_iface(struct hostapd_iface *iface)
{
	int i;

	for (i = 0; i < iface->interfaces->count; i++) {
		struct hostapd_iface *delayed_iface = iface->interfaces->iface[i];
		if(NULL_CHAR != delayed_iface->conf->start_after[0] && !delayed_iface_file_present(delayed_iface)) {
			struct hostapd_data *current_hapd = iface->bss[MASTER_VAP_BSS_IDX];
			int start_after_delay = delayed_iface->conf->start_after_delay;
			if(!os_strcmp(delayed_iface->conf->start_after, current_hapd->conf->iface)) {
#ifndef CONFIG_NO_STDOUT_DEBUG
				struct hostapd_data *delayed_hapd = delayed_iface->bss[MASTER_VAP_BSS_IDX];
#endif
				wpa_printf(MSG_DEBUG, "%s:%d current iface:%s delayed:%s start_after_delay:%d",
						__func__, __LINE__, current_hapd->conf->iface,
							delayed_hapd->conf->iface, start_after_delay);
				if (!eloop_is_timeout_registered(enable_iface_delayed_cb, current_hapd, delayed_iface))
					eloop_register_timeout(start_after_delay, 0, enable_iface_delayed_cb,
											current_hapd, delayed_iface);
				else
					wpa_printf(MSG_DEBUG, "%s:%d time out already registered for iface:%s",
						__func__, __LINE__, delayed_hapd->conf->iface);
			}
		}
	}
}

void hostapd_set_state(struct hostapd_iface *iface, enum hostapd_iface_state s)
{
	wpa_printf(MSG_INFO, "%s: interface state %s->%s",
		   iface->conf ? iface->conf->bss[0]->iface : "N/A",
		   hostapd_state_text(iface->state), hostapd_state_text(s));
	iface->state = s;

	if(HAPD_IFACE_ENABLED == s)
		hostapd_enable_dependent_iface(iface);
}


int hostapd_csa_in_progress(struct hostapd_iface *iface)
{
	unsigned int i;

	for (i = 0; i < iface->num_bss; i++)
		if (iface->bss[i]->csa_in_progress)
			return 1;
	return 0;
}

int hostapd_get_cs_count(struct hostapd_iface *iface)
{
	if (iface->dfs_domain == HOSTAPD_DFS_REGION_ETSI) {
		return DEFAULT_CS_COUNT;
	}
	return DEFAULT_CS_COUNT_NON_ETSI;
}

#ifdef NEED_AP_MLME

static void free_beacon_data(struct beacon_data *beacon)
{
	os_free(beacon->head);
	beacon->head = NULL;
	os_free(beacon->tail);
	beacon->tail = NULL;
	os_free(beacon->probe_resp);
	beacon->probe_resp = NULL;
	os_free(beacon->beacon_ies);
	beacon->beacon_ies = NULL;
	os_free(beacon->proberesp_ies);
	beacon->proberesp_ies = NULL;
	os_free(beacon->assocresp_ies);
	beacon->assocresp_ies = NULL;
}


static int hostapd_build_beacon_data(struct hostapd_data *hapd,
				     struct beacon_data *beacon)
{
	struct wpabuf *beacon_extra, *proberesp_extra, *assocresp_extra;
	struct wpa_driver_ap_params params;
	int ret;

	os_memset(beacon, 0, sizeof(*beacon));
	ret = ieee802_11_build_ap_params(hapd, &params);
	if (ret < 0)
		return ret;

	ret = hostapd_build_ap_extra_ies(hapd, &beacon_extra,
					 &proberesp_extra,
					 &assocresp_extra);
	if (ret)
		goto free_ap_params;

	ret = -1;
	beacon->head = os_memdup(params.head, params.head_len);
	if (!beacon->head)
		goto free_ap_extra_ies;

	beacon->head_len = params.head_len;

	beacon->tail = os_memdup(params.tail, params.tail_len);
	if (!beacon->tail)
		goto free_beacon;

	beacon->tail_len = params.tail_len;

	if (params.proberesp != NULL) {
		beacon->probe_resp = os_memdup(params.proberesp,
					       params.proberesp_len);
		if (!beacon->probe_resp)
			goto free_beacon;

		beacon->probe_resp_len = params.proberesp_len;
	}

	/* copy the extra ies */
	if (beacon_extra) {
		beacon->beacon_ies = os_memdup(beacon_extra->buf,
					       wpabuf_len(beacon_extra));
		if (!beacon->beacon_ies)
			goto free_beacon;

		beacon->beacon_ies_len = wpabuf_len(beacon_extra);
	}

	if (proberesp_extra) {
		beacon->proberesp_ies = os_memdup(proberesp_extra->buf,
						  wpabuf_len(proberesp_extra));
		if (!beacon->proberesp_ies)
			goto free_beacon;

		beacon->proberesp_ies_len = wpabuf_len(proberesp_extra);
	}

	if (assocresp_extra) {
		beacon->assocresp_ies = os_memdup(assocresp_extra->buf,
						  wpabuf_len(assocresp_extra));
		if (!beacon->assocresp_ies)
			goto free_beacon;

		beacon->assocresp_ies_len = wpabuf_len(assocresp_extra);
	}

	ret = 0;
free_beacon:
	/* if the function fails, the caller should not free beacon data */
	if (ret)
		free_beacon_data(beacon);

free_ap_extra_ies:
	hostapd_free_ap_extra_ies(hapd, beacon_extra, proberesp_extra,
				  assocresp_extra);
free_ap_params:
	ieee802_11_free_ap_params(&params);
	return ret;
}


/*
 * TODO: This flow currently supports only changing channel and width within
 * the same hw_mode. Any other changes to MAC parameters or provided settings
 * are not supported.
 */
static int hostapd_change_config_freq(struct hostapd_data *hapd,
				      struct hostapd_config *conf,
				      struct hostapd_freq_params *params,
				      struct hostapd_freq_params *old_params)
{
	int channel;
	u8 seg0, seg1;

	if (!params->channel) {
		/* check if the new channel is supported by hw */
		params->channel = hostapd_hw_get_channel(hapd, params->freq);
	}

	channel = params->channel;
	if (!channel)
		return -1;

	/* if a pointer to old_params is provided we save previous state */
	if (old_params &&
	    hostapd_set_freq_params(old_params, conf->hw_mode,
				    hostapd_hw_get_freq(hapd, conf->channel),
				    conf->channel, conf->enable_edmg,
				    conf->edmg_channel, conf->ieee80211n,
				    conf->ieee80211ac, conf->ieee80211ax,
				    conf->secondary_channel,
				    hostapd_get_oper_chwidth(conf),
                                    hostapd_get_oper_centr_freq_seg0_idx(conf),
                                    hostapd_get_oper_centr_freq_seg1_idx(conf),
				    hapd->iface->current_mode->vht_capab,
				    &hapd->iface->current_mode->he_capab))
		return -1;

	switch (params->bandwidth) {
	case 0:
	case 20:
	case 40:
		hostapd_set_oper_chwidth(conf,CHANWIDTH_USE_HT);
		break;
	case 80:
		if (params->center_freq2)
			hostapd_set_oper_chwidth(conf,CHANWIDTH_80P80MHZ);
		else
			hostapd_set_oper_chwidth(conf,CHANWIDTH_80MHZ);;
		break;
	case 160:
		hostapd_set_oper_chwidth(conf,CHANWIDTH_160MHZ);;
		break;
	default:
		return -1;
	}

	conf->channel = channel;
        conf->ieee80211n = params->ht_enabled;
        conf->secondary_channel = params->sec_channel_offset;
        ieee80211_freq_to_chan(params->center_freq1,
                               &seg0);
        ieee80211_freq_to_chan(params->center_freq2,
                               &seg1);
        hostapd_set_oper_centr_freq_seg0_idx(conf, seg0);
        hostapd_set_oper_centr_freq_seg1_idx(conf, seg1);

	/* TODO: maybe call here hostapd_config_check here? */

	return 0;
}


static int hostapd_fill_csa_settings(struct hostapd_data *hapd,
				     struct csa_settings *settings)
{
	struct hostapd_iface *iface = hapd->iface;
	struct hostapd_freq_params old_freq;
	int ret;
	u8 chan, bandwidth;

	os_memset(&old_freq, 0, sizeof(old_freq));
	if (!iface || !iface->freq || hapd->csa_in_progress)
		return -1;

	switch (settings->freq_params.bandwidth) {
	case 80:
		if (settings->freq_params.center_freq2)
			bandwidth = CHANWIDTH_80P80MHZ;
		else
			bandwidth = CHANWIDTH_80MHZ;
		break;
	case 160:
		bandwidth = CHANWIDTH_160MHZ;
		break;
	default:
		bandwidth = CHANWIDTH_USE_HT;
		break;
	}

	if (ieee80211_freq_to_channel_ext(
		    settings->freq_params.freq,
		    settings->freq_params.sec_channel_offset,
		    bandwidth,
		    &hapd->iface->cs_oper_class,
		    &chan) == NUM_HOSTAPD_MODES) {
		wpa_printf(MSG_DEBUG,
			   "invalid frequency for channel switch (freq=%d, sec_channel_offset=%d, vht_enabled=%d)",
			   settings->freq_params.freq,
			   settings->freq_params.sec_channel_offset,
			   settings->freq_params.vht_enabled);
		return -1;
	}

	settings->freq_params.channel = chan;

	ret = hostapd_change_config_freq(iface->bss[0], iface->conf,
					 &settings->freq_params,
					 &old_freq);
	if (ret)
		return ret;

	ret = hostapd_build_beacon_data(hapd, &settings->beacon_after);

	/* change back the configuration */
	hostapd_change_config_freq(iface->bss[0], iface->conf,
				   &old_freq, NULL);

	if (ret)
		return ret;

	/* set channel switch parameters for csa ie */
	hapd->cs_freq_params = settings->freq_params;
	hapd->cs_count = settings->cs_count;
	hapd->cs_block_tx = settings->block_tx;

	ret = hostapd_build_beacon_data(hapd, &settings->beacon_csa);
	if (ret) {
		free_beacon_data(&settings->beacon_after);
		return ret;
	}

	settings->counter_offset_beacon[0] = hapd->cs_c_off_beacon;
	settings->counter_offset_presp[0] = hapd->cs_c_off_proberesp;
	settings->counter_offset_beacon[1] = hapd->cs_c_off_ecsa_beacon;
	settings->counter_offset_presp[1] = hapd->cs_c_off_ecsa_proberesp;

	return 0;
}


void hostapd_cleanup_cs_params(struct hostapd_data *hapd)
{
	os_memset(&hapd->cs_freq_params, 0, sizeof(hapd->cs_freq_params));
	hapd->cs_count = 0;
	hapd->cs_block_tx = 0;
	hapd->cs_c_off_beacon = 0;
	hapd->cs_c_off_proberesp = 0;
	hapd->csa_in_progress = 0;
	hapd->cs_c_off_ecsa_beacon = 0;
	hapd->cs_c_off_ecsa_proberesp = 0;
}


void hostapd_chan_switch_vht_config(struct hostapd_data *hapd, int vht_enabled)
{
	if (vht_enabled)
		hapd->iconf->ch_switch_vht_config |= CH_SWITCH_VHT_ENABLED;
	else
		hapd->iconf->ch_switch_vht_config |= CH_SWITCH_VHT_DISABLED;

	hostapd_logger(hapd, NULL, HOSTAPD_MODULE_IEEE80211,
		       HOSTAPD_LEVEL_INFO, "CHAN_SWITCH VHT CONFIG 0x%x",
		       hapd->iconf->ch_switch_vht_config);
}


int hostapd_switch_channel(struct hostapd_data *hapd,
			   struct csa_settings *settings)
{
	int ret;

	if (!(hapd->iface->drv_flags & WPA_DRIVER_FLAGS_AP_CSA)) {
		wpa_printf(MSG_INFO, "CSA is not supported");
		return -1;
	}

	ret = hostapd_fill_csa_settings(hapd, settings);
	if (ret)
		return ret;

	ret = hostapd_drv_switch_channel(hapd, settings);
	free_beacon_data(&settings->beacon_csa);
	free_beacon_data(&settings->beacon_after);

	if (ret) {
		/* if we failed, clean cs parameters */
		hostapd_cleanup_cs_params(hapd);
		return ret;
	}

	if (NL80211_SB_DFS_BW_NORMAL == settings->sb_dfs_bw)
		hapd->iface->sb_dfs_cntr = 0;

	hapd->csa_in_progress = 1;
	return 0;
}


void
hostapd_switch_channel_fallback(struct hostapd_iface *iface,
				const struct hostapd_freq_params *freq_params)
{
	int seg0_idx = 0, seg1_idx = 0, bw = CHANWIDTH_USE_HT;

	wpa_printf(MSG_DEBUG, "Restarting all CSA-related BSSes");

	if (freq_params->center_freq1)
		seg0_idx = 36 + (freq_params->center_freq1 - 5180) / 5;
	if (freq_params->center_freq2)
		seg1_idx = 36 + (freq_params->center_freq2 - 5180) / 5;

	switch (freq_params->bandwidth) {
	case 0:
	case 20:
	case 40:
		bw = CHANWIDTH_USE_HT;
		break;
	case 80:
		if (freq_params->center_freq2)
			bw = CHANWIDTH_80P80MHZ;
		else
			bw = CHANWIDTH_80MHZ;
		break;
	case 160:
		bw = CHANWIDTH_160MHZ;
		break;
	default:
		wpa_printf(MSG_WARNING, "Unknown CSA bandwidth: %d",
			   freq_params->bandwidth);
		break;
	}

	iface->freq = freq_params->freq;
	iface->conf->channel = freq_params->channel;
	iface->conf->secondary_channel = freq_params->sec_channel_offset;
	hostapd_set_oper_centr_freq_seg0_idx(iface->conf, seg0_idx);
	hostapd_set_oper_centr_freq_seg1_idx(iface->conf, seg1_idx);
	hostapd_set_oper_chwidth(iface->conf, bw);
	iface->conf->ieee80211n = freq_params->ht_enabled;
	iface->conf->ieee80211ac = freq_params->vht_enabled;
	iface->conf->ieee80211ax = freq_params->he_enabled;

	/*
	 * cs_params must not be cleared earlier because the freq_params
	 * argument may actually point to one of these.
	 * These params will be cleared during interface disable below.
	 */
	hostapd_disable_iface(iface);
	hostapd_enable_iface(iface);
}

#endif /* NEED_AP_MLME */


struct hostapd_data * hostapd_get_iface(struct hapd_interfaces *interfaces,
					const char *ifname)
{
	size_t i, j;

	for (i = 0; i < interfaces->count; i++) {
		struct hostapd_iface *iface = interfaces->iface[i];

		for (j = 0; j < iface->num_bss; j++) {
			struct hostapd_data *hapd = iface->bss[j];

			if (os_strcmp(ifname, hapd->conf->iface) == 0)
				return hapd;
		}
	}

	return NULL;
}


void hostapd_periodic_iface(struct hostapd_iface *iface)
{
	size_t i;

	ap_list_timer(iface);

	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *hapd = iface->bss[i];

		if (!hapd->started)
			continue;

#ifndef CONFIG_NO_RADIUS
		hostapd_acl_expire(hapd);
#endif /* CONFIG_NO_RADIUS */
	}
}

/* Gets secondary channel offset and VHT operational center frequency segment 0 index
 * from 40 MHz, 80 MHz and 160 MHz ACS candidate tables for 5 GHz band */
int hostapd_get_chan_data_from_candidates_5g (int primary_chan, u8 ch_width,
					      int *secondary_chan_offset, u8 *vht_oper_centr_freq_seg0_idx)
{
	int i;

	if (ch_width == CHANWIDTH_USE_HT) { /* 40 MHz */
		for (i = 0; i < sizeof(g_channel_pairs_40_5G)/sizeof(g_channel_pairs_40_5G[0]); i++) {
			if (g_channel_pairs_40_5G[i].primary == primary_chan) {
				*secondary_chan_offset = g_channel_pairs_40_5G[i].secondary > g_channel_pairs_40_5G[i].primary ? 1 : -1;
				*vht_oper_centr_freq_seg0_idx = g_channel_pairs_40_5G[i].primary + 2 * (*secondary_chan_offset);
				return 0;
			}
		}
	}
	else if (ch_width == CHANWIDTH_80MHZ){ /*80 MHz*/
		for (i = 0; i < sizeof(g_channels80)/sizeof(g_channels80[0]); i++) {
			if (g_channels80[i].primary == primary_chan) {
				*secondary_chan_offset = g_channels80[i].secondary > g_channels80[i].primary ? 1 : -1;
				*vht_oper_centr_freq_seg0_idx = g_channels80[i].chan_no;
				return 0;
			}
		}
	}
	else if (ch_width == CHANWIDTH_160MHZ){ /*160 MHz*/
		for (i = 0; i < sizeof(g_channels160)/sizeof(g_channels160[0]); i++) {
			if (g_channels160[i].primary == primary_chan) {
				*secondary_chan_offset = g_channels160[i].secondary > g_channels160[i].primary ? 1 : -1;
				*vht_oper_centr_freq_seg0_idx = g_channels160[i].chan_no;
				return 0;
			}
		}
	}

	wpa_printf(MSG_ERROR, "Channel %d width %d not found in 5 GHz ACS candidates table", primary_chan, ch_width);
	return 1;
}

int get_num_width(int vht_width, int secondary)
{
	switch (vht_width) {
		case CHANWIDTH_USE_HT:   return secondary ? 40 : 20;
		case CHANWIDTH_80MHZ:    return 80;
		case CHANWIDTH_160MHZ:   return 160;
		case CHANWIDTH_80P80MHZ:
		default: wpa_printf(MSG_ERROR, "ACS: wrong or unsupported channel VHT width %d", vht_width);
	}

	return -1; /* should never happen */
}

int hostapd_chan_to_freq(int channel)
{
	int res = 0;

	/* IEEE Std 802.11-2012: 20.3.15 Channel numbering and channelization
	Channel center frequency = starting frequency + 5 * ch
	*/

#define CHANNEL_THRESHOLD 180

	/* channels 1..14 */
	if (channel >= 1 && channel <= 14) {
		res = 2407 + 5 * channel;
		if (channel == 14) /* IEEE Std 802.11-2012: 17.4.6.3 Channel Numbering of operating channels */
			res += 7;
	}
	else if (channel >= 36 && channel <= CHANNEL_THRESHOLD)
		res = 5000 + 5 * channel;
	else if (channel)
		res = 4000 + 5 * channel;

#undef CHANNEL_THRESHOLD
	return res;
}

void hostapd_handle_acs_whm_warning (struct hostapd_iface *iface)
{
	wpa_printf(MSG_ERROR,"WHM-ACS: current ACS state %d, num_candidates %d num_bss %d acs_num_completed_scans %d",
			iface->state, iface->num_candidates, iface->acs_num_bss, iface->acs_num_completed_scans);
	hostapd_drv_set_whm_trigger(iface->bss[0], WHM_HOSTAP_ACS_FAILURE);
}
