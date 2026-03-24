/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __DWPALD_HOSTAP_PARSE__H__
#define __DWPALD_HOSTAP_PARSE__H__

#include <stddef.h>
#include <stdbool.h>
#include "dwpald_client.h"

typedef enum dwpald_type
{
	DWPALD_TYPE_END = 0,
	DWPALD_TYPE_STR,
	DWPALD_TYPE_HEXSTR,
	DWPALD_TYPE_CHAR,
	DWPALD_TYPE_INT8,
	DWPALD_TYPE_UINT8,
	DWPALD_TYPE_SHORT,
	DWPALD_TYPE_USHORT,
	DWPALD_TYPE_INT,
	DWPALD_TYPE_UINT,
	DWPALD_TYPE_INT64,
	DWPALD_TYPE_UINT64,
	DWPALD_TYPE_BOOL,
	DWPALD_TYPE_HEX,
	/* Note: the output param for this type MUST be an array of strings with a length of
	   HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH, i.e.
	  "char non_pref_chan[32][HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH];"
	  or
	  "dwpald_string non_pref_chan[32];"
	  */
	DWPALD_TYPE_STR_ARRAY,
	DWPALD_TYPE_INT_ARRAY,
	DWPALD_TYPE_HEX_ARRAY,
	DWPALD_TYPE_DUMMY,

	/* Must be at the end */
	DWPALD_TYPE_MAXCOUNT
} dwpald_type;

#define HOSTAPD_TO_DWPAL_MSG_LENGTH            (4096 * 4)
#define HOSTAPD_TO_DWPAL_SHORT_REPLY_LENGTH    64
#define DWPAL_TO_HOSTAPD_MSG_LENGTH            512
#define DWPALD_HOSTAPD_MSG_LENGTH              512
#define HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH   4096
#define DWPAL_VAP_NAME_STRING_LENGTH           16  /* same as IF_NAMESIZE */
#define DWPAL_OPCODE_STRING_LENGTH             64
#define DWPAL_FIELD_NAME_LENGTH                128
#define DWPAL_MAX_NUM_OF_ELEMENTS              512
#define DWPAL_TO_HOSTAPD_MSG_LENGTH_3K         (3*1024)
#define DWPAL_TO_HOSTAPD_MSG_LENGTH_INTERNAL   (3*1024)

/*************************************************************************/
/* API for parameters building                                           */
/*************************************************************************/

typedef union dwpald_field
{
	void           *field;
	const char     *STR;
	const char     *HEXSTR;
	char            CHAR;
	int8_t          INT8;
	uint8_t         UINT8;
	short           SHORT;
	unsigned short  USHORT;
	int             INT;
	unsigned int    UINT;
	int64_t         INT64;
	uint64_t        UINT64;
	bool            BOOL;
	void*           DUMMY;
	unsigned int    HEX;
} dwpald_field;

typedef struct dwpald_fields_to_cmd
{
	dwpald_field field;          /*!< Output field */
	dwpald_type  type;           /*!< Field's type */
	const char  *prefix;         /*!< String prefix in cmd line */
} dwpald_fields_to_cmd;

