/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_SLID_H__
#define __MTLK_SLID_H__

#include "loggroups.h"
#include "mtlkbfield.h"

#define MTLK_SLID_OID      MTLK_BFIELD_INFO(0, 8)
#define MTLK_SLID_GID      MTLK_BFIELD_INFO(8, 8)
#define MTLK_SLID_FID      MTLK_BFIELD_INFO(16, 8)
#define MTLK_SLID_LID      MTLK_BFIELD_INFO(24, 16)
#define MTLK_SLID_CID      MTLK_BFIELD_INFO(40, 8)

#define MTLK_SLID_MAX_GID  MTLK_BFIELD_VALUE(MTLK_SLID_GID, -1, uint64)
#define MTLK_SLID_MAX_FID  MTLK_BFIELD_VALUE(MTLK_SLID_FID, -1, uint64)
#define MTLK_SLID_MAX_LID  MTLK_BFIELD_VALUE(MTLK_SLID_LID, -1, uint64)
#define MTLK_SLID_MAX_CID  MTLK_BFIELD_VALUE(MTLK_SLID_CID, -1, uint64)

typedef uint64 mtlk_slid_t;

#define MTLK_SLID   (MTLK_BFIELD_VALUE(MTLK_SLID_OID, LOG_LOCAL_OID, mtlk_slid_t) | \
                     MTLK_BFIELD_VALUE(MTLK_SLID_GID, LOG_LOCAL_GID, mtlk_slid_t) | \
                     MTLK_BFIELD_VALUE(MTLK_SLID_FID, LOG_LOCAL_FID, mtlk_slid_t) | \
                     MTLK_BFIELD_VALUE(MTLK_SLID_LID, __LINE__, mtlk_slid_t))

#define MTLK_SLID_CODE(code) \
                    (MTLK_SLID | (MTLK_BFIELD_VALUE(MTLK_SLID_CID, (code), mtlk_slid_t)))

static uint32 __INLINE
mtlk_slid_get_oid (mtlk_slid_t slid)
{
  return (uint32)MTLK_BFIELD_GET(slid, MTLK_SLID_OID);
}

static uint32 __INLINE
mtlk_slid_get_gid (mtlk_slid_t slid)
{
  return (uint32)MTLK_BFIELD_GET(slid, MTLK_SLID_GID);
}

static uint32 __INLINE
mtlk_slid_get_fid (mtlk_slid_t slid)
{
  return (uint32)MTLK_BFIELD_GET(slid, MTLK_SLID_FID);
}

static uint32 __INLINE
mtlk_slid_get_lid (mtlk_slid_t slid)
{
  return (uint32)MTLK_BFIELD_GET(slid, MTLK_SLID_LID);
}

static uint32 __INLINE
mtlk_slid_get_code (mtlk_slid_t slid)
{
  return (uint32)MTLK_BFIELD_GET(slid, MTLK_SLID_CID);
}

#define MTLK_SLID_FMT        \
  "[SLID:%u:%u:%u:%u]"

#define MTLK_SLID_ARGS(slid)  \
  mtlk_slid_get_oid(slid),    \
  mtlk_slid_get_gid(slid),    \
  mtlk_slid_get_fid(slid),    \
  mtlk_slid_get_lid(slid)

#endif /* __MTLK_SLID_H__ */
