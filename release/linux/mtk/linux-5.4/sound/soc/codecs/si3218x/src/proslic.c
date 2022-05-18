// SPDX-License-Identifier: GPL-2.0
/*
 * Distributed by:
 * Silicon Laboratories, Inc
 *
 * This file contains proprietary information.
 * No dissemination allowed without prior written permission from
 * Silicon Laboratories, Inc.
 *
 * File Description:
 * This is the generic interface file for the ProSLIC drivers.
 *
 * Customers should be calling this level for ProSLIC specific
 * functions (vs. chipset specific versions of the code)
 *
 */

#include "../config_inc/si_voice_datatypes.h"
#include "../inc/si_voice_ctrl.h"
#include "../inc/si_voice_timer_intf.h"
#include "../inc/proslic.h"
#include "../config_inc/proslic_api_config.h"

#ifdef ENABLE_DEBUG
#define LOGPRINT_PREFIX "ProSLIC:"
#endif

#ifdef SI3217X
#include "si3217x.h"
#include "si3217x_intf.h"
extern Si3217x_General_Cfg Si3217x_General_Configuration;
#ifndef DISABLE_FSK_SETUP
extern ProSLIC_FSK_Cfg Si3217x_FSK_Presets[];
#endif
#ifndef DISABLE_TONE_SETUP
extern ProSLIC_Tone_Cfg Si3217x_Tone_Presets[];
#endif

#endif /* 17X */

#ifdef SI3218X
#include "../inc/si3218x.h"
#include "../inc/si3218x_intf.h"
extern Si3218x_General_Cfg Si3218x_General_Configuration;
#ifndef DISABLE_FSK_SETUP
extern ProSLIC_FSK_Cfg Si3218x_FSK_Presets[];
#endif
#ifndef DISABLE_TONE_SETUP
extern ProSLIC_Tone_Cfg Si3218x_Tone_Presets[];
#endif
#endif /* 18X */ 

#ifdef SI3219X
#include "si3219x.h"
#include "si3219x_intf.h"
extern Si3219x_General_Cfg Si3219x_General_Configuration;
#ifndef DISABLE_FSK_SETUP
extern ProSLIC_FSK_Cfg Si3219x_FSK_Presets[];
#endif
#ifndef DISABLE_TONE_SETUP
extern ProSLIC_Tone_Cfg Si3219x_Tone_Presets[];
#endif
#endif /* 19X */

#ifdef SI3226X
#include "si3226x.h"
#include "si3226x_intf.h"
extern Si3226x_General_Cfg Si3226x_General_Configuration;
#ifndef DISABLE_FSK_SETUP
extern ProSLIC_FSK_Cfg Si3226x_FSK_Presets[];
#endif
#ifndef DISABLE_TONE_SETUP
extern ProSLIC_Tone_Cfg Si3226x_Tone_Presets[];
#endif
#endif /* 26X */

#ifdef SI3228X
#include "si3228x.h"
#include "si3228x_intf.h"
extern Si3228x_General_Cfg Si3228x_General_Configuration;
#ifndef DISABLE_FSK_SETUP
extern ProSLIC_FSK_Cfg Si3228x_FSK_Presets[];
#endif
#ifndef DISABLE_TONE_SETUP
extern ProSLIC_Tone_Cfg Si3228x_Tone_Presets[];
#endif

#endif /* 28X */

#define pCtrl(X)                                   (X)->deviceId->ctrlInterface
#define pProHW(X)                                  pCtrl((X))->hCtrl
#define WriteRAM(PCHAN, CHANNEL, RAMADDR, RAMDATA) (PCHAN)->deviceId->ctrlInterface->WriteRAM_fptr(pProHW(PCHAN), (CHANNEL), (RAMADDR), (RAMDATA))
#define ReadRAM(PCHAN, CHANNEL, RAMADDR)           (PCHAN)->deviceId->ctrlInterface->ReadRAM_fptr(pProHW(PCHAN), (CHANNEL), (RAMADDR))
#define SetSemaphore(X)                            (X)->deviceId->ctrlInterface->Semaphore_fptr
#define ReadReg(PCHAN, CHANNEL, REGADDR)           (PCHAN)->deviceId->ctrlInterface->ReadRegister_fptr(pProHW(PCHAN), (CHANNEL), (REGADDR))
#define WriteReg(PCHAN, CHANNEL, REGADDR, REGDATA) (PCHAN)->deviceId->ctrlInterface->WriteRegister_fptr(pProHW(PCHAN), (CHANNEL), (REGADDR), (REGDATA))

/*
** Timers
*/
#define TimeElapsed   pProslic->deviceId->ctrlInterface->timeElapsed_fptr
#define getTime       pProslic->deviceId->ctrlInterface->getTime_fptr
#define pProTimer	    pProslic->deviceId->ctrlInterface->hTimer
#define Delay         pProslic->deviceId->ctrlInterface->Delay_fptr
#define pProTimerX(X)	((X)->deviceId->ctrlInterface->hTimer)
#define DelayX(X,Y)   ((X)->deviceId->ctrlInterface->Delay_fptr(pProTimerX(X),Y))

#define PROSLIC_TIMEOUT_DCDC_DOWN 200 /* Number of 10 mSec ticks */

/*****************************************************************************************************/
int32 ProSLIC_ReadMADCScaled(proslicChanType_ptr hProslic,uInt16 addr,
                             int32 scale)
{
  TRACEPRINT(hProslic,"addr: %u scale: %ld\n", addr, scale);
#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_ReadMADCScaled(hProslic,addr,scale);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_ReadMADCScaled(hProslic,addr,scale);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_ReadMADCScaled(hProslic,addr,scale);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_ReadMADCScaled(hProslic,addr,scale);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_ReadMADCScaled(hProslic,addr,scale);
  }
#endif

  return -1;
}

/*****************************************************************************************************/
ramData ProSLIC_ReadRAM(proslicChanType_ptr hProslic,uInt16 addr)
{
  TRACEPRINT(hProslic, "addr: %u\n", addr);
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  return (ReadRAM(hProslic, hProslic->channel, addr));
}

/*****************************************************************************************************/
int ProSLIC_WriteRAM(proslicChanType_ptr hProslic,uInt16 addr, ramData data)
{
  TRACEPRINT(hProslic, "addr: %u data: 0x%04X\n", addr, data);
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  return (WriteRAM(hProslic, hProslic->channel, addr,data));
}

/*****************************************************************************************************/
int ProSLIC_PrintDebugData(proslicChanType_ptr hProslic)
{
  TRACEPRINT(hProslic, "\n", NULL);
#ifdef ENABLE_DEBUG
  ProSLIC_PrintDebugReg(hProslic);
  return ProSLIC_PrintDebugRAM(hProslic);
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  return RC_NONE;
#endif
}

/*****************************************************************************************************/
int ProSLIC_PrintDebugReg(proslicChanType_ptr hProslic)
{
#ifdef ENABLE_DEBUG
  uInt8 regAddr;
  TRACEPRINT(hProslic, "\n", NULL);

  /*
     NOTE: Not all ProSLICs have defined values after location 99
     (and have 1 location after that), but for simplicity, we print them anyway...
  */
  for(regAddr = 0; regAddr < 127; regAddr++)
  {
    LOGPRINT("%sRegister %03u = 0x%02X\n", LOGPRINT_PREFIX, regAddr,
             ReadReg(hProslic, hProslic->channel, regAddr));
  }
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
#endif

  return RC_NONE;
}

/*****************************************************************************************************/
/* NOTE: locations above 1024 are protected, the API disables protection during initialization, but if this
   function is called prior to initialization, then UAM is not set and will impact the ReadRAM call...
   Upper limit is based upon chipset type...
*/
int ProSLIC_PrintDebugRAM(proslicChanType_ptr hProslic)
{
#ifdef ENABLE_DEBUG
  uInt16 ramAddr;
  uInt16 maxAddr= 0;
  TRACEPRINT(hProslic, "\n",NULL);

  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    maxAddr = 1596;
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    maxAddr = 1644;
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    maxAddr = 1644;
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    maxAddr = 1646;
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    maxAddr = 1646;
  }
#endif

  for(ramAddr = 0; ramAddr < maxAddr; ramAddr++)
  {
    LOGPRINT("%sRAM %04u = 0x%08X\n", LOGPRINT_PREFIX, ramAddr,
             (unsigned int)(ReadRAM(hProslic, hProslic->channel, ramAddr)));
  }
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
#endif /* ENABLE_DEBUG */
  return RC_NONE;
}

/*****************************************************************************************************/
/*
** Function: isVerifiedProslic
**
** Description:
** Determine if DAA or ProSLIC present
**
** Input Parameters:
** pProslic: pointer to PROSLIC channel object
**
** Return:
** channelType
*/
int ProSLIC_identifyChannelType(proslicChanType *pProslic)
{
  uInt8 data;
  TRACEPRINT(pProslic, "\n",NULL);
  /*
  **  Register 13 (DAA) always has bits 0:1 set to 0 and bit 6 set to 1
  **  Register 13 (PROSLIC) can have bits 0:1, and 4 set, while all others are undefined
  **  Write 0x13 to Reg 13. The following return values are expected -
  **
  **   0x00 or 0xFF    :    No device present
  **   0x4X            :    DAA
  **   0x13            :    PROSLIC
  */

  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_PCMTXHI,0x13);
  Delay(pProTimer,5);

  /* Now check if the value took */
  data = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_PCMTXHI);

  if( data == 0x13)
  {
    return PROSLIC;
  }
  else if (data == 0x40)
  {
    return DAA;
  }
  else
  {
    return UNKNOWN;
  }
}

/*****************************************************************************************************/
int ProSLIC_VerifyControlInterface(proslicChanType_ptr hProslic)
{
  TRACEPRINT(hProslic, "\n",NULL);
  if (ProSLIC_identifyChannelType(hProslic) != PROSLIC)
  {
    return RC_CHANNEL_TYPE_ERR;
  }

  /* Note: ProSLIC_identifyChannelType() did a register w/r test earlier */

  /* Verify RAM rd/wr with innocuous RAM location */
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_VERIFY_IO,0x12345678L);
  if (ReadRAM(hProslic,hProslic->channel, PROSLIC_RAM_VERIFY_IO) != 0x12345678L)
  {
    hProslic->error = RC_SPI_FAIL;
    DEBUG_PRINT(hProslic, "%sProslic %d RAM not communicating. RAM access fail.\n",
                LOGPRINT_PREFIX, hProslic->channel);
    return RC_SPI_FAIL;
  }
  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_VerifyMasterStat(proslicChanType_ptr pProslic)
{
  uInt8 regData;

  TRACEPRINT(pProslic, "\n", NULL);
  WriteReg(pProslic,pProslic->channel, PROSLIC_REG_MSTRSTAT,
           0xFF); /* Clear Master status */
  regData = ReadReg(pProslic, pProslic->channel, PROSLIC_REG_MSTRSTAT);

  if( regData != 0x1F )
  {
    return RC_SPI_FAIL;
  }
  else
  {
    return RC_NONE;
  }
}

