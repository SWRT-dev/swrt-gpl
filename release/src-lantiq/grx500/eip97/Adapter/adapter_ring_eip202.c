/* adapter_ring_eip202.c
 *
 * Adapter EIP-202 implementation: EIP-202 specific layer.
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

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */

// SLAD Adapter PEC Device-specific API
#include "adapter_pecdev_dma.h"

// Ring Control configuration API
#include "adapter_ring_eip202.h"

#if defined(ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT) || \
    defined(ADAPTER_EIP202_RC_DMA_BANK_SUPPORT)
// Record Cache configuration API
#include "adapter_rc_eip207.h"
#endif


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default configuration
#include "c_adapter_eip202.h"

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // bool
#include "api_pec.h"
#include "api_dmabuf.h"

// Adapter DMABuf internal API
#include "adapter_dmabuf.h"

// Convert address to pair of 32-bit words.
#include "adapter_addrpair.h"

// Adapter interrupts API
#include "adapter_interrupts.h" // Adapter_Interrupt_*

// Driver Framework DMAResource API
#include "dmares_addr.h"      // AddrTrans_*
#include "dmares_buf.h"       // DMAResource_Alloc/Release
#include "dmares_rw.h"        // DMAResource_PreDMA/PostDMA
#include "dmares_mgmt.h"      // DMAResource_Alloc/Release

#include "device_types.h"  // Device_Handle_t
#include "device_mgmt.h" // Device_find

#ifdef ADAPTER_EIP202_ENABLE_SCATTERGATHER
#include "api_pec_sg.h"         // PEC_SG_* (the API we implement here)
#endif

// EIP97 Ring Control
#include "eip202_ring_types.h"
#include "eip202_cdr.h"
#include "eip202_rdr.h"

#if defined(ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT) || \
    defined(ADAPTER_EIP202_RC_DMA_BANK_SUPPORT)
#ifdef ADAPTER_EIP202_USE_SHDEVXS
// EIP-207 Record Cache (RC) interface
#include "shdevxs_dmapool.h"
#include "shdevxs_rc.h"
#else
// EIP-207 Record Cache (RC) interface
#include "eip207_rc.h"                  // EIP207_RC_Record_Update()
#endif // ADAPTER_EIP202_USE_SHDEVXS
#endif

#include "clib.h"  // memcpy

// Log API
#include "log.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

#if defined(ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT) || \
    defined(ADAPTER_EIP202_RC_DMA_BANK_SUPPORT)
#ifndef ADAPTER_EIP202_USE_SHDEVXS
// The default Record Cache set number to be used
// Note: Only one cache set is supported by this implementation!
#define EIP207_RC_SET_NR_DEFAULT              0
#endif
#endif // ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT || ADAPTER_EIP202_RC_DMA_BANK_SUPPORT

#if defined(ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT) || \
    defined(ADAPTER_EIP202_RC_DMA_BANK_SUPPORT)
#ifdef ADAPTER_EIP202_USE_SHDEVXS
#define ADAPTER_EIP202_DUMMY_DMA_ADDRRES      SHDevXS_RC_Record_Dummy_Addr_Get()
#else
#define ADAPTER_EIP202_DUMMY_DMA_ADDRRES      EIP207_RC_Record_Dummy_Addr_Get()
#endif // ADAPTER_EIP202_USE_SHDEVXS
#else

#ifdef ADAPTER_EIP202_USE_POINTER_TYPES
#define ADAPTER_EIP202_DUMMY_DMA_ADDRRES       0
#else
#define ADAPTER_EIP202_DUMMY_DMA_ADDRRES       3
#endif

#endif // ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT || ADAPTER_EIP202_RC_DMA_BANK_SUPPORT

// Move bit position src in value v to dst.
#define BIT_MOVE(v, src, dst)  ( ((src) < (dst)) ? \
    ((v) << ((dst) - (src))) & (1 << (dst)) :  \
    ((v) >> ((src) - (dst))) & (1 << (dst)))


#ifdef ADAPTER_EIP202_64BIT_DEVICE
#define ADAPTER_EIP202_CDR_ENTRY_WORDS 16
#define ADAPTER_EIP202_RDR_ENTRY_WORDS 16

#ifndef ADAPTER_EIP202_SEPARATE_RINGS
#define ADAPTER_EIP202_CDR_BYTE_OFFSET 16
#endif

#else  // !ADAPTER_EIP202_64BIT_DEVICE

#ifdef ADAPTER_EIP202_USE_EXTENDED_DESCRIPTOR
#define ADAPTER_EIP202_CDR_ENTRY_WORDS 16
#define ADAPTER_EIP202_RDR_ENTRY_WORDS 16

#ifndef ADAPTER_EIP202_SEPARATE_RINGS
#define ADAPTER_EIP202_CDR_BYTE_OFFSET 16
#endif

#else

#define ADAPTER_EIP202_CDR_ENTRY_WORDS 8
#define ADAPTER_EIP202_RDR_ENTRY_WORDS 8

#ifndef ADAPTER_EIP202_SEPARATE_RINGS
#define ADAPTER_EIP202_CDR_BYTE_OFFSET 8
#endif

#endif

#endif

#define ADAPTER_EIP202_DF_BIT       BIT_24

#ifndef ADAPTER_EIP202_CDR_DSCR_FETCH_WORD_COUNT
#define ADAPTER_EIP202_CDR_DSCR_FETCH_WORD_COUNT ADAPTER_EIP202_CDR_ENTRY_WORDS
#endif

typedef struct
{
    unsigned int CDR_IRQ_ID;

    unsigned int CDR_IRQ_Flags;

    const char * CDR_DeviceName_p;

    unsigned int RDR_IRQ_ID;

    unsigned int RDR_IRQ_Flags;

    const char * RDR_DeviceName_p;

} Adapter_Ring_EIP202_Device_t;


#ifdef ADAPTER_EIP202_USE_POINTER_TYPES
#define ADAPTER_EIP202_TR_ADDRESS       2
#ifndef ADAPTER_EIP202_USE_LARGE_TRANSFORM_DISABLE
#define ADAPTER_EIP202_TR_LARGE_ADDRESS 3
// Bit in first word of SA record to indicate it is large.
#define ADAPTER_EIP202_TR_ISLARGE              BIT_4
#endif
#endif


/*----------------------------------------------------------------------------
 * Local variables
 */

const Adapter_Ring_EIP202_Device_t EIP202_Devices [] =
{
    ADAPTER_EIP202_DEVICES
};

// number of devices supported calculated on ADAPTER_EIP202_DEVICES defined
// in c_adapter_eip202.h
#define ADAPTER_EIP202_DEVICE_COUNT_ACTUAL \
        (sizeof(EIP202_Devices) / sizeof(Adapter_Ring_EIP202_Device_t))

static EIP202_Ring_IOArea_t CDR_IOArea[ADAPTER_EIP202_DEVICE_COUNT];
static EIP202_Ring_IOArea_t RDR_IOArea[ADAPTER_EIP202_DEVICE_COUNT];

static DMAResource_Handle_t CDR_Handle[ADAPTER_EIP202_DEVICE_COUNT];
static DMAResource_Handle_t RDR_Handle[ADAPTER_EIP202_DEVICE_COUNT];

static  EIP202_ARM_CommandDescriptor_t
    EIP202_CDR_Entries[ADAPTER_EIP202_DEVICE_COUNT][ADAPTER_EIP202_MAX_LOGICDESCR];

static  EIP202_ARM_PreparedDescriptor_t
    EIP202_RDR_Prepared[ADAPTER_EIP202_DEVICE_COUNT][ADAPTER_EIP202_MAX_LOGICDESCR];

static  EIP202_ARM_ResultDescriptor_t
    EIP202_RDR_Entries[ADAPTER_EIP202_DEVICE_COUNT][ADAPTER_EIP202_MAX_LOGICDESCR];

static const PEC_Capabilities_t CapabilitiesString =
{
    "EIP-202 Packet Engine rings=__ (ARM,"        // szTextDescription
#ifdef ADAPTER_EIP202_ENABLE_SCATTERGATHER
    "SG,"
#endif
#ifndef ADAPTER_EIP202_REMOVE_BOUNCEBUFFERS
    "BB,"
#endif
#ifdef ADAPTER_EIP202_INTERRUPTS_ENABLE
    "Int)"
#else
    "Poll)"
#endif
};

// Static variables used by the adapter_ring_eip202.h interface implementation
static bool    Adapter_Ring_EIP202_Configured;
static uint8_t Adapter_Ring_EIP202_HDW;
static uint8_t Adapter_Ring_EIP202_CFSize;
static uint8_t Adapter_Ring_EIP202_RFSize;

#ifdef ADAPTER_EIP202_DMARESOURCE_BANKS_ENABLE
#ifdef ADAPTER_EIP202_RC_DMA_BANK_SUPPORT
static uint32_t EIP202_SABaseAddr;
static uint32_t EIP202_SABaseUpperAddr;
#endif // ADAPTER_EIP202_RC_DMA_BANK_SUPPORT
#endif // ADAPTER_EIP202_DMARESOURCE_BANKS_ENABLE

#ifdef ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT
// Static variables used by the adapter_rc_eip207.h interface implementation
static bool Adapter_RC_EIP207_Configured = true;
static bool Adapter_RC_EIP207_TRC_Enabled = true;
static bool Adapter_RC_EIP207_ARC4RC_Enabled;
static bool Adapter_RC_EIP207_Combined;
#endif // ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT

// DMA buffer allocation alignment in bytes
static int Adapter_Ring_EIP202_DMA_Alignment_ByteCount =
                                        ADAPTER_DMABUF_ALIGNMENT_INVALID;


/*----------------------------------------------------------------------------
 * Adapter_GetPhysAddr
 *
 * Obtain the physical address from a DMABuf handle.
 * Take the bounce handle into account.
 *
 */
static void
Adapter_GetPhysAddr(
        const DMABuf_Handle_t Handle,
        EIP202_DeviceAddress_t *PhysAddr_p)
{
    DMAResource_Handle_t DMAHandle =
        Adapter_DMABuf_Handle2DMAResourceHandle(Handle);
    DMAResource_Record_t * Rec_p = NULL;
    DMAResource_AddrPair_t PhysAddr_Pair;

    if (PhysAddr_p == NULL)
    {
        LOG_CRIT("Adapter_GetPhysAddr: PANIC\n");
        return;
    }

    PhysAddr_p->Addr        = ADAPTER_EIP202_DUMMY_DMA_ADDRRES;
    PhysAddr_p->UpperAddr   = 0;

    if (!DMAResource_IsValidHandle(DMAHandle))
        return;

    Rec_p = DMAResource_Handle2RecordPtr(DMAHandle);

    if (Rec_p == NULL)
        return;

#ifndef ADAPTER_EIP202_REMOVE_BOUNCEBUFFERS
    if (Rec_p->bounce.Bounce_Handle != NULL)
        DMAHandle = Rec_p->bounce.Bounce_Handle;
#endif

    if (DMAResource_Translate(DMAHandle, DMARES_DOMAIN_BUS,
                              &PhysAddr_Pair) == 0)
    {
        Adapter_AddrToWordPair(PhysAddr_Pair.Address_p, 0, &PhysAddr_p->Addr,
                              &PhysAddr_p->UpperAddr);
    }
}


