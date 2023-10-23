/* eip97_global_init.c
 *
 * EIP-97 Global Control Driver Library
 * Initialization Module
 */

/*****************************************************************************
* Copyright (c) 2008-2013 INSIDE Secure B.V. All Rights Reserved.
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

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */

#include "eip97_global_init.h"


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default configuration
#include "c_eip97_global.h"

// Driver Framework Basic Definitions API
#include "basic_defs.h"             // uint32_t

// Driver Framework Device API
#include "device_types.h"           // Device_Handle_t

// EIP-97 Global Control Driver Library Internal interfaces
#include "eip97_global_internal.h"
#include "eip97_global_level0.h"       // EIP-97 Level 0 macros
#include "eip202_global_level0.h"      // EIP-202 Level 0 macros
#include "eip206_level0.h"             // EIP-206 Level 0 macros
#include "eip96_level0.h"              // EIP-96 Level 0 macros
#include "eip97_global_fsm.h"          // State machine


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// Maximum number of Packet Engines that should be used
// Driver Library will check the maximum number supported run-time
#ifndef EIP97_GLOBAL_MAX_NOF_PE_TO_USE
#error "EIP97_GLOBAL_MAX_NOF_PE_TO_USE is not defined"
#endif

// Number of Ring interfaces
// Maximum number of Ring interfaces that should be used
// Driver Library will check the maximum number supported run-time
#ifndef EIP97_GLOBAL_MAX_NOF_RING_TO_USE
#error "EIP97_GLOBAL_MAX_NOF_RING_TO_USE is not defined"
#endif


/*----------------------------------------------------------------------------
 * Local variables
 */


/*----------------------------------------------------------------------------
 * EIP202Lib_Detect
 *
 * Checks the presence of EIP-202 HIA hardware. Returns true when found.
 */
static bool
EIP202Lib_Detect(
        const Device_Handle_t Device)
{
    uint32_t Value;

    Value = EIP202_Read32(Device, EIP202_G_REG_VERSION);
    if (!EIP202_REV_SIGNATURE_MATCH( Value ))
        return false;

    // read-write test one of the registers

#ifdef EIP97_GLOBAL_DFE_DSE_PRIO_CONFIGURE
    // Set MASK_4_BITS bits of the EIP202_DFE_REG_PRIO_0 register
    EIP202_DFE_PRIO_0_VALUE32_WR(Device, 0x8F8F);
    Value = EIP202_DFE_PRIO_0_VALUE32_RD(Device);
    if ((Value & MASK_4_BITS) != MASK_4_BITS)
        return false;

    // Clear MASK_4_BITS bits of the EIP202_DFE_REG_PRIO_0 register
    EIP202_DFE_PRIO_0_VALUE32_WR(Device, 0);
    Value = EIP202_DFE_PRIO_0_VALUE32_RD(Device);
    if ((Value & MASK_4_BITS) != 0)
       return false;
#endif // EIP97_GLOBAL_DFE_DSE_PRIO_CONFIGURE

    return true;
}


/*----------------------------------------------------------------------------
 * EIP206Lib_Detect
 *
 * Checks the presence of EIP-206 PE hardware. Returns true when found.
 */
static bool
EIP206Lib_Detect(
        const Device_Handle_t Device,
        const unsigned int PEnr)
{
    uint32_t Value;

    // No revision register for this HW version

    // read-write test one of the registers

    // Set MASK_8_BITS bits of the EIP206_OUT_REG_DBUF_TRESH register
    EIP206_Write32(Device,
                   EIP206_OUT_REG_DBUF_TRESH(PEnr),
                   MASK_8_BITS);
    Value = EIP206_Read32(Device,
                          EIP206_OUT_REG_DBUF_TRESH(PEnr));
    if ((Value & MASK_8_BITS) != MASK_8_BITS)
        return false;

    // Clear MASK_8_BITS bits of the EIP206_OUT_REG_DBUF_TRESH(PEnr) register
    EIP206_Write32(Device, EIP206_OUT_REG_DBUF_TRESH(PEnr), 0);
    Value = EIP206_Read32(Device, EIP206_OUT_REG_DBUF_TRESH(PEnr));
    if ((Value & MASK_8_BITS) != 0)
       return false;

    return true;
}


/*----------------------------------------------------------------------------
 * EIP96Lib_Detect
 *
 * Checks the presence of EIP-96 Engine hardware. Returns true when found.
 */
