/* rewrite by paldier */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <bcmnvram.h>
#include <wlutils.h>
#include "utils.h"
#include "shutils.h"
#include "shared.h"
#include "lantiq.h"

#ifdef RTCONFIG_AMAS
#include <net/ethernet.h>
#include <amas_path.h>
#endif
#include <net/if.h>
#include <netlink/errno.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include "nl80211_copy.h"
#include <linux/nl80211.h>

#ifdef LINUX26
#define GPIO_IOCTL
#endif


enum {
	WAN_PORT=0,
	LAN1_PORT,
	LAN2_PORT,
	LAN3_PORT,
	LAN4_PORT,
	MAX_WANLAN_PORT
};
#define NR_WANLAN_PORT	MAX_WANLAN_PORT

struct cmd_respdata {
	uint32_t attr;
	int len;
	void *data;
};

struct nlwifi_ctx {
	enum nl80211_commands cmd;
	int flags;

	/* cmd callback and data */
	int (*cb)(struct nl_msg *msg, void *data);
	void *data;
};


/**
 * The vport_to_iface array is used to get interface name of each virtual
 * port.  If bled need to know TX/RX statistics of LAN1~2, WAN1, WAN2 (AQR107),
 * and 10G SFP+, bled has to find this information from netdev.  So, define
 * this array and implement vport_to_iface_name() function which is used by
 * bled in update_swports_bled().
 *
 * array index:		virtual port mapping enumeration.
 * 			e.g. LAN1_PORT, LAN2_PORT, etc.
 * array element:	Interface name of specific virtual port.
 */
static const char *vport_to_iface[MAX_WANLAN_PORT] = {
	"eth1",							/* WAN */
	"eth0_1",							/* LAN1 */
	"eth0_2",							/* LAN2 */
	"eth0_3",							/* LAN3 */
	"eth0_4",							/* LAN4 */
};

/* Model-specific LANx ==> Model-specific virtual PortX.
 * array index:	Model-specific LANx (started from 0).
 * array value:	Model-specific virtual port number.
 */
const int lan_id_to_vport[NR_WANLAN_PORT] = {
	WAN_PORT,
	LAN1_PORT,
	LAN2_PORT,
	LAN3_PORT,
	LAN4_PORT,
};

/* Model-specific LANx (started from 0) ==> Model-specific virtual PortX */
static inline int lan_id_to_vport_nr(int id)
{
	return lan_id_to_vport[id];
}

