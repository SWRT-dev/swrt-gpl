/*
 * wpactrl_util.c - CLI to hostap and wpa_supplicant
 *
 * Copyright (C) 2020 iopsys Software Solutions AB. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <net/if.h>
#include <time.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <endian.h>
#include <dirent.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <easy/easy.h>
#include "wifiutils.h"
#include "wifi.h"
#include "debug.h"
#include "wpactrl_util.h"
#include "nlwifi.h"
#ifdef LIBWIFI_USE_CTRL_IFACE
#include "limits.h"
#include <ctype.h>
#include <sys/un.h>

#ifndef CONFIG_HOSTAPD_CTRL_IFACE_DIR
#define CONFIG_HOSTAPD_CTRL_IFACE_DIR "/var/run/hostapd"
#endif /* CONFIG_HOSTAPD_CTRL_IFACE_DIR */

#ifndef CONFIG_WPA_CTRL_IFACE_DIR
#define CONFIG_WPA_CTRL_IFACE_DIR "/var/run/wpa_supplicant"
#endif /* CONFIG_WPA_CTRL_IFACE_DIR */

#ifndef CONFIG_CTRL_IFACE_CLIENT_DIR
#define CONFIG_CTRL_IFACE_CLIENT_DIR "/tmp"
#endif /* CONFIG_CTRL_IFACE_CLIENT_DIR */

#ifndef CONFIG_CTRL_IFACE_CLIENT_PREFIX
#define CONFIG_CTRL_IFACE_CLIENT_PREFIX "wpa_ctrl_"
#endif /* CONFIG_CTRL_IFACE_CLIENT_PREFIX */

#define MAX_TRIES 5
#endif /*LIBWIFI_USE_CTRL_IFACE*/
#define WPACTRL_TIMEOUT 5

/* input:
*  buf = "key1=val1\nkey2=val2\nkey3=val3"
*  param = "key2"
*  char value[x]
*  value_max = x
*  output:
*  value = "val2\0"
*/
static int get_param(const char *buf, const char *param, char *value, size_t value_max)
{
	const char *line, *loc;
	char *p, *origin_p;
	char search_str[256] = { 0 };
	int ret = -1;

	if (WARN_ON(!buf) || WARN_ON(!param) || WARN_ON(!value) || WARN_ON(value_max == 0))
		return -EINVAL;

	memset(value,0x00,value_max);
	p = strdup(buf);
	origin_p = p;

	while ((line = strsep(&p, "\n"))) {
		loc = strstr(line, param);
		if (!loc || loc != line)
			continue;
		snprintf(search_str, sizeof(search_str), "%64s=%%%zu[^\n]s", param, value_max);

		if (WARN_ON(sscanf(line, search_str, value) != 1)) {	/* Flawfinder: ignore */
			free(origin_p);
			return ret;
		}
		ret = 0;
		break;
	}
	free(origin_p);
	return ret;
}

static int hostapd_cli_get_sta_list(char *buf, uint8_t *stas, int *num_stas)
{
	const char *line;
	char *p;
	uint8_t tmp_sta[6];
	int i = 0;
	int ret;

	if (strlen(buf) == 0) {
		*num_stas = 0;
		return 0;
	}

	p = buf;
	while ((line = strsep(&p, "\n"))) {
		ret = sscanf(line, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
				&tmp_sta[0],
				&tmp_sta[1],
				&tmp_sta[2],
				&tmp_sta[3],
				&tmp_sta[4],
				&tmp_sta[5]);

		if (WARN_ON(ret != 6))
			continue;

		if(i >= *num_stas) {
			libwifi_err("%s Number of connected stations %d exceeds expected %d!\n",
				__func__, (i + 1), *num_stas);
			return -EINVAL;
		}

		memcpy(&stas[i*6], tmp_sta, 6);
		i++;
	}
	*num_stas = i;
	return 0;
}

#ifdef LIBWIFI_USE_CTRL_IFACE
struct wpa_ctrl {
	int s;
	struct sockaddr_un local;
	struct sockaddr_un dest;
};

static struct wpa_ctrl * wpa_ctrl_open(const char *ctrl_path)
{
	struct wpa_ctrl *ctrl;
	static int counter = 0;		/* FIXME-CR */
	int ret;
	int tries = 0;
	int flags;

	if (ctrl_path == NULL)
		return NULL;

	ctrl = malloc(sizeof(*ctrl));
	if (ctrl)
		memset(ctrl, 0, sizeof(*ctrl));
	else
		return NULL;

	ctrl->s = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (ctrl->s < 0) {
		free(ctrl);
		return NULL;
	}

	ctrl->local.sun_family = AF_UNIX;
	counter++;
try_again:
	ret = snprintf(ctrl->local.sun_path,	/* Flawfinder: ignore */
				sizeof(ctrl->local.sun_path),
				CONFIG_CTRL_IFACE_CLIENT_DIR "/"
				CONFIG_CTRL_IFACE_CLIENT_PREFIX "%d-%d",
				(int) getpid(), counter);

	if (ret < 0 || (unsigned int) ret >= sizeof(ctrl->local.sun_path)) {
		close(ctrl->s);
		free(ctrl);
		return NULL;
	}
	tries++;

	if (bind(ctrl->s, (struct sockaddr *) &ctrl->local,
			sizeof(ctrl->local)) < 0) {
		if (errno == EADDRINUSE && tries < 2) {
			/*
			 * getpid() returns unique identifier for this instance,
			 * so the existing socket file must have
			 * been left by unclean termination of an earlier run.
			 * Remove the file and try again.
			 */
			unlink(ctrl->local.sun_path);
			goto try_again;
		}
		close(ctrl->s);
		free(ctrl);
		return NULL;
	}

	ctrl->dest.sun_family = AF_UNIX;

	if (strlen(ctrl_path) > sizeof(ctrl->dest.sun_path)) {
		close(ctrl->s);
		free(ctrl);
		return NULL;
	}

	memset(ctrl->dest.sun_path, '\0', sizeof(ctrl->dest.sun_path));
	strncpy(ctrl->dest.sun_path, ctrl_path, sizeof(ctrl->dest.sun_path) - 1);

	if (connect(ctrl->s, (struct sockaddr *) &ctrl->dest,
			sizeof(ctrl->dest)) < 0) {
		close(ctrl->s);
		unlink(ctrl->local.sun_path);
		free(ctrl);
		return NULL;
	}

	/*
	* Make socket non-blocking so that we don't hang forever if
	* target dies unexpectedly.
	*/
	flags = fcntl(ctrl->s, F_GETFL);
	if (flags >= 0) {
		flags |= O_NONBLOCK;
		if (fcntl(ctrl->s, F_SETFL, flags) < 0) {
			perror("fcntl(ctrl->s, O_NONBLOCK)");
			/* Not fatal, continue on.*/
		}
	}

	return ctrl;
}

static void wpa_ctrl_close(struct wpa_ctrl *ctrl)
{
	if (ctrl == NULL)
		return;
	unlink(ctrl->local.sun_path);
	if (ctrl->s >= 0)
		close(ctrl->s);
	free(ctrl);
}

static int wpa_ctrl_request(struct wpa_ctrl *ctrl, const char *cmd, size_t cmd_len,
			char *reply, size_t *reply_len)
{
	struct timeval tv;
	int res;
	fd_set rfds;
	int tries = 0;
	char *cmd_buf = NULL;

	errno = 0;
retry_send:
	tries++;
	if (send(ctrl->s, cmd, cmd_len, 0) < 0) {
		if (tries < MAX_TRIES && (errno == EAGAIN || errno == EBUSY || errno == EWOULDBLOCK)) {
			usleep(1 * 1000);
			goto retry_send;
		}
		free(cmd_buf);
		return -1;
	}
	free(cmd_buf);

	for (;;) {
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_SET(ctrl->s, &rfds);
		res = select(ctrl->s + 1, &rfds, NULL, NULL, &tv);
		if (res < 0 && errno == EINTR)
			continue;
		if (res < 0)
			return res;
		if (FD_ISSET(ctrl->s, &rfds)) {
			res = recv(ctrl->s, reply, *reply_len, 0);
			if (res < 0)
				return res;
			if ((res > 0 && reply[0] == '<') ||
				(res > 6 && strncmp(reply, "IFNAME=", 7) == 0)) {
				// unsolicited is not the response to the command sent
				continue;
			}
			*reply_len = res;
			break;
		} else {
			return -2;
		}
	}
	return 0;
}

static int wpa_ctrl_cmd(char *buf, size_t buflen, const char *ifname, const char *cmd, const char *ctrl_iface_dir)
{
	int ret;
	struct wpa_ctrl * conn_ptr;
	char cfile[PATH_MAX] = {};

	if (ifname == NULL)
		return -1;

	snprintf(cfile, sizeof(cfile), "%s/%s", ctrl_iface_dir, ifname);
	conn_ptr = wpa_ctrl_open(cfile);
	if (conn_ptr == NULL)
		return -1;

	ret = wpa_ctrl_request(conn_ptr, cmd, strlen(cmd), buf, &buflen);
	wpa_ctrl_close(conn_ptr);

	if (ret == -2) {
		libwifi_dbg("%s %s '%s' command timed out.\n\n", ifname, __func__, cmd);
		return -2;
	} else if (ret < 0) {
		libwifi_dbg("%s %s '%s' command failed.\n\n", ifname, __func__, cmd);
		return -1;
	}

	return 0;
}

