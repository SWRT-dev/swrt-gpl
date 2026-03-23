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
#include <fapi_wlan_private.h>
#include <fapi_wlan.h>
#include <help_objlist.h>
#include <wlan_config_api.h>

#define MAX_FRW 64
#define MACSIZE 12
#define	DEFAULT_SSID_2G	"ASUS" // asus app relies on this to identify
#define	DEFAULT_SSID_5G	"ASUS_5G"

#define APSCAN_WLIST	"/tmp/apscan_wlist"

#define MKNOD(name,mode,dev)		if (mknod(name,mode,dev))		perror("## mknod " name)
const int pre_rssi[] = {-91, -91, -91};

struct config_s {
	char *key;
	char *value;
};

const struct config_s config_in_SSID[] = {
	{"Enable", "true"},
	{"Status", "Down"},
	{"Alias", "cpe-SSID-4"},
	{"Name", ""},
	{"LastChange", "0"},
	{"LowerLayers", "Device.WiFi.Radio.1."},
	{"BSSID", ""},
	{"MACAddress", ""},
	{"SSID", ""},
	{"X_LANTIQ_COM_Vendor_BridgeName", "br-lan"},
	{"X_LANTIQ_COM_Vendor_SsidType", "EndPoint"},
	{"X_LANTIQ_COM_Vendor_IsEndPoint", "true"},
	{"X_LANTIQ_COM_Vendor_WaveAtfVapWeight", "0"},
	{NULL, NULL}
};

const struct config_s config_in_EndPoint[] = {
	{"Enable", "false"},
	{"Status", "Disabled"},
	{"Alias", "cpe-EndPoint-1"},
	{"ProfileReference", ""},
	{"SSIDReference", "Device.WiFi.SSID.4"},
	{"ProfileNumberOfEntries", "0"},
	{"X_LANTIQ_COM_Vendor_ScanStatus", ""},
	{"X_LANTIQ_COM_Vendor_ConnectionStatus", "Disconnected"},
	{"X_LANTIQ_COM_Vendor_WaveEndPointWDS", "false"},
	{"X_LANTIQ_COM_Vendor_WaveEndPointPMF", "0"},
	{"X_LANTIQ_COM_Vendor_WispEnable", "false"},
	{NULL, NULL}
};

const struct config_s config_in_EndPoint_WPS[] = {
	{"Enable", "true"},
	{"ConfigMethodsSupported", "PushButton,PIN"},
	{"ConfigMethodsEnabled", "PushButton,PIN"},
	{"X_LANTIQ_COM_Vendor_WPSStatus", "Idle"},
	{"X_LANTIQ_COM_Vendor_WPSAction", ""},
	{"X_LANTIQ_COM_Vendor_EndpointPIN", "12345670"},
	{NULL, NULL}
};

const struct config_s config_in_EndPoint_Security[] = {
	{"ModesSupported", "None,WEP-64,WEP-128,WPA2-Personal,WPA-WPA2-Personal,WPA2-Enterprise,WPA-WPA2-Enterprise"},
	{NULL, NULL}
};

const struct config_s config_in_EndPoint_AC_BE[] = {
	{"AccessCategory", "BE"},
	{"Alias", "cpe-AC-1"},
	{"AIFSN", "3"},
	{"ECWMin", "4"},
	{"ECWMax", "10"},
	{"TxOpMax", "0"},
	{"AckPolicy", "false"},
	{"OutQLenHistogramIntervals", ""},
	{"OutQLenHistogramSampleInterval", "0"},
	{NULL, NULL}
};

const struct config_s config_in_EndPoint_AC_BK[] = {
	{"AccessCategory", "BK"},
	{"Alias", "cpe-AC-2"},
	{"AIFSN", "7"},
	{"ECWMin", "4"},
	{"ECWMax", "10"},
	{"TxOpMax", "0"},
	{"AckPolicy", "false"},
	{"OutQLenHistogramIntervals", ""},
	{"OutQLenHistogramSampleInterval", "0"},
	{NULL, NULL}
};

const struct config_s config_in_EndPoint_AC_VI[] = {
	{"AccessCategory", "VI"},
	{"Alias", "cpe-AC-3"},
	{"AIFSN", "2"},
	{"ECWMin", "3"},
	{"ECWMax", "4"},
	{"TxOpMax", "94"},
	{"AckPolicy", "false"},
	{"OutQLenHistogramIntervals", ""},
	{"OutQLenHistogramSampleInterval", "0"},
	{NULL, NULL}
};

const struct config_s config_in_EndPoint_AC_VO[] = {
	{"AccessCategory", "VO"},
	{"Alias", "cpe-AC-4"},
	{"AIFSN", "2"},
	{"ECWMin", "2"},
	{"ECWMax", "3"},
	{"TxOpMax", "47"},
	{"AckPolicy", "false"},
	{"OutQLenHistogramIntervals", ""},
	{"OutQLenHistogramSampleInterval", "0"},
	{NULL, NULL}
};

int wav_ep_up(int band)
{
	return fapi_wlan_up(get_staifname(band), NULL, 1);
}

int wav_ep_start(int unit)
{
	int ret;
	char buf[32];
	char hwaddr[] = "00:11:22:33:44:55";
	ObjList *dbObjPtr = NULL;
	struct config_s *list = NULL;

	dbObjPtr = HELP_CREATE_OBJ(SOPT_OBJVALUE);
	help_addObjList(dbObjPtr, "Device.WiFi.SSID", 0, 0, 0, 0);
	for(list = &config_in_SSID[0]; list->key; list++)
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.SSID", list->key, list->value, 0, 0);

	snprintf(buf, sizeof(buf), "wl%d_hwaddr", unit);
	snprintf(hwaddr, sizeof(hwaddr), "%s", nvram_safe_get(buf));
	inc_mac(hwaddr, 1);
	HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.SSID", "MACAddress", hwaddr, 0, 0);
	HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.SSID", "Name", get_staifname(unit), 0, 0);
	printf("%s(%d) fapi_wlan_ssid_add: %d(%s)\n", __func__, __LINE__, unit, get_wififname(unit));
	ret = fapi_wlan_ssid_add(get_wififname(unit), dbObjPtr, 0);
	printf("%s(%d) fapi_wlan_ssid_add: %d\n", __func__, __LINE__, ret);
	HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, FREE_OBJLIST);
	dbObjPtr = HELP_CREATE_OBJ(SOPT_OBJVALUE);
	help_addObjList(dbObjPtr, "Device.WiFi.EndPoint", 0, 0, 0, 0);
	for(list = &config_in_EndPoint[0]; list->key; list++)
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint", list->key, list->value, 0, 0);
	help_addObjList(dbObjPtr, "Device.WiFi.EndPoint.Security", 0, 0, 0, 0);
	list = &config_in_EndPoint_Security[0];
	HELP_EDIT_NODE(v15, "Device.WiFi.EndPoint.Security", list->key, list->value, 0, 0);
	help_addObjList(dbObjPtr, "Device.WiFi.EndPoint.WPS", 0, 0, 0, 0);
	for(list = &config_in_EndPoint_WPS[0]; list->key; list++)
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.WPS", list->key, list->value, 0, 0);
	help_addObjList(dbObjPtr, "Device.WiFi.EndPoint.AC.1", 0, 0, 0, 0);
	for(list = &config_in_EndPoint_AC_BE[0]; list->key; list++)
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.AC.1", list->key, list->value, 0, 0);
	help_addObjList(dbObjPtr, "Device.WiFi.EndPoint.AC.2", 0, 0, 0, 0);
	for(list = &config_in_EndPoint_AC_BK[0]; list->key; list++)
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.AC.2", list->key, list->value, 0, 0);
	help_addObjList(dbObjPtr, "Device.WiFi.EndPoint.AC.3", 0, 0, 0, 0);
	for(list = &config_in_EndPoint_AC_VI[0]; list->key; list++)
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.AC.3", list->key, list->value, 0, 0);
 	help_addObjList(dbObjPtr, "Device.WiFi.EndPoint.AC.4", 0, 0, 0, 0);
	for(list = &config_in_EndPoint_AC_VO[0]; list->key; list++)
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.AC.4", list->key, list->value, 0, 0);
	if(strstr(dbObjPtr->sObjName, "AC"))
		strcpy(dbObjPtr->sObjName, "Device.WiFi.EndPoint.AC");

	printf("%s(%d) wifi_unit: %d\n", __func__, __LINE__, unit);
	fapi_wlan_endpoint_set(get_staifname(unit), dbObjPtr, 0);
	printf("%s(%d) wifi_unit: %d\n", __func__, __LINE__, unit);
	HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, FREE_OBJLIST);
	dbObjPtr = HELP_CREATE_OBJ(SOPT_OBJVALUE);
 	help_addObjList(dbObjPtr, "Device.WiFi.EndPoint", 0, 0, 0, 0);
	HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint", "Enable", "true", 0, 0);
	HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint", "X_LANTIQ_COM_Vendor_WaveEndPointWDS", "false", 0, 0);
	ret = fapi_wlan_endpoint_set(get_staifname(unit), dbObjPtr, 0);
	HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, FREE_OBJLIST);
	dbObjPtr = HELP_CREATE_OBJ(SOPT_OBJVALUE);
 	help_addObjList(dbObjPtr, "Device.WiFi.SSID", 0, 0, 0, 0);
	HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.SSID", "Enable", "true", 0, 0);
	HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.SSID", "X_LANTIQ_COM_Vendor_BridgeName", "br-lan", 0, 0);
 	ret += fapi_wlan_ssid_set(get_staifname(unit), dbObjPtr, 0);
	HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, FREE_OBJLIST);
	return ret;
}

