
/***********************************************************************************
File:				TxPdAccRegs.h
Module:				TxPdAcc
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TX_PD_ACC_REGS_H_
#define _TX_PD_ACC_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TX_PD_ACC_BASE_ADDRESS                             MEMORY_MAP_UNIT_42_BASE_ADDRESS
#define	REG_TX_PD_ACC_TXPD_ACTIVATION_CTRL                   (TX_PD_ACC_BASE_ADDRESS + 0x0)
#define	REG_TX_PD_ACC_TXPD_OPEARTION_PARAMS                  (TX_PD_ACC_BASE_ADDRESS + 0x4)
#define	REG_TX_PD_ACC_TXPD_ETH_TYPE_OVERHEAD                 (TX_PD_ACC_BASE_ADDRESS + 0x8)
#define	REG_TX_PD_ACC_TXPD_DMA_MODE                          (TX_PD_ACC_BASE_ADDRESS + 0xC)
#define	REG_TX_PD_ACC_TXPD_SENDER_PTD_FIFO_PARAMS            (TX_PD_ACC_BASE_ADDRESS + 0x10)
#define	REG_TX_PD_ACC_TXPD_SENDER_PTD_FIFO_STATUS            (TX_PD_ACC_BASE_ADDRESS + 0x14)
#define	REG_TX_PD_ACC_TXPD_SENDER_PTD_RD_ENTRIES_NUM         (TX_PD_ACC_BASE_ADDRESS + 0x18)
#define	REG_TX_PD_ACC_TXPD_SENDER_PTD_RELEASE_ENTRIES_NUM    (TX_PD_ACC_BASE_ADDRESS + 0x1C)
#define	REG_TX_PD_ACC_TXPD_ABORT_MODE                        (TX_PD_ACC_BASE_ADDRESS + 0x20)
#define	REG_TX_PD_ACC_TXPD_CONTROL_BITS                      (TX_PD_ACC_BASE_ADDRESS + 0x24)
#define	REG_TX_PD_ACC_TXPD_STATUS_0                          (TX_PD_ACC_BASE_ADDRESS + 0x28)
#define	REG_TX_PD_ACC_TXPD_STATUS_1                          (TX_PD_ACC_BASE_ADDRESS + 0x2C)
#define	REG_TX_PD_ACC_SPP_PER_PD_BITMAP                      (TX_PD_ACC_BASE_ADDRESS + 0x30)
#define	REG_TX_PD_ACC_SKIP_DMA_PER_PD_BITMAP                 (TX_PD_ACC_BASE_ADDRESS + 0x34)
#define	REG_TX_PD_ACC_ADD_MPDU_PER_PD_BITMAP                 (TX_PD_ACC_BASE_ADDRESS + 0x38)
#define	REG_TX_PD_ACC_DBG_DLM_COUNTERS_CONTROL               (TX_PD_ACC_BASE_ADDRESS + 0x40)
#define	REG_TX_PD_ACC_DBG_DLM_MPDU_COUNT                     (TX_PD_ACC_BASE_ADDRESS + 0x44)
#define	REG_TX_PD_ACC_DBG_DLM_PD_COUNT                       (TX_PD_ACC_BASE_ADDRESS + 0x48)
#define	REG_TX_PD_ACC_TXPD_LOGGER_CONTROL                    (TX_PD_ACC_BASE_ADDRESS + 0x50)
#define	REG_TX_PD_ACC_TXPD_LOGGER_STATUS                     (TX_PD_ACC_BASE_ADDRESS + 0x54)
#define	REG_TX_PD_ACC_TXPD_INT_ERROR_STATUS                  (TX_PD_ACC_BASE_ADDRESS + 0x60)
#define	REG_TX_PD_ACC_TXPD_INT_ERROR_EN                      (TX_PD_ACC_BASE_ADDRESS + 0x64)
#define	REG_TX_PD_ACC_TXPD_INT_ERROR_CLEAR                   (TX_PD_ACC_BASE_ADDRESS + 0x68)
#define	REG_TX_PD_ACC_TXPD_SPARE_REGISTER                    (TX_PD_ACC_BASE_ADDRESS + 0x70)
#define	REG_TX_PD_ACC_TXPD_SM_DEBUG                          (TX_PD_ACC_BASE_ADDRESS + 0x74)
#define	REG_TX_PD_ACC_STATISTICS_COUNTERS_EN                 (TX_PD_ACC_BASE_ADDRESS + 0x78)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TX_PD_ACC_TXPD_ACTIVATION_CTRL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdGo : 1; //Writing to this field a value of '1' serves as a Go command. The TX_PD Accelerator shall start processing the PPDU upon a Go command , It is forbidden to issue a Go command before the previous one had ended. , , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegTxPdAccTxpdActivationCtrl_u;

/*REG_TX_PD_ACC_TXPD_OPEARTION_PARAMS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 manipulateRetryFieldsEn : 1; //Enable manipulate retry bit and retry count, reset value: 0x1, access type: RW
		uint32 skipMiddlePdInMpdu : 1; //Enable Skip middle PDs in MPDU, reset value: 0x0, access type: RW
		uint32 reserved0 : 14;
		uint32 longMpduThreshold : 14; //Long MPDU threshold (in bytes). An MPDU which is greater or equal than this threshold shall be considered as long. Otherwise it shall be considered as short. This definition is used in the calculation of the retry limit., reset value: 0x300, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegTxPdAccTxpdOpeartionParams_u;

/*REG_TX_PD_ACC_TXPD_ETH_TYPE_OVERHEAD 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ethType0Overhead : 6; //Eth. Type0 (ETH2) overhead. Value is signed, reset value: 0x3a, access type: RW
		uint32 reserved0 : 2;
		uint32 ethType1Overhead : 6; //Eth. Type1 (Rsvd) overhead. Value is signed, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 ethType2Overhead : 6; //Eth. Type2 (SNAP) overhead. Value is signed, reset value: 0x32, access type: RW
		uint32 reserved2 : 2;
		uint32 ethType3Overhead : 6; //Eth. Type3 (EAPOL) overhead. Value is signed, reset value: 0x3a, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegTxPdAccTxpdEthTypeOverhead_u;

/*REG_TX_PD_ACC_TXPD_DMA_MODE 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaLastCtrl : 1; //Affects the DMA_Last field within the DMA Job (within the PTD). If DMA_LAST_CTRL configuration bit is set, then DMA Last shall be set for the last PD in the PPDU. Otherwise, DMA Last shall be written as 0., reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegTxPdAccTxpdDmaMode_u;

/*REG_TX_PD_ACC_TXPD_SENDER_PTD_FIFO_PARAMS 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdSenderPtdFifoBaseAddr : 11; //Base Address of the TXPD_SENDER_PTD_FIFO within the RAM. This is a DW address. Note: Address must be even !!, reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 txpdSenderPtdFifoDepthMinusOne : 7; //Depth of the FIFO minus one. Designates the number of PTDs (minus one), which can fit in the FIFO. This number is derived from PTD_WORD_SIZE, which is the actual size which is allocated for each PTD, reset value: 0x1f, access type: RW
		uint32 reserved1 : 1;
		uint32 ptdWordSize : 5; //PTD size in DW units. This field designates the number of DWs which is allocated for each PTD within the memory. This size must not be less than the actual size of the PTD. Note: Size must be even !!, reset value: 0xc, access type: RW
		uint32 reserved2 : 3;
	} bitFields;
} RegTxPdAccTxpdSenderPtdFifoParams_u;

/*REG_TX_PD_ACC_TXPD_SENDER_PTD_FIFO_STATUS 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdSenderPtdNumEntriesCount : 7; //Number of entries available for read in the FIFO. Each unit designates a PTD, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 txpdSenderPtdFifoWrPtr : 7; //FIFO Write pointer of pending PTDs. Each unit designates a PTD, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 txpdSenderPtdFifoRdPtr : 7; //FIFO Read pointer of released PTDs. Each unit designates a PTD, reset value: 0x0, access type: RO
		uint32 reserved2 : 5;
		uint32 txpdSenderPtdFifoDecrementLessThanZero : 1; //Number of PTDs decremented to less than zero (error indication). , Locked until cleared by txpd_sender_ptd_fifo_clear_dec_less_than_zero, reset value: 0x0, access type: RO
		uint32 reserved3 : 3;
	} bitFields;
} RegTxPdAccTxpdSenderPtdFifoStatus_u;

/*REG_TX_PD_ACC_TXPD_SENDER_PTD_RD_ENTRIES_NUM 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdSenderPtdRdEntriesNum : 7; //Number of PTDs to decrement. Each unit designates a PTD. The number of pending PTDs shall be decremented (with the amount that was written) when writing to this register, reset value: 0x0, access type: RW
		uint32 reserved0 : 25;
	} bitFields;
} RegTxPdAccTxpdSenderPtdRdEntriesNum_u;

/*REG_TX_PD_ACC_TXPD_SENDER_PTD_RELEASE_ENTRIES_NUM 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdSenderPtdReleaseEntriesNum : 7; //Number of PTDs to release. Each unit designates a PTD. The number of released PTDs shall be incremented (with the amount that was written) when writing to this register, reset value: 0x0, access type: RW
		uint32 reserved0 : 25;
	} bitFields;
} RegTxPdAccTxpdSenderPtdReleaseEntriesNum_u;

/*REG_TX_PD_ACC_TXPD_ABORT_MODE 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdAbortMode : 1; //When this field is active, the TX_PD Accelerator operates in Abort mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegTxPdAccTxpdAbortMode_u;

/*REG_TX_PD_ACC_TXPD_CONTROL_BITS 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 txpdPendingPtdClear : 1; //Write 1 to this field in order to clear the pending PTDs related pointers and accumulators, reset value: 0x0, access type: WO
		uint32 txpdOccupiedPtdClear : 1; //Write 1 to this field in order to clear the occupied PTDs related pointers and accumulators, reset value: 0x0, access type: WO
		uint32 txpdReleasedPtdClear : 1; //Write 1 to this field in order to clear the released PTDs related pointers and accumulators, reset value: 0x0, access type: WO
		uint32 txpdSenderPtdFifoClearDecLessThanZero : 1; //Write 1 to this field in order to clear txpd_sender_ptd_fifo_decrement_less_than_zero error, reset value: 0x0, access type: WO
		uint32 txpdPtdOccupancyMaxClear : 1; //Write 1 to this field in order to clear all max occupancy indications, reset value: 0x0, access type: WO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxPdAccTxpdControlBits_u;

/*REG_TX_PD_ACC_TXPD_STATUS_0 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdPendingPtdOccupancyMax : 7; //Maximal number of pending PTDs (Cleared by writing to txpd_ptd_occupancy_max_clear), reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 txpdOccupiedPtdOccupancyMax : 7; //Maximal number of occupied PTDs (Cleared by writing to txpd_ptd_occupancy_max_clear), reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 txpdReleasedPtdOccupancyMax : 7; //Maximal number of released PTDs (Cleared by writing to txpd_ptd_occupancy_max_clear), reset value: 0x0, access type: RO
		uint32 reserved2 : 9;
	} bitFields;
} RegTxPdAccTxpdStatus0_u;

/*REG_TX_PD_ACC_TXPD_STATUS_1 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdOccupiedPtdOccupancyLive : 7; //Live number of occupied PTDs, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 txpdReleasedPtdOccupancyLive : 7; //Live number of released PTDs, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 txpdIsIdle : 1; //Indication that TX_PD block is Idle. May be used as an indication for end of abort process, reset value: 0x1, access type: RO
		uint32 reserved2 : 15;
	} bitFields;
} RegTxPdAccTxpdStatus1_u;

/*REG_TX_PD_ACC_SPP_PER_PD_BITMAP 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sppPerPdBitmap : 32; //Sender PD Processing (SPP) bitmap. This bitmap is indexed by PD Type. The SPP bit is written to the PTD, and shall be passed to the Sender through the DMA and the TX_HC, reset value: 0x8038, access type: RW
	} bitFields;
} RegTxPdAccSppPerPdBitmap_u;

/*REG_TX_PD_ACC_SKIP_DMA_PER_PD_BITMAP 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 skipDmaPerPdBitmap : 32; //Skip DMA bitmap. This bitmap is indexed by PD Type. The Skip DMA bit is written to the PTD, and shall be passed to the DMA, reset value: 0x60100004, access type: RW
	} bitFields;
} RegTxPdAccSkipDmaPerPdBitmap_u;

/*REG_TX_PD_ACC_ADD_MPDU_PER_PD_BITMAP 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 addMpduPerPdBitmap : 32; //Add MPDU bitmap. This bitmap is indexed by PD Type. The ADD MPDU bit is written to the PTD, and shall be passed to the TX_HC through the DMA, reset value: 0x1ffffff, access type: RW
	} bitFields;
} RegTxPdAccAddMpduPerPdBitmap_u;

/*REG_TX_PD_ACC_DBG_DLM_COUNTERS_CONTROL 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlmMpduCountUserToMonitor : 7; //User ID to monitor for dlm_mpdu_count. If the value is set to all 1's (width of all 1's is band dependent), then all UIDs shall be counted, reset value: 0x7f, access type: RW
		uint32 reserved0 : 1;
		uint32 dlmPdCountUserToMonitor : 7; //User ID to monitor for dlm_pd_count. If the value is set to all 1's (width of all 1's is band dependent), then all UIDs shall be counted, reset value: 0x7f, access type: RW
		uint32 reserved1 : 1;
		uint32 dlmMpduCountClear : 1; //Write 1 to this field in order to clear dlm_mpdu counter, reset value: 0x0, access type: WO
		uint32 dlmPdCountClear : 1; //Write 1 to this field in order to clear dlm_pd counter, reset value: 0x0, access type: WO
		uint32 reserved2 : 14;
	} bitFields;
} RegTxPdAccDbgDlmCountersControl_u;

/*REG_TX_PD_ACC_DBG_DLM_MPDU_COUNT 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlmMpduCount : 16; //Number of MPDUs that were extracted from the DLM queues. The MPDUs can be counted per specific user or globally (based on dlm_mpdu_count_user_to_monitor), reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegTxPdAccDbgDlmMpduCount_u;

/*REG_TX_PD_ACC_DBG_DLM_PD_COUNT 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlmPdCount : 20; //Number of PDs that were extracted from the DLM queues. The PDs can be counted per specific user or globally (based on dlm_pd_count_user_to_monitor), reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegTxPdAccDbgDlmPdCount_u;

/*REG_TX_PD_ACC_TXPD_LOGGER_CONTROL 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdLoggerEn : 1; //Logger Enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 txpdLoggerPriority : 2; //TXPD Logger priority, reset value: 0x0, access type: RW
		uint32 txpdLoggerNumOfMsgsMinusOne : 4; //The maximal number of messages which will comprise a logger packet. If timeout (or logger disable) occurs before reaching this threshold, then the number of messages within a logger packet may be less than this threshold, reset value: 0x8, access type: RW
		uint32 reserved1 : 4;
		uint32 txpdLoggerMiddlePdEnable : 1; //When enabled, middle PDs (not first and not last) within an MPDU shall generate a logger message (assuming logger is enabled) , , reset value: 0x0, access type: RW
		uint32 txpdLoggerLastPdEnable : 1; //When enabled, any last PD within an MPDU shall generate a logger message (assuming logger is enabled) , , reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 txpdLoggerMessageDropClear : 1; //Write 1 to this field in order to clear the status indication (txpd_logger_message_drop_sticky) that a logger message was dropped, reset value: 0x0, access type: WO
		uint32 reserved3 : 15;
	} bitFields;
} RegTxPdAccTxpdLoggerControl_u;

/*REG_TX_PD_ACC_TXPD_LOGGER_STATUS 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdLoggerIdle : 1; //Indication that the logger is IDLE. Use this indication after the logger is disabled, in order to verify that it has finished any work in progress., reset value: 0x1, access type: RO
		uint32 txpdLoggerSm : 3; //State of the logger state machine, reset value: 0x0, access type: RO
		uint32 txpdLoggerPacketOpen : 1; //Indication whether or not the logger holds an open packet. An open packet designates that some messages were already output by the logger, but the packet was not closed and sent yet., reset value: 0x0, access type: RO
		uint32 txpdLoggerMessageDropSticky : 1; //Sticky Indication (cleared by writing to txpd_logger_message_drop_clear) - designates that a logger message was dropped, since a new message was initiated before the previuos one ended, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegTxPdAccTxpdLoggerStatus_u;

/*REG_TX_PD_ACC_TXPD_INT_ERROR_STATUS 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdGoWhileActiveMismatchStatus : 1; //Go command is issued while TX_PD is not idle, reset value: 0x0, access type: RO
		uint32 txpdSymbolOverheadOverflowStatus : 1; //Accumulated symbol length to transmit exceeds the allowed maximum, reset value: 0x0, access type: RO
		uint32 txpdLastInStaDlmMismatchStatus : 1; //Last is STA indication is set (within MPDU Descriptor) but DLM queue of the respective user is not yet empty, reset value: 0x0, access type: RO
		uint32 txpdNullPdNextPointerStatus : 1; //PD next pointer points to illegal NULL value, reset value: 0x0, access type: RO
		uint32 txpdOccupiedPtdUnderflowStatus : 1; //Number of occupied PTDs has underflowed, reset value: 0x0, access type: RO
		uint32 txpdSnSsnDiffOorStatus : 1; //Difference between SN and SSN is Out of Range (greater or equal than 256), reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegTxPdAccTxpdIntErrorStatus_u;

/*REG_TX_PD_ACC_TXPD_INT_ERROR_EN 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdGoWhileActiveMismatchEn : 1; //Go while active mismatch interrupt enable, reset value: 0x0, access type: RW
		uint32 txpdSymbolOverheadOverflowEn : 1; //Symbol Overhead Overflow interrupt enable, reset value: 0x0, access type: RW
		uint32 txpdLastInStaDlmMismatchEn : 1; //Last in STA DLM Mismatch interrupt enable, reset value: 0x0, access type: RW
		uint32 txpdNullPdNextPointerEn : 1; //NULL PD Next Pointer interrupt enable, reset value: 0x0, access type: RW
		uint32 txpdOccupiedPtdUnderflowEn : 1; //Occupied PTD Underflow interrupt enable, reset value: 0x0, access type: RW
		uint32 txpdSnSsnDiffOorEn : 1; //SN SSN diff OOR interrupt enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegTxPdAccTxpdIntErrorEn_u;

/*REG_TX_PD_ACC_TXPD_INT_ERROR_CLEAR 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdGoWhileActiveMismatchClr : 1; //Go while active mismatch interrupt clear, reset value: 0x0, access type: WO
		uint32 txpdSymbolOverheadOverflowClr : 1; //Symbol Overhead Overflow interrupt clear, reset value: 0x0, access type: WO
		uint32 txpdLastInStaDlmMismatchClr : 1; //Last in STA DLM Mismatch interrupt clear, reset value: 0x0, access type: WO
		uint32 txpdNullPdNextPointerClr : 1; //NULL PD Next Pointer interrupt clear, reset value: 0x0, access type: WO
		uint32 txpdOccupiedPtdUnderflowClr : 1; //Occupied PTD Underflow interrupt clear, reset value: 0x0, access type: WO
		uint32 txpdSnSsnDiffOorClr : 1; //SN SSN diff OOR interrupt clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 26;
	} bitFields;
} RegTxPdAccTxpdIntErrorClear_u;

/*REG_TX_PD_ACC_TXPD_SPARE_REGISTER 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareRegister : 16; //Spare Register, reset value: 0x8888, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegTxPdAccTxpdSpareRegister_u;

/*REG_TX_PD_ACC_TXPD_SM_DEBUG 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txpdCoreSm : 4; //txpd_core_sm, reset value: 0x0, access type: RO
		uint32 txpdPtdRdSm : 3; //txpd_ptd_rd_sm, reset value: 0x0, access type: RO
		uint32 txpdPdWrSm : 3; //txpd_pd_wr_sm, reset value: 0x0, access type: RO
		uint32 txpdRdbmSm : 4; //txpd_rdbm_sm, reset value: 0x0, access type: RO
		uint32 txpdStdmSm : 4; //txpd_stdm_sm, reset value: 0x0, access type: RO
		uint32 txpdVapmSm : 2; //txpd_vapm_sm, reset value: 0x0, access type: RO
		uint32 txpdDbitCalcSm : 2; //txpd_dbit_calc_sm, reset value: 0x0, access type: RO
		uint32 txpdInitSm : 2; //txpd_init_sm, reset value: 0x0, access type: RO
		uint32 txpdPtdWrSm : 2; //txpd_ptd_wr_sm, reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
	} bitFields;
} RegTxPdAccTxpdSmDebug_u;

/*REG_TX_PD_ACC_STATISTICS_COUNTERS_EN 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryCountEn : 1; //Retry counter enable., reset value: 0x0, access type: RW
		uint32 multipleRetryCountEn : 1; //Multiple retry counter enable., reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegTxPdAccStatisticsCountersEn_u;



#endif // _TX_PD_ACC_REGS_H_
