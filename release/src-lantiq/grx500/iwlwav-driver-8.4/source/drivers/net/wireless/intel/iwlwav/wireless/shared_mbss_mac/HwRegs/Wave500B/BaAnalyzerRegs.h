
/***********************************************************************************
File:				BaAnalyzerRegs.h
Module:				baAnalyzer
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _BA_ANALYZER_REGS_H_
#define _BA_ANALYZER_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define BA_ANALYZER_BASE_ADDRESS                             MEMORY_MAP_UNIT_45_BASE_ADDRESS
#define	REG_BA_ANALYZER_MODE                               (BA_ANALYZER_BASE_ADDRESS + 0x0)
#define	REG_BA_ANALYZER_INT_ERROR_STATUS                   (BA_ANALYZER_BASE_ADDRESS + 0x4)
#define	REG_BA_ANALYZER_INT_ERROR_EN                       (BA_ANALYZER_BASE_ADDRESS + 0x8)
#define	REG_BA_ANALYZER_CLEAR_INT_ERROR                    (BA_ANALYZER_BASE_ADDRESS + 0xC)
#define	REG_BA_ANALYZER_INT_STATUS                         (BA_ANALYZER_BASE_ADDRESS + 0x10)
#define	REG_BA_ANALYZER_INT_EN                             (BA_ANALYZER_BASE_ADDRESS + 0x14)
#define	REG_BA_ANALYZER_CLEAR_INT                          (BA_ANALYZER_BASE_ADDRESS + 0x18)
#define	REG_BA_ANALYZER_SENDER_REG                         (BA_ANALYZER_BASE_ADDRESS + 0x1C)
#define	REG_BA_ANALYZER_RETRY_CNT_PD_TYPE0_7               (BA_ANALYZER_BASE_ADDRESS + 0x20)
#define	REG_BA_ANALYZER_RETRY_CNT_PD_TYPE8_15              (BA_ANALYZER_BASE_ADDRESS + 0x24)
#define	REG_BA_ANALYZER_DPL_IDX                            (BA_ANALYZER_BASE_ADDRESS + 0x28)
#define	REG_BA_ANALYZER_DEBUG                              (BA_ANALYZER_BASE_ADDRESS + 0x2C)
#define	REG_BA_ANALYZER_TXH_PSDU_LENGHT_THR                (BA_ANALYZER_BASE_ADDRESS + 0x30)
#define	REG_BA_ANALYZER_BA_VECTOR_IN_LOW                   (BA_ANALYZER_BASE_ADDRESS + 0x34)
#define	REG_BA_ANALYZER_BA_VECTOR_IN_HIGH                  (BA_ANALYZER_BASE_ADDRESS + 0x38)
#define	REG_BA_ANALYZER_BA_STARTING_SEQUENCE               (BA_ANALYZER_BASE_ADDRESS + 0x3C)
#define	REG_BA_ANALYZER_PACKET_INDICATION                  (BA_ANALYZER_BASE_ADDRESS + 0x40)
#define	REG_BA_ANALYZER_MEM_GLOBAL_RM                      (BA_ANALYZER_BASE_ADDRESS + 0x48)
#define	REG_BA_ANALYZER_PACKETS_IN_FIFO_RATE_ADAPT         (BA_ANALYZER_BASE_ADDRESS + 0x4C)
#define	REG_BA_ANALYZER_LIB_DPL_IDX_SOURCE0_3              (BA_ANALYZER_BASE_ADDRESS + 0x50)
#define	REG_BA_ANALYZER_LIB_DPL_IDX_SOURCE4_7              (BA_ANALYZER_BASE_ADDRESS + 0x54)
#define	REG_BA_ANALYZER_LIB_DPL_IDX_INPUT                  (BA_ANALYZER_BASE_ADDRESS + 0x58)
#define	REG_BA_ANALYZER_LIB_HALT                           (BA_ANALYZER_BASE_ADDRESS + 0x5C)
#define	REG_BA_ANALYZER_SENDER_TX_STATUS_WORD0             (BA_ANALYZER_BASE_ADDRESS + 0x60)
#define	REG_BA_ANALYZER_SENDER_TX_STATUS_WORD1             (BA_ANALYZER_BASE_ADDRESS + 0x64)
#define	REG_BA_ANALYZER_UMAC_STATUS_START_ADDR             (BA_ANALYZER_BASE_ADDRESS + 0x68)
#define	REG_BA_ANALYZER_LMAC_STATUS_START_ADDR             (BA_ANALYZER_BASE_ADDRESS + 0x6C)
#define	REG_BA_ANALYZER_SENDER_STATUS_START_ADDR           (BA_ANALYZER_BASE_ADDRESS + 0x70)
#define	REG_BA_ANALYZER_PACKETS_IN_TX_ST_FIFO              (BA_ANALYZER_BASE_ADDRESS + 0x74)
#define	REG_BA_ANALYZER_UMAC_PACKET_READ_REG               (BA_ANALYZER_BASE_ADDRESS + 0x78)
#define	REG_BA_ANALYZER_LMAC_PACKET_READ_REG               (BA_ANALYZER_BASE_ADDRESS + 0x7C)
#define	REG_BA_ANALYZER_SENDER_PACKET_READ_REG             (BA_ANALYZER_BASE_ADDRESS + 0x80)
#define	REG_BA_ANALYZER_LMAC_INT_STATUS                    (BA_ANALYZER_BASE_ADDRESS + 0x84)
#define	REG_BA_ANALYZER_LMAC_INT_EN                        (BA_ANALYZER_BASE_ADDRESS + 0x88)
#define	REG_BA_ANALYZER_SENDER_INT_STATUS                  (BA_ANALYZER_BASE_ADDRESS + 0x8C)
#define	REG_BA_ANALYZER_SENDER_INT_EN                      (BA_ANALYZER_BASE_ADDRESS + 0x90)
#define	REG_BA_ANALYZER_RX_LIB_DPL_IDX_SOURCE0_3           (BA_ANALYZER_BASE_ADDRESS + 0x94)
#define	REG_BA_ANALYZER_RX_LIB_DPL_IDX_SOURCE4_7           (BA_ANALYZER_BASE_ADDRESS + 0x98)
#define	REG_BA_ANALYZER_RX_LIB_DPL_IDX_INPUT               (BA_ANALYZER_BASE_ADDRESS + 0x9C)
#define	REG_BA_ANALYZER_RX_LIB_HALT                        (BA_ANALYZER_BASE_ADDRESS + 0xA0)
#define	REG_BA_ANALYZER_SHORT_RETRY_CNT_PD_TYPE0_7         (BA_ANALYZER_BASE_ADDRESS + 0xA4)
#define	REG_BA_ANALYZER_SHORT_RETRY_CNT_PD_TYPE8_15        (BA_ANALYZER_BASE_ADDRESS + 0xA8)
#define	REG_BA_ANALYZER_BA_ANALYZER_SPARE                  (BA_ANALYZER_BASE_ADDRESS + 0xAC)
#define	REG_BA_ANALYZER_BA_ANALYZER_COUNTERS_EN            (BA_ANALYZER_BASE_ADDRESS + 0xB0)
#define	REG_BA_ANALYZER_LOGGER_REG                         (BA_ANALYZER_BASE_ADDRESS + 0xB4)
#define	REG_BA_ANALYZER_LIBERATOR_MAX_MESSAGE_TO_LOGGER    (BA_ANALYZER_BASE_ADDRESS + 0xB8)
#define	REG_BA_ANALYZER_RT_ADAPT_SENDER_BITS               (BA_ANALYZER_BASE_ADDRESS + 0xBC)
#define	REG_BA_ANALYZER_RA_FIFO_START_ADDR                 (BA_ANALYZER_BASE_ADDRESS + 0xC0)
#define	REG_BA_ANALYZER_RATE_ADAPT_DEBUG_REG               (BA_ANALYZER_BASE_ADDRESS + 0xC4)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_BA_ANALYZER_MODE 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 updateBw:1;	// Block window update enable 
		uint32 unicastPsUpEn:1;	// Power save execution enable
		uint32 blockEn:1;	// BAA block enable 
		uint32 liberatorBlockEn:1;	// BAA block enable 
		uint32 rxLiberatorBlockEn:1;	// BAA block enable 
		uint32 skipCheckTidMine:1;	// BAA block enable 
		uint32 reserved0:26;
	} bitFields;
} RegBaAnalyzerMode_u;

/*REG_BA_ANALYZER_INT_ERROR_STATUS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stdOwnerError:1;	// std_owner_error
		uint32 riscStdErrAccess:1;	// risc_std_err_access
		uint32 umacPushTxStWhileFullInt:1;	// umac_push_tx_st_while_full
		uint32 lmacPushTxStWhileFullInt:1;	// lmac_push_tx_st_while_full
		uint32 senderPushTxStWhileFullInt:1;	// sender_push_tx_st_while_full
		uint32 rtAdaptPushWhileFullInt:1;	// rt_adapt_push_while_full_int
		uint32 reserved0:26;
	} bitFields;
} RegBaAnalyzerIntErrorStatus_u;

/*REG_BA_ANALYZER_INT_ERROR_EN 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stdOwnerErrorEn:1;	// std_owner_error_en
		uint32 riscStdErrAccessEn:1;	// risc_std_err_access_en
		uint32 umacPushTxStWhileFullEn:1;	// umac_push_tx_st_while_full_en
		uint32 lmacPushTxStWhileFullEn:1;	// lmac_push_tx_st_while_full_en
		uint32 senderPushTxStWhileFullEn:1;	// sender_push_tx_st_while_full_en
		uint32 rtAdaptPushWhileFullEn:1;	// rt_adapt_push_while_full_en
		uint32 reserved0:26;
	} bitFields;
} RegBaAnalyzerIntErrorEn_u;

/*REG_BA_ANALYZER_CLEAR_INT_ERROR 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clrStdOwnerError:1;	// clr_std_owner_error
		uint32 clrRiscStdErrAccess:1;	// clr_risc_std_err_access
		uint32 clrUmacPushTxStWhileFull:1;	// clr_umac_push_tx_st_while_full
		uint32 clrLmacPushTxStWhileFull:1;	// clr_lmac_push_tx_st_while_full
		uint32 clrSenderPushTxStWhileFull:1;	// clr_sender_push_tx_st_while_full
		uint32 clrRtAdaptPushWhileFull:1;	// clr_rt_adapt_push_while_full
		uint32 reserved0:26;
	} bitFields;
} RegBaAnalyzerClearIntError_u;

/*REG_BA_ANALYZER_INT_STATUS 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rateAdaptiveFifoNotEmpty:1;	// rate_adaptive_fifo_not_empty
		uint32 reserved0:1;
		uint32 umacTxStNotEmpty:1;	// umac_tx_st_not_empty
		uint32 reserved1:29;
	} bitFields;
} RegBaAnalyzerIntStatus_u;

/*REG_BA_ANALYZER_INT_EN 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rateAdaptiveFifoNotEmptyEn:1;	// rate_adaptive_fifo_not_empty_en
		uint32 reserved0:1;
		uint32 umacTxStNotEmptyEn:1;	// umac_tx_st_not_empty_en
		uint32 reserved1:29;
	} bitFields;
} RegBaAnalyzerIntEn_u;

/*REG_BA_ANALYZER_CLEAR_INT 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clrRateAdaptiveFifoNotEmptyNc:1;	// clr_rate_adaptive_fifo_not_empty
		uint32 reserved0:1;
		uint32 clrUmacTxStNotEmptyNc:1;	// clr_umac_tx_st_not_empty_nc
		uint32 reserved1:29;
	} bitFields;
} RegBaAnalyzerClearInt_u;

/*REG_BA_ANALYZER_SENDER_REG 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderTrig:1;	// Sender will trigger the BAA that he should wait for BA.
		uint32 multicast:1;	// Multicast indication from sender 
		uint32 doNotWaitForRx:1;	// Do not wait for pac indication put all list to success list 
		uint32 unlockTxqbitmapEnTimeout:1;	// BAA will Unlock_TxQbitmap in case of timeout 
		uint32 updateHandlerParamsEnTimeout:1;	// update_handler_params_en_timeout
		uint32 unlockTxqbitmapEnOtherRx:1;	// BAA will Unlock_TxQbitmap in case of other_rx  
		uint32 updateHandlerParamsEnOtherRx:1;	// update_handler_params_en_other_rx
		uint32 unlockTxqbitmapEnBaOk:1;	// BAA will Unlock_TxQbitmap in case of BA_ok  
		uint32 updateHandlerParamsEnBaOk:1;	// update_handler_params_en_BA_ok
		uint32 doNotSeparateListTimeout:1;	// In case this value is 1 BAA will not execute list separation .
		uint32 doNotSeparateListOtherRx:1;	// In case this value is 1 BAA will not execute list separation .
		uint32 doNotSeparateListBaOk:1;	// In case this value is 1 BAA will not execute list separation .
		uint32 rateAdaptUpdateBypass:1;	// rate_adapt_update_bypass
		uint32 reserved0:1;
		uint32 txStatusRecipientIdentifier:2;	// 00 - umac,01 lmac,10 sender
		uint32 txFifoEnTimeout:1;	// tx_fifo_en_timeout
		uint32 txFifoEnOtherRx:1;	// tx_fifo_en_other_rx
		uint32 txFifoEnBaOk:1;	// tx_fifo_en_BA_ok
		uint32 rtsPacketTransmited:1;	// cts_packet_transmited
		uint32 reserved1:12;
	} bitFields;
} RegBaAnalyzerSenderReg_u;

/*REG_BA_ANALYZER_RETRY_CNT_PD_TYPE0_7 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryCntPdType0:4;	// retry_cnt_pd_type0
		uint32 retryCntPdType1:4;	// retry_cnt_pd_type1
		uint32 retryCntPdType2:4;	// retry_cnt_pd_type2
		uint32 retryCntPdType3:4;	// retry_cnt_pd_type3
		uint32 retryCntPdType4:4;	// retry_cnt_pd_type4
		uint32 retryCntPdType5:4;	// retry_cnt_pd_type5
		uint32 retryCntPdType6:4;	// retry_cnt_pd_type6
		uint32 retryCntPdType7:4;	// retry_cnt_pd_type7
	} bitFields;
} RegBaAnalyzerRetryCntPdType07_u;

/*REG_BA_ANALYZER_RETRY_CNT_PD_TYPE8_15 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryCntPdType8:4;	// retry_cnt_pd_type8
		uint32 retryCntPdType9:4;	// retry_cnt_pd_type9
		uint32 retryCntPdType10:4;	// retry_cnt_pd_type10
		uint32 retryCntPdType11:4;	// retry_cnt_pd_type11
		uint32 retryCntPdType12:4;	// retry_cnt_pd_type12
		uint32 retryCntPdType13:4;	// retry_cnt_pd_type13
		uint32 retryCntPdType14:4;	// retry_cnt_pd_type14
		uint32 retryCntPdType15:4;	// retry_cnt_pd_type15
	} bitFields;
} RegBaAnalyzerRetryCntPdType815_u;

/*REG_BA_ANALYZER_DPL_IDX 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dplIdxSuccess:5;	// dpl_idx_success
		uint32 reserved0:3;
		uint32 dplIdxDiscard:5;	// dpl_idx_discard
		uint32 reserved1:19;
	} bitFields;
} RegBaAnalyzerDplIdx_u;

/*REG_BA_ANALYZER_DEBUG 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mainStm:5;	// dpl_idx_discard
		uint32 mpduCnt:7;	// dpl_idx_discard
		uint32 liberatorWorking:1;	// liberator_working
		uint32 rxLiberatorWorking:1;	// liberator_working
		uint32 reserved0:18;
	} bitFields;
} RegBaAnalyzerDebug_u;

/*REG_BA_ANALYZER_TXH_PSDU_LENGHT_THR 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPsduLenghtThr:20;	// txh_psdu_lenght_thr
		uint32 reserved0:12;
	} bitFields;
} RegBaAnalyzerTxhPsduLenghtThr_u;

/*REG_BA_ANALYZER_BA_VECTOR_IN_LOW 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baVectorInLow:32;	// ba_vector_in_low
	} bitFields;
} RegBaAnalyzerBaVectorInLow_u;

/*REG_BA_ANALYZER_BA_VECTOR_IN_HIGH 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baVectorInHigh:32;	// ba_vector_in_high
	} bitFields;
} RegBaAnalyzerBaVectorInHigh_u;

/*REG_BA_ANALYZER_BA_STARTING_SEQUENCE 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baStartingSequence:16;	// ba_starting_sequence
		uint32 rxBaaBaControl:16;	// rx_baa_ba_control
	} bitFields;
} RegBaAnalyzerBaStartingSequence_u;

/*REG_BA_ANALYZER_PACKET_INDICATION 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhNtdBaaRxCts:1;	// txh_ntd_baa_rx_cts
		uint32 txhNtdBaaRxAck:1;	// txh_ntd_baa_rx_ack
		uint32 txhNtdBaaRxBa:1;	// txh_ntd_baa_rx_ba
		uint32 txhNtdBaaRxBfRpt:1;	// txh_ntd_baa_rx_bf_rpt
		uint32 txhNtdBaaOtherRx:1;	// txh_ntd_baa_other_rx
		uint32 txhNtdBaaRespTimeout:1;	// txh_ntd_baa_resp_timeout
		uint32 reserved0:26;
	} bitFields;
} RegBaAnalyzerPacketIndication_u;

/*REG_BA_ANALYZER_MEM_GLOBAL_RM 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 baaCountersRm:3;	// baa_counters_rm
		uint32 reserved1:21;
	} bitFields;
} RegBaAnalyzerMemGlobalRm_u;

/*REG_BA_ANALYZER_PACKETS_IN_FIFO_RATE_ADAPT 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetsInFifoRateAdapt:8;	// packets_in_fifo_rate_adapt
		uint32 rateAdaptNumPacketDrop:8;	// rate_adapt_num_packet_drop
		uint32 reserved0:16;
	} bitFields;
} RegBaAnalyzerPacketsInFifoRateAdapt_u;

/*REG_BA_ANALYZER_LIB_DPL_IDX_SOURCE0_3 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorDplIdxSource0:5;	// liberator_dpl_idx_source0
		uint32 reserved0:3;
		uint32 liberatorDplIdxSource1:5;	// liberator_dpl_idx_source1
		uint32 reserved1:3;
		uint32 liberatorDplIdxSource2:5;	// liberator_dpl_idx_source2
		uint32 reserved2:3;
		uint32 liberatorDplIdxSource3:5;	// liberator_dpl_idx_source3
		uint32 reserved3:3;
	} bitFields;
} RegBaAnalyzerLibDplIdxSource03_u;

/*REG_BA_ANALYZER_LIB_DPL_IDX_SOURCE4_7 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorDplIdxSource4:5;	// liberator_dpl_idx_source4
		uint32 reserved0:3;
		uint32 liberatorDplIdxSource5:5;	// liberator_dpl_idx_source5
		uint32 reserved1:3;
		uint32 liberatorDplIdxSource6:5;	// liberator_dpl_idx_source6
		uint32 reserved2:3;
		uint32 liberatorDplIdxSource7:5;	// liberator_dpl_idx_source7
		uint32 reserved3:3;
	} bitFields;
} RegBaAnalyzerLibDplIdxSource47_u;

/*REG_BA_ANALYZER_LIB_DPL_IDX_INPUT 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorDplIdxIn:5;	// liberator_dpl_idx_in
		uint32 reserved0:27;
	} bitFields;
} RegBaAnalyzerLibDplIdxInput_u;

/*REG_BA_ANALYZER_LIB_HALT 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorHaltWork:1;	// halt in oder to update id on the fly 
		uint32 reserved0:31;
	} bitFields;
} RegBaAnalyzerLibHalt_u;

/*REG_BA_ANALYZER_SENDER_TX_STATUS_WORD0 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderTxStatusWord0:32;	// sender_tx_status_word0
	} bitFields;
} RegBaAnalyzerSenderTxStatusWord0_u;

/*REG_BA_ANALYZER_SENDER_TX_STATUS_WORD1 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderTxStatusWord1:32;	// sender_tx_status_word1
	} bitFields;
} RegBaAnalyzerSenderTxStatusWord1_u;

/*REG_BA_ANALYZER_UMAC_STATUS_START_ADDR 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacStatusStartAddr:24;	// umac_status_start_addr
		uint32 umacStatusPacketSize:8;	// umac_status_start_addr
	} bitFields;
} RegBaAnalyzerUmacStatusStartAddr_u;

/*REG_BA_ANALYZER_LMAC_STATUS_START_ADDR 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacStatusStartAddr:24;	// lmac_status_start_addr
		uint32 lmacStatusPacketSize:8;	// lmac_status_start_addr
	} bitFields;
} RegBaAnalyzerLmacStatusStartAddr_u;

/*REG_BA_ANALYZER_SENDER_STATUS_START_ADDR 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderStatusStartAddr:24;	// sender_status_start_addr
		uint32 senderStatusPacketSize:8;	// sender_status_start_addr
	} bitFields;
} RegBaAnalyzerSenderStatusStartAddr_u;

/*REG_BA_ANALYZER_PACKETS_IN_TX_ST_FIFO 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacPacketsInTxStFifo:8;	// umac_packets_in_tx_st_fifo
		uint32 lmacPacketsInTxStFifo:8;	// lmac_packets_in_tx_st_fifo
		uint32 senderPacketsInTxStFifo:8;	// sender_packets_in_tx_st_fifo
		uint32 reserved0:8;
	} bitFields;
} RegBaAnalyzerPacketsInTxStFifo_u;

/*REG_BA_ANALYZER_UMAC_PACKET_READ_REG 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacPacketToSubstract:8;	// umac_packet_to_substract
		uint32 umacPacketRdStrb:1;	// umac_packet_rd_strb
		uint32 reserved0:3;
		uint32 umacRtAdaptPacketToSubstract:8;	// umac_rt_adapt_packet_to_substract
		uint32 umacRtAdaptPacketRdStrb:1;	// umac_rt_adapt_packet_rd_strb
		uint32 clrRateAdaptNumPacketDrop:1;	// clr_rate_adapt_num_packet_drop
		uint32 reserved1:10;
	} bitFields;
} RegBaAnalyzerUmacPacketReadReg_u;

/*REG_BA_ANALYZER_LMAC_PACKET_READ_REG 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacPacketToSubstract:8;	// lmac_packet_to_substract
		uint32 lmacPacketRdStrb:1;	// lmac_packet_rd_strb
		uint32 reserved0:23;
	} bitFields;
} RegBaAnalyzerLmacPacketReadReg_u;

/*REG_BA_ANALYZER_SENDER_PACKET_READ_REG 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderPacketToSubstract:8;	// sender_packet_to_substract
		uint32 senderPacketRdStrb:1;	// sender_packet_rd_strb
		uint32 reserved0:23;
	} bitFields;
} RegBaAnalyzerSenderPacketReadReg_u;

/*REG_BA_ANALYZER_LMAC_INT_STATUS 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacTxStNotEmpty:1;	// lmac_tx_st_not_empty
		uint32 lmacRateAdaptiveFifoNotEmpty:1;	// lmac_tx_st_not_empty
		uint32 reserved0:30;
	} bitFields;
} RegBaAnalyzerLmacIntStatus_u;

/*REG_BA_ANALYZER_LMAC_INT_EN 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacTxStNotEmptyEn:1;	// lmac_tx_st_not_empty_en
		uint32 lmacRateAdaptiveFifoNotEmptyEn:1;	// lmac_tx_st_not_empty_en
		uint32 reserved0:30;
	} bitFields;
} RegBaAnalyzerLmacIntEn_u;

/*REG_BA_ANALYZER_SENDER_INT_STATUS 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderTxStNotEmpty:1;	// sender_tx_st_not_empty
		uint32 reserved0:31;
	} bitFields;
} RegBaAnalyzerSenderIntStatus_u;

/*REG_BA_ANALYZER_SENDER_INT_EN 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderTxStNotEmptyEn:1;	// sender_tx_st_not_empty_en
		uint32 reserved0:31;
	} bitFields;
} RegBaAnalyzerSenderIntEn_u;

/*REG_BA_ANALYZER_RX_LIB_DPL_IDX_SOURCE0_3 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLiberatorDplIdxSource0:5;	// rx_liberator_dpl_idx_source0
		uint32 reserved0:3;
		uint32 rxLiberatorDplIdxSource1:5;	// rx_liberator_dpl_idx_source1
		uint32 reserved1:3;
		uint32 rxLiberatorDplIdxSource2:5;	// rx_liberator_dpl_idx_source2
		uint32 reserved2:3;
		uint32 rxLiberatorDplIdxSource3:5;	// rx_liberator_dpl_idx_source3
		uint32 reserved3:3;
	} bitFields;
} RegBaAnalyzerRxLibDplIdxSource03_u;

/*REG_BA_ANALYZER_RX_LIB_DPL_IDX_SOURCE4_7 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLiberatorDplIdxSource4:5;	// rx_liberator_dpl_idx_source4
		uint32 reserved0:3;
		uint32 rxLiberatorDplIdxSource5:5;	// rx_liberator_dpl_idx_source5
		uint32 reserved1:3;
		uint32 rxLiberatorDplIdxSource6:5;	// rx_liberator_dpl_idx_source6
		uint32 reserved2:3;
		uint32 rxLiberatorDplIdxSource7:5;	// rx_liberator_dpl_idx_source7
		uint32 reserved3:3;
	} bitFields;
} RegBaAnalyzerRxLibDplIdxSource47_u;

/*REG_BA_ANALYZER_RX_LIB_DPL_IDX_INPUT 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLiberatorDplIdxIn:5;	// rx_liberator_dpl_idx_in
		uint32 reserved0:3;
		uint32 rxLiberatorDplIdxToAdd:5;	// rx_liberator_dpl_idx_to_add
		uint32 reserved1:19;
	} bitFields;
} RegBaAnalyzerRxLibDplIdxInput_u;

/*REG_BA_ANALYZER_RX_LIB_HALT 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLiberatorHaltWork:1;	// halt in oder to update id on the fly 
		uint32 reserved0:31;
	} bitFields;
} RegBaAnalyzerRxLibHalt_u;

/*REG_BA_ANALYZER_SHORT_RETRY_CNT_PD_TYPE0_7 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortRetryCntPdType0:4;	// short_retry_cnt_pd_type0
		uint32 shortRetryCntPdType1:4;	// short_retry_cnt_pd_type1
		uint32 shortRetryCntPdType2:4;	// short_retry_cnt_pd_type2
		uint32 shortRetryCntPdType3:4;	// short_retry_cnt_pd_type3
		uint32 shortRetryCntPdType4:4;	// short_retry_cnt_pd_type4
		uint32 shortRetryCntPdType5:4;	// short_retry_cnt_pd_type5
		uint32 shortRetryCntPdType6:4;	// short_retry_cnt_pd_type6
		uint32 shortRetryCntPdType7:4;	// short_retry_cnt_pd_type7
	} bitFields;
} RegBaAnalyzerShortRetryCntPdType07_u;

/*REG_BA_ANALYZER_SHORT_RETRY_CNT_PD_TYPE8_15 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortRetryCntPdType8:4;	// short_retry_cnt_pd_type8
		uint32 shortRetryCntPdType9:4;	// short_retry_cnt_pd_type9
		uint32 shortRetryCntPdType10:4;	// short_retry_cnt_pd_type10
		uint32 shortRetryCntPdType11:4;	// short_retry_cnt_pd_type11
		uint32 shortRetryCntPdType12:4;	// short_retry_cnt_pd_type12
		uint32 shortRetryCntPdType13:4;	// short_retry_cnt_pd_type13
		uint32 shortRetryCntPdType14:4;	// short_retry_cnt_pd_type14
		uint32 shortRetryCntPdType15:4;	// short_retry_cnt_pd_type15
	} bitFields;
} RegBaAnalyzerShortRetryCntPdType815_u;

/*REG_BA_ANALYZER_BA_ANALYZER_SPARE 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baAnalyzerSpare:32;	// ba_analyzer_spare
	} bitFields;
} RegBaAnalyzerBaAnalyzerSpare_u;

/*REG_BA_ANALYZER_BA_ANALYZER_COUNTERS_EN 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baAnalyzerCountersEn:26;	// ba_analyzer_counters_en
		uint32 reserved0:6;
	} bitFields;
} RegBaAnalyzerBaAnalyzerCountersEn_u;

/*REG_BA_ANALYZER_LOGGER_REG 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baAnalyzerLoggerRegEn:1;	// ba_analyzer_logger_reg_en
		uint32 baAnalyzerLoggerRegPriority:2;	// ba_analyzer_logger_reg_priority
		uint32 baAnalyzerLoggerRegActive:1;	// ba_analyzer_logger_reg_active
		uint32 liberatorLoggerRegEn:1;	// liberator_logger_reg_en
		uint32 liberatorLoggerRegPriority:2;	// liberator_logger_reg_priority
		uint32 liberatorLoggerRegActive:1;	// liberator_logger_reg_active
		uint32 rxLiberatorLoggerRegEn:1;	// rx_liberator_logger_reg_en
		uint32 rxLiberatorLoggerRegPriority:2;	// rx_liberator_logger_reg_priority
		uint32 rxLiberatorLoggerRegActive:1;	// rx_liberator_logger_reg_active
		uint32 reserved0:20;
	} bitFields;
} RegBaAnalyzerLoggerReg_u;

/*REG_BA_ANALYZER_LIBERATOR_MAX_MESSAGE_TO_LOGGER 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorMaxMessageToLogger:8;	// liberator_max_message_to_logger
		uint32 reserved0:24;
	} bitFields;
} RegBaAnalyzerLiberatorMaxMessageToLogger_u;

/*REG_BA_ANALYZER_RT_ADAPT_SENDER_BITS 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtAdaptSenderBits:16;	// rt_adapt_sender_bits
		uint32 reserved0:16;
	} bitFields;
} RegBaAnalyzerRtAdaptSenderBits_u;

/*REG_BA_ANALYZER_RA_FIFO_START_ADDR 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 raFifoStartAddr:24;	// ra_fifo_start_addr
		uint32 raFifoPacketSize:8;	// ra_fifo_packet_size
	} bitFields;
} RegBaAnalyzerRaFifoStartAddr_u;

/*REG_BA_ANALYZER_RATE_ADAPT_DEBUG_REG 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 raFifoClearStrb:1;	// ra_fifo_clear_strb
		uint32 raClearDecLessThanZeroStrb:1;	// ra_clear_dec_less_than_zero_strb
		uint32 raDecrementLessThanZero:1;	// ra_decrement_less_than_zero
		uint32 reserved0:29;
	} bitFields;
} RegBaAnalyzerRateAdaptDebugReg_u;



#endif // _BA_ANALYZER_REGS_H_
