/*
 * Copyright 2021, ASUS
 * Copyright 2021-2022, SWRTdev
 * Copyright 2021-2022, paldier <paldier@hotmail.com>.
 * Copyright 2021-2022, lostlonger<lostlonger.g@gmail.com>.
 * All Rights Reserved.
 */

#include "rc.h"

#include <termios.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <time.h>
#include <errno.h>
#include <paths.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <wlutils.h>
#include <bcmdevs.h>
#include <shared.h>
#include <ralink.h>
#include <flash_mtd.h>
#include <ate.h>

extern int isResetSN(const char *sn);

struct sku_s {
	char *tcode;
	char *reg_spec;//fcc/cn/ec;
	char *band2g;
	char *band5g;
};
const struct sku_s ctrl_sku_tbl[] = {
	{ "UK", "CE", "2G_CH13", "5G_BAND123" },
	{ "EU", "CE", "2G_CH13", "5G_BAND123" },
	{ "AA", "FCC", "2G_CH11", "5G_BAND14" },
	{ "TW", "NCC", "2G_CH11", "5G_BAND14" },
	{ NULL, NULL, NULL, NULL }
};

char *ATE_RALINK_FACTORY_MODE_STR()
{
	char s[16];

	snprintf(s, 8, "%s%s%s%s%s%s%s%s", "A", "T", "E", "M", "O", "D", "E", "\000");
	return strdup(s);
}

int IS_ATE_FACTORY_MODE(void)
{
	int ret;
	char *p;

	p = ATE_RALINK_FACTORY_MODE_STR();
	ret = strcmp(nvram_safe_get(p), "1");
	free(p);
	return ret == 0;
}

int GetPhyStatus(int verbose, phy_info_list *list)
{
	ATE_mt7621_esw_port_status();
	return 1;
}

int getBootVer()
{
	unsigned char btv[5];
	char output_buf[32];
	memset(btv, 0, sizeof(btv));
	memset(output_buf, 0, sizeof(output_buf));
	FRead(btv, OFFSET_BOOT_VER, 4);
	snprintf(output_buf, sizeof(output_buf), "%s-%c.%c.%c.%c", nvram_safe_get("productid"), btv[0], btv[1], btv[2], btv[3]);
	puts(output_buf);
	return 0;
}

void ate_commit_bootlog(char *err_code)
{
	unsigned char fail_buffer[ OFFSET_SERIAL_NUMBER - OFFSET_FAIL_RET ];

	nvram_set("Ate_power_on_off_enable", err_code);
	nvram_commit();

	memset(fail_buffer, 0, sizeof(fail_buffer));
	strncpy(fail_buffer, err_code, OFFSET_FAIL_BOOT_LOG - OFFSET_FAIL_RET -1);
	Gen_fail_log(nvram_get("Ate_reboot_log"), nvram_get_int("Ate_boot_check"), (struct FAIL_LOG *) &fail_buffer[ OFFSET_FAIL_BOOT_LOG - OFFSET_FAIL_RET ]);
	Gen_fail_log(nvram_get("Ate_dev_log"), nvram_get_int("Ate_boot_check"), (struct FAIL_LOG *) &fail_buffer[ OFFSET_FAIL_DEV_LOG  - OFFSET_FAIL_RET ]);

	FWrite(fail_buffer, OFFSET_FAIL_RET, sizeof(fail_buffer));
}

int getMAC_5G()
{
	unsigned char buffer[6];
	char macaddr[18];
	memset(buffer, 0, sizeof(buffer));
	memset(macaddr, 0, sizeof(macaddr));

	if(FRead(buffer, OFFSET_MAC_ADDR, 6) < 0)
		dbg("READ MAC address: Out of scope\n");
	else
	{
		ether_etoa(buffer, macaddr);
		puts(macaddr);
	}
	return 0;
}

int getMAC_2G()
{
	unsigned char buffer[6];
	char macaddr[18];
	memset(buffer, 0, sizeof(buffer));
	memset(macaddr, 0, sizeof(macaddr));

	if(FRead(buffer, OFFSET_MAC_ADDR_2G, 6) < 0)
		dbg("READ MAC address 2G: Out of scope\n");
	else
	{
		ether_etoa(buffer, macaddr);
		puts(macaddr);
	}
	return 0;
}

int setMAC_5G(const char *mac)
{
	char ea[ETHER_ADDR_LEN];

	if(mac == NULL || !isValidMacAddr(mac) || !IS_ATE_FACTORY_MODE())
		return 0;
	if(ether_atoe(mac, ea))
	{
		FWrite(ea, OFFSET_MAC_ADDR, 6);
		FWrite(ea, OFFSET_MAC_GMAC0, 6);
		getMAC_5G();
	}
	return 1;
}

