// SPDX-License-Identifier: GPL-2.0
/*
 * Author(s):
 * laj
 *
 * Distributed by:
 * Silicon Laboratories, Inc
 *
 * This file contains proprietary information.
 * No dissemination allowed without prior written permission from
 * Silicon Laboratories, Inc.
 *
 * File Description:
 * This file contains common (ProSIC + DAA) functions.
 *
 */

#include "../config_inc/si_voice_datatypes.h"
#include "../inc/si_voice.h"
#include "../config_inc/proslic_api_config.h"

#ifdef ENABLE_DEBUG
#define LOGPRINT_PREFIX "SiVoice: "
#endif

#ifdef SI3217X
#include "si3217x.h"
#include "si3217x_intf.h"
#endif

#ifdef SI3218X
#include "../inc/si3218x.h"
#include "../inc/si3218x_intf.h"
#endif

#ifdef SI3219X
#include "si3219x.h"
#include "si3219x_intf.h"
#endif

#ifdef SI3226X
#include "si3226x.h"
#include "si3226x_intf.h"
#endif

#ifdef SI3228X
#include "si3228x.h"
#include "si3228x_intf.h"
#endif

#define pCtrl(X)           (X)->deviceId->ctrlInterface
#define pProHW(X)          pCtrl((X))->hCtrl
#define ReadReg(PCHAN, CHANNEL, REGADDR) (PCHAN)->deviceId->ctrlInterface->ReadRegister_fptr(pProHW(PCHAN), (CHANNEL), (REGADDR))
#define WriteReg(PCHAN, CHANNEL, REGADDR, REGDATA) (PCHAN)->deviceId->ctrlInterface->WriteRegister_fptr(pProHW(PCHAN), (CHANNEL), (REGADDR), (REGDATA))

/*
** Control object constructor/destructor
*/
int SiVoice_createControlInterfaces (SiVoiceControlInterfaceType **pCtrlIntf,
                                     uInt32 interface_count)
{
  TRACEPRINT_NOCHAN("count = %lu\n", (unsigned long)interface_count);
#ifndef DISABLE_MALLOC
  *pCtrlIntf = SIVOICE_CALLOC(sizeof(SiVoiceControlInterfaceType),
                              interface_count);
  if(*pCtrlIntf == NULL)
  {
#ifdef ENABLE_DEBUG
    LOGPRINT("%s%s: failed to allocate memory", LOGPRINT_PREFIX, __FUNCTION__);
#endif
    return RC_NO_MEM;
  }

  return RC_NONE;
#else
  SILABS_UNREFERENCED_PARAMETER(pCtrlIntf);
  SILABS_UNREFERENCED_PARAMETER(interface_count);
  return RC_UNSUPPORTED_FEATURE;
#endif
}

int SiVoice_destroyControlInterfaces ( SiVoiceControlInterfaceType **pCtrlIntf )
{

  TRACEPRINT_NOCHAN("\n",NULL);
#ifndef DISABLE_MALLOC
  if( pCtrlIntf && *pCtrlIntf)
  {
    SIVOICE_FREE ((SiVoiceControlInterfaceType *)*pCtrlIntf);
    *pCtrlIntf = NULL;
  }
  return RC_NONE;
#else
  SILABS_UNREFERENCED_PARAMETER(pCtrlIntf);
  return RC_UNSUPPORTED_FEATURE;
#endif
}

/*
** Device object constructor/destructor
*/
int SiVoice_createDevices (SiVoiceDeviceType **pDevice, uInt32 device_count)
{

  TRACEPRINT_NOCHAN("\n",NULL);
#ifndef DISABLE_MALLOC
  *pDevice = SIVOICE_CALLOC (sizeof(SiVoiceDeviceType), device_count);

  if(*pDevice == NULL)
  {
#ifdef ENABLE_DEBUG
    LOGPRINT("%s%s: failed to allocate memory", LOGPRINT_PREFIX, __FUNCTION__);
#endif
    return RC_NO_MEM;
  }
  return RC_NONE;
#else
  SILABS_UNREFERENCED_PARAMETER(pDevice);
  SILABS_UNREFERENCED_PARAMETER(device_count);
  return RC_UNSUPPORTED_FEATURE;
#endif
}

