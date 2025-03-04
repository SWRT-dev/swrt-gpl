/*
 * hostapd / Initialization and configuration
 * Copyright (c) 2002-2014, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef HOSTAPD_H
#define HOSTAPD_H

#ifdef CONFIG_SQLITE
#include <sqlite3.h>
#endif /* CONFIG_SQLITE */

#include "common/defs.h"
#include "utils/list.h"
#include "ap_config.h"
#include "drivers/driver.h"

#define OCE_STA_CFON_ENABLED(hapd) \
	((hapd->conf->oce & OCE_STA_CFON) && \
	 (hapd->iface->drv_flags & WPA_DRIVER_FLAGS_OCE_STA_CFON))
#define OCE_AP_ENABLED(hapd) \
	((hapd->conf->oce & OCE_AP) && \
	 (hapd->iface->drv_flags & WPA_DRIVER_FLAGS_OCE_AP))

#define LTQ_FLUSH_RADIO (-1)

#define DEFAULT_CS_COUNT_NON_ETSI 2
#define DEFAULT_CS_COUNT 5

#define MIN_NUM_BSS_IN_NON_MBSSID_SET	2 /* Non MultiBSS : Master(Dummy VAP) + one or more operational VAPs */
#define MIN_NUM_BSS_IN_MBSSID_SET		3 /* MultiBSS: Master(Dummy VAP) + one TX VAP + one or more Non TX VAPs */
#define NUM_NON_TX_VAPS(num_bss) (num_bss - MIN_NUM_BSS_IN_NON_MBSSID_SET)

#define MAX_EVENT_MSG_LEN    512
#define MAX_CACHED_EVENT_COUNT 128

typedef enum {
    GUARD_INTERVAL_400,
    GUARD_INTERVAL_800,
    GUARD_INTERVAL_1600,
    GUARD_INTERVAL_3200,
    GUARD_INTERVAL_AUTO,
} GUARD_INTERVAL_t;

typedef enum {
	GI_CP_MODE_SHORT_CP_SHORT_LTF = 0,
	GI_CP_MODE_MED_CP_SHORT_LTF = 1,
	GI_CP_MODE_SHORT_CP_MED_LTF = 2,
	GI_CP_MODE_MED_CP_MED_LTF = 3,
	GI_CP_MODE_LONG_CP_LONG_LTF = 5,
} GI_CP_MODE_t;

struct wpa_ctrl_dst;
struct radius_server_data;
struct upnp_wps_device_sm;
struct hostapd_data;
struct sta_info;
struct ieee80211_ht_capabilities;
struct full_dynamic_vlan;
enum wps_event;
union wps_event_data;
#ifdef CONFIG_MESH
struct mesh_conf;
#endif /* CONFIG_MESH */

/* STAs free reason - Recovery */
#define WLAN_STA_FREE_REASON_RECOVERY (-1)
#ifdef CONFIG_CTRL_IFACE_UDP
#define CTRL_IFACE_COOKIE_LEN 8
#endif /* CONFIG_CTRL_IFACE_UDP */

struct hostapd_iface;

struct hapd_interfaces {
	int (*reload_config)(struct hostapd_iface *iface);
	struct hostapd_config * (*config_read_cb)(const char *config_fname);
	int (*ctrl_iface_init)(struct hostapd_data *hapd);
	void (*ctrl_iface_deinit)(struct hostapd_data *hapd);
	int (*for_each_interface)(struct hapd_interfaces *interfaces,
				  int (*cb)(struct hostapd_iface *iface,
					    void *ctx), void *ctx);
	int (*driver_init)(struct hostapd_iface *iface);

	size_t count;
	int global_ctrl_sock;
	struct dl_list global_ctrl_dst;
	char *global_iface_path;
	char *global_iface_name;
#ifndef CONFIG_NATIVE_WINDOWS
	gid_t ctrl_iface_group;
#endif /* CONFIG_NATIVE_WINDOWS */
	struct hostapd_iface **iface;

