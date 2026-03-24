/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __DRVMGR_H__
#define __DRVMGR_H__

#include "compat.h"
#include "dataex.h"
#include "mtlk_pathutils.h"

/* Application return codes */
#define EVENT_DO_NOTHING  0
#define EVENT_REQ_RESET   1
#define EVENT_REQ_RMMOD   2
#define EVENT_CLI_EXIT    3
#define EVENT_INT_ERR     4
#define EVENT_SIGTERM     5
#define EVENT_REQ_RESTART 6

/* Application bitmask fields */
#define DHFLAG_NO_DRV_HUNG_HANDLING  0x00000001
#define DHFLAG_NO_DRV_RMMOD_HANDLING 0x00000002
#define DHFLAG_NO_WRITE_LED          0x00000004
#define DHFLAG_RECOVERY_HANDLING     0x00000008

#define MTLK_DELAY_READ_SECURITY_CONF 5
#define MTLK_RECOVERY_MAX_CMD_LEN (MAX_PATH + 100)

/* band */
typedef enum mtlk_hw_band {
  MTLK_HW_BAND_5_2_GHZ,
  MTLK_HW_BAND_2_4_GHZ,
  MTLK_HW_BAND_BOTH,
  MTLK_HW_BAND_NONE
} mtlk_hw_band_e;

int drvhlpr_set_script_path (char** dest, const char* str);

#endif // !__DRVMGR_H__

