/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* test.h: interface for the Hyp class. 
//
/////////////////////////////////////////////////////////////////////*/

#ifndef __test_h__
#define __test_h__



MT_UINT32 itemlist  (MT_UBYTE item);
MT_UINT32 pitemlist (MT_UBYTE item);
MT_UINT32 item32    (void);
MT_UINT32 item16    (void);
MT_UINT32 item8     (void);
MT_UINT32 dreg8     (MT_UBYTE space, void* address, MT_UBYTE count);
MT_UINT32 dreg16    (MT_UBYTE space, void* address, MT_UBYTE count);
MT_UINT32 dreg32    (MT_UBYTE space, void* address, MT_UBYTE count);
MT_UINT32 dmem      (MT_UBYTE mem_size, void* address, MT_UBYTE count);
MT_UINT32 ditem     (MT_UBYTE mem_size,MT_UINT32 address,MT_UBYTE offset, MT_UBYTE count);
MT_UINT32 item      (MT_UBYTE size);
MT_UINT32 bcl_exit  (void);
MT_UINT32 WriteStr  (void);
MT_UINT32 ReadStr   (MT_UBYTE * pAddress);



#endif /* __test_h__ */

