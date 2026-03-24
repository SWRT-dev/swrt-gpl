/* adapter_global_eip97.c
 *
 * SafeXcel-IP-97 Global Control Adapter
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
#include "api_global_eip97.h"


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default Adapter configuration
#include "c_adapter_global.h"

#ifndef GLOBALCONTROL_BUILD
#include "adapter_ring_eip202.h" // Ring EIP-202 interface to pass
                                 // config params from Global Control
#endif

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // uint8_t, uint32_t, bool, IDENTIFIER_NOT_USED

// Driver Framework C Library API
#include "clib.h"               // memcpy

// EIP-97 Driver Library Global Control API
#include "eip97_global_event.h" // Event Management
#include "eip97_global_init.h"  // Init/Uninit
#include "eip97_global_prng.h"  // PRNG Control

#include "device_types.h"  // Device_Handle_t
#include "device_mgmt.h" // Device_find
#include "log.h" // Log API

#ifdef GLOBALCONTROL_BUILD
#include "shdevxs_init.h"  // SHDevXS_Global_init()
#endif

/*----------------------------------------------------------------------------
 * Definitions and macros
 */


/*----------------------------------------------------------------------------
 * Local variables
 */
static EIP97_Global_IOArea_t Global_IOArea;
static bool Global_IsInitialized;

static const  GlobalControl97_Capabilities_t Global_CapabilitiesString =
{
  "EIP-97 v_._p_  with EIP-202 v_._p_ and EIP-96 v_._p_, "
  "#PE=__ #rings=__"// szTextDescription
};


/*----------------------------------------------------------------------------
 * YesNo
 */
static const char *
YesNo(
        const bool b)
{
    if (b)
        return "Yes";
    else
        return "No";
}


/*----------------------------------------------------------------------------
 * GlobalControl97_Capabilities_Get
 */
