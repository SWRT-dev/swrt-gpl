
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This file exposes functions of ATM Layer Bonding's Receiving Entity
 * to the rest of the Rx Module of IMA+ Bonding driver.
 * Within the IMA Bonding Driver, it occupies the layer shown below:
 *
 *    +-------+-------------------------------------+---------------------------+
 *    |       |  Rx Bonding Driver code components  |        Source code        |
 *    |       |        (sorted from UE to LE)       |                           |
 *    +-------+-------------------------------------+---------------------------+
 *    |       | T C  D R I V E R  (upper layer)     |                           |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |   1   | Reassembly FSM Library              | ima_rx_reassembler.c(.h)  |
 *    |   2   | Bonding Library                     | ima_rx_bonding.c(.h)      |<=
 *    |   3   | ATM Cell Decode Library             | ima_rx_atm_cell.h         |
 *    |   4   | Line Queue Abstraction Library      | ima_rx_queue.c(.h)        |
 *    |   5   | H/W RXOUT Ring Interface            | ima_atm_rx.c(.h)          |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |       | H/W RXOUT RINGS   (lower layer)     |                           |
 *    +-------+-------------------------------------+---------------------------+
 *
 *    Unit Tests: 1. src/test/ut_rx_bonding_testgroup_1.c
 *                2. src/test/ut_rx_bonding_testgroup_2.c
 *                3. src/test/ut_rx_bonding_testgroup_3.c
 *                4. src/test/ut_rx_bonding_and_reassembly_testgroup_1.c
 */

#ifndef __IMA_RX_BONDING_H__
#define __IMA_RX_BONDING_H__

// To be called from bonding driver's rx-init
int32_t init_rx_bonding(void);

// To be called from bonding driver's rx-cleanup
void cleanup_rx_bonding(void);

// To be called from bonding driver's rx-device-init
int32_t init_device_rx_bonding(uint32_t lineidx, void *cfg);

// To be called from bonding driver's rx-device-cleanup
void cleanup_device_rx_bonding(uint32_t lineidx);

// To be called from bonding driver's rx interrupt handler
int32_t poll_rx_bonding(void);

// To be called from ASM FSM
void update_rx_config(bool sid_is_8bit);
void activate_rx_link(u32 linkid);
void deactivate_rx_link(u32 linkid);
void set_rx_data_acceptable(u32 linkid);
void set_rx_data_unacceptable(u32 linkid);

// Pending:
// 1. get stats
char *getstats_rx_string(void);
// 2. ioctl
/**********************************************************************/
// Macros to make bonding driver code UNIT Test friendly

/**********************************************************************/

///////////////////////////////////////////////////////////////////////
// Macros to make bonding driver code independent of kernel or userspace environment


///////////////////////////////////////////////////////////////////////

#endif /* __IMA_RX_BONDING_H__ */
