/*
 * Copyright 2021, ASUS
 * Copyright 2021-2022, SWRTdev
 * Copyright 2021-2022, paldier <paldier@hotmail.com>.
 * Copyright 2021-2022, lostlonger<lostlonger.g@gmail.com>.
 * All Rights Reserved.
 */

#include <rc.h>
#include <stdio.h>
#include <fcntl.h>
#include <qca.h>
#include <asm/byteorder.h>
#include <bcmnvram.h>
//#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <net/if_arp.h>
#include <shutils.h>
#if defined(RTCONFIG_MUSL_LIBC)
#include <signal.h>
#else
#include <sys/signal.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/mount.h>
#include <net/if.h>
#include <linux/mii.h>
//#include <linux/if.h>
#include <iwlib.h>
//#include <wps.h>
//#include <stapriv.h>
#include <shared.h>
#include "flash_mtd.h"
#include "ate.h"
#ifdef RTCONFIG_CFGSYNC
#include <json.h>
#include <cfg_slavelist.h>
#include <cfg_string.h>
#endif

#define MAX_FRW 64
#define MACSIZE 12

#define	DEFAULT_SSID_2G	"ASUS"
#define	DEFAULT_SSID_5G	"ASUS_5G"
#ifdef RTCONFIG_WIFI_SON
#define	QCA_MACCMD	"maccmd_sec"
#define	QCA_ADDMAC	"addmac_sec"
#else
#define	QCA_MACCMD	"maccmd"
#define	QCA_ADDMAC	"addmac"
#endif

#define APSCAN_WLIST	"/tmp/apscan_wlist"

int bg = 0;

#ifdef RTCONFIG_WIRELESSREPEATER
char *wlc_nvname(char *keyword);
#endif

/* For more information to nawds repeater caps definition,
 * reference to struct ieee80211_nawds_repeater of qca-wifi driver.
 */
#if defined(RTCONFIG_WIFI_QCA9990_QCA9990) || defined(RTCONFIG_WIFI_QCA9994_QCA9994) || defined(RTCONFIG_SOC_IPQ40XX)
/* 10.4 qca-wifi driver */
#define CAP_DS              0x01
#define CAP_TS              0x02	/* Not support in VHT80_80 or VHT160 */
#define CAP_4S              0x04	/* Not support in VHT80_80 or VHT160 */
#define CAP_HT20            0x0100
#define CAP_HT2040          0x0200
#define CAP_11ACVHT20       0x0400
#define CAP_11ACVHT40       0x0800
#define CAP_11ACVHT80       0x1000
#define CAP_11ACVHT80_80    0x2000
#define CAP_11ACVHT160      0x4000
#define CAP_TXBF            0x010000
#elif defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
/* 11.4/12.0 qca-wifi driver */
#define CAP_DS              0x01
#define CAP_TS              0x02
#define CAP_4S              0x04
#define CAP_5S              0x08
#define CAP_6S              0x10
#define CAP_7S              0x20
#define CAP_8S              0x40
#define CAP_NSS_RSVD        0x80
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

#define NAWDS_REPEATER_CAP_RSVD            0x80000000
#else
#error	Define nawds capability!
#endif

#if defined(RTCONFIG_WLCSCAN_RSSI)
#include <ap_priv.h>
#include <wlscan.h>
#endif

int g_wsc_configured = 0;
int g_isEnrollee[MAX_NR_WL_IF] = { 0, };

int getCountryRegion5G(const char *countryCode, int *warning);


static int bw40cap2g = 1;
static int bw40cap[28] = {0};//14 * 2
static int bw80cap[14] = {0};//7 * 2
static int bw160cap[6] = {0};//3 * 2
static int bw4320cap[4] = {0};
static int bw6480cap[3] = {0};
static int bw8640cap[2] = {0};

struct chattr_s {
	int channel;
	int isupper;
	char *ht_str;
	int chan_low;
	int chan_high;
	int chan_cen;
	int bw40cap;
	int bw80cap;
	int bw160cap;
	int pad1;
	int bw4320cap;
	int bw6480cap;
	int bw8640cap;
	int pad2;
};

#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
struct bandx_defval_s {
	char *handler;
	short qam;
	short intop;
	short shortgi;
	short stbc;
	short txbf;
	short mumimo;
	short sumimo;
	short he_mumimo;
	short he_sumimo;
	short he_dl_ofdma;
	short he_ul_ofdma;
	short he_ul_mimo;
	short ldpc;
	short unhide_ssid;
	short beacon;
	short protmode;
	short shpreamble;
	short pad;
	short frag;
	short dtim_period;
	short beacon_int;
	short pad2;
	short pad3;
	short pad4;
	short pad5;
	short pad6;
	short uapsd;
	short ap_bridge;
	short mcastenhance;
};

struct bandx_defval_s bandx_defval[] = {
	{ "/sbin/vap_evhandler", 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 3, 0, 0, 1, 1, 2347, 2346, 1, 100, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0 }
};

char *wldefval_tbl[] = { bandx_defval, bandx_defval, bandx_defval, bandx_defval };
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

int calculate_bw_of_each_5g_channel(int band)
{
	char chList[256], word[128];
	char *next;
	int i, j, k;
	int bw1 = 0, bw2 = 0;
	uint8_t chlist40[] = { 36, 0, 44, 0, 52, 0, 60, 0, 100, 0, 108, 0, 116, 0, 124, 0, 132, 0, 140, 0, 149, 0, 157, 0};
	uint8_t chlist80[] = { 36, 0, 52, 0, 100, 0, 116, 0, 132, 0, 149, 0};

	if(band > 2)
		return -1;

	if(get_channel_list_via_driver(band, chList, sizeof(chList)) > 0)
	{
		foreach_44(word, chList, next){
			i = safe_atoi(word);
			for(j = 0; j < sizeof(chlist40); j += 2){
				k = i - chlist40[j];
				if(k < 5)
					chlist40[j + 1] += 1;
			}
			for(j = 0; j < sizeof(chlist80); j += 2){
				k = i - chlist80[j];
				if(k < 13)
					chlist80[j + 1] += 1;
			}
			if((i - 36) < 29)
				bw1++;
			if((i - 100) < 29)
				bw2++;
		}
		for(i = 0; i < 12; i++)
			bw40cap[14 * (band - 1) + i] = chlist40[2 * i + 1] == 2;
		for(i = 0; i < 6; i++)
			bw80cap[7 * (band - 1) + i] = chlist80[2 * i + 1] == 4;
		bw160cap[(band - 1)] = bw1 == 8;
		bw160cap[(band - 1) + 1] = bw2 == 8;
		return 0;
	}
	dbg("%s: can't get channel list of band %d\n", __func__, band);
	return -2;
}

char *get_wscd_pidfile(void)
{
	static char tmpstr[32] = "/var/run/wscd.pid.";
	char wif[8];

	__get_wlifname(nvram_get_int("wps_band_x"), 0, wif);
	snprintf(tmpstr, sizeof(tmpstr), "/var/run/wscd.pid.%s", wif);
	return tmpstr;
}

char *get_wscd_pidfile_band(int wps_band)
{
	static char tmpstr[32] = "/var/run/wscd.pid.";
	char wif[8];

	__get_wlifname(wps_band, 0, wif);
	snprintf(tmpstr, sizeof(tmpstr), "/var/run/wscd.pid.%s", wif);
	return tmpstr;
}

int get_wifname_num(char *name)
{
	if (strcmp(WIF_5G, name) == 0)
		return 1;
	else if (strcmp(WIF_5G2, name) == 0)
		return 2;
	else if (strcmp(WIF_60G, name) == 0)
		return 3;
	else if (strcmp(WIF_2G, name) == 0)
		return 0;
	else
		return -1;
}

const char *get_wifname(int band)
{
	return get_wififname(band);
}

const char *get_wpsifname(void)
{
	return get_wififname(nvram_get_int("wps_band_x"));
}

#if 0
char *get_non_wpsifname()
{
	int wps_band = nvram_get_int("wps_band_x");

	if (wps_band)
		return WIF_2G;
	else
		return WIF_5G;
}
#endif

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

//End of new ATE Command
//Ren.B
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

//Ren.E

//Ren.B
void gen_macmode(int mac_filter[], int band, char *prefix)
{
	char temp[128];

	snprintf(temp, sizeof(temp), "%smacmode", prefix);
	mac_filter[0] = check_macmode(nvram_get(temp));
	_dprintf("mac_filter[0] = %d\n", mac_filter[0]);
}

//Ren.E

static inline void __choose_mrate(char *prefix, int *mcast_phy, int *mcast_mcs, int *rate)
{
	int phy = 3, mcs = 7;	/* HTMIX 65/150Mbps */
	*rate = 65000;		/* 11N NSS1 MCS7 */
	char tmp[128];

#ifdef RTCONFIG_IPV6
	switch (get_ipv6_service()) {
	default:
		if (!nvram_get_int(ipv6_nvname("ipv6_radvd")))
			break;
		/* fall through */
#ifdef RTCONFIG_6RELAYD
	case IPV6_PASSTHROUGH:
#endif
		if (!strncmp(prefix, "wl0", 3)) {
			phy = 2;
			mcs = 2;	/* 2G: OFDM 12Mbps */
			*rate=12000;
		} else {
			phy = 3;
			mcs = 1;	/* 5G: HTMIX 13/30Mbps */
			*rate = 13000;
		}
		/* fall through */
	case IPV6_DISABLED:
		break;
	}
#endif

	if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "2") ||	/* legacy mode */
	    strstr(nvram_safe_get(strcat_r(prefix, "crypto", tmp)), "tkip")) {	/* tkip */
		/* In such case, choose OFDM instead of HTMIX */
		phy = 2;
		mcs = 4;	/* OFDM 24Mbps */
		*rate=24000;
	}

	*mcast_phy = phy;
	*mcast_mcs = mcs;
}

int bw40_channel_check(int band,char *ext)
{
	int ch, ret = 1;
	char prefix[sizeof("wlX_XXXXXX")];

	if (band < 0 || band >= MAX_NR_WL_IF) {
		dbg("%s: unknown wl%d band!\n", __func__, band);
		return 0;
	}
#if !defined(RTCONFIG_HAS_5G_2)
	if (band == 2) {
		return 0;
	}
#endif

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
	case WL_5G_2_BAND:
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
	case WL_60G_BAND:
		/* nothing to do */
		break;
	}

	return ret;
}

#if defined(RTCONFIG_VHT80_80)
/**
 * @return:
 * 	0:	only one or none 80M segment exist
 *  otherwise:	two or more 80M segments exist
 */
int validate_bw_80_80_support(int band)
{
	int n = 0;
	unsigned int ch;
	FILE *fp;
	char *p, var[256];
	char cmd[sizeof("wlanconfig ath1 list chanXXXXXXX")];

	if (band != WL_5G_BAND && band != WL_5G_2_BAND)
		return 0;

	snprintf(cmd, sizeof(cmd), "wlanconfig %s list chan", get_wififname(band));
	if ((fp = popen(cmd, "r")) == NULL)
		return 0;

	var[0] = '\0';
	while (fgets(var, sizeof(var), fp)) {
		p = strstr(var + 6, "Channel");
		if (p != NULL)
			*(p - 1) = '\0';

		if (strstr(var, "V80-") && sscanf(var, "Channel %u : %*[^\n]", &ch) == 1)
			n++;

		if (p && strstr(p, "V80-") && sscanf(p, "Channel %u : %*[^\n]", &ch) == 1)
			n++;
	}
	fclose(fp);

	return n / 4 >= 2;
}
#else
static inline int validate_bw_80_80_support(__attribute__((__unused__)) int band) { return 0; }
#endif

#if defined(RTCONFIG_VHT160)
/**
 * @return:
 * 	0:	none 160M segment exist
 *  otherwise:	one or more 160M segments exist
 */
int validate_bw_160_support(void)
{
	int n = 0;
	unsigned int ch;
	FILE *fp;
	char *p, var[256];
	char cmd[sizeof("wlanconfig ath1 list chanXXXXXXX")];

	snprintf(cmd, sizeof(cmd), "wlanconfig %s list chan", WIF_5G);
	if ((fp = popen(cmd, "r")) == NULL)
		return 0;

	var[0] = '\0';
	while (fgets(var, sizeof(var), fp)) {
		p = strstr(var + 6, "Channel");
		if (p != NULL)
			*(p - 1) = '\0';

		if (strstr(var, "V160-") && sscanf(var, "Channel %u : %*[^\n]", &ch) == 1)
			n++;

		if (p && strstr(p, "V160-") && sscanf(p, "Channel %u : %*[^\n]", &ch) == 1)
			n++;
	}
	fclose(fp);

	return n / 8 >= 1;
}
#else
static inline int validate_bw_160_support(void) { return 0; }
#endif

