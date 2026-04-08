/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*****************************************************************************
*	MT_BCL.C
*
*	BCL's main functions for setting up and executing BCL commands.
*	Author: Ron Zilberman
*
*****************************************************************************/
#include "mtlkinc.h"
#include "mt_bcl.h"
#include "mt_bcl_wrap.h"
#include "mt_util.h"

MT_UINT32 MT_BCL_item_list[MT_BCL_ITEM_LISTS_NUM][MT_BCL_ITEM_LIST_SIZE] = {{0}}; /*The item_list pointers. */
MT_UINT32 MT_BCL_arg[MAX_PARAM_COUNT] = {0};										/*Contains the arguments for the BCL function to execute. */
MT_UBYTE MT_BCL_error_occured = 0;									/*A boolean flag to indicate whether an error has occured. */
MT_UINT32 MT_BCL_CurrentCommand = 0;
static MT_UBYTE skipReturnValue = 0; /* if not zero, the bcl will not handle the return value from the function */

char *MT_BCL_cmd_line;						/*User's command line input pointer */

void BCL_SkipReturnValue()
{
    skipReturnValue = 1;
}

/**********************************************************
* Function Name: setupFuncArgs
* Description:	Sets up the parameters given by the user in the MT_BCL_arg 
*				array.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	int cmd_index	-	The index of the BCL command to execute, in 
*						MT_BCL_function_table.
*	char *params	-	A string which holds the parameters to the BCL 
*						function.  The parameter are space delimited.
* Output Parameters:
* Return Value:
*	unsigned int	- MT_TRUE if successful.  MT_FALSE if an error has occured.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
unsigned int setupFuncArgs(int cmd_index, char *params,char checkArgCount)
{
    int i;
    MT_UINT32 num_params = wordsCount(params, ' ');
    
    /*First, check the the number of given arguments matches the number of  */
    /*arguments that the BCL command expects to receive... */
    if((checkArgCount && num_params > MAX_PARAM_COUNT) || (!checkArgCount && num_params != BCL_function_info_table[cmd_index].nNumArguments))
    {
        MT_BCL_error_occured = MT_TRUE;
        strCpy(MT_BCL_cmd_line, "Incorrect number of parameters. Usage: ");
        strCpy(&MT_BCL_cmd_line[strLen(MT_BCL_cmd_line)], BCL_function_help_table[cmd_index]);
        return MT_FALSE;
    }
    i = 0;
    
    /*Now, set up the MT_BCL_arg array with the given arguments.  The BCL  */
    /*function expects to find its arguments in it. */
    while(*params)
    {
        int numConvErr;
        MT_BCL_arg[i] = asciiToUnsigned(params, ' ', &numConvErr);
        
        if(numConvErr)
        {
            MT_BCL_error_occured = MT_TRUE;
            strCpy(MT_BCL_cmd_line, "Invalid value entered for argument ");
            unsignedToAscii(i + 1, &MT_BCL_cmd_line[strLen(MT_BCL_cmd_line)]);
            return MT_FALSE;
        }
        params = skipWord(params, ' ');
        params = skipDelimiters(params, ' ');
        i++;
    }
    return MT_TRUE;
}


/**********************************************************
* Function Name: convertItemListPtrs
* Description:  If an ItemListPointer was defined for a BCL command, we want to
*				replace the user's input (item list number) with the actual 
*				address of the requested item list.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	unsigned int bsILPtrArgs -	This uint contains a packed bitfield, which	
*								stores whether an argument is an ILPtr or not.
*								It is done for all possible 8 arguments of a
*								BCL command.
* Output Parameters:
* Return Value:
*	int	-	Returns either MT_TRUE if everything went ok, or MT_FALSE if something 
*			went wrong.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
int convertItemListPtrs(unsigned int bsILPtrArgs, int *IL_error)
{
    int i = 0;
    *IL_error = 0;
    
    /*Find out which bits are lit in the bsILPtrArgs.  Each bit represent an */
    /*argument for the BCL command.  If the bit is lit, this argument is an  */
    /*ILPtr... */
    while(i < MAX_PARAM_COUNT)
    {
        if(bsILPtrArgs & 1) /*If the bit is lit... */
        {
            if(MT_BCL_arg[i] >= MT_BCL_ITEM_LISTS_NUM) /*Make sure the give item list number is valid... */
            {
                *IL_error = i + 1;
                return MT_FALSE;    /*Failed. */
            }
            MT_BCL_arg[i] = (MT_UINT32) & MT_BCL_item_list[MT_BCL_arg[i]];
        }
        i++;
        bsILPtrArgs >>= 1;    /*Prepare for the next bit. */
    }
    return MT_TRUE;    /*Success. */
}


