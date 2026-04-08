/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#include <asm/types.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/wireless.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/wait.h>
#include "driver_api.h"
#include "wpsctrl.h"

#include "drvhlpr.h"
#include "ledsctrl.h"
#include "mtlk_osal.h"

#include "iniparseraux.h"
#include "mtlkcontainer.h"

#define LOG_LOCAL_GID   GID_LEDSCTRL
#define LOG_LOCAL_FID   1

#define MTLK_LED_STRING_BUFFER_SIZE 100
#define SAFE_LEN(x) (wave_strlen(x, MTLK_LED_STRING_BUFFER_SIZE-1))
#define SAFE_SIZEOF(x) (sizeof(x) > MTLK_LED_STRING_BUFFER_SIZE ? (MTLK_LED_STRING_BUFFER_SIZE-1) : sizeof(x))
#define MTLK_NEW_LINE_ADDITION 1
#define MTLK_LED_ADDITION 1
#define MTLK_IFCMD_DATALEN 4096
#define MTLK_MAX_CONNECTION             16

#define DRV_RETRY_CONNECTION_SEC 1
#define MTLK_CHAR_OFFSET_HW_VAL          1
#define MTLK_CHAR_OFFSET_CONF_PARAM 2

uint32 u32TxCountLast[MTLK_MAX_CONNECTION];
uint32 u32RxCountLast[MTLK_MAX_CONNECTION];

/*only four errors, but error code do not start from 0 (see wps_led_status)*/
char wps_errors[][2] = {"0","0","0","0","0","0","0","0"};

char wlan_link_on[]             = "0";
char wlan_link_off[]            = "0";
char wlan_link_connecting[]     = "0";
char wlan_link_scan[]           = "0";
char wlan_link_data[]           = "0";
char security_on[]              = "0";
char security_off[]             = "0";
char security_wep[]             = "0";
char wps_idle[]                 = "0";
char wps_in_process[]           = "0";
char wps_error_overlap_err[]    = "0";
char wps_error_timeout_err[]    = "0";
char wps_error_stop_pin_err[]   = "0";
char wps_error_stop_err[]       = "0";


static int security_wep_timeout = 120; /*default, if not in conf, parser leave as is*/
static int wps_error_timeout = 120; /*default, if not in conf, parser leave as is*/
static int wps_activated_timeout = 150; /*default, if not in conf, parser leave as is*/

#define MTLK_CONF_BIN_PARAM_MAX 3
char *str_bin_array[] = {"security_wep_timeout",
                         "wps_error_timeout",
                         "wps_activated_timeout"};

#define MTLK_CONF_PARAM_MAX 14
char *str_array[] = {"wlan_link_on",
                     "wlan_link_off",
                     "wlan_link_connecting",
                     "wlan_link_scan",
                     "wlan_link_data",
                     "security_on",
                     "security_off",
                     "security_wep",
                     "wps_idle",
                     "wps_in_process",
                     "wps_error_overlap_err",
                     "wps_error_timeout_err",
                     "wps_error_stop_pin_err",
                     "wps_error_stop_err"};


static mtlk_osal_thread_t ledsctrl_thread;
static mtlk_osal_event_t  ledsctrl_thread_stop;

/* if adding leds, need to update define
* at the moment two link leds and 3 security 
*/

#define MTLK_STRING_BUFFER_LINE_SIZE    (MTLK_LED_STRING_BUFFER_SIZE<<1)

#define MTLK_TOTAL_LINK_LEDS            2
#define MTLK_SECURITY_LEDS_INDEX_MIN    2
#define MTLK_TOTAL_SECURITY_LEDS        3
#define MTLK_SECURITY_LEDS_MAX          (MTLK_TOTAL_SECURITY_LEDS+MTLK_SECURITY_LEDS_INDEX_MIN)
#define MTLK_TOTAL_LEDS                 (MTLK_TOTAL_LINK_LEDS+MTLK_TOTAL_SECURITY_LEDS)


#define FORCE_SET_SECURE_LEDS 0


#define MTLK_READ_ASSCI 1

#define MTLK_DATA_LED_OFF            '0'
#define MTLK_SECURITY_LED_OFF        '0'
#define MTLK_SECURITY_LED_ON         '1'
#define MTLK_SECURITY_LED_BLINK_IN_PROG '6'
#define MTLK_SECURITY_LED_BLINK_ERR_1 '3'
#define MTLK_SECURITY_LED_BLINK_ERR_2 '4'

#define MTLK_DATA_LED_ON             '1'
#define MTLK_DATA_LED_BLINK_SLOW     '2'
#define MTLK_DATA_LED_BLINK_FAST     '5'

/* Note:
The drvhlpr writes all events to a file for the use of the WPS scripts,
but not all the events are in use by the drvhlpr, for example:
Event 6 (MTLK_WPS_NOT_IN_USE_EVENT)
*/
enum wps_led_status {
  MTLK_WPS_IDLE,
  MTLK_WPS_ACTIVATED,
  MTLK_WPS_FINISH_OK,
  MTLK_WPS_STOP_OVERLAP,
  MTLK_WPS_STOP_ERROR,
  MTLK_WPS_TIMEOUT,
  MTLK_WPS_REGISTER,
  MTLK_WPS_STOP_PIN_ERROR
};


static struct {
         char entry[MTLK_LED_STRING_BUFFER_SIZE];
         char file[MTLK_LED_STRING_BUFFER_SIZE];
         int  index;
       } leds_config[] = {
{"WLAN_LED_5_2",    "null", 0/*band_52_led*/},
{"WLAN_LED_2_4",    "null", 1/*band_24_led*/},
{"Security_LED",    "null", 0/*security_led*/},
{"WPS_activity_LED","null", 1/*wps_activity_led*/},
{"WPS_error_LED",   "null", 2/*wps_error_led*/}
};

/* When changing leds_config struct,
   make sure to update these params/defines 
*/
#define WLAN_LED_5_2     0
#define WLAN_LED_2_4     1
#define SECURITY_LED     2
#define WPS_ACTIVITY_LED 3
#define WPS_ERROR_LED    4
int band_52_led          = WLAN_LED_5_2;
int band_24_led          = WLAN_LED_2_4;
int security_led         = SECURITY_LED;
int wps_activity_led     = WPS_ACTIVITY_LED;
int wps_error_led        = WPS_ERROR_LED;

//uint8  wps_status = 0;
time_t current_time = 0;
int    error_start = 0;
time_t error_start_time = 0;
time_t activate_start_time = 0;
time_t activate_current_time = 0;
/*wep*/
int    wep_start = 0;
time_t wep_start_time = 0;
time_t wep_current_time = 0;
int    wep_timeout = 0;
#define WEP_SECURITY_CHANGE

int leds_map_mask;

char security_leds_status[MTLK_TOTAL_LEDS][2];


const char leds_conf_file[] = "/tmp/HW.ini";
const char tmp_wpa_stat_path[] = "/tmp/tmp_wpa_state_mirr";