int SiVoice_destroyDevices (SiVoiceDeviceType **pDevice)
{

  TRACEPRINT_NOCHAN("\n", NULL);
#ifndef DISABLE_MALLOC
  if(pDevice && *pDevice)
  {
    SIVOICE_FREE ((SiVoiceDeviceType *)*pDevice);
    *pDevice = NULL;
  }
  return RC_NONE;
#else
  SILABS_UNREFERENCED_PARAMETER(pDevice);
  return RC_UNSUPPORTED_FEATURE;
#endif
}

/*
** Channel object constructor/destructor
*/
int SiVoice_createChannels (SiVoiceChanType_ptr *pChan, uInt32 channel_count)
{

  TRACEPRINT_NOCHAN("count = %lu\n", (unsigned long) channel_count);
#ifndef DISABLE_MALLOC
  *pChan = SIVOICE_CALLOC(sizeof(SiVoiceChanType),channel_count);
  if(*pChan == NULL)
  {
#ifdef ENABLE_DEBUG
    LOGPRINT("%s%s: failed to allocate memory", LOGPRINT_PREFIX, __FUNCTION__);
#endif
    return RC_NO_MEM;
  }
  return RC_NONE;
#else
  SILABS_UNREFERENCED_PARAMETER(pChan);
  SILABS_UNREFERENCED_PARAMETER(channel_count);
  return RC_UNSUPPORTED_FEATURE;
#endif
}

int SiVoice_destroyChannels (SiVoiceChanType_ptr *hProslic)
{

  TRACEPRINT_NOCHAN("\n",NULL);
#ifndef DISABLE_MALLOC
  if(hProslic && *hProslic)
  {
    SIVOICE_FREE ((SiVoiceChanType_ptr)*hProslic);
    *hProslic = NULL;
  }
  return RC_NONE;
#else
  SILABS_UNREFERENCED_PARAMETER(hProslic);
  return RC_UNSUPPORTED_FEATURE;
#endif
}

/*
** Host control linkage
*/
int SiVoice_setControlInterfaceCtrlObj (SiVoiceControlInterfaceType *pCtrlIntf,
                                        void *hCtrl)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->hCtrl = hCtrl;
  return RC_NONE;
}

/*
** Host reset linkage
*/
int SiVoice_setControlInterfaceReset (SiVoiceControlInterfaceType *pCtrlIntf,
                                      ctrl_Reset_fptr Reset_fptr)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->Reset_fptr = Reset_fptr;
  return RC_NONE;
}

/*
** Host register/RAM read/write linkage
*/
int SiVoice_setControlInterfaceWriteRegister (SiVoiceControlInterfaceType
    *pCtrlIntf, ctrl_WriteRegister_fptr WriteRegister_fptr)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->WriteRegister_fptr = WriteRegister_fptr;
  return RC_NONE;
}

int SiVoice_setControlInterfaceReadRegister (SiVoiceControlInterfaceType
    *pCtrlIntf, ctrl_ReadRegister_fptr ReadRegister_fptr)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->ReadRegister_fptr = ReadRegister_fptr;
  return RC_NONE;
}

int SiVoice_setControlInterfaceWriteRAM (SiVoiceControlInterfaceType *pCtrlIntf,
    ctrl_WriteRAM_fptr WriteRAM_fptr)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->WriteRAM_fptr = WriteRAM_fptr;
  return RC_NONE;
}

int SiVoice_setControlInterfaceReadRAM (SiVoiceControlInterfaceType *pCtrlIntf,
                                        ctrl_ReadRAM_fptr ReadRAM_fptr)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->ReadRAM_fptr = ReadRAM_fptr;
  return RC_NONE;
}

/*
** Host timer linkage
*/
int SiVoice_setControlInterfaceTimerObj (SiVoiceControlInterfaceType *pCtrlIntf,
    void *hTimer)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->hTimer = hTimer;
  return RC_NONE;
}

int SiVoice_setControlInterfaceDelay (SiVoiceControlInterfaceType *pCtrlIntf,
                                      system_delay_fptr Delay_fptr)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->Delay_fptr = Delay_fptr;
  return RC_NONE;
}

int SiVoice_setControlInterfaceSemaphore (SiVoiceControlInterfaceType
    *pCtrlIntf, ctrl_Semaphore_fptr semaphore_fptr)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->Semaphore_fptr = semaphore_fptr;
  return RC_NONE;
}

int SiVoice_setControlInterfaceTimeElapsed (SiVoiceControlInterfaceType
    *pCtrlIntf, system_timeElapsed_fptr timeElapsed_fptr)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->timeElapsed_fptr = timeElapsed_fptr;
  return RC_NONE;
}

