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
*	mt_bcl_funcs.c
*
*	BCL's main functions for memory access.
*	Author: Avri Golan
*
*****************************************************************************/

#include "mtlkinc.h"
#include "mt_bcl_defs.h"
#include "mt_bcl.h"
#include "mt_util.h"
#include "mt_bcl_funcs.h"
#define GET_ITEMLIST(VAR) if ((long)VAR >=0 && (long)VAR <=8) VAR = (char *)MT_BCL_item_list[(long)VAR];

extern MT_UBYTE MT_BCL_error_occured;
extern char *MT_BCL_cmd_line;

/* current offset in the itemlist */
static MT_UBYTE * IL_cur_ptr = (MT_UBYTE *)MT_BCL_item_list[0];


MT_UINT32 itemlist(MT_UBYTE item)
{
    if (item >= MT_BCL_ITEM_LISTS_NUM)
    {
        error("Illegal value for Itemlist.");
        return 0;
    }
    IL_cur_ptr = (MT_UBYTE *)MT_BCL_item_list[item];
    return item;
}

MT_UINT32 pitemlist(MT_UBYTE item)
{
    if (item >= MT_BCL_ITEM_LISTS_NUM)
    {
        error("Illegal value for Itemlist.");
        return 0;
    }
    return (MT_UINT32)(MT_BCL_item_list[item]);
}


MT_UINT32 item(MT_UBYTE size)
{
    char *params;
    MT_UINT32 i, numOfParams ;

    params = skipDelimiters(skipWord(MT_BCL_cmd_line,' '),' ');	/*params will now point to the first non-space  */
	            												/*character right after the DMEM_read command. */
	/* get the number of parameters */
    numOfParams = wordsCount(params,' ');
	
    if (numOfParams > 0)
    {
        /* write */
        if (setupFuncArgs(MT_BCL_CurrentCommand,params,1) != MT_TRUE)
            return MT_FALSE;
        for (i = 0; i < numOfParams; i++)
        {
            if (size == 1)
                *((MT_UBYTE *)IL_cur_ptr) = (MT_UBYTE)MT_BCL_arg[i];
            else if (size == 2)
                *((MT_UINT16 *)IL_cur_ptr) = (MT_UINT16)MT_BCL_arg[i];
            else if (size == 4)
                *((MT_UINT32 *)IL_cur_ptr) = (MT_UINT32)MT_BCL_arg[i];
            IL_cur_ptr+=size;
        }
        MT_BCL_cmd_line[0] = 0X0;
    }
    else
    {
        /* read */
        if (size == 1)
            unsignedToAsciiHex(*((MT_UBYTE *)IL_cur_ptr), MT_BCL_cmd_line);
        else if (size == 2)
        {
            if (((MT_UINT32)IL_cur_ptr & 0x1) == 0)
                unsignedToAsciiHex(*((MT_UINT16 *)IL_cur_ptr), MT_BCL_cmd_line);
            else
            {
                error("Pointer is not 16 bit aligned");
                return 1;
            }
        }
        else if (size == 4)
        {
            if (((MT_UINT32)IL_cur_ptr & 0x3) == 0)
                unsignedToAsciiHex(*((MT_UINT32 *)IL_cur_ptr), MT_BCL_cmd_line);
            else
            {
                error("Pointer is not 32 bit aligned");
                return 1;
            }
         
        }
        IL_cur_ptr+=size;
        MT_BCL_cmd_line[strLen(MT_BCL_cmd_line)+1] = 0x0;
    }
    return 0;

}

