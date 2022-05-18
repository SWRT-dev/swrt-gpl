/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Author(s):
 * cdp
 *
 * Distributed by:
 * Silicon Laboratories, Inc
 *
 * This file contains proprietary information.
 * No dissemination allowed without prior written permission from
 * Silicon Laboratories, Inc.
 *
 * File Description:
 * This is the header file for the inward test implementations.
 *
 */
#ifndef PROSLIC_TSTIN_H
#define PROSLIC_TSTIN_H

#include "../config_inc/si_voice_datatypes.h"
#include "proslic.h"

/** @addtogroup DIAGNOSTICS
 * @{
 */
/** @defgroup PROSLIC_TSTIN ProSLIC Inward Tests (Test-In)
 * This section documents functions and data structures related to the
 * ProSLIC/FXS inward test implementations.
 * @{
 */

#define MIN_RINGING_SAMPLES 16          /**< Minimum number of ringing samples for measuring ring amplitude */
#define MAX_RINGING_SAMPLES 255         /**< Maximum number of ringing samples for measuring ring amplitude */
#define MIN_RINGING_SAMPLE_INTERVAL 1   /**< Minimum sample interval for measuring ring amplitude */
#define MAX_RINGING_SAMPLE_INTERVAL 10  /**< Maximum sample interval for measuring ring amplitude */

/**
* Test-In results status
*/
enum
{
  TSTIN_RESULTS_INVALID,
  TSTIN_RESULTS_VALID
};

/**
* PCM format options
*/
enum
{
  PCM_8BIT,
  PCM_16BIT
};

/**
* Abort if Line-in-use option
*/
enum
{
  ABORT_LIU_DISABLED,
  ABORT_LIU_ENABLED
};

/**
* Check for loop closure option
*/
enum
{
  LCR_CHECK_DISABLED,
  LCR_CHECK_ENABLED
};

/**
* Check for ringtrip option
*/
enum
{
  RTP_CHECK_DISABLED,
  RTP_CHECK_ENABLED
};


/**
 * Defines generic test limit/value/status structure
 */
typedef struct
{
  int32			lowerLimit;     /**< Lower test limit */
  int32			upperLimit;     /**< Upper test limit */
  int32			value;          /**< Numeric test result */
  uInt8			testResult;		/**< 0 - Fail, 1 - pass */
} proslicTestObj;

/**
 * Defines structure for PCM Loopback Test
 */
typedef struct
{
  BOOLEAN			testEnable;		/**< Gate execution/updating of results with this flag */
  BOOLEAN			pcmLpbkEnabled;	/**< Indicates if test data is valid (1) or stale (0) */
  BOOLEAN			pcm8BitLinear;  /**< Set to use 8 bit linear mode (used if normally using ulaw or alaw) */
  uInt8           pcmModeSave;    /**< Store entry PCMMODE value  */
  uInt8			testResult;		/**< OR of all test results in this structure */
} proslicPcmLpbkTest;


/**
 * Defines structure for DC Feed Test
 */