	size_t terminate_on_error;
#ifndef CONFIG_NO_VLAN
	struct dynamic_iface *vlan_priv;
#endif /* CONFIG_NO_VLAN */
#ifdef CONFIG_ETH_P_OUI
	struct dl_list eth_p_oui; /* OUI Extended EtherType handlers */
#endif /* CONFIG_ETH_P_OUI */
	int eloop_initialized;

#ifdef CONFIG_DPP
	struct dpp_global *dpp;
#endif /* CONFIG_DPP */

#ifdef CONFIG_AIRTIME_POLICY
	unsigned int num_backlogged_sta;
	unsigned int airtime_weight;
#endif /* CONFIG_AIRTIME_POLICY */

#ifdef CONFIG_SQLITE
	sqlite3 *rad_attr_db;
#endif /* CONFIG_SQLITE */
};

/*16 - (3 digit) - (one underscore) - (one for \0) - (one character b) = 10*/
#define TRUNCATE_BKH_VRT_IFACE 10
struct backhaul_virtual_iface {
	char ifname[IFNAMSIZ];
	int is_up;
	int added_to_bridge;
#ifdef CONFIG_CTRL_IFACE_UDP
       unsigned char ctrl_iface_cookie[CTRL_IFACE_COOKIE_LEN];
#endif /* CONFIG_CTRL_IFACE_UDP */

};

enum hostapd_chan_status {
	HOSTAPD_CHAN_VALID = 0, /* channel is ready */
	HOSTAPD_CHAN_INVALID = 1, /* no usable channel found */
	HOSTAPD_CHAN_ACS = 2, /* ACS work being performed */
};

struct hostapd_probereq_cb {
	int (*cb)(void *ctx, const u8 *sa, const u8 *da, const u8 *bssid,
		  const u8 *ie, size_t ie_len, int ssi_signal);
	void *ctx;
};

#define HOSTAPD_RATE_BASIC 0x00000001

struct hostapd_rate_data {
	int rate; /* rate in 100 kbps */
	int flags; /* HOSTAPD_RATE_ flags */
};

struct hostapd_frame_info {
	unsigned int freq;
	u32 channel;
	u32 datarate;
	int ssi_signal; /* dBm */
	int snr_db;  
};

enum wps_status {
	WPS_STATUS_SUCCESS = 1,
	WPS_STATUS_FAILURE
};

enum pbc_status {
	WPS_PBC_STATUS_DISABLE,
	WPS_PBC_STATUS_ACTIVE,
	WPS_PBC_STATUS_TIMEOUT,
	WPS_PBC_STATUS_OVERLAP
};

struct wps_stat {
	enum wps_status status;
	enum wps_error_indication failure_reason;
	enum pbc_status pbc_status;
	u8 peer_addr[ETH_ALEN];
};

struct hostapd_neighbor_entry {
	struct dl_list list;
	u8 bssid[ETH_ALEN];
	struct wpa_ssid_value ssid;
	struct wpabuf *nr;
	struct wpabuf *lci;
	struct wpabuf *civic;
	/* LCI update time */
	struct os_time lci_date;
	int stationary;
};

struct hostapd_sae_commit_queue {
	struct dl_list list;
	int rssi;
	int snr_db;
	size_t len;
	u8 msg[];
};

struct event_info {
	struct event_info *next; /* next entry in event list */
	struct event_info *hnext; /* next entry in hash table list */
	u8 addr[6];
	char msg_string[MAX_EVENT_MSG_LEN];
};

/**
 * struct hostapd_data - hostapd per-BSS data structure
 */
struct hostapd_data {
	struct hostapd_iface *iface;
	struct hostapd_config *iconf;
	struct hostapd_bss_config *conf;
	int interface_added; /* virtual interface added for this BSS */
	unsigned int started:1;
	unsigned int disabled:1;
	unsigned int reenable_beacon:1;

	u8 own_addr[ETH_ALEN];

	int num_sta; /* number of entries in sta_list */
	struct sta_info *sta_list; /* STA info list head */
#define STA_HASH_SIZE 256
#define STA_HASH(sta) (sta[5])
	struct sta_info *sta_hash[STA_HASH_SIZE];

	struct event_info *event_list; /* event info list head */
	struct event_info *event_hash[STA_HASH_SIZE];
	u32 cached_event_count;

	/*
	 * Bitfield for indicating which AIDs are allocated. Only AID values
	 * 1-2007 are used and as such, the bit at index 0 corresponds to AID
	 * 1.
	 */
#define AID_WORDS ((2008 + 31) / 32)
	u32 sta_aid[AID_WORDS];

	const struct wpa_driver_ops *driver;
	void *drv_priv;

	void (*new_assoc_sta_cb)(struct hostapd_data *hapd,
				 struct sta_info *sta, int reassoc);

