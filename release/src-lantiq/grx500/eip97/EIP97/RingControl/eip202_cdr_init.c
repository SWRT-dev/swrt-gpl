/* eip202_cdr_init.c
 *
 * EIP-202 Ring Control Driver Library
 * CDR Init/Reset API implementation
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

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */

#include "eip202_cdr.h"


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default configuration
#include "c_eip202_ring.h"

// EIP-202 Ring Control Driver Library Types API
#include "eip202_ring_types.h"          // EIP202_Ring_* types

// EIP-202 Ring Control Driver Library Internal interfaces
#include "eip202_ring_internal.h"
#include "eip202_cdr_level0.h"          // EIP-202 Level 0 macros
#include "eip202_cdr_fsm.h"             // CDR State machine
#include "eip202_cdr_dscr.h"            // RingHelper callbacks
#include "eip202_cd_format.h"           // EIP-202 Command Descriptor

// RingHelper API
#include "ringhelper.h"                // RingHelper_Init

// Driver Framework Basic Definitions API
#include "basic_defs.h"                // IDENTIFIER_NOT_USED, bool, uint32_t

// Driver Framework Device API
#include "device_types.h"              // Device_Handle_t

// Driver Framework DMA Resource API
#include "dmares_types.h"         // types of the DMA resource API
#include "dmares_rw.h"            // read/write of the DMA resource API.

// Driver Framework C Run-Time Library API
#include "clib.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */


/*----------------------------------------------------------------------------
 * Local variables
 */


/*----------------------------------------------------------------------------
 * EIP202Lib_Detect
 *
 * Checks the presence of EIP-202 HIA hardware. Returns true when found.
 */
static bool
EIP202Lib_CDR_Detect(
        const Device_Handle_t Device)
{
    uint32_t Value;

    // read-write test one of the registers

    // Set MASK_31_BITS bits of the EIP202_CDR_RING_BASE_ADDR_LO register
    EIP202_CDR_Write32(Device,
                       EIP202_CDR_RING_BASE_ADDR_LO,
                       MASK_31_BITS );

    Value = EIP202_CDR_Read32(Device, EIP202_CDR_RING_BASE_ADDR_LO);
    if ((Value & MASK_31_BITS) != MASK_31_BITS)
        return false;

    // Clear MASK_31_BITS bits of the EIP202_CDR_RING_BASE_ADDR_LO register
    EIP202_CDR_Write32(Device, EIP202_CDR_RING_BASE_ADDR_LO, 0);
    Value = EIP202_CDR_Read32(Device, EIP202_CDR_RING_BASE_ADDR_LO);
    if ((Value & MASK_31_BITS) != 0)
       return false;

    return true;
}


/*----------------------------------------------------------------------------
 * EIP202Lib_CDR_ClearAllDescriptors
 *
 * Clear all descriptors
 */
static inline void
EIP202Lib_CDR_ClearAllDescriptors(
        DMAResource_Handle_t Handle,
        const uint32_t DescriptorSpacingWordCount,
        const uint32_t DescriptorSizeWordCount,
        const uint32_t NumberOfDescriptors)
{
    static const uint32_t Words[EIP202_CD_MAX_WORD_COUNT]; // Will be set to 0
    unsigned int i;

    for(i = 0; i < NumberOfDescriptors; i++)
    {
        DMAResource_Write32Array(
                Handle,
                i * DescriptorSpacingWordCount,
                DescriptorSizeWordCount,
                Words);
    }
}


/*----------------------------------------------------------------------------
 * EIP202_CDR_Init
 *
 */