int setMAC_2G(const char *mac)
{
	char ea[ETHER_ADDR_LEN];

	if(mac == NULL || !isValidMacAddr(mac) || !IS_ATE_FACTORY_MODE())
		return 0;
	if(ether_atoe(mac, ea))
	{
		FWrite(ea, OFFSET_MAC_ADDR_2G, 6);
		FWrite(ea, OFFSET_MAC_GMAC2, 6);
		getMAC_2G();
	}
	return 1;
}

#if defined(RTCONFIG_NEW_REGULATION_DOMAIN)
int getRegSpec(void)
{
	char value_str[MAX_REGSPEC_LEN+1];
	int i;

	memset(value_str, 0, sizeof(value_str));
	FRead(value_str, REGSPEC_ADDR, MAX_REGSPEC_LEN);
	for(i = 0; i < MAX_REGSPEC_LEN && value_str[i] != '\0'; i++) {
		if ((unsigned char)value_str[i] == 0xff)
		{
			value_str[i] = '\0';
			break;
		}
	}
	puts(value_str);
	return 0;
}

int getRegDomain_5G(void)
{
	char value_str[MAX_REGDOMAIN_LEN+1];
	int i;

	memset(value_str, 0, sizeof(value_str));
	FRead(value_str, REG5G_EEPROM_ADDR, MAX_REGDOMAIN_LEN);

	for(i=0; i<MAX_REGDOMAIN_LEN; ++i) {
		if((value_str[i] == 0xFF) || (value_str[i] == 0x0))
			break;
		printf("%c", value_str[i]);
	}
	printf("\n");
	return 0;
}

int getRegDomain_2G(void)
{
	char value_str[MAX_REGDOMAIN_LEN+1];
	int i;

	memset(value_str, 0, sizeof(value_str));
	FRead(value_str, REG2G_EEPROM_ADDR, MAX_REGDOMAIN_LEN);

	for(i=0; i<MAX_REGDOMAIN_LEN; ++i) {
		if((value_str[i] == 0xFF) || (value_str[i] == 0x0))
			break;
		printf("%c", value_str[i]);
	}
	printf("\n");
	return 0;
}
#endif

int setAllLedOn()
{
	led_control(LED_POWER, LED_ON);
	led_control(LED_WPS, LED_ON);
	led_control(LED_2G, LED_ON);
	led_control(LED_5G, LED_ON);
#if defined(RTCONFIG_HAS_5G_2)
	led_control(LED_5G2, LED_ON);
#endif
	led_control(LED_WAN, LED_ON);
    led_control(LED_LAN, LED_ON);
#if !defined(RTCONFIG_NO_USBPORT)
	led_control(LED_USB, LED_ON);
	if (have_usb3_led(get_model()))
		led_control(LED_USB3, LED_ON);
#endif
#if 0 //defined(RTAC85P) move to kernel
	system("switch reg w 7d00 11111; switch reg w 7d04 66666; switch reg w 7d10 11111; switch reg w 7d14 11111; switch reg w 7d18 01000");
#endif
#if defined(RTCONFIG_LED_ALL)
	led_control(LED_ALL, LED_ON);
#endif
#if defined(RTAX53U)
	eval("mii_mgr", "-s", "-p", "0", "-r", "31", "-v", "0x1f");
	eval("mii_mgr", "-s", "-p", "0", "-r", "41", "-v", "0x24");
	eval("mii_mgr", "-s", "-p", "0", "-r", "31", "-v", "0x401f");
	eval("mii_mgr", "-s", "-p", "0", "-r", "41", "-v", "0x0");
	eval("iwpriv", (char *)WIF_2G, "set", "led_setting=00-00-00-00-02-00-00-00");
	eval("iwpriv", (char *)WIF_2G, "set", "led_setting=01-00-00-00-02-00-00-00");
#endif
	puts("1");
	return 0;
}

int setAllLedOff()
{
	led_control(LED_POWER, LED_OFF);
	led_control(LED_WPS, LED_OFF);
	led_control(LED_2G, LED_OFF);
	led_control(LED_5G, LED_OFF);
#if defined(RTCONFIG_HAS_5G_2)
	led_control(LED_5G2, LED_OFF);
#endif
	led_control(LED_WAN, LED_OFF);
    led_control(LED_LAN, LED_OFF);
#if !defined(RTCONFIG_NO_USBPORT)
	led_control(LED_USB, LED_OFF);
	if (have_usb3_led(get_model()))
		led_control(LED_USB3, LED_ON);
#endif
#if 0 //defined(RTAC85P) move to kernel
	system("switch reg w 7d00 11111; switch reg w 7d04 66666; switch reg w 7d10 11111; switch reg w 7d14 11111; switch reg w 7d18 10111");
#endif
#if defined(RTCONFIG_LED_ALL)
	led_control(LED_ALL, LED_OFF);
#endif
#if defined(RTAX53U)
	eval("mii_mgr", "-s", "-p", "0", "-r", "31", "-v", "0x1f");
	eval("mii_mgr", "-s", "-p", "0", "-r", "41", "-v", "0x24");
	eval("mii_mgr", "-s", "-p", "0", "-r", "31", "-v", "0x401f");
	eval("mii_mgr", "-s", "-p", "0", "-r", "41", "-v", "0x4000");
	eval("iwpriv", (char *)WIF_2G, "set", "led_setting=00-00-00-00-02-00-00-01");
	eval("iwpriv", (char *)WIF_2G, "set", "led_setting=01-00-00-00-02-00-00-01");
#endif
	puts("1");
	return 0;
}