int get_bw_via_channel(int band, int channel)
{
	int wl_bw;
	char buf[32];

	int sw_mode = sw_mode();

	if (band < 0 || band >= MAX_NR_WL_IF)
		return 0;
	if (band == WL_60G_BAND) {	/* FIXME */
		dbg("%s: 802.11ad Wigig is not supported!\n", __func__);
		return  0;
	}

	if (sw_mode == SW_MODE_REPEATER){
		snprintf(buf, sizeof(buf), "wl%d.1_bw", band);
	}
	else{
		snprintf(buf, sizeof(buf), "wl%d_bw", band);
	}
	wl_bw = nvram_get_int(buf);
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
	snprintf(buf, sizeof(buf), "wl%d_country_code", band);
	if(nvram_match(buf, "TW")) {
		if(channel == 56)
			return 0;
		if(channel == 60 || channel == 64) {
			if(wl_bw == 0)
				return 0;
			return 2;		// 40 MHz
		}
	}
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

#ifdef RTCONFIG_WIRELESSREPEATER
int create_tmp_sta_part(int unit, char *sta, char *ssid_prefix)
{
	FILE *fp;
	int shortgi;
	char tmp[64], prefix[128];
	char pidfile[32];
	char *lan_ifname = nvram_safe_get("lan_ifname");
	get_wpa_supplicant_pidfile(sta, pidfile, 29);
	snprintf(tmp, sizeof(tmp), "/etc/Wireless/conf/wpa_supplicant-%s.conf", sta);
	if(!ssid_prefix)
	{
		if((fp = fopen(tmp, "w+")) == NULL)
		{
			_dprintf("%s: Can't open %s\n", __func__, tmp);
			return -1;
		}
		fprintf(fp, "ctrl_interface=/var/run/wpa_supplicant-%s\nupdate_config=1\nnetwork={\n}\n", sta);
		fclose(fp);
	}
	_dprintf("==>wps enrollee: create %s vap\n", sta);
	create_vap(sta, unit, "sta");
	sleep(1);
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	if(nvram_pf_get_int(prefix, "nmode_x") == 2)
		shortgi = 0;
	else
		shortgi = nvram_pf_get_int(prefix, "HT_GI") != 0;
	doSystem("cfg80211tool %s shortgi %d\n", sta, shortgi);
	doSystem("cfg80211tool %s mode %s", sta, "AUTO");
	doSystem("cfg80211tool %s extap 1", sta);
	if(!*lan_ifname)
		lan_ifname = "br0";
	eval("/usr/bin/wpa_supplicant", "-B", "-P", pidfile, "-D", get_wsup_drvname(unit), "-i", sta, "-b", lan_ifname, "-c", tmp);
	if(ssid_prefix)
	{
		conv_iwconfig_essid(nvram_pf_safe_get(ssid_prefix, "ssid"), prefix);
		doSystem("iwconfig %s essid -- %s", sta, prefix);
	}
	ifconfig(sta, IFUP, NULL, NULL);
	return 0;
}

int copy_rpt_params_to_wl(char *wl_prefix, char *wlc_prefix)
{
	int i;
	char tmp[128];
	char *str, *auth_mode;
	str = nvram_get(strcat_r(wlc_prefix, "ssid", tmp));
	if(str && *str)
		nvram_set("wl_ssid", str);
	nvram_pf_set(wl_prefix, "ssid", nvram_safe_get(strcat_r(wlc_prefix, "ssid", tmp)));
	nvram_set("wl_ssid", nvram_pf_safe_get(wlc_prefix, "ssid"));
	if(wlc_prefix && !strncmp(wlc_prefix, "wlc", 3))
	{
		nvram_pf_set(wl_prefix, "auth_mode_x", nvram_pf_safe_get(wlc_prefix, "auth_mode"));
		auth_mode = "wep";
	}
	else
	{
		nvram_pf_set(wl_prefix, "auth_mode_x", nvram_pf_safe_get(wlc_prefix, "auth_mode"));
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
	nvram_pf_set(wl_prefix, "crypto", nvram_safe_get(strcat_r(wlc_prefix, "crypto", tmp)));
	nvram_pf_set(wl_prefix, "wpa_psk", nvram_safe_get(strcat_r(wlc_prefix, "wpa_psk", tmp)));
	str = nvram_get(strcat_r(wlc_prefix, "nbw_cap", tmp));
	if(str && *str)
		nvram_pf_set(wl_prefix, "bw", str);
	nvram_pf_set(wl_prefix, "radius_ipaddr", nvram_safe_get(strcat_r(wlc_prefix, "radius_ipaddr", tmp)));
	nvram_pf_set(wl_prefix, "radius_key", nvram_safe_get(strcat_r(wlc_prefix, "radius_key", tmp)));
	nvram_pf_set(wl_prefix, "radius_port", nvram_safe_get(strcat_r(wlc_prefix, "radius_port", tmp)));
	return 0;
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
	char *str;
	int flag_wep = 0;
	int i;

	snprintf(tmp, sizeof(tmp), "/etc/Wireless/conf/wpa_supplicant-%s.conf", get_staifname(band));
	if (!(fp_wpa = fopen(tmp, "w+")))
		return;

	if ((str = nvram_get(strcat_r(prefix_wlc, "ssid", tmp))) && strcmp(str, ""))
		nvram_set("wl_ssid", str);
	nvram_pf_set(prefix_mssid, "ssid", nvram_safe_get(strcat_r(prefix_wlc, "ssid", tmp)));
	nvram_pf_set(prefix_mssid, "auth_mode_x", nvram_safe_get(strcat_r(prefix_wlc, "auth_mode", tmp)));

	nvram_pf_set(prefix_mssid, "wep_x", nvram_safe_get(strcat_r(prefix_wlc, "wep", tmp)));
	nvram_pf_set(prefix_mssid, "key", nvram_safe_get(strcat_r(prefix_wlc, "key", tmp)));
	for(i = 1; i <= 4; i++) {
		char prekey[16];
		if(nvram_get_int(strcat_r(prefix_wlc, "key", tmp))!=i)
			continue;

		snprintf(prekey, sizeof(prekey), "key%d", i);
		nvram_pf_set(prefix_mssid, prekey, nvram_safe_get(strcat_r(prefix_wlc, "wep_key", tmp)));
	}

	nvram_pf_set(prefix_mssid, "crypto", nvram_safe_get(strcat_r(prefix_wlc, "crypto", tmp)));
	nvram_pf_set(prefix_mssid, "wpa_psk", nvram_safe_get(strcat_r(prefix_wlc, "wpa_psk", tmp)));
	nvram_pf_set(prefix_mssid, "bw", nvram_safe_get(strcat_r(prefix_wlc, "nbw_cap", tmp)));


	fprintf(fp_wpa,
		"ctrl_interface=/var/run/wpa_supplicant-%s\n"
		"%s\n"
		"network={\n"
		"ssid=\"%s\"\n",
		get_staifname(band),
		(addition?:""),
		((str = nvram_get(strcat_r(prefix_wlc, "ssid", tmp))) && strcmp(str, "")) ?
			str : "8f3610e3c9feabed953a6");

	fprintf(fp_wpa,"\tscan_ssid=1\n");

	str = nvram_safe_get(strcat_r(prefix_wlc, "auth_mode", tmp));
	if (str && *str != '\0') {
		if (!strcmp(str, "open") && nvram_match(strcat_r(prefix_wlc, "wep", tmp), "0")) {
			fprintf(fp_wpa,"\tkey_mgmt=NONE\n");		//open/none
		}
		else if (!strcmp(str, "open")) {
			flag_wep = 1;
			fprintf(fp_wpa,"\tkey_mgmt=NONE\n");		//open
			fprintf(fp_wpa,"\tauth_alg=OPEN\n");
		}
		else if (!strcmp(str, "shared")) {
			flag_wep = 1;
			fprintf(fp_wpa,"\tkey_mgmt=NONE\n");		//shared
			fprintf(fp_wpa,"\tauth_alg=SHARED\n");
		}
		else if (!strcmp(str, "psk") || !strcmp(str, "psk2")) {
			fprintf(fp_wpa,"\tkey_mgmt=WPA-PSK\n");
			if (!strcmp(str, "psk"))
				fprintf(fp_wpa,"\tproto=WPA\n");	//wpapsk
			else
				fprintf(fp_wpa,"\tproto=RSN\n");	//wpa2psk

			//EncrypType
			if (nvram_match(strcat_r(prefix_wlc, "crypto", tmp), "tkip")) {
				fprintf(fp_wpa, "\tpairwise=TKIP\n");
				fprintf(fp_wpa, "\tgroup=TKIP\n");
			}
			else if (nvram_match(strcat_r(prefix_wlc, "crypto", tmp), "aes")) {
				char *aes_type = "CCMP TKIP";
				if(band == WL_60G_BAND)
					aes_type = "GCMP";
				fprintf(fp_wpa, "\tpairwise=%s\n", aes_type);
				fprintf(fp_wpa, "\tgroup=%s\n", aes_type);
			}

			//key
			fprintf(fp_wpa, "\tpsk=\"%s\"\n",nvram_safe_get(strcat_r(prefix_wlc, "wpa_psk", tmp)));
		}
		else
			fprintf(fp_wpa,"\tkey_mgmt=NONE\n");		//open/none
	}
	else
		fprintf(fp_wpa,"\tkey_mgmt=NONE\n");			//open/none

	//EncrypType
	if (flag_wep)
	{
		int len;
		for(i = 1 ; i <= 4; i++)
		{
			if(nvram_get_int(strcat_r(prefix_wlc, "key", tmp))!=i)
				continue;

			str = nvram_safe_get(strcat_r(prefix_wlc, "wep_key", tmp));
			len = strlen(str);
			if(len == 5 || len == 13) {
				fprintf(fp_wpa, "\twep_tx_keyidx=%d\n", i - 1);
				fprintf(fp_wpa, "\twep_key%d=\"%s\"\n", i - 1, str);
			}
			else if(len == 10 || len == 26)	{
				fprintf(fp_wpa, "\twep_tx_keyidx=%d\n", i - 1);
				fprintf(fp_wpa, "\twep_key%d=%s\n", i - 1, str);
			}
			else {
				fprintf(fp_wpa, "\twep_tx_keyidx=%d\n", i - 1);
				fprintf(fp_wpa, "\twep_key%d=0\n", i - 1);
			}
		}
	}
	fprintf(fp_wpa, "}\n");
	fclose(fp_wpa);
}
#endif	/* RTCONFIG_WIRELESSREPEATER */

int calculate_bw_of_each_channel(int band)
{
	int result;
	switch(band)
	{
		case WL_2G_BAND:
#ifdef RTCONFIG_WIFI6E
		case WL_6G_BAND:
#else
		case WL_60G_BAND:
#endif
			result = 0;
			break;
		case WL_5G_BAND:
			result = calculate_bw_of_each_5g_channel(WL_5G_BAND);
			break;
		case WL_5G_2_BAND:
			result = calculate_bw_of_each_5g_channel(WL_5G_2_BAND);
			break;
		default:
			dbg("%s: Unknown band %d\n", __func__, band);
			result = -1;
			break;
	}
	return result;
}

void gen_hostapd_wps_config(FILE *fp_h, int band, int subnet, char *br_if)
{
	char *p;
	char prefix[8];
	snprintf(prefix, sizeof(prefix), "wl%d_", band);

	fprintf(fp_h, "# Wi-Fi Protected Setup (WPS)\n");

	if(!subnet && nvram_pf_match(prefix, "auth_mode_x", "sae") && nvram_pf_match(prefix, "auth_mode_x", "wpa3")){
		char *uuid = nvram_safe_get("uuid");
		if(nvram_pf_match(prefix, "mode_x", "1")){
			fprintf(fp_h, "wps_state=0\n");
		}else{
			if (nvram_match("w_Setting", "0")) {
				fprintf(fp_h, "wps_state=1\n");
			} else {
				fprintf(fp_h, "wps_state=2\n");
				fprintf(fp_h, "ap_setup_locked=1\n");
			}
		}
		if (uuid && strlen(uuid) == 36)
			fprintf(fp_h, "uuid=%s\n", uuid);
	} else {
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

	//fprintf(fp_h, "ieee80211w=0\n");
}

void qca_wif_up(const char* wif)
{
	if( nvram_get("sw_mode") && nvram_get_int("sw_mode") == 2)
	{
		if(!strncmp(wif, "sta", 3))
			ifconfig(wif, IFUP, NULL, NULL);
	}
}

void
gen_qca_wifi_cfgs(void)
{
	pid_t pid;
	char *argv[]={"/sbin/delay_exec","1","/tmp/postwifi.sh",NULL};
	char path2[50];
	char wif[256], *next, lan_ifnames[512];
	FILE *fp,*fp2;
	char tmp[100], prefix[sizeof("wlXXXXXXX_")], main_prefix[sizeof("wlXXXXX_")];
	int led_onoff[4] = { LED_ON, LED_ON, LED_ON, LED_ON }, unit = -1, sunit = 0, max_sunit;
	int sidx = 0;
	/* Array index: 0 -> 2G, 1 -> 5G, 2 -> 5G2, 3 -> 802.11ad Wigig
	 * bit: MBSSID index
	 */
	unsigned int m, wl_mask[4];
	char conf_path[sizeof("/etc/Wireless/conf/hostapd_athXXX.confYYYYYY")];
	char pid_path[sizeof("/var/run/hostapd_athXXX.pidYYYYYY")];
	char entropy_path[sizeof("/var/run/entropy_athXXX.binYYYYYY")];
	char path[sizeof("/sys/class/net/ath001XXXXXX")];
	char path1[sizeof(NAWDS_SH_FMT) + 6];
	int i;
#ifdef RTCONFIG_WIRELESSREPEATER
	int wlc_band = nvram_get_int("wlc_band");
#endif

	if (!(fp = fopen("/tmp/prewifi.sh", "w+")))
		return;
	if (!(fp2 = fopen("/tmp/postwifi.sh", "w+")))
		return;

	fprintf(fp, "#!/bin/sh -p\n");
	fprintf(fp2, "#!/bin/sh -p\n");
	deinit_all_vaps(0);
	memset(wl_mask, 0, sizeof(wl_mask));

#ifdef RTCONFIG_CAPTIVE_PORTAL
#define CP_IFINDEX 2
	int start_idx;
	char lanifnames[16]={0};
	for(start_idx=0; start_idx<=CP_IFINDEX; start_idx++){
		if(start_idx == 0){
			snprintf(lanifnames, sizeof(lanifnames), "lan_ifnames");
		}else{
			if (!nvram_match("cp_enable", "1") && !nvram_match("chilli_enable", "1"))
				break;
			snprintf(lanifnames, sizeof(lanifnames), "lan%d_ifnames", start_idx);
		}
		strlcpy(lan_ifnames, nvram_safe_get(lanifnames), sizeof(lan_ifnames));
#else
	strlcpy(lan_ifnames, nvram_safe_get("lan_ifnames"), sizeof(lan_ifnames));
#endif
	foreach (wif, lan_ifnames, next) {
		SKIP_ABSENT_FAKE_IFACE(wif);

		if (!guest_wlif(wif) && (unit = get_wifi_unit(wif)) >= 0 && unit < ARRAY_SIZE(led_onoff)) {
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			if (nvram_match(strcat_r(prefix, "radio", tmp), "0"))
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
				gen_ath_config(0, 0);
			} else {
				/* 2.4G guest */
				sunit = get_wlsubnet(0, wif);
				if (sunit > 0) {
					sidx = atoi(wif + strlen(WIF_2G));
					wl_mask[0] |= 1 << sunit;
					gen_ath_config(0, sidx);
				}
			}
		} else if (!strncmp(wif, WIF_5G, strlen(WIF_5G))) {
			if (!guest_wlif(wif)) {
				/* 5G */
				wl_mask[1] |= 1;
				gen_ath_config(1, 0);
			} else {
				/* 5G guest */
				sunit = get_wlsubnet(1, wif);
				if (sunit > 0) {
					sidx = atoi(wif + strlen(WIF_5G));
					wl_mask[1] |= 1 << sunit;
					gen_ath_config(1, sidx);
				}
			}
		} else if (!strncmp(wif, WIF_5G2, strlen(WIF_5G2))) {
			if (!guest_wlif(wif)) {
				/* 5G2 */
				wl_mask[2] |= 1;
				gen_ath_config(2, 0);
			} else {
				/* 5G2 guest */
				sunit = get_wlsubnet(2, wif);
				if (sunit > 0) {
					sidx = atoi(wif + strlen(WIF_5G2));
					wl_mask[2] |= 1 << sunit;
					gen_ath_config(2, sidx);
				}
			}
#if defined(RTCONFIG_WIGIG)
		} else if (!strncmp(wif, WIF_60G, strlen(WIF_60G))) {
			if (!guest_wlif(wif)) {
				/* 802.11ad Wigig */
				wl_mask[3] |= 1;
				gen_nl80211_config(3, 1, 0);
			} else {
				/* 802.11ad Wigig guest, driver doesn't support. */
				sunit = get_wlsubnet(2, wif);
				if (sunit > 0) {
					sidx = atoi(wif + strlen(WIF_60G));
					wl_mask[3] |= 1 << sunit;
					gen_nl80211_config(3, 1, sidx);
				}
			}
#endif
		} else

			continue;

#ifdef RTCONFIG_WIRELESSREPEATER
		if (strcmp(wif, STA_2G) && strcmp(wif, STA_5G) && strcmp(wif, STA_5G2))
#endif
		{
#if defined(RTCONFIG_CONCURRENTREPEATER)
			if ((!strcmp(wif, WIF_2G) && nvram_get_int("wlc_express") == 1) ||
					(!strcmp(wif, WIF_5G) && nvram_get_int("wlc_express") == 2))
			{
#if defined(RPAC51) /* 5G chain 2x2 nss 1 */
				if (!strcmp(wif, WIF_5G) && nvram_get_int("wlc_express") == 2) {
					doSystem("iwpriv sta1 nss 1");
				}
					doSystem("iwpriv ath1 nss 1");
#endif
				continue;
			}
			else
#endif
			{
				fprintf(fp, "/etc/Wireless/sh/prewifi_%s.sh %s\n",wif,bg?"&":"");
				fprintf(fp2, "/etc/Wireless/sh/postwifi_%s.sh %s\n",wif,bg?"&":"");
#if defined(RTCONFIG_QCA) && defined(RTCONFIG_SOC_IPQ40XX)
				if (!strcmp(wif, WIF_5G)) {	// 5G
#if defined(RTAC82U) || defined(RTAC95U)
					fprintf(fp2, "echo d > /sys/class/net/ath1/queues/rx-0/rps_cpus\n");
#endif
					fprintf(fp2, "ethtool -K eth0 gro off\n");
					fprintf(fp2, "ethtool -K eth0 tso off\n");
					fprintf(fp2, "ethtool -K eth1 gro off\n");
					fprintf(fp2, "ethtool -K eth1 tso off\n");
#if defined(RTAC82U) || defined(RTAC95U)
					fprintf(fp2, "ethtool -K ath0 gro off\n");
					fprintf(fp2, "ethtool -K ath0 tso off\n");
					fprintf(fp2, "ethtool -K ath1 gro off\n");
					fprintf(fp2, "ethtool -K ath1 tso off\n");
#endif
				}
#endif

			}
		}
	}
#ifdef RTCONFIG_CAPTIVE_PORTAL
	}
#endif
	if ((wl_mask[0]&0xfe) || (wl_mask[1]&0xfe) || (wl_mask[2]&0xfe))
			fprintf(fp2, "sleep 4\n");

	for (i = 0; i < MAX_NR_WL_IF; ++i) {
		SKIP_ABSENT_BAND(i);
		if(sw_mode() == SW_MODE_REPEATER){
			snprintf(main_prefix, sizeof(main_prefix), "wl%d.1_", i);
		}else{
			snprintf(main_prefix, sizeof(main_prefix), "wl%d_", i);
		}
		if (i != WL_60G_BAND && nvram_pf_match(main_prefix, "channel", "0"))
			fprintf(fp2, "iwconfig %s channel auto\n", __get_wlifname(i, 0, wif));
	}

#if defined(RTCONFIG_SOC_IPQ8064)
	fprintf(fp2, "echo 90 > /proc/sys/vm/pagecache_ratio\n");
	fprintf(fp2, "echo -1 > /proc/net/skb_recycler/max_skbs\n");
#endif

#if !defined(RTCONFIG_CONCURRENTREPEATER)
#if defined(RTCONFIG_WPS_ALLLED_BTN)
	if (nvram_match("AllLED", "1")) {
#endif
#if defined(PLN12)
		fprintf(fp2, "[ -e /sys/class/net/%s ] && led_ctrl %d %d\n", WIF_2G, LED_2G_RED, led_onoff[0]);
#elif defined(PLAC56)
		fprintf(fp2, "[ -e /sys/class/net/%s ] && led_ctrl %d %d\n", WIF_2G, LED_2G_GREEN, led_onoff[0]);
#elif defined(MAPAC1750)
#else
		fprintf(fp2, "[ -e /sys/class/net/%s ] && led_ctrl %d %d\n", WIF_2G, LED_2G, led_onoff[0]);
#endif
#if defined(RTCONFIG_HAS_5G)
#if defined(PLAC56)
		fprintf(fp2, "[ -e /sys/class/net/%s ] && led_ctrl %d %d\n", WIF_5G, LED_5G_GREEN, led_onoff[1]);
#elif defined(MAPAC1750)
#else
		fprintf(fp2, "[ -e /sys/class/net/%s ] && led_ctrl %d %d\n", WIF_5G, LED_5G, led_onoff[1]);
#endif
#if defined(RTCONFIG_HAS_5G_2)
		fprintf(fp2, "[ -e /sys/class/net/%s ] && led_ctrl %d %d\n", WIF_5G2, LED_5G2, led_onoff[2]);
#endif
#endif
#if defined(RTCONFIG_WIGIG)
		fprintf(fp2, "[ -e /sys/class/net/%s ] && led_ctrl %d %d\n", WIF_60G, LED_60G, led_onoff[3]);
#endif
#if defined(RTCONFIG_WPS_ALLLED_BTN)
	}
#endif
	fprintf(fp2, "if [ -e /sys/class/net/%s ] ", WIF_2G);
#if defined(RTCONFIG_HAS_5G)
	fprintf(fp2, "&& [ -e /sys/class/net/%s ] ", WIF_5G);
#endif
#if defined(RTCONFIG_HAS_5G_2)
	fprintf(fp2, "&& [ -e /sys/class/net/%s ] ", WIF_5G2);
#endif
#if defined(RTCONFIG_WIGIG)
	fprintf(fp2, "&& [ -e /sys/class/net/%s ] ", WIF_60G);
#endif
	fprintf(fp2, "; then\n");
	fprintf(fp2, "    nvram set wlready=1\n");
	fprintf(fp2, "else\n");
	fprintf(fp2, "    logger Wireless not ready!\n");
	fprintf(fp2, "fi\n");
#else
#if defined(RPAC51)
	fprintf(fp2, "iwpriv wifi1 dl_loglevel 5\n"); // disable log from target firmware
#endif
 	fprintf(fp2, "nvram set wlready=1\n");
#endif	/* !defined(RTCONFIG_CONCURRENTREPEATER) */
#if defined(RTCONFIG_NOTIFICATION_CENTER) || defined(RTCONFIG_CFGSYNC)
	fprintf(fp2, "weventd &\n");
#endif

#if defined(RTAC58U) || defined(RT4GAC53U) /* for RAM 128MB */
	if (get_meminfo_item("MemTotal") <= 131072) {
		fprintf(fp2, "iwpriv wifi1 fc_buf_max 4096\n");
		fprintf(fp2, "iwpriv wifi1 fc_q_max 512\n");
		fprintf(fp2, "iwpriv wifi1 fc_q_min 32\n");
		fprintf(fp2, "iwpriv wifi0 fc_buf_max 4096\n");
		fprintf(fp2, "iwpriv wifi0 fc_q_max 512\n");
		fprintf(fp2, "iwpriv wifi0 fc_q_min 32\n");
	}
#endif

	fclose(fp);
	fclose(fp2);
	chmod("/tmp/prewifi.sh",0777);
	chmod("/tmp/postwifi.sh",0777);

	for (unit = 0; unit < ARRAY_SIZE(wl_mask); ++unit) {
		max_sunit = num_of_mssid_support(unit);
		for (sunit = 0; sunit <= max_sunit; ++sunit) {
			__get_wlifname(unit, sunit, wif);
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
#ifdef RTCONFIG_AMAS
	if(sw_mode() == SW_MODE_REPEATER || (sw_mode() == SW_MODE_AP && nvram_match("re_mode", "1")))
#else
	if(sw_mode() == SW_MODE_REPEATER)
#endif
	{
		for (i = 0; i < MAX_NR_WL_IF; ++i) {
			int nmode, shortgi;
			char prefix[16];
			char lan_if[16];
			char *sta = get_staifname(i);
			char pid_file[sizeof("/var/run/wifi-staX.pidXXXXXX")];
			char conf[sizeof("/etc/Wireless/conf/wpa_supplicant-STAX.confXXXXXX")];
#if defined(RTCONFIG_CONCURRENTREPEATER)
			char wlc_prefix[16];
			char wpa_sup[sizeof("/var/run/wpa_supplicant-staXXXXXXX")];
#endif

			SKIP_ABSENT_BAND(i);
#if !defined(RTCONFIG_CONCURRENTREPEATER)
#ifdef RTCONFIG_AMAS
			if(sw_mode() != SW_MODE_AP)
#endif
			if (i != wlc_band)
				continue;
#endif

			snprintf(lan_if, sizeof(lan_if), "%s", nvram_get("lan_ifname")? nvram_get("lan_ifname") : "br0");

			snprintf(prefix, sizeof(prefix), "wl%d_", i);
			nmode = nvram_pf_get_int(prefix, "nmode_x");
			shortgi = (nmode != 2)? !!nvram_pf_get_int(prefix, "HT_GI") : 0;
			doSystem("iwpriv %s shortgi %d\n", sta, shortgi);
			doSystem("iwpriv %s mode AUTO", sta);
			doSystem("iwpriv %s extap 1", sta);

			get_wpa_supplicant_pidfile(sta, pid_file, sizeof(pid_file));
			if(kill_pidfile(pid_file) == 0)
				cprintf("## WARNING: %s is alive ##\n", pid_file);
			sprintf(conf, "/etc/Wireless/conf/wpa_supplicant-%s.conf", sta);
			eval("/usr/bin/wpa_supplicant", "-B", "-P", pid_file, "-D", get_wsup_drvname(i), "-i", sta, "-b", lan_if, "-c", conf);
			{
				extern char * conv_iwconfig_essid(char *ssid_in, char *ssid_out);
				char prefix_wlc[16];
				if (sw_mode() == SW_MODE_REPEATER)
#if defined(RTCONFIG_CONCURRENTREPEATER)
					snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc%d_", i);
#else
					snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc_");
#endif
#if defined(RTCONFIG_AMAS)
				else if (nvram_match("re_mode", "1"))
					snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc%d_", i);
#endif
				if (conv_iwconfig_essid(nvram_pf_get(prefix_wlc, "ssid"), tmp) != NULL) {
					doSystem("iwconfig %s essid -- %s", sta, tmp);
				}
			}
			ifconfig(sta, IFUP, NULL, NULL);

#if defined(RTCONFIG_CONCURRENTREPEATER)
			sprintf(wlc_prefix, "wlc%d_", i);
			if (nvram_pf_match(wlc_prefix, "ssid", "")) {
				sprintf(wpa_sup, "/var/run/wpa_supplicant-%s", sta);
				eval("wpa_cli", "-p", wpa_sup, "-i", sta, "disconnect");
			}
#endif
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
		for (unit = 0; unit < ARRAY_SIZE(wl_mask); ++unit) {
			snprintf(main_prefix, sizeof(main_prefix), "wl%d_", unit);
			m = wl_mask[unit];
			for (sunit = 0, sidx = 0; m > 0; ++sunit, ++sidx, m >>= 1) {
#if defined(RTCONFIG_CONCURRENTREPEATER)
			if (sw_mode() == SW_MODE_REPEATER) {
				sunit++;
			}
#endif
				snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, sunit);
#if defined(RTCONFIG_CONCURRENTREPEATER)
			if (sw_mode() == SW_MODE_REPEATER) {
					if (((nvram_get_int("wlc_express") - 1) == unit && sunit == 1) || sunit > 1)
						continue;
			}
			else if (sw_mode() == SW_MODE_AP) {
				/* only take care primary 2G/5G, not guest */
				if (sunit > 0)
				continue;
			}
			else
#endif
				if (sidx > 0 && !nvram_match(wl_nvname("bss_enabled", unit, sunit), "1"))
				{
					sidx--;
					continue;
				}

				__get_wlifname(unit, sidx, wif);
				tweak_wifi_ps(wif);

#ifdef RTCONFIG_WIRELESSREPEATER
				if(sw_mode() == SW_MODE_REPEATER) {
					doSystem("iwpriv %s athnewind 1", wif);
#if defined(RTCONFIG_CONCURRENTREPEATER)
					/* workaround for default to avoid 2G can't connect */
					if (!strcmp(wif, WIF_2G) && nvram_match("x_Setting", "0"))
						doSystem("iwpriv %s extap 1", wif);
#endif	/* RTCONFIG_CONCURRENTREPEATER */
				}
#endif

				sprintf(path2, "/etc/Wireless/sh/postwifi_%s.sh", wif);

				if (nvram_match("skip_gen_ath_config", "1") && f_exists(path2))
					continue;

				if (!f_exists(path2))
					continue;
				if (!(fp = fopen(path2, "a")))
					continue;

				/* hostapd of QCA Wi-Fi 10.2 and 10.4 driver is not required if
				 * 1. Open system and WPS is disabled.
				 *    a. primary 2G/5G and WPS is disabled
				 *    b. guest 2G/5G
				 * 2. WEP
				 * But 802.11ad Wigig driver, nl80211 based, always need it due
				 * to some features are implemented by hostapd.
				 */

					if (unit == WL_60G_BAND) {
						/* Do nothing, just skip continuous statment below for
						 * 802.11ad Wigig driver.  Maybe we need to do same
						 * thing for newer nl80211 based 2.4G/5G driver too.
						 */
					}
					else if (!strcmp(nvram_safe_get(wl_nvname("auth_mode_x", unit, sunit)), "shared")) {
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
				if (unit == WL_60G_BAND) {
					if (!nvram_pf_match(main_prefix, "radio", "0")) {
						fprintf(fp, "hostapd -d -B %s -P %s -e %s\n", conf_path, pid_path, entropy_path);
					}
				} else {
					fprintf(fp, "hostapd -d -B %s -P %s -e %s\n", conf_path, pid_path, entropy_path);
				}
#ifdef RTCONFIG_AMAS
				if(unit == 0) {
					/* TODO need to kill old one? */
					fprintf(fp, "delay_exec 5 hostapd_cli -i%s -a/usr/sbin/qca_hostapd_event_handler.sh -B &\n", wif);
				}
#endif	/* RTCONFIG_AMAS */

				/* Hostapd will up VAP interface automatically.
				 * So, down VAP interface if radio is not on and
				 * not to up VAP interface anymore.
				 */
				if (nvram_pf_match(main_prefix, "radio", "0")) {
					fprintf(fp, "ifconfig %s down\n", wif);
				}

				/* Connect to peer WDS AP after VAP up */
				sprintf(path1, NAWDS_SH_FMT, wif);
				if (!sunit && nvram_pf_invmatch(main_prefix, "radio", "0") && f_exists(path1)) {
					fprintf(fp, "%s\n", path1);
				}

				/* WPS-OOB not work if wps_ap_pin disabled. */
				if (!nvram_match("w_Setting", "0"))
					fprintf(fp, "hostapd_cli -i %s wps_ap_pin disable\n", wif);

#if defined(RTCONFIG_CONCURRENTREPEATER)
#if defined(RPAC51)
					/* workaround for default to avoid 2G can't connect */
					if (strcmp(wif, WIF_2G)==0)
					{
						fprintf(fp, "ifconfig %s down up\n", wif);
					}
#endif
#endif	/* RTCONFIG_CONCURRENTREPEATER */
				fclose(fp);
			}
		}

		argv[1]="4";
		_eval(argv, NULL, 0, &pid);

		//system("/tmp/postwifi.sh");
		//sleep(1);
	} /* !mediabridge_mode() */

#if defined(RTCONFIG_WIRELESSREPEATER)
	if (repeater_mode() || mediabridge_mode())
		update_wifi_led_state_in_wlcmode();
#endif
}


void set_wlpara_qca(const char* wif, int band)
{
/* TBD.fill some eraly init here
	char tmp[100], prefix[]="wlXXXXXXX_";

	snprintf(prefix, sizeof(prefix), "wl%d_", band);

	if (nvram_match(strcat_r(prefix, "radio", tmp), "0"))
		radio_ra(wif, band, 0);
	else
	{
		int txpower = atoi(nvram_safe_get(strcat_r(prefix, "TxPower", tmp)));
		if ((txpower >= 0) && (txpower <= 100))
			doSystem("iwpriv %s set TxPower=%d",wif, txpower);
	}
	eval("iwpriv", (char *)wif, "set", "IgmpAdd=01:00:5e:7f:ff:fa");
	eval("iwpriv", (char *)wif, "set", "IgmpAdd=01:00:5e:00:00:09");
	eval("iwpriv", (char *)wif, "set", "IgmpAdd=01:00:5e:00:00:fb");
*/
}

int wlconf_qca(const char* wif)
{
	int unit = 0;
	char word[256], *next;
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";
	char *p;

	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		SKIP_ABSENT_BAND_AND_INC_UNIT(unit);
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);

		if (!strcmp(word, wif))
		{
			p = get_hwaddr(wif);
			if (p)
			{
				nvram_set(strcat_r(prefix, "hwaddr", tmp), p);
				free(p);
			}
#if 0
			if (!strcmp(word, WIF_2G))
				set_wlpara_qca(wif, 0);
			else if (!strcmp(word, WIF_5G))
				set_wlpara_qca(wif, 1);
			else if (!strcmp(word, WIF_5G2))
				set_wlpara_qca(wif, 2);
			else if (!strcmp(word, WIF_60G))
				set_wlpara_qca(wif, 2);
#endif
		}

		unit++;
	}
	return 0;
}

int gen_ath_config(int band, int subnet)
{
#ifdef RTCONFIG_WIRELESSREPEATER
	int wlc_band = nvram_get_int("wlc_band");
#endif
	FILE *fp, *fp2, *fp3, *fp5;
	char *str = NULL;
	char *str2 = NULL;
	int i, j, bw = 0, puren = 0, only_20m = 0;
	int max_subnet;
	char list[2048];
	char wds_mac[4][30], buf[128];
	int wds_keyidx;
	char wds_key[50];
	int flag_8021x = 0;
	int warning = 0;
	char tmp[128], prefix[] = "wlXXXXXXX_" , main_prefix[] = "wlXXX_", athfix[]="athXXXX_",tmpfix[]="wlXXXXX_",rpfix[]="wlXXXXX_";
	char temp[128], prefix_mssid[] = "wlXXXXXXXXXX_mssid_";
	char tmpstr[128];
	char *nv, *nvp, *b, *p;
	int mcast_phy = 0, mcast_mcs = 0;
	int mac_filter[MAX_NO_MSSID];
	char t_mode[30],t_bw[10],t_ext[10],mode_cmd[100];
	int bg_prot, ban, wmm, legacy = 0;
	int sw_mode = sw_mode();
	int wpapsk;
	int val,rate,caps;
	char wif[10], vphy[10];
	char path1[50],path2[50],path3[50];
	char path5[sizeof(NAWDS_SH_FMT) + 6] = "";
	int rep_mode, nmode, shortgi, stbc;
	int isax;
#if defined(RTCONFIG_WIFI_QCA9990_QCA9990) || defined(RTCONFIG_WIFI_QCA9994_QCA9994) || defined(RTCONFIG_SOC_IPQ40XX) || defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	int fc_buf_min = 1000;
	int txbf, mumimo, ldpc = 3, tqam, tqam_intop;
	unsigned int maxsta = 511;
#else
	unsigned int maxsta = 127;
#endif
#if defined(RTCONFIG_CONCURRENTREPEATER)
	int wlc_express = nvram_get_int("wlc_express");
#endif
	char br_if[IFNAMSIZ];
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	FILE *fp4;
	char path4[50];
	int acs_dfs = nvram_get_int("acs_dfs");
	int memtotal = get_meminfo_item("MemTotal");
	int nss_en = nss_wifi_offloading();
	struct bandx_defval_s *wldefval = &wldefval_tbl[band];
#endif
	char iwphy[16];

	struct chattr_s chattr_2g[] = {
		{ 1, 1, "[HT40+]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 1, "[HT40+]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 3, 1, "[HT40+]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 1, "[HT40+]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 5, 1, "[HT40+]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 6, 1, "[HT40+]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 7, 1, "[HT40+]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 8, -1, "[HT40-]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 9, -1, "[HT40-]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 10, -1, "[HT40-]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 11, -1, "[HT40-]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 12, -1, "[HT40-]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 13, -1, "[HT40-]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ 14, -1, "[HT40-]", 0, 0, 0, bw40cap2g, 0, 0, 0, 0, 0, 0, 0 },
		{ -1, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	struct chattr_s chattr_5g[] = {
		{ 36, 1, "[HT40+]", 38, 42, 50, bw40cap[0], bw80cap[0], bw160cap[0], 0, 0, 0, 0, 0 },
		{ 40, -1, "[HT40-]", 38, 42, 50, bw40cap[0], bw80cap[0], bw160cap[0], 0, 0, 0, 0, 0 },
		{ 44, 1, "[HT40+]", 38, 42, 50, bw40cap[1], bw80cap[0], bw160cap[0], 0, 0, 0, 0, 0 },
		{ 48, -1, "[HT40-]", 46, 42, 50, bw40cap[1], bw80cap[0], bw160cap[0], 0, 0, 0, 0, 0 },
		{ 52, 1, "[HT40+]", 54, 58, 50, bw40cap[2], bw80cap[1], bw160cap[0], 0, 0, 0, 0, 0 },
		{ 56, -1, "[HT40-]", 54, 58, 50, bw40cap[2], bw80cap[1], bw160cap[0], 0, 0, 0, 0, 0 },
		{ 60, 1, "[HT40+]", 62, 58, 50, bw40cap[3], bw80cap[1], bw160cap[0], 0, 0, 0, 0, 0 },
		{ 64, -1, "[HT40-]", 62, 58, 50, bw40cap[3], bw80cap[1], bw160cap[0], 0, 0, 0, 0, 0 },
		{ 100, 1, "[HT40+]", 102, 106, 114, bw40cap[4], bw80cap[2], bw160cap[1], 0, 0, 0, 0, 0 },
		{ 104, -1, "[HT40-]", 102, 106, 114, bw40cap[4], bw80cap[2], bw160cap[1], 0, 0, 0, 0, 0 },
		{ 108, 1, "[HT40+]", 110, 106, 114, bw40cap[5], bw80cap[2], bw160cap[1], 0, 0, 0, 0, 0 },
		{ 112, -1, "[HT40-]", 110, 106, 114, bw40cap[5], bw80cap[2], bw160cap[1], 0, 0, 0, 0, 0 },
		{ 116, 1, "[HT40+]", 118, 106, 114, bw40cap[6], bw80cap[3], bw160cap[1], 0, 0, 0, 0, 0 },
		{ 120, -1, "[HT40-]", 118, 106, 114, bw40cap[6], bw80cap[3], bw160cap[1], 0, 0, 0, 0, 0 },
		{ 124, 1, "[HT40+]", 126, 106, 114, bw40cap[7], bw80cap[3], bw160cap[1], 0, 0, 0, 0, 0 },
		{ 128, -1, "[HT40-]", 126, 106, 114, bw40cap[7], bw80cap[3], bw160cap[1], 0, 0, 0, 0, 0 },
		{ 132, 1, "[HT40+]", 134, 138, 0, bw40cap[8], bw80cap[4], 0, 0, 0, 0, 0, 0 },
		{ 136, -1, "[HT40-]", 134, 138, 0, bw40cap[8], bw80cap[4], 0, 0, 0, 0, 0, 0 },
		{ 140, 1, "[HT40+]", 142, 138, 0, bw40cap[9], bw80cap[4], 0, 0, 0, 0, 0, 0 },
		{ 144, -1, "[HT40-]", 142, 138, 0, bw40cap[9], bw80cap[4], 0, 0, 0, 0, 0, 0 },
		{ 149, 1, "[HT40+]", 151, 155, 163, bw40cap[10], bw80cap[5], bw160cap[2], 0, 0, 0, 0, 0 },
		{ 153, -1, "[HT40-]", 151, 155, 163, bw40cap[10], bw80cap[5], bw160cap[2], 0, 0, 0, 0, 0 },
		{ 157, 1, "[HT40+]", 159, 155, 163, bw40cap[11], bw80cap[5], bw160cap[2], 0, 0, 0, 0, 0 },
		{ 161, -1, "[HT40-]", 159, 155, 163, bw40cap[11], bw80cap[5], bw160cap[2], 0, 0, 0, 0, 0 },
		{ 165, 1, "[HT40+]", 167, 171, 163, bw40cap[12], bw80cap[6], bw160cap[2], 0, 0, 0, 0, 0 },
		{ 169, -1, "[HT40-]", 167, 171, 163, bw40cap[12], bw80cap[6], bw160cap[2], 0, 0, 0, 0, 0 },
		{ 173, 1, "[HT40+]", 175, 171, 163, bw40cap[13], bw80cap[6], bw160cap[2], 0, 0, 0, 0, 0 },
		{ 177, -1, "[HT40-]", 175, 171, 163, bw40cap[13], bw80cap[6], bw160cap[2], 0, 0, 0, 0, 0 },
		{ -1, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	struct chattr_s chattr_5g2[] = {
		{ 36, 1, "[HT40+]", 38, 42, 50, bw40cap[14], bw80cap[7], bw160cap[3], 0, 0, 0, 0, 0 },
		{ 40, -1, "[HT40-]", 38, 42, 50, bw40cap[14], bw80cap[7], bw160cap[3], 0, 0, 0, 0, 0 },
		{ 44, 1, "[HT40+]", 38, 42, 50, bw40cap[15], bw80cap[7], bw160cap[3], 0, 0, 0, 0, 0 },
		{ 48, -1, "[HT40-]", 46, 42, 50, bw40cap[15], bw80cap[7], bw160cap[3], 0, 0, 0, 0, 0 },
		{ 52, 1, "[HT40+]", 54, 58, 50, bw40cap[16], bw80cap[8], bw160cap[3], 0, 0, 0, 0, 0 },
		{ 56, -1, "[HT40-]", 54, 58, 50, bw40cap[16], bw80cap[8], bw160cap[3], 0, 0, 0, 0, 0 },
		{ 60, 1, "[HT40+]", 62, 58, 50, bw40cap[17], bw80cap[8], bw160cap[3], 0, 0, 0, 0, 0 },
		{ 64, -1, "[HT40-]", 62, 58, 50, bw40cap[17], bw80cap[8], bw160cap[3], 0, 0, 0, 0, 0 },
		{ 100, 1, "[HT40+]", 102, 106, 114, bw40cap[18], bw80cap[9], bw160cap[4], 0, 0, 0, 0, 0 },
		{ 104, -1, "[HT40-]", 102, 106, 114, bw40cap[18], bw80cap[9], bw160cap[4], 0, 0, 0, 0, 0 },
		{ 108, 1, "[HT40+]", 110, 106, 114, bw40cap[19], bw80cap[9], bw160cap[4], 0, 0, 0, 0, 0 },
		{ 112, -1, "[HT40-]", 110, 106, 114, bw40cap[19], bw80cap[9], bw160cap[4], 0, 0, 0, 0, 0 },
		{ 116, 1, "[HT40+]", 118, 106, 114, bw40cap[20], bw80cap[10], bw160cap[4], 0, 0, 0, 0, 0 },
		{ 120, -1, "[HT40-]", 118, 106, 114, bw40cap[20], bw80cap[10], bw160cap[4], 0, 0, 0, 0, 0 },
		{ 124, 1, "[HT40+]", 126, 106, 114, bw40cap[21], bw80cap[10], bw160cap[4], 0, 0, 0, 0, 0 },
		{ 128, -1, "[HT40-]", 126, 106, 114, bw40cap[21], bw80cap[10], bw160cap[4], 0, 0, 0, 0, 0 },
		{ 132, 1, "[HT40+]", 134, 138, 0, bw40cap[22], bw80cap[11], 0, 0, 0, 0, 0, 0 },
		{ 136, -1, "[HT40-]", 134, 138, 0, bw40cap[22], bw80cap[11], 0, 0, 0, 0, 0, 0 },
		{ 140, 1, "[HT40+]", 142, 138, 0, bw40cap[23], bw80cap[11], 0, 0, 0, 0, 0, 0 },
		{ 144, -1, "[HT40-]", 142, 138, 0, bw40cap[23], bw80cap[11], 0, 0, 0, 0, 0, 0 },
		{ 149, 1, "[HT40+]", 151, 155, 163, bw40cap[24], bw80cap[12], bw160cap[5], 0, 0, 0, 0, 0 },
		{ 153, -1, "[HT40-]", 151, 155, 163, bw40cap[24], bw80cap[12], bw160cap[5], 0, 0, 0, 0, 0 },
		{ 157, 1, "[HT40+]", 159, 155, 163, bw40cap[25], bw80cap[12], bw160cap[5], 0, 0, 0, 0, 0 },
		{ 161, -1, "[HT40-]", 159, 155, 163, bw40cap[25], bw80cap[12], bw160cap[5], 0, 0, 0, 0, 0 },
		{ 165, 1, "[HT40+]", 167, 171, 163, bw40cap[26], bw80cap[13], bw160cap[5], 0, 0, 0, 0, 0 },
		{ 169, -1, "[HT40-]", 167, 171, 163, bw40cap[26], bw80cap[13], bw160cap[5], 0, 0, 0, 0, 0 },
		{ 173, 1, "[HT40+]", 175, 171, 163, bw40cap[27], bw80cap[13], bw160cap[5], 0, 0, 0, 0, 0 },
		{ 177, -1, "[HT40-]", 175, 171, 163, bw40cap[27], bw80cap[13], bw160cap[5], 0, 0, 0, 0, 0 },
		{ -1, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	struct chattr_s chattr_60g[] = {
		{ 9, 0, NULL, 0, 0, 0, 0, 0, 0, 6, bw4320cap[0], 0, 0, 0 },
		{ 10, 0, NULL, 0, 0, 0, 0, 0, 0, 12, bw4320cap[1], 0, 0, 0 },
		{ 11, 0, NULL, 0, 0, 0, 0, 0, 0, 24, bw4320cap[2], 0, 0, 0 },
		{ 12, 0, NULL, 0, 0, 0, 0, 0, 0, 48, bw4320cap[3], 0, 0, 0 },
		{ 13, 0, NULL, 0, 0, 0, 0, 0, 0, 96, bw4320cap[4], 0, 0, 0 },
		{ 17, 0, NULL, 0, 0, 0, 0, 0, 0, 14, 0, bw6480cap[0], 0, 0 },
		{ 18, 0, NULL, 0, 0, 0, 0, 0, 0, 28, 0, bw6480cap[1], 0, 0 },
		{ 19, 0, NULL, 0, 0, 0, 0, 0, 0, 56, 0, bw6480cap[2], 0, 0 },
		{ 20, 0, NULL, 0, 0, 0, 0, 0, 0, 112, 0, bw6480cap[3], 0, 0 },
		{ 25, 0, NULL, 0, 0, 0, 0, 0, 0, 30, 0, 0, bw8640cap[0], 0 },
		{ 26, 0, NULL, 0, 0, 0, 0, 0, 0, 60, 0, 0, bw8640cap[1], 0 },
		{ 27, 0, NULL, 0, 0, 0, 0, 0, 0, 120, 0, 0, bw8640cap[2], 0 },
		{ -1, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	};

	struct chattr_s *chattr_tbl[] = { chattr_2g, chattr_5g, chattr_5g2, chattr_60g };

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
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	if(!wldefval)
	{
		dbg("%s: default value of band %d is not defined!\n", __func__, band);
		return -1;
	}
#endif
	__get_wlifname(band, subnet, wif);
	strcpy(vphy, get_vphyifname(band));

	snprintf(path1, sizeof(path1), "/etc/Wireless/conf/hostapd_%s.conf", wif);
	snprintf(path2, sizeof(path2), "/etc/Wireless/sh/postwifi_%s.sh", wif);
	snprintf(path3, sizeof(path3), "/etc/Wireless/sh/prewifi_%s.sh", wif);
	system("mkdir -p /etc/Wireless/conf");
	system("mkdir -p /etc/Wireless/sh");

	if (nvram_match("skip_gen_ath_config", "1") && f_exists(path2) && f_exists(path3)) {
		_dprintf("%s: reuse %s and %s\n", __func__, path2, path3);
		return 0;
	}

	_dprintf("gen qca config\n");
	if (!(fp = fopen(path1, "w+")))
		return 0;
	if (!(fp2 = fopen(path2, "w+")))
		return 0;
	if (!(fp3 = fopen(path3, "w+")))
		return 0;

#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	if(subnet==0){
		snprintf(path4, sizeof(path4), "/etc/Wireless/sh/prewifi_%s.sh", vphy);
		if (!(fp4 = fopen(path4, "w+")))
			return 0;
	}
	fprintf(fp2, "#!/bin/sh -p\n");
	fprintf(fp3, "#!/bin/sh -p\n");
	if(mediabridge_mode()){
		fclose(fp2);
		if (!(fp2 = fopen("/dev/null", "w+")))
			return 0;
	}
#endif

#ifdef RTCONFIG_WIRELESSREPEATER
#if defined(RTCONFIG_CONCURRENTREPEATER)
	if (sw_mode == SW_MODE_REPEATER //&& (wlc_express == 0 || (wlc_express - 1) == band) 
//			&& nvram_invmatch(strcat_r(prefix_wlc, "ssid", tmp), "")&& subnet==0)
			&& subnet==0)
#else
	if (sw_mode == SW_MODE_REPEATER && wlc_band == band && nvram_invmatch("wlc_ssid", "")&& subnet==0)
#endif
	{  
	   	rep_mode=1;
	}	
#endif
#if defined(RTCONFIG_CFG80211)
	get_iwphy_name(band, iwphy, sizeof(iwphy));
#endif
	fprintf(fp, "#The word of \"Default\" must not be removed\n");
	fprintf(fp, "#Default\n");

	snprintf(main_prefix, sizeof(main_prefix), "wl%d_", band);
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	if(nvram_match("x_Setting", "0") && band == 1)
		channel = 36;
	else{
		j = nvram_pf_get_int(main_prefix, "channel");
		if(j > 0){
			struct chattr_s *pchattr = (struct chattr_s *)&chattr_tbl[band];
			while(pchattr->channel > 0){
				if(pchattr->channel == j){
					channel = pchattr->channel;
					break;
				}else
					channel = 0;
			}
		}else{
			channel = 0;
		}
	}
#endif
	if(subnet==0)
		snprintf(prefix, sizeof(prefix), "wl%d_", band);
	else
	{
	   	j=0;
		for (i = 1;i <= max_subnet; i++) {
#if defined(RTCONFIG_AMAS)
			if(sw_mode == SW_MODE_AP && nvram_match("re_mode", "1") && i == 1){
				i = 2;
			}
#endif
			snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d.%d_", band, i);
			if (nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
			{   
			   	j++;
				if(j==subnet)
					snprintf(prefix, sizeof(prefix), "wl%d.%d_", band,i);
			}	

		}   
	}

#ifdef RTCONFIG_WIRELESSREPEATER
	if(rep_mode)
		snprintf(prefix, sizeof(prefix), "wl%d.1_", band);
#if defined(RTCONFIG_CONCURRENTREPEATER)
	if (sw_mode == SW_MODE_REPEATER && (wlc_express == 0 || (wlc_express - 1) == band) 
//			&& nvram_invmatch(strcat_r(prefix_wlc, "ssid", tmp), "")&& subnet==0)
			&& subnet==0)
	{
		char prefix_wlc[32];
		snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc%d_", band);
	    	snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d_", band);
		nvram_set("ure_disable", "0");
		// convert wlc%d_xxx to wlX_
		write_rpt_wpa_supplicant_conf(band, prefix_mssid, prefix_wlc, "update_config=1\n");
	}

#else	/* RTCONFIG_CONCURRENTREPEATER */
#ifdef RTCONFIG_AMAS
	if((nvram_match("x_Setting", "0") && !rep_mode) || (!rep_mode && __aimesh_re_node(sw_mode)))
		acs_dfs = 0;
	if (sw_mode == SW_MODE_AP && subnet == 0 && nvram_match("re_mode", "1"))
	{
		char prefix_wlc[32];
		snprintf(prefix_wlc, sizeof(prefix_wlc), "wl%d.1_", band);
		snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d_", band);
		nvram_pf_set(prefix_mssid, "closed", nvram_safe_get(strcat_r(prefix_wlc, "closed", tmp))));
		nvram_set("ure_disable", "0");
		nvram_pf_set(prefix_wlc, "nbw_cap", nvram_safe_get(strcat_r(prefix_mssid, "bw", tmp)));	//preset "nbw_cap" before restore
		copy_rpt_params_to_wl(prefix_mssid, prefix_wlc);
		if(strstr(nvram_safe_get("sta_ifnames"), get_staifname(band))){
			if(!strstr(nvram_safe_get("skip_ifnames"), get_staifname(band))){
				snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc%d_", band);
				if(en_multi_profile() && band == 1){
					snprintf(prefix_wlc, sizeof(prefix_wlc), "wsbh_");
					snprintf(prefix_mssid, sizeof(prefix_mssid), "wsfh_");
				}else{
					snprintf(prefix_wlc, sizeof(prefix_wlc), "");
					snprintf(prefix_mssid, sizeof(prefix_mssid), "");
				}
				// convert wlc%d_xxx to wlX_ 
				write_rpt_wpa_supplicant_conf(band, prefix_mssid, prefix_wlc, "INIT_DISABLE");
				rep_mode=1;	/* set here to avoid changing prefix */
			}
		}
	}
	else
#endif	/* RTCONFIG_AMAS */
	if (sw_mode == SW_MODE_REPEATER && wlc_band == band && nvram_invmatch("wlc_ssid", "") && subnet ==0)
	{
		char prefix_wlc[32];
		snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc_");
		snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d_", band);
		nvram_set("ure_disable", "0");
		// convert wlc_xxx to wlX_ according to wlc_band == band
		write_rpt_wpa_supplicant_conf(band, prefix_mssid, prefix_wlc, NULL);
	}
#endif	/* !RTCONFIG_CONCURRENTREPEATER */
#endif	/* RTCONFIG_WIRELESSREPEATER */

#ifdef RTCONFIG_AMAS
	if (!_aimesh_re_node(sw_mode) && !nvram_pf_get_int(main_prefix, "radio"))
		nvram_pf_set(main_prefix, "radio", "1");
#endif

	fprintf(fp, "interface=%s\n",wif);
	fprintf(fp, "ctrl_interface=/var/run/hostapd\n");
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	fprintf(fp, "send_probe_response=0\n");
	fprintf(fp, "driver=nl80211\n");
#else
	fprintf(fp, "dump_file=/tmp/hostapd.dump\n");
	fprintf(fp, "driver=atheros\n");
#endif
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	if(subnet && subnet <= wgn_guest_ifcount(band) && guest_wlif(wif) != 0)
		fprintf(fp, "bridge=%s\n", find_brX(wif));
	else
		fprintf(fp, "bridge=%s\n", get_vap_bridge());
#else
	strlcpy(br_if, nvram_get("lan_ifname")? : nvram_default_get("lan_ifname"), sizeof(br_if));
#if defined(RTCONFIG_TAGGED_BASED_VLAN) || defined(RTCONFIG_CAPTIVE_PORTAL)
	find_brifname_by_wlifname(wif,br_if,16);
#ifdef RTCONFIG_CAPTIVE_PORTAL
	FindBrifByWlif(wif, br_if, 16);
#endif
#endif
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
	fprintf(fp, "macaddr_acl=0\n");
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
#else
	fprintf(fp, "ignore_broadcast_ssid=0\n");
#endif
	fprintf(fp, "eapol_key_index_workaround=0\n");
	flag_8021x=0;

	str = nvram_pf_get(prefix, "auth_mode_x");
	if (str && (!strcmp(str, "radius") || !strcmp(str, "wpa") 
	    || !strcmp(str, "wpa2") || !strcmp(str, "wpawpa2")
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	    || !strcmp(str, "wpa3") || !strcmp(str, "wpa2wpa3") || !strcmp(str, "wpawpa2wpa3")
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
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
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
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
			if(nvram_pf_get_int(main_prefix, "11ax") == 1)
				strlcpy(t_mode, max_5g_ax_mode, sizeof(t_mode));
			else
#endif
			strlcpy(t_mode, max_5g_ac_mode, sizeof(t_mode));

			puren = 1;
		}
		isax = strstr(t_mode, "11AHE") != NULL;
	}
	else //2.4G
	{
		if (val == 8)
		{
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
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
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
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
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
			if(nvram_pf_get_int(main_prefix, "11ax") == 1)
				strlcpy(t_mode, max_2g_ax_mode, sizeof(t_mode));
			else
#endif
			strlcpy(t_mode, max_2g_n_mode, sizeof(t_mode));
			bg_prot = 1;
		}
		isax = strstr(t_mode, "11GHE") != NULL;
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
#if defined(RTCONFIG_WIFI_QCA9990_QCA9990) || defined(RTCONFIG_WIFI_QCA9994_QCA9994) || defined(RTCONFIG_SOC_IPQ40XX)
	// 2.4GHz 256 QAM
	if (!band) {
		/* 256-QAM can't be enabled, if HT mode is not enabled. */
		tqam = (nmode != 2)? !!nvram_pf_get_int(tmpfix, "turbo_qam") : 0;
		fprintf(fp2, "iwpriv %s vht_11ng %d\n", wif, tqam);
		tqam_intop = tqam? !!nvram_pf_get_int(tmpfix, "turbo_qam_brcm_intop") : 0;
		fprintf(fp2, "iwpriv %s 11ngvhtintop %d\n", wif, tqam_intop);
	}
#endif

	// Short-GI
	shortgi = (nmode != 2)? !!nvram_get_int(strcat_r(tmpfix, "HT_GI", tmp)) : 0;
	fprintf(fp2, "iwpriv %s shortgi %d\n", wif, shortgi);

	// STBC
	stbc = (nmode != 2)? !!nvram_get_int(strcat_r(tmpfix, "HT_STBC", tmp)) : 0;
	fprintf(fp2, "iwpriv %s tx_stbc %d\n", wif, stbc);
	fprintf(fp2, "iwpriv %s rx_stbc %d\n", wif, stbc);

#if defined(RTCONFIG_WIFI_QCA9990_QCA9990) || defined(RTCONFIG_WIFI_QCA9994_QCA9994) || defined(RTCONFIG_SOC_IPQ40XX) || defined(RPAC51)
	// TX BeamForming, must be set before association with the station.
	txbf = (nmode != 2)? !!nvram_get_int(strcat_r(tmpfix, "txbf", tmp)) : 0;
	mumimo = (nmode != 2)? !!(nvram_get_int(strcat_r(tmpfix, "mumimo", tmp))) : 0;
	if (!repeater_mode() && !mediabridge_mode()) {
		fprintf(fp2, "iwpriv %s vhtmubfer %d\n", wif, mumimo);	/* Multiple-user beam former, AP only */
	} else {
		fprintf(fp2, "iwpriv %s vhtmubfee %d\n", wif, mumimo);	/* Multiple-user beam formee, STA only */
	}
	fprintf(fp2, "iwpriv %s vhtsubfer %d\n", wif, txbf);	/* Single-user beam former */
	fprintf(fp2, "iwpriv %s vhtsubfee %d\n", wif, txbf);	/* Single-user beam formee */

	if(mumimo)	
		fprintf(fp2, "wifitool %s beeliner_fw_test 2 0\n", wif);	/* Improve Multiple-user mimo performance */

	fprintf(fp2, "iwpriv %s implicitbf %d\n", wif,nvram_get_int(strcat_r(tmpfix, "implicitxbf", tmp)));

#ifdef RTCONFIG_OPTIMIZE_XBOX
	// LDPC
	if (nvram_match(strcat_r(prefix, "optimizexbox", tmp), "1"))
		ldpc = 0;
	fprintf(fp2, "iwpriv %s ldpc %d\n", wif, ldpc? 3 : 0);
#endif

#if defined(RPAC51)
	ldpc = 3; /* 0 disable 1 Rx 2 Tx 3 TxRx */
	fprintf(fp2, "iwpriv %s ldpc %d\n", wif, ldpc);
#endif
#endif

	/* Set maximum number of clients of a guest network. */
	if (subnet) {
		if (nvram_match(strcat_r(prefix, "atf", tmp), "1"))
			maxsta = 32;
		val = nvram_get_int(strcat_r(prefix, "guest_num", tmp));
		if (val > 0 && val <= maxsta)
			fprintf(fp2, "iwpriv %s maxsta %d\n", wif, val);
	}
	   
	//fprintf(fp2,"ifconfig %s up\n",wif);
	fprintf(fp2,"iwpriv %s hide_ssid %d\n",wif,nvram_get_int(strcat_r(prefix, "closed", tmp)));
	strlcpy(tmp, nvram_pf_safe_get(prefix, "ssid"), sizeof(tmp));
	//replace SSID each char to "\char"
	conv_iwconfig_essid(tmp, buf);
	fprintf(fp2, "iwconfig %s essid -- %s\n", wif, buf);
	
	if (!rep_mode)
	{   
		//BGProtection, all VAPs on same band share same setting.
		if (!subnet) {
			str = nvram_safe_get(strcat_r(prefix, "gmode_protection", tmp));
			if (!str || *str == '\0') {
				warning = 13;
				str = "0";
			}
			if (!strcmp(str, "auto") && (bg_prot)) //only for 2.4G
				fprintf(fp2, "iwpriv %s protmode 1\n", wif);
			else
				fprintf(fp2, "iwpriv %s protmode 0\n", wif);
		}

		//TxPreamble, all VAPs on same band share same setting.
		if (!subnet) {
			str = nvram_safe_get(strcat_r(prefix, "plcphdr", tmp));
			if (!str)
				str = "long";
			fprintf(fp2, "iwpriv %s shpreamble %d\n", wif, !strcmp(str, "short")? 1 : 0);
		}

		//RTSThreshold  Default=2347, each VAPs have it's own setting.
		str = nvram_pf_safe_get(main_prefix, "rts");
		if (!str || *str == '\0') {
			warning = 14;
			str = "2347";
		}
		fprintf(fp2, "iwconfig %s rts %d\n", wif, safe_atoi(str));

		//Fragmentation Threshold  Default=2346, each VAPs have it's own setting.
		if (legacy) {
			str = nvram_pf_safe_get(main_prefix, "frag");
			if (!str || *str == '\0') {
				warning = 15;
				str = "2346";
			}
			fprintf(fp2, "iwconfig %s frag %d\n", wif, safe_atoi(str));
		}

		//DTIM Period, each VAPs have it's own setting.
		str = nvram_pf_safe_get(main_prefix, "dtim");
		if (!str || *str == '\0') {
			warning = 11;
			str = "1";
		}
		fprintf(fp2, "iwpriv %s dtim_period %d\n", wif, safe_atoi(str));

		//BeaconPeriod, all VAPs on same band share same setting.
		if (!subnet) {
			const int min_bintval =
#if defined(RTCONFIG_WIFI_QCA9994_QCA9994)
				100;
#else
				40;
#endif
			str = nvram_safe_get(strcat_r(prefix, "bcn", tmp));
			if (!str || *str == '\0') {
				warning = 10;
				str = "100";
			}
			if (atoi(str) > 1000 || atoi(str) < min_bintval) {
				nvram_set(strcat_r(prefix, "bcn", tmp), "100");
				fprintf(fp2, "iwpriv %s bintval 100\n", wif);
			} else
				fprintf(fp2, "iwpriv %s bintval %d\n", wif, safe_atoi(str));
		}

		/* WME, iwpriv part, each VAPs have it's own setting. */
		fprintf(fp2, "iwpriv %s wmm %d\n", wif, wmm);

		/* WME No acknowledge, all VAPs on same band share same setting. */
		if (!subnet && wmm) {
			str = nvram_pf_get(main_prefix, "wme_no_ack");
			if (!str || *str == '\0')
				str = "off";
			val = !strcmp(str, "on")? 1 : 0;
			fprintf(fp2, "iwpriv %s setwmmparams 6 0 0 %d\n", wif, val);	/* ac = BE */
			fprintf(fp2, "iwpriv %s setwmmparams 6 1 0 %d\n", wif, val);	/* ac = BK */
			fprintf(fp2, "iwpriv %s setwmmparams 6 2 0 %d\n", wif, val);	/* ac = VI */
			fprintf(fp2, "iwpriv %s setwmmparams 6 3 0 %d\n", wif, val);	/* ac = VO */
		}

		//APSDCapable, each VAPs have it's own setting.
		if (wmm) {
			str = nvram_pf_safe_get(main_prefix, "wme_apsd");
			if (!str || *str == '\0') {
				warning = 18;
				str = "off";
			}
			fprintf(fp2, "iwpriv %s uapsd %d\n", wif, strcmp(str, "off")? 1 : 0);
		}

		//TxBurst, all VAPs on same band share same setting.
		if (!subnet) {
			str = nvram_safe_get(strcat_r(prefix, "frameburst", tmp));
			if (!str || *str == '\0') {
				warning = 16;
				str = "off";
			}
			fprintf(fp2, "iwpriv %s burst %d\n", vphy, strcmp(str, "off") ? 1 : 0);
#if defined(RT4GAC53U)
			if (band && !strcmp(str, "off"))
				fprintf(fp2, "iwpriv %s aggr_burst 0 0\n", vphy);
#endif
		}
	}

	if (subnet == 0) {
		int obss_rssi_th = nvram_get_int(strcat_r(prefix, "obss_rssi_th", tmp));
		if (obss_rssi_th == 0)
			obss_rssi_th = 35;	//as default value
		fprintf(fp2, "iwpriv %s obss_rssi_th %d\n", vphy, obss_rssi_th);	//check RSSI of each scan entry
	}

#if defined(MAPAC2200)
	if (band > 0 && subnet==0 && rep_mode==0 && (nvram_match(strcat_r(main_prefix, "country_code", temp), "GB")))
	{ //for new CE adaptivity
		fprintf(fp2, "iwpriv %s aggr_burst 0 0\n", vphy);
	}
#endif	/* MAPAC2200 */

	if(!band) //2.4G
		fprintf(fp2,"iwpriv %s ap_bridge %d\n",wif,nvram_get_int("wl0_ap_isolate")?0:1);
	else
		fprintf(fp2,"iwpriv %s ap_bridge %d\n",wif,nvram_get_int("wl1_ap_isolate")?0:1);

#ifdef AMAZON_WSS
	if (amazon_wss_ap_isolate_support(prefix))
	{
		/* ap isolate */
		fprintf(fp2, "iwpriv %s ap_bridge %d\n", wif, nvram_pf_get_int(prefix, "ap_isolate") ? 0 : 1);

		/* only QCA need to enable arping isolate when amazon_wss is enabled*/
		fprintf(fp2, "echo %d > /sys/devices/virtual/net/%s/brport/isolate_mode\n", nvram_pf_get_int(prefix, "ap_isolate"), wif);
	}
#endif

	//AuthMode
	memset(tmpstr, 0x0, sizeof(tmpstr));
	str = nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp));
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

	str = nvram_safe_get(strcat_r(prefix, "wpa_gtk_rekey", tmp));
	if (str && strlen(str)) 
		fprintf(fp, "wpa_group_rekey=%d\n",safe_atoi(str));

	snprintf(prefix_mssid, sizeof(prefix_mssid), "%s", prefix);
	if ((nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "open")
		&& nvram_match(strcat_r(prefix_mssid, "wep_x", temp),"0")))
			fprintf(fp, "#wpa_pairwise=\n");
	else if ((nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "open")
		&&
		 nvram_invmatch(strcat_r(prefix_mssid, "wep_x", temp),"0"))
		||
		nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "shared")
		||
		nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp),"radius"))
	   	{
		   	//wep
			int kn = 1;

			if (nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "shared"))
				fprintf(fp2, "iwpriv %s authmode 2\n", wif);
			else if (nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "radius"))
				fprintf(fp2, "iwpriv %s authmode 3\n", wif);
			else
				fprintf(fp2, "iwpriv %s authmode 1\n", wif);

			for (kn = 1; kn <= 4; kn++) {
				snprintf(tmpstr, sizeof(tmpstr), "%skey%d", prefix_mssid, kn);
				if (strlen(nvram_safe_get(tmpstr))==10 || strlen(nvram_safe_get(tmpstr))==26)
					fprintf(fp2,"iwconfig %s key [%d] %s\n", wif, kn, nvram_safe_get(tmpstr));
				else if (strlen(nvram_safe_get(tmpstr))==5 || strlen(nvram_safe_get(tmpstr))==13)
					fprintf(fp2,"iwconfig %s key [%d] \"s:%s\"\n", wif, kn, nvram_safe_get(tmpstr));
				else
					fprintf(fp2,"iwconfig %s key [%d] off\n", wif, kn);
			}

		   	str = nvram_safe_get(strcat_r(prefix_mssid, "key", temp));
			snprintf(tmpstr, sizeof(tmpstr), "%skey%s", prefix_mssid, str);
			if (strlen(nvram_safe_get(tmpstr)) > 0)
				fprintf(fp2,"iwconfig %s key [%s]\n",wif,str); //key index

			if (strlen(nvram_safe_get(tmpstr))==10 || strlen(nvram_safe_get(tmpstr))==26)
				fprintf(fp2,"iwconfig %s key %s\n",wif,nvram_safe_get(tmpstr));
			else if (strlen(nvram_safe_get(tmpstr))==5 || strlen(nvram_safe_get(tmpstr))==13)
		   	   	fprintf(fp2,"iwconfig %s key \"s:%s\"\n",wif,nvram_safe_get(tmpstr));
			else
				fprintf(fp, "#wpa_pairwise=\n");
		}	
		else if (nvram_match(strcat_r(prefix_mssid, "crypto", temp), "tkip"))
		{
			if (flag_8021x)
				fprintf(fp, "wpa_key_mgmt=WPA-EAP\n");
			else
				fprintf(fp, "wpa_key_mgmt=WPA-PSK\n");
			fprintf(fp, "wpa_strict_rekey=1\n");
			fprintf(fp, "eapol_version=2\n");
			fprintf(fp, "wpa_pairwise=TKIP\n");
		}	 
		else if (nvram_match(strcat_r(prefix_mssid, "crypto", temp), "aes")) 
		{
			if (flag_8021x)
			 	fprintf(fp, "wpa_key_mgmt=WPA-EAP\n");
			else
				fprintf(fp, "wpa_key_mgmt=WPA-PSK\n");
			fprintf(fp, "wpa_strict_rekey=1\n");
			fprintf(fp, "eapol_version=2\n");
			fprintf(fp, "wpa_pairwise=CCMP\n");
		}
		else if (nvram_match(strcat_r(prefix_mssid, "crypto", temp), "tkip+aes")) 
		{
			if (flag_8021x)
				fprintf(fp, "wpa_key_mgmt=WPA-EAP\n");
			else
				fprintf(fp, "wpa_key_mgmt=WPA-PSK\n");
			fprintf(fp, "wpa_strict_rekey=1\n");
			fprintf(fp, "eapol_version=2\n");
			fprintf(fp, "wpa_pairwise=TKIP CCMP\n");
		}	
		else 
		{
			warning = 25;
			fprintf(fp, "#wpa_pairwise=\n");
		}
	
	wpapsk=0;
	if ((nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "open")
	     && nvram_match(strcat_r(prefix_mssid, "wep_x", temp),"0")))
			fprintf(fp, "wpa=0\n");
	else if ((nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "open")
		  &&
		  nvram_invmatch(strcat_r(prefix_mssid, "wep_x", temp), "0"))
	          ||
		  nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp),"shared")
		  ||
		nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp),"radius"))
			fprintf(fp, "wpa=0\n");
	else if (nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "psk") 
			|| nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "wpa"))
	{	
		wpapsk=1;
		fprintf(fp, "wpa=1\n");
	}		
	else if (nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "psk2") 
			|| nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "wpa2"))
	{		
		wpapsk=2;
		fprintf(fp, "wpa=2\n");
	}		
	else if (nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "pskpsk2") 
			|| nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "wpawpa2"))
	{	
		wpapsk=3;
		fprintf(fp, "wpa=3\n");
	}
	else
	{
		warning = 25;
		fprintf(fp, "wpa=0\n");
	}
	
	if(wpapsk!=0)
	{   
	   	if(!flag_8021x)
		{   
			char nv[65];

			snprintf(nv, sizeof(nv), "%s", nvram_safe_get(strcat_r(prefix_mssid, "wpa_psk",temp)));
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
		bw = get_bw_via_channel(band, nvram_get_int(strcat_r(tmpfix, "channel", tmp)));
		if (bw > 0)
		{
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
				*t_bw = '\0';
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

#if !defined(RTCONFIG_SOC_IPQ40XX) && !defined(MAPAC1750)
					//ext ch
					str = nvram_safe_get(strcat_r(tmpfix, "nctrlsb", tmp));
					if(!strcmp(str,"lower"))
						strlcpy(t_ext, "PLUS", sizeof(t_ext));
					else
						strlcpy(t_ext, "MINUS", sizeof(t_ext));

					bw40_channel_check(band,t_ext);
#endif
				}
			}
		}
		else {
			//warning = 34;
			if (!legacy)
				strlcpy(t_bw, "HT20", sizeof(t_bw));
		}
	}

	snprintf(mode_cmd, sizeof(mode_cmd), "%s%s%s", t_mode, t_bw, t_ext);
	fprintf(fp3,"iwpriv %s mode %s\n", wif, mode_cmd);
	fprintf(fp3, "iwpriv %s puren %d\n", wif, puren);

	if (band) //only 5G
	{
#if defined(RTCONFIG_WIFI_QCA9557_QCA9882) || defined(RTCONFIG_QCA953X) || defined(RTCONFIG_QCA956X)
		if (subnet==0)
			fprintf(fp3,"iwpriv %s enable_ol_stats %d\n", get_vphyifname(band), nvram_get_int("traffic_5g")==1? 1:0);
#endif
#if defined(RTCONFIG_VHT80_80)
		if (!subnet && strstr(t_mode, "11ACV") && nvram_get_int(strcat_r(tmpfix, "bw", tmp)) == 4 && !strcmp(t_bw, "HT80_80")) {
			char *p = nvram_get(strcat_r(tmpfix, "cfreq2", tmp));
			unsigned int cfreq2 = p? safe_atoi(p) : 0;

			/* if bw = 80+80, set central frequency of 2-nd 80MHz segment. */
			if (cfreq2)
				fprintf(fp3, "iwpriv %s cfreq2 %d\n", wif, cfreq2);
			fprintf(fp3, "iwpriv %s emiwar80p80 1\n", wif);
		}
#endif
#if defined(RTCONFIG_VHT80_80) || defined(RTCONFIG_VHT160)
		if(!strcmp(mode_cmd, "11ACVHT160") || !strcmp(mode_cmd, "11ACVHT80_80"))
			fprintf(fp3,"iwpriv %s assocwar160 1\n", wif);
#endif
	}else{
		if(!strcmp(mode_cmd, "11G"))
			fprintf(fp3,"iwpriv %s pureg 1\n", wif);
	}

	if(sw_mode() == SW_MODE_REPEATER){
		snprintf(rpfix, sizeof(rpfix), "wl%d.1_", band);
		val = nvram_pf_get_int(rpfix, "channel");
	}
	else
	{
		val = nvram_pf_get_int(main_prefix, "channel");
	}

	if(val)
	{
		fprintf(fp3, "iwpriv %s dcs_enable 0\n", get_vphyifname(band));	//not to scan and change to other channels
		fprintf(fp3, "iwconfig %s channel %d\n",wif,val);
	}
	else if(subnet==0)
	{	
		char *CC;
		char b_channels[128];
		char *p = b_channels;

		CC = nvram_pf_safe_get(main_prefix, "country_code");
		b_channels[0] = '\0';

		fprintf(fp3, "wifitool %s block_acs_channel 0\n",wif);	//reset block_acs_channel
#if defined(VZWAC1300) || defined(MAPAC1300) || defined(MAPAC1750) /* temporary solution until ACS weighting is done */
		if(band) //5G
			p += snprintf(p, sizeof(b_channels) + b_channels - p, "%s36,40,44,48", (b_channels==p)?"":",");	//skip band1
#endif
#ifdef RTCONFIG_QCA_TW_AUTO_BAND4
		if(band) //5G
		{
			//for TW, acs but skip 5G band1 & band2
	                if(!strcmp(CC,"TW")
#if defined(RTCONFIG_TCODE)
			  || !strncmp(nvram_safe_get("territory_code"), "TW", 2) 

#endif
			)
				p += snprintf(p, sizeof(b_channels) + b_channels - p, "%s36,40,44,48,52,56,60,64", (b_channels==p)?"":",");	//skip band12 to use band4 in TW
		}	
#if defined(RTAC58U) || defined(RTAC59U)
		else
		{
			if (!strncmp(nvram_safe_get("territory_code"), "CX/01", 5)
			 || !strncmp(nvram_safe_get("territory_code"), "CX/05", 5))
				p += snprintf(p, sizeof(b_channels) + b_channels - p, "%s12,13", (b_channels==p)?"":",");	//skip channel 12,13 in CX on RT-AC58U
		}
#endif
#endif	/* RTCONFIG_QCA_TW_AUTO_BAND4 */
#ifdef MAPAC2200
		if(strcmp(CC,"GB")== 0 && band == 2)
			p += snprintf(p, sizeof(b_channels) + b_channels - p, "%s52,56,60,64", (b_channels==p)?"":",");	//skip band2 in GB of 5G2 on MAP-AC2200 to use band1
#endif
#if defined(MAPAC2200) || defined(MAPAC1300)
		if(strcmp(CC,"GB")== 0 && band == 1)
			p += snprintf(p, sizeof(b_channels) + b_channels - p, "%s132,136,140", (b_channels==p)?"":",");	//skip channels with limited bandwidth
#endif

		if(strlen(b_channels)>0)
			fprintf(fp3, "wifitool %s block_acs_channel %s\n",wif, b_channels);

		fprintf(fp3, "iwpriv wifi%d dcs_enable 0\n",band);	//not to scan and change to other channels
	}
	if(!band && strstr(t_mode, "11N") != NULL) //only 2.4G && N mode is used
		fprintf(fp3,"iwpriv %s disablecoext %d\n",wif,(bw==2)?1:0);	// when N mode is used

