/*
 * hostapd / Configuration definitions and helpers functions
 * Copyright (c) 2003-2015, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef HOSTAPD_CONFIG_H
#define HOSTAPD_CONFIG_H

#include "common/defs.h"
#include "utils/list.h"
#include "ip_addr.h"
#include "common/wpa_common.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "common/vendor_cmds_copy.h"
#include "crypto/sha256.h"
#include "wps/wps.h"
#include "fst/fst.h"
#include "vlan.h"
#include "atf.h"
#include "utils/eloop.h"

#define SRX_TH_VAL -82 /* adaptive sensitivity threshold */

/**
 * mesh_conf - local MBSS state and settings
 */
struct mesh_conf {
	u8 meshid[32];
	u8 meshid_len;
	/* Active Path Selection Protocol Identifier */
	u8 mesh_pp_id;
	/* Active Path Selection Metric Identifier */
	u8 mesh_pm_id;
	/* Congestion Control Mode Identifier */
	u8 mesh_cc_id;
	/* Synchronization Protocol Identifier */
	u8 mesh_sp_id;
	/* Authentication Protocol Identifier */
	u8 mesh_auth_id;
	u8 *rsn_ie;
	int rsn_ie_len;
#define MESH_CONF_SEC_NONE BIT(0)
#define MESH_CONF_SEC_AUTH BIT(1)
#define MESH_CONF_SEC_AMPE BIT(2)
	unsigned int security;
	enum mfp_options ieee80211w;
	int ocv;
	unsigned int pairwise_cipher;
	unsigned int group_cipher;
	unsigned int mgmt_group_cipher;
	int dot11MeshMaxRetries;
	int dot11MeshRetryTimeout; /* msec */
	int dot11MeshConfirmTimeout; /* msec */
	int dot11MeshHoldingTimeout; /* msec */
};

#define MAX_STA_COUNT 2007
#define MAX_VLAN_ID 4094
#define VLAN_ID_DISABLED 0

typedef u8 macaddr[ETH_ALEN];

struct mac_acl_entry {
	macaddr addr;
	struct vlan_description vlan_id;
};

struct soft_block_acl_entry {
	struct dl_list list;
	macaddr addr;
	eloop_timeout_handler wait_handler;
	eloop_timeout_handler allow_handler;
	void *eloop_ctx;
	void *user_ctx;
};

struct soft_block_acl_db {
	struct dl_list acls;
	struct soft_block_acl_entry **indexes;
	int num;
};

struct hostapd_radius_servers;
struct ft_remote_r0kh;
struct ft_remote_r1kh;

#ifdef CONFIG_WEP
#define NUM_WEP_KEYS 4
struct hostapd_wep_keys {
	u8 idx;
	u8 *key[NUM_WEP_KEYS];
	size_t len[NUM_WEP_KEYS];
	int keys_set;
	size_t default_len; /* key length used for dynamic key generation */
};
#endif /* CONFIG_WEP */

typedef enum hostap_security_policy {
	SECURITY_PLAINTEXT = 0,
#ifdef CONFIG_WEP
	SECURITY_STATIC_WEP = 1,
#endif /* CONFIG_WEP */
	SECURITY_IEEE_802_1X = 2,
	SECURITY_WPA_PSK = 3,
	SECURITY_WPA = 4,
	SECURITY_OSEN = 5
} secpolicy;

struct hostapd_ssid {
	u8 ssid[SSID_MAX_LEN];
	size_t ssid_len;
	u32 short_ssid;
	unsigned int ssid_set:1;
	unsigned int utf8_ssid:1;
	unsigned int wpa_passphrase_set:1;
	unsigned int wpa_psk_set:1;
	unsigned int request_credentials:1;

	char vlan[IFNAMSIZ + 1];
	secpolicy security_policy;

	struct hostapd_wpa_psk *wpa_psk;
	char *wpa_passphrase;
	char *wpa_psk_file;
	struct sae_pt *pt;

#ifdef CONFIG_WEP
	struct hostapd_wep_keys wep;
#endif /* CONFIG_WEP */

#define DYNAMIC_VLAN_DISABLED 0
#define DYNAMIC_VLAN_OPTIONAL 1
#define DYNAMIC_VLAN_REQUIRED 2
	int dynamic_vlan;
#define DYNAMIC_VLAN_NAMING_WITHOUT_DEVICE 0
#define DYNAMIC_VLAN_NAMING_WITH_DEVICE 1
#define DYNAMIC_VLAN_NAMING_END 2
	int vlan_naming;
	int per_sta_vif;
#ifdef CONFIG_FULL_DYNAMIC_VLAN
	char *vlan_tagged_interface;
#endif /* CONFIG_FULL_DYNAMIC_VLAN */
};


#define VLAN_ID_WILDCARD -1

struct hostapd_vlan {
	struct hostapd_vlan *next;
	int vlan_id; /* VLAN ID or -1 (VLAN_ID_WILDCARD) for wildcard entry */
	struct vlan_description vlan_desc;
	char ifname[IFNAMSIZ + 1];
	char bridge[IFNAMSIZ + 1];
	int configured;
	int dynamic_vlan;
#ifdef CONFIG_FULL_DYNAMIC_VLAN

#define DVLAN_CLEAN_WLAN_PORT	0x8
	int clean;
#endif /* CONFIG_FULL_DYNAMIC_VLAN */
};

#define PMK_LEN 32
#define KEYID_LEN 32
#define MIN_PASSPHRASE_LEN 8
#define MAX_PASSPHRASE_LEN 63
struct hostapd_sta_wpa_psk_short {
	struct hostapd_sta_wpa_psk_short *next;
	unsigned int is_passphrase:1;
	u8 psk[PMK_LEN];
	char passphrase[MAX_PASSPHRASE_LEN + 1];
	int ref; /* (number of references held) - 1 */
};

struct hostapd_wpa_psk {
	struct hostapd_wpa_psk *next;
	int group;
	char keyid[KEYID_LEN];
	char wpa_passphrase[MAX_PASSPHRASE_LEN + 1];
	int wps;
	u8 psk[PMK_LEN];
	u8 addr[ETH_ALEN];
	u8 p2p_dev_addr[ETH_ALEN];
	int vlan_id;
};

struct hostapd_eap_user {
	struct hostapd_eap_user *next;
	u8 *identity;
	size_t identity_len;
	struct {
		int vendor;
		u32 method;
	} methods[EAP_MAX_METHODS];
	u8 *password;
	size_t password_len;
	u8 *salt;
	size_t salt_len; /* non-zero when password is salted */
	int phase2;
	int force_version;
	unsigned int wildcard_prefix:1;
	unsigned int password_hash:1; /* whether password is hashed with
				       * nt_password_hash() */
	unsigned int remediation:1;
	unsigned int macacl:1;
	int ttls_auth; /* EAP_TTLS_AUTH_* bitfield */
	struct hostapd_radius_attr *accept_attr;
	u32 t_c_timestamp;
};

struct hostapd_radius_attr {
	u8 type;
	struct wpabuf *val;
	struct hostapd_radius_attr *next;
};


#define NUM_TX_QUEUES 4
#define MAX_ROAMING_CONSORTIUM_LEN 15

struct hostapd_roaming_consortium {
	u8 len;
	u8 oi[MAX_ROAMING_CONSORTIUM_LEN];
};

struct hostapd_lang_string {
	u8 lang[3];
	u8 name_len;
	u8 name[252];
};

struct hostapd_venue_url {
	u8 venue_number;
	u8 url_len;
	u8 url[254];
};

#define MAX_NAI_REALMS 10
#define MAX_NAI_REALMLEN 255
#define MAX_NAI_EAP_METHODS 5
#define MAX_NAI_AUTH_TYPES 4
struct hostapd_nai_realm_data {
	u8 encoding;
	char realm_buf[MAX_NAI_REALMLEN + 1];
	char *realm[MAX_NAI_REALMS];
	u8 eap_method_count;
	struct hostapd_nai_realm_eap {
		u8 eap_method;
		u8 num_auths;
		u8 auth_id[MAX_NAI_AUTH_TYPES];
		u8 auth_val[MAX_NAI_AUTH_TYPES];
	} eap_method[MAX_NAI_EAP_METHODS];
};