//#define MTLK_DEBUG_CHARS

#define MTLK_WPS_ERROR_BLINK_TIME    120
#define MTLK_WPS_ACTIVATE_TIME       150
#define MTLK_WEP_BLINK_TIME          120

#define MTLK_MASK_SECURITY_NO_CONFLICT       0
#define MTLK_MASK_SECURITY_CONFLICT_ALL      7
#define MTLK_MASK_SECURITY_CONFLICT_WPS      2
#define MTLK_MASK_ACTIVITY_CONFLICT_WPS_ERR  4

#define MTLK_ON_WPS_TIMEOUT_OFF          0
#define MTLK_ON_WPS_FINISH_OK            1
#define MTLK_ON_WPS_ERROR                2

#define MTLK_LINK_UP                 1
#define MTLK_AP                      0x32 

#if WIRELESS_EXT < 18
#define IW_ENCODE_ALG_NONE      0
#define IW_ENCODE_ALG_WEP       1
#define IW_ENCODE_ALG_TKIP      2
#define IW_ENCODE_ALG_CCMP      3
#endif

static uint8  wps_status=0;       
static uint32 cfg_mask=0;         
static int led_resolution=1;      
static char *link_status_script_path = "";
static char *link_script_path = "";
static char network_type[2]="0";    
static const char *params_conf_file = NULL;


int leds_parse_configs(void);
void leds_config_security_map(void);

static int32 leds_thrdatamain(mtlk_handle_t context);

static int security_mode = 0;
static int reconf_security = -1;


static int 
load_security_cfg (const char *cfg_fname)
{
  int         res  = MTLK_ERR_UNKNOWN;
  dictionary *dict = NULL;

  MTLK_ASSERT(cfg_fname != NULL);
  MTLK_ASSERT(strlen(cfg_fname) != 0);

  dict = iniparser_load(cfg_fname);
  if (!dict) {
    ELOG_S("Can't load CFG file (%s)", cfg_fname);
    res = MTLK_ERR_PARAMS;
    goto end;
  }

  security_mode = iniparser_getint(dict, 
                                   iniparser_aux_gkey_const("NonProcSecurityMode"), 
                                   security_mode);
  ILOG2_D("security_mode = %d",security_mode);
  res  = MTLK_ERR_OK;
end:
  if (dict) {
    iniparser_freedict(dict);
  }
  return res;  
}


int
prepare_leds (void)
{
  int retval = -1;
  int i;

  for (i=0; i<ARRAY_SIZE(leds_config); i++) {
    wave_strcopy(security_leds_status[i], "0", sizeof(security_leds_status[i]));
    memset (leds_config[i].file, 0, sizeof(leds_config[i].file));
  }
  
  /**************************************************************
   * driver is ready, handle configurations
   * 
   * Do only one time:
   * 1. read configuration from HW.ini
   * 2. get network type AP/STA
   * 3. map leds to gpio (not on Dongle: to support all leds/gpio possibilities)
   **************************************************************/
  if (leds_parse_configs() != 0) {
    ELOG_V("return with error from leds_parse_configs()");
    goto end;
  }
  /* TODO: if drvhlpr handles leds on Dongle - enable the mapping
   */
  leds_config_security_map();

  retval = 0;

end:
  return retval;
}


static BOOL
create_led_ctrl_task (void)
{
  int ret_val = 0;

  ret_val = mtlk_osal_event_init(&ledsctrl_thread_stop);
  if (ret_val != MTLK_ERR_OK) {
    ELOG_D("ledsctrl_thread stop event init failed! (err=%d)", ret_val);
    goto evt_init_failed;
  }

  ret_val = mtlk_osal_thread_init(&ledsctrl_thread);
  if (ret_val != MTLK_ERR_OK) {
    ELOG_D("ledsctrl_thread init failed! (err=%d)", ret_val);
    goto thread_init_failed;
  }

  ret_val = mtlk_osal_thread_run(&ledsctrl_thread,
                                 leds_thrdatamain,
                                 HANDLE_T(0));
  if (ret_val != MTLK_ERR_OK) {
    ELOG_D("ledsctrl_thread start failed! (err=%d)", ret_val);
    goto thread_run_failed;
  }

  return TRUE;

thread_run_failed:
  mtlk_osal_thread_cleanup(&ledsctrl_thread);
thread_init_failed:
  mtlk_osal_event_cleanup(&ledsctrl_thread_stop);
evt_init_failed:
  return FALSE;
}

static void
delete_led_ctrl_task (void)
{
  mtlk_osal_event_set(&ledsctrl_thread_stop);

  mtlk_osal_thread_wait(&ledsctrl_thread, NULL);
  mtlk_osal_thread_cleanup(&ledsctrl_thread);
  mtlk_osal_event_cleanup(&ledsctrl_thread_stop);
}

static mtlk_handle_t
leds_ctrl_start (void)
{
  int res = 0; /*! res is ctx pass to leds_ctrl_stop by container !*/

  if (0 != prepare_leds()) {
    goto end;
  }

  res = create_led_ctrl_task();

end:
  return HANDLE_T(res);
}


static void
leds_ctrl_stop (mtlk_handle_t ctx)
{
  ILOG1_V("call pthread_cancel to close led thread\n");
  delete_led_ctrl_task();
}


const mtlk_component_api_t
irb_leds_ctrl_api = {
  leds_ctrl_start,
  NULL,
  leds_ctrl_stop
};


static int map_str_to_code(char *ptr, const char *value)
{
  if (strcmp("led_off", value) == 0) {
    ptr[0] = MTLK_DATA_LED_OFF;
  }
  else if (strcmp("led_on", value) == 0) {
    ptr[0] = MTLK_DATA_LED_ON;
  }
  else if (strcmp("led_pattern_1s_interval", value) == 0) {
    ptr[0] = MTLK_DATA_LED_BLINK_SLOW;
  }
  else if (strcmp("led_pattern_1ms_interval_end_5ms_off", value) == 0) {
    ptr[0] = MTLK_SECURITY_LED_BLINK_ERR_1;
  }
  else if (strcmp("led_pattern_1ms_interval", value) == 0) {
    ptr[0] = MTLK_SECURITY_LED_BLINK_ERR_2;
  }
  else if (strcmp("led_pattern_3ms_interval", value) == 0) {
    ptr[0] = MTLK_DATA_LED_BLINK_FAST;
  }
  else {
    ELOG_S("ERROR, param %s was not found in drvhlpr.conf",value);
    return MTLK_ERR_PARAMS;
  }

  return MTLK_ERR_OK;
}


/*************************************************************
* map_key_to_bin_param
*
* return param that matches the string in key
*/
int *map_key_to_bin_param(const char *key)
{
  if (strcmp("security_wep_timeout", key)==0)
    return &security_wep_timeout;
  if (strcmp("wps_error_timeout", key)==0)
    return &wps_error_timeout;
  if (strcmp("wps_activated_timeout", key)==0)
    return &wps_activated_timeout;

  ELOG_V("no bin param found !!");
  return 0;
}


