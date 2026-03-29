/*
 * Copyright 2021-2022, ASUS
 * Copyright 2021-2026, SWRTdev
 * Copyright 2021-2026, paldier <paldier@hotmail.com>.
 * Copyright 2021-2026, lostlonger<lostlonger.g@gmail.com>.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bcmnvram.h>
#include <shutils.h>
#include <rc.h>
#include <shared.h>
#include "flash_mtd.h"
#include "ate.h"
#include "sysdeps.h"
#include <lantiq.h>

#define MAX_FRW 64
#define MACSIZE 12
#define	DEFAULT_SSID_2G	"ASUS" // asus app relies on this to identify
#define	DEFAULT_SSID_5G	"ASUS_5G"

#define CAP_HT20            0x0100
#define CAP_HT2040          0x0200
#define CAP_11ACVHT20       0x0400
#define CAP_11ACVHT40       0x0800
#define CAP_11ACVHT80       0x1000
#define CAP_11ACVHT80_80    0x2000
#define CAP_11ACVHT160      0x4000
#define CAP_CHWD_RSVD       0x8000
#define CAP_TXBF            0x010000
#define CAP_11AXAHE20       0x01000000
#define CAP_11AXGHE20       0x02000000
#define CAP_11AXAHE40       0x04000000
#define CAP_11AXGHE40       0x08000000
#define CAP_11AXAHE80       0x10000000
#define CAP_11AXAHE160      0x20000000
#define CAP_11AXAHE80_80    0x40000000

#define APSCAN_WLIST	"/tmp/apscan_wlist"

#define MKNOD(name,mode,dev)		if (mknod(name,mode,dev))		perror("## mknod " name)
const int pre_rssi[] = {-91, -91, -91};
#ifdef RTCONFIG_WIRELESSREPEATER
int sta_status = 0;
#endif

int bg = 0;

extern int is_if_up(char *ifname);

static int bw40cap2g = 1;
static int bw40cap[14 * 2] = {0};
static int bw80cap[7 * 2] = {0};
static int bw160cap[3 * 2] = {0};

struct chattr_s {
	int channel;
	int isupper;
	char *ht_str;
	int chan_low;
	int chan_high;
	int chan_cen;
	int pad;
	int bw40cap;
	int bw80cap;
	int bw160cap;
	int bw4320cap;
	int bw6480cap;
	int bw8640cap;
};

#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
struct bandx_defval_s {
	short pad;
	short qam;
	short intop;
	short shortgi;
	short stbc;
	short txbf;
	short vhtmubfer;
	short vhtsubfer;
	short he_mubfer;
	short he_subfer;
	short he_dl_ofdma;
	short he_ul_ofdma;
	short he_ul_mimo;
	short ldpc;
	short unhide_ssid;
	short beacon;
	short protmode;
	short shpreamble;
	short pad3;
	short frag;
	short dtim_period;
	short beacon_int;
	short wmm;
	short wmmparams;
	short pad4;
	short pad5;
	short pad6;
	short uapsd;
	short ap_isolate;
	short mcastenhance;
	short pad7;
	short ext_nss;
	short pad8;
};

struct bandx_defval_s bandx_defval[] = {
	{ 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 3, 0, 0, 1, 1, 2347, 2346, 1, 100, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0 }
};

struct bandx_defval_s *wldefval_tbl[] = { bandx_defval, bandx_defval, NULL };
#endif

char *get_vap_bridge()
{
	static char br_if[16];
	char *ifname = nvram_default_get("lan_ifname");
	if(ifname)
		strlcpy(br_if, ifname, sizeof(br_if));
	else
		strlcpy(br_if, "br0", sizeof(br_if));
	return br_if;
}

char *wlc_nvname(char *keyword)
{
	return wl_nvname(keyword, nvram_get_int("wlc_band"), -1);
}

int get_wpa_key_mgmt(int band, char *auth_mode, int mfp, char *wpa_key_mgmt, size_t len)
{
	int t_mfp = mfp;
	char prefix[7];
	if(!auth_mode || !*auth_mode){
		dbg("%s: Invalid parameter. (auth_mode [%s] km [%p] km_size %zu)\n", __func__, auth_mode, wpa_key_mgmt, len);
		return -1;
	}
	if(t_mfp > 2){
		if(!strcmp(auth_mode, "sae") || !strcmp(auth_mode, "wpa3"))
			t_mfp = 2;
		else
			t_mfp = !strcmp(auth_mode, "psk2sae") || !strcmp(auth_mode, "wpa2wpa3");
	}
	snprintf(prefix, sizeof(prefix), "wl%d_", band);
	*wpa_key_mgmt = '\0';
	if(nvram_contains_word("rc_support", "wpa3")){
		if(!strcmp(auth_mode, "sae")){
			strlcpy(wpa_key_mgmt, "SAE", len);
		}else if(!strcmp(auth_mode, "psk2sae")){
			if(t_mfp == 2)
				snprintf(wpa_key_mgmt, len, "%s SAE", "WPA-PSK-SHA256");
			else if(t_mfp == 1)
				snprintf(wpa_key_mgmt, len, "%s SAE", "WPA-PSK WPA-PSK-SHA256");
			else
				snprintf(wpa_key_mgmt, len, "%s SAE", "WPA-PSK");
		}else if(!strcmp(auth_mode, "owe"))
			strlcpy(wpa_key_mgmt, "OWE", len);
	}


	if(!*wpa_key_mgmt && strcmp(auth_mode, "open") && strcmp(auth_mode, "shared")){
		if(!strcmp(auth_mode, "suite-b"))
			strlcpy(wpa_key_mgmt, "WPA-EAP-SUITE-B-192", len);
		else if(!strcmp(auth_mode, "wpa3"))
			strlcpy(wpa_key_mgmt, "WPA-EAP-SHA256", len);
		else if(!strcmp(auth_mode, "wpa2wpa3")){
			if(t_mfp == 2)
				strlcpy(wpa_key_mgmt, "WPA-EAP-SHA256", len);
			else if(t_mfp == 1)
				strlcpy(wpa_key_mgmt, "WPA-EAP WPA-EAP-SHA256", len);
			else
				strlcpy(wpa_key_mgmt, "WPA-EAP", len);
		}
	}

	if(!*wpa_key_mgmt){
		if(!strcmp(auth_mode, "psk2") || !strcmp(auth_mode, "pskpsk2")){
			if(t_mfp == 2)
				strlcpy(wpa_key_mgmt, "WPA-PSK-SHA256", len);
			else if(t_mfp == 1)
				strlcpy(wpa_key_mgmt, "WPA-PSK WPA-PSK-SHA256", len);
			else
				strlcpy(wpa_key_mgmt, "WPA-PSK", len);
		}else if(!strcmp(auth_mode, "wpa2") || !strcmp(auth_mode, "wpawpa2") || !strcmp(auth_mode, "wpa")){
			if(t_mfp == 2)
				strlcpy(wpa_key_mgmt, "WPA-EAP-SHA256", len);
			else if(t_mfp == 1)
				strlcpy(wpa_key_mgmt, "WPA-EAP WPA-EAP-SHA256", len);
			else
				strlcpy(wpa_key_mgmt, "WPA-EAP", len);
		}
		if(!*wpa_key_mgmt){
			if(strcmp(auth_mode, "open") && strcmp(auth_mode, "shared"))
				dbg("%s: Unknown auth_mode [%s]!\n", __func__, auth_mode);
			if(t_mfp == 2)
				strlcpy(wpa_key_mgmt, "WPA-PSK-SHA256", len);
			else if(t_mfp == 1)
				strlcpy(wpa_key_mgmt, "WPA-PSK WPA-PSK-SHA256", len);
			else
				strlcpy(wpa_key_mgmt, "WPA-PSK", len);
		}
	}
	return 0;
}

static unsigned char nibble_hex(char *c)
{
	int val;
	char tmpstr[3];

	tmpstr[2] = '\0';
	memcpy(tmpstr, c, 2);
	val = strtoul(tmpstr, NULL, 16);
	return val;
}

static int atoh(const char *a, unsigned char *e)
{
	char *c = (char *)a;
	int i = 0;

	memset(e, 0, MAX_FRW);
	for (i = 0; i < MAX_FRW; ++i, c += 3) {
		if (!isxdigit(*c) || !isxdigit(*(c + 1)) || isxdigit(*(c + 2)))	// should be "AA:BB:CC:DD:..."
			break;
		e[i] = (unsigned char)nibble_hex(c);
	}

	return i;
}

char * htoa(const unsigned char *e, char *a, int len)
{
	char *c = a;
	int i;

	for (i = 0; i < len; i++) {
		if (i)
			*c++ = ':';
		c += sprintf(c, "%02X", e[i] & 0xff);
	}
	return a;
}

int FREAD(unsigned int addr_sa, int len)
{
	unsigned char buffer[MAX_FRW];
	char buffer_h[128];
	memset(buffer, 0, sizeof(buffer));
	memset(buffer_h, 0, sizeof(buffer_h));

	if (FRead(buffer, addr_sa, len) < 0)
		dbg("FREAD: Out of scope\n");
	else {
		if (len > MAX_FRW)
			len = MAX_FRW;
		htoa(buffer, buffer_h, len);
		puts(buffer_h);
	}
	return 0;
}

/*
 * 	write str_hex to offset da
 *	console input:	FWRITE 0x45000 00:11:22:33:44:55:66:77
 *	console output:	00:11:22:33:44:55:66:77
 *
 */
int FWRITE(const char *da, const char *str_hex)
{
	unsigned char ee[MAX_FRW];
	unsigned int addr_da;
	int len;

	addr_da = strtoul(da, NULL, 16);
	if (addr_da && (len = atoh(str_hex, ee))) {
		FWrite(ee, addr_da, len);
		FREAD(addr_da, len);
	}
	return 0;
}

int check_macmode(const char *str)
{

	if ((!str) || (!strcmp(str, "")) || (!strcmp(str, "disabled"))) {
		return 0;
	}

	if (strcmp(str, "allow") == 0) {
		return 1;
	}

	if (strcmp(str, "deny") == 0) {
		return 2;
	}
	return 0;
}

void gen_macmode(int mac_filter[], int band, char *prefix)
{
	mac_filter[0] = check_macmode(nvram_pf_safe_get(prefix, "macmode"));
	_dprintf("mac_filter[0] = %d\n", mac_filter[0]);
}

int bw40_channel_check(int band,char *ext)
{
	int ch, ret = 1;
	char prefix[sizeof("wlX_XXXXXX")];

	if (band < 0 || band >= MAX_NR_WL_IF) {
		dbg("%s: unknown wl%d band!\n", __func__, band);
		return 0;
	}

	snprintf(prefix, sizeof(prefix), "wl%d_", band);
	ch = nvram_pf_get_int(prefix, "channel");
	if (!ch)
		return ret;	/* pass */

	switch (band) {
	case WL_2G_BAND:
		if ((ch == 1) || (ch == 2) ||(ch == 3)||(ch == 4)) {
			if (!strcmp(ext,"MINUS")) {
				dbG("stage 1: a  mismatch between %s mode and ch %d => fix mode\n", ext, ch);
				sprintf(ext,"PLUS");
			}
		} else if (ch >= 8) {
			if (!strcmp(ext,"PLUS")) {
				dbG("stage 2: a  mismatch between %s mode and ch %d => fix mode\n", ext, ch);
				sprintf(ext,"MINUS");
			}	   
		}
		//ch5,6,7:both
		break;
	case WL_5G_BAND:	/* fall-through */
//	case WL_5G_2_BAND:
			if ((ch == 36) || (ch == 44) || (ch == 52) || (ch == 60) ||
			    (ch == 100) || (ch == 108) ||(ch == 116) || (ch == 124) ||
			    (ch == 132) || (ch == 149) || (ch ==157))
			{
				if(!strcmp(ext, "MINUS")) {
					dbG("stage 1: a  mismatch between %s mode and ch %d => fix mode\n", ext, ch);
					sprintf(ext,"PLUS");
				}
			}
			else if ((ch == 40) || (ch == 48) || (ch == 56) || (ch == 64) ||
				 (ch == 104) || (ch == 112) || (ch == 120) || (ch == 128) ||
				 (ch == 136) || (ch == 153) ||(ch == 161))
			{
				if(!strcmp(ext, "PLUS")) {
					dbG("stage 2: a  mismatch between %s mode and ch %d => fix mode\n", ext, ch);
					sprintf(ext,"MINUS");
				}
			}
		break;
	}

	return ret;
}

#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
int get_bw_via_channel(int band, int channel, struct chattr_s *pchattr, int max_subnet, int bw160)
#else
int get_bw_via_channel(int band, int channel)
#endif
{
	int wl_bw;
	char prefix[32];

	int sw_mode = sw_mode();
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	int autochan = 0;
#endif

	if (band < 0 || band >= MAX_NR_WL_IF)
		return 0;

	if (sw_mode == SW_MODE_REPEATER){
		snprintf(prefix, sizeof(prefix), "wl%d.1_", band);
	}
	else{
		snprintf(prefix, sizeof(prefix), "wl%d_", band);
	}
	wl_bw = nvram_pf_get_int(prefix, "bw");
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(band && channel <= 165){
		autochan = channel == 0;
		if(wl_bw != 1 && channel == 0){
			if(bw160 <= 0 && wl_bw == 5)
				wl_bw = 4;
			if(wl_bw == 4 && max_subnet <= 1)
				wl_bw = 3;
		}
		if(wl_bw != 1)
			autochan = 1;
		if(!autochan){
			if(pchattr){
				if(pchattr->bw160cap == 1)
					wl_bw = 5;
				else
					wl_bw = 4;
				if(max_subnet <= 1)
					wl_bw = 3;
				if(pchattr->bw80cap != 1)
					wl_bw = 2;
				if(pchattr->bw40cap != 1)
					wl_bw = 0;
			}else
				wl_bw = 1;
		}
	}
#else
	if(band == 0 || channel < 14 || channel > 165 || wl_bw != 1)  {
		return wl_bw;
	}

	if(channel == 116 || channel == 140 || channel >= 165) {
		return 0;	// 20 MHz
	}
	if(channel == 132 || channel == 136) {
		if(wl_bw == 0)
			return 0;
		return 2;		// 40 MHz
	}

	//check for TW band2
	snprintf(prefix, sizeof(prefix), "wl%d_", band);
	if(nvram_pf_match(prefix, "country_code", "TW")) {
		if(channel == 56)
			return 0;
		if(channel == 60 || channel == 64) {
			if(wl_bw == 0)
				return 0;
			return 2;		// 40 MHz
		}
	}
#endif
	return wl_bw;
}

char * conv_iwconfig_essid(char *ssid_in, char *ssid_out)
{
	int i, len;
	if (ssid_out == NULL)
		return NULL;

	*ssid_out = '\0';
	if(ssid_in != NULL && (len = strlen(ssid_in)) <= 32) {
		char *p = ssid_out;
		for (i = 0; i < len; i++) {
			p += sprintf(p, "\\%c", ssid_in[i]);
		}
		return ssid_out;
	}
	return NULL;
}

void gen_lantiq_wifi_cfgs(void)
{
	pid_t pid;
	char *argv[]={"/sbin/delay_exec", "1", "nice -10 /tmp/postwifi.sh", NULL};
	char path2[50];
	char wif[256], *next, lan_ifnames[512];
	FILE *fp,*fp2;
	char tmp[100], prefix[sizeof("wlXXXXXXX_")], main_prefix[sizeof("wlXXXXX_")];
	int led_onoff[4] = { LED_ON, LED_ON }, unit = -1, sunit = 0;
	int sidx = 0;
	/* Array index: 0 -> 2G, 1 -> 5G
	 * bit: MBSSID index
	 */
	unsigned int m, wl_mask[4];
	char conf_path[sizeof("/etc/Wireless/conf/hostapd_athXXX.confYYYYYY")];
	char pid_path[sizeof("/var/run/hostapd_athXXX.pidYYYYYY")];
	char entropy_path[sizeof("/var/run/entropy_athXXX.binYYYYYY")];
	char path[sizeof(NAWDS_SH_FMT) + 6];
	int i, rate = 0;
	int max_sunit;
	int mrate_tbl[] = {-1, 1000, 2000, 5500, 6000, 9000, 11000, 12000, 18000, 24000, 36000, 48000, 54000,
			130000, 65000, 13000, 195000, 13000, 26000, 39000, 0};
#ifdef RTCONFIG_WIRELESSREPEATER
	int wlc_band = nvram_get_int("wlc_band");
#endif
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	int wsup_dbg = nvram_get_int("wsup_dbg");
#endif

	if (!(fp = fopen("/tmp/prewifi.sh", "w+")))
		return;
	if (!(fp2 = fopen("/tmp/postwifi.sh", "w+")))
		return;

	fprintf(fp, "#!/bin/sh -p\n");
	fprintf(fp2, "#!/bin/sh -p\n");
	deinit_all_vaps(0);
	memset(wl_mask, 0, sizeof(wl_mask));

	strlcpy(lan_ifnames, nvram_safe_get("lan_ifnames"), sizeof(lan_ifnames));
	foreach (wif, lan_ifnames, next) {
		SKIP_ABSENT_FAKE_IFACE(wif);

		if (!guest_wlif(wif) && (unit = get_wifi_unit(wif)) >= 0 && unit < ARRAY_SIZE(led_onoff)) {
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			if (nvram_pf_match(prefix, "radio", "0"))
				led_onoff[unit] = LED_OFF;
#if defined(RTCONFIG_WIRELESSREPEATER)
			if (unit == wlc_band && repeater_mode())
				led_onoff[unit] = LED_ON;
#endif
		}

		if (!strncmp(wif, WIF_2G, strlen(WIF_2G))) {
			if (!guest_wlif(wif)) {
				/* 2.4G */
				wl_mask[0] |= 1;
				gen_lantiq_config(0, 0);
			} else {
				/* 2.4G guest */
				sunit = get_wlsubnet(0, wif);
				if (sunit > 0) {
					sidx = atoi(wif + strlen(WIF_2G));
					wl_mask[0] |= 1 << sunit;
					gen_lantiq_config(0, sidx);
				}else
					dbg("%s: Unknown guest network interface [%s], band %d\n", __func__, wif, unit);
			}
		} else if (!strncmp(wif, WIF_5G, strlen(WIF_5G))) {
			if (!guest_wlif(wif)) {
				/* 5G */
				wl_mask[1] |= 1;
				gen_lantiq_config(1, 0);
			} else {
				/* 5G guest */
				sunit = get_wlsubnet(1, wif);
				if (sunit > 0) {
					sidx = atoi(wif + strlen(WIF_5G));
					wl_mask[1] |= 1 << sunit;
					gen_lantiq_config(1, sidx);
				}else
					dbg("%s: Unknown guest network interface [%s], band %d\n", __func__, wif, unit);
			}
		} else
			continue;

#ifdef RTCONFIG_WIRELESSREPEATER
		if (strcmp(wif, STA_2G) && strcmp(wif, STA_5G))
#endif
		{
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			if(unit != -1 && unit <= MAX_NR_WL_IF)
				fprintf(fp, "/etc/Wireless/sh/prewifi_%s.sh %s\n", get_vphyifname(unit), bg ? "&" :"");
#endif
			fprintf(fp, "/etc/Wireless/sh/prewifi_%s.sh %s\n",wif, bg ? "&" :"");
			fprintf(fp2, "/etc/Wireless/sh/postwifi_%s.sh %s\n",wif, bg ? "&" : "");
		}
	}
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(nvram_match("w_Setting", "0") && sw_mode() == SW_MODE_ROUTER){
		char ifname[IFNAMSIZ];
		for(i = 0; i < MAX_NR_WL_IF; ++i){
			SKIP_ABSENT_BAND(i);
			get_wlxy_ifname(i, 0, ifname);
			snprintf(pid_path, sizeof(pid_path), "/var/run/hostapd_cli-%s.pid", ifname);
			if(f_exists(pid_path))
				kill_pidfile(pid_path);
			if(nvram_get_int("wps_multiband") || nvram_get_int("wps_band_x") == i)
				fprintf(fp2, "hostapd_cli -i %s -P %s -a %s -B\n", ifname, pid_path, "/sbin/vap_evhandler");
		}
	}
#endif
	if ((wl_mask[0]&0xfe) || (wl_mask[1]&0xfe) || (wl_mask[2]&0xfe))
		fprintf(fp2, "sleep 4\n");

	for (unit = 0; unit < MAX_NR_WL_IF; ++unit) {
		SKIP_ABSENT_BAND(unit);
		if(sw_mode() == SW_MODE_REPEATER){
			snprintf(main_prefix, sizeof(main_prefix), "wl%d.1_", unit);
		}else{
			snprintf(main_prefix, sizeof(main_prefix), "wl%d_", unit);
		}
		i = nvram_pf_get_int(main_prefix, "mrate_x");
next_mrate:
		switch (i++) {
			default:
			case 0:		/* Driver default setting: Disable, means automatic rate instead of fixed rate
				 * Please refer to #ifdef MCAST_RATE_SPECIFIC section in
				 * file linuxxxx/drivers/net/wireless/rtxxxx/common/mlme.c
				 */
				goto next_mrate;
				//break;
			case 1:		/* Legacy CCK 1Mbps */
				if(unit)
					goto next_mrate;
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 2:		/* Legacy CCK 2Mbps */
				if(unit)
					goto next_mrate;
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 3:		/* Legacy CCK 5.5Mbps */
				if(unit)
					goto next_mrate;
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 4:		/* Legacy OFDM 6Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 5:		/* Legacy OFDM 9Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 6:		/* Legacy CCK 11Mbps */
				if(unit)
					goto next_mrate;
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 7:		/* Legacy OFDM 12Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 8:		/* Legacy OFDM 18Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 9:		/* Legacy OFDM 24Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 10:		/* Legacy OFDM 36Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 11:		/* Legacy OFDM 48Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 12:		/* Legacy OFDM 54Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 13:		/* HTMIX 130/300Mbps 2S */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 14:		/* HTMIX 6.5/15Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 15:		/* HTMIX 13/30Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 16:		/* HTMIX 19.5/45Mbps */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 17:		/* HTMIX 13/30Mbps 2S */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 18:		/* HTMIX 26/60Mbps 2S */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 19:		/* HTMIX 39/90Mbps 2S */
				if(unit)
					rate = mrate_tbl[i];
				else if(!nvram_pf_match(prefix, "rateset", "ofdm") || mrate_tbl[i] <= 5999
					|| mrate_tbl[i] == 11000)
					rate = mrate_tbl[i];
				break;
			case 20:
				/* Choose multicast rate base on mode, encryption type, and IPv6 is enabled or not. */
				rate = 65000;
#ifdef RTCONFIG_IPV6
				if(get_ipv6_service() && (
#ifdef RTCONFIG_6RELAYD
					get_ipv6_service() == IPV6_PASSTHROUGH 
#endif
					|| nvram_get_int(ipv6_nvname("ipv6_radvd"))))
					rate = !strncmp(main_prefix, "wl0", 3) ? 12000 : 13000;
#endif
				if(nvram_pf_match(main_prefix, "nmode_x", "2") ||	/* legacy mode */
					strstr(nvram_pf_safe_get(main_prefix, "crypto"), "tkip")) {	/* tkip */
					/* In such case, choose OFDM instead of HTMIX */
					rate = 24000;
				}
				break;
		}
		for(sunit = 0; !mediabridge_mode() && rate > 0 && sunit <= num_of_mssid_support(unit); sunit++){
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, sunit);
			if(!sunit || nvram_pf_match(prefix, "bss_enabled", "1")){
				get_wlxy_ifname(unit, sunit, wif);
				fprintf(fp2, "(sleep %d ; %s %s mcast_rate %d) &\n", 10, IWPRIV, wif, rate);
			}
		}
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		get_wlxy_ifname(unit, 0, wif);
		snprintf(path, sizeof(path), NAWDS_SH_FMT, wif);
		if(nvram_pf_invmatch(main_prefix, "radio", "0") && f_exists(path))
			fprintf(fp2, "%s &\n", path);
#else
		if (unit != WL_60G_BAND && nvram_pf_match(main_prefix, "channel", "0"))
			fprintf(fp2, "iwconfig %s channel auto\n", get_wlxy_ifname(unit, 0, wif));
#endif
	}
	if(mediabridge_mode())
		fprintf(fp, "rc rc_service restart_mcast &\n");
	else
		fprintf(fp2, "rc rc_service restart_mcast &\n");
