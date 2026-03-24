/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * 
 *
 * Utilities
 *
 * Written by: Andrey Fidrya
 *
 */

#include "mtlkinc.h"
#include <stdio.h>
#include <utils.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include "logsrv_utils.h"

#define LOG_LOCAL_GID   GID_LOGSRV_UTILS
#define LOG_LOCAL_FID   1

int
get_line(const char *filename, char *buf, size_t buf_size, FILE *fl,
  int trim_crlf, int *peof)
{
  int num_read;

  *peof = 0;

  if (!fgets(buf, buf_size, fl)) {
    if (feof(fl)) {
      *peof = 1;
      return 0;
    }
    ELOG_SS("%s: cannot read: %s", filename, strerror(errno));
    return -1;
  }

  num_read = mtlk_osal_strnlen(buf, buf_size);

  if (num_read >= 1 && buf[num_read - 1] != '\n') {
    ELOG_SS("%s: line too long: %s", filename, buf);
    return -1;
  }

  if (trim_crlf) {
    while (num_read >= 1 &&
        (buf[num_read - 1] == '\n' || buf[num_read - 1] == '\r')) {
      buf[--num_read] = '\0';
    }
  }

  return 0;
}

int
get_word(char **pp, char *buf, size_t buf_len)
{
  int at = 0;
  char c;

  skip_spcrlf(pp);

  for (;;) {
    c = **pp;
    if (!c || is_spcrlf(c))
      break;
    if (at >= buf_len - 1)
      return -1;
    *buf++ = c;
    ++at;
    ++(*pp);
  }
  *buf = '\0';
    
  return 0;
}

int
is_spcrlf(char c)
{
  switch (c) {
  case ' ':
  case '\t':
  case '\r':
  case '\n':
    return 1;
  default:
    break;
  }
  return 0;
}

void
skip_spcrlf(char **pp)
{
  for (;;) {
    switch (**pp) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
      ++(*pp);
      continue;
    case '\0':
    default:
      return;
    }
  }
}

int
str_decode(char *str)
{
  char *p = str;
  char *to = str;
  char c;

  skip_spcrlf(&p);

  while ((c = *p) != '\0') {
    ++p;

    if (c == '\\') {
      switch (*p) {
      case 'r':
        *to++ = '\r';
        break;
      case 't':
        *to++ = '\t';
        break;
      case 'n':
        *to++ = '\n';
        break;
      case '\\':
        *to++ = '\\';
        break;
      case '\'':
        *to++ = '\'';
        break;
      case '\"':
        *to++ = '\"';
        break;
      default:
        return -1;
      }
      ++p;
    } else {
      *to++ = c;
    }
  }

  *to = '\0';
  return 0;
}

