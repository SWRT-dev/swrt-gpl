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

#define __MTLK_IFUNC 
#define __INLINE     inline
#define __LIKELY
#define __UNLIKELY

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/if.h>

#if defined YOCTO
#include <puma_safe_libc.h>
#else
#include "libsafec/safe_str_lib.h"
#include "libsafec/safe_mem_lib.h"
#endif

#define SDI(f,...) { fprintf(stderr, "\n!!! [%u] " f "[%s:%d]\n", getpid(), ##__VA_ARGS__, __FUNCTION__, __LINE__); fflush(stderr); }

#ifdef CPTCFG_IWLWAV_DEBUG
#define MTLK_DEBUG
#endif


#ifdef CONFIG_WAVE_DEBUG
#define CONFIG_WAVE_RTLOG_REMOTE
#endif


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

#include <arpa/inet.h> /* hton... */

# if __BYTE_ORDER == __BIG_ENDIAN

#include <byteswap.h>

/******************************************
 * DUT CLIENT <=> DUT SERVER interface conversion
 *****************************************/
#define DUT_TO_HOST16(x)   bswap_16(x)
#define DUT_TO_HOST32(x)   bswap_32(x)
#define DUT_TO_HOST64(x)   bswap_64(x)

#define HOST_TO_DUT16(x)   bswap_16(x)
#define HOST_TO_DUT32(x)   bswap_32(x)
#define HOST_TO_DUT64(x)   bswap_64(x)
/******************************************/

/******************************************
 * MAC <=> Driver interface conversion
 *****************************************/
#define MAC_TO_HOST16(x)   bswap_16(x)
#define MAC_TO_HOST32(x)   bswap_32(x)
#define MAC_TO_HOST64(x)   bswap_64(x)

#define HOST_TO_MAC16(x)   bswap_16(x)
#define HOST_TO_MAC32(x)   bswap_32(x)
#define HOST_TO_MAC64(x)   bswap_64(x)
/******************************************/

/******************************************
 * 802.11 conversion
 *****************************************/
#define WLAN_TO_HOST16(x)  bswap_16(x)
#define WLAN_TO_HOST32(x)  bswap_32(x)
#define WLAN_TO_HOST64(x)  bswap_64(x)

#define HOST_TO_WLAN16(x)  bswap_16(x)
#define HOST_TO_WLAN32(x)  bswap_32(x)
#define HOST_TO_WLAN64(x)  bswap_64(x)
/******************************************/
#else /*__BYTE_ORDER == __BIG_ENDIAN */

/******************************************
 * DUT CLIENT <=> DUT SERVER interface conversion
 *****************************************/
#define DUT_TO_HOST16(x)   (x)
#define DUT_TO_HOST32(x)   (x)
#define DUT_TO_HOST64(x)   (x)

#define HOST_TO_DUT16(x)   (x)
#define HOST_TO_DUT32(x)   (x)
#define HOST_TO_DUT64(x)   (x)
/******************************************/

/******************************************
 * MAC <=> Driver interface conversion
 *****************************************/
#define MAC_TO_HOST16(x)   (x)
#define MAC_TO_HOST32(x)   (x)
#define MAC_TO_HOST64(x)   (x)

#define HOST_TO_MAC16(x)   (x)
#define HOST_TO_MAC32(x)   (x)
#define HOST_TO_MAC64(x)   (x)
/******************************************/

/******************************************
 * 802.11 conversion
 *****************************************/
#define WLAN_TO_HOST16(x)  (x)
#define WLAN_TO_HOST32(x)  (x)
#define WLAN_TO_HOST64(x)  (x)

#define HOST_TO_WLAN16(x)  (x)
#define HOST_TO_WLAN32(x)  (x)
#define HOST_TO_WLAN64(x)  (x)
/******************************************/
#endif
/******************************************
 * Network conversion
 *****************************************/
#define NET_TO_HOST16(x)        ntohs(x)
#define NET_TO_HOST32(x)        ntohl(x)
#define NET_TO_HOST16_CONST(x)  (__constant_ntohs(x))
#define HOST_TO_NET16_CONST(x)  (__constant_htons(x))

#define HOST_TO_NET16(x)        htons(x)
#define HOST_TO_NET32(x)        htonl(x)
/******************************************/

#define MTLK_UNUSED_VAR(x) ((void)(x))
#define MTLK_UNREFERENCED_PARAM(x) MTLK_UNUSED_VAR(x)
#define MTLK_OFFSET_OF(type, field) \
  (unsigned long)(&((type*)0)->field)
#define MTLK_CONTAINER_OF(address, type, field) \
  (type *)((uint8 *)(address) - MTLK_OFFSET_OF(type, field))

#ifndef NULL
#define NULL 0
#endif

#include <stdio.h>
#include <string.h>

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#define MTLK_IPAD4(x)   (((4 - ((x) & 0x3)) & 0x3) + (x))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#define MTLK_IP4_ALEN (4)

#define MTLK_MAX_HW_ADAPTERS_SUPPORTED (4)

#if defined(CONFIG_PPA_PUMA7) || defined(CONFIG_PUMA_LITEPATH)
#define MTLK_PUMA_LITEPATH
#endif

#if defined(CONFIG_LTQ_PPA_API_DIRECTPATH) || defined(MTLK_PUMA_LITEPATH)
#define MTLK_USE_DIRECTPATH_API
#endif

#include "mtlk_slid.h"
#include "mtlk_assert.h"
#include "mtlk_osal.h"
#include "utils.h"
#include "log_osdep.h"
#include "mtlkstartup.h"
#include "mtlk_algorithms.h"

#endif /* !_MTLKINC_H_ */
