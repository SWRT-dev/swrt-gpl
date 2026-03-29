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
#include <tcode.h>
#ifdef RTCONFIG_AMAS
#include <obd.h>
#endif

int wave_monitor_core(void);

static int restart_qo = 0;
static int restart_fwl = 0;
static int restart_wifi = 0;
static int dwb_mode_pre = -1;
static int wave_signal_idx = 0;
static int iwpriv_once = 0;
static char ap_mac_pre[20 *3];
static char ap_ssid_pre[40 *3];
static char ap_security_pre[40 *3];
static char ap_beacon_pre[40 *3];
static char ap_encrypt_pre[40 *3];
static char ap_key_pre[40 *3];

struct tcode_psd_s {
	char *tcode;
	char *ccode;
	char *new_ccode;
};

const struct tcode_psd_s psd_mod_country_list[] = {
	{ "AA", "KR", "US" },
	{ "KR", "AU", "US" },
	{ "CN", "KR", "US" },
	{ "AU", "KR", "US" },
	{ NULL, NULL, NULL }
};

struct ccode_s {
	char *tcode;
	char *ccode;
};

const struct ccode_s country_code_list[] = {
	{ "AU", "AU" },
	{ "CA", "CA" },
	{ "CN", "CN" },
	{ "GB", "GB" },
	{ "KR", "KR" },
	{ "US", "US" },
	{ NULL, NULL }
};

int is_hostapd_running(const char *ifname)
{
	struct wpa_ctrl *ctrl = NULL;
	char path[256], buf[3];
	size_t len;

	snprintf(path, sizeof(path), "/var/run/hostapd/%s", ifname);
	ctrl = wpa_ctrl_open(path);
	if(ctrl == NULL) return 0;
	if(wpa_ctrl_attach(ctrl))
	{
		wpa_ctrl_close(ctrl);
		return 0;
	}
	len = -1;
	if(wpa_ctrl_request(ctrl, "PING", 4, buf, &len, 0) < 0 || len < 4 || memcmp(buf, "PONG", 4)){
		printf("hostapd did not reply to PING command - exiting");
		wpa_ctrl_close(ctrl);
		return 0;
	}
	wpa_ctrl_close(ctrl);
	return 1;
}

void restart_hostapd(const char *ifname)
{
	int count;

	if(!strcmp(ifname, "wlan0"))
	{
		for(count = 10; count > 0; --count)
		{
			if(!is_hostapd_running("wlan0"))
			{
				if(!pids("hostpad_cli_wlan0"))
					break;
			}
			system("kill -9 `pidof hostapd_cli_wlan0`");
			system("kill -9 `pidof hostapd_wlan0`");
			sleep(1);
			_dprintf("%s : kill hostapd_cli_wlan0\n", __func__);
			_dprintf("%s : kill hostapd_wlan0\n", __func__);
		}
		for(count = 11; count > 0; --count)
		{
			if(is_hostapd_running("wlan0"))
				break;
			system("/tmp/hostapd_wlan0 /opt/lantiq/wave/confs/hostapd_wlan0.conf -e /tmp/hostapd_ent_wlan0 -B");
			sleep(1);
			_dprintf("%s : run hostapd_wlan0\n", __func__);
		}
		for(count = 11; count > 0; --count)
		{
			if(pids("hostapd_cli_wlan0"))
				break;
			system("/tmp/hostapd_cli_wlan0 -iwlan0 -a/opt/lantiq/wave/scripts/fapi_wlan_wave_events_hostapd.sh -B");
			sleep(1);
			_dprintf("%s : run hostapd_cli_wlan0\n", __func__);
		}
	}
	else if(!strcmp(ifname, "wlan2"))
	{
		for(count = 10; count > 0; --count)
		{
			if(!is_hostapd_running("wlan2"))
			{
				if(!pids("hostpad_cli_wlan2"))
					break;
			}
			system("kill -9 `pidof hostapd_cli_wlan2`");
			system("kill -9 `pidof hostapd_wlan2`");
			sleep(1);
			_dprintf("%s : kill hostapd_cli_wlan2\n", __func__);
			_dprintf("%s : kill hostapd_wlan2\n", __func__);
		}
		for(count = 11; count > 0; --count)
		{
			if(is_hostapd_running("wlan2"))
				break;
			system("/tmp/hostapd_wlan2 /opt/lantiq/wave/confs/hostapd_wlan2.conf -e /tmp/hostapd_ent_wlan2 -B");
			sleep(1);
			_dprintf("%s : run hostapd_wlan2\n", __func__);
		}
		for(count = 11; count > 0; --count)
		{
			if(pids("hostapd_cli_wlan2"))
				break;
			system("/tmp/hostapd_cli_wlan2 -iwlan2 -a/opt/lantiq/wave/scripts/fapi_wlan_wave_events_hostapd.sh -B");
			sleep(1);
			_dprintf("%s : run hostapd_cli_wlan2\n", __func__);
		}
	}
}

#if defined(RTCONFIG_AMAS)
void wave_clear_all_probe_req_vsie(void)
{
	char cmd[300];
	int pktflag = 0xE;
	char *ifname;
	FILE *fp;
	
	ifname = get_staifname(0);

	snprintf(cmd, sizeof(cmd), "/usr/bin/wpa_cli vendor_elem_get -i%s %d", ifname, pktflag);
	if ((fp = popen(cmd, "r")) != NULL) {
		char vendor_elem[MAX_VSIE_LEN];
		vendor_elem[sizeof(vendor_elem)-1] = '\0';
		while (fgets(vendor_elem , sizeof(vendor_elem)-1 , fp) != NULL) {
			if(strlen(vendor_elem) > 0) {
				snprintf(cmd, sizeof(cmd), "/usr/bin/wpa_cli vendor_elem_remove -i%s %d %s",
					ifname, pktflag, vendor_elem);
				OBD_DBG("cmd=%s\n", cmd);
				system(cmd);
			}
		}
		pclose(fp);
	}
}
#endif

int gen_vap_index_file(int unit)
{
	FILE *fp;

	fp = fopen("/tmp/cur_vap_asuswrt.txt", "w+");
	switch(unit)
	{
		case 6:
			fprintf(fp, "wlan0.0");
			break;
		case 7:
			fprintf(fp, "wlan0.1");
			break;
		case 8:
			fprintf(fp, "wlan0.2");
			break;
		case 9:
			fprintf(fp, "wlan2.0");
			break;
		case 10:
			fprintf(fp, "wlan2.1");
			break;
		case 11:
			fprintf(fp, "wlan2.2");
			break;
	}
	fclose(fp);
	return 1;
}

int del_vap_index_file(void)
{
	unlink("/tmp/cur_vap_asuswrt.txt");
	return 0;
}

void clean_wave_db(void)
{
	system("rm -f /jffs/db_*.tgz");
	system("rm -rf /jffs/db");
}

