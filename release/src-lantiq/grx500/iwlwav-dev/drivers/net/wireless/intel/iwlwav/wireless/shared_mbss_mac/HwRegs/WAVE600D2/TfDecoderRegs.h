
/***********************************************************************************
File:				TfDecoderRegs.h
Module:				tfDecoder
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TF_DECODER_REGS_H_
#define _TF_DECODER_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TF_DECODER_BASE_ADDRESS                             MEMORY_MAP_UNIT_14_BASE_ADDRESS
#define	REG_TF_DECODER_TF_DEC_PARAMS_0                                (TF_DECODER_BASE_ADDRESS + 0x0)
#define	REG_TF_DECODER_TF_DEC_PARAMS_1                                (TF_DECODER_BASE_ADDRESS + 0x4)
#define	REG_TF_DECODER_TF_DEC_PARAMS_2                                (TF_DECODER_BASE_ADDRESS + 0x8)
#define	REG_TF_DECODER_TF_DEC_TCR_ANTENNA_PARAMS                      (TF_DECODER_BASE_ADDRESS + 0xC)
#define	REG_TF_DECODER_TF_DEC_TCR_BASE_ADDRESS                        (TF_DECODER_BASE_ADDRESS + 0x10)
#define	REG_TF_DECODER_TF_DEC_CONTROL_BITS                            (TF_DECODER_BASE_ADDRESS + 0x20)
#define	REG_TF_DECODER_TF_DEC_LOGGER_CONTROL                          (TF_DECODER_BASE_ADDRESS + 0x40)
#define	REG_TF_DECODER_TF_DEC_LOGGER_STATUS                           (TF_DECODER_BASE_ADDRESS + 0x44)
#define	REG_TF_DECODER_TF_DEC_INT_ERROR_STATUS                        (TF_DECODER_BASE_ADDRESS + 0x50)
#define	REG_TF_DECODER_TF_DEC_INT_ERROR_EN                            (TF_DECODER_BASE_ADDRESS + 0x54)
#define	REG_TF_DECODER_TF_DEC_INT_ERROR_CLEAR                         (TF_DECODER_BASE_ADDRESS + 0x58)
#define	REG_TF_DECODER_DBG_TF_DEC_STATS_CONTROL                       (TF_DECODER_BASE_ADDRESS + 0x60)
#define	REG_TF_DECODER_DBG_RXC_TF_COUNT_0                             (TF_DECODER_BASE_ADDRESS + 0x64)
#define	REG_TF_DECODER_DBG_RXC_TF_COUNT_1                             (TF_DECODER_BASE_ADDRESS + 0x68)
#define	REG_TF_DECODER_DBG_TF_DEC_STATS                               (TF_DECODER_BASE_ADDRESS + 0x6C)
#define	REG_TF_DECODER_DBG_TF_DEC_TF_DB_COMMON_INFO_0                 (TF_DECODER_BASE_ADDRESS + 0x80)
#define	REG_TF_DECODER_DBG_TF_DEC_TF_DB_COMMON_INFO_1                 (TF_DECODER_BASE_ADDRESS + 0x84)
#define	REG_TF_DECODER_DBG_TF_DEC_TF_DB_COMMON_GENERAL_USER_INFO_0    (TF_DECODER_BASE_ADDRESS + 0x88)
#define	REG_TF_DECODER_DBG_TF_DEC_TF_DB_GENERAL_USER_INFO_0           (TF_DECODER_BASE_ADDRESS + 0x8C)
#define	REG_TF_DECODER_DBG_TF_DEC_TF_DB_DEPENDENT_USER_INFO           (TF_DECODER_BASE_ADDRESS + 0x90)
#define	REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_0                (TF_DECODER_BASE_ADDRESS + 0x94)
#define	REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_1                (TF_DECODER_BASE_ADDRESS + 0x98)
#define	REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_2                (TF_DECODER_BASE_ADDRESS + 0x9C)
#define	REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_3                (TF_DECODER_BASE_ADDRESS + 0xA0)
#define	REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_4                (TF_DECODER_BASE_ADDRESS + 0xA4)
#define	REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_5                (TF_DECODER_BASE_ADDRESS + 0xA8)
#define	REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_6                (TF_DECODER_BASE_ADDRESS + 0xAC)
#define	REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_7                (TF_DECODER_BASE_ADDRESS + 0xB0)
#define	REG_TF_DECODER_TF_DEC_SPARE_REGISTER                          (TF_DECODER_BASE_ADDRESS + 0xD0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TF_DECODER_TF_DEC_PARAMS_0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 systemBw : 2; //System BW configuration. It is used for verifying that the UL BW (which is extracted from the TF) is never greater than the System BW. It is coded as follows: 0 - 20MHz; 1 - 40MHz; 2 -ÂÂ 80MHz; 3 -ÂÂ 160MHz, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 primary20 : 3; //Primary 20 Sub-Band configuration. It represents the spectral location of the Primary Channel within the full BW supported by the system. It is used by the RU Conversion process. It is the SW responsibility to configure legal values (documneted in the spec), reset value: 0x0, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegTfDecoderTfDecParams0_u;

/*REG_TF_DECODER_TF_DEC_PARAMS_1 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 resourceRequestBufferThresholdExp : 8; //Exponent of Threshold used for generating FEEDBACK_STATUS field within the TCR in case of NFRP. This number is the exponent. The actual Threshold in bytes is 2^(Resource Request Buffer Threshold Exp), reset value: 0x8, access type: RW
		uint32 nfrpSupport : 1; //This configuration defines whether NFRP is supported. If NFRP TF is received, but the configuration is disabled, then the TF shall be aborted, reset value: 0x1, access type: RW
		uint32 multiTidInfoCalcFullModeMyAid : 1; //This configuration controls the analysis of TID field in case of Multi-TID MU-BAR. It is used only when the encountered AID matches the AID to search for and when TID > 7. , 0 - TID analysis is done based on TID[2:0] (TID[3] is ignored) , 1 - TID analysis is done based on TID[3:0], reset value: 0x1, access type: RW
		uint32 multiTidInfoGt7Abort : 1; //This configuration controls the analysis of TID_INFO field in case of Multi-TID MU-BAR, when TID_INFO > 7. It is used when the encountered AID is different than the AID to search for. , 0 -TF Decoding continues to the next user (if available), based on multi_tid_info_calc_full_mode configuration , 1 -If TID_INFO > 7, then the current TF is aborted, reset value: 0x0, access type: RW
		uint32 multiTidInfoCalcFullMode : 1; //This configuration controls the analysis of TID_INFO field in case of Multi-TID MU-BAR, when TID_INFO > 7 and MULTI_TID_INFO_GT7_ABORT is 0. It is used when the encountered AID is different than the AID to search for. , 0 - Jumping to the next user is done based on TID_INFO[2:0] , 1 - Jumping to the next user is done based on TID_INFO[3:0], reset value: 0x1, access type: RW
		uint32 unicastCheckMyAid : 1; //This configuration is used in case of Unicast. It controls whether to compare the extracted AID (from the TF) against MyAID in case of Unicast. , 0 - The extracted AID is ignored and not compared against MyAID. Processing continues normally. , 1 - The extracted AID is compared against MyAID. In case of a mismatch, the current TF is aborted, reset value: 0x1, access type: RW
		uint32 rfMaxOffset : 8; //This field designates the offset from MAX Tx Power within the TX Power Calculation process. It is used in case a defualt TX Power is selected. The format is identical to that of the TX Power Table: , - 8bit, Signed value , - dBm units, LSB=0.5dB, reset value: 0x30, access type: RW
		uint32 applyDefaultRfPower : 1; //Enable bit for using Default RF Power (within TX Power Calculation process). , 0 -ÂÂ TF Decoder applies RF Power in TCR according to UL Power calculations , 1 -ÂÂ TF Decoder applies RF Power in TCR according to Max_Tx_pwr value (MCS and BW dependent) from TX Power Table, reset value: 0x0, access type: RW
		uint32 applyCl : 1; //This field controls the CL value within the TCR. , 0 -ÂÂ TF Decoder shall apply Tx loop mode=0 (CL) in TCR , 1 - TF Decoder shall apply Tx loop mode=0 (CL) in TCR, reset value: 0x1, access type: RW
		uint32 applyTxBf : 1; //This field controls the TxBF value within the TCR. , 0 -ÂÂ TxBF is set to 0 , 1 - TxBF is calculated based on the STA DB parameters, reset value: 0x0, access type: RW
		uint32 forceTx : 8; //This is a bitmap configuration per Trigger Type. The Force_TX field within the TCR is updated with the value of this configuration, based on the Trigger Type. Note that only 8 bits are allocated, since Trigger Types of 8-15 are invalid., reset value: 0xff, access type: RW
	} bitFields;
} RegTfDecoderTfDecParams1_u;

/*REG_TF_DECODER_TF_DEC_PARAMS_2 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 totalNumOfSts : 3; //This field sets the threshold for (Starting SS + NSS) in case of UL MU-MIMO, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 maxNss : 2; //This field sets the threshold for NSS in case of both OFDMA and UL MU-MIMO, reset value: 0x3, access type: RW
		uint32 maxMcs : 2; //This field defines whether values of 12-13 are allowed (values of 0-11 are always legal).  0 -ÂÂ Max value is 11;   1 -ÂÂ Max value is 12;   2 -ÂÂ Max value is 13;   3 -ÂÂ Reserved, reset value: 0x0, access type: RW
		uint32 reserved1 : 24;
	} bitFields;
} RegTfDecoderTfDecParams2_u;

/*REG_TF_DECODER_TF_DEC_TCR_ANTENNA_PARAMS 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 antennaSelection : 16; //This field is used as the source of antenna_selection field within the TCR and also for calculating the Number of Tx Antennas (within Tx Power Calculation process). Each 2 bits designate an antenna, reset value: 0x0, access type: RW
		uint32 ant0Boost : 2; //This field is used as the source of ant0_boost field within the TCR, reset value: 0x0, access type: RW
		uint32 ant1Boost : 2; //This field is used as the source of ant1_boost field within the TCR, reset value: 0x0, access type: RW
		uint32 ant2Boost : 2; //This field is used as the source of ant2_boost field within the TCR, reset value: 0x0, access type: RW
		uint32 ant3Boost : 2; //This field is used as the source of ant3_boost field within the TCR, reset value: 0x0, access type: RW
		uint32 ant4Boost : 2; //This field is used as the source of ant4_boost field within the TCR, reset value: 0x0, access type: RW
		uint32 ant5Boost : 2; //This field is used as the source of ant5_boost field within the TCR, reset value: 0x0, access type: RW
		uint32 ant6Boost : 2; //This field is used as the source of ant6_boost field within the TCR, reset value: 0x0, access type: RW
		uint32 ant7Boost : 2; //This field is used as the source of ant7_boost field within the TCR, reset value: 0x0, access type: RW
	} bitFields;
} RegTfDecoderTfDecTcrAntennaParams_u;

/*REG_TF_DECODER_TF_DEC_TCR_BASE_ADDRESS 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrBaseAddress : 22; //Byte Address of the TCR location within the Tx Cyclic buffer, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegTfDecoderTfDecTcrBaseAddress_u;

/*REG_TF_DECODER_TF_DEC_CONTROL_BITS 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfDecFifoClear : 1; //Write 1 to this field in order to clear TF_DEC FIFO, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegTfDecoderTfDecControlBits_u;

/*REG_TF_DECODER_TF_DEC_LOGGER_CONTROL 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfDecLoggerEn : 1; //Logger Enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 tfDecLoggerPriority : 2; //tf_dec Logger priority, reset value: 0x0, access type: RW
		uint32 reserved1 : 26;
	} bitFields;
} RegTfDecoderTfDecLoggerControl_u;

/*REG_TF_DECODER_TF_DEC_LOGGER_STATUS 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfDecLoggerIdle : 1; //Indication that the logger is IDLE. Use this indication after the logger is disabled, in order to verify that it has finished any work in progress., reset value: 0x1, access type: RO
		uint32 tfDecLoggerSm : 3; //State of the logger state machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTfDecoderTfDecLoggerStatus_u;

/*REG_TF_DECODER_TF_DEC_INT_ERROR_STATUS 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfDecLengthErrorStatus : 1; //Incoming length is greater than declared MPDU Length , reset value: 0x0, access type: RO
		uint32 tfDecCompressedMuBarTidGt7Status : 1; //TID_INFO > 7 in case of Compressed MU-BAR. Asserted only in case  of FCS_OK, reset value: 0x0, access type: RO
		uint32 tfDecMultiTidMuBarTidGt7Status : 1; //TID_INFO > 7 in case of Multi-TID MU-BAR. This interrupt is set only if MyAID was found and MULTI_TID_INFO_CALC_FULL_MODE_MY_AID configuration is set. Asserted only in case  of FCS_OK, reset value: 0x0, access type: RO
		uint32 tfDecMultiTidMuBarTidNonUniqueStatus : 1; //The same TID arrived more than once in case of Multi-TID MU-BAR. This interrupt is set only if MyAID was found. If MULTI_TID_INFO_CALC_FULL_MODE_MY_AID configuration is not set, then duplication is identified based on identical TID[2:0] only. Asserted only in case  of FCS_OK, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTfDecoderTfDecIntErrorStatus_u;

/*REG_TF_DECODER_TF_DEC_INT_ERROR_EN 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfDecLengthErrorEn : 1; //Length Error interrupt enable, reset value: 0x0, access type: RW
		uint32 tfDecCompressedMuBarTidGt7En : 1; //Compressed MU-BAR TID GT7 interrupt enable, reset value: 0x0, access type: RW
		uint32 tfDecMultiTidMuBarTidGt7En : 1; //Multi-TID MU-BAR TID GT7 interrupt enable, reset value: 0x0, access type: RW
		uint32 tfDecMultiTidMuBarTidNonUniqueEn : 1; //Multi-TID MU-BAR TID Non-Unique interrupt enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegTfDecoderTfDecIntErrorEn_u;

/*REG_TF_DECODER_TF_DEC_INT_ERROR_CLEAR 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfDecLengthErrorClr : 1; //Length Error interrupt clear, reset value: 0x0, access type: WO
		uint32 tfDecCompressedMuBarTidGt7Clr : 1; //Compressed MU-BAR TID GT7 interrupt clear, reset value: 0x0, access type: WO
		uint32 tfDecMultiTidMuBarTidGt7Clr : 1; //Multi-TID MU-BAR TID GT7 interrupt clear, reset value: 0x0, access type: WO
		uint32 tfDecMultiTidMuBarTidNonUniqueClr : 1; //Multi-TID MU-BAR TID Non-Unique interrupt clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegTfDecoderTfDecIntErrorClear_u;

/*REG_TF_DECODER_DBG_TF_DEC_STATS_CONTROL 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcTfCountClear : 1; //Write 1 to this field in order to clear all rxc_tf_count counters, reset value: 0x0, access type: WO
		uint32 airTimeCalcAbortCountClear : 1; //Write 1 to this field in order to clear num_of_air_time_calculator_aborts counter, reset value: 0x0, access type: WO
		uint32 tfDecFifoOccupancyMaxClear : 1; //Write 1 to this field in order to reset tf_dec_fifo_occupancy_max, reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegTfDecoderDbgTfDecStatsControl_u;

/*REG_TF_DECODER_DBG_RXC_TF_COUNT_0 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcNumOfTotalFrames : 16; //Total number of frames that were received from the RXC. This counter is reset by writing 1 to rxc_tf_count_clear, reset value: 0x0, access type: RO
		uint32 rxcNumOfGoodFrames : 16; //Number of good frames (frames with FCS OK) that were received from the RXC. This counter is reset by writing 1 to rxc_tf_count_clear, reset value: 0x0, access type: RO
	} bitFields;
} RegTfDecoderDbgRxcTfCount0_u;

/*REG_TF_DECODER_DBG_RXC_TF_COUNT_1 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcNumOfBadFrames : 16; //Number of bad frames (frames with Bad FCS) that were received from the RXC. This counter is reset by writing 1 to rxc_tf_count_clear, reset value: 0x0, access type: RO
		uint32 rxcNumOfVerifiedFrames : 16; //Number of frames that were declared as valid by the TF Decoder out of the frames with FCS OK. This counter is reset by writing 1 to rxc_tf_count_clear, reset value: 0x0, access type: RO
	} bitFields;
} RegTfDecoderDbgRxcTfCount1_u;

/*REG_TF_DECODER_DBG_TF_DEC_STATS 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 numOfBadFcsBeforeGoodInRxSession : 12; //Number of bad frames (frames with Bad FCS) that were received from the RXC before receiving a good frame (frame with FCS OK) in each Rx session. This counter restarts every rx session (rx_session_post_status_stb restarts the counter), reset value: 0x0, access type: RO
		uint32 numOfAirTimeCalculatorAborts : 16; //Number of times the Air Time Calculator was aborted (i.e. clear command was issued while still processing). This counter is reset by writing 1 to air_time_calc_abort_count_clear, reset value: 0x0, access type: RO
		uint32 tfDecFifoOccupancyMax : 3; //Maximal occupancy of the TF Decoder input FIFO. This indication is reset by writing 1 to tf_dec_occupancy_max_clear, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
	} bitFields;
} RegTfDecoderDbgTfDecStats_u;

/*REG_TF_DECODER_DBG_TF_DEC_TF_DB_COMMON_INFO_0 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 triggerType : 4; //Trigger Type field within Common Info, reset value: 0x0, access type: RO
		uint32 ulLength : 12; //UL Length field within Common Info, reset value: 0x0, access type: RO
		uint32 moreTf : 1; //More TF field within Common Info, reset value: 0x0, access type: RO
		uint32 csRequired : 1; //CS Required field within Common Info, reset value: 0x0, access type: RO
		uint32 ulBw : 2; //UL BW field within Common Info, reset value: 0x0, access type: RO
		uint32 giLtfType : 2; //GI & LTF Type field within Common Info, reset value: 0x0, access type: RO
		uint32 muMimoLtfMode : 1; //MU-MIMO LTF Mode field within Common Info, reset value: 0x0, access type: RO
		uint32 numHeltfMidamblePeriodicity : 3; //Number Of HE-LTF Symbols And Midamble Periodicity field within Common Info, reset value: 0x0, access type: RO
		uint32 ulStbc : 1; //UL STBC field within Common Info, reset value: 0x0, access type: RO
		uint32 ldpcExtraSymbolSegment : 1; //LDPC Extra Symbol Segment field within Common Info, reset value: 0x0, access type: RO
		uint32 count0 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
	} bitFields;
} RegTfDecoderDbgTfDecTfDbCommonInfo0_u;

/*REG_TF_DECODER_DBG_TF_DEC_TF_DB_COMMON_INFO_1 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 apTxPower : 6; //AP Tx Power field within Common Info, reset value: 0x0, access type: RO
		uint32 preFecPaddingFactor : 2; //Pre-FEC Padding Factor field within Common Info, reset value: 0x0, access type: RO
		uint32 peDisambiguity : 1; //PE Disambiguity field within Common Info, reset value: 0x0, access type: RO
		uint32 ulSpatialReuse : 16; //UL Spatial Reuse field within Common Info, reset value: 0x0, access type: RO
		uint32 doppler : 1; //Doppler field within Common Info, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 count1 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
	} bitFields;
} RegTfDecoderDbgTfDecTfDbCommonInfo1_u;

/*REG_TF_DECODER_DBG_TF_DEC_TF_DB_COMMON_GENERAL_USER_INFO_0 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulHeSigA2Reserved : 9; //UL HE-SIG-A2 Reserved field within Common Info, reset value: 0x0, access type: RO
		uint32 aid12 : 12; //AID12 field within General User Info. In case of NFRP it represents Starting AID, reset value: 0x0, access type: RO
		uint32 ulFecCodingType : 1; // UL FEC Coding Type field within General User Info, reset value: 0x0, access type: RO
		uint32 ulMcs : 4; //UL MCS field within General User Info, reset value: 0x0, access type: RO
		uint32 ulDcm : 1; //UL DCM field within General User Info, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 count2 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
	} bitFields;
} RegTfDecoderDbgTfDecTfDbCommonGeneralUserInfo0_u;

/*REG_TF_DECODER_DBG_TF_DEC_TF_DB_GENERAL_USER_INFO_0 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ruAllocation : 8; //RU Allocation field within General User Info, reset value: 0x0, access type: RO
		uint32 startingSpatialStream : 3; //Starting Spatial tream field within General User Info, reset value: 0x0, access type: RO
		uint32 numberOfSpatialStreams : 3; //Number of Spatial Streams field within General User Info, reset value: 0x0, access type: RO
		uint32 ulTargetRssi : 7; //UL Target RSSI field within General User Info (also valid in case of NFRP), reset value: 0x0, access type: RO
		uint32 nfrpFeedbackType : 4; //NFRP Feedback Type field within General User Info. Valid in case of NFRP, reset value: 0x0, access type: RO
		uint32 nfrpMultiplexingFlag : 1; //NFRP Multiplexing Flag field within General User Info. Valid in case of NFRP, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 count3 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
	} bitFields;
} RegTfDecoderDbgTfDecTfDbGeneralUserInfo0_u;

/*REG_TF_DECODER_DBG_TF_DEC_TF_DB_DEPENDENT_USER_INFO 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 basicTriggerMpduMuSpacingFactor : 2; //MPDU MU Spacing Factor field within Trigger Dependent User info of Basic Trigger, reset value: 0x0, access type: RO
		uint32 basicTriggerTidAggLimit : 3; //TID Agg Limit field within Trigger Dependent User info of Basic Trigger, reset value: 0x0, access type: RO
		uint32 basicTriggerPreferredAc : 2; //Preferred AC field within Trigger Dependent User info of Basic Trigger, reset value: 0x0, access type: RO
		uint32 bfrpFbSgmntRetransmissionBitmap : 8; //Feedback Segment Retransmission Bitmap field within Trigger Dependent User info of BFRP, reset value: 0x0, access type: RO
		uint32 muBarControlBarAckPolicy : 1; //BAR Ack Policy field within Trigger Dependent User info of MU-BAR, reset value: 0x0, access type: RO
		uint32 muBarControlBarType : 4; //BAR Type field within Trigger Dependent User info of MU-BAR, reset value: 0x0, access type: RO
		uint32 muBarControlTidInfo : 4; //TID Info field within Trigger Dependent User info of MU-BAR, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
		uint32 count4 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
	} bitFields;
} RegTfDecoderDbgTfDecTfDbDependentUserInfo_u;

/*REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_0 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentNumber0 : 4; //Fragment Number field of TID 0 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 startingSequenceNumber0 : 12; //Starting Sequence Number field of TID 0 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 count5 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegTfDecoderDbgTfDecMuBarInformation0_u;

/*REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_1 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentNumber1 : 4; //Fragment Number field of TID 1 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 startingSequenceNumber1 : 12; //Starting Sequence Number field of TID 1 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 count6 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegTfDecoderDbgTfDecMuBarInformation1_u;

/*REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_2 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentNumber2 : 4; //Fragment Number field of TID 2 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 startingSequenceNumber2 : 12; //Starting Sequence Number field of TID 2 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 count7 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegTfDecoderDbgTfDecMuBarInformation2_u;

/*REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_3 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentNumber3 : 4; //Fragment Number field of TID 3 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 startingSequenceNumber3 : 12; //Starting Sequence Number field of TID 3 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 count8 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegTfDecoderDbgTfDecMuBarInformation3_u;

/*REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_4 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentNumber4 : 4; //Fragment Number field of TID 4 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 startingSequenceNumber4 : 12; //Starting Sequence Number field of TID 4 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 count9 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegTfDecoderDbgTfDecMuBarInformation4_u;

/*REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_5 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentNumber5 : 4; //Fragment Number field of TID 5 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 startingSequenceNumber5 : 12; //Starting Sequence Number field of TID 5 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 count10 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegTfDecoderDbgTfDecMuBarInformation5_u;

/*REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_6 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentNumber6 : 4; //Fragment Number field of TID 6 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 startingSequenceNumber6 : 12; //Starting Sequence Number field of TID 6 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 count11 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegTfDecoderDbgTfDecMuBarInformation6_u;

/*REG_TF_DECODER_DBG_TF_DEC_MU_BAR_INFORMATION_7 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentNumber7 : 4; //Fragment Number field of TID 7 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 startingSequenceNumber7 : 12; //Starting Sequence Number field of TID 7 within Bar Information of MU-BAR. Valid in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 count12 : 4; //Incremental counter of frames with FCS OK, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegTfDecoderDbgTfDecMuBarInformation7_u;

/*REG_TF_DECODER_TF_DEC_SPARE_REGISTER 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareRegister : 16; //Spare Register, reset value: 0x8888, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegTfDecoderTfDecSpareRegister_u;



#endif // _TF_DECODER_REGS_H_
