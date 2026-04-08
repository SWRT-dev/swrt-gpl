/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifdef ATOMIC64_INIT

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_sub (mtlk_atomic64_t* val, uint64 i)
{
  return atomic64_sub_return(i, val);
}

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_dec (mtlk_atomic64_t* val)
{
  return atomic64_dec_return(val);
}

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_xchg (mtlk_atomic64_t* target, uint64 value)
{
  return atomic64_xchg(target, value);
}

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_add (mtlk_atomic64_t* val, uint64 i)
{
  return atomic64_add_return(i, val);
}

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_inc (mtlk_atomic64_t* val)
{
  return atomic64_inc_return(val);
}

__MTLK_ATOMIC64_API void
__mtlk_osal_atomic64_set (mtlk_atomic64_t* target, uint64 value)
{
  atomic64_set(target, value);
}

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_get (mtlk_atomic64_t* val)
{
  return atomic64_read(val);
}

#else /* !ATOMIC64_INIT */

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_sub (mtlk_atomic64_t* val, uint64 i)
{
  uint64 retval;
  mtlk_handle_t         lock_val;

  lock_val = mtlk_osal_lock_acquire_irq(&mtlk_osal_atomic64_lock);
  retval = val->counter -= i;
  mtlk_osal_lock_release_irq(&mtlk_osal_atomic64_lock, lock_val);

  return retval;
}

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_dec (mtlk_atomic64_t* val)
{
  return __mtlk_osal_atomic64_sub(val, 1LL);
}

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_xchg (mtlk_atomic64_t* target, uint64 value)
{
  uint64 retval;
  mtlk_handle_t         lock_val;

  lock_val = mtlk_osal_lock_acquire_irq(&mtlk_osal_atomic64_lock);
  retval = target->counter;
  target->counter = value;
  mtlk_osal_lock_release_irq(&mtlk_osal_atomic64_lock, lock_val);
  return retval;
}

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_add (mtlk_atomic64_t* val, uint64 i)
{
  uint64 retval;
  mtlk_handle_t         lock_val;

  lock_val = mtlk_osal_lock_acquire_irq(&mtlk_osal_atomic64_lock);
  retval = val->counter += i;
  mtlk_osal_lock_release_irq(&mtlk_osal_atomic64_lock, lock_val);
  return retval;
}

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_inc (mtlk_atomic64_t* val)
{
  return __mtlk_osal_atomic64_add(val, 1LL);
}

__MTLK_ATOMIC64_API void
__mtlk_osal_atomic64_set (mtlk_atomic64_t* target, uint64 value)
{
  mtlk_handle_t         lock_val;

  lock_val = mtlk_osal_lock_acquire_irq(&mtlk_osal_atomic64_lock);
  target->counter = value;
  mtlk_osal_lock_release_irq(&mtlk_osal_atomic64_lock, lock_val);
}

__MTLK_ATOMIC64_API uint64
__mtlk_osal_atomic64_get (mtlk_atomic64_t* val)
{
  uint64 retval;
  mtlk_handle_t         lock_val;

  lock_val = mtlk_osal_lock_acquire_irq(&mtlk_osal_atomic64_lock);
  retval = val->counter;
  mtlk_osal_lock_release_irq(&mtlk_osal_atomic64_lock, lock_val);
  return retval;

}

#endif /* ATOMIC64_INIT */
