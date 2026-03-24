/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "whm_handler.h"
#include "argv_parser.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/statvfs.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>

#if defined YOCTO
#include <wav-dpal/dwpal.h>
#include <wav-dpal/dwpald_client.h>
#else
#include "dwpal.h"
#include "dwpald_client.h"
#endif

#ifdef CONFIG_WAVE_RTLOG_REMOTE
    #include "mtlk_rtlog_app.h"
#endif
#define LOG_LOCAL_GID   GID_WHM_HANDLER
#define LOG_LOCAL_FID   1

#define WHM_HANDLER_VERSION "1.0.2"
#if defined YOCTO
#define WHM_MAX_DUMP_FILES 200 /* TBD - need to check what is the free space on CGR*/
#else
#define WHM_MAX_DUMP_FILES 200 /* on AXP we have ~25MB free space, deduction of (3*FW_DUMP_TARBALL_SIZE+ MIN_FREE_SPACE_ON_FS) will give 200*/
#endif
#define WHM_DUMP_TARBALL_SIZE(size) ((size)*1024) /* e.g. 100KB */
#define WHM_DUMP_MAC_FATAL_SIZE (200) /* fixed 200KB */
#define MIN_FREE_SPACE_ON_FS (512 * 1204)  /*500KB this is the minmum free space we want to keep free for the FS*/

/* driver event */
#define WHM_DRIVER_EVENT_ID LTQ_NL80211_VENDOR_EVENT_WHM

#define DRIVER_LOG     "/bin/dmesg"
#define EEPROM_PARSED  "eeprom_parsed"
#if defined YOCTO
#define SYS_LOG  		"/var/log/daemon"
#define SYS_LOG_NAME	"daemon"
#define PLAT_VERSION "/etc/wave/bins/wave_components.ver"
#else
#define SYS_LOG  		"/var/log/messages"
#define SYS_LOG_NAME	"messages"
#define PLAT_VERSION "/etc/wave_components.ver"
#endif

#define FW_TRACE_SIZE (6*1024) /*6KB*/
#define DRIVER_LOG_SIZE (127*1024) /*124KB*/
/*Assuming: DRIVER_LOG 124KB tar-> 17KB ,FW_TRACE_SIZE 6KB tar-> 3KB - the space left will be for syslog*/
#define FW_TRACE_BUF_CR 2
#define DRIVER_LOG_CR 5
#define SYS_LOG_CR 8
#define SYS_LOG_MAX_SIZE(size) ( ((size*1024) - (FW_TRACE_SIZE/FW_TRACE_BUF_CR) - (DRIVER_LOG_SIZE/DRIVER_LOG_CR)) * SYS_LOG_CR )
/* SYS_LOG_MAX_SIZE = 560 KB will compressed to ~80KB +( 17 KB driver + 3KB FW) */
#define FW_DUMP_TARBALL_SIZE (1500 * 1204) /*1500KB saved for possible FW dumps*/


#define IFACE_NAME_LEN 6 /* wlanX */
#define EVENT_TYPE_NAME_LEN 8 /* driver , hostapd */
#define MAX_FILE_NAME_SIZE 256
#define MAX_CMD_SIZE 1024
#define BUF_SIZE 4096

#define WHM_TMP_PATH "/tmp/whm_dump_files"
#define WHM_CFG_FILE "/var/run/whm.conf"
#define FW_WHM_SHRAM_TMP_FOLDER "/tmp/whm_shram"

/* e.g. /proc/net/mtlk/wlan0/Debug/fw_trace . MAX_LIMIT = 10KB */
#define PROC_NET_MTLK_PATH "/proc/net/mtlk/"
#define FW_TRACE_BUF_FILE_SUFFIX "/fw_trace"
#define FW_TRACE_BUF_SIZE_MAX_LIMIT 10240
#define FW_TRACE_BUF_FILE_LOG_NAME "fw_trace"
#define FW_TRACE_BUF_HEADER_LEN_SIZE 4

#define MAC_FATAL_EVENT_ID 1

#ifdef DEBUG_BUILD
#define USB_MOUNT_PATH "/mnt/usb/*/"
#define USB_MOUNT_PATH_PUMA "/tmp/mnt/*/"
#endif

static time_t time_last_dump_saved = 0;
static const char   *layer_name[MAX_FILE_NAME_SIZE] = { "EMPTY", "HST", "DRV", "FW", "IW", "PHY" };

struct whm_handler_cfg
{
  char iface_name[IFACE_NAME_LEN];
  char storage_path[MAX_FILE_NAME_SIZE];
  uint32 tarball_size_kb;
  uint32 num_of_dumps;
  BOOL no_limit_dumps;
  BOOL terminate;
};
struct whm_handler_cfg g_wh_config;

typedef struct _whm_event_t
{
  int    id;
  int    layer_id;
  uint32 num_of_cards;
  char   type[EVENT_TYPE_NAME_LEN];
} whm_event;

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static rtlog_app_info_t rtlog_info_data;
#endif

static const struct mtlk_argv_param_info_ex param_path =  {
  {
    "f",
    "storage_path",
    MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
  },
  "persistent storage path",
  MTLK_ARGV_PTYPE_MANDATORY
};

static BOOL _safe_system_cmd_string (const char * string, int size) {
  int i;
  for(i=0; i<size && string[i]!=0; i++) {
    if (string[i] == ';' || string[i] == '|' || string[i] == '&' ||
        string[i] == '\n' || string[i] == '\r' || string[i] == '`')
      goto failure;
    if (i<size-1 && string[i] == '$' && string[i+1] == '(')
      goto failure;
  }
  return TRUE;

failure:
  ELOG_S("error: string %s contains unsafe characters", string);
  return FALSE;
}

static int get_file_size (const char *file_path,
                           unsigned int *size) {
  int res = MTLK_ERR_OK;
  struct stat fs_stat;

  if (stat(file_path, &fs_stat) != 0) {
    ELOG_V("Failed to get file stat");
    res = MTLK_ERR_UNKNOWN;
  } else {
    *size = fs_stat.st_size;
  }

  return res;
}

