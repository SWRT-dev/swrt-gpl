/*
 * hostapd / IEEE 802.11 authentication (ACL)
 * Copyright (c) 2003-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * Access control list for IEEE 802.11 authentication can uses statically
 * configured ACL from configuration files or an external RADIUS server.
 * Results from external RADIUS queries are cached to allow faster
 * authentication frame processing.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/eloop.h"
#include "radius/radius.h"
#include "radius/radius_client.h"
#include "hostapd.h"
#include "ap_config.h"
#include "ap_drv_ops.h"
#include "ieee802_11.h"
#include "ieee802_1x.h"
#include "ieee802_11_auth.h"
#include "utils/list.h"
#include "sta_info.h"

#define RADIUS_ACL_TIMEOUT 30


struct hostapd_cached_radius_acl {
	struct os_reltime timestamp;
	macaddr addr;
	int accepted; /* HOSTAPD_ACL_* */
	struct hostapd_cached_radius_acl *next;
	u32 session_timeout;
	u32 acct_interim_interval;
	struct vlan_description vlan_id;
	struct hostapd_sta_wpa_psk_short *psk;
	char *identity;
	char *radius_cui;
};


struct hostapd_acl_query_data {
	struct os_reltime timestamp;
	u8 radius_id;
	macaddr addr;
	u8 *auth_msg; /* IEEE 802.11 authentication frame from station */
	size_t auth_msg_len;
	struct hostapd_acl_query_data *next;
};


#ifndef CONFIG_NO_RADIUS
static void hostapd_acl_cache_free_entry(struct hostapd_cached_radius_acl *e)
{
	os_free(e->identity);
	os_free(e->radius_cui);
	hostapd_free_psk_list(e->psk);
	os_free(e);
}


static void hostapd_acl_cache_free(struct hostapd_cached_radius_acl *acl_cache)
{
	struct hostapd_cached_radius_acl *prev;

	while (acl_cache) {
		prev = acl_cache;
		acl_cache = acl_cache->next;
		hostapd_acl_cache_free_entry(prev);
	}
}


static void copy_psk_list(struct hostapd_sta_wpa_psk_short **psk,
			  struct hostapd_sta_wpa_psk_short *src)
{
	if (!psk)
		return;

	if (src)
		src->ref++;

	*psk = src;
}


static int hostapd_acl_cache_get(struct hostapd_data *hapd, const u8 *addr,
				 u32 *session_timeout,
				 u32 *acct_interim_interval,
				 struct vlan_description *vlan_id,
				 struct hostapd_sta_wpa_psk_short **psk,
				 char **identity, char **radius_cui)
{
	struct hostapd_cached_radius_acl *entry;
	struct os_reltime now;

	os_get_reltime(&now);

	for (entry = hapd->acl_cache; entry; entry = entry->next) {
		if (os_memcmp(entry->addr, addr, ETH_ALEN) != 0)
			continue;

		if (os_reltime_expired(&now, &entry->timestamp,
				       RADIUS_ACL_TIMEOUT))
			return -1; /* entry has expired */
		if (entry->accepted == HOSTAPD_ACL_ACCEPT_TIMEOUT)
			if (session_timeout)
				*session_timeout = entry->session_timeout;
		if (acct_interim_interval)
			*acct_interim_interval =
				entry->acct_interim_interval;
		if (vlan_id)
			*vlan_id = entry->vlan_id;
		copy_psk_list(psk, entry->psk);
		if (identity) {
			if (entry->identity)
				*identity = os_strdup(entry->identity);
			else
				*identity = NULL;
		}
		if (radius_cui) {
			if (entry->radius_cui)
				*radius_cui = os_strdup(entry->radius_cui);
			else
				*radius_cui = NULL;
		}
		return entry->accepted;
	}

	return -1;
}
#endif /* CONFIG_NO_RADIUS */


static void hostapd_acl_query_free(struct hostapd_acl_query_data *query)
{
	if (query == NULL)
		return;
	os_free(query->auth_msg);
	os_free(query);
}


#ifndef CONFIG_NO_RADIUS
static int hostapd_radius_acl_query(struct hostapd_data *hapd, const u8 *addr,
				    struct hostapd_acl_query_data *query)
{
	struct radius_msg *msg;
	char buf[128];

	query->radius_id = radius_client_get_id(hapd->radius);
	msg = radius_msg_new(RADIUS_CODE_ACCESS_REQUEST, query->radius_id);
	if (msg == NULL)
		return -1;

	if (radius_msg_make_authenticator(msg) < 0) {
		wpa_printf(MSG_INFO, "Could not make Request Authenticator");
		goto fail;
	}

	os_snprintf(buf, sizeof(buf), RADIUS_ADDR_FORMAT, MAC2STR(addr));
	if (!radius_msg_add_attr(msg, RADIUS_ATTR_USER_NAME, (u8 *) buf,
				 os_strlen(buf))) {
		wpa_printf(MSG_DEBUG, "Could not add User-Name");
		goto fail;
	}

	if (!radius_msg_add_attr_user_password(
		    msg, (u8 *) buf, os_strlen(buf),
		    hapd->conf->radius->auth_server->shared_secret,
		    hapd->conf->radius->auth_server->shared_secret_len)) {
		wpa_printf(MSG_DEBUG, "Could not add User-Password");
		goto fail;
	}

