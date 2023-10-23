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
 * 
 * 
 * This file has common data for the driver and tools utilities
 * to implement and support data exchange
 *
 */

#ifndef __DATAEX_H_
#define __DATAEX_H_

#include <linux/sockios.h>
#include <linux/if.h>
#include <linux/wireless.h>

#include "mtlkguid.h"
#include "mhi_ieee_address.h"

#define WE_GEN_DATAEX_PROTO_VER           3 // Protocol version

#define WE_GEN_DATAEX_SUCCESS             0 // Command succeeded
#define WE_GEN_DATAEX_FAIL               -1 // Command failed
#define WE_GEN_DATAEX_PROTO_MISMATCH     -2 // Protocol version mismatch
#define WE_GEN_DATAEX_UNKNOWN_CMD        -3 // Unknown command
#define WE_GEN_DATAEX_DATABUF_TOO_SMALL  -4 // Results do not fit into the data buffer

#define WE_GEN_DATAEX_CMD_CONNECTION_STATS  1
#define WE_GEN_DATAEX_CMD_STATUS            2

typedef struct _WE_GEN_DATAEX_REQUEST {
  uint32 ver;
  uint32 cmd_id;
  uint32 datalen;
} __attribute__((aligned(1), packed)) WE_GEN_DATAEX_REQUEST;

typedef struct _WE_GEN_DATAEX_RESPONSE {
  uint32 ver;
  int32 status;
  uint32 datalen;
} __attribute__((aligned(1), packed)) WE_GEN_DATAEX_RESPONSE;

typedef struct _WE_GEN_DATAEX_DEVICE_STATUS {
  uint32    u32RxCount;
  uint32    u32TxCount;
} __attribute__((aligned(1), packed)) WE_GEN_DATAEX_DEVICE_STATUS;

typedef struct _WE_GEN_DATAEX_CONNECTION_STATUS {
  uint32 u32NumOfConnections;
  WE_GEN_DATAEX_DEVICE_STATUS sDeviceStatus[0];
} __attribute__((aligned(1), packed)) WE_GEN_DATAEX_CONNECTION_STATUS;

typedef struct _WE_GEN_DATAEX_STATUS {
  uint8 security_on;
  uint8 scan_started;
  uint8 frequency_band;
  uint8 link_up;
  uint8 wep_enabled;
} __attribute__((aligned(1), packed)) WE_GEN_DATAEX_STATUS;

#define MTLK_IOCTL_FIRST       SIOCDEVPRIVATE
#define MTLK_IOCTL_LAST        (SIOCDEVPRIVATE + 15)

enum _MTLK_DEV_PRIVATE_IOCTLS {
  MTLK_IOCTL_DATAEX = MTLK_IOCTL_FIRST,
  MTLK_IOCTL_IRBM
};

#define MTLK_WE_IOCTL_GEN_GPIO (SIOCIWFIRSTPRIV + 30)

#define MTLK_WE_IOCTL_FIRST    SIOCIWFIRSTPRIV
#define MTLK_WE_IOCTL_LAST     SIOCIWLASTPRIV

typedef struct _IRBM_DRIVER_CALL_HDR {
  mtlk_guid_t    event;
  uint32         data_length;
} __attribute__((aligned(1), packed)) IRBM_DRIVER_CALL_HDR;

typedef struct _IRBM_APP_NOTIFY_HDR {
  mtlk_guid_t    event;
  uint32         sequence_number;
  uint32         data_length;
} __attribute__((aligned(1), packed)) IRBM_APP_NOTIFY_HDR;

// {45EDC219-D0A4-4105-94F6-AE9F3AAE8D9F}
#define MTLK_IRB_GUID_ARP_REQ                                           \
  MTLK_DECLARE_GUID(0x45edc219, 0xd0a4, 0x4105, 0x94, 0xf6, 0xae, 0x9f, 0x3a, 0xae, 0x8d, 0x9f)
struct mtlk_arp_data {
  char   ifname[IFNAMSIZ];
  /* IP addresses are in network byte-order */
  uint32 daddr;
  uint32 saddr;
  uint8  smac[ETH_ALEN];
} __attribute__ ((aligned(1), packed));

// {90A27675-19B8-463b-A6AD-775D6925AF6B}
#define MTLK_IRB_GUID_HANG                                              \
  MTLK_DECLARE_GUID(0x90a27675, 0x19b8, 0x463b, 0xa6, 0xad, 0x77, 0x5d, 0x69, 0x25, 0xaf, 0x6b)

