/*
 * Miscellaneous services
 *
 * Copyright (C) 2009, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: services.c,v 1.100 2010/03/04 09:39:18 Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <bcmnvram.h>
#include <shutils.h>
#include <rc.h>
#include <shared.h>

//#define NOT_SHELL_FAPI

int 
start_wps_method(void)
{
	int w_setting = 1;
	if(getpid()!=1) {
		notify_rc("start_wps_method");
		return 0;
	}

#ifdef RTCONFIG_WPS_ENROLLEE
	if (nvram_match("wps_enrollee", "1"))
		start_wsc_enrollee();
	else 
#endif
	{
		// To avoid WPS is unconfigured state
		nvram_set_int("w_Setting", 1);
		nvram_commit();
		start_wsc();
	}

	return 0;
}

int 
stop_wps_method(void)
{
	if(getpid()!=1) {
		notify_rc("stop_wps_method");
		return 0;
	}

#ifdef RTCONFIG_WPS_ENROLLEE
	if (nvram_match("wps_enrollee", "1")) {
		stop_wsc_enrollee();
	}
	else
#endif
		stop_wsc();

	return 0;
}

void runtime_onoff_wps(int onoff)
{
	int unit = 0;
	char wif[8], *next, prefix[] = "wlXXXXXXXXXX_";

	foreach(wif, nvram_safe_get("wl_ifnames"), next) {
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		if (!nvram_pf_match(prefix, "auth_mode_x", "sae")
#ifdef RTCONFIG_WIFI6E
		 && !nvram_pf_match(prefix, "auth_mode_x", "owe")
#endif
		 && !nvram_pf_match(prefix, "auth_mode_x", "wpa3")) {
			//dbg("%s: %s WPS IE of Beacon/Probe resp for %s\n", __func__, onoff ? "unhide" : "hide", wif);
			doSystem(IWPRIV " %s hide_wpsie %d\n", wif, onoff ? 0 : 1);
			doSystem("hostapd_cli -i%s update_beacon", wif);
		}

		unit++;
	}
}

int
start_wps_method_ob(void)
{
	if (nvram_get_int("wps_enable") == 0)
		runtime_onoff_wps(1);
	start_wps_method();
	return 0;
}

int
stop_wps_method_ob(void)
{
	stop_wps_method();
	if (nvram_get_int("wps_enable") == 0)
		runtime_onoff_wps(0);
	return 0;
}

extern int g_isEnrollee[MAX_NR_WL_IF];

int is_wps_stopped(void)
{
	int i = 0, ret = 1;
	char status[16], tmp[128], prefix[] = "wlXXXXXXXXXX_", word[256], *next, ifnames[128];
	int wps_band = nvram_get_int("wps_band_x"), multiband = get_wps_multiband();
	char tmpbuf[512];
	int obding = nvram_match("cfg_obstatus", "4") ? 1 : 0;

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next){
		/* don't perform the following checks when onboarding */
		if (obding == 0) {
			if(i >= MAX_NR_WL_IF)
				break;
			if(!multiband && wps_band != i){
				++i;
				continue;
			}
			SKIP_ABSENT_BAND_AND_INC_UNIT(i);
			snprintf(prefix, sizeof(prefix), "wl%d_", i);
			if(!__need_to_start_wps_band(prefix) || nvram_pf_match(prefix, "radio", "0")){
				ret = 0;
				++i;
				continue;
			}
		}

#ifdef RTCONFIG_WPS_ENROLLEE
		if(nvram_match("wps_enrollee", "1")) {
#if defined(RTCONFIG_AMAS)
			if (i != wps_enrollee_band) {
				++i;
				continue;
			}
#endif
			strcpy(status, getWscStatus_enrollee(i, tmpbuf, sizeof(tmpbuf)));
			dbG("band %d(%s) wps status: %s\n", i, get_staifname(i), status);
		}
		else
