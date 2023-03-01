/*
 * Copyright 2021, ASUS
 * Copyright 2021-2022, SWRTdev
 * Copyright 2021-2022, paldier <paldier@hotmail.com>.
 * Copyright 2021-2022, lostlonger<lostlonger.g@gmail.com>.
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
#define MAX_FRW 64
#define MACSIZE 12

#define	DEFAULT_SSID_2G	"ASUS"
#define	DEFAULT_SSID_5G	"ASUS_5G"

char *wlc_nvname(char *keyword);
extern char *ether_cal_b(const unsigned char *e, char *a, int i);
extern char *ether_cal(const char *e, char *a, int i);


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
	else
		return -1;
}

const char *get_wifname(int band)
{
	if (band)
		return WIF_5G;
	else
		return WIF_2G;
}

const char *get_wpsifname(void)
{
	int wps_band = nvram_get_int("wps_band_x");

	if (wps_band)
		return WIF_5G;
	else
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
		case 5: return 14;
	}
	return 14;
}

int getCountryRegion5G(const char *countryCode, int *warning, int IEEE80211H)
{
#ifdef RTCONFIG_RALINK_DFS
	if (IEEE80211H)
	{
		if(	(!strcasecmp(countryCode, "GB")) )
			return 18;
		if(	(!strcasecmp(countryCode, "JP")) )
			return 23;
	}
#endif	/* RTCONFIG_RALINK_DFS */

	if ((!strcasecmp(countryCode, "AE")) ||
			(!strcasecmp(countryCode, "AL")) ||
			(!strcasecmp(countryCode, "AR")) ||
			(!strcasecmp(countryCode, "AU")) ||
			(!strcasecmp(countryCode, "BH")) ||
			(!strcasecmp(countryCode, "BY")) ||
			(!strcasecmp(countryCode, "CA")) ||
			(!strcasecmp(countryCode, "CL")) ||
			(!strcasecmp(countryCode, "CO")) ||
			(!strcasecmp(countryCode, "CR")) ||
			(!strcasecmp(countryCode, "DO")) ||
			(!strcasecmp(countryCode, "DZ")) ||
			(!strcasecmp(countryCode, "EC")) ||
			(!strcasecmp(countryCode, "GT")) ||
			(!strcasecmp(countryCode, "HK")) ||
			(!strcasecmp(countryCode, "HN")) ||
			(!strcasecmp(countryCode, "IL")) ||
			(!strcasecmp(countryCode, "IN")) ||
			(!strcasecmp(countryCode, "JO")) ||
			(!strcasecmp(countryCode, "KW")) ||
			(!strcasecmp(countryCode, "KZ")) ||
			(!strcasecmp(countryCode, "LB")) ||
			(!strcasecmp(countryCode, "MA")) ||
			(!strcasecmp(countryCode, "MK")) ||
			(!strcasecmp(countryCode, "MO")) ||
			(!strcasecmp(countryCode, "MX")) ||
			(!strcasecmp(countryCode, "MY")) ||
			(!strcasecmp(countryCode, "NO")) ||
			(!strcasecmp(countryCode, "NZ")) ||
			(!strcasecmp(countryCode, "OM")) ||
			(!strcasecmp(countryCode, "PA")) ||
			(!strcasecmp(countryCode, "PK")) ||
			(!strcasecmp(countryCode, "PR")) ||
			(!strcasecmp(countryCode, "QA")) ||
			(!strcasecmp(countryCode, "RO")) ||
			(!strcasecmp(countryCode, "RU")) ||
			(!strcasecmp(countryCode, "SA")) ||
			(!strcasecmp(countryCode, "SG")) ||
			(!strcasecmp(countryCode, "SV")) ||
			(!strcasecmp(countryCode, "SY")) ||
			(!strcasecmp(countryCode, "TH")) ||
			(!strcasecmp(countryCode, "UA")) ||
			(!strcasecmp(countryCode, "US")) ||
			(!strcasecmp(countryCode, "UY")) ||
			(!strcasecmp(countryCode, "VN")) ||
			(!strcasecmp(countryCode, "YE")) ||
			(!strcasecmp(countryCode, "ZW")) ||
			(!strcasecmp(countryCode, "AA")) ||
			(!strcasecmp(countryCode, "CN")) ||//unlock bw160
			0
	)
	{
		if (nvram_contains_word("rc_support", "loclist") && !strcasecmp(countryCode, "AU"))
			return 9;
		return 0;
	}
	else if (
			(!strcasecmp(countryCode, "AM")) ||
			(!strcasecmp(countryCode, "AT")) ||
			(!strcasecmp(countryCode, "AZ")) ||
			(!strcasecmp(countryCode, "BE")) ||
			(!strcasecmp(countryCode, "BG")) ||
			(!strcasecmp(countryCode, "BR")) ||
			(!strcasecmp(countryCode, "CH")) ||
			(!strcasecmp(countryCode, "CY")) ||
			(!strcasecmp(countryCode, "CZ")) ||
			(!strcasecmp(countryCode, "DE")) ||
			(!strcasecmp(countryCode, "DK")) ||
			(!strcasecmp(countryCode, "EE")) ||
			(!strcasecmp(countryCode, "EG")) ||
			(!strcasecmp(countryCode, "ES")) ||
			(!strcasecmp(countryCode, "FI")) ||
			(!strcasecmp(countryCode, "FR")) ||
			(!strcasecmp(countryCode, "GB")) ||
			(!strcasecmp(countryCode, "GE")) ||
			(!strcasecmp(countryCode, "GR")) ||
			(!strcasecmp(countryCode, "HR")) ||
			(!strcasecmp(countryCode, "HU")) ||
			(!strcasecmp(countryCode, "IE")) ||
			(!strcasecmp(countryCode, "IS")) ||
			(!strcasecmp(countryCode, "IT")) ||
			(!strcasecmp(countryCode, "JP")) ||
			(!strcasecmp(countryCode, "KP")) ||
			(!strcasecmp(countryCode, "KR")) ||
			(!strcasecmp(countryCode, "LI")) ||
			(!strcasecmp(countryCode, "LT")) ||
			(!strcasecmp(countryCode, "LU")) ||
			(!strcasecmp(countryCode, "LV")) ||
			(!strcasecmp(countryCode, "MC")) ||
			(!strcasecmp(countryCode, "NL")) ||
			(!strcasecmp(countryCode, "NO")) ||
			(!strcasecmp(countryCode, "PL")) ||
			(!strcasecmp(countryCode, "PT")) ||
			(!strcasecmp(countryCode, "RO")) ||
			(!strcasecmp(countryCode, "SE")) ||
			(!strcasecmp(countryCode, "SI")) ||
			(!strcasecmp(countryCode, "SK")) ||
			(!strcasecmp(countryCode, "TN")) ||
			(!strcasecmp(countryCode, "TR")) ||
			(!strcasecmp(countryCode, "TT")) ||
			(!strcasecmp(countryCode, "UZ")) ||
			(!strcasecmp(countryCode, "ZA")) ||
			0
	)
	{
		return 1;
	}
	else if ((!strcasecmp(countryCode, "IN")) || (!strcasecmp(countryCode, "MX")))
	{
		return 2;
	}
	else if ((!strcasecmp(countryCode, "TW")))
	{
		return 3;
	}
	else if (
			(!strcasecmp(countryCode, "BR")) ||
			(!strcasecmp(countryCode, "BZ")) ||
			(!strcasecmp(countryCode, "BO")) ||
			(!strcasecmp(countryCode, "BN")) ||
//			(!strcasecmp(countryCode, "CN")) ||
			(!strcasecmp(countryCode, "ID")) ||
			(!strcasecmp(countryCode, "IR")) ||
			(!strcasecmp(countryCode, "MO")) ||
			(!strcasecmp(countryCode, "PE")) ||
			(!strcasecmp(countryCode, "PH")) ||
			(!strcasecmp(countryCode, "VE"))
	)
	{
		return 4;
	}
	else if (!strcasecmp(countryCode, "RU"))
	{
		return 6;
	}
	else if (!strcasecmp(countryCode, "DB"))
	{
		return 7;
	}
	else if ((!strcasecmp(countryCode, "UA")))
	{
		return 9;
	}
	else
	{
		if (warning)
			*warning = 2;
		return 7;
	}
}

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
			phy = 2; mcs = 2;	/* 2G: OFDM 12Mbps */
		} else {
			phy = 3; mcs = 1;	/* 5G: HTMIX 13/30Mbps */
		}
		/* fall through */
	case IPV6_DISABLED:
		break;
	}
#endif

	if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "2") ||		/* legacy mode */
	    strstr(nvram_safe_get(strcat_r(prefix, "crypto", tmp)), "tkip"))	/* tkip */
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