/*****************************************************************************************************/
#ifdef SIVOICE_MULTI_BOM_SUPPORT
int ProSLIC_Init_MultiBOM (proslicChanType_ptr *hProslic,int size, int preset)
{
  TRACEPRINT(*hProslic, "size: %d preset: %d\n", size, preset);
#ifdef SI3217X
  if ((*hProslic)->deviceId->chipType >= SI32171
      && (*hProslic)->deviceId->chipType <= SI32179)
  {
    return Si3217x_Init_MultiBOM(hProslic,size,preset);
  }
#endif

#ifdef SI3218X
  if ((*hProslic)->deviceId->chipType >= SI32180
      && (*hProslic)->deviceId->chipType <= SI32189)
  {
    return Si3218x_Init_MultiBOM(hProslic,size,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X((*hProslic)->deviceId) )
  {
    return Si3219x_Init_MultiBOM(hProslic,size,preset);
  }
#endif

#ifdef SI3226X
  if ((*hProslic)->deviceId->chipType >= SI32260
      && (*hProslic)->deviceId->chipType <= SI32269)
  {
    return Si3226x_Init_MultiBOM(hProslic,size,preset);
  }
#endif

#ifdef SI3228X
  if ((*hProslic)->deviceId->chipType >= SI32280
      && (*hProslic)->deviceId->chipType <= SI32289)
  {
    return Si3228x_Init_MultiBOM(hProslic,size,preset);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}
#endif

/*****************************************************************************************************/
int ProSLIC_Init (proslicChanType_ptr *hProslic,int size)
{
  TRACEPRINT(*hProslic, "size: %d\n", size);
  return ProSLIC_Init_with_Options(hProslic, size, INIT_NO_OPT);
}

/*****************************************************************************************************/
int ProSLIC_Reinit (proslicChanType_ptr *hProslic,int size)
{
  TRACEPRINT(*hProslic, "size: %d\n", size);
  return ProSLIC_Init_with_Options(hProslic, size, INIT_REINIT);
}

/*****************************************************************************************************/
int ProSLIC_Init_with_Options (proslicChanType_ptr *hProslic,int size,
                               int option)
{
  TRACEPRINT(*hProslic, "size: %d option: %d\n", size, option);
#ifdef SI3226X
  if ((*hProslic)->deviceId->chipType >= SI32260
      && (*hProslic)->deviceId->chipType <= SI32269)
  {
    return Si3226x_Init_with_Options(hProslic,size, option);
  }
#endif

#ifdef SI3228X
  if ((*hProslic)->deviceId->chipType >= SI32280
      && (*hProslic)->deviceId->chipType <= SI32289)
  {
    return Si3228x_Init_with_Options(hProslic,size, option);
  }
#endif

#ifdef SI3217X
  if ((*hProslic)->deviceId->chipType >= SI32171
      && (*hProslic)->deviceId->chipType <= SI32179)
  {
    return Si3217x_Init_with_Options(hProslic,size, option);
  }
#endif

#ifdef SI3218X
  if ((*hProslic)->deviceId->chipType >= SI32180
      && (*hProslic)->deviceId->chipType <= SI32189)
  {
    return Si3218x_Init_with_Options(hProslic,size, option);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X((*hProslic)->deviceId) )
  {
    return Si3219x_Init_with_Options(hProslic,size, option);
  }
#endif

  return RC_IGNORE;
}

/*****************************************************************************************************/
#if defined(SI3217X) || defined(SI3218X) || defined SI3226X || defined SI3228X || defined(SI3219X)
/* Check patch data - returns TRUE if no error.*/
static BOOLEAN ProSLIC_VerifyPatchData(proslicChanType *pProslic,
                                       const ramData *data, uInt16 maxCount )
{
  int loop;
  ramData read_data;
  TRACEPRINT(pProslic, "dataptr: %p, count: %d\n", data, maxCount);

  for(loop = 0; loop < maxCount; loop++)
  {
    if(*data)
    {
      read_data = ReadRAM(pProslic, pProslic->channel, PROSLIC_RAM_PRAM_DATA);
      if( ((*data) << 9) != read_data)
      {
        return FALSE;
      }
    }
    else
    {
      break;
    }
    data++;
  }
  return TRUE;
}

/* Check if the jump table is written correctly.. */
static BOOLEAN ProSLIC_VerifyPatchJMPLow(proslicChanType *pProslic,
    const uInt16 *data)
{
  uInt8 address = PATCH_JMPTBL_START_ADDR;
  int regData;

  TRACEPRINT(pProslic, "dataptr: %p\n", data);
  for(address = PATCH_JMPTBL_START_ADDR;
      address < (PATCH_JMPTBL_START_ADDR+(2*PATCH_NUM_LOW_ENTRIES)); address++)
  {
    if(*data)
    {
      regData = ReadReg(pProslic, pProslic->channel, address);
      if(regData != ((*data) & 0xFF))
      {
        return FALSE;
      }

      address++;

      regData = ReadReg(pProslic, pProslic->channel, address);
      if(regData != (((*data)>>8) & 0xFF))
      {
        return FALSE;
      }

      data++;
    }
    else
    {
      break;
    }
  }
  return TRUE;
}

#if defined SI3226X || defined SI3228X || defined SI3218X || defined SI3219X
/* For chipsets supporting more than 8 jump entries, verify them */
static BOOLEAN ProSLIC_VerifyPatchJMPHigh(proslicChanType *pProslic,
    const uInt16 *data)
{
  uInt16 address = PATCH_JMPTBL_HIGH_ADDR;
  ramData read_data;

  TRACEPRINT(pProslic, "dataptr: %p\n", data);
  for(address = PATCH_JMPTBL_HIGH_ADDR;
      address < (PATCH_JMPTBL_HIGH_ADDR+PATCH_NUM_HIGH_ENTRIES); address++)
  {
    read_data = (ReadRAM(pProslic, pProslic->channel, address) & 0x1FFFL);
    if(*data != read_data)
    {
      return FALSE;

    }
    data++;
  }
  return TRUE;
}
#endif /* SI3226X, SI3228X, SI3218X, SI3219X */

static BOOLEAN ProSLIC_VerifyPatchSupportRAM(proslicChanType *pProslic, 
                                           const uInt16 *ramAddr, const ramData *ramData)
{
  int i;

  for(i = 0; ramAddr[i]; i++)
  {
    if( ReadRAM(pProslic, pProslic->channel, ramAddr[i]) != ramData[i])
    {
      return FALSE;
    }
  } 

  return TRUE;
}

/* Load the first 8 jump table entries */
static void ProSLIC_LoadPatchJMPLow(proslicChanType *pProslic, uInt8 channel,
                                    const uInt16 *data)
{
  uInt8 address;

  TRACEPRINT(pProslic, "chan: %d dataptr: %p\n", channel, data);

  for(address = PATCH_JMPTBL_START_ADDR;
      address < (PATCH_JMPTBL_START_ADDR+(2*PATCH_NUM_LOW_ENTRIES)); address++)
  {
    WriteReg(pProslic, channel, address,((*data) & 0xFF));
    address++;
    WriteReg(pProslic, channel, address,(((*data)>>8) & 0xFF));
    data++;
  }
}

/* Load Patch data */
static void ProSLIC_LoadPatchData(proslicChanType *pProslic, uInt8 channel,
                                  const ramData *data)
{
  int loop;

  TRACEPRINT(pProslic, "chan: %d dataptr: %p\n", channel, data);
  WriteRAM(pProslic, channel, PROSLIC_RAM_PRAM_ADDR, 0);

  for(loop = 0; loop < PATCH_MAX_SIZE; loop++)
  {
    if(*data)
    {
      /* Can we take advantage of auto increment, if not, set the address */
      if( (pProslic->deviceId->chipRev < 3)
          && (channel == PROSLIC_CHAN_BROADCAST))
      {
        WriteRAM(pProslic, channel, PROSLIC_RAM_PRAM_ADDR, loop << 19);
      }

      WriteRAM(pProslic, channel, PROSLIC_RAM_PRAM_DATA, (*data) << 9);
    }
    else
    {
      return;
    }
    data++;
  }
}

/* Load Support RAM - basically RAM address/data pairs to be written as part of the Patch process - do not call directly */
void ProSLIC_LoadSupportRAM(proslicChanType *pProslic, uInt8 channel,
                                   const uInt16 *address, const ramData *data)
{
  TRACEPRINT(pProslic, "chan: %d addressptr: %p dataptr: %p\n", channel, address,
             data);
  while( *address )
  {
    WriteRAM(pProslic, channel, *address, *data);
    address++;
    data++;
  }
}

#if defined SI3226X || defined SI3228X || defined SI3218X || defined SI3219X
/* Load Jump table high for chipsets supporting more than 8 jump entries */
static void ProSLIC_LoadJMPHigh(proslicChanType *pProslic, uInt8 channel,
                                const uInt16 *data)
{
  uInt16 loop;
  TRACEPRINT(pProslic, "chan: %d dataptr: %p\n", channel, data);
  for(loop = PATCH_JMPTBL_HIGH_ADDR;
      loop < (PATCH_JMPTBL_HIGH_ADDR+PATCH_NUM_HIGH_ENTRIES); loop++)
  {
    WriteRAM(pProslic, channel, loop, ((*data) & 0x1FFFL) );
    data++;
  }
}
#endif /* SI3226X, SI3228X, SI3218X, SI3219X */

/* Code assumes ProSLIC_LoadPatch has verified chip type. This is NOT meant to be called
 * by the user directly.
 */
BOOLEAN ProSLIC_LoadPatch_extended(proslicChanType *pProslic,
                                   const proslicPatch *pPatch, 
                                   BOOLEAN is_broadcast, BOOLEAN is_second_chan)
{
  uInt8 channel;
  const uInt16 jmp_disable[PATCH_NUM_LOW_ENTRIES] = {0,0,0,0,0,0,0,0};

#if defined SI3226X || defined SI3228X || defined SI3218X ||defined SI3219X
  BOOLEAN hasJmpTableHigh = FALSE;
  const uInt16 jmphigh_disable[PATCH_NUM_HIGH_ENTRIES] = {0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL};
#endif

  TRACEPRINT(pProslic, "patchptr: %p bcast: %d\n", pPatch, is_broadcast);

  if(pPatch == NULL)
  {
    return RC_NONE;
  }

  if(is_broadcast == TRUE)
  {
    channel = PROSLIC_CHAN_BROADCAST;
  }
  else
  {
    channel = pProslic->channel;
  }

  ProSLIC_SetUserMode(pProslic,TRUE, is_broadcast);

  /* Disable Patch */
  WriteReg(pProslic, channel, PROSLIC_REG_JMPEN, 0);
  if(is_second_chan == FALSE)
  {
    DEBUG_PRINT(pProslic, "%sloading patch: %08lx\n", LOGPRINT_PREFIX,
                 (long unsigned int)pPatch->patchSerial);

    ProSLIC_LoadPatchJMPLow(pProslic, channel, jmp_disable);

#if defined SI3226X || defined SI3228X || defined SI3218X || defined SI3219X
    if( 0
#ifdef SI3226X
        || (pProslic->deviceId->chipType >= SI32260
            && pProslic->deviceId->chipType <= SI32269)
#endif
#ifdef SI3228X
        || (pProslic->deviceId->chipType >= SI32280
            && pProslic->deviceId->chipType <= SI32289)
#endif
#ifdef SI3218X
        || (pProslic->deviceId->chipType >= SI32180
            && pProslic->deviceId->chipType <= SI32189)
#endif
#ifdef SI3219X
        || ( IS_SI3219X(pProslic->deviceId) )
#endif
      )
    {
      hasJmpTableHigh = TRUE;
      ProSLIC_LoadJMPHigh(pProslic, channel, jmphigh_disable);
    }
#endif

    ProSLIC_LoadPatchData(pProslic, channel, pPatch->patchData);

    WriteReg(pProslic, channel, PROSLIC_REG_RAM_ADDR_HI, 0);

    ProSLIC_LoadPatchJMPLow(pProslic, channel, pPatch->patchEntries);

#if defined SI3226X || defined SI3228X || defined SI3218X || defined SI3219X
    if(hasJmpTableHigh == TRUE)
    {
      ProSLIC_LoadJMPHigh(pProslic, channel,
                          &(pPatch->patchEntries[PATCH_NUM_LOW_ENTRIES]));
    }
#endif
    WriteRAM(pProslic, channel, PROSLIC_RAM_PATCHID,
             pPatch->patchSerial); /* write the patch ID */
  } /* !second channel */

  ProSLIC_LoadSupportRAM(pProslic, channel, pPatch->psRamAddr, pPatch->psRamData);

  return RC_NONE;

}

#endif /* patch helper functions */

int ProSLIC_LoadPatch (proslicChanType_ptr pProslic,const proslicPatch *pPatch)
{
  int rc;

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  if( (rc = ProSLIC_LoadPatch_extended(pProslic, pPatch, FALSE, FALSE) ) == RC_NONE)
  {
#ifdef DISABLE_VERIFY_PATCH
    return WriteReg(pProslic, pProslic->channel, PROSLIC_REG_JMPEN, 1);
#endif
  }

  return rc;
}

/*****************************************************************************************************/
int ProSLIC_VerifyPatch (proslicChanType_ptr hProslic,const proslicPatch *pPatch)
{
  TRACEPRINT(hProslic, "patchptr: %p\n", pPatch);
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  if(pPatch == NULL)
  {
    return RC_NONE;
  }

  WriteReg(hProslic, hProslic->channel, PROSLIC_REG_JMPEN, 0);
  WriteRAM(hProslic, hProslic->channel, PROSLIC_RAM_PRAM_ADDR, 0);

  if(ProSLIC_VerifyPatchData(hProslic, pPatch->patchData,
                             PATCH_MAX_SIZE) == FALSE)
  {
    DEBUG_PRINT(hProslic, "%sPatch data corrupted: channel %d\n",LOGPRINT_PREFIX,
                hProslic->channel);
    WriteRAM(hProslic, hProslic->channel, PROSLIC_RAM_PATCHID,
             0UL); /* Mark patch as invalid */
    return RC_PATCH_RAM_VERIFY_FAIL;
  }

  /*zero out RAM_ADDR_HI*/
  WriteReg (hProslic, hProslic->channel, PROSLIC_REG_RAM_ADDR_HI,0);

  if( ProSLIC_VerifyPatchJMPLow(hProslic, pPatch->patchEntries) == FALSE)
  {
    DEBUG_PRINT(hProslic,"%sPatch jumptable corrupted: channel %d\n",
                LOGPRINT_PREFIX,hProslic->channel);
    WriteRAM(hProslic, hProslic->channel, PROSLIC_RAM_PATCHID,
             0UL); /* Mark patch as invalid */
    return RC_PATCH_RAM_VERIFY_FAIL;
  }

#if defined SI3226X || defined SI3228X || defined SI3218X || defined SI3219X
  if( 0
#ifdef SI3226X
      || (hProslic->deviceId->chipType >= SI32260
          && hProslic->deviceId->chipType <= SI32269)
#endif
#ifdef SI3228X
      || (hProslic->deviceId->chipType >= SI32280
          && hProslic->deviceId->chipType <= SI32289)
#endif
#ifdef SI3218X
      || (hProslic->deviceId->chipType >= SI32180
          && hProslic->deviceId->chipType <= SI32189)
#endif
#ifdef SI3219X
      || ( IS_SI3219X(hProslic->deviceId) )
#endif
    )

  {
    if( ProSLIC_VerifyPatchJMPHigh(hProslic,
                                   &(pPatch->patchEntries[PATCH_NUM_LOW_ENTRIES])) == FALSE)
    {
      DEBUG_PRINT(hProslic,"%sPatch jumptable high corrupted: channel %d\n",
                  LOGPRINT_PREFIX,hProslic->channel);
      WriteRAM(hProslic, hProslic->channel, PROSLIC_RAM_PATCHID,
               0UL); /* Mark patch as invalid */
      return RC_PATCH_ENTRY_VERIFY_FAIL;
    }
  }

#endif

  if( ProSLIC_VerifyPatchSupportRAM( hProslic, pPatch->psRamAddr, pPatch->psRamData) == FALSE)
  {
      DEBUG_PRINT(hProslic,"%sPatch init data corrupted: channel %d\n",
                  LOGPRINT_PREFIX,hProslic->channel);
      WriteRAM(hProslic, hProslic->channel, PROSLIC_RAM_PATCHID,
               0UL); /* Mark patch as invalid */
      return RC_PATCH_ENTRY_VERIFY_FAIL;
  }

  WriteReg (hProslic, hProslic->channel, PROSLIC_REG_JMPEN,
            1); /*enable the patch*/

  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_SetMuteStatus (proslicChanType_ptr pProslic,
                           ProslicMuteModes muteEn)
{

  uInt8 regTemp;
  uInt8 newRegValue;
  TRACEPRINT(pProslic, "muteEn: %d\n", muteEn);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  regTemp = ReadReg (pProslic,pProslic->channel,PROSLIC_REG_DIGCON);

  WriteReg (pProslic,pProslic->channel,PROSLIC_REG_DIGCON,regTemp&~(0x3));
  newRegValue = regTemp &~(0x3);

  if (muteEn & PROSLIC_MUTE_RX)
  {
    newRegValue |= 1;
  }

  if (muteEn & PROSLIC_MUTE_TX)
  {
    newRegValue |= 2;
  }

  if(newRegValue != regTemp)
  {
    return WriteReg (pProslic,pProslic->channel,PROSLIC_REG_DIGCON,newRegValue);
  }

  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_SetLoopbackMode (proslicChanType_ptr pProslic,
                             ProslicLoopbackModes newMode)
{
  uInt8 regTemp, newValue;
  TRACEPRINT(pProslic, "mode: %d\n", newMode);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  newValue = regTemp = ReadReg (pProslic,pProslic->channel,PROSLIC_REG_LOOPBACK);

  switch (newMode)
  {
    case PROSLIC_LOOPBACK_NONE:
      newValue &= ~(0x11);
      break;

    case PROSLIC_LOOPBACK_DIG:
      newValue |= 1;
      break;

    case PROSLIC_LOOPBACK_ANA:
      newValue |= 0x10;
      break;
  }

  if(newValue != regTemp)
  {
    return WriteReg (pProslic,pProslic->channel,PROSLIC_REG_LOOPBACK, newValue);
  }
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_EnableInterrupts (proslicChanType_ptr hProslic)
{
  TRACEPRINT(hProslic, "\n", NULL);
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_EnableInterrupts(hProslic);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_EnableInterrupts(hProslic);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_EnableInterrupts(hProslic);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_EnableInterrupts(hProslic);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_EnableInterrupts(hProslic);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}


/*****************************************************************************************************/
int ProSLIC_DisableInterrupts (proslicChanType_ptr hProslic)
{
#ifdef GCI_MODE
  uInt8 data;
#endif
  uInt8 i;
  TRACEPRINT(hProslic, "\n", NULL);

  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  for(i = PROSLIC_REG_IRQEN1; i < PROSLIC_REG_IRQEN4; i++)
  {
    /* Disable the interrupts */
    WriteReg(hProslic, hProslic->channel, i, 0);
  }

  /* Clear the pending interrupts */
  for(i = PROSLIC_REG_IRQ1; i < PROSLIC_REG_IRQ4; i++)
  {
#ifdef GCI_MODE
    data = ReadReg(hProslic, hProslic->channel, i);
    WriteReg( hProslic, hProslic->channel, i, data);
#else
    (void)ReadReg(hProslic, hProslic->channel, i);
#endif
  }
  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_RingSetup (proslicChanType_ptr hProslic,int preset)
{
  TRACEPRINT(hProslic, "preset: %d\n", preset);
#ifndef DISABLE_RING_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_RingSetup(hProslic,preset);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_RingSetup(hProslic,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_RingSetup(hProslic,preset);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_RingSetup(hProslic,preset);
  }
#endif
#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_RingSetup(hProslic,preset);
  }
#endif
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif /*DISABLE_RING_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_ToneGenSetup (proslicChanType_ptr hProslic,int preset)
{
  TRACEPRINT(hProslic, "preset: %d\n", preset);
#ifndef DISABLE_TONE_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return ProSLIC_ToneGenSetupPtr(hProslic,&(Si3217x_Tone_Presets[preset]));
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return ProSLIC_ToneGenSetupPtr(hProslic, &(Si3218x_Tone_Presets[preset]));
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return ProSLIC_ToneGenSetupPtr(hProslic, &(Si3219x_Tone_Presets[preset]));
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return ProSLIC_ToneGenSetupPtr(hProslic, &(Si3226x_Tone_Presets[preset]));
  }
#endif
#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return ProSLIC_ToneGenSetupPtr(hProslic, &(Si3228x_Tone_Presets[preset]));
  }
#endif
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif /*DISABLE_TONE_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_FSKSetup (proslicChanType_ptr hProslic,int preset)
{
  TRACEPRINT(hProslic, "preset: %d\n", preset);
#ifndef DISABLE_FSK_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return ProSLIC_FSKSetupPtr(hProslic, &Si3217x_FSK_Presets[preset]);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return ProSLIC_FSKSetupPtr(hProslic, &Si3218x_FSK_Presets[preset]);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return ProSLIC_FSKSetupPtr(hProslic, &Si3219x_FSK_Presets[preset]);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return ProSLIC_FSKSetupPtr(hProslic, &Si3226x_FSK_Presets[preset]);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return ProSLIC_FSKSetupPtr(hProslic,&Si3228x_FSK_Presets[preset]);
  }
#endif
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif /*DISABLE_FSK_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_ZsynthSetup (proslicChanType_ptr hProslic,int preset)
{
  TRACEPRINT(hProslic, "preset: %d\n", preset);
#ifndef DISABLE_ZSYNTH_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_ZsynthSetup(hProslic,preset);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_ZsynthSetup(hProslic,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_ZsynthSetup(hProslic,preset);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_ZsynthSetup(hProslic,preset);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_ZsynthSetup(hProslic,preset);
  }
#endif
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif /*DISABLE_ZSYNTH_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
#ifndef DISABLE_CI_SETUP
int ProSLIC_GciCISetup (proslicChanType_ptr hProslic,int preset)
{
  TRACEPRINT(hProslic, "preset: %d\n", preset);
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_GciCISetup(hProslic,preset);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_GciCISetup(hProslic,preset);
  }
#endif

#if !defined(SI3217X) && !defined(SI3226X)
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif

  return RC_COMPLETE_NO_ERR;
}
#endif /*DISABLE_CI_SETUP*/

/*****************************************************************************************************/
int ProSLIC_TXAudioGainSetup (proslicChanType_ptr hProslic,int preset)
{
  TRACEPRINT(hProslic, "preset: %d\n", preset);
#ifndef DISABLE_AUDIOGAIN_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_TXAudioGainSetup(hProslic,preset);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_TXAudioGainSetup(hProslic,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_TXAudioGainSetup(hProslic,preset);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_TXAudioGainSetup(hProslic,preset);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_TXAudioGainSetup(hProslic,preset);
  }
#endif
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);

#endif /*DISABLE_AUDIOGAIN_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_RXAudioGainSetup (proslicChanType_ptr hProslic,int preset)
{
  TRACEPRINT(hProslic, "preset: %d\n", preset);
#ifndef DISABLE_AUDIOGAIN_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_RXAudioGainSetup(hProslic,preset);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_RXAudioGainSetup(hProslic,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_RXAudioGainSetup(hProslic,preset);
  }
#endif


#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_RXAudioGainSetup(hProslic,preset);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_RXAudioGainSetup(hProslic,preset);
  }
#endif

#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif /*DISABLE_AUDIOGAIN_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_TXAudioGainScale (proslicChanType_ptr hProslic,int preset,
                              uInt32 pga_scale, uInt32 eq_scale)
{
  TRACEPRINT(hProslic, "preset: %d pga_scale: %u eq_scale: %u\n", preset,
             pga_scale, eq_scale);
#ifndef DISABLE_AUDIOGAIN_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_TXAudioGainScale(hProslic,preset,pga_scale,eq_scale);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_TXAudioGainScale(hProslic,preset,pga_scale,eq_scale);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_TXAudioGainScale(hProslic,preset,pga_scale,eq_scale);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_TXAudioGainScale(hProslic,preset,pga_scale,eq_scale);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_TXAudioGainScale(hProslic,preset,pga_scale,eq_scale);
  }
#endif

#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif /*DISABLE_AUDIOGAIN_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_RXAudioGainScale (proslicChanType_ptr hProslic,int preset,
                              uInt32 pga_scale, uInt32 eq_scale)
{
  TRACEPRINT(hProslic, "preset: %d pga_scale: %u eq_scale: %u\n", preset,
             pga_scale, eq_scale);
#ifndef DISABLE_AUDIOGAIN_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_RXAudioGainScale(hProslic,preset,pga_scale,eq_scale);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_RXAudioGainScale(hProslic,preset,pga_scale,eq_scale);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_RXAudioGainScale(hProslic,preset,pga_scale,eq_scale);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_RXAudioGainScale(hProslic,preset,pga_scale,eq_scale);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_RXAudioGainScale(hProslic,preset,pga_scale,eq_scale);
  }
#endif

#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif /*DISABLE_AUDIOGAIN_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_AudioGainSetup (proslicChanType_ptr pProslic,int32 rxgain,
                            int32 txgain,int preset)
{
  int rc;
#ifndef DISABLE_AUDIOGAIN_SETUP
  int atx_preset = TXACGAIN_SEL;
  int arx_preset = RXACGAIN_SEL;
  TRACEPRINT(pProslic, "rxgain: %d txgain: %d preset: %d\n", rxgain, txgain,
             preset);

  rc = ProSLIC_dbgSetTXGain(pProslic,txgain,preset,atx_preset);

  if( rc  == RC_NONE)
  {
    rc = ProSLIC_TXAudioGainSetup(pProslic,TXACGAIN_SEL);
  }

  if( rc  == RC_NONE)
  {
    rc = ProSLIC_dbgSetRXGain(pProslic,rxgain,preset,arx_preset);
  }

  if( rc  == RC_NONE)
  {
    rc = ProSLIC_RXAudioGainSetup(pProslic,RXACGAIN_SEL);
  }
  return rc;
#else
  SILABS_UNREFERENCED_PARAMETER(pProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
  return RC_IGNORE;
#endif /*DISABLE_AUDIOGAIN_SETUP*/
}

/*****************************************************************************************************/
int ProSLIC_DCFeedSetup (proslicChanType_ptr hProslic,int preset)
{
  TRACEPRINT(hProslic, "preset: %d\n", preset);
#ifndef DISABLE_DCFEED_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_DCFeedSetup(hProslic,preset);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_DCFeedSetup(hProslic,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_DCFeedSetupCfg(hProslic, Si3219x_DCfeed_Presets, preset);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_DCFeedSetup(hProslic,preset);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_DCFeedSetup(hProslic,preset);
  }
#endif
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif /*DISABLE_DCFEED_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_DCFeedSetupCfg (proslicChanType_ptr hProslic,
                            ProSLIC_DCfeed_Cfg *cfg, int preset)
{
  TRACEPRINT(hProslic, "cfgPtr = %p preset = %d\n", cfg, preset);
#ifndef DISABLE_DCFEED_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_DCFeedSetupCfg(hProslic,cfg,preset);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_DCFeedSetupCfg(hProslic,cfg,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_DCFeedSetupCfg(hProslic,cfg,preset);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_DCFeedSetupCfg(hProslic,cfg,preset);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_DCFeedSetupCfg(hProslic,cfg,preset);
  }
#endif

#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
  SILABS_UNREFERENCED_PARAMETER(cfg);
#endif /*DISABLE_DCFEED_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_GPIOSetup (proslicChanType_ptr hProslic)
{
  TRACEPRINT(hProslic, "\n", NULL);
#ifndef DISABLE_GPIO_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_GPIOSetup(hProslic);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_GPIOSetup(hProslic);
  }
#endif

#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
#endif /*DISABLE_GPIO_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
#ifndef DISABLE_PULSEMETERING
int ProSLIC_PulseMeterSetup (proslicChanType_ptr hProslic,int preset)
{
  TRACEPRINT(hProslic, "preset = %d\n", preset);
#ifndef DISABLE_PULSE_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_PulseMeterSetup(hProslic,preset);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_PulseMeterSetup(hProslic,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_PulseMeterSetup(hProslic,preset);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_PulseMeterSetup(hProslic,preset);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_PulseMeterSetup(hProslic,preset);
  }
#endif

#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif /*DISABLE_PULSE_SETUP*/
  return RC_COMPLETE_NO_ERR;
}
#endif