struct anqp_element {
	struct dl_list list;
	u16 infoid;
	struct wpabuf *payload;
};

struct fils_realm {
	struct dl_list list;
	u8 hash[2];
	char realm[];
};

struct sae_password_entry {
	struct sae_password_entry *next;
	char *password;
	char *identifier;
	u8 peer_addr[ETH_ALEN];
	int vlan_id;
	struct sae_pt *pt;
	struct sae_pk *pk;
};
typedef enum mesh_mode {
	MESH_MODE_FRONTHAUL_AP	= 0,
	MESH_MODE_BACKHAUL_AP	= 1,
	MESH_MODE_HYBRID	= 2,
	MESH_MODE_EXT_HYBRID	= 3,
	MESH_MODE_RESERVED	= 4,
	MESH_MODE_LAST
} mesh_mode_t;

#define MESH_MODE_DEFAULT	MESH_MODE_FRONTHAUL_AP

struct dpp_controller_conf {
	struct dpp_controller_conf *next;
	u8 pkhash[SHA256_MAC_LEN];
	struct hostapd_ip_addr ipaddr;
};

struct airtime_sta_weight {
	struct airtime_sta_weight *next;
	unsigned int weight;
	u8 addr[ETH_ALEN];
};

/**
 * struct hostapd_bss_config - Per-BSS configuration
 */
struct hostapd_bss_config {
	char iface[IFNAMSIZ + 1];
	char bridge[IFNAMSIZ + 1];
	char vlan_bridge[IFNAMSIZ + 1];
	char wds_bridge[IFNAMSIZ + 1];

	enum hostapd_logger_level logger_syslog_level, logger_stdout_level;

	unsigned int logger_syslog; /* module bitfield */
	unsigned int logger_stdout; /* module bitfield */

	int max_num_sta; /* maximum number of STAs in station table */
	int num_res_sta; /* number of reserved STAs in the BSS */
	int num_vrt_bkh_netdevs; /* maximun number of backhual STAs for ext_hybrid interface vap */

	u16 beacon_int;
	int dtim_period;
	int management_frames_rate;
	int dynamic_multicast_mode;
	int dynamic_multicast_rate;
	unsigned int bss_load_update_period;
	unsigned int chan_util_avg_period;
	int enable_bss_load_ie;

	int ieee802_1x; /* use IEEE 802.1X */
	int eapol_version;
	int eap_server; /* Use internal EAP server instead of external
			 * RADIUS server */
	struct hostapd_eap_user *eap_user;
	char *eap_user_sqlite;
	char *eap_sim_db;
	unsigned int eap_sim_db_timeout;
	int eap_server_erp; /* Whether ERP is enabled on internal EAP server */
	struct hostapd_ip_addr own_ip_addr;
	char *nas_identifier;
	struct hostapd_radius_servers *radius;
	int acct_interim_interval;
	int radius_request_cui;
	struct hostapd_radius_attr *radius_auth_req_attr;
	struct hostapd_radius_attr *radius_acct_req_attr;
	char *radius_req_attr_sqlite;
	int radius_das_port;
	unsigned int radius_das_time_window;
	int radius_das_require_event_timestamp;
	int radius_das_require_message_authenticator;
	struct hostapd_ip_addr radius_das_client_addr;
	u8 *radius_das_shared_secret;
	size_t radius_das_shared_secret_len;

	struct hostapd_ssid ssid;

	char *eap_req_id_text; /* optional displayable message sent with
				* EAP Request-Identity */
	size_t eap_req_id_text_len;
	int eap_req_id_retry_interval; /* Time Interval in seconds between
				identity requests retries. A value of 0 (zero) disables it */
	int eap_aaa_req_retries; /* Number of retries for Radius requests */
	int eap_aaa_req_timeout; /* Radius request timeout in seconds after which
			the request must be retransmitted for the # of retries available */
	int max_eap_failure; /* Number of time a client can attempt to
				login with incorrect credentials. When this limit is reached,
				the client is blacklisted and not allowed to attempt loging
				into the network. Settings this parameter to 0 (zero) disables
				the blacklisting feature.*/
	int auth_fail_blacklist_duration; /* Time interval in seconds for which a
				client will continue to be blacklisted once it is marked so.*/
	int auth_quiet_period; /* The enforced quiet period (time interval) in
				seconds following failed authentication.A value of 0 (zero)
				disables it.*/
	int eapol_key_index_workaround;

#ifdef CONFIG_WEP
	size_t default_wep_key_len;
	int individual_wep_key_len;
	int wep_rekeying_period;
	int broadcast_key_idx_min, broadcast_key_idx_max;
#endif /* CONFIG_WEP */
	int eap_reauth_period;
	int erp_send_reauth_start;
	char *erp_domain;

	enum macaddr_acl {
		ACCEPT_UNLESS_DENIED = 0,
		DENY_UNLESS_ACCEPTED = 1,
		USE_EXTERNAL_RADIUS_AUTH = 2
	} macaddr_acl;
	struct mac_acl_entry *accept_mac;
	int num_accept_mac;
	struct mac_acl_entry *deny_mac;
	int num_deny_mac;
	struct soft_block_acl_db deny_mac_sb;
	int wds_sta;
	int isolate;
	int enable_hairpin;
	int start_disabled;

	int auth_algs; /* bitfield of allowed IEEE 802.11 authentication
			* algorithms, WPA_AUTH_ALG_{OPEN,SHARED,LEAP} */

	int wpa; /* bitfield of WPA_PROTO_WPA, WPA_PROTO_RSN */
	int wpa_key_mgmt;
	enum mfp_options ieee80211w;
	int group_mgmt_cipher;
	/* dot11AssociationSAQueryMaximumTimeout (in TUs) */
	unsigned int assoc_sa_query_max_timeout;
	/* dot11AssociationSAQueryRetryTimeout (in TUs) */
	int assoc_sa_query_retry_timeout;
	int beacon_protection_enabled;
	int disable_bigtk_rekey;
#ifdef CONFIG_OCV
	int ocv; /* Operating Channel Validation */
#endif /* CONFIG_OCV */
	enum {
		PSK_RADIUS_IGNORED = 0,
		PSK_RADIUS_ACCEPTED = 1,
		PSK_RADIUS_REQUIRED = 2
	} wpa_psk_radius;
	int wpa_pairwise;
	int group_cipher; /* wpa_group value override from configuation */
	int wpa_group;
	int wpa_group_rekey;
	int wpa_group_rekey_set;
	int wpa_strict_rekey;
	int wpa_gmk_rekey;
	int wpa_ptk_rekey;
	enum ptk0_rekey_handling wpa_deny_ptk0_rekey;
	u32 wpa_group_update_count;
	u32 wpa_pairwise_update_count;
	int wpa_disable_eapol_key_retries;
	int rsn_pairwise;
	int rsn_preauth;
	char *rsn_preauth_interfaces;

#ifdef CONFIG_IEEE80211R_AP
	/* IEEE 802.11r - Fast BSS Transition */
	u8 mobility_domain[MOBILITY_DOMAIN_ID_LEN];
	u8 r1_key_holder[FT_R1KH_ID_LEN];
	u32 r0_key_lifetime; /* PMK-R0 lifetime seconds */
	int rkh_pos_timeout;
	int rkh_neg_timeout;
	int rkh_pull_timeout; /* ms */
	int rkh_pull_retries;
	u32 reassociation_deadline;
	struct ft_remote_r0kh *r0kh_list;
	struct ft_remote_r1kh *r1kh_list;
	int pmk_r1_push;
	int ft_over_ds;
	int ft_psk_generate_local;
	int r1_max_key_lifetime;
#endif /* CONFIG_IEEE80211R_AP */