int gen_ralink_config(int band, int is_iNIC)
{
	FILE *fp;
	char *str = NULL;
	char *str2 = NULL;
	int  i;
	int ssid_num = 1;
	char wmm_enable[8];
	char wmm_noack[8];
	char list[2048];
	int flag_8021x = 0;
	int wsc_configure = 0;
	int warning = 0;
	int ChannelNumMax_2G = 11;
	char tmp[128], prefix[] = "wlXXXXXXX_";
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
	int IEEE80211H = 0;
#if defined(RTCONFIG_MUMIMO_2G) || defined(RTCONFIG_MUMIMO_5G)
	int mumimo = 0;
#endif
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	unsigned char *dst;
	char macaddr[]="00:11:22:33:44:55";
	char macaddr2[]="00:11:22:33:44:58";
#endif

#if defined(RTCONFIG_CONCURRENTREPEATER)
	char prefix_wlc[] = "wlcXXXXXX_";
	char tmp1[128];
	int wlc_express = nvram_get_int("wlc_express");
#endif
	char *tcode = nvram_safe_get("territory_code");

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
	snprintf(prefix, sizeof(prefix), "wl%d_", band);

	//CountryRegion
	str = nvram_safe_get(strcat_r(prefix, "country_code", tmp));
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
	if(band && nvram_match(strcat_r(prefix, "IEEE80211H", tmp), "1"))
		IEEE80211H = 1;
#endif	/* RTCONFIG_RALINK_DFS */

	//CountryRegion for A band
	str = nvram_safe_get(strcat_r(prefix, "country_code", tmp));
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
	str = nvram_safe_get(strcat_r(prefix, "country_code", tmp));
#ifdef CE_ADAPTIVITY
	if (nvram_match("reg_spec", "CE"))
	{
		fprintf(fp, "CountryCode=FR\n");
#ifndef RTCONFIG_RALINK_EDCCA
		fprintf(fp, "ED_MODE=1\n");
		fprintf(fp, "EDCCA_AP_STA_TH=255\n");
		fprintf(fp, "EDCCA_AP_AP_TH=255\n");
		fprintf(fp, "EDCCA_FALSE_CCA_TH=3000\n");
		fprintf(fp, "TxBurst=0\n");
		fprintf(fp, "HT_RDG=0\n");
		fprintf(fp, "EDCCA_ED_TH=90\n");
		fprintf(fp, "EDCCA_BLOCK_CHECK_TH=2\n");
		fprintf(fp, "EDCCA_AP_RSSI_TH=-80\n");
#endif
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
			fprintf(fp, "CountryCode=%s\n", str);
	}
	else
	{
		warning = 4;
		fprintf(fp, "CountryCode=DB\n");
	}
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	dst = tmp;
	if(band)
	{
		if (FRead(dst, OFFSET_MAC_ADDR, 6) < 0)
		{
			_dprintf("READ MAC address: Out of scope\n");
		}
		else
		{
			ether_etoa(tmp, macaddr);
			if(!strcmp(macaddr, "FF:FF:FF:FF:FF:FF"))
			{
				if (FRead(dst, OFFSET_MAC_ADDR_2G, 6) < 0)
					strlcpy(macaddr, "00:0C:43:26:60:2C", 17);
				else
					ether_cal_b(dst, macaddr, 4);
			}
		}
	}
	else
	{
		if (FRead(dst, OFFSET_MAC_ADDR_2G, 6) < 0)
		{
			_dprintf("READ MAC address 2G: Out of scope\n");
		}
		else
		{
			ether_etoa(tmp, macaddr);
		}
	}
	fprintf(fp, "MacAddress=%s\n", macaddr);
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, macaddr, strlen(macaddr));
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
	if(nvram_match("wl_atf", "1"))
		fprintf(fp, "VOW_Airtime_Fairness_En=%d\n", 1);
	else
		fprintf(fp, "VOW_Airtime_Fairness_En=%d\n", 0);
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
	if(nvram_match(strcat_r(prefix, "twt", tmp), "1"))
		fprintf(fp, "TWTSupport=%d\n", 1);
	else
		fprintf(fp, "TWTSupport=%d\n", 0);
	if(nvram_match(strcat_r(prefix, "mbo_enable", tmp), "1"))
		fprintf(fp, "MboSupport=%d\n", 1);
	else
		fprintf(fp, "MboSupport=%d\n", 0);
	if(nvram_match(strcat_r(prefix, "11ax", tmp), "1"))
	{ 
		if(nvram_match(strcat_r(prefix, "ofdma", tmp), "1"))
		{
			fprintf(fp, "MuOfdmaDlEnable=%d\n", 1);
			fprintf(fp, "MuOfdmaUlEnable=%d\n", 0);
			fprintf(fp, "MuMimoDlEnable=%d\n", 0);
			fprintf(fp, "MuMimoUlEnable=%d\n", 0);
		}
		else if(nvram_match(strcat_r(prefix, "ofdma", tmp), "2"))
		{
			fprintf(fp, "MuOfdmaDlEnable=%d\n", 1);
			fprintf(fp, "MuOfdmaUlEnable=%d\n", 1);
			fprintf(fp, "MuMimoDlEnable=%d\n", 0);
			fprintf(fp, "MuMimoUlEnable=%d\n", 0);
		}
		else if(nvram_match(strcat_r(prefix, "ofdma", tmp), "3"))
		{
			fprintf(fp, "MuOfdmaDlEnable=%d\n", 1);
			fprintf(fp, "MuOfdmaUlEnable=%d\n", 1);
			fprintf(fp, "MuMimoDlEnable=%d\n", 1);
			fprintf(fp, "MuMimoUlEnable=%d\n", 1);
		}
		else if(nvram_match(strcat_r(prefix, "ofdma", tmp), "4"))
		{
			fprintf(fp, "MuOfdmaDlEnable=%d\n", 1);
			fprintf(fp, "MuOfdmaUlEnable=%d\n", 0);
			fprintf(fp, "MuMimoDlEnable=%d\n", 1);
			fprintf(fp, "MuMimoUlEnable=%d\n", 0);
		}
		else
		{
			fprintf(fp, "MuOfdmaDlEnable=%d\n", 0);
			fprintf(fp, "MuOfdmaUlEnable=%d\n", 0);
			fprintf(fp, "MuMimoDlEnable=%d\n", 0);
			fprintf(fp, "MuMimoUlEnable=%d\n", 0);
		}
	}
	else
	{
		fprintf(fp, "MuOfdmaDlEnable=%d\n", 0);
		fprintf(fp, "MuOfdmaUlEnable=%d\n", 0);
		fprintf(fp, "MuMimoDlEnable=%d\n", 0);
		fprintf(fp, "MuMimoUlEnable=%d\n", 0);
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
	}

	//BeaconPeriod
	str = nvram_safe_get(strcat_r(prefix, "bcn", tmp));
	if (str && *str)
	{
		if (atoi(str) > 1000 || atoi(str) < 20)
		{
			nvram_set(strcat_r(prefix, "bcn", tmp), "100");
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
	str = nvram_safe_get(strcat_r(prefix, "dtim", tmp));
	if (str && *str)
		fprintf(fp, "DtimPeriod=%d\n", atoi(str));
	else
	{
		warning = 11;
		fprintf(fp, "DtimPeriod=%d\n", 1);
	}

	//TxPower
	str = nvram_safe_get(strcat_r(prefix, "txpower", tmp));
	if (nvram_match(strcat_r(prefix, "radio", tmp), "0"))
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
	str = nvram_safe_get(strcat_r(prefix, "gmode_protection", tmp));
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
	fprintf(fp, "TxAntenna=\n");

	//RxAntenna
	fprintf(fp, "RxAntenna=\n");

	//TxPreamble
	str = nvram_safe_get(strcat_r(prefix, "plcphdr", tmp));
	if (str && strcmp(str, "long") == 0)
		fprintf(fp, "TxPreamble=%d\n", 0);
	else if (str && strcmp(str, "short") == 0)
		fprintf(fp, "TxPreamble=%d\n", 1);
	else	/* auto mode applicable for STA only */
		fprintf(fp, "TxPreamble=%d\n", 0);

	//RTSThreshold  Default=2347
	str = nvram_safe_get(strcat_r(prefix, "rts", tmp));
	if (str && *str)
		fprintf(fp, "RTSThreshold=%d\n", atoi(str));
	else
	{
		warning = 14;
		fprintf(fp, "RTSThreshold=%d\n", 2347);
	}

	//FragThreshold  Default=2346
	str = nvram_safe_get(strcat_r(prefix, "frag", tmp));
	if (str && *str)
		fprintf(fp, "FragThreshold=%d\n", atoi(str));
	else
	{
		warning = 15;
		fprintf(fp, "FragThreshold=%d\n", 2346);
	}

	//TxBurst
	str = nvram_safe_get(strcat_r(prefix, "frameburst", tmp));
#ifdef CE_ADAPTIVITY
	if (nvram_match("reg_spec", "CE"))
		;
	else
#endif
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	if (!nvram_match("reg_spec", "EAC"))
	{
#endif
	if (str && *str)
		fprintf(fp, "TxBurst=%d\n", strcmp(str, "off") ? 1 : 0);
	else
	{
		warning = 16;
		fprintf(fp, "TxBurst=%d\n", 1);
	}
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	}
#endif
	//PktAggregate
	str = nvram_safe_get(strcat_r(prefix, "PktAggregate", tmp));
	if (str && *str)
		fprintf(fp, "PktAggregate=%d\n", atoi(str));
	else
	{
		warning = 17;
		fprintf(fp, "PktAggregate=%d\n", 1);
	}

	fprintf(fp, "FreqDelta=%d\n", 0);
	fprintf(fp, "TurboRate=%d\n", 0);

	//WmmCapable
	memset(tmpstr, 0, sizeof(tmpstr));
	memset(wmm_enable, 0, sizeof(wmm_enable));

	str = nvram_safe_get(strcat_r(prefix, "nmode_x", tmp));
	if (str && atoi(str) == 1)	// always enable WMM in N only mode
		sprintf(wmm_enable+strlen(wmm_enable), "%d", 1);
	else
		sprintf(wmm_enable+strlen(wmm_enable), "%d", strcmp(nvram_safe_get(strcat_r(prefix, "wme", tmp)), "off") ? 1 : 0);

	for (i = 0; i < ssid_num; i++)
	{
		if (i)
			strlcat(tmpstr, ";", sizeof(tmpstr));

		strlcat(tmpstr, wmm_enable, sizeof(tmpstr));
	}
	fprintf(fp, "WmmCapable=%s\n", tmpstr);

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
	bzero(wmm_noack, sizeof(char)*8);
	for (i = 0; i < 4; i++)
	{
		sprintf(wmm_noack+strlen(wmm_noack), "%d", strcmp(nvram_safe_get(strcat_r(prefix, "wme_no_ack", tmp)), "on") ? 0 : 1);
		sprintf(wmm_noack+strlen(wmm_noack), "%c", ';');
	}
	wmm_noack[strlen(wmm_noack) - 1] = '\0';
	fprintf(fp, "AckPolicy=%s\n", wmm_noack);

	snprintf(prefix, sizeof(prefix), "wl%d_", band);
//	snprintf(prefix2, sizeof(prefix2), "wl%d_", band);

	//APSDCapable UAPSDCapable
	str = nvram_safe_get(strcat_r(prefix, "wme_apsd", tmp));
	if (str && *str){
		fprintf(fp, "APSDCapable=%d\n", strcmp(str, "off") ? 1 : 0);
		fprintf(fp, "UAPSDCapable=%d\n", strcmp(str, "off") ? 1 : 0);
		fprintf(fp, "ApCliAPSDCapable=%d\n", strcmp(str, "off") ? 1 : 0);
	}
	else
	{
		warning = 18;
		fprintf(fp, "APSDCapable=%d\n", 1);
		fprintf(fp, "UAPSDCapable=%d\n", 1);
		fprintf(fp, "ApCliAPSDCapable=%d\n", 1);
	}

	//DLSDCapable
	str = nvram_safe_get(strcat_r(prefix, "DLSCapable", tmp));
	if (str && *str)
		fprintf(fp, "DLSCapable=%d\n", atoi(str));
	else
	{
		warning = 19;
		fprintf(fp, "DLSCapable=%d\n", 0);
	}

	//NoForwarding pre SSID & NoForwardingBTNBSSID
	memset(tmpstr, 0x0, sizeof(tmpstr));

	str = nvram_safe_get(strcat_r(prefix, "ap_isolate", tmp));
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

			if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
				continue;

			strlcat(tmpstr, ";", sizeof(tmpstr));
		}
		else
			sprintf(prefix_mssid, "wl%d_", band);

		str = nvram_safe_get(strcat_r(prefix_mssid, "closed", temp));

		if (sw_mode == SW_MODE_REPEATER){
			if(i == 1)
				sprintf(tmpstr, "%s", str);
			else if (i > 1)
				strlcat(tmpstr, str, sizeof(tmpstr));
		}else
			strlcat(tmpstr, str, sizeof(tmpstr));
	}
	fprintf(fp, "HideSSID=%s\n", tmpstr);

	//ShortSlot
	fprintf(fp, "ShortSlot=%d\n", 1);

	//AutoChannelSelect
