/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_GUID_H__
#define __MTLK_GUID_H__

#define  MTLK_IDEFS_ON
#define  MTLK_IDEFS_PACKING 1
#include "mtlkidefs.h"
#include "mtlk_snprintf.h"

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
mtlk_guid_to_string (const mtlk_guid_t* guid, char *buffer, uint32 size)
{
  return mtlk_snprintf(buffer,
                      size,
                      "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
                      guid->data1,
                      guid->data2,
                      guid->data3,
                      guid->data4[0],
                      guid->data4[1],
                      guid->data4[2],
                      guid->data4[3],
                      guid->data4[4],
                      guid->data4[5],
                      guid->data4[6],
                      guid->data4[7]);
}

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