char *wlc_nvname(char *keyword)
{
	return wl_nvname(keyword, nvram_get_int("wlc_band"), -1);
}

int wav_ep_down(int unit, unsigned int flags)
{
  	return fapi_wlan_down(get_staifname(unit), NULL, flags);
}

static int wav_ep_scan(int band, ObjList *dbObjPtr)
{
	printf("%s(%d) wifi_unit: %d\n", __func__, __LINE__, band);
    fapi_wlan_endpoint_set(get_staifname(band), dbObjPtr, 0);
	printf("%s(%d) wifi_unit: %d\n", __func__, __LINE__, band);
}

int wlan_setEndpointEnabled(int band)
{
	int ret;
	ObjList *dbObjPtr = NULL;
	if (fapiWlanFailSafeLoad())
	{
		printf("%s fapiWlanFailSafeLoad failure\n", __func__);
    	return -1;
	}
	dbObjPtr = HELP_CREATE_OBJ(SOPT_OBJVALUE);
	help_addObjList(dbObjPtr, "Device.WiFi.EndPoint", 0, 0, 0, 0);
	HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint", "ProfileReference", "Device.WiFi.EndPoint.1.Profile.1", 0, 0);
	ret = fapi_wlan_endpoint_set(get_staifname(band), dbObjPtr, 0);
	HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, EMPTY_OBJLIST);
	if(ret)
	{
		ret = wav_ep_start(band);
		ret += wav_ep_up(band);
	}
	fapiWlanFailSafeStore();
   	return ret;
}

int wlan_setEndpointConnect(int band, char *mac, char *ssid, char *auth, char *beacon, char *encrypt, char *key)
{
	int load = fapiWlanFailSafeLoad();
	int ret;
	char prefix[] = "wlXXXXXXXXXXXXX_", tmp[128];
	ObjList *dbObjPtr = NULL;

	if(!load)
	{
		wav_ep_start(band);
 		dbObjPtr = HELP_CREATE_OBJ(SOPT_OBJVALUE);
		help_addObjList(dbObjPtr, "Device.WiFi.EndPoint", 0, 0, 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint", "ProfileReference", "Device.WiFi.EndPoint.1.Profile.1", 0, 0);
		ret = fapi_wlan_endpoint_set(get_staifname(band), dbObjPtr, 0);
		HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, EMPTY_OBJLIST);
		if(ret){
			fapiWlanFailSafeStore();
			return load;
		}
		help_addObjList(dbObjPtr, "Device.WiFi.EndPoint.Profile", 0, 0, 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile", "Enable", "true", 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile", "Status", "Active", 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile", "Priority", "0", 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile", "Alias", "CPE-Profile-1", 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile", "X_LANTIQ_COM_Vendor_IsHiddenSsid", "true", 0, 0);
		if(mac && *mac )
			HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile", "X_LANTIQ_COM_Vendor_BSSID", mac, 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile", "SSID", ssid, 0, 0);
		fapi_wlan_endpoint_set(get_staifname(band), dbObjPtr, 0);
		HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, EMPTY_OBJLIST);
		help_addObjList(dbObjPtr, "Device.WiFi.EndPoint.Profile.Security", 0, 0, 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile.Security", "ModeEnabled", auth, 0, 0);

		if(strcmp(auth, "WEP-64") && strcmp(auth, "WEP-128"))
			HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile.Security", "KeyPassphrase", key, 0, 0);
		else
			HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile.Security", "WEPKey", key, 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile.Security", "BeaconType", beacon, 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile.Security", "EncryptionMode", encrypt, 0, 0);
		if(!strcmp(auth, "WPA2-Enterprise") || !strcmp(auth, "WPA-WPA2-Enterprise"))
		{
			wl_nvprefix(prefix, sizeof(prefix), band, -1);
			HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile.Security", "RadiusServerIPAddr", nvram_safe_get(strcat_r(prefix, "radius_ipaddr", tmp)), 0, 0);
			HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile.Security", "RadiusServerPort", nvram_safe_get(strcat_r(prefix, "radius_port", tmp)), 0, 0);
			HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.Profile.Security", "RadiusSecret", nvram_safe_get(strcat_r(prefix, "radius_key", tmp)), 0, 0);
		}
		fapi_wlan_endpoint_set(get_staifname(band), dbObjPtr, 0);
		HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, EMPTY_OBJLIST);
		wav_ep_up(band);
		fapiWlanFailSafeStore();
		return load;
	}
	printf("%s fapiWlanFailSafeLoad failure\n", __func__);
	return -1;
}

int wlan_setEndpointScan(int band)
{
	int load = 0;
	ObjList *dbObjPtr = NULL;

	load = fapiWlanFailSafeLoad();
	if(load)
	{
    	printf("%s fapiWlanFailSafeLoad failure\n", __func__);
    	return -1;
	}
	else
	{
		wav_ep_start(band);
 		dbObjPtr = HELP_CREATE_OBJ(SOPT_OBJVALUE);
		help_addObjList(dbObjPtr, "Device.WiFi.EndPoint", 0, 0, 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint", "X_LANTIQ_COM_Vendor_ScanStatus", "Scanning", 0, 0);
		wav_ep_scan(band, dbObjPtr);
		HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, FREE_OBJLIST);
		wav_ep_up(band);
		fapiWlanFailSafeStore();
	}
	return v2;
}