static char *upstr_cmd(char *str)
{
	char *p = str;
	/* uppercase only the command, not the arguments */
	while (*p && *p != ' ') {
		*p = (char)toupper((unsigned char) *p);
		p++;
	}
	return str;
}

static char *del_escapes(char *str)
{
	int start = 0, end = 0;

	if (!str)
		return NULL;

	end = strlen(str);
	if (end == 0)
		return str;

	while (start < end && str[start] != '\0') {
		if (str[start] == '\\') {
			memmove(str + start, str + start + 1, end - start);
			end--;
		}
		start++;
	}

	return str;
}

static int ctrl_iface_set(const char *ifname, const char *cmd, bool check_ok, bool isHostapd)
{
	char buf[128] = {};
	char cmdbuf[128] = {};
	char *p;

	libwifi_dbg("%s %s set:\n%s\n", ifname, __func__, cmd);

	if (!ifname)
		return -1;
	if (!strlen(ifname))
		return -1;

	p = strstr(cmd, "raw");
	if (p)
		strlcpy(cmdbuf, p + strlen("raw"), sizeof(cmdbuf));
	else
		strlcpy(cmdbuf, cmd, sizeof(cmdbuf));

	trim(cmdbuf);
	upstr_cmd(cmdbuf);
	del_escapes(cmdbuf);

	if (wpa_ctrl_cmd(buf, sizeof(buf), ifname, cmdbuf,
			isHostapd ? CONFIG_HOSTAPD_CTRL_IFACE_DIR : CONFIG_WPA_CTRL_IFACE_DIR))
		return -1;

	remove_newline(buf);

	if(!check_ok)
		return 0;

	if (!strcmp(buf, "OK")) {
		libwifi_dbg("OK\n");
		return 0;
	}

	return -1;
}

int hostapd_cli_set(const char *ifname, const char *cmd, bool check_ok)
{
	return ctrl_iface_set(ifname, cmd, check_ok, true);
}

static int wpa_cli_set(const char *ifname, const char *cmd, bool check_ok)
{
	return ctrl_iface_set(ifname, cmd, check_ok, false);
}

static int ctrl_iface_get(const char *ifname, const char *cmd, char *out, size_t out_size, bool isHostapd)
{
	char cmdbuf[128] = {};
	char *p;

	libwifi_dbg("%s %s get:\n%s\n", ifname, __func__, cmd);

	if (!ifname)
		return -1;
	if (!strlen(ifname))
		return -1;

	p = strstr(cmd, "raw");
	if (p)
		strlcpy(cmdbuf, p + strlen("raw"), sizeof(cmdbuf));
	else
		strlcpy(cmdbuf, cmd, sizeof(cmdbuf));

	trim(cmdbuf);
	upstr_cmd(cmdbuf);

	if (wpa_ctrl_cmd(out, out_size, ifname, cmdbuf,
			isHostapd ? CONFIG_HOSTAPD_CTRL_IFACE_DIR : CONFIG_WPA_CTRL_IFACE_DIR))
		return -1;

	remove_newline(out);

	/* STA-FIRST and STA-NEXT can return empty string and this is not error */
	if (out[0] == '\0' && !strstr(cmd, "STA-")) {
		libwifi_err("%s %s failed\n", ifname, __func__);
		return -1;
	}

	libwifi_dbg("%s %s get:\n%s\n", ifname, __func__, out);
	return 0;
}

int hostapd_cli_get(const char *ifname, const char *cmd, char *out, size_t out_size)
{
	return ctrl_iface_get(ifname, cmd, out, out_size, true);
}

static int wpa_cli_get(const char *ifname, const char *cmd, char *out, size_t out_size)
{
	return ctrl_iface_get(ifname, cmd, out, out_size, false);
}

static int hostapd_ctrl_get_stas(const char *ifname, char *ctrl_buf, size_t ctrl_buf_size)
{
	char buf[2048] = { 0 };
	char sta_mac[32] = { 0 };
	char cmd[64] = { 0 };
	char *pos;
	int ret, n;

	pos = ctrl_buf;
	ret = ctrl_iface_get(ifname, "STA-FIRST", buf, sizeof(buf), true);
	if (WARN_ON(ret))
		return ret;

	if (!strlen(buf)) {
		libwifi_dbg("[%s] %s: None associated devices\n", ifname, __func__);
		return 0;
	}

	if (ctrl_buf_size < strlen(buf)) {
		libwifi_err("[%s] %s: Output buffer too small %i < %i\n", ifname, __func__, ctrl_buf_size, strlen(buf));
		return -1;
	}

	do {
		if (!strlen(buf))
			break;
		if (strstr(buf, "FAIL")) {
			libwifi_warn("[%s] %s:Failed to get STA from hostapd\n", ifname, __func__);
			return -1;
		}

		if (WARN_ON(!strchr(buf, '\n')))
			return -1;

		n = (int)(strchr(buf, '\n') - buf);
		strncpy(sta_mac, buf, n);
		if ((pos + n + 1 - ctrl_buf) < ctrl_buf_size) {
			strncpy(pos, buf, n + 1);
			pos += n + 1;
		} else {
			libwifi_warn("[%s] %s Collected sta mac's exceed output buffer size %i > %i\n",
			             ifname, __func__, (int)(pos + n +1 - ctrl_buf), ctrl_buf_size);
			break;
		}
		snprintf(cmd, sizeof(cmd), "STA-NEXT %s" , sta_mac);
		memset(buf, 0, 1024);
		ret = ctrl_iface_get(ifname, cmd, buf, sizeof(buf), true);
		if (WARN_ON(ret))
			return ret;
	} while (true);
	*(pos - 1) = '\0';
	return 0;
}

int hostapd_iface_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas)
{
	char ctrl_buf[4096] = { 0 };

	libwifi_dbg("[%s] %s\n", ifname, __func__);

	if (!ifname || !strlen(ifname)) {
		libwifi_err("[%s] %s Invalid args!\n", ifname, __func__);
		return -EINVAL;
	}
	if (!stas || *num_stas <= 0) {
		libwifi_err("[%s] %s Invalid args!\n", ifname, __func__);
		return -EINVAL;
	}

	memset(stas, 0, *num_stas * 6);
	if (hostapd_ctrl_get_stas(ifname, ctrl_buf, sizeof(ctrl_buf)))
		return -1;

	if (hostapd_cli_get_sta_list(ctrl_buf, stas, num_stas))
		return -1;

	return 0;
}

#else
int hostapd_cli_set(const char *ifname, const char *cmd, bool check_ok)
{
	char buf[128];

	libwifi_dbg("%s %s set:\n%s\n", ifname, __func__, cmd);

	if (!ifname)
		return -1;
	if (!strlen(ifname))
		return -1;

	chrCmd(buf, sizeof(buf), "timeout %d hostapd_cli -i %s %s", WPACTRL_TIMEOUT, ifname, cmd);

	if(!check_ok)
		return 0;

	if (!strcmp(buf, "OK")) {
		libwifi_dbg("OK\n");
		return 0;
	}

	return -1;
}

int hostapd_cli_get(const char *ifname, const char *cmd, char *out, size_t out_size)
{
	if (!ifname)
		return -1;
	if (!strlen(ifname))
		return -1;

	chrCmd(out, out_size, "timeout %d hostapd_cli -i %s %s", WPACTRL_TIMEOUT, ifname, cmd);

	if (out[0] == '\0') {
		libwifi_err("%s %s failed\n", ifname, __func__);
		return -1;
	}

	libwifi_dbg("%s %s get:\n%s\n", ifname, __func__, out);
	return 0;
}

static int wpa_cli_set(const char *ifname, const char *cmd, bool check_ok)
{
	char buf[128];

	libwifi_dbg("%s %s set:\n%s\n", ifname, __func__, cmd);

	if (!ifname)
		return -1;
	if (!strlen(ifname))
		return -1;

	chrCmd(buf, sizeof(buf), "timeout %d wpa_cli -i %s %s", WPACTRL_TIMEOUT, ifname, cmd);

	if(!check_ok)
		return 0;

	if (!strcmp(buf, "OK")) {
		libwifi_dbg("OK\n");
		return 0;
	}

	return -1;
}

static int wpa_cli_get(const char *ifname, const char *cmd, char *out, size_t out_size)
{
	if (!ifname)
		return -1;
	if (!strlen(ifname))
		return -1;

	chrCmd(out, out_size, "timeout %d wpa_cli -i %s %s", WPACTRL_TIMEOUT, ifname, cmd);

	if (out[0] == '\0') {
		libwifi_err("%s %s failed\n", ifname, __func__);
		return -1;
	}

	libwifi_dbg("%s %s get:\n%s\n", ifname, __func__, out);
	return 0;
}