	void *msg_ctx; /* ctx for wpa_msg() calls */
	void *msg_ctx_parent; /* parent interface ctx for wpa_msg() calls */

	struct radius_client_data *radius;
	u64 acct_session_id;
	struct radius_das_data *radius_das;

	struct hostapd_cached_radius_acl *acl_cache;
	struct hostapd_acl_query_data *acl_queries;

	struct wpa_authenticator *wpa_auth;
	struct eapol_authenticator *eapol_auth;
	struct eap_config *eap_cfg;

	struct rsn_preauth_interface *preauth_iface;
	struct os_reltime michael_mic_failure;
	int michael_mic_failures;
	int tkip_countermeasures;

	int ctrl_sock;
	struct dl_list ctrl_dst;

	void *ssl_ctx;
	void *eap_sim_db_priv;
	struct radius_server_data *radius_srv;
	struct dl_list erp_keys; /* struct eap_server_erp_key */

	int parameter_set_count;

	/* Time Advertisement */
	u8 time_update_counter;
	struct wpabuf *time_adv;

#ifdef CONFIG_FULL_DYNAMIC_VLAN
	struct full_dynamic_vlan *full_dynamic_vlan;
#endif /* CONFIG_FULL_DYNAMIC_VLAN */

	struct l2_packet_data *l2;

#ifdef CONFIG_IEEE80211R_AP
	struct dl_list l2_queue;
	struct dl_list l2_oui_queue;
	struct eth_p_oui_ctx *oui_pull;
	struct eth_p_oui_ctx *oui_resp;
	struct eth_p_oui_ctx *oui_push;
	struct eth_p_oui_ctx *oui_sreq;
	struct eth_p_oui_ctx *oui_sresp;
#endif /* CONFIG_IEEE80211R_AP */

	struct wps_context *wps;

	int beacon_set_done;
	struct wpabuf *wps_beacon_ie;
	struct wpabuf *wps_probe_resp_ie;
#ifdef CONFIG_WPS
	unsigned int ap_pin_failures;
	unsigned int ap_pin_failures_consecutive;
	struct upnp_wps_device_sm *wps_upnp;
	unsigned int ap_pin_lockout_time;

	struct wps_stat wps_stats;
#endif /* CONFIG_WPS */
	u8 last_1x_eapol_key_replay_counter[8];
#ifdef CONFIG_MACSEC
	struct ieee802_1x_kay *kay;
#endif /* CONFIG_MACSEC */

	struct hostapd_probereq_cb *probereq_cb;
	size_t num_probereq_cb;

	void (*public_action_cb)(void *ctx, const u8 *buf, size_t len,
				 int freq);
	void *public_action_cb_ctx;
	void (*public_action_cb2)(void *ctx, const u8 *buf, size_t len,
				  int freq);
	void *public_action_cb2_ctx;

	int (*vendor_action_cb)(void *ctx, const u8 *buf, size_t len,
				int freq);
	void *vendor_action_cb_ctx;

	void (*wps_reg_success_cb)(void *ctx, const u8 *mac_addr,
				   const u8 *uuid_e);
	void *wps_reg_success_cb_ctx;

	void (*wps_event_cb)(void *ctx, enum wps_event event,
			     union wps_event_data *data);
	void *wps_event_cb_ctx;

	void (*sta_authorized_cb)(void *ctx, const u8 *mac_addr,
				  int authorized, const u8 *p2p_dev_addr);
	void *sta_authorized_cb_ctx;

	void (*setup_complete_cb)(void *ctx);
	void *setup_complete_cb_ctx;

	void (*new_psk_cb)(void *ctx, const u8 *mac_addr,
			   const u8 *p2p_dev_addr, const u8 *psk,
			   size_t psk_len);
	void *new_psk_cb_ctx;

	/* channel switch parameters */
	struct hostapd_freq_params cs_freq_params;
	u8 cs_count;
	int cs_block_tx;
	unsigned int cs_c_off_beacon;
	unsigned int cs_c_off_proberesp;
	int csa_in_progress;
	unsigned int cs_c_off_ecsa_beacon;
	unsigned int cs_c_off_ecsa_proberesp;

#ifdef CONFIG_P2P
	struct p2p_data *p2p;
	struct p2p_group *p2p_group;
	struct wpabuf *p2p_beacon_ie;
	struct wpabuf *p2p_probe_resp_ie;