#if defined(RTCONFIG_WIFI_QCA9994_QCA9994)
	if (!subnet) {
		if (band == 1) {
			val = nvram_get_int("qca_fc_buf_min");
			if (val >= 64 && val <= 1000)
				fc_buf_min = val;
			fprintf(fp3, "iwpriv %s fc_buf_min %d\n", get_vphyifname(band), fc_buf_min);
		}
		if (nvram_pf_match(prefix, "hwol", "1")) {
			int i;
			char *vphyif = get_vphyifname(band);
			char *fc_buf_max[4] = { "4096", "8192", "12288", "16384" };

			if (strstr(t_mode, "11ACV")) {
				fc_buf_max[0] = "8192";
				fc_buf_max[1] = "16384";
				fc_buf_max[2] = "24576";
				fc_buf_max[3] = "32768";
			}

			for (i = 0; i < 4; ++i)
				fprintf(fp3, "iwpriv %s fc_buf%d_max %s\n", vphyif, i, fc_buf_max[i]);
		}
	}
#endif

#if defined(RTCONFIG_AMAS)
	if (sw_mode == SW_MODE_AP && nvram_match("re_mode", "1")) {
		fprintf(fp3, "iwpriv %s dbdc_enable 0\n", get_vphyifname(band));	//disable dbdc to avoid traffic suppression in sta interfaces such as sta0 and sta1.
	}
