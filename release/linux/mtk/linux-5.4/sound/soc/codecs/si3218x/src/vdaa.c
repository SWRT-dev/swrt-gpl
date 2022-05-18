// SPDX-License-Identifier: GPL-2.0
/*
 * Vdaa  VoiceDAA interface implementation file
 *
 * Distributed by:
 * Silicon Laboratories, Inc
 *
 * This file contains proprietary information.
 * No dissemination allowed without prior written permission from
 * Silicon Laboratories, Inc.
 *
 * File Description:
 * This is the implementation file for the main VoiceDAA API.
 *
 */

#include "../config_inc/si_voice_datatypes.h"
#include "../inc/si_voice_ctrl.h"
#include "../inc/si_voice_timer_intf.h"
#include "../inc/vdaa.h"
#include "../inc/vdaa_registers.h"
#include "../config_inc/vdaa_api_config.h"

#define WriteReg      pVdaa->deviceId->ctrlInterface->WriteRegister_fptr
#define ReadReg        pVdaa->deviceId->ctrlInterface->ReadRegister_fptr
#define pVdaaHW        pVdaa->deviceId->ctrlInterface->hCtrl

#define WriteRegX      deviceId->ctrlInterface->WriteRegister_fptr
#define ReadRegX      deviceId->ctrlInterface->ReadRegister_fptr
#define pVdaaHWX      deviceId->ctrlInterface->hCtrl
#define LOGPRINT_PREFIX "VDAA: "

/*
** Static VDAA driver functions
*/

/*
** Function: isVerifiedDAA
**
** Description:
** Verifies addressed channel is DAA
**
** Input Parameters:
** pVdaa: pointer to SiVoiceChanType or vdaaChanType
**
** Return:
** Verified DAA
** Not DAA          RC_CHANNEL_TYPE_ERR
**
*/

static int isVerifiedDAA(vdaaChanType *pVdaa)
{
  uInt8 data = ReadReg(pVdaaHW,pVdaa->channel,LSIDE_REV);
  if ( (data & 0x40) == 0 ) /*This bit is always 1 for DAA*/
  {
    LOGPRINT("%sDAA device not detected\n",LOGPRINT_PREFIX);
    return RC_CHANNEL_TYPE_ERR;
  }
  else
  {
    /* For Si3050, the value will be zero initially (calloc), for Si32178/9, this will
     * be non-zero (Rev B).
     */
    if(pVdaa->deviceId->chipRev == 0)
    {
      /* Read Device ID and store it */
      /* NOTE: in earlier releases we also read the line side info here.  This is now done
         Vdaa_duringPowerUpLineside since the information we read was always 0.
       */
      data = pVdaa->ReadRegX(pVdaa->pVdaaHWX,pVdaa->channel,SYS_LINE_DEV_REV);
      pVdaa->deviceId->chipRev= data&0xF;
#ifdef ENABLE_DEBUG
      LOGPRINT("%sChipRev = 0x%x\n", LOGPRINT_PREFIX, pVdaa->deviceId->chipRev);
#endif
    }
  }

  return RC_NONE;
}

#if 0 /* Removed for now since it isn't used, keeping code as reference */
/*
** Function: probeDaisyChain
**
** Description:
** Identify how many VDAA devices are in daisychain
** Only called if channel 0 has be previously qualified
** as a VDAA.
**
** Input Parameters:
** pVdaa:       pointer to SiVoiceChanType or vdaaChanType
**
** Return:
** number of channels in daisy chain
**
*/

static int probeDaisyChain (vdaaChanType *pVdaa)
{
  int i=0;
  WriteReg(pVdaaHW,BROADCAST,PCMRX_CNT_LO,0x23);        /* Broadcast */
  while ((ReadReg(pVdaaHW,(uInt8)i++,PCMRX_CNT_LO) == 0x23)
         &&(i<=16));  /* Count number of channels */
  return i-1;                          /* Return number of channels */
}
#endif

/*
**
**  ------ VDAA CONFIGURATION FUNCTIONS -----
**
*/

/*
** Function:  Vdaa_RingDetectSetup
**
** Description:
** configure ring detect setup
**
** Returns:
**
*/