/*************************************************************
* map_key_to_param
*
* return param that matches the string in key
*/
static char *map_key_to_param(const char *key)
{
  if (strcmp("wlan_link_on", key)==0)
    return wlan_link_on;
  if (strcmp("wlan_link_off", key)==0)
    return wlan_link_off;
  if (strcmp("wlan_link_connecting", key)==0)
    return wlan_link_connecting;
  if (strcmp("wlan_link_scan", key)==0)
    return wlan_link_scan;
  if (strcmp("wlan_link_data", key)==0)
    return wlan_link_data;
  if (strcmp("security_on", key)==0)
    return security_on;
  if (strcmp("security_off", key)==0)
    return security_off;
  if (strcmp("security_wep", key)==0)
    return security_wep;
  if (strcmp("wps_idle", key)==0)
    return wps_idle;
  if (strcmp("wps_in_process", key)==0)
    return wps_in_process;
  if (strcmp("wps_error_overlap_err", key)==0)
    return wps_error_overlap_err;
  if (strcmp("wps_error_timeout_err", key)==0)
    return wps_error_timeout_err;
  if (strcmp("wps_error_stop_pin_err", key)==0)
    return wps_error_stop_pin_err;
  if (strcmp("wps_error_stop_err", key)==0)
    return wps_error_stop_err;

  ELOG_V("no param found !!");
  return 0;
}

static void set_error_array(const char *key, const char *value)
{
  if (strcmp("wps_error_overlap_err", key)==0) {
    wps_errors[MTLK_WPS_STOP_OVERLAP][0] = wps_error_overlap_err[0];
    ILOG2_S("wps_errors[MTLK_WPS_STOP_OVERLAP] = %s",wps_errors[MTLK_WPS_STOP_OVERLAP]);
  }
  if (strcmp("wps_error_timeout_err", key)==0) {
    wps_errors[MTLK_WPS_TIMEOUT][0] = wps_error_timeout_err[0];
    ILOG2_S("wps_errors[MTLK_WPS_TIMEOUT] = %s",wps_errors[MTLK_WPS_TIMEOUT]);
  }
  if (strcmp("wps_error_stop_pin_err", key)==0) {
    wps_errors[MTLK_WPS_STOP_PIN_ERROR][0] = wps_error_stop_pin_err[0];
    ILOG2_S("wps_errors[MTLK_WPS_STOP_PIN_ERROR] = %s",wps_errors[MTLK_WPS_STOP_PIN_ERROR]);
  }
  if (strcmp("wps_error_stop_err", key)==0) {
    wps_errors[MTLK_WPS_STOP_ERROR][0] = wps_error_stop_err[0];
    ILOG2_S("wps_errors[MTLK_WPS_STOP_ERROR] = %s",wps_errors[MTLK_WPS_STOP_ERROR]);
  }
}

/***************************************************************************************
** APIs
***************************************************************************************/
/***********************************************
* set wps_status param
*/
void leds_ctrl_set_wps_stat(uint8  val)
{
  wps_status = val;
  ILOG2_D("leds API: wps_status = %d",wps_status);
}


/***********************************************
* set cfg_mask param
*/
void leds_ctrl_set_mask(uint32 val)
{
  cfg_mask = val;
  ILOG2_D("leds API: cfg_mask = %d",cfg_mask);
}


/***********************************************
* set led_resolution param
*/
void leds_ctrl_set_leds_resolution(int val)
{
  led_resolution = val;
  ILOG2_D("leds API: led_resolution = %d",led_resolution);
}


/***********************************************
* set network_type param
*/
int leds_ctrl_set_network_type(const char* str)
{
  int size = strlen(str) + 1;

  if (size <= sizeof(network_type)) {
    wave_memcpy(network_type, sizeof(network_type), str, size);
    ILOG2_S("leds API: network_type = %s",network_type);
    return MTLK_ERR_OK;
  }

  return MTLK_ERR_BUF_TOO_SMALL;
}

/***********************************************
* set link_script_path param
*/
int leds_ctrl_set_link_path(const char* str)
{
  return drvhlpr_set_script_path (&link_script_path, str);
}


/***********************************************
* set link_status_script_path param
*/
int leds_ctrl_set_link_status_path(const char* str)
{
  return drvhlpr_set_script_path (&link_status_script_path, str);
}


/*************************************************************
* leds_ctrl_get_bin_string
*
* returns number of bin params in conf file
*/
char *leds_ctrl_get_bin_string(void)
{
  static int index = 0;
  
  ILOG2_D("index = %d",index);
  if (index < MTLK_CONF_BIN_PARAM_MAX) {
    ILOG2_S("str_bin_array[index] = %s",str_bin_array[index]);
    return str_bin_array[index++];
  }
  ILOG2_V("end of bin params");
  index = 0;

  return NULL;
}


/*************************************************************
* leds_ctrl_get_string
*
* returns number of params in conf file
*/
char *leds_ctrl_get_string(void)
{
  static int index = 0;
  
  if (index < MTLK_CONF_PARAM_MAX) {
    ILOG2_S("leds_ctrl_get_string: str_array entry = %s",str_array[index]);
    return str_array[index++];
  }
  index = 0;

  return NULL;
}


/*************************************************************
* leds_ctrl_set_bin_param
*
* sets param with value from conf file.
* it maps the key to the parameter, and call to set it with the value.
*
* key - string of param as in the conf file
* param - value from conf file
*/
void leds_ctrl_set_bin_param(const char *key, const int value)
{
  int *param = map_key_to_bin_param(key);
  if (param) {
    *param = value;
    ILOG2_DD("value = %d, *param = %d",value,*param);
  }
  else
    ELOG_V("param was not found");
}


/*************************************************************
* leds_ctrl_set_param
*
* sets param with value from conf file.
* it maps the key to the parameter, and call to set it with the value.
*
* key - string of param as in the conf file
* param - value from conf file
*/
void leds_ctrl_set_param(const char *key, const char *value)
{
  char *param;;
  param = map_key_to_param(key);
  if (param) {
    if (map_str_to_code(param, value)) {
      ELOG_V("str to code mapping error");
    }
    ILOG2_SS("test...value = %s, param = %s",value,param);
    set_error_array(key, value);
    ILOG2_SS("value = %s, param = %s",value,param);
  }
  else
    ELOG_V("param was not found");
}

void leds_ctrl_set_security_mode(int val)
{
  security_mode = val;
  ILOG2_D("leds API: security_mode = %d",security_mode);
}

void leds_ctrl_set_reconf_security(int val)
{
  reconf_security = val;
  ILOG2_D("leds API: reconf_security = %d",reconf_security);
}

void leds_ctrl_set_conf_file(const char* str)
{
  params_conf_file = str;
}

