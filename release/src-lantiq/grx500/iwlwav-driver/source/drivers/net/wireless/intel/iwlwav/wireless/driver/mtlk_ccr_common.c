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

#define LOG_LOCAL_GID   GID_CCR_COMMON
#define LOG_LOCAL_FID   0
void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_array(void *ccr_mem, uint32 *values)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(values);
}

void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_param(void *ccr_mem, uint32 param)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(param);
}

int __MTLK_IFUNC mtlk_ccr_dummy_ret_ok_param(void *ccr_mem, uint32 param)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(param);
 return MTLK_ERR_OK;
}

void __MTLK_IFUNC mtlk_ccr_dummy_ret_void(void *ccr_mem)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
}

uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_uzero(void *ccr_mem)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  return 0;
}

#ifdef UNUSED_CODE
int __MTLK_IFUNC mtlk_ccr_dummy_ret_ok(void *ccr_mem)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  return MTLK_ERR_OK;
}

uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_uzero_by_ptr(void *ccr_mem, uint32 *ptr)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(ptr);
  return 0;
}

void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_by_ptr(void *ccr_mem, void *ptr)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(ptr);
}

int __MTLK_IFUNC mtlk_ccr_dummy_ret_not_supported(void *ccr_mem, uint32 param1, uint32 param2)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(param1);
  MTLK_UNREFERENCED_PARAM(param2);
  return MTLK_ERR_NOT_SUPPORTED;
}

uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_ffs(void *ccr_mem)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  return (uint32)(-1);
}

BOOL __MTLK_IFUNC mtlk_ccr_dummy_ret_true(void *ccr_mem)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  return TRUE;
}

void* __MTLK_IFUNC mtlk_ccr_dummy_ret_ptr_param(void *ccr_mem, uint32 param)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(param);
  return NULL;
}

#endif /* UNUSED_CODE */
