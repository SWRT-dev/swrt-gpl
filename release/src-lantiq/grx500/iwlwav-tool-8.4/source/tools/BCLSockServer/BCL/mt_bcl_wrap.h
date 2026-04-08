/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*******************************************************************
*                                                                  *
*   MT_BCL_WRAP.H                                                  *
*                                                                  *
*   Do not modify this file directly.                              *
*   To update the table, modify BCL_H_GEN.XLS and click the        *
*   'Generate .h File' button.                                     *
*   Author: Ron Zilberman                                          *
*                                                                  *
*******************************************************************/
#ifndef MT_BCL_WRAP /* include once */
#define MT_BCL_WRAP

#include "mt_wapi.h"
#include "mt_bcl_defs.h"
#include "mt_bcl_funcs.h"
#include "mt_help.h"
#include "mt_chacc.h"

/*This struct holds information specific for each of the BCL commands... */
typedef struct FunctionInfoTable
{
   unsigned char bsILPtrArgs   : MAX_PARAM_COUNT;  /*packed MAX_PARAM_COUNT bits.  If bit0 is lit, the first parameter is */
   /*an ItemList pointer.  If bit1 is lit, the second parameter  */
   /*is an ItemList pointer, and so on. */
   unsigned char nNumArguments : 4;    /*The number of arguments the BCL command expects. */
   unsigned char bReturnSigned : 1;    /*If this bit is lit, the return value should be signed.  Unsigned otherwise. */
   unsigned char bReturnHex        : 1;    /*If this bit is lit, the return value should be in hex.  Decimal otherwise. */
   unsigned char bSpecialCmd   : 1;    /*If this bit is lit, this BCL function is a special command.  This means it  */
   /*won't set up the arguments regularly (as ints) but read the input buffer  */
   /*independantly. */
} stFunctionInfoTable;
char *BCL_function_table[] =
{
    " ",
    "ATHENA_READ",
    "ATHENA_WRITE",
    "CHIPVAR_GET",
    "DEBUGTABLE",
    "DITEM",
    "DMEM",
    "DREG16",
    "DREG32",
    "DREG8",
    "DRV_CAT_FREE",
    "DRV_CAT_INIT",
    "DRV_NAME_GET",
    "DRV_VAL_GET",
    "DRV_VAL_SET",
    "EXIT",
    "GENIOCTL",
    "GETMIB",
    "HELP",
    "HYP_READ       ",
    "HYP_WRITE      ",
    "HYP_WRITEBUFFER",
    "ITEM16",
    "ITEM32",
    "ITEM8",
    "ITEMLIST",
    "MACCALIBRATE",
    "MACVARGET",
    "MACVARSET",
    "MEM_READ",
    "MEM_WRITE",
    "MTAPI",
    "PITEMLIST",
    "PRINT_LEVEL",
    "PROM_READ",
    "PROM_WRITE",
    "READ_FILE",
    "REBOOT",
    "RECONF",
    "RG_CONF_GET",
    "RG_CONF_SET",
    "RG_SHELL",
    "SETMIB",
    "SHELL",
    "STR_READ",
    "STR_WRITE",
    "SYSTEM",
    "TEST32",
    "TFTP_GET",
    "TFTP_PUT",
    "VER",
};