	char *ctrl_interface; /* directory for UNIX domain sockets */
#ifndef CONFIG_NATIVE_WINDOWS
	gid_t ctrl_interface_gid;
#endif /* CONFIG_NATIVE_WINDOWS */
	int ctrl_interface_gid_set;

	char *ca_cert;
	char *server_cert;
	char *server_cert2;
	char *private_key;
	char *private_key2;
	char *private_key_passwd;
	char *private_key_passwd2;
	char *check_cert_subject;
	int check_crl;
	int check_crl_strict;
	unsigned int crl_reload_interval;
	unsigned int tls_session_lifetime;
	unsigned int tls_flags;
	unsigned int max_auth_rounds;
	unsigned int max_auth_rounds_short;
	char *ocsp_stapling_response;
	char *ocsp_stapling_response_multi;
	char *dh_file;
	char *openssl_ciphers;
	char *openssl_ecdh_curves;
	u8 *pac_opaque_encr_key;
	u8 *eap_fast_a_id;
	size_t eap_fast_a_id_len;
	char *eap_fast_a_id_info;
	int eap_fast_prov;
	int pac_key_lifetime;
	int pac_key_refresh_time;
	int eap_teap_auth;
	int eap_teap_pac_no_inner;
	int eap_teap_separate_result;
	int eap_teap_id;
	int eap_sim_aka_result_ind;
	int eap_sim_id;
	int tnc;
	int fragment_size;
	u16 pwd_group;

	char *radius_server_clients;
	int radius_server_auth_port;
	int radius_server_acct_port;
	int radius_server_ipv6;

	int use_pae_group_addr; /* Whether to send EAPOL frames to PAE group
				 * address instead of individual address
				 * (for driver_wired.c).
				 */

	int ap_max_inactivity;
	int ap_protected_keep_alive_required;
	int ignore_broadcast_ssid;
	int no_probe_resp_if_max_sta;

	int wmm_enabled;
	int wmm_uapsd;

	struct hostapd_vlan *vlan;

	macaddr bssid;

	/*
	 * Maximum listen interval that STAs can use when associating with this
	 * BSS. If a STA tries to use larger value, the association will be
	 * denied with status code 51.
	 */
	u16 max_listen_interval;

	int disable_pmksa_caching;
	int okc; /* Opportunistic Key Caching */
	/* Default time in seconds after which a Wi-Fi client is forced
	 * to ReAuthenticate
	 */
	int pmksa_life_time;
	/* Time interval in seconds after which the PMKSA (Pairwise Master Key
	 * Security Association) cache is purged
	 */
	int pmksa_interval;
	int wps_state;
#ifdef CONFIG_WPS
	int wps_independent;
	int ap_setup_locked;
	u8 uuid[16];
	char *wps_pin_requests;
	char *device_name;
	char *manufacturer;
	char *model_name;
	char *model_number;
	char *serial_number;
	u8 device_type[WPS_DEV_TYPE_LEN];
	char *config_methods;
	u8 os_version[4];
	char *ap_pin;
	int skip_cred_build;
	u8 *extra_cred;
	size_t extra_cred_len;
	int wps_cred_processing;
	int wps_cred_add_sae;
	int force_per_enrollee_psk;
	u8 *ap_settings;
	size_t ap_settings_len;
	struct hostapd_ssid multi_ap_backhaul_ssid;
	char *upnp_iface;
	char *friendly_name;
	char *manufacturer_url;
	char *model_description;
	char *model_url;
	char *upc;
	struct wpabuf *wps_vendor_ext[MAX_WPS_VENDOR_EXTENSIONS];
	struct wpabuf *wps_application_ext;
	int wps_nfc_pw_from_config;
	int wps_nfc_dev_pw_id;
	struct wpabuf *wps_nfc_dh_pubkey;
	struct wpabuf *wps_nfc_dh_privkey;
	struct wpabuf *wps_nfc_dev_pw;
#endif /* CONFIG_WPS */
	int pbc_in_m1;
	char *server_id;

#define P2P_ENABLED BIT(0)
#define P2P_GROUP_OWNER BIT(1)
#define P2P_GROUP_FORMATION BIT(2)
#define P2P_MANAGE BIT(3)
#define P2P_ALLOW_CROSS_CONNECTION BIT(4)
	int p2p;
#ifdef CONFIG_P2P
	u8 ip_addr_go[4];
	u8 ip_addr_mask[4];
	u8 ip_addr_start[4];
	u8 ip_addr_end[4];
#endif /* CONFIG_P2P */

	int disassoc_low_ack;
	int skip_inactivity_poll;

#define TDLS_PROHIBIT BIT(0)
#define TDLS_PROHIBIT_CHAN_SWITCH BIT(1)
	int tdls;
	int disable_11n;
	int disable_11ac;
	int disable_11ax;
	int twt_responder_support;
	/* IEEE 802.11v */
	int time_advertisement;
	char *time_zone;
	int wnm_sleep_mode;
	int wnm_sleep_mode_no_keys;
	int bss_transition;

	/* IEEE 802.11u - Interworking */
	int interworking;
	int access_network_type;
	int internet;
	int asra;
	int esr;
	int uesa;
	int venue_info_set;
	u8 venue_group;
	u8 venue_type;
	u8 hessid[ETH_ALEN];

	/* IEEE 802.11u - Roaming Consortium list */
	unsigned int roaming_consortium_count;
	struct hostapd_roaming_consortium *roaming_consortium;

	/* override roaming_consortium count for ANQP off-load */
	unsigned int roaming_anqp_ois_count;
	int roaming_anqp_ois_count_set;

	/* IEEE 802.11u - Venue Name duples */
	unsigned int venue_name_count;
	struct hostapd_lang_string *venue_name;

	/* Venue URL duples */
	unsigned int venue_url_count;
	struct hostapd_venue_url *venue_url;

	/* IEEE 802.11u - Network Authentication Type */
	u8 *network_auth_type;
	size_t network_auth_type_len;

	/* IEEE 802.11u - IP Address Type Availability */
	u8 ipaddr_type_availability;
	u8 ipaddr_type_configured;

	/* IEEE 802.11u - 3GPP Cellular Network */
	u8 *anqp_3gpp_cell_net;
	size_t anqp_3gpp_cell_net_len;

	/* IEEE 802.11u - Domain Name */
	u8 *domain_name;
	size_t domain_name_len;

	unsigned int nai_realm_count;
	struct hostapd_nai_realm_data *nai_realm_data;

	struct dl_list anqp_elem; /* list of struct anqp_element */

	u16 gas_comeback_delay;
	size_t gas_frag_limit;
	int gas_address3;

	u8 qos_map_set[16 + 2 * 21];
	unsigned int qos_map_set_len;

	int osen;
	int proxy_arp;
        int opmode_notif;
        int enable_snooping;
	int na_mcast_to_ucast;

#ifdef CONFIG_HS20
	int hs20;
	int hs20_release;
	int offload_gas;
	int disable_dgaf;
	u16 anqp_domain_id;
	unsigned int hs20_oper_friendly_name_count;
	struct hostapd_lang_string *hs20_oper_friendly_name;
	u8 *hs20_wan_metrics;
	u8 *hs20_connection_capability;
	size_t hs20_connection_capability_len;
	u8 *hs20_operating_class;
	u8 hs20_operating_class_len;
	struct hs20_icon {
		u16 width;
		u16 height;
		char language[3];
		char type[256];
		char name[256];
		char file[256];
	} *hs20_icons;
	size_t hs20_icons_count;
	u8 osu_ssid[SSID_MAX_LEN];
	size_t osu_ssid_len;
	struct hs20_osu_provider {
		unsigned int friendly_name_count;
		struct hostapd_lang_string *friendly_name;
		char *server_uri;
		int *method_list;
		char **icons;
		size_t icons_count;
		char *osu_nai;
		char *osu_nai2;
		unsigned int service_desc_count;
		struct hostapd_lang_string *service_desc;
	} *hs20_osu_providers, *last_osu;
	size_t hs20_osu_providers_count;
	size_t hs20_osu_providers_nai_count;
	char **hs20_operator_icon;
	size_t hs20_operator_icon_count;
	unsigned int hs20_deauth_req_timeout;
	char *subscr_remediation_url;
	u8 subscr_remediation_method;
	char *hs20_sim_provisioning_url;
	char *t_c_filename;
	u32 t_c_timestamp;
	char *t_c_server_url;
#endif /* CONFIG_HS20 */