static bool
EIP96Lib_Detect(
        const Device_Handle_t Device,
        const unsigned int PEnr)
{
    uint32_t Value, DefaultValue;
    bool fSuccess = true;

    // No revision register for this HW version

    // Save the default register value
    DefaultValue = EIP96_Read32(Device,
                                EIP96_REG_CONTEXT_CTRL(PEnr));

    // read-write test one of the registers

    // Set MASK_8_BITS bits of the EIP96_REG_CONTEXT_CTRL register
    EIP96_Write32(Device,
                  EIP96_REG_CONTEXT_CTRL(PEnr),
                  MASK_8_BITS );
    Value = EIP96_Read32(Device, EIP96_REG_CONTEXT_CTRL(PEnr));
    if ((Value & MASK_8_BITS) != MASK_8_BITS)
        fSuccess = false;

    if( fSuccess )
    {
        // Clear MASK_8_BITS bits of the EIP96_REG_CONTEXT_CTRL register
        EIP96_Write32(Device, EIP96_REG_CONTEXT_CTRL(PEnr), 0);
        Value = EIP96_Read32(Device, EIP96_REG_CONTEXT_CTRL(PEnr));
        if ((Value & MASK_8_BITS) != 0)
            fSuccess = false;
    }

    // Restore the default register value
    EIP96_Write32(Device,
            EIP96_REG_CONTEXT_CTRL(PEnr),
                  DefaultValue );
    return fSuccess;
}


/*----------------------------------------------------------------------------
 * EIP97Lib_Detect
 *
 * Checks the presence of EIP-97 Engine hardware. Returns true when found.
 */
static bool
EIP97Lib_Detect(
        const Device_Handle_t Device)
{
#ifdef EIP97_GLOBAL_VERSION_CHECK_ENABLE
    uint32_t Value;

    // read and check the revision register
    Value = EIP97_Read32(Device, EIP97_REG_VERSION);
    if (!EIP97_REV_SIGNATURE_MATCH( Value ))
        return false;
#else
    IDENTIFIER_NOT_USED(Device);
#endif // EIP97_GLOBAL_VERSION_CHECK_ENABLE

    return true;
}


/*----------------------------------------------------------------------------
 * EIP202Lib_HWRevision_Get
 */
static void
EIP202Lib_HWRevision_Get(
        const Device_Handle_t Device,
        EIP202_Options_t * const Options_p,
        EIP_Version_t * const Version_p)
{
    EIP202_EIP_REV_RD(Device,
                      &Version_p->EipNumber,
                      &Version_p->ComplmtEipNumber,
                      &Version_p->HWPatchLevel,
                      &Version_p->MinHWRevision,
                      &Version_p->MajHWRevision);

    EIP202_OPTIONS_RD(Device,
                      &Options_p->NofRings,
                      &Options_p->NofPes,
                      &Options_p->fExpPlf,
                      &Options_p->CF_Size,
                      &Options_p->RF_Size,
                      &Options_p->HostIfc,
                      &Options_p->DMA_Len,
                      &Options_p->HDW,
                      &Options_p->TgtAlign,
                      &Options_p->fAddr64);
}


/*----------------------------------------------------------------------------
 * EIP202Lib_Endianness_Slave_Configure
 *
 * Configure Endianness Conversion method
 * for the EIP-202 slave (MMIO) interface
 *
 */
static inline EIP97_Global_Error_t
EIP202Lib_Endianness_Slave_Configure(
        const Device_Handle_t Device)
{
#ifdef EIP97_GLOBAL_ENABLE_SWAP_REG_DATA
    uint32_t Value;

    // Read and check the revision register
    Value = EIP202_Read32(Device, EIP202_G_REG_VERSION);
    if (!EIP202_REV_SIGNATURE_MATCH( Value ))
    {
        // No match, try to enable the Slave interface byte swap
        // Must be done via EIP-202 HIA GLobal
        EIP202_MST_CTRL_BYTE_SWAP_UPDATE(Device, true);

        // Read and check the revision register again
        Value = EIP202_Read32(Device, EIP202_G_REG_VERSION);
        if (!EIP202_REV_SIGNATURE_MATCH( Value ))
            // Bail out if still not OK
            return EIP97_GLOBAL_UNSUPPORTED_FEATURE_ERROR;
    }

    return EIP97_GLOBAL_NO_ERROR;
#else
    IDENTIFIER_NOT_USED(Device);
    return EIP97_GLOBAL_NO_ERROR;
#endif // EIP97_GLOBAL_ENABLE_SWAP_REG_DATA
}