void mac_allow_list_add(int band, unsigned char *mac)
{
	char *ai_mac;
	char *tmp;
	char mac_buf[17] = "XX:XX:XX:XX:XX:XX";

	snprintf(mac_buf, sizeof(mac_buf), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	if(band)
		ai_mac = nvram_safe_get("aimesh_macacl_5g_mac");
	else
		ai_mac = nvram_safe_get("aimesh_macacl_2g_mac");

	if(!strstr(ai_mac, mac_buf)){
		if((tmp = calloc(1, strlen(ai_mac) + strlen(mac_buf) + 2))){
			strcpy(tmp, ai_mac);
			tmp[strlen(ai_mac)] = '<';
			strcpy(&tmp[strlen(ai_mac) + 1], mac_buf);
			if(band)
				nvram_set("aimesh_macacl_5g_mac", tmp);
			else
				nvram_set("aimesh_macacl_2g_mac", tmp);
			free(tmp);
		}else{
			_dprintf("malloc error\n");
		}
	}
}

// --- move begin ---
#ifdef GPIO_IOCTL

#include <sys/ioctl.h>

static int _gpio_ioctl(int f, int gpioreg, unsigned int mask, unsigned int val)
{
	return 0;
}

static int _gpio_open()
{
	return 0;
}

int gpio_open(uint32_t mask)
{
	return 0;
}

void gpio_write(uint32_t bitvalue, int en)
{

}

uint32_t _gpio_read(int f)
{
	return 0;
}

uint32_t gpio_read(void)
{
	return 0;
}

#else

int gpio_open(uint32_t mask)
{
	return 0;
}

void gpio_write(uint32_t bitvalue, int en)
{

}

uint32_t _gpio_read(int f)
{
	return 0;
}

uint32_t gpio_read(void)
{
	return 0;
}

#endif

#ifdef RTCONFIG_AMAS 
static bool g_swap = FALSE;
#define htod32(i) (g_swap?bcmswap32(i):(uint32)(i))
#define dtoh32(i) (g_swap?bcmswap32(i):(uint32)(i))
#define dtoh16(i) (g_swap?bcmswap16(i):(uint16)(i))
char *get_pap_bssid(int unit, char bssid_str[])
{
	FILE *fp;
	int len;
	char tmp[8192];
	char *ap;

	memset(bssid_str, 0, sizeof(bssid_str));
	snprintf(tmp, sizeof(tmp), "iwconfig %s", get_staifname(unit));
	if ((fp=popen(tmp, "r"))){
		memset(tmp, 0, sizeof(tmp));
		len = fread(tmp, 1, sizeof(tmp), fp)
		pclose(fp);
		if(len < 2)
			continue;
		tmp[len-1] = 0;
		if((ap = strstr(tmp, "Access Point:"))){
			if(!strstr(ap+14, "Not-Associated"))
				strncpy(bssid_str, ap+14, sizeof(bssid_str));
		}
	}

	return bssid_str;
}

int set_wl_macfilter()
{
	_dprintf("shared %s %d\n", __func__, __LINE__);
	return 0;
}


int get_psta_status(int unit)
{
	FILE *fp;
	int len, status;
	char tmp[8192];
	char *ap;

	snprintf(tmp, sizeof(tmp), "iwconfig %s", get_staifname(unit));
	if ((fp=popen(tmp, "r"))){
		memset(tmp, 0, sizeof(tmp));
		len = fread(tmp, 1, sizeof(tmp), fp)
		pclose(fp);
		if(len < 2)
			status = 3;
		else {
			tmp[len-1] = 0;
			if((ap = strstr(tmp, "Access Point:"))){
				status = 2;
				if(strstr(ap+13, "Not-Associated")){
					snprintf(tmp, sizeof(tmp), "ifconfig | grep %s", get_staifname(unit));
					if ((fp=popen(tmp, "r"))){
						memset(tmp, 0, sizeof(tmp));
						len = fread(tmp, 1, sizeof(tmp), fp)
						pclose(fp);
						status = 0;
						if(len > 0)
							return status;
						status = 3;
					} else
						status = 0;
				}
			} else
				status = 3;
		}
	} else
		status = 3;
	return status;
}

#ifdef RTCONFIG_AMAS
void add_beacon_vsie(char *hexdata)
{
	nvram_set("amas_add_beacon_vsie", hexdata);
	trigger_wave_monitor_and_wait(__func__, __LINE__, WAVE_ACTION_ADD_BEACON_VSIE, 1);
}

void del_beacon_vsie(char *hexdata)
{
	nvram_set("amas_del_beacon_vsie", hexdata);
	trigger_wave_monitor_and_wait(__func__, __LINE__, WAVE_ACTION_DEL_BEACON_VSIE, 1);
}
#endif

#ifdef RTCONFIG_CFGSYNC
void update_macfilter_relist()
{
	char maclist_buf[4096] = {0};
	struct maclist *maclist = NULL;
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";
	char word[256], *next;
	char mac2g[32], mac5g[32], *next_mac;
	int unit = 0;
	char wlif_name[IFNAMSIZ] = { 0 };
	struct ether_addr *ea;
	unsigned char sta_ea[6] = {0};
	int ret = 0;
	char *nv, *nvp, *b;
	char *reMac, *maclist2g, *maclist5g, *timestamp;
	char stamac2g[18] = {0};
	char stamac5g[18] = {0};
	char wl_ifnames[32] = { 0 };
#ifdef RTCONFIG_AMAS
	nvram_unset("aimesh_macacl_2g_mac");
	nvram_unset("aimesh_macacl_5g_mac");
#endif
	if (nvram_get("cfg_relist"))
	{
#ifdef RTCONFIG_AMAS
		if (nvram_get_int("re_mode") == 1) {
			nv = nvp = strdup(nvram_safe_get("cfg_relist"));
			if (nv) {
				while ((b = strsep(&nvp, "<")) != NULL) {
					if ((vstrsep(b, ">", &reMac, &maclist2g, &maclist5g, &timestamp) != 4))
						continue;
					/* first mac for sta 2g of dut */
					foreach_44 (mac2g, maclist2g, next_mac)
						break;
					/* first mac for sta 5g of dut */
					foreach_44 (mac5g, maclist5g, next_mac)
						break;

					if (strcmp(reMac, get_lan_hwaddr()) == 0) {
						snprintf(stamac2g, sizeof(stamac2g), "%s", mac2g);
						dbg("dut 2g sta (%s)\n", stamac2g);
						snprintf(stamac5g, sizeof(stamac5g), "%s", mac5g);
						dbg("dut 5g sta (%s)\n", stamac5g);
						break;
					}
				}
				free(nv);
			}
		}
#endif

		strlcpy(wl_ifnames, nvram_safe_get("wl_ifnames"), sizeof(wl_ifnames));
		foreach (word, wl_ifnames, next) {
			SKIP_ABSENT_BAND_AND_INC_UNIT(unit);

#ifdef RTCONFIG_AMAS
			if (nvram_get_int("re_mode") == 1)
				snprintf(prefix, sizeof(prefix), "wl%d.1_", unit);
			else
#endif
				snprintf(prefix, sizeof(prefix), "wl%d_", unit);

			strlcpy(wlif_name, nvram_pf_safe_get(prefix, "ifname"), sizeof(wlif_name));
			maclist = (struct maclist *)maclist_buf;
			memset(maclist_buf, 0, sizeof(maclist_buf));
			ea = &(maclist->ea[0]);

			if (nvram_pf_match(prefix, "macmode", "allow")) {
				nv = nvp = strdup(nvram_pf_safe_get(prefix, "maclist_x"));
				if (nv) {
					while ((b = strsep(&nvp, "<")) != NULL) {
						if (strlen(b) == 0) continue;

#ifdef RTCONFIG_AMAS
						if (nvram_get_int("re_mode") == 1) {
							if (strcmp(b, stamac2g) == 0 ||
								strcmp(b, stamac5g) == 0)
								continue;
						}
#endif
						dbg("maclist sta (%s) in %s\n", b, wlif_name);
						ether_atoe(b, sta_ea);
						memcpy(ea, sta_ea, sizeof(struct ether_addr));
						maclist->count++;
						ea++;
					}
					free(nv);
				}

				nv = nvp = strdup(nvram_safe_get("cfg_relist"));
				if (nv) {
					while ((b = strsep(&nvp, "<")) != NULL) {
						if ((vstrsep(b, ">", &reMac, &maclist2g, &maclist5g, &timestamp) != 4))
							continue;

						if (strcmp(reMac, get_lan_hwaddr()) == 0)
							continue;

						if (unit == 0) {
							foreach_44 (mac2g, maclist2g, next_mac) {
								if (check_re_in_macfilter(unit, mac2g))
									continue;
								dbg("relist sta (%s) in %s\n", mac2g, wlif_name);
								ether_atoe(mac2g, sta_ea);
								memcpy(ea, sta_ea, sizeof(struct ether_addr));
								maclist->count++;
								ea++;
							}
						}
						else
						{
							foreach_44 (mac5g, maclist5g, next_mac) {
								if (check_re_in_macfilter(unit, mac5g))
									continue;
								dbg("relist sta (%s) in %s\n", mac5g, wlif_name);
								ether_atoe(mac5g, sta_ea);
								memcpy(ea, sta_ea, sizeof(struct ether_addr));
								maclist->count++;
								ea++;
							}
						}
					}
					free(nv);
				}

				dbg("maclist count[%d]\n", maclist->count);

				ret = wl_ioctl(wlif_name, WLC_SET_MACLIST, maclist, sizeof(maclist_buf));
				if (ret < 0)
					dbg("[%s] set maclist failed\n", wlif_name);
			}

			unit++;
		}
	}
}
#endif
//sta_info_t *wl_sta_info(char *ifname, struct ether_addr *ea)
//{
#if 0
	static char buf[sizeof(sta_info_t)];
	sta_info_t *sta = NULL;

	strcpy(buf, "sta_info");
	memcpy(buf + strlen(buf) + 1, (void *)ea, ETHER_ADDR_LEN);

	if (!wl_ioctl(ifname, WLC_GET_VAR, buf, sizeof(buf))) {
		sta = (sta_info_t *)buf;
		sta->ver = dtoh16(sta->ver);

		/* Report unrecognized version */
		if (sta->ver > WL_STA_VER) {
			dbg(" ERROR: unknown driver station info version %d\n", sta->ver);
			return NULL;
		}

		sta->len = dtoh16(sta->len);
		sta->cap = dtoh16(sta->cap);
#ifdef RTCONFIG_BCMARM
		sta->aid = dtoh16(sta->aid);
#endif
		sta->flags = dtoh32(sta->flags);
		sta->idle = dtoh32(sta->idle);
		sta->rateset.count = dtoh32(sta->rateset.count);
		sta->in = dtoh32(sta->in);
		sta->listen_interval_inms = dtoh32(sta->listen_interval_inms);
#ifdef RTCONFIG_BCMARM
		sta->ht_capabilities = dtoh16(sta->ht_capabilities);
		sta->vht_flags = dtoh16(sta->vht_flags);
#endif
	}

	return sta;

#endif
//}

int get_maxassoc(char *ifname)
{
#if 0
	FILE *fp = NULL;
	char maxassoc_file[128]={0};
	char buf[64]={0};
	char maxassoc[64]={0};

	snprintf(maxassoc_file, sizeof(maxassoc_file), "/tmp/maxassoc.%s", ifname);

	doSystem("wl -i %s maxassoc > %s", ifname, maxassoc_file);

	if ((fp = fopen(maxassoc_file, "r")) != NULL) {
		fscanf(fp, "%s", buf);
		fclose(fp);
	}
	sscanf(buf, "%s", maxassoc);

	return atoi(maxassoc);
#endif
}

void set_wl_macfilter_test(int bssidx, int vifidx, struct maclist_r *client_list, int block)
{
#if 0
		int ret = 0, size = 0, i = 0, cnt = 0, match = 0;
		char *wlif_name = NULL;
		char tmp[128], prefix[] = "wlXXXXXXXXXX_";
		struct ether_addr *ea = NULL;

		int macmode = 0;
		char maclist_buf[WLC_IOCTL_MAXLEN] = {0};
		struct maclist *maclist = (struct maclist *) maclist_buf;
		struct maclist *static_maclist= NULL;

		if(vifidx > 0)
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", bssidx, vifidx);
		else
			snprintf(prefix, sizeof(prefix), "wl%d", bssidx);

		wlif_name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));


		ret = wl_ioctl(wlif_name, WLC_GET_MACMODE, &macmode, sizeof(macmode));
		if(ret < 0) 
		{
			dbG("[WARNING] %s get macmode error!!!\n", wlif_name);
			return;
		}

		cprintf("[%s] macmode = %s\n",
		wlif_name,
		macmode==WLC_MACMODE_DISABLED ? "DISABLE" :
		macmode==WLC_MACMODE_DENY ? "DENY" : "ALLOW");

		ret = wl_ioctl(wlif_name, WLC_GET_MACLIST, (void *)maclist, sizeof(maclist_buf));
		if(ret < 0) 
		{
			cprintf("[WARNING] %s get maclist error!!!\n", wlif_name);
			return;
		}

		if (maclist->count > 0 && maclist->count < 128) 
		{
			size = sizeof(uint) + sizeof(struct ether_addr) * (maclist->count);

			static_maclist = (struct maclist *)malloc(size);

			if (!(static_maclist)) 
			{
				cprintf("%s malloc [%d] failure... \n", __FUNCTION__, size);
				return;
			}

			memcpy(static_maclist, maclist, size);
			static_maclist->count = maclist->count;
			memset(maclist, 0x00, sizeof(maclist_buf));
			//maclist = static_maclist;

			for (size = 0; size < maclist->count; size++) {
				cprintf("%d [%s] (%d)mac:"MACF"\n",__LINE__, wlif_name, size, ETHER_TO_MACF(maclist->ea[size]));
			}	

		} 
		else if (maclist->count != 0) 
		{
			cprintf("Err: %s maclist cnt [%d] too large\n",
			wlif_name, maclist->count);
			return;
		}
		else if (maclist->count == 0) {
			cprintf("maclist is empty\n");
		}

		if (block == 0 && macmode == WLC_MACMODE_DISABLED)
			macmode = WLC_MACMODE_DISABLED;
		else if (macmode == WLC_MACMODE_DENY || macmode == WLC_MACMODE_DISABLED)
			macmode = WLC_MACMODE_DENY;
		else
			macmode = WLC_MACMODE_ALLOW;


		if (macmode == WLC_MACMODE_DENY || macmode == WLC_MACMODE_DISABLED) 
		{
			if (block == 1) 
			{              	
				if (static_maclist) 
				{
					cprintf("Deny mode: Adding static maclist\n");
					maclist->count = static_maclist->count;
					memcpy(maclist_buf, static_maclist,
					sizeof(uint) + ETHER_ADDR_LEN * (maclist->count));

					ea = &(maclist->ea[maclist->count]);

					for (cnt = 0; cnt < client_list->count; cnt++) {
						cprintf("Deny mode: static mac[%d] addr:"MACF"\n", cnt,ETHER_TO_MACF(client_list->ea[cnt]));
						match = 0;
						for  (i = 0; i < static_maclist->count ; i++) {
							cprintf("Checking "MACF"\n", ETHER_TO_MACF(static_maclist->ea[i]));
							if (memcmp(&(client_list->ea[cnt]), &(static_maclist->ea[i]), sizeof(struct ether_addr)) == 0) {
								cprintf("MATCH maclist "MACF"\n", ETHER_TO_MACF(client_list->ea[cnt]));
								match = 1;
								break;
							}
						}
						if (!match) {
							memcpy(ea, &(client_list->ea[cnt]), sizeof(struct ether_addr));
							maclist->count++;
							cprintf("Adding to Deny list: cnt[%d] addr:"MACF"\n",maclist->count, ETHERP_TO_MACF(ea));
							ea++;
						}
					}
				}
				else {
					if (client_list->count > 0) {
					maclist->count = client_list->count;
					memcpy(maclist, client_list, sizeof(uint) + ETHER_ADDR_LEN * (client_list->count));	                			                	
					}
				}
			}
			else if (block == 0) 
			{

				ea = &(maclist->ea[0]);
				maclist->count = 0;
				if (static_maclist) 
				{

					for (cnt = 0; cnt < static_maclist->count; cnt++) {
						cprintf("%d:Deny mode: static mac[%d] addr:"MACF"\n",__LINE__, cnt,
						ETHER_TO_MACF(static_maclist->ea[cnt]));
						match = 0;
						for  (i = 0; i < client_list->count ; i++) {
							cprintf("Checking "MACF"\n", ETHER_TO_MACF(client_list->ea[i]));
							if (memcmp(&(client_list->ea[i]), &(static_maclist->ea[cnt]),  sizeof(struct ether_addr)) == 0) 
							{
								cprintf("MATCH maclist "MACF"\n", ETHER_TO_MACF(client_list->ea[i]));
								match = 1;
								break;
							}
							else {
								cprintf("Remove from Deny list: cnt[%d] addr:"MACF"\n",
								i,ETHER_TO_MACF(client_list->ea[i]));
							}
						}
						if (!match) 
						{
							memcpy(ea, &(static_maclist->ea[cnt]), sizeof(struct ether_addr));
							maclist->count++;
							cprintf("Adding to Deny list: cnt[%d] addr:"MACF"\n",
							maclist->count, ETHERP_TO_MACF(ea));
							ea++;
						}	                
					}
				}
			}      
		}
		else {  //ALLOW MODE

			if (block == 1) 
			{
				ea = &(maclist->ea[0]);
				maclist->count = 0;

				if (static_maclist) 
				{

					for (cnt = 0; cnt < static_maclist->count; cnt++) {
						cprintf("%d:Allow mode: static mac[%d] addr:"MACF"\n",__LINE__, cnt,
						ETHER_TO_MACF(static_maclist->ea[cnt]));
						match = 0;
						for  (i = 0; i < client_list->count ; i++) {
							cprintf("%d Checking client_list :"MACF", static_maclist:"MACF"\n",__LINE__, ETHER_TO_MACF(client_list->ea[i]), ETHER_TO_MACF(static_maclist->ea[cnt]));
							if (memcmp(&(client_list->ea[i]), &(static_maclist->ea[cnt]),  sizeof(struct ether_addr)) == 0) {
							cprintf("%d MATCH maclist "MACF"\n", __LINE__,ETHER_TO_MACF(static_maclist->ea[cnt]));
							match = 1;
							break;
							}
						}	                        
						if (!match) {
							memcpy(ea, &(static_maclist->ea[cnt]), sizeof(struct ether_addr));
							maclist->count++;
							cprintf("%d Adding to Allow list: cnt[%d] addr:"MACF"\n",__LINE__,
							maclist->count, ETHERP_TO_MACF(ea));
							ea++;
						}	                
					}
				}
			}
			else if (block == 0) 
			{
				if (static_maclist) 
				{
					cprintf("Allow mode: Adding static maclist\n");
					maclist->count = static_maclist->count;
					memcpy(maclist_buf, static_maclist,
					sizeof(uint) + ETHER_ADDR_LEN * (maclist->count));


					ea = &(maclist->ea[maclist->count]);

					for (cnt = 0; cnt < client_list->count; cnt++) {
						cprintf("Allow mode: static mac[%d] addr:"MACF"\n", cnt,ETHER_TO_MACF(client_list->ea[cnt]));
						match = 0;
						
						for  (i = 0; i < static_maclist->count ; i++) {
							cprintf("Checking "MACF"\n", ETHER_TO_MACF(static_maclist->ea[i]));
							if (memcmp(&(client_list->ea[cnt]), &(static_maclist->ea[i]), sizeof(struct ether_addr)) == 0) {
								cprintf("MATCH maclist "MACF"\n", ETHER_TO_MACF(client_list->ea[cnt]));
								match = 1;
								break;
							}
						}
						if (!match) {
							memcpy(ea, &(client_list->ea[cnt]), sizeof(struct ether_addr));
							maclist->count++;
							cprintf("Adding to Allow list: cnt[%d] addr:"MACF"\n",
							maclist->count, ETHERP_TO_MACF(ea));
							ea++;
						}
					}
				}
				else {
					if (client_list->count > 0) {
						maclist->count = client_list->count;
						memcpy(maclist, client_list, sizeof(uint) + ETHER_ADDR_LEN * (client_list->count));	                			                	
					}
				}
			}
		}

		cprintf("maclist count[%d] \n", maclist->count);
		for (cnt = 0; cnt < maclist->count; cnt++) {
			cprintf("%d maclist: "MACF"\n",__LINE__,
			ETHER_TO_MACF(maclist->ea[cnt]));
		}

		doSystem("wl -i %s mac none",wlif_name);

		if (maclist->count > 0) {
			ret = wl_ioctl(wlif_name, WLC_SET_MACMODE, &macmode, sizeof(macmode));
			if(ret < 0) {
				cprintf("[WARNING] %s set macmode[%d] error!!!\n", wlif_name, macmode);
				goto err;
			}

			ret = wl_ioctl(wlif_name, WLC_SET_MACLIST, maclist, sizeof(maclist_buf));
			if (ret < 0) {
				cprintf("Err: [%s] set maclist...\n", wlif_name);
			}
		}