#endif
		{
			getWscStatusStr(wl_wave_unit(i), status, sizeof(status));
			dbG("band %d(%s) wps status: %s\n", i, get_wififname(i), status);
		}

		if(!strcmp(status, "Success")
#ifdef RTCONFIG_WPS_ENROLLEE
				|| !strcmp(status, "COMPLETED")
#endif
				){
			dbG("\nWPS %s\n", status);
#if defined(RTCONFIG_WPS_LED) || defined(BLUECAVE)
			nvram_set("wps_success", "1");
#endif
#if (defined(RTCONFIG_WPS_ENROLLEE))
			if(nvram_match("wps_enrollee", "1")){
				nvram_set("wps_e_success", "1");
#if defined(RTCONFIG_AMAS)
				amas_save_wifi_para();
#endif
#if defined(RTCONFIG_WIFI_CLONE)
				wifi_clone(i);
#endif
			}
#endif
			ret = 1;
		}
		else if(!strcmp(status, "Failed")
				|| !strcmp(status, "Timeout") || !strcmp(status, "Timed-out")
				|| !strcmp(status, "Overlap")
#ifdef RTCONFIG_WPS_ENROLLEE
				|| !strcmp(status, "INACTIVE")
#endif
				){
			dbG("\nWPS %s\n", status);
			ret = 1;
			if (!strcmp(status, "Overlap"))
				stop_wps_method();
		}
		else if (!strcmp(status, "Idle")
			) {
			ret = 1;
		}
		else
			ret = 0;

		if(ret)
			break;

		++i;
	}

	return ret;
}

int is_wps_success(void)
{
#ifdef RTCONFIG_WPS_ENROLLEE
	if(nvram_match("wps_enrollee", "1"))
		return nvram_get_int("wps_e_success");
#endif
	return nvram_get_int("wps_success");
}

/*
 * save new AP configuration from WPS external registrar
 */
void nvram_set_wlX(char* nv, char* value)
{
	int i;
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";

	for (i = WL_2G_BAND; i < MAX_NR_WL_IF; ++i) {
		SKIP_ABSENT_BAND(i);
		snprintf(prefix, sizeof(prefix), "wl%d_", i);
		nvram_set(strcat_r(prefix, nv, tmp), value);
	}
}


#define WPS_CONFIG_FILE "/tmp/wps_config.xml"

int write_wps_config(int configured){
	FILE *fp = fopen(WPS_CONFIG_FILE, "w+");

	if(!fp){
		_dprintf("%s: cannot open the WPS config file.\n", __func__);
		return -1;
	}

	fprintf(fp, "Object_0=Device.WiFi.Radio.X_LANTIQ_COM_Vendor.WPS\n");
	fprintf(fp, "ConfigState_0=%s\n", (configured == 1)?"Configured":"Unconfigured");
	fprintf(fp, "Status_0=Idle\n");
	if(configured == 0)
		fprintf(fp, "WPSAction_0=ResetWPS\n");

	fclose(fp);

	return 0;
}