/*****************************************************************************************************/
int ProSLIC_PCMSetup (proslicChanType_ptr hProslic,int preset)
{
  TRACEPRINT(hProslic, "preset = %d\n", preset);
#ifndef DISABLE_PCM_SETUP
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_PCMSetup(hProslic,preset);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_PCMSetup(hProslic,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_PCMSetup(hProslic,preset);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_PCMSetup(hProslic,preset);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_PCMSetup(hProslic,preset);
  }
#endif
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  SILABS_UNREFERENCED_PARAMETER(preset);
#endif /*DISABLE_PCM_SETUP*/
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_PCMTimeSlotSetup (proslicChanType_ptr pProslic, uInt16 rxcount,
                              uInt16 txcount)
{
  uInt8 data;
  TRACEPRINT(pProslic, "rx = %u tx = %u\n", rxcount, txcount);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  data = txcount & 0xff;
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_PCMTXLO,data);

  data = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_PCMTXHI);
  data &= 0x10;  /* keep TX_EDGE bit */
  data |= ((txcount >> 8)&0x03) ;
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_PCMTXHI,data);

  data = rxcount & 0xff;
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_PCMRXLO,data);

  data = (rxcount >> 8) & 0x3; /* PCMRXHI has only 2 bits for timeslot */
  /* add to the calculated timeslot values the non timeslot bits */
  data |= (ReadReg(pProslic,pProslic->channel,PROSLIC_REG_PCMRXHI) & 0xFC);
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_PCMRXHI,data);

  return RC_NONE;
}

/*****************************************************************************************************/
typedef ProslicInt proslicIntTypeMap[SI_MAX_INTERRUPTS][8];

static int ProSLIC_ReadInterruptsHelper(proslicChanType_ptr pProslic,
                                        uInt8 *regData, uInt8 numChannels)
{
  uInt8 i;
  uInt8 intsActive;
  uInt8 *currentData = regData;
  SILABS_UNREFERENCED_PARAMETER(numChannels);

  intsActive = ReadReg(pProslic, pProslic->channel, PROSLIC_REG_IRQ0);

#ifdef PROSLIC_OPTIMIZE_INTERRUPTS
  /* For dual channel devices, we need to shift the upper nibble for the second channel
     if the caller requested the second channel.  We determine this by channel ID being
     even or odd.  If this is NOT true - for example a Si3217x and then a Si3226x chipset
     configuration on the same daisychain, then this optimization logic will not work */
#if defined(SI3226X) || defined(SI3228X)
  if( (numChannels != 0) && ((pProslic->channel) & 0x1))
  {
    intsActive = intsActive >> 4;
  }
#endif /* Multichannel devices */
#endif

  /* If there are no interrupts, stop... return back to calling function */
  if((intsActive &0xF) == 0)
  {
    return RC_IGNORE;
  }

  for(i = PROSLIC_REG_IRQ1; i <= PROSLIC_REG_IRQ4; i++)
  {
#ifdef PROSLIC_OPTIMIZE_INTERRUPTS
    /* Read IRQn Register only if IRQ0 states there was an interrupt pending, otherwise
       skip it. This eliminates unneeded SPI transactions.
    */
    if( (intsActive & (1<<(i-PROSLIC_REG_IRQ1))) == 0)
    {
      *currentData++ = 0;
      continue;
    }
#endif
    *currentData = (uInt8)ReadReg(pProslic, pProslic->channel, i);
#ifdef GCI_MODDE
    WriteReg(pProslic, pProslic->channel, i, *current_data);
#endif
    currentData++;
  } /* for loop */

  return RC_COMPLETE_NO_ERR;
}

/*
 Reads IRQ0 to determine if an interrupt has occurred for the particular device,
 if so, reads the interrupt register(s) that fired the interrupt and then
 maps the interrupt(s) to the generic interrupt value to return to the caller.

 Code assumes normal chipset/compatibility testing has already been done.
*/
static int ProSLIC_GetInterruptHelper(proslicChanType_ptr pProslic,
                                      const proslicIntTypeMap intMap, proslicIntType *pIntData, uInt8 numChannels)
{
  int i,j;
  uInt8 intData[SI_MAX_INTERRUPTS];
  uInt8 *currentData;
  uInt8 map, intCount;
  BOOLEAN safetyInt = FALSE;

  /* Initialize interrupt count to 0 */
  pIntData-> number = 0;

  if( ProSLIC_ReadInterruptsHelper(pProslic, intData, numChannels) == RC_IGNORE)
  {
    /* No interrupts for the given channel. */
    return RC_NONE;
  }

  /* At this point we've collected all the registers, now decode the data */
  currentData = intData;
  intCount = 0;

  for(i = 0; i < SI_MAX_INTERRUPTS; i++)
  {
    if(*currentData)
    {
      for(j = 0; j < 8; j++)
      {
        if( *currentData & (1<<j) )
        {
          map = intMap[i][j];
          pIntData->irqs[intCount] = map;

          if( (map == IRQ_P_HVIC)
              || (map == IRQ_P_THERM) )
          {
            safetyInt = TRUE;
          }

          intCount++;
        }
      }
    }
    currentData++;
  }
  
  pIntData->number = intCount;
  
  if( safetyInt == TRUE)
  {
    if(ProSLIC_isReinitRequired(pProslic))
    {
      return RC_REINIT_REQUIRED;
    }
  }

  return pIntData->number;
}


