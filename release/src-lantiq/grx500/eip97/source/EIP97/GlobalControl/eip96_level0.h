/* eip96_level0.h
 *
 * EIP-96 Packet Engine Level0 Internal interface
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

#ifndef EIP96_LEVEL0_H_
#define EIP96_LEVEL0_H_


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // BIT definitions, bool, uint32_t

// EIP-96 Packet Engine HW interface
#include "eip96_hw_interface.h"

// Driver Framework Device API
#include "device_types.h"       // Device_Handle_t
#include "device_rw.h"          // Read32, Write32


/*----------------------------------------------------------------------------
 * Definitions and macros
 */


/*----------------------------------------------------------------------------
 * Local variables
 */


/*----------------------------------------------------------------------------
 * EIP96_Read32
 *
 * This routine writes to a Register location in the EIP-96.
 */
static inline uint32_t
EIP96_Read32(
        Device_Handle_t Device,
        const unsigned int Offset)
{
    return Device_Read32(Device, Offset);
}


/*----------------------------------------------------------------------------
 * EIP96_Write32
 *
 * This routine writes to a Register location in the EIP-96.
 */
static inline void
EIP96_Write32(
        Device_Handle_t Device,
        const unsigned int Offset,
        const uint32_t Value)
{
    Device_Write32(Device, Offset, Value);
}


static inline void
EIP96_EIP_REV_RD(
        Device_Handle_t Device,
        const unsigned int PEnr,
        uint8_t * const EipNumber,
        uint8_t * const ComplmtEipNumber,
        uint8_t * const HWPatchLevel,
        uint8_t * const MinHWRevision,
        uint8_t * const MajHWRevision)
{
    uint32_t RevRegVal;

    RevRegVal = EIP96_Read32(Device, EIP96_REG_VERSION(PEnr));

    *MajHWRevision     = (uint8_t)((RevRegVal >> 24) & MASK_4_BITS);
    *MinHWRevision     = (uint8_t)((RevRegVal >> 20) & MASK_4_BITS);
    *HWPatchLevel      = (uint8_t)((RevRegVal >> 16) & MASK_4_BITS);
    *ComplmtEipNumber  = (uint8_t)((RevRegVal >> 8)  & MASK_8_BITS);;
    *EipNumber         = (uint8_t)((RevRegVal)       & MASK_8_BITS);
}


static inline void
EIP96_OPTIONS_RD(
        Device_Handle_t Device,
        const unsigned int PEnr,
        bool * const fAES,
        bool * const fAESfb,
        bool * const fAESspeed,
        bool * const fDES,
        bool * const fDESfb,
        bool * const fDESspeed,
        uint8_t * const ARC4,
        bool * const fMD5,
        bool * const fSHA1,
        bool * const fSHA1speed,
        bool * const fSHA224_256,
        bool * const fSHA384_512,
        bool * const fXCBC_MAC,
        bool * const fCBC_MACspeed,
        bool * const fCBC_MACkeylens,
        bool * const fGHASH)
{
    uint32_t RegVal;

    RegVal = EIP96_Read32(Device, EIP96_REG_OPTIONS(PEnr));

    *fGHASH           = ((RegVal & BIT_30) != 0);
    *fCBC_MACkeylens  = ((RegVal & BIT_29) != 0);
    *fCBC_MACspeed    = ((RegVal & BIT_28) != 0);
    *fXCBC_MAC        = ((RegVal & BIT_27) != 0);
    *fSHA384_512      = ((RegVal & BIT_26) != 0);
    *fSHA224_256      = ((RegVal & BIT_25) != 0);
    *fSHA1speed       = ((RegVal & BIT_24) != 0);
    *fSHA1            = ((RegVal & BIT_23) != 0);
    *fMD5             = ((RegVal & BIT_22) != 0);
    *ARC4             = (uint8_t)((RegVal >> 18) & MASK_2_BITS);
    *fDESspeed        = ((RegVal & BIT_17) != 0);
    *fDESfb           = ((RegVal & BIT_16) != 0);
    *fDES             = ((RegVal & BIT_15) != 0);
    *fAESspeed        = ((RegVal & BIT_14) != 0);
    *fAESfb           = ((RegVal & BIT_13) != 0);
    *fAES             = ((RegVal & BIT_12) != 0);
}