static int get_free_space (const char *storage_path,
                           unsigned long long *free_space_in_fs) {
  struct statvfs fs_stat;

  if (statvfs(storage_path, &fs_stat) != 0)
    return MTLK_ERR_UNKNOWN;

  *free_space_in_fs = fs_stat.f_bavail * fs_stat.f_bsize;
  return MTLK_ERR_OK;
}

/*
As part of wireless DB using: uci show wireless.generic
config wifi-device 'generic'
        option whm_enable '1'
        option whm_tar_size '100'
        option whm_max_dumps '200'
        option whm_driver_warn '1'
        option whm_hostapd_warn '1'
        option whm_fw_warn '1'
        option whm_phy_warn '1'

ubus call uci set '{ "config" : "wireless" ,"section" : "generic", "values" : {"whm_enable" : "0" }}'
ubus call uci commit '{"config" : "wireless"}'
*/
static int manage_cfg_file(BOOL *enable, unsigned int *tar_size, unsigned int *max_dumps) {

  int                 sprintf_res = 0;
  char                sys_cmd[MAX_CMD_SIZE];
  FILE                *pf = NULL;
  char                param[MAX_FILE_NAME_SIZE] = {0};
  const char          *ret_str = NULL;
  char                filter_str[] = "sed \"s/wireless.generic.//; s/\'//g; 1d\"";
  int                 res = MTLK_ERR_OK;
  unsigned int        cfg_param = 0;

  /* prepare config file for reading */
  sprintf_res = sprintf_s(sys_cmd, sizeof (sys_cmd),"uci show wireless.generic 2>/dev/null | %s > %s", filter_str, WHM_CFG_FILE);
  if (sprintf_res <= 0 || sprintf_res >= sizeof (sys_cmd)) {
    ELOG_V("sprintf_s file sys cmd failure");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }
  ILOG1_S("sys_cmd [%s]", sys_cmd);
  system (sys_cmd);

  pf = fopen(WHM_CFG_FILE, "r");
  if (pf == NULL) {
    ELOG_S("Cannot open conf file %s", WHM_CFG_FILE);
    res = MTLK_ERR_FILEOP;
    goto end;
  }

  ILOG0_S("Read [%s]", WHM_CFG_FILE);
  while (pf && (fgets(param, MAX_FILE_NAME_SIZE , pf) != NULL)) {
    ILOG0_S("WHM_CFG_FILE param[%s]", param);
    ret_str = strstr(param, "=");
    if (ret_str != NULL) {
      ret_str++;
      cfg_param = atoi(ret_str);
      ILOG1_SD("WHM_CFG_FILE ret_str[%s] cfg_param[%d]", ret_str, cfg_param);
    } else {
      res = MTLK_ERR_FILEOP;
      ILOG0_V("WHM_CFG_FILE wrong file format");
      goto end;
    }
    if (cfg_param) {
      if (strstr(param, "enable") != NULL) {
        *enable = cfg_param;
      } else if (strstr(param, "whm_tar_size") != NULL) {
        *tar_size = cfg_param;
      } else if (strstr(param, "whm_max_dumps") != NULL) {
        *max_dumps = cfg_param;
      } else if (strstr(param, "whm_driver_warn") != NULL) {
        ILOG1_D("WHM_CFG_FILE whm_driver_warn=%d", cfg_param);
      } else if (strstr(param, "whm_hostapd_warn") != NULL) {
        ILOG1_D("WHM_CFG_FILE whm_hostapd_warn=%d", cfg_param);
      } else if (strstr(param, "whm_fw_warn") != NULL) {
        ILOG1_D("WHM_CFG_FILE whm_fw_warn=%d", cfg_param);
      } else if (strstr(param, "whm_phy_warn") != NULL) {
        ILOG1_D("WHM_CFG_FILE whm_phy_warn=%d", cfg_param);
      } else {
        ILOG0_V("WHM_CFG_FILE wrong param name");
      }
    } else {
      ILOG0_V("WHM_CFG_FILE param value is zero");
    }
  }

end:
  if (pf)
    fclose(pf);

   ILOG0_V("WHM_CFG_FILE END");

  return res;
}


