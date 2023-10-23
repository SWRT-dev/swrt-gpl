/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "dwpald_hostap_parse.h"
#include "logs.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#if defined YOCTO
#include <slibc/string.h>
#else
#include <stddef.h>
#include "libsafec/safe_str_lib.h"
#include "libsafec/safe_mem_lib.h"
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(__array) (sizeof(__array)/sizeof(__array[0]))
#endif


/* Convert regular string to hex string */
static inline void _char_to_hex(const char c, char *out)
{
	static const char hex[16]="0123456789ABCDEF";
	out[0] = hex[(unsigned)c >> 4];
	out[1] = hex[(unsigned)c & 0x0f];
}

static inline int _hex_to_num(const char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

static int _hex_to_byte(const char *hex)
{
	int hi=0, lo=0;

	hi = _hex_to_num(hex[0]);
	if (hi < 0) return -1;

	lo = _hex_to_num(hex[1]);
	if (lo < 0) return -1;

	return (hi << 4) | lo;
}

bool dwpald_str_to_hex(char *out_hex_str, size_t size, const char *str)
{
	size_t len = strnlen_s(str, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
	if (len*2 + 1 > size)
		goto failure;

	while (*str) {
		_char_to_hex(*str, out_hex_str);
		str++; out_hex_str+=2;
	}
	*out_hex_str = '\0';
	return true;

failure:
	*out_hex_str = '\0';
	return false;
}

bool dwpald_hex_to_str(char *out_str, size_t size, const char *hex_str)
{
	size_t len;
	if (size == 0)
		return false;

	len = strnlen_s(hex_str, (size+1)*2);
	if (len/2 + 1 > size)
		goto failure;

	if ((len & 1) != 0)
		goto failure;

	while (*hex_str) {
		int c = _hex_to_byte(hex_str);
		if (c < 0) goto failure;
		*out_str = (char)c;
		out_str++; hex_str+=2;
	}
	*out_str = '\0';
	return true;

failure:
	*out_str = '\0';
	return false;
}


/**************************************************************************/
/*! \fn int dwpald_hostap_cmd_build(char *out, size_t size,
		const char *cmd, const dwpald_param params[])
 **************************************************************************
 *  \brief      Build hostap command using fields descriptor params[]
 *  \param[out] char* out        - The output buffer
 *  \param[in]  size_t size      - Size of the output buffer in bytes
 *  \param[in]  const char *cmd  - Hostapd command name
 *  \param[in]  const dwpald_param params[] - The command build information (list of the parameters)
 *  \return     int  (length of the output string, or -1 for failure)
 ***************************************************************************/
int dwpald_hostap_cmd_build(char *out, size_t size,
		const char *cmd, const dwpald_fields_to_cmd params[])
{
	char *outp;
	int  i, len, total = 0;

	if (!out || !size || !cmd) {
		ELOG("%s: invalid parameters", __func__);
		return -1;
	}

	memset(out, 0, size);
	outp = out;
	len = sprintf_s(outp, size, "%s ", cmd);
	if (len < 0) {
		ELOG("%s: sprintf_s failed", __func__);
		return -1;
	}

	outp += len; size -= len; total += len;

	if (params) {
		i = 0;
		while (params[i].type != DWPALD_TYPE_END && params[i].type < DWPALD_TYPE_MAXCOUNT)
		{
			const char *prefix = params[i].prefix ? params[i].prefix : "";
			{
				switch (params[i].type) {
					case DWPALD_TYPE_STR:
						len = sprintf_s(outp, size, "%s%s ", prefix, params[i].field.STR);
						break;

					case DWPALD_TYPE_HEXSTR:
					{
						size_t strlen = strnlen_s(params[i].field.HEXSTR, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH-1);
						size_t hexsize = (strlen * 2) + 1; /* len * two hex digits + zero char */
						char *hex = malloc(hexsize);
						if (!hex) {
							return -1;
						}
						if (!dwpald_str_to_hex(hex, hexsize, params[i].field.HEXSTR)) {
							free(hex);
							return -1;
						}
						len = sprintf_s(outp, size, "%s%s ", prefix, hex);
						free(hex);
						break;
					}

					case DWPALD_TYPE_CHAR:
						len = sprintf_s(outp, size, "%s%c ", prefix, params[i].field.CHAR);
						break;

					case DWPALD_TYPE_INT8:
						len = sprintf_s(outp, size, "%s%d ", prefix, (int)params[i].field.INT8);
						break;

					case DWPALD_TYPE_UINT8:
						len = sprintf_s(outp, size, "%s%u ", prefix, (unsigned int)params[i].field.UINT8);
						break;

					case DWPALD_TYPE_SHORT:
						len = sprintf_s(outp, size, "%s%d ", prefix, (int)params[i].field.SHORT);
						break;

					case DWPALD_TYPE_USHORT:
						len = sprintf_s(outp, size, "%s%u ", prefix, (unsigned int)params[i].field.USHORT);
						break;

					case DWPALD_TYPE_INT:
						len = sprintf_s(outp, size, "%s%d ", prefix, params[i].field.INT);
						break;

					case DWPALD_TYPE_UINT:
						len = sprintf_s(outp, size, "%s%u ", prefix, params[i].field.UINT);
						break;

					case DWPALD_TYPE_INT64:
						len = sprintf_s(outp, size, "%s%lld ", prefix, params[i].field.INT64);
						break;

					case DWPALD_TYPE_UINT64:
						len = sprintf_s(outp, size, "%s%llu ", prefix, params[i].field.UINT64);
						break;

					case DWPALD_TYPE_BOOL:
						len = sprintf_s(outp, size, "%s%u ", prefix, (unsigned)!!params[i].field.BOOL);
						break;

					case DWPALD_TYPE_HEX:
						len = sprintf_s(outp, size, "%s%X ", prefix, params[i].field.HEX);
						break;

					case DWPALD_TYPE_DUMMY:
						len = 0;
						break;

					default:
						ELOG("%s: type(%u) is not supported", __func__, params[i].type);
						return -1;
						break;
				}

				if (len < 0) {
					ELOG("%s: sprintf_s failed", __func__);
					return -1;
				}
				outp += len; size -= len; total += len;
			}

			++i;
		}
	}

	/* Remove last space */
	--outp;
	if (*outp == ' ') *outp = '\0';
	return total;

}

/**************************************************************************/
/*! \fn int dwpald_hostap_cmd_format(char *out, size_t size,
		const char *cmd, const char *format, ...)
 **************************************************************************
 *  \brief      Build hostapd command using format string
 *  \param[out] char* out        - The output buffer
 *  \param[in]  size_t size      - Size of the output buffer in bytes
 *  \param[in]  const char *cmd  - Hostapd command name
 *  \param[in]  const char *format - Format string
 *  \return     int  (length of the output string, or -1 for failure)
 ***************************************************************************/
int dwpald_hostap_cmd_format(char *out, size_t size,
		const char *cmd, const char *format, ...)
{
	va_list args;
	int len, total = 0;

	if (!out || !size || !cmd || !format) {
		ELOG("%s: invalid parameters", __func__);
		return -1;
	}

	memset(out, 0, size);
	len = sprintf_s(out, size, "%s ", cmd);
	if (len < 0) {
		ELOG("%s: sprintf_s failed", __func__);
		return -1;
	}
	out += len; size -= len; total += len;

	va_start( args, format );
	len = vsprintf_s(out, size, format, args);
	va_end( args );
	if (len < 0) {
		ELOG("%s: vsprintf_s failed", __func__);
		return -1;
	}

	total += len;
	return total;
}


static bool isColumnOfFields(char *msg, char *endFieldName[])
{
	int i = 0, numOfFieldsInLine = 0;

	//LOG(2,"%s; line= '%s'", __FUNCTION__, msg);

	if (endFieldName == NULL)
	{
		ELOG("%s; endFieldName= 'NULL' ==> not a column!", __FUNCTION__);
		return false;
	}

	while (strncmp(endFieldName[i], "\n", 1))
	{  /* run over all field names in the string */
		if (strstr(msg, endFieldName[i]) != NULL)
		{
			numOfFieldsInLine++;

			if (numOfFieldsInLine > 1)
			{
				//LOG(2,"%s; Not a column (numOfFieldsInLine= %d) ==> return!", __FUNCTION__, numOfFieldsInLine);
				return false;
			}

			/* Move ahead inside the line, to avoid double recognition (like "PacketsSent" and "DiscardPacketsSent") */
			msg += strnlen_s(endFieldName[i], HOSTAPD_TO_DWPAL_MSG_LENGTH);
		}

		i++;
	}

	//LOG(2,"%s; It is a column (numOfFieldsInLine= %d)", __FUNCTION__, numOfFieldsInLine);

	return true;
}


static bool mandatoryFieldValueGet(char *buf, size_t *bufLen, char **p2str, size_t totalSizeOfArg, char fieldValue[] /*OUT*/)
{
	char *param = strtok_s(buf, bufLen, " ", p2str);

	if (param == NULL)
	{
		ELOG("%s; param is NULL ==> Abort!", __FUNCTION__);
		return false;
	}

	if (fieldValue != NULL)
	{
		if (strcpy_s(fieldValue, totalSizeOfArg, param)) {
			ELOG("%s; param ('%s') length (%zu) is higher than allocated size (%zu) ==> Abort!", __FUNCTION__, param, strnlen_s(param, totalSizeOfArg), totalSizeOfArg-1);
			return false;
		}
	}

	return true;
}


static bool arrayValuesGet(char *stringOfValues, size_t totalSizeOfArg, ParamParsingType paramParsingType, size_t *numOfValidArgs /*OUT*/, void *array /*OUT*/)
{
	/* fill in the output array with list of integer elements (from decimal/hex base), for example:
	   "SupportedRates=2 4 11 22 12 18 24 36 48 72 96 108" or "HT_MCS=FF FF FF 00 00 00 00 00 00 00 C2 01 01 00 00 00"
	   also, in case of "DWPAL_STR_ARRAY_PARAM", handle multiple repetitive field, for example:
	   "... non_pref_chan=81:200:1:5 non_pref_chan=81:100:2:9 non_pref_chan=81:200:1:7 non_pref_chan=81:100:2:5 ..." or
	   "... non_pref_chan=81:200:1:5 81:100:2:9 81:200:1:7 81:100:2:5 ..." */

	size_t  idx = 0;
	char    *p2str, *param, *tokenString;
	rsize_t dmaxLen = strnlen_s(stringOfValues, DWPAL_TO_HOSTAPD_MSG_LENGTH);

	tokenString = stringOfValues;

	switch (paramParsingType) {
		case DWPALD_TYPE_INT_ARRAY:
		case DWPALD_TYPE_HEX_ARRAY:
			totalSizeOfArg /= sizeof(int);
			break;
		case DWPALD_TYPE_STR_ARRAY:
			totalSizeOfArg /= sizeof(dwpald_string);
			break;
		default:
			break;
	}

	do
	{
		param = strtok_s(tokenString, &dmaxLen, " ", &p2str);
		if (param == NULL)
		{
			((int *)array)[idx] = 0;
			break;
		}

		if (idx < totalSizeOfArg)
		{
			if (numOfValidArgs != NULL)
			{
				(*numOfValidArgs)++;
			}

			switch (paramParsingType) {
			case DWPALD_TYPE_HEX_ARRAY:
				((int *)array)[idx] = strtol(param, NULL, 16);
				break;
			case DWPALD_TYPE_INT_ARRAY:
				((int *)array)[idx] = strtol(param, NULL, 0);
				break;
			case DWPALD_TYPE_STR_ARRAY:
				strcpy_s(&(((char *)array)[idx * HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH]), HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH, param);
				break;
			default:
				ELOG("%s; Invalid type  ==> Abort!", __FUNCTION__);
				return false;
				break;
			}
		}

		tokenString = NULL;

		idx++;
	} while (idx < DWPAL_MAX_NUM_OF_ELEMENTS);  /* allow up to 512 elements per field (array) */

	if (idx > totalSizeOfArg)
	{
		ELOG("%s; actual number of arguments (%zu) is bigger/equal then totalSizeOfArg (%zu) ==> Abort!", __FUNCTION__, idx, totalSizeOfArg);
		return false;
	}

	return true;
}


static bool fieldValuesGet(char *buf, size_t bufLen, ParamParsingType parsingType, const char *stringToSearch, char *endFieldName[], char *stringOfValues /*OUT*/)
{
	/* handles list of fields, one by one in the same row, for example: "... btm_supported=1 ..." or
	   "... SupportedRates=2 4 11 22 12 18 24 36 48 72 96 108 ..." */

	char    *stringStart, *stringEnd, *restOfStringStart, *closerStringEnd = NULL;
	char    *localBuf = NULL;
	char    *localStringToSearch = NULL;
	int     i, idx=0, numOfCharacters = 0, numOfCharactersToCopy = 0;
	bool    isFirstEndOfString = true, ret = false;
	char    *tempStringOfValues=NULL, *localEndFieldName=NULL;
	int		status;

	tempStringOfValues = (char *)malloc(HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
	if (tempStringOfValues == NULL)
	{
		ELOG("%s; malloc failed ==> Abort!", __FUNCTION__);
		goto failure;
	}

	localEndFieldName = (char *)malloc(HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
	if (localEndFieldName == NULL)
	{
		ELOG("%s; malloc failed ==> Abort!", __FUNCTION__);
		goto failure;
	}

	localBuf = (char *)malloc(bufLen + 2 /* '\0' & 'blank' */);
	if (localBuf == NULL)
	{
		ELOG("%s; malloc failed ==> Abort!", __FUNCTION__);
		goto failure;
	}

	/* Add ' ' at the beginning of a string - to handle a case in which the buf starts with the
	   value of stringToSearch, like buf= 'candidate=d8:fe:e3:3e:bd:14,2178,83,5,7,255 candidate=...' */
	status = sprintf_s(localBuf, bufLen + 2, " %s", buf);
	if (status <= 0)
		ELOG("%s; sprintf_s failed!", __FUNCTION__);

	/* localStringToSearch set to stringToSearch with addition of " " at the beginning -
	   it is a MUST in order to differentiate between "ssid" and "bssid" */
	localStringToSearch = (char *)malloc(strnlen_s(stringToSearch, DWPAL_FIELD_NAME_LENGTH) + 2 /*'\0' & 'blank' */);
	if (localStringToSearch == NULL)
	{
		ELOG("%s; localStringToSearch is NULL ==> Abort!", __FUNCTION__);
		goto failure;
	}

	status = sprintf_s(localStringToSearch, DWPAL_FIELD_NAME_LENGTH, " %s", stringToSearch);
	if (status <= 0)
		ELOG("%s; sprintf_s failed!", __FUNCTION__);

	restOfStringStart = localBuf;

	while ( (stringStart = strstr(restOfStringStart, localStringToSearch)) != NULL )
	{
		ret = true;  /* mark that at least one fiels was found */

		/* move the string pointer to the beginning of the field's value */
		restOfStringStart = stringStart + strnlen_s(localStringToSearch, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
		//LOG(2,"%s; stringStart= 0x%x, strlen of ('%s')= %d ==> restOfStringStart= 0x%x",
			   //__FUNCTION__, (unsigned int)stringStart, localStringToSearch, strnlen_s(localStringToSearch, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH), (unsigned int)restOfStringStart);

		/* find all beginning of all other fields (and get the closest to the current field) in order to know where the field's value ends */
		i = 0;
		while (strncmp(endFieldName[i], "\n", 1))
		{  /* run over all field names in the string */
			status = sprintf_s(localEndFieldName, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH, " %s", endFieldName[i]);  /* in order to differentiate between VHT_MCS and HT_MCS */
			if (status <= 0)
				ELOG("%s; sprintf_s failed!", __FUNCTION__);
			stringEnd = strstr(restOfStringStart, localEndFieldName);
			if (stringEnd != NULL)
			{
				stringEnd++;  /* move one character ahead due to the ' ' at the beginning of localEndFieldName */
				//LOG(2,"%s; localEndFieldName= '%s' FOUND! (i= %d)", __FUNCTION__, localEndFieldName, i);
				if (isFirstEndOfString)
				{
					isFirstEndOfString = false;
					closerStringEnd = stringEnd;
				}
				else
				{  /* Make sure that closerStringEnd will point to the closest field ahead */
					closerStringEnd = (stringEnd < closerStringEnd)? stringEnd : closerStringEnd;
				}

				//LOG(2,"%s; inside the 'while': closerStringEnd= 0x%x", __FUNCTION__, (unsigned int)closerStringEnd);
			}

			i++;
		}

		//LOG(2,"%s; after the 'while': closerStringEnd= 0x%x", __FUNCTION__, (unsigned int)closerStringEnd);

		if (closerStringEnd == NULL)
		{  /* Meaning, this is the last parameter in the string */
			//LOG(2,"%s; closerStringEnd is NULL; restOfStringStart= '%s'", __FUNCTION__, restOfStringStart);
			closerStringEnd = restOfStringStart + strnlen_s(restOfStringStart, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH) + 1 /* for '\0' */;
			//LOG(2,"%s; closerStringEnd is 'NULL': closerStringEnd= 0x%x", __FUNCTION__, (unsigned int)closerStringEnd);

			//LOG(2,"%s; String end did NOT found ==> set closerStringEnd to the end of buf; closerStringEnd= 0x%x", __FUNCTION__, (unsigned int)closerStringEnd);
		}

		//LOG(2,"%s; stringToSearch= '%s'; restOfStringStart= '%s'; buf= '%s'", __FUNCTION__, stringToSearch, restOfStringStart, buf);
		//LOG(2,"%s; restOfStringStart= 0x%x, closerStringEnd= 0x%x ==> characters to copy = %d", __FUNCTION__, (unsigned int)restOfStringStart, (unsigned int)closerStringEnd, closerStringEnd - restOfStringStart);

		/* set 'numOfCharacters' with the number of characters to copy (including the blank or end-of-string at the end) */
		numOfCharacters = closerStringEnd - restOfStringStart;
		if (numOfCharacters <= 0)
		{
			ELOG("%s; numOfCharacters= %d ==> Abort!", __FUNCTION__, numOfCharacters);
			ret = false;
			goto failure;
		}

		if (numOfCharacters >= (int)HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH)
		{
			ELOG("%s; numOfCharacters (%d) >= HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH (%d) ==> Abort!", __FUNCTION__, numOfCharacters, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
			ret = false;
			goto failure;
		}

		/* Copy the characters of the value, and set the last one to '\0' */
		strncpy_s(tempStringOfValues, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH, restOfStringStart, numOfCharacters);
		tempStringOfValues[numOfCharacters - 1] = '\0';
		//LOG(2,"%s; stringToSearch= '%s'; tempStringOfValues= '%s'", __FUNCTION__, stringToSearch, tempStringOfValues);

		/* Check if all elements are valid; if an element of an array contains "=", it is NOT valid ==> do NOT copy it! */
		for (i=0; i < numOfCharacters; i++)
		{
			if ( (parsingType == DWPALD_TYPE_STR_ARRAY) ||
			     (parsingType == DWPALD_TYPE_INT_ARRAY) ||
				 (parsingType == DWPALD_TYPE_HEX_ARRAY) )
			{
				if ( (tempStringOfValues[i] == '=') || (tempStringOfValues[i] == '\0') )
				{
					numOfCharactersToCopy = i + 1 /* convert index to number-of */;
					break;  /* Only in case of array of values, do NOT allow "=" character */
				}
			}
			else if ( (tempStringOfValues[i] == ' ') || (tempStringOfValues[i] == '\0') )
			{
				numOfCharactersToCopy = i + 1 /* convert index to number-of */;
				break;
			}
		}

		strncpy_s(&stringOfValues[idx], HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH, restOfStringStart, numOfCharactersToCopy);
		idx += numOfCharactersToCopy;
		stringOfValues[idx] = '\0';

		//LOG(2,"%s; stringToSearch= '%s'; stringOfValues= '%s'", __FUNCTION__, stringToSearch, stringOfValues);

		closerStringEnd = NULL;
	}

	/* Remove all ' ' from the end of the string */
	for (i= idx-1; i > 0; i--)
	{
		if (stringOfValues[i] != ' ')
		{
			break;  /* Stop removing the ' ' characters when the first non-blank character was found! */
		}
		stringOfValues[i] = '\0';
	}

	//LOG(2,"%s; stringToSearch= '%s'; stringOfValues= '%s'", __FUNCTION__, stringToSearch, stringOfValues);

failure:
	free(localBuf);
	free(localStringToSearch);
	free(tempStringOfValues);
	free(localEndFieldName);

	//LOG(2,"%s; ret= %d, stringToSearch= '%s'; stringOfValues= '%s'", __FUNCTION__, ret, stringToSearch, stringOfValues);

	return ret;
}

static bool columnOfParamsToRowConvert(char *msg, size_t msgLen, char *endFieldName[])
{
	char    *localMsg = strdup(msg), *lineMsg, *p2str;
	rsize_t dmaxLen = (rsize_t)msgLen;
	bool    isColumn = true;
	size_t  i;

	if (localMsg == NULL)
	{
		ELOG("%s; strdup error ==> Abort!", __FUNCTION__);
		return false;
	}

	lineMsg = strtok_s(localMsg, &dmaxLen, "\n", &p2str);

	while (lineMsg != NULL)
	{
		isColumn = isColumnOfFields(lineMsg, endFieldName);

		if (isColumn == false)
		{
			//console_printf("%s; Not a column ==> break!", __FUNCTION__);
			break;
		}

		lineMsg = strtok_s(NULL, &dmaxLen, "\n", &p2str);
	}

	free (localMsg);

	if (isColumn)
	{
		/* Modify the column string to be in ONE raw  */
		for (i=0; i < msgLen; i++)
		{
			if (msg[i] == '\n')
			{
				msg[i] = ' ';
			}
		}

		msg[msgLen] = '\0';
	}

	return true;
}

static inline bool is_array_field(dwpald_type type)
{
	return (
		type == DWPALD_TYPE_STR_ARRAY ||
		type == DWPALD_TYPE_INT_ARRAY ||
		type == DWPALD_TYPE_HEX_ARRAY
	);
}

static size_t get_field_size(dwpald_fields_to_parse *field)
{
	if (!field || !field->field) {
		return 0;
	}

	switch (field->type)
	{
		case DWPALD_TYPE_DUMMY:
			return 0;

		case DWPALD_TYPE_STR:
		case DWPALD_TYPE_HEXSTR:
			/* array of characters (string) */
			return field->totalSizeOfArg;

		case DWPALD_TYPE_CHAR:
			return sizeof(char);

		case DWPALD_TYPE_INT8:
			return sizeof(int8_t);

		case DWPALD_TYPE_UINT8:
			return sizeof(uint8_t);

		case DWPALD_TYPE_SHORT:
			return sizeof(short);

		case DWPALD_TYPE_USHORT:
			return sizeof(unsigned short);

		case DWPALD_TYPE_INT:
			return sizeof(int);

		case DWPALD_TYPE_UINT:
		case DWPALD_TYPE_HEX:
			return sizeof(unsigned int);

		case DWPALD_TYPE_INT64:
			return sizeof(int64_t);

		case DWPALD_TYPE_UINT64:
			return sizeof(uint64_t);

		case DWPALD_TYPE_BOOL:
			return sizeof(bool);

		case DWPALD_TYPE_STR_ARRAY:
			/* TODO: Check for correctness */
			return field->totalSizeOfArg;

		case DWPALD_TYPE_INT_ARRAY:
		case DWPALD_TYPE_HEX_ARRAY:
			/* Check for correctness */
			return field->totalSizeOfArg;

		default:
			ELOG("Unsupported type: %u", field->type);
			return 0;
	}
}

static bool set_field(void *field, dwpald_fields_to_parse *fieldToParse, const char *stringOfValues)
{
	switch (fieldToParse->type)
	{
		case DWPALD_TYPE_DUMMY:
			/* Simply do nothing */
			return true;

		case DWPALD_TYPE_STR:
			if (0 != strcpy_s(field, fieldToParse->totalSizeOfArg, stringOfValues))
			{
				ELOG("%s; string length (%zu) is bigger the allocated string size (%zu)",
							__FUNCTION__, strnlen_s(stringOfValues, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH) + 1,
							fieldToParse->totalSizeOfArg);
				/* longer string then allocated ==> Abort! */
				return false;
			}
			break;

		case DWPALD_TYPE_HEXSTR:
			if (!dwpald_hex_to_str(field, fieldToParse->totalSizeOfArg, stringOfValues))
			{
				ELOG("%s; hex string to regular string conversion error", __FUNCTION__);
				return false;
			}
			break;

		case DWPALD_TYPE_CHAR:
			*(char *)field = stringOfValues[0];
			break;

		case DWPALD_TYPE_INT8:
			*(int8_t *)field = (int8_t)strtol(stringOfValues, NULL, 0);
			break;

		case DWPALD_TYPE_UINT8:
			*(uint8_t *)field = (uint8_t)strtol(stringOfValues, NULL, 0);
			break;

		case DWPALD_TYPE_SHORT:
			*(short*)field = (short)strtol(stringOfValues, NULL, 0);
			break;

		case DWPALD_TYPE_USHORT:
			*(unsigned short*)field = (unsigned short)strtol(stringOfValues, NULL, 0);
			break;

		case DWPALD_TYPE_INT:
			*(int *)field = strtol(stringOfValues, NULL, 0);
			break;

		case DWPALD_TYPE_UINT:
			*(unsigned int *)field = strtoul(stringOfValues, NULL, 0);
			break;

		case DWPALD_TYPE_INT64:
			*(int64_t*)field = strtoll(stringOfValues, NULL, 0);
			break;

		case DWPALD_TYPE_UINT64:
			*(uint64_t*)field = strtoull(stringOfValues, NULL, 0);
			break;

		case DWPALD_TYPE_HEX:
			*(int *)field = strtol(stringOfValues, NULL, 16);
			break;

		case DWPALD_TYPE_BOOL:
			*(bool*)field = atoi(stringOfValues);
			break;

		case DWPALD_TYPE_STR_ARRAY:
		case DWPALD_TYPE_INT_ARRAY:
		case DWPALD_TYPE_HEX_ARRAY:
			/* not supported here */
			ELOG("Array parameters are not supported here!");
			return false;

		default:
			ELOG("%s; (parsingType= %d) ERROR ==> Abort!", __FUNCTION__, fieldToParse->type);
			return false;
	}

	if (fieldToParse->numOfValidArgs)
	{
		(*fieldToParse->numOfValidArgs)++;
	}
	return true;
}

static void cleanup_all_fields(dwpald_fields_to_parse fields[])
{
	unsigned i;
	size_t size;

	for (i = 0; fields[i].type < DWPALD_TYPE_MAXCOUNT
			 && fields[i].type != DWPALD_TYPE_END; i++)
	{
		/* Cleanup counters */
		if (fields[i].numOfValidArgs) {
			*fields[i].numOfValidArgs = 0;
		}

		/* Cleanup output fields */
		size = get_field_size(&fields[i]);
		if (size) {
			memset(fields[i].field, 0, size);
		}
	}
}

/**************************************************************************/
/*! \fn dwpald_ret dwpald_hostap_response_parse(char *msg, size_t msgLen,
		dwpald_fields_to_parse fieldsToParse[], size_t userBufLen)
 **************************************************************************
 *  \brief Provides parsing services from hostap string to structure
 *  \param[in] char *msg - Provides the string to be parsed
 *  \param[in] size_t msgLen - The string’s length
 *  \param[in] dwpald_fields_to_parse fieldsToParse[] - The information needed for the actual parsing
 *  \param[in] size_t userBufLen - The user total buffer size to hold parsed info from inside fieldsToParse
 *  \return dwpald_ret (DWPALD_SUCCESS for success, other for failure)
 ***************************************************************************/
dwpald_ret dwpald_hostap_response_parse(char *msg, size_t msgLen,
		dwpald_fields_to_parse fieldsToParse[], size_t userBufLen)
{
	dwpald_ret ret = DWPALD_FAILED;
	int       i = 0, idx = 0, numOfNameArrayArgs = 0, lineIdx = 0;
	bool      isEndFieldNameAllocated = false;
	char      *stringOfValues = NULL;
	char      *lineMsg, *localMsg, *p2str = NULL, *p2strMandatory = NULL;
	rsize_t   dmaxLen, dmaxLenMandatory;
	size_t    sizeOfStruct = 0, msgStringLen;
	char      **endFieldName = NULL;
	char      *localMsgDup = NULL;

	//LOG(0, "#### %s() start: %s", __FUNCTION__, msg);

	if (!fieldsToParse)
	{
		ELOG("%s; input params error ==> Abort!", __FUNCTION__);
		goto failure;
	}
	cleanup_all_fields(fieldsToParse);

	if (!msg || !msgLen)
	{
		ELOG("%s; input params error ==> Abort!", __FUNCTION__);
		goto failure;
	}

	if ( (msgStringLen = strnlen_s(msg, HOSTAPD_TO_DWPAL_MSG_LENGTH)) > msgLen )
	{
		ELOG("%s; msgStringLen (%zu) is bigger than msgLen (%zu) ==> Abort!", __FUNCTION__, msgStringLen, msgLen);
		goto failure;
	}

	stringOfValues = (char *)malloc(HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
	if (!stringOfValues)
	{
		ELOG("%s; malloc failed ==> Abort!", __FUNCTION__);
		goto failure;
	}

	/* Check for failure strings */
	if (strncmp(msg, "UNKNOWN", sizeof("UNKNOWN")-1) == 0)
	{
		ELOG("'Unknown command' received ==> Abort!");
		goto failure;
	}
	if (strncmp(msg, "FAIL", sizeof("FAIL")-1) == 0)
	{
		ELOG("'FAIL' received ==> Abort!");
		goto failure;
	}

	/* Convert msgLen to string length format (without the '\0' character) */
	msgLen = dmaxLen = msgStringLen;

	/* Set values for 'numOfNameArrayArgs' and 'sizeOfStruct' */
	for (i = 0; fieldsToParse[i].type < DWPALD_TYPE_MAXCOUNT
			 && fieldsToParse[i].type != DWPALD_TYPE_END; i++)
	{
		if (DWPALD_TYPE_DUMMY == fieldsToParse[i].type)
			continue;

		/* Set numOfNameArrayArgs with the number of endFieldName arguments - needed for the dynamic allocation */
		if (fieldsToParse[i].prefix)
		{
			numOfNameArrayArgs++;
		}

		/* Set sizeOfStruct with the structure size of the output parameter - needed for advancing the output array index (in case of many lines) */
		if (fieldsToParse[i].field)
		{
			size_t field_size = get_field_size(&fieldsToParse[i]);

			/* Verify size */
			switch (fieldsToParse[i].type)
			{
				case DWPALD_TYPE_STR:
				case DWPALD_TYPE_HEXSTR:
					if ( (fieldsToParse[i].field != NULL) && (field_size == 0) )
					{
						ELOG("%s; 'string' parameter must have positive value for totalSizeOfArg ==> Abort!", __FUNCTION__);
						goto failure;
					}
					break;

				case DWPALD_TYPE_STR_ARRAY:
					if ( (fieldsToParse[i].field != NULL) && (field_size == 0) )
					{
						ELOG("%s; 'string' parameter must have positive value for totalSizeOfArg ==> Abort!", __FUNCTION__);
						goto failure;
					}
					if ((field_size % sizeof(dwpald_string)) != 0)
					{
						ELOG("%s; size of 'strings array' parameter must be aligned to %zu characters ==> Abort!", __FUNCTION__, sizeof(dwpald_string));
						goto failure;
					}
					break;

				case DWPALD_TYPE_INT_ARRAY:
				case DWPALD_TYPE_HEX_ARRAY:
					if ( (fieldsToParse[i].field != NULL) && (field_size == 0) )
					{
						ELOG("%s; 'array' parameter must have positive value for totalSizeOfArg ==> Abort!", __FUNCTION__);
						goto failure;
					}

					if ((field_size % sizeof(int)) != 0)
					{
						ELOG("%s; size of 'array' parameter must be aligned to sizeof(int) == Abort!", __FUNCTION__);
						goto failure;
					}
					break;

				default:
					break;
			}

			if (field_size == 0)
			{
				ELOG("%s; (parsingType= %d) ERROR ==> Abort!", __FUNCTION__, fieldsToParse[i].type);
				goto failure;
			}
			sizeOfStruct += field_size;
		}
	}

	/* Allocate and set the value for each endFieldName[] string */
	if (numOfNameArrayArgs > 0)
	{
		numOfNameArrayArgs++;  /* for the last allocated argument */

		endFieldName = calloc(numOfNameArrayArgs, sizeof(*endFieldName));
		if (!endFieldName)
		{
			ELOG("%s; malloc endFieldName failed ==> Abort!", __FUNCTION__);
			goto failure;
		}

		idx = 0;
		for (i = 0; fieldsToParse[i].type < DWPALD_TYPE_MAXCOUNT
				 && fieldsToParse[i].type != DWPALD_TYPE_END
				 && idx < (numOfNameArrayArgs-1); i++)
		{
			if (fieldsToParse[i].prefix)
			{
				/* Just copy pointer to the prefix string into array */
				endFieldName[idx] = (char*)fieldsToParse[i].prefix;
				idx++;
			}
		}

		endFieldName[idx] = "\n";
		isEndFieldNameAllocated = true;
	}

	/* In case of a column, convert it to one raw */
	if ( isEndFieldNameAllocated )
	{
		if (!columnOfParamsToRowConvert(msg, msgLen , endFieldName))
		{
			ELOG("%s; columnOfParamsToRowConvert error ==> Abort!", __FUNCTION__);
			goto failure;
		}
	}


	/* Perform the actual parsing */
	lineMsg = strtok_s(msg, &dmaxLen, "\n", &p2str);
	localMsg = lineMsg;
	lineIdx = 0;
	localMsgDup = NULL;

	while (lineMsg)
	{
		void *field;

		/* Handle mandatory parameters WITHOUT any string-prefix */
		if (localMsg)
		{
			localMsgDup = strdup(localMsg);
			if (!localMsgDup)
			{
				ELOG("%s; localMsgDup is NULL, Failed strdup ==> Abort!", __FUNCTION__);
				goto failure;
			}
			dmaxLenMandatory = strnlen_s(localMsgDup, HOSTAPD_TO_DWPAL_MSG_LENGTH);
			//LOG(2, "dmaxLenMandatory=%u", dmaxLenMandatory);
		}

		for (i = 0; fieldsToParse[i].type < DWPALD_TYPE_MAXCOUNT
				 && fieldsToParse[i].type != DWPALD_TYPE_END; i++)
		{
			if (DWPAL_DUMMY_PARAM == fieldsToParse[i].type)
				continue;

			if (fieldsToParse[i].field == NULL)
			{
				field = NULL;
			}
			else
			{
				if (lineIdx * sizeOfStruct >= userBufLen)
				{
					ELOG("%s; user did not allocate enough buffer for receiving all lines ==> Abort!", __FUNCTION__);
					goto failure;
				}
				/* set the output parameter - move it to the next array index (needed when parsing many lines) */
				field = (void *)((char*)fieldsToParse[i].field + (lineIdx * sizeOfStruct));
				//LOG(2,"%s; lineIdx= %d, sizeOfStruct= %d, field= 0x%x", __FUNCTION__, lineIdx, sizeOfStruct, (unsigned int)field);
			}

			/* reset the string value array */
			memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);

			if (!is_array_field(fieldsToParse[i].type))
			{
				/* Common processing for all fields which can be mandatory */
				if (fieldsToParse[i].prefix == NULL)
				{
					/* Mandatory field: parse parameter always */
					if (mandatoryFieldValueGet(((localMsg != NULL)? localMsgDup : NULL) /*will be NULL starting from 2nd param*/,
											   &dmaxLenMandatory,
											   &p2strMandatory,
											   HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH,
											   stringOfValues /*OUT*/) == false)
					{
						ELOG("%s; mandatory parameter is missing ==> Abort!", __FUNCTION__);
						goto failure;
					}

					/* If necessary, copy the value into the resulting variable */
					if (field)
					{
						if (!set_field(field, &fieldsToParse[i], stringOfValues))
							goto failure;
					}
				}
				else if (field)
				{
					if (!endFieldName)
						goto failure;
					/* Trying to parse named parameter */
					if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].type, fieldsToParse[i].prefix, endFieldName, stringOfValues) == true)
					{
						/* Parameter was found, copy the value into the resulting variable */
						if (!set_field(field, &fieldsToParse[i], stringOfValues))
							goto failure;
					}
				}
			}
			else {
				/* Array fields must have prefix string */
				if (isEndFieldNameAllocated == false)
				{
					ELOG("%s; array type %u; isEndFieldNameAllocated=false ==> Abort!", __FUNCTION__, fieldsToParse[i].type);
					goto failure;
				}

				if (field)
				{
					/* Trying to parse named parameter */
					if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].type, fieldsToParse[i].prefix, endFieldName, stringOfValues) == true)
					{
						if (arrayValuesGet(stringOfValues,
								fieldsToParse[i].totalSizeOfArg,
								fieldsToParse[i].type,
								fieldsToParse[i].numOfValidArgs, (char *)field) == false)
						{
							ELOG("%s; arrayValuesGet ERROR", __FUNCTION__);
							goto failure;
						}
					}
				}
			}

			//LOG(2, "dmaxLenMandatory=%u, p2strMandatory=%s", dmaxLenMandatory, p2strMandatory);
			localMsg = NULL;  /* for 2nd, 3rd, ... parameter */
		}

		free(localMsgDup); localMsgDup = NULL;

		lineMsg = strtok_s(NULL, &dmaxLen, "\n", &p2str);
		lineIdx++;
		localMsg = lineMsg;
	}

	ret = DWPALD_SUCCESS;

failure:
	/* free the allocated string array (if needed) */
	free(localMsgDup);
	free(endFieldName);
	free(stringOfValues);
	//LOG(0, "#### %s() end", __FUNCTION__);
	return ret;
}