	/* Number of non-P2P association stations */
	int num_sta_no_p2p;

	/* Periodic NoA (used only when no non-P2P clients in the group) */
	int noa_enabled;
	int noa_start;
	int noa_duration;
#endif /* CONFIG_P2P */
#ifdef CONFIG_PROXYARP
	struct l2_packet_data *sock_dhcp;
	struct l2_packet_data *sock_ndisc;
#endif /* CONFIG_PROXYARP */
#ifdef CONFIG_MESH
	int num_plinks;
	int max_plinks;
	void (*mesh_sta_free_cb)(struct hostapd_data *hapd,
				 struct sta_info *sta);
	struct wpabuf *mesh_pending_auth;
	struct os_reltime mesh_pending_auth_time;
	u8 mesh_required_peer[ETH_ALEN];
#endif /* CONFIG_MESH */
	u16 num_bkh_vrt_ifaces;
	struct backhaul_virtual_iface **bkh_vrt_ifaces;

#ifdef CONFIG_SQLITE
	struct hostapd_eap_user tmp_eap_user;
#endif /* CONFIG_SQLITE */

#ifdef CONFIG_SAE
	/** Key used for generating SAE anti-clogging tokens */
	u8 sae_token_key[8];
	struct os_reltime last_sae_token_key_update;
	u16 sae_token_idx;
	u16 sae_pending_token_idx[256];
	int dot11RSNASAERetransPeriod; /* msec */
	struct dl_list sae_commit_queue; /* struct hostapd_sae_commit_queue */
#endif /* CONFIG_SAE */

#ifdef CONFIG_TESTING_OPTIONS
	unsigned int ext_mgmt_frame_handling:1;
	unsigned int ext_eapol_frame_io:1;

	struct l2_packet_data *l2_test;

	enum wpa_alg last_gtk_alg;
	int last_gtk_key_idx;
	u8 last_gtk[WPA_GTK_MAX_LEN];
	size_t last_gtk_len;

	enum wpa_alg last_igtk_alg;
	int last_igtk_key_idx;
	u8 last_igtk[WPA_IGTK_MAX_LEN];
	size_t last_igtk_len;

	enum wpa_alg last_bigtk_alg;
	int last_bigtk_key_idx;
	u8 last_bigtk[WPA_IGTK_MAX_LEN];
	size_t last_bigtk_len;
#endif /* CONFIG_TESTING_OPTIONS */

#ifdef CONFIG_MBO
	unsigned int mbo_assoc_disallow;
	unsigned int mbo_bss_assoc_disallow;
	unsigned int mbo_cell_pref;
	/**
	 * enable_oce - Enable OCE if it is enabled by user and device also
	 *		supports OCE.
	 */
	u8 enable_oce;
#endif /* CONFIG_MBO */

	struct dl_list nr_db;

	u8 beacon_req_token;
	u8 lci_req_token;
	u8 range_req_token;
	u8 channel_load_token;
	u8 noise_histogram_token;
	u8 link_measurement_token;
	u8 sta_statistics_token;
	u8 bss_transition_token;
	unsigned int lci_req_active:1;
	unsigned int range_req_active:1;

	int dhcp_sock; /* UDP socket used with the DHCP server */