/*----------------------------------------------------------------------------
 * EIP96Lib_HWRevision_Get
 */
static void
EIP96Lib_HWRevision_Get(
        const Device_Handle_t Device,
        const unsigned int PEnr,
        EIP96_Options_t * const Options_p,
        EIP_Version_t * const Version_p)
{
    EIP96_EIP_REV_RD(Device,
                     PEnr,
                     &Version_p->EipNumber,
                     &Version_p->ComplmtEipNumber,
                     &Version_p->HWPatchLevel,
                     &Version_p->MinHWRevision,
                     &Version_p->MajHWRevision);

    EIP96_OPTIONS_RD(Device,
                     PEnr,
                     &Options_p->fAES,
                     &Options_p->fAESfb,
                     &Options_p->fAESspeed,
                     &Options_p->fDES,
                     &Options_p->fDESfb,
                     &Options_p->fDESspeed,
                     &Options_p->ARC4,
                     &Options_p->fMD5,
                     &Options_p->fSHA1,
                     &Options_p->fSHA1speed,
                     &Options_p->fSHA224_256,
                     &Options_p->fSHA384_512,
                     &Options_p->fXCBC_MAC,
                     &Options_p->fCBC_MACspeed,
                     &Options_p->fCBC_MACkeylens,
                     &Options_p->fGHASH);
}


/*----------------------------------------------------------------------------
 * EIP97Lib_HWRevision_Get
 */
static void
EIP97Lib_HWRevision_Get(
        const Device_Handle_t Device,
        EIP97_Options_t * const Options_p,
        EIP_Version_t * const Version_p)
{
    EIP97_EIP_REV_RD(Device,
                     &Version_p->EipNumber,
                     &Version_p->ComplmtEipNumber,
                     &Version_p->HWPatchLevel,
                     &Version_p->MinHWRevision,
                     &Version_p->MajHWRevision);

    EIP97_OPTIONS_RD(Device,
                     &Options_p->NofPes,
                     &Options_p->in_tbuf_size,
                     &Options_p->in_dbuf_size,
                     &Options_p->out_tbuf_size,
                     &Options_p->out_dbuf_size,
                     &Options_p->tg,
                     &Options_p->trc);
}


/*----------------------------------------------------------------------------
 * EIP97Lib_Reset_IsDone
 */
static EIP97_Global_Error_t
EIP97Lib_Reset_IsDone(
        const Device_Handle_t Device,
        volatile uint32_t * State_p,
        const unsigned int PEnr)
{
    uint8_t RingId;
    bool fResetDone = false;

    // Check for completion of all DMA transfers
    EIP202_DFE_TRD_STAT_RINGID_RD(Device, PEnr, &RingId);
    if(RingId == EIP202_DFE_TRD_REG_STAT_IDLE)
    {
        EIP202_DSE_TRD_STAT_RINGID_RD(Device, PEnr, &RingId);
        if(RingId == EIP202_DFE_TRD_REG_STAT_IDLE)
            fResetDone = true;
    }

    if(fResetDone)
    {
        // Take DFE thread out of reset
        EIP202_DFE_TRD_CTRL_DEFAULT_WR(Device, PEnr);

        // Take DSE thread out of reset
        EIP202_DSE_TRD_CTRL_DEFAULT_WR(Device, PEnr);

        // Restore the EIP-202 Master Control default configuration
        // this can change the endianness conversion for the Slave interface
#ifdef EIP97_GLOBAL_ENABLE_SWAP_REG_DATA
        // Configure the Slave interface byte order
        // Must be done via EIP-202 HIA Global
        EIP202_MST_CTRL_BYTE_SWAP_UPDATE(Device, false);
#endif // EIP97_GLOBAL_ENABLE_SWAP_REG_DATA

        // Transit to a new state
#ifdef EIP97_GLOBAL_DEBUG_FSM
        {
            EIP97_Global_Error_t rv;

            rv = EIP97_Global_State_Set((volatile EIP97_Global_State_t*)State_p,
                                        EIP97_GLOBAL_STATE_SW_RESET_DONE);
            if(rv != EIP97_GLOBAL_NO_ERROR)
                return EIP97_GLOBAL_ILLEGAL_IN_STATE;
        }
#endif // EIP97_GLOBAL_DEBUG_FSM
    }
    else
    {
#ifdef EIP97_GLOBAL_DEBUG_FSM
        {
            EIP97_Global_Error_t rv;

            // SW Reset is ongoing, retry later
            rv = EIP97_Global_State_Set((volatile EIP97_Global_State_t*)State_p,
                                        EIP97_GLOBAL_STATE_SW_RESET_START);
            if(rv != EIP97_GLOBAL_NO_ERROR)
                return EIP97_GLOBAL_ILLEGAL_IN_STATE;
        }
#endif // EIP97_GLOBAL_DEBUG_FSM

        return EIP97_GLOBAL_BUSY_RETRY_LATER;
    }

    return EIP97_GLOBAL_NO_ERROR;
}


