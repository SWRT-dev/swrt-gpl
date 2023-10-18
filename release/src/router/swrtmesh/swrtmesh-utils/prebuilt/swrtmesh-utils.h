/*
** swrtmesh-utils.h
**
**
*/
#ifndef __SWRTMESHUTILSH__
#define __SWRTMESHUTILSH__
#define TYPEDEF_BOOL
#include <uci.h>
#undef TYPEDEF_BOOL

#define SWRTMESH_FUNC extern

#define SWRTMESHUTILS_DEBUG             "/tmp/SWRTMESHUTILS_DEBUG"
#define SWRTMESHUTILS_DEBUG_FILE                  "/tmp/SWRTMESHUTILS_DEBUG.log"
#define SWRTMESHUTILS_DBG(fmt, args...) ({ \
	if (f_exists(SWRTMESHUTILS_DEBUG) > 0){ \
		char info[1080]; \
        char msg[1024]; \
		snprintf(msg, sizeof(msg), "[%s:(%d)]: "fmt, __FUNCTION__, __LINE__, ##args); \
		fprintf(stderr, "%s", msg); \
		snprintf(info, sizeof(info), "echo \"%s\" >> %s", msg, SWRTMESHUTILS_DEBUG_FILE); \
		system(info); \
	} \
})
extern struct json_object *json_msg;
////////////////////////////////////////////////////////////////////////////////
//
//	EXTERNAL FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////
SWRTMESH_FUNC int type_to_multi_ap(const char *type);
SWRTMESH_FUNC struct uci_package *swrtmesh_uci_load_pkg(struct uci_context **ctx, const char *config);
SWRTMESH_FUNC int swrtmesh_set_value(struct uci_context *ctx, struct uci_package *pkg, struct uci_section *section, const char *key, const char *value, enum uci_option_type type);
SWRTMESH_FUNC int swrtmesh_set_value_by_string(const char *package, const char *section, const char *key, const char *value, enum uci_option_type type);
SWRTMESH_FUNC int swrtmesh_get_value_by_string(const char *package, const char *section, const char *key, char *value, size_t len);
SWRTMESH_FUNC int swrtmesh_get_band_value_by_string(const char *package, const char *section, const char *band, const char *key, char *value, size_t len);
SWRTMESH_FUNC bool swrtmesh_uci_set_option(char *package_name, char *section_type, char *search_key, char *search_val, char *option, char *value);
SWRTMESH_FUNC struct uci_section *swrtmesh_config_get_section(struct uci_context *ctx, struct uci_package *pkg, const char *type, const char *key, const char *value);
SWRTMESH_FUNC void swrtmesh_uci_add_option(struct uci_context *ctx, struct uci_package *p, struct uci_section *s, const char *option, void *value, bool is_list);
SWRTMESH_FUNC void swrtmesh_uci_del_option(struct uci_context *ctx, struct uci_package *p, struct uci_section *s, const char *option);
SWRTMESH_FUNC int swrtmesh_lookup_option_match(struct uci_context *ctx, struct uci_section *s, char *option, char *value);
SWRTMESH_FUNC int swrtmesh_generate_controller_config(void);
SWRTMESH_FUNC int swrtmesh_generate_agent_config(void);
SWRTMESH_FUNC int swrtmesh_generate_wireless_config(void);
SWRTMESH_FUNC int swrtmesh_generate_ieee1905_config(void);
SWRTMESH_FUNC int swrtmesh_generate_topology_config(void);
SWRTMESH_FUNC int swrtmesh_generate_hosts_config(void);
SWRTMESH_FUNC int swrtmesh_ubus_call(char *path, char *method, char *message);
SWRTMESH_FUNC int swrtmesh_ubus_list(const char *path);
SWRTMESH_FUNC char *swrtmesh_utils_version_text(void);
SWRTMESH_FUNC void swrtmesh_utils_set_debug(unsigned int enable);
SWRTMESH_FUNC int swrtmesh_get_cost(char *ifname, int bandindex, int capability5g, char *ifmac, int *cost);
SWRTMESH_FUNC int swrtmesh_is_plc_head(char *ifname, int *is_plc_head);
SWRTMESH_FUNC int swrtmesh_find_mac_role(char *ifname, char *mac, int *role);
SWRTMESH_FUNC int swrtmesh_find_role_lan(char *ifname, char *mac);
SWRTMESH_FUNC int swrtmesh_set_cost(int cost);
SWRTMESH_FUNC int swrtmesh_set_obstatus(int status);
SWRTMESH_FUNC int swrtmesh_set_peermac(unsigned char *macaddr);
SWRTMESH_FUNC int swrtmesh_get_peermac(unsigned char *macaddr);
SWRTMESH_FUNC int swrtmesh_get_newremac(unsigned char *macaddr);
SWRTMESH_FUNC int swrtmesh_set_secstatus(int status);
SWRTMESH_FUNC int swrtmesh_set_sessionkey(unsigned char *sessionkey);
//SWRTMESH_FUNC int swrtmesh_get_sessionkey(unsigned char *sessionkey, int *len);
SWRTMESH_FUNC int swrtmesh_set_wifisec(char *type, unsigned char *value);
SWRTMESH_FUNC int swrtmesh_get_wifisec(char *type, unsigned char *value, int *len);
SWRTMESH_FUNC int swrtmesh_set_group(unsigned char *group);
SWRTMESH_FUNC int swrtmesh_get_group(unsigned char *group, int *len);
SWRTMESH_FUNC int swrtmesh_get_rssi_score(char *ifname, int bandindex, int capability5g, char *ifmac, int *rssi_score);
SWRTMESH_FUNC int swrtmesh_set_rssi_score(int rssi_score);
SWRTMESH_FUNC int swrtmesh_set_timeout(int rtime, int ctimeout, int ttimeout);
SWRTMESH_FUNC int swrtmesh_get_wifi_lastbyte(char *ifname, int bandindex, int capability5g, char *ifmac, char *wifi_lastbyte, int wifi_lastbyte_len);
SWRTMESH_FUNC int swrtmesh_set_wifi_lastbyte(unsigned char *input_wifi_lastbyte);
SWRTMESH_FUNC int swrtmesh_gen_hash_bundle_key(unsigned char *key);
SWRTMESH_FUNC int swrtmesh_verify_hash_bundle_key(unsigned char *key, int *result);
SWRTMESH_FUNC int swrtmesh_set_hash_bundle_key(int reset);
SWRTMESH_FUNC int swrtmesh_gen_default_backhaul_security(char *ssid, int ssid_len, char *psk, int psk_len);
SWRTMESH_FUNC int swrtmesh_verify_default_backhaul_security(char *ssid, char *psk, int *result);
SWRTMESH_FUNC int swrtmesh_get_default_hash_bundle_key(unsigned char *hash_key, int hash_key_len);
SWRTMESH_FUNC int swrtmesh_prelink_band_sync_bypass(int unit, int *result);
SWRTMESH_FUNC int swrtmesh_gen_onboarding_vif_security(char *ssid, int ssid_len, char *psk, int psk_len);
SWRTMESH_FUNC int swrtmesh_set_eth_role(char *input_eth_role);
SWRTMESH_FUNC int swrtmesh_get_dest_eth_role(char *ifname, int *eth_role);
SWRTMESH_FUNC int swrtmesh_set_misc_info(int index, char *value);
SWRTMESH_FUNC int swrtmesh_get_misc_info(unsigned char *misc_info, int *misc_info_len);
#endif /* !__AMASUTILSH__ */