int ProSLIC_GetInterrupts (proslicChanType_ptr hProslic,
                           proslicIntType *pIntData)
{
  const proslicIntTypeMap interruptMap =
  {
    {IRQ_OSC1_T1, IRQ_OSC1_T2, IRQ_OSC2_T1, IRQ_OSC2_T2, IRQ_RING_T1, IRQ_RING_T2, IRQ_FSKBUF_AVAIL, IRQ_VBAT},
    {IRQ_RING_TRIP, IRQ_LOOP_STATUS, IRQ_LONG_STAT, IRQ_VOC_TRACK, IRQ_DTMF, IRQ_INDIRECT, IRQ_RXMDM, IRQ_TXMDM},
    {IRQ_P_HVIC, IRQ_P_THERM, IRQ_PQ3, IRQ_PQ4, IRQ_PQ5, IRQ_PQ6, IRQ_DSP, IRQ_MADC_FS},
    {IRQ_USER_0, IRQ_USER_1, IRQ_USER_2, IRQ_USER_3, IRQ_USER_4, IRQ_USER_5, IRQ_USER_6, IRQ_USER_7}
  };

  pIntData->number=0;
  /* TRACEPRINT(hProslic, "\n", NULL); */
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#if defined (SI3217X) || defined (SI3218X) || defined(SI3219X)
  if (0
#ifdef SI3217X
      || (hProslic->deviceId->chipType >= SI32171
          && hProslic->deviceId->chipType <= SI32179)
#endif
#ifdef SI3218X
      || (hProslic->deviceId->chipType >= SI32180
          && hProslic->deviceId->chipType <= SI32189)
#endif
#ifdef SI3219X
      || ( IS_SI3219X(hProslic->deviceId) )
#endif
     )
  {
    return ProSLIC_GetInterruptHelper(hProslic, interruptMap, pIntData, 1);
  }
#endif
#if defined (SI3226X) || defined (SI3228X)
  if( 0
#ifdef SI3226X
      || (hProslic->deviceId->chipType >= SI32260
          && hProslic->deviceId->chipType <= SI32269)
#endif
#ifdef SI3228X
      || (hProslic->deviceId->chipType >= SI32280
          && hProslic->deviceId->chipType <= SI32289)
#endif
    )
  {
    return ProSLIC_GetInterruptHelper(hProslic, interruptMap, pIntData, 2);
  }
#endif
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_ReadHookStatus (proslicChanType_ptr pProslic,uInt8 *pHookStat)
{
  TRACEPRINT(pProslic, "\n", NULL);
  *pHookStat = PROSLIC_ONHOOK;

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  if (ReadReg(pProslic,pProslic->channel,PROSLIC_REG_LCRRTP) & 2)
  {
    *pHookStat=PROSLIC_OFFHOOK;
  }
  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_SetLinefeedStatus (proslicChanType_ptr pProslic, uInt8 newLinefeed)
{
  uInt8 lfState;
  uInt8 irqen1=0;
  TRACEPRINT(pProslic, "linefeed = %u\n", newLinefeed);
  
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  /* Get the irqen1 setting - used to determine if vbat interrupt was set... */
#ifdef SI3217X
  if (pProslic->deviceId->chipType >= SI32171
    && pProslic->deviceId->chipType <= SI32179)
  {
      irqen1 = Si3217x_General_Configuration.irqen1;
  }
#endif

#ifdef SI3218X
  if (pProslic->deviceId->chipType >= SI32180
    && pProslic->deviceId->chipType <= SI32189)
  {
      irqen1 = Si3218x_General_Configuration.irqen1;
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(pProslic->deviceId) )
  {
      irqen1 = Si3219x_General_Configuration.irqen1;
  }
#endif

#ifdef SI3226X
  if (pProslic->deviceId->chipType >= SI32260
    && pProslic->deviceId->chipType <= SI32269)
  {
      irqen1 = Si3226x_General_Configuration.irqen1;
  }
#endif

#ifdef SI3228X
  if (pProslic->deviceId->chipType >= SI32280
    && pProslic->deviceId->chipType <= SI32289)
  {
      irqen1 = Si3228x_General_Configuration.irqen1;
  }
#endif

  if( (newLinefeed& 0xF)  == LF_RINGING )
  {
    uInt8 regTemp;

    lfState = ReadReg(pProslic, pProslic->channel, PROSLIC_REG_LINEFEED);

    /* Only change to ringing state if not ringing... */
    if( (lfState & 0xF) != LF_RINGING )
    {
      if(irqen1 & 0x80)
      {
        /*disable vbat interrupt during ringing*/
        regTemp = ReadReg(pProslic, pProslic->channel, PROSLIC_REG_IRQEN1 );
        WriteReg (pProslic, pProslic->channel, PROSLIC_REG_IRQEN1, regTemp&(~0x80));
      }

      WriteReg(pProslic, pProslic->channel, PROSLIC_REG_LINEFEED, LF_RINGING);
    }
    else
    {
      return RC_IGNORE;
    }
  }
  else
  {
    uInt8 autord;

    /* Preserve the auto register so we can restore it at the end */
    autord = ReadReg(pProslic, pProslic->channel, PROSLIC_REG_AUTORD );
    WriteReg(pProslic, pProslic->channel, PROSLIC_REG_AUTORD, (autord & 0xFB) ); /* Disable AutoRD */

    lfState = ReadReg(pProslic, pProslic->channel, PROSLIC_REG_LINEFEED);

    if( (lfState & 0xF) == LF_RINGING ) 
    {
      WriteReg(pProslic, pProslic->channel, PROSLIC_REG_LINEFEED, newLinefeed);
    }
    else
    {
      /* We are already doing a state transition, abort request */
      if( ((lfState & 0xF0) == (LF_RINGING << 4) )
        && ( (lfState & 0xF) != LF_RINGING) )
      {
        WriteReg(pProslic, pProslic->channel, PROSLIC_REG_AUTORD, autord ); /* restore the autord bit */
        return RC_IGNORE;
      }

      WriteReg(pProslic, pProslic->channel, PROSLIC_REG_LINEFEED, newLinefeed);
    }
    /* Restore autord */
    WriteReg(pProslic, pProslic->channel, PROSLIC_REG_AUTORD, autord );

    /* Restore IRQEN1 to what the user specified - if we changed it.. */
    if(irqen1 & 0x80)
    {
      WriteReg (pProslic, pProslic->channel, PROSLIC_REG_IRQEN1, irqen1);
    }
  }
  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_SetLinefeedStatusBroadcast (proslicChanType_ptr hProslic,
                                        uInt8 newLinefeed)
{
  TRACEPRINT(hProslic, "linefeed = %u\n", newLinefeed);
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  WriteReg(hProslic, PROSLIC_CHAN_BROADCAST, PROSLIC_REG_LINEFEED, newLinefeed);
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_PolRev (proslicChanType_ptr pProslic,uInt8 abrupt,
                    uInt8 newPolRevState)
{
  uInt8 data = 0;
  TRACEPRINT(pProslic, "abrupt = %u newPolRevState = %u\n", abrupt,
             newPolRevState);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  switch (newPolRevState)
  {
    case POLREV_STOP:
      data = 0;
      break;
    case POLREV_START:
      data = 2;
      break;
    case WINK_START:
      data = 6;
      break;
    case WINK_STOP:
      data = 4;
      break;
  }

  /* Cannot polrev/wink while low power mode is active */
  ProSLIC_SetPowersaveMode(pProslic,PWRSAVE_DISABLE);

  if (abrupt)
  {
    data |= 1;
  }

  WriteReg(pProslic, pProslic->channel, PROSLIC_REG_POLREV,data);

  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_GPIOControl (proslicChanType_ptr pProslic,uInt8 *pGpioData,
                         uInt8 read)
{
  TRACEPRINT(pProslic, "\n", NULL);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#if defined(SI3217X) || defined (SI3226X)
  if( 0
#ifdef SI3217X
      || (pProslic->deviceId->chipType >= SI32171
          && pProslic->deviceId->chipType <= SI32179)
#endif
#ifdef SI3226X
      || (pProslic->deviceId->chipType >= SI32260
          && pProslic->deviceId->chipType <= SI32269)
#endif
    )
  {
    if (read)
    {
      *pGpioData = 0xf & ReadReg(pProslic,pProslic->channel,PROSLIC_REG_GPIO);
    }
    else
    {
      WriteReg(pProslic,pProslic->channel,PROSLIC_REG_GPIO,
               (*pGpioData)|(ReadReg(pProslic,pProslic->channel,PROSLIC_REG_GPIO)&0xf0));
    }
    return RC_NONE;
  }
#else
  SILABS_UNREFERENCED_PARAMETER(read);
  SILABS_UNREFERENCED_PARAMETER(pGpioData);
#endif 
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
/*
** Optional Neon Message Waiting Support
*/
#ifdef SIVOICE_NEON_MWI_SUPPORT
int ProSLIC_MWISetV (proslicChanType_ptr hProslic, uInt16 vpk_mag)
{
  uInt32 ram_val;
  uInt8 max;
  
  TRACEPRINT(hProslic, "vpk_mag = %u\n", vpk_mag);
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  
  /* Set the maximum MWI voltage according to the chipset */
#if defined (SI3217X) || defined (SI3226X)
  if( 0
#ifdef SI3217X
      || (hProslic->deviceId->chipType >= SI32171
          && hProslic->deviceId->chipType <= SI32179)
#endif
#ifdef SI3226X
      || (hProslic->deviceId->chipType >= SI32260
          && hProslic->deviceId->chipType <= SI32269)
#endif
    )
  {
    max = SIL_MWI_VPK_MAX;
  }
#endif /*17x/26x */

#if defined (SI3218X) || defined (SI3228X) || defined(SI3219X)
  if( 0
#ifdef SI3218X
    || (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
#endif

#ifdef SI3228X
    || (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
#endif

#ifdef SI3219X
    || IS_SI3219X(hProslic->deviceId) 
#endif

#ifdef SI3228X
      || (hProslic->deviceId->chipType >= SI32280
          && hProslic->deviceId->chipType <= SI32289)
#endif
    )
  {
    max = SIL_MWI_VPK_MAX_LO;
  }
#endif /* 18x, 28x, 19x */

  /* Voltage mod */
  if(vpk_mag > 0)  /* Skip if 0 passed */
  {
    /* Clamp supplied value to allowable range */
    if(vpk_mag > max)
    {
      vpk_mag = max;
    }
    if(vpk_mag < SIL_MWI_VPK_MIN)
    {
      vpk_mag = SIL_MWI_VPK_MIN;
    }
    ram_val = vpk_mag * SCALE_R_MADC * 1000L;
    WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_MWI_V,ram_val);
  }
  return RC_COMPLETE_NO_ERR;
}
#endif

/*****************************************************************************************************/
#ifdef SIVOICE_NEON_MWI_SUPPORT
int ProSLIC_MWIEnable (proslicChanType_ptr hProslic)
{
  uInt8 val;
  ramData ram_val;
  
  TRACEPRINT(hProslic, "\n", NULL);
  
  /*
  ** Check for conditions that would prevent enabling MWI
  */
  ProSLIC_ReadHookStatus(hProslic,&val);
  if(val != PROSLIC_ONHOOK)
  {
    return RC_MWI_ENABLE_FAIL;
  }
  
  val = ReadReg(hProslic,hProslic->channel,PROSLIC_REG_USERSTAT);

  /* Check if PROSLIC and disabled MWI, if not, ignore this request. */
  if( (hProslic->channelType != PROSLIC)
       || (val & SIL_MWI_USTAT_SET ) )
  {
    return RC_IGNORE;
  }
  
  /* Save parameters */
  hProslic->mwiSave.ringof = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_RINGOF);
  hProslic->mwiSave.ringamp = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_RINGAMP);
  hProslic->mwiSave.vbatr_expect = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_VBATR_EXPECT);
  hProslic->mwiSave.vov_ring_bat = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_VOV_RING_BAT);
  hProslic->mwiSave.vov_ring_gnd = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_VOV_RING_GND);
  hProslic->mwiSave.rtacth = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTACTH);
  hProslic->mwiSave.rtdcth = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTDCTH);
  hProslic->mwiSave.iring_lim = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_IRING_LIM);
  hProslic->mwiSave.dcdc_rngtype = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_DCDC_RNGTYPE);
  hProslic->mwiSave.slope_ring = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_SLOPE_RING);
  hProslic->mwiSave.rtper = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTPER);
  hProslic->mwiSave.ringfr = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_RINGFR);
  hProslic->mwiSave.rtdcdb = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTDCDB);
  hProslic->mwiSave.lcrmask = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_LCRMASK);
  hProslic->mwiSave.dcdc_oithresh_lo = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_DCDC_OITHRESH_LO);
  hProslic->mwiSave.enhance = ReadReg(hProslic,hProslic->channel,PROSLIC_REG_ENHANCE);
  hProslic->mwiSave.ringcon = ReadReg(hProslic,hProslic->channel,PROSLIC_REG_RINGCON);
  hProslic->mwiSave.userstat =  val;
  hProslic->mwiSave.linefeed = ReadReg(hProslic,hProslic->channel,PROSLIC_REG_LINEFEED);
  
  /* Modify parameters */
  ProSLIC_SetLinefeedStatus(hProslic,LF_FWD_ACTIVE);
  ram_val = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_MWI_V);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RINGOF,ram_val);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RINGAMP,0x0L);

  /* Set the VBATR_EXPECT according to the chipset */
#if defined (SI3217X) || defined (SI3226X)
  if( 0
#ifdef SI3217X
      || (hProslic->deviceId->chipType >= SI32171
          && hProslic->deviceId->chipType <= SI32179)
#endif
#ifdef SI3226X
      || (hProslic->deviceId->chipType >= SI32260
          && hProslic->deviceId->chipType <= SI32269)
#endif
    )
  {
    ram_val = 0x7FFFFC2L;
  }
#endif

#if defined (SI3218X) || defined (SI3228X) || defined(SI3219X)
  if( 0
#ifdef SI3218X
      || (hProslic->deviceId->chipType >= SI32180
          && hProslic->deviceId->chipType <= SI32189)
#endif

#ifdef SI3228X
      || (hProslic->deviceId->chipType >= SI32280
          && hProslic->deviceId->chipType <= SI32289)
#endif

#ifdef SI3219X
    || IS_SI3219X(hProslic->deviceId) 
#endif
    )
  {
    ram_val = 0x06866635L;
  }
#endif /* 18x,28x, 19x */

  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_VBATR_EXPECT,ram_val);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_VOV_RING_BAT,0x0L);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_VOV_RING_GND,0x051EB80L);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTACTH,0x0FFFFFFFL);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTDCTH,0x38E38EL);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_IRING_LIM,0x380000L);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_DCDC_RNGTYPE,0x100000L);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_SLOPE_RING,0x15E5200EL);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTPER,0x50000L);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RINGFR,0x07EFE000L);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTDCDB,0x0000A000L);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_LCRMASK,0x000F0000L);
  ram_val = ReadRAM(hProslic,hProslic->channel,PROSLIC_RAM_DCDC_OITHRESH_HI);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_DCDC_OITHRESH_LO,ram_val);
  WriteReg(hProslic,hProslic->channel,PROSLIC_REG_ENHANCE,hProslic->mwiSave.enhance&0xEF);
  WriteReg(hProslic,hProslic->channel,PROSLIC_REG_RINGCON,0x00);
  WriteReg(hProslic,hProslic->channel,PROSLIC_REG_USERSTAT,0x0C);
  
  return RC_COMPLETE_NO_ERR;
}
#endif

/*****************************************************************************************************/
#ifdef SIVOICE_NEON_MWI_SUPPORT
int ProSLIC_MWIDisable (proslicChanType_ptr hProslic)
{
  TRACEPRINT(hProslic, "\n", NULL);

  /* Check if PROSLIC and enabled MWI, if not, ignore this request. */
  if( (hProslic->channelType != PROSLIC)
       || !(ReadReg(hProslic,hProslic->channel,PROSLIC_REG_USERSTAT) & SIL_MWI_USTAT_SET ) )
  {
    return RC_IGNORE;
  }

  ProSLIC_SetLinefeedStatus(hProslic,LF_FWD_ACTIVE);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RINGOF,hProslic->mwiSave.ringof);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RINGAMP,hProslic->mwiSave.ringamp);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_VBATR_EXPECT,hProslic->mwiSave.vbatr_expect);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_VOV_RING_BAT,hProslic->mwiSave.vov_ring_bat);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_VOV_RING_GND,hProslic->mwiSave.vov_ring_gnd);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTACTH,hProslic->mwiSave.rtacth);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTDCTH,hProslic->mwiSave.rtdcth);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_IRING_LIM,hProslic->mwiSave.iring_lim);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_DCDC_RNGTYPE,hProslic->mwiSave.dcdc_rngtype);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_SLOPE_RING,hProslic->mwiSave.slope_ring);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTPER,hProslic->mwiSave.rtper);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RINGFR,hProslic->mwiSave.ringfr);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_RTDCDB,hProslic->mwiSave.rtdcdb);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_LCRMASK,hProslic->mwiSave.lcrmask);
  WriteRAM(hProslic,hProslic->channel,PROSLIC_RAM_DCDC_OITHRESH_LO,hProslic->mwiSave.dcdc_oithresh_lo);
  WriteReg(hProslic,hProslic->channel,PROSLIC_REG_ENHANCE,hProslic->mwiSave.enhance);
  WriteReg(hProslic,hProslic->channel,PROSLIC_REG_RINGCON,hProslic->mwiSave.ringcon);
  WriteReg(hProslic,hProslic->channel,PROSLIC_REG_USERSTAT,hProslic->mwiSave.userstat);
  ProSLIC_SetLinefeedStatus(hProslic,hProslic->mwiSave.linefeed);
  
  return RC_COMPLETE_NO_ERR;
}
#endif

/*****************************************************************************************************/
#ifdef SIVOICE_NEON_MWI_SUPPORT
int ProSLIC_SetMWIState (proslicChanType_ptr hProslic,uInt8 flash_on)
{
  TRACEPRINT(hProslic, "flash_on = %u\n", flash_on);
  /* Check if PROSLIC and enabled MWI, if not, ignore this request. */
  if( (hProslic->channelType != PROSLIC)
       || !(ReadReg(hProslic,hProslic->channel,PROSLIC_REG_USERSTAT) & SIL_MWI_USTAT_SET ) )
  {
    return RC_IGNORE;
  }
  
  if(flash_on)
  {
    ProSLIC_SetLinefeedStatus(hProslic,LF_RINGING);
  }
  else
  {
    ProSLIC_SetLinefeedStatus(hProslic,LF_FWD_ACTIVE);
  }
  
  return RC_COMPLETE_NO_ERR;
}
#endif

