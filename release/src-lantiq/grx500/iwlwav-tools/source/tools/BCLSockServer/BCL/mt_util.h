/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*****************************************************************************
*
* MT_UTIL.H
*
* BCL's utility functions for string and numbers manipulation
*
*****************************************************************************/
#include "mt_types.h"
#include "mt_bcl_defs.h"

/*****************************************************************************
*
*
*
*
*
*
*****************************************************************************/

/*****************************************************************************
*
*        Global Definitions, Data and Function Prototypes (of this module)
*
*****************************************************************************/


/*****************************************************************************
*
*       Local Function Prototypes
*
*****************************************************************************/
char *getLine(char *buff, int buff_len);
unsigned int asciiToUnsigned(char * pAscii, char delim, int *error_occured);
signed int asciiToSigned(char * pAscii, int *numConvErr);
char *skipDelimiters(char *str, char delim);
char *skipWord(char *str, char delim);
int wordsCount(char *str, char delim);
int compareFirstWordOnly(char *word1, char* word2, char delimiter);
int unsignedToAsciiHex(unsigned int number, char *dest);
int signedToAscii(signed int number, char *dest);
int unsignedToAscii(unsigned int number, char *dest);
char *strUpr(char *str, char delimiter);
int strCpy(char *dest, const char *source);
unsigned int strLen(const char *str);

MT_UINT32 showHelp(void);
void error(char *msg);
void mt_print(char level,char *fmt, ...);
MT_UINT32 PrintLevel(char level);
void strip_unsafe_chars(char *dst, const char *src, size_t size);
