/*
 * Copyright 2021, ASUS
 * Copyright 2021-2024, SWRTdev
 * Copyright 2021-2024, paldier <paldier@hotmail.com>.
 * Copyright 2021-2024, lostlonger<lostlonger.g@gmail.com>.
 * All Rights Reserved.
 */

#include <rc.h>
#ifdef RTCONFIG_RALINK
#include <stdio.h>
#include <fcntl.h>
#include <ralink.h>
#include <bcmnvram.h>
//#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <net/if_arp.h>
#include <shutils.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
//#include <linux/if.h>
#include <iwlib.h>
#include <wps.h>
#include <stapriv.h>
#include <shared.h>
#include "flash_mtd.h"
#include "ate.h"

#ifdef RTCONFIG_WIRELESSREPEATER
#include <ap_priv.h>
#endif
#ifdef RTCONFIG_MULTILAN_CFG
#include <mtlan_utils.h>
#endif
#define MAX_FRW 64
#define MACSIZE 12

#define	DEFAULT_SSID_2G	"ASUS"
#define	DEFAULT_SSID_5G	"ASUS_5G"

char *wlc_nvname(char *keyword);
extern char *ether_cal_b(const unsigned char *e, char *a, int i);
extern char *ether_cal(const char *e, char *a, int i);
#if defined(RTCONFIG_WIRELESSREPEATER)
extern int set_site_survey(const char *wif, char *ssid);
extern int use_partial_scan(const char *wif);
extern int partial_scan_status(const char *wif);
extern int get_site_survey(const char *wif, char *buffer, size_t len, SSA **ssap, int *count);
#endif

#undef OLD_ACL  //for standalone acl policy

#define xR_MAX	4
typedef struct _roaming_info
{
       char mac[19];
       char rssi[xR_MAX][7];
       char dump;
} roam;

typedef struct _roam_sta
{
       roam sta[128];
}roam_sta;

#if 0
typedef struct {
	unsigned int link[5];
	unsigned int speed[5];
} phyState;
#endif

int g_wsc_configured = 0;
int g_isEnrollee[MAX_NR_WL_IF] = { 0, };
static int pap_channel = 0;
static int apcli_status = 0;

int enum_sname_mvalue_w_fixed_value(FILE *fp, int ssid_num, const char *key, const char *value)
{
	int i;
	char tmp[128] = {0}, tmp2[256] = {0};

	if(fp == NULL || ssid_num < 0 || !value)
		return -1;

	for(i = 0; i < ssid_num; i++){
		snprintf(tmp, sizeof(tmp), "%s%s", i ? ";" : "", value);
		strlcat(tmp2, tmp, sizeof(tmp2));
	}
	fprintf(fp, "%s=%s\n", key, tmp2);
	return 0;
}

int enum_mname_svalue_w_fixed_value(FILE *fp, int max, char *key_prefix, const char *value, int base)
{
	int i;
	char tmp[64];
	if(fp == NULL || max <= 0)
		return -1;
	if(!strstr(key_prefix, "%d") || !value)
		return -1;
	for(i = 0; i < max; i++){
		snprintf(tmp, sizeof(tmp), key_prefix, base + i);
		fprintf(fp, "%s=%s\n", tmp, value);
	}
	return 0;
}

int getCountryRegion5G(const char *countryCode, int *warning, int IEEE80211H);

static void iwprivSet(const char *ifname, const char *name, const char *value)
{
	char tmpBuf[256];
	snprintf(tmpBuf, sizeof(tmpBuf), "%s=%s", name, value);
	_dprintf("[%s] %s (%s)\n", __func__, ifname, tmpBuf);
	eval("iwpriv", (char*) ifname, "set", tmpBuf);
}


char *get_wscd_pidfile(void)
{
	static char tmpstr[32];

	snprintf(tmpstr, sizeof(tmpstr), "/var/run/wscd.pid.%s", nvram_get_int("wps_band_x") ? WIF_5G : WIF_2G);
	return tmpstr;
}

char *get_wscd_pidfile_band(int wps_band)
{
	static char tmpstr[32];

	snprintf(tmpstr, sizeof(tmpstr), "/var/run/wscd.pid.%s", wps_band ? WIF_5G : WIF_2G);
	return tmpstr;
}

int get_wifname_num(char *name)
{
	if(strcmp(WIF_5G,name)==0)
	   	return 1;
	else if (strcmp(WIF_2G,name)==0)
	   	return 0;
//	else if (strcmp(WIF_5G2,name)==0)
//	   	return 2;
	return -1;
}

const char *get_wifname(int band)
{
	if (band == 1)
		return WIF_5G;
//	else if (band == 2)
//		return WIF_5G2;
	return WIF_2G;
}

const char *get_wpsifname(void)
{
	int wps_band = nvram_get_int("wps_band_x");

	if (wps_band == 1)
		return WIF_5G;
//	else if (wps_band == 2)
//		return WIF_5G2;
	return WIF_2G;
}

#if 0
char *
get_non_wpsifname()
{
	int wps_band = nvram_get_int("wps_band_x");

	if (wps_band)
		return WIF_2G;
	else
		return WIF_5G;
}
#endif

#ifdef RTCONFIG_DSL
// used by rc
void get_country_code_from_rc(char* country_code)
{
	unsigned char CC[3];
	memset(CC, 0, sizeof(CC));
	FRead(CC, OFFSET_COUNTRY_CODE, 2);

	if (CC[0] == 0xff && CC[1] == 0xff)
	{
		*country_code++ = 'T';
		*country_code++ = 'W';
		*country_code = 0;
	}
	else
	{
		*country_code++ = CC[0];
		*country_code++ = CC[1];
		*country_code = 0;
	}
}
#endif

static unsigned char nibble_hex(char *c)
{
	int val;
	char tmpstr[3];

	tmpstr[2]='\0';
	memcpy(tmpstr,c,2);
	val= strtoul(tmpstr, NULL, 16);
	return val;
}

static int atoh(const char *a, unsigned char *e)
{
	char *c = (char *) a;
	int i = 0;

	memset(e, 0, MAX_FRW);
	for (i = 0; i < MAX_FRW; ++i, c += 3) {
		if (!isxdigit(*c) || !isxdigit(*(c+1)) || isxdigit(*(c+2))) // should be "AA:BB:CC:DD:..."
			break;
		e[i] = (unsigned char) nibble_hex(c);
	}

	return i;
}

char *htoa(const unsigned char *e, char *a, int len)
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

int
FREAD(unsigned int addr_sa, int len)
{
	unsigned char buffer[MAX_FRW];
	char buffer_h[ MAX_FRW * 3 ];
	memset(buffer, 0, sizeof(buffer));
	memset(buffer_h, 0, sizeof(buffer_h));

	if (len > MAX_FRW)
	{
		dbg("FREAD: cut to %d bytes\n", MAX_FRW);
		len = MAX_FRW;
	}
	if (FRead(buffer, addr_sa, len)<0)
		dbg("FREAD: Out of scope\n");
	else
	{
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
int
FWRITE(const char *da, const char* str_hex)
{
	unsigned char ee[MAX_FRW];
	unsigned int addr_da;
	int len;

	addr_da = strtoul(da, NULL, 16);
	if (addr_da && (len = atoh(str_hex, ee)))
	{
		FWrite(ee, addr_da, len);
		FREAD(addr_da, len);
	}
	return 0;
}
//End of new ATE Command

int getCountryRegion2G(const char *countryCode)
{
	if (countryCode == NULL)
	{
		return 5;	// 1-14
	}
	else if((strcasecmp(countryCode, "CA") == 0) || (strcasecmp(countryCode, "CO") == 0) ||
		(strcasecmp(countryCode, "DO") == 0) || (strcasecmp(countryCode, "GT") == 0) ||
		(strcasecmp(countryCode, "MX") == 0) || (strcasecmp(countryCode, "NO") == 0) ||
		(strcasecmp(countryCode, "PA") == 0) || (strcasecmp(countryCode, "PR") == 0) ||
		(strcasecmp(countryCode, "TW") == 0) || (strcasecmp(countryCode, "US") == 0) ||
		(strcasecmp(countryCode, "UZ") == 0) ||
		(strcasecmp(countryCode, "Z1") == 0) || (strcasecmp(countryCode, "Z3") == 0) ||
		(strcasecmp(countryCode, "AA") == 0)
		)
	{
		return 0;	// 1-11
	}
	else if (strcasecmp(countryCode, "DB") == 0  || strcasecmp(countryCode, "") == 0)
	{
		return 5;	// 1-14
	}

	return 1;	// 1-13
}

int getChannelNumMax2G(int region)
{
	switch(region)
	{
		case 0: return 11;
		case 1: return 13;
		case 2: return 14;
		case 3: return 14;
		case 4: return 14;
		case 5: return 14;
	}
	return 14;
}

int getCountryRegion5G(const char *countryCode, int *warning, int IEEE80211H)
{
#ifdef RTCONFIG_RALINK_DFS
	if (IEEE80211H)
	{
		if(!strcasecmp(countryCode, "GB"))
			return 18;
		else if(!strcasecmp(countryCode, "JP"))
			return 23;
		else if(!strcasecmp(countryCode, "AA"))
			return 9;
		else if(!strcasecmp(countryCode, "TW") || !strcasecmp(countryCode, "US") || !strcasecmp(countryCode, "BZ"))
			return 13;
		else if(!strcasecmp(countryCode, "CA"))
			return 14;
		else if(!strcasecmp(countryCode, "CN"))
			return 0;
		else if(!strcasecmp(countryCode, "GB"))
			return 12;
	}else
#endif	/* RTCONFIG_RALINK_DFS */
	if(nvram_match("location_code", "XX") || nvram_match("location_code", "AU") || nvram_match("location_code", "US"))
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
		return 26;
#else
		return 9;
#endif
	else if(!strcasecmp(countryCode, "UA"))
		return 9;
	else if (!strcasecmp(countryCode, "AE") || !strcasecmp(countryCode, "AL") || !strcasecmp(countryCode, "AR") || !strcasecmp(countryCode, "AU") ||
		!strcasecmp(countryCode, "BH") || !strcasecmp(countryCode, "BY") || !strcasecmp(countryCode, "CA") || !strcasecmp(countryCode, "CL") ||
		!strcasecmp(countryCode, "CO") || !strcasecmp(countryCode, "CR") || !strcasecmp(countryCode, "DO") || !strcasecmp(countryCode, "DZ") ||
		!strcasecmp(countryCode, "EC") || !strcasecmp(countryCode, "GT") || !strcasecmp(countryCode, "HK") || !strcasecmp(countryCode, "HN") ||
		!strcasecmp(countryCode, "IL") || !strcasecmp(countryCode, "IN") || !strcasecmp(countryCode, "JO") || !strcasecmp(countryCode, "KW") ||
		!strcasecmp(countryCode, "KZ") || !strcasecmp(countryCode, "LB") || !strcasecmp(countryCode, "MA") || !strcasecmp(countryCode, "MK") ||
		!strcasecmp(countryCode, "MO") || !strcasecmp(countryCode, "MX") || !strcasecmp(countryCode, "MY") || !strcasecmp(countryCode, "NO") ||
		!strcasecmp(countryCode, "NZ") || !strcasecmp(countryCode, "OM") || !strcasecmp(countryCode, "PA") || !strcasecmp(countryCode, "PK") ||
		!strcasecmp(countryCode, "PR") || !strcasecmp(countryCode, "QA") || !strcasecmp(countryCode, "RO") || !strcasecmp(countryCode, "RU") ||
		!strcasecmp(countryCode, "SA") || !strcasecmp(countryCode, "SG") || !strcasecmp(countryCode, "SV") || !strcasecmp(countryCode, "SY") ||
		!strcasecmp(countryCode, "TH") || !strcasecmp(countryCode, "UA") || !strcasecmp(countryCode, "US") || !strcasecmp(countryCode, "UY") ||
		!strcasecmp(countryCode, "VN") || !strcasecmp(countryCode, "YE") || !strcasecmp(countryCode, "ZW") || !strcasecmp(countryCode, "AA") ||
		!strcasecmp(countryCode, "CN")){
		if (nvram_contains_word("rc_support", "loclist") && !strcasecmp(countryCode, "AU"))
			return 9;
		return 0;
	}else if (!strcasecmp(countryCode, "AM") ||	!strcasecmp(countryCode, "AT") || !strcasecmp(countryCode, "AZ") || !strcasecmp(countryCode, "BE") ||
		!strcasecmp(countryCode, "BG") || !strcasecmp(countryCode, "BR") || !strcasecmp(countryCode, "CH") || !strcasecmp(countryCode, "CY") ||
		!strcasecmp(countryCode, "CZ") || !strcasecmp(countryCode, "DE") || !strcasecmp(countryCode, "DK") || !strcasecmp(countryCode, "EE") ||
		!strcasecmp(countryCode, "EG") || !strcasecmp(countryCode, "ES") || !strcasecmp(countryCode, "FI") || !strcasecmp(countryCode, "FR") ||
		!strcasecmp(countryCode, "GB") || !strcasecmp(countryCode, "GE") || !strcasecmp(countryCode, "GR") || !strcasecmp(countryCode, "HR") ||
		!strcasecmp(countryCode, "HU") || !strcasecmp(countryCode, "IE") || !strcasecmp(countryCode, "IS") || !strcasecmp(countryCode, "IT") ||
		!strcasecmp(countryCode, "JP") || !strcasecmp(countryCode, "KP") || !strcasecmp(countryCode, "KR") || !strcasecmp(countryCode, "LI") ||
		!strcasecmp(countryCode, "LT") || !strcasecmp(countryCode, "LU") || !strcasecmp(countryCode, "LV") || !strcasecmp(countryCode, "MC") ||
		!strcasecmp(countryCode, "NL") || !strcasecmp(countryCode, "NO") || !strcasecmp(countryCode, "PL") || !strcasecmp(countryCode, "PT") ||
		!strcasecmp(countryCode, "RO") || !strcasecmp(countryCode, "SE") || !strcasecmp(countryCode, "SI") || !strcasecmp(countryCode, "SK") ||
		!strcasecmp(countryCode, "TN") || !strcasecmp(countryCode, "TR") || !strcasecmp(countryCode, "TT") || !strcasecmp(countryCode, "UZ") ||
		!strcasecmp(countryCode, "ZA"))
		return 1;
	else if (!strcasecmp(countryCode, "IN") || !strcasecmp(countryCode, "MX"))
		return 2;
	else if (!strcasecmp(countryCode, "TW"))
		return 3;
	else if (!strcasecmp(countryCode, "BR") || !strcasecmp(countryCode, "BZ") || !strcasecmp(countryCode, "BO") || !strcasecmp(countryCode, "BN") ||
		!strcasecmp(countryCode, "ID") || !strcasecmp(countryCode, "IR") || !strcasecmp(countryCode, "MO") || !strcasecmp(countryCode, "PE") ||
		!strcasecmp(countryCode, "PH") || !strcasecmp(countryCode, "VE"))
		return 4;
	else if (!strcasecmp(countryCode, "RU"))
		return 6;
	else if (!strcasecmp(countryCode, "DB"))
		return 7;
	if (warning)
		*warning = 2;
	return 7;
}

#if defined(RTCONFIG_WIFI6E)
int getCountryRegion6G(const char *countryCode, int *warning, int IEEE80211H)
{
	if (!strcasecmp(countryCode, "US"))
		return 0;
	else if (!strcasecmp(countryCode, "CN"))
		return 1;
	else
	{
		if (warning)
			*warning = 2;
		return 1;
	}
}
#endif

int check_macmode(const char *str)
{
	if(str == NULL || !strcmp(str, "") || !strcmp(str, "disabled"))
		return 0;
	else if(!strcmp(str, "allow"))
		return 1;
	else if(!strcmp(str, "deny"))
		return 2;
	return 0;
}

void gen_macmode(int mac_filter[], int band)
{
	int i,j;
	char temp[128], prefix_mssid[] = "wlXXXXXXXXXX_mssid_";
	for (i = 0,j = 0; i < MAX_NO_MSSID; i++)
	{
		if (i)
			sprintf(prefix_mssid, "wl%d.%d_", band, i);
		else
			sprintf(prefix_mssid, "wl%d_", band);

		if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
			continue;

		mac_filter[j] = check_macmode(nvram_safe_get(strcat_r(prefix_mssid, "macmode", temp)));
		j++;
	}
}

static inline void __choose_mrate(char *prefix, int *mcast_phy, int *mcast_mcs)
{
	int phy = 3, mcs = 7;			/* HTMIX 65/150Mbps */

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
			phy = 2; mcs = 2;	/* 2G: OFDM 12Mbps */
		} else {
			phy = 3; mcs = 1;	/* 5G: HTMIX 13/30Mbps */
		}
		/* fall through */
	case IPV6_DISABLED:
		break;
	}
#endif

	if (nvram_pf_match(prefix, "nmode_x", "2") ||		/* legacy mode */
	    strstr(nvram_pf_safe_get(prefix, "crypto"), "tkip"))	/* tkip */
	{
		/* In such case, choose OFDM instead of HTMIX */
		phy = 2; mcs = 4;		/* OFDM 24Mbps */
	}

	*mcast_phy = phy;
	*mcast_mcs = mcs;
}

int get_bw_via_channel(int band, int channel)
{
	int wl_bw;
	char buf[32];

	snprintf(buf, sizeof(buf), "wl%d_bw", band);
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

const char *getCountryCode()
{
	static char tcode[3];
	strlcpy(tcode, nvram_safe_get("territory_code"), 2);

	if(nvram_match("location_code", "XX") || nvram_match("location_code", "AU"))
		return "AU";
	else if(nvram_match("location_code", "US"))
		return "US";
	else if(nvram_match("location_code", "CN"))
		return "CN";
	else if(nvram_match("location_code", "EU"))
		return "FR";
	else if(nvram_match("location_code", "AA"))
		return "US";
	else if(nvram_match("location_code", "KR"))
		return "KR";
	else if(!strncmp(tcode, "TW", 2))
		return "US";
	else if(!strncmp(tcode, "JP", 2))
		return "JP";
	else if(!strncmp(tcode, "UK", 2))
		return "GB";
	else if(!strncmp(tcode, "EU", 2))
		return "FR";
	else if(!strncmp(tcode, "AA", 2))
		return "US";
	else if(!strncmp(tcode, "KR", 2))
		return "KR";
	else if(!strncmp(tcode, "RU", 2))
		return "RU";
	else
		_dprintf("XXXXXXXXX invalid Tcode? [%s] XXXXXXXXX\n", tcode);
	return tcode;
}

int gen_ralink_config(int band, int is_iNIC)
{
	FILE *fp;
	char *str = NULL;
	char *str2 = NULL;
	int  i, r;
	int ssid_num = 1;
	char list[2048];
	int flag_8021x = 0;
	int wsc_configure = 0;
	int warning = 0;
	int ChannelNumMax_2G = 11;
	char tmp[128], prefix[] = "wlXXXXXXX_", prefix_wlc[] = "wlXXXXXXX_";
	char temp[128], prefix_mssid[] = "wlXXXXXXXXXX_mssid_";
	char tmpstr[128];
	int j;
	char *nv, *nvp, *b;
	int wl_key_type[MAX_NO_MSSID];
	int mcast_phy = 0, mcast_mcs = 0;
	int mac_filter[MAX_NO_MSSID];
	int VHTBW_MAX = 0;
	int sw_mode  = sw_mode();
	int wlc_band = nvram_get_int("wlc_band");
	int acs_dfs = nvram_get_int("acs_dfs");
	int IEEE80211H = 0;
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
//	unsigned char *dst;
//	char macaddr[]="00:11:22:33:44:55";
//	char macaddr2[]="00:11:22:33:44:58";
#endif

#if defined(RTCONFIG_CONCURRENTREPEATER)
	char prefix_wlc[] = "wlcXXXXXX_";
	char tmp1[128];
	int wlc_express = nvram_get_int("wlc_express");
#endif
	char *tcode = nvram_safe_get("territory_code");

	if(band > 3)
		return 0;
	if(!nvram_match("acs_dfs", "1")){
		if((band == 1 && nvram_match("wl1_bw_160", "1")) || (band == 2 && nvram_match("wl2_bw_160", "1"))){
			nvram_set("acs_dfs", "1");
			acs_dfs = 1;
		}
	}
	if(nvram_match("x_Setting", "0"))
		acs_dfs = 0;
	if (!is_iNIC)
	{
		_dprintf("gen ralink config\n");
		system("mkdir -p /etc/Wireless/RT2860");
		if (!(fp=fopen("/etc/Wireless/RT2860/RT2860.dat", "w+")))
			return 0;
	}
	else
	{
		_dprintf("gen ralink iNIC config\n");
		system("mkdir -p /etc/Wireless/iNIC");
		if (!(fp=fopen("/etc/Wireless/iNIC/iNIC_ap.dat", "w+")))
			return 0;
	}

	fprintf(fp, "#The word of \"Default\" must not be removed\n");
	fprintf(fp, "Default\n");
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RALINK_DBDC_MODE) || defined(RTCONFIG_MT798X)
	fprintf(fp, "DBDC_MODE=1\n");
#endif
	r = snprintf(prefix, sizeof(prefix), "wl%d_", band);
	if(unlikely(r < 0))
		dbg("snprintf failed\n");
	//CountryRegion
	str = nvram_pf_get(prefix, "country_code");
	if (str && *str)
	{
		int region;
		region = getCountryRegion2G(str);
		fprintf(fp, "CountryRegion=%d\n", region);
		ChannelNumMax_2G = getChannelNumMax2G(region);
	}
	else
	{
		warning = 1;
		fprintf(fp, "CountryRegion=%d\n", 5);
	}

#ifdef RTCONFIG_RALINK_DFS
	if(band && nvram_pf_match(prefix, "IEEE80211H", "1"))
		IEEE80211H = 1;
#endif	/* RTCONFIG_RALINK_DFS */

	//CountryRegion for A band
	str = nvram_pf_get(prefix, "country_code");
	if (str && *str)
	{
		int region;
#ifdef RTCONFIG_RALINK_DFS		
		if(nvram_match("reg_spec", "JP") && nvram_match(strcat_r(prefix, "IEEE80211H", tmp), "1"))
			str = nvram_safe_get("reg_spec");
#endif		
		region = getCountryRegion5G(str, &warning, IEEE80211H);
		fprintf(fp, "CountryRegionABand=%d\n", region);
	}
	else
	{
		warning = 3;
		fprintf(fp, "CountryRegionABand=%d\n", 7);
	}

	//CountryCode
	str = nvram_get("force_wifi_CC");
	if(str && *str)
		fprintf(fp, "CountryCode=%s\n", str);
	else{
		str = nvram_safe_get(strcat_r(prefix, "country_code", tmp));
#ifdef CE_ADAPTIVITY
		if (nvram_match("reg_spec", "CE"))
		{
			fprintf(fp, "CountryCode=%s\n", getCountryCode());
		}
		else
#endif	/* CE_ADAPTIVITY */
		if (str && *str)
		{
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
			if(nvram_match("JP_CS","1"))
				fprintf(fp, "CountryCode=JP\n");
			else
#endif
				fprintf(fp, "CountryCode=%s\n", getCountryCode());
		}
		else
		{
			warning = 4;
			fprintf(fp, "CountryCode=DB\n");
		}
	}
#if 0 //defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	dst = tmp;
	if (FRead(dst, OFFSET_MAC_ADDR_2G, 6) < 0)
		_dprintf("READ MAC address 2G: Out of scope\n");
	else
		ether_etoa(tmp, macaddr);
	fprintf(fp, "MacAddress=%s\n", macaddr);
	for(i = 1; i < MAX_NO_MSSID; i++)
	{
//fix me
		ether_cal(macaddr, macaddr2, i);
		fprintf(fp, "MacAddress%d=%s\n", i, macaddr2);
	}
#endif
	for (i = 0; i < MAX_NO_MSSID; i++)
	{
		if (i)
			snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d.%d_", band, i);
		else
			snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d_", band);
		if (i && (nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1")))
			ssid_num++;
	}

	if ((ssid_num < 1) || (ssid_num > MAX_NO_MSSID))
	{
		warning = 0;
		ssid_num = 1;
	}

#ifdef RTCONFIG_WIRELESSREPEATER
#if defined(RTCONFIG_CONCURRENTREPEATER)
	if (sw_mode == SW_MODE_REPEATER && (wlc_express == 0 || (wlc_express - 1) != band))  //(wlc_express - 1) != band for set ra0 or rai0
#else
	if (sw_mode == SW_MODE_REPEATER && wlc_band == band)
#endif
	{
		ssid_num = 1;
		fprintf(fp, "BssidNum=%d\n", ssid_num);
		if(band == 0)
			sprintf(tmpstr, "SSID1=%s\n",  nvram_safe_get("wl0.1_ssid"));
		else
			sprintf(tmpstr, "SSID1=%s\n",  nvram_safe_get("wl1.1_ssid"));
		fprintf(fp, "%s", tmpstr);	
	}
	else
#endif	/* RTCONFIG_WIRELESSREPEATER */
	{
#if defined(RTCONFIG_CONCURRENTREPEATER)
		if (sw_mode == SW_MODE_AP)
			ssid_num = 1;
#endif
		fprintf(fp, "BssidNum=%d\n", ssid_num);
		//SSID
		for (i = 0, j = 0; i < MAX_NO_MSSID; i++)
		{
			if (i)
			{
				sprintf(prefix_mssid, "wl%d.%d_", band, i);
				if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);
			if (strlen(nvram_safe_get(strcat_r(prefix_mssid, "ssid", temp))))
				sprintf(tmpstr, "SSID%d=%s\n", j + 1, nvram_safe_get(strcat_r(prefix_mssid, "ssid", temp)));
			else
			{
				warning = 5;
				sprintf(tmpstr, "SSID%d=%s%d\n", j + 1, "ASUS", j + 1);
			}
			fprintf(fp, "%s", tmpstr);
		}
	}
	for (i = ssid_num; i < 8; i++)
	{
		sprintf(tmpstr, "SSID%d=\n", i + 1);
		fprintf(fp, "%s", tmpstr);
	}
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	fprintf(fp, "VOW_Airtime_Fairness_En=%d\n", nvram_pf_match(prefix, "atf", "1"));
#endif
#if 0//defined(RTCONFIG_WISP)
	if(wisp_mode()){
	fprintf(fp, "SkipMAT=1\n");
    str = nvram_get("wan0_hwaddr_x");
	if(str && *str){
		fprintf(fp, "ApcliCloneMAC=%s\n", str);
		fprintf(fp, "ApcliCloneBand=%d\n", band);
	}
#endif
	//Network Mode
	str = nvram_safe_get(strcat_r(prefix, "nmode_x", tmp));
	if (band)
	{
		if (str && *str)
		{
			if (atoi(str) == 0)       // Auto
			{
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
				if(nvram_match("wl1_11ax", "1"))
					fprintf(fp, "WirelessMode=%d\n", 17);		// A + AN + AC +AX mixed
				else
#endif
				fprintf(fp, "WirelessMode=%d\n", 14);	// A + AN + AC mixed
				VHTBW_MAX = 1;
			}
			else if (atoi(str) == 1)  // N Only
			{
				fprintf(fp, "WirelessMode=%d\n", 11);	// N in 5G
			}
			else if (atoi(str) == 8)  // AN/AC Mixed
			{
				fprintf(fp, "WirelessMode=%d\n", 15);	// AN + AC mixed
				VHTBW_MAX = 1;
			}
			else if (atoi(str) == 2)  // A
				fprintf(fp, "WirelessMode=%d\n", 2);
			else			// A,N[,AC]
			{
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
				if(nvram_match("wl1_11ax", "1"))
					fprintf(fp, "WirelessMode=%d\n", 17);		// A + AN + AC +AX mixed
				else
#endif
				fprintf(fp, "WirelessMode=%d\n", 14);
				VHTBW_MAX = 1;
			}
		}
		else
		{
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
			if(nvram_match("wl1_11ax", "1"))
				fprintf(fp, "WirelessMode=%d\n", 17);		// A + AN + AC +AX mixed
			else
#endif
			fprintf(fp, "WirelessMode=%d\n", 14);		// A + AN + AC mixed
			VHTBW_MAX = 1;
			warning = 6;
		}
	}
	else
	{
		if (str && *str)
		{
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
			if(nvram_match("wl0_11ax", "1"))
				fprintf(fp, "WirelessMode=%d\n", 16);		// B + G + N +AX mixed
			else
#endif
			if (atoi(str) == 0)       // B,G,N
				fprintf(fp, "WirelessMode=%d\n", 9);
			else if (atoi(str) == 2)  // B,G
				fprintf(fp, "WirelessMode=%d\n", 0);
			else if (atoi(str) == 1)  // N
				fprintf(fp, "WirelessMode=%d\n", 6);
//			else if (atoi(str) == 4)  // G
//				fprintf(fp, "WirelessMode=%d\n", 4);
//			else if (atoi(str) == 0)  // B
//				fprintf(fp, "WirelessMode=%d\n", 1);
			else			// B,G,N
				fprintf(fp, "WirelessMode=%d\n", 9);
		}
		else
		{
			warning = 7;
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
			if(nvram_match("wl0_11ax", "1"))
				fprintf(fp, "WirelessMode=%d\n", 16);		// B + G + N +AX mixed
			else
#endif
			fprintf(fp, "WirelessMode=%d\n", 9);
		}
	}

	fprintf(fp, "TxCmdMode=%d\n", 1);
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	if(nvram_pf_match(prefix, "twt", "1"))
		fprintf(fp, "TWTSupport=%d\n", 1);
	else
		fprintf(fp, "TWTSupport=%d\n", 0);
	str = nvram_pf_get(prefix, "mbo_enable");
	if(str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MboSupport", str);
	else
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MboSupport", "0");
	if(nvram_pf_match(prefix, "11ax", "1"))
	{
		if(nvram_pf_match(prefix, "ofdma", "1"))
		{
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaDlEnable", "1");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaUlEnable", "0");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoDlEnable", "0");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoUlEnable", "0");
		}
		else if(nvram_pf_match(prefix, "ofdma", "2"))
		{
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaDlEnable", "1");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaUlEnable", "1");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoDlEnable", "0");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoUlEnable", "0");
		}
		else if(nvram_pf_match(prefix, "ofdma", "3"))
		{
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaDlEnable", "1");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaUlEnable", "1");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoDlEnable", "1");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoUlEnable", "1");
		}
		else if(nvram_pf_match(prefix, "ofdma", "4"))
		{
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaDlEnable", "1");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaUlEnable", "0");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoDlEnable", "1");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoUlEnable", "0");
		}
		else
		{
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaDlEnable", "0");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaUlEnable", "0");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoDlEnable", "0");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoUlEnable", "0");
		}
	}
	else
	{
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaDlEnable", "0");
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuOfdmaUlEnable", "0");
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoDlEnable", "0");
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoUlEnable", "0");
	}