typedef struct
{
  BOOLEAN			testEnable;		    /**< Gate execution/updating of results with this flag */
  BOOLEAN			testDataValid;	    /**< Indicates if test data is valid (1) or stale (0) */
  BOOLEAN			abortIfLineInUse;   /**< Abort test if LCR set at the start of test. Leaves results invalid */
  BOOLEAN			applyLcrThresh;     /**< Apply alternate LCR thresholds to ensure LCR event occurs */
  uInt32
  altLcrOffThresh;    /**< Optional LCROFFHK threshold to apply during test */
  uInt32			altLcrOnThresh;     /**< Optional LCRONHK threshold to apply during test */
  BOOLEAN			lcrStatus;		    /**< Indicates LCR status after applying test load */
  proslicTestObj	dcfeedVtipOnhook;   /**< On-hook VTIP test results */
  proslicTestObj	dcfeedVringOnhook;  /**< On-hook VRING test results */
  proslicTestObj	dcfeedVloopOnhook;  /**< On-hook VLOOP test results */
  proslicTestObj	dcfeedVbatOnhook;   /**< On-hook VBAT test results */
  proslicTestObj	dcfeedItipOnhook;   /**< On-hook ITIP test results */
  proslicTestObj	dcfeedIringOnhook;  /**< On-hook IRING test results */
  proslicTestObj	dcfeedIloopOnhook;  /**< On-hook ILOOP test results */
  proslicTestObj	dcfeedIlongOnhook;  /**< On-hook ILONG test results */
  proslicTestObj	dcfeedVtipOffhook;  /**< Off-hook VTIP test results */
  proslicTestObj	dcfeedVringOffhook; /**< Off-hook VRING test results */
  proslicTestObj	dcfeedVloopOffhook; /**< Off-hook VLOOP test results */
  proslicTestObj	dcfeedVbatOffhook;  /**< Off-hook VBAT test results */
  proslicTestObj	dcfeedItipOffhook;  /**< Off-hook ITIP test results */
  proslicTestObj	dcfeedIringOffhook; /**< Off-hook IRING test results */
  proslicTestObj	dcfeedIloopOffhook; /**< Off-hook ILOOP test results */
  proslicTestObj	dcfeedIlongOffhook; /**< Off-hook ILONG test results */
  uInt8			testResult;		    /**< OR of all test results in this structure */
} proslicDcFeedTest;


/**
 * Defines structure for Ringing Test
 */
typedef struct
{
  BOOLEAN			testEnable;		    /**< Gate execution/updating of results with this flag */
  BOOLEAN			testDataValid;	    /**< Indicates if test data is valid (1) or stale (0) */
  BOOLEAN			abortIfLineInUse;   /**< Abort test if LCR set at the start of test. Leaves results invalid */
  uInt16			numSamples;         /**< Number of samples taken */
  uInt8
  sampleInterval;     /**< Sample interval (in ms - range 1 to 100) */
  BOOLEAN			ringtripTestEnable; /**< Enable ringtrip test */
  BOOLEAN			rtpStatus;          /**< RTP Bit */
  proslicTestObj	ringingVac;         /**< Ringing AC Voltage test results */
  proslicTestObj	ringingVdc;         /**< Ringing DC Voltage test results */
  uInt8			testResult;		    /**< OR of all test results in this structure */
} proslicRingingTest;

/**
 * Defines structure for Battery Test
 */
typedef struct
{
  BOOLEAN			testEnable;		/**< Gate execution/updating of results with this flag */
  BOOLEAN			testDataValid;	/**< Indicates if test data is valid (1) or stale (0) */
  proslicTestObj	vbat;           /**< VBAT test results */
  uInt8			testResult;		/**< OR of all test results in this structure */
} proslicBatteryTest;

/**
 * Defines structure for Audio Test
 */
typedef struct
{
  BOOLEAN			testEnable;		  /**< Gate execution/updating of results with this flag */
  BOOLEAN			testDataValid;	  /**< Indicates if test data is valid (1) or stale (0) */
  BOOLEAN			abortIfLineInUse; /**< Abort test if LCR set at the start of test. Leaves results invalid */
  int32
  zerodBm_mVpk;     /**< 0dBm voltage (in mVpk) of ref impedance */
  proslicTestObj	txGain;		      /**< TX path gain test results */
  proslicTestObj  rxGain;           /**< RX path gain test results */
  uInt8			testResult;		  /**< OR of all test results in this structure */
} proslicAudioTest;


/**
 * Defines structure for all tests
 */
typedef struct
{
  proslicPcmLpbkTest	pcmLpbkTest;
  proslicDcFeedTest	dcFeedTest;
  proslicRingingTest	ringingTest;
  proslicBatteryTest	batteryTest;
  proslicAudioTest	audioTest;
} proslicTestInObjType;

typedef proslicTestInObjType *proslicTestInObjType_ptr;


/**
 * @brief
 *  Allocate memory and initialize the given structure.
 *
 * @param[in,out] *pTstin - the structure to initialize
 * @param[in]     num_objs - number of Testin structures to allocate.
 *
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 * @sa ProSLIC_DestroyTestInObjs
 */
int ProSLIC_createTestInObjs(proslicTestInObjType_ptr *pTstin, uInt32 num_objs);
#define ProSLIC_createTestInObj(X) ProSLIC_createTestInObjs((X),1)

