/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Vdaa  VoiceDAA interface header file
 *
 *
 * Distributed by:
 * Silicon Laboratories, Inc
 *
 * This file contains proprietary information.
 * No dissemination allowed without prior written permission from
 * Silicon Laboratories, Inc.
 *
 * File Description:
 * This is the header file for the main  VoiceDAA API and is used
 * in the  VoiceDAA demonstration code.
 *
 *
 */

#ifndef VDAA_INTF_H
#define VDAA_INTF_H

#include "../config_inc/si_voice_datatypes.h"
#include "si_voice.h"
/*****************************************************************************/
/** @defgroup VDAA_API VDAA API
 * This section covers APIs and definitions related to the VDAA/FXO.
 * @{
 */

/*
** Constants
*/
#define BROADCAST 0xff
#define LCS_SCALE_NUM 33		/* Line Current Status Scale */
#define LCS_SCALE_DEN 10		/* Line Current Status Scale */

/* Macro substitution from Vdaa legacy API's to SiVoice API's */
#define Vdaa_createControlInterface SiVoice_createControlInterface
#define Vdaa_destroyControlInterface SiVoice_destroyControlInterface
#define Vdaa_createDevice SiVoice_createDevice
#define Vdaa_destroyDevice SiVoice_destroyDevice
#define Vdaa_createChannel SiVoice_createChannel
#define Vdaa_destroyChannel SiVoice_destroyChannel

#define Vdaa_setControlInterfaceCtrlObj SiVoice_setControlInterfaceCtrlObj
#define Vdaa_setControlInterfaceReset SiVoice_setControlInterfaceReset
#define Vdaa_setControlInterfaceWriteRAM SiVoice_setControlInterfaceWriteRAM
#define Vdaa_setControlInterfaceReadRAM SiVoice_setControlInterfaceReadRAM
#define Vdaa_setControlInterfaceWriteRegister SiVoice_setControlInterfaceWriteRegister
#define Vdaa_setControlInterfaceReadRegister SiVoice_setControlInterfaceReadRegister
#define Vdaa_setControlInterfaceTimerObj SiVoice_setControlInterfaceTimerObj
#define Vdaa_setControlInterfaceDelay SiVoice_setControlInterfaceDelay
#define Vdaa_setControlInterfaceSemaphore SiVoice_setControlInterfaceSemaphore
#define Vdaa_setControlInterfaceTimeElapsed SiVoice_setControlInterfaceTimeElapsed
#define Vdaa_setControlInterfaceGetTime SiVoice_setControlInterfaceGetTime

#define Vdaa_SWInitChan SiVoice_SWInitChan

#define Vdaa_setSWDebugMode SiVoice_setSWDebugMode
#define Vdaa_getErrorFlag SiVoice_getErrorFlag
#define Vdaa_clearErrorFlag SiVoice_clearErrorFlag

#define Vdaa_setChannelEnable SiVoice_setChannelEnable
#define Vdaa_getChannelEnable SiVoice_getChannelEnable

#define Vdaa_Reset SiVoice_Reset
#define Vdaa_Version SiVoice_Version

#define SI3050_CHAN_PER_DEVICE 1

/*
**
** VDAA Initialization/Configuration Parameter Options
**
*/
/*
** This defines names for the PCM Data Format
*/
typedef enum
{
  A_LAW = 0,	/**< 00 = A-Law. Signed magnitude data format */
  U_LAW = 1,	/**< 01 = u-Law. Signed magnitude data format */
  LINEAR_16_BIT = 3 /**< 11 = 16-bit linear (2s complement data format) */
} tPcmFormat;

/*
** This defines names for the phcf bits
*/
typedef enum
{
  PCLK_1_PER_BIT = 0,
  PCLK_2_PER_BIT = 1
} tPHCF;


/*
** This defines names for the tri bit
*/
typedef enum
{
  TRI_POS_EDGE = 0,
  TRI_NEG_EDGE = 1
} tTRI;


