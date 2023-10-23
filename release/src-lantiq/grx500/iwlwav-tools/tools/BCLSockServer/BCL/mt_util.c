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
*	MT_UTIL.C
*
*	BCL's utility functions for string and numbers manipulation
*	Author: Ron Zilberman
*
*****************************************************************************/

/*****************************************************************************
*
*
*
*
*
*
*****************************************************************************/
#include "mtlkinc.h"
#include "mt_util.h"
#include "mt_bcl.h"
#include <stdio.h>
#include <stdarg.h>
typedef enum 
{
	NUMBER_TYPE_UNKNOWN,
	NUMBER_TYPE_DEC,
	NUMBER_TYPE_HEX
} eNUMBER_TYPE;


/*****************************************************************************
*
*       Local Function Prototypes
*
*****************************************************************************/
char g_PrintLevel = 10;


/**********************************************************
* Function Name: checkNumber
* Description:  Checks whether the number specified in pAscii is HEX or DEC
* Called From:
*	asciiToUnsigned
* Author:
*	Avri Golan
* Input Parameters:
*	char *pAscii		-	A string which represents a hex or dec number.
* Output Parameters:
*	eNUMBER_TYPE *pType	-	Will hold either NUMBER_TYPE_DEC or NUMBER_TYPE_HEX 
*							according to the number's type.
* Return Value:
*	unsigned int	- The length of the string representing the number.
* Algorithm:
*	Checks if the first character is "h" or "H".  If it is, its a hex number.
* Global Variables Used:
* Revisions:
**********************************************************/
unsigned int checkNumber(char * pAscii,eNUMBER_TYPE * pType, char delim)
{
	char byte,length = 0;

	if (*pAscii == 'H' || *pAscii == 'h')
	{
		*pType = NUMBER_TYPE_HEX;
		pAscii++; /* move next to begining of the number */
		byte = *pAscii++;
		while ((byte >= '0' && byte <= '9') || (byte >= 'a' && byte <= 'f') || (byte >= 'A' && byte <= 'F'))
		{
			byte = *pAscii++;
			length++;
		} 
	}
	else
	{
		*pType = NUMBER_TYPE_DEC;
		byte = *pAscii++;
		while (byte >= '0' && byte <= '9')
		{
			length++;
			byte = *pAscii++;
		} 
	}
	if (byte == delim || byte == 0) 
		return length;
	*pType = NUMBER_TYPE_UNKNOWN;
	return 0;
}


/**********************************************************
* Function Name: asciiToUnsigned
* Description: Convert a string represented number to an unsigned 32 bit number
* Called From:
* Author:
*	Avri Golan, Ron Zilberman
* Input Parameters:
*	char *pAscii	-	A string which represents a hex or dec number.
* Output Parameters:
*	int *error_occured	-	A pointer to an int.  If a conversion error should occure, 
*							the error_occured parameter will be MT_TRUE.  If all goes well,
*							it will be MT_FALSE.
* Return Value:
*	unsigned int	- The numeric value represented by the string.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
unsigned int asciiToUnsigned(char * pAscii, char delim, int *error_occured)
{
    char  ucDigit;
	unsigned int unValue = 0;
	eNUMBER_TYPE type;
	*error_occured = MT_FALSE;

	if (!pAscii)
	{
		*error_occured = MT_TRUE;
		return 0;
	}

	strUpr(pAscii, delim);  /*This function assumes that the number is in UPPERCASE. */

	checkNumber(pAscii,&type,' ');
	if (type == NUMBER_TYPE_UNKNOWN)
	{
		*error_occured = MT_TRUE;
		return 0;
	}
	else if (type == NUMBER_TYPE_HEX)
		pAscii++;
	do
    {
        /* specify the multiply according to the format */
        switch (type)        
        {
            case NUMBER_TYPE_DEC:
                unValue = (unValue << 3) + (unValue << 1);
            break;    
            case NUMBER_TYPE_HEX:
				if (unValue<<4 < unValue)  /*if an overflow has occured... */
				{
					*error_occured = MT_TRUE;
					return 0;
				}
                unValue <<= 4;
            break;
			default:
				/* Do nothing, but this is needed to satisfy gcc. */
            break;
        }

        /* Read character from buffer */
        ucDigit = *(pAscii++);
        
        /* Convert nibble to numeric value */
        if ( ucDigit >= '0' && ucDigit <= '9' )
            ucDigit -= '0'; /* '0' -> 0, '9' -> 9 */
        else if (ucDigit  >= 'A' &&ucDigit <= 'F')
            ucDigit -= 'A' - 10; /* 'A' -> 10 , 'F' -> 15 */
                    
        /* Add to the result */
        if (unValue > unValue+ucDigit)  /*if an overflow has occured... */
		{
			*error_occured = MT_TRUE;
			return 0;
		}
		unValue += ucDigit;
    }
    while(*pAscii && *pAscii != delim);
	return unValue;
}


