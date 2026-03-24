/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_HASH_IEEE_ADDR_H__
#define __MTLK_HASH_IEEE_ADDR_H__

#include "mtlkhash.h"
#include "mhi_ieee_address.h"

#define  MTLK_IDEFS_ON
#include "mtlkidefs.h"

/* Use MTLK_HASH_ENTRY_T(ieee_addr) for hash entry type */

MTLK_HASH_DECLARE_ENTRY_T(ieee_addr, IEEE_ADDR);

MTLK_HASH_DECLARE_EXTERN(ieee_addr, IEEE_ADDR);

static __INLINE void ieee_addr_copy (IEEE_ADDR *dest, const IEEE_ADDR *src) {
  *dest = *src;
}

static __INLINE void ieee_addr_zero (IEEE_ADDR *dest) {
  mtlk_osal_zero_eth_address(dest->au8Addr);
}

static __INLINE void ieee_addr_set (IEEE_ADDR *dest, const uint8 *src) {
  mtlk_osal_copy_eth_addresses(dest->au8Addr, src);
}

static __INLINE void ieee_addr_get (uint8 *dest, const IEEE_ADDR *src) {
  mtlk_osal_copy_eth_addresses(dest, src->au8Addr);
}

static __INLINE unsigned ieee_addr_compare (const IEEE_ADDR *addr1, const IEEE_ADDR *addr2) {
  return mtlk_osal_compare_eth_addresses(addr1->au8Addr, addr2->au8Addr);
}

static __INLINE BOOL ieee_addr_is_zero (const IEEE_ADDR *addr) {
  return mtlk_osal_is_zero_address(addr->au8Addr);
}

static __INLINE BOOL ieee_addr_is_valid (const IEEE_ADDR *addr) {
  return mtlk_osal_is_valid_ether_addr(addr->au8Addr);
}

#define  MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif
