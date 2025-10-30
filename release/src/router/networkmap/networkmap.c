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
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <netinet/in.h>
#if !defined(RTCONFIG_MUSL_LIBC) && !defined(MUSL_LIBC) || defined(CONFIG_BCMWL5)
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
#include "protect_name.h"

#define vstrsep(buf, sep, args...) _vstrsep(buf, sep, args, NULL)
#define MAX_SUBNET_SCAN		(24) /* 23..24 */
#define NUM_CLIENTS_SCAN	(1U << (32U - MAX_SUBNET_SCAN))

unsigned char my_hwaddr[6];
unsigned char my_ipaddr[4];
unsigned char my_ipmask[4];
unsigned char gateway_ipaddr[4] = {0};
unsigned char broadcast_hwaddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
volatile unsigned int scan_now = 0;
static unsigned int scan_max = NUM_CLIENTS_SCAN - 1;
static unsigned int scan_net = 0xC0A80A00;
static volatile int scan_block = 0;
static volatile int networkmap_fullscan = 0; //0=init,1=start scan,2=done,3=reset list and restart scan
static volatile int daemon_exit = 0;
static int arp_sockfd = -1;
int lock;
static int scan_count = 0, sw_mode = 0;
static int delete_sig;
int arp_full_scan = 0, update_oui = 0;
int networkmap_lock;

struct json_object *nmp_cl_json = NULL;
struct json_object *G_OUI_JSON = NULL;
CLIENT_DETAIL_INFO_TABLE *G_CLIENT_TAB = NULL;
time_t sig_refresh_start_ts = 0;
int sig_refresh_interval = 0;
int check_clients_interval = 0;
int clients_start_ts = 0;
ac_state *g_conv_state = NULL, *g_vendor_state= NULL;
#ifdef RTCONFIG_BWDPI
ac_state *g_bwdpi_state = NULL;
#endif
#ifdef RTCONFIG_MULTILAN_CFG
int mtlan_map = 0;
int mtlan_updated = 0;
int mtlan_arpfd[MTLAN_MAXINUM] = {0};
char mtlan_br[MTLAN_MAXINUM][16] = {{0}};
unsigned char mtlan_ip[4];
struct sockaddr_in mtlan_ipaddr[MTLAN_MAXINUM] = {0};
#endif

extern int nmp_wl_offline_check(CLIENT_DETAIL_INFO_TABLE *p_client_tab, int offline);
extern int FindDevice(unsigned char *pIP, unsigned char *pMac, int replaceMac);
extern int check_vendorclass(const char *mac, CLIENT_DETAIL_INFO_TABLE *tab, int x, char *file);
extern int check_oui(const char *mac_buf, CLIENT_DETAIL_INFO_TABLE *tab, unsigned int x);
extern int QueryVendorOuiInfo(CLIENT_DETAIL_INFO_TABLE *tab, int x);
time_t refresh_arp_scan();
extern int json_checker(const char *json_str);
extern int free_ac_state(ac_state *state);

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
	{2, "dsl-", BASE_TYPE_DEFAULT},
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


#if 0
static int is_same_subnet(struct in_addr *ip1, struct in_addr *ip2, struct in_addr *msk)
{
	unsigned int mask = ntohl(msk->s_addr);

	return ((ntohl(ip1->s_addr) & mask) == (ntohl(ip2->s_addr) & mask)) ? 1 : 0;
}
#endif

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
	int err, flags = 1;;
	socklen_t errlen = sizeof(err);

	memset(&src_sockll, 0, sizeof(src_sockll));
	src_sockll.sll_family          = AF_PACKET;
	src_sockll.sll_ifindex         = ifindex;
	src_sockll.sll_protocol        = htons(ETH_P_ARP);
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flags, sizeof(flags));
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

int sent_arppacket(int raw_sockfd, unsigned char *dst_ipaddr, unsigned char *source_ipaddr)
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
	memcpy(arp->source_ipaddr, source_ipaddr, 4);
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
	NMP_DEBUG("Send ARP Request success, src_ipaddr = %hhu.%hhu.%hhu.%hhu, to dst_ipaddr = %hhu.%hhu.%hhu.%hhu\n", source_ipaddr[0], source_ipaddr[1], source_ipaddr[2], source_ipaddr[3], dst_ipaddr[0], dst_ipaddr[1], dst_ipaddr[2], dst_ipaddr[3]);
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
		if(nvram_match("refresh_networkmap", "9999"))
			networkmap_fullscan = 0;
		else
			refresh_arp_scan();
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

time_t refresh_arp_scan()
{
	time_t sig_refresh_end_ts;

	sig_refresh_end_ts = time(NULL);
	if(sig_refresh_end_ts - sig_refresh_start_ts <= sig_refresh_interval){
		if (nvram_get_int("refresh_networkmap"))
			nvram_unset("refresh_networkmap");
	}else{
		NMP_DEBUG("refresh_arp_scan, sig_refresh_end_ts(%lu) - sig_refresh_start_ts(%lu) > sig_refresh_interval(%d)\n", sig_refresh_end_ts, sig_refresh_start_ts, sig_refresh_interval);
		sig_refresh_start_ts = time(NULL);
		networkmap_fullscan = 0;
	}
	return sig_refresh_start_ts;
}

#ifdef NMP_DB
int commit_no = 0;
int client_updated = 0;