int hostapd_iface_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas)
{
	char buf[1024];

	if (!ifname || !strlen(ifname)) {
		libwifi_err("[%s] %s Invalid args!\n", ifname, __func__);
		return -EINVAL;
	}
	if (!stas || *num_stas <= 0) {
		libwifi_err("[%s] %s Invalid args!\n", ifname, __func__);
		return -EINVAL;
	}

	memset(stas, 0, *num_stas * 6);
	chrCmd(buf, sizeof(buf), "timeout %d hostapd_cli -i %s list_sta", WPACTRL_TIMEOUT, ifname);

	if (hostapd_cli_get_sta_list(buf, stas, num_stas))
		return -1;

	return 0;
}

#endif
static int hostapd_cli_iface_get_sta_flags(const char *buf, char *flags, size_t flags_max)
{
	char val[256] = { 0 };
	size_t flags_len;

	if (!flags || flags_max == 0)
		return -EINVAL;

	if (get_param(buf, "flags", val, sizeof(val))) {
		libwifi_warn("%s flags parameter not found in buffer [%s]!\n",
			__func__, buf);
		return -1;
	}

	flags_len = strlen(val);
	if (WARN_ON(flags_len >= flags_max))
		return -EINVAL;

	snprintf(flags, flags_max, "%s", val);

	return 0;
}

static int hostapd_cli_hexstr_to_bin(char *hex_buf, uint8_t *bin_buf, size_t *bin_buf_len)
{
	char *ptr;
	uint8_t octet = 0;
	int i = 0;
	int hex_buf_len;

	if (WARN_ON(!hex_buf) || WARN_ON(!bin_buf) || WARN_ON(!bin_buf_len) || WARN_ON(*bin_buf_len == 0))
		return -EINVAL;

	hex_buf_len = strlen(hex_buf);
	if (hex_buf_len == 0 || hex_buf_len % 2 != 0) {
		libwifi_err("Invalid hex string [%s] len[%ld]\n", hex_buf, hex_buf_len);
		return -1;
	}

	ptr = hex_buf;
	while (*ptr != '\0' && i < *bin_buf_len) {
		sscanf(ptr, "%2hhx", &octet);
		bin_buf[i] = octet;
		ptr += 2;
		i++;
	}
	*bin_buf_len = i;

	return 0;
}