int set_wps_idle(int unit)
{
	char cmd[256];
	FILE *fp = fopen(WPS_CONFIG_FILE, "w+");

	if(!fp){
		_dprintf("[%s][%d]: cannot open the WPS config file.\n",
			__func__, __LINE__);
		return -1;
	}
	fprintf(fp, "Object_0=Device.WiFi.Radio.X_LANTIQ_COM_Vendor.WPS\n");
	fprintf(fp, "Status_0=Idle\n");
	fclose(fp);

	_dprintf("[%s][%d]: set_wps_idle:[%d]\n",
			__func__, __LINE__, unit);

	snprintf(cmd, sizeof(cmd),
		"/usr/sbin/fapi_wlan_cli setWpsTR181 -i%d -f%s",
		wl_wave_unit(unit), WPS_CONFIG_FILE);

	system(cmd);

	return 0;
}
int set_wps_config(int unit, int configured){
	int retVal;
	char cmd[256];
	FILE *fp;
	char *pt1, *pt2, *pt3;
	int orig_config = 0;

	_dprintf("%s(%d): configured=%d.\n", __func__, unit, configured);

#if 0  // Don't check wether change occur. Always set WPS as configured.
#ifdef NOT_SHELL_FAPI
	retVal = wlan_getWpsConfigurationState(wl_wave_unit(unit), buf);
	_dprintf("%s(%d): buf=%s.\n", __func__, unit, buf);
	if((!strcmp(buf, "Configured") && configured == 1)
			|| (!strcmp(buf, "Unconfigured") && configured == 0)
			){
		_dprintf("%s(%d): config state was not changed.\n", __func__, unit);
		return retVal;
	}

	dbObjPtr = HELP_CREATE_OBJ(SOPT_OBJVALUE);
#else
	snprintf(cmd, sizeof(cmd), "/usr/sbin/fapi_wlan_cli getWpsConfigurationState -i%d", wl_wave_unit(unit));
	fp = popen(cmd, "r");
	if(fp){
		memset(cmd, 0, sizeof(cmd));
		while(fgets(cmd, sizeof(cmd), fp)){
			pt1 = strstr(cmd, "ConfigState= '");
			if(pt1){
				pt2 = pt1+strlen("ConfigState= '");
				pt3 = strchr(pt2, '\'');
				pt3[0] = '\0';

				if(!strcmp(pt2, "Configured"))
					orig_config = 1;
				else
					orig_config = 0;

				if(orig_config == configured){
					_dprintf("%s(%d): config state was not changed.\n", __func__, unit);
					pclose(fp);
					return 0;
				}
			}
		}
		pclose(fp);
	}
#endif
#endif

	write_wps_config(configured);

	snprintf(cmd, sizeof(cmd), "/usr/sbin/fapi_wlan_cli setWpsTR181 -i%d -f%s", wl_wave_unit(unit), WPS_CONFIG_FILE);
	system(cmd);

	_dprintf("%s(%d): done.\n", __func__, unit);

	return retVal;
}

int set_all_wps_config(int configured){
	int i;
	char word[256], *next;
	char ifnames[128];

	if(nvram_match("lan_ipaddr", ""))
		return -1;

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next){
		if(i >= MAX_NR_WL_IF)
			break;

		_dprintf("%s: %s %s...\n", __func__, (configured == 1)?"Configure":"Unconfigure", word);
		set_wps_config(i, configured);

		++i;
	}

	return 0;
}

void wps_oob(void){
	if(nvram_match("lan_ipaddr", ""))
		return;

	nvram_set("wps_reset", "1");
	restart_wireless();

	nvram_commit();
}
/*
  wps_pin <BSS_name> <uuid> <pin> [timeout] [addr] = add WPS Enrollee PIN
  wps_check_pin <PIN> = verify PIN checksum
  wps_pbc <BSS_name> = indicate button pushed to initiate PBC
  wps_cancel <BSS_name> = cancel the pending WPS operation
  wps_ap_pin <BSS_name> <cmd> [params..] = enable/disable AP PIN
  wps_config <BSS_name> <SSID> <auth> <encr> <key> = configure AP
  wps_get_status <BSS_name> = show current WPS status

*/

void start_wsc(void){
	char *wps_sta_pin = nvram_safe_get("wps_sta_pin");
	int wps_band = nvram_get_int("wps_band_x");
	char cmd[256];

	if(nvram_match("lan_ipaddr", ""))
		return;

	if(!need_to_start_wps_band(wps_band))
		return;

	dbg("%s: start wsc(%d)\n", __func__, wps_band);

	_dprintf("%s: cancel wlan%d's WPS...\n", __func__, wl_wave_unit(wps_band));
	//snprintf(cmd, sizeof(cmd), "/usr/sbin/fapi_wlan_cli cancelWps -i%d", wl_wave_unit(wps_band));
	snprintf(cmd, sizeof(cmd), "hostapd_cli -i%s wps_cancel %s", get_wififname(wps_band), get_wififname(wps_band));
	_dprintf("%s\n", cmd);
	system(cmd);
	sleep(1);

	if(strlen(wps_sta_pin) && strcmp(wps_sta_pin, "00000000")
			&& wl_wpsPincheck(wps_sta_pin) == 0){
		dbg("WPS: PIN\n");	// PIN method
		//snprintf(cmd, sizeof(cmd), "/usr/sbin/fapi_wlan_cli setWpsEnrolleePin -i%d -p%s", wl_wave_unit(wps_band), wps_sta_pin);
		snprintf(cmd, sizeof(cmd), "hostapd_cli -i%s wps_pin %s any %s", get_wififname(wps_band), get_wififname(wps_band), wps_sta_pin);
		_dprintf("%s\n", cmd);
		system(cmd);
	}
	else{
		dbg("WPS: PBC\n");	// PBC method
		//snprintf(cmd, sizeof(cmd), "/usr/sbin/fapi_wlan_cli setWpsPbcTrigger -i%d", wl_wave_unit(wps_band));
		snprintf(cmd, sizeof(cmd), "hostapd_cli -i%s wps_pbc %s", get_wififname(wps_band), get_wififname(wps_band));
		_dprintf("%s\n", cmd);
		system(cmd);
	}
}

