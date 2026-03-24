/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * $Id$
 *
 * Driver framework firmware binaries access API.
 *
 */

#ifndef __DF_FW_H__
#define __DF_FW_H__

#include "mtlk_vap_manager.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

/**********************************************************************
 * DF FW utilities
 **********************************************************************/
typedef struct
{
  const uint8  *buffer;
  uint32        size;
  mtlk_handle_t context; /* for mtlk_hw_bus_t's usage. MMB does not use it. */
} __MTLK_IDATA mtlk_df_fw_file_buf_t;

/*
 * \brief   Load FW binaries from FS in memory.
 * */
int __MTLK_IFUNC
mtlk_df_fw_load_file (void *ctx, const char *name, mtlk_df_fw_file_buf_t *fb);

/*
 * \brief   Release FW related data in memory.
 * */
void __MTLK_IFUNC
mtlk_df_fw_unload_file(void *ctx, mtlk_df_fw_file_buf_t *fb);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __DF_FW_H__ */