#ifndef DISABLE_VDAA_RING_DETECT_SETUP
extern vdaa_Ring_Detect_Cfg Vdaa_Ring_Detect_Presets[];
int Vdaa_RingDetectSetup (vdaaChanType *pVdaa,int32 preset)
{

  uInt8 regTemp = 0;
  TRACEPRINT( pVdaa, "preset = %d\n", (unsigned int) preset);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,CTRL2) & 0xfB;
  regTemp |= Vdaa_Ring_Detect_Presets[preset].rdi<<2;
  WriteReg(pVdaaHW,pVdaa->channel,CTRL2,regTemp);

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,INTL_CTRL1) & 0xfe;
  regTemp |= Vdaa_Ring_Detect_Presets[preset].rt&1;
  WriteReg(pVdaaHW,pVdaa->channel,INTL_CTRL1,regTemp);

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,INTL_CTRL2) & 0xef;
  regTemp |= ((Vdaa_Ring_Detect_Presets[preset].rt>>1)<<4);
  WriteReg(pVdaaHW,pVdaa->channel,INTL_CTRL2,regTemp);

  WriteReg(pVdaaHW,pVdaa->channel,INTL_CTRL3,
           Vdaa_Ring_Detect_Presets[preset].rfwe<<1);

  regTemp = (Vdaa_Ring_Detect_Presets[preset].rdly&0x3) << 6;
  regTemp |= Vdaa_Ring_Detect_Presets[preset].rmx ;
  WriteReg(pVdaaHW,pVdaa->channel,RNG_VLD_CTRL1,regTemp);

  regTemp = (Vdaa_Ring_Detect_Presets[preset].rdly>>2) << 7;
  regTemp |= Vdaa_Ring_Detect_Presets[preset].rto << 3 ;
  regTemp |= Vdaa_Ring_Detect_Presets[preset].rcc ;
  WriteReg(pVdaaHW,pVdaa->channel,RNG_VLD_CTRL2,regTemp);

  regTemp = Vdaa_Ring_Detect_Presets[preset].rngv << 7;
  regTemp |= Vdaa_Ring_Detect_Presets[preset].ras ;
  WriteReg(pVdaaHW,pVdaa->channel,RNG_VLD_CTRL3,regTemp);

  regTemp = Vdaa_Ring_Detect_Presets[preset].rpol<<1;
  WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL4,regTemp);

  return RC_NONE;
}
#endif

/*
** Function:  Vdaa_TXAudioGainSetup
**
** Description:
** configure tx audio gain
**
** Returns:
**
**
*/
#ifndef DISABLE_VDAA_AUDIO_GAIN_SETUP
extern vdaa_audioGain_Cfg Vdaa_audioGain_Presets[];

int Vdaa_TXAudioGainSetup (vdaaChanType *pVdaa,int32 preset)
{
  uInt8 regTemp = 0;
  TRACEPRINT( pVdaa, "preset = %d\n", (unsigned int) preset);

  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  if(Vdaa_audioGain_Presets[preset].xga2 == XGA_ATTEN)
  {
    regTemp = 0x10;
  }

  regTemp |= Vdaa_audioGain_Presets[preset].acgain2 ;
  WriteReg(pVdaaHW,pVdaa->channel,TX_GN_CTRL2,regTemp);

  regTemp = 0;
  if(Vdaa_audioGain_Presets[preset].xga3 == XGA_ATTEN)
  {
    regTemp = 0x10 ;
  }

  regTemp |= Vdaa_audioGain_Presets[preset].acgain3 ;
  WriteReg(pVdaaHW,pVdaa->channel,TX_GN_CTRL3,regTemp);

  if(Vdaa_audioGain_Presets[preset].cpEn)
  {
    WriteReg(pVdaaHW,pVdaa->channel,TXCALL_PROG_ATTEN,
             Vdaa_audioGain_Presets[preset].callProgress);
  }

  return RC_NONE;
}
# endif


/*
** Function:  Vdaa_RXAudioGainSetup
**
** Description:
** configure rx audio gain
**
** Returns:
**
**
*/
#ifndef DISABLE_VDAA_AUDIO_GAIN_SETUP
extern vdaa_audioGain_Cfg Vdaa_audioGain_Presets[];

int Vdaa_RXAudioGainSetup (vdaaChanType *pVdaa,int32 preset)
{
  uInt8 regTemp = 0;

  TRACEPRINT( pVdaa, "preset = %d\n", (unsigned int) preset);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  if(Vdaa_audioGain_Presets[preset].xga2 == XGA_ATTEN)
  {
    regTemp = 0x10;
  }
  regTemp |= Vdaa_audioGain_Presets[preset].acgain2 ;
  WriteReg(pVdaaHW,pVdaa->channel,RX_GN_CTRL2,regTemp);

  regTemp = 0;
  if(Vdaa_audioGain_Presets[preset].xga3 == XGA_ATTEN)
  {
    regTemp = 0x10;
  }
  regTemp |= Vdaa_audioGain_Presets[preset].acgain3 ;
  WriteReg(pVdaaHW,pVdaa->channel,RX_GN_CTRL3,regTemp);

  if(Vdaa_audioGain_Presets[preset].cpEn)
  {
    WriteReg(pVdaaHW,pVdaa->channel,RXCALL_PROG_ATTEN,
             Vdaa_audioGain_Presets[preset].callProgress);
  }

  return RC_NONE;
}
#endif


/*
** Function:  Vdaa_PCMSetup
**
** Description:
** configure pcm format, clocking and edge placement
**
** Returns:
**
**
*/
#ifndef DISABLE_VDAA_PCM_SETUP
extern vdaa_PCM_Cfg Vdaa_PCM_Presets [];

int Vdaa_PCMSetup (vdaaChanType *pVdaa,int32 preset)
{
  uInt8 regTemp = 0;

  TRACEPRINT( pVdaa, "preset = %d\n", (unsigned int) preset);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL)&0xe0;
  regTemp |= Vdaa_PCM_Presets[preset].pcm_tri;
  regTemp |= Vdaa_PCM_Presets[preset].pcmHwy << 1;
  regTemp |= Vdaa_PCM_Presets[preset].pcmFormat << 3;
  WriteReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL,regTemp);

  return RC_NONE;
}
#endif

