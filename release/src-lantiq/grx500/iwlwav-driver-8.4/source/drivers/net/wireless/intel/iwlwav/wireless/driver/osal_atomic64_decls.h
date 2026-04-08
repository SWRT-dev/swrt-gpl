/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_sub (mtlk_atomic64_t* val, uint64 i);

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_dec (mtlk_atomic64_t* val);

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_xchg (mtlk_atomic64_t* target, uint64 value);

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_add (mtlk_atomic64_t* val, uint64 i);

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_inc (mtlk_atomic64_t* val);

__MTLK_ATOMIC64_API void
__mtlk_osal_atomic64_set (mtlk_atomic64_t* target, uint64 value);

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_get (mtlk_atomic64_t* val);