#endif	/* RTCONFIG_AMAS */

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
	__get_wlifname(band, subnet, athfix);
	str = nvram_safe_get(strcat_r(prefix, "macmode", tmp));
	if (str && strlen(str)) {
		fprintf(fp2, "iwpriv %s %s 3\n", athfix, QCA_MACCMD); //clear acl list
		fprintf(fp2, "iwpriv %s %s %d\n", athfix, QCA_MACCMD, mac_filter[0]);
	} else {
		warning = 47;
		fprintf(fp2, "iwpriv %s %s 0\n", athfix, QCA_MACCMD); //disable acl
	}

	list[0] = 0;
	list[1] = 0;
	if (nvram_invmatch(strcat_r(prefix, "macmode", tmp), "disabled")) {
		nv = nvp = strdup(nvram_safe_get(strcat_r(prefix, "maclist_x", tmp)));
		if (nv) {
			while ((b = strsep(&nvp, "<")) != NULL) {
				if (strlen(b) == 0)
					continue;
				fprintf(fp2,"iwpriv %s %s %s\n", athfix, QCA_ADDMAC, b);
			}
			free(nv);
		}
	}
    }

	if(rep_mode){
#if defined(RTCONFIG_AMAS)
		if(nvram_match("re_mode", "1"))
			gen_hostapd_wps_config(fp, band, subnet, br_if);
#endif
		if((nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "shared") || nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "open")) && !nvram_get_int("wps_enable")){
			fprintf(fp2, "ifconfig %s up\n", wif);
		}
	   goto next;
	}	
		
	int WdsEnable=0;
        int WdsEncrypType=0;
	if(subnet==0)
	{   
		snprintf(path5, sizeof(path5), NAWDS_SH_FMT, wif);
		unlink(path5);

		fprintf(fp2,"wlanconfig %s nawds mode 0\n",wif);
        	//WDS Enable
		if (sw_mode != SW_MODE_REPEATER
		    && !nvram_match(strcat_r(prefix, "mode_x", tmp), "0")) {

			if (!(fp5 = fopen(path5, "w+"))) {
				dbg("%s: open %s fail!\n", __func__, path5);
			} else {
				//WdsEnable
				str = nvram_safe_get(strcat_r(prefix, "mode_x", tmp));
				if (str && strlen(str)) {
					if ((nvram_match
					     (strcat_r(prefix, "auth_mode_x", tmp), "open")
					     ||
					     (nvram_match
					      (strcat_r(prefix, "auth_mode_x", tmp), "psk2")
					      && nvram_match(strcat_r(prefix, "crypto", tmp),
							     "aes")))
					 ) {
						if (safe_atoi(str) == 0)
							WdsEnable=0;
						else if (safe_atoi(str) == 1)
							WdsEnable=2;
						else if (safe_atoi(str) == 2) {
							if (nvram_match(strcat_r(prefix, "wdsapply_x", tmp), "0"))
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
				if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "open")
				    && nvram_match(strcat_r(prefix, "wep_x", tmp), "0"))
					     WdsEncrypType=0; //none
				else if (nvram_match
					 (strcat_r(prefix, "auth_mode_x", tmp), "open")
					 && nvram_invmatch(strcat_r(prefix, "wep_x", tmp), "0"))
					     WdsEncrypType=1; //wep
				else if (nvram_match
					 (strcat_r(prefix, "auth_mode_x", tmp), "psk2")
					 && nvram_match(strcat_r(prefix, "crypto", tmp), "aes"))
					     WdsEncrypType=2; //aes
				else
					     WdsEncrypType=0; //none

				i=0;

				wds_keyidx=0;
				memset(wds_key,0,sizeof(wds_key));
				memset(wds_mac,0,sizeof(wds_mac));
				if ((nvram_match(strcat_r(prefix, "mode_x", tmp), "1")
				     || (nvram_match(strcat_r(prefix, "mode_x", tmp), "2")
					 && nvram_match(strcat_r(prefix, "wdsapply_x", tmp),
							"1")))
				 &&
				  (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "open")
				 ||
				  (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "psk2")
				&& nvram_match(strcat_r(prefix, "crypto", tmp), "aes")))
				 ) {
					nv = nvp =
					    strdup(nvram_safe_get
						   (strcat_r(prefix, "wdslist", tmp)));
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

				if (nvram_match
					(strcat_r(prefix, "auth_mode_x", tmp), "open")
					&& nvram_invmatch(strcat_r(prefix, "wep_x", tmp),"0")) {
					wds_keyidx=nvram_get_int(strcat_r(prefix, "key", tmp));
					//snprintf(list, sizeof(list), "wl%d_key%s", band, nvram_safe_get(strcat_r(prefix, "key", tmp)));
					str = strcat_r(prefix, "key", tmp);
					str2 = nvram_safe_get(str);
					snprintf(list, sizeof(list), "%s%s", str, str2);
					snprintf(wds_key, sizeof(wds_key), "%s", nvram_safe_get(list));
				} else
				    if (nvram_match
					(strcat_r(prefix, "auth_mode_x", tmp), "psk2")
					&& nvram_match(strcat_r(prefix, "crypto", tmp),
						       "aes")) {
					wds_keyidx=nvram_get_int(strcat_r(prefix, "wpa_psk", tmp));
				    }

#if 0//debug
				dbg("wds_keyidx: %d\n",wds_keyidx);
				dbg("wds_key: %s\n",wds_key);
				dbg("wds_mac: %s | %s | %s |%s \n",wds_mac[0],wds_mac[1],wds_mac[2],wds_mac[3]);
#endif

				if (!band) {
					switch (nvram_get_int(strcat_r(prefix, "nmode_x", tmp))) {
					case 1:	/* N ; fall through */
					case 0:	/* B, G, N */
						caps = CAP_HT2040 | CAP_HT20 | CAP_DS;
#if defined(RTCONFIG_WIFI_QCA9990_QCA9990) || defined(RTCONFIG_WIFI_QCA9994_QCA9994)
						caps |= CAP_TS | CAP_4S;
#endif
						if (nvram_match(strcat_r(prefix, "txbf", tmp),"1"))
							caps |= CAP_TXBF;
						break;
					case 2:	/* B, G ; fall through */
					default:
						caps = 0;
						break;
					}
				} else {
					caps = CAP_HT2040 | CAP_HT20;
					switch (nvram_get_int(strcat_r(prefix, "nmode_x", tmp))) {
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

						if (nvram_match(strcat_r(prefix, "txbf", tmp),"1"))
							caps |= CAP_TXBF;
						break;
					case 2:	/* Legacy, MTK: A only ; QCA: A & N ; fall through */
					default:
						caps = 0;
						break;
					}
				}

				dbg("WDS:defcaps 0x%x\n", caps);
				fprintf(fp2,"iwpriv %s wds 1\n", wif);
				fprintf(fp2,"wlanconfig %s nawds override 1\n",wif);
				fprintf(fp2,"wlanconfig %s nawds defcaps 0x%x\n", wif, caps);
				if(WdsEnable!=2 || nvram_match(strcat_r(prefix, "wdsapply_x", tmp),"1"))
					fprintf(fp2,"wlanconfig %s nawds mode %d\n",wif,(WdsEnable==2)?4:3);

				for(i=0;i<4;i++)
					if(strlen(wds_mac[i]) && nvram_match(strcat_r(prefix, "wdsapply_x", tmp),"1"))
						fprintf(fp5, "wlanconfig %s nawds add-repeater %s 0x%x\n", wif, wds_mac[i], caps);

				if(WdsEncrypType==0)
					dbg("WDS:open/none\n");
				else  if(WdsEncrypType==1)
				{
					dbg("WDS:open/wep\n");
					fprintf(fp2,"iwconfig %s key [%d]\n",wif,wds_keyidx);

					if(strlen(wds_key)==10 || strlen(wds_key)==26)
						fprintf(fp2,"iwconfig %s key %s\n",wif,wds_key);
					else if(strlen(wds_key)==5 || strlen(wds_key)==13)
						fprintf(fp2,"iwconfig %s key \"s:%s\"\n",wif,wds_key);
				}
				else
					dbg("WDS:unknown\n");

				fclose(fp5);
			}
		}
	}
	if(flag_8021x)
	{    
		//radius server
		if (!strcmp(nvram_safe_get(strcat_r(tmpfix, "radius_ipaddr", tmp)), ""))
			fprintf(fp, "auth_server_addr=169.254.1.1\n");
		else
			fprintf(fp, "auth_server_addr=%s\n",
				nvram_safe_get(strcat_r(tmpfix, "radius_ipaddr", tmp)));

		//radius port
		str = nvram_safe_get(strcat_r(tmpfix, "radius_port", tmp));
		if (str && strlen(str))
			fprintf(fp, "auth_server_port=%d\n", safe_atoi(str));
		else {
			warning = 50;
			fprintf(fp, "auth_server_port=1812\n");
		}

		//radius key
		str = nvram_safe_get(strcat_r(tmpfix, "radius_key", tmp));
		if (str && strlen(str))
			fprintf(fp, "auth_server_shared_secret=%s\n",
				nvram_safe_get(strcat_r(tmpfix, "radius_key", tmp)));
		else
		   	fprintf(fp,"#auth_server_shared_secret=\n");
	}	


	//RadioOn
	str = nvram_safe_get(strcat_r(tmpfix, "radio", tmp));
	if (str && strlen(str)) {
		char *updown = (safe_atoi(str) /* && nvram_get_int(strcat_r(prefix, "bss_enabled", tmp)) */ )? "up" : "down";
		fprintf(fp2, "ifconfig %s %s\n", wif, updown);

		/* Connect to peer WDS AP after VAP up */
		if (safe_atoi(str) && !subnet && f_exists(path5))
			fprintf(fp2, "%s\n", path5);
	}
	//igmp
	fprintf(fp2, "iwpriv %s mcastenhance %d\n",wif,
		nvram_get_int(strcat_r(tmpfix, "igs", tmp)) ? 2 : 0);

	i = nvram_get_int(strcat_r(tmpfix, "mrate_x", tmp));
next_mrate:
	switch (i++) {
		default:
		case 0:		/* Driver default setting: Disable, means automatic rate instead of fixed rate
			 * Please refer to #ifdef MCAST_RATE_SPECIFIC section in
			 * file linuxxxx/drivers/net/wireless/rtxxxx/common/mlme.c
			 */
			break;
		case 1:		/* Legacy CCK 1Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 1000) &\n",wif);
			mcast_phy = 1;
			break;
		case 2:		/* Legacy CCK 2Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 2000) &\n",wif);
			mcast_phy = 1;
			break;
		case 3:		/* Legacy CCK 5.5Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 5500) &\n",wif);
			mcast_phy = 1;
			break;
		case 4:		/* Legacy OFDM 6Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 6000) &\n",wif);
			break;
		case 5:		/* Legacy OFDM 9Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 9000) &\n",wif);
			break;
		case 6:		/* Legacy CCK 11Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 11000) &\n",wif);
			mcast_phy = 1;
			break;
		case 7:		/* Legacy OFDM 12Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 12000) &\n",wif);
			break;
		case 8:		/* Legacy OFDM 18Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 18000) &\n",wif);
			break;
		case 9:		/* Legacy OFDM 24Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 24000) &\n",wif);
			break;
		case 10:		/* Legacy OFDM 36Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 36000) &\n",wif);
			break;
		case 11:		/* Legacy OFDM 48Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 48000) &\n",wif);
			break;
		case 12:		/* Legacy OFDM 54Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 54000) &\n",wif);
			break;
		case 13:		/* HTMIX 130/300Mbps 2S */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 130000) &\n",wif);
			break;
		case 14:		/* HTMIX 6.5/15Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 6500) &\n",wif);
			break;
		case 15:		/* HTMIX 13/30Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 13000) &\n",wif);
			break;
		case 16:		/* HTMIX 19.5/45Mbps */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 19500) &\n",wif);
			break;
		case 17:		/* HTMIX 13/30Mbps 2S */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 13000) &\n",wif);
			break;
		case 18:		/* HTMIX 26/60Mbps 2S */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 26000) &\n",wif);
			break;
		case 19:		/* HTMIX 39/90Mbps 2S */
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate 39000) &\n",wif);
			break;
		case 20:
			/* Choose multicast rate base on mode, encryption type, and IPv6 is enabled or not. */
			__choose_mrate(tmpfix, &mcast_phy, &mcast_mcs, &rate);
			fprintf(fp2, "(sleep 10 ; iwpriv %s mcast_rate %d) &\n",wif,rate);
			break;
		}
	/* No CCK for 5Ghz band */
	if (band && mcast_phy == 1)
		goto next_mrate;

	gen_hostapd_wps_config(fp, band, subnet, br_if);

