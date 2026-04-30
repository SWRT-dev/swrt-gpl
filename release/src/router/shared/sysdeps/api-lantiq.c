/* rewrite by paldier */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <typedefs.h>
#include <bcmnvram.h>
#include <sys/ioctl.h>
#include <lantiq.h>
#include <iwlib.h>
#include "utils.h"
#include "shutils.h"
#include <shared.h>
#include <trxhdr.h>
#include <bcmutils.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <iwlib.h>
#ifndef O_BINARY
#define O_BINARY 	0
#endif
#include <image.h>
#ifndef MAP_FAILED
#define MAP_FAILED (-1)
#endif


typedef uint32_t __u32;

u_int
ieee80211_mhz2ieee(u_int freq)
{
#define IS_CHAN_IN_PUBLIC_SAFETY_BAND(_c) ((_c) > 4940 && (_c) < 4990)
	if (freq < 2412)
		return 0;
	if (freq == 2484)
		return 14;
	if (freq < 2484)
		return (freq - 2407) / 5;
	if (freq < 5000) {
		if (IS_CHAN_IN_PUBLIC_SAFETY_BAND(freq)) {
			return ((freq * 10) + (((freq % 5) == 2) ? 5 : 0) - 49400)/5;
		} else if (freq > 4900) {
			return (freq - 4000) / 5;
		} else {
			return 15 + ((freq - 2512) / 20);
		}
	}
#ifdef RTCONFIG_WIFI6E
	if (freq >= 6115 && freq <= 7115) {
		return (freq - 5950) / 5;
	}
#endif

	return (freq - 5000) / 5;
}
/////////////

//phy0->wlan0->wifi0
const char WIF_5G2[] = "wifi2";
const char WIF_5G[] = "wifi1";
const char WIF_2G[] = "wifi0";
const char STA_5G2[] = "sta2";
const char STA_5G[] = "sta1";
const char STA_2G[] = "sta0";
const char VPHY_5G2[] = "wlan4";
const char VPHY_5G[] = "wlan2";
const char VPHY_2G[] = "wlan0";
const char PHY_5G2[] = "phy2";
const char PHY_5G[] = "phy1";
const char PHY_2G[] = "phy0";

const char *max_2g_ax_mode = "11GHE";	/* B,G,N,AX */
const char *max_5g_ax_mode = "11AHE";	/* A,N,AC,AX */
const char *max_2g_n_mode = "11NG";	/* B,G,N */
const char *max_5g_ac_mode = "11ACV";	/* A,N,AC */

/* [0]: 11AC
 * [1]: 11AX, 11BE
 */
const char *bw20[2] = { "HT20", "20" };
const char *bw40[2] = { "HT40", "40" };
const char *bw80[2] = { "HT80", "80" };
const char *bw80_80_tbl[2] = { "HT80_80", "80_80" };
const char *bw160_tbl[2] = { "HT160", "160" };

#define GPIOLIB_DIR	"/sys/class/gpio"
#ifdef RTCONFIG_LEDS_CLASS
#define LEDSLIB_DIR	"/sys/class/leds"
#endif

/* Export specified GPIO
 * @return:
 * 	0:	success
 *  otherwise:	fail
 */
static int __export_gpio(uint32_t gpio)
{
	char gpio_path[PATH_MAX], export_path[PATH_MAX], gpio_str[] = "999XXX";

	if (!d_exists(GPIOLIB_DIR)) {
		_dprintf("%s does not exist!\n", __func__);
		return -1;
	}
	snprintf(gpio_path, sizeof(gpio_path),"%s/gpio%d", GPIOLIB_DIR, gpio);
	if (d_exists(gpio_path))
		return 0;

	snprintf(export_path, sizeof(export_path), "%s/export", GPIOLIB_DIR);
	snprintf(gpio_str, sizeof(gpio_str), "%d", gpio);
	f_write_string(export_path, gpio_str, 0, 0);

	return 0;
}

uint32_t gpio_dir(uint32_t gpio, int dir)
{
	char path[PATH_MAX], v[10], *dir_str = "in";

	if (dir == GPIO_DIR_OUT) {
		dir_str = "out";		/* output, low voltage */
		*v = '\0';
		snprintf(path, sizeof(path), "%s/gpio%d/value", GPIOLIB_DIR, gpio);
		if (f_read_string(path, v, sizeof(v)) > 0 && safe_atoi(v) == 1)
			dir_str = "high";	/* output, high voltage */
	} else if (dir == GPIO_DIR_OUT_LOW) {
		dir_str = "low";
	} else if (dir == GPIO_DIR_OUT_HIGH) {
		dir_str = "high";
	}

	__export_gpio(gpio);
	snprintf(path, sizeof(path), "%s/gpio%d/direction", GPIOLIB_DIR, gpio);
	f_write_string(path, dir_str, 0, 0);

	return 0;
}

uint32_t get_gpio(uint32_t gpio)
{
	char path[PATH_MAX], value[10];

#ifdef RTCONFIG_LEDS_CLASS
	snprintf(path, sizeof(path), "%s/led%d/brightness", LEDSLIB_DIR, gpio);
#else
	snprintf(path, sizeof(path), "%s/gpio%d/value", GPIOLIB_DIR, gpio);
#endif
	f_read_string(path, value, sizeof(value));

	return safe_atoi(value);
}

uint32_t set_gpio(uint32_t gpio, uint32_t value)
{
	char path[PATH_MAX], val_str[10];

	snprintf(val_str, sizeof(val_str), "%d", !!value);
#ifdef RTCONFIG_LEDS_CLASS
	snprintf(path, sizeof(path), "%s/led%d/brightness", LEDSLIB_DIR, gpio);
#else
	snprintf(path, sizeof(path), "%s/gpio%d/value", GPIOLIB_DIR, gpio);
#endif
	f_write_string(path, val_str, 0, 0);

	return 0;
}

int get_switch_model(void)
{
	// TODO
	return SWITCH_UNKNOWN;
}

uint32_t get_phy_status(int wan_unit)
{
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	char *wan_ifname;
	char buf[32];

	snprintf(prefix, sizeof(prefix), "wan%d_", wan_unit);
	wan_ifname = nvram_pf_safe_get(prefix, "ifname");

	if(strlen(wan_ifname) <= 0)
		return 0;

	snprintf(tmp, sizeof(tmp), "/sys/class/net/%s/operstate", wan_ifname);

	f_read_string(tmp, buf, sizeof(buf));
	if(!strncmp(buf, "up", 2))
		return 1;
	else
		return 0;
}

uint32_t get_phy_speed(int wan_unit)
{
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	char *wan_ifname;
	char buf[32];

	snprintf(prefix, sizeof(prefix), "wan%d_", wan_unit);
	wan_ifname = nvram_pf_safe_get(prefix, "ifname");

	if(strlen(wan_ifname) <= 0)
		return 0;

	snprintf(tmp, sizeof(tmp), "/sys/class/net/%s/speed", wan_ifname);

	f_read_string(tmp, buf, sizeof(buf));
	return strtoul(buf, NULL, 10);
}

uint32_t set_phy_ctrl(int wan_unit, int ctrl)
{
	// TODO
	return 1;		/* FIXME */
}

int wl_ioctl(const char *ifname, int cmd, struct iwreq *pwrq)
{
	int ret = 0;
	int s;

	/* open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return errno;
	}

	/* do it */
	strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
	if ((ret = ioctl(s, cmd, pwrq)) < 0)
		perror(pwrq->ifr_name);

	/* cleanup */
	close(s);
	return ret;
}

int wl_wave_unit(int unit){
	return unit;
}

char *wl_vifname_wave(int unit, int subunit)
{
	static char tmp[128];

	if(!client_mode() && (subunit > 0) && (subunit < 4))
		snprintf(tmp, sizeof(tmp), "wl%d.%d", unit, subunit);
	else
		snprintf(tmp, sizeof(tmp), "%s", get_wififname(unit));
	return tmp;
}

int wl_client_unit(int unit){
	return unit;
}

int get_wifname_band(char *name){
	if(!strcmp(name, WIF_5G) || !strcmp(name, STA_5G))
		return 1;
	else if(!strcmp(name, WIF_5G2) || !strcmp(name, STA_5G2))
		return 2;
	else
		return 0;
}

unsigned int get_radio_status(char *ifname)
{
	struct ifreq ifr;
	int sfd;
	int ret;

	if ((sfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0)
	{
		strcpy(ifr.ifr_name, ifname);
		ret = ioctl(sfd, SIOCGIFFLAGS, &ifr);
		close(sfd);
		if (ret == 0)
			return !!(ifr.ifr_flags & IFF_UP);
	}
	return 0;
}

int match_radio_status(int unit, int status)
{
	int sub = 0, rs = status;
	char prefix[] = "wlXXXXXXXXXXXXXX", athfix[]="athXXXXXX";

	do {
		if (sub > 0)
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, sub);
		else
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		strcpy(athfix, nvram_pf_safe_get(prefix, "ifname"));

		if (*athfix != '\0') {
			if (status)
				rs &= get_radio_status(athfix);
			else
				rs |= get_radio_status(athfix);
		}
		sub++;
	} while (sub <= 3);

	return (status == rs);
}