int hostapd_cli_iface_ap_info(const char *ifname, struct wifi_ap *ap)
{
	char buf[1024];
	char val[256] = { 0 };
	int ret = 0;
	uint8_t beacon[4096] = { 0 };
	uint8_t *beacon_ies;
	uint8_t *ie_ptr;
	size_t beacon_len = sizeof(beacon);
	size_t ies_len;
	struct beacon_frame *bf;

	/* get beacon */
	ret = hostapd_cli_get_beacon(ifname, beacon, &beacon_len);
	if (WARN_ON(ret)) {
		ap->enabled = false;
		return ret;
	}

	ap->enabled = true;
	bf = (struct beacon_frame *)beacon;
	ies_len = beacon_len - ((uint8_t *)&bf->cap_info - beacon + 2);
	beacon_ies = bf->var;

	WARN_ON(wifi_oper_stds_set_from_ie(beacon_ies, ies_len, &ap->bss.oper_std));
	WARN_ON(wifi_ssid_advertised_set_from_ie(beacon_ies, ies_len, &ap->ssid_advertised));
	WARN_ON(wifi_get_bss_security_from_ies(&ap->bss, beacon_ies, ies_len));
	WARN_ON(wifi_apload_set_from_ie(beacon_ies, ies_len, &ap->bss.load));
	ap->bss.beacon_int = bf->beacon_int;

	/* capabilities information */
	ie_ptr = (uint8_t *)&bf->cap_info;
	ap->bss.caps.valid |= WIFI_CAP_BASIC_VALID;
	memcpy(&ap->bss.caps.basic, ie_ptr, sizeof(bf->cap_info));
	wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, sizeof(bf->cap_info));

	/* ht capabilities */
	ie_ptr = wifi_find_ie(beacon_ies, ies_len, IE_HT_CAP);
	if (ie_ptr) {
		ap->bss.caps.valid |= WIFI_CAP_HT_VALID;
		memcpy(&ap->bss.caps.ht, &ie_ptr[2], ie_ptr[1]);
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* vht capabilities */
	ie_ptr = wifi_find_ie(beacon_ies, ies_len, IE_VHT_CAP);
	if (ie_ptr) {
		ap->bss.caps.valid |= WIFI_CAP_VHT_VALID;
		memcpy(&ap->bss.caps.vht, &ie_ptr[2], ie_ptr[1]);
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* extended capabilities */
	ie_ptr = wifi_find_ie(beacon_ies, ies_len, IE_EXT_CAP);
	if (ie_ptr) {
		ap->bss.caps.valid |= WIFI_CAP_EXT_VALID;
		memcpy(&ap->bss.caps.ext, &ie_ptr[2], ie_ptr[1]);
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* RM enabled capabilities */
	ie_ptr = wifi_find_ie(beacon_ies, ies_len, IE_RRM);
	if (ie_ptr) {
		ap->bss.caps.valid |= WIFI_CAP_RM_VALID;
		memcpy(&ap->bss.caps.ext, &ie_ptr[2], ie_ptr[1]);
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* mobility domain element */
	ie_ptr = wifi_find_ie(beacon_ies, ies_len, IE_MDE);
	if (ie_ptr) {
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* HE capabilities */
	ie_ptr = wifi_find_ie_ext(beacon_ies, ies_len, IE_EXT_HE_CAP);
	if (ie_ptr) {
		ap->bss.caps.valid |= WIFI_CAP_HE_VALID;
		memcpy(&ap->bss.caps.he, &ie_ptr[3], min(ie_ptr[1], sizeof(struct wifi_caps_he)));
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* EHT capabilities */
	ie_ptr = wifi_find_ie_ext(beacon_ies, ies_len, IE_EXT_EHT_CAP);
	if (ie_ptr) {
		ap->bss.caps.valid |= WIFI_CAP_EHT_VALID;
		memcpy(&ap->bss.caps.eht, &ie_ptr[3], min(ie_ptr[1], sizeof(struct wifi_caps_eht)));
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* WMM */
	ie_ptr = wifi_find_vsie(beacon_ies, ies_len, microsoft_oui, 2, 1);
	if (ie_ptr) {
		wifi_wmm_set_from_ie(ap->ac, ie_ptr, ie_ptr[1] + 2);
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* get ap info from STATUS-DRIVER */
	ret = hostapd_cli_get(ifname, "raw STATUS-DRIVER", buf, sizeof(buf));
	if (WARN_ON(ret))
		return ret;

	if (get_param(buf, "capa.max_stations", val, sizeof(val))) {
		libwifi_warn("%s capa.max_stations parameter not found in buffer [%s]!\n",
			__func__, buf);
	}

	if (WARN_ON(sscanf(val, "%d", &ap->assoclist_max) != 1)) {
		libwifi_warn("Failed to parse capa.max_stations parameter\n");
	}

	return ret;
}

static int hostapd_cli_get_sta_capability_information(const char *sta_buf, uint8_t *ie, size_t *ie_len) {
	char val[256] = { 0 };
	uint16_t capa = 0;
	uint8_t *ptr;

	if (WARN_ON(!ie) || WARN_ON(*ie_len < 2))
		return -EINVAL;

	if (get_param(sta_buf, "capability", val, sizeof(val))) {
		libwifi_warn("%s capability parameter not found in buffer [%s]!\n",
		__func__, sta_buf);
		return -1;
	}

	if (sscanf(val, "%hx", &capa) != 1)
		return -1;

	ptr = (uint8_t *)&capa;
	ie[0] = ptr[0];
	ie[1] = ptr[1];
	*ie_len = 2;

	return 0;
}

static int hostapd_cli_get_sta_ht_capabilities_info(const char *sta_buf, uint8_t *ie, size_t *ie_len) {
	char val[256] = { 0 };
	uint16_t capa = 0;
	uint8_t *ptr;

	if (WARN_ON(!ie) || WARN_ON(*ie_len < 2))
		return -EINVAL;

	if (get_param(sta_buf, "ht_caps_info", val, sizeof(val))) {
		libwifi_warn("%s ht_caps_info parameter not found in buffer [%s]!\n",
		__func__, sta_buf);
		return -1;
	}

	if (sscanf(val, "%hx", &capa) != 1)
		return -1;

	ptr = (uint8_t *)&capa;
	ie[0] = ptr[0];
	ie[1] = ptr[1];
	*ie_len = 2;

	return 0;
}

static int hostapd_cli_get_sta_vht_capabilities_info(const char *sta_buf, uint8_t *ie, size_t *ie_len) {
	char val[256] = { 0 };
	uint32_t capa = 0;
	uint8_t *ptr;

	if (WARN_ON(!ie) || WARN_ON(*ie_len < 4))
		return -EINVAL;

	if (get_param(sta_buf, "vht_caps_info", val, sizeof(val))) {
		libwifi_warn("%s vht_caps_info parameter not found in buffer [%s]!\n",
		__func__, sta_buf);
		return -1;
	}

	if (sscanf(val, "%x", &capa) != 1)
		return -1;
	ptr = (uint8_t *)&capa;
	ie[0] = ptr[0];
	ie[1] = ptr[1];
	ie[2] = ptr[2];
	ie[3] = ptr[3];
	*ie_len = 4;

	return 0;
}

static int hostapd_cli_get_extended_capabilities(const char *sta_buf, uint8_t *ie, size_t *ie_len)
{
	int i = 0;
	size_t ext_len;
	char val[256] = { 0 };
	uint8_t octet = 0;
	char *ptr;

	if (WARN_ON(!ie) || WARN_ON(*ie_len < 8))
		return -EINVAL;

	if (get_param(sta_buf, "ext_capab", val, sizeof(val))) {
		libwifi_warn("%s ext_capab parameter not found in buffer [%s]!\n",
		__func__, sta_buf);
		return -1;
	}
	ext_len = strlen(val);

	if (ext_len == 0 || ext_len % 2 != 0) {
		libwifi_err("Invalid format of ext_capab[%s] len[%ld]\n", val, ext_len);
		return -1;
	}

	ptr = val;
	while (*ptr != '\0' && i < *ie_len) {
		sscanf(ptr, "%2hhx", &octet);
		ie[i] = octet;
		ptr += 2;
		i++;
	}
	*ie_len = i;

	return 0;
}

int hostapd_cli_iface_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *info)
{
	char sta_flags[128] = { 0 };
	char cmd[64] = { 0 };
	char buf[2048] = { 0 };
	uint8_t ie[128] = { 0 };
	int ret = 0;
	size_t ie_max = sizeof(ie);
	size_t ie_len;

	libwifi_dbg("%s %s \n", ifname, __func__);

	snprintf(cmd, sizeof(cmd), "sta " MACSTR, MAC2STR(addr));	/* Flawfinder: ignore */

	if (WARN_ON(hostapd_cli_get(ifname, cmd, buf, sizeof(buf))))
		return -1;

	/* Make sure we set back addr */
	memcpy(info->macaddr, addr, sizeof(info->macaddr));

	ie_len = ie_max;
	ret = hostapd_cli_get_sta_capability_information(buf, ie, &ie_len);

	if (WARN_ON(ret))
		wifi_cap_set_from_capability_information(info->cbitmap, ie, ie_len);

	hostapd_cli_iface_get_sta_flags(buf, sta_flags, sizeof(sta_flags));

	if (strstr(sta_flags, "[HT]")) {
		info->caps.valid |= WIFI_CAP_HT_VALID;
		ie_len = ie_max;
		ret = hostapd_cli_get_sta_ht_capabilities_info(buf, ie, &ie_len);
		if (!ret)
			wifi_cap_set_from_ht_capabilities_info(info->cbitmap, ie, ie_len);
	}

	if (strstr(sta_flags, "[VHT]")) {
		info->caps.valid |= WIFI_CAP_VHT_VALID;
		ie_len = ie_max;
		ret = hostapd_cli_get_sta_vht_capabilities_info(buf, ie, &ie_len);
		if (!ret)
			wifi_cap_set_from_vht_capabilities_info(info->cbitmap, ie, ie_len);
	}

	ie_len = ie_max;
	ret = hostapd_cli_get_extended_capabilities(buf, ie, &ie_len);
	if (!ret) {
		info->caps.valid |= WIFI_CAP_EXT_VALID;
		wifi_cap_set_from_extended_capabilities(info->cbitmap, ie, ie_len);
	}

	return 0;
}

int hostapd_cli_get_ssid(const char *ifname, char *ssid, size_t ssid_size)
{
	char buf[1024];
	int ret;

	if (!ssid || ssid_size == 0)
		return -EINVAL;

	memset(ssid, 0, ssid_size);

	ret = hostapd_cli_get(ifname, "get_config", buf, sizeof(buf));
	if (WARN_ON(ret))
		return ret;

	ret = get_param(buf, "ssid", ssid, ssid_size);
	if (ret) {
		libwifi_warn("%s ssid parameter not found in buffer [%s]!\n",
				__func__, buf);
		return ret;
	}

	return 0;
}

int hostapd_cli_iface_add_neighbor(const char *ifname, struct nbr *nbr, size_t nbr_size)
{
	char bssid[18] = { 0 };
	char nr[256] = { 0 };
	char cmd[1024] = { 0 };
	char ssid[128];

	if (hostapd_cli_get_ssid(ifname, ssid, sizeof(ssid)))
		return -1;

	snprintf(bssid, sizeof(bssid), MACSTR, MAC2STR(nbr->bssid));	/* Flawfinder: ignore */
	btostr((uint8_t *) nbr, nbr_size, nr);

	snprintf(cmd, sizeof(cmd), "set_neighbor " MACSTR " ssid=\\\"%s\\\" nr=%s",
		 MAC2STR(nbr->bssid), ssid, nr);

	return hostapd_cli_set(ifname, cmd, true);
}

int hostapd_cli_iface_del_neighbor(const char *ifname, unsigned char *bssid)
{
	char cmd[1024] = { 0 };
	char ssid[128];

	if (hostapd_cli_get_ssid(ifname, ssid, sizeof(ssid)))
		return -1;

	snprintf(cmd, sizeof(cmd), "remove_neighbor " MACSTR " ssid=\\\"%s\\\"",
		 MAC2STR(bssid), ssid);

	return hostapd_cli_set(ifname, cmd, true);
}

int hostapd_cli_iface_start_wps(const char *ifname, struct wps_param wps)
{
	char cmd[1024] = { 0 };
	enum wifi_mode mode;
	int ret = 0;

	if (wps.method == WPS_METHOD_PBC){
		snprintf(cmd, sizeof(cmd),"wps_pbc");
	} else if (wps.method == WPS_METHOD_PIN) {
		snprintf(cmd, sizeof(cmd),"wps_pin %s %08lu", "any", wps.pin);
	}

	ret = wifi_get_mode(ifname, &mode);

	if(ret != 0)
		return ret;

	switch (mode) {
	case WIFI_MODE_AP:
	case WIFI_MODE_AP_VLAN:
		ret = hostapd_cli_set(ifname, cmd, true);
		break;
	case WIFI_MODE_STA:
		if(wps.role & WPS_ENROLLEE_BSTA) snprintf(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), " multi_ap=1");
		ret = wpa_cli_set(ifname, cmd, false); //Do not check OK answer. wpa_cli wps_pin returns pin.
		break;
	default:
		WARN_ON(1);
		ret = -1;
		break;
	}

	return ret;
}

int hostapd_cli_iface_stop_wps(const char *ifname)
{
	enum wifi_mode mode;
	int ret = 0;

	ret = wifi_get_mode(ifname, &mode);

	if(ret != 0)
		return ret;

	switch (mode) {
	case WIFI_MODE_AP:
	case WIFI_MODE_AP_VLAN:
		ret = hostapd_cli_set(ifname, "wps_cancel", true);
		break;
	case WIFI_MODE_STA:
		ret = wpa_cli_set(ifname, "wps_cancel", true);
		break;
	default:
		WARN_ON(1);
		ret = -1;
		break;
	}

	return ret;
}

int hostapd_cli_iface_get_wps_status(const char *ifname, enum wps_status *s)
{
	char buf[256] = {0};
	int ret;

	ret = hostapd_cli_get(ifname, "wps_get_status", buf, sizeof(buf));
	if (WARN_ON(ret))
		return ret;

	const char *f1 = "PBC Status: ";
	const char *f2 = "Last WPS result: ";
	char *pos1, *pos2;

	pos1 = strstr(buf, f1);
	pos2 = strstr(buf, f2);

	if (!pos1 || !pos2)
		return -1;

	pos1 += strlen(f1);
	if (strstr(pos1, "Disabled"))
		*s = WPS_STATUS_INIT;
	else if (strstr(pos1, "Active"))
		*s = WPS_STATUS_PROCESSING;
	else if (strstr(pos1, "Timed-out"))
		*s = WPS_STATUS_TIMEOUT;
	else if (strstr(pos1, "Overlap"))
		*s = WPS_STATUS_FAIL;
	else
		*s = WPS_STATUS_UNKNOWN;

	if (strstr(pos2, "Success"))
		*s = WPS_STATUS_SUCCESS;

	libwifi_dbg("%s %s status %d \n", ifname, __func__,*s);
	return 0;
}

int hostapd_cli_iface_get_wps_ap_pin(const char *ifname, unsigned long *pin)
{
	char buf[256] = {0};
	int ret = 0;

	ret = hostapd_cli_get(ifname, "wps_ap_pin get", buf, sizeof(buf));
	if (WARN_ON(ret))
		return ret;

	if (WARN_ON(1 != sscanf(buf,"%lu",pin)))
		ret = -1;

	return ret;
}

int hostapd_cli_iface_set_wps_ap_pin(const char *ifname, unsigned long pin)
{
	char cmd[1024] = {0};
	char buf[256] = {0};
	char pinbuf[9] = {0};
	int ret = 0;

	snprintf(pinbuf, 9, "%08lu", pin);
	snprintf(cmd, sizeof(cmd),"wps_ap_pin set %08lu", pin);

	ret = hostapd_cli_set(ifname, cmd, false);
	if (WARN_ON(ret))
		return ret;

	ret = hostapd_cli_get(ifname, "wps_ap_pin get", buf, sizeof(buf));
	if (WARN_ON(ret))
		return ret;

	if (WARN_ON(!strstr(buf, pinbuf)))
		ret = -1;

	return ret;
}

int hostapd_cli_iface_get_neighbor_list(const char *ifname, struct nbr *nbrs, int *nr)
{
	struct nbr *nbr;
	char *p, *k, *l;
	char *line;
	char *word;
	char *buf;
	int count;
	int len;
	int ret;

	/* make sure we have place for show_neighbor output */
	buf = calloc(*nr, 256);
	WARN_ON(!buf);
	if (!buf)
		return -1;

	ret = hostapd_cli_get(ifname, "show_neighbor", buf, *nr * 256);
	if (WARN_ON(ret)) {
		free(buf);
		return ret;
	}

	nbr = nbrs;
	count = 0;

	p = buf;
	while ((line = strsep(&p, "\n"))) {
		k = line;
		l = NULL;

		if (WARN_ON(count >= *nr))
			break;

		while ((word = strsep(&k, " "))) {
			if (!(l = strstr(word, "nr=")))
				continue;

			len = strlen(l) - 3;
			if (len > 2 * sizeof(*nbr))
				len = 2 * sizeof(*nbr);

			strtob(l + 3, len, (uint8_t *) nbr);
			break;
		}

		if (!l)
			continue;

		libwifi_dbg("nbr[%d] " MACSTR " bssid_info 0x%x reg %u chan %u phy %u\n",
			count, MAC2STR(nbr->bssid), nbr->bssid_info, nbr->reg,
			nbr->channel, nbr->phy);

		count++;
		nbr = &nbrs[count];
	}

	free(buf);
	*nr = count;
	return ret;
}

int hostapd_cli_iface_req_beacon(const char *ifname, unsigned char *sta,
				 struct wifi_beacon_req *req,
				 size_t req_size)
{
	char cmd[1024] = { 0 };
	char req_hex[256] = { 0 };
	size_t size;
	struct wifi_opclass opclass;
	uint8_t mcast_bssid[6] = "\xff\xff\xff\xff\xff\xff";
	uint8_t empty_bssid[6] = "\x00\x00\x00\x00\x00\x00";
	enum wifi_bw bw = BW_UNKNOWN;
	bool is_dfs = true;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	size = req_size;
	if (WARN_ON(req_size * 2 >= sizeof(req_hex)))
		size = sizeof(req_hex) / 2;

	/* opclass must be set */
	if (!req->oper_class) {
		if (WARN_ON(wifi_get_opclass_ht20(ifname, &opclass)))
			return -1;
		req->oper_class = (uint8_t)opclass.g_opclass;
	}

	/* use multicast if bssid is not set */
	if (!memcmp(req->bssid, empty_bssid, 6))
		memcpy(req->bssid, mcast_bssid, sizeof(mcast_bssid));

	/* use most appropriate mode if not set explicitly */
	if (req->mode == WIFI_BCNREQ_MODE_UNSET) {
		if (req->channel) {
			wifi_get_bandwidth(ifname, &bw);

			if (WARN_ON(bw < BW20 || bw > BW160))
				return -1;

			is_dfs = wifi_is_dfs_channel(ifname, req->channel, (20 << bw));
		}

		if (!is_dfs)
			req->mode = WIFI_BCNREQ_MODE_ACTIVE;
		else
			req->mode = WIFI_BCNREQ_MODE_PASSIVE;
	}

	/* adjust duration in case not set explicitly */
	if (!req->duration) {
		if (req->mode == WIFI_BCNREQ_MODE_PASSIVE)
			req->duration = 100;
		else if (req->mode == WIFI_BCNREQ_MODE_ACTIVE)
			req->duration = 20;
	}

	/* convert to LE */
	req->rand_interval = htole16(req->rand_interval);
	req->duration = htole16(req->duration);

	btostr((uint8_t *) req, size, req_hex);
	snprintf(cmd, sizeof(cmd), "req_beacon " MACSTR " %s",
		 MAC2STR(sta), req_hex);

	/* For some reason req_beacon don't reply with OK */
	hostapd_cli_set(ifname, cmd, false);
	return 0;
}

int hostapd_cli_iface_req_beacon_default(const char *ifname, unsigned char *sta)
{
	uint8_t bssid[6] = "\xff\xff\xff\xff\xff\xff";
	struct wifi_beacon_req *req_beacon;
	char req[sizeof(*req_beacon) + 64 + 2] = { 0 };
	struct wifi_opclass opclass;
	char ssid[64] = { 0 };

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (WARN_ON(hostapd_cli_get_ssid(ifname, ssid, sizeof(ssid))))
		return -1;

	if (WARN_ON(wifi_get_opclass_ht20(ifname, &opclass)))
		return -1;

	req_beacon = (struct wifi_beacon_req *) req;

	/* Minimal configuration - passive, all channels, all bssid and our SSID */
	req_beacon->oper_class = (uint8_t)opclass.g_opclass;
	req_beacon->mode = WIFI_BCNREQ_MODE_PASSIVE;
	req_beacon->duration = 100;
	memcpy(req_beacon->bssid, bssid, sizeof(bssid));

	/* Add SSID */
	req_beacon->variable[0] = WIFI_BCNREQ_SSID;
	req_beacon->variable[1] = (uint8_t)strlen(ssid);
	strncpy((char *) &req_beacon->variable[2], ssid, 64);

	return hostapd_cli_iface_req_beacon(ifname, sta, req_beacon,
				sizeof(*req_beacon) + 2 + strlen(ssid));
}

int hostapd_cli_disconnect_sta(const char *ifname, uint8_t *sta, uint16_t reason)
{
	char cmd[1024] = { 0 };

	snprintf(cmd, sizeof(cmd), "deauthenticate " MACSTR, MAC2STR(sta));	/* Flawfinder: ignore */

	if (reason > 0)
		snprintf(cmd + strlen(cmd), sizeof(cmd), " reason=%d", reason);

	return hostapd_cli_set(ifname, cmd, true);
}

int hostapd_cli_sta_disconnect_ap(const char *ifname, uint32_t reason)
{
	/* reason parameter is skiped because wpa_supplicant command
	 * disconnect does not allow setup this parameter,
	 * it is hardcoded to 3.
	 */
	return wpa_cli_set(ifname, "disconnect", true);
}

static int get_neighbor_data(const char *ifname, uint8_t *bssid, struct nbr *nbr)
{
	struct nbr *nbrs;
	int ret = -1;
	int nr = 50;
	int i;

	nbrs = calloc(nr, sizeof(*nbr));
	WARN_ON(!nbrs);
	if (!nbrs)
		return -1;

	if (wifi_get_neighbor_list(ifname, nbrs, &nr)) {
		ret = -1;
		goto free;
	}

	if (WARN_ON(!nr)) {
		ret = -1;
		goto free;
	}

	for (i = 0; i < nr; i++) {
		if (!memcmp(bssid, nbrs[i].bssid, 6)) {
			*nbr = nbrs[i];
			ret = 0;
			break;
		}
	}

free:
	free(nbrs);
	return ret;
}

int hostapd_cli_iface_req_bss_transition(const char *ifname, unsigned char *sta,
					 int bsss_nr, struct nbr *bsss,
					 struct bss_transition_params *params)
{
	size_t buflen;
	size_t len;
	char *buf;
	int i;
	int ret;

	buflen = 256 + bsss_nr * 256;

	buf = malloc(buflen);
	WARN_ON(!buf);
	if (!buf)
		return -1;

	len = 0;
	len += snprintf(buf + len, buflen - len, "bss_tm_req " MACSTR " ", MAC2STR(sta));

	for (i = 0 ; i < bsss_nr; i++) {
		struct nbr nbr = {};

		/* Lookup nbr on neighbor list to get missing params */
		if (params->pref && get_neighbor_data(ifname, bsss[i].bssid, &nbr))
			libwifi_warn("[%s] Preferred BSSID " MACSTR " not on neighbor list!\n",
			             ifname, MAC2STR(bsss[i].bssid));

		if (!bsss[i].bssid_info && !nbr.bssid_info)
			libwifi_dbg("[%s] Missing bssid_info for BSSID " MACSTR "!\n",
			            ifname, MAC2STR(bsss[i].bssid));

		if (!bsss[i].reg && !nbr.reg)
			libwifi_dbg("[%s] Missing reg for BSSID " MACSTR "!\n",
			            ifname, MAC2STR(bsss[i].bssid));

		if (!bsss[i].channel && !nbr.channel)
			libwifi_dbg("[%s] Missing channel for BSSID " MACSTR "!\n",
			            ifname, MAC2STR(bsss[i].bssid));

		if (!bsss[i].phy && !nbr.phy)
			libwifi_dbg("[%s] Missing phy for BSSID " MACSTR "!\n",
			            ifname, MAC2STR(bsss[i].bssid));

		len += snprintf(buf + len, buflen - len,
				"neighbor=" MACSTR ",%u,%hhu,%hhu,%hhu ",
				MAC2STR(bsss[i].bssid),
				bsss[i].bssid_info ? bsss[i].bssid_info : nbr.bssid_info,
				bsss[i].reg ? bsss[i].reg : nbr.reg,
				bsss[i].channel ? bsss[i].channel : nbr.channel,
				bsss[i].phy ? bsss[i].phy : nbr.phy);
	}

	len += snprintf(buf + len, buflen - len,
			"pref=%hhu disassoc_imminent=%hhu ",
			params->pref, params->disassoc_imminent);

	if (params->valid_int)
		len += snprintf(buf + len, buflen - len,
				"valid_int=%hhu ", params->valid_int);

	if (params->bss_term)
		len += snprintf(buf + len, buflen - len,
				"bss_term=%hu ", params->bss_term);
	if (params->disassoc_timer)
		len += snprintf(buf + len, buflen - len,
				"disassoc_timer=%hu ", params->disassoc_timer);
	if (params->abridged)
		len += snprintf(buf + len, buflen - len,
				"abridged=%hhu ", params->abridged);
	if (params->ess_term)
		len += snprintf(buf + len, buflen - len,
				"url=%s ", params->url);

	if (params->mbo.valid) {
		len += snprintf(buf + len, buflen - len,
				"mbo=%u:%u:%u", params->mbo.reason,
				params->mbo.reassoc_delay,
				params->mbo.cell_pref);
	}

	ret = hostapd_cli_set(ifname, buf, false);

	free(buf);

	return ret;
}

int hostapd_cli_mbo_disallow_assoc(const char *ifname, int reason)
{
	char cmd[1024] = { 0 };

	snprintf(cmd, sizeof(cmd), "set mbo_assoc_disallow %d", reason);

	return hostapd_cli_set(ifname, cmd, true);
}

int hostapd_cli_ap_get_state(const char *ifname, char *state, int state_size)
{
	char buf[1024] = { 0 };

	/* Get current state */
	if (WARN_ON(hostapd_cli_get(ifname, "status", buf, sizeof(buf))))
		return -1;
	return get_param(buf, "state", state, state_size);
}

int hostapd_cli_ap_set_state(const char *ifname, bool up)
{
	char buf[1024] = { 0 };
	char state[128] = { 0 };
	int ret = 0;

	/* Get current state */
	if (WARN_ON(hostapd_cli_get(ifname, "status", buf, sizeof(buf))))
		return -1;
	if (WARN_ON(get_param(buf, "state", state, sizeof(state))))
		return -1;

	if (up) {
		if (strcmp(state, "ENABLED") && strcmp(state, "DFS"))
			ret = hostapd_cli_set(ifname, "enable", true);
		WARN_ON(hostapd_cli_set(ifname, "update_beacon", true));
	} else {
		if (strcmp(state, "DISABLED"))
			ret = hostapd_cli_set(ifname, "disable", true);
	}

	return ret;
}

int hostapd_cli_iface_add_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	char cmd[1024] = { 0 };
	char ie[512] = { 0 };
	size_t size;

	if (WARN_ON(!req->mgmt_subtype))
		return 0;

	size = req->ie.ie_hdr.len + sizeof(req->ie.ie_hdr);
	if (WARN_ON(size * 2 >= sizeof(ie)))
		return -1;

	btostr((uint8_t *) &req->ie, size, ie);
	snprintf(cmd, sizeof(cmd), "set vendor_elements %s", ie);

	return hostapd_cli_set(ifname, cmd, true);
}

int hostapd_cli_iface_del_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	char cmd[1024] = { 0 };

	snprintf(cmd, sizeof(cmd), "set vendor_elements \"\"");
	return hostapd_cli_set(ifname, cmd, true);
}

int hostapd_cli_is_wds_sta(const char *ifname, uint8_t *sta_mac, char *ifname_wds, size_t ifname_max)
{
	char cmd[64] = { 0 };
	char val[256] = { 0 };
	char buf[2048] = { 0 };
	int is_wds = 0;

	snprintf(cmd, sizeof(cmd), "sta " MACSTR, MAC2STR(sta_mac));	/* Flawfinder: ignore */
	if (hostapd_cli_get(ifname, cmd, buf, sizeof(buf)))
		return 0;

	if (hostapd_cli_iface_get_sta_flags(buf, val, sizeof(val)))
		return 0;

	if (strstr(val, "[WDS]") || strstr(val, "[MULTI_AP]"))
		is_wds = 1;

	if (!ifname_wds)
		return is_wds;

	if (get_param(buf, "ifname_wds", val, sizeof(val))) {
		libwifi_warn("[%s] %s ifname_wds parameter not found in buffer [%s]!\n",
			ifname, __func__, buf);
		return is_wds;
	}

	if (WARN_ON((strlen(val) + 1) > ifname_max))
		return is_wds;

	snprintf(ifname_wds, ifname_max, "%s", val);

	return is_wds;
}

int hostapd_cli_get_oper_stds(const char *ifname, uint8_t *std)
{
	uint8_t beacon_ies[4096] = { 0 };
	size_t ies_len = sizeof(beacon_ies);
	int ret;

	if (WARN_ON(!std))
		return -EINVAL;

	ret = hostapd_cli_get_beacon_ies(ifname, beacon_ies, &ies_len);
	if (WARN_ON(ret)) {
		return ret;
	}

	ret = wifi_oper_stds_set_from_ie(beacon_ies, ies_len, std);
	if (WARN_ON(ret)) {
		return ret;
	}

	return 0;
}

int hostapd_cli_get_beacon(const char *ifname, uint8_t *beacon, size_t *beacon_len)
{
	char buf[4096] = { 0 };

	if (WARN_ON(hostapd_cli_get(ifname, "get beacon", buf, sizeof(buf))))
		return -1;

	return hostapd_cli_hexstr_to_bin(buf, beacon, beacon_len);
}

int hostapd_cli_get_beacon_ies(const char *ifname, uint8_t *beacon_ies, size_t *beacon_ies_len)
{
	char buf[4096] = { 0 };

	if (WARN_ON(hostapd_cli_get(ifname, "get_beacon ies", buf, sizeof(buf))))
		return -1;

	return hostapd_cli_hexstr_to_bin(buf, beacon_ies, beacon_ies_len);
}

int hostapd_ubus_iface_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg)
{
	char buf[256];

	libwifi_dbg("[%s] hostapd monitor sta " MACSTR " %s\n", ifname, MAC2STR(sta),
			cfg->enable ? "enable" : "disable");

	/* Today we enable probe-req monitor in hostapd - even STA isn't connected */
	chrCmd(buf, sizeof(buf), "ubus call hostapd.%s %s '{\"addr\":\"" MACSTR "\"}'",
			ifname, cfg->enable ? "sta_monitor_add" : "sta_monitor_del", MAC2STR(sta));

	if (cfg->enable)
		chrCmd(buf, sizeof(buf),
				"ubus call hostapd.%s event_mask_add '{\"event\":\"probe-req-filtered\"}'",
				ifname);

	return 0;
}

int hostapd_cli_probe_sta(const char *ifname, uint8_t *sta)
{
	char cmd[512] = { 0 };

	if (WARN_ON(!sta))
		return -1;

	snprintf(cmd, sizeof(cmd), "poll_sta " MACSTR, MAC2STR(sta));	/* Flawfinder: ignore */

	return hostapd_cli_set(ifname, cmd, true);
}

int hostapd_ubus_iface_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *mon)
{
	char buf[1024];
	char format[1024];
	int rssi;
	int age;

	libwifi_dbg("[%s] get monitor sta " MACSTR "\n", ifname, MAC2STR(sta));

	chrCmd(buf, sizeof(buf), "ubus -S call hostapd.%s sta_monitor_get '{\"addr\":\"" MACSTR "\"}'",
			ifname, MAC2STR(sta));

	snprintf(format, sizeof(format), "{\"sta\":{\"macaddr\":\"" MACSTR "\",\"rssi\":%%d,\"age\":%%d}}", MAC2STR(sta));
	/* Format: {"sta":{"macaddr":"30:10:b3:6d:8d:ba","rssi":-29,"age":782}} */
	if (WARN_ON(sscanf(buf, format, &rssi, &age) != 2))	/* Flawfinder: ignore */
		return -1;

	memcpy(mon->macaddr, sta, 6);
	mon->rssi_avg = (int8_t)rssi;
	mon->last_seen = age;

	return 0;
}

int hostapd_ubus_iface_get_monitor_stas(const char *ifname, struct wifi_monsta *stas, int *num)
{
	char buf[1024];
	struct wifi_monsta *sta;
	char *line;
	char *key;
	char *p;
	int i;

	chrCmd(buf, sizeof(buf), "ubus call hostapd.%s sta_monitor_get", ifname);

	p = buf;
	i = 0;

	while ((line = strsep(&p, "\n"))) {
		if (WARN_ON(i >= *num))
			break;

		key = strstr(line, "\"macaddr\":");
		if (!key)
			continue;

		sta = &stas[i];
		if (sscanf(key, "\"macaddr\": \"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx\"",
			   &sta->macaddr[0],
			   &sta->macaddr[1],
			   &sta->macaddr[2],
			   &sta->macaddr[3],
			   &sta->macaddr[4],
			   &sta->macaddr[5]) != 6)
			continue;
		i++;
	}

	*num = i;
	return 0;
}

static int
hostapd_ubus_iface_subscribe_unsubscribe_frame(const char *ifname, uint8_t type, uint8_t stype, int sub)
{
	char buf[1024];
	char evt[64];
	char *res;

	if (type != WIFI_FRAME_MGMT)
		return -ENOTSUP;

	switch (stype) {
	case WIFI_FRAME_ASSOC_REQ:
		snprintf(evt, sizeof(evt), "raw-assoc");
		break;
	case WIFI_FRAME_REASSOC_REQ:
		snprintf(evt, sizeof(evt), "raw-reassoc");
		break;
	case WIFI_FRAME_PROBE_REQ:
		snprintf(evt, sizeof(evt), "raw-probe-req");
		break;
	case WIFI_FRAME_DISASSOC:
		snprintf(evt, sizeof(evt), "raw-disassoc");
		break;
	case WIFI_FRAME_AUTH:
		snprintf(evt, sizeof(evt), "raw-auth");
		break;
	case WIFI_FRAME_DEAUTH:
		snprintf(evt, sizeof(evt), "raw-deauth");
		break;
	case WIFI_FRAME_ACTION:
		snprintf(evt, sizeof(evt), "raw-action");
		break;
	default:
		return -ENOTSUP;
	}

	chrCmd(buf, sizeof(buf), "ubus call hostapd.%s %s '{\"event\":\"%s\"}'",
		ifname, sub ? "event_mask_add" : "event_mask_del", evt);

	chrCmd(buf, sizeof(buf), "ubus -S call hostapd.%s event_mask_get", ifname);
	res = strstr(buf, evt);

	if (sub && !res)
		return -1;

	if (!sub && res)
		return -1;

	return 0;
}

int hostapd_ubus_iface_subscribe_frame(const char *ifname, uint8_t type, uint8_t stype)
{
	return hostapd_ubus_iface_subscribe_unsubscribe_frame(ifname, type, stype, 1);
}

int hostapd_ubus_iface_unsubscribe_frame(const char *ifname, uint8_t type, uint8_t stype)
{
	return hostapd_ubus_iface_subscribe_unsubscribe_frame(ifname, type, stype, 0);
}

int hostapd_cli_iface_chan_switch(const char *ifname, struct chan_switch_param *param)
{
	char cmd[1024] = { 0 };

	/* <cs_count> <freq> [sec_channel_offset=] [center_freq1=] [center_freq2=] [bandwidth=] [blocktx] [ht|vht] */
	snprintf(cmd, sizeof(cmd), "chan_switch %d %d", param->count, param->freq);
	if (param->sec_chan_offset)
		snprintf(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), " sec_channel_offset=%d", param->sec_chan_offset);
	if (param->cf1)
		snprintf(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), " center_freq1=%d", param->cf1);
	if (param->cf2)
		snprintf(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), " center_freq2=%d", param->cf2);
	if (param->bandwidth)
		snprintf(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), " bandwidth=%d", param->bandwidth);
	if (param->blocktx)
		snprintf(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), " blocktx");
	if (param->ht)
		snprintf(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), " ht");
	if (param->vht)
		snprintf(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), " vht");
	if (param->he)
		snprintf(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), " he");

	return hostapd_cli_set(ifname, cmd, true);
}