	u8 wps_rf_bands; /* RF bands for WPS (WPS_RF_*) */

#ifdef CONFIG_RADIUS_TEST
	char *dump_msk_file;
#endif /* CONFIG_RADIUS_TEST */

	struct wpabuf *vendor_elements;
	struct wpabuf *authresp_elements;
	struct wpabuf *assocresp_elements;

	unsigned int sae_anti_clogging_threshold;
	unsigned int sae_sync;
	int sae_require_mfp;
	int sae_confirm_immediate;
	int sae_pwe;
	int *sae_groups;
	struct sae_password_entry *sae_passwords;

	char *wowlan_triggers; /* Wake-on-WLAN triggers */

#ifdef CONFIG_TESTING_OPTIONS
	u8 bss_load_test[5];
	u8 bss_load_test_set;
	struct wpabuf *own_ie_override;
	int sae_reflection_attack;
	int sae_commit_status;
	int sae_pk_omit;
	struct wpabuf *sae_commit_override;
	struct wpabuf *rsnxe_override_eapol;
	struct wpabuf *rsne_override_ft;
	struct wpabuf *rsnxe_override_ft;
	struct wpabuf *gtk_rsc_override;
	struct wpabuf *igtk_rsc_override;
	int no_beacon_rsnxe;
	int skip_prune_assoc;
	int ft_rsnxe_used;
	unsigned int oci_freq_override_eapol_m3;
	unsigned int oci_freq_override_eapol_g1;
	unsigned int oci_freq_override_saquery_req;
	unsigned int oci_freq_override_saquery_resp;
	unsigned int oci_freq_override_ft_assoc;
	unsigned int oci_freq_override_fils_assoc;
	unsigned int oci_freq_override_wnm_sleep;
#endif /* CONFIG_TESTING_OPTIONS */

#define MESH_ENABLED BIT(0)
	int mesh;

	u8 radio_measurements[RRM_CAPABILITIES_IE_LEN];

	int rrm_manipulate_measurement;
	int wnm_bss_trans_query_auto_resp;

	int vendor_vht;
	int use_sta_nsts;
	char *no_probe_resp_if_seen_on;
	char *no_auth_if_seen_on;

	int pbss;

#ifdef CONFIG_MBO
	int mbo_enabled;
	/**
	 * oce - Enable OCE in AP and/or STA-CFON mode
	 *  - BIT(0) is Reserved
	 *  - Set BIT(1) to enable OCE in STA-CFON mode
	 *  - Set BIT(2) to enable OCE in AP mode
	 */
	unsigned int oce;
	int mbo_cell_data_conn_pref;
	int mbo_cell_aware;
	int mbo_pmf_bypass;
#endif /* CONFIG_MBO */

	int ftm_responder;
	int ftm_initiator;

#ifdef CONFIG_FILS
	u8 fils_cache_id[FILS_CACHE_ID_LEN];
	int fils_cache_id_set;
	struct dl_list fils_realms; /* list of struct fils_realm */
	int fils_dh_group;
	struct hostapd_ip_addr dhcp_server;
	int dhcp_rapid_commit_proxy;
	unsigned int fils_hlp_wait_time;
	u16 dhcp_server_port;
	u16 dhcp_relay_port;
#endif /* CONFIG_FILS */

	int multicast_to_unicast;

	int broadcast_deauth;

#ifdef CONFIG_DPP
	char *dpp_name;
	char *dpp_mud_url;
	char *dpp_connector;
	struct wpabuf *dpp_netaccesskey;
	unsigned int dpp_netaccesskey_expiry;
	struct wpabuf *dpp_csign;

	/* Timeouts. Copied from hostapd_data */
	unsigned int dpp_init_max_tries;
	unsigned int dpp_init_retry_time;
	unsigned int dpp_resp_wait_time;
	unsigned int dpp_resp_max_tries;
	unsigned int dpp_resp_retry_time;
	unsigned int dpp_gas_query_timeout_period;

#ifdef CONFIG_DPP2
	struct dpp_controller_conf *dpp_controller;
	int dpp_configurator_connectivity;
	int dpp_pfs;
#endif /* CONFIG_DPP2 */
#endif /* CONFIG_DPP */

#ifdef CONFIG_OWE
	macaddr owe_transition_bssid;
	u8 owe_transition_ssid[SSID_MAX_LEN];
	size_t owe_transition_ssid_len;
	char owe_transition_ifname[IFNAMSIZ + 1];
	int *owe_groups;
	int owe_ptk_workaround;
#endif /* CONFIG_OWE */

	int coloc_intf_reporting;
	u8 send_probe_response;
	u8 transition_disable;
#define BACKHAUL_BSS 1
#define FRONTHAUL_BSS 2
	int multi_ap; /* bitmap of BACKHAUL_BSS, FRONTHAUL_BSS */
	int multi_ap_profile1_disallow;
	int multi_ap_profile2_disallow;
	u8 multi_ap_vlan_per_bsta;
#ifdef CONFIG_WDS_WPA
	struct mac_acl_entry *wds_wpa_sta;
	int num_wds_wpa_sta;
#endif

	enum mesh_mode mesh_mode;
	int ul_csa;
	int mesh_mode_set;
	int bap_vifs_follow_bridge;
	u32 scan_timeout;

	int *sAggrConfig; /*amsdu_mode ba_mode window_size*/
	int sAggrConfigSize;
	int s11nProtection;
	int sUdmaEnabled;
	int sUdmaVlanId;
	int bss_idx;
	int sDisableSoftblock;
	int sFwrdUnkwnMcast;
	int sBridgeMode;
	macaddr *sAddPeerAP;
	int num_sAddPeerAP;
	int vendor_4addr;
	u8 ap_retry_limit_exce;
#ifdef CONFIG_AIRTIME_POLICY
	unsigned int airtime_weight;
	int airtime_limit;
	struct airtime_sta_weight *airtime_weight_list;
#endif /* CONFIG_AIRTIME_POLICY */

#ifdef CONFIG_MACSEC
	/**
	 * macsec_policy - Determines the policy for MACsec secure session
	 *
	 * 0: MACsec not in use (default)
	 * 1: MACsec enabled - Should secure, accept key server's advice to
	 *    determine whether to use a secure session or not.
	 */
	int macsec_policy;

	/**
	 * macsec_integ_only - Determines how MACsec are transmitted
	 *
	 * This setting applies only when MACsec is in use, i.e.,
	 *  - macsec_policy is enabled
	 *  - the key server has decided to enable MACsec
	 *
	 * 0: Encrypt traffic (default)
	 * 1: Integrity only
	 */
	int macsec_integ_only;

	/**
	 * macsec_replay_protect - Enable MACsec replay protection
	 *
	 * This setting applies only when MACsec is in use, i.e.,
	 *  - macsec_policy is enabled
	 *  - the key server has decided to enable MACsec
	 *
	 * 0: Replay protection disabled (default)
	 * 1: Replay protection enabled
	 */
	int macsec_replay_protect;

	/**
	 * macsec_replay_window - MACsec replay protection window
	 *
	 * A window in which replay is tolerated, to allow receipt of frames
	 * that have been misordered by the network.
	 *
	 * This setting applies only when MACsec replay protection active, i.e.,
	 *  - macsec_replay_protect is enabled
	 *  - the key server has decided to enable MACsec
	 *
	 * 0: No replay window, strict check (default)
	 * 1..2^32-1: number of packets that could be misordered
	 */
	u32 macsec_replay_window;