/**
** This defines names for the AC impedance range
*/
typedef enum
{
  AC_600,				/**< 600 Ohms */
  AC_900,				/**< 900 O */
  AC_270__750_150,		/**< 270 O + (750 O || 150 nF) and 275 O + (780 O || 150 nF) */
  AC_220__820_120,		/**< 220 O + (820 O || 120 nF) and 220 O + (820 O || 115 nF) */
  AC_370__620_310,		/**< 370 O + (620 O || 310 nF) */
  AC_320__1050_230,		/**< 320 O + (1050 O || 230 nF) */
  AC_370__820_110,		/**< 370 O + (820 O || 110 nF) */
  AC_275__780_115,		/**< 275 O + (780 O || 115 nF) */
  AC_120__820_110,		/**< 120 O + (820 O || 110 nF) */
  AC_350__1000_210,		/**< 350 O + (1000 O || 210 nF) */
  AC_200__680_100,		/**< 200 O + (680 O || 100 nF) */
  AC_600__2160,			/**< 600 O + 2.16 uF */
  AC_900__1000,			/**< 900 O + 1 uF */
  AC_900__2160,			/**< 900 O + 2.16 uF */
  AC_600__1000,			/**< 600 O + 1 uF */
  AC_Global_impedance		/**< Global impedance */
} tAC_Z;

/**
** This defines names for the DC impedance range
*/
typedef enum
{
  DC_50,					/**< 50 Ohms dc termination is selected */
  DC_800					/**< 800 Ohms dc termination is selected */

} tDC_Z;

/**
** This defines names for the ringer impedance range
*/
typedef enum
{
  RZ_MAX = 0,
  RZ_SYNTH = 1
} tRZ;

/**
** This defines names for the dc voltage adjust
*/
typedef enum
{
  DCV3_1 = 0,
  DCV3_2 = 1,
  DCV3_35 = 2,
  DCV3_5 = 3
} tDCV;

/**
** This defines names for the minimum loop current
*/
typedef enum
{
  MINI_10MA = 0,
  MINI_12MA = 1,
  MINI_14MA = 2,
  MINI_16MA = 3
} tMINI;

/**
** This defines names for the current limiting enable bit
*/
typedef enum
{
  ILIM_DISABLED = 0,
  ILIM_ENABLED = 1
} tILIM;

/**
** This defines names for the ring detect interupt mode
*/
typedef enum
{
  RDI_BEG_BURST = 0,
  RDI_BEG_END_BURST = 1
} tRDI;

/**
** This defines names for the on hook speed / spark quenching
*/
typedef enum
{
  OHS_LESS_THAN_0_5MS = 0,
  OHS_3MS = 1,
  OHS_26MS = 0xE
} tOHS;

/**
** This defines names for the hbe bit
*/
typedef enum
{
  HYBRID_DISABLED = 0,
  HYBRID_ENABLED  = 1
} tHBE;


/**
** Gain/Attenuation Select
*/
typedef enum
{
  XGA_GAIN,
  XGA_ATTEN
} tXGA;

/**
** MUTE Control Options
*/
typedef enum
{
  MUTE_DISABLE_ALL,
  MUTE_DISABLE_RX,
  MUTE_DISABLE_TX,
  MUTE_ENABLE_RX,
  MUTE_ENABLE_TX,
  MUTE_ENABLE_ALL
} tMUTE;

/**
** This defines names for the ring delay setting
*/
typedef enum
{
  RDLY_0MS = 0,
  RDLY_256MS = 1,
  RDLY_512MS = 2,
  RDLY_768MS = 3,
  RDLY_1024MS = 4,
  RDLY_1280MS = 5,
  RDLY_1536MS = 6,
  RDLY_1792MS = 7
} tRDLY;

/**
** This defines names for the ring timeouts
*/
typedef enum
{
  RTO_128MS = 1,
  RTO_256MS = 2,
  RTO_384MS = 3,
  RTO_512MS = 4,
  RTO_640MS = 5,
  RTO_768MS = 6,
  RTO_896MS = 7,
  RTO_1024MS = 8,
  RTO_1152MS = 9,
  RTO_1280MS = 10,
  RTO_1408MS = 11,
  RTO_1536MS = 12,
  RTO_1664MS = 13,
  RTO_1792MS = 14,
  RTO_1920MS = 15
} tRTO;

/**
** This defines names for the ring timeouts
*/
typedef enum
{
  RCC_100MS = 0,
  RCC_150MS = 1,
  RCC_200MS = 2,
  RCC_256MS = 3,
  RCC_384MS = 4,
  RCC_512MS = 5,
  RCC_640MS = 6,
  RCC_1024MS = 7
} tRCC;

/**
** This defines names for the ring validation modes
*/
typedef enum
{
  RNGV_DISABLED = 0,
  RNGV_ENABLED = 1
} tRNGV;

