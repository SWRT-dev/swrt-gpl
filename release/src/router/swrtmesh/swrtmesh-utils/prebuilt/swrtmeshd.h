#ifndef __SWRTMESHD_H__
#define __SWRTMESHD_H__

#ifdef RTCONFIG_MAX_RE
#define MAX_NR_CLIENT_LIST	(RTCONFIG_MAX_RE + 1)
#else
#define MAX_NR_CLIENT_LIST	(MAX_RELIST_NUM + 1)
#endif

#define SHMKEY_SWRTMESHD 2001
#define KEY_SHM_CFG		SHMKEY_SWRTMESHD

typedef struct {
	unsigned char cpu_temp;
	unsigned char wifi1_temp;
	unsigned char wifi2_temp;
	unsigned char wifi3_temp;
	unsigned char wifi4_temp;
	unsigned char ramtotal;
	unsigned char ramfree;
	unsigned char jffstotal;
	unsigned char jffsfree;
}CLIENT_SYSINFO_INFO_TABLE;

typedef struct {
	unsigned char bandnum;
	unsigned char wl0_unit;
	unsigned char wl1_unit;
	unsigned char wl2_unit;
	unsigned char wl3_unit;
}BAND_INFO_TABLE;

typedef struct {
	char	model[MAX_NR_CLIENT_LIST][32];
	char	modelname[MAX_NR_CLIENT_LIST][32];
	unsigned char	ip_addr[MAX_NR_CLIENT_LIST][4];
	unsigned char	mac_addr[MAX_NR_CLIENT_LIST][6];
	unsigned char	timeout[MAX_NR_CLIENT_LIST];//120s
	unsigned char	nvram_sync_status[MAX_NR_CLIENT_LIST];//0:no sync, 1:sync complete
	CLIENT_SYSINFO_INFO_TABLE sysinfo[MAX_NR_CLIENT_LIST];
	BAND_INFO_TABLE band[MAX_NR_CLIENT_LIST];
	int		count;
}CLIENT_SYNC_DETAIL_INFO_TABLE;

#if defined(BIT)
#undef BIT
#endif
#define BIT(x)	((1 << x))

typedef struct _capability_ss {
	unsigned int type;
	unsigned int subtype;
	unsigned int capSupportRole;
} capability_s;

typedef struct _led_capability_ss {
	unsigned int type;
	unsigned int subtype;
} led_capability_s;
/* type */
enum capabilityType {
	LED_CONTROL = 1,
	REBOOT_CTL = 2,
#ifdef RTCONFIG_BHCOST_OPT
	FORCE_TOPOLOGY_CTL = 3,
#endif
	RC_SUPPORT = 4,
	LINK_AGGREGATION = 5,
#ifdef RTCONFIG_AMAS_WGN
	GUEST_NETWORK_NO_2G = 6,
	GUEST_NETWORK_NO_5G = 7,
	GUEST_NETWORK_NO_5GH = 8,
#endif
	STA_BAND0_AUTH = 9,			/* supported authentication for band0 sta (upstream) */
	STA_BAND1_AUTH = 10,			/* supported authentication for band1 sta (upstream) */
	STA_BAND2_AUTH = 11,			/* supported authentication for band2 sta (upstream) */
	AP_BAND0_AUTH = 12,			/* supported authentication for band0 ap (downstream) */
	AP_BAND1_AUTH = 13,			/* supported authentication for band1 ap (downstream) */
	AP_BAND2_AUTH = 14,			/* supported authentication for band2 ap (downstream) */
	WANS_CAP = 15,
	RE_RECONNECT = 16,
	FORCE_ROAMING = 17,
#ifdef RTCONFIG_FRONTHAUL_DWB
	FRONTHAUL_AP_CTL = 18,
#endif
#ifdef RTCONFIG_STA_AP_BAND_BIND
	STA_BINDING_AP = 19,
#endif
	RESET_DEFAULT = 20,
#ifdef RTCONFIG_BHCOST_OPT
	CONN_UPLINK_PORTS = 21,
#endif
	WIFI_RADIO_CTL = 22,
#ifdef RTCONFIG_BHCOST_OPT
	CONN_EAP_MODE = 23,
#endif

#ifdef RTCONFIG_AMAS_WGN
	GUEST_NETWORK_NO_6G = 24, 	
#endif
#ifdef RTCONFIG_AMAS_CHANNEL_PLAN
	CHANNEL_PLAN = 25,
#endif

#ifdef RTCONFIG_MULTILAN_CFG
	WIFI_BAND_CAP = 26,
	LAN_PORT_CAP = 27,
	WAN_PORT_CAP = 28,
#endif // RTCONFIG_MULTILAN_CFG

#ifdef RTCONFIG_AMAS_CENTRAL_ADS
	DIVERSITY_PORT_STATE_BAND0 = 29,
	DIVERSITY_PORT_STATE_BAND1 = 30,
	DIVERSITY_PORT_STATE_BAND2 = 31,
	DIVERSITY_PORT_STATE_BAND3 = 32,
#endif

#if defined(RTCONFIG_AMAS_WGN) || defined(RTCONFIG_MULTILAN_CFG)
	GUEST_NETWORK_NO_6GH = 34, 	
#endif

#ifdef RTCONFIG_MULTILAN_CFG
	MAX_MTLAN = 35,
	AVAILABLE_SDN_INDEX = 36,
#endif	// RTCONFIG_MULTILAN_CFG

#ifdef RTCONFIG_MLO
	MLO_RADIO = 40,
#ifdef SMARTHAUL
	SMART_HAUL = 41,
#endif
#endif
#ifdef RTCONFIG_REBOOT_SCHEDULE_V2
	REBOOT_SCHEDULE_V2 = 42,
#endif
	CAPABILITY_MAX
};