#if 0
extern dwpald_ret ___test(void);
dwpald_ret ___test(void)
{
	unsigned i;
	struct {
		int  param0;
		int  param1;
		char param2[8];

		unsigned u_param;
		int64_t  i64_param;
		uint64_t u64_param;
		bool b_Param;
		char hex_str[31];

		int  param_name1;
		char param_name2[32];

		int int_array1[6];
		dwpald_string str_array2[6];
	} data;
	char msg[] = "2000 2111 kuku 124 -5678 0x9ABC 1 41424344"
			" param_name1=567 int_array1=10 11 12 13 14 param_name2=keke str_array2=a1 a2 a3 a4 a5 a6";
	size_t numOfValidArgs[12];

	dwpald_fields_to_parse fields[] =
	{
		DWPALD_PARSE_INT(data.param0, &numOfValidArgs[0], NULL),
		DWPALD_PARSE_INT(data.param1, &numOfValidArgs[1], NULL),
		DWPALD_PARSE_STR(data.param2, &numOfValidArgs[2], NULL),

		DWPALD_PARSE_UINT  (data.u_param,   &numOfValidArgs[3], NULL),
		DWPALD_PARSE_INT64 (data.i64_param, &numOfValidArgs[4], NULL),
		DWPALD_PARSE_UINT64(data.u64_param, &numOfValidArgs[5], NULL),
		DWPALD_PARSE_BOOL  (data.b_Param,   &numOfValidArgs[6], NULL),
		DWPALD_PARSE_HEXSTR(data.hex_str,   &numOfValidArgs[7], NULL),

		DWPALD_PARSE_INT( data.param_name1, &numOfValidArgs[8], "param_name1=" ),
		DWPALD_PARSE_STR( data.param_name2, &numOfValidArgs[9], "param_name2=" ),

		DWPALD_PARSE_INT_ARRAY( data.int_array1, &numOfValidArgs[10], "int_array1=" ),
		DWPALD_PARSE_STR_ARRAY( data.str_array2, &numOfValidArgs[11], "str_array2=" ),
		DWPALD_PARSE_END
	};

	__log_level = 2;
	dwpald_ret ret = dwpald_hostap_response_parse(msg, strlen(msg), fields, sizeof(data));
	LOG(0,"ret=%d", ret);
	LOG(0,"param0=%i,     \t numOfValidArgs[0]=%u", data.param0,		numOfValidArgs[0]);
	LOG(0,"param1=%i,     \t numOfValidArgs[1]=%u", data.param1,		numOfValidArgs[1]);
	LOG(0,"param2=%s,     \t numOfValidArgs[2]=%u", data.param2,		numOfValidArgs[2]);
	LOG(0,"u_param=%u,    \t numOfValidArgs[3]=%u", data.u_param,		numOfValidArgs[3]);
	LOG(0,"i64_param=%lld,\t numOfValidArgs[4]=%u", data.i64_param,		numOfValidArgs[4]);
	LOG(0,"u64_param=%llu (0x%llx),\t numOfValidArgs[5]=%u", data.u64_param, data.u64_param,		numOfValidArgs[5]);
	LOG(0,"b_Param=%u,    \t numOfValidArgs[6]=%u", data.b_Param,		numOfValidArgs[6]);
	LOG(0,"hex_str=%s,    \t numOfValidArgs[7]=%u", data.hex_str, 		numOfValidArgs[7]);
	LOG(0,"param_name1=%i,\t numOfValidArgs[8]=%u", data.param_name1,	numOfValidArgs[8]);
	LOG(0,"param_name2=%s,\t numOfValidArgs[9]=%u", data.param_name2,	numOfValidArgs[9]);

	LOG(0,"int_array1:\t numOfValidArgs[10]=%u", numOfValidArgs[10]);
	for (i=0; i<ARRAY_SIZE(data.int_array1); i++) {
		LOG(0,"int_array1[%i]=%i", i, data.int_array1[i]);
	}
	LOG(0,"str_array2:\t numOfValidArgs[11]=%u", numOfValidArgs[11]);
	for (i=0; i<ARRAY_SIZE(data.str_array2); i++) {
		LOG(0,"str_array2[%i]=%s", i, data.str_array2[i]);
	}
	{
		LOG(0, "=== FORMAT TEST ====");
		const dwpald_fields_to_cmd params[] = {
			DWPALD_PARAM_INT( 1, "int="),
			DWPALD_PARAM_STR( "kuku", NULL ),
			DWPALD_PARAM_HEXSTR(" ABCD ", "hexstr=" ),
			DWPALD_PARAM_END
		};

		char out[1024];
		ret = dwpald_hostap_cmd_build(out, sizeof(out), "COMMAND", params);

		LOG(0,"ret=%d", ret);
		LOG(0,"out=%s", out);
	}
	return ret;
}
#endif