#elif defined(RTCONFIG_MUMIMO_2G) || defined(RTCONFIG_MUMIMO_5G)
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
	if(band)
#endif
	{
		if(nvram_pf_get_int(prefix, "mumimo") == 1){
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MUTxRxEnable", "1");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoDlEnable", "1");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoUlEnable", "0");
		}else{
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MUTxRxEnable", "0");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoDlEnable", "0");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "MuMimoUlEnable", "0");
		}
	}
#endif
	fprintf(fp, "ApCliMuMimoDlEnable=%d\n", 0);
	fprintf(fp, "ApCliMuMimoUlEnable=%d\n", 0);
	fprintf(fp, "ApCliMuOfdmaUlEnable=%d\n", 0);
	fprintf(fp, "ApCliMuOfdmaDlEnable=%d\n", 0);
	fprintf(fp, "FixedTxMode=\n");

	if (ssid_num == 1)
		fprintf(fp, "TxRate=%d\n", 0);
	else
	{
		memset(tmpstr, 0, sizeof(tmpstr));

		for (i = 0; i < ssid_num; i++)
		{
			if (i)
				strlcat(tmpstr, ";", sizeof(tmpstr));

			strlcat(tmpstr, "0", sizeof(tmpstr));
		}
		fprintf(fp, "TxRate=%s\n", tmpstr);
	}

	//Channel
		int cht = 0;				
		for (i = 0; i < MAX_NO_MSSID; i++)
		{
			if (sw_mode == SW_MODE_REPEATER

	#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
				&& (wlc_express == 0 || (wlc_express - 1) != band)
	#else
				&& wlc_band == band
	#endif
				&& i != 1)
					continue;
				
			if (i && sw_mode == SW_MODE_REPEATER)
			{
				sprintf(prefix_mssid, "wl%d.%d_", band, i);

				if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
					continue;	
				cht =  nvram_get_int(strcat_r(prefix_mssid, "channel", temp));					
			}
			else {
				sprintf(prefix_mssid, "wl%d_", band);
				cht =  nvram_get_int(strcat_r(prefix_mssid, "channel", temp));	
			}
			
		}
		fprintf(fp, "Channel=%d\n", cht);
		
	//BasicRate
	if (!band)
	{
	/*
 	 * not supported in 5G mode
	 */
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
		if(nvram_pf_match(prefix, "nmode_x", "2") && !nvram_pf_match(prefix, "rateset", "default"))
				nvram_pf_set(prefix, "rateset", "default");
		else if(nvram_pf_match(prefix, "nmode_x", "1") && !nvram_pf_match(prefix, "rateset", "ofdm"))
				nvram_pf_set(prefix, "rateset", "ofdm");
		if(!nvram_pf_match(prefix, "nmode_x", "2") && nvram_pf_match(prefix, "rateset", "ofdm")){
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "mgmrateset1", "1-2-0");
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "mgmrateset2", "2-2-0");
		}
		if(nvram_pf_match(prefix, "rateset", "ofdm"))
			fprintf(fp, "BasicRate=%d\n", 0);
#else
		str = nvram_safe_get(strcat_r(prefix, "rateset", tmp));
		if (str && *str)
		{
			if (!strcmp(str, "default"))	// 1, 2, 5.5, 11
				fprintf(fp, "BasicRate=%d\n", 15);
			else if (!strcmp(str, "all"))	// 1, 2, 5.5, 6, 11, 12, 24
				fprintf(fp, "BasicRate=%d\n", 351);
			else if (!strcmp(str, "12"))	// 1, 2
				fprintf(fp, "BasicRate=%d\n", 3);
			else
				fprintf(fp, "BasicRate=%d\n", 15);
		}
		else
		{
			warning = 9;
			fprintf(fp, "BasicRate=%d\n", 15);
		}
#endif
	}

	//BeaconPeriod
	str = nvram_pf_get(prefix, "bcn");
	if (str && *str)
	{
		if (atoi(str) > 1000 || atoi(str) < 20)
		{
			nvram_pf_set(prefix, "bcn", "100");
			fprintf(fp, "BeaconPeriod=%d\n", 100);
		}
		else
			fprintf(fp, "BeaconPeriod=%d\n", atoi(str));
	}
	else
	{
		warning = 10;
		fprintf(fp, "BeaconPeriod=%d\n", 100);
	}

	//DTIM Period
	str = nvram_pf_get(prefix, "dtim");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "DtimPeriod", str);
	else
	{
		warning = 11;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "DtimPeriod", "1");
	}

	//TxPower
	str = nvram_pf_get(prefix, "txpower");
	if (nvram_pf_match(prefix, "radio", "0"))
		fprintf(fp, "TxPower=%d\n", 0);
	else if (str && *str)
		fprintf(fp, "TxPower=%d\n", atoi(str));
	else
	{
		warning = 12;
		fprintf(fp, "TxPower=%d\n", 100);
	}

	//DisableOLBC
	fprintf(fp, "DisableOLBC=%d\n", 0);

	//BGProtection
	str = nvram_pf_get(prefix, "gmode_protection");
	if (str && *str)
	{
		if (!strcmp(str, "auto"))
			fprintf(fp, "BGProtection=%d\n", 0);
		else
			fprintf(fp, "BGProtection=%d\n", 2);
	}
	else
	{
		warning = 13;
		fprintf(fp, "BGProtection=%d\n", 0);
	}

	//TxAntenna
//	fprintf(fp, "TxAntenna=\n");

	//RxAntenna
//	fprintf(fp, "RxAntenna=\n");

	//TxPreamble
	str = nvram_pf_get(prefix, "plcphdr");
	if (str && strcmp(str, "long") == 0)
		fprintf(fp, "TxPreamble=%d\n", 0);
	else if (str && strcmp(str, "short") == 0)
		fprintf(fp, "TxPreamble=%d\n", 1);
	else	/* auto mode applicable for STA only */
		fprintf(fp, "TxPreamble=%d\n", 0);

	//RTSThreshold  Default=2347
	str = nvram_pf_get(prefix, "rts");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "RTSThreshold", str);
	else
	{
		warning = 14;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "RTSThreshold", "2347");
	}

	//FragThreshold  Default=2346
	str = nvram_pf_get(prefix, "frag");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "FragThreshold", str);
	else
	{
		warning = 15;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "FragThreshold", "2346");
	}

	//TxBurst
	str = nvram_pf_get(prefix, "frameburst");
	if(nvram_match("reg_spec", "CE") || nvram_match("reg_spec", "EAC"))
		fprintf(fp, "TxBurst=0\n");
	else
		fprintf(fp, "TxBurst=%d\n", strcmp(str, "off") ? 1 : 0);

	//PktAggregate
	str = nvram_pf_get(prefix, "PktAggregate");
	if (str && *str)
		fprintf(fp, "PktAggregate=%d\n", atoi(str));
	else
	{
		warning = 17;
		fprintf(fp, "PktAggregate=%d\n", 1);
	}

	fprintf(fp, "FreqDelta=%d\n", 0);
#if defined (RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7603E_AP)
	fprintf(fp, "TurboRate=%d\n", 0);
#endif

	//WmmCapable
	if ((nvram_pf_get_int(prefix, "nmode_x") == 1 || nvram_pf_get_int(prefix, "nmode_x") == 1) && !nvram_pf_match(prefix, "wme", "on"))
		nvram_pf_set(prefix, "wme", "on");
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "WmmCapable", nvram_pf_match(prefix, "wme", "off") ? "0" : "1");

	fprintf(fp, "APAifsn=3;7;1;1\n");
	fprintf(fp, "APCwmin=4;4;3;2\n");
	fprintf(fp, "APCwmax=6;10;4;3\n");
	fprintf(fp, "APTxop=0;0;94;47\n");
	fprintf(fp, "APACM=0;0;0;0\n");
	fprintf(fp, "BSSAifsn=3;7;2;2\n");
	fprintf(fp, "BSSCwmin=4;4;3;2\n");
	fprintf(fp, "BSSCwmax=10;10;4;3\n");
	fprintf(fp, "BSSTxop=0;0;94;47\n");
	fprintf(fp, "BSSACM=0;0;0;0\n");

	//AckPolicy
	enum_sname_mvalue_w_fixed_value(fp, 4, "AckPolicy", nvram_pf_match(prefix, "wme_no_ack", "on") ? "1" : "0");

	r = snprintf(prefix, sizeof(prefix), "wl%d_", band);
	if(unlikely(r < 0))
		dbg("snprintf failed\n");
//	snprintf(prefix2, sizeof(prefix2), "wl%d_", band);

	//APSDCapable UAPSDCapable
	str = nvram_pf_get(prefix, "wme_apsd");
	if (str && *str){
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "APSDCapable", strcmp(str, "off") ? "1" : "0");
		fprintf(fp, "UAPSDCapable=%d\n", strcmp(str, "off") ? 1 : 0);
		fprintf(fp, "ApCliAPSDCapable=%d\n", strcmp(str, "off") ? 1 : 0);
	}
	else
	{
		warning = 18;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "APSDCapable", "1");
		fprintf(fp, "UAPSDCapable=%d\n", 1);
		fprintf(fp, "ApCliAPSDCapable=%d\n", 1);
	}

	//DLSDCapable
	str = nvram_pf_get(prefix, "DLSCapable");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "DLSCapable", str);
	else
	{
		warning = 19;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "DLSCapable", "0");
	}

	//NoForwarding pre SSID & NoForwardingBTNBSSID
	memset(tmpstr, 0x0, sizeof(tmpstr));

	str = nvram_pf_get(prefix, "ap_isolate");
	if (str && *str)
	{
		for (i = 0; i < ssid_num; i++)
		{
			if (i)
				strlcat(tmpstr, ";", sizeof(tmpstr));

			strlcat(tmpstr, str, sizeof(tmpstr));
		}
		fprintf(fp, "NoForwarding=%s\n", tmpstr);
		fprintf(fp, "NoForwardingMBCast=%s\n", tmpstr);
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RALINK_DBDC_MODE) || defined(RTCONFIG_MT798X)
		fprintf(fp, "NoForwardingBTNBSSID=0\n");
#else
		fprintf(fp, "NoForwardingBTNBSSID=%d\n", atoi(str));
#endif
	}
	else
	{
		warning = 20;

		for (i = 0; i < ssid_num; i++)
		{
			if (i)
				strlcat(tmpstr, ";", sizeof(tmpstr));

			strlcat(tmpstr, "0", sizeof(tmpstr));
		}
		fprintf(fp, "NoForwarding=%s\n", tmpstr);
		fprintf(fp, "NoForwardingMBCast=%s\n", tmpstr);
		fprintf(fp, "NoForwardingBTNBSSID=%d\n", 0);
	}

	//HideSSID
	memset(tmpstr, 0x0, sizeof(tmpstr));

	for (i = 0; i < MAX_NO_MSSID; i++)
	{
		if (i)
		{
			sprintf(prefix_mssid, "wl%d.%d_", band, i);

			if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
				continue;

			strlcat(tmpstr, ";", sizeof(tmpstr));
		}
		else
			sprintf(prefix_mssid, "wl%d_", band);

		str = nvram_pf_get(prefix_mssid, "closed");
		strlcat(tmpstr, str, sizeof(tmpstr));
	}
	fprintf(fp, "HideSSID=%s\n", tmpstr);

	//ShortSlot
	fprintf(fp, "ShortSlot=%d\n", nvram_pf_match(prefix, "nmode_x", "1") == 0);

	//AutoChannelSelect
/*
0: Disable
1: Old CSA (AP count)
2: New CSA (CCA)
3: MT7615/MT7915 CSA (Busy time)
*/
	{
		str = nvram_pf_get(prefix, "channel");

		if ((sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WISP)
			|| wisp_mode()
#endif
			)
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
			&& wlc_band == band
#endif
			)
		{ 
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
			fprintf(fp, "AutoChannelSelect=%d\n", 3);
#else
			fprintf(fp, "AutoChannelSelect=%d\n", 2);
#endif
#ifdef RTCONFIG_RALINK_DFS
			if(band){
					snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d.1_", band);
					if(nvram_pf_get(prefix_mssid, "channel") == 0){
						memset(tmpstr, 0x0, sizeof(tmpstr));
						if ((nvram_pf_get_int(prefix_mssid, "bw") == 1) || (nvram_pf_get_int(prefix_mssid, "bw") == 3)){
							if(IEEE80211H){
								sprintf(tmpstr,"%d;%d;%d;%d",116,132,136,140);
								fprintf(fp,"AutoChannelSkipList=%s\n",tmpstr);
							}
						}
						else if (nvram_pf_get_int(prefix_mssid, "bw") == 2){
							if(IEEE80211H){
								sprintf(tmpstr,"%d;%d",116,140);
								fprintf(fp,"AutoChannelSkipList=%s\n",tmpstr);
							}
						}
					}
				}
#endif	/* RTCONFIG_RALINK_DFS */
		}
		else if (str && *str)
		{
			if (atoi(str) == 0)
			{
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
				fprintf(fp, "AutoChannelSelect=%d\n", 3);
#else
				fprintf(fp, "AutoChannelSelect=%d\n", 2);
#endif
				memset(tmpstr, 0x0, sizeof(tmpstr));
				if (band && nvram_pf_get_int(prefix, "bw") > 0) {
					sprintf(tmpstr,"%d",165);// skip 165 in A band when bw setting to 20/40Mhz or 40Mhz.

#ifdef RTCONFIG_RALINK_DFS
					if(IEEE80211H){
						if((nvram_pf_get_int(prefix, "bw")==1) || (nvram_pf_get_int(prefix, "bw")==3)){
							if(!nvram_match("reg_spec", "EAC"))
							{
								if(!strncmp(tcode, "JP", 2))
									strlcat(tmpstr, ";132;136;140", sizeof(tmpstr));
//									sprintf(tmpstr,"%s;%d;%d;%d", tmpstr ,132,136,140);	//skip 132 136 140 under auto mode					
								else
									strlcat(tmpstr, ";116;132;136;140", sizeof(tmpstr));
//									sprintf(tmpstr,"%s;%d;%d;%d;%d",tmpstr,116,132,136,140);	//skip 116 132 136 140 under auto mode					
							}				
						}
						else if(nvram_pf_get_int(prefix, "bw")==2){
							if(!strncmp(tcode, "JP", 2))
								strlcat(tmpstr, ";140", sizeof(tmpstr));
//								sprintf(tmpstr,"%s;%d",tmpstr,140);	//skip 140 under auto mode					
							else
								strlcat(tmpstr, ";116;140", sizeof(tmpstr));
//							sprintf(tmpstr,"%s;%d;%d",tmpstr,116,140);	//skip 116 140 under auto mode					
						}
					}
#endif	/* RTCONFIG_RALINK_DFS */
				}

#ifdef RTCONFIG_MTK_TW_AUTO_BAND4 //NCC: for 5G BAND24 & BAND14
				if(band)
				{
					if(strlen(tmpstr))
						strlcat(tmpstr, ";", sizeof(tmpstr));
					//autochannel selection  but skip 5G band1 & band2, TW only
					if(
#if defined(RTCONFIG_TCODE)
					  !strncmp(tcode, "TW", 2) ||

#endif
#ifdef RTCONFIG_HAS_5G
					   nvram_match("wl_reg_5g","5G_BAND24") ||
#endif
#if defined(RTCONFIG_NEW_REGULATION_DOMAIN)
					   (nvram_match("reg_spec","NCC") || nvram_match("reg_spec","NCC2"))
#else
					   (nvram_pf_match(prefix, "country_code", "TW") || nvram_pf_match(prefix, "country_code", "Z3"))
#endif
					 )
						strlcat(tmpstr, "36;40;44;48;52;56;60;64", sizeof(tmpstr));
				}
#endif
#if defined(RTCONFIG_NO_SELECT_CHANNEL)
			char *t_code_noselect_2G[]={"EU", "RU", "EE", "UK", "DE", "TR", "CZ", "JP", "SG", "CN", "UA", "KR", "AU"};
			char *t_code_noselect_5G[]={"CA", "TW", "EU", "RU", "EE", "UK", "DE", "TR", "CZ", "JP", "KR"};
			char *t_code_noselect3_5G[]={"UA"}; //5G_BAND123, Skip Band3 only
			char *reg_2g[]={"2G_CH11", "2G_CH13", "2G_CH14"};
			char *reg_5g[]={"5G_BAND14", "5G_BAND123", "5G_BAND1", "5G_BAND24", "5G_BAND4", "5G_BAND124"};	
	
			if(!band) {
				nvram_set("skip_channel_2g", "0");	//for GUI checkbox
				//2G_CH13, 2G No Selection T-Code, Skip channel 12, 13
				if(!strcmp(nvram_safe_get("wl_reg_2g"), &reg_2g[1][0])) { 
					for(i=0; i < (sizeof(t_code_noselect_2G)/sizeof(&t_code_noselect_2G[0][0])); i++) {
						if(!strncmp(tcode,&t_code_noselect_2G[i][0],2)) {
							if(atoi(nvram_safe_get("acs_ch13")) == 0) {
								memset(tmpstr, 0x00, sizeof(tmpstr));
								sprintf(tmpstr,"%d;%d",12,13);	
								fprintf(fp,"AutoChannelSkipList=%s\n",tmpstr);							
							}
							else
								fprintf(fp,"AutoChannelSkipList=%d\n", atoi(nvram_safe_get("acs_ch13")));
							nvram_set("skip_channel_2g", "CH13");	
						}
					}
				}
			}
			else {
				nvram_set("skip_channel_5g", "0");	//for GUI checkbox
				//5G_BAND14, 5G No Selection T-Code, skip band1
				if(!strcmp(nvram_safe_get("wl_reg_5g"), &reg_5g[0][0])) {   
					for(i=0; i < (sizeof(t_code_noselect_5G)/sizeof(&t_code_noselect_5G[0][0])); i++) {
						if(!strncmp(tcode,&t_code_noselect_5G[i][0],2)) {
							if(atoi(nvram_safe_get("acs_band1")) == 0) {
								memset(tmpstr, 0x00, sizeof(tmpstr));
								sprintf(tmpstr,"%d;%d;%d;%d",36,40,44,48);	
								fprintf(fp,"AutoChannelSkipList=%s\n", tmpstr);
							}
							else
								fprintf(fp,"AutoChannelSkipList=%d\n", atoi(nvram_safe_get("acs_band1")));
							nvram_set("skip_channel_5g", "band1");	
						}
					}
				}
				
				//5G_BAND123, 5G No Selection T-Code
				if(!strcmp(nvram_safe_get("wl_reg_5g"), &reg_5g[1][0])) 
				{ 
					//skip band3
					for(i=0; i < (sizeof(t_code_noselect3_5G)/sizeof(&t_code_noselect3_5G[0][0])); i++) {
						if(!strncmp(tcode,&t_code_noselect3_5G[i][0],2)) {
							if(atoi(nvram_safe_get("acs_band3")) == 0) {
								memset(tmpstr, 0x00, sizeof(tmpstr));
								sprintf(tmpstr,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d", 100,104,108,112,116,120,124,128,132,136,140);	
								fprintf(fp,"AutoChannelSkipList=%s\n",tmpstr);
							}
							else
								fprintf(fp,"AutoChannelSkipList=%d\n", atoi(nvram_safe_get("acs_band3")));
							nvram_set("skip_channel_5g", "band3");	
						}
					}
					//skip band2,band3		
					for(i=0; i < (sizeof(t_code_noselect_5G)/sizeof(&t_code_noselect_5G[0][0])); i++) {
						if(!strncmp(tcode,&t_code_noselect_5G[i][0],2)){
							memset(tmpstr, 0x00, sizeof(tmpstr));
							if(nvram_match("location_code", "RU"))
							{
								if(nvram_match("acs_dfs", "0"))
								{
									sprintf(tmpstr,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d", 100,104,108,112,116,120,124,128,132,136,140);	
									fprintf(fp,"AutoChannelSkipList=%s\n",tmpstr);	
								}
							}
							else
							{
								if(atoi(nvram_safe_get("acs_dfs")) == 0) {							
									sprintf(tmpstr,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",52,56,60,64,100,104,108,112,116,120,124,128,132,136,140);
									fprintf(fp,"AutoChannelSkipList=%s\n",tmpstr);
									nvram_set("skip_channel_5g", "band23");
								}
								else if(IEEE80211H)
								{
									if((nvram_pf_get_int(prefix, "bw")==1) || (nvram_pf_get_int(prefix, "bw")==3)){
										strlcat(tmpstr, ";116;132;136;140", sizeof(tmpstr));
//										sprintf(tmpstr,"%s;%d;%d;%d;%d",tmpstr,116,132,136,140);	//skip 116 132 136 140 under auto mode					
									}
									else if(nvram_pf_get_int(prefix, "bw")==2){
										strlcat(tmpstr, ";116;140", sizeof(tmpstr));
//										sprintf(tmpstr,"%s;%d;%d",tmpstr,116,140);	//skip 116 140 under auto mode					
									}
									fprintf(fp,"AutoChannelSkipList=%s\n",tmpstr);
									nvram_set("skip_channel_5g", "band23");
								}
							}
						}
					}
				}
				if(!strcmp(nvram_safe_get("wl_reg_5g"), &reg_5g[3][0]) || !strcmp(nvram_safe_get("wl_reg_5g"), &reg_5g[4][0])
					|| !strcmp(nvram_safe_get("wl_reg_5g"), &reg_5g[5][0])) 
				{
					memset(tmpstr, 0, sizeof(tmpstr));
					sprintf(tmpstr,"%d", 165);	//skip 165 under auto mode		
					fprintf(fp,"AutoChannelSkipList=%s\n",tmpstr);
				}
				if(!strcmp(nvram_safe_get("wl_reg_5g"), "5G_ALL"))
				{
					memset(tmpstr, 0, sizeof(tmpstr));
					for(i=0; i < (sizeof(t_code_noselect_5G)/sizeof(&t_code_noselect_5G[0][0])); i++) {
						if(!strncmp(tcode,&t_code_noselect_5G[i][0],2)) {
							if(IEEE80211H)
							{
								if((nvram_pf_get_int(prefix, "bw") == 1) || (nvram_pf_get_int(prefix, "bw") == 3)){
									strlcat(tmpstr, ";116;132;136;140", sizeof(tmpstr));
								}
								else if(nvram_pf_get_int(prefix, "bw") == 2){
									strlcat(tmpstr, ";116;140", sizeof(tmpstr));
								}
							}
							else
							{
								strlcat(tmpstr, ";116;120;124;128;165", sizeof(tmpstr));
							}
						}
					}
					fprintf(fp,"AutoChannelSkipList=%s\n", tmpstr);
				}
			}					
#else
				
        //only band 4 for auto channel select if support band 4
				if(band)
				{
					if (strchr(nvram_safe_get("wl_reg_5g"), '4')) {	//check band4 support?
						if (strchr(nvram_safe_get("wl_reg_5g"), '1')){	//skip band 1
				     		if(strlen(tmpstr))
								strlcat(tmpstr, ";", sizeof(tmpstr));
							strlcat(tmpstr, "36;40;44;48", sizeof(tmpstr));
						}
						if (strchr(nvram_safe_get("wl_reg_5g"), '2')) {	//skip band 2
              				if(strlen(tmpstr))
								strlcat(tmpstr, ";", sizeof(tmpstr));
							strlcat(tmpstr, "52;56;60;64", sizeof(tmpstr));
						}
					}
				}

				fprintf(fp,"AutoChannelSkipList=%s\n",tmpstr);			
#endif	
			}
			else
				fprintf(fp, "AutoChannelSelect=%d\n", 0);
		}
		else
		{
			warning = 21;
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
			fprintf(fp, "AutoChannelSelect=%d\n", 3);
#else
			fprintf(fp, "AutoChannelSelect=%d\n", 2);
#endif
		}
	}				

	//IEEE8021X
	memset(tmpstr, 0x0, sizeof(tmpstr));

	for (i = 0; i < MAX_NO_MSSID; i++)
	{
		if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
			&& wlc_band == band
#endif
			&& i != 1)
				continue;
		if (i)
		{
			sprintf(prefix_mssid, "wl%d.%d_", band, i);

			if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
				continue;
		}
		else
			sprintf(prefix_mssid, "wl%d_", band);

		if (nvram_pf_match(prefix_mssid, "auth_mode_x", "radius"))
			strlcat(tmpstr, "1;", sizeof(tmpstr));
		else
			strlcat(tmpstr, "0;", sizeof(tmpstr));
	}
	if ((i = strlen(tmpstr)) > 0)
	{
		tmpstr[ i-1 ] = '\0';
	}
	fprintf(fp, "IEEE8021X=%s\n", tmpstr);


	fprintf(fp, "IEEE80211H=%d\n", IEEE80211H);

#ifdef RTCONFIG_RALINK_DFS
	if (band) {
		if (IEEE80211H)
			fprintf(fp, "DfsEnable=%d\n", 1);
		else
			fprintf(fp, "DfsEnable=%d\n", 0);
		fprintf(fp, "DfsFalseAlarmPrevent=%d\n", 1);
		fprintf(fp, "DfsZeroWaitCacTime=255\n");
	}
#endif
	if (nvram_pf_match(prefix, "country_code", "JP"))
	{
		fprintf(fp, "RDRegion=%s\n", "JAP");
	}
	else
	{
		fprintf(fp, "RDRegion=FCC\n");
	
	}
	fprintf(fp, "DfsZeroWaitDefault=%d\n", 0);
	fprintf(fp, "DfsDedicatedZeroWait=%d\n", 0);
	fprintf(fp, "CarrierDetect=%d\n", 0);
//	if (band)
#if defined (RTCONFIG_WLMODULE_MT7615E_AP)
	fprintf(fp, "ChannelGeography=%d\n", 2);
	fprintf(fp, "PhyRateLimit=%d\n", 0);
#endif
	fprintf(fp, "PreAntSwitch=\n");
	fprintf(fp, "DebugFlags=%d\n", 0);
	fprintf(fp, "FineAGC=%d\n", 0);
	fprintf(fp, "StreamMode=%d\n", 0);
	fprintf(fp, "StreamModeMac0=\n");
	fprintf(fp, "StreamModeMac1=\n");
	fprintf(fp, "StreamModeMac2=\n");
	fprintf(fp, "StreamModeMac3=\n");
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "StationKeepAlive", "0");
#if defined (RTCONFIG_WLMODULE_MT7615E_AP) || (RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	fprintf(fp, "CSPeriod=6\n");
#else
	fprintf(fp, "CSPeriod=10\n");
#endif
	fprintf(fp, "DfsLowerLimit=%d\n", 0);
	fprintf(fp, "DfsUpperLimit=%d\n", 0);
	fprintf(fp, "DfsIndoor=%d\n", 0);
	fprintf(fp, "DFSParamFromConfig=%d\n", 0);
	fprintf(fp, "FCCParamCh0=\n");
	fprintf(fp, "FCCParamCh1=\n");
	fprintf(fp, "FCCParamCh2=\n");
	fprintf(fp, "FCCParamCh3=\n");
	fprintf(fp, "CEParamCh0=\n");
	fprintf(fp, "CEParamCh1=\n");
	fprintf(fp, "CEParamCh2=\n");
	fprintf(fp, "CEParamCh3=\n");
	fprintf(fp, "JAPParamCh0=\n");
	fprintf(fp, "JAPParamCh1=\n");
	fprintf(fp, "JAPParamCh2=\n");
	fprintf(fp, "JAPParamCh3=\n");
	fprintf(fp, "JAPW53ParamCh0=\n");
	fprintf(fp, "JAPW53ParamCh1=\n");
	fprintf(fp, "JAPW53ParamCh2=\n");
	fprintf(fp, "JAPW53ParamCh3=\n");
	fprintf(fp, "FixDfsLimit=%d\n", 0);
	fprintf(fp, "LongPulseRadarTh=%d\n", 0);
	fprintf(fp, "AvgRssiReq=%d\n", 0);
	fprintf(fp, "DFS_R66=%d\n", 0);
	fprintf(fp, "BlockCh=\n");

	//GreenAP
#if defined(RTCONFIG_MTK_REP)
	/// MT7620 GreenAP will impact TSSI, force to disable GreenAP here..
	//  MT7620 GreenAP cause bad site survey result on RTAC52 2G.
	{
		fprintf(fp, "GreenAP=%d\n", 0);
	}
#else
	str = nvram_pf_get(prefix, "GreenAP");
	if (str && *str)
		fprintf(fp, "GreenAP=%d\n", atoi(str));
	else
	{
		warning = 22;
		fprintf(fp, "GreenAP=%d\n", 0);
	}
