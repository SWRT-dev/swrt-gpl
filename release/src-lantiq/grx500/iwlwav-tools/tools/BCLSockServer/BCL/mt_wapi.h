/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

// wls_chacc.h: interface for the Hyp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __mt_wapi_h__
#define __mt_wapi_h__


#include "mt_cnfg.h"

 
MT_UINT32 Hyp_Read          (MT_UINT32 address, MT_UINT32 mask, MT_UBYTE size);
MT_UINT32 Hyp_Write         (MT_UINT32 address, MT_UINT32 mask, MT_UINT32 data);
MT_UINT32 Hyp_WriteBuffer   (MT_UINT32 address, MT_UINT32 mask ,MT_UBYTE size, MT_UINT32 *pData);


// Athena access functions
MT_UBYTE  AthenaWrite(MT_UBYTE page, MT_UBYTE address, MT_UBYTE data);
MT_UBYTE  AthenaRead (MT_UBYTE page, MT_UBYTE address);

MT_UINT32 PromRead ( MT_UINT32 chip_select,MT_UINT32 prom_address, MT_UINT32 mask);
MT_UINT32 PromWrite(MT_UINT32 chip_select,MT_UINT32 prom_address, MT_UINT32 mask,MT_UINT32 data);


MT_UINT32       HypVersion(void);
MT_UINT32       Test32(void);
MT_UINT32       version(MT_UBYTE type);


/* OpenRG Support: RG_conf access functions */
MT_UINT32 RG_conf_get(MT_BYTE* path);
MT_UINT32 RG_conf_set(MT_BYTE* path, MT_BYTE* value);
MT_UINT32 OpenRG_reboot(void);
MT_UINT32 OpenRG_reconf(MT_UINT32 TimeFrame);

/* tftp functions on the host board */    
MT_UINT32 TFTP_Get(MT_BYTE* source_path, MT_BYTE* dest_path);
MT_UINT32 TFTP_Put(MT_BYTE* source_path, MT_BYTE* dest_path);

MT_UINT32 RG_Command(void);
MT_UINT32 Shell_Command(void);
MT_UINT32 System_Command(void);

MT_UINT32 Read_File(void);
MT_UINT32 DrvCatInit(MT_UINT32 category, MT_UINT32 * cnt) ;
MT_UINT32 DrvCatFree(MT_UINT32 category);  
MT_UINT32 DrvNameGet(MT_UINT32 category, MT_UINT32 index, MT_UBYTE * name); 
MT_UINT32 DrvValGet(MT_UINT32 category, MT_UINT32 index, MT_UINT32 * val); 
MT_UINT32 DrvValSet(MT_UINT32 category, MT_UINT32 index, MT_UINT32 val) ;

MT_UINT32 Chipvar_Get(MT_UBYTE cpu, MT_UINT32 index, MT_UINT32 offset, MT_UINT32 count);

#endif // __mt_wapi_h__


