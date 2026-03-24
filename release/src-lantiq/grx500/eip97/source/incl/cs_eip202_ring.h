/* cs_eip202_ring.h
 *
 * Top-level configuration parameters
 * for the EIP-202 Ring Control Driver Library
 */

/*****************************************************************************
* Copyright (c) 2011-2014 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef CS_EIP202_RING_H_
#define CS_EIP202_RING_H_

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */

#include "cs_driver.h"


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// Is device 64-bit?
#ifdef DRIVER_64BIT_DEVICE
#define EIP202_64BIT_DEVICE
#endif // DRIVER_64BIT_DEVICE

// Enable anti DMA race condition CDS mechanism.
// When enabled the Application ID field in the descriptors cannot be used
// by the Ring Control Driver Library users.
#ifndef DRIVER_USE_EXTENDED_DESCRIPTOR
#define EIP202_RING_ANTI_DMA_RACE_CONDITION_CDS
#endif

// Disable clustered write operations, e.g. every write operation to
// an EIP-202 RD register will be followed by one read operation to
// a pre-defined EIP-202 register
#define EIP202_CLUSTERED_WRITES_DISABLE

// Strict argument checking for the input parameters
// If required then define this parameter in the top-level configuration
#define EIP202_RING_STRICT_ARGS

// Finite State Machine that can be used for verifying that the Driver Library
// API is called in a correct order
#define EIP202_RING_DEBUG_FSM

#ifdef DRIVER_USE_EXTENDED_DESCRIPTOR
// Define if the hardware uses extended command and result descriptors.
#define EIP202_USE_EXTENDED_DESCRIPTOR
#endif


#endif /* CS_EIP202_RING_H_ */


/* end of file cs_eip202_ring.h */

