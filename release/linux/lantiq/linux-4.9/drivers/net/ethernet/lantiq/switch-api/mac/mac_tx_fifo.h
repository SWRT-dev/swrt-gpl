/******************************************************************************
 *                Copyright (c) 2016, 2017, 2018 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#ifndef _MAC_TXFIFO_H
#define _MAC_TXFIFO_H

#define FIFO_TIMEOUT_IN_SEC         	100     /* 100 Sec */
#define MAX_FIFO_ENTRY			64	/* Max Hw Fifo available */
#define START_FIFO			1	/* */

enum {
	FIFO_FULL = -1,
	FIFO_ENTRY_AVAIL = -2,
	FIFO_ENTRY_NOT_AVAIL = -3,
};

u32 fifo_init(void *pdev);
int fifo_entry_add(void *pdev, u8 ttse, u8 ostc, u8 ostpa, u8 cic,
		   u32 ttsl, u32 ttsh);
int fifo_entry_del(void *pdev, u32 rec_id);
void fifo_isr_handler(void *pdev);
int print_fifo(void *pdev);

#endif