/*
** Function:  Vdaa_PCMTimeSlotSetup
**
** Description:
** configure pcm timeslot
**
** Returns:
**
*/
int Vdaa_PCMTimeSlotSetup (vdaaChanType *pVdaa, uInt16 rxcount, uInt16 txcount)
{
  uInt8 data = 0;
  uInt8 pcmStatus;

  TRACEPRINT( pVdaa, "rxcount = %u txcount = %u\n", (unsigned int)rxcount,
              (unsigned int)txcount);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  /* Disable PCM if enabled - restore after updating timeslots */
  pcmStatus = ReadReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL);
  if (pcmStatus&0x20)
  {
    WriteReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL,pcmStatus&~(0x20));
  }

  /*Storing 10 bit value of Transmit PCM sample in REG 34 and REG 35[0:1]*/
  data = (uInt8)(txcount & 0xff);
  WriteReg(pVdaaHW,pVdaa->channel,PCMTX_CNT_LO,data);
  data = (uInt8)(txcount >> 8) ;
  WriteReg(pVdaaHW,pVdaa->channel,PCMTX_CNT_HI,data);

  /*Storing 10 bit value of Receive PCM sample in REG 34 and REG 35[0:1]*/
  data = (uInt8)(rxcount & 0xff);
  WriteReg(pVdaaHW,pVdaa->channel,PCMRX_CNT_LO,data);
  data = (uInt8)(rxcount >> 8);
  WriteReg(pVdaaHW,pVdaa->channel,PCMRX_CNT_HI,data);

  /* Enable back the PCM after storing the values*/
  WriteReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL,pcmStatus);

  return RC_NONE;
}

/*
** Function:  Vdaa_CountrySetup
**
** Description:
** configure country specific settings
**
** Returns:
**
**
*/

#ifndef DISABLE_VDAA_COUNTRY_SETUP
extern vdaa_Country_Cfg Vdaa_Country_Presets [];

int Vdaa_CountrySetup (vdaaChanType *pVdaa,int32 preset)
{
  uInt8 regTemp = 0;

  TRACEPRINT( pVdaa, "preset = %d\n", (unsigned int) preset);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,CTRL2) & 0xFD;
  WriteReg(pVdaaHW,pVdaa->channel,CTRL2,regTemp); /* disable hybrid */

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,INTL_CTRL1) & 0xFD;
  regTemp |= Vdaa_Country_Presets[preset].rz << 1 ;
  WriteReg(pVdaaHW,pVdaa->channel,INTL_CTRL1,regTemp);

  regTemp = Vdaa_Country_Presets[preset].dcr;
  regTemp |= Vdaa_Country_Presets[preset].ilim<<1;
  regTemp |= Vdaa_Country_Presets[preset].mini<<4;
  regTemp |= Vdaa_Country_Presets[preset].dcv<<6;
  WriteReg(pVdaaHW,pVdaa->channel,DC_TERM_CTRL,regTemp);

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,AC_TERM_CTRL) & 0xF0;
  regTemp |= Vdaa_Country_Presets[preset].acim;
  WriteReg(pVdaaHW,pVdaa->channel,AC_TERM_CTRL,regTemp);

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,SPRK_QNCH_CTRL) & 0xAF;
  regTemp |= ((Vdaa_Country_Presets[preset].ohs_sq >> 2)&1)<<4 ;
  regTemp |= ((Vdaa_Country_Presets[preset].ohs_sq >> 3)&1)<<6 ;
  WriteReg(pVdaaHW,pVdaa->channel,SPRK_QNCH_CTRL,regTemp);

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,INTL_CTRL1) & 0xBF;
  regTemp |= ((Vdaa_Country_Presets[preset].ohs_sq >> 1)&1)<<6 ;
  WriteReg(pVdaaHW,pVdaa->channel,INTL_CTRL1,regTemp);

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL5) & 0xE7;
  regTemp |= (Vdaa_Country_Presets[preset].ohs_sq&1)<<3 ;
  WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL5,regTemp);

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,CTRL2) & 0xFD;
  regTemp |= (Vdaa_Country_Presets[preset].hbe)<<1 ;
  WriteReg(pVdaaHW,pVdaa->channel,CTRL2,regTemp);

  return RC_NONE;
}
#endif

/*
** Function:  Vdaa_HybridSetup
**
** Description:
** configure hybrid
**
*/

#ifndef DISABLE_VDAA_HYBRID_SETUP
extern vdaa_Hybrid_Cfg Vdaa_Hybrid_Presets [];

