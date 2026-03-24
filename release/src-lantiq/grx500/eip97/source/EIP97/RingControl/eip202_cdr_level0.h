/* eip202_cdr_level0.h
 *
 * EIP-202 Internal interface
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

#ifndef EIP202_CDR_LEVEL0_H_
#define EIP202_CDR_LEVEL0_H_


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default configuration
#include "c_eip202_ring.h"

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // uint8_t, uint32_t

// Driver Framework Device API
#include "device_types.h"       // Device_Handle_t
#include "device_rw.h"          // Read32, Write32
#include "device_swap.h"        // Device_SwapEndian32


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// Read/Write register constants

/*****************************************************************************
 * Byte offsets of the EIP-202 HIA CDR registers
 *****************************************************************************/

#define EIP202_CDR_OFFS           4

// 2 KB MMIO space for one CDR instance
#define EIP202_CDR_BASE                   0x00000000
#define EIP202_CDR_RING_BASE_ADDR_LO      ((EIP202_CDR_BASE) + \
                                            (0x00 * EIP202_CDR_OFFS))
#define EIP202_CDR_RING_BASE_ADDR_HI      ((EIP202_CDR_BASE) + \
                                            (0x01 * EIP202_CDR_OFFS))
#define EIP202_CDR_DATA_BASE_ADDR_LO      ((EIP202_CDR_BASE) + \
                                            (0x02 * EIP202_CDR_OFFS))
#define EIP202_CDR_DATA_BASE_ADDR_HI      ((EIP202_CDR_BASE) + \
                                            (0x03 * EIP202_CDR_OFFS))
#define EIP202_CDR_ATOK_BASE_ADDR_LO      ((EIP202_CDR_BASE) + \
                                            (0x04 * EIP202_CDR_OFFS))
#define EIP202_CDR_ATOK_BASE_ADDR_HI      ((EIP202_CDR_BASE) + \
                                            (0x05 * EIP202_CDR_OFFS))
#define EIP202_CDR_RING_SIZE              ((EIP202_CDR_BASE) + \
                                            (0x06 * EIP202_CDR_OFFS))
#define EIP202_CDR_DESC_SIZE              ((EIP202_CDR_BASE) + \
                                            (0x07 * EIP202_CDR_OFFS))
#define EIP202_CDR_CFG                    ((EIP202_CDR_BASE) + \
                                            (0x08 * EIP202_CDR_OFFS))
#define EIP202_CDR_DMA_CFG                ((EIP202_CDR_BASE) + \
                                            (0x09 * EIP202_CDR_OFFS))
#define EIP202_CDR_THRESH                 ((EIP202_CDR_BASE) + \
                                            (0x0A * EIP202_CDR_OFFS))
#define EIP202_CDR_COUNT                  ((EIP202_CDR_BASE) + \
                                            (0x0B * EIP202_CDR_OFFS))
#define EIP202_CDR_POINTER                ((EIP202_CDR_BASE) + \
                                            (0x0D * EIP202_CDR_OFFS))
#define EIP202_CDR_STAT                   ((EIP202_CDR_BASE) + \
                                            (0x0F * EIP202_CDR_OFFS))

// Default EIP202_CDR_x register values
#define EIP202_CDR_RING_BASE_ADDR_LO_DEFAULT       0x00000000
#define EIP202_CDR_RING_BASE_ADDR_HI_DEFAULT       0x00000000
#define EIP202_CDR_DATA_BASE_ADDR_LO_DEFAULT       0x00000000
#define EIP202_CDR_DATA_BASE_ADDR_HI_DEFAULT       0x00000000
#define EIP202_CDR_ATOK_BASE_ADDR_LO_DEFAULT       0x00000000
#define EIP202_CDR_ATOK_BASE_ADDR_HI_DEFAULT       0x00000000
#define EIP202_CDR_RING_SIZE_DEFAULT               0x00000000
#define EIP202_CDR_DESC_SIZE_DEFAULT               0x00000000
#define EIP202_CDR_CFG_DEFAULT                     0x00000000
#define EIP202_CDR_DMA_CFG_DEFAULT                 0x01000000
#define EIP202_CDR_THRESH_DEFAULT                  0x00000000
#define EIP202_CDR_COUNT_DEFAULT                   0x00000000
#define EIP202_CDR_POINTER_DEFAULT                 0x00000000
// Ignore the CD FIFO size after reset for this register default value for now
#define EIP202_CDR_STAT_DEFAULT                    0x00000000


/*----------------------------------------------------------------------------
 * Local variables
 */


/*----------------------------------------------------------------------------
 * EIP202_CDR_Read32
 *
 * This routine writes to a Register location in the EIP-202 CDR.
 */
