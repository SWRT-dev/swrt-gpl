/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_GUID_H__
#define __MTLK_GUID_H__

#define  MTLK_IDEFS_ON
#define  MTLK_IDEFS_PACKING 1
#include "mtlkidefs.h"

typedef struct _mtlk_guid_t
{
     uint32 data1;
     uint16 data2;
     uint16 data3;
     uint8  data4[8];
} __MTLK_IDATA mtlk_guid_t;

#define MTLK_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
  const mtlk_guid_t name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#define MTLK_DECLARE_GUID(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
  { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#define MTLK_GUID_STR_SIZE sizeof("{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}")

static __INLINE int
mtlk_guid_compare (const mtlk_guid_t* guid1, const mtlk_guid_t* guid2)
{
  return memcmp(guid1, guid2, sizeof(mtlk_guid_t));
}

static __INLINE void
mtlk_guid_copy (mtlk_guid_t* dst_guid, const mtlk_guid_t* src_guid)
{
  *dst_guid = *src_guid;
}

#define  MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_GUID_H__ */