/*
0: Disable
1: Old CSA (AP count)
2: New CSA (CCA)
3: MT7615/MT7915 CSA (Busy time)
*/
	{
		str = nvram_safe_get(strcat_r(prefix, "channel", tmp));

		if (sw_mode == SW_MODE_REPEATER
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
					if(nvram_get_int(strcat_r(prefix_mssid, "channel", tmp)) == 0){
						memset(tmpstr, 0x0, sizeof(tmpstr));
						if ((nvram_get_int(strcat_r(prefix_mssid, "bw", tmp)) == 1) || (nvram_get_int(strcat_r(prefix_mssid, "bw", tmp)) == 3)){
							if(IEEE80211H){
								sprintf(tmpstr,"%d;%d;%d;%d",116,132,136,140);
								fprintf(fp,"AutoChannelSkipList=%s\n",tmpstr);
							}
						}
						else if (nvram_get_int(strcat_r(prefix_mssid, "bw", tmp)) == 2){
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
				if (band && nvram_get_int(strcat_r(prefix, "bw", tmp)) > 0) {
					sprintf(tmpstr,"%d",165);// skip 165 in A band when bw setting to 20/40Mhz or 40Mhz.

#ifdef RTCONFIG_RALINK_DFS
					if(IEEE80211H){
						if((nvram_get_int(strcat_r(prefix, "bw", tmp))==1) || (nvram_get_int(strcat_r(prefix, "bw", tmp))==3)){
							if(!nvram_match("reg_spec", "EAC"))
							{
								if(!strncmp(nvram_safe_get("territory_code"), "JP", 2))
									strlcat(tmpstr, ";132;136;140", sizeof(tmpstr));
//									sprintf(tmpstr,"%s;%d;%d;%d", tmpstr ,132,136,140);	//skip 132 136 140 under auto mode					
								else
									strlcat(tmpstr, ";116;132;136;140", sizeof(tmpstr));
//									sprintf(tmpstr,"%s;%d;%d;%d;%d",tmpstr,116,132,136,140);	//skip 116 132 136 140 under auto mode					
							}				
						}
						else if(nvram_get_int(strcat_r(prefix, "bw", tmp))==2){
							if(!strncmp(nvram_safe_get("territory_code"), "JP", 2))
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
					  !strncmp(nvram_safe_get("territory_code"), "TW", 2) ||

#endif
#ifdef RTCONFIG_HAS_5G
					   nvram_match("wl_reg_5g","5G_BAND24") ||
#endif
#if defined(RTCONFIG_NEW_REGULATION_DOMAIN)
					   (nvram_match("reg_spec","NCC")  ||
                                            nvram_match("reg_spec","NCC2"))
#else
					   (nvram_match(strcat_r(prefix, "country_code", tmp), "TW") ||
					    nvram_match(strcat_r(prefix, "country_code", tmp), "Z3"))
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
						if(!strncmp(nvram_safe_get("territory_code"),&t_code_noselect_2G[i][0],2)) {
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
						if(!strncmp(nvram_safe_get("territory_code"),&t_code_noselect_5G[i][0],2)) {
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
						if(!strncmp(nvram_safe_get("territory_code"),&t_code_noselect3_5G[i][0],2)) {
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
						if(!strncmp(nvram_safe_get("territory_code"),&t_code_noselect_5G[i][0],2)){
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
									if((nvram_get_int(strcat_r(prefix, "bw", tmp))==1) || (nvram_get_int(strcat_r(prefix, "bw", tmp))==3)){
										strlcat(tmpstr, ";116;132;136;140", sizeof(tmpstr));
//										sprintf(tmpstr,"%s;%d;%d;%d;%d",tmpstr,116,132,136,140);	//skip 116 132 136 140 under auto mode					
									}
									else if(nvram_get_int(strcat_r(prefix, "bw", tmp))==2){
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
						if(!strncmp(nvram_safe_get("territory_code"),&t_code_noselect_5G[i][0],2)) {
							if(IEEE80211H)
							{
								if((nvram_get_int(strcat_r(prefix, "bw", tmp)) == 1) || (nvram_get_int(strcat_r(prefix, "bw", tmp)) == 3)){
									sprintf(tmpstr,"%s;%d;%d;%d;%d", tmpstr, 116, 132, 136, 140);
								}
								else if(nvram_get_int(strcat_r(prefix, "bw", tmp)) == 2){
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

			if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
				continue;
		}
		else
			sprintf(prefix_mssid, "wl%d_", band);

		if (nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "radius"))
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
#if defined (RTCONFIG_WLMODULE_MT7615E_AP) || (RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	if (band) {
		if (IEEE80211H)
			fprintf(fp, "DfsEnable=%d\n", 1);
		else
			fprintf(fp, "DfsEnable=%d\n", 0);
		fprintf(fp, "DfsFalseAlarmPrevent=%d\n", 1);
		fprintf(fp, "DfsZeroWaitCacTime=255\n");
	}
#endif
#endif

#ifdef RTCONFIG_AP_CARRIER_DETECTION
	if (nvram_match(strcat_r(prefix, "country_code", tmp), "JP"))
	{
		fprintf(fp, "RDRegion=%s\n", "JAP");
		fprintf(fp, "CarrierDetect=%d\n", 1);
	}
	else
#endif
	{
#ifdef RTCONFIG_RALINK_DFS
		fprintf(fp, "RDRegion=%s\n", nvram_get("reg_spec"));
#else
		fprintf(fp, "RDRegion=\n");
#endif
		fprintf(fp, "DfsZeroWaitDefault=%d\n", 0);
		fprintf(fp, "DfsDedicatedZeroWait=%d\n", 0);
		fprintf(fp, "CarrierDetect=%d\n", 0);
	}
//	if (band)
	fprintf(fp, "ChannelGeography=%d\n", 2);
	fprintf(fp, "PreAntSwitch=\n");
	fprintf(fp, "PhyRateLimit=%d\n", 0);
	fprintf(fp, "DebugFlags=%d\n", 0);
	fprintf(fp, "FineAGC=%d\n", 0);
#if !defined(RTCONFIG_MT798X)
	if(band)
		fprintf(fp, "StreamMode=%d\n", 3);
	else
#endif
		fprintf(fp, "StreamMode=%d\n", 0);
	fprintf(fp, "StreamModeMac0=\n");
	fprintf(fp, "StreamModeMac1=\n");
	fprintf(fp, "StreamModeMac2=\n");
	fprintf(fp, "StreamModeMac3=\n");
	fprintf(fp, "StationKeepAlive=%d\n", 0);
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
	str = nvram_safe_get(strcat_r(prefix, "GreenAP", tmp));
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
			snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d.%d_", band, i);
		else
			snprintf(prefix_mssid, sizeof(prefix_mssid), "wl%d_", band);

		if (i)
			strlcat(tmpstr, ";", sizeof(tmpstr));
		strlcat(tmpstr, strstr(nvram_safe_get(strcat_r(prefix_mssid, "auth_mode_x", temp)), "wpa") ? "1" : "0", sizeof(tmpstr));
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

			if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
				continue;

			if(strlen(tmpstr))
				strlcat(tmpstr, ";", sizeof(tmpstr));
		}
		else
			sprintf(prefix_mssid, "wl%d_", band);

		str = nvram_safe_get(strcat_r(prefix_mssid, "auth_mode_x", temp));
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

			if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
				continue;
			if(strlen(tmpstr))
				strlcat(tmpstr, ";", sizeof(tmpstr));
		}
		else
			sprintf(prefix_mssid, "wl%d_", band);

		if ((nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "open")
			&& nvram_match(strcat_r(prefix_mssid, "wep_x", temp), "0")))
			strlcat(tmpstr, "NONE", sizeof(tmpstr));
		else if ((nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "open")
			&& nvram_invmatch(strcat_r(prefix_mssid, "wep_x", temp), "0")) ||
				nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "shared") ||
				nvram_match(strcat_r(prefix_mssid, "auth_mode_x", temp), "radius"))
			strlcat(tmpstr, "WEP", sizeof(tmpstr));
		else if (nvram_match(strcat_r(prefix_mssid, "crypto", temp), "tkip"))
		{
			strlcat(tmpstr, "TKIP", sizeof(tmpstr));
		}
		else if (nvram_match(strcat_r(prefix_mssid, "crypto", temp), "aes"))
		{
			strlcat(tmpstr, "AES", sizeof(tmpstr));
		}
		else if (nvram_match(strcat_r(prefix_mssid, "crypto", temp), "tkip+aes"))
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

	//RekeyInterval
	str = nvram_safe_get(strcat_r(prefix, "wpa_gtk_rekey", tmp));
	if (str && *str)
	{
		if (atol(str) == 0)
			fprintf(fp, "RekeyMethod=%s\n", "DISABLE");
		else
			fprintf(fp, "RekeyMethod=TIME\n");

		fprintf(fp, "RekeyInterval=%ld\n", atol(str));
	}
	else
	{
		warning = 26;
		fprintf(fp, "RekeyMethod=%s\n", "DISABLE");
		fprintf(fp, "RekeyInterval=%d\n", 0);
	}

	//PMKCachePeriod (in minutes)
	str = nvram_safe_get(strcat_r(prefix, "pmk_cache", tmp));
	if (str && *str)
		fprintf(fp, "PMKCachePeriod=%ld\n", atol(str));
	else
	fprintf(fp, "PMKCachePeriod=%d\n", 10);

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

				if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);

			sprintf(tmpstr, "WPAPSK%d=%s\n", j + 1, nvram_safe_get(strcat_r(prefix_mssid, "wpa_psk", temp)));
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

			if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
				continue;

			if(strlen(tmpstr)==0)
				sprintf(tmpstr, "%s", tmpstr);
			else
				sprintf(tmpstr, "%s;", tmpstr);
		}
		else
			sprintf(prefix_mssid, "wl%d_", band);

		str = nvram_safe_get(strcat_r(prefix_mssid, "key", temp));
		sprintf(tmpstr, "%s%s", tmpstr, str);
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
			(i && (nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))))
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

				if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);

			sprintf(tmpstr, "Key1Str%d=%s\n", j + 1, nvram_safe_get(strcat_r(prefix_mssid, "key1", temp)));
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

				if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);

			sprintf(tmpstr, "Key2Str%d=%s\n", j + 1, nvram_safe_get(strcat_r(prefix_mssid, "key2", temp)));
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

				if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);

			sprintf(tmpstr, "Key3Str%d=%s\n", j + 1, nvram_safe_get(strcat_r(prefix_mssid, "key3", temp)));
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

				if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
					continue;
				else
					j++;
			}
			else
				sprintf(prefix_mssid, "wl%d_", band);

			sprintf(tmpstr, "Key4Str%d=%s\n", j + 1, nvram_safe_get(strcat_r(prefix_mssid, "key4", temp)));
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

	//HT_RDG
	str = nvram_safe_get(strcat_r(prefix, "HT_RDG", tmp));