#if RTCONFIG_AIR_TIME_FAIRNESS
	if (nvram_get_int(strcat_r(prefix, "atf", tmp)) && subnet == 0) {
		char *nv, *nvp, *b;
		char *mac, *pct, *ssid;
		char wlfix[] = "wlXXXXXXX_";
		unsigned char emac[ETHER_ADDR_LEN];
		int atf_mode = nvram_get_int(strcat_r(prefix, "atf_mode", tmp));
		int i = 0;

		switch (atf_mode) {
			case 2:/* by client + SSID */
			case 0:/* by client */
				nv = nvp = strdup(nvram_safe_get(strcat_r(prefix, "atf_sta", tmp)));
				while ((b = strsep(&nvp, "<")) != NULL) {
					if ((vstrsep(b, ">", &mac, &pct) != 2)) continue;
					ether_atoe(mac, emac);
					//dbg("[ATF by client] mac[%s] set %s%\n", mac, pct);
					fprintf(fp2, "wlanconfig %s addsta %02X%02X%02X%02X%02X%02X %s\n", 
							wif, emac[0], emac[1], emac[2], emac[3], emac[4], emac[5], pct);
				}
				free(nv);
				if (atf_mode == 0)
					break;
				/* fall through */
			case 1:/* by SSID */
				nv = nvp = strdup(nvram_safe_get(strcat_r(prefix, "atf_ssid", tmp)));
				while ((b = strsep(&nvp, "<")) != NULL) {
					if (!strcmp(b, "")) continue;
					if (i == 0)
						snprintf(wlfix, sizeof(wlfix), "wl%d_", band);
					else
						snprintf(wlfix, sizeof(wlfix), "wl%d.%d_", band, i);
					ssid = nvram_safe_get(strcat_r(wlfix, "ssid", tmp));
					i++;
					if (!strcmp(b, "0")) continue;
					//dbg("[ATF by SSID] ssid[%s] set %s%\n", ssid, b);
					fprintf(fp2, "wlanconfig %s addssid %s %s\n", wif, ssid, b);
				}
				free(nv);
				break;
			default:
				;
		}
		fprintf(fp2, "iwpriv %s commitatf 1\n", wif);
	}
#endif

#if defined(RTCONFIG_SOC_IPQ40XX)
	if (!subnet)
		fprintf(fp2, "iwpriv wifi%d dl_loglevel 5\n",band);	// disable log from target firmware
#endif

next:
	fclose(fp);
	fclose(fp2);
	fclose(fp3);
	chmod(path2, 0777);	/* postwifi_athX.sh */
	chmod(path3, 0777);	/* prewifi_athX.sh */
	chmod(path5, 0777);	/* nawds_athX.sh */

	(void) warning;
	return 0;
}

#if defined(RTCONFIG_WIGIG)
/**
 * In charge of nl80211 based 802.11ad Wigig driver's WDS support.
 * @band
 * @subnet
 * @return:
 * 	1:	WDS is not enabled.
 * 	0:	success
 * 	-1:	invalid parameter.
 * 	-2:	open nawds shell script for writing failed.
 */
static int _gen_nl80211_wds_config(int band, int subnet, FILE *fp_post)
{
#if 0
	int i, v, warning, caps, sw_mode = sw_mode();
	int WdsEnable, WdsEncrypType, wds_keyidx;
	char tmp[128], list[2048], wds_mac[4][30], wds_key[50], mode_cmd[100];
	char *str, *str2, *nv, *nvp, *b;
	char wif[IFNAMSIZ], prefix[] = "wlXXXXXXX_";
	char wds_path[sizeof(NAWDS_SH_FMT) + 6] = "";
	FILE *fp_wds;

	if (band < 0 || band >= MAX_NR_WL_IF || subnet != 0 || !fp_post)
		return -1;

#if !defined(RTCONFIG_HAS_5G_2)
	if (band == 2)
		return -1;
#endif

	__get_wlifname(band, subnet, wif);
	snprintf(wds_path, sizeof(wds_path), NAWDS_SH_FMT, wif);
	unlink(wds_path);
	snprintf(prefix, sizeof(prefix), "wl%d_", band);

	fprintf(fp_post, "wlanconfig %s nawds mode 0\n", wif);
	/* WDS Enable */
	if (sw_mode == SW_MODE_REPEATER || nvram_pf_match(prefix, "mode_x", "0"))
		return 1;

	if (!(fp_wds = fopen(wds_path, "w"))) {
		dbg("%s: open %s fail!\n", __func__, wds_path);
		return -2;
	}

	list[0] = 0;
	list[1] = 0;
	/* WdsEnable */
	WdsEnable = 0;
	str = nvram_pf_safe_get(prefix,"mode_x");
	if (str && *str != '\0') {
		v = safe_atoi(str);
		if (nvram_pf_match(prefix,"auth_mode_x", "open") ||
		    (nvram_pf_match(prefix,"auth_mode_x", "psk2") &&
		     nvram_pf_match(prefix,"crypto", "aes")))
		{
			switch (v) {
			case 0:
				WdsEnable = 0;
				break;
			case 1:
				WdsEnable = 2;
				break;
			case 2:
				if (nvram_pf_match(prefix,"wdsapply_x", "0"))
					WdsEnable = 4;
				else
					WdsEnable = 3;
				break;
			default:
				dbg("%s: Unknown WDS mode [%d]\n", __func__, v);
			}
		}
	} else {
		warning = 49;
	}

	/* WdsEncrypType */
	WdsEncrypType = 0;	//none
	if (nvram_pf_match(prefix, "auth_mode_x", "open") &&
	    nvram_pf_match(prefix, "wep_x", "0"))
		WdsEncrypType = 0;	//none
	else if (nvram_pf_match(prefix, "auth_mode_x", "open") &&
		 nvram_pf_invmatch(prefix, "wep_x", "0"))
		WdsEncrypType = 1;	//wep
	else if (nvram_pf_match(prefix, "auth_mode_x", "psk2") &&
		 nvram_pf_match(prefix, "crypto", "aes"))
		WdsEncrypType = 2;	//aes

	i = 0;
	wds_keyidx = 0;
	memset(wds_key, 0, sizeof(wds_key));
	memset(wds_mac, 0, sizeof(wds_mac));
	if ((nvram_pf_match(prefix, "mode_x", "1") ||
	     (nvram_pf_match(prefix, "mode_x", "2") &&
	      nvram_pf_match(prefix, "wdsapply_x", "1"))) &&
	    (nvram_pf_match(prefix, "auth_mode_x", "open") ||
	     (nvram_pf_match(prefix, "auth_mode_x", "psk2") &&
	      nvram_pf_match(prefix, "crypto", "aes"))))
	{
		nv = nvp = strdup(nvram_pf_safe_get(prefix, "wdslist"));
		if (nv) {
			while ((b = strsep(&nvp, "<")) != NULL) {
				if (strlen(b) == 0)
					continue;
				sprintf(wds_mac[i], "%s", b);
				i++;
			}
			free(nv);
		}
	}

	if (nvram_pf_match(prefix, "auth_mode_x", "open") &&
	    nvram_pf_invmatch(prefix, "wep_x", "0")) {
		wds_keyidx = nvram_pf_get_int(prefix, "key");
		//sprintf(list, "wl%d_key%s", band, nvram_pf_safe_get(prefix, "key"));
		str = strcat_r(prefix, "key", tmp);
		str2 = nvram_safe_get(str);
		sprintf(list, "%s%s", str, str2);
		sprintf(wds_key, "%s", nvram_safe_get(list));
	} else if (nvram_pf_match(prefix, "auth_mode_x", "psk2") &&
		   nvram_pf_match(prefix, "crypto", "aes")) {
		wds_keyidx = nvram_pf_get_int(prefix, "wpa_psk");
	}

	if (!band) {
		switch (nvram_pf_get_int(prefix, "nmode_x")) {
		case 1:	/* N ; fall through */
		case 0:	/* B, G, N */
			caps = CAP_HT2040 | CAP_HT20 | CAP_DS;
#if defined(RTCONFIG_WIFI_QCA9990_QCA9990) || defined(RTCONFIG_WIFI_QCA9994_QCA9994)
			caps |= CAP_TS | CAP_4S;
#endif
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
	fprintf(fp_post, "iwpriv %s wds 1\n", wif);
	fprintf(fp_post, "wlanconfig %s nawds override 1\n", wif);
	fprintf(fp_post, "wlanconfig %s nawds defcaps 0x%x\n", wif, caps);
	if (WdsEnable != 2 || nvram_pf_match(prefix, "wdsapply_x", "1"))
		fprintf(fp_post, "wlanconfig %s nawds mode %d\n", wif, (WdsEnable == 2) ? 4 : 3);

	for (i = 0; i < 4; i++)
		if (strlen(wds_mac[i])
		    && nvram_pf_match(prefix, "wdsapply_x", "1"))
			fprintf(fp_wds, "wlanconfig %s nawds add-repeater %s 0x%x\n", wif, wds_mac[i], caps);

	if (WdsEncrypType == 0)
		dbg("WDS:open/none\n");
	else if (WdsEncrypType == 1) {
		dbg("WDS:open/wep\n");
		fprintf(fp_post, "iwconfig %s key [%d]\n", wif, wds_keyidx);

		if (strlen(wds_key) == 10 || strlen(wds_key) == 26)
			fprintf(fp_post, "iwconfig %s key %s\n", wif, wds_key);
		else if (strlen(wds_key) == 5 || strlen(wds_key) == 13)
			fprintf(fp_post, "iwconfig %s key \"s:%s\"\n", wif, wds_key);
	} else
		dbg("WDS:unknown\n");

	fclose(fp_wds);

	(void)warning;
#endif
	return 0;
}

/**
 * nl80211 based 802.11ad Wigig driver configuartion code. (Generate shell script for configuration only.)
 * @band:	So far, it is supposed 3 which is dedicated to 802.11ad Wigig.
 * @is_iNIC:	Not used.
 * @subnet:	If non-zero, id of guest network.
 * 		PS: Kernel 3.4 802.11ad Wigig driver doesn't su pport multiple SSID.
 * @return:
 * 	0:	Generate shell script success.
 *  otherwise:	failure.
 */
int gen_nl80211_config(int band, int is_iNIC, int subnet)
{
	const int min_bintval = 100;
	int i, j, v, bw = 0, only_20m = 0, max_subnet;
	int ban, sw_mode = sw_mode(), wpapsk, nband;
	int val, rep_mode, nmode, macmode;
	int flag_8021x = 0, warning = 0;
//	int shortgi, stbc, rate, puren = 0, mcast_phy = 0, mcast_mcs = 0;
	char *str = NULL, *nv, *nvp, *b, *hw_mode = "", *wps_band = "";
	char prefix[] = "wlXXXXXXX_", main_prefix[] = "wlXXX_";
	char prefix_mssid[] = "wlXXXXXXXXXX_mssid_", tmpstr[128], psk[65], *p;
	char t_mode[30], t_bw[10], t_ext[10], mode_cmd[100];
	char wif[IFNAMSIZ], vphy[IFNAMSIZ];
	char hostapd_path[50], postwifi_path[50], prewifi_path[50], mac_path[50];
	char wds_path[sizeof(NAWDS_SH_FMT) + 6] = "";
	char *uuid = nvram_safe_get("uuid");
//	char code[10] = "";
	FILE *fp_h, *fp_post, *fp_pre, *fp_mac;
#if 0 //defined(RTCONFIG_WIFI_QCA9990_QCA9990) || defined(RTCONFIG_WIFI_QCA9994_QCA9994) || defined(RTCONFIG_SOC_IPQ40XX)
	int fc_buf_min = 1000;
	int txbf, mumimo, ldpc = 1, tqam, tqam_intop;
	unsigned int maxsta = 511;
#else
	unsigned int maxsta = 127;
#endif
#ifdef RTCONFIG_WIRELESSREPEATER
	int wlc_band = nvram_get_int("wlc_band");
#endif
	char br_if[IFNAMSIZ];

	if (band < 0 || band >= MAX_NR_WL_IF || subnet < 0)
		return -1;

#if !defined(RTCONFIG_HAS_5G_2)
	if (band == 2)
		return -1;
#endif

	max_subnet = num_of_mssid_support(band);
	if (subnet > max_subnet)
		return -1;
	snprintf(main_prefix, sizeof(main_prefix), "wl%d_", band);
	if (subnet == 0)
		snprintf(prefix, sizeof(prefix), "wl%d_", band);
	else {
		j = 0;
		for (i = 1; i <= max_subnet; i++) {
			snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d.%d_", band, i);
			if (nvram_pf_match(prefix_mssid,"bss_enabled", "1")) {
				j++;
				if (j == subnet)
					snprintf(prefix, sizeof(prefix), "wl%d.%d_", band, i);
			}

		}
	}

	nmode = nvram_pf_get_int(main_prefix, "nmode_x");
	switch (band) {
	case WL_2G_BAND:
		dbg("%s: nl80211-based 2.4G hasn't been supported.\n", __func__);
		hw_mode = wps_band = "g";
		break;
	case WL_5G_BAND:	/* fall-through */
	case WL_5G_2_BAND:
		dbg("%s: nl80211-based 5G/5G-2 hasn't been supported.\n", __func__);
		hw_mode = wps_band = "a";
		break;
	case WL_60G_BAND:
		/* Kernel v3.4 802.11ad Wigig driver only supports 8 clients. */
		maxsta = 8;
		wps_band = "ad";
		switch (nmode) {
		case 0:	/* Auto, ad */
			hw_mode = "ad";
			break;
		default:
			warning = 5;
			hw_mode = "ad";
		}
		break;
	default:
		dbg("%s: unknown wl%d band!\n", __func__, band);
		return -1;
	}

	rep_mode = 0;
	ban = 0;
	memset(mode_cmd, 0, sizeof(mode_cmd));
	memset(t_mode, 0, sizeof(t_mode));
	memset(t_bw, 0, sizeof(t_bw));
	memset(t_ext, 0, sizeof(t_ext));

	__get_wlifname(band, subnet, wif);
	strcpy(vphy, get_vphyifname(band));

	snprintf(hostapd_path, sizeof(hostapd_path), "/etc/Wireless/conf/hostapd_%s.conf", wif);
	snprintf(mac_path, sizeof(hostapd_path), "/etc/Wireless/conf/maclist_%s.conf", wif);
	snprintf(postwifi_path, sizeof(postwifi_path), "/etc/Wireless/sh/postwifi_%s.sh", wif);
	snprintf(prewifi_path, sizeof(prewifi_path), "/etc/Wireless/sh/prewifi_%s.sh", wif);
	snprintf(wds_path, sizeof(wds_path), NAWDS_SH_FMT, wif);
	eval("mkdir", "-p", "/etc/Wireless/conf");
	eval("mkdir", "-p", "/etc/Wireless/sh");

	if (nvram_match("skip_gen_ath_config", "1") &&
	    f_exists(postwifi_path) && f_exists(prewifi_path))
	{
		dbg("%s: reuse %s and %s\n", __func__, postwifi_path, prewifi_path);
		return 0;
	}

	_dprintf("gen qca 11ad config\n");
	if (!(fp_h = fopen(hostapd_path, "w+")))
		return 0;
	if (!(fp_mac = fopen(mac_path, "w")))
		return 0;
	if (!(fp_post = fopen(postwifi_path, "w+")))
		return 0;
	if (!(fp_pre = fopen(prewifi_path, "w+")))
		return 0;

#ifdef RTCONFIG_WIRELESSREPEATER
	if (sw_mode == SW_MODE_REPEATER && wlc_band == band && nvram_invmatch("wlc_ssid", "") && !subnet) {
		rep_mode = 1;
	}
#endif

	fprintf(fp_h, "#The word of \"Default\" must not be removed\n");
	fprintf(fp_h, "#Default\n");

#ifdef RTCONFIG_WIRELESSREPEATER
	if (rep_mode == 1) {
		char prefix_wlc[32];
		snprintf(prefix, sizeof(prefix), "wl%d.1_", band);
		snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc_");
		snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d_", band);
		// convert wlc_xxx to wlX_ according to wlc_band == band
		nvram_set("ure_disable", "0");
		write_rpt_wpa_supplicant_conf(band, prefix_mssid, prefix_wlc, NULL);
	}
#endif /* RTCONFIG_WIRELESSREPEATER */

	fprintf(fp_h, "interface=%s\n", wif);
	fprintf(fp_h, "ctrl_interface=/var/run/hostapd\n");
	fprintf(fp_h, "driver=nl80211\n");
	strlcpy(br_if, nvram_get("lan_ifname")? : nvram_default_get("lan_ifname"), sizeof(br_if));
#if defined(RTCONFIG_TAGGED_BASED_VLAN) || defined(RTCONFIG_CAPTIVE_PORTAL)
	find_brifname_by_wlifname(wif, br_if, 16);
#ifdef RTCONFIG_CAPTIVE_PORTAL
	FindBrifByWlif(wif, br_if, 16);
#endif
#endif
	if(*br_if == '\0')
		strcpy(br_if, "br0");
	fprintf(fp_h, "bridge=%s\n", br_if);
	fprintf(fp_h, "logger_syslog=-1\n");
	fprintf(fp_h, "logger_syslog_level=2\n");
	fprintf(fp_h, "logger_stdout=-1\n");
	fprintf(fp_h, "logger_stdout_level=2\n");
	fprintf(fp_h, "ctrl_interface_group=0\n");
	nband = nvram_pf_get_int(main_prefix, "nband");
#if 0
	/* Use "iw reg set CC" to set country in __load_wifi_driver()
	 * instead of set it in each bands.
	 */
	if (country_to_code(nvram_pf_get(main_prefix, "country_code"), nband, code, sizeof(code)) < 0) {
		dbg("%s: unknown country code. (band %d, country_code %s\n",
			__func__, band, nvram_pf_get(main_prefix, "country_code"));
		country_to_code("DB", nband, code, sizeof(code));
	}
	fprintf(fp_h, "country_code=%s\n", code);
#endif

	/* Set maximum number of clients of a guest network. */
	if (subnet) {
		val = nvram_pf_get_int(prefix, "guest_num");
		if (val > 0 && val <= maxsta)
			fprintf(fp_h, "max_num_sta=%d\n", maxsta);
	}

	fprintf(fp_h, "ignore_broadcast_ssid=0\n");

	fprintf(fp_h, "eapol_key_index_workaround=0\n");
	flag_8021x = 0;

	/* Kernel v3.4 802.11ad Wigig driver doesn't support WPA-Enterprise. */
	str = nvram_pf_safe_get(prefix, "auth_mode_x");
	if (str && (!strcmp(str, "radius") || !strcmp(str, "wpa") ||
		    !strcmp(str, "wpa2") || !strcmp(str, "wpawpa2")))
	{
		flag_8021x = 1;
		fprintf(fp_h, "ieee8021x=1\n");
		fprintf(fp_h, "eap_server=0\n");
	} else {
		fprintf(fp_h, "ieee8021x=0\n");
		fprintf(fp_h, "eap_server=1\n");
	}

	/* SSID Num. [MSSID Only] */
	fprintf(fp_h, "ssid=%s\n", nvram_pf_safe_get(prefix, "ssid"));

	/* WMM */
	fprintf(fp_h, "wmm_enabled=1\n");

	/* Network Mode */
	fprintf(fp_h, "hw_mode=%s\n", hw_mode);

#if 0
#if defined(RTCONFIG_WIFI_QCA9990_QCA9990) || defined(RTCONFIG_WIFI_QCA9994_QCA9994) || defined(RTCONFIG_SOC_IPQ40XX)
	/* 2.4GHz 256 QAM */
	if (!band) {
		/* 256-QAM can't be enabled, if HT mode is not enabled. */
		tqam = (nmode != 2)? !!nvram_pf_get_int(main_prefix, "turbo_qam") : 0;
		fprintf(fp_post, "iwpriv %s vht_11ng %d\n", wif, tqam);
		tqam_intop = tqam? !!nvram_pf_get_int(main_prefix, "turbo_qam_brcm_intop") : 0;
		fprintf(fp_post, "iwpriv %s 11ngvhtintop %d\n", wif, tqam_intop);
	}
#endif

	/* Short-GI */
	shortgi = (nmode != 2)? !!nvram_pf_get_int(main_prefix, "HT_GI") : 0;
	fprintf(fp_post, "iwpriv %s shortgi %d\n", wif, shortgi);

	// STBC
	stbc = (nmode != 2)? !!nvram_pf_get_int(main_prefix, "HT_STBC") : 0;
	fprintf(fp_post, "iwpriv %s tx_stbc %d\n", wif, stbc);
	fprintf(fp_post, "iwpriv %s rx_stbc %d\n", wif, stbc);

#if defined(RTCONFIG_WIFI_QCA9990_QCA9990) || defined(RTCONFIG_WIFI_QCA9994_QCA9994) || defined(RTCONFIG_SOC_IPQ40XX)
	// TX BeamForming, must be set before association with the station.
	txbf = (nmode != 2)? !!nvram_pf_get_int(main_prefix, "txbf") : 0;
	mumimo = (nmode != 2)? !!(band && nvram_pf_get_int(main_prefix, "mumimo")) : 0;
	fprintf(fp_post, "iwpriv %s vhtsubfer %d\n", wif, txbf);	/* Single-user beam former */
	fprintf(fp_post, "iwpriv %s vhtsubfee %d\n", wif, txbf);	/* Single-user beam formee */
	if (!repeater_mode() && !mediabridge_mode()) {
		fprintf(fp_post, "iwpriv %s vhtmubfer %d\n", wif, mumimo);	/* Multiple-user beam former, AP only */
	} else {
		fprintf(fp_post, "iwpriv %s vhtmubfee %d\n", wif, mumimo);	/* Multiple-user beam formee, STA only */
	}

	if(mumimo)
		fprintf(fp_post, "wifitool %s beeliner_fw_test 2 0\n", wif);	/* Improve Multiple-user mimo performance */

	fprintf(fp_post, "iwpriv %s implicitbf %d\n", wif, nvram_pf_get_int(main_prefix, "implicitxbf"));

#ifdef RTCONFIG_OPTIMIZE_XBOX
	// LDPC
	if (nvram_pf_match(prefix, "optimizexbox", "1"))
		ldpc = 0;
	fprintf(fp_post, "iwpriv %s ldpc %d\n", wif, ldpc);
#endif
#endif
#endif

#if 0
	/* Hide SSID, need kernel v3.14 or above 802.11ad Wigig driver. */
	fprintf(fp_post, "iwpriv %s hide_ssid %d\n", wif, nvram_pf_get_int(prefix, "closed"));
	if (!nvram_pf_get_int(prefix, "closed")) {
		int n;
		char nv[33], buf[128];

		snprintf(nv, sizeof(nv), "%s", nvram_pf_safe_get(prefix, "ssid"));
		//replace SSID each char to "\char"
		memset(buf, 0x0, sizeof(buf));
		for (n = 0; n < strlen(nv); n++)
			snprintf(buf, sizeof(buf), "%s\\%c", buf, nv[n]);
		fprintf(fp_post, "iwconfig %s essid -- %s\n", wif, buf);
	}
#endif

	if (subnet == 0 && rep_mode == 0) {
#if 0
		/* TxPreamble */
		if (nvram_pf_match(prefix, "plcphdr", "short"))
			v = 1;
		fprintf(fp_post, "iwpriv %s shpreamble %d\n", wif, v);

		/* RTSThreshold  Default=2347 */
		v = 2347;
		str = nvram_pf_get(prefix, "rts");
		if (str && *str != '\0') {
			v = safe_atoi(str);
		} else {
			warning = 14;
		}
		fprintf(fp_post, "iwconfig %s rts %d\n", wif, v);

		/* DTIM Period */
		v = 1;
		str = nvram_pf_get(prefix, "dtim");
		if (str && *str != '\0') {
			v = safe_atoi(str);
		} else {
			warning = 11;
		}
		fprintf(fp_post, "iwpriv %s dtim_period %d\n", wif, v);
#endif

		/* BeaconPeriod */
		v = min_bintval;
		str = nvram_pf_get(prefix, "bcn");
		if (str && *str != '\0') {
			if ((i = safe_atoi(str)) <= 1000 && i > min_bintval)
				v = i;
		} else {
			warning = 10;
		}
		fprintf(fp_h, "beacon_int=%d\n", v);

#if 0
		/* APSDCapable */
		v = 1;
		str = nvram_pf_safe_get(prefix,"wme_apsd");
		if (str && *str != '\0') {
			v = strcmp(str, "off") ? 1 : 0;
		} else {
			warning = 18;
		}
		fprintf(fp_post, "iwpriv %s uapsd %d\n", wif, v);

		/* TxBurst */
		v = 1;
		str = nvram_pf_safe_get(prefix,"frameburst");
		if (str && *str != '\0') {
			v = strcmp(str, "off") ? 1 : 0;
		} else {
			warning = 16;
		}
		fprintf(fp_post, "iwpriv %s burst %d\n", vphy, v);
#endif
	}

	fprintf(fp_h, "ap_isolate=%d\n", !!nvram_pf_get_int(main_prefix, "ap_isolate"));

	/* AuthMode */
	memset(tmpstr, 0x0, sizeof(tmpstr));
	str = nvram_pf_get(prefix,"auth_mode_x");
	if (str && strlen(str)) {
		if (!strcmp(str, "open")) {
			fprintf(fp_h, "auth_algs=1\n");
		} else if (!strcmp(str, "shared")) {
			fprintf(fp_h, "auth_algs=2\n");
		} else if (!strcmp(str, "radius")) {
			fprintf(fp_h, "auth_algs=1\n");
			fprintf(fp_h, "wep_key_len_broadcast=5\n");
			fprintf(fp_h, "wep_key_len_unicast=5\n");
			fprintf(fp_h, "wep_rekey_period=300\n");
		} else {
			//wpa/wpa2/wpa-auto-enterprise:wpa/wpa2/wpawpa2
			//wpa/wpa2/wpa-auto-personal:psk/psk2/pskpsk2
			fprintf(fp_h, "auth_algs=1\n");
		}
	} else {
		warning = 24;
		fprintf(fp_h, "auth_algs=1\n");
	}

	/* EncrypType */
	memset(tmpstr, 0x0, sizeof(tmpstr));

	str = nvram_pf_get(prefix,"wpa_gtk_rekey");
	if (str && *str != '\0') {
		fprintf(fp_h, "wpa_group_rekey=%d\n", safe_atoi(str));
	}

	if ((nvram_pf_match(prefix, "auth_mode_x", "open") &&
	     nvram_pf_match(prefix, "wep_x", "0"))) {
		fprintf(fp_h, "#wpa_pairwise=\n");
	} else if ((nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_invmatch(prefix, "wep_x", "0")) ||
		    nvram_pf_match(prefix, "auth_mode_x", "shared") ||
		    nvram_pf_match(prefix, "auth_mode_x", "radius"))
	{
		//wep
		if (nvram_pf_match(prefix, "auth_mode_x", "shared"))
			fprintf(fp_post, "iwpriv %s authmode 2\n", wif);
		else if (nvram_pf_match(prefix, "auth_mode_x", "radius"))
			fprintf(fp_post, "iwpriv %s authmode 3\n", wif);
		else
			fprintf(fp_post, "iwpriv %s authmode 1\n", wif);

		for (i = 1; i <= 4; i++) {
			snprintf(tmpstr, sizeof(tmpstr), "%skey%d", prefix, i);
			if (strlen(nvram_safe_get(tmpstr)) == 10 || strlen(nvram_safe_get(tmpstr)) == 26)
				fprintf(fp_post, "iwconfig %s key [%d] %s\n", wif, i, nvram_safe_get(tmpstr));
			else if (strlen(nvram_safe_get(tmpstr)) == 5 || strlen(nvram_safe_get(tmpstr)) == 13)
				fprintf(fp_post, "iwconfig %s key [%d] \"s:%s\"\n", wif, i, nvram_safe_get(tmpstr));
			else
				fprintf(fp_post, "iwconfig %s key [%d] off\n", wif, i);
		}

		str = nvram_pf_safe_get(prefix, "key");
		snprintf(tmpstr, sizeof(tmpstr), "%skey%s", prefix, str);
		fprintf(fp_post, "iwconfig %s key [%s]\n", wif, str);	//key index

		if (strlen(nvram_safe_get(tmpstr)) == 10 || strlen(nvram_safe_get(tmpstr)) == 26)
			fprintf(fp_post, "iwconfig %s key %s\n", wif, nvram_safe_get(tmpstr));
		else if (strlen(nvram_safe_get(tmpstr)) == 5 || strlen(nvram_safe_get(tmpstr)) == 13)
			fprintf(fp_post, "iwconfig %s key \"s:%s\"\n", wif, nvram_safe_get(tmpstr));
		else
			fprintf(fp_h, "#wpa_pairwise=\n");
	} else if (nvram_pf_match(prefix, "crypto", "tkip")) {
		fprintf(fp_h, "wpa_key_mgmt=WPA-%s\n", flag_8021x? "EAP" : "PSK");
		fprintf(fp_h, "wpa_strict_rekey=1\n");
		fprintf(fp_h, "eapol_version=2\n");
		fprintf(fp_h, "wpa_pairwise=TKIP\n");
	} else if (nvram_pf_match(prefix, "crypto", "aes")) {
		fprintf(fp_h, "wpa_key_mgmt=WPA-%s\n", flag_8021x? "EAP" : "PSK");
		fprintf(fp_h, "wpa_strict_rekey=1\n");
		fprintf(fp_h, "eapol_version=2\n");
		fprintf(fp_h, "wpa_pairwise=%s\n", (band == WL_60G_BAND)? "GCMP" : "CCMP");
	} else if (nvram_pf_match(prefix, "crypto", "tkip+aes")) {
		fprintf(fp_h, "wpa_key_mgmt=WPA-%s\n", flag_8021x? "EAP" : "PSK");
		fprintf(fp_h, "wpa_strict_rekey=1\n");
		fprintf(fp_h, "eapol_version=2\n");
		fprintf(fp_h, "wpa_pairwise=%s\n", (band == WL_60G_BAND)? "GCMP" : "TKIP CCMP");
	} else {
		warning = 25;
		fprintf(fp_h, "#wpa_pairwise=\n");
	}

	wpapsk = 0;
	if (nvram_pf_match(prefix, "auth_mode_x", "open") &&
	    nvram_pf_match(prefix, "wep_x", "0")) {
		fprintf(fp_h, "wpa=0\n");
	} else if ((nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_invmatch(prefix, "wep_x", "0")) ||
		    nvram_pf_match(prefix, "auth_mode_x", "shared") ||
		    nvram_pf_match(prefix, "auth_mode_x", "radius")) {
		fprintf(fp_h, "wpa=0\n");
	} else if (nvram_pf_match(prefix, "auth_mode_x", "psk") ||
		   nvram_pf_match(prefix, "auth_mode_x", "wpa")) {
		wpapsk = 1;
		fprintf(fp_h, "wpa=1\n");
	} else if (nvram_pf_match(prefix, "auth_mode_x", "psk2") ||
		   nvram_pf_match(prefix, "auth_mode_x", "wpa2")) {
		wpapsk = 2;
		fprintf(fp_h, "wpa=2\n");
	} else if (nvram_pf_match(prefix, "auth_mode_x", "pskpsk2") ||
		   nvram_pf_match(prefix, "auth_mode_x", "wpawpa2")) {
		wpapsk = 3;
		fprintf(fp_h, "wpa=3\n");
	} else {
		warning = 25;
		fprintf(fp_h, "wpa=0\n");
	}

	if (wpapsk != 0 && !flag_8021x) {
		strlcpy(psk, nvram_pf_safe_get(prefix, "wpa_psk"), sizeof(psk));
		if (strlen(psk) == 64)
			fprintf(fp_h, "wpa_psk=%s\n", psk);
		else
			fprintf(fp_h, "wpa_passphrase=%s\n", psk);
	}

	if (!ban) {
		//HT_BW
		bw = get_bw_via_channel(band, nvram_pf_get_int(main_prefix, "channel"));
		if (bw > 0) {
			if (strstr(t_mode, "11ACV") && (bw == 3 || bw == 1))	//80 BW or auto BW
				strlcpy(t_bw, "HT80", sizeof(t_bw));
			else {
				/* 0: 20Mhz
				 * 1: 20/40/80MHz (5G) or 20/40MHz (2G)
				 * 2: 40MHz
				 * 3: 80MHz
				 * 4: 80+80MHz
				 * 5: 160MHz
				 */
				*t_bw = '\0';
				if (strstr(t_mode, "11ACV")) {
					switch (bw) {
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
					strlcpy(t_bw, "HT40", sizeof(t_bw));
					//ext ch
					str = nvram_pf_safe_get(main_prefix, "nctrlsb");
					if (!strcmp(str, "lower"))
						strlcpy(t_ext, "PLUS", sizeof(t_ext));
					else
						strlcpy(t_ext, "MINUS", sizeof(t_ext));

					bw40_channel_check(band, t_ext);
				}
			}
		} else {
			//warning = 34;
			strlcpy(t_bw, "HT20", sizeof(t_bw));
		}
	}

#if 0
	snprintf(mode_cmd, sizeof(mode_cmd), "%s%s%s", t_mode, t_bw, t_ext);
	fprintf(fp_pre, "%s\n", mode_cmd);
	fprintf(fp_pre, "iwpriv %s puren %d\n", wif, puren);
#endif

	if (!subnet) {
		strlcpy(tmpstr, "acs_survey", sizeof(tmpstr));
		if ((val = nvram_pf_get_int(main_prefix, "channel")) != 0)
			snprintf(tmpstr, sizeof(tmpstr), "%d", val);
		fprintf(fp_h, "channel=%s\n", tmpstr);
	}

#if 0 //defined(RTCONFIG_WIFI_QCA9994_QCA9994)
	if (!subnet) {
		if (band == 1) {
			val = nvram_get_int("qca_fc_buf_min");
			if (val >= 64 && val <= 1000)
				fc_buf_min = val;
			fprintf(fp3, "iwpriv %s fc_buf_min %d\n", get_vphyifname(band), fc_buf_min);
		}
		if (nvram_pf_match(prefix, "hwol", "1")) {
			int i;
			char *vphyif = get_vphyifname(band);
			char *fc_buf_max[4] = { "4096", "8192", "12288", "16384" };

			if (strstr(t_mode, "11ACV")) {
				fc_buf_max[0] = "8192";
				fc_buf_max[1] = "16384";
				fc_buf_max[2] = "24576";
				fc_buf_max[3] = "32768";
			}

			for (i = 0; i < 4; ++i)
				fprintf(fp3, "iwpriv %s fc_buf%d_max %s\n", vphyif, i, fc_buf_max[i]);
		}
	}
#endif

	/* AccessPolicy0 */
	if (nvram_pf_invmatch(prefix, "macmode", "disabled")) {
		macmode = nvram_pf_match(prefix, "macmode", "deny")? 1 : 0;	/* 0: accept, 1: deny  */
		fprintf(fp_h, "macaddr_acl=%d\n", macmode? 0 : 1);		/* 0: accept_unless_denied, 1: deny_unless_accepted */
		fprintf(fp_h, "%s_mac_file=%s\n", macmode? "deny" : "accept", mac_path);
		nv = nvp = strdup(nvram_pf_safe_get(prefix, "maclist_x"));
		while (nv && (b = strsep(&nvp, "<")) != NULL) {
			if (*b == '\0')
				continue;
			fprintf(fp_mac, "%s\n", b);
		}
		free(nv);
	}

	if (rep_mode)
		goto next;

	/* Generate WDS configuration script. */
	_gen_nl80211_wds_config(band, subnet, fp_post);

	if (flag_8021x) {
		/* radius server */
		fprintf(fp_h, "auth_server_addr=%s\n", nvram_pf_get(main_prefix, "radius_ipaddr") ? : "169.254.1.1");

		/* radius port */
		v = 1812;
		str = nvram_pf_safe_get(main_prefix, "radius_port");
		if (str && *str != '\0') {
			v = safe_atoi(str);
		} else {
			warning = 50;
		}
		fprintf(fp_h, "auth_server_port=%d\n", v);

		/* radius key */
		fprintf(fp_h, "auth_server_shared_secret=%s\n", nvram_pf_safe_get(main_prefix, "radius_key"));
	}

	/* RadioOn */
	str = nvram_pf_safe_get(main_prefix,"radio");
	if (str && *str != '\0') {
		v = safe_atoi(str);
		/* Always down nl80211 based interface.
		 * 1. It will be up after executing hostapd.
		 * 2. If we didn't down it and auto-channel is selected,
		 *    hostapd may not be able to start due to ACS error.
		 */
		fprintf(fp_post, "ifconfig %s down\n", wif);

		/* Connect to peer WDS AP after VAP up */
		if (!subnet && v && f_exists(wds_path))
			fprintf(fp_post, "%s\n", wds_path);
	}
	/* IGMP */
#if 0
	fprintf(fp_post, "iwpriv %s mcastenhance %d\n", wif, nvram_pf_get_int(main_prefix, "igs") ? 2 : 0);

	i = nvram_pf_get_int(main_prefix,"mrate_x");
next_mrate:
	switch (i++) {
	default:
	case 0:		/* Driver default setting: Disable, means automatic rate instead of fixed rate
				 * Please refer to #ifdef MCAST_RATE_SPECIFIC section in
				 * file linuxxxx/drivers/net/wireless/rtxxxx/common/mlme.c
				 */
		break;
	case 1:		/* Legacy CCK 1Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 1000\n", wif);
		mcast_phy = 1;
		break;
	case 2:		/* Legacy CCK 2Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 2000\n", wif);
		mcast_phy = 1;
		break;
	case 3:		/* Legacy CCK 5.5Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 5500\n", wif);
		mcast_phy = 1;
		break;
	case 4:		/* Legacy OFDM 6Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 6000\n", wif);
		break;
	case 5:		/* Legacy OFDM 9Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 9000\n", wif);
		break;
	case 6:		/* Legacy CCK 11Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 11000\n", wif);
		mcast_phy = 1;
		break;
	case 7:		/* Legacy OFDM 12Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 12000\n", wif);
		break;
	case 8:		/* Legacy OFDM 18Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 18000\n", wif);
		break;
	case 9:		/* Legacy OFDM 24Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 24000\n", wif);
		break;
	case 10:		/* Legacy OFDM 36Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 36000\n", wif);
		break;
	case 11:		/* Legacy OFDM 48Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 48000\n", wif);
		break;
	case 12:		/* Legacy OFDM 54Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 54000\n", wif);
		break;
	case 13:		/* HTMIX 130/300Mbps 2S */
		fprintf(fp_post, "iwpriv %s mcast_rate 300000\n", wif);
		break;
	case 14:		/* HTMIX 6.5/15Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 15000\n", wif);
		break;
	case 15:		/* HTMIX 13/30Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 30000\n", wif);
		break;
	case 16:		/* HTMIX 19.5/45Mbps */
		fprintf(fp_post, "iwpriv %s mcast_rate 45000\n", wif);
		break;
	case 17:		/* HTMIX 13/30Mbps 2S */
		fprintf(fp_post, "iwpriv %s mcast_rate 30000\n", wif);
		break;
	case 18:		/* HTMIX 26/60Mbps 2S */
		fprintf(fp_post, "iwpriv %s mcast_rate 60000\n", wif);
		break;
	case 19:		/* HTMIX 39/90Mbps 2S */
		fprintf(fp_post, "iwpriv %s mcast_rate 90000\n", wif);
		break;
	case 20:
		/* Choose multicast rate base on mode, encryption type, and IPv6 is enabled or not. */
		__choose_mrate(main_prefix, &mcast_phy, &mcast_mcs, &rate);
		fprintf(fp_post, "iwpriv %s mcast_rate %d\n", wif, rate);
		break;
	}
	/* No CCK for 5Ghz band */
	if (band && mcast_phy == 1)
		goto next_mrate;
#endif

	gen_hostapd_wps_config(fp, band, subnet, br_if);


next:
	fclose(fp_h);
	fclose(fp_mac);
	fclose(fp_post);
	fclose(fp_pre);
	chmod(postwifi_path, 0777);	/* postwifi_athX.sh */
	chmod(prewifi_path, 0777);	/* prewifi_athX.sh */
	chmod(wds_path, 0777);		/* nawds_athX.sh */

	(void)warning;
	return 0;
}
#endif	/* RTCONFIG_WIGIG */

/************************ CONSTANTS & MACROS ************************/

/*
 * Constants fof WE-9->15
 */
#define IW15_MAX_FREQUENCIES	16
#define IW15_MAX_BITRATES	8
#define IW15_MAX_TXPOWER	8
#define IW15_MAX_ENCODING_SIZES	8
#define IW15_MAX_SPY		8
#define IW15_MAX_AP		8

/****************************** TYPES ******************************/

/*
 *	Struct iw_range up to WE-15
 */
struct iw15_range {
	__u32 throughput;
	__u32 min_nwid;
	__u32 max_nwid;
	__u16 num_channels;
	__u8 num_frequency;
	struct iw_freq freq[IW15_MAX_FREQUENCIES];
	__s32 sensitivity;
	struct iw_quality max_qual;
	__u8 num_bitrates;
	__s32 bitrate[IW15_MAX_BITRATES];
	__s32 min_rts;
	__s32 max_rts;
	__s32 min_frag;
	__s32 max_frag;
	__s32 min_pmp;
	__s32 max_pmp;
	__s32 min_pmt;
	__s32 max_pmt;
	__u16 pmp_flags;
	__u16 pmt_flags;
	__u16 pm_capa;
	__u16 encoding_size[IW15_MAX_ENCODING_SIZES];
	__u8 num_encoding_sizes;
	__u8 max_encoding_tokens;
	__u16 txpower_capa;
	__u8 num_txpower;
	__s32 txpower[IW15_MAX_TXPOWER];
	__u8 we_version_compiled;
	__u8 we_version_source;
	__u16 retry_capa;
	__u16 retry_flags;
	__u16 r_time_flags;
	__s32 min_retry;
	__s32 max_retry;
	__s32 min_r_time;
	__s32 max_r_time;
	struct iw_quality avg_qual;
};

/*
 * Union for all the versions of iwrange.
 * Fortunately, I mostly only add fields at the end, and big-bang
 * reorganisations are few.
 */
union iw_range_raw {
	struct iw15_range range15;	/* WE 9->15 */
	struct iw_range range;	/* WE 16->current */
};

/*
 * Offsets in iw_range struct
 */
#define iwr15_off(f)	( ((char *) &(((struct iw15_range *) NULL)->f)) - \
			  (char *) NULL)
#define iwr_off(f)	( ((char *) &(((struct iw_range *) NULL)->f)) - \
			  (char *) NULL)

/* Disable runtime version warning in ralink_get_range_info() */
int iw_ignore_version_sp = 0;

/*
int getSiteSurvey(int band, char *ofile)
=> TBD. implement it if we want to support media bridge or repeater mode
*/

int __need_to_start_wps_band(char *prefix)
{
	char *p, tmp[128];

	if (!prefix || *prefix == '\0')
		return 0;

	p = nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp));
	if ((!strcmp(p, "open")
	     && !nvram_match(strcat_r(prefix, "wep_x", tmp), "0"))
	    || !strcmp(p, "shared") || !strcmp(p, "psk") || !strcmp(p, "wpa")
	    || !strcmp(p, "wpa2") || !strcmp(p, "wpawpa2")
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	    || !strcmp(p, "wpawpa2wpa3") || !strcmp(p, "wpa2wpa3") || !strcmp(p, "wpa3")
#endif
	    || !strcmp(p, "radius")
	    || nvram_match(strcat_r(prefix, "radio", tmp), "0")
		|| nvram_match(strcat_r(prefix, "closed", tmp), "1")
//	    || !((sw_mode() == SW_MODE_ROUTER)
//		|| (sw_mode() == SW_MODE_AP))
		)
		return 0;

	return 1;
}