/**
** This defines names for the rfwe bit
*/
typedef enum
{
  RFWE_HALF_WAVE = 0,
  RFWE_FULL_WAVE = 1,
  RFWE_RNGV_RING_ENV = 0,
  RFWE_RNGV_THRESH_CROSS = 1
} tRFWE;

/**
** This defines names for the rt and rt2 bit
*/
typedef enum
{
  RT__13_5VRMS_16_5VRMS = 0,
  RT__19_35VRMS_23_65VRMS = 2,
  RT__40_5VRMS_49_5VRMS = 3
} tRT;

/**
** This defines names for the rt and rt2 bit
*/
typedef enum
{
  RGDT_ACTIVE_LOW = 0,
  RGDT_ACTIVE_HI = 1
} tRPOL;


/**
** This defines names for the interrupts
*/
typedef enum
{
  POLI,
  TGDI,
  LCSOI,
  DODI,
  BTDI,
  FTDI,
  ROVI,
  RDTI,
  CVI		/**< Current/Voltage Interrupt REGISTER#44 */
} vdaaInt;

/**
** Interrupt Bitmask Fields
*/
typedef enum
{
  POLM = 1,
  TGDM = 2,   /**< Si3050  Only */
  LCSOM = 4,
  DODM = 8,
  BTDM = 16,
  FDTM = 32,
  ROVM = 64,
  RDTM = 128
} vdaaIntMask;


/**
** This defines names for the idl bit (obsolete)
*/
typedef enum
{
  IDL_DISABLED = 0,
  IDL_ENABLED = 1
} tIDL;

/**
** This defines names for the ddl bit (obsolete)
*/
typedef enum
{
  DDL_NORMAL_OPERATION = 0,
  DDL_PCM_LOOPBACK = 1
} tDDL;

/**
** Loopback Modes
*/
typedef enum
{
  LPBK_NONE = 0,
  LPBK_IDL = 1,
  LPBK_DDL = 2,
  LPBK_PCML = 3
} tLpbkMode;

/**
** Loopback Status
*/
typedef enum
{
  LPBK_DISABLED = 0,
  LPBK_ENABLED = 1
} tLpbkStatus;

/**
** This defines names for the interrupt pin modes
*/
typedef enum
{
  INTE_DISABLED = 0,
  INTE_ENABLED = 1
} tInte;

/**
** This defines names for the interrupt pin polarities
*/
typedef enum
{
  INTE_ACTIVE_LOW = 0,
  INTE_ACTIVE_HIGH = 1
} tIntePol;

/**
** This defines names for the pwm settings
*/
typedef enum
{
  PWM_DELTA_SIGMA = 0,
  PWM_CONVENTIONAL_16KHZ = 1,
  PWM_CONVENTIONAL_32KHZ = 2
} tPwmMode;

/**
** PWME
*/
typedef enum
{
  PWM_DISABLED = 0,
  PWM_ENABLED
} tPWME;

/**
** RCALD control
*/
typedef enum
{
  RES_CAL_ENABLED = 0,
  RES_CAL_DISABLED
} tRCALD;

/**
** Voice DAA Hook states
*/
enum
{
  VDAA_DIG_LOOPBACK = 1,
  VDAA_ONHOOK = 2,
  VDAA_OFFHOOK = 3,
  VDAA_ONHOOK_MONITOR = 4
};

/**
** FDT Monitoring Options
*/
enum
{
  FDT_MONITOR_OFF = 0,
  FDT_MONITOR_ON
};


/**
** Offhook Speed Select
*/
typedef enum
{
  FOH_512,
  FOH_128,
  FOH_64,
  FOH_8
} tFOH;

/**
** Sample Rate Control
*/
typedef enum
{
  FS_8KHZ,
  FS_16KHZ
} tHSSM;

/**
** Line Voltage Force Disable
*/
typedef enum
{
  LVS_FORCE_ENABLED = 0,
  LVS_FORCE_DISABLED
} tLVFD;

/**
** Current/Voltage Monitor Select
*/
typedef enum
{
  CVS_CURRENT,
  CVS_VOLTAGE
} tCVS;

/**
** Current/Voltage Interrupt Polarity
*/
typedef enum
{
  CVP_BELOW,
  CVP_ABOVE
} tCVP;

/**
** Guarded Clear
*/
typedef enum
{
  GCE_DISABLED = 0,
  GCE_ENABLED
} tGCE;