/*----------------------------------------------------------------------------
 * EIP97_Global_Init
 */
EIP97_Global_Error_t
EIP97_Global_Init(
        EIP97_Global_IOArea_t * const IOArea_p,
        const Device_Handle_t Device)
{
    EIP97_Global_Capabilities_t Capabilities;
    volatile EIP97_True_IOArea_t * const TrueIOArea_p = IOAREA(IOArea_p);

    EIP97_GLOBAL_CHECK_POINTER(IOArea_p);

    // Configure Endianness Conversion method for slave (MMIO) interface
    if (EIP202Lib_Endianness_Slave_Configure(Device) != EIP97_GLOBAL_NO_ERROR)
        return EIP97_GLOBAL_UNSUPPORTED_FEATURE_ERROR;

    // Detect presence of EIP-97 HW hardware
    if (!EIP202Lib_Detect(Device) ||
        !EIP206Lib_Detect(Device, PE_DEFAULT_NR) ||
        !EIP96Lib_Detect(Device, PE_DEFAULT_NR) ||
        !EIP97Lib_Detect(Device))
        return EIP97_GLOBAL_UNSUPPORTED_FEATURE_ERROR;

    // Initialize the IO Area
    TrueIOArea_p->Device = Device;
    // Can also be EIP97_GLOBAL_STATE_HW_RESET_DONE
    TrueIOArea_p->State = (uint32_t)EIP97_GLOBAL_STATE_SW_RESET_DONE;

    EIP97Lib_HWRevision_Get(Device,
                            &Capabilities.EIP97_Options,
                            &Capabilities.EIP97_Version);

    EIP202Lib_HWRevision_Get(Device,
                             &Capabilities.EIP202_Options,
                             &Capabilities.EIP202_Version);

    EIP96Lib_HWRevision_Get(Device,
                            PE_DEFAULT_NR,
                            &Capabilities.EIP96_Options,
                            &Capabilities.EIP96_Version);

    // Check actual configuration HW against capabilities
    // Number of configure PE's
    if(Capabilities.EIP202_Options.NofPes < EIP97_GLOBAL_MAX_NOF_PE_TO_USE)
        return EIP97_GLOBAL_UNSUPPORTED_FEATURE_ERROR;

    // Number of configure ring interfaces
    if(Capabilities.EIP202_Options.NofRings < EIP97_GLOBAL_MAX_NOF_RING_TO_USE)
        return EIP97_GLOBAL_UNSUPPORTED_FEATURE_ERROR;

#ifdef EIP97_GLOBAL_DEBUG_FSM
        {
            EIP97_Global_Error_t rv;

            // Transit to a new state
            rv = EIP97_Global_State_Set(
                    (volatile EIP97_Global_State_t*)&TrueIOArea_p->State,
                    EIP97_GLOBAL_STATE_INITIALIZED);
            if(rv != EIP97_GLOBAL_NO_ERROR)
                return EIP97_GLOBAL_ILLEGAL_IN_STATE;
        }
#endif // EIP97_GLOBAL_DEBUG_FSM

    // Configure Endianness Conversion method for master (DMA) interface
#if defined(EIP97_GLOBAL_BYTE_SWAP_FLUE_DATA)    || \
    defined(EIP97_GLOBAL_BYTE_SWAP_FLOW_DATA)    || \
    defined(EIP97_GLOBAL_BYTE_SWAP_CONTEXT_DATA) || \
    defined(EIP97_GLOBAL_BYTE_SWAP_ARC4_CONTEXT_DATA)

    EIP97_MST_CTRL_WR(Device, PE_DEFAULT_NR, EIP97_GLOBAL_BYTE_SWAP_METHOD);

    EIP97_CS_DMA_CTRL2_WR(Device,
#ifdef EIP97_GLOBAL_BYTE_SWAP_FLUE_DATA
                          EIP97_GLOBAL_BYTE_SWAP_METHOD,
#else
                          0,
#endif
#ifdef EIP97_GLOBAL_BYTE_SWAP_FLOW_DATA
                          EIP97_GLOBAL_BYTE_SWAP_METHOD,
#else
                          0,
#endif
#ifdef EIP97_GLOBAL_BYTE_SWAP_CONTEXT_DATA
                          EIP97_GLOBAL_BYTE_SWAP_METHOD,
#else
                          0,
#endif
#ifdef EIP97_GLOBAL_BYTE_SWAP_ARC4_CONTEXT_DATA
                          EIP97_GLOBAL_BYTE_SWAP_METHOD);
#else
                          0);
