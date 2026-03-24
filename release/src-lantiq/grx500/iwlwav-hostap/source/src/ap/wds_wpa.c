#include "includes.h"
#include "common.h"
#include "drivers/driver.h"
#include "hostapd.h"
#include "sta_info.h"
#include "ap_drv_ops.h"
#include "ieee802_11.h"
#include "accounting.h"
#include "beacon.h"
#include "ap_mlme.h"
#include "wpa_auth.h"
#include "wpa_auth_i.h"
#include "wpa_auth_ie.h"
#include "rsn_supp/wpa.h"
#include "utils/eloop.h"
#include "wds_wpa.h"

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

struct ltq_wpa_supplicant {
  enum wpa_states wpa_state;
  struct hostapd_data *hapd;
  struct sta_info *sta;
};

u16 check_assoc_ies(struct hostapd_data *hapd, struct sta_info *sta,
         const u8 *ies, size_t ies_len, int reassoc);


/**
 * wpa_supplicant_state_txt - Get the connection state name as a text string
 * @state: State (wpa_state; WPA_*)
 * Returns: The state name as a printable text string
 */
const char * ltq_wds_wpa_state_txt(enum wpa_states state)
{
  switch (state) {
  case WPA_DISCONNECTED:
    return "DISCONNECTED";
  case WPA_INACTIVE:
    return "INACTIVE";
  case WPA_INTERFACE_DISABLED:
    return "INTERFACE_DISABLED";
  case WPA_SCANNING:
    return "SCANNING";
  case WPA_AUTHENTICATING:
    return "AUTHENTICATING";
  case WPA_ASSOCIATING:
    return "ASSOCIATING";
  case WPA_ASSOCIATED:
    return "ASSOCIATED";
  case WPA_4WAY_HANDSHAKE:
    return "4WAY_HANDSHAKE";
  case WPA_GROUP_HANDSHAKE:
    return "GROUP_HANDSHAKE";
  case WPA_COMPLETED:
    return "COMPLETED";
  default:
    return "UNKNOWN";
  }
}

static void ltq_supplicant_timeout(void *eloop_ctx, void *timeout_ctx)
{
  struct ltq_wpa_supplicant *supplicant = eloop_ctx;
  struct hostapd_data *hapd = supplicant->hapd;
  struct sta_info *sta = supplicant->sta;

  wpa_msg(hapd, MSG_INFO, "Authentication with " MACSTR " timed out.",
    MAC2STR(sta->addr));
  ap_sta_disconnect(hapd, sta, sta->addr, WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT);
#if 0
  /* implementation without STA disconnect */
  hostapd_drv_sta_deauth(hapd, sta->addr, WLAN_REASON_DEAUTH_LEAVING);
  wpa_sm_key_request(sta->wpa, 0, 1);
  hostapd_drv_poll_client(hapd, hapd->own_addr, sta->addr,
    sta->flags & WLAN_STA_WMM);
  eloop_register_timeout(5, 0, ltq_supplicant_timeout, supplicant, NULL);
#endif
}


static void ltq_supplicant_sta_check(void *eloop_ctx, void *timeout_ctx)
{
  struct hostapd_data *hapd = eloop_ctx;
  int i;

  for(i = 0; i < hapd->conf->num_wds_wpa_sta; i++) {
    ltq_wds_wpa_add(hapd, hapd->conf->wds_wpa_sta[i].addr);
  }
  eloop_register_timeout(10, 0, ltq_supplicant_sta_check, hapd, NULL);
}


static enum wpa_states ltq_wds_wpa_get_state(void *ctx)
{
  struct ltq_wpa_supplicant *supplicant = ctx;

  return supplicant->wpa_state;
}

static void ltq_wds_wpa_set_state(void *ctx, enum wpa_states state)
{
  struct ltq_wpa_supplicant *supplicant = ctx;

  wpa_printf(MSG_DEBUG, "SUPP: %s addr=" MACSTR " wpa_state: %s",
    __func__, MAC2STR(supplicant->sta->addr),
    ltq_wds_wpa_state_txt(state));
  supplicant->wpa_state = state;
}