/*****************************************************************************************************/
#ifdef SIVOICE_NEON_MWI_SUPPORT
int ProSLIC_GetMWIState (proslicChanType_ptr hProslic)
{
  TRACEPRINT(hProslic, "\n", NULL);
  /* Check if PROSLIC and enabled MWI, if not, ignore this request. */
  if( (hProslic->channelType != PROSLIC)
       || !(ReadReg(hProslic,hProslic->channel,PROSLIC_REG_USERSTAT) & SIL_MWI_USTAT_SET ) )
  {
    return RC_IGNORE;
  }
  
  if(ReadReg(hProslic,hProslic->channel,PROSLIC_REG_LINEFEED) == 0x44)
  {
    return SIL_MWI_FLASH_ON;
  }
  else
  {
    return SIL_MWI_FLASH_OFF;
  }
  
  return RC_COMPLETE_NO_ERR;
}
#endif

/******************************************************************************/
#ifdef SIVOICE_NEON_MWI_SUPPORT
int ProSLIC_MWIRampPoll(proslicChanType_ptr pProslic)
{
  uInt32 mwi_voltage;
  
  /* Check if PROSLIC and enabled MWI, if not, ignore this request. */
  if( (pProslic->channelType != PROSLIC)
       || !(ReadReg(pProslic,pProslic->channel,PROSLIC_REG_USERSTAT) & SIL_MWI_USTAT_SET ) )
  {
    return RC_IGNORE;
  }
  
  switch(pProslic->mwiState.state)
  {
    case PROSLIC_MWI_RAMP_ON:
    {
      if(pProslic->mwiState.ticks == pProslic->mwiState.poll.steps)
      {
        mwi_voltage = pProslic->mwiState.poll.voff + pProslic->mwiState.poll.step_size;
        WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_RINGOF,mwi_voltage);
        ProSLIC_SetLinefeedStatus(pProslic,LF_RINGING);
      }
      else if(pProslic->mwiState.ticks == 0)
      {
        mwi_voltage = pProslic->mwiState.poll.von;
        WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_RINGOF,mwi_voltage);
        pProslic->mwiState.ticks = pProslic->mwiState.poll.onTime+1;
        pProslic->mwiState.state = PROSLIC_MWI_ON;
      }
      else
      {
        mwi_voltage = pProslic->mwiState.poll.voff + pProslic->mwiState.poll.step_size*(pProslic->mwiState.poll.steps - pProslic->mwiState.ticks + 1);
        WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_RINGOF,mwi_voltage);
      }
    }
    break;

    case PROSLIC_MWI_RAMP_OFF:
    {
      if(pProslic->mwiState.ticks == 0)
      {
        pProslic->mwiState.ticks = pProslic->mwiState.poll.offTime+1;
        pProslic->mwiState.state = PROSLIC_MWI_OFF;
        ProSLIC_SetLinefeedStatus(pProslic,LF_FWD_ACTIVE);
        }
      else 
      {
        mwi_voltage = pProslic->mwiState.poll.von - (pProslic->mwiState.poll.steps - pProslic->mwiState.ticks + 1)*pProslic->mwiState.poll.step_size;
        WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_RINGOF,mwi_voltage);
      }
    break;
    }
    
    case PROSLIC_MWI_ON:
      if(pProslic->mwiState.ticks == 0)
      {
        pProslic->mwiState.ticks = pProslic->mwiState.poll.steps+1;
        pProslic->mwiState.state = PROSLIC_MWI_RAMP_OFF;
      }
      break;
      
    case PROSLIC_MWI_OFF:
      if(pProslic->mwiState.ticks == 0)
      {
        pProslic->mwiState.ticks = pProslic->mwiState.poll.steps+1;
        pProslic->mwiState.state = PROSLIC_MWI_RAMP_ON;
      }
      break;

    default:
      /* Do nothing */
      break;
  }
  (pProslic->mwiState.ticks)--; 

  return RC_COMPLETE_NO_ERR;
}
#endif

/******************************************************************************/
#ifdef SIVOICE_NEON_MWI_SUPPORT
int ProSLIC_MWIRampStart(proslicChanType_ptr pProslic, uInt32 steps, uInt32 onTime, uInt32 offTime)
{
  /* Check if PROSLIC and enabled MWI, if not, ignore this request. */
  if( (pProslic->channelType != PROSLIC)
       || !(ReadReg(pProslic,pProslic->channel,PROSLIC_REG_USERSTAT) & SIL_MWI_USTAT_SET ) )
  {
    return RC_IGNORE;
  }
  
  pProslic->mwiState.poll.steps = steps-1;
  pProslic->mwiState.poll.onTime = onTime;
  pProslic->mwiState.poll.offTime = offTime;
  
  pProslic->mwiState.poll.von = ReadRAM(pProslic,pProslic->channel,PROSLIC_RAM_MWI_V);
  pProslic->mwiState.poll.voff = (ReadRAM(pProslic,pProslic->channel,PROSLIC_RAM_V_VLIM)*5)/6;
  pProslic->mwiState.poll.step_size = (pProslic->mwiState.poll.von - pProslic->mwiState.poll.voff)/steps;
  
  pProslic->mwiState.state = PROSLIC_MWI_RAMP_ON;
  pProslic->mwiState.ticks = pProslic->mwiState.poll.steps;

  return RC_COMPLETE_NO_ERR;
}
#endif

/******************************************************************************/
#ifdef SIVOICE_NEON_MWI_SUPPORT
int ProSLIC_MWIRampStop(proslicChanType_ptr pProslic)
{
  /* Check if PROSLIC and enabled MWI, if not, ignore this request. */
  if( (pProslic->channelType != PROSLIC)
       || !(ReadReg(pProslic,pProslic->channel,PROSLIC_REG_USERSTAT) & SIL_MWI_USTAT_SET ) )
  {
    return RC_IGNORE;
  }
  pProslic->mwiState.state = PROSLIC_MWI_OFF;
  
  return RC_COMPLETE_NO_ERR;
}
#endif

/*****************************************************************************************************/
int ProSLIC_ToneGenStart (proslicChanType_ptr pProslic,uInt8 timerEn)
{
  uInt8 data;
  TRACEPRINT(pProslic, "timerEn = %u\n", timerEn);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  DEBUG_PRINT(pProslic, "%s%s on channel %d\n",LOGPRINT_PREFIX, __FUNCTION__,
              pProslic->channel);

  data = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_OCON);
  data |= 0x11 + (timerEn ? 0x66 : 0);
  return WriteReg(pProslic,pProslic->channel,PROSLIC_REG_OCON,data);
}

/*****************************************************************************************************/
int ProSLIC_ToneGenStop (proslicChanType_ptr pProslic)
{
  uInt8 data;
  TRACEPRINT(pProslic, "\n", NULL);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  DEBUG_PRINT(pProslic,"%s%s on channel %d\n",LOGPRINT_PREFIX, __FUNCTION__,
              pProslic->channel);
  data = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_OCON);
  data &= ~(0x77);
  return WriteReg(pProslic,pProslic->channel,PROSLIC_REG_OCON,data);
}

/*****************************************************************************************************/
int ProSLIC_RingStart (proslicChanType_ptr pProslic)
{
  TRACEPRINT(pProslic, "\n", NULL);
  return(ProSLIC_SetLinefeedStatus(pProslic, LF_RINGING));
}

/*****************************************************************************************************/
int ProSLIC_RingStop (proslicChanType_ptr pProslic)
{
  TRACEPRINT(pProslic, "\n", NULL);
  return(ProSLIC_SetLinefeedStatus(pProslic, LF_FWD_ACTIVE));
}

#ifndef DISABLE_FSK_SETUP
/*****************************************************************************************************/
int ProSLIC_CheckCIDBuffer (proslicChanType_ptr pProslic, uInt8 *fsk_buf_avail)
{
  uInt8 data;
  TRACEPRINT(pProslic, "\n", NULL);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  data = ReadReg(pProslic,pProslic->channel, PROSLIC_REG_IRQ1);
#ifdef GCI_MODE
  WriteReg(pProslic,pProslic->channel, PROSLIC_REG_IRQ1,data); /*clear (for GCI)*/
#endif
  *fsk_buf_avail = (data&0x40) ? 1 : 0;
  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_EnableCID (proslicChanType_ptr pProslic)
{
  uInt8 data;
  TRACEPRINT(pProslic, "\n", NULL);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  DEBUG_PRINT(pProslic, "%sEnableCID on channel %d\n",LOGPRINT_PREFIX,
              pProslic->channel);
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_OCON,0);

  data = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_OMODE);
  data |= 0xA;
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_OMODE,data);

  return(WriteReg(pProslic,pProslic->channel,PROSLIC_REG_OCON,0x5));
}

/*****************************************************************************************************/
int ProSLIC_DisableCID (proslicChanType_ptr pProslic)
{
  uInt8 data;
  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  DEBUG_PRINT(pProslic, "%sDisableCID on channel %d\n",LOGPRINT_PREFIX,
              pProslic->channel);
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_OCON,0);
  data = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_OMODE);
  data &= ~(0x8);

  return(WriteReg(pProslic,pProslic->channel,PROSLIC_REG_OMODE,data));
}

/*****************************************************************************************************/
int ProSLIC_SendCID (proslicChanType_ptr pProslic, uInt8 *buffer,
                     uInt8 numBytes)
{
  TRACEPRINT(pProslic, "numBytes = %u\n", numBytes);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  DEBUG_PRINT (pProslic, "%sSendCID on channel %d\n",LOGPRINT_PREFIX,
               pProslic->channel);
  while (numBytes-- > 0)
  {
    WriteReg(pProslic,pProslic->channel,PROSLIC_REG_FSKDAT,*(buffer++));
  }
  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_ModifyCIDStartBits(proslicChanType_ptr pProslic,
                               uInt8 enable_startStop)
{
  uInt8 data;
  TRACEPRINT(pProslic, "enable_startStop = %u\n", enable_startStop);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_CHANNEL_TYPE_ERR;
  }

  data = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_OMODE);

  if(enable_startStop == FALSE)
  {
    data &= ~0x80;
  }
  else
  {
    data |= 0x80;
  }

  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_OMODE,data);

  return RC_NONE;
}
#endif /* DISABLE_FSK_SETUP */
/*****************************************************************************************************/
int ProSLIC_PCMStart (proslicChanType_ptr pProslic)
{
  uInt8 data;
  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  DEBUG_PRINT(pProslic, "%sPCMStart\n", LOGPRINT_PREFIX);
  data = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_PCMMODE);
  data |= 0x10;

  return WriteReg(pProslic,pProslic->channel,PROSLIC_REG_PCMMODE,data);
}

/*****************************************************************************************************/
int ProSLIC_PCMStop (proslicChanType_ptr pProslic)
{
  uInt8 data;
  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  DEBUG_PRINT(pProslic, "%sPCMStart\n", LOGPRINT_PREFIX);
  data = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_PCMMODE);
  data &= ~0x10;

  return WriteReg(pProslic,pProslic->channel,PROSLIC_REG_PCMMODE,data);
}

