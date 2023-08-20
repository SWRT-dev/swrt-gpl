/*
 * Copyright 2023, SWRTdev
 * Copyright 2023, paldier <paldier@hotmail.com>.
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
#include <wpa_ctrl.h>
#include <wpa_helpers.h>

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
#ifdef RTCONFIG_LANTIQ
	if(!strcmp(ifname, "wlan0")){
		for(count = 10; count > 0; --count){
			if(!is_hostapd_running("wlan0")){
				if(!pids("hostpad_cli_wlan0"))
					break;
			}
			system("kill -9 `pidof hostapd_cli_wlan0`");
			system("kill -9 `pidof hostapd_wlan0`");
			sleep(1);
			_dprintf("%s : kill hostapd_cli_wlan0\n", __func__);
			_dprintf("%s : kill hostapd_wlan0\n", __func__);
		}
		for(count = 11; count > 0; --count){
			if(is_hostapd_running("wlan0"))
				break;
			system("/tmp/hostapd_wlan0 /opt/lantiq/wave/confs/hostapd_wlan0.conf -e /tmp/hostapd_ent_wlan0 -B");
			sleep(1);
			_dprintf("%s : run hostapd_wlan0\n", __func__);
		}
		for(count = 11; count > 0; --count){
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
#elif defined(RTCONFIG_RALINK)
	if(!strcmp(ifname, WIF_2G)){
		for(count = 10; count > 0; --count){
			if(!is_hostapd_running(WIF_2G)){
				if(!pids("hostpad_cli_ra0"))
					break;
			}
			doSystem("kill -9 `pidof hostapd_cli_%s`", WIF_2G);
			doSystem("kill -9 `pidof hostapd_%s`", WIF_2G);
			sleep(1);
			_dprintf("%s : kill hostapd_cli_%s\n", __func__, WIF_2G);
			_dprintf("%s : kill hostapd_%s\n", __func__, WIF_2G);
		}
		for(count = 11; count > 0; --count){
			if(is_hostapd_running(WIF_2G))
				break;
			doSystem("/tmp/hostapd_%s -B -P /var/run/hostapd_%s.pid -f /tmp/hostapd_%s.log -e /var/run/entropy_%s.bin /etc/Wireless/hostapd_%s.conf", WIF_2G, WIF_2G, WIF_2G, WIF_2G, WIF_2G);
			sleep(1);
			_dprintf("%s : run hostapd_%s\n", __func__, WIF_2G);
		}
//		for(count = 11; count > 0; --count){
//			if(pids("hostapd_cli_ra0"))
//				break;
//			doSystem("/tmp/hostapd_cli_%s -i%s -a/usr/sbin/ralink_events_hostapd.sh -B", WIF_2G, WIF_2G);
//			sleep(1);
//			_dprintf("%s : run hostapd_cli_%s\n", __func__, WIF_2G);
//		}
	}
	else if(!strcmp(ifname, WIF_5G)){
		for(count = 10; count > 0; --count){
			if(!is_hostapd_running(WIF_5G)){
#if defined(RTCONFIG_MT798X)
				if(!pids("hostpad_cli_rax0"))
#else
				if(!pids("hostpad_cli_rai0"))
#endif
					break;
			}
			doSystem("kill -9 `pidof hostapd_cli_%s`", WIF_5G);
			doSystem("kill -9 `pidof hostapd_%s`", WIF_5G);
			sleep(1);
			_dprintf("%s : kill hostapd_cli_%s\n", __func__, WIF_5G);
			_dprintf("%s : kill hostapd_%s\n", __func__, WIF_5G);
		}
		for(count = 11; count > 0; --count){
			if(is_hostapd_running(WIF_5G))
				break;
			doSystem("/tmp/hostapd_%s -B -P /var/run/hostapd_%s.pid -f /tmp/hostapd_%s.log -e /var/run/entropy_%s.bin /etc/Wireless/hostapd_%s.conf", WIF_5G, WIF_5G, WIF_5G, WIF_5G, WIF_5G);
			sleep(1);
			_dprintf("%s : run hostapd_%s\n", __func__, WIF_5G);
		}
//		for(count = 11; count > 0; --count){
#if defined(RTCONFIG_MT798X)
//			if(pids("hostpad_cli_rax0"))
#else
//			if(pids("hostpad_cli_rai0"))
#endif
//				break;
//			doSystem("/tmp/hostapd_cli_%s -i%s -a/usr/sbin/ralink_events_hostapd.sh -B", WIF_5G, WIF_5G);
//			sleep(1);
//			_dprintf("%s : run hostapd_cli_%s\n", __func__, WIF_5G);
//		}
	}
#elif defined(RTCONFIG_QCA)
#endif
}