stFunctionInfoTable BCL_function_info_table[] =
{
    //ILPtr bitfield, Number of arguments, Return signed value, Return Hex display, Special command
    { 0 ,0, 0 , 0 , 1 }, //Test32
    { 0 ,2, 0 , 0 , 0 }, //AthenaRead 
    { 0 ,3, 0 , 0 , 0 }, //AthenaWrite
    { 0 ,4, 0 , 0 , 0 }, //Chipvar_Get
    { 32 ,6, 0 , 0 , 0 }, //DebugTable
    { 2 ,4, 0 , 0 , 0 }, //ditem
    { 0 ,3, 0 , 0 , 0 }, //dmem
    { 0 ,3, 0 , 0 , 0 }, //dreg16   
    { 0 ,3, 0 , 0 , 0 }, //dreg32   
    { 0 ,3, 0 , 0 , 0 }, //dreg8    
    { 0 ,1, 0 , 0 , 0 }, //DrvCatFree
    { 2 ,2, 0 , 0 , 0 }, //DrvCatInit
    { 4 ,3, 0 , 0 , 0 }, //DrvNameGet
    { 4 ,3, 0 , 0 , 0 }, //DrvValGet
    { 0 ,3, 0 , 0 , 0 }, //DrvValSet
    { 0 ,0, 0 , 0 , 0 }, //bcl_exit
    { 0 ,0, 0 , 0 , 1 }, //GenIoctl
    { 8 ,4, 0 , 0 , 0 }, //GetMibParams
    { 0 ,0, 0 , 1 , 1 }, //showHelp
    { 0 ,3, 0 , 0 , 0 }, //Hyp_Read       
    { 0 ,3, 0 , 0 , 0 }, //Hyp_Write      
    { 8 ,4, 0 , 0 , 0 }, //Hyp_WriteBuffer
    { 0 ,0, 0 , 0 , 1 }, //item16   
    { 0 ,0, 0 , 0 , 1 }, //item32   
    { 0 ,0, 0 , 0 , 1 }, //item8    
    { 0 ,1, 0 , 0 , 0 }, //itemlist 
    { 0 ,0, 0 , 0 , 0 }, //SetMacCalibration
    { 0 ,0, 0 , 0 , 1 }, //GenIoctl
    { 0 ,0, 0 , 0 , 1 }, //GenIoctl
    { 8 ,4, 0 , 0 , 0 }, //HypPciRead
    { 8 ,4, 0 , 0 , 0 }, //HypPciWrite
    { 0 ,0, 0 , 0 , 1 }, //Test32
    { 0 ,1, 0 , 0 , 0 }, //pitemlist
    { 0 ,1, 0 , 0 , 0 }, //PrintLevel
    { 0 ,3, 0 , 0 , 0 }, //PromRead 
    { 0 ,4, 0 , 0 , 0 }, //PromWrite
    { 0 ,0, 0 , 0 , 1 }, //Read_File
    { 0 ,0, 0 , 0 , 0 }, //OpenRG_reboot
    { 0 ,1, 0 , 0 , 0 }, //OpenRG_reconf
    { 1 ,1, 0 , 0 , 0 }, //RG_conf_get
    { 3 ,2, 0 , 0 , 0 }, //RG_conf_set
    { 0 ,0, 0 , 0 , 1 }, //RG_Command
    { 8 ,4, 0 , 0 , 0 }, //SetMibParams
    { 0 ,0, 0 , 0 , 1 }, //Shell_Command
    { 0 ,1, 0 , 0 , 0 }, //ReadStr
    { 0 ,0, 0 , 0 , 1 }, //WriteStr
    { 0 ,0, 0 , 0 , 1 }, //System_Command
    { 0 ,0, 0 , 0 , 1 }, //Test32
    { 3 ,2, 0 , 0 , 0 }, //TFTP_Get
    { 3 ,2, 0 , 0 , 0 }, //TFTP_Put
    { 0 ,1, 0 , 0 , 0 }, //version
};