	/* Multi-AP blacklist */
	struct dl_list multi_ap_blacklist;
	/* Auth fail list */
	struct dl_list auth_fail_list;

#ifdef CONFIG_DPP
	int dpp_init_done;
	struct dpp_authentication *dpp_auth;
	u8 dpp_allowed_roles;
	int dpp_qr_mutual;
	int dpp_auth_ok_on_ack;
	int dpp_in_response_listen;
	struct gas_query_ap *gas;
	struct dpp_pkex *dpp_pkex;
	struct dpp_bootstrap_info *dpp_pkex_bi;
	char *dpp_pkex_code;
	char *dpp_pkex_identifier;
	char *dpp_pkex_auth_cmd;
	char *dpp_configurator_params;
	struct os_reltime dpp_last_init;
	struct os_reltime dpp_init_iter_start;
	/* TODO: It is better if the parameters below are moved to hostapd_bss_conf */
	unsigned int dpp_init_max_tries;
	unsigned int dpp_init_retry_time;
	unsigned int dpp_resp_wait_time;
	unsigned int dpp_resp_max_tries;
	unsigned int dpp_resp_retry_time;
	unsigned int dpp_gas_query_timeout_period;

#ifdef CONFIG_DPP2
	struct wpabuf *dpp_presence_announcement;
	struct dpp_bootstrap_info *dpp_chirp_bi;
	int dpp_chirp_freq;
	int *dpp_chirp_freqs;
	int dpp_chirp_iter;
	int dpp_chirp_round;
	int dpp_chirp_scan_done;
	int dpp_chirp_listen;
#endif /* CONFIG_DPP2 */
#ifdef CONFIG_TESTING_OPTIONS
	char *dpp_config_obj_override;
	char *dpp_discovery_override;
	char *dpp_groups_override;
	unsigned int dpp_ignore_netaccesskey_mismatch:1;
#endif /* CONFIG_TESTING_OPTIONS */
#endif /* CONFIG_DPP */
	u8 csa_deauth_mode;
	u16 csa_deauth_tx_time[INTEL_CSA_DEAUTH_TX_TIME_ARR_SIZE];

#ifdef CONFIG_AIRTIME_POLICY
	unsigned int num_backlogged_sta;
	unsigned int airtime_weight;
#endif /* CONFIG_AIRTIME_POLICY */

#ifdef CONFIG_SQLITE
	sqlite3 *rad_attr_db;
#endif /* CONFIG_SQLITE */

#ifdef CONFIG_CTRL_IFACE_UDP
       unsigned char ctrl_iface_cookie[CTRL_IFACE_COOKIE_LEN];
#endif /* CONFIG_CTRL_IFACE_UDP */
	enum hostapd_channel_switch_reason {
		HAPD_CHAN_SWITCH_OTHER,
		HAPD_CHAN_SWITCH_RADAR_DETECTED,
	} chan_switch_reason;
};


struct hostapd_sta_info {
	struct dl_list list;
	u8 addr[ETH_ALEN];
	struct os_reltime last_seen;
	int ssi_signal;
#ifdef CONFIG_TAXONOMY
	struct wpabuf *probe_ie_taxonomy;
#endif /* CONFIG_TAXONOMY */
};

typedef struct hostapd_sb_dfs_orig_chandef {
	u8  channel;
	int secondary_offset;
	u8  vht_oper_chwidth;
	u8  vht_oper_centr_freq_seg0_idx;
}hostapd_sb_dfs_orig_chandef_t;

typedef struct hostapd_dfs_history {
	struct dl_list list;
	os_time_t detect_time;
	int bandwidth;
	u8 primary;
	u8 seg0_idx;
	u8 seg1_idx;
	u8 rbm; /* Radar Bit Map */
	u8 type;
}hostapd_dfs_history_t;

#define MAX_DFS_HIST_ENTRIES 50
#define DFS_CHAN_SWITCH      0
#define DFS_SUB_BAND_SWICH   1
#define DFS_NO_SWITCH        2


/**
 * struct hostapd_iface - hostapd per-interface data structure
 */
struct hostapd_iface {
	struct hapd_interfaces *interfaces;
	void *owner;
	char *config_fname;
	struct hostapd_config *conf;
	char phy[16]; /* Name of the PHY (radio) */

	enum hostapd_iface_state {
		HAPD_IFACE_UNINITIALIZED,
		HAPD_IFACE_DISABLED,
		HAPD_IFACE_COUNTRY_UPDATE,
		HAPD_IFACE_ACS,
		HAPD_IFACE_ACS_DONE,
		HAPD_IFACE_HT_SCAN,
		HAPD_IFACE_DFS,
		HAPD_IFACE_ENABLED
	} state;

#ifdef CONFIG_MESH
	struct mesh_conf *mconf;
#endif /* CONFIG_MESH */

	size_t num_bss;
	struct hostapd_data **bss;

	unsigned int wait_channel_update:1;
	unsigned int cac_started:1;
	unsigned int dbg_dfs_in_cac:1;
	unsigned int acs_update_in_progress:1;
	unsigned int self_beacon_req:1;

	unsigned int cac_required:1;

#ifdef CONFIG_FST
	struct fst_iface *fst;
	const struct wpabuf *fst_ies;
#endif /* CONFIG_FST */

	/*
	 * When set, indicates that the driver will handle the AP
	 * teardown: delete global keys, station keys, and stations.
	 */
	unsigned int driver_ap_teardown:1;

	/*
	 * When set, indicates that this interface is part of list of
	 * interfaces that need to be started together (synchronously).
	 */
	unsigned int need_to_start_in_sync:1;