#ifndef DISABLE_HOOKCHANGE
/*****************************************************************************************************/
/*
** Function: ProSLIC_ResetDialPulseDetect
**
** Description:
** reset dial pulse detection state machine (helper function for
** ProSLIC_InitializeHookChangeDetect.
*/
static void ProSLIC_ResetDialPulseDetect(hookChangeType *pPulse)
{
  pPulse->currentPulseDigit = 0;
  pPulse->lookingForTimeout = 0;
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_InitializeHookChangeDetect
**
** Description:
** Initialize dial pulse detection parameters
*/
int ProSLIC_InitializeHookChangeDetect(hookChangeType *pPulse,void *hookTime)
{
  TRACEPRINT_NOCHAN("\n", NULL);
  pPulse->hookTime = hookTime;
  pPulse->last_state_reported =  SI_HC_NO_ACTIVITY;
  ProSLIC_ResetDialPulseDetect(pPulse);
  return RC_NONE;
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_HookChangeDetect
**
** Description:
** implements pulse dial detection and should be called at every hook transition
*/
uInt8 ProSLIC_HookChangeDetect (proslicChanType *pProslic,
                                hookChange_Cfg *pHookChangeCfg, hookChangeType *pHookChangeData)
{
  uInt8 hookStat;
  int delta_time;

  TRACEPRINT(pProslic, "\n", NULL);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  TimeElapsed(pProTimer,pHookChangeData->hookTime,&delta_time);
  ProSLIC_ReadHookStatus(pProslic,&hookStat);

  /* Did we have a hook state change? */
  if(hookStat !=  pHookChangeData->last_hook_state)
  {
    pHookChangeData->last_hook_state = hookStat;
    getTime(pProTimer,pHookChangeData->hookTime);
    pHookChangeData->lookingForTimeout = 1;
    if (hookStat == PROSLIC_OFFHOOK)
    {
      if ((delta_time >= (pHookChangeCfg->minOnHook))
          && (delta_time <= (pHookChangeCfg->maxOnHook)))
      {
        pHookChangeData->currentPulseDigit++;
      }
      else
      {
        /* Did we see a hook flash? */
        if( (delta_time >= pHookChangeCfg->minHookFlash)
            && (delta_time <= pHookChangeCfg->maxHookFlash) )
        {
          pHookChangeData->last_state_reported = SI_HC_HOOKFLASH;
          ProSLIC_ResetDialPulseDetect(pHookChangeData);
          return SI_HC_HOOKFLASH;
        }
      }
    }

    return SI_HC_NEED_MORE_POLLS;
  }

  if( (pHookChangeData->lookingForTimeout == 1)
      && (delta_time >=  pHookChangeCfg->minInterDigit) )
  {

    if(delta_time  > pHookChangeCfg->minHook)
    {
      if(pHookChangeData->last_hook_state == PROSLIC_ONHOOK)
      {
        ProSLIC_ResetDialPulseDetect(pHookChangeData);
        pHookChangeData->last_state_reported = SI_HC_ONHOOK_TIMEOUT;
        return SI_HC_ONHOOK_TIMEOUT;
      }

      if(pHookChangeData->last_hook_state == PROSLIC_OFFHOOK)
      {
        ProSLIC_ResetDialPulseDetect(pHookChangeData);

        /* Check if we saw either a pulse digit or hook flash prior to this,
         * if so, we're already offhook, so do not report a offhook event,
         * just stop polling.
         */
        if((pHookChangeData->last_state_reported == SI_HC_ONHOOK_TIMEOUT)
            || (pHookChangeData->last_state_reported == SI_HC_NO_ACTIVITY) )
        {
          pHookChangeData->last_state_reported = SI_HC_OFFHOOK_TIMEOUT;
          return SI_HC_OFFHOOK_TIMEOUT;
        }
        else
        {
          return SI_HC_NO_ACTIVITY;
        }
      }
    }
    else
    {
      uInt8 last_digit = pHookChangeData->currentPulseDigit;

      if(last_digit)
      {
        pHookChangeData->last_state_reported = last_digit;
        ProSLIC_ResetDialPulseDetect(pHookChangeData);
        return last_digit;
      }
    }
    return SI_HC_NEED_MORE_POLLS;
  }

  return SI_HC_NEED_MORE_POLLS;
}
#endif /* DISABLE_HOOKCHANGE */

/*****************************************************************************************************/
int ProSLIC_DTMFReadDigit (proslicChanType_ptr pProslic,uInt8 *pDigit)
{
  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  DEBUG_PRINT(pProslic, "%sDTMFReadDigit on channel %d\n",LOGPRINT_PREFIX,
              pProslic->channel);
  *pDigit = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_TONDTMF) & 0xf;

  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_PLLFreeRunStart (proslicChanType_ptr hProslic)
{
  uInt8 tmp;
  TRACEPRINT(hProslic, "\n", NULL);

  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  WriteReg(hProslic,hProslic->channel,PROSLIC_REG_ZCAL_EN,0x00);
  tmp = ReadReg(hProslic,hProslic->channel,PROSLIC_REG_ENHANCE);

  return(WriteReg(hProslic,hProslic->channel,PROSLIC_REG_ENHANCE,tmp|0x4));
}

/*****************************************************************************************************/
int ProSLIC_PLLFreeRunStop (proslicChanType_ptr hProslic)
{
  uInt8 tmp;
  TRACEPRINT(hProslic, "\n", NULL);

  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  tmp = ReadReg(hProslic,hProslic->channel,PROSLIC_REG_ENHANCE);
  WriteReg(hProslic,hProslic->channel,PROSLIC_REG_ENHANCE,tmp&~(0x4));

  return WriteReg(hProslic,hProslic->channel,PROSLIC_REG_ZCAL_EN,0x04);
}

/*****************************************************************************************************/
int ProSLIC_GetPLLFreeRunStatus (proslicChanType_ptr hProslic)
{
  uInt8 tmp;
  TRACEPRINT(hProslic, "\n", NULL);

  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  tmp = ReadReg(hProslic,hProslic->channel,PROSLIC_REG_ENHANCE);

  if(tmp & 0x02)
  {
    return RC_PLL_FREERUN_ACTIVE;
  }

  return RC_NONE;
}

/*****************************************************************************************************/
#ifndef DISABLE_PULSEMETERING
int ProSLIC_PulseMeterEnable (proslicChanType_ptr hProslic)
{
  uInt8 widebandEn;
  TRACEPRINT(hProslic, "\n", NULL);

  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  widebandEn = ReadReg(hProslic,hProslic->channel,PROSLIC_REG_ENHANCE) & 0x01;

  if (widebandEn)
  {
    DEBUG_PRINT (hProslic,
                 "%s Pulse Metering is not supported while Wideband Mode is enabled.\n",
                 LOGPRINT_PREFIX);
  }

#if defined (SI3217X) || defined (SI3218X) || defined (SI3226X) || defined (SI3228X) || defined(SI3219X)
  if(!widebandEn && (0 
#ifdef SI3217X
      || (hProslic->deviceId->chipType >= SI32171
          && hProslic->deviceId->chipType <= SI32179)
#endif
#ifdef SI3218X
      ||(hProslic->deviceId->chipType >= SI32180
         && hProslic->deviceId->chipType <= SI32189) 
#endif
#ifdef SI3219X
      || (IS_SI3219X(hProslic->deviceId) )
#endif
#ifdef SI3226X
      || (hProslic->deviceId->chipType >= SI32260
          && hProslic->deviceId->chipType <= SI32269)
#endif
#ifdef SI3228X
      || (hProslic->deviceId->chipType >= SI32280
          && hProslic->deviceId->chipType <= SI32289 )
#endif
    ))
  {
    return WriteReg(hProslic,hProslic->channel,PROSLIC_REG_PMCON,ReadReg(hProslic,
                    hProslic->channel,PROSLIC_REG_PMCON) | (0x01));
  }
#endif
  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_PulseMeterDisable (proslicChanType_ptr hProslic)
{
  TRACEPRINT(hProslic, "\n", NULL);

  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  return(WriteReg(hProslic,hProslic->channel,PROSLIC_REG_PMCON,ReadReg(hProslic,
                  hProslic->channel,PROSLIC_REG_PMCON) & ~(0x05)));
}

/*****************************************************************************************************/
int ProSLIC_PulseMeterStart (proslicChanType_ptr hProslic)
{
  TRACEPRINT(hProslic, "\n", NULL);

  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  return WriteReg(hProslic,hProslic->channel,PROSLIC_REG_PMCON,ReadReg(hProslic,
                  hProslic->channel,PROSLIC_REG_PMCON) | (0x5));
}

/*****************************************************************************************************/
int ProSLIC_PulseMeterStop (proslicChanType_ptr hProslic)
{
  TRACEPRINT(hProslic, "\n", NULL);

  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return(WriteReg(hProslic,hProslic->channel,PROSLIC_REG_PMCON,ReadReg(hProslic,
                    hProslic->channel,PROSLIC_REG_PMCON) & ~(0x04)));
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return(WriteReg(hProslic,hProslic->channel,PROSLIC_REG_PMCON,ReadReg(hProslic,
                    hProslic->channel,PROSLIC_REG_PMCON) & ~(0x04)));
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return(WriteReg(hProslic,hProslic->channel,PROSLIC_REG_PMCON,ReadReg(hProslic,
                    hProslic->channel,PROSLIC_REG_PMCON) & ~(0x04)));
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return ProSLIC_PulseMeterDisable(hProslic);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return ProSLIC_PulseMeterDisable(hProslic);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}
#endif /* DISABLE_PULSEMETERING */
/*****************************************************************************************************/
int ProSLIC_SetDCDCInversionFlag (proslicChanType_ptr hProslic, uInt8 flag)
{
  TRACEPRINT(hProslic, "flag = %u\n", flag);

  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  hProslic->dcdc_polarity_invert = flag;

  return RC_COMPLETE_NO_ERR;
}


int ProSLIC_PowerUpConverter (proslicChanType_ptr hProslic)
{
  if(hProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (hProslic->deviceId->chipType >= SI32171
      && hProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_PowerUpConverter(hProslic);
  }
#endif

#ifdef SI3218X
  if (hProslic->deviceId->chipType >= SI32180
      && hProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_PowerUpConverter(hProslic);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(hProslic->deviceId) )
  {
    return Si3219x_PowerUpConverter(hProslic);
  }
#endif

#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_PowerUpConverter(hProslic);
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_PowerUpConverter(hProslic);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}
/*****************************************************************************************************/
static int ProSLIC_GetBatType(proslicChanType_ptr hProslic)
{
#ifdef SI3226X
  if (hProslic->deviceId->chipType >= SI32260
      && hProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_General_Configuration.batType;
  }
#endif

#ifdef SI3228X
  if (hProslic->deviceId->chipType >= SI32280
      && hProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_General_Configuration.batType;
  }
#endif

#if !defined(SI3226X) && !defined(SI3228X)
  SILABS_UNREFERENCED_PARAMETER(hProslic);
#endif
  return BO_DCDC_UNKNOWN;
}

/*****************************************************************************************************/
int ProSLIC_PowerDownConverter (proslicChanType_ptr pProslic)
{
  errorCodeType error = RC_NONE;
  int32 vbat;
  int timer = 0;

  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  /* Force channel out of powersavings mode and then put it in open state */
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_LINEFEED,LF_FWD_OHT);
  Delay(pProTimer,10);
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_LINEFEED, LF_OPEN);
  Delay(pProTimer,50);

  /* Don't try to shutdown converter if we're using external supplies or if the
     converter is already shutdown.
  */
  if((ProSLIC_GetBatType(pProslic) == BO_DCDC_EXTERNAL) ||
      (ReadRAM(pProslic,pProslic->channel,PROSLIC_RAM_PD_DCDC) & 0x100000) )
  {
    return RC_NONE;
  }

  WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_PD_DCDC,0x900000L);
  Delay(pProTimer,50);

  do
  {
    vbat = ReadRAM(pProslic, pProslic->channel, PROSLIC_RAM_MADC_VBAT);
    if(vbat & 0x10000000L)
    {
      vbat |= 0xF0000000L;
    }
    Delay(pProTimer,10);
  }
  while((vbat > COMP_5V) && (timer++ < PROSLIC_TIMEOUT_DCDC_DOWN));

  DEBUG_PRINT(pProslic, "%s VBAT Down = %d.%d v\n", LOGPRINT_PREFIX,
              (int)((vbat/SCALE_V_MADC)/1000),
              SIVOICE_ABS(((vbat/SCALE_V_MADC) - (vbat/SCALE_V_MADC)/1000*1000)));

  if(timer > PROSLIC_TIMEOUT_DCDC_DOWN)
  {
    /* Error handling - shutdown converter, disable channel, set error tag */
    pProslic->channelEnable = 0;
    pProslic->error = error = RC_VBAT_DOWN_TIMEOUT;
    DEBUG_PRINT(pProslic, "%sDCDC Power Down timeout channel %d\n", LOGPRINT_PREFIX,
                pProslic->channel);
  }
  return error;
}

/*****************************************************************************************************/
int ProSLIC_LBCal (proslicChanType_ptr *pProslic, int size)
{
  int k;
  int timeout;
  uInt8 data;
  uInt8 lfState, enhance_value;
  TRACEPRINT(*pProslic, "size = %d\n", size);

  if((*pProslic)->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  /* For all channels, save settings, initiate LBCAL */
  for(k = 0; k < size; k++)
  {
    if( (pProslic[k]->channelEnable) && (pProslic[k]->channelType == PROSLIC) )
    {
      DEBUG_PRINT(pProslic[k], "%sStarting LB Cal on channel %d\n", LOGPRINT_PREFIX,
                  pProslic[k]->channel);
      /* Preserve old settings */
      lfState = ReadReg(pProslic[k], pProslic[k]->channel, PROSLIC_REG_LINEFEED);
      enhance_value = ReadReg(pProslic[k], pProslic[k]->channel, PROSLIC_REG_ENHANCE);
      ProSLIC_SetPowersaveMode(pProslic[k], PWRSAVE_DISABLE);
      ProSLIC_SetLinefeedStatus(pProslic[k], LF_FWD_ACTIVE);
      WriteReg(pProslic[k], pProslic[k]->channel, PROSLIC_REG_CALR0, CAL_LB_ALL);
      WriteReg(pProslic[k], pProslic[k]->channel, PROSLIC_REG_CALR3,  0x80);
      pProslic[k]->error = RC_CAL_TIMEOUT; /* Assume failure */

      timeout = 0;
      do
      {
        data = ReadReg(pProslic[k], pProslic[k]->channel, PROSLIC_REG_CALR3);
        DelayX(*pProslic, 10);
        timeout++;
      }
      while((data & 0x80) && (timeout < TIMEOUT_LB_CAL));

      if( (data & 0x80) )
      {
        DEBUG_PRINT(pProslic[k], "%sLB Cal timeout on channel %d\n", LOGPRINT_PREFIX,
                    pProslic[k]->channel);
        WriteReg(pProslic[k], pProslic[k]->channel, PROSLIC_REG_LINEFEED, LF_OPEN);
        return RC_CAL_TIMEOUT;
      }
      else
      {
        WriteReg(pProslic[k], pProslic[k]->channel, PROSLIC_REG_LINEFEED, lfState);
        WriteReg(pProslic[k], pProslic[k]->channel, PROSLIC_REG_ENHANCE, enhance_value);
        pProslic[k]->error = RC_NONE;
      }
    }
  }
  return RC_NONE;
}

/*****************************************************************************************************/
#define PROSLIC_RAM_CMDAC_FWD 1476
#define PROSLIC_RAM_CMDAC_REV 1477
#define PROSLIC_RAM_CAL_TRNRD_DACT 1458
#define PROSLIC_RAM_CAL_TRNRD_DACR 1459

int ProSLIC_GetLBCalResult (proslicChanType *pProslic,int32 *result1,
                            int32 *result2, int32 *result3, int32 *result4)
{
  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  *result1 = ReadRAM(pProslic, pProslic->channel, PROSLIC_RAM_CMDAC_FWD);
  *result2 = ReadRAM(pProslic, pProslic->channel, PROSLIC_RAM_CMDAC_REV);
  *result3 = ReadRAM(pProslic, pProslic->channel, PROSLIC_RAM_CAL_TRNRD_DACT);
  *result4 = ReadRAM(pProslic, pProslic->channel, PROSLIC_RAM_CAL_TRNRD_DACR);

  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_GetLBCalResultPacked (proslicChanType *pProslic,int32 *result)
{
  int32 results[4];
  int rc;
  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  rc = ProSLIC_GetLBCalResult(pProslic, &(results[0]), &(results[1]),
                             &(results[2]), &(results[3]));
  if(rc == RC_NONE)
  {
    /*
      Bits 31:24   CMDAC_FWD[25:18]
      Bits 23:16   CMDAC_REV[25:18]
      Bits 15:8    CAL_TRNRD_DACT[20:13]
      Bits 7:0     CAL_TRNRD_DACR[20:13]
    */
    *result = (results[0]<<6) & 0xff000000L;
    *result |=(results[1]>>1) & 0x00ff0000L;
    *result |=(results[2]>>5) & 0x0000ff00L;
    *result |=(results[3]>>13)& 0x000000ffL;
  }

  return rc;
}

/*****************************************************************************************************/
int ProSLIC_LoadPreviousLBCal (proslicChanType *pProslic,int32 result1,
                               int32 result2,int32 result3,int32 result4)
{
  TRACEPRINT(pProslic, "\n", NULL);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_CMDAC_FWD,result1);
  WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_CMDAC_REV,result2);
  WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_CAL_TRNRD_DACT,result3);
  WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_CAL_TRNRD_DACR,result4);

#ifdef API_TEST
  ramVal = ReadRAM(pProslic,pProslic->channel,PROSLIC_RAM_CMDAC_FWD);
  LOGPRINT ("%s UNPACKED CMDAC_FWD = %08x\n", LOGPRINT_PREFIX, ramVal);
  ramVal = ReadRAM(pProslic, pProslic->channel,PROSLIC_RAM_CMDAC_REV);
  LOGPRINT ("%s UNPACKED CMDAC_REF = %08x\n", LOGPRINT_PREFIX, ramVal);
  ramVal = ReadRAM(pProslic,pProslic->channel,PROSLIC_RAM_CAL_TRNRD_DACT);
  LOGPRINT ("%s UNPACKED CAL_TRNRD_DACT = %08x\n", LOGPRINT_PREFIX, ramVal);
  ramVal = ReadRAM(pProslic,pProslic->channel,PROSLIC_RAM_CAL_TRNRD_DACR);
  LOGPRINT ("%s UNPACKED CAL_TRNRD_DACR = %08x\n", LOGPRINT_PREFIX, ramVal);
#endif

  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_LoadPreviousLBCalPacked (proslicChanType *pProslic,int32 *result)
{
  ramData ramVal[4];
  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
  ramVal[0] = (*result&0xff000000L)>>6;
  ramVal[1] = (*result&0x00ff0000L)<<1;
  ramVal[2] = (*result&0x0000ff00L)<<5;
  ramVal[3] = (*result&0x000000ffL)<<13;

  return ProSLIC_LoadPreviousLBCal(pProslic, ramVal[0], ramVal[1], ramVal[2],
                                   ramVal[3]);
}