#endif
#endif

    // Configure EIP-202 HIA Global
    EIP202_MST_CTRL_BUS_BURST_SIZE_UPDATE(Device,
                                          EIP97_GLOBAL_BUS_BURST_SIZE);

    {
        uint8_t ipbuf, itbuf, opbuf;
        unsigned int i;

        // Configure EIP-202 HIA DFE Global
        ipbuf = MIN(Capabilities.EIP97_Options.in_dbuf_size +
                    Capabilities.EIP202_Options.HDW + 2,
                    Capabilities.EIP202_Options.DMA_Len - 1);

        itbuf = Capabilities.EIP97_Options.in_tbuf_size +
                    Capabilities.EIP202_Options.HDW + 2;
        if( itbuf > 7 )
            itbuf = 7;

        itbuf = MIN(itbuf, Capabilities.EIP202_Options.DMA_Len - 1);

        EIP202_DFE_CFG_WR(Device,
                          ipbuf - 1, // EIP97_GLOBAL_DFE_MIN_DATA_XFER_SIZE
                          ipbuf,     // EIP97_GLOBAL_DFE_MAX_DATA_XFER_SIZE
                          itbuf - 1, // EIP97_GLOBAL_DFE_MIN_TOKEN_XFER_SIZE
                          itbuf,     // EIP97_GLOBAL_DFE_MAX_TOKEN_XFER_SIZE
                          (EIP97_GLOBAL_DFE_AGGRESSIVE_DMA_FLAG == 1));

        // Configure EIP-202 HIA DSE Global
        opbuf = MIN(Capabilities.EIP97_Options.out_dbuf_size +
                    Capabilities.EIP202_Options.HDW + 2,
                    Capabilities.EIP202_Options.DMA_Len - 1);

        EIP202_DSE_CFG_WR(Device,
                          opbuf - 1, // EIP97_GLOBAL_DSE_MIN_DATA_XFER_SIZE
                          opbuf,     // EIP97_GLOBAL_DSE_MAX_DATA_XFER_SIZE
                          (EIP97_GLOBAL_DSE_AGGRESSIVE_DMA_FLAG == 1));

        // Configure EIP-206 Processing Engine
        for (i = 0; i < EIP97_GLOBAL_MAX_NOF_PE_TO_USE; i++)
        {
            EIP206_IN_DBUF_THRESH_WR(
                            Device,
                            i,
                            ipbuf -1, // EIP96_INPUT_DATA_BUFFER_MIN_THRESHOLD
                            ipbuf);   //EIP96_INPUT_DATA_BUFFER_MAX_THRESHOLD

            EIP206_IN_TBUF_THRESH_WR(
                            Device,
                            i,
                            itbuf - 1, // EIP96_COMMAND_TOKEN_BUFFER_MIN_THRESHOLD
                            itbuf);    // EIP96_COMMAND_TOKEN_BUFFER_MAX_THRESHOLD

            EIP206_OUT_DBUF_THRESH_WR(
                            Device,
                            i,
                            opbuf -1 , // EIP96_OUTPUT_DATA_BUFFER_MIN_THRESHOLD
                            opbuf);    // EIP96_OUTPUT_DATA_BUFFER_MAX_THRESHOLD

            // Configure EIP-96 Packet Engine
            EIP96_TOKEN_CTRL_STAT_WR(
                    Device,
                    i,
                    (EIP97_GLOBAL_EIP96_INTERRUPT_LEVEL_FLAG == 0) ? false : true,
                    (EIP97_GLOBAL_EIP96_TIMEOUT_CNTR_FLAG == 0) ? false : true);

            EIP96_INT_CTRL_STAT_WR(
                    Device,
                    i,
                    (EIP97_GLOBAL_EIP96_IN_DMA_INT_FLAG == 0) ? false : true,
                    (EIP97_GLOBAL_EIP96_OUT_DMA_INT_FLAG == 0) ? false : true,
                    (EIP97_GLOBAL_EIP96_PKT_PROC_INT_FLAG == 0) ? false : true,
                    (EIP97_GLOBAL_EIP96_PKT_TIMEOUT_INT_FLAG == 0) ? false : true,
                    (EIP97_GLOBAL_EIP96_FATAL_ERROR_INT_FLAG == 0) ? false : true,
                    (EIP97_GLOBAL_EIP96_PE_OUT_INT_FLAG == 0) ? false : true);

            EIP96_OUT_BUF_CTRL_WR(
                    Device,
                    i,
                    EIP97_GLOBAL_EIP96_PE_HOLD_OUTPUT_DATA);
        } // for
    }

    return EIP97_GLOBAL_NO_ERROR;
}


