/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _MTLK_ALGORITHMS_H_
#define _MTLK_ALGORITHMS_H_

typedef void* (*mtlk_get_next_t)(void* item);
typedef void  (*mtlk_set_next_t)(void* item, void* next);
typedef int   (*mtlk_is_less_t)(void* item1, void* item2);

/*! 
  \fn      void* __MTLK_IFUNC mtlk_sort_slist(...)
  \brief   Sort abstract singly linked list.

  \param   list          Pointer to the first element of the list to be sorted
  \param   func_get_next Pointer to function that returns next element of the given element
  \param   func_set_next Pointer to function that sets next element for the given element
  \param   func_is_less  Pointer to compare function that returns non-zero if first given element less then second, 0 otherwise.

  \return  Pointer to new first element of the list

  \warning This routine assumes that given list is non-cyclic singly linked list, next of its last element is NULL
 */

void __MTLK_IFUNC
mtlk_sort_slist(void** list, mtlk_get_next_t func_get_next,
                mtlk_set_next_t func_set_next, mtlk_is_less_t func_is_less);


/*!
\fn       int wave_strcopy(char *dest, const char *src, size_t size)
\brief    function copies up to size-1 characters from the NUL-terminated string src to dst.
          Resulting string is always NUL-terminated.
          This function works similar to "strlcpy", but unlike "strlcpy" it doesn't calculate a length of the source string
          and doesn't require that the source string be NUL-terminated.

\param    src The sources string (can be NULL).
\param    dst The destination buffer.
\param    size The size of the destination buffer.

\return the result of copying
\retval non-zero - source string was copied completely.
\retval 0        - source string was truncated.
*/
int __MTLK_IFUNC
wave_strcopy (char *dest, const char *src, size_t size);

/*!
\fn       int wave_strncopy(char *dest, const char *src, size_t size, ssize_t count)
\brief    function copies up to "count", but not more than "size-1" characters
          from the NUL-terminated string src to dst.
          Unlike standard "strncpy" this function:
          - takes into account size of destination buffer;
          - doesn't calculate a length of the source string;
          - doesn't fill rest of destination buffer with zeroes;
          - resulting string is always NUL-terminated.

\param    src The sources string (can be NULL).
\param    dst The destination buffer.
\param    size The size of the destination buffer.
\param    count The maximal number of characters for copying

\return the result of copying
\retval non-zero - source string was copied completely.
\retval 0        - source string was truncated.
*/