#if defined(LANTIQ_BSD)
	if(nvram_match("smart_connect_x", "1") && !repeater_mode())
		fprintf(fp2, "rc rc_service restart_bsd &\n");
#endif

#if defined(RTCONFIG_WPS_ALLLED_BTN)
	if (nvram_match("AllLED", "1")) {
#endif
		fprintf(fp2, "[ -e /sys/class/net/%s ] && led_ctrl %d %d\n", WIF_2G, LED_2G, led_onoff[0]);
#if defined(RTCONFIG_HAS_5G)
		fprintf(fp2, "[ -e /sys/class/net/%s ] && led_ctrl %d %d\n", WIF_5G, LED_5G, led_onoff[1]);
#endif
#if defined(RTCONFIG_WPS_ALLLED_BTN)
	}
#endif
	fprintf(fp2, "if [ -e /sys/class/net/%s ] ", WIF_2G);
#if defined(RTCONFIG_HAS_5G)
	fprintf(fp2, "&& [ -e /sys/class/net/%s ] ", WIF_5G);
#endif
	fprintf(fp2, "; then\n");
	fprintf(fp2, "    nvram set %s=1\n", WLREADY);
	fprintf(fp2, "else\n");
	fprintf(fp2, "    logger Wireless not ready!\n");
	fprintf(fp2, "fi\n");
#if defined(RTCONFIG_NOTIFICATION_CENTER) || defined(RTCONFIG_CFGSYNC)
	fprintf(fp2, "weventd &\n");
#endif
#ifdef RTCONFIG_HAPDEVENT
	fprintf(fp2, "killall hapdevent\n");
	fprintf(fp2, "delay_exec 5 hapdevent &\n");
#endif

	fclose(fp);
	fclose(fp2);
	chmod("/tmp/prewifi.sh",0777);
	chmod("/tmp/postwifi.sh",0777);

	for (unit = 0; unit < ARRAY_SIZE(wl_mask); ++unit) {
		max_sunit = num_of_mssid_support(unit);
		for (sunit = 0; sunit <= max_sunit; ++sunit) {
			get_wlxy_ifname(unit, sunit, wif);
			sprintf(path, "/sys/class/net/%s", wif);
			if (d_exists(path))
				ifconfig(wif, 0, NULL, NULL);
			sprintf(pid_path, "/var/run/hostapd_%s.pid", wif);
			if (!f_exists(pid_path))
				continue;

			kill_pidfile_tk(pid_path);
		}
	}

#if defined(RTCONFIG_NOTIFICATION_CENTER)
	killall_tk("weventd");
#endif
	doSystem("/tmp/prewifi.sh");

#ifdef RTCONFIG_WIRELESSREPEATER
	if(sw_mode() == SW_MODE_REPEATER
#ifdef RTCONFIG_AMAS
		|| aimesh_re_node()
#elif defined(RTCONFIG_WISP)
		|| wisp_mode()
#endif
	){
		for (i = 0; i < MAX_NR_WL_IF; ++i) {
			int nmode, shortgi;
			char lan_if[16];
			char *sta = get_staifname(i);
			char pid_file[sizeof("/var/run/wifi-staX.pidXXXXXX")];
			char conf[sizeof("/etc/Wireless/conf/wpa_supplicant-STAX.confXXXXXX")];
#if defined(RTCONFIG_SINGLE_HOSTAPD)
			char *dbg_str;
#endif

			SKIP_ABSENT_BAND(i);
			if(i != wlc_band
#ifdef RTCONFIG_AMAS
				&& nvram_match("re_mode", "1")
#endif
			)
				continue;
#ifdef RTCONFIG_AMAS
			if(strstr(nvram_safe_get("sta_ifnames"), sta) && strstr(nvram_safe_get("skip_ifnames"), sta))
				continue;
#endif
			snprintf(lan_if, sizeof(lan_if), "%s", nvram_get("lan_ifname")? : "br0");

			snprintf(prefix, sizeof(prefix), "wl%d_", i);
			nmode = nvram_pf_get_int(prefix, "nmode_x");
			shortgi = (nmode != 2) ? !!nvram_pf_get_int(prefix, "HT_GI") : 0;

			get_wpa_supplicant_pidfile(sta, pid_file, sizeof(pid_file));
			if(kill_pidfile(pid_file) == 0)
				cprintf("## WARNING: %s is alive ##\n", pid_file);
#if defined(RTCONFIG_SINGLE_HOSTAPD)
			snprintf(path2, sizeof(path2), "/etc/Wireless/sh/postwifi_%s.sh", wif);
			if (!(fp = fopen(path2, "a")))
				continue;
			if(wsup_dbg == 1)
				dbg_str = "-d";
			else if(wsup_dbg == 2)
				dbg_str = "-dd";
			else if(wsup_dbg >= 3)
				dbg_str = "-ddd";
			else
				dbg_str = "";
			snprintf(conf, sizeof(conf), "/etc/Wireless/conf/wpa_supplicant-%s.conf", sta);
			fprintf(fp, "/usr/bin/wpa_supplicant -g /var/run/wpa_supplicantglobal -B -P %s %s\n", pid_file, dbg_str);
			fprintf(fp, "sleep 1\n");
			fprintf(fp, "/usr/bin/wpa_cli -g /var/run/wpa_supplicantglobal interface_add %s %s nl80211 /var/run/wpa_supplicant-%s %s\n", sta, conf, sta, lan_if);
#else
			snprintf(conf, sizeof(conf), "/etc/Wireless/conf/wpa_supplicant-%s.conf", sta);
			eval("/usr/bin/wpa_supplicant", "-B", "-P", pid_file, "-D", "nl80211", "-i", sta, "-b", lan_if, "-c", conf);
#endif
			{
				extern char * conv_iwconfig_essid(char *ssid_in, char *ssid_out);
				char prefix_wlc[16];
				if (sw_mode() == SW_MODE_REPEATER)
					snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc_");
#if defined(RTCONFIG_AMAS)
				else if (nvram_match("re_mode", "1"))
					snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc%d_", i);
#endif
#if defined(RTCONFIG_WISP)
				else if(wisp_mode())
					snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc_");
#endif
				if (conv_iwconfig_essid(nvram_pf_get(prefix_wlc, "ssid"), tmp) != NULL) {
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
					fprintf(fp, "iwconfig %s essid -- %s\n", sta, tmp);
#else
					doSystem("iwconfig %s essid -- %s", sta, tmp);
#endif
				}
			}
			fprintf(fp, "ifconfig %s up\n", sta);
			fprintf(fp, "delay_exec 15 /usr/bin/wpa_cli -p /var/run/wpa_supplicant-%s -i %s enable_network all&\n", sta, sta);
			fclose(fp);
		}
	}
#endif
#ifdef RTCONFIG_PROXYSTA
	if (!mediabridge_mode())
#endif
	{
		/* Router, access-point, and repeater mode.
		 * Run postwifi.sh 4 seconds later.
		 */
#if defined(RTCONFIG_SINGLE_HOSTAPD)
		char *hostapd_argv[] = {"hostapd", "-g", QHOSTAPD_CTRL_IFACE, "-B", "-P", pid_path, NULL};
		snprintf(pid_path, sizeof(pid_path), "%s", "/var/run/hostapd_global.pid");
		_eval(hostapd_argv, NULL, 0, NULL);
#endif
		for(unit = 0; unit < ARRAY_SIZE(wl_mask); ++unit){
			snprintf(main_prefix, sizeof(main_prefix), "wl%d_", unit);
			m = wl_mask[unit];
			for(sunit = 0, sidx = 0; m > 0; ++sunit, ++sidx, m >>= 1){
				snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, sunit);
				if(sidx > 0 && !nvram_match(wl_nvname("bss_enabled", unit, sunit), "1")){
					sidx--;
					continue;
				}

				get_wlxy_ifname(unit, sidx, wif);
				snprintf(path2, sizeof(path2), "/etc/Wireless/sh/postwifi_%s.sh", wif);
				if(nvram_match("skip_gen_lantiq_config", "1") && f_exists(path2))
					continue;

				if (!f_exists(path2))
					continue;
				if (!(fp = fopen(path2, "a")))
					continue;

#if defined(RTCONFIG_SINGLE_HOSTAPD)
				fprintf(fp, "%s %s rrm 1\n", IWPRIV, wif);
				snprintf(conf_path, sizeof(conf_path), "/etc/Wireless/conf/hostapd_%s.conf", wif);
				if(sunit)
					fprintf(fp, "[ `iwpriv %s get_mode|cut -f 2` != `iwpriv %s get_mode|cut -f 2` ] && sleep 3\n", get_wififname(unit), wif);
				fprintf(fp, "/usr/bin/wpa_cli -g %s raw ADD bss_config=%s:%s\n", QHOSTAPD_CTRL_IFACE, wif, conf_path);
#else
					if (!strcmp(nvram_safe_get(wl_nvname("auth_mode_x", unit, sunit)), "shared")) {
						fclose(fp);
						continue;
					}
					else if (!strcmp(nvram_safe_get(wl_nvname("auth_mode_x", unit, sunit)), "open") &&
							((!sunit && !nvram_get_int("wps_enable")) || sunit)) {
						fclose(fp);
						continue;
					}


				sprintf(conf_path, "/etc/Wireless/conf/hostapd_%s.conf", wif);
				sprintf(pid_path, "/var/run/hostapd_%s.pid", wif);
				sprintf(entropy_path, "/var/run/entropy_%s.bin", wif);
				fprintf(fp, "hostapd -B -P %s -e %s %s\n", pid_path, entropy_path, conf_path);
#endif
				/* Hostapd will up VAP interface automatically.
				 * So, down VAP interface if radio is not on and
				 * not to up VAP interface anymore.
				 */
				if (nvram_pf_match(main_prefix, "radio", "0")) {
					fprintf(fp, "ifconfig %s down\n", wif);
				}
				/* WPS-OOB not work if wps_ap_pin disabled. */
				if (!nvram_match("w_Setting", "0"))
					fprintf(fp, "hostapd_cli -i %s wps_ap_pin disable\n", wif);
				fclose(fp);
			}
		}

		argv[1] = "4";
		_eval(argv, NULL, 0, &pid);

		//system("/tmp/postwifi.sh");
		//sleep(1);
	} /* !mediabridge_mode() */

#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_BLINK_LED)
	if (repeater_mode() || mediabridge_mode())
		update_wifi_led_state_in_wlcmode();
#endif
}

void gen_hostapd_wps_config(FILE *fp_h, int band, int subnet, char *br_if)
{
	char *p;
	char prefix[8];

	snprintf(prefix, sizeof(prefix), "wl%d_", band);

	fprintf(fp_h, "# Wi-Fi Protected Setup (WPS)\n");

	if(!subnet && nvram_get_int("wps_enable")){
		char *uuid = nvram_safe_get("uuid");
		if(nvram_pf_match(prefix, "mode_x", "1")){
			fprintf(fp_h, "wps_state=0\n");
		}else if(nvram_match("w_Setting", "0")){
			fprintf(fp_h, "wps_state=1\n");
		}else{
			fprintf(fp_h, "wps_state=2\n");
			fprintf(fp_h, "ap_setup_locked=1\n");
		}
		if (uuid && strlen(uuid) == 36)
			fprintf(fp_h, "uuid=%s\n", uuid);
	}else{
		/* Turn off WPS on guest network. */
		fprintf(fp_h, "wps_state=0\n");
	}

	fprintf(fp_h, "wps_independent=1\n");
	fprintf(fp_h, "device_name=ASUS Router\n");
	fprintf(fp_h, "manufacturer=ASUSTek Computer Inc.\n");
	fprintf(fp_h, "model_name=%s\n", nvram_safe_get("productid"));
	fprintf(fp_h, "model_number=%s\n", ((p = nvram_safe_get("odmpid")) && *p != '\0')? p : nvram_safe_get("productid"));
	fprintf(fp_h, "serial_number=%s\n", get_lan_hwaddr());
	fprintf(fp_h, "device_type=6-0050F204-1\n");
	fprintf(fp_h, "config_methods=push_button display virtual_display virtual_push_button physical_push_button label\n");
	fprintf(fp_h, "pbc_in_m1=1\n");
	fprintf(fp_h, "ap_pin=%s\n", nvram_safe_get("secret_code"));
	fprintf(fp_h, "upnp_iface=%s\n",br_if);
	fprintf(fp_h, "friendly_name=WPS Access Point\n");
	fprintf(fp_h, "manufacturer_url=http://www.asus.com\n");
	fprintf(fp_h, "model_description=ASUS Router\n");
	fprintf(fp_h, "model_url=http://www.asus.com\n");
	//fprintf(fp_h, "wps_rf_bands=ag\n"); /* according to spec */
	fprintf(fp_h, "wps_rf_bands=%c\n",band?'a':'g');
	if(nvram_pf_match(prefix, "auth_mode_x", "sae") || nvram_pf_match(prefix, "auth_mode_x", "owe")){
		fprintf(fp_h, "ieee80211w=%d\n", 2);
	}else if(nvram_pf_match(prefix, "auth_mode_x", "psk2sae")){
		fprintf(fp_h, "ieee80211w=%d\n", 1);
	}else{
		fprintf(fp_h, "ieee80211w=%d\n", 0);
	}
}

#ifdef RTCONFIG_WIRELESSREPEATER
int copy_rpt_params_to_wl(char *wl_prefix, char *wlc_prefix)
{
	int i;
	char *str, *auth_mode;

	str = nvram_pf_get(wlc_prefix, "ssid");
	if(str && *str)
		nvram_set("wl_ssid", str);
	nvram_pf_set(wl_prefix, "ssid", nvram_pf_safe_get(wlc_prefix, "ssid"));
	nvram_set("wl_ssid", nvram_pf_safe_get(wlc_prefix, "ssid"));
	if(wlc_prefix && !strncmp(wlc_prefix, "wlc", 3)){
		nvram_pf_set(wl_prefix, "auth_mode_x", nvram_pf_safe_get(wlc_prefix, "auth_mode"));
		auth_mode = "wep";
	}else{
		nvram_pf_set(wl_prefix, "auth_mode_x", nvram_pf_safe_get(wlc_prefix, "auth_mode_x"));
		auth_mode = "wep_x";
	}
	nvram_pf_set(wl_prefix, "wep_x", nvram_pf_safe_get(wlc_prefix, auth_mode));
	nvram_pf_set(wl_prefix, "key", nvram_pf_safe_get(wlc_prefix, "key"));
	for(i = 1; i < 5; i++) {
		char prekey[16];
		if(nvram_pf_get_int(wlc_prefix, "key") != i)
		{
			snprintf(prekey, sizeof(prekey), "key%d", i);
			nvram_pf_set(wl_prefix, prekey, nvram_pf_get(wlc_prefix, "wep_key"));
		}
	}
	nvram_pf_set(wl_prefix, "crypto", nvram_pf_safe_get(wlc_prefix, "crypto"));
	nvram_pf_set(wl_prefix, "wpa_psk", nvram_pf_safe_get(wlc_prefix, "wpa_psk"));
	str = nvram_pf_get(wlc_prefix, "nbw_cap");
	if(str && *str)
		nvram_pf_set(wl_prefix, "bw", str);
	nvram_pf_set(wl_prefix, "radius_ipaddr", nvram_pf_safe_get(wlc_prefix, "radius_ipaddr"));
	nvram_pf_set(wl_prefix, "radius_key", nvram_pf_safe_get(wlc_prefix, "radius_key"));
	nvram_pf_set(wl_prefix, "radius_port", nvram_pf_safe_get(wlc_prefix, "radius_port"));
	return 0;
}