int wlan_setEndpointWpsPbcTrigger(int band)
{
	int load = 0;
	char *sta_if = NULL;
	ObjList *dbObjPtr = NULL;

	load = fapiWlanFailSafeLoad();
	if(!load)
	{
		dbObjPtr = HELP_CREATE_OBJ(SOPT_OBJVALUE);
		help_addObjListt(dbObjPtr, "Device.WiFi.EndPoint.WPS", 0, 0, 0, 0);
		HELP_EDIT_NODE(dbObjPtr, "Device.WiFi.EndPoint.WPS", "X_LANTIQ_COM_Vendor_WPSAction", "PBC", 0, 0);
		if(band)
		{
			if(band > 1)
			{
				HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, FREE_OBJLIST);
				fapiWlanFailSafeStore();
				return load;
			}
			sta_if = "wlan3";
		}
		else
			sta_if = "wlan1";
		fapi_wlan_endpoint_wps_set(sta_if, dbObjPtr, 0);
		HELP_DELETE_OBJ(dbObjPtr, SOPT_OBJVALUE, FREE_OBJLIST);
		fapiWlanFailSafeStore();
		return load;
	}
	printf("%s fapiWlanFailSafeLoad failure\n", __func__);
	return -1;
}

const char *wav_get_security_str(const char *auth_mode, const char *crypto, int wep_type)
{
	if(!strcmp(auth_mode, "open"))
	{
		if(wep_type == 2)
			return "WEP-128";
		if(wep_type == 1)
			return "WEP-64";
	}else if(!strcmp(auth_mode, "psk2")){
		if(!strcmp(crypto, "aes"))
			return "WPA2-Personal";
	}else if(!strcmp(auth_mode, "pskpsk2")){
		if(!strcmp(crypto, "aes") || strcmp(crypto, "tkip+aes"))
			return "WPA-WPA2-Personal";
	}else if(!strcmp(auth_mode, "sae")){
		if(!strcmp(crypto, "aes"))
			return "WPA3-Personal";
	}else if(!strcmp(auth_mode, "psk2sae")){
		if(!strcmp(crypto, "aes"))
			return "WPA2-WPA3-Personal";
	}else if(!strcmp(auth_mode, "psk")){
		if(!strcmp(crypto, "tkip"))
			return "WPA-Personal";
	}else if(!strcmp(auth_mode, "wpa2")){
		if(!strcmp(crypto, "aes"))
			return "WPA2-Enterprise";
	}else if(!strcmp(auth_mode, "wpa3")){
		if(!strcmp(crypto, "aes"))
			return "WPA3-Enterprise";
	}else if(!strcmp(auth_mode, "wpawpa2")){
		if(!strcmp(crypto, "aes") || strcmp(crypto, "tkip+aes"))
			return "WPA-WPA2-Enterprise";
	}else
		return "";

	return "None";
}

const char *wav_get_beacon_type(const char *word)
{
	if(!strcmp(word, "tkip"))
		return "WPA";
	if(!strcmp(word, "aes"))
		return "11i";
	if(!strcmp(word, "tkip+aes"))
		return "WPAand11i";
	return "None";
}

const char *wav_get_encrypt(const char *word)
{
	if(!strcmp(word, "tkip"))
		return "ENC_TKIP";
	if(!strcmp(word, "aes"))
		return "ENC_AES";
	if(!strcmp(worda1, "tkip+aes"))
		return "ENC_TKIP_AND_AES";
	return "None";
}

int start_repeater(void)
{
	int wlc_band = nvram_get_int("wlc_band");
	int wlc_wep = nvram_get_int("wlc_wep");
	char wlc_ap_mac[20], wlc_ssid[40], wlc_auth_mode[40], wlc_crypto[40], wlc_key[40];
	char wlc_auth[40], wlc_beacon[40], wlc_encrypt[40];

	memset(wlc_ap_mac, 0, sizeof(wlc_ap_mac));
	memset(wlc_ssid, 0, sizeof(wlc_ssid));
	memset(wlc_auth_mode, 0, sizeof(wlc_auth_mode));
	memset(wlc_crypto, 0, sizeof(wlc_crypto));
	memset(wlc_key, 0, sizeof(wlc_key));
	memset(wlc_auth, 0, sizeof(wlc_auth));
	memset(wlc_beacon, 0, sizeof(wlc_beacon));
	memset(wlc_encrypt, 0, sizeof(wlc_encrypt));
	snprintf(wlc_ap_mac, sizeof(wlc_ap_mac), "%s", nvram_safe_get("wlc_ap_mac"));
	snprintf(wlc_ssid, sizeof(wlc_ssid), "%s", nvram_safe_get("wlc_ssid"));
	snprintf(wlc_auth_mode, sizeof(wlc_auth_mode), "%s", nvram_safe_get("wlc_auth_mode"));
	snprintf(wlc_crypto, sizeof(wlc_crypto), "%s", nvram_safe_get("wlc_crypto"));
	if(!strcmp(wlc_auth_mode, "open") && wlc_wep > 0)
		snprintf(wlc_key, sizeof(wlc_key), "%s", nvram_safe_get("wlc_wep_key"));
	else
		snprintf(wlc_key, sizeof(wlc_key), "%s", nvram_safe_get("wlc_wpa_psk"));
	if(wlc_ssid[0] == 0x0){
		_dprintf("%s: no ap_mac or ap_ssid.\n", __func__);
		return -1;
	}
	snprintf(wlc_auth, sizeof(wlc_auth), "%s", wav_get_security_str(wlc_auth_mode, wlc_crypto, wlc_wep));
	snprintf(wlc_beacon, sizeof(wlc_beacon), "%s", wav_get_beacon_type(wlc_crypto));
	snprintf(wlc_encrypt, sizeof(wlc_encrypt), "%s", wav_get_encrypt(wlc_crypto));
	if(wlc_auth[0] == 0x0){
		_dprintf("[warning] not support this auth. mode\n");
		return -1;
	}
	_dprintf("%s: ap_mac=%s, ap_ssid=%s, ap_security=%s, ap_key=%s.\n", __func__, wlc_ap_mac, wlc_ssid, wlc_auth, wlc_key);
	_dprintf("%s: ap_beacon=%s, ap_encrypt=%s.\n", __func__, wlc_beacon, wlc_encrypt);
	wlan_setEndpointConnect(wlc_band, wlc_ap_mac, wlc_ssid, wlc_auth, wlc_beacon, wlc_encrypt, wlc_key);
	return 0;
}

static int atoh(const char *a, unsigned char *e)
{
	char *c = (char *)a;
	int i = 0;

	memset(e, 0, MAX_FRW);
	for (i = 0; i < MAX_FRW; ++i, c += 3) {
		if (!isxdigit(*c) || !isxdigit(*(c + 1)) || isxdigit(*(c + 2)))	// should be "AA:BB:CC:DD:..."
			break;
		e[i] = (unsigned char)nibble_hex(c);
	}

	return i;
}

char * htoa(const unsigned char *e, char *a, int len)
{
	char *c = a;
	int i;

	for (i = 0; i < len; i++) {
		if (i)
			*c++ = ':';
		c += sprintf(c, "%02X", e[i] & 0xff);
	}
	return a;
}

int FREAD(unsigned int addr_sa, int len)
{
	unsigned char buffer[MAX_FRW];
	char buffer_h[128];
	memset(buffer, 0, sizeof(buffer));
	memset(buffer_h, 0, sizeof(buffer_h));

	if (FRead(buffer, addr_sa, len) < 0)
		dbg("FREAD: Out of scope\n");
	else {
		if (len > MAX_FRW)
			len = MAX_FRW;
		htoa(buffer, buffer_h, len);
		puts(buffer_h);
	}
	return 0;
}

/*
 * 	write str_hex to offset da
 *	console input:	FWRITE 0x45000 00:11:22:33:44:55:66:77
 *	console output:	00:11:22:33:44:55:66:77
 *
 */
int FWRITE(const char *da, const char *str_hex)
{
	unsigned char ee[MAX_FRW];
	unsigned int addr_da;
	int len;

	addr_da = strtoul(da, NULL, 16);
	if (addr_da && (len = atoh(str_hex, ee))) {
		FWrite(ee, addr_da, len);
		FREAD(addr_da, len);
	}
	return 0;
}

