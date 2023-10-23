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
#include "mtlk_vap_manager.h"
#include "mhi_mac_event.h"
#include "core_common.h"
#include "mtlkhal.h"
#include "txmm.h"
#include "hw_mmb.h"

#define LOG_LOCAL_GID   GID_COMMON_CORE
#define LOG_LOCAL_FID   0

#if (IWLWAV_RTLOG_MAX_DLEVEL >= IWLWAV_RTLOG_WARNING_DLEVEL)
static const char
mtlk_mac_event_prefix[] = "MAC event"; // don't change this - used from CLI
#endif

void __MTLK_IFUNC
mtlk_cc_handle_eeprom_failure(mtlk_vap_handle_t vap_handle, const EEPROM_FAILURE_EVENT *data)
{
  WLOG_DSD("CID-%04x: %s: EEPROM failure: Code %d", mtlk_vap_get_oid(vap_handle),
           mtlk_mac_event_prefix, data->u8ErrCode);
}

void __MTLK_IFUNC
mtlk_cc_handle_generic_event(mtlk_vap_handle_t vap_handle, const GENERIC_EVENT *data)
{
  ILOG0_DSD("CID-%04x: %s: Generic data: size %u", mtlk_vap_get_oid(vap_handle),
            mtlk_mac_event_prefix, MAC_TO_HOST32(data->u32dataLength));
  mtlk_dump(0, &data->u8data[0], GENERIC_DATA_SIZE, "Generic MAC data");
}

void __MTLK_IFUNC
mtlk_cc_handle_algo_calibration_failure(mtlk_vap_handle_t vap_handle, const CALIBR_ALGO_EVENT* calibration_event)
{
  ILOG1_DSDD("CID-%04x: %s: "
            "Offline calibration failed on channel %u, failure mask is 0x%08X",
            mtlk_vap_get_oid(vap_handle),
            mtlk_mac_event_prefix,
            MAC_TO_HOST32(calibration_event->u32calibrAlgoType),
            MAC_TO_HOST32(calibration_event->u32ErrCode));
}

void __MTLK_IFUNC
mtlk_cc_handle_dummy_event(mtlk_vap_handle_t vap_handle, const DUMMY_EVENT* data)
{
  ILOG0_DSDDDDDDDD("CID-%04x: %s: Dummy event : %u %u %u %u %u %u %u %u", mtlk_vap_get_oid(vap_handle),
                   mtlk_mac_event_prefix,
                   MAC_TO_HOST32(data->u32Dummy1),
                   MAC_TO_HOST32(data->u32Dummy2),
                   MAC_TO_HOST32(data->u32Dummy3),
                   MAC_TO_HOST32(data->u32Dummy4),
                   MAC_TO_HOST32(data->u32Dummy5),
                   MAC_TO_HOST32(data->u32Dummy6),
                   MAC_TO_HOST32(data->u32Dummy7),
                   MAC_TO_HOST32(data->u32Dummy8));
}

void __MTLK_IFUNC
mtlk_cc_handle_unknown_event(mtlk_vap_handle_t vap_handle, uint32 event_id)
{
  ILOG0_DSD("CID-%04x: %s: unknown MAC event id %u",
            mtlk_vap_get_oid(vap_handle),
            mtlk_mac_event_prefix, MAC_TO_HOST32(event_id));
}