int Vdaa_HybridSetup (vdaaChanType *pVdaa,int32 preset)
{
  uInt8 regSave = 0;

  TRACEPRINT( pVdaa, "preset = %d\n", (unsigned int) preset);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  regSave = ReadReg(pVdaaHW,pVdaa->channel,CTRL2);
  WriteReg(pVdaaHW,pVdaa->channel,CTRL2,regSave&0xFD); /* disable hybrid */

  WriteReg(pVdaaHW,pVdaa->channel,HYB1,Vdaa_Hybrid_Presets[preset].hyb1);
  WriteReg(pVdaaHW,pVdaa->channel,HYB2,Vdaa_Hybrid_Presets[preset].hyb2);
  WriteReg(pVdaaHW,pVdaa->channel,HYB3,Vdaa_Hybrid_Presets[preset].hyb3);
  WriteReg(pVdaaHW,pVdaa->channel,HYB4,Vdaa_Hybrid_Presets[preset].hyb4);
  WriteReg(pVdaaHW,pVdaa->channel,HYB5,Vdaa_Hybrid_Presets[preset].hyb5);
  WriteReg(pVdaaHW,pVdaa->channel,HYB6,Vdaa_Hybrid_Presets[preset].hyb6);
  WriteReg(pVdaaHW,pVdaa->channel,HYB7,Vdaa_Hybrid_Presets[preset].hyb7);
  WriteReg(pVdaaHW,pVdaa->channel,HYB8,Vdaa_Hybrid_Presets[preset].hyb8);

  WriteReg(pVdaaHW,pVdaa->channel,CTRL2,
           regSave); /* Restore hybrid enable state at entry */

  return RC_NONE;
}
#endif

/*
** Function:  Vdaa_SetAudioMute
**
** Description:
** Control RX and TX mute
**
*/

int Vdaa_SetAudioMute(vdaaChanType *pVdaa, tMUTE mute)
{
  uInt8 regData;

  TRACEPRINT( pVdaa, "mode = %u\n", (unsigned int) mute);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  regData = ReadReg(pVdaaHW,pVdaa->channel,TXRX_GN_CTRL);

  switch(mute)
  {
    case MUTE_DISABLE_ALL:
      regData = 0;
      break;

    case MUTE_DISABLE_RX:
      regData &= 0x80;
      break;

    case MUTE_DISABLE_TX:
      regData &= 0x08;
      break;

    case MUTE_ENABLE_RX:
      regData |= 0x08;
      break;

    case MUTE_ENABLE_TX:
      regData |= 0x80;
      break;

    case MUTE_ENABLE_ALL:
      regData = 0x88;
      break;
  }

  WriteReg(pVdaaHW,pVdaa->channel,TXRX_GN_CTRL,regData);

  return RC_NONE;
}

/*
** Function:  Vdaa_PCMStart
**
** Description:
** Enables PCM bus
**
*/

int Vdaa_PCMStart (vdaaChanType *pVdaa)
{
  uInt8 data = 0;

  TRACEPRINT( pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  /*Enable PCM transfers by setting REG 33[5]=1 */
  data = ReadReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL);
  data |= 0x20;
  WriteReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL,data);
  return RC_NONE;
}

/*
** Function:  Vdaa_PCMStop
**
** Description:
** Disables PCM bus
**
*/

int Vdaa_PCMStop (vdaaChanType *pVdaa)
{
  uInt8 data = 0;

  TRACEPRINT( pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  /*disable PCM transfers by setting REG 33[5]=0 */
  data = ReadReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL);
  data &= ~(0x20);
  WriteReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL,data);
  return RC_NONE;
}

/*
** Function:  Vdaa_SetInterruptMask
**
** Description:
** Enables interrupts based on passed 9-bit bitmask.  Bit
** values defined by vdaaIntMask enum.
**
*/

int Vdaa_SetInterruptMask(vdaaChanType *pVdaa, vdaaIntMask bitmask)
{
  uInt8 intMaskReg = 0;
  uInt8 cviReg = 0;

  TRACEPRINT( pVdaa, "mask = 0x%03x\n", (unsigned int)bitmask);
  /* Channel validation */
  if(pVdaa->channelType != DAA)
  {
    return RC_CHANNEL_TYPE_ERR;
  }

  intMaskReg = (uInt8)(bitmask & 0x00ff);
  cviReg = ReadReg(pVdaaHW,pVdaa->channel,LN_VI_THRESH_INTE_CTRL);
  cviReg |= (uInt8) ((bitmask >> 7) & 0x0002);

  WriteReg (pVdaaHW,pVdaa->channel,INTE_MSK,intMaskReg);
  WriteReg(pVdaaHW,pVdaa->channel,LN_VI_THRESH_INTE_CTRL,cviReg);

  return RC_NONE;
}

/*
** Function:  Vdaa_ReadRingDetectStatus
**
** Description:
** Reads ring detect/hook status
**
** Returns:
**
**
*/

int Vdaa_ReadRingDetectStatus (vdaaChanType *pVdaa,
                               vdaaRingDetectStatusType *pStatus)
{
  uInt8 reg;

  TRACEPRINT( pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  reg = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL1);
  pStatus->offhook = reg & 0x01;
  pStatus->ringDetected = (reg & 0x4)>>2;
  pStatus->onhookLineMonitor = (reg & 0x8)>>3;
  pStatus->ringDetectedPos = (reg & 0x20)>>5;
  pStatus->ringDetectedNeg = (reg & 0x40)>>6;
  return RC_NONE;
}