#ifdef CE_ADAPTIVITY
	if (nvram_match("reg_spec", "CE"))
		;
	else
#endif	/* CE_ADAPTIVITY */
	if (str && strlen(str))
		fprintf(fp, "HT_RDG=%d\n", atoi(str));
	else
	{
		warning = 29;
		fprintf(fp, "HT_RDG=%d\n", 0);
	}

	//HT_LinkAdapt
	str = nvram_safe_get(strcat_r(prefix, "HT_LinkAdapt", tmp));
	if (str && *str)
		fprintf(fp, "HT_LinkAdapt=%d\n", atoi(str));
	else
	{
		warning = 30;
		fprintf(fp, "HT_LinkAdapt=%d\n", 1);
	}

	//HT_OpMode
//	str = nvram_safe_get(strcat_r(prefix, "HT_OpMode", tmp));
	str = nvram_safe_get(strcat_r(prefix, "mimo_preamble", tmp));
	int opmode = strcmp(str, "mm") ? 1 : 0;
	if (str && *str)
//		fprintf(fp, "HT_OpMode=%d\n", atoi(str));
		fprintf(fp, "HT_OpMode=%d\n", opmode);
	else
	{
		warning = 31;
		fprintf(fp, "HT_OpMode=%d\n", 0);
	}

	//HT_MpduDensity
	str = nvram_safe_get(strcat_r(prefix, "HT_MpduDensity", tmp));
	if (str && *str)
		fprintf(fp, "HT_MpduDensity=%d\n", atoi(str));
	else
	{
		warning = 32;
		fprintf(fp, "HT_MpduDensity=%d\n", 5);
	}

	int Channel = nvram_get_int(strcat_r(prefix, "channel", tmp));
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
		str = nvram_safe_get(strcat_r(prefix, "nctrlsb", tmp));
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
				fprintf(fp, "HT_BW=%d\n", 0);
		else if ((wl_bw > 0) && (HTBW_MAX == 1))
			fprintf(fp, "HT_BW=%d\n", 1);
	#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
		else if ((wlc_express - 1) == band)   //express way (apclii0)
			fprintf(fp, "HT_BW=%d\n", 1);
	#endif
		else
		{
	//		warning = 34;
			fprintf(fp, "HT_BW=%d\n", 0);
		}

		//HT_BSSCoexistence
		if ((wl_bw > 1) && (HTBW_MAX == 1) 
	#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_CONCURRENTREPEATER)
			&& (wlc_express == 0 || (wlc_express - 1) != band)
	#else
			&& !((sw_mode == SW_MODE_REPEATER) && (wlc_band == band))
	#endif
			)
			fprintf(fp, "HT_BSSCoexistence=%d\n", 0);
		else
			fprintf(fp, "HT_BSSCoexistence=%d\n", 1);	

	//HT_AutoBA
	str = nvram_safe_get(strcat_r(prefix, "HT_AutoBA", tmp));
	if (str && *str)
		fprintf(fp, "HT_AutoBA=%d\n", atoi(str));
	else
	{
		warning = 35;
		fprintf(fp, "HT_AutoBA=%d\n", 1);
	}

	//HT_BADecline
	str = nvram_safe_get(strcat_r(prefix, "HT_BADecline", tmp));
	if (str && *str)
		fprintf(fp, "HT_BADecline=%d\n", atoi(str));
	else
	{
		warning = 36;
		fprintf(fp, "HT_BADecline=%d\n", 0);
	}

	//HT_AMSDU
	str = nvram_safe_get(strcat_r(prefix, "HT_AMSDU", tmp));
	if (str && *str)
		fprintf(fp, "HT_AMSDU=%d\n", atoi(str));
	else
	{
		warning = 37;
		fprintf(fp, "HT_AMSDU=%d\n", 0);
	}

	//HT_BAWinSize
	str = nvram_safe_get(strcat_r(prefix, "HT_BAWinSize", tmp));
	if (str && *str)
		fprintf(fp, "HT_BAWinSize=%d\n", atoi(str));
	else
	{
		warning = 38;
		fprintf(fp, "HT_BAWinSize=%d\n", 64);
	}

	//HT_GI
	str = nvram_safe_get(strcat_r(prefix, "HT_GI", tmp));
	if (str && *str)
	{
		fprintf(fp, "HT_GI=%d\n", atoi(str));
		if(band)
			fprintf(fp, "VHT_SGI=%d\n", atoi(str));
	}
	else
	{
		warning = 39;
		fprintf(fp, "HT_GI=%d\n", 1);
		if(band)
			fprintf(fp, "VHT_SGI=%d\n", 1);
	}

	if(band){
		fprintf(fp, "VHT_STBC=%d\n",1);
		fprintf(fp, "VHT_LDPC=%d\n",1);
	}

	//HT_STBC
	str = nvram_safe_get(strcat_r(prefix, "HT_STBC", tmp));
	if (str && *str)
		fprintf(fp, "HT_STBC=%d\n", atoi(str));
	else
	{
		warning = 40;
		fprintf(fp, "HT_STBC=%d\n", 1);
	}

	//HT_MCS
	str = nvram_safe_get(strcat_r(prefix, "HT_MCS", tmp));
	if (str && *str)
		fprintf(fp, "HT_MCS=%d\n", atoi(str));
	else
	{
		warning = 41;
		fprintf(fp, "HT_MCS=%d\n", 33);
	}

	//HT_TxStream
	str = nvram_safe_get(strcat_r(prefix, "HT_TxStream", tmp));
	if (str && *str)
		fprintf(fp, "HT_TxStream=%d\n", atoi(str));
	else
	{
		warning = 42;
		fprintf(fp, "HT_TxStream=%d\n", 2);
	}

	//HT_RxStream
	str = nvram_safe_get(strcat_r(prefix, "HT_RxStream", tmp));
	if (str && *str)
		fprintf(fp, "HT_RxStream=%d\n", atoi(str));
	else
	{
		warning = 43;
		fprintf(fp, "HT_RxStream=%d\n", 3);
	}

	//HT_PROTECT
//	str = nvram_safe_get(strcat_r(prefix, "HT_PROTECT", tmp));
	str = nvram_safe_get(strcat_r(prefix, "nmode_protection", tmp));
	int nprotect = strcmp(str, "auto") ? 0 : 1;
	if (str && *str)
//		fprintf(fp, "HT_PROTECT=%d\n", atoi(str));
		fprintf(fp, "HT_PROTECT=%d\n", nprotect);
	else
	{
		warning = 44;
		fprintf(fp, "HT_PROTECT=%d\n", 1);
	}

	//HT_DisallowTKIP
	fprintf(fp, "HT_DisallowTKIP=%d\n", 1);
	if (band) {
		if (wl_bw > 0)
		{
			if (wl_bw == 2)	// 40 MHz					
				fprintf(fp, "VHT_BW=%d\n", 0);
#if defined(RTCONFIG_VHT160) && !defined(RALINK_DBDC_MODE)
			else if (sw_mode != SW_MODE_REPEATER && (wl_bw == 5 || nvram_match(strcat_r(prefix, "bw_160", tmp), "1"))) // 20/40/80/160 MHz
				fprintf(fp, "VHT_BW=%d\n", 2);
#endif
			else // wl_bw == 3 ==> 20/40/80 MHz
				fprintf(fp, "VHT_BW=%d\n", 1);
		}
		else
		{
			warning = 8;
			fprintf(fp, "VHT_BW=%d\n", 1);//20/40/80 MHz
		}
			
		str = nvram_get(strcat_r(prefix_mssid, "VHT_DisallowNonVHT", tmp));
		if (str && *str)
			fprintf(fp, "VHT_DisallowNonVHT=%d\n", atoi(str));
		else
			fprintf(fp, "VHT_DisallowNonVHT=%d\n", 0);
		fprintf(fp, "VHT_BW_SIGNAL=%d\n", 0);
	}
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	else
		fprintf(fp, "VHT_BW=%d\n", 0);// 40 MHz
