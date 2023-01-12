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
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <netinet/in.h>
#if !defined(RTCONFIG_MUSL_LIBC) && !defined(MUSL_LIBC)
#include <linux/if_ether.h>
#endif
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "../shared/shutils.h"    // for eval()
#include "../shared/rtstate.h"
#include <bcmnvram.h>
#include <stdlib.h>
#include <asm/byteorder.h>
#include "networkmap.h"
//#include "endianness.h"
//2011.02 Yau add shard memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <rtconfig.h>
//#include "asusdiscovery.h"
#ifdef RTCONFIG_BWDPI
#include <bwdpi.h>
#endif
#ifdef RTCONFIG_NOTIFICATION_CENTER
#include <libnt.h>
int TRIGGER_FLAG;
#endif
#include <json.h>

#define vstrsep(buf, sep, args...) _vstrsep(buf, sep, args, NULL)

unsigned char my_hwaddr[6];
unsigned char my_ipaddr[4];
unsigned char my_ipmask[4];
unsigned char gateway_ipaddr[4] = {0};
unsigned char broadcast_hwaddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int networkmap_fullscan, lock;
static int scan_count = 0, sw_mode = 0;
static int delete_sig;
int arp_full_scan = 0, update_oui = 0;
int networkmap_lock, daemon_exit = 0;
struct json_object *nmp_cl_json = NULL, *networkmap_oui_json = NULL;
CLIENT_DETAIL_INFO_TABLE *G_CLIENT_TAB = NULL;
ac_state *g_conv_state = NULL, *g_bwdpi_state = NULL, *g_vendor_state= NULL;

convType nmp_vendor_type[] = {
	{0, "ADOBE", BASE_TYPE_DEFAULT},
	{0, "Amazon", BASE_TYPE_DEFAULT},
	{0, "Apple", BASE_TYPE_APPLE},
	{0, "ASUS", BASE_TYPE_ASUS},
	{0, "Asus", BASE_TYPE_ASUS},
	{0, "BELKIN", BASE_TYPE_DEFAULT},
	{0, "Belkin", BASE_TYPE_DEFAULT},
	{0, "BizLink", BASE_TYPE_DEFAULT},
	{0, "BUFFALO", BASE_TYPE_DEFAULT},
	{0, "Dell", BASE_TYPE_DEFAULT},
	{0, "DellKing", BASE_TYPE_DEFAULT},
	{0, "D-Link", BASE_TYPE_DEFAULT},
	{0, "FUJITSU", BASE_TYPE_DEFAULT},
	{0, "Fujitsu", BASE_TYPE_DEFAULT},
	{0, "Google", BASE_TYPE_DEFAULT},
	{0, "HON HAI", BASE_TYPE_DEFAULT},
	{0, "Hon Hai", BASE_TYPE_DEFAULT},
	{0, "HTC", BASE_TYPE_DEFAULT},
	{0, "HUAWEI", BASE_TYPE_DEFAULT},
	{0, "Huawei", BASE_TYPE_DEFAULT},
	{0, "IBM", BASE_TYPE_DEFAULT},
	{0, "Lenovo", BASE_TYPE_DEFAULT},
	{0, "NEC ", BASE_TYPE_DEFAULT},
	{0, "MICROSOFT", BASE_TYPE_WINDOW},
	{0, "Microsoft", BASE_TYPE_WINDOW},
	{TYPE_WINDOWS, "MSFT 5.0", BASE_TYPE_WINDOW},
	{TYPE_WINDOWS, "MSFT", BASE_TYPE_WINDOW},
	{TYPE_LINUX_DEVICE, "dhcpcd", BASE_TYPE_LINUX},
	{9, "android", BASE_TYPE_ANDROID},
	{0, "Panasonic", BASE_TYPE_DEFAULT},
	{0, "PANASONIC", BASE_TYPE_DEFAULT},
	{0, "PIONEER", BASE_TYPE_DEFAULT},
	{0, "Pioneer", BASE_TYPE_DEFAULT},
	{0, "Ralink", BASE_TYPE_DEFAULT},
	{0, "Samsung", BASE_TYPE_DEFAULT},
	{0, "SAMSUNG", BASE_TYPE_DEFAULT},
	{0, "Sony", BASE_TYPE_DEFAULT},
	{0, "Synology", BASE_TYPE_DEFAULT},
	{0, "TOSHIBA", BASE_TYPE_DEFAULT},
	{0, "Toshiba", BASE_TYPE_DEFAULT},
	{0, "TP-LINK", BASE_TYPE_DEFAULT},
	{0, "VMware", BASE_TYPE_DEFAULT},
	{5, "AiCam", BASE_TYPE_ANDROID},
	{28, "ZenFone", BASE_TYPE_ANDROID},
	{28, "ASUS_Phone", BASE_TYPE_ANDROID},
	{0, NULL, BASE_TYPE_DEFAULT},
};

convType nmp_bwdpi_type[] = {
	{2, "Wireless", BASE_TYPE_DEFAULT},
	{2, "Router", BASE_TYPE_DEFAULT},
	{2, "Voip Gateway", BASE_TYPE_DEFAULT},
	{4, "NAS", BASE_TYPE_DEFAULT},
	{5, "IP Network Camera", BASE_TYPE_ASUS},
	{6, "Mac OS", BASE_TYPE_APPLE},
	{7, "Game Console", BASE_TYPE_DEFAULT},
	{9, "Android Device", BASE_TYPE_ANDROID},
	{9, "Smartphone", BASE_TYPE_ANDROID},
	{9, "Voip Phone", BASE_TYPE_ANDROID},
	{9, "MiPhone", BASE_TYPE_ANDROID},
	{10, "Apple iOS Device", BASE_TYPE_APPLE},
	{10, "iPhone", BASE_TYPE_APPLE},
	{11, "Apple TV", BASE_TYPE_APPLE},
	{14, "Macintosh", BASE_TYPE_APPLE},
	{18, "Printer", BASE_TYPE_DEFAULT},
	{19, "Windows Phone", BASE_TYPE_WINDOW},
	{19, "Nokia", BASE_TYPE_DEFAULT},
	{19, "Windows Mobile", BASE_TYPE_WINDOW},
	{20, "Tablet", BASE_TYPE_ANDROID},
	{21, "iPad", BASE_TYPE_APPLE},
	{23, "SmartTV", BASE_TYPE_DEFAULT},
	{23, "Kindle", BASE_TYPE_DEFAULT},
	{25, "Fire TV", BASE_TYPE_DEFAULT},
	{26, "Scanner", BASE_TYPE_DEFAULT},
	{27, "Chromecast", BASE_TYPE_DEFAULT},
	{28, "ZenFone", BASE_TYPE_ASUS},
	{28, "PadFone", BASE_TYPE_ASUS},
	{29, "Asus Pad", BASE_TYPE_ASUS},
	{29, "Asus ZenPad", BASE_TYPE_ASUS},
	{29, "Transformer", BASE_TYPE_ASUS},
	{34, "Desktop/Laptop", BASE_TYPE_DEFAULT},
	{0, NULL, BASE_TYPE_DEFAULT},
};

convType nmp_conv_type[] = {
	{1, "win", BASE_TYPE_WINDOW},
	{1, "pc", BASE_TYPE_WINDOW},
	{1, "nb", BASE_TYPE_WINDOW},
	{2, "rt-", BASE_TYPE_DEFAULT},
	{2, "pl-", BASE_TYPE_DEFAULT},
	{4, "storage", BASE_TYPE_DEFAULT},
	{4, "nas", BASE_TYPE_DEFAULT},
	{4, "synology", BASE_TYPE_DEFAULT},
	{5, "cam", BASE_TYPE_ASUS},
	{5, "AiCam", BASE_TYPE_DEFAULT},
	{5, "blink-mini", BASE_TYPE_DEFAULT},
	{5, "ASUS", BASE_TYPE_ASUS},
	{6, "mac", BASE_TYPE_APPLE},
	{6, "mbp", BASE_TYPE_APPLE},
	{6, "mba", BASE_TYPE_APPLE},
	{7, "play station", BASE_TYPE_DEFAULT},
	{7, "playstation", BASE_TYPE_DEFAULT},
	{7, "PS5", BASE_TYPE_DEFAULT},
	{7, "xbox", BASE_TYPE_DEFAULT},
	{9, "android", BASE_TYPE_ANDROID},
	{9, "htc", BASE_TYPE_ANDROID},
	{9, "MiPhone", BASE_TYPE_ANDROID},
	{10, "iphone", BASE_TYPE_APPLE},
	{10, "ipod", BASE_TYPE_APPLE},
	{11, "appletv", BASE_TYPE_APPLE},
	{11, "apple tv", BASE_TYPE_APPLE},
	{11, "apple-tv", BASE_TYPE_APPLE},
	{14, "imac", BASE_TYPE_APPLE},
	{15, "rog", BASE_TYPE_DEFAULT},
	{18, "epson", BASE_TYPE_DEFAULT},
	{18, "fuji xerox", BASE_TYPE_DEFAULT},
	{18, "canon", BASE_TYPE_DEFAULT},
	{18, "brother", BASE_TYPE_DEFAULT},
	{21, "ipad", BASE_TYPE_APPLE},
	{TYPE_LINUX_DEVICE, "linux", BASE_TYPE_LINUX},
	{24, "rp-", BASE_TYPE_DEFAULT},
	{24, "ea-", BASE_TYPE_DEFAULT},
	{24, "wmp-", BASE_TYPE_DEFAULT},
	{27, "chromecast", BASE_TYPE_DEFAULT},
	{33, "iqoo-", BASE_TYPE_DEFAULT},
	{34, "desktop", BASE_TYPE_DEFAULT},
	{35, "thinkpad", BASE_TYPE_DEFAULT},
	{52, "chunmi-cooker", BASE_TYPE_DEFAULT},
	{54, "zhimi-airpurifier", BASE_TYPE_DEFAULT},
	{55, "zhimi-fan", BASE_TYPE_DEFAULT},
	{55, "dmaker-fan", BASE_TYPE_DEFAULT},
	{55, "leshow-fan", BASE_TYPE_DEFAULT},
	{60, "yeelink-light", BASE_TYPE_DEFAULT},
	{60, "yilai-light", BASE_TYPE_DEFAULT},
	{60, "philips-light", BASE_TYPE_DEFAULT},
	{60, "philips-hue", BASE_TYPE_DEFAULT},
	{60, "opple-light", BASE_TYPE_DEFAULT},
	{60, "leshi-hue", BASE_TYPE_DEFAULT},
	{60, "huayi-hue", BASE_TYPE_DEFAULT},
	{60, "lifx", BASE_TYPE_DEFAULT},
	{60, "ge_switch", BASE_TYPE_DEFAULT},
	{61, "rockrobo", BASE_TYPE_DEFAULT},
	{70, "chuangmi-plug", BASE_TYPE_DEFAULT},
	{70, "hs100", BASE_TYPE_DEFAULT},
	{70, "hs103", BASE_TYPE_DEFAULT},
	{70, "Etekcity-Outlet", BASE_TYPE_DEFAULT},
	{70, "Wall-Outlet", BASE_TYPE_DEFAULT},
	{70, "AmazonPlug", BASE_TYPE_DEFAULT},
	{70, "cuco-plug", BASE_TYPE_DEFAULT},
	{70, "SmartPlug", BASE_TYPE_DEFAULT},
	{70, "Smart_Plug", BASE_TYPE_DEFAULT},
	{70, "shellyplug", BASE_TYPE_DEFAULT},
	{70, "EZPlug", BASE_TYPE_DEFAULT},
	{70, "KONKE_miniK", BASE_TYPE_DEFAULT},
	{70, "ge_plug", BASE_TYPE_DEFAULT},
	{78, "switch", BASE_TYPE_DEFAULT},
	{81, "Google-Nest", BASE_TYPE_DEFAULT},
	{81, "Google-Home", BASE_TYPE_DEFAULT},
	{81, "Sonos", BASE_TYPE_DEFAULT},
	{85, "RingPro", BASE_TYPE_DEFAULT},
	{0, NULL, BASE_TYPE_DEFAULT},
};