#endif

	//PreAuth
	memset(tmpstr, 0x0, sizeof(tmpstr));
	for (i = 0; i < ssid_num; i++)
	{
		if (i)
			r = snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d.%d_", band, i);
		else
			r = snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d_", band);

		if(unlikely(r < 0))
			dbg("snprintf failed\n");

		if (i)
			strlcat(tmpstr, ";", sizeof(tmpstr));
		strlcat(tmpstr, strstr(nvram_pf_safe_get(prefix_mssid, "auth_mode_x"), "wpa") ? "1" : "0", sizeof(tmpstr));
	}
	fprintf(fp, "PreAuth=%s\n", tmpstr);

	//AuthMode
	memset(tmpstr, 0x0, sizeof(tmpstr));

	for (i = 0; i < MAX_NO_MSSID; i++)
	{
		if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
			&& wlc_band == band
#endif
			&& i != 1)
				continue;
		if (i)
		{
			sprintf(prefix_mssid, "wl%d.%d_", band, i);

			if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
				continue;

			if(strlen(tmpstr))
				strlcat(tmpstr, ";", sizeof(tmpstr));
		}
		else
			sprintf(prefix_mssid, "wl%d_", band);

		str = nvram_pf_safe_get(prefix_mssid, "auth_mode_x");
		if (str && strlen(str))
		{
			if (!strcmp(str, "open"))
			{
				strlcat(tmpstr, "OPEN", sizeof(tmpstr));
			}
			else if (!strcmp(str, "shared"))
			{
				strlcat(tmpstr, "SHARED", sizeof(tmpstr));
			}
			else if (!strcmp(str, "psk"))
			{
				strlcat(tmpstr, "WPAPSK", sizeof(tmpstr));
			}
			else if (!strcmp(str, "psk2"))
			{
				strlcat(tmpstr, "WPA2PSK", sizeof(tmpstr));
			}
			else if (!strcmp(str, "pskpsk2"))
			{
				strlcat(tmpstr, "WPAPSKWPA2PSK", sizeof(tmpstr));
			}
			else if (!strcmp(str, "wpa"))
			{
				strlcat(tmpstr, "WPA", sizeof(tmpstr));
				flag_8021x = 1;
			}
			else if (!strcmp(str, "wpa2"))
			{
				strlcat(tmpstr, "WPA2", sizeof(tmpstr));
				flag_8021x = 1;
			}
			else if (!strcmp(str, "wpawpa2"))
			{
				strlcat(tmpstr, "WPA1WPA2", sizeof(tmpstr));
				flag_8021x = 1;
			}
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
			else if (!strcmp(str, "sae"))
			{
				strlcat(tmpstr, "WPA3PSK", sizeof(tmpstr));
				flag_8021x = 1;
			}
			else if (!strcmp(str, "psk2sae"))
			{
				strlcat(tmpstr, "WPA2PSKWPA3PSK", sizeof(tmpstr));
				flag_8021x = 1;
			}
#endif
			else if ((!strcmp(str, "radius")))
			{
				strlcat(tmpstr, "OPEN", sizeof(tmpstr));
				flag_8021x = 1;
			}
			else
			{
				warning = 23;
				strlcat(tmpstr, "OPEN", sizeof(tmpstr));
			}
		}
		else
		{
			warning = 24;
			strlcat(tmpstr, "OPEN", sizeof(tmpstr));
		}
	}
	fprintf(fp, "AuthMode=%s\n", tmpstr);

	//EncrypType
	memset(tmpstr, 0x0, sizeof(tmpstr));

	for (i = 0; i < MAX_NO_MSSID; i++)
	{
		if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
			&& wlc_band == band
#endif
			&& i != 1)
				continue;
		if (i)
		{
			sprintf(prefix_mssid, "wl%d.%d_", band, i);

			if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
				continue;
			if(strlen(tmpstr))
				strlcat(tmpstr, ";", sizeof(tmpstr));
		}
		else
			sprintf(prefix_mssid, "wl%d_", band);

		if ((nvram_pf_match(prefix_mssid, "auth_mode_x", "open") && nvram_pf_match(prefix_mssid, "wep_x", "0")))
			strlcat(tmpstr, "NONE", sizeof(tmpstr));
		else if ((nvram_pf_match(prefix_mssid, "auth_mode_x", "open") && nvram_pf_invmatch(prefix_mssid, "wep_x", "0")) ||
				nvram_pf_match(prefix_mssid, "auth_mode_x", "shared") || nvram_pf_match(prefix_mssid, "auth_mode_x", "radius"))
			strlcat(tmpstr, "WEP", sizeof(tmpstr));
		else if (nvram_pf_match(prefix_mssid, "crypto", "tkip"))
		{
			strlcat(tmpstr, "TKIP", sizeof(tmpstr));
		}
		else if (nvram_pf_match(prefix_mssid, "crypto", "aes"))
		{
			strlcat(tmpstr, "AES", sizeof(tmpstr));
		}
		else if (nvram_pf_match(prefix_mssid, "crypto", "tkip+aes"))
		{
			strlcat(tmpstr, "TKIPAES", sizeof(tmpstr));
		}
		else
		{
			warning = 25;
			strlcat(tmpstr, "NONE", sizeof(tmpstr));
		}
	}
	fprintf(fp, "EncrypType=%s\n", tmpstr);
#if defined (RTCONFIG_WLMODULE_MT7615E_AP)
	fprintf(fp, "WapiPsk1=\n");
	fprintf(fp, "WapiPsk2=\n");
	fprintf(fp, "WapiPsk3=\n");
	fprintf(fp, "WapiPsk4=\n");
	fprintf(fp, "WapiPsk5=\n");
	fprintf(fp, "WapiPsk6=\n");
	fprintf(fp, "WapiPsk7=\n");
	fprintf(fp, "WapiPsk8=\n");
	fprintf(fp, "WapiPskType=\n");
	fprintf(fp, "Wapiifname=\n");
	fprintf(fp, "WapiAsCertPath=\n");
	fprintf(fp, "WapiUserCertPath=\n");
	fprintf(fp, "WapiAsIpAddr=\n");
	fprintf(fp, "WapiAsPort=\n");
#endif
	//RekeyInterval
	str = nvram_pf_safe_get(prefix, "wpa_gtk_rekey");
	if (str && *str)
	{
		if (atol(str) == 0)
			fprintf(fp, "RekeyMethod=%s\n", "DISABLE");
		else
			fprintf(fp, "RekeyMethod=TIME\n");
	}
	else
	{
		warning = 26;
		fprintf(fp, "RekeyMethod=%s\n", "DISABLE");
	}

	memset(tmpstr, 0x0, sizeof(tmpstr));
	for (i = 0; i < MAX_NO_MSSID; i++)
	{
		if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
			&& wlc_band == band
#endif
			&& i != 1)
				continue;
		if (i)
			strlcat(tmpstr, ";", sizeof(tmpstr));
		if(str && *str)
			strlcat(tmpstr, str, sizeof(tmpstr));
		else
			strlcat(tmpstr, "0", sizeof(tmpstr));
	}
	fprintf(fp, "RekeyInterval=%s\n", tmpstr);
#if defined(RTCONFIG_MFP)
	{
		char PMFMFPC[128] = {0}, PMFMFPR[128] = {0}, PMFSHA256[128] = {0};
		memset(tmpstr, 0x0, sizeof(tmpstr));
		str = nvram_pf_safe_get(prefix, "auth_mode_x");
		if(aimesh_re_node()){
			if(!strcmp(str, "sae"))
				nvram_pf_set_int(prefix, "mfp", 2);
			else if(!strcmp(str, "psk2sae"))
				nvram_pf_set_int(prefix, "mfp", 1);
			else
				nvram_pf_set_int(prefix, "mfp", 0);
		}
		else if (!strcmp(str, "open") || !strcmp(str, "shared") || !strcmp(str, "radius"))
			nvram_pf_set_int(prefix, "mfp", 0);
		else if((!strcmp(str, "wpa") || !strcmp(str, "wpa2") || !strcmp(str, "wpawpa2") || !strcmp(str, "psk") || !strcmp(str, "psk2") || !strcmp(str, "pskpsk2"))){
			if(nvram_pf_get_int(prefix, "mfp") == 2)
				nvram_pf_set_int(prefix, "mfp", 1);
		}

		for (i = 0; i < MAX_NO_MSSID; i++){
			if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
				&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
				&& wlc_band == band
#endif
				&& i != 1)
					continue;
			if (i)
				sprintf(prefix_mssid, "wl%d.%d_", band, i);
			else
				sprintf(prefix_mssid, "wl%d_", band);
	
			if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
				continue;
			if(i){
				strlcat(PMFMFPC, ";", sizeof(PMFMFPC));
				strlcat(PMFMFPR, ";", sizeof(PMFMFPR));
				strlcat(PMFSHA256, ";", sizeof(PMFSHA256));
			}
			if(nvram_pf_get_int(prefix_mssid, "mfp") == 2){//sae
				strlcat(PMFMFPC, "1", sizeof(PMFMFPC));
				strlcat(PMFMFPR, "1", sizeof(PMFMFPR));
				strlcat(PMFSHA256, "1", sizeof(PMFSHA256));
			}else{
				strlcat(PMFMFPC, "0", sizeof(PMFMFPC));
				strlcat(PMFMFPR, "0", sizeof(PMFMFPR));
				strlcat(PMFSHA256, "0", sizeof(PMFSHA256));
			}
		}
		fprintf(fp, "PMFMFPC=%s\n", PMFMFPC);
		fprintf(fp, "PMFMFPR=%s\n", PMFMFPR);
		fprintf(fp, "PMFSHA256=%s\n", PMFSHA256);
	}
#endif
	//PMKCachePeriod (in minutes)
	str = nvram_pf_safe_get(prefix, "pmk_cache");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "PMKCachePeriod", str);
	else
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "PMKCachePeriod", "10");

	fprintf(fp, "MeshAutoLink=%d\n", 0);
	fprintf(fp, "MeshAuthMode=\n");
	fprintf(fp, "MeshEncrypType=\n");
	fprintf(fp, "MeshDefaultkey=%d\n", 0);
	fprintf(fp, "MeshWEPKEY=\n");
	fprintf(fp, "MeshWPAKEY=\n");
	fprintf(fp, "MeshId=\n");
	if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
			&& wlc_band == band
#endif
			)
	{
		if(band == 0)
			sprintf(tmpstr, "WPAPSK1=%s\n", nvram_safe_get("wl0.1_wpa_psk"));
		else
			sprintf(tmpstr, "WPAPSK1=%s\n", nvram_safe_get("wl1.1_wpa_psk"));
	 	fprintf(fp, "%s", tmpstr);
	}
	else
	{
		//WPAPSK
		for (i = 0, j = 0; i < MAX_NO_MSSID; i++)
		{
			if (i)
			{
				sprintf(prefix_mssid, "wl%d.%d_", band, i);

				if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);

			sprintf(tmpstr, "WPAPSK%d=%s\n", j + 1, nvram_pf_safe_get(prefix_mssid, "wpa_psk"));
			fprintf(fp, "%s", tmpstr);
		}
	}
	for (i = ssid_num; i < 8; i++)
	{
		sprintf(tmpstr, "WPAPSK%d=\n", i + 1);
		fprintf(fp, "%s", tmpstr);
	}

	//DefaultKeyID

	memset(tmpstr, 0x0, sizeof(tmpstr));

	for (i = 0; i < MAX_NO_MSSID; i++)
	{
		if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
			&& wlc_band == band
#endif
			&& i != 1)
				continue;
		if (i)
		{
			sprintf(prefix_mssid, "wl%d.%d_", band, i);

			if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
				continue;

			if(strlen(tmpstr))
				strlcat(tmpstr, ";", sizeof(tmpstr));
		}
		else
			sprintf(prefix_mssid, "wl%d_", band);

		str = nvram_pf_safe_get(prefix_mssid, "key");
		strlcat(tmpstr, str, sizeof(tmpstr));
	}
	fprintf(fp, "DefaultKeyID=%s\n", tmpstr);

	list[0] = 0;
	list[1] = 0;
	memset(wl_key_type, 0x0, sizeof(wl_key_type));

	for (i = 0, j = 0; i < MAX_NO_MSSID; i++)
	{

		if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
			&& wlc_band == band
#endif
			&& i != 1)
				continue;

		if (i)
			snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d.%d_", band, i);
		else
			snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d_", band);

		if ((!i) ||
			(i && (nvram_pf_match(prefix_mssid, "bss_enabled", "1"))))
		{
			str = strcat_r(prefix_mssid, "key", temp);
			str2 = nvram_safe_get(str);
			sprintf(list, "%s%s", str, str2);

			if ((strlen(nvram_safe_get(list)) == 5) || (strlen(nvram_safe_get(list)) == 13))
			{
				wl_key_type[j] = 1;
				warning = 271;
			}
			else if ((strlen(nvram_safe_get(list)) == 10) || (strlen(nvram_safe_get(list)) == 26))
			{
				wl_key_type[j] = 0;
				warning = 272;
			}
			else if ((strlen(nvram_safe_get(list)) != 0))
			{
				warning = 273;
			}

			j++;
		}
	}

	// Key1Type(0 -> Hex, 1->Ascii)
	memset(tmpstr, 0x0, sizeof(tmpstr));

	for (i = 0; i < ssid_num; i++)
	{
		if (i)
			strlcat(tmpstr, ";", sizeof(tmpstr));
		if(wl_key_type[i] == 1)
			strlcat(tmpstr, "1", sizeof(tmpstr));
		else
			strlcat(tmpstr, "0", sizeof(tmpstr));
	}
	fprintf(fp, "Key1Type=%s\n", tmpstr);


	// Key1Str
	if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
		&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
		&& wlc_band == band
#endif
		)
	{
		if (band == 0)
			sprintf(tmpstr, "Key1Str1=%s\n", nvram_safe_get("wl0.1_key1"));
		else
			sprintf(tmpstr, "Key1Str1=%s\n", nvram_safe_get("wl1.1_key1"));
	 	fprintf(fp, "%s", tmpstr);
	}
	else
	{
		for (i = 0, j = 0; i < MAX_NO_MSSID; i++)
		{
			if (i)
			{
				sprintf(prefix_mssid, "wl%d.%d_", band, i);

				if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);

			sprintf(tmpstr, "Key1Str%d=%s\n", j + 1, nvram_pf_safe_get(prefix_mssid, "key1"));
			fprintf(fp, "%s", tmpstr);
		}
	}
	for (i = ssid_num; i < 8; i++)
	{
		sprintf(tmpstr, "Key1Str%d=\n", i + 1);
		fprintf(fp, "%s", tmpstr);
	}

	// Key2Type
	memset(tmpstr, 0x0, sizeof(tmpstr));

	for (i = 0; i < ssid_num; i++)
	{
		if (i)
			strlcat(tmpstr, ";", sizeof(tmpstr));

		if(wl_key_type[i] == 1)
			strlcat(tmpstr, "1", sizeof(tmpstr));
		else
			strlcat(tmpstr, "0", sizeof(tmpstr));
	}
	fprintf(fp, "Key2Type=%s\n", tmpstr);

	// Key2Str
	if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
		&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
		&& wlc_band == band
#endif
		)
	{
		if (band == 0)
			sprintf(tmpstr, "Key2Str1=%s\n", nvram_safe_get("wl0.1_key2"));
		else
			sprintf(tmpstr, "Key2Str1=%s\n", nvram_safe_get("wl1.1_key2"));
	 	fprintf(fp, "%s", tmpstr);
	}
	else
	{
		for (i = 0, j = 0; i < MAX_NO_MSSID; i++)
		{
			if (i)
			{
				sprintf(prefix_mssid, "wl%d.%d_", band, i);

				if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);

			sprintf(tmpstr, "Key2Str%d=%s\n", j + 1, nvram_pf_safe_get(prefix_mssid, "key2"));
			fprintf(fp, "%s", tmpstr);
		}
	}
	for (i = ssid_num; i < 8; i++)
	{
		sprintf(tmpstr, "Key2Str%d=\n", i + 1);
		fprintf(fp, "%s", tmpstr);
	}

	// Key3Type
	memset(tmpstr, 0x0, sizeof(tmpstr));

	for (i = 0; i < ssid_num; i++)
	{
		if (i)
			strlcat(tmpstr, ";", sizeof(tmpstr));

		if(wl_key_type[i] == 1)
			strlcat(tmpstr, "1", sizeof(tmpstr));
		else
			strlcat(tmpstr, "0", sizeof(tmpstr));
	}
	fprintf(fp, "Key3Type=%s\n", tmpstr);

	// Key3Str
	if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
		&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
		&& wlc_band == band
#endif
		)
	{
		if(band == 0)
			sprintf(tmpstr, "Key3Str1=%s\n", nvram_safe_get("wl0.1_key3"));
		else
			sprintf(tmpstr, "Key3Str1=%s\n", nvram_safe_get("wl1.1_key3"));
	 	fprintf(fp, "%s", tmpstr);
	}
	else
	{
		for (i = 0, j = 0; i < MAX_NO_MSSID; i++)
		{
			if (i)
			{
				sprintf(prefix_mssid, "wl%d.%d_", band, i);

				if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);

			sprintf(tmpstr, "Key3Str%d=%s\n", j + 1, nvram_pf_safe_get(prefix_mssid, "key3"));
			fprintf(fp, "%s", tmpstr);
		}
	}
	for (i = ssid_num; i < 8; i++)
	{
		sprintf(tmpstr, "Key3Str%d=\n", i + 1);
		fprintf(fp, "%s", tmpstr);
	}

	// Key4Type
	memset(tmpstr, 0x0, sizeof(tmpstr));

	for (i = 0; i < ssid_num; i++)
	{
		if (i)
			strlcat(tmpstr, ";", sizeof(tmpstr));

		if(wl_key_type[i] == 1)
			strlcat(tmpstr, "1", sizeof(tmpstr));
		else
			strlcat(tmpstr, "0", sizeof(tmpstr));
	}
	fprintf(fp, "Key4Type=%s\n", tmpstr);

	// Key4Str

	if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
		&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
		&& wlc_band == band
#endif
		)
	{
		if (band == 0)
			sprintf(tmpstr, "Key4Str1=%s\n", nvram_safe_get("wl0.1_key4"));
		else
			sprintf(tmpstr, "Key4Str1=%s\n", nvram_safe_get("wl1.1_key4"));
	 	fprintf(fp, "%s", tmpstr);
	}
	else
	{
		for (i = 0, j = 0; i < MAX_NO_MSSID; i++)
		{
			if (i)
			{
				sprintf(prefix_mssid, "wl%d.%d_", band, i);

				if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);

			sprintf(tmpstr, "Key4Str%d=%s\n", j + 1, nvram_pf_safe_get(prefix_mssid, "key4"));
			fprintf(fp, "%s", tmpstr);
		}
	}
	for (i = ssid_num; i < 8; i++)
	{
		sprintf(tmpstr, "Key4Str%d=\n", i + 1);
		fprintf(fp, "%s", tmpstr);
	}

	fprintf(fp, "HSCounter=%d\n", 0);

	//HT_HTC
	str = nvram_safe_get(strcat_r(prefix, "HT_HTC", tmp));
	if (str && *str)
		fprintf(fp, "HT_HTC=%d\n", atoi(str));
	else
	{
		warning = 28;
		fprintf(fp, "HT_HTC=%d\n", 1);
	}

#if defined(RTCONFIG_EASYMESH)
//	fprintf(fp, "VLANTag=%d\n", is_wgn_enabled());	
//	if(mesh_re_node())
//		fprintf(fp, "STAVLANTag=%d\n", is_wgn_enabled());	
#elif defined(RTCONFIG_MULTILAN_CFG)
	fprintf(fp, "VLANTag=%d\n", sdn_enable());
	if(mesh_re_node())
		fprintf(fp, "STAVLANTag=%d\n", sdn_enable());
#endif

	//HT_RDG
	str = nvram_pf_safe_get(prefix, "HT_RDG");
#ifdef CE_ADAPTIVITY
	if (nvram_match("reg_spec", "CE"))
		fprintf(fp, "HT_RDG=%d\n", 0);
	else
#endif	/* CE_ADAPTIVITY */
	if (str && strlen(str))
		fprintf(fp, "HT_RDG=%d\n", atoi(str));
	else
	{
		warning = 29;
		fprintf(fp, "HT_RDG=%d\n", 0);
	}
#if 0
	//HT_LinkAdapt
	str = nvram_safe_get(strcat_r(prefix, "HT_LinkAdapt", tmp));
	if (str && *str)
		fprintf(fp, "HT_LinkAdapt=%d\n", atoi(str));
	else
	{
		warning = 30;
		fprintf(fp, "HT_LinkAdapt=%d\n", 1);
	}
#endif
	//HT_OpMode
	str = nvram_pf_safe_get(prefix, "mimo_preamble");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_OpMode", strcmp(str, "mm") ? "1" : "0");
	else
	{
		warning = 31;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_OpMode", "0");
	}

	//HT_MpduDensity
	str = nvram_pf_safe_get(prefix, "HT_MpduDensity");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_MpduDensity", str);
	else
	{
		warning = 32;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_MpduDensity", "5");
	}

	int Channel = nvram_pf_get_int(prefix, "channel");
	int EXTCHA = 0;
	int EXTCHA_MAX = 0;
	int HTBW_MAX = 1;
	int wl_bw = get_bw_via_channel(band, Channel);

	if (band)
	{
		if (Channel != 0)
		{
			switch (Channel)
			{
			case 36:
			case 44:
			case 52:
			case 60:
			case 100:
			case 108:
			case 116:
			case 124:
			case 132:
			case 149:
			case 157:
				EXTCHA = 1;
				break;
			case 40:
			case 48:
			case 56:
			case 64:
			case 104:
			case 112:
			case 120:
			case 128:
			case 136:
			case 153:
			case 161:
				EXTCHA = 0;
				break;
			default:
				HTBW_MAX = 0;
				break;
			}
		}
	}
	else
	{
		if ((Channel >= 0) && (Channel <= 7))
			EXTCHA_MAX = 1;
		else if ((Channel >= 8) && (Channel <= 14))
		{
			if ((ChannelNumMax_2G - Channel) < 4)
				EXTCHA_MAX = 0;
			else
				EXTCHA_MAX = 1;
		}
		else
			HTBW_MAX = 0;
	}

	//HT_EXTCHA
	if (band)
	{
		fprintf(fp, "HT_EXTCHA=%d\n", EXTCHA);
	}
	else
	{
//		str = nvram_safe_get(strcat_r(prefix, "HT_EXTCHA", tmp));
		str = nvram_pf_safe_get(prefix, "nctrlsb");
		int extcha = strcmp(str, "lower") ? 0 : 1;
		if (str && *str)
		{
			if ((Channel >= 1) && (Channel <= 4))
				fprintf(fp, "HT_EXTCHA=%d\n", 1);
//			else if (atoi(str) <= EXTCHA_MAX)
			else if (extcha <= EXTCHA_MAX)
//				fprintf(fp, "HT_EXTCHA=%d\n", atoi(str));
				fprintf(fp, "HT_EXTCHA=%d\n", extcha);
			else
				fprintf(fp, "HT_EXTCHA=%d\n", 0);
		}
		else
		{
			warning = 33;
			fprintf(fp, "HT_EXTCHA=%d\n", 0);
		}
	}

	//HT_BW
	for (i = 0; i < MAX_NO_MSSID; i++)
	{
		if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
			&& wlc_band == band
#endif
			&& i != 1) 
			continue;
					
		if (i && sw_mode == SW_MODE_REPEATER)
			wl_bw = 1;
		else
			wl_bw = get_bw_via_channel(band, Channel);
	}

		if(wl_bw == 0)
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_BW", "0");
		else if ((wl_bw > 0) && (HTBW_MAX == 1))
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_BW", "1");
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
		else if ((wlc_express - 1) == band)   //express way (apclii0)
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_BW", "1");
#endif
		else
		{
	//		warning = 34;
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_BW", "0");
		}

		//HT_BSSCoexistence
		if ((wl_bw > 1) && (HTBW_MAX == 1) 
	#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
	#else
			&& !(sw_mode == SW_MODE_REPEATER && (wlc_band == band))
	#endif
			)
			fprintf(fp, "HT_BSSCoexistence=%d\n", 0);
		else
			fprintf(fp, "HT_BSSCoexistence=%d\n", 1);	

	//HT_AutoBA
	str = nvram_pf_safe_get(prefix, "HT_AutoBA");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_AutoBA", str);
	else
	{
		warning = 35;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_AutoBA", "1");
	}

	//HT_BADecline
	str = nvram_pf_safe_get(prefix, "HT_BADecline");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_BADecline", str);
	else
	{
		warning = 36;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_BADecline", "0");
	}

	//HT_AMSDU
	str = nvram_pf_safe_get(prefix, "HT_AMSDU");
	if (str && *str)
		fprintf(fp, "HT_AMSDU=%d\n", atoi(str));
	else
	{
		warning = 37;
		fprintf(fp, "HT_AMSDU=%d\n", 0);
	}

	//HT_BAWinSize
	str = nvram_pf_safe_get(prefix, "HT_BAWinSize");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_BAWinSize", str);
	else
	{
		warning = 38;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_BAWinSize", "64");
	}

	//HT_GI
	str = nvram_pf_safe_get(prefix, "HT_GI");
	if (str && *str)
	{
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_GI", str);
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
		if(band)
#endif
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "VHT_SGI", str);
	}
	else
	{
		warning = 39;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_GI", "1");
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
		if(band)
#endif
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "VHT_SGI", "1");
	}
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
	if(band){
#endif
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "VHT_STBC", "1");
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "VHT_LDPC", "1");
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
	}
#endif

	//HT_STBC
	str = nvram_pf_safe_get(prefix, "HT_STBC");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_STBC", str);
	else
	{
		warning = 40;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_STBC", "1");
	}

	//HT_MCS
	str = nvram_pf_safe_get(prefix, "HT_MCS");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_MCS", str);
	else
	{
		warning = 41;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_MCS", "33");
	}

	//HT_TxStream
	str = nvram_pf_safe_get(prefix, "HT_TxStream");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, 1, "HT_TxStream", str);
	else
	{
		warning = 42;
		enum_sname_mvalue_w_fixed_value(fp, 1, "HT_TxStream", "2");
	}

	//HT_RxStream
	str = nvram_pf_safe_get(prefix, "HT_RxStream");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, 1, "HT_RxStream", str);
	else
	{
		warning = 43;
		enum_sname_mvalue_w_fixed_value(fp, 1, "HT_RxStream", "3");
	}

	//HT_PROTECT
	str = nvram_pf_safe_get(prefix, "nmode_protection");
	if (str && *str)
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_PROTECT", strcmp(str, "auto") ? "0" : "1");
	else
	{
		warning = 44;
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_PROTECT", "1");
	}

	//HT_DisallowTKIP
	fprintf(fp, "HT_DisallowTKIP=%d\n", 1);
	if (band) {
		if (wl_bw > 0)
		{
			if (wl_bw == 2)	// 40 MHz
				enum_sname_mvalue_w_fixed_value(fp, ssid_num, "VHT_BW", "0");
#if defined(RTCONFIG_VHT160) && !defined(RALINK_DBDC_MODE)
			else if (sw_mode != SW_MODE_REPEATER && (wl_bw == 5 || nvram_match(strcat_r(prefix, "bw_160", tmp), "1"))) // 20/40/80/160 MHz
				enum_sname_mvalue_w_fixed_value(fp, ssid_num, "VHT_BW", "2");
#endif
			else // wl_bw == 3 ==> 20/40/80 MHz
				enum_sname_mvalue_w_fixed_value(fp, ssid_num, "VHT_BW", "1");
		}
		else
		{
			warning = 8;
			enum_sname_mvalue_w_fixed_value(fp, ssid_num, "VHT_BW", "1");			//20/40/80 MHz
		}
			
		str = nvram_pf_safe_get(prefix_mssid, "VHT_DisallowNonVHT");
		if (str && *str)
			fprintf(fp, "VHT_DisallowNonVHT=%d\n", atoi(str));
		else
			fprintf(fp, "VHT_DisallowNonVHT=%d\n", 0);
		fprintf(fp, "VHT_BW_SIGNAL=%d\n", 0);
	}
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	else
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "VHT_BW", "0");// 40 MHz
#endif
//	if(!nvram_match("territory_code", ""))
//		fprintf(fp, "BFBACKOFFenable=%d\n", 1);
//	else
		fprintf(fp, "BFBACKOFFenable=%d\n", 0);
#if defined(RMAC2100)
	// TxBF
	if (band)