static void ltq_wds_wpa_deauthenticate(void * ctx, u16 reason_code)
{
  struct ltq_wpa_supplicant *supplicant = ctx;
  struct hostapd_data *hapd = supplicant->hapd;
  struct sta_info *sta = supplicant->sta;

  wpa_msg(hapd, MSG_INFO, "Deauthenticate " MACSTR,
    MAC2STR(sta->addr));
  ap_sta_disconnect(hapd, sta, sta->addr, WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT);
}

static void * ltq_wds_wpa_get_network_ctx(void *ctx)
{
  return ctx;
}

static u8 * ltq_wds_wpa_alloc_eapol(void *ctx, u8 type,
      const void *data, u16 data_len,
      size_t *msg_len, void **data_pos)
{
  errno_t err;
  struct ieee802_1x_hdr *hdr;

  *msg_len = sizeof(*hdr) + data_len;
  hdr = os_malloc(*msg_len);
  if (hdr == NULL)
    return NULL;

  hdr->version = 2;
  hdr->type = type;
  hdr->length = host_to_be16(data_len);

  if (data) {
    err = memcpy_s(hdr + 1, data_len, data, data_len);
    if (EOK != err) {
      wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
	  os_free(hdr);
      return NULL;
    }
  }
  else
    os_memset(hdr + 1, 0, data_len);

  if (data_pos)
    *data_pos = hdr + 1;

  return (u8 *) hdr;
}

static int ltq_wds_wpa_get_bssid(void *ctx, u8 *bssid)
{
  errno_t err;
  struct ltq_wpa_supplicant *supplicant = ctx;

  wpa_printf(MSG_DEBUG, "ltq_wds_wpa_get_bssid: " MACSTR,
    MAC2STR(supplicant->sta->addr));
  err = memcpy_s(bssid, ETH_ALEN, supplicant->sta->addr, ETH_ALEN);
  if (EOK != err) {
    wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
  }

  return 0;
}

static int ltq_wds_wpa_eth_send(void *ctx, const u8 *dest, u16 proto,
  const u8 *buf, size_t len)
{
  struct ltq_wpa_supplicant *supplicant = ctx;
  struct hostapd_data *hapd = supplicant->hapd;
  struct sta_info *sta = supplicant->sta;

  return hostapd_drv_hapd_send_eapol(hapd, dest, buf, len,
    0, hostapd_sta_flags_to_drv(sta->flags));
}

static int ltq_wds_wpa_get_beacon_ie(void *ctx)
{
  wpa_printf(MSG_DEBUG, "SUPP: %s NOT IMPLEMENTED!!!", __func__);
  return -1;
}

static int ltq_wds_wpa_set_key(void *ctx, enum wpa_alg alg,
  const u8 *addr, int key_idx, int set_tx,
  const u8 *seq, size_t seq_len,
  const u8 *key, size_t key_len, enum key_flag key_flag)
{
  struct ltq_wpa_supplicant *supplicant = ctx;
  struct hostapd_data *hapd = supplicant->hapd;
  const char *ifname = hapd->conf->iface;

  wpa_printf(MSG_DEBUG, "SUPP: %s(alg=%d addr=" MACSTR " key_idx=%d "
    "set_tx=%d)", __func__, alg, MAC2STR(addr), key_idx, set_tx);
  if (key_idx != 0) {
    wpa_printf(MSG_INFO, "SUPP: %s. Ignoring broadcast key set request",
      __func__);
    return 0;
  }
  return hostapd_drv_set_key(ifname, hapd, alg, addr, 0, key_idx, set_tx, seq,
    seq_len, key, key_len, KEY_FLAG_DEFAULT);
}

static int ltq_wds_wpa_set_protection(void *ctx, const u8 *addr,
  int protection_type, int key_type)
{
  struct ltq_wpa_supplicant *supplicant = ctx;
  struct hostapd_data *hapd = supplicant->hapd;
  struct sta_info *sta = supplicant->sta;

  wpa_printf(MSG_DEBUG, "SUPP: %s(addr=" MACSTR " protection_type=%d "
       "key_type=%d)",
       __func__, MAC2STR(addr), protection_type, key_type);

  return hostapd_set_authorized(hapd, sta, 1);
}

