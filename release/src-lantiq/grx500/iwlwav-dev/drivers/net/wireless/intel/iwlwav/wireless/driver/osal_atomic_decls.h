/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

__MTLK_ATOMIC_API uint32
__mtlk_osal_atomic_sub (mtlk_atomic_t* val, uint32 i);

__MTLK_ATOMIC_API uint32
__mtlk_osal_atomic_dec (mtlk_atomic_t* val);

__MTLK_ATOMIC_API uint32
__mtlk_osal_atomic_xchg (mtlk_atomic_t* target, uint32 value);

__MTLK_ATOMIC_API uint32
__mtlk_osal_atomic_add (mtlk_atomic_t* val, uint32 i);

__MTLK_ATOMIC_API uint32
__mtlk_osal_atomic_inc (mtlk_atomic_t* val);

__MTLK_ATOMIC_API void
__mtlk_osal_atomic_set (mtlk_atomic_t* target, uint32 value);

__MTLK_ATOMIC_API uint32
__mtlk_osal_atomic_get (const mtlk_atomic_t* val);

