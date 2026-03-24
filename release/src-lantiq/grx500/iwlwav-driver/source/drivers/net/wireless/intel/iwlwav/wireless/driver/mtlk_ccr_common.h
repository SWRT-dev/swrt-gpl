/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* $Id$ */

#ifndef _MTLK_CCR_COMMON_H_
#define _MTLK_CCR_COMMON_H_
void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_array(void *ccr_mem, uint32 *values);
void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_param(void *ccr_mem, uint32 param);
int  __MTLK_IFUNC mtlk_ccr_dummy_ret_ok_param(void *ccr_mem, uint32 param);
void __MTLK_IFUNC mtlk_ccr_dummy_ret_void(void *ccr_mem);
uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_uzero(void *ccr_mem);
#ifdef UNUSED_CODE
int __MTLK_IFUNC mtlk_ccr_dummy_ret_ok(void *ccr_mem);
uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_uzero_by_ptr(void *ccr_mem, void *ptr);
int __MTLK_IFUNC mtlk_ccr_dummy_ret_not_supported(void *ccr_mem, uint32 param1, uint32 param2);
void __MTLK_IFUNC mtlk_ccr_dummy_ret_void_by_ptr(void *ccr_mem, void *ptr);
uint32 __MTLK_IFUNC mtlk_ccr_dummy_ret_ffs(void *ccr_mem);
BOOL __MTLK_IFUNC mtlk_ccr_dummy_ret_true(void *ccr_mem);
void* __MTLK_IFUNC mtlk_ccr_dummy_ret_ptr_param(void *ccr_mem, uint32 param);
#endif /* UNUSED_CODE */
#endif /*_MTLK_CCR_COMMON_H_*/