void setAllLedNormal(void)
{
	led_control(LED_POWER, LED_ON);
	led_control(LED_WPS, LED_ON);

	if(nvram_match("wl0_radio", "1"))
#if defined(RTAX53U)
		eval("iwpriv", (char *)WIF_2G, "set", "led_setting=00-00-00-00-02-00-00-00");
#else
		led_control(LED_2G, LED_ON);
#endif
	if(nvram_match("wl1_radio", "1"))
#if defined(RTAX53U)
		eval("iwpriv", (char *)WIF_2G, "set", "led_setting=01-00-00-00-02-00-00-00");
#else
		led_control(LED_5G, LED_ON);
#endif
#if defined(RTAX53U)
	eval("mii_mgr", "-s", "-p", "0", "-r", "41", "-v", "0xC007");
#endif
	kill_pidfile_s("/var/run/wanduck.pid", SIGUSR2);
#if !defined(RTCONFIG_NO_USBPORT)
	start_usbled();
#endif
}

#if defined(RTCONFIG_NEW_REGULATION_DOMAIN)

int setRegSpec(const char *regSpec, int do_write)
{
	char REGSPEC[MAX_REGSPEC_LEN+1];
	char file[64];
	int i;

	if(regSpec == NULL || regSpec[0] == '\0' || strlen(regSpec) > MAX_REGSPEC_LEN)
		return -1;
	if(do_write == 1 && !IS_ATE_FACTORY_MODE())
		return -1;

	memset(REGSPEC, 0, sizeof(REGSPEC));
	for (i=0; regSpec[i]!='\0' ;i++)
		REGSPEC[i]=(char)toupper(regSpec[i]);

	// may be CE, FCC, AU, SG, NCC, JP. It is based on files in /ra_SKU/
	snprintf(file, sizeof(file), "/ra_SKU/SingleSKU_%s.dat", REGSPEC);
	if (!f_exists(file))
		return -1;

#ifdef RTCONFIG_HAS_5G
	snprintf(file, sizeof(file), "/ra_SKU/SingleSKU_5G_%s.dat", REGSPEC);
	if (!f_exists(file))
		return -1;
#endif

	FWrite(REGSPEC, REGSPEC_ADDR, MAX_REGSPEC_LEN);
	return 0;
}

int setRegDomain_2G(const char *cc)
{
	char CC[MAX_REGDOMAIN_LEN+1];
	int i;

	if(!IS_ATE_FACTORY_MODE() || (strcasecmp(cc, "2G_CH11")	&& strcasecmp(cc, "2G_CH13") && strcasecmp(cc, "2G_CH14")))
		return -1;

	memset(CC, 0x0, sizeof(CC));
	strcpy(CC, cc);
	for (i=0;CC[i]!='\0';i++)
		CC[i]=(char)toupper(CC[i]);
	FWrite(CC, REG2G_EEPROM_ADDR, MAX_REGDOMAIN_LEN);
	return 0;
}

int setRegDomain_5G(const char *cc)
{
	char CC[MAX_REGDOMAIN_LEN+1];
	int i;

	if(!IS_ATE_FACTORY_MODE() || (strcasecmp(cc, "5G_ALL") && strcasecmp(cc, "5G_BAND14")
		&& strcasecmp(cc, "5G_BAND12") && strcasecmp(cc, "5G_BAND24") && strcasecmp(cc, "5G_BAND1")
		&& strcasecmp(cc, "5G_BAND4") && strcasecmp(cc, "5G_BAND123") && strcasecmp(cc, "5G_BAND124")))
	{
		return -1;
	}
	memset(CC, 0, sizeof(CC));
	strcpy(CC, cc);
	for (i=0;CC[i]!='\0';i++)
		CC[i]=(char)toupper(CC[i]);
	FWrite(CC, REG5G_EEPROM_ADDR, MAX_REGDOMAIN_LEN);
	return 0;
}
#endif

int lenSN32(char *buf)
{
	int i;

	for(i = 0; i < 32 && (*(buf + i) - 1) < 0xfe; ++i)
		;
	return i;
}


int getSN(void)
{
	char sn[SERIAL_NUMBER_LENGTH +1];

	memset(sn, 0, sizeof(sn));
	if(FRead(sn, OFFSET_SERIAL_NUMBER, SERIAL_NUMBER_LENGTH) <0)
		dbg("READ Serial Number: Out of scope\n");
	else
	{
		sn[lenSN32(sn)] = 0x0;
		if(sn[0])
			puts(sn);
		else
			puts("NONE");
	}
	return 1;
}