/***************************************************************************************
** APIs END
***************************************************************************************/

static void
dataex_report_error(int ioctl_rslt, char *pdata)
{
  switch (ioctl_rslt) {
  case WE_GEN_DATAEX_FAIL:
    ELOG_V("Command failed, please check driver logs");
    break;
  case WE_GEN_DATAEX_PROTO_MISMATCH:
    ELOG_DD("Protocol version mismatch (got %lu, expected %u)",
      (unsigned long) &pdata[0], WE_GEN_DATAEX_PROTO_VER);
    break;
  case WE_GEN_DATAEX_UNKNOWN_CMD:
    ELOG_V("Driver cannot process this command");
    break;
  case WE_GEN_DATAEX_DATABUF_TOO_SMALL:
    ELOG_V("Data buffer supplied is too small");
    break;
  default:
    ELOG_V("Unknown data exchange error");
    break;
  }
}

/***********************************************************************
* leds_control
* 
* Description: get the link led status
*   leds_config[i].file contains path to proc - the value is written to proc by driver.
*   The "null" string in leds_config[i].file indicates that no need to write to Link LED.
*/
void
leds_control(char * val, WE_GEN_DATAEX_STATUS* pstatus)
{
  FILE *fp;
  int i;

  if (!(cfg_mask & DHFLAG_NO_WRITE_LED) &&
      pstatus->frequency_band != MTLK_HW_BAND_NONE) {
    for (i = 0; i < MTLK_SECURITY_LEDS_INDEX_MIN; i++) {
      ILOG2_DSDS("i = %d, val = %s, frequency_band = %d, file = %s"
              ,i
              ,val
              ,pstatus->frequency_band
              ,leds_config[i].file);

      if (memcmp(leds_config[i].file, "null", SAFE_SIZEOF("null")-1) == 0) {
        /**************************************
        * null - do nothing
        */
        ILOG2_V("null, do nothing");
      }
      else {
        /**************************************
        * path to proc - direct write to LED
        */
        size_t len;
        ILOG2_DD("direct write to LED, pstatus->frequency_band=%d, leds_config[i].index=%d"
                 ,pstatus->frequency_band
                 ,leds_config[i].index);
        if (pstatus->frequency_band == leds_config[i].index) {
          /*supported band (only one band can be supported)*/
          fp = fopen(leds_config[i].file,"w");
          if (fp == NULL)
          {
            WLOG_V("Error in open LED proc");
            return;
          }

          len = SAFE_LEN(val);
          if (fwrite(val, 1, len, fp) < len)
            WLOG_V("Error write to LED proc");
    
          fclose(fp);
        }
      }
    } //for
  } //if
  else {
    ILOG2_V("masked or band was not set");
  }
}

/********************************************************************************
* leds_control_security
*
* Description: write the value to gpio file.
* Note- the i starts from after LINK leds.
*       leds_config[i].index points to the mapped entry,
*       and is aligned with the value as set in parse_security_status.
*       The new index starts from 0, therefore have + MTLK_SECURITY_LEDS_INDEX_MIN
*       to point at the security range.

* leds_config[MTLK_SECURITY_LEDS_INDEX_MIN]   //Security_LED
* leds_config[1+MTLK_SECURITY_LEDS_INDEX_MIN] //WPS_activity_LED
* leds_config[2+MTLK_SECURITY_LEDS_INDEX_MIN] //WPS_error_LED
*/
void
leds_control_security(void)
{
  FILE *fp = NULL;
  int i = 0;

  if (!(cfg_mask & DHFLAG_NO_WRITE_LED)) {
    /*write value to LED*/
    for (i = MTLK_SECURITY_LEDS_INDEX_MIN; i < MTLK_SECURITY_LEDS_MAX; i++) {
      ILOG2_DDSSDS("i = %d, index = %d, file origin = %s, file new index = %s, val[%d] = %s"
              ,i
              ,leds_config[i].index
              ,leds_config[i].file
              ,leds_config[leds_config[i].index+MTLK_SECURITY_LEDS_INDEX_MIN].file
              ,i
              ,security_leds_status[leds_config[i].index]);
      
      if (memcmp(leds_config[i].file, "null", SAFE_SIZEOF("null")-1) != 0) {
        fp = fopen(leds_config[leds_config[i].index+MTLK_SECURITY_LEDS_INDEX_MIN].file,"w");    
        if (fp == NULL)
        {
          WLOG_S("Error in open %s LED proc"
                   ,leds_config[leds_config[i].index+MTLK_SECURITY_LEDS_INDEX_MIN].file);
          return;
        }
        if (fwrite(security_leds_status[leds_config[i].index], 1, 2, fp) <
                   strlen(security_leds_status[leds_config[i].index]))
          WLOG_S("Error write to %s LED proc"
                   ,leds_config[leds_config[i].index+MTLK_SECURITY_LEDS_INDEX_MIN].file);
    
        fclose(fp);
      } //if (memcmp..
    } //for
  } //if
  else
    ILOG2_D("cfg_mask = 0x%x", cfg_mask);
}

/***********************************************************************
* parse_status
* 
* Description: parse link related params and return the led status
*   scan started overwrite also for if there is another link with data.
*/
unsigned char
parse_status(WE_GEN_DATAEX_DEVICE_STATUS* devst, WE_GEN_DATAEX_STATUS* pstatus, int sta)
{
  if ((devst->u32TxCount != u32TxCountLast[sta]) ||
      (devst->u32RxCount != u32RxCountLast[sta]) ||
       (pstatus->scan_started)) {
    u32TxCountLast[sta] = devst->u32TxCount;
    u32RxCountLast[sta] = devst->u32RxCount;
    ILOG2_DD("sta=%d, data or scan, pstatus->scan_started=%d",sta,pstatus->scan_started);
    if (pstatus->scan_started)
      return wlan_link_scan[0];
    else {
      ILOG2_DD("Rx=%d, Tx=%d",devst->u32RxCount,devst->u32TxCount);
      return wlan_link_data[0];
    }
  }
  else {
    ILOG2_V("no data");
    if (pstatus->scan_started)
      return wlan_link_scan[0];
    else
      return wlan_link_on[0];
  }
}

/********************************************************************************
* security_led_no_conflict
*
* Description:
            return 1 if can clear security led, else return 0.
* Algo:
            if not in active timeout (of wps notification) return 1,
            or
            if in active notification, and security led has no conflict with activity
            led return 1,
            else, return 0
*/
int security_led_no_conflict()
{
  if (wep_timeout == MTLK_ON_WPS_TIMEOUT_OFF || leds_map_mask == MTLK_MASK_SECURITY_NO_CONFLICT)
    return 1;
  else if ((wep_timeout == MTLK_ON_WPS_FINISH_OK && leds_map_mask == MTLK_MASK_SECURITY_CONFLICT_WPS) || 
      (wep_timeout == MTLK_ON_WPS_ERROR && leds_map_mask == MTLK_MASK_ACTIVITY_CONFLICT_WPS_ERR))
    return 0;
  else
    return 1;
}