/**
** SPI Mode (Si3050 Only)
*/
typedef enum
{
  SPIM_TRI_CS,
  SPIM_TRI_SCLK
} tSPIM;

/**
**  FILT
*/
typedef enum
{
  FILT_HPF_5HZ,
  FILT_HPF_200HZ
} tFILT;

/**
** IIRE
*/
typedef enum
{
  IIR_DISABLED = 0,
  IIR_ENABLED
} tIIRE;

/**
** FULL2
*/
typedef enum
{
  FULL2_DISABLED = 0,
  FULL2_ENABLED
} tFULL2;

/**
** FULL
*/
typedef enum
{
  FULL_DISABLED = 0,
  FULL_ENABLED
} tFULL;

/**
** RG1
*/
typedef enum
{
  RG1_DISABLED = 0,
  RG1_ENABLED
} tRG1;


/**
** -----------------------------
** CONFIGURATION DATA STRUCTURES
** -----------------------------
*/

/**
** (Updated) Structure for General Parameters
*/
typedef struct
{
  tInte inte;             /* INTE */
  tIntePol intp;          /* INTP */
  tRCALD rcald;           /* RCALD */
  tHSSM hssm;             /* HSSM */
  tFOH foh;               /* FOH */
  tLVFD lvfd;             /* LVFD */
  tCVS cvs;               /* CVS */
  tCVP cvp;               /* CVP */
  tGCE gce;               /* GCE */
  tIIRE iire;             /* IIRE */
  tFULL2 full2;           /* FULL2 */
  tFULL full;             /* FULL */
  tFILT filt;             /* FILT */
  tRG1 rg1;               /* RG1 */
  tPwmMode pwmm;          /* PWMM Si3050 Only */
  tPWME pwmEnable;        /* PWME Si3050 Only */
  tSPIM spim;             /* SPIM Si3050 Only */
} vdaa_General_Cfg;


/**
** (NEW) Structure for Country Presets
*/
typedef struct
{
  tRZ rz;
  tDC_Z dcr;
  tAC_Z acim;
  tDCV dcv;
  tMINI mini;
  tILIM ilim;
  tOHS ohs_sq;
  tHBE hbe;
} vdaa_Country_Cfg;

/**
** (NEW) Structure for Hybrid Presets
*/
typedef struct
{
  uInt8 hyb1;
  uInt8 hyb2;
  uInt8 hyb3;
  uInt8 hyb4;
  uInt8 hyb5;
  uInt8 hyb6;
  uInt8 hyb7;
  uInt8 hyb8;
} vdaa_Hybrid_Cfg;

/**
** Structure for PCM configuration presets
*/
typedef struct
{
  tPcmFormat pcmFormat;
  tPHCF pcmHwy;
  tTRI pcm_tri;
} vdaa_PCM_Cfg;

/** @addtogroup VDAA_AUDIO
 * @{
 */
/** @addtogroup VDAA_GAIN_CONTROL
 * @{
 */
/*
** (Updated) Structure for Audio path gain preset
*/
typedef struct
{
  uInt8		mute;
  tXGA		xga2;
  uInt8   	acgain2;
  tXGA		xga3;
  uInt8   	acgain3;
  uInt8		callProgress;
  BOOLEAN     cpEn;
} vdaa_audioGain_Cfg;

/** @} VDAA_GAIN_CONTROL*/
/** @} */
/*
** Structure for configuring ring detect config
*/

typedef struct
{
  tRDLY rdly;
  tRT rt;
  uInt8 rmx;
  tRTO rto;
  tRCC rcc;
  tRNGV rngv;
  uInt8 ras;
  tRFWE rfwe;
  tRDI rdi;
  tRPOL rpol;
} vdaa_Ring_Detect_Cfg;


/*
** Defines structure of interrupt data
*/
typedef struct
{
  vdaaInt *irqs;
  uInt8 number;
} vdaaIntType;

/*
** Defines structure for configuring Loop Back
*/
typedef struct
{
  tIDL isoDigLB;
  tDDL digDataLB;
} vdaa_Loopback_Cfg;


/*
** Generic Flag
*/
typedef enum
{
  VDAA_BIT_SET = 1,
  VDAA_BIT_CLEAR = 0
} tVdaaBit;

/*
** Defines structure for daa current status (ring detect/hook stat)
*/
typedef struct
{
  tVdaaBit ringDetectedNeg;
  tVdaaBit ringDetectedPos;
  tVdaaBit ringDetected;
  tVdaaBit offhook;
  tVdaaBit onhookLineMonitor;
} vdaaRingDetectStatusType;


