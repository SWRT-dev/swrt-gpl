/*
 * hostapd - Driver operations
 * Copyright (c) 2009-2014, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef AP_DRV_OPS
#define AP_DRV_OPS

enum wpa_driver_if_type;
struct wpa_bss_params;
struct wpa_driver_scan_params;
struct ieee80211_ht_capabilities;
struct ieee80211_vht_capabilities;
struct hostapd_freq_params;
struct ieee80211_mgmt;

u32 hostapd_sta_flags_to_drv(u32 flags);
int hostapd_build_ap_extra_ies(struct hostapd_data *hapd,
			       struct wpabuf **beacon,
			       struct wpabuf **proberesp,
			       struct wpabuf **assocresp);
void hostapd_free_ap_extra_ies(struct hostapd_data *hapd, struct wpabuf *beacon,
			       struct wpabuf *proberesp,
			       struct wpabuf *assocresp);
int hostapd_reset_ap_wps_ie(struct hostapd_data *hapd);
int hostapd_set_ap_wps_ie(struct hostapd_data *hapd);
int hostapd_set_authorized(struct hostapd_data *hapd,
			   struct sta_info *sta, int authorized);
int hostapd_set_sta_flags(struct hostapd_data *hapd, struct sta_info *sta);
int hostapd_set_drv_ieee8021x(struct hostapd_data *hapd, const char *ifname,
			      int enabled);
int hostapd_vlan_if_add(struct hostapd_data *hapd, const char *ifname);
int hostapd_vlan_if_remove(struct hostapd_data *hapd, const char *ifname);
int hostapd_set_wds_sta(struct hostapd_data *hapd, char *ifname_wds,
			const u8 *addr, int aid, int val);
int hostapd_sta_add(struct hostapd_data *hapd,
		    const u8 *addr, u16 aid, u16 capability,
		    const u8 *supp_rates, size_t supp_rates_len,
		    u16 listen_interval,
		    const struct ieee80211_ht_capabilities *ht_capab,
		    const struct ieee80211_vht_capabilities *vht_capab,
		    const struct ieee80211_he_capabilities *he_capab,
		    u32 flags, u8 qosinfo, u8 vht_opmode,
            const struct ieee80211_he_operation *he_operation,
		    int supp_p2p_ps,
		    int set, struct ieee80211_mgmt *last_assoc_req,
		    size_t last_assoc_req_len, int ssi_signal,
		    const u8 *ext_capab,
		    const struct ieee80211_he_6ghz_band_cap *he_6ghz_capab);
int hostapd_set_privacy(struct hostapd_data *hapd, int enabled);
int hostapd_set_generic_elem(struct hostapd_data *hapd, const u8 *elem,
			     size_t elem_len);
int hostapd_get_ssid(struct hostapd_data *hapd, u8 *buf, size_t len);
int hostapd_set_ssid(struct hostapd_data *hapd, const u8 *buf, size_t len);
int hostapd_if_add(struct hostapd_data *hapd, enum wpa_driver_if_type type,
		   const char *ifname, const u8 *addr, void *bss_ctx,
		   void **drv_priv, char *force_ifname, u8 *if_addr,
		   const char *bridge, int use_existing);
int hostapd_if_remove(struct hostapd_data *hapd, enum wpa_driver_if_type type,
		      const char *ifname);
int hostapd_set_ieee8021x(struct hostapd_data *hapd,
			  struct wpa_bss_params *params);
int hostapd_get_seqnum(const char *ifname, struct hostapd_data *hapd,
		       const u8 *addr, int idx, u8 *seq);
int hostapd_flush(struct hostapd_data *hapd);
int hostapd_set_freq(struct hostapd_data *hapd, enum hostapd_hw_mode mode,
		     int freq, int channel, int edmg, u8 edmg_channel,
		     int ht_enabled, int vht_enabled,
		     int he_enabled, int sec_channel_offset, int oper_chwidth,
		     int center_segment0, int center_segment1);
int hostapd_set_rts(struct hostapd_data *hapd, int rts);
int hostapd_set_frag(struct hostapd_data *hapd, int frag);
int hostapd_sta_set_flags(struct hostapd_data *hapd, u8 *addr,
			  int total_flags, int flags_or, int flags_and);
int hostapd_sta_set_airtime_weight(struct hostapd_data *hapd, const u8 *addr,
				   unsigned int weight);
int hostapd_set_country(struct hostapd_data *hapd, const char *country);
int hostapd_set_tx_queue_params(struct hostapd_data *hapd, int queue, int aifs,
				int cw_min, int cw_max, int burst_time);
struct hostapd_hw_modes *
hostapd_get_hw_feature_data(struct hostapd_data *hapd, u16 *num_modes,
			    u16 *flags, u8 *dfs_domain);
int hostapd_driver_commit(struct hostapd_data *hapd);
int hostapd_drv_none(struct hostapd_data *hapd);
int hostapd_driver_scan(struct hostapd_data *hapd,
			struct wpa_driver_scan_params *params);
struct wpa_scan_results * hostapd_driver_get_scan_results(
	struct hostapd_data *hapd);
int hostapd_driver_set_noa(struct hostapd_data *hapd, u8 count, int start,
			   int duration);
int hostapd_drv_set_key(const char *ifname,
			struct hostapd_data *hapd,
			enum wpa_alg alg, const u8 *addr,
			int key_idx, int vlan_id, int set_tx,
			const u8 *seq, size_t seq_len,
			const u8 *key, size_t key_len, enum key_flag key_flag);
int hostapd_drv_send_mlme(struct hostapd_data *hapd,
			  const void *msg, size_t len, int noack,
			  const u16 *csa_offs, size_t csa_offs_len,
			  int no_encrypt);
int hostapd_drv_sta_deauth(struct hostapd_data *hapd,
			   const u8 *addr, int reason);
int hostapd_drv_sta_disassoc(struct hostapd_data *hapd,
			     const u8 *addr, int reason);
int hostapd_drv_send_action(struct hostapd_data *hapd, unsigned int freq,
			    unsigned int wait, const u8 *dst, const u8 *data,
			    size_t len);
int hostapd_drv_send_action_addr3_ap(struct hostapd_data *hapd,
				     unsigned int freq,
				     unsigned int wait, const u8 *dst,
				     const u8 *data, size_t len);
static inline void
hostapd_drv_send_action_cancel_wait(struct hostapd_data *hapd)
{
	if (!hapd->driver || !hapd->driver->send_action_cancel_wait ||
	    !hapd->drv_priv)
		return;
	hapd->driver->send_action_cancel_wait(hapd->drv_priv);
}
int hostapd_add_sta_node(struct hostapd_data *hapd, const u8 *addr,
			 u16 auth_alg);
int hostapd_sta_auth(struct hostapd_data *hapd, const u8 *addr,
		     u16 seq, u16 status, const u8 *ie, size_t len);
int hostapd_sta_assoc(struct hostapd_data *hapd, const u8 *addr,
		      int reassoc, u16 status, const u8 *ie, size_t len);
int hostapd_add_tspec(struct hostapd_data *hapd, const u8 *addr,
		      u8 *tspec_ie, size_t tspec_ielen);
int hostapd_start_dfs_cac(struct hostapd_iface *iface,
			  enum hostapd_hw_mode mode, int freq,
			  int channel, int ht_enabled, int vht_enabled, int he_enabled,
			  int sec_channel_offset, int oper_chwidth,
			  int center_segment0, int center_segment1);
int hostapd_set_chan_dfs_state(struct hostapd_iface *iface,
			  enum nl80211_dfs_state dfs_state, int freq,
			  int dfs_time, int dfs_debug);
int hostapd_drv_do_acs(struct hostapd_data *hapd);
int hostapd_drv_update_dh_ie(struct hostapd_data *hapd, const u8 *peer,
			     u16 reason_code, const u8 *ie, size_t ielen);
int hostapd_drv_dpp_listen(struct hostapd_data *hapd, bool enable);


#include "drivers/driver.h"

int hostapd_drv_wnm_oper(struct hostapd_data *hapd,
			 enum wnm_oper oper, const u8 *peer,
			 u8 *buf, u16 *buf_len);

int hostapd_drv_set_qos_map(struct hostapd_data *hapd, const u8 *qos_map_set,
			    u8 qos_map_set_len);

void hostapd_get_ext_capa(struct hostapd_iface *iface);

static inline int hostapd_drv_set_countermeasures(struct hostapd_data *hapd,
						  int enabled)
{
	if (hapd->driver == NULL ||
	    hapd->driver->hapd_set_countermeasures == NULL)
		return 0;
	return hapd->driver->hapd_set_countermeasures(hapd->drv_priv, enabled);
}

static inline int hostapd_drv_set_sta_vlan(const char *ifname,
					   struct hostapd_data *hapd,
					   const u8 *addr, int vlan_id)
{
	if (hapd->driver == NULL || hapd->driver->set_sta_vlan == NULL)
		return 0;
	return hapd->driver->set_sta_vlan(hapd->drv_priv, addr, ifname,
					  vlan_id);
}

static inline int hostapd_drv_get_inact_sec(struct hostapd_data *hapd,
					    const u8 *addr)
{
	if (hapd->driver == NULL || hapd->driver->get_inact_sec == NULL)
		return 0;
	return hapd->driver->get_inact_sec(hapd->drv_priv, addr);
}

static inline int hostapd_drv_send_atf_quotas(struct hostapd_data *hapd,
		const u8* changed_sta, /* if NULL, ATF config changed */
		int in_driver, int not_in_driver)
{
	int res = 0;

	/* Do we have ATF function enabled? */
	if (hapd->iconf->atf_config_file == NULL || hapd->iconf->atf_config_file[0] == '\0') {
		hapd->iface->atf_enabled = 0;
		return -ENOTSUP;
	}

	/* Re-read ATF configuration file */
	if (changed_sta == NULL &&
		hostapd_atf_read_config (&hapd->iconf->atf_cfg, hapd->iconf->atf_config_file))
		return -EINVAL;

	/* Do it even if ATF is OFF. It traces the "in driver" flags for stations. */
	struct intel_vendor_atf_quotas* atf_quotas = hostapd_atf_calc_quotas(&hapd->iconf->atf_cfg,
									hapd, changed_sta, in_driver, not_in_driver);

	if (atf_quotas != NULL) { /* Is NULL if ATF quotas are already up to date */
		res = hapd->driver->send_atf_quotas(hapd->drv_priv, atf_quotas);
		os_free (atf_quotas); /* allocated by os_zalloc in hostapd_atf_calc_quotas */
	}

	return res;
}