int get_radio(int unit, int subunit)
{
	char prefix[] = "wlXXXXXXXXXXXXXX";

	if (subunit > 0)
		snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);

	if (subunit > 0)
		return nvram_pf_match(prefix, "radio", "1");
	else
		return get_radio_status(nvram_pf_safe_get(prefix, "ifname"));
}

void set_radio(int on, int unit, int subunit)
{
	int onoff = (!on)? LED_OFF:LED_ON;
	int led = get_wl_led_id(unit);
	int sub = (subunit >= 0) ? subunit : 0;
	char prefix[] = "wlXXXXXXXXXXXXXX", athfix[]="athXXXXXX";
	char path[sizeof(NAWDS_SH_FMT) + 6], wds_iface[IFNAMSIZ] = "";
	char conf_path[sizeof("/etc/Wireless/conf/hostapd_athXXX.confYYYYYY")];
	char pid_path[sizeof("/var/run/hostapd_athXXX.pidYYYYYY")];
	char entropy_path[sizeof("/var/run/entropy_athXXX.binYYYYYY")];
	if (unit < WL_2G_BAND || unit >= WL_NR_BANDS) {
		dbg("%s: wl%d is not supported!\n", __func__, unit);
		return;
	}

	do {
		if (sub > 0)
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, sub);
		else
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		strlcpy(athfix, nvram_pf_safe_get(prefix, "ifname"), sizeof(athfix));

		if (*athfix != '\0' && strncmp(athfix, "sta", 3)) {
			/* all lan-interfaces except sta when running repeater mode */
			_dprintf("%s: unit %d-%d, %s\n", __func__, unit, sub, (on?"on":"off"));
			eval("ifconfig", athfix, on? "up":"down");
			if (on) {
				char cfg[sizeof("bss_config=") + IFNAMSIZ + sizeof(":/etc/Wireless/conf/hostapd_XXX.conf") + IFNAMSIZ];
				snprintf(cfg, sizeof(cfg), "bss_config=%s:/etc/Wireless/conf/hostapd_%s.conf", athfix, athfix);
				eval(QWPA_CLI, "-g", QHOSTAPD_CTRL_IFACE, "raw", "ADD", cfg);
			} else {
				eval(QWPA_CLI, "-g", QHOSTAPD_CTRL_IFACE, "raw", "REMOVE", athfix);
			}

			/* Reconnect to peer WDS AP */
			if (!sub) {
				snprintf(path, sizeof(path), NAWDS_SH_FMT, wds_iface);
				if (!nvram_pf_match(prefix, "mode_x", "0") && f_exists(path))
					doSystem(path);
			}
		}
		sub++;
	} while (subunit < 0 && sub <= 3);

	led_control(led, inhibit_led_on()? LED_OFF : onoff);
}

char *wif_to_vif(char *wif)
{
	static char vif[32];
	int unit = 0, subunit = 0;
	char prefix[] = "wlXXXXXXXXXXXXXX";

	vif[0] = '\0';

	for (unit = 0; unit < MAX_NR_WL_IF; unit++) {
		SKIP_ABSENT_BAND(unit);
		for (subunit = 1; subunit < MAX_NO_MSSID; subunit++) {
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);

			if (nvram_pf_match(prefix, "ifname", wif)) {
				snprintf(vif, sizeof(vif), "%s", prefix);
				goto RETURN_VIF;
			}
		}
	}

RETURN_VIF:
	return vif;
}

/* get channel list via currently setting in wifi driver */
int get_channel_list_via_driver(int unit, char *buffer, int len)
{
	int l, r, found, freq, first = 1;
	FILE *fp;
	char *p = buffer, line[256], cmd[sizeof("iw phy0 infoXXXXXXXXXXXX")];

	if (buffer == NULL || len <= 0 || unit < 0 || unit >= MAX_NR_WL_IF)
		return -1;

	memset(buffer, 0, len);
	snprintf(cmd, sizeof(cmd), "iw %s info", get_realphyifname(unit));
	fp = popen(cmd, "r");
	if (!fp)
		return 0;

	/* Example:
	 * Wiphy phy0
	 *       Band 1:
	 *               Capabilities: 0x00
	 *                       HT20
	 *                       Static SM Power Save
	 *                       No RX STBC
	 *                       Max AMSDU length: 3839 bytes
	 *                       No DSSS/CCK HT40
	 *               Maximum RX AMPDU length 65535 bytes (exponent: 0x003)
	 *               Minimum RX AMPDU time spacing: 8 usec (0x06)
	 *               HT TX/RX MCS rate indexes supported: 1-12
	 *               Frequencies:
	 *                       * 58320 MHz [1] (0.0 dBm)
	 *                       * 60480 MHz [2] (0.0 dBm)
	 *                       * 62640 MHz [3] (0.0 dBm)
	 *               Bitrates (non-HT):
	 */
	r = found = 0;
	while (len > 0 && fgets(line, sizeof(line), fp)) {
		if (!found) {
			if (!strstr(line, "Frequencies:")) {
				continue;
			} else {
				found = 1;
				continue;
			}
		} else {
			if (strstr(line, "disabled"))
				continue;
			if (!strstr(line, "MHz") || (r = sscanf(line, "%*[^0-9]%d Mhz%*[^\n]", &freq)) != 1) {
				found = 0;
				continue;
			}
			l = snprintf(p, len, "%s%u", first? "" : ",", ieee80211_mhz2ieee(freq));
			p += l;
			len -= l;
			first = 0;
		}
	}
	pclose(fp);
	return (p - buffer);
}

/* get channel list via value of countryCode */
unsigned char A_BAND_REGION_0_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 149, 153, 157, 161, 165 };
unsigned char A_BAND_REGION_1_CHANNEL_LIST[] = { 36, 40, 44, 48 };

#ifdef RTCONFIG_LOCALE2012
unsigned char A_BAND_REGION_2_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165 };
unsigned char A_BAND_REGION_3_CHANNEL_LIST[] =
    { 52, 56, 60, 64, 149, 153, 157, 161, 165 };
#else
unsigned char A_BAND_REGION_2_CHANNEL_LIST[] = { 36, 40, 44, 48 };
unsigned char A_BAND_REGION_3_CHANNEL_LIST[] = { 149, 153, 157, 161 };
#endif
unsigned char A_BAND_REGION_4_CHANNEL_LIST[] = { 149, 153, 157, 161, 165 };
unsigned char A_BAND_REGION_5_CHANNEL_LIST[] = { 149, 153, 157, 161 };

#ifdef RTCONFIG_LOCALE2012
unsigned char A_BAND_REGION_6_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 132, 136, 140, 149, 153, 157, 161, 165 };
#else
unsigned char A_BAND_REGION_6_CHANNEL_LIST[] = { 36, 40, 44, 48 };
#endif
unsigned char A_BAND_REGION_7_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165, 169, 173 };
unsigned char A_BAND_REGION_8_CHANNEL_LIST[] = { 52, 56, 60, 64 };

#ifdef RTCONFIG_LOCALE2012
unsigned char A_BAND_REGION_9_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132 };
#else
unsigned char A_BAND_REGION_9_CHANNEL_LIST[] = { 36, 40, 44, 48 };
#endif
unsigned char A_BAND_REGION_10_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 149, 153, 157, 161, 165 };
unsigned char A_BAND_REGION_11_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 149, 153, 157, 161 };
unsigned char A_BAND_REGION_12_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140 };
unsigned char A_BAND_REGION_13_CHANNEL_LIST[] =
    { 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161 };
unsigned char A_BAND_REGION_14_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 136, 140, 149, 153, 157, 161, 165 };
unsigned char A_BAND_REGION_15_CHANNEL_LIST[] =
    { 149, 153, 157, 161, 165, 169, 173 };
unsigned char A_BAND_REGION_16_CHANNEL_LIST[] =
    { 52, 56, 60, 64, 149, 153, 157, 161, 165 };
unsigned char A_BAND_REGION_17_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 149, 153, 157, 161 };
unsigned char A_BAND_REGION_18_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140 };
unsigned char A_BAND_REGION_19_CHANNEL_LIST[] =
    { 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161 };
unsigned char A_BAND_REGION_20_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 149, 153, 157, 161 };
unsigned char A_BAND_REGION_21_CHANNEL_LIST[] =
    { 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161 };

unsigned char G_BAND_REGION_0_CHANNEL_LIST[] =
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
unsigned char G_BAND_REGION_1_CHANNEL_LIST[] =
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
unsigned char G_BAND_REGION_5_CHANNEL_LIST[] =
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