/*****************************************************************************************************/
int ProSLIC_dbgSetDCFeedVopen (proslicChanType *pProslic, uInt32 v_vlim_val,
                               int32 preset)
{
  TRACEPRINT(pProslic, "v_vlim_val = %u preset = %d\n", v_vlim_val, preset);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#ifdef SI3217X
  if (pProslic->deviceId->chipType >= SI32171
      && pProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_dbgSetDCFeedVopen (pProslic,v_vlim_val,preset);
  }
#endif

#ifdef SI3218X
  if (pProslic->deviceId->chipType >= SI32180
      && pProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_dbgSetDCFeedVopen (pProslic,v_vlim_val,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(pProslic->deviceId) )
  {
    return Si3219x_dbgSetDCFeedVopen (pProslic,v_vlim_val,preset);
  }
#endif

#ifdef SI3226X
  if (pProslic->deviceId->chipType >= SI32260
      && pProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_dbgSetDCFeedVopen (pProslic,v_vlim_val,preset);
  }
#endif

#ifdef SI3228X
  if (pProslic->deviceId->chipType >= SI32280
      && pProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_dbgSetDCFeedVopen (pProslic,v_vlim_val,preset);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_dbgSetDCFeedIloop (proslicChanType *pProslic, uInt32 i_ilim_val,
                               int32 preset)
{
  TRACEPRINT(pProslic, "i_ilim_val = %u preset = %d\n", i_ilim_val, preset);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }
#ifdef SI3217X
  if (pProslic->deviceId->chipType >= SI32171
      && pProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_dbgSetDCFeedIloop (pProslic,i_ilim_val,preset);
  }
#endif

#ifdef SI3218X
  if (pProslic->deviceId->chipType >= SI32180
      && pProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_dbgSetDCFeedIloop (pProslic,i_ilim_val,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(pProslic->deviceId) )
  {
    return Si3219x_dbgSetDCFeedIloop (pProslic,i_ilim_val,preset);
  }
#endif

#ifdef SI3226X
  if (pProslic->deviceId->chipType >= SI32260
      && pProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_dbgSetDCFeedIloop (pProslic,i_ilim_val,preset);
  }
#endif

#ifdef SI3228X
  if (pProslic->deviceId->chipType >= SI32280
      && pProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_dbgSetDCFeedIloop (pProslic,i_ilim_val,preset);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_dbgSetRinging (proslicChanType *pProslic,
                           ProSLIC_dbgRingCfg *ringCfg, int preset)
{
  TRACEPRINT(pProslic, "preset = %d\n", preset);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (pProslic->deviceId->chipType >= SI32171
      && pProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_dbgSetRinging (pProslic,ringCfg,preset);
  }
#endif

#ifdef SI3218X
  if (pProslic->deviceId->chipType >= SI32180
      && pProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_dbgSetRinging (pProslic,ringCfg,preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(pProslic->deviceId) )
  {
    return Si3219x_dbgSetRinging (pProslic,ringCfg,preset);
  }
#endif

#ifdef SI3226X
  if (pProslic->deviceId->chipType >= SI32260
      && pProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_dbgSetRinging (pProslic,ringCfg,preset);
  }
#endif

#ifdef SI3228X
  if (pProslic->deviceId->chipType >= SI32280
      && pProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_dbgSetRinging (pProslic,ringCfg,preset);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_dbgSetRXGain (proslicChanType *pProslic, int32 gain,
                          int impedance_preset, int audio_gain_preset)
{
  TRACEPRINT(pProslic, "gain = %d imp_preset = %d audio_gain_preset = %d\n", gain,
             impedance_preset, audio_gain_preset);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (pProslic->deviceId->chipType >= SI32171
      && pProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_dbgSetRXGain (pProslic,gain,impedance_preset,audio_gain_preset);
  }
#endif

#ifdef SI3218X
  if (pProslic->deviceId->chipType >= SI32180
      && pProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_dbgSetRXGain (pProslic,gain,impedance_preset,audio_gain_preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(pProslic->deviceId) )
  {
    return Si3219x_dbgSetRXGain (pProslic,gain,impedance_preset,audio_gain_preset);
  }
#endif

#ifdef SI3226X
  if (pProslic->deviceId->chipType >= SI32260
      && pProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_dbgSetRXGain (pProslic,gain,impedance_preset,audio_gain_preset);
  }
#endif

#ifdef SI3228X
  if (pProslic->deviceId->chipType >= SI32280
      && pProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_dbgSetRXGain (pProslic,gain,impedance_preset,audio_gain_preset);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
int ProSLIC_dbgSetTXGain (proslicChanType *pProslic, int32 gain,
                          int impedance_preset, int audio_gain_preset)
{
  TRACEPRINT(pProslic, "gain = %d imp_preset = %d audio_gain_preset = %d\n", gain,
             impedance_preset, audio_gain_preset);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (pProslic->deviceId->chipType >= SI32171
      && pProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_dbgSetTXGain (pProslic,gain,impedance_preset,audio_gain_preset);
  }
#endif

#ifdef SI3218X
  if (pProslic->deviceId->chipType >= SI32180
      && pProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_dbgSetTXGain (pProslic,gain,impedance_preset,audio_gain_preset);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(pProslic->deviceId) )
  {
    return Si3219x_dbgSetTXGain (pProslic,gain,impedance_preset,audio_gain_preset);
  }
#endif

#ifdef SI3226X
  if (pProslic->deviceId->chipType >= SI32260
      && pProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_dbgSetTXGain (pProslic,gain,impedance_preset,audio_gain_preset);
  }
#endif

#ifdef SI3228X
  if (pProslic->deviceId->chipType >= SI32280
      && pProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_dbgSetTXGain (pProslic,gain,impedance_preset,audio_gain_preset);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_LineMonitor
**
** Description:
** Generic monitoring function
**
** Returns:
** 0
*/

int ProSLIC_LineMonitor (proslicChanType *pProslic, proslicMonitorType *monitor)
{
  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (pProslic->deviceId->chipType >= SI32171
      && pProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_LineMonitor (pProslic, monitor);
  }
#endif

#ifdef SI3218X
  if (pProslic->deviceId->chipType >= SI32180
      && pProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_LineMonitor (pProslic, monitor);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(pProslic->deviceId) )
  {
    return Si3219x_LineMonitor (pProslic, monitor);
  }
#endif

#ifdef SI3226X
  if (pProslic->deviceId->chipType >= SI32260
      && pProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_LineMonitor (pProslic, monitor);
  }
#endif

#ifdef SI3228X
  if (pProslic->deviceId->chipType >= SI32280
      && pProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_LineMonitor (pProslic, monitor);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_PSTNCheck
**
** Description:
** Monitor for excessive longitudinal current, which
** would be present if a live pstn line was connected
** to the port.
**
** Returns:
** 0 - no pstn detected
** 1 - pstn detected
*/
int ProSLIC_PSTNCheck (proslicChanType *pProslic,
                       proslicPSTNCheckObjType *pPSTNCheck)
{
  TRACEPRINT(pProslic, "\n", NULL);
  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (pProslic->deviceId->chipType >= SI32171
      && pProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_PSTNCheck (pProslic,pPSTNCheck);
  }
#endif

#ifdef SI3218X
  if (pProslic->deviceId->chipType >= SI32180
      && pProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_PSTNCheck (pProslic,pPSTNCheck);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(pProslic->deviceId) )
  {
    return Si3219x_PSTNCheck (pProslic,pPSTNCheck);
  }
#endif

#ifdef SI3226X
  if (pProslic->deviceId->chipType >= SI32260
      && pProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_PSTNCheck (pProslic,pPSTNCheck);
  }
#endif

#ifdef SI3228X
  if (pProslic->deviceId->chipType >= SI32280
      && pProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_PSTNCheck (pProslic,pPSTNCheck);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_DiffPSTNCheck
**
** Description:
** Monitor for excessive longitudinal current, which
** would be present if a live pstn line was connected
** to the port.
**
** Returns:
** RC_NONE - test in progress
** RC_IGNORE - non-ProSLIC channel
** RC_COMPLETE_NO_ERR - ProSLIC does not support feature.
** RC_PSTN_OPEN_FEMF | RC_COMPLETE_NO_ERR - test completed.
**
*/
#ifdef PSTN_DET_ENABLE
int ProSLIC_DiffPSTNCheck (proslicChanType *pProslic,
                           proslicDiffPSTNCheckObjType *pPSTNCheck)
{
  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  if (pProslic->deviceId->chipType >= SI32171
      && pProslic->deviceId->chipType <= SI32179)
  {
    return Si3217x_DiffPSTNCheck (pProslic,pPSTNCheck);
  }
#endif

#ifdef SI3218X
  if (pProslic->deviceId->chipType >= SI32180
      && pProslic->deviceId->chipType <= SI32189)
  {
    return Si3218x_DiffPSTNCheck (pProslic,pPSTNCheck);
  }
#endif

#ifdef SI3219X
  if ( IS_SI3219X(pProslic->deviceId) )
  {
    return Si3219x_DiffPSTNCheck (pProslic,pPSTNCheck);
  }
#endif

#ifdef SI3226X
  if (pProslic->deviceId->chipType >= SI32260
      && pProslic->deviceId->chipType <= SI32269)
  {
    return Si3226x_DiffPSTNCheck (pProslic,pPSTNCheck);
  }
#endif

#ifdef SI3228X
  if (pProslic->deviceId->chipType >= SI32280
      && pProslic->deviceId->chipType <= SI32289)
  {
    return Si3228x_DiffPSTNCheck (pProslic,pPSTNCheck);
  }
#endif

  return RC_COMPLETE_NO_ERR;
}
#endif

/*****************************************************************************************************/
/*
** Function: ProSLIC_CreatePSTNCheckObj
**
** Description:
** Allocate memory for pstnCheckObj
**
** Returns:
** RC_NONE
** RC_NO_MEM if failed to allocate memory.
** RC_UNSUPPORTED_FEATER if malloc disabled
*/
int ProSLIC_CreatePSTNCheckObjs(proslicPSTNCheckObjType_ptr *pstnCheckObj,
                                unsigned int num_objs)
{
  TRACEPRINT_NOCHAN("num_objs = %u\n", num_objs);
#ifndef DISABLE_MALLOC
  *pstnCheckObj = SIVOICE_CALLOC(sizeof(proslicPSTNCheckObjType),num_objs);
  if(*pstnCheckObj == NULL)
  {
#ifdef ENABLE_DEBUG
    LOGPRINT("%s: %s: failed to allocate memory", LOGPRINT_PREFIX, __FUNCTION__);
#endif
    return RC_NO_MEM;
  }
  return RC_NONE;
#else
  SILABS_UNREFERENCED_PARAMETER(pstnCheckObj);
  SILABS_UNREFERENCED_PARAMETER(num_objs);
  return RC_UNSUPPORTED_FEATURE;
#endif
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_CreateDiffPSTNCheckObjs
**
** Description:
** Allocate memory for proslicDiffPstnCheckObjs
**
** Returns:
** RC_NONE
** RC_NO_MEM if failed to allocate memory.
** RC_UNSUPPORTED_FEATURE if malloc disabled
*/

#ifdef PSTN_DET_ENABLE
int ProSLIC_CreateDiffPSTNCheckObjs(proslicDiffPSTNCheckObjType_ptr
                                    *pstnCheckObj, unsigned int num_objs)
{
  TRACEPRINT_NOCHAN("num_objs = %u\n", num_objs);

#ifndef DISABLE_MALLOC
  *pstnCheckObj = SIVOICE_CALLOC(sizeof(proslicDiffPSTNCheckObjType),num_objs);
  if(*pstnCheckObj == NULL)
  {
#ifdef ENABLE_DEBUG
    LOGPRINT("%s: %s: failed to allocate memory", LOGPRINT_PREFIX, __FUNCTION__);
#endif
    return RC_NO_MEM;
  }
  return RC_NONE;
#else
  SILABS_UNREFERENCED_PARAMETER(pstnCheckObj);
  SILABS_UNREFERENCED_PARAMETER(num_objs);
  return RC_UNSUPPORTED_FEATURE;
#endif
}
#endif

/*****************************************************************************************************/
/*
** Function: ProSLIC_DestroyPSTNCheckObjs
**
** Description:
** Free memory for pstnCheckObj
**
** Returns:
** RC_NONE
** RC_UNSUPPORTED_FEATER if malloc disabled
*/
int ProSLIC_DestroyPSTNCheckObjs(proslicPSTNCheckObjType_ptr *pstnCheckObj)
{
  TRACEPRINT_NOCHAN("\n", NULL);

#ifndef DISABLE_MALLOC
  if(pstnCheckObj)
  {
    SIVOICE_FREE((proslicPSTNCheckObjType_ptr)*pstnCheckObj);
  }

  return RC_NONE;

#else
  SILABS_UNREFERENCED_PARAMETER(pstnCheckObj);
  return RC_UNSUPPORTED_FEATURE;
#endif
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_DestroyDiffPSTNCheckObjs
**
** Description:
** Free memory for pstnDiffCheckObj
**
** Returns:
** RC_NONE
** RC_UNSUPPORTED_FEATER if malloc disabled
*/
#ifdef PSTN_DET_ENABLE
int ProSLIC_DestroyDiffPSTNCheckObjs(proslicDiffPSTNCheckObjType_ptr
                                     *pstnCheckObj)
{
  TRACEPRINT_NOCHAN("\n", NULL);

#ifndef DISABLE_MALLOC
  if(pstnCheckObj)
  {
    SIVOICE_FREE((proslicDiffPSTNCheckObjType_ptr)*pstnCheckObj);
  }
  return RC_NONE;
#else
  SILABS_UNREFERENCED_PARAMETER(pstnCheckObj);
  return RC_UNSUPPORTED_FEATURE;
#endif
}
#endif

/*****************************************************************************************************/
/*
** Function: ProSLIC_InitPSTNCheckObj
**
** Description:
** Initialize pstnCheckObj structure members
**
** Returns:
** RC_NONE
*/
int ProSLIC_InitPSTNCheckObj(proslicPSTNCheckObjType_ptr pstnCheckObj,
                             int32 avgThresh, int32 singleThresh, uInt8 samples)
{
  TRACEPRINT_NOCHAN("avgThres = %d singleThresh = %d samples = %u\n", avgThresh,
                    avgThresh, samples);

  pstnCheckObj->avgThresh = avgThresh;
  pstnCheckObj->singleThresh = singleThresh;
  pstnCheckObj->samples = samples;
  pstnCheckObj->avgIlong = 0;
  pstnCheckObj->count = 0;
  pstnCheckObj->buffFull = 0;

  return RC_NONE;
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_InitDiffPSTNCheckObj
**
** Description:
** Initialize pstnCheckObj structure members
**
** Returns:
** RC_NONE
*/
#ifdef PSTN_DET_ENABLE
int ProSLIC_InitDiffPSTNCheckObj(proslicDiffPSTNCheckObjType_ptr
                                 pstnDiffCheckObj,
                                 int preset1,
                                 int preset2,
                                 int entry_preset,
                                 int femf_enable)
{
  TRACEPRINT_NOCHAN("p1 = %d p2 = %d ep = %d femf_enable = %d\n", preset1,
                    preset2, entry_preset, femf_enable);

  pstnDiffCheckObj->samples = PSTN_DET_DIFF_SAMPLES;
  pstnDiffCheckObj->max_femf_vopen = PSTN_DET_MAX_FEMF;
  pstnDiffCheckObj->entryDCFeedPreset = entry_preset;
  pstnDiffCheckObj->dcfPreset1 = preset1;
  pstnDiffCheckObj->dcfPreset2 = preset2;
  pstnDiffCheckObj->femf_enable = femf_enable;
  pstnDiffCheckObj->pState.stage = 0;
  pstnDiffCheckObj->pState.sampleIterations = 0;
  pstnDiffCheckObj->pState.waitIterations = 0;

  return RC_NONE;
}
#endif

/*****************************************************************************************************/
/*
** Function: ProSLIC_SetPwrsaveMode
**
** Description:
** Enable or disable powersave mode
**
** Returns:
** RC_NONE
*/
#define PROSLIC_REG_ENHANCE 47

int ProSLIC_SetPowersaveMode (proslicChanType *pProslic, int pwrsave)
{
  uInt8 regData;
  TRACEPRINT(pProslic, "pwrsave = %d\n", pwrsave);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  regData = ReadReg(pProslic,pProslic->channel, PROSLIC_REG_ENHANCE);

  if(pwrsave == PWRSAVE_DISABLE)
  {
#ifdef SI3217X
    if (pProslic->deviceId->chipType >= SI32171
        && pProslic->deviceId->chipType <= SI32179)
    {
      regData &= 0x27;
    }
    else
    {
#endif
      regData &= 0x07;
#ifdef SI3217X
    }
#endif
  }
  else
  {
    regData |= 0x10;
  }

  return WriteReg(pProslic,pProslic->channel, PROSLIC_REG_ENHANCE, regData);
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_SetDAAEnable
**
** Description:
** Enable or disable adjacent FXO (Si32178 only)
**
** Returns:
** RC_NONE
*/
int ProSLIC_SetDAAEnable (proslicChanType *pProslic, int enable)
{
  TRACEPRINT(pProslic, "\n", NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

#ifdef SI3217X
  return Si3217x_SetDAAEnable (pProslic,enable);
#else
  SILABS_UNREFERENCED_PARAMETER(enable);
  return RC_NONE;
#endif
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_SetUserMode
**
** Description: Enables User Access Mode (UAM) if the part supports it.
**
** Returns:
** RC_NONE if successful.
*/
#define PROSLIC_REG_USERMODE_ENABLE 126

int ProSLIC_SetUserMode(proslicChanType *pProslic,BOOLEAN isEnabled,
                        BOOLEAN isBcast)
{
  uInt8 data;
  uInt8 channel;

  TRACEPRINT(pProslic, "enable = %d bcast = %d\n", (int)isEnabled, (int)isBcast);

  if(isEnabled == FALSE)
  {
    return RC_NONE;
  }

  if( pProslic->channelType != PROSLIC )
  {
    return RC_UNSUPPORTED_FEATURE;
  }

  if(isBcast == TRUE)
  {
    channel = PROSLIC_CHAN_BROADCAST;
  }
  else
  {
    channel = pProslic->channel;
  }

  data = ReadReg(pProslic,pProslic->channel,
                 PROSLIC_REG_USERMODE_ENABLE); /*we check first channel. we assume all channels same user mode state for broadcast */

  if (((data&1) != 0) == isEnabled)
  {
    return RC_NONE;
  }

  WriteReg(pProslic,channel,PROSLIC_REG_USERMODE_ENABLE,2);
  WriteReg(pProslic,channel,PROSLIC_REG_USERMODE_ENABLE,8);
  WriteReg(pProslic,channel,PROSLIC_REG_USERMODE_ENABLE,0xe);
  return WriteReg(pProslic,channel,PROSLIC_REG_USERMODE_ENABLE,0);
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_isReinitRequired
**
** Description:
** Checks for improper ring exit
**
** Returns:
** RC_NONE                -  Reinit not required
** RC_REINIT_REQUIRED     -  Corrupted state machine - reinit required
**
*/

int ProSLIC_isReinitRequired(proslicChanType *pProslic)
{
  uInt8 lf;
  ramData rkdc_sum;
  TRACEPRINT(pProslic, "\n", NULL);

  if( pProslic->channelType != PROSLIC)
  {
    return RC_IGNORE;
  }

  /* Check for improper ring exit which may cause dcfeed corruption */

  lf = ReadReg(pProslic, pProslic->channel,PROSLIC_REG_LINEFEED);
  rkdc_sum = ReadRAM(pProslic,pProslic->channel,PROSLIC_RAM_RDC_SUM);
  DEBUG_PRINT(pProslic, "%sisReinitRequired: Linefeed = 0x%02X RDC_SUM = %d\n",
              LOGPRINT_PREFIX, (int)lf, (int) rkdc_sum);
  if((rkdc_sum & 0x400000)&&(lf != 0x44))
  {
    return RC_REINIT_REQUIRED;
  }
  else
  {
    return RC_NONE;
  }
}

/*****************************************************************************************************/
/*
** Function: LoadRegTable
**
** Description:
** Generic function to load register/RAM with predefined addr/value
*/
int ProSLIC_LoadRegTable (proslicChanType *pProslic, 
                          const ProslicRAMInit *pRamTable,
                          const ProslicRegInit *pRegTable, 
                          int broadcast)
{
  uInt16 i;
  uInt8 channel;

  TRACEPRINT(pProslic, "bcast = %d\n", broadcast);
  /* DAA doesn't have a RAM table.. skip it... */
  if( (pRamTable != 0)
      && (pProslic->channelType != PROSLIC) )
  {
    return RC_IGNORE;
  }

  if (broadcast)
  {
    channel = PROSLIC_CHAN_BROADCAST;
  }
  else
  {
    channel = pProslic->channel;
  }

  i=0;
  if (pRamTable != 0)
  {
    while (pRamTable[i].address != 0xffff)
    {
      WriteRAM(pProslic,channel,pRamTable[i].address,pRamTable[i].initValue);
      i++;
    }
  }
  i=0;
  if (pRegTable != 0)
  {
    while (pRegTable[i].address != 0xff)
    {
      WriteReg(pProslic,channel,pRegTable[i].address,pRegTable[i].initValue);
      i++;
    }
  }

  return RC_NONE;
}

/*****************************************************************************************************/
/*
** Function: LoadRegTables
**
** Description:
** Generic function to load register/RAM with predefined addr/value
*/

int ProSLIC_LoadRegTables(proslicChanType_ptr *pProslic,
                          const ProslicRAMInit *pRamTable, const ProslicRegInit *pRegTable, int size)
{
  int i;
  TRACEPRINT(*pProslic, "size = %d\n", size);
  for (i=0; i<size; i++)
  {
    if (pProslic[i]->channelEnable)
    {
      ProSLIC_LoadRegTable(pProslic[i],pRamTable,pRegTable,FALSE);
    }
  }
  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_UnsupportedFeatureNoArg(proslicChanType_ptr pProslic,
                                    const char *function_name)
{
#ifdef ENABLE_DEBUG
  LOGPRINT("%s: unsupported %s was called on channel %d\n", LOGPRINT_PREFIX,
           function_name, pProslic->channel);
#else
  SILABS_UNREFERENCED_PARAMETER(pProslic);
  SILABS_UNREFERENCED_PARAMETER(function_name);
#endif
  return RC_UNSUPPORTED_FEATURE;
}

/*****************************************************************************************************/
/*
** Function: ProSLIC_PSTN_delay_poll
**
** Description:
** Delay function called within PSTN detection functions
**
** Return Value:
** none
*/
#ifdef PSTN_DET_ENABLE
void ProSLIC_PSTN_delay_poll(proslicTestStateType *pState, uInt16 delay)
{
  uInt16 delayCount;

  if((delay/PSTN_DET_POLL_RATE) < 2)
  {
    delayCount = 0;
  }
  else
  {
    delayCount = (delay/PSTN_DET_POLL_RATE) - 2;
  }

  pState->waitIterations++;
  if((pState->waitIterations == delayCount) || (delayCount == 0))
  {
    pState->waitIterations = 0;
    pState->stage++;
  }
}
#endif

/*****************************************************************************************************/
/*
** Function: ProSLIC_Calibrate
**
** Description:
** Performs calibration based on passed ptr to array of
** desired CALRn settings.
**
** Starts calibration on all channels sequentially (not broadcast)
** and continuously polls for completion.  Return error code if
** CAL_EN does not clear for each enabled channel within the passed
** timeout period.
*/
int ProSLIC_Calibrate(proslicChanType_ptr *pProslic, int maxChan, uInt8 *calr,
                      int maxTime)
{
  int i,j, cal_en;
  int cal_en_chan = 0;
  int timer = 0;

  TRACEPRINT(*pProslic, "maxChan = %d time = %d\n", maxChan, maxTime );

  if ((*pProslic)->channelType != PROSLIC)
  {
    return RC_CHANNEL_TYPE_ERR;
  }

  for(i = 0; i < maxChan; i++)
  {
    if((pProslic[i]->channelEnable)&&(pProslic[i]->channelType == PROSLIC))
    {
      for(j = 0; j < 4; j++)
      {
        WriteReg(pProslic[i], pProslic[i]->channel, (PROSLIC_REG_CALR0+j), calr[j]);
      }
    }
  }

  /* Wait for calibration to complete */
  do
  {
    cal_en = 0;
    DelayX(*pProslic, 10);
    for(i = 0; i < maxChan; i++)
    {
      if( (pProslic[i]->channelEnable) && (pProslic[i]->channelType == PROSLIC))
      {
        cal_en_chan = ReadReg(pProslic[i], pProslic[i]->channel, PROSLIC_REG_CALR3);
        if( (cal_en_chan & 0x80) && (timer == maxTime) )
        {
          DEBUG_PRINT(pProslic[i], "%sCalibration timed out on channel %d\n",
                      LOGPRINT_PREFIX, pProslic[i]->channel);
          pProslic[i]->channelEnable = 0;
          pProslic[i]->error = RC_CAL_TIMEOUT;
        }
        cal_en |= cal_en_chan;
      }
    }
  }
  while( (timer++ <= maxTime) && (cal_en & 0x80) );

  if(timer <= maxTime)
  {
    return RC_NONE;
  }
  else
  {
    return RC_CAL_TIMEOUT;
  }
}
/*****************************************************************************************************/
/* This function is used by the xxxx_Init_with_Options - do not call this directly... */
int ProSLIC_ReInit_helper(proslicChanType_ptr *pProslic, int size,
                          initOptionsType init_option, int numChanPerDevice)
{
  int i;

  /* Preserve linefeed state.. */
  for(i = 0; i < size; i++)
  {
    pProslic[i]->scratch = ReadReg( pProslic[i], pProslic[i]->channel,
                                   PROSLIC_REG_LINEFEED);
    ProSLIC_PowerDownConverter( pProslic[i] );

    /* Clear MSTRSTAT on a device basis */
    if( (numChanPerDevice == 1) 
      || ((numChanPerDevice == 2) && ((i&1) == 0)  ) )
    {
      ProSLIC_WriteReg(pProslic[i], PROSLIC_REG_MSTRSTAT, 0xFF); /* Clear master status register */
    }
  }

  DelayX(*pProslic, 10); /* Wait to ensure system is powered down... */

  /* Do a soft reset if reinit is requested...*/
  if(init_option == INIT_REINIT)
  {
    for(i = 0; i < size; i++)
    {
      if(numChanPerDevice == 2) 
      {
        WriteReg(pProslic[i], (pProslic[i])->channel, PROSLIC_REG_RESET,
                 (((pProslic[i])->channel)&1)+1); /* reset 1 channel at a time */
      }
      else
      {
        WriteReg(pProslic[i], (pProslic[i])->channel, PROSLIC_REG_RESET, 1);
      }

      pProslic[i]->error = RC_NONE; /* Clear any errors we may have seen in powering down the converter */
    }
    DelayX(*pProslic, 100); /* Wait for a soft reset */

  }
  return RC_NONE;
}

/*****************************************************************************************************/
int ProSLIC_SoftReset(proslicChanType_ptr pProslic, uInt16 resetOptions)
{
  uInt8 data;

  TRACEPRINT(pProslic, "reset option: %d\n",resetOptions);

  /* We can't use channelType since it may not been set.  We assume at least 
   * SWInit was called... */
  if( pProslic->deviceId->chipType == SI3050 )
  {
    return RC_CHANNEL_TYPE_ERR;
  }

  /* Check if we're in free-run mode, if so abort */
  data = ReadReg(pProslic, pProslic->channel, PROSLIC_REG_ENHANCE);

  if(data & 0x6)
  {
    return RC_PLL_FREERUN_ACTIVE;
  }

  if(resetOptions & PROSLIC_BCAST_RESET)
  {
    return WriteReg(pProslic, PROSLIC_CHAN_BROADCAST, PROSLIC_REG_RESET, (uInt8)(resetOptions & 0xFF));
  }
  else
  {
    return WriteReg(pProslic, pProslic->channel, PROSLIC_REG_RESET, (uInt8)(resetOptions & 0xFF));
  }
}

/*****************************************************************************************************/
#ifndef DISABLE_TONE_SETUP
int ProSLIC_ToneGenSetupPtr(proslicChanType_ptr pProslic, ProSLIC_Tone_Cfg *cfg)
{
  TRACEPRINT(pProslic, "\n",NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_CHANNEL_TYPE_ERR;
  }

  WriteRAM(pProslic, pProslic->channel,PROSLIC_RAM_OSC1FREQ, cfg->osc1.freq);
  WriteRAM(pProslic, pProslic->channel,PROSLIC_RAM_OSC1AMP, cfg->osc1.amp);
  WriteRAM(pProslic, pProslic->channel,PROSLIC_RAM_OSC1PHAS, cfg->osc1.phas);
  WriteReg(pProslic, pProslic->channel,PROSLIC_REG_O1TAHI,cfg->osc1.tahi);
  WriteReg(pProslic, pProslic->channel,PROSLIC_REG_O1TALO,cfg->osc1.talo);
  WriteReg(pProslic, pProslic->channel,PROSLIC_REG_O1TIHI,cfg->osc1.tihi);
  WriteReg(pProslic, pProslic->channel,PROSLIC_REG_O1TILO,cfg->osc1.tilo);

  WriteRAM(pProslic, pProslic->channel,PROSLIC_RAM_OSC2FREQ, cfg->osc2.freq);
  WriteRAM(pProslic, pProslic->channel,PROSLIC_RAM_OSC2AMP, cfg->osc2.amp);
  WriteRAM(pProslic, pProslic->channel,PROSLIC_RAM_OSC2PHAS, cfg->osc2.phas);
  WriteReg(pProslic, pProslic->channel,PROSLIC_REG_O2TAHI,cfg->osc2.tahi);
  WriteReg(pProslic, pProslic->channel,PROSLIC_REG_O2TALO,cfg->osc2.talo);
  WriteReg(pProslic, pProslic->channel,PROSLIC_REG_O2TIHI,cfg->osc2.tihi);
  WriteReg(pProslic, pProslic->channel,PROSLIC_REG_O2TILO,cfg->osc2.tilo);


  WriteReg(pProslic, pProslic->channel,PROSLIC_REG_OMODE,cfg->omode);
  return RC_NONE;
}
#endif /* DISABLE_TONE_SETUP */

/*****************************************************************************************************/
#ifndef DISABLE_FSK_SETUP
int ProSLIC_FSKSetupPtr (proslicChanType_ptr pProslic, ProSLIC_FSK_Cfg *cfg)
{
  uInt8 data;
  int i;

  TRACEPRINT(pProslic, "\n",NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_CHANNEL_TYPE_ERR;
  }

  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_FSKDEPTH,
           0x08); /* Clear Buffer */
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_O1TAHI,0);
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_O1TIHI,0);
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_O1TILO,0);
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_O1TALO,0x13);

  data = ReadReg(pProslic,pProslic->channel,PROSLIC_REG_OMODE);
  if (cfg->eightBit)
  {
    data |= 0x80;
  }
  else
  {
    data &= ~(0x80);
  }
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_FSKDEPTH,cfg->fskdepth);
  WriteReg(pProslic,pProslic->channel,PROSLIC_REG_OMODE,data);

  for(i = 0; i < 2; i++)
  {
    WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_FSK01+i,cfg->fsk[i]);
    WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_FSKAMP0+i,cfg->fskamp[i]);
    WriteRAM(pProslic,pProslic->channel,PROSLIC_RAM_FSKFREQ0+i,cfg->fskfreq[i]);
  }
  return RC_NONE;
}
#endif

/*****************************************************************************************************/
int ProSLIC_EnableFastRingStart(proslicChanType_ptr pProslic, BOOLEAN isEnabled)
{
  uInt8 data;

  TRACEPRINT(pProslic, "\n",NULL);

  if(pProslic->channelType != PROSLIC)
  {
    return RC_CHANNEL_TYPE_ERR;
  }
#if defined(SI3217X) && !defined(Si3217X_REVC_ONLY)
  if (pProslic->deviceId->chipType >= SI32171
      && pProslic->deviceId->chipType <= SI32179)
  {
    if(pProslic->deviceId->chipRev < 2)
    {
      return RC_UNSUPPORTED_FEATURE;
    }
  }
#endif

  data = ReadReg(pProslic, pProslic->channel, PROSLIC_REG_USERSTAT);

  if( isEnabled )
  {
    data |= 8;
  }
  else
  {
    data &= ~8;
  }

  return WriteReg(pProslic, pProslic->channel, PROSLIC_REG_USERSTAT, data);
}

