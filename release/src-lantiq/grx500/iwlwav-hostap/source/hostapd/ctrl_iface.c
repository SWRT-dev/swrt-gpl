/*
 * hostapd / UNIX domain socket -based control interface
 * Copyright (c) 2004-2018, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#ifndef CONFIG_NATIVE_WINDOWS

#ifdef CONFIG_TESTING_OPTIONS
#ifdef __NetBSD__
#include <net/if_ether.h>
#else
#include <net/ethernet.h>
#endif
#include <netinet/ip.h>
#endif /* CONFIG_TESTING_OPTIONS */

#include <sys/un.h>
#include <sys/stat.h>
#include <stddef.h>

#ifdef CONFIG_CTRL_IFACE_UDP
#include <netdb.h>
#endif /* CONFIG_CTRL_IFACE_UDP */

#include "utils/common.h"
#include "utils/eloop.h"
#include "utils/module_tests.h"
#include "utils/crc32.h"
#include "common/wpa_ctrl.h"
#include "common/version.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "common/ctrl_iface_common.h"
#ifdef CONFIG_DPP
#include "common/dpp.h"
#endif /* CONFIG_DPP */
#include "common/wpa_ctrl.h"
#include "crypto/tls.h"
#include "drivers/driver.h"
#include "eapol_auth/eapol_auth_sm.h"
#include "radius/radius_client.h"
#include "radius/radius_server.h"
#include "l2_packet/l2_packet.h"
#include "ap/hostapd.h"
#include "ap/ap_config.h"
#include "ap/ieee802_1x.h"
#include "ap/wpa_auth.h"
#include "ap/ieee802_11.h"
#include "ap/sta_info.h"
#include "ap/wps_hostapd.h"
#include "ap/ctrl_iface_ap.h"
#include "ap/ap_drv_ops.h"
#include "ap/hs20.h"
#include "ap/wnm_ap.h"
#include "ap/wpa_auth.h"
#include "ap/acs.h"
#include "ap/dfs.h"
#include "ap/bss_load.h"
#include "ap/hw_features.h"
#include "ap/beacon.h"
#include "ap/neighbor_db.h"
#include "ap/rrm.h"
#include "ap/dpp_hostapd.h"
#include "wps/wps_defs.h"
#include "wps/wps.h"
#include "fst/fst_ctrl_iface.h"
#include "config_file.h"
#include "ctrl_iface.h"
#include "drivers/nl80211_copy.h"
#include "ap/atf.h"
#include "ap/ieee802_11_auth.h"
#include "ap/wpa_auth_i.h"

#define HOSTAPD_CLI_DUP_VALUE_MAX_LEN 256

#ifdef CONFIG_CTRL_IFACE_UDP
#define HOSTAPD_CTRL_IFACE_PORT		8877
#define HOSTAPD_CTRL_IFACE_PORT_LIMIT	50
#define HOSTAPD_GLOBAL_CTRL_IFACE_PORT		8878
#define HOSTAPD_GLOBAL_CTRL_IFACE_PORT_LIMIT	50
#endif /* CONFIG_CTRL_IFACE_UDP */

static void hostapd_ctrl_iface_send(struct hostapd_data *hapd, int level,
				    enum wpa_msg_type type,
				    const char *buf, size_t len);
static int hostapd_ctrl_iface_disable(struct hostapd_iface *iface);


static int hostapd_ctrl_iface_attach(struct hostapd_data *hapd,
				     struct sockaddr_storage *from,
				     socklen_t fromlen, const char *input)
{
	return ctrl_iface_attach(&hapd->ctrl_dst, from, fromlen, input);
}


static int hostapd_ctrl_iface_detach(struct hostapd_data *hapd,
				     struct sockaddr_storage *from,
				     socklen_t fromlen)
{
	return ctrl_iface_detach(&hapd->ctrl_dst, from, fromlen);
}


static int hostapd_ctrl_iface_level(struct hostapd_data *hapd,
				    struct sockaddr_storage *from,
				    socklen_t fromlen,
				    char *level)
{
	return ctrl_iface_level(&hapd->ctrl_dst, from, fromlen, level);
}


static int hostapd_ctrl_iface_new_sta(struct hostapd_data *hapd,
				      const char *txtaddr)
{
	u8 addr[ETH_ALEN];
	struct sta_info *sta;

	wpa_printf(MSG_DEBUG, "CTRL_IFACE NEW_STA %s", txtaddr);

	if (hwaddr_aton(txtaddr, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (sta)
		return 0;

	wpa_printf(MSG_DEBUG, "Add new STA " MACSTR " based on ctrl_iface "
		   "notification", MAC2STR(addr));
	sta = ap_sta_add(hapd, addr);
	if (sta == NULL)
		return -1;

	hostapd_new_assoc_sta(hapd, sta, 0);
	return 0;
}


#ifdef NEED_AP_MLME
static int hostapd_ctrl_iface_sa_query(struct hostapd_data *hapd,
				       const char *txtaddr)
{
	u8 addr[ETH_ALEN];
	u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];

	wpa_printf(MSG_DEBUG, "CTRL_IFACE SA_QUERY %s", txtaddr);

	if (hwaddr_aton(txtaddr, addr) ||
	    os_get_random(trans_id, WLAN_SA_QUERY_TR_ID_LEN) < 0)
		return -1;

	ieee802_11_send_sa_query_req(hapd, addr, trans_id);

	return 0;
}
#endif /* NEED_AP_MLME */


#ifdef CONFIG_WPS
static int hostapd_ctrl_iface_wps_pin(struct hostapd_data *hapd, char *txt)
{
	char *pin = os_strchr(txt, ' ');
	char *timeout_txt;
	int timeout, i, len;
	u8 addr_buf[ETH_ALEN], *addr = NULL;
	char *pos;
	char *pin_src, *pin_dst;

	if (pin == NULL)
		return -1;
	*pin++ = '\0';

	/* remove dash or space from pin (nnnn-nnnn or nnnn nnnn) */
	pin_src = pin_dst = pin;
	i = 0;
	len = os_strlen(pin);
	while ((*pin_src != ' ' || i == 4) && i < len) {
		if (!(i== 4 && (*pin_src == '-' || *pin_src == ' '))) {
			*pin_dst++ = *pin_src;
			i++;
		}
		pin_src++;
	}

	if (pin_dst != pin_src)
		*pin_dst++ = '\0';

	timeout_txt = os_strchr(pin_dst, ' ');
	if (timeout_txt) {
		*timeout_txt++ = '\0';
		timeout = atoi(timeout_txt);
		if (!IS_VALID_RANGE(timeout, AP_TIMER_DISABLE, AP_TIMER_MAX)) {
			wpa_printf(MSG_ERROR, "invalid timeout value %d for WPS_PIN", timeout);
			return -1;
		}
		pos = os_strchr(timeout_txt, ' ');
		if (pos) {
			*pos++ = '\0';
			if (hwaddr_aton(pos, addr_buf) == 0)
				addr = addr_buf;
		}
	} else
		timeout = 0;

	return hostapd_wps_add_pin(hapd, addr, txt, pin, timeout);
}


static int hostapd_ctrl_iface_wps_check_pin(
	struct hostapd_data *hapd, char *cmd, char *buf, size_t buflen)
{
	char pin[9];
	size_t len;
	char *pos;
	int ret;

	wpa_hexdump_ascii_key(MSG_DEBUG, "WPS_CHECK_PIN",
			      (u8 *) cmd, os_strlen(cmd));
	for (pos = cmd, len = 0; *pos != '\0'; pos++) {
		if (*pos < '0' || *pos > '9')
			continue;
		pin[len++] = *pos;
		if (len == 9) {
			wpa_printf(MSG_DEBUG, "WPS: Too long PIN");
			return -1;
		}
	}
	if (len != 4 && len != 8) {
		wpa_printf(MSG_DEBUG, "WPS: Invalid PIN length %d", (int) len);
		return -1;
	}
	pin[len] = '\0';

	if (len == 8) {
		unsigned int pin_val;
		pin_val = atoi(pin);
		//Fix for Klocwork issue: SV.TAINTED.CALL.LOOP_BOUND (2:Error)
		//Validating "pin_val",it must be an 8 digit integer.
		if(pin_val > 99999999)
			return -1;

		if (!wps_pin_valid(pin_val)) {
			wpa_printf(MSG_DEBUG, "WPS: Invalid checksum digit");
			ret = os_snprintf(buf, buflen, "FAIL-CHECKSUM\n");
			if (os_snprintf_error(buflen, ret))
				return -1;
			return ret;
		}
	}

	ret = os_snprintf(buf, buflen, "%s", pin);
	if (os_snprintf_error(buflen, ret))
		return -1;

	return ret;
}


#ifdef CONFIG_WPS_NFC
static int hostapd_ctrl_iface_wps_nfc_tag_read(struct hostapd_data *hapd,
					       char *pos)
{
	size_t len;
	struct wpabuf *buf;
	int ret;

	len = os_strlen(pos);
	if (len & 0x01)
		return -1;
	len /= 2;

	buf = wpabuf_alloc(len);
	if (buf == NULL)
		return -1;
	if (hexstr2bin(pos, wpabuf_put(buf, len), len) < 0) {
		wpabuf_free(buf);
		return -1;
	}

	ret = hostapd_wps_nfc_tag_read(hapd, buf);
	wpabuf_free(buf);

	return ret;
}


static int hostapd_ctrl_iface_wps_nfc_config_token(struct hostapd_data *hapd,
						   char *cmd, char *reply,
						   size_t max_len)
{
	int ndef;
	struct wpabuf *buf;
	int res;

	if (os_strcmp(cmd, "WPS") == 0)
		ndef = 0;
	else if (os_strcmp(cmd, "NDEF") == 0)
		ndef = 1;
	else
		return -1;

	buf = hostapd_wps_nfc_config_token(hapd, ndef);
	if (buf == NULL)
		return -1;

	res = wpa_snprintf_hex_uppercase(reply, max_len, wpabuf_head(buf),
					 wpabuf_len(buf));
	reply[res++] = '\n';
	reply[res] = '\0';

	wpabuf_free(buf);

	return res;
}


static int hostapd_ctrl_iface_wps_nfc_token_gen(struct hostapd_data *hapd,
						char *reply, size_t max_len,
						int ndef)
{
	struct wpabuf *buf;
	int res;

	buf = hostapd_wps_nfc_token_gen(hapd, ndef);
	if (buf == NULL)
		return -1;

	res = wpa_snprintf_hex_uppercase(reply, max_len, wpabuf_head(buf),
					 wpabuf_len(buf));
	reply[res++] = '\n';
	reply[res] = '\0';

	wpabuf_free(buf);

	return res;
}


static int hostapd_ctrl_iface_wps_nfc_token(struct hostapd_data *hapd,
					    char *cmd, char *reply,
					    size_t max_len)
{
	if (os_strcmp(cmd, "WPS") == 0)
		return hostapd_ctrl_iface_wps_nfc_token_gen(hapd, reply,
							    max_len, 0);

	if (os_strcmp(cmd, "NDEF") == 0)
		return hostapd_ctrl_iface_wps_nfc_token_gen(hapd, reply,
							    max_len, 1);

	if (os_strcmp(cmd, "enable") == 0)
		return hostapd_wps_nfc_token_enable(hapd);

	if (os_strcmp(cmd, "disable") == 0) {
		hostapd_wps_nfc_token_disable(hapd);
		return 0;
	}

	return -1;
}


static int hostapd_ctrl_iface_nfc_get_handover_sel(struct hostapd_data *hapd,
						   char *cmd, char *reply,
						   size_t max_len)
{
	struct wpabuf *buf;
	int res;
	char *pos;
	int ndef;

	pos = os_strchr(cmd, ' ');
	if (pos == NULL)
		return -1;
	*pos++ = '\0';

	if (os_strcmp(cmd, "WPS") == 0)
		ndef = 0;
	else if (os_strcmp(cmd, "NDEF") == 0)
		ndef = 1;
	else
		return -1;

	if (os_strcmp(pos, "WPS-CR") == 0)
		buf = hostapd_wps_nfc_hs_cr(hapd, ndef);
	else
		buf = NULL;
	if (buf == NULL)
		return -1;

	res = wpa_snprintf_hex_uppercase(reply, max_len, wpabuf_head(buf),
					 wpabuf_len(buf));
	reply[res++] = '\n';
	reply[res] = '\0';

	wpabuf_free(buf);

	return res;
}


static int hostapd_ctrl_iface_nfc_report_handover(struct hostapd_data *hapd,
						  char *cmd)
{
	size_t len;
	struct wpabuf *req, *sel;
	int ret;
	char *pos, *role, *type, *pos2;

	role = cmd;
	pos = os_strchr(role, ' ');
	if (pos == NULL)
		return -1;
	*pos++ = '\0';

	type = pos;
	pos = os_strchr(type, ' ');
	if (pos == NULL)
		return -1;
	*pos++ = '\0';

	pos2 = os_strchr(pos, ' ');
	if (pos2 == NULL)
		return -1;
	*pos2++ = '\0';

	len = os_strlen(pos);
	if (len & 0x01)
		return -1;
	len /= 2;

	req = wpabuf_alloc(len);
	if (req == NULL)
		return -1;
	if (hexstr2bin(pos, wpabuf_put(req, len), len) < 0) {
		wpabuf_free(req);
		return -1;
	}

	len = os_strlen(pos2);
	if (len & 0x01) {
		wpabuf_free(req);
		return -1;
	}
	len /= 2;

	sel = wpabuf_alloc(len);
	if (sel == NULL) {
		wpabuf_free(req);
		return -1;
	}
	if (hexstr2bin(pos2, wpabuf_put(sel, len), len) < 0) {
		wpabuf_free(req);
		wpabuf_free(sel);
		return -1;
	}

	if (os_strcmp(role, "RESP") == 0 && os_strcmp(type, "WPS") == 0) {
		ret = hostapd_wps_nfc_report_handover(hapd, req, sel);
	} else {
		wpa_printf(MSG_DEBUG, "NFC: Unsupported connection handover "
			   "reported: role=%s type=%s", role, type);
		ret = -1;
	}
	wpabuf_free(req);
	wpabuf_free(sel);

	return ret;
}

#endif /* CONFIG_WPS_NFC */


static int hostapd_ctrl_iface_wps_ap_pin(struct hostapd_data *hapd, char *txt,
					 char *buf, size_t buflen)
{
	int timeout = 300;
	char *pos;
	const char *pin_txt;

	pos = os_strchr(txt, ' ');
	if (pos)
		*pos++ = '\0';

	if (os_strcmp(txt, "disable") == 0) {
		hostapd_wps_ap_pin_disable(hapd);
		return os_snprintf(buf, buflen, "OK\n");
	}

	if (os_strcmp(txt, "random") == 0) {
		if (pos)
			timeout = atoi(pos);
		pin_txt = hostapd_wps_ap_pin_random(hapd, timeout);
		if (pin_txt == NULL)
			return -1;
		return os_snprintf(buf, buflen, "%s", pin_txt);
	}

	if (os_strcmp(txt, "get") == 0) {
		pin_txt = hostapd_wps_ap_pin_get(hapd);
		if (pin_txt == NULL)
			return -1;
		return os_snprintf(buf, buflen, "%s", pin_txt);
	}

	if (os_strcmp(txt, "set") == 0) {
		char *pin;
		if (pos == NULL)
			return -1;
		pin = pos;
		pos = os_strchr(pos, ' ');
		if (pos) {
			*pos++ = '\0';
			timeout = atoi(pos);
			if (!IS_VALID_RANGE(timeout, AP_TIMER_MIN, AP_TIMER_MAX)) {
				wpa_printf(MSG_ERROR, "invalid timeout value %d for WPS_AP_PIN", timeout);
				return -1;
			}
		}
		if (os_strlen(pin) > buflen)
			return -1;
		if (hostapd_wps_ap_pin_set(hapd, pin, timeout) < 0)
			return -1;
		return os_snprintf(buf, buflen, "%s", pin);
	}

	return -1;
}


static int hostapd_ctrl_iface_wps_config(struct hostapd_data *hapd, char *txt)
{
	char *pos;
	char *ssid, *auth, *encr = NULL, *key = NULL;

	ssid = txt;
	pos = os_strchr(txt, ' ');
	if (!pos)
		return -1;
	*pos++ = '\0';

	auth = pos;
	pos = os_strchr(pos, ' ');
	if (pos) {
		*pos++ = '\0';
		encr = pos;
		pos = os_strchr(pos, ' ');
		if (pos) {
			*pos++ = '\0';
			key = pos;
		}
	}

	return hostapd_wps_config_ap(hapd, ssid, auth, encr, key);
}


static const char * pbc_status_str(enum pbc_status status)
{
	switch (status) {
	case WPS_PBC_STATUS_DISABLE:
		return "Disabled";
	case WPS_PBC_STATUS_ACTIVE:
		return "Active";
	case WPS_PBC_STATUS_TIMEOUT:
		return "Timed-out";
	case WPS_PBC_STATUS_OVERLAP:
		return "Overlap";
	default:
		return "Unknown";
	}
}


static int hostapd_ctrl_iface_wps_get_status(struct hostapd_data *hapd,
					     char *buf, size_t buflen)
{
	int ret;
	char *pos, *end;

	pos = buf;
	end = buf + buflen;

	ret = os_snprintf(pos, end - pos, "PBC Status: %s\n",
			  pbc_status_str(hapd->wps_stats.pbc_status));

	if (os_snprintf_error(end - pos, ret))
		return pos - buf;
	pos += ret;

	ret = os_snprintf(pos, end - pos, "Last WPS result: %s\n",
			  (hapd->wps_stats.status == WPS_STATUS_SUCCESS ?
			   "Success":
			   (hapd->wps_stats.status == WPS_STATUS_FAILURE ?
			    "Failed" : "None")));

	if (os_snprintf_error(end - pos, ret))
		return pos - buf;
	pos += ret;

	/* If status == Failure - Add possible Reasons */
	if(hapd->wps_stats.status == WPS_STATUS_FAILURE &&
	   hapd->wps_stats.failure_reason > 0) {
		ret = os_snprintf(pos, end - pos,
				  "Failure Reason: %s\n",
				  wps_ei_str(hapd->wps_stats.failure_reason));

		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}

	if (hapd->wps_stats.status) {
		ret = os_snprintf(pos, end - pos, "Peer Address: " MACSTR "\n",
				  MAC2STR(hapd->wps_stats.peer_addr));

		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}

	return pos - buf;
}

#endif /* CONFIG_WPS */

#ifdef CONFIG_HS20

static int hostapd_ctrl_iface_hs20_wnm_notif(struct hostapd_data *hapd,
					     const char *cmd)
{
	u8 addr[ETH_ALEN];
	const char *url;

	hapd = get_bss_index(cmd, hapd->iface);
	if (NULL == hapd) {
		return -1;
	}

	cmd = os_strchr(cmd, ' ');
	if (NULL == cmd) {
		return -1;
	}
	cmd++;

	if (hwaddr_aton(cmd, addr))
		return -1;
	url = cmd + 17;
	if (*url == '\0') {
		url = NULL;
	} else {
		if (*url != ' ')
			return -1;
		url++;
		if (*url == '\0')
			url = NULL;
	}

	return hs20_send_wnm_notification(hapd, addr, 1, url);
}


static int hostapd_ctrl_iface_hs20_deauth_req(struct hostapd_data *hapd,
					      const char *cmd)
{
	u8 addr[ETH_ALEN];
	int code, reauth_delay, ret, val;
	const char *pos;
	size_t url_len;
	struct wpabuf *req;

	hapd = get_bss_index(cmd, hapd->iface);
	if (NULL == hapd) {
		return -1;
	}

	cmd = os_strchr(cmd, ' ');
	if (NULL == cmd) {
		return -1;
	}
	cmd++;

	/* <STA MAC Addr> <Code(0/1)> <Re-auth-Delay(sec)> [URL] */
	if (hwaddr_aton(cmd, addr))
		return -1;

	pos = os_strchr(cmd, ' ');
	if (pos == NULL)
		return -1;
	pos++;
	val = atoi(pos);
	if (!IS_VALID_BOOL_RANGE(val)) {
		wpa_printf(MSG_ERROR, "invalid hostapd_ctrl_iface_hs20_deauth_req code %d", val);
		return 1;
	}
	code = val;

	pos = os_strchr(pos, ' ');
	if (pos == NULL)
		return -1;
	pos++;
	reauth_delay = atoi(pos);
	if (!IS_VALID_RANGE(reauth_delay, AP_TIMER_MIN, AP_TIMER_MAX)) {
		wpa_printf(MSG_ERROR, "invalid reauth_delay value %d for HS20_DEAUTH_REQ", reauth_delay);
		return -1;
	}

	url_len = 0;
	pos = os_strchr(pos, ' ');
	if (pos) {
		pos++;
		url_len = os_strlen(pos);
	}

	req = wpabuf_alloc(4 + url_len);
	if (req == NULL)
		return -1;
	wpabuf_put_u8(req, code);
	wpabuf_put_le16(req, reauth_delay);
	wpabuf_put_u8(req, url_len);
	if (pos)
		wpabuf_put_data(req, pos, url_len);

	wpa_printf(MSG_DEBUG, "HS 2.0: Send WNM-Notification to " MACSTR
		   " to indicate imminent deauthentication (code=%d "
		   "reauth_delay=%d)", MAC2STR(addr), code, reauth_delay);
	ret = hs20_send_wnm_notification_deauth_req(hapd, addr, req);
	wpabuf_free(req);
	return ret;
}

#endif /* CONFIG_HS20 */

static int hostapd_ctrl_iface_update_wan_metrics(struct hostapd_data *hapd,
						const char *cmd)
{
	char buf[128];

	hapd = get_bss_index(cmd, hapd->iface);
	if (NULL == hapd) {
		return -1;
	}

	cmd = os_strchr(cmd, ' ');
	if (NULL == cmd) {
		return -1;
	}
	cmd++;

	os_strlcpy(buf, cmd, 128);
	wpa_printf(MSG_DEBUG, "hostapd_ctrl_iface_update_wan_metrics, bss=%s", hapd->conf->iface);

	if (hs20_parse_wan_metrics(hapd->conf, buf, 0) != 0)
		return -1;

	return 0;
}

#ifdef CONFIG_INTERWORKING

static int hostapd_ctrl_iface_set_qos_map_set(struct hostapd_data *hapd,
					      const char *cmd)
{
	u8 qos_map_set[16 + 2 * 21], count = 0;
	const char *pos = cmd;
	int val, ret;

	hapd = get_bss_index(pos, hapd->iface);
	if (NULL == hapd) {
		return -1;
	}

	pos = os_strchr(pos, ' ');
	if (NULL == pos) {
		return -1;
	}
	pos++;

	for (;;) {
		if (count == sizeof(qos_map_set)) {
			wpa_printf(MSG_ERROR, "Too many qos_map_set parameters");
			return -1;
		}

		val = atoi(pos);
		if (val < 0 || val > 255) {
			wpa_printf(MSG_INFO, "Invalid QoS Map Set");
			return -1;
		}

		qos_map_set[count++] = val;
		pos = os_strchr(pos, ',');
		if (!pos)
			break;
		pos++;
	}

	if (count < 16 || count & 1) {
		wpa_printf(MSG_INFO, "Invalid QoS Map Set");
		return -1;
	}

	ret = hostapd_drv_set_qos_map(hapd, qos_map_set, count);
	if (ret) {
		wpa_printf(MSG_INFO, "Failed to set QoS Map Set");
		return -1;
	}

	os_memcpy(hapd->conf->qos_map_set, qos_map_set, count);
	hapd->conf->qos_map_set_len = count;

	return 0;
}


static int hostapd_ctrl_iface_send_qos_map_conf(struct hostapd_data *hapd,
						const char *cmd)
{
	u8 addr[ETH_ALEN];
	struct sta_info *sta;
	struct wpabuf *buf;
	u8 *qos_map_set = hapd->conf->qos_map_set;
	u8 qos_map_set_len = hapd->conf->qos_map_set_len;
	int ret;

	hapd = get_bss_index(cmd, hapd->iface);
	if (NULL == hapd) {
		return -1;
	}

	cmd = os_strchr(cmd, ' ');
	if (NULL == cmd) {
		return -1;
	}
	cmd++;

	if (!qos_map_set_len) {
		wpa_printf(MSG_INFO, "QoS Map Set is not set");
		return -1;
	}

	if (hwaddr_aton(cmd, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (sta == NULL) {
		wpa_printf(MSG_DEBUG, "Station " MACSTR " not found "
			   "for QoS Map Configuration message",
			   MAC2STR(addr));
		return -1;
	}

	if (!sta->qos_map_enabled) {
		wpa_printf(MSG_DEBUG, "Station " MACSTR " did not indicate "
			   "support for QoS Map", MAC2STR(addr));
		return -1;
	}

	buf = wpabuf_alloc(2 + 2 + qos_map_set_len);
	if (buf == NULL)
		return -1;

	wpabuf_put_u8(buf, WLAN_ACTION_QOS);
	wpabuf_put_u8(buf, QOS_QOS_MAP_CONFIG);

	/* QoS Map Set Element */
	wpabuf_put_u8(buf, WLAN_EID_QOS_MAP_SET);
	wpabuf_put_u8(buf, qos_map_set_len);
	wpabuf_put_data(buf, qos_map_set, qos_map_set_len);

	ret = hostapd_drv_send_action(hapd, hapd->iface->freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));
	wpabuf_free(buf);

	return ret;
}

#endif /* CONFIG_INTERWORKING */


#ifdef CONFIG_WNM_AP

static int hostapd_ctrl_iface_disassoc_imminent(struct hostapd_data *hapd,
						const char *cmd)
{
	u8 addr[ETH_ALEN];
	int disassoc_timer;
	struct sta_info *sta;

	if (hwaddr_aton(cmd, addr))
		return -1;
	if (cmd[17] != ' ')
		return -1;
	disassoc_timer = atoi(cmd + 17);
	if (!IS_VALID_RANGE(disassoc_timer, AP_TIMER_MIN, AP_TIMER_MAX)) {
		wpa_printf(MSG_ERROR, "invalid disassoc_timer value %d for DISASSOC_IMMINENT", disassoc_timer);
		return -1;
	}

	sta = ap_get_sta(hapd, addr);
	if (sta == NULL) {
		wpa_printf(MSG_DEBUG, "Station " MACSTR
			   " not found for disassociation imminent message",
			   MAC2STR(addr));
		return -1;
	}

	return wnm_send_disassoc_imminent(hapd, sta, disassoc_timer);
}


static int hostapd_ctrl_iface_ess_disassoc(struct hostapd_data *hapd,
					   const char *cmd)
{
	u8 addr[ETH_ALEN];
	const char *url, *timerstr;
	int disassoc_timer;
	struct sta_info *sta;

	if (hwaddr_aton(cmd, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (sta == NULL) {
		wpa_printf(MSG_DEBUG, "Station " MACSTR
			   " not found for ESS disassociation imminent message",
			   MAC2STR(addr));
		return -1;
	}

	timerstr = cmd + 17;
	if (*timerstr != ' ')
		return -1;
	timerstr++;
	disassoc_timer = atoi(timerstr);
	if (disassoc_timer < 0 || disassoc_timer > 65535)
		return -1;

	url = os_strchr(timerstr, ' ');
	if (url == NULL)
		return -1;
	url++;

	return wnm_send_ess_disassoc_imminent(hapd, sta, url, disassoc_timer);
}


static int hostapd_ctrl_iface_bss_tm_req(struct hostapd_data *hapd,
					 const char *cmd, char *buf, size_t buflen)
{
	u8 addr[ETH_ALEN];
	const char *pos, *end;
	int disassoc_timer = 0;
	struct sta_info *sta = NULL;
	u8 req_mode = 0, valid_int = 0x01;
	u8 bss_term_dur[12];
	char *url = NULL;
	int i, ret;
	u8 nei_rep[1000];
	int nei_len;
	u8 mbo[10];
	size_t mbo_len = 0;
	int dialog_token_to_send = 0, ret_dialog_token = -1;

	if (hwaddr_aton(cmd, addr)) {
		wpa_printf(MSG_DEBUG, "Invalid STA MAC address");
		return -1;
	}

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta) {
		wpa_printf(MSG_DEBUG, "Station " MACSTR
			" not found for BSS TM Request message", MAC2STR(addr));
		return -1;
	}

	pos = os_strstr(cmd, " dialog_token=");
	if (pos) {
		pos += 14;
		dialog_token_to_send = atoi(pos);
		if (dialog_token_to_send < 0 || dialog_token_to_send > 0xff) {
			wpa_printf(MSG_DEBUG, "Invalid dialog_token");
			return -1;
		}
	}

	pos = os_strstr(cmd, " disassoc_timer=");
	if (pos) {
		pos += 16;
		disassoc_timer = atoi(pos);
		if (disassoc_timer < 0 || disassoc_timer > 65535) {
			wpa_printf(MSG_DEBUG, "Invalid disassoc_timer");
			return -1;
		}
	}

	pos = os_strstr(cmd, " valid_int=");
	if (pos) {
		pos += 11;
		valid_int = atoi(pos);
	}

	pos = os_strstr(cmd, " bss_term=");
	if (pos) {
		pos += 10;
		req_mode |= WNM_BSS_TM_REQ_BSS_TERMINATION_INCLUDED;
		/* TODO: TSF configurable/learnable */
		bss_term_dur[0] = 4; /* Subelement ID */
		bss_term_dur[1] = 10; /* Length */

		/* BSS Termination TSF (value of the TSF timer when BSS termination will occur) */
		errno = 0;
		u64 tsf = strtoull (pos, NULL, 10);
		if (errno) {
			wpa_printf(MSG_DEBUG, "Invalid TSF value in bss_term data");
			return -1;
		}
		WPA_PUT_LE64(&bss_term_dur[2], tsf);

		/* Duration (unsigned 2-octet integer that indicates the number of minutes for which the BSS is not present). */
		end = os_strchr(pos, ',');
		if (end == NULL) {
			wpa_printf(MSG_DEBUG, "Invalid bss_term data");
			return -1;
		}
		end++;
		WPA_PUT_LE16(&bss_term_dur[10], (short)atoi(end));
	}

	nei_len = ieee802_11_parse_candidate_list(cmd, nei_rep,
						  sizeof(nei_rep));
	if (nei_len < 0)
		return -1;

	pos = os_strstr(cmd, " url=");
	if (pos) {
		size_t len;
		pos += 5;
		end = os_strchr(pos, ' ');
		if (end)
			len = end - pos;
		else
			len = os_strlen(pos);
		url = os_malloc(len + 1);
		if (url == NULL)
			return -1;
		os_memcpy(url, pos, len);
		url[len] = '\0';
		req_mode |= WNM_BSS_TM_REQ_ESS_DISASSOC_IMMINENT;
	}

	if (os_strstr(cmd, " pref=1"))
		req_mode |= WNM_BSS_TM_REQ_PREF_CAND_LIST_INCLUDED;
	if (os_strstr(cmd, " abridged=1"))
		req_mode |= WNM_BSS_TM_REQ_ABRIDGED;
	if (os_strstr(cmd, " disassoc_imminent=1"))
		req_mode |= WNM_BSS_TM_REQ_DISASSOC_IMMINENT;

#ifdef CONFIG_MBO
	pos = os_strstr(cmd, "mbo=");
	if (pos) {
		if (hapd->conf->mbo_enabled && sta->mbo_supported) {
			unsigned int mbo_reason, reassoc_delay;
			int cell_pref;
			u8 *mbo_pos = mbo;

			ret = sscanf(pos, "mbo=%u:%u:%d", &mbo_reason,
					 &reassoc_delay, &cell_pref);
			if (ret < 3) {
				wpa_printf(MSG_DEBUG,
					   "BTM request: MBO requires three arguments: mbo=<reason>:<reassoc_delay>:<cell_pref>");
				ret = -1;
				goto fail;
			}

			if (mbo_reason > MBO_TRANSITION_REASON_PREMIUM_AP) {
				wpa_printf(MSG_DEBUG,
						"BTM request: Invalid MBO transition reason code %u",
						mbo_reason);
				ret = -1;
				goto fail;
			}

			/* Valid values for Cellular preference are: 0, 1, 255 */
			/* A value of -1 indicates that Cellular Data Connection Preference Attribute will not be added to MBO IE */
			if (cell_pref != -1 && cell_pref != MBO_CELL_PREF_EXCLUDED &&
				cell_pref != MBO_CELL_PREF_NO_USE && cell_pref != MBO_CELL_PREF_USE) {
				wpa_printf(MSG_DEBUG,
						"BTM request:Invalid MBO cellular capability %u",
						cell_pref);
				ret = -1;
				goto fail;
			}

			if (reassoc_delay > 65535 ||
			   (reassoc_delay &&
			   !(req_mode & WNM_BSS_TM_REQ_DISASSOC_IMMINENT))) {
				wpa_printf(MSG_DEBUG,
						"BTM request (MBO): Assoc retry delay is only valid in disassoc imminent mode");
				ret = -1;
				goto fail;
			}

			*mbo_pos++ = MBO_ATTR_ID_TRANSITION_REASON;
			*mbo_pos++ = 1;
			*mbo_pos++ = mbo_reason;
			if (cell_pref != -1) {
				if (!hapd->conf->mbo_cell_aware) {
					wpa_printf(MSG_DEBUG, "BTM request (MBO) Cell preference set, but VAP is not cellular aware");
				} else {
					*mbo_pos++ = MBO_ATTR_ID_CELL_DATA_PREF;
					*mbo_pos++ = 1;
					*mbo_pos++ = cell_pref;
				}
			}

			if (reassoc_delay) {
				*mbo_pos++ = MBO_ATTR_ID_ASSOC_RETRY_DELAY;
				*mbo_pos++ = 2;
				WPA_PUT_LE16(mbo_pos, reassoc_delay);
				mbo_pos += 2;
			}

			mbo_len = mbo_pos - mbo;
		} else {
			wpa_printf(MSG_DEBUG,
					"BTM request: MBO set but VAP is not MBO enabled or STA does not support MBO.");
		}
	}
#endif /* CONFIG_MBO */

	ret_dialog_token = wnm_send_bss_tm_req(hapd, sta, dialog_token_to_send,
			req_mode, disassoc_timer, valid_int, bss_term_dur, url,
			nei_len ? nei_rep : NULL, nei_len,
			mbo_len ? mbo : NULL, mbo_len);

#ifdef CONFIG_MBO
fail:
#endif /* CONFIG_MBO */

	os_free(url);

	if (ret_dialog_token < 0)
		return -1;
	return sprintf_s(buf, buflen, "dialog_token=%d\n", ret_dialog_token);
}


static int hostapd_ctrl_iface_coloc_intf_req(struct hostapd_data *hapd,
					     const char *cmd)
{
	u8 addr[ETH_ALEN];
	struct sta_info *sta;
	const char *pos;
	unsigned int auto_report, timeout;

	if (hwaddr_aton(cmd, addr)) {
		wpa_printf(MSG_DEBUG, "Invalid STA MAC address");
		return -1;
	}

	sta = ap_get_sta(hapd, addr);
	if (!sta) {
		wpa_printf(MSG_DEBUG, "Station " MACSTR
			   " not found for Collocated Interference Request",
			   MAC2STR(addr));
		return -1;
	}

	pos = cmd + 17;
	if (*pos != ' ')
		return -1;
	pos++;
	auto_report = atoi(pos);
	pos = os_strchr(pos, ' ');
	if (!pos)
		return -1;
	pos++;
	timeout = atoi(pos);

	return wnm_send_coloc_intf_req(hapd, sta, auto_report, timeout);
}

#endif /* CONFIG_WNM_AP */


static int hostapd_ctrl_iface_get_key_mgmt(struct hostapd_data *hapd,
					   char *buf, size_t buflen)
{
	int ret = 0;
	char *pos, *end;

	pos = buf;
	end = buf + buflen;

	WPA_ASSERT(hapd->conf->wpa_key_mgmt);

	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_PSK) {
		ret = os_snprintf(pos, end - pos, "WPA-PSK ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
		ret = os_snprintf(pos, end - pos, "WPA-EAP ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#ifdef CONFIG_IEEE80211R_AP
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_PSK) {
		ret = os_snprintf(pos, end - pos, "FT-PSK ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_IEEE8021X) {
		ret = os_snprintf(pos, end - pos, "FT-EAP ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#ifdef CONFIG_SHA384
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_IEEE8021X_SHA384) {
		ret = os_snprintf(pos, end - pos, "FT-EAP-SHA384 ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#endif /* CONFIG_SHA384 */
#ifdef CONFIG_SAE
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_SAE) {
		ret = os_snprintf(pos, end - pos, "FT-SAE ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#endif /* CONFIG_SAE */
#ifdef CONFIG_FILS
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_FILS_SHA256) {
		ret = os_snprintf(pos, end - pos, "FT-FILS-SHA256 ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_FILS_SHA384) {
		ret = os_snprintf(pos, end - pos, "FT-FILS-SHA384 ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#endif /* CONFIG_FILS */
#endif /* CONFIG_IEEE80211R_AP */
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_PSK_SHA256) {
		ret = os_snprintf(pos, end - pos, "WPA-PSK-SHA256 ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_IEEE8021X_SHA256) {
		ret = os_snprintf(pos, end - pos, "WPA-EAP-SHA256 ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#ifdef CONFIG_SAE
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_SAE) {
		ret = os_snprintf(pos, end - pos, "SAE ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#endif /* CONFIG_SAE */
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_IEEE8021X_SUITE_B) {
		ret = os_snprintf(pos, end - pos, "WPA-EAP-SUITE-B ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
	if (hapd->conf->wpa_key_mgmt &
	    WPA_KEY_MGMT_IEEE8021X_SUITE_B_192) {
		ret = os_snprintf(pos, end - pos,
				  "WPA-EAP-SUITE-B-192 ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#ifdef CONFIG_FILS
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_FILS_SHA256) {
		ret = os_snprintf(pos, end - pos, "FILS-SHA256 ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_FILS_SHA384) {
		ret = os_snprintf(pos, end - pos, "FILS-SHA384 ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#endif /* CONFIG_FILS */

#ifdef CONFIG_OWE
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_OWE) {
		ret = os_snprintf(pos, end - pos, "OWE ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#endif /* CONFIG_OWE */

#ifdef CONFIG_DPP
	if (hapd->conf->wpa_key_mgmt & WPA_KEY_MGMT_DPP) {
		ret = os_snprintf(pos, end - pos, "DPP ");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#endif /* CONFIG_DPP */

	if (pos > buf && *(pos - 1) == ' ') {
		*(pos - 1) = '\0';
		pos--;
	}

	return pos - buf;
}


static int hostapd_ctrl_iface_get_config(struct hostapd_data *hapd,
					 char *buf, size_t buflen)
{
	int ret;
	char *pos, *end;

	pos = buf;
	end = buf + buflen;

	ret = os_snprintf(pos, end - pos, "bssid=" MACSTR "\n"
			  "ssid=%s\n",
			  MAC2STR(hapd->own_addr),
			  wpa_ssid_txt(hapd->conf->ssid.ssid,
				       hapd->conf->ssid.ssid_len));
	if (os_snprintf_error(end - pos, ret))
		return pos - buf;
	pos += ret;

#ifdef CONFIG_WPS
	ret = os_snprintf(pos, end - pos, "wps_state=%s\n",
			  hapd->conf->wps_state == 0 ? "disabled" :
			  (hapd->conf->wps_state == 1 ? "not configured" :
			   "configured"));
	if (os_snprintf_error(end - pos, ret))
		return pos - buf;
	pos += ret;

#ifdef CONFIG_ALLOW_HOSTAPD
	if (hapd->conf->wps_state && hapd->conf->wpa &&
	    hapd->conf->ssid.wpa_passphrase) {
		ret = os_snprintf(pos, end - pos, "passphrase=%s\n",
				  hapd->conf->ssid.wpa_passphrase);
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}

	if (hapd->conf->wps_state && hapd->conf->wpa &&
	    hapd->conf->ssid.wpa_psk &&
	    hapd->conf->ssid.wpa_psk->group) {
		char hex[PMK_LEN * 2 + 1];
		wpa_snprintf_hex(hex, sizeof(hex),
				 hapd->conf->ssid.wpa_psk->psk, PMK_LEN);
		ret = os_snprintf(pos, end - pos, "psk=%s\n", hex);
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}
#endif /* CONFIG_ALLOW_HOSTAPD */
#endif /* CONFIG_WPS */

	if (hapd->conf->wpa) {
		ret = os_snprintf(pos, end - pos, "wpa=%d\n", hapd->conf->wpa);
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}

	if (hapd->conf->wpa && hapd->conf->wpa_key_mgmt) {
		ret = os_snprintf(pos, end - pos, "key_mgmt=");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;

		pos += hostapd_ctrl_iface_get_key_mgmt(hapd, pos, end - pos);

		ret = os_snprintf(pos, end - pos, "\n");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}

	if (hapd->conf->wpa) {
		ret = os_snprintf(pos, end - pos, "group_cipher=%s\n",
				  wpa_cipher_txt(hapd->conf->wpa_group));
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}

	if ((hapd->conf->wpa & WPA_PROTO_RSN) && hapd->conf->rsn_pairwise) {
		ret = os_snprintf(pos, end - pos, "rsn_pairwise_cipher=");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;

		ret = wpa_write_ciphers(pos, end, hapd->conf->rsn_pairwise,
					" ");
		if (ret < 0)
			return pos - buf;
		pos += ret;

		ret = os_snprintf(pos, end - pos, "\n");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}

	if ((hapd->conf->wpa & WPA_PROTO_WPA) && hapd->conf->wpa_pairwise) {
		ret = os_snprintf(pos, end - pos, "wpa_pairwise_cipher=");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;

		ret = wpa_write_ciphers(pos, end, hapd->conf->wpa_pairwise,
					" ");
		if (ret < 0)
			return pos - buf;
		pos += ret;

		ret = os_snprintf(pos, end - pos, "\n");
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}

	if (hapd->conf->wpa && hapd->conf->wpa_deny_ptk0_rekey) {
		ret = os_snprintf(pos, end - pos, "wpa_deny_ptk0_rekey=%d\n",
				  hapd->conf->wpa_deny_ptk0_rekey);
		if (os_snprintf_error(end - pos, ret))
			return pos - buf;
		pos += ret;
	}

	return pos - buf;
}


static void hostapd_disassoc_accept_mac(struct hostapd_data *hapd)
{
	struct sta_info *sta;
	struct vlan_description vlan_id;

	if (hapd->conf->macaddr_acl != DENY_UNLESS_ACCEPTED)
		return;

	for (sta = hapd->sta_list; sta; sta = sta->next) {
		if (!hostapd_maclist_found(hapd->conf->accept_mac,
					   hapd->conf->num_accept_mac,
					   sta->addr, &vlan_id) ||
		    (vlan_id.notempty &&
		     vlan_compare(&vlan_id, sta->vlan_desc)))
			ap_sta_disconnect(hapd, sta, sta->addr,
					  WLAN_REASON_UNSPECIFIED);
	}
}


static void hostapd_disassoc_deny_mac(struct hostapd_data *hapd)
{
	struct sta_info *sta;
	struct vlan_description vlan_id;

	for (sta = hapd->sta_list; sta; sta = sta->next) {
		if (hostapd_maclist_found(hapd->conf->deny_mac,
					  hapd->conf->num_deny_mac, sta->addr,
					  &vlan_id) &&
		    (!vlan_id.notempty ||
		     !vlan_compare(&vlan_id, sta->vlan_desc)))
			ap_sta_disconnect(hapd, sta, sta->addr,
					  WLAN_REASON_PREV_AUTH_NOT_VALID);
	}
}

/* Soft Block ACL */
static void hostapd_soft_block_disassoc_deny_mac(struct hostapd_data *hapd)
{
	struct sta_info *sta;

	for (sta = hapd->sta_list; sta; sta = sta->next) {
		if (hostapd_soft_block_maclist_found(&hapd->conf->deny_mac_sb, sta->addr))
			ap_sta_disconnect(hapd, sta, sta->addr, WLAN_REASON_UNSPECIFIED);
	}
}

static int hostapd_ctrl_iface_set(struct hostapd_data *hapd, char *cmd)
{
	char *value;
	int ret = 0;

	value = os_strchr(cmd, ' ');
	if (value == NULL)
		return -1;
	*value++ = '\0';

	wpa_printf(MSG_DEBUG, "CTRL_IFACE SET '%s'='%s'", cmd, value);
	if (0) {
#ifdef CONFIG_WPS_TESTING
	} else if (os_strcasecmp(cmd, "wps_version_number") == 0) {
		long int val;
		val = strtol(value, NULL, 0);
		if (val < 0 || val > 0xff) {
			ret = -1;
			wpa_printf(MSG_DEBUG, "WPS: Invalid "
				   "wps_version_number %ld", val);
		} else {
			wps_version_number = val;
			wpa_printf(MSG_DEBUG, "WPS: Testing - force WPS "
				   "version %u.%u",
				   (wps_version_number & 0xf0) >> 4,
				   wps_version_number & 0x0f);
			hostapd_wps_update_ie(hapd);
		}
	} else if (os_strcasecmp(cmd, "wps_testing_dummy_cred") == 0) {
		wps_testing_dummy_cred = atoi(value);
		wpa_printf(MSG_DEBUG, "WPS: Testing - dummy_cred=%d",
			   wps_testing_dummy_cred);
	} else if (os_strcasecmp(cmd, "wps_corrupt_pkhash") == 0) {
		wps_corrupt_pkhash = atoi(value);
		wpa_printf(MSG_DEBUG, "WPS: Testing - wps_corrupt_pkhash=%d",
			   wps_corrupt_pkhash);
#endif /* CONFIG_WPS_TESTING */
#ifdef CONFIG_TESTING_OPTIONS
	} else if (os_strcasecmp(cmd, "ext_mgmt_frame_handling") == 0) {
		hapd->ext_mgmt_frame_handling = atoi(value);
	} else if (os_strcasecmp(cmd, "ext_eapol_frame_io") == 0) {
		hapd->ext_eapol_frame_io = atoi(value);
#ifdef CONFIG_DPP
	} else if (os_strcasecmp(cmd, "dpp_config_obj_override") == 0) {
		os_free(hapd->dpp_config_obj_override);
		hapd->dpp_config_obj_override = os_strdup(value);
	} else if (os_strcasecmp(cmd, "dpp_discovery_override") == 0) {
		os_free(hapd->dpp_discovery_override);
		hapd->dpp_discovery_override = os_strdup(value);
	} else if (os_strcasecmp(cmd, "dpp_groups_override") == 0) {
		os_free(hapd->dpp_groups_override);
		hapd->dpp_groups_override = os_strdup(value);
	} else if (os_strcasecmp(cmd,
				 "dpp_ignore_netaccesskey_mismatch") == 0) {
		hapd->dpp_ignore_netaccesskey_mismatch = atoi(value);
	} else if (os_strcasecmp(cmd, "dpp_test") == 0) {
		dpp_test = atoi(value);
	} else if (os_strcasecmp(cmd, "dpp_version_override") == 0) {
		dpp_version_override = atoi(value);
#endif /* CONFIG_DPP */
#endif /* CONFIG_TESTING_OPTIONS */
#ifdef CONFIG_MBO
	} else if (os_strcasecmp(cmd, "mbo_assoc_disallow") == 0) {
		int val;

		if (!hapd->conf->mbo_enabled)
			return -1;

		val = atoi(value);
		if (val < 0 || val > 1)
			return -1;

		hapd->mbo_assoc_disallow = val;
		ieee802_11_update_beacons(hapd->iface);

		/*
		 * TODO: Need to configure drivers that do AP MLME offload with
		 * disallowing station logic.
		 */
	} else if (os_strcasecmp(cmd, "mbo_cell_aware") == 0) {
		int val;

		if (!hapd->conf->mbo_enabled) {
			wpa_printf(MSG_DEBUG, "MBO: mbo is not enabled");
			return -1;
		}

		val = atoi(value);
		if (val < 0 || val > 1) {
			wpa_printf(MSG_DEBUG,
				"MBO: mbo_cell_aware value is out of range (can be 0 or 1)");
			return -1;
		}

		hapd->conf->mbo_cell_aware = val;
		ieee802_11_update_beacons(hapd->iface);
#endif /* CONFIG_MBO */
#ifdef CONFIG_DPP
	} else if (os_strcasecmp(cmd, "dpp_configurator_params") == 0) {
		os_free(hapd->dpp_configurator_params);
		hapd->dpp_configurator_params = os_strdup(value);
	} else if (os_strcasecmp(cmd, "dpp_init_max_tries") == 0) {
		hapd->dpp_init_max_tries = atoi(value);
	} else if (os_strcasecmp(cmd, "dpp_init_retry_time") == 0) {
		hapd->dpp_init_retry_time = atoi(value);
	} else if (os_strcasecmp(cmd, "dpp_resp_wait_time") == 0) {
		hapd->dpp_resp_wait_time = atoi(value);
	} else if (os_strcasecmp(cmd, "dpp_resp_max_tries") == 0) {
		hapd->dpp_resp_max_tries = atoi(value);
	} else if (os_strcasecmp(cmd, "dpp_resp_retry_time") == 0) {
		hapd->dpp_resp_retry_time = atoi(value);
#endif /* CONFIG_DPP */
	} else if (os_strcasecmp(cmd, "start_disabled") == 0) {
		int val;

		val = atoi(value);
		if (val < 0 || val > 1) {
			wpa_printf(MSG_DEBUG,
				"start_disabled value is out of range (can be 0 or 1)");
			return -1;
		}

		hapd->conf->start_disabled = val;

		if (hapd->started && !hapd->conf->start_disabled &&
		    ieee802_11_set_beacon(hapd) < 0)
		    return -1;
	} else {
		ret = hostapd_set_iface(hapd->iconf, hapd->conf, cmd, value);
		if (ret)
			return ret;
#ifdef CONFIG_DPP
		hostapd_dpp_reload_timeouts(hapd);
#endif
		if (os_strcasecmp(cmd, "deny_mac_file") == 0) {
			hostapd_disassoc_deny_mac(hapd);
		} else if (os_strcasecmp(cmd, "accept_mac_file") == 0) {
			hostapd_disassoc_accept_mac(hapd);
		} else if (os_strncmp(cmd, "wme_ac_", 7) == 0 ||
			   os_strncmp(cmd, "wmm_ac_", 7) == 0) {
			hapd->parameter_set_count++;
			if (ieee802_11_update_beacons(hapd->iface))
				wpa_printf(MSG_DEBUG,
					   "Failed to update beacons with WMM parameters");
		} else if (os_strcmp(cmd, "wpa_passphrase") == 0 ||
			   os_strcmp(cmd, "sae_password") == 0 ||
			   os_strcmp(cmd, "sae_pwe") == 0) {
			if (hapd->started)
				hostapd_setup_sae_pt(hapd->conf);
		} else if (os_strcasecmp(cmd, "transition_disable") == 0) {
			wpa_auth_set_transition_disable(hapd->wpa_auth,
							hapd->conf->transition_disable);
		}

#ifdef CONFIG_TESTING_OPTIONS
		if (os_strcmp(cmd, "ft_rsnxe_used") == 0)
			wpa_auth_set_ft_rsnxe_used(hapd->wpa_auth,
						   hapd->conf->ft_rsnxe_used);
		else if (os_strcmp(cmd, "oci_freq_override_eapol_m3") == 0)
			wpa_auth_set_ocv_override_freq(
				hapd->wpa_auth, WPA_AUTH_OCV_OVERRIDE_EAPOL_M3,
				atoi(value));
		else if (os_strcmp(cmd, "oci_freq_override_eapol_g1") == 0)
			wpa_auth_set_ocv_override_freq(
				hapd->wpa_auth, WPA_AUTH_OCV_OVERRIDE_EAPOL_G1,
				atoi(value));
		else if (os_strcmp(cmd, "oci_freq_override_ft_assoc") == 0)
			wpa_auth_set_ocv_override_freq(
				hapd->wpa_auth, WPA_AUTH_OCV_OVERRIDE_FT_ASSOC,
				atoi(value));
		else if (os_strcmp(cmd, "oci_freq_override_fils_assoc") == 0)
			wpa_auth_set_ocv_override_freq(
				hapd->wpa_auth,
				WPA_AUTH_OCV_OVERRIDE_FILS_ASSOC, atoi(value));
#endif /* CONFIG_TESTING_OPTIONS */
	}

	return ret;
}


static int hostapd_ctrl_iface_get(struct hostapd_data *hapd, char *cmd,
				  char *buf, size_t buflen)
{
	int res;

	wpa_printf(MSG_DEBUG, "CTRL_IFACE GET '%s'", cmd);

	if (os_strcmp(cmd, "version") == 0) {
		res = os_snprintf(buf, buflen, "%s", VERSION_STR);
		if (os_snprintf_error(buflen, res))
			return -1;
		return res;
	} else if (os_strcmp(cmd, "tls_library") == 0) {
		res = tls_get_library_version(buf, buflen);
		if (os_snprintf_error(buflen, res))
			return -1;
		return res;
	} else if (os_strcmp(cmd, "dynamic_failsafe") == 0) {
		res = sprintf_s(buf, buflen, "%d\n", hapd->iface->conf->dynamic_failsafe);
		if (res <= 0)
			return -1;
		return res;
	}

	return -1;
}


static int hostapd_ctrl_iface_enable(struct hostapd_iface *iface)
{
	if (hostapd_enable_iface(iface) < 0) {
		wpa_printf(MSG_ERROR, "Enabling of interface failed");
		return -1;
	}
	return 0;
}


static int hostapd_ctrl_iface_reload(struct hostapd_iface *iface)
{
	if (hostapd_reload_iface(iface) < 0) {
		wpa_printf(MSG_ERROR, "Reloading of interface failed");
		return -1;
	}
	return 0;
}
#ifdef CONFIG_IEEE80211AX
static int hostapd_ctrl_iface_update_edca_counter(struct hostapd_iface *iface)
{
	int count = get_he_cap(iface->conf->he_mu_edca.he_qos_info, HE_QOS_INFO_EDCA_PARAM_SET_COUNT);
	count++;
	clr_set_he_cap(&iface->conf->he_mu_edca.he_qos_info,
				   count, HE_QOS_INFO_EDCA_PARAM_SET_COUNT);
	if (ieee802_11_update_beacons(iface) < 0) {
		wpa_printf(MSG_ERROR, "Beacon update failed on MU EDCA Parameter Set Count increment");
		return -1;
	}
	return 0;
}
#endif /* CONFIG_IEEE80211AX */


static void hostapd_ctrl_iface_config_update(struct hostapd_config *conf, char* reload_opts)
{
	char *str;
	int val;

	str = os_strstr(reload_opts, "channel=");
	if (str) {
		str += sizeof("channel=") - 1;
		conf->channel = strtol(str, NULL, 10);
	}

	str = os_strstr(reload_opts, "sec_chan=");
	if (str) {
		str += sizeof("sec_chan=") - 1;
		conf->secondary_channel = strtol(str, NULL, 10);
		if (conf->secondary_channel)
			conf->ht_capab |= HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET;
		else
			conf->ht_capab &= ~HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET;
	} else {
		conf->ht_capab &= ~HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET;
	}

	str = os_strstr(reload_opts, "center_freq1=");
	if (str) {
		u8 cf1 = 0;
		str += sizeof("center_freq1=") - 1;
		val = strtol(str, NULL, 10);

		/* In case invalid or 0 freq given, cf1 will stay at 0 */
		ieee80211_freq_to_chan(val, &cf1);
		hostapd_set_oper_centr_freq_seg0_idx(conf, cf1);
	}

	str = os_strstr(reload_opts, "center_freq2=");
	if (str) {
		u8 cf2 = 0;
		str += sizeof("center_freq2=") - 1;
		val = strtol(str, NULL, 10);

		/* In case invalid or 0 freq given, cf2 will stay at 0 */
		ieee80211_freq_to_chan(val, &cf2);
		hostapd_set_oper_centr_freq_seg1_idx(conf, cf2);
	}

	str = os_strstr(reload_opts, "bandwidth=");
	if (str) {
		str += sizeof("bandwidth=") - 1;
		val = strtol(str, NULL, 10);

		switch (val) {
		case CHAN_WIDTH_20_NOHT:
		case CHAN_WIDTH_20:
		case CHAN_WIDTH_40:
			hostapd_set_oper_chwidth(conf, CHANWIDTH_USE_HT);
			break;
		case CHAN_WIDTH_80P80:
			wpa_printf(MSG_WARNING, "80P80 not supported");
		case CHAN_WIDTH_80:
			hostapd_set_oper_chwidth(conf, CHANWIDTH_80MHZ);
			break;
		case CHAN_WIDTH_160:
			hostapd_set_oper_chwidth(conf, CHANWIDTH_160MHZ);
			break;
		default:
			wpa_printf(MSG_WARNING, "Unknown bandwidth: %d", val);
			break;
		}
	}
}


static int hostapd_ctrl_iface_update_reload(struct hostapd_data *hapd, char *cmd)
{
	struct hostapd_iface *iface = hapd->iface;
	struct hostapd_config *conf = hapd->iconf;
	int j;

	hostapd_ctrl_iface_disable(iface);

	hostapd_ctrl_iface_config_update(conf, cmd);

	for (j = 0; j < iface->num_bss; j++) {
		iface->bss[j]->conf->start_disabled = 0;
	}

	return hostapd_ctrl_iface_enable(iface);
}


static int hostapd_ctrl_iface_reconf(struct hostapd_iface *iface,
		char *param)
{
	int i, bss_idx = -1;
	rsize_t len;

	while (param[0] == ' ')
		param++;

	if (iface->state == HAPD_IFACE_UNINITIALIZED) {
		wpa_printf(MSG_ERROR, "Reconfig cannot be done before interface is initialized");
		return -1;
	}

	len = strnlen_s(param, IFNAMSIZ);
	if (len) {
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, IFNAMSIZ - 1)) {
			wpa_printf(MSG_ERROR, "wrong interface name len");
			return -1;
		}

		for (i = 0; i < iface->num_bss; i++) {
			struct hostapd_data *bss = iface->bss[i];

			if (!strncmp(bss->conf->iface, param, IFNAMSIZ)) {
				bss_idx = i;
				break;
			}
		}

		if (bss_idx == 0) {
			wpa_printf(MSG_ERROR, "Reconfig of master BSS is illegal");
			return -1;
		}
	}

	if (hostapd_reconf_iface(iface, bss_idx) < 0) {
		wpa_printf(MSG_ERROR, "Reconfig of interface failed");
		return -1;
	}

	return 0;
}

static int hostapd_set_credentials(struct hostapd_iface *iface,
			struct hostapd_data *hapd, char *input_data)
{
	int ret = 0;
	rsize_t len = 0;
	int pos;
	int key_no = 0;
	char *key, *val_start, *val_end, *val;
	size_t slen;
	struct hostapd_bss_config *bss = hapd->conf;

	while (*input_data == ' ')
		input_data++;

	len = strnlen_s(input_data, MAX_BUFF_LEN - sizeof("SET_CREDENTIALS ") - 1);
	/* minimum theoretical length of key=value pair */
	if (len < 3 || (len > (MAX_BUFF_LEN - sizeof("SET_CREDENTIALS ") - 1))) {
		wpa_printf(MSG_ERROR, "CTRL: SET_CREDENTIALS - wrong len");
		return -1;
	}

#ifndef CONFIG_NO_RADIUS
	if (bss->radius->auth_server)
		bss->radius->auth_server = bss->radius->auth_servers;

	if (bss->radius->acct_server)
		bss->radius->acct_server = bss->radius->acct_servers;
#endif

	/* Now do input string parsing */
	for (pos = 0, key_no = 0; pos < len; pos = val_end + 1 - input_data, key_no++) {

		// get key=value pair
		key = input_data + pos;
		val_start = os_strchr(key, '=');
		if (val_start) {
			*val_start = '\0';
			val_start++;
		} else break; /* no more keys available */

		/* search for the end of the current key, because wpa_config_parse_string
		 * only takes a NULL-terminated value
		 */
		if ((val_start + 2) > (input_data + len)) break;

		if (*val_start == '"') {
			val_end = strchr(val_start + 1, '"');
			if (!val_end)
				return -1;

			val_end++;
		}
		else if (*val_start == 'P' && val_start[1] == '"') {
			val_end = strchr(val_start + 2, '"');
			if (!val_end)
				return -1;

			val_end++;
		}
		else {
			val_end = strchr(val_start, ' ');
			if (!val_end) val_end = input_data + len;
		}

		if (val_end < val_start) {
			wpa_printf(MSG_ERROR, "Unable to find enclosing quote for configuration option %s string %s", key, val_start);
			return 1;
		}
		*val_end = '\0';

		val = wpa_config_parse_string(val_start, &slen);
		if (!val) {
			wpa_printf(MSG_ERROR, "Unable to parse configuration option %s string %s", key, val_start);
			return 1;
		}

		ret = hostapd_config_fill(hapd->iconf, bss, key, val, key_no);
		if (ret) {
			os_free(val);
			return ret;
		}

#ifndef CONFIG_NO_RADIUS
		if (!os_strcmp(key, "auth_server_shared_secret")) {
			if (bss->radius->auth_server &&
				(bss->radius->auth_server->index < bss->radius->num_auth_servers)) {
				bss->radius->auth_server++;
			}
		}

		if (!os_strcmp(key, "acct_server_shared_secret")) {
			if (bss->radius->acct_server &&
				(bss->radius->acct_server->index < bss->radius->num_acct_servers)) {
				bss->radius->acct_server++;
			}
		}
#endif

	}

	hapd->conf->ssid.request_credentials = 0;

	if (eloop_is_timeout_registered(cred_request_timeout, hapd, NULL)) {
		eloop_cancel_timeout(send_cred_request_handler, hapd, NULL);
		eloop_cancel_timeout(cred_request_timeout, hapd, NULL);
	}

	/* initialize the vap */
	wpa_printf(MSG_INFO, "%s starting %s", __func__, hapd->conf->iface);

	if (hapd == iface->bss[0]) /* for the master bss we need to initialize the entire interface */
		ret = hostapd_setup_interface_complete(iface, 0);
	else {
		ret = hostapd_setup_bss(hapd, 0);
		hostapd_tx_queue_params(hapd);
	}

	return ret;
}


static int hostapd_ctrl_iface_credentials(struct hostapd_iface *iface, char *cmd)
{
	struct hostapd_data *hapd;

	while (*cmd == ' ') /* skip spaces */
		cmd++;

	hapd = get_bss_index(cmd, iface);
	if (hapd == NULL)
		return -1;

	if (!hapd->conf->ssid.request_credentials)
		return -1;

	cmd = os_strchr(cmd, ' ');
	if (!cmd) {
		wpa_printf(MSG_ERROR, "Empty credentials data");
		return -1;
	}

	return hostapd_set_credentials(iface, hapd, cmd);
}


static int hostapd_ctrl_iface_disable(struct hostapd_iface *iface)
{
	if (hostapd_disable_iface(iface) < 0) {
		wpa_printf(MSG_ERROR, "Disabling of interface failed");
		return -1;
	}
	return 0;
}
static int
hostapd_ctrl_iface_kick_mismatch_psk_sta_iter(struct hostapd_data *hapd,
					      struct sta_info *sta, void *ctx)
{
	struct hostapd_wpa_psk *psk;
	const u8 *pmk;
	int pmk_len;
	int pmk_match;
	int sta_match;
	int bss_match;
	int reason;

	pmk = wpa_auth_get_pmk(sta->wpa_sm, &pmk_len);

	for (psk = hapd->conf->ssid.wpa_psk; pmk && psk; psk = psk->next) {
		pmk_match = PMK_LEN == pmk_len &&
			os_memcmp(psk->psk, pmk, pmk_len) == 0;
		sta_match = psk->group == 0 &&
			os_memcmp(sta->addr, psk->addr, ETH_ALEN) == 0;
		bss_match = psk->group == 1;

		if (pmk_match && (sta_match || bss_match))
			return 0;
	}

	wpa_printf(MSG_INFO, "STA " MACSTR
		   " PSK/passphrase no longer valid - disconnect",
		   MAC2STR(sta->addr));
	reason = WLAN_REASON_PREV_AUTH_NOT_VALID;
	hostapd_drv_sta_deauth(hapd, sta->addr, reason);
	ap_sta_deauthenticate(hapd, sta, reason);

	return 0;
}

static int hostapd_ctrl_iface_get_sta_wpa_psk(struct hostapd_data *hapd, char *cmd,
					      char *buf, size_t buflen)
{
	u8 addr[ETH_ALEN];
	struct sta_info *sta;
	const char *key_id, *wpa_passphrase;

	hapd = get_bss_index(cmd, hapd->iface);
	if (hapd == NULL) {
		wpa_printf(MSG_ERROR, "Could not find the provided BSS name");
		return -1;
	}

	cmd = os_strchr(cmd, ' ');
	if (cmd) {
		cmd++;
	}
	else {
		wpa_printf(MSG_ERROR, "No STA MAC address provided");
		return -1;
	}

	if (hwaddr_aton(cmd, addr)) {
		wpa_printf(MSG_ERROR, "Invalid STA MAC address provided");
		return -1;
	}

	sta = ap_get_sta(hapd, addr);
	if (!sta) {
		wpa_printf(MSG_WARNING, "STA" MACSTR "does not exist", MAC2STR(addr));
		return -1;
	}

	key_id = ap_sta_wpa_get_keyid(hapd, sta);
	wpa_passphrase = ap_sta_wpa_get_passphrase(hapd, sta);

	if (!key_id || !wpa_passphrase) {
		wpa_printf(MSG_INFO, "STA" MACSTR "does not use PSK from Multi PSK list", MAC2STR(addr));
		return 0;
	}

	return sprintf_s(buf, buflen, "key_id=%s\nwpa_passphrase=%s\n",
			 key_id, wpa_passphrase);
}

static int hostapd_ctrl_iface_reload_wpa_psk(struct hostapd_data *hapd)
{
	struct hostapd_bss_config *conf = hapd->conf;
	int err;

	hostapd_config_clear_wpa_psk(&conf->ssid.wpa_psk);

	err = hostapd_setup_wpa_psk(conf);
	if (err < 0) {
		wpa_printf(MSG_ERROR, "Reloading WPA-PSK passwords failed: %d",
			   err);
		return -1;
	}

	ap_for_each_sta(hapd, hostapd_ctrl_iface_kick_mismatch_psk_sta_iter,
			NULL);

	return 0;
}

#ifdef CONFIG_WIFI_CERTIFICATION
static int hostapd_ctrl_iface_set_unprotected_deauth(struct hostapd_data *hapd, char *buf)
{
	if ( ( atoi(buf) == 0 ) || ( atoi(buf) == 1 ) )
		return hostapd_drv_set_unprotected_deauth(hapd, atoi(buf));
	return -1;
}

static int hostapd_ctrl_iface_set_pn_reset(struct hostapd_data *hapd, char *buf)
{
	if ( ( atoi(buf) == 0 ) || ( atoi(buf) == 1 ) )
		return hostapd_drv_set_pn_reset(hapd, atoi(buf));
	return -1;
}

static int hostapd_ctrl_set_incorrect_pn(struct hostapd_data *hapd, const char *cmd)
{
	struct sta_info *sta;
	u8 addr[ETH_ALEN];
	u8 incorrect_pn[WPA_TK_MAX_LEN];

	if (hwaddr_aton(cmd, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (!sta)
		return -1;

	if (sta->last_tk_alg == WPA_ALG_NONE)
		return -1;

	wpa_printf(MSG_ERROR, "TESTING: Incorrect PN for " MACSTR,
		   MAC2STR(sta->addr));

	/* copy current key and change complement last byte */
	os_memcpy(incorrect_pn, sta->last_tk, sta->last_tk_len);
	incorrect_pn[sta->last_tk_len - 1 ] = ~(incorrect_pn[sta->last_tk_len - 1 ]);

	return hostapd_drv_set_key(hapd->conf->iface, hapd, sta->last_tk_alg,
				   sta->addr, sta->last_tk_key_idx, 0, 1, NULL, 0,
				   incorrect_pn, sta->last_tk_len, KEY_FLAG_DEFAULT);
}

static int hostapd_ctrl_set_correct_pn(struct hostapd_data *hapd, const char *cmd)
{
	struct sta_info *sta;
	u8 addr[ETH_ALEN];

	if (hwaddr_aton(cmd, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (!sta)
		return -1;

	if (sta->last_tk_alg == WPA_ALG_NONE)
		return -1;

	wpa_printf(MSG_ERROR, "TESTING: correct PN for " MACSTR,
		   MAC2STR(sta->addr));

	return hostapd_drv_set_key(hapd->conf->iface, hapd, sta->last_tk_alg,
				   sta->addr, sta->last_tk_key_idx, 0, 1, NULL, 0,
				   sta->last_tk, sta->last_tk_len, KEY_FLAG_DEFAULT);
}

static int hostapd_ctrl_get_ptk_rekey_timer(struct hostapd_data *hapd, char *cmd, char *reply,
						size_t reply_size)
{
	struct sta_info *sta;
	u8 addr[ETH_ALEN];

	if (hwaddr_aton(cmd, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (!sta)
		return -1;

	return sprintf_s(reply, reply_size, "%d",
			sta->wpa_sm->wpa_auth->conf.wpa_ptk_rekey);

}

static int hostapd_ctrl_set_ptk_rekey_timer(struct hostapd_data *hapd, char *cmd)
{
	struct sta_info *sta;
	u8 addr[ETH_ALEN];
	char *tmp, *ptr = NULL;
	int val;

	PARSE_TOKEN(tmp, cmd, " ", &ptr);
	if (hwaddr_aton(tmp, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (!sta)
		return -1;

	PARSE_TOKEN(tmp,cmd," ",&ptr);
	val = atoi(tmp);
	if (!IS_VALID_RANGE(val, AP_TIMER_DISABLE, AP_TIMER_MAX)) {
		wpa_printf(MSG_ERROR, "invalid wpa_ptk_rekey %d",val);
		return -1;  
	}

	sta->wpa_sm->wpa_auth->conf.wpa_ptk_rekey = val;

	wpa_printf(MSG_DEBUG, "TESTING: set new ptk timer %d", val);

	wpa_auth_set_ptk_rekey_timer(sta->wpa_sm);

	return 0;
}

#endif /* CONFIG_WIFI_CERTIFICATION */

#ifdef CONFIG_TESTING_OPTIONS

static int hostapd_ctrl_iface_radar(struct hostapd_data *hapd, char *cmd)
{
	union wpa_event_data data;
	char *pos, *param;
	enum wpa_event_type event;

	wpa_printf(MSG_DEBUG, "RADAR TEST: %s", cmd);

	os_memset(&data, 0, sizeof(data));

	param = os_strchr(cmd, ' ');
	if (param == NULL)
		return -1;
	*param++ = '\0';

	if (os_strcmp(cmd, "DETECTED") == 0)
		event = EVENT_DFS_RADAR_DETECTED;
	else if (os_strcmp(cmd, "CAC-FINISHED") == 0)
		event = EVENT_DFS_CAC_FINISHED;
	else if (os_strcmp(cmd, "CAC-ABORTED") == 0)
		event = EVENT_DFS_CAC_ABORTED;
	else if (os_strcmp(cmd, "NOP-FINISHED") == 0)
		event = EVENT_DFS_NOP_FINISHED;
	else {
		wpa_printf(MSG_DEBUG, "Unsupported RADAR test command: %s",
			   cmd);
		return -1;
	}

	pos = os_strstr(param, "freq=");
	if (pos)
		data.dfs_event.freq = atoi(pos + 5);

	pos = os_strstr(param, "ht_enabled=1");
	if (pos)
		data.dfs_event.ht_enabled = 1;

	pos = os_strstr(param, "chan_offset=");
	if (pos)
		data.dfs_event.chan_offset = atoi(pos + 12);

	pos = os_strstr(param, "chan_width=");
	if (pos)
		data.dfs_event.chan_width = atoi(pos + 11);

	pos = os_strstr(param, "cf1=");
	if (pos)
		data.dfs_event.cf1 = atoi(pos + 4);

	pos = os_strstr(param, "cf2=");
	if (pos)
		data.dfs_event.cf2 = atoi(pos + 4);

	wpa_supplicant_event(hapd, event, &data);

	return 0;
}


static int hostapd_ctrl_iface_mgmt_tx(struct hostapd_data *hapd, char *cmd)
{
	size_t len;
	u8 *buf;
	int res;

	wpa_printf(MSG_DEBUG, "External MGMT TX: %s", cmd);

	len = os_strlen(cmd);
	if (len & 1)
		return -1;
	len /= 2;

	buf = os_malloc(len);
	if (buf == NULL)
		return -1;

	if (hexstr2bin(cmd, buf, len) < 0) {
		os_free(buf);
		return -1;
	}

	res = hostapd_drv_send_mlme(hapd, buf, len, 0, NULL, 0, 0);
	os_free(buf);
	return res;
}


static int hostapd_ctrl_iface_mgmt_tx_status_process(struct hostapd_data *hapd,
						     char *cmd)
{
	char *pos, *param;
	size_t len;
	u8 *buf;
	int stype = 0, ok = 0;
	union wpa_event_data event;

	if (!hapd->ext_mgmt_frame_handling)
		return -1;

	/* stype=<val> ok=<0/1> buf=<frame hexdump> */

	wpa_printf(MSG_DEBUG, "External MGMT TX status process: %s", cmd);

	pos = cmd;
	param = os_strstr(pos, "stype=");
	if (param) {
		param += 6;
		stype = atoi(param);
	}

	param = os_strstr(pos, " ok=");
	if (param) {
		param += 4;
		ok = atoi(param);
	}

	param = os_strstr(pos, " buf=");
	if (!param)
		return -1;
	param += 5;

	len = os_strlen(param);
	if (len & 1)
		return -1;
	len /= 2;

	buf = os_malloc(len);
	if (!buf || hexstr2bin(param, buf, len) < 0) {
		os_free(buf);
		return -1;
	}

	os_memset(&event, 0, sizeof(event));
	event.tx_status.type = WLAN_FC_TYPE_MGMT;
	event.tx_status.data = buf;
	event.tx_status.data_len = len;
	event.tx_status.stype = stype;
	event.tx_status.ack = ok;
	hapd->ext_mgmt_frame_handling = 0;
	wpa_supplicant_event(hapd, EVENT_TX_STATUS, &event);
	hapd->ext_mgmt_frame_handling = 1;

	os_free(buf);

	return 0;
}


static int hostapd_ctrl_iface_mgmt_rx_process(struct hostapd_data *hapd,
					      char *cmd)
{
	char *pos, *param;
	size_t len;
	u8 *buf;
	int freq = 0, datarate = 0, ssi_signal = 0;
	union wpa_event_data event;

	if (!hapd->ext_mgmt_frame_handling)
		return -1;

	/* freq=<MHz> datarate=<val> ssi_signal=<val> frame=<frame hexdump> */

	wpa_printf(MSG_DEBUG, "External MGMT RX process: %s", cmd);

	pos = cmd;
	param = os_strstr(pos, "freq=");
	if (param) {
		param += 5;
		freq = atoi(param);
	}

	param = os_strstr(pos, " datarate=");
	if (param) {
		param += 10;
		datarate = atoi(param);
	}

	param = os_strstr(pos, " ssi_signal=");
	if (param) {
		param += 12;
		ssi_signal = atoi(param);
	}

	param = os_strstr(pos, " frame=");
	if (param == NULL)
		return -1;
	param += 7;

	len = os_strlen(param);
	if (len & 1)
		return -1;
	len /= 2;

	buf = os_malloc(len);
	if (buf == NULL)
		return -1;

	if (hexstr2bin(param, buf, len) < 0) {
		os_free(buf);
		return -1;
	}

	os_memset(&event, 0, sizeof(event));
	event.rx_mgmt.freq = freq;
	event.rx_mgmt.frame = buf;
	event.rx_mgmt.frame_len = len;
	event.rx_mgmt.ssi_signal = ssi_signal;
	event.rx_mgmt.datarate = datarate;
	hapd->ext_mgmt_frame_handling = 0;
	wpa_supplicant_event(hapd, EVENT_RX_MGMT, &event);
	hapd->ext_mgmt_frame_handling = 1;

	os_free(buf);

	return 0;
}


static int hostapd_ctrl_iface_eapol_rx(struct hostapd_data *hapd, char *cmd)
{
	char *pos;
	u8 src[ETH_ALEN], *buf;
	int used;
	size_t len;

	wpa_printf(MSG_DEBUG, "External EAPOL RX: %s", cmd);

	pos = cmd;
	used = hwaddr_aton2(pos, src);
	if (used < 0)
		return -1;
	pos += used;
	while (*pos == ' ')
		pos++;

	len = os_strlen(pos);
	if (len & 1)
		return -1;
	len /= 2;

	buf = os_malloc(len);
	if (buf == NULL)
		return -1;

	if (hexstr2bin(pos, buf, len) < 0) {
		os_free(buf);
		return -1;
	}

	ieee802_1x_receive(hapd, src, buf, len);
	os_free(buf);

	return 0;
}


static u16 ipv4_hdr_checksum(const void *buf, size_t len)
{
	size_t i;
	u32 sum = 0;
	const u16 *pos = buf;

	for (i = 0; i < len / 2; i++)
		sum += *pos++;

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return sum ^ 0xffff;
}


#define HWSIM_PACKETLEN 1500
#define HWSIM_IP_LEN (HWSIM_PACKETLEN - sizeof(struct ether_header))

static void hostapd_data_test_rx(void *ctx, const u8 *src_addr, const u8 *buf,
				 size_t len)
{
	struct hostapd_data *hapd = ctx;
	const struct ether_header *eth;
	struct ip ip;
	const u8 *pos;
	unsigned int i;
	char extra[30];

	if (len < sizeof(*eth) + sizeof(ip) || len > HWSIM_PACKETLEN) {
		wpa_printf(MSG_DEBUG,
			   "test data: RX - ignore unexpected length %d",
			   (int) len);
		return;
	}

	eth = (const struct ether_header *) buf;
	os_memcpy(&ip, eth + 1, sizeof(ip));
	pos = &buf[sizeof(*eth) + sizeof(ip)];

	if (ip.ip_hl != 5 || ip.ip_v != 4 ||
	    ntohs(ip.ip_len) > HWSIM_IP_LEN) {
		wpa_printf(MSG_DEBUG,
			   "test data: RX - ignore unexpect IP header");
		return;
	}

	for (i = 0; i < ntohs(ip.ip_len) - sizeof(ip); i++) {
		if (*pos != (u8) i) {
			wpa_printf(MSG_DEBUG,
				   "test data: RX - ignore mismatching payload");
			return;
		}
		pos++;
	}

	extra[0] = '\0';
	if (ntohs(ip.ip_len) != HWSIM_IP_LEN)
		os_snprintf(extra, sizeof(extra), " len=%d", ntohs(ip.ip_len));
	wpa_msg(hapd->msg_ctx, MSG_INFO, "DATA-TEST-RX " MACSTR " " MACSTR "%s",
		MAC2STR(eth->ether_dhost), MAC2STR(eth->ether_shost), extra);
}


static int hostapd_ctrl_iface_data_test_config(struct hostapd_data *hapd,
					       char *cmd)
{
	int enabled = atoi(cmd);
	char *pos;
	const char *ifname;

	if (!enabled) {
		if (hapd->l2_test) {
			l2_packet_deinit(hapd->l2_test);
			hapd->l2_test = NULL;
			wpa_dbg(hapd->msg_ctx, MSG_DEBUG,
				"test data: Disabled");
		}
		return 0;
	}

	if (hapd->l2_test)
		return 0;

	pos = os_strstr(cmd, " ifname=");
	if (pos)
		ifname = pos + 8;
	else
		ifname = hapd->conf->iface;

	hapd->l2_test = l2_packet_init(ifname, hapd->own_addr,
					ETHERTYPE_IP, hostapd_data_test_rx,
					hapd, 1);
	if (hapd->l2_test == NULL)
		return -1;

	wpa_dbg(hapd->msg_ctx, MSG_DEBUG, "test data: Enabled");

	return 0;
}


static int hostapd_ctrl_iface_data_test_tx(struct hostapd_data *hapd, char *cmd)
{
	u8 dst[ETH_ALEN], src[ETH_ALEN];
	char *pos, *pos2;
	int used;
	long int val;
	u8 tos;
	u8 buf[2 + HWSIM_PACKETLEN];
	struct ether_header *eth;
	struct ip *ip;
	u8 *dpos;
	unsigned int i;
	size_t send_len = HWSIM_IP_LEN;

	if (hapd->l2_test == NULL)
		return -1;

	/* format: <dst> <src> <tos> [len=<length>] */

	pos = cmd;
	used = hwaddr_aton2(pos, dst);
	if (used < 0)
		return -1;
	pos += used;
	while (*pos == ' ')
		pos++;
	used = hwaddr_aton2(pos, src);
	if (used < 0)
		return -1;
	pos += used;

	val = strtol(pos, &pos2, 0);
	if (val < 0 || val > 0xff)
		return -1;
	tos = val;

	pos = os_strstr(pos2, " len=");
	if (pos) {
		i = atoi(pos + 5);
		if (i < sizeof(*ip) || i > HWSIM_IP_LEN)
			return -1;
		send_len = i;
	}

	eth = (struct ether_header *) &buf[2];
	os_memcpy(eth->ether_dhost, dst, ETH_ALEN);
	os_memcpy(eth->ether_shost, src, ETH_ALEN);
	eth->ether_type = htons(ETHERTYPE_IP);
	ip = (struct ip *) (eth + 1);
	os_memset(ip, 0, sizeof(*ip));
	ip->ip_hl = 5;
	ip->ip_v = 4;
	ip->ip_ttl = 64;
	ip->ip_tos = tos;
	ip->ip_len = htons(send_len);
	ip->ip_p = 1;
	ip->ip_src.s_addr = htonl(192U << 24 | 168 << 16 | 1 << 8 | 1);
	ip->ip_dst.s_addr = htonl(192U << 24 | 168 << 16 | 1 << 8 | 2);
	ip->ip_sum = ipv4_hdr_checksum(ip, sizeof(*ip));
	dpos = (u8 *) (ip + 1);
	for (i = 0; i < send_len - sizeof(*ip); i++)
		*dpos++ = i;

	if (l2_packet_send(hapd->l2_test, dst, ETHERTYPE_IP, &buf[2],
			   sizeof(struct ether_header) + send_len) < 0)
		return -1;

	wpa_dbg(hapd->msg_ctx, MSG_DEBUG, "test data: TX dst=" MACSTR
		" src=" MACSTR " tos=0x%x", MAC2STR(dst), MAC2STR(src), tos);

	return 0;
}


static int hostapd_ctrl_iface_data_test_frame(struct hostapd_data *hapd,
					      char *cmd)
{
	u8 *buf;
	struct ether_header *eth;
	struct l2_packet_data *l2 = NULL;
	size_t len;
	u16 ethertype;
	int res = -1;
	const char *ifname = hapd->conf->iface;

	if (os_strncmp(cmd, "ifname=", 7) == 0) {
		cmd += 7;
		ifname = cmd;
		cmd = os_strchr(cmd, ' ');
		if (cmd == NULL)
			return -1;
		*cmd++ = '\0';
	}

	len = os_strlen(cmd);
	if (len & 1 || len < ETH_HLEN * 2)
		return -1;
	len /= 2;

	buf = os_malloc(len);
	if (buf == NULL)
		return -1;

	if (hexstr2bin(cmd, buf, len) < 0)
		goto done;

	eth = (struct ether_header *) buf;
	ethertype = ntohs(eth->ether_type);

	l2 = l2_packet_init(ifname, hapd->own_addr, ethertype,
			    hostapd_data_test_rx, hapd, 1);
	if (l2 == NULL)
		goto done;

	res = l2_packet_send(l2, eth->ether_dhost, ethertype, buf, len);
	wpa_dbg(hapd->msg_ctx, MSG_DEBUG, "test data: TX frame res=%d", res);
done:
	if (l2)
		l2_packet_deinit(l2);
	os_free(buf);

	return res < 0 ? -1 : 0;
}


static int hostapd_ctrl_test_alloc_fail(struct hostapd_data *hapd, char *cmd)
{
#ifdef WPA_TRACE_BFD
	char *pos;

	wpa_trace_fail_after = atoi(cmd);
	pos = os_strchr(cmd, ':');
	if (pos) {
		pos++;
		os_strlcpy(wpa_trace_fail_func, pos,
			   sizeof(wpa_trace_fail_func));
	} else {
		wpa_trace_fail_after = 0;
	}

	return 0;
#else /* WPA_TRACE_BFD */
	return -1;
#endif /* WPA_TRACE_BFD */
}


static int hostapd_ctrl_get_alloc_fail(struct hostapd_data *hapd,
				       char *buf, size_t buflen)
{
#ifdef WPA_TRACE_BFD
	return os_snprintf(buf, buflen, "%u:%s", wpa_trace_fail_after,
			   wpa_trace_fail_func);
#else /* WPA_TRACE_BFD */
	return -1;
#endif /* WPA_TRACE_BFD */
}


static int hostapd_ctrl_test_fail(struct hostapd_data *hapd, char *cmd)
{
#ifdef WPA_TRACE_BFD
	char *pos;

	wpa_trace_test_fail_after = atoi(cmd);
	pos = os_strchr(cmd, ':');
	if (pos) {
		pos++;
		os_strlcpy(wpa_trace_test_fail_func, pos,
			   sizeof(wpa_trace_test_fail_func));
	} else {
		wpa_trace_test_fail_after = 0;
	}

	return 0;
#else /* WPA_TRACE_BFD */
	return -1;
#endif /* WPA_TRACE_BFD */
}


static int hostapd_ctrl_get_fail(struct hostapd_data *hapd,
				 char *buf, size_t buflen)
{
#ifdef WPA_TRACE_BFD
	return os_snprintf(buf, buflen, "%u:%s", wpa_trace_test_fail_after,
			   wpa_trace_test_fail_func);
#else /* WPA_TRACE_BFD */
	return -1;
#endif /* WPA_TRACE_BFD */
}


static int hostapd_ctrl_reset_pn(struct hostapd_data *hapd, const char *cmd)
{
	struct sta_info *sta;
	u8 addr[ETH_ALEN];
	u8 zero[WPA_TK_MAX_LEN];

	os_memset(zero, 0, sizeof(zero));

	if (hwaddr_aton(cmd, addr))
		return -1;

	if (is_broadcast_ether_addr(addr) && os_strstr(cmd, "IGTK")) {
		if (hapd->last_igtk_alg == WPA_ALG_NONE)
			return -1;

		wpa_printf(MSG_INFO, "TESTING: Reset IPN for IGTK");

		/* First, use a zero key to avoid any possible duplicate key
		 * avoidance in the driver. */
		if (hostapd_drv_set_key(hapd->conf->iface, hapd,
					hapd->last_igtk_alg,
					broadcast_ether_addr,
					hapd->last_igtk_key_idx, 0, 1, NULL, 0,
					zero, hapd->last_igtk_len,
					KEY_FLAG_GROUP_TX_DEFAULT) < 0)
			return -1;

		/* Set the previously configured key to reset its TSC */
		return hostapd_drv_set_key(hapd->conf->iface, hapd,
					   hapd->last_igtk_alg,
					   broadcast_ether_addr,
					   hapd->last_igtk_key_idx, 0, 1, NULL,
					   0, hapd->last_igtk,
					   hapd->last_igtk_len,
					   KEY_FLAG_GROUP_TX_DEFAULT);
	}

	if (is_broadcast_ether_addr(addr) && os_strstr(cmd, "BIGTK")) {
		if (hapd->last_bigtk_alg == WPA_ALG_NONE)
			return -1;

		wpa_printf(MSG_INFO, "TESTING: Reset IPN for BIGTK");

		/* First, use a zero key to avoid any possible duplicate key
		 * avoidance in the driver. */
		if (hostapd_drv_set_key(hapd->conf->iface, hapd,
					hapd->last_bigtk_alg,
					broadcast_ether_addr,
					hapd->last_gtk_key_idx, 0, 1, NULL, 0,
					zero, hapd->last_gtk_len,
					KEY_FLAG_GROUP_TX_DEFAULT) < 0)
			return -1;

		/* Set the previously configured key to reset its TSC */
		return hostapd_drv_set_key(hapd->conf->iface, hapd,
					   hapd->last_bigtk_alg,
					   broadcast_ether_addr,
					   hapd->last_gtk_key_idx, 0, 1, NULL,
					   0, hapd->last_gtk,
					   hapd->last_gtk_len,
					   KEY_FLAG_GROUP_TX_DEFAULT);
	}

	if (is_broadcast_ether_addr(addr)) {
		if (hapd->last_gtk_alg == WPA_ALG_NONE)
			return -1;

		wpa_printf(MSG_INFO, "TESTING: Reset PN for GTK");

		/* First, use a zero key to avoid any possible duplicate key
		 * avoidance in the driver. */
		if (hostapd_drv_set_key(hapd->conf->iface, hapd,
					hapd->last_gtk_alg,
					broadcast_ether_addr,
					hapd->last_gtk_key_idx, 1, NULL, 0,
					zero, hapd->last_gtk_len) < 0)
			return -1;

		/* Set the previously configured key to reset its TSC */
		return hostapd_drv_set_key(hapd->conf->iface, hapd,
					   hapd->last_gtk_alg,
					   broadcast_ether_addr,
					   hapd->last_gtk_key_idx, 1, NULL, 0,
					   hapd->last_gtk, hapd->last_gtk_len);
	}

	sta = ap_get_sta(hapd, addr);
	if (!sta)
		return -1;

	if (sta->last_tk_alg == WPA_ALG_NONE)
		return -1;

	wpa_printf(MSG_INFO, "TESTING: Reset PN for " MACSTR,
		   MAC2STR(sta->addr));

	/* First, use a zero key to avoid any possible duplicate key avoidance
	 * in the driver. */
	if (hostapd_drv_set_key(hapd->conf->iface, hapd, sta->last_tk_alg,
				sta->addr, sta->last_tk_key_idx, 0, 1, NULL, 0,
				zero, sta->last_tk_len,
				KEY_FLAG_PAIRWISE_RX_TX) < 0)
		return -1;

	/* Set the previously configured key to reset its TSC/RSC */
	return hostapd_drv_set_key(hapd->conf->iface, hapd, sta->last_tk_alg,
				   sta->addr, sta->last_tk_key_idx, 0, 1, NULL,
				   0, sta->last_tk, sta->last_tk_len,
				   KEY_FLAG_PAIRWISE_RX_TX);
}


static int hostapd_ctrl_set_key(struct hostapd_data *hapd, const char *cmd)
{
	u8 addr[ETH_ALEN];
	const char *pos = cmd;
	enum wpa_alg alg;
	enum key_flag key_flag;
	int idx, set_tx;
	u8 seq[6], key[WPA_TK_MAX_LEN];
	size_t key_len;

	/* parameters: alg addr idx set_tx seq key key_flag */

	alg = atoi(pos);
	pos = os_strchr(pos, ' ');
	if (!pos)
		return -1;
	pos++;
	if (hwaddr_aton(pos, addr))
		return -1;
	pos += 17;
	if (*pos != ' ')
		return -1;
	pos++;
	idx = atoi(pos);
	pos = os_strchr(pos, ' ');
	if (!pos)
		return -1;
	pos++;
	set_tx = atoi(pos);
	pos = os_strchr(pos, ' ');
	if (!pos)
		return -1;
	pos++;
	if (hexstr2bin(pos, seq, sizeof(seq)) < 0)
		return -1;
	pos += 2 * 6;
	if (*pos != ' ')
		return -1;
	pos++;
	if (!os_strchr(pos, ' '))
		return -1;
	key_len = (os_strchr(pos, ' ') - pos) / 2;
	if (hexstr2bin(pos, key, key_len) < 0)
		return -1;
	pos += 2 * key_len;
	if (*pos != ' ')
		return -1;

	pos++;
	key_flag = atoi(pos);
	pos = os_strchr(pos, ' ');
	if (pos)
		return -1;

	wpa_printf(MSG_INFO, "TESTING: Set key");
	return hostapd_drv_set_key(hapd->conf->iface, hapd, alg, addr, idx, 0,
				   set_tx, seq, 6, key, key_len, key_flag);
}


static void restore_tk(void *ctx1, void *ctx2)
{
	struct hostapd_data *hapd = ctx1;
	struct sta_info *sta = ctx2;

	wpa_printf(MSG_INFO, "TESTING: Restore TK for " MACSTR,
		   MAC2STR(sta->addr));
	/* This does not really restore the TSC properly, so this will result
	 * in replay protection issues for now since there is no clean way of
	 * preventing encryption of a single EAPOL frame. */
	hostapd_drv_set_key(hapd->conf->iface, hapd, sta->last_tk_alg,
			    sta->addr, sta->last_tk_key_idx, 0, 1, NULL, 0,
			    sta->last_tk, sta->last_tk_len,
			    KEY_FLAG_PAIRWISE_RX_TX);
}


static int hostapd_ctrl_resend_m1(struct hostapd_data *hapd, const char *cmd)
{
	struct sta_info *sta;
	u8 addr[ETH_ALEN];
	int plain = os_strstr(cmd, "plaintext") != NULL;

	if (hwaddr_aton(cmd, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (!sta || !sta->wpa_sm)
		return -1;

	if (plain && sta->last_tk_alg == WPA_ALG_NONE)
		plain = 0; /* no need for special processing */
	if (plain) {
		wpa_printf(MSG_INFO, "TESTING: Clear TK for " MACSTR,
			   MAC2STR(sta->addr));
		hostapd_drv_set_key(hapd->conf->iface, hapd, WPA_ALG_NONE,
				    sta->addr, sta->last_tk_key_idx, 0, 0, NULL,
				    0, NULL, 0, KEY_FLAG_PAIRWISE);
	}

	wpa_printf(MSG_INFO, "TESTING: Send M1 to " MACSTR, MAC2STR(sta->addr));
	return wpa_auth_resend_m1(sta->wpa_sm,
				  os_strstr(cmd, "change-anonce") != NULL,
				  plain ? restore_tk : NULL, hapd, sta);
}


static int hostapd_ctrl_resend_m3(struct hostapd_data *hapd, const char *cmd)
{
	struct sta_info *sta;
	u8 addr[ETH_ALEN];
	int plain = os_strstr(cmd, "plaintext") != NULL;

	if (hwaddr_aton(cmd, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (!sta || !sta->wpa_sm)
		return -1;

	if (plain && sta->last_tk_alg == WPA_ALG_NONE)
		plain = 0; /* no need for special processing */
	if (plain) {
		wpa_printf(MSG_INFO, "TESTING: Clear TK for " MACSTR,
			   MAC2STR(sta->addr));
		hostapd_drv_set_key(hapd->conf->iface, hapd, WPA_ALG_NONE,
				    sta->addr, sta->last_tk_key_idx, 0, 0, NULL,
				    0, NULL, 0, KEY_FLAG_PAIRWISE);
	}

	wpa_printf(MSG_INFO, "TESTING: Send M3 to " MACSTR, MAC2STR(sta->addr));
	return wpa_auth_resend_m3(sta->wpa_sm,
				  plain ? restore_tk : NULL, hapd, sta);
}


static int hostapd_ctrl_resend_group_m1(struct hostapd_data *hapd,
					const char *cmd)
{
	struct sta_info *sta;
	u8 addr[ETH_ALEN];
	int plain = os_strstr(cmd, "plaintext") != NULL;

	if (hwaddr_aton(cmd, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (!sta || !sta->wpa_sm)
		return -1;

	if (plain && sta->last_tk_alg == WPA_ALG_NONE)
		plain = 0; /* no need for special processing */
	if (plain) {
		wpa_printf(MSG_INFO, "TESTING: Clear TK for " MACSTR,
			   MAC2STR(sta->addr));
		hostapd_drv_set_key(hapd->conf->iface, hapd, WPA_ALG_NONE,
				    sta->addr, sta->last_tk_key_idx, 0, 0, NULL,
				    0, NULL, 0, KEY_FLAG_PAIRWISE);
	}

	wpa_printf(MSG_INFO,
		   "TESTING: Send group M1 for the same GTK and zero RSC to "
		   MACSTR, MAC2STR(sta->addr));
	return wpa_auth_resend_group_m1(sta->wpa_sm,
					plain ? restore_tk : NULL, hapd, sta);
}


static int hostapd_ctrl_get_pmk(struct hostapd_data *hapd, const char *cmd,
				char *buf, size_t buflen)
{
	struct sta_info *sta;
	u8 addr[ETH_ALEN];
	const u8 *pmk;
	int pmk_len;

	if (hwaddr_aton(cmd, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (!sta || !sta->wpa_sm) {
		wpa_printf(MSG_DEBUG, "No STA WPA state machine for " MACSTR,
			   MAC2STR(addr));
		return -1;
	}
	pmk = wpa_auth_get_pmk(sta->wpa_sm, &pmk_len);
	if (!pmk) {
		wpa_printf(MSG_DEBUG, "No PMK stored for " MACSTR,
			   MAC2STR(addr));
		return -1;
	}

	return wpa_snprintf_hex(buf, buflen, pmk, pmk_len);
}

#endif /* CONFIG_TESTING_OPTIONS */

int hostapd_get_secondary_offset (int primary, int bandwidth, int vht_seg0_idx)
{
	if (bandwidth == 20)
		return 0;

	while (bandwidth > 40)
	{
		if (vht_seg0_idx > primary)
			vht_seg0_idx -= bandwidth / 20;
		else
			vht_seg0_idx += bandwidth / 20;

		bandwidth /= 2;
	}

	if (vht_seg0_idx > primary)
		return 1;
	else
		return -1;
}

void
set_iface_conf(struct hostapd_iface *iface,
        const struct hostapd_freq_params *freq_params)
{
  int seg0_idx = 0, seg1_idx = 0, bw = CHANWIDTH_USE_HT;

  if(freq_params->center_freq1) {
    if (is_6ghz_freq(freq_params->center_freq1))
      seg0_idx = (freq_params->center_freq1 - STARTING_6G_FREQ_IN_MHZ) / 5;
    else
      seg0_idx = 36 + (freq_params->center_freq1 - 5180) / 5;
  }

  if(freq_params->center_freq2) {
    if (is_6ghz_freq(freq_params->center_freq2))
      seg1_idx = (freq_params->center_freq2 - STARTING_6G_FREQ_IN_MHZ) / 5;
    else
      seg1_idx = 36 + (freq_params->center_freq2 - 5180) / 5;
  }

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
  if (freq_params->bandwidth > 20 && iface->conf->secondary_channel == 0) {
    iface->conf->secondary_channel = hostapd_get_secondary_offset(iface->conf->channel, freq_params->bandwidth, seg0_idx);
  }
  hostapd_set_oper_centr_freq_seg0_idx(iface->conf,seg0_idx);
  hostapd_set_oper_centr_freq_seg1_idx(iface->conf,seg1_idx);
  hostapd_set_oper_chwidth(iface->conf,bw);
}

#ifdef NEED_AP_MLME
static bool ctrl_iface_chan_is_dfs_usable(struct hostapd_iface *iface, int frequency)
{
	int i;
	struct hostapd_hw_modes *mode = iface->current_mode;

	for (i = 0; i < mode->num_channels; i++) {
		if (mode->channels[i].freq == frequency) {
			int flag = mode->channels[i].flag;
			if ((flag & HOSTAPD_CHAN_RADAR) && ((flag & HOSTAPD_CHAN_DFS_MASK) != HOSTAPD_CHAN_DFS_AVAILABLE))
				return true;
			else
				return false;
		}
	}
	return false;
}

static int ctrl_iface_chan_switch_is_dfs_required(struct hostapd_iface *iface, struct csa_settings *settings)
{
#define ONE_CHAN_BW_MHZ	20
	int i;
	int n_chans = 1;
	int frequency = settings->freq_params.freq;

	if (!frequency) {
		wpa_printf(MSG_ERROR, "chan_switch: incorrect cmd-line parameter: freq=%d",
			settings->freq_params.freq);
		return -1;
	}

	if ((settings->freq_params.sec_channel_offset < -1) ||	/* optional cmd-line parameter */
		(settings->freq_params.sec_channel_offset > 1)) {
		wpa_printf(MSG_ERROR, "chan_switch: incorrect cmd-line parameter: sec_channel_offset=%d",
			settings->freq_params.sec_channel_offset);
		return -1;
	}

	if (settings->freq_params.bandwidth) {					/* optional cmd-line parameter */
		if ((settings->freq_params.bandwidth % ONE_CHAN_BW_MHZ)) {
			wpa_printf(MSG_ERROR, "chan_switch: incorrect cmd-line parameter: bandwidth=%d",
				settings->freq_params.bandwidth);
			return -1;
		}
		n_chans = settings->freq_params.bandwidth / ONE_CHAN_BW_MHZ;
	}

	if (n_chans > 1) {
		if (settings->freq_params.center_freq1)				/* optional cmd-line parameter */
			frequency = settings->freq_params.center_freq1 + (ONE_CHAN_BW_MHZ >> 1) - (ONE_CHAN_BW_MHZ * (n_chans >> 1));
		else if (settings->freq_params.sec_channel_offset == -1)
			frequency -= ONE_CHAN_BW_MHZ;
		/*else if (settings->freq_params.sec_channel_offset == 1)
			frequency = frequency;*/
		else if (!settings->freq_params.sec_channel_offset) {
			wpa_printf(MSG_ERROR, "chan_switch: incorrect cmd-line parameters: center_freq1=%d, sec_channel_offset=%d",
				settings->freq_params.center_freq1, settings->freq_params.sec_channel_offset);
			return -1;
		}
	}

	for (i = 0; i < n_chans; i++) {
		if (ctrl_iface_chan_is_dfs_usable(iface, frequency))
			return 1;	/* DFS procedure is required */

		frequency += ONE_CHAN_BW_MHZ;
	}
#undef ONE_CHAN_BW_MHZ
	return 0;				/* DFS procedure is not required */
}
#endif /* NEED_AP_MLME */

static int hostapd_ctrl_iface_chan_switch(struct hostapd_iface *iface,
					  char *pos)
{
#ifdef NEED_AP_MLME
	struct csa_settings settings;
	int ret;
	unsigned int i;
	struct hostapd_channel_data *channel;
	int tx_ant_mask = -1, rx_ant_mask = -1;
	char *str, *pos2;
	bool switch_type_scan = false;

	ret = hostapd_parse_csa_settings(pos, &settings);
	if (ret)
		return ret;

	if (os_strstr(pos, " auto-ht")) {
		settings.freq_params.ht_enabled = iface->conf->ieee80211n;
		settings.freq_params.vht_enabled = iface->conf->ieee80211ac;
	}

	str = os_strstr(pos, "tx_ant_mask=");
  if (str) {
    str += sizeof("tx_ant_mask=") - 1;

    tx_ant_mask = strtol(str, &pos2, 10);
    if (str == pos2 || tx_ant_mask < 0) {
      wpa_printf(MSG_ERROR, "chan_switch: invalid tx_ant_mask provided");
      return -1;
    }
  }
  str = os_strstr(pos, "rx_ant_mask=");
  if (str) {
    str += sizeof("rx_ant_mask=") - 1;

    rx_ant_mask = strtol(str, &pos2, 10);
    if (str == pos2 || rx_ant_mask < 0) {
      wpa_printf(MSG_ERROR, "chan_switch: invalid rx_ant_mask provided");
      return -1;
    }
  }
  if ((tx_ant_mask >= 0 && rx_ant_mask < 0) || (rx_ant_mask >= 0 && tx_ant_mask < 0)) {
    wpa_printf(MSG_ERROR, "chan_switch: changing only TX or RX "
      "antenna mask is not possible");
    return -1;
  }
  str = os_strstr(pos, "switch_type=");
  if (str) {
    str += sizeof("switch_type=") - 1;
    if (!strncmp(str, "scan", sizeof("scan") - 1))
      switch_type_scan = true;
  }

  /* Check if active CAC */
  if (iface->cac_started)
    return -1;

  if (tx_ant_mask >= 0) {
      ret = hostapd_drv_set_antenna(iface->bss[0], tx_ant_mask, rx_ant_mask);
      if (ret < 0) {
        wpa_printf(MSG_ERROR, "chan_switch: setting antenna mask failed: %s",
          strerror(errno));
      }
  }

	/* ACS */
	if (settings.freq_params.freq == 0) {
	  iface->conf->acs_scan_mode = !!os_strstr(pos, " acs_scan_mode");
	  set_iface_conf(iface, &settings.freq_params);
	  if (acs_init(iface) == HOSTAPD_CHAN_ACS)
	    return 0;
	  else
	    return -1;
	}

  channel = hostapd_hw_get_channel_data(iface, settings.freq_params.freq);
  if (!channel)
    return -1;
  /* check if usable */
  if (channel->flag & HOSTAPD_CHAN_DISABLED)
    return -1;

	ret = ctrl_iface_chan_switch_is_dfs_required(iface, &settings);
	if (ret < 0)
		return ret;

  iface->block_tx = false;
  /* check CAC required */
  if (ret)
  {
    if (switch_type_scan)
      iface->block_tx = true;
    settings.freq_params.channel = channel->chan;
    set_iface_conf(iface, &settings.freq_params);
    hostapd_setup_interface_complete(iface, 0);
    return 0;
  }

	ret = hostapd_prepare_and_send_csa_deauth_cfg_to_driver(iface->bss[0]);
	if (ret) {
		wpa_printf(MSG_ERROR, "hostapd_prepare_and_send_csa_deauth_cfg_to_driver failed: %s",
					iface->bss[0]->conf->iface);
		return ret;
	}
	for (i = 0; i < iface->num_bss; i++) {

		/* Save CHAN_SWITCH VHT config */
		hostapd_chan_switch_vht_config(
			iface->bss[i], settings.freq_params.vht_enabled);

		ret = hostapd_switch_channel(iface->bss[i], &settings);
		if (ret) {
			/* FIX: What do we do if CSA fails in the middle of
			 * submitting multi-BSS CSA requests? */
			return ret;
		}
	}

	return 0;
#else /* NEED_AP_MLME */
	return -1;
#endif /* NEED_AP_MLME */
}


static int hostapd_ctrl_iface_set_csa_deauth(struct hostapd_data *hapd,
					  char *pos)
{
	int ret;
	char *str, *pos2;

	hapd = get_bss_index(pos, hapd->iface);
	if (!hapd) {
		wpa_printf(MSG_INFO,
				"CTRL: SET_CSA_DEAUTH - there is no iface with the given name");
		return -1;
	}

	str = os_strstr(pos, "mode=");
	if (str) {
		str += sizeof("mode=") - 1;

		hapd->csa_deauth_mode = strtol(str, &pos2, 10);
		if (str == pos2 || hapd->csa_deauth_mode > CSA_DEAUTH_MODE_BROADCAST) {
			wpa_printf(MSG_ERROR, "csa deauth: invalid mode value provided");
			return -1;
		}
	}
	str = os_strstr(pos, "tx_time=");
	if (str) {
		ret = sscanf(str, "tx_time=%hu,%hu", &hapd->csa_deauth_tx_time[INTEL_CSA_DEAUTH_TX_TIME_UC_IDX],
				&hapd->csa_deauth_tx_time[INTEL_CSA_DEAUTH_TX_TIME_MC_IDX]);
		if (ret == 1) {
			wpa_printf(MSG_INFO, "csa deauth: tx_time input format is not correct");
			return 0;
		}
		if (ret == 0) {
			wpa_printf(MSG_INFO, "csa deauth: tx_time not configured, will be using defaults");
			return 0;
		}
		if (hapd->csa_deauth_tx_time[INTEL_CSA_DEAUTH_TX_TIME_UC_IDX] < hapd->iconf->beacon_int ||
			hapd->csa_deauth_tx_time[INTEL_CSA_DEAUTH_TX_TIME_MC_IDX] < hapd->iconf->beacon_int) {
			/* TODO how to identify max value */
			wpa_printf(MSG_ERROR, "csa deauth: tx time value can't be less than beacon interval");
			return -1;
		}
	}
	return 0;
}


static int hostapd_ctrl_iface_mib(struct hostapd_data *hapd, char *reply,
				  int reply_size, const char *param)
{
#ifdef RADIUS_SERVER
	if (os_strcmp(param, "radius_server") == 0) {
		return radius_server_get_mib(hapd->radius_srv, reply,
					     reply_size);
	}
#endif /* RADIUS_SERVER */
	return -1;
}


static int hostapd_ctrl_iface_vendor(struct hostapd_data *hapd, char *cmd,
				     char *buf, size_t buflen)
{
	int ret;
	char *pos;
	u8 *data = NULL;
	unsigned int vendor_id, subcmd;
	struct wpabuf *reply;
	size_t data_len = 0;

	/* cmd: <vendor id> <subcommand id> [<hex formatted data>] */
	vendor_id = strtoul(cmd, &pos, 16);
	if (!isblank((unsigned char) *pos))
		return -EINVAL;

	subcmd = strtoul(pos, &pos, 10);

	if (*pos != '\0') {
		if (!isblank((unsigned char) *pos++))
			return -EINVAL;
		data_len = os_strlen(pos);
	}

	if (data_len) {
		data_len /= 2;
		data = os_malloc(data_len);
		if (!data)
			return -ENOBUFS;

		if (hexstr2bin(pos, data, data_len)) {
			wpa_printf(MSG_DEBUG,
				   "Vendor command: wrong parameter format");
			os_free(data);
			return -EINVAL;
		}
	}

	reply = wpabuf_alloc((buflen - 1) / 2);
	if (!reply) {
		os_free(data);
		return -ENOBUFS;
	}

	ret = hostapd_drv_vendor_cmd(hapd, vendor_id, subcmd, data, data_len,
				     reply);

	if (ret == 0)
		ret = wpa_snprintf_hex(buf, buflen, wpabuf_head_u8(reply),
				       wpabuf_len(reply));

	wpabuf_free(reply);
	os_free(data);

	return ret;
}

#ifdef CONFIG_ALLOW_HOSTAPD
static int hostapd_ctrl_iface_eapol_reauth(struct hostapd_data *hapd,
					   const char *cmd)
{
	u8 addr[ETH_ALEN];
	struct sta_info *sta;

	if (hwaddr_aton(cmd, addr))
		return -1;

	sta = ap_get_sta(hapd, addr);
	if (!sta || !sta->eapol_sm)
		return -1;

	eapol_auth_reauthenticate(sta->eapol_sm);
	return 0;
}


static int hostapd_ctrl_iface_eapol_set(struct hostapd_data *hapd, char *cmd)
{
	u8 addr[ETH_ALEN];
	struct sta_info *sta;
	char *pos = cmd, *param;

	if (hwaddr_aton(pos, addr) || pos[17] != ' ')
		return -1;
	pos += 18;
	param = pos;
	pos = os_strchr(pos, ' ');
	if (!pos)
		return -1;
	*pos++ = '\0';

	sta = ap_get_sta(hapd, addr);
	if (!sta || !sta->eapol_sm)
		return -1;

	return eapol_auth_set_conf(sta->eapol_sm, param, pos);
}
#endif /* CONFIG_ALLOW_HOSTAPD */

static int hostapd_ctrl_iface_log_level(struct hostapd_data *hapd, char *cmd,
					char *buf, size_t buflen)
{
	char *pos, *end, *stamp;
	int ret;

	/* cmd: "LOG_LEVEL [<level>]" */
	if (*cmd == '\0') {
		pos = buf;
		end = buf + buflen;
		ret = os_snprintf(pos, end - pos, "Current level: %s\n"
				  "Timestamp: %d\n",
				  debug_level_str(wpa_debug_level),
				  wpa_debug_timestamp);
		if (os_snprintf_error(end - pos, ret))
			ret = 0;

		return ret;
	}

	while (*cmd == ' ')
		cmd++;

	stamp = os_strchr(cmd, ' ');
	if (stamp) {
		*stamp++ = '\0';
		while (*stamp == ' ') {
			stamp++;
		}
	}

	if (os_strlen(cmd)) {
		int level = str_to_debug_level(cmd);
		if (level < 0)
			return -1;
		wpa_debug_level = level;
	}

	if (stamp && os_strlen(stamp))
		wpa_debug_timestamp = atoi(stamp);

	os_memcpy(buf, "OK\n", 3);
	return 3;
}

#ifdef CONFIG_ALLOW_HOSTAPD
#ifdef NEED_AP_MLME
static int hostapd_ctrl_iface_track_sta_list(struct hostapd_data *hapd,
					     char *buf, size_t buflen)
{
	struct hostapd_iface *iface = hapd->iface;
	char *pos, *end;
	struct hostapd_sta_info *info;
	struct os_reltime now;

	if (!iface->num_sta_seen)
		return 0;

	sta_track_expire(iface, 0);

	pos = buf;
	end = buf + buflen;

	os_get_reltime(&now);
	dl_list_for_each_reverse(info, &iface->sta_seen,
				 struct hostapd_sta_info, list) {
		struct os_reltime age;
		int ret;

		os_reltime_sub(&now, &info->last_seen, &age);
		ret = os_snprintf(pos, end - pos, MACSTR " %u %d\n",
				  MAC2STR(info->addr), (unsigned int) age.sec,
				  info->ssi_signal);
		if (os_snprintf_error(end - pos, ret))
			break;
		pos += ret;
	}

	return pos - buf;
}
#endif /* NEED_AP_MLME */
#endif /* CONFIG_ALLOW_HOSTAPD */

#ifdef CONFIG_ACS
static int hostapd_ctrl_iface_acs_recalc(struct hostapd_iface *iface)
{
	struct intel_vendor_channel_data ch_data;

	memset(&ch_data, 0, sizeof(ch_data));
	hostapd_ltq_update_channel_data(iface, (const u8 *)&ch_data, sizeof(ch_data));
	return 0;
}
#endif

#ifdef CONFIG_ALLOW_HOSTAPD
static int hostapd_ctrl_iface_req_lci(struct hostapd_data *hapd,
				      const char *cmd)
{
	u8 addr[ETH_ALEN];

	if (hwaddr_aton(cmd, addr)) {
		wpa_printf(MSG_INFO, "CTRL: REQ_LCI: Invalid MAC address");
		return -1;
	}

	return hostapd_send_lci_req(hapd, addr);
}


static int hostapd_ctrl_iface_req_range(struct hostapd_data *hapd, char *cmd)
{
	u8 addr[ETH_ALEN];
	char *token, *context = NULL;
	int random_interval, min_ap;
	u8 responders[ETH_ALEN * RRM_RANGE_REQ_MAX_RESPONDERS];
	unsigned int n_responders;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_INFO,
			   "CTRL: REQ_RANGE - Bad destination address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token)
		return -1;

	random_interval = atoi(token);
	if (random_interval < 0 || random_interval > 0xffff)
		return -1;

	token = str_token(cmd, " ", &context);
	if (!token)
		return -1;

	min_ap = atoi(token);
	if (min_ap <= 0 || min_ap > WLAN_RRM_RANGE_REQ_MAX_MIN_AP)
		return -1;

	n_responders = 0;
	while ((token = str_token(cmd, " ", &context))) {
		if (n_responders == RRM_RANGE_REQ_MAX_RESPONDERS) {
			wpa_printf(MSG_INFO,
				   "CTRL: REQ_RANGE: Too many responders");
			return -1;
		}

		if (hwaddr_aton(token, responders + n_responders * ETH_ALEN)) {
			wpa_printf(MSG_INFO,
				   "CTRL: REQ_RANGE: Bad responder address");
			return -1;
		}

		n_responders++;
	}

	if (!n_responders) {
		wpa_printf(MSG_INFO,
			   "CTRL: REQ_RANGE - No FTM responder address");
		return -1;
	}

	return hostapd_send_range_req(hapd, addr, random_interval, min_ap,
				      responders, n_responders);
}
#endif /* CONFIG_ALLOW_HOSTAPD */

int hostapd_ctrl_iface_update_atf_cfg(struct hostapd_data *hapd)
{
	int ret_code = hostapd_drv_send_atf_quotas(hapd, NULL /*reread cfg file*/, 0, 0);
	return ret_code;
}

#ifdef CONFIG_MBO
static int hostapd_ctrl_iface_mbo_bss_assoc_disallow(struct hostapd_data *hapd, char *cmd)
{
	char *token, *context = NULL;
	int val, i;
	u8 bssid[ETH_ALEN];
	struct hostapd_data *target_bss = NULL;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, bssid)) {
		wpa_printf(MSG_INFO,
				"CTRL: MBO_BSS_ASSOC_DISALLOW - Bad bssid address");
		return -1;
	}

	for (i = 0; i < hapd->iface->num_bss; i++) {
		if (os_memcmp(bssid, hapd->iface->bss[i]->own_addr, ETH_ALEN) == 0) {
			target_bss = hapd->iface->bss[i];
			break;
		}
	}

	if (!target_bss) {
		wpa_printf(MSG_INFO,
				"CTRL: MBO_BSS_ASSOC_DISALLOW - there is no iface with the given bssid");
		return -1;
	}

	if (!target_bss->conf->mbo_enabled) {
		wpa_printf(MSG_INFO,
				"CTRL: MBO_BSS_ASSOC_DISALLOW - mbo is not enabled");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_INFO,
				"CTRL: MBO_BSS_ASSOC_DISALLOW - enable/disable is missing");
		return -1;
	}

	val = atoi(token);
	if (val < 0 || val > 5) {
		wpa_printf(MSG_INFO,
				"CTRL: MBO_BSS_ASSOC_DISALLOW - enable/disable value is out of range");
		return -1;
	}

	target_bss->mbo_bss_assoc_disallow = val;
	ieee802_11_update_beacons(target_bss->iface);

	return 0;
}

static int hostapd_ctrl_iface_show_neighbor(struct hostapd_data *hapd,
                                            char *buf, size_t buflen)
{
        if (!(hapd->conf->radio_measurements[0] &
              WLAN_RRM_CAPS_NEIGHBOR_REPORT)) {
                wpa_printf(MSG_ERROR,
                           "CTRL: SHOW_NEIGHBOR: Neighbor report is not enabled");
                return -1;
        }

        return hostapd_neighbor_show(hapd, buf, buflen);
}


static int hostapd_ctrl_iface_cellular_pref_set(struct hostapd_data *hapd, char *cmd)
{
	char *token, *context = NULL;
	int val;

	hapd = get_bss_index(cmd, hapd->iface);
	if (!hapd) {
		wpa_printf(MSG_INFO,
				"CTRL: CELLULAR_PREF_SET - there is no iface with the given name");
		return -1;
	}

	if (!hapd->conf->mbo_enabled) {
		wpa_printf(MSG_INFO,
				"CTRL: CELLULAR_PREF_SET - mbo is not enabled");
		return -1;
	}

	if (!hapd->conf->mbo_cell_aware) {
		wpa_printf(MSG_INFO,
				"CTRL: CELLULAR_PREF_SET - AP is not cellular aware");
		return -1;
	}

	cmd = os_strchr(cmd, ' ');
	if (cmd)
		cmd++;
	else {
		wpa_printf(MSG_INFO,
				"CTRL: CELLULAR_PREF_SET - Cellular Preference is missing");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_INFO,
				"CTRL: CELLULAR_PREF_SET - Cellular Preference is missing");
		return -1;
	}

	val = atoi(token);
	if (val != MBO_CELL_PREF_EXCLUDED && val != MBO_CELL_PREF_NO_USE && val != MBO_CELL_PREF_USE) {
		wpa_printf(MSG_INFO,
				"CTRL: CELLULAR_PREF_SET - Cellular Preference value is out of range (can be 0, 1 or 255)");
		return -1;
	}

	hapd->mbo_cell_pref = val;

	return 0;
}
#endif /* CONFIG_MBO */

static int hostapd_ctrl_iface_req_beacon(struct hostapd_data *hapd, char *cmd,
		char *buf, size_t buflen)
{
	u8 addr[ETH_ALEN], bssid[ETH_ALEN];
	char *token, *context = NULL;
	char *pos, *cmd_end;
	int op_class, channel, random_interval, mode, measurement_duration;
	rsize_t len;
	struct wpa_ssid_value ssid, *ssid_p = NULL;
	int num_of_repetitions, measurement_request_mode, last_indication = 0;
	int ret_dialog_token;
	char *buffer_end_address = cmd + MAX_BUFF_LEN - sizeof("REQ_BEACON ") - 1;

	u8 rep_cond, rep_cond_threshold;
	u8* rep_cond_p = NULL, *rep_cond_threshold_p = NULL;
	u8 rep_detail;
	u8* rep_detail_p = NULL;
	u8 ap_ch_rep[255];
	u8* ap_ch_rep_p = NULL;
	unsigned int ap_ch_rep_len = 0;
	u8 req_elem[255];
	u8* req_elem_p = NULL;
	unsigned int req_elem_len = 0;

	u8 channel_width, channel_center_frequency0, channel_center_frequency1;
	u8 *channel_width_p = NULL;
	u8 *channel_center_frequency0_p = NULL, *channel_center_frequency1_p = NULL;

	len = strnlen_s(cmd, buffer_end_address - cmd);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - cmd - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - wrong len");
		return -1;
	}
	cmd_end = cmd + len;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - Bad destination address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
				"CTRL: REQ_BEACON - number of repetitions is missing");
		return -1;
	}
	num_of_repetitions = atoi(token);
	if (!IS_VALID_U16_RANGE(num_of_repetitions)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_BEACON - number of repetitions is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_BEACON - measurement request mode is missing");
		return -1;
	}
	measurement_request_mode = atoi(token);
	if (!IS_VALID_U8_RANGE(measurement_request_mode)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_BEACON - measurement request mode is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_BEACON - operating class is missing");
		return -1;
	}

	op_class = atoi(token);

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - channel is missing");
		return -1;
	}

	channel = atoi(token);
	/* Quote from 80211-2016, Chapter 9.4.2.21.7 Beacon request:
	  "For operating classes that encompass a primary channel but do not identify the location of the primary
	  channel, the Channel Number field value is either 0 or 255; otherwise, the Channel Number field value is 0,
	  255, or the channel number for which the measurement request applies and is defined within an operating
	  class as shown in Annex E." */
	if(channel != 0 && channel != 255) {
		if (ieee80211_chan_to_freq(NULL, op_class, channel) < 0) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_BEACON - channel/op_class combination invalid");
			return -1;
		}
	} else {
		if (!IS_VALID_U8_RANGE(op_class)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_BEACON - operation class is out of range");
			return -1;
		}
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_BEACON - random interval is missing");
		return -1;
	}
	random_interval = atoi(token);
	if (!IS_VALID_U16_RANGE(random_interval)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_BEACON - random interval out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_BEACON - measurement duration is missing");
		return -1;
	}
	measurement_duration = atoi(token);
	if (!IS_VALID_U16_RANGE(measurement_duration)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REQ_BEACON - measurement duration out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - mode is missing");
		return -1;
	}
	if (!os_strcmp(token, "passive")) {
		mode = 0;
	} else if (!os_strcmp(token, "active")) {
		mode = 1;
	} else if (!os_strcmp(token, "table")) {
		mode = 2;
	} else {
		wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - mode is invalid");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, bssid)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - bad BSSID address");
		return -1;
	}

	len = strnlen_s(token, buffer_end_address - token);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - token - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - wrong len");
		return -1;
	}
	/* optional tokens */
	token = token + len + 1;
	if (token >= cmd_end) {
		/* we've reached the end of command (no optional arguments) */
		goto exit;
	}

	pos = os_strstr(token, "ssid=");
	if (pos) {
		pos += 5;
		if (ssid_parse(pos, &ssid)) {
			wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - bad SSID");
			return -1;
		}
		ssid_p = &ssid;
	}

	pos = os_strstr(token, "beacon_rep=");
	if (pos) {
		int cond, threshold;
		char *end, *comma;
		pos += 11;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos -1)) {
				wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma = os_strchr(pos, ',');
		if (!comma || comma == pos || comma >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_BEACON - report condition format is invalid");
			return -1;
		}
		cond = atoi(pos);
		if (!IS_VALID_RANGE(cond, 0, 10)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_BEACON - report condition is invalid");
			return -1;
		}
		comma++;
		threshold = atoi(comma);
		if (!IS_VALID_U8_RANGE(threshold)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_BEACON - report condition threshold is invalid");
			return -1;
		}
		rep_cond = (u8) cond;
		rep_cond_p = &rep_cond;
		rep_cond_threshold = (u8) threshold;
		rep_cond_threshold_p = &rep_cond_threshold;
	}

	pos = os_strstr(token, "rep_detail=");
	if (pos) {
		pos += 11;
		if ((pos[1] != ' ' && pos[1] != '\0') ||
			(pos[0] != '0' && pos[0] != '1' && pos[0] != '2')) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_BEACON - report detail value is invalid");
			return -1;
		}
		rep_detail = *pos - '0';
		rep_detail_p = &rep_detail;
	}

	pos = os_strstr(token, "ap_ch_report=");
	if (pos) {
		int ch;
		char *end;
		pos += 13;
		ap_ch_rep_len = 0;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - wrong len");
				return -1;
			}
			end = pos + len;
		}
		while (pos && pos < end) {
			ch = atoi(pos);
			ap_ch_rep[ap_ch_rep_len++] = ch;
			pos = os_strchr(pos, ',');
			if (pos)
				pos++;
		}
		if (ap_ch_rep_len) {
			ap_ch_rep_p = ap_ch_rep;
		} else {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_BEACON - ap channel report invalid values");
			return -1;
		}
	}

	pos = os_strstr(token, "req_elements=");
	if (pos) {
		int elem;
		char* end;
		pos += 13;
		req_elem_len = 0;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - wrong len");
				return -1;
			}
			end = pos + len;
		}
		while (pos && pos < end) {
			elem = atoi(pos);
			req_elem[req_elem_len++] = elem;
			pos = os_strchr(pos, ',');
			if (pos)
				pos++;
		}
		if (req_elem_len) {
			req_elem_p = req_elem;
		}
	}

	pos = os_strstr(token, "wide_band_ch_switch=");
	if (pos) {
		int ch_width, ch_center_freq0, ch_center_freq1;
		char *end, *comma1, *comma2;
		pos += 20;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma1 = os_strchr(pos, ',');
		if (!comma1 || comma1 == pos || comma1 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_BEACON - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma1++;
		comma2 = os_strchr(comma1, ',');
		if (!comma2 || comma2 == comma1 || comma2 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_BEACON - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma2++;
		ch_width = atoi(pos);
		/* Quote from IEE80211-2016, Chapter 9.4.2.161 Wide Bandwidth Channel Switch element:
		   "The subfields New Channel Width, New Channel Center Frequency Segment 0, and New Channel Center
		   Frequency Segment 1 have the same definition, respectively, as Channel Width, Channel Center Frequency
		   Segment 0, and Channel Center Frequency Segment 1 in the VHT Operation Information field, described in
		   Table 9-252." */
		if (!IS_VALID_RANGE(ch_width, VHT_OPER_CHANWIDTH_20_40MHZ, VHT_OPER_CHANWIDTH_80P80MHZ)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_BEACON - channel width is invalid");
			return -1;
		}

		ch_center_freq0 = atoi(comma1);
		/* According to IEE80211-2016, Chapter 21.3.14 Channelization */
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq0) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REQ_BEACON - channel center frequency 0 is invalid");
		} else {
			channel_center_frequency0 = (u8) ch_center_freq0;
			channel_center_frequency0_p = &channel_center_frequency0;
		}

		ch_center_freq1 = atoi(comma2);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq1) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REQ_BEACON - channel center frequency 1 is invalid");
		} else {
			channel_center_frequency1 = (u8) ch_center_freq1;
			channel_center_frequency1_p = &channel_center_frequency1;
		}

		channel_width = (u8) ch_width;
		channel_width_p = &channel_width;
	}
	pos = os_strstr(token, "last_indication=");
	if (pos) {
		pos += 16;
		last_indication = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(last_indication)) {
			wpa_printf(MSG_ERROR,
				"CTRL: REQ_BEACON - last_indication invalid values");
			return -1;
		}
	}

exit:
	ret_dialog_token = hostapd_send_beacon_req(hapd, addr, num_of_repetitions,
			measurement_request_mode, op_class, channel, random_interval,
			measurement_duration, mode, bssid, ssid_p, rep_cond_p,
			rep_cond_threshold_p,rep_detail_p, ap_ch_rep_p, ap_ch_rep_len,
			req_elem_p, req_elem_len, channel_width_p,
			channel_center_frequency0_p, channel_center_frequency1_p,
			last_indication);

	if (ret_dialog_token < 0)
		return -1;
	return sprintf_s(buf, buflen, "dialog_token=%d\n", ret_dialog_token);
}

static int hostapd_ctrl_iface_report_beacon(struct hostapd_data *hapd,
		char *cmd)
{
	char *token, *context = NULL;
	char *pos, *cmd_end;
	u8 addr[ETH_ALEN], bssid[ETH_ALEN];
	int op_class, channel, measurement_duration, dialog_token;
	int measurement_token, measurement_rep_mode;
	rsize_t len;
	int frame_info, rcpi, rsni, antenna_id;
	u32 parent_tsf;
	u64 start_time;
	s64 val;
	char *buffer_end_address = cmd + MAX_BUFF_LEN - sizeof("REPORT_BEACON ") - 1;

	u8 channel_width, channel_center_frequency0, channel_center_frequency1;
	u8 *channel_width_p = NULL;
	u8 *channel_center_frequency0_p = NULL, *channel_center_frequency1_p = NULL;
	u8 rep_frame_body_length,  rep_frame_body[256];
	u8 *rep_frame_body_p = NULL, *rep_frame_body_length_p = NULL;

	len = strnlen_s(cmd, buffer_end_address - cmd);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - cmd - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REP_BEACON - wrong len");
		return -1;
	}
	cmd_end = cmd + len;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_ERROR, "CTRL: REP_BEACON - Bad destination address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - dialog token is missing");
		return -1;
	}
	dialog_token = atoi(token);
	if (!IS_VALID_U8_RANGE(dialog_token)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - dialog token is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - measurement token is missing");
		return -1;
	}
	measurement_token = atoi(token);
	if (!IS_VALID_RANGE(measurement_token, WLAN_RRM_RADIO_MEASUREMENT_REPORT_TOKEN_VALUE_MIN,
						WLAN_RRM_RADIO_MEASUREMENT_REPORT_TOKEN_VALUE_MAX)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - measurement token is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - measurement mode is missing");
		return -1;
	}
	measurement_rep_mode = atoi(token);
	if (!IS_VALID_U8_RANGE(measurement_rep_mode)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - measurement mode is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - operation class is missing");
		return -1;
	}

	op_class = atoi(token);

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - channel is missing");
		return -1;
	}
	channel = atoi(token);
	/* Quote from IEEE80211-16, 9.4.2.22.7 Beacon report
	   "The Operating Class field indicates the operating class that identifies the channel set of the received Beacon
	   or Probe Response frame. The Country, Operating Class, and Channel Number fields together specify the
	   channel frequency and spacing of the received Beacon or Probe Response frame. Valid operating classes are
	   listed in Annex E." */
	if (ieee80211_chan_to_freq(NULL, op_class, channel) < 0) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - channel/op_class combination is invalid");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - start time is missing");
		return -1;
	}

	/* Any unsigned long long value
	   returned by strtoull is within valid range. */
	start_time = strtoull(token, NULL, 10);
	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - measurement duration is missing");
		return -1;
	}
	measurement_duration = atoi(token);
	if (!IS_VALID_U16_RANGE(measurement_duration)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REQ_BEACON - measurement duration out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - frame info is missing");
		return -1;
	}
	frame_info = atoi(token);
	if (!IS_VALID_U8_RANGE(frame_info)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - frame info is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - rcpi is missing");
		return -1;
	}
	rcpi = atoi(token);
	if (!IS_VALID_U8_RANGE(rcpi)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - rcpi is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - rsni is missing");
		return -1;
	}
	rsni = atoi(token);
	if (!IS_VALID_U8_RANGE(rsni)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - rsni is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, bssid)) {
		wpa_printf(MSG_ERROR, "CTRL: REP_BEACON - bad BSSID address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - antenna id is missing");
		return -1;
	}
	antenna_id = atoi(token);
	if (!IS_VALID_U8_RANGE(antenna_id)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - antenna id is out of range");
		return -1;
	}


	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REP_BEACON - parent tsf is missing");
		return -1;
	}

	val = atoll(token);
	if (!IS_VALID_U32_RANGE(val)) {
		   wpa_printf(MSG_ERROR,
					  "CTRL: REP_BEACON - parent tsf is out of range");
		   return -1;
	}
	parent_tsf = (u32)val;

	len = strnlen_s(token, buffer_end_address - token);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - token - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REP_BEACON - wrong len");
		return -1;
	}
	/* optional tokens */
	token = token + len + 1;
	if (token >= cmd_end) {
		/* we've reached the end of command (no optional arguments) */
		goto exit;
	}

	pos = os_strstr(token, "rep_frame_body=");
	if (pos) {
		int length;
		errno_t err;
		char *end, *comma;
		pos += 15;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REP_BEACON - wrong len");
				return -1;
			}	
			end = pos + len;
		}
		comma = os_strchr(pos, ',');
		if (!comma || comma == pos || comma >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REP_BEACON - reported frame body format is invalid");
			return -1;
		}
		comma++;
		length = atoi(pos);
		if (!IS_VALID_U8_RANGE(length)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REP_BEACON - reported frame body length is invalid");
			return -1;
		}
		err = memcpy_s(rep_frame_body, sizeof(rep_frame_body), comma, length);
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: CTRL: REP_BEACON - memcpy_s error %d", __func__, err) ;
			return -1;
		}
		rep_frame_body_length = (u8) length;
		rep_frame_body_length_p = &rep_frame_body_length;
		rep_frame_body_p = (u8 *)rep_frame_body;
	}

	pos = os_strstr(token, "wide_band_ch_switch=");
	if (pos) {
		int ch_width, ch_center_freq0, ch_center_freq1;
		char *end, *comma1, *comma2;
		pos += 20;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REP_BEACON - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma1 = os_strchr(pos, ',');
		if (!comma1 || comma1 == pos || comma1 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REP_BEACON - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma1++;
		comma2 = os_strchr(comma1, ',');
		if (!comma2 || comma2 == comma1 || comma2 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REP_BEACON - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma2++;
		ch_width = atoi(pos);
		/* Quote from IEE80211-2016: "The subfields New Channel Width, New Channel Center Frequency Segment 0, and New Channel Center
		   Frequency Segment 1 have the same definition, respectively, as Channel Width, Channel Center Frequency
		   Segment 0, and Channel Center Frequency Segment 1 in the VHT Operation Information field, described in
		   Table 9-252." */
		if (!IS_VALID_RANGE(ch_width, VHT_OPER_CHANWIDTH_20_40MHZ, VHT_OPER_CHANWIDTH_80P80MHZ)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REP_BEACON - channel width is invalid");
			return -1;
		}
		ch_center_freq0 = atoi(comma1);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq0) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REPORT_CHANNEL_LOAD - channel center frequency 0 is invalid");
		} else {
			channel_center_frequency0 = (u8) ch_center_freq0;
			channel_center_frequency0_p = &channel_center_frequency0;
		}

		ch_center_freq1 = atoi(comma2);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq1) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REP_BEACON - channel center frequency 1 is invalid");
		} else {
			channel_center_frequency1 = (u8) ch_center_freq1;
			channel_center_frequency1_p = &channel_center_frequency1;
		}

		channel_width = (u8) ch_width;
		channel_width_p = &channel_width;
	}

exit:
	return hostapd_send_beacon_report_resp(hapd, addr, dialog_token,
			measurement_token, measurement_rep_mode, op_class, channel,
			start_time, measurement_duration, frame_info, rcpi, rsni,
			bssid, antenna_id, parent_tsf, channel_width_p,
			channel_center_frequency0_p, channel_center_frequency1_p,
			rep_frame_body_p, rep_frame_body_length_p);
}

static int hostapd_ctrl_iface_req_self_beacon(struct hostapd_data *hapd,
		char *cmd)
{
	char *token, *context = NULL;
	int random_interval, mode, measurement_duration;

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_SELF_BEACON - random interval is missing");
		return -1;
	}
	random_interval = atoi(token);
	if (!IS_VALID_U16_RANGE(random_interval)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_SELF_BEACON - random interval out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_SELF_BEACON - measurement duration is missing");
		return -1;
	}
	measurement_duration = atoi(token);
	if (!IS_VALID_U16_RANGE(measurement_duration)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REQ_SELF_BEACON - measurement duration out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_SELF_BEACON - mode is missing");
		return -1;
	}
	if (!os_strcmp(token, "passive")) {
		mode = 0;
	} else if (!os_strcmp(token, "active")) {
		mode = 1;
	} else if (!os_strcmp(token, "table")) {
		mode = 2;
	} else {
		wpa_printf(MSG_ERROR, "CTRL: REQ_BEACON - mode is invalid");
		return -1;
	}

	return hostapd_handle_self_beacon_req(hapd, random_interval,
			measurement_duration, mode);
}

static int hostapd_ctrl_iface_set_zwdfs_antenna(struct hostapd_data *hapd,
						char *cmd)
{
	int enable = atoi(cmd), enabled;
	struct hostapd_iface *iface = hapd->iface;
	struct hostapd_config *conf = iface->conf;
	int needs_restart = 0;

	if (!IS_VALID_BOOL_RANGE(enable)) {
		wpa_printf(MSG_ERROR, "CTRL: ZWDFS_ANT_SWITCH - Bad configuration option");
		return -1;
	}

	if (hostapd_drv_get_zwdfs_antenna(hapd, &enabled)) {
		wpa_printf(MSG_ERROR, "Could not get current zwdfs antenna state");
		return -1;
	}

	if (enabled == enable) {
		wpa_printf(MSG_WARNING, "zwdfs antenna state is already in requested state");
		return 0;
	}

	needs_restart = !(iface->state == HAPD_IFACE_ENABLED);

	if (set_first_non_dfs_channel(iface)) {
		wpa_printf(MSG_ERROR, "Could not set channel");
		return -1;
	}

	iface->conf->dynamic_failsafe = 0; /* turn off dynamic failsafe */

	/* set failsafe channel to channel retreived by set_first_non_dfs_channel().
	 * this + disabling dynamic failsafe, will make sure zwdfs radio will not
	 * jump to another DFS channel upon radar hit
	 */
	memset(&iface->failsafe, 0, sizeof(iface->failsafe));
	iface->failsafe.freq = iface->freq;
	iface->failsafe.center_freq1 = iface->freq;
	iface->failsafe.center_freq2 = 0;
	iface->failsafe.bandwidth = 20;

	if (needs_restart) {
		iface->whm_if_fail = false;
		hostapd_disable_iface(iface);
	}
	else
		hostapd_drv_stop_ap(hapd);

	if (!needs_restart &&
	    hostapd_set_freq(hapd, hapd->iconf->hw_mode, iface->freq,
			     conf->channel, conf->enable_edmg,
			     conf->edmg_channel, conf->ieee80211n,
			     conf->ieee80211ac, conf->ieee80211ax,
			     conf->secondary_channel, hostapd_get_oper_chwidth(conf),
			     hostapd_get_oper_centr_freq_seg0_idx(conf),
			     hostapd_get_oper_centr_freq_seg1_idx(conf))) {
		wpa_printf(MSG_ERROR, "Could not set channel for kernel driver");
		return -1;
	} else if (needs_restart) {
		if (hostapd_enable_iface(iface)) {
			wpa_printf(MSG_ERROR, "Could not enable interface");
			return -1;
		}

		hostapd_drv_stop_ap(hapd);
	}

	if (hostapd_drv_set_zwdfs_antenna(hapd, enable) < 0) {
		wpa_printf(MSG_ERROR, "Failed to enable ZWDFS antenna");
		return -1;
	}

	if (ieee802_11_set_beacon(hapd)) {
		wpa_printf(MSG_ERROR, "Failed to reload ZWDFS interface");
		return -1;
	}

	wpa_msg(hapd->msg_ctx, MSG_INFO, enable ? ZWDFS_ANT_ENABLE : ZWDFS_ANT_DISABLE);

	return 0;
}

static int hostapd_ctrl_iface_get_zwdfs_antenna(struct hostapd_data *hapd,
						char *reply, size_t reply_size)
{
	int res = 0;
	int enabled;

	if (hostapd_drv_get_zwdfs_antenna(hapd, &enabled))
		return -1;

	res = sprintf_s(reply, reply_size, "%d\n", enabled);
	if (res <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, res);
		return -1;
	}

	return res;
}

static int delete_rnr_from_beacon(struct hostapd_data *hapd, int radio_index, int vap_index)
{
	/* if more than 1 vap, then let radio_info available, only reset vap_info 
	   if only 1 vap, for which delete is called, meaning now no more vap, so clear complete rnr */
	if ( hapd->iconf->coloc_6g_ap_info[radio_index].TbttInfoCount > 1 ) 
	{
		hapd->iconf->coloc_6g_ap_info[radio_index].TbttInfoCount--;
		os_memset(( void *) &hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index], '\0',
				 sizeof(hapd->iconf->coloc_6g_ap_info[radio_index].bss_info[vap_index]));
	}
	else
		os_memset((void *)&hapd->iconf->coloc_6g_ap_info[radio_index], '\0',
				 sizeof(hapd->iconf->coloc_6g_ap_info[radio_index]));
		
	


	if( ieee802_11_update_beacon(hapd) )
	{
		wpa_printf(MSG_ERROR,"CTRL: SET_BEACON failed");
		return -1;
	}

	return 0;

}

static int hostapd_ctrl_iface_update_rnr(struct hostapd_data *hapd,
                char *cmd)
{
	struct update_rnr rnr = { '\0' };
	char *tmp, *ptr = NULL;
	int i = 0 ,j = 0, ret = 0, param_val = 0;
	bool found = false; // flag will be used to find whether bssid entry found
	char empty_bssid[ETH_ALEN] = { '\0' };
	errno_t err = EOK;

	if( NULL == cmd )
	{
		wpa_printf(MSG_ERROR,"CTRL: UPDATE_RNR: not valid cmd");
		return -1;
	}


	/* expected order: BSSID OperatingClass Channel SSID multibss_enable is_transmitted_bssid
				max_tx_power hidden_mode*/

	PARSE_TOKEN(tmp,cmd," ",&ptr);
	if (hwaddr_aton(tmp, rnr.bssid)) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad %s BSSID", tmp);
		return -1;
	}

	if( !os_memcmp( rnr.bssid, empty_bssid, ETH_ALEN)) 
	{
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: All 0's BSSID, not valid?");
		return -1;
	}

	PARSE_TOKEN(tmp,cmd," ",&ptr);
	param_val = atoi(tmp);
	if( ( param_val < HOSTAP_6GHZ_20MHZ_OP_CLASS ) ||
		 ( param_val > HOSTAP_6GHZ_80P80MHZ_OP_CLASS ) ) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad %s OperatingClass", tmp);
		return -1;
	}
	rnr.OperatingClass = param_val;

	PARSE_TOKEN(tmp,cmd," ",&ptr);
	param_val = atoi(tmp);
	if( ( param_val < HOSTAP_6GHZ_CHANNEL_FIRST ) ||
		 ( param_val > HOSTAP_6GHZ_CHANNEL_LAST ) ) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad %s Channel", tmp);
		return -1;
	}
	rnr.Channel = param_val;

	PARSE_TOKEN(tmp,cmd," ",&ptr);
	if( strnlen_s(tmp, RSIZE_MAX_STR) >= SSID_MAX_LEN  ) {
		wpa_printf(MSG_ERROR,
		   "CTRL: UPDATE_RNR: Bad %s SSID", tmp);
		return -1;
	}
	err = memcpy_s(rnr.ssid, SSID_MAX_LEN, tmp, strnlen_s(tmp, RSIZE_MAX_STR));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return -1;
	}

	PARSE_TOKEN(tmp,cmd," ",&ptr);
	param_val = atoi(tmp);
	if( ( param_val < false ) ||
		 ( param_val > true ) ) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad multlbss_enable %s", tmp);
		return -1;
	}
	rnr.multibss_enable = param_val;

	PARSE_TOKEN(tmp,cmd," ",&ptr);
	param_val = atoi(tmp);
	if( ( param_val < false ) ||
		 ( param_val > true ) ) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad is_transmitted_bssid %s", tmp);
		return -1;
	}
	rnr.is_transmitted_bssid = param_val;

	PARSE_TOKEN(tmp,cmd," ",&ptr);
	param_val = atoi(tmp);
	if( ( param_val < false ) ||
		 ( param_val > true ) ) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad unsolicited_frame_support %s", tmp);
		return -1;
	}
	rnr.unsolicited_frame_support = param_val;

	PARSE_TOKEN(tmp,cmd," ",&ptr);
	param_val = ( atoi(tmp) - HOSTAP_6GHZ_10LOG_20MHZ ) * HOSTAP_6GHZ_CONVERT_HALF_DB_UNIT;
	if( ( param_val < HOSTAP_6GHZ_PSD_20MHZ_MIN ) ||
		 ( param_val > HOSTAP_6GHZ_PSD_20MHZ_MAX ) ) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad %s psd 20MHz", tmp);
		return -1;
	}
	if ( param_val < 0 )
		rnr.psd_20MHz = param_val + HOSTAPD_BYTE_2S_COMPLEMENT; // 2's complement
	else
		rnr.psd_20MHz = param_val;

	PARSE_TOKEN(tmp,cmd," ",&ptr);
	param_val = atoi(tmp);
	if( ( param_val < false ) ||
		( param_val > true ) ) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad hidden_mode %s", tmp);
		return -1;
	}
	rnr.hidden_mode = param_val;

	/* Update for existing RNR ? */
	for( i = 0; ( i <  MAX_SUPPORTED_6GHZ_RADIO ) && ( found == false );  )
	{ 
		for (  j = 0; ( j < MAX_SUPPORTED_6GHZ_VAP_PER_RADIO ) && !found; )
		{
			if( !os_memcmp(hapd->iconf->coloc_6g_ap_info[i].bss_info[j].bssid , rnr.bssid,ETH_ALEN) )
			{ 
				found = true;
				wpa_printf(MSG_DEBUG,"CTRL: UPDATE_RNR: Found bssid entry");
			}
			else
				j++;

		}
		
		if( found == false )
			i++;
	}

	/* not existing RNR, find empty buffer in array and update new RNR */
	if( found == false )
	{ /*TODO (6GHz) how to find new radio or existing? channel/op_class can be same, coloc as of now hardcoded so can't use for knowing if new radio
		  current limitation we support only one radio, so good for now, once we support multiple radio and support coloc flag dyanmic,
		  need to add check here to understand new radio */
		for( i = 0; ( i < MAX_SUPPORTED_6GHZ_RADIO ) && !found;  )
		{
			for (  j = 0; ( j < MAX_SUPPORTED_6GHZ_VAP_PER_RADIO ) && !found ;  ) 
			{
				char empty_bssid[ETH_ALEN] = {'\0'};
				if( !os_memcmp( hapd->iconf->coloc_6g_ap_info[i].bss_info[j].bssid, empty_bssid, ETH_ALEN)) 
				{
					found = true;
					wpa_printf(MSG_DEBUG,"CTRL: UPDATE_RNR: Found empty bssid entry");
				}
				else
					j++;
			}
			if( found  == false )
				i++;
		}
	}

	if( found == false )
	{
		wpa_printf(MSG_ERROR,"CTRL: UPDATE_RNR: No availble space to hold new rnr report");
		return -1;
	}
	wpa_msg(hapd->msg_ctx, MSG_INFO, UPDATE_RNR);

	if( hapd == hapd->iface->bss[0] )
	{ 
		int k = 0;
		/* loop through all vap and update beacon for each of them with RNR inclusion */
		for (k = 0; k < hapd->iface->num_bss; k++) 
		{
			wpa_printf(MSG_DEBUG,"CTRL: UPDATE_RNR: No. of vaps %d", (int)hapd->iface->num_bss);
			struct hostapd_data *h = hapd->iface->bss[k];
			if ( h )
			{
				if ( ( ret = update_rnr_in_beacon(h, rnr, i, j) ) )
					break;
				
			}
			
		}
	}
	else
		ret = update_rnr_in_beacon(hapd, rnr, i, j); /* update came for only slave vap */

	return ret;
}

static int hostapd_ctrl_iface_delete_rnr(struct hostapd_data *hapd,
                char *cmd)
{
	struct delete_rnr rnr;
	int i = 0,j = 0,ret = 0;	
	bool found = false; // flag will be used to find whether bssid entry found

	if( NULL == cmd )
	{
		wpa_printf(MSG_ERROR,"CTRL: DELETE_RNR: not valid cmd");
		return -1;
	}

	if (hwaddr_aton(cmd, rnr.bssid))
	{
		wpa_printf(MSG_ERROR, "CTRL: DELETE_RNR: Bad  %s BSSID",cmd);
		return -1;
	}

	/* Find do we hold the entry for RNR ? */
	for( i = 0; ( i < MAX_SUPPORTED_6GHZ_RADIO ) && ( found == false );  )
	{
		for (  j = 0; ( j < MAX_SUPPORTED_6GHZ_VAP_PER_RADIO ) && !found; )
		{
			if( !os_memcmp(hapd->iconf->coloc_6g_ap_info[i].bss_info[j].bssid , rnr.bssid,ETH_ALEN) )
				found = true;
			else
				j++;
		}
		if( found == false )
			i++;
	}

	if( found == false )
	{
		wpa_printf(MSG_ERROR,"CTRL: delete_rnr %s entry doesn't exist",cmd);
		return -1;
	}

	if( delete_rnr_from_beacon( hapd, i, j ) )
		return -1;	
	
	wpa_msg(hapd->msg_ctx, MSG_INFO, DELETE_RNR);


	 /* delete_rnr called for master vap, propogate same to all vaps */
	if( hapd == hapd->iface->bss[0] )
	{
		int k = 0;
		struct hostapd_data *h;

		/* loop through all vap and update beacon for each of them with RNR exclusion */
		wpa_printf(MSG_ERROR,"CTRL: delete_rnr received on master vap");
		for (k = 1; k < hapd->iface->num_bss; k++) /* for 0th already sent */
		{
			h = hapd->iface->bss[k];
			if( h )
			{
				if( delete_rnr_from_beacon( h, i, j ) )
					return -1;	
			}
		}
	}

	return ret;
}

static int hostapd_ctrl_iface_get_multibss_enable(struct hostapd_data *hapd, char *reply,
							size_t reply_size)
{
#define HOSTAPD_MULTIBSS_ENABLE 1
#define HOSTAPD_MULTIBSS_DISABLE 0
	int ret = 0;
	if(hapd->iconf->multibss_enable)
		ret = sprintf_s(reply, reply_size, "%d", HOSTAPD_MULTIBSS_ENABLE);
	else
		ret = sprintf_s(reply, reply_size, "%d", HOSTAPD_MULTIBSS_DISABLE);

	return ret;

}

static int hostapd_ctrl_iface_is_transmitted_bssid(struct hostapd_data *hapd, char *reply,
							size_t reply_size)
{
#define HOSTAPD_TRANSMITTED_BSSID 1
#define HOSTAPD_NON_TRANSMITTED_BSSID 0
	int ret = 0;
	if(hapd->iconf->multibss_enable)
	{
		if (MULTIBSS_REFERENCE_BSS_IDX == hapd->conf->bss_idx)
			ret = sprintf_s(reply, reply_size, "%d", HOSTAPD_TRANSMITTED_BSSID);
		else
			ret = sprintf_s(reply, reply_size, "%d", HOSTAPD_NON_TRANSMITTED_BSSID);
	}
	else
		ret= sprintf_s(reply, reply_size, "%d", HOSTAPD_NON_TRANSMITTED_BSSID);

	return ret;
		
}

static int hostapd_ctrl_iface_get_unsolicited_frame_duration(struct hostapd_data *hapd, char *reply,
								size_t reply_size)
{
	int ret = 0;
	ret = sprintf_s(reply, reply_size, "%d", hapd->iconf->unsolicited_frame_duration);
	if (ret <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
		return -1;
	}
	return ret;
}

static int hostapd_ctrl_iface_set_unsolicited_frame_duration(struct hostapd_data *hapd, char *cmd)
{
	int val = atoi(cmd);
	if (!IS_VALID_RANGE(val, MIN_UNSOLICITED_FRAME_DURATION + 1, MAX_UNSOLICITED_FRAME_DURATION))
		return -1;
	else if ((hapd->iconf->num_bss == MIN_NUM_BSS_IN_NON_MBSSID_SET) || (hapd->iconf->multibss_enable)) {
			if (hapd->iconf->unsolicited_frame_support) {
				if(hostapd_drv_set_unsolicited_frame(hapd, hapd->iconf->unsolicited_frame_support,
													 val) < 0)
						return -1;
			}
			hapd->iconf->unsolicited_frame_duration = val;
	} else {
		wpa_printf(MSG_ERROR, "CTRL: SET_UNSOLICITED_FRAME_DURATION not supported as multiple VAPs are present but MultiBSS mode is disabled");
		return -1;
	}
	return 0;
}

static int hostapd_ctrl_iface_get_unsolicited_frame_support(struct hostapd_data *hapd, char *reply,
								size_t reply_size)
{
	int ret = 0;
	ret = sprintf_s(reply, reply_size, "%d", hapd->iconf->unsolicited_frame_support);
	if (ret <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
		return -1;
	}
	return ret;
}

static int hostapd_ctrl_iface_set_unsolicited_frame_support(struct hostapd_data *hapd, char *cmd)
{
	int val = atoi(cmd);
	if (!IS_VALID_RANGE(val, DISABLE_UNSOLICITED_FRAME_SUPPORT, FILS_DISCOVERY_ENABLE)) {
		wpa_printf(MSG_ERROR, "CTRL: Bad unsolicited_frame_support %s"
				" choose 0:Disable, 1:20TU_Probe_response, 2:FILS Discovery", cmd);
		return -1;
	} else if ((hapd->iconf->num_bss == MIN_NUM_BSS_IN_NON_MBSSID_SET) || (hapd->iconf->multibss_enable)) {
		hapd->iconf->unsolicited_frame_support = val;
		if( hostapd_drv_set_unsolicited_frame(hapd, val, hapd->iconf->unsolicited_frame_duration) < 0 )
			return -1;
		if ( val == FRAME_20TU_PROBE_RESPONSE_ENABLE )
			hostapd_send_event_unsolicited_probe_response(hapd);
	} else {
		wpa_printf(MSG_ERROR, "CTRL: SET_UNSOLICITED_FRAME_SUPPORT not supported as multiple VAPs are present but MultiBSS mode is disabled");
		return -1;
	}

	return 0;
}

static int hostapd_ctrl_iface_get_max_tx_power(struct hostapd_data *hapd, char *reply, size_t reply_size)
{
	int ret = 0;
	struct hostapd_channel_data *ch = NULL;
	if (!(ch = hostapd_hw_get_channel_data_from_ch(hapd->iface, hapd->iconf->channel)))
		return -1;
	ret = sprintf_s(reply, reply_size, "%u", ch->max_tx_power);
	if (ret <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
		return -1;
	}
	return ret;
}


static int hostapd_ctrl_iface_req_link_measurement(struct hostapd_data *hapd,
		char *cmd, char *buf, size_t buflen)
{
	u8 addr[ETH_ALEN];
	char *token, *context = NULL;
	int ret_dialog_token;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_LINK_MEASUREMENT - Bad destination address");
		return -1;
	}

	ret_dialog_token = hostapd_send_link_measurement_req(hapd, addr);

	if (ret_dialog_token < 0)
		return -1;
	return sprintf_s(buf, buflen, "dialog_token=%d\n", ret_dialog_token);
}

static int hostapd_ctrl_iface_link_measurement_report(struct hostapd_data *hapd,
		char *cmd)
{
	rsize_t len;
	u8 addr[ETH_ALEN];
	char *token, *context = NULL;
	char *pos, *cmd_end;
	char *buffer_end_address = cmd + MAX_BUFF_LEN - sizeof("LINK_MEASUREMENT_REPORT ") - 1;
	int val;

	struct rrm_link_measurement_report link_msmt_report;
	struct dmg_link_margin dmg_margin, *dmg_margin_p = NULL;
	struct dmg_link_adaptation_ack dmg_ack, *dmg_ack_p = NULL;

	os_memset(&link_msmt_report, 0, sizeof(link_msmt_report));
	os_memset(&dmg_margin, 0, sizeof(dmg_margin));
	os_memset(&dmg_ack, 0, sizeof(dmg_ack));

	len = strnlen_s(cmd, buffer_end_address - cmd);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - cmd - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: LINK_MEASUREMENT_REPORT - wrong len");
		return -1;
	}
	cmd_end = cmd + len;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_ERROR, "CTRL: LINK_MEASUREMENT_REPORT - Bad destination address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: LINK_MEASUREMENT_REPORT - dialog token is missing");
		return -1;
	}
	val = atoi(token);
	if(!IS_VALID_RANGE(val, WLAN_RRM_RADIO_MEASUREMENT_REPORT_TOKEN_VALUE_MIN,
					   WLAN_RRM_RADIO_MEASUREMENT_REPORT_TOKEN_VALUE_MAX)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: LINK_MEASUREMENT_REPORT - dialog token is out of range");
		return -1;
	}
	val = link_msmt_report.dialog_token;

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: LINK_MEASUREMENT_REPORT - rx_ant_id is missing");
		return -1;
	}
	val = atoi(token);
	if (!IS_VALID_RANGE(val, WLAN_RRM_RADIO_MEASUREMENT_REPORT_ANTENNA_ID_MIN,
						WLAN_RRM_RADIO_MEASUREMENT_REPORT_ANTENNA_ID_MAX)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: LINK_MEASUREMENT_REPORT - rx_ant_id is out of range");
		return -1;
	}
	link_msmt_report.rx_ant_id = (u8)val;

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: LINK_MEASUREMENT_REPORT - tx_ant_id is missing");
		return -1;
	}
	val = atoi(token);
	if (!IS_VALID_RANGE(val, WLAN_RRM_RADIO_MEASUREMENT_REPORT_ANTENNA_ID_MIN,
						WLAN_RRM_RADIO_MEASUREMENT_REPORT_ANTENNA_ID_MAX)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: LINK_MEASUREMENT_REPORT - tx_ant_id is out of range");
		return -1;
	}
	link_msmt_report.tx_ant_id = (u8)val;

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR, "CTRL: LINK_MEASUREMENT_REPORT - rcpi is missing");
		return -1;
	}
	val = atoi(token);
	if (!IS_VALID_U8_RANGE(val)) {
		wpa_printf(MSG_ERROR,
			  "CTRL: LINK_MEASUREMENT_REPORT - rcpi is out of range");
		return -1;
	}
	link_msmt_report.rcpi = (u8)val;
	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: LINK_MEASUREMENT_REPORT - rsni is missing");
		return -1;
	}
	val = atoi(token);
	if (!IS_VALID_U8_RANGE(val)) {
	       wpa_printf(MSG_ERROR,
			  "CTRL: LINK_MEASUREMENT_REPORT - rsni out of range");
	       return -1;
	}
	link_msmt_report.rsni = (u8)val;
	len = strnlen_s(token, buffer_end_address - token);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - token - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: LINK_MEASUREMENT_REPORT - wrong len");
		return -1;
	}
	token = token + len + 1;
	if (token >= cmd_end) {
		wpa_printf(MSG_ERROR,
			   "CTRL: LINK_MEASUREMENT_REPORT - tpc_report is missing");
		return -1;
	}

	pos = os_strstr(token, "tpc_report=");
	if (pos) {
		char *end, *comma;
		pos += 11;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos -1)) {
				wpa_printf(MSG_ERROR, "CTRL: LINK_MEASUREMENT_REPORT - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma = os_strchr(pos, ',');
		if (!comma || comma == pos || comma >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: LINK_MEASUREMENT_REPORT - tpc_report format is invalid");
			return -1;
		}
		val = atoi(pos);
		if (!IS_VALID_U8_RANGE(val)) {
			 wpa_printf(MSG_ERROR,
				       "CTRL: LINK_MEASUREMENT_REPORT - tpc_report.tx_power is invalid");
			return -1;
		}
		link_msmt_report.tpc.tx_power = (u8)val;
		comma++;
		val = atoi(comma);
		if (!IS_VALID_U8_RANGE(val)) {
			 wpa_printf(MSG_ERROR,
					 "CTRL: LINK_MEASUREMENT_REPORT - tpc_report.link_margin is invalid");
			return -1;
		}
		link_msmt_report.tpc.link_margin = (u8)val;
		link_msmt_report.tpc.eid = WLAN_EID_TPC_REPORT;
		link_msmt_report.tpc.len = 2;
	}

	/* optional tokens */
	pos = os_strstr(token, "dmg_link_margin=");
	if (pos) {
		char *end, *comma1, *comma2, *comma3, *comma4;
		pos += 16;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: LINK_MEASUREMENT_REPORT - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma1 = os_strchr(pos, ',');
		if (!comma1 || comma1 == pos || comma1 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: LINK_MEASUREMENT_REPORT - dmg_link_margin format is invalid");
			return -1;
		}
		comma1++;
		comma2 = os_strchr(comma1, ',');
		if (!comma2 || comma2 == comma1 || comma2 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: LINK_MEASUREMENT_REPORT - dmg_link_margin format is invalid");
			return -1;
		}
		comma2++;
		comma3 = os_strchr(comma2, ',');
		if (!comma3 || comma3 == comma2 || comma3 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: LINK_MEASUREMENT_REPORT - dmg_link_margin format is invalid");
			return -1;
		}
		comma3++;
		comma4 = os_strchr(comma3, ',');
		if (!comma4 || comma4 == comma3 || comma4 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: LINK_MEASUREMENT_REPORT - dmg_link_margin format is invalid");
			return -1;
		}
		comma4++;
		val = atoi(pos);
		if (!IS_VALID_RANGE(val, 0, 6)) {
			wpa_printf(MSG_ERROR,
					"CTRL: LINK_MEASUREMENT_REPORT - dmg_margin.activity is invalid");
			return -1;
		}
		dmg_margin.activity = (u8)val;
		val = atoi(comma1);
		if (!IS_VALID_U8_RANGE(val)) {
			wpa_printf(MSG_ERROR,
					"CTRL: LINK_MEASUREMENT_REPORT - dmg_margin.mcs is invalid");
			return -1;
		}
		dmg_margin.mcs = (u8)val;
		val = atoi(comma2);
		if (!IS_VALID_S8_RANGE(dmg_margin.link_margin)) {
			wpa_printf(MSG_ERROR,
				       "CTRL: LINK_MEASUREMENT_REPORT - dmg_margin.link_margin is invalid");
			return -1;
		}
		dmg_margin.link_margin = (s8)val;
		val = atoi(comma3);
		if (!IS_VALID_U8_RANGE(val)) {
			wpa_printf(MSG_ERROR,
				       "CTRL: LINK_MEASUREMENT_REPORT - dmg_margin.snr is invalid");
			return -1;
		}
		dmg_margin.snr = (u8)val;
		dmg_margin.ref_timestamp = atoi(comma4);
		dmg_margin.eid = WLAN_EID_DMG_LINK_MARGIN;
		dmg_margin.len = 8;
		dmg_margin_p = &dmg_margin;
	}

	pos = os_strstr(token, "dmg_link_adapt_ack=");
	if (pos) {
		char *end, *comma;
		pos += 19;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: LINK_MEASUREMENT_REPORT - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma = os_strchr(pos, ',');
		if (!comma || comma == pos || comma >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: LINK_MEASUREMENT_REPORT - dmg_link_adapt_ack format is invalid");
			return -1;
		}
		val = atoi(pos);
		if (!IS_VALID_RANGE(val, 0, 6)) {
			wpa_printf(MSG_ERROR,
					"CTRL: LINK_MEASUREMENT_REPORT - dmg_ack.activity is invalid");
			return -1;
		}
		dmg_ack.activity = (u8)val;
		comma++;
		dmg_ack.ref_timestamp = atoi(comma);
		dmg_ack.eid = WLAN_EID_DMG_LINK_ADAPTATION_ACK;
		dmg_ack.len = 5;
		dmg_ack_p = &dmg_ack;
	}

	wpa_printf(MSG_DEBUG, MACSTR
			" dialog_token=%d rx_ant_id=%d tx_ant_id=%d rcpi=%d rsni=%d tpc_report=%d,%d dmg_margin=%d,%d,%d,%d,%d dmg_ack=%d,%d",
			MAC2STR(addr), link_msmt_report.dialog_token,
			link_msmt_report.rx_ant_id, link_msmt_report.tx_ant_id,
			link_msmt_report.rcpi, link_msmt_report.rsni,
			link_msmt_report.tpc.tx_power, link_msmt_report.tpc.link_margin,
			dmg_margin.activity, dmg_margin.mcs, dmg_margin.link_margin,
			dmg_margin.snr, dmg_margin.ref_timestamp, dmg_ack.activity,
			dmg_ack.ref_timestamp);

	return hostapd_send_link_measurement_report(hapd, addr, link_msmt_report,
			dmg_margin_p, dmg_ack_p);
}

/* return -1 = error
 * return 1 = optional element exist
 * return 0 = optional element does not exist
 */
static int parse_trigger_reporting_counters(char *token, char *pos, char *str,
		struct rrm_trig_rep_counters *trig_rep_counters)
{
	s64 val;
	pos = os_strstr(token, str);
	if (pos) {
		int i, len;
		char *end, *comma1, *comma2, *counter_pos;
		pos += 22;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, MAX_BUFF_LEN);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, MAX_BUFF_LEN)) {
				wpa_printf(MSG_ERROR, "CTRL: REQ_STA_STATISTICS - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma1 = os_strchr(pos, ',');
		if (!comma1 || comma1 == pos || comma1 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_STA_STATISTICS - trig_rep_counters format is invalid");
			return -1;
		}
		comma1++;
		comma2 = os_strchr(comma1, ',');
		if (!comma2 || comma2 == comma1 || comma2 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_STA_STATISTICS - trig_rep_counters format is invalid");
			return -1;
		}
		comma2++;

		val = atoll(pos);
		if (!IS_VALID_U32_RANGE(val)) {
			   wpa_printf(MSG_ERROR,
						  "CTRL: REQ_STA_STATISTICS - measurement_count is invalid");
			   return -1;
		}
		(*trig_rep_counters).measurement_count = (u32)val;

		val = atoi(comma1);
		if (!IS_VALID_U16_RANGE(val)) {
			wpa_printf(MSG_ERROR,
				       "CTRL: REQ_STA_STATISTICS - trigger_timeout is invalid");
			return -1;
		}
		(*trig_rep_counters).trigger_timeout = (u16)val;
		val = atoi(comma2);
		if (!IS_VALID_U16_RANGE(val)) {
			wpa_printf(MSG_ERROR,
				       "CTRL: REQ_STA_STATISTICS - counters_trigger_condition is invalid");
			return -1;
		}
		(*trig_rep_counters).counters_trigger_condition = (u16)val;
		counter_pos = comma2;
		for (i = 0; i < 7; i++) {
			if (((*trig_rep_counters).counters_trigger_condition >> i) & 1) {
				counter_pos = os_strchr(counter_pos, ',');
				if (!counter_pos || counter_pos == pos || counter_pos >= end) {
					wpa_printf(MSG_ERROR,
							"CTRL: REQ_STA_STATISTICS - rep_sta_counters.counters format is invalid");
					return -1;
				}
				counter_pos++;
				(*trig_rep_counters).counters[i] = atoi(counter_pos);
				(*trig_rep_counters).num_of_counters++;
			}
		}

		return 1;
	}

	return 0;
}

static int hostapd_ctrl_iface_req_sta_statistics(struct hostapd_data *hapd,
		char *cmd, char *buf, size_t buflen)
{
	u8 addr[ETH_ALEN], peer_addr[ETH_ALEN];
	char *token, *context = NULL;
	int group_identity, random_interval, measurement_duration;
	int num_of_repetitions, measurement_request_mode, ret;
	char *pos, *cmd_end;
	int ret_dialog_token;
	rsize_t len;
	char *buffer_end_address = cmd + MAX_BUFF_LEN - sizeof("REQ_STA_STATISTICS ") - 1;

	struct rrm_trig_rep_counters trig_rep_sta_counters;
	struct rrm_trig_rep_counters *trig_rep_sta_counters_p = NULL;
	struct rrm_trig_rep_counters trig_rep_qos_sta_counters;
	struct rrm_trig_rep_counters *trig_rep_qos_sta_counters_p = NULL;
	struct rrm_trig_rep_counters trig_rep_rsna_counters;
	struct rrm_trig_rep_counters *trig_rep_rsna_counters_p = NULL;

	os_memset(&trig_rep_sta_counters, 0, sizeof(trig_rep_sta_counters));
	os_memset(&trig_rep_qos_sta_counters, 0, sizeof(trig_rep_qos_sta_counters));
	os_memset(&trig_rep_rsna_counters, 0, sizeof(trig_rep_rsna_counters));

	len = strnlen_s(cmd, buffer_end_address - cmd);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - cmd - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_STA_STATISTICS - wrong len");
		return -1;
	}
	cmd_end = cmd + len;
	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_STA_STATISTICS - Bad destination address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_STA_STATISTICS - number of repetitions is missing");
		return -1;
	}
	num_of_repetitions = atoi(token);
	if (!IS_VALID_U16_RANGE(num_of_repetitions)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_STA_STATISTICS - number of repetitions is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_STA_STATISTICS - measurement request mode is missing");
		return -1;
	}
	measurement_request_mode = atoi(token);
	if (!IS_VALID_U8_RANGE(measurement_request_mode)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_STA_STATISTICS - measurement request mode is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, peer_addr)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_STA_STATISTICS - Bad peer address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_STA_STATISTICS - random interval is missing");
		return -1;
	}
	random_interval = atoi(token);
	if (!IS_VALID_U16_RANGE(random_interval)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_STA_STATISTICS - random interval out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_STA_STATISTICS - measurement duration is missing");
		return -1;
	}
	measurement_duration = atoi(token);
	if (!IS_VALID_U16_RANGE(measurement_duration)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REQ_STA_STATISTICS - measurement duration out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_STA_STATISTICS - channel is missing");
		return -1;
	}
	group_identity = atoi(token);
	if (!IS_VALID_U8_RANGE(group_identity)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_STA_STATISTICS - group_identity value is out of range");
		return -1;
	}

	len = strnlen_s(token, buffer_end_address - token);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - token - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_STA_STATISTICS - wrong len");
		return -1;
	}
	/* optional tokens */
	token = token + len + 1;
	if (token >= cmd_end) {
		/* we've reached the end of command (no optional arguments) */
		goto exit;
	}

	ret = parse_trigger_reporting_counters(token, pos, "trig_rep_sta_counters=",
				&trig_rep_sta_counters);
	if (ret == -1)
		return ret;
	else if (ret == 1)
		trig_rep_sta_counters_p = &trig_rep_sta_counters;

	ret = parse_trigger_reporting_counters(token, pos, "trig_rep_qos_sta_counters=",
			&trig_rep_qos_sta_counters);
	if (ret == -1)
		return ret;
	else if (ret == 1)
		trig_rep_qos_sta_counters_p = &trig_rep_qos_sta_counters;

	ret = parse_trigger_reporting_counters(token, pos, "trig_rep_rsna_counters=",
			&trig_rep_rsna_counters);
	if (ret == -1)
		return ret;
	else if (ret == 1)
		trig_rep_rsna_counters_p = &trig_rep_rsna_counters;

exit:
	ret_dialog_token =  hostapd_send_sta_statistics_req(hapd, addr,
			num_of_repetitions, measurement_request_mode, peer_addr,
			random_interval, measurement_duration, group_identity,
			trig_rep_sta_counters_p, trig_rep_qos_sta_counters_p,
			trig_rep_rsna_counters_p);

	if (ret_dialog_token < 0)
		return -1;
	return sprintf_s(buf, buflen, "dialog_token=%d\n", ret_dialog_token);
}

static int hostapd_ctrl_iface_report_sta_statistics(struct hostapd_data *hapd,
		char *cmd)
{
	u8 addr[ETH_ALEN];
	rsize_t len;
	errno_t err;
	char *token, *context = NULL;
	char *pos, *cmd_end, *counter_pos;
	int group_identity, measurement_duration, i;
	int dialog_token, measurement_token, measurement_rep_mode;
	u32 statistics_group_data[RRM_STA_STATISTICS_GROUP_DATA_SIZE] = {};
	u32 statistics_group_data_len = 0;
	char *buffer_end_address = cmd + MAX_BUFF_LEN - sizeof("REPORT_STA_STATISTICS ") - 1;

	u8 rep_reason = 0, *rep_reason_p = NULL;
	s64 val;

	len = strnlen_s(cmd, buffer_end_address - cmd);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - cmd - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_STA_STATISTICS - wrong len");
		return -1;
	}
	cmd_end = cmd + len;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_STA_STATISTICS - Bad destination address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - dialog_token is missing");
		return -1;
	}
	dialog_token = atoi(token);
	if (!IS_VALID_RANGE(dialog_token, WLAN_RRM_RADIO_MEASUREMENT_REPORT_DIALOG_VALUE_MIN,
						WLAN_RRM_RADIO_MEASUREMENT_REPORT_DIALOG_VALUE_MAX)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - dialog_token is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - measurement_token is missing");
		return -1;
	}
	measurement_token = atoi(token);
	if (!IS_VALID_RANGE(measurement_token, WLAN_RRM_RADIO_MEASUREMENT_REPORT_TOKEN_VALUE_MIN,
						WLAN_RRM_RADIO_MEASUREMENT_REPORT_TOKEN_VALUE_MAX)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - measurement_token is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - measurement_rep_mode is missing");
		return -1;
	}
	measurement_rep_mode = atoi(token);
	if (!IS_VALID_U8_RANGE(measurement_rep_mode)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - measurement_rep_mode is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - measurement duration is missing");
		return -1;
	}
	measurement_duration = atoi(token);
	if (!IS_VALID_U16_RANGE(measurement_duration)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REPORT_STA_STATISTICS - measurement duration out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - group_identity is missing");
		return -1;
	}
	group_identity = atoi(token);
	if (!IS_VALID_U8_RANGE(group_identity)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - group_identity is out of range");
		return -1;
	}

	statistics_group_data_len =
			hostapd_get_statistics_group_data_len(group_identity);

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - statistics_group_data[0] is missing");
		return -1;
	}

	val = atoll(token);
	if (!IS_VALID_U32_RANGE(val)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_STA_STATISTICS - statistics_group_data[0] is out of range");
		return -1;
	}
	statistics_group_data[0] = (u32)val;

	counter_pos = token;
	for (i = 1; i < statistics_group_data_len; i++) {
		counter_pos = os_strchr(counter_pos, ',');
		if (!counter_pos) {
			wpa_printf(MSG_ERROR,
					"CTRL: REPORT_STA_STATISTICS - statistics_group_data[%d] is missing", i);
			return -1;
		}
		counter_pos++;
		if (((group_identity == 11) && ((i == 4) || (i == 8)))
		  ||((group_identity == 12) && ((i == 2) || (i == 6)))) {
			u64 tmp_counter = strtoull(counter_pos, NULL, 10);
			err = memcpy_s(&statistics_group_data[i], sizeof(statistics_group_data),
				&tmp_counter, sizeof(tmp_counter));
			if (EOK != err) {
				wpa_printf(MSG_ERROR, "%s: CTRL: REPORT_STA_STATISTICS - memcpy_s error %d", __func__, err);
				return -1;
			}
			i++;
		} else {
			val = atoll(counter_pos);
			if (!IS_VALID_U32_RANGE(val)) {
				wpa_printf(MSG_ERROR,
					   "CTRL: REPORT_STA_STATISTICS - statistics_group_data[%d] is out of range", i);
				return -1;
			}
			statistics_group_data[i] = (u32)val;
		}
	}

	len = strnlen_s(token, buffer_end_address - token);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - token - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_STA_STATISTICS - wrong len");
		return -1;
	}
	/* optional tokens */
	token = token + len + 1;
	if (token >= cmd_end) {
		/* we've reached the end of command (no optional arguments) */
		goto exit;
	}

	pos = os_strstr(token, "rep_reason=");
	if (pos) {
		int value;
		char *end;
		pos += 11;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REPORT_STA_STATISTICS - wrong len");
				return -1;
			}
			end = pos + len;
		}
		value = atoi(pos);
		if (!IS_VALID_U8_RANGE(value)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REPORT_STA_STATISTICS - rep_reason is invalid");
			return -1;

		}
		rep_reason = (u8)value;
		rep_reason_p = &rep_reason;
	}

exit:
	return hostapd_send_sta_statistics_report(hapd, addr, dialog_token,
			measurement_token, measurement_rep_mode, measurement_duration,
			group_identity, statistics_group_data, statistics_group_data_len,
			rep_reason_p);
}

static int hostapd_ctrl_iface_req_channel_load(struct hostapd_data *hapd,
		char *cmd, char *buf, size_t buflen)
{
	u8 addr[ETH_ALEN];
	rsize_t len;
	char *token, *context = NULL;
	char *pos, *cmd_end;
	int op_class, channel, random_interval, measurement_duration;
	int num_of_repetitions, measurement_request_mode;
	int ret_dialog_token;
	char *buffer_end_address = cmd + MAX_BUFF_LEN - sizeof("REQ_CHANNEL_LOAD ") - 1;

	u8 rep_cond, ch_load_ref_val;
	u8 *rep_cond_p = NULL, *ch_load_ref_val_p = NULL;

	u8 channel_width, channel_center_frequency0, channel_center_frequency1;
	u8 *channel_width_p = NULL;
	u8 *channel_center_frequency0_p = NULL, *channel_center_frequency1_p = NULL;

	len = strnlen_s(cmd, buffer_end_address - cmd);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - cmd - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_CHANNEL_LOAD - wrong len");
		return -1;
	}
	cmd_end = cmd + len;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_CHANNEL_LOAD - Bad destination address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_CHANNEL_LOAD - number of repetitions is missing");
		return -1;
	}
	num_of_repetitions = atoi(token);
	if (!IS_VALID_U16_RANGE(num_of_repetitions)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_CHANNEL_LOAD - number of repetitions is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_CHANNEL_LOAD - measurement request mode is missing");
		return -1;
	}
	measurement_request_mode = atoi(token);
	if (!IS_VALID_U8_RANGE(measurement_request_mode)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_CHANNEL_LOAD - measurement request mode is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_CHANNEL_LOAD - operating class is missing");
		return -1;
	}

	op_class = atoi(token);

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_CHANNEL_LOAD - channel is missing");
		return -1;
	}
	channel = atoi(token);
	/* Quote from IEEE80211-2016, 9.4.2.21.5 Channel Load request:
	  "If the Wide Bandwidth Channel Switch subelement is not included, the Operating Class field indicates the
	  operating class that identifies the channel set for which the measurement request applies. The Country,
	  Operating Class, and Channel Number fields together specify the channel frequency and spacing for which
	  the measurement request applies. Valid operating classes are listed in Annex E, excluding operating classes
	  that encompass a primary channel but do not identify the location of the primary channel.
	  Number field indicates the channel number for which the measurement request applies. The Channel number is
	  defined within an operating class as shown in Annex E.*/
	if (ieee80211_chan_to_freq(NULL, op_class, channel) < 0) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_CHANNEL_LOAD - channel/op_class combination is ivalid");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_CHANNEL_LOAD - random interval is missing");
		return -1;
	}
	random_interval = atoi(token);
	if (!IS_VALID_U16_RANGE(random_interval)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_CHANNEL_LOAD - random interval out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_CHANNEL_LOAD - measurement duration is missing");
		return -1;
	}
	measurement_duration = atoi(token);
	if (!IS_VALID_U16_RANGE(measurement_duration)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REQ_CHANNEL_LOAD - measurement duration out of range");
		return -1;
	}

	len = strnlen_s(token, buffer_end_address - token);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - token - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_CHANNEL_LOAD - wrong len");
		return -1;
	}
	/* optional tokens */
	token = token + len + 1;
	if (token >= cmd_end) {
		/* we've reached the end of command (no optional arguments) */
		goto exit;
	}

	pos = os_strstr(token, "ch_load_rep=");
	if (pos) {
		int cond, ref_val;
		char *end, *comma;
		pos += 12;
		end = os_strchr(pos, ' ');
		if (!end) {
			end = pos + os_strlen(pos);
		}
		comma = os_strchr(pos, ',');
		if (!comma || comma == pos || comma >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_CHANNEL_LOAD - channel load reporting format is invalid");
			return -1;
		}
		cond = atoi(pos);
		if (cond < 0 || cond > 2) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_CHANNEL_LOAD - channel load reporting condition is invalid");
			return -1;
		}
		comma++;
		ref_val = atoi(comma);
		if (!IS_VALID_U8_RANGE(ref_val)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_CHANNEL_LOAD - channel load reporting ref val is invalid");
			return -1;
		}
		rep_cond = (u8) cond;
		rep_cond_p = &rep_cond;
		ch_load_ref_val = (u8) ref_val;
		ch_load_ref_val_p = &ch_load_ref_val;
	}

	pos = os_strstr(token, "wide_band_ch_switch=");
	if (pos) {
		int ch_width, ch_center_freq0, ch_center_freq1;
		char *end, *comma1, *comma2;
		pos += 20;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REQ_CHANNEL_LOAD - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma1 = os_strchr(pos, ',');
		if (!comma1 || comma1 == pos || comma1 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_CHANNEL_LOAD - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma1++;
		comma2 = os_strchr(comma1, ',');
		if (!comma2 || comma2 == comma1 || comma2 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_CHANNEL_LOAD - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma2++;
		ch_width = atoi(pos);
		/* Quote from IEE80211-2016: "The subfields New Channel Width, New Channel Center Frequency Segment 0, and New Channel Center
		   Frequency Segment 1 have the same definition, respectively, as Channel Width, Channel Center Frequency
		   Segment 0, and Channel Center Frequency Segment 1 in the VHT Operation Information field, described in
		   Table 9-252."*/
		if (!IS_VALID_RANGE(ch_width, VHT_OPER_CHANWIDTH_20_40MHZ, VHT_OPER_CHANWIDTH_80P80MHZ)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_CHANNEL_LOAD - channel width is invalid");
			return -1;
		}

		ch_center_freq0 = atoi(comma1);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq0) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REQ_CHANNEL_LOAD - channel center frequency 0 is invalid");
		} else {
			channel_center_frequency0 = (u8) ch_center_freq0;
			channel_center_frequency0_p = &channel_center_frequency0;
		}

		ch_center_freq1 = atoi(comma2);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq1) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REQ_CHANNEL_LOAD - channel center frequency 1 is invalid");
		} else {
			channel_center_frequency1 = (u8) ch_center_freq1;
			channel_center_frequency1_p = &channel_center_frequency1;
		}

		channel_width = (u8) ch_width;
		channel_width_p = &channel_width;
	}

exit:
	ret_dialog_token = hostapd_send_channel_load_req(hapd, addr,
			num_of_repetitions, measurement_request_mode, op_class, channel,
			random_interval, measurement_duration, rep_cond_p,
			ch_load_ref_val_p, channel_width_p, channel_center_frequency0_p,
			channel_center_frequency1_p);

	if (ret_dialog_token < 0)
		return -1;
	return sprintf_s(buf, buflen, "dialog_token=%d\n", ret_dialog_token);
}

static int hostapd_ctrl_iface_report_channel_load(struct hostapd_data *hapd,
		char *cmd)
{
	u8 addr[ETH_ALEN];
	rsize_t len;
	char *token, *context = NULL;
	char *pos, *cmd_end;
	int op_class, channel, measurement_duration;
	int dialog_token, measurement_token, measurement_rep_mode, channel_load;
	u64 start_time;
	char *buffer_end_address = cmd + MAX_BUFF_LEN - sizeof("REPORT_CHANNEL_LOAD ") - 1;

	u8 channel_width, channel_center_frequency0, channel_center_frequency1;
	u8 *channel_width_p = NULL;
	u8 *channel_center_frequency0_p = NULL, *channel_center_frequency1_p = NULL;

	len = strnlen_s(cmd, buffer_end_address - cmd);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - cmd - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_CHANNEL_LOAD - wrong len");
		return -1;
	}
	cmd_end = cmd + len;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_CHANNEL_LOAD - Bad destination address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - dialog_token is missing");
		return -1;
	}
	dialog_token = atoi(token);
	if (!IS_VALID_RANGE(dialog_token, WLAN_RRM_RADIO_MEASUREMENT_REPORT_DIALOG_VALUE_MIN,
						WLAN_RRM_RADIO_MEASUREMENT_REPORT_DIALOG_VALUE_MAX)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - dialog_token is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - measurement_token is missing");
		return -1;
	}
	measurement_token = atoi(token);
	if (!IS_VALID_RANGE(measurement_token, WLAN_RRM_RADIO_MEASUREMENT_REPORT_TOKEN_VALUE_MIN,
						WLAN_RRM_RADIO_MEASUREMENT_REPORT_TOKEN_VALUE_MAX)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - measurement_token is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - measurement_rep_mode is missing");
		return -1;
	}
	measurement_rep_mode = atoi(token);
	if (!IS_VALID_U8_RANGE(measurement_rep_mode)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - measurement_rep_mode is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - operating class is missing");
		return -1;
	}

	op_class = atoi(token);

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_CHANNEL_LOAD - channel is missing");
		return -1;
	}

	channel = atoi(token);
	/* Quote from IEEE80211-2016, Chapter 9.4.2.22.5 Channel Load report:
	   "If the Wide Bandwidth Channel Switch subelement is not included, the Operating Class field indicates the
	   operating class that identifies the channel set for which the measurement report applies. The Country,
	   Operating Class, and Channel Number fields together specify the channel frequency and spacing for which
	   the measurement report applies. Valid operating classes are listed in Annex E, excluding operating classes
	   that encompass a primary channel but do not identify the location of the primary channel. The Channel
	   Number field indicates the channel number for which the measurement report applies. Channel number is
	   defined within an operating class as shown in Annex E." */
	if (ieee80211_chan_to_freq(NULL, op_class, channel) < 0) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - channel/op_class combination is invalid");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - start_time is missing");
		return -1;
	}

	start_time = strtoull (token, NULL, 10);

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - measurement duration is missing");
		return -1;
	}
	measurement_duration = atoi(token);
	if (!IS_VALID_U16_RANGE(measurement_duration)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REPORT_CHANNEL_LOAD - measurement duration out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_CHANNEL_LOAD - channel_load is missing");
		return -1;
	}
	channel_load = atoi(token);
	if (!IS_VALID_U8_RANGE(channel_load)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REPORT_CHANNEL_LOAD - channel_load out of range");
		return -1;
	}

	/* optional tokens */
	len = strnlen_s(token, buffer_end_address - token);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - token - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_CHANNEL_LOAD - wrong len");
		return -1;
	}
	token = token + len + 1;
	if (token >= cmd_end) {
		/* we've reached the end of command (no optional arguments) */
		goto exit;
	}

	pos = os_strstr(token, "wide_band_ch_switch=");
	if (pos) {
		int ch_width, ch_center_freq0, ch_center_freq1;
		char *end, *comma1, *comma2;
		pos += 20;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REPORT_CHANNEL_LOAD - wrong len");
			}
			end = pos + len;
		}
		comma1 = os_strchr(pos, ',');
		if (!comma1 || comma1 == pos || comma1 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REPORT_CHANNEL_LOAD - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma1++;
		comma2 = os_strchr(comma1, ',');
		if (!comma2 || comma2 == comma1 || comma2 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REPORT_CHANNEL_LOAD - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma2++;
		ch_width = atoi(pos);
		if (!IS_VALID_RANGE(ch_width, VHT_OPER_CHANWIDTH_20_40MHZ, VHT_OPER_CHANWIDTH_80P80MHZ)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REPORT_CHANNEL_LOAD - channel width is invalid");
			return -1;
		}

		ch_center_freq0 = atoi(comma1);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq0) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REPORT_CHANNEL_LOAD - channel center frequency 0 is invalid");
		} else {
			channel_center_frequency0 = (u8) ch_center_freq0;
			channel_center_frequency0_p = &channel_center_frequency0;
		}

		ch_center_freq1 = atoi(comma2);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq1) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REPORT_CHANNEL_LOAD - channel center frequency 1 is invalid");
		} else {
			channel_center_frequency1 = (u8) ch_center_freq1;
			channel_center_frequency1_p = &channel_center_frequency1;
		}

		channel_width = (u8) ch_width;
		channel_width_p = &channel_width;
	}

exit:
	return hostapd_send_channel_load_report(hapd, addr, dialog_token,
			measurement_token, measurement_rep_mode, op_class, channel,
			start_time, measurement_duration, channel_load, channel_width_p,
			channel_center_frequency0_p, channel_center_frequency1_p);
}

static int hostapd_ctrl_iface_req_noise_histogram(struct hostapd_data *hapd,
		char *cmd, char *buf, size_t buflen)
{
	u8 addr[ETH_ALEN];
	char *token, *context = NULL;
	char *pos, *cmd_end;
	int op_class, channel, random_interval, measurement_duration;
	int num_of_repetitions, measurement_request_mode;
	int ret_dialog_token, len;
	char *buffer_end_address = cmd + MAX_BUFF_LEN - sizeof("REQ_NOISE_HISTOGRAM ") - 1;

	u8 rep_cond, anpi_ref_val;
	u8 *rep_cond_p = NULL, *anpi_ref_val_p = NULL;

	u8 channel_width, channel_center_frequency0, channel_center_frequency1;
	u8 *channel_width_p = NULL;
	u8 *channel_center_frequency0_p = NULL, *channel_center_frequency1_p = NULL;

	len = strnlen_s(cmd, buffer_end_address - cmd);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - cmd - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_NOISE_HISTOGRAM - wrong len");
		return -1;
	}
	cmd_end = cmd + len;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_NOISE_HISTOGRAM - Bad destination address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_NOISE_HISTOGRAM - number of repetitions is missing");
		return -1;
	}
	num_of_repetitions = atoi(token);
	if (!IS_VALID_U16_RANGE(num_of_repetitions)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_NOISE_HISTOGRAM - number of repetitions is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_NOISE_HISTOGRAM - measurement request mode is missing");
		return -1;
	}
	measurement_request_mode = atoi(token);
	if (!IS_VALID_U8_RANGE(measurement_request_mode)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_NOISE_HISTOGRAM - measurement request mode is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_NOISE_HISTOGRAM - operating class is missing");
		return -1;
	}

	op_class = atoi(token);

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_NOISE_HISTOGRAM - channel is missing");
		return -1;
	}

	channel = atoi(token);
	/* Quote from IEEE80211-2016, Chapter 9.4.2.21.6 Noise Histogram request:
	   "If the Wide Bandwidth Channel Switch subelement is not included, the Operating Class field indicates the
	   operating class that identifies the channel set for which the measurement report applies. The Country,
	   Operating Class, and Channel Number fields together specify the channel frequency and spacing for which
	   the measurement report applies. Valid operating classes are listed in Annex E, excluding operating classes
	   that encompass a primary channel but do not identify the location of the primary channel. The Channel
	   Number field indicates the channel number for which the measurement report applies. Channel number is
	   defined within an operating class as shown in Annex E." */
	if (ieee80211_chan_to_freq(NULL, op_class, channel) < 0) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_NOISE_HISTOGRAM - channel/op_class combination is invalid");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_NOISE_HISTOGRAM - random interval is missing");
		return -1;
	}
	random_interval = atoi(token);
	if (!IS_VALID_U16_RANGE(random_interval)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_NOISE_HISTOGRAM - random interval out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REQ_NOISE_HISTOGRAM - measurement duration is missing");
		return -1;
	}
	measurement_duration = atoi(token);
	if (!IS_VALID_U16_RANGE(measurement_duration)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REQ_NOISE_HISTOGRAM - measurement duration out of range");
		return -1;
	}

	len = strnlen_s(token, buffer_end_address - token);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - token - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REQ_NOISE_HISTOGRAM - wrong len");
		return -1;
	}
	/* optional tokens */
	token = token + len + 1;
	if (token >= cmd_end) {
		/* we've reached the end of command (no optional arguments) */
		goto exit;
	}

	pos = os_strstr(token, "noise_histogram_rep=");
	if (pos) {
		int cond, ref_val;
		char *end, *comma;
		pos += 20;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REQ_NOISE_HISTOGRAM - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma = os_strchr(pos, ',');
		if (!comma || comma == pos || comma >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_NOISE_HISTOGRAM - noise histogram reporting format is invalid");
			return -1;
		}
		cond = atoi(pos);
		if (cond < 0 || cond > 2) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_NOISE_HISTOGRAM - noise histogram reporting condition is invalid");
			return -1;
		}
		comma++;
		ref_val = atoi(comma);
		if (!IS_VALID_U8_RANGE(ref_val)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_NOISE_HISTOGRAM - channel load reporting ref val is invalid");
			return -1;
		}
		rep_cond = (u8) cond;
		rep_cond_p = &rep_cond;
		anpi_ref_val = (u8) ref_val;
		anpi_ref_val_p = &anpi_ref_val;
	}

	pos = os_strstr(token, "wide_band_ch_switch=");
	if (pos) {
		int ch_width, ch_center_freq0, ch_center_freq1;
		char *end, *comma1, *comma2;
		pos += 20;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REQ_NOISE_HISTOGRAM - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma1 = os_strchr(pos, ',');
		if (!comma1 || comma1 == pos || comma1 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_NOISE_HISTOGRAM - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma1++;
		comma2 = os_strchr(comma1, ',');
		if (!comma2 || comma2 == comma1 || comma2 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_NOISE_HISTOGRAM - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma2++;
		ch_width = atoi(pos);
		if (!IS_VALID_RANGE(ch_width, VHT_OPER_CHANWIDTH_20_40MHZ, VHT_OPER_CHANWIDTH_80P80MHZ)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REQ_NOISE_HISTOGRAM - channel width is invalid");
			return -1;
		}

		ch_center_freq0 = atoi(comma1);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq0) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REQ_NOISE_HISTOGRAM - channel center frequency 0 is invalid");
		} else {
			channel_center_frequency0 = (u8) ch_center_freq0;
			channel_center_frequency0_p = &channel_center_frequency0;
		}

		ch_center_freq1 = atoi(comma2);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq1) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REQ_NOISE_HISTOGRAM - channel center frequency 1 is invalid");
		} else {
			channel_center_frequency1 = (u8) ch_center_freq1;
			channel_center_frequency1_p = &channel_center_frequency1;
		}

		channel_width = (u8) ch_width;
		channel_width_p = &channel_width;
	}

exit:
	ret_dialog_token = hostapd_send_noise_histogram_req(hapd, addr,
			num_of_repetitions, measurement_request_mode, op_class, channel,
			random_interval, measurement_duration, rep_cond_p, anpi_ref_val_p,
			channel_width_p, channel_center_frequency0_p,
			channel_center_frequency1_p);

	if (ret_dialog_token < 0)
		return -1;
	return sprintf_s(buf, buflen, "dialog_token=%d\n", ret_dialog_token);
}

static int hostapd_ctrl_iface_report_noise_histogram(struct hostapd_data *hapd,
		char *cmd)
{
	u8 addr[ETH_ALEN];
	rsize_t len;
	char *token, *context = NULL, *ipi_density_pos;
	char *pos, *cmd_end;
	int op_class, channel, measurement_duration, anpi, ant_id;
	int dialog_token, measurement_token, measurement_rep_mode, i, val;
	u64 start_time;
	u8 ipi_density[RRM_NOISE_HISTO_IPI_DENSITY_SIZE] = {};
	char *buffer_end_address = cmd + MAX_BUFF_LEN - sizeof("REPORT_NOISE_HISTOGRAM ") - 1;

	u8 channel_width, channel_center_frequency0, channel_center_frequency1;
	u8 *channel_width_p = NULL;
	u8 *channel_center_frequency0_p = NULL, *channel_center_frequency1_p = NULL;

	len = strnlen_s(cmd, buffer_end_address - cmd);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - cmd - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_NOISE_HISTOGRAM - wrong len");
		return -1;
	}
	cmd_end = cmd + len;

	token = str_token(cmd, " ", &context);
	if (!token || hwaddr_aton(token, addr)) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_NOISE_HISTOGRAM - Bad destination address");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - dialog_token is missing");
		return -1;
	}
	dialog_token = atoi(token);
	if (!IS_VALID_RANGE(dialog_token, WLAN_RRM_RADIO_MEASUREMENT_REPORT_DIALOG_VALUE_MIN,
						WLAN_RRM_RADIO_MEASUREMENT_REPORT_DIALOG_VALUE_MAX)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - dialog_token is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - measurement_token is missing");
		return -1;
	}
	measurement_token = atoi(token);
	if (!IS_VALID_RANGE(measurement_token, WLAN_RRM_RADIO_MEASUREMENT_REPORT_TOKEN_VALUE_MIN,
						WLAN_RRM_RADIO_MEASUREMENT_REPORT_TOKEN_VALUE_MAX)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - measurement_token is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - measurement_rep_mode is missing");
		return -1;
	}
	measurement_rep_mode = atoi(token);
	if (!IS_VALID_U8_RANGE(measurement_rep_mode)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - measurement_rep_mode is out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - operating class is missing");
		return -1;
	}

	op_class = atoi(token);

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_NOISE_HISTOGRAM - channel is missing");
		return -1;
	}
	channel = atoi(token);
	/* Quote from IEEE80211-2016, Chapter 9.4.2.22.6 Noise Histogram report:
	   "If the Wide Bandwidth Channel Switch subelement is not included, the Operating Class field indicates the
	   operating class that identifies the channel set for which the measurement report applies. The Country,
	   Operating Class, and Channel Number fields together specify the channel frequency and spacing for which
	   the measurement report applies. Valid operating classes are listed in Annex E, excluding operating classes
	   that encompass a primary channel but do not identify the location of the primary channel. The Channel
	   Number field indicates the channel number for which the measurement report applies. Channel number is
	   defined within an operating class as shown in Annex E." */
	if (ieee80211_chan_to_freq(NULL, op_class, channel) < 0) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - channel/op_class combination is invalid");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - start_time is missing");
		return -1;
	}
	start_time = strtoull (token, NULL, 10);
	if (start_time <= 0) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - start_time out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - measurement duration is missing");
		return -1;
	}
	measurement_duration = atoi(token);
	if (!IS_VALID_U16_RANGE(measurement_duration)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REPORT_NOISE_HISTOGRAM - measurement duration out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - ant_id is missing");
		return -1;
	}
	ant_id = atoi(token);
	if (!IS_VALID_U8_RANGE(ant_id)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REPORT_NOISE_HISTOGRAM - ant_id out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - anpi is missing");
		return -1;
	}
	anpi = atoi(token);
	if (!IS_VALID_U8_RANGE(anpi)) {
		wpa_printf(MSG_ERROR,
				"CTRL: REPORT_NOISE_HISTOGRAM - anpi out of range");
		return -1;
	}

	token = str_token(cmd, " ", &context);
	if (!token) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REPORT_NOISE_HISTOGRAM - ipi_density[0] is missing");
		return -1;
	}

	val = atoi(token);
	if (!IS_VALID_U8_RANGE(val)) {
		wpa_printf(MSG_ERROR,
			"CTRL: REPORT_NOISE_HISTOGRAM - ipi_density[0] is out of range");
		return -1;
	}
	ipi_density[0] = (u8)val;
	ipi_density_pos = token;
	for (i = 1; i < RRM_NOISE_HISTO_IPI_DENSITY_SIZE; i++) {
		ipi_density_pos = os_strchr(ipi_density_pos, ',');
		if (!ipi_density_pos) {
			wpa_printf(MSG_ERROR,
					"CTRL: REPORT_NOISE_HISTOGRAM - ipi_density[%d] is missing", i);
			return -1;
		}
		ipi_density_pos++;
		val = atoi(ipi_density_pos);
		if (!IS_VALID_U8_RANGE(val)) {
			wpa_printf(MSG_ERROR,
				"CTRL: REPORT_NOISE_HISTOGRAM - ipi_density[%d] is out of range", i);
			return -1;
		}
		ipi_density[i] = (u8)val;
	}

	len = strnlen_s(token, buffer_end_address - token);
	if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - token - 1)) {
		wpa_printf(MSG_ERROR, "CTRL: REPORT_NOISE_HISTOGRAM - wrong len");
		return -1;
	}
	/* optional tokens */
	token = token + len + 1;
	if (token >= cmd_end) {
		/* we've reached the end of command (no optional arguments) */
		goto exit;
	}

	pos = os_strstr(token, "wide_band_ch_switch=");
	if (pos) {
		int ch_width, ch_center_freq0, ch_center_freq1;
		char *end, *comma1, *comma2;
		pos += 20;
		end = os_strchr(pos, ' ');
		if (!end) {
			len = strnlen_s(pos, buffer_end_address - pos);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - pos - 1)) {
				wpa_printf(MSG_ERROR, "CTRL: REPORT_NOISE_HISTOGRAM - wrong len");
				return -1;
			}
			end = pos + len;
		}
		comma1 = os_strchr(pos, ',');
		if (!comma1 || comma1 == pos || comma1 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REPORT_NOISE_HISTOGRAM - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma1++;
		comma2 = os_strchr(comma1, ',');
		if (!comma2 || comma2 == comma1 || comma2 >= end) {
			wpa_printf(MSG_ERROR,
					"CTRL: REPORT_NOISE_HISTOGRAM - wide_band_ch_switch format is invalid");
			return -1;
		}
		comma2++;
		ch_width = atoi(pos);
		if (!IS_VALID_RANGE(ch_width, VHT_OPER_CHANWIDTH_20_40MHZ, VHT_OPER_CHANWIDTH_80P80MHZ)) {
			wpa_printf(MSG_ERROR,
					"CTRL: REPORT_NOISE_HISTOGRAM - channel width is invalid");
			return -1;
		}

		ch_center_freq0 = atoi(comma1);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq0) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REPORT_NOISE_HISTOGRAM - channel center frequency 0 is invalid");
		} else {
			channel_center_frequency0 = (u8) ch_center_freq0;
			channel_center_frequency0_p = &channel_center_frequency0;
		}

		ch_center_freq1 = atoi(comma2);
		if (ieee80211_chan_to_freq(NULL, op_class, ch_center_freq1) < 0) {
			wpa_printf(MSG_WARNING,
					"CTRL: REPORT_NOISE_HISTOGRAM - channel center frequency 1 is invalid");
		} else {
			channel_center_frequency1 = (u8) ch_center_freq1;
			channel_center_frequency1_p = &channel_center_frequency1;
		}

		channel_width = (u8) ch_width;
		channel_width_p = &channel_width;
	}

exit:
	return hostapd_send_noise_histogram_report(hapd, addr, dialog_token,
			measurement_token, measurement_rep_mode, op_class, channel,
			start_time, measurement_duration, ant_id, anpi, ipi_density,
			channel_width_p, channel_center_frequency0_p,
			channel_center_frequency1_p);
}

static int hostapd_ctrl_iface_set_neighbor(struct hostapd_data *hapd, char *buf)
{
	struct wpa_ssid_value ssid;
	u8 bssid[ETH_ALEN];
	struct wpabuf *nr, *lci = NULL, *civic = NULL;
	int stationary = 0;
	char *tmp;
	int ret;

	if (!(hapd->conf->radio_measurements[0] &
	      WLAN_RRM_CAPS_NEIGHBOR_REPORT)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: SET_NEIGHBOR: Neighbor report is not enabled");
		return -1;
	}

	if (hwaddr_aton(buf, bssid)) {
		wpa_printf(MSG_ERROR, "CTRL: SET_NEIGHBOR: Bad BSSID");
		return -1;
	}

	tmp = os_strstr(buf, "ssid=");
	if (!tmp || ssid_parse(tmp + 5, &ssid)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: SET_NEIGHBOR: Bad or missing SSID");
		return -1;
	}
	buf = os_strchr(tmp + 6, tmp[5] == '"' ? '"' : ' ');
	if (!buf)
		return -1;

	tmp = os_strstr(buf, "nr=");
	if (!tmp) {
		wpa_printf(MSG_ERROR,
			   "CTRL: SET_NEIGHBOR: Missing Neighbor Report element");
		return -1;
	}

	buf = os_strchr(tmp, ' ');
	if (buf)
		*buf++ = '\0';

	nr = wpabuf_parse_bin(tmp + 3);
	if (!nr) {
		wpa_printf(MSG_ERROR,
			   "CTRL: SET_NEIGHBOR: Bad Neighbor Report element");
		return -1;
	}

	if (!buf)
		goto set;

	tmp = os_strstr(buf, "lci=");
	if (tmp) {
		buf = os_strchr(tmp, ' ');
		if (buf)
			*buf++ = '\0';
		lci = wpabuf_parse_bin(tmp + 4);
		if (!lci) {
			wpa_printf(MSG_ERROR,
				   "CTRL: SET_NEIGHBOR: Bad LCI subelement");
			wpabuf_free(nr);
			return -1;
		}
	}

	if (!buf)
		goto set;

	tmp = os_strstr(buf, "civic=");
	if (tmp) {
		buf = os_strchr(tmp, ' ');
		if (buf)
			*buf++ = '\0';
		civic = wpabuf_parse_bin(tmp + 6);
		if (!civic) {
			wpa_printf(MSG_ERROR,
				   "CTRL: SET_NEIGHBOR: Bad civic subelement");
			wpabuf_free(nr);
			wpabuf_free(lci);
			return -1;
		}
	}

	if (!buf)
		goto set;

	if (os_strstr(buf, "stat"))
		stationary = 1;

set:
	ret = hostapd_neighbor_set(hapd, bssid, &ssid, nr, lci, civic,
				   stationary);

	wpabuf_free(nr);
	wpabuf_free(lci);
	wpabuf_free(civic);

	return ret;
}

static int hostapd_ctrl_iface_set_neighbor_per_vap(struct hostapd_data *hapd, char *buf)
{
	struct wpa_ssid_value ssid;
	u8 bssid[ETH_ALEN];
	struct wpabuf *nr, *lci = NULL, *civic = NULL;
	int stationary = 0;
	char *tmp;
	int ret;

	hapd = get_bss_index(buf, hapd->iface);
	if (!hapd) {
		wpa_printf(MSG_INFO,
			   "CTRL: SET_NEIGHBOR_PER_VAP - there is no iface with the given name");
		return -1;
	}

	if (!(hapd->conf->radio_measurements[0] &
	      WLAN_RRM_CAPS_NEIGHBOR_REPORT)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: SET_NEIGHBOR_PER_VAP: Neighbor report is not enabled");
		return -1;
	}

	buf = os_strchr(buf, ' ');
	if (buf)
		buf++;
	else {
		wpa_printf(MSG_INFO,
			   "CTRL: SET_NEIGHBOR_PER_VAP - neighbor BSSID is missing");
		return -1;
	}

	if (hwaddr_aton(buf, bssid)) {
		wpa_printf(MSG_ERROR, "CTRL: SET_NEIGHBOR_PER_VAP: Bad BSSID");
		return -1;
	}

	tmp = os_strstr(buf, "ssid=");
	if (!tmp || ssid_parse(tmp + 5, &ssid)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: SET_NEIGHBOR_PER_VAP: Bad or missing SSID");
		return -1;
	}
	buf = os_strchr(tmp + 6, tmp[5] == '"' ? '"' : ' ');
	if (!buf)
		return -1;

	tmp = os_strstr(buf, "nr=");
	if (!tmp) {
		wpa_printf(MSG_ERROR,
			   "CTRL: SET_NEIGHBOR_PER_VAP: Missing Neighbor Report element");
		return -1;
	}

	buf = os_strchr(tmp, ' ');
	if (buf)
		*buf++ = '\0';

	nr = wpabuf_parse_bin(tmp + 3);
	if (!nr) {
		wpa_printf(MSG_ERROR,
			   "CTRL: SET_NEIGHBOR_PER_VAP: Bad Neighbor Report element");
		return -1;
	}

	if (!buf)
		goto set;

	tmp = os_strstr(buf, "lci=");
	if (tmp) {
		buf = os_strchr(tmp, ' ');
		if (buf)
			*buf++ = '\0';
		lci = wpabuf_parse_bin(tmp + 4);
		if (!lci) {
			wpa_printf(MSG_ERROR,
				   "CTRL: SET_NEIGHBOR_PER_VAP: Bad LCI subelement");
			wpabuf_free(nr);
			return -1;
		}
	}

	if (!buf)
		goto set;

	tmp = os_strstr(buf, "civic=");
	if (tmp) {
		buf = os_strchr(tmp, ' ');
		if (buf)
			*buf++ = '\0';
		civic = wpabuf_parse_bin(tmp + 6);
		if (!civic) {
			wpa_printf(MSG_ERROR,
				   "CTRL: SET_NEIGHBOR_PER_VAP: Bad civic subelement");
			wpabuf_free(nr);
			wpabuf_free(lci);
			return -1;
		}
	}

	if (!buf)
		goto set;

	if (os_strstr(buf, "stat"))
		stationary = 1;

set:
	ret = hostapd_neighbor_set(hapd, bssid, &ssid, nr, lci, civic, stationary);

	wpabuf_free(nr);
	wpabuf_free(lci);
	wpabuf_free(civic);

	return ret;
}

static int hostapd_ctrl_iface_remove_neighbor(struct hostapd_data *hapd,
					      char *buf)
{
	struct wpa_ssid_value ssid;
	u8 bssid[ETH_ALEN];
	char *tmp;

	if (hwaddr_aton(buf, bssid)) {
		wpa_printf(MSG_ERROR, "CTRL: REMOVE_NEIGHBOR: Bad BSSID");
		return -1;
	}

	tmp = os_strstr(buf, "ssid=");
	if (!tmp || ssid_parse(tmp + 5, &ssid)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REMOVE_NEIGHBORr: Bad or missing SSID");
		return -1;
	}

	return hostapd_neighbor_remove(hapd, bssid, &ssid);
}

static int hostapd_ctrl_iface_remove_neighbor_per_vap(struct hostapd_data *hapd,
					      char *buf)
{
	struct wpa_ssid_value ssid;
	u8 bssid[ETH_ALEN];
	char *tmp;

	hapd = get_bss_index(buf, hapd->iface);
	if (!hapd) {
		wpa_printf(MSG_INFO,
				"CTRL: REMOVE_NEIGHBOR_PER_VAP - there is no iface with the given name");
		return -1;
	}

	buf = os_strchr(buf, ' ');
	if (buf)
		buf++;
	else {
		wpa_printf(MSG_INFO,
				"CTRL: REMOVE_NEIGHBOR_PER_VAP - neighbor BSSID is missing");
		return -1;
	}

	if (hwaddr_aton(buf, bssid)) {
		wpa_printf(MSG_ERROR, "CTRL: REMOVE_NEIGHBOR_PER_VAP: Bad BSSID");
		return -1;
	}

	tmp = os_strstr(buf, "ssid=");
	if (!tmp || ssid_parse(tmp + 5, &ssid)) {
		wpa_printf(MSG_ERROR,
			   "CTRL: REMOVE_NEIGHBOR_PER_VAP: Bad or missing SSID");
		return -1;
	}

	return hostapd_neighbor_remove(hapd, bssid, &ssid);
}

static int hostapd_ctrl_iface_clean_neighbordb_per_vap(struct hostapd_data *hapd,
					      char *buf)
{
	hapd = get_bss_index(buf, hapd->iface);
	if (!hapd) {
		wpa_printf(MSG_INFO,
				"CTRL: CLEAN_NEIGHBOR_DB_PER_VAP - there is no iface with the given name");
		return -1;
	}
	hostapd_free_neighbor_db(hapd);
	return 0;
}

static int hostapd_ctrl_driver_flags(struct hostapd_iface *iface, char *buf,
				     size_t buflen)
{
	int ret, i;
	char *pos, *end;

	ret = os_snprintf(buf, buflen, "%016llX:\n",
			  (long long unsigned) iface->drv_flags);
	if (os_snprintf_error(buflen, ret))
		return -1;

	pos = buf + ret;
	end = buf + buflen;

	for (i = 0; i < 64; i++) {
		if (iface->drv_flags & (1LLU << i)) {
			ret = os_snprintf(pos, end - pos, "%s\n",
					  driver_flag_to_string(1LLU << i));
			if (os_snprintf_error(end - pos, ret))
				return -1;
			pos += ret;
		}
	}

	return pos - buf;
}


static int hostapd_ctrl_iface_acl_del_mac(struct mac_acl_entry **acl, int *num,
					  const char *txtaddr)
{
	u8 addr[ETH_ALEN];
	struct vlan_description vlan_id;

	if (!(*num))
		return 0;

	if (hwaddr_aton(txtaddr, addr))
		return -1;

	if (hostapd_maclist_found(*acl, *num, addr, &vlan_id))
		hostapd_remove_acl_mac(acl, num, addr);

	return 0;
}


static void hostapd_ctrl_iface_acl_clear_list(struct mac_acl_entry **acl,
					      int *num)
{
	while (*num)
		hostapd_remove_acl_mac(acl, num, (*acl)[0].addr);
}


static int hostapd_ctrl_iface_acl_show_mac(struct mac_acl_entry *acl, int num,
					   char *buf, size_t buflen)
{
	int i = 0, len = 0, ret = 0;

	if (!acl)
		return 0;

	while (i < num) {
		ret = sprintf_s(buf + len, buflen - len,
				MACSTR " VLAN_ID=%d\n",
				MAC2STR(acl[i].addr),
				acl[i].vlan_id.untagged);
		if (ret <= 0)
			return len;
		i++;
		len += ret;
	}
	return len;
}


static int hostapd_ctrl_iface_acl_add_mac(struct mac_acl_entry **acl, int *num,
					  const char *cmd)
{
	u8 addr[ETH_ALEN];
	struct vlan_description vlan_id;
	int ret = 0, vlanid = 0;
	const char *pos;

	if (hwaddr_aton(cmd, addr))
		return -1;

	pos = os_strstr(cmd, "VLAN_ID=");
	if (pos) {
		vlanid = atoi(pos + 8);
		if (!IS_VALID_RANGE(vlanid, VLANID_MIN, VLANID_MAX)) {
			wpa_printf(MSG_DEBUG, "CTRL_IFACE: Invalid vlanid %d",vlanid);
			return -1;
		}
	}

	if (!hostapd_maclist_found(*acl, *num, addr, &vlan_id)) {
		ret = hostapd_add_acl_maclist(acl, num, vlanid, addr);
		if (ret != -1 && *acl)
			qsort(*acl, *num, sizeof(**acl), hostapd_acl_comp);
	}

	return ret < 0 ? -1 : 0;
}

/* Soft Block */
static int hostapd_ctrl_iface_soft_block_acl_add_mac(struct soft_block_acl_db *acl, const char *cmd)
{
	u8 addr[ETH_ALEN];
	int ret = 0;

	if (hwaddr_aton(cmd, addr))
		return -1;

	if (!hostapd_soft_block_maclist_found(acl, addr)) {
		ret = hostapd_soft_block_add_acl_maclist(acl, addr);
		if (ret != -1 && acl->indexes)
			qsort(acl->indexes, acl->num, sizeof(acl->indexes), hostapd_soft_block_acl_comp);
	}

	return ret < 0 ? -1 : 0;
}

static int hostapd_ctrl_iface_soft_block_acl_del_mac(struct soft_block_acl_db *acl, const char *txtaddr)
{
	u8 addr[ETH_ALEN];

	if (!(acl->num))
		return 0;

	if (hwaddr_aton(txtaddr, addr))
		return -1;

	if (hostapd_soft_block_maclist_found(acl, addr))
		hostapd_soft_block_remove_acl_mac(acl, addr);

	return 0;
}

static int hostapd_ctrl_iface_soft_block_acl_show_mac(struct soft_block_acl_db *acl, char *buf, size_t buflen)
{
	int i = 0, len = 0, ret = 0;

	if (NULL == acl)
		return 0;

	while (i < acl->num) {
		ret = sprintf_s(buf + len, buflen - len,
				MACSTR"\n", MAC2STR(acl->indexes[i]->addr));
		if (ret <= 0)
			return len;
		i++;
		len += ret;
	}
	return len;
}

static int hostapd_ctrl_iface_soft_block_acl_get_params(struct hostapd_data *hapd, char *buf, size_t buflen)
{

	int ret;

	ret = os_snprintf(buf, buflen,
			  "soft_block_acl_enable=%d\n"
			  "soft_block_acl_wait_time=%u\n"
			  "soft_block_acl_allow_time=%u\n"
			  "soft_block_acl_on_probe_req=%d\n"
			  "soft_block_acl_on_auth_req=%d\n",
			  hapd->conf->soft_block_acl_enable,
			  hapd->conf->soft_block_acl_wait_time,
			  hapd->conf->soft_block_acl_allow_time,
			  hapd->conf->soft_block_acl_on_probe_req,
			  hapd->conf->soft_block_acl_on_auth_req);

	return  (os_snprintf_error(buflen, ret)) ? 0 : ret;
}


int hostapd_ctrl_iface_deny_mac(struct hostapd_data *hapd, const char *cmd)
{
	const char *pos;
	struct hostapd_iface *iface = hapd->iface;
	u8 remove = 0;
	int ret, i, status;
	struct multi_ap_blacklist entry;

	os_memset(&entry, 0, sizeof(entry));
	if (hwaddr_aton(cmd, entry.addr))
		return -1;

	pos = os_strchr(cmd, ' ');
	if (pos) {
		pos++;
		if (os_strcmp(pos, "1") == 0)
			remove = 1;
	}

	pos = os_strstr(cmd, "reject_sta=");
	if (pos) {
		pos += sizeof("reject_sta=") - 1;
		status = atoi(pos);
		if (status < 0) {
			wpa_printf(MSG_ERROR, "deny_mac: invalid reject status code");
			return -1;
		}
		entry.status = status;
	}

	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *bss = iface->bss[i];

		ret = ieee802_11_multi_ap_set_deny_mac(bss, &entry, remove);
		if (ret < 0)
			return ret;
	}

	return 0;
}

int hostapd_ctrl_iface_sta_softblock(struct hostapd_data *hapd, const char *cmd)
{
	const char *pos;
	u8 remove = 0, snr;
	int ret, status;
	struct multi_ap_blacklist entry;

	hapd = get_bss_index(cmd, hapd->iface);
	if (!hapd) {
		wpa_printf(MSG_INFO,
			"sta_softblock - there is no iface with the given name");
		return -1;
	}

	if (hapd->conf->sDisableSoftblock) {
		wpa_printf(MSG_INFO,
			"sta_softblock - softblock is disabled");
		return -1;
	}

	cmd = os_strchr(cmd, ' ');
	if (cmd)
		cmd++;
	else {
		wpa_printf(MSG_INFO,
			"sta_softblock - BSS is missing");
		return -1;
	}

	os_memset(&entry, 0, sizeof(entry));
	if (hwaddr_aton(cmd, entry.addr))
		return -1;

	pos = os_strstr(cmd, "remove=");
	if (pos) {
		pos += sizeof("remove=") - 1;
		status = atoi(pos);
		if (status < 0) {
			wpa_printf(MSG_ERROR, "sta_softblock: invalid remove");
			return -1;
		}
		remove = status;
	} else {
		wpa_printf(MSG_ERROR, "sta_softblock: missing parameter(s)");
		return -1;
	}

	if (remove)
		goto end;

	pos = os_strstr(cmd, "reject_sta=");
	if (pos) {
		pos += sizeof("reject_sta=") - 1;
		status = atoi(pos);
		if (status < 0) {
			wpa_printf(MSG_ERROR, "sta_softblock: invalid reject status code");
			return -1;
		}
		entry.status = status;
	} else {
		wpa_printf(MSG_ERROR, "sta_softblock: missing parameter(s)");
		return -1;
	}

	pos = os_strstr(cmd, "snrProbeHWM=");
	if (pos) {
		pos += sizeof("snrProbeHWM=") - 1;
		snr = atoi(pos);
		entry.snrProbeHWM = snr;
	} else {
		wpa_printf(MSG_ERROR, "sta_softblock: missing parameter(s)");
		return -1;
	}

	pos = os_strstr(cmd, "snrProbeLWM=");
	if (pos) {
		pos += sizeof("snrProbeLWM=") - 1;
		int val = atoi(pos);
		if (!IS_VALID_U8_RANGE(val)) {
			wpa_printf(MSG_ERROR, "sta_softblock: invalid snrProbeHWM");
			return -1;
		}
		entry.snrProbeLWM = (u8)val;
	} else {
		wpa_printf(MSG_ERROR, "sta_softblock: missing parameter(s)");
		return -1;
	}

	pos = os_strstr(cmd, "snrAuthHWM=");
	if (pos) {
		pos += sizeof("snrAuthHWM=") - 1;
		int val = atoi(pos);
		if (!IS_VALID_U8_RANGE(val)) {
			wpa_printf(MSG_ERROR, "sta_softblock: invalid snrAuthHWM");
			return -1;
		}
		entry.snrAuthHWM = (u8)val;
	} else {
		wpa_printf(MSG_ERROR, "sta_softblock: missing parameter(s)");
		return -1;
	}

	pos = os_strstr(cmd, "snrAuthLWM=");
	if (pos) {
		pos += sizeof("snrAuthLWM=") - 1;
		int val = atoi(pos);
		if (!IS_VALID_U8_RANGE(val)) {
			wpa_printf(MSG_ERROR, "sta_softblock: invalid snrAuthLWM");
			return -1;
		}
		entry.snrAuthLWM = (u8)val;
	} else {
		wpa_printf(MSG_ERROR, "sta_softblock: missing parameter(s)");
		return -1;
	}

end:
	if (remove) {
		wpa_printf(MSG_DEBUG, "sta_softblock: bss=%s, " MACSTR ", remove=%u ",
			hapd->conf->iface, MAC2STR(entry.addr), remove);
	} else {
		wpa_printf(MSG_DEBUG, "sta_softblock: bss=%s, " MACSTR ", remove=%u, reject_sta=%u, snrProbeHWM=%u, snrProbeLWM=%u,"
			"snrAuthHWM=%u, snrAuthLWM=%u", hapd->conf->iface, MAC2STR(entry.addr), remove, entry.status,
			entry.snrProbeHWM, entry.snrProbeLWM, entry.snrAuthHWM, entry.snrAuthLWM);
	}

	ret = ieee802_11_multi_ap_set_softblock_thresholds(hapd, &entry, remove);

	if (ret < 0)
		return ret;

	return 0;
}

int hostapd_ctrl_iface_get_sta_softblock(struct hostapd_data *hapd, const char *cmd, char *reply, size_t reply_len)
{
	int ret;
	struct multi_ap_blacklist *entry;
	u8 sta_mac[ETH_ALEN];
	os_memset(&sta_mac, 0, sizeof(sta_mac));

	hapd = get_bss_index(cmd, hapd->iface);
	if (!hapd) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_STA_SOFTBLOCK - there is no iface with the given name\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	if (hapd->conf->sDisableSoftblock) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_STA_SOFTBLOCK - softblock is disabled\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	cmd = os_strchr(cmd, ' ');
	if (cmd)
		cmd++;
	else {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_STA_SOFTBLOCK - MAC address is missing\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	if (hwaddr_aton(cmd, sta_mac)) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_STA_SOFTBLOCK - Invalid MAC address passed\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	entry = ieee802_11_multi_ap_blacklist_find(hapd, sta_mac);

	if (!entry)
	{
		ret = sprintf_s(reply, reply_len, "CTRL: GET_STA_SOFTBLOCK - MAC address is not softblocked\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	ret = sprintf_s(reply, reply_len, "CTRL: GET_STA_SOFTBLOCK - status=%u, snrProbeHWM=%u, snrProbeLWM=%u,"
			" snrAuthHWM=%u, snrAuthLWM=%u\n", entry->status, entry->snrProbeHWM, entry->snrProbeLWM,
			entry->snrAuthHWM, entry->snrAuthLWM);
	if (ret <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
		return 0;
	}
	return ret;
}

int hostapd_ctrl_iface_sta_steer(struct hostapd_data *hapd, const char *cmd)
{
	u8 sta_addr[ETH_ALEN], bssid[ETH_ALEN];
	const char *pos, *btm_parameters = NULL;
	struct hostapd_iface *iface = hapd->iface;
	struct sta_info *sta = NULL;
	int ret, i, status, res;
	errno_t err;
	struct multi_ap_blacklist entry;

	os_memset(&entry, 0, sizeof(entry));
	if (hwaddr_aton(cmd, sta_addr) || is_zero_ether_addr(sta_addr)) {
		wpa_printf(MSG_DEBUG, "sta_steer: invalid station MAC Address.");
		return -1;
	}

	os_memset(bssid, 0, sizeof(bssid));
	pos = os_strchr(cmd, ' ');
	if (pos && (*(pos + 1) != '\0')) {
		pos++;
		if (hwaddr_aton(pos, bssid)) {
			btm_parameters = pos;
		} else {
			btm_parameters = os_strchr(pos + 1, ' ');
		}
	}

	pos = os_strstr(cmd, "reject_sta=");
	if (pos) {
		pos += sizeof("reject_sta=") - 1;
		status = atoi(pos);
		if (status < 0) {
			wpa_printf(MSG_ERROR, "sta_steer: invalid reject status code");
			return -1;
		}
		entry.status = status;
	}

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], sta_addr);
		if (sta) {
			hapd = hapd->iface->bss[i];
			break;
		}
	}

	if (!sta || !sta->btm_supported) {
		/*Station is not associated or does not support BSS Transition Management.
		 Use black list mechanism .*/

		wpa_printf(MSG_DEBUG,
				"Steer STA " MACSTR " , station is not associated to BSS "
				"or does not support BTM.", MAC2STR(sta_addr));

		err = memcpy_s(entry.addr, sizeof(entry.addr), sta_addr, sizeof(sta_addr));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			return -1;
		}
		ret = hostapd_drv_sta_steer(hapd, &entry, bssid);
		if (ret < 0)
			return ret;
		status = entry.status ? entry.status : WLAN_REASON_UNSPECIFIED;

		for (i = 0; i < iface->num_bss; i++) {
			struct hostapd_data *bss = iface->bss[i];

			/* remove STA from all BSS, except the specified one */
			if (os_memcmp(bss->own_addr, bssid, sizeof(bssid))) {
				hostapd_drv_sta_disassoc(bss, sta_addr, status);
				sta = ap_get_sta(bss, sta_addr);
				if (sta)
					ap_sta_disassociate(bss, sta, status);
				ieee802_11_multi_ap_blacklist_add(bss, &entry);
			} else {
				ieee802_11_multi_ap_blacklist_remove(bss, &entry);
			}
		}
	} else {
		/* Station supports 802.11v BTM, send BSS Transition Management
		 Request instead of black listing station. */
		char buf[2000], ret_buf[32];
		wpa_printf(MSG_DEBUG, "Steer STA " MACSTR ", station supports BTM.",
				MAC2STR(sta_addr));

		res = sprintf_s(buf, sizeof(buf), MACSTR " %s", MAC2STR(sta_addr),
			  btm_parameters ? btm_parameters : "");
		if (res <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, res);
			return -1;
		}

		ret = hostapd_ctrl_iface_bss_tm_req(hapd, buf, ret_buf, 32);
		if (ret > 0)
			ret = 0;
	}

	return ret;
}

struct hostapd_data *get_bss_index(const char *cmd, struct hostapd_iface *iface)
{
	char *pos_end;
	char bss_name[IFNAMSIZ + 1];
	int i;
	rsize_t len;
	errno_t err;

	pos_end = os_strchr(cmd, ' ');
	if (!pos_end) {
		len = strnlen_s(cmd, RSIZE_MAX_STR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, RSIZE_MAX_STR - 1)) {
			wpa_printf(MSG_ERROR, "get_bss_index - wrong len");
			return NULL;
		}
		pos_end = (char *) cmd + len;
	}
	err = memcpy_s(bss_name, sizeof(bss_name), cmd, pos_end - cmd);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return NULL;
	}
	bss_name[pos_end - cmd] = '\0';
	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *bss = iface->bss[i];

		if (!strncmp(bss->conf->iface, bss_name, IFNAMSIZ))
			return bss;
	}

	return NULL;
}

int hostapd_ctrl_iface_sta_allow(struct hostapd_data *hapd, const char *cmd)
{
	u8 sta_addr[ETH_ALEN];
	rsize_t len;
	errno_t err;
	int i, j, ret = 0, count = 0;
	const char *pos;
	struct hostapd_iface *iface = hapd->iface;
	u8 *stations = NULL;
	struct multi_ap_blacklist entry;
	const char *buffer_end_address = cmd + MAX_BUFF_LEN - 9;

	if (cmd[0] == ' ')
		cmd++;
	len = strnlen_s(cmd, buffer_end_address - cmd);
	if (IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, buffer_end_address - cmd - 1)) {
		pos = cmd;
		/* count the number of stations and verify the validity */
		while (pos && (*pos != '\0')) {
			if (hwaddr_aton(pos, sta_addr))
				return -1;
			count++;
			pos = os_strchr(pos, ' ');
			if (pos)
				pos++;
		}
		if (count == 0)
			return -1;
		stations = os_malloc(count * ETH_ALEN);
		if (!stations)
			return -ENOMEM;
		pos = cmd;
		for (i = 0; i < count; i++) {
			hwaddr_aton(pos, stations + i * ETH_ALEN);
			pos = os_strchr(pos, ' ');
			if (!pos)
				break;
			pos++;
		}
	}

	os_memset(&entry, 0, sizeof(entry));
	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *bss = iface->bss[i];

		ret = hostapd_drv_sta_allow(bss, stations, count);
		if (ret < 0)
			break;

		for (j = 0; j < count; j++) {
			err = memcpy_s(entry.addr, sizeof(entry.addr), &stations[ETH_ALEN * j], ETH_ALEN);
			if (EOK != err) {
				wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
				os_free(stations);
				return -1;
			}
			ieee802_11_multi_ap_blacklist_remove(bss, &entry);
		}

	}

	os_free(stations);
	return ret;
}

int hostapd_ctrl_iface_set_bss_load(struct hostapd_data *hapd, const char *cmd,
		char *buf, size_t buflen)
{
	int ret;
	int is_enable;

	hapd = get_bss_index(cmd, hapd->iface);
	if (hapd == NULL) {
		ret = sprintf_s(buf, buflen, "FAIL\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	cmd = os_strchr(cmd, ' ');
	if (cmd) {
		cmd++;
		is_enable = atoi(cmd);
		if (!IS_VALID_BOOL_RANGE(is_enable)) {
			wpa_printf(MSG_ERROR, "set_bss_load: invalid value");
			return -1;
		}
	} else {
		ret = sprintf_s(buf, buflen, "FAIL\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	ret = bss_load_enable(hapd, is_enable);
	if (ret) {
		ret = sprintf_s(buf, buflen, "FAIL\n");
		if (ret <= 0)
			return 0;
		return ret;
	}

	return 0;
}

int hostapd_ctrl_iface_get_blacklist(struct hostapd_iface *iface,
	char *buf, size_t buflen)
{
	int ret = 0, len = 0, i;

	for (i = 0; i < iface->num_bss; i++) {
		struct hostapd_data *bss = iface->bss[i];

		ret = sprintf_s(buf + len, buflen - len, "%s\n", bss->conf->iface);
		if (ret <= 0)
			break;
		len += ret;

		ret = ieee802_11_multi_ap_blacklist_print(bss, buf + len, buflen - len);
		if (os_snprintf_error(buflen - len, ret))
			break;
		len += ret;
	}

	return len;
}

int hostapd_ctrl_iface_get_sta_measurements(struct hostapd_data *hapd,
  const char *cmd, char *buf, size_t buflen)
{
/* the following defines have to be aligned with mtlk_wssa_net_modes_supported_e
 * from mtlk driver file mtlkwssa_drvinfo.h
 */
#define INTEL_11A_SUPPORTED  0x00000001
#define INTEL_11B_SUPPORTED  0x00000002
#define INTEL_11G_SUPPORTED  0x00000004
#define INTEL_11N_SUPPORTED  0x00000008
#define INTEL_11AC_SUPPORTED 0x00000010
#define INTEL_11AX_SUPPORTED 0x00000020

  u8 sta_addr[ETH_ALEN];
  int ret, len = 0;
  struct intel_vendor_sta_info sta_info;
  struct sta_info *sta;
//  struct mtlk_wssa_drv_peer_rate_info1* rate_info = NULL;

  hapd = get_bss_index(cmd, hapd->iface);
  if (hapd == NULL) {
    ret = sprintf_s(buf, buflen, "FAIL\n");
    if (ret <= 0) {
      wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
      return 0;
    }
    return ret;
  }

  cmd = os_strchr(cmd, ' ');
  if (cmd)
    cmd++;
  else {
    ret = sprintf_s(buf, buflen, "FAIL\n");
    if (ret <= 0) {
      wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
      return 0;
    }
    return ret;
  }

  if (hwaddr_aton(cmd, sta_addr)) {
    ret = sprintf_s(buf, buflen, "FAIL\n");
    if (ret <= 0) {
      wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
      return 0;
    }
    return ret;
  }

  sta = ap_get_sta(hapd, sta_addr);
  if (!sta || !(sta->flags & WLAN_STA_AUTHORIZED)) {
    ret = sprintf_s(buf, buflen, "FAIL\n");
    if (ret <= 0) {
      wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
      return 0;
    }
    return ret;
  }

  ret = hostapd_drv_get_sta_measurements(hapd, sta_addr, &sta_info);
  if (ret) {
    ret = sprintf_s(buf, buflen, "FAIL\n");
    if (ret < 0) {
      wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
      return 0;
    }
    return ret;
  }

  /* Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}. */
  ret = sprintf_s(buf, buflen, "MACAddress="MACSTR"\n", MAC2STR(sta_addr));
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "OperatingStandard=");
  if (ret <= 0)
    return len;
  len += ret;
  if (sta_info.NetModesSupported & INTEL_11A_SUPPORTED) {
    ret = sprintf_s(buf + len, buflen - len, "a ");
    if (ret <= 0)
      return len;
    len += ret;
  }
  if (sta_info.NetModesSupported & INTEL_11B_SUPPORTED) {
    ret = sprintf_s(buf + len, buflen - len, "b ");
    if (ret <= 0)
      return len;
    len += ret;
  }
  if (sta_info.NetModesSupported & INTEL_11G_SUPPORTED) {
    ret = sprintf_s(buf + len, buflen - len, "g ");
    if (ret <= 0)
      return len;
    len += ret;
  }
  if (sta_info.NetModesSupported & INTEL_11N_SUPPORTED) {
    ret = sprintf_s(buf + len, buflen - len, "n ");
    if (ret <= 0)
      return len;
    len += ret;
  }
  if (sta_info.NetModesSupported & INTEL_11AC_SUPPORTED) {
    ret = sprintf_s(buf + len, buflen - len, "ac ");
    if (ret <= 0)
      return len;
    len += ret;
  }
  if (sta_info.NetModesSupported & INTEL_11AX_SUPPORTED) {
    ret = sprintf_s(buf + len, buflen - len, "ax ");
    if (ret <= 0)
      return len;
    len += ret;
  }
  /* replace the last space with newline */
  buf[len - 1] = '\n';
  ret = sprintf_s(buf + len, buflen - len,
    "AuthenticationState=%s\n",
    sta ? (sta->flags & WLAN_STA_AUTH ? "1" : "0") : "UNKNOWN");
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "LastDataDownlinkRate=%d\n",
    sta_info.LastDataDownlinkRate);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "LastDataUplinkRate=%d\n",
    sta_info.LastDataUplinkRate);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "SignalStrength=%d\n",
    sta_info.SignalStrength);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "ShortTermRSSIAverage=%d %d %d %d\n",
  sta_info.ShortTermRSSIAverage[0],
  sta_info.ShortTermRSSIAverage[1],
  sta_info.ShortTermRSSIAverage[2],
  sta_info.ShortTermRSSIAverage[3]);
  if (ret <= 0)
	return len;
  len += ret;

  ret = sprintf_s(buf + len, buflen - len, "SNR=%d %d %d %d\n",
  sta_info.snr[0], sta_info.snr[1], sta_info.snr[2], sta_info.snr[3]);
  if (ret <= 0)
    return len;
  len += ret;

  ret = sprintf_s(buf + len, buflen - len, "Active=%s\n", sta ? "1" : "0");
  if (ret <= 0)
    return len;
  len += ret;
  /* Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.Stats. */
  ret = sprintf_s(buf + len, buflen - len, "BytesSent=%d\n",
    sta_info.BytesSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "BytesReceived=%d\n",
    sta_info.BytesReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "PacketsSent=%d\n",
    sta_info.PacketsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "PacketsReceived=%d\n",
    sta_info.PacketsReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "MaxRate=%d\n",
    sta_info.MaxRate);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "ErrorsSent=%d\n",
    sta_info.ErrorsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "RetransCount=%d\n",
    sta_info.RetransCount);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "RetryCount=%d\n",
    sta_info.RetryCount);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "MultipleRetryCount=UNKNOWN\n");
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "FailedRetransCount=%d\n",
    sta_info.FailedRetransCount);
  if (ret <= 0)
    return len;
  len += ret;

  /* peer Phy related information */
  if (sta_info.RateInfoFlag) { /* available */
    ret = sprintf_s(buf + len, buflen - len, "PhyMode=%d\n", sta_info.RatePhyMode);
    if (ret <= 0)
      return len;
    len += ret;
    ret = sprintf_s(buf + len, buflen - len, "Nss=%d\n", sta_info.RateNss);
    if (ret <= 0)
      return len;
    len += ret;
    ret = sprintf_s(buf + len, buflen - len, "CbwMHz=%d\n", sta_info.RateCbwMHz);
    if (ret <= 0)
      return len;
    len += ret;
  }

  ret = sprintf_s(buf + len, buflen - len, "TxStbcMode=%d\n", sta_info.TxStbcMode);
  if (ret <= 0)
    return len;
  len += ret;

  ret = sprintf_s(buf + len, buflen - len, "pairwise_cipher=%x\n", sta->wpa_sm ? wpa_auth_get_pairwise(sta->wpa_sm) : 0);
  if (ret <= 0)
    return len;
  len += ret;

  ret = sprintf_s(buf + len, buflen - len, "wpa_key_mgmt=%x\n", sta->wpa_sm ? wpa_auth_sta_key_mgmt(sta->wpa_sm) : 0);
  if (ret <= 0)
    return len;
  len += ret;

  /* ret = os_snprintf(buf + len, buflen - len, "PsState=%d\n",
    sta_info.stationInfo & STA_INFO_PS_STATE);
  if (ret >= buflen - len || ret < 0)
    return len;
  len += ret;*/

  return len;
}


int hostapd_ctrl_iface_get_vap_measurements(struct hostapd_data *hapd,
  const char *cmd, char *buf, size_t buflen)
{
  int ret, len = 0;
  struct intel_vendor_vap_info vap_info;

  hapd = get_bss_index(cmd, hapd->iface);
  if (hapd == NULL) {
    ret = sprintf_s(buf, buflen, "FAIL: bss index\n");
    if (ret <= 0) {
      wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
      return 0;
    }
    return ret;
  }

  ret = hostapd_drv_get_vap_measurements(hapd, &vap_info);
  if (ret) {
    ret = sprintf_s(buf, buflen, "FAIL: vap measurements\n");
    if (ret <= 0) {
      wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
      return 0;
    }
    return ret;
  }

  ret = sprintf_s(buf + len, buflen - len, "Name=%s\n",
    hapd->conf->iface);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "Enable=%d\n",
    hapd->started);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "BSSID="MACSTR"\n",
    MAC2STR(hapd->own_addr));
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "SSID=%s\n",
    wpa_ssid_txt(hapd->conf->ssid.ssid,
      hapd->conf->ssid.ssid_len));
  if (ret <= 0)
    return len;
  len += ret;
  /* Device.WiFi.SSID.{i}.Stats. */
  ret = sprintf_s(buf + len, buflen - len, "BytesSent=%llu\n",
		  vap_info.traffic_stats.BytesSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "BytesReceived=%llu\n",
		  vap_info.traffic_stats.BytesReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "PacketsSent=%llu\n",
		  vap_info.traffic_stats.PacketsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "PacketsReceived=%llu\n",
		  vap_info.traffic_stats.PacketsReceived);
  if (ret < 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "ErrorsSent=%d\n",
		  vap_info.error_stats.ErrorsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "RetransCount=%d\n",
		  vap_info.RetransCount);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "FailedRetransCount=%d\n",
		  vap_info.FailedRetransCount);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "RetryCount=%d\n",
		  vap_info.RetryCount);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "MultipleRetryCount=%d\n",
		  vap_info.MultipleRetryCount);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "ACKFailureCount=%d\n",
		  vap_info.ACKFailureCount);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "AggregatedPacketCount=%d\n",
		  vap_info.AggregatedPacketCount);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "ErrorsReceived=%d\n",
		  vap_info.error_stats.ErrorsReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "UnicastPacketsSent=%d\n",
		  vap_info.traffic_stats.UnicastPacketsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "UnicastPacketsReceived=%d\n",
		  vap_info.traffic_stats.UnicastPacketsReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "DiscardPacketsSent=%d\n",
		  vap_info.error_stats.DiscardPacketsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "DiscardPacketsReceived=%d\n",
		  vap_info.error_stats.DiscardPacketsReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "MulticastPacketsSent=%d\n",
		  vap_info.traffic_stats.MulticastPacketsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len,
    "MulticastPacketsReceived=%d\n",
	vap_info.traffic_stats.MulticastPacketsReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len,
    "BroadcastPacketsSent=%d\n",
	vap_info.traffic_stats.BroadcastPacketsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len,
    "BroadcastPacketsReceived=%d\n",
	vap_info.traffic_stats.BroadcastPacketsReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len,
    "UnknownProtoPacketsReceived=%d\n",
	vap_info.UnknownProtoPacketsReceived);
  if (ret <= 0)
    return len;
  len += ret;

  return len;
}

int hostapd_ctrl_iface_get_radio_state (enum hostapd_iface_state state)
{
	switch (state) {
	case HAPD_IFACE_ENABLED:
		return 1;
	case HAPD_IFACE_ACS_DONE:
		return 2;
	default:
		return 0;
	}
}

int hostapd_ctrl_iface_get_radio_info(struct hostapd_data *hapd,
  const char *cmd, char *buf, size_t buflen)
{
  int ret = 0, len = 0;
  struct intel_vendor_radio_info radio_info;
  enum hostapd_iface_state state = hapd->iface->state;

  ret = hostapd_drv_get_radio_info(hapd, &radio_info);
  if (ret) {
    ret = sprintf_s(buf, buflen, "FAIL\n");
    if (ret <= 0) {
      wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
      return 0;
    }
    return ret;
  }

  ret = sprintf_s(buf + len, buflen - len, "Name=%s\n",
    hapd->conf->iface);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "HostapdEnabled=%d\n",
      hostapd_ctrl_iface_get_radio_state(state));
  if (ret <= 0)
    return len;
  len += ret;
  /* Device.WiFi.Radio.{i}. */
  ret = sprintf_s(buf + len, buflen - len, "TxEnabled=%d\n",
      radio_info.Enable);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "Channel=%d\n",
      radio_info.Channel);
  if (ret <= 0)
    return len;
  len += ret;
  /* Device.WiFi.Radio.{i}.Stats. */
  ret = sprintf_s(buf + len, buflen - len, "BytesSent=%llu\n",
    radio_info.traffic_stats.BytesSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "BytesReceived=%llu\n",
    radio_info.traffic_stats.BytesReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "PacketsSent=%llu\n",
    radio_info.traffic_stats.PacketsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "PacketsReceived=%llu\n",
    radio_info.traffic_stats.PacketsReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "ErrorsSent=%d\n",
    radio_info.error_stats.ErrorsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "ErrorsReceived=%d\n",
    radio_info.error_stats.ErrorsReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "DiscardPacketsSent=%d\n",
    radio_info.error_stats.DiscardPacketsSent);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len,
    "DiscardPacketsReceived=%d\n",
    radio_info.error_stats.DiscardPacketsReceived);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "PLCPErrorCount=UNKNOWN\n");
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "FCSErrorCount=%d\n",
    radio_info.FCSErrorCount);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len,
    "InvalidMACCount=UNKNOWN\n");
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len,
    "PacketsOtherReceived=UNKNOWN\n");
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "Noise=%d\n",
    radio_info.Noise);
  if (ret < 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "BSS load=%d\n",
      radio_info.load);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "TxPower=%.2f\n",
      radio_info.tx_pwr_cfg / 100.);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "RxAntennas=%d\n",
    radio_info.num_rx_antennas);
  if (ret <= 0)
    return len;
  len += ret;
  ret = sprintf_s(buf + len, buflen - len, "TxAntennas=%d\n",
    radio_info.num_tx_antennas);
  if (ret <= 0)
    return len;
  len += ret;
  if (!radio_info.primary_center_freq) {
    ret = sprintf_s(buf + len, buflen - len, "Freq=UNKNOWN\n");
    if (ret <= 0)
      return len;
    len += ret;

    ret = sprintf_s(buf + len, buflen - len, "OperatingChannelBandwidt=UNKNOWN\n");
    if (ret <= 0)
      return len;
    len += ret;

    ret = sprintf_s(buf + len, buflen - len, "Cf1=UNKNOWN\n");
    if (ret <= 0)
      return len;
    len += ret;

    ret = sprintf_s(buf + len, buflen - len, "Dfs_chan=UNKNOWN\n");
    if (ret <= 0)
      return len;
    len += ret;
  } else {
    ret = sprintf_s(buf + len, buflen - len, "Freq=%d\n",
      radio_info.primary_center_freq);
    if (ret <= 0)
      return len;
    len += ret;

    ret = sprintf_s(buf + len, buflen - len, "OperatingChannelBandwidt=%d\n",
      radio_info.width);
    if (ret <= 0)
      return len;
    len += ret;

    ret = sprintf_s(buf + len, buflen - len, "Cf1=%d\n",
      radio_info.center_freq1);
    if (ret <= 0)
      return len;
    len += ret;

    ret = sprintf_s(buf + len, buflen - len, "Dfs_chan=%d\n",
      ieee80211_is_dfs(radio_info.primary_center_freq,hapd->iface->hw_features,hapd->iface->num_hw_features));
    if (ret <= 0)
      return len;
    len += ret;
  }

  return len;
}


static int
hostapd_is_chandef_valid (struct hostapd_iface *iface, unsigned int center_freq, unsigned int bandwidth)
{
  int valid = 0;
  unsigned int cur_freq;
  unsigned int low_freq, high_freq;

  if (!iface->current_mode)
    return 0;

  low_freq  = center_freq - bandwidth/2 + 10;
  high_freq = center_freq + bandwidth/2 - 10;

  for (cur_freq = low_freq; cur_freq <= high_freq; cur_freq += 20) {
    if (!hostapd_get_mode_channel(iface, cur_freq))
      return 0;

    valid = 1;
  }
  return valid;
}

int hostapd_ctrl_iface_unconnected_sta(struct hostapd_data *hapd, const char *cmd)
{
  struct intel_vendor_unconnected_sta_req_cfg req;
  char *pos;
  unsigned int bandwidth;
  struct sta_info *sta;

  os_memset(&req, 0, sizeof(req));

  if (hwaddr_aton(cmd, req.addr))
    return -1;

  pos = os_strstr(cmd, " ");
  if (pos) {
    pos++;
    req.freq = atoi(pos);
    if (!hostapd_is_chandef_valid(hapd->iface, req.freq, 20)){
      wpa_printf(MSG_ERROR, "unconnected_sta: invalid freq provided");
      return -1;
    }
  } else {
    wpa_printf(MSG_ERROR, "unconnected_sta: freq not specified");
    return -1;
  }

  pos = os_strstr(cmd, "center_freq1=");
  if (pos) {
    pos += sizeof("center_freq1=") - 1;
    req.center_freq1 = atoi(pos);
  } else {
    wpa_printf(MSG_ERROR, "unconnected_sta: center_freq1 not specified");
    return -1;
  }

  pos = os_strstr(cmd, "center_freq2=");
  if (pos) {
    pos += sizeof("center_freq2=") - 1;
    req.center_freq2 = atoi(pos);
  }

  pos = os_strstr(cmd, "bandwidth=");
  if (pos) {
    pos += sizeof("bandwidth=") - 1;
    bandwidth = atoi(pos);
  } else {
    wpa_printf(MSG_ERROR, "unconnected_sta: bandwidth not specified");
    return -1;
  }
  switch (bandwidth) {
  case 20:
    req.bandwidth = NL80211_CHAN_WIDTH_20;
    break;
  case 40:
    req.bandwidth = NL80211_CHAN_WIDTH_40;
    break;
  case 80:
    if (req.center_freq2)
      req.bandwidth = NL80211_CHAN_WIDTH_80P80;
    else
      req.bandwidth = NL80211_CHAN_WIDTH_80;
    break;
  case 160:
    req.bandwidth = NL80211_CHAN_WIDTH_160;
    break;
  default:
    wpa_printf(MSG_ERROR, "unconnected_sta: invalid bandwidth provided");
    return -1;
  }

  if (!hostapd_is_chandef_valid(hapd->iface, req.center_freq1, bandwidth)) {
    wpa_printf(MSG_ERROR, "unconnected_sta: invalid center_freq1 provided");
    return -1;
  }

  if (req.center_freq2 &&
      !hostapd_is_chandef_valid(hapd->iface, req.center_freq2, bandwidth)) {
    wpa_printf(MSG_ERROR, "unconnected_sta: invalid center_freq2 provided");
    return -1;
  }

  sta = ap_get_sta(hapd, req.addr);
  if (sta) {
    wpa_printf(MSG_ERROR, "unconnected_sta: disconnect STA "MACSTR" first",
      MAC2STR(req.addr));
    return -1;
  }

  return hostapd_drv_unconnected_sta(hapd, &req);
}


int hostapd_ctrl_iface_set_failsafe_chan(struct hostapd_iface *iface, const char *cmd)
{
  errno_t err;
  char *pos, *pos2;
  struct hostapd_channel_data *channel;
  struct hostapd_failsafe_freq_params old_freq_params;
  int tx_ant_mask = -1, rx_ant_mask = -1;

  err = memcpy_s(&old_freq_params, sizeof(old_freq_params), &iface->failsafe, sizeof(iface->failsafe));
  if (EOK != err) {
    wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
    return -1;
  }
  iface->failsafe.freq = atoi(cmd);

  /* ACS requested, the rest of parameters are irrelevant */
  if (iface->failsafe.freq == 0)
    return 0;

  channel = hostapd_hw_get_channel_data(iface, iface->failsafe.freq);
  if (!channel) {
    wpa_printf(MSG_ERROR, "set_failsafe_chan: invalid frequency %d provided, "
      "channel not found", iface->failsafe.freq);
    goto err;
  }
  if (channel->flag & HOSTAPD_CHAN_RADAR) {
    wpa_printf(MSG_ERROR, "set_failsafe_chan: channel %d is DFS channel. It "
      "cannot be failsafe", channel->chan);
    goto err;
  }

  pos = os_strstr(cmd, "center_freq1=");
  if (pos) {
    pos += sizeof("center_freq1=") - 1;
    iface->failsafe.center_freq1 = atoi(pos);
    if (iface->failsafe.center_freq1 == 0) {
      wpa_printf(MSG_ERROR, "set_failsafe_chan: invalid center_freq1 provided");
      goto err;
    }
  } else {
    wpa_printf(MSG_ERROR, "set_failsafe_chan: center_freq1 not specified");
    goto err;
  }

  pos = os_strstr(cmd, "center_freq2=");
  if (pos) {
    pos += sizeof("center_freq2=") - 1;
    iface->failsafe.center_freq2 = atoi(pos);
    if (iface->failsafe.center_freq2 == 0) {
      wpa_printf(MSG_ERROR, "set_failsafe_chan: invalid center_freq2 provided");
      goto err;
    }
  }

  pos = os_strstr(cmd, "bandwidth=");
  if (pos) {
    pos += sizeof("bandwidth=") - 1;
    iface->failsafe.bandwidth = atoi(pos);
    if (iface->failsafe.bandwidth == 0) {
      wpa_printf(MSG_ERROR, "set_failsafe_chan: invalid bandwidth provided");
      goto err;
    }
    if (iface->failsafe.bandwidth != 20 &&
      iface->failsafe.bandwidth != 40 &&
      iface->failsafe.bandwidth != 80 &&
      iface->failsafe.bandwidth != 160) {
      wpa_printf(MSG_ERROR, "set_failsafe_chan: invalid bandwidth %d provided",
        iface->failsafe.bandwidth);
      goto err;
    }
  } else {
    wpa_printf(MSG_ERROR, "set_failsafe_chan: bandwidth not specified");
    goto err;
  }

  pos = os_strstr(cmd, "tx_ant_mask=");
  if (pos) {
    pos += sizeof("tx_ant_mask=") - 1;
    tx_ant_mask = strtol(pos, &pos2, 10);
    if (pos == pos2 || tx_ant_mask < 0) {
      wpa_printf(MSG_ERROR, "set_failsafe_chan: invalid tx_ant_mask provided");
      return -1;
    }
  }
  pos = os_strstr(cmd, "rx_ant_mask=");
  if (pos) {
    pos += sizeof("rx_ant_mask=") - 1;
    rx_ant_mask = strtol(pos, &pos2, 10);
    if (pos == pos2 || rx_ant_mask < 0) {
      wpa_printf(MSG_ERROR, "set_failsafe_chan: invalid rx_ant_mask provided");
      return -1;
    }
  }
  if ((tx_ant_mask >= 0 && rx_ant_mask < 0) || (rx_ant_mask >= 0 && tx_ant_mask < 0)) {
    wpa_printf(MSG_ERROR, "set_failsafe_chan: changing only TX or RX "
      "antenna mask is not possible");
    goto err;
  }
  iface->failsafe.tx_ant = tx_ant_mask;
  iface->failsafe.rx_ant = rx_ant_mask;

  return 0;

err:
  err = memcpy_s(&iface->failsafe, sizeof(iface->failsafe), &old_freq_params, sizeof(old_freq_params));
  if (EOK != err) {
    wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
  }
  return -1;
}


int hostapd_ctrl_iface_get_failsafe_chan(struct hostapd_iface *iface,
  const char *cmd, char *buf, size_t buflen)
{
  int ret = 0, len = 0;

  if (iface->failsafe.freq == 0)
    return sprintf_s(buf + len, buflen - len, "UNSPECIFIED\n");

  ret = sprintf_s(buf + len, buflen - len, "%d center_freq1=%d "
    "center_freq2=%d bandwidth=%d",
    iface->failsafe.freq,
    iface->failsafe.center_freq1,
    iface->failsafe.center_freq2,
    iface->failsafe.bandwidth);

  if (ret <= 0) {
    wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
    return ret;
  }
  len += ret;

  if (iface->failsafe.tx_ant > 0) {
    ret = sprintf_s(buf + len, buflen - len, " tx_ant=%d rx_ant=%d",
      iface->failsafe.tx_ant,
      iface->failsafe.rx_ant);
    if (ret <= 0) {
      wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
      return ret;
    }
    len += ret;
  }

  ret = sprintf_s(buf + len, buflen - len, "\n");
  if (ret <= 0) {
    wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
    return ret;
  }
  len += ret;

  return len;
}

#ifdef CONFIG_ACS
int hostapd_ctrl_iface_acs_report(struct hostapd_iface *iface,
				  const char *cmd, char *buf, size_t buflen,
				  bool skip_excluded_ch)
{
	int ret = 0, len = 0, i;
	struct acs_candidate_table *candidates = iface->candidates;

	if (!candidates)
		return -1;

	for (i = 0; i < iface->num_candidates; i++) {
		/* skip excluded channels */
		if (skip_excluded_ch){
			if (candidates[i].exclude_reason != ACS_NO_EXCLUSION)
				continue;
		}

		if (candidates[i].rank < 0)
			continue;

		ret = sprintf_s(buf + len, buflen - len, "Ch=%d BW=%d",
				candidates[i].primary,
				candidates[i].width);
		if (ret <= 0)
			break;
		len += ret;

		ret = sprintf_s(buf + len, buflen - len, " DFS=%d",
			candidates[i].radar_affected);
		if (ret <= 0)
			break;
		len += ret;

		if (candidates[i].filled_mask & CHDATA_TX_POWER) {
			ret = sprintf_s(buf + len, buflen - len, " pow=%d",
					candidates[i].tx_power);
			if (ret <= 0)
				break;
			len += ret;
		}

		ret = sprintf_s(buf + len, buflen - len, " bss=%d",
				candidates[i].num_bss);
		if (ret <= 0)
			break;
		len += ret;
		ret = sprintf_s(buf + len, buflen - len, " pri=%d",
				candidates[i].num_bss_pri);
		if (ret <= 0)
			break;
		len += ret;

		if (candidates[i].filled_mask & CHDATA_LOAD) {
			ret = sprintf_s(buf + len, buflen - len, " load=%d",
					candidates[i].channel_load[PRI_20]);
			if (ret <= 0)
				break;
			len += ret;
		}

		ret = sprintf_s(buf + len, buflen - len, " rank=%d",
				candidates[i].rank);
		if (ret <= 0)
			break;
		len += ret;

		ret = sprintf_s(buf + len, buflen - len, "\n");
		if (ret <= 0)
			break;
		len += ret;
	}

	return len;
}
#endif

/* checks weather channel exists */
static int hostapd_ctrl_iface_restricted_chan_number_check_cb(struct hostapd_iface *iface,
  int channel)
{
  if (!hostapd_hw_get_channel_data_from_ch(iface, channel))
    return -1;
  return 0;
}

/* Sets channel as restricted. Ignores channels that are already disabled. */
static int hostapd_ctrl_iface_restricted_chan_number_cb(struct hostapd_iface *iface,
  int channel)
{
  struct hostapd_channel_data* ch_data;

  ch_data = hostapd_hw_get_channel_data_from_ch(iface, channel);
  if (!ch_data)
    return -1;
  if (!(ch_data->flag & HOSTAPD_CHAN_DISABLED)) {
    ch_data->flag |= HOSTAPD_CHAN_DISABLED;
    ch_data->flag |= HOSTAPD_CHAN_RESTRICTED;
  }

  return 0;
}

/* checks weather any channel in given range exists */
static int hostapd_ctrl_iface_restricted_chan_range_check_cb(struct hostapd_iface *iface,
  int channel_from, int channel_to)
{
  bool exists = false;
  int i;

  for (i = channel_from; i <= channel_to; i++) {
    if (hostapd_hw_get_channel_data_from_ch(iface, i)) {
      exists = true;
      break;
    }
  }
  if (exists)
    return 0;
  else
    return -1;
}

/* Sets channels in given range as restricted. Ignores channels that are already
 * disabled. */
static int hostapd_ctrl_iface_restricted_chan_range_cb(struct hostapd_iface *iface,
  int channel_from, int channel_to)
{
  bool exists = false;
  int i;
  struct hostapd_channel_data* ch_data;

  for (i = channel_from; i <= channel_to; i++) {
    ch_data = hostapd_hw_get_channel_data_from_ch(iface, i);
    if (!ch_data)
      continue;
    exists = true;
    if (!(ch_data->flag & HOSTAPD_CHAN_DISABLED)) {
      ch_data->flag |= HOSTAPD_CHAN_DISABLED;
      ch_data->flag |= HOSTAPD_CHAN_RESTRICTED;
    }
  }
  if (exists)
    return 0;
  else
    return -1;
}


int hostapd_ctrl_iface_set_restricted_chan(struct hostapd_iface *iface, const char *cmd)
{
  char *end;
  struct hostapd_hw_modes *mode;
  int i;
  struct hostapd_channel_data *ch;

  if (!iface->current_mode)
    return -1;
  mode = iface->current_mode;

  /* check list for validity */
  if (hostapd_get_list(cmd, &end, iface,
    hostapd_ctrl_iface_restricted_chan_number_check_cb,
    hostapd_ctrl_iface_restricted_chan_range_check_cb))
    return -1;

  /* clear restricted flag on all channels */
  for (i = 0; i < mode->num_channels; i++) {
    ch = &iface->current_mode->channels[i];
    if ((ch->flag & HOSTAPD_CHAN_DISABLED) &&
      (ch->flag & HOSTAPD_CHAN_RESTRICTED)) {
      ch->flag &= ~HOSTAPD_CHAN_DISABLED;
      ch->flag &= ~HOSTAPD_CHAN_RESTRICTED;
    }
  }

  /* set restricted flag on specified channels */
  return hostapd_get_list(cmd, &end, iface,
    hostapd_ctrl_iface_restricted_chan_number_cb,
    hostapd_ctrl_iface_restricted_chan_range_cb);
}


int hostapd_ctrl_iface_get_restricted_chan(struct hostapd_iface *iface,
  const char *cmd, char *buf, size_t buflen)
{
  int ret = 0, len = 0, i;
  struct hostapd_hw_modes *mode;
  struct hostapd_channel_data *ch;

  if (!iface->current_mode)
    return -1;
  mode = iface->current_mode;

  for (i = 0; i < mode->num_channels; i++) {
    ch = &iface->current_mode->channels[i];
    if ((ch->flag & HOSTAPD_CHAN_DISABLED) &&
      (ch->flag & HOSTAPD_CHAN_RESTRICTED)) {
        ret = sprintf_s(buf + len, buflen - len, "%d ", ch->chan);
        if (ret <= 0)
          break;
        len += ret;
    }
  }
  if (len > 0)
    buf[len - 1] = '\n';

  return len;
}


int hostapd_ctrl_iface_get_hw_features(struct hostapd_iface *iface,
  const char *cmd, char *buf, size_t buflen)
{
  int ret = 0, len = 0;
  struct hostapd_data *hapd = iface->bss[0];
  int i, j;

  for (i = 0; i < iface->num_hw_features; i++) {
    struct hostapd_hw_modes *feature = &iface->hw_features[i];
    int dfs_enabled = hapd->iconf->ieee80211h &&
      (iface->drv_flags & WPA_DRIVER_FLAGS_RADAR);

    for (j = 0; j < feature->num_channels; j++) {
      int dfs = 0;

      if ((feature->channels[j].flag &
           HOSTAPD_CHAN_RADAR) && dfs_enabled) {
        dfs = 1;
      }

      if (feature->channels[j].flag & HOSTAPD_CHAN_DISABLED)
        continue;

      ret = sprintf_s(buf + len, buflen - len, "Allowed channel: mode=%d "
           "chan=%d freq=%d MHz max_tx_power=%d dBm%s\n",
           feature->mode,
           feature->channels[j].chan,
           feature->channels[j].freq,
           feature->channels[j].max_tx_power,
           dfs ? dfs_info(&feature->channels[j]) : "");
      if (ret <= 0)
        return len;
      len += ret;
    }
  }

  return len;
}

static int hostapd_ctrl_iface_get_dfs_stats (struct hostapd_iface *iface, const char *cmd,
					     char *buf, size_t buflen)
{
	struct os_tm tm;
	int ret = 0, len = 0;
	hostapd_dfs_history_t *entry;
	const char *month_str = "Jan\0Feb\0Mar\0Apr\0May\0Jun\0"
				"Jul\0Aug\0Sep\0Oct\0Nov\0Dec";

	ret = sprintf_s(buf + len, buflen - len, "Sub Band DFS configured counter value %d\n",
		        iface->conf->sub_band_dfs);
	if (ret <= 0)
		return len;
	len += ret;

	ret = sprintf_s(buf + len, buflen - len, "Sub Band DFS current counter value %d\n",
		        iface->sb_dfs_cntr ? iface->sb_dfs_cntr - 1 : 0);
	if (ret <= 0)
		return len;
	len += ret;

	/* DFS History statistics */
	ret = sprintf_s(buf + len, buflen - len, "Radar detection history, last %d radar detections:\n", MAX_DFS_HIST_ENTRIES);
	if (ret <= 0)
		return len;
	len += ret;

	dl_list_for_each(entry, &iface->dfs_history, hostapd_dfs_history_t, list) {
		if (os_gmtime(entry->detect_time, &tm))
			os_memset(&tm, 0, sizeof(tm));

		ret = sprintf_s(buf + len, buflen - len, "%s %d %02d:%02d:%02d UTC: center chan %d, prim %d, bw %d, rbm 0x%02x, %s\n",
				&month_str[4 * (tm.month - 1)], tm.day, tm.hour, tm.min, tm.sec, entry->seg0_idx, entry->primary, entry->bandwidth, entry->rbm,
				entry->type == DFS_CHAN_SWITCH ? "new chan switch" : (entry->type == DFS_NO_SWITCH ? "no channel switch" : "sub band switch"));

		if (ret <= 0)
			return len;
		len += ret;
	}

	return len;
}

static int hostapd_ctrl_iface_get_mesh_mode (struct hostapd_data *hapd, const char *cmd,
	char *buf, size_t buflen)
{
	int ret, len = 0;

	hapd = get_bss_index(cmd, hapd->iface);
	if (hapd == NULL) {
		ret = sprintf_s(buf, buflen, "CTRL: GET_MESH_MODE - there is no iface with the given name\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	ret = sprintf_s(buf + len, buflen - len, "mesh_mode=%s (%d)\n",
			hostapd_config_get_mesh_mode_str(hapd->conf->mesh_mode), hapd->conf->mesh_mode);
	if (ret <= 0)
		return len;
	len += ret;

	return len;
}

static int hostapd_ctrl_iface_set_mesh_mode (struct hostapd_data *hapd, const char *cmd)
{
	int ret;
	unsigned mesh_mode;

	hapd = get_bss_index(cmd, hapd->iface);
	if (NULL == hapd) {
		wpa_printf(MSG_INFO, "CTRL: MESH_MODE - there is no iface with the given name");
		return -1;
	}

	cmd = os_strchr(cmd, ' ');
	if (!cmd) {
		wpa_printf(MSG_INFO, "CTRL: MESH_MODE - mesh_mode missing");
		return -1;
	}
	cmd++;

	mesh_mode = hostapd_config_parse_mesh_mode(cmd);
	if (mesh_mode >= MESH_MODE_LAST) {
		wpa_printf(MSG_INFO, "CTRL: MESH_MODE - incorrect mesh_mode");
		return -EINVAL;
	}

	ret = hostapd_drv_set_mesh_mode(hapd, mesh_mode);
	if (ret) {
		wpa_printf(MSG_ERROR, "CTRL: MESH_MODE - Failed to set mesh_mode");
	} else {
		hapd->conf->mesh_mode = mesh_mode;
		hapd->conf->mesh_mode_set = 1;
		/* if needed activate the extra backhaul interfaces and add them to a bridge */
		if ((hapd->conf->mesh_mode == MESH_MODE_BACKHAUL_AP ||
		     hapd->conf->mesh_mode == MESH_MODE_EXT_HYBRID)
		    && hostapd_activate_backhaul_ifaces(hapd)) {
			wpa_printf(MSG_ERROR, "Failed to activate extra backhaul ap interfaces");
			return -1;
		}
	}
	return ret;
}

enum elem_type_e {
	VENDOR_ELEM_TYPE,
	AUTHRESP_ELEM_TYPE,
	ASSOCRESP_ELEM_TYPE,
	LAST_ELEM_TYPE
};

struct elem_type_item_t {
	const char *param_name_prefix;
	const char *cmd_prefix;
};

static const struct elem_type_item_t elem_types[LAST_ELEM_TYPE] = {
	{ "vendor", "VENDOR" },
	{ "authresp", "AUTHRESP" },
	{ "assocresp", "ASSOCRESP" }
};

static int hostapd_ctrl_iface_get_elements (struct hostapd_data *hapd, const char *cmd,
	char *buf, size_t buflen, enum elem_type_e elem_type)
{
	int ret, len = 0;
	struct wpabuf *elements;

	if (elem_type >= LAST_ELEM_TYPE)
		return 0;

	hapd = get_bss_index(cmd, hapd->iface);
	if (hapd == NULL) {
		ret = sprintf_s(buf, buflen, "CTRL: GET_%s_ELEMENTS - there is no iface with the given name\n",
			elem_types[elem_type].cmd_prefix);
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	switch (elem_type) {
		case VENDOR_ELEM_TYPE:    elements = hapd->conf->vendor_elements;    break;
		case AUTHRESP_ELEM_TYPE:  elements = hapd->conf->authresp_elements;  break;
		case ASSOCRESP_ELEM_TYPE: elements = hapd->conf->assocresp_elements; break;
		default: return 0;
	}

	ret = sprintf_s(buf + len, buflen - len, "%s_elements=", elem_types[elem_type].param_name_prefix);
	if (ret <= 0)
		return len;
	len += ret;

	if (elements) {
		ret = wpa_snprintf_hex_uppercase(buf + len, buflen - len,
			wpabuf_head_u8(elements), wpabuf_len(elements));
		if (os_snprintf_error(buflen - len, ret))
			return len;
		len += ret;
	}

	ret = sprintf_s(buf + len, buflen - len, "\n");
	if (ret <= 0)
		return len;
	len += ret;

	return len;
}

static int hostapd_ctrl_iface_set_elements (struct hostapd_data *hapd, const char *cmd, enum elem_type_e elem_type)
{
	struct wpabuf *new_elements;
	struct wpabuf **p_elements;

	if (elem_type >= LAST_ELEM_TYPE)
		return -1;

	hapd = get_bss_index(cmd, hapd->iface);
	if (NULL == hapd) {
		wpa_printf(MSG_INFO, "CTRL: %s_ELEMENTS - there is no iface with the given name",
			elem_types[elem_type].cmd_prefix);
		return -1;
	}

	switch (elem_type) {
		case VENDOR_ELEM_TYPE:    p_elements = &hapd->conf->vendor_elements;    break;
		case AUTHRESP_ELEM_TYPE:  p_elements = &hapd->conf->authresp_elements;  break;
		case ASSOCRESP_ELEM_TYPE: p_elements = &hapd->conf->assocresp_elements; break;
		default: return -1;
	}

	cmd = os_strchr(cmd, ' ');
	if (!cmd) {
		/* No elements specified */
		wpabuf_free(*p_elements);
		*p_elements = NULL;
		/* remove Beacon/Probe resp IE */
		if (elem_type == VENDOR_ELEM_TYPE)
			if (ieee802_11_update_beacon(hapd))
				return -1;
		return 0;
	}
	cmd++;

	new_elements = wpabuf_parse_bin(cmd);
	if (!new_elements) {
		wpa_printf(MSG_ERROR, "Invalid hex line '%s'", cmd);
		return -1;
	}
	wpabuf_free(*p_elements);
	*p_elements = new_elements;
	
	/* update Beacon/Probe resp IE only */
	if (elem_type == VENDOR_ELEM_TYPE)
		if (ieee802_11_update_beacon(hapd))
			return -1;

	return 0;
}

static int hostapd_ctrl_iface_get_last_assoc_req (struct hostapd_data *hapd, const char *cmd, char *reply, size_t reply_len)
{
	int i, ret, len = 0;
	struct sta_info *sta = NULL;
	u8 sta_mac[ETH_ALEN];

	if (hwaddr_aton(cmd, sta_mac)) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_LAST_ASSOC_REQ - Incorrect MAC address passed\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], sta_mac);

		if (sta)
			 break;
	}

	if (!sta) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_LAST_ASSOC_REQ - STA doesn't exist\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	if (!sta->last_assoc_req || !sta->last_assoc_req_len) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_LAST_ASSOC_REQ - STA doesn't have last assoc req\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	ret = sprintf_s(reply + len, reply_len - len, "last_assoc_req=");
	if (ret <= 0)
		return len;
	len += ret;

	ret = wpa_snprintf_hex_uppercase(reply + len, reply_len - len, (const u8 *)sta->last_assoc_req, sta->last_assoc_req_len);
	if (ret <= 0)
		return len;
	len += ret;

	ret = sprintf_s(reply + len, reply_len - len, "\n");
	if (ret <= 0)
		return len;
	len += ret;

	return len;
}

static int hostapd_ctrl_iface_get_he_phy_channel_width_set  (struct hostapd_data *hapd, const char *cmd, char *reply, size_t reply_len)
{
	int i, ret, len = 0;
	struct sta_info *sta = NULL;
	u8 sta_mac[ETH_ALEN];

	if (hwaddr_aton(cmd, sta_mac)) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_HE_PHY_CHANNEL_WIDTH_SET - Incorrect MAC address passed\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], sta_mac);

		if (sta)
			 break;
	}

	if (!sta) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_HE_PHY_CHANNEL_WIDTH_SET - STA doesn't exist\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	if (!sta->last_assoc_req || !sta->last_assoc_req_len) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_HE_PHY_CHANNEL_WIDTH_SET - STA doesn't have last assoc req\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	if ((NULL == sta->he_capabilities) || (sta->he_capabilities_len_from_sta < 1)) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_HE_PHY_CHANNEL_WIDTH_SET - STA doesn't have HE elements in assoc req\n");
		if (ret <= 0 ) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	struct ieee80211_he_capabilities *he_caps = (struct ieee80211_he_capabilities *)sta->he_capabilities;

	ret = sprintf_s(reply + len, reply_len - len, "he_phy_chanwidth_set=20MHz");
	if (ret <= 0)
		return len;
	len += ret;

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B0) {
			ret = sprintf_s(reply + len, reply_len - len, ",40MHz");
			if (ret <= 0)
				return len;
			len += ret;
	}

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B1) {
			ret = sprintf_s(reply + len, reply_len - len, ",80MHz");
			if (ret <= 0)
				return len;
			len += ret;
	}

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2) {
			ret = sprintf_s(reply + len, reply_len - len, ",160MHz");
			if (ret <= 0)
				return len;
			len += ret;
	}

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B3) {
			ret = sprintf_s(reply + len, reply_len - len, ",80+80MHz");
			if (ret <= 0)
				return len;
			len += ret;
	}

	ret = sprintf_s(reply + len, reply_len - len, "\n");
	if (ret <= 0)
		return len;
	len += ret;

	return len;
}

static int hostapd_ctrl_iface_get_sta_he_caps(struct hostapd_data *hapd, const char *cmd, char *reply, size_t reply_len)
{
	int i, ret, len = 0;
	struct sta_info *sta = NULL;
	u8 sta_mac[ETH_ALEN];

	if (hwaddr_aton(cmd, sta_mac)) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_STA_HE_CAPS - Incorrect MAC address passed\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	for (i = 0; i < hapd->iface->num_bss; i++) {
		sta = ap_get_sta(hapd->iface->bss[i], sta_mac);

		if (sta)
			 break;
	}

	if (!sta) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_STA_HE_CAPS - STA doesn't exist\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	if (!sta->last_assoc_req || !sta->last_assoc_req_len) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_STA_HE_CAPS - STA doesn't have last assoc req\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	if ((NULL == sta->he_capabilities) || (sta->he_capabilities_len_from_sta < 1)) {
		ret = sprintf_s(reply, reply_len, "CTRL: GET_STA_HE_CAPS - STA doesn't have HE elements in assoc req\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	struct ieee80211_he_capabilities *he_caps = (struct ieee80211_he_capabilities *)sta->he_capabilities;

	ret = sprintf_s(reply + len, reply_len - len, "he_phy_chanwidth_set=");
	if (os_snprintf_error(reply_len - len, ret))
		return len;
	len += ret;

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B0) {
			ret = sprintf_s(reply + len, reply_len - len, "B0");
			if (ret <= 0)
				return len;
			len += ret;
	}

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B1) {
			ret = sprintf_s(reply + len, reply_len - len, ",B1");
			if (ret <= 0)
				return len;
			len += ret;
	}

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2) {
			ret = sprintf_s(reply + len, reply_len - len, ",B2");
			if (ret <= 0)
				return len;
			len += ret;
	}

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B3) {
			ret = sprintf_s(reply + len, reply_len - len, ",B3");
			if (ret <= 0)
				return len;
			len += ret;
	}

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B4) {
			ret = sprintf_s(reply + len, reply_len - len, ",B4");
			if (ret <= 0)
				return len;
			len += ret;
	}

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B5) {
			ret = sprintf_s(reply + len, reply_len - len, ",B5");
			if (ret <= 0)
				return len;
			len += ret;
	}

	if(he_caps->he_phy_capab_info[HE_PHYCAP_CAP0_IDX] & HE_PHY_CAP0_CHANNEL_WIDTH_SET_B6) {
			ret = sprintf_s(reply + len, reply_len - len, ",B6(reserved)");
			if (ret <= 0)
				return len;
			len += ret;
	}


	ret = sprintf_s(reply + len, reply_len - len, "\n");
	if (ret <= 0)
		return len;
	len += ret;

	ret = sprintf_s(reply + len, reply_len - len, "he_phy_he_er_su_ppdu_with_4x_he_ltf_and_08_us_gi=%d\n",
		(he_caps->he_phy_capab_info[HE_PHYCAP_CAP7_IDX] & HE_PHY_CAP7_SU_PPDU_AND_HE_MU_WITH_4X_HE_LTF_0_8US_GI)>>2);
	if (ret <= 0)
		return len;
	len += ret;

	ret = sprintf_s(reply + len, reply_len - len, "he_phy_20_mhz_in_40_mhz_he_ppdu_in_24_ghz_band=%d\n",
		(he_caps->he_phy_capab_info[HE_PHYCAP_CAP8_IDX] & HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_2_4_GHZ_BAND)>>1);
	if (ret <= 0)
		return len;
	len += ret;

	ret = sprintf_s(reply + len, reply_len - len, "he_phy_20_mhz_in_160_slash_80_plus_80_mhz_he_ppdu=%d\n",
		(he_caps->he_phy_capab_info[HE_PHYCAP_CAP8_IDX] & HE_PHY_CAP8_20MHZ_IN_160MHZ_HE_PPDU)>>2);
	if (ret <= 0)
		return len;
	len += ret;

	/* Add next fields here */

	return len;
}

static int hostapd_ctrl_driver_flags2(struct hostapd_iface *iface, char *buf,
                                      size_t buflen)
{
        int ret, i;
        char *pos, *end;

        ret = os_snprintf(buf, buflen, "%016llX:\n",
                          (long long unsigned) iface->drv_flags2);
        if (os_snprintf_error(buflen, ret))
                return -1;

        pos = buf + ret;
        end = buf + buflen;

        for (i = 0; i < 64; i++) {
                if (iface->drv_flags2 & (1LLU << i)) {
                        ret = os_snprintf(pos, end - pos, "%s\n",
                                          driver_flag2_to_string(1LLU << i));
                        if (os_snprintf_error(end - pos, ret))
                                return -1;
                        pos += ret;
                }
        }

        return pos - buf;
}


static int hostapd_ctrl_iface_send_action_frame(struct hostapd_data *hapd,
					      char *cmd)
{
	u8 addr[ETH_ALEN];
	unsigned int freq;
	struct wpabuf *buf;
	char *tmp;
	int ret;

	hapd = get_bss_index(cmd, hapd->iface);
	if (!hapd) {
		wpa_printf(MSG_ERROR,
				"CTRL: SEND_ACTION_FRAME - there is no iface with the given name");
		return -1;
	}
	cmd = os_strchr(cmd, ' ');
	if (!cmd) {
		wpa_printf(MSG_ERROR,
				"CTRL: SEND_ACTION_FRAME - mac address is missing");
		return -1;
	}
	cmd++;

	if (hwaddr_aton(cmd, addr)) {
		wpa_printf(MSG_ERROR,
				"CTRL: SEND_ACTION_FRAME - invalid peer mac address");
		return -1;
	}

	cmd = os_strchr(cmd, ' ');
	if (cmd == NULL) {
		wpa_printf(MSG_ERROR, "CTRL: SEND_ACTION_FRAME - Freq is missing");
		return -1;
	}
	cmd++;
	freq = strtoul(cmd, &tmp, 10);
	if (freq != (unsigned int)hapd->iface->freq) {
		wpa_printf(MSG_DEBUG,
				"CTRL: SEND_ACTION_FRAME - requested Off channel");
	}

	tmp = os_strstr(cmd, "frame=");
	if (tmp == NULL) {
		wpa_printf(MSG_ERROR,
				"CTRL: SEND_ACTION_FRAME - Frame buffer is missing");
		return -1;
	}
	cmd = os_strchr(cmd, ' ');
	if (cmd)
		*cmd++ = '\0';

	buf = wpabuf_parse_bin(tmp + 6);
	if (!buf) {
		wpa_printf(MSG_ERROR,
			   "CTRL: SEND_ACTION_FRAME: Bad frame buffer");
		return -1;
	}

	wpa_printf(MSG_INFO, "Send action frame to " MACSTR, MAC2STR(addr));

	ret = hostapd_drv_send_action(hapd, freq, 0, addr,
				      wpabuf_head(buf), wpabuf_len(buf));

	wpabuf_free(buf);

	return ret;
}

#ifdef CONFIG_ALLOW_HOSTAPD
static int hostapd_ctrl_iface_get_capability(struct hostapd_data *hapd,
					     const char *field, char *buf,
					     size_t buflen)
{
	wpa_printf(MSG_DEBUG, "CTRL_IFACE: GET_CAPABILITY '%s'", field);

#ifdef CONFIG_DPP
	if (os_strcmp(field, "dpp") == 0) {
		int res;

#ifdef CONFIG_DPP2
		res = os_snprintf(buf, buflen, "DPP=2");
#else /* CONFIG_DPP2 */
		res = os_snprintf(buf, buflen, "DPP=1");
#endif /* CONFIG_DPP2 */
		if (os_snprintf_error(buflen, res))
			return -1;
		return res;
	}
#endif /* CONFIG_DPP */

	wpa_printf(MSG_DEBUG, "CTRL_IFACE: Unknown GET_CAPABILITY field '%s'",
		   field);

	return -1;
}
#endif /* CONFIG_ALLOW_HOSTAPD */

static int hostapd_ctrl_iface_get_csa_deauth (struct hostapd_data *hapd, const char *cmd,
	char *buf, size_t buflen)
{
	int ret, len = 0;

	hapd = get_bss_index(cmd, hapd->iface);
	if (hapd == NULL) {
		ret = sprintf_s(buf, buflen, "CTRL: GET_CSA_DEAUTH - there is no iface with the given name\n");
		if (ret <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, ret);
			return 0;
		}
		return ret;
	}

	ret = sprintf_s(buf + len, buflen - len, "mode=%d unicast_tx_time=%d multicast_tx_time=%d\n",
		hapd->csa_deauth_mode, hapd->csa_deauth_tx_time[0], hapd->csa_deauth_tx_time[1]);
	if (ret <= 0)
		return len;
	len += ret;

	return len;
}

#ifdef CONFIG_ACS
static void hostapd_ctrl_iface_reset_acs_state(struct hostapd_data *hapd, char *cmd)
{
	hapd->iface->num_candidates=0;
	hapd->iconf->acs_init_done=0;
}
#endif /* CONFIG_ACS */

#ifdef CONFIG_DEBUG_CTRL_IFACE
static void hostapd_ctrl_iface_inject_debug_hostap_event(struct hostapd_data *hapd, char *cmd)
{
	wpa_msg(hapd->msg_ctx, MSG_INFO, "%s", cmd);
}
#endif /* CONFIG_DEBUG_CTRL_IFACE */

static int hostapd_ctrl_iface_receive_process(struct hostapd_data *hapd,
					      char *buf, char *reply,
					      int reply_size,
					      struct sockaddr_storage *from,
					      socklen_t fromlen)
{
	int reply_len, res;
	os_memcpy(reply, "OK\n", 3);
	reply_len = 3;

	if (os_strcmp(buf, "PING") == 0) {
		os_memcpy(reply, "PONG\n", 5);
		reply_len = 5;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strncmp(buf, "RELOG", 5) == 0) {
		if (wpa_debug_reopen_file() < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "NOTE ", 5) == 0) {
		wpa_printf(MSG_INFO, "NOTE: %s", buf + 5);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "STATUS") == 0) {
		reply_len = hostapd_ctrl_iface_status(hapd, reply,
						      reply_size);
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "STATUS-DRIVER") == 0) {
		reply_len = hostapd_drv_status(hapd, reply, reply_size);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "MIB") == 0) {
		reply_len = ieee802_11_get_mib(hapd, reply, reply_size);
		if (reply_len >= 0) {
			res = wpa_get_mib(hapd->wpa_auth, reply + reply_len,
					  reply_size - reply_len);
			if (res < 0)
				reply_len = -1;
			else
				reply_len += res;
		}
		if (reply_len >= 0) {
			res = ieee802_1x_get_mib(hapd, reply + reply_len,
						 reply_size - reply_len);
			if (res < 0)
				reply_len = -1;
			else
				reply_len += res;
		}
#ifndef CONFIG_NO_RADIUS
		if (reply_len >= 0) {
			res = radius_client_get_mib(hapd->radius,
						    reply + reply_len,
						    reply_size - reply_len);
			if (res < 0)
				reply_len = -1;
			else
				reply_len += res;
		}
#endif /* CONFIG_NO_RADIUS */
	} else if (os_strncmp(buf, "MIB ", 4) == 0) {
		reply_len = hostapd_ctrl_iface_mib(hapd, reply, reply_size,
						   buf + 4);
	} else if (os_strncmp(buf, "STA-FIRST ", 10) == 0) {
		reply_len = hostapd_ctrl_iface_sta_first(hapd, buf + 10, reply,
							 reply_size);
	} else if (os_strncmp(buf, "STA ", 4) == 0) {
		reply_len = hostapd_ctrl_iface_sta(hapd, buf + 4, reply,
						   reply_size);
	} else if (os_strncmp(buf, "STA-NEXT ", 9) == 0) {
		reply_len = hostapd_ctrl_iface_sta_next(hapd, buf + 9, reply,
							reply_size);
	} else if (os_strcmp(buf, "ATTACH") == 0) {
		if (hostapd_ctrl_iface_attach(hapd, from, fromlen, NULL))
			reply_len = -1;
	} else if (os_strncmp(buf, "ATTACH ", 7) == 0) {
		if (hostapd_ctrl_iface_attach(hapd, from, fromlen, buf + 7))
			reply_len = -1;
	} else if (os_strcmp(buf, "DETACH") == 0) {
		if (hostapd_ctrl_iface_detach(hapd, from, fromlen))
			reply_len = -1;
	} else if (os_strncmp(buf, "LEVEL ", 6) == 0) {
		if (hostapd_ctrl_iface_level(hapd, from, fromlen,
						    buf + 6))
			reply_len = -1;
	} else if (os_strncmp(buf, "NEW_STA ", 8) == 0) {
		if (hostapd_ctrl_iface_new_sta(hapd, buf + 8))
			reply_len = -1;
	} else if (os_strncmp(buf, "DEAUTHENTICATE ", 15) == 0) {
		if (hostapd_ctrl_iface_deauthenticate(hapd, buf + 15))
			reply_len = -1;
	} else if (os_strncmp(buf, "DISASSOCIATE ", 13) == 0) {
		if (hostapd_ctrl_iface_disassociate(hapd, buf + 13))
			reply_len = -1;
#ifdef CONFIG_TAXONOMY
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strncmp(buf, "SIGNATURE ", 10) == 0) {
		reply_len = hostapd_ctrl_iface_signature(hapd, buf + 10,
							 reply, reply_size);
#endif /* CONFIG_ALLOW_HOSTAPD */
#endif /* CONFIG_TAXONOMY */
	} else if (os_strncmp(buf, "POLL_STA ", 9) == 0) {
		if (hostapd_ctrl_iface_poll_sta(hapd, buf + 9))
			reply_len = -1;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "STOP_AP") == 0) {
		if (hostapd_ctrl_iface_stop_ap(hapd))
			reply_len = -1;
#endif /* CONFIG_ALLOW_HOSTAPD */
#ifdef NEED_AP_MLME
	} else if (os_strncmp(buf, "SA_QUERY ", 9) == 0) {
		if (hostapd_ctrl_iface_sa_query(hapd, buf + 9))
			reply_len = -1;
#endif /* NEED_AP_MLME */
#ifdef CONFIG_WPS
	} else if (os_strncmp(buf, "WPS_PIN ", 8) == 0) {
		if (hostapd_ctrl_iface_wps_pin(hapd, buf + 8))
			reply_len = -1;
	} else if (os_strncmp(buf, "WPS_CHECK_PIN ", 14) == 0) {
		reply_len = hostapd_ctrl_iface_wps_check_pin(
			hapd, buf + 14, reply, reply_size);
	} else if (os_strcmp(buf, "WPS_PBC") == 0) {
		if (hostapd_wps_button_pushed(hapd, NULL))
			reply_len = -1;
	} else if (os_strcmp(buf, "WPS_CANCEL") == 0) {
		if (hostapd_wps_cancel(hapd))
			reply_len = -1;
	} else if (os_strncmp(buf, "WPS_AP_PIN ", 11) == 0) {
		reply_len = hostapd_ctrl_iface_wps_ap_pin(hapd, buf + 11,
							  reply, reply_size);
	} else if (os_strncmp(buf, "WPS_CONFIG ", 11) == 0) {
		if (hostapd_ctrl_iface_wps_config(hapd, buf + 11) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "WPS_GET_STATUS", 13) == 0) {
		reply_len = hostapd_ctrl_iface_wps_get_status(hapd, reply,
							      reply_size);
#ifdef CONFIG_WPS_NFC
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strncmp(buf, "WPS_NFC_TAG_READ ", 17) == 0) {
		if (hostapd_ctrl_iface_wps_nfc_tag_read(hapd, buf + 17))
			reply_len = -1;
	} else if (os_strncmp(buf, "WPS_NFC_CONFIG_TOKEN ", 21) == 0) {
		reply_len = hostapd_ctrl_iface_wps_nfc_config_token(
			hapd, buf + 21, reply, reply_size);
	} else if (os_strncmp(buf, "WPS_NFC_TOKEN ", 14) == 0) {
		reply_len = hostapd_ctrl_iface_wps_nfc_token(
			hapd, buf + 14, reply, reply_size);
	} else if (os_strncmp(buf, "NFC_GET_HANDOVER_SEL ", 21) == 0) {
		reply_len = hostapd_ctrl_iface_nfc_get_handover_sel(
			hapd, buf + 21, reply, reply_size);
	} else if (os_strncmp(buf, "NFC_REPORT_HANDOVER ", 20) == 0) {
		if (hostapd_ctrl_iface_nfc_report_handover(hapd, buf + 20))
			reply_len = -1;
#endif /* CONFIG_ALLOW_HOSTAPD */
#endif /* CONFIG_WPS_NFC */
#endif /* CONFIG_WPS */
	} else if (os_strncmp(buf, "UPDATE_WAN_METRICS ", 19) == 0) {
		if (hostapd_ctrl_iface_update_wan_metrics(hapd, buf + 19))
			reply_len = -1;
#ifdef CONFIG_INTERWORKING
	} else if (os_strncmp(buf, "SET_QOS_MAP_SET ", 16) == 0) {
		if (hostapd_ctrl_iface_set_qos_map_set(hapd, buf + 16))
			reply_len = -1;
	} else if (os_strncmp(buf, "SEND_QOS_MAP_CONF ", 18) == 0) {
		if (hostapd_ctrl_iface_send_qos_map_conf(hapd, buf + 18))
			reply_len = -1;
#endif /* CONFIG_INTERWORKING */
#ifdef CONFIG_HS20
	} else if (os_strncmp(buf, "HS20_WNM_NOTIF ", 15) == 0) {
		if (hostapd_ctrl_iface_hs20_wnm_notif(hapd, buf + 15))
			reply_len = -1;
	} else if (os_strncmp(buf, "HS20_DEAUTH_REQ ", 16) == 0) {
		if (hostapd_ctrl_iface_hs20_deauth_req(hapd, buf + 16))
			reply_len = -1;
#endif /* CONFIG_HS20 */
#ifdef CONFIG_WNM_AP
	} else if (os_strncmp(buf, "DISASSOC_IMMINENT ", 18) == 0) {
		if (hostapd_ctrl_iface_disassoc_imminent(hapd, buf + 18))
			reply_len = -1;
	} else if (os_strncmp(buf, "ESS_DISASSOC ", 13) == 0) {
		if (hostapd_ctrl_iface_ess_disassoc(hapd, buf + 13))
			reply_len = -1;
	} else if (os_strncmp(buf, "BSS_TM_REQ ", 11) == 0) {
		reply_len = hostapd_ctrl_iface_bss_tm_req(hapd, buf + 11,
                                reply, reply_size);
	} else if (os_strncmp(buf, "COLOC_INTF_REQ ", 15) == 0) {
		if (hostapd_ctrl_iface_coloc_intf_req(hapd, buf + 15))
			reply_len = -1;
#endif /* CONFIG_WNM_AP */
	} else if (os_strcmp(buf, "GET_CONFIG") == 0) {
		reply_len = hostapd_ctrl_iface_get_config(hapd, reply,
							  reply_size);
	} else if (os_strncmp(buf, "SET ", 4) == 0) {
		if (hostapd_ctrl_iface_set(hapd, buf + 4))
			reply_len = -1;
	} else if (os_strncmp(buf, "GET ", 4) == 0) {
		reply_len = hostapd_ctrl_iface_get(hapd, buf + 4, reply,
						   reply_size);
	} else if (os_strncmp(buf, "ENABLE", 6) == 0) {
		if (hostapd_ctrl_iface_enable(hapd->iface))
			reply_len = -1;
	} else if (os_strcmp(buf, "RELOAD_WPA_PSK") == 0) {
		if (hostapd_ctrl_iface_reload_wpa_psk(hapd))
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_STA_WPA_PSK ", 16) == 0) {
		reply_len = hostapd_ctrl_iface_get_sta_wpa_psk(hapd, buf + 16,
							       reply, reply_size);
	} else if (os_strncmp(buf, "RELOAD", 6) == 0) {
		if (hostapd_ctrl_iface_reload(hapd->iface))
			reply_len = -1;
#ifdef CONFIG_IEEE80211AX
	} else if (os_strncmp(buf, "UPDATE_EDCA_CNTR", 16) == 0) {
		if (hostapd_ctrl_iface_update_edca_counter(hapd->iface))
			reply_len = -1;
#endif /* CONFIG_IEEE80211AX */
	} else if (os_strncmp(buf, "RECONF", 6) == 0) {
		if (hostapd_ctrl_iface_reconf(hapd->iface, buf + 6))
			reply_len = -1;
	} else if (os_strncmp(buf, "SET_CREDENTIALS", 15) == 0) {
		if (hostapd_ctrl_iface_credentials(hapd->iface, buf + 15))
			reply_len = -1;
	} else if (os_strncmp(buf, "DISABLE", 7) == 0) {
		if (hostapd_ctrl_iface_disable(hapd->iface))
			reply_len = -1;
	} else if (os_strcmp(buf, "UPDATE_BEACON") == 0) {
		if (ieee802_11_set_beacon(hapd))
			reply_len = -1;
#ifdef CONFIG_TESTING_OPTIONS
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strncmp(buf, "RADAR ", 6) == 0) {
		if (hostapd_ctrl_iface_radar(hapd, buf + 6))
			reply_len = -1;
	} else if (os_strncmp(buf, "MGMT_TX ", 8) == 0) {
		if (hostapd_ctrl_iface_mgmt_tx(hapd, buf + 8))
			reply_len = -1;
	} else if (os_strncmp(buf, "MGMT_TX_STATUS_PROCESS ", 23) == 0) {
		if (hostapd_ctrl_iface_mgmt_tx_status_process(hapd,
							      buf + 23) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "MGMT_RX_PROCESS ", 16) == 0) {
		if (hostapd_ctrl_iface_mgmt_rx_process(hapd, buf + 16) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "EAPOL_RX ", 9) == 0) {
		if (hostapd_ctrl_iface_eapol_rx(hapd, buf + 9) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "DATA_TEST_CONFIG ", 17) == 0) {
		if (hostapd_ctrl_iface_data_test_config(hapd, buf + 17) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "DATA_TEST_TX ", 13) == 0) {
		if (hostapd_ctrl_iface_data_test_tx(hapd, buf + 13) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "DATA_TEST_FRAME ", 16) == 0) {
		if (hostapd_ctrl_iface_data_test_frame(hapd, buf + 16) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "TEST_ALLOC_FAIL ", 16) == 0) {
		if (hostapd_ctrl_test_alloc_fail(hapd, buf + 16) < 0)
			reply_len = -1;
	} else if (os_strcmp(buf, "GET_ALLOC_FAIL") == 0) {
		reply_len = hostapd_ctrl_get_alloc_fail(hapd, reply,
							reply_size);
	} else if (os_strncmp(buf, "TEST_FAIL ", 10) == 0) {
		if (hostapd_ctrl_test_fail(hapd, buf + 10) < 0)
			reply_len = -1;
	} else if (os_strcmp(buf, "GET_FAIL") == 0) {
		reply_len = hostapd_ctrl_get_fail(hapd, reply, reply_size);
	} else if (os_strncmp(buf, "RESET_PN ", 9) == 0) {
		if (hostapd_ctrl_reset_pn(hapd, buf + 9) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "SET_KEY ", 8) == 0) {
		if (hostapd_ctrl_set_key(hapd, buf + 8) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "RESEND_M1 ", 10) == 0) {
		if (hostapd_ctrl_resend_m1(hapd, buf + 10) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "RESEND_M3 ", 10) == 0) {
		if (hostapd_ctrl_resend_m3(hapd, buf + 10) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "RESEND_GROUP_M1 ", 16) == 0) {
		if (hostapd_ctrl_resend_group_m1(hapd, buf + 16) < 0)
			reply_len = -1;
	} else if (os_strcmp(buf, "REKEY_GTK") == 0) {
		if (wpa_auth_rekey_gtk(hapd->wpa_auth) < 0)
			reply_len = -1;
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strncmp(buf, "GET_PMK ", 8) == 0) {
		reply_len = hostapd_ctrl_get_pmk(hapd, buf + 8, reply,
						 reply_size);
#endif /* CONFIG_TESTING_OPTIONS */
#ifdef CONFIG_WIFI_CERTIFICATION
	} else if (os_strncmp(buf, "SET_UNPROTECTED_DEAUTH ", 23) == 0) {
		if (hostapd_ctrl_iface_set_unprotected_deauth(hapd, buf + 23) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "SET_PN_RESET ", 13) == 0) {
		if (hostapd_ctrl_iface_set_pn_reset(hapd, buf + 13) < 0)
			reply_len = -1;
	} else if (os_strcmp(buf, "PMK") == 0) {
		reply_len = hostapd_ctrl_iface_pmk_list(hapd, reply,
							  reply_size);
	} else if (os_strncmp(buf, "SET_INCORRECT_PN ", 17) == 0) {
		if (hostapd_ctrl_set_incorrect_pn(hapd, buf + 17) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "SET_CORRECT_PN ", 15) == 0) {
		if (hostapd_ctrl_set_correct_pn(hapd, buf + 15) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_PTK_REKEY_TIMER ", 20) == 0) {
		reply_len = hostapd_ctrl_get_ptk_rekey_timer(hapd, buf + 20,
				 reply, reply_size);
	} else if (os_strncmp(buf, "SET_PTK_REKEY_TIMER ", 20) == 0) {
		if (hostapd_ctrl_set_ptk_rekey_timer(hapd, buf + 20) < 0)
			reply_len = -1;
#endif /*CONFIG_WIFI_CERTIFICATION */
	} else if (os_strncmp(buf, "CHAN_SWITCH ", 12) == 0) {
		if (hostapd_ctrl_iface_chan_switch(hapd->iface, buf + 12))
			reply_len = -1;
	} else if (os_strncmp(buf, "VENDOR ", 7) == 0) {
		reply_len = hostapd_ctrl_iface_vendor(hapd, buf + 7, reply,
						      reply_size);
#ifdef CONFIG_ACS
	} else if (os_strncmp(buf, "ACS_RECALC", 10) == 0) {
		if (hostapd_ctrl_iface_acs_recalc(hapd->iface))
			reply_len = -1;
#endif
	} else if (os_strncmp(buf, "DENY_MAC ", 9) == 0) {
		if (hostapd_ctrl_iface_deny_mac(hapd, buf + 9))
			reply_len = -1;
	} else if (os_strncmp(buf, "STA_SOFTBLOCK ", 14) == 0) {
		if (hostapd_ctrl_iface_sta_softblock(hapd, buf + 14))
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_STA_SOFTBLOCK ", 18) == 0) {
		reply_len = hostapd_ctrl_iface_get_sta_softblock(hapd, buf + 18, reply, reply_size);
	} else if (os_strncmp(buf, "STA_STEER ", 10) == 0) {
		wpa_printf(MSG_MUTE, "%s; *** Received 'STA_STEER' (buf= '%s') ***\n", __FUNCTION__, buf);
		if (hostapd_ctrl_iface_sta_steer(hapd, buf + 10))
			reply_len = -1;
	} else if (os_strncmp(buf, "STA_ALLOW", 9) == 0) {
		wpa_printf(MSG_MUTE, "%s; *** Received 'STA_ALLOW' (buf= '%s') ***\n", __FUNCTION__, buf);
		if (hostapd_ctrl_iface_sta_allow(hapd, buf + 9))
			reply_len = -1;
    } else if (os_strncmp(buf, "SET_BSS_LOAD ", 13) == 0) {
        wpa_printf(MSG_MUTE, "%s; *** Received: 'SET_BSS_LOAD' (buf= '%s') ***\n", __FUNCTION__, buf);
        reply_len = hostapd_ctrl_iface_set_bss_load(hapd, buf + 13, reply,
                    reply_size);
	} else if (os_strncmp(buf, "GET_BLACKLIST", 13) == 0) {
		reply_len = hostapd_ctrl_iface_get_blacklist(hapd->iface, reply, reply_size);
	} else if (os_strncmp(buf, "GET_STA_MEASUREMENTS ", 21) == 0) {
		wpa_printf(MSG_DEBUG, "%s; *** Received 'GET_STA_MEASUREMENTS' (buf= '%s') ***\n", __FUNCTION__, buf);
		reply_len = hostapd_ctrl_iface_get_sta_measurements(hapd, buf + 21, reply,
					reply_size);
	} else if (os_strncmp(buf, "GET_VAP_MEASUREMENTS ", 21) == 0) {
		wpa_printf(MSG_DEBUG, "%s; *** Received 'GET_VAP_MEASUREMENTS' (buf= '%s') ***\n", __FUNCTION__, buf);
		reply_len = hostapd_ctrl_iface_get_vap_measurements(hapd, buf + 21, reply,
					reply_size);
	} else if (os_strncmp(buf, "GET_RADIO_INFO", 14) == 0) {
		wpa_printf(MSG_DEBUG, "%s; *** Received 'GET_RADIO_INFO' (buf= '%s') ***\n", __FUNCTION__, buf);
		reply_len = hostapd_ctrl_iface_get_radio_info(hapd, NULL, reply,
					reply_size);
	} else if (os_strncmp(buf, "UPDATE_ATF_CFG", 14) == 0) {
		if (hostapd_ctrl_iface_update_atf_cfg(hapd))
			reply_len = -1;
	} else if (os_strncmp(buf, "UNCONNECTED_STA_RSSI ", 21) == 0) {
		if (hostapd_ctrl_iface_unconnected_sta(hapd, buf + 21))
			reply_len = -1;
	} else if (os_strncmp(buf, "SET_FAILSAFE_CHAN ", 18) == 0) {
		if (hostapd_ctrl_iface_set_failsafe_chan(hapd->iface, buf + 18))
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_FAILSAFE_CHAN", 17) == 0) {
		reply_len = hostapd_ctrl_iface_get_failsafe_chan(hapd->iface, NULL, reply,
					reply_size);
#ifdef CONFIG_ACS
	} else if (os_strncmp(buf, "GET_ACS_REPORT_ALL_CH", 21) == 0) {
		reply_len = hostapd_ctrl_iface_acs_report(hapd->iface, NULL, reply, reply_size, false);
	} else if (os_strncmp(buf, "GET_ACS_REPORT", 14) == 0) {
		reply_len = hostapd_ctrl_iface_acs_report(hapd->iface, NULL, reply, reply_size, true);
#endif
	} else if (os_strncmp(buf, "RESTRICTED_CHANNELS", 19) == 0) {
		if (hostapd_ctrl_iface_set_restricted_chan(hapd->iface, buf + 19))
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_RESTRICTED_CHANNELS", 23) == 0) {
		reply_len = hostapd_ctrl_iface_get_restricted_chan(hapd->iface, NULL, reply,
					reply_size);
#ifdef CONFIG_MBO
	} else if (os_strncmp(buf, "MBO_BSS_ASSOC_DISALLOW ", 23) == 0) {
		if (hostapd_ctrl_iface_mbo_bss_assoc_disallow(hapd, buf + 23))
			reply_len = -1;
	} else if (os_strncmp(buf, "CELLULAR_PREF_SET ", 18) == 0) {
		if (hostapd_ctrl_iface_cellular_pref_set(hapd, buf + 18))
			reply_len = -1;
#endif /* CONFIG_MBO */
	} else if (os_strncmp(buf, "UPDATE_RELOAD ", 14) == 0) {
		if (hostapd_ctrl_iface_update_reload(hapd, buf + 14))
			reply_len = -1;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "ERP_FLUSH") == 0) {
		ieee802_1x_erp_flush(hapd);
#ifdef RADIUS_SERVER
		radius_server_erp_flush(hapd->radius_srv);
#endif /* RADIUS_SERVER */
	} else if (os_strncmp(buf, "EAPOL_REAUTH ", 13) == 0) {
		if (hostapd_ctrl_iface_eapol_reauth(hapd, buf + 13))
			reply_len = -1;
	} else if (os_strncmp(buf, "EAPOL_SET ", 10) == 0) {
		if (hostapd_ctrl_iface_eapol_set(hapd, buf + 10))
			reply_len = -1;
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strncmp(buf, "LOG_LEVEL", 9) == 0) {
		reply_len = hostapd_ctrl_iface_log_level(
			hapd, buf + 9, reply, reply_size);
#ifdef CONFIG_ALLOW_HOSTAPD
#ifdef NEED_AP_MLME
	} else if (os_strcmp(buf, "TRACK_STA_LIST") == 0) {
		reply_len = hostapd_ctrl_iface_track_sta_list(
			hapd, reply, reply_size);
#endif /* NEED_AP_MLME */
	} else if (os_strcmp(buf, "PMKSA") == 0) {
		reply_len = hostapd_ctrl_iface_pmksa_list(hapd, reply,
							  reply_size);
	} else if (os_strcmp(buf, "PMKSA_FLUSH") == 0) {
		hostapd_ctrl_iface_pmksa_flush(hapd);
	} else if (os_strncmp(buf, "PMKSA_ADD ", 10) == 0) {
		if (hostapd_ctrl_iface_pmksa_add(hapd, buf + 10) < 0)
			reply_len = -1;
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strncmp(buf, "SET_NEIGHBOR ", 13) == 0) {
		if (hostapd_ctrl_iface_set_neighbor(hapd, buf + 13))
			reply_len = -1;
	} else if (os_strncmp(buf, "SET_NEIGHBOR_PER_VAP ", 21) == 0) {
		if (hostapd_ctrl_iface_set_neighbor_per_vap(hapd, buf + 21))
			reply_len = -1;
	} else if (os_strcmp(buf, "SHOW_NEIGHBOR") == 0) {
		reply_len = hostapd_ctrl_iface_show_neighbor(hapd, reply,
							     reply_size);
	} else if (os_strncmp(buf, "REMOVE_NEIGHBOR ", 16) == 0) {
		if (hostapd_ctrl_iface_remove_neighbor(hapd, buf + 16))
			reply_len = -1;
	} else if (os_strncmp(buf, "REMOVE_NEIGHBOR_PER_VAP ", 24) == 0) {
		if (hostapd_ctrl_iface_remove_neighbor_per_vap(hapd, buf + 24))
			reply_len = -1;
	} else if (os_strncmp(buf, "CLEAN_NEIGHBOR_DB_PER_VAP ", 26) == 0) {
		if (hostapd_ctrl_iface_clean_neighbordb_per_vap(hapd, buf + 26))
			reply_len = -1;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strncmp(buf, "REQ_LCI ", 8) == 0) {
		if (hostapd_ctrl_iface_req_lci(hapd, buf + 8))
			reply_len = -1;
	} else if (os_strncmp(buf, "REQ_RANGE ", 10) == 0) {
		if (hostapd_ctrl_iface_req_range(hapd, buf + 10))
			reply_len = -1;
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strncmp(buf, "REQ_BEACON ", 11) == 0) {
		reply_len = hostapd_ctrl_iface_req_beacon(hapd, buf + 11,
							  reply, reply_size);
	} else if (os_strncmp(buf, "REPORT_BEACON ", 14) == 0) {
		if (hostapd_ctrl_iface_report_beacon(hapd, buf + 14))
			reply_len = -1;
	} else if (os_strncmp(buf, "REQ_SELF_BEACON ", 16) == 0) {
		if (hostapd_ctrl_iface_req_self_beacon(hapd, buf + 16))
			reply_len = -1;
	} else if (os_strncmp(buf, "REQ_LINK_MEASUREMENT ", 21) == 0) {
		reply_len = hostapd_ctrl_iface_req_link_measurement(hapd, buf + 21,
				reply, reply_size);
	} else if (os_strncmp(buf, "LINK_MEASUREMENT_REPORT ", 24) == 0) {
		if (hostapd_ctrl_iface_link_measurement_report(hapd, buf + 24))
			reply_len = -1;
	} else if (os_strncmp(buf, "REQ_STA_STATISTICS ", 19) == 0) {
		reply_len = hostapd_ctrl_iface_req_sta_statistics(hapd, buf + 19,
				reply, reply_size);
	} else if (os_strncmp(buf, "REPORT_STA_STATISTICS ", 22) == 0) {
		if (hostapd_ctrl_iface_report_sta_statistics(hapd, buf + 22))
			reply_len = -1;
	} else if (os_strncmp(buf, "REQ_CHANNEL_LOAD ", 17) == 0) {
		reply_len = hostapd_ctrl_iface_req_channel_load(hapd, buf + 17,
				reply, reply_size);
	} else if (os_strncmp(buf, "REPORT_CHANNEL_LOAD ", 20) == 0) {
		if (hostapd_ctrl_iface_report_channel_load(hapd, buf + 20))
			reply_len = -1;
	} else if (os_strncmp(buf, "REQ_NOISE_HISTOGRAM ", 20) == 0) {
		reply_len = hostapd_ctrl_iface_req_noise_histogram(hapd, buf + 20,
				reply, reply_size);
	} else if (os_strncmp(buf, "REPORT_NOISE_HISTOGRAM ", 23) == 0) {
		if (hostapd_ctrl_iface_report_noise_histogram(hapd, buf + 23))
			reply_len = -1;
	} else if (os_strcmp(buf, "DRIVER_FLAGS") == 0) {
		reply_len = hostapd_ctrl_driver_flags(hapd->iface, reply,
						      reply_size);
	} else if (os_strcmp(buf, "DRIVER_FLAGS2") == 0) {
		reply_len = hostapd_ctrl_driver_flags2(hapd->iface, reply,
						       reply_size);
	} else if (os_strcmp(buf, "TERMINATE") == 0) {
		eloop_terminate();
	} else if (os_strncmp(buf, "ACCEPT_ACL ", sizeof("ACCEPT_ACL ") - 1 ) == 0) {
		char *buf_start,*mac;
		int len;
		struct hostapd_data *bss = get_bss_index(buf + sizeof("ACCEPT_ACL ") - 1, hapd->iface);
		if(!bss)
			reply_len = -1;
		else{
			//ACCEPT_ACL wlan0.3 ADD_MAC 11:22:33:44:55:66
			len = strnlen_s(bss->conf->iface, sizeof(bss->conf->iface) - 1);
			if (IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, sizeof(bss->conf->iface) - 1)) {
				buf_start = buf + sizeof("ACCEPT_ACL ") - 1 + len + sizeof(" ") - 1;
				mac = buf_start + sizeof("ADD_MAC ") - 1 ;//ADD_MAC and DEL_MAC same len 
				if (os_strncmp(buf_start, "ADD_MAC ", sizeof("ADD_MAC ") - 1) == 0) {
					if (!hostapd_ctrl_iface_acl_add_mac(
								&bss->conf->accept_mac,
								&bss->conf->num_accept_mac, mac))
						hostapd_disassoc_accept_mac(bss);
					else
						reply_len = -1;
				} else if (os_strncmp(buf_start, "DEL_MAC ", sizeof("DEL_MAC ") - 1) == 0) {
					hostapd_ctrl_iface_acl_del_mac(
							&bss->conf->accept_mac,
							&bss->conf->num_accept_mac, mac);
				} else if (os_strncmp(buf_start, "DIS_MAC ", sizeof("DIS_MAC ") - 1) == 0) {
					if (!hostapd_ctrl_iface_acl_del_mac(
								&bss->conf->accept_mac,
								&bss->conf->num_accept_mac, mac))
						hostapd_disassoc_accept_mac(bss);
					else
						reply_len = -1;
				} else if (os_strncmp(buf_start, "DIS_ALL", sizeof("DIS_ALL") - 1) == 0) {
					hostapd_ctrl_iface_acl_clear_list(
							&bss->conf->accept_mac,
							&bss->conf->num_accept_mac);
					hostapd_disassoc_accept_mac(bss);
				} else if (os_strncmp(buf_start, "SHOW", sizeof("SHOW") - 1) == 0) {
					reply_len = hostapd_ctrl_iface_acl_show_mac(
							bss->conf->accept_mac,
							bss->conf->num_accept_mac, reply, reply_size);
				} else if (os_strncmp(buf_start, "CLEAR", sizeof("CLEAR") - 1) == 0) {
					hostapd_ctrl_iface_acl_clear_list(
							&bss->conf->accept_mac,
							&bss->conf->num_accept_mac);
				}
			} else {
				reply_len = -1;
			}
		}
	} else if (os_strncmp(buf, "DENY_ACL ", sizeof("DENY_ACL ") -1) == 0) {
		char *buf_start, *mac, *is_softblock;
		int len;
		struct hostapd_data *bss = get_bss_index(buf + sizeof("DENY_ACL ") - 1, hapd->iface);
		if(!bss)
			reply_len = -1;
		else {
			len = strnlen_s(bss->conf->iface, sizeof(bss->conf->iface) - 1);
			if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, len - 1)) {
				buf_start = buf + sizeof("DENY_ACL ") - 1 + len + sizeof(" ") - 1;
				mac = buf_start + sizeof("ADD_MAC ") - 1;
				is_softblock = os_strstr(buf_start, "TIMER");
				if (is_softblock) { /* softblock deny list */
					if (os_strncmp(buf_start, "ADD_MAC ", sizeof("ADD_MAC ") - 1) == 0) {
						if (!hostapd_ctrl_iface_soft_block_acl_add_mac(
								&bss->conf->deny_mac_sb, mac))
							hostapd_soft_block_disassoc_deny_mac(bss);
					} else if (os_strncmp(buf_start, "DEL_MAC ", sizeof("DEL_MAC ") - 1) == 0) {
						hostapd_ctrl_iface_soft_block_acl_del_mac(
							&bss->conf->deny_mac_sb, mac);
					} else if (os_strncmp(buf_start, "SHOW", sizeof("SHOW") -1) == 0) {
						reply_len = hostapd_ctrl_iface_soft_block_acl_show_mac(
							&bss->conf->deny_mac_sb, reply, reply_size);
					} else if (os_strncmp(buf_start, "CLEAR", sizeof("CLEAR") -1) == 0) {
						hostapd_soft_block_acl_clear_list(
							&bss->conf->deny_mac_sb);
					} else if (os_strncmp(buf_start, "GET_PARAMS", sizeof("GET_PARAMS") -1) == 0) {
						reply_len = hostapd_ctrl_iface_soft_block_acl_get_params(
							bss, reply, reply_size);
					}
				} else { /* regular deny list */
					if (os_strncmp(buf_start, "ADD_MAC ", sizeof("ADD_MAC ") - 1) == 0) {
						if (!hostapd_ctrl_iface_acl_add_mac(
								&bss->conf->deny_mac,
								&bss->conf->num_deny_mac, mac))
							hostapd_disassoc_deny_mac(bss);
					} else if (os_strncmp(buf_start, "DEL_MAC ", sizeof("DEL_MAC ") - 1) == 0) {
						hostapd_ctrl_iface_acl_del_mac(
							&bss->conf->deny_mac,
							&bss->conf->num_deny_mac, mac);
					} else if (os_strncmp(buf_start, "SHOW", sizeof("SHOW") -1) == 0) {
						reply_len = hostapd_ctrl_iface_acl_show_mac(
							bss->conf->deny_mac,
							bss->conf->num_deny_mac, reply, reply_size);
					} else if (os_strncmp(buf_start, "CLEAR", sizeof("CLEAR") -1) == 0) {
						hostapd_ctrl_iface_acl_clear_list(
							&bss->conf->deny_mac,
							&bss->conf->num_deny_mac);
					}
				}
			} else {
				reply_len = -1;
			}
		}
	} else if (os_strncmp(buf, "GET_HW_FEATURES", 15) == 0) {
		reply_len = hostapd_ctrl_iface_get_hw_features(hapd->iface, NULL, reply,
				reply_size);
	} else if (os_strncmp(buf, "GET_DFS_STATS", 13) == 0) {
		reply_len = hostapd_ctrl_iface_get_dfs_stats(hapd->iface, NULL, reply,
				reply_size);
	} else if (os_strncmp(buf, "GET_MESH_MODE ", sizeof("GET_MESH_MODE ")-1) == 0) {
		reply_len = hostapd_ctrl_iface_get_mesh_mode(hapd, buf + sizeof("GET_MESH_MODE ")-1,
			  reply, reply_size);
	} else if (os_strncmp(buf, "MESH_MODE ", sizeof("MESH_MODE ")-1) == 0) {
		if (hostapd_ctrl_iface_set_mesh_mode(hapd, buf + sizeof("MESH_MODE ")-1))
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_VENDOR_ELEMENTS ", sizeof("GET_VENDOR_ELEMENTS ") - 1) == 0) {
		reply_len = hostapd_ctrl_iface_get_elements(hapd,
				buf + sizeof("GET_VENDOR_ELEMENTS ") - 1, reply, reply_size,
				VENDOR_ELEM_TYPE);
	} else if (os_strncmp(buf, "VENDOR_ELEMENTS ", sizeof("VENDOR_ELEMENTS ") - 1) == 0) {
		if (hostapd_ctrl_iface_set_elements(hapd, buf + sizeof("VENDOR_ELEMENTS ") - 1, VENDOR_ELEM_TYPE))
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_AUTHRESP_ELEMENTS ", sizeof("GET_AUTHRESP_ELEMENTS ") - 1) == 0) {
		reply_len = hostapd_ctrl_iface_get_elements(hapd,
				buf + sizeof("GET_AUTHRESP_ELEMENTS ") - 1, reply, reply_size,
				AUTHRESP_ELEM_TYPE);
	} else if (os_strncmp(buf, "AUTHRESP_ELEMENTS ", sizeof("AUTHRESP_ELEMENTS ") - 1) == 0) {
		if (hostapd_ctrl_iface_set_elements(hapd, buf + sizeof("AUTHRESP_ELEMENTS ") - 1, AUTHRESP_ELEM_TYPE))
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_ASSOCRESP_ELEMENTS ", sizeof("GET_ASSOCRESP_ELEMENTS ") - 1) == 0) {
		reply_len = hostapd_ctrl_iface_get_elements(hapd,
				buf + sizeof("GET_ASSOCRESP_ELEMENTS ") - 1, reply, reply_size,
				ASSOCRESP_ELEM_TYPE);
	} else if (os_strncmp(buf, "ASSOCRESP_ELEMENTS ", sizeof("ASSOCRESP_ELEMENTS ") - 1) == 0) {
		if (hostapd_ctrl_iface_set_elements(hapd, buf + sizeof("ASSOCRESP_ELEMENTS ") - 1, ASSOCRESP_ELEM_TYPE))
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_LAST_ASSOC_REQ ", sizeof("GET_LAST_ASSOC_REQ ") - 1) == 0) {
		reply_len = hostapd_ctrl_iface_get_last_assoc_req(hapd,
				buf + sizeof("GET_LAST_ASSOC_REQ ") - 1, reply, reply_size);
	} else if (os_strncmp(buf, "GET_HE_PHY_CHANNEL_WIDTH_SET ", sizeof("GET_HE_PHY_CHANNEL_WIDTH_SET ") - 1) == 0) {
		reply_len = hostapd_ctrl_iface_get_he_phy_channel_width_set(hapd,
				buf + sizeof("GET_HE_PHY_CHANNEL_WIDTH_SET ") - 1, reply, reply_size);
	} else if (os_strncmp(buf, "GET_STA_HE_CAPS ", sizeof("GET_STA_HE_CAPS ") - 1) == 0) {
		reply_len = hostapd_ctrl_iface_get_sta_he_caps(hapd,
				buf + sizeof("GET_STA_HE_CAPS ") - 1, reply, reply_size);
	} else if (os_strncmp(buf, "SEND_ACTION_FRAME ", sizeof("SEND_ACTION_FRAME ") - 1) == 0) {
		if (hostapd_ctrl_iface_send_action_frame(hapd, buf + sizeof("SEND_ACTION_FRAME ") - 1))
			reply_len = -1;
#ifdef CONFIG_DPP
	} else if (os_strncmp(buf, "DPP_QR_CODE ", 12) == 0) {
		res = hostapd_dpp_qr_code(hapd, buf + 12);
		if (res < 0) {
			reply_len = -1;
		} else {
			reply_len = os_snprintf(reply, reply_size, "%d", res);
			if (os_snprintf_error(reply_size, reply_len))
				reply_len = -1;
		}
	} else if (os_strncmp(buf, "DPP_NFC_URI ", 12) == 0) {
		res = hostapd_dpp_nfc_uri(hapd, buf + 12);
		if (res < 0) {
			reply_len = -1;
		} else {
			reply_len = os_snprintf(reply, reply_size, "%d", res);
			if (os_snprintf_error(reply_size, reply_len))
				reply_len = -1;
		}
	} else if (os_strncmp(buf, "DPP_NFC_HANDOVER_REQ ", 21) == 0) {
		res = hostapd_dpp_nfc_handover_req(hapd, buf + 20);
		if (res < 0) {
			reply_len = -1;
		} else {
			reply_len = os_snprintf(reply, reply_size, "%d", res);
			if (os_snprintf_error(reply_size, reply_len))
				reply_len = -1;
		}
	} else if (os_strncmp(buf, "DPP_NFC_HANDOVER_SEL ", 21) == 0) {
		res = hostapd_dpp_nfc_handover_sel(hapd, buf + 20);
		if (res < 0) {
			reply_len = -1;
		} else {
			reply_len = os_snprintf(reply, reply_size, "%d", res);
			if (os_snprintf_error(reply_size, reply_len))
				reply_len = -1;
		}
	} else if (os_strncmp(buf, "DPP_BOOTSTRAP_GEN ", 18) == 0) {
		res = dpp_bootstrap_gen(hapd->iface->interfaces->dpp, buf + 18);
		if (res < 0) {
			reply_len = -1;
		} else {
			reply_len = os_snprintf(reply, reply_size, "%d", res);
			if (os_snprintf_error(reply_size, reply_len))
				reply_len = -1;
		}
	} else if (os_strncmp(buf, "DPP_BOOTSTRAP_REMOVE ", 21) == 0) {
		if (dpp_bootstrap_remove(hapd->iface->interfaces->dpp,
					 buf + 21) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "DPP_BOOTSTRAP_GET_URI ", 22) == 0) {
		const char *uri;
		uri = dpp_bootstrap_get_uri(hapd->iface->interfaces->dpp,
					    atoi(buf + 22));
		if (!uri) {
			reply_len = -1;
		} else {
			reply_len = os_snprintf(reply, reply_size, "%s", uri);
			if (os_snprintf_error(reply_size, reply_len))
				reply_len = -1;
		}
	} else if (os_strncmp(buf, "DPP_BOOTSTRAP_INFO ", 19) == 0) {
		reply_len = dpp_bootstrap_info(hapd->iface->interfaces->dpp,
					       atoi(buf + 19),
			reply, reply_size);
	} else if (os_strncmp(buf, "DPP_BOOTSTRAP_SET ", 18) == 0) {
		if (dpp_bootstrap_set(hapd->iface->interfaces->dpp,
				      atoi(buf + 18),
				      os_strchr(buf + 18, ' ')) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "DPP_AUTH_INIT ", 14) == 0) {
		if (hostapd_dpp_auth_init(hapd, buf + 13) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "DPP_LISTEN ", 11) == 0) {
		if (hostapd_dpp_listen(hapd, buf + 11) < 0)
			reply_len = -1;
	} else if (os_strcmp(buf, "DPP_STOP_LISTEN") == 0) {
		hostapd_dpp_stop(hapd);
		hostapd_dpp_listen_stop(hapd);
	} else if (os_strncmp(buf, "DPP_CONFIGURATOR_ADD", 20) == 0) {
		res = dpp_configurator_add(hapd->iface->interfaces->dpp,
					   buf + 20);
		if (res < 0) {
			reply_len = -1;
		} else {
			reply_len = os_snprintf(reply, reply_size, "%d", res);
			if (os_snprintf_error(reply_size, reply_len))
				reply_len = -1;
		}
	} else if (os_strncmp(buf, "DPP_CONFIGURATOR_REMOVE ", 24) == 0) {
		if (dpp_configurator_remove(hapd->iface->interfaces->dpp,
					    buf + 24) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "DPP_CONFIGURATOR_SIGN ", 22) == 0) {
		if (hostapd_dpp_configurator_sign(hapd, buf + 21) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "DPP_CONFIGURATOR_GET_KEY ", 25) == 0) {
		reply_len = dpp_configurator_get_key_id(
			hapd->iface->interfaces->dpp,
			atoi(buf + 25),
			reply, reply_size);
	} else if (os_strncmp(buf, "DPP_PKEX_ADD ", 13) == 0) {
		res = hostapd_dpp_pkex_add(hapd, buf + 12);
		if (res < 0) {
			reply_len = -1;
		} else {
			reply_len = os_snprintf(reply, reply_size, "%d", res);
			if (os_snprintf_error(reply_size, reply_len))
				reply_len = -1;
		}
	} else if (os_strncmp(buf, "DPP_PKEX_REMOVE ", 16) == 0) {
		if (hostapd_dpp_pkex_remove(hapd, buf + 16) < 0)
			reply_len = -1;
#ifdef CONFIG_DPP2
	} else if (os_strncmp(buf, "DPP_CONTROLLER_START ", 21) == 0) {
		if (hostapd_dpp_controller_start(hapd, buf + 20) < 0)
			reply_len = -1;
	} else if (os_strcmp(buf, "DPP_CONTROLLER_START") == 0) {
		if (hostapd_dpp_controller_start(hapd, NULL) < 0)
			reply_len = -1;
	} else if (os_strcmp(buf, "DPP_CONTROLLER_STOP") == 0) {
		dpp_controller_stop(hapd->iface->interfaces->dpp);
	} else if (os_strncmp(buf, "DPP_CHIRP ", 10) == 0) {
		if (hostapd_dpp_chirp(hapd, buf + 9) < 0)
			reply_len = -1;
	} else if (os_strcmp(buf, "DPP_STOP_CHIRP") == 0) {
		hostapd_dpp_chirp_stop(hapd);
	} else if (os_strncmp(buf, "DPP_ENABLE_CC ", 14) == 0) {
		if (hostapd_dpp_enable_cc(hapd, buf + 14) < 0)
			reply_len = -1;
#endif /* CONFIG_DPP2 */
#endif /* CONFIG_DPP */
#ifdef CONFIG_ALLOW_HOSTAPD
#ifdef RADIUS_SERVER
	} else if (os_strncmp(buf, "DAC_REQUEST ", 12) == 0) {
		if (radius_server_dac_request(hapd->radius_srv, buf + 12) < 0)
			reply_len = -1;
#endif /* RADIUS_SERVER */
	} else if (os_strncmp(buf, "GET_CAPABILITY ", 15) == 0) {
		reply_len = hostapd_ctrl_iface_get_capability(
			hapd, buf + 15, reply, reply_size);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strncmp(buf, "ZWDFS_ANT_SWITCH ", 17) == 0) {
		if (hostapd_ctrl_iface_set_zwdfs_antenna(hapd, buf + 17) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_ZWDFS_ANT", 13) == 0) {
		if (hostapd_ctrl_iface_get_zwdfs_antenna(hapd, reply, reply_size) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "SET_CSA_DEAUTH ", sizeof("SET_CSA_DEAUTH ") - 1) == 0) {
		if (hostapd_ctrl_iface_set_csa_deauth(hapd, buf + sizeof("SET_CSA_DEAUTH ") - 1))
			reply_len = -1;
	} else if (os_strncmp(buf, "GET_CSA_DEAUTH ", sizeof("GET_CSA_DEAUTH ") - 1) == 0) {
		reply_len = hostapd_ctrl_iface_get_csa_deauth(hapd, buf + sizeof("GET_CSA_DEAUTH ") - 1,
			reply, reply_size);
	} else if (os_strncmp(buf, "UPDATE_RNR ", 11) == 0) {
		reply_len = hostapd_ctrl_iface_update_rnr(hapd, buf + 11);
	} else if (os_strncmp(buf, "DELETE_RNR ", 11) == 0) {
		reply_len = hostapd_ctrl_iface_delete_rnr(hapd, buf + 11);
	} else if (os_strncmp(buf, "GET_MULTIBSS_ENABLE", 19) == 0) {
		reply_len = hostapd_ctrl_iface_get_multibss_enable(hapd,reply,reply_size);
	} else if (os_strncmp(buf, "IS_TRANSMITTED_BSSID", 20) == 0) {
		reply_len = hostapd_ctrl_iface_is_transmitted_bssid(hapd,reply,reply_size);
	} else if (os_strncmp(buf, "GET_UNSOLICITED_FRAME_SUPPORT", 29) == 0) {
		reply_len = hostapd_ctrl_iface_get_unsolicited_frame_support(hapd,reply,reply_size);
	} else if (os_strncmp(buf, "SET_UNSOLICITED_FRAME_SUPPORT ", 30) == 0) {
		reply_len = hostapd_ctrl_iface_set_unsolicited_frame_support(hapd,buf + 30);
	} else if (os_strncmp(buf, "GET_UNSOLICITED_FRAME_DURATION", 30) == 0) {
		reply_len = hostapd_ctrl_iface_get_unsolicited_frame_duration(hapd,reply,reply_size);
	} else if (os_strncmp(buf, "SET_UNSOLICITED_FRAME_DURATION ", 31) == 0) {
		reply_len = hostapd_ctrl_iface_set_unsolicited_frame_duration(hapd,buf + 31);
	} else if (os_strncmp(buf, "GET_MAX_TX_POWER", 16) == 0) {
		reply_len = hostapd_ctrl_iface_get_max_tx_power(hapd,reply,reply_size);
#ifdef CONFIG_ACS
	} else if (os_strncmp(buf, "RESET_ACS_STATE", 15) == 0) {
		hostapd_ctrl_iface_reset_acs_state(hapd, buf + 15);
#endif /* CONFIG_ACS */
#ifdef CONFIG_DEBUG_CTRL_IFACE
	} else if (os_strncmp(buf, "INJECT_DEBUG_HOSTAP_EVENT ", 26) == 0) {
		hostapd_ctrl_iface_inject_debug_hostap_event(hapd, buf + 26);
#endif /* CONFIG_DEBUG_CTRL_IFACE */
	} else {
		os_memcpy(reply, "UNKNOWN COMMAND\n", 16);
		reply_len = 16;
	}

	if (reply_len < 0) {
		os_memcpy(reply, "FAIL\n", 5);
		reply_len = 5;
	}

	return reply_len;
}



static void hostapd_ctrl_iface_receive(int sock, void *eloop_ctx,
				       void *sock_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	char buf[4096];
	int res;
	struct sockaddr_storage from;
	socklen_t fromlen = sizeof(from);
	char *reply, *pos = buf;
	const int reply_size = 8192;
	int reply_len;
	int level = MSG_DEBUG;
	UNUSED_VAR(level);
#ifdef CONFIG_CTRL_IFACE_UDP
	unsigned char lcookie[CTRL_IFACE_COOKIE_LEN];
#endif /* CONFIG_CTRL_IFACE_UDP */

	res = recvfrom(sock, buf, sizeof(buf) - 1, 0,
		       (struct sockaddr *) &from, &fromlen);
	if (res < 0) {
		wpa_printf(MSG_ERROR, "recvfrom(ctrl_iface): %s",
			   strerror(errno));
		return;
	}
	buf[res] = '\0';

	reply = os_malloc(reply_size);
	if (reply == NULL) {
		if (sendto(sock, "FAIL\n", 5, 0, (struct sockaddr *) &from,
			   fromlen) < 0) {
			wpa_printf(MSG_DEBUG, "CTRL: sendto failed: %s",
				   strerror(errno));
		}
		return;
	}

#ifdef CONFIG_CTRL_IFACE_UDP
#ifdef CONFIG_ALLOW_HOSTAPD
	if (os_strcmp(buf, "GET_COOKIE") == 0) {
		os_memcpy(reply, "COOKIE=", 7);
		wpa_snprintf_hex(reply + 7, 2 * CTRL_IFACE_COOKIE_LEN + 1,
				 hapd->ctrl_iface_cookie,
				 CTRL_IFACE_COOKIE_LEN);
		reply_len = 7 + 2 * CTRL_IFACE_COOKIE_LEN;
		goto done;
	}

	if (os_strncmp(buf, "COOKIE=", 7) != 0 ||
	    hexstr2bin(buf + 7, lcookie, CTRL_IFACE_COOKIE_LEN) < 0) {
		wpa_printf(MSG_DEBUG,
			   "CTRL: No cookie in the request - drop request");
		os_free(reply);
		return;
	}

	if (os_memcmp(hapd->ctrl_iface_cookie, lcookie,
		      CTRL_IFACE_COOKIE_LEN) != 0) {
		wpa_printf(MSG_DEBUG,
			   "CTRL: Invalid cookie in the request - drop request");
		os_free(reply);
		return;
	}

	pos = buf + 7 + 2 * CTRL_IFACE_COOKIE_LEN;
	while (*pos == ' ')
		pos++;
#endif /* CONFIG_ALLOW_HOSTAPD */
#endif /* CONFIG_CTRL_IFACE_UDP */

	if (os_strcmp(pos, "PING") == 0)
		level = MSG_EXCESSIVE;
	wpa_hexdump_ascii(level, "RX ctrl_iface", pos, res);

	reply_len = hostapd_ctrl_iface_receive_process(hapd, pos,
						       reply, reply_size,
						       &from, fromlen);

#ifdef CONFIG_CTRL_IFACE_UDP
done:
#endif /* CONFIG_CTRL_IFACE_UDP */
	if (sendto(sock, reply, reply_len, 0, (struct sockaddr *) &from,
		   fromlen) < 0) {
		wpa_printf(MSG_DEBUG, "CTRL: sendto failed: %s",
			   strerror(errno));
	}
	os_free(reply);
}


#ifndef CONFIG_CTRL_IFACE_UDP
static char * hostapd_ctrl_iface_path(struct hostapd_data *hapd)
{
	char *buf;
	size_t len;

	if (hapd->conf->ctrl_interface == NULL)
		return NULL;

	len = os_strlen(hapd->conf->ctrl_interface) +
		os_strlen(hapd->conf->iface) + 2;
	buf = os_malloc(len);
	if (buf == NULL)
		return NULL;

	os_snprintf(buf, len, "%s/%s",
		    hapd->conf->ctrl_interface, hapd->conf->iface);
	buf[len - 1] = '\0';
	return buf;
}
#endif /* CONFIG_CTRL_IFACE_UDP */


static void hostapd_ctrl_iface_msg_cb(void *ctx, int level,
				      enum wpa_msg_type type,
				      const char *txt, size_t len)
{
	struct hostapd_data *hapd = ctx;
	char *buf, *pos;
	size_t total, ifacelen, bufsize;
	errno_t err;

	if ((hapd == NULL) || (hapd->iface->bss[0] == NULL))
		return;

	pos = os_strchr(txt, ' ');
	if (pos == NULL)
		total = len;
	else {
		total = pos - txt;
		pos++;
	}

	bufsize = len + sizeof(hapd->conf->iface);
	buf = os_malloc(bufsize);
	if (buf == NULL)
		return;

	ifacelen = strnlen_s(hapd->conf->iface, sizeof(hapd->conf->iface));
	if (!IS_VALID_RANGE(ifacelen, AP_MIN_VALID_CHAR, sizeof(hapd->conf->iface) - 1)) {
		wpa_printf(MSG_ERROR, "hostapd_ctrl_iface_msg_cb - wrong len");
		os_free(buf);
		return;
	}
	err = memcpy_s(buf, bufsize, txt, total);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		os_free(buf);
		return;
	}
	buf[total] = ' ';
	total++;
	err = memcpy_s(buf + total, bufsize - total, hapd->conf->iface, ifacelen);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		os_free(buf);
		return;
	}
	total += ifacelen;
	if (pos) {
		buf[total] = ' ';
		total++;
		err = memcpy_s(buf + total, bufsize - total, pos, len - (pos - txt));
		if (EOK != err) {
			wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
			os_free(buf);
			return;
		}
	}

	/* send all events to the first VAP */
	hapd = hapd->iface->bss[0];

	hostapd_ctrl_iface_send(hapd, level, type, buf, len + ifacelen + 1);

	os_free(buf);
}


int hostapd_ctrl_iface_init(struct hostapd_data *hapd)
{
#ifdef CONFIG_CTRL_IFACE_UDP
	int port = HOSTAPD_CTRL_IFACE_PORT;
	char p[32] = { 0 };
	char port_str[40], *tmp;
	char *pos;
	struct addrinfo hints = { 0 }, *res, *saveres;
	int n;

	if (hapd->ctrl_sock > -1) {
		wpa_printf(MSG_DEBUG, "ctrl_iface already exists!");
		return 0;
	}

	if (hapd->conf->ctrl_interface == NULL)
		return 0;

	pos = os_strstr(hapd->conf->ctrl_interface, "udp:");
	if (pos) {
		pos += 4;
		port = atoi(pos);
		if (port <= 0) {
			wpa_printf(MSG_ERROR, "Invalid ctrl_iface UDP port");
			goto fail;
		}
	}

	dl_list_init(&hapd->ctrl_dst);
	hapd->ctrl_sock = -1;
	os_get_random(hapd->ctrl_iface_cookie, CTRL_IFACE_COOKIE_LEN);

#ifdef CONFIG_CTRL_IFACE_UDP_REMOTE
	hints.ai_flags = AI_PASSIVE;
#endif /* CONFIG_CTRL_IFACE_UDP_REMOTE */

#ifdef CONFIG_CTRL_IFACE_UDP_IPV6
	hints.ai_family = AF_INET6;
#else /* CONFIG_CTRL_IFACE_UDP_IPV6 */
	hints.ai_family = AF_INET;
#endif /* CONFIG_CTRL_IFACE_UDP_IPV6 */
	hints.ai_socktype = SOCK_DGRAM;

try_again:
	os_snprintf(p, sizeof(p), "%d", port);
	n = getaddrinfo(NULL, p, &hints, &res);
	if (n) {
		wpa_printf(MSG_ERROR, "getaddrinfo(): %s", gai_strerror(n));
		goto fail;
	}

	saveres = res;
	hapd->ctrl_sock = socket(res->ai_family, res->ai_socktype,
				 res->ai_protocol);
	if (hapd->ctrl_sock < 0) {
		wpa_printf(MSG_ERROR, "socket(PF_INET): %s", strerror(errno));
		goto fail;
	}

	if (bind(hapd->ctrl_sock, res->ai_addr, res->ai_addrlen) < 0) {
		port--;
		if ((HOSTAPD_CTRL_IFACE_PORT - port) <
		    HOSTAPD_CTRL_IFACE_PORT_LIMIT && !pos)
			goto try_again;
		wpa_printf(MSG_ERROR, "bind(AF_INET): %s", strerror(errno));
		goto fail;
	}

	freeaddrinfo(saveres);

	os_snprintf(port_str, sizeof(port_str), "udp:%d", port);
	tmp = os_strdup(port_str);
	if (tmp) {
		os_free(hapd->conf->ctrl_interface);
		hapd->conf->ctrl_interface = tmp;
	}
	wpa_printf(MSG_DEBUG, "ctrl_iface_init UDP port: %d", port);

	if (eloop_register_read_sock(hapd->ctrl_sock,
				     hostapd_ctrl_iface_receive, hapd, NULL) <
	    0) {
		hostapd_ctrl_iface_deinit(hapd);
		return -1;
	}

	hapd->msg_ctx = hapd;
	wpa_msg_register_cb(hostapd_ctrl_iface_msg_cb);

	return 0;

fail:
	if (hapd->ctrl_sock >= 0)
		close(hapd->ctrl_sock);
	return -1;
#else /* CONFIG_CTRL_IFACE_UDP */
	struct sockaddr_un addr;
	int s = -1;
	char *fname = NULL;

	if (hapd->ctrl_sock > -1) {
		wpa_printf(MSG_DEBUG, "ctrl_iface already exists!");
		return 0;
	}

	dl_list_init(&hapd->ctrl_dst);

	if (hapd->conf->ctrl_interface == NULL)
		return 0;

	if (mkdir(hapd->conf->ctrl_interface, S_IRWXU | S_IRWXG) < 0) {
		if (errno == EEXIST) {
			wpa_printf(MSG_DEBUG, "Using existing control "
				   "interface directory.");
		} else {
			wpa_printf(MSG_ERROR, "mkdir[ctrl_interface]: %s",
				   strerror(errno));
			goto fail;
		}
	}

	if (hapd->conf->ctrl_interface_gid_set &&
	    lchown(hapd->conf->ctrl_interface, -1,
		  hapd->conf->ctrl_interface_gid) < 0) {
		wpa_printf(MSG_ERROR, "lchown[ctrl_interface]: %s",
			   strerror(errno));
		return -1;
	}

	if (!hapd->conf->ctrl_interface_gid_set &&
	    hapd->iface->interfaces->ctrl_iface_group &&
	    lchown(hapd->conf->ctrl_interface, -1,
		  hapd->iface->interfaces->ctrl_iface_group) < 0) {
		wpa_printf(MSG_ERROR, "lchown[ctrl_interface]: %s",
			   strerror(errno));
		return -1;
	}

#ifdef ANDROID
	/*
	 * Android is using umask 0077 which would leave the control interface
	 * directory without group access. This breaks things since Wi-Fi
	 * framework assumes that this directory can be accessed by other
	 * applications in the wifi group. Fix this by adding group access even
	 * if umask value would prevent this.
	 */
	if (chmod(hapd->conf->ctrl_interface, S_IRWXU | S_IRWXG) < 0) {
		wpa_printf(MSG_ERROR, "CTRL: Could not chmod directory: %s",
			   strerror(errno));
		/* Try to continue anyway */
	}
#endif /* ANDROID */

	if (os_strlen(hapd->conf->ctrl_interface) + 1 +
	    os_strlen(hapd->conf->iface) >= sizeof(addr.sun_path))
		goto fail;

	s = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (s < 0) {
		wpa_printf(MSG_ERROR, "socket(PF_UNIX): %s", strerror(errno));
		goto fail;
	}

	os_memset(&addr, 0, sizeof(addr));
#ifdef __FreeBSD__
	addr.sun_len = sizeof(addr);
#endif /* __FreeBSD__ */
	addr.sun_family = AF_UNIX;
	fname = hostapd_ctrl_iface_path(hapd);
	if (fname == NULL)
		goto fail;
	os_strlcpy(addr.sun_path, fname, sizeof(addr.sun_path));
	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		wpa_printf(MSG_DEBUG, "ctrl_iface bind(PF_UNIX) failed: %s",
			   strerror(errno));
		if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			wpa_printf(MSG_DEBUG, "ctrl_iface exists, but does not"
				   " allow connections - assuming it was left"
				   "over from forced program termination");
			if (unlink(fname) < 0) {
				wpa_printf(MSG_ERROR,
					   "Could not unlink existing ctrl_iface socket '%s': %s",
					   fname, strerror(errno));
				goto fail;
			}
			if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) <
			    0) {
				wpa_printf(MSG_ERROR,
					   "hostapd-ctrl-iface: bind(PF_UNIX): %s",
					   strerror(errno));
				goto fail;
			}
			wpa_printf(MSG_DEBUG, "Successfully replaced leftover "
				   "ctrl_iface socket '%s'", fname);
		} else {
			wpa_printf(MSG_INFO, "ctrl_iface exists and seems to "
				   "be in use - cannot override it");
			wpa_printf(MSG_INFO, "Delete '%s' manually if it is "
				   "not used anymore", fname);
			os_free(fname);
			fname = NULL;
			goto fail;
		}
	}

	if (hapd->conf->ctrl_interface_gid_set &&
	    lchown(fname, -1, hapd->conf->ctrl_interface_gid) < 0) {
		wpa_printf(MSG_ERROR, "lchown[ctrl_interface/ifname]: %s",
			   strerror(errno));
		goto fail;
	}

	if (!hapd->conf->ctrl_interface_gid_set &&
	    hapd->iface->interfaces->ctrl_iface_group &&
	    lchown(fname, -1, hapd->iface->interfaces->ctrl_iface_group) < 0) {
		wpa_printf(MSG_ERROR, "lchown[ctrl_interface/ifname]: %s",
			   strerror(errno));
		goto fail;
	}

	if (chmod(fname, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) < 0) {
		wpa_printf(MSG_ERROR, "chmod[ctrl_interface/ifname]: %s",
			   strerror(errno));
		goto fail;
	}
	os_free(fname);

	hapd->ctrl_sock = s;
	if (eloop_register_read_sock(s, hostapd_ctrl_iface_receive, hapd,
				     NULL) < 0) {
		hostapd_ctrl_iface_deinit(hapd);
		return -1;
	}
	hapd->msg_ctx = hapd;
	wpa_msg_register_cb(hostapd_ctrl_iface_msg_cb);

	return 0;

fail:
	if (s >= 0)
		close(s);
	if (fname) {
		unlink(fname);
		os_free(fname);
	}
	return -1;
#endif /* CONFIG_CTRL_IFACE_UDP */
}


void hostapd_ctrl_iface_deinit(struct hostapd_data *hapd)
{
	struct wpa_ctrl_dst *dst, *prev;

  wpa_msg(hapd->msg_ctx, MSG_INFO, HOSTAPD_DISCONNECTED);

	if (hapd->ctrl_sock > -1) {
#ifndef CONFIG_CTRL_IFACE_UDP
		char *fname;
#endif /* !CONFIG_CTRL_IFACE_UDP */

		eloop_unregister_read_sock(hapd->ctrl_sock);
		close(hapd->ctrl_sock);
		hapd->ctrl_sock = -1;
#ifndef CONFIG_CTRL_IFACE_UDP
		fname = hostapd_ctrl_iface_path(hapd);
		if (fname)
			unlink(fname);
		os_free(fname);

		if (hapd->conf->ctrl_interface &&
		    rmdir(hapd->conf->ctrl_interface) < 0) {
			if (errno == ENOTEMPTY) {
				wpa_printf(MSG_DEBUG, "Control interface "
					   "directory not empty - leaving it "
					   "behind");
			} else {
				wpa_printf(MSG_ERROR,
					   "rmdir[ctrl_interface=%s]: %s",
					   hapd->conf->ctrl_interface,
					   strerror(errno));
			}
		}
#endif /* !CONFIG_CTRL_IFACE_UDP */
	}

	dl_list_for_each_safe(dst, prev, &hapd->ctrl_dst, struct wpa_ctrl_dst,
			      list)
		os_free(dst);

#ifdef CONFIG_TESTING_OPTIONS
	l2_packet_deinit(hapd->l2_test);
	hapd->l2_test = NULL;
#endif /* CONFIG_TESTING_OPTIONS */
}

static int hostapd_ctrl_iface_add(struct hapd_interfaces *interfaces,
				  char *buf)
{
	if (hostapd_add_iface(interfaces, buf) < 0) {
		wpa_printf(MSG_ERROR, "Adding interface %s failed", buf);
		return -1;
	}
	return 0;
}


static int hostapd_ctrl_iface_remove(struct hapd_interfaces *interfaces,
				     char *buf)
{
	if (hostapd_remove_iface(interfaces, buf) < 0) {
		wpa_printf(MSG_ERROR, "Removing interface %s failed", buf);
		return -1;
	}
	return 0;
}

static int hostapd_global_ctrl_iface_attach(struct hapd_interfaces *interfaces,
					    struct sockaddr_storage *from,
					    socklen_t fromlen, char *input)
{
	return ctrl_iface_attach(&interfaces->global_ctrl_dst, from, fromlen,
				 input);
}


static int hostapd_global_ctrl_iface_detach(struct hapd_interfaces *interfaces,
					    struct sockaddr_storage *from,
					    socklen_t fromlen)
{
	return ctrl_iface_detach(&interfaces->global_ctrl_dst, from, fromlen);
}

#ifdef CONFIG_ALLOW_HOSTAPD
static void hostapd_ctrl_iface_flush(struct hapd_interfaces *interfaces)
{
#ifdef CONFIG_WPS_TESTING
	wps_version_number = 0x20;
	wps_testing_dummy_cred = 0;
	wps_corrupt_pkhash = 0;
#endif /* CONFIG_WPS_TESTING */

#ifdef CONFIG_TESTING_OPTIONS
#ifdef CONFIG_DPP
	dpp_test = DPP_TEST_DISABLED;
#ifdef CONFIG_DPP2
	dpp_version_override = 2;
#else /* CONFIG_DPP2 */
	dpp_version_override = 1;
#endif /* CONFIG_DPP2 */
#endif /* CONFIG_DPP */
#endif /* CONFIG_TESTING_OPTIONS */

#ifdef CONFIG_DPP
	dpp_global_clear(interfaces->dpp);
#endif /* CONFIG_DPP */
}
#endif /* CONFIG_ALLOW_HOSTAPD */

#ifdef CONFIG_FST

static int
hostapd_global_ctrl_iface_fst_attach(struct hapd_interfaces *interfaces,
				     const char *cmd)
{
	char ifname[IFNAMSIZ + 1];
	struct fst_iface_cfg cfg;
	struct hostapd_data *hapd;
	struct fst_wpa_obj iface_obj;

	if (!fst_parse_attach_command(cmd, ifname, sizeof(ifname), &cfg)) {
		hapd = hostapd_get_iface(interfaces, ifname);
		if (hapd) {
			if (hapd->iface->fst) {
				wpa_printf(MSG_INFO, "FST: Already attached");
				return -1;
			}
			fst_hostapd_fill_iface_obj(hapd, &iface_obj);
			hapd->iface->fst = fst_attach(ifname, hapd->own_addr,
						      &iface_obj, &cfg);
			if (hapd->iface->fst)
				return 0;
		}
	}

	return -EINVAL;
}


static int
hostapd_global_ctrl_iface_fst_detach(struct hapd_interfaces *interfaces,
				     const char *cmd)
{
	char ifname[IFNAMSIZ + 1];
	struct hostapd_data * hapd;

	if (!fst_parse_detach_command(cmd, ifname, sizeof(ifname))) {
		hapd = hostapd_get_iface(interfaces, ifname);
		if (hapd) {
			if (!fst_iface_detach(ifname)) {
				hapd->iface->fst = NULL;
				hapd->iface->fst_ies = NULL;
				return 0;
			}
		}
	}

	return -EINVAL;
}

#endif /* CONFIG_FST */

#ifdef CONFIG_ALLOW_HOSTAPD
static struct hostapd_data *
hostapd_interfaces_get_hapd(struct hapd_interfaces *interfaces,
			    const char *ifname)
{
	size_t i, j;

	for (i = 0; i < interfaces->count; i++) {
		struct hostapd_iface *iface = interfaces->iface[i];

		for (j = 0; j < iface->num_bss; j++) {
			struct hostapd_data *hapd;

			hapd = iface->bss[j];
			if (os_strcmp(ifname, hapd->conf->iface) == 0)
				return hapd;
		}
	}

	return NULL;
}


static int hostapd_ctrl_iface_dup_param(struct hostapd_data *src_hapd,
					struct hostapd_data *dst_hapd,
					const char *param)
{
	int res;
	char *value;

	value = os_zalloc(HOSTAPD_CLI_DUP_VALUE_MAX_LEN);
	if (!value) {
		wpa_printf(MSG_ERROR,
			   "DUP: cannot allocate buffer to stringify %s",
			   param);
		goto error_return;
	}

	if (os_strcmp(param, "wpa") == 0) {
		os_snprintf(value, HOSTAPD_CLI_DUP_VALUE_MAX_LEN, "%d",
			    src_hapd->conf->wpa);
	} else if (os_strcmp(param, "wpa_key_mgmt") == 0 &&
		   src_hapd->conf->wpa_key_mgmt) {
		res = hostapd_ctrl_iface_get_key_mgmt(
			src_hapd, value, HOSTAPD_CLI_DUP_VALUE_MAX_LEN);
		if (os_snprintf_error(HOSTAPD_CLI_DUP_VALUE_MAX_LEN, res))
			goto error_stringify;
	} else if (os_strcmp(param, "wpa_pairwise") == 0 &&
		   src_hapd->conf->wpa_pairwise) {
		res = wpa_write_ciphers(value,
					value + HOSTAPD_CLI_DUP_VALUE_MAX_LEN,
					src_hapd->conf->wpa_pairwise, " ");
		if (res < 0)
			goto error_stringify;
	} else if (os_strcmp(param, "rsn_pairwise") == 0 &&
		   src_hapd->conf->rsn_pairwise) {
		res = wpa_write_ciphers(value,
					value + HOSTAPD_CLI_DUP_VALUE_MAX_LEN,
					src_hapd->conf->rsn_pairwise, " ");
		if (res < 0)
			goto error_stringify;
	} else if (os_strcmp(param, "wpa_passphrase") == 0 &&
		   src_hapd->conf->ssid.wpa_passphrase) {
		os_snprintf(value, HOSTAPD_CLI_DUP_VALUE_MAX_LEN, "%s",
			    src_hapd->conf->ssid.wpa_passphrase);
	} else if (os_strcmp(param, "wpa_psk") == 0 &&
		   src_hapd->conf->ssid.wpa_psk_set) {
		wpa_snprintf_hex(value, HOSTAPD_CLI_DUP_VALUE_MAX_LEN,
			src_hapd->conf->ssid.wpa_psk->psk, PMK_LEN);
	} else {
		wpa_printf(MSG_WARNING, "DUP: %s cannot be duplicated", param);
		goto error_return;
	}

	res = hostapd_set_iface(dst_hapd->iconf, dst_hapd->conf, param, value);
	os_free(value);
	return res;

error_stringify:
	wpa_printf(MSG_ERROR, "DUP: cannot stringify %s", param);
error_return:
	os_free(value);
	return -1;
}
#endif /* CONFIG_ALLOW_HOSTAPD */


static int
hostapd_global_ctrl_iface_interfaces(struct hapd_interfaces *interfaces,
				     const char *input,
				     char *reply, int reply_size)
{
	size_t i, j;
	int res;
	char *pos, *end;
	struct hostapd_iface *iface;
	int show_ctrl = 0;

	if (input)
		show_ctrl = !!os_strstr(input, "ctrl");

	pos = reply;
	end = reply + reply_size;

	for (i = 0; i < interfaces->count; i++) {
		iface = interfaces->iface[i];

		for (j = 0; j < iface->num_bss; j++) {
			struct hostapd_bss_config *conf;

			conf = iface->conf->bss[j];
			if (show_ctrl)
				res = os_snprintf(pos, end - pos,
						  "%s ctrl_iface=%s\n",
						  conf->iface,
						  conf->ctrl_interface ?
						  conf->ctrl_interface : "N/A");
			else
				res = os_snprintf(pos, end - pos, "%s\n",
						  conf->iface);
			if (os_snprintf_error(end - pos, res)) {
				*pos = '\0';
				return pos - reply;
			}
			pos += res;
		}
	}

	return pos - reply;
}

#ifdef CONFIG_ALLOW_HOSTAPD
static int
hostapd_global_ctrl_iface_dup_network(struct hapd_interfaces *interfaces,
				      char *cmd)
{
	char *p_start = cmd, *p_end;
	struct hostapd_data *src_hapd, *dst_hapd;

	/* cmd: "<src ifname> <dst ifname> <variable name> */

	p_end = os_strchr(p_start, ' ');
	if (!p_end) {
		wpa_printf(MSG_ERROR, "DUP: no src ifname found in cmd: '%s'",
			   cmd);
		return -1;
	}

	*p_end = '\0';
	src_hapd = hostapd_interfaces_get_hapd(interfaces, p_start);
	if (!src_hapd) {
		wpa_printf(MSG_ERROR, "DUP: no src ifname found: '%s'",
			   p_start);
		return -1;
	}

	p_start = p_end + 1;
	p_end = os_strchr(p_start, ' ');
	if (!p_end) {
		wpa_printf(MSG_ERROR, "DUP: no dst ifname found in cmd: '%s'",
			   cmd);
		return -1;
	}

	*p_end = '\0';
	dst_hapd = hostapd_interfaces_get_hapd(interfaces, p_start);
	if (!dst_hapd) {
		wpa_printf(MSG_ERROR, "DUP: no dst ifname found: '%s'",
			   p_start);
		return -1;
	}

	p_start = p_end + 1;
	return hostapd_ctrl_iface_dup_param(src_hapd, dst_hapd, p_start);
}


static int hostapd_global_ctrl_iface_ifname(struct hapd_interfaces *interfaces,
					    const char *ifname,
					    char *buf, char *reply,
					    int reply_size,
					    struct sockaddr_storage *from,
					    socklen_t fromlen)
{
	struct hostapd_data *hapd;

	hapd = hostapd_interfaces_get_hapd(interfaces, ifname);
	if (hapd == NULL) {
		int res;

		res = os_snprintf(reply, reply_size, "FAIL-NO-IFNAME-MATCH\n");
		if (os_snprintf_error(reply_size, res))
			return -1;
		return res;
	}

	return hostapd_ctrl_iface_receive_process(hapd, buf, reply,reply_size,
						  from, fromlen);
}
#endif /* CONFIG_ALLOW_HOSTAPD */

static void hostapd_global_ctrl_iface_receive(int sock, void *eloop_ctx,
					      void *sock_ctx)
{
	struct hapd_interfaces *interfaces = eloop_ctx;
	char buffer[256], *buf = buffer;
	int res;
	struct sockaddr_storage from;
	socklen_t fromlen = sizeof(from);
	char *reply;
	int reply_len;
	const int reply_size = 4096;
#ifdef CONFIG_CTRL_IFACE_UDP
	unsigned char lcookie[CTRL_IFACE_COOKIE_LEN];
#endif /* CONFIG_CTRL_IFACE_UDP */

	res = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
		       (struct sockaddr *) &from, &fromlen);
	if (res < 0) {
		wpa_printf(MSG_ERROR, "recvfrom(ctrl_iface): %s",
			   strerror(errno));
		return;
	}
	buf[res] = '\0';
	wpa_printf(MSG_DEBUG, "Global ctrl_iface command: %s", buf);

	reply = os_malloc(reply_size);
	if (reply == NULL) {
		if (sendto(sock, "FAIL\n", 5, 0, (struct sockaddr *) &from,
			   fromlen) < 0) {
			wpa_printf(MSG_DEBUG, "CTRL: sendto failed: %s",
				   strerror(errno));
		}
		return;
	}

	os_memcpy(reply, "OK\n", 3);
	reply_len = 3;

#ifdef CONFIG_ALLOW_HOSTAPD
#ifdef CONFIG_CTRL_IFACE_UDP
	if (os_strcmp(buf, "GET_COOKIE") == 0) {
		os_memcpy(reply, "COOKIE=", 7);
		wpa_snprintf_hex(reply + 7, 2 * CTRL_IFACE_COOKIE_LEN + 1,
				 interfaces->ctrl_iface_cookie,
				 CTRL_IFACE_COOKIE_LEN);
		reply_len = 7 + 2 * CTRL_IFACE_COOKIE_LEN;
		goto send_reply;
	}

	if (os_strncmp(buf, "COOKIE=", 7) != 0 ||
	    hexstr2bin(buf + 7, lcookie, CTRL_IFACE_COOKIE_LEN) < 0) {
		wpa_printf(MSG_DEBUG,
			   "CTRL: No cookie in the request - drop request");
		os_free(reply);
		return;
	}

	if (os_memcmp(interfaces->ctrl_iface_cookie, lcookie,
		      CTRL_IFACE_COOKIE_LEN) != 0) {
		wpa_printf(MSG_DEBUG,
			   "CTRL: Invalid cookie in the request - drop request");
		os_free(reply);
		return;
	}

	buf += 7 + 2 * CTRL_IFACE_COOKIE_LEN;
	while (*buf == ' ')
		buf++;
#endif /* CONFIG_CTRL_IFACE_UDP */

	if (os_strncmp(buf, "IFNAME=", 7) == 0) {
		char *pos = os_strchr(buf + 7, ' ');

		if (pos) {
			*pos++ = '\0';
			reply_len = hostapd_global_ctrl_iface_ifname(
				interfaces, buf + 7, pos, reply, reply_size,
				&from, fromlen);
			goto send_reply;
		}
	}
#endif /* CONFIG_ALLOW_HOSTAPD */

	if (os_strcmp(buf, "PING") == 0) {
		os_memcpy(reply, "PONG\n", 5);
		reply_len = 5;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strncmp(buf, "RELOG", 5) == 0) {
		if (wpa_debug_reopen_file() < 0)
			reply_len = -1;
	} else if (os_strcmp(buf, "FLUSH") == 0) {
		hostapd_ctrl_iface_flush(interfaces);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strncmp(buf, "ADD ", 4) == 0) {
		if (hostapd_ctrl_iface_add(interfaces, buf + 4) < 0)
			reply_len = -1;
	} else if (os_strncmp(buf, "REMOVE ", 7) == 0) {
		if (hostapd_ctrl_iface_remove(interfaces, buf + 7) < 0)
			reply_len = -1;
	} else if (os_strcmp(buf, "ATTACH") == 0) {
		if (hostapd_global_ctrl_iface_attach(interfaces, &from,
						     fromlen, NULL))
			reply_len = -1;
	} else if (os_strncmp(buf, "ATTACH ", 7) == 0) {
		if (hostapd_global_ctrl_iface_attach(interfaces, &from,
						     fromlen, buf + 7))
			reply_len = -1;
	} else if (os_strcmp(buf, "DETACH") == 0) {
		if (hostapd_global_ctrl_iface_detach(interfaces, &from,
			fromlen))
			reply_len = -1;
#ifdef CONFIG_ALLOW_HOSTAPD
#ifdef CONFIG_MODULE_TESTS
	} else if (os_strcmp(buf, "MODULE_TESTS") == 0) {
		if (hapd_module_tests() < 0)
			reply_len = -1;
#endif /* CONFIG_MODULE_TESTS */
#ifdef CONFIG_FST
	} else if (os_strncmp(buf, "FST-ATTACH ", 11) == 0) {
		if (!hostapd_global_ctrl_iface_fst_attach(interfaces, buf + 11))
			reply_len = os_snprintf(reply, reply_size, "OK\n");
		else
			reply_len = -1;
	} else if (os_strncmp(buf, "FST-DETACH ", 11) == 0) {
		if (!hostapd_global_ctrl_iface_fst_detach(interfaces, buf + 11))
			reply_len = os_snprintf(reply, reply_size, "OK\n");
		else
			reply_len = -1;
	} else if (os_strncmp(buf, "FST-MANAGER ", 12) == 0) {
		reply_len = fst_ctrl_iface_receive(buf + 12, reply, reply_size);
#endif /* CONFIG_FST */
	} else if (os_strncmp(buf, "DUP_NETWORK ", 12) == 0) {
		if (!hostapd_global_ctrl_iface_dup_network(interfaces,
							   buf + 12))
			reply_len = os_snprintf(reply, reply_size, "OK\n");
		else
			reply_len = -1;
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strncmp(buf, "INTERFACES", 10) == 0) {
		reply_len = hostapd_global_ctrl_iface_interfaces(
			interfaces, buf + 10, reply, sizeof(buffer));
	} else if (os_strcmp(buf, "TERMINATE") == 0) {
		eloop_terminate();
	} else {
		wpa_printf(MSG_DEBUG, "Unrecognized global ctrl_iface command "
			   "ignored");
		reply_len = -1;
	}
#ifdef CONFIG_ALLOW_HOSTAPD
send_reply:
#endif /* CONFIG_ALLOW_HOSTAPD */
	if (reply_len < 0) {
		os_memcpy(reply, "FAIL\n", 5);
		reply_len = 5;
	}

	if (sendto(sock, reply, reply_len, 0, (struct sockaddr *) &from,
		   fromlen) < 0) {
		wpa_printf(MSG_DEBUG, "CTRL: sendto failed: %s",
			   strerror(errno));
	}
	os_free(reply);
}


#ifndef CONFIG_CTRL_IFACE_UDP
static char * hostapd_global_ctrl_iface_path(struct hapd_interfaces *interface)
{
	char *buf;
	size_t len;

	if (interface->global_iface_path == NULL)
		return NULL;

	len = os_strlen(interface->global_iface_path) +
		os_strlen(interface->global_iface_name) + 2;
	buf = os_malloc(len);
	if (buf == NULL)
		return NULL;

	os_snprintf(buf, len, "%s/%s", interface->global_iface_path,
		    interface->global_iface_name);
	buf[len - 1] = '\0';
	return buf;
}
#endif /* CONFIG_CTRL_IFACE_UDP */


int hostapd_global_ctrl_iface_init(struct hapd_interfaces *interface)
{
#ifdef CONFIG_CTRL_IFACE_UDP
	int port = HOSTAPD_GLOBAL_CTRL_IFACE_PORT;
	char p[32] = { 0 };
	char *pos;
	struct addrinfo hints = { 0 }, *res, *saveres;
	int n;

	if (interface->global_ctrl_sock > -1) {
		wpa_printf(MSG_DEBUG, "ctrl_iface already exists!");
		return 0;
	}

	if (interface->global_iface_path == NULL)
		return 0;

	pos = os_strstr(interface->global_iface_path, "udp:");
	if (pos) {
		pos += 4;
		port = atoi(pos);
		if (port <= 0) {
			wpa_printf(MSG_ERROR, "Invalid global ctrl UDP port");
			goto fail;
		}
	}

	os_get_random(interface->ctrl_iface_cookie, CTRL_IFACE_COOKIE_LEN);

#ifdef CONFIG_CTRL_IFACE_UDP_REMOTE
	hints.ai_flags = AI_PASSIVE;
#endif /* CONFIG_CTRL_IFACE_UDP_REMOTE */

#ifdef CONFIG_CTRL_IFACE_UDP_IPV6
	hints.ai_family = AF_INET6;
#else /* CONFIG_CTRL_IFACE_UDP_IPV6 */
	hints.ai_family = AF_INET;
#endif /* CONFIG_CTRL_IFACE_UDP_IPV6 */
	hints.ai_socktype = SOCK_DGRAM;

try_again:
	os_snprintf(p, sizeof(p), "%d", port);
	n = getaddrinfo(NULL, p, &hints, &res);
	if (n) {
		wpa_printf(MSG_ERROR, "getaddrinfo(): %s", gai_strerror(n));
		goto fail;
	}

	saveres = res;
	interface->global_ctrl_sock = socket(res->ai_family, res->ai_socktype,
					     res->ai_protocol);
	if (interface->global_ctrl_sock < 0) {
		wpa_printf(MSG_ERROR, "socket(PF_INET): %s", strerror(errno));
		goto fail;
	}

	if (bind(interface->global_ctrl_sock, res->ai_addr, res->ai_addrlen) <
	    0) {
		port++;
		if ((port - HOSTAPD_GLOBAL_CTRL_IFACE_PORT) <
		    HOSTAPD_GLOBAL_CTRL_IFACE_PORT_LIMIT && !pos)
			goto try_again;
		wpa_printf(MSG_ERROR, "bind(AF_INET): %s", strerror(errno));
		goto fail;
	}

	freeaddrinfo(saveres);

	wpa_printf(MSG_DEBUG, "global ctrl_iface_init UDP port: %d", port);

	if (eloop_register_read_sock(interface->global_ctrl_sock,
				     hostapd_global_ctrl_iface_receive,
				     interface, NULL) < 0) {
		hostapd_global_ctrl_iface_deinit(interface);
		return -1;
	}

	wpa_msg_register_cb(hostapd_ctrl_iface_msg_cb);

	return 0;

fail:
	if (interface->global_ctrl_sock >= 0)
		close(interface->global_ctrl_sock);
	return -1;
#else /* CONFIG_CTRL_IFACE_UDP */
	struct sockaddr_un addr;
	int s = -1;
	char *fname = NULL;

	if (interface->global_iface_path == NULL) {
		wpa_printf(MSG_DEBUG, "ctrl_iface not configured!");
		return 0;
	}

	if (mkdir(interface->global_iface_path, S_IRWXU | S_IRWXG) < 0) {
		if (errno == EEXIST) {
			wpa_printf(MSG_DEBUG, "Using existing control "
				   "interface directory.");
		} else {
			wpa_printf(MSG_ERROR, "mkdir[ctrl_interface]: %s",
				   strerror(errno));
			goto fail;
		}
	} else if (interface->ctrl_iface_group &&
		   lchown(interface->global_iface_path, -1,
			 interface->ctrl_iface_group) < 0) {
		wpa_printf(MSG_ERROR, "lchown[ctrl_interface]: %s",
			   strerror(errno));
		goto fail;
	}

	if (os_strlen(interface->global_iface_path) + 1 +
	    os_strlen(interface->global_iface_name) >= sizeof(addr.sun_path))
		goto fail;

	s = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (s < 0) {
		wpa_printf(MSG_ERROR, "socket(PF_UNIX): %s", strerror(errno));
		goto fail;
	}

	os_memset(&addr, 0, sizeof(addr));
#ifdef __FreeBSD__
	addr.sun_len = sizeof(addr);
#endif /* __FreeBSD__ */
	addr.sun_family = AF_UNIX;
	fname = hostapd_global_ctrl_iface_path(interface);
	if (fname == NULL)
		goto fail;
	os_strlcpy(addr.sun_path, fname, sizeof(addr.sun_path));
	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		wpa_printf(MSG_DEBUG, "ctrl_iface bind(PF_UNIX) failed: %s",
			   strerror(errno));
		if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			wpa_printf(MSG_DEBUG, "ctrl_iface exists, but does not"
				   " allow connections - assuming it was left"
				   "over from forced program termination");
			if (unlink(fname) < 0) {
				wpa_printf(MSG_ERROR,
					   "Could not unlink existing ctrl_iface socket '%s': %s",
					   fname, strerror(errno));
				goto fail;
			}
			if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) <
			    0) {
				wpa_printf(MSG_ERROR, "bind(PF_UNIX): %s",
					   strerror(errno));
				goto fail;
			}
			wpa_printf(MSG_DEBUG, "Successfully replaced leftover "
				   "ctrl_iface socket '%s'", fname);
		} else {
			wpa_printf(MSG_INFO, "ctrl_iface exists and seems to "
				   "be in use - cannot override it");
			wpa_printf(MSG_INFO, "Delete '%s' manually if it is "
				   "not used anymore", fname);
			os_free(fname);
			fname = NULL;
			goto fail;
		}
	}

	if (interface->ctrl_iface_group &&
	    lchown(fname, -1, interface->ctrl_iface_group) < 0) {
		wpa_printf(MSG_ERROR, "lchown[ctrl_interface]: %s",
			   strerror(errno));
		goto fail;
	}

	if (chmod(fname, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) < 0) {
		wpa_printf(MSG_ERROR, "chmod[ctrl_interface/ifname]: %s",
			   strerror(errno));
		goto fail;
	}
	os_free(fname);

	interface->global_ctrl_sock = s;
	eloop_register_read_sock(s, hostapd_global_ctrl_iface_receive,
				 interface, NULL);

	wpa_msg_register_cb(hostapd_ctrl_iface_msg_cb);

	return 0;

fail:
	if (s >= 0)
		close(s);
	if (fname) {
		unlink(fname);
		os_free(fname);
	}
	return -1;
#endif /* CONFIG_CTRL_IFACE_UDP */
}


void hostapd_global_ctrl_iface_deinit(struct hapd_interfaces *interfaces)
{
#ifndef CONFIG_CTRL_IFACE_UDP
	char *fname = NULL;
#endif /* CONFIG_CTRL_IFACE_UDP */
	struct wpa_ctrl_dst *dst, *prev;

	if (interfaces->global_ctrl_sock > -1) {
		eloop_unregister_read_sock(interfaces->global_ctrl_sock);
		close(interfaces->global_ctrl_sock);
		interfaces->global_ctrl_sock = -1;
#ifndef CONFIG_CTRL_IFACE_UDP
		fname = hostapd_global_ctrl_iface_path(interfaces);
		if (fname) {
			unlink(fname);
			os_free(fname);
		}

		if (interfaces->global_iface_path &&
		    rmdir(interfaces->global_iface_path) < 0) {
			if (errno == ENOTEMPTY) {
				wpa_printf(MSG_DEBUG, "Control interface "
					   "directory not empty - leaving it "
					   "behind");
			} else {
				wpa_printf(MSG_ERROR,
					   "rmdir[ctrl_interface=%s]: %s",
					   interfaces->global_iface_path,
					   strerror(errno));
			}
		}
#endif /* CONFIG_CTRL_IFACE_UDP */
	}

	os_free(interfaces->global_iface_path);
	interfaces->global_iface_path = NULL;

	dl_list_for_each_safe(dst, prev, &interfaces->global_ctrl_dst,
			      struct wpa_ctrl_dst, list)
		os_free(dst);
}


static int hostapd_ctrl_check_event_enabled(struct wpa_ctrl_dst *dst,
					    const char *buf)
{
	/* Enable Probe Request events based on explicit request.
	 * Other events are enabled by default.
	 */
	if (str_starts(buf, RX_PROBE_REQUEST))
		return !!(dst->events & WPA_EVENT_RX_PROBE_REQUEST);
	return 1;
}


static void hostapd_ctrl_iface_send_internal(int sock, struct dl_list *ctrl_dst,
					     const char *ifname, int level,
					     const char *buf, size_t len)
{
	struct wpa_ctrl_dst *dst, *next;
	struct msghdr msg;
	int idx, res;
	struct iovec io[5];
	char levelstr[10];

	if (sock < 0 || dl_list_empty(ctrl_dst))
		return;

	res = os_snprintf(levelstr, sizeof(levelstr), "<%d>", level);
	if (os_snprintf_error(sizeof(levelstr), res))
		return;
	idx = 0;
	if (ifname) {
		io[idx].iov_base = "IFNAME=";
		io[idx].iov_len = 7;
		idx++;
		io[idx].iov_base = (char *) ifname;
		io[idx].iov_len = os_strlen(ifname);
		idx++;
		io[idx].iov_base = " ";
		io[idx].iov_len = 1;
		idx++;
	}
	io[idx].iov_base = levelstr;
	io[idx].iov_len = os_strlen(levelstr);
	idx++;
	io[idx].iov_base = (char *) buf;
	io[idx].iov_len = len;
	idx++;
	os_memset(&msg, 0, sizeof(msg));
	msg.msg_iov = io;
	msg.msg_iovlen = idx;

	idx = 0;
	dl_list_for_each_safe(dst, next, ctrl_dst, struct wpa_ctrl_dst, list) {
		if ((level >= dst->debug_level) &&
		     hostapd_ctrl_check_event_enabled(dst, buf)) {
			sockaddr_print(MSG_DEBUG, "CTRL_IFACE monitor send",
				       &dst->addr, dst->addrlen);
			msg.msg_name = &dst->addr;
			msg.msg_namelen = dst->addrlen;
			if (sendmsg(sock, &msg, 0) < 0) {
				int _errno = errno;
				wpa_printf(MSG_INFO, "CTRL_IFACE monitor[%d]: "
					   "%d - %s",
					   idx, errno, strerror(errno));
				dst->errors++;
				if (dst->errors > 10 || _errno == ENOENT) {
					ctrl_iface_detach(ctrl_dst,
							  &dst->addr,
							  dst->addrlen);
				}
			} else
				dst->errors = 0;
		}
		idx++;
	}
}


static void hostapd_ctrl_iface_send(struct hostapd_data *hapd, int level,
				    enum wpa_msg_type type,
				    const char *buf, size_t len)
{
	if (type != WPA_MSG_NO_GLOBAL) {
		hostapd_ctrl_iface_send_internal(
			hapd->iface->interfaces->global_ctrl_sock,
			&hapd->iface->interfaces->global_ctrl_dst,
			type != WPA_MSG_PER_INTERFACE ?
			NULL : hapd->conf->iface,
			level, buf, len);
	}

	if (type != WPA_MSG_ONLY_GLOBAL) {
		hostapd_ctrl_iface_send_internal(
			hapd->ctrl_sock, &hapd->ctrl_dst,
			NULL, level, buf, len);
	}
}

#endif /* CONFIG_NATIVE_WINDOWS */