static inline uint32_t
EIP202_CDR_Read32(
        Device_Handle_t Device,
        const unsigned int Offset)
{
    return Device_Read32(Device, Offset);
}


/*----------------------------------------------------------------------------
 * EIP202_CDR_Write32
 *
 * This routine writes to a Register location in the EIP-202 CDR.
 */
static inline void
EIP202_CDR_Write32(
        Device_Handle_t Device,
        const unsigned int Offset,
        const uint32_t Value)
{
    Device_Write32(Device, Offset, Value);
}


static inline void
EIP202_CDR_STAT_RD(
        Device_Handle_t Device,
        bool * const fDMAErrorIrq,
        bool * const fTreshIrq,
        bool * const fErrorIrq,
        bool * const fOuflowIrq,
        bool * const fTimeoutIrq,
        uint16_t * const CDFIFOWordCount)
{
    uint32_t RegVal;

    RegVal = EIP202_CDR_Read32(Device, EIP202_CDR_STAT);

    *fDMAErrorIrq      = ((RegVal & BIT_0) != 0);
    *fTreshIrq         = ((RegVal & BIT_1) != 0);
    *fErrorIrq         = ((RegVal & BIT_2) != 0);
    *fOuflowIrq        = ((RegVal & BIT_3) != 0);
    *fTimeoutIrq       = ((RegVal & BIT_4) != 0);
    *CDFIFOWordCount   = (uint16_t)((RegVal >> 16) & MASK_12_BITS);
}


static inline void
EIP202_CDR_STAT_FIFO_SIZE_RD(
        Device_Handle_t Device,
        uint16_t * const CDFIFOWordCount)
{
    uint32_t RegVal;

    RegVal = EIP202_CDR_Read32(Device, EIP202_CDR_STAT);

    *CDFIFOWordCount   = (uint16_t)((RegVal >> 16) & MASK_12_BITS);
}


static inline void
EIP202_CDR_STAT_CLEAR_ALL_IRQ_WR(
        Device_Handle_t Device)
{
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_STAT,
                       (uint32_t)(EIP202_CDR_STAT_DEFAULT | MASK_5_BITS));
}


static inline void
EIP202_CDR_RING_BASE_ADDR_LO_DEFAULT_WR(
        Device_Handle_t Device)
{
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_RING_BASE_ADDR_LO,
                       EIP202_CDR_RING_BASE_ADDR_LO_DEFAULT);
}


static inline void
EIP202_CDR_RING_BASE_ADDR_LO_WR(
        Device_Handle_t Device,
        const uint32_t LowAddr)
{
    EIP202_CDR_Write32(Device, EIP202_CDR_RING_BASE_ADDR_LO, LowAddr);
}


static inline void
EIP202_CDR_RING_BASE_ADDR_HI_DEFAULT_WR(
        Device_Handle_t Device)
{
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_RING_BASE_ADDR_HI,
                       EIP202_CDR_RING_BASE_ADDR_HI_DEFAULT);
}


static inline void
EIP202_CDR_RING_BASE_ADDR_HI_WR(
        Device_Handle_t Device,
        const uint32_t HiAddr)
{
    EIP202_CDR_Write32(Device, EIP202_CDR_RING_BASE_ADDR_HI, HiAddr);
}


static inline void
EIP202_CDR_RING_SIZE_DEFAULT_WR(
        Device_Handle_t Device)
{
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_RING_SIZE,
                       EIP202_CDR_RING_SIZE_DEFAULT);
}


static inline void
EIP202_CDR_RING_SIZE_WR(
        Device_Handle_t Device,
        const uint32_t CDRWordCount)
{
    uint32_t RegVal = EIP202_CDR_RING_SIZE_DEFAULT;

    RegVal |= ((((uint32_t)CDRWordCount) & MASK_22_BITS) << 2);

    EIP202_CDR_Write32(Device, EIP202_CDR_RING_SIZE, RegVal);
}


static inline void
EIP202_CDR_DESC_SIZE_DEFAULT_WR(
        Device_Handle_t Device)
{
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_DESC_SIZE,
                       EIP202_CDR_DESC_SIZE_DEFAULT);
}


static inline void
EIP202_CDR_DESC_SIZE_WR(
        Device_Handle_t Device,
        const uint8_t DscrWordCount,
        const uint8_t DscrOffsWordCount,
        const bool fATPToToken,
        const bool fATP,
        const bool f64bit)
{
    uint32_t RegVal = EIP202_CDR_DESC_SIZE_DEFAULT;

    if(f64bit)
        RegVal |= BIT_31;

    if(fATP)
        RegVal |= BIT_30;

    if(fATPToToken)
        RegVal |= BIT_29;

    RegVal |= ((((uint32_t)DscrOffsWordCount) & MASK_8_BITS) << 16);
    RegVal |= ((((uint32_t)DscrWordCount)     & MASK_8_BITS)      );

    EIP202_CDR_Write32(Device, EIP202_CDR_DESC_SIZE, RegVal);
}