int supplicant_cli_get_oper_std(const char *ifname, uint8_t *std)
{
	char buf[2048] = { 0 };
	char val[256] = { 0 };
	uint32_t freq = 0;
	uint8_t n_state = 0;
	uint8_t ac_state = 0;

	if (WARN_ON(!std))
		return -EINVAL;

	if (WARN_ON(wpa_cli_get(ifname, "status", buf, sizeof(buf))))
		return -1;

	if (get_param(buf, "freq", val, sizeof(val))) {
		libwifi_warn("%s freq parameter not found in buffer [%s]!\n",
		__func__, buf);
	}
	if (WARN_ON(sscanf(val, "%u", &freq) != 1)) {
		libwifi_warn("Failed to parse freq parameter\n");
	}

	if (get_param(buf, "ieee80211n", val, sizeof(val))) {
		libwifi_warn("%s ieee80211n parameter not found in buffer [%s]!\n",
		__func__, buf);
	}
	if (WARN_ON(sscanf(val, "%1hhu", &n_state) != 1)) {
		libwifi_warn("Failed to parse ieee80211n parameter\n");
	}

	if (get_param(buf, "ieee80211ac", val, sizeof(val))) {
		libwifi_warn("%s ieee80211ac parameter not found in buffer [%s]!\n",
		__func__, buf);
	}
	if (WARN_ON(sscanf(val, "%1hhu", &ac_state) != 1)) {
		libwifi_warn("Failed to parse ieee80211ac parameter\n");
	}

	*std = 0;

	if (ac_state) {
		*std |= WIFI_AC;
	}

	if (n_state) {
		*std |= WIFI_N;
		if (freq >= 5180)
			*std |= WIFI_A;
		return 0;
	}

	if (freq >= 5180) {
		*std |= WIFI_A;
		return 0;
	}

	if (freq <= 2484) {
		*std |= WIFI_B | WIFI_G;
		return 0;
	}

	return 0;
}