struct vc_list_s {
	char *device;
	int type;
};
struct vc_list_s vendorclasslist[] = {
	{"android-dhcp-10", BASE_TYPE_ANDROID},
	{"android-dhcp-7.1.1", BASE_TYPE_ANDROID},
	{"MSFT 5.0", BASE_TYPE_WINDOW},
	{"dhcpcd-5.5.6", BASE_TYPE_LINUX},
	{NULL, BASE_TYPE_DEFAULT}
};

extern int nmp_wl_offline_check(CLIENT_DETAIL_INFO_TABLE *p_client_tab, int offline);
extern int FindDevice(unsigned char *pIP, unsigned char *pMac, int replaceMac);

static int is_same_subnet(struct in_addr *ip1, struct in_addr *ip2, struct in_addr *msk)
{
	unsigned int mask = ntohl(msk->s_addr);

	return ((ntohl(ip1->s_addr) & mask) == (ntohl(ip2->s_addr) & mask)) ? 1 : 0;
}

static int check_db_size()
{
	FILE *fp;
	int size, ret;

	NMP_DEBUG("check database size\n");
	fp = fopen(NMP_CL_JSON_FILE, "r");
	if(fp == NULL)
		return 1;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	ret = size < NCL_LIMIT;
	if(size == 0)
		ret = 0;
	fclose(fp);
	return ret;
}

/******** Build ARP Socket Function *********/
struct sockaddr_ll src_sockll, dst_sockll;

static int iface_get_id(int fd, const char *device)
{
	struct ifreq    ifr;
	memset(&ifr, 0, sizeof(ifr));
	if(device == NULL){
		perror("iface_get_id ERR:\n");
		NMP_DEBUG("iface_get_id REEOR");
		return -1;
	}
	NMP_DEBUG("interface %s\n", device);
	strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(fd, SIOCGIFINDEX, &ifr) == -1) {
		perror("iface_get_id ERR:\n");
		NMP_DEBUG("iface_get_id REEOR");
		return -1;
	}

	return ifr.ifr_ifindex;
}
/*
 *  Bind the socket associated with FD to the given device.
 */
static int
iface_bind(int fd, int ifindex)
{
	int err;
	socklen_t errlen = sizeof(err);

	memset(&src_sockll, 0, sizeof(src_sockll));
	src_sockll.sll_family          = AF_PACKET;
	src_sockll.sll_ifindex         = ifindex;
	src_sockll.sll_protocol        = htons(ETH_P_ARP);

	if (bind(fd, (struct sockaddr *) &src_sockll, sizeof(src_sockll)) == -1) {
		perror("bind device ERR:\n");
		NMP_DEBUG("iface_bind ERROR");
		return -1;
	}
	/* Any pending errors, e.g., network is down? */
	if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1 || err > 0) {
		NMP_DEBUG("iface_bind ERROR");
		return -2;
	}
	int alen = sizeof(src_sockll);
	if (getsockname(fd, (struct sockaddr*)&src_sockll, (socklen_t *)&alen) == -1) {
		perror("getsockname");
		exit(2);
	}
	if (src_sockll.sll_halen == 0) {
		printf("Interface is not ARPable (no ll address)\n");
		exit(2);
	}
	dst_sockll = src_sockll;

	return 0;
}

int create_socket(char *device)
{
	/* create UDP socket */
	int sock_fd, device_id;
	sock_fd = socket(PF_PACKET, SOCK_DGRAM, 0);

	if(sock_fd < 0)
	perror("create socket ERR:");

	device_id = iface_get_id(sock_fd, device);

	if (device_id == -1){
		NMP_DEBUG("iface_get_id REEOR");
		return -1;
	}

	iface_bind(sock_fd, device_id);
	return sock_fd;
}

int sent_arppacket(int raw_sockfd, unsigned char *dst_ipaddr)
{
	ARP_HEADER * arp;
	char raw_buffer[46];

	memset(dst_sockll.sll_addr, -1, sizeof(dst_sockll.sll_addr));  // set dmac addr FF:FF:FF:FF:FF:FF
	if (raw_buffer == NULL)
	{
		perror("ARP: Oops, out of memory\r");
		return 1;
	 }                                                                                                                          
	bzero(raw_buffer, 46);

	// Allow 14 bytes for the ethernet header
	arp = (ARP_HEADER *)(raw_buffer);// + 14);
	arp->hardware_type =htons(DIX_ETHERNET);
	arp->protocol_type = htons(IP_PACKET);
	arp->hwaddr_len = 6;
	arp->ipaddr_len = 4;
	arp->message_type = htons(ARP_REQUEST);
	// My hardware address and IP addresses
	memcpy(arp->source_hwaddr, my_hwaddr, 6);
	memcpy(arp->source_ipaddr, my_ipaddr, 4);
	// Destination hwaddr and dest IP addr
	memcpy(arp->dest_hwaddr, broadcast_hwaddr, 6);
	memcpy(arp->dest_ipaddr, dst_ipaddr, 4);

	if( (sendto(raw_sockfd, raw_buffer, 46, 0, (struct sockaddr *)&dst_sockll, sizeof(dst_sockll))) < 0 )
	{
		perror("sendto");
		return 1;
	}
//	NMP_DEBUG_M("%s:my_ipaddr: %d.%d.%d.%d\n", __func__, my_ipaddr[0],my_ipaddr[1], my_ipaddr[2], my_ipaddr[3]);
//	NMP_DEBUG_M("%s:broadcast_hwaddr: %x.%x.%x.%x\n", __func__, broadcast_hwaddr[0],broadcast_hwaddr[1], broadcast_hwaddr[2], broadcast_hwaddr[3]);
	NMP_DEBUG_M("%s:Send ARP Request success to: %d.%d.%d.%d\n", __func__, dst_ipaddr[0], dst_ipaddr[1], dst_ipaddr[2], dst_ipaddr[3]);
	return 0;
}
/******* End of Build ARP Socket Function ********/

/*********** Signal function **************/
static void refresh_sig(int signo)
{
	if(networkmap_fullscan == 1){
		NMP_DEBUG("Fullscan is already in process!\n");
	}else{
		NMP_DEBUG("Refresh network map!\n");
		networkmap_fullscan = 1;
		nvram_set("networkmap_status", "1");
		nvram_set("networkmap_fullscan", "1");
	}
}

static void safe_leave(int signo)
{
	daemon_exit = 1;
	NMP_DEBUG("SIGNAL terminal\n");
}

#if defined(RTCONFIG_QCA) && defined(RTCONFIG_WIRELESSREPEATER)
char *getStaMAC()
{
	char buf[512];
	FILE *fp;
	int len,unit;
	char *pt1,*pt2;
	unit=nvram_get_int("wlc_band");

	sprintf(buf, "ifconfig sta%d", unit);

	fp = popen(buf, "r");
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
				chomp(pt2);
				return pt2;
			}
		}
	}
	return NULL;
}
#endif

#ifdef NMP_DB
int commit_no = 0;
int client_updated = 0;

void convert_mac_to_string(unsigned char *mac, char *mac_str)
{
	sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X",
		*mac,*(mac+1),*(mac+2),*(mac+3),*(mac+4),*(mac+5));
}

static void vendorClassCheck(CLIENT_DETAIL_INFO_TABLE *p_client_tab)
{
	struct vc_list_s *tmp = NULL;
	if(p_client_tab->vendorClass[p_client_tab->detail_info_num][0]){
		if(p_client_tab->os_type[p_client_tab->detail_info_num] == 0 || p_client_tab->type[p_client_tab->detail_info_num] == 0){
			NMP_DEBUG_VC("%s()\n", __func__);
			if(p_client_tab->os_type[p_client_tab->detail_info_num] == 0){
				for(tmp = &vendorclasslist[0]; tmp->device; tmp++){
					if(strstr(p_client_tab->vendorClass[p_client_tab->detail_info_num], tmp->device))
						NMP_DEBUG_VC("vendor class Check name=%s, find type=%d\n", p_client_tab->vendorClass[p_client_tab->detail_info_num], p_client_tab->os_type[p_client_tab->detail_info_num]);
				}
				if(strstr(p_client_tab->vendorClass[p_client_tab->detail_info_num], "android"))
					p_client_tab->os_type[p_client_tab->detail_info_num] = BASE_TYPE_ANDROID;
				else if(strstr(p_client_tab->vendorClass[p_client_tab->detail_info_num], "MSFT"))
					p_client_tab->os_type[p_client_tab->detail_info_num] = BASE_TYPE_WINDOW;
				else if(strstr(p_client_tab->vendorClass[p_client_tab->detail_info_num], "dhcpcd"))
					p_client_tab->os_type[p_client_tab->detail_info_num] = BASE_TYPE_LINUX;
				NMP_DEBUG_VC("find os type=%d\n", p_client_tab->os_type[p_client_tab->detail_info_num]);
			}
			if(p_client_tab->type[p_client_tab->detail_info_num] == 0){
				switch(p_client_tab->os_type[p_client_tab->detail_info_num]){
					case BASE_TYPE_ANDROID:
						p_client_tab->type[p_client_tab->detail_info_num] = TYPE_ANDROID;
						break;
					case BASE_TYPE_WINDOW:
						p_client_tab->type[p_client_tab->detail_info_num] = TYPE_WINDOWS;
						break;
					case BASE_TYPE_LINUX:
						p_client_tab->type[p_client_tab->detail_info_num] = TYPE_LINUX_DEVICE;
						break;
				}
				NMP_DEBUG_VC("find device type=%d\n", p_client_tab->type[p_client_tab->detail_info_num]);
			}
			NMP_DEBUG_VC("vendor class Check name=%s, os_type=%d device_type=%d\n",p_client_tab->vendorClass[p_client_tab->detail_info_num] , p_client_tab->os_type[p_client_tab->detail_info_num], p_client_tab->type[p_client_tab->detail_info_num]);
		}
	}
}

