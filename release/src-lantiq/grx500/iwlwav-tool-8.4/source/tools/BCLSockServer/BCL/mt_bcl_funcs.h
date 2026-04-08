/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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
MT_UINT32 dreg8     (MT_UBYTE space, MT_UINT32 address, MT_UBYTE count);
MT_UINT32 dreg16    (MT_UBYTE space, MT_UINT32 address, MT_UBYTE count);
MT_UINT32 dreg32    (MT_UBYTE space, MT_UINT32 address, MT_UBYTE count);
MT_UINT32 dmem      (MT_UBYTE mem_size,MT_UINT32 address, MT_UBYTE count);
MT_UINT32 ditem     (MT_UBYTE mem_size,MT_UINT32 address,MT_UBYTE offset, MT_UBYTE count);
MT_UINT32 item      (MT_UBYTE size);
MT_UINT32 bcl_exit  (void);
MT_UINT32 WriteStr  (void);
MT_UINT32 ReadStr   (MT_UBYTE * pAddress);



#endif /* __test_h__ */

