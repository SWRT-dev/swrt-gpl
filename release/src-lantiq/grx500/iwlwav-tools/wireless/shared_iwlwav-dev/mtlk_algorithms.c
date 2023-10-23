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
#include "mtlk_algorithms.h"

#define LOG_LOCAL_GID   GID_ALGO
#define LOG_LOCAL_FID   0

void __MTLK_IFUNC
mtlk_sort_slist(void** list, mtlk_get_next_t func_get_next,
                mtlk_set_next_t func_set_next, mtlk_is_less_t func_is_less)
{
  /* This function implements generic insertion sort algorithm */
  /* for single-linked list abstraction.                       */

  void  *front_iterator, *prev_front_iterator, *tmp = NULL;

  if (!*list) return;

  prev_front_iterator = *list;
  front_iterator = func_get_next(prev_front_iterator);

  while(front_iterator) {
    void *rear_iterator = *list,
         *prev_rear_iterator = NULL;

    while(rear_iterator != front_iterator) {

      if( func_is_less(front_iterator, rear_iterator) ) {
        /* Put item pointed by front iterator before item */
        /* pointed by rear iterator                       */

        /* 1. Remove front_iterator from list             */
        func_set_next(prev_front_iterator,
                      func_get_next(front_iterator));
        /* 2. Put it to the new location                  */
        if(prev_rear_iterator) {
          tmp = func_get_next(prev_rear_iterator);
          func_set_next(prev_rear_iterator, front_iterator);
        }
        else {
          tmp = *list;
          *list = front_iterator;
        }
        func_set_next(front_iterator, tmp);

        break;
      }
      /* Advance rear iterator */
      prev_rear_iterator = rear_iterator;
      rear_iterator = func_get_next(rear_iterator);
    }
    /* Advance front iterator */
    prev_front_iterator = front_iterator;
    front_iterator = func_get_next(front_iterator);
  }
}

/*!
\fn       int wave_strcopy(char *dest, const char *src, size_t size)
\brief    function copies up to size-1 characters from the NUL-terminated string src to dst.
          Resulting string is always NUL-terminated.
          This function works similar to "strlcpy", but unlike "strlcpy" it doesn't calculate a length of the source string
          and doesn't require that the source string be NUL-terminated.

\param    src The source string (can point to NULL).
\param    dst The destination buffer.
\param    size The size of the destination buffer.

\return   the result of copying.
\retval   non-zero - source string was copied completely.
\retval   0        - source string was truncated.
*/
int __MTLK_IFUNC wave_strcopy (char *dest, const char *src, size_t size)
{
  size_t count = size;

  if (!count || !dest)
    return 0; /* Source string was truncated */

  if (!src) {
      *dest = '\0';
      return 1;
  }

  while (--count) {
      if ((*dest++ = *src++) == '\0')
        return 1; /* Source string was copied completely */
  }

  /* force NULL termination */
  *dest = '\0';
  /* Check if source string was copied completely */
  return ('\0' == *src);
}


/*!
\fn       int wave_strcat (char *dest, const char *src, size_t size)
\brief    function appends the NUL-terminated string src to dst.
          Resulting string is always NUL-terminated.
          This function works similar to "strlcat", but unlike "strlcat" it doesn't calculate a length of the resulting string
          and doesn't require that the source string be NUL-terminated.

          The destination buffer also may not be NUL-terminated.
          The function will add termination NUL at the end of dest.buffer and return 0 as result.

\param    src The source string (can point to NULL).
\param    dst The destination buffer.
\param    size The size of the destination buffer.

\return   the result of concatenation.
\retval   non-zero - source string was copied completely.
\retval   0        - source string was truncated.
*/
int __MTLK_IFUNC wave_strcat (char *dest, const char *src, size_t size)
{
  size_t count = size;

  if (!count || !dest)
    return 0; /* Source string was truncated */

  /* Find the end of dest string */
  while (--count && *dest) dest++;

  if (!count) {
    /* End of dest buffer reached */
    *dest = '\0';   /* force NULL termination */
    return 0;       /* No space in dest. buffer. */
  }

  if (!src)
    return 1;

  /* Copy source string */
  while (count--) {
      if ((*dest++ = *src++) == '\0')
        return 1; /* Source string was copied completely */
  }

  /* force NULL termination */
  *dest = '\0';
  /* Check if source string was copied completely */
  return ( '\0' == *src );
}


/*!
\fn       int wave_strlen (const char *src, const size_t maxlen)
\brief    function calculates the number of characters in the source string, excluding terminating NUL,
          but at most "maxlen" bytes.
          This function works similar to "strnlen", but unlike "strnlen" it allows "src" to point to NULL.

\param    src The source string (can point to NULL).
\param    maxlen The size of the destination buffer.

\return   number of characters in the source string.
*/
size_t __MTLK_IFUNC wave_strlen (const char *src, const size_t maxlen) {
  size_t count;

  if (!src)
    return 0;

  for (count = 0; count < maxlen; count++) {
    if (src[count] == '\0') {
      break;
    }
  }
  return count;
}

/*****************************************************
 * Safe memcpy
 *****************************************************/

#ifdef WAVE_MEMCPY_DEBUG_INFO
/* Debug output DOESN'T contains function name and line numbers */
void __MTLK_IFUNC _wave_mem_constraint_handler(const mtlk_slid_t slid)
{
  _wave_mem_constraint_handler_cleardst(slid, NULL, 0);
}

void __MTLK_IFUNC _wave_mem_constraint_handler_cleardst(const mtlk_slid_t slid, void *dst, size_t size)
#else
void __MTLK_IFUNC _wave_mem_constraint_handler(const int msg_code)
{
  _wave_mem_constraint_handler_cleardst(msg_code, NULL, 0);
}

void __MTLK_IFUNC _wave_mem_constraint_handler_cleardst(const int msg_code, void *dst, size_t size)
#endif
{
  const char *msg = NULL;

  MTLK_UNREFERENCED_PARAM(msg);

  if (dst) { memset(dst, 0, size); }
  switch (mtlk_slid_get_code(slid)) {
    case WAVE_MEMCPY_ERR_NULL_DEST:
      msg = "dest is NULL"; break;
    case WAVE_MEMCPY_ERR_NULL_SRC:
      msg = "src is NULL" ; break;
    case WAVE_MEMCPY_ERR_DMAX_EXCEEDS_MAX:
      msg = "dmax exceeds max"; break;
    case WAVE_MEMCPY_ERR_SMAX_EXCEEDS_DMAX:
      msg = "smax exceeds dmax"; break;
    default:
      msg = "unknown reason"; break;
  }
#ifdef WAVE_MEMCPY_DEBUG_INFO
  ELOG_DDDDS("wave_memcpy at [%u:%u:%u:%u]: %s",
      mtlk_slid_get_oid(slid),
      mtlk_slid_get_gid(slid),
      mtlk_slid_get_fid(slid),
      mtlk_slid_get_lid(slid), msg);
#else
  ELOG_S("[wave_memcpy()]: %s", msg);
#endif
#ifdef __KERNEL__
  dump_stack();
#endif
}