int check_macmode(const char *str)
{

	if ((!str) || (!strcmp(str, "")) || (!strcmp(str, "disabled"))) {
		return 0;
	}

	if (strcmp(str, "allow") == 0) {
		return 1;
	}

	if (strcmp(str, "deny") == 0) {
		return 2;
	}
	return 0;
}

void gen_macmode(int mac_filter[], int band, char *prefix)
{
	char temp[128];

	snprintf(temp, sizeof(temp), "%smacmode", prefix);
	mac_filter[0] = check_macmode(nvram_get(temp));
	_dprintf("mac_filter[0] = %d\n", mac_filter[0]);
}

int bw40_channel_check(int band,char *ext)
{
	int ch, ret = 1;
	char prefix[sizeof("wlX_XXXXXX")];

	if (band < 0 || band >= MAX_NR_WL_IF) {
		dbg("%s: unknown wl%d band!\n", __func__, band);
		return 0;
	}

	snprintf(prefix, sizeof(prefix), "wl%d_", band);
	ch = nvram_pf_get_int(prefix, "channel");
	if (!ch)
		return ret;	/* pass */

	switch (band) {
	case WL_2G_BAND:
		if ((ch == 1) || (ch == 2) ||(ch == 3)||(ch == 4)) {
			if (!strcmp(ext,"MINUS")) {
				dbG("stage 1: a  mismatch between %s mode and ch %d => fix mode\n", ext, ch);
				sprintf(ext,"PLUS");
			}
		} else if (ch >= 8) {
			if (!strcmp(ext,"PLUS")) {
				dbG("stage 2: a  mismatch between %s mode and ch %d => fix mode\n", ext, ch);
				sprintf(ext,"MINUS");
			}	   
		}
		//ch5,6,7:both
		break;
	case WL_5G_BAND:	/* fall-through */
//	case WL_5G_2_BAND:
			if ((ch == 36) || (ch == 44) || (ch == 52) || (ch == 60) ||
			    (ch == 100) || (ch == 108) ||(ch == 116) || (ch == 124) ||
			    (ch == 132) || (ch == 149) || (ch ==157))
			{
				if(!strcmp(ext, "MINUS")) {
					dbG("stage 1: a  mismatch between %s mode and ch %d => fix mode\n", ext, ch);
					sprintf(ext,"PLUS");
				}
			}
			else if ((ch == 40) || (ch == 48) || (ch == 56) || (ch == 64) ||
				 (ch == 104) || (ch == 112) || (ch == 120) || (ch == 128) ||
				 (ch == 136) || (ch == 153) ||(ch == 161))
			{
				if(!strcmp(ext, "PLUS")) {
					dbG("stage 2: a  mismatch between %s mode and ch %d => fix mode\n", ext, ch);
					sprintf(ext,"MINUS");
				}
			}
		break;
	}

	return ret;
}

int gen_ath_config()
{
	_dprintf("skip gen_ath_config()\n");
	return 0;
}

void Get_fail_log(char *buf, int size, unsigned int offset)
{
	struct FAIL_LOG fail_log, *log = &fail_log;
	char *p = buf;
	int x, y;

	memset(buf, 0, size);
	FRead((char *)&fail_log, offset, sizeof(fail_log));
	if (log->num == 0 || log->num > FAIL_LOG_MAX) {
		return;
	}
	for (x = 0; x < (FAIL_LOG_MAX >> 3); x++) {
		for (y = 0; log->bits[x] != 0 && y < 7; y++) {
			if (log->bits[x] & (1 << y)) {
				p += snprintf(p, size - (p - buf), "%d,",
					      (x << 3) + y);
			}
		}
	}
}

void ate_commit_bootlog(char *err_code)
{
	printf("[ATE] err_code:[%s]\n", err_code);
	nvram_set("Ate_power_on_off_enable", err_code);
	nvram_commit();
}

void platform_start_ate_mode(void)
{
	int model = get_model();
	switch (model) {
		case MODEL_BLUECAVE:
		case MODEL_RAX40:
		break;
	default:
		_dprintf("%s: model %d\n", __func__, model);
	}
}

/* Run iwlist command to do site-survey.
 * @ssv_if:
 * @return:
 *     -1:	invalid parameter
 * 	0:	site-survey fail
 * 	1:	site-survey success
 * NOTE:	sitesurvey filelock must be hold by caller!
 */
static int do_sitesurvey(char *ssv_if)
{
	int retry, ssv_ok;
	char *result, *p;
	char *iwlist_argv[] = { "iwlist", ssv_if, "scanning", NULL };

	if (!ssv_if || *ssv_if == '\0')
		return -1;

	for (retry = 0, ssv_ok = 0; !ssv_ok && retry < 1; ++retry) {
		_eval(iwlist_argv, ">/tmp/apscan_wlist", 0, NULL);

		if (!f_exists(APSCAN_WLIST) || !(result = file2str(APSCAN_WLIST)))
			continue;
		if (!(p = strstr(result, "Scan completed"))) {
			if ((p = strchr(result, '\n')))
				*p = '\0';
			if ((p = strchr(result, '\r')))
				*p = '\0';
			_dprintf("%s: iwlist %s scanning fail!! (%s)!\n", __func__, ssv_if, result);
			free(result);
			continue;
		}

		free(result);
		ssv_ok = 1;
	}

	return ssv_ok;
}

#define target 9
char str[target][40]={"Address:","ESSID:","Frequency:","Quality=","Encryption key:","IE:","Authentication Suites","Pairwise Ciphers","phy_mode="};