static int _zip_whm_files (whm_event *event, char *tmp_whm_folder, const char *storage_path, char *ifname,
                           BOOL no_limit_dumps, uint32 num_of_dumps) {
  char                system_cmd[MAX_CMD_SIZE];
  char                oldest[MAX_FILE_NAME_SIZE];
  char                file_full_path[MAX_FILE_NAME_SIZE];
  DIR                 *storage_dir = NULL;
  struct              dirent *ent = NULL;
  int                 whm_file_count = 0;
  BOOL                whm_remove_oldest = FALSE;
  BOOL                whm_limit_space = FALSE;
  int                 sprintf_res = 0;
  char                tar_file_name[MAX_FILE_NAME_SIZE];
  time_t              curr_time = time(NULL);
  time_t              mtime = 0;
  struct              tm *tm = localtime(&curr_time);
  struct              timeval tv = {0};
  unsigned long       time_stamp_ms = 0;
  unsigned long long  free_space_in_fs = 0;
  struct stat         sb = {0};
  int                 res = MTLK_ERR_OK;
  char                *whm_folder_name = NULL;

  ILOG1_V("ENTRY");

  /* True in USB case:if this set then we can save as much as we can on the flash */
  if (no_limit_dumps)
    goto save_whm_dumps;

  /* if we fail to have enough free space do not create tarball */
  ILOG1_S("get_free_space storage_path[%s]", storage_path);

  if (get_free_space(storage_path, &free_space_in_fs) != MTLK_ERR_OK) {
    ELOG_V("Getting available free space has failed.");
    res = MTLK_ERR_VALUE;
    goto end;
  }

  ILOG1_D("get_free_space free_space_in_fs[%d]", free_space_in_fs);

  /* On limit space, if we have more then 1 WHM dump tarball then we can delete the last one
     In any case keep spcae for 1 FW dump: FW_DUMP_TARBALL_SIZE*/
  if (free_space_in_fs < (MIN_FREE_SPACE_ON_FS + FW_DUMP_TARBALL_SIZE)) {
    whm_remove_oldest = TRUE;
    whm_limit_space = TRUE;
    ILOG0_V("LIMIT-SPACE-DETECTED remove WHM oldest dump");
  }

  /* if user defined a fixed number of WHM dumps then when MAX reached -> remove the oldest
     MAX dumps defined in: WHM_MAX_DUMP_FILES*/
  if (num_of_dumps != 0) {
    whm_remove_oldest = TRUE;
  }

  if ( whm_remove_oldest == TRUE) {
    if ((storage_dir = opendir (storage_path)) != NULL) {
      /* go over all the files and directories within directory */
      while ((ent = readdir (storage_dir)) != NULL) {
        ILOG1_S("[%s]", ent->d_name);
        if (strstr(ent->d_name, "whm_dump") != NULL) {
          whm_file_count +=1;
          ILOG1_DS("found whm file[%d][%s]", whm_file_count, ent->d_name);
          sprintf_res = sprintf_s(file_full_path, sizeof(file_full_path), "%s/%s", storage_path, ent->d_name);
          if (sprintf_res <= 0 || sprintf_res >= sizeof (file_full_path)) {
            ELOG_V("sprintf_s filename failure");
            res = MTLK_ERR_UNKNOWN;
            goto end;
          }
          ILOG1_S("full path:whm file[%s]", file_full_path);
          if (stat(file_full_path, &sb) != MTLK_ERR_OK) {
            ELOG_S("stat file[%s] failed", ent->d_name);
            res = MTLK_ERR_FILEOP;
            goto end;
          }
          /* save the full path and file name of the oldest WHM dump*/
          if (mtime == 0 || sb.st_mtime < mtime) {
            mtime = sb.st_mtime;
            mtlk_osal_strlcpy(oldest, file_full_path, MAX_FILE_NAME_SIZE);
            ILOG1_S("oldest is whm file[%s]", oldest);
          }
        }
      }
    } else {
      /* could not open directory */
      ELOG_V("Could not open directory");
      res = MTLK_ERR_FILEOP;
      goto end;
    }
    closedir(storage_dir);
    storage_dir = NULL;

    if ( (whm_file_count > 1) &&
          ((whm_file_count >= num_of_dumps) ||
          (whm_file_count >= WHM_MAX_DUMP_FILES) ||
          (whm_limit_space == TRUE)) ) {
      remove(oldest);
      ILOG1_S("remove oldest whm file[%s]", oldest);
    }
  }

  ILOG1_D("num_of_dumps[%d]", num_of_dumps);

save_whm_dumps:

  ILOG1_V("save_whm_dumps START");

  if (tm == NULL) {
    ELOG_V("Failed to convert time to localtime");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  if (gettimeofday(&tv,NULL) != 0) {
    ELOG_V("Failed to gettimeofday");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  } else {
    time_stamp_ms = (tv.tv_usec/1000);
  }

  /* whm tarball name */
  if ((event->layer_id == WHM_DRIVER_TRIGGER) && (event->id == MAC_FATAL_EVENT_ID)) {
    sprintf_res = sprintf_s(tar_file_name, sizeof (tar_file_name),
                          "whm_dump_%d_%02d_%02d_%02d_%02d_%02d_%d_%s.tar.gz",
                          tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                          tm->tm_hour, tm->tm_min, tm->tm_sec,time_stamp_ms,"MAC_FATAL");
  } else {
    sprintf_res = sprintf_s(tar_file_name, sizeof (tar_file_name),
                          "whm_dump_%d_%02d_%02d_%02d_%02d_%02d_%d_%s_W%d.tar.gz",
                          tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                          tm->tm_hour, tm->tm_min, tm->tm_sec,time_stamp_ms,layer_name[event->layer_id], event->id);
  }

  if (sprintf_res <= 0 || sprintf_res >= sizeof (system_cmd)) {
    ELOG_V("sprintf_s file tar filename failure");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  ILOG1_S("tar_file_name[%s]", tar_file_name);

  whm_folder_name = tmp_whm_folder + (sizeof("/tmp/") - 1);
  ILOG1_S("folder name of without path: whm_folder_name[%s]", whm_folder_name);

 /* 2 > /tmp/whm-error_%d.log for debug*/
  sprintf_res = sprintf_s(system_cmd, sizeof (system_cmd),
                          "/bin/tar -C /tmp -czf %s/%s %s > /dev/null 2>&1",
                          storage_path, tar_file_name, whm_folder_name);
  ILOG1_S("sprintf_s tar cmd[%s]", system_cmd);
  if(sprintf_res <= 0 || sprintf_res >= sizeof (system_cmd)) {
    ELOG_V("sprintf_s tar command failure");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  system(system_cmd);

end:
  ILOG1_V("END");

  if(storage_dir != NULL)
    closedir (storage_dir);

  return res;
}

static int
_fetch_fw_shram (char *ifname, char *tmp_whm_folder, unsigned int *file_size) {
  int           sprintf_res = 0;
  int           res = MTLK_ERR_OK;
  char          out_file_full_name[MAX_FILE_NAME_SIZE];
  char          shram_tmp[MAX_FILE_NAME_SIZE];
  struct        stat fs_stat;
  unsigned int  wcount = 0;

  ILOG1_V("ENTRY");

  sprintf_res = sprintf_s(out_file_full_name, MAX_FILE_NAME_SIZE, "%s/%s", tmp_whm_folder,
                          "shram");
  if (sprintf_res <= 0 || sprintf_res >= MAX_FILE_NAME_SIZE) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }
  ILOG1_S("[%s]", out_file_full_name);

  sprintf_s(shram_tmp, sizeof(shram_tmp), "%s/%s", FW_WHM_SHRAM_TMP_FOLDER, "shram");
  ILOG1_SS("rename from [%s] to [%s]", shram_tmp, out_file_full_name);

   while ((stat(shram_tmp, &fs_stat) != 0) && (wcount < 5)){
     sleep(1);
     wcount++;
     ILOG1_SD("wait for [%s][%d] ...", shram_tmp, wcount);
  }

  if ((res = rename(shram_tmp,out_file_full_name)) == 0) {
    ILOG1_S("shram file was moved to [%s] for smaller WHM dump", shram_tmp);
  } else {
    ELOG_DD("rename res[%d]errno[%d]", res, errno);
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  get_file_size(out_file_full_name, file_size);
  ILOG1_SD("Size of FW_LOG[%s] is file_size[%u]", out_file_full_name, *file_size);

end:

  ILOG1_V("END");

  /*remove all shram tmp files*/
  remove(shram_tmp);
  if (rmdir(FW_WHM_SHRAM_TMP_FOLDER) == 0)
      ILOG1_S("remove FW_WHM_SHRAM_TMP_FOLDER[%s]", FW_WHM_SHRAM_TMP_FOLDER);
   else
      ELOG_S("Error remove FW_WHM_SHRAM_TMP_FOLDER[%s]", FW_WHM_SHRAM_TMP_FOLDER);

  return res;
}

static int
_prepare_fw_trace_buffer (char *ifname, char *tmp_whm_folder, unsigned int *file_size) {
  FILE          *dump_file = NULL;
  FILE          *out_file = NULL;
  char          fw_trace_buffer_filename[MAX_FILE_NAME_SIZE];
  unsigned int  remaining = 0;
  unsigned int  read, written;
  char          out_file_full_name[MAX_FILE_NAME_SIZE];
  unsigned char buf[BUF_SIZE];
  int           sprintf_res = 0;
  int           res = MTLK_ERR_OK;

  ILOG1_V("ENTRY");

  sprintf_res = sprintf_s(fw_trace_buffer_filename, sizeof(fw_trace_buffer_filename), "%s%s%s",
                           PROC_NET_MTLK_PATH, ifname, FW_TRACE_BUF_FILE_SUFFIX);

  if (sprintf_res <= 0 || sprintf_res >= sizeof(fw_trace_buffer_filename)) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  ILOG1_S("[%s]", fw_trace_buffer_filename);

  dump_file = fopen(fw_trace_buffer_filename, "rb");
  if (dump_file == NULL) {
    ELOG_S("Cannot open WHM dump file[%s]", fw_trace_buffer_filename);
    res = MTLK_ERR_FILEOP;
    goto end;
  }

  ILOG1_S("[%s] opened", fw_trace_buffer_filename);

  /* get fw_trace buffer size */
  if (!fread(&remaining, sizeof(remaining), 1, dump_file)) {
    ELOG_S("Invalid fw_trace buffer, missing file size %s", fw_trace_buffer_filename);
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  if (remaining > FW_TRACE_BUF_SIZE_MAX_LIMIT) {
    ELOG_DD("Invalid fw_trace buffer, buffer size[%d] > FW_TRACE_BUF_SIZE_MAX_LIMIT[%d]",
    remaining, FW_TRACE_BUF_SIZE_MAX_LIMIT);
    res = MTLK_ERR_VALUE;
    goto end;
  }

  ILOG1_DD("buffer size=[0x%x][%d]", remaining, remaining);

  sprintf_res = sprintf_s(out_file_full_name, MAX_FILE_NAME_SIZE, "%s/%s_%s", tmp_whm_folder,
                          FW_TRACE_BUF_FILE_LOG_NAME, ifname);
  if (sprintf_res <= 0 || sprintf_res >= MAX_FILE_NAME_SIZE) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  ILOG1_S("[%s]", out_file_full_name);

  out_file = fopen(out_file_full_name, "wb+");
  if (out_file == NULL) {
    ELOG_S("Cannot open output dump file %s", out_file_full_name);
    res = MTLK_ERR_FILEOP;
    goto end;
  }

  /* read the fw_trace buffer and write to tmp_whm_folder location for compressing*/
  while(remaining > 0 && remaining < FW_TRACE_BUF_SIZE_MAX_LIMIT) {

    ILOG1_SDD("out_file_full_name[%s], remaining[%d], numB2read[%d]",
           out_file_full_name,remaining , (remaining < BUF_SIZE ?remaining:BUF_SIZE));

    read = fread(buf, 1, remaining < BUF_SIZE ?remaining:BUF_SIZE, dump_file);
    if (!read) {
      ELOG_S("Error reading %s", FW_TRACE_BUF_FILE_LOG_NAME);
      res = MTLK_ERR_FILEOP;
      goto end;
    }
    remaining -= read;

    while (read) {
      written = fwrite (buf, 1, read, out_file);
      if (!written) {
        ELOG_S("Error writing %s",FW_TRACE_BUF_FILE_LOG_NAME);
        res = MTLK_ERR_FILEOP;
        goto end;
      }
      ILOG1_DS("write[%d] to [%s]", written, out_file_full_name);
      read -= written;
    }
  }

  ILOG1_S("SUCCESS[%s]",
           out_file_full_name);

  get_file_size(out_file_full_name, file_size);
  ILOG1_SD("Size of FW_LOG[%s] is file_size[%u]", out_file_full_name, *file_size);

end:

  ILOG1_V("END");

  if (dump_file)
    fclose(dump_file);

  if (out_file)
    fclose (out_file);

  return res;

}

static int
_fetch_fw_trace_buffer (whm_event *event, char *ifname, char *tmp_whm_folder, unsigned int *file_size) {
  char          iface_name[IFACE_NAME_LEN];
  const int     iface_index[4] = { 0, 0, 2, 4};
  int           sprintf_res = 0;
  int           res = MTLK_ERR_OK;
  int           i,if_idx = 0;
  int           if_idx_last = 0;
  dwpald_ret    ret_dwpld = DWPALD_SUCCESS;
  unsigned int  whm_reset = 1;
  unsigned int  total_file_size = 0;
  unsigned int  size = 0;

  ILOG1_V("ENTRY");

  ILOG1_DDD("layer = %d : id : %d num_of_cards: %d\n",event->layer_id,event->id,event->num_of_cards);

 /* on case of warning from DRV/HOSTAPD/IW layers we will fetch all reported cards fw_trace_buffers */
 if ( (event->layer_id == WHM_DRIVER_TRIGGER) ||
      (event->layer_id == WHM_HOSTAPD_TRIGGER) ||
      (event->layer_id == WHM_IW_TRIGGER) ) {
    if (event->num_of_cards >= 1 && event->num_of_cards <= 3) {
      if_idx_last = iface_index[event->num_of_cards];
      ILOG1_D("if_idx_last=[%d]", if_idx_last);
    } else {
      ILOG0_D("wrong number of cards [%d]", event->num_of_cards);
      res = MTLK_ERR_UNKNOWN;
      goto end;
    }
  } else { /* All other layers will be handled as before per interface received(fetch interface index) */
    for (i = 0; ifname[i] != '\0' && i<7; i++)
    {
      if (isdigit(ifname[i]) != 0)
        break;
    }
    if_idx = if_idx_last = ifname[i] - '0';
    if (if_idx < 0 || if_idx > 4) {
      ILOG0_D("wrong interface index [%d]", if_idx);
      res = MTLK_ERR_UNKNOWN;
      goto end;
    }
    ILOG1_D("if_idx=if_idx_last=[%d]", if_idx);
  }

  ILOG1_DD("if_idx[%d] if_idx_last=[%d]", if_idx, if_idx_last);
  for(; if_idx <= if_idx_last; if_idx+=2) {

    sprintf_res = sprintf_s(iface_name, sizeof(iface_name), "wlan%d", if_idx);
    ILOG1_S("iface_name -> %s", iface_name);
    if (sprintf_res <= 0 || sprintf_res >= sizeof(iface_name)) {
      ELOG_V("sprintf_s() error");
      res = MTLK_ERR_UNKNOWN;
      goto end;
    }

    if (_prepare_fw_trace_buffer(iface_name, tmp_whm_folder, &size) != MTLK_ERR_OK) {
      ELOG_V("_prepare_fw_trace_buffer FAILED");
      res = MTLK_ERR_UNKNOWN;
      goto end;
    }

    total_file_size += size;
  }

  *file_size = total_file_size;

end:
  ILOG1_V("END");

  /* Once dumps are fetched or on error we must notify the driver that action was done */
  /* reset on wlan0 will take care of reset all exisiting interfaces*/
  ret_dwpld = dwpald_drv_set("wlan0",
                             LTQ_NL80211_VENDOR_SUBCMD_SET_WHM_RESET,
                             &res, &whm_reset, sizeof(whm_reset));
  if (ret_dwpld != DWPALD_SUCCESS) {
    ELOG_D("dwpald_drv_set returned err[%d]", ret_dwpld);
  }
  ILOG1_S("dwpald_drv_set: driver send cmd:SET_WHM_RESET ifname[%s]", g_wh_config.iface_name);

   return res;
}

static int
_fetch_sys_log (char *tmp_whm_folder, unsigned int log_size) {

  char   sys_cmd[MAX_CMD_SIZE];
  int    res = MTLK_ERR_OK;
  int    sprintf_res = 0;

  /* unify 3 last syslog files ( max possible size > 3MB) into SYS_LOG.tmp so we can cat customized size */
  sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "cat %s.%d %s.%d %s >> %s/%s.tmp 2>/dev/null", SYS_LOG, 2, SYS_LOG, 1, SYS_LOG, tmp_whm_folder, SYS_LOG_NAME);
  ILOG1_S("sys_cmd -> %s", sys_cmd);
  if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_FILEOP;
    goto end;
  } else {
    system (sys_cmd);
    ILOG1_V("Unified all syslog to SYS_LOG_NAME.tmp");
  }

  ILOG1_DD("log_size=[%d] vs. SYS_LOG_MAX_SIZE=[%d]", log_size, (SYS_LOG_MAX_SIZE(g_wh_config.tarball_size_kb)));

  if (log_size > (SYS_LOG_MAX_SIZE(g_wh_config.tarball_size_kb)))
    log_size = (SYS_LOG_MAX_SIZE(g_wh_config.tarball_size_kb));

 /* cat from syslog the needed size , if smaller tail will take all there is*/
  sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "/usr/bin/tail -c%d %s/%s.tmp > %s/%s", log_size, tmp_whm_folder, SYS_LOG_NAME, tmp_whm_folder, SYS_LOG_NAME);
  ILOG1_S("sys_cmd -> %s", sys_cmd);
  if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_FILEOP;
    goto end;
  } else {
    system (sys_cmd);
    ILOG1_SS("copy SYS_LOG[%s] to tmp_whm_folder[%s]", SYS_LOG, tmp_whm_folder);
  }

  /* remove SYS_LOG.tmp */
  sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "rm -f %s/%s.tmp 2>/dev/null", tmp_whm_folder, SYS_LOG_NAME);
  ILOG1_S("sys_cmd -> %s", sys_cmd);
  if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_FILEOP;
    goto end;
  } else {
    system (sys_cmd);
    ILOG1_V("remove SYS_LOG_NAME.tmp");
  }

