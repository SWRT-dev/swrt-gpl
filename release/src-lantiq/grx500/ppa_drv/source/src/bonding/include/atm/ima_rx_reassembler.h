
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This file exposes functions of AAL5 Reassembler component to the rest of
 * the Rx Module of IMA+ Bonding driver.
 * Within the IMA+ Bonding Driver, it occupies the layer shown below:
 *
 *    +-------+-------------------------------------+---------------------------+
 *    |       |  Rx Bonding Driver code components  |        Source code        |
 *    |       |        (sorted from UE to LE)       |                           |
 *    +-------+-------------------------------------+---------------------------+
 *    |       | T C  D R I V E R  (upper layer)     |                           |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |   1   | Reassembly FSM Library              | ima_rx_reassembler.c(.h)  |<=
 *    |   2   | Bonding Library                     | ima_rx_bonding.c(.h)      |
 *    |   3   | ATM Cell Decode Library             | ima_rx_atm_cell.h         |
 *    |   4   | Line Queue Abstraction Library      | ima_rx_queue.c(.h)        |
 *    |   5   | H/W RXOUT Ring Interface            | ima_atm_rx.c(.h)          |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |       | H/W RXOUT RINGS   (lower layer)     |                           |
 *    +-------+-------------------------------------+---------------------------+
 *
 *    Unit Tests: 1. src/test/ut_rx_reassembly_testgroup_1.c
 *                2. src/test/ut_rx_bonding_and_reassembly_testgroup_1.c
 *
 */

#ifndef __IMA_RX_REASSEMBLER_H__
#define __IMA_RX_REASSEMBLER_H__

void init_rx_reassembler(unsigned int maxcellcount);
void reassemble_aal5(ima_rx_atm_header_t *cell, u32 sid, u32 sid_range, u32 linkid);

#endif /* __IMA_RX_REASSEMBLER_H__ */
