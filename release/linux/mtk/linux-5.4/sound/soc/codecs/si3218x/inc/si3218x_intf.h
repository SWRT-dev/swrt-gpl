/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Si3218x ProSLIC interface header file
 *
 * Distributed by:
 * Silicon Laboratories, Inc
 *
 * This file contains proprietary information.
 * No dissemination allowed without prior written permission from
 * Silicon Laboratories, Inc.
 *
 * File Description:
 * This is the header file for the ProSLIC driver.
 *
 *
 */

#ifndef SI3218X_INTF_H
#define SI3218X_INTF_H
#include "si3218x.h"

/* The following macros are for backward compatibility */
#define Si3218x_LoadPatch                    ProSLIC_LoadPatch
#define Si3218x_ReadHookStatus               ProSLIC_ReadHookStatus
#define Si3218x_DCFeedSetup(PCHAN, NDX)      Si3218x_DCFeedSetupCfg((PCHAN),Si3218x_DCfeed_Presets, (NDX))
#define Si3218x_VerifyPatch		             ProSLIC_VerifyPatch
#define Si3218x_SetPowersaveMode             ProSLIC_SetPowersaveMode
#define Si3218x_Init(PCHAN, SZ)              Si3218x_Init_with_Options((PCHAN), (SZ), INIT_NO_OPT)
#define Si3218x_LoadRegTables                ProSLIC_LoadRegTables
#define Si3218x_VerifyControlInterface       ProSLIC_VefifyControlInterface
#define Si3218x_ShutdownChannel              ProSLIC_PowerDownConverter
#define Si3218x_PowerDownConverter           ProSLIC_PowerDownConverter
#define Si3218x_Calibrate                    ProSLIC_Calibrate
#define Si3218x_SetLinefeedStatusBroadcast   ProSLIC_SetLinefeedStatusBroadcast
#define Si3218x_SetLinefeedStatus            ProSLIC_SetLinefeedStatus
#define Si3218x_MWIEnable                    ProSLIC_MWIEnable
#define Si3218x_MWIDisable                   ProSLIC_MWIDisable
#define Si3218x_SetMWIState                  ProSLIC_SetMWIState
#define Si3218x_GetMWIState                  ProSLIC_GetMWIState
#define Si3218x_MWISetup(PCHAN,VPK,LCR)      ProSLIC_MWISetV(PCHAN,VPK)

/* DC Feed */
#ifndef DISABLE_DCFEED_SETUP
extern Si3218x_DCfeed_Cfg Si3218x_DCfeed_Presets[];
#endif

/*
**
** PROSLIC INITIALIZATION FUNCTIONS
**
*/

/*
** Function: PROSLIC_Init_MultiBOM
**
** Description:
** Initializes the ProSLIC w/ selected general parameters
**
** Input Parameters:
** pProslic: pointer to PROSLIC object
** size:     number of channels
** preset:   General configuration preset
**
** Return:
** none
*/
#ifdef SIVOICE_MULTI_BOM_SUPPORT
int Si3218x_Init_MultiBOM (proslicChanType_ptr *hProslic,int size,int preset);
#endif

/*
** Function: Si3218x_Init_with_Options
**
** Description:
** Initializes the ProSLIC with an option.
**
** Input Parameters:
** pProslic: pointer to PROSLIC object
** size - number of continuous channels to initialize
** init_opt - which initialization type to do.
**
** Return:
** none
*/
int Si3218x_Init_with_Options (proslicChanType_ptr *pProslic, int size,
                               initOptionsType init_opt);

/*
** Function: PROSLIC_VerifyControlInterface
**
** Description:
** Verify SPI port read capabilities
**
** Input Parameters:
** pProslic: pointer to PROSLIC object
**
** Return:
** none
*/
int Si3218x_VerifyControlInterface (proslicChanType_ptr hProslic);

/*
** Function: Si3218x_PowerUpConverter
**
** Description:
** Powers all DC/DC converters sequentially with delay to minimize
** peak power draw on VDC.
**
** Returns:
** int (error)
**
*/
int Si3218x_PowerUpConverter(proslicChanType_ptr hProslic);

/*
** Function: Si3218x_PowerDownConverter
**
** Description:
** Power down DCDC converter (selected channel only)
**
** Returns:
** int (error)
**
*/
int Si3218x_PowerDownConverter(proslicChanType_ptr hProslic);