#endif

#if defined(RMAC2100)
	// TxBF
	if (band)
#endif
	{
	/* mu-mimo */
#if defined(RTCONFIG_MUMIMO_2G) || defined(RTCONFIG_MUMIMO_5G)
		mumimo = atoi(nvram_safe_get(strcat_r(prefix, "mumimo", tmp)));
#endif
#ifdef RTCONFIG_TXBF_BAND3ONLY
		if (sw_mode == SW_MODE_AP) {

			cht =  nvram_get_int(strcat_r(prefix, "channel", tmp));
			if (cht >= 100 && cht <= 144) { // BAND3
				nvram_set_int(strcat_r(prefix, "txbf", tmp), 1);
				nvram_set_int(strcat_r(prefix, "txbf_en", tmp), 1);
			}
			else {
				nvram_set_int(strcat_r(prefix, "txbf", tmp), 0);
				nvram_set_int(strcat_r(prefix, "txbf_en", tmp), 0);
			}

		} else {
			nvram_set_int(strcat_r(prefix, "txbf", tmp), 0);
			nvram_set_int(strcat_r(prefix, "txbf_en", tmp), 0);
		}
#endif		
		str = nvram_safe_get(strcat_r(prefix, "txbf", tmp));
		if ((atoi(str) > 0) && nvram_match(strcat_r(prefix, "txbf_en", tmp), "1"))
		{
			if(!nvram_match("territory_code", ""))
				fprintf(fp, "BFBACKOFFenable=%d\n", 1);
			else
				fprintf(fp, "BFBACKOFFenable=%d\n", 0);
			str = nvram_safe_get(strcat_r(prefix, "itxbf", tmp));
			fprintf(fp, "ITxBfEn=%d\n", (atoi(str) ? 1 : 0));
			fprintf(fp, "ETxBfIncapable=%d\n", 0);
			fprintf(fp, "ETxBfEnCond=%d\n", 1);

#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
			if (band)
#endif
				fprintf(fp, "MUTxRxEnable=%d\n", (mumimo ? 1 : 0));
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
			fprintf(fp, "MuMimoDlEnable=%d\n", (mumimo ? 1 : 0));
			fprintf(fp, "MuMimoUlEnable=%d\n", 0);
#endif
		}
		else
		{
			str = nvram_safe_get(strcat_r(prefix, "itxbf", tmp));
			fprintf(fp, "ITxBfEn=%d\n", (atoi(str) ? 1 : 0));
#if defined(RTCONFIG_MUMIMO_2G) || defined(RTCONFIG_MUMIMO_5G)
			if (band) {
				if (mumimo) {
					fprintf(fp, "ETxBfEnCond=%d\n", 1);
					fprintf(fp, "MUTxRxEnable=%d\n", 1);
				}
				else
				{
					fprintf(fp, "ETxBfEnCond=%d\n", 0);
					fprintf(fp, "MUTxRxEnable=%d\n", 0);
				}
			}
			else
				fprintf(fp, "ETxBfEnCond=%d\n", 0);
#else
			fprintf(fp, "ETxBfEnCond=%d\n", 0);
#endif
			fprintf(fp, "ETxBfIncapable=%d\n", 0);
		}
	}
	//WscConfStatus
	str2 = nvram_safe_get("w_Setting");

	if (str2)
//		wsc_configure |= atoi(str2);
		wsc_configure = atoi(str2);

	if (wsc_configure == 0)
	{
		fprintf(fp, "WscConfMode=%d\n", 0);
		fprintf(fp, "WscConfStatus=%d\n", 1);
		g_wsc_configured = 0;						// AP is unconfigured
		nvram_set(strcat_r(prefix, "wsc_config_state", tmp), "0");
	}
	else
	{
		fprintf(fp, "WscConfMode=%d\n", 0);
		fprintf(fp, "WscConfStatus=%d\n", 2);
		g_wsc_configured = 1;						// AP is configured
		nvram_set(strcat_r(prefix, "wsc_config_state", tmp), "1");
	}

	fprintf(fp, "WscVendorPinCode=%s\n", nvram_safe_get("secret_code"));

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


		if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
			continue;

		if (nvram_invmatch(strcat_r(prefix_mssid, "macmode", tmp), "disabled"))
		{
			nv = nvp = strdup(nvram_safe_get(strcat_r(prefix_mssid, "maclist_x", tmp)));
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
	if (sw_mode == SW_MODE_REPEATER || nvram_match(strcat_r(prefix, "mode_x", tmp), "0"))
	{
		fprintf(fp, "WdsEnable=%d\n", 0);
		fprintf(fp, "WdsPhyMode=%d\n", 0);
		fprintf(fp, "WdsEncrypType=%s\n", "NONE");
		fprintf(fp, "WdsList=\n");
		fprintf(fp, "Wds0Key=\n");
		fprintf(fp, "Wds1Key=\n");
		fprintf(fp, "Wds2Key=\n");
		fprintf(fp, "Wds3Key=\n");
	}
	else
	{
		str = nvram_safe_get(strcat_r(prefix, "mode_x", tmp));
		if (str && *str)
		{
			if ((nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "open") ||
				(nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "psk2") && nvram_match(strcat_r(prefix, "crypto", tmp), "aes")))
			)
			{
				if (atoi(str) == 0)
					fprintf(fp, "WdsEnable=%d\n", 0);
				else if (atoi(str) == 1)
					fprintf(fp, "WdsEnable=%d\n", 2);
				else if (atoi(str) == 2)
				{
					if (nvram_match(strcat_r(prefix, "wdsapply_x", tmp), "0"))
						fprintf(fp, "WdsEnable=%d\n", 4);
					else
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
		str = nvram_safe_get(strcat_r(prefix, "mode_x", tmp));
		if (str && strlen(str))
		{
			{
				if ((nvram_get_int(strcat_r(prefix, "nmode_x", tmp)) == 0))
					fprintf(fp, "WdsPhyMode=%s\n", "HTMIX");
				else if ((nvram_get_int(strcat_r(prefix, "nmode_x", tmp)) == 1))
					fprintf(fp, "WdsPhyMode=%s\n", "GREENFIELD");
				else
					fprintf(fp, "WdsPhyMode=\n");
			}
		}

		//WdsEncrypType
		if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "open") && nvram_match(strcat_r(prefix, "wep_x", tmp), "0"))
			fprintf(fp, "WdsEncrypType=%s\n", "NONE;NONE;NONE;NONE");
		else if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "open") && nvram_invmatch(strcat_r(prefix, "wep_x", tmp), "0"))
			fprintf(fp, "WdsEncrypType=%s\n", "WEP;WEP;WEP;WEP");
		else if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "psk2") && nvram_match(strcat_r(prefix, "crypto", tmp), "aes"))
			fprintf(fp, "WdsEncrypType=%s\n", "AES;AES;AES;AES");
		else
			fprintf(fp, "WdsEncrypType=%s\n", "NONE;NONE;NONE;NONE");

		list[0]=0;
		list[1]=0;
		if ((nvram_match(strcat_r(prefix, "mode_x", tmp), "1") || (nvram_match(strcat_r(prefix, "mode_x", tmp), "2") && nvram_match(strcat_r(prefix, "wdsapply_x", tmp), "1"))) &&
			(nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "open") ||
			(nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "psk2") && nvram_match(strcat_r(prefix, "crypto", tmp), "aes")))
		)
		{
			nv = nvp = strdup(nvram_safe_get(strcat_r(prefix, "wdslist", tmp)));
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
		if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "open") && nvram_match(strcat_r(prefix, "wep_x", tmp), "0"))
		{
			fprintf(fp, "WdsDefaultKeyID=\n");
			fprintf(fp, "Wds0Key=\n");
			fprintf(fp, "Wds1Key=\n");
			fprintf(fp, "Wds2Key=\n");
			fprintf(fp, "Wds3Key=\n");
		}
		else if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "open") && nvram_invmatch(strcat_r(prefix, "wep_x", tmp), "0"))
		{
			fprintf(fp, "WdsDefaultKeyID=%s;%s;%s;%s\n", nvram_safe_get(strcat_r(prefix, "key", tmp)), nvram_safe_get(strcat_r(prefix, "key", tmp)), nvram_safe_get(strcat_r(prefix, "key", tmp)), nvram_safe_get(strcat_r(prefix, "key", tmp)));
	//		sprintf(list, "wl%d_key%s", band, nvram_safe_get(strcat_r(prefix, "key", tmp)));
			str = strcat_r(prefix, "key", tmp);
			str2 = nvram_safe_get(str);
			sprintf(list, "%s%s", str, str2);

			fprintf(fp, "Wds0Key=%s\n", nvram_safe_get(list));
			fprintf(fp, "Wds1Key=%s\n", nvram_safe_get(list));
			fprintf(fp, "Wds2Key=%s\n", nvram_safe_get(list));
			fprintf(fp, "Wds3Key=%s\n", nvram_safe_get(list));
		}
		else if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "psk2") && nvram_match(strcat_r(prefix, "crypto", tmp), "aes"))
		{
			fprintf(fp, "WdsKey=%s\n", nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp)));
			fprintf(fp, "Wds0Key=%s\n", nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp)));
			fprintf(fp, "Wds1Key=%s\n", nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp)));
			fprintf(fp, "Wds2Key=%s\n", nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp)));
			fprintf(fp, "Wds3Key=%s\n", nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp)));
		}

	} // sw_mode