#ifdef MT_BCL_INCLUDE_CMDS_HELP
char *BCL_function_help_table[] =
{
    /*Test32*/                         "TEST32 command [param0 … param7]",
    /*AthenaRead */                    "ATHENA_READ page address",
    /*AthenaWrite*/                    "ATHENA_WRITE page address data",
    /*Chipvar_Get*/                    "ChipvarGet CPU index offset count itemlist",
    /*DebugTable*/                     "DEBUGTABLE    ID action value size offset itemlistIndex",
    /*ditem*/                          "DITEM data_size itemlist_number offset count",
    /*dmem*/                           "DMEM data_size address count",
    /*dreg16   */                      "DREG16 3 address count",
    /*dreg32   */                      "DREG32  3 address count",
    /*dreg8    */                      "DREG8  3 address count",
    /*DrvCatFree*/                     "DRV_CAT_FREE Category",
    /*DrvCatInit*/                     "DRV_CAT_INIT Category Count",
    /*DrvNameGet*/                     "DRV_NAME_GET Category Index Name",
    /*DrvValGet*/                      "DRV_VAL_GET Category Index Value",
    /*DrvValSet*/                      "DRV_VAL_SET Category Index Value",
    /*bcl_exit*/                       "EXIT",
    /*GenIoctl*/                       "GENIOCTL Opcode Sizeof Action [reserv_0] [reserv_1] [reserv_2] Itemlist",
    /*GetMibParams*/                   "GETMIB Parameter Type Size Itemlist",
    /*showHelp*/                       "helpHelp",
    /*Hyp_Read       */                "HYP_READ address mask length",
    /*Hyp_Write      */                "HYP_WRITE address mask data",
    /*Hyp_WriteBuffer*/                "HYP_WRITEBUFFER address mask length itemlist",
    /*item16   */                      "ITEM16 [data0 … data7] ",
    /*item32   */                      "ITEM32 [data0 … data7]",
    /*item8    */                      "ITEM8 [data0 … data7]",
    /*itemlist */                      "ITEMLIST item_number",
    /*SetMacCalibration*/              "MACCALIBRATE - Enables MIB calibration on the fly, no parameters",
    /*GenIoctl*/                       "MACVARGET 1 Sizeof 0 Offset Itemlist",
    /*GenIoctl*/                       "MACVARSET 1 Sizeof 1 Offset Itemlist",
    /*HypPciRead*/                     "MEM_READ unit mac_address count itemlist_number",
    /*HypPciWrite*/                    "MEM_Write unit mac_address count itemlist_number",
    /*Test32*/                         "MTAPI command [param0 … param7]",
    /*pitemlist*/                      "PITEMLIST item_number",
    /*PrintLevel*/                     "",
    /*PromRead */                      "PROM_READ chip address mask",
    /*PromWrite*/                      "PROM_WRITE chip address mask data",
    /*Read_File*/                      "READ_FILE offset len filenameString",
    /*OpenRG_reboot*/                  "REBOOT",
    /*OpenRG_reconf*/                  "RECONF 1  (Save rg_conf changes to flash)",
    /*RG_conf_get*/                    "RG_CONF_GET path  (path is passed in an itemlist)",
    /*RG_conf_set*/                    "RG_CONF_SET path value   (path and value are passed in itemlists)",
    /*RG_Command*/                     "RG_SHELL command - runs a management command in the open RG",
    /*SetMibParams*/                   "SETMIB Parameter Type Size Itemlist",
    /*Shell_Command*/                  "SHELL command - runs as shell command on the system",
    /*ReadStr*/                        "STR_READ address_to_read_from",
    /*WriteStr*/                       "STR_WRITE address_to_write string",
    /*System_Command*/                 "SYSTEM command - runs as shell command and returns the stdout",
    /*Test32*/                         "TEST32 command [param0 … param7]",
    /*TFTP_Get*/                       "TFTP_GET filename_on_pc filename_on_host (filenames are passed in itemlists)",
    /*TFTP_Put*/                       "TFTP_PUT filename_on_host filename_on_pc (filenames are passed in itemlists)",
    /*version*/                        "VER [type]",
};
#endif