#endif
	{
#ifdef RTCONFIG_TXBF_BAND3ONLY
		if (sw_mode == SW_MODE_AP) {

			cht =  nvram_pf_get_int(prefix, "channel");
			if (cht >= 100 && cht <= 144) { // BAND3
				nvram_pf_set_int(prefix, "txbf",  1);
				nvram_pf_set_int(prefix, "txbf_en", 1);
			}
			else {
				nvram_pf_set_int(prefix, "txbf",  0);
				nvram_pf_set_int(prefix, "txbf_en", 0);
			}

		} else {
			nvram_pf_set_int(prefix, "txbf", 0);
			nvram_pf_set_int(prefix, "txbf_en", 0);
		}
#endif		
		if(nvram_match("reg_spec", "CE") && mediabridge_mode() && nvram_get_int("wlc_psta") == 1)
			fprintf(fp, "ITxBfEn=%d\n", 0);
		else
			fprintf(fp, "ITxBfEn=%d\n", nvram_pf_get_int(prefix, "itxbf"));
		if(nvram_match("reg_spec", "CE") && mediabridge_mode() && nvram_get_int("wlc_psta") == 1)
			fprintf(fp, "ETxBfEnCond=%d\n", 0);
		else if(nvram_pf_get_int(prefix, "nmode_x") == 2)
			fprintf(fp, "ETxBfEnCond=%d\n", 0);
		else
			fprintf(fp, "ETxBfEnCond=%d\n", nvram_pf_get_int(prefix, "txbf"));
		fprintf(fp, "ETxBfIncapable=%d\n", 0);
	}
	//WscConfStatus

	if (nvram_get_int("w_Setting") == 0)
	{
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "WscConfMode", "0");
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "WscConfStatus", "1");
		g_wsc_configured = 0;						// AP is unconfigured
		nvram_pf_set(prefix, "wsc_config_state", "0");
	}
	else
	{
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "WscConfMode", "0");
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "WscConfStatus", "2");
		g_wsc_configured = 1;						// AP is configured
		nvram_pf_set(prefix, "wsc_config_state", "1");
	}
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "WscVendorPinCode", nvram_safe_get("secret_code"));

	memset(mac_filter,0,sizeof(mac_filter));
	//AccessPolicy0
	gen_macmode(mac_filter, band); //Ren
	for (i = 0; i < MAX_NO_MSSID; i++)
		fprintf(fp, "AccessPolicy%d=%d\n", i, mac_filter[i]);

	for (i = 0,j = 0; i < MAX_NO_MSSID; i++)
	{
		list[0]=0;
		list[1]=0;
		if (i)
			sprintf(prefix_mssid, "wl%d.%d_", band, i);
		else
			sprintf(prefix_mssid, "wl%d_", band);


		if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
			continue;

		if (nvram_pf_invmatch(prefix_mssid, "macmode", "disabled"))
		{
			nv = nvp = strdup(nvram_pf_safe_get(prefix_mssid, "maclist_x"));
			if (nv) {
				while ((b = strsep(&nvp, "<")) != NULL) {
					if (strlen(b)==0) continue;
					if (list[0]==0)
						sprintf(list, "%s", b);
					else{
						strlcat(list, ";", sizeof(list));
						strlcat(list, b, sizeof(list));
					}
				}
				free(nv);
			}
			fprintf(fp, "AccessControlList%d=%s\n", j, list);
		}
		else
			fprintf(fp, "AccessControlList%d=%s\n", j, "");
		j++;

	} //for loop
	//WdsEnable
	if (sw_mode == SW_MODE_REPEATER || nvram_pf_match(prefix, "mode_x", "0"))
	{
		fprintf(fp, "WdsEnable=%d\n", 0);
		fprintf(fp, "WdsPhyMode=%d\n", 0);
		fprintf(fp, "WdsEncrypType=%s\n", "NONE");
		fprintf(fp, "WdsList=\n");
		enum_mname_svalue_w_fixed_value(fp, 8, "Wds%dKey", "", 0);
	}
	else
	{
		int wds_num = 0;
		nv = nvp = strdup(nvram_pf_safe_get(prefix, "wdslist"));
		if (nv) {
			while ((b = strsep(&nvp, "<")) != NULL) {
				if (strlen(b)==0) continue;
				wds_num++;
			}
			free(nv);
		}
		str = nvram_pf_safe_get(prefix, "mode_x");
		if (str && *str)
		{
			if (nvram_pf_match(prefix, "auth_mode_x", "open") || (nvram_pf_match(prefix, "auth_mode_x", "psk2") && nvram_pf_match(prefix, "crypto", "aes")))
			{
				if(nvram_pf_get(prefix, "wdslist") == NULL || !*nvram_pf_get(prefix, "wdslist")){
					wds_num = 8;
					fprintf(fp, "WdsEnable=%d\n", 4);
				}else if (atoi(str) == 0)
					fprintf(fp, "WdsEnable=%d\n", 0);
				else if (atoi(str) == 1)
					fprintf(fp, "WdsEnable=%d\n", 2);
				else if (atoi(str) == 2)
				{
					if (nvram_pf_match(prefix, "wdsapply_x", "0")){
						wds_num = 8;
						fprintf(fp, "WdsEnable=%d\n", 4);
					}else
						fprintf(fp, "WdsEnable=%d\n", 3);
				}
			}
			else
				fprintf(fp, "WdsEnable=%d\n", 0);
		}
		else
		{
			warning = 49;
			fprintf(fp, "WdsEnable=%d\n", 0);
		}

		//WdsPhyMode
		str = nvram_pf_safe_get(prefix, "mode_x");
		if (str && strlen(str))
		{
			if ((nvram_pf_get_int(prefix, "nmode_x") == 1))
				enum_sname_mvalue_w_fixed_value(fp, wds_num, "WdsPhyMode", "GREENFIELD");
			else if ((nvram_pf_get_int(prefix, "nmode_x") == 2))
				enum_sname_mvalue_w_fixed_value(fp, wds_num, "WdsPhyMode", "OFDM");
			else if ((nvram_pf_get_int(prefix, "nmode_x") == 0)){
				if(find_word(nvram_safe_get("rc_support"), "11AX"))
					enum_sname_mvalue_w_fixed_value(fp, wds_num, "WdsPhyMode", "HE");
				else if(band && find_word(nvram_safe_get("rc_support"), "11AC"))
					enum_sname_mvalue_w_fixed_value(fp, wds_num, "WdsPhyMode", "VHT");
				else
					enum_sname_mvalue_w_fixed_value(fp, wds_num, "WdsPhyMode", "HTMIX");
			}
			else
				enum_sname_mvalue_w_fixed_value(fp, wds_num, "WdsPhyMode", "HTMIX");
		}

		//WdsEncrypType
		if (nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_match(prefix, "wep_x", "0"))
			enum_sname_mvalue_w_fixed_value(fp, wds_num, "WdsEncrypType", "NONE");
		else if (nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_invmatch(prefix, "wep_x", "0"))
			enum_sname_mvalue_w_fixed_value(fp, wds_num, "WdsEncrypType", "WEP");
		else if (nvram_pf_match(prefix, "auth_mode_x", "psk2") && nvram_pf_match(prefix, "crypto", "aes"))
			enum_sname_mvalue_w_fixed_value(fp, wds_num, "WdsEncrypType", "AES");
		else
			enum_sname_mvalue_w_fixed_value(fp, wds_num, "WdsEncrypType", "NONE");

		list[0]=0;
		list[1]=0;
		if ((nvram_pf_match(prefix, "mode_x", "1") || (nvram_pf_match(prefix, "mode_x", "2") && nvram_pf_match(prefix, "wdsapply_x", "1"))) &&
			(nvram_pf_match(prefix, "auth_mode_x", "open") ||
			(nvram_pf_match(prefix, "auth_mode_x", "psk2") && nvram_pf_match(prefix, "crypto", "aes"))))
		{
			nv = nvp = strdup(nvram_pf_get(prefix, "wdslist"));
			if (nv) {
				while ((b = strsep(&nvp, "<")) != NULL) {
					if (strlen(b)==0) continue;
					if (list[0]==0)
						sprintf(list, "%s", b);
					else{
						strlcat(list, ";", sizeof(list));
						strlcat(list, b, sizeof(list));
					}
				}
				free(nv);
			}
		}

		//WdsList
		fprintf(fp, "WdsList=%s\n", list);

		//WdsKey
		if (nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_match(prefix, "wep_x", "0"))
		{
			fprintf(fp, "WdsDefaultKeyID=\n");
			enum_mname_svalue_w_fixed_value(fp, wds_num, "Wds%dKey", "", 0);
		}
		else if (nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_invmatch(prefix, "wep_x", "0"))
		{
			enum_sname_mvalue_w_fixed_value(fp, 8, "WdsDefaultKeyID", nvram_pf_safe_get(prefix, "key"));
			str = strcat_r(prefix, "key", tmp);
			str2 = nvram_safe_get(str);
			sprintf(list, "%s%s", str, str2);
			enum_mname_svalue_w_fixed_value(fp, wds_num, "Wds%dKey", nvram_safe_get(list), 0);
		}
		else if (nvram_pf_match(prefix, "auth_mode_x", "psk2") && nvram_pf_match(prefix, "crypto", "aes"))
		{
			enum_mname_svalue_w_fixed_value(fp, wds_num, "Wds%dKey", nvram_pf_safe_get(prefix, "wpa_psk"), 0);
		}

	} // sw_mode

//	fprintf(fp, "WirelessEvent=%d\n", 0);

	//RADIUS_Server
	//RADIUS_Port
	//RADIUS_Key
	if(flag_8021x)
	{
		char *radius_server, *radius_port, *radius_key;
		radius_server = nvram_pf_safe_get(prefix, "radius_ipaddr");
		radius_port   = nvram_pf_safe_get(prefix, "radius_port");
		radius_key    = nvram_pf_safe_get(prefix, "radius_key");
		j = 1;
		for (i = 0; i < MAX_NO_MSSID; i++)
		{
			if (sw_mode == SW_MODE_REPEATER
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
				&& (wlc_express == 0 || (wlc_express - 1) != band)
#else
				&& wlc_band == band
#endif
				&& i != 1)
				continue;
			if (i)
			{
				sprintf(prefix_mssid, "wl%d.%d_", band, i);
				if (!nvram_pf_match(prefix_mssid, "bss_enabled", "1"))
					continue;
			}
			j++;
		}
		enum_mname_svalue_w_fixed_value(fp, j, "RADIUS_Key%d", radius_key, 1);
		enum_sname_mvalue_w_fixed_value(fp, j, "RADIUS_Server", radius_server);
		enum_sname_mvalue_w_fixed_value(fp, j, "RADIUS_Port", radius_port);
	}
	else
	{
		warning = 50;
		enum_mname_svalue_w_fixed_value(fp, ssid_num, "RADIUS_Key%d", "", 1);
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "RADIUS_Server", "0");
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "RADIUS_Port", "1812");
	}
	enum_sname_mvalue_w_fixed_value(fp, 1, "RADIUS_Acct_Server", "");
	enum_sname_mvalue_w_fixed_value(fp, 1, "RADIUS_Acct_Port", "1813");
	enum_sname_mvalue_w_fixed_value(fp, 1, "RADIUS_Acct_Key", "");

	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "own_ip_addr", nvram_safe_get("lan_ipaddr"));
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "EAPifname", "br0");
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "PreAuthifname", "br0");
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "session_timeout_interval", "0");


	fprintf(fp, "session_timeout_interval=%d\n", 0);
	fprintf(fp, "idle_timeout_interval=%d\n", 0);

	//WiFiTest
	fprintf(fp, "WiFiTest=0\n");

	//TGnWifiTest
	fprintf(fp, "TGnWifiTest=0\n");


#ifdef RTCONFIG_WIRELESSREPEATER
#if defined(RTCONFIG_CONCURRENTREPEATER)
	if (sw_mode == SW_MODE_REPEATER && (wlc_express == 0 || (wlc_express - 1) == band))
	{
		int flag_wep = 0;
		int p;
	// convert wlc_xxx to wlX_ according to wlc_band == band
		nvram_set("ure_disable", "0");
		snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc%d_", band);
		nvram_set(strcat_r(prefix, "ssid", tmp), nvram_safe_get(strcat_r(prefix_wlc, "ssid", tmp1)));
		nvram_set(strcat_r(prefix, "auth_mode_x", tmp), nvram_safe_get(strcat_r(prefix_wlc, "auth_mode", tmp1)));

		//nvram_set(strcat_r(prefix, "wep_x", tmp), nvram_safe_get("wlc_wep"));
		nvram_set(strcat_r(prefix, "wep_x", tmp), nvram_safe_get(strcat_r(prefix_wlc, "wep", tmp1)));

		nvram_set(strcat_r(prefix, "key", tmp), nvram_safe_get(strcat_r(prefix_wlc, "key", tmp1)));
		for(p = 1; p <= 4; p++)
		{
			char prekey[16];
			snprintf(prekey, sizeof(prekey), "key%d", p);
			if(nvram_get_int(strcat_r(prefix_wlc, "key", tmp))==p)
				nvram_set(strcat_r(prefix, prekey, tmp), nvram_safe_get(strcat_r(prefix_wlc, "wep_key", tmp1)));
		}

		nvram_set(strcat_r(prefix, "crypto", tmp), nvram_safe_get(strcat_r(prefix_wlc, "crypto", tmp1)));
		nvram_set(strcat_r(prefix, "wpa_psk", tmp), nvram_safe_get(strcat_r(prefix_wlc, "wpa_psk", tmp1)));
		nvram_set(strcat_r(prefix, "bw", tmp), nvram_safe_get(strcat_r(prefix_wlc, "nbw_cap", tmp1)));
		nvram_set(strcat_r(prefix, "hide_pap", tmp), nvram_safe_get(strcat_r(prefix_wlc, "hide_pap", tmp1)));
		nvram_set(strcat_r(prefix, "wifipxy", tmp), nvram_safe_get(strcat_r(prefix_wlc, "wifipxy", tmp1)));

		fprintf(fp, "ApCliEnable=1\n");
		fprintf(fp, "ApCliSsid%d=%s\n", 1, nvram_safe_get(strcat_r(prefix_wlc, "ssid", tmp1)));
		fprintf(fp, "ApCliBssid=\n");
#if defined(RTCONFIG_MT798X)
		fprintf(fp, "ApCliBcnProt=0\n");
#endif
		fprintf(fp, "MACRepeaterEn=%s\n", nvram_safe_get(strcat_r(prefix_wlc, "wifipxy", tmp1)));

		str = nvram_safe_get(strcat_r(prefix_wlc, "auth_mode", tmp));
		if (str && strlen(str))
		{
			if (!strcmp(str, "open") && nvram_match(strcat_r(prefix_wlc, "wep", tmp), "0"))
			{
				fprintf(fp, "ApCliAuthMode=%s\n", "OPEN");
				fprintf(fp, "ApCliEncrypType=%s\n", "NONE");
			}
			else if (!strcmp(str, "open") || !strcmp(str, "shared"))
			{
				flag_wep = 1;
				fprintf(fp, "ApCliAuthMode=%s\n", "WEPAUTO");
				fprintf(fp, "ApCliEncrypType=%s\n", "WEP");
			}
			else if (!strcmp(str, "psk") || !strcmp(str, "psk2")
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
			|| !strcmp(str, "pskpsk2") || !strcmp(str, "sae") || !strcmp(str, "psk2sae")
#endif
			)
			{
				if (!strcmp(str, "psk"))
					fprintf(fp, "ApCliAuthMode=%s\n", "WPAPSK");
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
				else if( !strcmp(str, "pskpsk2") )
					fprintf(fp, "ApCliAuthMode=%s\n", "WPAPSKWPA2PSK");
				else if( !strcmp(str, "sae") )
					fprintf(fp, "ApCliAuthMode=%s\n", "WPA3PSK");
				else if( !strcmp(str, "psk2sae") )
					fprintf(fp, "ApCliAuthMode=%s\n", "WPA2PSKWPA3PSK");
#endif
				else
					fprintf(fp, "ApCliAuthMode=%s\n", "WPA2PSK");

				//EncrypType
				if (nvram_match(strcat_r(prefix_wlc, "crypto", tmp), "tkip"))
					fprintf(fp, "ApCliEncrypType=%s\n", "TKIP");
				else if (nvram_match(strcat_r(prefix_wlc, "crypto", tmp), "aes"))
					fprintf(fp, "ApCliEncrypType=%s\n", "AES");

				//WPAPSK
				fprintf(fp, "ApCliWPAPSK%d=%s\n", 1, nvram_safe_get(strcat_r(prefix_wlc, "wpa_psk", tmp)));
			}
			else
			{
				fprintf(fp, "ApCliAuthMode=%s\n", "OPEN");
				fprintf(fp, "ApCliEncrypType=%s\n", "NONE");
			}
		}
		else
		{
			fprintf(fp, "ApCliAuthMode=%s\n", "OPEN");
			fprintf(fp, "ApCliEncrypType=%s\n", "NONE");
		}

		//EncrypType
		if (flag_wep)
		{
			//DefaultKeyID
			fprintf(fp, "ApCliDefaultKeyID=%s\n", nvram_safe_get(strcat_r(prefix_wlc, "key", tmp1)));

			//KeyType (0 -> Hex, 1->Ascii)
			for(p = 1 ; p <= 4; p++)
			{
				if(nvram_get_int(strcat_r(prefix_wlc, "key", tmp))==p)
				{
					if((strlen(nvram_safe_get(strcat_r(prefix_wlc, "wep_key", tmp)))==5)||(strlen(nvram_safe_get(strcat_r(prefix_wlc, "wep_key", tmp1)))==13))
						fprintf(fp, "ApCliKey%dType=1\n",p);
					else if((strlen(nvram_safe_get(strcat_r(prefix_wlc, "wep_key", tmp)))==10)||(strlen(nvram_safe_get(strcat_r(prefix_wlc, "wep_key", tmp1)))==26))
						fprintf(fp, "ApCliKey%dType=0\n",p);
					else
					   	fprintf(fp, "ApCliKey%dType=\n",p);
				}
				else
				   	 fprintf(fp, "ApCliKey%dType=\n",p);
			}

			//KeyStr
			for(p = 1 ; p <= 4; p++)
			{
				if(nvram_get_int(strcat_r(prefix_wlc, "key", tmp))==p)
					fprintf(fp, "ApCliKey%dStr=%s\n",p,nvram_safe_get(strcat_r(prefix_wlc, "wep_key", tmp1)));
				else
					fprintf(fp, "ApCliKey%dStr=\n",p);
			}
		}
		else
		{
			fprintf(fp, "ApCliDefaultKeyID=0\n");
			fprintf(fp, "ApCliKey1Type=0\n");
			fprintf(fp, "ApCliKey1Str=\n");
			fprintf(fp, "ApCliKey2Type=0\n");
			fprintf(fp, "ApCliKey2Str=\n");
			fprintf(fp, "ApCliKey3Type=0\n");
			fprintf(fp, "ApCliKey3Str=\n");
			fprintf(fp, "ApCliKey4Type=0\n");
			fprintf(fp, "ApCliKey4Str=\n");
		}
	}
	else
#else // RTCONFIG_CONCURRENTREPEATER
	if ((sw_mode == SW_MODE_REPEATER && wlc_band == band && nvram_invmatch("wlc_ssid", "")) || (wisp_mode() && wlc_band == band))
	{
		int flag_wep = 0;
		int p;
	// convert wlc_xxx to wlX_ according to wlc_band == band
		nvram_set("ure_disable", "0");
		if((sw_mode == SW_MODE_REPEATER && nvram_invmatch("wlc_ssid", "")) || wisp_mode())
			snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc_");
		else{
			r = snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc%d_", band);
			if(unlikely(r < 0))
				dbg("snprintf failed\n");
		}
		if(!wisp_mode()){
			nvram_pf_set(prefix, "ssid", nvram_pf_safe_get(prefix_wlc, "ssid"));
			nvram_pf_set(prefix, "auth_mode_x", nvram_pf_safe_get(prefix_wlc, "auth_mode"));
			nvram_pf_set(prefix, "wep_x", nvram_pf_safe_get(prefix_wlc, "wep"));
			nvram_pf_set(prefix, "key", nvram_pf_safe_get(prefix_wlc, "key"));
			for(p = 1; p <= 4; p++){
				char prekey[16];
				snprintf(prekey, sizeof(prekey), "key%d", p);
				if(nvram_pf_get_int(prefix_wlc, "key") == p)
					nvram_pf_set(prefix, prekey, nvram_pf_safe_get(prefix_wlc, "wep_key"));
			}
			nvram_pf_set(prefix, "crypto", nvram_pf_safe_get(prefix_wlc, "crypto"));
			nvram_pf_set(prefix, "wpa_psk", nvram_pf_safe_get(prefix_wlc, "wpa_psk"));

//nbw_cap is null or empty in qisv3, set bw to auto
//			nvram_pf_set(prefix, "bw", nvram_pf_get(prefix_wlc, "nbw_cap"));
			nvram_pf_set(prefix, "bw", "1");
			//nvram_pf_set(prefix, "hide_pap", nvram_pf_safe_get(prefix_wlc, "hide_pap"));
			if(strlen(nvram_pf_safe_get(prefix_wlc, "wifipxy")))
				nvram_pf_set(prefix, "wifipxy", nvram_pf_get(prefix_wlc, "wifipxy"));
		}
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
		fprintf(fp, "ApCliEnable=0\n");
#else
		fprintf(fp, "ApCliEnable=1\n");
#endif
		fprintf(fp, "ApCliSsid=%s\n", nvram_pf_safe_get(prefix_wlc, "ssid"));
		fprintf(fp, "ApCliBssid=\n");
		fprintf(fp, "MACRepeaterEn=%s\n", nvram_pf_safe_get(prefix_wlc, "wifipxy"));

		str = nvram_pf_safe_get(prefix_wlc, "auth_mode");
		if (str && strlen(str))
		{
			if (!strcmp(str, "open") && nvram_pf_match(prefix_wlc, "wep", "0"))
			{
				fprintf(fp, "ApCliAuthMode=%s\n", "OPEN");
				fprintf(fp, "ApCliEncrypType=%s\n", "NONE");
			}
			else if (!strcmp(str, "open") || !strcmp(str, "shared"))
			{
				flag_wep = 1;
				fprintf(fp, "ApCliAuthMode=%s\n", "WEPAUTO");
				fprintf(fp, "ApCliEncrypType=%s\n", "WEP");
			}
			else if (!strcmp(str, "psk") || !strcmp(str, "psk2")
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
			|| !strcmp(str, "pskpsk2") || !strcmp(str, "sae") || !strcmp(str, "psk2sae")
#endif
			)
			{
				if (!strcmp(str, "psk")){
					fprintf(fp, "ApCliAuthMode=%s\n", "WPAPSK");
					fprintf(fp, "ApCliPMFMFPR=%s\n", "0");
				}else if( !strcmp(str, "pskpsk2") ){
					fprintf(fp, "ApCliAuthMode=%s\n", "WPAPSKWPA2PSK");
					fprintf(fp, "ApCliPMFMFPR=%s\n", "0");
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
				}else if( !strcmp(str, "sae") ){
					fprintf(fp, "ApCliAuthMode=%s\n", "WPA3PSK");
					fprintf(fp, "ApCliPMFMFPR=%s\n", "1");
				}else if( !strcmp(str, "psk2sae") ){
					fprintf(fp, "ApCliAuthMode=%s\n", "WPA2PSKWPA3PSK");
					fprintf(fp, "ApCliPMFMFPR=%s\n", "0");
#endif
				}else{
					fprintf(fp, "ApCliAuthMode=%s\n", "WPA2PSK");
					fprintf(fp, "ApCliPMFMFPR=%s\n", "0");
				}
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
				fprintf(fp, "ApCliPMFMFPC=%s\n", "1");
#else
				fprintf(fp, "ApCliPMFMFPC=%s\n", "0");
#endif
				fprintf(fp, "ApCliPMFSHA256=%s\n", "0");
				//EncrypType
				if (nvram_pf_match(prefix_wlc, "crypto", "tkip"))
					fprintf(fp, "ApCliEncrypType=%s\n", "TKIP");
				else if (nvram_pf_match(prefix_wlc, "crypto", "aes"))
					fprintf(fp, "ApCliEncrypType=%s\n", "AES");
				else if (nvram_pf_match(prefix_wlc, "crypto", "tkip+aes"))
					fprintf(fp, "ApCliEncrypType=%s\n", "TKIPAES");
					//WPAPSK
				fprintf(fp, "ApCliWPAPSK=%s\n", nvram_pf_safe_get(prefix_wlc, "wpa_psk"));
				fprintf(fp, "ApCliWPAPSK%d=%s\n", 1, nvram_pf_safe_get(prefix_wlc, "wpa_psk"));
			}
		}
		else
		{
			fprintf(fp, "ApCliAuthMode=%s\n", "OPEN");
			fprintf(fp, "ApCliEncrypType=%s\n", "NONE");
		}

		//EncrypType
		if (flag_wep)
		{
			//DefaultKeyID
			fprintf(fp, "ApCliDefaultKeyID=%s\n", nvram_pf_safe_get(prefix_wlc, "key"));

			//KeyType (0 -> Hex, 1->Ascii)
			for(p = 1 ; p <= 4; p++)
			{
				if(nvram_pf_get_int(prefix_wlc, "key") == p)
				{
				   	if(strlen(nvram_pf_safe_get(prefix_wlc, "wep_key")) == 5 || strlen(nvram_pf_safe_get(prefix_wlc, "wep_key")) == 13)
						fprintf(fp, "ApCliKey%dType=1\n", p);
					else if(strlen(nvram_pf_safe_get(prefix_wlc, "wep_key")) == 10 || strlen(nvram_pf_safe_get(prefix_wlc, "wep_key")) == 26)
						fprintf(fp, "ApCliKey%dType=0\n", p);
					else
					   	fprintf(fp, "ApCliKey%dType=\n", p);
				}
				else
				   	 fprintf(fp, "ApCliKey%dType=\n", p);
			}

			//KeyStr
			for(p = 1 ; p <= 4; p++)
			{
				if(nvram_pf_get_int(prefix_wlc, "key") == p)
					fprintf(fp, "ApCliKey%dStr=%s\n", p, nvram_pf_safe_get(prefix_wlc, "wep_key"));
				else
					fprintf(fp, "ApCliKey%dStr=\n", p);
			}
		}
		else
		{
			fprintf(fp, "ApCliDefaultKeyID=0\n");
			fprintf(fp, "ApCliKey1Type=0\n");
			fprintf(fp, "ApCliKey1Str=\n");
			fprintf(fp, "ApCliKey2Type=0\n");
			fprintf(fp, "ApCliKey2Str=\n");
			fprintf(fp, "ApCliKey3Type=0\n");
			fprintf(fp, "ApCliKey3Str=\n");
			fprintf(fp, "ApCliKey4Type=0\n");
			fprintf(fp, "ApCliKey4Str=\n");
		}
	}
	else
#endif
#endif // RTCONFIG_WIRELESSREPEATER
	{
		fprintf(fp, "ApCliEnable=0\n");
		fprintf(fp, "ApCliSsid=\n");
		fprintf(fp, "ApCliBssid=\n");
		fprintf(fp, "ApCliAuthMode=\n");
		fprintf(fp, "ApCliEncrypType=\n");
		fprintf(fp, "ApCliWPAPSK=\n");
		fprintf(fp, "ApCliDefaultKeyID=0\n");
		fprintf(fp, "ApCliKey1Type=0\n");
		fprintf(fp, "ApCliKey1Str=\n");
		fprintf(fp, "ApCliKey2Type=0\n");
		fprintf(fp, "ApCliKey2Str=\n");
		fprintf(fp, "ApCliKey3Type=0\n");
		fprintf(fp, "ApCliKey3Str=\n");
		fprintf(fp, "ApCliKey4Type=0\n");
		fprintf(fp, "ApCliKey4Str=\n");
		fprintf(fp, "ApCliPMFMFPC=0\n");
		fprintf(fp, "ApCliPMFMFPR=0\n");
		fprintf(fp, "ApCliPMFSHA256=0\n");
		fprintf(fp, "MACRepeaterEn=0\n");
		fprintf(fp, "MACRepeaterOuiMode=0\n");
	}

	//RadioOn
	str = nvram_pf_safe_get(prefix, "radio");
	if (str && *str)
		fprintf(fp, "RadioOn=%d\n", atoi(str));
	else
	{
		warning = 51;
		fprintf(fp, "RadioOn=%d\n", 1);
	}

//	fprintf(fp, "SSID=\n");
//	fprintf(fp, "WPAPSK=\n");
//	fprintf(fp, "Key1Str=\n");
//	fprintf(fp, "Key2Str=\n");
//	fprintf(fp, "Key3Str=\n");
//	fprintf(fp, "Key4Str=\n");

	/* Wireless IGMP Snooping */
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "IgmpSnEnable", nvram_pf_get_int(prefix, "igs") ? "1" : "0");
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	/*	McastPhyMode, PHY mode for Multicast frames
	 *	McastMcs, MCS for Multicast frames
	 *
	 *	MODE=1, MCS=0: Legacy CCK 1Mbps
	 *	MODE=1, MCS=1: Legacy CCK 2Mbps
	 *	MODE=1, MCS=2: Legacy CCK 5.5Mbps
	 *	MODE=1, MCS=3: Legacy CCK 11Mbps
	 *	MODE=2, MCS=0: Legacy OFDM 6Mbps
	 *	MODE=2, MCS=1: Legacy OFDM 9Mbps
	 *	MODE=2, MCS=2: Legacy OFDM 12Mbps
	 *	MODE=2, MCS=3: Legacy OFDM 18Mbps
	 *	MODE=2, MCS=4: Legacy OFDM 24Mbps
	 * 	MODE=2, MCS=5: Legacy OFDM 36Mbps
	 *	MODE=2, MCS=6: Legacy OFDM 48Mbps
	 *	MODE=2, MCS=7: Legacy OFDM 54Mbps
	 *	MODE=3, MCS=0: HTMIX 6.5/15Mbps
	 *	MODE=3, MCS=1: HTMIX 13/30Mbps
	 *	MODE=3, MCS=2: HTMIX 19.5/45Mbps
	 *	MODE=3, MCS=7: HTMIX 65/150Mbps
	 *	MODE=3, MCS=8: HTMIX 13/30Mbps 2S
	 *	MODE=3, MCS=9: HTMIX 26/60Mbps 2S
	 *	MODE=3, MCS=10: HTMIX 39/90Mbps 2S
	 *	MODE=3, MCS=15: HTMIX 130/300Mbps 2S
	 */
	i = nvram_pf_get_int(prefix, "mrate_x");
