/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __FORMATS_H__
#define __FORMATS_H__

#define __BYTE_ARG(b,i)            (((uint8 *)(b))[i])
#define __BYTE_ARG_TYPE(t,b,i)     ((t)__BYTE_ARG((b),(i)))

#define __MAKE_WORD(h,l)           ((((uint16)((uint8)(h))) << 8) | (uint8)(l))
#define __WORD_ARG_TYPE(t,b,ih,il) ((t)__MAKE_WORD(__BYTE_ARG((b),(ih)),__BYTE_ARG((b),(il))))

/* to use with %d, %i etc. conversion specifiers as they expect int argument */
#define _BYTE_ARG_S(b,i)            __BYTE_ARG_TYPE(int,(b),(i))
#define _WORD_ARG_S(b,ih,il)        __WORD_ARG_TYPE(int,(b),(ih),(il))
/* to use with %u, %x, %X etc. conversion specifiers as they expect uint32 argument */
#define _BYTE_ARG_U(b,i)            __BYTE_ARG_TYPE(uint32,(b),(i))
#define _WORD_ARG_U(b,ih,il)        __WORD_ARG_TYPE(uint32,(b),(ih),(il))

#define MAC_ADDR_LENGTH (6)
#define MAC_PRINTF_FMT "%02X:%02X:%02X:%02X:%02X:%02X"
#define MAC_PRINTF_ARG(x) \
  _BYTE_ARG_U((x), 0),_BYTE_ARG_U((x), 1),_BYTE_ARG_U((x), 2),_BYTE_ARG_U((x), 3),_BYTE_ARG_U((x), 4),_BYTE_ARG_U((x), 5)

#define IP4_ADDR_LENGTH (4)
#define IP4_PRINTF_FMT "%u.%u.%u.%u"
#define IP4_PRINTF_ARG(x) \
  _BYTE_ARG_U((x), 0),_BYTE_ARG_U((x), 1),_BYTE_ARG_U((x), 2),_BYTE_ARG_U((x), 3)

#define IP6_ADDR_LENGTH (16)
#define IP6_PRINTF_FMT "%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X"
#define IP6_PRINTF_ARG(x) \
  _WORD_ARG_U((x),  0,  1), \
  _WORD_ARG_U((x),  2,  3), \
  _WORD_ARG_U((x),  4,  5), \
  _WORD_ARG_U((x),  6,  7), \
  _WORD_ARG_U((x),  8,  9), \
  _WORD_ARG_U((x), 10, 11), \
  _WORD_ARG_U((x), 12, 13), \
  _WORD_ARG_U((x), 14, 15)

#endif /* __FORMATS_H__ */