	if (add_common_radius_attr(hapd, hapd->conf->radius_auth_req_attr,
				   NULL, msg) < 0)
		goto fail;

	os_snprintf(buf, sizeof(buf), RADIUS_802_1X_ADDR_FORMAT,
		    MAC2STR(addr));
	if (!radius_msg_add_attr(msg, RADIUS_ATTR_CALLING_STATION_ID,
				 (u8 *) buf, os_strlen(buf))) {
		wpa_printf(MSG_DEBUG, "Could not add Calling-Station-Id");
		goto fail;
	}

	os_snprintf(buf, sizeof(buf), "CONNECT 11Mbps 802.11b");
	if (!radius_msg_add_attr(msg, RADIUS_ATTR_CONNECT_INFO,
				 (u8 *) buf, os_strlen(buf))) {
		wpa_printf(MSG_DEBUG, "Could not add Connect-Info");
		goto fail;
	}

	if (radius_client_send(hapd->radius, msg, RADIUS_AUTH, addr) < 0)
		goto fail;
	return 0;

 fail:
	radius_msg_free(msg);
	return -1;
}
#endif /* CONFIG_NO_RADIUS */

static void ieee802_11_auth_fail_list_flush(struct hostapd_data *hapd)
{
	struct auth_fail_list *iter;

	while ((iter = dl_list_first(&hapd->auth_fail_list, struct auth_fail_list,
		list)) != NULL)
	{
		dl_list_del(&iter->list);
		os_free(iter);
	}
}

void ieee802_11_auth_fail_blacklist_flush(void *eloop_ctx, void *user_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	struct auth_fail_list *iter;

	while ((iter = dl_list_first(&hapd->auth_fail_list, struct auth_fail_list,
		list)) != NULL)
	{
		if (iter->auth_fail_count == hapd->conf->max_eap_failure)
		{
			dl_list_del(&iter->list);
			os_free(iter);
		}
	}
	eloop_cancel_timeout(ieee802_11_auth_fail_blacklist_flush, hapd, NULL);
	eloop_register_timeout(hapd->conf->auth_fail_blacklist_duration, 0,
					ieee802_11_auth_fail_blacklist_flush, hapd, NULL);
}

static int ieee802_11_auth_fail_blacklist_find(struct hostapd_data *hapd,
		const u8 *addr)
{
	struct auth_fail_list *iter;

	dl_list_for_each(iter, &hapd->auth_fail_list, struct auth_fail_list, list)
		if ((os_memcmp(addr, iter->addr, ETH_ALEN) == 0) &&
				(iter->auth_fail_count == hapd->conf->max_eap_failure))
			return 0;
	return -1;
}

int ieee802_11_auth_fail_entry_add(struct hostapd_data *hapd,
		u8 *addr)
{
	errno_t err;
	struct auth_fail_list *entry = NULL;

	dl_list_for_each(entry, &hapd->auth_fail_list, struct auth_fail_list, list)
	{
		if (os_memcmp(addr, entry->addr, ETH_ALEN) == 0)
		{
			if (entry->auth_fail_count < hapd->conf->max_eap_failure)
			{
				entry->auth_fail_count++;
			} else {
				entry->auth_fail_count = hapd->conf->max_eap_failure;
			}
			return 0;
		}
	}
	entry = os_malloc(sizeof(*entry));
	if (!entry)
	{
		wpa_printf(MSG_ERROR, "Failed to allocate memory for Auth fail entry");
		return -1;
	}
	os_memset(entry, 0, sizeof(*entry));
	err = memcpy_s(entry->addr, sizeof(entry->addr), addr, sizeof(entry->addr));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: __func__memcpy_s error %d", __func__, err);
		os_free(entry);
		return -1;
	}

	DL_LIST_ADD(&hapd->auth_fail_list, entry, list);
	return 0;
}

void hostapd_clean_auth_fail_list(struct hostapd_data *hapd)
{
	if(!hapd->conf->ieee802_1x && !hapd->conf->max_eap_failure)
		return;
	ieee802_11_auth_fail_list_flush(hapd);
	if (eloop_is_timeout_registered(ieee802_11_auth_fail_blacklist_flush,
					hapd, NULL))
		eloop_cancel_timeout(ieee802_11_auth_fail_blacklist_flush, hapd, NULL);
}


/**
 * hostapd_check_acl - Check a specified STA against accept/deny ACLs
 * @hapd: hostapd BSS data
 * @addr: MAC address of the STA
 * @vlan_id: Buffer for returning VLAN ID
 * Returns: HOSTAPD_ACL_ACCEPT, HOSTAPD_ACL_REJECT, or HOSTAPD_ACL_PENDING
 */
int hostapd_check_acl(struct hostapd_data *hapd, const u8 *addr,
		      struct vlan_description *vlan_id)
{
	if (hostapd_maclist_found(hapd->conf->accept_mac,
				  hapd->conf->num_accept_mac, addr, vlan_id))
		return HOSTAPD_ACL_ACCEPT;

	if (hostapd_maclist_found(hapd->conf->deny_mac,
				  hapd->conf->num_deny_mac, addr, vlan_id))
		return HOSTAPD_ACL_REJECT;