int supplicant_sta_info(const char *ifname, struct wifi_sta *info) {
	libwifi_warn("info->caps.valid = 0x%x\n",info->caps.valid);
	WARN_ON(supplicant_cli_get_oper_std(ifname, &info->oper_std));
	WARN_ON(supplicant_cli_get_sta_supported_security(ifname, &info->sec.supp_modes));
	WARN_ON(supplicant_cli_get_sta_security(ifname, &info->sec));

	return 0;
}

int supplicant_cli_get_sta_supported_security(const char *ifname, uint32_t *sec)
{
	char proto[512] = { 0 };
	char key_mgmt[512] = { 0 };
	char auth_alg[512] = { 0 };
	char group[512] = { 0 };
	char buf[32] = { 0 };
	int buildin_wep = 0;
	int buildin_sae = 0;
	int buildin_ft = 0;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	*sec = 0;

	if (WARN_ON(wpa_cli_get(ifname, "get_capability proto", proto, sizeof(proto))))
		return -1;
	if (WARN_ON(wpa_cli_get(ifname, "get_capability key_mgmt", key_mgmt, sizeof(key_mgmt))))
		return -1;
	if (WARN_ON(wpa_cli_get(ifname, "get_capability auth_alg", auth_alg, sizeof(auth_alg))))
		return -1;
	if (WARN_ON(wpa_cli_get(ifname, "get_capability group", group, sizeof(group))))
		return -1;

	if (Cmd(buf, sizeof(buf), "wpa_supplicant -vwep") == 0)
		buildin_wep = 1;

	if (Cmd(buf, sizeof(buf), "wpa_supplicant -vsae") == 0)
		buildin_sae = 1;

	if (Cmd(buf, sizeof(buf), "wpa_supplicant -v11r") == 0)
		buildin_ft = 1;

	if (strstr(key_mgmt, "WPA-PSK")) {
		if (strstr(proto, "WPA"))
			*sec |= BIT(WIFI_SECURITY_WPAPSK);
		if (strstr(proto, "RSN")) {
			*sec |= BIT(WIFI_SECURITY_WPA2PSK);
			if (buildin_ft)
				*sec |= BIT(WIFI_SECURITY_FT_WPA2PSK);
		}
		if (strstr(auth_alg, "SAE") && buildin_sae == 1) {
			*sec |= BIT(WIFI_SECURITY_WPA3PSK);
			if (buildin_ft)
				*sec |= BIT(WIFI_SECURITY_FT_WPA2PSK);
		}
	}

	if (strstr(key_mgmt, "WPA-EAP")) {
		if (strstr(proto, "WPA"))
			*sec |= BIT(WIFI_SECURITY_WPA);
		if (strstr(proto, "RSN"))
			*sec |= BIT(WIFI_SECURITY_WPA2);
		if (strstr(auth_alg, "SAE") && buildin_sae == 1)
			*sec |= BIT(WIFI_SECURITY_WPA3);
	}

	if (strstr(group, "WEP40") && buildin_wep == 1)
		*sec |= BIT(WIFI_SECURITY_WEP64);
	if (strstr(group, "WEP104") && buildin_wep == 1)
		*sec |= BIT(WIFI_SECURITY_WEP128);

	*sec |= BIT(WIFI_SECURITY_NONE);  /* always supported */

	return 0;
}

