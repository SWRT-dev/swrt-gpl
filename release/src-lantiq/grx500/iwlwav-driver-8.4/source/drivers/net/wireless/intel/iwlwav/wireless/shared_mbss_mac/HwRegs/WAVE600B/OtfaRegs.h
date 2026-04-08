
/***********************************************************************************
File:				OtfaRegs.h
Module:				Otfa
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _OTFA_REGS_H_
#define _OTFA_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define OTFA_BASE_ADDRESS                             MEMORY_MAP_UNIT_6_BASE_ADDRESS
#define	REG_OTFA_GENERAL_CONTROL                     (OTFA_BASE_ADDRESS + 0x0)
#define	REG_OTFA_RETRY_BUFFER_INDEX                  (OTFA_BASE_ADDRESS + 0x4)
#define	REG_OTFA_ZLDS_CONTROL_1                      (OTFA_BASE_ADDRESS + 0x8)
#define	REG_OTFA_ZLDS_CONTROL_2                      (OTFA_BASE_ADDRESS + 0xC)
#define	REG_OTFA_STOP_BUILDING                       (OTFA_BASE_ADDRESS + 0x10)
#define	REG_OTFA_BEACON_TIM_OVERHEAD_LENGTH          (OTFA_BASE_ADDRESS + 0x14)
#define	REG_OTFA_ETH_TYPE_OVERHEAD                   (OTFA_BASE_ADDRESS + 0x18)
#define	REG_OTFA_VAP_IS_STA                          (OTFA_BASE_ADDRESS + 0x20)
#define	REG_OTFA_PD_ERR_ENABLE                       (OTFA_BASE_ADDRESS + 0x24)
#define	REG_OTFA_AVG_MPDU_LENGTH_MIN_TH              (OTFA_BASE_ADDRESS + 0x30)
#define	REG_OTFA_MAX_MPDU_LENGTH_11AG                (OTFA_BASE_ADDRESS + 0x34)
#define	REG_OTFA_MAX_MPDU_LENGTH_11B                 (OTFA_BASE_ADDRESS + 0x38)
#define	REG_OTFA_MAX_MPDU_LENGTH_HT                  (OTFA_BASE_ADDRESS + 0x3C)
#define	REG_OTFA_MAX_MPDU_LENGTH_VHT                 (OTFA_BASE_ADDRESS + 0x40)
#define	REG_OTFA_MAX_MPDU_LENGTH_HE                  (OTFA_BASE_ADDRESS + 0x44)
#define	REG_OTFA_MAX_MSDU_LENGTH                     (OTFA_BASE_ADDRESS + 0x48)
#define	REG_OTFA_MIN_FRAG_LENGTH_PROG                (OTFA_BASE_ADDRESS + 0x50)
#define	REG_OTFA_FREE_MD_PTR_FIFO_STATUS             (OTFA_BASE_ADDRESS + 0x54)
#define	REG_OTFA_FREE_MD_PTR_FIFO_DATA0_1            (OTFA_BASE_ADDRESS + 0x58)
#define	REG_OTFA_FREE_MD_PTR_FIFO_DATA2_3            (OTFA_BASE_ADDRESS + 0x5C)
#define	REG_OTFA_MD_USER_0_LIST_INDEX                (OTFA_BASE_ADDRESS + 0x70)
#define	REG_OTFA_OTF_START                           (OTFA_BASE_ADDRESS + 0x74)
#define	REG_OTFA_FIFO_ERROR                          (OTFA_BASE_ADDRESS + 0x78)
#define	REG_OTFA_FIFO_CLEAN                          (OTFA_BASE_ADDRESS + 0x7C)
#define	REG_OTFA_ALLOWED_PD_TYPE_OTF_MODE            (OTFA_BASE_ADDRESS + 0x80)
#define	REG_OTFA_TID_ERROR_FIFO_BASE_ADDR            (OTFA_BASE_ADDRESS + 0x84)
#define	REG_OTFA_TID_ERROR_FIFO_DEPTH_MINUS_ONE      (OTFA_BASE_ADDRESS + 0x88)
#define	REG_OTFA_TID_ERROR_FIFO_CLEAR_STRB           (OTFA_BASE_ADDRESS + 0x8C)
#define	REG_OTFA_TID_ERROR_FIFO_RD_ENTRIES_NUM       (OTFA_BASE_ADDRESS + 0x90)
#define	REG_OTFA_TID_ERROR_FIFO_NUM_ENTRIES_COUNT    (OTFA_BASE_ADDRESS + 0x94)
#define	REG_OTFA_TID_ERROR_FIFO_DEBUG                (OTFA_BASE_ADDRESS + 0x98)
#define	REG_OTFA_BAR_ZLDS_CTRL                       (OTFA_BASE_ADDRESS + 0x9C)
#define	REG_OTFA_USER0_MORE_DATA                     (OTFA_BASE_ADDRESS + 0xA0)
#define	REG_OTFA_SENDER_DURATION_SETTING_EN          (OTFA_BASE_ADDRESS + 0xA4)
#define	REG_OTFA_DEBUG_INFO2STD                      (OTFA_BASE_ADDRESS + 0xA8)
#define	REG_OTFA_LOGGER                              (OTFA_BASE_ADDRESS + 0xAC)
#define	REG_OTFA_TOTAL_PDS_COUNT                     (OTFA_BASE_ADDRESS + 0xB0)
#define	REG_OTFA_TOTAL_MPDUS_COUNT                   (OTFA_BASE_ADDRESS + 0xB4)
#define	REG_OTFA_SELECTED_TID_INDEX4DEBUG            (OTFA_BASE_ADDRESS + 0xB8)
#define	REG_OTFA_SELECTED_TID_INDEX_LIMIT_INFO       (OTFA_BASE_ADDRESS + 0xBC)
#define	REG_OTFA_SLOWEST_HW_MIN_PD_SPACE             (OTFA_BASE_ADDRESS + 0xC0)
#define	REG_OTFA_STATE_MACHINES1                     (OTFA_BASE_ADDRESS + 0xC4)
#define	REG_OTFA_STATE_MACHINES2                     (OTFA_BASE_ADDRESS + 0xC8)
#define	REG_OTFA_ERROR_IRQ_STATUS                    (OTFA_BASE_ADDRESS + 0xCC)
#define	REG_OTFA_ERROR_IRQ_ENABLE                    (OTFA_BASE_ADDRESS + 0xD0)
#define	REG_OTFA_SPARE_REGISTERS                     (OTFA_BASE_ADDRESS + 0xFC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_OTFA_GENERAL_CONTROL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryMaxMpduLengthLimitCheckEn : 1; //Enable checking Max MPDU length in case of retry, reset value: 0x1, access type: RW
		uint32 retryMaxMsdusAtAmsduLimitCheckEn : 1; //Enable checking Max MSDU length in case of retry, reset value: 0x1, access type: RW
		uint32 reserved0 : 2;
		uint32 forceDelimiterInSingleMpduHt : 1; //Force adding delimiter when A-MPDU contains only one MPDU in HT, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 dmaTran256Bytes : 1; //DMA transaction length , 0 - 128 bytes , 1 - 256 bytes, reset value: 0x0, access type: RW
		uint32 setEofInTf : 1; //set EOF in TF, reset value: 0x0, access type: RW
		uint32 eofConfA : 1; //eof_conf_a, reset value: 0x1, access type: RW
		uint32 eofConfB : 1; //eof_conf_b, reset value: 0x0, access type: RW
		uint32 eofConfC : 1; //eof_conf_c, reset value: 0x1, access type: RW
		uint32 eofLastOnly : 1; //eof_last_only, reset value: 0x0, access type: RW
		uint32 reserved2 : 19;
	} bitFields;
} RegOtfaGeneralControl_u;

/*REG_OTFA_RETRY_BUFFER_INDEX 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryBufferIndex : 1; //Retry DB buffer index- toggled by OTFA whenever it starts a new transmission., reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegOtfaRetryBufferIndex_u;

/*REG_OTFA_ZLDS_CONTROL_1 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 zldsInitPeriod : 6; //ZLDs init period [uS], reset value: 0x10, access type: RW
		uint32 reserved0 : 2;
		uint32 zldsMinThresholdPeriod : 5; //ZLDs min threshold period [uS], reset value: 0x1, access type: RW
		uint32 reserved1 : 3;
		uint32 zldsRecoverThresholdPeriod : 5; //ZLDs recover threshold period, reset value: 0x4, access type: RW
		uint32 reserved2 : 11;
	} bitFields;
} RegOtfaZldsControl1_u;

/*REG_OTFA_ZLDS_CONTROL_2 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 zldsMaxBudget4K : 13; //ZLDs max budget for circular buffer of 4K, reset value: 0xf30, access type: RW
		uint32 reserved0 : 3;
		uint32 zldsMaxBudget8K : 14; //ZLDs max budget for circular buffer of 8K, reset value: 0x1f00, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegOtfaZldsControl2_u;

/*REG_OTFA_STOP_BUILDING 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopBuilding : 1; //Stop MPDU building, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegOtfaStopBuilding_u;

/*REG_OTFA_BEACON_TIM_OVERHEAD_LENGTH 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beaconTimOverheadLength : 8; //TIM overhead length for Beacon, reset value: 0x2, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegOtfaBeaconTimOverheadLength_u;

/*REG_OTFA_ETH_TYPE_OVERHEAD 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ethType0Oh : 8; //Overhead length for Ether type = 0 , 8-bit two's complement value, reset value: 0xFA, access type: RW
		uint32 ethType1Oh : 8; //Overhead length for Ether type = 1 , 8-bit two's complement value, reset value: 0xFA, access type: RW
		uint32 ethType2Oh : 8; //Overhead length for Ether type = 2 , 8-bit two's complement value, reset value: 0xF2, access type: RW
		uint32 ethType3Oh : 8; //Overhead length for Ether type = 3 , 8-bit two's complement value, reset value: 0xFA, access type: RW
	} bitFields;
} RegOtfaEthTypeOverhead_u;

/*REG_OTFA_VAP_IS_STA 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapIsSta : 32; //VAP is working in STA mode, reset value: 0x0, access type: RW
	} bitFields;
} RegOtfaVapIsSta_u;

/*REG_OTFA_PD_ERR_ENABLE 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdErrEnable : 12; //pd_err_enable, reset value: 0xFFF, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegOtfaPdErrEnable_u;

/*REG_OTFA_AVG_MPDU_LENGTH_MIN_TH 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 avgMpduLengthMinTh : 16; //Average MPDU length minimum threshold , 16-bit two's complement value, reset value: 0xf000, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegOtfaAvgMpduLengthMinTh_u;

/*REG_OTFA_MAX_MPDU_LENGTH_11AG 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduLength11Ag : 14; //Max MPDU length 11ag, reset value: 0x3fff, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegOtfaMaxMpduLength11Ag_u;

/*REG_OTFA_MAX_MPDU_LENGTH_11B 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduLength11B : 14; //Max MPDU length 11b, reset value: 0x3fff, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegOtfaMaxMpduLength11B_u;

/*REG_OTFA_MAX_MPDU_LENGTH_HT 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduLengthHt : 14; //Max MPDU length for HT non-aggregate, reset value: 0x3fff, access type: RW
		uint32 reserved0 : 2;
		uint32 maxMpduLengthHtAgg : 14; //Max MPDU length for HT ggregate, reset value: 0x3fff, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegOtfaMaxMpduLengthHt_u;

/*REG_OTFA_MAX_MPDU_LENGTH_VHT 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduLengthVht : 14; //Max MPDU length VHT, reset value: 0x3fff, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegOtfaMaxMpduLengthVht_u;

/*REG_OTFA_MAX_MPDU_LENGTH_HE 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduLengthHe : 14; //Max MPDU length HE, reset value: 0x3fff, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegOtfaMaxMpduLengthHe_u;

/*REG_OTFA_MAX_MSDU_LENGTH 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMsduLength : 14; //Max MSDU length, reset value: 0x900, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegOtfaMaxMsduLength_u;

/*REG_OTFA_MIN_FRAG_LENGTH_PROG 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minFragLengthProg : 12; //Min Frag length, reset value: 0x80, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegOtfaMinFragLengthProg_u;

/*REG_OTFA_FREE_MD_PTR_FIFO_STATUS 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeMdPtrFifoFullness : 3; //Free MPDU Descriptor pointers FIFO fullness, reset value: 0x0, access type: RO
		uint32 reserved0 : 5;
		uint32 freeMdPtrFifoRdPtr : 2; //Free MPDU Descriptor pointers FIFO - read pointer, reset value: 0x0, access type: RO
		uint32 reserved1 : 6;
		uint32 freeMdPtrFifoWrPtr : 2; //Free MPDU Descriptor pointers FIFO - write pointer, reset value: 0x0, access type: RO
		uint32 reserved2 : 14;
	} bitFields;
} RegOtfaFreeMdPtrFifoStatus_u;

/*REG_OTFA_FREE_MD_PTR_FIFO_DATA0_1 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeMdPtrFifoSwRdData0 : 16; //Free MPDU Descriptor pointer FiFO - SW read data 0, reset value: 0x0, access type: RO
		uint32 freeMdPtrFifoSwRdData1 : 16; //Free MPDU Descriptor pointer FiFO - SW read data 1, reset value: 0x0, access type: RO
	} bitFields;
} RegOtfaFreeMdPtrFifoData01_u;

/*REG_OTFA_FREE_MD_PTR_FIFO_DATA2_3 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeMdPtrFifoSwRdData2 : 16; //Free MPDU Descriptor pointer FiFO - SW read data 2, reset value: 0x0, access type: RO
		uint32 freeMdPtrFifoSwRdData3 : 16; //Free MPDU Descriptor pointer FiFO - SW read data 3, reset value: 0x0, access type: RO
	} bitFields;
} RegOtfaFreeMdPtrFifoData23_u;

/*REG_OTFA_MD_USER_0_LIST_INDEX 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mdUser0ListIndex : 8; //MPDU descriptor - user 0 list index, reset value: 0x2C, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegOtfaMdUser0ListIndex_u;

/*REG_OTFA_OTF_START 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 otfStart : 1; //no description, reset value: 0x0, access type: WO
		uint32 buildOnlyTf : 1; //Build only TF, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegOtfaOtfStart_u;

/*REG_OTFA_FIFO_ERROR 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduDescFifoErrorSticky : 2; //bit 0 - indicates underflow , bit 1 - indicates overflow, reset value: 0x0, access type: RO
		uint32 freeMdPtrFifoErrorSticky : 2; //bit 0 - indicates underflow , bit 1 - indicates overflow, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegOtfaFifoError_u;

/*REG_OTFA_FIFO_CLEAN 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduDescFifoClean : 1; //mpdu_desc_fifo_clean, reset value: 0x0, access type: WO
		uint32 freeMdPtrFifoClean : 1; //free_md_ptr_fifo_clean, reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegOtfaFifoClean_u;

/*REG_OTFA_ALLOWED_PD_TYPE_OTF_MODE 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 allowedPdTypeOtfMode : 32; //Allowed PD types in OTF mode, reset value: 0x3, access type: RW
	} bitFields;
} RegOtfaAllowedPdTypeOtfMode_u;

/*REG_OTFA_TID_ERROR_FIFO_BASE_ADDR 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tidErrorFifoBaseAddr : 22; //TID error FIFO base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegOtfaTidErrorFifoBaseAddr_u;

/*REG_OTFA_TID_ERROR_FIFO_DEPTH_MINUS_ONE 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tidErrorFifoDepthMinusOne : 8; //TID error FIFO depth minus one, reset value: 0xff, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegOtfaTidErrorFifoDepthMinusOne_u;

/*REG_OTFA_TID_ERROR_FIFO_CLEAR_STRB 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tidErrorFifoClearStrb : 1; //Clear TID error FIFO. , By write '1'., reset value: 0x0, access type: WO
		uint32 tidErrorFifoClearFullDropCtrStrb : 1; //Clear TID error FIFO full drop counter. , By write '1'., reset value: 0x0, access type: WO
		uint32 tidErrorFifoClearDecLessThanZeroStrb : 1; //Clear TID error FIFO decrement amount less than zero indication. , By write '1'.  , , reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegOtfaTidErrorFifoClearStrb_u;

/*REG_OTFA_TID_ERROR_FIFO_RD_ENTRIES_NUM 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tidErrorFifoRdEntriesNum : 9; //TID error FIFO number of entries to decrement., reset value: 0x0, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegOtfaTidErrorFifoRdEntriesNum_u;

/*REG_OTFA_TID_ERROR_FIFO_NUM_ENTRIES_COUNT 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tidErrorFifoNumEntriesCount : 9; //TID error FIFO number of entries count, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegOtfaTidErrorFifoNumEntriesCount_u;

/*REG_OTFA_TID_ERROR_FIFO_DEBUG 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tidErrorFifoWrPtr : 8; //TID error FIFO write pointer index, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
		uint32 tidErrorFifoNotEmpty : 1; //TID error FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 tidErrorFifoFull : 1; //TID error FIFO full indication, reset value: 0x0, access type: RO
		uint32 tidErrorFifoDecrementLessThanZero : 1; //TID error setting FIFO amount has been decremented less than zero, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 tidErrorFifoFullDropCtr : 8; //TID error setting FIFO full drop counter, reset value: 0x0, access type: RO
	} bitFields;
} RegOtfaTidErrorFifoDebug_u;

/*REG_OTFA_BAR_ZLDS_CTRL 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bar8ZldsCnt : 10; //Num of 8 ZLDs to add before BAR, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegOtfaBarZldsCtrl_u;

/*REG_OTFA_USER0_MORE_DATA 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 otfaUser0MoreData : 1; //User 0 more data, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegOtfaUser0MoreData_u;

/*REG_OTFA_SENDER_DURATION_SETTING_EN 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderDurationSettingEn : 1; //Sender duration setting enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegOtfaSenderDurationSettingEn_u;

/*REG_OTFA_DEBUG_INFO2STD 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugInfo2StdAddr : 12; //Base address in STD for TID debug info, reset value: 0x26d, access type: RW
		uint32 reserved0 : 4;
		uint32 debugInfo2StdEnable : 1; //Enable TID debug info to STD, reset value: 0x0, access type: RW
		uint32 reserved1 : 15;
	} bitFields;
} RegOtfaDebugInfo2Std_u;

/*REG_OTFA_LOGGER 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerActive : 1; //OTFA logger active, reset value: 0x0, access type: RO
		uint32 loggerEnable : 1; //OTFA logger enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 otfaLoggerPriority : 2; //OTFA logger priority, reset value: 0x0, access type: RW
		uint32 reserved1 : 26;
	} bitFields;
} RegOtfaLogger_u;

/*REG_OTFA_TOTAL_PDS_COUNT 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 totalPdsCount : 20; //total_pds_count, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegOtfaTotalPdsCount_u;

/*REG_OTFA_TOTAL_MPDUS_COUNT 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 totalMpdusCount : 15; //total_mpdus_count, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegOtfaTotalMpdusCount_u;

/*REG_OTFA_SELECTED_TID_INDEX4DEBUG 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedTidIndex4Debug : 8; //selected_tid_index4debug, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegOtfaSelectedTidIndex4Debug_u;

/*REG_OTFA_SELECTED_TID_INDEX_LIMIT_INFO 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedTidIndexLimitInfo : 32; //selected_tid_index_limit_info, reset value: 0x0, access type: RO
	} bitFields;
} RegOtfaSelectedTidIndexLimitInfo_u;

/*REG_OTFA_SLOWEST_HW_MIN_PD_SPACE 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 slowestHwMinPdSpace : 8; //slowest_hw_min_pd_space , Units are in [us]/128  (value of 128 means 1uS, 64 - 500nS, etc.). , reset value: 0x1a, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegOtfaSlowestHwMinPdSpace_u;

/*REG_OTFA_STATE_MACHINES1 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaTidCntrsSmOut : 3; //no description, reset value: 0x0, access type: RO
		uint32 pdPtrCacheCtrlSmOut : 3; //no description, reset value: 0x0, access type: RO
		uint32 descCacheCtrlSmOut : 2; //no description, reset value: 0x0, access type: RO
		uint32 mpduBuilderSmOut : 6; //no description, reset value: 0x0, access type: RO
		uint32 mpduDescHandlerSmOut : 3; //no description, reset value: 0x0, access type: RO
		uint32 otfaCtrlSmOut : 3; //no description, reset value: 0x0, access type: RO
		uint32 pdCacheCtrlSmOut : 3; //no description, reset value: 0x0, access type: RO
		uint32 retryDbCacheCtrlSmOut : 4; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
		uint32 otfaInIdle : 1; //no description, reset value: 0x1, access type: RO
	} bitFields;
} RegOtfaStateMachines1_u;

/*REG_OTFA_STATE_MACHINES2 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stdFetchReqSmOut : 3; //no description, reset value: 0x0, access type: RO
		uint32 stdFetchAckSmOut : 3; //no description, reset value: 0x0, access type: RO
		uint32 tidErrorFifoSmOut : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegOtfaStateMachines2_u;

/*REG_OTFA_ERROR_IRQ_STATUS 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tidErrorFifoFullDropIrq : 1; //tid_error_fifo_full_drop_irq, reset value: 0x0, access type: RO
		uint32 tidErrorFifoDecrementLessThanZeroIrq : 1; //tid_error_fifo_decrement_less_than_zero_irq, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegOtfaErrorIrqStatus_u;

/*REG_OTFA_ERROR_IRQ_ENABLE 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tidErrorFifoFullDropIrqEnabled : 1; //tid_error_fifo_full_drop_irq_enabled, reset value: 0x0, access type: RW
		uint32 tidErrorFifoDecrementLessThanZeroIrqEnabled : 1; //tid_error_fifo_decrement_less_than_zero_irq_enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegOtfaErrorIrqEnable_u;

/*REG_OTFA_SPARE_REGISTERS 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareRegisters : 16; //Spare registers, reset value: 0x8888, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegOtfaSpareRegisters_u;



#endif // _OTFA_REGS_H_