void write_vendorclass(const char *mac, CLIENT_DETAIL_INFO_TABLE *p_client_tab, struct json_object *client)
{
	struct json_object *vc_json = NULL;
	struct json_object *macEntryObj = NULL;
	struct json_object *vendorclass = NULL;

	NMP_DEBUG("write_vendorclass: %s\n", mac);
	vc_json = json_object_from_file(NMP_VC_JSON_FILE);
	if(vc_json){
		json_object_object_get_ex(vc_json, mac, &macEntryObj);
		if(macEntryObj){
			json_object_object_get_ex(macEntryObj, "vendorclass", &vendorclass);
			if(vendorclass){
				strlcpy(p_client_tab->vendorClass[p_client_tab->detail_info_num], json_object_get_string(vendorclass), sizeof(p_client_tab->vendorClass[0]));
				NMP_DEBUG("%s(): %s json:%s\n", __func__, p_client_tab->vendorClass[p_client_tab->detail_info_num], json_object_get_string(vendorclass));
				vendorClassCheck(p_client_tab);
				json_object_object_add(client, "vendorclass", json_object_new_string(p_client_tab->vendorClass[p_client_tab->detail_info_num]));
				json_object_object_add(client, "os_type", json_object_new_int(p_client_tab->os_type[p_client_tab->detail_info_num]));
				client_updated = 1;
			}
		}
		json_object_put(vc_json);
	}
}

void write_to_DB(CLIENT_DETAIL_INFO_TABLE *p_client_tab, struct json_object *ncl_json)
{
	char new_mac[32] = {0};
	struct json_object *client = NULL, *macaddr = NULL, *name = NULL, *vendor = NULL, *type = NULL;
	convert_mac_to_string(p_client_tab->mac_addr[p_client_tab->detail_info_num], new_mac);
	NMP_DEBUG("*** write_to_memory: %s ***\n", new_mac);
	json_object_object_get_ex(ncl_json, new_mac, &client);
	if(client == NULL){
		client = json_object_new_object();
		json_object_object_add(client, "mac", json_object_new_string(new_mac));
		json_object_object_add(client, "name", json_object_new_string(p_client_tab->device_name[p_client_tab->detail_info_num]));
		json_object_object_add(client, "vendor", json_object_new_string(p_client_tab->vendor_name[p_client_tab->detail_info_num]));
		write_vendorclass((const char *)new_mac, p_client_tab, client);
		json_object_object_add(client, "type", json_object_new_int(p_client_tab->type[p_client_tab->detail_info_num]));
		json_object_object_add(ncl_json, new_mac, client);
		return;
	}else{
		json_object_object_get_ex(client, "mac", &macaddr);
		if(strncmp(new_mac, json_object_get_string(macaddr), sizeof(new_mac))){
			json_object_object_add(client, "mac", json_object_new_string(new_mac));
			client_updated = 1;
		}
		json_object_object_get_ex(client, "name", &name);
		if(strncmp(p_client_tab->device_name[p_client_tab->detail_info_num], json_object_get_string(name), sizeof(p_client_tab->device_name[0]))){
			json_object_object_add(client, "name", json_object_new_string(json_object_get_string(name)));
			client_updated = 1;
		}
		json_object_object_get_ex(client, "vendor", &vendor);
		if(strncmp(p_client_tab->vendor_name[p_client_tab->detail_info_num], json_object_get_string(vendor), sizeof(p_client_tab->vendor_name[0]))){
			json_object_object_add(client, "vendor", json_object_new_string(json_object_get_string(vendor)));
			client_updated = 1;
		}
		write_vendorclass((const char *)new_mac, p_client_tab, client);
		json_object_object_get_ex(client, "type", &type);
		if(p_client_tab->type[p_client_tab->detail_info_num] != atoi(json_object_get_string(type))){
			json_object_object_add(client, "type", json_object_new_string(json_object_get_string(type)));
			client_updated = 1;
		}
	}
}

int DeletefromDB(CLIENT_DETAIL_INFO_TABLE *p_client_tab, struct json_object * nmp_cl_json){
	char *mac_str;
	char macaddr[18];
	struct json_object *client = NULL;

	mac_str = p_client_tab->delete_mac;
	NMP_DEBUG("delete_from_database: %s\n", mac_str);
	toUpperCase(mac_str);
	snprintf(macaddr, sizeof(macaddr), "%C%C:%C%C:%C%C:%C%C:%C%C:%C%C", *mac_str, *(mac_str+1), *(mac_str+2), *(mac_str+3), *(mac_str+4), *(mac_str+5),
		 *(mac_str+6), *(mac_str+7), *(mac_str+8), *(mac_str+9), *(mac_str+10), *(mac_str+11));
	json_object_object_get_ex(nmp_cl_json, macaddr, &client);
	if(client){
		NMP_DEBUG("Delete json database\n");
		json_object_object_del(nmp_cl_json, macaddr);
		return 1;
	}
	return 0;
}

void swap_string(void *buf1, void *buf2, size_t len)
{
	char buf[512];

	memset(buf, 0, sizeof(buf));
	memcpy(buf, buf1, len);
	memcpy(buf1, buf2, len);
	memcpy(buf2, buf, len);
}

int swap_client_tab(CLIENT_DETAIL_INFO_TABLE *p_client_tab, int client1, int client2)
{
	int tmp;
	swap_string(p_client_tab->ip_addr[client2], p_client_tab->ip_addr[client1], 4);
	swap_string(p_client_tab->mac_addr[client2], p_client_tab->mac_addr[client1], 6);
	swap_string(p_client_tab->user_define[client2], p_client_tab->user_define[client1], 16);
	swap_string(p_client_tab->vendor_name[client2], p_client_tab->vendor_name[client1], 128);
	swap_string(p_client_tab->device_name[client2], p_client_tab->device_name[client1], 32);
	swap_string(p_client_tab->apple_model[client2], p_client_tab->apple_model[client1], 16);
	swap_string(p_client_tab->ipMethod[client2], p_client_tab->ipMethod[client1], 7);
	tmp = p_client_tab->os_type[client2];
	p_client_tab->os_type[client2] = p_client_tab->os_type[client1];
	p_client_tab->os_type[client1] = tmp;
	tmp = p_client_tab->type[client2];
	p_client_tab->type[client2] = p_client_tab->type[client1];
	p_client_tab->type[client1] = tmp;
	tmp = p_client_tab->opMode[client2];
	p_client_tab->opMode[client2] = p_client_tab->opMode[client1];
	p_client_tab->opMode[client1] = tmp;
	tmp = p_client_tab->device_flag[client2];
	p_client_tab->device_flag[client2] = p_client_tab->device_flag[client1];
	p_client_tab->device_flag[client1] = tmp;
 	tmp = p_client_tab->wireless[client2];
	p_client_tab->wireless[client2] = p_client_tab->wireless[client1];
	p_client_tab->wireless[client1] = tmp;
 	tmp = p_client_tab->rssi[client2];
	p_client_tab->rssi[client2] = p_client_tab->rssi[client1];
	p_client_tab->rssi[client1] = tmp;
#ifdef RTCONFIG_LANTIQ
 	tmp = p_client_tab->tstamp[client2];
	p_client_tab->tstamp[client2] = p_client_tab->tstamp[client1];
	p_client_tab->tstamp[client1] = tmp;
#endif
	swap_string(p_client_tab->ssid[client2], p_client_tab->ssid[client1], 32);
	swap_string(p_client_tab->txrate[client2], p_client_tab->txrate[client1], 7);
	swap_string(p_client_tab->rxrate[client2], p_client_tab->rxrate[client1], 10);
	swap_string(p_client_tab->conn_time[client2], p_client_tab->conn_time[client1], 12);
#if defined(RTCONFIG_BWDPI)
	swap_string(p_client_tab->bwdpi_host[client2], p_client_tab->bwdpi_host[client1], 32);
	swap_string(p_client_tab->bwdpi_vendor + v26, p_client_tab->bwdpi_vendor[client1], 100);
	swap_string(p_client_tab->bwdpi_type + v26, p_client_tab->bwdpi_type[client1], 100);
	swap_string(p_client_tab->bwdpi_device + v26, p_client_tab->bwdpi_device[client1], 100);
#endif
	NMP_DEBUG("**** swap client %d and %d\n", client2, client1);
	return 0;
}

void remove_client(int x)
{
	char ipaddr[18];
	snprintf(ipaddr, sizeof(ipaddr), "%d.%d.%d.%d", G_CLIENT_TAB->ip_addr[x][0],G_CLIENT_TAB->ip_addr[x][1], G_CLIENT_TAB->ip_addr[x][2],G_CLIENT_TAB->ip_addr[x][3]);
	memset(G_CLIENT_TAB->ip_addr[x], 0, sizeof(G_CLIENT_TAB->ip_addr[x]));
	memset(G_CLIENT_TAB->mac_addr[x], 0, sizeof(G_CLIENT_TAB->mac_addr[x]));
	memset(G_CLIENT_TAB->user_define[x], 0, sizeof(G_CLIENT_TAB->user_define[x]));
	memset(G_CLIENT_TAB->vendor_name[x], 0, sizeof(G_CLIENT_TAB->vendor_name[x]));
	memset(G_CLIENT_TAB->device_name[x], 0, sizeof(G_CLIENT_TAB->device_name[x]));
	memset(G_CLIENT_TAB->apple_model[x], 0, sizeof(G_CLIENT_TAB->apple_model[x]));
	memset(G_CLIENT_TAB->device_type[x], 0, sizeof(G_CLIENT_TAB->device_type[x]));
	memset(G_CLIENT_TAB->vendorClass[x], 0, sizeof(G_CLIENT_TAB->vendorClass[x]));
	memset(G_CLIENT_TAB->ipMethod[x], 0, sizeof(G_CLIENT_TAB->ipMethod[x]));
	G_CLIENT_TAB->os_type[x] = 0;
	G_CLIENT_TAB->type[x] = 0;
	G_CLIENT_TAB->opMode[x] = 0;
	G_CLIENT_TAB->device_flag[x] = 0;
	G_CLIENT_TAB->wireless[x] = 0;
#ifdef RTCONFIG_LANTIQ
	G_CLIENT_TAB->tstamp[x] = 0;
#endif
	memset(G_CLIENT_TAB->ssid[x], 0, sizeof(G_CLIENT_TAB->ssid[x]));
	memset(G_CLIENT_TAB->txrate[x], 0, sizeof(G_CLIENT_TAB->txrate[x]));
	memset(G_CLIENT_TAB->rxrate[x], 0, sizeof(G_CLIENT_TAB->rxrate[x]));
	memset(G_CLIENT_TAB->conn_time[x], 0, sizeof(G_CLIENT_TAB->conn_time[x]));
	G_CLIENT_TAB->rssi[x] = 0;
#if defined(RTCONFIG_BWDPI)
	memset(G_CLIENT_TAB->bwdpi_host[x], 0, sizeof(G_CLIENT_TAB->bwdpi_host[x]));
	memset(G_CLIENT_TAB->bwdpi_vendor[x], 0, sizeof(G_CLIENT_TAB->bwdpi_vendor[x]));
	memset(G_CLIENT_TAB->bwdpi_type[x], 0, sizeof(G_CLIENT_TAB->bwdpi_type[x]));
	memset(G_CLIENT_TAB->bwdpi_device[x], 0, sizeof(G_CLIENT_TAB->bwdpi_device[x]));
#endif
	eval("arp", "-d", ipaddr);
	NMP_DEBUG("**** remove client %d IP:%s\n", x, ipaddr);
}

