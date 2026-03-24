/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _MTLKINC_H_
#define _MTLKINC_H_

#include <linux/types.h>

/* TODO: WAVE300_SW-100: remove this after FW fix */
#define MBSS_FORCE_VAP_ACTIVATION_SUCCEEDED

#define EEPROM_DATA_ACCESS

#if defined(MBSS_FORCE_NO_AOCS_INITIAL_SELECTION) && !defined(MBSS_FORCE_NO_CHANNEL_SWITCH)
#error No channel switch is available while AOCS initial selection is OFF
#endif

/* AirPeek specific compiling elimination */
#define WILD_PACKETS           0

#define __MTLK_IFUNC
#define __INLINE     inline
#define __LIKELY     likely
#define __UNLIKELY   unlikely

#ifdef CPTCFG_IWLWAV_SILENT
#undef CPTCFG_IWLWAV_DEBUG
#endif

#ifdef CPTCFG_IWLWAV_DEBUG
#define MTLK_DEBUG
#endif

#ifdef CONFIG_WAVE_DEBUG
#define CONFIG_WAVE_RTLOG_REMOTE
#endif

#define MTLK_TEXT(x)           x

#define ARGUMENT_PRESENT
#define MTLK_UNUSED_VAR(x) ((void)(x))
#define MTLK_UNREFERENCED_PARAM(x) MTLK_UNUSED_VAR(x)

#define MTLK_OFFSET_OF(type, field)             offsetof(type, field)
#define MTLK_CONTAINER_OF(address, type, field) container_of(address, type, field)
#define MTLK_FIELD_SIZEOF(type, field)          sizeof(((type *)0)->field)

