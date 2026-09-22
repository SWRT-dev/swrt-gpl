#ifndef __LIBSWRTH__
#define __LIBSWRTH__
#include <limits.h>
#include <sqlite3.h>
#include <pthread.h>

#define DIAG_MAX_USB_HUB_PORT 6

struct swrt_eth_port {
	char label_name[8];
	unsigned int cap;
	uint8 is_on;
	unsigned int link_speed;
	unsigned int link_speed_max;
	int brown;
	int brown_len;
	int blue;
	int blue_len;
	int green;
	int green_len;
	int orange;
	int orange_len;
	usb_device_info_t usb_devices[DIAG_MAX_USB_HUB_PORT];
	struct swrt_eth_port *next;
	int cable_diag_triger_link_st;
	time_t cmd_time;
	int seq_no;
	char ui_display[32];
	int phy_port_id;
	int ext_port_id;
	char ifname[32];
	unsigned int flag;
};

struct swrt_eth_port_table {
	char node_mac[18];
	struct swrt_eth_port *portlist;
	struct swrt_eth_port_table *next;
	uint8 cable_diag_active;//indicate cable-idag state of this node
};

struct stainfo {
	char sta_mac[18];
	double tx_rate;
	double rx_rate;
	int conn_time;
	int inactive_flag;
	char conn_if[16];
	int conn_if_idx;
	int conn_if_vidx;
	int rssi;
	//time_t last_update;
	struct stainfo *next;
};

struct stainfo_table {
	char node_mac[18];
	struct stainfo *stalist;
	struct stainfo_table *next;
};

#define COLUMN_TYPE_MASK 0x0000FFFF
#define COLUMN_IGNORE 0x00010000


#define COLUMN_TYPE_ALL_STR 0xFFFFFFFF //for db_value_types

struct CONNDIAG_DB_t { 
	int mode; int db_type; char *db_name; int db_th_type; int db_th; int db_ext_column_num;
	struct sql_column_prototype *db_sql_columns;
};

//start from column0
#define COLUMN_X_IS_STR(x) (unsigned int)(1 << x) 
#define IS_COLUMN_X_STR(types,x) ((unsigned int)(types >> x) & 1) 

enum {
	DB_NONE=0,
	DB_DEFAULT,
	DB_SYS_DETECT,
	DB_SYS_SETTING,
	DB_WIFI_DETECT,
	DB_WIFI_SETTING,
	DB_STAINFO,
	DB_NET_DETECT,
	DB_ETH_DETECT,
	DB_PORTINFO,
	DB_WIFI_DFS,
	DB_SITE_SURVEY,
	DB_CHANNEL_CHANGE,
	DB_PORT_STATUS_CHANGE,
	DB_CABLEDIAG,
	DB_PORT_STATUS_USB_CHANGE,
	DB_STAINFO_STABLE,
	DB_IPERF_SERVER,
	DB_IPERF_CLIENT,
	DB_WLC_EVENT,
	DB_WIFI_CBP,
	DB_MAX
};

enum {
	//legacy,dont change
	DIAGMODE_NONE = 0, 				//legacy,dont change
	DIAGMODE_CHKSTA = 0x1,			//legacy,dont change
	DIAGMODE_SYS_DETECT = 0x2,		//legacy,dont change
	DIAGMODE_SYS_SETTING = 0x4,		//legacy,dont change
	DIAGMODE_WIFI_DETECT = 0x8,		//legacy,dont change
	DIAGMODE_WIFI_SETTING = 0x10,	//legacy,dont change
	DIAGMODE_STAINFO = 0x20,		//legacy,dont change
	DIAGMODE_NET_DETECT = 0x40,		//legacy,dont change
	DIAGMODE_ETH_DETECT = 0x80,		//legacy,dont change
	DIAGMODE_PORTINFO = 0x100,		//legacy,dont change
	DIAGMODE_WIFI_DFS = 0x200,		//legacy,dont change
	
	//legacy,dont change

	DIAGMODE_LEGACY_MAX = 0x0FFF,