int getSiteSurvey(int band, char* ofile)
{
	int apCount=0;
	char header[128];
	FILE *fp,*ofp;
	char buf[target][200],set_flag[target];
	int i, ssv_ok = 0, radio, is_sta = 0, bitrate;
	char *pt1, *pt2, *pt3;
	char a1[10],a2[10];
	char ssid_str[256], ssv_if[10];
	char ch[4] = "", ssid[33] = "", address[18] = "", enc[9] = "";
	char auth[16] = "", sig[9] = "", wmode[8] = "";
	int  lock;
	char *staifname = get_staifname(band);
#if defined(RTCONFIG_WIRELESSREPEATER)
	char ure_mac[18];
	int wl_authorized = 0;
#endif
	int is_ready, wlc_band = -1;
	char temp1[200];
	char prefix_header[]="Cell xx - Address:";
#if defined(RTCONFIG_AMAS)
	int start_ldb = 0;
#endif

	wlan_setEndpointEnabled(band);
	dbG("site survey...\n");
	if (band < 0 || band >= MAX_NR_WL_IF)
		return 0;
	lock = file_lock("sitesurvey");
	do_sitesurvey(staifname);
	file_unlock(lock);
	if (!(fp= fopen(APSCAN_WLIST, "r")))
		return 0;
	snprintf(header, sizeof(header), "%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode");
	dbg("\n%s", header);
	if ((ofp = fopen(ofile, "a")) == NULL)
	{
		fclose(fp);
		return 0;
	}

	apCount=1;
	while(1)
	{
		is_ready=0;
		memset(set_flag,0,sizeof(set_flag));
		memset(buf,0,sizeof(buf));
		memset(temp1,0,sizeof(temp1));
		snprintf(prefix_header, sizeof(prefix_header), "Cell %02d - Address:",apCount);

		if(feof(fp))
			break;

		while(fgets(temp1,sizeof(temp1),fp))
		{
			if(strstr(temp1,prefix_header)!=NULL)
			{
				if(is_ready)
				{
					fseek(fp,-sizeof(temp1), SEEK_CUR);
					break;
				}
				else
				{
					is_ready=1;
					snprintf(prefix_header, sizeof(prefix_header),"Cell %02d - Address:",apCount+1);
				}	
			}
			if(is_ready)
			{
				for(i=0;i<target;i++)
				{
					if(strstr(temp1,str[i])!=NULL && set_flag[i]==0)
					{
						set_flag[i]=1;
						memcpy(buf[i],temp1,sizeof(temp1));
						break;
					}
				}
			}
		}
#if defined(RTCONFIG_CONCURRENTREPEATER)
		if(feof(fp)) {
			if(!is_ready)
				break;
		}
#endif
		dbg("\napCount=%d\n",apCount);
		apCount++;
		//ch
		pt1 = strstr(buf[2], "Channel ");	
		if(pt1)
		{

			pt2 = strstr(pt1,")");
			memset(ch,0,sizeof(ch));
			strncpy(ch,pt1+strlen("Channel "),pt2-pt1-strlen("Channel "));
		}

		//ssid
		pt1 = strstr(buf[1], "ESSID:");	
		if(pt1)
		{
			memset(ssid,0,sizeof(ssid));
			strncpy(ssid,pt1+strlen("ESSID:")+1,strlen(buf[1])-2-(pt1+strlen("ESSID:")+1-buf[1]));
		}


		//bssid
		pt1 = strstr(buf[0], "Address: ");	
		if(pt1)
		{
			memset(address,0,sizeof(address));
			strncpy(address,pt1+strlen("Address: "),strlen(buf[0])-(pt1+strlen("Address: ")-buf[0])-1);
		}

		//enc
		pt1=strstr(buf[4],"Encryption key:");
		if(pt1)
		{
			if(strstr(pt1+strlen("Encryption key:"),"on"))
			{
				pt2=strstr(buf[7],"Pairwise Ciphers");
				if(pt2)
				{
					if(strstr(pt2,"CCMP TKIP") || strstr(pt2,"TKIP CCMP"))
						strlcpy(enc, "TKIP+AES", sizeof(enc));
					else if(strstr(pt2,"CCMP"))
						strlcpy(enc, "AES", sizeof(enc));
					else
						strlcpy(enc, "TKIP", sizeof(enc));
				}
				else
					strlcpy(enc, "WEP", sizeof(enc));
			}
			else
				strlcpy(enc, "NONE", sizeof(enc));
		}

		//auth
		memset(auth,0,sizeof(auth));
		pt1=strstr(buf[5],"IE:");
		if(pt1 && strstr(buf[5],"Unknown")==NULL)
		{
			if(strstr(pt1+strlen("IE:"),"WPAWPA2")!=NULL)
				strlcpy(auth, "WPA-WPA2-", sizeof(auth));
			else if(strstr(pt1+strlen("IE:"),"WPA2")!=NULL)
				strlcpy(auth, "WPA2-", sizeof(auth));
			else if(strstr(pt1+strlen("IE:"),"WPA")!=NULL) 
				strlcpy(auth, "WPA-", sizeof(auth));

			pt2=strstr(buf[6],"Authentication Suites");
			if(pt2)
			{
				if(strstr(pt2+strlen("Authentication Suites"),"SAE")!=NULL){
					if(strstr(pt2+strlen("Authentication Suites"),"PSK")!=NULL)
						strlcpy(auth,"WPA2-WPA3-Personal", sizeof(auth));
					else
						strlcpy(auth,"WPA3-Personal", sizeof(auth));
				}else if(strstr(pt2+strlen("Authentication Suites"),"PSK")!=NULL)
					strcat(auth,"Personal");
				else if(strstr(pt2+strlen("Authentication Suites"),"802.1x")!=NULL)
					strcat(auth,"Enterprise");
				else
					_dprintf("%s:%d ERROR!! NO AKM TYPE Set!!\n", __func__, __LINE__);
			}
		}
		else
		{
			if(strcmp(enc,"WEP")==0)
			{
				strlcpy(auth, "Unknown", sizeof(auth));
			}else{
				strlcpy(auth, "Open System", sizeof(auth));
			}
		}

		//sig
		pt1 = strstr(buf[3], "Quality=");	
		pt2 = NULL;
		if (pt1 != NULL)
			pt2 = strstr(pt1,"/");
		if(pt1 && pt2)
		{
			memset(sig,0,sizeof(sig));
			memset(a1,0,sizeof(a1));
			memset(a2,0,sizeof(a2));
			strncpy(a1,pt1+strlen("Quality="),pt2-pt1-strlen("Quality="));
			strncpy(a2,pt2+1,strstr(pt2," ")-(pt2+1));
			sprintf(sig,"%d",100*(atoi(a1)+6)/(atoi(a2)+6));
		}

		//wmode
		memset(wmode,0,sizeof(wmode));
		pt1=strstr(buf[8],"phy_mode=");
		if(pt1)
		{
			pt2 = pt1+strlen("phy_mode=");
			if(strstr(pt2, "11AC_VHT")!=NULL)
				strlcpy(wmode, "ac", sizeof(wmode));
			else if(strstr(pt2,"11AXA_HE")!=NULL || strstr(pt2,"11AXG_HE")!=NULL)
				strlcpy(wmode, "ax", sizeof(wmode));
			else if(strstr(pt2,"11A")!=NULL || strstr(pt2,"TURBO_A")!=NULL)
				strlcpy(wmode, "a", sizeof(wmode));
			else if(strstr(pt2,"11B")!=NULL)
				strlcpy(wmode, "b", sizeof(wmode));
			else if(strstr(pt2,"11G")!=NULL || strstr(pt2,"TURBO_G")!=NULL)
				strlcpy(wmode, "bg", sizeof(wmode));
			else if(strstr(pt2,"11NA")!=NULL)
				strlcpy(wmode, "an", sizeof(wmode));
			else if(strstr(pt2,"11NG")!=NULL)
				strlcpy(wmode, "bgn", sizeof(wmode));
			else
				dbg("%s: Unknown phymode [%s]\n", __func__, pt2);
		}
		else
			strlcpy(wmode, "unknown", sizeof(wmode));

		dbg("%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n",ch,ssid,address,enc,auth,sig,wmode);

		if(safe_atoi(ch)<0)
			fprintf(ofp, "\"ERR_BAND\",");
		else if(safe_atoi(ch)>0 && safe_atoi(ch)<14)
			fprintf(ofp, "\"2G\",");
		else if(safe_atoi(ch)>14 && safe_atoi(ch)<166)
			fprintf(ofp, "\"5G\",");
		else
			fprintf(ofp, "\"ERR_BAND\",");


		memset(ssid_str, 0, sizeof(ssid_str));
#if defined(RTCONFIG_UTF8_SSID)
		char_to_ascii_with_utf8(ssid_str, trim_r(ssid));
#else
		char_to_ascii(ssid_str, trim_r(ssid));
#endif
		
		if(strlen(ssid)==0)
			fprintf(ofp, "\"\",");
		else
			fprintf(ofp, "\"%s\",", ssid_str);

		fprintf(ofp, "\"%d\",", safe_atoi(ch));

		fprintf(ofp, "\"%s\",",auth);
		
		fprintf(ofp, "\"%s\",", enc); 

		fprintf(ofp, "\"%d\",", safe_atoi(sig));

		fprintf(ofp, "\"%s\",", address);

		fprintf(ofp, "\"%s\",", wmode); 

#ifdef RTCONFIG_WIRELESSREPEATER		
		//memset(ure_mac, 0x0, 18);
		//snprintf(ure_mac, sizeof(ure_mac), "%02X:%02X:%02X:%02X:%02X:%02X",xxxx);
		if (strcmp(nvram_safe_get(wlc_nvname("ssid")), ssid)){
			if (strcmp(ssid, ""))
				fprintf(ofp, "\"%s\"", "0");				// none
			else if (!strcmp(ure_mac, address)){
				// hidden AP (null SSID)
				if (strstr(nvram_safe_get(wlc_nvname("akm")), "psk")!=NULL){
					if (wl_authorized){
						// in profile, connected
						fprintf(ofp, "\"%s\"", "4");
					}else{
						// in profile, connecting
						fprintf(ofp, "\"%s\"", "5");
					}
				}else{
					// in profile, connected
					fprintf(ofp, "\"%s\"", "4");
				}
			}else{
				// hidden AP (null SSID)
				fprintf(ofp, "\"%s\"", "0");				// none
			}
		}else if (!strcmp(nvram_safe_get(wlc_nvname("ssid")), ssid)){
			if (!strlen(ure_mac)){
				// in profile, disconnected
				fprintf(ofp, "\"%s\",", "1");
			}else if (!strcmp(ure_mac, address)){
				if (strstr(nvram_safe_get(wlc_nvname("akm")), "psk")!=NULL){
					if (wl_authorized){
						// in profile, connected
						fprintf(ofp, "\"%s\"", "2");
					}else{
						// in profile, connecting
						fprintf(ofp, "\"%s\"", "3");
					}
				}else{
					// in profile, connected
					fprintf(ofp, "\"%s\"", "2");
				}
			}else{
				fprintf(ofp, "\"%s\"", "0");				// impossible...
			}
		}else{
			// wl0_ssid is empty
			fprintf(ofp, "\"%s\"", "0");
		}
#else
		fprintf(ofp, "\"%s\"", "0");      
#endif
		fprintf(ofp, "\n");
	}
	fclose(fp);
	fclose(ofp);
	return 1;
}