/**
 * @brief
 *  Free memory reserved by the given structure.
 *
 * @param[in,out] *pTstin - the structure to initialize
 *
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */
int ProSLIC_destroyTestInObjs(proslicTestInObjType_ptr *pTstin);
#define ProSLIC_destroyTestInObj ProSLIC_destroyTestInObjs

/**
 * @brief
 *  Enable PCM loopback.
 *
 * @param[in]      pProslic  -  channel data structure
 * @param[in,out]  pTstin - all control, limits, and results
 *
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */
int ProSLIC_testInPCMLpbkEnable(proslicChanType_ptr pProslic,
                                proslicTestInObjType_ptr pTstin);


/**
 * @brief
 *  Disable PCM loopback.
 *
 * @param[in]      pProslic  -  channel data structure
 * @param[in,out]  pTstin - all control, limits, and results
 *
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */
int ProSLIC_testInPCMLpbkDisable(proslicChanType_ptr pProslic,
                                 proslicTestInObjType_ptr pTstin);


/**
 * @brief
 * DC Feed Test - the pTstin->dcFeedTest contains control, limits and test result information.
 *
 * @param[in]      pProslic  -  channel data structure
 * @param[in,out]  pTstin - all control, limits, and results
 *
 * @retval int - error from @ref errorCodeType
 * @retval RC_TEST_PASSED indicates test completed and passed
 * @retval RC_TEST_FAILED indicates test completed and failed
 * @retval RC_UNSUPPORTED_FEATURE indicates feature not supported on this device
 * @retval RC_TEST_DISABLED indicates test has not been initialized/enabled
 * @retval RC_LINE_IN_USE indicates LCS already set
 *
 */
int ProSLIC_testInDCFeed(proslicChanType_ptr pProslic,
                         proslicTestInObjType_ptr pTstin);


/**
 * @brief
 * Ringing and Ringtrip Test
 *
 * @param[in]      pProslic  -  channel data structure
 * @param[in,out]  pTstin - all control, limits, and results
 *
 * @retval int - error from @ref errorCodeType
 * @retval RC_TEST_PASSED indicates test completed and passed
 * @retval RC_TEST_FAILED indicates test completed and failed
 * @retval RC_UNSUPPORTED_FEATURE indicates feature not supported on this device
 * @retval RC_TEST_DISABLED indicates test has not been initialized/enabled
 * @retval RC_LINE_IN_USE indicates LCS already set
 *
 */
int ProSLIC_testInRinging(proslicChanType_ptr pProslic,
                          proslicTestInObjType_ptr pTstin);

/**
 * @brief
 * Battery Test -
 * pTstin contains both the configuration data and test results.
 * See **pTstin->batteryTest.testResult - Pass/Fail result
 *
 * @param[in]      pProslic  -  channel data structure
 * @param[in,out]  pTstin - configuration and test results
 *
 * @retval int - error from @ref errorCodeType
 * @retval RC_TEST_PASSED indicates test completed and passed
 * @retval RC_TEST_FAILED indicates test completed and failed
 * @retval RC_UNSUPPORTED_FEATURE indicates feature not supported on this device
 * @retval RC_TEST_DISABLED indicates test has not been initialized/enabled
 *
 */
int ProSLIC_testInBattery(proslicChanType_ptr pProslic,
                          proslicTestInObjType_ptr pTstin);

/**
 * @brief
 * Audio level inward test.  pTstin->audioTest contains all control, limits and results.
 *
 * @param[in]      pProslic  -  channel data structure
 * @param[in,out]  pTstin - all control, limits, and results
 *
 * @retval int - error from @ref errorCodeType
 * @retval RC_TEST_PASSED indicates test completed and passed
 * @retval RC_TEST_FAILED indicates test completed and failed
 * @retval RC_UNSUPPORTED_FEATURE indicates feature not supported on this device
 * @retval RC_TEST_DISABLED indicates test has not been initialized/enabled
 * @retval RC_LINE_IN_USE indicates LCS already set
 *
 */
int ProSLIC_testInAudio(proslicChanType_ptr pProslic,
                        proslicTestInObjType_ptr pTstin);