err:
		if (static_maclist != NULL)
			free(static_maclist);
#endif			
}

#endif

struct nl_sock *nl80211_socket(void)
{
	struct nl_sock *nl;

	nl = nl_socket_alloc();
	if (nl == NULL)
		return NULL;

	genl_connect(nl);

	return nl;
}

void *nl80211_alloc_msg(struct nl_sock *sk, int cmd, int flags, size_t priv)
{
	struct nl_msg *msg;
	int id;

	msg = nlmsg_alloc();
	if (!msg)
		return NULL;

	id = genl_ctrl_resolve(sk, "nl80211");
	if (id < 0) {
		nlmsg_free(msg);
		return NULL;
	}

	genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, id, priv, flags, (uint8_t)cmd, 0);

	return msg;
}

static int nl80211_ack_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;

	*ret = 0;
	return NL_STOP;
}

static int nl80211_finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;

	*ret = 0;
	return NL_SKIP;
}

static int nl80211_err_handler(struct sockaddr_nl *nla, struct nlmsgerr *err,
								void *arg)
{
	int *ret = arg;

	*ret = err->error;
	return NL_STOP;
}

static int nl80211_no_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

int nl80211_send_msg(struct nl_sock *nl, struct nl_msg *msg,
			int (*cmd_resp_handler)(struct nl_msg *, void *),
			void *cmd_resp_data)
{
	struct nl_cb *cb;
	int err = 1;

	if (!msg)
		return -1;

	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb)
		return -1;

	nl_socket_set_cb(nl, cb);

	nl_cb_err(cb, NL_CB_CUSTOM, nl80211_err_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, nl80211_finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, nl80211_ack_handler, &err);
	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, nl80211_no_seq_check, NULL);

	if (cmd_resp_handler)
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, cmd_resp_handler, cmd_resp_data);


	err = nl_send_auto(nl, msg);
	if (err < 0)
		goto out;

	while(err > 0) {
		if (nl_recvmsgs(nl, cb) < 0) {
			break;
		}
	}
