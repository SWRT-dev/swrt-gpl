/* cs_driver.h
 *
 * Top-level Product Configuration Settings.
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

#ifndef INCLUDE_GUARD_CS_DRIVER_H
#define INCLUDE_GUARD_CS_DRIVER_H

#include "cs_systemtestconfig.h"      // defines SYSTEMTEST_CONFIG_Cnn

// Host hardware platform specific extensions
#include "cs_driver_ext.h"


// Driver license for the module registration with the Linux kernel
#define DRIVER_LICENSE  "GPL"

// Driver name used for reporting
#define DRIVER_NAME     "ltq-eip97-drv"

// activate in case of endianess difference between CPU and EIP
// to ask driver to swap byte order of all control words
// we assume that if ARCH is not x86, then CPU is big endian
#ifndef ARCH_X86
//#define DRIVER_SWAPENDIAN
#endif //ARCH_X86

// Maximum number of Processing Engines (EIP-206) to use
#define DRIVER_MAX_NOF_PE_TO_USE                       1

// Number of Ring interfaces to use
#define DRIVER_MAX_NOF_RING_TO_USE                     CONFIG_LTQ_CRYPTO_MAX_RING_USED

// C0 = ARM, Overlapped, Interrupt coalescing, BB=Y, Perf=N, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C0
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
#define DRIVER_INTERRUPTS
#define DRIVER_INTERRUPT_COALESCING
#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#define DRIVER_PEC_MAX_SAS         600
#define DRIVER_PEC_MAX_PACKETS     32
#define DRIVER_MAX_PECLOGICDESCR   32
#endif

// C1 = ARM, Overlapped, Polling, BB=N, Perf=N, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C1
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
//#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#endif

// C2 = ARM, Overlapped, Interrupts, BB=N, Perf=N, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C2
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#endif

// C3 = ARM, Separate, Polling, BB=N, Perf=N, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C3
//#define DRIVER_PE_TCM
#define DRIVER_PE_ARM_SEPARATE
//#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#endif

// C4 = ARM, Overlapped, Polling, BB=Yes, Perf=N, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C4
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
//#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#endif

// C5 = ARM, Overlapped, Interrupts, BB=Yes, Perf=N, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C5
//#define DRIVER_PE_TCM
#define DRIVER_PE_ARM_SEPARATE
#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
#define DRIVER_SCATTERGATHER
//#define DRIVER_ENABLE_SWAP_SLAVE
//#define DRIVER_ENABLE_SWAP_MASTER
//#define DRIVER_SWAPENDIAN
#define DRIVER_PEC_MAX_SAS         200
#define DRIVER_PEC_MAX_PACKETS     1500
#define DRIVER_MAX_PECLOGICDESCR   1500
#endif

// C6 = ARM, Separate, Polling, BB=Yes, Perf=N, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C6
//#define DRIVER_PE_TCM
#define DRIVER_PE_ARM_SEPARATE
#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#endif

// C8 = ARM, Polling, Overlapped, BB=N, Perf=Yes, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C8
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
//#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#define DRIVER_PEC_MAX_SAS         600
#define DRIVER_PEC_MAX_PACKETS     32
#define DRIVER_MAX_PECLOGICDESCR   32
#endif

// C9 = ARM, Interrupts, Overlapped, BB=N, Perf=Yes, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C9
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#define DRIVER_PEC_MAX_SAS         600
#define DRIVER_PEC_MAX_PACKETS     32
#define DRIVER_MAX_PECLOGICDESCR   32
#endif

// C10 = ARM, Polling, Separate, BB=N, Perf=Yes, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C10
//#define DRIVER_PE_TCM
#define DRIVER_PE_ARM_SEPARATE
//#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#endif

// C11 = ARM, Polling, Overlapped, BB=Yes, Perf=Yes, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C11
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
//#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#endif

// C12 = ARM, Interrupts, Overlapped, BB=Yes, Perf=Yes, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C12
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#endif

// C13 = ARM, Polling, Separate, BB=Yes, Perf=Yes, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C13
//#define DRIVER_PE_TCM
#define DRIVER_PE_ARM_SEPARATE
//#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#endif

// C14 = ARM, Polling, Overlapped, BB=N, Perf=N, SG=Yes
#ifdef SYSTEMTEST_CONFIGURATION_C14
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
//#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
#define DRIVER_SCATTERGATHER
#define DRIVER_PEC_MAX_PACKETS     64
#define DRIVER_MAX_PECLOGICDESCR   64
#endif

// C15 = ARM, Interrupts, Overlapped, BB=N, Perf=N, SG=Yes
#ifdef SYSTEMTEST_CONFIGURATION_C15
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
#define DRIVER_SCATTERGATHER
#define DRIVER_PEC_MAX_PACKETS     64
#define DRIVER_MAX_PECLOGICDESCR   64
#endif

// C16 = ARM, Polling, Separate, BB=N, Perf=N, SG=Yes
#ifdef SYSTEMTEST_CONFIGURATION_C16
//#define DRIVER_PE_TCM
#define DRIVER_PE_ARM_SEPARATE
//#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
#define DRIVER_SCATTERGATHER
#define DRIVER_PEC_MAX_PACKETS     64
#define DRIVER_MAX_PECLOGICDESCR   64
#endif

// C17 = ARM, Interrupt + Coalescing, Overlapped, BB=N, Perf=Yes, SG=N
#ifdef SYSTEMTEST_CONFIGURATION_C17
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
#define DRIVER_INTERRUPTS
#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
#define DRIVER_PEC_MAX_SAS           600
#define DRIVER_PEC_MAX_PACKETS       32
#define DRIVER_MAX_PECLOGICDESCR     32
#endif

// C18 = ARM, Polling, Overlapped, BB=N, Perf=No, SG=N Byteswap on
#ifdef SYSTEMTEST_CONFIGURATION_C18
//#define DRIVER_PE_TCM
//#define DRIVER_PE_ARM_SEPARATE
//#define DRIVER_INTERRUPTS
//#define DRIVER_INTERRUPT_COALESCING
//#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
//#define DRIVER_SCATTERGATHER
// PLB FPGA does not allow for endianness conversion
// in the Board Control device slave interface required on x86,
// so just disable the test
#ifndef EIP97_BUS_VERSION_PLB
#define DRIVER_SWAPENDIAN         // Host and device have different endianness
#ifndef ARCH_X86
#undef  DRIVER_SWAPENDIAN         // Switch off byte swap by the host processor
#endif //ARCH_X86
// Enable byte swap by the Engine slave interface
#define DRIVER_ENABLE_SWAP_SLAVE
// Enable byte swap by the Engine master interface
#define DRIVER_ENABLE_SWAP_MASTER
#endif // not EIP97_BUS_VERSION_PLB
#endif // SYSTEMTEST_CONFIGURATION_C18

#ifndef DRIVER_PEC_MAX_SAS
#define DRIVER_PEC_MAX_SAS                      60
#endif


// EIP-97 hardware specific extensions
#include "cs_driver_ext2.h"


#endif /* Include Guard */


/* end of file cs_driver.h */