next_mrate:
	switch (i++) {
	default:
	case 0:/* Driver default setting: Disable, means automatic rate instead of fixed rate
		* Please refer to #ifdef MCAST_RATE_SPECIFIC section in
		* file linuxxxx/drivers/net/wireless/rtxxxx/common/mlme.c
		*/
		mcast_phy = 0, mcast_mcs = 0;
		break;
	case 1: /* Legacy CCK 1Mbps */
		mcast_phy = 1, mcast_mcs = 0;
		break;
	case 2: /* Legacy CCK 2Mbps */
		mcast_phy = 1, mcast_mcs = 1;
		break;
	case 3: /* Legacy CCK 5.5Mbps */
		mcast_phy = 1, mcast_mcs = 2;
		break;
	case 4: /* Legacy OFDM 6Mbps */
		mcast_phy = 2, mcast_mcs = 0;
		break;
	case 5: /* Legacy OFDM 9Mbps */
		mcast_phy = 2, mcast_mcs = 1;
		break;
	case 6: /* Legacy CCK 11Mbps */
		mcast_phy = 1, mcast_mcs = 3;
		break;
	case 7: /* Legacy OFDM 12Mbps */
		mcast_phy = 2, mcast_mcs = 2;
		break;
	case 8: /* Legacy OFDM 18Mbps */
		mcast_phy = 2, mcast_mcs = 3;
		break;
	case 9: /* Legacy OFDM 24Mbps */
		mcast_phy = 2, mcast_mcs = 4;
		break;
	case 10:/* Legacy OFDM 36Mbps */
		mcast_phy = 2, mcast_mcs = 5;
		break;
	case 11:/* Legacy OFDM 48Mbps */
		mcast_phy = 2, mcast_mcs = 6;
		break;
	case 12:/* Legacy OFDM 54Mbps */
		mcast_phy = 2, mcast_mcs = 7;
		break;
	case 13:/* HTMIX 130/300Mbps 2S */
		mcast_phy = 3, mcast_mcs = 15;
		break;
	case 14:/* HTMIX 6.5/15Mbps */
		mcast_phy = 3, mcast_mcs = 0;
		break;
	case 15:/* HTMIX 13/30Mbps */
		mcast_phy = 3, mcast_mcs = 1;
		break;
	case 16:/* HTMIX 19.5/45Mbps */
		mcast_phy = 3, mcast_mcs = 2;
		break;
	case 17:/* HTMIX 13/30Mbps 2S */
		mcast_phy = 3, mcast_mcs = 8;
		break;
	case 18:/* HTMIX 26/60Mbps 2S */
		mcast_phy = 3, mcast_mcs = 9;
		break;
	case 19:/* HTMIX 39/90Mbps 2S */
		mcast_phy = 3, mcast_mcs = 10;
		break;
	case 20:
		/* Choose multicast rate base on mode, encryption type, and IPv6 is enabled or not. */
		__choose_mrate(prefix, &mcast_phy, &mcast_mcs);
		break;
	}
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	if (band == 0 && nvram_pf_match(prefix, "rateset", "ofdm") && mcast_phy == 1)
		goto next_mrate;
#endif
	/* No CCK for 5Ghz band */
	if (band && mcast_phy == 1)
		goto next_mrate;
	snprintf(tmp, sizeof(tmp), "%d", mcast_phy);
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "McastPhyMode", tmp);
	snprintf(tmp, sizeof(tmp), "%d", mcast_mcs);
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "McastMcs", tmp);
#endif

	/* Set WSC/WPS variables */
	if(band)
		str = " (5G)";
	else
		str = "";
	fprintf(fp, "WscManufacturer=%s\n", "ASUSTeK Computer Inc.");
	fprintf(fp, "WscModelName=%s%s\n", "WPS Router", str);
	fprintf(fp, "WscDeviceName=%s%s\n", "ASUS WPS Router", str);
	fprintf(fp, "WscModelNumber=%s\n", get_productid());
	fprintf(fp, "WscSerialNumber=%s\n", "00000000");

	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "WscV2Support", "0");	// WPS/WSC v2 feature allows client to create connection via the PinCode of AP.
						// Disable this feature causes longer detection time (click AP till show dialog box) on WIN7 client when WSC disabled either.
	fprintf(fp, "MbssMaxStaNum=0");
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	/* Set number of clients of guest network. */
	for (i = 0; i < ssid_num; i++) {
		int maxsta;

		if (!i) {
			fprintf(fp, "MaxStaNum=0;");
		} else {
			r = snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d.%d_", band, i);
			if(unlikely(r < 0))
				dbg("snprintf failed\n");
			maxsta = nvram_pf_get_int(prefix_mssid, "guest_num");
			/* If maxsta illegal, disable MaxStaNum. */
			if (maxsta < 0 || maxsta > 32)
				maxsta = 0;
			fprintf(fp, "%d;", maxsta);
		}
	}
	fprintf(fp, "\n");

	fprintf(fp, "EfuseBufferMode=0\n");
#endif
	fprintf(fp, "E2pAccessMode=2\n");
	fprintf(fp, "VOW_RX_En=%d\n", 1);
#if defined(RTCONFIG_MT798X)
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "AMSDU_NUM", "8");
#else
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "AMSDU_NUM", "4");
#endif
	fprintf(fp, "CP_SUPPORT=%d\n", 2);
	fprintf(fp, "RED_Enable=%d\n", 1);
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	if(band) {
		if(nvram_pf_get_int(prefix, "nmode_x") != 2){
			str = nvram_pf_safe_get(prefix, "turbo_qam");
			if (str && *str)
				fprintf(fp, "Vht1024QamSupport=%d\n", atoi(str));
		}
	} else {
		if(nvram_pf_get_int(prefix, "nmode_x") != 2){
			str = nvram_pf_safe_get(prefix, "turbo_qam");
			if (str && *str)
				fprintf(fp, "G_BAND_256QAM=%d\n", atoi(str));
		}
	}
#elif defined (RTCONFIG_QAM256_2G)
	/* 2.4G 256QAM */
	if(!band) {
		if(nvram_pf_get_int(prefix, "nmode_x") != 2){
			str = nvram_pf_safe_get(prefix, "turbo_qam");
			if (str && *str)
				fprintf(fp, "G_BAND_256QAM=%d\n", atoi(str));
		}
	}
#endif
#if !defined(RTCONFIG_MT798X)
	if(*tcode)
		fprintf(fp, "SKUenable=1\n");
	else
#endif
		fprintf(fp, "SKUenable=0\n");
	fprintf(fp, "WirelessEvent=1\n");
#if defined(RTCONFIG_MT798X) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
	enum_sname_mvalue_w_fixed_value(fp, 1, "BSSColorValue", "255");
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "HT_LDPC", "1");
#endif
#if defined(RTCONFIG_MT798X)
	if(band){
		if(nvram_match(strcat_r(prefix, "11ax", tmp), "1")){
			fprintf(fp, "SREnable=1\n");
			fprintf(fp, "SRMode=0\n");
			fprintf(fp, "SRDPDEnable=0\n");
			fprintf(fp, "SRSDEnable=1\n");
		}else{
			fprintf(fp, "SREnable=0\n");
			fprintf(fp, "SRMode=0\n");
			fprintf(fp, "SRDPDEnable=0\n");
			fprintf(fp, "SRSDEnable=0\n");

		}
		fprintf(fp, "PPEnable=1\n");
	}else
		fprintf(fp, "PPEnable=0\n");
	if(mediabridge_mode())
		fprintf(fp, "WHNAT=0\n");
	else
		fprintf(fp, "WHNAT=1\n");
	fprintf(fp, "IsICAPFW=0\n");
	fprintf(fp, "KernelRps=1\n");
	fprintf(fp, "BcnProt=0\n");
	fprintf(fp, "CCKTxStream=4\n");
#if defined(RTCONFIG_EASYMESH)
	fprintf(fp, "QoSR1Enable=1\n");
#endif
	enum_sname_mvalue_w_fixed_value(fp, ssid_num, "DscpPriMapEnable", "1");
	fprintf(fp, "ScsEnable=0\n");
	fprintf(fp, "QoSMgmtCapa=0\n");
	fprintf(fp, "ApMWDS=0;0;0\n");
	fprintf(fp, "ApCliMWDS=0\n");
#endif

#if defined(RTCONFIG_SWRT_KVR)
	if(nvram_match(strcat_r(prefix, "kvr_kv", tmp), "1")){
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "WNMEnable", "1");
		enum_sname_mvalue_w_fixed_value(fp, ssid_num, "RRMEnable", "1");
	}
	if(nvram_match(strcat_r(prefix, "kvr_ft", tmp), "1")){
		fprintf(fp, "FtSupport=1\n");
		fprintf(fp, "FtOtd=1;1;1;1\n");
		fprintf(fp, "FtRic=1;1;1;1\n");
		fprintf(fp, "FtMdId1=A1\n");
		fprintf(fp, "FtMdId2=A2\n");
		fprintf(fp, "FtMdId3=A3\n");
		fprintf(fp, "FtMdId4=A4\n");
		fprintf(fp, "FtR0khId1=4f577274\n");
		fprintf(fp, "FtR0khId2=4f577276\n");
		fprintf(fp, "FtR0khId3=4f577278\n");
		fprintf(fp, "FtR0khId4=4f57727A\n");
	}
#endif

	if (warning)
	{
		printf("warning: %d!!!!\n", warning);
		printf("Miss some configuration, please check!!!!\n");
	}

	fclose(fp);
	return 0;
}


PAIR_CHANNEL_FREQ_ENTRY ChannelFreqTable[] = {
	//channel Frequency
	{1,     2412000},
	{2,     2417000},
	{3,     2422000},
	{4,     2427000},
	{5,     2432000},
	{6,     2437000},
	{7,     2442000},
	{8,     2447000},
	{9,     2452000},
	{10,    2457000},
	{11,    2462000},
	{12,    2467000},
	{13,    2472000},
	{14,    2484000},
	{34,    5170000},
	{36,    5180000},
	{38,    5190000},
	{40,    5200000},
	{42,    5210000},
	{44,    5220000},
	{46,    5230000},
	{48,    5240000},
	{52,    5260000},
	{56,    5280000},
	{60,    5300000},
	{64,    5320000},
	{100,   5500000},
	{104,   5520000},
	{108,   5540000},
	{112,   5560000},
	{116,   5580000},
	{120,   5600000},
	{124,   5620000},
	{128,   5640000},
	{132,   5660000},
	{136,   5680000},
	{140,   5700000},
	{149,   5745000},
	{153,   5765000},
	{157,   5785000},
	{161,   5805000},
};

char G_bRadio = 1;
int G_nChanFreqCount = sizeof (ChannelFreqTable) / sizeof(PAIR_CHANNEL_FREQ_ENTRY);

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
struct	iw15_range
{
	__u32		throughput;
	__u32		min_nwid;
	__u32		max_nwid;
	__u16		num_channels;
	__u8		num_frequency;
	struct iw_freq	freq[IW15_MAX_FREQUENCIES];
	__s32		sensitivity;
	struct iw_quality	max_qual;
	__u8		num_bitrates;
	__s32		bitrate[IW15_MAX_BITRATES];
	__s32		min_rts;
	__s32		max_rts;
	__s32		min_frag;
	__s32		max_frag;
	__s32		min_pmp;
	__s32		max_pmp;
	__s32		min_pmt;
	__s32		max_pmt;
	__u16		pmp_flags;
	__u16		pmt_flags;
	__u16		pm_capa;
	__u16		encoding_size[IW15_MAX_ENCODING_SIZES];
	__u8		num_encoding_sizes;
	__u8		max_encoding_tokens;
	__u16		txpower_capa;
	__u8		num_txpower;
	__s32		txpower[IW15_MAX_TXPOWER];
	__u8		we_version_compiled;
	__u8		we_version_source;
	__u16		retry_capa;
	__u16		retry_flags;
	__u16		r_time_flags;
	__s32		min_retry;
	__s32		max_retry;
	__s32		min_r_time;
	__s32		max_r_time;
	struct iw_quality	avg_qual;
};

/*
 * Union for all the versions of iwrange.
 * Fortunately, I mostly only add fields at the end, and big-bang
 * reorganisations are few.
 */
union	iw_range_raw
{
	struct iw15_range	range15;	/* WE 9->15 */
	struct iw_range		range;		/* WE 16->current */
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

/*------------------------------------------------------------------*/
/*
 * Get the range information out of the driver
 */
int ralink_get_range_info(iwrange *	range, char* buffer, int length)
{
  union iw_range_raw *	range_raw;

  /* Point to the buffer */
  range_raw = (union iw_range_raw *) buffer;

  /* For new versions, we can check the version directly, for old versions
   * we use magic. 300 bytes is a also magic number, don't touch... */
  if (length < 300)
    {
      /* That's v10 or earlier. Ouch ! Let's make a guess...*/
      range_raw->range.we_version_compiled = 9;
    }

  /* Check how it needs to be processed */
  if (range_raw->range.we_version_compiled > 15)
    {
      /* This is our native format, that's easy... */
      /* Copy stuff at the right place, ignore extra */
      memcpy((char *) range, buffer, sizeof(iwrange));
    }
  else
    {
      /* Zero unknown fields */
      bzero((char *) range, sizeof(struct iw_range));

      /* Initial part unmoved */
      memcpy((char *) range,
	     buffer,
	     iwr15_off(num_channels));
      /* Frequencies pushed futher down towards the end */
      memcpy((char *) range + iwr_off(num_channels),
	     buffer + iwr15_off(num_channels),
	     iwr15_off(sensitivity) - iwr15_off(num_channels));
      /* This one moved up */
      memcpy((char *) range + iwr_off(sensitivity),
	     buffer + iwr15_off(sensitivity),
	     iwr15_off(num_bitrates) - iwr15_off(sensitivity));
      /* This one goes after avg_qual */
      memcpy((char *) range + iwr_off(num_bitrates),
	     buffer + iwr15_off(num_bitrates),
	     iwr15_off(min_rts) - iwr15_off(num_bitrates));
      /* Number of bitrates has changed, put it after */
      memcpy((char *) range + iwr_off(min_rts),
	     buffer + iwr15_off(min_rts),
	     iwr15_off(txpower_capa) - iwr15_off(min_rts));
      /* Added encoding_login_index, put it after */
      memcpy((char *) range + iwr_off(txpower_capa),
	     buffer + iwr15_off(txpower_capa),
	     iwr15_off(txpower) - iwr15_off(txpower_capa));
      /* Hum... That's an unexpected glitch. Bummer. */
      memcpy((char *) range + iwr_off(txpower),
	     buffer + iwr15_off(txpower),
	     iwr15_off(avg_qual) - iwr15_off(txpower));
      /* Avg qual moved up next to max_qual */
      memcpy((char *) range + iwr_off(avg_qual),
	     buffer + iwr15_off(avg_qual),
	     sizeof(struct iw_quality));
    }

  /* We are now checking much less than we used to do, because we can
   * accomodate more WE version. But, there are still cases where things
   * will break... */
  if (!iw_ignore_version_sp)
    {
      /* We don't like very old version (unfortunately kernel 2.2.X) */
      if (range->we_version_compiled <= 10)
	{
	  dbg("Warning: Driver for the device has been compiled with an ancient version\n");
	  dbg("of Wireless Extension, while this program support version 11 and later.\n");
	  dbg("Some things may be broken...\n\n");
	}

      /* We don't like future versions of WE, because we can't cope with
       * the unknown */
      if (range->we_version_compiled > WE_MAX_VERSION)
	{
	  dbg("Warning: Driver for the device has been compiled with version %d\n", range->we_version_compiled);
	  dbg("of Wireless Extension, while this program supports up to version %d.\n", WE_VERSION);
	  dbg("Some things may be broken...\n\n");
	}

      /* Driver version verification */
      if ((range->we_version_compiled > 10) &&
	 (range->we_version_compiled < range->we_version_source))
	{
	  dbg("Warning: Driver for the device recommend version %d of Wireless Extension,\n", range->we_version_source);
	  dbg("but has been compiled with version %d, therefore some driver features\n", range->we_version_compiled);
	  dbg("may not be available...\n\n");
	}
      /* Note : we are only trying to catch compile difference, not source.
       * If the driver source has not been updated to the latest, it doesn't
       * matter because the new fields are set to zero */
    }

  /* Don't complain twice.
   * In theory, the test apply to each individual driver, but usually
   * all drivers are compiled from the same kernel. */
  iw_ignore_version_sp = 1;

  return (0);
}

int getSSID(int band)
{
	struct iwreq wrq;
	wrq.u.data.flags = 0;
	char buffer[33];
	memset(buffer, 0, sizeof(buffer));
	wrq.u.essid.pointer = (caddr_t) buffer;
	wrq.u.essid.length = IW_ESSID_MAX_SIZE + 1;
	wrq.u.essid.flags = 0;

	if (wl_ioctl(get_wifname(band), SIOCGIWESSID, &wrq) < 0)
	{
		dbg("!!!\n");
		return 0;
	}

	if (wrq.u.essid.length>0)
	{
		unsigned char SSID[33];
		memset(SSID, 0, sizeof(SSID));
		memcpy(SSID, wrq.u.essid.pointer, wrq.u.essid.length);
		puts(SSID);
	}

	return 0;
}

int getChannel(int band)
{
	int channel;
	struct iw_range	range;
	double freq;
	struct iwreq wrq1;
	struct iwreq wrq2;
	char ch_str[3];

	if (wl_ioctl(get_wifname(band), SIOCGIWFREQ, &wrq1) < 0)
		return 0;

	char buffer[sizeof(iwrange) * 2];
	memset(buffer, 0, sizeof(buffer));
	wrq2.u.data.pointer = (caddr_t) buffer;
	wrq2.u.data.length = sizeof(buffer);
	wrq2.u.data.flags = 0;

	if (wl_ioctl(get_wifname(band), SIOCGIWRANGE, &wrq2) < 0)
		return 0;

	if (ralink_get_range_info(&range, buffer, wrq2.u.data.length) < 0)
		return 0;

	freq = iw_freq2float(&(wrq1.u.freq));
	if (freq < KILO)
		channel = (int) freq;
	else
	{
		channel = iw_freq_to_channel(freq, &range);
		if (channel < 0)
			return 0;
	}

	memset(ch_str, 0, sizeof(ch_str));
	sprintf(ch_str, "%d", channel);
	puts(ch_str);
	return 0;
}

int getSiteSurvey(int band, char* ofile)
{
	int i = 0, apCount = 0;
	char data[SITE_SURVEY_APS_MAX];
	SSA *ssap = NULL;
	FILE *fp;
	char ssid_str[256],tmp[256];
	char ure_mac[18] = {0};
	int wl_authorized = 0;
	const char *wif = get_wifname(band);
	memset(data, 0, sizeof(data));
#ifdef RTCONFIG_MTK_REP
	    char folder_path[] = "/tmp/ssidList";
	    char file_ssidlist[30]={0};
		char *fp_list = NULL;
		memset(file_ssidlist, 0x0, sizeof(file_ssidlist));

 		if ( access(folder_path, F_OK) != 0 )
		if (ENOENT == errno)
			mkdir(folder_path, 0755);

		sprintf(file_ssidlist, "/tmp/ssidList/ssid%d.txt", band );

		fp_list=fopen(file_ssidlist, "w+");
#endif
	if(set_site_survey(wif, NULL))
		return -1;
	dbg("Please wait");
	if(use_partial_scan(wif)){
		do{
			sleep(1);
			dbg(".");
		}while(partial_scan_status(wif));
	}else{
		dbg("Please wait");
		sleep(1);
		dbg(".");
		sleep(1);
		dbg(".");
		sleep(1);
		dbg(".");
		sleep(1);
		dbg(".");
	}
	dbg(".\n\n");
	if(get_site_survey(wif, data, sizeof(data), &ssap, &apCount))
		return -1;
	if (apCount > 0)
	{
		for (i=0;i<apCount;i++)
		{
			dbg("%-4s%-33s%-20s%-23s%-9s%-12s%-7s%-3s%-4s%-5s\n",
			ssap->SiteSurvey[i].channel,
			(char*)ssap->SiteSurvey[i].ssid,
			ssap->SiteSurvey[i].bssid,
			ssap->SiteSurvey[i].security,
			ssap->SiteSurvey[i].signal,
			ssap->SiteSurvey[i].wmode,
			ssap->SiteSurvey[i].extch,
			ssap->SiteSurvey[i].nt,
			ssap->SiteSurvey[i].wps,
			ssap->SiteSurvey[i].dpid
			);
#ifdef RTCONFIG_MTK_REP
		trim_r((char*)ssap->SiteSurvey[i].ssid);
		fprintf(fp_list, "%s\n", (char*)ssap->SiteSurvey[i].ssid );
#endif
		}
#ifdef RTCONFIG_MTK_REP
		fclose(fp_list);
#endif
		dbg("\n");

		if (apCount > 0){
			/* write pid */
			if ((fp = fopen(ofile, "a")) == NULL){
				printf("[wlcscan] Output %s error\n", ofile);
			}else{
				for (i = 0; i < apCount; i++){
					char buf[33];
					char auth[16], enc[9];
					memset(buf, 0, sizeof(buf));
					memset(auth, 0, sizeof(auth));
					memset(enc, 0, sizeof(enc));
					if(atoi(ssap->SiteSurvey[i].channel) < 0 )
					{
						fprintf(fp, "\"ERR_BNAD\",");
					}else if( atoi(ssap->SiteSurvey[i].channel) > 0 && atoi(ssap->SiteSurvey[i].channel) < 14)
					{
						fprintf(fp, "\"2G\",");
					}else if( atoi(ssap->SiteSurvey[i].channel) > 14 && atoi(ssap->SiteSurvey[i].channel) < 166)
					{
						fprintf(fp, "\"5G\",");
					}
					else{
						fprintf(fp, "\"ERR_BNAD\",");
					}

					if (strlen(ssap->SiteSurvey[i].ssid) == 0){
						fprintf(fp, "\"\",");
					}else{
						//memset(ssid_str, 0, sizeof(ssid_str));
						//char_to_ascii(ssid_str, ssap->SiteSurvey[i].ssid);
						//fprintf(fp, "\"%s\",", ssid_str);

						memset(tmp, 0, sizeof(tmp));
						memset(ssid_str, 0, sizeof(ssid_str));

						strncpy(tmp,ssap->SiteSurvey[i].ssid,strlen(trim_r(ssap->SiteSurvey[i].ssid)));
						char_to_ascii_with_utf8(ssid_str, tmp);
						//strncpy(ssid_str,ssap->SiteSurvey[i].ssid,strlen(trim_r(ssap->SiteSurvey[i].ssid)));
						fprintf(fp, "\"%s\",", ssid_str);
					}

					fprintf(fp, "\"%d\",", atoi(ssap->SiteSurvey[i].channel));

					snprintf(buf, sizeof(buf), "%s", ssap->SiteSurvey[i].security);
					sscanf(buf, "%[A-Z0-9]/%[A-Z]", auth, enc);
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
					if(!strcmp(auth,"WPA3PSK"))
						fprintf(fp, "\"%s\",","WPA3-Personal");
					else if(!strcmp(auth,"WPA2PSKWPA3PSK"))
						fprintf(fp, "\"%s\",","WPA2PSKWPA3PSK");
					else
#endif
					if(!strcmp(auth,"WPAPSKWPA2PSK"))
						fprintf(fp, "\"%s\",","WPA2-Personal");
					else if(!strcmp(auth,"WPA2PSK"))
						fprintf(fp, "\"%s\",","WPA2-Personal");
					else if(!strcmp(auth,"WPAPSK"))
						fprintf(fp, "\"%s\",","WPA-Personal");
					else if(!strcmp(auth,"WPA2"))
						fprintf(fp, "\"%s\",","WPA2-Enterprise");
					else if(!strcmp(auth,"WPA"))
						fprintf(fp, "\"%s\",","WPA-Enterprise");
					else if(!strcmp(auth,"OPEN")) {
						if(!strcmp(enc, "WEP"))
							fprintf(fp, "\"%s\",","Unknown");
						else
							fprintf(fp, "\"%s\",","Open System");
					}
					else
						fprintf(fp, "\"%s\",","Unknown");

					if(!strcmp(enc, "NONE"))
						fprintf(fp, "\"%s\",", "NONE");
					else if(!strcmp(enc, "WEP"))
						fprintf(fp, "\"%s\",", "WEP");
					else if(!strcmp(enc, "TKIP"))
						fprintf(fp, "\"%s\",", "TKIP");
					else if(!strcmp(enc, "AES"))
						fprintf(fp, "\"%s\",", "AES");
					else if(!strcmp(enc, "TKIPAES"))
						fprintf(fp, "\"%s\",", "TKIPAES");
					else
						fprintf(fp, "\"%s\",", "UNKNOW");

#if 0
					if (apinfos[i].wpa == 1){
						if (apinfos[i].wid.key_mgmt == WPA_KEY_MGMT_IEEE8021X_)
							fprintf(fp, "\"%s\",", "WPA");
						else if (apinfos[i].wid.key_mgmt == WPA_KEY_MGMT_IEEE8021X2_)
							fprintf(fp, "\"%s\",", "WPA2");
						else if (apinfos[i].wid.key_mgmt == WPA_KEY_MGMT_PSK_)
							fprintf(fp, "\"%s\",", "WPA-PSK");
						else if (apinfos[i].wid.key_mgmt == WPA_KEY_MGMT_PSK2_)
							fprintf(fp, "\"%s\",", "WPA2-PSK");
						else if (apinfos[i].wid.key_mgmt == WPA_KEY_MGMT_NONE_)
							fprintf(fp, "\"%s\",", "NONE");
						else if (apinfos[i].wid.key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA_)
							fprintf(fp, "\"%s\",", "IEEE 802.1X");
						else
							fprintf(fp, "\"%s\",", "Unknown");
					}else if (apinfos[i].wep == 1){
						fprintf(fp, "\"%s\",", "Unknown");
					}else{
						fprintf(fp, "\"%s\",", "Open System");
					}

					if (apinfos[i].wpa == 1){
						if (apinfos[i].wid.pairwise_cipher == WPA_CIPHER_NONE_)
							fprintf(fp, "\"%s\",", "NONE");
						else if (apinfos[i].wid.pairwise_cipher == WPA_CIPHER_WEP40_)
							fprintf(fp, "\"%s\",", "WEP");
						else if (apinfos[i].wid.pairwise_cipher == WPA_CIPHER_WEP104_)
							fprintf(fp, "\"%s\",", "WEP");
						else if (apinfos[i].wid.pairwise_cipher == WPA_CIPHER_TKIP_)
							fprintf(fp, "\"%s\",", "TKIP");
						else if (apinfos[i].wid.pairwise_cipher == WPA_CIPHER_CCMP_)
							fprintf(fp, "\"%s\",", "AES");
						else if (apinfos[i].wid.pairwise_cipher == WPA_CIPHER_TKIP_|WPA_CIPHER_CCMP_)
							fprintf(fp, "\"%s\",", "TKIP+AES");
						else
							fprintf(fp, "\"%s\",", "Unknown");
					}else if (apinfos[i].wep == 1){
						fprintf(fp, "\"%s\",", "WEP");
					}else{
						fprintf(fp, "\"%s\",", "NONE");
					}
#endif
					fprintf(fp, "\"%d\",", atoi(ssap->SiteSurvey[i].signal));
					fprintf(fp, "\"%s\",", ssap->SiteSurvey[i].bssid);
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
					if(strncmp(ssap->SiteSurvey[i].wmode, "11b/g/n/ax", 10) == 0 )
						fprintf(fp, "\"%s\",", "ax");
					else if(strncmp(ssap->SiteSurvey[i].wmode, "11a/n/ac/ax", 10) == 0)
						fprintf(fp, "\"%s\",", "ax");
					else if(strncmp(ssap->SiteSurvey[i].wmode, "11ax", 4) == 0)
						fprintf(fp, "\"%s\",", "ax");
					else
#endif
					if(strncmp(ssap->SiteSurvey[i].wmode, "11a/n/ac", 8) == 0 || strncmp(ssap->SiteSurvey[i].wmode, "11ac", 4) == 0)
						fprintf(fp, "\"%s\",", "ac");
					else if(strncmp(ssap->SiteSurvey[i].wmode, "11b/g/n", 7) == 0)
						fprintf(fp, "\"%s\",", "bgn");
					else if(strncmp(ssap->SiteSurvey[i].wmode, "11a/n", 5) == 0)
						fprintf(fp, "\"%s\",", "an");
					else if(strncmp(ssap->SiteSurvey[i].wmode, "11b/g", 5) == 0)
						fprintf(fp, "\"%s\",", "bg");
					else if(strncmp(ssap->SiteSurvey[i].wmode, "11a", 3) == 0)
						fprintf(fp, "\"%s\",", "a");
					else if(strncmp(ssap->SiteSurvey[i].wmode, "11b", 3) == 0)
						fprintf(fp, "\"%s\",", "b");
					else if(strncmp(ssap->SiteSurvey[i].wmode, "11n", 3) == 0)
						fprintf(fp, "\"%s\",", "n");
					else
						fprintf(fp, "\"%s\",", "");
#if 0
					if (apinfos[i].NetworkType == Ndis802_11FH || apinfos[i].NetworkType == Ndis802_11DS)
						fprintf(fp, "\"%s\",", "b");
					else if (apinfos[i].NetworkType == Ndis802_11OFDM5)
						fprintf(fp, "\"%s\",", "a");
					else if (apinfos[i].NetworkType == Ndis802_11OFDM5_N)
						fprintf(fp, "\"%s\",", "an");
					else if (apinfos[i].NetworkType == Ndis802_11OFDM5_VHT)
						fprintf(fp, "\"%s\",", "ac");
					else if (apinfos[i].NetworkType == Ndis802_11OFDM24)
						fprintf(fp, "\"%s\",", "bg");
					else if (apinfos[i].NetworkType == Ndis802_11OFDM24_N)
						fprintf(fp, "\"%s\",", "bgn");
					else
						fprintf(fp, "\"%s\",", "");
#endif
					if (strcmp(nvram_safe_get(wlc_nvname("ssid")), ssap->SiteSurvey[i].ssid)){
						if (strcmp(ssap->SiteSurvey[i].ssid, ""))
							fprintf(fp, "\"%s\"", "0");				// none
						else if (!strcmp(ure_mac, ssap->SiteSurvey[i].bssid)){
							// hidden AP (null SSID)
							if (strstr(nvram_safe_get(wlc_nvname("akm")), "psk")){
								if (wl_authorized){
									// in profile, connected
									fprintf(fp, "\"%s\"", "4");
								}else{
									// in profile, connecting
									fprintf(fp, "\"%s\"", "5");
								}
							}else{
								// in profile, connected
								fprintf(fp, "\"%s\"", "4");
							}
						}else{
							// hidden AP (null SSID)
							fprintf(fp, "\"%s\"", "0");				// none
						}
					}else if (!strcmp(nvram_safe_get(wlc_nvname("ssid")), ssap->SiteSurvey[i].ssid)){
						if (!strlen(ure_mac)){
							// in profile, disconnected
							fprintf(fp, "\"%s\",", "1");
						}else if (!strcmp(ure_mac, ssap->SiteSurvey[i].bssid)){
							if (strstr(nvram_safe_get(wlc_nvname("akm")), "psk")){
								if (wl_authorized){
									// in profile, connected
									fprintf(fp, "\"%s\"", "2");
								}else{
									// in profile, connecting
									fprintf(fp, "\"%s\"", "3");
								}
							}else{
								// in profile, connected
								fprintf(fp, "\"%s\"", "2");
							}
						}else{
							fprintf(fp, "\"%s\"", "0");				// impossible...
						}
					}else{
						// wl0_ssid is empty
						fprintf(fp, "\"%s\"", "0");
					}

					if (i == apCount - 1){
						fprintf(fp, "\n");
					}else{
						fprintf(fp, "\n");
					}
				}	/* for */
				fclose(fp);
			}
		}	/* if */
	}
	else
	{
		dbg("no ap!!\n");
		return 0;
	}

	return 1;
}

int startScan(int band)
{
	const char *ifname;
	char prefix[] = "wlXXXXXXXXXX_";
#if defined(RTCONFIG_AMAS)
	if(aimesh_re_node())
#elif defined(RTCONFIG_EASYMESH) || defined(RTCONFIG_SWRTMESH)
	if(mesh_re_node())
#else
	if(0)
#endif
	{
    	snprintf(prefix, sizeof(prefix), "wlc%d_", band);
		ifname = get_staifname(band);
		if(ra_get_channel(band))
			eval("iwpriv", (char *)ifname, "set", "Channel=0");
	}else{
    	snprintf(prefix, sizeof(prefix), "wl%d_", band);
		ifname = get_wifname(band);
	}
	if(set_site_survey(ifname, nvram_pf_safe_get(prefix, "ssid")))
		return 0;
	sleep(4);
	return 1;
}

int getSiteSurveyVSIEcount(int band)
{
	const char *ifname;
	char data[5];
	struct iwreq wrq;
#if defined(RTCONFIG_AMAS)
	if(aimesh_re_node())
#elif defined(RTCONFIG_EASYMESH) || defined(RTCONFIG_SWRTMESH)
	if(mesh_re_node())
#else
	if(0)
#endif
		ifname = get_staifname(band);
	else
		ifname = get_wifname(band);
	memset(data, 0, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = ASUS_SUBCMD_GETSITESURVEY_VSIE_COUNT;

	if(wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0){
		dbg("errors in getting site survey vie result\n");
	}else{
    	sleep(2);
    	return atoi(data);
	}
	return 0;
}

int getSiteSurveyVSIE(int band, char *buf, int length)
{
	const char *ifname;
	char data[SITE_SURVEY_APS_MAX];
	struct iwreq wrq;
	struct _SITESURVEY_VSIE *result = (struct _SITESURVEY_VSIE *)buf;
#if defined(RTCONFIG_AMAS)
	if(aimesh_re_node())
#elif defined(RTCONFIG_EASYMESH) || defined(RTCONFIG_SWRTMESH)
	if(mesh_re_node())
#else
	if(0)
#endif
		ifname = get_staifname(band);
	else
		ifname = get_wifname(band);
	memset(data, 0, length);
	wrq.u.data.length = length;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = ASUS_SUBCMD_GETSITESURVEY_VSIE;

	if (wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0)
	{
		dbg("errors in getting site survey vie result\n");
		return 0;
	}

	if(result == NULL){
		dbg("The output var result is NULL\n");
		return 0;
	}
	memcpy(result, data, length);
	return 1;
}

int getBSSID(int band)	// get AP's BSSID
{
	unsigned char data[MACSIZE];
	char macaddr[18];
	struct iwreq wrq;

	memset(data, 0, MACSIZE);
	wrq.u.data.length = MACSIZE;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = OID_802_11_BSSID;

	if (wl_ioctl(get_wifname(band), RT_PRIV_IOCTL, &wrq) < 0)
	{
		dbg("errors in getting bssid!\n");
		return -1;
	}
	else
	{
		ether_etoa(data, macaddr);
		puts(macaddr);
		return 0;
	}
}

#if defined(RTCONFIG_WIRELESSREPEATER)
int site_survey_for_channel(int band, const char *wif)
{
	int idx = -1;
	int i = 0, apCount = 0;
	int commonchannel, signal_max = -1, signal_tmp = -1;
	char prefix[] = "wlXXXXXXXXXX_";
	char *ssid;
	char data[SITE_SURVEY_APS_MAX];
	char commch[4];
	SSA *ssap = NULL;

#ifdef RTCONFIG_WISP
	if(wisp_mode())
		snprintf(prefix, sizeof(prefix), "wlc_");
	else
#endif
		snprintf(prefix, sizeof(prefix), "wl%d_", band);
	ssid = nvram_pf_safe_get(prefix, "ssid");

	if (!ssid || !strcmp(ssid, "") || set_site_survey(wif, ssid)) {
		return -1;
	}

	fprintf(stderr, "Look for SSID: %s\n", ssid);
	fprintf(stderr, "Please wait");
	if(use_partial_scan(wif)){
		do{
			sleep(1);
			fprintf(stderr, ".");
		}while(partial_scan_status(wif));
	}else{
		sleep(1);
		fprintf(stderr, ".");
		sleep(1);
		fprintf(stderr, ".");
		sleep(1);
		fprintf(stderr, ".");
		sleep(1);
		fprintf(stderr, ".");
	}
	fprintf(stderr, "\n");
	if(get_site_survey(wif, data, sizeof(data), &ssap, &apCount) || !apCount)
		return -1;
	if (apCount) {
		for (i = 0; i < apCount; i++) {
			memset(commch,0,sizeof(commch));
			memcpy(commch, ssap->SiteSurvey[i].channel, 3);
			commonchannel=atoi(commch);
			if ((ssid && !strcmp(ssid, trim_r(ssap->SiteSurvey[i].ssid)))/*non-hidden AP*/
			 ) {
				if (!strncmp(ssap->SiteSurvey[i].bssid, nvram_pf_safe_get(prefix, "bssid"), 17)) {
						return commonchannel;
				}
				else if ((signal_tmp = atoi(trim_r(ssap->SiteSurvey[i].signal))) > signal_max) {
					signal_max = signal_tmp;
					idx = commonchannel;
				}
			}
		}
	}
	fprintf(stderr, "\n");
	return idx;
}
#endif	/* RTCONFIG_WIRELESSREPEATER */

int ra_get_channel(int band)
{
	int channel;
	struct iw_range	range;
	double freq;
	struct iwreq wrq1;
	struct iwreq wrq2;

	if (wl_ioctl(get_wifname(band), SIOCGIWFREQ, &wrq1) < 0)
		return 0;

	char buffer[sizeof(iwrange) * 2];
	memset(buffer, 0, sizeof(buffer));
	wrq2.u.data.pointer = (caddr_t) buffer;
	wrq2.u.data.length = sizeof(buffer);
	wrq2.u.data.flags = 0;

	if (wl_ioctl(get_wifname(band), SIOCGIWRANGE, &wrq2) < 0)
		return 0;

	if (ralink_get_range_info(&range, buffer, wrq2.u.data.length) < 0)
		return 0;

	freq = iw_freq2float(&(wrq1.u.freq));
	if (freq < KILO)
		channel = (int) freq;
	else
	{
		channel = iw_freq_to_channel(freq, &range);
		if (channel < 0)
			return 0;
	}

	return channel;
}

int
asuscfe(const char *PwqV, const char *IF)
{
	if (strcmp(PwqV, "stat") == 0)
	{
		eval("iwpriv", (char*) IF, "stat");
	}
	else if (strstr(PwqV, "=") && strstr(PwqV, "=") != PwqV)
	{
		eval("iwpriv", (char*) IF, "set", (char*) PwqV);
		puts("1");
	}
	return 0;
}

int __need_to_start_wps_band(char *prefix)
{
	char *p, tmp[128];

	if (!prefix || *prefix == '\0')
		return 0;

	p = nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp));
	if ((!strcmp(p, "open") && !nvram_match(strcat_r(prefix, "wep_x", tmp), "0")) ||
	    !strcmp(p, "shared") || !strcmp(p, "psk") || !strcmp(p, "wpa") ||
	    !strcmp(p, "wpa2") || !strcmp(p, "wpawpa2") || !strcmp(p, "radius") ||
	    nvram_match(strcat_r(prefix, "radio", tmp), "0") ||
	    !((sw_mode() == SW_MODE_ROUTER) || (sw_mode() == SW_MODE_AP)))
		return 0;