/**********************************************************
* Function Name: asciiToSigned
* Description: Convert a string represented number to a signed 32 bit number
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *pAscii	-	A string which represents a hex or dec number.
* Output Parameters:
* Return Value:
*	signed int	- The numeric value represented by the string.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
signed int asciiToSigned(char * pAscii, int *error_occured)
{
    signed char sign = 1;
	int numConvErr;

	if (!pAscii)
		return 0;

	if (*pAscii == '-')
	{
		sign = -1;
		pAscii++;
	}
	else if (*pAscii == '+')
		pAscii++;

	return sign * asciiToUnsigned(pAscii,' ', &numConvErr);
	*error_occured = numConvErr;
}


/**********************************************************
* Function Name: unsignedToAsciiHex
* Description: Converts an unsigned number to its hex string representation.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	unsigned int number	-	The number to represent in hex.
* Output Parameters:
*	char *dest	-	The buffer to hold the hex representation of the number.
* Return Value:
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
int unsignedToAsciiHex(unsigned int number, char *dest)
{
	unsigned int currDigit=1;
	unsigned int num=0;  
	unsigned int countDigits=0;
	num--;/*This will necessarily bring us the largest unsigned int */

	dest[0] = 'h';  /*the hex prefix */
	
	/*First, count how many digits we need to represent the number in hex... */
	while (num > 0)
	{
		num /= 16;
		countDigits++;
	}

	/*Now, start finding the hex representation, going from right to left... */
	while (countDigits-currDigit+1 > 0)
	{
		char digit = number % 16;
		if (digit < 10)
			digit += '0';
		else
			digit += 'a'-10;  /*Take care of hex representation */
		dest[countDigits-currDigit+1] = digit;
		number /= 16;
		currDigit++;
	}
	dest[countDigits+1] = '\0';  /*null termination for the string. */
    return countDigits+1; /* +1 for the h */
}

/**********************************************************
* Function Name: unsignedToAscii
* Description: Converts an unsigned number to its decimal string representation.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	unsigned int number	-	The number to convert to its decimal string 
*							representation.
* Output Parameters:
*	char *dest	-	The buffer to hold the string representation of the number.
* Return Value:
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
int unsignedToAscii(unsigned int number, char *dest)
{
	int currDigit=1;
	unsigned int num=number;
	int countDigits=0;
	
	/*Calculate how many digits we need for the ascii string... */
	do
	{
		num /= 10;
		countDigits++;
	} while (num > 0);

	/*Now fill the string going from right to left... */
	do
	{
		dest[countDigits-currDigit] = '0'+ (number % 10);
		number /= 10;
		currDigit++;
	} while (number > 0);

	dest[countDigits] = '\0';
    return countDigits;
}

/**********************************************************
* Function Name: signedToAscii
* Description: Converts a signed number to its decimal string representation.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	signed int number	-	The number to convert to its decimal string 
*							representation.
* Output Parameters:
*	char *dest	-	The buffer to hold the string representation of the number.
* Return Value:
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
int signedToAscii(signed int number, char *dest)
{
	if (number < 0)
	{
		dest[0] = '-';
		return 1 + unsignedToAscii((unsigned int)(-number), &dest[1]);
	}
	else
		return unsignedToAscii((unsigned int)number, dest);
}

/**********************************************************
* Function Name: toUpper
* Description:	Converts a char to its uppercase form (i.e. a..z will turn into 
*				A..Z)
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char ch	-	The character to convert to uppercase.
* Output Parameters:
* Return Value:
*	char	- The uppercase form of the input char.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
char toUpper(char ch)
{
	if ((ch >= 'a') && (ch <= 'z'))
		ch += 'A'-'a';
	return ch;
}

/**********************************************************
* Function Name: strUpr
* Description:	Converts an entire string to its uppercase form.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *str		-	The string to convert to uppercase.
*	char delimiter	-	A character to use as a delimiter.  Once this character
*						is encountered, conversion will stop.
* Output Parameters:
*	char *str	- The input string will be overwritten with its uppercase form.
* Return Value:
*	char *	- A pointer to the uppercased string.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
char *strUpr(char *str, char delimiter)
{
	unsigned int i;
	for (i=0; str[i] && (str[i] != delimiter); i++)
		str[i] = toUpper(str[i]);
	return str;
}


/**********************************************************
* Function Name: skipDelimiters
* Description:	Skips the delimiters at the beginning of the input string, and
*				return a pointer to the first encountered character which isn't
*				a delimiter.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *str	-	The input string to skip spaces on.
*	char delim	-	The delimiter char.
* Output Parameters:
* Return Value:
*	char *	- A pointer to the first character in the string which isn't a delimiter.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
char *skipDelimiters(char *str, char delim)
{
	int i;
	for (i=0; str[i] == delim; i++);
	return &str[i];
}


/**********************************************************
* Function Name: skipWord
* Description:	Skips the first word in the string, until a space is 
*				encountered.  Return a pointer to it.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *str		-	The input string, on which we'll skip the first word.
* Output Parameters:
* Return Value:
*	char *	- A pointer to the first character in the string which is space.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
char *skipWord(char *str, char delim)
{
	int i;
	for (i=0; str[i] && (str[i] != delim); i++);
	return &str[i];
}


/**********************************************************
* Function Name: wordsCount
* Description:	Calculate the number of space-seperated words in the string.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *str		-	The input string, to count words in.
* Output Parameters:
* Return Value:
*	int	-	The number of space-seperated words in the string.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
int wordsCount(char *str, char delim)
{
	int count=0;
	str = skipDelimiters(str,delim);
	
	/*Skip the first word, then skip the spaces trailing it.  Do this until no  */
	/*words are left in the string... */
	while ( str[0] != '\0')
	{
		str = skipWord(str,delim);	/*Skip the word pointed by str's current position */
		str = skipDelimiters(str,delim);	/*Skip the spaces trailing the skipped word */
		count++;
	}
	return count;
}


