/*
 * hostapd / IEEE 802.11 Management
 * Copyright (c) 2002-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef IEEE802_11_H
#define IEEE802_11_H

#define BW_20  20
#define BW_40  40
#define BW_80  80
#define BW_160 160
#define BW_MOVE_DOWN 1
#define BW_MOVE_UP 2
#define COC_POWER_1x1_STATE 1
#define COC_POWER_2x2_STATE 2
#define COC_POWER_3x3_STATE 3
#define COC_POWER_4x4_STATE 4

/* Space required after MBSSID IE (conservative reservation) */
/* buffer size of 360 bytes is reserved for all the IEs following the MBSSID IE.
 * buffer availability check is carried out before updating the MBSSID IE in order to ensure that the entire MBSSID IE
 * (including individual security profiles for Non Tx VAPs, if necessary) can be constructed despite the reservation of 360 bytes
 * in the buffer for the remainder of IEs*/
#define RESERVED_LEN_FOR_REMAINING_IES 360


struct hostapd_iface;
struct hostapd_data;
struct sta_info;
struct hostapd_frame_info;
struct ieee80211_ht_capabilities;
struct ieee80211_vht_capabilities;
struct ieee80211_he_capabilities;
struct ieee80211_mgmt;
struct ieee802_11_elems;
struct vlan_description;
struct hostapd_sta_wpa_psk_short;
struct intel_vendor_event_coc;

u8* hostapd_eid_opmodenotif(struct hostapd_data *hapd, u8 *eid);

int ieee802_11_mgmt(struct hostapd_data *hapd, const u8 *buf, size_t len,
		    struct hostapd_frame_info *fi);
void ieee802_11_mgmt_cb(struct hostapd_data *hapd, const u8 *buf, size_t len,
			u16 stype, int ok);
void hostapd_2040_coex_action(struct hostapd_data *hapd,
			      const struct ieee80211_mgmt *mgmt, size_t len);
void hostapd_obss_beacon(struct hostapd_data *hapd, const struct ieee80211_mgmt *mgmt,
				         struct ieee802_11_elems *elems, int rssi_level);
void hostapd_ltq_update_coc(struct hostapd_iface *, struct intel_vendor_event_coc *, size_t data_len);
void ap_max_nss_omn_elem_timeout(void *eloop_data, void *user_data);
#ifdef NEED_AP_MLME
int ieee802_11_get_mib(struct hostapd_data *hapd, char *buf, size_t buflen);
int ieee802_11_get_mib_sta(struct hostapd_data *hapd, struct sta_info *sta,
			   char *buf, size_t buflen);
#else /* NEED_AP_MLME */
static inline int ieee802_11_get_mib(struct hostapd_data *hapd, char *buf,
				     size_t buflen)
{
	return 0;
}