/**
 * @brief
 *  Initialize/Enable PCM Loopback Test.  Links test config/limits
 *  to inward test data structure.
 *
 * @param[in,out]  pTstin->pcmLpbkTest - all control, limits, and results
 * @param[in]      pcmLpbkTest -  test config and limits to link to pTstin->pcmLpbkTest
 *
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */
int ProSLIC_testInPcmLpbkSetup(proslicTestInObjType *pTstin,
                               proslicPcmLpbkTest *pcmLpbkTest);

/**
 * @brief
 *  Initialize/Enable DC Feed Test.  Links test config/limits
 *  to inward test data structure.
 *
 * @param[in,out]  pTstin->dcFeedTest - all control, limits, and results
 * @param[in]      dcFeedTest -  test config and limits to link to pTstin->dcFeedTest
 *
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */
int ProSLIC_testInDcFeedSetup(proslicTestInObjType *pTstin,
                              proslicDcFeedTest *dcFeedTest);

/**
 * @brief
 *  Initialize/Enable Ringing Test.  Links test config/limits
 *  to inward test data structure.
 *
 * @param[in,out]  pTstin->ringingTest - all control, limits, and results
 * @param[in]      ringingTest -  test config and limits to link to pTstin->ringingTest
 *
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */
int ProSLIC_testInRingingSetup(proslicTestInObjType *pTstin,
                               proslicRingingTest *ringingTest);

/**
 * @brief
 *  Initialize/Enable Battery Test.  Links test config/limits
 *  to inward test data structure.
 *
 * @param[in,out]  pTstin->batteryTest - all control, limits, and results
 * @param[in]      batteryTest -  test config and limits to link to pTstin->batteryTest
 *
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */
int ProSLIC_testInBatterySetup(proslicTestInObjType *pTstin,
                               proslicBatteryTest *batteryTest);

/**
 * @brief
 *  Initialize/Enable Audio Test.  Links test config/limits
 *  to inward test data structure.
 *
 * @param[in,out]  pTstin->audioTest - all control, limits, and results
 * @param[in]      audioTest -  test config and limits to link to pTstin->audioTest
 *
 * @retval int - error from @ref errorCodeType  @ref RC_NONE indicates no error.
 */
int ProSLIC_testInAudioSetup(proslicTestInObjType *pTstin,
                             proslicAudioTest *audioTest);


/**
 * @brief
 *  Debug utility to log presently loaded test limits
 *
 * @param[in]      pProslic  -  channel data structure
 * @param[in]      pTstin - inward test data structure
 *
 * @retval int - error from @ref errorCodeType
 * @retval RC_NONE indicates no error.
 * @retval RC_UNSUPPORTED_FEATURE indicates feature not supported on this device
 */
int ProSLIC_testInPrintLimits(proslicChanType *pProslic,
                              proslicTestInObjType *pTstin);

/**
 * @brief
 *  Provide a simulated offhook using the ProSLIC test load.  You would call 
 *  this function twice: once to enable the simulated test load 
 *  (is_testStart = TRUE) and check if the upper layer correctly detected
 *  the hook state change and possibly pass some audio.  After you have
 *  completed this test/check, you would call the same function again to
 *  restore the system to normal state (is_testStart = FALSE).
 *
 * @note This test can only be executed one at a time.  Running this on
 *       multiple channels that have different DC Feed and digital control
 *       settings will cause problems. If the all the channels have the
 *       same settings, this restriction can be loosened.
 *       
 *
 * @param[in]      pProslic  -  channel data structure
 * @param[in]      is_testStart - is the the start or end of test.
 * @param[in]      timeDelayMsec - how may mSec to delay prior to returning from
                   the function.
 *
 * @retval int - error from @ref errorCodeType
 * @retval RC_NONE indicates no error.
 * @retval RC_UNSUPPORTED_FEATURE indicates feature not supported on this device
 */

int ProSLIC_testLoadTest(
  SiVoiceChanType_ptr pProslic, 
  BOOLEAN is_testStart, 
  uint32_t timeDelayMsec);

/** @} PROSLIC_TSTIN */
/** @} DIAGNOSTICS */
#endif