typedef SiVoiceControlInterfaceType vdaaControlInterfaceType;
typedef SiVoiceDeviceType vdaaDeviceType;
typedef SiVoiceChanType vdaaChanType;

/*
** This is the main  VoiceDAA interface object pointer
*/
typedef vdaaChanType *vdaaChanType_ptr;

/*
** Defines initialization data structures
*/
typedef struct
{
  uInt8 address;
  uInt8 initValue;
} vdaaRegInit;


typedef enum
{
  PAR_HANDSET_NOT_DETECTED = 0,
  PAR_HANDSET_DETECTED = 1
} vdaaPHDStatus;

/*
** Line In Use Configuration Structure
*/
typedef struct
{
  vdaaPHDStatus status;
  int8 min_onhook_vloop;
  int8 min_offhook_vloop;
  int16 min_offhook_iloop;
  int8 measured_vloop;
  int16 measured_iloop;
} vdaa_LIU_Config;

/*
** Function Declarations
*/

/*****************************************************************************/
/** @defgroup VDAA_AUDIO Audio
 * @{
 */
/** @defgroup VDAA_GAIN_CONTROL Gain Control
 * This section covers functions that allow one to adjust the audio
 * gains - with TX toward the network/SOC/DSP and RX toward the tip/ring.
 *
 * @{
 */

/**
 * @brief
 *  Sets the TX audio gain (toward the network).
 *
 * @param[in] pVdaa - which channel to configure
 * @param[in] preset - which preset to use (this may be from the constants file)
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_TXAudioGainSetup (vdaaChanType *pVdaa,int32 preset);

/**
 * @brief
 *  Sets the RX audio gain (toward the tip/ring).
 *
 * @param[in] pVdaa - which channel to configure
 * @param[in] preset - which preset to use (this may be from the constants file)
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_RXAudioGainSetup (vdaaChanType *pVdaa,int32 preset);

/** @} VDAA_GAIN_CONTROL */

/*****************************************************************************/
/** @defgroup VDAA_AUDIO_CONTROL Audio control/configuration
 * This group of functions is used to configure and control the PCM bus.  It is essential that @ref Vdaa_PCMSetup,
 * @ref Vdaa_PCMTimeSlotSetup and @ref Vdaa_PCMStart are called prior to any audio processing.
 * @{
 */

/**
 * @brief
 *  This configures the PCM bus with parameters such as companding and data latching timing.
 *
 * @param[in] pVdaa - which channel should be configured
 * @param[in] preset - which preset to use from the constants file (see configuration tool, PCM dialog box)
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 *
 * @sa Vdaa_PCMTimeSlotSetup ProSLIC_PCMStart
 */

int Vdaa_PCMSetup (vdaaChanType *pVdaa,int32 preset);

/**
 * @brief
 *  This configures the ProSLIC to latch and send the PCM data at a particular timeslot in terms of PCM clocks (PCLK).
 *
 *  Typically, for aLaw and uLaw, one can use the following calculation to set the rxcount and txcount parameters:
 *
 *  rxcount = txcount = (channel_number)*8;
 *
 *  For 16 bit linear, one can do the following:
 *
 *  rxcount = txcount = (channel_number)*16;
 *
 * where channel_number = which ProSLIC channel on the PCM bus.  For example, if one were to have 2 dual channel
 * VDAA's on the same PCM bus, this value would range from 0 to 3.
 *
 * @param[in] pVdaa - which channel should be configured
 * @param[in] rxcount -  how many clocks until reading data from the PCM bus
 * @param[in] txcount -  how many clocks until writing data to the PCM bus.
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 *
 * @sa Vdaa_PCMSetup Vdaa_PCMStart
 */

int Vdaa_PCMTimeSlotSetup (vdaaChanType *pVdaa, uInt16 rxcount, uInt16 txcount);

/**
 * @brief
 *  This enables PCM transfer on the given ProSLIC channel.
 *
 * @param[in] pVdaa - - which channel should be enabled
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 *
 * @sa ProSLIC_PCMSetup ProSLIC_PCMTimeSlotSetup ProSLIC_PCMStop
 */

int Vdaa_PCMStart (vdaaChanType *pVdaa);

