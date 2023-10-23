
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifndef __IMA_MODULES_H__
#define __IMA_MODULES_H__

s32 init_modules(void);
s32 start_modules(unsigned int pollinterval);
s32 stop_modules(void);
s32 cleanup_modules(void);

#endif /* __IMA_MODULES_H__ */
