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
 * Copyright 2019, ASUSTeK Inc.
 * Copyright 2023, SWRTdev.
 * Copyright 2023, paldier <paldier@hotmail.com>.
 * All Rights Reserved.
 *
 */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <stdio.h>
//#include <linux/in.h>
#if !defined(RTCONFIG_MUSL_LIBC) && !defined(MUSL_LIBC)
#include <linux/if_ether.h>
#endif
#include <net/if.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <bcmnvram.h>
#include "networkmap.h"
#include "sm.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <asm/byteorder.h>
#include "iboxcom.h"
#include "../shared/shutils.h"
#ifdef RTCONFIG_NOTIFICATION_CENTER
#include <libnt.h>
#endif
#include <shared.h>
#if defined(RTCONFIG_RALINK)
#include <ralink.h>
#elif defined(RTCONFIG_LANTIQ)
#include <lantiq.h>
#elif defined(RTCONFIG_QCA)
#include <qca.h>
#elif defined(RTCONFIG_BCMARM)
#include <wlutils.h>
#include <wlioctl.h>
#include <wlscan.h>
#endif

typedef struct sta_info_table STA_INFO_TABLE;
struct sta_info_table{
	unsigned char mac_addr[6];
	unsigned char wireless;//0:wired 1:2.4G 2:5G 3:5G-2
	char txrate[7];
	char rxrate[10];
	unsigned int rssi;
	char conn_time[12];
	STA_INFO_TABLE *next;
};
int g_show_sta_info = 0;
STA_INFO_TABLE *g_sta_info_tab = NULL;

#if defined(RTCONFIG_RALINK)
#include "mtk.c"
#elif defined(RTCONFIG_QCA)
#include "qca.c"
#elif defined(RTCONFIG_HND_ROUTER)
#include "hnd.c"
#elif defined(RTCONFIG_BCMARM)
#include "bcm.c"
#endif

void conn_debug_info()
{
	STA_INFO_TABLE *tmp;
	tmp = g_sta_info_tab;
	while(tmp != NULL){
		if(f_exists("/tmp/conn_debug"))
			_dprintf("%02X%02X%02X%02X%02X%02X\n", tmp->mac_addr[0], tmp->mac_addr[1], tmp->mac_addr[2], tmp->mac_addr[3], tmp->mac_addr[4], tmp->mac_addr[5]);
		tmp = tmp->next;
	}
}

