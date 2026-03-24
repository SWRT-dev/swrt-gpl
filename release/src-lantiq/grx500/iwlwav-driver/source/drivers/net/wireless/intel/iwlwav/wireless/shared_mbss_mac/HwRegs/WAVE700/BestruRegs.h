
/***********************************************************************************
File:				BestruRegs.h
Module:				bestru
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _BESTRU_REGS_H_
#define _BESTRU_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define BESTRU_BASE_ADDRESS                             MEMORY_MAP_UNIT_66_BASE_ADDRESS
#define	REG_BESTRU_SW_CONFIG                 (BESTRU_BASE_ADDRESS + 0x0)
#define	REG_BESTRU_SW_REQUEST                (BESTRU_BASE_ADDRESS + 0x4)
#define	REG_BESTRU_SW_REQ_RU_MASK_L          (BESTRU_BASE_ADDRESS + 0x8)
#define	REG_BESTRU_SW_REQ_RU_MASK_H          (BESTRU_BASE_ADDRESS + 0xC)
#define	REG_BESTRU_SW_RESULT_RESP            (BESTRU_BASE_ADDRESS + 0x10)
#define	REG_BESTRU_SW_DBG                    (BESTRU_BASE_ADDRESS + 0x14)
#define	REG_BESTRU_SW_DBG_USR_VEC_L          (BESTRU_BASE_ADDRESS + 0x18)
#define	REG_BESTRU_SW_DBG_USR_VEC_H          (BESTRU_BASE_ADDRESS + 0x1C)
#define	REG_BESTRU_SW_DBG_CURR_PROC          (BESTRU_BASE_ADDRESS + 0x20)
#define	REG_BESTRU_SW_LUT1_MCS0              (BESTRU_BASE_ADDRESS + 0x24)
#define	REG_BESTRU_SW_LUT1_MCS1              (BESTRU_BASE_ADDRESS + 0x28)
#define	REG_BESTRU_SW_LUT1_MCS2              (BESTRU_BASE_ADDRESS + 0x2C)
#define	REG_BESTRU_SW_LUT1_MCS3              (BESTRU_BASE_ADDRESS + 0x30)
#define	REG_BESTRU_SW_LUT1_MCS4              (BESTRU_BASE_ADDRESS + 0x34)
#define	REG_BESTRU_SW_LUT1_MCS5              (BESTRU_BASE_ADDRESS + 0x38)
#define	REG_BESTRU_SW_LUT1_MCS6              (BESTRU_BASE_ADDRESS + 0x3C)
#define	REG_BESTRU_SW_LUT1_MCS7              (BESTRU_BASE_ADDRESS + 0x40)
#define	REG_BESTRU_SW_LUT1_MCS8              (BESTRU_BASE_ADDRESS + 0x44)
#define	REG_BESTRU_SW_LUT1_MCS9              (BESTRU_BASE_ADDRESS + 0x48)
#define	REG_BESTRU_SW_LUT1_MCS10             (BESTRU_BASE_ADDRESS + 0x4C)
#define	REG_BESTRU_SW_LUT1_MCS11             (BESTRU_BASE_ADDRESS + 0x50)
#define	REG_BESTRU_SW_LUT2_MCS0              (BESTRU_BASE_ADDRESS + 0x54)
#define	REG_BESTRU_SW_LUT2_MCS1              (BESTRU_BASE_ADDRESS + 0x58)
#define	REG_BESTRU_SW_LUT2_MCS2              (BESTRU_BASE_ADDRESS + 0x5C)
#define	REG_BESTRU_SW_LUT2_MCS3              (BESTRU_BASE_ADDRESS + 0x60)
#define	REG_BESTRU_SW_LUT2_MCS4              (BESTRU_BASE_ADDRESS + 0x64)
#define	REG_BESTRU_SW_LUT2_MCS5              (BESTRU_BASE_ADDRESS + 0x68)
#define	REG_BESTRU_SW_LUT2_MCS6              (BESTRU_BASE_ADDRESS + 0x6C)
#define	REG_BESTRU_SW_LUT2_MCS7              (BESTRU_BASE_ADDRESS + 0x70)
#define	REG_BESTRU_SW_LUT2_MCS8              (BESTRU_BASE_ADDRESS + 0x74)
#define	REG_BESTRU_SW_LUT2_MCS9              (BESTRU_BASE_ADDRESS + 0x78)
#define	REG_BESTRU_SW_LUT2_MCS10             (BESTRU_BASE_ADDRESS + 0x7C)
#define	REG_BESTRU_SW_LUT2_MCS11             (BESTRU_BASE_ADDRESS + 0x80)
#define	REG_BESTRU_SW_CALC_ASNR_DBG          (BESTRU_BASE_ADDRESS + 0x84)
#define	REG_BESTRU_SW_ASNR2MCS_LUT_SEL       (BESTRU_BASE_ADDRESS + 0x88)
#define	REG_BESTRU_SW_DBG_CALC_RES_L         (BESTRU_BASE_ADDRESS + 0x8C)
#define	REG_BESTRU_SW_DBG_CALC_RES_H         (BESTRU_BASE_ADDRESS + 0x90)
#define	REG_BESTRU_SW_DBG_FIFOS              (BESTRU_BASE_ADDRESS + 0x94)
#define	REG_BESTRU_BESTRU_LOGGER_CFG         (BESTRU_BASE_ADDRESS + 0x98)
#define	REG_BESTRU_BESTRU_LOGGER_BUSY        (BESTRU_BASE_ADDRESS + 0x9C)
#define	REG_BESTRU_SW_ABORT_CNT_RST_PULSE    (BESTRU_BASE_ADDRESS + 0xA0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_BESTRU_SW_CONFIG 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBestruEna : 1; //"1" enables the block. "0" disables the block after finishing curr user., reset value: 0x0, access type: RW
		uint32 swTsfShft : 3; //# of bits to shift right TSF10 (then 5 LSB taken) , tsf5_stamp      <= 5'(tsf10_stamp >> sw_tsf_shft);, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegBestruSwConfig_u;

/*REG_BESTRU_SW_REQUEST 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReq : 1; //Requests BRU result according to the "mode" (next reg). , Poll sw_resp_ready to know when done., reset value: 0x0, access type: WO
		uint32 swReqMode : 2; //"0"=provide single BestRU result in SW regs. "1"=copy STA entry to mirror. "2"=invalidate HDR (set 0) of the specified STA (@ disconnect or outdated TSF). , , reset value: 0x0, access type: RW
		uint32 swReqStaId : 8; //Requested STA_ID. , reset value: 0x0, access type: RW
		uint32 swReqRuSizeIdx : 3; //RU size index for “result_request” mode. , (0 = RU26, 1 = RU52, 2 = RU106, 3 = RU242, 4 = RU484, 5 = RU996) , , reset value: 0x0, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegBestruSwRequest_u;

/*REG_BESTRU_SW_REQ_RU_MASK_L 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqRuMaskL : 32; //bit_X="1" means the RU_X is occupied and shall be excluded from bestRU search., reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwReqRuMaskL_u;

/*REG_BESTRU_SW_REQ_RU_MASK_H 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqRuMaskH : 4; //bit_X="1" means the RU_(X+32) is occupied and shall be excluded from bestRU search., reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegBestruSwReqRuMaskH_u;

/*REG_BESTRU_SW_RESULT_RESP 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swRespReady : 1; //The result is ready to read. Poll this reg and check valid flag, next bit., reset value: 0x0, access type: RO
		uint32 swResultValid : 1; //If ‘0’: STA DB entry is not valid (never written or reset @ STA disconnect)., reset value: 0x0, access type: RO
		uint32 swResultBw : 2; //Bandwidth of the reported STA. , (0 = 20MHz, 1 = 40MHz, 2 = 80MHz, 3 = 160MHz) , , reset value: 0x0, access type: RO
		uint32 swResultRuIdx : 6; //“Best” RU index, allowing max rate, in the specific RU size tone plan., reset value: 0x0, access type: RO
		uint32 swResultMaxMcs : 4; //maxMCS related to the “best” RU., reset value: 0x0, access type: RO
		uint32 swResultMaxNss : 2; //maxNss related to the “best” RU., reset value: 0x0, access type: RO
		uint32 swResultTsf5 : 5; //Time-stamp of the last update to this STA entry., reset value: 0x0, access type: RO
		uint32 swBwRusizeError : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegBestruSwResultResp_u;

/*REG_BESTRU_SW_DBG 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mainSm : 2; //Main (top) state-machine, reset value: 0x0, access type: RO
		uint32 coreSm : 3; //Core state-machine (trig per user), reset value: 0x0, access type: RO
		uint32 swApiSm : 3; //SW API Server state-machine (trig by sw req), reset value: 0x0, access type: RO
		uint32 swApiBruSm : 2; //BestRU look-up state-machine, reset value: 0x0, access type: RO
		uint32 bfrRdSm : 2; //State of the BFR read logic block (SM), reset value: 0x0, access type: RO
		uint32 bruActive : 1; //"1" = the block isn't idle., reset value: 0x0, access type: RO
		uint32 swApiActive : 1; //"1" = SW request processing is active., reset value: 0x0, access type: RO
		uint32 swPending : 1; //this flag shows if there is a pending SW request, delayed cos of HW working on the same STA., reset value: 0x0, access type: RO
		uint32 hwPending : 1; //this flag shows if there is a pending HW process, delayed cos of copy the same STA entry for the SW., reset value: 0x0, access type: RO
		uint32 initAsnrSm : 2; //State of the ScratchPad initialization logic block (SM), reset value: 0x0, access type: RO
		uint32 abortCnt : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
	} bitFields;
} RegBestruSwDbg_u;

/*REG_BESTRU_SW_DBG_USR_VEC_L 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swDbgUsrVecL : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RO
	} bitFields;
} RegBestruSwDbgUsrVecL_u;

/*REG_BESTRU_SW_DBG_USR_VEC_H 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swDbgUsrVecH : 4; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegBestruSwDbgUsrVecH_u;

/*REG_BESTRU_SW_DBG_CURR_PROC 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swDbgCurrStaId : 8; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RO
		uint32 swDbgCurrUserId : 6; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegBestruSwDbgCurrProc_u;

/*REG_BESTRU_SW_LUT1_MCS0 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs0 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs0_u;

/*REG_BESTRU_SW_LUT1_MCS1 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs1 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs1_u;

/*REG_BESTRU_SW_LUT1_MCS2 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs2 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs2_u;

/*REG_BESTRU_SW_LUT1_MCS3 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs3 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs3_u;

/*REG_BESTRU_SW_LUT1_MCS4 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs4 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs4_u;

/*REG_BESTRU_SW_LUT1_MCS5 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs5 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs5_u;

/*REG_BESTRU_SW_LUT1_MCS6 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs6 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs6_u;

/*REG_BESTRU_SW_LUT1_MCS7 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs7 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs7_u;

/*REG_BESTRU_SW_LUT1_MCS8 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs8 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs8_u;

/*REG_BESTRU_SW_LUT1_MCS9 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs9 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs9_u;

/*REG_BESTRU_SW_LUT1_MCS10 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs10 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs10_u;

/*REG_BESTRU_SW_LUT1_MCS11 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut1Mcs11 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut1Mcs11_u;

/*REG_BESTRU_SW_LUT2_MCS0 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs0 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs0_u;

/*REG_BESTRU_SW_LUT2_MCS1 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs1 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs1_u;

/*REG_BESTRU_SW_LUT2_MCS2 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs2 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs2_u;

/*REG_BESTRU_SW_LUT2_MCS3 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs3 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs3_u;

/*REG_BESTRU_SW_LUT2_MCS4 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs4 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs4_u;

/*REG_BESTRU_SW_LUT2_MCS5 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs5 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs5_u;

/*REG_BESTRU_SW_LUT2_MCS6 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs6 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs6_u;

/*REG_BESTRU_SW_LUT2_MCS7 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs7 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs7_u;

/*REG_BESTRU_SW_LUT2_MCS8 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs8 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs8_u;

/*REG_BESTRU_SW_LUT2_MCS9 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs9 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs9_u;

/*REG_BESTRU_SW_LUT2_MCS10 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs10 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs10_u;

/*REG_BESTRU_SW_LUT2_MCS11 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLut2Mcs11 : 32; //aSNR-to-maxMCS LUT configuration, reset value: 0x0, access type: RW
	} bitFields;
} RegBestruSwLut2Mcs11_u;

/*REG_BESTRU_SW_CALC_ASNR_DBG 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 calcAsnrSm : 2; //no description, reset value: 0x0, access type: RO
		uint32 ruRdCnt : 6; //no description, reset value: 0x0, access type: RO
		uint32 wrAddrCnt : 6; //no description, reset value: 0x0, access type: RO
		uint32 wrRuSizeCnt : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegBestruSwCalcAsnrDbg_u;

/*REG_BESTRU_SW_ASNR2MCS_LUT_SEL 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swAsnr2McsLutSel : 6; //Bit0 - LUT select for RU26 , Bit1 - LUT select for RU52 , Bit2 - LUT select for RU106 , Bit3 - LUT select for RU242 , Bit4 - LUT select for RU484 , Bit5 - LUT select for RU996, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegBestruSwAsnr2McsLutSel_u;

/*REG_BESTRU_SW_DBG_CALC_RES_L 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swDbgCalcResSm : 3; //no description, reset value: 0x0, access type: RO
		uint32 swDbgCalcResRuSizeCnt : 3; //no description, reset value: 0x0, access type: RO
		uint32 swDbgCalcResRuCnt : 6; //no description, reset value: 0x0, access type: RO
		uint32 swDbgCalcResWrCnt : 4; //no description, reset value: 0x0, access type: RO
		uint32 swDbgCalcResLastRdAddr : 6; //no description, reset value: 0x0, access type: RO
		uint32 swDbgCalcResScratchRdAddr : 6; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
	} bitFields;
} RegBestruSwDbgCalcResL_u;

/*REG_BESTRU_SW_DBG_CALC_RES_H 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swDbgCalcResDbWrAddr : 11; //no description, reset value: 0x0, access type: RO
		uint32 swDbgCalcResLowSnr : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegBestruSwDbgCalcResH_u;

/*REG_BESTRU_SW_DBG_FIFOS 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swDbgBfrRdFifo : 8; //{smc_ena,started,start_pending,was_pop_empty,was_push_full,full,empty}, reset value: 0x01, access type: RO
		uint32 swDbgCalcResFifo : 8; //{smc_ena,started,start_pending,was_pop_empty,was_push_full,full,empty}, reset value: 0x01, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegBestruSwDbgFifos_u;

/*REG_BESTRU_BESTRU_LOGGER_CFG 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLoggerEna : 1; //no description, reset value: 0x0, access type: RW
		uint32 swLoggerPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegBestruBestruLoggerCfg_u;

/*REG_BESTRU_BESTRU_LOGGER_BUSY 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLoggerBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegBestruBestruLoggerBusy_u;

/*REG_BESTRU_SW_ABORT_CNT_RST_PULSE 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swAbortCntRstPulse : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegBestruSwAbortCntRstPulse_u;



#endif // _BESTRU_REGS_H_