/*----------------------------------------------------------------------------
 * BoolToString()
 *
 * Convert boolean value to string.
 */
static const char *
BoolToString(
        bool b)
{
    if (b)
        return "true";
    else
        return "false";
}


/*----------------------------------------------------------------------------
 * AdapterLib_Ring_EIP202_Status_Report
 *
 * Report the status of the CDR and RDR interface
 *
 */
static void
AdapterLib_Ring_EIP202_Status_Report(
        unsigned int InterfaceId)
{
    EIP202_Ring_Error_t res;
    EIP202_CDR_Status_t CDR_Status;
    EIP202_RDR_Status_t RDR_Status;
    LOG_CRIT("Status of CDR/RDR interface %d\n",InterfaceId);

    res = EIP202_CDR_Status_Get(
        CDR_IOArea + InterfaceId,
        &CDR_Status);
    if (res != EIP202_RING_NO_ERROR)
    {
        LOG_CRIT("EIP202_CDR_Status_Get returned error\n");
        return;
    }
    LOG_CRIT("CDR Status: DMA err: %s, err: %s: ovf/under err: %s\n"
             "Threshold int: %s, timeout int: %s, FIFO count: %d\n",
             BoolToString(CDR_Status.fDMAError),
             BoolToString(CDR_Status.fError),
             BoolToString(CDR_Status.fOUFlowError),
             BoolToString(CDR_Status.fTresholdInt),
             BoolToString(CDR_Status.fTimeoutInt),
             CDR_Status.CDFIFOWordCount);

    LOG_INFO("\n\t\t\t EIP202_RDR_Status_Get \n");

    res = EIP202_RDR_Status_Get(
        RDR_IOArea + InterfaceId,
        &RDR_Status);
    if (res != EIP202_RING_NO_ERROR)
    {
        LOG_CRIT("EIP202_RDR_Status_Get returned error\n");
        return;
    }
    LOG_CRIT("RDR Status: DMA err: %s, err: %s: ovf/under err: %s\n"
             "Buf ovf: %s, Descriptor ovf %s\n"
             "Threshold int: %s, timeout int: %s, FIFO count: %d\n",
             BoolToString(RDR_Status.fDMAError),
             BoolToString(RDR_Status.fError),
             BoolToString(RDR_Status.fOUFlowError),
             BoolToString(RDR_Status.fRDBufOverflowInt),
             BoolToString(RDR_Status.fRDOverflowInt),
             BoolToString(RDR_Status.fTresholdInt),
             BoolToString(RDR_Status.fTimeoutInt),
             RDR_Status.RDFIFOWordCount);
}


/*----------------------------------------------------------------------------
 * AdapterLib_Ring_EIP202_DMA_Alignment_Determine
 *
 * Determine the required EIP-202 DMA alignment value
 *
 */
static bool
AdapterLib_Ring_EIP202_DMA_Alignment_Determine(void)
{
    // Default alignment value is invalid
    Adapter_Ring_EIP202_DMA_Alignment_ByteCount =
                                ADAPTER_DMABUF_ALIGNMENT_INVALID;

    if (Adapter_Ring_EIP202_Configured)
    {
        int AlignTo = Adapter_Ring_EIP202_HDW;

        // Determine the EIP-202 master interface hardware data width
        switch (AlignTo)
        {
            case EIP202_RING_DMA_ALIGNMENT_4_BYTES:
                Adapter_DMAResource_Alignment_Set(4);
                break;
            case EIP202_RING_DMA_ALIGNMENT_8_BYTES:
                Adapter_DMAResource_Alignment_Set(8);
                break;
            case EIP202_RING_DMA_ALIGNMENT_16_BYTES:
                Adapter_DMAResource_Alignment_Set(16);
                break;
            case EIP202_RING_DMA_ALIGNMENT_32_BYTES:
                Adapter_DMAResource_Alignment_Set(32);
                break;
            default:
                // Not supported, the alignment value cannot be determined
                LOG_CRIT("AdapterLib_Ring_EIP202_DMA_Alignment_Determine: "
                         "EIP-202 master interface HW data width "
                         "(%d) is unsupported\n",
                         AlignTo);
                return false; // Error
        } // switch
    }
    else
    {
#if ADAPTER_EIP202_DMA_ALIGNMENT_BYTE_COUNT == 0
        // The alignment value cannot be determined
        return false; // Error
#else
        // Set the configured non-zero alignment value
        Adapter_DMAResource_Alignment_Set(
                ADAPTER_EIP202_DMA_ALIGNMENT_BYTE_COUNT);
#endif
    }

    Adapter_Ring_EIP202_DMA_Alignment_ByteCount =
                            Adapter_DMAResource_Alignment_Get();

    return true; // Success
}


/*----------------------------------------------------------------------------
 * AdapterLib_Ring_EIP202_DMA_Alignment_Get
 *
 * Get the required EIP-202 DMA alignment value
 *
 */
static int
AdapterLib_Ring_EIP202_DMA_Alignment_Get(void)
{
    return Adapter_Ring_EIP202_DMA_Alignment_ByteCount;
}


/*----------------------------------------------------------------------------
 * Implementation of the adapter_ring_eip202.h interface
 *
 */

/*----------------------------------------------------------------------------
 * Adapter_Ring_EIP202_Configure
 */
void
Adapter_Ring_EIP202_Configure(
        const uint8_t HostDataWidth,
        const uint8_t CF_Size,
        const uint8_t RF_Size)
{
    Adapter_Ring_EIP202_HDW    = HostDataWidth;
    Adapter_Ring_EIP202_CFSize = CF_Size;
    Adapter_Ring_EIP202_RFSize = RF_Size;

    Adapter_Ring_EIP202_Configured = true;
}

/*----------------------------------------------------------------------------
 * Implementation of the adapter_rc_eip207.h interface
 *
 */

#ifdef ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT
/*----------------------------------------------------------------------------
 * Adapter_RC_EIP207_Configure
 */
void
Adapter_RC_EIP207_Configure(
        const bool fEnabledTRC,
        const bool fEnabledARC4RC,
        const bool fCombined)
{
    Adapter_RC_EIP207_Combined       = fCombined;
    Adapter_RC_EIP207_TRC_Enabled    = fEnabledTRC;
    Adapter_RC_EIP207_ARC4RC_Enabled = fEnabledARC4RC;

    Adapter_RC_EIP207_Configured     = true;

}
#endif // ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT


/*----------------------------------------------------------------------------
 * Implementation of the adapter_pecdev_dma.h interface
 *
 */

/*----------------------------------------------------------------------------
 * Adapter_PECDev_Capabilities_Get
 */
PEC_Status_t
Adapter_PECDev_Capabilities_Get(
        PEC_Capabilities_t * const Capabilities_p)
{
    uint8_t Versions[2];

    if (Capabilities_p == NULL)
        return PEC_ERROR_BAD_PARAMETER;

    memcpy(Capabilities_p, &CapabilitiesString, sizeof(CapabilitiesString));

    // now fill in the number of rings.
    {
        Versions[0] = ADAPTER_EIP202_DEVICE_COUNT / 10;
        Versions[1] = ADAPTER_EIP202_DEVICE_COUNT % 10;
    }

    {
        char * p = Capabilities_p->szTextDescription;
        int VerIndex = 0;
        int i = 0;

        if (p[sizeof(CapabilitiesString)-1] != 0)
            return PEC_ERROR_INTERNAL;

        while(p[i])
        {
            if (p[i] == '_')
            {
                if (VerIndex == sizeof(Versions)/sizeof(Versions[0]))
                    return PEC_ERROR_INTERNAL;
                if (Versions[VerIndex] > 9)
                    p[i] = '?';
                else
                    p[i] = '0' + Versions[VerIndex++];
            }

            i++;
        }
    }

    return PEC_STATUS_OK;
}

/*----------------------------------------------------------------------------
 * Adapter_PECDev_Control_Write
 */