void stop_wsc(){
	int wps_band = nvram_get_int("wps_band_x");
	char cmd[256];

	if(!need_to_start_wps_band(wps_band))
		return;

	dbg("%s: stop wsc(%d)\n", __func__, wps_band);

	_dprintf("%s: cancel wlan%d's WPS...\n", __func__, wl_wave_unit(wps_band));
	//snprintf(cmd, sizeof(cmd), "/usr/sbin/fapi_wlan_cli cancelWps -i%d", wl_wave_unit(wps_band));
	snprintf(cmd, sizeof(cmd), "hostapd_cli -i%s wps_cancel %s", get_wififname(wps_band), get_wififname(wps_band));
	_dprintf("%s\n", cmd);
	system(cmd);
}

int getWscStatusStr(int unit, char *ret_buf, int buf_size){
	char buf[512] = {0};
	FILE *fp;
	int len;
	char *pt1,*pt2,*pt3;

	snprintf(buf, sizeof(buf), "hostapd_cli -i%s wps_get_status %s", get_wififname(unit), get_wififname(unit));
	fp = popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, "PBC Status: ");
			pt3 = strstr(buf, "Last WPS result: ");

			if (pt1) {
				pt2 = pt1 + strlen("PBC Status: ");
				pt1 = strstr(pt2, "Last WPS result: ");
				if (pt1) {
					*pt1 = '\0';
					chomp(pt2);
				}
				
				if (!strcmp(pt2, "Disabled")){
					// Check Last WPS result
					if (pt3) {
						pt2 = pt3 + strlen("Last WPS result: ");
						pt1 = strstr(pt2, "Peer Address: ");
						if (pt1) {
							*pt1 = '\0';
							chomp(pt2);
						}
						if (!strcmp(pt2, "Success")) {
							strncpy(ret_buf,"Success",buf_size);
							return 1;
						}
					}
					strncpy(ret_buf,"Disabled",buf_size);
					return 0;
				}
				else if (!strcmp(pt2, "Active") )
				{
					strncpy(ret_buf,"In Progress",buf_size);
					return 0;
				}
				else if (!strcmp(pt2, "Timed-out"))
				{
					strncpy(ret_buf,"Timeout",buf_size);
					return 0;
				}
				else
				{
					strncpy(ret_buf,pt2,buf_size);
					return 0;					
				}
			}
		}
	}
}
#if defined(RTCONFIG_AMAS)
void amas_save_wifi_para()
{
	char buf[512];
	FILE *fp;
	int len, i;
	char *pt1, *pt2;
	char tmp[128], tmp2[128], prefix[] = "wlcXXXXXXXXX_", prefix2[] = "wlXXXXXXXXX_", word[256], *next, ifnames[128];

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		sprintf(buf, "/opt/lantiq/wave/confs/wpa_supplicant_%s.conf", get_staifname(0));
		snprintf(prefix, sizeof(prefix), "wlc%d_", i);
		fp = fopen(buf, "r");
		if (fp) {
			memset(buf, 0, sizeof(buf));
			len = fread(buf, 1, sizeof(buf), fp);
			pclose(fp);
			if (len > 1) {
				buf[len-1] = '\0';
				//PSK
				pt1 = strstr(buf, "psk=\"");
				if (pt1) {	//WPA2-PSK
					pt2 = pt1 + strlen("psk=\"");
					pt1 = strstr(pt2, "\"");
					if (pt1) {
						*pt1 = '\0';
						chomp(pt2);
						nvram_set(strcat_r(prefix, "wpa_psk", tmp), pt2);
					}
				}
				else {		//OPEN
					nvram_set(strcat_r(prefix, "auth_mode", tmp), "open");
					nvram_set(strcat_r(prefix, "wep", tmp), "0");
				}
			}
		}

		sprintf(buf, "wpa_cli -i%s status", get_staifname(0));
		fp = popen(buf, "r");
		if (fp) {
			memset(buf, 0, sizeof(buf));
			len = fread(buf, 1, sizeof(buf), fp);
			pclose(fp);
			if (len > 1) {
				buf[len-1] = '\0';
				// ap_mac
				pt1 = strstr(buf, "bssid=");
				if (pt1) {
					pt2 = pt1 + strlen("bssid=");
					pt1 = strstr(pt2, "freq=");
					if (pt1) {
						*pt1 = '\0';
						chomp(pt2);
						//nvram_set(strcat_r(prefix, "ap_mac", tmp), pt2);
					}
				}

				// ssid
				pt2 = pt1 + 1;
				pt1 = strstr(pt2, "ssid=");
				if (pt1) {
					pt2 = pt1 + strlen("ssid=");
					pt1 = strstr(pt2, "id=");
					if (pt1) {
						*pt1 = '\0';
						chomp(pt2);
						nvram_set(strcat_r(prefix, "ssid", tmp), pt2);
					}
				}

				// auth_mode and crypto
				pt2 = pt1 + 1;
				pt1 = strstr(pt2, "key_mgmt=");
				if (pt1) {
					pt2 = pt1 + strlen("key_mgmt=");
					pt1 = strstr(pt2, "wpa_state=");
					if (pt1) {
						*pt1 = '\0';
						chomp(pt2);
						if (strstr(pt2, "WPA-")) {
							nvram_set(strcat_r(prefix, "auth_mode", tmp), "psk");
							nvram_set(strcat_r(prefix, "crypto", tmp), "aes+tkip");
						} else if (strstr(pt2, "NONE")) {
							nvram_set(strcat_r(prefix, "auth_mode", tmp), "open");
							nvram_set(strcat_r(prefix, "wep", tmp), "0");
						} else{
							nvram_set(strcat_r(prefix, "auth_mode", tmp), "psk2");
							nvram_set(strcat_r(prefix, "crypto", tmp), "aes");
						}
					}
				}
			}
		}
		++i;
	}

	i = 0;
	foreach(word, ifnames, next) {
		snprintf(prefix, sizeof(prefix), "wlc%d_", i);

		//wlx
		snprintf(prefix2, sizeof(prefix2), "wl%d_", i);
		{
			nvram_set(strcat_r(prefix2, "ssid", tmp), nvram_safe_get(strcat_r(prefix, "ssid", tmp2)));
			nvram_set(strcat_r(prefix2, "auth_mode_x", tmp), nvram_safe_get(strcat_r(prefix, "auth_mode", tmp2)));
			nvram_set(strcat_r(prefix2, "wep_x", tmp), nvram_safe_get(strcat_r(prefix, "wep", tmp2)));
			if (nvram_get_int(strcat_r(prefix, "wep", tmp))) {
				nvram_set(strcat_r(prefix2, "key", tmp), nvram_safe_get(strcat_r(prefix, "key", tmp2)));
				nvram_set(strcat_r(prefix2, "key1", tmp), nvram_safe_get(strcat_r(prefix, "wep_key", tmp2)));
				nvram_set(strcat_r(prefix2, "key2", tmp), nvram_safe_get(strcat_r(prefix, "wep_key", tmp2)));
				nvram_set(strcat_r(prefix2, "key3", tmp), nvram_safe_get(strcat_r(prefix, "wep_key", tmp2)));
				nvram_set(strcat_r(prefix2, "key4", tmp), nvram_safe_get(strcat_r(prefix, "wep_key", tmp2)));
			}
			nvram_set(strcat_r(prefix2, "crypto", tmp), nvram_safe_get(strcat_r(prefix, "crypto", tmp2)));
			nvram_set(strcat_r(prefix2, "wpa_psk", tmp), nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp2)));
		}

