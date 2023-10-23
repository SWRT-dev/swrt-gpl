/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __QOS_AL_UTIL_API_H
#define __QOS_AL_UTIL_API_H
#include <commondefs.h>

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

typedef struct {
	char name[MAX_NAME_LEN];
	int32_t value;
}name_value_t;

char* util_get_name_from_value(int32_t value, name_value_t *nv);
int32_t util_get_ipaddr_family(const char *ipaddr);
void splitStr(char *pcStr, char *pcDelim, char pcSubStr[][MAX_NAME_LEN], int32_t *nSubCnt);

#define IS_VALID_CHAR(x) \
	( (x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') || (x >= '0' && x <= '9') || (x == '_') )

#define IS_SPECIAL_CHAR(x) !(IS_VALID_CHAR(x))

#define validate_range(value, min, max) \
	((value >= min && value <= max )? 1: 0)
#endif