int need_to_start_wps_band(int wps_band)
{
	int ret = 1;
	char prefix[] = "wlXXXXXXXXXX_";

	switch (wps_band) {
#if defined(RTCONFIG_HAS_5G_2)
	case WL_5G_2_BAND:
#endif
#if defined(RTCONFIG_WIFI6E)
	case WL_6G_BAND:
#endif
#if defined(RTCONFIG_WIGIG)
	case WL_60G_BAND:
#endif
	case WL_2G_BAND:
	case WL_5G_BAND:
		snprintf(prefix, sizeof(prefix), "wl%d_", wps_band);
		ret = __need_to_start_wps_band(prefix);
		break;
	default:
		ret = 0;
	}

	return ret;
}

int wps_pin(int pincode)
{
	int i;
	char word[256], *next, ifnames[128];
	int wps_band = nvram_get_int("wps_band_x"), multiband = get_wps_multiband();

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;

		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);

		if (!need_to_start_wps_band(i)) {
			++i;
			continue;
		}
//              dbg("WPS: PIN\n");

		if (pincode == 0) {
			;
		} else {
			doSystem("hostapd_cli -i%s wps_pin any %08d", get_wifname(i), pincode);
		}

		++i;
	}

	return 0;
}

static int __wps_pbc(int multiband)
{
	int i;
	char word[256], *next, ifnames[128];
	int wps_band = nvram_get_int("wps_band_x");

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;

		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);

		if (!need_to_start_wps_band(i)) {
			++i;
			continue;
		}
//              dbg("WPS: PBC\n");
		g_isEnrollee[i] = 1;
		eval("hostapd_cli", "-i", (char*)get_wifname(i), "wps_pbc");

		++i;
	}

	return 0;
}

int wps_pbc(void)
{
	return __wps_pbc(get_wps_multiband());
}

int wps_pbc_both(void)
{
#if defined(RTCONFIG_WPSMULTIBAND)
	return __wps_pbc(1);
#endif
}

extern void wl_default_wps(int unit);

void __wps_oob(const int multiband)
{
	int i, wps_band = nvram_get_int("wps_band_x");
	char word[256], *next;
	char ifnames[128];

	if (nvram_match("lan_ipaddr", ""))
		return;

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;

		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);

		nvram_set("w_Setting", "0");
		wl_default_wps(i);

		qca_wif_up(word);
		g_isEnrollee[i] = 0;

		++i;
	}

#ifdef RTCONFIG_TCODE
	restore_defaults_wifi(0);
#endif
	nvram_commit();

	gen_qca_wifi_cfgs();
}

void wps_oob(void)
{
	__wps_oob(get_wps_multiband());
}

void wps_oob_both(void)
{
#if defined(RTCONFIG_WPSMULTIBAND)
	__wps_oob(1);
#else
	wps_oob();
#endif /* RTCONFIG_WPSMULTIBAND */
}

#ifdef RTCONFIG_CONCURRENTREPEATER
void start_wps_cli(void)
{
	stop_wps_cli();

	int wlc_express = nvram_get_int("wlc_express");

	if (wlc_express < 0 || wlc_express > 2)
		wlc_express = 0;

	nvram_set_int("led_status", LED_WPS_SCANNING);

	nvram_set("wps_cli_state", "1");

	if (wlc_express == 0 || wlc_express == 1)
		eval("wpa_cli", "-p", "/var/run/wpa_supplicant-sta0", "-i", "sta0", "wps_pbc");
	
	if (wlc_express == 0 || wlc_express == 2)
		eval("wpa_cli", "-p", "/var/run/wpa_supplicant-sta1", "-i", "sta1", "wps_pbc");
}

void stop_wps_cli(void)
{
	eval("wpa_cli", "-p", "/var/run/wpa_supplicant-sta0", "-i", "sta0", "wps_cancel");
	eval("wpa_cli", "-p", "/var/run/wpa_supplicant-sta1", "-i", "sta1", "wps_cancel");
	nvram_set("wps_cli_state", "0");
}

#endif

void start_wsc(void)
{
	int i;
	char *wps_sta_pin = nvram_safe_get("wps_sta_pin");
	char word[256], *next, ifnames[128];
	int wps_band = nvram_get_int("wps_band_x"), multiband = get_wps_multiband();

	if (nvram_match("lan_ipaddr", ""))
		return;

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
#ifdef RTCONFIG_AMAS
		if (i != 0)
			continue;	/* allow only band 0 when AiMesh setup is running */
#endif
#ifdef RTCONFIG_WIFI_SON
#ifndef RTCONFIG_DUAL_BACKHAUL
		if(sw_mode() != SW_MODE_REPEATER && i==0)
		{	
			i++;
			continue;
		}
#endif
#endif
		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);

		if (!need_to_start_wps_band(i)) {
			++i;
			continue;
		}

		dbg("%s: start wsc(%d)\n", __func__, i);
		doSystem("hostapd_cli -i%s wps_cancel", get_wifname(i));	// WPS disabled

		if (strlen(wps_sta_pin) && strcmp(wps_sta_pin, "00000000")
		    && (wl_wpsPincheck(wps_sta_pin) == 0)) {
			dbg("WPS: PIN\n");	// PIN method
			g_isEnrollee[i] = 0;
			doSystem("hostapd_cli -i%s wps_pin any %s", get_wifname(i), wps_sta_pin);
		} else {
			dbg("WPS: PBC\n");	// PBC method
			g_isEnrollee[i] = 1;
			eval("hostapd_cli", "-i", (char*)get_wifname(i), "wps_pbc");
		}

		++i;
	}
#ifdef RTCONFIG_CONCURRENTREPEATER
	nvram_set_int("led_status", LED_WPS_SCANNING);
#endif
}

static void __stop_wsc(int multiband)
{
	int i;
	char word[256], *next, ifnames[128];
	int wps_band = nvram_get_int("wps_band_x");

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;

		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);
		if (!need_to_start_wps_band(i)) {
			++i;
			continue;
		}

		doSystem("hostapd_cli -i%s wps_cancel", get_wifname(i));	// WPS disabled

		++i;
	}
}

void stop_wsc(void)
{
	__stop_wsc(get_wps_multiband());
}

void stop_wsc_both(void)
{
#if defined(RTCONFIG_WPSMULTIBAND)
	__stop_wsc(1);
#endif
}

int create_tmp_sta(int unit, char *sta, char *ssid_prefix)
{
	if(is_intf_up(sta) < 0)
		return create_tmp_sta_part(unit, sta, ssid_prefix);
	return 0;
}

void destroy_tmp_sta(char *sta)
{
	char fpath[64];

	snprintf(fpath, sizeof(fpath), "/var/run/wifi-%s.pid", sta);
	kill_pidfile_tk(fpath);
	unlink(fpath);
	snprintf(fpath, sizeof(fpath), "/etc/Wireless/conf/wpa_supplicant-%s.conf", sta);
	unlink(fpath);

#ifdef RTCONFIG_WIFI_SON
	if(nvram_get_int("x_Setting") && (sw_mode()==SW_MODE_AP && !nvram_match("cfg_master", "1")))
		_dprintf("==>wps enrollee: do not destroy original %s vap\n",sta);
	else
#endif
	{
		_dprintf("==>wps enrollee: destroy %s vap\n",sta);
		ifconfig(sta, 0, NULL, NULL);
		//doSystem("wlanconfig %s destroy", sta);
		destroy_vap(sta);
	}
}

#ifdef RTCONFIG_WPS_ENROLLEE
void start_wsc_enrollee_band(int band)
{
	char sta[64];

	dbg("%s: start wsc enrollee(%d)\n", __func__, band);
	strcpy(sta, get_staifname(band));
	if (sw_mode() == SW_MODE_ROUTER
			|| sw_mode() == SW_MODE_AP) {
		create_tmp_sta(band, sta, NULL);
	}

	set_wpa_cli_cmd(band, "wps_pbc", NULL);
}

void start_wsc_enrollee(void)
{
	int i;
	char word[256], *next, ifnames[128];

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);

#if defined(MAPAC2200) 
		if(i>=2)        //only need sta0 & sta1
                       	break;
#endif
#ifdef RTCONFIG_WIFI_SON
#ifndef RTCONFIG_DUAL_BACKHAUL
		if(sw_mode() != SW_MODE_REPEATER && i==0)
		{	
			i++;
			continue;
		}
#endif
#endif
		start_wsc_enrollee_band(i);
		i++;
	}
}

void stop_wsc_enrollee_band(int band)
{
	char sta[64];
	strlcpy(sta, get_staifname(band), sizeof(sta));
	set_wpa_cli_cmd(band, "wps_cancel", NULL);

	if (sw_mode() == SW_MODE_ROUTER
			|| sw_mode() == SW_MODE_AP) {
		destroy_tmp_sta(sta);
	}
}

void stop_wsc_enrollee(void)
{
	int i;
	char word[256], *next, ifnames[128];

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);
#if defined(MAPAC2200) 
                if(i>=2)        //only need sta0 & sta1
                        break;