end:
  return res;
}


static int
_fetch_driver_log (char *tmp_whm_folder, unsigned int *file_size) {

  int    sprintf_res = 0;
  char   sys_cmd[MAX_CMD_SIZE];
  int    res = MTLK_ERR_OK;

  sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "%s > %s/%s", DRIVER_LOG, tmp_whm_folder, "dmesg.log");
  ILOG1_S("sys_cmd -> %s", sys_cmd);
  if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_FILEOP;
    goto end;
  } else {
    system (sys_cmd);
    ILOG1_SSS("copy DRIVER_LOG[%s] to tmp_whm_folder[%s/%s]", DRIVER_LOG, tmp_whm_folder, "dmesg.log" );
  }

  sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "%s/%s", tmp_whm_folder, "dmesg.log");
  if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_FILEOP;
    goto end;
  } else {
    get_file_size(sys_cmd, file_size);
    ILOG1_SD("Size of DRIVER_LOG[%s] is file_size[%u]", sys_cmd, *file_size);
  }

end:
  return res;
}

static int
_fetch_env_info (char *tmp_whm_folder) {

  int    sprintf_res = 0;
  char   sys_cmd[MAX_CMD_SIZE];
  int    res = MTLK_ERR_OK;
  int    if_idx = 0;

  sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "cp %s  %s", PLAT_VERSION, tmp_whm_folder );
  ILOG1_S("sys_cmd -> %s", sys_cmd);
  if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_FILEOP;
    goto end;
  } else {
    system (sys_cmd);
  }

  for(if_idx=0; if_idx <= 4; if_idx+=2) {
    sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "cp %swlan%d/%s  %s%s_wlan%d > /dev/null 2>&1", PROC_NET_MTLK_PATH, if_idx, EEPROM_PARSED, tmp_whm_folder, EEPROM_PARSED, if_idx );
    ILOG1_S("sys_cmd -> %s", sys_cmd);
    if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)) {
      ELOG_V("sprintf_s() error");
    } else {
      system (sys_cmd);
    }
  }