int SiVoice_setControlInterfaceGetTime (SiVoiceControlInterfaceType *pCtrlIntf,
                                        system_getTime_fptr getTime_fptr)
{
  TRACEPRINT_NOCHAN("\n",NULL);
  pCtrlIntf->getTime_fptr = getTime_fptr;
  return RC_NONE;
}

/*
** Channel object initialization
*/
int SiVoice_SWInitChan (SiVoiceChanType_ptr pChan,int channel,int chipType,
                        SiVoiceDeviceType *pDeviceObj, SiVoiceControlInterfaceType *pCtrlIntf)
{
  TRACEPRINT_NOCHAN( "channel = %d chipType = %d\n", channel, chipType);
  pChan->channel = (uInt8)channel;
  pChan->deviceId = pDeviceObj;
  pChan->deviceId->ctrlInterface = pCtrlIntf;
  pChan->channelEnable=1;
  pChan->error = RC_NONE;
  pChan->debugMode = 0;
  pChan->dcdc_polarity_invert = 0;
#ifdef PROSLIC_BOM_DEFAULT
  pChan->bomOption = PROSLIC_BOM_DEFAULT;
#else
  pChan->bomOption = 0;
#endif

  switch (chipType)
  {
    case SI3217X_TYPE:
      pChan->deviceId->chipType = SI32171;
      break;

    case SI3218X_TYPE:
      pChan->deviceId->chipType = SI32180;
      break;

    case SI3219X_TYPE:
      pChan->deviceId->chipType = SI32190;
      break;

    case SI3226X_TYPE:
      pChan->deviceId->chipType = SI32260;
      break;

    case SI3228X_TYPE:
      pChan->deviceId->chipType = SI32280;
      break;

    case SI3050_TYPE:
      pChan->deviceId->chipType = SI3050;
      break;

    default:
      return RC_UNSUPPORTED_FEATURE;
  }
  return RC_NONE;
}

/*
** Reset control
*/
int SiVoice_Reset (SiVoiceChanType_ptr pChan)
{
  TRACEPRINT_NOCHAN( "\n", NULL);
  /*
  ** assert reset, wait 250ms, release reset, wait 250ms
  */
  pChan->deviceId->ctrlInterface->Reset_fptr(
    pChan->deviceId->ctrlInterface->hCtrl,1);

  pChan->deviceId->ctrlInterface->Delay_fptr(
    pChan->deviceId->ctrlInterface->hTimer,250);

  pChan->deviceId->ctrlInterface->Reset_fptr(
    pChan->deviceId->ctrlInterface->hCtrl,0);

  pChan->deviceId->ctrlInterface->Delay_fptr(
    pChan->deviceId->ctrlInterface->hTimer,250);

  return RC_NONE;
}

/*
** Debug Mode Control
*/
int SiVoice_setSWDebugMode (SiVoiceChanType_ptr pChan, int debugEn)
{
#ifdef ENABLE_DEBUG
  TRACEPRINT_NOCHAN( "debugEn %d\n", debugEn);
  pChan->debugMode = (0xFE & pChan->debugMode) | (debugEn != 0);
#else
  SILABS_UNREFERENCED_PARAMETER(pChan);
  SILABS_UNREFERENCED_PARAMETER(debugEn);
#endif
  return RC_NONE;
}

/*
** Trace Mode Control
*/
int SiVoice_setTraceMode (SiVoiceChanType_ptr pChan, int traceEn)
{
#ifdef ENABLE_TRACES
  TRACEPRINT_NOCHAN( "debugEn %d\n", traceEn);
  pChan->debugMode = (0xFD & pChan->debugMode) | ((traceEn != 0)<<1);
#else
  SILABS_UNREFERENCED_PARAMETER(pChan);
  SILABS_UNREFERENCED_PARAMETER(traceEn);
#endif
  return RC_NONE;
}

/*
** Error status
*/
int SiVoice_getErrorFlag (SiVoiceChanType_ptr pChan, int *error)
{
  TRACEPRINT( pChan, "\n", NULL);
  *error = pChan->error;
  return RC_NONE;
}

int SiVoice_clearErrorFlag (SiVoiceChanType_ptr pChan)
{
  TRACEPRINT( pChan, "\n", NULL);
  pChan->error = RC_NONE;
  return RC_NONE;
}

/*
** Channel status
*/
int SiVoice_setChannelEnable (SiVoiceChanType_ptr pChan, int chanEn)
{
  TRACEPRINT( pChan, "enable = %d\n", chanEn);
  pChan->channelEnable = chanEn;
  return RC_NONE;
}