/********************************************************************************
* parse_security_status
*
* Description: set leds with the value when event was found. Else, old value left.
*
* Note:
* security_led, wps_activity_led and wps_error_led hold indexes that were evaluated
* in Init time, mapped according to total number of gpios.
* security_led = leds_config[SECURITY_LED].index;
* wps_activity_led = leds_config[WPS_ACTIVITY_LED].index;
* wps_error_led = leds_config[WPS_ERROR_LED].index;
*
* The implementation below result with:
*    1. seperate led to WPS and security - security always reflect security status
        of the active link (STA), or any of the links (AP-if any link in security, it is ON)
*    2. same led for WPS and security - the WPS overwrite the security led as required,
*       with the exception of:
             At the end of the error indication time- if security on, set led.
             New secured link while error indication- stop error indication, set led
             (new secured connection elstablished not via WPS)

* Requirements:
    1. different blink paterns for the different WPS errors
    2. WPE security is equal to open mode in terms of LEDs handling
    3. timeout on error blink (or led-on) indication
    4. timeout on WPS led-on success indication
    5. in regards to 2 & 3:
       wep/open clears the security_led, but if same led (gpb237) should wait for timeout expire
       when activated.
*/
int
parse_security_status(WE_GEN_DATAEX_STATUS* psecurity_status
                      ,uint8 wps_status_mirror
                      ,int *do_leds)
{
  static int8 wps_status_before = -1;
  static int8 security_status_before = -1;
  static int start_activate_timer = 1;
  int result = 0;

  if (--reconf_security == 0) {
    if (load_security_cfg(params_conf_file) != MTLK_ERR_OK) {
      ILOG2_S("error reading security param from %s",params_conf_file);
      reconf_security = MTLK_DELAY_READ_SECURITY_CONF;
    }
  }
  
  /*get to SM only when change. Therefore, special handling for error event*/
  if ((wps_status_mirror == wps_status_before) &&
      (psecurity_status->security_on == security_status_before)) {
    *do_leds = 0;
    if ((wps_status_mirror != MTLK_WPS_ACTIVATED) && (wps_status_mirror != MTLK_WPS_REGISTER)) {
        start_activate_timer = 1;
    }
    ILOG2_DD("no event change, wps_status_mirror = %d, wps_status_before = %d",wps_status_mirror,wps_status_before);
  }
  else {
    /*Note:
      during WPS the security may change, therefore setting of security_leds_status may take place in leds*/
    *do_leds = 1;
    ILOG2_DDDD("event change: wps_status_mirror=%d (before=%d), security_on=%d (before=%d)"
             ,wps_status_mirror
             ,wps_status_before
             ,psecurity_status->security_on
             ,security_status_before);
  }

  /*set security. If same led as WPS, WPS overwrite it*/
  if (psecurity_status->security_on && psecurity_status->wep_enabled == 0)
    wave_strcopy(security_leds_status[security_led], security_on, sizeof(security_leds_status[security_led]));
  else if (security_led_no_conflict()) {
    /*clear led only after timeout, or security led is not conflict with other leds*/
    wave_strcopy(security_leds_status[security_led], security_off, sizeof(security_leds_status[security_led]));
    ILOG2_V("wep_timeout, MTLK_SECURITY_LED_OFF");
  }

  switch (wps_status_mirror) {
    case MTLK_WPS_IDLE:
      ILOG2_V("MTLK_WPS_IDLE");
      wave_strcopy(security_leds_status[wps_error_led], security_off, sizeof(security_leds_status[wps_error_led]));
      wave_strcopy(security_leds_status[wps_activity_led], security_off, sizeof(security_leds_status[wps_activity_led]));
      /*if case same led: overwrite as Security prio > WPS prio (AP)*/
      if (psecurity_status->security_on && psecurity_status->wep_enabled == 0)
        wave_strcopy(security_leds_status[security_led], security_on, sizeof(security_leds_status[security_led]));
    break;
    
    case MTLK_WPS_ACTIVATED:
    case MTLK_WPS_REGISTER:
      /*blue blink, orange off, if new session, overwrite error led
        (all in same led overwrite security as well)*/
      ILOG2_V("MTLK_WPS_ACTIVATED");
      wep_start = 0;
      error_start = 0;
      wave_strcopy(security_leds_status[wps_error_led], security_off, sizeof(security_leds_status[wps_error_led]));
      wave_strcopy(security_leds_status[wps_activity_led], wps_in_process, sizeof(security_leds_status[wps_activity_led]));
      if (start_activate_timer) {
        /*start timer*/
        start_activate_timer = 0;
        activate_start_time = time (NULL);
        ILOG2_D("start timer in activate, time = %d",activate_start_time);
        ILOG2_SS("set led to code %s (%s)",wps_in_process, (security_leds_status[wps_activity_led]));
      }
       activate_current_time = time (NULL);
       ILOG2_D("in activate condition, time left = %d"
                ,(activate_start_time+wps_activated_timeout - activate_current_time));
      if (activate_current_time - activate_start_time >= wps_activated_timeout) {
        /*report error -simulate error as got WPS indication (path for WPS crash)*/
        ILOG2_V("activate T.O. report error");
        wps_status = MTLK_WPS_STOP_ERROR;
      }
    break;
    
    case MTLK_WPS_FINISH_OK:
      ILOG2_V("MTLK_WPS_FINISH_OK");
      /*wps_error led always OFF
        wps led conditional !!! (if wps=security = ON, else OFF)
        security led always ON
      */
      if (psecurity_status->wep_enabled == 0 && psecurity_status->security_on) {
        /*it is WPA security*/
        wave_strcopy(security_leds_status[wps_error_led], security_off, sizeof(security_leds_status[wps_error_led]));
        if (wps_activity_led == security_led)
          wave_strcopy(security_leds_status[wps_activity_led], security_on, sizeof(security_leds_status[wps_activity_led]));
        else
          wave_strcopy(security_leds_status[wps_activity_led], security_off, sizeof(security_leds_status[wps_activity_led]));
        ILOG2_S("security_on, set led to code %s",security_leds_status[wps_activity_led]);
      }
      else {
        /*it is WEP security or open,
          do not reset error led, done in MTLK_WPS_ACTIVATED
          */
        //strcpy(security_leds_status[wps_error_led], security_off); - error, clear wps status if same led
        if (wep_start == 0) {
          ILOG2_V("wep security mode or open, start timer");
          wep_timeout = MTLK_ON_WPS_FINISH_OK;
          /*wep_enabled, start timer*/
          wep_start = 1;
          wep_start_time = time (NULL);
          wave_strcopy(security_leds_status[wps_activity_led], security_on, sizeof(security_leds_status[wps_activity_led]));
        }
        else {
          wep_current_time = time (NULL);
          /*set the wps_activity_led param for if do_leds is set and there is conflict on leds*/
          wave_strcopy(security_leds_status[wps_activity_led], security_on, sizeof(security_leds_status[wps_activity_led]));
          if (wep_current_time - wep_start_time >= security_wep_timeout) {
            ILOG2_V("wep time elapse, change led status :-) ");
            wep_timeout = MTLK_ON_WPS_TIMEOUT_OFF;
            if (wps_activity_led == security_led)
              wave_strcopy(security_leds_status[wps_activity_led], security_off, sizeof(security_leds_status[wps_activity_led]));
            else
              wave_strcopy(security_leds_status[security_led], security_off, sizeof(security_leds_status[security_led]));
            /*no new event, therefore need to set flag (was not set before)*/
            *do_leds = 1; 
          }
        }
      }
    break;
    
    case MTLK_WPS_STOP_ERROR:
    case MTLK_WPS_STOP_OVERLAP:
    case MTLK_WPS_TIMEOUT:
    case MTLK_WPS_STOP_PIN_ERROR:
      ILOG2_V("MTLK_WPS_X (TIMEOUT/STOP_ERROR/STOP_OVERLAP)");
      if (error_start == 0) {
#if 0
        if (wps_status_mirror != wps_status_before) {
#endif
          /*start error led
            security according to security status, handled above 
            wps (if wps != security = OFF, else according to security status)
            wps_error ON
          */
          error_start = 1;
          wep_timeout = MTLK_ON_WPS_ERROR; /*wep should not change led status during error indication*/
          error_start_time = time (NULL);
          //blue off, orange blink (1, 2 ?)
          if (wps_activity_led != security_led)
            wave_strcopy(security_leds_status[wps_activity_led], security_off, sizeof(security_leds_status[wps_activity_led]));
          ILOG2_DS("MTLK_WPS_ERROR, entry = %i, code = %s",wps_status_mirror, wps_errors[wps_status_mirror]);
          wave_strcopy(security_leds_status[wps_activity_led], wps_errors[wps_status_mirror], sizeof(security_leds_status[wps_activity_led]));
          ILOG2_S("set security_leds_status to = %s",security_leds_status[wps_activity_led]);
          ILOG2_D("time error_start_time = %ld",error_start_time);
          ILOG2_D("wps_error_timeout = %d",wps_error_timeout);
#if 0
        }
#endif
      }
      else {
        /*error leds are blinking*/
        current_time = time (NULL);
        ILOG2_D("in error condition, time left = %d"
                 ,(error_start_time+wps_error_timeout - current_time));
        if (current_time - error_start_time >= wps_error_timeout) {
          ILOG2_V("time elaps, change led status");
          /*stop error leds, test if need to set security led.
            wps_error OFF
            security (if wps_error=security = OK, else according to security status)
            wps N/A
          */
          wave_strcopy(security_leds_status[wps_error_led], security_off, sizeof(security_leds_status[wps_error_led]));
          /* if wps=security, was off, and it is set already in the security handling above
          */
          error_start = 0;
          /*no new event, therefore need to set flag (was not set before)*/
          *do_leds = 1; 
        }
        if ((security_status_before != psecurity_status->security_on /*new event*/)&&
              (psecurity_status->security_on && psecurity_status->wep_enabled)) {
          /*if we have NEW security set event and error blink, clear error led.
            wps_error led OFF
            security led ON
            wps led N/A
          */
          ILOG2_V("in error_start event, test if new connection established");
          error_start = 0;
          wave_strcopy(security_leds_status[wps_error_led], security_off, sizeof(security_leds_status[wps_error_led]));
          if (psecurity_status->wep_enabled == 0)
            wave_strcopy(security_leds_status[security_led], security_on, sizeof(security_leds_status[security_led]));
        }
      }
    break;

    default:
    /*some "illegal" codes are ok, for WPS scripts only*/
      ILOG2_D("Error in wps_status_mirror, wps_status_mirror = %d"
               ,wps_status_mirror);
      result = -1; //in case of error it ignores do_leds
  } //switch...
  
  if (result != 0) {
      /*do only if no error*/
      *do_leds = 0;
  }
  
  if (result == 0) {
    wps_status_before = wps_status_mirror;
    security_status_before = psecurity_status->security_on;
  }

  return result;
}