	return 1;
}

int need_to_start_wps_band(int wps_band)
{
	int ret = 1;
	char prefix[] = "wlXXXXXXXXXX_";

	switch (wps_band) {
	case 0:	/* fall through */
	case 1:
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
	char prefix[] = "wlXXXXXXXXXX_", word[256], *next, ifnames[128];
	int wps_band = nvram_get_int("wps_band_x"), multiband = get_wps_multiband();

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach (word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);
		snprintf(prefix, sizeof(prefix), "wl%d_", i);

		if (!need_to_start_wps_band(i)) {
			++i;
			continue;
		}

//		dbg("WPS: PIN\n");
		doSystem("iwpriv %s set WscMode=1", get_wifname(i));

		if (pincode == 0) {
			doSystem("iwpriv %s set WscGetConf=%d", get_wifname(i), 1);	// Trigger WPS AP to do simple config with WPS Client
		} else {
			doSystem("iwpriv %s set WscPinCode=%08d", get_wifname(i), pincode);
		}

		++i;
	}

	return 0;
}

static int
__wps_pbc(const int multiband)
{
	int i;
	char prefix[] = "wlXXXXXXXXXX_", word[256], *next, ifnames[128];
	int wps_band = nvram_get_int("wps_band_x");

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach (word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);
		snprintf(prefix, sizeof(prefix), "wl%d_", i);

		if (!need_to_start_wps_band(i)) {
			++i;
			continue;
		}

//		dbg("WPS: PBC\n");
		g_isEnrollee[i] = 1;
		doSystem("iwpriv %s set WscMode=%d", get_wifname(i), 2);		// PBC method
		doSystem("iwpriv %s set WscGetConf=%d", get_wifname(i), 1);	// Trigger WPS AP to do simple config with WPS Client

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
#else
	if (!__need_to_start_wps_band("wl1") || !__need_to_start_wps_band("wl0")) return 0;

//	dbg("WPS: PBC\n");
#if defined(RTCONFIG_HAS_5G)
	g_isEnrollee[1] = 1;
	doSystem("iwpriv %s set WscMode=%d", get_wifname(1), 2);		// PBC method
	doSystem("iwpriv %s set WscGetConf=%d", get_wifname(1), 1);		// Trigger WPS AP to do simple config with WPS Client
#endif	/* RTCONFIG_HAS_5G */

	g_isEnrollee[0] = 1;
	doSystem("iwpriv %s set WscMode=%d", get_wifname(0), 2);		// PBC method
	doSystem("iwpriv %s set WscGetConf=%d", get_wifname(0), 1);		// Trigger WPS AP to do simple config with WPS Client

	return 0;
#endif
}

extern void wl_default_wps(int unit);

void __wps_oob(const int multiband)
{
	int i, wps_band = nvram_get_int("wps_band_x");
	char tmp[128], prefix[] = "wlXXXXXXXXXX_", word[256], *next;
	char *p, ifnames[128];

	if (nvram_match("lan_ipaddr", ""))
		return;

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach (word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);
		snprintf(prefix, sizeof(prefix), "wl%d_", i);
		nvram_set("w_Setting", "0");
		if (!i)
			nvram_set("wl0_wsc_config_state", "0");
		else
			nvram_set("wl1_wsc_config_state", "0");

		wl_default_wps(i);
		nvram_commit();

		snprintf(prefix, sizeof(prefix), "wl%d_", wps_band);

		doSystem("iwpriv %s set AuthMode=%s", get_wifname(i), "OPEN");
		doSystem("iwpriv %s set EncrypType=%s", get_wifname(i), "NONE");
		doSystem("iwpriv %s set IEEE8021X=%d", get_wifname(i), 0);
		if (strlen((p = nvram_safe_get(strcat_r(prefix, "key1", tmp)))))
			iwprivSet(get_wifname(i), "Key1", p);
		if (strlen((p = nvram_safe_get(strcat_r(prefix, "key2", tmp)))))
			iwprivSet(get_wifname(i), "Key2", p);
		if (strlen((p = nvram_safe_get(strcat_r(prefix, "key3", tmp)))))
			iwprivSet(get_wifname(i), "Key3", p);
		if (strlen((p = nvram_safe_get(strcat_r(prefix, "key4", tmp)))))
			iwprivSet(get_wifname(i), "Key4", p);
		doSystem("iwpriv %s set DefaultKeyID=%s", get_wifname(i), nvram_safe_get(strcat_r(prefix, "key", tmp)));
		iwprivSet(get_wifname(i), "SSID", nvram_safe_get(strcat_r(prefix, "ssid", tmp)));
		doSystem("iwpriv %s set WscStop=1", get_wifname(i));	// Stop WPS Process.
		doSystem("iwpriv %s set WscConfMode=%d", get_wifname(i), 7);		// WPS enabled. Force WPS status to change
		doSystem("iwpriv %s set WscConfStatus=%d", get_wifname(i), 1);		// AP is unconfigured

		g_isEnrollee[i] = 0;

		++i;
	}
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
	if (nvram_match("lan_ipaddr", ""))
		return;

//	if (!__need_to_start_wps_band("wl1") || !__need_to_start_wps_band("wl0")) return 0;

	nvram_set("w_Setting", "0");
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	nvram_set("wl0_wsc_config_state", "0");
	nvram_set("wl1_wsc_config_state", "0");
#endif
	wl_defaults_wps();
	nvram_commit();

#if defined (W7_LOGO) || defined (wifi_LOGO)
#if defined(RTCONFIG_HAS_5G)
	doSystem("iwpriv %s set AuthMode=%s", get_wifname(1), "OPEN");
	doSystem("iwpriv %s set EncrypType=%s", get_wifname(1), "NONE");
	doSystem("iwpriv %s set IEEE8021X=%d", get_wifname(1), 0);
	if (strlen(nvram_safe_get("wl1_key1")))
		iwprivSet(get_wifname(1), "Key1", nvram_safe_get("wl1_key1"));
	if (strlen(nvram_safe_get("wl1_key2")))
		iwprivSet(get_wifname(1), "Key2", nvram_safe_get("wl1_key2"));
	if (strlen(nvram_safe_get("wl1_key3")))
		iwprivSet(get_wifname(1), "Key3", nvram_safe_get("wl1_key3"));
	if (strlen(nvram_safe_get("wl1_key4")))
		iwprivSet(get_wifname(1), "Key4", nvram_safe_get("wl1_key4"));
	doSystem("iwpriv %s set DefaultKeyID=%s", get_wifname(1), nvram_safe_get("wl1_key"));
	iwprivSet(get_wifname(1), "SSID", nvram_safe_get("wl1_ssid"));
#endif	/* RTCONFIG_HAS_5G */

	doSystem("iwpriv %s set AuthMode=%s", get_wifname(0), "OPEN");
	doSystem("iwpriv %s set EncrypType=%s", get_wifname(0), "NONE");
	doSystem("iwpriv %s set IEEE8021X=%d", get_wifname(0), 0);
	if (strlen(nvram_safe_get("wl0_key1")))
		iwprivSet(get_wifname(0), "Key1", nvram_safe_get("wl0_key1"));
	if (strlen(nvram_safe_get("wl0_key2")))
		iwprivSet(get_wifname(0), "Key2", nvram_safe_get("wl0_key2"));
	if (strlen(nvram_safe_get("wl0_key3")))
		iwprivSet(get_wifname(0), "Key3", nvram_safe_get("wl0_key3"));
	if (strlen(nvram_safe_get("wl0_key4")))
		iwprivSet(get_wifname(0), "Key4", nvram_safe_get("wl0_key4"));
	doSystem("iwpriv %s set DefaultKeyID=%s", get_wifname(0), nvram_safe_get("wl0_key"));
	iwprivSet(get_wifname(0), "SSID", nvram_safe_get("wl0_ssid"));

	eval("route", "delete", "239.255.255.250");

	kill_pidfile_s_rm(get_wscd_pidfile_band(0), SIGKILL);

#if defined(RTCONFIG_HAS_5G)
	kill_pidfile_s_rm(get_wscd_pidfile_band(1), SIGKILL);
#if defined(RTCONFIG_RALINK_RT3883) || defined(RTCONFIG_RALINK_RT3052)
		doSystem("iwpriv %s set WscConfMode=%d", get_wifname(1), 0);		// WPS disabled
#else
		doSystem("iwpriv %s set WscStop=1", get_wifname(1));	// Stop WPS Process.
#endif
	doSystem("iwpriv %s set WscConfMode=%d", get_wifname(1), 7);		// Enrollee + Proxy + Registrar
	doSystem("iwpriv %s set WscConfStatus=%d", get_wifname(1), 1);		// AP is unconfigured
#endif	/* RTCONFIG_HAS_5G */

#if defined(RTCONFIG_RALINK_RT3883) || defined(RTCONFIG_RALINK_RT3052)
		doSystem("iwpriv %s set WscConfMode=%d", get_wifname(0), 0);		// WPS disabled
#else
		doSystem("iwpriv %s set WscStop=1", get_wifname(0));			// Stop WPS Process.
#endif
	doSystem("iwpriv %s set WscConfMode=%d", get_wifname(0), 7);		// Enrollee + Proxy + Registrar
	doSystem("iwpriv %s set WscConfStatus=%d", get_wifname(0), 1);		// AP is unconfigured

	g_wsc_configured = 0;

	eval("route", "add", "-host", "239.255.255.250", "dev", "br0");

	char str_lan_ipaddr[16];
	strcpy(str_lan_ipaddr, nvram_safe_get("lan_ipaddr"));

#if defined(RTCONFIG_HAS_5G)
	doSystem("wscd -m 1 -a %s -i %s &", str_lan_ipaddr, get_wifname(1));
	doSystem("iwpriv %s set WscMode=1", get_wifname(1));			// PIN method
//	doSystem("iwpriv %s set WscGetConf=%d", get_wifname(1), 1);		// Trigger WPS AP to do simple config with WPS Client
#endif	/* RTCONFIG_HAS_5G */

	doSystem("wscd -m 1 -a %s -i %s &", str_lan_ipaddr, get_wifname(0));
	doSystem("iwpriv %s set WscMode=1", get_wifname(0));			// PIN method

#else
#if defined(RTCONFIG_HAS_5G)
	doSystem("iwpriv %s set AuthMode=%s", get_wifname(1), "OPEN");
	doSystem("iwpriv %s set EncrypType=%s", get_wifname(1), "NONE");
	doSystem("iwpriv %s set IEEE8021X=%d", get_wifname(1), 0);
	if (strlen(nvram_safe_get("wl1_key1")))
		iwprivSet(get_wifname(1), "Key1", nvram_safe_get("wl1_key1"));
	if (strlen(nvram_safe_get("wl1_key2")))
		iwprivSet(get_wifname(1), "Key2", nvram_safe_get("wl1_key2"));
	if (strlen(nvram_safe_get("wl1_key3")))
		iwprivSet(get_wifname(1), "Key3", nvram_safe_get("wl1_key3"));
	if (strlen(nvram_safe_get("wl1_key4")))
		iwprivSet(get_wifname(1), "Key4", nvram_safe_get("wl1_key4"));
	doSystem("iwpriv %s set DefaultKeyID=%s", get_wifname(1), nvram_safe_get("wl1_key"));
	iwprivSet(get_wifname(1), "SSID", nvram_safe_get("wl1_ssid"));

	doSystem("iwpriv %s set WscStop=1", get_wifname(1));			// Stop WPS Process.
	doSystem("iwpriv %s set WscConfMode=%d", get_wifname(1), 7);		// WPS enabled. Force WPS status to change
	doSystem("iwpriv %s set WscConfStatus=%d", get_wifname(1), 1);		// AP is unconfigured
#endif	/* RTCONFIG_HAS_5G */

	doSystem("iwpriv %s set AuthMode=%s", get_wifname(0), "OPEN");
	doSystem("iwpriv %s set EncrypType=%s", get_wifname(0), "NONE");
	doSystem("iwpriv %s set IEEE8021X=%d", get_wifname(0), 0);
	if (strlen(nvram_safe_get("wl0_key1")))
		iwprivSet(get_wifname(0), "Key1", nvram_safe_get("wl0_key1"));
	if (strlen(nvram_safe_get("wl0_key2")))
		iwprivSet(get_wifname(0), "Key2", nvram_safe_get("wl0_key2"));
	if (strlen(nvram_safe_get("wl0_key3")))
		iwprivSet(get_wifname(0), "Key3", nvram_safe_get("wl0_key3"));
	if (strlen(nvram_safe_get("wl0_key4")))
		iwprivSet(get_wifname(0), "Key4", nvram_safe_get("wl0_key4"));
	doSystem("iwpriv %s set DefaultKeyID=%s", get_wifname(0), nvram_safe_get("wl0_key"));
	iwprivSet(get_wifname(0), "SSID", nvram_safe_get("wl0_ssid"));
	doSystem("iwpriv %s set WscStop=1", get_wifname(0));			// Stop WPS Process.
	doSystem("iwpriv %s set WscConfMode=%d", get_wifname(0), 7);		// WPS enabled. Force WPS status to change
	doSystem("iwpriv %s set WscConfStatus=%d", get_wifname(0), 1);		// AP is unconfigured
#endif
	g_isEnrollee[0] = 0;
#endif	/* RTCONFIG_WPSMULTIBAND */
}

void start_wsc(void)
{
	int i;
	char *wps_sta_pin = nvram_safe_get("wps_sta_pin");
	char str_lan_ipaddr[16];
	char prefix[] = "wlXXXXXXXXXX_", word[256], *next, ifnames[128];
	int wps_band = nvram_get_int("wps_band_x"), multiband = get_wps_multiband();

	if (nvram_match("lan_ipaddr", ""))
		return;

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach (word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);
		snprintf(prefix, sizeof(prefix), "wl%d_", i);

		if (!need_to_start_wps_band(i)) {
			++i;
			continue;
		}

		eval("route", "delete", "239.255.255.250");
		kill_pidfile_s_rm(get_wscd_pidfile_band(i), SIGKILL, 1);

		dbg("%s: start wsc(%d)\n", __func__, i);

		doSystem("iwpriv %s set WscStop=1", get_wifname(i));			// Stop WPS Process.
		doSystem("iwpriv %s set WscConfMode=%d", get_wifname(i), 7);	// Enrollee + Proxy + Registrar

		eval("route", "add", "-host", "239.255.255.250", "dev", "br0");
		strcpy(str_lan_ipaddr, nvram_safe_get("lan_ipaddr"));
		doSystem("wscd -m 1 -a %s -i %s &", str_lan_ipaddr, get_wifname(i));

//#if defined (W7_LOGO) || defined (wifi_LOGO)
		if (strlen(wps_sta_pin) && strcmp(wps_sta_pin, "00000000") && (wl_wpsPincheck(wps_sta_pin) == 0)) {
			dbg("WPS: PIN\n");					// PIN method
			g_isEnrollee[i] = 0;
			doSystem("iwpriv %s set WscMode=1", get_wifname(i));
			doSystem("iwpriv %s set WscPinCode=%s", get_wifname(i), wps_sta_pin);
		}
		else {
			dbg("WPS: PBC\n");					// PBC method
			g_isEnrollee[i] = 1;
			doSystem("iwpriv %s set WscMode=2", get_wifname(i));
//			doSystem("iwpriv %s set WscPinCode=%s", get_wifname(i), "00000000");
		}

		doSystem("iwpriv %s set WscGetConf=%d", get_wifname(i), 1);	// Trigger WPS AP to do simple config with WPS Client
//#endif
		sleep(2);
		++i;
	}
}

void
start_wsc_pin_enrollee(void)
{
	int i;
	char str_lan_ipaddr[16];
	char prefix[] = "wlXXXXXXXXXX_", word[256], *next, ifnames[128];
	int wps_band = nvram_get_int("wps_band_x"), multiband = get_wps_multiband();

	if (nvram_match("lan_ipaddr", "")) {
		nvram_set("wps_enable", "0");
		return;
	}

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach (word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);
		snprintf(prefix, sizeof(prefix), "wl%d_", i);

		if (!need_to_start_wps_band(i)) {
			++i;
			continue;
		}

		eval("route", "add", "-host", "239.255.255.250", "dev", "br0");
		kill_pidfile_s_rm(get_wscd_pidfile_band(i), SIGKILL, 1);

		dbg("%s: start wsc (%d)\n", __func__, i);

		strcpy(str_lan_ipaddr, nvram_safe_get("lan_ipaddr"));
		doSystem("wscd -m 1 -a %s -i %s &", str_lan_ipaddr, get_wifname(i));
		doSystem("iwpriv %s set WscConfMode=%d", get_wifname(i), 7);		// Enrollee + Proxy + Registrar

		dbg("WPS: PIN\n");
		doSystem("iwpriv %s set WscMode=1", get_wifname(i));

		++i;
	}
}

static void __stop_wsc(int multiband)
{
	int i;
	char prefix[] = "wlXXXXXXXXXX_", word[256], *next, ifnames[128];
	int wps_band = nvram_get_int("wps_band_x");

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach (word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
		if (!multiband && wps_band != i) {
			++i;
			continue;
		}
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);
		snprintf(prefix, sizeof(prefix), "wl%d_", i);

		if (!need_to_start_wps_band(i)) {
			++i;
			continue;
		}

		eval("route", "delete", "239.255.255.250");
		kill_pidfile_s_rm(get_wscd_pidfile_band(i), SIGKILL, 1);
		doSystem("iwpriv %s set WscStop=1", get_wifname(i));			// Stop WPS Process.
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
#else
	if (!__need_to_start_wps_band("wl0")
#if defined(RTCONFIG_HAS_5G)
 && !__need_to_start_wps_band("wl1")
#endif	/* RTCONFIG_HAS_5G */
	   )
		return;

	system("route delete 239.255.255.250 1>/dev/null 2>&1");

	kill_pidfile_s_rm(get_wscd_pidfile_band(0), SIGKILL, 1);

#if defined(RTCONFIG_HAS_5G)
	kill_pidfile_s_rm(get_wscd_pidfile_band(1), SIGKILL, 1);
		doSystem("iwpriv %s set WscStop=1", get_wifname(1));			// Stop WPS Process.
#endif
		doSystem("iwpriv %s set WscStop=1", get_wifname(0));			// Stop WPS Process.
#endif
}

int getWscStatus(int unit)
{
	int data = 0;
	struct iwreq wrq;
	int wps_band = unit? 1:0;

	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;

	if (wl_ioctl(get_wifname(wps_band), RT_PRIV_IOCTL, &wrq) < 0)
		dbg("errors in getting WSC status\n");

	return data;
}

int getWscProfile(char *interface, WSC_CONFIGURED_VALUE *data, int len)
{
	int socket_id;
	struct iwreq wrq;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy((char *)data, "get_wsc_profile");
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = len;
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = 0;
	ioctl(socket_id, RTPRIV_IOCTL_WSC_PROFILE, &wrq);
	close(socket_id);
	return 0;
}

