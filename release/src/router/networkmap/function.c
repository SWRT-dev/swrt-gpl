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
 * Copyright 2023-2024, SWRTdev.
 * Copyright 2023-2024, paldier <paldier@hotmail.com>.
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

extern ac_state *g_conv_state;

#ifdef RTCONFIG_NOTIFICATION_CENTER
extern int TRIGGER_FLAG;

int nt_send_trigger_event(unsigned char flag, int event)
{
	NOTIFY_EVENT_T *event_t = initial_nt_event();
	event_t->event = event;
	event_t->msg[0] = 0x0;
	NMP_DEBUG("NT_CENTER: Send event ID:%08x !\n", event_t->event);
	send_trigger_event(event_t);
	nt_event_free(event_t);
	TRIGGER_FLAG |= (1 << flag);
	NMP_DEBUG("check TRIGGER_FLAG %d\n", TRIGGER_FLAG);
	nvram_set_int("networkmap_trigger_flag", TRIGGER_FLAG);
	nvram_commint();

	return 0;
}

int nt_send_trigger_event2(unsigned char flag, int event)
{
	if(((TRIGGER_FLAG >> flag) & 1) == 0){
		nt_send_trigger_event(flag, event);
	}
	return 0;
}

void nt_send_event_update(void)
{
	struct json_object *root = NULL;
	root = json_object_new_object();
	json_object_object_add(root, "from", json_object_new_string("NETWORKMAP"));
	SEND_NT_EVENT(GENERAL_DEV_UPDATE, json_object_to_json_string(root));
	if(root)
		json_object_put(root);
}
#endif

/*
* 0:error or same ip/mac, 1:different ip/mac
*/
int FindDevice(unsigned char *pIP, unsigned char *pMac, int replaceMac)
{
	FILE *fp;
	char line[256], dev_mac[18], dev_ip[16];
	char *next, *ret;
	char *macaddr, *ipaddr, *name, *expire;
	unsigned int expires;

	if(!nvram_get_int("dhcp_enable_x") || !nvram_match("sw_mode", "1"))
		return 0;

	sprintf(dev_ip, "%d.%d.%d.%d", *pIP, *(pIP+1), *(pIP+2), *(pIP+3));
	sprintf(dev_mac, "%02x:%02x:%02x:%02x:%02x:%02x", *pMac, *(pMac+1), *(pMac+2), *(pMac+3), *(pMac+4), *(pMac+5));

	/* Read leases file */
	if (!(fp = fopen("/var/lib/misc/dnsmasq.leases", "r")))
		return 0;
	fcntl(fileno(fp), F_SETFL, fcntl(fileno(fp), F_GETFL) | O_NONBLOCK);
	while ((next = fgets(line, sizeof(line), fp)) != NULL) {
		if (vstrsep(next, " ", &expire, &macaddr, &ipaddr, &name) == 4) {
			if(replaceMac && !strcmp(ipaddr, dev_ip) && strcmp(macaddr, dev_mac)){
				sscanf(macaddr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &pMac[0], &pMac[1], &pMac[2], &pMac[3], &pMac[4], &pMac[5]);
				fclose(fp);
				return 1;
			}else if(!strcmp(macaddr, dev_mac) && strcmp(ipaddr, dev_ip)){
				sscanf(ipaddr, "%hhu.%hhu.%hhu.%hhu", &pIP[0], &pIP[1], &pIP[2], &pIP[3]);
				fclose(fp);
				return 1;
			}
		}
	}
	fclose(fp);
	return 0;
}