GlobalControl97_Error_t
GlobalControl97_Capabilities_Get(
        GlobalControl97_Capabilities_t * const Capabilities_p)
{
    uint8_t Versions[13];

    LOG_INFO("\n\t\t\t GlobalControl97_Capabilities_Get \n");

    memcpy(Capabilities_p, &Global_CapabilitiesString,
           sizeof(Global_CapabilitiesString));

    {
        EIP97_Global_Error_t rc;
        EIP97_Global_Capabilities_t Capabilities;

        ZEROINIT(Capabilities);

        LOG_INFO("\n\t\t\t\t EIP97_Global_HWRevision_Get \n");

        rc = EIP97_Global_HWRevision_Get(&Global_IOArea, &Capabilities);

        if (rc != EIP97_GLOBAL_NO_ERROR)
        {
            LOG_CRIT("GlobalControl97_Capabilities_Get: returned error");
            return GLOBAL_CONTROL_ERROR_INTERNAL;
        }

        // Show those capabilities not propagated to higher layer.
        LOG_CRIT("GlobalControl97_Capabilities_Get\n");
        LOG_CRIT("EIP202: PEs=%d rings=%d 64-bit=%s, fill level extension=%s\n"
                 "CF size=%d RF size=%d DMA len = %d "
                 "Align=%d HDW=%d HostIfc=%d\n",
                 Capabilities.EIP202_Options.NofPes,
                 Capabilities.EIP202_Options.NofRings,
                 YesNo(Capabilities.EIP202_Options.fAddr64),
                 YesNo(Capabilities.EIP202_Options.fExpPlf),
                 Capabilities.EIP202_Options.CF_Size,
                 Capabilities.EIP202_Options.RF_Size,
                 Capabilities.EIP202_Options.DMA_Len,
                 Capabilities.EIP202_Options.TgtAlign,
                 Capabilities.EIP202_Options.HDW,
                 Capabilities.EIP202_Options.HostIfc);
        LOG_CRIT("EIP96 options:\n"
                 "AES: %s with CFB/OFB: %s Fast: %s\n"
                 "DES: %s with CFB/OFB: %s Fast: %s\n"
                 "ARCFOUR level: %d\n"
                 "MD5: %s SHA1: %s Fast: %s SHA256: %s SHA512: %s\n"
                 "(X)CBC-MAC: %s Fast: %s All key sizes: %s GHASH %s\n",
                 YesNo(Capabilities.EIP96_Options.fAES),
                 YesNo(Capabilities.EIP96_Options.fAESfb),
                 YesNo(Capabilities.EIP96_Options.fAESspeed),
                 YesNo(Capabilities.EIP96_Options.fDES),
                 YesNo(Capabilities.EIP96_Options.fDESfb),
                 YesNo(Capabilities.EIP96_Options.fDESspeed),
                 Capabilities.EIP96_Options.ARC4,
                 YesNo(Capabilities.EIP96_Options.fMD5),
                 YesNo(Capabilities.EIP96_Options.fSHA1),
                 YesNo(Capabilities.EIP96_Options.fSHA1speed),
                 YesNo(Capabilities.EIP96_Options.fSHA224_256),
                 YesNo(Capabilities.EIP96_Options.fSHA384_512),
                 YesNo(Capabilities.EIP96_Options.fXCBC_MAC),
                 YesNo(Capabilities.EIP96_Options.fCBC_MACspeed),
                 YesNo(Capabilities.EIP96_Options.fCBC_MACkeylens),
                 YesNo(Capabilities.EIP96_Options.fGHASH));
        LOG_CRIT("EIP97 options: PEs=%d, In Dbuf size=%d In Tbuf size=%d,"
                 " Out Dbuf size=%d, Out Tbuf size=%d, \n"
                 "Token Generator: %s, Transform Record Cache: %s\n",
                 Capabilities.EIP97_Options.NofPes,
                 Capabilities.EIP97_Options.in_dbuf_size,
                 Capabilities.EIP97_Options.in_tbuf_size,
                 Capabilities.EIP97_Options.out_dbuf_size,
                 Capabilities.EIP97_Options.out_tbuf_size,
                 YesNo(Capabilities.EIP97_Options.tg),
                 YesNo(Capabilities.EIP97_Options.trc));
        LOG_CRIT("EIP206 options: PE type=%d InClassifier=%d OutClassifier=%d "
                 "MAC chans=%d \n"
                 "InDBuf=%dkB InTBuf=%dkB OutDBuf=%dkB OutTBuf=%dkB\n",
                 Capabilities.EIP206_Options.PE_Type,
                 Capabilities.EIP206_Options.InClassifier,
                 Capabilities.EIP206_Options.OutClassifier,
                 Capabilities.EIP206_Options.NofMAC_Channels,
                 Capabilities.EIP206_Options.InDbufSizeKB,
                 Capabilities.EIP206_Options.InTbufSizeKB,
                 Capabilities.EIP206_Options.OutDbufSizeKB,
                 Capabilities.EIP206_Options.OutTbufSizeKB);

        Versions[0] = Capabilities.EIP97_Version.MajHWRevision;
        Versions[1] = Capabilities.EIP97_Version.MinHWRevision;
        Versions[2] = Capabilities.EIP97_Version.HWPatchLevel;

        Versions[3] = Capabilities.EIP202_Version.MajHWRevision;
        Versions[4] = Capabilities.EIP202_Version.MinHWRevision;
        Versions[5] = Capabilities.EIP202_Version.HWPatchLevel;

        Versions[6] = Capabilities.EIP96_Version.MajHWRevision;
        Versions[7] = Capabilities.EIP96_Version.MinHWRevision;
        Versions[8] = Capabilities.EIP96_Version.HWPatchLevel;

        Versions[9]  = Capabilities.EIP202_Options.NofPes / 10;
        Versions[10] = Capabilities.EIP202_Options.NofPes % 10;
        Versions[11] = Capabilities.EIP202_Options.NofRings / 10;
        Versions[12] = Capabilities.EIP202_Options.NofRings % 10;
    }

    {
        char * p = Capabilities_p->szTextDescription;
        int VerIndex = 0;
        int i = 0;

        while(p[i])
        {
            if (p[i] == '_')
            {
                if (Versions[VerIndex] > 9)
                    p[i] = '?';
                else
                    p[i] = '0' + Versions[VerIndex];

                VerIndex++;
            }

            i++;
        }
    }

    return GLOBAL_CONTROL_NO_ERROR;
}