#define A_BAND_REGION_0				0
#define A_BAND_REGION_1				1
#define A_BAND_REGION_2				2
#define A_BAND_REGION_3				3
#define A_BAND_REGION_4				4
#define A_BAND_REGION_5				5
#define A_BAND_REGION_6				6
#define A_BAND_REGION_7				7
#define A_BAND_REGION_8				8
#define A_BAND_REGION_9				9
#define A_BAND_REGION_10			10
#define A_BAND_REGION_11			11
#define A_BAND_REGION_12			12
#define A_BAND_REGION_13			13
#define A_BAND_REGION_14			14
#define A_BAND_REGION_15			15
#define A_BAND_REGION_16			16
#define A_BAND_REGION_17			17
#define A_BAND_REGION_18			18
#define A_BAND_REGION_19			19
#define A_BAND_REGION_20			20
#define A_BAND_REGION_21			21

#define G_BAND_REGION_0				0
#define G_BAND_REGION_1				1
#define G_BAND_REGION_2				2
#define G_BAND_REGION_3				3
#define G_BAND_REGION_4				4
#define G_BAND_REGION_5				5
#define G_BAND_REGION_6				6

typedef struct CountryCodeToCountryRegion {
	unsigned char IsoName[3];
	unsigned char RegDomainNum11A;
	unsigned char RegDomainNum11G;
} COUNTRY_CODE_TO_COUNTRY_REGION;

COUNTRY_CODE_TO_COUNTRY_REGION allCountry[] = {
	/* {Country Number, ISO Name, Country Name, Support 11A, 11A Country Region, Support 11G, 11G Country Region} */
	{"DB", A_BAND_REGION_7, G_BAND_REGION_5},
	{"AL", A_BAND_REGION_0, G_BAND_REGION_1},
	{"DZ", A_BAND_REGION_0, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"AR", A_BAND_REGION_0, G_BAND_REGION_1},
	{"AM", A_BAND_REGION_1, G_BAND_REGION_1},
#else
	{"AR", A_BAND_REGION_3, G_BAND_REGION_1},
	{"AM", A_BAND_REGION_2, G_BAND_REGION_1},
#endif
	{"AU", A_BAND_REGION_0, G_BAND_REGION_1},
	{"AT", A_BAND_REGION_1, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"AZ", A_BAND_REGION_1, G_BAND_REGION_1},
#else
	{"AZ", A_BAND_REGION_2, G_BAND_REGION_1},
#endif
	{"BH", A_BAND_REGION_0, G_BAND_REGION_1},
	{"BY", A_BAND_REGION_0, G_BAND_REGION_1},
	{"BE", A_BAND_REGION_1, G_BAND_REGION_1},
	{"BZ", A_BAND_REGION_4, G_BAND_REGION_1},
	{"BO", A_BAND_REGION_4, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"BR", A_BAND_REGION_4, G_BAND_REGION_1},
#else
	{"BR", A_BAND_REGION_1, G_BAND_REGION_1},
#endif
	{"BN", A_BAND_REGION_4, G_BAND_REGION_1},
	{"BG", A_BAND_REGION_1, G_BAND_REGION_1},
	{"CA", A_BAND_REGION_0, G_BAND_REGION_0},
	{"CL", A_BAND_REGION_0, G_BAND_REGION_1},
	{"CN", A_BAND_REGION_4, G_BAND_REGION_1},
	{"CO", A_BAND_REGION_0, G_BAND_REGION_0},
	{"CR", A_BAND_REGION_0, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"HR", A_BAND_REGION_1, G_BAND_REGION_1},
#else
	{"HR", A_BAND_REGION_2, G_BAND_REGION_1},
#endif
	{"CY", A_BAND_REGION_1, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"CZ", A_BAND_REGION_1, G_BAND_REGION_1},
#else
	{"CZ", A_BAND_REGION_2, G_BAND_REGION_1},
#endif
	{"DK", A_BAND_REGION_1, G_BAND_REGION_1},
	{"DO", A_BAND_REGION_0, G_BAND_REGION_0},
	{"EC", A_BAND_REGION_0, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"EG", A_BAND_REGION_1, G_BAND_REGION_1},
#else
	{"EG", A_BAND_REGION_2, G_BAND_REGION_1},
#endif
	{"SV", A_BAND_REGION_0, G_BAND_REGION_1},
	{"EE", A_BAND_REGION_1, G_BAND_REGION_1},
	{"FI", A_BAND_REGION_1, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"FR", A_BAND_REGION_1, G_BAND_REGION_1},
	{"GE", A_BAND_REGION_1, G_BAND_REGION_1},
#else
	{"FR", A_BAND_REGION_2, G_BAND_REGION_1},
	{"GE", A_BAND_REGION_2, G_BAND_REGION_1},
#endif
	{"DE", A_BAND_REGION_1, G_BAND_REGION_1},
	{"GR", A_BAND_REGION_1, G_BAND_REGION_1},
	{"GT", A_BAND_REGION_0, G_BAND_REGION_0},
	{"HN", A_BAND_REGION_0, G_BAND_REGION_1},
	{"HK", A_BAND_REGION_0, G_BAND_REGION_1},
	{"HU", A_BAND_REGION_1, G_BAND_REGION_1},
	{"IS", A_BAND_REGION_1, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"IN", A_BAND_REGION_2, G_BAND_REGION_1},
#else
	{"IN", A_BAND_REGION_0, G_BAND_REGION_1},
#endif
	{"ID", A_BAND_REGION_4, G_BAND_REGION_1},
	{"IR", A_BAND_REGION_4, G_BAND_REGION_1},
	{"IE", A_BAND_REGION_1, G_BAND_REGION_1},
	{"IL", A_BAND_REGION_0, G_BAND_REGION_1},
	{"IT", A_BAND_REGION_1, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"JP", A_BAND_REGION_1, G_BAND_REGION_1},
#else
	{"JP", A_BAND_REGION_9, G_BAND_REGION_1},
#endif
	{"JO", A_BAND_REGION_0, G_BAND_REGION_1},
	{"KZ", A_BAND_REGION_0, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"KP", A_BAND_REGION_1, G_BAND_REGION_1},
	{"KR", A_BAND_REGION_1, G_BAND_REGION_1},
#else
	{"KP", A_BAND_REGION_5, G_BAND_REGION_1},
	{"KR", A_BAND_REGION_5, G_BAND_REGION_1},
#endif
	{"KW", A_BAND_REGION_0, G_BAND_REGION_1},
	{"LV", A_BAND_REGION_1, G_BAND_REGION_1},
	{"LB", A_BAND_REGION_0, G_BAND_REGION_1},
	{"LI", A_BAND_REGION_1, G_BAND_REGION_1},
	{"LT", A_BAND_REGION_1, G_BAND_REGION_1},
	{"LU", A_BAND_REGION_1, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"MO", A_BAND_REGION_4, G_BAND_REGION_1},
#else
	{"MO", A_BAND_REGION_0, G_BAND_REGION_1},
#endif
	{"MK", A_BAND_REGION_0, G_BAND_REGION_1},
	{"MY", A_BAND_REGION_0, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"MX", A_BAND_REGION_2, G_BAND_REGION_0},
	{"MC", A_BAND_REGION_1, G_BAND_REGION_1},
#else
	{"MX", A_BAND_REGION_0, G_BAND_REGION_0},
	{"MC", A_BAND_REGION_2, G_BAND_REGION_1},
#endif
	{"MA", A_BAND_REGION_0, G_BAND_REGION_1},
	{"NL", A_BAND_REGION_1, G_BAND_REGION_1},
	{"NZ", A_BAND_REGION_0, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"NO", A_BAND_REGION_1, G_BAND_REGION_0},
#else
	{"NO", A_BAND_REGION_0, G_BAND_REGION_0},
#endif
	{"OM", A_BAND_REGION_0, G_BAND_REGION_1},
	{"PK", A_BAND_REGION_0, G_BAND_REGION_1},
	{"PA", A_BAND_REGION_0, G_BAND_REGION_0},
	{"PE", A_BAND_REGION_4, G_BAND_REGION_1},
	{"PH", A_BAND_REGION_4, G_BAND_REGION_1},
	{"PL", A_BAND_REGION_1, G_BAND_REGION_1},
	{"PT", A_BAND_REGION_1, G_BAND_REGION_1},
	{"PR", A_BAND_REGION_0, G_BAND_REGION_0},
	{"QA", A_BAND_REGION_0, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"RO", A_BAND_REGION_1, G_BAND_REGION_1},
	{"RU", A_BAND_REGION_6, G_BAND_REGION_1},
#else
	{"RO", A_BAND_REGION_0, G_BAND_REGION_1},
	{"RU", A_BAND_REGION_0, G_BAND_REGION_1},
#endif
	{"SA", A_BAND_REGION_0, G_BAND_REGION_1},
	{"SG", A_BAND_REGION_0, G_BAND_REGION_1},
	{"SK", A_BAND_REGION_1, G_BAND_REGION_1},
	{"SI", A_BAND_REGION_1, G_BAND_REGION_1},
	{"ZA", A_BAND_REGION_1, G_BAND_REGION_1},
	{"ES", A_BAND_REGION_1, G_BAND_REGION_1},
	{"SE", A_BAND_REGION_1, G_BAND_REGION_1},
	{"CH", A_BAND_REGION_1, G_BAND_REGION_1},
	{"SY", A_BAND_REGION_0, G_BAND_REGION_1},
	{"TW", A_BAND_REGION_3, G_BAND_REGION_0},
	{"TH", A_BAND_REGION_0, G_BAND_REGION_1},
#ifdef RTCONFIG_LOCALE2012
	{"TT", A_BAND_REGION_1, G_BAND_REGION_1},
	{"TN", A_BAND_REGION_1, G_BAND_REGION_1},
	{"TR", A_BAND_REGION_1, G_BAND_REGION_1},
	{"UA", A_BAND_REGION_9, G_BAND_REGION_1},
#else
	{"TT", A_BAND_REGION_2, G_BAND_REGION_1},
	{"TN", A_BAND_REGION_2, G_BAND_REGION_1},
	{"TR", A_BAND_REGION_2, G_BAND_REGION_1},
	{"UA", A_BAND_REGION_0, G_BAND_REGION_1},
#endif
	{"AE", A_BAND_REGION_0, G_BAND_REGION_1},
	{"GB", A_BAND_REGION_1, G_BAND_REGION_1},
	{"US", A_BAND_REGION_0, G_BAND_REGION_0},
#ifdef RTCONFIG_LOCALE2012
	{"UY", A_BAND_REGION_0, G_BAND_REGION_1},
#else
	{"UY", A_BAND_REGION_5, G_BAND_REGION_1},
#endif
	{"UZ", A_BAND_REGION_1, G_BAND_REGION_0},
#ifdef RTCONFIG_LOCALE2012
	{"VE", A_BAND_REGION_4, G_BAND_REGION_1},
#else
	{"VE", A_BAND_REGION_5, G_BAND_REGION_1},
#endif
	{"VN", A_BAND_REGION_0, G_BAND_REGION_1},
	{"YE", A_BAND_REGION_0, G_BAND_REGION_1},
	{"ZW", A_BAND_REGION_0, G_BAND_REGION_1},
	{"", 0, 0}
};