int delete_from_share_memory(CLIENT_DETAIL_INFO_TABLE *p_client_tab)
{
	char *mac_str;
	int i = 0, ip_mac_num, lock, n = 0;
	unsigned int mac_binary[6];
	char macaddr[18] = {0};
	char *p = macaddr;
	mac_str = p_client_tab->delete_mac;
	NMP_DEBUG("delete_from_share_memory: %s\n", mac_str);
	ip_mac_num = p_client_tab->ip_mac_num;
	while(mac_str != &p_client_tab->delete_mac[12])
	{
		strncpy(macaddr, mac_str, 2);
		mac_binary[i] = strtol(macaddr, &p, 16);
		mac_str += 2;
		i++;
	}
	NMP_DEBUG("%02x%02x%02x%02x%02x%02x\n", mac_binary[0], mac_binary[1], mac_binary[2], mac_binary[3], mac_binary[4], mac_binary[5]);
	if(ip_mac_num){
		while(1){
			if(!memcmp(mac_binary, p_client_tab->mac_addr[i], 6))
				break;
			i++;
			if(ip_mac_num == i){
				n = ip_mac_num;
				break;
			}
		}
		if(ip_mac_num != i){
			n = i - 1;
			lock = file_lock("networkmap");
			NMP_DEBUG("remove %d\n", n);
			remove_client(n);
			file_unlock(lock);
		}
	}
	NMP_DEBUG("SWAP client list\n");
	if(n >= ip_mac_num)
		return 0;
	else{
		lock = file_lock("networkmap");
		G_CLIENT_TAB->detail_info_num -= 1;
		G_CLIENT_TAB->commit_no -= 1;
		if((G_CLIENT_TAB->device_flag[n] & (1<<FLAG_ASUS)) != 0)
			G_CLIENT_TAB->asus_device_num -= 1;
		if(n < G_CLIENT_TAB->ip_mac_num){
			while(1){
				swap_client_tab(G_CLIENT_TAB, n, n + 1);
				if((n + 1) == G_CLIENT_TAB->ip_mac_num)
					break;
				n++;
			}
		}
		file_unlock(lock);
	}
	return 0;
}

void reset_client_list(CLIENT_DETAIL_INFO_TABLE *p_client_tab)
{
	int i;
	networkmap_lock = file_lock("networkmap");
	for(i = 0; i < MAX_NR_CLIENT_LIST; i++)
		p_client_tab->device_flag[i] &= 0xf7;
	file_unlock(networkmap_lock);
}

void reset_networkmap(CLIENT_DETAIL_INFO_TABLE *p_client_detail_info_tab)
{
	networkmap_lock = file_lock("networkmap");
	p_client_detail_info_tab->detail_info_num = 0;
	file_unlock(networkmap_lock);
}

void delete_sig_on(int signo)
{
	NMP_DEBUG("DELETE OFFLINE CLIENT\n");
	delete_sig = 1;
}

void clean_client_list(int signo)
{
	networkmap_fullscan = 3;
	NMP_DEBUG("SIGNAL clean share memory client list\n");
}
#endif


void StringChk(char *chk_string)
{
        char *ptr = chk_string;
        while(*ptr != 0) {
                if(*ptr < 0x20 || *ptr > 0x7E)
                        *ptr = ' ';
                ptr++;
        }
}

#ifdef RTCONFIG_BWDPI
static int QueryBwdpiInfo(P_CLIENT_DETAIL_INFO_TABLE p_client_detail_info_tab, int x)
{
	bwdpi_device bwdpi_dev_info;
	char macaddr[18], ipaddr[18];
	char *bwdpi_host, *bwdpi_vendor, *bwdpi_type, *bwdpi_device;
	char *nv, *nvp;
	snprintf(ipaddr, sizeof(ipaddr), "%d.%d.%d.%d", tmp->ip_addr[x][0],tmp->ip_addr[x][1], tmp->ip_addr[x][2],tmp->ip_addr[x][3]);
	sprintf(macaddr,"%02X:%02X:%02X:%02X:%02X:%02X", p_client_detail_info_tab->mac_addr[x][0], p_client_detail_info_tab->mac_addr[x][1],
		p_client_detail_info_tab->mac_addr[x][2], p_client_detail_info_tab->mac_addr[x][3], p_client_detail_info_tab->mac_addr[x][4],
                p_client_detail_info_tab->mac_addr[x][5]);
	NMP_DEBUG("Bwdpi Query: %s, %s\n", macaddr, ipaddr);
	if(bwdpi_client_info(&macaddr, &ipaddr, &bwdpi_dev_info)) {
		NMP_DEBUG("  Get: %s/%s/%s/%s\n", bwdpi_dev_info.hostname,bwdpi_dev_info.vendor_name, bwdpi_dev_info.type_name,bwdpi_dev_info.device_name);
		strlcpy(p_client_detail_info_tab->bwdpi_host[x], bwdpi_dev_info.hostname, sizeof(p_client_detail_info_tab->bwdpi_host[x]));
		strlcpy(p_client_detail_info_tab->bwdpi_vendor[x], bwdpi_dev_info.vendor_name, sizeof(p_client_detail_info_tab->bwdpi_vendor[x]));
		strlcpy(p_client_detail_info_tab->bwdpi_type[x], bwdpi_dev_info.type_name, sizeof(p_client_detail_info_tab->bwdpi_type[x]));
		strlcpy(p_client_detail_info_tab->bwdpi_device[x], bwdpi_dev_info.device_name, sizeof(p_client_detail_info_tab->bwdpi_device[x]));
		StringChk(p_client_detail_info_tab->bwdpi_host[x]);
		StringChk(p_client_detail_info_tab->bwdpi_vendor[x]);
		StringChk(p_client_detail_info_tab->bwdpi_type[x]);
		StringChk(p_client_detail_info_tab->bwdpi_device[x]);
	}
	bwdpi_host = p_client_detail_info_tab->bwdpi_host[x];
	bwdpi_vendor = p_client_detail_info_tab->bwdpi_vendor[x];
	bwdpi_type = p_client_detail_info_tab->bwdpi_type[x];
	bwdpi_device = p_client_detail_info_tab->bwdpi_device[x];
	if(*bwdpi_device != 0){
		id = prefix_search(g_bwdpi_state, bwdpi_device, baseID);
		if(id != 0){
			type_filter(p_client_detail_info_tab, x, id, baseID[0], 1);
			NMP_DEBUG("*** BWDPI_DEVICE Find device type %d\n", id);
		}
	}
	if(p_client_detail_info_tab->vendor_name[x][0] == 0 && *bwdpi_vendor){
		strlcpy(tmp->vendor_name[x], device.vendor_name, 128);
		NMP_DEBUG("*** Add BWDPI vendor %s\n", p_client_detail_info_tab->vendor_name[x]);
	}
	type = p_client_detail_info_tab->type[x];
	if((type == 0 || (((type - 9) >= 26 || ((0x2004801 >> (type - 9)) & 1) == 0) && isBaseType(type))) && *bwdpi_device){
		id = full_search(g_bwdpi_state, bwdpi_device, baseID);
		if(id){
			type_filter(p_client_detail_info_tab, x, id, baseID[0], 1);
			NMP_DEBUG("*** BWDPI_DEVICE Find device type %d\n", id);
		}
	}
	if(type && (isBaseType(type) || *bwdpi_type)){
		id = prefix_search(g_bwdpi_state, bwdpi_type, baseID);
		if(id){
			type_filter(p_client_detail_info_tab, x, id, baseID[0], 0);
			NMP_DEBUG("*** BWDPI_TYPE Find device type %d\n", id);
		}
	}
	if((type == 0 || isBaseType(type)) && *bwdpi_host){
		nv = nvp = strdup(bwdpi_host);
		toLowerCase(nv);
		id = full_search(g_bwdpi_state, nv, baseID);
		if(id){
			type_filter(p_client_detail_info_tab, x, id, baseID[0], 0);
			NMP_DEBUG("*** BWDPI_HOST Find device type %d\n", id);
		}
		free(nv);
	}
	
#ifdef RTCONFIG_NOTIFICATION_CENTER
	NMP_DEBUG("bwdpi info: %d, %s, %s, %S\n", type, bwdpi_device, bwdpi_type, bwdpi_host);
	if(type == 7)
		nt_send_trigger_event2(FLAG_XBOX_PS, HINT_XBOX_PS_EVENT);
	else if(type == 27)
		nt_send_trigger_event2(FLAG_UPNP_RENDERER, HINT_UPNP_RENDERER_EVENT);
	else if(type == 6)
		nt_send_trigger_event2(FLAG_OSX_INLAN, HINT_OSX_INLAN_EVENT);
#endif
		
//NMP_DEBUG("Result: %s, %s\n\n", p_client_detail_info_tab->bwdpi_hostname[x],
//p_client_detail_info_tab->bwdpi_devicetype[x]);

	return 0;
}
#endif

int free_ac_state(ac_state *state)
{
	ac_state *state_tmp;
	ac_trans *trans_tmp;
	match_rule *rule_tmp;

	if(state){
		while(state){
			state_tmp = state;
			state = state->next;
			trans_tmp = state_tmp->nextTrans;
			if(trans_tmp){
				while(trans_tmp){
					state_tmp->nextTrans = trans_tmp->nextTrans;
					free(trans_tmp);
					trans_tmp = state_tmp->nextTrans;
				}
			}
			rule_tmp = state_tmp->matchRuleList;
			if(rule_tmp){
				while(rule_tmp){
					state_tmp->matchRuleList = rule_tmp->next;
					free(rule_tmp);
					rule_tmp = state_tmp->matchRuleList;
				}
			}
			free(state_tmp);
		}
	}
	SM_DEBUG("free state machine success!\n");
	return 0;
}

CLIENT_DETAIL_INFO_TABLE *init_shared_memory(CLIENT_DETAIL_INFO_TABLE *p_client_detail_info_tab, int key)
{
	int lock, shm_client_detail_info_id;
	lock = file_lock("networkmap");
	shm_client_detail_info_id = shmget((key_t)key, sizeof(CLIENT_DETAIL_INFO_TABLE), 0666|IPC_CREAT);
	if (shm_client_detail_info_id == -1){
		fprintf(stderr,"client info shmget failed\n");
	    file_unlock(lock);
		exit(1);
	}

	p_client_detail_info_tab = (P_CLIENT_DETAIL_INFO_TABLE)shmat(shm_client_detail_info_id,(void *) 0,0);
	memset(p_client_detail_info_tab, 0x0, sizeof(CLIENT_DETAIL_INFO_TABLE));
	p_client_detail_info_tab->ip_mac_num = 0;
	p_client_detail_info_tab->detail_info_num = 0;
	file_unlock(lock);
	return p_client_detail_info_tab;
}