/*----------------------------------------------------------------------------
 * GlobalControl97_Init
 */
GlobalControl97_Error_t
GlobalControl97_Init(
        const bool fHWResetDone)
{
    EIP97_Global_Error_t rc;
    Device_Handle_t dev = Device_Find(ADAPTER_GLOBAL_DEVICE_NAME);

    LOG_INFO("\n\t\t\t GlobalControl97_Init \n");

    if (Global_IsInitialized)
    {
        LOG_CRIT("GlobalControl97_Init: called while already initialized\n");
        return GLOBAL_CONTROL_ERROR_BAD_USE_ORDER;
    }

    if (dev == NULL)
    {
        LOG_CRIT("GlobalControl97_Init: Could not find device\n");
        return GLOBAL_CONTROL_ERROR_INTERNAL;
    }

    if (!fHWResetDone)
    {
        LOG_INFO("\n\t\t\t\t EIP97_Global_Reset \n");

        // Need to do a software reset first.
        rc = EIP97_Global_Reset(&Global_IOArea, dev);
        if (rc == EIP97_GLOBAL_BUSY_RETRY_LATER)
        {
            unsigned int tries = 0;
            do {
                LOG_INFO("\n\t\t\t\t EIP97_Global_Reset_IsDone \n");

                rc = EIP97_Global_Reset_IsDone(&Global_IOArea);
                if (rc != EIP97_GLOBAL_NO_ERROR &&
                    rc != EIP97_GLOBAL_BUSY_RETRY_LATER)
                {
                    LOG_CRIT("GlobalControl97_Init:"
                             " Error from EIP97_Global_Reset_IsDone\n");
                    return GLOBAL_CONTROL_ERROR_INTERNAL;
                }
                tries ++;
                if (tries > ADAPTER_GLOBAL_RESET_MAX_RETRIES)
                {
                    LOG_CRIT("GlobalControl97_Init: Reset timeout\n");
                    return GLOBAL_CONTROL_ERROR_INTERNAL;

                }
            } while (rc == EIP97_GLOBAL_BUSY_RETRY_LATER);
        }
        else if (rc != EIP97_GLOBAL_NO_ERROR)
        {
            LOG_CRIT("GlobalControl97_Init: Error from EIP97_Global_Reset\n");
            return GLOBAL_CONTROL_ERROR_INTERNAL;
        }
    }

    LOG_INFO("\n\t\t\t\t EIP97_Global_Init \n");

    rc = EIP97_Global_Init(&Global_IOArea, dev);
    if (rc != EIP97_GLOBAL_NO_ERROR)
    {
        LOG_CRIT("GlobalControl97_Init: Error from EIP97_Global_Init\n");
        return GLOBAL_CONTROL_ERROR_INTERNAL;
    }
    else
    {
        EIP97_Global_Error_t rc;
        EIP97_Global_Capabilities_t Capabilities;

        Global_IsInitialized = true;

        rc = EIP97_Global_HWRevision_Get(&Global_IOArea, &Capabilities);
        if (rc != EIP97_GLOBAL_NO_ERROR)
        {
            LOG_CRIT("GlobalControl97_Init: returned error");
            return GLOBAL_CONTROL_ERROR_INTERNAL;
        }

#ifndef GLOBALCONTROL_BUILD
        // Pass HW default configuration parameters obtained via
        // the Global Control interface to the Ring Control
        // for its automatic configuration
        Adapter_Ring_EIP202_Configure(Capabilities.EIP202_Options.HDW,
                                      Capabilities.EIP202_Options.CF_Size,
                                      Capabilities.EIP202_Options.RF_Size);
#endif

#ifdef GLOBALCONTROL_BUILD
        if (SHDevXS_Global_Init() != 0)
        {
            LOG_CRIT(
                "GlobalControl97_Init: SHDevXS_Global_Init() returned error");
            return GLOBAL_CONTROL_ERROR_INTERNAL;
        }

#endif

        return GLOBAL_CONTROL_NO_ERROR;
    }
}