	//DIAGMODE_SITE_SURVEY = 0x400,
	DIAGMODE_MODE = 0x1000,
	DIAGMODE_STAINFO_STABLE = DIAGMODE_MODE+1,
	//DIAGMODE_MODEXXX = DIAGMODE_MODE + x-;
	DIAGMODE_ACTION = 0x2000,
	DIAGMODE_ACTION_SITE_SURVEY 	= DIAGMODE_ACTION+1,
	DIAGMODE_ACTION_SITE_SURVEY_2G 	= DIAGMODE_ACTION+2,
	DIAGMODE_ACTION_SITE_SURVEY_5G1 = DIAGMODE_ACTION+3,
	DIAGMODE_ACTION_SITE_SURVEY_5G2 = DIAGMODE_ACTION+4,
	DIAGMODE_ACTION_CABLE_DIAG 		= DIAGMODE_ACTION+5,
	DIAGMODE_ACTION_IPERF_SERVER	= DIAGMODE_ACTION+6,
	DIAGMODE_ACTION_IPERF_CLIENT	= DIAGMODE_ACTION+7,
	//DIAGMODE_ACTION
	//DIAGMODE_ACTION
	//DIAGMODE_ACTION
	DIAGMODE_EVENT = 0x4000,
	DIAGMODE_EVENT_CHANNEL_CHANGE 			= DIAGMODE_EVENT+1,
	DIAGMODE_EVENT_PORT_STATUS_CHANGE 		= DIAGMODE_EVENT+2,
	DIAGMODE_EVENT_ALL_CHAN_RADAR 			= DIAGMODE_EVENT+3,
	DIAGMODE_EVENT_CHLIST_CHANGE 			= DIAGMODE_EVENT+4,
	DIAGMODE_EVENT_PORT_STATUS_USB_CHANGE 	= DIAGMODE_EVENT+5,
	DIAGMODE_EVENT_WLC 	                    = DIAGMODE_EVENT+6,
	DIAGMODE_WIFI_CBP 	                    = DIAGMODE_EVENT+7,
	DIAGMODE_EVENT_PORT_STATUS_MOCA_CHANGE  = DIAGMODE_EVENT+8,
	//DIAGMODE_EVENT
	//DIAGMODE_SITE_SURVEY_2G = 0x800, //need modify
	//DIAGMODE_SITE_SURVEY_5G1 = 0x1000, //need modify
	//DIAGMODE_SITE_SURVEY_5G2 = 0x2000, //need modify
	//DIAGMODE_CHANNEL_CHANGE = 0x4000,
	//DIAGMODE_PORT_STATUS_CHANGE = 0x8000,
	//DIAGMODE_ALL_CHAN_RADAR = 0x10000,
	//DIAGMODE_CHLIST_CHANGE = 0x20000,
	DIAGMODE_MIX = 0xF000, // 0xF000, for indicating received pkt are mixed(multi-mode)
	DIAGMODE_MAX
};

#define CABLEDIAG_STATUS_RUN -2
//start from 0
#define CABLEDIAG_STATUS_INVALID CD_INVALID
#define CABLEDIAG_STATUS_OK CD_OK
#define CABLEDIAG_STATUS_OPEN CD_OPEN
#define CABLEDIAG_STATUS_INTRA_SHORT CD_INTRA_SHORT
#define CABLEDIAG_STATUS_INTER_SHORT CD_INTER_SHORT
#define CABLEDIAG_STATUS_ENABLED CD_ENABLED
#define CABLEDIAG_STATUS_DISABLED CD_DISABLED
#define CABLEDIAG_STATUS_NOT_SUPPORTED CD_NOT_SUPPORTED

#define DB_TYPE_MASK 0x0000FFFF
#define DB_SUBTYPE_MASK 0xFFFF0000

typedef struct _json_result json_result_t;
struct _json_result {
	char db_path[PATH_MAX];
	int row_count;
	int col_count;
	char **result;
	json_result_t *next;
};

extern void diag_log_status();
extern int get_ts_from_db_name(char *str, unsigned long *ts1, unsigned long *ts2);
extern int save_data_in_sql(const char *event, char *raw,int db_type);
extern int specific_data_on_day(unsigned long specific_ts, const char *where, int *row_count, int *field_count, char ***raw);
// Get data produced after the specific timestamp. If specific timestamp is 0, get all data of today.
extern int get_sql_on_day(unsigned long specific_ts, const char *event, const char *node_ip, const char *node_mac,
		int *row_count, int *field_count, char ***raw);
// Get data produced after the specific timestamp. If specific timestamp is 0, get all data of today.
extern int get_json_on_day(unsigned long specific_ts, const char *event, const char *node_ip, const char *node_mac,
		int *row_count, int *field_count, char ***raw);
extern int get_json_in_period(unsigned long start_ts, unsigned long end_ts, const char *event, const char *node_ip, const char *node_mac,
		json_result_t **json_result);
