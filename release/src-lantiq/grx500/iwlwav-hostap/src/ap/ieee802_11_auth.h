/*
 * hostapd / IEEE 802.11 authentication (ACL)
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef IEEE802_11_AUTH_H
#define IEEE802_11_AUTH_H

#define WLAN_MSG_TYPE_EAPOL 16

enum {
	HOSTAPD_ACL_REJECT = 0,
	HOSTAPD_ACL_ACCEPT = 1,
	HOSTAPD_ACL_PENDING = 2,
	HOSTAPD_ACL_ACCEPT_TIMEOUT = 3
};

enum {
	HOSTAPD_SOFTBLOCK_ACCEPT = 0,
	HOSTAPD_SOFTBLOCK_DROP = 1,
	HOSTAPD_SOFTBLOCK_REJECT = 2,
	HOSTAPD_MULTI_AP_BLACKLIST_FOUND = 3,
	HOSTAPD_SOFTBLOCK_ALLOWED = 4
};

enum {
	HOSTAPD_EVENT_AUTH_FAIL_UNKNOWN_REASON = 0,
	HOSTAPD_EVENT_AUTH_FAIL_WRONG_PASSWORD = 1,
	HOSTAPD_EVENT_AUTH_FAIL_TIMEOUT = 2,
};

enum {
	HOSTAPD_EVENT_ASSOC_NEW_CONNECTION = 1,
	HOSTAPD_EVENT_ASSOC_RENEW_CONNECTION = 2,
	HOSTAPD_EVENT_ASSOC_RECONNECTION_AFTER_INACTIVITY = 3,
	HOSTAPD_EVENT_ASSOC_NEW_CONNECTION_COMPLETE = 4,
};

int hostapd_check_acl(struct hostapd_data *hapd, const u8 *addr,
		      struct vlan_description *vlan_id);
int hostapd_allowed_address(struct hostapd_data *hapd, const u8 *addr,
			    const u8 *msg, size_t len, u32 *session_timeout,
			    u32 *acct_interim_interval,
			    struct vlan_description *vlan_id,
			    struct hostapd_sta_wpa_psk_short **psk,
			    char **identity, char **radius_cui,
			    int is_probe_req);
int hostapd_acl_init(struct hostapd_data *hapd);
void hostapd_acl_deinit(struct hostapd_data *hapd);
void hostapd_free_psk_list(struct hostapd_sta_wpa_psk_short *psk);
void hostapd_acl_expire(struct hostapd_data *hapd);
int ieee802_11_multi_ap_blacklist_add(struct hostapd_data *hapd,
	struct multi_ap_blacklist *entry);
void ieee802_11_multi_ap_blacklist_remove(struct hostapd_data *hapd,
	struct multi_ap_blacklist *entry);
void ieee802_11_multi_ap_blacklist_flush(struct hostapd_data *hapd);
int ieee802_11_multi_ap_blacklist_print(struct hostapd_data *hapd, char *buf,
	size_t buflen);
int ieee802_11_multi_ap_set_deny_mac(struct hostapd_data *hapd,
	struct multi_ap_blacklist* entry, const u8 remove);
int ieee802_11_multi_ap_set_softblock_thresholds(struct hostapd_data *hapd,
	struct multi_ap_blacklist* entry, const u8 remove);
int hostapd_check_softblock(struct hostapd_data *hapd,const u8 *addr, u16 *status,
	u16 msgtype, int snr_db, struct intel_vendor_event_msg_drop *msg_dropped);
struct multi_ap_blacklist * ieee802_11_multi_ap_blacklist_find(struct hostapd_data *hapd,
	const u8 *addr);
int ieee802_11_auth_fail_entry_add(struct hostapd_data *hapd, u8 *addr);
void ieee802_11_auth_fail_blacklist_flush(void *eloop_ctx, void *user_ctx);
void hostapd_clean_auth_fail_list(struct hostapd_data *hapd);

enum {
	SOFT_BLOCK_ACL_ALLOW,
	SOFT_BLOCK_ACL_DROP,
	SOFT_BLOCK_ACL_AUTH_RESP_FORCED
};

int hostapd_soft_block_acl_client_status(struct hostapd_data *hapd, const u8 *addr, u16 frame_type, u16 *status);

#endif /* IEEE802_11_AUTH_H */