#endif

#ifdef RTCONFIG_WIFI_SON
#ifndef RTCONFIG_DUAL_BACKHAUL
		if(sw_mode() != SW_MODE_REPEATER && i==0)
		{	
			i++;
			continue;
		}
#endif
#endif

		stop_wsc_enrollee_band(i);
		i++;
	}
}

int wps_ascii_to_char_with_utf8(const char *output, const char *input, int outsize)
{
	char dest[28];
	char *instr = input;
	char *word;
	char *end = output + outsize - 1;

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

int getting_wps_enroll_conf(int unit, char *ssid, char *psk)
{
	char buf[512];
	FILE *fp;
	int len;
	char *pt1, *pt2;

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
			if (pt1) {
				pt2 = pt1 + strlen("ssid=\"");
				pt1 = strstr(pt2, "\"");
				if (pt1) {
					*pt1 = '\0';
					chomp(pt2);
					strcpy(ssid, pt2);
				}
			}else{
				pt1 = strstr(buf, "ssid=");
				if (pt1) {
					pt2 = pt1 + strlen("ssid=");
					pt1 = strchr(pt2, '\n');
					if (pt1) {
						*pt1 = '\0';
						chomp(pt2);
						wps_ascii_to_char_with_utf8(ssid, pt2, 33);
					}
				}
			}
			//PSK
			pt2 = pt1 + 1;
			pt1 = strstr(pt2, "psk=\"");
			if (pt1) {	//WPA-PSK
				pt2 = pt1 + strlen("psk=\"");
				pt1 = strstr(pt2, "\"");
				if (pt1) {
					*pt1 = '\0';
					chomp(pt2);
					strcpy(psk, pt2);
				}
			}
			else {		//OPEN
			}
			return 0;
		}
	}
	return -1;
}

#if defined(RTCONFIG_AMAS)
int getting_wps_result(int unit, char *bssid, char *ssid, char *key_mgmt) {
	char buf[512];
	FILE *fp;
	int len;
	char *pt1, *pt2;
	char ctrl_sk[32];

	get_wpa_ctrl_sk(unit, ctrl_sk, sizeof(ctrl_sk));
	sprintf(buf, "wpa_cli -p %s -i%s status", ctrl_sk, get_staifname(unit));
	fp = PS_popen(buf, "r");
	if (fp) {
		len = fread(buf, 1, sizeof(buf), fp);
		PS_pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';

			// ap_mac
			pt1 = strstr(buf, "bssid=");
			if (pt1) {
				pt2 = pt1 + strlen("bssid=");
				pt1 = strstr(pt2, "freq=");
				if (pt1 && bssid) {
					*pt1 = '\0';
					chomp(pt2);
					strcpy(bssid, pt2);
				}
			}

			// ssid
			pt2 = pt1 + 1;
			pt1 = strstr(pt2, "ssid=");
			if (pt1) {
				pt2 = pt1 + strlen("ssid=");
				pt1 = strstr(pt2, "id=");
				if (pt1 && ssid) {
					*pt1 = '\0';
					chomp(pt2);
					strcpy(ssid, pt2);
				}
			}

			// auth_mode and crypto
			pt2 = pt1 + 1;
			pt1 = strstr(pt2, "key_mgmt=");
			if (pt1) {
				pt2 = pt1 + strlen("key_mgmt=");
				pt1 = strstr(pt2, "wpa_state=");
				if (pt1 && key_mgmt) {
					*pt1 = '\0';
					chomp(pt2);
					strcpy(key_mgmt, pt2);
				}
			}
		return 0;
		}
	}
	return -1;
}

void amas_save_wifi_para(void)
{
	char psk[65];
	char ssid[33];
	char key_mgmt[16];
	char tmp[128], prefix[] = "wlcXXXXXXXXX_", prefix2[] = "wlcXXXXXXXXX_";
	char word[256], *next;
	char ifnames[128];
	int i;

	if(getting_wps_enroll_conf(0, ssid, psk) != 0)
		return;
	if(getting_wps_result(0, NULL, NULL, key_mgmt) != 0)
		return;

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		snprintf(prefix, sizeof(prefix), "wlc%d_", i);
		snprintf(prefix2, sizeof(prefix2), "wl%d.1_", i);
		//ssid
		if(ssid[0] != '\0')
			nvram_set(strcat_r(prefix, "ssid", tmp), ssid);
		//auth_mode and crypto
		if(psk[0] != '\0') {
			if(strncmp(key_mgmt, "WPA-", 4) == 0) {
				nvram_set(strcat_r(prefix, "auth_mode", tmp), "psk");
				nvram_set(strcat_r(prefix2, "auth_mode_x", tmp), "psk");
				nvram_set(strcat_r(prefix, "crypto", tmp), "aes+tkip");
				nvram_set(strcat_r(prefix2, "crypto", tmp), "aes+tkip");
			}
			else {
				nvram_set(strcat_r(prefix, "auth_mode", tmp), "psk2");
				nvram_set(strcat_r(prefix2, "auth_mode_x", tmp), "psk2");
				nvram_set(strcat_r(prefix, "crypto", tmp), "aes");
				nvram_set(strcat_r(prefix2, "crypto", tmp), "aes");
			}
			nvram_set(strcat_r(prefix2, "wpa_psk", tmp), psk);
		}
		else {
			nvram_set(strcat_r(prefix, "auth_mode", tmp), "open");
			nvram_set(strcat_r(prefix2, "auth_mode_x", tmp), "open");
			nvram_set(strcat_r(prefix, "wep", tmp), "0");
			nvram_set(strcat_r(prefix2, "wep_x", tmp), "0");
		}
		++i;
	}

	nvram_set("obd_Setting", "1");
	nvram_commit();
}
#endif	/* RTCONFIG_AMAS */

#if defined(RTCONFIG_WIFI_CLONE)
void wifi_clone(int unit)
{
	char psk[65];
	char ssid[33];
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";
#ifdef RTCONFIG_WIFI_SON
	char stmp[128], sprefix[] = "wlXXXXXXXXXX_";
#ifndef RTCONFIG_DUAL_BACKHAUL
	if(unit==0)
	{
		_dprintf("RE: do not use sta0!\n");
		return;
	}
#endif
#endif

	if(getting_wps_enroll_conf(unit, ssid, psk) == 0) {
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		if(ssid[0] != '\0')
			nvram_set(strcat_r(prefix, "ssid", tmp), ssid);
		if(psk[0] != '\0') {
			nvram_set(strcat_r(prefix, "crypto", tmp), "aes");
			nvram_set(strcat_r(prefix, "auth_mode_x", tmp), "psk2");
			nvram_set(strcat_r(prefix, "wpa_psk", tmp), psk);
		}
		else {
			nvram_set(strcat_r(prefix, "auth_mode_x", tmp), "open");
			nvram_set(strcat_r(prefix, "wep_x", tmp), "0");
		}
	}
#ifdef RTCONFIG_WIFI_SON
	if(sw_mode() != SW_MODE_REPEATER) {
		snprintf(sprefix, sizeof(sprefix), "wl%d_", unit);
		snprintf(prefix, sizeof(prefix), "wl%d_", unit?0:1);
		nvram_set(strcat_r(prefix, "ssid", tmp), nvram_get(strcat_r(sprefix, "ssid", stmp)));
		nvram_set(strcat_r(prefix, "wpa_psk", tmp), nvram_get(strcat_r(sprefix, "wpa_psk", stmp)));
		nvram_set(strcat_r(prefix, "crypto", tmp), nvram_get(strcat_r(sprefix, "crypto", stmp)));
		nvram_set(strcat_r(prefix, "auth_mode_x", tmp), nvram_get(strcat_r(sprefix, "auth_mode_x", stmp)));
		_dprintf("=> wifi clone ok! duplicate %s wifi-setting to %s\n",unit?"5G":"2G",unit?"2G":"5G");
#if defined(MAPAC2200)
		duplicate_5g2();
#endif
	}
#endif

	nvram_set("x_Setting", "1");
	nvram_commit();
}
#endif

char *getWscStatus_enrollee(int unit)
{
	char buf[512];
	FILE *fp;
	int len;
	char *pt1, *pt2;
	char ctrl_sk[32];

	get_wpa_ctrl_sk(unit, ctrl_sk, sizeof(ctrl_sk));
	snprintf(buf, sizeof(buf), "wpa_cli -p %s -i %s status", ctrl_sk, get_staifname(unit));
	fp = PS_popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		PS_pclose(fp);
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

char *getWscStatus(int unit, char *buf, int buflen)
{
	char cmd[512];
	FILE *fp;
	int len;
	char *pt1,*pt2;

	snprintf(cmd, sizeof(cmd), "hostapd_cli -i%s wps_get_status", get_wififname(unit));
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

void wsc_user_commit(void)
{
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
				p += snprintf(p, size - (p - buf), "%d,",
					      (x << 3) + y);
			}
		}
	}
}


void ate_commit_bootlog(char *err_code)
{
	unsigned char fail_buffer[OFFSET_SERIAL_NUMBER - OFFSET_FAIL_RET];

#if defined(RTCONFIG_QCA) && defined(RTCONFIG_CONCURRENTREPEATER)
	stop_led_monitor();
#endif

	nvram_set("Ate_power_on_off_enable", err_code);
	nvram_commit();

	memset(fail_buffer, 0, sizeof(fail_buffer));
	strncpy(fail_buffer, err_code,
		OFFSET_FAIL_BOOT_LOG - OFFSET_FAIL_RET - 1);
	Gen_fail_log(nvram_get("Ate_reboot_log"),
		     nvram_get_int("Ate_boot_check"),
		     (struct FAIL_LOG *)&fail_buffer[OFFSET_FAIL_BOOT_LOG -
						     OFFSET_FAIL_RET]);
	Gen_fail_log(nvram_get("Ate_dev_log"), nvram_get_int("Ate_boot_check"),
		     (struct FAIL_LOG *)&fail_buffer[OFFSET_FAIL_DEV_LOG -
						     OFFSET_FAIL_RET]);

	FWrite(fail_buffer, OFFSET_FAIL_RET, sizeof(fail_buffer));
}

#ifdef RTCONFIG_USER_LOW_RSSI
typedef struct _WLANCONFIG_LIST {
         char addr[18];
         unsigned int aid;
         unsigned int chan;
         char txrate[6];
         char rxrate[6];
         unsigned int rssi;
         unsigned int idle;
         unsigned int txseq;
         unsigned int rcseq;
         char caps[12];
         char acaps[10];
         char erp[7];
         char state_maxrate[20];
         char wps[4];
         char rsn[4];
         char wme[4];
         char mode[31];
} WLANCONFIG_LIST;

void rssi_check_unit(int unit)
{
	#define STA_LOW_RSSI_PATH "/tmp/low_rssi"
	int rssi_th, channf;
	FILE *fp;
	char line_buf[300],cmd[300],tmp[128],wif[8]; // max 14x
	char prefix[sizeof("wlXXXXXXXXXX_")];
	WLANCONFIG_LIST *result;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	if (!(rssi_th= nvram_get_int(strcat_r(prefix, "user_rssi", tmp))))
		return;

	result=malloc(sizeof(WLANCONFIG_LIST));
	memset(result, 0, sizeof(WLANCONFIG_LIST));
	__get_wlifname(unit, 0, wif);
	channf = QCA_DEFAULT_NOISE_FLOOR;
	doSystem("wlanconfig %s list > %s", wif, STA_LOW_RSSI_PATH);
	if (!(fp = fopen(STA_LOW_RSSI_PATH, "r")))
		return;
	fgets(line_buf, sizeof(line_buf), fp); // ignore header
	while ( fgets(line_buf, sizeof(line_buf), fp) ) {
		sscanf(line_buf, "%s%u%u%s%s%u%u%u%u%s%s%s%s%s%s%s%s",
					result->addr,
					&result->aid,
					&result->chan,
					result->txrate,
					result->rxrate,
					&result->rssi,
					&result->idle,
					&result->txseq,
					&result->rcseq,
					result->caps,
					result->acaps,
					result->erp,
					result->state_maxrate,
					result->wps,
					result->rsn,
					result->wme,
					result->mode);

		if (rssi_th <= (result->rssi + channf)) {
			snprintf(cmd, sizeof(cmd), IWPRIV " %s kickmac %s", wif, result->addr);
			doSystem(cmd);
			dbg("=====>Roaming with %s:Disconnect Station: %s  RSSI: %d\n",
				wif, result->addr, (result->rssi + channf));
		}
	}
	free(result);
	fclose(fp);
	unlink(STA_LOW_RSSI_PATH);
}
#endif

void platform_start_ate_mode(void)
{
	int model = get_model();

	switch (model) {
#if defined(RTAC58U) || defined(RT4GAC53U) || defined(RTAC82U) || defined(RTAC95U)
	case MODEL_RTAC58U:
	case MODEL_RT4GAC53U:
	case MODEL_RTAC82U:
	case MODEL_RTAC95U:
#ifndef RTCONFIG_ATEUSB3_FORCE
		// this way is unstable
		if(nvram_get_int("usb_usb3") == 0) {
			eval("ejusb", "-1");
			modprobe_r(USBOHCI_MOD);
			modprobe_r(USB20_MOD);
#ifdef USB30_MOD
#if defined(RTCONFIG_SOC_IPQ8064)
			modprobe_r("dwc3-ipq");
			modprobe_r("udc-core");
#endif
			modprobe_r(USB30_MOD);
#endif
			nvram_set("xhci_ports", "");
#if defined(RTCONFIG_M2_SSD)
			nvram_set("ehci_ports", "3-1 1-1 ata1");
#else
			nvram_set("ehci_ports", "3-1 1-1");
#endif
			nvram_set("ohci_ports", "4-1 1-1");
			modprobe(USB20_MOD);
			modprobe(USBOHCI_MOD);
#ifdef USB30_MOD
			modprobe(USB30_MOD, "u3intf=1");
#if defined(RTCONFIG_SOC_IPQ8064)
			modprobe("udc-core");
			modprobe("dwc3-ipq");
#endif
#endif
		}
#endif
		break;
#endif
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
	int retry, ssv_ok;
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
int
getSiteSurvey(int band,char* ofile)
{
   	int apCount=0;
	char header[128];
	FILE *fp,*ofp;
	char buf[target][200],set_flag[target];
	int i, ssv_ok = 0, radio, is_sta = 0, bitrate;
	char *pt1,*pt2;
	char a1[10],a2[10];
	char ssid_str[256], ssv_if[10];
	char ch[4] = "", ssid[33] = "", address[18] = "", enc[9] = "";
	char auth[16] = "", sig[9] = "", wmode[8] = "";
	int  lock;
#if defined(RTCONFIG_WIRELESSREPEATER)
	char ure_mac[18];
	int wl_authorized = 0;
#endif
#ifdef RTCONFIG_WIFI_SON
	int j,mac1=0,mac2=0,cmp=0;
	char address2[18]="",tm[5]="";
	char tmpnv[30],tmps[5];
	int sig_tmp,sig_bk,siglv_tmp,siglv_bk;
	char siglv[5]="",a3[10],*pt3,*pt4;
	char addr_tmp[18]="";
#endif
	int is_ready, wlc_band = -1;
	char temp1[200];
	char prefix_header[]="Cell xx - Address:";
#if defined(RTCONFIG_AMAS)
	int start_ldb = 0;
#endif
	dbG("site survey...\n");
	if (band < 0 || band >= MAX_NR_WL_IF)
		return 0;

#ifdef RTCONFIG_WIFI_SON
	if(sw_mode() != SW_MODE_REPEATER) {
		sig_tmp=0;
		sig_bk=0;
		memset(addr_tmp,0,sizeof(addr_tmp));
		goto skip_init;
	}
#endif
#if defined(RTCONFIG_WIRELESSREPEATER)
	if (nvram_get("wlc_band") && (repeater_mode() || mediabridge_mode()))
		wlc_band = nvram_get_int("wlc_band");
#endif

#if defined(RTCONFIG_AMAS)
	if(nvram_match("smart_connect_x", "1") && pids("lbd")){
		stop_qca_lbd();
		start_ldb = 1;
	}
#endif
	/* Router/access-point/repeater mode
	 * 1. If VAP i/f is UP, radio on, use VAP i/f to site-survey.
	 * 2. If VAP i/f is DOWN, radio off, create new STA i/f to site-survey.
	 * Media-bridge mode
	 * 1. Always use STA i/f to site-survey, VAP i/f may not work in this mode.
	 *    e.g. Cascade WiFi driver.
	 * 2. If a WiFi band is not used to connect to parent-AP, create new one
	 *    temporarilly, site-survey, and destroy it.
	 */
	__get_wlifname(band, 0, ssv_if);
	radio = get_radio_status(ssv_if);
	bitrate = get_bitrate(ssv_if);
	if (!radio || bitrate || (mediabridge_mode() && band != wlc_band))
		strcpy(ssv_if, get_staifname(band));
	if (!strncmp(ssv_if, "sta", 3))
		is_sta = 1;

	lock = file_lock("sitesurvey");
	if (band != wlc_band && is_sta) {
		eval("wlanconfig", ssv_if, "create", "wlandev", get_vphyifname(band), "wlanmode", "sta", "nosbeacon");
		ifconfig(ssv_if, IFUP, NULL, NULL);
	}
	ssv_ok = do_sitesurvey(ssv_if);

	if (band != wlc_band && is_sta) {
		ifconfig(ssv_if, 0, NULL, NULL);
		eval("wlanconfig", ssv_if, "destroy");
	}
	if (ssv_ok <= 0 && !radio && is_sta) {
		__get_wlifname(band, 0, ssv_if);
		ifconfig(ssv_if, IFUP, NULL, NULL);
		ssv_ok = do_sitesurvey(ssv_if);
		ifconfig(ssv_if, 0, NULL, NULL);
	}

#ifdef RTCONFIG_WIFI_SON
skip_init:
	if(sw_mode() != SW_MODE_REPEATER) {
		lock = file_lock("sitesurvey");
		strcpy(ssv_if, get_staifname(band));
		ifconfig(ssv_if, IFUP, NULL, NULL);
		ssv_ok = do_sitesurvey(ssv_if);
	}
#endif
	file_unlock(lock);

#if defined(RTCONFIG_AMAS)
	if(start_ldb)
		start_qca_lbd();
#endif

	if (!(fp= fopen(APSCAN_WLIST, "r")))
		return 0;
	
	snprintf(header, sizeof(header), "%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode");

#ifndef RTCONFIG_WIFI_SON
	if(sw_mode() != SW_MODE_REPEATER)
		dbg("\n%s", header);
#endif
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
#if defined(RTCONFIG_CONCURRENTREPEATER)
  		if(feof(fp)) {
			if(!is_ready)
		   		break;
		}
#endif

#ifndef RTCONFIG_WIFI_SON
		if(sw_mode() != SW_MODE_REPEATER)
			dbg("\napCount=%d\n",apCount);
#endif
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
				strlcpy(auth, "WPA2-", sizeof(auth));
			else if(strstr(pt1+strlen("IE:"),"WPA")!=NULL) 
				strlcpy(auth, "WPA-", sizeof(auth));
		
			pt2=strstr(buf[6],"Authentication Suites");
			if(pt2)
	  		{
				if(strstr(pt2+strlen("Authentication Suites"),"SAE")!=NULL)
			   		strcat(auth,"WPA3-Personal");
				else if(strstr(pt2+strlen("Authentication Suites"),"PSK")!=NULL)
			   		strcat(auth,"Personal");
				else if(strstr(pt2+strlen("Authentication Suites"),"802.1x")!=NULL)
				   	strcat(auth,"Enterprise");
				else
					_dprintf("%s:%d ERROR!! NO AKM TYPE Set!!\n", __func__, __LINE__);
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
			sprintf(sig,"%d",100*(atoi(a1)+6)/(atoi(a2)+6));
#ifdef RTCONFIG_WIFI_SON
			if(sw_mode() != SW_MODE_REPEATER) {
				memset(siglv,0,sizeof(siglv));
				memset(a3,0,sizeof(a3));
				pt3=strstr(pt2,"Signal level=-");
				if(pt3)
				{
					pt4=strstr(pt3," dBm");
					if(pt3 && pt4)
					{
						strncpy(a3,pt3+strlen("Signal level=-"),pt4-pt3-strlen("Signal level=-"));
						sprintf(siglv,"%s",a3);
					}	
				}
			}
#endif
		}   

		//wmode
		memset(wmode,0,sizeof(wmode));
		pt1=strstr(buf[8],"phy_mode=");
		if(pt1)
		{   

			if((pt2=strstr(pt1+strlen("phy_mode="),"IEEE80211_MODE_11AC_VHT"))!=NULL)
				strlcpy(wmode, "ac", sizeof(wmode));
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
			else if((pt2=strstr(pt1+strlen("phy_mode="),"IEEE80211_MODE_11AXA_HE"))!=NULL
					|| (pt2=strstr(pt1+strlen("phy_mode="),"IEEE80211_MODE_11AXG_HE"))!=NULL)
				strlcpy(wmode, "ax", sizeof(wmode));
#endif
			else if((pt2=strstr(pt1+strlen("phy_mode="),"IEEE80211_MODE_11A"))!=NULL
			        || (pt2=strstr(pt1+strlen("phy_mode="),"IEEE80211_MODE_TURBO_A"))!=NULL)
				strlcpy(wmode, "a", sizeof(wmode));
			else if((pt2=strstr(pt1+strlen("phy_mode="),"IEEE80211_MODE_11B"))!=NULL)
				strlcpy(wmode, "b", sizeof(wmode));
			else if((pt2=strstr(pt1+strlen("phy_mode="),"IEEE80211_MODE_11G"))!=NULL
			        || (pt2=strstr(pt1+strlen("phy_mode="),"IEEE80211_MODE_TURBO_G"))!=NULL)
				strlcpy(wmode, "bg", sizeof(wmode));
			else if((pt2=strstr(pt1+strlen("phy_mode="),"IEEE80211_MODE_11NA"))!=NULL)
				strlcpy(wmode, "an", sizeof(wmode));
			else if(strstr(pt1+strlen("phy_mode="),"IEEE80211_MODE_11NG"))
				strlcpy(wmode, "bgn", sizeof(wmode));
		}
		else
			strlcpy(wmode, "unknown", sizeof(wmode));

#ifndef RTCONFIG_WIFI_SON
		dbg("%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n",ch,ssid,address,enc,auth,sig,wmode);
#endif
//////
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


#ifdef RTCONFIG_WIFI_SON
		if(nvram_get_int("x_Setting") && (sw_mode()==SW_MODE_AP && !nvram_match("cfg_master", "1"))) //RE
		{
			if(band)
			{
				memset(tmpnv,0,sizeof(tmpnv));
				sprintf(tmpnv,"wl%d_ssid",band);
				if(!strcmp(nvram_safe_get(tmpnv), ssid))
				{
					if(atoi(sig)>sig_tmp)
					{
						siglv_bk=siglv_tmp;
						sig_bk=sig_tmp;
						nvram_set("sta_bssid_bk",addr_tmp);
						sig_tmp=atoi(sig);
						siglv_tmp=atoi(siglv);
						sprintf(tmps,"%d",sig_bk);
						nvram_set("sig_bk",tmps);
						sprintf(tmps,"%d",siglv_bk);
						nvram_set("siglv_bk",tmps);
						strcpy(addr_tmp,address);
						//_dprintf("=>sig=%d,address=%s\n",sig_tmp, addr_tmp);	
					}
					else if (atoi(sig) > sig_bk)
					{
						sig_bk=atoi(sig);
						siglv_bk=atoi(siglv);
						nvram_set("sta_bssid_bk",address);
						sprintf(tmps,"%d",sig_bk);
						nvram_set("sig_bk",tmps);
						sprintf(tmps,"%d",siglv_bk);
						nvram_set("siglv_bk",tmps);
					}
				}
			}
			else //2G
			{
				memset(tmpnv,0,sizeof(tmpnv));
				sprintf(tmpnv,"wl%d_ssid",band);
				if(!strcmp(nvram_safe_get(tmpnv), ssid))
				{
					memset(tmpnv,0,sizeof(tmpnv));
					sprintf(tmpnv,"wl%d_sta_bssid",band?0:1);//compare with 5G's bssid if scan 2G's bssid
					if(strlen(nvram_safe_get(tmpnv)))
					{
						memset(address2,0,sizeof(address2));
						strcpy(address2,nvram_safe_get(tmpnv));
					}
					else
						goto skip_cmp;

					for(j=0;j<15;j++)
					{
					//	_dprintf("==>mac1[%d]=%c, mac2[%d]=%c\n",j,*(address+j),j,*(address2+j));
						cmp=abs(*(address+j)-*(address2+j));
						if(cmp!=0 && cmp!=32) //upper or lower 
							goto skip_cmp;
					}
					//last macfield
#if 0
					memset(tm,0,sizeof(tm));
					sprintf(tm,"%c%c",*(address+15),*(address+16));
					sscanf(tm,"%x",&mac1);
					memset(tm,0,sizeof(tm));
					sprintf(tm,"%c%c",*(address2+15),*(address2+16));
					sscanf(tm,"%x",&mac2);

					if(abs(mac1-mac2)==2 || abs(mac1-mac2)==4)
					{
						//_dprintf("=>set sta_bssid_tmp=%s\n",address);
						nvram_set("sta_bssid_tmp",address);
					}
#else
					cmp=abs(*(address+15)-*(address2+15));
					if(cmp!=0 && cmp!=32) //upper or lower 
						goto skip_cmp;
					else
						nvram_set("sta_bssid_tmp",address);
				
#endif
					
				}
			}
		}

skip_cmp:
#endif

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
		
//////

	}

#ifdef RTCONFIG_WIFI_SON
	if(nvram_get_int("x_Setting") && (sw_mode()==SW_MODE_AP && !nvram_match("cfg_master", "1"))) //RE
	{
		if(band)
		{
			if(sig_tmp!=0 && strlen(addr_tmp)>0)
			{
				nvram_set("sta_bssid_tmp",addr_tmp);	
				sprintf(tmps,"%d",sig_tmp);
				nvram_set("sig_tmp",tmps);
				sprintf(tmps,"%d",siglv_tmp);
				nvram_set("siglv_tmp",tmps);
			}
		}
	}
#endif

	fclose(fp);
	fclose(ofp);
	return 1;
}   


#ifdef RTCONFIG_WIRELESSREPEATER
char *wlc_nvname(char *keyword)
{
	return(wl_nvname(keyword, nvram_get_int("wlc_band"), -1));
}
#endif

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

unsigned int getStaXRssi(int unit)
{
	char buf[8192], sta[64];
	FILE *fp;
	int len;
	char *pt1 = "95", *pt2;

	strcpy(sta, get_staifname(unit));
	snprintf(buf, sizeof(buf), "iwconfig %s", sta);
	fp = PS_popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		PS_pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, "Signal level=");
			if (pt1) {
				pt2 = pt1 + strlen("Signal level=");
				pt1 = strtok(pt2, " ");
			}
		}
	}

	return safe_atoi(pt1);
}
	