void write_wpa_supplicant_network(FILE *fp, int band, const char *prefix, int disabled, char *path)
{
	int mfp, wpapsk = 0, i;
	int flag_wep = 0;
	size_t len;
	char crypto[32] = {0}, auth_mode[32] = {0}, tmp_prefix[16] = {0}, wpa_key[128] = {0};
	if(!fp || !prefix || !path)
		return;
	strlcpy(crypto, nvram_pf_safe_get(prefix, "crypto"), sizeof(crypto));
	strlcpy(auth_mode, nvram_pf_safe_get(prefix, "auth_mode"), sizeof(auth_mode));
	if(is_routing_enabled())
		snprintf(tmp_prefix, sizeof(tmp_prefix), "wl%d_", band);
#if defined(RTCONFIG_AMAS)
	else if(aimesh_re_node())
		snprintf(tmp_prefix, sizeof(tmp_prefix), "wl%d.1_", band);
#endif
	if(tmp_prefix[0])
		mfp = nvram_pf_get_int(tmp_prefix, "mfp");
	else
		mfp = 0;
	len = strlen(nvram_pf_safe_get(prefix, "wpa_psk"));
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(!strcmp(auth_mode, "sae") || !strcmp(auth_mode, "wpa3") || !strcmp(auth_mode, "owe")
		|| !strcmp(auth_mode, "suite-b"))
		mfp = 2;
	else
#endif
	if((!strcmp(auth_mode, "psk2") || !strcmp(auth_mode, "wpawpa2")
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		|| !strcmp(auth_mode, "psk2sae")|| !strcmp(auth_mode, "wpa2wpa3")
#endif
		) && mfp <= 0)
		mfp = 1;
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(!strcmp(auth_mode, "sae") || !strcmp(auth_mode, "psk2sae"))
		fprintf(fp,"sae_pwe=2\n");
#endif
	fprintf(fp,"network={\n\tssid=\"%s\"\n", nvram_pf_get(prefix, "ssid") ? : "8f3610e3c9feabed953a6");
	if(disabled)
		fprintf(fp,"\tdisabled=1\n");
	fprintf(fp,"\tscan_ssid=1\n");
	get_wpa_key_mgmt(band, auth_mode, mfp, wpa_key, sizeof(wpa_key));
	if(!auth_mode[0] || (!strcmp(auth_mode, "open") && nvram_pf_match(prefix, "wep", "0"))){
		fprintf(fp,"\tkey_mgmt=NONE\n");//open/none
	}else if (!strcmp(auth_mode, "open")){
		flag_wep = 1;
		fprintf(fp,"\tkey_mgmt=NONE\n");//open
		fprintf(fp,"\tauth_alg=OPEN\n");
	}else if (!strcmp(auth_mode, "shared")){
		flag_wep = 1;
		fprintf(fp,"\tkey_mgmt=NONE\n");//shared
		fprintf(fp,"\tauth_alg=SHARED\n");
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	}else if(!strcmp(auth_mode, "sae") || !strcmp(auth_mode, "psk2sae")){
		fprintf(fp,"\tkey_mgmt=%s\n", wpa_key);
		fprintf(fp,"\tproto=RSN\n");
		if(len == 64)
			fprintf(fp,"\tsae_password=%s\n", nvram_pf_safe_get(prefix, "wpa_psk"));
		else
			fprintf(fp,"\tsae_password=\"%s\"\n", nvram_pf_safe_get(prefix, "wpa_psk"));
		if(nvram_contains_word("rc_support", "wpa3"))
 			fprintf(fp,"\tieee80211w=%d\n", mfp);
#endif
	}else if(!strcmp(auth_mode, "psk") || !strcmp(auth_mode, "psk2") || !strcmp(auth_mode, "pskpsk2")
		|| !strcmp(auth_mode, "wpa") || !strcmp(auth_mode, "wpa2") || !strcmp(auth_mode, "wpawpa2")
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		|| !strcmp(auth_mode, "wpa3") || !strcmp(auth_mode, "wpa2wpa3")
#endif
		){
		if(!auth_mode[0] || !strcmp(auth_mode, "owe") || !strcmp(auth_mode, "suite-b")
			|| !strcmp(auth_mode, "psk2") || !strcmp(auth_mode, "wpa2")
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			|| !strcmp(auth_mode, "wpa3") || !strcmp(auth_mode, "wpa2wpa3")
#endif
			)
			wpapsk = 2;
		else if(!strcmp(auth_mode, "wpawpa2"))
			wpapsk = 3;
		else if(!strcmp(auth_mode, "wpa"))
			wpapsk = 1;
		else if(strcmp(auth_mode, "radius"))
			wpapsk = 2;
		fprintf(fp, "\tkey_mgmt=%s\n", wpa_key);
		if(wpapsk){
			if(wpapsk == 1)
				fprintf(fp, "\tproto=%s\n", "WPA");
			else if(wpapsk == 2)
				fprintf(fp, "\tproto=%s\n", "RSN");
			else if(wpapsk == 3)
				fprintf(fp, "\tproto=%s\n", "WPA RSN");
		}
		if(!strcmp(crypto, "tkip")){
			fprintf(fp, "\tpairwise=%s\n", "TKIP");
			fprintf(fp, "\tgroup=%s\n", "TKIP");
		}else if(!strcmp(crypto, "aes")){
			char *aes_type = "CCMP";
			fprintf(fp, "\tpairwise=%s\n", aes_type);
			fprintf(fp, "\tgroup=%s\n", aes_type);
		}else if(!strcmp(crypto, "aes+gcmp256")){
			fprintf(fp, "\tpairwise=%s\n", "CCMP GCMP-256");
			fprintf(fp, "\tgroup=%s\n", "CCMP GCMP-256");
		}else if(!strcmp(crypto, "suite-b")){
			fprintf(fp, "\tpairwise=%s\n", "GCMP-256");
			fprintf(fp, "\tgroup=%s\n", "GCMP-256");
			fprintf(fp, "\tgroup_mgmt=%s\n", "BIP-GMAC-256");
		}else{
			fprintf(fp, "\tpairwise=%s\n", "CCMP TKIP");
			fprintf(fp, "\tgroup=%s\n", "CCMP TKIP");
		}
		if(strstr(wpa_key, "WPA-PSK")){
			if(len == 64)
				fprintf(fp, "\tpsk=%s\n", nvram_pf_safe_get(prefix, "wpa_psk"));
			else
				fprintf(fp, "\tpsk=\"%s\"\n", nvram_pf_safe_get(prefix, "wpa_psk"));
		}else
			_dprintf("%s: WPA-EAP support is incompleted !!\n", __func__);
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		if(nvram_contains_word("rc_support", "wpa3"))
			fprintf(fp,"\tieee80211w=%d\n", mfp);
	}else if(!strcmp(auth_mode, "owe")){
		fprintf(fp, "\tkey_mgmt=%s\n", wpa_key);
		fprintf(fp, "\tproto=%s\n", "RSN");
		fprintf(fp, "\tpairwise=%s\n", "CCMP");
		fprintf(fp, "\tgroup=%s\n", "CCMP");
		fprintf(fp, "\tieee80211w=%d\n", mfp);
#endif
	}else{
		if(!strcmp(auth_mode, "aes+gcmp256")){
			fprintf(fp, "\tkey_mgmt=%s\n", wpa_key);
			fprintf(fp, "\tproto=%s\n", "RSN");
			fprintf(fp, "\tpairwise=%s\n", "CCMP GCMP-256");
			fprintf(fp, "\tgroup=%s\n", "CCMP GCMP-256");
			if(strstr(wpa_key, "WPA-PSK")){
				if(len == 64)
					fprintf(fp, "\tpsk=%s\n", nvram_pf_safe_get(prefix, "wpa_psk"));
				else
					fprintf(fp, "\tpsk=\"%s\"\n", nvram_pf_safe_get(prefix, "wpa_psk"));
			}else
				_dprintf("%s: WPA-EAP support is incompleted !!\n", __func__);
		}else
			fprintf(fp,"\tkey_mgmt=NONE\n");
	}
	if(flag_wep && nvram_pf_get_int(prefix, "wep_x")){
		char *str;
		for(i = 1 ; i <= 4; i++){
			if(nvram_pf_get_int(prefix, "key") != i)
				continue;
			str = nvram_pf_safe_get(prefix, "wep_key");
			len = strlen(str);
			if(len == 5 || len == 13){
				fprintf(fp, "\twep_tx_keyidx=%d\n", i - 1);
				fprintf(fp, "\twep_key%d=\"%s\"\n", i - 1, str);
			}else if(len == 10 || len == 26){
				fprintf(fp, "\twep_tx_keyidx=%d\n", i - 1);
				fprintf(fp, "\twep_key%d=%s\n", i - 1, str);
			}else
				fprintf(fp, "# %s: invalid %swep_key(%s)\n", __func__, prefix, str);
		}
	}
	fprintf(fp,"}\n");
}

/*
 * write_rpt_wpa_supplicant_conf()
 *
 * copy wifi nvram variable from prefix_wlc"xxx" to prefix_mssid"xxx"
 * and generate wpa_supplicant configure file
 */
void write_rpt_wpa_supplicant_conf(int band, const char *prefix_mssid, const char *prefix_wlc, const char *addition)
{
	FILE *fp_wpa;
	char tmp[128];
	int i;

	snprintf(tmp, sizeof(tmp), "/etc/Wireless/conf/wpa_supplicant-%s.conf", get_staifname(band));
	if (!(fp_wpa = fopen(tmp, "w+")))
		return;

	fprintf(fp_wpa, "ctrl_interface=/var/run/wpa_supplicant-%s\n%s\n", get_staifname(band),	(addition ? : ""));
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP) 
	fprintf(fp_wpa, "bss_expiration_age=%d\n", 20);
#endif
	i = addition && !strcmp(addition, "INIT_DISABLE");
	write_wpa_supplicant_network(fp_wpa, band, prefix_mssid, i, tmp);
	write_wpa_supplicant_network(fp_wpa, band, prefix_wlc, i, tmp);
	fclose(fp_wpa);
}
#endif	/* RTCONFIG_WIRELESSREPEATER */