out:
	nl_cb_put(cb);
	return err;
}

static int nl80211_get_iface_cb(struct nl_msg *msg, void *data)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct cmd_respdata *res = data;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_IFNAME])
		return NL_SKIP;

	if (!tb[res->attr])
		return NL_SKIP;

	switch (res->attr) {
	case NL80211_ATTR_CHANNEL_WIDTH:
		{
			uint32_t bw;

			bw = nla_get_u32(tb[NL80211_ATTR_CHANNEL_WIDTH]);
			*((uint32_t *)res->data) = bw;
		}
		break;
	case NL80211_ATTR_WIPHY_FREQ:
		{
			uint32_t freq;

			freq = nla_get_u32(tb[NL80211_ATTR_WIPHY_FREQ]);
			*((uint32_t *)res->data) = freq;
		}
		break;
	case NL80211_ATTR_IFTYPE:
		{
			uint32_t iftype;

			iftype = nla_get_u32(tb[NL80211_ATTR_IFTYPE]);
			*((uint32_t *)res->data) = iftype;
		}
		break;
	case NL80211_ATTR_CENTER_FREQ1:
		{
			uint32_t center1;

			center1 = nla_get_u32(tb[NL80211_ATTR_CENTER_FREQ1]);
			*((uint32_t *)res->data) = center1;
		}
		break;
	default:
		/* libwifi_dbg("res->attr = default\n"); */
		res->len = nla_len(tb[res->attr]);
		memcpy(res->data, nla_data(tb[res->attr]), res->len);
		break;
	}

	return NL_SKIP;
}