void unload_mtlk(void)
{
	while(pidof("hostapd_wlan0") > 0)
		system("kill -9 `pidof hostapd_wlan0`");
	 while(pidof("hostapd_wlan2") > 0)
		system("kill -9 `pidof hostapd_wlan2`");
	while(pidof("drvhlpr_wlan0") > 0)
		system("kill -9 `pidof drvhlpr_wlan0`");
	while(pidof("drvhlpr_wlan2") > 0)
		system("kill -9 `pidof drvhlpr_wlan2`");
	wlan_uninit();
	nvram_set("wave_ready", "0");
	system("rm -rf /opt/lantiq/wave/confs/; mkdir /opt/lantiq/wave/confs/");
	system("rm -rf /tmp/wlan_wave/");
	system("rm -rf /opt/lantiq/wave/db/default/");
	system("rm -rf /opt/lantiq/wave/db/instance/");
	clean_wave_db();
}

int wl_wave_unit(int unit)
{
	if(unit == 1)
		return 2;
	else
		return 0;
}

int sync_nvram_wireless(void)
{
	_dprintf("sync_nvram_wireless: todo\n");
	return 1;
}

int write_to_file(const char *path, const char *buf)
{
	FILE *fp;

	fp = fopen(path, "w+");
	fprintf(fp, buf);
	fclose(fp);
	return 1;
}

int switch_to_20mhz(int channel)
{
	char ccode[3];

	snprintf(ccode, sizeof(ccode), "%s", nvram_safe_get("wl_country_code"));
	if(strcmp(ccode, "GB"))
	{
		if(channel != 165)
			return (channel ^ 140) == 0;
	}
	else if(channel != 116)
	{
		if(channel != 132)
			return (channel ^ 136) == 0;
	}
	return 1;
}

void reset_lanifnames(char *ifname, int add)
{
	char lan_ifnames[256];

	strcpy(lan_ifnames, nvram_safe_get("lan_ifnames"));
	if(add)
		add_to_list(ifname, lan_ifnames, sizeof(lan_ifnames));
	else
		remove_from_list(ifname, lan_ifnames, sizeof(lan_ifnames));
	nvram_set("lan_ifnames", lan_ifnames);
}

int config_guest_network_bridge()
{
	int i, band;
	char cmd[256];
	char ifname[256];
	char prefix[] = "wlXXXXXXXXXXXXXXXXXXX_";
	char lan_ifname[16];

	memset(cmd, 0, sizeof(cmd));
	snprintf(lan_ifname, sizeof(lan_ifname), "%s", nvram_safe_get("lan_ifname"));
	for (band = WL_2G_BAND; band < MAX_NR_WL_IF; ++band) {
		for(i = 1; i < MAX_NO_MSSID; i++){
			memset(ifname, 0, sizeof(ifname));
//			getInterfaceName(i + 5, ifname);
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", band, i);
			if(nvram_pf_get_int(prefix, "bss_enabled") == 1){
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "ppacmd addlan -i %s", ifname);
				_dprintf("[%s][%d] %s\n", __func__, __LINE__, cmd);
				system(cmd);
				_dprintf("btctl addif %s %s\n", lan_ifname, ifname);
				eval("brctl", "addif", lan_ifname, ifname);
			}else{
				if(strncmp(ifname, "wlan", 4))
					continue;
				memset(cmd, 0, sizeof(cmd));
				snprintf(cmd, sizeof(cmd), "ppacmd dellan -i %s", ifname);
				_dprintf("[%s][%d] %s\n", __func__, __LINE__, cmd);
				system(cmd);
				_dprintf("btctl delif %s %s\n", lan_ifname, ifname);
				eval("brctl", "delif", lan_ifname, ifname);
			}
		}
	}
	return 1;
}

int wave_set_radio_onoff(int action, int unit, int subunit, int onoff)
{

	_dprintf("[%s][%d][%d][%d]\n", __func__, __LINE__, unit, subunit);

	if(onoff == 1){
		_dprintf("[%s][%d][%d][%d]:set radio on\n", __func__, __LINE__, unit, subunit);
		if(unit == 1){
			restart_hostapd("wlan2");
			nvram_set("wave_wlan2_up", "1");
		}else if(unit == 0){
			restart_hostapd("wlan0");
			nvram_set("wave_wlan0_up", "1");
		}
	}else if(onoff == 0){
		_dprintf("[%s][%d][%d][%d]:set radio off\n", __func__, __LINE__, unit, subunit);
		if(unit == 1){
			system("ifconfig wlan2 down");
			nvram_set("wave_wlan2_up", "0");
		}else if(unit == 0){
			system("ifconfig wlan0 down");
			nvram_set("wave_wlan0_up", "0");
		}
		sleep(2);
	}else
		_dprintf("[%s][%d][%d][%d]: error, onoff=[%d], skip this action\n", __func__, __LINE__, unit, subunit, onoff);
	return 1;
}

static struct itimerval itv_wave;

static void alarmtimer(unsigned long sec, unsigned long usec)
{
	itv_wave.it_value.tv_sec = sec;
	itv_wave.it_value.tv_usec = usec;
	itv_wave.it_interval = itv_wave.it_value;
	setitimer(ITIMER_REAL, &itv_wave, NULL);
}

int wave_monitor_main(int argc, char *argv[])
{
	FILE *fp;
	pid_t pid;
	sigset_t sigs_to_catch;

	if((fp = fopen("/var/run/wave_monitor.pid", "w"))){
		fprintf(fp, "%d", getpid());
		fclose(fp);
	}

	sigemptyset(&sigs_to_catch);
	sigaddset(&sigs_to_catch, SIGUSR1);
	sigprocmask(SIG_UNBLOCK, &sigs_to_catch, NULL);
	signal(SIGUSR1, wave_monitor_core);
	alarmtimer(30, 0);
	if(nvram_get_int("wave_CFG") != 1)
		nvram_set("wave_action", "1");
	kill_pidfile_s("/var/run/wave_monitor.pid", SIGUSR1);
	while(1)
		pause();
	return 0;
}

void stop_wifi_service(void)
{
	_dprintf("[%s][%d] clean wave files\n", __func__, __LINE__);
//	system("rm -rf /opt/lantiq/wave/db/instance/");
//	system("rm -rf /tmp/wireless");
//	wlan_uninit();
}

int update_client_event(void)
{
	return WLCNT_TRIGGER();
}

int wave_is_radio_on(int unit)
{
	int ret = is_if_up(get_wififname(unit));
	
	if(ret){
		if(ret != 1)
		{
			_dprintf("[%s][%d] %s get up status error\n", __func__, __LINE__, get_wififname(unit));
			return 0;
		}
		return 1;
	}
	return 0;
}

int replace_str_form_to(const char *str, char from, char to)
{
	char *p = NULL, *i = NULL;

	if(!str)
		return 0;
	for(i = str; i - p < strlen(p); ++i){
		if(*i == from)
			*i = to;
	}
	return 1;
}