/**********************************************************
* Function Name: executeBCLCommand
* Description:  This is the main function of the BCL.  It receives a string 
*				with a BCL command, including its parameters, and executes it.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *cmd_line	-	The buffer containing the BCL commands with its arguments.
* Output Parameters:
*	char *cmd_line	-	The result is also returned via this buffer.
* Return Value:
*	int	-	Either MT_TRUE if everything went ok, or MT_FALSE if something went wrong.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
int executeBCLCommand(char *cmd_line, int length)
{
    MT_UINT32 i;
    char * cmd, *params;
    unsigned int num_cmds = sizeof(BCL_function_table) / sizeof(BCL_function_table[0]);
    skipReturnValue = 0; /* defualt use is without skipping the return value */

    /*If there's nothing to execute... */
    if(!cmd_line)
        return MT_TRUE;
    
    if(!cmd_line[0])
        return MT_TRUE;
    MT_BCL_cmd_line = cmd_line;
    cmd_line = skipDelimiters(cmd_line, ' ');    /*Get the input without any prefix spaces... */
    cmd = strUpr(cmd_line, ' ');    /*Get the BCL command itself.  Uppercase the command itself, without the params. */
    params = skipDelimiters(skipWord(cmd_line, ' '), ' ');    
    /*'params' will now point to the first character of the first parameter, with any prefix spaces ommited. */
    MT_BCL_error_occured = MT_FALSE;    /*Reset the error flag. */
    
    /*Search for the first word of 'cmd_line' inside the BCL_function_table... */
    for(i = 0; i < num_cmds; i++)
    {
        if(compareFirstWordOnly(BCL_function_table[i], cmd, ' ') == 0) 
                /*If we've found the entry for the command inside BCL_function_table... */
        {
            int result;    /*Will hold the result returned from the WrapperFunction. */
            int IL_error;    
            /*Will either hold 0 if no problems were detected during convertItemListPtrs, or it'll hold the problematic argument number if something went wrong. */
            MT_BCL_CurrentCommand = i;
            if(!BCL_function_info_table[i].bSpecialCmd) 
                    /*A special command doesn't accept arguments, so don't call setupFuncArgs */
            {
                if(!setupFuncArgs(i, params,0))
                    return 0;
            }
            
            /*ItemList pointers needs to be converted in a special manner.  This function  */
            /*call does that, and also checks that the given values are valid... */
            if(!convertItemListPtrs(BCL_function_info_table[i].bsILPtrArgs, &IL_error))
            {
                MT_BCL_error_occured = MT_TRUE;
                strCpy(cmd_line, "Bad item list index in argument ");
                unsignedToAscii(IL_error, &cmd_line[strLen(cmd_line)]);
                return MT_FALSE;
            }
            
            /*Run the function associated with the given command... */
            result = WrapperFunction(i);
            
            if(!skipReturnValue && !BCL_function_info_table[i].bSpecialCmd) /*If this isn't a special command... */
            {
                if(BCL_function_info_table[i].bReturnHex)
                    unsignedToAsciiHex(result, cmd_line);    
                /*Hex doesn't care whether it is signed or not.  The display is the same. */
                else /*return decimal... */
                {
                    if(BCL_function_info_table[i].bReturnSigned)
                        signedToAscii(result, cmd_line);    /*return signed decimal */
                    else
                        unsignedToAscii(result, cmd_line);    /*return unsigned decimal */
                }
            }
            return !MT_BCL_error_occured;    /*If an error occured, return MT_FALSE. */
        }
    }
    
    /*If we got here, the command wasn't recognized... */
    MT_BCL_error_occured = MT_TRUE;
    strCpy(cmd_line, "Illegal command.");
    return MT_FALSE;
}

/******************************************************************************
 * main
 *****************************************************************************/
#ifdef _STANDALONE	/*These functions are only defined if the project is compiled as a standalone (i.e. not as a library). */
#include "stdio.h"


 /**********************************************************
* Function Name: getLine
* Description:  Reads an input line from the keyboard into specified buffer.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	char *buff		- Points to a char * buffer to hold the input data.
*	int buff_len	- The size of the buffer
* Output Parameters:
* Return Value:
*    char *	- Returns a pointer to the input buffer.
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
char * getLine(char *buff, int buff_len)
{
    char ch = 0;
    int count = 0;
    ch = (char)fgetc(stdin);
    
    while((ch != '\n') && (count < buff_len))
    {
        buff[count] = ch;
        ch = (char)fgetc(stdin);
        count++;
    }
    buff[count] = '\0';
    return buff;
}

#ifdef WIN32
int __cdecl main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    char cmdline[256];
    
    for(;;)
    {
        printf("> ");
        
        /*Read the user's input string... */
        getLine(cmdline, 256);
        
        /*Try to execute user's command... */
        executeBCLCommand(cmdline, strLen(cmdline));
        
        /*If there's something to print, print it... */
        if(cmdline[0])
            printf("%s\n\n", cmdline);
    }
    return 0;
}
#endif
