/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#define MT_BCL_ITEM_LISTS_NUM 10        /*Number of ItemList buffers*/
#define MT_BCL_ITEM_LIST_SIZE 1024         /*Size of each ItemList buffer*/
#define MT_BCL_INCLUDE_CMDS_HELP     /*If defined, help will be included in the BCL project.  If ommited, the help won't be included in the project, which will result in rather significant space saving.*/
#define MT_BCL_INCLUDE_DMEM               /*If defined, DMEM actions will be supported */
#define MAX_PARAM_COUNT 8                   /*8 parameters are supported for regular BCL functions.  Modifying this value, requires extensive changes to this excel file, as well as various parts of the source code*/

#define MT_BCL_COMMANDS_NUMBER  51 