int gen_lantiq_config(int band, int subnet)
{
#ifdef RTCONFIG_WIRELESSREPEATER
	int wlc_band = nvram_get_int("wlc_band");
#endif
	FILE *fp, *fp2, *fp3, *fp5;
	char *str = NULL;
	char *str2 = NULL;
	int i, j, bw = 0, puren = 0, only_20m = 0, bw160 = 0;
	int max_subnet, subunit;
	char list[2048];
	char wds_mac[4][30], buf[128];
	int wds_keyidx;
	char wds_key[50];
	int flag_8021x = 0;
	int warning = 0;
	char tmp[128], prefix[] = "wlXXXXXXX_", prefix2[] = "wlXXXXXXX_", main_prefix[] = "wlXXX_", athfix[]="athXXXX_",tmpfix[]="wlXXXXX_",rpfix[]="wlXXXXX_";
	char temp[128], prefix_mssid[] = "wlXXXXXXXXXX_mssid_";
	char tmpstr[128];
	char *nv, *nvp, *b, *p;
	int mcast_phy = 0, mcast_mcs = 0;
	int mac_filter[MAX_NO_MSSID];
	char t_mode[30],t_bw[10],t_ext[10],mode_cmd[100];
	int bg_prot, ban, wmm, legacy = 0;
	int sw_mode = sw_mode();
	int wpapsk;
	int val,rate;
	char wif[10], vphy[10];
	char path1[50],path2[50],path3[50];
	int rep_mode, nmode, shortgi, /*stbc, */ap_isolate;
	char ht_capab[128] = {0};
	long caps;

#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	int channel;
	int txbf, mumimo;
#endif
	char br_if[IFNAMSIZ], wpa_key_mgmt[128] = {0};
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	int acs_dfs = nvram_get_int("acs_dfs");
	struct bandx_defval_s *wldefval = wldefval_tbl[band];
	struct chattr_s *pchattr = NULL;
	int vhtsubfer = 0;
	int vhtmubfer = 0;
	int isax = 0;
	int max_band = 0, max_bw160 = 0, ofdma;
	int bfPeriod = 0;//defalut=0
#endif
	char iwphy[16];

	struct chattr_s chattr_2g[] = {
		{ 1, 1, "[HT40+]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 2, 1, "[HT40+]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 3, 1, "[HT40+]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 4, 1, "[HT40+]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 5, 1, "[HT40+]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 6, 1, "[HT40+]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 7, 1, "[HT40+]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 8, -1, "[HT40-]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 9, -1, "[HT40-]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 10, -1, "[HT40-]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 11, -1, "[HT40-]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 12, -1, "[HT40-]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 13, -1, "[HT40-]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ 14, -1, "[HT40-]", 0, 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0 },
		{ -1, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	struct chattr_s chattr_5g[] = {
		{ 36, 1, "[HT40+]", 38, 42, 50, 0, bw40cap[0], bw80cap[0], bw160cap[0], 0, 0, 0 },
		{ 40, -1, "[HT40-]", 38, 42, 50, 0, bw40cap[0], bw80cap[0], bw160cap[0], 0, 0, 0 },
		{ 44, 1, "[HT40+]", 38, 42, 50, 0, bw40cap[1], bw80cap[0], bw160cap[0], 0, 0, 0 },
		{ 48, -1, "[HT40-]", 46, 42, 50, 0, bw40cap[1], bw80cap[0], bw160cap[0], 0, 0, 0 },
		{ 52, 1, "[HT40+]", 54, 58, 50, 0, bw40cap[2], bw80cap[1], bw160cap[0], 0, 0, 0 },
		{ 56, -1, "[HT40-]", 54, 58, 50, 0, bw40cap[2], bw80cap[1], bw160cap[0], 0, 0, 0 },
		{ 60, 1, "[HT40+]", 62, 58, 50, 0, bw40cap[3], bw80cap[1], bw160cap[0], 0, 0, 0 },
		{ 64, -1, "[HT40-]", 62, 58, 50, 0, bw40cap[3], bw80cap[1], bw160cap[0], 0, 0, 0 },
		{ 100, 1, "[HT40+]", 102, 106, 114, 0, bw40cap[4], bw80cap[2], bw160cap[1], 0, 0, 0 },
		{ 104, -1, "[HT40-]", 102, 106, 114, 0, bw40cap[4], bw80cap[2], bw160cap[1], 0, 0, 0 },
		{ 108, 1, "[HT40+]", 110, 106, 114, 0, bw40cap[5], bw80cap[2], bw160cap[1], 0, 0, 0 },
		{ 112, -1, "[HT40-]", 110, 106, 114, 0, bw40cap[5], bw80cap[2], bw160cap[1], 0, 0, 0 },
		{ 116, 1, "[HT40+]", 118, 106, 114, 0, bw40cap[6], bw80cap[3], bw160cap[1], 0, 0, 0 },
		{ 120, -1, "[HT40-]", 118, 106, 114, 0, bw40cap[6], bw80cap[3], bw160cap[1], 0, 0, 0 },
		{ 124, 1, "[HT40+]", 126, 106, 114, 0, bw40cap[7], bw80cap[3], bw160cap[1], 0, 0, 0 },
		{ 128, -1, "[HT40-]", 126, 106, 114, 0, bw40cap[7], bw80cap[3], bw160cap[1], 0, 0, 0 },
		{ 132, 1, "[HT40+]", 134, 138, 0, 0, bw40cap[8], bw80cap[4], 0, 0, 0, 0 },
		{ 136, -1, "[HT40-]", 134, 138, 0, 0, bw40cap[8], bw80cap[4], 0, 0, 0, 0 },
		{ 140, 1, "[HT40+]", 142, 138, 0, 0, bw40cap[9], bw80cap[4], 0, 0, 0, 0 },
		{ 144, -1, "[HT40-]", 142, 138, 0, 0, bw40cap[9], bw80cap[4], 0, 0, 0, 0 },
		{ 149, 1, "[HT40+]", 151, 155, 163, 0, bw40cap[10], bw80cap[5], bw160cap[2], 0, 0, 0 },
		{ 153, -1, "[HT40-]", 151, 155, 163, 0, bw40cap[10], bw80cap[5], bw160cap[2], 0, 0, 0 },
		{ 157, 1, "[HT40+]", 159, 155, 163, 0, bw40cap[11], bw80cap[5], bw160cap[2], 0, 0, 0 },
		{ 161, -1, "[HT40-]", 159, 155, 163, 0, bw40cap[11], bw80cap[5], bw160cap[2], 0, 0, 0 },
		{ 165, 1, "[HT40+]", 167, 171, 163, 0, bw40cap[12], bw80cap[6], bw160cap[2], 0, 0, 0 },
		{ 169, -1, "[HT40-]", 167, 171, 163, 0, bw40cap[12], bw80cap[6], bw160cap[2], 0, 0, 0 },
		{ 173, 1, "[HT40+]", 175, 171, 163, 0, bw40cap[13], bw80cap[6], bw160cap[2], 0, 0, 0 },
		{ 177, -1, "[HT40-]", 175, 171, 163, 0, bw40cap[13], bw80cap[6], bw160cap[2], 0, 0, 0 },
		{ -1, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	struct chattr_s *chattr_tbl[] = { chattr_2g, chattr_5g};

	int ac_2cap[] = {CAP_HT20, CAP_HT2040 | CAP_HT20, CAP_HT2040};
	int ac_5cap[] = {CAP_11ACVHT20, CAP_11ACVHT20 | CAP_11ACVHT40 | CAP_11ACVHT80, CAP_11ACVHT40,
						 CAP_11ACVHT80, CAP_11ACVHT80_80 | CAP_11ACVHT160};
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	int ax_2cap[] = {CAP_11AXGHE20, CAP_11AXGHE20, CAP_11AXGHE40};
	int ax_5cap[] = {CAP_11AXAHE20, CAP_11AXAHE20 | CAP_11AXAHE40 | CAP_11AXAHE80, CAP_11AXAHE40,
						CAP_11AXAHE80, CAP_11AXAHE160};
#endif

	rep_mode=0;
	bg_prot=0;
	ban=0;
	memset(mode_cmd,0,sizeof(mode_cmd));
	memset(t_mode,0,sizeof(t_mode));
	memset(t_bw,0,sizeof(t_bw));
	memset(t_ext,0,sizeof(t_ext));
	max_subnet = num_of_mssid_support(band);

	if(max_subnet < subnet)
		return -1;
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(!wldefval)
	{
		dbg("%s: default value of band %d is not defined!\n", __func__, band);
		return -1;
	}
#endif
	get_wlxy_ifname(band, subnet, wif);
	strcpy(vphy, get_vphyifname(band));

	snprintf(path1, sizeof(path1), "/etc/Wireless/conf/hostapd_%s.conf", wif);
	snprintf(path2, sizeof(path2), "/etc/Wireless/sh/postwifi_%s.sh", wif);
	snprintf(path3, sizeof(path3), "/etc/Wireless/sh/prewifi_%s.sh", wif);
	system("mkdir -p /etc/Wireless/conf");
	system("mkdir -p /etc/Wireless/sh");

	if (nvram_match("skip_gen_lantiq_config", "1") && f_exists(path2) && f_exists(path3)) {
		_dprintf("%s: reuse %s and %s\n", __func__, path2, path3);
		return 0;
	}

	_dprintf("gen lantiq config\n");
	if (!(fp = fopen(path1, "w+")))
		return 0;
	if (!(fp2 = fopen(path2, "w+"))){
		fclose(fp);
		return 0;
	}
	if (!(fp3 = fopen(path3, "w+"))){
		fclose(fp);
		fclose(fp2);
		return 0;
	}

#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	fprintf(fp2, "#!/bin/sh -p\n");
	fprintf(fp3, "#!/bin/sh -p\n");
	if(mediabridge_mode()){
		fclose(fp2);
		if (!(fp2 = fopen("/dev/null", "w+")))
			return 0;
	}
#endif

#ifdef RTCONFIG_WIRELESSREPEATER
	if (sw_mode == SW_MODE_REPEATER && wlc_band == band && nvram_invmatch("wlc_ssid", "")&& subnet==0)
		rep_mode=1;
#endif
	get_iwphy_name(band, iwphy, sizeof(iwphy));
	fprintf(fp, "#The word of \"Default\" must not be removed\n");
	fprintf(fp, "#Default\n");

	snprintf(main_prefix, sizeof(main_prefix), "wl%d_", band);
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	channel = nvram_pf_get_int(main_prefix, "channel");
	if(channel == 0){
		if(get_channel_list_via_driver(band, list, sizeof(list)) > 0){
			p = strtok(list, ",");
			channel = safe_atoi(p);
		}
	}
	if(channel > 0){
		for(pchattr = chattr_tbl[band]; pchattr->channel > 0; pchattr++){
			if(pchattr && pchattr->channel == channel){
				break;
			}
		}
	}
#endif
	if(subnet == 0){
		subunit = 0;
		snprintf(prefix, sizeof(prefix), "wl%d_", band);
	}else{
		subunit = subnet;
		j = 0;
		for(i = 1;i <= max_subnet; i++){
#if defined(RTCONFIG_AMAS)
			if(sw_mode == SW_MODE_AP && nvram_match("re_mode", "1") && i == 1){
				i = 2;
			}
#endif
			snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d.%d_", band, i);
			if(nvram_pf_match(prefix_mssid, "bss_enabled", "1")){
				j++;
				if(j == subnet){
					snprintf(prefix, sizeof(prefix), "wl%d.%d_", band,i);
					subunit = i;
				}
			}
		}
	}
	strlcpy(prefix2, prefix, sizeof(prefix2));
	bw160 = nvram_pf_match(main_prefix, "bw_160", "1") != 0;
#ifdef RTCONFIG_WIRELESSREPEATER
	if(rep_mode)
		snprintf(prefix, sizeof(prefix), "wl%d.1_", band);
#ifdef RTCONFIG_AMAS
	if(nvram_match("x_Setting", "0")){
		if(!rep_mode)
			acs_dfs = 0;
	}else{
		if(!rep_mode && aimesh_re_node())
			acs_dfs = 0;
	}
	if(sw_mode == SW_MODE_AP && subnet == 0 && nvram_match("re_mode", "1"))
	{
		char prefix_wlc[32];
		snprintf(prefix_wlc, sizeof(prefix_wlc), "wl%d.1_", band);
		snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d_", band);
		nvram_pf_set(prefix_mssid, "closed", nvram_pf_safe_get(prefix_wlc, "closed"));
		nvram_pf_set(prefix_mssid, "ap_isolate", nvram_pf_safe_get(prefix_wlc, "ap_isolate"));
		nvram_set("ure_disable", "0");
		nvram_pf_set(prefix_wlc, "nbw_cap", nvram_pf_safe_get(prefix_mssid, "bw"));	//preset "nbw_cap" before restore
		copy_rpt_params_to_wl(prefix_mssid, prefix_wlc);
		if(strstr(nvram_safe_get("sta_ifnames"), get_staifname(band))){
			if(!strstr(nvram_safe_get("skip_ifnames"), get_staifname(band))){
				snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc%d_", band);
//				if(en_multi_profile() && band == 1){
//					snprintf(prefix_wlc, sizeof(prefix_wlc), "wsbh_");
//					snprintf(prefix_mssid, sizeof(prefix_mssid), "wsfh_");
//				}else{
//					snprintf(prefix_wlc, sizeof(prefix_wlc), "");
//					snprintf(prefix_mssid, sizeof(prefix_mssid), "");
//				}
				// convert wlc%d_xxx to wlX_ 
				write_rpt_wpa_supplicant_conf(band, prefix_mssid, prefix_wlc, "INIT_DISABLE");
				rep_mode=1;	/* set here to avoid changing prefix */
			}
		}
	}
	else
#endif	/* RTCONFIG_AMAS */
	if ((sw_mode == SW_MODE_REPEATER && wlc_band == band && nvram_invmatch("wlc_ssid", "") && subnet == 0)
		|| (wisp_mode() && wlc_band == band && subnet == 0))
	{
		char prefix_wlc[32];
		snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc_");
		snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d_", band);

		nvram_pf_set(prefix_wlc, "nbw_cap", nvram_pf_safe_get(prefix_mssid, "bw"));
#if defined(RTCONFIG_WISP)
		if(!wisp_mode()){
#endif
		copy_rpt_params_to_wl(prefix_mssid, prefix_wlc);
#if defined(RTCONFIG_WISP)
		}
#endif
		nvram_set("ure_disable", "0");
		// convert wlc_xxx to wlX_ according to wlc_band == band
		write_rpt_wpa_supplicant_conf(band, prefix_mssid, prefix_wlc, NULL);
	}
#endif	/* RTCONFIG_WIRELESSREPEATER */

#ifdef RTCONFIG_AMAS
	if (!aimesh_re_node() && !nvram_pf_get_int(main_prefix, "radio"))
		nvram_pf_set(main_prefix, "radio", "1");
#endif

	fprintf(fp, "interface=%s\n",wif);
	fprintf(fp, "ctrl_interface=/var/run/hostapd\n");
	fprintf(fp, "send_probe_response=0\n");
	fprintf(fp, "driver=nl80211\n");
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
#if defined(RTCONFIG_MULTILAN_CFG)
	if(subnet){
		if(get_apg_vid_by_ifname(wif) < 0)
			snprintf(list, sizeof(list), "br%d", 0);
		else
			snprintf(list, sizeof(list), "br%d", get_apg_vid_by_ifname(wif));
		fprintf(fp, "bridge=%s\n", get_apg_br_by_vid(get_apg_vid_by_ifname(wif)));
	}else
#elif defined(RTCONFIG_AMAS_WGN)
	if(subnet && subnet <= wgn_guest_ifcount(band) && guest_wlif(wif) != 0)
		fprintf(fp, "bridge=%s\n", find_brX(wif));
	else
#endif
		fprintf(fp, "bridge=%s\n", get_vap_bridge());
	strlcpy(br_if, get_vap_bridge(), sizeof(br_if));
#else
	strlcpy(br_if, nvram_get("lan_ifname")? : nvram_default_get("lan_ifname"), sizeof(br_if));
	if(*br_if == '\0')
		strcpy(br_if, "br0");
	fprintf(fp, "bridge=%s\n",br_if);
#endif

	fprintf(fp, "logger_syslog=-1\n");
	fprintf(fp, "logger_syslog_level=2\n");
	fprintf(fp, "logger_stdout=-1\n");
	fprintf(fp, "logger_stdout_level=2\n");
	fprintf(fp, "ctrl_interface_group=0\n");
	fprintf(fp, "max_num_sta=255\n");
	fprintf(fp, "eapol_key_index_workaround=0\n");
	fprintf(fp, "disassoc_low_ack=1\n");
	flag_8021x=0;

	str = nvram_pf_get(prefix, "auth_mode_x");
	if (str && (!strcmp(str, "radius") || !strcmp(str, "wpa") 
		|| !strcmp(str, "wpa2") || !strcmp(str, "wpawpa2")
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		|| !strcmp(str, "wpa3") || !strcmp(str, "wpa2wpa3") || !strcmp(str, "suite-b")
#endif
		))
	{
		flag_8021x=1;
		fprintf(fp, "ieee8021x=1\n");
		fprintf(fp, "eap_server=0\n");
	}
	else
	{
		fprintf(fp, "ieee8021x=0\n");
		fprintf(fp, "eap_server=1\n");
	}	

	//SSID Num. [MSSID Only]
	fprintf(fp, "ssid=%s\n", nvram_pf_get(prefix, "ssid"));

	snprintf(tmpfix, sizeof(tmpfix), "wl%d_", band);
	//Network Mode
	val = nvram_pf_get_int(tmpfix, "nmode_x");
	//5G
	if (band) {
		if (val == 8)
		{
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			strlcpy(t_mode, max_5g_ax_mode, sizeof(t_mode));
#else
			strlcpy(t_mode, max_5g_ac_mode, sizeof(t_mode));
#endif
			puren = 1;
		}
		else if (val == 1)
		{
			strlcpy(t_mode, "11NA", sizeof(t_mode));
			puren = 1;
		}
		else if (val == 2)
		{
			strlcpy(t_mode, "11A", sizeof(t_mode));
			legacy = 1;
		}
		else
		{
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			if(nvram_pf_get_int(main_prefix, "11ax") == 1)
				strlcpy(t_mode, max_5g_ax_mode, sizeof(t_mode));
			else
#endif
			strlcpy(t_mode, max_5g_ac_mode, sizeof(t_mode));

			puren = 1;
		}
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		isax = strstr(t_mode, "11AHE") != NULL;
#endif
	}
	else //2.4G
	{
		if (val == 8)
		{
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			strlcpy(t_mode, max_2g_ax_mode, sizeof(t_mode));
#else
			strlcpy(t_mode, max_2g_n_mode, sizeof(t_mode));
#endif
			bg_prot = 1;
		}
		else if (val == 2)
		{
			strlcpy(t_mode, "11G", sizeof(t_mode));
			bg_prot = 1;
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
#else
			ban = 1;
#endif
			legacy = 1;
		}
		else if (val == 1)
		{
			strlcpy(t_mode, max_2g_n_mode, sizeof(t_mode));
			puren = 1;
		}
		else
		{
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			if(nvram_pf_get_int(main_prefix, "11ax") == 1)
				strlcpy(t_mode, max_2g_ax_mode, sizeof(t_mode));
			else
#endif
			strlcpy(t_mode, max_2g_n_mode, sizeof(t_mode));
			bg_prot = 1;
		}
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		isax = strstr(t_mode, "11GHE") != NULL;
#endif
	}
	/* WME, hostapd part. */
	str = nvram_pf_get(main_prefix, "wme");
	if (!str || *str == '\0')
		str = "auto";
	/* WMM mustn't be disabled if legacy client is not allowed, e.g., N only or N/AC mixed mode.
	 * If it happened, client can't connect to AP.
	 * In auto mode, if WMM is disabled, all client becomes legacy client!
	 */
	if (!puren && !strcmp(str, "off"))
		wmm = 0;
	else
		wmm = 1;
	fprintf(fp, "wmm_enabled=%d\n", wmm);

	nmode = nvram_pf_get_int(tmpfix, "nmode_x");
	// Short-GI
	shortgi = (nmode != 2);
//	fprintf(fp2, "%s %s shortgi %d\n", IWPRIV, wif, shortgi);//2g:20/40 5g:20/40/80/160 Always ON

	// STBC
//	stbc = (nmode != 2) && nvram_pf_get_int(tmpfix, "HT_STBC") != 0;
//	fprintf(fp2, "%s %s tx_stbc %d\n", IWPRIV, wif, stbc);//Always ON
//	fprintf(fp2, "%s %s rx_stbc %d\n", IWPRIV, wif, stbc);//Always OFF
	//TWT
	fprintf(fp, "twt_responder_support=%d\n", isax ? nvram_pf_get_int(main_prefix, "twt") != 0 : 0);
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	//0 - Disable, 1 - Enable, 2 - Dynamic
	if(nmode == 2 || !nvram_pf_get_int(tmpfix, "txbf")){
		vhtsubfer = 0;
		vhtmubfer = 0;
		if(isax){
			ofdma = nvram_pf_get_int(main_prefix, "ofdma");
			if(wldefval->he_mubfer != 0)
				fprintf(fp, "he_mu_beamformer=%d\n", 1);
			if(wldefval->he_subfer != 0)
				fprintf(fp, "he_su_beamformer=%d\n", 1);
			fprintf(fp2, "iw dev %s iwlwav sMuOfdmaBf %d %d\n", wif, vhtmubfer, bfPeriod);
			fprintf(fp2, "iw dev %s iwlwav sDynamicMu %d 0 4 4 2\n", wif, ofdma ? 1 : 0);
		}else if(wldefval->vhtmubfer != vhtmubfer || wldefval->vhtsubfer != vhtsubfer)
			fprintf(fp2, "iw dev %s iwlwav sMuOfdmaBf %d %d\n", wif, vhtmubfer, bfPeriod);
	}else{
		if(band == 1){
			vhtsubfer = 1;
			vhtmubfer = nvram_pf_get_int(tmpfix, "mumimo") != 0;
		}else{
			vhtsubfer = 1;
			if(!nmode && isax && vhtsubfer){
				vhtmubfer = nvram_pf_get_int(tmpfix, "mumimo") != 0;
			}else
				vhtmubfer = 0;
		}
		if(isax){
			ofdma = nvram_pf_get_int(main_prefix, "ofdma");
			fprintf(fp2, "iw dev %s iwlwav sMuOfdmaBf %d %d\n", wif, vhtmubfer, bfPeriod);
			fprintf(fp, "he_mu_beamformer=%d\n", vhtmubfer);
			fprintf(fp, "he_su_beamformer=%d\n", vhtsubfer);
			fprintf(fp2, "iw dev %s iwlwav sDynamicMu %d 0 4 4 2\n", wif, ofdma ? 1 : 0);
		}
	}
#ifdef RTCONFIG_OPTIMIZE_XBOX
	// LDPC
//	if(nvram_pf_match(prefix, "optimizexbox", "1"))
//		ldpc = 0;
//	else
//		ldpc = 3;
//		fprintf(fp2, "%s %s ldpc %d\n", IWPRIV, wif, ldpc);//Always ON
#endif
#else
	// TX BeamForming, must be set before association with the station.
	fprintf(fp2, "iw dev %s iwlwav sBfMode %d\n", wif, nvram_pf_get_int(tmpfix, "itxbf"));

#ifdef RTCONFIG_OPTIMIZE_XBOX
	// LDPC
//	if (nvram_pf_match(prefix, "optimizexbox", "1"))
//		ldpc = 0;
//	fprintf(fp2, "%s %s ldpc %d\n", IWPRIV, wif, ldpc? 3 : 0);//Always ON
#endif
#endif

	//fprintf(fp2,"ifconfig %s up\n",wif);
	strlcpy(tmp, nvram_pf_safe_get(prefix, "ssid"), sizeof(tmp));
	//replace SSID each char to "\char"
//	conv_iwconfig_essid(tmp, buf);
//	fprintf(fp2, "iwconfig %s essid -- %s\n", wif, buf);
	
	if(!rep_mode
#if defined(RTCONFIG_WISP)
		&& !wisp_mode()
#endif
	){

		if (!subnet) {
			//TxPreamble, all VAPs on same band share same setting.
			str = nvram_pf_safe_get(prefix, "plcphdr");
			if (!str)
				str = "long";
			fprintf(fp, "preamble=%d\n", strcmp(str, "short") == 0);
		}

		//RTSThreshold  Default=2347, each VAPs have it's own setting.
		str = nvram_pf_safe_get(main_prefix, "rts");
		if (!str || *str == '\0') {
			warning = 14;
			str = "2347";
		}
		if(safe_atoi(str) != 2347 && subnet == 0)
			fprintf(fp2, "iw phy %s set rts %d\n", iwphy, safe_atoi(str));
		fprintf(fp, "rts_threshold= %d\n", safe_atoi(str));

		//Fragmentation Threshold  Default=2346, each VAPs have it's own setting.
		if (legacy) {
			str = nvram_pf_safe_get(main_prefix, "frag");
			if (!str || *str == '\0') {
				warning = 15;
				str = "2346";
			}
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			if(safe_atoi(str) != wldefval->frag && subnet == 0)
#endif
				fprintf(fp2, "iw phy %s set frag %d\n", iwphy, safe_atoi(str));
		}

		//DTIM Period, each VAPs have it's own setting.
		str = nvram_pf_safe_get(main_prefix, "dtim");
		if (!str || *str == '\0') {
			warning = 11;
			str = "1";
		}
		fprintf(fp, "dtim_period=%d\n", safe_atoi(str));

		/* WME No acknowledge, all VAPs on same band share same setting. */
		if (!subnet && wmm) {
			if(nvram_pf_match(main_prefix, "wme_no_ack", "on")){
				fprintf(fp, "wmm_ac_bk_cwmin=%d\n", 4);
				fprintf(fp, "wmm_ac_bk_cwmax=%d\n", 10);
				fprintf(fp, "wmm_ac_bk_aifs=%d\n", 7);
				fprintf(fp, "wmm_ac_bk_txop_limit=%d\n", 0);
				fprintf(fp, "wmm_ac_bk_acm=%d\n", 0);
				fprintf(fp, "wmm_ac_be_aifs=%d\n", 3);
				fprintf(fp, "wmm_ac_be_cwmin=%d\n", 4);
				fprintf(fp, "wmm_ac_be_cwmax=%d\n", 10);
				fprintf(fp, "wmm_ac_be_txop_limit=%d\n", 0);
				fprintf(fp, "wmm_ac_be_acm=%d\n", 0);
				fprintf(fp, "wmm_ac_vi_aifs=%d\n", 2);
				fprintf(fp, "wmm_ac_vi_cwmin=%d\n", 3);
				fprintf(fp, "wmm_ac_vi_cwmax=%d\n", 4);
				fprintf(fp, "wmm_ac_vi_txop_limit=%d\n", 94);
				fprintf(fp, "wmm_ac_vi_acm=%d\n", 0);
				fprintf(fp, "wmm_ac_vo_aifs=%d\n", 2);
				fprintf(fp, "wmm_ac_vo_cwmin=%d\n", 2);
				fprintf(fp, "wmm_ac_vo_cwmax=%d\n", 3);
				fprintf(fp, "wmm_ac_vo_txop_limit=%d\n", 47);
				fprintf(fp, "wmm_ac_vo_acm=%d\n", 0);
			}
		}

		//APSDCapable, each VAPs have it's own setting.
		if (wmm) {
			str = nvram_pf_safe_get(main_prefix, "wme_apsd");
			if (!str || *str == '\0') {
				warning = 18;
				str = "off";
			}
			fprintf(fp, "uapsd_advertisement_enabled=%d\n", strcmp(str, "off") ? 1 : 0);
		}

		//BeaconPeriod, all VAPs on same band share same setting.
		if (!subnet) {
			const int min_bintval = 40;
			str = nvram_pf_safe_get(prefix, "bcn");
			if (!str || *str == '\0') {
				warning = 10;
				str = "100";
			}
			if (atoi(str) > 1000 || atoi(str) < min_bintval) {
				nvram_pf_set(prefix, "bcn", "100");
				fprintf(fp, "beacon_int=%d\n", 100);
			}else
				fprintf(fp, "beacon_int=%d\n", safe_atoi(str));
			fprintf(fp, "bss_beacon_int=0\n");//# 0: use global beacon_int.# 10..655350: override global beacon_int for this bss.
		}else
			fprintf(fp, "bss_beacon_int=0\n");

#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		//TxBurst, all VAPs on same band share same setting.
		if (!subnet) {
			str = nvram_pf_safe_get(prefix, "frameburst");
			if (!str || *str == '\0') {
				warning = 16;
				str = "off";
			}
			fprintf(fp, "tx_queue_data0_burst=%d.0\n", strcmp(str, "off") ? 3 : 0);
			fprintf(fp, "tx_queue_data1_burst=%d.0\n", strcmp(str, "off") ? 3 : 0);
			fprintf(fp, "tx_queue_data2_burst=%d.0\n", strcmp(str, "off") ? 3 : 0);
			fprintf(fp, "tx_queue_data3_burst=0\n");
		}
#endif
	}

	if (subnet == 0) {
		int obss_rssi_th = nvram_pf_get_int(prefix, "obss_rssi_th");
		if (obss_rssi_th == 0)
			obss_rssi_th = 35;	//as default value
		fprintf(fp, "obss_beacon_rssi_threshold=-60\n");//check RSSI of each scan entry
	}

	ap_isolate = nvram_pf_get_int(main_prefix, "ap_isolate") == 0;
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(wldefval->ap_isolate != ap_isolate)
#endif
		fprintf(fp,"ap_isolate=%d\n", ap_isolate);

#ifdef AMAZON_WSS
	if (amazon_wss_ap_isolate_support(prefix))
	{
		/* ap isolate */
		fprintf(fp,"ap_isolate=%d\n", ap_isolate);

		/* only QCA need to enable arping isolate when amazon_wss is enabled*/
#if LINUX_KERNEL_VERSION > KERNEL_VERSION(5,4,0)
		fprintf(fp2, "echo %d > /sys/devices/virtual/net/%s/brport/%s\n", nvram_pf_get_int(prefix, "ap_isolate"), wif, "isolated");
#else
		fprintf(fp2, "echo %d > /sys/devices/virtual/net/%s/brport/%s\n", nvram_pf_get_int(prefix, "ap_isolate"), wif, "isolate_mode");
#endif
	}
#endif

	//AuthMode
	memset(tmpstr, 0x0, sizeof(tmpstr));
	str = nvram_pf_safe_get(prefix, "auth_mode_x");
	if (str && strlen(str)) {
		if (!strcmp(str, "open")) 	
			fprintf(fp, "auth_algs=1\n");
		else if (!strcmp(str, "shared")) 
			fprintf(fp, "auth_algs=2\n");
		else if (!strcmp(str, "radius")) {
			fprintf(fp, "auth_algs=1\n");
			fprintf(fp, "wep_key_len_broadcast=5\n");
			fprintf(fp, "wep_key_len_unicast=5\n");
			fprintf(fp, "wep_rekey_period=300\n");
		}
		else	//wpa/wpa2/wpa-auto-enterprise:wpa/wpa2/wpawpa2
			//wpa/wpa2/wpa-auto-personal:psk/psk2/pskpsk2
			fprintf(fp, "auth_algs=1\n");
	}
	else
	{
		warning = 24;
		fprintf(fp, "auth_algs=1\n");
	}

	//EncrypType
	memset(tmpstr, 0x0, sizeof(tmpstr));

	str = nvram_pf_safe_get(prefix, "wpa_gtk_rekey");
	if (str && strlen(str)) 
		fprintf(fp, "wpa_group_rekey=%d\n", safe_atoi(str));

	snprintf(prefix_mssid, sizeof(prefix_mssid), "%s", prefix);
	get_wpa_key_mgmt(band, nvram_pf_safe_get(prefix_mssid, "auth_mode_x"), nvram_pf_get_int(prefix, "mfp"), wpa_key_mgmt, sizeof(wpa_key_mgmt));
	if(nvram_pf_match(prefix_mssid, "auth_mode_x", "open") && nvram_pf_match(prefix_mssid, "wep_x", "0"))
		fprintf(fp, "#wpa_pairwise=\n");
	else if((nvram_pf_match(prefix_mssid, "auth_mode_x", "open") && nvram_pf_invmatch(prefix_mssid, "wep_x", "0"))
		|| nvram_pf_match(prefix_mssid, "auth_mode_x", "shared") 
		|| nvram_pf_match(prefix_mssid, "auth_mode_x", "radius")){
		//wep
		int kn = 1;

#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		kn = nvram_pf_get_int(prefix_mssid, "key");
		snprintf(tmpstr, sizeof(tmpstr), "%skey%d", prefix_mssid, kn);
		if(strlen(nvram_safe_get(tmpstr))==10 || strlen(nvram_safe_get(tmpstr))==26)
			fprintf(fp, "wep_key%d=%s\n", kn - 1, nvram_safe_get(tmpstr));
		else if(strlen(nvram_safe_get(tmpstr))==5 || strlen(nvram_safe_get(tmpstr))==13)
			fprintf(fp, "wep_key%d=\"%s\"\n", kn - 1, nvram_safe_get(tmpstr));
		fprintf(fp, "wep_default_key=%d\n", kn - 1);
#else
		for (kn = 1; kn <= 4; kn++) {
			snprintf(tmpstr, sizeof(tmpstr), "%skey%d", prefix_mssid, kn);
			if (strlen(nvram_safe_get(tmpstr))==10 || strlen(nvram_safe_get(tmpstr))==26)
				fprintf(fp2,"iwconfig %s key [%d] %s\n", wif, kn, nvram_safe_get(tmpstr));
			else if (strlen(nvram_safe_get(tmpstr))==5 || strlen(nvram_safe_get(tmpstr))==13)
				fprintf(fp2,"iwconfig %s key [%d] \"s:%s\"\n", wif, kn, nvram_safe_get(tmpstr));
			else
				fprintf(fp2,"iwconfig %s key [%d] off\n", wif, kn);
		}
		str = nvram_pf_safe_get(prefix_mssid, "key");
		snprintf(tmpstr, sizeof(tmpstr), "%skey%s", prefix_mssid, str);
		if (strlen(nvram_safe_get(tmpstr)) > 0)
			fprintf(fp2,"iwconfig %s key [%s]\n",wif,str); //key index
		if (strlen(nvram_safe_get(tmpstr))==10 || strlen(nvram_safe_get(tmpstr))==26)
			fprintf(fp2,"iwconfig %s key %s\n",wif,nvram_safe_get(tmpstr));
		else if (strlen(nvram_safe_get(tmpstr))==5 || strlen(nvram_safe_get(tmpstr))==13)
			fprintf(fp2,"iwconfig %s key \"s:%s\"\n",wif,nvram_safe_get(tmpstr));
		else
			fprintf(fp, "#wpa_pairwise=\n");
#endif
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	}else if (nvram_pf_match(prefix, "auth_mode_x", "owe")){
		fprintf(fp, "wpa_key_mgmt=%s\n", wpa_key_mgmt);
		fprintf(fp, "eapol_version=2\n");
		fprintf(fp, "wpa_pairwise=CCMP\n");
#endif
	}else if (nvram_pf_match(prefix_mssid, "crypto", "tkip")){
		fprintf(fp, "wpa_key_mgmt=%s\n", wpa_key_mgmt);
		fprintf(fp, "wpa_strict_rekey=1\n");
		fprintf(fp, "eapol_version=2\n");
		fprintf(fp, "wpa_pairwise=TKIP\n");
	}else if (nvram_pf_match(prefix_mssid, "crypto", "aes")){
		fprintf(fp, "wpa_key_mgmt=%s\n", wpa_key_mgmt);
		fprintf(fp, "wpa_strict_rekey=1\n");
		fprintf(fp, "eapol_version=2\n");
		fprintf(fp, "wpa_pairwise=CCMP\n");
		if(nvram_pf_match(prefix, "auth_mode_x", "sae") || nvram_pf_match(prefix, "auth_mode_x", "psk2sae"))
			fprintf(fp, "sae_pwe=2\n");
	}else if (nvram_pf_match(prefix_mssid, "crypto", "tkip+aes")){
		fprintf(fp, "wpa_key_mgmt=%s\n", wpa_key_mgmt);
		fprintf(fp, "wpa_strict_rekey=1\n");
		fprintf(fp, "eapol_version=2\n");
		fprintf(fp, "wpa_pairwise=TKIP CCMP\n");
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	}else if (nvram_pf_match(prefix_mssid, "crypto", "suite-b")){
		fprintf(fp, "wpa_key_mgmt=%s\n", wpa_key_mgmt);
		fprintf(fp, "wpa_strict_rekey=1\n");
		fprintf(fp, "eapol_version=2\n");
		fprintf(fp, "wpa_pairwise=GCMP-256\n");
		fprintf(fp, "group_mgmt_cipher=BIP-GMAC-256\n");
	}else if (nvram_pf_match(prefix_mssid, "crypto", "aes+gcmp256")){
		fprintf(fp, "wpa_key_mgmt=%s\n", wpa_key_mgmt);
		fprintf(fp, "wpa_strict_rekey=1\n");
		fprintf(fp, "eapol_version=2\n");
			fprintf(fp, "wpa_pairwise=CCMP %s\n", "");
		if(nvram_pf_match(prefix, "auth_mode_x", "sae") || nvram_pf_match(prefix, "auth_mode_x", "psk2sae"))
			fprintf(fp, "sae_pwe=2\n");
#endif
	}else{
		warning = 25;
		fprintf(fp, "#wpa_pairwise=\n");
	}
	
	wpapsk=0;
	if(nvram_pf_match(prefix_mssid, "auth_mode_x", "open") && nvram_pf_match(prefix_mssid, "wep_x", "0"))
		fprintf(fp, "wpa=0\n");
	else if((nvram_pf_match(prefix_mssid, "auth_mode_x", "open") && nvram_pf_invmatch(prefix_mssid, "wep_x", "0"))
		|| nvram_pf_match(prefix_mssid, "auth_mode_x", "shared")
		|| nvram_pf_match(prefix_mssid, "auth_mode_x", "radius"))
		fprintf(fp, "wpa=0\n");
	else if(nvram_pf_match(prefix_mssid, "auth_mode_x", "psk") 
		|| nvram_pf_match(prefix_mssid, "auth_mode_x", "wpa")){
		wpapsk=1;
		fprintf(fp, "wpa=1\n");
	}else if(nvram_pf_match(prefix_mssid, "auth_mode_x", "psk2") 
		|| nvram_pf_match(prefix_mssid, "auth_mode_x", "wpa2")){		
		wpapsk=2;
		fprintf(fp, "wpa=2\n");
	}else if(nvram_pf_match(prefix_mssid, "auth_mode_x", "pskpsk2") 
		|| nvram_pf_match(prefix_mssid, "auth_mode_x", "wpawpa2")){	
		wpapsk=3;
		fprintf(fp, "wpa=3\n");
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	}else if(nvram_pf_match(prefix_mssid, "auth_mode_x", "sae")
		|| nvram_pf_match(prefix_mssid, "auth_mode_x", "wpa3")
		|| nvram_pf_match(prefix_mssid, "auth_mode_x", "psk2sae")
		|| nvram_pf_match(prefix_mssid, "auth_mode_x", "wpa2wpa3")){
		wpapsk = 4;
		fprintf(fp, "wpa=2\n");
		if(nvram_pf_match(prefix_mssid, "auth_mode_x", "sae") 
			|| nvram_pf_match(prefix_mssid, "auth_mode_x", "psk2sae"))
			fprintf(fp, "sae_password=%s\n", nvram_pf_safe_get(prefix_mssid, "wpa_psk"));
#endif
	}else{
		warning = 25;
		fprintf(fp, "wpa=0\n");
	}
	
	if(wpapsk != 0){
		if(!flag_8021x){
			char nv[65];

			snprintf(nv, sizeof(nv), "%s", nvram_pf_safe_get(prefix_mssid, "wpa_psk"));
			if (strlen(nv) == 64)
				snprintf(tmpstr, sizeof(tmpstr), "wpa_psk=%s\n", nv);
			else
				snprintf(tmpstr, sizeof(tmpstr), "wpa_passphrase=%s\n", nv);
			fprintf(fp, "%s", tmpstr);
		}
	}

	if(!ban)
	{
		//HT_BW
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		if(band){
			for(i = 0; i < 7; i++){
				if(bw80cap[i] == 1)
					max_band++;
			}
			if(find_word(nvram_safe_get("rc_support"), "vht160")){
				max_bw160 = bw160cap[0] == 1;
				if(bw160cap[1] == 1)
					max_bw160++;
				if(bw160cap[2] == 1)
					max_bw160++;
			}
		}
		bw = get_bw_via_channel(band, channel, pchattr, max_band, max_bw160);
#else
		bw = get_bw_via_channel(band, nvram_pf_get_int(tmpfix, "channel"));
#endif
		if (bw > 0)
		{
			*t_bw = '\0';
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			if(strstr(t_mode, "11ACV") || strstr(t_mode, "11AHE")){
				switch(bw){
					case 1:
						if(max_bw160 && channel | acs_dfs){
							if(bw160)
								strlcpy(t_bw, bw160_tbl[isax], sizeof(t_bw));
							else if(max_band)
								strlcpy(t_bw, bw80[isax], sizeof(t_bw));
							else
								strlcpy(t_bw, bw40[isax], sizeof(t_bw));
						}else{
							if(max_band)
								strlcpy(t_bw, bw80[isax], sizeof(t_bw));
							else
								strlcpy(t_bw, bw40[isax], sizeof(t_bw));
						}
						break;
					case 3:
					case 4:
						strlcpy(t_bw, bw80[isax], sizeof(t_bw));
						break;
					case 5:
						if(bw160)
							strlcpy(t_bw, bw160_tbl[isax], sizeof(t_bw));
						else if(max_band)
							strlcpy(t_bw, bw80[isax], sizeof(t_bw));
						else
							strlcpy(t_bw, bw40[isax], sizeof(t_bw));
						break;
					default:
						strlcpy(t_bw, bw40[isax], sizeof(t_bw));
						break;
				}
			}else if(strstr(t_mode, "11NA") || strstr(t_mode, "11NG") || strstr(t_mode, "11GHE")
				)
				strlcpy(t_bw, bw40[isax], sizeof(t_bw));
			else if(!strcmp(t_mode, "11A") || !strcmp(t_mode, "11G"))
				;
			else
				dbg("%s: Unknown t_mode [%s] bw %d\n", __func__, t_mode, bw);
			if(!t_bw[0])
				strlcpy(t_bw, bw40[isax], sizeof(t_bw));
#else
			if (strstr(t_mode,"11ACV") && (bw == 3 || bw == 1)) //80 BW or auto BW
				strlcpy(t_bw, "HT80", sizeof(t_bw));
			else
			{
				/* 0: 20Mhz
				 * 1: 20/40/80MHz (5G) or 20/40MHz (2G)
				 * 2: 40MHz
				 * 3: 80MHz
				 * 4: 80+80MHz
				 * 5: 160MHz
				 */
				if (strstr(t_mode, "11ACV")) {
					switch (bw) {
#if defined(RTCONFIG_VHT160)
					case 5:	/* 160Mhz */
						if (validate_bw_160_support() && nvram_get_int(strcat_r(tmpfix, "bw", tmp)) == 5)
							strlcpy(t_bw, "HT160", sizeof(t_bw));
						else {
							dbg("%s: Can't enable 160MHz support!\n", __func__);
							strlcpy(t_bw, "HT80", sizeof(t_bw));
						}
						break;
#endif
#if defined(RTCONFIG_VHT80_80)
					case 4:	/* 80+80MHz */
						if (validate_bw_80_80_support(band) && nvram_get_int(strcat_r(tmpfix, "bw", tmp)) == 4)
							strlcpy(t_bw, "HT80_80", sizeof(t_bw));
						else {
							dbg("%s: Can't enable 80+80MHz support!\n", __func__);
							strlcpy(t_bw, "HT80", sizeof(t_bw));
						}
						break;
#endif
					case 3:	/* 80MHz */
					case 1:	/* 20/40/80MHz */
						strlcpy(t_bw, "HT80", sizeof(t_bw));
						break;
					}
				} else if (strstr(t_mode, "11NA") || strstr(t_mode, "11NG")) {
					if (bw == 0)
						strlcpy(t_bw, "HT20", sizeof(t_bw));
					/* If bw == 1 (20/40MHz) or bw == 2 (40MHz),
					 * fallthrough to below code to decide plus/minus.
					 */
				} else if ((p = strstr(t_mode, "11A")) != NULL && *(p + 3) == '\0') {
					only_20m = 1;
				} else {
					dbg("%s: Unknown t_mode [%s] bw %d\n", __func__, t_mode, bw);
				}

				if (!only_20m && *t_bw == '\0') {
					sprintf(t_bw,"HT40");	

					//ext ch
					str = nvram_safe_get(strcat_r(tmpfix, "nctrlsb", tmp));
					if(!strcmp(str,"lower"))
						strlcpy(t_ext, "PLUS", sizeof(t_ext));
					else
						strlcpy(t_ext, "MINUS", sizeof(t_ext));

					bw40_channel_check(band,t_ext);
				}
			}
#endif
		}
		else {
			//warning = 34;
			if (!legacy)
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
				strlcpy(t_bw, /*"HT20"*/bw20[isax], sizeof(t_bw));
#else
				strlcpy(t_bw, "HT20", sizeof(t_bw));
#endif
		}
	}

	snprintf(mode_cmd, sizeof(mode_cmd), "%s%s%s", t_mode, t_bw, t_ext);
	if(sw_mode() == SW_MODE_REPEATER){
		snprintf(rpfix, sizeof(rpfix), "wl%d.1_", band);
		val = nvram_pf_get_int(rpfix, "channel");
	}
	else
	{
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		val = channel;
#else
		val = nvram_pf_get_int(main_prefix, "channel");
#endif
	}

	if(val){
		fprintf(fp, "channel=%d\n", val);
	}else{
		fprintf(fp, "channel=%s\n", "acs_smart");//0==auto_channel
	}
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
//	if(strstr(t_mode, "11NG") || strstr(t_mode, "11GHE") || strstr(t_mode, "11GEHT"))
#else
//	if(!band && strstr(t_mode, "11N") != NULL) //only 2.4G && N mode is used
#endif
//		fprintf(fp3,"%s %s disablecoext %d\n", IWPRIV,wif,(bw==2)?1:0);	// when N mode is used

#if defined(RTCONFIG_WISP)
//	if (wisp_mode())
//		fprintf(fp3, "%s %s dbdc_enable 0\n", IWPRIV, get_vphyifname(band));//dbdc?
#endif

	//AccessPolicy0
	{
#ifdef RTCONFIG_AMAS	/* reset prefix locally */
		char prefix[16];
		if (subnet <= 0 && nvram_get_int("re_mode") == 1)
			snprintf(prefix, sizeof(prefix), "wl%d.1_", band);
		else if(subnet > 0)
			snprintf(prefix, sizeof(prefix), "wl%d.%d", band, subnet);
		else
			snprintf(prefix, sizeof(prefix), "wl%d_", band);
#endif
		gen_macmode(mac_filter, band, prefix);	//Ren
		get_wlxy_ifname(band, subnet, athfix);
		str = nvram_pf_safe_get(prefix, "macmode");
		if (nvram_pf_invmatch(prefix, "macmode", "disabled")) {
			if(nvram_pf_match(prefix, "macmode", "allow"))
				fprintf(fp, "macaddr_acl=1\n");
			else
				fprintf(fp, "macaddr_acl=0\n");
			snprintf(tmp, sizeof(tmp), "/var/run/hostapd-%s.maclist", athfix);
			fprintf(fp, "accept_mac_file=%s\n", tmp);
		}

		list[0] = 0;
		list[1] = 0;
		if (nvram_pf_invmatch(prefix, "macmode", "disabled")) {
			nv = nvp = strdup(nvram_pf_safe_get(prefix, "maclist_x"));
			FILE *mac_fp;
			mac_fp = fopen(tmp, "w+");
			if (nv) {
				while ((b = strsep(&nvp, "<")) != NULL) {
					if (strlen(b) == 0)
						continue;
					if(mac_fp)
						fprintf(mac_fp,"%s\n", b);
				}
				free(nv);
			}
			if(mac_fp)
				fclose(mac_fp);
		}
	}

	if(rep_mode){
#if defined(RTCONFIG_AMAS)
		if(nvram_match("re_mode", "1"))
			gen_hostapd_wps_config(fp, band, subnet, br_if);
#endif
		if((nvram_pf_match(prefix_mssid, "auth_mode_x", "shared") 
			|| nvram_pf_match(prefix_mssid, "auth_mode_x", "open")) && !nvram_get_int("wps_enable")){
			fprintf(fp2, "ifconfig %s up\n", wif);
		}
		goto next;
	}	
		
	int WdsEnable=0;
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
#else
	int WdsEncrypType=0;
#endif
	if(subnet==0)
	{
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		int wds_ac, wds_ax, wds_nmode, first = 1;
#endif
		//WDS Enable
		if (sw_mode != SW_MODE_REPEATER && !nvram_pf_match(prefix, "mode_x", "0")) {
			//WdsEnable
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			str = nvram_pf_safe_get(main_prefix, "mode_x");
			if (str && strlen(str)) {
				if (nvram_pf_match(main_prefix, "auth_mode_x", "open")
					|| (nvram_pf_match(main_prefix, "auth_mode_x", "psk2")
					&& nvram_pf_match(main_prefix, "crypto", "aes"))) {
					if (safe_atoi(str) == 0)
						WdsEnable=0;
					else if (safe_atoi(str) == 1)
						WdsEnable=2;
					else if (safe_atoi(str) == 2) {
						if (nvram_pf_match(main_prefix, "wdsapply_x", "0"))
							WdsEnable=4;
						else
							WdsEnable=3;
					}
				 } else
					WdsEnable=0;
			} else {
				warning = 49;
				WdsEnable=0;
			}
			i=0;
			wds_keyidx=0;
			memset(wds_key,0,sizeof(wds_key));
			memset(wds_mac,0,sizeof(wds_mac));
			if ((nvram_pf_match(main_prefix, "mode_x", "1") || (nvram_pf_match(main_prefix, "mode_x", "2")
				&& nvram_pf_match(main_prefix, "wdsapply_x", "1")))
				&& (nvram_pf_match(main_prefix, "auth_mode_x", "open")
				|| (nvram_pf_match(main_prefix, "auth_mode_x", "psk2") 
				&& nvram_pf_match(main_prefix, "crypto", "aes")))) {
				nv = nvp = strdup(nvram_pf_safe_get(main_prefix, "wdslist"));
				if (nv) {
					while ((b = strsep(&nvp, "<")) != NULL) {
						if (strlen(b) == 0)
							continue;
						snprintf(wds_mac[i], 30, "%s", b);
						i++;
					}
					free(nv);
				}
			}
			wds_ac = (band == WL_5G_BAND && strstr(t_mode, "11ACVHT"));
			wds_ax = (strstr(t_mode, "11AHE") || strstr(t_mode, "11GHE"));
			wds_nmode = nvram_pf_get_int(main_prefix, "nmode_x");
			if(!(wds_nmode <= 1 || wds_nmode == 8)){
				max_bw160 = 0;
				caps = 0;
			}else{
				if(nvram_pf_match(prefix, "txbf", "1"))
					caps |= CAP_TXBF;
				if(band){
					caps |= ac_2cap[bw];
					if(wds_ac)
						caps |= ac_5cap[bw];
					if(wds_ax)
						caps |= ax_5cap[bw];
					if(bw != 1){
						max_bw160 = 0;
						if((caps & (CAP_11ACVHT80_80 | CAP_11ACVHT160 | CAP_11AXAHE80_80 | CAP_11AXAHE160)) != 0)
							max_bw160++;
						if(!max_bw160 && bw != 1){
							if(bw == 2)
								max_bw160 = ((channel >= 52 && channel <= 144) 
									|| (channel >= 48 && channel <= 144));
							else if(bw == 3)
								max_bw160 = ((channel >= 52 && channel <= 144) 
									|| (channel >= 40 && channel <= 144));
							else
								max_bw160 = (channel >= 52 && channel <= 144);
						}
					}else{
						if(find_word(nvram_safe_get("rc_support"), "vht80_80")){
							if(max_bw160){
								if(!channel || (pchattr && pchattr->bw160cap == 1)){
									max_bw160 = 1;
									caps |= (wds_ax << 30) | (wds_ac << 13);
									if(find_word(nvram_safe_get("rc_support"), "vht160")){
										if(max_band > 1 && (!channel || (pchattr && pchattr->bw80cap == 1))){
											max_bw160++;
											caps |= (wds_ax << 29) | (wds_ac << 14);
										}
									}
									if((caps & (CAP_11ACVHT80_80 | CAP_11ACVHT160 | CAP_11AXAHE80_80 | CAP_11AXAHE160)) != 0)
										max_bw160++;
									if(!max_bw160 && bw != 1){
										if(bw == 2)
											max_bw160 = ((channel >= 52 && channel <= 144) 
												|| (channel >= 48 && channel <= 144));
										else if(bw == 3)
											max_bw160 = ((channel >= 52 && channel <= 144) 
												|| (channel >= 40 && channel <= 144));
										else
											max_bw160 = (channel >= 52 && channel <= 144);
									}
								}
							}else{
								if(find_word(nvram_safe_get("rc_support"), "vht160")){
									if(max_band > 1 && (!channel || (pchattr && pchattr->bw80cap == 1))){
										max_bw160++;
										caps |= (wds_ax << 29) | (wds_ac << 14);
									}
								}
								if((caps & (CAP_11ACVHT80_80 | CAP_11ACVHT160 | CAP_11AXAHE80_80 | CAP_11AXAHE160)) != 0)
									max_bw160++;
								if(!max_bw160 && bw != 1){
									if(bw == 2)
										max_bw160 = ((channel >= 52 && channel <= 144) 
											|| (channel >= 48 && channel <= 144));
									else if(bw == 3)
										max_bw160 = ((channel >= 52 && channel <= 144) 
											|| (channel >= 40 && channel <= 144));
									else
										max_bw160 = (channel >= 52 && channel <= 144);
								}
							}
						}else{
							max_bw160 = 0;
							if(find_word(nvram_safe_get("rc_support"), "vht160")){
								if(max_band > 1 && (!channel || (pchattr && pchattr->bw80cap == 1))){
									max_bw160++;
									caps |= (wds_ax << 29) | (wds_ac << 14);
								}
							}
							if((caps & (CAP_11ACVHT80_80 | CAP_11ACVHT160 | CAP_11AXAHE80_80 | CAP_11AXAHE160)) != 0)
								max_bw160++;
							if(!max_bw160 && bw != 1){
								if(bw == 2)
									max_bw160 = ((channel >= 52 && channel <= 144) 
										|| (channel >= 48 && channel <= 144));
								else if(bw == 3)
									max_bw160 = ((channel >= 52 && channel <= 144) 
										|| (channel >= 40 && channel <= 144));
								else
									max_bw160 = (channel >= 52 && channel <= 144);
							}
						}
					}
				}else{
					caps |= ac_2cap[bw];
					if(wds_ax)
						caps |= ax_2cap[bw];
					max_bw160 = 0;
				}
			}
			dbg("WDS:defcaps 0x%x\n", caps);
			dbg("band %d ac %d ax %d nmode %d bw %d 80+80 %d 160 %d\n", band, wds_ac,
				wds_ax, wds_nmode, bw, (caps & (CAP_11ACVHT80_80 | CAP_11AXAHE80_80)) != 0, 
				(caps & (CAP_11ACVHT160 | CAP_11AXAHE160)) != 0);
			fprintf(fp, "wds_sta=1\n");
			//fprintf(fp2,"iw phy %s interface add %s type managed 4addr on\n", iwphy, wif);
			for(i = 0; i < 4; i++){
				if(strlen(wds_mac[i]) && nvram_pf_match(prefix, "wdsapply_x", "1")){
					if(first){
						if(max_bw160)
							fprintf(fp5, "sleep %d\n", 65);
						else
							fprintf(fp5, "sleep %d\n", 5);
						first = 0;
					}
				}
			}
#else
			str = nvram_pf_safe_get(prefix, "mode_x");
			if (str && strlen(str)) {
				if (nvram_pf_match(prefix, "auth_mode_x", "open")
				     || (nvram_pf_match(prefix, "auth_mode_x", "psk2")
				      && nvram_pf_match(prefix, "crypto", "aes"))) {
					if (safe_atoi(str) == 0)
						WdsEnable=0;
					else if (safe_atoi(str) == 1)
						WdsEnable=2;
					else if (safe_atoi(str) == 2) {
						if (nvram_pf_match(prefix, "wdsapply_x", "0"))
							WdsEnable=4;
						else
							WdsEnable=3;
					}
				 } else
					WdsEnable=0;
			} else {
				warning = 49;
				WdsEnable=0;
			}
			//WdsEncrypType
			if (nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_match(prefix, "wep_x", "0"))
				WdsEncrypType=0; //none
			else if (nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_invmatch(prefix, "wep_x", "0"))
				WdsEncrypType=1; //wep
			else if (nvram_pf_match(prefix, "auth_mode_x", "psk2") && nvram_pf_match(prefix, "crypto", "aes"))
				WdsEncrypType=2; //aes
			else
				WdsEncrypType=0; //none
			i=0;
			wds_keyidx=0;
			memset(wds_key,0,sizeof(wds_key));
			memset(wds_mac,0,sizeof(wds_mac));
			if ((nvram_pf_match(prefix, "mode_x", "1") || (nvram_pf_match(prefix, "mode_x", "2")
				&& nvram_pf_match(prefix, "wdsapply_x", "1")))
				&& (nvram_pf_match(prefix, "auth_mode_x", "open")
				|| (nvram_pf_match(prefix, "auth_mode_x", "psk2") 
				&& nvram_pf_match(prefix, "crypto", "aes")))) {
				nv = nvp = strdup(nvram_pf_safe_get(prefix, "wdslist"));
				if (nv) {
					while ((b = strsep(&nvp, "<")) != NULL) {
						if (strlen(b) == 0)
							continue;
						snprintf(wds_mac[i], 30, "%s", b);
						i++;
					}
					free(nv);
				}
			}

			if (nvram_pf_match(prefix, "auth_mode_x", "open")
				&& nvram_pf_invmatch(prefix, "wep_x", "0")) {
				wds_keyidx=nvram_pf_get_int(prefix, "key");
				//snprintf(list, sizeof(list), "wl%d_key%s", band, nvram_safe_get(strcat_r(prefix, "key", tmp)));
				str = strcat_r(prefix, "key", tmp);
				str2 = nvram_safe_get(str);
				snprintf(list, sizeof(list), "%s%s", str, str2);
				snprintf(wds_key, sizeof(wds_key), "%s", nvram_safe_get(list));
			} else if (nvram_pf_match(prefix, "auth_mode_x", "psk2") 
				&& nvram_pf_match(prefix, "crypto", "aes")) {
				wds_keyidx=nvram_pf_get_int(prefix, "wpa_psk");
			}

#if 0
			//debug
			dbg("wds_keyidx: %d\n",wds_keyidx);
			dbg("wds_key: %s\n",wds_key);
			dbg("wds_mac: %s | %s | %s |%s \n",wds_mac[0],wds_mac[1],wds_mac[2],wds_mac[3]);
#endif

			if (!band) {
				switch (nvram_pf_get_int(prefix, "nmode_x")) {
					case 1:	/* N ; fall through */
					case 0:	/* B, G, N */
						caps = CAP_HT2040 | CAP_HT20 | CAP_DS;
						caps |= CAP_TS | CAP_4S;
						if (nvram_pf_match(prefix, "txbf", "1"))
							caps |= CAP_TXBF;
						break;
					case 2:	/* B, G ; fall through */
					default:
						caps = 0;
						break;
				}
			} else {
				caps = CAP_HT2040 | CAP_HT20;
				switch (nvram_pf_get_int(prefix, "nmode_x")) {
					case 1:	/* N + AC ; it is not supported on QCA platform ; fall through */
					case 0:	/* Auto */
						if (strstr(mode_cmd, "VHT160"))
							caps |= CAP_11ACVHT160 | CAP_DS;
						else if (strstr(mode_cmd, "VHT80_80"))
							caps |= CAP_11ACVHT80_80 | CAP_DS;
						else if (strstr(mode_cmd, "VHT80"))
							caps |= CAP_11ACVHT80 | CAP_DS | CAP_TS | CAP_4S;
						else if (strstr(mode_cmd, "VHT40"))
							caps |= CAP_11ACVHT40 | CAP_11ACVHT20 | CAP_DS | CAP_TS | CAP_4S;
						else
							caps |= CAP_HT2040 | CAP_HT20;

						if (nvram_pf_match(prefix, "txbf", "1"))
							caps |= CAP_TXBF;
						break;
					case 2:	/* Legacy, MTK: A only ; QCA: A & N ; fall through */
					default:
						caps = 0;
						break;
				}
			}

			dbg("WDS:defcaps 0x%x\n", caps);
			fprintf(fp2,"%s %s wds 1\n", IWPRIV, wif);
			fprintf(fp, "wds_sta=1\n", wif);
			//fprintf(fp2,"iw phy %s interface add %s type managed 4addr on\n", iwphy, wif);
				if(WdsEncrypType==0)
				dbg("WDS:open/none\n");
			else if(WdsEncrypType==1){
				dbg("WDS:open/wep\n");
				fprintf(fp2,"iwconfig %s key [%d]\n",wif,wds_keyidx);
					if(strlen(wds_key)==10 || strlen(wds_key)==26)
					fprintf(fp2,"iwconfig %s key %s\n",wif,wds_key);
				else if(strlen(wds_key)==5 || strlen(wds_key)==13)
					fprintf(fp2,"iwconfig %s key \"s:%s\"\n",wif,wds_key);
			}else
				dbg("WDS:unknown\n");
#endif
			fclose(fp5);
		}
	}
	if(flag_8021x)
	{
		//radius server
		if (!strcmp(nvram_pf_safe_get(tmpfix, "radius_ipaddr"), ""))
			fprintf(fp, "auth_server_addr=169.254.1.1\n");
		else
			fprintf(fp, "auth_server_addr=%s\n", nvram_pf_safe_get(tmpfix, "radius_ipaddr"));

		//radius port
		str = nvram_pf_safe_get(tmpfix, "radius_port");
		if (str && strlen(str))
			fprintf(fp, "auth_server_port=%d\n", safe_atoi(str));
		else {
			warning = 50;
			fprintf(fp, "auth_server_port=1812\n");
		}

		//radius key
		str = nvram_pf_safe_get(tmpfix, "radius_key");
		if (str && strlen(str))
			fprintf(fp, "auth_server_shared_secret=%s\n", nvram_pf_safe_get(tmpfix, "radius_key"));
		else
			fprintf(fp,"#auth_server_shared_secret=\n");
	}	

	//RadioOn
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
#else
	str = nvram_pf_safe_get(tmpfix, "radio");
	if (str && strlen(str)) {
		char *updown = (safe_atoi(str) /* && nvram_get_int(strcat_r(prefix, "bss_enabled", tmp)) */ )? "up" : "down";
		fprintf(fp2, "ifconfig %s %s\n", wif, updown);
	}
#endif

#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(strstr(t_mode, "11AHE") || strstr(t_mode, "11GHE")){
		fprintf(fp, "ieee80211n=1\n");
		fprintf(fp, "ieee80211ac=1\n");
		fprintf(fp, "ieee80211ax=1\n");
	}else
#endif
	if(strstr(t_mode, "11ACV")){
		fprintf(fp, "ieee80211n=1\n");
		fprintf(fp, "ieee80211ac=1\n");
	}else if(strstr(t_mode, "11NG") || strstr(t_mode, "11NA")){
		fprintf(fp, "ieee80211n=1\n");
	}
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(strstr(t_mode, "11NG") || strstr(t_mode, "11NA")	|| strstr(t_mode, "11GHE") || strstr(t_mode, "11GEHT")){
		char *ht_str = NULL;
		if(!strstr(t_bw, "40")){
			strlcat(ht_capab, "[HT20]", sizeof(ht_capab));
			if(shortgi){
				snprintf(tmp, sizeof(tmp), "[SHORT-GI-%d]", 20);
				strlcat(ht_capab, tmp, sizeof(ht_capab));
			}
		}else if(pchattr && (ht_str = pchattr->ht_str) != NULL){
			strlcat(ht_capab, ht_str, sizeof(ht_capab));
			if(shortgi){
				snprintf(tmp, sizeof(tmp), "[SHORT-GI-%d]", 40);
				strlcat(ht_capab, tmp, sizeof(ht_capab));
			}
		}else{
			if(!channel)
				strlcat(ht_capab, "[HT40+][HT40-]", sizeof(ht_capab));
			if(shortgi){
				snprintf(tmp, sizeof(tmp), "[SHORT-GI-%d]", 40);
				strlcat(ht_capab, tmp, sizeof(ht_capab));
			}
		}
	}else if((strstr(t_mode, "11ACV") || strstr(t_mode, "11AHE")
		|| strstr(t_mode, "11AEHT")) && !strstr(t_bw, "80_80")){
		int puncture = 0, chwidth = 0;
		char *ht_str = NULL, *p_mode, t_chan[12] = "1";
		if(strstr(t_mode, "11AEHT")){
			puncture = 1;
			p_mode = "eht";
		}else if(strstr(t_mode, "11AHE"))
			p_mode = "he";
		else
			p_mode = "vht";

		if(strstr(t_bw, "20")){
			fprintf(fp, "%s_oper_chwidth=%d\n", p_mode, 0);
			fprintf(fp, "%s_oper_centr_freq_seg0_idx=%d\n", p_mode, channel);
			if(shortgi){
				snprintf(tmp, sizeof(tmp), "[SHORT-GI-%d]", 20);
				strlcat(ht_capab, tmp, sizeof(ht_capab));
			}
		}else if(strstr(t_bw, "40") || strstr(t_bw, "80") || strstr(t_bw, "160")){
			if(pchattr && (ht_str = pchattr->ht_str) != NULL){
				strlcat(ht_capab, ht_str, sizeof(ht_capab));
			}else{
				if(channel || !strstr(t_bw, "40")){
					if(strstr(t_bw, "20") || strstr(t_bw, "40")){
						if(pchattr)
							snprintf(t_chan, sizeof(t_chan), "%d", pchattr->chan_low);
						chwidth = 0;
						fprintf(fp, "%s_oper_chwidth=%d\n", p_mode, chwidth);
						fprintf(fp, "%s_oper_centr_freq_seg0_idx=%s\n", p_mode, t_chan);
					}else if(strstr(t_bw, "80") && !strstr(t_bw, "80_80")){
						if(pchattr)
							snprintf(t_chan, sizeof(t_chan), "%d", pchattr->chan_high);
						chwidth = 1;
						fprintf(fp, "%s_oper_chwidth=%d\n", p_mode, chwidth);
						fprintf(fp, "%s_oper_centr_freq_seg0_idx=%s\n", p_mode, t_chan);
					}else if(strstr(t_bw, "160")){
						if(pchattr)
							snprintf(t_chan, sizeof(t_chan), "%d", pchattr->chan_cen);
						chwidth = 2;
						fprintf(fp, "%s_oper_chwidth=%d\n", p_mode, chwidth);
						fprintf(fp, "%s_oper_centr_freq_seg0_idx=%s\n", p_mode, t_chan);
					}else{
						dbg("%s: Can't resolve chattr %pt_bw [%s]\n", __func__, pchattr, t_bw);
					}
					if(puncture)
						fprintf(fp, "puncture_bitmap=0x%x\n", 0xFFFF);
					if(shortgi){
						snprintf(tmp, sizeof(tmp), "[SHORT-GI-%d]", 40);
						strlcat(ht_capab, tmp, sizeof(ht_capab));
					}
				}else
					strlcat(ht_capab, "[HT40+][HT40-]", sizeof(ht_capab));
			}
		}
	}
	snprintf(tmp, sizeof(tmp), "%s/%s/cfg80211_htcaps", "/sys/class/net", wif);
	f_read_alloc_string(tmp, &p, 1024);
	if(p){
		fprintf(fp, "ht_capab=%s%s\n", p, ht_capab);
		free(p);
	}
	snprintf(tmp, sizeof(tmp), "%s/%s/cfg80211_vhtcaps", "/sys/class/net", wif);
	f_read_alloc_string(tmp, &p, 1024);
	if(p){
		fprintf(fp, "vht_capab=%s\n", p);
		free(p);
	}
	if(band)
		fprintf(fp, "hw_mode=%s\n", "a");
	else
		fprintf(fp, "hw_mode=%s\n", "g");
	str = nvram_pf_safe_get(main_prefix, "dtim");
	if (!str || *str == '\0') {
		warning = 11;
		str = "1";
	}
	fprintf(fp, "dtim_period=%d\n", safe_atoi(str));
	if(repeater_mode())
		fprintf(fp, "wds_sta=1\n");
	fprintf(fp, "ignore_broadcast_ssid=%d\n", nvram_pf_get_int(prefix, "closed") != 0);
	fprintf(fp, "owe_ptk_workaround=1\n");
	str = nvram_pf_safe_get(tmpfix, "radio");
	if (str && strlen(str)) {
		char *updown = safe_atoi(str) ? "up" : "down";
		fprintf(fp2, "ifconfig %s %s\n", wif, updown);
	}
#endif
	gen_hostapd_wps_config(fp, band, subnet, br_if);

#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(nvram_pf_match(prefix2, "auth_mode_x", "sae") || nvram_pf_match(prefix2, "auth_mode_x", "wpa3")
		|| nvram_pf_match(prefix2, "auth_mode_x", "owe") || nvram_pf_match(prefix2, "auth_mode_x", "suite-b")){
		if(nvram_pf_get_int(prefix2, "mfp") != 2)
			nvram_pf_set_int(prefix2, "mfp", 2);
		fprintf(fp, "ieee80211w=%d\n", 2);
	}else if(nvram_pf_match(prefix2, "auth_mode_x", "psk2sae")
		|| nvram_pf_match(prefix2, "auth_mode_x", "wpawpa2")
		|| nvram_pf_match(prefix2, "auth_mode_x", "wpa2wpa3")){
		if(nvram_pf_get_int(prefix2, "mfp") == 1 || (subnet || repeater_mode()))
			nvram_pf_set_int(prefix2, "mfp", 1);
		fprintf(fp, "ieee80211w=%d\n", 1);
	}else{
		if(nvram_pf_get_int(prefix2, "mfp") == 0 || (subnet || repeater_mode()))
			nvram_pf_set_int(prefix2, "mfp", 0);
		fprintf(fp, "ieee80211w=%d\n", 0);
	}
#endif
	fprintf(fp, "acs_num_scans=1\n");
	fprintf(fp, "acs_smart_info_file=/tmp/acs_smart_info_%s.txt\n", wif);
	fprintf(fp, "acs_history_file=/tmp/acs_history_%s.txt\n", wif);
next:
	fclose(fp);
	fclose(fp2);
	fclose(fp3);
	chmod(path2, 0777);	/* postwifi_athX.sh */
	chmod(path3, 0777);	/* prewifi_athX.sh */
//	chmod(path5, 0777);	/* nawds_athX.sh */

	(void) warning;
	return 0;
}

void Get_fail_log(char *buf, int size, unsigned int offset)
{
	struct FAIL_LOG fail_log, *log = &fail_log;
	char *p = buf;
	int x, y;

	memset(buf, 0, size);
	FRead((char *)&fail_log, offset, sizeof(fail_log));
	if (log->num == 0 || log->num > FAIL_LOG_MAX) {
		return;
	}
	for (x = 0; x < (FAIL_LOG_MAX >> 3); x++) {
		for (y = 0; log->bits[x] != 0 && y < 7; y++) {
			if (log->bits[x] & (1 << y)) {
				p += snprintf(p, size - (p - buf), "%d,", (x << 3) + y);
			}
		}
	}
}

void ate_commit_bootlog(char *err_code)
{
	printf("[ATE] err_code:[%s]\n", err_code);
	nvram_set("Ate_power_on_off_enable", err_code);
	nvram_commit();
}

void platform_start_ate_mode(void)
{
	int model = get_model();
	switch (model) {
		case MODEL_BLUECAVE:
		case MODEL_RAX40:
		break;
	default:
		_dprintf("%s: model %d\n", __func__, model);
	}
}

/* Run iwlist command to do site-survey.
 * @ssv_if:
 * @return:
 *     -1:	invalid parameter
 * 	0:	site-survey fail
 * 	1:	site-survey success
 * NOTE:	sitesurvey filelock must be hold by caller!
 */
static int do_sitesurvey(char *ssv_if)
{
	int retry, ssv_ok = 0;
	char *result, *p;
	char *iwlist_argv[] = { "iwlist", ssv_if, "scanning", NULL };

	if (!ssv_if || *ssv_if == '\0')
		return -1;

	for (retry = 0, ssv_ok = 0; !ssv_ok && retry < 1; ++retry) {
		_eval(iwlist_argv, ">/tmp/apscan_wlist", 0, NULL);

		if (!f_exists(APSCAN_WLIST) || !(result = file2str(APSCAN_WLIST)))
			continue;
		if (!(p = strstr(result, "Scan completed"))) {
			if ((p = strchr(result, '\n')))
				*p = '\0';
			if ((p = strchr(result, '\r')))
				*p = '\0';
			_dprintf("%s: iwlist %s scanning fail!! (%s)!\n", __func__, ssv_if, result);
			free(result);
			continue;
		}

		free(result);
		ssv_ok = 1;
	}

	return ssv_ok;
}

#define target 9
char str[target][40]={"Address:","ESSID:","Frequency:","Quality=","Encryption key:","IE:","Authentication Suites","Pairwise Ciphers","phy_mode="};

int getSiteSurvey(int band, char* ofile)
{
	int apCount=0;
	char header[128];
	FILE *fp,*ofp;
	char buf[target][200],set_flag[target];
	int i, ssv_ok = 0, radio, is_sta = 0, bitrate;
	char *pt1, *pt2, *pt3;
	char a1[10],a2[10];
	char ssid_str[256], ssv_if[10];
	char ch[4] = "", ssid[33] = "", address[18] = "", enc[9] = "";
	char auth[16] = "", sig[9] = "", wmode[8] = "";
	int lock, wap_ver = 0;
	char *staifname = get_staifname(band);
#if defined(RTCONFIG_WIRELESSREPEATER)
	char ure_mac[18];
	int wl_authorized = 0;
#endif
	int is_ready, wlc_band = -1;
	char temp1[200];
	char prefix_header[]="Cell xx - Address:";

	dbG("site survey...\n");
	if (band < 0 || band >= MAX_NR_WL_IF)
		return 0;
#if defined(RTCONFIG_WIRELESSREPEATER)
	if (nvram_get("wlc_band") && (repeater_mode() || mediabridge_mode()
#if defined(RTCONFIG_WISP)
		|| wisp_mode()
#endif
		))
		wlc_band = nvram_get_int("wlc_band");
#endif
	lock = file_lock("sitesurvey");
	do_sitesurvey(staifname);
	file_unlock(lock);
	if (!(fp= fopen(APSCAN_WLIST, "r")))
		return 0;
	snprintf(header, sizeof(header), "%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode");
	dbg("\n%s", header);
	if ((ofp = fopen(ofile, "a")) == NULL)
	{
		fclose(fp);
		return 0;
	}

	apCount=1;
	while(1)
	{
		is_ready=0;
		memset(set_flag,0,sizeof(set_flag));
		memset(buf,0,sizeof(buf));
		memset(temp1,0,sizeof(temp1));
		snprintf(prefix_header, sizeof(prefix_header), "Cell %02d - Address:",apCount);

		if(feof(fp))
			break;

		while(fgets(temp1,sizeof(temp1),fp))
		{
			if(strstr(temp1,prefix_header)!=NULL)
			{
				if(is_ready)
				{
					fseek(fp,-sizeof(temp1), SEEK_CUR);
					break;
				}
				else
				{
					is_ready=1;
					snprintf(prefix_header, sizeof(prefix_header),"Cell %02d - Address:",apCount+1);
				}	
			}
			if(is_ready)
			{
				for(i=0;i<target;i++)
				{
					if(strstr(temp1,str[i])!=NULL && set_flag[i]==0)
					{
						set_flag[i]=1;
						memcpy(buf[i],temp1,sizeof(temp1));
						break;
					}
				}
			}
		}

		dbg("\napCount=%d\n",apCount);
		apCount++;
		//ch
		pt1 = strstr(buf[2], "Channel ");	
		if(pt1)
		{

			pt2 = strstr(pt1,")");
			memset(ch,0,sizeof(ch));
			strncpy(ch,pt1+strlen("Channel "),pt2-pt1-strlen("Channel "));
		}

		//ssid
		pt1 = strstr(buf[1], "ESSID:");	
		if(pt1)
		{
			memset(ssid,0,sizeof(ssid));
			strncpy(ssid,pt1+strlen("ESSID:")+1,strlen(buf[1])-2-(pt1+strlen("ESSID:")+1-buf[1]));
		}


		//bssid
		pt1 = strstr(buf[0], "Address: ");	
		if(pt1)
		{
			memset(address,0,sizeof(address));
			strncpy(address,pt1+strlen("Address: "),strlen(buf[0])-(pt1+strlen("Address: ")-buf[0])-1);
		}

		//enc
		pt1=strstr(buf[4],"Encryption key:");
		if(pt1)
		{
			if(strstr(pt1+strlen("Encryption key:"),"on"))
			{
				pt2=strstr(buf[7],"Pairwise Ciphers");
				if(pt2)
				{
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
					if(strstr(pt2,"GCMP-256") && strstr(pt2,"CCMP"))
						strlcpy(enc, "AES+GCMP256", sizeof(enc));
					else
#endif
					if(strstr(pt2,"CCMP TKIP") || strstr(pt2,"TKIP CCMP"))
						strlcpy(enc, "TKIP+AES", sizeof(enc));
					else if(strstr(pt2,"CCMP"))
						strlcpy(enc, "AES", sizeof(enc));
					else
						strlcpy(enc, "TKIP", sizeof(enc));
				}
				else
					strlcpy(enc, "WEP", sizeof(enc));
			}
			else
				strlcpy(enc, "NONE", sizeof(enc));
		}

		//auth
		memset(auth,0,sizeof(auth));
		pt1=strstr(buf[5],"IE:");
		if(pt1 && strstr(buf[5],"Unknown")==NULL)
		{
			if(strstr(pt1+strlen("IE:"),"WPA2")!=NULL)
				wap_ver = 2;
			else if(strstr(pt1+strlen("IE:"),"WPA")!=NULL) 
				wap_ver = 1;
			else
				_dprintf("%s:%d WARNING!! unknown AUTH in 1st IE:[%s]\n", __func__, __LINE__, pt1);
#if 0
			if(set_flag[6]){
				pt2=strstr(buf[6],"IE:");
				if(pt2 && !strstr(buf[6],"Unknown")){
					if(strstr(pt2+strlen("IE:"),"WPA2")!=NULL)
						wap_ver |= 2;
					else if(strstr(pt1+strlen("IE:"),"WPA")!=NULL)
						wap_ver |= 1;
					else
						_dprintf("%s:%d WARNING!! unknown AUTH in 2st IE:[%s]\n", __func__, __LINE__, pt2);
				}
			}
#endif
			switch(wap_ver){
				case 2:
					strlcpy(auth, "WPA2-", sizeof(auth));
				case 3:
					strlcpy(auth, "WPA-WPA2-", sizeof(auth));
				case 1:
					strlcpy(auth, "WPA-", sizeof(auth));
					pt2=strstr(buf[6],"Authentication Suites");
					if(pt2){
						if(strstr(pt2+strlen("Authentication Suites"),"SAE")!=NULL){
							if(strstr(pt2+strlen("Authentication Suites"),"PSK")!=NULL)
								strlcpy(auth,"WPA2-WPA3-Personal", sizeof(auth));
							else
								strlcpy(auth,"WPA3-Personal", sizeof(auth));
						}else if(strstr(pt2+strlen("Authentication Suites"),"PSK")!=NULL){
							if(strstr(enc, "AES") || strstr(enc, "TKIP"))
								strlcat(auth, "Personal", sizeof(auth));
							else
								dbg("%s: Unknown psk auth. (s [%s] enc [%s])\n", __func__, pt2+strlen("Authentication Suites"), enc);
						}else if(strstr(pt2+strlen("Authentication Suites"),"OWE")!=NULL){
							strlcpy(auth,"Enhanced Open", sizeof(auth));
						}else if(strstr(pt2+strlen("Authentication Suites"),"802.1x")!=NULL){
							if(strstr(enc, "AES") || strstr(enc, "TKIP"))
								strlcat(auth, "Enterprise", sizeof(auth));
							else
								dbg("%s: Unknown psk auth. (s [%s] enc [%s])\n", __func__, pt2+strlen("Authentication Suites"), enc);
						}else
							_dprintf("%s:%d ERROR!! NO AKM TYPE Set!!\n", __func__, __LINE__);
					}else{//bug!!
						if(strstr(enc, "AES") || strstr(enc, "TKIP"))
							strlcat(auth, "Personal", sizeof(auth));
					}
				default:
					_dprintf("%s:%d ERROR!! NO AUTH TYPE Set!!\n", __func__, __LINE__);
					break;
			}
		}
		else
		{
			if(strcmp(enc,"WEP")==0)
			{
				strlcpy(auth, "Unknown", sizeof(auth));
			}else{
				strlcpy(auth, "Open System", sizeof(auth));
			}
		}

		//sig
		pt1 = strstr(buf[3], "Quality=");	
		pt2 = NULL;
		if (pt1 != NULL)
			pt2 = strstr(pt1,"/");
		if(pt1 && pt2)
		{
			memset(sig,0,sizeof(sig));
			memset(a1,0,sizeof(a1));
			memset(a2,0,sizeof(a2));
			strncpy(a1,pt1+strlen("Quality="),pt2-pt1-strlen("Quality="));
			strncpy(a2,pt2+1,strstr(pt2," ")-(pt2+1));
			snprintf(sig, sizeof(sig), "%d", 100*(atoi(a1)+6)/(atoi(a2)+6));
		}

		//wmode
		memset(wmode,0,sizeof(wmode));
		pt1=strstr(buf[8],"phy_mode=");
		if(pt1)
		{
			pt2 = pt1+strlen("phy_mode=");
			if(strstr(pt2, "11AC_VHT")!=NULL)
				strlcpy(wmode, "ac", sizeof(wmode));
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			else if(strstr(pt2,"11AXA_HE")!=NULL || strstr(pt2,"11AXG_HE")!=NULL)
				strlcpy(wmode, "ax", sizeof(wmode));
#endif
			else if(strstr(pt2,"11A")!=NULL || strstr(pt2,"TURBO_A")!=NULL)
				strlcpy(wmode, "a", sizeof(wmode));
			else if(strstr(pt2,"11B")!=NULL)
				strlcpy(wmode, "b", sizeof(wmode));
			else if(strstr(pt2,"11G")!=NULL || strstr(pt2,"TURBO_G")!=NULL)
				strlcpy(wmode, "bg", sizeof(wmode));
			else if(strstr(pt2,"11NA")!=NULL)
				strlcpy(wmode, "an", sizeof(wmode));
			else if(strstr(pt2,"11NG")!=NULL)
				strlcpy(wmode, "bgn", sizeof(wmode));
			else
				dbg("%s: Unknown phymode [%s]\n", __func__, pt2);
		}
		else
			strlcpy(wmode, "unknown", sizeof(wmode));

		dbg("%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n",ch,ssid,address,enc,auth,sig,wmode);

		if(safe_atoi(ch)<0)
			fprintf(ofp, "\"ERR_BAND\",");
		else if(safe_atoi(ch)>0 && safe_atoi(ch)<14)
			fprintf(ofp, "\"2G\",");
		else if(safe_atoi(ch)>14 && safe_atoi(ch)<166)
			fprintf(ofp, "\"5G\",");
		else
			fprintf(ofp, "\"ERR_BAND\",");


		memset(ssid_str, 0, sizeof(ssid_str));
#if defined(RTCONFIG_UTF8_SSID)
		char_to_ascii_with_utf8(ssid_str, trim_r(ssid));
#else
		char_to_ascii(ssid_str, trim_r(ssid));
#endif
		
		if(strlen(ssid)==0)
			fprintf(ofp, "\"\",");
		else
			fprintf(ofp, "\"%s\",", ssid_str);

		fprintf(ofp, "\"%d\",", safe_atoi(ch));

		fprintf(ofp, "\"%s\",",auth);
		
		fprintf(ofp, "\"%s\",", enc); 

		fprintf(ofp, "\"%d\",", safe_atoi(sig));

		fprintf(ofp, "\"%s\",", address);

		fprintf(ofp, "\"%s\",", wmode); 

#ifdef RTCONFIG_WIRELESSREPEATER		
		//memset(ure_mac, 0x0, 18);
		//snprintf(ure_mac, sizeof(ure_mac), "%02X:%02X:%02X:%02X:%02X:%02X",xxxx);
		if (strcmp(nvram_safe_get(wlc_nvname("ssid")), ssid)){
			if (strcmp(ssid, ""))
				fprintf(ofp, "\"%s\"", "0");				// none
			else if (!strcmp(ure_mac, address)){
				// hidden AP (null SSID)
				if (strstr(nvram_safe_get(wlc_nvname("akm")), "psk")!=NULL){
					if (wl_authorized){
						// in profile, connected
						fprintf(ofp, "\"%s\"", "4");
					}else{
						// in profile, connecting
						fprintf(ofp, "\"%s\"", "5");
					}
				}else{
					// in profile, connected
					fprintf(ofp, "\"%s\"", "4");
				}
			}else{
				// hidden AP (null SSID)
				fprintf(ofp, "\"%s\"", "0");				// none
			}
		}else if (!strcmp(nvram_safe_get(wlc_nvname("ssid")), ssid)){
			if (!strlen(ure_mac)){
				// in profile, disconnected
				fprintf(ofp, "\"%s\",", "1");
			}else if (!strcmp(ure_mac, address)){
				if (strstr(nvram_safe_get(wlc_nvname("akm")), "psk")!=NULL){
					if (wl_authorized){
						// in profile, connected
						fprintf(ofp, "\"%s\"", "2");
					}else{
						// in profile, connecting
						fprintf(ofp, "\"%s\"", "3");
					}
				}else{
					// in profile, connected
					fprintf(ofp, "\"%s\"", "2");
				}
			}else{
				fprintf(ofp, "\"%s\"", "0");				// impossible...
			}
		}else{
			// wl0_ssid is empty
			fprintf(ofp, "\"%s\"", "0");
		}
#else
		fprintf(ofp, "\"%s\"", "0");      
#endif
		fprintf(ofp, "\n");
	}
	fclose(fp);
	fclose(ofp);
	return 1;
}

char *getStaMAC(char *buf, int buflen)
{
	FILE *fp;
	int len;
	char *pt1,*pt2;
	char cmd[512];

	snprintf(cmd, sizeof(cmd), "ifconfig %s", get_staifname(nvram_get_int("wlc_band")));

	fp = PS_popen(buf, "r");
	if (fp) {
		memset(buf, 0, buflen);
		len = fread(buf, 1, buflen, fp);
		PS_pclose(fp);
		if (len > 1) {
			buf[len - 1] = 0x0;
			pt1 = strstr(buf, "HWaddr ");
			if (pt1) 
			{
				pt1[24] = 0x0;
				pt2 = pt1 + strlen("HWaddr ");
				return pt2;
			}
		}
	}
	return NULL;
}

unsigned int getPapState(int unit)
{
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	int ret = 0;
	char ctrl_sk[32];
	char buf2[64] = {0};
	char *wpa_state;
	FILE *fp;
#else
	char buf[8192], sta[64];
	FILE *fp;
	int len;
	char *pt1, *pt2;
#endif
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(!nvram_get_int(WLREADY)) return 0;
	get_wpa_ctrl_sk(unit, ctrl_sk, sizeof(ctrl_sk));
	snprintf(buf2, sizeof(buf2), "wpa_cli -p %s -i %s status", ctrl_sk, get_staifname(unit));
	fp = PS_popen(buf2, "r");
	if(fp){
		while(fgets(buf2, sizeof(buf2), fp) != NULL){
			if(strstr(buf2, "wpa_state")){
				if(strstr(buf2, "COMPLETED") || strstr(buf2, "ASSOCIATED"))
					ret = 2;
				else if(strstr(buf2, "4WAY_HANDSHAKE"))
					ret = 1;
			}
		}
		PS_pclose(fp);
	}else
		ret = 3;
	return ret;
#else
	strcpy(sta, get_staifname(unit));
	sprintf(buf, "iwconfig %s", sta);
	fp = popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, "Access Point:");
			if (pt1) {
				pt2 = pt1 + strlen("Access Point:");
				pt1 = strstr(pt2, "Not-Associated");
				if (pt1) 
				{
					sprintf(buf, "ifconfig | grep %s", sta);
				     	fp = popen(buf, "r");
					if(fp)
				   	{
						 memset(buf, 0, sizeof(buf));
						 len = fread(buf, 1, sizeof(buf), fp);
						 pclose(fp);
						 if(len>=1)
						    return 0;
						 else
						    return 3;
					}	
				     	else	
				   		return 0; //init
				}	
				else
				   	return 2; //connect and auth ?????
					
				}
			}
		}
	
	return 3; // stop
#endif
}