MT_UINT32 dmem(MT_UBYTE mem_size, void* address, MT_UBYTE count)
{
    MT_UINT32 i;
    MT_UINT32 pos = 0;
    MT_UINT32 * ptr32  = (MT_UINT32 *) address;
    MT_UINT16 * ptr16  = (MT_UINT16 *) address;
    MT_UBYTE  * ptr8   = (MT_UBYTE *)  address;

    if (((mem_size != 1) && (mem_size!=2) && (mem_size !=4)) || address == 0 || count == 0)
    {
        error("dreg8 - wrong arguments");
        return 0;
    }
    
    /* tell BCL not to handle the return value. */
    BCL_SkipReturnValue();


    for (i = 0; i < count; i++)
    {
        if (mem_size == 1)
            unsignedToAsciiHex(ptr8[i], MT_BCL_cmd_line+pos);    /*return signed hexdecimal */
        else if (mem_size == 2)
            unsignedToAsciiHex(ptr16[i], MT_BCL_cmd_line+pos);    /*return signed hexdecimal */
        else if (mem_size == 4)
            unsignedToAsciiHex(ptr32[i], MT_BCL_cmd_line+pos);    /*return signed hexdecimal */
        
        pos += strLen(&MT_BCL_cmd_line[pos]);	/*Update the position on the output buffer MT_BCL_cmd_line. */
        MT_BCL_cmd_line[pos++] = ' ';	/*Plant the seperating space. */
	}

    MT_BCL_cmd_line[pos--] = 0x0; /* end of string */
    return 0;
}

MT_UINT32 ditem (MT_UBYTE mem_size,MT_UINT32 address, MT_UBYTE offset, MT_UBYTE count)
{
    return dmem(mem_size, (void*)(address+offset),count);
}


MT_UINT32 ReadStr(MT_UBYTE * pAddress)
{
	MT_UINT32 itemList = (MT_UINT32)pAddress;
    // if given a number between 0 to itemlist count, use it as item list.
    if (itemList < MT_BCL_ITEM_LISTS_NUM)
	{
		pAddress = (MT_UBYTE *)MT_BCL_item_list[itemList];
	}

    BCL_SkipReturnValue();
    /* copy from the address to the return buffer */
    *MT_BCL_cmd_line = '_'; // add '_' as a prefix to avoid BBS to identify it as a number
    strCpy(MT_BCL_cmd_line+1,(char*) pAddress);
    return 0;
}

MT_UINT32 WriteStr(void)
{
    char * params;
    char * Copy2address;
    MT_UINT32 numOfParams;
    int numConvErr ;
    
    params = skipDelimiters(skipWord(MT_BCL_cmd_line,' '),' ');	/*params will now point to the first non-space  */
    /*character right after the DMEM_read command. */
    /* get the number of parameters */
    numOfParams = wordsCount(params,' ');
    
    /* we should get at least two params: address and string */
    if (numOfParams < 2)
    {
        MT_BCL_error_occured = MT_TRUE;
        strCpy(MT_BCL_cmd_line, "Wrong usage ");
        return 1;
    }
    
    Copy2address = (char *)asciiToUnsigned(params, ' ', &numConvErr);
    
    if (numConvErr || Copy2address == 0)
    {
        MT_BCL_error_occured = MT_TRUE;
        strCpy(MT_BCL_cmd_line, "Invalid value entered for argument ");
        return 1;
    }
    GET_ITEMLIST(Copy2address);
    params = skipWord(params, ' ');
    params = skipDelimiters(params, ' ');
    strCpy(Copy2address,params);
    return 0;
    
}

/*************** LEGACY (SCL) commands support *********************/

MT_UINT32 item32()
{
    return item(4);
}

MT_UINT32 item16()
{
    return item(2);
}

MT_UINT32  item8()
{
    return item(1);
}



MT_UINT32  dreg32(MT_UBYTE space, void* address, MT_UBYTE count)
{
    if (space != 3)
    {
        error("space must be 3");
        return MT_FALSE;
    }
    
    return dmem(4,address,count);
}

MT_UINT32  dreg16(MT_UBYTE space, void* address, MT_UBYTE count)
{
 
    if (space != 3)
    {
        error("space must be 3");
        return 0;
    }
    
    return dmem(2,address,count);
}

MT_UINT32  dreg8(MT_UBYTE space, void* address, MT_UBYTE count)
{
    if (space != 3)
    {
        error("space must be 3");
        return 0;
    }
    
    return dmem(1,address,count);
}

extern int BCL_nWantToExit;

MT_UINT32 bcl_exit()
{
    BCL_nWantToExit = 1;
    return 1;
}