/**********************************************************
* Function Name: compareFirstWordOnly
* Description:	Compares between the first word in the first input string, and
*				the first word in the second input string.  A "word" here is a
*				space delimited string (or any other defined delimiter).
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *word1		-	The first string to compare.  Only its first word will be 
*						compared.
*	char *word2		-	The second string to compare.  Only its first word will be 
*						compared.
*	char delimiter	-	The char to use as a word delimiter.
* Output Parameters:
* Return Value:
*	int	-	Contains 0 if the strings are the same.  
*			A positive number if word1 > word2 lexicographically.
*			A negative number if word1 < word2 lexicographically.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
int compareFirstWordOnly(char *word1, char *word2, char delimiter)
{
	int i;
	if (!word1 && !word2)
		return 0;
	if (!word1)
		return -1;
	if (!word2)
		return 1;
	/*Advance while none of the words has terminated (either because of a  */
	/*delimiter or a NULL encounter) and while both words are the equal... */
	for (	i=0; 
			word1[i] &&	word2[i] && (word1[i] != delimiter) && 
				(word2[i] != delimiter) && !(word1[i]-word2[i]); 
			++i)
		{};
	while ( (word1[i] == delimiter || word1[i] == '\0') && (word2[i] == delimiter || word2[i] == '\0') )
		i--;
	return word1[i]-word2[i];
}

/**********************************************************
* Function Name: strCpy
* Description:	Copies a string to another.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *source	-	The source string, which we'll copy from.
* Output Parameters:
*	char *dest		-	The string to copy to.
* Return Value:
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
int strCpy(char *dest, const char *source)
{
	int counter = 0;
    if (!source)
		return 0;
	while (*source)
    {
		*dest++ = *source++;
        counter++;
    }
	*dest = '\0';
    return counter;
}

/**********************************************************
* Function Name: strLen
* Description:	Returns the length in bytes of the string, not including the
*				ending '\0'
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *str	-	The source string to measure its length.
* Output Parameters:
* Return Value:
*	unsigned int	- The length of the input string.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
unsigned int strLen(const char *str)
{
	int i=0;
	if (!str)
		return 0;
	while (*str++) i++;
	return i;
}



/**********************************************************
* Function Name: error
* Description:	Sets the MT_BCL_error_occured flag to MT_TRUE, and copies the 
*				error message to the MT_BCL_cmd_line buffer.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *msg	-	The error message to copy to the MT_BCL_cmd_line buffer.
* Output Parameters:
*	char *MT_BCL_cmd_line	-	This global variable is updated with the error 
*								message.
* Return Value:
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/

extern MT_UBYTE MT_BCL_error_occured;
extern char *MT_BCL_cmd_line;

void error(char *msg)
{
	MT_BCL_error_occured = MT_TRUE;
    BCL_SkipReturnValue();
    mt_print(0,"Error: %s\n",msg);
	strCpy(MT_BCL_cmd_line, msg);
}

void mt_print(char level,char *fmt, ...)
{
	if (level <= g_PrintLevel)
	{
		va_list args_list;
		va_start(args_list, fmt);
		vprintf(fmt,args_list);
		va_end(args_list);
	}
}

MT_UINT32 PrintLevel(char level)
{
	g_PrintLevel = level;
	return 1;

}

void strip_unsafe_chars(char *dst, const char *src, size_t size)
{
	if (!size)
		return;

	while (--size) {
		if ((*src == '\0') || (*src == ';') || (*src == '\x0D') || (*src == '\x0A'))
			break;

		*dst++ = *src++;
	}

	*dst = '\0';
}