static inline void
EIP96_TOKEN_CTRL_STAT_DEFAULT_WR(
        Device_Handle_t Device,
        const unsigned int PEnr)
{
    EIP96_Write32(Device,
                  EIP96_REG_TOKEN_CTRL_STAT(PEnr),
                  EIP96_REG_TOKEN_CTRL_STAT_DEFAULT);
}


static inline void
EIP96_TOKEN_CTRL_STAT_RD(
        Device_Handle_t Device,
        const unsigned int PEnr,
        uint8_t * const ActiveTokenCount,
        bool * const fTokenLocationAvailable,
        bool * const fResultTokenAvailable,
        bool * const fTokenReadActive,
        bool * const fContextCacheActive,
        bool * const fContextFetch,
        bool * const fResultContext,
        bool * const fProcessingHeld,
        bool * const fBusy)
{
    uint32_t RegVal;

    RegVal = EIP96_Read32(Device, EIP96_REG_TOKEN_CTRL_STAT(PEnr));

    *fBusy                   = ((RegVal & BIT_15) != 0);
    *fProcessingHeld         = ((RegVal & BIT_14) != 0);
    *fResultContext          = ((RegVal & BIT_7) != 0);
    *fContextFetch           = ((RegVal & BIT_6) != 0);
    *fContextCacheActive     = ((RegVal & BIT_5) != 0);
    *fTokenReadActive        = ((RegVal & BIT_4) != 0);
    *fResultTokenAvailable   = ((RegVal & BIT_3) != 0);
    *fTokenLocationAvailable = ((RegVal & BIT_2) != 0);
    *ActiveTokenCount        = (uint8_t)((RegVal) & MASK_2_BITS);
}


static inline void
EIP96_TOKEN_CTRL_STAT_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const bool fInterrupLevel,
        const bool fTimeoutCounterEnable)
{
    uint32_t RegVal = EIP96_REG_TOKEN_CTRL_STAT_DEFAULT;

    if(fInterrupLevel)
        RegVal |= BIT_17;

    if(fTimeoutCounterEnable)
        RegVal |= BIT_22;

    EIP96_Write32(Device, EIP96_REG_TOKEN_CTRL_STAT(PEnr), RegVal);
}


static inline void
EIP96_INT_CTRL_STAT_DEFAULT_WR(
        Device_Handle_t Device,
        const unsigned int PEnr)
{
    EIP96_Write32(Device,
                  EIP96_REG_INT_CTRL_STAT(PEnr),
                  EIP96_REG_INT_CTRL_STAT_DEFAULT);
}


static inline void
EIP96_INT_CTRL_STAT_CLEAR_ALL_IRQ_WR(
        Device_Handle_t Device,
        const unsigned int PEnr)
{
    EIP96_Write32(Device,
                  EIP96_REG_INT_CTRL_STAT(PEnr),
                  EIP96_REG_INT_CTRL_STAT_DEFAULT | BIT_14 | MASK_4_BITS);
}