/**
 * @brief
 *  This disables PCM transfer on the given VDAA channel. Typically, this is called for debugging
 *  purposes only.
 *
 * @param[in] pVdaa - - which channel should be disabled
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 *
 * @sa Vdaa_PCMSetup Vdaa_PCMTimeSlotSetup Vdaa_PCMStart
 */

int Vdaa_PCMStop (vdaaChanType *pVdaa);

/**
 * @brief
 * Program desired mute mode for the given channel.
 *
 * @param[in] pVdaa - which channel should be modified
 * @param[in] mute - what is the desired mode.
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 *
 */

int Vdaa_SetAudioMute(vdaaChanType *pVdaa, tMUTE mute);

/**
 * @brief
 * Program desired loopback test mode for the given channel.
 *
 * @param[in] pVdaa -  which channel should be modified
 * @param[in] lpbk_mode - what is the desired loopback mode.
 * @param[in] lpbk_status - is this to enable or disable the loopback mode.
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 *
 */

int Vdaa_SetLoopbackMode(vdaaChanType_ptr pVdaa, tLpbkMode lpbk_mode,
                         tLpbkStatus lpbk_status);

/*****************************************************************************/
/** @} VDAA_AUDIO */
/** @} */
/** @defgroup VDAA_RING Ring detection
 * @{
 */

/**
 * @brief This function configures ringing detect for the Vdaa.
 * @param[in] pVdaa - which channel to program
 * @param[in] preset - Index of predefined ring detect setup configuration
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_RingDetectSetup (vdaaChanType *pVdaa,int32 preset);


/**
 * @brief This function reads ring detect/hook status and populates the structure passed via pStatus.
 * @param[in] pVdaa - Pointer to Voice DAA channel structure
 * @param[out] pStatus - updated ring status.
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_ReadRingDetectStatus (vdaaChanType *pVdaa,
                               vdaaRingDetectStatusType *pStatus);

/** @} VDAA_RING */

/*****************************************************************************/
/** @defgroup VDAA_LINE_STATE Line state
 * @{
 */


/**
 * @brief This function sets the Voice DAA hook status
 * @param[in]  pVdaa - Pointer to Voice DAA channel structure
 * @param[in]  newHookStatus - new hook state (VDAA_ONHOOK, VDAA_OFFHOOK, VDAA_DIG_LOOPBACK or VDAA_ONHOOK_MONITOR)
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_SetHookStatus (vdaaChanType *pVdaa,uInt8 newHookStatus);

/**
 * @brief This function powers up the Voice DAA lineside device.
 * @param[in]  pVdaa - Pointer to Voice DAA channel structure
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_PowerupLineside(vdaaChanType_ptr pVdaa);

/**
 * @brief This function powers down the Voice DAA line side device.
 * @param[in]  pVdaa - Pointer to Voice DAA channel structure
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_PowerdownLineside(vdaaChanType_ptr pVdaa);

/**
 * @brief Read VDAA Hook Status
 * @param[in] pVdaa - which channel to return the hook status.
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

uInt8 Vdaa_GetHookStatus (vdaaChanType *pVdaa);

/** @} VDAA_LINE_STATE */

/*****************************************************************************/
/** @defgroup VDAA_DIAG Diagnostics
 * @{
 */

/**
 * @brief This function returns the Voice DAA linefeed status.
 * @param[in] pVdaa - Pointer to Voice DAA channel structure
 * @param[in,out] vloop - Pointer to loop voltage variable (set by this function) - in Volts
 * @param[in,out] iloop - Pointer to loop current variable (set by this function) - in mA
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_ReadLinefeedStatus (vdaaChanType *pVdaa,int8 *vloop, int16 *iloop);

/**
 * @brief This function initializes the vdaa_LIU_Config datastructure for line in use feature
 * @param[in,out] liuCfg - Pointer to vdaa_LIU_Config structure (user allocates)
 * @param[in] minOnV - minimum onhook loop voltage that indicates no parallel handset is present (V)
 * @param[in] minOffV - minimum offhook loop voltage that indicates no parallel handset is present (V)
 * @param[in] minOffI - minimum offhook loop current that indicates no parallel handset is preset (mA)
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 * @sa Vdaa_CheckForLineInUse
 */

int Vdaa_InitLineInUseCheck(vdaa_LIU_Config *liuCfg,int8 minOnV,int8 minOffV,
                            int16 minOffI );

