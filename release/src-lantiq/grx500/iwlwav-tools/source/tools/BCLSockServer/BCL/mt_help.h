/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mt_util.h"
#include "mt_bcl_defs.h"

/**********************************************************
* Function Name: showHelp
* Description:	This BCL command shows help for the specified command.
* Called From:
* Author:
*	Ron Zilberman
* Input Parameters:
*	BCLcmd	-	The BCL command to show help info for.
* Output Parameters:
*	char *MT_BCL_cmd_line	-	This buffer will hold either an error message or
*								the help info for the specified BCL command.
* Return Value:
* Algorithm:
* Global Variables Used:
* Revisions:
**********************************************************/
extern char *BCL_function_table[];
extern char *BCL_function_help_table[];

MT_UINT32 showHelp()
{
#ifdef MT_BCL_INCLUDE_CMDS_HELP  /*This function is defined only if the user has chosen to include help commands (space saver). */
	int i=0;
	int offset = 0;
	/*cmd will point to the first character in the second word of the  */
	/*MT_BCL_cmd_line.  This word will also be converted to uppercase.... */
	char *cmd = strUpr( skipDelimiters(skipWord(MT_BCL_cmd_line,' '),' '), ' ' );

	/*Locate the command in the BCL_function_help_table... */
	while (i < MT_BCL_COMMANDS_NUMBER)
	{
		if (0 == compareFirstWordOnly(cmd, BCL_function_table[i], ' '))  /*If we've found a match... */
		{
			strCpy(MT_BCL_cmd_line, BCL_function_help_table[i]);  /*Copy back to the buffer the help string.   */
																	/*(in a special BCL command, the buffer serves as an output). */
			return 0;
		};
		i++;
	}
	if (strLen(cmd) == 0)  /*If no parameter was given to the help command... */
    {
        for(i = 0; i < MT_BCL_COMMANDS_NUMBER; i++)
        {
            offset += strCpy(MT_BCL_cmd_line+offset, BCL_function_table[i]);
            MT_BCL_cmd_line[offset++] = '\n';
        }
        MT_BCL_cmd_line[--offset] = 0x0;
    }

	else  /*If an unrecognized command was given to the help command... */
	{
		MT_BCL_error_occured = MT_TRUE;  
		strCpy(MT_BCL_cmd_line, "HELP: Unknown command");
	}
#else
	MT_BCL_error_occured = MT_TRUE;
	strCpy(MT_BCL_cmd_line, "Help support is turned off through a compilation switch.");
#endif
	return 0;
}