/*
** Function:  Vdaa_Init
**
** Description:
** Initialize VDAA, load general config parameters
**
*/

extern vdaa_General_Cfg Vdaa_General_Configuration;

int Vdaa_Init (vdaaChanType_ptr *pVdaa,int size)
{
  uInt8 data;
  int k;
  int num_devices = 0;

  TRACEPRINT( *pVdaa, "size = %d\n", size);

  for (k=0; k<size; k++)
  {

    if(pVdaa[k]->channelType == PROSLIC)
    {
      continue;  /* Skip if we know this is a ProSLIC, else check the device out */
    }

    if (isVerifiedDAA(pVdaa[k]) == RC_CHANNEL_TYPE_ERR)
    {
      pVdaa[k]->channelEnable = FALSE;
      pVdaa[k]->error = RC_CHANNEL_TYPE_ERR;
      pVdaa[k]->channelType = UNKNOWN;
      DEBUG_PRINT(pVdaa[k], "%sVDAA not supported on this device\n",LOGPRINT_PREFIX);
      continue;
    }
    else
    {
      pVdaa[k]->channelType = DAA;
    }

    if (pVdaa[k]->channelEnable)
    {
      /*Try to write innocuous register to test SPI is working*/
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,PCMRX_CNT_LO,0x5A);
      data = pVdaa[k]->ReadRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,PCMRX_CNT_LO);
      if (data != 0x5A)
      {
        pVdaa[k]->error = RC_SPI_FAIL;
        pVdaa[k]->channelEnable = FALSE;
        DEBUG_PRINT(pVdaa[k], "%sVDAA %d not communicating\n",LOGPRINT_PREFIX,
                    pVdaa[k]->channel);
      }
      else
      {
        num_devices++;
      }
    }
  }

  if(num_devices == 0)
  {
    DEBUG_PRINT(*pVdaa, "%sNo DAA devices detected\n", LOGPRINT_PREFIX);
    return RC_SPI_FAIL;
  }

  for (k=0; k<size; k++)
  {
    if(pVdaa[k]->channelType != DAA)
    {
      continue;  /* Skip PROSLIC or UNDEFINED ports */
    }
    if (pVdaa[k]->channelEnable)
    {

      /* Apply General Configuration parameters */

      /* No need to read-modify-write here since registers are in their reset state */
      data = (Vdaa_General_Configuration.pwmm << 4) |
             (Vdaa_General_Configuration.pwmEnable << 3);
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,CTRL1, data);

      data = (Vdaa_General_Configuration.inte << 7) | (Vdaa_General_Configuration.intp
             << 6) | 0x03;
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,CTRL2, data);

      data = (Vdaa_General_Configuration.hssm << 3);
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,SMPL_CTRL, data);

      data = (Vdaa_General_Configuration.iire << 4);
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,INTL_CTRL1, data);

      data = (Vdaa_General_Configuration.rcald << 4);
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,RES_CALIB, data);

      data = (Vdaa_General_Configuration.full2 << 4);
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,AC_TERM_CTRL, data);

      data = (Vdaa_General_Configuration.lvfd) | (Vdaa_General_Configuration.filt <<
             1) |
             (Vdaa_General_Configuration.foh << 5) | (Vdaa_General_Configuration.full << 7);
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,DAA_CTRL5, data);

      data = (Vdaa_General_Configuration.spim << 6);
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,PCM_SPI_CTRL, data);

      data = (Vdaa_General_Configuration.cvp) | (Vdaa_General_Configuration.cvs << 2);
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,LN_VI_THRESH_INTE_CTRL,
                          data);

      data = (Vdaa_General_Configuration.gce << 1) | (Vdaa_General_Configuration.rg1
             << 2);
      pVdaa[k]->WriteRegX(pVdaa[k]->pVdaaHWX,pVdaa[k]->channel,SPRK_QNCH_CTRL, data);

      /* Enable Lineside Device */
      Vdaa_PowerupLineside(pVdaa[k]);
    }
  }
  DEBUG_PRINT(*pVdaa,"%sDAA initialization completed\n",LOGPRINT_PREFIX);
  return RC_NONE;
}

/*
** Function:  Vdaa_ReadLinefeedStatus
**
** Description:
** Read Status of Line Feed
**
** Returns:
** RC_VDAA_ILOOP_OVLD if LCS >= 0x1F
**      - no overload
**
*/

int Vdaa_ReadLinefeedStatus (vdaaChanType *pVdaa,int8 *vloop, int16 *iloop)
{
  int16 regTemp = 0x1F;
  uInt8 iloop_reg; /* REG 12[4:0] = Loop current*/

  TRACEPRINT( pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }
  regTemp &= ReadReg(pVdaaHW,pVdaa->channel,LSIDE_STAT);
  iloop_reg = (uInt8)regTemp;
  *iloop = (regTemp*LCS_SCALE_NUM) /
           LCS_SCALE_DEN; /* Multiply the read result by 3.3mA/bit*/

  *vloop = (int8) ReadReg(pVdaaHW,pVdaa->channel,LINE_VOLT_STAT);
  if(*vloop & 0x80)
  {
    *vloop = ~(*vloop - 1)*(-1);
  }

  if (iloop_reg == 0x1F)
  {
    return RC_VDAA_ILOOP_OVLD;
  }
  return RC_NONE;
}

