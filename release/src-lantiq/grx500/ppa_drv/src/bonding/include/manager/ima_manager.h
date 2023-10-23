
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * Manager module of ima+ bonding driver.
 */

#ifndef __IMA_MANAGER_H__
#define __IMA_MANAGER_H__

#include <linux/kernel.h>
#include <linux/module.h>

#include "common/ima_modules.h"
#include "atm/ima_tx_cell_buffer.h"
#include "common/ima_vrx518_tc_interface.h"
#include "atm/ima_atm_rx.h"
#include "manager/ima_bonding_ioctl_if.h"

MODULE_INFO(version, "1.1.3");
MODULE_INFO(license, "Dual BSD/GPL");
MODULE_INFO(description, "IMA+ bonding driver");

/*
 * This is the entry point of IMA+ bonding driver
 */
static int __init manager_init(void);

/*
 * This is the exit/cleanup point of IMA+ bonding driver
 */
static void __exit manager_exit(void);

#endif /* __IMA_MANAGER_H__ */