#define NUM_OF_COUNTRIES	(sizeof(allCountry)/sizeof(COUNTRY_CODE_TO_COUNTRY_REGION))

int get_channel_list_via_country(int unit, const char *country_code,
				 char *buffer, int len)
{
	unsigned char *pChannelListTemp = NULL;
	int index, num, i;
	char *p = buffer;
	int band = unit;

	if (buffer == NULL || len <= 0)
		return -1;

	memset(buffer, 0, len);
	if (band != 0 && band != 1)
		return -1;

	for (index = 0; index < NUM_OF_COUNTRIES; index++) {
		if (strncmp((char *)allCountry[index].IsoName, country_code, 2)
		    == 0)
			break;
	}

	if (index >= NUM_OF_COUNTRIES)
		return 0;

	if (band == 1)
		switch (allCountry[index].RegDomainNum11A) {
		case A_BAND_REGION_0:
			num =
			    sizeof(A_BAND_REGION_0_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_0_CHANNEL_LIST;
			break;
		case A_BAND_REGION_1:
			num =
			    sizeof(A_BAND_REGION_1_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_1_CHANNEL_LIST;
			break;
		case A_BAND_REGION_2:
			num =
			    sizeof(A_BAND_REGION_2_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_2_CHANNEL_LIST;
			break;
		case A_BAND_REGION_3:
			num =
			    sizeof(A_BAND_REGION_3_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_3_CHANNEL_LIST;
			break;
		case A_BAND_REGION_4:
			num =
			    sizeof(A_BAND_REGION_4_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_4_CHANNEL_LIST;
			break;
		case A_BAND_REGION_5:
			num =
			    sizeof(A_BAND_REGION_5_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_5_CHANNEL_LIST;
			break;
		case A_BAND_REGION_6:
			num =
			    sizeof(A_BAND_REGION_6_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_6_CHANNEL_LIST;
			break;
		case A_BAND_REGION_7:
			num =
			    sizeof(A_BAND_REGION_7_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_7_CHANNEL_LIST;
			break;
		case A_BAND_REGION_8:
			num =
			    sizeof(A_BAND_REGION_8_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_8_CHANNEL_LIST;
			break;
		case A_BAND_REGION_9:
			num =
			    sizeof(A_BAND_REGION_9_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_9_CHANNEL_LIST;
			break;
		case A_BAND_REGION_10:
			num =
			    sizeof(A_BAND_REGION_10_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_10_CHANNEL_LIST;
			break;
		case A_BAND_REGION_11:
			num =
			    sizeof(A_BAND_REGION_11_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_11_CHANNEL_LIST;
			break;
		case A_BAND_REGION_12:
			num =
			    sizeof(A_BAND_REGION_12_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_12_CHANNEL_LIST;
			break;
		case A_BAND_REGION_13:
			num =
			    sizeof(A_BAND_REGION_13_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_13_CHANNEL_LIST;
			break;
		case A_BAND_REGION_14:
			num =
			    sizeof(A_BAND_REGION_14_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_14_CHANNEL_LIST;
			break;
		case A_BAND_REGION_15:
			num =
			    sizeof(A_BAND_REGION_15_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_15_CHANNEL_LIST;
			break;
		case A_BAND_REGION_16:
			num =
			    sizeof(A_BAND_REGION_16_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_16_CHANNEL_LIST;
			break;
		case A_BAND_REGION_17:
			num =
			    sizeof(A_BAND_REGION_17_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_17_CHANNEL_LIST;
			break;
		case A_BAND_REGION_18:
			num =
			    sizeof(A_BAND_REGION_18_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_18_CHANNEL_LIST;
			break;
		case A_BAND_REGION_19:
			num =
			    sizeof(A_BAND_REGION_19_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_19_CHANNEL_LIST;
			break;
		case A_BAND_REGION_20:
			num =
			    sizeof(A_BAND_REGION_20_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_20_CHANNEL_LIST;
			break;
		case A_BAND_REGION_21:
			num =
			    sizeof(A_BAND_REGION_21_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = A_BAND_REGION_21_CHANNEL_LIST;
			break;
		default:	// Error. should never happen
			dbg("countryregionA=%d not support",
			    allCountry[index].RegDomainNum11A);
			break;
	} else if (band == 0)
		switch (allCountry[index].RegDomainNum11G) {
		case G_BAND_REGION_0:
			num =
			    sizeof(G_BAND_REGION_0_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = G_BAND_REGION_0_CHANNEL_LIST;
			break;
		case G_BAND_REGION_1:
			num =
			    sizeof(G_BAND_REGION_1_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = G_BAND_REGION_1_CHANNEL_LIST;
			break;
		case G_BAND_REGION_5:
			num =
			    sizeof(G_BAND_REGION_5_CHANNEL_LIST) /
			    sizeof(unsigned char);
			pChannelListTemp = G_BAND_REGION_5_CHANNEL_LIST;
			break;
		default:	// Error. should never happen
			dbg("countryregionG=%d not support",
			    allCountry[index].RegDomainNum11G);
			break;
		}

	if (pChannelListTemp != NULL) {
		for (i = 0; i < num; i++) {
#if 0
			if (i == 0)
				p += sprintf(p, "\"%d\"", pChannelListTemp[i]);
			else
				p += sprintf(p, ", \"%d\"",
					     pChannelListTemp[i]);
#else
			if (i == 0)
				p += sprintf(p, "%d", pChannelListTemp[i]);
			else
				p += sprintf(p, ",%d", pChannelListTemp[i]);
#endif
		}
	}

	return (p - buffer);
}

#ifdef RTCONFIG_BONDING_WAN
/** Return speed of a bonding interface.
 * @bond_if:	name of bonding interface. LAN bond_if = bond0; WAN bond_if = bond1.
 * @return:
 *  <= 0	error
 *  otherwise	link speed
 */
int get_bonding_speed(char *bond_if)
{
	int speed;
	char confbuf[sizeof(SYS_CLASS_NET) + IFNAMSIZ + sizeof("/speedXXXXXX")];
	char buf[32] = { 0 };

	snprintf(confbuf, sizeof(confbuf), SYS_CLASS_NET "/%s/speed", bond_if);
	if (f_read_string(confbuf, buf, sizeof(buf)) <= 0)
		return 0;

	speed = safe_atoi(buf);
	if (speed <= 0)
		speed = 0;

	return speed;
}

/** Return link speed of a bonding slave port if it's connected or 0 if it's disconnected.
 * @port:	0: WAN, 1~8: LAN1~8, 30: 10G base-T (RJ-45), 31: 10G SFP+
 * @return:
 *  <= 0:	disconnected
 *  otherwise:	link speed
 */
int get_bonding_port_status(int port)
{
	int ret = 0;

	if (__get_bonding_port_status)
		ret = __get_bonding_port_status(port);

	return ret;
}
#endif /* RTCONFIG_BONDING_WAN */


/* Return nvram variable name, e.g. et1macaddr, which is used to repented as LAN MAC.
 * @return:
 */
char *get_lan_mac_name(void)
{
	char *mac_name = "et0macaddr";
	return mac_name;
}

/* Return nvram variable name, e.g. et0macaddr, which is used to repented as WAN MAC.
 * @return:
 */
char *get_wan_mac_name(void)
{
	int model = get_model();
	char *mac_name = "et1macaddr";
	char *hwaddr = nvram_safe_get("wan_hwaddr_x");
	unsigned char mac_binary[6], mac2_binary[6];

	/* Check below configuration in convert_wan_nvram() too. */
	switch (model) {
	case MODEL_BLUECAVE:	/* fall-through */
	case MODEL_RAX40:	/* fall-through */
		/* Use 2G MAC address as LAN MAC address. */
		memset(mac2_binary, 0xff, sizeof(mac2_binary));
		if (!ether_atoe(hwaddr, mac_binary) || !memcmp(mac_binary, mac2_binary, 6))
			mac_name = "et1macaddr";
		else
			mac_name = "wan_hwaddr_x";
		break;
	default:
		dbg("%s: Define WAN MAC address for model %d\n", __func__, model);
		mac_name = "et0macaddr";
		break;
	};

	return mac_name;
}

char *get_2g_hwaddr(void)
{
	return nvram_safe_get(get_lan_mac_name());
}

char *get_5g_hwaddr(void)
{
	return nvram_safe_get(get_wan_mac_name());
}


char *get_label_mac()
{
	return get_2g_hwaddr();
}

char *get_lan_hwaddr(void)
{
	return nvram_safe_get(get_lan_mac_name());
}

char *get_wan_hwaddr(void)
{
	return nvram_safe_get(get_wan_mac_name());
}

/**
 * Generate interface name based on @band and @subunit. (@subunit is NOT y in wlX.Y)
 * @band:
 * @subunit:
 * @buf:
 * @return:
 */
char *__get_wlifname(int band, int subunit, char *buf)
{
	if (!buf)
		return buf;

	if (!subunit)
		strcpy(buf, get_wififname(band));
	else
		sprintf(buf, "%s0%d", get_wififname(band), subunit);

	return buf;
}

/**
 * Check wlX.Y_bss_enabled nvram variable and generate interface name based on
 * wlX.Y_bss_enabled, @band,  and @subunit. (@subunit is NOT y in wlX.Y)
 * @unit:
 * @subunit:
 * @subunit_x:
 * @buf:
 * @return:
 */
char *get_wlifname(int unit, int subunit, int subunit_x, char *buf)
{
#if 1
	char wifbuf[32];
	char prefix[] = "wlXXXXXX_";
	int wlc_band __attribute__((unused)) = nvram_get_int("wlc_band");
#if defined(RTCONFIG_WIRELESSREPEATER)
	if (sw_mode() == SW_MODE_REPEATER && wlc_band == unit && subunit == 1) {
		strcpy(buf, get_staifname(unit));
		return buf;
	} else
#endif /* RTCONFIG_WIRELESSREPEATER */
#if defined(RTCONFIG_AMAS)
	if (sw_mode() == SW_MODE_AP && nvram_match("re_mode", "1")) {
		/*
		 * wlX.0_bss_enabled: the interface to Upper.
		 * wlX.1_bss_enabled: the interface to Lowwer (main)
		 * wlX.2_bss_enabled: the interface for guest networks 1
		 */
		if (subunit <= 1) {
			strcpy(buf, "");
			return buf;
		}
	}
#endif	/* RTCONFIG_AMAS */
	{
		__get_wlifname(unit, 0, wifbuf);
		snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);
		if (nvram_pf_match(prefix, "bss_enabled", "1"))
			sprintf(buf, "%s0%d", wifbuf, subunit_x);
		else
			sprintf(buf, "%s", "");
	}
	return buf;
#else
	return __get_wlifname(unit, subunit, buf);
#endif
}

/**
 * Generate VAP interface name of wlX.Y for Guest network, Free Wi-Fi, and Facebook Wi-Fi
 * @x:		X of wlX.Y, aka unit
 * @y:		Y of wlX.Y
 * @buf:	Pointer to buffer of VAP interface name. Must greater than or equal to IFNAMSIZ
 * @return:
 * 	NULL	Invalid @buf
 * 	""	Invalid parameters
 *  otherwise	VAP interface name of wlX.Y
 */
char *get_wlxy_ifname(int x, int y, char *buf)
{
	int i, sidx;
	char prefix[sizeof("wlX.Yxxx")];

	if (!buf)
		return buf;

	if (x < 0 || y < 0 || y >= MAX_NO_MSSID)
		return "";

	if (y == 0) {
		__get_wlifname(x, 0, buf);
		return buf;
	}

	*buf = '\0';
	for (i = 1, sidx = 1; i < MAX_NO_MSSID; ++i) {
		if (i == y) {
			__get_wlifname(x, sidx, buf);
			break;
		}

		snprintf(prefix, sizeof(prefix), "wl%d.%d_", x, i);
		if (nvram_pf_match(prefix, "bss_enabled", "1"))
			sidx++;
	}

	return buf;
}

char *get_wififname(int band)
{
	const char *wif[] = { WIF_2G, WIF_5G, WIF_5G2 };
	if (band < 0 || band >= ARRAY_SIZE(wif)) {
		dbg("%s: Invalid wl%d band!\n", __func__, band);
		band = 0;
	}
	return (char*) wif[band];
}

char *get_staifname(int band)
{
	const char *sta[] = { STA_2G, STA_5G, STA_5G2 };
	if (band < 0 || band >= ARRAY_SIZE(sta)) {
		dbg("%s: Invalid wl%d band!\n", __func__, band);
		band = 0;
	}
	return (char*) sta[band];
}

char *get_vphyifname(int band)
{
	const char *vphy[] = { VPHY_2G, VPHY_5G, VPHY_5G2 };
	if (band < 0 || band >= ARRAY_SIZE(vphy)) {
		dbg("%s: Invalid wl%d band!\n", __func__, band);
		band = 0;
	}
	return (char *) vphy[band];
}


char *get_realphyifname(int band)
{
	const char *phy[] = { PHY_2G, PHY_5G, PHY_5G2 };
	if (band < 0 || band >= ARRAY_SIZE(phy)) {
		dbg("%s: Invalid wl%d band!\n", __func__, band);
		band = 0;
	}
	return (char *) phy[band];
}


/**
 * Return band if @ifname is STA interface name and the band is supported.
 * @return:
 * 	-1:	@ifname is not STA interface name
 * 	band:	@ifname is STA interface.
 *  otherwise:	not defined.
 */
int get_sta_ifname_unit(const char *ifname)
{
	int band;
	const char *sta[] = { STA_2G, STA_5G, STA_5G2 };

	if (!ifname)
		return -1;
	for (band = 0; band < min(MAX_NR_WL_IF, ARRAY_SIZE(sta)); ++band) {
		SKIP_ABSENT_BAND(band);

		if (!strncmp(ifname, sta[band], strlen(sta[band])))
			return band;
	}
	return -1;
}

/**
 * Return true if @ifname is main/guest VAP interface name and the band is supported.
 * @return:
 * 	0:	@ifname is not VAP interface name
 * 	1:	@ifname is VAP interface name.
 *  otherwise:	not defined.
 */
int is_vap_ifname(const char *ifname)
{
	int band;
	const char *wif[] = { WIF_2G, WIF_5G, WIF_5G2 };

	if (!ifname)
		return 0;
	for (band = 0; band < min(MAX_NR_WL_IF, ARRAY_SIZE(wif)); ++band) {
		SKIP_ABSENT_BAND(band);

		if (!strncmp(ifname, wif[band], strlen(wif[band])))
			return 1;
	}
	return 0;
}

/**
 * Return true if @ifname is STA interface name and the band is supported.
 * @return:
 * 	0:	@ifname is not STA interface name
 * 	1:	@ifname is STA interface name.
 *  otherwise:	not defined.
 */
int is_sta_ifname(const char *ifname)
{
	int band;
	const char *sta[] = { STA_2G, STA_5G, STA_5G2 };

	if (!ifname)
		return 0;
	for (band = 0; band < min(MAX_NR_WL_IF, ARRAY_SIZE(sta)); ++band) {
		SKIP_ABSENT_BAND(band);

		if (!strncmp(ifname, sta[band], strlen(sta[band])))
			return 1;
	}
	return 0;
}

/**
 * Return true if @ifname is main/guest VPHY interface name and the band is supported.
 * @return:
 * 	0:	@ifname is not VPHY interface name
 * 	1:	@ifname is VPHY interface name.
 *  otherwise:	not defined.
 */
int is_vphy_ifname(const char *ifname)
{
	int band;
	const char *vphy[] = { VPHY_2G, VPHY_5G, VPHY_5G2 };

	if (!ifname)
		return 0;
	for (band = 0; band < min(MAX_NR_WL_IF, ARRAY_SIZE(vphy)); ++band) {
		SKIP_ABSENT_BAND(band);

		if (!strcmp(ifname, vphy[band]))
			return 1;
	}
	return 0;
}

/**
 * Input @band and @ifname and return Y of wlX.Y.
 * Last digit of VAP interface name of guest is NOT always equal to Y of wlX.Y,
 * if guest network is not enabled continuously.
 * @band:
 * @ifname:	ath0, ath1, ath001, ath002, ath103, etc
 * @return:	index of guest network configuration. (wlX.Y: X = @band, Y = @return)
 * 		If both main 2G/5G, 1st/3rd 2G guest network, and 2-nd 5G guest network are enabled,
 * 		return value should as below:
 * 		ath0:	0
 * 		ath001:	1
 * 		ath002: 3
 * 		ath1:	0
 * 		ath101: 2
 */
int get_wlsubnet(int band, const char *ifname)
{
	int subnet, sidx;
	char buf[32];

	for (subnet = 0, sidx = 0; subnet < MAX_NO_MSSID; subnet++)
	{
#ifdef RTCONFIG_AMAS
		if (sw_mode() == SW_MODE_AP && nvram_match("re_mode", "1")) {
			if(subnet == 1)
				subnet++;
		}
#endif	/* RTCONFIG_AMAS */
		if(!nvram_match(wl_nvname("bss_enabled", band, subnet), "1")) {
			if (!subnet)
				sidx++;
			continue;
		}

		if(strcmp(ifname, __get_wlifname(band, sidx, buf)) == 0)
			return subnet;

		sidx++;
	}
	return -1;
}

int get_wlif_unit(const char *wlifname, int *unit, int *subunit)
{
	int i;
	int _unit = -1, _subunit = -1;
	char *wlif;
	int cmp;
	int len;

	for (i = WL_2G_BAND; i < MAX_NR_WL_IF; ++i) {
		SKIP_ABSENT_BAND(i);

		wlif = get_wififname(i);
		len = strlen(wlif);
		cmp = strncmp(wlifname, wlif, len);
		if(cmp)		/* wlifname is less than wlif */
			continue;
		_unit = i;
		break;
	}
	if (_unit < 0 || absent_band(_unit))
		return -1;

	if(subunit == NULL || strlen(wlifname) == len) {
		_subunit = 0;
	}
	else {
		_subunit = get_wlsubnet(_unit, wlifname);
		if (_subunit < 0)
			_subunit = 0;
	}

	if(unit)
		*unit = _unit;
	if(subunit)
		*subunit = _subunit;

	return 0;
}

#define PROC_NET_WIRELESS  "/proc/net/wireless"

int get_wl_status_ioctl(const char *ifname, int *status, int *quality, int *signal, int *noise, unsigned int *update)
{
	iwstats stats;
	struct iwreq wrq;

	if(ifname == NULL || ifname[0] == '\0' || status == NULL)
		return -1;

	*status = 0;
	if(quality)
		*quality = 0;
	if(signal)
		*signal = 0;
	if(noise)
		*noise = 0;
	if(update)
		*update = 0;

	memset(&stats, 0, sizeof(stats));
	wrq.u.data.pointer = (caddr_t) &stats;
	wrq.u.data.length = sizeof(struct iw_statistics);
	wrq.u.data.flags = 1;					/* Clear updated flag */
	if(wl_ioctl(ifname, SIOCGIWSTATS, &wrq) < 0)
		return -2;

	*status = stats.status;
	if(quality && !(stats.qual.updated & IW_QUAL_QUAL_INVALID)) {
			*quality = stats.qual.qual;
		if(update)
			*update |= (stats.qual.updated & 1);
	}

	if(signal && !(stats.qual.updated & IW_QUAL_LEVEL_INVALID)) {
		if(stats.qual.updated & IW_QUAL_RCPI) {
			double    rcpivalue = (stats.qual.level / 2.0) - 110.0;
			*signal = (int) rcpivalue;
		}
		else {
			*signal = stats.qual.level;
			if(*signal >= 64)
				*signal -= 0x100;	/* convert from unsigned to signed */
		}
		if(*signal >= 0)
			*signal = -1;

		if(update)
			*update |= (stats.qual.updated & 2);
	}

	if(noise && !(stats.qual.updated & IW_QUAL_NOISE_INVALID)) {
		if(stats.qual.updated & IW_QUAL_RCPI) {
			double    rcpivalue = (stats.qual.level / 2.0) - 110.0;
			*noise = (int) rcpivalue;
		}
		else {
			*noise = stats.qual.noise;
			if(*noise >= 64)
				*noise -= 0x100;	/* convert from unsigned to signed */
		}
		if(update)
			*update |= (stats.qual.updated & 4);
	}

	return *status;
}

int get_wl_status_proc(const char *ifname, int *status, int *quality, int *signal, int *noise, unsigned int *update)
{
	char line[512];
	FILE *fp;
	int size;
	char *p1, *p2;
	int value;
	int len;

	if(ifname == NULL || ifname[0] == '\0' || status == NULL)
		return -1;

	*status = 0;

	if((fp = fopen(PROC_NET_WIRELESS, "r")) == NULL)
		return -2;

	len = strlen(ifname);
	size = sizeof(line)-1;
	line[size] = '\0';
	if(update)
		*update = 0;

	while(fgets(line, size, fp)) {
		p1 = line;

		strip_new_line(p1);
		skip_space(p1);

		if(strncmp(p1, ifname, len) != 0 || p1[len] != ':')
			continue;

		p1 = p1 + len + 1;		// point to the next char of ':'
		skip_space(p1);
		*status = strtoul(p1, &p2, 16);

		/* Quality */
		p1 = p2;
		skip_space(p1);
		value = strtoul(p1, &p2, 0);
		if(quality) {
			*quality = value;
			if(update && *p2 == '.') {
				p2++;
				*update |= 1;
			}
		}

		/* Signal level */
		p1 = p2;
		skip_space(p1);
		value = strtoul(p1, &p2, 0);
		if(signal) {
			*signal = value;
			if(*signal >= 0)
				*signal = -1;
			if(update && *p2 == '.') {
				p2++;
				*update |= 2;
			}
		}

		/* Noise level */
		p1 = p2;
		skip_space(p1);
		value = strtoul(p1, &p2, 0);
		if(noise) {
			*noise = value;
			if(update && *p2 == '.') {
				p2++;
				*update |= 4;
			}
		}

		break;
	}
	fclose(fp);
	return *status;
}

int get_wl_status(const char *ifname, int *status, int *quality, int *signal, int *noise, unsigned int *update)
{
	int ret;
	pid_t pid;
	if((ret = get_wl_status_ioctl(ifname, status, quality, signal, noise, update)) >= 0)
		return ret;

	pid = getpid();
	cprintf("get_wl_status_ioctl(%s) pid(%s) failed ret(%d)\n", ifname, get_process_name_by_pid(pid), ret);
	if((ret = get_wl_status_proc(ifname, status, quality, signal, noise, update)) >= 0)
		return ret;

	cprintf("get_wl_status_proc(%s) pid(%s) failed ret(%d)\n", ifname, get_process_name_by_pid(pid), ret);
	return ret;
}

int get_ap_mac(const char *ifname, struct iwreq *pwrq)
{
	return wl_ioctl(ifname, SIOCGIWAP, pwrq);
}

const unsigned char ether_zero[6]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char ether_bcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

int chk_assoc(const char *ifname)
{
	struct iwreq wrq;
	int ret;

	if((ret = get_ap_mac(ifname, &wrq)) < 0)
		return ret;

#if 0
cprintf("## %s(): ret(%d) ap_addr(%02x:%02x:%02x:%02x:%02x:%02x)\n", __func__, ret
, wrq.u.ap_addr.sa_data[0], wrq.u.ap_addr.sa_data[1], wrq.u.ap_addr.sa_data[2]
, wrq.u.ap_addr.sa_data[3], wrq.u.ap_addr.sa_data[4], wrq.u.ap_addr.sa_data[5]);
#endif
	if(memcmp(&(wrq.u.ap_addr.sa_data), ether_zero, 6) == 0)
		return 0;	// Not-Associated
	else if(memcmp(&(wrq.u.ap_addr.sa_data), ether_bcast, 6) == 0)
		return -1;	// Invalid

	return 1;
}


#if defined(RTCONFIG_BCN_RPT)
void save_wlxy_mac(char *mode, char* ifname)
{
	char cmdbuf[20],buf[1024];
 	FILE *fp;
        int len;
        char *pt1,*pt2;
	int x,y;
	char prefix[sizeof("wlXXXXXXXXXXXXX_")];
	x=-1;y=-1;
	if(!strcmp(mode,"ap"))
	{
		get_wlif_unit(ifname,&x,&y);
		if(x!=-1 && y>0)
		{
			snprintf(prefix, sizeof(prefix), "wl%d.%d_hwaddr", x,y);
			snprintf(cmdbuf, sizeof(cmdbuf), "ifconfig %s", ifname);
			fp = popen(cmdbuf, "r");
			pt1=NULL;
			pt2=NULL;
        		if (fp) {
                		memset(buf, 0, sizeof(buf));
                		len = fread(buf, 1, sizeof(buf), fp);
                		pclose(fp);
                		if (len > 1) {
                        		buf[len-1] = '\0';
                        		pt1 = strstr(buf, "HWaddr ");
                        		if (pt1)
                        		{
                                		pt2 = pt1 + strlen("HWaddr ");
                                		*(pt2+17)='\0';
					}
				}	
                        }
			if(pt2 && strlen(pt2)==17)
			{	
				_dprintf("%s=%s\n",prefix,pt2);
				nvram_set(prefix,pt2);
			}	
		}
	}			
}
#endif	

/**
 * Get PHY name of a cfg80211 based VAP interface.
 * @unit:	wl_unit
 * @iwphy:	buffer that is used to store PHY name.
 * @size:	sizeof @iwphy
 * @return:
 * 	0:	success
 *     -1:	invalid parameter
 *  otherwise:	error
 */
int get_iwphy_name(int unit, char *iwphy, size_t size)
{
	if (unit < 0 || unit >= MAX_NR_WL_IF || !iwphy || !size)
		return -1;

	strlcpy(iwphy, get_realphyifname(unit), size);
	return 0;
}

/**
 * Create a VAP interface onto specified VPHY unit.
 * @ifname:	VAP interface name.
 * @unit:	VPHY unit number.
 * @mode:	VAP mode. "ap", or "sta".
 * @return:
 * 	0:	success
 *     -1:	invalid parameter.
 *  otherwise:	error
 */
int create_vap(char *ifname, int unit, char *mode)
{
	char iwmode[sizeof("managedXXXXX")] = { 0 };
	char iwphy[IFNAMSIZ] = { 0 };
	char *iwargv[] = { "iw", "phy", iwphy, "interface", "add", ifname, "type", iwmode, NULL };

	if (!ifname || !mode || unit < 0 || unit >= MAX_NR_WL_IF)
		return -1;

	if (!strcmp(mode, "ap")) {
		strlcpy(iwmode, "__ap", sizeof(iwmode));
	} else if (!strcmp(mode, "sta")) {
		strlcpy(iwmode, "managed", sizeof(iwmode));
	} else
		return -1;

	get_iwphy_name(unit, iwphy, sizeof(iwphy));
	dbG("\ncreate a wifi node %s from %s\n", ifname, iwphy);
	_eval(iwargv, NULL, 0, NULL);

	if (!strcmp(mode, "sta"))
		eval("iw", "dev", ifname, "set", "4addr", "on");

#if defined(RTCONFIG_BCN_RPT)
	save_wlxy_mac(mode,ifname);
#endif	

	return 0;
}

/**
 * Destroy a VAP interface.
 * @ifname:	VAP interface name.
 * @return:
 * 	0:	success
 *     -1:	invalid parameter
 */
int destroy_vap(char *ifname)
{
	if (!ifname)
		return -1;

	eval("iw", ifname, "del");
	return 0;
}

#ifndef pow
#define pow(v,e) ({double d=1; int i; for(i=0;i<e;i++) d*=v; d;})
#endif

int iwfreq_to_ch(const iwfreq *fr)
{
	double frd;
	int freq;

	frd = ((double) fr->m) * pow(10,fr->e);
	if(frd < 1e3)
		return (int)frd;
	else if(frd < 1e9)
		return -2;

	freq = (int)(frd / 1e6);
	return (int)ieee80211_mhz2ieee((u_int)freq);
}

int shared_get_channel(const char *ifname)
{
	struct iwreq wrq;
	const iwfreq *fr;

	if(ifname == NULL)
		return -1;

	if(wl_ioctl(ifname, SIOCGIWFREQ, &wrq))
		return -1;

	fr = &(wrq.u.freq);
	return iwfreq_to_ch(fr);
}

unsigned long long get_bitrate(const char *ifname)
{
	struct iwreq wrq;
	double ratio = 1;

#if defined(RTCONFIG_QCA_BIGRATE_WIFI)
	ratio = 1000;
#endif

	if (ifname == NULL)
		return -1;

	if (wl_ioctl(ifname, SIOCGIWRATE, &wrq))
		return -1;

	return ratio * wrq.u.bitrate.value;
}

/*
 * get_channel_list(ifname, ch_list[], size)
 *
 * get channel list from wifi driver via wl_ioctl(SIOCGIWRANGE)
 * and store the channel list to ch_list[]
 *
 * ch_list[]: an array to save the list of channel that currently used by ifname interface.
 *
 * size: the number of the ch_list[] that can use.
 *
 * return value: a native value for error OR the count of channel in the ch_list[].
 *
 */
int get_channel_list(const char *ifname, int ch_list[], int size)
{
	struct iwreq wrq;
	struct iw_range *range;
	unsigned char buffer[sizeof(iwrange) * 2];	/* Large enough */
	int i;

	if(wl_ioctl(ifname, SIOCGIWNAME, &wrq))		/* check wireless extensions. */
		return -1;

	memset(buffer, 0, sizeof(buffer));
	wrq.u.data.pointer = (caddr_t) buffer;
	wrq.u.data.length = sizeof(buffer);
	wrq.u.data.flags = 0;

	if(wl_ioctl(ifname, SIOCGIWRANGE, &wrq)) {
		cprintf("NOT SUPPORT SIOCGIWRANGE in %s\n", ifname);
		return -1;
	}

	range = (struct iw_range *) buffer;
	if(wrq.u.data.length < 300 || range->we_version_compiled <= 15) {
		cprintf("Wireless Extensions is TOO OLD length(%d) ver(%d)\n", wrq.u.data.length, range->we_version_compiled);
		return -2;
	}

	if (range->num_frequency < size)
		size = range->num_frequency;

	for(i = 0; i < size; i++) {
		//_dprintf("# freq[%2d].i(%d) ch(%d)\n", i, range->freq[i].i, iwfreq_to_ch(&(range->freq[i])));
		ch_list[i] = range->freq[i].i;
	}
	return size;
}

uint64_t get_channel_list_mask(enum wl_band_id band)
{
	uint64_t m = 0;
	int i, ch_list[64] = { 0 };
	char vap[IFNAMSIZ];

	if (band < 0 || band >= WL_NR_BANDS)
		return 0;

	strlcpy(vap, get_wififname(band), sizeof(vap));
	get_channel_list(vap, ch_list, ARRAY_SIZE(ch_list));
	for (i = 0; i < ARRAY_SIZE(ch_list) && ch_list[i] != 0; ++i)
		m |= ch2bitmask(band, ch_list[i]);

	return m;
}

int has_dfs_channel(void)
{
	char word[8], *next;
	int has_dfs_channel = 0;
	int i;

	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		char buf[256], *p = buf;
		int ch_list[32];
		int ret;

		ret = get_channel_list(word, ch_list, ARRAY_SIZE(ch_list));
		for(i = 0; i < ret; i++) {
			if(is_dfs_channel(ch_list[i]))
				has_dfs_channel++;
			p += sprintf(p, "%s%d", i?",":"", ch_list[i]);
		}
		p = '\0';
//		_dprintf("# get_channel_list(%s) ret(%d) ch_list(%s) has_dfs_channel(%d)\n", word, ret, buf, has_dfs_channel);
	}
	if(has_dfs_channel)
		nvram_set_int("has_dfs_channel", has_dfs_channel);
	else
		nvram_unset("has_dfs_channel");
	return has_dfs_channel;
}

int get_radar_channel_list(const char *vphy, int radar_list[], int size)
{
	int i;
	int cnt, ret;
	int ch_list[32];

	if(vphy == NULL || radar_list == NULL || size < 0)
		return -1;

	if (!strcmp(vphy, VPHY_2G))
		return 0;

	ret = get_channel_list(vphy, ch_list, ARRAY_SIZE(ch_list));
	for(i = 0, cnt = 0; i < ret && cnt < size; i++) {
		if(is_dfs_channel(ch_list[i])){
			radar_list[cnt] = ch_list[i];
			cnt++;
		}
	}
	return cnt;
}

int check_wave_ready(int action)
{
	int time = 21;

	nvram_unset("check_wave_ready");
	while (!nvram_get_int("wave_ready"))
	{
		_dprintf("[%s][%d][%d] current action:[%d], waiting wave_ready=1\n", __func__, __LINE__, action, nvram_get_int("wave_action_cur"));
		--time;
		sleep(3);
		if (!time)
		{
			_dprintf("[%s][%d] wait wave_ready=1 over 60 seconds, please check wave_monitor status (current action:[%d]), skip [%d]\n", __func__, __LINE__, nvram_get_int("wave_action_cur"), action);
			nvram_set_int("check_wave_ready", -1);
			return -1;
		}
	}
	return 0;
}

int trigger_wave_monitor_and_wait(const char *func, int line, int action, int time)
{
	int result;

	result = check_wave_ready(action);
	if ( result != -1 )
	{
		_dprintf("[%s][%d] %s(%d) trigger action:[%d]\n", __func__, __LINE__, func, line, action);
		nvram_set_int("wave_action", action);
		kill_pidfile_s("/var/run/wave_monitor.pid", 16);
		if (time)
			sleep(time);
		result = check_wave_ready(action);
	}
	return result;
}

int trigger_wave_monitor(const char *func, int line, int action)
{
	return trigger_wave_monitor_and_wait(func, line, action, 5);
}

#if 0
#ifdef RTCONFIG_AMAS
void add_obd_probe_req_vsie(char *hexdata)
{
#define MAX_VSIE_LEN 1024
	// 13 : Associatino Request
	// 14 : Probe Reqeust
	// 15 : Authentication Request
	char cmd[300] = {0};
	int pktflag = 0xE;
	int len = 0;
	char *ifname = NULL;
	strlen(ifname);
	FILE *fp;

	len = 3 + strlen(hexdata)/2;	/* 3 is oui's len */

	ifname = get_wififname(0); // TODO: Should we get the band from nvram?

	snprintf(cmd, sizeof(cmd), "/tmp/wpa_cli vendor_elem_get -i%s %d",
		"wlan1", pktflag);
	if ((fp = popen(cmd, "r")) != NULL) {
		char *vendor_elem[MAX_VSIE_LEN];
		memset(vendor_elem, 0, sizeof(vendor_elem));

		if (fgets(vendor_elem , sizeof(vendor_elem) , fp) != NULL) {
			if (strlen(vendor_elem)) {
				snprintf(cmd, sizeof(cmd), "/tmp/wpa_cli vendor_elem_remove -i%s %d %s",
					"wlan1", pktflag, vendor_elem);
				_dprintf("%s: cmd=%s\n", __func__, cmd);
				system(cmd);
			}
		}
		pclose(fp);
	}

	//_dprintf("%s: wl0_ifname=%s\n", __func__, ifname);

	if (ifname && strlen(ifname)) {
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/tmp/wpa_cli vendor_elem_add -i%s %d DD%02X%02X%02X%02X%s",
			"wlan1", pktflag, (uint8_t)len, (uint8_t)OUI_ASUS[0],  (uint8_t)OUI_ASUS[1],  (uint8_t)OUI_ASUS[2], hexdata);
		_dprintf("%s: cmd=%s\n", __func__, cmd);
		system(cmd);
	}
}

void del_obd_probe_req_vsie(char *hexdata)
{
	char cmd[300] = {0};
	int pktflag = 0xE;
	int len = 0;
	char *ifname = NULL;

	len = 3 + strlen(hexdata)/2;	/* 3 is oui's len */

	ifname = get_wififname(0); // TODO: Should we get the band from nvram?

	//_dprintf("%s: wl0_ifname=%s\n", __func__, ifname);

	if (ifname && strlen(ifname)) {
		snprintf(cmd, sizeof(cmd), "/tmp/wpa_cli vendor_elem_remove -i%s %d DD%02X%02X%02X%02X%s",
			"wlan1", pktflag, (uint8_t)len,  (uint8_t)OUI_ASUS[0],  (uint8_t)OUI_ASUS[1],  (uint8_t)OUI_ASUS[2], hexdata);
		_dprintf("%s: cmd=%s\n", __func__, cmd);
		system(cmd);
	}
}
#endif /* RTCONFIG_AMAS */
#endif

char * get_wpa_ctrl_sk(int band, char ctrl_sk[], int size)
{
	char *sta;
	if(band < 0 || band >= MAX_NR_WL_IF || ctrl_sk == NULL)
		return NULL;
	sta = get_staifname(band);
	snprintf(ctrl_sk, size, "/var/run/wpa_supplicant-%s", sta);
	return ctrl_sk;
}

#define WPA_CLI_REPLY_SIZE		32
#define QUERY_WPA_CLI_REPLY_TIMEOUT	10
#define QUERY_WPA_STATE_TIMEOUT		25
char *wpa_cli_reply(const char *fcmd, char *reply)
{
	FILE *fp;
	int rlen;

	fp = popen(fcmd, "r");
	if (fp) {
		rlen = fread(reply, 1, WPA_CLI_REPLY_SIZE, fp);
		pclose(fp);
		if (rlen > 1) {
			reply[rlen-1] = '\0';
			return reply;
		}
	}

	return "";
}

void set_wpa_cli_cmd(int band, const char *cmd, int chk_reply)
{
	char ctrl_sk[32];
	char *sta;
	char fcmd[128];
	char reply[WPA_CLI_REPLY_SIZE];
	int timeout = QUERY_WPA_CLI_REPLY_TIMEOUT;
	int scan_and_with_scan_events = 0;

	if(band < 0 || band >= MAX_NR_WL_IF || cmd == NULL || cmd[0] == '\0')
		return;

	get_wpa_ctrl_sk(band, ctrl_sk, sizeof(ctrl_sk));
	sta = get_staifname(band);
	if (chk_reply) {
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		if (strcmp(cmd, "scan") == 0) { // check if scan_events is supported
			char *rpt;
			snprintf(fcmd, sizeof(fcmd), "/usr/bin/wpa_cli -p %s -i %s scan_events", ctrl_sk, sta);
			rpt = wpa_cli_reply(fcmd, reply);
			if ((strcmp(rpt, "YES")==0) || (strcmp(rpt, "NO")==0))
				scan_and_with_scan_events = 1;
		}
#endif
		if (scan_and_with_scan_events) {
			eval("/usr/bin/wpa_cli", "-p", ctrl_sk, "-i", sta, (char*) cmd); // just issue scan command & wait scan_events
			snprintf(fcmd, sizeof(fcmd), "/usr/bin/wpa_cli -p %s -i %s scan_events", ctrl_sk, sta);
			timeout = QUERY_WPA_STATE_TIMEOUT;
			while (strcmp(wpa_cli_reply(fcmd, reply), "YES") && timeout--) {
				//dbg("%s(%d): reply [%s] ...(%d/%d)\n", __func__, band, reply, timeout, QUERY_WPA_STATE_TIMEOUT);
				sleep(1);
			};
		} else { // non-scan cmd or no scan_events supported
			snprintf(fcmd, sizeof(fcmd), "/usr/bin/wpa_cli -p %s -i %s %s", ctrl_sk, sta, cmd);
			while (strcmp(wpa_cli_reply(fcmd, reply), "OK") && timeout--) {
				//dbg("%s(%d): reply [%s] ...(%d/%d)\n", __func__, band, reply, timeout, QUERY_WPA_CLI_REPLY_TIMEOUT);
				sleep(1);
			};

			if (strcmp(cmd, "scan") == 0) {
				snprintf(fcmd, sizeof(fcmd), "/usr/bin/wpa_cli -p %s -i %s status | grep wpa_state=", ctrl_sk, sta);
				timeout = QUERY_WPA_STATE_TIMEOUT;
				while (!strcmp(wpa_cli_reply(fcmd, reply), "wpa_state=SCANNING") && timeout--) {
					//dbg("%s(%d): reply [%s] ...(%d/%d)\n", __func__, band, reply, timeout, QUERY_WPA_STATE_TIMEOUT);
					sleep(1);
				};
			}
		}
	}
	else
		eval("/usr/bin/wpa_cli", "-p", ctrl_sk, "-i", sta, (char*) cmd);
}

void disassoc_sta(char *ifname, char *sta_addr)
{
	int found;
	char vap[IFNAMSIZ];

	if(ifname == NULL || *ifname == '\0' || sta_addr == NULL || *sta_addr == '\0')
		return;

	strlcpy(vap, ifname, sizeof(vap));
	found = find_vap_by_sta(sta_addr, vap);

	if (found) {
		eval("hostapd_cli", "-i", vap, "disassociate", sta_addr);
	}
}


int get_amas_eth_phy_status(int wan_unit)
{
	char *wan_ifnames;
	char tmp[100];
	char buf[32];
#if !defined(RTCONFIG_DUALWAN)
	if (wan_unit > 0)
	{
		_dprintf("There is only one wan port in BLUECAVE RE mode.\n");
		return 0;
	}
#endif
	wan_ifnames = nvram_safe_get("eth_ifnames");
	if (*wan_ifnames && strlen(wan_ifnames) < 5)
	{
		snprintf(tmp, sizeof(tmp), "/sys/class/net/%s/operstate", wan_ifnames);
		f_read_string(tmp, buf, sizeof(buf));
		if(!strncmp(buf, "up", 2))
			return 1;
		else
			return 0;
	}
	_dprintf("WAN port should be eth1 in BLUECAVE RE mode.\n");
	return 0;
}