end:
  return res;
}

static int
_fetch_dumps (whm_event *event, const char *storage_path, char *ifname,
              BOOL no_limit_dumps, uint32 num_of_dumps) {
  struct              stat st = {0};
  char                tmp_whm_folder[MAX_FILE_NAME_SIZE];
  int                 res = MTLK_ERR_OK;
  int                 sprintf_res = 0;
  char                sys_cmd[MAX_CMD_SIZE];
  struct              timeval tv = {0};
  unsigned long long  time_stamp_ms = 0;
  unsigned int        size;
  unsigned int        acu_comp_size = 0;
  unsigned int        syslog_size = 0;

  ILOG1_V("ENTRY");

  if (gettimeofday(&tv,NULL) != MTLK_ERR_OK) {
    ELOG_V("Failed to gettimeofday");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  } else {
    time_stamp_ms = (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
  }

  /* if the dir exist skip ,if not create*/
  if ((event->layer_id == WHM_DRIVER_TRIGGER) && (event->id == MAC_FATAL_EVENT_ID)) {
    sprintf_res = sprintf_s(tmp_whm_folder, sizeof(tmp_whm_folder), "%s_%s_%s_%lld_%s/",
                                                                WHM_TMP_PATH,
                                                                event->type,
                                                                "MAC_FATAL",
                                                                time_stamp_ms,
                                                                ifname);
  } else {
    sprintf_res = sprintf_s(tmp_whm_folder, sizeof(tmp_whm_folder), "%s_%s_%s_W%d_%lld_%s/",
                                                                WHM_TMP_PATH,
                                                                event->type,
                                                                layer_name[event->layer_id],
                                                                event->id,
                                                                time_stamp_ms,
                                                                ifname);
  }
  if (sprintf_res <= 0 || sprintf_res >= sizeof(tmp_whm_folder)) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  /* validity check */
  if (!_safe_system_cmd_string(tmp_whm_folder, MAX_FILE_NAME_SIZE)) {
    ELOG_V("Invalid tmp_whm_folder path");
    res = MTLK_ERR_PARAMS;
    goto end;
  }

  if (stat(tmp_whm_folder, &st) == 0) {
    ILOG1_S("Do not create TMP DIR [%s], already exist", tmp_whm_folder);
  } else {
    ILOG1_S("create TMP DIR [%s]", tmp_whm_folder);
    mkdir(tmp_whm_folder, 0777);
  }

  ILOG1_S("[%s]", tmp_whm_folder);

  /* if event = MAC FATAL then take shram instead of fw_trace_buffer*/
  if ((event->layer_id == WHM_DRIVER_TRIGGER) && (event->id == MAC_FATAL_EVENT_ID)) {
    if (_fetch_fw_shram(ifname, tmp_whm_folder, &size) != MTLK_ERR_OK) {
      ELOG_V("_fetch_fw_shram FAILED");
      res = MTLK_ERR_UNKNOWN;
      goto end;
    }
  } else {
    /* on event which is not MAC FATAL collect fw_trace buffer */
    if (_fetch_fw_trace_buffer(event, ifname, tmp_whm_folder, &size) != MTLK_ERR_OK) {
      ELOG_V("_fetch_fw_trace_buffer FAILED");
      res = MTLK_ERR_UNKNOWN;
      goto end;
    }
  }

  /* estimate tar size */
  if (size > 0)
    acu_comp_size += (size/FW_TRACE_BUF_CR);

  /* collect driver logs */
  if (_fetch_driver_log(tmp_whm_folder, &size) != MTLK_ERR_OK) {
    ELOG_V("_fetch_driver_log FAILED");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }
  /* estimate tar size */
  if (size > 0)
    acu_comp_size += (size/DRIVER_LOG_CR);

  /*if event = MAC FATAL tarball total size will be set to fix size of WHM_DUMP_MAC_FATAL_SIZE*/
  /* calculate the remaining compressed area and convert to syslog un-compressed size */
  if ((event->layer_id == WHM_DRIVER_TRIGGER) && (event->id == MAC_FATAL_EVENT_ID)) {
     syslog_size = ((WHM_DUMP_TARBALL_SIZE(WHM_DUMP_MAC_FATAL_SIZE)) - acu_comp_size);
  } else {
     syslog_size = ((WHM_DUMP_TARBALL_SIZE(g_wh_config.tarball_size_kb)) - acu_comp_size);
  }
  ILOG1_DDD("syslog_size[%d] = WHM_DUMP_TARBALL_SIZE[%d] - acu_comp_size[%d]", syslog_size, (WHM_DUMP_TARBALL_SIZE(g_wh_config.tarball_size_kb)), acu_comp_size);
  /* getting the un-compressed size of syslog */
  syslog_size *= SYS_LOG_CR;

  /* collect hostapd logs */
  if (_fetch_sys_log(tmp_whm_folder, syslog_size) != MTLK_ERR_OK) {
    ELOG_V("_fetch_sys_log FAILED");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  /* collect env information */
  if (_fetch_env_info(tmp_whm_folder) != MTLK_ERR_OK) {
    ELOG_V("_fetch_sys_log FAILED");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  if (_zip_whm_files(event, tmp_whm_folder, storage_path, ifname, no_limit_dumps, num_of_dumps) != MTLK_ERR_OK) {
    ELOG_V("_zip_whm_files FAILED");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

end:

  ILOG1_V("END");

  if (stat(tmp_whm_folder, &st) == 0) {
    sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "rm -rf %s > /dev/null 2>&1", tmp_whm_folder);
    if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)) {
      ELOG_V("sprintf_s() error");
      res = MTLK_ERR_FILEOP;
    } else {
      system (sys_cmd);
      ILOG1_S("remove tmp_whm_folder[%s]", tmp_whm_folder);
    }
  }

  return res;
}

static void
_print_help (const char *app_name)
{
  const struct mtlk_argv_param_info_ex *all_params[] = {
    &param_path,
  };
  const char *app_fname = strrchr(app_name, '/');
  char  version[MAX_FILE_NAME_SIZE];
  int   sprintf_res = 0;

  if (!app_fname) {
    app_fname = app_name;
  }
  else {
    ++app_fname; /* skip '/' */
  }

  sprintf_res = sprintf_s(version, sizeof(version), "%s: %s\n%s",
                           "WHM dump evacuation application version", WHM_HANDLER_VERSION, "Debug: echo 6 cdebug=1 > /proc/net/mtlk_log/debug");

  if (sprintf_res <= 0 || sprintf_res >= sizeof(version)) {
    ELOG_V("sprintf_s() error");
  }

  ILOG0_S("%s\n", version);

  mtlk_argv_print_help(stdout,
                       app_fname,
                       version,
                       all_params,
                       (uint32)ARRAY_SIZE(all_params));
}

static int driver_whm_dump_event_handler(char *ifname, int drv_event_id, void *data, size_t len)
{
  whm_event event = {0};
  struct intel_vendor_whm_event_cfg *whm_event_handle;
  int res = MTLK_ERR_OK;

  if (data != NULL)
  {
    whm_event_handle = (struct intel_vendor_whm_event_cfg *)data;
    event.id = whm_event_handle->warning_id;
    event.layer_id = whm_event_handle->warning_layer;
    event.num_of_cards = whm_event_handle->num_cards;
    ILOG1_PDDD("data = %p : layer = %d : id : %d num_of_cards: %d\n",data,whm_event_handle->warning_layer,whm_event_handle->warning_id, whm_event_handle->num_cards);
  }
  else
  {
    ILOG1_V("Data(warning_layer,warning_id) is null\n");
  }

  wave_strncopy(event.type, "event", EVENT_TYPE_NAME_LEN, EVENT_TYPE_NAME_LEN);

  ILOG1_DSD("Received event '%d' from iface '%s' (len=%zu)", drv_event_id, ifname, len);

  mtlk_osal_strlcpy(g_wh_config.iface_name, ifname, sizeof(g_wh_config.iface_name));

  if (WHM_DRIVER_EVENT_ID != drv_event_id) {
    WLOG_D("Unexpected event %d received", drv_event_id);
    return 1;
  }

  res = _fetch_dumps(&event, g_wh_config.storage_path,
                     g_wh_config.iface_name,
                     g_wh_config.no_limit_dumps,
                     g_wh_config.num_of_dumps);

  if (MTLK_ERR_FILEOP == res) {
    g_wh_config.terminate = TRUE;
  }

  return 0;
}

static dwpald_driver_nl_event whm_handler_drv_events[] = {
  { WHM_DRIVER_EVENT_ID , driver_whm_dump_event_handler },
};

int whm_handler_dwpald_term_cond(void)
{
  return g_wh_config.terminate;
}

static void _listener(char * storage_path,
                        BOOL no_limit_dumps)
{
  dwpald_ret ret;
  int num_attach_attempts = 10;
  size_t num_drv_events = ARRAY_SIZE(whm_handler_drv_events);
  ILOG0_V("dwpal_daemon :_listener  new method init\n");

  g_wh_config.terminate = FALSE;
  g_wh_config.no_limit_dumps = no_limit_dumps;

  mtlk_osal_strlcpy(g_wh_config.storage_path, storage_path, sizeof(g_wh_config.storage_path));

  ILOG0_V("dwpal_daemon :dwpald_attach  init\n");

  ret = dwpald_connect("whm_handler");
  if (DWPALD_ERROR == ret) {
    ELOG_D("dwpal_daemon:dwpald_connect error %d", ret);
    return;
  }

again:
  ret = dwpald_nl_drv_attach(num_drv_events, whm_handler_drv_events, NULL);
  if (ret != DWPALD_SUCCESS) {
    ELOG_DD("dwpal_daemon:dwpald_nl_drv_attach error %d, num attempts left %d", ret, num_attach_attempts);
    if (num_attach_attempts-- <= 0) {
      sleep(1);
      goto again;
    }

    dwpald_disconnect();
    return;
  }

  ret = dwpald_start_blocked_listen(whm_handler_dwpald_term_cond);
  if (DWPALD_SUCCESS != ret) {
    ELOG_D("dwpal_daemon:dwpald_start_blocked_listen error %d", ret);
  }

  dwpald_disconnect();
}

#ifdef DEBUG_BUILD
static int check_if_usb_mounted(char * mnt_path, char * storage_path, BOOL * found_mount)
{

  int                 sprintf_res = 0;
  char                sys_cmd[MAX_CMD_SIZE];
  char                usb_path[MAX_FILE_NAME_SIZE] = {0};
  FILE                *pf;
  int                 res = MTLK_ERR_OK;
  int                 stat;

  *found_mount = FALSE;
  sprintf_res = sprintf_s(sys_cmd, sizeof (sys_cmd), "/bin/ls -d %s 2>/dev/null | /usr/bin/head -1",
                          mnt_path);
  if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)) {
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_UNKNOWN;
    return res;
  }

  if (!_safe_system_cmd_string(mnt_path, sizeof (sys_cmd))) {
    ELOG_V("unsafe command failure");
    res = MTLK_ERR_PARAMS;
    return res;
  }

  pf =  popen(sys_cmd, "r");
  if (pf && (fgets(usb_path, MAX_FILE_NAME_SIZE , pf) != NULL)) {
    stat = pclose(pf);
    if (WEXITSTATUS(stat) == 0) {
      if (*usb_path) {
        int len;
        wave_strncopy(storage_path, usb_path, MAX_FILE_NAME_SIZE, MAX_FILE_NAME_SIZE);
        len = mtlk_osal_strnlen(storage_path, MAX_FILE_NAME_SIZE);

        if (storage_path[len - 1] == '\n')
          storage_path[len - 1] = '\0';

        *found_mount = TRUE;
      }
    }
  }
  else {
    if (pf) {
      pclose(pf);
      pf = NULL;
    }
    ILOG0_S("USB mount path %s not found", mnt_path);
  }
  return res;
}
#endif