	/* Ready to start but waiting for other interfaces to become ready. */
	unsigned int ready_to_start_in_sync:1;

	int num_ap; /* number of entries in ap_list */
	struct ap_info *ap_list; /* AP info list head */
	struct ap_info *ap_hash[STA_HASH_SIZE];

	u64 drv_flags;
	u64 drv_flags2;

	/*
	 * A bitmap of supported protocols for probe response offload. See
	 * struct wpa_driver_capa in driver.h
	 */
	unsigned int probe_resp_offloads;

	/**
	 * max_remain_on_chan - Maximum remain-on-channel duration in msec
	 */
	unsigned int max_remain_on_chan;

	/* extended capabilities supported by the driver */
	const u8 *extended_capa, *extended_capa_mask;
	unsigned int extended_capa_len;

	unsigned int drv_max_acl_mac_addrs;

	struct hostapd_hw_modes *hw_features;
	int num_hw_features;
	struct hostapd_hw_modes *current_mode;
	/* Rates that are currently used (i.e., filtered copy of
	 * current_mode->channels */
	int num_rates;
	struct hostapd_rate_data *current_rates;
	int *basic_rates;
	int freq;
        int set_freq_done;
	u8 fallback_coc_freq;        /* Maximum BW configured per interface during hostapd up/reconfigure */
	u8 require_omn;              /* Variable to keep track whether OMN IE should be added or not */
	s8 fb_coc_sec_chan;          /* fall back sec channel offset while moving up in auto coc  */
	u8 require_csa;              /* Variable to keep track whether csa IE should be added or not */

	u16 hw_flags;

	/* Number of associated Non-ERP stations (i.e., stations using 802.11b
	 * in 802.11g BSS) */
	int num_sta_non_erp;

	/* Number of associated stations that do not support Short Slot Time */
	int num_sta_no_short_slot_time;

	/* Number of associated stations that do not support Short Preamble */
	int num_sta_no_short_preamble;

	int olbc; /* Overlapping Legacy BSS Condition */

	/* Number of HT associated stations that do not support greenfield */
	int num_sta_ht_no_gf;

	/* Number of associated non-HT stations */
	int num_sta_no_ht;

	/* Number of HT associated stations 20 MHz */
	int num_sta_ht_20mhz;

	/* Number of HT40 intolerant stations */
	int num_sta_ht40_intolerant;

	/* Overlapping BSS information */
	int olbc_ht;

	u16 ht_op_mode;

	/* surveying helpers */

	/* number of channels surveyed */
	unsigned int chans_surveyed;

	/* lowest observed noise floor in dBm */
	s8 lowest_nf;

	/* channel utilization calculation */
	u64 last_channel_time;
	u64 last_channel_time_busy;
	u8 channel_utilization;

	unsigned int chan_util_samples_sum;
	unsigned int chan_util_num_sample_periods;
	unsigned int chan_util_average;

	/* eCSA IE will be added only if operating class is specified */
	u8 cs_oper_class;

	unsigned int dfs_cac_ms;
	struct os_reltime dfs_cac_start;
	int allow_scan_during_cac;

	/* Latched with the actual secondary channel information and will be
	 * used while juggling between HT20 and HT40 modes. */
	int secondary_ch;

	int in_scan;
#ifdef CONFIG_ACS
	unsigned int acs_num_completed_scans;

	acs_candidate_table candidates[MAX_CANDIDATES]; /* MAX_CANDIDATES == all possible bondings */
	u32 num_candidates;
	int selected_candidate;
	u32 min_noise;
	u32 acs_num_bss; /* over the whole band */
	u32 max_tx_power; /* over the whole band */
	u32 max_num_bss_pen;
#endif /* CONFIG_ACS */

	void (*scan_cb)(struct hostapd_iface *iface);
	int num_ht40_scan_tries;

	struct hostapd_failsafe_freq_params failsafe;

	struct dl_list sta_seen; /* struct hostapd_sta_info */
	unsigned int num_sta_seen;

	struct beacon_request last_beacon_req;
	u8 dfs_domain;

	int atf_enabled; /* If ATF is currently enabled in FW */
	u32 atf_sta_in_driver[2048 / 32]; /* One bit per aid */
	u32 atf_sta_has_quota[2048 / 32]; /* One bit per aid */