/*----------------------------------------------------------------------------
 * GlobalControl97_UnInit
 */
GlobalControl97_Error_t
GlobalControl97_UnInit(void)
{
    EIP97_Global_Error_t rc;
    Device_Handle_t dev = Device_Find(ADAPTER_GLOBAL_DEVICE_NAME);

    LOG_INFO("\n\t\t\t GlobalControl97_UnInit \n");

    if (!Global_IsInitialized)
    {
        LOG_CRIT("GlobalControl97_UnInit: called while not initialized\n");
        return GLOBAL_CONTROL_ERROR_BAD_USE_ORDER;
    }

#ifdef GLOBALCONTROL_BUILD
    SHDevXS_Global_UnInit();
#endif

    if (dev == NULL)
    {
        LOG_CRIT("GlobalControl97_UnInit: Could not find device\n");
        return GLOBAL_CONTROL_ERROR_INTERNAL;
    }

    LOG_INFO("\n\t\t\t\t EIP97_Global_Reset \n");

    rc = EIP97_Global_Reset(&Global_IOArea, dev);
    if (rc == EIP97_GLOBAL_BUSY_RETRY_LATER)
    {
        unsigned int tries = 0;
        do {
            LOG_INFO("\n\t\t\t\t EIP97_Global_Reset_IsDone \n");

            rc = EIP97_Global_Reset_IsDone(&Global_IOArea);
            if (rc != EIP97_GLOBAL_NO_ERROR &&
                rc != EIP97_GLOBAL_BUSY_RETRY_LATER)
            {
                LOG_CRIT("GlobalControl97_UnInit:"
                         " Error from EIP97_Global_Reset_IsDone\n");
                return GLOBAL_CONTROL_ERROR_INTERNAL;
            }
                tries ++;
                if (tries > ADAPTER_GLOBAL_RESET_MAX_RETRIES)
                {
                    LOG_CRIT("GlobalControl97_UnInit: Reset timeout\n");
                    return GLOBAL_CONTROL_ERROR_INTERNAL;

                }
        } while (rc == EIP97_GLOBAL_BUSY_RETRY_LATER);
    }
    else if (rc != EIP97_GLOBAL_NO_ERROR)
    {
        LOG_CRIT("GlobalControl97_Init: Error from EIP97_Global_Reset\n");
        return GLOBAL_CONTROL_ERROR_INTERNAL;
    }

    Global_IsInitialized = false;
    return GLOBAL_CONTROL_NO_ERROR;
}


/*----------------------------------------------------------------------------
 * GlobalControl97_Configure
 */
GlobalControl97_Error_t
GlobalControl97_Configure(
        const unsigned int PE_Number,
        const GlobalControl97_Ring_PE_Map_t * const RingPEMap_p)
{
    EIP97_Global_Error_t rc;

    LOG_INFO("\n\t\t\t GlobalControl97_Configure \n");

    LOG_INFO("\n\t\t\t\t EIP97_Global_Configure \n");

    rc = EIP97_Global_Configure(&Global_IOArea, PE_Number, RingPEMap_p);
    if (rc == EIP97_GLOBAL_NO_ERROR)
        return GLOBAL_CONTROL_NO_ERROR;
    else if (rc == EIP97_GLOBAL_ARGUMENT_ERROR)
        return GLOBAL_CONTROL_ERROR_BAD_PARAMETER;
    else
        return GLOBAL_CONTROL_ERROR_INTERNAL;
}


/*----------------------------------------------------------------------------
 * GlobalControl97_PRNG_Reseed
 */