#define SIZEOF(a) MTLK_ARRAY_SIZE(a)
#define INC_WRAP_IDX(i,s) do { (i)++; if ((i) == (s)) (i) = 0; } while (0)

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define MTLK_PRAGMA(x)  _Pragma(#x)

typedef unsigned char BOOL;
#define FALSE (0)
#define TRUE  (1)

/* Generic integer types */
typedef ssize_t   wave_int;
typedef size_t    wave_uint;
typedef uintptr_t wave_addr;

#ifndef HANDLE_T_DEFINED
typedef uintptr_t mtlk_handle_t;
#define HANDLE_T_DEFINED

#define MTLK_INVALID_HANDLE HANDLE_T(0)

#define HANDLE_T(x)       ((mtlk_handle_t)(x))
#define HANDLE_T_PTR(t,x) ((t*)(x))
#define HANDLE_T_INT(t,x) ((t)(x))
#endif

#define MTLK_IPV4(a1,a2,a3,a4) \
   (((u32)((u8)a1) << 24u) |   \
    ((u32)((u8)a2) << 16u) |   \
    ((u32)((u8)a3) << 8u)  |   \
    ((u32)((u8)a4)))

/******************************************
 * MAC <=> Driver interface conversion
 *****************************************/
#define MAC_TO_HOST16(x)   le16_to_cpu(x)
#define MAC_TO_HOST32(x)   le32_to_cpu(x)
#define MAC_TO_HOST64(x)   le64_to_cpu(x)

#define HOST_TO_MAC16(x)   cpu_to_le16(x)
#define HOST_TO_MAC32(x)   cpu_to_le32(x)
#define HOST_TO_MAC64(x)   cpu_to_le64(x)
/******************************************/

/******************************************
 * DUT CLIENT <=> Driver interface conversion
 *****************************************/
#define DUT_TO_HOST16(x)   le16_to_cpu(x)
#define DUT_TO_HOST32(x)   le32_to_cpu(x)
#define DUT_TO_HOST64(x)   le64_to_cpu(x)

#define HOST_TO_DUT16(x)   cpu_to_le16(x)
#define HOST_TO_DUT32(x)   cpu_to_le32(x)
#define HOST_TO_DUT64(x)   cpu_to_le64(x)
/******************************************/

/******************************************
 * 802.11 conversion
 *****************************************/
#define WLAN_TO_HOST16(x)  le16_to_cpu(x)
#define WLAN_TO_HOST32(x)  le32_to_cpu(x)
#define WLAN_TO_HOST64(x)  le64_to_cpu(x)

#define HOST_TO_WLAN16(x)  cpu_to_le16(x)
#define HOST_TO_WLAN32(x)  cpu_to_le32(x)
#define HOST_TO_WLAN64(x)  cpu_to_le64(x)
/******************************************/

/******************************************
 * Network conversion
 *****************************************/
#define NET_TO_HOST16(x)        ntohs(x)
#define NET_TO_HOST32(x)        ntohl(x)
#define NET_TO_HOST16_CONST(x)  (__constant_ntohs(x))
#define HOST_TO_NET16_CONST(x)  (__constant_htons(x))

#define HOST_TO_NET16(x)   htons(x)
#define HOST_TO_NET32(x)   htonl(x)
/******************************************/


#define CPU_TO_BE32(x)          cpu_to_be32(x)
#define CPU_TO_LE32(x)          cpu_to_le32(x)
#define LE32_TO_CPU(x)          le32_to_cpu(x)
#define BE32_TO_CPU(x)          be32_to_cpu(x)


typedef s8  int8;
typedef s16 int16;
typedef s32 int32;
typedef s64 int64;

typedef u8  uint8;
typedef u16 uint16;
typedef u32 uint32;
typedef u64 uint64;

typedef uint16      K_MSG_TYPE;

/*
#define MAX_UINT8  ((uint8)(-1))
#define MIN_INT8   ((int8)0x80)
#define MAX_INT8   ((int8)0x7F)
#define MAX_UINT32 ((uint32)(-1))
*/

#define MTLK_IPAD4(x)   (((4 - ((x) & 0x3)) & 0x3) + (x))

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#define MTLK_MAX_HW_ADAPTERS_SUPPORTED (4)

/* Debug code for tracking wrong BD indexes (returned from FW or erroneously generated)
   also re-use of BD buffers already in use/ free of already freed buffers */
#define BD_DBG

#ifdef CONFIG_X86_PUMA7
#define MTLK_PUMA_PLATFORM
#endif

#ifdef CONFIG_PRX300_CQM
#define MTLK_FLM_PLATFORM
#endif

#ifdef CONFIG_X86_INTEL_LGM
#define MTLK_LGM_PLATFORM
/* NOTE: MTLK_LGM_PLATFORM_FPGA must be defined if LGM x86 host is running on FPGA */
/* #define MTLK_LGM_PLATFORM_FPGA */
#endif

/* Define conditional defines, depended from external flags */
#if IS_ENABLED(CONFIG_DIRECTCONNECT_DP_API) && defined(CPTCFG_IWLWAV_DC_DP_LIB_SUPPORT)
#define MTLK_USE_DIRECTCONNECT_DP_API 1
#else
#define MTLK_USE_DIRECTCONNECT_DP_API 0
#endif

#if defined(CONFIG_UDMA) && defined(CPTCFG_IWLWAV_PUMA6_UDMA_SUPPORT)
#define MTLK_USE_PUMA6_UDMA 1
#else
#define MTLK_USE_PUMA6_UDMA 0
#endif

#if defined(CONFIG_PPA_PUMA7) || defined(CONFIG_PUMA_LITEPATH)
#define MTLK_PUMA_LITEPATH
#endif

#define __MTLK_INT_HANDLER_SECTION      __attribute__((section (".int_handler")))

/* Fall through in switch statement */
#if defined(__GNUC__) && __GNUC__ >= 7
 #define FALLTHROUGH __attribute__ ((fallthrough))
#else
 #define FALLTHROUGH ((void)0)
#endif /* __GNUC__ >= 7 */

#include "formats.h"
#include "mtlk_slid.h"
#include "mtlk_assert.h"
#include "mtlk_osal.h"
#include "log_osdep.h"
#include "mtlkstartup.h"
#include "cpu_stat.h"
#include "utils.h"
#include "mtlk_algorithms.h"

#define MTLK_NDEV_NAME  "wlan"

#endif /* !_MTLKINC_H_ */