#if 0
		//wlx.1
		snprintf(prefix2, sizeof(prefix2), "wl%d.1_", i);
		{
			nvram_set(strcat_r(prefix2, "ssid", tmp), nvram_safe_get(strcat_r(prefix, "ssid", tmp2)));
			nvram_set(strcat_r(prefix2, "auth_mode_x", tmp), nvram_safe_get(strcat_r(prefix, "auth_mode", tmp2)));
			nvram_set(strcat_r(prefix2, "wep_x", tmp), nvram_safe_get(strcat_r(prefix, "wep", tmp2)));
			if (nvram_get_int(strcat_r(prefix, "wep", tmp))) {
				nvram_set(strcat_r(prefix2, "key", tmp), nvram_safe_get(strcat_r(prefix, "key", tmp2)));
				nvram_set(strcat_r(prefix2, "key1", tmp), nvram_safe_get(strcat_r(prefix, "wep_key", tmp2)));
				nvram_set(strcat_r(prefix2, "key2", tmp), nvram_safe_get(strcat_r(prefix, "wep_key", tmp2)));
				nvram_set(strcat_r(prefix2, "key3", tmp), nvram_safe_get(strcat_r(prefix, "wep_key", tmp2)));
				nvram_set(strcat_r(prefix2, "key4", tmp), nvram_safe_get(strcat_r(prefix, "wep_key", tmp2)));
			}
			nvram_set(strcat_r(prefix2, "crypto", tmp), nvram_safe_get(strcat_r(prefix, "crypto", tmp2)));
			nvram_set(strcat_r(prefix2, "wpa_psk", tmp), nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp2)));
		}