	if (hapd->conf->macaddr_acl == ACCEPT_UNLESS_DENIED)
		return HOSTAPD_ACL_ACCEPT;
	if (hapd->conf->macaddr_acl == DENY_UNLESS_ACCEPTED)
		return HOSTAPD_ACL_REJECT;

	return HOSTAPD_ACL_PENDING;
}


/**
 * hostapd_allowed_address - Check whether a specified STA can be authenticated
 * @hapd: hostapd BSS data
 * @addr: MAC address of the STA
 * @msg: Authentication message
 * @len: Length of msg in octets
 * @session_timeout: Buffer for returning session timeout (from RADIUS)
 * @acct_interim_interval: Buffer for returning account interval (from RADIUS)
 * @vlan_id: Buffer for returning VLAN ID
 * @psk: Linked list buffer for returning WPA PSK
 * @identity: Buffer for returning identity (from RADIUS)
 * @radius_cui: Buffer for returning CUI (from RADIUS)
 * @is_probe_req: Whether this query for a Probe Request frame
 * Returns: HOSTAPD_ACL_ACCEPT, HOSTAPD_ACL_REJECT, or HOSTAPD_ACL_PENDING
 *
 * The caller is responsible for freeing the returned *identity and *radius_cui
 * values with os_free().
 */
int hostapd_allowed_address(struct hostapd_data *hapd, const u8 *addr,
			    const u8 *msg, size_t len, u32 *session_timeout,
			    u32 *acct_interim_interval,
			    struct vlan_description *vlan_id,
			    struct hostapd_sta_wpa_psk_short **psk,
			    char **identity, char **radius_cui,
			    int is_probe_req)
{
	int res;

	if (session_timeout)
		*session_timeout = 0;
	if (acct_interim_interval)
		*acct_interim_interval = 0;
	if (vlan_id)
		os_memset(vlan_id, 0, sizeof(*vlan_id));
	if (psk)
		*psk = NULL;
	if (identity)
		*identity = NULL;
	if (radius_cui)
		*radius_cui = NULL;

	if ((hapd->conf->ieee802_1x) && (hapd->conf->max_eap_failure) &&
		!(ieee802_11_auth_fail_blacklist_find(hapd, addr)))
		return HOSTAPD_ACL_REJECT;

	res = hostapd_check_acl(hapd, addr, vlan_id);
	if (res != HOSTAPD_ACL_PENDING)
		return res;

	if (hapd->conf->macaddr_acl == USE_EXTERNAL_RADIUS_AUTH) {
#ifdef CONFIG_NO_RADIUS
		return HOSTAPD_ACL_REJECT;
#else /* CONFIG_NO_RADIUS */
		struct hostapd_acl_query_data *query;

		if (is_probe_req) {
			/* Skip RADIUS queries for Probe Request frames to avoid
			 * excessive load on the authentication server. */
			return HOSTAPD_ACL_ACCEPT;
		};

		if (hapd->conf->ssid.dynamic_vlan == DYNAMIC_VLAN_DISABLED)
			vlan_id = NULL;

		/* Check whether ACL cache has an entry for this station */
		res = hostapd_acl_cache_get(hapd, addr, session_timeout,
					    acct_interim_interval, vlan_id, psk,
					    identity, radius_cui);
		if (res == HOSTAPD_ACL_ACCEPT ||
		    res == HOSTAPD_ACL_ACCEPT_TIMEOUT)
			return res;
		if (res == HOSTAPD_ACL_REJECT)
			return HOSTAPD_ACL_REJECT;

		query = hapd->acl_queries;
		while (query) {
			if (os_memcmp(query->addr, addr, ETH_ALEN) == 0) {
				/* pending query in RADIUS retransmit queue;
				 * do not generate a new one */
				if (identity) {
					os_free(*identity);
					*identity = NULL;
				}
				if (radius_cui) {
					os_free(*radius_cui);
					*radius_cui = NULL;
				}
				return HOSTAPD_ACL_PENDING;
			}
			query = query->next;
		}

		if (!hapd->conf->radius->auth_server)
			return HOSTAPD_ACL_REJECT;

		/* No entry in the cache - query external RADIUS server */
		query = os_zalloc(sizeof(*query));
		if (query == NULL) {
			wpa_printf(MSG_ERROR, "malloc for query data failed");
			return HOSTAPD_ACL_REJECT;
		}
		os_get_reltime(&query->timestamp);
		os_memcpy(query->addr, addr, ETH_ALEN);
		if (hostapd_radius_acl_query(hapd, addr, query)) {
			wpa_printf(MSG_DEBUG, "Failed to send Access-Request "
				   "for ACL query.");
			hostapd_acl_query_free(query);
			return HOSTAPD_ACL_REJECT;
		}

		query->auth_msg = os_memdup(msg, len);
		if (query->auth_msg == NULL) {
			wpa_printf(MSG_ERROR, "Failed to allocate memory for "
				   "auth frame.");
			hostapd_acl_query_free(query);
			return HOSTAPD_ACL_REJECT;
		}
		query->auth_msg_len = len;
		query->next = hapd->acl_queries;
		hapd->acl_queries = query;

		/* Queued data will be processed in hostapd_acl_recv_radius()
		 * when RADIUS server replies to the sent Access-Request. */
		return HOSTAPD_ACL_PENDING;
#endif /* CONFIG_NO_RADIUS */
	}

	return HOSTAPD_ACL_REJECT;
}


