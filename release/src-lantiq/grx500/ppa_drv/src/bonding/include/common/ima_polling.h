
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * Task of this module is to poll several resources like host umt address,
 * device side rxout ring owneship bit and wake up atm transmit tasklet, common
 * receive processor tasklet
 */

#ifndef __IMA_POLLING_H__
#define __IMA_POLLING_H__

#include <linux/kthread.h>
#include <linux/delay.h>

#include "common/ima_common.h"

#define DELAY_MINIMUM 95
#define DELAY_MAXIMUM 105
#define MAX_32_BIT_VALUE 0xFFFFFFFF

/*
 * Task of this module is to initialize the polling module
 */
s32 init_polling(void);

/*
 * Task of this function is to cleanup all the module initialized data
 */
s32 cleanup_polling(void);

#endif /* __IMA_POLLING_H__ */
