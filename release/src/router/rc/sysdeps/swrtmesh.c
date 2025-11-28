/*
 * Copyright 2023-2025, SWRTdev
 * Copyright 2023-2025, paldier <paldier@hotmail.com>.
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
#if defined(RTCONFIG_RALINK)
#include <ralink.h>
#elif defined(RTCONFIG_QCA)
#include <qca.h>
#elif defined(RTCONFIG_LANTIQ)
#include <lantiq.h>
#endif
#include <flash_mtd.h>
#include <swrt.h>
#include <swrtmesh.h>
#include <swrtmesh-utils.h>

static char *wl_auth_mode_to_uci(char *auth_mode)
{
	if(!strcmp(auth_mode, "open") || !strcmp(auth_mode, "none"))
		return "none";
	else if(!strcmp(auth_mode, "psk"))
		return "psk";
	else if(!strcmp(auth_mode, "psk2"))
		return "psk2";
	else if(!strcmp(auth_mode, "pskpsk2"))
		return "psk-mixed";
	else if(!strcmp(auth_mode, "sae"))
		return "sae";
	else if(!strcmp(auth_mode, "psk2sae"))
		return "sae-mixed";
	else if(!strcmp(auth_mode, "wpa"))
		return "wpa";
	else if(!strcmp(auth_mode, "wpa2"))
		return "wpa2";
	else if(!strcmp(auth_mode, "wpawpa2"))
		return "wpa-mixed";
	else
		return "none";
}

void auto_generate_config(void)
{
	if(!check_if_dir_exist("/var/run/multiap"))
		eval("mkdir", "-p", "/var/run/multiap");
	if(!check_if_dir_exist("/etc/multiap"))
		eval("mkdir", "-p", "/etc/multiap");
	if(!check_if_file_exist("/etc/config/wireless"))
		swrtmesh_generate_wireless_config();
	if(!check_if_file_exist("/etc/config/ieee1905"))
		swrtmesh_generate_ieee1905_config();
	if(!check_if_file_exist("/etc/config/mapcontroller"))
		swrtmesh_generate_controller_config();
	if(!check_if_file_exist("/etc/config/mapagent"))
		swrtmesh_generate_agent_config();
	if(!check_if_file_exist("/etc/config/topology"))
		swrtmesh_generate_topology_config();
	if(!check_if_file_exist("/etc/config/hosts"))
		swrtmesh_generate_hosts_config();
}

int start_swrtmesh(void)
{
	int idx = 1;
	int sw_mode = sw_mode();
	pid_t pid;
	char *ubusd_argv[] = { "ubusd", NULL };
	char *wifimngr_argv[] = { "wifimngr", NULL };
	char *ieee1905_argv[] = { "ieee1905d", NULL, NULL, NULL, NULL, NULL, NULL };
	char *cntl_argv[] = { "mapcontroller", NULL, NULL, NULL, NULL, NULL, NULL };
	char *tp_argv[] = { "topologyd", NULL };
//	char *swrtmeshd_argv[] = { "swrtmeshd", NULL };

	if (getpid() != 1) {
		notify_rc("start_swrtmesh");
		return 0;
	}
	if(nvram_match("swrtmesh_enable", "0") || nvram_match("x_Setting", "0"))
		return 0;
	if(sw_mode == SW_MODE_REPEATER || sw_mode == SW_MODE_HOTSPOT || sw_mode == SW_MODE_NONE)
		return 0;
	swrtmesh_autoconf();
	stop_swrtmesh();
	if(nvram_match("swrtmesh_debug", "1")){
		system("touch /tmp/SWRTMESHUTILS_DEBUG");
		if(nvram_match("swrtmesh_agent_enable", "0") || nvram_match("swrtmesh_controller_enable", "0")){
			ieee1905_argv[idx] = "---no-lo";//there is no controller or agent on the local device, skip lo(less logging).
			idx++;
		}
		ieee1905_argv[idx] = "-dddd";
		idx++;
		ieee1905_argv[idx] = "-o";
		idx++;
		ieee1905_argv[idx] = "/tmp/ieee1905.log";
		//idx++;
		//ieee1905_argv[idx] = "-f";
	}
	auto_generate_config();
//	swrtmesh_resync_config();
	_eval(ubusd_argv, NULL, 0, &pid);
	_eval(wifimngr_argv, NULL, 0, &pid);
	_eval(ieee1905_argv, NULL, 0, &pid);
	_eval(tp_argv, NULL, 0, &pid);
	if(nvram_match("swrtmesh_controller_enable", "1")){
		char buf[2] = {0};
		if(nvram_match("swrtmesh_debug", "1")){
			idx = 1;
			cntl_argv[idx] = "-o";
			idx++;
			cntl_argv[idx] = "/tmp/mapcontroller.log";
			idx++;
			cntl_argv[idx] = "-d";
			idx++;
			cntl_argv[idx] = "-vvvv";
		}else{
			idx = 1;
			cntl_argv[idx] = "-o";
			idx++;
			cntl_argv[idx] = "/dev/null";
		}
		swrtmesh_get_value_by_string("mapagent", "controller_select", "local", buf, sizeof(buf));
		if(!strcmp(buf, "0")){
			idx++;
			cntl_argv[idx] = "-w";
		}
		_eval(cntl_argv, NULL, 0, &pid);
		if(check_if_file_exist("/proc/sys/net/netfilter/nf_conntrack_timestamp"))
			system("echo 1 >/proc/sys/net/netfilter/nf_conntrack_timestamp");
	}
	if(nvram_match("swrtmesh_agent_enable", "1")){
		FILE *fp;
		char *argv[]={"/sbin/delay_exec","15","/tmp/agent.sh",NULL};
		unlink("/var/run/multiap/multiap.backhaul");
		if (!(fp = fopen("/tmp/agent.sh", "w+")))
			return -1;
		fprintf(fp, "#!/bin/sh\n");
		fprintf(fp, "dynbhd &\n");
		fprintf(fp, "if [ -z \"$(pidof mapagent)\" ];then\n");
		if(nvram_match("swrtmesh_debug", "1"))
			fprintf(fp, "mapagent -o /tmp/mapagent.log -d -vvvv &\n");
		else
			fprintf(fp, "mapagent -o /dev/null\n");
		fprintf(fp, "fi\n");
		fclose(fp);
		chmod("/tmp/agent.sh",0777);
		_eval(argv, NULL, 0, &pid);
	}
	//_eval(swrtmeshd_argv, NULL, 0, &pid);
	return 0;
}

void stop_swrtmesh(void)
{
	if(getpid() != 1){
		notify_rc("stop_swrtmesh");
		return;
	}
	if(pids("topologyd"))
		killall_tk("topologyd");
	if(pids("dynbhd"))
		killall_tk("dynbhd");
	if(pids("swrtmeshd"))
		killall_tk("swrtmeshd");
	if(pids("mapagent"))
		killall_tk("mapagent");
	if(pids("mapcontroller"))
		killall_tk("mapcontroller");
	if(pids("ieee1905d"))
		killall_tk("ieee1905d");
	if(pids("wifimngr"))
		killall_tk("wifimngr");
	if(pids("ubusd"))
		killall_tk("ubusd");
}

int start_mapcontroller(void)
{
	pid_t pid;
	char *cntl_argv[] = { "mapcontroller", NULL, NULL, NULL, NULL, NULL, NULL };
	if(nvram_match("swrtmesh_controller_enable", "1")){
		int idx = 1;
		char buf[2] = {0};
		if(nvram_match("swrtmesh_debug", "1")){
			cntl_argv[idx] = "-o";
			idx++;
			cntl_argv[idx] = "/tmp/mapcontroller.log";
			idx++;
			cntl_argv[idx] = "-d";
			idx++;
			cntl_argv[idx] = "-vvvv";
		}else{
			cntl_argv[idx] = "-o";
			idx++;
			cntl_argv[idx] = "/dev/null";
		}
		swrtmesh_get_value_by_string("mapagent", "controller_select", "local", buf, sizeof(buf));
		if(!strcmp(buf, "0")){
			idx++;
			cntl_argv[idx] = "-w";
		}
	}
	_eval(cntl_argv, NULL, 0, &pid);
	if(check_if_file_exist("/proc/sys/net/netfilter/nf_conntrack_timestamp"))
		system("echo 1 >/proc/sys/net/netfilter/nf_conntrack_timestamp");
	return 0;
}

void stop_mapcontroller(void)
{
	if(pids("mapcontroller"))
		killall_tk("mapcontroller");
}

int wl_isup(char* ifname)
{
	char path[128] = {0}, buf[16] = {0};
	snprintf(path, sizeof(path), "/sys/class/net/%s/operstate", ifname);
	if(!f_exists(path))
		return 0;
	if(f_read_string(path, buf, sizeof(buf)) > 0 && !strcmp(buf, "up"))
		return 1;
	return 0;
}

void start_bandsteer()
{
	int sw_mode = sw_mode();
	struct uci_context *ctx = NULL;
	struct uci_package *pkg = NULL;
	struct uci_section *section = NULL;
	if(nvram_match("swrtmesh_enable", "0") || nvram_match("x_Setting", "0"))
		return;
	if(sw_mode == SW_MODE_REPEATER || sw_mode == SW_MODE_HOTSPOT || sw_mode == SW_MODE_NONE || wisp_mode())
		return;
	pkg = swrtmesh_uci_load_pkg(&ctx, "mapcontroller");
	if(!pkg){
		SWRTMESHUTILS_DBG("uci_load mapcontroller failed\n");
		return;
	}
	section = swrtmesh_config_get_section(ctx, pkg, "controller", "initial_channel_scan", "0");
	if(section)
		swrtmesh_uci_add_option(ctx, pkg, section, "initial_channel_scan", "1", false);
	section = swrtmesh_config_get_section(ctx, pkg, "sta_steering", "enable_sta_steer", "0");
	if(section)
		swrtmesh_uci_add_option(ctx, pkg, section, "enable_sta_steer", "1", false);
#if defined(RTCONFIG_QCA_LBD) || defined(RTCONFIG_MTK_BSD) || defined(RTCONFIG_RALINK_BSD)
	section = swrtmesh_config_get_section(ctx, pkg, "sta_steering", "bandsteer", "0");
	if(section)
		swrtmesh_uci_add_option(ctx, pkg, section, "bandsteer", "1", false);
#endif
//#if defined(RTCONFIG_BCN_RPT) || defined(RTCONFIG_BTM_11V)
	section = swrtmesh_config_get_section(ctx, pkg, "sta_steering", "use_bcn_metrics", "0");
	if(section)
		swrtmesh_uci_add_option(ctx, pkg, section, "use_bcn_metrics", "1", false);
//#endif
	uci_commit(ctx, &pkg, false);
	uci_unload(ctx, pkg);
}

void stop_bandsteer()
{
	struct uci_context *ctx = NULL;
	struct uci_package *pkg = NULL;
	struct uci_section *section = NULL;
	pkg = swrtmesh_uci_load_pkg(&ctx, "mapcontroller");
	if(!pkg){
		SWRTMESHUTILS_DBG("uci_load mapcontroller failed\n");
		return;
	}
	section = swrtmesh_config_get_section(ctx, pkg, "controller", "initial_channel_scan", "1");
	if(section)
		swrtmesh_uci_add_option(ctx, pkg, section, "initial_channel_scan", "0", false);
	section = swrtmesh_config_get_section(ctx, pkg, "sta_steering", "enable_sta_steer", "1");
	if(section)
		swrtmesh_uci_add_option(ctx, pkg, section, "enable_sta_steer", "0", false);
#if defined(RTCONFIG_QCA_LBD) || defined(RTCONFIG_MTK_BSD) || defined(RTCONFIG_RALINK_BSD)
	section = swrtmesh_config_get_section(ctx, pkg, "sta_steering", "bandsteer", "1");
	if(section)
		swrtmesh_uci_add_option(ctx, pkg, section, "bandsteer", "0", false);
#endif
//#if defined(RTCONFIG_BCN_RPT) || defined(RTCONFIG_BTM_11V)
	section = swrtmesh_config_get_section(ctx, pkg, "sta_steering", "use_bcn_metrics", "1");
	if(section)
		swrtmesh_uci_add_option(ctx, pkg, section, "use_bcn_metrics", "0", false);
//#endif
	uci_commit(ctx, &pkg, false);
	uci_unload(ctx, pkg);
}

void duplicate_wl_sync_uci(char *prefix, char *prefix2)
{
	char ssid[64] = {0}, key[64] = {0}, encryption[32] = {0};
	char ssid2[64] = {0}, key2[64] = {0}, encryption2[32] = {0};
	struct uci_context *ctx = NULL;
	struct uci_package *pkg = NULL;
	struct uci_section *section = NULL;
	strlcpy(ssid, nvram_pf_safe_get(prefix, "ssid"), sizeof(ssid));
	strlcpy(encryption, wl_auth_mode_to_uci(nvram_pf_safe_get(prefix, "auth_mode_x")), sizeof(encryption));
	if(!strcmp(encryption, "wpa") || !strcmp(encryption, "wpa2") || !strcmp(encryption, "wpa-mixed"))
		strlcpy(key, nvram_pf_safe_get(prefix, "key"), sizeof(key));
	else
		strlcpy(key, nvram_pf_safe_get(prefix, "wpa_psk"), sizeof(key));
	strlcpy(ssid2, nvram_pf_safe_get(prefix2, "ssid"), sizeof(ssid2));
	strlcpy(encryption2, wl_auth_mode_to_uci(nvram_pf_safe_get(prefix2, "auth_mode_x")), sizeof(encryption2));
	if(!strcmp(encryption2, "wpa") || !strcmp(encryption2, "wpa2") || !strcmp(encryption2, "wpa-mixed"))
		strlcpy(key2, nvram_pf_safe_get(prefix2, "key"), sizeof(key2));
	else
		strlcpy(key2, nvram_pf_safe_get(prefix2, "wpa_psk"), sizeof(key2));

	pkg = swrtmesh_uci_load_pkg(&ctx, "mapcontroller");
	if(pkg){
		section = swrtmesh_config_get_section(ctx, pkg, "ap", "ssid", ssid2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "ssid", ssid, false);
		section = swrtmesh_config_get_section(ctx, pkg, "ap", "encryption", encryption2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "encryption", encryption, false);
		section = swrtmesh_config_get_section(ctx, pkg, "ap", "key", key2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "key", key, false);
		uci_commit(ctx, &pkg, false);
		uci_unload(ctx, pkg);
	}
	pkg = swrtmesh_uci_load_pkg(&ctx, "mapagent");
	if(pkg){
		section = swrtmesh_config_get_section(ctx, pkg, "ap", "ssid", ssid2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "ssid", ssid, false);
		section = swrtmesh_config_get_section(ctx, pkg, "ap", "encryption", encryption2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "encryption", encryption, false);
		section = swrtmesh_config_get_section(ctx, pkg, "ap", "key", key2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "key", key, false);
		uci_commit(ctx, &pkg, false);
		uci_unload(ctx, pkg);
	}
	pkg = swrtmesh_uci_load_pkg(&ctx, "ieee1905");
	if(pkg){
		section = swrtmesh_config_get_section(ctx, pkg, "ap", "ssid", ssid2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "ssid", ssid, false);
		section = swrtmesh_config_get_section(ctx, pkg, "ap", "encryption", encryption2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "encryption", encryption, false);
		section = swrtmesh_config_get_section(ctx, pkg, "ap", "key", key2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "key", key, false);
		uci_commit(ctx, &pkg, false);
		uci_unload(ctx, pkg);
	}
	pkg = swrtmesh_uci_load_pkg(&ctx, "wireless");
	if(pkg){
		section = swrtmesh_config_get_section(ctx, pkg, "wifi-iface", "ssid", ssid2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "ssid", ssid, false);
		section = swrtmesh_config_get_section(ctx, pkg, "wifi-iface", "encryption", encryption2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "encryption", encryption, false);
		section = swrtmesh_config_get_section(ctx, pkg, "wifi-iface", "key", key2);
		if(section)
			swrtmesh_uci_add_option(ctx, pkg, section, "key", key, false);
		uci_commit(ctx, &pkg, false);
		uci_unload(ctx, pkg);
	}
}