int wlcconnect_core(void)
{
	int unit, result;
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	static int reconnected;
	int shortgi = -1, i;
	char buf[18] = {0};
	char ifname[16] = {0}, ifname2[16] = {0}, prefix[8];
#endif
	result = getPapState(nvram_get_int("wlc_band"));
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
	if(result!=2){
		dbG("check..wlconnect=%d \n",result);
		reconnected = 0;
	}else{
		memset(buf, 0, sizeof(buf));
		strlcpy(ifname, get_staifname(unit), sizeof(ifname));
		strlcpy(buf, iwpriv_get(ifname, "get_mode") ? : "", sizeof(buf));
		if(!strncmp(buf, "11GHE", 5) || !strncmp(buf, "11AHE", 5)
			|| !strncmp(buf, "11GEHT", 6) || !strncmp(buf, "11AEHT", 6)){
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			for(i = 0; i <= num_of_mssid_support(unit); i++){
				get_wlxy_ifname(unit, i, ifname2);
				if(iface_exist(ifname2)){
					strlcpy(buf, iwpriv_get(ifname2, "get_mode") ? : "", sizeof(buf));
					if((!strncmp(buf, "11GHE", 5) || !strncmp(buf, "11AHE", 5) || !strncmp(buf, "11GEHT", 6) || !strncmp(buf, "11AEHT", 6)) && iwpriv_get_int(ifname2, "get_shortgi", &shortgi) >= 0 && shortgi == 1){
						eval(IWPRIV, ifname2, "shortgi", "0");
					}
				}
			}
		}
		if(!reconnected && nvram_pf_match(prefix, "nband", "1")){
			doSystem("%s %s mode %s", IWPRIV, ifname, sta_default_mode(unit, 1));
			reconnected = 1;
		}
	}
#else
	if(result != 2)
		dbg("%s(0x%04x): check..wlconnect=%d \n", __func__, __LINE__, result);
#endif
	return result;
}

