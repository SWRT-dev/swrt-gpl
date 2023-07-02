/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Copyright 2023, SWRTdev.
 * Copyright 2023, paldier <paldier@hotmail.com>.
 * All Rights Reserved.
 * 
 */
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#ifndef TYPEDEF_FLOAT_T
#include <math.h>
#define TYPEDEF_FLOAT_T
#endif
#include <regex.h>
#include <ctype.h>
#include <shared.h>
#include <shutils.h>
#include <json.h>
#include <webapi.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#ifdef RTCONFIG_CFGSYNC
//#include <cfg_param.h>
#include <cfg_slavelist.h>
#include <cfg_wevent.h>
#include <cfg_event.h>
#include <cfg_lib.h>
#include <cfg_clientlist.h>
#include <cfg_onboarding.h>
#endif
#if defined(RTCONFIG_BWDPI)
#include "bwdpi.h"
#include "sqlite3.h"
#include "bwdpi_sqlite.h"
#endif

struct tcode_location_s {
	int model;
	char *location;
	char *prefix_fmt;
	int idx_base;
#if defined(RTCONFIG_RALINK)
	char *wl0_ccode;
	char *reg_spec;//fcc/cn/ec
	char *wl1_ccode;
	char *regrev1;//null
	char *regrev2;//null
	char *regrev3;//null
#elif defined(RTCONFIG_QCA)
	char *wl0_ccode;
	char *ctl_ccode;
	char *wl1_ccode;
	char *regrev1;
	char *regrev2;
	char *regrev3;
#else
	char *ccode_2g;
	char *regrev_2g;
	char *ccode_5g;
	char *regre_5g;
	char *ccode_5g_2;
	char *regrev_5g_2;
#if defined(GTAXE16000)
	char *ccode_6g;
	char *regrev_6g;
#endif
#if defined(RTAC68U)
	int hw_flag;
#endif
#endif
};

struct JFFS_BACKUP_PROFILE_S jffs_backup_profile_t[] = {
	{"openvpn", "openvpn", "all", "", "restart_openvpnd;restart_chpass", 1},
	{"ipsec", "ca_files", "all", "", "ipsec_restart", 2},
	{"usericon", "usericon", "all", "", "", 3},
	{NULL, NULL, NULL, NULL, NULL, 0},
};
extern const struct tcode_location_s tcode_location_list[];