/*
** Function:  Vdaa_GetInterrupts
**
** Description:
** Get Interrupts
**
** Returns:
** number of interrupts
**
*/

int Vdaa_GetInterrupts (vdaaChanType *pVdaa,vdaaIntType *pIntData)
{
  uInt8 data = 0;
  int j;
  TRACEPRINT( pVdaa, "\n", NULL);
  pIntData->number = 0;

  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  data = ReadReg(pVdaaHW,pVdaa->channel,INTE_SRC);      /*Snapshot Interrupts*/
  WriteReg(pVdaaHW,pVdaa->channel,INTE_SRC,~(data));      /*Clear interrupts*/

  for (j=0; j<8; j++)
  {
    if (data &(1<<j))
    {

      pIntData->irqs[pIntData->number] =   j;
      pIntData->number++;

    }
  }
  data = ReadReg(pVdaaHW,pVdaa->channel,LN_VI_THRESH_INTE_CTRL);

  if (data &(0x08))                   /*to determine if CVI Interrupt is set*/
  {
    pIntData->irqs[pIntData->number] =   CVI;
    pIntData->number++;
    data &= ~(0x08);
    WriteReg(pVdaaHW,pVdaa->channel,LN_VI_THRESH_INTE_CTRL,data);

  }

  return pIntData->number;
}

/*
** Function:  Vdaa_ClearInterrupts
**
** Description:
** Clear Interrupts
**
** Returns:
**
**
*/

int Vdaa_ClearInterrupts (vdaaChanType *pVdaa)
{
  uInt8 data = 0;

  TRACEPRINT( pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  WriteReg(pVdaaHW,pVdaa->channel,INTE_SRC,
           0x00);    /* Clear interrupts in REG 4 by writing 0's*/

  /*Clear CVI interrupt by writing 0 at REG 44[3]*/
  data = ReadReg(pVdaaHW,pVdaa->channel,LN_VI_THRESH_INTE_CTRL);
  WriteReg(pVdaaHW,pVdaa->channel,LN_VI_THRESH_INTE_CTRL,data&0xF7);

  return RC_NONE;
}

/*
** Function:  Vdaa_GetHookStatus
**
** Description:
** Read VDAA Hook Status
**
** Return Values -
**      VDAA_ONHOOK
**      VDAA_OFFHOOK
**      VDAA_ONHOOK_MONITOR
**      RC_INVALID_HOOK_STATUS
*/

uInt8 Vdaa_GetHookStatus (vdaaChanType *pVdaa)
{
  uInt8 data;

  TRACEPRINT( pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  data = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL1);
  data &= 0x09;  /* Look at only ONHM and OH */

  if((data & 0x80)&&(data & 0x01))
  {
    return VDAA_ONHOOK_MONITOR;
  }
  else if (data & 0x01)
  {
    return VDAA_OFFHOOK;
  }
  else
  {
    return VDAA_ONHOOK;
  }
}

/*
** Function:  Vdaa_SetHookStatus
**
** Description:
** Set VDAA Hook switch to ONHOOK, OFFHOOK,
** or ONHOOK_MONITOR
**
*/

int Vdaa_SetHookStatus (vdaaChanType *pVdaa,uInt8 newHookStatus)
{
  uInt8 data= 0 ;

  TRACEPRINT( pVdaa, "hookstate = %u\n", (unsigned int) newHookStatus);

  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  switch (newHookStatus)
  {
    case VDAA_DIG_LOOPBACK:
      /*Setting REG6[4]=1,REG5[0]=0,REG5[3]=0*/
      data = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL2);
      data |= 0x10;
      WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL2,data);
      data = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL1);
      data &= ~(0x09);
      WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL1,data);
      break;

    case VDAA_ONHOOK:
      /*Setting REG6[4]=0,REG5[0]=0,REG5[3]=0*/
      data = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL1);
      data &= 0xF6;
      WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL1,data);
      data = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL2);
      data &= 0xEF;
      WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL2,data);
      break;

    case VDAA_OFFHOOK:
      /*Setting REG6[4]=0,REG5[0]=1,REG5[3]=0*/
      data = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL1);
      data &= 0xF7;
      data |= 0x01;
      WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL1,data);
      data = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL2);
      data &= 0xEF;
      WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL2,data);
      break;

    case VDAA_ONHOOK_MONITOR:
      /*Setting REG6[4]=0,REG5[0]=0,REG5[3]=1*/
      data = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL1);
      data &= 0xFE;
      data |= 0x08;
      WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL1,data);
      data = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL2);
      data &= 0xEF;
      WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL2,data);
      break;

    default:
      return RC_UNSUPPORTED_OPTION;
  }

  return RC_NONE;
}

/*
** Function:  Vdaa_SetLoopbackMode
**
** Description:
** Loopback mode control
**
*/

