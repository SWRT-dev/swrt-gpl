#ifndef WPA_CLI_H
#define WPA_CLI_H

#define KEY_MGMT_WPA            0x00000001
#define KEY_MGMT_WPA2           0x00000002
#define KEY_MGMT_WPA_PSK        0x00000004
#define KEY_MGMT_WPA2_PSK       0x00000008
#define KEY_MGMT_WPA_NONE       0x00000010
#define KEY_MGMT_FT             0x00000020
#define KEY_MGMT_FT_PSK         0x00000040
#define KEY_MGMT_WAPI_PSK       0x00000080
#define KEY_MGMT_SUITE_B        0x00000100
#define KEY_MGMT_SUITE_B_192    0x00000200
#define KEY_MGMT_OWE            0x00000400
#define KEY_MGMT_DPP            0x00000800
#define KEY_MGMT_FILS_SHA256    0x00001000
#define KEY_MGMT_FILS_SHA384    0x00002000
#define KEY_MGMT_FT_FILS_SHA256 0x00004000
#define KEY_MGMT_FT_FILS_SHA384 0x00008000
#define KEY_MGMT_SAE            0x00010000
#define KEY_MGMT_802_1X_SHA256  0x00020000
#define KEY_MGMT_PSK_SHA256     0x00040000
#define KEY_MGMT_TPK_HANDSHAKE  0x00080000
#define KEY_MGMT_FT_SAE         0x00100000
#define KEY_MGMT_FT_802_1X_SHA384       0x00200000
#define KEY_MGMT_CCKM           0x00400000
#define KEY_MGMT_OSEN           0x00800000

#define ENC_WEP40       0x00000001
#define ENC_WEP104      0x00000002
#define ENC_TKIP        0x00000004
#define ENC_CCMP        0x00000008
#define ENC_WEP128      0x00000010
#define ENC_GCMP        0x00000020
#define ENC_GCMP_256    0x00000040
#define ENC_CCMP_256    0x00000080
#define ENC_BIP         0x00000100
#define ENC_BIP_GMAC_128        0x00000200
#define ENC_BIP_GMAC_256        0x00000400
#define ENC_BIP_CMAC_256        0x00000800
#define ENC_GTK_NOT_USED        0x00001000

/** struct bss_transition_params */
struct bss_transition_params {
	uint8_t valid_int;
	uint8_t abridged;
	uint8_t pref;
	uint8_t ess_term;
	uint8_t disassoc_imminent;
	uint16_t bss_term;
	uint16_t disassoc_timer;
	char url[1024];
	struct wifi_btmreq_mbo mbo;
};

struct beacon_frame {
	uint16_t fc;
	uint16_t dur;
	uint8_t da[6];
	uint8_t sa[6];
	uint8_t bssid[6];
	uint16_t seq;
	uint8_t ts[8];
	uint16_t beacon_int;
	uint16_t cap_info;
	uint8_t var[0];
} __attribute__((packed));

int hostapd_cli_get_ssid(const char *ifname, char *ssid, size_t ssid_size);
int hostapd_cli_iface_ap_info(const char *ifname, struct wifi_ap *ap);
int hostapd_iface_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas);
int hostapd_cli_iface_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *info);
int hostapd_cli_iface_add_neighbor(const char *ifname, struct nbr *nbr, size_t nbr_size);
int hostapd_cli_iface_del_neighbor(const char *ifname, unsigned char *bssid);
int hostapd_cli_iface_get_neighbor_list(const char *ifname, struct nbr *nbrs, int *nr);
int hostapd_cli_iface_start_wps(const char *ifname, struct wps_param wps);
int hostapd_cli_iface_stop_wps(const char *ifname);
int hostapd_cli_iface_get_wps_status(const char *ifname, enum wps_status *s);
int hostapd_cli_iface_set_wps_ap_pin(const char *ifname, unsigned long pin);
int hostapd_cli_iface_get_wps_ap_pin(const char *ifname, unsigned long *pin);
int hostapd_cli_disconnect_sta(const char *ifname, uint8_t *sta, uint16_t reason);
int hostapd_cli_probe_sta(const char *ifname, uint8_t *sta);
int hostapd_cli_sta_disconnect_ap(const char *ifname, uint32_t reason);
int hostapd_cli_iface_req_beacon(const char *ifname, unsigned char *sta,
				 struct wifi_beacon_req *req,
				 size_t req_size);
int hostapd_cli_iface_req_beacon_default(const char *ifname, unsigned char *sta);
int hostapd_cli_iface_req_bss_transition(const char *ifname, unsigned char *sta,
					 int bsss_nr, struct nbr *bsss,
					 struct bss_transition_params *params);
int hostapd_cli_mbo_disallow_assoc(const char *ifname, int disallow);
int hostapd_cli_ap_get_state(const char *ifname, char *state, int state_size);
int hostapd_cli_ap_set_state(const char *ifname, bool up);
int hostapd_cli_iface_add_vendor_ie(const char *ifname, struct vendor_iereq *req);
int hostapd_cli_iface_del_vendor_ie(const char *ifname, struct vendor_iereq *req);
int hostapd_cli_is_wds_sta(const char *ifname, uint8_t *sta_mac, char *ifname_wds,
			   size_t ifname_max);
int hostapd_cli_get_oper_stds(const char *ifname, uint8_t *std);
int hostapd_cli_get_beacon(const char *ifname, uint8_t *beacon, size_t *beacon_len);
int hostapd_cli_get_beacon_ies(const char *ifname, uint8_t *beacon_ies, size_t *beacon_ies_len);
int hostapd_ubus_iface_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg);
int hostapd_ubus_iface_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *mon);
int hostapd_ubus_iface_get_monitor_stas(const char *ifname, struct wifi_monsta *stas, int *num);
int hostapd_ubus_iface_subscribe_frame(const char *ifname, uint8_t type, uint8_t stype);
int hostapd_ubus_iface_unsubscribe_frame(const char *ifname, uint8_t type, uint8_t stype);
int hostapd_cli_iface_chan_switch(const char *ifname, struct chan_switch_param *param);
int hostapd_cli_get_security_cap(const char *name, uint32_t *sec);
int hostapd_cli_get_4addr_parent(const char* ifname, char* parent);

int supplicant_sta_info(const char *ifname, struct wifi_sta *info);
int supplicant_cli_get_oper_std(const char *ifname, uint8_t *std);
int supplicant_cli_get_sta_supported_security(const char *ifname, uint32_t *sec);
int supplicant_cli_get_sta_security(const char *ifname, struct wifi_sta_security *sec);

#endif /* WPA_CLI_H */
