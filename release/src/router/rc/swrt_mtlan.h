#ifndef __SWRT_WLLIBH__
#define __SWRT_WLLIBH__
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bcmnvram.h>
#include <shared.h>
#include <mtlan_utils.h>

#define APG_IFNAMES_USED_FILE "/jffs/.sys/apg_ifnames_used.json"

typedef struct __apg_dut_list_t__
{
	char dev_mac[18];
	int wifiband_id;
	char lanport_id[32];
} APG_DUT_LIST_T;
extern int get_vlanid_by_idx(int idx, VLAN_T *lst, size_t lst_sz);
extern int get_vid_from_vlan_rl(int  unit);
extern APG_DUT_LIST_T *get_apg_dut_list_info(APG_DUT_LIST_T *vlst, size_t *sz,int unit);
extern int get_apg_wifiband(char *mac, int unit);
extern char *get_apg_lanport(char *mac, int unit);
extern char *match_wifiband_from_dutlist(char *mac, int unit, char *ret_buf, size_t ret_bsize);
extern char *match_lanports_from_dutlist(char *mac, json_object *capabilityObj, int unit, char *ret_buf, size_t ret_bsize);
extern char *create_vlan_trunk_rulelist(char *mac, json_object *capabilityObj, char *buffer, int bsize);
extern int check_apg_vif_type(json_object *wifiBandCapObj, unsigned short band, int subunit, unsigned long type);
extern unsigned long get_apg_vif_type(json_object *wifiBandCapObj, unsigned short band, int subunit);
extern int get_apg_vif_status(json_object *wifiBandCapObj, unsigned short wifi_band, int subunit, unsigned long *ret_status_bits);
extern json_object* gen_wifi_band_cap(json_object *root);
extern json_object* gen_wan_port_cap(json_object *root);
extern json_object* gen_lan_port_cap(json_object *root);
extern char* get_wificap_ifname_from_wlprefix(char *dut_mac, char *wlprefix, char *ret_ifname, size_t ifname_bsize);
extern char* get_lancap_ifname_from_portindex(char *dut_mac, int port_index, char *ret_ifname, size_t ifname_bsize);
extern int get_sdn_vid_by_ifname_used(char *dut_mac, char *ifname);
extern int num_of_no_used_vif_from_wificap(char *dut_mac, unsigned short wifi_band);
extern int num_of_vif_used(char *dut_mac, unsigned short wifi_band);
extern int num_of_wifi_band(char *dut_mac, unsigned short wifi_band);
extern char *get_used_vid_by_dut_mac(char *dut_mac, char *ret_buffer, size_t buffer_size);
extern char* get_ifnames_used_by_sdn_vid(char *dut_mac, int sdn_vid, int sdn_idx, char *ret_ifnames, size_t ifnames_bsize);
extern char* get_wificap_ifnames(char *dut_mac, unsigned short wifi_band, unsigned long type, char *ret_ifnames, size_t ifnames_bsize, int get_prefix);
extern int sync_apgx_to_wlunit(void);
extern char* get_unused_ifname_by_dut(char *dut_mac, unsigned short wifi_band, char *ret_ifname, size_t ret_ifname_bsize);
extern unsigned short get_wifi_band_by_ifname(char *dut_mac, char *ifname);
extern char* get_ifname_used_by_band(char *dut_mac, int sdn_vid, int sdn_idx, unsigned short wifi_band, char *ret_ifname, size_t ret_ifname_bsize); 
extern void check_apg_ifnames_used_dir(char *path);
extern int get_rm_sdn_idx(const unsigned int apg_idx);
extern int get_sdn_idx(const unsigned int apg_idx);
extern char* get_wificap_all_ifnames(char *dut_mac, unsigned long type, char *ret_ifnames, size_t ifnames_bsize, int get_prefix);
extern int is_sdn_supported(char *mac);
extern char* get_wgn_ifnames(int band, int total, char *ret_ifnames, size_t ret_bsize);
extern unsigned short get_wifi_band_for_wgn(char *dut_mac);
extern int is_wgn_auth_mode_supported(char *auth_mode);
extern char* create_wgn_vlan_rl(char *mac, char *ret_buf, size_t ret_bsize);
extern char* create_ap_wifi_rl(char *mac, char *ret_buf, size_t ret_bsize);
extern char* create_ap_lanif_rl(char *mac, json_object *capabilityObj, char *ret_buf, size_t ret_bsize);
extern char* max_of_mssid_ifnames(int unit, char *ret_ifnames, int buffer_size);
extern int update_apg_ifnames_used(int action, char *dut_mac, int sdn_vid, int sdn_idx, unsigned short wifi_band, char *ifname);
#endif
