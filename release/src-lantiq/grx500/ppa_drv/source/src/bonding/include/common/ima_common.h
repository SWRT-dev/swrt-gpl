
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifndef __IMA_COMMON_H__
#define __IMA_COMMON_H__

/*
 * This is a common module and shared across all other modules(atm tx, atm rx,
 * asm tx, asm rx, polling thread)
 */

#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/dma-mapping.h>
#include <linux/crc32.h>

#include "common/atm_bond.h"

#define RS_SUCCESS 0
#define RS_FAILURE -1

#define CONST_STR ", line : %d, function : %s, file : %s\n"
#define ERROR_PRINTK(err_str) \
	do { \
		if (printk_ratelimit()) \
			printk(KERN_ALERT err_str CONST_STR, __LINE__, __func__, __FILE__); \
	} while (0)

#define ENABLE_LOG 0

/*
 * Task of this function is to initialize the common module
 */
s32 init_common(void);

/*
 * Task of this function is to cleanup all the module initialized data
 */
s32 cleanup_common(void);

/*
 * This is an interface function between common module and asm receive module.
 * Implementation of this function is in asm receive module. Task of this
 * function is to process the received asm message, update asm state machine
 * and update global private structure.
 */
s32 asm_rx_processor(void);

/*
 * This is an interface function between vrx518 tc module and polling thread
 * module. Implementation is in polling thread module. Task of this function
 * is to create and run a kernel thread.
 */
s32 start_polling_thread(void);

/*
 * This is an interface function between vrx518 tc module and polling thread
 * module. Implementation is in polling thread module. Task of this function
 * is to stop the running polling thread.
 */
s32 stop_polling_thread(void);

/*
 * This is an interface function between vrx518 tc module and atm receive
 * module. Implementation of this function is in vrx518 tc module. Task of
 * this function is to pass the received aal5 frame to vrx518 tc driver.
 */
int send_to_vrx518_tc_driver(struct atm_aal5_t *p_st_p_aal5_frame);

/*
 * This is an interface function between atm transmit module and vrx518 tc
 * interface module, implementation is in vrx518 tc interface module. This
 * function is called by atm transmit tasklet. Task of this function is to
 * release the host cbm buffer.
 */
s32 release_cbm_buffer(void *p_v_p_cbm_buff_ptr);

/*
 * This is an interface function between vrx518 tc module and polling thread
 * module. Implementation of this function is in vrx518 tc module. Task of
 * this function is to return the umt counter via tc callback.
 */
s32 get_umt(u32 *counter);

void first_asm_received(u8 linkid);

#endif /* __IMA_COMMON_H__ */
