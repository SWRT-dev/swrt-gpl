
/***********************************************************************************
File:				TfDecoderRegs.h
Module:				TfDecoder
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

#define TF_DECODER_BASE_ADDRESS                             MEMORY_MAP_UNIT__BASE_ADDRESS
#define	REG_TF_DECODER_TF_DEC_PARAMS_0              (TF_DECODER_BASE_ADDRESS + 0x0)
#define	REG_TF_DECODER_TF_DEC_PARAMS_1              (TF_DECODER_BASE_ADDRESS + 0x4)
#define	REG_TF_DECODER_TF_DEC_PARAMS_2              (TF_DECODER_BASE_ADDRESS + 0x8)
#define	REG_TF_DECODER_TF_DEC_TCR_ANTENNA_PARAMS    (TF_DECODER_BASE_ADDRESS + 0xC)
#define	REG_TF_DECODER_TF_DEC_TCR_BASE_ADDRESS      (TF_DECODER_BASE_ADDRESS + 0x10)
#define	REG_TF_DECODER_TF_DEC_LOGGER_CONTROL        (TF_DECODER_BASE_ADDRESS + 0xA0)
#define	REG_TF_DECODER_TF_DEC_LOGGER_STATUS         (TF_DECODER_BASE_ADDRESS + 0xA4)
#define	REG_TF_DECODER_TF_DEC_SPARE_REGISTER        (TF_DECODER_BASE_ADDRESS + 0xB0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TF_DECODER_TF_DEC_PARAMS_0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 systemBw : 2; //System BW configuration. It is used for verifying that the UL BW (which is extracted from the TF) is never greater than the System BW. It is coded as follows: 0 - 20MHz; 1 - 40MHz; 2 -€“ 80MHz; 3 -€“ 160MHz, reset value: 0x0, access type: RW
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
		uint32 airTimeCalcAbortEnable : 1; //This configuration defines whether Reset can be issued towards the Air Time Calculator module in order to stop its operation during Abort procedure. , 0 -€“ Reset shall never be issued towards the Air Time Calculator. In case the Air Tome Calculator is busy during Abort, the TF Decoder shall wait until the Air Time Calculator finishes its operation in order to complete the Abort procedure. , 1 -€“ Reset shall be issued towards the Air Time Calculator (in case it is busy) during Abort, reset value: 0x1, access type: RW
		uint32 multiTidInfoGt7Abort : 1; //This configuration controls the analysis of TID_INFO field in case of Multi-TID MU-BAR, when TID_INFO > 7. It is used when the encountered AID is different than the AID to search for. , 0 -€“ TF Decoding continues to the next user (if available), based on multi_tid_info_calc_full_mode configuration , 1 -€“ If TID_INFO > 7, then the current TF is aborted, reset value: 0x0, access type: RW
		uint32 multiTidInfoCalcFullMode : 1; //This configuration controls the analysis of TID_INFO field in case of Multi-TID MU-BAR, when TID_INFO > 7 and MULTI_TID_INFO_GT7_ABORT is 0. It is used when the encountered AID is different than the AID to search for. , 0 -€“ Jumping to the next user is done based on TID_INFO[2:0] , 1 -€“ Jumping to the next user is done based on TID_INFO[3:0], reset value: 0x1, access type: RW
		uint32 rfMaxOffset : 8; //This field designates the offset from MAX Tx Power within the TX Power Calculation process. It is used in case a defualt TX Power is selected. The format is identical to that of the TX Power Table: , - 8bit, Signed value , - dBm units, LSB=0.5dB, reset value: 0x30, access type: RW
		uint32 applyDefaultRfPower : 1; //Enable bit for using Default RF Power (within TX Power Calculation process). , 0 -€“ TF Decoder applies RF Power in TCR according to UL Power calculations , 1 -€“ TF Decoder applies RF Power in TCR according to Max_Tx_pwr value (MCS and BW dependent) from TX Power Table, reset value: 0x0, access type: RW
		uint32 applyCl : 1; //This field controls the CL value within the TCR. , 0 -€“ TF Decoder shall apply Tx loop mode=0 (CL) in TCR , 1 - TF Decoder shall apply Tx loop mode=0 (CL) in TCR, reset value: 0x1, access type: RW
		uint32 applyTxBf : 1; //This field controls the TxBF value within the TCR. , 0 -€“ TxBF is set to 0 , 1 - TxBF is calculated based on the STA DB parameters, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
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
		uint32 maxMcs : 2; //This field defines whether values of 12-13 are allowed (values of 0-11 are always legal).  0 -€“ Max value is 11;   1 -€“ Max value is 12;   2 -€“ Max value is 13;   3 -€“ Reserved, reset value: 0x0, access type: RW
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

/*REG_TF_DECODER_TF_DEC_LOGGER_CONTROL 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfDecLoggerEn : 1; //Logger Enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 tfDecLoggerPriority : 2; //tf_dec Logger priority, reset value: 0x0, access type: RW
		uint32 reserved1 : 28;
	} bitFields;
} RegTfDecoderTfDecLoggerControl_u;

/*REG_TF_DECODER_TF_DEC_LOGGER_STATUS 0xA4 */
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

/*REG_TF_DECODER_TF_DEC_SPARE_REGISTER 0xB0 */
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
