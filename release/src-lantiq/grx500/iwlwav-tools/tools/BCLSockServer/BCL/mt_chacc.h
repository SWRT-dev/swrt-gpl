/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

// mt_chacc.h: interface for the Hyp class.
//
//////////////////////////////////////////////////////////////////////
//#ifndef __mt_chacc_h__
//#define __mt_chacc_h__

#include "mt_cnfg.h"


MT_UINT32 HypPciRead(MT_UBYTE unit, MT_UINT32 address, MT_UBYTE size, MT_UINT32 * data);
MT_UINT32 HypPciWrite(MT_UBYTE unit, MT_UINT32 address, MT_UBYTE size, MT_UINT32 * data);
MT_UINT32 HypPciCmd( MT_UINT16 command, MT_UBYTE inSize,  MT_UINT32 * inData, MT_UINT32 * outData);
MT_UINT32 HypInit(void);
MT_UINT32 GetMibParams(MT_UINT32 Id, MT_UBYTE type, MT_UINT32 size, MT_BYTE * data);
MT_UINT32 SetMibParams(MT_UINT32 Id, MT_UBYTE type, MT_UINT32 size, MT_BYTE * data);
MT_UINT32 SetMacCalibration(void);
MT_UINT32 GenIoctl(void);
MT_UINT32 DebugTable(MT_UINT32 Id, MT_UINT32 action, MT_UINT32 value, MT_UINT32 size, MT_UINT32 offset, MT_BYTE * data);

#ifdef ERROR_A
//xchbxvh
#endif




//#endif // __mt_chacc_h__