	/**
	 * macsec_port - MACsec port (in SCI)
	 *
	 * Port component of the SCI.
	 *
	 * Range: 1-65534 (default: 1)
	 */
	int macsec_port;

	/**
	 * mka_priority - Priority of MKA Actor
	 *
	 * Range: 0-255 (default: 255)
	 */
	int mka_priority;

	/**
	 * mka_ckn - MKA pre-shared CKN
	 */
#define MACSEC_CKN_MAX_LEN 32
	size_t mka_ckn_len;
	u8 mka_ckn[MACSEC_CKN_MAX_LEN];

	/**
	 * mka_cak - MKA pre-shared CAK
	 */
#define MACSEC_CAK_MAX_LEN 32
	size_t mka_cak_len;
	u8 mka_cak[MACSEC_CAK_MAX_LEN];

#define MKA_PSK_SET_CKN BIT(0)
#define MKA_PSK_SET_CAK BIT(1)
#define MKA_PSK_SET (MKA_PSK_SET_CKN | MKA_PSK_SET_CAK)
	/**
	 * mka_psk_set - Whether mka_ckn and mka_cak are set
	 */
	u8 mka_psk_set;
#endif /* CONFIG_MACSEC */

	/* Soft Block ACL */
	int soft_block_acl_enable;
	unsigned soft_block_acl_wait_time;
	unsigned soft_block_acl_allow_time;
	int soft_block_acl_on_probe_req;
	int soft_block_acl_on_auth_req;

	int rnr_auto_update;
	int wpa_ie_required_for_mbss;
};

typedef struct acs_chan {
	int primary;
	int secondary;
	int width;
} acs_chan;

typedef struct neigh_ap_info_field {
   u8 NeighApTbttOffset;
   u8 bssid[6];
   u8 ShortSsid[4];
   u8 Ssid[32];
   u8 BssParams;
   u8 psd_20MHz;
} neigh_ap_info_field_t;

#define MAX_SUPPORTED_6GHZ_VAP_PER_RADIO 16
typedef struct colocated_6g_ap_info {
   u8 TbttInfoFieldType;
   u8 Reserved;
   u8 TbttInfoCount;
   u8 TbttInfoLen;
   u8 OperatingClass;
   u8 ChanNum;
   neigh_ap_info_field_t bss_info[MAX_SUPPORTED_6GHZ_VAP_PER_RADIO];
} colocated_6g_ap_info_t;

#define ACS_NUM_PENALTY_FACTORS      4
#define ACS_PEN_MIN                  0
#define ACS_PEN_MAX               1000
#define ACS_PEN_DEFAULT            100
#define ACS_PEN_DEFAULT_WBSS        50
#define ACS_NUM_DEGRADATION_FACTORS  3
#define ACS_NUM_GRP_PRIORITIES       4
#define ACS_MAX_CHANNELS           197
#define ACS_SWITCH_THRESHOLD       100
#define ACS_MIN_NUM_SCANS            1
#define ACS_MAX_NUM_SCANS          100
#define COC_AUTO_CONFIG 14
#define ERP_SET_SIZE 10
#define RTS_MODE_SIZE 2
#define N_PROTECTION_MIN 0
#define N_PROTECTION_MAX 4
#define FIXED_RATE_CONFIG_SIZE 11
#define CCA_ADAPT_SIZE 7
#define DYNAMIC_MU_TYPE_SIZE 5
#define GI_ILLEGAL_VALUE (-1)
#define DTIM_PERIOD_MIN 1
#define DTIM_PERIOD_MAX 255
#define BSS_LOAD_PERIOD_MIN 0
#define BSS_LOAD_PERIOD_MAX 100
#define RTS_THRESHOLD_MIN (-1)
#define RTS_THRESHOLD_MAX 65535
#define FRAGM_THRESHOLD_MIN 256
#define FRAGM_THRESHOLD_MAX 2346
#define BEACON_RATE_HT_MCS_MIN 0
#define BEACON_RATE_HT_MCS_MAX 31
#define BEACON_RATE_VHT_MCS_MIN 0
#define BEACON_RATE_VHT_MCS_MAX 9
#define BEACON_RATE_LEGACY_MCS_MIN 10
#define BEACON_RATE_LEGACY_MCS_MAX 10000
#define OBSS_BEACON_RSSI_TH_MIN (-100)
#define OBSS_BEACON_RSSI_TH_MAX 0
#define WPS_STATE_MIN 0
#define WPS_STATE_MAX 2
#define TIME_ZONE_MIN 4
#define TIME_ZONE_MAX 255
#define SUPPORTED_HS20_VERSION_MIN 1
#define SUPPORTED_HS20_VERSION_MAX ((HS20_VERSION >> 4) + 1)
#define SUBBAND_DFS_MIN 0
#define SUBBAND_DFS_MAX 60
#define DFS_DEBUG_CHAN_MIN 32
#define DFS_DEBUG_CHAN_MAX 196
#define BEACON_INTERVAL_MIN 10
#define BEACON_INTERVAL_MAX UINT16_MAX
#define MULTI_AP_MIN 0
#define MULTI_AP_MAX 3
#define MULTI_AP_PROFILE_MIN 1
#define MULTI_AP_PROFILE_MAX 2
#define NUM_VRT_BKH_NETDEVS_MIN 0
#define NUM_VRT_BKH_NETDEVS_MAX 8
#define ACCESS_NETWORK_TYPE_MIN 0
#define ACCESS_NETWORK_TYPE_MAX 15
#define FIRST_NON_DFS_CHAN	(UINT8_MAX)

enum multibss_aid_offset {
	NON_MULTIBSS_AID_OFFSET = 0,
	MULTIBSS_AID_OFFSET = 64
};

/**
 * struct hostapd_config - Per-radio interface configuration
 */
struct hostapd_config {
	struct hostapd_bss_config **bss, *last_bss;
	size_t num_bss;

	u16 beacon_int;
	u8  dfs_debug_chan;
	u8  dfs_dbg_need_cac;
	int rts_threshold;
	int fragm_threshold;
	u8 op_class;
	u8 channel;
	int enable_edmg;
	u8 edmg_channel;
	u8 acs;
	int ap_max_num_sta;  /*maximum number of stations per-radio */
	struct wpa_freq_range_list acs_ch_list;
	struct wpa_freq_range_list acs_freq_list;
	u8 acs_freq_list_present;
	int acs_exclude_dfs;
	enum hostapd_hw_mode hw_mode; /* HOSTAPD_MODE_IEEE80211A, .. */
	enum {
		LONG_PREAMBLE = 0,
		SHORT_PREAMBLE = 1
	} preamble;

	int *supported_rates;
	int *basic_rates;
	unsigned int beacon_rate;
	enum beacon_rate_type rate_type;

	const struct wpa_driver_ops *driver;
	char *driver_params;

	int ap_table_max_size;
	int ap_table_expiration_time;

	unsigned int track_sta_max_num;
	unsigned int track_sta_max_age;

	char country[3]; /* first two octets: country code as described in
			  * ISO/IEC 3166-1. Third octet:
			  * ' ' (ascii 32): all environments
			  * 'O': Outdoor environemnt only
			  * 'I': Indoor environment only
			  * 'X': Used with noncountry entity ("XXX")
			  * 0x00..0x31: identifying IEEE 802.11 standard
			  *	Annex E table (0x04 = global table)
			  */

	int dynamic_country; /* Country and channel list will updated run-time
			      * after receiving regulatory changed event
			      * regardless of the initiator and regardless
			      * if hostapd is waiting for channel list update
			      * And country specified in conf file will be
			      * set to kernel only in case non-valid ocuntry
			      * have been already set */

	int ieee80211d;

	int ieee80211h; /* DFS */
	int sub_band_dfs; /* Sub Band DFS */

	/*
	 * Local power constraint is an octet encoded as an unsigned integer in
	 * units of decibels. Invalid value -1 indicates that Power Constraint
	 * element will not be added.
	 */
	int local_pwr_constraint;

