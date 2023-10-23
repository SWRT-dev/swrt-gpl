/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_BFIELD_H__
#define __MTLK_BFIELD_H__

/*********************************************************************************
 * Defines for working with bitmasks.
 *
 * CAUTION: PRESET does not zero the bits prior to set!
 *          Use it only if you sure the var is zeroed! For example:
 * Example:
 *
 * uint32 var = 0;
 *
 * MTLK_U_BITS_GET(var, 2, 5);        - gets 5  bits from BIT2
 * MTLK_U_BITS_GET(var, 0, 11);       - gets 11 bits from BIT0
 *
 * MTLK_U_BITS_SET(var, 2, 5, 0x1F);  - sets 5  bits from BIT2 to 0x1F
 * MTLK_U_BITS_SET(var, 0, 11, 0);    - sets 11 bits from BIT0 to 0
 *
 * MTLK_U_BITS_PRESET(var, 4, 7, 3);  - sets 7  bits from BIT4 to 3 
 *
 * MTLK_U_BITS_ZERO(var, 3, 2);       - zeros 2 bits from BIT3
 * 
 * MTLK_U_BITS_VALUE(3, 4, 2, uint32);- returns uint32 value of 2 set in 4 bits from 
 *                                      BIT3. I.e. 0x00000010. May be used for initialization:
 *                                      uint32 var = MTLK_U_BITS_VALUE(3, 4, 2, uint32) |
 *                                                   MTLK_U_BITS_VALUE(7, 1, 1, uint32);
**********************************************************************************/

#define _MTLK_U_AUX_BITS(len)                            \
    ((1 << (len)) - 1)
#define _MTLK_U_AUX_BITMASK(from, len)                   \
    (_MTLK_U_AUX_BITS(len) << (from))

#define _MTLK_U_AUX_BITS_GET(bf, from, len)              \
    (((bf) >> (from)) & ((1 << (len)) - 1))
#define _MTLK_U_AUX_BITS_ZERO(bf, from, len)             \
    (bf) &= ~_MTLK_U_AUX_BITMASK(from, len)
#define _MTLK_U_AUX_BITS_SET(bf, from, len, val)         \
    (bf) |= (((val) << (from)) & _MTLK_U_AUX_BITMASK(from, len))

#define MTLK_U_BITS_GET(bf, from, len)                   \
    _MTLK_U_AUX_BITS_GET((bf), (from), (len))

#define MTLK_U_BITS_ZERO(bf, from, len)                  \
    _MTLK_U_AUX_BITS_ZERO((bf), (from), (len))

#define MTLK_U_BITS_SET(bf, from, len, val)              \
    do {                                                 \
      _MTLK_U_AUX_BITS_ZERO(bf, from, len);              \
      _MTLK_U_AUX_BITS_SET(bf, from, len, val);          \
    } while(0)

#define MTLK_U_BITS_VALUE(from, len, val, type)          \
  ((((type)(val)) & _MTLK_U_AUX_BITS(len)) << (from))

/*********************************************************************************
 * Defines for working with bitmask structure fields.
 *
 * #define Field1 MTLK_BFIELD_INFO(0, 1)
 * #define Field2 MTLK_BFIELD_INFO(3, 10)
 *
 * MTLK_U_BITS_GET(mask_field, Field1);     - gets Field1 bitfield
 * MTLK_U_BITS_SET(mask_field, Field1, 10); - sets Field1 bitfield to 10
 * MTLK_U_BITS_ZERO(mask_field, Field1);    - zeros Field1 bitfield
 *********************************************************************************/

#define _MTLK_BFIELD_INFO_PACK(info, offset)             \
    (((uint16)(uint8)(info)) << (offset))
#define _MTLK_BFIELD_INFO_UNPACK(info, offset)           \
    ((uint8)(((uint16)(info)) >> (offset)))

#define MTLK_BFIELD_INFO(from, len)                      \
    (_MTLK_BFIELD_INFO_PACK(from, 0) |                   \
     _MTLK_BFIELD_INFO_PACK(len, 8))

#define MTLK_BFIELD_GET(var, info)                       \
    MTLK_U_BITS_GET((var),                               \
                    _MTLK_BFIELD_INFO_UNPACK(info, 0),   \
                    _MTLK_BFIELD_INFO_UNPACK(info, 8))

#define MTLK_BFIELD_ZERO(var, info)                      \
    MTLK_U_BITS_ZERO((var),                              \
                     _MTLK_BFIELD_INFO_UNPACK(info, 0),  \
                     _MTLK_BFIELD_INFO_UNPACK(info, 8))

#define MTLK_BFIELD_SET(var, info, val)                  \
    MTLK_U_BITS_SET(var,                                 \
                    _MTLK_BFIELD_INFO_UNPACK(info, 0),   \
                    _MTLK_BFIELD_INFO_UNPACK(info, 8),   \
                    (val))

#define MTLK_BFIELD_VALUE(info, val, type)               \
    MTLK_U_BITS_VALUE(_MTLK_BFIELD_INFO_UNPACK(info, 0), \
                      _MTLK_BFIELD_INFO_UNPACK(info, 8), \
                      (val),                             \
                      type)

/* Handy shortcuts for specific bit set/get */
#define MTLK_BIT_SET(var, bitno, val) MTLK_U_BITS_SET(var, bitno, 1, val)
#define MTLK_BIT_GET(var, bitno)      MTLK_U_BITS_GET(var, bitno, 1)

#endif /* __MTLK_BFIELD_H__ */