int SiVoice_getChannelEnable (SiVoiceChanType_ptr pChan, int *chanEn)
{
  TRACEPRINT( pChan, "\n", NULL);
  *chanEn = pChan->channelEnable;
  return RC_NONE;
}

uInt8 SiVoice_ReadReg(SiVoiceChanType_ptr hProslic,uInt8 addr)
{
  TRACEPRINT( hProslic, "addr = %u\n", (unsigned int)addr);
  return ReadReg(hProslic, hProslic->channel, addr);
}

int SiVoice_WriteReg(SiVoiceChanType_ptr hProslic,uInt8 addr,uInt8 data)
{
  TRACEPRINT( hProslic, "addr = %u data = 0x%02X\n", (unsigned int)addr,
              (unsigned int) data);
  return WriteReg(hProslic, hProslic->channel, addr, data);
}

/*****************************************************************************************************/
#if defined(SI3217X) || defined(SI3218X) || defined(SI3219X) || defined(SI3226X) || defined(SI3228X) 
#include "../inc/proslic.h"

/* Iterate through the number of channels to determine if it's a SLIC, DAA, or unknown. Rev ID and chiptype is
 * also filled in.
 */
int SiVoice_IdentifyChannels(SiVoiceChanType_ptr *pProslic, int size,
                             int *slicCount, int *daaCount)
{
  int i;
  int rc = RC_NONE;
  SiVoiceChanType_ptr currentChannel;

  TRACEPRINT( *pProslic, "size = %d\n", size);

  if(slicCount)
  {
    *slicCount = 0;
  }
  if(daaCount)
  {
    *daaCount = 0;
  }

  for(i = 0; i < size; i++)
  {
    currentChannel = pProslic[i];
    /* SiVoice_SWInitChan() fills in the chipType initially with something the user provided, fill it
     * in with the correct info.. The GetChipInfo may probe registers to compare them with their
     * initial values, so this function MUST only be called after a chipset reset.
     */
#ifdef SI3217X
    if (currentChannel->deviceId->chipType >= SI32171
        && currentChannel->deviceId->chipType <= SI32179)
    {
      rc = Si3217x_GetChipInfo(currentChannel);
    }
#endif

#ifdef SI3218X
    if (currentChannel->deviceId->chipType >= SI32180
        && currentChannel->deviceId->chipType <= SI32189)
    {
      rc = Si3218x_GetChipInfo(currentChannel);
    }
#endif

#ifdef SI3219X
    if ( IS_SI3219X(currentChannel->deviceId) )
    {
      rc = Si3219x_GetChipInfo(currentChannel);
    }
#endif

#ifdef SI3226X
    if (currentChannel->deviceId->chipType >= SI32260
        && currentChannel->deviceId->chipType <= SI32269)
    {
      rc = Si3226x_GetChipInfo(currentChannel);
    }
#endif

#ifdef SI3228X
    if (currentChannel->deviceId->chipType >= SI32280
        && currentChannel->deviceId->chipType <= SI32289)
    {
      rc = Si3228x_GetChipInfo(currentChannel);
    }
#endif

    if(rc != RC_NONE)
    {
      return rc;
    }

    currentChannel->channelType = ProSLIC_identifyChannelType(currentChannel);
    if(currentChannel->channelType == PROSLIC)
    {
      if(slicCount)
      {
        (*slicCount)++;
      }
    }
    else if(currentChannel->channelType == DAA)
    {
      if(daaCount)
      {
        (*daaCount)++;
      }
    }
#ifdef ENABLE_DEBUG
    {
      const char *dev_type = "UNKNOWN";
      if(currentChannel->channelType == PROSLIC)
      {
        dev_type = "PROSLIC";
      }
      else if(currentChannel->channelType == DAA)
      {
        dev_type = "DAA";
      }
      LOGPRINT("%sChannel %d: Type = %s Rev = %d\n",
               LOGPRINT_PREFIX, currentChannel->channel, dev_type,
               currentChannel->deviceId->chipRev);

    }
#endif /* ENABLE_DEBUG */
  }
  return RC_NONE;
}
#endif

/*
** Function: ProSLIC_Version
**
** Description:
** Return API version
**
** Returns:
** string of the API release.
*/

extern const char *SiVoiceAPIVersion;
char *SiVoice_Version()
{
  return (char *)SiVoiceAPIVersion;
}