// {F5C4029B-B5D0-4c23-99A6-7937D3B9A4A6}
#define MTLK_IRB_GUID_RMMOD                                             \
  MTLK_DECLARE_GUID(0xf5c4029b, 0xb5d0, 0x4c23, 0x99, 0xa6, 0x79, 0x37, 0xd3, 0xb9, 0xa4, 0xa6)

// {F4D86A07-C926-473A-A766-C7E62CCC7ED5}
#define MTLK_IRB_GUID_DUT_FW_CMD                                               \
  MTLK_DECLARE_GUID(0xf4d86a07, 0xc926, 0x473a, 0xa7, 0x66, 0xc7, 0xe6, 0x2c, 0xcc, 0x7e, 0xd5)

/* {4016b6c7-f07c-423c-bb70-7397de50a728} */
#define MTLK_IRB_GUID_DUT_DRV_CMD \
    MTLK_DECLARE_GUID(0x4016b6c7, 0xf07c, 0x423c, 0xbb, 0x70, 0x73, 0x97, 0xde, 0x50, 0xa7, 0x28);

// {7AB742C1-6A15-4CE4-A9E1-2806E4F5AF63}
#define MTLK_IRB_GUID_DUT_PROGMODEL_CMD                                     \
  MTLK_DECLARE_GUID(0x7ab742c1, 0x6a15, 0x4ce4, 0xa9, 0xe1, 0x28, 0x06, 0xe4, 0xf5, 0xaf, 0x63)

/* {b6d9870a-7f1b-11e2-a4dd-2fce4c3f4044} */
#define MTLK_IRB_GUID_RECOVERY_START_DUMP                                          \
  MTLK_DECLARE_GUID(0xb6d9870a, 0x7f1b, 0x11e2, 0xa4, 0xdd, 0x2f, 0xce, 0x4c, 0x3f, 0x40, 0x44)

/* {b7781742-9142-11e2-8e5d-4b632b639c93} */
#define MTLK_IRB_GUID_RECOVERY_RCVRY_FINISH                                        \
  MTLK_DECLARE_GUID(0xb7781742, 0x9142, 0x11e2, 0x8e, 0x5d, 0x4b, 0x63, 0x2b, 0x63, 0x9c, 0x93)

/* {3642ce30-7f2a-11e2-9b72-d3366bed65e4} */
#define MTLK_IRB_GUID_RECOVERY_START_DUMP_CFM                                      \
  MTLK_DECLARE_GUID(0x3642ce30, 0x7f2a, 0x11e2, 0x9b, 0x72, 0xd3, 0x36, 0x6b, 0xed, 0x65, 0xe4)

/* {4519c272-9138-11e2-89ed-a7896b2f99fe} */
#define MTLK_IRB_GUID_RECOVERY_STOP_DUMP_CFM                                      \
  MTLK_DECLARE_GUID(0x4519c272, 0x9138, 0x11e2, 0x89, 0xed, 0xa7, 0x89, 0x6b, 0x2f, 0x99, 0xfe)

/* {b6d9870b-7f30-0000-a4dd-2fce4c3f4044} */
#define MTLK_IRB_GUID_RECOVERY_RESTART_HOSTAPD \
  MTLK_DECLARE_GUID(0xb6d9870b, 0x7f30, 0x0000, 0xc4, 0xdf, 0x3f, 0xae, 0x4c, 0x3f, 0x40, 0x44)

/* {b6d9870b-7f30-0001-a4dd-2fce4c3f4044} */
#define MTLK_IRB_GUID_RECOVERY_RESTART_HOSTAPD_CFM \
  MTLK_DECLARE_GUID(0xb6d9870b, 0x7f30, 0x0001, 0xc4, 0xdf, 0x3f, 0xae, 0x4c, 0x3f, 0x40, 0x44)

/* FIXME: this is temporary solution */
/* the structure with status should be received from the DUT GUI */
typedef struct _dut_progmodel_t {
  int status;
  uint32 len;
  char name[0]; /* this field should be the last in the structure */
} dut_progmodel_t;

/* {3055fcee-03c0-11e2-8918-5f26df6fee1b} */
#define MTLK_IRB_GUID_DUT_START_CMD \
  MTLK_DECLARE_GUID(0x3055fcee, 0x03c0, 0x11e2, 0x89, 0x18, 0x5f, 0x26, 0xdf, 0x6f, 0xee, 0x1b)

/* {ff8fc4a4-03c0-11e2-872d-3fd85cb8afc2} */
#define MTLK_IRB_GUID_DUT_STOP_CMD \
  MTLK_DECLARE_GUID(0xff8fc4a4, 0x03c0, 0x11e2, 0x87, 0x2d, 0x3f, 0xd8, 0x5c, 0xb8, 0xaf, 0xc2)

