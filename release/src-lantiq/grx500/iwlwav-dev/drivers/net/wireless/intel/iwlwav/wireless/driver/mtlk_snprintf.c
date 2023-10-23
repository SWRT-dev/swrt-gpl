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
#include "mtlk_snprintf.h"

#define LOG_LOCAL_GID   GID_SPRINTF
#define LOG_LOCAL_FID   0

#define MTLK_SNPRINTF_MAX_SUBFORMAT 16

#define MTLK_SNPRINTF_HANDLE_RES                     \
  do {                                               \
    if (written < 0) {                               \
      goto finish;                                   \
    }                                                \
    else if ((ptrdiff_t)written >= (end - buffer)) { \
      buffer = end;                                  \
      goto finish;                                   \
    }                                                \
    else {                                           \
      buffer += (ptrdiff_t)written;                  \
    }                                                \
  } while (0)

#define VSNPRINTF_ADD_SUBFORMAT_CHAR(c)                                   \
  for (;;) {                                                              \
    if ((size_t)(subformat - subformat_buffer) >= sizeof(subformat_buffer) - 1) {   \
      MTLK_ASSERT(0);                                                     \
      written = snprintf(buffer, end - buffer, "%%subformat overflow%%"); \
      MTLK_SNPRINTF_HANDLE_RES;                                           \
      goto finish;                                                        \
    }                                                                     \
    *subformat++ = (c);                                                   \
    break;                                                                \
  }

int __MTLK_IFUNC mtlk_vsnprintf (char *buffer, size_t buffer_size, const char *format, va_list argv)
{
  char *start = buffer;
  char *end = buffer + buffer_size;
  char subformat_buffer[MTLK_SNPRINTF_MAX_SUBFORMAT];
  char *subformat;
  int param_size;
  int written;

  for (; *format; format++) {
    if ((end - buffer) <= 0)
      break;

    if (*format != '%') {
      *buffer = *format;
      buffer++;
      continue;
    }

    subformat = subformat_buffer;

    VSNPRINTF_ADD_SUBFORMAT_CHAR('%');

    param_size = sizeof(int);

    process_qualifier:
      format++; /* this also skips first '%' */
      switch (*format) {
      case '-': /* walk through */
      case '+': /* walk through */
      case '0': /* walk through */
      case '1': /* walk through */
      case '2': /* walk through */
      case '3': /* walk through */
      case '4': /* walk through */
      case '5': /* walk through */
      case '6': /* walk through */
      case '7': /* walk through */
      case '8': /* walk through */
      case '9':
        VSNPRINTF_ADD_SUBFORMAT_CHAR(*format);
        goto process_qualifier;
      case 'l':
        VSNPRINTF_ADD_SUBFORMAT_CHAR(*format);
        if (*(format + 1) == 'l') {
          VSNPRINTF_ADD_SUBFORMAT_CHAR(*(format + 1));
          param_size = sizeof(long long int);
          format++;
        } else {
          param_size = sizeof(long int);
        }
        goto process_qualifier;
      case 'h':
        if (*(format + 1) == 'h') {
          param_size = sizeof(char);
          format++;
        } else {
          param_size = sizeof(short int);
        }
        goto process_qualifier;
      case 'z':
        param_size = sizeof(size_t);
        goto process_qualifier;
      default:
        break;
      }

    switch (*format) {
    case '%':
      *buffer = *format;
      buffer++;
      continue;
    case 's':
      written = snprintf(buffer, end - buffer, "%s", va_arg(argv, char*));
      MTLK_SNPRINTF_HANDLE_RES;
      continue;
    case 'p':
      {
#ifdef CONFIG_WAVE_DEBUG
        /* On new kernels %p may not print addresses due to changes in Kernel. The %px is recommended. */
        written = snprintf(buffer, end - buffer, "%px", va_arg(argv, void*));
        MTLK_SNPRINTF_HANDLE_RES;
#else
        written = snprintf(buffer, end - buffer, "%s", "(REMOVED)");
        MTLK_SNPRINTF_HANDLE_RES;
        va_arg(argv, void*);
#endif
        break;
      }
    case 'c':
    case 'd':
    case 'i':
    case 'u':
    case 'x':
    case 'X':
      {
        VSNPRINTF_ADD_SUBFORMAT_CHAR(*format);
        VSNPRINTF_ADD_SUBFORMAT_CHAR('\0');
        switch (param_size) {
        case sizeof(uint8): /* walk through */
        case sizeof(uint16):
          /*
           * Both uint8 and uint16
           * promoted to int when passed though (, ...).
           */
          written = snprintf(buffer, end - buffer, subformat_buffer, va_arg(argv, int));
          MTLK_SNPRINTF_HANDLE_RES;
          break;
        case sizeof(uint32):
          written = snprintf(buffer, end - buffer, subformat_buffer, va_arg(argv, uint32));
          MTLK_SNPRINTF_HANDLE_RES;
          break;
        case sizeof(uint64):
          written = snprintf(buffer, end - buffer, subformat_buffer, va_arg(argv, uint64));
          MTLK_SNPRINTF_HANDLE_RES;
          break;
        default:
          written = snprintf(buffer, end - buffer, "%%invalid size%%");
          MTLK_SNPRINTF_HANDLE_RES;
          goto finish;
        }
        break;
      }
    case 'Y': /* extension: ethernet address */
      {
        uint8 *mac_addr = va_arg(argv, uint8*);
        written = snprintf(buffer, end - buffer, MAC_PRINTF_FMT, MAC_PRINTF_ARG(mac_addr));
        MTLK_SNPRINTF_HANDLE_RES;
        break;
      }
    case 'B': /* extension: IPv4  address */
      {
        uint32 arg = va_arg(argv, uint32);
        uint8 *ipv4_addr = (uint8*)&arg;
        written = snprintf(buffer, end - buffer, IP4_PRINTF_FMT, IP4_PRINTF_ARG(ipv4_addr));
        MTLK_SNPRINTF_HANDLE_RES;
        break;
      }
    case 'K': /* extension: IPv6  address */
      {
        uint8 *ipv6_addr = va_arg(argv, uint8*);
        written = snprintf(buffer, end - buffer, IP6_PRINTF_FMT, IP6_PRINTF_ARG(ipv6_addr));
        MTLK_SNPRINTF_HANDLE_RES;
        break;
      }
    default:
      written = snprintf(buffer, end - buffer, "%%invalid qualifier%%");
      MTLK_SNPRINTF_HANDLE_RES;
      goto finish;
    }
  }

finish:
  /*
   * Ensure that resulting string is zero-terminated.
   */
  if (buffer_size) {
    if (buffer < end)
      *buffer = '\0';
    else
      *(end - 1) = '\0';
  }

  return buffer - start;
}


int __MTLK_IFUNC mtlk_snprintf (char *buffer, size_t buffer_size, const char *format, ...)
{
  int     res;
  va_list argv;

  va_start(argv, format);
  res = mtlk_vsnprintf(buffer, buffer_size, format, argv);
  va_end(argv);

  return res;
}