static void convert_mac_to_string(unsigned char *mac, char *mac_str, size_t len)
{
	snprintf(mac_str, len, "%02X:%02X:%02X:%02X:%02X:%02X",
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

void write_vendorclass(const char *mac, CLIENT_DETAIL_INFO_TABLE *tab, struct json_object *a3)
{
	NMP_DEBUG("write_vendorclass, mac : %s\n", mac);
}

int check_vendorclass(const char *mac, CLIENT_DETAIL_INFO_TABLE *tab, int x, char *file)
{
	char typeID = 0, base = 0;
	struct json_object *root = NULL;
	struct json_object *o_macaddr = NULL;
	struct json_object *o_vendorclass = NULL;
	NMP_DEBUG("check_vendorclass: %s\n", mac);
	root = json_object_from_file(file);
	if(!root)
		return -1;
	json_object_object_get_ex(root, mac, &o_macaddr);
	if(!o_macaddr){
		json_object_put(root);
		return -1;
	}
	json_object_object_get_ex(o_macaddr, "vendorclass", &o_vendorclass);
	if(!o_vendorclass){
		json_object_put(root);
		return -1;
	}
	strlcpy(tab->vendorClass[x], json_object_get_string(o_vendorclass), sizeof(tab->vendorClass[0]));
	NMP_DEBUG("%s(): Get vendorClass = %s (len=%d), os_type = %d, type = %d\n", __func__, tab->vendorClass[x],
		strlen(tab->vendorClass[x]), tab->os_type[x], tab->type[x]);
	typeID = full_search(g_vendor_state, tab->vendorClass[x], &base);
	if(tab->vendorClass[x][0] && typeID != 0){
		strlcpy(tab->vendor_name[x], tab->vendorClass[x], sizeof(tab->vendor_name[0]));
		strlcpy(tab->device_name[x], tab->vendorClass[x], sizeof(tab->device_name[0]));
		type_filter(tab, x, typeID, base, 0);
		NMP_DEBUG("%s >> mac[%s] >> vendor_class data >> Find vendorType >> device name = %s, vendor_name = %s, index = %d, typeID = %d,  os_type = %d \n", __func__, mac, tab->device_name[x], tab->vendor_name[x], x, typeID, base);
		json_object_put(root);
		return 0;
	}
	json_object_put(root);
	return -1;
}

void write_to_DB(CLIENT_DETAIL_INFO_TABLE *p_client_tab, struct json_object *ncl_json)
{
	char new_mac[32] = {0};
	struct json_object *client = NULL, *tmpobj;
	convert_mac_to_string(p_client_tab->mac_addr[p_client_tab->detail_info_num], new_mac, sizeof(new_mac));
	NMP_DEBUG("write_to_DB, write to mac: %s\n", new_mac);
	json_object_object_get_ex(ncl_json, new_mac, &client);
	if(client == NULL){
		client = json_object_new_object();
		json_object_object_add(client, "mac", json_object_new_string(new_mac));
		json_object_object_add(client, "name", json_object_new_string(p_client_tab->device_name[p_client_tab->detail_info_num]));
		json_object_object_add(client, "vendor", json_object_new_string(p_client_tab->vendor_name[p_client_tab->detail_info_num]));
		json_object_object_add(client, "vendorclass", json_object_new_string(p_client_tab->vendorClass[p_client_tab->detail_info_num]));
		json_object_object_add(client, "type", json_object_new_int(p_client_tab->type[p_client_tab->detail_info_num]));
		json_object_object_add(client, "os_type", json_object_new_int(p_client_tab->os_type[p_client_tab->detail_info_num]));
#ifdef RTCONFIG_MULTILAN_CFG
		json_object_object_add(client, "sdn_idx", json_object_new_int(p_client_tab->sdn_idx[p_client_tab->detail_info_num]));
		json_object_object_add(client, "sdn_type", json_object_new_string(p_client_tab->sdn_type[p_client_tab->detail_info_num]));
#endif
		json_object_object_add(client, "online", json_object_new_int(p_client_tab->online[p_client_tab->detail_info_num]));
		json_object_object_add(client, "wireless", json_object_new_int(p_client_tab->wireless[p_client_tab->detail_info_num]));
		json_object_object_add(client, "is_wireless", json_object_new_int(p_client_tab->is_wireless[p_client_tab->detail_info_num]));
		json_object_object_add(client, "conn_ts", json_object_new_int(p_client_tab->conn_ts[p_client_tab->detail_info_num]));
		json_object_object_add(ncl_json, new_mac, client);
	}else{
		json_object_object_get_ex(client, "mac", &tmpobj);
		if(tmpobj && strncmp(new_mac, json_object_get_string(tmpobj), sizeof(new_mac))){
			json_object_object_add(client, "mac", json_object_new_string(new_mac));
			client_updated = 1;
		}
		json_object_object_get_ex(client, "name", &tmpobj);
		if(tmpobj && strncmp(p_client_tab->device_name[p_client_tab->detail_info_num], json_object_get_string(tmpobj), sizeof(p_client_tab->device_name[0]))){
			json_object_object_add(client, "name", json_object_new_string(p_client_tab->device_name[p_client_tab->detail_info_num]));
			client_updated = 1;
		}
		json_object_object_get_ex(client, "vendor", &tmpobj);
		if(tmpobj && strncmp(p_client_tab->vendor_name[p_client_tab->detail_info_num], json_object_get_string(tmpobj), sizeof(p_client_tab->vendor_name[0]))){
			json_object_object_add(client, "vendor", json_object_new_string(p_client_tab->vendor_name[p_client_tab->detail_info_num]));
			client_updated = 1;
		}
		json_object_object_get_ex(client, "vendorclass", &tmpobj);
		if(tmpobj && strncmp(p_client_tab->vendorClass[p_client_tab->detail_info_num], json_object_get_string(tmpobj), sizeof(p_client_tab->vendorClass[0]))){
			json_object_object_add(client, "vendorclass", json_object_new_string(p_client_tab->vendorClass[p_client_tab->detail_info_num]));
			client_updated = 1;
		}
		json_object_object_get_ex(client, "type", &tmpobj);
		if(tmpobj && p_client_tab->type[p_client_tab->detail_info_num] != json_object_get_int(tmpobj)){
			json_object_object_add(client, "type", json_object_new_int(p_client_tab->type[p_client_tab->detail_info_num]));
			client_updated = 1;
		}
		json_object_object_get_ex(client, "os_type", &tmpobj);
		if(tmpobj && p_client_tab->os_type[p_client_tab->detail_info_num] != json_object_get_int(tmpobj)){
			json_object_object_add(client, "os_type", json_object_new_int(p_client_tab->os_type[p_client_tab->detail_info_num]));
			client_updated = 1;
		}
#ifdef RTCONFIG_MULTILAN_CFG
		json_object_object_get_ex(client, "sdn_idx", &tmpobj);
		if(tmpobj && p_client_tab->sdn_idx[p_client_tab->detail_info_num] != json_object_get_int(tmpobj)){
			json_object_object_add(client, "sdn_idx", json_object_new_int(p_client_tab->sdn_idx[p_client_tab->detail_info_num]));
			client_updated = 1;
		}
		json_object_object_get_ex(client, "sdn_type", &tmpobj);
		if(tmpobj && strncmp(p_client_tab->sdn_type[p_client_tab->detail_info_num],json_object_get_string(tmpobj), sizeof(p_client_tab->sdn_type[0]))){
			json_object_object_add(client, "sdn_type", json_object_new_string(p_client_tab->sdn_type[p_client_tab->detail_info_num]));
			client_updated = 1;
		}
#endif
		json_object_object_get_ex(client, "online", &tmpobj);
		if(tmpobj && p_client_tab->online[p_client_tab->detail_info_num] != json_object_get_int(tmpobj)){
			json_object_object_add(client, "online", json_object_new_int(p_client_tab->online[p_client_tab->detail_info_num]));
			client_updated = 1;
		}
		json_object_object_get_ex(client, "wireless", &tmpobj);
		if(tmpobj && p_client_tab->wireless[p_client_tab->detail_info_num] != json_object_get_int(tmpobj)){
			json_object_object_add(client, "wireless", json_object_new_int(p_client_tab->wireless[p_client_tab->detail_info_num]));
			client_updated = 1;
		}
		json_object_object_get_ex(client, "is_wireless", &tmpobj);
		if(tmpobj && p_client_tab->is_wireless[p_client_tab->detail_info_num] != json_object_get_int(tmpobj)){
			if(p_client_tab->is_wireless[p_client_tab->detail_info_num]){
				json_object_object_add(client, "is_wireless", json_object_new_int(p_client_tab->is_wireless[p_client_tab->detail_info_num]));
				client_updated = 1;
			}
		}
		json_object_object_get_ex(client, "conn_ts", &tmpobj);
		if(tmpobj && p_client_tab->conn_ts[p_client_tab->detail_info_num] != json_object_get_int(tmpobj)){
			json_object_object_add(client, "conn_ts", json_object_new_int(p_client_tab->conn_ts[p_client_tab->detail_info_num]));
			client_updated = 1;
		}
	}
}

int DeletefromDB(CLIENT_DETAIL_INFO_TABLE *p_client_tab, struct json_object * nmp_cl_json)
{
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
	swap_string(p_client_tab->ip_addr[client2], p_client_tab->ip_addr[client1], sizeof(p_client_tab->ip_addr[0]));
	swap_string(p_client_tab->mac_addr[client2], p_client_tab->mac_addr[client1], sizeof(p_client_tab->mac_addr[0]));
	swap_string(p_client_tab->user_define[client2], p_client_tab->user_define[client1], sizeof(p_client_tab->user_define[0]));
	swap_string(p_client_tab->vendor_name[client2], p_client_tab->vendor_name[client1], sizeof(p_client_tab->vendor_name[0]));
	swap_string(p_client_tab->device_name[client2], p_client_tab->device_name[client1], sizeof(p_client_tab->device_name[0]));
	swap_string(p_client_tab->apple_model[client2], p_client_tab->apple_model[client1], sizeof(p_client_tab->apple_model[0]));
	swap_string(p_client_tab->device_type[client2], p_client_tab->device_type[client1], sizeof(p_client_tab->device_type[0]));
	swap_string(p_client_tab->vendorClass[client2], p_client_tab->vendorClass[client1], sizeof(p_client_tab->vendorClass[0]));
	tmp = p_client_tab->os_type[client2];
	p_client_tab->os_type[client2] = p_client_tab->os_type[client1];
	p_client_tab->os_type[client1] = tmp;
#ifdef RTCONFIG_MULTILAN_CFG
	tmp = p_client_tab->sdn_idx[client2];
	p_client_tab->sdn_idx[client2] = p_client_tab->sdn_idx[client1];
	p_client_tab->sdn_idx[client1] = tmp;
	swap_string(p_client_tab->sdn_type[client2], p_client_tab->sdn_type[client1], sizeof(p_client_tab->sdn_type[0]));
#endif
	tmp = p_client_tab->online[client2];
	p_client_tab->online[client2] = p_client_tab->online[client1];
	p_client_tab->online[client1] = tmp;
	tmp = p_client_tab->type[client2];
	p_client_tab->type[client2] = p_client_tab->type[client1];
	p_client_tab->type[client1] = tmp;
	swap_string(p_client_tab->ipMethod[client2], p_client_tab->ipMethod[client1], sizeof(p_client_tab->ipMethod[0]));
	tmp = p_client_tab->opMode[client2];
	p_client_tab->opMode[client2] = p_client_tab->opMode[client1];
	p_client_tab->opMode[client1] = tmp;
	tmp = p_client_tab->device_flag[client2];
	p_client_tab->device_flag[client2] = p_client_tab->device_flag[client1];
	p_client_tab->device_flag[client1] = tmp;
 	tmp = p_client_tab->wireless[client2];
	p_client_tab->wireless[client2] = p_client_tab->wireless[client1];
	p_client_tab->wireless[client1] = tmp;
 	tmp = p_client_tab->is_wireless[client2];
	p_client_tab->is_wireless[client2] = p_client_tab->is_wireless[client1];
	p_client_tab->is_wireless[client1] = tmp;
 	tmp = p_client_tab->conn_ts[client2];
	p_client_tab->conn_ts[client2] = p_client_tab->conn_ts[client1];
	p_client_tab->conn_ts[client1] = tmp;
 	tmp = p_client_tab->offline_time[client2];
	p_client_tab->offline_time[client2] = p_client_tab->offline_time[client1];
	p_client_tab->offline_time[client1] = tmp;
#ifdef RTCONFIG_LANTIQ
 	tmp = p_client_tab->tstamp[client2];
	p_client_tab->tstamp[client2] = p_client_tab->tstamp[client1];
	p_client_tab->tstamp[client1] = tmp;
#endif
	swap_string(p_client_tab->ssid[client2], p_client_tab->ssid[client1], sizeof(p_client_tab->ssid[0]));
	swap_string(p_client_tab->txrate[client2], p_client_tab->txrate[client1], sizeof(p_client_tab->txrate[0]));
	swap_string(p_client_tab->rxrate[client2], p_client_tab->rxrate[client1], sizeof(p_client_tab->rxrate[0]));
 	tmp = p_client_tab->rssi[client2];
	p_client_tab->rssi[client2] = p_client_tab->rssi[client1];
	p_client_tab->rssi[client1] = tmp;
	swap_string(p_client_tab->conn_time[client2], p_client_tab->conn_time[client1], sizeof(p_client_tab->conn_time[0]));
#if defined(RTCONFIG_BWDPI)
	swap_string(p_client_tab->bwdpi_host[client2], p_client_tab->bwdpi_host[client1], sizeof(p_client_tab->bwdpi_host[0]));
	swap_string(p_client_tab->bwdpi_vendor[client2], p_client_tab->bwdpi_vendor[client1], sizeof(p_client_tab->bwdpi_vendor[0]));
	swap_string(p_client_tab->bwdpi_type[client2], p_client_tab->bwdpi_type[client1], sizeof(p_client_tab->bwdpi_type[0]));
	swap_string(p_client_tab->bwdpi_device[client2], p_client_tab->bwdpi_device[client1], sizeof(p_client_tab->bwdpi_device[0]));
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
	G_CLIENT_TAB->os_type[x] = 0;
#ifdef RTCONFIG_MULTILAN_CFG
	G_CLIENT_TAB->sdn_idx[x] = 0;
	memset(G_CLIENT_TAB->sdn_type[x], 0, sizeof(G_CLIENT_TAB->sdn_type[x]));
#endif
	G_CLIENT_TAB->online[x] = 0;
	G_CLIENT_TAB->type[x] = 0;
	memset(G_CLIENT_TAB->ipMethod[x], 0, sizeof(G_CLIENT_TAB->ipMethod[x]));
	G_CLIENT_TAB->opMode[x] = 0;
	G_CLIENT_TAB->device_flag[x] = 0;
	G_CLIENT_TAB->wireless[x] = 0;
	G_CLIENT_TAB->is_wireless[x] = 0;
	G_CLIENT_TAB->conn_ts[x] = 0;
	G_CLIENT_TAB->offline_time[x] = 0;
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
	int i = 0, ip_mac_num, lock;
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
	if(ip_mac_num > 0){
		for(i = 0; i < ip_mac_num; i++){
			if(!memcmp(mac_binary, p_client_tab->mac_addr[i], 6)){
				lock = file_lock("networkmap");
				NMP_DEBUG("remove %d\n", i);
				remove_client(i);
				file_unlock(lock);
				break;
			}
		}
	}
	NMP_DEBUG("SWAP client list\n");
	if(i < ip_mac_num){
		lock = file_lock("networkmap");
		G_CLIENT_TAB->ip_mac_num -= 1;
		G_CLIENT_TAB->detail_info_num -= 1;
		G_CLIENT_TAB->commit_no -= 1;
		if((G_CLIENT_TAB->device_flag[i] & (1<<FLAG_ASUS)) != 0)
			G_CLIENT_TAB->asus_device_num -= 1;
		if(i < G_CLIENT_TAB->ip_mac_num){
			while(i != G_CLIENT_TAB->ip_mac_num){
				swap_client_tab(G_CLIENT_TAB, i, i + 1);
				i++;
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

#ifdef RTCONFIG_BWDPI
void StringChk(char *chk_string)
{
        char *ptr = chk_string;
        while(*ptr != 0) {
                if(*ptr < 0x20 || *ptr > 0x7E)
                        *ptr = ' ';
                ptr++;
        }
}

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
	file_unlock(lock);
	return p_client_detail_info_tab;
}

void type_filter(P_CLIENT_DETAIL_INFO_TABLE p_client_detail_info_tab, int x, unsigned char type, unsigned char base, int isDev)
{
	char *devname;
	P_CLIENT_DETAIL_INFO_TABLE tmp;
	tmp = p_client_detail_info_tab;
	if(type == 34 && (base == 2 || base == 0))
		tmp->type[x] = type;
    else if(type < 29 && (base == 1 || base == 4 || base == 0))
		tmp->type[x] = type;
	else if(base == 0)
		tmp->type[x] = type;
	else if(tmp->os_type[x] != base){
		tmp->type[x] = type;
		tmp->os_type[x] = base;
	}
	NMP_DEBUG("%s: define type %d, os_type = %d \n", __func__, tmp->type[x], tmp->os_type[x]);
#if 0
	if(isDev){
		if(tmp->device_name[x][0] == 0x0)
			strlcpy(tmp->device_name[x], tmp->bwdpi_device[x], sizeof(tmp->device_name[0]));
		else{
			devname = strdup(tmp->device_name[x]);
			toLowerCase(devname);
			if(strstr(devname, "android"))
				strlcpy(tmp->device_name[x], tmp->bwdpi_device[x], sizeof(tmp->device_name[0]));
			free(devname);
		}
		strlcpy(tmp->apple_model[x], tmp->bwdpi_device[x], sizeof(tmp->apple_model[0]));
		NMP_DEBUG("*** Add BWDPI device model %s\n", __func__, tmp->apple_model[x]);
	}
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
		snprintf(macaddr, sizeof(macaddr), "%02X:%02X:%02X:%02X:%02X:%02X", p_client_tab->mac_addr[x][0], p_client_tab->mac_addr[x][1],
			p_client_tab->mac_addr[x][2], p_client_tab->mac_addr[x][3], p_client_tab->mac_addr[x][4], p_client_tab->mac_addr[x][5]);
		NMP_DEBUG("check_asus_discovery:  search i = %d, MAC= %s\n", x, macaddr);
		nv = nvp = strdup(nvram_safe_get("asus_device_list"));
		if(nv){
			if(strstr(nv, macaddr)){
				while((p = strsep(&nvp, "<")) != NULL){
					if((vstrsep(p, ">",&type ,&name, &ip, &mac, &netmask, &ssid, &submask, &opmode)) != 8)
						continue;
					NMP_DEBUG("Find MAC in Asus Discovery: %s, %s, %s, %s, %s, %s, %s, %s\n", type, name, ip,
						mac, netmask, ssid, submask, opmode);
					if(strcmp(mac, macaddr)) continue;
					name_str = strdup(name);
					toLowerCase(name_str);
					if(strstr(name_str, "cam")){
						p_client_tab->type[x] = 5;
						NMP_DEBUG("***** Find AiCam****\n");
					}else if(opmode && *opmode){
						if(atoi(opmode) == 2)
							p_client_tab->type[x] = 24;
						else
							p_client_tab->type[x] = 2;
					}
					strlcpy(p_client_tab->device_name[x], name, sizeof(p_client_tab->device_name[0]));
					strlcpy(p_client_tab->vendor_name[x], "Asus", sizeof(p_client_tab->vendor_name[0]));
					strlcpy(p_client_tab->ssid[x], ssid, sizeof(p_client_tab->ssid[0]));
					p_client_tab->opMode[x] = atoi(opmode);
					p_client_tab->device_flag[x] |= (1<<FLAG_ASUS | 1<<FLAG_HTTP);
					free(name_str);
					NMP_DEBUG("asus device: %d, %s, opMode:%d\n", p_client_tab->type[x], p_client_tab->device_name[x], p_client_tab->opMode[x]);
					p_client_tab->asus_device_num += 1;
					NMP_DEBUG("**** asus device num %d\n", p_client_tab->asus_device_num);
					break;
				}
			}
			free(nv);
		}else{
			NMP_DEBUG("%s, strdup failed\n", __func__);
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

void handle_client_list_from_arp(CLIENT_DETAIL_INFO_TABLE *tab, const char *subnet, char *myip)
{
	int ip_mac_num, ip_mac_num2, i, len = 0, isnew, arp_count = 0, lock;
	unsigned char device_flag;
	unsigned char ip_binary[4] = {0};
	unsigned char mac_binary[6] = {0};
	unsigned char *pIP = ip_binary, *pMac = mac_binary;
	FILE *fp;
	char ipaddr[18], macaddr[18], ipaddr2[18], macaddr2[18], flag[4], dev[8], line[300];
	char *next;
	struct json_object *clientlist = NULL;
	NMP_DEBUG("### %s:Start ARP file parser scan\n", __func__);
	if((fp = fopen(ARP_PATH, "r"))){
		while((next = fgets(line, sizeof(line), fp)) != NULL){
			sscanf(next, "%s%*s%s%s%*s%s\n", ipaddr, flag, macaddr, dev);
			arp_count++;
			isnew = 0;
			if(!strcmp(macaddr, "00:00:00:00:00:00"))
				continue;
			NMP_DEBUG("arp check arp_count = %d : ipBuff = %s, flags = %s, macBuff = %s, lanIf = %s, subnet = %s\n", arp_count, ipaddr, flag,  macaddr, dev, subnet);
			if(strcmp(dev, subnet) || !strncmp(flag, "0x0", 3))
				continue;
			sscanf(ipaddr, "%hhu.%hhu.%hhu.%hhu", &pIP[0], &pIP[1], &pIP[2], &pIP[3]);
			sscanf(macaddr, "%hhX:%hhX:%hhX:%hhX:%hhX:%hhX", &pMac[0], &pMac[1], &pMac[2], &pMac[3], &pMac[4], &pMac[5]);
			NMP_DEBUG("arp connect IP:%s, MAC:%s\n", ipaddr, macaddr);
			if(FindDevice(ip_binary, mac_binary, 1) == 0){
#ifdef RTCONFIG_AMAS
				lock = file_lock(CLIENTLIST_FILE_LOCK);
				clientlist = json_object_from_file(CLIENT_LIST_JSON_PATH);
				file_unlock(lock);
				snprintf(macaddr2, sizeof(macaddr2), "%02X:%02X:%02X:%02X:%02X:%02X", mac_binary[0], mac_binary[1], mac_binary[2], mac_binary[3], mac_binary[4], mac_binary[5]);
				snprintf(ipaddr2, sizeof(ipaddr2), "%d.%d.%d.%d", ip_binary[0], ip_binary[1], ip_binary[2], ip_binary[3]);
				if(!clientlist)
					break;//why not use continue?
				isnew = get_amas_client_mac(clientlist, ipaddr2, macaddr2);
				if(isnew){
					json_object_put(clientlist);
					if(!is_re_node(macaddr2, 1)){
						sscanf(macaddr2, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", pMac, &pMac[1], &pMac[2], &pMac[3], &pMac[4], &pMac[5]);
						NMP_DEBUG("CFG filter IP:%s, MAC:%s\n", ipaddr2, macaddr2);
					}
				}else{
					json_object_put(clientlist);
					if(!is_re_node(macaddr2, 1))
						NMP_DEBUG("CFG filter IP:%s, MAC:%s\n", ipaddr2, macaddr2);
				}
#endif
			}
			ip_mac_num = tab->ip_mac_num;
			isnew = 1;
			if(ip_mac_num > 0){
				for(i = 0; i < ip_mac_num; i++){
					NMP_DEBUG("*handle_client_list_from_arp, DBG dump client list, No.%d >> %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X\n", i + 1, tab->ip_addr[i][0], tab->ip_addr[i][1], tab->ip_addr[i][2], tab->ip_addr[i][3], tab->mac_addr[i][0], tab->mac_addr[i][1], tab->mac_addr[i][2], tab->mac_addr[i][3], tab->mac_addr[i][4], tab->mac_addr[i][5]);
					if(!memcmp(tab->ip_addr[i], ip_binary, sizeof(tab->ip_addr[0]))){
						if(!memcmp(tab->mac_addr[i], mac_binary, sizeof(tab->mac_addr[0]))){
							lock = file_lock("networkmap");
							tab->device_flag[i] |= (1<<FLAG_EXIST);
							file_unlock(lock);
							NMP_DEBUG("handle_client_list_from_arp , get same ip && mac, set device_flag = FLAG_EXIST");
						}else{
							NMP_DEBUG("handle_client_list_from_arp , IP assigned to another device. Refill client list.");
							NMP_DEBUG("*CMP %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X\n", tab->ip_addr[i][0], tab->ip_addr[i][1],
								tab->ip_addr[i][2], tab->ip_addr[i][3], tab->mac_addr[i][0], tab->mac_addr[i][1],
								tab->mac_addr[i][2], tab->mac_addr[i][3], tab->mac_addr[i][4], tab->mac_addr[i][5]);
							lock = file_lock("networkmap");
							tab->mac_addr[i][0] = mac_binary[0];
							tab->mac_addr[i][1] = mac_binary[1];
							tab->mac_addr[i][2] = mac_binary[2];
							tab->mac_addr[i][3] = mac_binary[3];
							tab->mac_addr[i][4] = mac_binary[4];
							tab->mac_addr[i][5] = mac_binary[5];
							tab->device_flag[i] |= (1<<FLAG_EXIST);
							file_unlock(lock);
						}
						isnew = 0;
						break;
					}else if(!memcmp(tab->mac_addr[i], mac_binary, sizeof(tab->mac_addr[0]))){
						if(FindDevice(ip_binary, mac_binary, 0) == 0){//it's an error, not same ip.
							if((tab->device_flag[i] & (1<<FLAG_EXIST)) == 0){
								NMP_DEBUG("duplicate entry upon static IP: replace IP when original arp entry not exist");
								lock = file_lock("networkmap");
								tab->ip_addr[i][0] = ip_binary[0];
								tab->ip_addr[i][1] = ip_binary[1];
								tab->ip_addr[i][2] = ip_binary[2];
								tab->ip_addr[i][3] = ip_binary[3];
								tab->device_flag[i] |= (1<<FLAG_EXIST);
								file_unlock(lock);
							}
						}else{
							NMP_DEBUG("get dhcp_ip, IP changed. Refill client list.");
							NMP_DEBUG("*CMP %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X, device_flag = FLAG_EXIST\n", tab->ip_addr[i][0], tab->ip_addr[i][1],
								tab->ip_addr[i][2], tab->ip_addr[i][3], tab->mac_addr[i][0], tab->mac_addr[i][1],
								tab->mac_addr[i][2], tab->mac_addr[i][3], tab->mac_addr[i][4], tab->mac_addr[i][5]);
							lock = file_lock("networkmap");
							tab->ip_addr[i][0] = ip_binary[0];
							tab->ip_addr[i][1] = ip_binary[1];
							tab->ip_addr[i][2] = ip_binary[2];
							tab->ip_addr[i][3] = ip_binary[3];
							tab->device_flag[i] |= (1<<FLAG_EXIST);
							file_unlock(lock);
						}
						isnew = 0;
						break;
					}
				}
			}
			if(isnew == 1 && !memcmp(ip_binary, myip, 3)){//new device, add to table
#if defined(RTCONFIG_NOTIFICATION_CENTER)
				nt_send_event_update(ip_mac_num);
#endif
				lock = file_lock("networkmap");
				tab->ip_addr[ip_mac_num][0] = ip_binary[0];
				tab->ip_addr[ip_mac_num][1] = ip_binary[1];
				tab->ip_addr[ip_mac_num][2] = ip_binary[2];
				tab->ip_addr[ip_mac_num][3] = ip_binary[3];
				tab->mac_addr[ip_mac_num][0] = mac_binary[0];
				tab->mac_addr[ip_mac_num][1] = mac_binary[1];
				tab->mac_addr[ip_mac_num][2] = mac_binary[2];
				tab->mac_addr[ip_mac_num][3] = mac_binary[3];
				tab->mac_addr[ip_mac_num][4] = mac_binary[4];
				tab->mac_addr[ip_mac_num][5] = mac_binary[5];
				tab->device_flag[ip_mac_num] |= (1<<FLAG_EXIST);
				NMP_DEBUG("*No.%d, device_flag = FLAG_EXIST, ip_addr = %d.%d.%d.%d, mac_addr = %02X:%02X:%02X:%02X:%02X:%02X\n", tab->ip_addr[ip_mac_num][0], tab->ip_addr[ip_mac_num][1], tab->ip_addr[ip_mac_num][2], tab->ip_addr[ip_mac_num][3], tab->mac_addr[ip_mac_num][0], tab->mac_addr[ip_mac_num][1], tab->mac_addr[ip_mac_num][2], tab->mac_addr[ip_mac_num][3], tab->mac_addr[ip_mac_num][4], tab->mac_addr[ip_mac_num][5]);
				check_asus_discovery(tab, ip_mac_num);
				if((tab->device_flag[ip_mac_num] & (1<<FLAG_ASUS)) == 0){
					QueryVendorOuiInfo(tab, ip_mac_num);
					NMP_DEBUG("%s(): device_name = %s, vendor_name = %s, vendorClass :  %s, os_type = %d, type = %d \n", __func__, tab->device_name[ip_mac_num], tab->vendor_name[ip_mac_num], tab->vendorClass[ip_mac_num], tab->os_type[ip_mac_num], tab->type[ip_mac_num]);
#if defined(RTCONFIG_BWDPI)
					if(sw_mode == 1 && check_bwdpi_nvram_setting()){
						NMP_DEBUG("BWDPI ON!\n");
						QueryBwdpiInfo(tab, ip_mac_num);
					}
#endif
					FindHostname(tab, ip_mac_num);
				}
				if(tab->device_name[ip_mac_num][0]){
					char *pt = &tab->device_name[ip_mac_num][0];
					while(*pt){
						if((*pt - ' ') > '^')
							*pt = ' ';
						pt++;
					}
				}
				check_wrieless_info(tab, ip_mac_num, 1, NULL);
				if((sw_mode == 3 || sw_mode == 1) && !nvram_get_int("re_mode"))
					rc_diag_stainfo(tab, ip_mac_num);
#ifdef RTCONFIG_MULTILAN_CFG
				if(!nvram_get_int("re_mode")){
					check_wrie_client_sdn_idx(tab, ip_mac_num);
					if(!tab->sdn_idx[ip_mac_num])
						strlcpy(tab->sdn_type[ip_mac_num], "DEFAULT", sizeof(tab->sdn_type[0]));
					NMP_DEBUG("handle_client_list_from_arp_table, sdn_idx = %d, sdn_type = %s\n", tab->sdn_idx[ip_mac_num], tab->sdn_type[ip_mac_num]);
				}
#endif
				NMP_DEBUG("handle_client_list_from_arp_table, Fill: %d, ip : %d, -> %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X, wireless = %d, is_wireless = %d, conn_ts = %d\n", ip_mac_num + 1, tab->ip_mac_num, tab->ip_addr[ip_mac_num][0], 
					tab->ip_addr[ip_mac_num][1], tab->ip_addr[ip_mac_num][2], tab->ip_addr[ip_mac_num][3], 
					tab->mac_addr[ip_mac_num][0], tab->mac_addr[ip_mac_num][1], tab->mac_addr[ip_mac_num][2], 
					tab->mac_addr[ip_mac_num][3], tab->mac_addr[ip_mac_num][4], tab->mac_addr[ip_mac_num][5],
					tab->wireless[ip_mac_num], tab->is_wireless[ip_mac_num], tab->conn_ts[ip_mac_num]);
				tab->online[ip_mac_num] = 1;
				tab->ip_mac_num++;
				file_unlock(lock);
			}
		}
	}
	fclose(fp);
}

void handle_client_list(CLIENT_DETAIL_INFO_TABLE *p_client_tab, ARP_HEADER *arp_hdr, char *myip, int x)
{
	int ip_mac_num, i, len = 0, isnew, lock, dhcp_flag = 0;
	uint32_t gw_ipaddr = 0;
	struct in_addr src_addr;
	unsigned short msg_type;

	//Check ARP packet if source ip and router ip at the same network
	if(memcmp(myip, arp_hdr->source_ipaddr, sizeof(arp_hdr->source_ipaddr) - 1))
		return;
	msg_type = ntohs(arp_hdr->message_type);
	if(arp_full_scan && msg_type == ARP_RESPONSE){
		if(memcmp(arp_hdr->dest_ipaddr, myip, sizeof(arp_hdr->dest_ipaddr)) || memcmp(arp_hdr->dest_hwaddr, my_hwaddr, sizeof(my_hwaddr)))
			return;
	}else{
		if(msg_type != ARP_REQUEST)
			return;
		if(memcmp(arp_hdr->dest_ipaddr, myip, sizeof(arp_hdr->dest_ipaddr)) && memcmp(arp_hdr->dest_ipaddr, gateway_ipaddr, sizeof(gateway_ipaddr)))
			return;
	}
	NMP_DEBUG("*It's an ARP Response to Router!\n");
	NMP_DEBUG("*RCV %d.%d.%d.%d, mac:%02X:%02X:%02X:%02X:%02X:%02X, scanCount : %d, msg_type: %02x,IP num:%d\n", arp_hdr->source_ipaddr[0],arp_hdr->source_ipaddr[1], arp_hdr->source_ipaddr[2],arp_hdr->source_ipaddr[3], arp_hdr->source_hwaddr[0],arp_hdr->source_hwaddr[1], arp_hdr->source_hwaddr[2], arp_hdr->source_hwaddr[3], arp_hdr->source_hwaddr[4],arp_hdr->source_hwaddr[5], x, msg_type, p_client_tab->ip_mac_num);
	NMP_DEBUG("*DEST IP %d.%d.%d.%d-%d,  sw_mode = %d, detail_info_num : %d,  ip_mac_num : %d\n", arp_hdr->dest_ipaddr[0],arp_hdr->dest_ipaddr[1],arp_hdr->dest_ipaddr[2], arp_hdr->dest_ipaddr[3], sw_mode, p_client_tab->detail_info_num, p_client_tab->ip_mac_num);
	dhcp_flag = FindDevice(arp_hdr->source_ipaddr, arp_hdr->source_hwaddr, 1);
	if(dhcp_flag == 0){
#ifdef RTCONFIG_AMAS
		char macaddr2[18], ipaddr2[16];
		struct json_object *clientlist;
		lock = file_lock(CLIENTLIST_FILE_LOCK);
		clientlist = json_object_from_file(CLIENT_LIST_JSON_PATH);
		file_unlock(lock);
		snprintf(macaddr2, sizeof(macaddr2), "%02X:%02X:%02X:%02X:%02X:%02X", arp_hdr->source_hwaddr[0], arp_hdr->source_hwaddr[1], arp_hdr->source_hwaddr[2], arp_hdr->source_hwaddr[3], arp_hdr->source_hwaddr[4], arp_hdr->source_hwaddr[5]);
		snprintf(ipaddr2, sizeof(ipaddr2), "%d.%d.%d.%d", arp_hdr->source_ipaddr[0], arp_hdr->source_ipaddr[1], arp_hdr->source_ipaddr[2], arp_hdr->source_ipaddr[3]);
		if(clientlist && get_amas_client_mac(clientlist, ipaddr2, macaddr2) != 0){
			json_object_put(clientlist);
			if(is_re_node(macaddr2, 1))
				return;
			sscanf(macaddr2, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &arp_hdr->source_hwaddr[0], &arp_hdr->source_hwaddr[1], &arp_hdr->source_hwaddr[2], &arp_hdr->source_hwaddr[3], &arp_hdr->source_hwaddr[4], &arp_hdr->source_hwaddr[5]);
			NMP_DEBUG("CFG filter IP:%s, MAC:%s, is_RE_client : %d\n", ipaddr2, macaddr2, 1);
		}else{
			json_object_put(clientlist);
			if(is_re_node(macaddr2, 1))
				return;
			NMP_DEBUG("CFG filter IP:%s, MAC:%s, is_RE_client : %d\n", ipaddr2, macaddr2, 0);
		}
#endif
		ip_mac_num = p_client_tab->ip_mac_num;
		isnew = 1;
		if(ip_mac_num > 0){
			for(i = 0; i < ip_mac_num; i++){
				if(!memcmp(p_client_tab->ip_addr[i], arp_hdr->source_ipaddr, sizeof(p_client_tab->ip_addr[0]))){// same ip, same/different mac
					isnew = 0;
					if(!memcmp(p_client_tab->mac_addr[i], arp_hdr->source_hwaddr, sizeof(p_client_tab->mac_addr[0]))){
						lock = file_lock("networkmap");
						p_client_tab->device_flag[i] |= (1<<FLAG_EXIST);
						p_client_tab->online[i] = 1;
						file_unlock(lock);
					}else{
						NMP_DEBUG("handle_client_list : IP assigned to another device. Refill client list.");
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
						p_client_tab->online[i] = 1;
						file_unlock(lock);
					}
					break;
				}else if(!memcmp(p_client_tab->mac_addr[i], arp_hdr->source_hwaddr, sizeof(p_client_tab->mac_addr[0]))){//same mac, different ip
					isnew = 0;
					dhcp_flag = FindDevice(arp_hdr->source_ipaddr, arp_hdr->source_hwaddr, 0);
					if((p_client_tab->device_flag[i] & (1<<FLAG_EXIST)) == 0){
						NMP_DEBUG("duplicate entry upon static IP: replace IP when original arp entry not exist");
						p_client_tab->ip_addr[i][0] = arp_hdr->source_ipaddr[0];
						p_client_tab->ip_addr[i][1] = arp_hdr->source_ipaddr[1];
						p_client_tab->ip_addr[i][2] = arp_hdr->source_ipaddr[2];
						p_client_tab->ip_addr[i][3] = arp_hdr->source_ipaddr[3];
						p_client_tab->device_flag[i] |= (1<<FLAG_EXIST);
						p_client_tab->online[i] = 1;
					}
					break;
				}
			}
		}
		if(isnew == 1 && !memcmp(arp_hdr->source_ipaddr, myip, sizeof(arp_hdr->source_ipaddr) - 1)){//new device, add to table
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
			if(dhcp_flag)
				p_client_tab->dhcp_flag[ip_mac_num] = 1;
			check_asus_discovery(p_client_tab, ip_mac_num);
			if((p_client_tab->device_flag[ip_mac_num] & (1<<FLAG_ASUS)) == 0){
				QueryVendorOuiInfo(p_client_tab, ip_mac_num);
				NMP_DEBUG("%s(): device_name = %s, vendor_name = %s, vendorClass :  %s, os_type = %d, type = %d \n", __func__, p_client_tab->device_name[ip_mac_num], p_client_tab->vendor_name[ip_mac_num], p_client_tab->vendorClass[ip_mac_num], p_client_tab->os_type[ip_mac_num], p_client_tab->type[ip_mac_num]);
#if defined(RTCONFIG_BWDPI)
				if(sw_mode == 1 && check_bwdpi_nvram_setting()){
					NMP_DEBUG("BWDPI ON!\n");
					QueryBwdpiInfo(p_client_tab, ip_mac_num);
				}
#endif
				FindHostname(p_client_tab, ip_mac_num);
			}
			if(p_client_tab->device_name[ip_mac_num][0]){
				char *pt = &p_client_tab->device_name[ip_mac_num][0];
				while(*pt){
					if((*pt - ' ') > '^')
						*pt = ' ';
					pt++;
				}
			}
			check_wrieless_info(p_client_tab, ip_mac_num, 1, NULL);
			if((sw_mode == 3 || sw_mode == 1) && !nvram_get_int("re_mode"))
				rc_diag_stainfo(p_client_tab, ip_mac_num);
#ifdef RTCONFIG_MULTILAN_CFG
			if(!nvram_get_int("re_mode")){
				check_wrie_client_sdn_idx(p_client_tab, ip_mac_num);
				if(!p_client_tab->sdn_idx[ip_mac_num])
					strlcpy(p_client_tab->sdn_type[ip_mac_num], "DEFAULT", sizeof(p_client_tab->sdn_type[0]));
				NMP_DEBUG("handle_client_list, sdn_idx = %d, sdn_type = %s\n", p_client_tab->sdn_idx[ip_mac_num], p_client_tab->sdn_type[ip_mac_num]);
			}
#endif
			NMP_DEBUG("Fill: %d -> %d.%d.%d.%d-%02X:%02X:%02X:%02X:%02X:%02X, wireless = %d, is_wireless = %d, conn_ts = %d\n", ip_mac_num, p_client_tab->ip_addr[ip_mac_num][0], 
				p_client_tab->ip_addr[ip_mac_num][1], p_client_tab->ip_addr[ip_mac_num][2], p_client_tab->ip_addr[ip_mac_num][3], 
				p_client_tab->mac_addr[ip_mac_num][0], p_client_tab->mac_addr[ip_mac_num][1], p_client_tab->mac_addr[ip_mac_num][2], 
				p_client_tab->mac_addr[ip_mac_num][3], p_client_tab->mac_addr[ip_mac_num][4], p_client_tab->mac_addr[ip_mac_num][5],
				p_client_tab->wireless[ip_mac_num], p_client_tab->is_wireless[ip_mac_num], p_client_tab->conn_ts[ip_mac_num]);
			p_client_tab->online[ip_mac_num] = 1;
			p_client_tab->ip_mac_num++;
			file_unlock(lock);
		}
	}
}

void handle_detail_client_list(CLIENT_DETAIL_INFO_TABLE *tab)
{
	int i, hlock;
	if(tab->detail_info_num < tab->ip_mac_num){
		NMP_DEBUG("handle_detail_client_list Scan ! detail_info_num = %d, ip_mac_num = %d\n", tab->detail_info_num, tab->ip_mac_num);
		hlock = file_lock("networkmap");
		check_asus_discovery(tab, tab->detail_info_num);
		if((tab->device_flag[tab->detail_info_num] & (1<<FLAG_ASUS)) == 0){
			QueryVendorOuiInfo(tab, tab->detail_info_num);
			NMP_DEBUG("%s(): device_name = %s, vendor_name = %s, vendorClass :  %s, os_type = %d, type = %d \n", __func__, tab->device_name[tab->detail_info_num], tab->vendor_name[tab->detail_info_num], tab->vendorClass[tab->detail_info_num], tab->os_type[tab->detail_info_num], tab->type[tab->detail_info_num]);
#if defined(RTCONFIG_BWDPI)
			if(sw_mode == 1 && check_bwdpi_nvram_setting()){
				NMP_DEBUG("BWDPI ON!\n");
				QueryBwdpiInfo(tab, tab->detail_info_num);
			}
#endif
			FindHostname(tab, tab->detail_info_num);
			if(!tab->ipMethod[tab->detail_info_num][0]){
				NMP_DEBUG("Static client found!\n");
				strlcpy(tab->ipMethod[tab->detail_info_num], "Static", sizeof(tab->ipMethod[0]));
			}
		}
		if(tab->device_name[tab->detail_info_num][0]){
			for(i = 0; i < 32; i++){
				if((tab->device_name[tab->detail_info_num][i] - ' ') > 95)
					tab->device_name[tab->detail_info_num][i] = ' ';
			}
		}
		check_wrieless_info(tab, tab->detail_info_num, 1, NULL);
		check_asus_device(tab, tab->detail_info_num);
		if((sw_mode == 3 || sw_mode == 1) && !nvram_get_int("re_mode"))
			rc_diag_stainfo(tab, tab->detail_info_num);
#ifdef RTCONFIG_MULTILAN_CFG
		if(!nvram_get_int("re_mode")){
			check_wrie_client_sdn_idx(tab, tab->detail_info_num);
			if(!tab->sdn_idx[tab->detail_info_num])
				strlcpy(tab->sdn_type[tab->detail_info_num], "DEFAULT", sizeof(tab->sdn_type[0]));
			NMP_DEBUG("handle_detail_client_list, sdn_idx = %d, sdn_type = %s\n", tab->sdn_idx[tab->detail_info_num], tab->sdn_type[tab->detail_info_num]);
		}
#endif
		nmp_wl_offline_check(tab, 0);
		NMP_DEBUG("handle_detail_client_list, detail_info_num = %d, wireless: %d\n", tab->detail_info_num, tab->wireless[tab->detail_info_num]);
		if(tab->type[tab->detail_info_num] == 9 && strstr(tab->vendor_name[tab->detail_info_num], "ASUS"))
			tab->os_type[tab->detail_info_num] = 28;
		check_brctl_mac_online(tab);
		file_unlock(hlock);
#ifdef NMP_DB
		if(check_db_size())
			write_to_DB(tab, nmp_cl_json);
#endif
		tab->detail_info_num++;
		NMP_DEBUG("Finish Deep Scan no.%d!\n", tab->detail_info_num);
	}
#ifdef NMP_DB
	else{
		if(tab->detail_info_num != tab->commit_no || client_updated){
			NMP_DEBUG_M("Write to DB file!\n");
			hlock = file_lock("networkmap");
			tab->commit_no = tab->detail_info_num;
			file_unlock(hlock);
			json_object_to_file(NMP_CL_JSON_FILE, nmp_cl_json);
			client_updated = 0;
			NMP_DEBUG_M("Finish Write to DB file!\n");
		}
	}
#endif
	if(networkmap_fullscan == 2){
		if(nvram_match("nmp_wl_offline_check", "1")){
			check_brctl_macs(tab);
			check_ip6_addr(tab);
			if(sw_mode != 1){
				hlock = file_lock("networkmap");
				nmp_wl_offline_check(tab, 1);
				file_unlock(hlock);
				return;
			}
		}
		check_clientlist_offline(tab);
	}
}

void count_num_of_clients()
{
	int i = 0, nmp_wired = 0, nmp_wlan_2g = 0, nmp_wlan_5g_1 = 0, nmp_wlan_5g_2 = 0;
	CLIENT_DETAIL_INFO_TABLE *tab = G_CLIENT_TAB;
	NMP_CONSOLE_DEBUG("[count_num_of_clients]p_client_detail_info_tab->ip_mac_num=%d\n", tab->ip_mac_num);
	NMP_CONSOLE_DEBUG("[count_num_of_clients]p_client_detail_info_tab->detail_info_num=%d\n", tab->detail_info_num);
	if(tab->detail_info_num >= tab->ip_mac_num){
		for(i = 0; i < MAX_NR_CLIENT_LIST; i++){
			if((tab->device_flag[i] & (1<<FLAG_EXIST)) == 0)
				continue;
			NMP_CONSOLE_DEBUG("-----[%02d]-----\n", i);
			NMP_CONSOLE_DEBUG("[count_num_of_clients]p_client_detail_info_tab->device_flag=%d\n", tab->device_flag[i]);
			NMP_CONSOLE_DEBUG("[count_num_of_clients]p_client_detail_info_tab->wireless=%d\n", tab->wireless[i]);
			if(tab->wireless[i] == 1)
				nmp_wlan_2g++;
			else if(tab->wireless[i] == 0)
				nmp_wired++;
			else if(tab->wireless[i] == 2)
				nmp_wlan_5g_1++;
			else if(tab->wireless[i] == 3)
				nmp_wlan_5g_2++;
			else
				_dprintf("[count_num_of_clients]Error!!Should not come here!!!\n");
		}
		NMP_CONSOLE_DEBUG("[count_num_of_clients]%d, %d, %d, %d\n", nmp_wired, nmp_wlan_2g, nmp_wlan_5g_1, nmp_wlan_5g_2);
		nvram_set_int("fb_nmp_wired", nmp_wired);
		nvram_set_int("fb_nmp_wlan_2g", nmp_wlan_2g);
		nvram_set_int("fb_nmp_wlan_5g_1", nmp_wlan_5g_1);
		nvram_set_int("fb_nmp_wlan_5g_2", nmp_wlan_5g_2);
		nvram_set("fb_nmp_scan", "0");
	}
}

int arp_loop(CLIENT_DETAIL_INFO_TABLE *p_client_tab, struct sockaddr_in router_addr, int fd, char *br_name, char *myip)
{
	int arp_getlen;
	char buffer[ARP_BUFFER_SIZE];
	unsigned char scan_ipaddr[4]; // scan ip
	ARP_HEADER * arp_ptr;
	struct timeval arp_timeout = {2, 0};

	while(daemon_exit == 0){
		if(scan_count == 0){
			// (re)-start from the begining
			NMP_DEBUG("Starting full scan,  networkmap_fullscan == SCAN_PROC, scan_count == 0\n");
			if(nvram_match("refresh_networkmap", "1")){//reset client tables
				NMP_DEBUG("Flush arp!\n");
				if(arp_full_scan == 0)
					system("ip -s -s neigh flush all\n");
				if(pids("asusdiscovery"))
					doSystem("killall -9 asusdiscovery");
				eval("asusdiscovery");	//find asus device
				nvram_unset("refresh_networkmap");
				NMP_DEBUG("networkmap: refresh_client_list\n");
				reset_client_list(G_CLIENT_TAB);
				NMP_DEBUG("reset client list over\n");
			}
			memset(scan_ipaddr, 0x00, 4);
			memcpy(scan_ipaddr, &router_addr.sin_addr, 3);
			arp_timeout.tv_sec = 0;
			arp_timeout.tv_usec = 60000;
			setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &arp_timeout, sizeof(arp_timeout));//set receive timeout
		}
		if(nvram_match("rescan_networkmap", "1")){
			nvram_unset("rescan_networkmap");
			scan_count = 0;
			continue;
		}
		scan_count++;
		scan_ipaddr[3]++;	
		if(scan_count < MAX_NR_CLIENT_LIST && memcmp(scan_ipaddr, myip, 4)){
			sent_arppacket(fd, (unsigned char *)scan_ipaddr, myip);
			memset(buffer, 0, ARP_BUFFER_SIZE);
			arp_getlen = recvfrom(fd, buffer, ARP_BUFFER_SIZE, 0, NULL, NULL);
			if(arp_getlen == -1 || arp_getlen < (int)(sizeof(ARP_HEADER))){
				if(scan_count >= MAX_NR_CLIENT_LIST)
					break;
			}else{
				arp_ptr = (ARP_HEADER*)(buffer);
				if(p_client_tab->ip_mac_num < MAX_NR_CLIENT_LIST){
					handle_client_list(p_client_tab, arp_ptr, myip, scan_count);
				}else{
					clean_client_list(0);
					nvram_set("refresh_networkmap", "1");
					continue;
				}
			}
		}         
		else if(scan_count >= MAX_NR_CLIENT_LIST){ //Scan completed
			arp_timeout.tv_sec = 2;
			arp_timeout.tv_usec = 0;
			setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &arp_timeout, sizeof(arp_timeout));//set receive timeout
			networkmap_fullscan = 2;
			scan_count = 0;
			handle_client_list_from_arp(G_CLIENT_TAB, br_name, myip);//scan arp cache again
			nvram_set("networkmap_fullscan", "2");
			NMP_DEBUG("Finish full scan!\n");
			break;
		}
	}
	return 0;
}

/******************************************/
int main(int argc, char *argv[])
{
	int i;
	struct sockaddr_in router_addr, router_netmask;
	char router_ipaddr[16], router_mac[18];
	struct timeval arp_timeout = {2, 0};
	int shm_client_detail_info_id;
	int lock;
#if defined(RTCONFIG_QCA) && defined(RTCONFIG_WIRELESSREPEATER)	
	char *mac;
#endif
	//Rawny: save client_list in memory 
	char word[128];
	char *next, *b, *idx, *ifname, *addr, *nv, *nvp;
	CLIENT_DETAIL_INFO_TABLE *p_client_tab;
	time_t scan_start_time;
	sw_mode = sw_mode();
	scan_start_time = time(NULL);
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
	if(json_checker(json_object_get_string(nmp_cl_json))){
		unlink(NMP_CL_JSON_FILE);
		NMP_DEBUG("del nmp_json file, json format err : %s\n", json_object_get_string(nmp_cl_json));
		json_object_put(nmp_cl_json);
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
#ifdef RTCONFIG_MULTILAN_CFG
	i = 0;
	foreach(word, nvram_safe_get("apg_ifnames"), next){
		NMP_DEBUG("gn apg lan %s\n", word);	
		nv = nvp = strdup(nvram_safe_get("subnet_rl"));
		if(nv){
			while ((b = strsep(&nvp, "<")) != NULL) {
				NMP_DEBUG("gn_b = %s, gn_word = %s\n", b, word);	
				if(strstr(b, word) && vstrsep(b, ">", &idx, &ifname, &addr) == 3){
					memcpy(router_ipaddr, addr, sizeof(router_ipaddr));
					NMP_DEBUG("gn apg IP %s!!\n", router_ipaddr);
					inet_aton(router_ipaddr, &mtlan_ipaddr[i].sin_addr);
					mtlan_arpfd[i] = create_socket(word);
					strlcpy(mtlan_br[i], word, sizeof(mtlan_br[0]));
					if(mtlan_arpfd[i] < 0)
						perror("create socket ERR:\n");
					else{
						setsockopt(mtlan_arpfd[i], SOL_SOCKET, SO_RCVTIMEO, &arp_timeout, sizeof(arp_timeout));
						NMP_DEBUG("gn apg %s socket create success!!\n", addr);
						mtlan_map |= 1 << i;
						i++;
					}
				}
			}
			free(nv);
		}
	}
	if(mtlan_map)
		mtlan_updated = 1;
	NMP_DEBUG("***gn_apg subnet bitmap %d\n", mtlan_map);
#endif
	//initial trigger flag
#ifdef RTCONFIG_NOTIFICATION_CENTER
	TRIGGER_FLAG = atoi(nvram_safe_get("networkmap_trigger_flag"));
	if(TRIGGER_FLAG < 0 || TRIGGER_FLAG > 15) TRIGGER_FLAG = 0;
	NMP_DEBUG(" Test networkmap trigger flag >>> %d!\n", TRIGGER_FLAG);	
#endif
	G_OUI_JSON = json_object_from_file(NEWORKMAP_OUI_FILE);
	if(G_OUI_JSON)
		update_oui = 1;
	else
		NMP_DEBUG("open OUI database ERR:\n");
	NMP_DEBUG(" OUI enable %d!\n", update_oui);
	if(check_if_file_exist(merge_conv_type_path[0]))
		g_conv_state = construct_ac_file(&nmp_conv_type[0], merge_conv_type_path[0]);
	else
		g_conv_state = construct_ac_file(&nmp_conv_type[0], "/usr/networkmap/nmp_conv_type.js");
	if(check_if_file_exist(merge_vendor_type_path[0]))
		g_vendor_state = construct_ac_file(&nmp_vendor_type[0], merge_vendor_type_path[0]);
	else
		g_vendor_state = construct_ac_file(&nmp_vendor_type[0], "/usr/networkmap/nmp_vendor_type.js");
#ifdef RTCONFIG_BWDPI
	if(check_if_file_exist(merge_bwdpi_type_path[0]))
		g_bwdpi_state = construct_ac_file(&nmp_bwdpi_type[0], merge_bwdpi_type_path[0]);
	else
		g_bwdpi_state = construct_ac_file(&nmp_bwdpi_type[0], "/usr/networkmap/nmp_bwdpi_type.js");
#endif
	clients_start_ts = time(NULL) - 15;
	networkmap_fullscan = 0;
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
	if(pids("asusdiscovery"))
		doSystem("killall -9 asusdiscovery");
	eval("asusdiscovery &");	//find asus device

	while(daemon_exit == 0){
		if(networkmap_fullscan == 3){
			NMP_DEBUG("Clean share memory client list!\n");
			lock = file_lock("networkmap");
			memset(G_CLIENT_TAB, 0, sizeof(CLIENT_DETAIL_INFO_TABLE));
			file_unlock(lock);
			networkmap_fullscan == 0;
		}
		if(networkmap_fullscan == 0){
			networkmap_fullscan = 1;
			nvram_set("networkmap_fullscan", "1");
			network_ip_scan();
			sig_refresh_start_ts = time(NULL);
			handle_client_list_from_arp(G_CLIENT_TAB, INTERFACE, my_ipaddr);//scan arp cache
#ifdef RTCONFIG_MULTILAN_CFG
			for(i = 0; i < MTLAN_MAXINUM; i++){
				if(((mtlan_map >> i) & 0x1) == 1){
					memcpy(mtlan_ip, &mtlan_ipaddr[i].sin_addr, sizeof(mtlan_ip));
					handle_client_list_from_arp(G_CLIENT_TAB, mtlan_br[i], mtlan_ip);//scan arp cache
				}
			}
#endif
		}
		if(networkmap_fullscan == 1){ //Scan all IP address in the subnetwork
			arp_loop(p_client_tab, router_addr, arp_sockfd, INTERFACE, my_ipaddr);
#ifdef RTCONFIG_MULTILAN_CFG
		for(i = 0; i < MTLAN_MAXINUM; i++){
			if(((mtlan_map >> i) & 0x1) == 1){
				memcpy(mtlan_ip, &mtlan_ipaddr[i].sin_addr, sizeof(mtlan_ip));
				arp_loop(p_client_tab, mtlan_ipaddr[i], mtlan_arpfd[i], mtlan_br[i], mtlan_ip);
			}
		}
#endif
		}
#ifdef NMP_DB
		//RAwny: check delete signal
		if(delete_sig) {
			client_updated = DeletefromDB(G_CLIENT_TAB, nmp_cl_json);
			delete_from_share_memory(G_CLIENT_TAB);
			delete_sig = 0;
			memset(G_CLIENT_TAB->delete_mac, 0, sizeof(G_CLIENT_TAB->delete_mac));
		}
#endif

		if(nmp_deep_scan < time(NULL) - scan_start_time){
			NMP_DEBUG("t_scan(%d) - t_start(%d) > deep_scan_interval(%d)\n", time(NULL), scan_start_time, nmp_deep_scan);
			scan_start_time = time(NULL);
			networkmap_fullscan = 0;
			nvram_set("networkmap_status", "1");
			nvram_set("nmp_wl_offline_check", "1");
			continue;
		}
		handle_client_list_from_arp(G_CLIENT_TAB, INTERFACE, my_ipaddr);//scan arp cache again
#ifdef RTCONFIG_MULTILAN_CFG
			for(i = 0; i < MTLAN_MAXINUM; i++){
				if(((mtlan_map >> i) & 0x1) == 1){
					memcpy(mtlan_ip, &mtlan_ipaddr[i].sin_addr, sizeof(mtlan_ip));
					handle_client_list_from_arp(G_CLIENT_TAB, mtlan_br[i], mtlan_ip);//scan arp cache again
				}
			}
#endif
		handle_detail_client_list(G_CLIENT_TAB);
		if(nvram_match("fb_nmp_scan", "1"))
			count_num_of_clients();
		if(nvram_match("nmp_wl_offline_check", "1"))
			nvram_unset("nmp_wl_offline_check");
		if(nvram_match("rescan_networkmap", "1"))
			continue;
		nvram_set("networkmap_status", "0");    // Done scanning and resolving
		sleep(2);
	} //End of main while loop
	file_unlock(networkmap_lock);
	json_object_put(nmp_cl_json);
	unlink(NMP_VC_JSON_FILE);
	free_ac_state(g_vendor_state);
	free_ac_state(g_conv_state);
#ifdef RTCONFIG_BWDPI
	free_ac_state(g_bwdpi_state);
#endif
	shmdt(G_CLIENT_TAB);
	if(update_oui)
		json_object_put(G_OUI_JSON);
	close(arp_sockfd);
#ifdef RTCONFIG_MULTILAN_CFG
	for(i = 0; i < MTLAN_MAXINUM; i++){
		if(((mtlan_map >> i) & 0x1) == 1){
			close(mtlan_arpfd[i]);
		}
	}
#endif
	nvram_unset("rescan_networkmap");
	nvram_unset("refresh_networkmap");
	return 0;
}