void type_filter(P_CLIENT_DETAIL_INFO_TABLE p_client_detail_info_tab, int x, unsigned char type, unsigned char base, int isDev)
{
	int n;
	char *nv, *nvp;
	P_CLIENT_DETAIL_INFO_TABLE tmp;
	tmp = p_client_detail_info_tab;
	if(type == 34){
		if(base == 2 || base == 4)
			tmp->type[x] = 34;
		else if(base == 0 || tmp->os_type[x] == base)
			tmp->type[x] = type;
    }else if(type > 29){
		if(base == 0 || tmp->os_type[x] == base)
			tmp->type[x] = type;
	}else{
		if(base == 1 || base == 4 || base == 0 || tmp->os_type[x] == base)
			tmp->type[x] = type;
		else{
			NMP_DEBUG("%s: base type not match!\n found type:%d base:%d os type:%d\n", __func__, type, base, tmp->os_type[x]);
			NMP_DEBUG("%s: define type %d\n", __func__, tmp->type[x]);
			return;
		}
	}
	NMP_DEBUG("%s: define type %d\n", __func__, tmp->type[x]);
	if(isDev == 0)
		return;
#if defined(RTCONFIG_BWDPI)
	if(tmp->device_name[x][0]){
		nv = nvp = strdup(tmp->device_name[x]);
		if(*nv){
			n = *nv;
			while(*nv){
				if((n - 65 ) < 26)
					*nv = n + 32;
				nv++;
				n = *nv;
			}
		}
		if(strstr(nvp, "android"))
			strlcpy(tmp->device_name[x], tmp->bwdpi_device[x], 32);
			free(nvp);
			if(tmp->bwdpi_device[x][0] == 0)
				return;
		}else{
			strlcpy(tmp->device_name[x], tmp->bwdpi_device[x], 32);
			if(tmp->bwdpi_device[x][0] == 0)
				return;
		}
		strlcpy(tmp->vendor_name[x], tmp->bwdpi_device[x], 16);
		NMP_DEBUG("*** Add BWDPI device model %s\n", tmp->vendor_name[x]);
#endif
}


void check_asus_discovery(CLIENT_DETAIL_INFO_TABLE *p_client_tab, int x)
{
	int n;
	char *nv, *nvp, *p;
	char *type, *name, *ip, *mac, *netmask, *opmode, *ssid, *submask;
	char *name_str;
	char macaddr[18];
	if((p_client_tab->device_flag[x] & (1<<FLAG_ASUS)) == 0){
		NMP_DEBUG("check_asus_discovery:\n");
		nv = nvp = strdup(nvram_safe_get("asus_device_list"));
		if(nv){
			sprintf(macaddr,"%02X:%02X:%02X:%02X:%02X:%02X", p_client_tab->mac_addr[x][0], p_client_tab->mac_addr[x][1],
				p_client_tab->mac_addr[x][2], p_client_tab->mac_addr[x][3], p_client_tab->mac_addr[x][4], p_client_tab->mac_addr[x][5]);
			NMP_DEBUG("search MAC= %s\n", macaddr);
			if(strstr(nv, macaddr))
			{
				while(nv){
					if ((p = strsep(&nv, "<")) == NULL) break;
					if((vstrsep(p, ">",&type ,&name, &ip, &mac, &netmask, &ssid, &submask, &opmode)) != 8) continue;
					if(strcmp(mac, macaddr)) continue;
					name_str = strdup(name);
					toLowerCase(name_str);
					if(strstr(name_str, "cam")){
						p_client_tab->type[x] = 5;
						NMP_DEBUG("***** Find AiCam****\n");
					}else if(opmode != ""){
						if(atoi(opmode) != 2)
							p_client_tab->type[x] = 2;
						else
							p_client_tab->type[x] = 24;
					}
					strlcpy(p_client_tab->device_name[x], name, sizeof(p_client_tab->device_name[x]));
					strlcpy(p_client_tab->vendor_name[x], "Asus", sizeof(p_client_tab->vendor_name[x]));
					strlcpy(p_client_tab->ssid[x], ssid, sizeof(p_client_tab->ssid[x]));
					p_client_tab->opMode[x] = atoi(opmode);
					p_client_tab->device_flag[x] = p_client_tab->device_flag[x] | (1<<FLAG_ASUS | 1<<FLAG_HTTP);
					free(name_str);
					NMP_DEBUG("asus device: %d, %s, opMode:%d\n", p_client_tab->type[x], p_client_tab->device_name[x], p_client_tab->opMode[x]);
					n = p_client_tab->asus_device_num;
					if(n != x){
						swap_client_tab(p_client_tab, x, n);
						n = p_client_tab->asus_device_num;
					}
					p_client_tab->asus_device_num = n + 1;
					NMP_DEBUG("**** asus device num %d\n", p_client_tab->asus_device_num);
					goto SKIP_OUI;
				}
			}
			if(update_oui){
				char *vendor_name;
				unsigned char base = 0;
				json_object *vendor_nameobj= NULL;
				NMP_DEBUG("check_oui_json_db:\n");
				if(p_client_tab->mac_addr[x][0] || p_client_tab->mac_addr[x][1] || p_client_tab->mac_addr[x][2]){
					sprintf(macaddr, "%02X%02X%02X", p_client_tab->mac_addr[x][0], p_client_tab->mac_addr[x][1], p_client_tab->mac_addr[x][2]);
					json_object_object_get_ex(networkmap_oui_json, macaddr, &vendor_nameobj);
					vendor_name = (char *)json_object_get_string(vendor_nameobj);
					if(vendor_name){
						n = prefix_search_index(g_vendor_state, vendor_name, &base);
						if(n){
							strlcpy(p_client_tab->vendor_name[x], vendor_name, n);
							strlcpy(p_client_tab->device_name[x], vendor_name, n);
							p_client_tab->os_type[x] = base;
							NMP_DEBUG("*** Find Favous OUI %s base type %d\n", p_client_tab->vendor_name[x], base);
						}else{
							strlcpy(p_client_tab->vendor_name[x], vendor_name, sizeof(p_client_tab->vendor_name[x]));
							strlcpy(p_client_tab->device_name[x], vendor_name, sizeof(p_client_tab->device_name[x]));
						}
					}
				}
			}
SKIP_OUI:
			free(nvp);
		}else{
			printf("%s, strdup failed\n", __func__);
		}
	}
}

#if defined(RTCONFIG_AMAS) || defined(RTCONFIG_CFGSYNC)
size_t update_from_cfgsync(json_object *clientlist, const char *ip_str, char *mac_str)
{
	int i;
	int ret = 0;
	int shm_client_tbl_id;
	P_CM_CLIENT_TABLE p_client_tbl;
	void *shared_client_info=(void *) 0;
	unsigned char mac_buf[6] = {0};
	json_object *brMacObj = NULL;
	json_object *clientObj = NULL;
	json_object *ip = NULL;
	json_object_object_foreach(clientlist, key, val){
		brMacObj = val;
		json_object_object_foreach(brMacObj, key, val){
			clientObj = val;
			json_object_object_foreach(clientObj, key, val){
				if(!is_re_node(key, 0)){
					json_object_object_get_ex(val, "ip", &ip);
					if(ip){
						if(!strcmp(ip_str, json_object_get_string(ip))){
							sprintf(mac_str, "%s", key);
							break;
						}
					}
				}
			}
		}
	}
	if(*mac_str == 0)
		return strlen(mac_str);

	shm_client_tbl_id = shmget((key_t)KEY_SHM_CFG, sizeof(CM_CLIENT_TABLE), 0666|IPC_CREAT);
	if (shm_client_tbl_id == -1){
		fprintf(stderr, "shmget failed\n");
		return 0;
	}

	shared_client_info = shmat(shm_client_tbl_id,(void *) 0,0);
	if (shared_client_info == (void *)-1){
		fprintf(stderr, "shmat failed\n");
		return 0;
	}
	ether_atoe(client_mac, mac_buf);

	p_client_tbl = (P_CM_CLIENT_TABLE)shared_client_info;
	count = cm_tab->count;
	if(count < 2){
		shmdt(cm_tab);
		return 0;
	}
	for(i = 1; i < p_client_tbl->count; i++) {
		if(memcmp(p_client_tbl->sta2g[i], mac_buf, sizeof(mac_buf)) == 0 || memcmp(p_client_tbl->sta5g[i], mac_buf, sizeof(mac_buf)) == 0){
			sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", p_client_tbl->realMacAddr[i][0], p_client_tbl->realMacAddr[i][1], 
				p_client_tbl->realMacAddr[i][2], p_client_tbl->realMacAddr[i][3], p_client_tbl->realMacAddr[i][4], p_client_tbl->realMacAddr[i][5]);
			break;
		}
	}
	shmdt(shared_client_info);
	return strlen(mac_str);
}

int is_re_node(char *client_mac, int check_path)
{
	int i;
	int ret = 0;
	int shm_client_tbl_id;
	P_CM_CLIENT_TABLE p_client_tbl;
	void *shared_client_info=(void *) 0;
	unsigned char mac_buf[6] = {0};

	shm_client_tbl_id = shmget((key_t)KEY_SHM_CFG, sizeof(CM_CLIENT_TABLE), 0666|IPC_CREAT);
	if (shm_client_tbl_id == -1){
		fprintf(stderr, "shmget failed\n");
		return 0;
	}

	shared_client_info = shmat(shm_client_tbl_id,(void *) 0,0);
	if (shared_client_info == (void *)-1){
		fprintf(stderr, "shmat failed\n");
		return 0;
	}

	ether_atoe(client_mac, mac_buf);

	p_client_tbl = (P_CM_CLIENT_TABLE)shared_client_info;
	for(i = 1; i < p_client_tbl->count; i++) {
		if ((memcmp(p_client_tbl->sta2g[i], mac_buf, sizeof(mac_buf)) == 0 ||
			memcmp(p_client_tbl->sta5g[i], mac_buf, sizeof(mac_buf)) == 0) &&
			((check_path && p_client_tbl->activePath[i] == 1) || check_path == 0))
		{
			ret = 1;
			break;
		}
	}

	shmdt(shared_client_info);

	return ret;
}
#endif