#ifndef CONFIG_NO_RADIUS
static void hostapd_acl_expire_cache(struct hostapd_data *hapd,
				     struct os_reltime *now)
{
	struct hostapd_cached_radius_acl *prev, *entry, *tmp;

	prev = NULL;
	entry = hapd->acl_cache;

	while (entry) {
		if (os_reltime_expired(now, &entry->timestamp,
				       RADIUS_ACL_TIMEOUT)) {
			wpa_printf(MSG_DEBUG, "Cached ACL entry for " MACSTR
				   " has expired.", MAC2STR(entry->addr));
			if (prev)
				prev->next = entry->next;
			else
				hapd->acl_cache = entry->next;
			hostapd_drv_set_radius_acl_expire(hapd, entry->addr);
			tmp = entry;
			entry = entry->next;
			hostapd_acl_cache_free_entry(tmp);
			continue;
		}

		prev = entry;
		entry = entry->next;
	}
}


static void hostapd_acl_expire_queries(struct hostapd_data *hapd,
				       struct os_reltime *now)
{
	struct hostapd_acl_query_data *prev, *entry, *tmp;

	prev = NULL;
	entry = hapd->acl_queries;

	while (entry) {
		if (os_reltime_expired(now, &entry->timestamp,
				       RADIUS_ACL_TIMEOUT)) {
			wpa_printf(MSG_DEBUG, "ACL query for " MACSTR
				   " has expired.", MAC2STR(entry->addr));
			if (prev)
				prev->next = entry->next;
			else
				hapd->acl_queries = entry->next;

			tmp = entry;
			entry = entry->next;
			hostapd_acl_query_free(tmp);
			continue;
		}

		prev = entry;
		entry = entry->next;
	}
}


/**
 * hostapd_acl_expire - ACL cache expiration callback
 * @hapd: struct hostapd_data *
 */
void hostapd_acl_expire(struct hostapd_data *hapd)
{
	struct os_reltime now;

	os_get_reltime(&now);
	hostapd_acl_expire_cache(hapd, &now);
	hostapd_acl_expire_queries(hapd, &now);
}


static void decode_tunnel_passwords(struct hostapd_data *hapd,
				    const u8 *shared_secret,
				    size_t shared_secret_len,
				    struct radius_msg *msg,
				    struct radius_msg *req,
				    struct hostapd_cached_radius_acl *cache)
{
	int passphraselen;
	char *passphrase;
	size_t i;
	struct hostapd_sta_wpa_psk_short *psk;

	/*
	 * Decode all tunnel passwords as PSK and save them into a linked list.
	 */
	for (i = 0; ; i++) {
		passphrase = radius_msg_get_tunnel_password(
			msg, &passphraselen, shared_secret, shared_secret_len,
			req, i);
		/*
		 * Passphrase is NULL iff there is no i-th Tunnel-Password
		 * attribute in msg.
		 */
		if (passphrase == NULL)
			break;

		/*
		 * Passphase should be 8..63 chars (to be hashed with SSID)
		 * or 64 chars hex string (no separate hashing with SSID).
		 */

		if (passphraselen < MIN_PASSPHRASE_LEN ||
		    passphraselen > MAX_PASSPHRASE_LEN + 1)
			goto free_pass;

		/*
		 * passphrase does not contain the NULL termination.
		 * Add it here as pbkdf2_sha1() requires it.
		 */
		psk = os_zalloc(sizeof(struct hostapd_sta_wpa_psk_short));
		if (psk) {
			if ((passphraselen == MAX_PASSPHRASE_LEN + 1) &&
			    (hexstr2bin(passphrase, psk->psk, PMK_LEN) < 0)) {
				hostapd_logger(hapd, cache->addr,
					       HOSTAPD_MODULE_RADIUS,
					       HOSTAPD_LEVEL_WARNING,
					       "invalid hex string (%d chars) in Tunnel-Password",
					       passphraselen);
				goto skip;
			} else if (passphraselen <= MAX_PASSPHRASE_LEN) {
				os_memcpy(psk->passphrase, passphrase,
					  passphraselen);
				psk->is_passphrase = 1;
			}
			psk->next = cache->psk;
			cache->psk = psk;
			psk = NULL;
		}
skip:
		os_free(psk);
free_pass:
		os_free(passphrase);
	}
}


/**
 * hostapd_acl_recv_radius - Process incoming RADIUS Authentication messages
 * @msg: RADIUS response message
 * @req: RADIUS request message
 * @shared_secret: RADIUS shared secret
 * @shared_secret_len: Length of shared_secret in octets
 * @data: Context data (struct hostapd_data *)
 * Returns: RADIUS_RX_PROCESSED if RADIUS message was a reply to ACL query (and
 * was processed here) or RADIUS_RX_UNKNOWN if not.
 */