PEC_Status_t
Adapter_PECDev_CD_Control_Write(
    PEC_CommandDescriptor_t *Command_p,
    const PEC_PacketParams_t *PacketParams_p)
{
#ifdef ADAPTER_EIP202_USE_EXTENDED_DESCRIPTOR
    Command_p->Control1 = PacketParams_p->TokenHeaderWord;
    Command_p->Control2 = (PacketParams_p->HW_Services << 24);
    Command_p->Control3 =
        (PacketParams_p->PadByte << 16) | (PacketParams_p->Offset << 8);
    return PEC_STATUS_OK;
#elif defined(ADAPTER_EIP202_USE_INVALIDATE_COMMANDS)
    Command_p->Control1 = PacketParams_p->TokenHeaderWord;
    Command_p->Control2 = (PacketParams_p->HW_Services << 23) ^ BIT_25;
    return PEC_STATUS_OK;
#else
    IDENTIFIER_NOT_USED(Command_p);
    IDENTIFIER_NOT_USED(PacketParams_p);

    return PEC_ERROR_NOT_IMPLEMENTED;
#endif
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_RD_Status_Read
 */
PEC_Status_t
Adapter_PECDev_RD_Status_Read(
        const PEC_ResultDescriptor_t * const Result_p,
        PEC_ResultStatus_t * const ResultStatus_p)
{
    unsigned int s = Result_p->Status1;

    ResultStatus_p->NextHeader   = Result_p->Status2 & 0xff;

    ResultStatus_p->PadByteCount = (Result_p->Status2 >> 8) & 0xff;

    ResultStatus_p->errors =
        BIT_MOVE(s, 17, 11) | // EIP-96 Packet length error
        BIT_MOVE(s, 18, 17) | // EIP-96 Token error
        BIT_MOVE(s, 19, 18) | // EIP-96 Bypass error
        BIT_MOVE(s, 20,  9) | // EIP-96 Block size error
        BIT_MOVE(s, 21, 16) | // EIP-96 Hash block size error.
        BIT_MOVE(s, 22, 10) | // EIP-96 Invalid combo
        BIT_MOVE(s, 23,  5) | // EIP-96 Prohibited algo
        BIT_MOVE(s, 24, 19) | // EIP-96 Hash overflow
#ifndef ADAPTER_EIP202_RING_TTL_ERROR_WA
        BIT_MOVE(s, 25, 20) | // EIP-96 TTL/Hop limit underflow.
#endif
        BIT_MOVE(s, 26,  0) | // EIP-96 Authentication failed
        BIT_MOVE(s, 27,  2) | // EIP-96 Sequence number check failed.
        BIT_MOVE(s, 28,  8) | // EIP-96 SPI check failed.
        BIT_MOVE(s, 29, 21) | // EIP-96 Incorrect checksum
        BIT_MOVE(s, 30,  1) | // EIP-96 Pad verification.
        BIT_MOVE(s, 31, 22) | // EIP-96 Timeout
        BIT_MOVE(s, 15, 26) | // EIP-202 Buffer overflow
        BIT_MOVE(s, 14, 25);  // EIP-202 Descriptor overflow

#ifdef EIP202_USE_EXTENDED_DESCRIPTOR
    if (DMABuf_Handle_IsSame(&Result_p->DstPkt_Handle, &DMABuf_NULLHandle))
    {
        // For record invalidate commands, the destination buffer
        // is NULL, suppress the buffer overflow error from the device.
        ResultStatus_p->errors &= ~BIT_26;
    }

    ResultStatus_p->NextHeaderOffset =
                            (uint16_t)(Result_p->Status3 & MASK_8_BITS);
    ResultStatus_p->PadByteCount     =
                            (uint8_t)((Result_p->Status3 >> 8) & MASK_8_BITS);

    // Reference to the Header Processor Context
    ResultStatus_p->HdrProcCtxRef    = Result_p->Status4;

    // DF
    ResultStatus_p->fDF    = ((Result_p->Status6 & BIT_13) != 0);

    // TOS/CS
    ResultStatus_p->TOS_TC = ((Result_p->Status6 >> 5) & MASK_8_BITS);

    // Check extended errors
    s = (Result_p->Status6 >> 16) & MASK_5_BITS;
    switch (s)
    {
        case 1: // D=0, S=0: Unsupported protocol
        case 2: // D=0, S=1: Unsupported input token type / transformation
            ResultStatus_p->errors |= PEC_PKT_ERROR_BAD_PROTO;
            break;
        case 3: // D=1, S=0: Sanity checks fail
            ResultStatus_p->errors |= PEC_PKT_ERROR_SANITY;
            break;
        case 4: // D=1, S=1: Sanity checks fail & unsupported protocol
            ResultStatus_p->errors |= (PEC_PKT_ERROR_BAD_PROTO |
                                       PEC_PKT_ERROR_SANITY);
            break;
    } // switch
#else
    if (Result_p->Bypass_WordCount == 1)
    {
        uint32_t ErrorFlags = (Result_p->Status2 >> 16);

        if (ErrorFlags & EIP202_RDR_BYPASS_FLAG_PROTOCOL_UNSUPPORTED)
            ResultStatus_p->errors |= PEC_PKT_ERROR_BAD_PROTO;
        else if (ErrorFlags & EIP202_RDR_BYPASS_FLAG_SANITY_CHECK_FAILED)
            ResultStatus_p->errors |= PEC_PKT_ERROR_SANITY;
    }
    else if(Result_p->Bypass_WordCount == 2)
    {
        ResultStatus_p->fDF =
                ((Result_p->Status2 & ADAPTER_EIP202_DF_BIT) != 0);
        ResultStatus_p->TOS_TC = ((Result_p->Status2 >> 16) & MASK_8_BITS);
        ResultStatus_p->HdrProcCtxRef = Result_p->Status3;
        ResultStatus_p->NextHeaderOffset = Result_p->Status4 & MASK_16_BITS;
    }
#endif // !EIP202_USE_EXTENDED_DESCRIPTOR

    return PEC_STATUS_OK;
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_Init
 */
PEC_Status_t
Adapter_PECDev_Init(
        const unsigned int InterfaceId,
        const PEC_InitBlock_t * const InitBlock_p)
{
    Device_Handle_t CDR_Device, RDR_Device;
    EIP202_Ring_Error_t res;
    DMAResource_AddrPair_t PhysAddr_Pair;
    IDENTIFIER_NOT_USED(InitBlock_p);

    LOG_INFO("\n\t\t Adapter_PECDev_Init \n");

    if (ADAPTER_EIP202_DEVICE_COUNT > ADAPTER_EIP202_DEVICE_COUNT_ACTUAL)
    {
        LOG_CRIT("Adapter_PECDev_Init: "
                 "Adapter EIP-202 devices configuration is invalid\n");
        return PEC_ERROR_BAD_PARAMETER;
    }

    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return PEC_ERROR_BAD_PARAMETER;

    CDR_Device = Device_Find(EIP202_Devices[InterfaceId].CDR_DeviceName_p);
    RDR_Device = Device_Find(EIP202_Devices[InterfaceId].RDR_DeviceName_p);

    if (CDR_Device == NULL || RDR_Device == NULL)
        return PEC_ERROR_INTERNAL;

    LOG_INFO("\n\t\t\t EIP202_CDR_Reset \n");

    // Reset both the CDR and RDR devives.
    res = EIP202_CDR_Reset(CDR_IOArea + InterfaceId,
                    CDR_Device);

    if (res != EIP202_RING_NO_ERROR)
        return PEC_ERROR_INTERNAL;

    LOG_INFO("\n\t\t\t EIP202_RDR_Reset \n");

    res = EIP202_RDR_Reset(RDR_IOArea + InterfaceId,
                    RDR_Device);

    if (res != EIP202_RING_NO_ERROR)
        return PEC_ERROR_INTERNAL;

    // Determine the DMA buffer allocation alignment
    if (!AdapterLib_Ring_EIP202_DMA_Alignment_Determine())
        return PEC_ERROR_INTERNAL;

#ifdef ADAPTER_EIP202_DMARESOURCE_BANKS_ENABLE
#ifdef ADAPTER_EIP202_RC_DMA_BANK_SUPPORT
#ifdef ADAPTER_EIP202_USE_SHDEVXS
    {
        void *SABaseAddr;

        SHDevXS_DMAPool_GetBase(&SABaseAddr);

        LOG_INFO("Adapter_PECDev_Init: got SA base %p\n",SABaseAddr);

        Adapter_AddrToWordPair(SABaseAddr,
                               0,
                               &EIP202_SABaseAddr,
                               &EIP202_SABaseUpperAddr);
    }
#else
    { // Set the SA pool base address.
        int dmares;
        Device_Handle_t Device;
        DMAResource_Handle_t DMAHandle;
        DMAResource_Properties_t DMAProperties;
        DMAResource_AddrPair_t DMAAddr;

        // Perform a dummy allocation in bank 1 to obtain the pool base
        // address.
        DMAProperties.Alignment = AdapterLib_Ring_EIP202_DMA_Alignment_Get();
        DMAProperties.Bank      = ADAPTER_EIP202_BANK_SA;
        DMAProperties.fCached   = false;
        DMAProperties.Size      = ADAPTER_EIP202_TRANSFORM_RECORD_COUNT *
                                    ADAPTER_EIP202_TRANSFORM_RECORD_BYTE_COUNT;

        dmares = DMAResource_Alloc(
                     DMAProperties,
                     &DMAAddr,
                     &DMAHandle);

        if (dmares != 0)
            return PEC_ERROR_INTERNAL;

        // Derive the physical address from the DMA resource.
        if (DMAResource_Translate(DMAHandle,
                                  DMARES_DOMAIN_BUS,
                                  &DMAAddr) < 0)
        {
            DMAResource_Release(DMAHandle);
            return PEC_ERROR_INTERNAL;
        }

        Adapter_AddrToWordPair(DMAAddr.Address_p,
                               0,
                               &EIP202_SABaseAddr,
                               &EIP202_SABaseUpperAddr);

        // Set the cache base address.
        Device = Device_Find(ADAPTER_EIP202_GLOBAL_DEVICE_NAME);
        if (Device == NULL)
        {
            LOG_CRIT("Adapter_PECDev_UnInit: Could not find device\n");
            return PEC_ERROR_INTERNAL;
        }

        LOG_INFO("\n\t\t\t EIP207_RC_BaseAddr_Set \n");

        EIP207_RC_BaseAddr_Set(
            Device,
            EIP207_TRC_REG_BASE,
            EIP207_RC_SET_NR_DEFAULT,
            EIP202_SABaseAddr,
            EIP202_SABaseUpperAddr);

        // Release the DMA resource.
        DMAResource_Release(DMAHandle);
    }
#endif // ADAPTER_EIP202_USE_SHDEVXS
#endif // ADAPTER_EIP202_RC_DMA_BANK_SUPPORT
#endif // ADAPTER_EIP202_DMARESOURCE_BANKS_ENABLE

    // Allocate the ring buffers(s).
    {
        int dmares;
#ifdef ADAPTER_EIP202_SEPARATE_RINGS
        unsigned int CDRByteCount = 4 * ADAPTER_EIP202_MAX_PACKETS *
            ADAPTER_EIP202_CDR_ENTRY_WORDS;
        unsigned int RDRByteCount = 4 * ADAPTER_EIP202_MAX_PACKETS *
            ADAPTER_EIP202_RDR_ENTRY_WORDS;
#else
        unsigned int RDRByteCount = 4 * ADAPTER_EIP202_MAX_PACKETS *
            ADAPTER_EIP202_CDR_ENTRY_WORDS + ADAPTER_EIP202_CDR_BYTE_OFFSET;
#endif

        DMAResource_Properties_t RingProperties;
        DMAResource_AddrPair_t RingHostAddr;

        // used as uint32_t array
        RingProperties.Alignment = AdapterLib_Ring_EIP202_DMA_Alignment_Get();
        RingProperties.Bank      = ADAPTER_EIP202_BANK_RING;
        RingProperties.fCached   = false;
        RingProperties.Size      = RDRByteCount;

        dmares = DMAResource_Alloc(
                     RingProperties,
                     &RingHostAddr,
                     RDR_Handle + InterfaceId);

        if (dmares != 0)
            return PEC_ERROR_INTERNAL;

#ifdef ADAPTER_EIP202_ARMRING_ENABLE_SWAP
        DMAResource_SwapEndianess_Set(RDR_Handle[InterfaceId],true);
#endif

        memset (RingHostAddr.Address_p, 0, RDRByteCount);

#ifdef ADAPTER_EIP202_SEPARATE_RINGS

        RingProperties.Size = CDRByteCount;

        dmares = DMAResource_Alloc(
                     RingProperties,
                     &RingHostAddr,
                     CDR_Handle + InterfaceId);

        if (dmares != 0)
        {
            DMAResource_Release(RDR_Handle[InterfaceId]);
            return PEC_ERROR_INTERNAL;
        }

#ifdef ADAPTER_EIP202_ARMRING_ENABLE_SWAP
        DMAResource_SwapEndianess_Set(CDR_Handle[InterfaceId], true);
#endif

        memset (RingHostAddr.Address_p, 0, CDRByteCount);
#else
        RingProperties.Size -= ADAPTER_EIP202_CDR_BYTE_OFFSET ;
        RingProperties.fCached = false;

        {
            uint8_t * Byte_p = (uint8_t*)RingHostAddr.Address_p;

            Byte_p += ADAPTER_EIP202_CDR_BYTE_OFFSET;

            RingHostAddr.Address_p = Byte_p;
        }

        dmares = DMAResource_CheckAndRegister(
            RingProperties,
            RingHostAddr,
            'R',
            CDR_Handle + InterfaceId);

        if (dmares != 0)
        {
            DMAResource_Release(RDR_Handle[InterfaceId]);
            return PEC_ERROR_INTERNAL;
        }

#ifdef ADAPTER_EIP202_ARMRING_ENABLE_SWAP
        DMAResource_SwapEndianess_Set(CDR_Handle[InterfaceId], true);
#endif

#endif
    }

    // Initialize the CDR and RDR devices.
    {
        EIP202_CDR_Settings_t  CDR_Settings;

#ifdef ADAPTER_EIP202_RING_MANUAL_CONFIGURE
        // Configure the EIP-202 Ring Managers with manually set
        // configuration parameters
        Adapter_Ring_EIP202_Configure(ADAPTER_EIP202_HOST_DATA_WIDTH,
                                      ADAPTER_EIP202_CF_SIZE,
                                      ADAPTER_EIP202_RF_SIZE);
#endif

        CDR_Settings.fATP = true;
        CDR_Settings.fATPtoToken = false;

        CDR_Settings.Params.DataBusWidthWordCount = 1;

        // Not used for CDR
        CDR_Settings.Params.ByteSwap_DataType_Mask      = 0;
        CDR_Settings.Params.ByteSwap_Packet_Mask        = 1;

        // Enable endianess conversion for the RDR master interface
        // if configured
#ifdef ADAPTER_EIP202_CDR_BYTE_SWAP_ENABLE
        CDR_Settings.Params.ByteSwap_Token_Mask         =
        CDR_Settings.Params.ByteSwap_Descriptor_Mask    =
                                            EIP202_RING_BYTE_SWAP_METHOD_32;
#else
        CDR_Settings.Params.ByteSwap_Token_Mask         = 0;
        CDR_Settings.Params.ByteSwap_Descriptor_Mask    = 0;
#endif

        CDR_Settings.Params.Bufferability = 0;
#ifdef ADAPTER_EIP202_64BIT_DEVICE
        CDR_Settings.Params.DMA_AddressMode = EIP202_RING_64BIT_DMA_DSCR_PTR;
#else
        CDR_Settings.Params.DMA_AddressMode = EIP202_RING_64BIT_DMA_DISABLED;
#endif
        CDR_Settings.Params.RingSizeWordCount =
            ADAPTER_EIP202_CDR_ENTRY_WORDS * ADAPTER_EIP202_MAX_PACKETS;
        CDR_Settings.Params.RingDMA_Handle = CDR_Handle[InterfaceId];

#ifdef ADAPTER_EIP202_SEPARATE_RINGS
        if (DMAResource_Translate(CDR_Handle[InterfaceId], DMARES_DOMAIN_BUS,
                                  &PhysAddr_Pair) < 0)
        {
            Adapter_PECDev_UnInit(InterfaceId);
            return PEC_ERROR_INTERNAL;
        }
        Adapter_AddrToWordPair(PhysAddr_Pair.Address_p, 0,
                               &CDR_Settings.Params.RingDMA_Address.Addr,
                               &CDR_Settings.Params.RingDMA_Address.UpperAddr);
#else
        if (DMAResource_Translate(RDR_Handle[InterfaceId], DMARES_DOMAIN_BUS,
                                  &PhysAddr_Pair) < 0)
        {
            Adapter_PECDev_UnInit(InterfaceId);
            return PEC_ERROR_INTERNAL;
        }
        Adapter_AddrToWordPair(PhysAddr_Pair.Address_p,
                               ADAPTER_EIP202_CDR_BYTE_OFFSET,
                               &CDR_Settings.Params.RingDMA_Address.Addr,
                               &CDR_Settings.Params.RingDMA_Address.UpperAddr);
#endif

        CDR_Settings.Params.DscrSizeWordCount = EIP202_CDR_DSCR_MAX_WORD_COUNT;
        CDR_Settings.Params.DscrOffsWordCount = ADAPTER_EIP202_CDR_ENTRY_WORDS;

#ifndef ADAPTER_EIP202_AUTO_THRESH_DISABLE
        if(Adapter_Ring_EIP202_Configured)
        {
            uint32_t cd_size_rndup;
            int cfcount;

            // Use configuration parameters received via
            // the Ring 97 Configuration (adapter_ring_eip202.h) interface
            if(CDR_Settings.Params.DscrOffsWordCount &
                    ((1 << Adapter_Ring_EIP202_HDW) - 1))
            {
                LOG_CRIT("Adapter_PECDev_Init: Error, "
                         "Command Descriptor Offset %d"
                         " is not an integer multiple of Host Data Width %d\n",
                         CDR_Settings.Params.DscrOffsWordCount,
                         Adapter_Ring_EIP202_HDW);

                Adapter_PECDev_UnInit(InterfaceId);
                return PEC_ERROR_INTERNAL;
            }

            // Round up to the next multiple of HDW words
            cd_size_rndup = (CDR_Settings.Params.DscrSizeWordCount +
                                ((1 << Adapter_Ring_EIP202_HDW) - 1)) >>
                                         Adapter_Ring_EIP202_HDW;

            // Number of full descriptors that fit FIFO minus one
            // Note: Adapter_Ring_EIP202_CFSize is in HDW words
            cfcount = Adapter_Ring_EIP202_CFSize / cd_size_rndup - 1;

            // Check if command descriptor fits in fetch FIFO
            if(cfcount <= 0)
                cfcount = 1; // does not fit, adjust the count

            // Note: cfcount must be also checked for not exceeding
            //       max DMA length

            // Convert to 32-bits word counts
            CDR_Settings.Params.DscrFetchSizeWordCount =
                     cfcount * CDR_Settings.Params.DscrOffsWordCount;
            CDR_Settings.Params.DscrThresholdWordCount =
                     cfcount * (cd_size_rndup << Adapter_Ring_EIP202_HDW);
        }
        else
#endif // #ifndef ADAPTER_EIP202_AUTO_THRESH_DISABLE
        {
            // Use default static (user-defined) configuration parameters
            CDR_Settings.Params.DscrFetchSizeWordCount =
                                    ADAPTER_EIP202_CDR_DSCR_FETCH_WORD_COUNT;
            CDR_Settings.Params.DscrThresholdWordCount =
                                    ADAPTER_EIP202_CDR_DSCR_THRESH_WORD_COUNT;
        }

        LOG_CRIT("Adapter_PECDev_Init: CDR fetch size 0x%x, thresh 0x%x, "
                 "HDW=%d, CFsize=%d\n",
                 CDR_Settings.Params.DscrFetchSizeWordCount,
                 CDR_Settings.Params.DscrThresholdWordCount,
                 Adapter_Ring_EIP202_HDW,
                 Adapter_Ring_EIP202_CFSize);

        // CDR Interrupts will be enabled via the Event Mgmt API functions
        CDR_Settings.Params.IntThresholdDscrCount = 0;
        CDR_Settings.Params.IntTimeoutDscrCount = 0;

        LOG_INFO("\n\t\t\t EIP202_CDR_Init \n");

        res = EIP202_CDR_Init(CDR_IOArea + InterfaceId,
                             CDR_Device,
                             &CDR_Settings);

        if (res != EIP202_RING_NO_ERROR)
        {
            Adapter_PECDev_UnInit(InterfaceId);
            return PEC_ERROR_INTERNAL;
        }
    }

    {
        EIP202_RDR_Settings_t  RDR_Settings;
        RDR_Settings.Params.DataBusWidthWordCount = 1;

        // Not used for RDR
        RDR_Settings.Params.ByteSwap_DataType_Mask = 0;
        RDR_Settings.Params.ByteSwap_Token_Mask = 0;

        // Enable endianess conversion for the RDR master interface
        // if configured
        RDR_Settings.Params.ByteSwap_Packet_Mask        = 1;
#ifdef ADAPTER_EIP202_RDR_BYTE_SWAP_ENABLE
        RDR_Settings.Params.ByteSwap_Descriptor_Mask    =
                                            EIP202_RING_BYTE_SWAP_METHOD_32;
#else
        RDR_Settings.Params.ByteSwap_Descriptor_Mask    = 0;
#endif

        RDR_Settings.Params.Bufferability = 0;

#ifdef ADAPTER_EIP202_64BIT_DEVICE
        RDR_Settings.Params.DMA_AddressMode = EIP202_RING_64BIT_DMA_DSCR_PTR;
#else
        RDR_Settings.Params.DMA_AddressMode = EIP202_RING_64BIT_DMA_DISABLED;
#endif

        RDR_Settings.Params.RingSizeWordCount =
            ADAPTER_EIP202_RDR_ENTRY_WORDS * ADAPTER_EIP202_MAX_PACKETS;

        RDR_Settings.Params.RingDMA_Handle = RDR_Handle[InterfaceId];

        if (DMAResource_Translate(RDR_Handle[InterfaceId], DMARES_DOMAIN_BUS,
                                  &PhysAddr_Pair) < 0)
        {
            Adapter_PECDev_UnInit(InterfaceId);
            return PEC_ERROR_INTERNAL;
        }
        Adapter_AddrToWordPair(PhysAddr_Pair.Address_p, 0,
                               &RDR_Settings.Params.RingDMA_Address.Addr,
                               &RDR_Settings.Params.RingDMA_Address.UpperAddr);

        RDR_Settings.Params.DscrSizeWordCount = EIP202_RDR_DSCR_MAX_WORD_COUNT;
        RDR_Settings.Params.DscrOffsWordCount = ADAPTER_EIP202_RDR_ENTRY_WORDS;

#ifndef ADAPTER_EIP202_AUTO_THRESH_DISABLE
        if(Adapter_Ring_EIP202_Configured)
        {
            uint32_t rd_size_rndup;
            int rfcount;

            // Use configuration parameters received via
            // the Ring 97 Configuration (adapter_ring_eip202.h) interface
            if(RDR_Settings.Params.DscrOffsWordCount &
                    ((1 << Adapter_Ring_EIP202_HDW) - 1))
            {
                LOG_CRIT("Adapter_PECDev_Init: Error, "
                         "Result Descriptor Offset %d"
                         " is not an integer multiple of Host Data Width %d\n",
                         RDR_Settings.Params.DscrOffsWordCount,
                         Adapter_Ring_EIP202_HDW);

                Adapter_PECDev_UnInit(InterfaceId);
                return PEC_ERROR_INTERNAL;
            }

            // Round up to the next multiple of HDW words
            // Note: this is done for prepared result descriptor which
            //       has fixed size of 2 or 4 32-bit words
#ifdef ADAPTER_EIP202_64BIT_DEVICE
            rd_size_rndup = (4 + ((1 << Adapter_Ring_EIP202_HDW) - 1)) >>
                                                       Adapter_Ring_EIP202_HDW;
#else
            rd_size_rndup = (2 + ((1 << Adapter_Ring_EIP202_HDW) - 1)) >>
                                                       Adapter_Ring_EIP202_HDW;
#endif

            // Number of full descriptors that fit FIFO minus one
            // Note: Adapter_Ring_EIP202_RFSize is in HDW words
            rfcount = Adapter_Ring_EIP202_RFSize / rd_size_rndup - 1;

            // Check if prepared result descriptor fits in fetch FIFO
            if(rfcount <= 0)
                rfcount = 1; // does not fit, adjust the count

            // Note: rfcount must be also checked for not exceeding
            //       max DMA length

            // Convert to 32-bit words counts
            RDR_Settings.Params.DscrFetchSizeWordCount =
                     rfcount * RDR_Settings.Params.DscrOffsWordCount;
            RDR_Settings.Params.DscrThresholdWordCount =
                     rfcount * (rd_size_rndup << Adapter_Ring_EIP202_HDW);
        }
        else
#endif // #ifndef ADAPTER_EIP202_AUTO_THRESH_DISABLE
        {
            // Use default static (user-defined) configuration parameters
            RDR_Settings.Params.DscrFetchSizeWordCount =
                                    ADAPTER_EIP202_RDR_DSCR_FETCH_WORD_COUNT;
            RDR_Settings.Params.DscrThresholdWordCount =
                                    ADAPTER_EIP202_RDR_DSCR_THRESH_WORD_COUNT;
        }

        LOG_CRIT("Adapter_PECDev_Init: RDR fetch size 0x%x, thresh 0x%x, "
                 "RFsize=%d\n",
                 RDR_Settings.Params.DscrFetchSizeWordCount,
                 RDR_Settings.Params.DscrThresholdWordCount,
                 Adapter_Ring_EIP202_RFSize);

        // RDR Interrupts will be enabled via the Event Mgmt API functions
        RDR_Settings.Params.IntThresholdDscrCount = 0;
        RDR_Settings.Params.IntTimeoutDscrCount = 0;

        LOG_INFO("\n\t\t\t EIP202_RDR_Init \n");

        res = EIP202_RDR_Init(RDR_IOArea + InterfaceId,
                             RDR_Device,
                             &RDR_Settings);

        if (res != EIP202_RING_NO_ERROR)
        {
            Adapter_PECDev_UnInit(InterfaceId);
            return PEC_ERROR_INTERNAL;
        }

    }

    AdapterLib_Ring_EIP202_Status_Report(InterfaceId);

    return PEC_STATUS_OK;
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_UnInit
 */
PEC_Status_t
Adapter_PECDev_UnInit(
        const unsigned int InterfaceId)
{
    Device_Handle_t CDR_Device, RDR_Device;

    LOG_INFO("\n\t\t Adapter_PECDev_UnInit \n");

    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return PEC_ERROR_BAD_PARAMETER;

    AdapterLib_Ring_EIP202_Status_Report(InterfaceId);
#ifdef ADAPTER_EIP202_ENABLE_SCATTERGATHER
    {
        // Make a last attempt to get rid of any remaining result descriptors
        // belonging to unused scatter particles.
        uint32_t DscrDoneCount,DscrCount;

        LOG_INFO("\n\t\t\t EIP202_RDR_Descriptor_Get \n");

        EIP202_RDR_Descriptor_Get(RDR_IOArea + InterfaceId,
                                 EIP202_RDR_Entries[InterfaceId],
                                 ADAPTER_EIP202_MAX_LOGICDESCR,
                                 ADAPTER_EIP202_MAX_LOGICDESCR,
                                 &DscrDoneCount,
                                 &DscrCount);
    }
#endif

#ifdef ADAPTER_EIP202_RC_DMA_BANK_SUPPORT
#ifndef ADAPTER_EIP202_USE_SHDEVXS
    {
        // Reset the TRC base address to 0.
        Device_Handle_t Device;

        Device = Device_Find(ADAPTER_EIP202_GLOBAL_DEVICE_NAME);
        if (Device == NULL)
        {
            LOG_CRIT("Adapter_PECDev_UnInit: Could not find device\n");
            return PEC_ERROR_INTERNAL;
        }

        LOG_INFO("\n\t\t\t EIP207_RC_BaseAddr_Set \n");

        EIP207_RC_BaseAddr_Set(
            Device,
            EIP207_TRC_REG_BASE,
            EIP207_RC_SET_NR_DEFAULT,
            0,
            0);
    }
#endif
#endif // ADAPTER_EIP202_RC_DMA_BANK_SUPPORT

    CDR_Device = Device_Find(EIP202_Devices[InterfaceId].CDR_DeviceName_p);
    RDR_Device = Device_Find(EIP202_Devices[InterfaceId].RDR_DeviceName_p);

    if (CDR_Device == NULL || RDR_Device == NULL)
        return PEC_ERROR_INTERNAL;

    LOG_INFO("\n\t\t\t EIP202_CDR_Reset \n");

    EIP202_CDR_Reset(CDR_IOArea + InterfaceId,
                    CDR_Device);

    LOG_INFO("\n\t\t\t EIP202_RDR_Reset \n");

    EIP202_RDR_Reset(RDR_IOArea + InterfaceId,
                    RDR_Device);

    if (RDR_Handle[InterfaceId] != NULL)
    {
        DMAResource_Release(RDR_Handle[InterfaceId]);
        RDR_Handle[InterfaceId] = NULL;
    }

    if (CDR_Handle[InterfaceId] != NULL)
    {
        DMAResource_Release(CDR_Handle[InterfaceId]);
        CDR_Handle[InterfaceId] = NULL;
    }

#ifdef ADAPTER_EIP202_INTERRUPTS_ENABLE
    Adapter_Interrupt_SetHandler(EIP202_Devices[InterfaceId].RDR_IRQ_ID, NULL);
    Adapter_Interrupt_SetHandler(EIP202_Devices[InterfaceId].CDR_IRQ_ID, NULL);
#endif

    return PEC_STATUS_OK;
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_SA_Prepare
 */
PEC_Status_t
Adapter_PECDev_SA_Prepare(
        const DMABuf_Handle_t SAHandle,
        const DMABuf_Handle_t StateHandle,
        const DMABuf_Handle_t ARC4Handle)
{
    DMAResource_Handle_t SA_DMAHandle;

    IDENTIFIER_NOT_USED(StateHandle.p);
    IDENTIFIER_NOT_USED(ARC4Handle.p);

    if (DMABuf_Handle_IsSame(&SAHandle, &DMABuf_NULLHandle))
        return PEC_ERROR_BAD_PARAMETER;
    else
    {
        DMAResource_Record_t * Rec_p;

        SA_DMAHandle = Adapter_DMABuf_Handle2DMAResourceHandle(SAHandle);
        Rec_p = DMAResource_Handle2RecordPtr(SA_DMAHandle);

        if (Rec_p == NULL)
            return PEC_ERROR_INTERNAL;

#ifdef ADAPTER_EIP202_DMARESOURCE_BANKS_ENABLE
        if (Rec_p->Props.Bank != ADAPTER_EIP202_BANK_SA)
        {
            LOG_CRIT("PEC_SA_Register: Invalid bank for SA\n");
            return PEC_ERROR_BAD_PARAMETER;
        }
#endif

#ifndef ADAPTER_EIP202_USE_LARGE_TRANSFORM_DISABLE
        {
            uint32_t FirstWord = DMAResource_Read32(SA_DMAHandle, 0);
            // Register in the DMA resource record whether the transform
            // is large.
            if ( (FirstWord & ADAPTER_EIP202_TR_ISLARGE) != 0)
            {
                Rec_p->fIsLargeTransform = true;
                DMAResource_Write32(SA_DMAHandle,
                                    0,
                                    FirstWord & ~ADAPTER_EIP202_TR_ISLARGE);
                // Clear that bit in the SA record itself.
            }
            else
            {
                Rec_p->fIsLargeTransform = false;
            }
        }
#endif

        Rec_p->fIsNewSA = true;
    }

    return PEC_STATUS_OK;
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_SA_Remove
 */
PEC_Status_t
Adapter_PECDev_SA_Remove(
        const DMABuf_Handle_t SAHandle,
        const DMABuf_Handle_t StateHandle,
        const DMABuf_Handle_t ARC4Handle)
{
    IDENTIFIER_NOT_USED(StateHandle.p);

    if (DMABuf_Handle_IsSame(&SAHandle, &DMABuf_NULLHandle))
        return PEC_ERROR_BAD_PARAMETER;

#ifdef ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT
    // Invalidate the record in the EIP-207 Transform Record Cache
    // or/and ARC4 State Record Cache
    // Not configured = disabled
    if (Adapter_RC_EIP207_Configured)
    {
#ifndef ADAPTER_EIP202_USE_SHDEVXS
        Device_Handle_t Device;

        Device = Device_Find(ADAPTER_EIP202_GLOBAL_DEVICE_NAME);
        if (Device == NULL)
        {
            LOG_CRIT("Adapter_PECDev_SA_Remove: Could not find device\n");
            return PEC_ERROR_INTERNAL;
        }
#endif

        if (Adapter_RC_EIP207_TRC_Enabled)
        {
            EIP202_DeviceAddress_t DMA_Addr;

            Adapter_GetPhysAddr(SAHandle, &DMA_Addr);

#ifdef ADAPTER_EIP202_DMARESOURCE_BANKS_ENABLE
            DMA_Addr.Addr -= EIP202_SABaseAddr;
#endif

            // Invalidate the SA Record in the TRC
#ifdef ADAPTER_EIP202_USE_SHDEVXS
            LOG_INFO("\n\t\t\t SHDevXS_TRC_Record_Invalidate \n");

            SHDevXS_TRC_Record_Invalidate(DMA_Addr.Addr);
#else
            LOG_INFO("\n\t\t\t EIP207_RC_Record_Update \n");

            EIP207_RC_Record_Update(
                    Device,
                    EIP207_TRC_REG_BASE,
                    EIP207_RC_SET_NR_DEFAULT,
                    DMA_Addr.Addr,
                    EIP207_RC_CMD_SET_BITS,
                    EIP207_RC_REG_DATA_BYTE_OFFSET - 3 * sizeof(uint32_t),
                    EIP207_RC_HDR_WORD_3_RELOAD_BIT);
#endif // ADAPTER_EIP202_USE_SHDEVXS
        }

        if (!DMABuf_Handle_IsSame(&ARC4Handle, &DMABuf_NULLHandle) &&
            Adapter_RC_EIP207_ARC4RC_Enabled)
        {
            EIP202_DeviceAddress_t DMA_Addr;

            Adapter_GetPhysAddr(ARC4Handle, &DMA_Addr);

            // Invalidate the ARC4 State record in the TRC or ARC4RC
#ifdef ADAPTER_EIP202_USE_SHDEVXS
            LOG_INFO("\n\t\t\t SHDevXS_ARC4RC_Record_Invalidate \n");

            SHDevXS_ARC4RC_Record_Invalidate(DMA_Addr.Addr);
#else
            LOG_INFO("\n\t\t\t EIP207_RC_Record_Update \n");

            EIP207_RC_Record_Update(
                    Device,
                    EIP207_ARC4RC_REG_BASE,
                    EIP207_RC_SET_NR_DEFAULT,
                    DMA_Addr.Addr,
                    EIP207_RC_CMD_SET_BITS,
                    EIP207_RC_REG_DATA_BYTE_OFFSET - 3 * sizeof(uint32_t),
                    EIP207_RC_HDR_WORD_3_RELOAD_BIT);
#endif // ADAPTER_EIP202_USE_SHDEVXS
        }
    }
#else
    IDENTIFIER_NOT_USED(ARC4Handle.p);
#endif // ADAPTER_EIP202_RC_DIRECT_INVALIDATE_SUPPORT

    return PEC_STATUS_OK;
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_GetFreeSpace
 */
unsigned int
Adapter_PECDev_GetFreeSpace(
        const unsigned int InterfaceId)
{
    unsigned int FreeCDR, FreeRDR, FilledCDR, FilledRDR;
    EIP202_Ring_Error_t res;

    LOG_INFO("\n\t\t Adapter_PECDev_GetFreeSpace \n");

    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return PEC_ERROR_BAD_PARAMETER;

    LOG_INFO("\n\t\t\t EIP202_CDR_FillLevel_Get \n");

    res = EIP202_CDR_FillLevel_Get(CDR_IOArea + InterfaceId,
                                  &FilledCDR);
    if (res != EIP202_RING_NO_ERROR)
        return 0;

    if (FilledCDR > ADAPTER_EIP202_MAX_PACKETS)
        return 0;

    FreeCDR = ADAPTER_EIP202_MAX_PACKETS - FilledCDR;

    LOG_INFO("\n\t\t\t EIP202_RDR_FillLevel_Get \n");

    res = EIP202_RDR_FillLevel_Get(RDR_IOArea + InterfaceId,
                                           &FilledRDR);
    if (res != EIP202_RING_NO_ERROR)
        return 0;

    if (FilledRDR > ADAPTER_EIP202_MAX_PACKETS)
        return 0;

    FreeRDR = ADAPTER_EIP202_MAX_PACKETS - FilledRDR;

    return MIN(FreeCDR, FreeRDR);
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_PacketPut
 */
PEC_Status_t
Adapter_PECDev_Packet_Put(
        const unsigned int InterfaceId,
        const PEC_CommandDescriptor_t * Commands_p,
        const unsigned int CommandsCount,
        unsigned int * const PutCount_p)
{
    unsigned int CmdLp;
#ifdef ADAPTER_EIP202_STRICT_ARGS
    unsigned int FreeCDR,FreeRDR;
#endif
    unsigned int FilledCDR, FilledRDR, CDRIndex=0, RDRIndex=0;
    unsigned int Submitted = 0;
    EIP202_Ring_Error_t res;
    EIP202_CDR_Control_t CDR_Control;
    EIP202_RDR_Prepared_Control_t RDR_Control;

    LOG_INFO("\n\t\t Adapter_PECDev_Packet_Put \n");

    *PutCount_p = 0;
    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return PEC_ERROR_BAD_PARAMETER;

#ifdef ADAPTER_EIP202_STRICT_ARGS
    LOG_INFO("\n\t\t\t EIP202_CDR_FillLevel_Get \n");

    res = EIP202_CDR_FillLevel_Get(CDR_IOArea + InterfaceId,
                                  &FilledCDR);
    if (res != EIP202_RING_NO_ERROR)
        return PEC_ERROR_INTERNAL;

    if (FilledCDR > ADAPTER_EIP202_MAX_PACKETS)
        return PEC_ERROR_INTERNAL;

    FreeCDR = ADAPTER_EIP202_MAX_PACKETS - FilledCDR;

    LOG_INFO("\n\t\t\t EIP202_RDR_FillLevel_Get \n");

    res = EIP202_RDR_FillLevel_Get(RDR_IOArea + InterfaceId,
                                           &FilledRDR);
    if (res != EIP202_RING_NO_ERROR)
        return PEC_ERROR_INTERNAL;

    if (FilledRDR > ADAPTER_EIP202_MAX_PACKETS)
        return PEC_ERROR_INTERNAL;

    FreeRDR = ADAPTER_EIP202_MAX_PACKETS - FilledRDR;

    FreeCDR = MIN(ADAPTER_EIP202_MAX_LOGICDESCR, FreeCDR);
    FreeRDR = MIN(ADAPTER_EIP202_MAX_LOGICDESCR, FreeRDR);
#endif

    for (CmdLp = 0; CmdLp < CommandsCount; CmdLp++)
    {
        uint32_t TokenHeaderWord;
        uint8_t TokenWordCount;

#ifdef ADAPTER_EIP202_STRICT_ARGS
        if (CDRIndex == FreeCDR || RDRIndex == FreeRDR)
            break; // Run out of free descriptors in any of the rings.
#endif

        // Prepare (first) descriptor, except for source pointer/size.
        EIP202_CDR_Entries[InterfaceId][CDRIndex].SrcPacketByteCount =
                                            Commands_p[CmdLp].SrcPkt_ByteCount;

        if (DMABuf_Handle_IsSame(&Commands_p[CmdLp].Token_Handle,
                                 &DMABuf_NULLHandle))
        {
#ifdef ADAPTER_EIP202_USE_EXTENDED_DESCRIPTOR
            // Hybrid use case, token will be created inside the engine
            TokenHeaderWord = BIT_31| BIT_30 |
                              Commands_p[CmdLp].SrcPkt_ByteCount |
                              // For In-line DTLS
                (Commands_p[CmdLp].Control1 & 0x3c000000);
#elif defined(ADAPTER_EIP202_USE_INVALIDATE_COMMANDS)
            // Hybrid use case, token will be created inside the engine
            TokenHeaderWord = BIT_31 | Commands_p[CmdLp].Control2 |
                                       Commands_p[CmdLp].SrcPkt_ByteCount;
#else
            // Hybrid use case, token will be created inside the engine
            TokenHeaderWord = BIT_31 | Commands_p[CmdLp].SrcPkt_ByteCount;
#endif
            TokenWordCount = 0;
        }
        else
        {
            // Look-aside use case. token is created by the caller
            DMAResource_Handle_t DMAHandle =
                Adapter_DMABuf_Handle2DMAResourceHandle(
                                        Commands_p[CmdLp].SA_Handle1);
            DMAResource_Record_t * Rec_p = DMAResource_Handle2RecordPtr(
                                                                DMAHandle);
            if (Rec_p == NULL)
                return PEC_ERROR_INTERNAL;

#ifdef ADAPTER_EIP202_DMARESOURCE_BANKS_ENABLE
            if (Rec_p->Props.Bank != ADAPTER_EIP202_BANK_SA)
            {
                LOG_CRIT("PEC_Packet_Put: Invalid bank for SA\n");
                return PEC_ERROR_BAD_PARAMETER;
            }
#endif

            // Token header word is provided as separate parameter in Control1.
            TokenHeaderWord = Commands_p[CmdLp].Control1 & ~0x00300000;
#ifdef ADAPTER_EIP202_USE_EXTENDED_DESCRIPTOR
            TokenHeaderWord |= BIT_30;
#endif
            TokenHeaderWord |= BIT_17; // Set token header format to EIP97.
#ifdef ADAPTER_EIP202_64BIT_DEVICE
            TokenHeaderWord |= BIT_18; // Set 64-bit Context (SA) pointer
#endif
            if (Rec_p->fIsNewSA)
            {
                Rec_p->fIsNewSA = false;
            }
            else
            {
                // Enable Context Reuse auto detect if no new SA.
                TokenHeaderWord |= 0x00200000;
            }

            if (Commands_p[CmdLp].Token_WordCount > 255)
                return PEC_ERROR_INTERNAL;

            TokenWordCount = (uint8_t)Commands_p[CmdLp].Token_WordCount;
        }

        EIP202_CDR_Entries[InterfaceId][CDRIndex].TokenHeader =
                                                               TokenHeaderWord;

#ifdef ADAPTER_EIP202_USE_EXTENDED_DESCRIPTOR
        EIP202_CDR_Entries[InterfaceId][CDRIndex].Control2 =
            Commands_p[CmdLp].Control2;
        EIP202_CDR_Entries[InterfaceId][CDRIndex].Control3 =
            Commands_p[CmdLp].Control3;
#endif
        EIP202_CDR_Entries[InterfaceId][CDRIndex].ApplicationId = 0;

        CDR_Control.TokenWordCount = TokenWordCount;

        Adapter_GetPhysAddr(Commands_p[CmdLp].Token_Handle,
              &(EIP202_CDR_Entries[InterfaceId][CDRIndex].TokenDataAddr));

        CDR_Control.fFirstSegment = true;
        CDR_Control.fLastSegment = false;

        Adapter_GetPhysAddr(Commands_p[CmdLp].SA_Handle1,
              &(EIP202_CDR_Entries[InterfaceId][CDRIndex].ContextDataAddr));

#ifdef ADAPTER_EIP202_USE_POINTER_TYPES
        if (EIP202_CDR_Entries[InterfaceId][CDRIndex].ContextDataAddr.Addr !=
            ADAPTER_EIP202_DUMMY_DMA_ADDRRES)
        {
#ifndef ADAPTER_EIP202_USE_LARGE_TRANSFORM_DISABLE
            DMAResource_Handle_t DMAHandle =
                Adapter_DMABuf_Handle2DMAResourceHandle(
                                        Commands_p[CmdLp].SA_Handle1);
            DMAResource_Record_t * Rec_p = DMAResource_Handle2RecordPtr(
                                                                DMAHandle);
            if (Rec_p->fIsLargeTransform)
                EIP202_CDR_Entries[InterfaceId][CDRIndex].ContextDataAddr.Addr |=
                    ADAPTER_EIP202_TR_LARGE_ADDRESS;
            else
#endif
                EIP202_CDR_Entries[InterfaceId][CDRIndex].ContextDataAddr.Addr |=
                    ADAPTER_EIP202_TR_ADDRESS;
        }
#endif

#ifdef ADAPTER_EIP202_DMARESOURCE_BANKS_ENABLE
#ifdef ADAPTER_EIP202_RC_DMA_BANK_SUPPORT
        if (EIP202_CDR_Entries[InterfaceId][CDRIndex].ContextDataAddr.Addr !=
                                   ADAPTER_EIP202_DUMMY_DMA_ADDRRES)
            EIP202_CDR_Entries[InterfaceId][CDRIndex].ContextDataAddr.Addr -=
                                                              EIP202_SABaseAddr;

        EIP202_CDR_Entries[InterfaceId][CDRIndex].ContextDataAddr.UpperAddr = 0;
#endif // ADAPTER_EIP202_RC_DMA_BANKS_SUPPORT
#endif // ADAPTER_EIP202_DMARESOURCE_BANKS_ENABLE

        RDR_Control.fFirstSegment = true;
        RDR_Control.fLastSegment = false;
        RDR_Control.ExpectedResultWordCount = 0;

#ifdef ADAPTER_EIP202_ENABLE_SCATTERGATHER
        {
            unsigned int GatherParticles;
            unsigned int ScatterParticles;
            unsigned int RequiredCDR, RequiredRDR;
            unsigned int i;
            unsigned int GatherByteCount;

            PEC_SGList_GetCapacity(Commands_p[CmdLp].SrcPkt_Handle,
                                   &GatherParticles);
            PEC_SGList_GetCapacity(Commands_p[CmdLp].DstPkt_Handle,
                                   &ScatterParticles);

            if (GatherParticles == 0)
                RequiredCDR = 1;
            else
                RequiredCDR = GatherParticles;

            if (ScatterParticles == 0)
                RequiredRDR = 1;
            else
                RequiredRDR = ScatterParticles;

#ifndef ADAPTER_EIP202_SEPARATE_RINGS
            // If using overlapping rings, require an equal number of CDR
            // and RDR entries for the packet, the maximum of both.
            RequiredCDR = MAX(RequiredCDR,RequiredRDR);
            RequiredRDR = RequiredCDR;
#endif
            /* Check whether it will fit into the rings and the
             * prepared descriptor arrays.*/
#ifdef ADAPTER_EIP202_STRICT_ARGS
            if (CDRIndex + RequiredCDR > FreeCDR ||
                RDRIndex + RequiredRDR > FreeRDR)
                break;
#endif

            if (GatherParticles > 0)
            {
                GatherByteCount = Commands_p[CmdLp].SrcPkt_ByteCount;
                for (i=0; i<GatherParticles; i++)
                {
                    DMABuf_Handle_t ParticleHandle;
                    uint8_t * DummyPtr;
                    unsigned int ParticleSize;

                    PEC_SGList_Read(Commands_p[CmdLp].SrcPkt_Handle,
                                    i,
                                    &ParticleHandle,
                                    &ParticleSize,
                                    &DummyPtr);
                    Adapter_GetPhysAddr(ParticleHandle,
                      &(EIP202_CDR_Entries[InterfaceId][CDRIndex+i].SrcPacketAddr));
                    if (ParticleSize > GatherByteCount)
                        ParticleSize = GatherByteCount;
                    GatherByteCount -= ParticleSize;
                    // Limit the total size of the gather particles to the
                    // actual packet length.

                    CDR_Control.fLastSegment = (RequiredCDR == i + 1);
                    CDR_Control.SegmentByteCount = ParticleSize;
                    EIP202_CDR_Entries[InterfaceId][CDRIndex+i].ControlWord =
                        EIP202_CDR_Write_ControlWord(&CDR_Control);
                    CDR_Control.fFirstSegment = false;
                    CDR_Control.TokenWordCount = 0;
                }
            }
            else
            { /* No gather, use single source buffer */

                Adapter_GetPhysAddr(Commands_p[CmdLp].SrcPkt_Handle,
                 &(EIP202_CDR_Entries[InterfaceId][CDRIndex].SrcPacketAddr));
                CDR_Control.fLastSegment = (RequiredCDR == 1);
                CDR_Control.SegmentByteCount =
                    Commands_p[CmdLp].SrcPkt_ByteCount;
                EIP202_CDR_Entries[InterfaceId][CDRIndex].ControlWord =
                    EIP202_CDR_Write_ControlWord(&CDR_Control);

                CDR_Control.fFirstSegment = false;
                CDR_Control.TokenWordCount = 0;
                i = 1;
            }

            /* Add any dummy segments for overlapping rings */
            for ( ; i<RequiredCDR; i++)
            {

                CDR_Control.fLastSegment = (RequiredCDR == i + 1);
                CDR_Control.SegmentByteCount = 0;
                EIP202_CDR_Entries[InterfaceId][CDRIndex+i].ControlWord =
                    EIP202_CDR_Write_ControlWord(&CDR_Control);
            }

            if (ScatterParticles > 0)
            {
                for (i=0; i<ScatterParticles; i++)
                {
                    DMABuf_Handle_t ParticleHandle;
                    uint8_t * DummyPtr;
                    unsigned int ParticleSize;

                    PEC_SGList_Read(Commands_p[CmdLp].DstPkt_Handle,
                                    i,
                                    &ParticleHandle,
                                    &ParticleSize,
                                    &DummyPtr);
                    Adapter_GetPhysAddr(ParticleHandle,
                      &(EIP202_RDR_Prepared[InterfaceId][RDRIndex+i].DstPacketAddr));

                    RDR_Control.fLastSegment = (RequiredRDR == i + 1);
                    RDR_Control.PrepSegmentByteCount = ParticleSize;
                    EIP202_RDR_Prepared[InterfaceId][RDRIndex+i].PrepControlWord =
                        EIP202_RDR_Write_Prepared_ControlWord(&RDR_Control);

                    RDR_Control.fFirstSegment = false;
                }
            }
            else
            { /* No scatter, use single destination buffer */
                DMAResource_Record_t * Rec_p = DMAResource_Handle2RecordPtr(
                        Adapter_DMABuf_Handle2DMAResourceHandle(
                            Commands_p[CmdLp].DstPkt_Handle));

                // Extended descriptors can be used for record invalidation in
                // the Record Cache with Src and Dst buffers set to 0
#ifndef ADAPTER_EIP202_USE_EXTENDED_DESCRIPTOR
                if (Rec_p == NULL) // For non-extended descriptors only
                    return PEC_ERROR_INTERNAL;
#endif

                Adapter_GetPhysAddr(Commands_p[CmdLp].DstPkt_Handle,
                  &(EIP202_RDR_Prepared[InterfaceId][RDRIndex].DstPacketAddr));
                RDR_Control.fLastSegment = (RequiredRDR==1);

#ifdef ADAPTER_EIP202_USE_EXTENDED_DESCRIPTOR
               // For extended descriptors only, record cache invalidation
                if (Rec_p == NULL)
                    RDR_Control.PrepSegmentByteCount = 0;
                else
#endif
                    RDR_Control.PrepSegmentByteCount = Rec_p->Props.Size;

                EIP202_RDR_Prepared[InterfaceId][RDRIndex].PrepControlWord =
                    EIP202_RDR_Write_Prepared_ControlWord(&RDR_Control);

                RDR_Control.fFirstSegment = false;
                i = 1;
            }

            /* Add any dummy segments for overlapping rings */
            for ( ; i<RequiredRDR; i++)
            {
                RDR_Control.fLastSegment = (RequiredRDR == i + 1);
                RDR_Control.PrepSegmentByteCount = 0;
                EIP202_RDR_Prepared[InterfaceId][RDRIndex+i].PrepControlWord =
                    EIP202_RDR_Write_Prepared_ControlWord(&RDR_Control);
            }

            CDRIndex += RequiredCDR;
            RDRIndex += RequiredRDR;
        }
#else
        {
            // Prepare source and destination buffer in non-SG case.
            DMAResource_Record_t * Rec_p = DMAResource_Handle2RecordPtr(
                Adapter_DMABuf_Handle2DMAResourceHandle(
                    Commands_p[CmdLp].DstPkt_Handle));

            // Extended descriptors can be used for record invalidation in
            // the Record Cache with src and dst buffers set to 0
#ifndef ADAPTER_EIP202_USE_EXTENDED_DESCRIPTOR
            if (Rec_p == NULL) // For non-extended descriptors only
                return PEC_ERROR_INTERNAL;
#endif

            Adapter_GetPhysAddr(Commands_p[CmdLp].SrcPkt_Handle,
           &(EIP202_CDR_Entries[InterfaceId][CDRIndex].SrcPacketAddr));
            CDR_Control.fLastSegment = true;
            CDR_Control.SegmentByteCount = Commands_p[CmdLp].SrcPkt_ByteCount;
            EIP202_CDR_Entries[InterfaceId][CDRIndex].ControlWord =
                EIP202_CDR_Write_ControlWord(&CDR_Control);

            Adapter_GetPhysAddr(Commands_p[CmdLp].DstPkt_Handle,
                &(EIP202_RDR_Prepared[InterfaceId][RDRIndex].DstPacketAddr));
            RDR_Control.fLastSegment = true;

#ifdef ADAPTER_EIP202_USE_EXTENDED_DESCRIPTOR
            // For extended descriptors only, record cache invalidation
            if (Rec_p == NULL)
                RDR_Control.PrepSegmentByteCount = 0;
            else
#endif
                RDR_Control.PrepSegmentByteCount = Rec_p->Props.Size;

            EIP202_RDR_Prepared[InterfaceId][RDRIndex].PrepControlWord =
                EIP202_RDR_Write_Prepared_ControlWord(&RDR_Control);

            CDRIndex +=1;
            RDRIndex +=1;
        }
#endif
        *PutCount_p += 1;
    }

    LOG_INFO("\n\t\t\t EIP202_RDR_Descriptor_Prepare \n");

    res = EIP202_RDR_Descriptor_Prepare(RDR_IOArea + InterfaceId,
                                       EIP202_RDR_Prepared[InterfaceId],
                                       RDRIndex,
                                       &Submitted,
                                       &FilledRDR);
    if (res != EIP202_RING_NO_ERROR || Submitted != RDRIndex)
    {
        LOG_CRIT("Adapter_PECDev_Packet_Put: writing prepared descriptors"
                 "error code %d count=%d expected=%d\n",
                 res, Submitted, RDRIndex);
        return PEC_ERROR_INTERNAL;
    }

    LOG_INFO("\n\t\t\t EIP202_CDR_Descriptor_Put \n");

    res = EIP202_CDR_Descriptor_Put(CDR_IOArea + InterfaceId,
                                   EIP202_CDR_Entries[InterfaceId],
                                   CDRIndex,
                                   &Submitted,
                                   &FilledCDR);
    if (res != EIP202_RING_NO_ERROR || Submitted != CDRIndex)
    {
        LOG_CRIT("Adapter_PECDev_Packet_Put: writing command descriptors"
                 "error code %d count=%d expected=%d\n",
                 res, Submitted, CDRIndex);
        return PEC_ERROR_INTERNAL;
    }

    return PEC_STATUS_OK;
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_Packet_Get
 */
PEC_Status_t
Adapter_PECDev_Packet_Get(
        const unsigned int InterfaceId,
        PEC_ResultDescriptor_t * Results_p,
        const unsigned int ResultsLimit,
        unsigned int * const GetCount_p)
{
    unsigned int ResLp;
    unsigned int DscrCount;
    unsigned int DscrDoneCount;
    unsigned int ResIndex;

    EIP202_Ring_Error_t res;

    LOG_INFO("\n\t\t Adapter_PECDev_Packet_Get \n");

    *GetCount_p = 0;

    if (ResultsLimit == 0)
        return PEC_STATUS_OK;

    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return PEC_ERROR_BAD_PARAMETER;

    LOG_INFO("\n\t\t\t EIP202_RDR_Descriptor_Get \n");

    // Assume that we do get the requested number of descriptors
    // as they were reported available.
    res = EIP202_RDR_Descriptor_Get(RDR_IOArea + InterfaceId,
                                   EIP202_RDR_Entries[InterfaceId],
                                   ResultsLimit, // Max number of packets.
                                   ADAPTER_EIP202_MAX_LOGICDESCR,
                                   &DscrDoneCount,
                                   &DscrCount);
    if (res != EIP202_RING_NO_ERROR)
        return PEC_ERROR_INTERNAL;

    ResIndex = 0;
    for (ResLp = 0; ResLp < ResultsLimit; ResLp++)
    {
        EIP202_RDR_Result_Token_t ResultToken;
        EIP202_RDR_Result_Control_t ControlWord;
#ifndef ADAPTER_EIP202_USE_EXTENDED_DESCRIPTOR
        EIP202_RDR_BypassData_t BD;
#endif
        bool fEncounteredFirst = false;

        if (ResIndex >= DscrDoneCount)
            break;

        for (;;)
        {
            LOG_INFO("\n\t\t\t EIP202_RDR_Read_Processed_ControlWord \n");

            EIP202_RDR_Read_Processed_ControlWord(
                EIP202_RDR_Entries[InterfaceId] + ResIndex,
                &ControlWord,
                &ResultToken);

            if ( ControlWord.fFirstSegment)
                fEncounteredFirst = true;

            if ( ControlWord.fLastSegment && fEncounteredFirst)
                break; // Last segment of packet, only valid when
                       // fist segment was encountered.
            ResIndex++;

            // There may be unused scatter particles after the last segment
            // that must be skipped.
            if (ResIndex >= DscrDoneCount)
                return PEC_STATUS_OK;
        }

        Results_p[ResLp].DstPkt_ByteCount = ResultToken.PacketByteCount;

        // Copy the EIP-96 error bits from the first EIP-96 result token word
        Results_p[ResLp].Status1 =
            EIP202_RDR_Entries[InterfaceId][ResIndex].ResultTokenData[0];

        // Copy the Buffer overflow (BIT_21) and Descriptor overflow (BIT_20)
        // EIP-202 error bits from the first EIP-202 result descriptor word
        Results_p[ResLp].Status1 |=
                ((EIP202_RDR_Entries[InterfaceId][ResIndex].ProcControlWord &
                                                (BIT_21 | BIT_20)) >> 6);

        Results_p[ResLp].Status2 =
                (ResultToken.PadByteCount << 8) | ResultToken.NextHeader;

        Results_p[ResLp].Bypass_WordCount = ResultToken.BypassWordCount;


#ifdef EIP202_USE_EXTENDED_DESCRIPTOR
        Results_p[ResLp].Status3 =
            EIP202_RDR_Entries[InterfaceId][ResIndex].ResultTokenData[2];
        Results_p[ResLp].Status4 =
            EIP202_RDR_Entries[InterfaceId][ResIndex].ResultTokenData[4];
        Results_p[ResLp].Status5 =
            EIP202_RDR_Entries[InterfaceId][ResIndex].ResultTokenData[5];
        Results_p[ResLp].Status6 =
            EIP202_RDR_Entries[InterfaceId][ResIndex].ResultTokenData[1];
#else
        EIP202_RDR_Read_Processed_BypassData(&ResultToken, &BD);

        Results_p[ResLp].Status3 = 0;
        Results_p[ResLp].Status4 = 0;

        if (ResultToken.BypassWordCount == 1)
        {
            Results_p[ResLp].Status2 |= (BD.Fail.ErrorFlags << 16);
        }
        else if (ResultToken.BypassWordCount == 2)
        {
            Results_p[ResLp].Status2 |= (BD.Pass.TOS_TC << 16);
            Results_p[ResLp].Status2 |= (BD.Pass.fDF ?
                                             ADAPTER_EIP202_DF_BIT : 0);
            Results_p[ResLp].Status3 = BD.Pass.HdrProcCtxRef;
            Results_p[ResLp].Status4 = BD.Pass.NextHeaderOffset;
        }
#endif

        *GetCount_p += 1;
        ResIndex++;
    }

    return PEC_STATUS_OK;
}


#ifdef ADAPTER_EIP202_ENABLE_SCATTERGATHER
/*----------------------------------------------------------------------------
 * Adapter_PECDev_TestSG
 */
bool
Adapter_PECDev_TestSG(
    const unsigned int InterfaceId,
    const unsigned int GatherParticleCount,
    const unsigned int ScatterParticleCount)
{
    unsigned int GCount = GatherParticleCount;
    unsigned int SCount = ScatterParticleCount;
    unsigned int FreeCDR, FreeRDR, FilledCDR, FilledRDR;
    EIP202_Ring_Error_t res;

    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return false;

    if (GCount == 0)
        GCount = 1;

    if (SCount == 0)
        SCount = 1;

#ifndef ADAPTER_EIP202_SEPARATE_RINGS
    GCount = MAX(GCount, SCount);
    SCount = GCount;
#endif

    if (GCount > ADAPTER_EIP202_MAX_LOGICDESCR ||
        SCount > ADAPTER_EIP202_MAX_LOGICDESCR)
        return false;

    LOG_INFO("\n\t\t\t EIP202_CDR_FillLevel_Get \n");

    res = EIP202_CDR_FillLevel_Get(CDR_IOArea + InterfaceId,
                                  &FilledCDR);
    if (res != EIP202_RING_NO_ERROR)
        return false;

    if (FilledCDR > ADAPTER_EIP202_MAX_PACKETS)
        return false;

    FreeCDR = ADAPTER_EIP202_MAX_PACKETS - FilledCDR;

    LOG_INFO("\n\t\t\t EIP202_RDR_FillLevel_Get \n");

    res = EIP202_RDR_FillLevel_Get(RDR_IOArea + InterfaceId,
                                           &FilledRDR);
    if (res != EIP202_RING_NO_ERROR)
        return false;

    if (FilledRDR > ADAPTER_EIP202_MAX_PACKETS)
        return false;

    FreeRDR = ADAPTER_EIP202_MAX_PACKETS - FilledRDR;

    return (FreeCDR >= GCount && FreeRDR >= SCount);
}
#endif // ADAPTER_EIP202_ENABLE_SCATTERGATHER


#ifdef ADAPTER_EIP202_INTERRUPTS_ENABLE
/* Adapter_PECDev_IRQToInteraceID
 */
unsigned int
Adapter_PECDev_IRQToInferfaceId(
        const int nIRQ)
{
    unsigned int i, IRQ_Nr;

    if (nIRQ < 0)
        return 0;

    IRQ_Nr = (unsigned int)nIRQ;

    for (i = 0; i < ADAPTER_EIP202_DEVICE_COUNT; i++)
    {
        if (IRQ_Nr == EIP202_Devices[i].RDR_IRQ_ID ||
            IRQ_Nr == EIP202_Devices[i].CDR_IRQ_ID)
        {
            return i;
        }
    }

    LOG_CRIT("Adapter_PECDev_IRQToInterfaceId: unknown interrupt %d\n",nIRQ);

    return 0;
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_Enable_ResultIRQ
 */
void
Adapter_PECDev_Enable_ResultIRQ(
        const unsigned int InterfaceId)
{
    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return;

    LOG_INFO(
      "\n\t\t\t EIP202_RDR_Processed_FillLevel_High_INT_ClearAndDisable \n");

    EIP202_RDR_Processed_FillLevel_High_INT_ClearAndDisable (
        RDR_IOArea + InterfaceId,
        false);

    LOG_INFO("\n\t\t\t EIP202_RDR_Processed_FillLevel_High_INT_Enable \n");

    EIP202_RDR_Processed_FillLevel_High_INT_Enable(
        RDR_IOArea + InterfaceId,
        ADAPTER_EIP202_DESCRIPTORDONECOUNT,
        ADAPTER_EIP202_DESCRIPTORDONETIMEOUT,
        true);

    Adapter_Interrupt_Enable(EIP202_Devices[InterfaceId].RDR_IRQ_ID,
                             EIP202_Devices[InterfaceId].RDR_IRQ_Flags);
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_Disable_ResultIRQ
 */
void
Adapter_PECDev_Disable_ResultIRQ(
        const unsigned int InterfaceId)
{
    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return;

    LOG_INFO(
       "\n\t\t\t EIP202_RDR_Processed_FillLevel_High_INT_ClearAndDisable \n");

    EIP202_RDR_Processed_FillLevel_High_INT_ClearAndDisable (
        RDR_IOArea + InterfaceId,
        false);

    Adapter_Interrupt_Disable(EIP202_Devices[InterfaceId].RDR_IRQ_ID,
                              EIP202_Devices[InterfaceId].RDR_IRQ_Flags);
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_Enable_CommandIRQ
 */
void
Adapter_PECDev_Enable_CommandIRQ(
        const unsigned int InterfaceId)
{
    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return;

    LOG_INFO("\n\t\t\t EIP202_CDR_FillLevel_Low_INT_Enable \n");

    EIP202_CDR_FillLevel_Low_INT_Enable(
        CDR_IOArea + InterfaceId,
        ADAPTER_EIP202_DESCRIPTORDONECOUNT,
        ADAPTER_EIP202_DESCRIPTORDONETIMEOUT);

    Adapter_Interrupt_Enable(EIP202_Devices[InterfaceId].CDR_IRQ_ID,
                             EIP202_Devices[InterfaceId].CDR_IRQ_Flags);
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_Disable_CommandIRQ
 */
void
Adapter_PECDev_Disable_CommandIRQ(
        const unsigned int InterfaceId)
{
    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return;

    LOG_INFO("\n\t\t\t EIP202_CDR_FillLevel_Low_INT_ClearAndDisable \n");

    EIP202_CDR_FillLevel_Low_INT_ClearAndDisable(
        CDR_IOArea + InterfaceId);

    Adapter_Interrupt_Disable(EIP202_Devices[InterfaceId].CDR_IRQ_ID,
                              EIP202_Devices[InterfaceId].CDR_IRQ_Flags);
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_SetResultHandler
 */
void Adapter_PECDev_SetResultHandler(
        const unsigned int InterfaceId,
        Adapter_InterruptHandler_t HandlerFunction)
{
    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return;

    Adapter_Interrupt_SetHandler(EIP202_Devices[InterfaceId].RDR_IRQ_ID,
                                 HandlerFunction);
}


/*----------------------------------------------------------------------------
 * Adapter_PECDev_SetCommandHandler
 */
void Adapter_PECDev_SetCommandHandler(
        const unsigned int InterfaceId,
        Adapter_InterruptHandler_t HandlerFunction)
{
    if (InterfaceId >= ADAPTER_EIP202_DEVICE_COUNT)
        return;

    Adapter_Interrupt_SetHandler(EIP202_Devices[InterfaceId].CDR_IRQ_ID,
                                 HandlerFunction);
}
#endif // ADAPTER_EIP202_INTERRUPTS_ENABLE


/* end of file adapter_ring_eip202.c */