EIP202_Ring_Error_t
EIP202_CDR_Init(
        EIP202_Ring_IOArea_t * IOArea_p,
        const Device_Handle_t Device,
        const EIP202_CDR_Settings_t * const CDRSettings_p)
{
    uint16_t CDFIFOWordCount;
    EIP202_Ring_Error_t rv;
    volatile EIP202_CDR_True_IOArea_t * const TrueIOArea_p = CDRIOAREA(IOArea_p);

    EIP202_RING_CHECK_POINTER(IOArea_p);

    // Initialize the IO Area
    TrueIOArea_p->Device = Device;
    TrueIOArea_p->State = (unsigned int)EIP202_CDR_STATE_UNINITIALIZED;

    // Check if the CPU integer size is enough to store 32-bit value
    if(sizeof(unsigned int) < sizeof(uint32_t))
        return EIP202_RING_UNSUPPORTED_FEATURE_ERROR;

    // Detect presence of EIP-202 CDR hardware
    if(!EIP202Lib_CDR_Detect(Device))
        return EIP202_RING_UNSUPPORTED_FEATURE_ERROR;

    // Token must be in a separate (from CDR) DMA buffer
    if(!CDRSettings_p->fATP)
        return EIP202_RING_UNSUPPORTED_FEATURE_ERROR;

    // Extension of 32-bit pointers to 64-bit addresses not supported.
    if(CDRSettings_p->Params.DMA_AddressMode == EIP202_RING_64BIT_DMA_EXT_ADDR)
        return EIP202_RING_UNSUPPORTED_FEATURE_ERROR;

    if(CDRSettings_p->Params.DscrOffsWordCount == 0 ||
       CDRSettings_p->Params.DscrOffsWordCount <
       CDRSettings_p->Params.DscrSizeWordCount)
        return EIP202_RING_ARGUMENT_ERROR;

    // Ring size cannot be smaller than one descriptor size or
    // larger than 4194303 (16MB / 4 - 1), in 32-bit words
    if(CDRSettings_p->Params.RingSizeWordCount <
       CDRSettings_p->Params.DscrOffsWordCount ||
       CDRSettings_p->Params.RingSizeWordCount > 4194303)
        return EIP202_RING_ARGUMENT_ERROR;

    // Read Command Descriptor FIFO size (in 32-bit words)
    EIP202_CDR_STAT_FIFO_SIZE_RD(Device, &CDFIFOWordCount);

    if(CDRSettings_p->Params.DscrSizeWordCount > EIP202_CD_MAX_WORD_COUNT ||
       CDRSettings_p->Params.DscrSizeWordCount > CDFIFOWordCount)
        return EIP202_RING_ARGUMENT_ERROR;

    if( CDRSettings_p->Params.DscrThresholdWordCount > CDFIFOWordCount ||
        (CDRSettings_p->Params.DscrFetchSizeWordCount &
                (CDRSettings_p->Params.DscrOffsWordCount - 1)) != 0)
        return EIP202_RING_ARGUMENT_ERROR;

    if( CDRSettings_p->Params.IntThresholdDscrCount >
        CDRSettings_p->Params.RingSizeWordCount )
        return EIP202_RING_ARGUMENT_ERROR;

    // Configure the Ring Helper
    TrueIOArea_p->RingHelperCallbacks.WriteFunc_p = &EIP202_CDR_WriteCB;
    TrueIOArea_p->RingHelperCallbacks.ReadFunc_p = &EIP202_CDR_ReadCB;
    TrueIOArea_p->RingHelperCallbacks.StatusFunc_p = &EIP202_CDR_StatusCB;
    TrueIOArea_p->RingHelperCallbacks.CallbackParam1_p = IOArea_p;
    TrueIOArea_p->RingHelperCallbacks.CallbackParam2 = 0;
    TrueIOArea_p->RingHandle = CDRSettings_p->Params.RingDMA_Handle;
    TrueIOArea_p->DescOffsWordCount = CDRSettings_p->Params.DscrOffsWordCount;
    TrueIOArea_p->RingSizeWordCount = CDRSettings_p->Params.RingSizeWordCount;

    // Initialize one RingHelper instance for one CDR instance
    if( RingHelper_Init(
         (volatile RingHelper_t*)&TrueIOArea_p->RingHelper,
         (volatile RingHelper_CallbackInterface_t*)&TrueIOArea_p->RingHelperCallbacks,
         true, // Separate CDR ring
         (unsigned int)(CDRSettings_p->Params.RingSizeWordCount /
             CDRSettings_p->Params.DscrOffsWordCount),
         (unsigned int)(CDRSettings_p->Params.RingSizeWordCount /
                        CDRSettings_p->Params.DscrOffsWordCount)) < 0)
        return EIP202_RING_ARGUMENT_ERROR;

    // Transit to a new state
    rv = EIP202_CDR_State_Set((volatile EIP202_CDR_State_t*)&TrueIOArea_p->State,
                             EIP202_CDR_STATE_INITIALIZED);
    if(rv != EIP202_RING_NO_ERROR)
        return EIP202_RING_ILLEGAL_IN_STATE;

    // Prepare the CDR DMA buffer
    // Initialize all descriptors with zero for CDR
    EIP202Lib_CDR_ClearAllDescriptors(
            TrueIOArea_p->RingHandle,
            CDRSettings_p->Params.DscrOffsWordCount,
            CDRSettings_p->Params.DscrSizeWordCount,
            CDRSettings_p->Params.RingSizeWordCount /
                 CDRSettings_p->Params.DscrOffsWordCount);

    // Call PreDMA to make sure engine sees it
    DMAResource_PreDMA(TrueIOArea_p->RingHandle,
                       0,
                       (unsigned int)(TrueIOArea_p->RingSizeWordCount*4));

    EIP202_CDR_RING_BASE_ADDR_LO_WR(
                       Device,
                       CDRSettings_p->Params.RingDMA_Address.Addr);

    EIP202_CDR_RING_BASE_ADDR_HI_WR(
                       Device,
                       CDRSettings_p->Params.RingDMA_Address.UpperAddr);

    EIP202_CDR_RING_SIZE_WR(
                       Device,
                       CDRSettings_p->Params.RingSizeWordCount);

    EIP202_CDR_DESC_SIZE_WR(
                       Device,
                       CDRSettings_p->Params.DscrSizeWordCount,
                       CDRSettings_p->Params.DscrOffsWordCount,
                       CDRSettings_p->fATPtoToken,
                       CDRSettings_p->fATP,
                       CDRSettings_p->Params.DMA_AddressMode == EIP202_RING_64BIT_DMA_DSCR_PTR);

    EIP202_CDR_CFG_WR(
                       Device,
                       CDRSettings_p->Params.DscrFetchSizeWordCount,
                       CDRSettings_p->Params.DscrThresholdWordCount);

    EIP202_CDR_DMA_CFG_WR(
                       Device,
                       (uint8_t)CDRSettings_p->Params.ByteSwap_Descriptor_Mask,
                       (uint8_t)CDRSettings_p->Params.ByteSwap_Packet_Mask,
                       (uint8_t)CDRSettings_p->Params.ByteSwap_Token_Mask,
                       // Bufferability control
                       true,  // Buffer Ownership Word DMA writes
                       EIP202_RING_CD_WR_CACHE_CTRL, // Write cache type control
                       EIP202_RING_CD_RD_CACHE_CTRL); // Read cache type control

    return EIP202_RING_NO_ERROR;
}