static inline void
EIP96_INT_CTRL_STAT_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const bool fInputDMAErrorEnable,
        const bool fOutputDMAErrorEnable,
        const bool fPacketProcessingEnable,
        const bool fPacketTimeoutEnable,
        const bool fFatalErrorEnable,
        const bool fPEInterruptOutEnable)
{
    uint32_t RegVal = (uint32_t)EIP96_REG_INT_CTRL_STAT_DEFAULT;

    if(fInputDMAErrorEnable)
        RegVal |= BIT_16;

    RegVal &= (~BIT_17);
    if(fOutputDMAErrorEnable)
        RegVal |= BIT_17;

    RegVal &= (~BIT_18);
    if(fPacketProcessingEnable)
        RegVal |= BIT_18;

    RegVal &= (~BIT_19);
    if(fPacketTimeoutEnable)
        RegVal |= BIT_19;

    RegVal &= (~BIT_30);
    if(fFatalErrorEnable)
        RegVal |= BIT_30;

    RegVal &= (~BIT_31);
    if(fPEInterruptOutEnable)
        RegVal |= BIT_31;

    EIP96_Write32(Device, EIP96_REG_INT_CTRL_STAT(PEnr), RegVal);
}


static inline void
EIP96_CONTEXT_STAT_RD(
        Device_Handle_t Device,
        const unsigned int PEnr,
        uint16_t * const Error,
        uint8_t * const AvailableTokenCount,
        bool * const fActiveContext,
        bool * const fNextContext,
        bool * const fResultContext,
        bool * const fErrorRecovery)
{
    uint32_t RegVal;

    RegVal = EIP96_Read32(Device, EIP96_REG_CONTEXT_STAT(PEnr));

    *fErrorRecovery          = ((RegVal & BIT_21) != 0);
    *fResultContext          = ((RegVal & BIT_20) != 0);
    *fNextContext            = ((RegVal & BIT_19) != 0);
    *fActiveContext          = ((RegVal & BIT_18) != 0);
    *AvailableTokenCount     = (uint16_t)((RegVal >> 16) & MASK_2_BITS);
    *Error                   = (uint16_t)((RegVal)       & MASK_16_BITS);
}


static inline void
EIP96_OUT_TRANS_CTRL_STAT_RD(
        Device_Handle_t Device,
        const unsigned int PEnr,
        uint8_t * const AvailableWord32Count,
        uint8_t * const MinTransferWordCount,
        uint8_t * const MaxTransferWordCount,
        uint8_t * const TransferSizeMask)
{
    uint32_t RegVal;

    RegVal = EIP96_Read32(Device, EIP96_REG_OUT_TRANS_CTRL_STAT(PEnr));

    *TransferSizeMask      = (uint8_t)((RegVal >> 24) & MASK_8_BITS);
    *MaxTransferWordCount  = (uint8_t)((RegVal >> 16) & MASK_8_BITS);
    *MinTransferWordCount  = (uint8_t)((RegVal >> 8)  & MASK_8_BITS);
    *AvailableWord32Count  = (uint8_t)((RegVal)       & MASK_8_BITS);
}


static inline void
EIP96_OUT_BUF_CTRL_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const uint8_t HoldOutputData)
{
    uint32_t RegVal = EIP96_REG_OUT_BUF_CTRL_DEFAULT;

    RegVal |= (uint32_t)((((uint32_t)HoldOutputData) & MASK_5_BITS) << 3);

    EIP96_Write32(Device, EIP96_REG_OUT_BUF_CTRL(PEnr), RegVal);
}


static inline void
EIP96_INT_CTRL_STAT_RD(
        Device_Handle_t Device,
        const unsigned int PEnr,
        bool * const fInputDMAError,
        bool * const fOutputDMAError,
        bool * const fPacketProcessingError,
        bool * const fPacketTimeout,
        bool * const fFatalError,
        bool * const fPeInterruptOut,
        bool * const fInputDMAErrorEnabled,
        bool * const fOutputDMAErrorEnabled,
        bool * const fPacketProcessingEnabled,
        bool * const fPacketTimeoutEnabled,
        bool * const fFatalErrorEnabled,
        bool * const fPeInterruptOutEnabled)
{
    uint32_t RegVal;

    RegVal = EIP96_Read32(Device, EIP96_REG_INT_CTRL_STAT(PEnr));

    *fPeInterruptOutEnabled   = ((RegVal & BIT_31) != 0);
    *fFatalErrorEnabled       = ((RegVal & BIT_30) != 0);

    *fPacketTimeoutEnabled    = ((RegVal & BIT_19) != 0);
    *fPacketProcessingEnabled = ((RegVal & BIT_18) != 0);
    *fOutputDMAErrorEnabled   = ((RegVal & BIT_17) != 0);
    *fInputDMAErrorEnabled    = ((RegVal & BIT_16) != 0);
    *fPeInterruptOut          = ((RegVal & BIT_15) != 0);
    *fFatalError              = ((RegVal & BIT_14) != 0);

    *fPacketTimeout           = ((RegVal & BIT_3)  != 0);
    *fPacketProcessingError   = ((RegVal & BIT_2)  != 0);
    *fOutputDMAError          = ((RegVal & BIT_1)  != 0);
    *fInputDMAError           = ((RegVal & BIT_0)  != 0);
}