int Vdaa_SetLoopbackMode(vdaaChanType_ptr pVdaa, tLpbkMode lpbk_mode,
                         tLpbkStatus lpbk_status)
{
  uInt8 regData;

  TRACEPRINT(pVdaa, "lpbk_mode = %u lpbk_status = %d\n", (unsigned int) lpbk_mode,
             (unsigned int) lpbk_status);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  switch(lpbk_mode)
  {
    case LPBK_NONE:
      /* Disable all loopback types, regardless of lpbk_status */
      regData = ReadReg(pVdaaHW,pVdaa->channel,CTRL1);
      if(regData & 0x02)
      {
        WriteReg(pVdaaHW,pVdaa->channel,CTRL1, regData & ~(0x02));
      }

      regData = ReadReg(pVdaaHW,pVdaa->channel,DAA_CTRL3);
      if(regData & 0x01)
      {
        WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL3,0);
      }

      regData = ReadReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL);
      if(regData & 0x80)
      {
        WriteReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL, regData & ~(0x80));
      }
      break;

    case LPBK_IDL:
      if(lpbk_status)
      {
        regData = ReadReg(pVdaaHW,pVdaa->channel,CTRL1);
        WriteReg(pVdaaHW,pVdaa->channel,CTRL1, regData | 0x02);
      }
      else
      {
        regData = ReadReg(pVdaaHW,pVdaa->channel,CTRL1);
        WriteReg(pVdaaHW,pVdaa->channel,CTRL1, regData & ~(0x02));
      }
      break;

    case LPBK_DDL:
      if(lpbk_status)
      {
        WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL3, 0x01);
      }
      else
      {
        WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL3, 0);
      }
      break;

    case LPBK_PCML:
      if(lpbk_status)
      {
        regData = ReadReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL);
        WriteReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL, regData | 0x80);
      }
      else
      {
        regData = ReadReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL);
        WriteReg(pVdaaHW,pVdaa->channel,PCM_SPI_CTRL, regData & ~(0x80));
      }
      break;

    default:
      return RC_UNSUPPORTED_OPTION;
  }

  return RC_NONE;
}

/*
** Function:  Vdaa_ADCCal
**
** Description:
** This function calibrates the VDAA ADC manually
**
*/

int Vdaa_ADCCal (vdaaChanType_ptr pVdaa, int32 size)
{
  uInt8 regTemp = 0;
  int32 i;

  TRACEPRINT(pVdaa, "size = %d\n", (int) size);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  for(i = 0; i < size; i++)
  {
    /* Clear the previous ADC Calibration data by toggling CALZ*/
    regTemp = ReadReg(pVdaaHW,pVdaa->channel,INTL_CTRL2);
    regTemp |= 0x80;
    WriteReg(pVdaaHW,pVdaa->channel,INTL_CTRL2,regTemp);
    regTemp &= ~0x80;
    WriteReg(pVdaaHW,pVdaa->channel,INTL_CTRL2,regTemp);

    regTemp = ReadReg(pVdaaHW,pVdaa->channel,INTL_CTRL2); /*disable auto cal*/
    regTemp |= 0x20;
    WriteReg(pVdaaHW,pVdaa->channel,INTL_CTRL2,regTemp);

    regTemp |= 0x40;                  /*initiate manual cal*/
    WriteReg(pVdaaHW,pVdaa->channel,INTL_CTRL2,regTemp);
    regTemp &= ~0x40;
    WriteReg(pVdaaHW,pVdaa->channel,INTL_CTRL2,regTemp);
    pVdaa++;
  }
  return RC_NONE;
}

/*
** Function:  Vdaa_EnableWatchdog
**
** Description:
** Enables watchdog timer
**
*/

int Vdaa_EnableWatchdog(vdaaChanType_ptr pVdaa)
{
  uInt8 regTemp;

  TRACEPRINT(pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,CTRL2);
  WriteReg(pVdaaHW,pVdaa->channel,CTRL2,regTemp | 0x10);

  return RC_NONE;
}

/*
** Function:  Vdaa_SetHybridEnable
**
** Description:
** Enables watchdog timer
**
*/

int Vdaa_SetHybridEnable(vdaaChanType_ptr pVdaa, int enable)
{
  uInt8 regTemp;
  TRACEPRINT(pVdaa, "enable = %d\n", enable);

  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  regTemp = ReadReg(pVdaaHW,pVdaa->channel,CTRL2);
  if(enable)
  {
    WriteReg(pVdaaHW,pVdaa->channel,CTRL2,regTemp | 0x02);
  }
  else
  {
    WriteReg(pVdaaHW,pVdaa->channel,CTRL2,regTemp & ~(0x02));
  }

  return RC_NONE;
}


/*
** Function:  Vdaa_SoftReset
**
** Description:
** Execute soft reset
**
*/