static RadiusRxResult
hostapd_acl_recv_radius(struct radius_msg *msg, struct radius_msg *req,
			const u8 *shared_secret, size_t shared_secret_len,
			void *data)
{
	struct hostapd_data *hapd = data;
	struct hostapd_acl_query_data *query, *prev;
	struct hostapd_cached_radius_acl *cache;
	struct radius_hdr *hdr = radius_msg_get_hdr(msg);

	query = hapd->acl_queries;
	prev = NULL;
	while (query) {
		if (query->radius_id == hdr->identifier)
			break;
		prev = query;
		query = query->next;
	}
	if (query == NULL)
		return RADIUS_RX_UNKNOWN;

	wpa_printf(MSG_DEBUG, "Found matching Access-Request for RADIUS "
		   "message (id=%d)", query->radius_id);

	if (radius_msg_verify(msg, shared_secret, shared_secret_len, req, 0)) {
		wpa_printf(MSG_INFO, "Incoming RADIUS packet did not have "
			   "correct authenticator - dropped\n");
		return RADIUS_RX_INVALID_AUTHENTICATOR;
	}

	if (hdr->code != RADIUS_CODE_ACCESS_ACCEPT &&
	    hdr->code != RADIUS_CODE_ACCESS_REJECT) {
		wpa_printf(MSG_DEBUG, "Unknown RADIUS message code %d to ACL "
			   "query", hdr->code);
		return RADIUS_RX_UNKNOWN;
	}

	/* Insert Accept/Reject info into ACL cache */
	cache = os_zalloc(sizeof(*cache));
	if (cache == NULL) {
		wpa_printf(MSG_DEBUG, "Failed to add ACL cache entry");
		goto done;
	}
	os_get_reltime(&cache->timestamp);
	os_memcpy(cache->addr, query->addr, sizeof(cache->addr));
	if (hdr->code == RADIUS_CODE_ACCESS_ACCEPT) {
		u8 *buf;
		size_t len;

		if (radius_msg_get_attr_int32(msg, RADIUS_ATTR_SESSION_TIMEOUT,
					      &cache->session_timeout) == 0)
			cache->accepted = HOSTAPD_ACL_ACCEPT_TIMEOUT;
		else
			cache->accepted = HOSTAPD_ACL_ACCEPT;

		if (radius_msg_get_attr_int32(
			    msg, RADIUS_ATTR_ACCT_INTERIM_INTERVAL,
			    &cache->acct_interim_interval) == 0 &&
		    cache->acct_interim_interval < 60) {
			wpa_printf(MSG_DEBUG, "Ignored too small "
				   "Acct-Interim-Interval %d for STA " MACSTR,
				   cache->acct_interim_interval,
				   MAC2STR(query->addr));
			cache->acct_interim_interval = 0;
		}

		if (hapd->conf->ssid.dynamic_vlan != DYNAMIC_VLAN_DISABLED)
			cache->vlan_id.notempty = !!radius_msg_get_vlanid(
				msg, &cache->vlan_id.untagged,
				MAX_NUM_TAGGED_VLAN, cache->vlan_id.tagged);

		decode_tunnel_passwords(hapd, shared_secret, shared_secret_len,
					msg, req, cache);

		if (radius_msg_get_attr_ptr(msg, RADIUS_ATTR_USER_NAME,
					    &buf, &len, NULL) == 0) {
			cache->identity = os_zalloc(len + 1);
			if (cache->identity)
				os_memcpy(cache->identity, buf, len);
		}
		if (radius_msg_get_attr_ptr(
			    msg, RADIUS_ATTR_CHARGEABLE_USER_IDENTITY,
			    &buf, &len, NULL) == 0) {
			cache->radius_cui = os_zalloc(len + 1);
			if (cache->radius_cui)
				os_memcpy(cache->radius_cui, buf, len);
		}

		if (hapd->conf->wpa_psk_radius == PSK_RADIUS_REQUIRED &&
		    !cache->psk)
			cache->accepted = HOSTAPD_ACL_REJECT;

		if (cache->vlan_id.notempty &&
		    !hostapd_vlan_valid(hapd->conf->vlan, &cache->vlan_id)) {
			hostapd_logger(hapd, query->addr,
				       HOSTAPD_MODULE_RADIUS,
				       HOSTAPD_LEVEL_INFO,
				       "Invalid VLAN %d%s received from RADIUS server",
				       cache->vlan_id.untagged,
				       cache->vlan_id.tagged[0] ? "+" : "");
			os_memset(&cache->vlan_id, 0, sizeof(cache->vlan_id));
		}
		if (hapd->conf->ssid.dynamic_vlan == DYNAMIC_VLAN_REQUIRED &&
		    !cache->vlan_id.notempty)
			cache->accepted = HOSTAPD_ACL_REJECT;
	} else
		cache->accepted = HOSTAPD_ACL_REJECT;
	cache->next = hapd->acl_cache;
	hapd->acl_cache = cache;

#ifdef CONFIG_DRIVER_RADIUS_ACL
	hostapd_drv_set_radius_acl_auth(hapd, query->addr, cache->accepted,
					cache->session_timeout);
#else /* CONFIG_DRIVER_RADIUS_ACL */
#ifdef NEED_AP_MLME
	/* Re-send original authentication frame for 802.11 processing */
	wpa_printf(MSG_DEBUG, "Re-sending authentication frame after "
		   "successful RADIUS ACL query");
	ieee802_11_mgmt(hapd, query->auth_msg, query->auth_msg_len, NULL);
#endif /* NEED_AP_MLME */
#endif /* CONFIG_DRIVER_RADIUS_ACL */

 done:
	if (prev == NULL)
		hapd->acl_queries = query->next;
	else
		prev->next = query->next;

	hostapd_acl_query_free(query);

	return RADIUS_RX_PROCESSED;
}
#endif /* CONFIG_NO_RADIUS */


