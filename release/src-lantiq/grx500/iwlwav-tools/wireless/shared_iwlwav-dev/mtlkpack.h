/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifdef MTLK_PACK_OFF
#undef MTLK_PACK_OFF
#undef MTLK_PACK_ON
#endif

#if defined(MTLK_PACK_ON) && defined(__INTERNAL_MTLK_PACK_ON__)

#error mtlkinc.h already included!

#elif defined(MTLK_PACK_ON) && !defined(__INTERNAL_MTLK_PACK_ON__)
#define __INTERNAL_MTLK_PACK_ON__

/**********************************************************/
/* Packing ON here                                        */
// For uint8 arrays
#define MTLK_PAD4(S)       (((4 - ((S) & 0x3)) & 0x3) + (S))
// For uint16 arrays
#define MTLK_PAD2(S)       (((2 - ((S) & 0x1)) & 0x1) + (S))

#define __MTLK_PACKED       __attribute__((aligned(1), packed))
/**********************************************************/

#elif !defined(MTLK_PACK_ON) && defined(__INTERNAL_MTLK_PACK_ON__)

/**********************************************************/
/* Packing OFF here                                       */
#undef __MTLK_PACKED
#undef MTLK_PAD4
#undef MTLK_PAD2
/**********************************************************/

#undef __INTERNAL_MTLK_PACK_ON__
#endif