static inline int hostapd_drv_sta_remove(struct hostapd_data *hapd,
					 const u8 *addr)
{
	int ret_code;

	if (!hapd->driver || !hapd->driver->sta_remove || !hapd->drv_priv)
		return 0;

	ret_code = hapd->driver->sta_remove(hapd->drv_priv, addr);
	if (ret_code == 0)
		hostapd_drv_send_atf_quotas(hapd, addr, 0, 1);

	return ret_code;
}

static inline int hostapd_drv_hapd_send_eapol(struct hostapd_data *hapd,
					      const u8 *addr, const u8 *data,
					      size_t data_len, int encrypt,
					      u32 flags)
{
	if (hapd->driver == NULL || hapd->driver->hapd_send_eapol == NULL)
		return 0;
	return hapd->driver->hapd_send_eapol(hapd->drv_priv, addr, data,
					     data_len, encrypt,
					     hapd->own_addr, flags);
}

static inline int hostapd_drv_read_sta_data(
	struct hostapd_data *hapd, struct hostap_sta_driver_data *data,
	const u8 *addr)
{
	if (hapd->driver == NULL || hapd->driver->read_sta_data == NULL)
		return -1;
	return hapd->driver->read_sta_data(hapd->drv_priv, data, addr);
}

static inline int hostapd_drv_sta_clear_stats(struct hostapd_data *hapd,
					      const u8 *addr)
{
	if (hapd->driver == NULL || hapd->driver->sta_clear_stats == NULL)
		return 0;
	return hapd->driver->sta_clear_stats(hapd->drv_priv, addr);
}