int en_multi_profile(void)
{
	if(nvram_match("re_mode", "1")){
		if(nvram_match("dwb_mode", "1") || nvram_match("dwb_mode", "3"))
			return 1;
	}
	return 0;
}

int get_pf_network_idx(int band, int if_idx)
{
	if(en_multi_profile()){
		if(band == 1)
			return if_idx;
	}
	return 0;
}

void wave_set_sta_config()
{
	FILE *fp = NULL;
	int iswep = 0, unit, i, count, len, band = 0, if_idx = 0, dwb_mode, vap_idx, if_tmp;
	char apmac[20];
	char encrypt[40];
	char beacon[40];
	char authmode[40];
	char key[40];
	char crypto[40];
	char auth[40];
	char ssid[40];
	char wlc_prefix[] = "wlcXXXXXXX_";
	char prefix[] = "wlcXXXXXXX_";
	char word[128], path[128], cmd[128];
	char *next = NULL, *pauth = NULL, *pauth_alg = NULL, *ppsk = NULL, *pproto = NULL, *ppairwise = NULL, *pgroup = NULL, *pkey_mgmt = NULL, *pssid = NULL;

	foreach(word, nvram_safe_get("sta_ifnames"), next){
		memset(apmac, 0, sizeof(apmac));
		memset(ssid, 0, sizeof(ssid));
		memset(auth, 0, sizeof(auth));
		memset(crypto, 0, sizeof(crypto));
		memset(key, 0, sizeof(key));
		memset(authmode, 0, sizeof(authmode));
		memset(beacon, 0, sizeof(beacon));
		memset(encrypt, 0, sizeof(encrypt));
		unit = band;
		dwb_mode = nvram_get_int("dwb_mode");
		snprintf(wlc_prefix, sizeof(wlc_prefix), "wlc%d_", band);
		for(unit = band; unit < MAX_NR_WL_IF; unit++){
			SKIP_ABSENT_BAND_AND_INC_UNIT(unit);
			if(en_multi_profile() && band == 1){
				snprintf(wlc_prefix, sizeof(wlc_prefix), "%s", "wsfh_");
				if(strstr(wlc_prefix, "wsbh") || strstr(wlc_prefix, "wsfh"))
					snprintf(auth, sizeof(auth), "%s", nvram_pf_safe_get(wlc_prefix, "auth_mode_x"));
			}else
				snprintf(auth, sizeof(auth), "%s", nvram_pf_safe_get(wlc_prefix, "auth_mode"));
			snprintf(apmac, sizeof(apmac), "%s", nvram_pf_safe_get(wlc_prefix, "ap_mac"));
			snprintf(ssid, sizeof(ssid), "%s", nvram_pf_safe_get(wlc_prefix, "ssid"));
			snprintf(crypto, sizeof(crypto), "%s", nvram_pf_safe_get(wlc_prefix, "crypto"));
			if(strcmp(auth, "open") || nvram_pf_get_int(wlc_prefix, "wep") <= 0)
				snprintf(key, sizeof(key), "%s", nvram_pf_safe_get(wlc_prefix, "wpa_psk"));
			else
				snprintf(key, sizeof(key), "%s", nvram_pf_safe_get(wlc_prefix, "wep_key"));
			if(ssid[0] == 0x0)
				break;
			snprintf(beacon, sizeof(beacon), "%s", wav_get_beacon_type(crypto));
			snprintf(encrypt, sizeof(encrypt), "%s", wav_get_encrypt(crypto));
			snprintf(authmode, sizeof(authmode), "%s", wav_get_security_str(auth, crypto, nvram_pf_get_int(wlc_prefix, "wep")));
			if(authmode[0] == 0x0){
				fprintf(stderr, "[warning] not support this auth. mode\n");
				_dprintf("%s %d wlc setting is not changed. skip config setting. band=[%d]\n", __func__, __LINE__, band);
				goto badauth;
			}
			if(memcmp(apmac, &ap_mac_pre[20 * unit], 20)
				|| memcmp(ssid, &ap_ssid_pre[40 * unit], 40)
				|| memcmp(authmode, &ap_security_pre[40 * unit], 40)
				|| memcmp(beacon, &ap_beacon_pre[40 * unit], 40)
				|| memcmp(encrypt, &ap_encrypt_pre[40 * unit], 40)
				|| memcmp(key, &ap_key_pre[40 * unit], 40)
				|| (band == 1 && dwb_mode != dwb_mode_pre)){
				memcpy(&ap_mac_pre[20 * unit], apmac, 20);
				memcpy(&ap_ssid_pre[40 * unit], ssid, 40);
				memcpy(&ap_security_pre[40 * unit], authmode,40);
				memcpy(&ap_beacon_pre[40 * unit], beacon, 40);
				memcpy(&ap_encrypt_pre[40 * unit], encrypt, 40);
				memcpy(&ap_key_pre[40 * unit], key, 40);
				if(band == 1){
					dwb_mode_pre = dwb_mode;
					vap_idx = 3;
				}else if(band == 0){
					vap_idx = 1;
				}
				snprintf(prefix, sizeof(prefix), "wlc%d_", band);
				pauth_alg = NULL;
				ppsk = NULL;
				pproto = NULL;
				ppairwise = NULL;
				pgroup = NULL;
				while(1){
					if(en_multi_profile() && band == 1){
						snprintf(prefix, sizeof(prefix), "%s", "wsfh_");
						if (!strstr(prefix, "wsbh") && !strstr(prefix, "wsfh")){
							if(pauth == NULL){
								pkey_mgmt = strdup("NONE");
								goto nopsk;
							}
						}else
							pauth = nvram_pf_get(prefix, "auth_mode_x");
					}else
						pauth = nvram_pf_get(prefix, "auth_mode");

					if(*pauth){
						if(strcmp(pauth, "open") || !nvram_pf_match(prefix, "wep", "0")){
							if(!strcmp(pauth, "open")){
								pkey_mgmt = strdup("NONE");
								pauth_alg = strdup("OPEN");
								iswep = 1;
								goto nopsk;
							}else if(!strcmp(pauth, "shared")){
								pkey_mgmt = strdup("NONE");
								pauth_alg = strdup("SHARED");
								iswep = 1;
								goto nopsk;
							}else if(!strcmp(pauth, "psk") || !strcmp(pauth, "psk2") || !strcmp(pauth, "pskpsk2")){
								pkey_mgmt = strdup("WPA-PSK");
								if(!strcmp(pauth, "psk"))
									pproto = strdup("WPA");
								else if(strcmp(pauth, "psk2"))
									pproto = strdup("RSN");
								else
									pproto = strdup("WPA RSN");
							}
							if(nvram_pf_match(prefix, "crypto", "tkip")){
								ppairwise = strdup("TKIP");
								pgroup = strdup("TKIP");

							}else if(nvram_pf_match(prefix, "crypto", "aes")){
								ppairwise = strdup("CCMP TKIP");
								pgroup = strdup("CCMP TKIP");
							}
							ppsk = strdup(nvram_pf_safe_get(prefix, "wpa_psk"));
						}
					}
nopsk:
					snprintf(path, sizeof(path), "/tmp/run_wlan%d_wpa_%d.sh", vap_idx, if_idx);
					fp = fopen(path, "w+");
					if(fp == NULL){
						_dprintf("Open wlan%d sh fail.\n", vap_idx);
						free(pkey_mgmt);
						free(pauth_alg);
						free(ppsk);
						free(pproto);
						free(ppairwise);
						free(pgroup);
						break;
					}
					fprintf(fp, "#!/bin/sh\n");
					if_tmp = get_pf_network_idx(band, if_idx);
					fprintf(fp, "wpa_cli -iwlan%d remove_network %d\n", vap_idx, if_tmp);
					if(band == 1 && if_idx == 0)
						fprintf(fp, "wpa_cli -iwlan%d remove_network all\n", vap_idx);
					fprintf(fp, "wpa_cli -iwlan%d add_network %d\n", vap_idx, if_tmp);
					pssid = nvram_pf_safe_get(prefix, "ssid");
					fprintf(fp, "wpa_cli -iwlan%d set_network %d ssid '\"%s\"'\n", vap_idx, if_tmp, pssid);
					if(pkey_mgmt)
						fprintf(fp, "wpa_cli -iwlan%d set_network %d key_mgmt %s\n", vap_idx, if_tmp, pkey_mgmt);
					if(pauth_alg)
						fprintf(fp, "wpa_cli -iwlan%d set_network %d auth_alg %s\n", vap_idx, if_tmp, pauth_alg);
					if(ppsk)
						fprintf(fp, "wpa_cli -iwlan%d set_network %d psk '\"%s\"'\n", vap_idx, if_tmp, ppsk);
					if(pproto)
						fprintf(fp, "wpa_cli -iwlan%d set_network %d proto \"%s\"\n", vap_idx, if_tmp, pproto);
 					if(ppairwise)
						fprintf(fp, "wpa_cli -iwlan%d set_network %d pairwise \"%s\"\n", vap_idx, if_tmp, ppairwise);
 					if(pgroup)
						fprintf(fp, "wpa_cli -iwlan%d set_network %d group \"%s\"\n", vap_idx, if_tmp, pgroup);
					fprintf(fp, "wpa_cli -iwlan%d set_network %d scan_ssid %d\n", vap_idx, if_tmp, 1);
 					if(iswep){
						for(i = 1; i < 5; i++){
							count = nvram_pf_get_int(prefix, "key");
							if(count == i){
								len = strlen(nvram_pf_safe_get(prefix, "wep_key"));
								if(len == 5 || len == 10 || len == 13 || len == 26){
									fprintf(fp, "wpa_cli -iwlan%d set_network %d wep_tx_keyidx %d\n", vap_idx, if_tmp, count - 1);
									fprintf(fp, "wpa_cli -iwlan%d set_network %d wep_key%d %s\n", vap_idx, if_tmp, count - 1, nvram_pf_get(prefix, "wep_key"));
  								}else{
									fprintf(fp, "wpa_cli -iwlan%d set_network %d wep_tx_keyidx %d\n", vap_idx, if_tmp, count - 1);
									fprintf(fp, "wpa_cli -iwlan%d set_network %d wep_key%d 0\n", vap_idx, if_tmp, count - 1);
								}
							}
						}
					}
					fprintf(fp, "wpa_cli -iwlan%d enable_network %d\n", vap_idx, if_tmp);
					fprintf(fp, "wpa_cli -iwlan%d save_config\n", vap_idx);
					fclose(fp);
					snprintf(cmd, sizeof(cmd), "/tmp/run_wlan%d_wpa_%d.sh", vap_idx, if_idx);
					eval("sh", cmd);
					if(en_multi_profile() && band == 1 && ++if_idx == 1)
						continue;
					free(pkey_mgmt);
					free(pauth_alg);
					free(ppsk);
					free(pproto);
					free(ppairwise);
					free(pgroup);
					break;
				}
			}
			if(en_multi_profile() == 0 || band > 1)
				break;
		}
		if(band > 1){
badauth:
			_dprintf( "%s %d wlc setting is not changed. skip config setting. band=[%d]\n", __func__, __LINE__, band);
		}
		band++;
 	}
}

