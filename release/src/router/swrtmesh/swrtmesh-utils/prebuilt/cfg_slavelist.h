#ifndef __CFG_SLAVELIST_H__
#define __CFG_SLAVELIST_H__

#include <json.h>

#define REPORT_TIME_INTERVAL	30
#define OFFLINE_THRESHOLD	(REPORT_TIME_INTERVAL * 3)

#define CFG_FILE_LOCK		"cfg_mnt"
#define CFG_APPLY_LOCK		"cfg_apply"
#define MAC_LIST_JSON_FILE	"/tmp/maclist.json"
#define ALIAS_LEN			33
#define IP_LEN				4
#define MAC_LEN				6
#define FWVER_LEN			65
#define MODEL_NAME_LEN		33
#define TERRITORY_CODE_LEN	33
#define RE_LIST_JSON_FILE	"/tmp/relist.json"
#ifdef RTCONFIG_MAX_RE
#define MAX_RELIST_COUNT	RTCONFIG_MAX_RE
#else
#define MAX_RELIST_COUNT	MAX_RELIST_NUM
#endif
#define CFG_CLIENT_NUM		(MAX_RELIST_COUNT + 1)
#define SSID_LEN				33
#define LLDP_STAT_LEN       128
#define RE_LIST_MAX_LEN		(MAX_RELIST_COUNT * 128)
enum reListAction {
	RELIST_ADD,
	RELIST_DEL,
	RELIST_UPDATE
};

typedef struct _CM_CLIENT_TABLE {
	char alias[CFG_CLIENT_NUM][ALIAS_LEN];
	unsigned char ipAddr[CFG_CLIENT_NUM][IP_LEN];
	unsigned char macAddr[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char realMacAddr[CFG_CLIENT_NUM][MAC_LEN];
	time_t reportStartTime[CFG_CLIENT_NUM];
	unsigned char pap2g[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char pap5g[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char pap6g[CFG_CLIENT_NUM][MAC_LEN];
	char pap2g_ssid[CFG_CLIENT_NUM][SSID_LEN];
	char pap5g_ssid[CFG_CLIENT_NUM][SSID_LEN];
	char pap6g_ssid[CFG_CLIENT_NUM][SSID_LEN];
	int rssi2g[CFG_CLIENT_NUM];
	int rssi5g[CFG_CLIENT_NUM];
	int rssi6g[CFG_CLIENT_NUM];
	unsigned char sta2g[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char sta5g[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char sta6g[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap2g[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap5g[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap5g1[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap6g[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap6g1[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap2g_fh[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap5g_fh[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap5g1_fh[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap6g_fh[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap6g1_fh[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap2g_iot_fh[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap5g_iot_fh[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap5g1_iot_fh[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap6g_iot_fh[CFG_CLIENT_NUM][MAC_LEN];
	unsigned char ap6g1_iot_fh[CFG_CLIENT_NUM][MAC_LEN];
	char ap2g_ssid[CFG_CLIENT_NUM][SSID_LEN];
	char ap5g_ssid[CFG_CLIENT_NUM][SSID_LEN];
	char ap5g1_ssid[CFG_CLIENT_NUM][SSID_LEN];
	char ap6g_ssid[CFG_CLIENT_NUM][SSID_LEN];
	char ap6g1_ssid[CFG_CLIENT_NUM][SSID_LEN];
	char ap2g_ssid_fh[CFG_CLIENT_NUM][SSID_LEN];
	char ap5g_ssid_fh[CFG_CLIENT_NUM][SSID_LEN];
	char ap5g1_ssid_fh[CFG_CLIENT_NUM][SSID_LEN];
	char ap6g_ssid_fh[CFG_CLIENT_NUM][SSID_LEN];
	char ap6g1_ssid_fh[CFG_CLIENT_NUM][SSID_LEN];
	int level[CFG_CLIENT_NUM];
	char fwVer[CFG_CLIENT_NUM][FWVER_LEN];
	char newFwVer[CFG_CLIENT_NUM][FWVER_LEN];
	char modelName[CFG_CLIENT_NUM][MODEL_NAME_LEN];
	char productId[CFG_CLIENT_NUM][MODEL_NAME_LEN];
	char territoryCode[CFG_CLIENT_NUM][TERRITORY_CODE_LEN];
	int activePath[CFG_CLIENT_NUM];
	int bandnum[CFG_CLIENT_NUM];
	int online[CFG_CLIENT_NUM];
	int maxLevel;
	int count;
#ifdef RTCONFIG_FRONTHAUL_DWB
	int BackhualStatus[CFG_CLIENT_NUM]; // bits 0(update or not) 0(reserved) 0(reserved) 0(used or not)
#endif
#ifdef RTCONFIG_BHCOST_OPT
	unsigned int joinTime[CFG_CLIENT_NUM];
#endif
} CM_CLIENT_TABLE, *P_CM_CLIENT_TABLE;

#endif /* __CFG_SLAVELIST_H__ */
/* End of cfg_slavelist.h */