int stainfo(int band)
{
	char data[2048];
	struct iwreq wrq;

	memset(data, 0, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = ASUS_SUBCMD_GSTAINFO;

	if (wl_ioctl(get_wifname(band), RTPRIV_IOCTL_ASUSCMD, &wrq) < 0)
	{
		dbg("errors in getting STAINFO result\n");
		return 0;
	}

	if (wrq.u.data.length > 0)
	{
		puts(wrq.u.data.pointer);
	}

	return 0;
}

int getstat(int band)
{
	char data[4096];
	struct iwreq wrq;

	memset(data, 0, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = ASUS_SUBCMD_GSTAT;

	if (wl_ioctl(get_wifname(band), RTPRIV_IOCTL_ASUSCMD, &wrq) < 0)
	{
		dbg("errors in getting STAT result\n");
		return 0;
	}

	if (wrq.u.data.length > 0)
	{
		puts(wrq.u.data.pointer);
	}

	return 0;
}

void wsc_user_commit(void)
{
	int i, flag_wep;
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";
	char auth_mode[16], wep[16];
	const char *wif;

	for (i = 0; i < MAX_NR_WL_IF; ++i) {
#if !defined(RTCONFIG_HAS_5G_2)
		if (i == 2)
			continue;
#endif
		sprintf(prefix, "wl%d_", i);
		if (!nvram_match(strcat_r(prefix, "wsc_config_state", tmp), "2"))
			continue;

		flag_wep = 0;
		wif = get_wifname(i);
		strcpy(auth_mode, nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)));
		strcpy(wep, nvram_safe_get(strcat_r(prefix, "wep_x", tmp)));

		if (!strcmp(auth_mode, "open") && !strcmp(wep, "0")) {
			doSystem("iwpriv %s set AuthMode=%s", wif, "OPEN");
			doSystem("iwpriv %s set EncrypType=%s", wif, "NONE");

			doSystem("iwpriv %s set IEEE8021X=%d", wif, 0);
		}
		else if (!strcmp(auth_mode, "open")) {
			flag_wep = 1;
			doSystem("iwpriv %s set AuthMode=%s", wif, "OPEN");
			doSystem("iwpriv %s set EncrypType=%s", wif, "WEP");

			doSystem("iwpriv %s set IEEE8021X=%d", wif, 0);
		}
		else if (!strcmp(auth_mode, "shared")) {
			flag_wep = 1;
			doSystem("iwpriv %s set AuthMode=%s", wif, "SHARED");
			doSystem("iwpriv %s set EncrypType=%s", wif, "WEP");

			doSystem("iwpriv %s set IEEE8021X=%d", wif, 0);
		}
		else if (!strcmp(auth_mode, "psk") || !strcmp(auth_mode, "psk2") || !strcmp(auth_mode, "pskpsk2")) {
			if (!strcmp(auth_mode, "pskpsk2"))
				doSystem("iwpriv %s set AuthMode=%s", wif, "WPAPSKWPA2PSK");
			else if (!strcmp(auth_mode, "psk"))
				doSystem("iwpriv %s set AuthMode=%s", wif, "WPAPSK");
			else if (!strcmp(auth_mode, "psk2"))
				doSystem("iwpriv %s set AuthMode=%s", wif, "WPA2PSK");

			//EncrypType
			if (nvram_match(strcat_r(prefix, "crypto", tmp), "tkip"))
				doSystem("iwpriv %s set EncrypType=%s", wif, "TKIP");
			else if (nvram_match(strcat_r(prefix, "crypto", tmp), "aes"))
				doSystem("iwpriv %s set EncrypType=%s", wif, "AES");
			else if (nvram_match(strcat_r(prefix, "crypto", tmp), "tkip+aes"))
				doSystem("iwpriv %s set EncrypType=%s", wif, "TKIPAES");

			doSystem("iwpriv %s set IEEE8021X=%d", wif, 0);

			iwprivSet(wif, "SSID", nvram_safe_get(strcat_r(prefix, "ssid", tmp)));

			//WPAPSK
			iwprivSet(wif, "WPAPSK", nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp)));

			doSystem("iwpriv %s set DefaultKeyID=%s", wif, "2");
		}
		else {
			doSystem("iwpriv %s set AuthMode=%s", wif, "OPEN");
			doSystem("iwpriv %s set EncrypType=%s", wif, "NONE");
		}

		if (flag_wep) {
			//KeyStr
			if (strlen(nvram_safe_get(strcat_r(prefix, "key1", tmp))))
				iwprivSet(wif, "Key1", nvram_safe_get(strcat_r(prefix, "key1", tmp)));
			if (strlen(nvram_safe_get(strcat_r(prefix, "key2", tmp))))
				iwprivSet(wif, "Key2", nvram_safe_get(strcat_r(prefix, "key2", tmp)));
			if (strlen(nvram_safe_get(strcat_r(prefix, "key3", tmp))))
				iwprivSet(wif, "Key3", nvram_safe_get(strcat_r(prefix, "key3", tmp)));
			if (strlen(nvram_safe_get(strcat_r(prefix, "key4", tmp))))
				iwprivSet(wif, "Key4", nvram_safe_get(strcat_r(prefix, "key4", tmp)));

			//DefaultKeyID
			doSystem("iwpriv %s set DefaultKeyID=%s", wif, nvram_safe_get(strcat_r(prefix, "key", tmp)));
		}

		iwprivSet(wif, "SSID", nvram_safe_get(strcat_r(prefix, "ssid", tmp)));

		nvram_set(strcat_r(prefix, "wsc_config_state", tmp), "1");
		doSystem("iwpriv %s set WscConfStatus=%d", wif, 2);	// AP is configured
	}

//	doSystem("iwpriv %s set WscConfMode=%d", get_non_wpsifname(), 7);	// trigger Windows OS to give a popup about WPS PBC AP
}

int wl_WscConfigured(int unit)
{
	WSC_CONFIGURED_VALUE result;
	struct iwreq wrq;
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);

	wrq.u.data.length = sizeof(WSC_CONFIGURED_VALUE);
	wrq.u.data.pointer = (caddr_t) &result;
	wrq.u.data.flags = 0;
	strcpy((char *)&result, "get_wsc_profile");

	if (wl_ioctl(nvram_safe_get(strcat_r(prefix, "ifname", tmp)), RTPRIV_IOCTL_WSC_PROFILE, &wrq) < 0)
	{
		fprintf(stderr, "errors in getting WSC profile\n");
		return -1;
	}

	if (result.WscConfigured == 2)
		return 1;
	else
		return 0;
}

#ifdef RTCONFIG_WIRELESSREPEATER
/*
 * write_rpt_wpa_supplicant_conf()
 *
 * copy wifi nvram variable from prefix_wlc"xxx" to prefix_mssid"xxx"
 * and generate wpa_supplicant configure file
 */
void write_rpt_wpa_supplicant_conf(int band, const char *prefix_mssid, char *prefix_wlc, const char *addition)
{
	FILE *fp_wpa;
	char tmp[128];
	char *str;
	int flag_wep = 0;
	int i;

	snprintf(tmp, sizeof(tmp), "/etc/Wireless/wpa_supplicant-%s.conf", get_staifname(band));
	if (!(fp_wpa = fopen(tmp, "w+")))
		return;
	ifconfig(get_staifname(band), 0, NULL, NULL);
#if defined(RTCONFIG_WISP)
	if(!wisp_mode()){
#endif
	if (strlen(nvram_pf_safe_get(prefix_wlc, "ssid")))
		nvram_set("wl_ssid", nvram_pf_safe_get(prefix_wlc, "ssid"));
	nvram_pf_set(prefix_mssid, "ssid", nvram_pf_safe_get(prefix_wlc, "ssid"));
	nvram_pf_set(prefix_mssid, "auth_mode_x", nvram_pf_safe_get(prefix_wlc, "auth_mode"));

	nvram_pf_set(prefix_mssid, "wep_x", nvram_pf_safe_get(prefix_wlc, "wep"));
	nvram_pf_set(prefix_mssid, "key", nvram_pf_safe_get(prefix_wlc, "key"));
	for(i = 1; i <= 4; i++) {
		char prekey[16];
		if(nvram_pf_get_int(prefix_wlc, "key") != i)
			continue;

		snprintf(prekey, sizeof(prekey), "key%d", i);
		nvram_pf_set(prefix_mssid, prekey, nvram_pf_safe_get(prefix_wlc, "wep_key"));
	}

	nvram_pf_set(prefix_mssid, "crypto", nvram_pf_safe_get(prefix_wlc, "crypto"));
	nvram_pf_set(prefix_mssid, "wpa_psk", nvram_pf_safe_get(prefix_wlc, "wpa_psk"));
	nvram_pf_set(prefix_mssid, "bw", nvram_pf_safe_get(prefix_wlc, "nbw_cap"));
#if defined(RTCONFIG_WISP)
	}
#endif

	fprintf(fp_wpa,	"ctrl_interface=/var/run/wpa_supplicant\n%s\nnetwork={\nssid=\"%s\"\n", (addition ? : ""), 
		nvram_pf_get(prefix_wlc, "ssid") ? : "8f3610e3c9feabed953a6");

	fprintf(fp_wpa,"\tscan_ssid=1\n");

	if(nvram_pf_match(prefix_wlc, "auth_mode", "open") && nvram_pf_match(prefix_wlc, "wep", "0"))
		fprintf(fp_wpa,"\tkey_mgmt=%s\n", "NONE");		//open/none
	else if(nvram_pf_match(prefix_wlc, "auth_mode", "open")){
		flag_wep = 1;
		fprintf(fp_wpa,"\tkey_mgmt=%s\n", "NONE");		//open
		fprintf(fp_wpa,"\tauth_alg=OPEN\n");
	}else if(nvram_pf_match(prefix_wlc, "auth_mode", "shared")){
		flag_wep = 1;
		fprintf(fp_wpa,"\tkey_mgmt=%s\n", "NONE");		//shared
		fprintf(fp_wpa,"\tauth_alg=SHARED\n");
	}else if (nvram_pf_match(prefix_wlc, "auth_mode", "psk") || nvram_pf_match(prefix_wlc, "auth_mode", "psk2")){
		fprintf(fp_wpa,"\tkey_mgmt=%s\n", "WPA-PSK");
		if(nvram_pf_match(prefix_wlc, "auth_mode", "psk"))
			fprintf(fp_wpa,"\tproto=WPA\n");	//wpapsk
		else
			fprintf(fp_wpa,"\tproto=RSN\n");	//wpa2psk

		//EncrypType
		if(nvram_pf_match(prefix_wlc, "crypto", "tkip")){
			fprintf(fp_wpa, "\tpairwise=%s\n", "TKIP");
			fprintf(fp_wpa, "\tgroup=%s\n", "TKIP");
		}else if (nvram_pf_match(prefix_wlc, "crypto", "aes")){
			fprintf(fp_wpa, "\tpairwise=%s\n", "CCMP");
			fprintf(fp_wpa, "\tgroup=%s\n", "CCMP");
		}else if (nvram_pf_match(prefix_wlc, "crypto", "tkip+aes")){
			fprintf(fp_wpa, "\tpairwise=%s\n", "CCMP TKIP");
			fprintf(fp_wpa, "\tgroup=%s\n", "CCMP TKIP");
		}

		//key
		fprintf(fp_wpa, "\tpsk=\"%s\"\n",nvram_pf_safe_get(prefix_wlc, "wpa_psk"));
	}else if (nvram_pf_match(prefix_wlc, "auth_mode", "sae")){
		fprintf(fp_wpa,"\tkey_mgmt=%s\n", "SAE");
		fprintf(fp_wpa, "\tpairwise=%s\n", "CCMP");
		fprintf(fp_wpa, "\tgroup=%s\n", "CCMP");
		fprintf(fp_wpa, "\tpsk=\"%s\"\n",nvram_pf_safe_get(prefix_wlc, "wpa_psk"));
	}else if (nvram_pf_match(prefix_wlc, "auth_mode", "psk2sae")){
		fprintf(fp_wpa,"\tkey_mgmt=WPA-PSK SAE\n");
		fprintf(fp_wpa,"\tproto=RSN\n");
		fprintf(fp_wpa, "\tpairwise=%s\n", "CCMP");
		fprintf(fp_wpa, "\tgroup=%s\n", "CCMP");
		fprintf(fp_wpa, "\tpsk=\"%s\"\n",nvram_pf_safe_get(prefix_wlc, "wpa_psk"));
	}else
		fprintf(fp_wpa,"\tkey_mgmt=NONE\n");		//open/none

	//EncrypType
	if (flag_wep){
		int len;
		for(i = 1 ; i <= 4; i++){
			if(nvram_pf_get_int(prefix_wlc, "key") != i)
				continue;

			str = nvram_pf_safe_get(prefix_wlc, "wep_key");
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

void gen_hostapd_wps_config(FILE *fp_h, int band, int subnet, char *br_if)
{
	char prefix[8];
	snprintf(prefix, sizeof(prefix), "wl%d_", band);

	fprintf(fp_h, "# Wi-Fi Protected Setup (WPS)\n");

	if(!subnet && nvram_get_int("wps_enable")){
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
	fprintf(fp_h, "model_number=%s\n", nvram_get("odmpid")? : nvram_safe_get("productid"));
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

int gen_hostapd_config(int band, int subunit)
{
	int v, r;
	int sw_mode = sw_mode(), wpapsk;
	int rep_mode, macmode;
	int flag_8021x = 0;
	char *str = NULL, *nv, *nvp, *b, *hw_mode = "";
	char prefix[] = "wlXXXXXXX_";
	char tmpstr[128], psk[65];
	char wif[IFNAMSIZ];
	char hostapd_path[50], mac_path[50];
	FILE *fp_h, *fp_mac;
#if defined(RTCONFIG_MT798X)
	unsigned int maxsta = 512;
#elif defined(RTCONFIG_WLMODULE_MT7915D_AP)
	unsigned int maxsta = 256;
#elif defined(RTCONFIG_WLMODULE_MT7615E_AP)
	unsigned int maxsta = 128;
#else
	unsigned int maxsta = 64;
#endif
#ifdef RTCONFIG_WIRELESSREPEATER
	int wlc_band = nvram_get_int("wlc_band");
#endif
	char br_if[IFNAMSIZ];

	if (band < 0 || band >= MAX_NR_WL_IF || subunit < 0)
		return -1;

#if !defined(RTCONFIG_HAS_5G_2)
	if (band == 2)
		return -1;
#endif

	switch (band) {
	case WL_2G_BAND:
#if defined(RTCONFIG_RALINK_MT7621)
		hw_mode = "b";//workaround for mt7621
#else
		hw_mode = "g";
#endif
		break;
	case WL_5G_BAND:
	case WL_5G_2_BAND:
		hw_mode = "a";
		break;
//	case WL_6G_BAND:
//		hw_mode = "a";
//		break;
	default:
		dbg("%s: unknown wl%d band!\n", __func__, band);
		return -1;
	}

	rep_mode = 0;

	get_wlxy_ifname(band, subunit, wif);

	snprintf(hostapd_path, sizeof(hostapd_path), "/etc/Wireless/hostapd_%s.conf", wif);
	snprintf(mac_path, sizeof(mac_path), "/etc/Wireless/maclist_%s.conf", wif);
	_dprintf("gen ralink hostapd config : %s\n", wif);
	if (!(fp_h = fopen(hostapd_path, "w+")))
		return 0;
	if (!(fp_mac = fopen(mac_path, "w")))
		return 0;
#ifdef RTCONFIG_WIRELESSREPEATER
	if (sw_mode == SW_MODE_REPEATER && wlc_band == band && nvram_invmatch("wlc_ssid", "") && !subunit) {
		rep_mode = 1;
	}
#endif

#ifdef RTCONFIG_WIRELESSREPEATER
	if (rep_mode == 1
#if defined(RTCONFIG_WISP)
		|| wisp_mode()
#endif
	) {
		char prefix_wlc[] = "wlXXXXXXX_";
		snprintf(prefix, sizeof(prefix), "wl%d_", band);
		snprintf(prefix_wlc, sizeof(prefix_wlc), "wlc_");
		nvram_set("ure_disable", "0");
		write_rpt_wpa_supplicant_conf(band, prefix, prefix_wlc, NULL);
	}
#endif /* RTCONFIG_WIRELESSREPEATER */
	if(subunit)
		r = snprintf(prefix, sizeof(prefix), "wl%d.%d_", band, subunit);
	else
		r= snprintf(prefix, sizeof(prefix), "wl%d_", band);
	if(unlikely(r < 0))
		dbg("snprintf failed\n");
	fprintf(fp_h, "interface=%s\n", wif);
	fprintf(fp_h, "ctrl_interface=/var/run/hostapd\n");
	fprintf(fp_h, "driver=nl80211\n");
	strlcpy(br_if, nvram_get("lan_ifname")? : nvram_default_get("lan_ifname"), sizeof(br_if));
	fprintf(fp_h, "bridge=%s\n", br_if);
//	fprintf(fp_h, "logger_syslog=-1\n");
//	fprintf(fp_h, "logger_syslog_level=2\n");
//	fprintf(fp_h, "logger_stdout=-1\n");
//	fprintf(fp_h, "logger_stdout_level=2\n");
	fprintf(fp_h, "ctrl_interface_group=0\n");
	fprintf(fp_h, "max_num_sta=%d\n", maxsta);
	fprintf(fp_h, "ignore_broadcast_ssid=0\n");
	fprintf(fp_h, "#eapol_key_index_workaround=0\n");
	fprintf(fp_h, "nas_identifier=ap.mtk.com\n");
	flag_8021x = 0;

	if (nvram_pf_match(prefix, "auth_mode_x", "radius") || nvram_pf_match(prefix, "auth_mode_x", "wpa") || nvram_pf_match(prefix, "auth_mode_x", "wpa2") 
		|| nvram_pf_match(prefix, "auth_mode_x", "wpawpa2"))
	{
		flag_8021x = 1;
		fprintf(fp_h, "ieee8021x=1\n");
		fprintf(fp_h, "eap_server=0\n");
	} else {
		fprintf(fp_h, "ieee8021x=0\n");
		fprintf(fp_h, "eap_server=1\n");
	}

	fprintf(fp_h, "ssid=%s\n", nvram_pf_safe_get(prefix, "ssid"));

	/* Network Mode */
	fprintf(fp_h, "hw_mode=%s\n", hw_mode);
	//AutoChannelSelect, workaround for hostapd to work
	if(band == 1)
		fprintf(fp_h, "channel=%d\n", 36);
	else
		fprintf(fp_h, "channel=%d\n", 1);
	/* AuthMode */
	memset(tmpstr, 0x0, sizeof(tmpstr));
	str = nvram_pf_get(prefix, "auth_mode_x");
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
//		fprintf(fp_h, "#wpa_pairwise=\n");
		fprintf(fp_h, "#rsn_pairwise=\n");
	} else if ((nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_invmatch(prefix, "wep_x", "0")) ||
		    nvram_pf_match(prefix, "auth_mode_x", "shared") ||
		    nvram_pf_match(prefix, "auth_mode_x", "radius"))
	{
		//wep
//		fprintf(fp_h, "#wpa_pairwise=\n");
		fprintf(fp_h, "#rsn_pairwise=\n");
	} else if (nvram_pf_match(prefix, "auth_mode_x", "sae")) {
		fprintf(fp_h, "wpa_key_mgmt=%s\n", "SAE");
		fprintf(fp_h, "rsn_pairwise=%s\n", "CCMP");
		fprintf(fp_h, "wpa_ptk_rekey=0\n");
		fprintf(fp_h, "wpa_pairwise_update_count=4\n");
		fprintf(fp_h, "eapol_version=2\n");
		fprintf(fp_h, "ieee80211w=2\n");
	} else if (nvram_pf_match(prefix, "auth_mode_x", "psk2sae")) {
		fprintf(fp_h, "wpa_key_mgmt=%s\n", "WPA-PSK SAE");
		fprintf(fp_h, "rsn_pairwise=%s\n", "CCMP");
		fprintf(fp_h, "wpa_ptk_rekey=0\n");
		fprintf(fp_h, "wpa_pairwise_update_count=4\n");
		fprintf(fp_h, "eapol_version=2\n");
		fprintf(fp_h, "ieee80211w=2\n");
	} else if (nvram_pf_match(prefix, "crypto", "tkip")) {
		fprintf(fp_h, "wpa_key_mgmt=WPA-%s\n", flag_8021x? "EAP" : "PSK");
		fprintf(fp_h, "wpa_ptk_rekey=0\n");
		fprintf(fp_h, "wpa_pairwise_update_count=4\n");
		fprintf(fp_h, "eapol_version=2\n");
//		fprintf(fp_h, "wpa_pairwise=TKIP\n");
		fprintf(fp_h, "rsn_pairwise=%s\n", "TKIP");
	} else if (nvram_pf_match(prefix, "crypto", "aes")) {
		fprintf(fp_h, "wpa_key_mgmt=WPA-%s\n", flag_8021x? "EAP" : "PSK");
		fprintf(fp_h, "wpa_ptk_rekey=0\n");
		fprintf(fp_h, "wpa_pairwise_update_count=4\n");
		fprintf(fp_h, "eapol_version=2\n");
//		fprintf(fp_h, "wpa_pairwise=CCMP\n");
		fprintf(fp_h, "rsn_pairwise=%s\n", "CCMP");
	} else if (nvram_pf_match(prefix, "crypto", "tkip+aes")) {
		fprintf(fp_h, "wpa_key_mgmt=WPA-%s\n", flag_8021x? "EAP" : "PSK");
		fprintf(fp_h, "wpa_ptk_rekey=0\n");
		fprintf(fp_h, "wpa_pairwise_update_count=4\n");
		fprintf(fp_h, "eapol_version=2\n");
//		fprintf(fp_h, "wpa_pairwise=TKIP CCMP\n");
		fprintf(fp_h, "rsn_pairwise=%s\n", "TKIP CCMP");
	} else {
//		fprintf(fp_h, "#wpa_pairwise=\n");
		fprintf(fp_h, "#rsn_pairwise=\n");
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
	} else if (nvram_pf_match(prefix, "auth_mode_x", "sae") || nvram_pf_match(prefix, "auth_mode_x", "psk2sae")){
		wpapsk = 4;
		fprintf(fp_h, "wpa=2\n");
		fprintf(fp_h, "sae_password=%s\n", nvram_pf_safe_get(prefix, "wpa_psk"));
	} else if (nvram_pf_match(prefix, "auth_mode_x", "psk2") || nvram_pf_match(prefix, "auth_mode_x", "wpa2")) {
		wpapsk = 2;
		fprintf(fp_h, "wpa=2\n");
	} else if (nvram_pf_match(prefix, "auth_mode_x", "pskpsk2") ||
		   nvram_pf_match(prefix, "auth_mode_x", "wpawpa2")) {
		wpapsk = 3;
		fprintf(fp_h, "wpa=3\n");
	} else {
		fprintf(fp_h, "wpa=0\n");
	}

	if (wpapsk != 0 && !flag_8021x) {
		strlcpy(psk, nvram_pf_safe_get(prefix, "wpa_psk"), sizeof(psk));
		if (strlen(psk) == 64)
			fprintf(fp_h, "wpa_psk=%s\n", psk);
		else
			fprintf(fp_h, "wpa_passphrase=%s\n", psk);
	}

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

	if (flag_8021x) {
		fprintf(fp_h, "own_ip_addr=%s\n", nvram_get("lan_ipaddr") ? : nvram_default_get("lan_ipaddr"));
		/* radius server */
		fprintf(fp_h, "auth_server_addr=%s\n", nvram_pf_get(prefix, "radius_ipaddr") ? : "169.254.1.1");

		/* radius port */
		v = 1812;
		str = nvram_pf_safe_get(prefix, "radius_port");
		if (str && *str != '\0') {
			v = safe_atoi(str);
		}
		fprintf(fp_h, "auth_server_port=%d\n", v);

		/* radius key */
		fprintf(fp_h, "auth_server_shared_secret=%s\n", nvram_pf_safe_get(prefix, "radius_key"));
	}
	gen_hostapd_wps_config(fp_h, band, subunit, br_if);

next:
	fclose(fp_h);
	fclose(fp_mac);
	return 0;
}

void ralink_hostapd_start(void)
{
	FILE *fp;
	pid_t pid;
	int unit = 0, subunit = 0, r; 
	char prefix[] = "wlXXXXXXX_";
	char wif[IFNAMSIZ];
	char hostapd_path[24] = {0}, conf_path[50], pid_path[32], log_path[32], entropy_path[32];
	char *argv[]={"/sbin/delay_exec","4","/tmp/postwifi.sh",NULL};
	ralink_hostapd_stop();
	if (!(fp = fopen("/tmp/postwifi.sh", "w+")))
		return;
	fprintf(fp, "#!/bin/sh\n");
#if defined(RTCONFIG_SINGLE_HOSTAPD)
	eval("hostapd", "-g", "/var/run/hostapd/global", "-B", "-P", "/var/run/hostapd_global.pid");
#endif
	for(unit = 0; unit < MAX_NR_WL_IF; unit++){
		for(subunit = 0; subunit < MAX_NO_MSSID; subunit++){
			if(subunit)
				snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);
			else
				snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			if(nvram_pf_match(prefix, "bss_enabled", "1")){
				get_wlxy_ifname(unit, subunit, wif);
#if defined(RTCONFIG_SINGLE_HOSTAPD)
				fprintf(fp, "wpa_cli -g /var/run/hostapd/global raw ADD bss_config=%s:/etc/Wireless/hostapd_%s.conf", wif, wif);
#else
				r = snprintf(hostapd_path, sizeof(hostapd_path), "/tmp/hostapd_%s", wif);
				if(unlikely(r < 0))
					dbg("snprintf failed\n");
				doSystem("ln -sf /usr/sbin/hostapd %s", hostapd_path);
				snprintf(conf_path, sizeof(conf_path), "/etc/Wireless/hostapd_%s.conf", wif);
				r = snprintf(pid_path, sizeof(pid_path), "/var/run/hostapd_%s.pid", wif);
				if(unlikely(r < 0))
					dbg("snprintf failed\n");
				r = snprintf(log_path, sizeof(log_path), "/tmp/hostapd_%s.log", wif);
				if(unlikely(r < 0))
					dbg("snprintf failed\n");
				r = snprintf(entropy_path, sizeof(entropy_path), "/var/run/entropy_%s.bin", wif);
				if(unlikely(r < 0))
					dbg("snprintf failed\n");
				doSystem("echo \"%s -B -P %s -f %s -e %s %s\" >> /tmp/hostapd.log", hostapd_path, pid_path, log_path, entropy_path, conf_path);
				fprintf(fp, "while [ -z $(pidof hostapd_%s) ]\n", wif);
				fprintf(fp, "do\n");
				//fprintf(fp, "%s -B -P %s -f %s -e %s -dddd %s\n", hostapd_path, pid_path, log_path, entropy_path, conf_path);
				fprintf(fp, "%s -B -P %s -f %s -e %s %s\n", hostapd_path, pid_path, log_path, entropy_path, conf_path);
				fprintf(fp, "sleep 1s\n");
				fprintf(fp, "done\n");
#endif
			}
		}
	}
#if defined(RTCONFIG_WIRELESSREPEATER)
	if(sw_mode() == SW_MODE_REPEATER
#if defined(RTCONFIG_SWRTMESH)
		|| (sw_mode() == SW_MODE_AP && nvram_match("re_mode", "1"))
#endif
#if defined(RTCONFIG_WISP)
		|| wisp_mode()
#endif
	){
		int r;
		int wlc_band = nvram_get_int("wlc_band");
		snprintf(wif, sizeof(wif), "%s", get_staifname(wlc_band));
		ifconfig(wif, IFUP, NULL, NULL);//up ifname
#if defined(RTCONFIG_WISP)
		if(!wisp_mode())
#endif
		eval("brctl", "addif", "br0", wif);//add to bridge
		r = snprintf(pid_path, sizeof(pid_path), "/var/run/wifi-%s.pid", wif);
		if(unlikely(r < 0))
			dbg("snprintf failed\n");
		snprintf(conf_path, sizeof(conf_path), "/etc/Wireless/wpa_supplicant-%s.conf", wif);
		r = snprintf(log_path, sizeof(log_path), "/tmp/hostapd_%s.log", wif);
		if(unlikely(r < 0))
			dbg("snprintf failed\n");
		fprintf(fp, "while [ -z $(pidof wpa_supplicant) ]\n");
		fprintf(fp, "do\n");
		fprintf(fp, "wpa_supplicant -B -P %s -D nl80211 -i %s -b br0 -f %s -c %s\n", pid_path, wif, log_path, conf_path);
		fprintf(fp, "sleep 1s\n");
		fprintf(fp, "done\n");
	}
#endif
	fclose(fp);
	chmod("/tmp/postwifi.sh",0777);
	_eval(argv, NULL, 0, &pid);
}

void gen_ralink_wifi_cfgs(void)
{
	int unit = 0, subunit = 0; 
	char prefix[] = "wlXXXXXXX_";
	char wif[IFNAMSIZ];
	for(unit = 0; unit < MAX_NR_WL_IF; unit++){
		for(subunit = 0; subunit < MAX_NO_MSSID; subunit++){
			if(subunit)
				snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);
			else
				snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			if(nvram_pf_match(prefix, "bss_enabled", "1")){
				get_wlxy_ifname(unit, subunit, wif);
				ifconfig(wif, 0, NULL, NULL);
				gen_hostapd_config(unit, subunit);
			}
		}
	}
#if defined(RTCONFIG_SWRTMESH)
//	auto_generate_config();
//	swrtmesh_sysdep_bh_start();
#endif
}

void ralink_hostapd_stop(void)
{
	int unit = 0, subunit = 0, r; 
	char prefix[] = "wlXXXXXXX_";
	char wif[IFNAMSIZ];
#if defined(RTCONFIG_SINGLE_HOSTAPD)
	char cmd[128];
#else
	char pid_path[32];
#endif
	for(unit = 0; unit < MAX_NR_WL_IF; unit++){
		for(subunit = 0; subunit < MAX_NO_MSSID; subunit++){
			if(subunit)
				snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);
			else
				snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			get_wlxy_ifname(unit, subunit, wif);
#if defined(RTCONFIG_SINGLE_HOSTAPD)
			snprintf(cmd, sizeof(cmd), "wpa_cli -g /var/run/hostapd/global raw REMOVE %s", wif);
			doSystem(cmd);
#else
			r = snprintf(pid_path, sizeof(pid_path), "/var/run/hostapd_%s.pid", wif);
			if(unlikely(r < 0))
				dbg("snprintf failed\n");
			if(f_exists(pid_path)){
				kill_pidfile_tk(pid_path);
				unlink(pid_path);
//				ifconfig(wif, 0, NULL, NULL);
			}
			r = snprintf(pid_path, sizeof(pid_path), "/var/run/hostapd/%s", wif);
			if(unlikely(r < 0))
				dbg("snprintf failed\n");
			if(f_exists(pid_path))
				unlink(pid_path);
#endif
		}
	}
#if defined(RTCONFIG_SINGLE_HOSTAPD)
	kill_pidfile_tk("/var/run/hostapd_global.pid");
#endif
#if defined(RTCONFIG_SWRTMESH)
//	swrtmesh_sysdep_bh_stop();
#endif
}

void stop_wifi_wpa_supplicant(void)
{
	int unit = 0, r; 
	char wif[IFNAMSIZ], pid_path[32];
	for(unit = 0; unit < MAX_NR_WL_IF; unit++){
		snprintf(wif, sizeof(wif), "%s", get_staifname(unit));
		r = snprintf(pid_path, sizeof(pid_path), "/var/run/wifi-%s.pid", wif);
		if(unlikely(r < 0))
			dbg("snprintf failed\n");
		if(f_exists(pid_path)){
			kill_pidfile_tk(pid_path);
			unlink(pid_path);
			ifconfig(wif, 0, NULL, NULL);
		}
	}
}

void gen_ra_config(const char *ifname)
{
	char word[256], *next;

	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		SKIP_ABSENT_FAKE_IFACE(word);
		if (!strcmp(word, ifname))
		{
			if (!strcmp(word, nvram_safe_get("wl0_ifname"))) // 2.4G
			{
				gen_ralink_config(0, 0);
#if defined(RALINK_DBDC_MODE) || defined(RTCONFIG_MT798X) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
				gen_ralink_config(1, 1);
#endif
			}
			else if (!strcmp(word, nvram_safe_get("wl1_ifname"))) // 5G
			{
				gen_ralink_config(1, 1);
			}
		}
	}
#if defined(RTCONFIG_SWRTMESH)
	gen_ralink_wifi_cfgs();
#endif
}