int wlcscan_core(char *ofile, char *wif)
{
	int count = 0;
	while (count != 2)
	{
		if(getSiteSurvey(get_wifname_band(wif), ofile))
			break;
		count++;
		dbg("[rc] set scan results command failed, retry %d\n", count);
		sleep(1);
	}
	return 0;
}

int ubi_remove_dev(const char *mtdname, int remove)
{
//deprecated function
	return 0;
}

void check_ubi_partition(void)
{
	int dev, part, size;

	fprintf(stderr, "... check_ubi_partition() ...\n");
#if defined(K3C) || defined(BLUECAVE)
	MKNOD("/dev/ubi1", S_IFCHR | 0666, makedev(248, 0));
	MKNOD("/dev/ubi1_0", S_IFCHR | 0666, makedev(248, 1));

	fprintf(stderr, "... start ubiattach ...\n");
#if defined(K3C)
	system("ubiattach /dev/ubi_ctrl -m 7");
#elif defined(BLUECAVE)
	system("ubiattach /dev/ubi_ctrl -m 8");
#else
#error fix me
#endif

	while(pids("ubiattach"))
	{
		fprintf(stderr, "... waiting ubiattach finished ...\n");
		sleep(1);
	}
	fprintf(stderr, "... ubiattach finished ...\n");
#endif
	if((ubi_getinfo("jffs2", &dev, &part, &size)) != 0)
	{
#if defined(K3C) || defined(BLUECAVE)
#if defined(K3C)
		fprintf(stderr, "... detach mtd7 ...\n");
		system("ubidetach -p /dev/mtd7");
    	fprintf(stderr, "... start flash_erase ...\n");
		system("flash_erase /dev/mtd7 0 0");
#elif defined(BLUECAVE)
		fprintf(stderr, "... detach mtd8 ...\n");
		system("ubidetach -p /dev/mtd8");
    	fprintf(stderr, "... start flash_erase ...\n");
		system("flash_erase /dev/mtd8 0 0");
#else
#error fix me
#endif
		while(pids("flash_erase"))
		{
			fprintf(stderr, "... waiting flash_erase finished ...\n");
			sleep(1);
		}
		fprintf(stderr, "... flash_erase finished ...\n");
		fprintf(stderr, "... start ubiattach ...\n");
#if defined(K3C)
		system("ubiattach /dev/ubi_ctrl -m 7");
#elif defined(BLUECAVE)
		system("ubiattach /dev/ubi_ctrl -m 8");
#else
#error fix me
#endif

		while(pids("ubiattach"))
		{
			fprintf(stderr, "... waiting ubiattach finished ...\n");
			sleep(1);
		}
		fprintf(stderr, "... ubiattach finished ...\n");
#endif
		fprintf(stderr, "... start ubimkvol ...\n");
#if defined(RAX40)
		if((ubi_getinfo("data_vol", &dev, &part, &size)) == 0)
			system("ubirmvol /dev/ubi0 -N data_vol");
		system("ubimkvol /dev/ubi0 -N jffs2 -s 100MiB");
		system("ubimkvol /dev/ubi0 -N nvram -s 1MiB");
#else
		system("ubimkvol /dev/ubi1 -N jffs2 -m");
#endif
		while(pids("ubimkvol"))
		{
			fprintf(stderr, "... waiting ubimkvol finished ...\n");
			sleep(1);
		}
		fprintf(stderr, "... ubimkvol finished ...\n");
	}
	fprintf(stderr, "... ubi_getinfo() finished ...\n");
}