int setSN(const char *SN)
{
	unsigned char buf[33];
	unsigned char snbuf[33];

	memset(buf, 0xff, sizeof(buf));
	memset(snbuf, 0, sizeof(snbuf));
	if(SN == NULL || !IS_ATE_FACTORY_MODE() || !isValidSN(SN) || !strcmp(SN, "NONE"))
		return 0;
	if(!isResetSN(SN))
	{
		if(FRead(snbuf, OFFSET_SERIAL_NUMBER, 32) < 0)
		{
			dbg("READ Serial Number: Out of scope\n");
			if(FWrite(SN, OFFSET_SERIAL_NUMBER, strlen(SN)) < 0)	
				return 0;
			else{
				getSN();
				return 1;
			}
		}
		if(strlen(SN) >= lenSN32(snbuf) || (FWrite(buf, OFFSET_SERIAL_NUMBER, lenSN32(snbuf)) < 0))
		{
			if(FWrite(SN, OFFSET_SERIAL_NUMBER, strlen(SN)) < 0)	
				return 0;
			else{
				getSN();
				return 1;
			}
		}
	}
	else
	{
		if(FWrite(buf, OFFSET_SERIAL_NUMBER, 32) < 0)
			return 0;
		else{
			getSN();
			return 1;
		}
	}
	return 1;
}

#ifdef RTCONFIG_ODMPID
int setMN(const char *MN)
{
	char modelname[16];

	if(!MN || !is_valid_hostname(MN) || !IS_ATE_FACTORY_MODE())
		return 0;
	memset(modelname, 0, sizeof(modelname));
	strncpy(modelname, MN, sizeof(modelname) - 1);
	FWrite(modelname, OFFSET_ODMPID, sizeof(modelname));
	nvram_set("odmpid", modelname);
	puts(nvram_safe_get("odmpid"));
	return 1;
}

int getMN(void)
{
	puts(nvram_safe_get("odmpid"));
	return 1;
}
#endif


int getSSID_2G()
{
	puts(nvram_safe_get("wl0_ssid"));
	return 1;
}

int getSSID_5G()
{
	puts(nvram_safe_get("wl1_ssid"));
	return 1;
}

int getPIN(void)
{
	unsigned char PIN[9];

	memset(PIN, 0, sizeof(PIN));
	FRead(PIN, OFFSET_PIN_CODE, 8);
	if(PIN[0] != 0xff)
		puts(PIN);
	return 0;
}

int setPIN(const char *pin)
{
	char PIN[9];
	if(!IS_ATE_FACTORY_MODE() || !pincheck(pin))
		return 0;
	FWrite(pin, OFFSET_PIN_CODE, 8);
	memset(PIN, 0, sizeof(PIN));
	memcpy(PIN, pin, 8);
	puts(PIN);
	return 1;
}

int Get_channel_list(int unit)
{
	unsigned char countryCode[3];
	char chList[256];

#ifdef RTCONFIG_NEW_REGULATION_DOMAIN
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	memset(countryCode, 0, sizeof(countryCode));
	strncpy(countryCode, nvram_safe_get(strcat_r(prefix, "country_code", tmp)), 2);
#else
	memset(countryCode, 0, sizeof(countryCode));
	FRead(countryCode, OFFSET_COUNTRY_CODE, 2);
#endif
	if(get_channel_list_via_driver(unit, chList, sizeof(chList)) > 0)
	{
		puts(chList);
	}
	else if (countryCode[0] != 0xff && countryCode[1] != 0xff)	// 0xffff is default
	{
		if(get_channel_list_via_country(unit, countryCode, chList, sizeof(chList)) > 0)
		{
			puts(chList);
		}
	}
	return 1;
}

int Get_ChannelList_5G()
{
	return Get_channel_list(1);
}

int Get_ChannelList_2G()
{
	return Get_channel_list(0);
}

void Get_fail_ret(void)
{
	unsigned char str[ OFFSET_FAIL_BOOT_LOG - OFFSET_FAIL_RET ];
	FRead(str, OFFSET_FAIL_RET, sizeof(str));
	if(str[0] == 0 || str[0] == 0xff)
		return;
	str[sizeof(str) -1] = '\0';
	puts(str);
}

void Get_fail_reboot_log()
{
	char buf[512];

	Get_fail_log(buf, sizeof(buf), OFFSET_FAIL_BOOT_LOG);
	puts(buf);
}

void Get_fail_dev_log()
{
	char buf[512];

	Get_fail_log(buf, sizeof(buf), OFFSET_FAIL_DEV_LOG);
	puts(buf);
}

