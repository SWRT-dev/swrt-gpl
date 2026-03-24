/* c_eip97_global.h
 *
 * Default configuration parameters
 * for the EIP-97 Global Control Driver Library
 *
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

#ifndef C_EIP97_GLOBAL_H_
#define C_EIP97_GLOBAL_H_


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Top-level configuration
#include "cs_eip97_global.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// EIP-97 supported HW version
#define EIP97_GLOBAL_MAJOR_VERSION           1
#define EIP97_GLOBAL_MINOR_VERSION           0
#define EIP97_GLOBAL_PATCH_LEVEL             0

// EIP-202 supported HW version
#define EIP202_GLOBAL_MAJOR_VERSION          1
#define EIP202_GLOBAL_MINOR_VERSION          0
#define EIP202_GLOBAL_PATCH_LEVEL            0

// EIP-96 supported HW version
#define EIP96_GLOBAL_MAJOR_VERSION           2
#define EIP96_GLOBAL_MINOR_VERSION           0
#define EIP96_GLOBAL_PATCH_LEVEL             3

// Enables aggressive DMA mode. Set to 1 for optimal performance.
#ifndef EIP97_GLOBAL_DFE_AGGRESSIVE_DMA_FLAG
#define EIP97_GLOBAL_DFE_AGGRESSIVE_DMA_FLAG   1
#endif

// Enables DSE aggressive DMA mode. Set to 1 for optimal performance.
#ifndef EIP97_GLOBAL_DSE_AGGRESSIVE_DMA_FLAG
#define EIP97_GLOBAL_DSE_AGGRESSIVE_DMA_FLAG   1
#endif

// Bus burst size (bus type specific, AHB, AXI, PLB)
// AHB:
// Determines the maximum burst size that will be used on the AHB bus,
// values „n‟ in the range 0...4 select 2^n beat maximum bursts (i.e. from
// 1 to 16 beats) – other values are reserved and must not be used. The
// system reset value of 4 selects 16 beat bursts.
// AXI:
// Determines the maximum burst size that will be used on the AXI
// interface, values „n‟ in the range 1...4 select 2^n beat maximum bursts
// (i.e. from 1 to 16 beats) – other values are reserved and must not be
// used. The system reset value of 4 selects 16 beat bursts.
// Maximum values:
//    0100b - AXI v3 and AXI v4
// PLB (YYZZb):
// ZZ: Selects the maximum burst size on the PLB bus, value in range 00b – 11b:
// 00b = 2 words, 01b = 4 words, 10b2 = 8 words, 11b = 16 words.
// YY: Set data swap, value in range 00b – 11b:
//     00b - swap 8-bit chunks within each 16-bit word
//     01b - swap 8-bit chunks within each 32-bit word
//     10b - swap 8-bit chunks within each 64-bit word (only if HDW>=001b)
//     11b - swap 8-bit chunks within each 128-bit word (only if HDW>=010b)
// This field cannot be changed when PLB Timeout, Write or Read error occurs
#ifndef EIP97_GLOBAL_BUS_BURST_SIZE
#define EIP97_GLOBAL_BUS_BURST_SIZE          7 // 7 for PLB
#endif

// For AXI bus only, for non-AXI bus must be 0!
// This field controls when an AXI read channel Master timeout Irq will be
// generated. The minimum value is 1. The actual timeout fires when the last
// data for a read transfer has not arrived within Timeout*(2^26) clock
// cycles after the read command has been transferred.
#ifndef EIP97_GLOBAL_TIMEOUT_VALUE
#define EIP97_GLOBAL_TIMEOUT_VALUE           0xF
#endif

// Number of Processing Engines to use
// Maximum number of processing that should be used
// Should not exceed the number of engines physically available
#ifndef EIP97_GLOBAL_MAX_NOF_PE_TO_USE
#define EIP97_GLOBAL_MAX_NOF_PE_TO_USE              1
#endif

// Set this option to enable swapping of bytes in 32-bit words written via
// the Packet Engine slave interface, e.g. device registers.
//#define EIP97_GLOBAL_ENABLE_SWAP_REG_DATA

// Set this option to disable the endianness conversion by the host
// processor of the first words written to the Packet Engine registers
// during its initialization.
// If endianness conversion is configured for the Packet Engine Slave
// interface (by defining the EIP97_GLOBAL_ENABLE_SWAP_REG_DATA parameter)
// then also configure whether the endianness conversion is required
// for the very first registers written during the Packet Engine
// initialization, e.g. the Packet Engine registers used to reset
// the engine and activate the endianness conversion for the Slave interface.
//#define EIP97_GLOBAL_DISABLE_HOST_SWAP_INIT

// Enable Flow Lookup Data Endianness Conversion
// by the Classification Engine hardware master interface
//#define EIP97_GLOBAL_BYTE_SWAP_FLUE_DATA

// Enable Flow Record Data Endianness Conversion
// by the Classification Engine hardware master interface
//#define EIP97_GLOBAL_BYTE_SWAP_FLOW_DATA

// Enable Context Data Endianness Conversion
// by the Processing Engine hardware master interface
//#define EIP97_GLOBAL_BYTE_SWAP_CONTEXT_DATA

// Enable ARC4 Context Data Endianness Conversion
// by the PE hardware master interface
//#define EIP97_GLOBAL_BYTE_SWAP_ARC4_CONTEXT_DATA

// One or several of the following methods must be configured:
// Swap bytes within each 32 bit word
//#define EIP97_GLOBAL_BYTE_SWAP_METHOD_32
// Swap 32 bit chunks within each 64 bit chunk
//#define EIP97_GLOBAL_BYTE_SWAP_METHOD_64
// Swap 64 bit chunks within each 128 bit chunk
//#define EIP97_GLOBAL_BYTE_SWAP_METHOD_128
// Swap 128 bit chunks within each 256 bit chunk
//#define EIP97_GLOBAL_BYTE_SWAP_METHOD_256

#if defined(EIP97_GLOBAL_BYTE_SWAP_METHOD_32)
#define EIP97_GLOBAL_BYTE_SWAP_METHOD   1
#elif defined(EIP97_GLOBAL_BYTE_SWAP_METHOD_64)
#define EIP97_GLOBAL_BYTE_SWAP_METHOD   2
#elif defined(EIP97_GLOBAL_BYTE_SWAP_METHOD_128)
#define EIP97_GLOBAL_BYTE_SWAP_METHOD   4
#elif defined(EIP97_GLOBAL_BYTE_SWAP_METHOD_256)
#define EIP97_GLOBAL_BYTE_SWAP_METHOD   8
#else
#define EIP97_GLOBAL_BYTE_SWAP_METHOD   0
#endif

// EIP-96 Packet Engine Interrupt Level
// When set to 1 interrupt output will be a level signal that will be active
// until it is cleared by the Host.
// When set to 0b the interrupt will be pulsed for one clock cycle.
#ifndef EIP97_GLOBAL_EIP96_INTERRUPT_LEVEL_FLAG
#define EIP97_GLOBAL_EIP96_INTERRUPT_LEVEL_FLAG     0
#endif

// EIP-96 Packet Engine Timeout Counter
// Enables the time-out counter that generates an error in case of a „hang‟
// situation. If this bit is not set, the time-out error can never occur.
#ifndef EIP97_GLOBAL_EIP96_TIMEOUT_CNTR_FLAG
#define EIP97_GLOBAL_EIP96_TIMEOUT_CNTR_FLAG     1
#endif

// EIP-96 Packet Engine Input DMA Error Interrupt Enable
#ifndef EIP97_GLOBAL_EIP96_IN_DMA_INT_FLAG
#define EIP97_GLOBAL_EIP96_IN_DMA_INT_FLAG     1
#endif

// EIP-96 Packet Engine Output DMA Error Interrupt Enable
#ifndef EIP97_GLOBAL_EIP96_OUT_DMA_INT_FLAG
#define EIP97_GLOBAL_EIP96_OUT_DMA_INT_FLAG     1
#endif

// EIP-96 Packet Engine Packet Processign Error Interrupt Enable
#ifndef EIP97_GLOBAL_EIP96_PKT_PROC_INT_FLAG
#define EIP97_GLOBAL_EIP96_PKT_PROC_INT_FLAG     1
#endif

// EIP-96 Packet Engine Packet Timeout Error Interrupt Enable
#ifndef EIP97_GLOBAL_EIP96_PKT_TIMEOUT_INT_FLAG
#define EIP97_GLOBAL_EIP96_PKT_TIMEOUT_INT_FLAG     1
#endif

// EIP-96 Packet Engine Fatal Error Interrupt Enable
#ifndef EIP97_GLOBAL_EIP96_FATAL_ERROR_INT_FLAG
#define EIP97_GLOBAL_EIP96_FATAL_ERROR_INT_FLAG     1
#endif

// EIP-96 Packet Engine Interrupt Output Enable
#ifndef EIP97_GLOBAL_EIP96_PE_OUT_INT_FLAG
#define EIP97_GLOBAL_EIP96_PE_OUT_INT_FLAG     1
#endif

// EIP-96 Packet Engine hold output data
#ifndef EIP97_GLOBAL_EIP96_PE_HOLD_OUTPUT_DATA
#define EIP97_GLOBAL_EIP96_PE_HOLD_OUTPUT_DATA      0
#endif // EIP97_GLOBAL_EIP96_PE_HOLD_OUTPUT_DATA

// Define this parameter to enable the EIP-97 HW version check
//#define EIP97_GLOBAL_VERSION_CHECK_ENABLE

// Define this parameter in order to configure the DFE and DSE ring priorities
// Note: Some EIP-97 HW version do not have the DFE and DSE ring priority
//       configuration registers.
//#define EIP97_GLOBAL_DFE_DSE_PRIO_CONFIGURE

// Set this parameter to a correct value in top-level configuration file
// EIP-207s Classification Support, DMA Control base address
//#define EIP97_RC_BASE      0x37000

// Set this parameter to a correct value in top-level configuration file
// EIP-207s Classification Support, DMA Control base address
//#define EIP97_BASE         0x3FFF4

// Read cache type control (EIP197_MST_CTRL rd_cache field)
#ifndef EIP97_GLOBAL_RD_CACHE_VALUE
#define EIP97_GLOBAL_RD_CACHE_VALUE                 0
#endif

// Write cache type control (EIP197_MST_CTRL wr_cache field)
// Note: the buffering is enabled here by default
#ifndef EIP97_GLOBAL_WR_CACHE_VALUE
#define EIP97_GLOBAL_WR_CACHE_VALUE                 1
#endif

// Data read cache type control (data_cache_ctrl field in HIA_DFE_n_CFG reg)
#ifndef EIP97_GLOBAL_DFE_DATA_CACHE_CTRL
#define EIP97_GLOBAL_DFE_DATA_CACHE_CTRL            0
#endif

// Control read cache type control (ctrl_cache_ctrl field in HIA_DFE_n_CFG reg)
#ifndef EIP97_GLOBAL_DFE_CTRL_CACHE_CTRL
#define EIP97_GLOBAL_DFE_CTRL_CACHE_CTRL            0
#endif

// Data read cache type control (data_cache_ctrl field in HIA_DSE_n_CFG reg)
#ifndef EIP97_GLOBAL_DSE_DATA_CACHE_CTRL
#define EIP97_GLOBAL_DSE_DATA_CACHE_CTRL            0
#endif

// Maximum EIP-96 token size in 32-bit words: 2^EIP97_GLOBAL_MAX_TOKEN_SIZE
// Note: The EIP-96 Token Builder may not use larger tokens!
#ifndef EIP97_GLOBAL_MAX_TOKEN_SIZE
#define EIP97_GLOBAL_MAX_TOKEN_SIZE                     7
#endif

// Enable advance threshold mode for optimal performance and
// latency compensation in the internal engine buffers
//#define EIP97_GLOBAL_DFE_ADV_THRESH_MODE_FLAG           1

// Define this parameter for automatic calculation of the EIP-202 and EIP-206
// Global Control threshold values.
// If not defined then the statically configured values will be used.
//#define EIP97_GLOBAL_THRESH_CONFIG_AUTO

#ifndef EIP97_GLOBAL_THRESH_CONFIG_AUTO
// Example static EIP-202 and EIP-206 Global Control threshold configuration
// for optimal performance
//#define EIP97_GLOBAL_DFE_MIN_DATA_XFER_SIZE     5
//#define EIP97_GLOBAL_DFE_MAX_DATA_XFER_SIZE     9

//#define EIP97_GLOBAL_DSE_MIN_DATA_XFER_SIZE     7
//#define EIP97_GLOBAL_DSE_MAX_DATA_XFER_SIZE     8

//#define EIP97_GLOBAL_DFE_MIN_TOKEN_XFER_SIZE    5
//#define EIP97_GLOBAL_DFE_MAX_TOKEN_XFER_SIZE    EIP97_GLOBAL_MAX_TOKEN_SIZE
#endif // EIP97_GLOBAL_THRESH_CONFIG_AUTO

// Disable usage of the EIP-202 Ring Arbiter
//#define EIP202_RA_DISABLE

// Strict argument checking for the input parameters
// If required then define this parameter in the top-level configuration
//#define EIP97_GLOBAL_STRICT_ARGS

// Finite State Machine that can be used for verifying that the Driver Library
// API is called in a correct order
//#define EIP97_GLOBAL_DEBUG_FSM


#endif /* C_EIP97_GLOBAL_H_ */


/* end of file c_eip97_global.h */