/****************************************************************
* leds_config_security_map
* Description: map index according to path (of security leds).
*   Add leds with the same pgio to same index.
*   It is needed to avoid overrun of value in leds as gpio setting is not fix.
*   The led that has the change always to be set at the end.
*   For example, in error, clear WPS first, then set error.
*   (map only for security)
*/
void leds_config_security_map(void) 
{
  int i, j;
  
  
  for(i=MTLK_SECURITY_LEDS_INDEX_MIN; i<MTLK_SECURITY_LEDS_MAX-1; i++) {
    for(j=i+1; j<MTLK_SECURITY_LEDS_MAX; j++) {
      if (strncmp(leds_config[i].file, leds_config[j].file, SAFE_SIZEOF(leds_config[i].file)) == 0) {
        leds_config[j].index = leds_config[i].index;
      }
    }
  }
  security_led = leds_config[SECURITY_LED].index;
  wps_activity_led = leds_config[WPS_ACTIVITY_LED].index;
  wps_error_led = leds_config[WPS_ERROR_LED].index;
  ILOG2_DDD("security_led = %d, wps_activity_led = %d, wps_error_led = %d"
           ,security_led, wps_activity_led, wps_error_led);
  
  leds_map_mask = MTLK_MASK_SECURITY_NO_CONFLICT; /*every led different gpio*/
  if (security_led == wps_activity_led && security_led == wps_error_led)
    leds_map_mask = MTLK_MASK_SECURITY_CONFLICT_ALL;
  else if (security_led == wps_activity_led)
    leds_map_mask = MTLK_MASK_SECURITY_CONFLICT_WPS;
  else if (wps_error_led == wps_activity_led)
    leds_map_mask = MTLK_MASK_ACTIVITY_CONFLICT_WPS_ERR;

}