	/* Control Spectrum Management bit */
	int spectrum_mgmt_required;

	struct hostapd_tx_queue_params tx_queue[NUM_TX_QUEUES];

	/*
	 * WMM AC parameters, in same order as 802.1D, i.e.
	 * 0 = BE (best effort)
	 * 1 = BK (background)
	 * 2 = VI (video)
	 * 3 = VO (voice)
	 */
	struct hostapd_wmm_ac_params wmm_ac_params[4];

	int ht_op_mode_fixed;
	int ht_tx_bf_capab_from_hw;
	u16 ht_capab;
	u32 ht_tx_bf_capab;
	u8  ht_cap_ampdu_parameters;
	u8  ht_cap_ampdu_parameters_override_mask;
	u32 ht_mcs_set[4];

	int ieee80211n;
	int ieee80211n_acax_compat;
	int secondary_channel;
	bool secondary_channel_set;
	int no_pri_sec_switch;
	int ht_rifs;
	int require_ht;
	int obss_interval;
	u16 scan_passive_dwell;
	u16 scan_active_dwell;
	u16 scan_passive_total_per_channel;
	u16 scan_active_total_per_channel;
	u16 channel_transition_delay_factor;
	u16 scan_activity_threshold;
	u32 allow_scan_during_cac;
	int obss_beacon_rssi_threshold;
	int ignore_40_mhz_intolerant;
	int acs_scan_mode;
	int ScanRssiTh;
	int acs_scan_no_flush;
	u32 vht_capab;
	int ieee80211ac;
	int orig_ieee80211ac;
	int require_vht;
	u8 vht_oper_chwidth;
	u8 vht_oper_centr_freq_seg0_idx;
	u8 vht_oper_centr_freq_seg1_idx;
	u32 vht_mcs_set[2];

	u8 ht40_plus_minus_allowed;
        int assoc_rsp_rx_mcs_mask;
	int require_he;
	u8 multibss_enable;
	u8 mbssid_aid_offset;
	u8 ext_cap_max_num_msdus_in_amsdu;	/* Extended Capabilities: Max Number of MSDUs in A-MSDU */

	/* Use driver-generated interface addresses when adding multiple BSSs */
	u8 use_driver_iface_addr;
#ifdef CONFIG_IEEE80211AX
	u8 he_spatial_reuse_ie_present_in_beacon;
	u8 he_spatial_reuse_ie_present_in_assoc_response;
	u8 he_spatial_reuse_ie_present_in_probe_response;
	u8 he_nfr_buffer_threshold;
	u8 he_mu_edca_ie_present;
	struct ieee80211_he_capabilities he_capab;
	struct ieee80211_he_operation he_oper;
	struct ieee80211_he_mu_edca_parameter_set he_mu_edca;
	struct ieee80211_he_spatial_reuse_parameter_set he_spatial_reuse;
	struct intel_vendor_he_capa he_non_advertised_caps;
	/* Override non-advertised HE caps */
	u8 he_cap_non_adv_multi_bss_rx_control_support;
	u8 he_cap_non_adv_multi_bss_rx_control_support_override;
	u8 he_cap_non_adv_punc_pream_rx_support;
	u8 he_cap_punc_pream_rx_support_override;

	/* Override driver reported HE Capabilities
	 * by ones defined in hostapd configuration file.
	 */
	u8 enable_he_debug_mode;
	struct he_override_hw_capab override_hw_capab;
	u8 he_oper_chwidth;
	u8 he_oper_centr_freq_seg0_idx;
	u8 he_oper_centr_freq_seg1_idx;
#endif /* CONFIG_IEEE80211AX */

#ifdef CONFIG_FST
	struct fst_iface_cfg fst_cfg;
#endif /* CONFIG_FST */

#ifdef CONFIG_P2P
	u8 p2p_go_ctwindow;
#endif /* CONFIG_P2P */

#ifdef CONFIG_TESTING_OPTIONS
	double ignore_probe_probability;
	double ignore_auth_probability;
	double ignore_assoc_probability;
	double ignore_reassoc_probability;
	double corrupt_gtk_rekey_mic_probability;
	int ecsa_ie_only;
#endif /* CONFIG_TESTING_OPTIONS */

	enum {
		ACS_ALGO_SURVEY = 0,
		ACS_ALGO_SMART  = 1,
	} acs_algo;
#ifdef CONFIG_ACS
	int acs_init_done;
	unsigned int acs_num_scans;

	/* SmartACS */
	char *acs_smart_info_file;
	char *acs_history_file;
	struct acs_chan acs_fallback_chan;
	int *acs_penalty_factors;
	int acs_chan_cust_penalty[ACS_MAX_CHANNELS];
	int *acs_to_degradation;
	int *acs_grp_priorities_throughput; // [ACS_NUM_GRP_PRIORITIES];
	int *acs_grp_priorities_reach; // [ACS_NUM_GRP_PRIORITIES];
	int acs_grp_prio_reach_map[ACS_NUM_GRP_PRIORITIES];
	int acs_grp_prio_tp_map[ACS_NUM_GRP_PRIORITIES];
	int acs_policy;
	int acs_switch_thresh;
	int acs_bw_comparison;
	int acs_use24overlapped;
	int acs_bg_scan_do_switch;
	int acs_update_do_switch;

	struct acs_bias {
		int channel;
		double bias;
	} *acs_chan_bias;
	unsigned int num_acs_chan_bias;
#endif /* CONFIG_ACS */
	int sPowerSelection; /* tx power: 12%:9 25%:6 50%:3 75%:1 100%:0 */

	int *sCoCPower;
	int sCoCPowerSize;
	int *sCoCAutoCfg;
	int *sErpSet;
	int sRadarRssiTh;
	int sRTSmode[RTS_MODE_SIZE];
	int *sFixedRateCfg;
	int sInterferDetThresh;
	int sCcaAdapt[CCA_ADAPT_SIZE];
	int *sFWRecovery;
	int sFWRecoverySize;
	int sStationsStat;
	int sQAMplus;
	int sConfigMRCoex;
	int sConfigMRCoexActiveTime;
	int sConfigMRCoexInactiveTime;
	int sConfigMRCoexCts2SelfActive;
	int sBfMode;
	int sProbeReqCltMode;
	int sMaxMpduLen;

	struct wpabuf *lci;
	struct wpabuf *civic;
	int stationary_ap;

	int ieee80211ax;
	int sDynamicMuTypeDownLink;
	int sDynamicMuTypeUpLink;
	int sDynamicMuMinStationsInGroup;
	int sDynamicMuMaxStationsInGroup;
	int sDynamicMuCdbConfig;
	int sFixedLtfGi;
#define AP_TX_EXCE_RETRY_LIMIT_DEFAULT  50
#define AP_TX_RETRY_LIMIT_MAX  15
#define AP_TX_RETRY_LIMIT_DEFAULT  4
#define AP_TX_RETRY_LIMIT_DEFAULT_DATA  7
	u8 ap_retry_limit;
	u8 ap_retry_limit_data;

	/* VHT enable/disable config from CHAN_SWITCH */
#define CH_SWITCH_VHT_ENABLED BIT(0)
#define CH_SWITCH_VHT_DISABLED BIT(1)
	unsigned int ch_switch_vht_config;
	char *atf_config_file;		/* Air Time Fairness configuration filename */
	struct atf_config atf_cfg; /* Air Time Fairness configuration */

	int testbed_mode;

	/* used when reducing channel width, when no usable DFS channels left */
	int cw_reduced;
	int secondary_channel_bak;
	u8 vht_oper_chwidth_bak;

	u8 owl; /* Forwarding incoming probe reqests to upper layers */
	int rssi_reject_assoc_rssi;
	int rssi_reject_assoc_timeout;

#ifdef CONFIG_AIRTIME_POLICY
	enum {
		AIRTIME_MODE_OFF = 0,
		AIRTIME_MODE_STATIC = 1,
		AIRTIME_MODE_DYNAMIC = 2,
		AIRTIME_MODE_LIMIT = 3,
		__AIRTIME_MODE_MAX,
	} airtime_mode;
	unsigned int airtime_update_interval;
#define AIRTIME_MODE_MAX (__AIRTIME_MODE_MAX - 1)
#endif /* CONFIG_AIRTIME_POLICY */