int FindHostname(P_CLIENT_DETAIL_INFO_TABLE p_client_detail_info_tab, int i)
{
	unsigned char base;
	char ipaddr[16], macaddr[18];
	char *nv, *nvp, *b;
	char *mac, *ip, *name, *expire;
	FILE *fp;
	char line[256];
	char *next;
	P_CLIENT_DETAIL_INFO_TABLE tab = p_client_detail_info_tab;
	snprintf(ipaddr, sizeof(ipaddr), "%d.%d.%d.%d", tab->ip_addr[i][0], tab->ip_addr[i][1],
		tab->ip_addr[i][2], tab->ip_addr[i][3]);
	snprintf(macaddr, sizeof(macaddr), "%02X:%02X:%02X:%02X:%02X:%02X",
		tab->mac_addr[i][0], tab->mac_addr[i][1], tab->mac_addr[i][2], tab->mac_addr[i][3],
		tab->mac_addr[i][4], tab->mac_addr[i][5]);
// Get current hostname from DHCP leases
	if (!nvram_get_int("dhcp_enable_x") || !nvram_match("sw_mode", "1"))
		return 0;

	if ((fp = fopen("/var/lib/misc/dnsmasq.leases", "r"))) {
		fcntl(fileno(fp), F_SETFL, fcntl(fileno(fp), F_GETFL) | O_NONBLOCK);
		while ((next = fgets(line, sizeof(line), fp)) != NULL) {
			if (vstrsep(next, " ", &expire, &mac, &ip, &name) == 4) {
				if(!strcmp(ipaddr, ip)){
				    if(*name &&
				    !strchr(name, '*') &&	// Ensure it's not a clientid in
				    !strchr(name, ':')){	// case device didn't have a hostname
						strlcpy(tab->device_name[i], name, sizeof(tab->device_name[0]));
						strlcpy(tab->device_type[i], name, sizeof(tab->device_type[0]));
						unsigned char type = tab->type[i];
						if(type == 0 || type == 22 || type > 31){
							unsigned char type2;
							unsigned char *device_name = strdup(tab->device_name[i]);
							unsigned char *tmp = device_name;
							if(*device_name){
								unsigned int c;
								for(c = *tmp; *tmp; tmp++, c = *tmp){
									if(c - 'A' < 26)
										*tmp = c + 32;
								}
							}
							type2 = full_search(g_conv_state, device_name, &base);
							if(type2){
								type_filter(tab, i, type2, base, 0);
								NMP_DEBUG("DHCP: Find type :%d, os_type = %d \n", type2, base);
							}
							free(device_name);
						}
						find_wireless_device(tab, i);
						if(!strcmp(ipaddr, ip)){
							strlcpy(tab->ipMethod[i], "DHCP", sizeof(tab->ipMethod[0]));
							if(strcmp(macaddr, mac))
								sscanf(mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &tab->mac_addr[i][0], 
									&tab->mac_addr[i][1], &tab->mac_addr[i][2], &tab->mac_addr[i][3], 
									&tab->mac_addr[i][4], &tab->mac_addr[i][5]);
						}
#ifdef RTCONFIG_NOTIFICATION_CENTER
						if(tab->type[i] == 1)
							nt_send_trigger_event2(FLAG_SAMBA_INLAN, HINT_SAMBA_INLAN_EVENT);
						if(tab->type[i] == 7)
							nt_send_trigger_event2(FLAG_XBOX_PS, HINT_XBOX_PS_EVENT);
						if(tab->type[i] == 27)
							nt_send_trigger_event2(FLAG_UPNP_RENDERER, HINT_UPNP_RENDERER_EVENT);
						if(tab->type[i] == 6)
							nt_send_trigger_event2(FLAG_OSX_INLAN, HINT_OSX_INLAN_EVENT);
#endif
					}else{
						strlcpy(tab->ipMethod[i], "DHCP", sizeof(tab->ipMethod[0]));
						if(strcmp(macaddr, mac))
							sscanf(mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &tab->mac_addr[i][0], 
								&tab->mac_addr[i][1], &tab->mac_addr[i][2], &tab->mac_addr[i][3], 
								&tab->mac_addr[i][4], &tab->mac_addr[i][5]);
					}
				}
			}
		}
		fclose(fp);
	}

// Get names from static lease list, overruling anything else
	nv = nvp = strdup(nvram_safe_get("dhcp_staticlist"));

	if (nv) {
		while ((b = strsep(&nvp, "<")) != NULL) {
			if ((vstrsep(b, ">", &mac, &ip) == 2) && *ip) {
				if (!strcmp(ipaddr, ip))
					strlcpy(tab->ipMethod[i], "Manual", sizeof(tab->ipMethod[0]));
			}
		}
		free(nv);
	}

	return 1;
}

void find_wireless_device(P_CLIENT_DETAIL_INFO_TABLE p_client_detail_info_tab, int offline)
{
	int n;
	char *nv, *nvp, *b;
	char *android = "android";
	char *device_name = p_client_detail_info_tab->device_name[offline];
	nv = nvp = strdup(device_name);
	if (nv) {
		for(n = *nv; *nv; nv++, n = *nv){
			if((n - 65) < 26)
				*nv = n + 32;
		}
	}
	if(strstr(nvp, android)){
		memset(device_name, 0, sizeof(p_client_detail_info_tab->device_name[offline]));
		if(p_client_detail_info_tab->vendor_name[offline][0] && strlen(p_client_detail_info_tab->vendor_name[offline]) < 23)
			sprintf(device_name, "%s(android)", p_client_detail_info_tab->vendor_name[offline]);
		else
			sprintf(device_name, "%s", android);
	}
	free(nvp);
	NMP_DEBUG("android device filter:\n%s\n", device_name);
}
