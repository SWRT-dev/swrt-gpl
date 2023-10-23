/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __QOS_AL_UTIL_API_H
#define __QOS_AL_UTIL_API_H
#include <commondefs.h>

typedef struct {
	char name[MAX_NAME_LEN];
	int32_t value;
}name_value_t;

char* util_get_name_from_value(int32_t value, name_value_t *nv);
ifinggrp_t xml_lookup_ingress_group(const char *ifname);
#endif