int get_usb_mode()
{
	int ret = 0;
	FILE *fp;
	char *tmp;
	unsigned int value;
	char buf[64];

	memset(buf, 0, sizeof(buf));
	if((fp = popen("mem -s 0x1a40c020 -du", "r")) != NULL)
	{
		fgets(buf, sizeof(buf), fp);
		tmp = strchr(buf, ':');
		if (tmp)
		{
			value = strtol(tmp + 1, NULL, 16);
			_dprintf("Read 0x1a40c020 as [0x%08x]\n", value);
			ret = 1;
			if(value == 0x2000)
				ret = 0;
			else if (value)
				ret = -1;
		} else
			ret = 0;
		pclose(fp);
	}
	return ret;
}

void gen_config_sh(void)
{
//	system("cp -f /rom/opt/lantiq/etc/rc.d/config.sh /etc/; cd /etc/rc.d; ln -s ../config.sh config.sh");
}

void usb_pwr_ctl(int onoff)
{
	FILE *fp;
	char *tmp;
	unsigned int value;
	char cmd[64];
	char buf[64];


	memset(buf, 0, sizeof(buf));
	if((fp = popen("mem -s 0x16c00000 -du", "r")) != NULL)
	{
		fgets(buf, sizeof(buf), fp);
		tmp = strchr(buf, ':');
		if(tmp)
		{
			value = strtol(tmp + 1, NULL, 16);
			_dprintf("Read 0x16c00000 as [0x%08x]\n", value);
			if(onoff == 1)
			{
				_dprintf("Set usb power [on]\n");
				value = value | 0x80;
			} else {
				_dprintf("Set usb power [off]\n");
		    	value = value & 0xFFFFFF7F;
			}
			_dprintf("Write 0x16c00000 as [0x%08x]\n", value);
			snprintf(cmd, sizeof(cmd), "mem -s 0x16c00000 -w 0x%08x -u", value);
			system(cmd);
		}
		pclose(fp);
	}
}

void set_usb3_to_usb2()
{
	if (!get_usb_mode()){
		_dprintf("[usb] already USB2 mode, skip\n");
		return;
	}
	_dprintf("[wait] usb3 to usb2 start\n");
	__ejusb_main("-1", 0);
	notify_rc_after_wait("restart_nasapps");
	_dprintf("[warning] power off usb and change mode\n");
	usb_pwr_ctl(0);
	system("mem -s 0x1a40c020 -uw 0x2000");
	usb_pwr_ctl(1);
	_dprintf("[wait] usb3 to usb2 end\n");
}

void set_usb2_to_usb3()
{
	if (get_usb_mode() == 1){
		_dprintf("[usb] already USB3 mode, skip\n");
		return;
	}
	_dprintf("[wait] usb2 to usb3 start\n");
	__ejusb_main("-1", 0);
	notify_rc_after_wait("restart_nasapps");
	_dprintf("[warning] power off usb and change mode\n");
	usb_pwr_ctl(0);
	system("mem -s 0x1a40c020 -uw 0x0");
	usb_pwr_ctl(1);
	_dprintf("[wait] usb2 to usb3 end\n");
}

void apply_config_to_driver()
{
	char prefix[16] = {0}, wlc_prefix[16] = {0};
	snprintf(wlc_prefix, sizeof(wlc_prefix), "wlc%d_", band);
	snprintf(prefix, sizeof(prefix), "wl%d_", band);
	copy_rpt_params_to_wl(prefix, wlc_prefix);
	write_rpt_wpa_supplicant_conf(band, wlc_prefix, NULL, NULL);
	set_wpa_cli_cmd(band, "reconfigure", 0);
	set_wpa_cli_cmd(band, "enable_network all", 0);
}

int Pty_get_wlc_status(int band)
{
	return 0;
}

int Pty_start_wlc_connect(int band)
{
	int result = 0;
	char *sta;

	sta = get_staifname(band);
	if(!is_if_up(sta))
		result = eval("ifconfig", sta, "up");

	return result;
}

int get_psta_rssi(int band)
{
	char *sta;
	FILE *fp;
	int ant;
	int trssi;
	int result;
	int rssi;
	char tmp[128], buf[128];
	int signal;

	memset(tmp, 0, sizeof(tmp));
	signal = 0;
	sta = get_staifname(band);
	snprintf(tmp, sizeof(tmp), "/proc/net/mtlk/%s/PeerFlowStatus", sta);
	if ((fp = fopen(tmp, "r")) != NULL)
	{
		ant = 0;
		trssi = 0;
		memset(buf, 0, sizeof(buf));
		while (fgets(buf, sizeof(buf), fp))
		{
			if(strstr(buf, "RSSI"))
			{
				sscanf(buf, "%d", &signal);
				++ant;
				trssi += signal;
			}
		}
		fclose(fp);
		if (!ant)
			return pre_rssi[band];
		rssi = trssi/ant;
		if(rssi == -128)
			return pre_rssi[band];
		result = -1;
		if(rssi < 0)
		{
			result = pre_rssi[band];
			if (rssi >= -91)
				result = rssi;
		}
	} else
		result = pre_rssi[band];

	return result;
}

int Pty_stop_wlc_connect(int band)
{
	int result = 0;
	char *sta;

	sta = get_staifname(band);
	if(is_if_up(sta))
		result = eval("ifconfig", sta, "down");

	return result;
}

int Pty_get_upstream_rssi(int band)
{
	return get_psta_rssi(band);
}

int get_psta_status(int unit)
{
	return getPapState(unit);
}

void wlconf_pre()
{
	generate_wl_para(0, -1);
	generate_wl_para(1, -1);
}

enum {
	IFTYPE_WAN = 1,
	IFTYPE_2G = 2,
	IFTYPE_5G = 4,
	IFTYPE_5G2 = 8
};

void set_pre_sysdep_config(int iftype)
{
	char word[256], *next = NULL, cmd[64], ifnames[128];
	memset(cmd, 0, sizeof(cmd));
	strcpy(ifnames, nvram_safe_get("sta_phy_ifnames"));
	foreach(word, ifnames, next) {
		snprintf(cmd, sizeof(cmd), "ppacmd dellan -i %s", word);
		doSystem(cmd);
	}
	strcpy(ifnames, nvram_safe_get("eth_ifnames"));
	foreach(word, ifnames, next) {
		snprintf(cmd, sizeof(cmd), "ppacmd dellan -i %s", word);
		doSystem(cmd);
	}
}