static inline int hostapd_drv_set_acl(struct hostapd_data *hapd,
				      struct hostapd_acl_params *params)
{
	if (hapd->driver == NULL || hapd->driver->set_acl == NULL)
		return 0;
	return hapd->driver->set_acl(hapd->drv_priv, params);
}

static inline int hostapd_drv_set_ap(struct hostapd_data *hapd,
				     struct wpa_driver_ap_params *params)
{
	if (hapd->driver == NULL || hapd->driver->set_ap == NULL)
		return 0;
	return hapd->driver->set_ap(hapd->drv_priv, params);
}

static inline int hostapd_drv_set_radius_acl_auth(struct hostapd_data *hapd,
						  const u8 *mac, int accepted,
						  u32 session_timeout)
{
	if (hapd->driver == NULL || hapd->driver->set_radius_acl_auth == NULL)
		return 0;
	return hapd->driver->set_radius_acl_auth(hapd->drv_priv, mac, accepted,
						 session_timeout);
}

static inline int hostapd_drv_set_radius_acl_expire(struct hostapd_data *hapd,
						    const u8 *mac)
{
	if (hapd->driver == NULL ||
	    hapd->driver->set_radius_acl_expire == NULL)
		return 0;
	return hapd->driver->set_radius_acl_expire(hapd->drv_priv, mac);
}

