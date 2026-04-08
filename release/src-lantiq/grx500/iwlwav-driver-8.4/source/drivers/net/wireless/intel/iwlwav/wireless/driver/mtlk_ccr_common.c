/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include "mtlkinc.h"

#define LOG_LOCAL_GID   GID_CCR_COMMON
#define LOG_LOCAL_FID   0

int __MTLK_IFUNC mtlk_ccr_dummy_ret_ok(void *ccr_mem)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  return MTLK_ERR_OK;
}

uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_uzero(void *ccr_mem)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  return 0;
}

uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_uzero_by_ptr(void *ccr_mem, uint32 *ptr)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(ptr);
  return 0;
}

void __MTLK_IFUNC mtlk_ccr_dummy_ret_void(void *ccr_mem)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
}

void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_param(void *ccr_mem, uint32 param)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(param);
}

void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_by_ptr(void *ccr_mem, void *ptr)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(ptr);
}

void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_array(void *ccr_mem, uint32 *values)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(values);
}

int __MTLK_IFUNC mtlk_ccr_dummy_ret_ok_param(void *ccr_mem, uint32 param)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(param);
 return MTLK_ERR_OK;
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

uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_efuse_rw(void *ccr_mem, void *param1, uint32 param2, uint32 param3)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(param1);
  MTLK_UNREFERENCED_PARAM(param2);
  MTLK_UNREFERENCED_PARAM(param3);
  return 0;
}