/****************************************************************
* leds_parse_configs
* Description: parse configuration read from HW.ini file,
* set leds_config[].file with the proper string and value.
* The fopen is in the loop for the case that the strings in the HW.ini
* are not in the same order as in the leds_config struct.
* Note- this function is called after driver is up, however,
*       the HW.ini may not be ready yet  (to loop)!?
*
* Options:
* WLAN_LED_2_4     - null, /dev/gpiox
* WLAN_LED_5_2     - null, /dev/gpiox
* Security_LED     - null, /dev/gpiox
* WPS_activity_LED - null, /dev/gpiox
* WPS_error_LED    - null, /dev/gpiox
*   (x = 0,1,..15)
*/
int leds_parse_configs(void) 
{
  FILE* fp = NULL;
  char *ptr;
  char line[MTLK_STRING_BUFFER_LINE_SIZE+1];
  int i = 0, /*j = 0,*/ open_count = 0;
  
  while ((i < MTLK_SECURITY_LEDS_MAX) && (open_count < MTLK_SECURITY_LEDS_MAX)) {
    fp = fopen(leds_conf_file, "r");
    if (!fp) {
      ELOG_S("error open %s file", leds_conf_file);
      return -1;
    }
    open_count++;
    while ( (fgets(line, MTLK_STRING_BUFFER_LINE_SIZE, fp) != NULL) &&
            (i < MTLK_SECURITY_LEDS_MAX) ) {
      ILOG2_DSS("i = %d, search for %s, line  = %s",i, leds_config[i].entry, line);
      /*get the ptr to string*/
      ptr = strstr(line, leds_config[i].entry);
      if (ptr) {
        /*we have a match*/
        ptr = strchr(line, '=');
        if (ptr) {
          ptr += MTLK_CHAR_OFFSET_HW_VAL;
          /*copy to array*/
          wave_strncopy(leds_config[i].file, ptr, sizeof(leds_config[i].file),
              (MTLK_LED_STRING_BUFFER_SIZE-1) - MTLK_NEW_LINE_ADDITION);
          ILOG2_SD("set file with %s, strlen(ptr) = %d",
                   leds_config[i].file,
                   wave_strlen(ptr, MTLK_LED_STRING_BUFFER_SIZE - 1 - MTLK_NEW_LINE_ADDITION));
#ifdef MTLK_DEBUG_CHARS
          for (j = 0; j < 13; j++)
            ILOG0_D("char = 0x%x", leds_config[i].file[j]);
#endif
        }
        else {
          ILOG2_SS("character %s is not in this line %s", "=", line);
        }
        i++;
      }
      else {
        ILOG2_S("string %s is not in this line",leds_config[i].entry);
      }
    }
    fclose(fp);
  }
  if (i == 0)
    ELOG_S("error, no string found in %s file", leds_conf_file);
  
  //close file
//  fclose(fp);
  
  return 0;
}

/****************************************************************
* wpa_state_complete
* run script to execute wpa cli, put the status in a file.
* read the wpa status from a mirror file.
* return values:
*    (-1), error and try again
*    0,    not found
*    1,    string found, have data link
* note- redundant char 'w' for compatibility with other location exec the file.
* TODO - Eliminate Zombie !!! 
*/
int
wpa_state_complete(void)
{

  int ret = 0;
  int pid;
  FILE* fp = NULL;
  char *ptr = NULL;
  char line[MTLK_STRING_BUFFER_LINE_SIZE+1];

  pid = fork();
  if (pid == 0) {
    ILOG2_V("fork: write 0 to mtlk_data_link_status");
    char tmp[] = {(char)('w'), (char)(0x30), (char)0};
    if (execl(link_status_script_path, link_status_script_path, tmp, (char *)NULL) == -1)
      ELOG_V("error execute execl command for mtlk_data_link_status.sh");
    exit(EVENT_DO_NOTHING);
  }
  waitpid(pid, NULL, 0);
  ILOG2_V("out waitpid");

  fp = fopen(tmp_wpa_stat_path,"r");
  if (!fp) {
    ILOG2_V("error open tmp_wpa_state_mirr file");
    return -1;
  }
  /*get the ptr to string*/
  while (ptr == NULL && (fgets(line, MTLK_STRING_BUFFER_LINE_SIZE, fp) != NULL)) {
    ptr = strstr(line, "COMPLETED");
  }
  if (ptr) {
    /*string found, data link state*/
    ILOG2_S("string found, data link state, string = %s ",line);
    ret = 1;
  }

  fclose(fp);
  return ret;
}

/****************************************************************
* exec_script
* execute script with link event change (link/disconnect)
* Params: script_ptr - pointer to script
* Params: val - link status (hex)
* send 'w0' or 'w1' indication to script.
*/
void
exec_script(char* script_ptr, char val)
{
  int pid;
  char tmp[] = {(char)('w'),(char)(val),(char)0};

  pid = fork();
  if (pid == 0) {
    ILOG2_SC("fork: exec %s script with value = %c",script_ptr,val);
    if (execl(script_ptr, script_ptr, tmp, (char *)NULL) == -1)
      ELOG_V("error execute execl command");
    exit(EVENT_DO_NOTHING);
  }
  waitpid(pid, NULL, 0);
  ILOG2_V("out waitpid");
}


