/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*******************************************************************
*   MT_BCL.H                                                  
*                                                                  
*   Exports the ExecuteBCLCommand function.
*   Author: Ron Zilberman                                          
*                                                                  
*******************************************************************/
#include "mt_types.h"
#include "mt_bcl_defs.h"
#include "mtlk_rtlog_app.h"

extern MT_UINT32 MT_BCL_item_list[MT_BCL_ITEM_LISTS_NUM][MT_BCL_ITEM_LIST_SIZE]; /*The item_list pointers. */
extern MT_UINT32 MT_BCL_arg[MAX_PARAM_COUNT];										/*Contains the arguments for the BCL function to execute. */
extern MT_UBYTE MT_BCL_error_occured;									/*A boolean flag to indicate whether an error has occured. */
extern MT_UINT32 MT_BCL_CurrentCommand;

extern char *MT_BCL_cmd_line;						/*User's command line input pointer */

/* BCL API */

int  executeBCLCommand(char *cmd_line, int length);

#if defined(WIN32)
int  BCL_Init();
#else
int  BCL_Init(char* ifname, rtlog_app_info_t *rtlog_info_data);
#endif /* #if defined(WIN32) */

void BCL_Exit(void);

unsigned int setupFuncArgs(int cmd_index, char *params,char checkArgCount);
void BCL_SkipReturnValue(void);


#ifdef _WIN32
#define __FTP_PATH__ "c:/inetpub/ftproot"
#else /*!_WIN32*/
#define __FTP_PATH__ "/tmp"
#endif /*_WIN32*/