//	fprintf(fp, "WirelessEvent=%d\n", 0);

	//RADIUS_Server
	//RADIUS_Port
	//RADIUS_Key
	if(flag_8021x)
	{
		char RADIUS_Server[128];
		char RADIUS_Port[64];
		char *radius_server, *radius_port, *radius_key;

		memset(RADIUS_Server, 0, sizeof(RADIUS_Server));
		memset(RADIUS_Port, 0, sizeof(RADIUS_Port));
		radius_server = nvram_safe_get(strcat_r(prefix, "radius_ipaddr", tmp));
		radius_port   = nvram_safe_get(strcat_r(prefix, "radius_port", tmp));
		radius_key    = nvram_safe_get(strcat_r(prefix, "radius_key", tmp));
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

				if (!nvram_match(strcat_r(prefix_mssid, "bss_enabled", temp), "1"))
					continue;
			}

			{
				fprintf(fp, "RADIUS_Key%d=%s\n", j, radius_key);
			}
			strlcat(RADIUS_Server, radius_server, sizeof(RADIUS_Server));
			strlcat(RADIUS_Server, ";", sizeof(RADIUS_Server));
			strlcat(RADIUS_Port, radius_port, sizeof(RADIUS_Port));
			strlcat(RADIUS_Port, ";", sizeof(RADIUS_Port));
