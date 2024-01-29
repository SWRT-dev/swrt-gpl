/*
 *
 * Copyright (C) 2023 SWRTdev. All rights reserved.
 * Copyright (C) 2023 paldier <paldier@hotmail.com>.
 *
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
#include "limits.h"
#include <ctype.h>
#include <sys/un.h>

#ifndef CONFIG_HOSTAPD_CTRL_IFACE_DIR
#define CONFIG_HOSTAPD_CTRL_IFACE_DIR "/var/run/hostapd"
#endif

#ifndef CONFIG_WPA_CTRL_IFACE_DIR
#define CONFIG_WPA_CTRL_IFACE_DIR "/var/run/wpa_supplicant"
#endif

#ifndef CONFIG_CTRL_IFACE_CLIENT_DIR
#define CONFIG_CTRL_IFACE_CLIENT_DIR "/tmp"
#endif

#ifndef CONFIG_CTRL_IFACE_CLIENT_PREFIX
#define CONFIG_CTRL_IFACE_CLIENT_PREFIX "wpa_ctrl_"
#endif

#define MAX_TRIES 5
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
			NMP_DEBUG("%s Number of connected stations %d exceeds expected %d!\n",
				__func__, (i + 1), *num_stas);
			return -EINVAL;
		}

		memcpy(&stas[i*6], tmp_sta, 6);
		i++;
	}
	*num_stas = i;
	return 0;
}

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
		NMP_DEBUG("%s %s '%s' command timed out.\n\n", ifname, __func__, cmd);
		return -2;
	} else if (ret < 0) {
		NMP_DEBUG("%s %s '%s' command failed.\n\n", ifname, __func__, cmd);
		return -1;
	}

	return 0;
}

static int ctrl_iface_get(const char *ifname, const char *cmd, char *out, size_t out_size, bool isHostapd)
{
	char cmdbuf[128] = {};
	char *p;

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
		NMP_DEBUG("%s %s failed\n", ifname, __func__);
		return -1;
	}

	NMP_DEBUG("%s %s get:\n%s\n", ifname, __func__, out);
	return 0;
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
		NMP_DEBUG("[%s] %s: None associated devices\n", ifname, __func__);
		return 0;
	}

	if (ctrl_buf_size < strlen(buf)) {
		NMP_DEBUG("[%s] %s: Output buffer too small %i < %i\n", ifname, __func__, ctrl_buf_size, strlen(buf));
		return -1;
	}

	do {
		if (!strlen(buf))
			break;
		if (strstr(buf, "FAIL")) {
			NMP_DEBUG("[%s] %s:Failed to get STA from hostapd\n", ifname, __func__);
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
			NMP_DEBUG("[%s] %s Collected sta mac's exceed output buffer size %i > %i\n",
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

	NMP_DEBUG("[%s] %s\n", ifname, __func__);

	if (!ifname || !strlen(ifname)) {
		NMP_DEBUG("[%s] %s Invalid args!\n", ifname, __func__);
		return -EINVAL;
	}
	if (!stas || *num_stas <= 0) {
		NMP_DEBUG("[%s] %s Invalid args!\n", ifname, __func__);
		return -EINVAL;
	}

	memset(stas, 0, *num_stas * 6);
	if (hostapd_ctrl_get_stas(ifname, ctrl_buf, sizeof(ctrl_buf)))
		return -1;

	if (hostapd_cli_get_sta_list(ctrl_buf, stas, num_stas))
		return -1;

	return 0;
}

int hostapd_cli_get(const char *ifname, const char *cmd, char *out, size_t out_size)
{
	return ctrl_iface_get(ifname, cmd, out, out_size, true);
}

#if 0
38:00:25:a9:58:b7
flags=[AUTH][ASSOC][AUTHORIZED]
aid=0
capability=0x0
listen_interval=0
supported_rates=0c 18 30
timeout_next=NULLFUNC POLL
dot11RSNAStatsSTAAddress=38:00:25:a9:58:b7
dot11RSNAStatsVersion=1
dot11RSNAStatsSelectedPairwiseCipher=00-0f-ac-4
dot11RSNAStatsTKIPLocalMICFailures=0
dot11RSNAStatsTKIPRemoteMICFailures=0
wpa=2
AKMSuiteSelector=00-0f-ac-2
hostapdWPAPTKState=11
hostapdWPAPTKGroupState=0
rx_packets=28584
tx_packets=109679
rx_bytes=2307896
tx_bytes=138356664
inactive_msec=0
signal=-41
rx_rate_info=0
tx_rate_info=63928
connected_time=692
supp_op_classes=80515354737475767778797a7b7c7d7e7f8081
#endif
int hostapd_cli_iface_get_sta_info(const char *ifname, uint8_t *addr, STA_INFO_TABLE *info)
{
	char sta_flags[128] = { 0 };
	char cmd[64] = { 0 };
	char buf[2048] = { 0 };
	uint8_t ie[128] = { 0 };
	int ret = 0;
	size_t ie_max = sizeof(ie);
	size_t ie_len;

	NMP_DEBUG("%s %s \n", ifname, __func__);

	snprintf(cmd, sizeof(cmd), "sta " MACSTR, MAC2STR(addr));

	if (WARN_ON(hostapd_cli_get(ifname, cmd, buf, sizeof(buf))))
		return -1;

	/* Make sure we set back addr */
	memcpy(info->macaddr, addr, sizeof(info->macaddr));


	return 0;
}