int Set_SwitchPort_LEDs(const char *group, const char *action)
{
	int act, val;
	int base = strtol(group, NULL, 0);

	if(base >= 3)
		return -1;
	val = base << 16;
	if(!strcmp(action, "normal"))
	{
		act = 0;
		return rtkswitch_ioctl(100, val | act);
	}
	else if(!strcmp(action, "blink"))
	{
		act = 1;
		return rtkswitch_ioctl(100, val | act);
	}
	else if(!strcmp(action, "off"))
	{
		act = 2;
		return rtkswitch_ioctl(100, val | act);
	}
	else if(!strcmp(action, "on"))
	{
		act = 3;
		return rtkswitch_ioctl(100, val | act);
	}
	return -1;
}

int set_wantolan(void)
{
	return 0;
}


int set40M_Channel_5G(char *channel)
{
	char chl_buf[12];
	if (channel == NULL || !isValidChannel(0, channel) || !IS_ATE_FACTORY_MODE())
		return 0;
	sprintf(chl_buf,"Channel=%s", channel);
	eval("iwpriv", (char *)WIF_2G, "set", chl_buf);
	eval("iwpriv", (char *)WIF_2G, "set", "HtBw=1");
	puts("1");
	return 1;
}

int set40M_Channel_2G(char *channel)
{
	char chl_buf[12];
	if (channel == NULL || !isValidChannel(1, channel) || !IS_ATE_FACTORY_MODE())
		return 0;
	sprintf(chl_buf,"Channel=%s", channel);
	eval("iwpriv", (char *)WIF_2G, "set", chl_buf);
	eval("iwpriv", (char *)WIF_2G, "set", "HtBw=1");
	puts("1");
	return 1;
}

#if defined(RTCONFIG_TCODE)
int getTerritoryCode()
{
	char buf[6];

	memset(buf, 0, sizeof(buf));
	FRead(buf, OFFSET_TERRITORY_CODE, 5);
	if(buf[0] != 0xff)
		puts(buf);
	return 0;
}

int setTerritoryCode(const char *tcode)
{
	unsigned char buf[5];

	/* special case
	 * if tcode == "FFFFF", Write FF, FF, FF, FF, FF to OFFSET_TERRITORY_CODE
	 */
	if(!strcmp(tcode, "FFFFF"))
	{
		memset(buf, 0xFF, sizeof(buf));
		FWrite(buf, OFFSET_TERRITORY_CODE, 5);
		nvram_unset("territory_code");
		return 0;
	}

	if(tcode[2] == '/' && isupper(tcode[0]) && isupper(tcode[1]) && isdigit(tcode[3]) && isdigit(tcode[4]))
    {
		if(IS_ATE_FACTORY_MODE())
		{
			FWrite(tcode, OFFSET_TERRITORY_CODE, 5);
			nvram_set("territory_code", tcode);
			return 0;
		}
	}
	return -1;
}
#endif

int getPSK(void)
{
	int i;
	char *p = NULL;
	char buf[16];

	memset(buf, 0, sizeof(buf));
	FRead(buf, OFFSET_PSK, 14);
	if(buf[0] == 0xff)
		p = "NONE";
	else
	{
		for(i = 0; i < 14; i++){
			if(buf[i] == 0xff)
				buf[i] = 0x0;
		}
		p = buf;
	}
	puts(p);
	return 0;
}

int setPSK(const char *psk)
{
	int i, len;
	char buf[16];

	if(psk == NULL)
		return -1;
	if(!strcmp(psk, "NONE"))
	{
		memset(buf, 0xff, sizeof(buf));
		FWrite(buf, OFFSET_PSK, 15);
		return 0;
	}

	len = strlen(psk);
	if(len < 8 || len > 32 || !IS_ATE_FACTORY_MODE())
		return -1;
	for(i = 0; i < len; i++)
	{
		if(psk[i] == '0' || psk[i] == '1' || psk[i] == '8')
			return -1;
		else if(psk[i] != '_' && isalnum(psk[i]) == 0)
			return -1;
	}
	memset(buf, 0, sizeof(buf));
	strlcpy(buf, psk, 14);
	FWrite(buf, OFFSET_PSK, 15);
	return 0;
}

int getrssi(int band)
{
	char buf[32];
	struct iwreq wrq;
	memset(buf, 0, sizeof(buf));
	wrq.u.data.length = sizeof(buf);
	wrq.u.data.pointer = buf;
	wrq.u.data.flags = ASUS_SUBCMD_GRSSI;

	if(wl_ioctl(get_wifname(band), RTPRIV_IOCTL_ASUSCMD, &wrq) >= 0)
	{
		if(wrq.u.data.length)
			puts(wrq.u.data.pointer);
	}
	else
		dbg("errors in getting RSSI result\n");
	return 0;
}

int ResetDefault()
{
	int ret;
	ret = eval("mtd-erase", "-d", "nvram");
	if(ret == 1)
		puts("Timeout");	
	else if(ret)
		puts("0");	
	else
		puts("1");
	return 0;
}