/*----------------------------------------------------------------------------
 * EIP97_Global_Reset
 */
EIP97_Global_Error_t
EIP97_Global_Reset(
        EIP97_Global_IOArea_t * const IOArea_p,
        const Device_Handle_t Device)
{
    volatile EIP97_True_IOArea_t * const TrueIOArea_p = IOAREA(IOArea_p);
    unsigned int i;

    EIP97_GLOBAL_CHECK_POINTER(IOArea_p);

    // Initialize the IO Area
    TrueIOArea_p->Device = Device;
    // Assume this function is called in the Unknown state but
    // this may not always be true
    TrueIOArea_p->State = (uint32_t)EIP97_GLOBAL_STATE_UNKNOWN;

    // Configure Endianness Conversion method for slave (MMIO) interface
    if (EIP202Lib_Endianness_Slave_Configure(Device) != EIP97_GLOBAL_NO_ERROR)
        return EIP97_GLOBAL_UNSUPPORTED_FEATURE_ERROR;

    // Restore the EIP-202 default configuration
    // Resets DFE thread and clears ring assignment
    for (i = 0; i < EIP97_GLOBAL_MAX_NOF_PE_TO_USE; i++)
        EIP202_DFE_TRD_CTRL_WR(Device, i, 0, false, true);

    // HIA DFE defaults
    EIP202_DFE_CFG_DEFAULT_WR(Device);

#ifdef EIP97_GLOBAL_DFE_DSE_PRIO_CONFIGURE
    EIP202_DFE_PRIO_0_DEFAULT_WR(Device);
    EIP202_DFE_PRIO_1_DEFAULT_WR(Device);
    EIP202_DFE_PRIO_2_DEFAULT_WR(Device);
    EIP202_DFE_PRIO_3_DEFAULT_WR(Device);
#endif // EIP97_GLOBAL_DFE_DSE_PRIO_CONFIGURE

    // Resets DSE thread and clears ring assignment
    for (i = 0; i < EIP97_GLOBAL_MAX_NOF_PE_TO_USE; i++)
        EIP202_DSE_TRD_CTRL_WR(Device, i, 0, false, true);

    // HIA DSE defaults
    EIP202_DSE_CFG_DEFAULT_WR(Device);

#ifdef EIP97_GLOBAL_DFE_DSE_PRIO_CONFIGURE
    EIP202_DSE_PRIO_0_DEFAULT_WR(Device);
    EIP202_DSE_PRIO_1_DEFAULT_WR(Device);
    EIP202_DSE_PRIO_2_DEFAULT_WR(Device);
    EIP202_DSE_PRIO_3_DEFAULT_WR(Device);
#endif // EIP97_GLOBAL_DFE_DSE_PRIO_CONFIGURE

    for (i = 0; i < EIP97_GLOBAL_MAX_NOF_PE_TO_USE; i++)
    {
        // Restore the EIP-206 default configuration
        EIP206_IN_DBUF_THRESH_DEFAULT_WR(Device, i);
        EIP206_IN_TBUF_THRESH_DEFAULT_WR(Device, i);
        EIP206_OUT_DBUF_THRESH_DEFAULT_WR(Device, i);
        EIP206_OUT_TBUF_THRESH_DEFAULT_WR(Device, i);

        // Restore the EIP-96 default configuration
        EIP96_TOKEN_CTRL_STAT_DEFAULT_WR(Device, i);

        // Clear all pending EIP-96 interrupts
        EIP96_INT_CTRL_STAT_CLEAR_ALL_IRQ_WR(Device, i);
    }

    // Check if Global SW Reset is done
    for (i = 0; i < EIP97_GLOBAL_MAX_NOF_PE_TO_USE; i++)
    {
        EIP97_Global_Error_t EIP97_Rc;

        EIP97_Rc =
            EIP97Lib_Reset_IsDone(Device, &TrueIOArea_p->State, i);

        if (EIP97_Rc != EIP97_GLOBAL_NO_ERROR)
            return EIP97_Rc;
    }

    return EIP97_GLOBAL_NO_ERROR;
}