static inline int ieee802_11_get_mib_sta(struct hostapd_data *hapd,
					 struct sta_info *sta,
					 char *buf, size_t buflen)
{
	return 0;
}
#endif /* NEED_AP_MLME */
u16 hostapd_own_capab_info(struct hostapd_data *hapd);
void ap_ht2040_timeout(void *eloop_data, void *user_data);
u8 * hostapd_eid_ext_capab(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_qos_map_set(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_supp_rates(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_ext_supp_rates(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_ht_capabilities(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_ht_capabilities_assoc_resp(struct hostapd_data *hapd, u8 *eid,
     struct sta_info *sta);
u8 * hostapd_eid_ht_operation(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_overlapping_bss_scan_params(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_vht_capabilities(struct hostapd_data *hapd, u8 *eid, u32 nsts);
u8 * hostapd_eid_vht_operation(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_he_capab(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_he_operation(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_he_mu_edca_parameter_set(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_he_spatial_reuse_parameter_set(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_he_ndp_feedback_report_parameters_set(struct hostapd_data *hapd, u8 *eid);
void hostapd_get_he_capab(struct hostapd_data *hapd,
						   const struct ieee80211_he_capabilities *sta_he_elems,
						   struct ieee80211_he_capabilities *out_he_elems,
						   u32 he_capabilities_len_from_sta);
u16 copy_sta_he_capab(struct hostapd_data *hapd, struct sta_info *sta,
                       const u8 *he_capab, u8 he_capab_len);
u16 copy_sta_he_operation(struct hostapd_data *hapd, struct sta_info *sta,
			const u8 *he_operation, u8 he_operation_len);
u8 * hostapd_eid_vendor_vht(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_wb_chsw_wrapper(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_txpower_envelope(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_rm_enabled_capab(struct hostapd_data *hapd, u8 *eid,
					 size_t len);
u8 * hostapd_eid_multiple_bssid(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_multiple_bssid_idx(struct hostapd_data *hapd, u8 *eid, u8 idx);
u8 * hostapd_eid_nontransmitted_bssid_capability(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_ssid(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_he_6ghz_band_cap(struct hostapd_data *hapd, u8 *eid);

int hostapd_ht_operation_update(struct hostapd_iface *iface);
void ieee802_11_send_sa_query_req(struct hostapd_data *hapd,
				  const u8 *addr, const u8 *trans_id);
void hostapd_get_ht_capab(struct hostapd_data *hapd,
			  struct ieee80211_ht_capabilities *ht_cap,
			  struct ieee80211_ht_capabilities *neg_ht_cap);
void hostapd_get_vht_capab(struct hostapd_data *hapd,
			   struct ieee80211_vht_capabilities *vht_cap,
			   struct ieee80211_vht_capabilities *neg_vht_cap);
int hostapd_get_aid(struct hostapd_data *hapd, struct sta_info *sta);
u16 copy_sta_ht_capab(struct hostapd_data *hapd, struct sta_info *sta,
		      const u8 *ht_capab);
u16 copy_sta_ht_operation(struct hostapd_data *hapd, struct sta_info *sta,
		      const u8 *ht_operation);
u16 copy_sta_vendor_vht(struct hostapd_data *hapd, struct sta_info *sta,
			const u8 *ie, size_t len);
u16 copy_sta_vendor2_vht(struct hostapd_data *hapd, struct sta_info *sta,
			const u8 *vht_capab);
u16 copy_supp_rates(struct hostapd_data *hapd, struct sta_info *sta,
			   struct ieee802_11_elems *elems);
u16 check_ext_capab(struct hostapd_data *hapd, struct sta_info *sta,
			   const u8 *ext_capab_ie, size_t ext_capab_ie_len);

int update_ht_state(struct hostapd_data *hapd, struct sta_info *sta);
void ht40_intolerant_add(struct hostapd_iface *iface, struct sta_info *sta);
void ht40_intolerant_remove(struct hostapd_iface *iface, struct sta_info *sta);
u16 copy_sta_vht_capab(struct hostapd_data *hapd, struct sta_info *sta,
		       const u8 *vht_capab);
u16 copy_sta_vht_oper(struct hostapd_data *hapd, struct sta_info *sta,
		      const u8 *vht_oper);
u16 copy_sta_vht_operation(struct hostapd_data *hapd, struct sta_info *sta,
		       const u8 *vht_operation);
u16 set_sta_vht_opmode(struct hostapd_data *hapd, struct sta_info *sta,
		       const u8 *vht_opmode);
u16 copy_sta_he_6ghz_capab(struct hostapd_data *hapd, struct sta_info *sta,
			   const u8 *he_6ghz_capab);
void hostapd_tx_status(struct hostapd_data *hapd, const u8 *addr,
		       const u8 *buf, size_t len, int ack);
void hostapd_eapol_tx_status(struct hostapd_data *hapd, const u8 *dst,
			     const u8 *data, size_t len, int ack);
void ieee802_11_rx_from_unknown(struct hostapd_data *hapd, const u8 *src,
				int wds);
u8 * hostapd_eid_assoc_comeback_time(struct hostapd_data *hapd,
				     struct sta_info *sta, u8 *eid);
void ieee802_11_sa_query_action(struct hostapd_data *hapd,
				const struct ieee80211_mgmt *mgmt,
				size_t len);
u8 * hostapd_eid_interworking(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_adv_proto(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_roaming_consortium(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_time_adv(struct hostapd_data *hapd, u8 *eid);
u8 * hostapd_eid_time_zone(struct hostapd_data *hapd, u8 *eid);
int hostapd_update_time_adv(struct hostapd_data *hapd);
void hostapd_client_poll_ok(struct hostapd_data *hapd, const u8 *addr);
u8 * hostapd_eid_bss_max_idle_period(struct hostapd_data *hapd, u8 *eid);

int auth_sae_init_committed(struct hostapd_data *hapd, struct sta_info *sta);
#ifdef CONFIG_SAE
void sae_clear_retransmit_timer(struct hostapd_data *hapd,
				struct sta_info *sta);
void sae_accept_sta(struct hostapd_data *hapd, struct sta_info *sta);
#else /* CONFIG_SAE */
static inline void sae_clear_retransmit_timer(struct hostapd_data *hapd,
					      struct sta_info *sta)
{
}
#endif /* CONFIG_SAE */

#ifdef CONFIG_MBO

u8 * hostapd_eid_mbo(struct hostapd_data *hapd, u8 *eid, size_t len);

u8 hostapd_mbo_ie_len(struct hostapd_data *hapd);

u8 * hostapd_eid_mbo_rssi_assoc_rej(struct hostapd_data *hapd, u8 *eid,
				    size_t len, int delta);

#else /* CONFIG_MBO */

static inline u8 * hostapd_eid_mbo(struct hostapd_data *hapd, u8 *eid,
				   size_t len)
{
	return eid;
}

static inline u8 hostapd_mbo_ie_len(struct hostapd_data *hapd)
{
	return 0;
}

#endif /* CONFIG_MBO */

u8 * hostapd_eid_multi_ap(struct hostapd_data *hapd, u8 *eid, size_t len);

void ap_copy_sta_supp_op_classes(struct sta_info *sta,
				 const u8 *supp_op_classes,
				 size_t supp_op_classes_len);

u8 * hostapd_eid_fils_indic(struct hostapd_data *hapd, u8 *eid, int hessid);
void ieee802_11_finish_fils_auth(struct hostapd_data *hapd,
				 struct sta_info *sta, int success,
				 struct wpabuf *erp_resp,
				 const u8 *msk, size_t msk_len);
u8 * owe_assoc_req_process(struct hostapd_data *hapd, struct sta_info *sta,
			   const u8 *owe_dh, u8 owe_dh_len,
			   u8 *owe_buf, size_t owe_buf_len, u16 *status);
u16 owe_process_rsn_ie(struct hostapd_data *hapd, struct sta_info *sta,
		       const u8 *rsn_ie, size_t rsn_ie_len,
		       const u8 *owe_dh, size_t owe_dh_len);
u16 owe_validate_request(struct hostapd_data *hapd, const u8 *peer,
			 const u8 *rsn_ie, size_t rsn_ie_len,
			 const u8 *owe_dh, size_t owe_dh_len);
void fils_hlp_timeout(void *eloop_ctx, void *eloop_data);
void fils_hlp_finish_assoc(struct hostapd_data *hapd, struct sta_info *sta);
void handle_auth_fils(struct hostapd_data *hapd, struct sta_info *sta,
		      const u8 *pos, size_t len, u16 auth_alg,
		      u16 auth_transaction, u16 status_code,
		      void (*cb)(struct hostapd_data *hapd,
				 struct sta_info *sta,
				 u16 resp, struct wpabuf *data, int pub));

size_t hostapd_eid_owe_trans_len(struct hostapd_data *hapd);
u8 * hostapd_eid_owe_trans(struct hostapd_data *hapd, u8 *eid, size_t len);
int ieee802_11_allowed_address(struct hostapd_data *hapd, const u8 *addr,
			       const u8 *msg, size_t len, u32 *session_timeout,
			       u32 *acct_interim_interval,
			       struct vlan_description *vlan_id,
			       struct hostapd_sta_wpa_psk_short **psk,
			       char **identity, char **radius_cui,
			       int is_probe_req);

size_t hostapd_eid_dpp_cc_len(struct hostapd_data *hapd);
u8 * hostapd_eid_dpp_cc(struct hostapd_data *hapd, u8 *eid, size_t len);

int get_tx_parameters(struct sta_info *sta, int ap_max_chanwidth,
		      int ap_seg1_idx, int *bandwidth, int *seg1_idx);

void auth_sae_process_commit(void *eloop_ctx, void *user_ctx);
u8 * hostapd_eid_rsnxe(struct hostapd_data *hapd, u8 *eid, size_t len);

int get_he_mcs_nss_size(const u8 *he_capab);
int get_he_ppe_th_size(const u8 *he_capab);
u8 round_pow2_up(u8 x);

void _notify_action_frame(struct hostapd_data *hapd, const u8 *action_frame,
			      size_t length);
u16 check_6ghz_assoc_valid(struct sta_info *sta);
int multiple_bssid_ie_get_len(struct hostapd_data *hapd);

#endif /* IEEE802_11_H */
