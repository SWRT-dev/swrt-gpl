/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"

#include <linux/device.h>
#include <linux/firmware.h>

#include "mtlk_df_fw.h"
#include "mtlkhal.h"

#define LOG_LOCAL_GID   GID_DFFW
#define LOG_LOCAL_FID   1

void __MTLK_IFUNC
mtlk_df_fw_unload_file(void *ctx, mtlk_df_fw_file_buf_t *fb)
{
  MTLK_ASSERT(NULL != ctx);
  MTLK_UNREFERENCED_PARAM(ctx);

  release_firmware((const struct firmware *)fb->context);
}

static const struct firmware *
_mtlk_df_fw_request_firmware(void *ctx, const char *fname)
{
  const struct firmware *fw_entry = NULL;
  int result = 0;

  ILOG0_S("Request Firmware ('%s') is starting", fname);
  result = request_firmware(&fw_entry, fname, ctx);

  if (result)
  {
    WLOG_S("Failed to request Firmware ('%s')", fname);

    if (fw_entry) {
      release_firmware(fw_entry);
      fw_entry = NULL;
    }
  }
  else
  {
    ILOG3_SDP("%s firmware: size=0x%x, data=0x%p",
              fname, (unsigned int)fw_entry->size, fw_entry->data);
  }

  return fw_entry;
}

int __MTLK_IFUNC
mtlk_df_fw_load_file (void *ctx, const char *name, mtlk_df_fw_file_buf_t *fb)
{
  int                    res      = MTLK_ERR_UNKNOWN;
  const struct firmware *fw_entry = NULL;

  MTLK_ASSERT(NULL != ctx);
  MTLK_ASSERT(NULL != name);
  MTLK_ASSERT(NULL != fb);

  fw_entry = _mtlk_df_fw_request_firmware(ctx, name);

  if (fw_entry) {
    fb->buffer  = fw_entry->data;
    fb->size    = fw_entry->size;
    fb->context = HANDLE_T(fw_entry);
    res         = MTLK_ERR_OK;
  }

  return res;
}