int
main(int argc, char *argv[])
{
  int                 res = MTLK_ERR_OK;
  mtlk_argv_parser_t  argv_parser;
  mtlk_argv_param_t   *param = NULL;
  int                 argvparser_inited = 0;
  const char          *user_path = NULL;
  char                storage_path[MAX_FILE_NAME_SIZE];
  BOOL                print_help = FALSE;
  BOOL                no_limit_dumps = FALSE;
#ifdef DEBUG_BUILD
  BOOL                found_mount = FALSE;
#endif
  BOOL                whm_enable = FALSE;
  unsigned int        whm_dump_tarball_size = 0;
  unsigned int        whm_max_dump_files = 0;
  int                 pid = getpid();
  struct              stat st = {0};

  ILOG0_SD("WHM dump evacuation application v.%s, pid = %d",
           MTLK_SOURCE_VERSION, pid);

  if (MTLK_ERR_OK != _mtlk_osdep_log_init(IWLWAV_RTLOG_APP_NAME_WHMHANDLER)) {
    ELOG_V("WHM dump evacuation _mtlk_osdep_log_init ERROR\n");
    return 1;
  }

#ifdef CONFIG_WAVE_RTLOG_REMOTE
    if (MTLK_ERR_OK != mtlk_rtlog_app_init(&rtlog_info_data, IWLWAV_RTLOG_APP_NAME_WHMHANDLER)) {
      ELOG_V("WHM dump evacuation application mtlk_rtlog_app_init ERROR\n");
      goto end;
    }
#endif

  res = manage_cfg_file(&whm_enable, &whm_dump_tarball_size, &whm_max_dump_files);
  if (res == MTLK_ERR_OK)
    ILOG0_DDD("CONFIG:whm_enable[%d], whm_dump_tarball_size[%d], whm_max_dump_files[%d]",
              whm_enable,
              whm_dump_tarball_size,
              whm_max_dump_files);

  if ((pid > 0) && ((res != MTLK_ERR_OK) || (whm_enable == 0))) {
    ILOG0_V("WHM is disabled or configuration is invalid:WHM handler is exiting...");
    exit(EXIT_SUCCESS);
  } else {
    ILOG0_D("WHM is enabled :WHM handler is running...PID[%d]", pid);
  }
  g_wh_config.num_of_dumps = whm_max_dump_files;
  g_wh_config.tarball_size_kb = whm_dump_tarball_size;

  mtlk_argv_parser_init(&argv_parser, argc, argv);
  argvparser_inited = 1;

  param = mtlk_argv_parser_param_get(&argv_parser, &param_path.info);
  if (param) {
    user_path = mtlk_argv_parser_param_get_str_val(param);
    mtlk_argv_parser_param_release(param);

    if (NULL == user_path) {
      ELOG_V("Invalid storage path");
      goto end;
    }
    if (stat(user_path, &st)) {
      ELOG_V("Storage path does not exist");
      goto end;
    }
  }
  else {
    ELOG_V("Storage path must be specified");
    print_help = TRUE;
    goto end;
  }
  mtlk_osal_strlcpy(storage_path, user_path, MAX_FILE_NAME_SIZE);

  /* sanity */
  if(!_safe_system_cmd_string(storage_path, MAX_FILE_NAME_SIZE)) {
    ELOG_V("Invalid storage path");
    goto end;
  }

  /* in debug mode: override storage path in case DOK was inserted*/
#ifdef DEBUG_BUILD
  res = check_if_usb_mounted(USB_MOUNT_PATH, storage_path, &found_mount);
  if (res != MTLK_ERR_OK)
    goto end;

  if (found_mount) {
    no_limit_dumps = TRUE;
  } else {
    res = check_if_usb_mounted(USB_MOUNT_PATH_PUMA, storage_path, &found_mount);
    if (res != MTLK_ERR_OK)
      goto end;

    if (found_mount)
      no_limit_dumps = TRUE;
  }
#endif

  _listener(storage_path, no_limit_dumps);

end:
  if (print_help)
    _print_help(argv[0]);

  if (argvparser_inited)
    mtlk_argv_parser_cleanup(&argv_parser);

  ILOG0_V("WHM handler is exiting...");

  return res;
}