	int cred_req_timeout;
	u8 notify_action_frame; /* Forwarding incoming action frames to upper layers */

	char *dfs_channels_state_file;
#define MAX_SUPPORTED_6GHZ_RADIO 2
	colocated_6g_ap_info_t coloc_6g_ap_info[MAX_SUPPORTED_6GHZ_RADIO]; /* Max number of 6GHz radios */

	int he_beacon;

	int unsolicited_frame_support;
	int unsolicited_frame_duration;
	int tx_mcs_set_defined;
	int ignore_supported_channels_errors;

	int dynamic_failsafe;
#define DEFAULT_MU_OPERATION 1
	int sMuOperation;

	int multi_ap_profile;
	u16 multi_ap_primary_vlanid;

	int override_6g_mbssid_default_mode;
	char start_after[IFNAMSIZ + 1];
	int start_after_delay;
	int start_after_watchdog_time;
#define DEFAULT_WHM      (0)
#define WHM_ENABLE       (0)
#define WHM_HOSTAP_WARN  (1)
#define WHM_DRV_WARN     (2)
#define WHM_FW_WARN      (3)
#define WHM_PHY_WARN     (4)
	char *whm_config_file;      /* WHM configuration filename */
	int whm_cfg_flag;           /* WHM configuration bitfields */

	unsigned adv_proto_query_resp_len;    /* Advertisement protocol IE */

	u32 event_cache_interval;
};

/* scan max and min params refer IEEE 802.11-2016, C.3 MIB detail*/
#define SCAN_PASSIVE_DWELL_DEFAULT		20
#define SCAN_PASSIVE_DWELL_MIN			5
#define SCAN_PASSIVE_DWELL_MAX			1000
#define SCAN_ACTIVE_DWELL_MIN			10
#define SCAN_ACTIVE_DWELL_MAX			1000
#define SCAN_PASSIVE_TOTAL_PER_CHANNEL_MIN	200
#define SCAN_PASSIVE_TOTAL_PER_CHANNEL_MAX	10000
#define SCAN_ACTIVE_TOTAL_PER_CHANNEL_MIN	20
#define SCAN_ACTIVE_TOTAL_PER_CHANNEL_MAX	10000
#define CHANNEL_TRANSITION_DELAY_FACTOR_MIN	5
#define CHANNEL_TRANSITION_DELAY_FACTOR_MAX	100
#define SCAN_ACTIVITY_THRESHOLD_DEFAULT		25
#define SCAN_ACTIVITY_THRESHOLD_MIN		0
#define SCAN_ACTIVITY_THRESHOLD_MAX		100
#define OBSS_INTERVAL_MIN			10
#define OBSS_INTERVAL_MAX			900
#define SCAN_RSSI_THRESHOLD_DEFAULT		-86

/* country 3 */
#define GLOBAL_OPERATING_CLASSES_MIN		0x01
#define GLOBAL_OPERATING_CLASSES_MAX		0x1f
#define ALL_ENVIRONMENTS			0x20
#define OUTDOOR_ENVIRONMENT_ONLY		0x4f
#define INDOOR_ENVIRONMENT_ONLY			0x49
#define NONCOUNTRY_ENTITY			0x58

/* hostapd.conf values */
#define RADIUS_ACCT_INTERIM_INTERVAL_MIN	60
#define RADIUS_ACCT_INTERIM_INTERVAL_MAX	INT32_MAX
#define FT_R0_KEY_LIFETIME_MIN			60
#define FT_R0_KEY_LIFETIME_MAX			UINT32_MAX
#define R0_KEY_LIFETIME_MIN			(FT_R0_KEY_LIFETIME_MIN / 60)
#define R0_KEY_LIFETIME_MAX			(FT_R0_KEY_LIFETIME_MAX / 60)
#define REASSOCIATION_DEADLINE_MIN		1000
#define REASSOCIATION_DEADLINE_MAX		65535
#define RSSI_REJECT_ASSOC_TIMEOUT_MIN		0
#define RSSI_REJECT_ASSOC_TIMEOUT_MAX		255
#define RSSI_REJECT_ASSOC_RSSI_MIN		-90
#define RSSI_REJECT_ASSOC_RSSI_MAX		-60
#define FILS_DH_GROUP_MIN			0
#define FILS_DH_GROUP_MAX			65535
#define LOCAL_PWR_CONSTRAINT_MIN		0
#define LOCAL_PWR_CONSTRAINT_MAX		255

#define GAS_ADDRESS3_MIN			0
#define GAS_ADDRESS3_MAX			2

#define WPS_CRED_PROCESS			0	/* process received credentials internally (default) */
#define WPS_CRED_PASS				1	/* do not process received credentials; just pass them over ctrl_iface to external program(s)*/
#define WPS_CRED_PROCESS_AND_PASS		2	/* process received credentials internally and pass them over ctrl_iface to external program*/

#define AP_STA_NO_CELLULAR_DATA_CONN		0	/* AP does not want STA to use the cellular data connection */
#define AP_STA_PREFERS_NO_CELLULAR_DATA_CONN	1	/* AP prefers the STA not to use cellular data connection */
#define AP_STA_PREFERS_CELLULAR_DATA_CONN	255	/* AP prefers the STA to use cellular data connection */

#define IGNORE_BROADCAST_SSID_DISABLED		0	/* disabled */
#define IGNORE_BROADCAST_SSID_EMPTY_SSID	1	/* send empty (length=0) SSID in beacon and ignore probe request for broadcast SSID */
#define IGNORE_BROADCAST_SSID_CLEAR_SSID	2	/*clear SSID (ASCII 0), but keep the original length (this may be required with some clients 
							  that do not support empty SSID) and ignore probe requests for broadcast SSID*/
#define TIME_ADVERTISEMENT_DISABLED		0
#define TIME_ADVERTISEMENT_ENABLED		2
#define MODEL_DESCRIPTION_MAX_LEN		128
#define FRIENDLY_NAME_MAX_LEN			64
#define UNIVERSAL_PRODUCT_CODE_MAX_DIGIT	12

#define VLANID_MIN				0
#define VLANID_MAX				4095
#define AP_MAX_INACTIVITY_MIN			0
#define AP_MAX_INACTIVITY_MAX			86400
#define RKH_PULL_TIMEOUT_MIN			100
#define RKH_PULL_TIMEOUT_MAX			10000
#define RKH_PULL_RETRIES_MIN			1
#define RKH_PULL_RETRIES_MAX			10
#define GAS_COMEBACK_DELAY_MIN			1
#define GAS_COMEBACK_DELAY_MAX			5000
#define HS20_DEAUTH_REQ_TIMEOUT_MIN		1
#define HS20_DEAUTH_REQ_TIMEOUT_MAX		3600
#define SUBSCR_REMEDIATION_METHOD_MIN		0
#define SUBSCR_REMEDIATION_METHOD_MAX		255
#define FILS_HLP_WAIT_TIME_MIN			15
#define FILS_HLP_WAIT_TIME_MAX			100
#define AP_TIMER_DISABLE			0
#define AP_TIMER_MIN				1
#define AP_TIMER_MAX				86400
#define AP_PORT_MIN				1
#define AP_PORT_MAX				65535
#define TRACK_STA_MAX_AGE_MIN			0
#define TRACK_STA_MAX_AGE_MAX			360
#define TRACK_STA_MAX_NUM_MIN			0
#define TRACK_STA_MAX_NUM_MAX			256
#define AP_TABLE_EXPIRATION_TIME_MIN		600
#define AP_TABLE_EXPIRATION_TIME_MAX		86400
#define AP_MAX_VALID_CHAR			255
#define INTERFERENCE_DET_THRESH_MIN		-128
#define INTERFERENCE_DET_THRESH_MAX		1
#define INTERFERENCE_DET_THRESH_DEFAULT		-68
#define COEX_MODE_MIN 0
#define COEX_MODE_MAX 2
#define COEX_ACTIVE_TIME_MIN 2
#define COEX_ACTIVE_TIME_MAX 0x1fffff
#define COEX_INACTIVE_TIME_MIN 2
#define COEX_INACTIVE_TIME_MAX 32