int Vdaa_SoftReset(vdaaChanType_ptr pVdaa)
{
  TRACEPRINT(pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  WriteReg(pVdaaHW,pVdaa->channel,CTRL1,0x80);
  return RC_NONE;
}

/*
** Function:  Vdaa_ReadFDTStatus
**
** Description:
** Read FDT bit
**
** Returns:
** 0 - Frame Not Detected
** 1 - Frame Detected
**
*/

int Vdaa_ReadFDTStatus(vdaaChanType_ptr pVdaa)
{
  TRACEPRINT(pVdaa, "\n", NULL);
  return (ReadReg(pVdaaHW,pVdaa->channel,LSIDE_REV) & 0x40);
}

/*
** Function:  Vdaa_PowerupLineside
**
** Description:
** Power up lineside device
**
*/

int Vdaa_PowerupLineside(vdaaChanType_ptr pVdaa)
{
  uInt8 data; 

  TRACEPRINT(pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL2,0); /* Powerup lineside device */
  /* We do a double read to give the front end time to power up and sync up with the system side.. */
  data = ReadReg(pVdaaHW,pVdaa->channel,LSIDE_REV);
  data = 0;
  data = ReadReg(pVdaaHW,pVdaa->channel,LSIDE_REV);
  pVdaa->deviceId->lsRev= ((data&0x3C)>>2);
  data = pVdaa->ReadRegX(pVdaa->pVdaaHWX,pVdaa->channel,SYS_LINE_DEV_REV);
  pVdaa->deviceId->lsType= ((data&~(0xF))>>4);

  return RC_NONE;
}

/*
** Function:  Vdaa_PowerdownLineside
**
** Description:
** Power down lineside device
**
*/

int Vdaa_PowerdownLineside(vdaaChanType_ptr pVdaa)
{
  TRACEPRINT(pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  WriteReg(pVdaaHW,pVdaa->channel,DAA_CTRL2,0x10);
  return RC_NONE;
}

/*
** Function: Vdaa_PrintDebugData
**
** Description:
** Dump of VDAA register space
**
** Input Parameters:
** pVdaa:       pointer to SiVoiceChanType or vdaaChanType
**
*/

int Vdaa_PrintDebugData (vdaaChanType *pVdaa)
{
#ifdef ENABLE_DEBUG
  int i;
  for (i=0; i<60; i++)
  {
    LOGPRINT ("%sRegister %d = %X\n",LOGPRINT_PREFIX,i,ReadReg(pVdaaHW,
              pVdaa->channel,i));
  }
#endif
  return RC_NONE;
}

/*
** Function: Vdaa_InitLineInUseCheck
**
** Description:
** Set line in use test limits
**
** Input Parameters:
** liuCfg:      pointer to vdaa_LIU_Config
** minOnV:      minimum acceptable onhook voltage (below indicates parallel handset)
** minOffV:     minimum acceptable offhook voltage (below indicates parallel handset)
** minOffI:     minimum acceptable offhook loop current (below indicates parallel handset)
**
*/

int Vdaa_InitLineInUseCheck(vdaa_LIU_Config *liuCfg, int8 minOnV, int8 minOffV,
                            int16 minOffI)
{
  TRACEPRINT_NOCHAN("min0nV = %d minoffV = %d minOffI = %d\n", minOnV, minOffV,
                    minOffI);
  liuCfg->status = PAR_HANDSET_NOT_DETECTED;
  liuCfg->min_onhook_vloop = minOnV;
  liuCfg->min_offhook_vloop = minOffV;
  liuCfg->min_offhook_iloop = minOffI;
  return RC_NONE;
}

/*
** Function: Vdaa_CheckForLineInUse
**
** Description:
** Monitor LVCS to detect intrusion or parallel handset
**
** Input Parameters:
** pVdaa:       pointer to SiVoiceChanType or vdaaChanType
** liuCfg:      pointer to vdaa_LIU_Config
**
** Output Parameters:
**
** Return:
** VDAA_ONHOOK    -   line is onhook
** VDAA_OFFHOOK   -   line is offhook (in use)
**
*/

uInt8 Vdaa_CheckForLineInUse(vdaaChanType *pVdaa, vdaa_LIU_Config *liuCfg)
{
  int8 vloop;
  int16 iloop;

  TRACEPRINT(pVdaa, "\n", NULL);
  if(pVdaa->channelType != DAA)
  {
    return RC_IGNORE;
  }

  /* Check voltage and current */
  Vdaa_ReadLinefeedStatus(pVdaa, &vloop,&iloop);
  if(vloop < 0)
  {
    vloop *= -1;
  }
  liuCfg->measured_iloop = iloop;
  liuCfg->measured_vloop = vloop;
  liuCfg->status = PAR_HANDSET_NOT_DETECTED;

  /* Read hookswitch status */
  if(Vdaa_GetHookStatus(pVdaa) == VDAA_OFFHOOK)
  {
    if((vloop < liuCfg->min_offhook_vloop)||(iloop < liuCfg->min_offhook_iloop))
    {
      liuCfg->status = PAR_HANDSET_DETECTED;
    }

    return VDAA_OFFHOOK;
  }
  else
  {
    if(vloop < liuCfg->min_onhook_vloop)
    {
      liuCfg->status = PAR_HANDSET_DETECTED;
    }

    return VDAA_ONHOOK;
  }
}