char *get_qca_iwpriv(char *name, char *command)
{
	char buf[512];
	FILE *fp;
	int len;
	char *pt1, *pt2;
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	sprintf(buf, "cfg80211tool %s %s", name, command);
#else
	sprintf(buf, "iwpriv %s %s", name, command);
#endif
	fp = PS_popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		PS_pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, command);
			if (pt1) {
				pt2 = pt1 + strlen(command) + 1/* ':' */;
				pt1 = pt2;
				while ( *pt1!='\r' && *pt1!='\n' && *pt1!='\0' )
					pt1++;
				*pt1='\0';
				return strdup(pt2); // the caller must free it
			}
		}
	}
	return NULL;
}

unsigned int getPapState(int unit)
{
	char buf[8192], sta[64];
	FILE *fp;
	int len;
	char *pt1 = "95", *pt2;

#if defined(RTCONFIG_CONCURRENTREPEATER)	
	if (!nvram_get_int("wlready") && !mediabridge_mode()) return 0;
#endif
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	if(!nvram_get_int("wlready")) return 0;
#endif
	strcpy(sta, get_staifname(unit));
	snprintf(buf, sizeof(buf), "iwconfig %s", sta);
	fp = PS_popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		PS_pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, "Access Point:");
			if (pt1) {
				pt2 = pt1 + strlen("Access Point:");
				pt1 = strstr(pt2, "Not-Associated");
				if (pt1) 
				{
					snprintf(buf, sizeof(buf), "ifconfig | grep %s", sta);
				     	fp = PS_popen(buf, "r");
					if(fp)
				   	{
						 memset(buf, 0, sizeof(buf));
						 len = fread(buf, 1, sizeof(buf), fp);
						 PS_pclose(fp);
						 if(len>=1)
						    return 0;
						 else
						    return 3;
					}	
				     	else	
				   		return 0; //init
				}	
				else
#if defined(RTCONFIG_CONCURRENTREPEATER)
				{
					char buf2[64] = {0};
					char *wpa_state;
					char ctrl_sk[32];
					get_wpa_ctrl_sk(unit, ctrl_sk, sizeof(ctrl_sk));
					snprintf(buf2, sizeof(buf2), "wpa_cli -p %s -i sta%d status", ctrl_sk, unit);
					FILE *fp2 = NULL;
					fp2 = popen(buf2, "r");
					if (fp2) {
						while (fgets(buf2, 64, fp2) != NULL) {
							if (strstr(buf2, "wpa_state")) {
								wpa_state = buf2 + 10;

								if (strstr(wpa_state, "COMPLETED")) {
									pclose(fp2);
								 	return 2;
								}
							}
						}
						pclose(fp2);
						return 0; // Auth fail.
					}
					else // Auth fail.
						return 0;
				}
#else
				   	return 2; //connect and auth ?????
#endif
					
				}
			}
		}
	
	return 3; // stop
}

#ifdef RTCONFIG_CONCURRENTREPEATER
unsigned int get_conn_link_quality(int unit)
{
	char buf[8192];
	FILE *fp;
	int len;
	char *pt1, *pt2;
	int link_quality = 0;

	if (!nvram_get_int("wlready") && !mediabridge_mode()) return 0;

	snprintf(buf, sizeof(buf), "iwconfig %s", get_staifname(unit));
	fp = popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, "Access Point:");
			/* check whether connected to PAP */
			if (pt1) {
				pt2 = pt1 + strlen("Access Point:");
				pt1 = strstr(pt2, "Not-Associated");
				if (pt1) 
					return 0; /* not connected to PAP */	
			}

			/* get link quality */
			pt1 = strstr(buf, "Link Quality=");
			if (pt1) {
				pt2 = pt1 + strlen("Link Quality=");
				pt1 = strchr(pt2, '/');
				if (pt1) {
					*pt1 = 0;
					//dbG("Link Quality(%d)=%s\n", unit, pt2);
					link_quality = safe_atoi(pt2);
				}
			}
		}
	}
	
	return link_quality;
}

int select_wlc_band()
{
	int wlc_band = -1;
	int wlc0_quality = 0;
	int wlc1_quality = 0;

	if (nvram_get_int("wlc0_state") != WLC_STATE_CONNECTED)
		wlc0_quality = 0;
	else
		wlc0_quality = get_conn_link_quality(0);

	if (nvram_get_int("wlc1_state") != WLC_STATE_CONNECTED)
		wlc1_quality = 0;
	else
		wlc1_quality = get_conn_link_quality(1);

	if (wlc0_quality <= wlc1_quality)
		wlc_band = 1;
	else
		wlc_band = 0;

	return wlc_band;
}
#endif

// TODO: wlcconnect_main
//	wireless ap monitor to connect to ap
//	when wlc_list, then connect to it according to priority
#define FIND_CHANNEL_INTERVAL	15
int wlcconnect_core(void)
{
	int unit, ret;
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	static int reconnected;
	int shortgi = -1, i;
	char buf[18] = {0};
	char ifname[16] = {0}, ifname2[16] = {0}, prefix[8];
#endif

#ifdef RTCONFIG_CONCURRENTREPEATER
	int sw_mode = sw_mode();
	char buf[32] = {0};

	int wlc_express = nvram_get_int("wlc_express");
	
	if (nvram_get_int("wps_cli_state") == 1 && sw_mode == SW_MODE_REPEATER)
		return 0;

	if (!nvram_get_int("wlready") && !mediabridge_mode())
		return 0;

	if (sw_mode == SW_MODE_REPEATER) {
	 	if (wlc_express == 0) {	/* for sta0 & sta1 */
			unit = nvram_get_int("wlc_band");
			if (unit == 0 || unit == 1) {
				ret = getPapState(unit);	/* for sta0 or sta1 */
				snprintf(buf, sizeof(buf), "wlc%d_state", unit);
				nvram_set_int(buf, ret);
#ifdef RTCONFIG_HAS_5G
				/* Update the other band state */
				if (unit) { 
					nvram_set_int("wlc0_state", getPapState(0));
				}
				else {
					nvram_set_int("wlc1_state", getPapState(1));
				}
#endif
			}
			else
			{
				ret = getPapState(0);	/* for sta0 */
				nvram_set_int("wlc0_state", ret);
#ifdef RTCONFIG_HAS_5G
				if (ret != WLC_STATE_CONNECTED) {
					ret = getPapState(1);	/* for sta1 */
					nvram_set_int("wlc1_state", ret);
				}
				else { // Update sta1 state
					nvram_set_int("wlc1_state", getPapState(1));
				}
#endif
			}
		}
		else if (wlc_express == 1) {
			ret = getPapState(0);	/* for sta0 */
			nvram_set_int("wlc0_state", ret);
		}
#ifdef RTCONFIG_HAS_5G
		else if (wlc_express == 2) {
			ret = getPapState(1);	/* for sta1 */
			nvram_set_int("wlc1_state", ret);
		}
#endif
	}
#else
	unit=nvram_get_int("wlc_band");
	ret=getPapState(unit);
#endif
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	if(ret!=2){
		dbG("check..wlconnect=%d \n",ret);
		reconnected = 0;
	}else{
		memset(buf, 0, sizeof(buf));
		strlcpy(ifname, get_staifname(unit), sizeof(ifname));
		strlcpy(buf, iwpriv_get(ifname, "get_mode") ? : "", sizeof(buf))
		if(!strncmp(buf, "11GHE", 5) || !strncmp(s, "11AHE", 5)){
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			for(i = 0; i < num_of_mssid_support(); i++){
				get_wlxy_ifname(unit, 0, ifname2);
				if(iface_exist(ifname2)){
					strlcpy(buf, iwpriv_get(ifname2, "get_mode") ? : "", sizeof(buf));
					if((!strncmp(buf, "11GHE", 5) || !strncmp(buf, "11AHE", 5)) && iwpriv_get_int(ifname2, "get_shortgi", &shortgi) >= 0 && shortgi == 1){
						eval("cfg80211tool", ifname2, "shortgi", "0");
					}
				}
			}
		}
		if(!reconnected && nvram_match(strcat_r(prefix, "nband", buf), "1")){
			doSystem("cfg80211tool %s mode %s", ifname, "AUTO");
			reconnected = 1;
		}
	}
#else
	if(ret!=2) //connected
		dbG("check..wlconnect=%d \n",ret);
#endif
	return ret;
}

#if defined(RTCONFIG_WLCSCAN_RSSI)
/**
 * Get specific SSID signal quility via sitesurvey.
 * @param  band 0: 2.4G. 1: 5G.
 * @param  ssid Specific SSID for RSSI
 * @return      Not for anything.
 */
int wlcscan_ssid_rssi(int band, char *ssid)
{

	if (band < 0 || band > 1 || ssid == NULL || *ssid == '\0') {
		nvram_set_int("wlcscan_ssid_rssi_state", -1);
		return -1;
	}

	nvram_set_int("wlcscan_ssid_rssi_state", 1);

	int ret, i, k;
	int retval = 0, ap_count = 0, count = 0;
	unsigned char bssid[6];
	wl_scan_results_t *result;
	wl_scan_params_t *params;
	int params_size = WL_SCAN_PARAMS_FIXED_SIZE + NUMCHANS * sizeof(uint16);
	char wif[8] = {0}, prefix[] = "wlXXXXXXXXXX_", tmp[100] = {0};

	snprintf(prefix, sizeof(prefix), "wl%d_", band);
	strncpy(wif, nvram_safe_get(strcat_r(prefix, "ifname", tmp)), sizeof(wif));

	retval = 0;

	snprintf(tmp, sizeof(tmp), "wlc%d_scan_rssi", band);
	nvram_set_int(tmp, 50);
	nvram_set_int("wlcscan_ssid_rssi_state", 2);
	return retval;
}
#endif

int wlcscan_core(char *ofile, char *wif)
{
	int ret,count;

	count=0;

	while((ret=getSiteSurvey(get_wifname_num(wif),ofile)==0)&& count++ < 2)
	{
		 dbg("[rc] set scan results command failed, retry %d\n", count);
		 sleep(1);
	}   

	return 0;
}

int LanWanLedCtrl(void)
{
#ifdef RTCONFIG_LANWAN_LED
	if(get_lanports_status()
#if defined(RTCONFIG_WPS_ALLLED_BTN) || defined(RTCONFIG_WPS_RST_BTN)
			&& nvram_match("AllLED", "1")
#endif
			)
		led_control(LED_LAN, LED_ON);
	else
		led_control(LED_LAN, LED_OFF);
#elif defined(RTCONFIG_LAN4WAN_LED)
#ifdef RTCONFIG_WPS_ALLLED_BTN
	if (nvram_match("AllLED", "1"))
#endif
	led_ctrl();
#endif

	return 1;
}

#if defined(MAPAC1300) || defined(VZWAC1300)
int get_wifi_thermal(int band)
{
	char thermal_path[64];
	char value[16];
	char *wifi_if;
	int len;

	if((wifi_if = get_vphyifname(band)) == NULL)
	{
		cprintf("%s: cannot get_vphyifname(%d)\n", __func__, band);
		return -1;
	}

	snprintf(thermal_path, sizeof(thermal_path), "/sys/class/net/%s/thermal/temp", wifi_if);
	if((len = f_read_string(thermal_path, value, sizeof(value))) <= 0)
	{
		cprintf("%s: thermal_path(%s) fail !!!\n", __func__, thermal_path);
		return -1;
	}
	//cprintf("## %s(%d) value(%s)\n", __func__, band, value);
	return atoi(value);
}

int thermal_get_txpwr(int thermal)
{
	int th_high, th_low, th_pwr, th_max;

	if((th_high = nvram_get_int("th_high")) <= 0)
		th_high = 135;

	if((th_low = nvram_get_int("th_low")) <= 0)
		th_low  = 115;

	if((th_pwr = nvram_get_int("th_pwr")) <= 0)
		th_pwr  = 22;

	if((th_max = nvram_get_int("th_max")) <= 0)
		th_max  = 25;

	//cprintf("## th_high(%d) th_low(%d) th_pwr(%d) th_max(%d)\n", th_high, th_low, th_pwr, th_max);

	if(thermal <= th_low)
		return th_max;
	else if(thermal >= th_high)
		return th_pwr;

	return -1;
}

void set_thermaltool_level(const char *wifi_if, int enable, int level, int low, int high, int off)
{
	if(wifi_if == NULL)
		return;

	doSystem("thermaltool -i %s -set -e %d -dc 100 -lo%d %d -hi%d %d -off%d %d", wifi_if, enable
			, level, low
			, level, high
			, level, off);
}

void set_thermaltool(int band)
{
	char *wifi_if;
	if((wifi_if = get_vphyifname(band)) == NULL)
	{
		cprintf("%s: cannot get_vphyifname(%d)\n", __func__, band);
		return;
	}

	set_thermaltool_level(wifi_if, 1, 0, -100, 115, 0);
	set_thermaltool_level(wifi_if, 1, 1,  105, 125, 25);
	set_thermaltool_level(wifi_if, 1, 2,  115, 135, 75);
	set_thermaltool_level(wifi_if, 1, 3,  130, 150, 100);
}

static int last_txpwr[4];

void set_thermal(int band, int thermal)
{
	int target_txpwr;
	int need_to_set;

	need_to_set = doSystem("thermaltool -i %s -get | grep -q 'dcoffpercent: 25'", get_vphyifname(band));
	if((target_txpwr = thermal_get_txpwr(thermal)) >= 0 && (target_txpwr != last_txpwr[band] || need_to_set))
	{
		char txpower[16];
		char *ifname;

		txpower[0] = '\0';
		ifname = get_wififname(band);

		if(band == 0)
		{
			snprintf(txpower, sizeof(txpower), "%d", target_txpwr);
			eval("iwconfig", ifname, "txpower", txpower);
		}

		set_thermaltool(band);
		logmessage(__func__, "%s: thermal(%d) txpower(%s)\n", ifname, thermal, txpower);

		last_txpwr[band] = target_txpwr;
	}
}


int thermal_txpwr_main(int argc, char *argv[])
{
	int thermal;
	int band;
	int th_intv;
	char word[128], *next, wl_ifnames[128];

	memset(last_txpwr, 0, sizeof(last_txpwr));

	while(1)
	{
		th_intv = nvram_get_int("th_intv");
		if(th_intv <= 0)
			th_intv = 30;

		band = 0;
		strcpy(wl_ifnames, nvram_safe_get("wl_ifnames"));

		foreach(word, wl_ifnames, next) {
			if((thermal = get_wifi_thermal(band)) > 0)
			{ //has valid value
				set_thermal(band, thermal);
			}
			band++;
		}
		sleep(th_intv);
	}
	return 0;
}
#endif	/* MAPAC1300 || VZWAC1300 */

#ifdef RTCONFIG_WIFI_SON
#define CFG_JSON_FILE           "/jffs/cfg.json"
void gen_spcmd(char *cmd)
{
	char cfg_ver[17] = {0};
	srand(time(NULL));
	snprintf(cfg_ver, sizeof(cfg_ver), "%d%d%d", rand(), rand(), rand());
	f_write_string(CFG_JSON_FILE, "{\"spcmd\":\"\",\"action_script\":\"\"}", 0, 0);
	/* trigger */
	nvram_set("cfg_ver", cfg_ver);
	nvram_set("spcmd", cmd);
	kill_pidfile_s("/var/run/cfg_server.pid", SIGUSR2);
}
#endif

struct acs_chweight_t {
	int model;
	char *country;
	const char *band;
	char *chanlist;
}; 
/*	
 *	channel initial: "0"
 *	2.4G channel list: "10", "20", "30", "40", "50", "60", "70", "80", "90", "100", "110", "120", "130", "140"
 *	5G channel list:
		  "360", "400", "440", "480"
		, "520", "560", "600", "640"
		, "1000", "1040", "1080", "1120", "1160", "1200", "1240", "1280", "1320", "1360", "1400"
		, "1490", "1530", "1570", "1610", "1650"
 *	channel weight: units digit
 */
struct acs_chweight_t acs_chweight[] = {
#if defined(VZWAC1300)
	{ MODEL_VZWAC1300, "US", VPHY_2G, "0 12 112" },
	{ MODEL_VZWAC1300, "US", VPHY_5G, "0 364 1652" },
#endif
	{ MODEL_UNKNOWN, NULL, NULL, NULL }
};

/*
 * chan + weight = chanweight.
 * weight: 0 ~ 9
 * e.g. channel=5, weight=6, chanweight=56
 *
 * */
void acs_ch_weight_param()
{
	struct acs_chweight_t *acs_chweight_p=acs_chweight; 
	char *country=nvram_safe_get("wl0_country_code");
	char *band, *buf;
	char *delim=" ", tmp[256];

	for (; acs_chweight_p->model!=0; acs_chweight_p++) {
		if (!strncmp(country, acs_chweight_p->country, strlen(country))) {
			memset(tmp, '\0', sizeof(tmp));
			band = strdup(acs_chweight_p->band);
			strncpy(tmp, acs_chweight_p->chanlist, sizeof(tmp));

			buf = strtok(tmp, delim);
			while (buf!=NULL) {
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
				eval("cfg80211tool", band, "acs_weight", buf);
#else
				eval("iwpriv", band, "acs_weight", buf);
#endif
				buf = strtok(NULL, delim);
			}
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
			eval("cfg80211tool", band, "g_acs_weight", "");
#else
			eval("iwpriv", band, "g_acs_weight", "");
#endif
		}
	}

}

#ifdef RTCONFIG_AMAS
int get_psta_rssi(int unit)
{
	return Pty_get_upstream_rssi(unit);
}

extern int g_upgrade;

int no_need_obd(void)
{
	if ((getAmasSupportMode() & 2) == 0 || g_reboot || g_upgrade)
		return -1;

	if (IS_ATE_FACTORY_MODE())
		return -1;

	if (!is_router_mode() || (nvram_get_int("obd_Setting") == 1) || (nvram_get_int("x_Setting") == 1))
		return -1;

	if (nvram_get_int("wlready") == 0)
		return -1;

	return pids("obd");
}

int no_need_obdeth(void)
{
	if ((getAmasSupportMode() & 2) == 0 || g_reboot || g_upgrade)
		return -1;

	if (IS_ATE_FACTORY_MODE())
		return -1;

	if (!is_router_mode() || (nvram_get_int("obd_Setting") == 1) || (nvram_get_int("x_Setting") == 1))
		return -1;

	if (nvram_get_int("wlready") == 0)
		return -1;

	return pids("obdeth");
}

void amas_wait_wifi_ready(void)
{
	while( !nvram_get_int("wlready") )
		sleep(5);
}

int en_multi_profile()
{
	if(nvram_match("re_mode", "1"))
	{
		if(nvram_get_int("dwb_mode") != 1)
      		return nvram_get_int("dwb_mode") == 3;
	}
	return 0;
}


int __fastcall write_wpa_supplicant_network(int result, int a2, char *src, int a4, char *a5, const char *a6, int a7)

#ifdef RTCONFIG_BHCOST_OPT
void apply_config_to_driver(int band)
#else
void apply_config_to_driver()
#endif
{
}

int check_gn()
{
}

#ifdef RTCONFIG_AMAS_WGN
void wgn_sysdep_swtich_unset(int vid)
{
	if(&__wgn_sysdep_swtich_unset)
		_wgn_sysdep_swtich_unset(vid);
	else
		doSystem("ssdk_sh vlan entry del %d", vid);
}

void wgn_sysdep_swtich_set(int vid)
{
	int i;
	if(&__wgn_sysdep_swtich_set)
		return __wgn_sysdep_swtich_set();

	doSystem("ssdk_sh vlan entry create %d", vid);
	for(i = 0; i < 6; i++)
		doSystem("ssdk_sh vlan member add %d %d unmodified", vid, i);
}
#endif
char *find_brX(const char *br)
{
	int id;
	static char br_if[16];
	char *wgn_if = nvram_safe_get("wgn_ifnames");
	char *next, *p;
	char dest[32], buf[80];
	memset(br_if, 0, sizeof(br_if));
	if(*wgn_if)
	{
		foreach(dest, wgn_if, next)
		{
			if(sscanf(dest, "br%d", &id) == 1)
			{
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "br%d_ifnames", id);
				p = nvram_get(buf);
				if(p && *p && strstr(p, br))
					strlcpy(br_if, dest, sizeof(br_if));
			}
		}
	}
	if(!br_if[0])
		strlcpy(br_if, nvram_safe_get("lan_ifname"), sizeof(br_if));
	return br_if;
}
#endif

void post_wlc_connected()
{
	;
}

void pre_addif_bridge(int iftype)
{
	;
}

void post_sent_action(void)
{
	;
}

void post_update_status(void)
{
	;
}

void post_bh_changed(int iftype)
{
	int if_type;
	char s[64], tmp[64];
	char *next, *next2, *p;
	char dest[32], dest2[32];
	char *ifname = "lan";
	if(nvram_get_int("wgn_enabled") && nvram_get_int("re_mode"))
	{
		memset(s, 0, sizeof(s));
		strlcpy(s, nvram_safe_get("amas_ifname"), sizeof(s));
		if(s[0])
		{
			foreach(dest, nvram_safe_get("wgn_ifnames"), next){
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "wgn_%s_lan_ifnames", dest);
				foreach(dest2, nvram_safe_get(tmp), next2)
					eval("brctl", "delif", dest2);
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "wgn_%s_sta_ifnames", dest);
				foreach(dest2, nvram_safe_get(tmp), next2)
					eval("brctl", "delif", dest2);
			}
			if_type = iftype - 1;
			if(if_type >= 0x8000)
				ifname = "sta";
			foreach(dest, nvram_safe_get("wgn_ifnames"), next){
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "wgn_%s_%s_ifnames", dest, ifname);
				if(if_type >= 0x8000)
				{
					p = strstr(s, ".0");
					if(p)
						*p = 0x0;
					foreach(dest2, nvram_safe_get(tmp), next2){
						if(strstr(s, dest2))
							eval("brctl", "addif", dest, dest2);
					}
				}else{
					foreach(dest2, nvram_safe_get(tmp), next2){
						if(strstr(s, dest2))
							eval("brctl", "addif", dest, dest2);
					}
				}
			}
		}else{
			foreach(dest, nvram_safe_get("wgn_ifnames"), next){
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "wgn_%s_lan_ifnames", dest);
				foreach(dest2, nvram_safe_get(tmp), next2){
					eval("brctl", "delif", dest2);
				}
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "wgn_%s_sta_ifnames", dest);
				foreach(dest2, nvram_safe_get(tmp), next2){
					eval("brctl", "delif", dest2);
				}
			}
		}
	}
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

void duplicate_wl_ifaces(void)
{
	int unit = 1;
	char prefix[] = "wlXXXXXXX_", prefix2[] = "wlXXXXXXX_";
	char tmp[128], tmp2[128];

	snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	while(unit < num_of_wl_if())
	{
		if((nvram_get_int("dwb_mode") == 1 || nvram_get_int("dwb_mode") == 3) && nvram_get_int("dwb_band") > 0 && unit == nvram_get_int("dwb_band"))
		{
			snprintf(prefix2, sizeof(prefix2), "wl%d_", unit);
			if(!nvram_get("sw_mode") || nvram_get_int("sw_mode") == 1)
			{
				if(strstr(nvram_safe_get(strcat_r(prefix2, "ssid", tmp2)), "dwb"))
				{
					dbg("Don't apply wl0 to wl%d in smart_connect function, if enabled DWB mode.\n", unit);
					continue;
				}
			}
			nvram_set_int("dwb_mode", 0);
		}
		snprintf(prefix2, sizeof(prefix2), "wl%d_", unit);
		nvram_set(strcat_r(prefix2, "ssid", tmp2), nvram_safe_get(strcat_r(prefix, "ssid", tmp)));
		nvram_set(strcat_r(prefix2, "auth_mode_x", tmp2), nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)));
		nvram_set(strcat_r(prefix2, "wep_x", tmp2), nvram_safe_get(strcat_r(prefix, "wep_x", tmp)));
		nvram_set(strcat_r(prefix2, "key", tmp2), nvram_safe_get(strcat_r(prefix, "key", tmp)));
		nvram_set(strcat_r(prefix2, "key1", tmp2), nvram_safe_get(strcat_r(prefix, "key1", tmp)));
		nvram_set(strcat_r(prefix2, "key2", tmp2), nvram_safe_get(strcat_r(prefix, "key2", tmp)));
		nvram_set(strcat_r(prefix2, "key3", tmp2), nvram_safe_get(strcat_r(prefix, "key3", tmp)));
		nvram_set(strcat_r(prefix2, "key4", tmp2), nvram_safe_get(strcat_r(prefix, "key4", tmp)));
		nvram_set(strcat_r(prefix2, "phrase_x", tmp2), nvram_safe_get(strcat_r(prefix, "phrase_x", tmp)));
		nvram_set(strcat_r(prefix2, "crypto", tmp2), nvram_safe_get(strcat_r(prefix, "crypto", tmp)));
		nvram_set(strcat_r(prefix2, "wpa_psk", tmp2), nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp)));
		nvram_set(strcat_r(prefix2, "radius_ipaddr", tmp2), nvram_safe_get(strcat_r(prefix, "radius_ipaddr", tmp)));
		nvram_set(strcat_r(prefix2, "radius_key", tmp2), nvram_safe_get(strcat_r(prefix, "radius_key", tmp)));
		nvram_set(strcat_r(prefix2, "radius_port", tmp2), nvram_safe_get(strcat_r(prefix, "radius_port", tmp)));
		nvram_set(strcat_r(prefix2, "closed", tmp2), nvram_safe_get(strcat_r(prefix, "closed", tmp)));
		nvram_set(strcat_r(prefix2, "11ax", tmp2), nvram_safe_get(strcat_r(prefix, "11ax", tmp)));
	}
	nvram_commit();
}

int get_wifi_country_code_tmp(char *ccode, char *ccode_str, int len)
{
	return -1;
}