void set_post_sysdep_config(int iftype)
{
	char word[256], *next = NULL, cmd[64], ifnames[128];
	int result, i;
	memset(cmd, 0, sizeof(cmd));
	if(iftype == IFTYPE_WAN){
		strcpy(cmd, "ppacmd addlan -i eth1");
		doSystem(cmd);
	} else {
		result = 7;	
		if(iftype){
			result = (iftype & IFTYPE_2G) != 0;
			if((iftype & IFTYPE_5G) != 0)
				result |= 2;
		}
		if((iftype & IFTYPE_5G2) != 0)
			result |= 4;
		strcpy(ifnames, nvram_safe_get("sta_ifnames"));
		i = 0;
		foreach(word, ifnames, next) {
			if(((result >> i) & 1) != 0){
				snprintf(cmd, sizeof(cmd), "echo \"add %s\" > /proc/l2nat/dev", word);
				doSystem(cmd);
				snprintf(cmd, sizeof(cmd), "ppacmd addlan -i %s", word);
				doSystem(cmd);
			}
			++i;
		}
	}
}

int get_radar_status(int bssidx)
{
	if(bssidx)
		return nvram_get_int("radar_status");
	else
		return 0;
}

int wps_ascii_to_char_with_utf8(char *output, const char *input, int outsize)
{
	char dest[28];
	const char *instr = input;
	char *word;
	const char *end = output + outsize - 1;

	if(!output || outsize <= 0 || !input)
		return 0;

	for(word = output; *instr && word != end; ++word)
	{
		memset(dest, 0, sizeof(dest));
		strncpy(dest, instr, 2);
		instr += 2;
		*word = strtol(dest, NULL, 16);
	}
	if(word != end)
		*word = 0;
	return 0;
}

#ifdef LANTIQ_BSD
void bandstr_sync_wl_settings(void)
{
	char tmp[100], tmp2[100], prefix[16] = "wlXXXXXXXXXX_", prefix2[16] = "wlXXXXXXXXXX_";
	int i, n;
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	n = num_of_wl_if();
	snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	for(i = 1; i < n; i++){
		snprintf(prefix2, sizeof(prefix2), "wl%d_", i);
		nvram_pf_set(prefix2, "ssid",  nvram_pf_safe_get(prefix, "ssid"));
		nvram_pf_set(prefix2, "auth_mode_x", nvram_pf_safe_get(prefix, "auth_mode_x"));
		nvram_pf_set(prefix2, "wep_x", nvram_pf_safe_get(prefix, "wep_x"));
		nvram_pf_set(prefix2, "key", nvram_pf_safe_get(prefix, "key"));
		nvram_pf_set(prefix2, "key1", nvram_pf_safe_get(prefix, "key1"));
		nvram_pf_set(prefix2, "key2", nvram_pf_safe_get(prefix, "key2"));
		nvram_pf_set(prefix2, "key3", nvram_pf_safe_get(prefix, "key3"));
		nvram_pf_set(prefix2, "key4", nvram_pf_safe_get(prefix, "key4"));
		nvram_pf_set(prefix2, "phrase_x", nvram_pf_safe_get(prefix, "phrase_x"));
		nvram_pf_set(prefix2, "crypto", nvram_pf_safe_get(prefix, "crypto"));
		nvram_pf_set(prefix2, "wpa_psk", nvram_pf_safe_get(prefix, "wpa_psk"));
		nvram_pf_set(prefix2, "radius_ipaddr", nvram_pf_safe_get(prefix, "radius_ipaddr"));
		nvram_pf_set(prefix2, "radius_key", nvram_pf_safe_get(prefix, "radius_key"));
		nvram_pf_set(prefix2, "radius_port", nvram_pf_safe_get(prefix, "radius_port"));
		nvram_pf_set(prefix2, "closed", nvram_pf_safe_get(prefix, "closed"));
		nvram_pf_set(prefix2, "mfp", nvram_pf_safe_get(prefix, "mfp"));
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		nvram_pf_set(prefix2, "11ax", nvram_pf_safe_get(prefix, "11ax"));
#endif
	}
}
#endif

#ifdef RTCONFIG_WPS_ENROLLEE
void start_wsc_enrollee(void)
{
	nvram_set("wps_enrollee", "1");
	doSystem("wpa_cli -i %s wps_pbc", get_staifname(0));//only 2.4G?
}

void stop_wsc_enrollee(void)
{
	int i;
	char word[256], *next, ifnames[128];
	char fpath[64], sta[64];

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);
		strcpy(sta, get_staifname(i));
		doSystem("wpa_cli -i %s wps_cancel", sta);
		i++;
	}
}

char *getWscStatus_enrollee(int unit)
{
	char buf[512];
	FILE *fp;
	int len;
	char *pt1, *pt2;

	snprintf(buf, sizeof(buf), "wpa_cli -i %s status", get_staifname(unit));
	fp = popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, "wpa_state=");
			if (pt1) {
				pt2 = pt1 + strlen("wpa_state=");
				pt1 = strstr(pt2, "address=");
				if (pt1) {
					*pt1 = '\0';
					chomp(pt2);
				}
				return pt2;
			}
		}
	}

	return "";
}
#endif

int getting_wps_enroll_conf(int unit, char *ssid, char *psk)
{
	char buf[512];
	FILE *fp;
	int len;
	char *pt1, *pt2, *pt3 = NULL, *pt4 = NULL, *pt5, *pt6;

	if(unit < 0 || ssid == NULL || psk == NULL)
		return -1;

	ssid[0] = '\0';
	psk[0] = '\0';
	snprintf(buf, sizeof(buf), "/etc/Wireless/conf/wpa_supplicant-%s.conf", get_staifname(unit));
	fp = fopen(buf, "r");
	if (fp) {
		len = fread(buf, 1, sizeof(buf), fp);
		fclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			//SSID
			pt1 = strstr(buf, "ssid=\"");
			if(pt1){
				pt2 = pt1 + strlen("ssid=\"");
				pt3 = strstr(pt2, "\"\n");
				if(pt3){
					*pt3 = '\0';
					chomp(pt2);
					strlcpy(ssid, pt2, 33);
				}
			}else{
				pt1 = strstr(buf, "ssid=");
				if(pt1){
					pt2 = pt1 + strlen("ssid=");
					pt3 = strchr(pt2, '\n');
					if(pt3){
						*pt3 = '\0';
						chomp(pt2);
						wps_ascii_to_char_with_utf8(ssid, pt2, 33);
					}
				}
			}
			//PSK
			if(pt3)
				pt4 = strstr(pt3 + 1, "psk=\"");
			else if(pt1)
				pt4 = strstr(pt1 + 1, "psk=\"");
			if(pt4){	//WPA-PSK
				pt5 = pt4 + strlen("psk=\"");
				pt6 = strstr(pt5, "\"");
				if(pt6){
					*pt6 = '\0';
					chomp(pt5);
					strlcpy(psk, pt5, 65);
				}
			}else{
				if(pt3)
					pt4 = strstr(pt3 + 1, "psk=");
				else if(pt1)
					pt4 = strstr(pt1 + 1, "psk=");
				if(pt4){
					char tmp[65] = {0};
					memcpy(tmp, pt4 + strlen("psk="), sizeof(tmp));
					chomp(tmp);
					strlcpy(psk, tmp, 65);
				}
			}
			return 0;
		}
	}
	return -1;
}

int getting_wps_result(int unit, char *bssid, char *ssid, char *key_mgmt)
{
	char buf[512];
	FILE *fp;
	int len;
	char *pt1, *pt2 = NULL, *pt3 = NULL;
	char ctrl_sk[32];

	get_wpa_ctrl_sk(unit, ctrl_sk, sizeof(ctrl_sk));
	snprintf(buf, sizeof(buf), "wpa_cli -p %s -i%s status", ctrl_sk, get_staifname(unit));
	fp = PS_popen(buf, "r");
	if(fp){
		len = fread(buf, 1, sizeof(buf), fp);
		PS_pclose(fp);
		if(len > 1){
			buf[len-1] = '\0';

			// ap_mac
			pt1 = strstr(buf, "bssid=");
			if(pt1){
				pt2 = pt1 + strlen("bssid=");
				pt3 = strstr(pt2, "freq=");
				if(pt3 && bssid){
					*pt3 = '\0';
					chomp(pt2);
					strlcpy(bssid, pt2, 18);
				}
			}

			// ssid
			if(pt3)
				pt2 = pt3 + 1;
			else if(pt1)
				pt2 = pt1 + 1;
			if(pt2)
				pt1 = strstr(pt2, "ssid=");
			if(pt1){
				pt2 = pt1 + strlen("ssid=");
				pt3 = strstr(pt2, "id=");
				if (pt3 && ssid) {
					*pt3 = '\0';
					chomp(pt2);
					strlcpy(ssid, pt2, 33);
				}
			}

			// auth_mode and crypto
			if(pt3)
				pt2 = pt3 + 1;
			else if(pt1)
				pt2 = pt1 + 1;
			if(pt2)
				pt1 = strstr(pt2, "key_mgmt=");
			if(pt1){
				pt2 = pt1 + strlen("key_mgmt=");
				pt3 = strstr(pt2, "wpa_state=");
				if(pt3 && key_mgmt){
					*pt3 = '\0';
					chomp(pt2);
					strlcpy(key_mgmt, pt2, 16);
				}
			}
			return 0;
		}
	}
	return -1;
}

char *getWscStatus(char *ifname, char *buf, int buflen)
{
	char cmd[512];
	FILE *fp;
	int len;
	char *pt1,*pt2;

	snprintf(cmd, sizeof(cmd), "hostapd_cli -i%s wps_get_status", ifname);
	fp = PS_popen(buf, "r");
	if (fp) {
		memset(buf, 0, buflen);
		len = fread(buf, 1, buflen, fp);
		PS_pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, "Last WPS result: ");
			if (pt1) {
				pt2 = pt1 + strlen("Last WPS result: ");
				pt1 = strstr(pt2, "Peer Address: ");
				if (pt1) {
					*pt1 = '\0';
					chomp(pt2);
				}
				return pt2;
			}
		}
	}

	return "";	/* FIXME */
}

int Pty_procedure_check(int unit, int wlif_count)
{
	return 0;
}
#ifdef RTCONFIG_AMAS
extern int g_upgrade;
int is_default()
{
	if(g_reboot || g_upgrade || IS_ATE_FACTORY_MODE() || nvram_get_int("obd_Setting") == 1 || nvram_get_int("x_Setting") == 1)
		return 0;
	else
		return nvram_get_int("obdeth_Setting");
}

int no_need_obd(void)
{
	if(g_reboot || g_upgrade || IS_ATE_FACTORY_MODE() || (nvram_get("sw_mode") && nvram_get_int("sw_mode") != SW_MODE_ROUTER) || 
    nvram_get_int("obd_Setting") == 1 || nvram_get_int("x_Setting") == 1 && nvram_get_int("obdeth_Setting") == 1 ) ||
	!nvram_get_int("wave_ready"))
		return -1;
	else
		return pids("obd");
}

int no_need_obdeth(void)
{
	if(g_reboot || g_upgrade || IS_ATE_FACTORY_MODE() || (nvram_get("sw_mode") && nvram_get_int("sw_mode") != SW_MODE_ROUTER) || 
    nvram_get_int("obd_Setting") == 1 || nvram_get_int("x_Setting") == 1 || nvram_get_int("obdeth_Setting") == 1 ) ||
	!nvram_get_int("wave_ready"))
		return -1;
	else
		return pids("obd_eth");
}

int amas_wait_wifi_ready(void)
{
	int result = 0;

	while(1)
	{
		result = nvram_get_int("wave_ready");
		if (result)
			break;
		sleep(5);
	}
	return result;
}
#endif

int __need_to_start_wps_band(char *prefix)
{
	char *p;

	if (!prefix || *prefix == '\0')
		return 0;

	p = nvram_pf_safe_get(prefix, "auth_mode_x");
	if ((!strcmp(p, "open") && !nvram_pf_match(prefix, "wep_x", "0"))
		|| !strcmp(p, "shared") || !strcmp(p, "psk") || !strcmp(p, "wpa")
		|| !strcmp(p, "wpa2") || !strcmp(p, "wpawpa2")
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		|| !strcmp(p, "wpa2wpa3") || !strcmp(p, "wpa3") || !strcmp(p, "sae")
#endif
		|| !strcmp(p, "radius") || nvram_pf_match(prefix, "radio", "0")
		|| nvram_pf_match(prefix, "closed", "1")
		|| !((sw_mode() == SW_MODE_ROUTER) || (sw_mode() == SW_MODE_AP)))
		return 0;

	return 1;
}

int need_to_start_wps_band(int wps_band)
{
	int ret = 0;
	char prefix[] = "wlXXXXXXXXXX_";

	switch (wps_band) {
#if defined(RTCONFIG_HAS_5G_2)
	case WL_5G_2_BAND:
#endif
#if defined(RTCONFIG_WIFI6E)
	case WL_6G_BAND:
#endif
	case WL_2G_BAND:
	case WL_5G_BAND:
		snprintf(prefix, sizeof(prefix), "wl%d_", wps_band);
		ret = __need_to_start_wps_band(prefix);
		break;
	default:
		break;
	}

	return ret;
}

void post_addif_bridge(int iftype)
{
	;
}

void pre_delif_bridge(int iftype)
{
	;
}

void post_delif_bridge(int iftype)
{
	;
}

#if defined(LANTIQ_BSD)
void duplicate_wl_ifaces(void)
{
	int unit = 1, unitmax = num_of_wl_if();
	int dwb_band = nvram_get_int("dwb_band"), dwb_mode = nvram_get_int("dwb_mode");
	char prefix[] = "wlXXXXXXX_", prefix2[] = "wlXXXXXXX_";
	snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	for(unit = 1; unit < unitmax; unit++){
		if((dwb_mode == 1 || dwb_mode == 3) && dwb_band > 0 && unit == dwb_band){
			dbg("Don't apply wl0 to wl%d in smart_connect function, if enabled DWB mode.\n", unit);
		}else{
			snprintf(prefix2, sizeof(prefix2), "wl%d_", unit);
			nvram_pf_set(prefix2, "ssid",  nvram_pf_safe_get(prefix, "ssid"));
			nvram_pf_set(prefix2, "auth_mode_x", nvram_pf_safe_get(prefix, "auth_mode_x"));
			nvram_pf_set(prefix2, "wep_x", nvram_pf_safe_get(prefix, "wep_x"));
			nvram_pf_set(prefix2, "key", nvram_pf_safe_get(prefix, "key"));
			nvram_pf_set(prefix2, "key1", nvram_pf_safe_get(prefix, "key1"));
			nvram_pf_set(prefix2, "key2", nvram_pf_safe_get(prefix, "key2"));
			nvram_pf_set(prefix2, "key3", nvram_pf_safe_get(prefix, "key3"));
			nvram_pf_set(prefix2, "key4", nvram_pf_safe_get(prefix, "key4"));
			nvram_pf_set(prefix2, "phrase_x", nvram_pf_safe_get(prefix, "phrase_x"));
			nvram_pf_set(prefix2, "crypto", nvram_pf_safe_get(prefix, "crypto"));
			nvram_pf_set(prefix2, "wpa_psk", nvram_pf_safe_get(prefix, "wpa_psk"));
			nvram_pf_set(prefix2, "radius_ipaddr", nvram_pf_safe_get(prefix, "radius_ipaddr"));
			nvram_pf_set(prefix2, "radius_key", nvram_pf_safe_get(prefix, "radius_key"));
			nvram_pf_set(prefix2, "radius_port", nvram_pf_safe_get(prefix, "radius_port"));
			nvram_pf_set(prefix2, "closed", nvram_pf_safe_get(prefix, "closed"));
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
			nvram_pf_set(prefix2, "11ax", nvram_pf_safe_get(prefix, "11ax"));
#endif
			nvram_pf_set(prefix2, "mfp", nvram_pf_safe_get(prefix, "mfp"));
		}
	}
#if defined(RTCONFIG_SWRTMESH)
	duplicate_wl_sync_uci();
#endif
	nvram_commit();
}
#endif

void config_mssid_isolate(char *ifname, int vif)
{
	char path[64] = {0}, prefix[16] = {0};
	if(sw_mode() == SW_MODE_ROUTER && ifname){
#if LINUX_KERNEL_VERSION > KERNEL_VERSION(5,4,0)
		snprintf(path, sizeof(path), "/sys/class/net/%s/brport/%s", ifname, "isolated");
#else
		snprintf(path, sizeof(path), "/sys/class/net/%s/brport/%s", ifname, "isolate_mode");
#endif
		if(f_exists(path)){
			if(vif)
				snprintf(prefix, sizeof(prefix), "%s_", wif_to_vif(ifname));
			else{
				if(absent_band(0) || strcmp(ifname, get_wififname(0))){
					if(absent_band(1) || strcmp(ifname, get_wififname(1))){
						dbg("%s: ifname [%s] vif [%d], unknown unit [%d]\n", __func__, ifname, 0, -1);
						return;
					}else
						snprintf(prefix, sizeof(prefix), "wl%d_", 1);
				}else
					snprintf(prefix, sizeof(prefix), "wl%d_", 0);
			}
			if(nvram_pf_get_int(prefix, "ap_isolate") || (vif && nvram_pf_match(prefix, "lanaccess", "off")))
				f_write_string(path, "1", 0, 0);
			else
				f_write_string(path, "0", 0, 0);
		}else{
			dbg("%s: %s doesn't exist!\n", __func__, path);
		}
	}
}

void wlcconnect_sig_handle(int sig)
{
	int wlc_band = nvram_get_int("wlc_band");
	if(sig == SIGCONT){
		if(!sta_status)
			sleep(5);
		else
			sleep(20);
	}else if(sig == SIGTSTP){
		pause();
	}else
		dbg("%s: received an invalid signal!\n", get_staifname(wlc_band));
}

int get_wisp_status(void)
{
	return getPapState(nvram_get_int("wlc_band")) == WLC_STATE_CONNECTED;
}

void pre_syspara(void)
{
	int unit;
	char prefix[16];
	for(unit = WL_2G_BAND; unit < MAX_NR_WL_IF; ++unit){
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		if(!nvram_pf_get(prefix, "itxbf") && nvram_pf_get(prefix, "implicitxbf"))
			nvram_pf_set(prefix, "itxbf", nvram_pf_get(prefix, "implicitxbf"));
	}
}

#if defined(RTCONFIG_LANWAN_LED) || defined(RTCONFIG_LAN4WAN_LED)
int LanWanLedCtrl(void)
{
	if(get_lanports_status() && nvram_match("AllLED", "1"))
#if defined(RTCONFIG_LAN4WAN_LED)
	{
		led_control(LED_LAN1, LED_ON);
		led_control(LED_LAN2, LED_ON);
		led_control(LED_LAN3, LED_ON);
		led_control(LED_LAN4, LED_ON);
	}
#else
		led_control(LED_LAN, LED_ON);
#endif
	else
#if defined(RTCONFIG_LAN4WAN_LED)
	{
		led_control(LED_LAN1, LED_OFF);
		led_control(LED_LAN2, LED_OFF);
		led_control(LED_LAN3, LED_OFF);
		led_control(LED_LAN4, LED_OFF);
	}
#else
		led_control(LED_LAN, LED_OFF);
#endif
	return 1;
}
#endif

int skip_ifconfig_up(const char *ifname)
{
	if(!strcmp(ifname, "wlan0"))
	{
		if(nvram_get_int("wl0_radio") == 0){
			_dprintf("[%s][%d] %s=0, skip IFUP %s\n", __func__, __LINE__, "wl0_radio", "wlan0");
			return -1;
		}
	}
	else if(!strcmp(ifname, "wlan0.0"))
	{
		if(nvram_get_int("wl0.1_bss_enabled")){
			_dprintf("[%s][%d] %s=0, skip IFUP %s\n", __func__, __LINE__, "wl0.1_bss_enabled", "wlan0.0");
			return -1;
		}
	}
	else if(!strcmp(ifname, "wlan0.1"))
	{
		if(nvram_get_int("wl0.2_bss_enabled")){
			_dprintf("[%s][%d] %s=0, skip IFUP %s\n", __func__, __LINE__, "wl0.2_bss_enabled", "wlan0.1");
			return -1;
		}
	}
	else if(!strcmp(ifname, "wlan0.2"))
	{
		if(nvram_get_int("wl0.3_bss_enabled")){
			_dprintf("[%s][%d] %s=0, skip IFUP %s\n", __func__, __LINE__, "wl0.3_bss_enabled", "wlan0.2");
			return -1;
		}
	}
	else if(!strcmp(ifname, "wlan2"))
	{
		if(nvram_get_int("wl1_radio")){
			_dprintf("[%s][%d] %s=0, skip IFUP %s\n", __func__, __LINE__, "wl1_radio", "wlan2");
			return -1;
		}
	}
	else if(!strcmp(ifname, "wlan2.0"))
	{
		if(nvram_get_int("wl1.1_bss_enabled")){
			_dprintf("[%s][%d] %s=0, skip IFUP %s\n", __func__, __LINE__, "wl1.1_bss_enabled", "wlan2.0");
			return -1;
		}
	}
	else if(!strcmp(ifname, "wlan2.1"))
	{
		if(nvram_get_int("wl1.2_bss_enabled")){
			_dprintf("[%s][%d] %s=0, skip IFUP %s\n", __func__, __LINE__, "wl1.2_bss_enabled", "wlan2.1");
			return -1;
		}
	}
	else if(!strcmp(ifname, "wlan2.2"))
	{
		if(nvram_get_int("wl1.3_bss_enabled")){
			_dprintf("[%s][%d] %s=0, skip IFUP %s\n", __func__, __LINE__, "wl1.3_bss_enabled", "wlan2.2");
			return -1;
		}
	}
	return 0;
}

void update_txburst_status(void)
{
	system("nvram set wl1_frameburst=\"`iwpriv wlan0 gTxopConfig|awk '{print $3}'`\"");
	if(nvram_get_int("wl1_frameburst"))
		nvram_set("wl1_frameburst", "on");
	else
		nvram_set("wl1_frameburst", "off");
}
