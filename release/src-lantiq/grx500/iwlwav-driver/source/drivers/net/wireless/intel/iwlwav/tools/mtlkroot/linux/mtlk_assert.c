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
#include "mtlk_assert.h"

#include <linux/module.h>

#define LOG_LOCAL_GID   GID_ROOT
#define LOG_LOCAL_FID   1

void __MTLK_IFUNC
__mtlk_assert (mtlk_slid_t slid)
{
  printk("Assertion failed " MTLK_SLID_FMT "\n",
         MTLK_SLID_ARGS(slid));
  BUG();
}
EXPORT_SYMBOL(__mtlk_assert);

void __MTLK_IFUNC
mtlk_assert_log_s (mtlk_slid_t caller_slid, const char *msg)
{
  ELOG_DDDDS("[SLID:%u:%u:%u:%u] %s",
    mtlk_slid_get_oid(caller_slid),
    mtlk_slid_get_gid(caller_slid),
    mtlk_slid_get_fid(caller_slid),
    mtlk_slid_get_lid(caller_slid), msg);
  MTLK_ASSERT(FALSE);
}
EXPORT_SYMBOL(mtlk_assert_log_s);

void __MTLK_IFUNC
mtlk_assert_log_ss (mtlk_slid_t caller_slid, const char *msg1, const char *msg2)
{
  ELOG_DDDDSS("[SLID:%u:%u:%u:%u] %s: %s",
    mtlk_slid_get_oid(caller_slid),
    mtlk_slid_get_gid(caller_slid),
    mtlk_slid_get_fid(caller_slid),
    mtlk_slid_get_lid(caller_slid), msg1, msg2);
  MTLK_ASSERT(FALSE);
}
EXPORT_SYMBOL(mtlk_assert_log_ss);
