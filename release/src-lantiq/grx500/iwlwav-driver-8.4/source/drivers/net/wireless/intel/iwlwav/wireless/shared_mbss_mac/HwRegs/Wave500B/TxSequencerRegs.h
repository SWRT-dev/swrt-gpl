
/***********************************************************************************
File:				TxSequencerRegs.h
Module:				txSequencer
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TX_SEQUENCER_REGS_H_
#define _TX_SEQUENCER_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TX_SEQUENCER_BASE_ADDRESS                             MEMORY_MAP_UNIT_68_BASE_ADDRESS
#define	REG_TX_SEQUENCER_SUCCESSFUL_RX_SEQUENCE_EN             (TX_SEQUENCER_BASE_ADDRESS + 0x0)
#define	REG_TX_SEQUENCER_OTHER_RX_SEQUENCE_EN                  (TX_SEQUENCER_BASE_ADDRESS + 0x4)
#define	REG_TX_SEQUENCER_RESP_TIMEOUT_SEQUENCE_EN              (TX_SEQUENCER_BASE_ADDRESS + 0x8)
#define	REG_TX_SEQUENCER_POST_TX_SEQUENCE_EN                   (TX_SEQUENCER_BASE_ADDRESS + 0xC)
#define	REG_TX_SEQUENCER_TX_END_ERR_SEQUENCE_EN                (TX_SEQUENCER_BASE_ADDRESS + 0x10)
#define	REG_TX_SEQUENCER_MINIMAL_SEQ_TIME_THRESHOLD            (TX_SEQUENCER_BASE_ADDRESS + 0x14)
#define	REG_TX_SEQUENCER_MINIMAL_SU_TXOP_TIME_THRESHOLD        (TX_SEQUENCER_BASE_ADDRESS + 0x18)
#define	REG_TX_SEQUENCER_MINIMAL_MU_TXOP_TIME_THRESHOLD        (TX_SEQUENCER_BASE_ADDRESS + 0x1C)
#define	REG_TX_SEQUENCER_MINIMAL_BF_SEQ_TIME_THRESHOLD         (TX_SEQUENCER_BASE_ADDRESS + 0x20)
#define	REG_TX_SEQUENCER_MINIMAL_CF_END_TIME_THRESHOLD         (TX_SEQUENCER_BASE_ADDRESS + 0x24)
#define	REG_TX_SEQUENCER_MAX_NUM_BF_SEQ_ITERATIONS             (TX_SEQUENCER_BASE_ADDRESS + 0x28)
#define	REG_TX_SEQUENCER_TX_SEQ_DEL_RECIPE_POINTER             (TX_SEQUENCER_BASE_ADDRESS + 0x2C)
#define	REG_TX_SEQUENCER_TX_NAV_TIMER_SAMPLE_SIFS_COMP         (TX_SEQUENCER_BASE_ADDRESS + 0x30)
#define	REG_TX_SEQUENCER_CCA_SAMPLE_TIME_DELAY_TX              (TX_SEQUENCER_BASE_ADDRESS + 0x34)
#define	REG_TX_SEQUENCER_CCA_SAMPLE_TIME_DELAY_RX              (TX_SEQUENCER_BASE_ADDRESS + 0x38)
#define	REG_TX_SEQUENCER_CCA_SAMPLE_TIME_DELAY_TIMEOUT         (TX_SEQUENCER_BASE_ADDRESS + 0x3C)
#define	REG_TX_SEQUENCER_MAC_PHY_TX_TXOP_SIGNAL_EN             (TX_SEQUENCER_BASE_ADDRESS + 0x40)
#define	REG_TX_SEQUENCER_MAC_PHY_TX_TXOP_SIGNAL                (TX_SEQUENCER_BASE_ADDRESS + 0x44)
#define	REG_TX_SEQUENCER_BW_LIMITS                             (TX_SEQUENCER_BASE_ADDRESS + 0x48)
#define	REG_TX_SEQUENCER_SET_BW_LIMIT                          (TX_SEQUENCER_BASE_ADDRESS + 0x4C)
#define	REG_TX_SEQUENCER_SET_FILLER_RA_BF_RPT_BW_LOWER_EN      (TX_SEQUENCER_BASE_ADDRESS + 0x50)
#define	REG_TX_SEQUENCER_TX_SEQUENCER_IRQ_STATUS               (TX_SEQUENCER_BASE_ADDRESS + 0x54)
#define	REG_TX_SEQUENCER_TX_SEQUENCER_IRQ_EN                   (TX_SEQUENCER_BASE_ADDRESS + 0x58)
#define	REG_TX_SEQUENCER_TX_SEQUENCER_IRQ_CLR                  (TX_SEQUENCER_BASE_ADDRESS + 0x5C)
#define	REG_TX_SEQUENCER_TX_SEQUENCER_ERR_IRQ_STATUS           (TX_SEQUENCER_BASE_ADDRESS + 0x60)
#define	REG_TX_SEQUENCER_TX_SEQUENCER_ERR_IRQ_EN               (TX_SEQUENCER_BASE_ADDRESS + 0x64)
#define	REG_TX_SEQUENCER_TX_SEQUENCER_ERR_IRQ_CLR              (TX_SEQUENCER_BASE_ADDRESS + 0x68)
#define	REG_TX_SEQUENCER_CPU_RA_FILLER_IRQ                     (TX_SEQUENCER_BASE_ADDRESS + 0x6C)
#define	REG_TX_SEQUENCER_CPU_RA_FILLER_IRQ_EN                  (TX_SEQUENCER_BASE_ADDRESS + 0x70)
#define	REG_TX_SEQUENCER_CPU_RA_FILLER_IRQ_CLR                 (TX_SEQUENCER_BASE_ADDRESS + 0x74)
#define	REG_TX_SEQUENCER_BF_RPT_VALID                          (TX_SEQUENCER_BASE_ADDRESS + 0x78)
#define	REG_TX_SEQUENCER_BF_RPT_VALID_CLR                      (TX_SEQUENCER_BASE_ADDRESS + 0x7C)
#define	REG_TX_SEQUENCER_AGG_REACT_PSDU_DUR_LIMIT_ADD          (TX_SEQUENCER_BASE_ADDRESS + 0x80)
#define	REG_TX_SEQUENCER_AGG_REACT_STATUS                      (TX_SEQUENCER_BASE_ADDRESS + 0x84)
#define	REG_TX_SEQUENCER_SEQUENCER_FIFO_BASE_ADDR              (TX_SEQUENCER_BASE_ADDRESS + 0x88)
#define	REG_TX_SEQUENCER_SEQUENCER_FIFO_DEPTH_MINUS_ONE        (TX_SEQUENCER_BASE_ADDRESS + 0x8C)
#define	REG_TX_SEQUENCER_SEQUENCER_FIFO_CLEAR_STRB             (TX_SEQUENCER_BASE_ADDRESS + 0x90)
#define	REG_TX_SEQUENCER_SEQUENCER_FIFO_RD_ENTRIES_NUM         (TX_SEQUENCER_BASE_ADDRESS + 0x94)
#define	REG_TX_SEQUENCER_SEQUENCER_FIFO_NUM_ENTRIES_COUNT      (TX_SEQUENCER_BASE_ADDRESS + 0x98)
#define	REG_TX_SEQUENCER_SEQUENCER_FIFO_DEBUG                  (TX_SEQUENCER_BASE_ADDRESS + 0x9C)
#define	REG_TX_SEQUENCER_SEQUENCER_CCA_CONTROL                 (TX_SEQUENCER_BASE_ADDRESS + 0xA0)
#define	REG_TX_SEQUENCER_SEQUENCER_LOGGER                      (TX_SEQUENCER_BASE_ADDRESS + 0xA4)
#define	REG_TX_SEQUENCER_SEQUENCER_LOGGER_ACTIVE               (TX_SEQUENCER_BASE_ADDRESS + 0xA8)
#define	REG_TX_SEQUENCER_SEQUENCER_DEBUG_SM                    (TX_SEQUENCER_BASE_ADDRESS + 0xAC)
#define	REG_TX_SEQUENCER_SEQUENCER_DEBUG                       (TX_SEQUENCER_BASE_ADDRESS + 0xB0)
#define	REG_TX_SEQUENCER_SEQUENCER_PLANNED_EXECUTED_DEBUG      (TX_SEQUENCER_BASE_ADDRESS + 0xB4)
#define	REG_TX_SEQUENCER_CCA_SAMPLE_TIMER                      (TX_SEQUENCER_BASE_ADDRESS + 0xB8)
#define	REG_TX_SEQUENCER_USP_INDEX                             (TX_SEQUENCER_BASE_ADDRESS + 0xBC)
#define	REG_TX_SEQUENCER_BAA_NEEDED_SET                        (TX_SEQUENCER_BASE_ADDRESS + 0xC0)
#define	REG_TX_SEQUENCER_MAX_NUM_MULT_USERS_TRAINING           (TX_SEQUENCER_BASE_ADDRESS + 0xC4)
#define	REG_TX_SEQUENCER_SEQUENCER_REPORT_SENDER_BITS          (TX_SEQUENCER_BASE_ADDRESS + 0xC8)
#define	REG_TX_SEQUENCER_ABORT_WAIT_PHY_MULT_USERS_TRAINING    (TX_SEQUENCER_BASE_ADDRESS + 0xCC)
#define	REG_TX_SEQUENCER_PHY_SAMPLE_INFO_DEBUG_EN              (TX_SEQUENCER_BASE_ADDRESS + 0xD0)
#define	REG_TX_SEQUENCER_PHY_PAC_TX_PHY_MU_EFFECTIVE_RATE      (TX_SEQUENCER_BASE_ADDRESS + 0xD4)
#define	REG_TX_SEQUENCER_PHY_PAC_TX_PHY_TX_TYPE                (TX_SEQUENCER_BASE_ADDRESS + 0xD8)
#define	REG_TX_SEQUENCER_BF_MU_DB_INDEX                        (TX_SEQUENCER_BASE_ADDRESS + 0xDC)
#define	REG_TX_SEQUENCER_SEQUENCER_END_IRQ                     (TX_SEQUENCER_BASE_ADDRESS + 0xE0)
#define	REG_TX_SEQUENCER_SEQUENCER_END_IRQ_CLR                 (TX_SEQUENCER_BASE_ADDRESS + 0xE4)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TX_SEQUENCER_SUCCESSFUL_RX_SEQUENCE_EN 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successActivateBaaEn:1;	// Enable BAA activation
		uint32 successVerifyCcaPrimaryEn:1;	// Enable cancel Tx due to CCA primary 20MHz busy
		uint32 successVerifyCcaSecondaryEn:1;	// Enable verify CCA secondary
		uint32 successUpdateDynamicBwEn:1;	// Enable update BW based on PHY status service bits
		uint32 successBfIterationEn:1;	// Enable BF iteration
		uint32 successClrBfReqTxSelBitmapEn:1;	// Enable clear of BF request in Tx Selector BitMAP
		uint32 successAggBuilderReactivationEn:1;	// Enable AGG builder reactivation
		uint32 successAggBuilderReactivationForce:1;	// Force AGG builder reactivation
		uint32 successAggBuilderReactivationForceRdDb:1;	// Force AGG builder reactivation based on DB
		uint32 successVerifySifsLimitPreAggBuilderEn:1;	// Enable verify SIFS limit pre AGG Builder operation
		uint32 successTriggerTxReqEn:1;	// Enable triggering Tx request
		uint32 successFillRateAdaptiveRptEn:1;	// Enable filling rate adaptive report
		uint32 successTriggerCfEndForce:1;	// Force CF-end triggering
		uint32 successTriggerCfEndEn:1;	// Enable CF-end triggering
		uint32 successTxSelBitmapUnlockEn:1;	// Enable unlock queue in Tx selector BitMAP
		uint32 successTxhMapUnfreezeEn:1;	// Enable TXH MAP unfreeze
		uint32 successGenCpuIntEn:1;	// Generate interrupt to CPU
		uint32 successClrTxNavTimerEn:1;	// Enable clear Tx NAV timer
		uint32 successVerifyMinSeqTimeEn:1;	// Enable verify minimum sequence time threshold
		uint32 successWaitPhyMultUsersTrainingEn:1;	// Enable waiting PHY completes its operation on MU BF DB
		uint32 successManageBfPollEn:1;	// Enable handling BF poll1 bit and STD “Next BF report”
		uint32 reserved0:11;
	} bitFields;
} RegTxSequencerSuccessfulRxSequenceEn_u;

/*REG_TX_SEQUENCER_OTHER_RX_SEQUENCE_EN 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 otherActivateBaaEn:1;	// Enable BAA activation
		uint32 otherVerifyCcaPrimaryEn:1;	// Enable cancel Tx due to CCA primary 20MHz busy
		uint32 otherVerifyCcaSecondaryEn:1;	// Enable verify CCA secondary
		uint32 otherUpdateDynamicBwEn:1;	// Enable update BW based on PHY status service bits
		uint32 otherBfIterationEn:1;	// Enable BF iteration
		uint32 otherClrBfReqTxSelBitmapEn:1;	// Enable clear of BF request in Tx Selector BitMAP
		uint32 otherAggBuilderReactivationEn:1;	// Enable AGG builder reactivation
		uint32 otherAggBuilderReactivationForce:1;	// Force AGG builder reactivation
		uint32 otherAggBuilderReactivationForceRdDb:1;	// Force AGG builder reactivation based on DB
		uint32 otherVerifySifsLimitPreAggBuilderEn:1;	// Enable verify SIFS limit pre AGG Builder operation
		uint32 otherTriggerTxReqEn:1;	// Enable triggering Tx request
		uint32 otherFillRateAdaptiveRptEn:1;	// Enable filling rate adaptive report
		uint32 otherTriggerCfEndForce:1;	// Force CF-end triggering.
		uint32 otherTriggerCfEndEn:1;	// Enable CF-end triggering
		uint32 otherTxSelBitmapUnlockEn:1;	// Enable unlock queue in Tx selector BitMAP
		uint32 otherTxhMapUnfreezeEn:1;	// Enable TXH MAP unfreeze
		uint32 otherGenCpuIntEn:1;	// Generate interrupt to CPU
		uint32 otherClrTxNavTimerEn:1;	// Enable clear Tx NAV timer
		uint32 otherVerifyMinSeqTimeEn:1;	// Enable verify minimum sequence time threshold
		uint32 otherWaitPhyMultUsersTrainingEn:1;	// Enable waiting PHY completes its operation on MU BF DB
		uint32 otherManageBfPollEn:1;	// Enable handling BF poll1 bit and STD “Next BF report”
		uint32 reserved0:11;
	} bitFields;
} RegTxSequencerOtherRxSequenceEn_u;

/*REG_TX_SEQUENCER_RESP_TIMEOUT_SEQUENCE_EN 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timeoutActivateBaaEn:1;	// Enable BAA activation
		uint32 timeoutVerifyCcaPrimaryEn:1;	// Enable cancel Tx due to CCA primary 20MHz busy
		uint32 timeoutVerifyCcaSecondaryEn:1;	// Enable verify CCA secondary
		uint32 timeoutUpdateDynamicBwEn:1;	// Enable update BW based on PHY status service bits
		uint32 timeoutBfIterationEn:1;	// Enable BF iteration
		uint32 timeoutClrBfReqTxSelBitmapEn:1;	// Enable clear of BF request in Tx Selector BitMAP
		uint32 timeoutAggBuilderReactivationEn:1;	// Enable AGG builder reactivation
		uint32 timeoutAggBuilderReactivationForce:1;	// Force AGG builder reactivation
		uint32 timeoutAggBuilderReactivationForceRdDb:1;	// Force AGG builder reactivation based on DB
		uint32 timeoutVerifySifsLimitPreAggBuilderEn:1;	// Enable verify SIFS limit pre AGG Builder operation
		uint32 timeoutTriggerTxReqEn:1;	// Enable triggering Tx request
		uint32 timeoutFillRateAdaptiveRptEn:1;	// Enable filling rate adaptive report
		uint32 timeoutTriggerCfEndForce:1;	// Force CF-end triggering.
		uint32 timeoutTriggerCfEndEn:1;	// Enable CF-end triggering
		uint32 timeoutTxSelBitmapUnlockEn:1;	// Enable unlock queue in Tx selector BitMAP
		uint32 timeoutTxhMapUnfreezeEn:1;	// Enable TXH MAP unfreeze
		uint32 timeoutGenCpuIntEn:1;	// Generate interrupt to CPU
		uint32 timeoutClrTxNavTimerEn:1;	// Enable clear Tx NAV timer
		uint32 timeoutVerifyMinSeqTimeEn:1;	// Enable verify minimum sequence time threshold
		uint32 timeoutWaitPhyMultUsersTrainingEn:1;	// Enable waiting PHY completes its operation on MU BF DB
		uint32 timeoutManageBfPollEn:1;	// Enable handling BF poll1 bit and STD “Next BF report”
		uint32 reserved0:11;
	} bitFields;
} RegTxSequencerRespTimeoutSequenceEn_u;

/*REG_TX_SEQUENCER_POST_TX_SEQUENCE_EN 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 postTxActivateBaaEn:1;	// Enable BAA activation
		uint32 postTxVerifyCcaPrimaryEn:1;	// Enable cancel Tx due to CCA primary 20MHz busy
		uint32 postTxVerifyCcaSecondaryEn:1;	// Enable verify CCA secondary
		uint32 postTxUpdateDynamicBwEn:1;	// Enable update BW based on PHY status service bits
		uint32 postTxBfIterationEn:1;	// Enable BF iteration
		uint32 postTxClrBfReqTxSelBitmapEn:1;	// Enable clear of BF request in Tx Selector BitMAP
		uint32 postTxAggBuilderReactivationEn:1;	// Enable AGG builder reactivation
		uint32 postTxAggBuilderReactivationForce:1;	// Force AGG builder reactivation
		uint32 postTxAggBuilderReactivationForceRdDb:1;	// Force AGG builder reactivation based on DB
		uint32 postTxVerifySifsLimitPreAggBuilderEn:1;	// Enable verify SIFS limit pre AGG Builder operation
		uint32 postTxTriggerTxReqEn:1;	// Enable triggering Tx request
		uint32 postTxFillRateAdaptiveRptEn:1;	// Enable filling rate adaptive report
		uint32 postTxTriggerCfEndForce:1;	// Force CF-end triggering.
		uint32 postTxTriggerCfEndEn:1;	// Enable CF-end triggering
		uint32 postTxTxSelBitmapUnlockEn:1;	// Enable unlock queue in Tx selector BitMAP
		uint32 postTxTxhMapUnfreezeEn:1;	// Enable TXH MAP unfreeze
		uint32 postTxGenCpuIntEn:1;	// Generate interrupt to CPU
		uint32 postTxClrTxNavTimerEn:1;	// Enable Clear Tx NAV timer
		uint32 postTxVerifyMinSeqTimeEn:1;	// Enable verify minimum sequence time threshold
		uint32 postTxWaitPhyMultUsersTrainingEn:1;	// Enable waiting PHY completes its operation on MU BF DB
		uint32 postTxManageBfPollEn:1;	// Enable handling BF poll1 bit and STD “Next BF report”
		uint32 reserved0:11;
	} bitFields;
} RegTxSequencerPostTxSequenceEn_u;

/*REG_TX_SEQUENCER_TX_END_ERR_SEQUENCE_EN 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cca20PBusyTriggerCfEndEn:1;	// Enable CF-end triggering, , In case of CCA 20MHz primary is busy.
		uint32 cca20PBusyTxSelBitmapUnlockEn:1;	// Enable unlock queue in Tx selector BitMAP, , In case of CCA 20MHz primary is busy
		uint32 cca20PBusyTxhMapUnfreezeEn:1;	// Enable TXH MAP unfreeze, , In case of CCA 20MHz primary is busy
		uint32 cca20PBusyGenCpuIntEn:1;	// Generate interrupt to CPU, , In case of CCA 20MHz primary is busy
		uint32 cca20PBusyClrTxNavTimerEn:1;	// Enable clear Tx NAV timer, , In case of CCA 20MHz primary is busy.
		uint32 baaErrTriggerCfEndEn:1;	// Enable CF-end triggering, , In case of BAA error
		uint32 baaErrTxSelBitmapUnlockEn:1;	// Enable unlock queue in Tx selector BitMAP, , In case of BAA error
		uint32 baaErrTxhMapUnfreezeEn:1;	// Enable TXH MAP unfreeze, , In case of BAA error
		uint32 baaErrGenCpuIntEn:1;	// Generate interrupt to CPU, , In case of BAA error
		uint32 baaErrClrTxNavTimerEn:1;	// Enable clear Tx NAV timer, , In case of BAA error.
		uint32 aggErrTriggerCfEndEn:1;	// Enable CF-end triggering, , In case of AGG builder error
		uint32 aggErrTxSelBitmapUnlockEn:1;	// Enable unlock queue in Tx selector BitMAP, , In case of AGG builder error
		uint32 aggErrTxhMapUnfreezeEn:1;	// Enable TXH MAP unfreeze, , In case of AGG builder error
		uint32 aggErrGenCpuIntEn:1;	// Generate interrupt to CPU, , In case of AGG builder error
		uint32 aggErrClrTxNavTimerEn:1;	// Enable clear Tx NAV timer, , In case of AGG builder error
		uint32 noFillerTriggerCfEndEn:1;	// Enable CF-end triggering, , In case of no filler supported, but there is a need to add a filler
		uint32 noFillerTxSelBitmapUnlockEn:1;	// Enable unlock queue in Tx selector BitMAP, , In case of no filler supported, but there is a need to add a filler
		uint32 noFillerTxhMapUnfreezeEn:1;	// Enable TXH MAP unfreeze, , In case of no filler supported, but there is a need to add a filler
		uint32 noFillerGenCpuIntEn:1;	// Generate interrupt to CPU, , In case of no filler supported, but there is a need to add a filler
		uint32 noFillerClrTxNavTimerEn:1;	// Enable clear Tx NAV timer, , In case of no filler supported, but there is a need to add a filler
		uint32 reserved0:12;
	} bitFields;
} RegTxSequencerTxEndErrSequenceEn_u;

/*REG_TX_SEQUENCER_MINIMAL_SEQ_TIME_THRESHOLD 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minimalSeqTimeThreshold:18;	// Minimal time threshold for continue sequence flow, resolution [us].
		uint32 reserved0:14;
	} bitFields;
} RegTxSequencerMinimalSeqTimeThreshold_u;

/*REG_TX_SEQUENCER_MINIMAL_SU_TXOP_TIME_THRESHOLD 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minimalSuTxopTimeThreshold:18;	// Minimal time threshold for continue TXOP multiple main transmission sequence flow in SU, resolution [us].
		uint32 reserved0:14;
	} bitFields;
} RegTxSequencerMinimalSuTxopTimeThreshold_u;

/*REG_TX_SEQUENCER_MINIMAL_MU_TXOP_TIME_THRESHOLD 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minimalMuTxopTimeThreshold:18;	// Minimal time threshold for continue TXOP multiple main transmission sequence flow in MU, resolution [us].
		uint32 reserved0:14;
	} bitFields;
} RegTxSequencerMinimalMuTxopTimeThreshold_u;

/*REG_TX_SEQUENCER_MINIMAL_BF_SEQ_TIME_THRESHOLD 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minimalBfSeqTimeThreshold:18;	// Minimal time threshold for iteration of BF sequence, resolution [us].
		uint32 reserved0:14;
	} bitFields;
} RegTxSequencerMinimalBfSeqTimeThreshold_u;

/*REG_TX_SEQUENCER_MINIMAL_CF_END_TIME_THRESHOLD 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minimalCfEndTimeThreshold:18;	// Minimal time threshold for CF-end transmission, resolution [us].
		uint32 reserved0:14;
	} bitFields;
} RegTxSequencerMinimalCfEndTimeThreshold_u;

/*REG_TX_SEQUENCER_MAX_NUM_BF_SEQ_ITERATIONS 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxNumBfSeqIterations:2;	// Maximum number of BF sequence iterations minus one
		uint32 reserved0:30;
	} bitFields;
} RegTxSequencerMaxNumBfSeqIterations_u;

/*REG_TX_SEQUENCER_TX_SEQ_DEL_RECIPE_POINTER 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSeqDelRecipePointer:22;	// Tx Sequencer to Delia recipe pointer (byte address)
		uint32 reserved0:10;
	} bitFields;
} RegTxSequencerTxSeqDelRecipePointer_u;

/*REG_TX_SEQUENCER_TX_NAV_TIMER_SAMPLE_SIFS_COMP 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNavTimerSamplePostTxOfdmSifsComp:4;	// Time in micro seconds from âPHY-MAC Tx ready signal fallâ till next Delia start, when the last Tx was in PHY mode of OFDM
		uint32 txNavTimerSamplePostTx11BSifsComp:4;	// Time in micro seconds from âPHY-MAC Tx ready signal fallâ till next Delia start, when the last Tx was in PHY mode of 11B
		uint32 txNavTimerSamplePostRxOfdmSifsComp:4;	// Time in micro seconds from "CCA 20p becomes free (after Rx session)" till next Delia start, when the last Rx was in PHY mode of OFDM
		uint32 txNavTimerSamplePostRx11BSifsComp:4;	// Time in micro seconds from "CCA 20p becomes free (after Rx session)" till next Delia start, when the last Rx was in PHY mode of 11B
		uint32 txNavTimerSamplePostTimeoutSifsComp:4;	// Time in micro seconds from âResponse timeout expiredâ till next Delia start.
		uint32 reserved0:12;
	} bitFields;
} RegTxSequencerTxNavTimerSampleSifsComp_u;

/*REG_TX_SEQUENCER_CCA_SAMPLE_TIME_DELAY_TX 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaSampleTimeDelayTxOfdm:14;	// Time delay in cycles from the PHY-MAC "Tx ready" signal fall (when the PHY mode of last Tx frame is OFDM) till when to sample CCA signals
		uint32 reserved0:2;
		uint32 ccaSampleTimeDelayTx11B:14;	// Time delay in cycles from the PHY-MAC "Tx ready" signal fall (when the PHY mode of last Tx frame is 11B) till when to sample CCA signals
		uint32 reserved1:2;
	} bitFields;
} RegTxSequencerCcaSampleTimeDelayTx_u;

/*REG_TX_SEQUENCER_CCA_SAMPLE_TIME_DELAY_RX 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaSampleTimeDelayRxOfdm:14;	// Time delay in cycles from "CCA 20p becomes free (after Rx session)"  (when the PHY mode of last Rx frame is OFDM) till when to sample CCA signals
		uint32 reserved0:2;
		uint32 ccaSampleTimeDelayRx11B:14;	// Time delay in cycles from "CCA 20p becomes free (after Rx session)"  (when the PHY mode of last Rx frame is 11B) till when to sample CCA signals
		uint32 reserved1:2;
	} bitFields;
} RegTxSequencerCcaSampleTimeDelayRx_u;

/*REG_TX_SEQUENCER_CCA_SAMPLE_TIME_DELAY_TIMEOUT 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaSampleTimeDelayTimeout:14;	// Time delay in cycles from timeout expired event till when to sample CCA signals
		uint32 reserved0:18;
	} bitFields;
} RegTxSequencerCcaSampleTimeDelayTimeout_u;

/*REG_TX_SEQUENCER_MAC_PHY_TX_TXOP_SIGNAL_EN 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macPhyTxTxopSignalEn:1;	// Enable settings the MAC-PHY Tx TXOP signal
		uint32 macPhyTxTxopSignalDebugOverrideEn:1;	// Enable override the MAC-PHY Tx TXOP signal
		uint32 macPhyTxTxopSignalDebugOverrideVal:1;	// The override value of the MAC-PHY Tx TXOP signal
		uint32 reserved0:29;
	} bitFields;
} RegTxSequencerMacPhyTxTxopSignalEn_u;

/*REG_TX_SEQUENCER_MAC_PHY_TX_TXOP_SIGNAL 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macPhyTxTxopSignal:1;	// MAC-PHY Tx TXOP signal
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerMacPhyTxTxopSignal_u;

/*REG_TX_SEQUENCER_BW_LIMITS 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptMimoBw:2;	// BW limit based on BF report MIMO control, used only in case of SU
		uint32 rxPhyStatusDynamicBwLimit:2;	// BW limit based on dynamic/static BW
		uint32 lastGoodAggBwLimit:2;	// Current BW limit based on AGG builder activation
		uint32 reserved0:26;
	} bitFields;
} RegTxSequencerBwLimits_u;

/*REG_TX_SEQUENCER_SET_BW_LIMIT 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setBwLimit:2;	// Set BW limit in the middle of TXOP
		uint32 reserved0:30;
	} bitFields;
} RegTxSequencerSetBwLimit_u;

/*REG_TX_SEQUENCER_SET_FILLER_RA_BF_RPT_BW_LOWER_EN 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setFillerRaBfRptBwLowerEn:1;	// Enable settings Filler of rate adaptive on Rx BF report with BW lower than planned
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerSetFillerRaBfRptBwLowerEn_u;

/*REG_TX_SEQUENCER_TX_SEQUENCER_IRQ_STATUS 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successRxHdrBfRptIrq:1;	// Successful Rx BF report header IRQ
		uint32 successRxBfRptIrq:1;	// Successful Rx BF report IRQ
		uint32 successRxCtsIrq:1;	// Successful Rx CTS IRQ
		uint32 successRxAckIrq:1;	// Successful Rx ACK IRQ
		uint32 successRxBaIrq:1;	// Successful Rx BA IRQ
		uint32 otherRxIrq:1;	// Other Rx IRQ
		uint32 respTimeoutRxIrq:1;	// Response timeout Rx IRQ
		uint32 postTxIrq:1;	// Post Tx IRQ
		uint32 cca20PBusyIrq:1;	// Stop due CCA 20MHz primary busy IRQ
		uint32 baaErrIrq:1;	// Stop due BA analyzer error IRQ
		uint32 aggErrIrq:1;	// Stop due AGG builder error IRQ
		uint32 noFillerSupportIrq:1;	// Stop due to no filler support IRQ
		uint32 reserved0:20;
	} bitFields;
} RegTxSequencerTxSequencerIrqStatus_u;

/*REG_TX_SEQUENCER_TX_SEQUENCER_IRQ_EN 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successRxHdrBfRptIrqEn:1;	// Successful Rx BF report header IRQ enable
		uint32 successRxBfRptIrqEn:1;	// Successful Rx BF report IRQ enable
		uint32 successRxCtsIrqEn:1;	// Successful Rx CTS IRQ enable
		uint32 successRxAckIrqEn:1;	// Successful Rx ACK IRQ enable
		uint32 successRxBaIrqEn:1;	// Successful Rx BA IRQ enable
		uint32 otherRxIrqEn:1;	// Other Rx IRQ enable
		uint32 respTimeoutRxIrqEn:1;	// Response timeout Rx IRQ enable
		uint32 postTxIrqEn:1;	// Post Tx IRQ enable
		uint32 cca20PBusyIrqEn:1;	// Stop due CCA 20MHz primary busy IRQ enable
		uint32 baaErrIrqEn:1;	// Stop due BA analyzer error IRQ enable
		uint32 aggErrIrqEn:1;	// Stop due AGG builder error IRQ enable
		uint32 noFillerSupportIrqEn:1;	// Stop due to no filler support IRQ enable
		uint32 reserved0:20;
	} bitFields;
} RegTxSequencerTxSequencerIrqEn_u;

/*REG_TX_SEQUENCER_TX_SEQUENCER_IRQ_CLR 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successRxHdrBfRptIrqClr:1;	// Successful Rx BF report header IRQ clear
		uint32 successRxBfRptIrqClr:1;	// Successful Rx BF report IRQ clear
		uint32 successRxCtsIrqClr:1;	// Successful Rx CTS IRQ clear
		uint32 successRxAckIrqClr:1;	// Successful Rx ACK IRQ clear
		uint32 successRxBaIrqClr:1;	// Successful Rx BA IRQ clear
		uint32 otherRxIrqClr:1;	// Other Rx IRQ clear
		uint32 respTimeoutRxIrqClr:1;	// Response timeout Rx IRQ clear
		uint32 postTxIrqClr:1;	// Post Tx IRQ clear
		uint32 cca20PBusyIrqClr:1;	// Stop due CCA 20MHz primary busy IRQ clear
		uint32 baaErrIrqClr:1;	// Stop due BA analyzer error IRQ clear
		uint32 aggErrIrqClr:1;	// Stop due AGG builder error IRQ clear
		uint32 noFillerSupportIrqClr:1;	// Stop due to no filler support IRQ clear
		uint32 reserved0:20;
	} bitFields;
} RegTxSequencerTxSequencerIrqClr_u;

/*REG_TX_SEQUENCER_TX_SEQUENCER_ERR_IRQ_STATUS 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compOwnerErrIrq:1;	// Compare owner bit failed IRQ
		uint32 sequencerFifoFullIrq:1;	// Sequencer drop entry due to FIFO full IRQ
		uint32 sequencerFifoDecrementLessThanZeroIrq:1;	// Sequencer FIFO amount has been decremented less than zero IRQ
		uint32 reserved0:29;
	} bitFields;
} RegTxSequencerTxSequencerErrIrqStatus_u;

/*REG_TX_SEQUENCER_TX_SEQUENCER_ERR_IRQ_EN 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compOwnerErrIrqEn:1;	// Compare owner bit failed IRQ enable
		uint32 sequencerFifoFullIrqEn:1;	// Sequencer drop entry due to FIFO full IRQ enable
		uint32 sequencerFifoDecrementLessThanZeroIrqEn:1;	// Sequencer FIFO amount has been decremented less than zero IRQ enable
		uint32 reserved0:29;
	} bitFields;
} RegTxSequencerTxSequencerErrIrqEn_u;

/*REG_TX_SEQUENCER_TX_SEQUENCER_ERR_IRQ_CLR 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compOwnerErrIrqClr:1;	// Compare owner bit failed IRQ clear, by write '1'
		uint32 sequencerFifoFullIrqClr:1;	// Sequencer drop entry due to FIFO full IRQ clear, by write '1'
		uint32 sequencerFifoDecrementLessThanZeroIrqClr:1;	// Sequencer FIFO amount has been decremented less than zero IRQ clear, by write '1'
		uint32 reserved0:29;
	} bitFields;
} RegTxSequencerTxSequencerErrIrqClr_u;

/*REG_TX_SEQUENCER_CPU_RA_FILLER_IRQ 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuRaFillerIrq:1;	// CPU Rate adaptive filler IRQ
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerCpuRaFillerIrq_u;

/*REG_TX_SEQUENCER_CPU_RA_FILLER_IRQ_EN 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuRaFillerIrqEn:1;	// CPU Rate adaptive filler IRQ enable
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerCpuRaFillerIrqEn_u;

/*REG_TX_SEQUENCER_CPU_RA_FILLER_IRQ_CLR 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuRaFillerIrqClr:1;	// CPU Rate adaptive filler IRQ clear
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerCpuRaFillerIrqClr_u;

/*REG_TX_SEQUENCER_BF_RPT_VALID 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptSenderValid:1;	// BF report Sender valid bit
		uint32 bfRptCpuValid:1;	// BF report CPU valid bit
		uint32 bfRptSequencerValid:1;	// BF report Sequencer valid bit
		uint32 reserved0:29;
	} bitFields;
} RegTxSequencerBfRptValid_u;

/*REG_TX_SEQUENCER_BF_RPT_VALID_CLR 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptSenderValidClr:1;	// BF report Sender valid bit clear
		uint32 bfRptCpuValidClr:1;	// BF report CPU valid bit clear
		uint32 reserved0:30;
	} bitFields;
} RegTxSequencerBfRptValidClr_u;

/*REG_TX_SEQUENCER_AGG_REACT_PSDU_DUR_LIMIT_ADD 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggReactPsduDurLimitAdd:10;	// Add time in [us] for the remaining PSDU duration limit in case of AGG builder reactivation
		uint32 reserved0:22;
	} bitFields;
} RegTxSequencerAggReactPsduDurLimitAdd_u;

/*REG_TX_SEQUENCER_AGG_REACT_STATUS 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggErrDataAMsduCtrStop:1;	// AGG reactivate error on PD type data - can't construct even one MPDU due to A-MSDU counter limit
		uint32 aggErrDataMsduSizeStop:1;	// AGG reactivate error on PD type data - can't construct even one MPDU due to MSDU size limit
		uint32 aggErrDataMpduSizeDbStop:1;	// AGG reactivate error on PD type data - can't construct even one MPDU due to MPDU size limit based on DB
		uint32 aggErrDataPsduSizeStop:1;	// AGG reactivate error on PD type data - can't construct even one MPDU due to PSDU size limit
		uint32 aggErrDataPsduSizeEstimatedStop:1;	// AGG reactivate error on PD type data - can't construct even one MPDU due to PSDU size estimated limit (based on remaining Tx NAV timer)
		uint32 aggErrDataNotRetriesMpdusCountStop:1;	// AGG reactivate error on PD type data - can't construct even one MPDU due to not retries MPDUs count limit
		uint32 aggErrNoDataPsduSizeEstimatedStop:1;	// AGG reactivate error on not PD type data - can't construct even one MPDU due to PSDU size estimated limit (based on remaining Tx NAV timer)
		uint32 aggErrDataMpduSizeConfigStop:1;	// AGG reactivate error on PD type data - can't construct even one MPDU due to MPDU size limit based on configuration per PHY mode
		uint32 aggErrDataPsduSecondarySizePrimaryEndStop:1;	// AGG reactivate error on PD type data - can't construct even one MPDU due to PSDU size secondary limit when primary end
		uint32 reserved0:7;
		uint32 aggReactRequiredRaFiller:1;	// AGG reactivate required to set rate adaptive filler due to BF report
		uint32 aggReactFirstPdNull:1;	// AGG reactivate stop due to first PD is NULL
		uint32 aggReactLessMinTimer:1;	// AGG reactivate operation time is less than minimum 
		uint32 aggReactDone:1;	// AGG reactivation has been completed
		uint32 aggReactUspPrimaryDrop:1;	// AGG reactivate stop due to Primary USP indicated as Drop
		uint32 aggReactMuPrimaryNotDataPd:1;	// AGG reactivate stop due to MU Primary USP first PD is not data type
		uint32 reserved1:10;
	} bitFields;
} RegTxSequencerAggReactStatus_u;

/*REG_TX_SEQUENCER_SEQUENCER_FIFO_BASE_ADDR 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sequencerFifoBaseAddr:24;	// Sequencer FIFO base address (byte address)
		uint32 reserved0:8;
	} bitFields;
} RegTxSequencerSequencerFifoBaseAddr_u;

/*REG_TX_SEQUENCER_SEQUENCER_FIFO_DEPTH_MINUS_ONE 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sequencerFifoDepthMinusOne:10;	// Sequencer FIFO depth minus one (boundary of entries, each entry is 12 bytes)
		uint32 reserved0:22;
	} bitFields;
} RegTxSequencerSequencerFifoDepthMinusOne_u;

/*REG_TX_SEQUENCER_SEQUENCER_FIFO_CLEAR_STRB 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 sequencerFifoClearFullDropCtrStrb:1;	// Clear Sequencer fifo full drop counter, by write '1'
		uint32 sequencerFifoClearStrb:1;	// Clear Sequencer FIFO by write '1'
		uint32 reserved1:29;
	} bitFields;
} RegTxSequencerSequencerFifoClearStrb_u;

/*REG_TX_SEQUENCER_SEQUENCER_FIFO_RD_ENTRIES_NUM 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sequencerFifoRdEntriesNum:10;	// Sequencer FIFO number of entries to decrement
		uint32 reserved0:22;
	} bitFields;
} RegTxSequencerSequencerFifoRdEntriesNum_u;

/*REG_TX_SEQUENCER_SEQUENCER_FIFO_NUM_ENTRIES_COUNT 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sequencerFifoNumEntriesCount:11;	// Sequencer FIFO number of entries count
		uint32 reserved0:21;
	} bitFields;
} RegTxSequencerSequencerFifoNumEntriesCount_u;

/*REG_TX_SEQUENCER_SEQUENCER_FIFO_DEBUG 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sequencerFifoWrPtr:10;	// Sequencer FIFO write pointer entry index
		uint32 reserved0:6;
		uint32 sequencerFifoNotEmpty:1;	// Sequencer FIFO not empty indication
		uint32 sequencerFifoFull:1;	// Sequencer FIFO full indication
		uint32 reserved1:6;
		uint32 sequencerFifoFullDropCtr:8;	// Sequencer FIFO full drop counter
	} bitFields;
} RegTxSequencerSequencerFifoDebug_u;

/*REG_TX_SEQUENCER_SEQUENCER_CCA_CONTROL 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 forceDisable11BSampleCca:1;	// Force disable sample CCA signals in case the PHY mode is 11B by Sequencer
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerSequencerCcaControl_u;

/*REG_TX_SEQUENCER_SEQUENCER_LOGGER 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sequencerLoggerEn:1;	// Sequencer logger enable
		uint32 reserved0:7;
		uint32 sequencerLoggerPriority:2;	// Sequencer logger priority
		uint32 reserved1:22;
	} bitFields;
} RegTxSequencerSequencerLogger_u;

/*REG_TX_SEQUENCER_SEQUENCER_LOGGER_ACTIVE 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sequencerLoggerActive:1;	// Sequencer logger active
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerSequencerLoggerActive_u;

/*REG_TX_SEQUENCER_SEQUENCER_DEBUG_SM 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 seqMainSm:6;	// Sequencer main state machine
		uint32 seqBfValidSm:2;	// BF valid state machine
		uint32 senderConfigSm:3;	// Sender configuration state machine
		uint32 bfWaitSm:2;	// BF wait state machine
		uint32 txselUnlockIfSm:4;	// Tx Selector unlock interface state machine
		uint32 reserved0:15;
	} bitFields;
} RegTxSequencerSequencerDebugSm_u;

/*REG_TX_SEQUENCER_SEQUENCER_DEBUG 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 seqBaaRaReportCnt:6;	// Number of BAA rate adaptive reports from the start of Tx sequence
		uint32 baaSeqPsReqCleared:4;	// BAA cleared PS request, 1 bit per USP
		uint32 timeoutEventSticky:4;	// Timeout event sticky, 1 bit per USP
		uint32 otherRxEventSticky:4;	// Other Rx event sticky, 1 bit per USP
		uint32 reserved0:14;
	} bitFields;
} RegTxSequencerSequencerDebug_u;

/*REG_TX_SEQUENCER_SEQUENCER_PLANNED_EXECUTED_DEBUG 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stdPlannedFlow:16;	// STD planned flow bits, read at the start of Tx sequencer operation
		uint32 stdExecutedFlow:16;	// STD executed flow bits, read at the start of Tx sequencer operation
	} bitFields;
} RegTxSequencerSequencerPlannedExecutedDebug_u;

/*REG_TX_SEQUENCER_CCA_SAMPLE_TIMER 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaSampleTimer:14;	// CCA sample timer
		uint32 reserved0:18;
	} bitFields;
} RegTxSequencerCcaSampleTimer_u;

/*REG_TX_SEQUENCER_USP_INDEX 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uspIndex:2;	// Sender configuration of USP index for the use of BAA and Sequencer 
		uint32 reserved0:30;
	} bitFields;
} RegTxSequencerUspIndex_u;

/*REG_TX_SEQUENCER_BAA_NEEDED_SET 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaNeededSet:4;	// Set the BAA needed bits, 1 bit per USP
		uint32 reserved0:12;
		uint32 baaNeeded:4;	// BAA needed bits, 1 bit per USP
		uint32 reserved1:12;
	} bitFields;
} RegTxSequencerBaaNeededSet_u;

/*REG_TX_SEQUENCER_MAX_NUM_MULT_USERS_TRAINING 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxNumMultUsersTraining:4;	// Maximum number of users during MU multiple users training (minus one)
		uint32 reserved0:28;
	} bitFields;
} RegTxSequencerMaxNumMultUsersTraining_u;

/*REG_TX_SEQUENCER_SEQUENCER_REPORT_SENDER_BITS 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sequencerReportSenderBits:32;	// Sender bits for Sequencer report
	} bitFields;
} RegTxSequencerSequencerReportSenderBits_u;

/*REG_TX_SEQUENCER_ABORT_WAIT_PHY_MULT_USERS_TRAINING 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 abortWaitPhyMultUsersTraining:1;	// Abort waiting PHY completse its operation on MU BF DB at the end of MU multiple users training sequence
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerAbortWaitPhyMultUsersTraining_u;

/*REG_TX_SEQUENCER_PHY_SAMPLE_INFO_DEBUG_EN 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phySampleInfoDebugEn:1;	// Enable sample PHY Tx type and effective rate for debug
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerPhySampleInfoDebugEn_u;

/*REG_TX_SEQUENCER_PHY_PAC_TX_PHY_MU_EFFECTIVE_RATE 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPacTxPhyMuEffectiveRate:16;	// PHY Tx MU effective rate
		uint32 reserved0:16;
	} bitFields;
} RegTxSequencerPhyPacTxPhyMuEffectiveRate_u;

/*REG_TX_SEQUENCER_PHY_PAC_TX_PHY_TX_TYPE 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPacTxPhyTxType:2;	// PHY Tx type
		uint32 reserved0:30;
	} bitFields;
} RegTxSequencerPhyPacTxPhyTxType_u;

/*REG_TX_SEQUENCER_BF_MU_DB_INDEX 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfMuDbIndex:4;	// Sender configuration of MU BF DB index for the use of MU BF parser
		uint32 reserved0:28;
	} bitFields;
} RegTxSequencerBfMuDbIndex_u;

/*REG_TX_SEQUENCER_SEQUENCER_END_IRQ 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sequencerEndIrq:1;	// Sequencer end IRQ
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerSequencerEndIrq_u;

/*REG_TX_SEQUENCER_SEQUENCER_END_IRQ_CLR 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sequencerEndIrqClr:1;	// Sequencer end IRQ clear
		uint32 reserved0:31;
	} bitFields;
} RegTxSequencerSequencerEndIrqClr_u;



#endif // _TX_SEQUENCER_REGS_H_