/**
 * hostapd_acl_init: Initialize IEEE 802.11 ACL
 * @hapd: hostapd BSS data
 * Returns: 0 on success, -1 on failure
 */
int hostapd_acl_init(struct hostapd_data *hapd)
{
#ifndef CONFIG_NO_RADIUS
	if (radius_client_register(hapd->radius, RADIUS_AUTH,
				   hostapd_acl_recv_radius, hapd))
		return -1;
#endif /* CONFIG_NO_RADIUS */

	return 0;
}


/**
 * hostapd_acl_deinit - Deinitialize IEEE 802.11 ACL
 * @hapd: hostapd BSS data
 */
void hostapd_acl_deinit(struct hostapd_data *hapd)
{
	struct hostapd_acl_query_data *query, *prev;

#ifndef CONFIG_NO_RADIUS
	hostapd_acl_cache_free(hapd->acl_cache);
	hapd->acl_cache = NULL;
#endif /* CONFIG_NO_RADIUS */

	query = hapd->acl_queries;
	hapd->acl_queries = NULL;
	while (query) {
		prev = query;
		query = query->next;
		hostapd_acl_query_free(prev);
	}
}


void hostapd_free_psk_list(struct hostapd_sta_wpa_psk_short *psk)
{
	if (psk && psk->ref) {
		/* This will be freed when the last reference is dropped. */
		psk->ref--;
		return;
	}

	while (psk) {
		struct hostapd_sta_wpa_psk_short *prev = psk;
		psk = psk->next;
		os_free(prev);
	}
}

struct multi_ap_blacklist * ieee802_11_multi_ap_blacklist_find(struct hostapd_data *hapd, const u8 *addr)
{
	struct multi_ap_blacklist *iter;

	dl_list_for_each(iter, &hapd->multi_ap_blacklist, struct multi_ap_blacklist, list)
		if (os_memcmp(addr, iter->addr, ETH_ALEN) == 0)
			return iter;
	return NULL;
}

int hostapd_check_softblock(struct hostapd_data *hapd, const u8 *addr, u16 *status, u16 msgtype, int snr_db,
	struct intel_vendor_event_msg_drop *msg_dropped)
{
	errno_t err;
	int ret = HOSTAPD_SOFTBLOCK_ACCEPT;
	struct multi_ap_blacklist *found = NULL;

	found = ieee802_11_multi_ap_blacklist_find(hapd, addr);

	if (found) {
		struct sta_info *sta;
		sta = ap_get_sta(hapd, addr);
		if (sta && (sta->flags & WLAN_STA_AUTHORIZED)) {
			wpa_printf(MSG_DEBUG,
				"SoftBlock: station is connected, allow");
			return ret;
		}
		*status = found->status;
		if ((found->snrAuthHWM == 0) && (found->snrAuthLWM == 0)) {
			ret = HOSTAPD_MULTI_AP_BLACKLIST_FOUND;
		} else {
			/* Check if Softblock is disabled */
			if (hapd->conf->sDisableSoftblock) {
				wpa_printf(MSG_DEBUG,
					"softblock is disabled");
				return ret;
			}
			/* Check if the STA is candidate for softblock */
			switch(msgtype) {
				case WLAN_FC_STYPE_ASSOC_REQ:
					*status = WLAN_STATUS_SUCCESS;
					break; /* dont block on thresholds as of now */
				case WLAN_FC_STYPE_AUTH:
					if ((found->snrAuthHWM < snr_db) || (found->snrAuthLWM > snr_db)) {
						msg_dropped->blocked = true;
						msg_dropped->rx_snr = snr_db;
						msg_dropped->msgtype = WLAN_FC_STYPE_AUTH;
						err = memcpy_s(msg_dropped->addr, sizeof(msg_dropped->addr),
							 addr, ETH_ALEN);
						if (EOK != err) {
							wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
							msg_dropped->reason = 0;
							msg_dropped->rejected = false;
							return HOSTAPD_SOFTBLOCK_DROP;
						}
						msg_dropped->broadcast = 0;
						if (!found->status) {
							/* Silently Ignore the message from STA and
								consturct the msg to notify app of the drop event */
							wpa_printf(MSG_DEBUG, "Softblock: Auth Req dropped, snr_db = %d",
								snr_db);
							msg_dropped->reason = 0; 
							msg_dropped->rejected = false; /* Rejection not sent to STA */
							ret = HOSTAPD_SOFTBLOCK_DROP;
						}
						else {
							/* Notification will be done once the mesage is sent
								to STA after all the subsequent checks */
							wpa_printf(MSG_DEBUG, "Softblock: Auth Req reject, snr_db = %d",
								snr_db);
							msg_dropped->rejected = true; /* Rejection sent to STA */
							ret = HOSTAPD_SOFTBLOCK_REJECT;
						}
					} else {
						*status = WLAN_STATUS_SUCCESS;
						wpa_printf(MSG_DEBUG, "Softblock: Auth Req allowed, snr_db = %d",
							snr_db);
						ret = HOSTAPD_SOFTBLOCK_ALLOWED;
					}
					break;
				case WLAN_FC_STYPE_PROBE_REQ:
					msg_dropped->broadcast = false;
					msg_dropped->rx_snr = snr_db;
					msg_dropped->msgtype = WLAN_FC_STYPE_PROBE_REQ;
					msg_dropped->reason = 0;
					msg_dropped->rejected = 0;
					err = memcpy_s(msg_dropped->addr, sizeof(msg_dropped->addr),
						 addr, ETH_ALEN);
					if (EOK != err) {
						wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
						msg_dropped->reason = 0;
						msg_dropped->rejected = false;
						return HOSTAPD_SOFTBLOCK_DROP;
					}
					if ((found->snrProbeHWM < snr_db) || (found->snrProbeLWM > snr_db)) {
						/* Silently Ignore the message to STA and
							construct the msg to notify app of the drop event */
						wpa_printf(MSG_DEBUG, "SoftBlock: Probe Req dropped, snr_db = %d",
						   snr_db);
						msg_dropped->blocked = true;
						hostapd_event_ltq_softblock_drop(hapd, msg_dropped);
						ret = HOSTAPD_SOFTBLOCK_DROP;
					} else {
						wpa_printf(MSG_DEBUG, "SoftBlock: Probe Req allowed snr_db = %d",
						   snr_db);
						msg_dropped->blocked = false;
						*status = WLAN_STATUS_SUCCESS;
						ret = HOSTAPD_SOFTBLOCK_ALLOWED;
					}
					break;
			}
		}
	}