static inline u8 hostapd_get_oper_chwidth(struct hostapd_config *conf)
{
#ifdef CONFIG_IEEE80211AX
        if (conf->ieee80211ax)
                return conf->he_oper_chwidth;
#endif /* CONFIG_IEEE80211AX */
        return conf->vht_oper_chwidth;
}

static inline void
hostapd_set_oper_chwidth(struct hostapd_config *conf, u8 oper_chwidth)
{
#ifdef CONFIG_IEEE80211AX
        if (conf->ieee80211ax)
                conf->he_oper_chwidth = oper_chwidth;
#endif /* CONFIG_IEEE80211AX */
        conf->vht_oper_chwidth = oper_chwidth;
}

static inline u8
hostapd_get_oper_centr_freq_seg0_idx(struct hostapd_config *conf)
{
#ifdef CONFIG_IEEE80211AX
        if (conf->ieee80211ax)
                return conf->he_oper_centr_freq_seg0_idx;
#endif /* CONFIG_IEEE80211AX */
        return conf->vht_oper_centr_freq_seg0_idx;
}

static inline void
hostapd_set_oper_centr_freq_seg0_idx(struct hostapd_config *conf,
                                     u8 oper_centr_freq_seg0_idx)
{
#ifdef CONFIG_IEEE80211AX
        if (conf->ieee80211ax)
                conf->he_oper_centr_freq_seg0_idx = oper_centr_freq_seg0_idx;
#endif /* CONFIG_IEEE80211AX */
        conf->vht_oper_centr_freq_seg0_idx = oper_centr_freq_seg0_idx;
}
static inline u8
hostapd_get_oper_centr_freq_seg1_idx(struct hostapd_config *conf)
{
#ifdef CONFIG_IEEE80211AX
        if (conf->ieee80211ax)
                return conf->he_oper_centr_freq_seg1_idx;
#endif /* CONFIG_IEEE80211AX */
        return conf->vht_oper_centr_freq_seg1_idx;
}

static inline void
hostapd_set_oper_centr_freq_seg1_idx(struct hostapd_config *conf,
                                     u8 oper_centr_freq_seg1_idx)
{
#ifdef CONFIG_IEEE80211AX
        if (conf->ieee80211ax)
                conf->he_oper_centr_freq_seg1_idx = oper_centr_freq_seg1_idx;
#endif /* CONFIG_IEEE80211AX */
        conf->vht_oper_centr_freq_seg1_idx = oper_centr_freq_seg1_idx;
}

/* Soft Block */
#define SOFT_BLOCK_WAIT_TIME_MIN		0
#define SOFT_BLOCK_WAIT_TIME_MAX		100000u
#define SOFT_BLOCK_ALLOW_TIME_MIN		0
#define SOFT_BLOCK_ALLOW_TIME_MAX		100000u
#define SOFT_BLOCK_ON_AUTH_REQ_REJECT_ALL	0
#define SOFT_BLOCK_ON_AUTH_REQ_STATUS_CODE_MIN	SOFT_BLOCK_ON_AUTH_REQ_REJECT_ALL
#define SOFT_BLOCK_ON_AUTH_REQ_STATUS_CODE_MAX	WLAN_STATUS_SAE_HASH_TO_ELEMENT
#define SOFT_BLOCK_ON_PROBE_REQ_REJECT_ALL	0
#define SOFT_BLOCK_ON_PROBE_REQ_REJECT_BLOCKED	1
#define SOFT_BLOCK_ON_PROBE_REQ_REPLY_ALL	2
#define SOFT_BLOCK_ON_PROBE_REQ_MIN		SOFT_BLOCK_ON_PROBE_REQ_REJECT_ALL
#define SOFT_BLOCK_ON_PROBE_REQ_MAX		SOFT_BLOCK_ON_PROBE_REQ_REPLY_ALL

#define SOFT_BLOCK_ENABLE_DEFAULT			0
#define SOFT_BLOCK_WAIT_TIME_DEFAULT			10000u
#define SOFT_BLOCK_ALLOW_TIME_DEFAULT			15000u
#define SOFT_BLOCK_ON_AUTH_REQ_STATUS_CODE_DEFAULT	WLAN_STATUS_UNSPECIFIED_FAILURE
#define SOFT_BLOCK_ON_PROBE_REQ_DEFAULT			SOFT_BLOCK_ON_PROBE_REQ_REJECT_BLOCKED

#define MAX_EVENT_CACHE_INTERVAL		3600

#define IFACE_START_DELAY_MIN 1
#define IFACE_START_DELAY_MAX 600

/* Advertisement protocol IE*/
#define ADV_PROTO_QUERY_RESP_LEN_MIN		1u
#define ADV_PROTO_QUERY_RESP_LEN_MAX		0x7fu
#define ADV_PROTO_QUERY_RESP_LEN_DEFAULT	0x7fu
#define ADV_PROTO_QUERY_RESP_LEN_MASK		ADV_PROTO_QUERY_RESP_LEN_MAX

int hostapd_mac_comp(const void *a, const void *b);
struct hostapd_config * hostapd_config_defaults(void);
void hostapd_config_defaults_bss(struct hostapd_bss_config *bss);
void hostapd_config_free_radius_attr(struct hostapd_radius_attr *attr);
void hostapd_config_free_eap_user(struct hostapd_eap_user *user);
void hostapd_config_free_eap_users(struct hostapd_eap_user *user);
void hostapd_config_clear_wpa_psk(struct hostapd_wpa_psk **p);
void hostapd_config_free_bss(struct hostapd_bss_config *conf);
void hostapd_config_free(struct hostapd_config *conf);

const char* hostapd_config_get_mesh_mode_str(mesh_mode_t mesh_mode);
mesh_mode_t hostapd_config_parse_mesh_mode(const char *str);

int hostapd_maclist_found(struct mac_acl_entry *list, int num_entries,
			  const u8 *addr, struct vlan_description *vlan_id);
int hostapd_rate_found(int *list, int rate);
const u8 * hostapd_get_psk(const struct hostapd_bss_config *conf,
			   const u8 *addr, const u8 *p2p_dev_addr,
			   const u8 *prev_psk, int *vlan_id);
int hostapd_setup_wpa_psk(struct hostapd_bss_config *conf);
int hostapd_vlan_valid(struct hostapd_vlan *vlan,
		       struct vlan_description *vlan_desc);
const char * hostapd_get_vlan_id_ifname(struct hostapd_vlan *vlan,
					int vlan_id);
struct hostapd_radius_attr *
hostapd_config_get_radius_attr(struct hostapd_radius_attr *attr, u8 type);
struct hostapd_radius_attr * hostapd_parse_radius_attr(const char *value);
int hostapd_config_check(struct hostapd_config *conf, int full_config);
void hostapd_set_security_params(struct hostapd_bss_config *bss,
				 int full_config);
int hostapd_sae_pw_id_in_use(struct hostapd_bss_config *conf);
bool hostapd_sae_pk_in_use(struct hostapd_bss_config *conf);
bool hostapd_sae_pk_exclusively(struct hostapd_bss_config *conf);
int hostapd_setup_sae_pt(struct hostapd_bss_config *conf);

struct soft_block_acl_entry* hostapd_soft_block_maclist_entry_get(struct soft_block_acl_db *acl, const u8 *addr);
int hostapd_soft_block_maclist_found(struct soft_block_acl_db *acl,  const u8 *addr);
void hostapd_soft_block_acl_clear_list(struct soft_block_acl_db *acl);

#endif /* HOSTAPD_CONFIG_H */