char *getStaMAC(char *buf, int buflen)
{
	FILE *fp;
	int len;
	char *pt1,*pt2;
	char cmd[512];

	snprintf(cmd, sizeof(cmd), "ifconfig %s", get_staifname(nvram_get_int("wlc_band")));

	fp = PS_popen(buf, "r");
	if (fp) {
		memset(buf, 0, buflen);
		len = fread(buf, 1, buflen, fp);
		PS_pclose(fp);
		if (len > 1) {
			buf[len - 1] = 0x0;
			pt1 = strstr(buf, "HWaddr ");
			if (pt1) 
			{
				pt1[24] = 0x0;
				pt2 = pt1 + strlen("HWaddr ");
				return pt2;
			}
		}
	}
	return NULL;
}

unsigned int getPapState(int unit)
{
	char buf[8192], sta[64];
	FILE *fp;
	int len;
	char *pt1, *pt2;

	strcpy(sta, get_staifname(unit));
	sprintf(buf, "iwconfig %s", sta);
	fp = popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, "Access Point:");
			if (pt1) {
				pt2 = pt1 + strlen("Access Point:");
				pt1 = strstr(pt2, "Not-Associated");
				if (pt1) 
				{
					sprintf(buf, "ifconfig | grep %s", sta);
				     	fp = popen(buf, "r");
					if(fp)
				   	{
						 memset(buf, 0, sizeof(buf));
						 len = fread(buf, 1, sizeof(buf), fp);
						 pclose(fp);
						 if(len>=1)
						    return 0;
						 else
						    return 3;
					}	
				     	else	
				   		return 0; //init
				}	
				else
				   	return 2; //connect and auth ?????
					
				}
			}
		}
	
	return 3; // stop
}

int wlcconnect_core(void)
{
	int result;
	result = getPapState(nvram_get_int("wlc_band"));
	if(result != 2)
		dbg("%s(0x%04x): check..wlconnect=%d \n", __func__, __LINE__, result);
	return result;
}

int wlcscan_core(char *ofile, char *wif)
{
	int count = 0;
	while (count != 2)
	{
		if(getSiteSurvey(get_wifname_band(wif), ofile))
			break;
		count++;
		dbg("[rc] set scan results command failed, retry %d\n", count);
		sleep(1);
	}
	return 0;
}

int ubi_remove_dev(const char *mtdname, int remove)
{
//deprecated function
	return 0;
}

void check_ubi_partition(void)
{
	int dev, part, size;

	fprintf(stderr, "... check_ubi_partition() ...\n");
	MKNOD("/dev/ubi1", S_IFCHR | 0666, makedev(248, 0));
	MKNOD("/dev/ubi1_0", S_IFCHR | 0666, makedev(248, 1));
	fprintf(stderr, "... start ubiattach ...\n");
#if defined(K3C)
	system("ubiattach /dev/ubi_ctrl -m 7");
#elif defined(BLUECAVE)
	system("ubiattach /dev/ubi_ctrl -m 8");
#else
#error fix me
#endif
	while(pids("ubiattach"))
	{
		fprintf(stderr, "... waiting ubiattach finished ...\n");
		sleep(1);
	}
	fprintf(stderr, "... ubiattach finished ...\n");

	if((ubi_getinfo("jffs2", &dev, &part, &size)) != 0)
	{
#if defined(K3C)
		fprintf(stderr, "... detach mtd7 ...\n");
		system("ubidetach -p /dev/mtd7");
    	fprintf(stderr, "... start flash_erase ...\n");
		system("flash_erase /dev/mtd7 0 0");
#elif defined(BLUECAVE)
		fprintf(stderr, "... detach mtd8 ...\n");
		system("ubidetach -p /dev/mtd8");
    	fprintf(stderr, "... start flash_erase ...\n");
		system("flash_erase /dev/mtd8 0 0");
#else
#error fix me
#endif
		while(pids("flash_erase"))
		{
			fprintf(stderr, "... waiting flash_erase finished ...\n");
			sleep(1);
		}
		fprintf(stderr, "... flash_erase finished ...\n");
		fprintf(stderr, "... start ubiattach ...\n");
#if defined(K3C)
		system("ubiattach /dev/ubi_ctrl -m 7");
#elif defined(BLUECAVE)
		system("ubiattach /dev/ubi_ctrl -m 8");
#else
#error fix me
#endif
		while(pids("ubiattach"))
		{
			fprintf(stderr, "... waiting ubiattach finished ...\n");
			sleep(1);
		}
		fprintf(stderr, "... ubiattach finished ...\n");
    	fprintf(stderr, "... start ubimkvol ...\n");
    	system("ubimkvol /dev/ubi1 -N jffs2 -m");
		while(pids("ubimkvol"))
		{
			fprintf(stderr, "... waiting ubimkvol finished ...\n");
			sleep(1);
		}
		fprintf(stderr, "... ubimkvol finished ...\n");
	}
	fprintf(stderr, "... ubi_getinfo() finished ...\n");
}

int get_usb_mode()
{
	int ret = 0;
	FILE *fp;
	char *tmp;
	unsigned int value;
	char buf[64];

	memset(buf, 0, sizeof(buf));
	if((fp = popen("mem -s 0x1a40c020 -du", "r")) != NULL)
	{
		fgets(buf, sizeof(buf), fp);
		tmp = strchr(buf, ':');
		if (tmp)
		{
			value = strtol(tmp + 1, NULL, 16);
			_dprintf("Read 0x1a40c020 as [0x%08x]\n", value);
			ret = 1;
			if(value == 0x2000)
				ret = 0;
			else if (value)
				ret = -1;
		} else
			ret = 0;
		pclose(fp);
	}
	return ret;
}

void gen_config_sh(void)
{
	system("cp -f /rom/opt/lantiq/etc/rc.d/config.sh /etc/; cd /etc/rc.d; ln -s ../config.sh config.sh");
}