int static __INLINE
wave_strncopy (char *dest, const char *src, size_t size, ssize_t count)
{
    if (count < 0) return 0;
    return wave_strcopy(dest, src, MIN((size_t)count + 1, size));
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
int __MTLK_IFUNC
wave_strcat (char *dest, const char *src, size_t size);


#ifndef RSIZE_MAX_STR
#define RSIZE_MAX_STR      ( 4UL << 10 )  /* 4KB */
#endif


/*!
\fn       int wave_strlen (const char *src, const size_t maxlen)
\brief    function calculates the number of characters in the source string, excluding terminating NUL,
          but at most "maxlen" bytes.
          This function works similar to "strnlen", but unlike "strnlen" it allows "src" to point to NULL.

\param    src The source string (can point to NULL).
\param    maxlen The size of the destination buffer.

\return   number of characters in the source string.
*/
size_t __MTLK_IFUNC
wave_strlen (const char *src, const size_t maxlen);


/*****************************************************
 * Safe memcpy
 *****************************************************/

/* If defined, unsafe but more efficient memcpy() will be used */
/* #define WAVE_MEMCPY_UNSAFE */

/* Debug output contains function name and line numbers, but may increase overhead when use wave_memcpy() */
#define WAVE_MEMCPY_DEBUG_INFO

#ifndef RSIZE_MAX_MEM
#define RSIZE_MAX_MEM      ( 256UL << 20 )  /* 256MB */
#endif

#define WAVE_MEMCPY_ERR_NULL_DEST           1
#define WAVE_MEMCPY_ERR_NULL_SRC            2
#define WAVE_MEMCPY_ERR_DMAX_EXCEEDS_MAX    3
#define WAVE_MEMCPY_ERR_SMAX_EXCEEDS_DMAX   4

#ifdef WAVE_MEMCPY_DEBUG_INFO

#define wave_mem_constraint_handler(msg_code) \
  _wave_mem_constraint_handler(MTLK_SLID_CODE((msg_code)))

#define wave_mem_constraint_handler_cleardst(msg_code, dst, size) \
  _wave_mem_constraint_handler_cleardst(MTLK_SLID_CODE((msg_code)), (dst), (size))

void __MTLK_IFUNC _wave_mem_constraint_handler(const mtlk_slid_t slid);
void __MTLK_IFUNC _wave_mem_constraint_handler_cleardst(const mtlk_slid_t slid, void *dst, size_t size);
#else
#define wave_mem_constraint_handler(msg_code) \
  _wave_mem_constraint_handler((msg_code))

#define wave_mem_constraint_handler_cleardst(msg_code, dst, size) \
  _wave_mem_constraint_handler_cleardst((msg_code), (dst), (size))

void __MTLK_IFUNC _wave_mem_constraint_handler(const int msg_code);
void __MTLK_IFUNC _wave_mem_constraint_handler_cleardst(const int msg_code, void *dst, size_t size);
#endif

static __INLINE void * __wave_memcpy_prim (void *dst, const void *src, size_t size)
{
    return memcpy(dst, src, size);
}

#ifdef WAVE_MEMCPY_UNSAFE
#define wave_memcpy(dest, dmax, src, smax) \
    __wave_memcpy_prim((dest), (src), (smax))
#else
/* Original memcpy() does nothing if smax == 0.
 * wave_memcpy() keeps this behavior and don't produce warnings if (smax==0) && (src==NULL)
 */

#ifndef __KERNEL__
/* memcpy with runtime checking only (static error checking is not available in user space code at this moment) */
#define wave_memcpy(dest, dmax, src, smax) \
do {                                                                     \
    void *_dp = (dest);                                                  \
    const void *_sp = (src);                                             \
    size_t _dmax = (dmax), _smax = (smax);                               \
    if (!_smax)                                                          \
         break;                                                          \
    if (__UNLIKELY(_dp == NULL)) {                                       \
        wave_mem_constraint_handler(WAVE_MEMCPY_ERR_NULL_DEST);          \
    }                                                                    \
    else if (__UNLIKELY(_dmax > RSIZE_MAX_MEM)) {                        \
        wave_mem_constraint_handler(WAVE_MEMCPY_ERR_DMAX_EXCEEDS_MAX);   \
    }                                                                    \
    else if (__UNLIKELY(_smax > _dmax)) {                                \
        wave_mem_constraint_handler_cleardst(                            \
            WAVE_MEMCPY_ERR_SMAX_EXCEEDS_DMAX, _dp, _dmax);              \
    }                                                                    \
    else if (__UNLIKELY(_sp == NULL)) {                                  \
        wave_mem_constraint_handler_cleardst(                            \
            WAVE_MEMCPY_ERR_NULL_SRC, _dp, _dmax);                       \
    }                                                                    \
    else {                                                               \
        __wave_memcpy_prim(_dp, _sp, _smax);                             \
    }                                                                    \
} while (0)
#else
/* memcpy with compile time/runtime checking */
#define wave_memcpy(dest, dmax, src, smax) \
do {                                                                     \
    void *_dp = (dest);                                                  \
    const void *_sp = (src);                                             \
    size_t _dmax = (dmax), _smax = (smax);                               \
    if (!_smax)                                                          \
       break;                                                            \
    if (__builtin_constant_p(dmax) && __builtin_constant_p(smax)         \
       && (_smax > _dmax)) {                                             \
       MTLK_STATIC_ERROR("Error: smax > dmax");                          \
    }                                                                    \
    else if (__builtin_constant_p(dmax) && (_dmax > RSIZE_MAX_MEM)) {    \
       MTLK_STATIC_ERROR("Error: dmax > RSIZE_MAX_MEM");                 \
    }                                                                    \
    else if (__builtin_constant_p(dest) && !_dp) {                       \
       MTLK_STATIC_ERROR("Error: dest is NULL");                         \
    }                                                                    \
    else if (__builtin_constant_p(src) && !_sp) {                        \
       MTLK_STATIC_ERROR("Error: src is NULL");                          \
    }                                                                    \
    else if (__UNLIKELY(_dp == NULL)) {                                  \
        wave_mem_constraint_handler(WAVE_MEMCPY_ERR_NULL_DEST);          \
    }                                                                    \
    else if (__UNLIKELY(_dmax > RSIZE_MAX_MEM)) {                        \
        wave_mem_constraint_handler(WAVE_MEMCPY_ERR_DMAX_EXCEEDS_MAX);   \
    }                                                                    \
    else if (__UNLIKELY(_smax > _dmax)) {                                \
        wave_mem_constraint_handler_cleardst(                            \
            WAVE_MEMCPY_ERR_SMAX_EXCEEDS_DMAX, _dp, _dmax);              \
    }                                                                    \
    else if (__UNLIKELY(_sp == NULL)) {                                  \
        wave_mem_constraint_handler_cleardst(                            \
            WAVE_MEMCPY_ERR_NULL_SRC, _dp, _dmax);                       \
    }                                                                    \
    else {                                                               \
        __wave_memcpy_prim(_dp, _sp, _smax);                             \
    }                                                                    \
} while (0)
#endif /* __KERNEL__*/
#endif /* WAVE_MEMCPY_UNSAFE */

#define wave_wordcpy(dest, dmax, src, smax) \
          wave_memcpy(dest, ((dmax) * sizeof(uint32)), src, ((smax) * sizeof(uint32)))

/*************************************************************************
 * Support of Fixed point arithmetic
 *************************************************************************/

/* Fixed point format of signed integer values: NBI.NBF
 * NBI - number of bits of integer part, NBF - number of bits of fractional part
 **/
#define BITS_PER_INT              (8 * sizeof(int))
#define FIXP_MIN_INT(nbi, nbf)    ((-1) << ((nbi) + (nbf) - 1))

/* Sign extension */
static __INLINE int
wave_fixp_sign_ext (int val, const unsigned nbi, const unsigned nbf)
{
  val <<= (BITS_PER_INT - ((nbi) + (nbf)));
  val >>= (BITS_PER_INT - ((nbi) + (nbf)));
  return val;
}

/* Scale a fixed point value i.e. change the number of bits of the fractional part
 */
static __INLINE int
wave_fixp_scale (int val, const unsigned nbi, const unsigned nbf_in, const unsigned nbf_out)
{
  int nbs = nbf_out - nbf_in; /* to scale */
  int res;

  if (nbs > 0) { /* scale up */
    res = val << nbs;
  } else { /* scale down or keep the same scale */
    res = val;
    res += ((val >> (BITS_PER_INT - 1)) << (nbf_out - 1)); /* negative/positive round up */
    res >>= -nbs; /* scale down */
  }
  return res;
}

#endif //_MTLK_ALGORITHMS_H_