static int phy_nametoindex(const char *name)
{
	char buf[200];
	int fd, pos;

	snprintf(buf, sizeof(buf), "/sys/class/ieee80211/%s/index", name);

	fd = open(buf, O_RDONLY);
	if (fd < 0)
		return -1;
	pos = read(fd, buf, sizeof(buf) - 1);
	if (pos < 0) {
		close(fd);
		return -1;
	}
	buf[pos] = '\0';
	close(fd);
	return atoi(buf);
}

static int nl80211_cmd(const char *name, struct nlwifi_ctx *ctx)
{
	struct nl_sock *nl;
	struct nl_msg *msg;
	int devidx;
	int ret = -1;

	if (strstr(name, "phy")) {
		devidx = phy_nametoindex(name);
		if (devidx < 0)
			goto out;
	} else {
		devidx = if_nametoindex(name);
		if (devidx == 0) {
			ret = -errno;
			goto out;
		}
	}

	nl = nl80211_socket();
	if (!nl)
		goto out;

	msg = nl80211_alloc_msg(nl, ctx->cmd, ctx->flags, 0);
	if (!msg)
		goto out_msg_failure;

	if (ctx->flags == NLM_F_DUMP && ctx->cmd == NL80211_CMD_GET_WIPHY) {
		nla_put_flag(msg, NL80211_ATTR_SPLIT_WIPHY_DUMP);
		nlmsg_hdr(msg)->nlmsg_flags |= NLM_F_DUMP;
	}

	if (strstr(name, "phy"))
		NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, devidx);
	else
		NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, devidx);

	ret = nl80211_send_msg(nl, msg, ctx->cb, ctx->data);

nla_put_failure:
	nlmsg_free(msg);
out_msg_failure:
	nl_socket_free(nl);
out:
	return ret;
}

int wl_get_bw(int unit)
{
	int ret = -1;
	char ifname[16];
	uint32_t bw = NL80211_CHAN_WIDTH_20_NOHT;
	struct cmd_respdata res = {
		.attr = NL80211_ATTR_CHANNEL_WIDTH,
		.len = sizeof(bw),
		.data = &bw,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_INTERFACE,
		.cb = nl80211_get_iface_cb,
		.data = &res,
	};
	strlcpy(ifname, get_wififname(unit), sizeof(ifname));
	ret = nl80211_cmd(ifname, &ctx);
	if(ret == 0){
		switch (bw) {
			case NL80211_CHAN_WIDTH_20_NOHT:
			case NL80211_CHAN_WIDTH_20:
				return 20;
			case NL80211_CHAN_WIDTH_40:
				return 40;
			case NL80211_CHAN_WIDTH_80:
				return 80;
			case NL80211_CHAN_WIDTH_80P80:
			case NL80211_CHAN_WIDTH_160:
				return 160;
			default:
				break;
		}
	}
	return 20;
}

/**
 * @link:
 * 	0:	no-link
 * 	1:	link-up
 * @speed:
 * 	0,10:		10Mbps		==> 'M'
 * 	1,100:		100Mbps		==> 'M'
 * 	2,1000:		1000Mbps	==> 'G'
 * 	3,10000:	10Gbps		==> 'T'
 * 	4,2500:		2.5Gbps		==> 'Q'
 * 	5,5000:		5Gbps		==> 'F'
 */
static char conv_speed(unsigned int link, unsigned int speed)
{
	char ret = 'X';

	if (link != 1)
		return ret;

	if (speed == 2 || speed == 1000)
		ret = 'G';
	else if (speed == 3 || speed == 10000)
		ret = 'T';
	else if (speed == 4 || speed == 2500)
		ret = 'Q';
	else if (speed == 5 || speed == 5000)
		ret = 'F';
	else
		ret = 'M';

	return ret;
}