void arp_parser_scan(CLIENT_DETAIL_INFO_TABLE *p_client_tab, const char *br_ifname)
{
	int ip_mac_num, ip_mac_num2, i, len = 0, isnew;
	unsigned char device_flag;
	unsigned char ip_binary[4] = {0};
	unsigned char mac_binary[6] = {0};
	unsigned char *pIP = ip_binary, *pMac = mac_binary;
	FILE *fp;
	char ipaddr[18], macaddr[18], flag[4], dev[8], line[300];
	char *next;
	struct json_object *clientlist = NULL;
	NMP_DEBUG("### %s:Start ARP parser scan\n", __func__);
	if((fp = fopen(ARP_PATH, "r"))){
		while((next = fgets(line, sizeof(line), fp)) != NULL){
			sscanf(next, "%s%*s%s%s%*s%s\n", ipaddr, flag, macaddr, dev);
			isnew = 0;
			NMP_DEBUG("arp check: %s\n", next);
			if(strncmp(dev, br_ifname, 3) || !strncmp(flag, "0x0", 3))
				continue;
			sscanf(ipaddr, "%hhu.%hhu.%hhu.%hhu", &pIP[0], &pIP[1], &pIP[2], &pIP[3]);
			sscanf(macaddr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &pMac[0], &pMac[1], &pMac[2], &pMac[3], &pMac[4], &pMac[5]);
			NMP_DEBUG("RCV IP:%s, MAC:%s\n", ipaddr, macaddr);
			FindDevice(ip_binary, mac_binary, 1);//update ip via mac or update mac via ip.
			ip_mac_num = p_client_tab->ip_mac_num;
			ip_mac_num2 = p_client_tab->ip_mac_num + 1;
			if(ip_mac_num2 < 0)
				ip_mac_num2 = 0;
			isnew = 1;
			for(i = 0; i < ip_mac_num2; i++){//find ip or mac in tables
				if(!memcmp(p_client_tab->ip_addr[i], ip_binary, sizeof(p_client_tab->ip_addr[0]))){// same ip, same/different mac
					isnew = 0;
					if(!memcmp(p_client_tab->mac_addr[i], mac_binary, sizeof(p_client_tab->mac_addr[0]))){
						lock = file_lock("networkmap");
						p_client_tab->device_flag[i] |= (1<<FLAG_EXIST);
						file_unlock(lock);
					}else{
						NMP_DEBUG("IP assigned to another device. Refill client list.");
						NMP_DEBUG("*CMP %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X\n", p_client_tab->ip_addr[i][0], p_client_tab->ip_addr[i][1],
							p_client_tab->ip_addr[i][2], p_client_tab->ip_addr[i][3], p_client_tab->mac_addr[i][0], p_client_tab->mac_addr[i][1],
							p_client_tab->mac_addr[i][2], p_client_tab->mac_addr[i][3], p_client_tab->mac_addr[i][4], p_client_tab->mac_addr[i][5]);
						lock = file_lock("networkmap");
						p_client_tab->mac_addr[i][0] = mac_binary[0];
						p_client_tab->mac_addr[i][1] = mac_binary[1];
						p_client_tab->mac_addr[i][2] = mac_binary[2];
						p_client_tab->mac_addr[i][3] = mac_binary[3];
						p_client_tab->mac_addr[i][4] = mac_binary[4];
						p_client_tab->mac_addr[i][5] = mac_binary[5];
						p_client_tab->device_flag[i] |= (1<<FLAG_EXIST);
						file_unlock(lock);
					}
					break;
				}else if(!memcmp(p_client_tab->mac_addr[i], mac_binary, sizeof(p_client_tab->mac_addr[0]))){//same mac, different ip
					isnew = 0;
					if(!FindDevice(ip_binary, mac_binary, 0)){//it's an error, not same ip.
						if((p_client_tab->device_flag[i] & (1<<FLAG_EXIST)) == 0){
							lock = file_lock("networkmap");
							p_client_tab->ip_addr[i][0] = ip_binary[0];
							p_client_tab->ip_addr[i][1] = ip_binary[1];
							p_client_tab->ip_addr[i][2] = ip_binary[2];
							p_client_tab->ip_addr[i][3] = ip_binary[3];
							p_client_tab->device_flag[i] |= (1<<FLAG_EXIST);
							file_unlock(lock);
						}
					}else{
						NMP_DEBUG("IP changed. Refill client list.");
						NMP_DEBUG("*CMP %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X\n", p_client_tab->ip_addr[i][0], p_client_tab->ip_addr[i][1],
							p_client_tab->ip_addr[i][2], p_client_tab->ip_addr[i][3], p_client_tab->mac_addr[i][0], p_client_tab->mac_addr[i][1],
							p_client_tab->mac_addr[i][2], p_client_tab->mac_addr[i][3], p_client_tab->mac_addr[i][4], p_client_tab->mac_addr[i][5]);
						lock = file_lock("networkmap");
						p_client_tab->ip_addr[i][0] = ip_binary[0];
						p_client_tab->ip_addr[i][1] = ip_binary[1];
						p_client_tab->ip_addr[i][2] = ip_binary[2];
						p_client_tab->ip_addr[i][3] = ip_binary[3];
						p_client_tab->device_flag[i] |= (1<<FLAG_EXIST);
						file_unlock(lock);
					}
					break;
				}
			}
			if(isnew == 1 && !memcmp(ip_binary, my_ipaddr, 3)){//new device, add to table
#if defined(RTCONFIG_NOTIFICATION_CENTER)
				nt_send_event_update();
#endif
				lock = file_lock("networkmap");
				p_client_tab->ip_addr[ip_mac_num][0] = ip_binary[0];
				p_client_tab->ip_addr[ip_mac_num][1] = ip_binary[1];
				p_client_tab->ip_addr[ip_mac_num][2] = ip_binary[2];
				p_client_tab->ip_addr[ip_mac_num][3] = ip_binary[3];
				p_client_tab->mac_addr[ip_mac_num][0] = mac_binary[0];
				p_client_tab->mac_addr[ip_mac_num][1] = mac_binary[1];
				p_client_tab->mac_addr[ip_mac_num][2] = mac_binary[2];
				p_client_tab->mac_addr[ip_mac_num][3] = mac_binary[3];
				p_client_tab->mac_addr[ip_mac_num][4] = mac_binary[4];
				p_client_tab->mac_addr[ip_mac_num][5] = mac_binary[5];
				p_client_tab->device_flag[ip_mac_num] |= (1<<FLAG_EXIST);
				check_asus_discovery(p_client_tab, ip_mac_num);
				if((p_client_tab->device_flag[ip_mac_num] & (1<<FLAG_ASUS)) == 0){
#if defined(RTCONFIG_BWDPI)
					if(sw_mode == 1 && check_bwdpi_nvram_setting()){
						NMP_DEBUG("BWDPI ON!\n");
						QueryBwdpiInfo(p_client_tab, ip_mac_num);
					}
#endif
					FindHostname(p_client_tab);
				}
				if(p_client_tab->device_name[ip_mac_num][0]){
					char *pt = &p_client_tab->device_name[ip_mac_num][0];
					while(*pt){
						if((*pt - ' ') > '_')
							*pt = ' ';
						pt++;
					}
				}
				NMP_DEBUG("Fill: %d-> %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X\n", ip_mac_num2, p_client_tab->ip_addr[ip_mac_num][0], 
				p_client_tab->ip_addr[ip_mac_num][1], p_client_tab->ip_addr[ip_mac_num][2], p_client_tab->ip_addr[ip_mac_num][3], 
					p_client_tab->mac_addr[ip_mac_num][0], p_client_tab->mac_addr[ip_mac_num][1], p_client_tab->mac_addr[ip_mac_num][2], 
					p_client_tab->mac_addr[ip_mac_num][3], p_client_tab->mac_addr[ip_mac_num][4], p_client_tab->mac_addr[ip_mac_num][5]);
				p_client_tab->ip_mac_num++;
				file_unlock(lock);
			}
		}
	}
	fclose(fp);
}