static inline void
EIP202_CDR_CFG_DEFAULT_WR(
        Device_Handle_t Device)
{
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_CFG,
                       EIP202_CDR_CFG_DEFAULT);
}


static inline void
EIP202_CDR_CFG_WR(
        Device_Handle_t Device,
        const uint16_t CDFetchWordCount,
        const uint16_t CDFetchThreshWordCount)
{
    uint32_t RegVal = EIP202_CDR_CFG_DEFAULT;

    RegVal |= ((((uint32_t)CDFetchThreshWordCount) & MASK_10_BITS) << 16);
    RegVal |= ((((uint32_t)CDFetchWordCount)       & MASK_16_BITS)      );

    EIP202_CDR_Write32(Device, EIP202_CDR_CFG, RegVal);
}


static inline void
EIP202_CDR_DMA_CFG_DEFAULT_WR(
        Device_Handle_t Device)
{
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_DMA_CFG,
                       EIP202_CDR_DMA_CFG_DEFAULT);
}


static inline void
EIP202_CDR_DMA_CFG_WR(
        Device_Handle_t Device,
        const uint8_t CDSwap,
        const uint8_t DataSwap,
        const uint8_t TokenSwap,
        const bool fBuf,
        const uint8_t WrCache,
        const uint8_t RdCache)
{
    uint32_t RegVal = EIP202_CDR_DMA_CFG_DEFAULT;

    RegVal &= (~BIT_24);
    if(fBuf)
        RegVal |= BIT_24;

    RegVal |= ((((uint32_t)RdCache)   & MASK_3_BITS) << 29);
    RegVal |= ((((uint32_t)WrCache)   & MASK_3_BITS) << 25);
    RegVal |= ((((uint32_t)TokenSwap) & MASK_4_BITS) << 16);
    RegVal |= ((((uint32_t)DataSwap)  & MASK_4_BITS) << 8);
    RegVal |= ((((uint32_t)CDSwap)    & MASK_4_BITS));

    EIP202_CDR_Write32(Device, EIP202_CDR_DMA_CFG, RegVal);
}


static inline void
EIP202_CDR_THRESH_DEFAULT_WR(
        Device_Handle_t Device)
{
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_THRESH,
                       EIP202_CDR_THRESH_DEFAULT);
}


static inline void
EIP202_CDR_THRESH_WR(
        Device_Handle_t Device,
        const uint32_t CDThreshWordCount,
        const uint8_t CDTimeout)
{
    uint32_t RegVal = EIP202_CDR_THRESH_DEFAULT;

    RegVal |= ((((uint32_t)CDTimeout) & MASK_8_BITS) << 24);
    RegVal |= (CDThreshWordCount            & MASK_22_BITS);

    EIP202_CDR_Write32(Device, EIP202_CDR_THRESH, RegVal);
}


static inline void
EIP202_CDR_COUNT_DEFAULT_WR(
        Device_Handle_t Device)
{
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_COUNT,
                       EIP202_CDR_COUNT_DEFAULT);
}


static inline void
EIP202_CDR_COUNT_WR(
        Device_Handle_t Device,
        const uint16_t CDCount,
        const bool fClearCount)
{
    uint32_t RegVal = EIP202_CDR_COUNT_DEFAULT;

    if(fClearCount)
        RegVal |= BIT_31;

    RegVal |= ((((uint32_t)CDCount) & MASK_14_BITS) << 2);

    EIP202_CDR_Write32(Device, EIP202_CDR_COUNT, RegVal);
}


static inline void
EIP202_CDR_COUNT_RD(
        Device_Handle_t Device,
        uint32_t * const CDWordCount_p)
{
    uint32_t RegVal;

    RegVal = EIP202_CDR_Read32(Device, EIP202_CDR_COUNT);

    *CDWordCount_p   = (uint32_t)((RegVal >> 2) & MASK_22_BITS);
}


static inline void
EIP202_CDR_POINTER_DEFAULT_WR(
        Device_Handle_t Device)
{
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_POINTER,
                       EIP202_CDR_POINTER_DEFAULT);
}


static inline void
EIP202_CDR_POINTER_RD(
        Device_Handle_t Device,
        uint32_t * const CDRPointer_p)
{
    uint32_t RegVal;

    RegVal = EIP202_CDR_Read32(Device, EIP202_CDR_POINTER);

    *CDRPointer_p   = (uint32_t)((RegVal >> 2) & MASK_22_BITS);
}


#endif /* EIP202_CDR_LEVEL0_H_ */


/* end of file eip202_cdr_level0.h */