	return ret;
}

int ieee802_11_multi_ap_blacklist_add(struct hostapd_data *hapd,
	struct multi_ap_blacklist *entry)
{
	errno_t err;
	struct multi_ap_blacklist *found, *new_entry;

	found = ieee802_11_multi_ap_blacklist_find(hapd, entry->addr);

	if (found) {
		found->status = entry->status;
		found->snrProbeHWM = entry->snrProbeHWM;
		found->snrProbeLWM = entry->snrProbeLWM;
		found->snrAuthHWM = entry->snrAuthHWM;
		found->snrAuthLWM = entry->snrAuthLWM;
		return 0;
	}

	new_entry = os_malloc(sizeof(*entry));
	if (!new_entry) {
		wpa_printf(MSG_ERROR, "Failed to allocate memory for Multi-AP blacklist entry.");
		return -1;
	}
	err = memcpy_s(new_entry, sizeof(*new_entry), entry, sizeof(*entry));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		os_free(new_entry);
		return -1;
	}

	DL_LIST_ADD(&hapd->multi_ap_blacklist, new_entry, list);
	return 0;
}

void ieee802_11_multi_ap_blacklist_remove(struct hostapd_data *hapd,
	struct multi_ap_blacklist *entry)
{
	struct multi_ap_blacklist *found;

	found = ieee802_11_multi_ap_blacklist_find(hapd, entry->addr);
	if (found) {
		dl_list_del(&found->list);
		os_free(found);
	}
}

void ieee802_11_multi_ap_blacklist_flush(struct hostapd_data *hapd)
{
	struct multi_ap_blacklist *iter;

	while ((iter = dl_list_first(&hapd->multi_ap_blacklist, struct multi_ap_blacklist,
		list)) != NULL) {
		dl_list_del(&iter->list);
		os_free(iter);
	}
}

int ieee802_11_multi_ap_blacklist_print(struct hostapd_data *hapd, char *buf,
	size_t buflen)
{
	struct multi_ap_blacklist *iter;
	int ret = 0, len = 0;

	dl_list_for_each(iter, &hapd->multi_ap_blacklist, struct multi_ap_blacklist, list) {
		ret = sprintf_s(buf + len, buflen - len, MACSTR" %hu\n", MAC2STR(iter->addr),
			iter->status);
		if (ret <= 0)
			break;
		len += ret;
	}
	return len;
}

int ieee802_11_multi_ap_set_deny_mac(struct hostapd_data *hapd,
	struct multi_ap_blacklist* entry, const u8 remove)
{
	int ret = hostapd_drv_set_deny_mac(hapd, entry, remove);
	if (ret < 0)
		return ret;
	if (remove) {
		if (!is_zero_ether_addr(entry->addr))
			ieee802_11_multi_ap_blacklist_remove(hapd, entry);
		else
			ieee802_11_multi_ap_blacklist_flush(hapd);
	} else
		ret = ieee802_11_multi_ap_blacklist_add(hapd, entry);
	return ret;
}

int ieee802_11_multi_ap_set_softblock_thresholds(struct hostapd_data *hapd,
	struct multi_ap_blacklist* entry, const u8 remove)
{
	int ret;

	if (!is_valid_ether_addr(entry->addr)) {
		wpa_printf(MSG_DEBUG, "Softblock: Zero or multicast or broadcast eth addr not allowed");
		return -1;
	}

	ret = hostapd_drv_set_sta_softblock_thresholds(hapd, entry, remove);

	if (ret < 0)
		return ret;

	if (remove) {
		ieee802_11_multi_ap_blacklist_remove(hapd, entry);
	} else {
		ret = ieee802_11_multi_ap_blacklist_add(hapd, entry);
	}

	return ret;
}

/* Soft Block deny list logic */