static inline int hostapd_drv_set_authmode(struct hostapd_data *hapd,
					   int auth_algs)
{
	if (hapd->driver == NULL || hapd->driver->set_authmode == NULL)
		return 0;
	return hapd->driver->set_authmode(hapd->drv_priv, auth_algs);
}

static inline void hostapd_drv_poll_client(struct hostapd_data *hapd,
					   const u8 *own_addr, const u8 *addr,
					   int qos)
{
	if (hapd->driver == NULL || hapd->driver->poll_client == NULL)
		return;
	hapd->driver->poll_client(hapd->drv_priv, own_addr, addr, qos);
}

static inline int hostapd_drv_get_survey(struct hostapd_data *hapd,
					 unsigned int freq)
{
	if (hapd->driver == NULL)
		return -1;
	if (!hapd->driver->get_survey)
		return -1;
	return hapd->driver->get_survey(hapd->drv_priv, freq);
}

static inline int hostapd_get_country(struct hostapd_data *hapd, char *alpha2)
{
	if (hapd->driver == NULL || hapd->driver->get_country == NULL)
		return -1;
	return hapd->driver->get_country(hapd->drv_priv, alpha2);
}

static inline const char * hostapd_drv_get_radio_name(struct hostapd_data *hapd)
{
	if (hapd->driver == NULL || hapd->drv_priv == NULL ||
	    hapd->driver->get_radio_name == NULL)
		return NULL;
	return hapd->driver->get_radio_name(hapd->drv_priv);
}

static inline int hostapd_drv_switch_channel(struct hostapd_data *hapd,
					     struct csa_settings *settings)
{
	if (hapd->driver == NULL || hapd->driver->switch_channel == NULL ||
	    hapd->drv_priv == NULL)
		return -1;

	return hapd->driver->switch_channel(hapd->drv_priv, settings);
}

static inline int hostapd_drv_status(struct hostapd_data *hapd, char *buf,
				     size_t buflen)
{
	if (!hapd->driver || !hapd->driver->status || !hapd->drv_priv)
		return -1;
	return hapd->driver->status(hapd->drv_priv, buf, buflen);
}

static inline int hostapd_drv_br_add_ip_neigh(struct hostapd_data *hapd,
					      int version, const u8 *ipaddr,
					      int prefixlen, const u8 *addr)
{
	if (hapd->driver == NULL || hapd->drv_priv == NULL ||
	    hapd->driver->br_add_ip_neigh == NULL)
		return -1;
	return hapd->driver->br_add_ip_neigh(hapd->drv_priv, version, ipaddr,
					     prefixlen, addr);
}

static inline int hostapd_drv_br_delete_ip_neigh(struct hostapd_data *hapd,
						 u8 version, const u8 *ipaddr)
{
	if (hapd->driver == NULL || hapd->drv_priv == NULL ||
	    hapd->driver->br_delete_ip_neigh == NULL)
		return -1;
	return hapd->driver->br_delete_ip_neigh(hapd->drv_priv, version,
						ipaddr);
}

static inline int hostapd_drv_br_port_set_attr(struct hostapd_data *hapd,
					       enum drv_br_port_attr attr,
					       unsigned int val)
{
	if (hapd->driver == NULL || hapd->drv_priv == NULL ||
	    hapd->driver->br_port_set_attr == NULL)
		return -1;
	return hapd->driver->br_port_set_attr(hapd->drv_priv, attr, val);
}

static inline int hostapd_drv_br_set_net_param(struct hostapd_data *hapd,
					       enum drv_br_net_param param,
					       unsigned int val)
{
	if (hapd->driver == NULL || hapd->drv_priv == NULL ||
	    hapd->driver->br_set_net_param == NULL)
		return -1;
	return hapd->driver->br_set_net_param(hapd->drv_priv, param, val);
}

