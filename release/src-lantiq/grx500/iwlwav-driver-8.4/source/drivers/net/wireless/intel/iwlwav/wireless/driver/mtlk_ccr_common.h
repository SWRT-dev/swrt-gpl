/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/* $Id$ */

#ifndef _MTLK_CCR_COMMON_H_
#define _MTLK_CCR_COMMON_H_

int __MTLK_IFUNC mtlk_ccr_dummy_ret_ok(void *ccr_mem);
void __MTLK_IFUNC mtlk_ccr_dummy_ret_void(void *ccr_mem);
uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_uzero(void *ccr_mem);
uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_uzero_by_ptr(void *ccr_mem, void *ptr);
int __MTLK_IFUNC mtlk_ccr_dummy_ret_ok_param(void *ccr_mem, uint32 param);
int __MTLK_IFUNC mtlk_ccr_dummy_ret_not_supported(void *ccr_mem, uint32 param1, uint32 param2);
void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_param(void *ccr_mem, uint32 param);
void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_by_ptr(void *ccr_mem, void *ptr);
void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_array(void *ccr_mem, uint32 *values);
uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_ffs(void *ccr_mem);
BOOL __MTLK_IFUNC mtlk_ccr_dummy_ret_true(void *ccr_mem);
void* __MTLK_IFUNC mtlk_ccr_dummy_ret_ptr_param(void *ccr_mem, uint32 param);
uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_efuse_rw(void *ccr_mem, void *param1, uint32 param2, uint32 param3);

#endif /*_MTLK_CCR_COMMON_H_*/