static int parse_wpa_cipher(const char *cipher_text, uint32_t *cipher)
{
	if (WARN_ON(!cipher))
		return -EINVAL;

	if (strstr(cipher_text, "WEP")) {
		*cipher |= CIPHER_WEP;
	}

	if (strstr(cipher_text, "TKIP")) {
		*cipher |= CIPHER_TKIP;
	}

	if (strstr(cipher_text, "CCMP-256")) {
		*cipher |= CIPHER_CCMP256;
	} else if (strstr(cipher_text, "CCMP")) {
		*cipher |= CIPHER_AES;
	}

	if (strstr(cipher_text, "GCMP-256")) {
		*cipher |= CIPHER_GCMP256;
	} else if (strstr(cipher_text, "GCMP")) {
		*cipher |= CIPHER_GCMP128;
	}

	if(*cipher == 0) {
		*cipher |= CIPHER_NONE;
	}

	return 0;
}

int supplicant_cli_get_sta_security(const char *ifname, struct wifi_sta_security *sec)
{
	char buf[2048] = { 0 };
	char key_mgmt[256] = { 0 };
	char cipher[256] = { 0 };

	if (WARN_ON(!sec))
		return -EINVAL;

	sec->curr_mode = 0;
	sec->group_cipher = 0;
	sec->pair_ciphers =0;

	if (WARN_ON(wpa_cli_get(ifname, "status", buf, sizeof(buf))))
		return -1;

	if (get_param(buf, "key_mgmt", key_mgmt, sizeof(key_mgmt))) {
		libwifi_warn("%s key_mgmt parameter not found in buffer [%s]!\n",
		__func__, buf);
	}

	if (strstr(key_mgmt, "WPA-PSK")) {
		sec->curr_mode |= BIT(WIFI_SECURITY_WPAPSK);
	}

	if (strstr(key_mgmt, "WPA2-PSK")) {
		sec->curr_mode |= BIT(WIFI_SECURITY_WPA2PSK);
	}

	if (strstr(key_mgmt, "WPA-EAP")) {
		sec->curr_mode |= BIT(WIFI_SECURITY_WPA);
	}

	if (strstr(key_mgmt, "SAE")) {
		sec->curr_mode |= BIT(WIFI_SECURITY_WPA3PSK);
	}

	if (strstr(key_mgmt, "FT-SAE")) {
		sec->curr_mode |= BIT(WIFI_SECURITY_FT_WPA3PSK);
	}

	if (sec->curr_mode == 0) {
		sec->curr_mode |= BIT(WIFI_SECURITY_NONE);
	}

	if (get_param(buf, "pairwise_cipher", cipher, sizeof(cipher))) {
		libwifi_warn("%s pairwise_cipher parameter not found in buffer [%s]!\n",
		__func__, buf);
	}

	parse_wpa_cipher(cipher, &sec->pair_ciphers);

	if (get_param(buf, "group_cipher", cipher, sizeof(cipher))) {
		libwifi_warn("%s group_cipher parameter not found in buffer [%s]!\n",
		__func__, buf);
	}

	parse_wpa_cipher(cipher, &sec->group_cipher);

	return 0;
}