	int block_tx; /* Is TX block on or off */
	int sb_dfs_cntr;
	int dfs_hist_entry_cnt;
	hostapd_sb_dfs_orig_chandef_t sb_orig_chandef;
	struct dl_list dfs_history; /* struct hostapd_dfs_history */

	s8  stas_free_reason; /* the reason for all stations remove */
#ifdef CONFIG_AIRTIME_POLICY
	unsigned int airtime_quantum;
#endif /* CONFIG_AIRTIME_POLICY */

	bool boot;

	bool whm_if_fail;
	bool retrigger_acs;
};

typedef struct {
	u32 primary;
	u32 secondary;
} channel_pair;

typedef struct {
	u32 chan_no;
	u32 primary;
	u32 secondary;
	u32 freq;
} channel_80;

typedef struct {
	u32 chan_no;
	u32 primary;
	u32 secondary;
	u32 freq;
} channel_160;

#define NUM_CHAN_PAIRS_40_24G 18
#define NUM_CHAN_PAIRS_40_5G  24
#define NUM_CHAN_PAIRS_80     24
#define NUM_CHAN_PAIRS_160    16

/* hostapd.c */
int hostapd_for_each_interface(struct hapd_interfaces *interfaces,
			       int (*cb)(struct hostapd_iface *iface,
					 void *ctx), void *ctx);
int hostapd_reload_config(struct hostapd_iface *iface);
void hostapd_reconfig_encryption(struct hostapd_data *hapd);
struct hostapd_data *
hostapd_alloc_bss_data(struct hostapd_iface *hapd_iface,
		       struct hostapd_config *conf,
		       struct hostapd_bss_config *bss);
int hostapd_setup_interface(struct hostapd_iface *iface);
int hostapd_setup_interface_complete(struct hostapd_iface *iface, int err);
void hostapd_interface_deinit(struct hostapd_iface *iface);
void hostapd_interface_free(struct hostapd_iface *iface);
struct hostapd_iface * hostapd_alloc_iface(void);
struct hostapd_iface * hostapd_init(struct hapd_interfaces *interfaces,
				    const char *config_file);
struct hostapd_iface *
hostapd_interface_init_bss(struct hapd_interfaces *interfaces, const char *phy,
			   const char *config_fname, int debug);
void hostapd_new_assoc_sta(struct hostapd_data *hapd, struct sta_info *sta,
			   int reassoc);
void hostapd_interface_deinit_free(struct hostapd_iface *iface);
int hostapd_enable_iface(struct hostapd_iface *hapd_iface);
int hostapd_reconf_iface(struct hostapd_iface *hapd_iface, int changed_idx);
int hostapd_reload_iface(struct hostapd_iface *hapd_iface);
int hostapd_disable_iface(struct hostapd_iface *hapd_iface);
int hostapd_add_iface(struct hapd_interfaces *ifaces, char *buf);
int hostapd_remove_iface(struct hapd_interfaces *ifaces, char *buf);
int hostapd_setup_bss(struct hostapd_data *hapd, int first);

int hostapd_activate_backhaul_ifaces(struct hostapd_data *hapd);

void hostapd_channel_list_updated(struct hostapd_iface *iface, struct channel_list_changed *info);
void hostapd_set_state(struct hostapd_iface *iface, enum hostapd_iface_state s);
const char * hostapd_state_text(enum hostapd_iface_state s);
const char * hostapd_channel_switch_text(enum hostapd_channel_switch_reason s);
void hostapd_channel_switch_reason_set(struct hostapd_iface *iface,
				       enum hostapd_channel_switch_reason reason);
int hostapd_csa_in_progress(struct hostapd_iface *iface);
int hostapd_get_cs_count(struct hostapd_iface *iface);
void hostapd_chan_switch_vht_config(struct hostapd_data *hapd, int vht_enabled);
int hostapd_prepare_and_send_csa_deauth_cfg_to_driver(struct hostapd_data *hapd);
int hostapd_switch_channel(struct hostapd_data *hapd,
			   struct csa_settings *settings);
void hostapd_send_event_unsolicited_probe_response(struct hostapd_data *hapd);
void
hostapd_switch_channel_fallback(struct hostapd_iface *iface,
				const struct hostapd_freq_params *freq_params);
void hostapd_cleanup_cs_params(struct hostapd_data *hapd);
void hostapd_periodic_iface(struct hostapd_iface *iface);
int hostapd_owe_trans_get_info(struct hostapd_data *hapd);