int Get_Device_Flags(void)
{
	char log[132];
	char buf[4];
	unsigned short *flag = 0;
	flag = (unsigned short *) &buf[2];

	if(FRead(buf, OFFSET_DEV_FLAGS, 4) < 0)
	{
    	dbg("READ DEV Flags: Out of scope\n");
		return -1;
	}
	if(memcmp(buf, "FL", 2))
	{
		dbg("READ DEV Flags: no contents !\n");
		return -1;
	}
	if((*flag & 1) != 0)
		strcpy(log, " Has Thermal Pad.");
	printf("Flags: 0x%04x\n%s\n", *flag, log);
	return 0;
}

int Set_Device_Flags(const char *flags_str)
{
	int result = IS_ATE_FACTORY_MODE();
	char buf[4] = {0};
	unsigned short *flag = 0;
	flag = (unsigned short *) &buf[2];

	if(flags_str == NULL || strlen(flags_str) != 6 || strncmp(flags_str, "0x", 2) || !result)
		return -1;
	strncpy(buf, "FL", 2);
	*flag = strtoul(flags_str, NULL, 16);
	FWrite(buf, OFFSET_DEV_FLAGS, 4);
	result = Get_Device_Flags();

	return result;
}

void set_factory_mode()
{
	char *p;

	if(nvram_match("ateCommand_flag", "1"))
    {
      p = ATE_RALINK_FACTORY_MODE_STR();
      nvram_set(p, "1");
      nvram_commit();
      free(p);
	}
}

int _dump_powertable()
{
	int band;
	char buf[10000], word[256];
	char *next = NULL;
	struct iwreq wrq;

	wrq.u.data.length = sizeof(buf);
	wrq.u.data.pointer = buf;
	wrq.u.data.flags = ASUS_SUBCMD_GETSKUTABLE;
	band = 0;
	foreach(word, nvram_safe_get("wl_ifnames"), next){
		memset(buf, 0, sizeof(buf));
		if((wl_ioctl(word, RTPRIV_IOCTL_ASUSCMD, &wrq)) < 0){
			printf("ioctl error\n");
		}
		printf("%dg power table:\n%s", band ? 5 : 2, buf);
		band++;
	}
	return 0;
}

int _dump_txbftable(void)
{
	int band;
	char buf[10000], word[256];
	char *next = NULL;
	struct iwreq wrq;

	wrq.u.data.length = sizeof(buf);
	wrq.u.data.pointer = buf;
	wrq.u.data.flags = ASUS_SUBCMD_GETSKUTABLE_TXBF;
	band = 0;
	foreach(word, nvram_safe_get("wl_ifnames"), next){
		memset(buf, 0, sizeof(buf));
		if((wl_ioctl(word, RTPRIV_IOCTL_ASUSCMD, &wrq)) < 0){
			printf("ioctl error\n");
		}
		printf("%dg power table:\n%s", band ? 5 : 2, buf);
		band++;
	}
	return 0;
}

void set_IpAddr_Lan(const char *ip)
{
	char buf[16];

	if(IS_ATE_FACTORY_MODE())
	{
		if(!strcmp(ip, "NONE"))
		{
			memset(buf, 0xff, sizeof(buf));
			nvram_unset("IpAddr_Lan");
		}
		else
		{
			memset(buf, 0, sizeof(buf));
			strncpy(buf, ip, 15);
			nvram_set("IpAddr_Lan", buf);
		}
		FWrite(buf, OFFSET_IPADDR_LAN, 16);
		nvram_commit();
		puts(nvram_safe_get("IpAddr_Lan"));
	}
	else
    	puts("ATE_ERROR_INCORRECT_PARAMETER");
}

void get_IpAddr_Lan()
{
	char *ip = nvram_safe_get("IpAddr_Lan");

	if(*ip && strcmp(ip, "NONE"))
		puts(ip);
	else
		puts("NONE");
}

void set_MRFLAG(const char *flag)
{
	char buf[16];

	if(IS_ATE_FACTORY_MODE())
	{
		if(!strcmp(flag, "NONE"))
		{
			memset(buf, 0xff, sizeof(buf));
			nvram_unset("MRFLAG");
		}
		else
		{
			memset(buf, 0, sizeof(buf));
			strncpy(buf, flag, 15);
			nvram_set("MRFLAG", buf);
		}

		FWrite(buf, 0x6FF30, 16);
		nvram_commit();
    	puts(nvram_safe_get("MRFLAG"));
	}
	else
    	puts("ATE_ERROR_INCORRECT_PARAMETER");
}

void get_MRFLAG()
{
	char *flag = nvram_safe_get("MRFLAG");

	if(!*flag || !strcmp(flag, "NONE"))
    	puts("NONE");
	else
		puts(flag);
}

int set_HwId(const char *HwId)
{
	if(!IS_ATE_FACTORY_MODE() || HwId == NULL)
		return -1;

	FWrite(HwId, OFFSET_HWID, 4);
	puts(HwId);

	return 0;
}