static void ltq_wds_wpa_cancel_auth_timeout(void *ctx)
{
  struct ltq_wpa_supplicant *supplicant = ctx;
  struct sta_info *sta = supplicant->sta;
  UNUSED_VAR(sta);

  wpa_printf(MSG_DEBUG, "SUPP: Cancelling authentication timeout (addr=" MACSTR")",
       MAC2STR(sta->addr));

  eloop_cancel_timeout(ltq_supplicant_timeout, ctx, NULL);

}

static int ltq_wds_wpa_add_pmkid(void *_wpa_s, void *network_ctx,
                                    const u8 *bssid, const u8 *pmkid,
                                    const u8 *fils_cache_id,
                                    const u8 *pmk, size_t pmk_len,
                                    u32 pmk_lifetime, u8 pmk_reauth_threshold,
                                    int akmp)
{
  wpa_printf(MSG_DEBUG, "SUPP: %s NOT IMPLEMENTED!!!", __func__);
  return -1;
}


static int ltq_wds_wpa_remove_pmkid(void *ctx, void *network_ctx, const u8 *bssid,
        const u8 *pmkid, const u8 *fils_cache_id)
{
  wpa_printf(MSG_DEBUG, "SUPP: %s NOT IMPLEMENTED!!!", __func__);
  return -1;
}

static void ltq_wds_wpa_set_config_blob(void *ctx, struct wpa_config_blob *blob)
{
  wpa_printf(MSG_DEBUG, "SUPP: %s NOT IMPLEMENTED!!!", __func__);
}

static const struct wpa_config_blob* ltq_wds_wpa_get_config_blob(void *ctx, const char *name)
{
  wpa_printf(MSG_DEBUG, "SUPP: %s NOT IMPLEMENTED!!!", __func__);
  return NULL;
}

static int ltq_wds_wpa_update_ft_ies(void *ctx, const u8 *md, const u8 *ies,
  size_t ies_len)
{
  wpa_printf(MSG_DEBUG, "SUPP: %s NOT IMPLEMENTED!!!", __func__);
  return -1;
}

static int ltq_wds_wpa_send_ft_action(void *ctx, u8 action, const u8 *target_ap,
  const u8 *ies, size_t ies_len)
{
  wpa_printf(MSG_DEBUG, "SUPP: %s NOT IMPLEMENTED!!!", __func__);
  return -1;
}

static int ltq_wds_wpa_mark_authenticated(void *ctx, const u8 *target_ap)
{
  wpa_printf(MSG_DEBUG, "SUPP: %s NOT IMPLEMENTED!!!", __func__);
  return -1;
}

static void ltq_wds_wpa_set_rekey_offload(void *ctx, const u8 *kek, size_t kek_len,
  const u8 *kck, size_t kck_len,
  const u8 *replay_ctr)
{
  wpa_printf(MSG_DEBUG, "SUPP: %s NOT IMPLEMENTED!!!", __func__);
}

static int ltq_wds_wpa_key_mgmt_set_pmk(void *ctx, const u8 *pmk, size_t pmk_len)
{
  wpa_printf(MSG_DEBUG, "SUPP: %s NOT IMPLEMENTED!!!", __func__);
  return -1;
}