/* utils.c */
int hostapd_register_probereq_cb(struct hostapd_data *hapd,
				 int (*cb)(void *ctx, const u8 *sa,
					   const u8 *da, const u8 *bssid,
					   const u8 *ie, size_t ie_len,
					   int ssi_signal),
				 void *ctx);
void hostapd_prune_associations(struct hostapd_data *hapd, const u8 *addr);
int hostapd_get_list(const char *s, char **e, struct hostapd_iface *iface,
  int (*number_cb)(struct hostapd_iface*, int),
  int (*range_cb)(struct hostapd_iface*, int, int));

/* drv_callbacks.c (TODO: move to somewhere else?) */
void hostapd_notify_assoc_fils_finish(struct hostapd_data *hapd,
				      struct sta_info *sta);
int hostapd_notif_assoc(struct hostapd_data *hapd, const u8 *addr,
			const u8 *ie, size_t ielen, int reassoc);
void hostapd_notif_disassoc(struct hostapd_data *hapd, const u8 *addr);
void hostapd_event_sta_low_ack(struct hostapd_data *hapd, const u8 *addr);
void hostapd_event_connect_failed_reason(struct hostapd_data *hapd,
					 const u8 *addr, int reason_code);
int hostapd_probe_req_rx(struct hostapd_data *hapd, const u8 *sa, const u8 *da,
			 const u8 *bssid, const u8 *ie, size_t ie_len,
			 int ssi_signal);
void hostapd_event_ch_switch(struct hostapd_data *hapd, int freq, int ht,
			     int offset, int width, int cf1, int cf2,
			     int finished);
struct hostapd_channel_data * hostapd_get_mode_channel(struct hostapd_iface *iface,
						       unsigned int freq);
u16 hostapd_get_beacon_int(const struct hostapd_data *hapd);
struct survey_results;
void hostapd_event_get_survey(struct hostapd_iface *iface,
			      struct survey_results *survey_results);

const struct hostapd_eap_user *
hostapd_get_eap_user(struct hostapd_data *hapd, const u8 *identity,
		     size_t identity_len, int phase2);

struct hostapd_data * hostapd_get_iface(struct hapd_interfaces *interfaces,
					const char *ifname);
void hostapd_event_sta_opmode_changed(struct hostapd_data *hapd, const u8 *addr,
				      enum smps_mode smps_mode,
				      enum chan_width chan_width, u8 rx_nss);

#ifdef CONFIG_FST
void fst_hostapd_fill_iface_obj(struct hostapd_data *hapd,
				struct fst_wpa_obj *iface_obj);
#endif /* CONFIG_FST */

void hostapd_ltq_clear_old(struct hostapd_iface *iface, s32 flush);

#ifdef CONFIG_ACS
void hostapd_acs_channel_selected(struct hostapd_data *hapd,
				  struct acs_selected_channels *acs_res);
void hostapd_ltq_update_channel_data(struct hostapd_iface *iface, const u8 *data, size_t data_len);
#endif

void hostapd_tx_queue_params(struct hostapd_data *hapd);
void hostapd_event_ltq_softblock_drop(struct hostapd_data *hapd,
		struct intel_vendor_event_msg_drop *msg_dropped);
void hostapd_event_ltq_report_event(struct hostapd_data *hapd,
		u8 msgtype, u8 event, u8 *macaddr, u8 fail);
void hostapd_free_cached_events(struct hostapd_data *hapd);

void send_cred_request_to_ui(struct hostapd_data *hapd);
void terminate_hostapd(void *eloop_ctx, void *timeout_ctx);
void cred_request_timeout(void *eloop_ctx, void *user_data);
void send_cred_request_handler(void *eloop_ctx, void *user_data);
void hostapd_send_fils_discovery_frame(struct hostapd_data *hapd);
u8 * hostapd_eid_txpower_envelope_6g(struct hostapd_data *hapd, u8 *eid, bool only_one_psd_limit);

int get_num_width(int vht_width, int secondary);
int hostapd_chan_to_freq(int channel);
int hostapd_get_chan_data_from_candidates_5g (int primary_chan, u8 ch_width,
					      int *secondary_chan_offset, u8 *vht_oper_centr_freq_seg0_idx);
void hostapd_handle_acs_whm_warning (struct hostapd_iface *iface);
#endif /* HOSTAPD_H */