static inline int hostapd_drv_vendor_cmd(struct hostapd_data *hapd,
					 int vendor_id, int subcmd,
					 const u8 *data, size_t data_len,
					 struct wpabuf *buf)
{
	if (hapd->driver == NULL || hapd->driver->vendor_cmd == NULL || hapd->drv_priv == NULL)
		return -1;
	return hapd->driver->vendor_cmd(hapd->drv_priv, vendor_id, subcmd, data,
					data_len, buf);
}

static inline int hostapd_drv_stop_ap(struct hostapd_data *hapd)
{
	if (!hapd->driver || !hapd->driver->stop_ap || !hapd->drv_priv)
		return 0;
	return hapd->driver->stop_ap(hapd->drv_priv);
}

static inline int hostapd_drv_set_deny_mac(struct hostapd_data *hapd,
	struct multi_ap_blacklist* entry, const u8 remove)
{
  if (hapd->driver == NULL || hapd->driver->set_deny_mac_addr == NULL)
    return -ENOTSUP;

  return hapd->driver->set_deny_mac_addr(hapd->drv_priv, entry, remove);
}

static inline int hostapd_drv_set_sta_softblock_thresholds(struct hostapd_data *hapd,
		struct multi_ap_blacklist* entry, const u8 remove)
{
	if (hapd->driver == NULL || hapd->driver->set_sta_softblock_thresholds == NULL)
		return -ENOTSUP;
	
	return hapd->driver->set_sta_softblock_thresholds(hapd->drv_priv, entry, remove);
}

static inline int hostapd_drv_sta_steer(struct hostapd_data *hapd,
	struct multi_ap_blacklist* entry, const u8 *bssid)
{
  if (hapd->driver == NULL || hapd->driver->sta_steer == NULL)
    return -ENOTSUP;

  return hapd->driver->sta_steer(hapd->drv_priv, entry, bssid);
}

static inline int hostapd_drv_sta_allow(struct hostapd_data *hapd,
  const u8 *stations, int count)
{
  if (hapd->driver == NULL || hapd->driver->sta_allow == NULL)
    return -ENOTSUP;

  return hapd->driver->sta_allow(hapd->drv_priv, stations, count);
}

static inline int hostapd_drv_set_bss_load(struct hostapd_data *hapd,
  const u8 is_enable)
{
  if (hapd->driver == NULL || hapd->driver->set_bss_load == NULL)
    return -ENOTSUP;

  return hapd->driver->set_bss_load(hapd->drv_priv, is_enable);
}

static inline int hostapd_drv_set_mbssid_vap_mode(struct hostapd_data *hapd,
  const u8 mbssid_vap)
{
  if (hapd->driver == NULL || hapd->driver->set_mbssid_vap_mode == NULL)
    return -ENOTSUP;

  return hapd->driver->set_mbssid_vap_mode(hapd->drv_priv, mbssid_vap);
}

static inline int hostapd_drv_set_mbssid_num_vaps_in_group(struct hostapd_data *hapd,
  const u8 mbssid_num_vaps_in_group)
{
  if (hapd->driver == NULL || hapd->driver->set_mbssid_num_vaps_in_group == NULL)
    return -ENOTSUP;

  return hapd->driver->set_mbssid_num_vaps_in_group(hapd->drv_priv, mbssid_num_vaps_in_group);
}

static inline int hostapd_drv_set_management_frames_rate(struct hostapd_data *hapd,
		const u8 management_frames_rate)
{
  if (hapd->driver == NULL || hapd->driver->set_management_frames_rate == NULL)
    return -ENOTSUP;

  return hapd->driver->set_management_frames_rate(hapd->drv_priv, management_frames_rate);
}

static inline int hostapd_drv_get_sta_measurements(struct hostapd_data *hapd,
  const u8 *addr, struct intel_vendor_sta_info *sta_info)
{
  if (!hapd->driver || !hapd->driver->get_sta_measurements || !hapd->drv_priv)
    return -ENOTSUP;

  return hapd->driver->get_sta_measurements(hapd->drv_priv, addr, sta_info);
}

static inline int hostapd_drv_get_vap_measurements(struct hostapd_data *hapd,
		struct intel_vendor_vap_info *vap_info)
{
  if (!hapd->driver || !hapd->driver->get_vap_measurements || !hapd->drv_priv)
    return -ENOTSUP;