int set_HwVersion(const char *HwVer)
{
	if(!IS_ATE_FACTORY_MODE() || HwVer == NULL)
		return -1;

	FWrite(HwVer, OFFSET_HW_VERSION, 8);
	puts(HwVer);
	return 0;
}

int set_HwBom(const char *HwBom)
{
	if(!IS_ATE_FACTORY_MODE() || HwBom == NULL)
		return -1;
	FWrite(HwBom, OFFSET_HW_BOM, 32);
	puts(HwBom);
	return 0;
}

int set_DateCode(const char *DateCode)
{
	int i, len;

	if(DateCode == NULL || !IS_ATE_FACTORY_MODE())
		return -1;
	len = strlen(DateCode);
	if(len != 8)
		return -1;
	for(i = 0; i < len; i++)
		if(!isdigit(DateCode[i]))
			return -1;

	FWrite(DateCode, OFFSET_HW_DATE_CODE, 8);
	puts(DateCode);
	return 0;
}

int get_HwId(void)
{
	char HwId[5];
	char *p = NULL;
	memset(HwId, 0, sizeof(HwId));
	FRead(HwId, OFFSET_HWID, 4);
	p = strchr(HwId, 0xff);
	if( p)
		*p = 0x0;
	puts(HwId);
	return 0;
}

int get_HwVersion(void)
{
	char HwVersion[9];
	memset(HwVersion, 0, sizeof(HwVersion));
	FRead(HwVersion, OFFSET_HW_VERSION, 8);
	puts(HwVersion);
	return 0;
}

int get_HwBom(void)
{
	char HwBom[33];

	memset(HwBom, 0, sizeof(HwBom));
	FRead(HwBom, OFFSET_HW_BOM, 32);
	puts(HwBom);
	return 0;
}

int get_DateCode(void)
{
	char DateCode[9];
	memset(DateCode, 0, sizeof(DateCode));
	FRead(DateCode, OFFSET_HW_DATE_CODE, 8);
	puts(DateCode);
	return 0;
}

#if defined(RTCONFIG_ASUSCTRL)
int asus_ctrl_get()
{
	char ctrl[9];
	memset(ctrl, 0, sizeof(ctrl));
	if(FRead(ctrl, OFFSET_ASUSCTRL_FLAGS, 8))
		return -1;

	if(strcmp(nvram_safe_get("asusctrl_flags"), ctrl))
	{
		nvram_set("asusctrl_flags", ctrl);
		nvram_commit();
	}
	puts(ctrl);
	return 0;
}

int asus_ctrl_write(const char *asusctrl)
{
	char ctrl[9];
	memset(ctrl, 0, sizeof(ctrl));
	asus_ctrl_nv();
	if(!strcmp(asusctrl, "NONE"))
	{
		memset(ctrl, 0xff, sizeof(ctrl));
		FWrite(ctrl, OFFSET_ASUSCTRL_FLAGS, 8);
		_dprintf("asusctrl_value : clean-up-asusctrl-value.\n");
		return 0;
	}
	else if(nvram_match("asusctrl_flags", asusctrl))
	{
		_dprintf("asusctrl_value : all-up-to-date.\n");
		return 0;
	}
	else
	{
		strlcpy(ctrl, asusctrl, 9);
		if(FWrite(ctrl, OFFSET_ASUSCTRL_FLAGS, 8) < 0)
			return -1;
		else
			asus_ctrl_get();
	}
	return 0;
}