static int check_ip_cidr(const char *ip, int check_cidr)
{
	unsigned int ip1 = 0;
	unsigned int ip2 = 0;
	unsigned int ip3 = 0;
	unsigned int ip4 = 0;
	unsigned int cidr;
  	char buf[64] = {0};
	sscanf(ip, "%d.%d.%d.%d/%d", &ip1, &ip2, &ip3, &ip4, &cidr);
	snprintf(buf, sizeof(buf), "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
	if(!*ip || !validate_ip(buf))
		return 0;
	if(check_cidr)
		return cidr <= 32;
	return 1;
}

int upload_blacklist_config_cgi(char *blacklist_config)
{
	int ret;
	struct json_object *tmp_obj = NULL;

	tmp_obj = json_tokener_parse(blacklist_config);
	if(tmp_obj ==NULL)
		return HTTP_INVALID_INPUT;
	if(json_object_get_type(tmp_obj) == json_type_array){
		ret = HTTP_OK;
		json_object_to_file("/tmp/blacklist_config.json", tmp_obj);
	}else
		ret = HTTP_INVALID_INPUT;
	json_object_put(tmp_obj);
	return ret;
}

int RCisSupport(const char *name)
{
	if(!strcmp(name, "mssid"))
	{
		if(!nvram_contains_word("rc_support", name) || nvram_get_int("sw_mode") == 2)
			return 0;

		if(nvram_get_int("sw_mode") != 3 || nvram_get_int("wlc_psta") == 0)
			return 1;
	}else if(!strcmp(name, "rrsut")){
		if(nvram_get_int("sw_mode") != 1)
			return 0;
		return nvram_contains_word("rc_support", name) != 0;
	}else if(!strcmp(name, "ipsec_srv"))
		return 4;
	else if(!strcmp(name, "usericon"))
		return 3;
#if defined(RTCONFIG_OOKLA) || defined(RTCONFIG_OOKLA_LITE)
	else if(!strcmp(name, "ookla"))
		return check_if_file_exist("/www/internet_speed.html") != 0;
	else if(!strcmp(name, "AWV_ookla")){
		if(!check_if_file_exist("/www/internet_speed.html"))
			return 0;
		if(!check_if_file_exist("/www/css/internetSpeed_white_theme.css"))
			return 1;
		return 2;
	}
#endif
#if defined(RTCONFIG_BWDPI) && defined(DISABLE)
	else if(!strcmp(name, "bwdpi")){
		if(check_tcode_blacklist())
			return 0;
		return 2;
	}
#endif
	else if(!strcmp(name, "conndiag") || !strcmp(name, "frs_feedback") || !strcmp(name, "dnsfilter"))
		return 2;
	else if(!strcmp(name, "wtfast"))
		return is_CN_sku() == 0;
	return nvram_contains_word("rc_support", name) != 0;
}


int DPIisSupport(const char *name)
{
#if defined(RTCONFIG_BWDPI)
#if defined(RTCONFIG_BWDPI) && defined(DISABLE)
	if(check_tcode_blacklist())
		return 0;
#endif
	if (!strcmp(name, "dpi_mals"))
		return dump_dpi_support(INDEX_MALS);
	if (!strcmp(name, "dpi_vp"))
		return dump_dpi_support(INDEX_VP);
	if (!strcmp(name, "dpi_cc"))
		return dump_dpi_support(INDEX_CC);
	if (!strcmp(name, "adaptive_qos"))
		return dump_dpi_support(INDEX_ADAPTIVE_QOS);
	if (!strcmp(name, "traffic_analyzer"))
		return dump_dpi_support(INDEX_TRAFFIC_ANALYZER);
	if (!strcmp(name, "webs_filter"))
		return dump_dpi_support(INDEX_WEBS_FILTER);
	if (!strcmp(name, "apps_filter"))
		return dump_dpi_support(INDEX_APPS_FILTER);
	if (!strcmp(name, "web_history"))
		return dump_dpi_support(INDEX_WEB_HISTORY);
	if (!strcmp(name, "bandwidth_monitor"))
		return dump_dpi_support(INDEX_BANDWIDTH_MONITOR);
#endif
	return 0;
}

int separate_ssid(const char *model)
{
	if(strstr(model, "GT-") || !strcmp(model, "RT-AX88U") || !strcmp(model, "RT-AX89X"))
		return 1;
	return 0;
}

int totalband(void)
{
	int band = 0;
	char *next;
	char word[256];

  	memset(word, 0, sizeof(word));
	foreach(word, nvram_safe_get("wl_ifnames"), next){
		band++;
	}
	return band;
}

int is_aicloudipk(void)
{
	return 0;
}

int is_concurrep(void)
{
#if defined(RTCONFIG_CONCURRENTREPEATER)
	return strstr(get_productid(), "RP-") != 0;
#else
	return 0;
#endif
}

int is_rp_express_2g(void)
{
#if defined(RTCONFIG_CONCURRENTREPEATER)
	return 1;
#else
	return 0;
#endif
}

int is_rp_express_5g(void)
{
#if defined(RTCONFIG_CONCURRENTREPEATER)
	return 1;
#else
	return 0;
#endif
}


int is_hnd(void)
{
#if defined(RTCONFIG_HND_ROUTER)
	return 1;
#else
	return 0;
#endif
}


int is_localap(void)
{
	int psta = nvram_get_int("wlc_psta");
	int mode = sw_mode();
#if defined(RTCONFIG_BCMARM)
	if(mode == SW_MODE_HOTSPOT || (mode == SW_MODE_REPEATER && psta == 1))
		return 0;
	if(mode == SW_MODE_AP)
		return !psta;
#else
	if(mode == SW_MODE_HOTSPOT || (mode == SW_MODE_REPEATER && psta != 1))
		return 0;
	if(mode == SW_MODE_AP)
		return psta != 1;
#endif
	return 1;
}

int usbPortMax(void)
{
	char *ptr;

	ptr = strstr(nvram_safe_get("rc_support"), "usbX");
	if(ptr)
		return atoi(ptr + 4);
	return 0;
}

int is_usbX(void)
{
	return strstr(nvram_safe_get("rc_support"), "usbX") != NULL;
}

int is_usb3(void)
{
	return *nvram_safe_get("usb_usb3") != 0x0;
}

#if defined(RTCONFIG_BCMWL6) || defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK)
#if defined(MFP) || defined(RTCONFIG_MFP)
int is_wl_mfp(void)
{
	return *nvram_safe_get("wl_mfp") != 0x0;
}
#endif
#endif

#ifdef RTCONFIG_USB_XHCI
int is_wlopmode(void)
{
	return nvram_get_int("wlopmode") == 7;
}
#endif

int is_11AC(void)
{
	if(nvram_contains_word("rc_support", "rawifi") || nvram_contains_word("rc_support", "qcawifi"))
		return nvram_contains_word("rc_support", "11AC");
	else
		return nvram_match("wl1_phytype", "v");

	return 0;
}

int is_yadns(void)
{
	if(nvram_contains_word("rc_support", "yadns") || nvram_contains_word("rc_support", "yadns_hideqis") != 0)
		return 1;
	return 0;
}

int is_RPMesh(void)
{
	return strstr(get_productid(), "RP-") != NULL;
}

int is_noRouter(void)
{
	return is_RPMesh();
}

int is_odm(void)
{
	return nvram_match("odmpid", "GT-AX11000_BO4") != 0;
}


int is_uu_accel(void)
{
#if defined(RTCONFIG_UUPLUGIN)
#if defined(RTCONFIG_SWRT_UU)
	return 1;
#endif
	if(is_CN_sku() && sw_mode() == SW_MODE_ROUTER)
		return 1;
#endif
	return 0;
}


int mssid_count(void)
{
	int count = 0;
	char *next;
	char word[256];

  	memset(word, 0, sizeof(word));
	if(!nvram_contains_word("rc_support", "mssid") || sw_mode() == SW_MODE_REPEATER || (sw_mode() == SW_MODE_AP && nvram_get_int("wlc_psta")))
		return 0;
	foreach(word, nvram_safe_get("wl0_vifnames"), next){
		count++;
	}
	if(count > 3)
		count = 3;

	return count;
}

int is_amas_support(void)
{
#ifdef RTCONFIG_AMAS
	return getAmasSupportMode();
#else
	return 0;
#endif
}

#if defined(RTCONFIG_BWDPI)
int is_AWV_dpi_cc(void)
{
	if (check_if_file_exist("/www/AiProtection_InfectedDevicePreventBlock_m.asp"))
		return dump_dpi_support(INDEX_CC) != 0;
	return 0;
}

int is_AWV_dpi_vp(void)
{
	if(check_if_file_exist("/www/AiProtection_IntrusionPreventionSystem_m.asp"))
		return dump_dpi_support(INDEX_VP) != 0;
	return 0;
}

int is_AWV_dpi_mals(void)
{
	if(check_if_file_exist("/www/AiProtection_MaliciousSitesBlocking_m.asp"))
		return dump_dpi_support(INDEX_MALS) != 0;
	return 0;
}
#endif

int is_AWV_ledg(void)
{
#if defined(RTAX82U) || defined(DSL_AX82U) || defined(GSAX3000) || defined(GSAX5400) || defined(TUFAX5400)
#else
	return 0;
#endif
}

int get_ledg_count(void)
{
#if defined(RTAX82U) || defined(DSL_AX82U) || defined(GSAX3000) || defined(GSAX5400) || defined(TUFAX5400)
#else
	return 0;
#endif
}

int is_LUO_afwupg()
{
	if(strncmp(nvram_safe_get("territory_code"), "CH", 2))
		return nvram_contains_word("rc_support", "afwupg") != 0;
	return 0;
}

int is_LUO_betaupg()
{
	if(strncmp(nvram_safe_get("territory_code"), "CH", 2)){
		if(strncmp(nvram_safe_get("territory_code"), "SG", 2)){
			if(nvram_get_int("SG_mode") != 1)
				return nvram_contains_word("rc_support", "betaupg") != 0;
		}
	}
	return 0;
}

int is_LUO_revertfw()
{
	if(strncmp(nvram_safe_get("territory_code"), "CH", 2)){
		if(strncmp(nvram_safe_get("territory_code"), "SG", 2)){
			if(nvram_get_int("SG_mode") != 1)
				return nvram_contains_word("rc_support", "revertfw") != 0;
		}
	}
	return 0;
}

int is_noFwManual()
{
	if(nvram_get_int("noFwManual") == 1 || nvram_contains_word("rc_support", "noFwManual"))
		return 1;
	if(!strncmp(nvram_safe_get("territory_code"), "CH", 2))
		return 1;
	return nvram_get_int("SG_mode") == 1;
}

int is_noUpdate()
{
	if(nvram_get_int("noupdate") == 1)
		return 1;
	return nvram_contains_word("rc_support", "noupdate") != 0;
}

int is_AWV_vpnc()
{
	if(check_if_file_exist("/www/VPN/vpnc.html")){
		if(check_if_file_exist("/www/VPN/vpncWHITE.css"))
			return 2;
		return 1;
	}
	return 0;
}

int is_AWV_vpns()
{
	if(check_if_file_exist("/www/VPN/vpns.html")){
		if(check_if_file_exist("/www/VPN/vpnsWHITE.css"))
			return 2;
		return 1;
	}
	return 0;
}

int is_wpa3rp(void)
{
	return nvram_contains_word("rc_support", "wpa3") != 0;
}

int is_CN_Boost_support()
{
	int boost = 0;
	int model = get_model();
	const struct tcode_location_s *tptr = NULL;
	char tcode[5] = {0};

	strlcpy(tcode, nvram_safe_get("territory_code"), sizeof(tcode));
	if(!strncmp(tcode, "CN", 2) || !strncmp(tcode, "AA", 2) || !strncmp(tcode, "AU", 2) || !strncmp(tcode, "IN", 2))
		boost = 1;
	if(!strncmp(tcode, "KR", 2) || !strncmp(tcode, "HK", 2) || !strncmp(tcode, "SG", 2) || !strncmp(tcode, "GD", 2))
		boost = 1;
	if(!strncmp(tcode, "TC", 2) || !strncmp(tcode, "CT", 2))
		boost = 1;
	if(nvram_contains_word("rc_support", "loclist"))
		++boost;
 	for(tptr = tcode_location_list; tptr->model; tptr++){
		if(tptr->model == model){
			if(!strncmp(tptr->location, "XX", 2)){
				return boost == 2;
			}
		}
	}		
	return 0;
}

int acs_dfs_support()
{
	return 0;
}

int get_ui_support_info(struct json_object *ui_support_obj)
{
	char *next;
	char word[256];
	char *list[] = {"dpi_mals", "dpi_vp", "dpi_cc", "adaptive_qos", "traffic_analyzer", "webs_filter", "apps_filter", "web_history", "bandwidth_monitor"};
	int i;
#if defined(RTCONFIG_AMAS)
	int amasmode, amasRouter, cfgsync;
#endif
	struct json_object *ax_support = NULL;

  	memset(word, 0, sizeof(word));
	foreach(word, nvram_safe_get("rc_support"), next){
		json_object_object_add(ui_support_obj, word, json_object_new_int(RCisSupport(word)));
	}
	for(i = 0; i < 9; i++)
	{
		json_object_object_add(ui_support_obj, list[i], json_object_new_int(DPIisSupport(list[i])));
	}
	json_object_object_add(ui_support_obj, "aicloudipk", json_object_new_int(is_aicloudipk()));
	json_object_object_add(ui_support_obj, "concurrep", json_object_new_int(is_concurrep()));
	json_object_object_add(ui_support_obj, "rp_express_2g", json_object_new_int(is_rp_express_2g()));
	json_object_object_add(ui_support_obj, "rp_express_5g", json_object_new_int(is_rp_express_5g()));
	json_object_object_add(ui_support_obj, "hnd", json_object_new_int(is_hnd()));
	json_object_object_add(ui_support_obj, "localap", json_object_new_int(is_localap()));
	json_object_object_add(ui_support_obj, "nwtool", json_object_new_int(1));
	json_object_object_add(ui_support_obj, "usbPortMax", json_object_new_int(usbPortMax()));
	json_object_object_add(ui_support_obj, "usbX", json_object_new_int(is_usbX()));
	json_object_object_add(ui_support_obj, "usb3", json_object_new_int(is_usb3()));
#if defined(RTCONFIG_BCMWL6) || defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK)
#if defined(MFP) || defined(RTCONFIG_MFP)
	json_object_object_add(ui_support_obj, "wl_mfp", json_object_new_int(is_wl_mfp()));
#endif
#ifdef RTCONFIG_USB_XHCI
	json_object_object_add(ui_support_obj, "wlopmode", json_object_new_int(is_wlopmode()));
#endif
#endif
	json_object_object_add(ui_support_obj, "11AC", json_object_new_int(is_11AC()));
	json_object_object_add(ui_support_obj, "yadns", json_object_new_int(is_yadns()));
	json_object_object_add(ui_support_obj, "noRouter", json_object_new_int(is_noRouter()));
	json_object_object_add(ui_support_obj, "RPMesh", json_object_new_int(is_RPMesh()));
	json_object_object_add(ui_support_obj, "eula", json_object_new_int(1));
	json_object_object_add(ui_support_obj, "odm", json_object_new_int(is_odm()));
	json_object_object_add(ui_support_obj, "dualband", json_object_new_int(totalband() == 2));
	json_object_object_add(ui_support_obj, "triband", json_object_new_int(totalband() == 3));
	json_object_object_add(ui_support_obj, "quadband", json_object_new_int(totalband() == 4));
	json_object_object_add(ui_support_obj, "separate_ssid", json_object_new_int(separate_ssid(get_productid())));
	json_object_object_add(ui_support_obj, "mssid_count", json_object_new_int(mssid_count()));
	json_object_object_add(ui_support_obj, "dhcp_static_dns", json_object_new_int(1));
#if defined(RTCONFIG_OOKLA)
	json_object_object_add(ui_support_obj, "AWV_ookla", json_object_new_int(RCisSupport("AWV_ookla")));
#endif
#if defined(RTCONFIG_BCMARM)
	json_object_object_add(ui_support_obj, "acs_dfs", json_object_new_int((strtol(nvram_safe_get("wl1_band5grp"), NULL, 16) & 6) == 6));
#if defined(RTCONFIG_BCM_7114)
	json_object_object_add(ui_support_obj, "sdk7114", json_object_new_int(1));
#elif defined(RTAC3200)
	json_object_object_add(ui_support_obj, "sdk7", json_object_new_int(1));
#endif
#endif
	json_object_object_add(ui_support_obj, "wanMax", json_object_new_int(2));
#if defined(RTCONFIG_OPEN_NAT)
	json_object_object_add(ui_support_obj, "open_nat", json_object_new_int(1));
#endif
	json_object_object_add(ui_support_obj, "uu_accel", json_object_new_int(is_uu_accel()));
#if defined(RTCONFIG_AMAS_WGN)
	json_object_object_add(ui_support_obj, "amas_wgn", json_object_new_int(1));
#endif
#if defined(RTCONFIG_INTERNETCTRL)
	json_object_object_add(ui_support_obj, "internetctrl", json_object_new_int(1));
#endif
#if defined(RTCONFIG_PRELINK)
	json_object_object_add(ui_support_obj, "prelink", json_object_new_int(1));
	json_object_object_add(ui_support_obj, "prelink_mssid", json_object_new_int(1));
	json_object_object_add(ui_support_obj, "prelink_unit", json_object_new_int(num_of_wl_if() - 1));
#endif
#if defined(RTCONFIG_BWDPI)
	json_object_object_add(ui_support_obj, "AWV_dpi_cc", json_object_new_int(is_AWV_dpi_cc()));
	json_object_object_add(ui_support_obj, "AWV_dpi_vp", json_object_new_int(is_AWV_dpi_vp()));
	json_object_object_add(ui_support_obj, "AWV_dpi_mals", json_object_new_int(is_AWV_dpi_mals()));
#endif
#if defined(RTCONFIG_BCN_RPT)//386
	json_object_object_add(ui_support_obj, "force_roaming", json_object_new_int(1));
	json_object_object_add(ui_support_obj, "sta_ap_bind", json_object_new_int(1));
	json_object_object_add(ui_support_obj, "re_reconnect", json_object_new_int(1));
	json_object_object_add(ui_support_obj, "amas_eap", json_object_new_int(1));
#endif
	json_object_object_add(ui_support_obj, "del_client_data", json_object_new_int(1));
#if defined(RTCONFIG_CAPTCHA)
	json_object_object_add(ui_support_obj, "captcha", json_object_new_int(2));
#endif
	json_object_object_add(ui_support_obj, "ledg", json_object_new_int(is_AWV_ledg()));
	json_object_object_add(ui_support_obj, "AWV_ledg", json_object_new_int(is_AWV_ledg()));
	json_object_object_add(ui_support_obj, "ledg_count", json_object_new_int(get_ledg_count()));
	json_object_object_add(ui_support_obj, "AWV_vpnc", json_object_new_int(is_AWV_vpnc()));
	json_object_object_add(ui_support_obj, "AWV_vpns", json_object_new_int(is_AWV_vpns()));
#if defined(RTCONFIG_AUTO_FW_UPGRADE)
	json_object_object_add(ui_support_obj, "afwupg", json_object_new_int(is_LUO_afwupg()));
	json_object_object_add(ui_support_obj, "betaupg", json_object_new_int(is_LUO_betaupg()));
	json_object_object_add(ui_support_obj, "revertfw", json_object_new_int(is_LUO_revertfw()));
	json_object_object_add(ui_support_obj, "noFwManual", json_object_new_int(is_noFwManual()));
	json_object_object_add(ui_support_obj, "noupdate", json_object_new_int(is_noUpdate()));
#endif
	json_object_object_add(ui_support_obj, "WL_SCHED_V2", json_object_new_int(1));
#if defined(RTCONFIG_PC_SCHED_V3)
	json_object_object_add(ui_support_obj, "PC_SCHED_V3", json_object_new_int(2));
#else
	json_object_object_add(ui_support_obj, "PC_SCHED_V2", json_object_new_int(1));
#endif
#if defined(RTCONFIG_WL_SCHED_V3)
	json_object_object_add(ui_support_obj, "WL_SCHED_V3", json_object_new_int(3));
#endif
	json_object_object_add(ui_support_obj, "wpa3rp", json_object_new_int(is_wpa3rp()));
#if defined(RTCONFIG_AMAS)
	json_object_object_add(ui_support_obj, "led_ctrl_cap", json_object_new_int(nvram_get_int("led_ctrl_cap")));
#endif
#if defined(RTCONFIG_BWDPI)
	if(dump_dpi_support(INDEX_ADAPTIVE_QOS)/* && !check_tcode_blacklist()*/)
    	json_object_object_add(ui_support_obj, "mobile_game_mode", json_object_new_int(1));
#endif
#if defined(RTCONFIG_INSTANT_GUARD)
	json_object_object_add(ui_support_obj, "Instant_Guard", json_object_new_int(4));
#endif
#if defined(RTCONFIG_SCHED_V3)
	json_object_object_add(ui_support_obj, "MaxRule_bwdpi_wrs", json_object_new_int(64));
	json_object_object_add(ui_support_obj, "MaxRule_parentctrl", json_object_new_int(64));
	json_object_object_add(ui_support_obj, "MaxRule_PC_DAYTIME", json_object_new_int((get_nvram_dlen("MULTIFILTER_MACFILTER_DAYTIME_V2") / 13) < 256 ? : 256));
#ifdef RTCONFIG_PC_REWARD
	json_object_object_add(ui_support_obj, "PC_REWARD", json_object_new_int(1));
#endif
	json_object_object_add(ui_support_obj, "MaxRule_extend_limit", json_object_new_int(64));
	json_object_object_add(ui_support_obj, "MaxLen_http_name", json_object_new_int(32));
	json_object_object_add(ui_support_obj, "MaxLen_http_passwd", json_object_new_int(32));
#endif
	json_object_object_add(ui_support_obj, "CHPASS", json_object_new_int(1));
	json_object_object_add(ui_support_obj, "MaxRule_VPN_FUSION_Conn", json_object_new_int(nvram_get_int("vpnc_max_conn")));
#if defined(RTCONFIG_AMAS)
	if (nvram_contains_word("rc_support", "amas"))
	{
		amasmode = getAmasSupportMode();
		if ((amasmode & 0xFFFFFFFD) == 1)
			json_object_object_add(ui_support_obj, "amasRouter", json_object_new_int(1));
		if ((amasmode - 2) > 1)
		{
			if (!amasmode)
			{
				json_object_object_add(ui_support_obj, "amas", json_object_new_int(0));
				goto noamas;
			}
		}
		else
			json_object_object_add(ui_support_obj, "amasNode", json_object_new_int(1));
		json_object_object_add(ui_support_obj, "amas", json_object_new_int(1));
		if (nvram_match("sw_mode", "1") || (nvram_match("sw_mode", "3") && !nvram_get_int("wlc_psta")))
			cfgsync = 1;
		else
noamas:
			cfgsync = 0;
		json_object_object_add(ui_support_obj, "cfg_sync", json_object_new_int(cfgsync));
	}
#endif
#if defined(RTCONFIG_INADYN)
	json_object_object_add(ui_support_obj, "inadyn", json_object_new_int(1));
#endif
#if defined(RTCONFIG_GOOGLE_ASST)
	json_object_object_add(ui_support_obj, "google_asst", json_object_new_int(1));
#endif
#if defined(RTCONFIG_HND_ROUTER) || defined(RTCONFIG_SWRT_FULLCONE) || defined(RTCONFIG_SWRT_FULLCONEV2)
	json_object_object_add(ui_support_obj, "fullcone", json_object_new_int(1));
#endif
#if defined(RTCONFIG_NEW_PHYMAP)
	json_object_object_add(ui_support_obj, "NEW_PHYMAP", json_object_new_int(1));
#endif
	json_object_object_add(ui_support_obj, "FAMILY_GROUP", json_object_new_int(1));
#if defined(RTCONFIG_ACCOUNT_BINDING)
	json_object_object_add(ui_support_obj, "account_binding", json_object_new_int(2));
#endif
#if defined(RTCONFIG_AMAS)
	json_object_object_add(ui_support_obj, "nt_center", json_object_new_int(5));
	json_object_object_add(ui_support_obj, "nt_center_ui", json_object_new_int(0));
	json_object_object_add(ui_support_obj, "wps_method_ob", json_object_new_int(1));
#endif
	if(json_object_object_get_ex(ui_support_obj, "11AX", &ax_support))
	{
		json_object_object_add(ui_support_obj, "qis_hide_he_features", json_object_new_int((!strcmp(get_productid(), "RT-AX92U") && nvram_get_int("amas_bdl") == 1) ? 1 : 0));
	}
	if(!strncmp(nvram_safe_get("territory_code"), "CH", 2))
		json_object_object_add(ui_support_obj, "v6only", json_object_new_int(1));
	if(nvram_get_int("CoBrand")){
		char file[128];
		memset(file, 0, sizeof(file));
		snprintf(file, sizeof(file), "images/Model_product_%d.png", nvram_get_int("CoBrand"));
		json_object_object_add(ui_support_obj, "cobrand_change", json_object_new_int(check_if_file_exist(file) != 0));
	}else
		json_object_object_add(ui_support_obj, "cobrand_change", json_object_new_int(0));
	json_object_object_add(ui_support_obj, "wlband", json_object_new_int(1));
	json_object_object_add(ui_support_obj, "profile_sync", json_object_new_int(0));
	json_object_object_add(ui_support_obj, "CN_Boost", json_object_new_int(is_CN_Boost_support()));
	json_object_object_add(ui_support_obj, "5g1_dfs", json_object_new_int(acs_dfs_support()));
	json_object_object_add(ui_support_obj, "5g2_dfs", json_object_new_int(acs_dfs_support()));
	json_object_object_add(ui_support_obj, "cd_iperf", json_object_new_int(0));
	json_object_object_add(ui_support_obj, "rwd_mapping", json_object_new_int(0));
	return 0;
}

int set_wireguard_server(struct json_object *wireguard_server_obj, int *wgsc_idx)
{
	int idx = 1;
	int ip1, ip2, ip3, ip4, netmask, wgs_port;
	char prefix[16] = {0}, prefix_client[16] = {0};
	char wgs_enable[2] = {0}, wgs_addr[64] = {0}, wgs_ipaddr[64] = {0}, wgs_alive[6] = {0}, wgs_dns[2] = {0};
	char wgs_nat6[2] = {0}, wgs_psk[2] = {0}, wgs_priv[64] = {0}, wgs_pub[64] = {0}, wgs_lanaccess[2] = {0};
	char wgsc_enable[2] = {0}, wgsc_addr[64] = {0}, wgsc_aips[1024] = {0}, wgsc_caips[1024] = {0};
	char wgsc_priv[64] = {0}, wgsc_pub[64] = {0}, wgsc_psk[64] = {0}, wgsc_name[64] = {0};
	char *pt;
	struct json_object *tmp_obj;
	snprintf(prefix, sizeof(prefix), "%s%d_", "wgs", 1);
	while(1){
		snprintf(prefix_client, sizeof(prefix_client), "%sc%d_", prefix, idx);
		pt = nvram_pf_get(prefix_client, "enable");
		if(pt == NULL || *pt == '0' || *pt == 0)
			break;
		if(++idx == 11)
			return HTTP_OVER_MAX_RULE_LIMIT;
	}
	if(json_object_object_get_ex(wireguard_server_obj, "wgs_enable", &tmp_obj)){
		strlcpy(wgs_enable, json_object_get_string(tmp_obj), sizeof(wgs_enable));
		if(!isValidEnableOption(wgs_enable, 1))
			return HTTP_INVALID_ENABLE_OPT;
	}else
		strlcpy(wgs_enable, nvram_pf_safe_get(prefix, "enable"), sizeof(wgs_enable));
	if(json_object_object_get_ex(wireguard_server_obj, "wgs_addr", &tmp_obj)){
		strlcpy(wgs_addr, json_object_get_string(tmp_obj), sizeof(wgs_addr));
		if(strcmp(wgs_addr, nvram_pf_safe_get(prefix, "addr"))){
			sscanf(wgs_addr, "%d.%d.%d.%d/%d", &ip1, &ip2, &ip3, &ip4, &netmask);
			snprintf(wgs_ipaddr, sizeof(wgs_ipaddr), "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
			if(!validate_ip(wgs_ipaddr) || is_ip4_in_use(wgs_ipaddr) || netmask > 32)
				return HTTP_INVALID_IPADDR;
		}
	}else
		strlcpy(wgs_addr, nvram_pf_safe_get(prefix, "addr"), sizeof(wgs_addr));
	if(json_object_object_get_ex(wireguard_server_obj, "wgs_port", &tmp_obj)){
		wgs_port = strtoul(json_object_get_string(tmp_obj), NULL, 10);
		if(nvram_pf_get_int(prefix, "port") != wgs_port && (is_port_in_use(wgs_port) || wgs_port > 65535))
			return HTTP_INVALID_INPUT;
	}else
		wgs_port = nvram_pf_get_int(prefix, "port");
	if(json_object_object_get_ex(wireguard_server_obj, "wgs_alive", &tmp_obj)){
		strlcpy(wgs_alive, json_object_get_string(tmp_obj), sizeof(wgs_alive));
		if(!isValid_digit_string(wgs_alive) || safe_atoi(wgs_alive) < 0 || safe_atoi(wgs_alive) > 99999){
			return HTTP_INVALID_INPUT;
	}else
		strlcpy(wgs_alive, nvram_pf_safe_get(prefix, "alive"), sizeof(wgs_alive));
	if(json_object_object_get_ex(wireguard_server_obj, "wgs_dns", &tmp_obj)){
		strlcpy(wgs_dns, json_object_get_string(tmp_obj), sizeof(wgs_dns));
		if(!isValidEnableOption(wgs_dns, 1)
			return HTTP_INVALID_ENABLE_OPT;
	}else
		strlcpy(wgs_dns, nvram_pf_safe_get(prefix, "dns"), sizeof(wgs_dns));
	if(json_object_object_get_ex(wireguard_server_obj, "wgs_nat6", &tmp_obj)){
		strlcpy(wgs_nat6, json_object_get_string(tmp_obj), sizeof(wgs_nat6));
		if(!isValidEnableOption(wgs_nat6, 1))
			return HTTP_INVALID_ENABLE_OPT;
	}else
		strlcpy(wgs_nat6, nvram_pf_safe_get(prefix, "nat6"), sizeof(wgs_nat6));
	if(json_object_object_get_ex(wireguard_server_obj, "wgs_psk", &tmp_obj)){
		strlcpy(wgs_psk, json_object_get_string(tmp_obj), sizeof(wgs_psk));
		if(!isValidEnableOption(wgs_psk, 1))
			return HTTP_INVALID_ENABLE_OPT;
	}else
		strlcpy(wgs_psk, nvram_pf_safe_get(prefix, "psk"), sizeof(wgs_psk));
	if(json_object_object_get_ex(wireguard_server_obj, "wgs_priv", &tmp_obj))
		strlcpy(wgs_priv, json_object_get_string(tmp_obj), sizeof(wgs_priv));
	else
		strlcpy(wgs_priv, nvram_pf_safe_get(prefix, "priv"), sizeof(wgs_priv));
	if(json_object_object_get_ex(wireguard_server_obj, "wgs_pub", &tmp_obj))
		strlcpy(wgs_pub, json_object_get_string(tmp_obj), sizeof(wgs_pub));
	else
		strlcpy(wgs_pub, nvram_pf_safe_get(prefix, "pub"), sizeof(wgs_pub));
	if(json_object_object_get_ex(wireguard_server_obj, "wgs_lanaccess", &tmp_obj)){
		strlcpy(wgs_lanaccess, json_object_get_string(tmp_obj), sizeof(wgs_lanaccess));
		if(!isValidEnableOption(wgs_lanaccess, 1))
			return HTTP_INVALID_ENABLE_OPT;
	}else
		strlcpy(wgs_lanaccess, nvram_pf_safe_get(prefix, "lanaccess"), sizeof(wgs_lanaccess));
	if(json_object_object_get_ex(wireguard_server_obj, "wgsc_enable", &tmp_obj)){
		strlcpy(wgsc_enable, json_object_get_string(tmp_obj), sizeof(wgsc_enable));
		if(!isValidEnableOption(wgsc_enable, 1))
			return HTTP_INVALID_ENABLE_OPT;
	}
	if(json_object_object_get_ex(wireguard_server_obj, "wgsc_addr", &tmp_obj)){
		strlcpy(wgsc_addr, json_object_get_string(tmp_obj), sizeof(wgsc_addr));
		sscanf(wgsc_addr, "%d.%d.%d.%d/%d", &ip1, &ip2, &ip3, &ip4, &netmask);
		snprintf(wgs_ipaddr, sizeof(wgs_ipaddr), "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
		if(!validate_ip(wgs_ipaddr) || is_ip4_in_use(wgs_ipaddr) || netmask > 32)
			return HTTP_INVALID_IPADDR;
	}
	if(json_object_object_get_ex(wireguard_server_obj, "wgsc_aips", &tmp_obj))
		strlcpy(wgsc_aips, json_object_get_string(tmp_obj), sizeof(wgsc_aips));
	if(json_object_object_get_ex(wireguard_server_obj, "wgsc_caips", &tmp_obj))
		strlcpy(wgsc_caips, json_object_get_string(tmp_obj), sizeof(wgsc_caips));
	if(json_object_object_get_ex(wireguard_server_obj, "wgsc_priv", &tmp_obj))
		strlcpy(wgsc_priv, json_object_get_string(tmp_obj), sizeof(wgsc_priv));
	if(json_object_object_get_ex(wireguard_server_obj, "wgsc_pub", &tmp_obj))
		strlcpy(wgsc_pub, json_object_get_string(tmp_obj), sizeof(wgsc_pub));
	if(json_object_object_get_ex(wireguard_server_obj, "wgsc_psk", &tmp_obj))
		strlcpy(wgsc_psk, json_object_get_string(tmp_obj), sizeof(wgsc_psk));
	if(json_object_object_get_ex(wireguard_server_obj, "wgsc_name", &tmp_obj))
		strlcpy(wgsc_name, json_object_get_string(tmp_obj), sizeof(wgsc_name));
	*wgsc_idx = idx;

	if(strcmp(wgs_enable, nvram_pf_safe_get(prefix, "enable")))
		nvram_pf_set(prefix, "enable", wgs_enable);
	if(strcmp(wgs_addr, nvram_pf_safe_get(prefix, "addr")))
		nvram_pf_set(prefix, "addr", wgs_addr);
	if(nvram_pf_get_int(prefix, "port") != wgs_port)
		nvram_pf_set_int(prefix, "port", port);
	if(strcmp(wgs_alive, nvram_pf_safe_get(prefix, "alive")))
		nvram_pf_set(prefix, "alive", wgs_alive);
	if(strcmp(wgs_dns, nvram_pf_safe_get(prefix, "dns")))
		nvram_pf_set(prefix, "dns", wgs_dns);
	if(strcmp(wgs_nat6, nvram_pf_safe_get(prefix, "nat6")))
		nvram_pf_set(prefix, "nat6", wgs_nat6);
	if(strcmp(wgs_psk, nvram_pf_safe_get(prefix, "psk")))
		nvram_pf_set(prefix, "psk", wgs_psk);
	if(strcmp(wgs_priv, nvram_pf_safe_get(prefix, "priv")))
		nvram_pf_set(prefix, "priv", wgs_priv);
	if(strcmp(wgs_pub, nvram_pf_safe_get(prefix, "pub")))
		nvram_pf_set(prefix, "pub", wgs_pub);
	if(strcmp(wgs_lanaccess, nvram_pf_safe_get(prefix, "lanaccess")))
		nvram_pf_set(prefix, "lanaccess", wgs_lanaccess);
	nvram_pf_set(prefix_client, "enable", wgsc_enable);
	nvram_pf_set(prefix_client, "addr", wgsc_addr);
	nvram_pf_set(prefix_client, "aips", wgsc_aips);
	nvram_pf_set(prefix_client, "caips", wgsc_caips);
	nvram_pf_set(prefix_client, "priv", wgsc_priv);
	nvram_pf_set(prefix_client, "pub", wgsc_pub);
	nvram_pf_set(prefix_client, "psk", wgsc_psk);
	nvram_pf_set(prefix_client, "name", wgsc_name);
	nvram_pf_set(prefix_client, "caller", "AMSAPP");
	httpd_nvram_commit();
	if(json_object_object_get_ex(wireguard_server_obj, "do_rc", &tmp_obj)){
		if(!strcmp(json_object_get_string(tmp_obj), "1"))
			notify_rc("restart_wgs;restart_dnsmasq");
	}
	return HTTP_OK;
}
int set_wireguard_client(struct json_object *wireguard_client_obj, int *wgc_idx)
{
	int vpn_count = 0, unit = 0, vpn_client[OVPN_CLIENT_MAX] = {0}, index = 0, idx;
	int ip1, ip2, ip3, ip4;
	char word[1024] = {0}, vpnc_clientlist[CKN_STR8192] = {0}, word_tmp[1024] = {0}, vpn_name[16], vpn_unit[8];
	char prefix[16] = {0}, tmp[256], cmd[128], vpnc_pptp_options_x_list[2048] = {0};
	char wgc_name[64] = {0}, wgc_priv[64] = {0}, wgc_psk[64] = {0}, wgc_ppub[64] = {0}, wgc_enable[2] = {0};
	char wgc_nat[2] = {0}, wgc_addr[64] = {0}, wgc_ep_addr[64] = {0}, wgc_aips[64] = {0}, wgc_dns[128] = {0};
	char wgc_ep_port[6] = {0}, wgc_alive[6] = {0}, wgc_use_tnl[2] = {0}, wgc_ep_device_id[256] = {0};
	char wgc_ep_area[256] = {0};
	char dhcp_start[64] = {0}, dhcp_end[64] = {0}, lanip[64] = {0};
	char *next;
	struct json_object *tmp_obj;
	if(json_object_get_type(wireguard_client_obj) != json_type_object)
		return HTTP_INVALID_INPUT;
	strlcpy(vpnc_clientlist, nvram_safe_get("vpnc_clientlist"), sizeof(vpnc_clientlist));
	strlcpy(vpnc_pptp_options_x_list, nvram_safe_get("vpnc_pptp_options_x_list"), sizeof(vpnc_clientlist));
	foreach_60(word, vpnc_clientlist, next){
		strlcpy(word_tmp, word, sizeof(word_tmp));
		get_string_in_62(word_tmp, 1, vpn_name, sizeof(vpn_name));
		if(!strcmp(vpn_name, "WireGuard")){
			get_string_in_62(word, 2, &vpn_unit, sizeof(vpn_unit));
			vpn_count++;
			if(atoi(vpn_unit))
				vpn_client[atoi(vpn_unit) - 1] = 1;
		}
		unit++;
	}
	if(vpn_count >= OVPN_CLIENT_MAX)
		return HTTP_OVER_MAX_RULE_LIMIT;
	index = _get_new_vpnc_index();
	if(index == 0)
		return HTTP_OVER_MAX_RULE_LIMIT;
	for(idx = 0; idx < OVPN_CLIENT_MAX; idx++){
		if(vpn_client[idx] == 0){
			snprintf(prefix, sizeof(prefix), "wgc%d_", idx + 1);
			break;
		}
	}
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_name", &tmp_obj))
		strlcpy(wgc_name, json_object_get_string(tmp_obj), sizeof(wgc_name));
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_priv", &tmp_obj))
		strlcpy(wgc_priv, json_object_get_string(tmp_obj), sizeof(wgc_priv));
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_psk", &tmp_obj))
		strlcpy(wgc_psk, json_object_get_string(tmp_obj), sizeof(wgc_psk));
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_ppub", &tmp_obj))
		strlcpy(wgc_ppub, json_object_get_string(tmp_obj), sizeof(wgc_ppub));
	if(wgc_name[0] == 0 || wgc_priv[0] == 0 || wgc_ppub[0] == 0)
		return HTTP_INVALID_INPUT;
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_enable", &tmp_obj))
		strlcpy(wgc_enable, json_object_get_string(tmp_obj), sizeof(wgc_enable));
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_nat", &tmp_obj))
		strlcpy(wgc_nat, json_object_get_string(tmp_obj), sizeof(wgc_nat));
	if(!isValidEnableOption(wgc_enable, 1) || !isValidEnableOption(wgc_nat, 1))
		return HTTP_INVALID_ENABLE_OPT;
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_addr", &tmp_obj))
		strlcpy(wgc_addr, json_object_get_string(tmp_obj), sizeof(wgc_addr));
	if(!check_ip_cidr(wgc_addr, 1))
		return HTTP_INVALID_IPADDR;
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_ep_addr", &tmp_obj))
		strlcpy(wgc_ep_addr, json_object_get_string(tmp_obj), sizeof(wgc_ep_addr));
	if(!check_ip_cidr(wgc_ep_addr, 0))
		return HTTP_INVALID_IPADDR;
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_aips", &tmp_obj))
		strlcpy(wgc_aips, json_object_get_string(tmp_obj), sizeof(wgc_aips));
	if(!check_ip_cidr(wgc_aips, 1))
		return HTTP_INVALID_IPADDR;
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_dns", &tmp_obj))
		strlcpy(wgc_dns, json_object_get_string(tmp_obj), sizeof(wgc_dns));
	if(wgc_dns[0]){
		foreach_59(word, wgc_dns, next){
			if(!validate_ip(word))
				return HTTP_INVALID_IPADDR;
		}
	}
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_ep_port", &tmp_obj))
		strlcpy(wgc_ep_port, json_object_get_string(tmp_obj), sizeof(wgc_ep_port));
	if(!isValid_digit_string(wgc_ep_port) || safe_atoi(wgc_ep_port) < 0 || safe_atoi(wgc_ep_port) > 65535)
		return HTTP_INVALID_INPUT;
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_alive", &tmp_obj))
		strlcpy(wgc_alive, json_object_get_string(tmp_obj), sizeof(wgc_alive));
	if(!isValid_digit_string(wgc_alive) || safe_atoi(wgc_alive) < 0 || safe_atoi(wgc_alive) > 99999)
		return HTTP_INVALID_INPUT;
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_use_tnl", &tmp_obj))
		strlcpy(wgc_use_tnl, json_object_get_string(tmp_obj), sizeof(wgc_use_tnl));
	if(!isValidEnableOption(wgc_use_tnl, 1))
		return HTTP_INVALID_ENABLE_OPT;
	if(vpnc_clientlist[0])
		strlcat(vpnc_clientlist, "<", sizeof(vpnc_clientlist));
	snprintf(tmp, sizeof(tmp), "%s>WireGuard>%d>>>1>%d>>>%s>0>AMSAPP", wgc_name, (idx + 1), index, wgc_use_tnl);
	strlcat(vpnc_clientlist, tmp, sizeof(vpnc_clientlist));
	*wgc_idx = idx + 1;
	if(json_object_object_get_ex(wireguard_client_obj, "change_lanip", &tmp_obj)){
		strlcpy(lanip, json_object_get_string(tmp_obj), sizeof(lanip));
		if(!validate_ip(lanip))
			return HTTP_INVALID_IPADDR;
		sscanf(lanip, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
		snprintf(dhcp_start, sizeof(dhcp_start), "%d.%d.%d.2", ip1, ip2, ip3);
		snprintf(dhcp_end, sizeof(dhcp_end), "%d.%d.%d.254", ip1, ip2, ip3);
	}
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_ep_device_id", &tmp_obj)){
		char *pt;
		strlcpy(wgc_ep_device_id, json_object_get_string(tmp_obj), sizeof(wgc_ep_device_id));
		pt = wgc_ep_device_id;
		while(*pt){
			if(!isalnum(*pt))
				return HTTP_INVALID_INPUT;
			pt++;
		}
    }
	if(json_object_object_get_ex(wireguard_client_obj, "wgc_ep_area", &tmp_obj))
		strlcpy(wgc_ep_area, json_object_get_string(tmp_obj), sizeof(wgc_ep_area));
	nvram_pf_set(prefix, "enable", wgc_enable);
	nvram_pf_set(prefix, "nat", wgc_nat);
	nvram_pf_set(prefix, "name", wgc_name);
	nvram_pf_set(prefix, "priv", wgc_priv);
	nvram_pf_set(prefix, "psk", wgc_psk);
	nvram_pf_set(prefix, "ppub", wgc_ppub);
	nvram_pf_set(prefix, "addr", wgc_addr);
	nvram_pf_set(prefix, "ep_addr", wgc_ep_addr);
	nvram_pf_set(prefix, "aips", wgc_aips);
	nvram_pf_set(prefix, "dns", wgc_dns);
	nvram_pf_set(prefix, "ep_port", wgc_ep_port);
	nvram_pf_set(prefix, "alive", wgc_alive);
	nvram_pf_set(prefix, "use_tnl", wgc_use_tnl);
	nvram_pf_set(prefix, "ep_device_id", wgc_ep_device_id);
	nvram_pf_set(prefix, "ep_area", wgc_ep_area);
	strlcat(cmd, "restart_vpnc;", sizeof(cmd));
	if(lanip[0])){
		nvram_set("lan_ipaddr", lanip);
		nvram_set("lan_ipaddr_rt", lanip);
		nvram_set("dhcp_start", dhcp_start);
		nvram_set("dhcp_end", dhcp_end);
		strlcat(cmd, "restart_net_and_phy;", sizeof(cmd));
	}
	nvram_set("vpnc_clientlist", vpnc_clientlist);
	strlcat(vpnc_pptp_options_x_list, "<auto", sizeof(vpnc_pptp_options_x_list));
	nvram_set("vpnc_pptp_options_x_list", vpnc_pptp_options_x_list);
	nvram_set_int("vpnc_unit", unit);
	httpd_nvram_commit();
	if(json_object_object_get_ex(wireguard_client_obj, "do_rc", &tmp_obj)){
		if(!strcmp(json_object_get_string(tmp_obj), "1"))
			notify_rc(cmd);
	}
	return HTTP_OK;
}

int gen_jffs_backup_profile(char *name, char *file_path)
{
	char cmd[1024] = {0}, path[64] = {0};
	struct JFFS_BACKUP_PROFILE_S *p;
	for(p = jffs_backup_profile_t; p->name; p++){
		if(!strcmp(name, p->name)){
			snprintf(path, sizeof(path), "/jffs/%s", p->folder);
			if(check_if_dir_exist(path)){
				snprintf(cmd, sizeof(cmd), "echo '%s' >> %s", p->exclude, "/jffs/exclude_lists");
				system(cmd);
				snprintf(cmd, sizeof(cmd), "tar czvf %s -C /jffs -X %s %s", file_path, "/jffs/exclude_lists", p->folder);
				system(cmd);
				unlink("/jffs/exclude_lists");
				return HTTP_OK;
			}
		}
	}
	return HTTP_FAIL;
}

int upload_jffs_profile(char *name, int do_rc)
{
	int ret = HTTP_FAIL;
	char upload_rc[32] = {0}, upload_fifo[128] = {0}, upload_folder[128] = {0}, upload_flag[64] = {0};
	char cmd[1024] = {0};
	struct JFFS_BACKUP_PROFILE_S *p;
	for(p = jffs_backup_profile_t; p->name; p++){
		if(!strcmp(name, p->name)){
			snprintf(upload_folder, sizeof(upload_folder), "/tmp/%s_upload", p->name);
			snprintf(upload_fifo, sizeof(upload_fifo), "%s/%s.bak", upload_folder, p->name);
			snprintf(upload_flag, sizeof(upload_flag), "upload_%s_tmp", p->name);
			snprintf(upload_rc, sizeof(upload_rc), "%s", p->rc_service);
			break;
		}
	}
	if(nvram_get_int(upload_flag)){
		if(check_if_dir_exist(upload_fifo)){
			snprintf(cmd, sizeof(cmd), "%s %s %s %s %s", "tar", "xzvf", upload_fifo, "-C", "/jffs");
			system(cmd);
		}
		if(do_rc && upload_rc[0])
			notify_rc(upload_rc);
		if(p->sync_flag > 0)
			sync_profile_update_time(p->sync_flag);
		ret = HTTP_OK;
	}
	nvram_unset(upload_flag);
	doSystem("rm -rf %s", upload_folder);
	return ret;
}

int check_blacklist_config(char *key, struct json_object *blacklist)
{
	int arraylen, i;
	if(!strncmp(key, "wl", 2) || !strncmp(key, "wan", 3) || !strcmp(key, "http_username")
		|| !strcmp(key, "http_passwd") || !strcmp(key, "acc_list")
		|| !strncmp(key, "smart_connect", 13) || !strncmp(key, "bsd", 3) || !strncmp(key, "wps", 3))
		return 1;
	if(blacklist && json_object_get_type(blacklist) == json_type_array){
		arraylen = json_object_array_length(blacklist);
		for(i = 0; i < arraylen; i++){
			if(!strcmp(key, json_object_get_string(json_object_array_get_idx(blacklist, i))))
				return 1;
		}
	}
	return 0;
}

int upload_config_sync_cgi()
{
	uint32_t *filelenptr;
	unsigned long count, filelen, i;
	FILE *fp, *fp2;
	unsigned char rand, *randptr;
	char header[8], buf[0x40000], *p, *v;
	struct json_object *root = json_object_new_object();
	struct json_object *tmp = json_object_new_object();
	struct json_object *blacklist = NULL, *tmp_value = NULL;
	struct nvram_tuple *j;
	fp = fopen("/tmp/settings_u.prf", "r+");
	if(fp){
		count = fread(header, 1, sizeof(header), fp);
		if(count < sizeof(header)){
			fclose(fp);
			goto end;
		}else if(!strncmp(header, PROFILE_HEADER, 4)){
			filelenptr = (uint32_t *)(header + 4);
#ifdef SWRT_DEBUG
			_dprintf("restoring original text cfg of length %x\n", *filelenptr);
#endif
			fread(buf, 1, *filelenptr, fp);
		}else if(!strncmp(header, "N55U", 4) || !strncmp(header, "AC55U", 4) || !strncmp(header, "BLUE", 4) || !strncmp(header, "HDR2", 4)){
			filelenptr = (uint32_t *)(header + 4);
#if defined(RTCONFIG_SAVEJFFS)
			filelen = le32_to_cpu(*filelenptr) & 0xffffff;
#else
			filelen = *filelenptr & 0xffffff;
#endif
#ifdef SWRT_DEBUG
			_dprintf("restoring non-text cfg of length %x\n", filelen);
#endif
			randptr = (unsigned char *)(header + 7);
			rand = *randptr;
#ifdef SWRT_DEBUG
			_dprintf("non-text cfg random number %x\n", rand);
#endif
			count = fread(buf, 1, *filelenptr & 0xFFFFFF, fp);
			for(i = 0; i < count; i++){
				if((unsigned char) buf[i] > ( 0xfd - 0x1)){
					/* e.g.: to skip the case: 0x61 0x62 0x63 0x00 0x00 0x61 0x62 0x63 */
					if(i > 0 && buf[i-1] != 0x0)
						buf[i] = 0x0;
				}
				else
					buf[i] = 0xff + rand - buf[i];	
			}
#ifdef SWRT_DEBUG
			for(i = 0; i < count; i++){
				if(i % 16 == 0)
					_dprintf("\n");
				_dprintf("%2x ", (unsigned char) buf[i]);
			}

			for(i = 0; i < count; i++){
				if(i % 16 == 0)
					_dprintf("\n");
				_dprintf("%c", buf[i]);
			}
#endif
		}else{
			fclose(fp);
			goto end;
		}
		fclose(fp);
		fp2 = fopen("/tmp/d_settings.txt", "w");
		if(fp2){
			p = buf;
			while (*p || p - buf <= count){
				/* e.g.: to skip the case: 00 2e 30 2e 32 38 00 ff 77 61 6e */
				if(!isprint(*p)){
					p = p + 1;
					continue;
				}
				v = strchr(p, '=');
				if(v != NULL){
					*v++ = 0x0;
					fprintf(fp2, "%s=%s\n", p, v);
					if(*v)
						json_object_object_add(tmp, p, json_object_new_string(v));
					p = v + strlen(v) + 1;
				}else
					p = p + 1;
			}
			fclose(fp2);
			blacklist = json_object_from_file(BLACKLIST_CONFIG_FILE);
			for(j = router_defaults; j->name; j++){
				if(j->name && !check_blacklist_config(j->name, blacklist)){
					json_object_object_get_ex(tmp, j->name, &tmp_value);
					json_object_object_add(root, j->name, json_object_new_string(json_object_get_string(tmp_value)));
				}
			}
			json_object_to_file(SAVE_CONFIG_SYNC_FILE, root);
		}
	}
end:
	unlink("/tmp/settings_u.prf");
	if(root)
		json_object_put(root);
	if(tmp)
		json_object_put(tmp);
	if(blacklist)
		json_object_put(blacklist);
	return 0;
}

int start_config_sync_cgi()
{
	int ret;
	struct json_object *root = NULL, *tmp_value = NULL;
	struct nvram_tuple *t;
#ifdef RTCONFIG_NVRAM_ENCRYPT
	char buf[4000];
#endif
	root = json_object_from_file(SAVE_CONFIG_SYNC_FILE);
	if(root == NULL)
		return HTTP_FAIL;
	if(json_object_get_type(root) == json_type_object){
		for(t = router_defaults; t->name; t++){
			if(json_object_object_get_ex(root, t->name, &tmp_value)){
#ifdef RTCONFIG_NVRAM_ENCRYPT
				memset(buf, 0, sizeof(buf));
				if(t->enc == CKN_ENC_SVR){
					set_enc_nvram(t->name, json_object_get_string(tmp_value), buf);
					nvram_set(t->name, buf);
				}else
#endif
					nvram_set(t->name, json_object_get_string(tmp_value));
			}
		}
		ret = HTTP_OK;
	}else
		ret = HTTP_FAIL;
	json_object_put(root);
	if(ret == HTTP_OK){
		httpd_nvram_commit();
		notify_rc("reboot");
	}
	return ret;
}