/*----------------------------------------------------------------------------
 * EIP97_Global_Reset_IsDone
 */
EIP97_Global_Error_t
EIP97_Global_Reset_IsDone(
        EIP97_Global_IOArea_t * const IOArea_p)
{
    Device_Handle_t Device;
    volatile EIP97_True_IOArea_t * const TrueIOArea_p = IOAREA(IOArea_p);
    unsigned int i;

    EIP97_GLOBAL_CHECK_POINTER(IOArea_p);

    Device = TrueIOArea_p->Device;

    // Check if Global SW Reset is done
    for (i = 0; i < EIP97_GLOBAL_MAX_NOF_PE_TO_USE; i++)
    {
        EIP97_Global_Error_t EIP97_Rc;

        EIP97_Rc =
            EIP97Lib_Reset_IsDone(Device, &TrueIOArea_p->State, i);

        if (EIP97_Rc != EIP97_GLOBAL_NO_ERROR)
            return EIP97_Rc;
    }

    return EIP97_GLOBAL_NO_ERROR;
}


/*----------------------------------------------------------------------------
 * EIP97_Global_HWRevision_Get
 */
EIP97_Global_Error_t
EIP97_Global_HWRevision_Get(
        EIP97_Global_IOArea_t * const IOArea_p,
        EIP97_Global_Capabilities_t * const Capabilities_p)
{
    Device_Handle_t Device;
    volatile EIP97_True_IOArea_t * const TrueIOArea_p = IOAREA(IOArea_p);

    EIP97_GLOBAL_CHECK_POINTER(IOArea_p);
    EIP97_GLOBAL_CHECK_POINTER(Capabilities_p);

    Device = TrueIOArea_p->Device;

    EIP202Lib_HWRevision_Get(Device,
                             &Capabilities_p->EIP202_Options,
                             &Capabilities_p->EIP202_Version);

    EIP96Lib_HWRevision_Get(Device,
                            PE_DEFAULT_NR,
                            &Capabilities_p->EIP96_Options,
                            &Capabilities_p->EIP96_Version);

    EIP97Lib_HWRevision_Get(Device,
                            &Capabilities_p->EIP97_Options,
                            &Capabilities_p->EIP97_Version);

    return EIP97_GLOBAL_NO_ERROR;
}


/*----------------------------------------------------------------------------
 * EIP97_Global_Configure
 */