/*
** Function: PROSLIC_EnableInterrupts
**
** Description:
** Enables interrupts
**
** Input Parameters:
** hProslic: pointer to Proslic object
**
** Return:
**
*/
int Si3218x_EnableInterrupts (proslicChanType_ptr hProslic);

/*
**
** PROSLIC CONFIGURATION FUNCTIONS
**
*/

/*
** Function: PROSLIC_RingSetup
**
** Description:
** configure ringing
**
** Input Parameters:
** pProslic: pointer to Proslic object
** pRingSetup: pointer to ringing config structure
**
** Return:
** none
*/
int Si3218x_RingSetup (proslicChanType *pProslic, int preset);

/*
** Function: PROSLIC_ZsynthSetup
**
** Description:
** configure impedance synthesis
**
** Input Parameters:
** pProslic: pointer to Proslic object
** pZynth: pointer to zsynth config structure
**
** Return:
** none
*/
int Si3218x_ZsynthSetup (proslicChanType *pProslic, int preset);

/*
** Function: PROSLIC_AudioGainSetup
**
** Description:
** configure audio gains
**
** Input Parameters:
** pProslic: pointer to Proslic object
** preset:   impedance preset to scale
**
** Return:
** none
*/
int Si3218x_TXAudioGainSetup (proslicChanType *pProslic, int preset);
int Si3218x_RXAudioGainSetup (proslicChanType *pProslic, int preset);
#define Si3218x_AudioGainSetup ProSLIC_AudioGainSetup

/*
** Function: PROSLIC_TXAudioGainScale
**
** Description:
** configure audio gains
**
** Input Parameters:
** pProslic:  pointer to Proslic object
** preset:    pointer to audio gains config structure
** pga_scale: pga_scaling constant
** eq_scale:  equalizer scaling constant
**
** Return:
** none
*/
int Si3218x_TXAudioGainScale (proslicChanType *pProslic, int preset,
                              uInt32 pga_scale, uInt32 eq_scale);

/*
** Function: PROSLIC_RXAudioGainScale
**
** Description:
** configure audio gains
**
** Input Parameters:
** pProslic:  pointer to Proslic object
** preset:    pointer to audio gains config structure
** pga_scale: pga_scaling constant
** eq_scale:  equalizer scaling constant
**
** Return:
** none
*/
int Si3218x_RXAudioGainScale (proslicChanType *pProslic, int preset,
                              uInt32 pga_scale, uInt32 eq_scale);

/*
** Function: PROSLIC_AudioEQSetup
**
** Description:
** configure audio equalizers
**
** Input Parameters:
** pProslic: pointer to Proslic object
** pAudioEQ: pointer to ringing config structure
**
** Return:
** none
*/
int Si3218x_AudioEQSetup (proslicChanType *pProslic, int preset);

/*
** Function: PROSLIC_DCFeedSetup
**
** Description:
** configure dc feed
**
** Input Parameters:
** pProslic: pointer to Proslic object
** pDcFeed: pointer to dc feed config structure
**
** Return:
** none
*/
int Si3218x_DCFeedSetupCfg (proslicChanType *pProslic, ProSLIC_DCfeed_Cfg *cfg,
                            int preset);

/*
** Function: PROSLIC_PCMSetup
**
** Description:
** configure pcm
**
** Input Parameters:
** pProslic: pointer to Proslic object
** pPcm: pointer to pcm config structure
**
** Return:
** none
*/
int Si3218x_PCMSetup(proslicChanType *pProslic, int preset);

/*
**
** PROSLIC CONTROL FUNCTIONS
**
*/

/*
** Function: PROSLIC_GetInterrupts
**
** Description:
** Enables interrupts
**
** Input Parameters:
** hProslic: pointer to Proslic object
** pIntData: pointer to interrupt info retrieved
**
** Return:
**
*/
int Si3218x_GetInterrupts (proslicChanType_ptr hProslic,
                           proslicIntType *pIntData);

/*
** Function: PROSLIC_PulseMeterSetup
**
** Description:
** configure pulse metering
**
** Input Parameters:
** pProslic: pointer to Proslic object
** pPulseCfg: pointer to pulse metering config structure
**
** Return:
** none
*/
int Si3218x_PulseMeterSetup (proslicChanType *pProslic, int preset);