MT_UINT32 WrapperFunction(int index)
{
    switch (index)
    {
    case 0 : return (MT_UINT32)Test32();
    case 1 : return (MT_UINT32)AthenaRead ((MT_UBYTE)MT_BCL_arg[0],(MT_UBYTE)MT_BCL_arg[1]);
    case 2 : return (MT_UINT32)AthenaWrite((MT_UBYTE)MT_BCL_arg[0],(MT_UBYTE)MT_BCL_arg[1],(MT_UBYTE)MT_BCL_arg[2]);
    case 3 : return (MT_UINT32)Chipvar_Get((MT_UBYTE)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UINT32)MT_BCL_arg[2],(MT_UINT32)MT_BCL_arg[3]);
    case 4 : return (MT_UINT32)DebugTable((MT_UINT32)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UINT32)MT_BCL_arg[2],(MT_UINT32)MT_BCL_arg[3],(MT_UINT32)MT_BCL_arg[4],(MT_BYTE *)MT_BCL_arg[5]);
    case 5 : return (MT_UINT32)ditem((MT_UBYTE)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UBYTE)MT_BCL_arg[2],(MT_UBYTE)MT_BCL_arg[3]);
    case 6 : return (MT_UINT32)dmem((MT_UBYTE)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UBYTE)MT_BCL_arg[2]);
    case 7 : return (MT_UINT32)dreg16   ((MT_UBYTE)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UBYTE)MT_BCL_arg[2]);
    case 8 : return (MT_UINT32)dreg32   ((MT_UBYTE)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UBYTE)MT_BCL_arg[2]);
    case 9 : return (MT_UINT32)dreg8    ((MT_UBYTE)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UBYTE)MT_BCL_arg[2]);
    case 10 : return (MT_UINT32)DrvCatFree((MT_UINT32)MT_BCL_arg[0]);
    case 11 : return (MT_UINT32)DrvCatInit((MT_UINT32)MT_BCL_arg[0],(MT_UINT32 *)MT_BCL_arg[1]);
    case 12 : return (MT_UINT32)DrvNameGet((MT_UINT32)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UBYTE *)MT_BCL_arg[2]);
    case 13 : return (MT_UINT32)DrvValGet((MT_UINT32)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UINT32 *)MT_BCL_arg[2]);
    case 14 : return (MT_UINT32)DrvValSet((MT_UINT32)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UINT32)MT_BCL_arg[2]);
    case 15 : return (MT_UINT32)bcl_exit();
    case 16 : return (MT_UINT32)GenIoctl();
    case 17 : return (MT_UINT32)GetMibParams((MT_UINT32)MT_BCL_arg[0],(MT_UBYTE)MT_BCL_arg[1],(MT_UINT32)MT_BCL_arg[2],(MT_BYTE *)MT_BCL_arg[3]);
    case 18 : return (MT_UINT32)showHelp();
    case 19 : return (MT_UINT32)Hyp_Read       ((MT_UINT32)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_BYTE)MT_BCL_arg[2]);
    case 20 : return (MT_UINT32)Hyp_Write      ((MT_UINT32)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UINT32)MT_BCL_arg[2]);
    case 21 : return (MT_UINT32)Hyp_WriteBuffer((MT_UINT32)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_BYTE)MT_BCL_arg[2],(MT_UINT32 *)MT_BCL_arg[3]);
    case 22 : return (MT_UINT32)item16   ();
    case 23 : return (MT_UINT32)item32   ();
    case 24 : return (MT_UINT32)item8    ();
    case 25 : return (MT_UINT32)itemlist ((MT_UBYTE)MT_BCL_arg[0]);
    case 26 : return (MT_UINT32)SetMacCalibration();
    case 27 : return (MT_UINT32)GenIoctl();
    case 28 : return (MT_UINT32)GenIoctl();
    case 29 : return (MT_UINT32)HypPciRead((MT_UBYTE)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UBYTE)MT_BCL_arg[2],(MT_UINT32 *)MT_BCL_arg[3]);
    case 30 : return (MT_UINT32)HypPciWrite((MT_UBYTE)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UBYTE)MT_BCL_arg[2],(MT_UINT32 *)MT_BCL_arg[3]);
    case 31 : return (MT_UINT32)Test32();
    case 32 : return (MT_UINT32)pitemlist((MT_UBYTE)MT_BCL_arg[0]);
    case 33 : return (MT_UINT32)PrintLevel((MT_BYTE)MT_BCL_arg[0]);
    case 34 : return (MT_UINT32)PromRead ((MT_UINT32)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UINT32)MT_BCL_arg[2]);
    case 35 : return (MT_UINT32)PromWrite((MT_UINT32)MT_BCL_arg[0],(MT_UINT32)MT_BCL_arg[1],(MT_UINT32)MT_BCL_arg[2],(MT_UINT32)MT_BCL_arg[3]);
    case 36 : return (MT_UINT32)Read_File();
    case 37 : return (MT_UINT32)OpenRG_reboot();
    case 38 : return (MT_UINT32)OpenRG_reconf((MT_UINT32)MT_BCL_arg[0]);
    case 39 : return (MT_UINT32)RG_conf_get((MT_BYTE *)MT_BCL_arg[0]);
    case 40 : return (MT_UINT32)RG_conf_set((MT_BYTE *)MT_BCL_arg[0],(MT_BYTE *)MT_BCL_arg[1]);
    case 41 : return (MT_UINT32)RG_Command();
    case 42 : return (MT_UINT32)SetMibParams((MT_UINT32)MT_BCL_arg[0],(MT_UBYTE)MT_BCL_arg[1],(MT_UINT32)MT_BCL_arg[2],(MT_BYTE *)MT_BCL_arg[3]);
    case 43 : return (MT_UINT32)Shell_Command();
    case 44 : return (MT_UINT32)ReadStr((MT_UBYTE *)MT_BCL_arg[0]);
    case 45 : return (MT_UINT32)WriteStr();
    case 46 : return (MT_UINT32)System_Command();
    case 47 : return (MT_UINT32)Test32();
    case 48 : return (MT_UINT32)TFTP_Get((MT_BYTE *)MT_BCL_arg[0],(MT_BYTE *)MT_BCL_arg[1]);
    case 49 : return (MT_UINT32)TFTP_Put((MT_BYTE *)MT_BCL_arg[0],(MT_BYTE *)MT_BCL_arg[1]);
    case 50 : return (MT_UINT32)version((MT_UBYTE)MT_BCL_arg[0]);
    }
    return 0; //we shouldn't get here.
}

#endif /* ndef MT_BCL_WRAP */