/* subtype */

#ifdef RTCONFIG_BHCOST_OPT
/* for FORCE_TOPOLOGY_CTL & CONN_EAP_MODE */
#define GENERAL_MODE   BIT(0)
#define PREFER_NODE_APPLY    BIT(1)
#endif

#ifdef RTCONFIG_FRONTHAUL_DWB
/* for FRONTHAUL_AP_CTL */
#define FRONTHAUL_AP_OPTION_OFF	BIT(0)
#define FRONTHAUL_AP_OPTION_AUTO	BIT(1)
#define FRONTHAUL_AP_OPTION_ON	BIT(2)
#endif

/* for REBOOT_CTL */
#define MANUAL_REBOOT     BIT(0)

/* for RC_SUPPORT */
#define USBX			BIT(0)
#define GUEST_NETWORK		BIT(1)
#define CFG_MNT_WPA3		BIT(2)	/* amas-utils.h has a WPA3 definition with different value. */
#define VIF_ONBOARDING			BIT(3)
#define WL_SCHED_V2		BIT(4)
#define WIFI_RADIO		BIT(5)
#define WL_SCHED_V3		BIT(6)
#define REVERT_FW		BIT(7)
#define SWITCHCTRL		BIT(8)
#define PORT_STATUS		BIT(9)
#define LOCAL_ACCESS		BIT(10)
#define CABLE_DIAG		BIT(11)
#define NO_FW_MANUAL		BIT(12)
#define UPDATE			BIT(13)

#define WIFI7_SUPPORT  BIT(23)

/* for LINK_AGGREGATION */
#define LACP_ENABLE                    BIT(0)

/* for GUEST_NETWORK_NO */
#define ONE_GUEST_NETWORK	BIT(0)
#define TWO_GUEST_NETWORK	BIT(1)
#define THREE_GUEST_NETWORK	BIT(2)

/* for Wireless Authentication */
#define OPEN_SYSTEM			BIT(0)
#define SHARED_KEY				BIT(1)
#define WPA_PERSONAL			BIT(2)
#define WPA2_PERSONAL			BIT(3)
#define WPA3_PERSONAL			BIT(4)
#define WPA_WPA2_PERSONAL	BIT(5)
#define WPA2_WPA3_PERSONAL	BIT(6)
#define WPA_ENTERPRISE			BIT(7)
#define WPA2_ENTERPRISE		BIT(8)
#define WPA3_ENTERPRISE		BIT(9)
#define WPA_WPA2_ENTERPRISE	BIT(10)
#define WPA2_WPA3_ENTERPRISE	BIT(11)
#define RADIUS_WITH_8021X		BIT(12)
#define OWE						BIT(13)

/* for wan capability */
#define WANS_CAP_WAN        BIT(0)

/* for re reconnect */
#define MANUAL_RECONN		BIT(0)

/* for sta force roaming */
#define MANUAL_FORCE_ROAMING		BIT(0)

#ifdef RTCONFIG_STA_AP_BAND_BIND
/* for sta binding ap */
#define MANUAL_STA_BINDING		BIT(0)
#endif

/* for reset to default */
#define MANUAL_RESET_DEFAULT		BIT(0)

/* for WIFI_RADIO_NO */
#define WIFI_RADIO_2G		BIT(0)
#define WIFI_RADIO_5G		BIT(1)
#define WIFI_RADIO_5GH		BIT(2)
#define WIFI_RADIO_6G		BIT(3)

/* Capability support on role */
#define CAP_SUPPORT		BIT(0)
#define RE_SUPPORT		BIT(1)

/* for LED_CONTROL */
#define CENTRAL_LED		BIT(0)
#define LP55XX_LED		BIT(1)
#define LED_ON_OFF		BIT(2)
#define LED_BRIGHTNESS		BIT(3)
#define LED_AURA		BIT(4)

/* capability */
static capability_s capability_list[] __attribute__ ((unused)) = {
#ifdef RTCONFIG_BHCOST_OPT
	{ FORCE_TOPOLOGY_CTL, GENERAL_MODE | PREFER_NODE_APPLY, RE_SUPPORT |CAP_SUPPORT},
#endif
	{ REBOOT_CTL, MANUAL_REBOOT, RE_SUPPORT | CAP_SUPPORT},
#if defined(RTCONFIG_LACP)
	{ LINK_AGGREGATION, LACP_ENABLE, RE_SUPPORT},
#endif
#ifdef RTCONFIG_RE_RECONNECT
	{ RE_RECONNECT, MANUAL_RECONN, RE_SUPPORT |CAP_SUPPORT},
#endif
#ifdef RTCONFIG_FORCE_ROAMING
	{ FORCE_ROAMING, MANUAL_FORCE_ROAMING, RE_SUPPORT |CAP_SUPPORT},
#endif
#ifdef RTCONFIG_FRONTHAUL_DWB
	{ FRONTHAUL_AP_CTL,
	FRONTHAUL_AP_OPTION_OFF
#if defined(RTCONFIG_FRONTHAUL_AP_AUTO_OPT)
	| FRONTHAUL_AP_OPTION_AUTO
#endif
	| FRONTHAUL_AP_OPTION_ON,
	RE_SUPPORT | CAP_SUPPORT},
#endif
#ifdef RTCONFIG_STA_AP_BAND_BIND
	{ STA_BINDING_AP, MANUAL_STA_BINDING, RE_SUPPORT |CAP_SUPPORT},
#endif
	{ RESET_DEFAULT, MANUAL_RESET_DEFAULT, RE_SUPPORT |CAP_SUPPORT},

	/* END */
	{ 0, 0 }
};
#endif