void nmap_receive_arp(CLIENT_DETAIL_INFO_TABLE *p_client_tab, ARP_HEADER *arp_hdr, int x)
{
	int ip_mac_num, ip_mac_num2, i, len = 0, isnew;
	struct in_addr src_addr;
	unsigned short msg_type;
	NMP_DEBUG("### %s:Start ARP parser scan\n", __func__);

	//Check ARP packet if source ip and router ip at the same network
	if(!memcmp(my_ipaddr, arp_hdr->source_ipaddr, 3)){
		msg_type = ntohs(arp_hdr->message_type);
		memcpy(&src_addr, arp_hdr->source_ipaddr, 4);
		if(src_addr.s_addr == INADDR_ANY || src_addr.s_addr == INADDR_NONE || !memcmp(arp_hdr->source_ipaddr, my_ipaddr, 4))
			return;

		if((arp_full_scan && msg_type == ARP_RESPONSE && memcmp(arp_hdr->dest_ipaddr, my_ipaddr, 4) == 0 && memcmp(arp_hdr->dest_hwaddr, my_hwaddr, 6) == 0)
			|| (msg_type == ARP_REQUEST && (memcmp(arp_hdr->dest_ipaddr, my_ipaddr, 4) == 0 || memcmp(arp_hdr->dest_ipaddr, gateway_ipaddr, 4)))){
			NMP_DEBUG("*RCV %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X,%d,%02x,IP:%d\n", arp_hdr->source_ipaddr[0],arp_hdr->source_ipaddr[1],
				arp_hdr->source_ipaddr[2],arp_hdr->source_ipaddr[3], arp_hdr->source_hwaddr[0],arp_hdr->source_hwaddr[1], arp_hdr->source_hwaddr[2],
				arp_hdr->source_hwaddr[3], arp_hdr->source_hwaddr[4],arp_hdr->source_hwaddr[5], p_client_tab->ip_mac_num, msg_type, x);
			NMP_DEBUG("*DEST IP %d.%d.%d.%d-%d,%02x,IP:%d\n", arp_hdr->dest_ipaddr[0],arp_hdr->dest_ipaddr[1],arp_hdr->dest_ipaddr[2],
				arp_hdr->dest_ipaddr[3], p_client_tab->ip_mac_num, msg_type, x);
			FindDevice(arp_hdr->source_ipaddr, arp_hdr->source_hwaddr, 1);//update ip or mac via mac or ip.
			ip_mac_num = p_client_tab->ip_mac_num;
			ip_mac_num2 = p_client_tab->ip_mac_num + 1;
			if(ip_mac_num2 < 0)
				ip_mac_num2 = 0;
			isnew = 1;
			for(i = 0; i < ip_mac_num2; i++){//find ip or mac in tables
				if(!memcmp(p_client_tab->ip_addr[i], arp_hdr->source_ipaddr, sizeof(p_client_tab->ip_addr[0]))){// same ip, same/different mac
					isnew = 0;
					if(!memcmp(p_client_tab->mac_addr[i], arp_hdr->source_hwaddr, sizeof(p_client_tab->mac_addr[0]))){
						lock = file_lock("networkmap");
						p_client_tab->device_flag[i] |= (1<<FLAG_EXIST);
						file_unlock(lock);
					}else{
						NMP_DEBUG("IP assigned to another device. Refill client list.");
						NMP_DEBUG("*CMP %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X\n", p_client_tab->ip_addr[i][0], p_client_tab->ip_addr[i][1],
							p_client_tab->ip_addr[i][2], p_client_tab->ip_addr[i][3], p_client_tab->mac_addr[i][0], p_client_tab->mac_addr[i][1],
							p_client_tab->mac_addr[i][2], p_client_tab->mac_addr[i][3], p_client_tab->mac_addr[i][4], p_client_tab->mac_addr[i][5]);
						lock = file_lock("networkmap");
						p_client_tab->mac_addr[i][0] = arp_hdr->source_hwaddr[0];
						p_client_tab->mac_addr[i][1] = arp_hdr->source_hwaddr[1];
						p_client_tab->mac_addr[i][2] = arp_hdr->source_hwaddr[2];
						p_client_tab->mac_addr[i][3] = arp_hdr->source_hwaddr[3];
						p_client_tab->mac_addr[i][4] = arp_hdr->source_hwaddr[4];
						p_client_tab->mac_addr[i][5] = arp_hdr->source_hwaddr[5];
						p_client_tab->device_flag[i] |= (1<<FLAG_EXIST);
						file_unlock(lock);
					}
					break;
				}else if(!memcmp(p_client_tab->mac_addr[i], arp_hdr->source_hwaddr, sizeof(p_client_tab->mac_addr[0]))){//same mac, different ip
					isnew = 0;
					FindDevice(arp_hdr->source_ipaddr, arp_hdr->source_hwaddr, 0);
					if(!memcmp(p_client_tab->ip_addr[i], arp_hdr->source_ipaddr, sizeof(p_client_tab->ip_addr[0]))){
						lock = file_lock("networkmap");
						p_client_tab->ip_addr[i][0] = arp_hdr->source_ipaddr[0];
						p_client_tab->ip_addr[i][1] = arp_hdr->source_ipaddr[1];
						p_client_tab->ip_addr[i][2] = arp_hdr->source_ipaddr[2];
						p_client_tab->ip_addr[i][3] = arp_hdr->source_ipaddr[3];
						p_client_tab->device_flag[i] |= (1<<FLAG_EXIST);
						file_unlock(lock);
					}
					break;
				}
			}
			if(isnew == 1 && !memcmp(arp_hdr->source_ipaddr, my_ipaddr, 3)){//new device, add to table
#if defined(RTCONFIG_NOTIFICATION_CENTER)
				nt_send_event_update();
#endif
				lock = file_lock("networkmap");
				p_client_tab->ip_addr[ip_mac_num][0] = arp_hdr->source_ipaddr[0];
				p_client_tab->ip_addr[ip_mac_num][1] = arp_hdr->source_ipaddr[1];
				p_client_tab->ip_addr[ip_mac_num][2] = arp_hdr->source_ipaddr[2];
				p_client_tab->ip_addr[ip_mac_num][3] = arp_hdr->source_ipaddr[3];
				p_client_tab->mac_addr[ip_mac_num][0] = arp_hdr->source_hwaddr[0];
				p_client_tab->mac_addr[ip_mac_num][1] = arp_hdr->source_hwaddr[1];
				p_client_tab->mac_addr[ip_mac_num][2] = arp_hdr->source_hwaddr[2];
				p_client_tab->mac_addr[ip_mac_num][3] = arp_hdr->source_hwaddr[3];
				p_client_tab->mac_addr[ip_mac_num][4] = arp_hdr->source_hwaddr[4];
				p_client_tab->mac_addr[ip_mac_num][5] = arp_hdr->source_hwaddr[5];
				p_client_tab->device_flag[ip_mac_num] |= (1<<FLAG_EXIST);
				check_asus_discovery(p_client_tab, ip_mac_num);
				if((p_client_tab->device_flag[ip_mac_num] & (1<<FLAG_ASUS)) == 0){
#if defined(RTCONFIG_BWDPI)
					if(sw_mode == 1 && check_bwdpi_nvram_setting()){
						NMP_DEBUG("BWDPI ON!\n");
						QueryBwdpiInfo(p_client_tab, ip_mac_num);
					}
#endif
					FindHostname(p_client_tab);
				}
				if(p_client_tab->device_name[ip_mac_num][0]){
					char *pt = &p_client_tab->device_name[ip_mac_num][0];
					while(*pt){
						if((*pt - ' ') > '_')
							*pt = ' ';
						pt++;
					}
				}
				NMP_DEBUG("Fill: %d-> %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X\n", ip_mac_num, p_client_tab->ip_addr[ip_mac_num][0], 
				p_client_tab->ip_addr[ip_mac_num][1], p_client_tab->ip_addr[ip_mac_num][2], p_client_tab->ip_addr[ip_mac_num][3], 
					p_client_tab->mac_addr[ip_mac_num][0], p_client_tab->mac_addr[ip_mac_num][1], p_client_tab->mac_addr[ip_mac_num][2], 
					p_client_tab->mac_addr[ip_mac_num][3], p_client_tab->mac_addr[ip_mac_num][4], p_client_tab->mac_addr[ip_mac_num][5]);
				p_client_tab->ip_mac_num++;
				file_unlock(lock);
			}
		}
	}
}

void deep_scan(CLIENT_DETAIL_INFO_TABLE *p_client_tab)
{
	int i;
	if(p_client_tab->detail_info_num < p_client_tab->ip_mac_num){
		NMP_DEBUG("Deep Scan !\n");
		lock = file_lock("networkmap");
		check_asus_discovery(p_client_tab, p_client_tab->detail_info_num);
		nmp_wl_offline_check(p_client_tab, 0);
		NMP_DEBUG("wireless: %d\n", p_client_tab->wireless[p_client_tab->detail_info_num]);
		FindHostname(p_client_tab);
		if(p_client_tab->ipMethod[p_client_tab->detail_info_num][0] == 0){
			NMP_DEBUG("Static client found!\n");
			strlcpy(p_client_tab->ipMethod[p_client_tab->detail_info_num], "Static", 7);
		}
#if defined(RTCONFIG_BWDPI)
		if(sw_mode == 1 && check_bwdpi_nvram_setting()){
			NMP_DEBUG("BWDPI ON!\n");
			QueryBwdpiInfo(p_client_tab, p_client_tab->detail_info_num);
		}
#endif

		if(p_client_tab->device_name[p_client_tab->detail_info_num][0]){
			for(i = 0; i < 32; i++){
				if((p_client_tab->device_name[p_client_tab->detail_info_num][i] - ' ') > 95)
					p_client_tab->device_name[p_client_tab->detail_info_num][i] = ' ';
			}
		}
		if(p_client_tab->type[p_client_tab->detail_info_num] == 9 && strstr(p_client_tab->vendor_name[p_client_tab->detail_info_num], "ASUS"))
			p_client_tab->os_type[p_client_tab->detail_info_num] = 28;
		file_unlock(lock);
#ifdef NMP_DB
		if(check_db_size())
			write_to_DB(p_client_tab, nmp_cl_json);
#endif
		p_client_tab->detail_info_num++;
		NMP_DEBUG("Finish Deep Scan no.%d!\n", p_client_tab->detail_info_num);
	}
#ifdef NMP_DB
	else if(p_client_tab->detail_info_num != p_client_tab->commit_no || client_updated){
		NMP_DEBUG_M("Write to DB file!\n");
		lock = file_lock("networkmap");
		p_client_tab->commit_no = p_client_tab->detail_info_num;
		file_unlock(lock);
		json_object_to_file(NMP_CL_JSON_FILE, nmp_cl_json);
		client_updated = 0;
		NMP_DEBUG_M("Finish Write to DB file!\n");
		if(p_client_tab->detail_info_num != p_client_tab->ip_mac_num)
			return;

	}
#endif
	if(p_client_tab->detail_info_num == p_client_tab->ip_mac_num){
		if(nvram_match("nmp_wl_offline_check", "1")){
			lock = file_lock("networkmap");
			nmp_wl_offline_check(p_client_tab, 1);
			file_unlock(lock);
		}
	}
}

void count_num_of_clients()
{
	int i = 0, nmp_wired = 0, nmp_wlan_2g = 0, nmp_wlan_5g_1 = 0, nmp_wlan_5g_2 = 0;
	CLIENT_DETAIL_INFO_TABLE *p_client_tab = G_CLIENT_TAB;
	NMP_CONSOLE_DEBUG("[count_num_of_clients]p_client_detail_info_tab->ip_mac_num=%d\n", p_client_tab->ip_mac_num);
	NMP_CONSOLE_DEBUG("[count_num_of_clients]p_client_detail_info_tab->detail_info_num=%d\n", p_client_tab->detail_info_num);
	if(p_client_tab->detail_info_num >= p_client_tab->ip_mac_num){
		for(i = 0; i < MAX_NR_CLIENT_LIST; i++){
			if((p_client_tab->device_flag[i] & (1<<FLAG_EXIST)) == 0)
				continue;
			NMP_CONSOLE_DEBUG("-----[%02d]-----\n", i);
			NMP_CONSOLE_DEBUG("[count_num_of_clients]p_client_detail_info_tab->device_flag=%d\n", p_client_tab->device_flag[i]);
			NMP_CONSOLE_DEBUG("[count_num_of_clients]p_client_detail_info_tab->wireless=%d\n", p_client_tab->wireless[i]);
			if(p_client_tab->wireless[i] == 1)
				nmp_wlan_2g++;
			else if(p_client_tab->wireless[i] == 0)
				nmp_wired++;
			else if(p_client_tab->wireless[i] == 2)
				nmp_wlan_5g_1++;
			else if(p_client_tab->wireless[i] == 3)
				nmp_wlan_5g_2++;
			else
				cprintf("[count_num_of_clients]Error!!Should not come here!!!\n");
		}
		if(i == MAX_NR_CLIENT_LIST){
			NMP_CONSOLE_DEBUG("[count_num_of_clients]%d, %d, %d, %d\n", nmp_wired, nmp_wlan_2g, nmp_wlan_5g_1, nmp_wlan_5g_2);
			nvram_set_int("fb_nmp_wired", nmp_wired);
			nvram_set_int("fb_nmp_wlan_2g", nmp_wlan_2g);
			nvram_set_int("fb_nmp_wlan_5g_1", nmp_wlan_5g_1);
			nvram_set_int("fb_nmp_wlan_5g_2", nmp_wlan_5g_2);
			nvram_set("fb_nmp_scan", "0");
			return;
		}
	}
}