#endif
		++i;
	}


	nvram_set("obd_Setting", "1");
}
#endif


int set_wps_enable(int unit){
	int retVal = 0;
	bool orig_enable = 0;
	int enable = nvram_get_int("wps_enable");
	int band = nvram_get_int("wps_band_x");

	_dprintf("%s(%d): enable=%d, band=%d.\n", __func__, unit, enable, band);

	if(unit != band){
		_dprintf("%s(%d): skip.\n", __func__, unit);
		return 0;
	}

	set_wps_idle(unit);

	if(enable != 0 && enable != 1)
		enable = 0;

	retVal = wlan_getWpsEnable(wl_wave_unit(unit), &orig_enable);
	_dprintf("%s(%d): orig_enable=%d.\n", __func__, unit, orig_enable);
	if(orig_enable == enable){
		_dprintf("%s(%d): enable state was not changed.\n", __func__, unit);
		return retVal;
	}

	retVal = wlan_setWpsEnable(wl_wave_unit(unit), enable);
	_dprintf("%s(%d): done. %d.\n", __func__, unit, retVal);

	return retVal;
}

/**
 * Helper of WPS-OOB. Should be call by hostapd_cli.
 * argv[1]:	VAP interface name
 * argv[2]:	event name.  Only WPS-NEW-AP-SETTINGS handled.
 */
int vap_evhandler_main(int argc, char *argv[])
{
	int ret = -2;
	char path[sizeof("/sys/class/net/XXXXXX") + IFNAMSIZ];
	char *vap, *cmd;

	if (argc < 2)
		return -1;

	vap = argv[1];
	cmd = argv[2];
	snprintf(path, sizeof(path), "%s/%s", SYS_CLASS_NET, vap);
	if (!f_exists(path) && !d_exists(path))
		return -1;

	if (!strcmp(cmd, "WPS-NEW-AP-SETTINGS")) {
		sleep(2);
		_ifconfig(vap, 0, NULL, NULL, NULL, 0);
		_ifconfig(vap, IFUP, NULL, NULL, NULL, 0);
		ret = 0;
	}

	return ret;
}