static inline void
EIP96_PRNG_CTRL_DEFAULT_WR(
        Device_Handle_t Device,
        const unsigned int PEnr)
{
    EIP96_Write32(Device,
                  EIP96_REG_PRNG_CTRL(PEnr),
                  EIP96_REG_PRNG_CTRL_DEFAULT);
}


static inline void
EIP96_PRNG_CTRL_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const bool fEnable,
        const bool fAutoMode)
{
    uint32_t RegVal = EIP96_REG_PRNG_CTRL_DEFAULT;

    if(fEnable)
        RegVal |= BIT_0;

    if(fAutoMode)
        RegVal |= BIT_1;

    EIP96_Write32(Device, EIP96_REG_PRNG_CTRL(PEnr), RegVal);
}


static inline void
EIP96_PRNG_STAT_RD(
        Device_Handle_t Device,
        const unsigned int PEnr,
        bool * const fBusy,
        bool * const fResultReady)
{
    uint32_t RegVal;

    RegVal = EIP96_Read32(Device, EIP96_REG_PRNG_STAT(PEnr));

    *fResultReady  = ((RegVal & BIT_1) != 0);
    *fBusy         = ((RegVal & BIT_0) != 0);
}


static inline void
EIP96_PRNG_SEED_L_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const uint32_t SeedLo)
{
    EIP96_Write32(Device, EIP96_REG_PRNG_SEED_L(PEnr), SeedLo);
}


static inline void
EIP96_PRNG_SEED_H_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const uint32_t SeedHi)
{
    EIP96_Write32(Device, EIP96_REG_PRNG_SEED_H(PEnr), SeedHi);
}


static inline void
EIP96_PRNG_KEY_0_L_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const uint32_t Key0Lo)
{
    EIP96_Write32(Device, EIP96_REG_PRNG_KEY_0_L(PEnr), Key0Lo);
}


static inline void
EIP96_PRNG_KEY_0_H_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const uint32_t Key0Hi)
{
    EIP96_Write32(Device, EIP96_REG_PRNG_KEY_0_H(PEnr), Key0Hi);
}


static inline void
EIP96_PRNG_KEY_1_L_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const uint32_t Key1Lo)
{
    EIP96_Write32(Device, EIP96_REG_PRNG_KEY_1_L(PEnr), Key1Lo);
}


static inline void
EIP96_PRNG_KEY_1_H_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const uint32_t Key1Hi)
{
    EIP96_Write32(Device, EIP96_REG_PRNG_KEY_1_H(PEnr), Key1Hi);
}


static inline void
EIP96_PRNG_LFSR_L_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const uint32_t LFSRLo)
{
    EIP96_Write32(Device, EIP96_REG_PRNG_LFSR_L(PEnr), LFSRLo);
}


static inline void
EIP96_PRNG_LFSR_H_WR(
        Device_Handle_t Device,
        const unsigned int PEnr,
        const uint32_t LFSRHi)
{
    EIP96_Write32(Device, EIP96_REG_PRNG_LFSR_H(PEnr), LFSRHi);
}


#endif /* EIP96_LEVEL0_H_ */


/* end of file eip96_level0.h */
