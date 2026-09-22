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
	char	model[MAX_NR_CLIENT_LIST][32];
	char	modelname[MAX_NR_CLIENT_LIST][32];
	unsigned char	ip_addr[MAX_NR_CLIENT_LIST][4];
	unsigned char	mac_addr[MAX_NR_CLIENT_LIST][6];
	unsigned char	timeout[MAX_NR_CLIENT_LIST];//120s
	unsigned char	nvram_sync_status[MAX_NR_CLIENT_LIST];//0:no sync, 1:sync complete
	CLIENT_SYSINFO_INFO_TABLE sysinfo[MAX_NR_CLIENT_LIST];
	int		count;
}CLIENT_SYNC_DETAIL_INFO_TABLE;

#endif