/*----------------------------------------------------------------------------
 * EIP202_CDR_Reset
 *
 */
EIP202_Ring_Error_t
EIP202_CDR_Reset(
        EIP202_Ring_IOArea_t * const IOArea_p,
        const Device_Handle_t Device)
{
    EIP202_Ring_Error_t rv;
    volatile EIP202_CDR_True_IOArea_t * const TrueIOArea_p = CDRIOAREA(IOArea_p);

    EIP202_RING_CHECK_POINTER(IOArea_p);

    // Initialize the IO Area
    memset((void*)IOArea_p, 0, sizeof(*TrueIOArea_p));
    TrueIOArea_p->Device = Device;
    TrueIOArea_p->State = (unsigned int)EIP202_CDR_STATE_UNKNOWN;

    // Transit to a new state
    rv = EIP202_CDR_State_Set((volatile EIP202_CDR_State_t*)&TrueIOArea_p->State,
                             EIP202_CDR_STATE_UNINITIALIZED);
    if(rv != EIP202_RING_NO_ERROR)
        return EIP202_RING_ILLEGAL_IN_STATE;

    // Clear CDR count
    EIP202_CDR_COUNT_WR(Device, 0, true);

    // Re-init CDR
    EIP202_CDR_POINTER_DEFAULT_WR(Device);

    // Restore default register values
    EIP202_CDR_RING_BASE_ADDR_LO_DEFAULT_WR(Device);
    EIP202_CDR_RING_BASE_ADDR_HI_DEFAULT_WR(Device);
    EIP202_CDR_RING_SIZE_DEFAULT_WR(Device);
    EIP202_CDR_DESC_SIZE_DEFAULT_WR(Device);
    EIP202_CDR_CFG_DEFAULT_WR(Device);
    EIP202_CDR_DMA_CFG_DEFAULT_WR(Device);

    // Clear and disable all CDR interrupts
    EIP202_CDR_STAT_CLEAR_ALL_IRQ_WR(Device);

    return EIP202_RING_NO_ERROR;
}


/* end of file eip202_cdr_init.c */