int ltq_wds_wpa_add(struct hostapd_data *hapd, const u8 *src)
{
  errno_t err;
  struct sta_info *sta;
  struct wpa_driver_ap_params params;
  struct ieee80211_ht_capabilities ht_cap;
  struct ieee80211_vht_capabilities vht_cap;
  struct ieee80211_he_capabilities he_cap;
  struct ieee80211_mgmt *head;
  u8 *body;
  size_t head_len;
  struct wpa_sm_ctx *wctx = NULL;
  struct ltq_wpa_supplicant *supplicant = NULL;
  u8 wpa_ie[80];
  size_t wpa_ie_len;
  int wps_state;
  u8 *ext_capab = NULL;

  /* Check that station doesn't exist. We are allocating several
   * structures here. If we would call it with existing station,
   * memory leak would occur. */
  sta = ap_get_sta(hapd, src);
  if (sta)
    return 0;

  wpa_printf(MSG_DEBUG, "ltq_wds_wpa_add: adding WDS WPA station " MACSTR,
    MAC2STR(src));

  sta = ap_sta_add(hapd, src);
  if (!sta) {
    wpa_printf(MSG_ERROR, "Unable to handle new sta");
    return -1;
  }
  sta->flags |= WLAN_STA_ASSOC | WLAN_STA_ASSOC_REQ_OK;

  if (ieee802_11_build_ap_params(hapd, &params) < 0)
    goto free_sta;
  /* length of variable part of beacon head */
  head = (struct ieee80211_mgmt *)params.head;
  head_len = params.head_len - ((size_t)&head->u.beacon.variable[0] - (size_t)head);
  body = os_zalloc(head_len + params.tail_len);
  if (body == NULL) {
    wpa_printf(MSG_ERROR, "Out of memory");
    goto free_ap_params;
  }
  err = memcpy_s(body, head_len+params.tail_len, &head->u.beacon.variable[0], head_len);
  if (EOK != err) {
    wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
    goto free_ap_params; 
  }
  err = memcpy_s(body + head_len, params.tail_len, params.tail, params.tail_len);
  if (EOK != err) {
    wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
    goto free_ap_params;
  }
  wpa_hexdump(MSG_DEBUG, "Beacon head and tail", body,
    head_len + params.tail_len);
  /* disable WPS temporary, otherwise wpa_sm won't be created */
  wps_state = hapd->conf->wps_state;
  hapd->conf->wps_state = 0;
  check_assoc_ies(hapd, sta, body, head_len + params.tail_len, 0);
  hapd->conf->wps_state = wps_state;

  if (hapd->conf->wpa &&
    os_memcmp(hapd->own_addr, src, sizeof(hapd->own_addr)) < 0) {
    /* we are supplicant */
    wctx = os_zalloc(sizeof(*wctx));
    if (wctx == NULL) {
      wpa_printf(MSG_ERROR, "Out of memory");
      goto free_body;
    }
    supplicant = os_zalloc(sizeof(*supplicant));
    if (supplicant == NULL) {
      wpa_printf(MSG_ERROR, "Out of memory");
      os_free(wctx);
      goto free_body;
    }
    supplicant->hapd = hapd;
    supplicant->sta = sta;
    wctx->ctx = supplicant;
    wctx->msg_ctx = hapd;
    wctx->get_state = ltq_wds_wpa_get_state;
    wctx->set_state = ltq_wds_wpa_set_state;
    wctx->deauthenticate = ltq_wds_wpa_deauthenticate;
    wctx->get_network_ctx = ltq_wds_wpa_get_network_ctx;
    wctx->alloc_eapol = ltq_wds_wpa_alloc_eapol;
    wctx->get_bssid = ltq_wds_wpa_get_bssid;
    wctx->ether_send = ltq_wds_wpa_eth_send;
    wctx->get_beacon_ie = ltq_wds_wpa_get_beacon_ie;
    wctx->set_key = ltq_wds_wpa_set_key;
    wctx->mlme_setprotection = ltq_wds_wpa_set_protection;
    wctx->cancel_auth_timeout = ltq_wds_wpa_cancel_auth_timeout;
    wctx->add_pmkid = ltq_wds_wpa_add_pmkid;
    wctx->remove_pmkid = ltq_wds_wpa_remove_pmkid;
    wctx->set_config_blob = ltq_wds_wpa_set_config_blob;
    wctx->get_config_blob = ltq_wds_wpa_get_config_blob;
    wctx->update_ft_ies = ltq_wds_wpa_update_ft_ies;
    wctx->send_ft_action = ltq_wds_wpa_send_ft_action;
    wctx->mark_authenticated = ltq_wds_wpa_mark_authenticated;
    wctx->set_rekey_offload = ltq_wds_wpa_set_rekey_offload;
    wctx->key_mgmt_set_pmk = ltq_wds_wpa_key_mgmt_set_pmk;
    sta->wpa = wpa_sm_init(wctx);
    if (sta->wpa == NULL) {
      wpa_printf(MSG_ERROR, "Out of memory");
      os_free(wctx);
      os_free(supplicant);
      goto free_body;
    }
    /* wpa_sm_set_param(sta->wpa, WPA_PARAM_PROTO, hapd->conf->wpa); */
    wpa_sm_set_param(sta->wpa, WPA_PARAM_PROTO, WPA_PROTO_RSN);
    wpa_sm_set_param(sta->wpa, WPA_PARAM_RSN_ENABLED,
      !!(hapd->conf->wpa & (WPA_PROTO_RSN | WPA_PROTO_OSEN)));
    wpa_sm_set_param(sta->wpa, WPA_PARAM_KEY_MGMT, hapd->conf->wpa_key_mgmt);
    /* wpa_sm_set_param(sta->wpa, WPA_PARAM_PAIRWISE, hapd->conf->wpa_pairwise); */
    wpa_sm_set_param(sta->wpa, WPA_PARAM_PAIRWISE, WPA_CIPHER_CCMP);
   /* wpa_sm_set_param(sta->wpa, WPA_PARAM_GROUP, hapd->conf->wpa_group);*/
    wpa_sm_set_param(sta->wpa, WPA_PARAM_GROUP, WPA_CIPHER_CCMP);
    wpa_sm_set_own_addr(sta->wpa, hapd->own_addr);
    wpa_ie_len = sizeof(wpa_ie);
    os_memset(wpa_ie, 0, wpa_ie_len);
    wpa_sm_set_assoc_wpa_ie_default(sta->wpa, wpa_ie, &wpa_ie_len);
    if (wpa_modify_wpa_ie_rsn(wpa_ie, wpa_ie_len, RSN_SELECTOR(0x00, 0x10, 0x18, 0), 0))
      goto free_body;
    wpa_ie[1] -= 2;
    wpa_ie_len -= 2;
    if (wpa_sm_set_assoc_wpa_ie(sta->wpa, wpa_ie, MIN(wpa_ie_len, sizeof(wpa_ie))))
      goto free_body;
    if (wpa_sm_set_ap_wpa_ie(sta->wpa, wpa_ie, MIN(wpa_ie_len, sizeof(wpa_ie))))
      goto free_body;
    wpa_sm_set_pmk(sta->wpa, hapd->conf->ssid.wpa_psk->psk, PMK_LEN, NULL, NULL);
  }

  if (hostapd_get_aid(hapd, sta) < 0) {
    hostapd_logger(hapd, src, HOSTAPD_MODULE_IEEE80211,
      HOSTAPD_LEVEL_INFO, "No room for more AIDs");
    goto free_body;
  }
  hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
    HOSTAPD_LEVEL_DEBUG,
    "association OK (aid %d)", sta->aid);
  /* Stop previous accounting session, if one is started, and allocate
   * new session id for the new session. */
  accounting_sta_stop(hapd, sta);
  hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
    HOSTAPD_LEVEL_INFO,
    "associated (aid %d)",
    sta->aid);

  sta->capability = le_to_host16(head->u.beacon.capab_info);

  mlme_associate_indication(hapd, sta);
  sta->sa_query_timed_out = 0;

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
			err = memcpy_s(ext_capab, 1 + sta->ext_capability[0], sta->ext_capability, 1 + sta->ext_capability[0]);
			if (EOK != err) {
				wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
				goto free_ext_capab;
			}
		}
	}

  sta->ssi_signal = -5000;
  if (sta->wpa_sm == NULL)
    sta->flags |= WLAN_STA_AUTHORIZED;
  else if (sta->wpa != NULL) {
    /* Free authenticator SM, if we are supplicant. */
    wpa_auth_sta_deinit(sta->wpa_sm);
    sta->wpa_sm = NULL;
    wpa_printf(MSG_DEBUG, "ltq_wds_wpa_add: working with WDS WPA station "
      MACSTR " as supplicant",
      MAC2STR(src));
  } else {
    wpa_printf(MSG_DEBUG, "ltq_wds_wpa_add: working with WDS WPA station "
      MACSTR " as authenticator",
      MAC2STR(src));
  }

  if (hostapd_sta_add(hapd, sta->addr, sta->aid, sta->capability,
          sta->supported_rates, sta->supported_rates_len,
          sta->listen_interval,
          sta->flags & WLAN_STA_HT ? &ht_cap : NULL,
          sta->flags & WLAN_STA_VHT ? &vht_cap : NULL,
          sta->flags & WLAN_STA_HE ? &he_cap : NULL,
          sta->flags, sta->qosinfo, sta->vht_opmode,
          sta->he_operation,
          sta->p2p_ie ? 1 : 0, 0, sta->last_assoc_req,
          sta->last_assoc_req_len, sta->ssi_signal, ext_capab, sta->he_6ghz_capab)) {
    hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
             HOSTAPD_LEVEL_NOTICE,
             "Could not add STA to kernel driver");
    goto free_ext_capab;
  }
  os_free(ext_capab);
  if (sta->wpa_sm &&
    wpa_modify_wpa_ie(sta->wpa_sm, RSN_SELECTOR(0x00, 0x10, 0x18, 0), 0, -2)) {
    hostapd_logger(hapd, sta->addr, HOSTAPD_MODULE_IEEE80211,
             HOSTAPD_LEVEL_NOTICE,
             "Could not modify WPA RSN IE");
    goto sta_remove;
  }
  os_free(body);
  ieee802_11_free_ap_params(&params);

  wpa_auth_sm_event(sta->wpa_sm, WPA_ASSOC);
  if (sta->wpa && supplicant) {
    supplicant->wpa_state = WPA_ASSOCIATED;
    wpa_printf(MSG_DEBUG, "Setting authentication timeout: 5 sec");
    eloop_cancel_timeout(ltq_supplicant_timeout, supplicant, NULL);
    eloop_register_timeout(20, 0, ltq_supplicant_timeout, supplicant, NULL);
  }
  wpa_sm_notify_assoc(sta->wpa, src);
  if (sta->wpa_sm)
    hapd->new_assoc_sta_cb(hapd, sta, 1);
  if (sta->wpa)
    wpa_sm_key_request(sta->wpa, 0, 1);
  hostapd_drv_poll_client(hapd, hapd->own_addr, src, sta->flags & WLAN_STA_WMM);

  return 0;