EIP97_Global_Error_t
EIP97_Global_Configure(
        EIP97_Global_IOArea_t * const IOArea_p,
        const unsigned int PE_Number,
        const EIP97_Global_Ring_PE_Map_t * const RingPEMap_p)
{
    uint16_t RingMask;
    Device_Handle_t Device;
    volatile EIP97_True_IOArea_t * const TrueIOArea_p = IOAREA(IOArea_p);

    EIP97_GLOBAL_CHECK_POINTER(IOArea_p);
    EIP97_GLOBAL_CHECK_POINTER(RingPEMap_p);

    if(PE_Number >= EIP97_GLOBAL_MAX_NOF_PE_TO_USE)
        return EIP97_GLOBAL_ARGUMENT_ERROR;

    Device = TrueIOArea_p->Device;

    // Figure out which rings must be assigned to
    // DFE and DSE threads for PE_Number
    RingMask = (uint16_t)(RingPEMap_p->RingPE_Mask &
                          ((1 << EIP97_GLOBAL_MAX_NOF_RING_TO_USE)-1));

#ifdef EIP97_GLOBAL_DEBUG_FSM
    {
        EIP97_Global_Error_t rv;

        // Transit to a new state
        if(RingMask != 0)
        {
            rv = EIP97_Global_State_Set(
                    (volatile EIP97_Global_State_t*)&TrueIOArea_p->State,
                    EIP97_GLOBAL_STATE_ENABLED);
            if(rv != EIP97_GLOBAL_NO_ERROR)
                return EIP97_GLOBAL_ILLEGAL_IN_STATE;
        }
        else
        {
            rv = EIP97_Global_State_Set(
                    (volatile EIP97_Global_State_t*)&TrueIOArea_p->State,
                    EIP97_GLOBAL_STATE_INITIALIZED);
            if(rv != EIP97_GLOBAL_NO_ERROR)
                return EIP97_GLOBAL_ILLEGAL_IN_STATE;
        }
    }
#endif // EIP97_GLOBAL_DEBUG_FSM

    // Disable EIP-202 HIA DFE thread(s)
    EIP202_DFE_TRD_CTRL_WR(Device,
                           PE_Number,   // Thread Nr
                           0,
                           false,       // Disable thread
                           false);      // Do not reset thread

    // Disable EIP-202 HIA DSE thread(s)
    EIP202_DSE_TRD_CTRL_WR(Device,
                           PE_Number,   // Thread Nr
                           0,
                           false,       // Disable thread
                           false);      // Do not reset thread

#ifdef EIP97_GLOBAL_DFE_DSE_PRIO_CONFIGURE
    // Configure DFE ring priorities
    EIP202_DFE_PRIO_0_WR(
            Device,
            (RingPEMap_p->RingPrio_Mask & BIT_0) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_1) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_2) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_3) == 0 ? false : true);

    EIP202_DFE_PRIO_1_WR(
            Device,
            (RingPEMap_p->RingPrio_Mask & BIT_4) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_5) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_6) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_7) == 0 ? false : true);

    EIP202_DFE_PRIO_2_WR(
            Device,
            (RingPEMap_p->RingPrio_Mask & BIT_8) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_9) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_10) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_11) == 0 ? false : true);

    EIP202_DFE_PRIO_3_WR(
            Device,
            (RingPEMap_p->RingPrio_Mask & BIT_12) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_13) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_14) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_15) == 0 ? false : true);

    // Configure DSE ring priorities
    EIP202_DSE_PRIO_0_WR(
            Device,
            (RingPEMap_p->RingPrio_Mask & BIT_0) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_1) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_2) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_3) == 0 ? false : true);

    EIP202_DSE_PRIO_1_WR(
            Device,
            (RingPEMap_p->RingPrio_Mask & BIT_4) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_5) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_6) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_7) == 0 ? false : true);

    EIP202_DSE_PRIO_2_WR(
            Device,
            (RingPEMap_p->RingPrio_Mask & BIT_8) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_9) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_10) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_11) == 0 ? false : true);

    EIP202_DSE_PRIO_3_WR(
            Device,
            (RingPEMap_p->RingPrio_Mask & BIT_12) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_13) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_14) == 0 ? false : true,
            (RingPEMap_p->RingPrio_Mask & BIT_15) == 0 ? false : true);
#endif // EIP97_GLOBAL_DFE_DSE_PRIO_CONFIGURE

    {
        // Assign Rings to this DFE thread
        // Enable EIP-202 HIA DFE thread(s)
        EIP202_DFE_TRD_CTRL_WR(Device,
                               PE_Number,   // Thread Nr
                               RingMask,        // Rings to assign
                               true,            // Enable thread
                               false);          // Do not reset thread

        // Assign Rings to this DSE thread
        // Enable EIP-202 HIA DSE thread(s)
        EIP202_DSE_TRD_CTRL_WR(Device,
                               PE_Number,   // Thread Nr
                               RingMask,        // Rings to assign
                               true,            // Enable thread
                               false);          // Do not reset thread
    }

    return EIP97_GLOBAL_NO_ERROR;
}


/* end of file eip97_global_init.c */