GlobalControl97_Error_t
GlobalControl97_PRNG_Reseed(
        const unsigned int PE_Number,
        const GlobalControl97_PRNG_Reseed_t * const ReseedData_p)
{
    EIP97_Global_Error_t rc;

    LOG_INFO("\n\t\t\t GlobalControl97_PRNG_Reseed \n");

    LOG_INFO("\n\t\t\t\t EIP97_Global_PRNG_Reseed \n");

    rc = EIP97_Global_PRNG_Reseed(&Global_IOArea, PE_Number, ReseedData_p);
    if (rc == EIP97_GLOBAL_NO_ERROR)
        return GLOBAL_CONTROL_NO_ERROR;
    else if (rc == EIP97_GLOBAL_ARGUMENT_ERROR)
        return GLOBAL_CONTROL_ERROR_BAD_PARAMETER;
    else
        return GLOBAL_CONTROL_ERROR_INTERNAL;

}


/*----------------------------------------------------------------------------
 * GlobalControl97_DFE_Status_Get
 */
GlobalControl97_Error_t
GlobalControl97_DFE_Status_Get(
        const unsigned int PE_Number,
        GlobalControl97_DFE_Status_t * const DFE_Status_p)
{
    EIP97_Global_Error_t rc;

    LOG_INFO("\n\t\t\t GlobalControl97_DFE_Status_Get \n");

    LOG_INFO("\n\t\t\t\t EIP97_Global_DFE_Status_Get \n");

    rc = EIP97_Global_DFE_Status_Get(&Global_IOArea, PE_Number, DFE_Status_p);
    if (rc == EIP97_GLOBAL_NO_ERROR)
        return GLOBAL_CONTROL_NO_ERROR;
    else if (rc == EIP97_GLOBAL_ARGUMENT_ERROR)
        return GLOBAL_CONTROL_ERROR_BAD_PARAMETER;
    else
        return GLOBAL_CONTROL_ERROR_INTERNAL;
}


/*----------------------------------------------------------------------------
 * GlobalControl97_DSE_Status_Get
 */
GlobalControl97_Error_t
GlobalControl97_DSE_Status_Get(
        const unsigned int PE_Number,
        GlobalControl97_DSE_Status_t * const DSE_Status_p)
{
    EIP97_Global_Error_t rc;

    LOG_INFO("\n\t\t\t GlobalControl97_DSE_Status_Get \n");

    LOG_INFO("\n\t\t\t\t EIP97_Global_DSE_Status_Get \n");

    rc = EIP97_Global_DSE_Status_Get(&Global_IOArea, PE_Number, DSE_Status_p);
    if (rc == EIP97_GLOBAL_NO_ERROR)
        return GLOBAL_CONTROL_NO_ERROR;
    else if (rc == EIP97_GLOBAL_ARGUMENT_ERROR)
        return GLOBAL_CONTROL_ERROR_BAD_PARAMETER;
    else
        return GLOBAL_CONTROL_ERROR_INTERNAL;
}


/*----------------------------------------------------------------------------
 * GlobalControl97_Token_Status_Get
 */
GlobalControl97_Error_t
GlobalControl97_Token_Status_Get(
        const unsigned int PE_Number,
        GlobalControl97_Token_Status_t * const Token_Status_p)
{
    EIP97_Global_Error_t rc;

    LOG_INFO("\n\t\t\t GlobalControl97_Token_Status_Get \n");

    LOG_INFO("\n\t\t\t\t EIP97_Global_EIP96_Token_Status_Get \n");

    rc = EIP97_Global_EIP96_Token_Status_Get(&Global_IOArea,
                                             PE_Number,
                                             Token_Status_p);
    if (rc == EIP97_GLOBAL_NO_ERROR)
        return GLOBAL_CONTROL_NO_ERROR;
    else if (rc == EIP97_GLOBAL_ARGUMENT_ERROR)
        return GLOBAL_CONTROL_ERROR_BAD_PARAMETER;
    else
        return GLOBAL_CONTROL_ERROR_INTERNAL;
}


/*----------------------------------------------------------------------------
 * GlobalControl97_Context_Status_Get
 */