/*
** Function: PROSLIC_dbgSetDCFeed
**
** Description:
** provision function for setting up
** dcfeed given desired open circuit voltage
** and loop current.
*/
int Si3218x_dbgSetDCFeed (proslicChanType *pProslic, uInt32 v_vlim_val,
                          uInt32 i_ilim_val, int32 preset);

/*
** Function: PROSLIC_dbgSetDCFeedVopen
**
** Description:
** provision function for setting up
** dcfeed given desired open circuit voltage
** and loop current.
*/
int Si3218x_dbgSetDCFeedVopen (proslicChanType *pProslic, uInt32 v_vlim_val,
                               int32 preset);


/*
** Function: PROSLIC_dbgSetDCFeedIloop
**
** Description:
** provision function for setting up
** dcfeed given desired open circuit voltage
** and loop current.
*/
int Si3218x_dbgSetDCFeedIloop (proslicChanType *pProslic, uInt32 i_ilim_val,
                               int32 preset);


/*
** Function: PROSLIC_dbgRingingSetup
**
** Description:
** Provision function for setting up
** Ring type, frequency, amplitude and dc offset.
** Main use will be by peek/poke applications.
*/
int Si3218x_dbgSetRinging (proslicChanType *pProslic,
                           ProSLIC_dbgRingCfg *ringCfg, int preset);

/*
** Function: PROSLIC_dbgSetRXGain
**
** Description:
** Provision function for setting up
** RX path gain.
*/
int Si3218x_dbgSetRXGain (proslicChanType *pProslic, int32 gain,
                          int impedance_preset, int audio_gain_preset);

/*
** Function: PROSLIC_dbgSetTXGain
**
** Description:
** Provision function for setting up
** TX path gain.
*/
int Si3218x_dbgSetTXGain (proslicChanType *pProslic, int32 gain,
                          int impedance_preset, int audio_gain_preset);


/*
** Function: PROSLIC_LineMonitor
**
** Description:
** Monitor line voltages and currents
*/
int Si3218x_LineMonitor(proslicChanType *pProslic, proslicMonitorType *monitor);


/*
** Function: PROSLIC_PSTNCheck
**
** Description:
** Continuous monitor of ilong to detect hot pstn line
*/
int Si3218x_PSTNCheck(proslicChanType *pProslic,
                      proslicPSTNCheckObjType *pstnCheckObj);


/*
** Function: PROSLIC_PSTNCheck
**
** Description:
** Continuous monitor of ilong to detect hot pstn line
*/
int Si3218x_DiffPSTNCheck(proslicChanType *pProslic,
                          proslicDiffPSTNCheckObjType *pstnCheckObj);

/*
** Function: PROSLIC_ReadMADCScaled
**
** Description:
** Read MADC (or other sensed voltages/currents) and
** return scaled value in int32 format
*/
int32 Si3218x_ReadMADCScaled(proslicChanType_ptr pProslic, uInt16 addr,
                             int32 scale);

/*
** Function: Si3218x_GenParamUpdate
**
** Description:
** Update Si3218x general parameters and other required modifications
** to default reg/ram values
**
** Returns:
** int (error)
**
*/
int Si3218x_GenParamUpdate(proslicChanType_ptr hProslic,initSeqType seq);


/*
** Function: Si3218x_SelectPatch
**
** Description:
** Select appropriate patch based on general parameters
**
** Returns:
** int (error)
**
*/
int Si3218x_SelectPatch(proslicChanType_ptr hProslic,
                        const proslicPatch **patch);

/*
** Function: Si3218x_ConverterSetup
**
** Description:
** Program revision specific settings before powering converter
**
** Specifically, from general parameters and knowledge that this
** is Rev C, setup dcff drive, gate drive polarity, and charge pump.
**
** Returns:
** int (error)
**
*/
int Si3218x_ConverterSetup(proslicChanType_ptr pProslic);

/*
** Function: Si3218x8_PulseMeterSetup
**
** Description:
** configure pulse metering
*/
int Si3218x_PulseMeterSetup (proslicChanType_ptr hProslic, int preset);


/* 
** Function: Si3218x_GetChipInfo
** Description: Returns the specific chipset in the Si3218x family.
** Input parameters: channel pointer
** output: either RC_SPI_FAIL or RC_NONE.  channel pointer deviceId will be updated.
**/
int Si3218x_GetChipInfo(proslicChanType_ptr pProslic);


#endif

