/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/*******************************************************************************
 									Utility API's library that can be used across QoS AL
*********************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <netdb.h>
#include <string.h>
#include <qosal_utils.h>
#include <qosal_ugw_debug.h>

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

/* =============================================================================
 *  Function Name : util_get_name_from_value
 *  Description   : API that returns name in string form for given enum value.
 * ============================================================================*/
char* util_get_name_from_value(int32_t value, name_value_t *nv)
{
	name_value_t *temp_local = nv;

	while(temp_local && temp_local->value >= 0) {
		if(temp_local->value == value) {
			return temp_local->name;
		}
		temp_local++;
	}

	return NULL;
}


/* =============================================================================
 *  Function Name : util_get_ipaddr_family 
 *  Description   : Function that returns family type of a given ip address.
 * ============================================================================*/
int32_t util_get_ipaddr_family(const char *ipaddr) {
        struct addrinfo hint, *res = NULL;
        int32_t ret=-1;
        memset_s(&hint, sizeof(hint), '\0');

        hint.ai_family = PF_UNSPEC;
        hint.ai_flags = AI_NUMERICHOST;

        ret = getaddrinfo(ipaddr, NULL, &hint, &res);
        if (ret) {
                ret=-1;
        }
        else if(res->ai_family == AF_INET) {
                ret=AF_INET;
        } else if (res->ai_family == AF_INET6) {
                ret=AF_INET6;
        }

        freeaddrinfo(res);
        return ret;
}

void splitStr(char *pcStr, char *pcDelim, char pcSubStr[][MAX_NAME_LEN], int32_t *nSubCnt)
{
	char *pcStrCopy = NULL, *pcTemp1 = NULL, *pcTemp2 = NULL;
	rsize_t strmax;

	*nSubCnt = 0;
	pcStrCopy = pcStr;
	strmax = strnlen_s(pcStrCopy, MAX_STR_LEN);
	pcTemp1 = strtok_s(pcStrCopy, &strmax, pcDelim, &pcTemp2);
	while(pcTemp1 != NULL) {
		strncpy_s(pcSubStr[*nSubCnt], MAX_NAME_LEN, pcTemp1, MAX_NAME_LEN);
		pcTemp1 = strtok_s(NULL, &strmax, pcDelim, &pcTemp2);
		*nSubCnt = *nSubCnt + 1;
	}

	return;
}