GlobalControl97_Error_t
GlobalControl97_Context_Status_Get(
        const unsigned int PE_Number,
        GlobalControl97_Context_Status_t * const Context_Status_p)
{
    EIP97_Global_Error_t rc;

    LOG_INFO("\n\t\t\t GlobalControl97_Context_Status_Get \n");

    LOG_INFO("\n\t\t\t\t EIP97_Global_EIP96_Context_Status_Get \n");

    rc = EIP97_Global_EIP96_Context_Status_Get(&Global_IOArea,
                                               PE_Number,
                                               Context_Status_p);
    if (rc == EIP97_GLOBAL_NO_ERROR)
        return GLOBAL_CONTROL_NO_ERROR;
    else if (rc == EIP97_GLOBAL_ARGUMENT_ERROR)
        return GLOBAL_CONTROL_ERROR_BAD_PARAMETER;
    else
        return GLOBAL_CONTROL_ERROR_INTERNAL;
}


/*----------------------------------------------------------------------------
 * GlobalControl97_Interrupt_Status_Get
 */
GlobalControl97_Error_t
GlobalControl97_Interrupt_Status_Get(
        const unsigned int PE_Number,
        GlobalControl97_Interrupt_Status_t * const Interrupt_Status_p)
{
    IDENTIFIER_NOT_USED(PE_Number);

    LOG_INFO("\n\t\t\t GlobalControl97_Interrupt_Status_Get \n");

    LOG_INFO("\n\t\t\t\t EIP97_Global_EIP96_Interrupt_Status_Get \n");

    // Not implemented yet, must use EIP-96 AIC
    ZEROINIT(*Interrupt_Status_p);

    return GLOBAL_CONTROL_NO_ERROR;
}


/*----------------------------------------------------------------------------
 * GlobalControl97_OutXfer_Status_Get
 */
GlobalControl97_Error_t
GlobalControl97_OutXfer_Status_Get(
        const unsigned int PE_Number,
        GlobalControl97_Output_Transfer_Status_t * const OutXfer_Status_p)
{
    EIP97_Global_Error_t rc;

    LOG_INFO("\n\t\t\t GlobalControl97_OutXfer_Status_Get \n");

    LOG_INFO("\n\t\t\t\t EIP97_Global_EIP96_OutXfer_Status_Get \n");

    rc = EIP97_Global_EIP96_OutXfer_Status_Get(&Global_IOArea,
                                               PE_Number,
                                               OutXfer_Status_p);
    if (rc == EIP97_GLOBAL_NO_ERROR)
        return GLOBAL_CONTROL_NO_ERROR;
    else if (rc == EIP97_GLOBAL_ARGUMENT_ERROR)
        return GLOBAL_CONTROL_ERROR_BAD_PARAMETER;
    else
        return GLOBAL_CONTROL_ERROR_INTERNAL;
}


/*----------------------------------------------------------------------------
 * GlobalControl97_PRNG_Status_Get
 */
GlobalControl97_Error_t
GlobalControl97_PRNG_Status_Get(
        const unsigned int PE_Number,
        GlobalControl97_PRNG_Status_t * const PRNG_Status_p)
{
    EIP97_Global_Error_t rc;

    LOG_INFO("\n\t\t\t GlobalControl97_PRNG_Status_Get \n");

    LOG_INFO("\n\t\t\t\t EIP97_Global_EIP96_PRNG_Status_Get \n");

    rc = EIP97_Global_EIP96_PRNG_Status_Get(&Global_IOArea,
                                            PE_Number,
                                            PRNG_Status_p);
    if (rc == EIP97_GLOBAL_NO_ERROR)
        return GLOBAL_CONTROL_NO_ERROR;
    else if (rc == EIP97_GLOBAL_ARGUMENT_ERROR)
        return GLOBAL_CONTROL_ERROR_BAD_PARAMETER;
    else
        return GLOBAL_CONTROL_ERROR_INTERNAL;
}


/* end of file adapter_global_eip97.c */