  return hapd->driver->get_vap_measurements(hapd->drv_priv, vap_info);
}

static inline int hostapd_drv_get_radio_info(struct hostapd_data *hapd,
  struct intel_vendor_radio_info *radio_info)
{
  if (!hapd->driver || !hapd->driver->get_radio_info || !hapd->drv_priv)
    return -ENOTSUP;

  return hapd->driver->get_radio_info(hapd->drv_priv, radio_info);
}

static inline int hostapd_drv_get_he_operation(struct hostapd_data *hapd,
  struct intel_vendor_he_oper *he_operation)
{
  if (hapd->driver == NULL || hapd->driver->get_he_operation_info == NULL)
    return -ENOTSUP;

  return hapd->driver->get_he_operation_info(hapd->drv_priv, he_operation);
}

static inline int hostapd_drv_set_he_operation(struct hostapd_data *hapd,
  struct intel_vendor_he_oper *he_operation)
{
  if (hapd->driver == NULL || hapd->driver->set_he_operation_info == NULL)
    return -ENOTSUP;

  return hapd->driver->set_he_operation_info(hapd->drv_priv, he_operation);
}

static inline int hostapd_drv_get_he_non_advertised(struct hostapd_data *hapd,
                struct intel_vendor_he_capa *non_advertised_he_caps)
{
  if (hapd->driver == NULL || hapd->driver->get_he_non_advertised_info == NULL)
    return -ENOTSUP;

  return hapd->driver->get_he_non_advertised_info(hapd->drv_priv, non_advertised_he_caps);
}

static inline int
hostapd_drv_set_he_non_advertised(struct hostapd_data *hapd, struct intel_vendor_he_capa *he_capab)
{
	if (!hapd->driver || !hapd->driver->set_he_non_advertised_info)
		return -ENOTSUP;

	return hapd->driver->set_he_non_advertised_info(hapd->drv_priv, he_capab);
}

static inline int
hostapd_drv_send_ltq_he_debug_mode_data(struct hostapd_data *hapd,
					struct intel_vendor_he_capa *data)
{
	if (!hapd->driver || !hapd->driver->send_ltq_he_debug_mode_data)
		return -ENOTSUP;

	return hapd->driver->send_ltq_he_debug_mode_data(hapd->drv_priv, data);
}

static inline int hostapd_drv_unconnected_sta(struct hostapd_data *hapd,
  struct intel_vendor_unconnected_sta_req_cfg *req)
{
  if (hapd->driver == NULL || hapd->driver->unconnected_sta == NULL)
    return -ENOTSUP;

  return hapd->driver->unconnected_sta(hapd->drv_priv, req);
}

static inline int hostapd_drv_set_zwdfs_antenna(struct hostapd_data *hapd,
						int enable)
{
  if (hapd->driver == NULL || hapd->driver->set_zwdfs_antenna == NULL)
    return -ENOTSUP;

  return hapd->driver->set_zwdfs_antenna(hapd->drv_priv, enable);
}

static inline int hostapd_drv_get_zwdfs_antenna(struct hostapd_data *hapd,
						int *enable)
{
  if (hapd->driver == NULL || hapd->driver->get_zwdfs_antenna == NULL)
    return -ENOTSUP;

  return hapd->driver->get_zwdfs_antenna(hapd->drv_priv, enable);
}

static inline int hostapd_drv_set_unsolicited_frame(struct hostapd_data *hapd,
						int mode, int duration)
{
  if (hapd->driver == NULL || hapd->driver->set_unsolicited_frame == NULL)
    return -ENOTSUP;

  return hapd->driver->set_unsolicited_frame(hapd->drv_priv, mode, duration);
}

static inline int hostapd_drv_set_antenna(struct hostapd_data *hapd,
  u32 tx_ant, u32 rx_ant)
{
  if (hapd->driver == NULL || hapd->driver->set_antenna == NULL)
    return -ENOTSUP;

  return hapd->driver->set_antenna(hapd->drv_priv, tx_ant, rx_ant);
}

static inline int hostapd_drv_get_capa(struct hostapd_data *hapd,
  struct wpa_driver_capa *capa)
{
  if (hapd->driver == NULL || hapd->driver->get_capa == NULL)
    return -ENOTSUP;

