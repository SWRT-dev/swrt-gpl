#ifndef WLCTRL_H
#define WLCTRL_H

int bcmwl_radio_get_noise(const char *name, int *noise);
int bcmwl_radio_get_channel(const char *name, uint32_t *channel, enum wifi_bw *bw);
int bcmwl_radio_get_sideband(const char *name, enum wifi_chan_ext *sideband);
int bcmwl_radio_get_country(const char *name, char *alpha2);
int bcmwl_radio_get_countrylist(const char *name, char *cc, int *num);
int bcmwl_radio_get_stats(const char *name, struct wifi_radio_stats *s);
int bcmwl_iface_get_stats(const char *ifname, struct wifi_ap_stats *s);
int bcmwl_get_supported_security_const(const char *name, uint32_t *sec);
int bcmwl_iface_get_auth(const char *name, uint32_t *auth);
int bcmwl_iface_get_wsec(const char *name, uint32_t *enc);
int bcmwl_iface_get_wpa_auth(const char *name, uint32_t *wpa_auth);
int bcmwl_iface_get_beacon_ies(const char *name, uint8_t *ies, int *len);
int bcmwl_iface_ap_info(const char *name, struct wifi_ap *ap);
int bcmwl_ap_get_oper_stds(const char *name, uint8_t *std);
int bcmwl_get_oper_stds(const char *name, uint8_t *std);
int bcmwl_get_supp_stds(const char *name, uint8_t *std);
int bcmwl_iface_sta_info(const char *ifname, struct wifi_sta *sta);
int bcmwl_driver_info(const char *name, struct wifi_metainfo *info);
int bcmwl_radio_acs(const char *ifname, struct acs_param *p);
int bcmwl_start_cac(const char *ifname, int channel, enum wifi_bw bw,
		    enum wifi_cac_method method);
int bcmwl_stop_cac(const char *ifname);
int bcmwl_simulate_radar(const char *ifname, struct wifi_radar_args *radar);
int bcmwl_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *wsta);
int bcmwl_iface_chan_switch(const char *ifname, struct chan_switch_param *param);
int bcmwl_iface_link_measure(const char *ifname, uint8_t *sta);
int bcmwl_iface_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas);
int bcmwl_radio_channels_info(const char *ifname, struct chan_entry *channel, int *num);
int bcmwl_radio_get_beacon_int(const char *name, uint32_t *beacon_int);
int bcmwl_radio_get_dtim(const char *name, uint32_t *dtim_period);
int bcmwl_radio_get_txpower(const char *name, int8_t *txpower_dbm, int8_t *txpower_percent);
int bcmwl_radio_get_supp_bw(const char *name, uint32_t *supp_bw);
int bcmwl_radio_get_gi(const char *name, int he, enum wifi_guard *gi);
int bcmwl_radio_get_hw_txchain(const char *name, uint8_t *txchains);
int bcmwl_radio_get_hw_rxchain(const char *name, uint8_t *rxchains);
int bcmwl_radio_get_maxrates(const char *name, uint32_t *he_mcs, uint32_t *vht_mcs, uint32_t *ht_mcs, uint32_t *lr);
int bcmwl_radio_get_basic_rates(const char *name, int *num, uint32_t *rates);
int bcmwl_radio_get_oper_rates(const char *name, int *num, uint32_t *rates);
int bcmwl_radio_get_diag(const char *name, struct wifi_radio_diagnostic *diag);
int bcmwl_iface_restrict_sta(const char *ifname, uint8_t *sta, int enable);
int bcmwl_iface_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg);
int bcmwl_iface_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *monsta);
int bcmwl_iface_add_vendor_ie(const char *ifname, struct vendor_iereq *req);
int bcmwl_iface_del_vendor_ie(const char *ifname, struct vendor_iereq *req);
int bcmwl_iface_mbo_disallow_assoc(const char *ifname, uint8_t reason);
int bcmwl_iface_ap_set_state(const char *ifname, bool up);
int bcmwl_iface_get_4addr_parent(const char *ifname, char *parent);
int bcmwl_radio_get_channels(const char *name, enum wifi_bw bw, enum wifi_chan_ext sb, uint32_t *channels, int *num);
int bcmwl_get_oper_band(const char *name, enum wifi_band *band);
int bcmwl_iface_get_exp_tp(const char *ifname, uint8_t *macaddr, struct wifi_sta *sta);

int bcmwl_register_event(const char *ifname, struct event_struct *ev, void **evhandle);
int bcmwl_unregister_event(const char *ifname, void *evhandle);
int bcmwl_recv_event(const char *ifname, void *evhandle);

int bcmwl_enable_event_bit(const char *ifname, unsigned int bit);
int bcmwl_disable_event_bit(const char *ifname, unsigned int bit);

int bcmwl_radio_reset_chanim_stats(const char *name);
int bcmwl_radio_get_cac_methods(const char *name, uint32_t *methods);
#endif /* WLCTRL_H */