/******************************************/
int main(int argc, char *argv[])
{
	int arp_sockfd, arp_getlen, i, wifi_num = 0;
	struct sockaddr_in router_addr, router_netmask;
	char router_ipaddr[16], router_mac[18], buffer[ARP_BUFFER_SIZE];
	unsigned char scan_ipaddr[4]; // scan ip
	ARP_HEADER * arp_ptr;
	struct timeval arp_timeout = {2, 0};
	int shm_client_detail_info_id , refresh_networkmap;
	int arp_scan_trigger = 1;
	int lock;
#if defined(RTCONFIG_QCA) && defined(RTCONFIG_WIRELESSREPEATER)	
	char *mac;
#endif
	//Rawny: save client_list in memory 
	char word[128];
	char *next;
	CLIENT_DETAIL_INFO_TABLE *p_client_tab;
	time_t scan_start_time, scan_sys_arp_time, refresh_scan_arp_time;
	sw_mode = sw_mode();
	foreach(word, nvram_safe_get("wl_ifnames"), next)
		wifi_num++;
	scan_start_time = time(NULL);
	scan_sys_arp_time = time(NULL);
	refresh_scan_arp_time = time(NULL);
	int nmp_deep_scan = nvram_get_int("nmp_deep_scan");
	if(nmp_deep_scan == 0)
		nmp_deep_scan = 300;
	
	FILE *fp = fopen("/var/run/networkmap.pid", "w");
	if(fp != NULL){
		fprintf(fp, "%d", getpid());
		fclose(fp);
	}

	nmp_cl_json = json_object_from_file(NMP_CL_JSON_FILE);
	if(nmp_cl_json == NULL){
		NMP_DEBUG("open networkmap client list json database ERR:\n");
		nmp_cl_json = json_object_new_object();
	}
	if(nvram_get("nmp_client_list"))
		nvram_unset("nmp_client_list");
	if(sw_mode == SW_MODE_AP || sw_mode == SW_MODE_REPEATER)
		arp_full_scan = 1;
	//Get Router's IP/Mac
	strcpy(router_ipaddr, nvram_safe_get("lan_ipaddr"));
	strcpy(router_mac, get_lan_hwaddr());
#if defined(RTCONFIG_QCA) && defined(RTCONFIG_WIRELESSREPEATER)
	if (sw_mode == SW_MODE_REPEATER && (mac = getStaMAC()) != NULL)
		strncpy(router_mac, mac, sizeof(router_mac));
#endif
	inet_aton(router_ipaddr, &router_addr.sin_addr);
	memcpy(my_ipaddr,  &router_addr.sin_addr, 4);
	if(router_mac[0])
		ether_atoe(router_mac, my_hwaddr);
	if(nvram_get("lan_netmask"))
		inet_aton(nvram_get("lan_netmask"), &router_netmask.sin_addr);
	else
		inet_aton(nvram_default_get("lan_netmask"), &router_netmask.sin_addr);
	if(sw_mode == SW_MODE_ROUTER)
		memcpy(gateway_ipaddr, my_ipaddr, 4);
	else{
		struct sockaddr_in router_gateway;
		char lan_gateway[20];
		strlcpy(lan_gateway, nvram_safe_get("lan_gateway"), sizeof(lan_gateway));
		inet_aton(lan_gateway, &router_gateway.sin_addr);
		memcpy(gateway_ipaddr, &router_gateway.sin_addr, 4);
	}
	NMP_DEBUG("check max scan count: %d clients capacity %d\n", MAX_NR_CLIENT_LIST, MAX_NR_CLIENT_LIST);	
	// create UDP socket and bind to "br0" to get ARP packet//
	arp_sockfd = create_socket(INTERFACE);

	if(arp_sockfd < 0)
		perror("create socket ERR:");
	else {
		setsockopt(arp_sockfd, SOL_SOCKET, SO_RCVTIMEO, &arp_timeout, sizeof(arp_timeout));//set receive timeout
		dst_sockll = src_sockll; //Copy sockaddr info to dst
		memset(dst_sockll.sll_addr, 0xff, sizeof(dst_sockll.sll_addr)); // set dmac= FF:FF:FF:FF:FF:FF
		//Initial Shared Memory
		//client tables
		G_CLIENT_TAB = init_shared_memory(G_CLIENT_TAB, SHMKEY_LAN);
	}
	p_client_tab = G_CLIENT_TAB;

	//initial trigger flag
#ifdef RTCONFIG_NOTIFICATION_CENTER
	TRIGGER_FLAG = atoi(nvram_safe_get("networkmap_trigger_flag"));
	if(TRIGGER_FLAG < 0 || TRIGGER_FLAG > 15) TRIGGER_FLAG = 0;
	NMP_DEBUG(" Test networkmap trigger flag >>> %d!\n", TRIGGER_FLAG);	
#endif
	networkmap_oui_json = json_object_from_file(NEWORKMAP_OUI_FILE);
	if(networkmap_oui_json)
		update_oui = 1;
	else
		NMP_DEBUG("open OUI database ERR:\n");
	NMP_DEBUG(" OUI enable %d!\n", update_oui);
	g_conv_state = construct_ac_trie(&nmp_conv_type[0]);
	g_bwdpi_state = construct_ac_trie(&nmp_bwdpi_type[0]);
	g_vendor_state = construct_ac_trie(&nmp_vendor_type[0]);
	NMP_DEBUG("end of loading automata\n");
	networkmap_fullscan = 1;
	refresh_networkmap = 0;
	nvram_unset("rescan_networkmap");
	nvram_unset("refresh_networkmap");
	nvram_set("networkmap_fullscan", "0");
	if(argc > 1 && strcmp(argv[1], "--bootwait") == 0){
		sleep(30);
	}
	signal(SIGUSR1, refresh_sig); //catch UI refresh signal
	signal(SIGTERM, safe_leave);
	delete_sig = 0;
	signal(SIGUSR2, delete_sig_on);
	signal(SIGALRM, clean_client_list);
	eval("asusdiscovery");	//find asus device

	while(daemon_exit == 0){
//		scan_count = 0;
//		arp_full_scan = 1;
		while(arp_scan_trigger){
			if(networkmap_fullscan == 3){
				NMP_DEBUG("Clean share memory client list!\n");
				lock = file_lock("networkmap");
				memset(G_CLIENT_TAB, 0, sizeof(CLIENT_DETAIL_INFO_TABLE));
				file_unlock(lock);
				networkmap_fullscan = 1;
			}
			if(scan_count == 0)
				arp_parser_scan(G_CLIENT_TAB, INTERFACE);//scan arp cache
			if(networkmap_fullscan == 1){ //Scan all IP address in the subnetwork
				if(scan_count == 0){
					// (re)-start from the begining
					NMP_DEBUG("Starting full scan!\n");
					nvram_set("networkmap_fullscan", "1");
					if(nvram_match("refresh_networkmap", "1")){//reset client tables
						NMP_DEBUG("Flush arp!\n");
						if(arp_full_scan == 0)
							system("ip -s -s neigh flush all\n");
						eval("asusdiscovery");	//find asus device
						nvram_unset("refresh_networkmap");
						refresh_networkmap = 1;
						NMP_DEBUG("networkmap: rescan client list!\n");
						reset_client_list(G_CLIENT_TAB);
						NMP_DEBUG("reset client list over\n");
					}
					memset(scan_ipaddr, 0x00, 4);
					memcpy(scan_ipaddr, &router_addr.sin_addr, 3);
					arp_timeout.tv_sec = 0;
					arp_timeout.tv_usec = 60000;
					setsockopt(arp_sockfd, SOL_SOCKET, SO_RCVTIMEO, &arp_timeout, sizeof(arp_timeout));//set receive timeout
				}
				if(nvram_match("rescan_networkmap", "1")){
					nvram_unset("rescan_networkmap");
					scan_count = 0;
					continue;
				}
				scan_count++;
				scan_ipaddr[3]++;	
				if(scan_count < MAX_NR_CLIENT_LIST && memcmp(scan_ipaddr, my_ipaddr, 4)){
					sent_arppacket(arp_sockfd, (unsigned char *)scan_ipaddr);
				}         
				else if(scan_count >= MAX_NR_CLIENT_LIST){ //Scan completed
					arp_timeout.tv_sec = 2;
					arp_timeout.tv_usec = 0;
					setsockopt(arp_sockfd, SOL_SOCKET, SO_RCVTIMEO, &arp_timeout, sizeof(arp_timeout));//set receive timeout
					networkmap_fullscan = 0;
					scan_count = 0;
					arp_scan_trigger = 0;
					arp_parser_scan(G_CLIENT_TAB, INTERFACE);//scan arp cache again
					nvram_set("networkmap_fullscan", "2");
					NMP_DEBUG("Finish full scan!\n");
					break;
				}
			}// End of full scan
			memset(buffer, 0, ARP_BUFFER_SIZE);
			arp_getlen = recvfrom(arp_sockfd, buffer, ARP_BUFFER_SIZE, 0, NULL, NULL);
			if(arp_getlen == -1 || arp_getlen < (int)(sizeof(ARP_HEADER))){
				if(scan_count >= MAX_NR_CLIENT_LIST)
					break;
			}else{
				arp_ptr = (ARP_HEADER*)(buffer);
				if(p_client_tab->ip_mac_num < MAX_NR_CLIENT_LIST){
					nmap_receive_arp(p_client_tab, arp_ptr, scan_count);
				}else{
					clean_client_list(0);
					nvram_set("refresh_networkmap", "1");
					continue;
				}
			}//End of arp_getlen != -1
		} // End of while for flush buffer
#ifdef NMP_DB
		//RAwny: check delete signal
		if(delete_sig) {
			client_updated = DeletefromDB(G_CLIENT_TAB, nmp_cl_json);
			delete_from_share_memory(G_CLIENT_TAB);
			delete_sig = 0;
		}
#endif
		if(refresh_networkmap){
			reset_networkmap(G_CLIENT_TAB);
			eval("asusdiscovery");	//find asus device
			refresh_networkmap = 0;
		}else if(nmp_deep_scan < time(NULL) - scan_start_time){
			scan_start_time = time(NULL);
			refresh_networkmap = 0;
			arp_scan_trigger = 1;
			nvram_set("networkmap_status", "1");
		}else if(30 < time(NULL) - scan_sys_arp_time){
			scan_sys_arp_time = time(NULL);
			arp_parser_scan(G_CLIENT_TAB, INTERFACE);//scan arp cache again
		}else if(1800 < time(NULL) - refresh_scan_arp_time){
			refresh_scan_arp_time = time(NULL);
			networkmap_fullscan = 3;
		}
		deep_scan(G_CLIENT_TAB);
		if(nvram_match("fb_nmp_scan", "1"))
			count_num_of_clients();
		if(nvram_match("nmp_wl_offline_check", "1"))
			nvram_unset("nmp_wl_offline_check");
		if(nvram_match("rescan_networkmap", "1"))
			continue;
		nvram_set("networkmap_status", "0");    // Done scanning and resolving
	} //End of main while loop
	file_unlock(networkmap_lock);
	json_object_put(nmp_cl_json);
	unlink(NMP_VC_JSON_FILE);
	free_ac_state(g_vendor_state);
	free_ac_state(g_conv_state);
	free_ac_state(g_bwdpi_state);
	shmdt(G_CLIENT_TAB);
	if(update_oui)
		json_object_put(networkmap_oui_json);
	close(arp_sockfd);
	nvram_unset("rescan_networkmap");
	nvram_unset("refresh_networkmap");
	return 0;
}