  return hapd->driver->get_capa(hapd->drv_priv, capa);
}

#ifdef CONFIG_WDS_WPA
static inline int hostapd_drv_set_wds_wpa_sta(struct hostapd_data *hapd,
  const u8 *addr, const u8 remove)
{
  if (hapd->driver == NULL || hapd->driver->set_wds_wpa_sta == NULL)
    return -ENOTSUP;

  return hapd->driver->set_wds_wpa_sta(hapd->drv_priv, addr, remove);
}
#endif

static inline int hostapd_drv_set_mesh_mode(struct hostapd_data *hapd,
		int mesh_mode)
{
	if (hapd->driver == NULL || hapd->driver->set_mesh_mode == NULL)
		return -ENOTSUP;

	return hapd->driver->set_mesh_mode(hapd->drv_priv, mesh_mode);
}

static inline int hostapd_drv_add_backhaul_vrt_iface(struct hostapd_data *hapd,
						     const char *ifname)
{
	if (hapd->driver == NULL || hapd->driver->add_backhaul_vrt_iface == NULL)
		return -ENOTSUP;

	return hapd->driver->add_backhaul_vrt_iface(hapd->drv_priv, ifname);
}

static inline int hostapd_drv_channel_info(struct hostapd_data *hapd,
					   struct wpa_channel_info *ci)
{
	if (!hapd->driver || !hapd->driver->channel_info)
		return -1;
	return hapd->driver->channel_info(hapd->drv_priv, ci);
}

static inline int
hostapd_drv_send_external_auth_status(struct hostapd_data *hapd,
				      struct external_auth *params)
{
	if (!hapd->driver || !hapd->drv_priv ||
	    !hapd->driver->send_external_auth_status)
		return -1;
	return hapd->driver->send_external_auth_status(hapd->drv_priv, params);
}

static inline int hostapd_drv_set_he_beacon(struct hostapd_data *hapd,
                                                int enable)
{
  if (hapd->driver == NULL || hapd->driver->set_he_beacon == NULL)
    return -ENOTSUP;

  return hapd->driver->set_he_beacon(hapd->drv_priv, enable);
}


#ifdef CONFIG_WIFI_CERTIFICATION
static inline int hostapd_drv_set_unprotected_deauth(struct hostapd_data *hapd,
                                                int enable)
{
  if (hapd->driver == NULL || hapd->driver->set_unprotected_deauth == NULL)
    return -ENOTSUP;

  return hapd->driver->set_unprotected_deauth(hapd->drv_priv, enable);
}

static inline int hostapd_drv_set_pn_reset(struct hostapd_data *hapd,
                                                int enable)
{
  if (hapd->driver == NULL || hapd->driver->set_pn_reset == NULL)
    return -ENOTSUP;

  return hapd->driver->set_pn_reset(hapd->drv_priv, enable);
}

#endif

static inline int hostapd_drv_set_dynamic_multicast_mode_rate(struct hostapd_data *hapd,
  int dynamic_mc_mode, int dynamic_mc_rate)
{
  if (hapd->driver == NULL || hapd->driver->set_dynamic_mc_mode_rate == NULL)
    return -ENOTSUP;

  return hapd->driver->set_dynamic_mc_mode_rate(hapd->drv_priv, dynamic_mc_mode, dynamic_mc_rate);
}

static inline int hostapd_drv_set_whm_config(struct hostapd_data *hapd, int whm_cfg_flag)
{
  if (hapd->driver == NULL || hapd->driver->set_whm_config == NULL)
    return -ENOTSUP;

  return hapd->driver->set_whm_config(hapd->drv_priv, whm_cfg_flag);
}

static inline int hostapd_drv_set_whm_trigger(struct hostapd_data *hapd, int warning_id)
{
  if (hapd->driver == NULL || hapd->driver->set_whm_config == NULL)
    return -ENOTSUP;

  if (!((hapd->iconf->whm_cfg_flag & BIT(WHM_ENABLE)) && (hapd->iconf->whm_cfg_flag & BIT(WHM_HOSTAP_WARN)))) {
    wpa_printf(MSG_ERROR, "WHM is disabled - can't handle warning (%d) from Hostapd", warning_id);
    return -ENOTSUP;
  }

  return hapd->driver->set_whm_trigger(hapd->drv_priv, warning_id);
}
#endif /* AP_DRV_OPS */