extern void free_json_result(json_result_t **json_result);
extern int merge_data_in_sql(const char *dst_file, const char *src_file);
#ifdef RTCONFIG_UPLOADER
extern int run_upload_file_at_ts(unsigned long ts, unsigned long ts2);
extern int run_upload_file_by_name(const char *uploaded_file);
extern int run_download_file_at_ts(unsigned long ts, unsigned long ts2);
extern int run_download_file_by_name(const char *downloaded_file);
extern int is_valid_event(const char *name);
extern unsigned long get_mem_info(char *name);
extern int special_alphasort(const void *d1, const void *d2);
extern struct CONNDIAG_DB_t *find_db_profile_by_mode_and_version(int db_mode,char *version);
extern struct CONNDIAG_DB_t *find_db_profile_by_type_and_version(int db_type,char *version);
extern int diagmode_to_dbidx(int mode);
extern int get_node_eth_port_status(char *node_mac,char **buf);
extern void free_node_eth_port_status(char **buf);
extern int query_stainfo(char *sta_mac,char **buf);
extern int get_wifi_txrxbyte_avg(char *bandmac,char *mac,double *txbyte,double *rxbyte,int diff_range);
extern int get_eth_txrxbyte_avg(int is_bh,char *mac,double *txbyte,double *rxbyte,int diff_range);
extern int get_ethphy_txrxbyte_avg(int is_bh,char *mac,double *txbyte,double *rxbyte,int diff_range);
extern int get_sta_txrxbyte_avg(char *sta_mac,char *mac,double *txbyte,double *rxbyte,int diff_range);
extern int get_staphy_txrxbyte_avg(char *sta_mac,char *mac,double *txbyte,double *rxbyte,int diff_range);
#elif defined(RTCONFIG_SWRTMESH)
extern struct CONNDIAG_DB_t *find_db_profile_by_mode_and_version(int db_mode,char *version);
extern struct CONNDIAG_DB_t *find_db_profile_by_type_and_version(int db_type,char *version);
extern int get_wifi_txrxbyte_avg(char *bandmac,char *mac,double *txbyte,double *rxbyte,int diff_range);
extern int get_eth_txrxbyte_avg(int is_bh,char *mac,double *txbyte,double *rxbyte,int diff_range);
extern int get_ethphy_txrxbyte_avg(int is_bh,char *mac,double *txbyte,double *rxbyte,int diff_range);
extern int get_sta_txrxbyte_avg(char *sta_mac,char *mac,double *txbyte,double *rxbyte,int diff_range);
extern int get_staphy_txrxbyte_avg(char *sta_mac,char *mac,double *txbyte,double *rxbyte,int diff_range);
#endif
extern int exec_force_cable_diag(char *node_mac,char *label_name);
extern int exec_wifi_dfs_diag(char *json_data);
#ifdef RTCONFIG_CD_IPERF
extern int exec_iperf(char* caller, char *server_mac,char *client_mac);
#endif

extern int get_node_eth_port_status(char *node_mac,char **buf);
extern void free_node_eth_port_status(char **buf);
extern int query_stainfo(char *sta_mac,char **buf);
extern void free_stainfo(char **buf);

struct json_object* get_byte_field_string_json_object(unsigned char value, char *buf, int buf_len);
struct json_object* get_int_field_string_json_object(int value, char *buf, int buf_len);
struct json_object* get_uint_field_string_json_object(unsigned int value, char *buf, int buf_len);
struct json_object* get_uint64_field_string_json_object(unsigned long long value, char *buf, int buf_len);
struct json_object* get_rate_field_string_json_object(double value, char *buf, int buf_len);
extern int _get_node_eth_port_status(char *node_mac,char **buf);

#ifdef RTCONFIG_AWSIOT
extern int wifi_dfs_on_all_channels_process();
#endif

extern void ATE_port_status(int verbose, phy_info_list *list);
extern int GetPhyStatus(int verbose, phy_info_list *list);
extern void update_port_status_table(void);
#if defined(RTCONFIG_USB)
#if 1
void get_usb_devices_by_usb_port(usb_device_info_t device_list[], int max_devices, int usb_port);
#else
void get_usb_devices(usb_device_info_t **device_list);
void free_usb_devices(usb_device_info_t **device_list);
#endif
#endif
extern int libswrt_get_cpu_temp();
extern int libswrt_get_wifi_temp(int unit);
extern void get_sys_detect();
extern void libswrt_get_stainfo();
extern void libswrt_eth_detect();
extern int lantiq_getSTAInfo(int unit, struct stainfo **sta_info);
extern int qca_getSTAInfo(int unit, struct stainfo **sta_info);
extern int mtk_getSTAInfo(int unit, struct stainfo **sta_info);
extern int brcm_getSTAInfo(int unit, struct stainfo **sta_info);
#endif