int hostapd_cli_get_security_cap(const char *name, uint32_t *sec) {
	char buf[4096] = { 0 };
	char val[256] = { 0 };
	int wep = 0, sae = 0;
	uint32_t kf = 0;
	uint32_t cf = 0;
	int buildin_ft = 0;

	if (WARN_ON(!sec))
		return -EINVAL;

	*sec = 0;
	*sec |= BIT(WIFI_SECURITY_NONE);

	if (Cmd(buf, sizeof(buf), "hostapd -v11r") == 0)
		buildin_ft = 1;

	wep = Cmd(buf, sizeof(buf), "hostapd -vwep");

	if (wep == 0)
		*sec |= BIT(WIFI_SECURITY_WEP128);

	sae = Cmd(buf, sizeof(buf), "hostapd -vsae");
	if (sae == 0) {
		*sec |= BIT(WIFI_SECURITY_WPA3);
		*sec |= BIT(WIFI_SECURITY_WPA3PSK);
		if (buildin_ft)
			*sec |= BIT(WIFI_SECURITY_FT_WPA3PSK);
	}

	if (WARN_ON(hostapd_cli_get(name, "raw STATUS-DRIVER", buf, sizeof(buf))))
		return -1;

	if (get_param(buf, "capa.key_mgmt", val, sizeof(val))) {
		libwifi_warn("%s capa.key_mgmt parameter not found in buffer [%s]!\n",
			__func__, buf);
		return -1;
	}

	if (sscanf(val, "%x", &kf) != 1) {
		libwifi_warn("Failed to parse capa.key_mgmt parameter\n");
		return -1;
	}

	if (get_param(buf, "capa.enc", val, sizeof(val))) {
		libwifi_warn("%s capa.enc parameter not found in buffer [%s]!\n",
			__func__, buf);
		return -1;
	}

	if (sscanf(val, "%x", &cf) != 1) {
		libwifi_warn("Failed to parse capa.enc parameter\n");
		return -1;
	}

	if (wep == 0 && (cf & ENC_WEP40))
		*sec |= BIT(WIFI_SECURITY_WEP64);

	if (kf & KEY_MGMT_WPA)
		*sec |= BIT(WIFI_SECURITY_WPA);

	if (kf & KEY_MGMT_WPA2)
		*sec |= BIT(WIFI_SECURITY_WPA2);

	if (kf & KEY_MGMT_WPA_PSK)
		*sec |= BIT(WIFI_SECURITY_WPAPSK);

	if (kf & KEY_MGMT_WPA2_PSK) {
		*sec |= BIT(WIFI_SECURITY_WPA2PSK);
		if (buildin_ft)
			*sec |= BIT(WIFI_SECURITY_FT_WPA2PSK);
	}

	if (kf & KEY_MGMT_WPA_NONE)
		*sec |= BIT(WIFI_SECURITY_NONE);

	if (kf & KEY_MGMT_FT_PSK)
		*sec |= BIT(WIFI_SECURITY_FT_WPA2PSK);

	return 0;
}

static int parse_mac_address(char* buf, char *mac, size_t size) {
	char *pos;

	memset(mac, 0, size);

	pos = buf;
	while (*pos != '\0' && *pos != '\n')
		pos++;
	*pos = '\0';
	strncpy(mac, buf, size);

	return 0;
}

int hostapd_cli_get_4addr_parent(const char* ifname, char* parent) {
	char buf[2048] = { 0 };
	char cmd[64] = { 0 };
	char path[256] = { 0 };
	char phy[32] = { 0 };
	int ret = -1;
	struct dirent *p;
	char ifname_wds[16] = { 0 };
	char sta_mac[32] = { 0 };
	DIR *d;
	int i = 0;

	if (WARN_ON(nlwifi_get_phyname(ifname, phy)))
		return ret;

	snprintf(path, sizeof(path), "/sys/class/ieee80211/%s/device/net", phy);

	if (WARN_ON(!(d = opendir(path))))
		return ret;

	while ((p = readdir(d)) && (i < WIFI_IFACE_MAX_NUM)) {
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;

		memset(buf, 0, sizeof(buf));
		ret = hostapd_cli_get(p->d_name, "raw STA-FIRST", buf, sizeof(buf));

		if (ret || strlen(buf) == 0 || strstr(buf, "FAIL"))
			continue;

		memset(ifname_wds, 0, sizeof(ifname_wds));
		ret = get_param(buf, "ifname_wds", ifname_wds, sizeof(ifname_wds));
		if (!ret) {
			if (strcmp(ifname_wds, ifname) == 0) {
				memset(parent, 0, 16);
				memcpy(parent, p->d_name, 15);
				closedir(d);
				return 0;
			}
		}

		parse_mac_address(buf, sta_mac, sizeof(sta_mac));
		snprintf(cmd, sizeof(cmd), "raw STA-NEXT %s", sta_mac);

		memset(buf, 0, sizeof(buf));
		while (hostapd_cli_get(p->d_name, cmd, buf, sizeof(buf)) == 0) {
			if (strlen(buf) == 0 || strstr(buf, "FAIL"))
				return 0;
			memset(ifname_wds, 0, 16);
			ret = get_param(buf, "ifname_wds", ifname_wds, sizeof(ifname_wds));
			if (!ret) {
				if (strcmp(ifname_wds, ifname) == 0) {
					memset(parent, 0, 16);
					memcpy(parent, p->d_name, 15);
					closedir(d);
					return 0;
				}
			}

			parse_mac_address(buf, sta_mac, sizeof(sta_mac));
			snprintf(cmd, sizeof(cmd), "raw STA-NEXT %s", sta_mac);
			memset(buf, 0, sizeof(buf));
		}
		i++;
	}
	closedir(d);
	return -1;
}