int wave_monitor_core(void)
{
	int unit, wps_band, wave_flag, wave_action, i, ret;
	char lan_ifname[16], cmd[256], buf[256];
	char *pt1 = NULL, *pt2 = NULL;
	const struct ccode_s *cclist = NULL;

	memset(cmd, 0, sizeof(cmd));
	unit = nvram_get_int("wl_unit");
	wave_flag = nvram_get_int("wave_flag");
	wps_band = nvram_get_int("wps_band_x");
	snprintf(lan_ifname, sizeof(lan_ifname), "%s", nvram_safe_get("lan_ifname"));
	wave_action = nvram_get_int("wave_action");
	if(wave_action != 1 && nvram_get_int("wave_ready") != 1){
		_dprintf("[%s][%d] action:[%d], wave_ready is not 1, return\n", __func__, __LINE__, wave_action);
		return -1;
	}
	nvram_set_int("wave_action", 0);
	if(wave_action){
		_dprintf("[%s][%d] begin of wave_monitor_core(),action:[%d] signal_idx:[%d]\n", __func__, __LINE__, wave_action, wave_signal_idx);
		nvram_set_int("wave_action_cur", wave_action);
		for(cclist = &country_code_list[0]; cclist->tcode; cclist++){
			if(!strcmp(cclist->tcode, nvram_safe_get("wl0_country_code")))
				break;
		}
		if(nvram_match("territory_code", "KR/01"))
			system("mv /tmp/lantiq_wave/images/PSD_KR.bin /tmp/lantiq_wave/images/PSD.bin");
		f_write_string("/proc/sys/kernel/printk", "0", 0, 0);
		switch(wave_action){
			case WAVE_ACTION_INIT:
				nvram_set("wave_ready", "0");
				if(!nvram_get_int("x_Setting")){
					_dprintf("[warning] x_Setting=0, remove existed jffs db\n");
					clean_wave_db();
				}
				if(access("/jffs/db_instance_20260323.tgz", F_OK) == -1){
					_dprintf("***************** jffs db is not exist\n");
					_dprintf("[warning] no wireless setting, generate again\n");
					clean_wave_db();
					//wlan_createInitialConfigFiles(NULL);
					if(cclist->ccode){
						if(strcmp(cclist->ccode, "GB"))
							snprintf(cmd, sizeof(cmd), "/opt/lantiq/wave/scripts/fapi_wlan_wave_update_defaults.sh %s %s %s %s %d %d %s %s",
								"None", "None", "test_passphrase", "test_passphrase", 0, 0, cclist->ccode, cclist->ccode);
						else
							snprintf(cmd, sizeof(cmd), "/opt/lantiq/wave/scripts/fapi_wlan_wave_update_defaults.sh %s %s %s %s %d %d %s %s",
								"None", "None", "test_passphrase", "test_passphrase", 0, 36, cclist->ccode, cclist->ccode);
					}else{
						snprintf(cmd, sizeof(cmd), "/opt/lantiq/wave/scripts/fapi_wlan_wave_update_defaults.sh %s %s %s %s %d %d %s %s",
							"None", "None", "test_passphrase", "test_passphrase", 0, 0, "US", "US");
					}
					_dprintf("[warning][%s]\n", cmd);
					system(cmd);
					_dprintf("--------- fapi_wlan_cli init . -----------\n");
				}else{
					_dprintf("***************** jffs db is exist\n");
					system("cd /opt/lantiq/wave/; rm -rf db/default; rm -rf db/instance; rm -rf confs; tar zxf /jffs/db_instance_20260323.tgz; rm -f confs/wlan_notification_*");
					_dprintf("cd /opt/lantiq/wave/; rm -rf db/default; rm -rf db/instance; rm -rf confs; tar zxf /jffs/db_instance_20260323.tgz; rm -f confs/wlan_notification_*");
					_dprintf("--------- fapi_wlan_cli init . -----------\n");
				}
				set_wps_enable(wps_band);
				_dprintf("--------- fapi_wlan_cli init End. -----------\n");
				if(nvram_get_int("x_Setting") == 1){
					_dprintf("check nvram and wireless configuration start.\n");
					rpc_parse_nvram_from_httpd(0, -1);
					rpc_parse_nvram_from_httpd(1, -1);
					config_guest_network(0);
					_dprintf("check nvram and wireless configuration end.\n");
				}
 				eval("brctl", "addif", lan_ifname, "wlan0");
 				eval("brctl", "addif", lan_ifname, "wlan2");
  				if(repeater_mode() || mediabridge_mode())
					start_wlcconnect();
				if(aimesh_re_node()){
					_dprintf("Init wlan1 and wlan3 in RE mode\n");
					//wlan_setEndpointEnabled(0);
					//wlan_setEndpointEnabled(1);
					wave_set_sta_config();
				}else if(is_default()){
					_dprintf("Init wlan1 in default state.\n");
					//wlan_setEndpointEnabled(0);
				}
				if(nvram_get_int("ATEMODE") == 1)
					_dprintf("[%s][%d] eanble usb3\n", __func__, __LINE__);
				else{
					if(nvram_get_int("usb_usb3") == 1){
						_dprintf("[%s][%d] eanble usb3\n", __func__, __LINE__);
						system("set_usb2_to_usb3");
					}else{
						_dprintf("[%s][%d] eanble usb2\n", __func__, __LINE__);
						system("set_usb3_to_usb2");
					}
				}
				memset(buf, 0, sizeof(buf));
				//wlan_getPossibleChannels(0, buf);
				nvram_set("pc_list_2g", buf);
				memset(buf, 0, sizeof(buf));
				//wlan_getPossibleChannels(2, buf);
				nvram_set("pc_list_5g", buf);
				system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
				_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db /instance/ confs/");
				nvram_set_int("wave_flag", 0);
				_dprintf("*********************** start httpd ************************\n");
				nvram_set("wave_ready", "1");
				nvram_set("success_start_service", "1");
				sync();
				sync();
				sync();
				start_bsd();
				if(!no_need_obd()){
					notify_rc("start_obd");
					iwpriv_once = 0;
				}
				break;
			case WAVE_ACTION_WEB:
				check_wave_ready(WAVE_ACTION_WEB);
				stop_bsd();
				if(access("/opt/lantiq/wave/db/instance/wlan0/", F_OK) == -1 )
					_dprintf("[warning] wlan0 configuration is not existed\n");
				if(access("/opt/lantiq/wave/db/instance/wlan2/", F_OK) == -1 )
					_dprintf("[warning] wlan2 configuration is not existed\n");
				nvram_set("wave_ready", "0");
				if(!no_need_obd())
					notify_rc("stop_obd");
				if(nvram_get("sw_mode") && nvram_get_int("sw_mode") == 3 && nvram_get_int("wlc_psta") == 1 && nvram_get_int("wlc_band") == 1){
					_dprintf("[sw_mode] skip start_psta_wave, WAVE will run scripts automatically\n");
					eval("brctl", "addif", lan_ifname, "wlan0");
					eval("brctl", "addif", lan_ifname, "wlan2");
					system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					sync();
					sync();
					sync();
					nvram_set("wave_ready", "1");
					start_bsd();
					if(!no_need_obd())
						notify_rc("start_obd");
					if(nvram_get_int("re_mode") == 1){
						wave_set_sta_config();
						iwpriv_once = 0;
					}
					break;
				}
				_dprintf("[%s][%d] call rpc_parse_nvram_from_httpd, wave_flag:[%d]\n", __func__, __LINE__, wave_flag);
				if(nvram_get_int("wave_CFG") == 11){
					_dprintf("[%s][%d] dp-00\n", __func__, __LINE__);
					rpc_parse_nvram_from_httpd(0, -1);
					rpc_parse_nvram_from_httpd(1, -1);
					nvram_set("wave_CFG", "0");
					eval("brctl", "addif", lan_ifname, "wlan0");
					eval("brctl", "addif", lan_ifname, "wlan2");
					system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					sync();
					sync();
					sync();
					nvram_set("wave_ready", "1");
					start_bsd();
					if(!no_need_obd())
						notify_rc("start_obd");
					if(nvram_get_int("re_mode") == 1){
						wave_set_sta_config();
						iwpriv_once = 0;
					}
					break;
				}
				if(wave_flag == WAVE_FLAG_QIS){
					logmessage("WAVE", "[%s][%d][WAVE_FLAG_QIS] \n", __func__, __LINE__);
					wave_set_SSID(0, -1);
					wave_set_security(0, -1);
					wave_set_SSID(1, -1);
					wave_set_security(1, -1);
					set_all_wps_config(nvram_get_int("w_Setting"));
					_dprintf("[%s][%d] wlan_ifconfigUp\n", __func__, __LINE__);
					wlan_ifconfigUp(0);
					wlan_ifconfigUp(2);
					nvram_set_int("wave_flag", 0);
					eval("brctl", "addif", lan_ifname, "wlan0");
					eval("brctl", "addif", lan_ifname, "wlan2");
					system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					sync();
					sync();
					sync();
					nvram_set("wave_ready", "1");
					start_bsd();
					if(!no_need_obd())
						notify_rc("start_obd");
					if(nvram_get_int("re_mode") == 1){
						wave_set_sta_config();
						iwpriv_once = 0;
					}
					break;
				}else if(wave_flag == WAVE_FLAG_WPS){
					logmessage("WAVE", "[%s][%d][WAVE_FLAG_WPS] \n", __func__, __LINE__);
					set_all_wps_config(nvram_get_int("w_Setting"));
					set_wps_enable(wps_band);
					eval("brctl", "addif", lan_ifname, "wlan0");
					eval("brctl", "addif", lan_ifname, "wlan2");
					system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					sync();
					sync();
					sync();
					nvram_set("wave_ready", "1");
					start_bsd();
					if(!no_need_obd())
						notify_rc("start_obd");
					if(nvram_get_int("re_mode") == 1){
						wave_set_sta_config();
						iwpriv_once = 0;
					}
					break;
				}else if(wave_flag == WAVE_FLAG_WDS){
					logmessage("WAVE", "[%s][%d][WAVE_FLAG_WDS] \n", __func__, __LINE__);
					set_all_wps_config(nvram_get_int("w_Setting"));
					rpc_update_wdslist(unit, -1);
					_dprintf("[%s][%d] wlan_ifconfigUp\n", __func__, __LINE__);
					if(unit == 0)
						wlan_ifconfigUp(0);
					else
						wlan_ifconfigUp(2);
					eval("brctl", "addif", lan_ifname, "wlan0");
					eval("brctl", "addif", lan_ifname, "wlan2");
					system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					sync();
					sync();
					sync();
					nvram_set("wave_ready", "1");
					start_bsd();
					if(!no_need_obd())
						notify_rc("start_obd");
					if(nvram_get_int("re_mode") == 1){
						wave_set_sta_config();
						iwpriv_once = 0;
					}
					break;
        		}else if(wave_flag == WAVE_FLAG_ACL){
					logmessage("WAVE", "[%s][%d][WAVE_FLAG_ACL] \n", __func__, __LINE__);
					wave_set_macmode(unit, nvram_get_int("wl_subunit"));
					set_all_wps_config(nvram_get_int("w_Setting"));
					_dprintf("[%s][%d] wlan_ifconfigUp\n", __func__, __LINE__);
					if(unit == 0)
						wlan_ifconfigUp(0);
					else
						wlan_ifconfigUp(2);
					eval("brctl", "addif", lan_ifname, "wlan0");
					eval("brctl", "addif", lan_ifname, "wlan2");
					system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					sync();
					sync();
					sync();
					nvram_set("wave_ready", "1");
					start_bsd();
					if(!no_need_obd())
						notify_rc("start_obd");
					if(nvram_get_int("re_mode") == 1){
						wave_set_sta_config();
						iwpriv_once = 0;
					}
					break;
        		}else if(wave_flag == WAVE_FLAG_ADV){
					logmessage("WAVE", "[%s][%d][WAVE_FLAG_ADV] \n", __func__, __LINE__);
					set_all_wps_config(nvram_get_int("w_Setting"));
					if(unit == 0)
						wave_set_ap_professional(0, -1);
					else
						wave_set_ap_professional(1, -1);
					if(check_country_code() == 1){
						_dprintf("[%s][%d] wlan_ifconfigUp\n", __func__, __LINE__);
						logmessage("WAVE", "[%s][%d][WAVE_FLAG_ADV][0] \n", __func__, __LINE__);
						wlan_ifconfigUp(0);
						logmessage("WAVE", "[%s][%d][WAVE_FLAG_ADV][1] \n", __func__, __LINE__);
						wlan_ifconfigUp(2);
						nvram_set_int("wl_country_changed", 7);
					}else{
						logmessage("WAVE", "[%s][%d][WAVE_FLAG_ADV][%d] \n", __func__, __LINE__, unit);
						_dprintf("[%s][%d] wlan_ifconfigUp\n", __func__, __LINE__);
						if(unit == 0)
							wlan_ifconfigUp(0);
						else
							wlan_ifconfigUp(2);
					}
					if(nvram_get_int("usb_usb3"))
						set_usb2_to_usb3();
					else
						set_usb3_to_usb2();
					eval("brctl", "addif", lan_ifname, "wlan0");
					eval("brctl", "addif", lan_ifname, "wlan2");
					system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					sync();
					sync();
					sync();
					nvram_set("wave_ready", "1");
					start_bsd();
					if(!no_need_obd())
						notify_rc("start_obd");
					if(nvram_get_int("re_mode") == 1){
						wave_set_sta_config();
						iwpriv_once = 0;
					}
					break;
        		}else if(wave_flag == WAVE_FLAG_VAP){
					logmessage("WAVE", "[%s][%d][WAVE_FLAG_VAP] \n", __func__, __LINE__);
					config_guest_network(6);
					eval("brctl", "addif", lan_ifname, "wlan0");
					eval("brctl", "addif", lan_ifname, "wlan2");
					system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					sync();
					sync();
					sync();
					nvram_set("wave_ready", "1");
					start_bsd();
					if(!no_need_obd())
						notify_rc("start_obd");
					if(nvram_get_int("re_mode") == 1){
						wave_set_sta_config();
						iwpriv_once = 0;
					}
					break;
        		}else if(wave_flag == WAVE_FLAG_APP_VAP){
					logmessage("WAVE", "[%s][%d][WAVE_FLAG_APP_VAP] \n", __func__, __LINE__);
					config_guest_network(12);
					nvram_set_int("wave_flag", 0);
					eval("brctl", "addif", lan_ifname, "wlan0");
					eval("brctl", "addif", lan_ifname, "wlan2");
					system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					sync();
					sync();
					sync();
					nvram_set("wave_ready", "1");
					start_bsd();
					if(!no_need_obd())
						notify_rc("start_obd");
					if(nvram_get_int("re_mode") == 1){
						wave_set_sta_config();
						iwpriv_once = 0;
					}
					break;
        		}else if(wave_flag == WAVE_FLAG_NETWORKMAP){
					logmessage("WAVE", "[%s][%d][WAVE_FLAG_NETWORKMAP][0] \n", __func__, __LINE__);
					ret = wave_set_SSID(0, -1);
					ret += wave_set_security(0, -1);
					if(ret <= 0){
						_dprintf("[%s][%d] skip wlan_ifconfigUp(0)\n", __func__, __LINE__);
						logmessage("WAVE", "[%s][%d][WAVE_FLAG_NETWORKMAP][1] \n", __func__, __LINE__);
					}else{
						logmessage("WAVE", "[%s][%d] wlan_ifconfigUp(0) \n", __func__, __LINE__);
						_dprintf("[%s][%d] wlan_ifconfigUp\n", __func__, __LINE__);
						wlan_ifconfigUp(0);
						logmessage("WAVE", "[%s][%d][WAVE_FLAG_NETWORKMAP][1] \n", __func__, __LINE__);
					}
					ret = wave_set_SSID(1, -1);
					ret += wave_set_security(1, -1);
					if(ret > 0){
						_dprintf("[%s][%d] wlan_ifconfigUp\n", __func__, __LINE__);
						logmessage("WAVE", "[%s][%d] wlan_ifconfigUp(2) \n", __func__, __LINE__);
						wlan_ifconfigUp(2);
						eval("brctl", "addif", lan_ifname, "wlan0");
						eval("brctl", "addif", lan_ifname, "wlan2");
						system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
						_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
						sync();
						sync();
						sync();
						nvram_set("wave_ready", "1");
						start_bsd();
						if(!no_need_obd())
							notify_rc("start_obd");
						if(nvram_get_int("re_mode") == 1){
							wave_set_sta_config();
							iwpriv_once = 0;
						}
					}else{
						_dprintf("[%s][%d] skip wlan_ifconfigUp(2)\n", __func__, __LINE__);
						eval("brctl", "addif", lan_ifname, "wlan0");
						eval("brctl", "addif", lan_ifname, "wlan2");
						system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
						_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
						sync();
						sync();
						sync();
						nvram_set("wave_ready", "1");
						start_bsd();
						if(!no_need_obd())
							notify_rc("start_obd");
						if(nvram_get_int("re_mode") == 1){
							wave_set_sta_config();
							iwpriv_once = 0;
						}
					}
				}else{
					logmessage("WAVE", "[%s][%d][no WAVE_FLAG][]\n", __func__, __LINE__);
					_dprintf("[%s][%d][no WAVE_FLAG][]\n", __func__, __LINE__);
					rpc_parse_nvram_from_httpd(0, -1);
					rpc_parse_nvram_from_httpd(1, -1);
					config_guest_network(0);
					eval("brctl", "addif", lan_ifname, "wlan0");
					eval("brctl", "addif", lan_ifname, "wlan2");
					system("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					_dprintf("rm /jffs/db_instance_20260323.tgz; cd /opt/lantiq/wave/; tar zcf /jffs/db_instance_20260323.tgz db/default/ db/instance/ confs/");
					sync();
					sync();
					sync();
					nvram_set("wave_ready", "1");
					start_bsd();
					if(!no_need_obd())
						notify_rc("start_obd");
					if(nvram_get_int("re_mode") == 1){
						wave_set_sta_config();
						iwpriv_once = 0;
					}
				}
				break;
			case WAVE_ACTION_RE_AP2G_ON:
				check_wave_ready(WAVE_ACTION_RE_AP2G_ON);
				nvram_set_int("wave_ready", 0);
				wave_set_radio_onoff(WAVE_ACTION_RE_AP2G_ON, 0, -1, 1);
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
        		break;
			case WAVE_ACTION_RE_AP2G_OFF:
				check_wave_ready(WAVE_ACTION_RE_AP2G_OFF);
				nvram_set_int("wave_ready", 0);
				wave_set_radio_onoff(WAVE_ACTION_RE_AP2G_OFF, 0, -1, 0);
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
        		break;
			case WAVE_ACTION_RE_AP5G_ON:
				check_wave_ready(WAVE_ACTION_RE_AP5G_ON);
				nvram_set_int("wave_ready", 0);
				wave_set_radio_onoff(WAVE_ACTION_RE_AP5G_ON, 1, -1, 1);
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
        		break;
			case WAVE_ACTION_RE_AP5G_OFF:
				check_wave_ready(WAVE_ACTION_RE_AP5G_OFF);
				nvram_set_int("wave_ready", 0);
				wave_set_radio_onoff(WAVE_ACTION_RE_AP5G_OFF, 1, -1, 0);
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
        		break;
			case WAVE_ACTION_SET_CHANNEL_2G:
				check_wave_ready(WAVE_ACTION_SET_CHANNEL_2G);
				nvram_set_int("wave_ready", 0);
				ret = wave_set_radio_basic_aimesh(0, -1);
				ret += wave_set_radio_tr181_aimesh(0, -1);
				if(ret > 0){
					_dprintf("\n\n\n[wlan_ifconfigUp]%s %d\n\n\n", __func__, __LINE__);
					logmessage("WAVE", "[%s][%d] wlan_ifconfigUp(0) \n", __func__, __LINE__);
					wlan_ifconfigUp(0);
				}else
					_dprintf("[%s][%d] skip wlan_ifconfigUp(0)\n", __func__, __LINE__);
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_SET_CHANNEL_5G:
				check_wave_ready(WAVE_ACTION_SET_CHANNEL_5G);
				nvram_set_int("wave_ready", 0);
				ret = wave_set_radio_basic_aimesh(1, -1);
				ret += wave_set_radio_tr181_aimesh(1, -1);
				if(ret <= 0)
					_dprintf("[%s][%d] skip wlan_ifconfigUp(2)\n", __func__, __LINE__);
				else{
					_dprintf("\n\n\n[wlan_ifconfigUp]%s %d\n\n\n", __func__, __LINE__);
					logmessage("WAVE", "[%s][%d] wlan_ifconfigUp(2) \n", __func__, __LINE__);
					wlan_ifconfigUp(2);
				}
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_OPENACL_FOR_OBD:
				check_wave_ready(WAVE_ACTION_OPENACL_FOR_OBD);
				while(!nvram_get_int("wave_ready")){
					_dprintf("wait for ready WAVE_ACTION_OPENACL_FOR_OBD\n");
					sleep(2);
				}
				nvram_set_int("wave_ready", 0);
				if(wave_set_macfilter_enable(0, -1, 0) <= 0){
					nvram_set_int("wave_ready", 1);
					iwpriv_once = 0;
					break;
				}
				wlan_ifconfigUp(0);
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_RECOVERACL_FOR_OBD:
				check_wave_ready(WAVE_ACTION_RECOVERACL_FOR_OBD);
				while(!nvram_get_int("wave_ready")){
					_dprintf("wait for ready WAVE_ACTION_RECOVERACL_FOR_OBD\n");
					sleep(2);
				}
				nvram_set_int("wave_ready", 0);
				if(wave_set_macfilter_enable(0, -1, 0) > 0)
					wlan_ifconfigUp(0);
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_SETALLOWACL_2G:
				check_wave_ready(WAVE_ACTION_SETALLOWACL_2G);
				while(!nvram_get_int("wave_ready")){
					_dprintf("wait for ready WAVE_ACTION_SETALLOWACL_2G\n");
					sleep(5);
				}
				nvram_set_int("wave_ready", 0);
				if(wave_set_macmode(0, -1) > 0)
					wlan_ifconfigUp(0);
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_SETALLOWACL_5G:
				check_wave_ready(WAVE_ACTION_SETALLOWACL_5G);
				while(!nvram_get_int("wave_ready")){
					_dprintf("wait for ready WAVE_ACTION_SETALLOWACL_5G\n");
					sleep(5);
				}
				nvram_set_int("wave_ready", 0);
				if(wave_set_macmode(1, -1) > 0)
					wlan_ifconfigUp(2);
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_SET_WPS2G_CONFIGURED:
				check_wave_ready(WAVE_ACTION_SET_WPS2G_CONFIGURED);
				nvram_set_int("wave_ready", 0);
				set_wps_config(0, nvram_get_int("w_Setting"));
				wlan_ifconfigUp(0);
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_SET_WPS5G_CONFIGURED:
				check_wave_ready(WAVE_ACTION_SET_WPS5G_CONFIGURED);
				nvram_set_int("wave_ready", 0);
				set_wps_config(1, nvram_get_int("w_Setting"));
				wlan_ifconfigUp(1);
				iwpriv_once = 0;
				break;
#ifdef RTCONFIG_AMAS
			case WAVE_ACTION_ADD_BEACON_VSIE:
				check_wave_ready(WAVE_ACTION_ADD_BEACON_VSIE);
				nvram_set_int("wave_ready", 0);
				memset(buf, 0, sizeof(buf));
				pt1 = nvram_safe_get("amas_add_beacon_vsie");
				pt2 = get_wififname(0);
				if(pt1 && *pt1 && pt2 && *pt2){
					snprintf(buf, sizeof(buf), "hostapd_cli set_vsie -i%s %d DD%02X%02X%02X%02X%s", pt2, 0, ((strlen(pt1) >> 1) + 3), 0xF8, 0x32, 0xE4, pt1);
					_dprintf("%s: cmd=%s\n", "wave_add_beacon_vsie", buf);
					system(buf);
				}
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_DEL_BEACON_VSIE:
				check_wave_ready(WAVE_ACTION_DEL_BEACON_VSIE);
				nvram_set_int("wave_ready", 0);
				memset(buf, 0, sizeof(buf));
				pt1 = nvram_safe_get("amas_del_beacon_vsie");
				pt2 = get_wififname(0);
				if(pt1 && *pt1 && pt2 && *pt2){
					snprintf(buf, sizeof(buf), "hostapd_cli del_vsie -i%s %d DD%02X%02X%02X%02X%s", pt2, 0, ((strlen(pt1) >> 1) + 3), 0xF8, 0x32, 0xE4, pt1);
					_dprintf("%s: cmd=%s\n", "wave_del_beacon_vsie", buf);
					system(buf);
				}
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_ADD_PROBE_REQ_VSIE:
				check_wave_ready(WAVE_ACTION_ADD_PROBE_REQ_VSIE);
				nvram_set_int("wave_ready", 0);
				memset(buf, 0, sizeof(buf));
				pt1 = nvram_safe_get("amas_add_probe_req_vsie");
				pt2 = get_wififname(0);
				wave_clear_all_probe_req_vsie();
				if(pt1 && *pt1 && pt2 && *pt2){
					snprintf(buf, sizeof(buf), "hostapd_cli vendor_elem_add -i%s %d DD%02X%02X%02X%02X%s", pt2, 0, ((strlen(pt1) >> 1) + 3), 0xF8, 0x32, 0xE4, pt1); 
					if(f_exists("/tmp/OBD_DEBUG") > 0)
						dbg("OBD(%d) %lu: %s: cmd=%s\n", getpid(), uptime(), "wave_add_probe_req_vsie", buf);
					system(buf);
				}
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_DEL_PROBE_REQ_VSIE:
				check_wave_ready(WAVE_ACTION_DEL_PROBE_REQ_VSIE);
				nvram_set_int("wave_ready", 0);
				memset(buf, 0, sizeof(buf));
				pt1 = nvram_safe_get("amas_del_probe_req_vsie");
				pt2 = get_wififname(0);
				if(pt1 && *pt1 && pt2 && *pt2){
					snprintf(buf, sizeof(buf), "hostapd_cli vendor_elem_remove -i%s %d DD%02X%02X%02X%02X%s", pt2, 0, ((strlen(pt1) >> 1) + 3), 0xF8, 0x32, 0xE4, pt1);
					if(f_exists("/tmp/OBD_DEBUG") > 0)
						dbg("OBD(%d) %lu: %s: cmd=%s\n", getpid(), uptime(), "wave_del_probe_req_vsie", buf);
					system(buf);
				}
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
			case WAVE_ACTION_CLEAR_ALL_PROBE_REQ_VSIE:
				check_wave_ready(WAVE_ACTION_CLEAR_ALL_PROBE_REQ_VSIE);
				nvram_set_int("wave_ready", 0);
				wave_clear_all_probe_req_vsie();
				nvram_set_int("wave_ready", 1);
				iwpriv_once = 0;
				break;
#endif
			case WAVE_ACTION_SET_STA_CONFIG:
				wave_set_sta_config();
				iwpriv_once = 0;
				break;
			default:
				iwpriv_once = 0;
				break;
		}
	}

	if(iwpriv_once == 0){
		system("iwpriv wlan0 sSlowProbingMask 0x3e");
		system("iwpriv wlan2 sSlowProbingMask 0x3e");
		iwpriv_once = 1;
	}
	if(nvram_get_int("wl1_mumimo") == 1){
		_dprintf("[%s][%d] enable wlan2 MU-MIMO\n", __func__, __LINE__);
		system("iwpriv wlan2 sMuOperation 1");
	}else {
		_dprintf("[%s][%d] disable wlan2 MU-MIMO\n", __func__, __LINE__);
		system("iwpriv wlan2 sMuOperation 0");
	}
	_dprintf( "[%s][%d] end of wave_monitor_core(),action:[%d] signal_idx:[%d]\n", __func__, __LINE__, wave_action, wave_signal_idx);
	++wave_signal_idx;
	if(nvram_match("x_Setting", "1"))
		stop_bluetooth_service();
	for(i = 20; i> 0; i--){
		if(!restart_wifi && !restart_qo && !restart_fwl){
			if(wave_signal_idx > 100)
				wave_signal_idx = 0;
			nvram_unset("wave_action_cur");
			return 0;
		}
		_dprintf("[%s][%d] need to restart wifi/qos/firewall again, chk %d/%d/%d\n", __func__, __LINE__, nvram_get_int("restart_wifi"), nvram_get_int("restart_qo"), nvram_get_int("restart_fwl"));
		sleep(1);
		if(restart_wifi && nvram_match("restart_wifi", "0")){
			_dprintf("[%s][%d] : restart wireless again\n", __func__, __LINE__);
			restart_wifi = 0;
			if(check_wave_ready(3) == -1)
				return -1;
			notify_rc("restart_wireless");
		}
		if(restart_qo && nvram_match("restart_qo", "0")){
			_dprintf("[%s][%d] : restart qos again\n", __func__, __LINE__);
			restart_qo = 0;
			notify_rc("restart_qos");
		}
		if(restart_fwl && nvram_match("restart_fwl", "0")){
			_dprintf("[%s][%d] : restart firewall again\n", __func__, __LINE__);
			restart_fwl = 0;
			notify_rc("restart_firewall");
		}
	}
	if(wave_signal_idx > 100)
		wave_signal_idx = 0;
	nvram_unset("wave_action_cur");
	return 0;
}
