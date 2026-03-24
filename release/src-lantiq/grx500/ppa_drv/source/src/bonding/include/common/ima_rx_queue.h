
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This file exposes the line-queue abstraction to the bonding library upstairs.
 * Within the IMA+ Bonding Driver, it occupies the layer shown below:
 *
 *    +-------+-------------------------------------+---------------------------+
 *    |       |  Rx Bonding Driver code components  |        Source code        |
 *    |       |        (sorted from UE to LE)       |                           |
 *    +-------+-------------------------------------+---------------------------+
 *    |       | T C  D R I V E R  (upper layer)     |                           |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |   1   | Reassembly FSM Library              | ima_rx_reassembler.c(.h)  |
 *    |   2   | Bonding Library                     | ima_rx_bonding.c(.h)      |
 *    |   3   | ATM Cell Decode Library             | ima_rx_atm_cell.h         |
 *    |   4   | Line Queue Abstraction Library      | ima_rx_queue.c(.h)        |<=
 *    |   5   | H/W RXOUT Ring Interface            | ima_atm_rx.c(.h)          |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |       | H/W RXOUT RINGS   (lower layer)     |                           |
 *    +-------+-------------------------------------+---------------------------+
 *
 *    Unit Tests: 1. src/test/ut_rx_reassembly_testgroup_2.c
 *                2. src/test/ut_rx_reassembly_testgroup_3.c
 *                3. src/test/ut_rx_bonding_and_reassembly_testgroup_1.c
 */

#ifndef __IMA_RX_QUEUE_H__
#define __IMA_RX_QUEUE_H__

struct desc_4dw {
	union {
		struct {
			u32 resv:20;
			u32 ep:4;
			u32 resv1:8;
		} __packed field;
		u32 all;
	} __packed dw1;
	union {
		struct {
			u32 resv:17;
			u32 dest_id:15;
		} __packed field;
		u32 all;
	} __packed dw0;
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 resv:2;
			u32 byte_offset:3;
			u32 resv1:7;
			u32 data_len:16;
		} __packed field;
		u32 all;
	} __packed status; /*dw3 */
	dma_addr_t data_pointer; /* dw2 */
} __packed __aligned(16);

void init_rx_lineq(unsigned int lineid, void *cfg);
unsigned char *read_rx_lineq_head(unsigned int lineid);
void dequeue_rx_lineq(unsigned int lineid);
unsigned int enqueue_rx_lineq(unsigned int lineid);

#endif /* __IMA_RX_QUEUE_H__ */