/**
 * Get link status and/or phy speed of a port. (by sysfs)
 * use /sys/class/net/NIC/speed to retrieve information
 * @link:	pointer to unsigned integer.
 * 		If link != NULL,
 * 			*link = 0 means link-down
 * 			*link = 1 means link-up.
 * @speed:	pointer to unsigned integer.
 * 		If speed != NULL,
 * 			*speed = 10/100/1000/2500 Mbps
 * @return:
 * 	0:	success
 *     -1:	invalid parameter
 *  otherwise:	fail
 */
static void get_phy_info_by_sysfs(unsigned int vport, unsigned int *link, unsigned int *speed, phy_info *info)
{
	unsigned int l = 0, s = 0;
	char path[256];
	char sp_str[10]; // -1, 10, 100, 1000, 2500, 10000

	snprintf(path, sizeof(path), "/sys/class/net/%s/operstate", vport_to_iface[vport]);
	if (f_read_string(path, sp_str, sizeof(sp_str)) > 0) {
		if(!strncmp(sp_str, "up", 2)){
			if(info)
				snprintf(info->state, sizeof(info->state), "up");

			snprintf(path, sizeof(path), "/sys/class/net/%s/speed", vport_to_iface[vport]);
			if (f_read_string(path, sp_str, sizeof(sp_str)) > 0) {
				int sp_val = atoi(sp_str);
				if (sp_val > 0) {
					l = 1;
					s = sp_val;
				}
				if(info)
					info->link_rate = s;
			}
		}else if(info)
			snprintf(info->state, sizeof(info->state), "down");
	}else if(info)
		snprintf(info->state, sizeof(info->state), "down");

	if(link)
		*link = l;
	if(speed)
		*speed = s;
}

/**
 * Get link status and/or phy speed of a virtual port.
 * @vport:	virtual port number
 * @link:	pointer to unsigned integer.
 * 		If link != NULL,
 * 			*link = 0 means link-down
 * 			*link = 1 means link-up.
 * @speed:	pointer to unsigned integer.
 * 		If speed != NULL,
 * 			*speed = 1 means 100Mbps
 * 			*speed = 2 means 1000Mbps
 * @return:
 * 	0:	success
 *     -1:	invalid parameter
 *  otherwise:	fail
 */
int get_vport_info(unsigned int vport, unsigned int *link, unsigned int *speed, phy_info *info)
{
	if (vport >= MAX_WANLAN_PORT || (!link && !speed))
		return -1;

	if (link)
		*link = 0;
	if (speed)
		*speed = 0;

	get_phy_info_by_sysfs(vport, link, speed, info);

	return 0;
}