void usb_pwr_ctl(int onoff)
{
	FILE *fp;
	char *tmp;
	unsigned int value;
	char cmd[64];
	char buf[64];


	memset(buf, 0, sizeof(buf));
	if((fp = popen("mem -s 0x16c00000 -du", "r")) != NULL)
	{
		fgets(buf, sizeof(buf), fp);
		tmp = strchr(buf, ':');
		if(tmp)
		{
			value = strtol(tmp + 1, NULL, 16);
			_dprintf("Read 0x16c00000 as [0x%08x]\n", value);
			if(onoff == 1)
			{
				_dprintf("Set usb power [on]\n");
				value = value | 0x80;
			} else {
				_dprintf("Set usb power [off]\n");
		    	value = value & 0xFFFFFF7F;
			}
			_dprintf("Write 0x16c00000 as [0x%08x]\n", value);
      		snprintf(cmd, sizeof(cmd), "mem -s 0x16c00000 -w 0x%08x -u", value);
      		system(cmd);
		}
		pclose(fp);
	}
}

void set_usb3_to_usb2()
{
	if (!get_usb_mode()){
		_dprintf("[usb] already USB2 mode, skip\n");
		return
	}
	_dprintf("[wait] usb3 to usb2 start\n");
	_ejusb_main("-1", 0);
	notify_rc_after_wait("restart_nasapps");
	_dprintf("[warning] power off usb and change mode\n");
	usb_pwr_ctl(0);
	system("mem -s 0x1a40c020 -uw 0x2000");
	usb_pwr_ctl(1);
	_dprintf("[wait] usb3 to usb2 end\n");
}

void set_usb2_to_usb3()
{
	if (get_usb_mode() == 1){
		_dprintf("[usb] already USB3 mode, skip\n");
		return;
	}
	_dprintf("[wait] usb2 to usb3 start\n");
	_ejusb_main("-1", 0);
	notify_rc_after_wait("restart_nasapps");
	_dprintf("[warning] power off usb and change mode\n");
  	usb_pwr_ctl(0);
	system("mem -s 0x1a40c020 -uw 0x0");
	usb_pwr_ctl(1);
	_dprintf("[wait] usb2 to usb3 end\n");
}

void apply_config_to_driver()
{
	trigger_wave_monitor(__func__, __LINE__, WAVE_ACTION_SET_STA_CONFIG);
}

int Pty_get_wlc_status(int band)
{
	return 0;
}

int Pty_start_wlc_connect(int band)
{
	int result = 0;
	char *sta;

	sta = get_staifname(band);
	if(!is_if_up(sta))
		result = eval("ifconfig", sta, "up");

	return result;
}

int get_psta_rssi(int band)
{
	char *sta;
	FILE *fp;
	int ant;
	int trssi;
	int result;
	int rssi;
	char tmp[128], buf[128];
	int signal;

	memset(tmp, 0, sizeof(tmp));
	signal = 0;
	sta = get_staifname(band);
	snprintf(tmp, sizeof(tmp), "/proc/net/mtlk/%s/PeerFlowStatus", sta);
	if ((fp = fopen(tmp, "r")) != NULL)
	{
		ant = 0;
		trssi = 0;
		memset(buf, 0, sizeof(buf));
		while (fgets(buf, sizeof(buf), fp))
		{
			if(strstr(buf, "RSSI"))
			{
				sscanf(buf, "%d", &signal);
				++ant;
				trssi += signal;
			}
		}
		fclose(fp);
		if (!ant)
			return pre_rssi[band];
		rssi = trssi/ant;
		if(rssi == -128)
			return pre_rssi[band];
		result = -1;
		if(rssi < 0)
		{
			result = pre_rssi[band];
			if (rssi >= -91)
				result = rssi;
		}
	} else
		result = pre_rssi[band];

	return result;
}

int Pty_stop_wlc_connect(int band)
{
	int result = 0;
	char *sta;

	sta = get_staifname(band);
	if(is_if_up(sta))
		result = eval("ifconfig", sta, "down");

	return result;
}

int Pty_get_upstream_rssi(int band)
{
	return get_psta_rssi(band);
}

int get_psta_status()
{
	return getPapState();
}

void wlconf_pre()
{
	generate_wl_para(0, -1);
	generate_wl_para(1, -1);
}

int get_wlan_service_status(int bssidx, int vifidx)
{
	return wave_is_radio_on(wl_wave_unit(bssidx));
}