//			sprintf(RADIUS_Server, "%s%s;", RADIUS_Server, radius_server);	//cannot be empty ";"
//			sprintf(RADIUS_Port  , "%s%s;", RADIUS_Port  , radius_port);	//cannot be empty ";"
			j++;
		}
		for( ;j < MAX_NO_MSSID +1; j++)
			fprintf(fp, "RADIUS_Key%d=\n", j);

		if ((i = strlen(RADIUS_Server)) > 0)
			RADIUS_Server[ i-1 ] = '\0';
		if ((i = strlen(RADIUS_Port)) > 0)
			RADIUS_Port[ i-1 ] = '\0';
		fprintf(fp, "RADIUS_Server=%s\n", RADIUS_Server);
		fprintf(fp, "RADIUS_Port=%s\n"  , RADIUS_Port);
	}
	else
	{
		warning = 50;
		for(j = 1; j < MAX_NO_MSSID +1; j++)
			fprintf(fp, "RADIUS_Key%d=\n", j);
		fprintf(fp, "RADIUS_Server=\n");
		fprintf(fp, "RADIUS_Port=\n");	//default 1812
	}


	fprintf(fp, "RADIUS_Acct_Server=\n");
	fprintf(fp, "RADIUS_Acct_Port=%d\n", 1813);
	fprintf(fp, "RADIUS_Acct_Key=\n");

	//own_ip_addr
	if (flag_8021x == 1)
	{
		fprintf(fp, "own_ip_addr=%s\n", nvram_safe_get("lan_ipaddr"));
		fprintf(fp, "Ethifname=\n");
		fprintf(fp, "EAPifname=%s\n", "br0");
		fprintf(fp, "PreAuthifname=%s\n", "br0");
	}
	else
	{
		fprintf(fp, "own_ip_addr=\n");
		fprintf(fp, "Ethifname=\n");
		fprintf(fp, "EAPifname=\n");
		fprintf(fp, "PreAuthifname=\n");
	}

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
			else if (!strcmp(str, "psk") || !strcmp(str, "psk2"))
			{
				if (!strcmp(str, "psk"))
					fprintf(fp, "ApCliAuthMode=%s\n", "WPAPSK");
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
	if (sw_mode == SW_MODE_REPEATER && wlc_band == band && nvram_invmatch("wlc_ssid", ""))
	{
		int flag_wep = 0;
		int p;
	// convert wlc_xxx to wlX_ according to wlc_band == band
		nvram_set("ure_disable", "0");

		nvram_set("wl_ssid", nvram_safe_get("wlc_ssid"));
		nvram_set(strcat_r(prefix, "ssid", tmp), nvram_safe_get("wlc_ssid"));
		nvram_set(strcat_r(prefix, "auth_mode_x", tmp), nvram_safe_get("wlc_auth_mode"));

		nvram_set(strcat_r(prefix, "wep_x", tmp), nvram_safe_get("wlc_wep"));

		nvram_set(strcat_r(prefix, "key", tmp), nvram_safe_get("wlc_key"));
		for(p = 1; p <= 4; p++)
		{
			char prekey[16];
			snprintf(prekey, sizeof(prekey), "key%d", p);
			if(nvram_get_int("wlc_key")==p)
				nvram_set(strcat_r(prefix, prekey, tmp), nvram_safe_get("wlc_wep_key"));
		}

		nvram_set(strcat_r(prefix, "crypto", tmp), nvram_safe_get("wlc_crypto"));
		nvram_set(strcat_r(prefix, "wpa_psk", tmp), nvram_safe_get("wlc_wpa_psk"));
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
// ui force 40,but 7615/7915 support 80 and 160
		if(band)
			nvram_set(strcat_r(prefix, "bw", tmp), "1");
		else
#endif
		nvram_set(strcat_r(prefix, "bw", tmp), nvram_safe_get("wlc_nbw_cap"));



		fprintf(fp, "ApCliEnable=1\n");
		fprintf(fp, "ApCliSsid=%s\n", nvram_safe_get("wlc_ssid"));
		fprintf(fp, "ApCliBssid=\n");

		str = nvram_safe_get("wlc_auth_mode");
		if (str && strlen(str))
		{
			if (!strcmp(str, "open") && nvram_match("wlc_wep", "0"))
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
			else if (!strcmp(str, "psk") || !strcmp(str, "psk2"))
			{
				if (!strcmp(str, "psk"))
					fprintf(fp, "ApCliAuthMode=%s\n", "WPAPSK");
				else
					fprintf(fp, "ApCliAuthMode=%s\n", "WPA2PSK");

				//EncrypType
				if (nvram_match("wlc_crypto", "tkip"))
					fprintf(fp, "ApCliEncrypType=%s\n", "TKIP");
				else if (nvram_match("wlc_crypto", "aes"))
					fprintf(fp, "ApCliEncrypType=%s\n", "AES");

				//WPAPSK
				fprintf(fp, "ApCliWPAPSK=%s\n", nvram_safe_get("wlc_wpa_psk"));
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
			fprintf(fp, "ApCliDefaultKeyID=%s\n", nvram_safe_get("wlc_key"));

			//KeyType (0 -> Hex, 1->Ascii)
			for(p = 1 ; p <= 4; p++)
			{
				if(nvram_get_int("wlc_key")==p)
				{
				   	if((strlen(nvram_safe_get("wlc_wep_key"))==5)||(strlen(nvram_safe_get("wlc_wep_key"))==13))
						fprintf(fp, "ApCliKey%dType=1\n",p);
					else if((strlen(nvram_safe_get("wlc_wep_key"))==10)||(strlen(nvram_safe_get("wlc_wep_key"))==26))
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
				if(nvram_get_int("wlc_key")==p)
					fprintf(fp, "ApCliKey%dStr=%s\n",p,nvram_safe_get("wlc_wep_key"));
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
#if defined(MAC_REPEATER)
		fprintf(fp, "MACRepeaterEn=1\n");
		fprintf(fp, "MACRepeaterOuiMode=2\n");
#else
		fprintf(fp, "MACRepeaterEn=0\n");
		fprintf(fp, "MACRepeaterOuiMode=0\n");
#endif
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
		fprintf(fp, "MACRepeaterEn=0\n");
		fprintf(fp, "MACRepeaterOuiMode=0\n");
	}

	//RadioOn
	str = nvram_safe_get(strcat_r(prefix, "radio", tmp));
	if (str && *str)
		fprintf(fp, "RadioOn=%d\n", atoi(str));
	else
	{
		warning = 51;
		fprintf(fp, "RadioOn=%d\n", 1);
	}

	fprintf(fp, "SSID=\n");
	fprintf(fp, "WPAPSK=\n");
	fprintf(fp, "Key1Str=\n");
	fprintf(fp, "Key2Str=\n");
	fprintf(fp, "Key3Str=\n");
	fprintf(fp, "Key4Str=\n");

	/* Wireless IGMP Snooping */
	fprintf(fp, "IgmpSnEnable=%d\n", nvram_get_int(strcat_r(prefix, "igs", tmp)) ? 1 : 0);
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
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
	i = nvram_get_int(strcat_r(prefix, "mrate_x", tmp));
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
	/* No CCK for 5Ghz band */
	if (band && mcast_phy == 1)
		goto next_mrate;
	fprintf(fp, "McastPhyMode=%d\n", mcast_phy);
	fprintf(fp, "McastMcs=%d\n", mcast_mcs);
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

	fprintf(fp, "WscV2Support=%d\n", 0);	// WPS/WSC v2 feature allows client to create connection via the PinCode of AP.
						// Disable this feature causes longer detection time (click AP till show dialog box) on WIN7 client when WSC disabled either.
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
	/* Set number of clients of guest network. */
	for (i = 0; i < ssid_num; i++) {
		int maxsta;

		if (!i) {
			fprintf(fp, "MaxStaNum=0;");
		} else {
			sprintf(prefix_mssid, "wl%d.%d_", band, i);
			maxsta = nvram_get_int(strcat_r(prefix_mssid, "guest_num", temp));
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
	fprintf(fp, "AMSDU_NUM=%d\n", 8);
#else
	fprintf(fp, "AMSDU_NUM=%d\n", 4);
#endif
	fprintf(fp, "CP_SUPPORT=%d\n", 2);
	fprintf(fp, "RED_Enable=%d\n", 1);
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	if(band) {
		str = nvram_safe_get(strcat_r(prefix, "turbo_qam", tmp));
		if (str && *str)
			fprintf(fp, "Vht1024QamSupport=%d\n", atoi(str));
	} else {
		str = nvram_safe_get(strcat_r(prefix, "turbo_qam", tmp));
		if (str && *str)
			fprintf(fp, "G_BAND_256QAM=%d\n", atoi(str));
	}
#else
#if defined (RTCONFIG_QAM256_2G)
	/* 2.4G 256QAM */
	if(!band) {
		str = nvram_safe_get(strcat_r(prefix, "turbo_qam", tmp));
		if (str && *str)
			fprintf(fp, "G_BAND_256QAM=%d\n", atoi(str));
	}
#endif
#endif
	if(*tcode)
		fprintf(fp, "SKUenable=1\n");
	else
		fprintf(fp, "SKUenable=0\n");
	fprintf(fp, "WirelessEvent=1\n");

#if defined(RTCONFIG_MT798X)
	fprintf(fp, "SREnable=1\n");
	fprintf(fp, "SRMode=0\n");
	fprintf(fp, "SRDPDEnable=0\n");
	fprintf(fp, "SRSDEnable=1\n");
	fprintf(fp, "PPEnable=1\n");
	fprintf(fp, "WHNAT=1\n");
	fprintf(fp, "IsICAPFW=0\n");
	fprintf(fp, "KernelRps=1\n");
	fprintf(fp, "BSSColorValue=255\n");
	fprintf(fp, "BcnProt=0\n");
	fprintf(fp, "WiFiTest=0\n");
	fprintf(fp, "CCKTxStream=4\n");
#if defined(RTCONFIG_EASYMESH)
	fprintf(fp, "QoSR1Enable=1\n");
#endif
	fprintf(fp, "DscpPriMapEnable=1\n");
	fprintf(fp, "ScsEnable=0\n");
	fprintf(fp, "QoSMgmtCapa=0\n");
	fprintf(fp, "ApMWDS=0;0;0\n");
	fprintf(fp, "ApCliMWDS=0\n");
#endif

#if defined(RTCONFIG_SWRT_KVR)
	if(nvram_match(strcat_r(prefix, "kvr_kv", tmp), "1")){
		fprintf(fp, "WNMEnable=1\n");
		fprintf(fp, "RRMEnable=1\n");
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

int startScan(int band)
{
	int lock;
	char data[64];
	struct iwreq wrq;

	memset(data, 0, sizeof(data));
	strcpy(data, "SiteSurvey=1");
	wrq.u.data.length = strlen(data) + 1;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	lock = file_lock("sitesurvey");
	if(wl_ioctl(get_wifname(band), RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		file_unlock(lock);
		dbg("Site Survey fails\n");
		return 0;
	}
	file_unlock(lock);
	sleep(4);
	return 1;
}

int getSiteSurvey(int band, char* ofile)
{
	int i = 0, apCount = 0;
	char data[8192];
	char header[128];
	struct iwreq wrq;
	SSA *ssap;
	int lock;
	FILE *fp;
	char ssid_str[256],tmp[256];
	char ure_mac[18];
	int wl_authorized = 0;
	memset(data, 0, sizeof(data));
	strcpy(data, "SiteSurvey=1");
	wrq.u.data.length = strlen(data) + 1;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

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

	lock = file_lock("sitesurvey");
	if (wl_ioctl(get_wifname(band), RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		file_unlock(lock);

		dbg("Site Survey fails\n");
		return 0;
	}
	file_unlock(lock);

	dbg("Please wait");
	sleep(1);
	dbg(".");
	sleep(1);
	dbg(".");
	sleep(1);
	dbg(".");
	sleep(1);
	dbg(".\n\n");

	memset(data, 0, sizeof(data));
	strcpy(data, "");
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	if (wl_ioctl(get_wifname(band), RTPRIV_IOCTL_GSITESURVEY, &wrq) < 0)
	{
		dbg("errors in getting site survey result\n");
		return 0;
	}
	memset(header, 0, sizeof(header));
	sprintf(header, "%-4s%-33s%-20s%-23s%-9s%-12s%-7s%-3s%-4s%-5s\n", "Ch", "SSID", "BSSID", "Security", "Signal(%)", "W-Mode", "ExtCH", "NT", "WPS", "DPID");
	dbg("\n%s", header);

	if (wrq.u.data.length > 0 && strlen(wrq.u.data.pointer)>0)
	{
		ssap=(SSA *)(wrq.u.data.pointer+strlen(header)+1);
		int len = strlen(wrq.u.data.pointer+strlen(header))-1;
		char *sp, *op;
 		op = sp = wrq.u.data.pointer+strlen(header)+1;

		while (*sp && ((len - (sp-op)) >= 0))
		{
			ssap->SiteSurvey[i].channel[3] = '\0';
			ssap->SiteSurvey[i].ssid[32] = '\0';
			ssap->SiteSurvey[i].bssid[19] = '\0';
			ssap->SiteSurvey[i].security[22] = '\0';
			ssap->SiteSurvey[i].signal[8] = '\0';
			ssap->SiteSurvey[i].wmode[11] = '\0';
			ssap->SiteSurvey[i].extch[6] = '\0';
			ssap->SiteSurvey[i].nt[2] = '\0';
			ssap->SiteSurvey[i].wps[3] = '\0';
			ssap->SiteSurvey[i].dpid[4] = '\0';
			sp+=strlen(header);
			apCount=++i;
		}

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

int getSiteSurveyVSIE(int band, struct _SITESURVEY_VSIE *result, int length)
{
	char data[8192];
	struct iwreq wrq;
	int lock;

	memset(data, 0, sizeof(data));
	strcpy(data, "SiteSurvey=1");
	wrq.u.data.length = strlen(data) + 1;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	lock = file_lock("sitesurvey");
	if (wl_ioctl(get_wifname(band), RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		file_unlock(lock);
		dbg("Site Survey fails\n");
		return 0;
	}
	file_unlock(lock);
	sleep(4);

	memset(data, 0, sizeof(data));
	strcpy(data, "");
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = ASUS_SUBCMD_GETSITESURVEY_VSIE;

	if (wl_ioctl(get_wifname(band), RTPRIV_IOCTL_ASUSCMD, &wrq) < 0)
	{
		dbg("errors in getting site survey vie result\n");
		return 0;
	}

	if (wrq.u.data.length > 0 && strlen(wrq.u.data.pointer) > 0)
	{
		memcpy(result, wrq.u.data.pointer, wrq.u.data.length);
	}
	else
	{
		dbg("The output var result is NULL\n");
		return 0;
	}
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
int site_survey_for_channel(int n, const char *wif, int *HT_EXT)
{
	char tmp[128], header[128],prefix[] = "wlXXXXXXXXXX_";
	char *ssid;

	snprintf(prefix, sizeof(prefix), "wl%d_", n);
	ssid = nvram_safe_get(strcat_r(prefix, "ssid", tmp));

	if (!ssid || !strcmp(ssid, "")) {
		return -1;
	}

	int i = 0, apCount = 0;
	char data[8192];
	struct iwreq wrq;
	SSA *ssap;
	int chk_hidden_ap = nvram_get_int(strcat_r(prefix, "hide_pap", tmp));
	int channellistnum, commonchannel, centralchannel, ht_extcha = 1;
	int wep = 0;

	if (nvram_invmatch(strcat_r(prefix, "wep_x", tmp), "0")
			|| nvram_match(strcat_r(prefix, "auth_mode", tmp), "psk"))
		wep = 1;

	memset(data, 0, 255);
	if (chk_hidden_ap == 1) {
		sprintf(data, "SiteSurvey=%s", ssid);
	}else
		strcpy(data, "SiteSurvey=1");
	wrq.u.data.length = strlen(data) + 1;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	if (wl_ioctl(wif, RTPRIV_IOCTL_SET, &wrq) < 0) {
		fprintf(stderr, "Site Survey fails\n");
		return -1;
	}

	fprintf(stderr, "Look for SSID: %s\n", ssid);
	fprintf(stderr, "Please wait");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".\n");

	memset(data, 0, sizeof(data));
	strcpy(data, "");
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	if (wl_ioctl(wif, RTPRIV_IOCTL_GSITESURVEY, &wrq) < 0) {
		fprintf(stderr, "errors in getting site survey result\n");
		return -1;
	}

	memset(header, 0, sizeof(header));
	sprintf(header, "%-4s%-33s%-20s%-23s%-9s%-12s%-7s%-3s%-4s%-5s\n", "Ch", "SSID", "BSSID", "Security", "Signal(%)", "W-Mode", "ExtCH", "NT", "WPS", "DPID");
	//dbg("\n%s", header);
	if (wrq.u.data.length > 0) {
		char commch[4];
		char cench[4];
		int signal_max = -1, signal_tmp = -1, ht_extcha_max, idx = -1;
		ssap = (SSA *)(wrq.u.data.pointer + strlen(header) + 1);
		int len = strlen(wrq.u.data.pointer + strlen(header))-1;
		char *sp, *op;
 		op = sp = wrq.u.data.pointer + strlen(header) + 1;

		while (*sp && ((len - (sp-op)) >= 0)) {
			ssap->SiteSurvey[i].channel[3] = '\0';
			ssap->SiteSurvey[i].ssid[32] = '\0';
			ssap->SiteSurvey[i].bssid[19] = '\0';
			ssap->SiteSurvey[i].security[22] = '\0';
			ssap->SiteSurvey[i].signal[8] = '\0';
			ssap->SiteSurvey[i].wmode[11] = '\0';
			ssap->SiteSurvey[i].extch[6] = '\0';
			ssap->SiteSurvey[i].nt[2] = '\0';
			ssap->SiteSurvey[i].wps[3] = '\0';
			ssap->SiteSurvey[i].dpid[4] = '\0';
			sp += strlen(header);
			apCount = ++i;
		}

		if (apCount) {
			for (i = 0; i < apCount; i++) {
				memset(commch,0,sizeof(commch));
				memcpy(commch, ssap->SiteSurvey[i].channel, 3);
				commonchannel=atoi(commch);
				//fprintf(stderr, "##common ch=%d##\n",commonchannel);
#if 0
				memset(cench,0,sizeof(cench));
				memcpy(cench,ssap->SiteSurvey[i].centralchannel,3);
				centralchannel = atoi(cench);
				if (strstr(ssap->SiteSurvey[i].bsstype, "n")
						&& (commonchannel != centralchannel)) {
					if (n) {
						if (centralchannel < commonchannel)
							ht_extcha = 0;
						else
							ht_extcha = 1;
					}
					else {
						if (commonchannel <= 4)
							ht_extcha = 1;
						else if (commonchannel > 4 && commonchannel < 8) {
							if (centralchannel < commonchannel)
								ht_extcha = 0;
							else
								ht_extcha = 1;
						}
						else if (commonchannel >= 8) {
							char *value = nvram_safe_get("wl0_reg");

							if (!strcmp(value,"2G_CH11"))
								channellistnum = 11;
							else if (!strcmp(value,"2G_CH14"))
								channellistnum = 14;
							else	// 2G_CH13
								channellistnum = 13;

							if ((channellistnum - commonchannel) < 4)
								ht_extcha = 0;
							else {
								if (centralchannel < commonchannel)
									ht_extcha = 0;
								else
									ht_extcha = 1;
							}
						}
					}
				}
				else
#endif
					ht_extcha = -1;
/*
				_dprintf(
					"%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n",
					ssap->SiteSurvey[i].channel,
					(char*)ssap->SiteSurvey[i].ssid,
					ssap->SiteSurvey[i].bssid,
					ssap->SiteSurvey[i].encryption,
					ssap->SiteSurvey[i].authmode,
					ssap->SiteSurvey[i].signal,
					ssap->SiteSurvey[i].wmode);

*/
				if ((ssid && !strcmp(ssid, trim_r(ssap->SiteSurvey[i].ssid)))/*non-hidden AP*/
				 ) {

					if (chk_hidden_ap == 1) {
							nvram_set(strcat_r(prefix, "bssid", tmp), ssap->SiteSurvey[i].bssid);
					}

					if (!strncmp(ssap->SiteSurvey[i].bssid, nvram_safe_get(strcat_r(prefix, "bssid", tmp)), 17)) {
						*HT_EXT = ht_extcha;
						return commonchannel;
					}
					else if ((signal_tmp = atoi(trim_r(ssap->SiteSurvey[i].signal))) > signal_max) {
						signal_max = signal_tmp;
						//ht_extcha_max = ht_extcha;
						idx = commonchannel;
					}
				}
			}
			fprintf(stderr, "\n");

		}

		if (idx != -1) {
			//*HT_EXT = ht_extcha_max;
			return idx;
		}
	}

	return -1;
}
#endif	/* RTCONFIG_WIRELESSREPEATER */

int get_channel(int band)
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

void gen_ra_config(const char* wif)
{
	char word[256], *next;

	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		SKIP_ABSENT_FAKE_IFACE(word);
		if (!strcmp(word, wif))
		{
			if (!strcmp(word, nvram_safe_get("wl0_ifname"))) // 2.4G
			{
#if defined(RTCONFIG_MT798X)
				if (!strncmp(word, "rax", 3))	// iNIC
#else
				if (!strncmp(word, "rai", 3))	// iNIC
#endif
					gen_ralink_config(0, 1);
				else{
					gen_ralink_config(0, 0);
#if defined(RALINK_DBDC_MODE) || defined(RTCONFIG_MT798X) || defined(RTCONFIG_WLMODULE_MT7915D_AP)
					gen_ralink_config(1, 1);
#endif
				}
			}
			else if (!strcmp(word, nvram_safe_get("wl1_ifname"))) // 5G
			{
#if defined(RTCONFIG_MT798X)
				if (!strncmp(word, "rax", 3))	// iNIC
#else
				if (!strncmp(word, "rai", 3))	// iNIC
#endif
					gen_ralink_config(1, 1);
				else
					gen_ralink_config(1, 0);
			}
		}
	}
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
		int channel = get_channel(band);

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
	int ht_ext;

	if(sw_mode() == SW_MODE_REPEATER)
	{
#if defined(RTCONFIG_CONCURRENTREPEATER)
		int wlc_express = nvram_get_int("wlc_express");
		if (wlc_express == 0) {		// concurrent
			aif = nvram_safe_get("wl0_ifname");
			enable_apcli(aif, 0);
			aif = nvram_safe_get("wl1_ifname");
			enable_apcli(aif, 1);
		}
		else if (wlc_express == 1) {	// 2.4G express way
			aif = nvram_safe_get("wl0_ifname");
			enable_apcli(aif, 0);
		}
		else if (wlc_express == 2) {	// 5G express way
			aif = nvram_safe_get("wl1_ifname");
			enable_apcli(aif, 1);
		}
		else
			fprintf(stderr,"## No correct wlc_express for apcli ##\n");
#else /* RTCONFIG_CONCURRENTREPEATER */
		int wlc_band = nvram_get_int("wlc_band");
		aif = get_staifname(wlc_band);
#if defined(RTCONFIG_MT798X)
		ifconfig(aif, IFUP, NULL, NULL);//up ifname
		eval("brctl", "addif", "br0", aif);//add to bridge
#endif
		ch = site_survey_for_channel(wlc_band, aif, &ht_ext);
		if(ch != -1)
		{
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
			doSystem("iwpriv %s set Channel=%d", aif, ch);
#endif
			doSystem("iwpriv %s set ApCliEnable=1", aif);
			doSystem("iwpriv %s set ApCliAutoConnect=1", aif);
			fprintf(stderr, "##set channel=%d, enable apcli ..#\n", ch);
		}
		else
			fprintf(stderr, "## Can not find pap's ssid ##\n");
#endif /* RTCONFIG_CONCURRENTREPEATER */
	}
}

void stop_wds_ra(const char* lan_ifname, const char* wif)
{
	char prefix[32];
	char wdsif[32];
	int i;

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
		sprintf(wdsif, "%s%d", prefix, i);
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
#if defined(RTCONFIG_CONCURRENTREPEATER)
int get_apcli_status(int wlc_band)
#else
int get_apcli_status(void)
#endif
{
#if !defined(RTCONFIG_CONCURRENTREPEATER)
	int wlc_band;
#endif
	const char *ifname;
	char data[32];
	struct iwreq wrq;
	int status;
	static int old_status[2] = {-1, -1};
#if !defined(RTCONFIG_CONCURRENTREPEATER)
	wlc_band = nvram_get_int("wlc_band");
#endif
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

#if defined(RTCONFIG_RALINK_MT7620) || defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_MT798X) || defined(RTCONFIG_RALINK_EN7561)
	if(unit == 0)
#else
	if(unit == 1)
#endif
		aif = "apcli0";
	else
#if defined(RTCONFIG_MT798X)
		aif = "apclix0";
#else
		aif = "apclii0";
#endif

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
#ifdef RTCONFIG_CONCURRENTREPEATER

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
	int ht_ext = -1;
	static long lastUptime[2] = {-FIND_CHANNEL_INTERVAL, -FIND_CHANNEL_INTERVAL};  // doing "site survey" takes time. use this to prolong interval between site survey.
	long Uptime;

	Uptime = uptime();
	if((ret = get_apcli_status(band))==0) //init
	{
		if ((Uptime > lastUptime[band] && Uptime < lastUptime[band] + FIND_CHANNEL_INTERVAL)
		   || (Uptime < lastUptime[band] && Uptime < FIND_CHANNEL_INTERVAL))
			return ret;

#if defined(RTCONFIG_RALINK_MT7620) || defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_MT798X) || defined(RTCONFIG_RALINK_EN7561)
		if(band == 0)
#else
		if(band == 1)
#endif
			aif = "apcli0";
		else
#if defined(RTCONFIG_MT798X)
			aif = "apclix0";
#else
			aif = "apclii0";
#endif

		ch = site_survey_for_channel(band, aif, &ht_ext);
		if(ch != -1)
		{
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
			doSystem("iwpriv %s set Channel=%d", aif, ch);
#endif
			doSystem("iwpriv %s set ApCliEnable=1", aif);
			doSystem("ifconfig %s up", aif);
			dbg("set pap's channel=%d, enable apcli ..#\n",ch);
			doSystem("iwpriv %s set ApCliAutoConnect=1", aif);

			lastUptime[band] = Uptime;
		}
		else
			lastUptime[band] = -FIND_CHANNEL_INTERVAL;
	}
	else
		lastUptime[band] = Uptime;

	return ret;
}
#endif

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
	int ch;
	const char *aif;
	int band;
	int ht_ext = -1;
	static long lastUptime = -FIND_CHANNEL_INTERVAL; // doing "site survey" takes time. use this to prolong interval between site survey.
	long Uptime;

	Uptime = uptime();
	if((ret = get_apcli_status())==0) //init
	{
		if ((Uptime > lastUptime && Uptime < lastUptime + FIND_CHANNEL_INTERVAL)
		   || (Uptime < lastUptime && Uptime < FIND_CHANNEL_INTERVAL))
			return ret;

		band = nvram_get_int("wlc_band");
#if defined(RTCONFIG_RALINK_MT7620) || defined(RTCONFIG_RALINK_MT7621) || defined(RTCONFIG_RALINK_MT7628) || defined(RTCONFIG_MT798X) || defined(RTCONFIG_RALINK_EN7561)
		if(band == 0)
#else
		if(band == 1)
#endif
			aif = "apcli0";
		else
#if defined(RTCONFIG_MT798X)
			aif = "apclix0";
#else
			aif = "apclii0";
#endif

		ch = site_survey_for_channel(band, aif, &ht_ext);
		if(ch != -1)
		{
#if defined(RTCONFIG_WLMODULE_MT7615E_AP)
			doSystem("iwpriv %s set Channel=%d", aif, ch);
#endif
			doSystem("iwpriv %s set ApCliEnable=1", aif);
			doSystem("ifconfig %s up", aif);
			dbg("set pap's channel=%d, enable apcli ..#\n",ch);
			doSystem("iwpriv %s set ApCliAutoConnect=1", aif);
			lastUptime = Uptime;
		}
		else
			lastUptime = -FIND_CHANNEL_INTERVAL;
	}
	else
		lastUptime = Uptime;
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
		led_control(LED_LAN, LED_ON);
	else
		led_control(LED_LAN, LED_OFF);
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