void nmp_wl_offline_check(CLIENT_DETAIL_INFO_TABLE *p_client_tab, int offline)
{
	int i = 0;
	char word[128];
	char *next;
	STA_INFO_TABLE *tmp;
	if(f_exists("/tmp/conn_debug")){
		g_show_sta_info = 1;
		_dprintf("%s read wireless connections offline check %d\n", "[connection log]", offline);
#if defined(RTCONFIG_RALINK)
		_dprintf("MTK stainfo start\n");
#elif defined(RTCONFIG_QCA)
		_dprintf("QCA stainfo start\n");
#elif defined(RTCONFIG_LANTIQ)
		_dprintf("LANTIQ stainfo start\n");
#elif defined(RTCONFIG_BCMARM)
		_dprintf("BCM stainfo start\n");
#endif
	}else
		g_show_sta_info = 0;
#if defined(RTCONFIG_BCMARM)
	BCM_stainfo();
#else
	foreach(word, nvram_safe_get("wl_ifnames"), next){
		if(i < MAX_NR_WL_IF){
			if(nvram_get(wl_nvname("nband", i, 0)))
#if defined(RTCONFIG_RALINK)
				MTK_stainfo(i);
#elif defined(RTCONFIG_QCA)
				QCA_stainfo(word);
#elif defined(RTCONFIG_LANTIQ)
				LANTIQ_stainfo(word);
#endif
		}
		i++;
	}
#endif
	if(offline == 0){
		if(g_sta_info_tab){
			for(i = 0; i < p_client_tab->detail_info_num; i++){
				for(tmp = g_sta_info_tab; tmp != NULL; tmp = tmp->next){
					if(memcmp(p_client_tab->mac_addr[i], tmp->mac_addr, sizeof(p_client_tab->mac_addr[0])))
						continue;
					p_client_tab->wireless[i] = tmp->wireless;
					strlcpy(p_client_tab->txrate[i], tmp->txrate, sizeof(p_client_tab->txrate[0]));
					strlcpy(p_client_tab->rxrate[i], tmp->rxrate, sizeof(p_client_tab->rxrate[0]));
					p_client_tab->rssi[i] = tmp->rssi;
					strlcpy(p_client_tab->conn_time[i], tmp->conn_time, sizeof(p_client_tab->conn_time[0]));
					if(g_show_sta_info && f_exists("/tmp/conn_debug")){
						_dprintf("###%d client wl: %d, rx %s tx %s rssi %d conn_time %s \n", i, p_client_tab->wireless[i], 
							p_client_tab->rxrate[i], p_client_tab->txrate[i], p_client_tab->rssi[i], p_client_tab->conn_time[i]);
					}
				}
			}
		}
		if(g_show_sta_info != 0)
			conn_debug_info();
	}else{
		if(p_client_tab->detail_info_num > 0){
			for(i = 0; i < p_client_tab->detail_info_num; i++){
				if(p_client_tab->wireless[i] == 0)
					continue;
				if(g_show_sta_info == 0 && g_sta_info_tab == NULL){
					p_client_tab->device_flag[i] &= 0xf7;
					continue;
				}
				if(f_exists("/tmp/conn_debug"))
					_dprintf(" ###%d wireless client check: %02x%02x%02x%02x%02x%02x \n", i, p_client_tab->mac_addr[i][0], p_client_tab->mac_addr[i][1],
						p_client_tab->mac_addr[i][2], p_client_tab->mac_addr[i][3], p_client_tab->mac_addr[i][4], p_client_tab->mac_addr[i][5]);
				if(g_sta_info_tab){
					tmp = g_sta_info_tab;
					while(memcmp(p_client_tab->mac_addr[i], tmp->mac_addr, sizeof(p_client_tab->mac_addr[0]))){
						tmp = tmp->next;
						if(tmp == NULL){
							if(g_show_sta_info && f_exists("/tmp/conn_debug"))
								printf("### %d client leave! wireless: %d\n", i, p_client_tab->wireless[i]);		
							p_client_tab->device_flag[i] &= 0xf7;
							break;
						}
					}
					if(tmp == NULL)
						continue;
					p_client_tab->wireless[i] = tmp->wireless;
					strlcpy(p_client_tab->txrate[i], tmp->txrate, sizeof(p_client_tab->txrate[0]));
					strlcpy(p_client_tab->rxrate[i], tmp->rxrate, sizeof(p_client_tab->rxrate[0]));
					strlcpy(p_client_tab->conn_time[i], tmp->conn_time, sizeof(p_client_tab->conn_time[0]));
					if(g_show_sta_info && f_exists("/tmp/conn_debug"))
						_dprintf("### check: %d client wireless: %d\n", i, p_client_tab->wireless[i]);
					p_client_tab->device_flag[i] |= 8;
				}else{
					if(g_show_sta_info && f_exists("/tmp/conn_debug"))
						_dprintf("### %d client leave! wireless: %d\n", i, p_client_tab->wireless[i]);		
					p_client_tab->device_flag[i] &= 0xf7;
				}
			}
		}else if(g_show_sta_info != 0)
			conn_debug_info();
	}

	if(g_sta_info_tab){
		tmp = g_sta_info_tab;
		while(tmp != NULL){
			g_sta_info_tab = tmp->next;
			free(tmp);
			tmp = g_sta_info_tab;
		}
	}
	g_sta_info_tab = NULL;
}


