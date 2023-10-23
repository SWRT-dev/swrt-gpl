
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This module contains all the vrx518 tc driver callback function.
 */

#ifndef __IMA_VRX518_TC_INTERFACE_H__
#define __IMA_VRX518_TC_INTERFACE_H__

#include "common/ima_common.h"

/*
 * Task of this function is to initialize the vrx518 tc interface module and
 * register vrx518 tc driver callbacks.
 */
s32 init_vrx518_tc_interface(void);

/*
 * Task of this function is to cleanup all the module initialized data and
 * unregister vrx518 tc driver callbacks.
 */
s32 cleanup_vrx518_tc_interface(void);

#endif /* __IMA_VRX518_TC_INTERFACE_H__ */