sta_remove:
  hostapd_drv_sta_remove(hapd, src);
free_ext_capab:
	os_free(ext_capab);
free_body:
  os_free(body);
free_ap_params:
  ieee802_11_free_ap_params(&params);
free_sta:
  ap_free_sta(hapd, sta);
  return -1;
}

int ltq_wds_wpa_init(struct hostapd_data *hapd)
{
  int i, ret;

  wpa_printf(MSG_DEBUG, "wds_wpa_init");

  for(i = 0; i < hapd->conf->num_wds_wpa_sta; i++) {
    ret = hostapd_drv_set_wds_wpa_sta(hapd, hapd->conf->wds_wpa_sta[i].addr, 0);
    if (ret)
      return ret;
  }
  return eloop_register_timeout(10, 0, ltq_supplicant_sta_check, hapd, NULL);
}

void ltq_wds_wpa_deinit(struct hostapd_data *hapd)
{
  int i;

  wpa_printf(MSG_DEBUG, "wds_wpa_deinit");

  eloop_cancel_timeout(ltq_supplicant_sta_check, hapd, NULL);
  for(i = 0; i < hapd->conf->num_wds_wpa_sta; i++)
    hostapd_drv_set_wds_wpa_sta(hapd, hapd->conf->wds_wpa_sta[i].addr, 1);
}
