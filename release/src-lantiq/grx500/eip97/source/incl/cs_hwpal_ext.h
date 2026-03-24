/* cs_hwpal_ext.h
 *
 * SafeXcel-IP-97 (FPGA) PCI chip specific configuration parameters
 */

/*****************************************************************************
* Copyright (c) 2010-2014 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef CS_HWPAL_EXT_H_
#define CS_HWPAL_EXT_H_


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// For obtaining the IRQ number
#ifdef DRIVER_INTERRUPTS
#define HWPAL_INTERRUPTS
#endif

// FPGA board device ID.
#define HWPAL_DEVICE_ID             0x6018

// Xilinx PCI vendor ID
#define HWPAL_VENDOR_ID             0x10EE

//#define HWPAL_MAGIC_PCICONFIGSPACE  0xFF434647      // 43 46 47 = C F G

#define HWPAL_REMAP_ADDRESSES   ;

//#define HWPAL_DEVICE_TO_FIND       "PCI.0" // PCI Bar 0

// definition of static resources inside the PCI device
// Refer to the data sheet of device for the correct values
//                       Name         DevNr  Start    Last  Flags (see below)
#define HWPAL_DEVICES \
        HWPAL_DEVICE_ADD("EIP97_GLOBAL",  0, 0,        0x7ffff,  7),  \
        HWPAL_DEVICE_ADD("EIP202_CDR0",   0, 0x0000,   0x0fff,   7),  \
        HWPAL_DEVICE_ADD("EIP202_RDR0",   0, 0x0000,   0x0fff,   7),  \
        HWPAL_DEVICE_ADD("EIP202_CDR1",   0, 0x1000,   0x1fff,   7),  \
        HWPAL_DEVICE_ADD("EIP202_RDR1",   0, 0x1000,   0x1fff,   7),  \
        HWPAL_DEVICE_ADD("EIP202_CDR2",   0, 0x2000,   0x2fff,   7),  \
        HWPAL_DEVICE_ADD("EIP202_RDR2",   0, 0x2000,   0x2fff,   7),  \
        HWPAL_DEVICE_ADD("EIP202_CDR3",   0, 0x3000,   0x3fff,   7),  \
        HWPAL_DEVICE_ADD("EIP202_RDR3",   0, 0x3000,   0x3fff,   7),  \
        HWPAL_DEVICE_ADD("EIP201_GLOBAL", 0, 0xf800,   0xf8ff,   7),  \
        HWPAL_DEVICE_ADD("EIP201_RING0",  0, 0xe000,   0xe0ff,   7),  \
        HWPAL_DEVICE_ADD("EIP201_RING1",  0, 0xd000,   0xd0ff,   7),  \
        HWPAL_DEVICE_ADD("EIP201_RING2",  0, 0xc000,   0xc0ff,   7),  \
        HWPAL_DEVICE_ADD("EIP201_RING3",  0, 0xb000,   0xb0ff,   7),  \
        HWPAL_DEVICE_ADD("EIP201_CS",     0, 0x37800,  0x378ff,  7),  

// Flags:
//   bit0 = Trace reads (requires HWPAL_TRACE_DEVICE_READ)
//   bit1 = Trace writes (requires HWPAL_TRACE_DEVICE_WRITE)
//   bit2 = Swap word endianess (requires HWPAL_DEVICE_ENABLE_SWAP)

//#define HWPAL_USE_MSI


#endif /* CS_HWPAL_EXT_H_ */


/* end of file cs_hwpal_ext.h */