void set_wlan_service_status(int bssidx, int vifidx, int enabled)
{
	int time = 3;
	int action = 0;
	while(!nvram_get_int("wave_ready")){
		_dprintf("[%s][%d] wave_ready==0, wait... [%d][%d][%d]\n", __func__, __LINE__, bssidx, vifidx, enabled);
		--time;
		sleep(10);
		if(time == 0){
			_dprintf("[%s][%d] wait wave_ready=1 over 20 seconds, please check wave_monitor status, skip [%d][%d][%d]\n", __func__, __LINE__, bssidx, vifidx, enabled);
			return;
		}
	}
	while(nvram_get_int("wave_action")){
		_dprintf("wave_action != IDLE, wait... [%s]\n", __func__);
		sleep(1);
	}
#if 0
	if(bssidx == 0 && enabled == 1)
		trigger_wave_monitor(__func__, __LINE__, WAVE_ACTION_RE_AP2G_ON);
	else if((bssidx == 0 && enabled == 0)
		trigger_wave_monitor(__func__, __LINE__, WAVE_ACTION_RE_AP2G_OFF);
	else if(bssidx == 1 && enabled == 1)
		trigger_wave_monitor(__func__, __LINE__, WAVE_ACTION_RE_AP5G_ON);
	else if(bssidx == 1 && enabled == 0)
		trigger_wave_monitor(__func__, __LINE__, WAVE_ACTION_RE_AP5G_OFF);
#endif
	switch(bssidx){
	case 0:
		trigger_wave_monitor(__func__, __LINE__, enabled ? WAVE_ACTION_RE_AP2G_ON : WAVE_ACTION_RE_AP2G_OFF);
		break;
	case 1:
		trigger_wave_monitor(__func__, __LINE__, enabled ? WAVE_ACTION_RE_AP5G_ON : WAVE_ACTION_RE_AP5G_OFF);
		break;
	default:
		break;
	}
}
#if 0
int get_wlan_service_status(int bssidx, int vifidx)
{
#if 0
	FILE *fp = NULL;
	char maxassoc_file[128]={0};
	char buf[64]={0};
	char maxassoc[64]={0};
	char tmp[128] = {0}, prefix[] = "wlXXXXXXXXXX_";
	char *ifname = NULL;

	if(vifidx > 0)
		snprintf(prefix, sizeof(prefix), "wl%d.%d_", bssidx, vifidx);
	else
		snprintf(prefix, sizeof(prefix), "wl%d", bssidx);

	ifname = nvram_safe_get(strcat_r(prefix, "ifname", tmp));


	snprintf(maxassoc_file, sizeof(maxassoc_file), "/tmp/maxassoc.%s", ifname);

	doSystem("wl -i %s maxassoc > %s", ifname, maxassoc_file);

	if ((fp = fopen(maxassoc_file, "r")) != NULL) {
		fscanf(fp, "%s", buf);
		fclose(fp);
	}
	sscanf(buf, "%s", maxassoc);

	return atoi(maxassoc);
#endif
	return 0;
}

int set_wlan_service_status(int bssidx, int vifidx, int enabled)
{

#if 0
	char tmp[128]={0}, prefix[] = "wlXXXXXXXXXX_", wlprefix[] = "wlXXXXXXXXXX_";
	char *ifname = NULL;

	if(vifidx > 0)
		snprintf(prefix, sizeof(prefix), "wl%d.%d_", bssidx, vifidx);
	else
		snprintf(prefix, sizeof(prefix), "wl%d", bssidx);

	memset(wlprefix, 0x00, sizeof(wlprefix));
	snprintf(wlprefix, sizeof(wlprefix), "wl%d_", bssidx);

	ifname = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

	if (enabled == 0) {
		doSystem("wl -i %s maxassoc 0", ifname);
	}
	else {
#ifdef HND_ROUTER
		doSystem("wl -i %s maxassoc %d", ifname, nvram_get_int(strcat_r(wlprefix, "cfg_maxassoc", tmp)));
#else
		doSystem("wl -i %s maxassoc %d", ifname, nvram_get_int(strcat_r(wlprefix, "maxassoc", tmp)));
#endif
	}
#endif
	return 0;
}
#endif

enum {
	IFTYPE_WAN = 1,
	IFTYPE_2G = 2,
	IFTYPE_5G = 4,
	IFTYPE_5G2 = 8
};

void set_pre_sysdep_config(int iftype)
{
	char word[256], *next = NULL, cmd[64], ifnames[128];
	memset(cmd, 0, sizeof(cmd));
	strcpy(ifnames, nvram_safe_get("sta_phy_ifnames"));
	foreach(word, ifnames, next) {
		snprintf(cmd, sizeof(cmd), "ppacmd dellan -i %s", word);
		doSystem(cmd);
	}
	strcpy(ifnames, nvram_safe_get("eth_ifnames"));
	foreach(word, ifnames, next) {
		snprintf(cmd, sizeof(cmd), "ppacmd dellan -i %s", word);
		doSystem(cmd);
	}
}

void set_post_sysdep_config(int iftype)
{
	char word[256], *next = NULL, cmd[64], ifnames[128];
	int result, i;
	memset(cmd, 0, sizeof(cmd));
	if(iftype == IFTYPE_WAN){
		strcpy(cmd, "ppacmd addlan -i eth1");
		doSystem(cmd);
	} else {
		result = 7;	
		if(iftype){
			result = (iftype & IFTYPE_2G) != 0;
			if((iftype & IFTYPE_5G) != 0)
				result |= 2;
		}
		if((iftype & IFTYPE_5G2) != 0)
			result |= 4;
		strcpy(ifnames, nvram_safe_get("sta_ifnames"));
		i = 0;
		foreach(word, ifnames, next) {
			if(((result >> i) & 1) != 0){
				snprintf(cmd, sizeof(cmd), "echo \"add %s\" > /proc/l2nat/dev", word);
				doSystem(cmd);
				snprintf(cmd, sizeof(cmd), "ppacmd addlan -i %s", word);
				doSystem(cmd);
			}
			++i;
		}
	}
}

int get_radar_status(int bssidx)
{
	if(bssidx)
		return nvram_get_int("radar_status");
	else
		return 0;
}

#ifdef LANTIQ_BSD
void bandstr_sync_wl_settings(void)
{
	char tmp[100], tmp2[100], prefix[16] = "wlXXXXXXXXXX_", prefix2[16] = "wlXXXXXXXXXX_";
	int i, n;
	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));
	n = num_of_wl_if();
	snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	for(i = 1; i < n; i++){
		snprintf(prefix2, sizeof(prefix2), "wl%d_", i);
		nvram_pf_set(prefix2, "ssid",  nvram_pf_safe_get(prefix, "ssid"));
		nvram_pf_set(prefix2, "auth_mode_x", nvram_pf_safe_get(prefix, "auth_mode_x"));
		nvram_pf_set(prefix2, "wep_x", nvram_pf_safe_get(prefix, "wep_x"));
		nvram_pf_set(prefix2, "key", nvram_pf_safe_get(prefix, "key"));
		nvram_pf_set(prefix2, "key1", nvram_pf_safe_get(prefix, "key1"));
		nvram_pf_set(prefix2, "key2", nvram_pf_safe_get(prefix, "key2"));
		nvram_pf_set(prefix2, "key3", nvram_pf_safe_get(prefix, "key3"));
		nvram_pf_set(prefix2, "key4", nvram_pf_safe_get(prefix, "key4"));
		nvram_pf_set(prefix2, "phrase_x", nvram_pf_safe_get(prefix, "phrase_x"));
		nvram_pf_set(prefix2, "crypto", nvram_pf_safe_get(prefix, "crypto"));
		nvram_pf_set(prefix2, "wpa_psk", nvram_pf_safe_get(prefix, "wpa_psk"));
		nvram_pf_set(prefix2, "radius_ipaddr", nvram_pf_safe_get(prefix, "radius_ipaddr"));
		nvram_pf_set(prefix2, "radius_key", nvram_pf_safe_get(prefix, "radius_key"));
		nvram_pf_set(prefix2, "radius_port", nvram_pf_safe_get(prefix, "radius_port"));
		nvram_pf_set(prefix2, "closed", nvram_pf_safe_get(prefix, "closed"));
		nvram_pf_set(prefix2, "mfp", nvram_pf_safe_get(prefix, "mfp"));
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		nvram_pf_set(prefix2, "11ax", nvram_pf_safe_get(prefix, "11ax"));
#endif
	}
}
#endif

#ifdef RTCONFIG_WPS_ENROLLEE
void start_wsc_enrollee(void)
{
	nvram_set("wps_enrollee", "1");
	doSystem("wpa_cli -i %s wps_pbc", get_staifname(0));//only 2.4G?
}

void stop_wsc_enrollee(void)
{
	int i;
	char word[256], *next, ifnames[128];
	char fpath[64], sta[64];

	i = 0;
	strcpy(ifnames, nvram_safe_get("wl_ifnames"));
	foreach(word, ifnames, next) {
		if (i >= MAX_NR_WL_IF)
			break;
		SKIP_ABSENT_BAND_AND_INC_UNIT(i);
		strcpy(sta, get_staifname(i));
		doSystem("wpa_cli -i %s wps_cancel", sta);
		i++;
	}
}

char *getWscStatus_enrollee(int unit)
{
	char buf[512];
	FILE *fp;
	int len;
	char *pt1, *pt2;

	snprintf(buf, sizeof(buf), "wpa_cli -i %s status", get_staifname(unit));
	fp = popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, "wpa_state=");
			if (pt1) {
				pt2 = pt1 + strlen("wpa_state=");
				pt1 = strstr(pt2, "address=");
				if (pt1) {
					*pt1 = '\0';
					chomp(pt2);
				}
				return pt2;
			}
		}
	}

	return "";
}
#endif

int Pty_procedure_check(int unit, int wlif_count)
{
	return 0;
}

int is_default()
{
	if(g_reboot || g_upgrade || IS_ATE_FACTORY_MODE() || nvram_get_int("obd_Setting") == 1 || nvram_get_int("x_Setting") == 1)
		return 0;
	else
		return nvram_get_int("obdeth_Setting");
}

int no_need_obd(void)
{
	if(g_reboot || g_upgrade || IS_ATE_FACTORY_MODE() || (nvram_get("sw_mode") && nvram_get_int("sw_mode") != SW_MODE_ROUTER) || 
    nvram_get_int("obd_Setting") == 1 || nvram_get_int("x_Setting") == 1 && nvram_get_int("obdeth_Setting") == 1 ) ||
	!nvram_get_int("wave_ready"))
		return -1;
	else
		return pids("obd");
}

int no_need_obdeth(void)
{
	if(g_reboot || g_upgrade || IS_ATE_FACTORY_MODE() || (nvram_get("sw_mode") && nvram_get_int("sw_mode") != SW_MODE_ROUTER) || 
    nvram_get_int("obd_Setting") == 1 || nvram_get_int("x_Setting") == 1 || nvram_get_int("obdeth_Setting") == 1 ) ||
	!nvram_get_int("wave_ready"))
		return -1;
	else
		return pids("obd_eth");
}

int amas_wait_wifi_ready(void)
{
	int result = 0;

	while(1)
	{
		result = nvram_get_int("wave_ready");
		if (result)
			break;
		sleep(5);
	}
	return result;
}