/* Definition of parameter in CMD-builder */
#define DWPALD_PARAM(__type, __field, __prefix) \
	{ .field.__type=__field, .type=DWPALD_TYPE_##__type, .prefix=__prefix }

/* Definition of end-of-parameters-list */
#define DWPALD_PARAM_END \
	{ .field.field=NULL, .type=DWPALD_TYPE_END, .prefix=NULL }

#define DWPALD_PARAM_STR(field, prefix)     DWPALD_PARAM(STR, field, prefix)
#define DWPALD_PARAM_HEXSTR(field, prefix)  DWPALD_PARAM(HEXSTR, field, prefix)
#define DWPALD_PARAM_CHAR(field, prefix)    DWPALD_PARAM(CHAR, field, prefix)
#define DWPALD_PARAM_INT8(field, prefix)    DWPALD_PARAM(INT8, field, prefix)
#define DWPALD_PARAM_UINT8(field, prefix)   DWPALD_PARAM(UINT8, field, prefix)
#define DWPALD_PARAM_SHORT(field, prefix)   DWPALD_PARAM(SHORT, field, prefix)
#define DWPALD_PARAM_USHORT(field, prefix)  DWPALD_PARAM(USHORT, field, prefix)
#define DWPALD_PARAM_INT(field, prefix)     DWPALD_PARAM(INT, field, prefix)
#define DWPALD_PARAM_UINT(field, prefix)    DWPALD_PARAM(UINT, field, prefix)
#define DWPALD_PARAM_INT64(field, prefix)   DWPALD_PARAM(INT64, field, prefix)
#define DWPALD_PARAM_UINT64(field, prefix)  DWPALD_PARAM(UINT64, field, prefix)
#define DWPALD_PARAM_BOOL(field, prefix)    DWPALD_PARAM(BOOL, field, prefix)

#define DWPALD_SET_TYPE(__param,__type) \
	do { (__param).type = DWPALD_TYPE_##__type; } while(0)

#define DWPALD_SET_DUMMY(__param) \
	DWPALD_SET_TYPE(__param, DUMMY)

#define DWPALD_SET_PARAM(__param,__type,__value) do { \
	(__param).field.__type = (__value); \
	(__param).type = DWPALD_TYPE_##__type; \
	} while(0)


/**************************************************************************/
/*! \fn int dwpald_hostap_cmd_build(char *out, size_t size,
		const char *cmd, const dwpald_param params[])
 **************************************************************************
 *  \brief      Build hostapd command using fields descriptor params[]
 *  \param[out] char* out        - The output buffer
 *  \param[in]  size_t size      - Size of the output buffer in bytes
 *  \param[in]  const char *cmd  - Hostapd command name
 *  \param[in]  const dwpald_param params[] - The command build information (list of the parameters)
 *  \return     int  (length of the output string, or -1 for failure)
 ***************************************************************************/
extern int dwpald_hostap_cmd_build(char *out, size_t size,
		const char *cmd, const dwpald_fields_to_cmd params[]);


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
extern int dwpald_hostap_cmd_format(char *out, size_t size,
		const char *cmd, const char *format, ...) __attribute__ ((format(printf,4,5)));


/*************************************************************************/
/* API for fields building                                               */
/*************************************************************************/
typedef char dwpald_string[HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH];

typedef struct dwpald_fields_to_parse
{
	void        *field;  /*OUT*/
	size_t      *numOfValidArgs;  /*OUT*/
	dwpald_type  type;
	const char  *prefix;
	size_t       totalSizeOfArg;
} dwpald_fields_to_parse;

/* Compile-time asserts */
#define BUILD_BUG_OR_ZERO(msg,e) (sizeof(struct{ int:-!!(e);}))
#define BUILD_CHECK_SIZE(var, type)  BUILD_BUG_OR_ZERO("Wrong type!", sizeof(var)!=sizeof(type))

/* Macros for definition of scalar-type fields */
#define DWPALD_PARSE_END \
		{ NULL, NULL, DWPALD_TYPE_END, NULL, 0 }
#define DWPALD_PARSE_CHAR(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_CHAR, __prefix,  0 + BUILD_CHECK_SIZE(__field,char) }
#define DWPALD_PARSE_INT8(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_INT8, __prefix,  0 + BUILD_CHECK_SIZE(__field,int8_t) }
#define DWPALD_PARSE_UINT8(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_UINT8, __prefix,  0 + BUILD_CHECK_SIZE(__field,uint8_t) }
#define DWPALD_PARSE_SHORT(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_SHORT, __prefix,  0 + BUILD_CHECK_SIZE(__field,short) }
#define DWPALD_PARSE_USHORT(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_USHORT, __prefix,  0 + BUILD_CHECK_SIZE(__field,unsigned short) }
#define DWPALD_PARSE_INT(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_INT, __prefix,  0 + BUILD_CHECK_SIZE(__field,int) }
#define DWPALD_PARSE_UINT(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_UINT, __prefix,  0 + BUILD_CHECK_SIZE(__field,unsigned int) }
#define DWPALD_PARSE_INT64(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_INT64, __prefix,  0 + BUILD_CHECK_SIZE(__field,int64_t) }
#define DWPALD_PARSE_UINT64(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_UINT64, __prefix,  0 + BUILD_CHECK_SIZE(__field,uint64_t) }
#define DWPALD_PARSE_BOOL(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_BOOL, __prefix,  0 + BUILD_CHECK_SIZE(__field,bool) }
#define DWPALD_PARSE_HEX(__field, __numOfValidArgs, __prefix) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_HEX, __prefix,  0 + BUILD_CHECK_SIZE(__field,unsigned int) }

/* Macros for definition of array type fields */
#define DWPALD_PARSE_STR_SIZE(__field, __numOfValidArgs, __prefix, __size) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_STR, __prefix, __size }
#define DWPALD_PARSE_HEXSTR_SIZE(__field, __numOfValidArgs, __prefix, __size) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_HEXSTR, __prefix, __size }
#define DWPALD_PARSE_STR_ARRAY_SIZE(__field, __numOfValidArgs, __prefix, __size) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_STR_ARRAY, __prefix, (__size) + BUILD_CHECK_SIZE(__field[0],dwpald_string) }
#define DWPALD_PARSE_INT_ARRAY_SIZE(__field, __numOfValidArgs, __prefix, __size) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_INT_ARRAY, __prefix, (__size) + BUILD_CHECK_SIZE(__field[0],int) }
#define DWPALD_PARSE_HEX_ARRAY_SIZE(__field, __numOfValidArgs, __prefix, __size) \
		{ &__field,  __numOfValidArgs, DWPALD_TYPE_HEX_ARRAY, __prefix, (__size) + BUILD_CHECK_SIZE(__field[0],unsigned) }

#define DWPALD_PARSE_STR(__field, __numOfValidArgs, __prefix) \
		DWPALD_PARSE_STR_SIZE(__field, __numOfValidArgs, __prefix, sizeof(__field))
#define DWPALD_PARSE_HEXSTR(__field, __numOfValidArgs, __prefix) \
		DWPALD_PARSE_HEXSTR_SIZE(__field, __numOfValidArgs, __prefix, sizeof(__field))
#define DWPALD_PARSE_STR_ARRAY(__field, __numOfValidArgs, __prefix) \
		DWPALD_PARSE_STR_ARRAY_SIZE(__field, __numOfValidArgs, __prefix, sizeof(__field))
#define DWPALD_PARSE_INT_ARRAY(__field, __numOfValidArgs, __prefix) \
		DWPALD_PARSE_INT_ARRAY_SIZE(__field, __numOfValidArgs, __prefix, sizeof(__field))
#define DWPALD_PARSE_HEX_ARRAY(__field, __numOfValidArgs, __prefix) \
		DWPALD_PARSE_HEX_ARRAY_SIZE(__field, __numOfValidArgs, __prefix, sizeof(__field))

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
extern dwpald_ret dwpald_hostap_response_parse(char *msg, size_t msgLen,
		dwpald_fields_to_parse fieldsToParse[], size_t userBufLen);

/* Definitions for compatibility with DWPAL API */
typedef dwpald_fields_to_parse FieldsToParse;
typedef dwpald_type            ParamParsingType;

/* Result codes */
#define DWPAL_Ret		dwpald_ret
#define DWPAL_FAILURE	DWPALD_FAILED
#define DWPAL_SUCCESS	DWPALD_SUCCESS

#define DWPAL_STR_PARAM                       DWPALD_TYPE_STR
/* Note: the output param for this type MUST be an array of strings with a length of HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH, i.e. "char non_pref_chan[32][HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH];" */
#define DWPAL_STR_ARRAY_PARAM                 DWPALD_TYPE_STR_ARRAY
#define DWPAL_CHAR_PARAM                      DWPALD_TYPE_INT8
#define DWPAL_UNSIGNED_CHAR_PARAM             DWPALD_TYPE_UINT8
#define DWPAL_SHORT_INT_PARAM                 DWPALD_TYPE_SHORT
#define DWPAL_INT_PARAM                       DWPALD_TYPE_INT
#define DWPAL_UNSIGNED_INT_PARAM              DWPALD_TYPE_UINT
#define DWPAL_LONG_LONG_INT_PARAM             DWPALD_TYPE_INT64
#define DWPAL_UNSIGNED_LONG_LONG_INT_PARAM    DWPALD_TYPE_UINT64
#define DWPAL_INT_ARRAY_PARAM                 DWPALD_TYPE_INT_ARRAY
#define DWPAL_INT_HEX_PARAM                   DWPALD_TYPE_HEX
#define DWPAL_INT_HEX_ARRAY_PARAM             DWPALD_TYPE_HEX_ARRAY
#define DWPAL_BOOL_PARAM                      DWPALD_TYPE_BOOL
#define DWPAL_DUMMY_PARAM                     DWPALD_TYPE_DUMMY
#define DWPAL_NUM_OF_PARSING_TYPES            DWPALD_TYPE_MAXCOUNT

#define dwpal_string_to_struct_parse(msg, msg_len, fields_to_parse, userbuf_len) \
		dwpald_hostap_response_parse(msg, msg_len, fields_to_parse, userbuf_len)


/*************************************************************************/
/* Utility definitions functions                                         */
/*************************************************************************/

/**************************************************************************/
/*! \fn bool dwpald_str_to_hex(char *hex_str, size_t size, const char *str)
 **************************************************************************
 *  \brief      Convert regular string to the hex string
 *  \param[out] char* out_hex_str - The output buffer
 *  \param[in]  size_t size       - Size of the output buffer in bytes
 *  \param[in]  const char *str   - Source string
 *  \return     bool (true if successful, false if failure)
 ***************************************************************************/
bool dwpald_str_to_hex(char *out_hex_str, size_t size, const char *str);


/**************************************************************************/
/*! \fn bool dwpald_hex_to_str(char *str, size_t size, const char *hex_str);
 **************************************************************************
 *  \brief      Convert hex string to the regular string
 *  \param[out] char* out_str       - The output buffer
 *  \param[in]  size_t size         - Size of the output buffer in bytes
 *  \param[in]  const char *hex_str - Source string
 *  \return     bool (true if successful, false if failure)
 ***************************************************************************/
bool dwpald_hex_to_str(char *out_str, size_t size, const char *hex_str);

// extern dwpald_ret ___test(void);

/********************************************************/
/* EXAMPLE
 ********************************************************/

/*
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

*/

#endif /* __DWPALD_HOSTAP_PARSE__H__ */
