/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "dump_handler.h"
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

#ifndef CONFIG_USE_DWPAL_DAEMON
#define CONFIG_USE_DWPAL_DAEMON 0
#endif

#if CONFIG_USE_DWPAL_DAEMON
#if defined YOCTO
#include <wav-dpal/dwpal.h>
#include <wav-dpal/dwpald_client.h>
#else
#include "dwpal.h"
#include "dwpald_client.h"
#endif
#endif /* CONFIG_USE_DWPAL_DAEMON */

#ifdef CONFIG_WAVE_RTLOG_REMOTE
    #include "mtlk_rtlog_app.h"
#endif

#define LOG_LOCAL_GID   GID_DUMP_HANDLER
#define LOG_LOCAL_FID   1
#define DUMP_HANDLER_VERSION "1.1.1"

#define DUMP_HEADER_MAGIC "INTL"
#define DUMP_HEADER_MAGIC_SIZE 4
#define IFACE_NAME_LEN 6 /* wlanX */
#define MAX_FILE_NAME_SIZE 256
#define MAX_CMD_SIZE 1024
#define MAX_FW_FILES 100
#define BUF_SIZE 4096
#define FW_DUMP_FILE_PREFIX "/proc/net/mtlk/card"

/* FW trace buffer will be the same buffer log on all interface - TBD: relocate per card*/
/* e.g. /proc/net/mtlk/wlan0/Debug/fw_trace . MAX_LIMIT = 10KB */
#define FW_TRACE_BUF_FILE_PREFIX "/proc/net/mtlk/wlan"
#define FW_TRACE_BUF_FILE_SUFFIX "/fw_trace"
#define FW_TRACE_BUF_SIZE_MAX_LIMIT 10240
#define FW_TRACE_BUF_FILE_LOG_NAME "fw_trace"
#define FW_TRACE_BUF_HEADER_LEN_SIZE 4

#define FW_DUMP_FILE_SUFFIX "/FW/fw_dump"
#define FW_TMP_PATH "/tmp/fw_dump_files"
#define EVENT_LISTENER "/usr/sbin/dwpal_cli"
#define EVENT_LISTENER_PUMA "/usr/bin/dwpal_cli"
#define MAX_DUMP_TAR_FILES 1
#define DEF_NUM_OF_LATEST_DUMPS_TO_KEEP 1
#define DEF_NUM_OF_OLDEST_DUMPS_TO_KEEP 2
#define LEAVE_FREE_SPACE_ON_FS_IN_KB 512
#define DUMP_FILE_AGING_TIME_IN_SEC 24 * 60 * 60 /* 24 hrs */

#define FW_WHM_SHRAM_TMP_FOLDER "/tmp/whm_shram"

#define MTLK_STRINGIFY(NUM) #NUM
#define MTLK_TOSTRING(NUM) MTLK_STRINGIFY(NUM)

#ifdef DEBUG_BUILD
#define USB_MOUNT_PATH "/mnt/usb/*/"
#define USB_MOUNT_PATH_PUMA "/tmp/mnt/*/"
#endif

#define OUI_LTQ 0xAC9A96

static time_t time_last_dump_saved = 0;

#if CONFIG_USE_DWPAL_DAEMON
struct dump_handler_cfg
{
  int card_idx;
  char fw_dump_filename[MAX_FILE_NAME_SIZE];
  char storage_path[MAX_FILE_NAME_SIZE];
  uint32 no_files_to_keep;
  BOOL no_limit_dumps;
  BOOL terminate;
};

struct dump_handler_cfg g_dh_config;
#endif /* CONFIG_USE_DWPAL_DAEMON */

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static rtlog_app_info_t rtlog_info_data;
#endif

typedef struct {
  char name[MAX_FILE_NAME_SIZE];
  unsigned int size;
} fw_file;

static const struct mtlk_argv_param_info_ex param_card_idx =  {
  {
    "i",
    "card_idx",
    MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
  },
  "card index",
  MTLK_ARGV_PTYPE_MANDATORY
};

static const struct mtlk_argv_param_info_ex param_path =  {
  {
    "f",
    "storage_path",
    MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
  },
  "persistent storage path",
  MTLK_ARGV_PTYPE_MANDATORY
};

