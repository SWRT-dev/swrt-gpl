/* c_eip202_ring.h
 *
 * Default configuration of the EIP-202 Ring Control Driver Library
 */

/*****************************************************************************
* Copyright (c) 2011-2013 INSIDE Secure B.V. All Rights Reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef C_EIP202_RING_H_
#define C_EIP202_RING_H_

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */

// Top-level configuration, can override default configuration
// parameters specified in this file
#include "cs_eip202_ring.h"


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// Set to 1 to enable RDR interrupt generation per packet
// instead of interrupt generation per result descriptor
#ifndef EIP202_RING_RD_INTERRUPTS_PER_PACKET_FLAG
#define EIP202_RING_RD_INTERRUPTS_PER_PACKET_FLAG  1
#endif

// Enables the 64-bit DMA addresses support in the device
//#define EIP202_64BIT_DEVICE

// Enable anti DMA race condition CDS mechanism.
// When enabled the Application ID field in the descriptors cannot be used
// by the Ring Control Driver Library users.
//#define EIP202_RING_ANTI_DMA_RACE_CONDITION_CDS

// Disable clustered write operations, e.g. every write operation to
// an EIP-202 RD register will be followed by one read operation to
// a pre-defined EIP-202 register
//#define EIP202_CLUSTERED_WRITES_DISABLE

// CDR Read cache type control (rd_cache field in HIA_CDR_y_DMA_CFG register)
#ifndef EIP202_RING_CD_RD_CACHE_CTRL
#define EIP202_RING_CD_RD_CACHE_CTRL            0
#endif

// CDR Write cache type control (wr_cache field in HIA_CDR_y_DMA_CFG register)
#ifndef EIP202_RING_CD_WR_CACHE_CTRL
#define EIP202_RING_CD_WR_CACHE_CTRL            0
#endif

// RDR Read cache type control (rd_cache field in HIA_RDR_y_DMA_CFG register)
#ifndef EIP202_RING_RD_RD_CACHE_CTRL
#define EIP202_RING_RD_RD_CACHE_CTRL            0
#endif

// RDR Write cache type control (wr_cache field in HIA_RDR_y_DMA_CFG register)
#ifndef EIP202_RING_RD_WR_CACHE_CTRL
#define EIP202_RING_RD_WR_CACHE_CTRL            0
#endif

// Strict argument checking for the input parameters
// If required then define this parameter in the top-level configuration
//#define EIP202_RING_STRICT_ARGS

// Finite State Machine that can be used for verifying that the Driver Library
// API is called in a correct order
//#define EIP202_RING_DEBUG_FSM

// Define if the hardware uses extended command and result descriptors.
//#define EIP202_USE_EXTENDED_DESCRIPTOR


#endif /* C_EIP202_RING_H_ */


/* end of file c_eip202_ring.h */