#ifdef RTCONFIG_NEW_PHYMAP
void get_phy_port_mapping(phy_port_mapping *port_mapping)
{
	int i, id;
	static phy_port_mapping port_mapping_static = {
#if defined(BLUECAVE) || defined(RAX40)
		.count = NR_WANLAN_PORT,
		.port[0] = { .phy_port_id = WAN_PORT,  .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_WAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[1] = { .phy_port_id = LAN1_PORT, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[2] = { .phy_port_id = LAN2_PORT, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[3] = { .phy_port_id = LAN3_PORT, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[4] = { .phy_port_id = LAN4_PORT, .ext_port_id = -1, .label_name = "L4", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
#elif defined(K3C)
		.count = NR_WANLAN_PORT - 1,
		.port[0] = { .phy_port_id = WAN_PORT,  .ext_port_id = -1, .label_name = "W0", .cap = PHY_PORT_CAP_WAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[1] = { .phy_port_id = LAN1_PORT, .ext_port_id = -1, .label_name = "L1", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[2] = { .phy_port_id = LAN2_PORT, .ext_port_id = -1, .label_name = "L2", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
		.port[3] = { .phy_port_id = LAN3_PORT, .ext_port_id = -1, .label_name = "L3", .cap = PHY_PORT_CAP_LAN, .max_rate = 1000, .ifname = NULL, .flag = 0, .seq_no = -1, .ui_display = NULL },
#else
		#error "port_mapping is not defined."
#endif
	};
	if (!port_mapping)
		return;

///////////////// copy all Ethernet port here ////////////////////////
	memcpy(port_mapping, &port_mapping_static, sizeof(phy_port_mapping));

///////////////// Assign Ethernet NIC name here ////////////////////////
	for (i = 0; i < port_mapping->count; i++) {
		id = port_mapping->port[i].phy_port_id;
		if (id != -1)
			port_mapping->port[i].ifname = (char *)vport_to_iface[id];
	}

///////////////// Add USB port define here ////////////////////////
#if defined(BLUECAVE) || defined(K3C) || defined(RAX40)
////  1 USB3 port device
	i = port_mapping->count++;
	port_mapping->port[i].phy_port_id = -1;
	port_mapping->port[i].label_name = "U1";
	port_mapping->port[i].cap = PHY_PORT_CAP_USB;
	port_mapping->port[i].max_rate = 5000;
	port_mapping->port[i].ifname = NULL;
#endif

	add_sw_cap(port_mapping);
	swap_wanlan(port_mapping);
	return;
}
#endif // end of RTCONFIG_NEW_PHYMAP

void ATE_port_status(int verbose, phy_info_list *list)
{
	int i, len;
	char buf[64];
#ifdef RTCONFIG_NEW_PHYMAP
	char cap_buf[64] = {0};
#endif
	phyState pS;

#ifdef RTCONFIG_NEW_PHYMAP
	phy_port_mapping port_mapping;
	get_phy_port_mapping(&port_mapping);
	if (list)
		list->count = 0;

	len = 0;
	for (i = 0; i < port_mapping.count; i++) {
		// Only handle WAN/LAN ports
		if (((port_mapping.port[i].cap & PHY_PORT_CAP_WAN) == 0) && ((port_mapping.port[i].cap & PHY_PORT_CAP_LAN) == 0))
			continue;
		pS.link[i] = 0;
		pS.speed[i] = 0;
		get_vport_info(port_mapping.port[i].phy_port_id, &pS.link[i], &pS.speed[i], list ? &list->phy_info[i] : NULL);
		if (list) {
			list->phy_info[i].phy_port_id = port_mapping.port[i].phy_port_id;
			snprintf(list->phy_info[i].label_name, sizeof(list->phy_info[i].label_name), "%s", 
				port_mapping.port[i].label_name);
			list->phy_info[i].cap = port_mapping.port[i].cap;
			snprintf(list->phy_info[i].cap_name, sizeof(list->phy_info[i].cap_name), "%s", 
				get_phy_port_cap_name(port_mapping.port[i].cap, cap_buf, sizeof(cap_buf)));
			if (pS.link[i] == 1 && !list->status_and_speed_only) {
				// TODO not complete
				//get_port_mib(port_mapping.port[i].phy_port_id, &list->phy_info[i]);
			}

			list->count++;
		}
		len += sprintf(buf+len, "%s=%C;", port_mapping.port[i].label_name,
			conv_speed(pS.link[i], pS.speed[i]));
	}

#else
	for (i = 0; i < NR_WANLAN_PORT; i++) {
		get_vport_info(lan_id_to_vport_nr(i), &pS.link[i], &pS.speed[i], NULL);
	}

	len = 0;
	len += sprintf(buf+len, "W0=%C;", conv_speed(pS.link[WAN_PORT], pS.speed[WAN_PORT]));
	for (i = LAN1_PORT; i < NR_WANLAN_PORT; i++) {
		len += sprintf(buf+len, "L%d=%C;", i, conv_speed(pS.link[i], pS.speed[i]));
	}
#endif

	if (verbose)
		puts(buf);
}

/*
 * int get_wlan_service_status(int bssidx, int vifidx)
 *
 * Get the status of interface that indicate by bssidx and vifidx.
 *
 * return
 * 	-1: invalid
 * 	 0: inactive
 * 	 1: active
 */
int get_wlan_service_status(int bssidx, int vifidx)
{
	int ret;
	char athfix[8],ifname[20];
	char wl_radio[] = "wlXXXX_radio";

	if (nvram_get_int(WLREADY) == 0)
		return -1;

	snprintf(wl_radio, sizeof(wl_radio), "wl%d_radio", bssidx);
	if (nvram_get_int(wl_radio) == 0)
		return -2;

	if(bssidx < 0 || bssidx >= MAX_NR_WL_IF || vifidx < 0 || vifidx >= MAX_NO_MSSID)
		return -1;

	/* CAP: main ap
	 * RE:  sta
	 */
	if (vifidx == 0) {
		if (sw_mode() == SW_MODE_AP && nvram_match("re_mode", "1")) {
			strcpy(athfix, get_staifname(bssidx));
		}
		else
			get_wlxy_ifname(bssidx, 0, athfix);
	}
	/* CAP: guest network/vif
	 * RE:  main ap/guest network/vif
	 */
	else {
		snprintf(ifname, sizeof(ifname), "wl%d.%d_ifname", bssidx, vifidx);
		if (strlen(nvram_safe_get(ifname)))
			strcpy(athfix, nvram_get(ifname));
		else
			return -1;
	}
	//_dprintf("%s: bssidx=%d, vifidx=%d, ifname=%s\n", __func__, bssidx, vifidx, athfix);

	ret = is_intf_up(athfix);
	return ret;
}

void set_wlan_service_status(int bssidx, int vifidx, int enabled)
{
	int r;
	char *ifname = NULL;
	char prefix[] = "wlXXXXXXXXXX_";
	char wl_radio[] = "wlXXXX_radio";
	if (nvram_get_int(WLREADY) == 0)
		return;
	snprintf(wl_radio, sizeof(wl_radio), "wl%d_radio", bssidx);
	if (nvram_get_int(wl_radio) == 0)
		return;

	if (vifidx > 0){
		r = snprintf(prefix, sizeof(prefix), "wl%d.%d_", bssidx, vifidx);
	}else{
		r = snprintf(prefix, sizeof(prefix), "wl%d_", bssidx);
	}
	if(unlikely(r < 0))
		dbg("snprintf failed\n");
	ifname = nvram_pf_safe_get(prefix, "ifname");
	eval("ifconfig", ifname, enabled? "up":"down");
}

/* Helper of __get_lantiq_sta_info_by_ifname()
 * @src:	pointer to WLANCONFIG_LIST
 * @arg:
 * @return:
 * 	0:	success
 *  otherwise:	error
 */
static int handler_lantiq_sta_info(const WLANCONFIG_LIST *src, void *arg)
{
	WIFI_STA_TABLE *sta_info = arg;
	WLANCONFIG_LIST *dst;

	if (!src || !arg)
		return -1;

	dst = &sta_info->Entry[sta_info->Num++];
	*dst = *src;
#if 0
	dbg("[%s][%u][%u][%s][%s][%d][%s][%s]\n", dst->addr, dst->aid, dst->chan,
		dst->txrate, dst->rxrate, dst->rssi, dst->mode, dst->conn_time);
#endif


	return 0;
}

/* Parsing "iw wlan0 station dump" result, fill WLANCONFIG_LIST, and then pass it to @handler() with @arg which is provided by caller.
 * @ifname:	VAP interface name that is used to execute "iw @ifname station dump" command.
 * @subunit_id:	if non-zero, copied to WLANCONFIG_LIST.subunit
 * @handler:	handler function that will be execute for each client.
 * return:
 * 	0:	success
 *  otherwise:	error
 *
 * ILQ1.3.7 Example: iw wlan0 station dump
 * Station 04:ce:14:0a:21:17 (on wlan0)
 *       rx bytes:       0
 *       rx packets:     0
 *       tx bytes:       0
 *       tx packets:     0
 *       tx failed:      0
 *       tx bitrate:     27.5 MBit/s MCS 0
 *       rx bitrate:     27.5 MBit/s MCS 0
 *       connected time: 292 seconds
 * SPF10.0 FC Example: iw wlan0 station dump
 *  Station 04:ce:14:0b:46:12 (on wlan0)
 *        rx bytes:       0
 *        rx packets:     0
 *        tx bytes:       0
 *        tx packets:     0
 *        tx failed:      0
 *        rx drop misc:   0
 *        signal:         -55 dBm
 *        tx bitrate:     27.5 MBit/s MCS 0
 *        rx bitrate:     27.5 MBit/s MCS 0
 */

static int __get_IW_sta_info_by_ifname(const char *ifname, char subunit_id, int (*handler)(const WLANCONFIG_LIST *rptr, void *arg), void *arg)
{
	FILE *fp;
	int c, time_val, hr, min, sec, rssi;
	char rate[6], line_buf[300];
	char cmd[sizeof("iw wlan0 station dump XXXXXX")];
	WLANCONFIG_LIST result, *r = &result;

	if (!ifname || !handler)
		return -1;

	snprintf(cmd, sizeof(cmd), "iw %s station dump", ifname);
	fp = popen(cmd, "r");
	if (!fp)
		return -2;

	while (fgets(line_buf, sizeof(line_buf), fp)) {
		if (strncmp(line_buf, "Station", 7)) {
			continue;
		}

next_sta:
		memset(r, 0, sizeof(*r));
		c = sscanf(line_buf, "Station %17[0-9a-f:] %*[^\n]", r->addr);
		if (c != 1) {
			continue;
		}
		convert_mac_string(r->addr);
		if (subunit_id)
			r->subunit_id = subunit_id;
		strlcpy(r->mode, "11ax", sizeof(r->mode));	/* FIXME */
		while (fgets(line_buf, sizeof(line_buf), fp)) {
			if (!strncmp(line_buf, "Station", 7)) {

#if 0
				dbg("[%s][%u][%u][%s][%s][%u][%s]\n",
					r->addr, r->aid, r->chan, r->txrate, r->rxrate, r->rssi, r->mode);
#endif
				handler(r, arg);
				goto next_sta;
			} else if (strstr(line_buf, "tx bitrate:")) {
				c = sscanf(line_buf, "%*[ \t]tx bitrate:%*[ \t]%6[0-9.]", rate);
				if (c != 1) {
					continue;
				}
				snprintf(r->txrate, sizeof(r->txrate), "%sM", rate);
			} else if (strstr(line_buf, "rx bitrate:")) {
				c = sscanf(line_buf, "%*[ \t]rx bitrate:%*[ \t]%6[0-9.]", rate);
				if (c != 1) {
					continue;
				}
				snprintf(r->rxrate, sizeof(r->rxrate), "%sM", rate);
			} else if (strstr(line_buf, "connected time:")) {
				c = sscanf(line_buf, "%*[ \t]connected time:%*[ \t]%d seconds", &time_val);
				if (c != 1) {
					continue;
				}
				hr = time_val / 3600;
				time_val %= 3600;
				min = time_val / 60;
				sec = time_val % 60;
				snprintf(r->conn_time, sizeof(r->conn_time), "%02d:%02d:%02d", hr, min, sec);
			} else if (strstr(line_buf, "signal:")) {
				c = sscanf(line_buf, "%*[ \t]signal:%*[ \t]%d dBm", &rssi);
				r->rssi = rssi;
			} else {
				//dbg("%s: skip [%s]\n", __func__, line_buf);
			}
		}
	}
	pclose(fp);

	return 0;
}

/* Wrapper function of IW Wireless client list parser.
 * @ifname:	VAP interface name
 * @subunit_id:
 * @sta_info:	pointer to WIFI_STA_TABLE
 * @return:
 * 	0:	success
 *  otherwise:	error
 */
int __get_lantiq_sta_info_by_ifname(const char *ifname, char subunit_id, int (*handler)(const WLANCONFIG_LIST *rptr, void *arg), void *arg)
{
	if (!ifname)
		return -1;
	return __get_IW_sta_info_by_ifname(ifname, subunit_id, handler, arg);
}

/* Wrapper function of LANTIQ Wireless client list parser.
 * @ifname:	VAP interface name
 * @subunit_id:
 * @sta_info:	pointer to WIFI_STA_TABLE
 * @return:
 * 	0:	success
 *  otherwise:	error
 */
int get_lantiq_sta_info_by_ifname(const char *ifname, char subunit_id, WIFI_STA_TABLE *sta_info)
{
	if (!ifname)
		return -1;

	return __get_IW_sta_info_by_ifname(ifname, subunit_id, handler_lantiq_sta_info, sta_info);
}


struct find_vap_by_sta_priv_s {
	char *addr;

	int found;
};

/* Helper of rssi_check_unit()
 * @src:	pointer to WLANCONFIG_LIST
 * @arg:
 * @return:
 * 	0:	success
 *  otherwise:	error
 */
static int handle_find_vap_by_sta(const WLANCONFIG_LIST *src, void *arg)
{
	unsigned char ea1[6] = { 0 }, ea2[6] = { 0 };
	struct find_vap_by_sta_priv_s *priv = arg;

	if (!src || !arg || !priv->addr)
		return -1;
	if (priv->found)
		return 0;

	if (!ether_atoe(priv->addr, ea1) || !ether_atoe(src->addr, ea2) || memcmp(ea1, ea2, sizeof(ea1)))
		return 0;

	priv->found = 1;
	return 0;
}

/* Check whether @sta_addr exist on @vap, if not, find correct VAP and return it by @vap.
 * @sta_addr:
 * @vap:	pointer to char pointer, length IFNAMSIZ.
 * @return:
 *    < 0:	error
 * 	0:	@sta_addr is not found in all VAP at same band
 * 	1:	@sta_addr is found, @vap maybe update
 */
int find_vap_by_sta(char *sta_addr, char *vap)
{
	int band, y, max_subnet;
	char prefix[sizeof("wlX.XXX_")], ifname[IFNAMSIZ];
	struct find_vap_by_sta_priv_s priv;

	if (!sta_addr || *sta_addr == '\0' || !vap || *vap == '\0')
		return -1;

	memset(&priv, 0, sizeof(priv));
	priv.addr = sta_addr;
	__get_lantiq_sta_info_by_ifname(ifname, 0, handle_find_vap_by_sta, &priv);
	if (priv.found)
		return 1;

	band = -1;
	get_wlif_unit(vap, &band, NULL);
	if (band < 0 || band >= MAX_NR_WL_IF)
		return -1;

	/* Find correct VAP for @sta_addr. */
	max_subnet = num_of_mssid_support(band);
	for (y = 0; y < max_subnet; ++y) {
		snprintf(prefix, sizeof(prefix), "wl%d.%d_", band, y);
		if (!nvram_pf_match(prefix, "bss_enabled", "1"))
			continue;

		get_wlxy_ifname(band, y, ifname);
		if (!strcmp(ifname, vap))
			continue;

		memset(&priv, 0, sizeof(priv));
		priv.addr = sta_addr;
		if (!__get_lantiq_sta_info_by_ifname(ifname, 0, handle_find_vap_by_sta, &priv) && priv.found) {
			strlcpy(vap, ifname, IFNAMSIZ);
			break;
		}
	}

	return priv.found? 1 : 0;
}