int radio_ra(const char *wif, int band, int ctrl)
{
	char tmp[100], prefix[]="wlXXXXXXX_";

	snprintf(prefix, sizeof(prefix), "wl%d_", band);

	if (wif == NULL) return -1;

	if (!ctrl)
		//doSystem("iwpriv %s set RadioOn=0", wif);
		ifconfig(wif, 0, NULL, NULL);
	else
	{
		if(nvram_match(strcat_r(prefix, "radio", tmp), "1"))
			//doSystem("iwpriv %s set RadioOn=1", wif);
			ifconfig(wif, 1, NULL, NULL);
	}

	return 0;
}

void set_wlpara_ra(const char* wif, int band)
{
	char tmp[100], prefix[]="wlXXXXXXX_";

	snprintf(prefix, sizeof(prefix), "wl%d_", band);

	if (nvram_match(strcat_r(prefix, "radio", tmp), "0"))
		radio_ra(wif, band, 0);
	else
	{
		int txpower = nvram_get_int(strcat_r(prefix, "txpower", tmp));
		if ((txpower >= 0) && (txpower <= 100))
			doSystem("iwpriv %s set TxPower=%d", wif, txpower);
	}
#if 0
	if (nvram_match(strcat_r(prefix, "bw", tmp), "2"))
	{
		int channel = ra_get_channel(band);

		if (channel)
			eval("iwpriv", (char *)wif, "set", "HtBw=1");
	}
#endif
#if defined(RTCONFIG_WLMODULE_MT7915D_AP)
#if defined(RTAX53U)
	if(nvram_match(strcat_r(prefix, "radio", tmp), "1"))
		eval("iwpriv", (char *)wif, "set", "led_setting=00-00-01-00-02-00-00-02");
	else
		eval("iwpriv", (char *)wif, "set", "led_setting=00-00-01-00-02-00-00-00");
#endif
#endif
	eval("iwpriv", (char *)wif, "set", "IgmpAdd=01:00:5e:7f:ff:fa");
	eval("iwpriv", (char *)wif, "set", "IgmpAdd=01:00:5e:00:00:09");
	eval("iwpriv", (char *)wif, "set", "IgmpAdd=01:00:5e:00:00:fb");
}

void set_wlpara_ra_down(const char* wif, int band)
{

}

//fix me
int wlconf_ra_down(const char* wif)
{
	int unit = 0;
	char word[256], *next;
	char prefix[] = "wlXXXXXXXXXX_";

	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		SKIP_ABSENT_BAND_AND_INC_UNIT(unit);
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
#if 1
		if (!strcmp(word, wif))
		{
			if (!strcmp(word, WIF_2G))
				set_wlpara_ra_down(wif, 0);
#if defined(RTCONFIG_HAS_5G)
			else if (!strcmp(word, WIF_5G))
				set_wlpara_ra_down(wif, 1);
#endif
		}
#endif
		unit++;
	}
	return 0;
}

int wlconf_ra(const char* wif)
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

			if (!strcmp(word, WIF_2G))
				set_wlpara_ra(wif, 0);
#if defined(RTCONFIG_HAS_5G)
			else if (!strcmp(word, WIF_5G))
				set_wlpara_ra(wif, 1);
#endif
		}

		unit++;
	}
	return 0;
}

void apcli_start(void)
{
//repeater mode :sitesurvey channel and apclienable=1
	int ch;
	char *aif;

	if(sw_mode() == SW_MODE_REPEATER
#if defined(RTCONFIG_WISP) && (defined(RTCONFIG_WLMODULE_MT7603E_AP) || defined(RTCONFIG_WLMODULE_MT7615E_AP))
		|| wisp_mode()
#endif
		){
		int wlc_band = nvram_get_int("wlc_band");
#if defined(RTCONFIG_WISP) && (defined(RTCONFIG_WLMODULE_MT7603E_AP) || defined(RTCONFIG_WLMODULE_MT7615E_AP))
		if(wisp_mode()){
			doSystem("echo %d > /sys/kernel/debug/hnat/hook_toggle", 0);//disable hnat for old hw
			modprobe_r("mtkhnat");
		}
#endif
		aif = get_staifname(wlc_band);
#if defined(RTCONFIG_MT798X)
		ifconfig(aif, IFUP, NULL, NULL);//up ifname
		eval("brctl", "addif", "br0", aif);//add to bridge
#endif
		ch = site_survey_for_channel(wlc_band, aif);
		if(ch != -1)
		{
#if defined(RTCONFIG_WLMODULE_MT7603E_AP) || defined(RTCONFIG_WLMODULE_MT7610_AP)
			doSystem("iwpriv %s set Channel=%d", aif, ch);
#endif
			doSystem("iwpriv %s set ApCliEnable=1", aif);
#if defined(RTCONFIG_WLMODULE_MT7603E_AP) || defined(RTCONFIG_WLMODULE_MT7610_AP)
			doSystem("iwpriv %s set ApCliAutoConnect=1", aif);
#else
			doSystem("iwpriv %s set ApCliAutoConnect=3", aif);
#endif
			if(wlc_band)
				doSystem("iwpriv %s set DfsCacClean=1", aif);
			fprintf(stderr, "##set channel=%d, enable apcli ..#\n", ch);
		}
		else
			fprintf(stderr, "## Can not find pap's ssid ##\n");
	}
}

void stop_wds_ra(const char* lan_ifname, const char* wif)
{
	char prefix[32];
	char wdsif[32];
	int i, r;

	if (strcmp(wif, WIF_2G) && strcmp(wif, WIF_5G))
		return;

#if defined(RTCONFIG_MT798X)
	if (!strncmp(wif, "rax", 3))
		snprintf(prefix, sizeof(prefix), "wdsx");
#else
	if (!strncmp(wif, "rai", 3))
		snprintf(prefix, sizeof(prefix), "wdsi");
#endif
	else
		snprintf(prefix, sizeof(prefix), "wds");

	for (i = 0; i < 4; i++)
	{
		r = snprintf(wdsif, sizeof(wdsif), "%s%d", prefix, i);
		if(unlikely(r < 0))
			dbg("snprintf failed\n");
		doSystem("brctl delif %s %s 1>/dev/null 2>&1", lan_ifname, wdsif);
		ifconfig(wdsif, 0, NULL, NULL);
	}
}

#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
void start_wds_ra()
{
	char* lan_ifname = nvram_safe_get("lan_ifname");
	char prefix[32];
	char wdsif[32];
	int i, j, ret;

	for(i = 0; i < 2; i++) {
		memset(prefix, 0, sizeof(prefix));
		snprintf(prefix, sizeof(prefix), "wl%d_mode_x", i);
		ret = nvram_get_int(prefix);
		if((ret !=1) && (ret != 2))
			continue;

		memset(prefix, 0, sizeof(prefix));
		if(i == 0)
			snprintf(prefix, sizeof(prefix), "wds");
		else if( i == 1)
			snprintf(prefix, sizeof(prefix), "wdsi");

		for (j = 0; j < 4; j++)
		{
			snprintf(wdsif, sizeof(wdsif), "%s%d", prefix, j);
			ifconfig(wdsif, IFUP, NULL, NULL);
			doSystem("brctl addif %s %s 1>/dev/null 2>&1", lan_ifname, wdsif);
		}
	}
}
#endif

void Get_fail_log(char *buf, int size, unsigned int offset)
{
	struct FAIL_LOG fail_log, *log = &fail_log;
	char *p = buf;
	int x, y;

	memset(buf, 0, size);
	FRead((char*) &fail_log, offset, sizeof(fail_log));
	if(log->num == 0 || log->num > FAIL_LOG_MAX)
	{
		return;
	}
	for(x = 0; x < (FAIL_LOG_MAX >> 3); x++)
	{
		for(y = 0; log->bits[x] != 0 && y < 7; y++)
		{
			if(log->bits[x] & (1 << y))
			{
				p += snprintf(p, size - (p - buf), "%d,", (x << 3) + y);
			}
		}
	}
}

void Gen_fail_log(const char *logStr, int max, struct FAIL_LOG *log)
{
	const char *p = logStr;
	char *next;
	int num;
	int x,y;

	memset(log, 0, sizeof(struct FAIL_LOG));
	if(max > FAIL_LOG_MAX)
		log->num = FAIL_LOG_MAX;
	else
		log->num = max;

	if(logStr == NULL)
		return;

	while(*p != '\0')
	{
		while(*p != '\0' && !isdigit(*p))
			p++;
		if(*p == '\0')
			break;
		num = strtoul(p, &next, 0);
		if(num > FAIL_LOG_MAX)
			break;
		x = num >> 3;
		y = num & 0x7;
		log->bits[x] |= (1 << y);
		p = next;
	}
}
#endif //RTCONFIG_RALINK

#ifdef RTCONFIG_WIRELESSREPEATER
int get_apcli_status(int wlc_band)
{
#if defined(RTCONFIG_SWRTMESH)
	int unit = nvram_get_int("wlc_band");
	char buf[8192], sta[64];
	FILE *fp;
	int len;
	char *pt1 = "95", *pt2;

	if(!nvram_get_int("wlready")) return 0;

	strlcpy(sta, get_staifname(unit), sizeof(sta));
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
						    return WLC_STATE_INITIALIZING;
						 else
						    return WLC_STATE_STOPPED;
					}	
				     	else	
				   		return WLC_STATE_INITIALIZING;
				}	
				else
				   	return WLC_STATE_CONNECTED;
			}
		}
	}
	
	return WLC_STATE_STOPPED;
#else
	const char *ifname;
	char data[32];
	struct iwreq wrq;
	int status;
	static int old_status[2] = {-1, -1};
	if (wlc_band == 1)
		ifname = APCLI_5G;
	else
		ifname = APCLI_2G;

	memset(data, 0, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = ASUS_SUBCMD_CONN_STATUS;

	if (wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0)
	{
		dbg("errors in getting %s CONN_STATUS result\n", ifname);
		return -1;
	}

	status = *(int*)wrq.u.data.pointer;
	if (wlc_band >=0 && wlc_band <= 1 && old_status[wlc_band] != status)
	{
		cprintf("%s: %s connStatus(%d --> %d)\n", __func__, ifname, old_status[wlc_band], status);
		old_status[wlc_band] = status;
	}
	if (status == 6)	// APCLI_CTRL_CONNECTED
		return WLC_STATE_CONNECTED;
	else if (status == 4)	// APCLI_CTRL_ASSOC
		return WLC_STATE_CONNECTING;
	return WLC_STATE_INITIALIZING;
#endif
}

char *wlc_nvname(char *keyword)
{
	return(wl_nvname(keyword, nvram_get_int("wlc_band"), -1));
}

#ifdef RTCONFIG_CONCURRENTREPEATER

int getWscStatusCli(char *aif)
{
	int data = 0;
	struct iwreq wrq;

	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;

	if (wl_ioctl(aif, RT_PRIV_IOCTL, &wrq) < 0)
		dbg("errors in getting WSC status\n");

	return data;
}

unsigned int get_conn_link_quality(int unit)
{
	int link_quality = 0;
	const char *aif;
	char data[16];
	struct iwreq wrq;

	aif = get_staifname(band);

	memset(data, 0x00, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = ASUS_SUBCMD_CLIQ;

	if (wl_ioctl(aif, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0)
	{
		dbg("errors in getting STAINFO result\n");
		return 0;
	}

	link_quality = atoi(data);

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

/*
 * like: iwpriv [wif] set [pv_pair]
 */
int ap_set(char *wif, const char *pv_pair)
{
	struct iwreq wrq;
	char data[256];

	memset(data, 0, sizeof(data));
	strcpy(data, pv_pair);
	wrq.u.data.pointer = data;
	wrq.u.data.length = strlen(data);
	wrq.u.data.flags = 0;

	fprintf(stderr, "[%s] set %s\n", wif, pv_pair);

	if (wl_ioctl(wif, RTPRIV_IOCTL_SET, &wrq) < 0)
		return 0;
	else
		return 1;
}

int getPapState(int band)
{
	int ret;
	int ch;
	const char *aif;
	static long lastUptime[2] = {-FIND_CHANNEL_INTERVAL, -FIND_CHANNEL_INTERVAL};  // doing "site survey" takes time. use this to prolong interval between site survey.
	long Uptime;

	Uptime = uptime();
	if((ret = get_apcli_status(band))==0) //init
	{
		if ((Uptime > lastUptime[band] && Uptime < lastUptime[band] + FIND_CHANNEL_INTERVAL)
		   || (Uptime < lastUptime[band] && Uptime < FIND_CHANNEL_INTERVAL))
			return ret;

		aif = get_staifname(band);
		ch = site_survey_for_channel(band, aif);
		if(ch != -1)
		{
#if defined(RTCONFIG_WLMODULE_MT7603E_AP)
			doSystem("iwpriv %s set Channel=%d", aif, ch);
#endif
			doSystem("iwpriv %s set ApCliEnable=1", aif);

			doSystem("ifconfig %s up", aif);
			dbg("set pap's channel=%d, enable apcli ..#\n",ch);
#if defined(RTCONFIG_WLMODULE_MT7603E_AP)
			doSystem("iwpriv %s set ApCliAutoConnect=1", aif);
#else
			doSystem("iwpriv %s set ApCliAutoConnect=3", aif);
#endif
			if(band)
				doSystem("iwpriv %s set DfsCacClean=1", aif);
			lastUptime[band] = Uptime;
		}
		else
			lastUptime[band] = -FIND_CHANNEL_INTERVAL;
	}
	else
		lastUptime[band] = Uptime;

	return ret;
}

int wlcconnect_core(void)
{
	int ret = 0;
#if defined(RTCONFIG_CONCURRENTREPEATER)
	int unit = 0;
	char buf[32] = {0};
	int sw_mode = sw_mode();
	int wlc_express = nvram_get_int("wlc_express");

	if (nvram_get_int("wps_cli_state") == 1 && sw_mode == SW_MODE_REPEATER)
		return 0;

	if (!nvram_get_int("wlready") && !mediabridge_mode())
		return 0;

	if (sw_mode == SW_MODE_REPEATER) {
	 	if (wlc_express == 0) {	/* for apcli0 or apclii0  */
			unit = nvram_get_int("wlc_band");
			if (unit == 0 || unit == 1) {
				ret = getPapState(unit);	/* for apcli0 or apclii0 */
				sprintf(buf, "wlc%d_state", unit);
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
				ret = getPapState(0);	/* for apcli0 */
				nvram_set_int("wlc0_state", ret);
#ifdef RTCONFIG_HAS_5G
				if (ret != WLC_STATE_CONNECTED) {
					ret = getPapState(1);	/* for apclii0 */
					nvram_set_int("wlc1_state", ret);
				}
				else { // Update sta1 state
					nvram_set_int("wlc1_state", getPapState(1));
				}
#endif
			}
		}
		else if (wlc_express == 1) {
			ret = getPapState(0);	/* for apcli0 */
			nvram_set_int("wlc0_state", ret);
		}
#ifdef RTCONFIG_HAS_5G
		else if (wlc_express == 2) {
			ret = getPapState(1);	/* for apclii0 */
			nvram_set_int("wlc1_state", ret);
		}
#endif

	}
#else // RTCONFIG_CONCURRENTREPEATER
	ret = getPapState(nvram_get_int("wlc_band"));
#endif // RTCONFIG_CONCURRENTREPEATER
	//dbg("wlcconnect...check\n");
	return ret;
}

int wlcscan_core(char *ofile, char *wif)
{
	int ret,count;

	count=0;

#ifdef RTCONFIG_PROXYSTA
	if (mediabridge_mode())
		ifconfig(wif, IFUP, NULL, NULL);
#endif
	while((ret=getSiteSurvey(get_wifname_num(wif),ofile)==0)&& count++ < 2)
	{
		 dbg("[rc] set scan results command failed, retry %d\n", count);
		 sleep(1);
	}
#ifdef RTCONFIG_PROXYSTA
	if (mediabridge_mode())
		ifconfig(wif, 0, NULL, NULL);
#endif

	return 0;
}
#endif

#ifdef RTCONFIG_USER_LOW_RSSI
void rssi_check_unit(int unit)
{
	int xTxR;
	char header[128]={0};
	char header_t[128]={0};
	char rssinum[16]={0};
	int staCount = 0, rssi_th = 0;
	char data[2048],cmd[128],tmp[128];
	unsigned char pap_bssid[18];
	struct iwreq wrq,wrq2;
	roam_sta *ssap;
	char prefix[] = "wlXXXXXXXXXX_";
	char *wif;
	int hdrLen = 0;
	int stream = 0;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	if (!(rssi_th= nvram_get_int(strcat_r(prefix, "user_rssi", tmp))))
		return;
	//dbg("rssi_th=%d\n",rssi_th);

	if (!(xTxR = nvram_get_int(strcat_r(prefix, "HT_RxStream", tmp))))
		return;

	if(xTxR > xR_MAX)
		xTxR = xR_MAX;

	memset(data, 0x00, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = ASUS_SUBCMD_GROAM;
	wif = get_wifname(unit);
	if (wl_ioctl(wif, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0)
	{
		dbg("errors in getting STAINFO result\n");
		return;
	}
	//dbg("wif(%s) xTxR(%d) GROAM:\n%s", wif, xTxR, data);
#if 0
	memset(header, 0, sizeof(header));
	hdrLen = sprintf(header, "%-19s%-7s%-7s%-7s\n",
			"MAC", "RSSI0", "RSSI1","RSSI2");
#endif
	memset(header, 0, sizeof(header));
	memset(header_t, 0, sizeof(header_t));
	hdrLen = sprintf(header_t, "%-19s", "MAC");
	strcpy(header, header_t);

	for (stream = 0; stream < xTxR; stream++) {
		sprintf(rssinum, "RSSI%d", stream);
		hdrLen += sprintf(header_t, "%-7s", rssinum);
		strncat(header, header_t, strlen(header_t));
    }

	strcat(header,"\n");
	hdrLen++;
	//dbG("hdrLen=%d, final header####%s####\n", hdrLen, header);

	if (wrq.u.data.length > 0 && data[0] != 0)
	{
		int len = strlen(wrq.u.data.pointer + hdrLen);
		char *sp, *op;

		//dbg("%s\n",wrq.u.data.pointer);
		ssap = (roam_sta *)(wrq.u.data.pointer + hdrLen);
		op = sp = wrq.u.data.pointer + hdrLen;
		while (*sp && ((len - (sp-op)) >= 0)) {
			ssap->sta[staCount].mac[18]='\0';
			for (stream = 0; stream < xTxR; stream++) {
				ssap->sta[staCount].rssi[stream][6]='\0';
			}
			sp += hdrLen;
			staCount++;
		}

#ifdef RTCONFIG_WIRELESSREPEATER
		memset(pap_bssid,0,sizeof(pap_bssid));
		if(sw_mode() == SW_MODE_REPEATER && nvram_get_int("wlc_band") == unit)
		{
			char *aif;
			aif = nvram_get(strcat_r(prefix, "vifs", tmp));
			if(wl_ioctl(aif, SIOCGIWAP, &wrq2)>=0);
			{
				wrq2.u.ap_addr.sa_family = ARPHRD_ETHER;
				sprintf(pap_bssid,"%02X:%02X:%02X:%02X:%02X:%02X",
						(unsigned char)wrq2.u.ap_addr.sa_data[0],
						(unsigned char)wrq2.u.ap_addr.sa_data[1],
						(unsigned char)wrq2.u.ap_addr.sa_data[2],
						(unsigned char)wrq2.u.ap_addr.sa_data[3],
						(unsigned char)wrq2.u.ap_addr.sa_data[4],
						(unsigned char)wrq2.u.ap_addr.sa_data[5]);
			}
		}
#endif
		if (staCount)
		{
			char *strBand = NULL;
			int rssi = 0, lrssi = 0;
			int count = 0;
			int i = 0, k = 0;

			if(unit)
				strBand = "5G";
			else
				strBand = "2.4G";

			for(i = 0; i < staCount; i++)
			{
#ifdef RTCONFIG_WIRELESSREPEATER
				//dbg("pap bssid=#%s#\n",pap_bssid);
				if(!strncmp(pap_bssid,ssap->sta[i].mac,sizeof(pap_bssid)))
					continue; //pap bssid,skip
#endif
#if 0
				dbG("sta%d:mac=%s\n",i,ssap->sta[i].mac);

				for (stream = 0; stream < xTxR; stream++) {
					fprintf(stderr," rssi_th(%d) rssi[%d]=%s\n",rssi_th, stream, ssap->sta[i].rssi[stream]);
				}
#endif
				count = 0;
				for(k = 0; k < xTxR; k++)
				{
					rssi = atoi(ssap->sta[i].rssi[k]);
					if(rssi < rssi_th)
					{
						lrssi = rssi;
						count++;
					}
				}
				//disassociation
				if(count==xTxR)
				{
					logmessage("Roaming", "%s Disconnect Station: %s  RSSI: %d", strBand, ssap->sta[i].mac, lrssi);
					sprintf(cmd,"iwpriv %s set DisConnectSta=%s", wif, ssap->sta[i].mac);
					system(cmd);
				}
			}
		}
	}
}
#endif	/* RTCONFIG_USER_LOW_RSSI */

void set_default_psk()
{
	char *psk = nvram_safe_get("wifi_psk");
	if(!*psk)
	{
		unsigned char key[32];
		generate_wireless_key(key);
		nvram_set("wl0_auth_mode_x", "psk2");
		nvram_set("wl0_crypto", "aes");
		nvram_set("wl0_wpa_psk", key);
#if defined(RTAC1200)
		nvram_set("wl1_auth_mode_x", "psk2");
		nvram_set("wl1_crypto", "aes");
		nvram_set("wl1_wpa_psk", key);
#endif
	}
}

#if defined(RTCONFIG_LANWAN_LED) && !defined(RTCONFIG_SWRT_LED)
int update_wan_leds(int wan_unit, int link_wan_unit)
{
	if(nvram_get_int("x_Setting")) //normal use
	{
		int link_internet = nvram_get_int("link_internet");
		if(link_wan_unit && !inhibit_led_on())
			led_control(LED_WAN, LED_ON);
		else {
			if(link_internet != 2)
				led_control(LED_WAN, LED_OFF);
		}
	}
	else //first use
		led_control(LED_WAN, LED_OFF);
#if defined(RTCONFIG_FAILOVER_LED)
	update_failover_led();
#endif
	return 0;
}
#endif

#if defined(RTCONFIG_LANWAN_LED) || defined(RTCONFIG_LAN4WAN_LED)
int LanWanLedCtrl(void)
{
	if(get_lanports_status() && nvram_match("AllLED", "1"))
#if defined(CMCCA9)
		lanwan_led_check();
#else
		led_control(LED_LAN, LED_ON);
#endif
	else
#if defined(CMCCA9)
	{
    	led_control(LED_LAN1, LED_OFF);
	    led_control(LED_LAN2, LED_OFF);
	    led_control(LED_LAN3, LED_OFF);
	}
#else
		led_control(LED_LAN, LED_OFF);
#endif
	return 1;
}
#endif

void config_mssid_isolate(char *ifname, int vif)
{
	char path[64] = {0}, prefix[16] = {0};
	if(nvram_match("sw_mode", "1") && ifname){
		snprintf(path, sizeof(path), "/sys/class/net/%s/brport/%s", ifname, "isolated");
		if(f_exists(path)){
			if(vif)
				snprintf(prefix, sizeof(prefix), "%s_", wif_to_vif(ifname));
			else{
				if(absent_band(0) || strcmp(ifname, get_wififname(0))){
					if(absent_band(1) || strcmp(ifname, get_wififname(1))){
						dbg("%s: ifname [%s] vif [%d], unknown unit [%d]\n", __func__, ifname, 0, -1);
						return;
					}
				}
				snprintf(prefix, sizeof(prefix), "wl%d_", 1);
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

int get_wlc_func_enable(const char *ifname)
{
	struct iwreq wrq;
	char data[16];
	memset(data, 0x00, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = ASUS_SUBCMD_GETAPCLIENABLE;

	if(wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) >= 0)
		return atoi(data);
	dbg("errors in getting %s ASUS_SUBCMD_GETAPCLIENABLE result\n", ifname);
	return 0;
}

void wlcconnect_sig_handle(int sig)
{
	int wlc_band = nvram_get_int("wlc_band");
	if(sig == SIGCONT){
		if(pap_channel == -1 || !apcli_status)
			sleep(5);
		else{
			doSystem("iwpriv %s set Channel=%d", get_staifname(wlc_band), pap_channel);
			if(wlc_band)
				doSystem("iwpriv %s set DfsCacClean=1", get_staifname(wlc_band));
			if(!get_wlc_func_enable(get_staifname(wlc_band)))
				doSystem("iwpriv %s set ApCliEnable=1", get_staifname(wlc_band));
			sleep(20);
		}
	}else if(sig == SIGTSTP){
		pap_channel = ra_get_channel(wlc_band);
		pause();
	}else
		dbg("%s: received an invalid signal!\n", get_staifname(wlc_band));
}

#if defined(RTCONFIG_RALINK_BSD)
void gen_bsd_conf(void)
{
	FILE *fp = NULL;
	if((fp = fopen("/etc/bndstrg.conf", "w"))){
		fprintf(fp, "Default\n");
		fprintf(fp, "AssocTh=15\n");
		fprintf(fp, "DwellTime=300\n");
		fprintf(fp, "RSSICheckCount=10\n");
		fprintf(fp, "IdleTxByteCount=30\n");
		fprintf(fp, "IdleRxByteCount=30\n");
		fprintf(fp, "SteerTimeWindow=7200\n");
		fprintf(fp, "MaxSteerCount=10\n");
		fprintf(fp, "AgeTime=0\n");
		fprintf(fp, "CheckTime=30\n");
		fprintf(fp, "HoldTime=50\n");
		fprintf(fp, "RssiLow=-70\n");
		fprintf(fp, "RSSILowDownSteer=-70\n");
		fprintf(fp, "RSSIHighUpSteer=-60\n");
		fprintf(fp, "MinRssi2G=-65\n");
		fprintf(fp, "MinRssi5GL=-55\n");
		fprintf(fp, "MinRssi5GH=-55\n");
		fprintf(fp, "BtmMode=4\n");
		fprintf(fp, "CndPriority=9;13;16\n");//RSSI
//		fprintf(fp, "CndPriority=10;13;16\n");//MCS
		fprintf(fp, "NVRAMReset=0\n");
		fprintf(fp, "RSSIDisconnect=-75\n");
		fprintf(fp, "BlackListTime=10\n");
		fprintf(fp, "NVRAMTableSize=128\n");
		fprintf(fp, "EthIface=br0\n");
		fprintf(fp, "EthPort=2107\n");
		fprintf(fp, "IappSendTimer=10\n");
		fprintf(fp, "RoamingEntryTimeout=120\n");
		fclose(fp);
	}
}

void bandstr_sync_wl_settings(void)
{
	char tmp[100], tmp2[100], prefix[16] = "wlXXXXXXXXXX_", wlprefix[16] = "wlXXXXXXXXXX_";
	int i, n;
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	n = num_of_wl_if();
	snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	for(i = 1; i < n; i++){
		snprintf(wlprefix, sizeof(wlprefix), "wl%d_", i);
		nvram_set(strcat_r(wlprefix, "ssid", tmp), nvram_safe_get(strcat_r(prefix, "ssid", tmp2)));
		nvram_set(strcat_r(wlprefix, "auth_mode_x", tmp), nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp2)));
		nvram_set(strcat_r(wlprefix, "wep_x", tmp), nvram_safe_get(strcat_r(prefix, "wep_x", tmp2)));
		nvram_set(strcat_r(wlprefix, "key", tmp), nvram_safe_get(strcat_r(prefix, "key", tmp2)));
		nvram_set(strcat_r(wlprefix, "key1", tmp), nvram_safe_get(strcat_r(prefix, "key1", tmp2)));
		nvram_set(strcat_r(wlprefix, "key2", tmp), nvram_safe_get(strcat_r(prefix, "key2", tmp2)));
		nvram_set(strcat_r(wlprefix, "key3", tmp), nvram_safe_get(strcat_r(prefix, "key3", tmp2)));
		nvram_set(strcat_r(wlprefix, "key4", tmp), nvram_safe_get(strcat_r(prefix, "key4", tmp2)));
		nvram_set(strcat_r(wlprefix, "phrase_x", tmp), nvram_safe_get(strcat_r(prefix, "phrase_x", tmp2)));
		nvram_set(strcat_r(wlprefix, "crypto", tmp), nvram_safe_get(strcat_r(prefix, "crypto", tmp2)));
		nvram_set(strcat_r(wlprefix, "wpa_psk", tmp), nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp2)));
		nvram_set(strcat_r(wlprefix, "radius_ipaddr", tmp), nvram_safe_get(strcat_r(prefix, "radius_ipaddr", tmp2)));
		nvram_set(strcat_r(wlprefix, "radius_key", tmp), nvram_safe_get(strcat_r(prefix, "radius_key", tmp2)));
		nvram_set(strcat_r(wlprefix, "radius_port", tmp), nvram_safe_get(strcat_r(prefix, "radius_port", tmp2)));
		nvram_set(strcat_r(wlprefix, "closed", tmp), nvram_safe_get(strcat_r(prefix, "closed", tmp2)));
	}
	stop_bsd();
	start_bsd();
}
#endif

int get_wifi_country_code_tmp(char *ori_countrycode, char *output, int len)
{
	return -1;
}

int get_wisp_status(void)
{
	return get_apcli_status(nvram_get_int("wlc_band")) == WLC_STATE_CONNECTED;
}

void apply_config_to_driver(int band)
{
	;
}