/**
 * @brief Monitor LVCS to detect intrusion or parallel handset
 * @param[in] pVdaa - Pointer to Voice DAA channel structure
 * @param[in,out] liuCfg - Pointer to vdaa_LIU_Config structure
 * @retval VDAA_ONHOOK or VDAA_OFFHOOK (in use)
 * @sa Vdaa_InitLineInUseCheck
 */

uInt8 Vdaa_CheckForLineInUse(vdaaChanType *pVdaa, vdaa_LIU_Config *liuCfg);

/**
 * @brief This function returns the status of the Frame Detect (FDT) bit.
 * @param[in] pVdaa - Pointer to Voice DAA channel structure
 * @retval int - 0 - Frame NOT detected, 1 = Frame detected
 */

int Vdaa_ReadFDTStatus(vdaaChanType_ptr pVdaa);

/** @} VDAA_DIAG */

/*****************************************************************************/
/** @defgroup VDAA_INTERRUPTS Interrupts
 * @{
 */


/**
 * @brief Enables interrupts based on passed 9-bit bitmask.  Bit values defined by vdaaIntMask enum.
 * @param[in] pVdaa - Pointer to Voice DAA channel structure
 * @param[in] bitmask - a logical or of @ref vdaaIntMask enum
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_SetInterruptMask(vdaaChanType *pVdaa, vdaaIntMask bitmask);

/**
 * @brief Returns the current interrupt status.
 * @param[in] pVdaa - Pointer to Voice DAA channel structure
 * @param[out]  pIntData - interrupt mask showing which interrupts are set.
 * @retval int - the number of interrupts set or RC_IGNORE
 */

int Vdaa_GetInterrupts (vdaaChanType *pVdaa,vdaaIntType *pIntData);

/**
 * @brief Clears ALL interrupts
 * @param[in] pVdaa - Pointer to Voice DAA channel structure
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_ClearInterrupts (vdaaChanType *pVdaa);

/** @} VDAA_INTERRUPTS */

/*****************************************************************************/
/** @defgroup VDAA_INIT Initialization
 * @{
 */

/*****************************************************************************/
/** @defgroup VDAA_HW_INIT Hardware/line initialization
 * @{
 */

/**
 * @brief
 *  This function configures the Voice DAA with a predefined country configuration preset.
 *
 * @param[in] pVdaa - which channel to initialize
 * @param[in] preset - The preset to use to configure the VDAA with.
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 *
 */

int Vdaa_CountrySetup (vdaaChanType *pVdaa,int32 preset);

/**
 * @brief
 *  This function configures the Voice DAA digital hybrid with a predefined preset.
 *
 * @param[in] pVdaa - which channel to initialize
 * @param[in] preset - Index of predefined digital hybrid preset
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 * @sa Vdaa_SetHybridEnable
 */

int Vdaa_HybridSetup (vdaaChanType *pVdaa,int32 preset);

/**
 * @brief
 *  This function fully initializes and loads the general config parameters to all Vdaa devices
 *  a given daisychain up to a given number.
 *
 * @param[in] pVdaa - which channel to initialize or start from if size>1
 * @param[in] size - the number of channels to initialize, should be at least 1
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 *
 */

int Vdaa_Init (vdaaChanType_ptr *pVdaa,int size);

/**
 * @brief
 *  This function calibrates the ADC (analog to digital converter) of the Vdaa device(s).
 *
 * @param[in] pVdaa - which channel to calibrate or starting channel if size>1
 * @param[in] size - the number of channels to calibrate.
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 *
 */

int Vdaa_ADCCal (vdaaChanType_ptr pVdaa, int32 size);

/**
 * @brief
 *  This function controls the Voice DAA digital hybrid.
 *
 * @param[in] pVdaa - which channel to set/unset
 * @param[in] enable - TRUE: enable digital hybrid, FALSE: disable digital hybrid
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 *
 */

int Vdaa_SetHybridEnable(vdaaChanType_ptr pVdaa, int enable);

/** @} VDAA_HW_INIT */
/*****************************************************************************/


/**
 * @brief  Enables watchdog timer
 * @param[in] pVdaa - the channel to enable the watchdog
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_EnableWatchdog(vdaaChanType_ptr pVdaa);

/**
 * @brief Execute a soft reset on a given channel.
 * @param[in] pVdaa - the channel to reset
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */

int Vdaa_SoftReset(vdaaChanType_ptr pVdaa);/** @} VDAA_INIT */

/** @} MISC */

/** @} VDAA_API */
#endif

