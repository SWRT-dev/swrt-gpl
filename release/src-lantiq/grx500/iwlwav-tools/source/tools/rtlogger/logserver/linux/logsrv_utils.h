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

#ifndef __LOGSRV_UTILS_H__
#define __LOGSRV_UTILS_H__

#include <stdio.h>

extern int debug;

int get_line(const char *filename, char *buf, size_t buf_size, FILE *fl,
  int trim_crlf, int *peof);
int get_word(char **pp, char *buf, size_t buf_size);
int is_spcrlf(char c);
void skip_spcrlf(char **pp);
int str_decode(char *str);

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#endif

#define SIZEOF_MEMB(type, field) (sizeof(((type *)0)->field))

#define USE_TIMESTAMP

#ifdef USE_TIMESTAMP
#define TIMESTAMP fprintf(stderr, "[%010lu] ", timestamp())
#else
#define TIMESTAMP
#endif

#ifdef CPTCFG_IWLWAV_DEBUG
#define VERIFY(expr) ASSERT(expr)
#else
#define VERIFY(expr) ((void)(expr))
#endif

#define LOGSRV_LIST_REMOVE(head, item, type, next)  \
  do {                                       \
    if ((item) == (head)) {                  \
      (head) = (item)->next;                 \
    } else {                                 \
      type *temp = (head);                   \
      while (temp && (temp->next != (item))) \
        temp = temp->next;                   \
        if (temp)                            \
          temp->next = (item)->next;         \
    }                                        \
  } while(0)

#define LOGSRV_LIST_PUSH_FRONT(head, item, next)    \
  do {                                       \
    (item)->next = (head);                   \
    (head) = (item);                         \
  } while(0)

#endif // !__LOGSRV_UTILS_H__