/****************************************************************
* leds_thrdatamain
* New thread to handle the LEDs on the platform.
* It calls the different handlers that manages the LEDs operation.
*   Param/mem allocation
*   for (;;)
*     When driver ready
*       read configuration - only once
*       start LED handling every defined interval (by script)
*/
static int32
leds_thrdatamain (mtlk_handle_t context)
{
  char *pdata = NULL;
  int ioctl_rslt;
  static  uint8 wps_status_mirror = 0;
  int n, do_leds, i;
  char val[] = "0\n";
  char val_to_write[] = "0\n";
  static char valbefore[] = "0\n";
  int reset_led = 1, do_loop;
  static int connection = 0;
  WE_GEN_DATAEX_REQUEST *preq;
  WE_GEN_DATAEX_RESPONSE *presp;
  WE_GEN_DATAEX_CONNECTION_STATUS *pqual;
  WE_GEN_DATAEX_STATUS led_status;
  static int was_in_scan = 0;
  int wpa_status = -1;

  pdata = (char *) calloc(1, MTLK_IFCMD_DATALEN * sizeof(char));
  if (!pdata) {
    ELOG_V("Out of memory");
    goto stop;
  }
  for (i=0; i<MTLK_MAX_CONNECTION; i++) {
    u32RxCountLast[i] = 0;
    u32TxCountLast[i] = 0;
  }

  val[0] = wlan_link_off[0];

  for (;;) {
    // TODO: support reconnects if driver was rmmod-ed then insmod-ed again
    /*******************************************************
    * Request led info from driver
    */
    preq = (WE_GEN_DATAEX_REQUEST *) pdata;
    preq->ver = WE_GEN_DATAEX_PROTO_VER;
    preq->cmd_id = WE_GEN_DATAEX_CMD_STATUS;
    preq->datalen = MTLK_IFCMD_DATALEN;

    ioctl_rslt = driver_ioctl(MTLK_IOCTL_DATAEX, pdata, MTLK_IFCMD_DATALEN);
    if (ioctl_rslt == -1) {
      WLOG_V("Can't read security status; skipping LED update");
      goto stop;
    }

    presp = (WE_GEN_DATAEX_RESPONSE *) pdata;
    ILOG2_D("presp->status = %d\n",presp->status);
    
    if (presp->status == WE_GEN_DATAEX_SUCCESS) {
      wave_memcpy(&led_status, sizeof(led_status), (WE_GEN_DATAEX_STATUS*)(presp+ 1),
                                     sizeof(WE_GEN_DATAEX_STATUS));

      ILOG2_DDDDDD("led data: led_status=%d, security_on=%d, scan_started=%d, frequency_band=%d, link_up = %d, wep_enabled=%d\n"
               ,presp->status
               ,led_status.security_on
               ,led_status.scan_started
               ,led_status.frequency_band
               ,led_status.link_up
               ,led_status.wep_enabled);
    } else {
      dataex_report_error(presp->status, pdata);
      goto stop;
    }

    /*******************************************************
    * Request connection statistics
    */
    //TODO: if strcmp(file, "null") == 0, no need to get statistics
    preq = (WE_GEN_DATAEX_REQUEST *) pdata;
    preq->ver = WE_GEN_DATAEX_PROTO_VER;
    preq->cmd_id = WE_GEN_DATAEX_CMD_CONNECTION_STATS;
    preq->datalen = MTLK_IFCMD_DATALEN;

    ioctl_rslt = driver_ioctl(MTLK_IOCTL_DATAEX, pdata, MTLK_IFCMD_DATALEN);
    if (ioctl_rslt == -1) {
      WLOG_V("Can't read connection statistics; skipping LED update");
      goto stop;
    }

    presp = (WE_GEN_DATAEX_RESPONSE *) pdata;
    
    if (presp->status == WE_GEN_DATAEX_SUCCESS) {
      pqual = (WE_GEN_DATAEX_CONNECTION_STATUS *)(presp + 1);
      /*for AP the security mode is read from conf file,
        because the security led has to be set before station is connected
        (from driver we set security only on connection)*/
      if (network_type[0] == MTLK_AP) {
        ILOG2_D("network_type[0] = MTLK_AP, copy led_status.security_on from security_mode = %d\n",security_mode);
        led_status.security_on = (security_mode>=IW_ENCODE_ALG_TKIP)?1:0;
      }
      if (pqual->u32NumOfConnections > 0 || led_status.scan_started ||
          (network_type[0] == MTLK_AP && led_status.link_up == MTLK_LINK_UP)) {
        ILOG2_DDDD("u32NumOfConnections=%d, scan_started=%d, connection=%d, wps_status_mirror=%d \n"
                ,pqual->u32NumOfConnections
                ,led_status.scan_started
                ,connection
                ,wps_status_mirror);
        
        /*connection is up, test if need to indicate other application*/
        if ((link_script_path != NULL) && (connection == 0) && (pqual->u32NumOfConnections || led_status.link_up == MTLK_LINK_UP)) {
          /*test if secured link. If is, wait to 4-handshake to finish*/
          if ((wps_status_mirror != MTLK_WPS_ACTIVATED) && (wps_status_mirror != MTLK_WPS_REGISTER)) {
            wpa_status = 1;
            if (network_type[0] != MTLK_AP) {
              wpa_status = wpa_state_complete();
            }
            if (wpa_status == 1) {
              ILOG2_D("wpa state completed, wps_status_mirror = %d",wps_status_mirror);
              exec_script(link_script_path, '1');
              connection = 1;
            }
          }
//          else if (network_type[0] == MTLK_AP)
//            connection = 1;
        }
        else if ((link_script_path != NULL) && (connection == 1) && (network_type[0] == 0x30 && pqual->u32NumOfConnections == 0)) {
          /*scan process. set flag to detect "statistics out of synch" (?change to support AP if it scan upon Init)*/
          was_in_scan = 1;
        }
        else if ((was_in_scan) && (link_script_path != NULL) && (connection == 1) && (pqual->u32NumOfConnections == 1)) {
          /*a patch:
            send disconnect to DHCP in case of statistic out of synch event*/
          ILOG2_V("patch exec due to statistic mismatch");
          was_in_scan = 0;
          connection = 0;
          exec_script(link_script_path, '0');
          if (network_type[0] != MTLK_AP) {
            /*and delete link related tmp file*/
            exec_script(link_status_script_path, '1');
          }
        }
        do_loop = pqual->u32NumOfConnections > led_status.scan_started ? pqual->u32NumOfConnections : led_status.scan_started;
        if (do_loop == 0 && network_type[0] == MTLK_AP)
          do_loop = 1;
        ILOG2_D("do_loop = %d",do_loop);
        for (n = 0; n < do_loop; ++n) {
          WE_GEN_DATAEX_DEVICE_STATUS *devst = &pqual->sDeviceStatus[n];

          val[0] = parse_status(devst, &led_status, n);
          ILOG2_SSS("ret: val = %s, val_to_write = %s, valbefore = %s\n",val,val_to_write,valbefore);
          /* note- in case links > 1, priority is:
          prio  val status
          1.    2   scan
          2.    5   data
          3.    1   link
          */
          if (val[0] != valbefore[0]) {
            ILOG2_DS("val[0] != valbefore[0], n = %d, val = %s",n, val);
            if (n == 0)
              val_to_write[0] = val[0];
            else {
              /*AP multi connection, overwrite only if priority is higher*/
              if (val[0] == wlan_link_scan[0])
                val_to_write[0] = val[0];
              else if ((val[0] == wlan_link_data[0]) && (val_to_write[0] == wlan_link_on[0]))
                val_to_write[0] = val[0];
              ILOG2_S("val = %s",val);
            }
          }
        } //for...
        if (valbefore[0] != val_to_write[0]) {
          /*led change is updated, ready to reset in case of disconnection*/
          ILOG2_S("call leds_control, val = %s",val);
          reset_led = 1;
          valbefore[0] = val_to_write[0];
          leds_control(val_to_write, &led_status);
        }
      } else if (reset_led) {
          /*connection is down, test if need to indicate other application (link_script_path)*/
          if ((link_script_path != NULL) && (connection == 1)) {
            exec_script(link_script_path, '0');
            if (network_type[0] != MTLK_AP) {
              /*and delete link related tmp file*/
              exec_script(link_status_script_path, '1');
            }
            connection = 0;
            /*clear flag of "statistics out of synch"*/
            was_in_scan = 0;
          }
          reset_led = 0;
          val[0] = wlan_link_off[0];
          valbefore[0] = val[0];
          ILOG2_S("no connection reset led, val = %s",val);
          leds_control(val, &led_status);
      }
    } else {
      dataex_report_error(presp->status, pdata);
      goto stop;
    }

    /*******************************************************
    * Request WPS info
    */
     //Done in a seperate thread (leds_thrdatapipe), wps_status is set


    /*******************************************************
    * process - led_status.security_on and WPS
    */
    wps_status_mirror = wps_status;
    ILOG2_D("wps_status_mirror = %d\n",wps_status_mirror);
    if (parse_security_status(&led_status,
                              wps_status_mirror,
                              &do_leds) == 0) {
      if (do_leds) {
        do_leds = 0;
        leds_control_security();
      }
    }

    if (mtlk_osal_event_wait(&ledsctrl_thread_stop, 
                             led_resolution * 1000) == MTLK_ERR_OK) {
      break;
    }
  } /*while*/

stop:
  if (pdata)
    free(pdata);

  return 0;
}

