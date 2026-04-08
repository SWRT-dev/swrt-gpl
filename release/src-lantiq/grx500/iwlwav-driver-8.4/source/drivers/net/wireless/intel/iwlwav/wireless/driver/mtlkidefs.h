/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/


#ifdef MTLK_IDEFS_OFF
#undef MTLK_IDEFS_OFF
#undef MTLK_IDEFS_ON
#endif

#if defined(MTLK_IDEFS_ON) && defined(__INTERNAL_MTLK_IDEFS_ON__)

#error mtlkidefs.h already included here!

#elif defined(MTLK_IDEFS_ON) && !defined(__INTERNAL_MTLK_IDEFS_ON__)
#define __INTERNAL_MTLK_IDEFS_ON__

/**********************************************************/
/* Packing ON here                                        */
#if !defined(MTLK_IDEFS_PACKING) /* native (default) */
#define __MTLK_IDATA
#elif defined(MTLK_IDEFS_PACKING) && (MTLK_IDEFS_PACKING == 1)  /* pack(1) */
#define __MTLK_IDATA __attribute__((aligned(1), packed))
#else
#error Unsupported MTLK_I_PACKING
#endif
/**********************************************************/

#elif !defined(MTLK_IDEFS_ON) && defined(__INTERNAL_MTLK_IDEFS_ON__)

/**********************************************************/
/* Packing OFF here                                       */
#undef __MTLK_IDATA
#undef MTLK_IDEFS_PACKING
/**********************************************************/

#undef __INTERNAL_MTLK_IDEFS_ON__
#endif