int asus_ctrl_sku_get()
{
	char sku[3];
	memset(sku, 0, sizeof(sku));
	if(FRead(sku, OFFSET_ASUSCTRL_CHG_SKU, 2))
		return -1;
	if(strcmp(nvram_safe_get("asusctrl_chg_sku", sku))
	{
		nvram_set("asusctrl_chg_sku", sku);
		nvram_commit();
	}
	puts(sku);
	return 0;
}

int asus_ctrl_sku_write(const char *asusctrl_sku)
{
	char sku[3];
	char tcode[6];
	memset(sku, 0, sizeof(sku));
	memset(tcode, 0, sizeof(tcode));
	if(!strcmp(asusctrl_sku, "FF"))
	{
		memset(sku, 0xff, sizeof(sku));
		FWrite(sku, OFFSET_ASUSCTRL_CHG_SKU, 2);
		nvram_unset("asusctrl_chg_sku");
		return 0;
	}
	if(strlen(asusctrl_sku) != 2 || !isupper(*asusctrl_sku) || (!isupper(asusctrl_sku[1]) && !isdigit(asusctrl_sku[1])))
		return -1;
    if(FRead(tcode, OFFSET_TERRITORY_CODE, 5) < 0)
		return -2;
    if(strncmp(tcode, asusctrl_sku, 2))
	{
		nvram_set("webs_chg_sku", "1");
		strlcpy(sku, asusctrl_sku, 3);
		if(FWrite(sku, OFFSET_ASUSCTRL_CHG_SKU, 2) < 0)
			return -3;
		else
			asus_ctrl_sku_get();
	}
	return 0;
}

void init_asusctrl()
{
	int model = get_model();
	asus_ctrl_get();
}

void asus_ctrl_sku_check()
{
	int i;
	char sku[7] = {0};
	char tcode[7] = {0};
	char file[64] = {0};

	if(asus_ctrl_en(4))
	{
		asus_ctrl_sku_get();
		strlcpy(sku, nvram_safe_get("asusctrl_chg_sku"), 6);
		strlcpy(tcode, nvram_safe_get("territory_code"), 6);
		if(strlen(sku) == 2)
		{
			if(strncmp(tcode, sku, 2))
			{
				for(psku = &ctrl_sku_tbl[0]; psku->tcode; psku++)
				{
					if(psku->reg_spec == NULL || psku->band2g == NULL || psku->band5g == NULL)
						break;
					if(!strncmp(psku->tcode, tcode, 2))
					{
						char REGSPEC[4] = {0};
						char REGSPEC2[7] = {0};
						char REGSPEC5[10] = {0};
						snprintf(REGSPEC, sizeof(REGSPEC), "%s", psku->reg_spec);
						for(i = 0; REGSPEC[i] != '\0' ; i++)
							REGSPEC[i] = (char)toupper(REGSPEC[i]);

						// may be CE, FCC, AU, SG, NCC, JP. It is based on files in /ra_SKU/
						memset(file, 0, sizeof(file));
						snprintf(file, sizeof(file), "/ra_SKU/SingleSKU_%s.dat", REGSPEC);
						if (!f_exists(file))
							return -1;
#ifdef RTCONFIG_HAS_5G
						memset(file, 0, sizeof(file));
						snprintf(file, sizeof(file), "/ra_SKU/SingleSKU_5G_%s.dat", REGSPEC);
						if (!f_exists(file))
							return -1;
#endif
						if(strcasecmp(psku->band2g, "2G_CH11") && strcasecmp(psku->band2g, "2G_CH13") && 
							strcasecmp(psku->band2g, "2G_CH14"))
							return 1;
						snprintf(REGSPEC2, sizeof(REGSPEC2), "%s", psku->band2g);
						for(i = 0; REGSPEC2[i] != '\0' ; i++)
							REGSPEC2[i] = (char)toupper(REGSPEC2[i]);
						if(strcasecmp(psku->band5g, "5G_ALL") && strcasecmp(psku->band5g, "5G_BAND12") &&
							strcasecmp(psku->band5g, "5G_BAND14") && strcasecmp(psku->band5g, "5G_BAND24") &&
							strcasecmp(psku->band5g, "5G_BAND1") && strcasecmp(psku->band5g, "5G_BAND4") &&
							strcasecmp(psku->band5g, "5G_BAND123") && strcasecmp(psku->band5g, "5G_BAND124"))
							return 1;
						snprintf(REGSPEC5, sizeof(REGSPEC5), "%s", psku->band5g);
						for(i = 0; REGSPEC5[i] != '\0' ; i++)
							REGSPEC5[i] = (char)toupper(REGSPEC5[i]);
						strncpy(tcode, sku, 2);
						FWrite(tcode, OFFSET_TERRITORY_CODE, 5);
						nvram_set("territory_code", tcode);
						nvram_commit();
						FWrite(REGSPEC, REGSPEC_ADDR, 4);
						FWrite(REGSPEC2, REG2G_EEPROM_ADDR, 10);
						FWrite(REGSPEC5, REG5G_EEPROM_ADDR, 10);
					}
				}
			}
		}
	}
	return 0;
}

void asus_ctrl_sku_update()
{
	int flags;
	char tcode[8];
	char sku[24];


	asus_ctrl_get();
	asus_ctrl_sku_get();
	flags = strtoul(nvram_safe_get("asusctrl_flags"), NULL, 16);
	strlcpy(sku, nvram_safe_get("asusctrl_chg_sku"), 6);
	strlcpy(tcode, nvram_safe_get("territory_code"), 6);
	if(sku[0])
	{
		if(strncmp(tcode, sku, 2))
		{
			if((flags & 0x10) != 0)
			{
				_dprintf("\n%s: Reboot\n", __func__);
				killall(1, SIGTERM);
			}
		}
	}
}
#endif

#if defined(RTCONFIG_ATEUSB3_FORCE)
int getForceU3()
{
	unsigned char buffer[2];
	unsigned char *dst;
	dst = buffer;
	FRead(dst, OFFSET_FORCE_USB3, 1);
	if ( *dst == '1' )
		puts("1");
	else
		puts("0");
	return 0;
}

int setForceU3(const char *val)
{
	if((*val - '0') >= 2 || !IS_ATE_FACTORY_MODE())
		return -1;
	FWrite(val, OFFSET_FORCE_USB3, 1);
	if(*val == '0')
		nvram_unset("usb_usb3");
	else
		nvram_set("usb_usb3", "1");
	nvram_commit();
	return 0;
}
#endif