/* {0c1f626d-e091-4057-8c92-04c9cb070255} */
#define MTLK_IRB_GUID_FILE_SAVE \
    MTLK_DECLARE_GUID(0x0c1f626d, 0xe091, 0x4057, 0x8c, 0x92, 0x04, 0xc9, 0xcb, 0x07, 0x02, 0x55)

struct mtlk_file_save
{
  uint8   fname[0x40];
  uint32  size;
} __attribute__ ((aligned(1), packed));

/* {f89afd8c-51bf-4764-ad99-9924e9a6e528} */
#define MTLK_IRB_GUID_DUT_ADD_VAP_CMD \
    MTLK_DECLARE_GUID(0xf89afd8c, 0x51bf, 0x4764, 0xad, 0x99, 0x99, 0x24, 0xe9, 0xa6, 0xe5, 0x28)

/* {f89afddd-51bf-4764-ad99-9924e9a6e528} */
#define MTLK_IRB_GUID_DUT_REMOVE_VAP_CMD \
    MTLK_DECLARE_GUID(0xf89afddd, 0x51bf, 0x4764, 0xad, 0x99, 0x99, 0x24, 0xe9, 0xa6, 0xe5, 0x28)

/* {f89afd01-51bf-4764-ad99-9924e9a6e528} */
#define MTLK_IRB_GUID_DUT_SET_BSS_CMD \
    MTLK_DECLARE_GUID(0xf89afd01, 0x51bf, 0x4764, 0xad, 0x99, 0x99, 0x24, 0xe9, 0xa6, 0xe5, 0x28)

/* {f89afd02-51bf-4764-ad99-9924e9a6e528} */
#define MTLK_IRB_GUID_DUT_SET_WMM_PARAMETERS_CMD \
    MTLK_DECLARE_GUID(0xf89afd02, 0x51bf, 0x4764, 0xad, 0x99, 0x99, 0x24, 0xe9, 0xa6, 0xe5, 0x28)

/* {f89afd03-51bf-4764-ad99-9924e9a6e528} */
#define MTLK_IRB_GUID_DUT_STOP_VAP_TRAFFIC_CMD \
    MTLK_DECLARE_GUID(0xf89afd03, 0x51bf, 0x4764, 0xad, 0x99, 0x99, 0x24, 0xe9, 0xa6, 0xe5, 0x28)

/* {f89afd04-51bf-4764-ad99-9924e9a6e528} */
#define MTLK_IRB_GUID_DUT_DRIVER_FW_GENERAL_CMD \
    MTLK_DECLARE_GUID(0xf89afd04, 0x51bf, 0x4764, 0xad, 0x99, 0x99, 0x24, 0xe9, 0xa6, 0xe5, 0x28)

/* {f89afd05-51bf-4764-ad99-9924e9a6e528} */
#define MTLK_IRB_GUID_DUT_PLATFORM_DATA_FIELDS_CMD \
    MTLK_DECLARE_GUID(0xf89afd05, 0x51bf, 0x4764, 0xad, 0x99, 0x99, 0x24, 0xe9, 0xa6, 0xe5, 0x28)

/* Helper macros for data size checking */

#define WAVE_CHECK_SIZE_EXP_VOID(__size, __exp_size) { \
  if (!wave_check_size_exp(__size, __exp_size, __FUNCTION__, __LINE__)) { \
      return; \
  } \
}

#define WAVE_CHECK_SIZE_EXP_ERR(__data, __data_size) { \
  if (!wave_check_size_exp(__size, __exp_size, __FUNCTION__, __LINE__)) { \
      return MTLK_ERR_PARAMS; \
  } \
}

#define WAVE_CHECK_SIZE_MIN_VOID(__size, __exp_size) { \
  if (!wave_check_size_min(__size, __exp_size, __FUNCTION__, __LINE__)) { \
    __size = 0; \
    return; \
  } \
}

#define WAVE_CHECK_SIZE_MAX_VOID(__size, __exp_size) { \
  if (!wave_check_size_max(__size, __exp_size, __FUNCTION__, __LINE__)) { \
    __size = 0; \
    return; \
  } \
}

BOOL __MTLK_IFUNC
wave_check_size_exp (size_t data_size, size_t expected_size, const char *fn_name, int n_line);

BOOL __MTLK_IFUNC
wave_check_size_min (size_t data_size, size_t min_size, const char *fn_name, int n_line);

BOOL __MTLK_IFUNC
wave_check_size_max (size_t data_size, size_t max_size, const char *fn_name, int n_line);

#endif // !__DATAEX_H_

