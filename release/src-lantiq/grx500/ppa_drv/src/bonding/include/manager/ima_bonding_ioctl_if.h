
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifndef __IMA_BONDING_IOCTL_IF_H__
#define __IMA_BONDING_IOCTL_IF_H__

/*
 * Task of this module is to handle the ioctl request from user space
 */

#include "common/ima_common.h"
#include "common/ima_atm_bond_ioctl.h"

#define IMA_BOND_DEV_NAME "ima_bond"

/*
 * Task of this function is to initialize the ioctl module.
 */
s32 init_ioctl_interface(void);

/*
 * Task of this function is to cleanup the ioctl module
 */
s32 cleanup_ioctl_interface(void);

#endif /* __IMA_BONDING_IOCTL_IF_H__ */