static const struct mtlk_argv_param_info_ex param_offline_dump =  {
  {
    "d",
    "offline_dump",
    MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
  },
  "parse offline dump",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_no_files_to_keep =  {
  {
    "k",
    "no_files_to_keep",
    MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
  },
  "Number of dump files to keep (unless USB storage is used)",
  MTLK_ARGV_PTYPE_OPTIONAL
};


static BOOL _safe_system_cmd_string (const char * string, int size){
  int i;
  for(i=0; i<size && string[i]!=0; i++){
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

static int get_free_space (const char *storage_path,
                           unsigned long long *free_space_in_fs){
  struct statvfs fs_stat;

  if (statvfs(storage_path, &fs_stat) != 0)
    return MTLK_ERR_UNKNOWN;

  *free_space_in_fs = fs_stat.f_bavail * fs_stat.f_bsize;
  return MTLK_ERR_OK;
}

/* Make sure file path does not contain unsafe strings */
static char * get_safe_path(const char * path){
  char *verified_path = NULL;

  if (NULL == strstr (path, "../") && NULL == strstr(path, "%00")){
    /* Extra buffer required in order to avoid ckockwork errors */
    verified_path = malloc(MAX_FILE_NAME_SIZE);
    mtlk_osal_strlcpy(verified_path, path, MAX_FILE_NAME_SIZE);
    return verified_path;
  }
  else{
    return NULL;
  }
}

static void _zip_fw_files (const char *storage_path, int card_idx,
                           BOOL no_limit_dumps, uint32 no_files_to_keep){
  char                system_cmd[MAX_CMD_SIZE], out_str[MAX_CMD_SIZE];
  char                * b_name = NULL;
  char                * d_name = NULL;
  int                 sprintf_res = 0;
  char                * tmp_dump_dir = NULL;
  char                tmp_dump_dir_full_path[MAX_FILE_NAME_SIZE];
  char                tar_file_name[MAX_FILE_NAME_SIZE];
  char                old_dump_filename[MAX_FILE_NAME_SIZE + 1] = {0};
  char                * old_dump_filename_verified = NULL;
  char                * out_str_verified = NULL;
  char                * tmp_dump_dir_parent = NULL;
  int                 full_path_length = 0;
  time_t              curr_time = time(NULL);
  struct              tm *tm = localtime(&curr_time);
  unsigned long long  free_space_in_fs = 0;
  FILE                *pf = NULL;
  BOOL                zip_file_removed = FALSE;
  BOOL                single_dump_file = FALSE;
  int                 old_dump_size = 0;
  int                 status, no_old_files_to_keep;
  struct stat         sb = {0};

  if (NULL == tm) {
    ELOG_V("Failed to convert time to localtime");
    goto end;
  }

  /* sanity */
  if(!_safe_system_cmd_string(storage_path, MAX_FILE_NAME_SIZE)){
    ELOG_V("Invalid storage path");
    goto end;
  }

  sprintf_res = sprintf_s(tmp_dump_dir_full_path,
                          sizeof(tmp_dump_dir_full_path),
                          "%s_card_%d/", FW_TMP_PATH, card_idx);

  if(sprintf_res <= 0 || sprintf_res >= sizeof(tmp_dump_dir_full_path)){
    ELOG_V("FW file tar command failure");
    goto end;
  }

  full_path_length = mtlk_osal_strnlen(tmp_dump_dir_full_path, MAX_FILE_NAME_SIZE) + 1;

  b_name = malloc(full_path_length);
  if (!b_name){
    ELOG_V("FW file tar command failure");
    goto end;
  }
  mtlk_osal_strlcpy(b_name, tmp_dump_dir_full_path, MAX_FILE_NAME_SIZE);
  tmp_dump_dir = basename(b_name);

  d_name = malloc(full_path_length);
  if (!d_name){
    ELOG_V("FW file tar command failure");
    goto end;
  }
  mtlk_osal_strlcpy(d_name, tmp_dump_dir_full_path, MAX_FILE_NAME_SIZE);
  tmp_dump_dir_parent = dirname(d_name);

  if (no_limit_dumps)
    goto save_dumps;

  if (get_free_space(storage_path, &free_space_in_fs) != MTLK_ERR_OK){
    ELOG_V("Getting available free space has failed.");
    goto save_dumps;
  }

  if (no_files_to_keep == 0)
    no_files_to_keep = DEF_NUM_OF_OLDEST_DUMPS_TO_KEEP + DEF_NUM_OF_LATEST_DUMPS_TO_KEEP;

  /* Check if there is only one dump on fs + dump file size and name: */
  sprintf_res = sprintf_s(system_cmd, sizeof (system_cmd),
                          "[ `/bin/ls -l %s/fw_dump_*tar.gz 2>/dev/null | "
                          "/usr/bin/wc -l` -eq 1 ] && "
                          "/bin/ls -l %s/fw_dump_*tar.gz 2>/dev/null | "
                          "/usr/bin/head -1 | /usr/bin/awk '{print $5, $NF}'",
                          storage_path, storage_path);
  if (sprintf_res <= 0 || sprintf_res >= sizeof (system_cmd)){
    ELOG_V("fw dump file check command has failed");
    goto save_dumps;
  }

  pf =  popen(system_cmd, "r");
  if (pf && (fgets(out_str, MAX_FILE_NAME_SIZE , pf) != NULL)){
    status = pclose(pf);
    pf = NULL;
    if (WEXITSTATUS(status) == 0 && *out_str){

      out_str[MAX_FILE_NAME_SIZE - 1] = '\0';
      out_str_verified = get_safe_path(out_str);
      if (out_str_verified == NULL){
        ELOG_S("Invalid path for old dump files, %s", out_str);
        goto save_dumps;
      }

      if (sscanf(out_str_verified, "%d %" MTLK_TOSTRING(MAX_FILE_NAME_SIZE) "s\r",
          &old_dump_size, old_dump_filename) == 2){
        single_dump_file = TRUE;
      }
    }
  }
  else {
    if (pf){
      pclose(pf);
      pf = NULL;
    }
  }

  /* In case there is only one old dump file and not enough space to create
   * another one (and leave 0.5 MB free) or user wants to keep only 1 file on the fs:
   * If no dumps were created since dump handler was started or dump file has
   * aged (older than 24 hrs) replace with a newer dump. Otherwise, keep the old one.
   */
  if (single_dump_file &&
      (mtlk_osal_strnlen (old_dump_filename, MAX_FILE_NAME_SIZE) > 0) &&
      (no_files_to_keep == 1 || (free_space_in_fs < (old_dump_size + LEAVE_FREE_SPACE_ON_FS_IN_KB * 1024)))){
    if (difftime(curr_time, time_last_dump_saved) > DUMP_FILE_AGING_TIME_IN_SEC){
      old_dump_filename [MAX_FILE_NAME_SIZE - 1] = '\0';
      old_dump_filename_verified = get_safe_path(old_dump_filename);
      if (old_dump_filename_verified == NULL){
        ELOG_S("Removing old dump file %s failed due to wrong format", old_dump_filename);
        goto save_dumps;
      }

      if (stat(old_dump_filename_verified, &sb) == -1) {
        ELOG_S("Removing old dump file %s failed", old_dump_filename);
        goto save_dumps;
      }
      if (!S_ISREG(sb.st_mode)){
        ELOG_S("Removing old dump file %s failed, not a regular file", old_dump_filename_verified);
        goto save_dumps;
      }
      if (remove (old_dump_filename_verified) != 0)
        ELOG_S("Removing old dump file %s failed", old_dump_filename_verified);
      else
        ELOG_S("Removed old dump file %s", old_dump_filename_verified);

      goto save_dumps;

    } else {
      ILOG0_V("The existing fw dump hasn't aged, will not create a new fw dump file");
      goto end;
    }
  }

  /* multiple old dump files on FS, keep oldest and latest files */
  /* Make sure that we keep at least one recent dump file */
    no_old_files_to_keep = DEF_NUM_OF_OLDEST_DUMPS_TO_KEEP < no_files_to_keep ? DEF_NUM_OF_OLDEST_DUMPS_TO_KEEP : no_files_to_keep - 1;

  if (no_old_files_to_keep == 0)
    sprintf_res = sprintf_s(system_cmd, sizeof (system_cmd),
                            "/bin/rm %s/fw_dump_*tar.gz > /dev/null 2>&1;",
                            storage_path);
  else
    sprintf_res = sprintf_s(system_cmd, sizeof (system_cmd),
                            "files=`/bin/ls -tr1 %s/fw_dump_*tar.gz 2>/dev/null`;"
                            "if [ -n \"$files\" ]; then"
                            "  count=`/bin/echo $files | /usr/bin/wc -w 2>/dev/null`;"
                            "  if [ $count -gt %d ]; then"
                            "    files=`/bin/echo $files | /usr/bin/tr \"' '\" \"'\\n'\"| /usr/bin/tail -n $((count-%d)) | /usr/bin/head -n $((count-%d))`;"
                            "    /bin/echo $files | /usr/bin/xargs /bin/rm > /dev/null 2>&1;"
                            "  fi;"
                            "fi;",
                            storage_path,
                            no_files_to_keep - 1,
                            no_old_files_to_keep,
                            no_files_to_keep - 1);

  if (sprintf_res <= 0 || sprintf_res >= sizeof (system_cmd)){
    ELOG_V("Old file removal command has failed");
    goto save_dumps;
  }
  system(system_cmd);

save_dumps:
  /* tar+zip fw dump files */
  sprintf_res = sprintf_s(tar_file_name, sizeof (tar_file_name),
                          "fw_dump_%d_%02d_%02d_%02d_%02d_%02d.tar.gz",
                          tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                          tm->tm_hour, tm->tm_min, tm->tm_sec);
  if (sprintf_res <= 0 || sprintf_res >= sizeof (system_cmd)){
    ELOG_V("FW file tar filename failure");
    goto end;
  }

  sprintf_res = sprintf_s(system_cmd, sizeof (system_cmd),
                          "/bin/tar -C %s -czf %s/%s %s > /dev/null 2>&1",
                          tmp_dump_dir_parent, storage_path, tar_file_name, tmp_dump_dir);
  if((!_safe_system_cmd_string(tmp_dump_dir_parent, full_path_length)) ||
     (!_safe_system_cmd_string(tmp_dump_dir, full_path_length)) ||
     (!_safe_system_cmd_string(tar_file_name, full_path_length)) ||
     (!_safe_system_cmd_string(tmp_dump_dir_full_path, full_path_length)) ||
      sprintf_res <= 0 || sprintf_res >= sizeof (system_cmd)){
    ELOG_V("FW file tar command failure");
    goto end;
  }

  if (system(system_cmd)){
    ELOG_V("FW file tar command returned error");
  }
  else {
      time_last_dump_saved = time(NULL);
  }

  /* check remaining space on storage device */
  if (get_free_space (storage_path, &free_space_in_fs) == MTLK_ERR_OK){
    /* leave at least ~0.5MB on persistent storage */
    if (free_space_in_fs <= (LEAVE_FREE_SPACE_ON_FS_IN_KB * 1024)){
      ELOG_V("TELEMETRY: not enough space left on storage device, removing dump file");
      sprintf_res = sprintf_s(out_str, sizeof (out_str), "%s/%s",
                              storage_path, tar_file_name);
      if (sprintf_res <= 0 || sprintf_res >= sizeof (out_str)){
        ELOG_V("File removal failed");
      }
      else {

        if (out_str_verified){
          free(out_str_verified);
          out_str_verified = NULL;
        }
        out_str[MAX_FILE_NAME_SIZE - 1] = '\0';
        out_str_verified = get_safe_path(out_str);
        if (out_str_verified == NULL){
          ELOG_S("Removing dump file %s failed due to wrong format", out_str);
          goto end;
        }

        if (stat(out_str_verified, &sb) == -1) {
          ELOG_S("Removing dump file %s failed", out_str_verified);
          goto end;
        }
        if (!S_ISREG(sb.st_mode)){
          ELOG_S("Removing dump file %s failed, not a regular file", out_str_verified);
          goto end;
        }
        if (remove (out_str_verified) != 0)
          ELOG_V("Removing dump file to free up space failed");
        else
          zip_file_removed = TRUE;
      }
    }
  }
  else {
    ELOG_V("Error getting free space on filesystem");
  }

  if (!zip_file_removed){
    ILOG0_SS("TELEMETRY: Firmware dump tar file created. Location: %s/%s",
            storage_path, tar_file_name);
    printf("Dump Handler: Firmware dump file saved to: %s/%s\n",
            storage_path, tar_file_name);
  }

end:
  if (b_name)
    free(b_name);
  if (d_name)
    free(d_name);
  if (out_str_verified)
    free(out_str_verified);
  if(old_dump_filename_verified)
    free(old_dump_filename_verified);
}

static int
_fetch_fw_trace_buffer (int card_idx){
  FILE          *dump_file = NULL;
  FILE          *out_file = NULL;
  char          fw_trace_buffer_filename[MAX_FILE_NAME_SIZE];
  unsigned int  remaining = 0;
  unsigned int  read, written;
  char          out_file_full_name[MAX_FILE_NAME_SIZE];
  char          tmp_fw_folder[MAX_FILE_NAME_SIZE];
  unsigned char buf[BUF_SIZE];
  int           sprintf_res = 0;
  int           res = MTLK_ERR_OK;
  struct        stat st = {0};

  ILOG1_V("ENTRY");

  if (card_idx < 0 || card_idx > 2){
	ELOG_V("card_idx wrong boundaries");  
	res = MTLK_ERR_VALUE;
   goto end;
  }	

  sprintf_res = sprintf_s(fw_trace_buffer_filename, sizeof(fw_trace_buffer_filename), "%s%d%s",
                           FW_TRACE_BUF_FILE_PREFIX, card_idx*2, FW_TRACE_BUF_FILE_SUFFIX);

  if (sprintf_res <= 0 || sprintf_res >= sizeof(fw_trace_buffer_filename)){
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  ILOG1_S("[%s]", fw_trace_buffer_filename);
  
  dump_file = fopen(fw_trace_buffer_filename, "rb");
  if (dump_file == NULL){
    ELOG_S("Cannot open firmware dump file[%s]", fw_trace_buffer_filename);
    res = MTLK_ERR_FILEOP;
    goto end;
  }

  ILOG1_S("[%s] opened", fw_trace_buffer_filename);

  /* get fw_trace buffer size */
  if (!fread(&remaining, sizeof(remaining), 1, dump_file)){
	ELOG_S("Invalid fw_trace buffer, missing file size %s", fw_trace_buffer_filename);
	res = MTLK_ERR_UNKNOWN;
	goto end;
  }

  if (remaining > FW_TRACE_BUF_SIZE_MAX_LIMIT){
	ELOG_DD("Invalid fw_trace buffer, buffer size[%d] > FW_TRACE_BUF_SIZE_MAX_LIMIT[%d]",
		remaining, FW_TRACE_BUF_SIZE_MAX_LIMIT);
    res = MTLK_ERR_VALUE;
    goto end;	 
  }

  ILOG1_DD("buffer size=[0x%x][%d]", remaining, remaining);
  
  sprintf_res = sprintf_s(tmp_fw_folder, sizeof(tmp_fw_folder), "%s_card_%d/", FW_TMP_PATH, card_idx);
  if (sprintf_res <= 0 || sprintf_res >= sizeof(tmp_fw_folder)){
	 ELOG_V("sprintf_s() error");
	 res = MTLK_ERR_UNKNOWN;
	 goto end;
  }
  
    /* if the dir exist skip, if not create */
  if (stat(tmp_fw_folder, &st) == 0) {
	ILOG1_S("Do not create TMP DIR [%s], already exist", tmp_fw_folder);
  } else {
	ILOG1_S("create TMP DIR [%s]", tmp_fw_folder); 
	mkdir(tmp_fw_folder, 0700);
  }

  ILOG1_S("[%s]", tmp_fw_folder);

  sprintf_res = sprintf_s(out_file_full_name, MAX_FILE_NAME_SIZE, "%s/%s", tmp_fw_folder,
                          FW_TRACE_BUF_FILE_LOG_NAME);
  if (sprintf_res <= 0 || sprintf_res >= MAX_FILE_NAME_SIZE){
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  ILOG1_S("[%s]", out_file_full_name);

  out_file = fopen(out_file_full_name, "wb+");
  if (out_file == NULL){
    ELOG_S("Cannot open output dump file %s", out_file_full_name);
    res = MTLK_ERR_FILEOP;
    goto end;
  }

  /* read the fw_trace buffer and write to tmp_fw_folder location for compressing*/
  while(remaining > 0 && remaining < FW_TRACE_BUF_SIZE_MAX_LIMIT){
	  
  ILOG1_SDD("out_file_full_name[%s], remaining[%d], numB2read[%d]",
           out_file_full_name,remaining , (remaining < BUF_SIZE ?remaining:BUF_SIZE));  
	  
   read = fread(buf, 1, remaining < BUF_SIZE ?remaining:BUF_SIZE, dump_file);

   if (!read){
      ELOG_S("Error reading %s", FW_TRACE_BUF_FILE_LOG_NAME);
      res = MTLK_ERR_FILEOP;
      goto end;
   }
   remaining -= read;

   while (read){
     written = fwrite (buf, 1, read, out_file);
     if (!written){
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

end:

  ILOG1_V("END");

  if (dump_file)
    fclose(dump_file);

  if (out_file)
    fclose (out_file);

  return res;

}


static int
_fetch_dumps (char *fw_dump_filename, const char *storage_path, int card_idx,
              BOOL no_limit_dumps, uint32 no_files_to_keep){
  FILE                *dump_file = NULL;
  FILE                *out_file = NULL;
  FILE                *out_file_shram = NULL;
  char                line[MAX_FILE_NAME_SIZE];
  int                 header_size=0;
  unsigned char       buf[BUF_SIZE];
  fw_file             fw_files [MAX_FW_FILES];
  int                 num_files = 0;
  int                 cur_file;
  struct              stat st = {0};
  char                tmp_fw_folder[MAX_FILE_NAME_SIZE];
  int                 res = MTLK_ERR_OK;
  int                 sprintf_res = 0;
  char                dump_header_magic[DUMP_HEADER_MAGIC_SIZE+1];
  char                sys_cmd[MAX_CMD_SIZE];
  char                shram_orig[MAX_FILE_NAME_SIZE];
  char                shram_tmp[MAX_FILE_NAME_SIZE];
  BOOL                is_start = TRUE;
  BOOL                is_file_empty = TRUE;
  int                 i;


  MTLK_ASSERT(MAX_FILE_NAME_SIZE >= DUMP_HEADER_MAGIC_SIZE);

  /*
   * Header format:
   * -Magic identifier, 4 bytes: INTL (ASCII): 0x49, 0x4E, 0x54, 0x4C
   * -LF
   * The following is repeated per file:
   *   -File name in ASCII
   *   -LF
   *   -File size in ASCII hex string
   *   -LF
   * -Marker of the end of the header, 4 bytes ‘<<<<’: 0x3C, 0x3C, 0x3C, 0x3C
   * -LF
   * -Data. A concatenation of the dump files.
   * we only need read permissions
  */
  ILOG1_V("ENTRY");
  /* TBD */
  /*_fetch_fw_trace_buffer(card_idx);*/
  
  dump_file = fopen(fw_dump_filename, "rb");
  if (dump_file == NULL){
    ELOG_S("Cannot open firmware dump file %s", fw_dump_filename);
    res = MTLK_ERR_FILEOP;
    goto end;
  }

  /* Read file header */
  while (fgets(line, sizeof(line), dump_file)) {
    if (line[0] == '\0'){
      ELOG_S("Invalid firmware dump file %s", fw_dump_filename);
      res = MTLK_ERR_UNKNOWN;
      goto end;
    }

    is_file_empty = FALSE;

    mtlk_osal_strlcpy(dump_header_magic, DUMP_HEADER_MAGIC, DUMP_HEADER_MAGIC_SIZE+1);

    /*Done like this to avoid klocwork errors*/
    for (i=0;i<DUMP_HEADER_MAGIC_SIZE;i++){
      if (dump_header_magic[i] != line[i]){
        is_start = FALSE;
        break;
      }
    }

    /*Verify header magic*/
    if (header_size == 0 && !is_start){
      ELOG_S("Invalid firmware dump file %s", fw_dump_filename);
      res = MTLK_ERR_UNKNOWN;
      goto end;
    }

    header_size += mtlk_osal_strnlen (line, sizeof(line));
		
    /* look for where the header ends */
    if (line[0] == '<')
      break;

    if (!is_start){
      line [(mtlk_osal_strnlen (line, sizeof(line))) -1 ] = '\0';
      mtlk_osal_strlcpy(fw_files[num_files].name, line, MAX_FILE_NAME_SIZE);

      /* get file size */
      if (!fgets(line, sizeof(line), dump_file)){
        ELOG_S("Invalid firmware dump format, missing file size %s", fw_dump_filename);
        res = MTLK_ERR_UNKNOWN;
        goto end;
      }

      if (sscanf(line, "%x\r", &fw_files[num_files].size) != 1){
        ELOG_S("Invalid firmware dump format, wrong file size %s", fw_dump_filename);
        res = MTLK_ERR_UNKNOWN;
        goto end;
      }

      num_files ++;

    }

  }

  if (is_file_empty) {
    ELOG_S("Firmware dump file %s is empty", fw_dump_filename);
    res = MTLK_ERR_NO_ENTRY;
    goto end;
  }

  if (!num_files) {
    ELOG_S("Firmware dump file %s contains 0 files", fw_dump_filename);
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  /* break up dump file to seperate files */
  sprintf_res = sprintf_s(tmp_fw_folder, sizeof(tmp_fw_folder), "%s_card_%d/", FW_TMP_PATH, card_idx);
  if (sprintf_res <= 0 || sprintf_res >= sizeof(tmp_fw_folder)){
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  /* if the dir exist skip ,if not create*/
  if (stat(tmp_fw_folder, &st) == 0) {
	ILOG1_S("Do not create TMP DIR [%s], already exist", tmp_fw_folder);
  } else {
	ILOG1_S("create TMP DIR [%s]", tmp_fw_folder); 
	mkdir(tmp_fw_folder, 0700);
  }

  /* if the dir exist skip ,if not create*/
  if (stat(FW_WHM_SHRAM_TMP_FOLDER, &st) == 0) {
    ILOG1_S("Do not create TMP DIR [%s], already exist", FW_WHM_SHRAM_TMP_FOLDER);
    } else {
      ILOG1_S("create TMP DIR [%s]", FW_WHM_SHRAM_TMP_FOLDER);
      mkdir(FW_WHM_SHRAM_TMP_FOLDER, 0777);
    }

  for (cur_file = 0; cur_file < num_files ; cur_file++){
    int remaining = fw_files[cur_file].size;
    int read;
    int written = 0, written_shram = 0;
    char out_file_full_name[MAX_FILE_NAME_SIZE];
    char out_file_full_name_shram[MAX_FILE_NAME_SIZE];
    BOOL shram_detected = FALSE;

    if (!strcmp(fw_files[cur_file].name, "shram")) {
      shram_detected = TRUE;
      ILOG1_S("shram_detected:fw_files[cur_file].name [%s] in fw_dump tmp folder", fw_files[cur_file].name);
    } else {
      shram_detected = FALSE;
    }

    sprintf_res = sprintf_s(out_file_full_name, MAX_FILE_NAME_SIZE, "%s/%s", tmp_fw_folder,
                            fw_files[cur_file].name);
    if (sprintf_res <= 0 || sprintf_res >= MAX_FILE_NAME_SIZE){
      ELOG_V("sprintf_s() error");
      res = MTLK_ERR_UNKNOWN;
      goto end;
    }
    if (shram_detected) {
      sprintf_res = sprintf_s(out_file_full_name_shram, MAX_FILE_NAME_SIZE, "%s/%s", FW_WHM_SHRAM_TMP_FOLDER,
                              fw_files[cur_file].name);
      if (sprintf_res <= 0 || sprintf_res >= MAX_FILE_NAME_SIZE){
        ELOG_V("sprintf_s() error");
        res = MTLK_ERR_UNKNOWN;
        goto end;
      }
    }
    out_file = fopen(out_file_full_name, "wb+");
    if (out_file == NULL){
      ELOG_S("Cannot open output dump file %s", fw_files[cur_file].name);
      res = MTLK_ERR_FILEOP;
      goto end;
    }
    if (shram_detected) {
      out_file_shram = fopen(out_file_full_name_shram, "wb+");
      if (out_file_shram == NULL){
        ELOG_S("Cannot open output dump file %s", fw_files[cur_file].name);
        res = MTLK_ERR_FILEOP;
        goto end;
      }
    }
    while(remaining > 0){

      read = fread(buf, 1, remaining < BUF_SIZE ?remaining:BUF_SIZE, dump_file);

      if (!read){
        ELOG_S("Error reading %s", fw_files[cur_file].name);
        res = MTLK_ERR_FILEOP;
        goto end;
      }
      remaining -= read;

      while (read){
        written = fwrite (buf, 1, read, out_file);
        if (shram_detected)
          written_shram = fwrite (buf, 1, read, out_file_shram);
        if (!written && !written_shram) {
          ELOG_S("Error writing %s", fw_files[cur_file].name);
          res = MTLK_ERR_FILEOP;
          goto end;
        }
        read -= written;
      }
    }

    fclose (out_file);
    if (shram_detected)
      fclose (out_file_shram);
    out_file = NULL;
    out_file_shram = NULL;
  }

  _zip_fw_files(storage_path, card_idx, no_limit_dumps, no_files_to_keep);

end:

  ILOG1_V("END");

  if (dump_file)
    fclose(dump_file);

  if (out_file)
    fclose (out_file);

  if (out_file_shram)
    fclose (out_file_shram);

  /* if the dir exist remove it */
  if (stat(tmp_fw_folder, &st) == 0) {
    sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "rm -rf %s > /dev/null 2>&1", tmp_fw_folder);
    if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)){
      ELOG_V("sprintf_s() error");
	  res = MTLK_ERR_FILEOP;
    } else {
      system (sys_cmd);
	  ILOG1_S("remove tmp_fw_folder[%s]", tmp_fw_folder);
	}
  }

  return res;
}

static void
_print_help (const char *app_name)
{
  const struct mtlk_argv_param_info_ex *all_params[] = {
    &param_card_idx,
    &param_path,
    &param_offline_dump,
    &param_no_files_to_keep,
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
                           "Firmware dump evacuation application version", DUMP_HANDLER_VERSION, "Debug: echo 6 cdebug=1 > /proc/net/mtlk_log/debug");

  if (sprintf_res <= 0 || sprintf_res >= sizeof(version)){
    ELOG_V("sprintf_s() error");
  }
  
  ILOG0_S("%s\n", version);

  mtlk_argv_print_help(stdout,
                       app_fname,
                       version,
                       all_params,
                       (uint32)ARRAY_SIZE(all_params));
}

/*Check if recovery has already happend before this script was initiated
*(posibly before nl80211 has been initialized) */
static BOOL _rcvry_happend(char *dump_file_path){
  char                sys_cmd[MAX_CMD_SIZE];

  int sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "/usr/bin/head -1 %s  2>/dev/null | /bin/grep %s > /dev/null",
                              dump_file_path, DUMP_HEADER_MAGIC);
  if((!_safe_system_cmd_string(dump_file_path, sizeof (sys_cmd))) ||
     (!_safe_system_cmd_string(DUMP_HEADER_MAGIC, sizeof (sys_cmd)))){
    ELOG_V("unsafe command failure");
    return FALSE;
  }
  if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)){
    ELOG_V("sprintf_s() error");
    return FALSE;
  }
  return (system (sys_cmd) == 0);
}

#if CONFIG_USE_DWPAL_DAEMON
static int fw_dump_ready_event_handler(char *ifname, int drv_event_id, void *data, size_t len)
{
  int res;
  struct intel_vendor_fw_dump_ready_info dump_ready_event_info;

  ILOG1_DSD("Received event '%d' from iface '%s' (len=%zu)", drv_event_id, ifname, len);

  if (LTQ_NL80211_VENDOR_EVENT_ASSERT_DUMP_READY != drv_event_id) {
    WLOG_D("Unexpected event %d received", drv_event_id);
    return 1;
  }

  if (len != sizeof(dump_ready_event_info)) {
    WLOG_DD("Expected event data size %zu but received %zu", sizeof(dump_ready_event_info), len);
    return 1;
  }

  wave_memcpy(&dump_ready_event_info, sizeof(dump_ready_event_info), data, len);
  if (dump_ready_event_info.card_idx != g_dh_config.card_idx) {
    ILOG1_DD("Event received from different card_idx (%d) than listening to (%d)",
             dump_ready_event_info.card_idx, g_dh_config.card_idx);
    return 0;
  }

  res = _fetch_dumps(g_dh_config.fw_dump_filename,
                     g_dh_config.storage_path,
                     g_dh_config.card_idx,
                     g_dh_config.no_limit_dumps,
                     g_dh_config.no_files_to_keep);

  if (MTLK_ERR_FILEOP == res) {
    g_dh_config.terminate = TRUE;
  }

  return 0;
}

static dwpald_driver_nl_event dump_handler_drv_events[] = {
  { LTQ_NL80211_VENDOR_EVENT_ASSERT_DUMP_READY , fw_dump_ready_event_handler },
};

int dump_handler_dwpald_term_cond(void)
{
  return g_dh_config.terminate;
}

static void _listener(char * iface_name, char * fw_dump_filename,
                      char * storage_path, int card_idx, BOOL no_limit_dumps,
                      uint32 no_files_to_keep)
{
  dwpald_ret ret;
  int num_attach_attempts = 10;
  char app_name[MAX_FILE_NAME_SIZE];
  size_t num_drv_events = ARRAY_SIZE(dump_handler_drv_events);

  MTLK_UNREFERENCED_PARAM(iface_name);

  g_dh_config.terminate = FALSE;
  g_dh_config.card_idx = card_idx;
  g_dh_config.no_limit_dumps = no_limit_dumps;
  g_dh_config.no_files_to_keep = no_files_to_keep;
  mtlk_osal_strlcpy(g_dh_config.fw_dump_filename, fw_dump_filename, sizeof(g_dh_config.fw_dump_filename));
  mtlk_osal_strlcpy(g_dh_config.storage_path, storage_path, sizeof(g_dh_config.storage_path));

  if (sprintf_s(app_name, sizeof(app_name), "dump_handler%d", card_idx) <= 0)
    return;

  ret = dwpald_connect(app_name);
  if (DWPALD_ERROR == ret) {
    ELOG_D("dwpal_daemon:dwpald_connect error %d", ret);
    return;
  }

again:
  ret = dwpald_nl_drv_attach(num_drv_events, dump_handler_drv_events, NULL);
  if (ret != DWPALD_SUCCESS) {
    ELOG_DD("dwpal_daemon:dwpald_nl_drv_attach error %d, num attempts left %d", ret, num_attach_attempts);
    if (num_attach_attempts-- <= 0) {
      sleep(1);
      goto again;
    }

    dwpald_disconnect();
    return;
  }

  ret = dwpald_start_blocked_listen(dump_handler_dwpald_term_cond);
  if (DWPALD_SUCCESS != ret) {
    ELOG_D("dwpal_daemon:dwpald_start_blocked_listen error %d", ret);
  }

  dwpald_disconnect();
}
#else
static void _listener(char * iface_name, char * fw_dump_filename,
                      char * storage_path, int card_idx, BOOL no_limit_dumps,
                      uint32 no_files_to_keep){
  char                sys_cmd[MAX_CMD_SIZE];
  int                 res = MTLK_ERR_OK;
  int                 sprintf_res = 0;

  /*listen to d-wpal dumps-ready event*/
  /*We want to exit only in case of fatal error*/
  while (res != MTLK_ERR_FILEOP) {
    sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "EVENT_LISTENER=%s;[ -e $EVENT_LISTENER ] || EVENT_LISTENER=%s;"
                            "$EVENT_LISTENER -iDriver -v%s -l\"FW_DUMP_READY\"",
                            EVENT_LISTENER, EVENT_LISTENER_PUMA,  iface_name);
    if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)){
      ELOG_V("sprintf_s() error");
      return;
    }
    system(sys_cmd);
    res = _fetch_dumps(fw_dump_filename, storage_path, card_idx, no_limit_dumps, no_files_to_keep);
  }
}
#endif /* CONFIG_USE_DWPAL_DAEMON */

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

  if (!_safe_system_cmd_string(mnt_path, sizeof (sys_cmd))){
    ELOG_V("unsafe command failure");
    res = MTLK_ERR_PARAMS;
    return res;
  }

  if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)){
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_UNKNOWN;
    return res;
  }

  pf =  popen(sys_cmd, "r");
  if (pf && (fgets(usb_path, MAX_FILE_NAME_SIZE , pf) != NULL)){
    stat = pclose(pf);
    if (WEXITSTATUS(stat) == 0){
      if (*usb_path){
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
    if (pf){
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
  int                 res = MTLK_ERR_UNKNOWN;
  mtlk_argv_parser_t  argv_parser;
  mtlk_argv_param_t   *param = NULL;
  int                 argvparser_inited = 0;
  char                iface_name[IFACE_NAME_LEN] = {0};
  const char          *user_path = NULL;
  const char          *dump_path = NULL;
  char                storage_path[MAX_FILE_NAME_SIZE];
  char                dump_file_path[MAX_FILE_NAME_SIZE];
  BOOL                print_help = FALSE;
  BOOL                no_limit_dumps = FALSE;
  BOOL                offline_dump = FALSE;
  uint32              no_files_to_keep = 0;
  int                 card_idx;
  int                 sprintf_res = 0;
  char                fw_dump_filename[MAX_FILE_NAME_SIZE];
#ifdef DEBUG_BUILD
  BOOL                found_mount = FALSE;
#endif
  char                sys_cmd[MAX_CMD_SIZE];
  FILE                *pf;
  int                 status;
  struct              stat st = {0};

  ILOG0_SD("Firmware dump evacuation application v.%s, pid = %d",
           MTLK_SOURCE_VERSION, (int)getpid());

  if (MTLK_ERR_OK != _mtlk_osdep_log_init(IWLWAV_RTLOG_APP_NAME_DUMPHANDLER)) {
	  ELOG_V("Firmware dump evacuation _mtlk_osdep_log_init ERROR\n");
     return 1;
   }

#ifdef CONFIG_WAVE_RTLOG_REMOTE
    if (MTLK_ERR_OK != mtlk_rtlog_app_init(&rtlog_info_data, IWLWAV_RTLOG_APP_NAME_DUMPHANDLER)) {
		ELOG_V("Firmware dump evacuation application mtlk_rtlog_app_init ERROR\n");
      goto end;
    }
#endif

  res = mtlk_argv_parser_init(&argv_parser, argc, argv);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't init argv parser (err=%d)", res);
    goto end;
  }
  argvparser_inited = 1;

  param = mtlk_argv_parser_param_get(&argv_parser, &param_path.info);
  if (param) {
    user_path = mtlk_argv_parser_param_get_str_val(param);
    mtlk_argv_parser_param_release(param);

    if (NULL == user_path) {
      ELOG_V("Invalid storage path");
      goto end;
    }
    if (stat(user_path, &st)){
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

  param = mtlk_argv_parser_param_get(&argv_parser, &param_card_idx.info);
  if (param) {
    card_idx = mtlk_argv_parser_param_get_uint_val(param, -1);
    mtlk_argv_parser_param_release(param);

    if (card_idx < 0 || card_idx > 4) {
      ELOG_V("Invalid card index");
      goto end;
    }
  }
  else {
    ELOG_V("Card index must be set");
    print_help = TRUE;
    goto end;
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_no_files_to_keep.info);
  if (param) {
    no_files_to_keep = mtlk_argv_parser_param_get_uint_val(param, 0);
    mtlk_argv_parser_param_release(param);

    if (no_files_to_keep <= 0 || no_files_to_keep > 100) {
      ELOG_V("Invalid number of dump files");
      print_help = TRUE;
      goto end;
    }
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_offline_dump.info);
  if (param) {
    dump_path = mtlk_argv_parser_param_get_str_val(param);
    mtlk_argv_parser_param_release(param);

    if (NULL == dump_path) {
      ELOG_V("Invalid dump path");
    }
    else{
      offline_dump = TRUE;
      mtlk_osal_strlcpy(dump_file_path, dump_path, MAX_FILE_NAME_SIZE);
    }
  }

#ifndef CONFIG_USE_DWPAL_DAEMON
  /* get interface name for the selected card */
  sprintf_res = sprintf_s(sys_cmd, sizeof(sys_cmd), "line=`/bin/cat /proc/net/mtlk/topology | "
                           "/bin/grep -Fn [hw%d |"
                           "/usr/bin/awk -F ':' '{ print $1 }'`;"
                           "[ \"X$line\" != \"X\" ] && /usr/bin/tail +$line /proc/net/mtlk/topology |"
                           "/bin/grep -om1 \"wlan[0-9]\"", card_idx);

  if (sprintf_res <= 0 || sprintf_res >= sizeof(sys_cmd)){
      ELOG_V("sprintf_s() error");
      res = MTLK_ERR_UNKNOWN;
      goto end;
  }

  pf =  popen(sys_cmd, "r");
  if (pf && (fgets(iface_name, IFACE_NAME_LEN , pf) != NULL)){
    status = pclose(pf);
    pf = NULL;
    if (WEXITSTATUS(status) != 0){
      ELOG_V("Interface name/index is invalid");
      mtlk_osal_strlcpy(iface_name, "wlan0", IFACE_NAME_LEN);
    }
  }
  else {
    if (pf){
      pclose(pf);
      pf = NULL;
    }
    ELOG_V("Failed to retrieve interface name, using default (wlan0)");
    mtlk_osal_strlcpy(iface_name, "wlan0", IFACE_NAME_LEN);
  }
#endif

  if (offline_dump){
    res = _fetch_dumps(dump_file_path, storage_path, card_idx, TRUE, no_files_to_keep);
    goto end;
  }

  /* in debug mode: override storage path in case DOK was inserted*/
#ifdef DEBUG_BUILD
  res = check_if_usb_mounted(USB_MOUNT_PATH, storage_path, &found_mount);
  if (res != MTLK_ERR_OK)
    goto end;

  if (found_mount){
    no_limit_dumps = TRUE;
  }
  else{
    res = check_if_usb_mounted(USB_MOUNT_PATH_PUMA, storage_path, &found_mount);
    if (res != MTLK_ERR_OK)
      goto end;

    if (found_mount)
      no_limit_dumps = TRUE;
  }
#endif

  sprintf_res = sprintf_s(fw_dump_filename, sizeof(fw_dump_filename), "%s%d%s",
                           FW_DUMP_FILE_PREFIX, card_idx, FW_DUMP_FILE_SUFFIX);

  if (sprintf_res <= 0 || sprintf_res >= sizeof(fw_dump_filename)){
    ELOG_V("sprintf_s() error");
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  if (_rcvry_happend(fw_dump_filename)){
    ILOG0_D("Firmware recovery detected, trying to retrieve dump files card %d",
            card_idx);
    _fetch_dumps(fw_dump_filename, storage_path, card_idx, no_limit_dumps, no_files_to_keep);
  }

  _listener(iface_name, fw_dump_filename, storage_path, card_idx, no_limit_dumps, no_files_to_keep);

end:
  if (print_help)
    _print_help(argv[0]);

  if (argvparser_inited)
    mtlk_argv_parser_cleanup(&argv_parser);

  ILOG0_V("Dump handler is exiting...");

  return res;
}