static void hostapd_soft_block_acl_allow_time_eloop(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	const u8 *addr = timeout_ctx;
	struct soft_block_acl_entry *blocked;

	/* might be removed already */
	blocked = hostapd_soft_block_maclist_entry_get(&hapd->conf->deny_mac_sb, addr);
	if (!blocked)
		return;

	eloop_cancel_timeout(hostapd_soft_block_acl_allow_time_eloop, hapd, blocked->addr);
	/* block */
	blocked->allow_handler = NULL;

	wpa_printf(MSG_DEBUG, "SofBLock ACL: STA " MACSTR " ALLOW_TIME elapsed",
		MAC2STR(blocked->addr));
}

static void hostapd_soft_block_acl_wait_time_eloop(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	const u8 *addr = timeout_ctx;
	struct soft_block_acl_entry *blocked;

	/* might be removed already */
	blocked = hostapd_soft_block_maclist_entry_get(&hapd->conf->deny_mac_sb, addr);
	if (!blocked)
		return;

	/* unblock */
	eloop_cancel_timeout(hostapd_soft_block_acl_wait_time_eloop, hapd, blocked->addr);
	blocked->wait_handler = NULL;

	wpa_printf(MSG_DEBUG, "SofBLock ACL: STA " MACSTR " WAIT_TIME elapsed",
		MAC2STR(blocked->addr));

	/* no allow time means allow forever */
	if (0 >= hapd->conf->soft_block_acl_allow_time)
		return;

	blocked->allow_handler = hostapd_soft_block_acl_allow_time_eloop;
	blocked->eloop_ctx = hapd;
	blocked->user_ctx = blocked->addr;
	eloop_register_timeout(hapd->conf->soft_block_acl_allow_time / 1000u,
		(hapd->conf->soft_block_acl_allow_time % 1000u) * 1000u,
		hostapd_soft_block_acl_allow_time_eloop, hapd, blocked->addr);

	wpa_printf(MSG_DEBUG, "SofBLock ACL: STA " MACSTR " ALLOW_TIME started for %d ms",
		MAC2STR(blocked->addr), hapd->conf->soft_block_acl_allow_time);

}

static int hostapd_soft_block_acl_allow(struct hostapd_data *hapd, struct soft_block_acl_entry *entry)
{
	if (0 >= hapd->conf->soft_block_acl_wait_time)
		return 1;
	if (!entry)
		return 1;
	if (entry->allow_handler)
		return 1;

	return 0;
}

int hostapd_soft_block_acl_client_status(struct hostapd_data *hapd, const u8 *addr, u16 frame_type, u16 *status)
{
	struct soft_block_acl_entry *blocked;
	int res = SOFT_BLOCK_ACL_ALLOW;

	if (status)
		*status = WLAN_STATUS_SUCCESS;

	if (!hapd->conf->soft_block_acl_enable)
		goto end;

	if (frame_type == WLAN_FC_STYPE_PROBE_REQ) {
		if (hapd->conf->soft_block_acl_on_probe_req == SOFT_BLOCK_ON_PROBE_REQ_REJECT_ALL) {
			res = SOFT_BLOCK_ACL_DROP;
			goto end;
		}
	}

	blocked = hostapd_soft_block_maclist_entry_get(&hapd->conf->deny_mac_sb, addr);
	if (hostapd_soft_block_acl_allow(hapd, blocked)) {
		res = SOFT_BLOCK_ACL_ALLOW;
		goto end;
	}

	switch (frame_type) {
		case WLAN_FC_STYPE_PROBE_REQ:
			res = SOFT_BLOCK_ACL_DROP;
			if (hapd->conf->soft_block_acl_on_probe_req == SOFT_BLOCK_ON_PROBE_REQ_REPLY_ALL) {
				res = SOFT_BLOCK_ACL_ALLOW;
			}
		break;
		case WLAN_FC_STYPE_AUTH:
			res = SOFT_BLOCK_ACL_AUTH_RESP_FORCED;
			if (status)
				*status = hapd->conf->soft_block_acl_on_auth_req;

			if (hapd->conf->soft_block_acl_on_auth_req == SOFT_BLOCK_ON_AUTH_REQ_REJECT_ALL) {
				res = SOFT_BLOCK_ACL_DROP;
				if (status)
					*status = WLAN_STATUS_UNSPECIFIED_FAILURE;
			}
		break;
		default:
			res = SOFT_BLOCK_ACL_ALLOW;
			goto end;
	}

	if (!blocked->wait_handler) {
		/* shouldn't happen, but just in case: cancel eloop */
		if (eloop_is_timeout_registered(hostapd_soft_block_acl_wait_time_eloop, hapd, blocked->addr))
			eloop_cancel_timeout(hostapd_soft_block_acl_wait_time_eloop, hapd, blocked->addr);

		blocked->wait_handler = hostapd_soft_block_acl_wait_time_eloop;
		blocked->eloop_ctx = hapd;
		blocked->user_ctx = blocked->addr;
		eloop_register_timeout(hapd->conf->soft_block_acl_wait_time / 1000u,
			(hapd->conf->soft_block_acl_wait_time % 1000u) * 1000u,
			hostapd_soft_block_acl_wait_time_eloop, hapd, blocked->addr);
		wpa_printf(MSG_DEBUG, "SofBLock ACL: STA " MACSTR " WAIT_TIME started for %u ms",
			MAC2STR(blocked->addr), hapd->conf->soft_block_acl_wait_time);
	}

end:
	return res;
}
