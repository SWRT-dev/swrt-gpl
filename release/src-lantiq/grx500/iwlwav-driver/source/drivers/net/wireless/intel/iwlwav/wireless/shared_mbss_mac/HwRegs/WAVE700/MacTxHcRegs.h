
/***********************************************************************************
File:				MacTxHcRegs.h
Module:				macTxHc
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_TX_HC_REGS_H_
#define _MAC_TX_HC_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_TX_HC_BASE_ADDRESS                             MEMORY_MAP_UNIT_20_BASE_ADDRESS
#define	REG_MAC_TX_HC_FRAME_MONITOR_OFFSET              (MAC_TX_HC_BASE_ADDRESS + 0x0)
#define	REG_MAC_TX_HC_ETHERTYPE_FOR_SNAP_1_0            (MAC_TX_HC_BASE_ADDRESS + 0x4)
#define	REG_MAC_TX_HC_ETHERTYPE_FOR_SNAP_3_2            (MAC_TX_HC_BASE_ADDRESS + 0x8)
#define	REG_MAC_TX_HC_ETHERTYPE_FOR_SNAP_4              (MAC_TX_HC_BASE_ADDRESS + 0xC)
#define	REG_MAC_TX_HC_LLC_SNAP_PREFIX_31TO0_0           (MAC_TX_HC_BASE_ADDRESS + 0x10)
#define	REG_MAC_TX_HC_LLC_SNAP_PREFIX_31TO0_1           (MAC_TX_HC_BASE_ADDRESS + 0x14)
#define	REG_MAC_TX_HC_LLC_SNAP_PREFIX_47TO32_1_0        (MAC_TX_HC_BASE_ADDRESS + 0x18)
#define	REG_MAC_TX_HC_LLC_SNAP_PREFIX_31TO0_2           (MAC_TX_HC_BASE_ADDRESS + 0x1C)
#define	REG_MAC_TX_HC_LLC_SNAP_PREFIX_31TO0_3           (MAC_TX_HC_BASE_ADDRESS + 0x20)
#define	REG_MAC_TX_HC_LLC_SNAP_PREFIX_47TO32_3_2        (MAC_TX_HC_BASE_ADDRESS + 0x24)
#define	REG_MAC_TX_HC_LLC_SNAP_PREFIX_31TO0_4           (MAC_TX_HC_BASE_ADDRESS + 0x28)
#define	REG_MAC_TX_HC_LLC_SNAP_PREFIX_47TO32_4          (MAC_TX_HC_BASE_ADDRESS + 0x2C)
#define	REG_MAC_TX_HC_LLC_SNAP_DEFAULT_PREFIX_31TO0     (MAC_TX_HC_BASE_ADDRESS + 0x30)
#define	REG_MAC_TX_HC_LLC_SNAP_DEFAULT_PREFIX_47TO32    (MAC_TX_HC_BASE_ADDRESS + 0x34)
#define	REG_MAC_TX_HC_TX_HC_FLOW_CONTROL_CMD            (MAC_TX_HC_BASE_ADDRESS + 0x38)
#define	REG_MAC_TX_HC_PPDU_STA_QUEUE_MODE               (MAC_TX_HC_BASE_ADDRESS + 0x40)
#define	REG_MAC_TX_HC_VAP_DB_PARAMS                     (MAC_TX_HC_BASE_ADDRESS + 0x44)
#define	REG_MAC_TX_HC_WAPI_CONTROL                      (MAC_TX_HC_BASE_ADDRESS + 0x48)
#define	REG_MAC_TX_HC_HC_SENDER_DESC_FIFO_PARAMS        (MAC_TX_HC_BASE_ADDRESS + 0x50)
#define	REG_MAC_TX_HC_HC_SENDER_DESC_RD_ENTRIES_NUM     (MAC_TX_HC_BASE_ADDRESS + 0x54)
#define	REG_MAC_TX_HC_HC_SENDER_DESC_FIFO_STATUS        (MAC_TX_HC_BASE_ADDRESS + 0x58)
#define	REG_MAC_TX_HC_TX_HC_CONTROL_BITS                (MAC_TX_HC_BASE_ADDRESS + 0x5C)
#define	REG_MAC_TX_HC_TX_HC_STATUS_0                    (MAC_TX_HC_BASE_ADDRESS + 0x60)
#define	REG_MAC_TX_HC_TX_HC_STATUS_1                    (MAC_TX_HC_BASE_ADDRESS + 0x64)
#define	REG_MAC_TX_HC_TX_HC_LOGGER_CONTROL              (MAC_TX_HC_BASE_ADDRESS + 0x68)
#define	REG_MAC_TX_HC_TX_HC_LOGGER_STATUS               (MAC_TX_HC_BASE_ADDRESS + 0x6C)
#define	REG_MAC_TX_HC_TX_HC_INT_ERROR_STATUS            (MAC_TX_HC_BASE_ADDRESS + 0x70)
#define	REG_MAC_TX_HC_TX_HC_INT_ERROR_EN                (MAC_TX_HC_BASE_ADDRESS + 0x74)
#define	REG_MAC_TX_HC_TX_HC_INT_ERROR_CLEAR             (MAC_TX_HC_BASE_ADDRESS + 0x78)
#define	REG_MAC_TX_HC_TX_HC_SPARE_REGISTER              (MAC_TX_HC_BASE_ADDRESS + 0x7C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_TX_HC_FRAME_MONITOR_OFFSET 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataFrameMonitorOffset : 8; //Number of bytes to wait (until being written by the DMA) beyond DMA START POINTER. , This value is used in case of data frames. , The default value is 14B, since it is necessary to wait until {DA, SA, Etype} are written by the DMA. The actual value which is waited is (Start_DMA_Pointer + MIN{Data_Frame_Offset, DMA_Length}), reset value: 0xE, access type: RW
		uint32 nonDataFrameMonitorOffset : 8; //Number of bytes to wait (until being written by the DMA) beyond DMA START POINTER. , This value is used in case of non-data frames. , The default value is 24B, in order to resolve management frames, in which it is necessary to wait until the MAC Header (without HT Control and Security Header) is written by the DMA. The actual value which is waited is (Start_DMA_Pointer + MIN{Non_Data_Frame_Offset, DMA_Length}), reset value: 0x28, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacTxHcFrameMonitorOffset_u;

/*REG_MAC_TX_HC_ETHERTYPE_FOR_SNAP_1_0 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ethertypeForSnap0 : 16; //In case of data frames with ETH2, when the Ethertype matches this field (ethertype_for_snap_0), then llc_snap_prefix_0 shall be used as the 6 byte LLC/SNAP prefix, reset value: 0x80f3, access type: RW
		uint32 ethertypeForSnap1 : 16; //In case of data frames with ETH2, when the Ethertype matches this field (ethertype_for_snap_1), then llc_snap_prefix_1 shall be used as the 6 byte LLC/SNAP prefix, reset value: 0x8137, access type: RW
	} bitFields;
} RegMacTxHcEthertypeForSnap10_u;

/*REG_MAC_TX_HC_ETHERTYPE_FOR_SNAP_3_2 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ethertypeForSnap2 : 16; //In case of data frames with ETH2, when the Ethertype matches this field (ethertype_for_snap_2), then llc_snap_prefix_2 shall be used as the 6 byte LLC/SNAP prefix, reset value: 0x80f3, access type: RW
		uint32 ethertypeForSnap3 : 16; //In case of data frames with ETH2, when the Ethertype matches this field (ethertype_for_snap_3), then llc_snap_prefix_3 shall be used as the 6 byte LLC/SNAP prefix, reset value: 0x8137, access type: RW
	} bitFields;
} RegMacTxHcEthertypeForSnap32_u;

/*REG_MAC_TX_HC_ETHERTYPE_FOR_SNAP_4 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ethertypeForSnap4 : 16; //In case of data frames with ETH2, when the Ethertype matches this field (ethertype_for_snap_4), then llc_snap_prefix_4 shall be used as the 6 byte LLC/SNAP prefix, reset value: 0x80f3, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacTxHcEthertypeForSnap4_u;

/*REG_MAC_TX_HC_LLC_SNAP_PREFIX_31TO0_0 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 llcSnapPrefix31To00 : 32; //Bits 31:0 of llc_snap_prefix_0, which is the LLC/SNAP header that is inserted in case ethertype matches ethertype_for_snap_0 (in case of data frames with ETH2), reset value: 0x30000f8, access type: RW
	} bitFields;
} RegMacTxHcLlcSnapPrefix31To00_u;

/*REG_MAC_TX_HC_LLC_SNAP_PREFIX_31TO0_1 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 llcSnapPrefix31To01 : 32; //Bits 31:0 of llc_snap_prefix_1, which is the LLC/SNAP header that is inserted in case ethertype matches ethertype_for_snap_1 (in case of data frames with ETH2), reset value: 0x30000f8, access type: RW
	} bitFields;
} RegMacTxHcLlcSnapPrefix31To01_u;

/*REG_MAC_TX_HC_LLC_SNAP_PREFIX_47TO32_1_0 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 llcSnapPrefix47To320 : 16; //Bits 47:32 of llc_snap_prefix_0, which is the LLC/SNAP header that is inserted in case ethertype matches ethertype_for_snap_0 (in case of data frames with ETH2), reset value: 0xaaaa, access type: RW
		uint32 llcSnapPrefix47To321 : 16; //Bits 47:32 of llc_snap_prefix_1, which is the LLC/SNAP header that is inserted in case ethertype matches ethertype_for_snap_1 (in case of data frames with ETH2), reset value: 0xaaaa, access type: RW
	} bitFields;
} RegMacTxHcLlcSnapPrefix47To3210_u;

/*REG_MAC_TX_HC_LLC_SNAP_PREFIX_31TO0_2 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 llcSnapPrefix31To02 : 32; //Bits 31:0 of llc_snap_prefix_2, which is the LLC/SNAP header that is inserted in case ethertype matches ethertype_for_snap_2 (in case of data frames with ETH2), reset value: 0x30000f8, access type: RW
	} bitFields;
} RegMacTxHcLlcSnapPrefix31To02_u;

/*REG_MAC_TX_HC_LLC_SNAP_PREFIX_31TO0_3 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 llcSnapPrefix31To03 : 32; //Bits 31:0 of llc_snap_prefix_3, which is the LLC/SNAP header that is inserted in case ethertype matches ethertype_for_snap_3 (in case of data frames with ETH2), reset value: 0x30000f8, access type: RW
	} bitFields;
} RegMacTxHcLlcSnapPrefix31To03_u;

/*REG_MAC_TX_HC_LLC_SNAP_PREFIX_47TO32_3_2 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 llcSnapPrefix47To322 : 16; //Bits 47:32 of llc_snap_prefix_2, which is the LLC/SNAP header that is inserted in case ethertype matches ethertype_for_snap_2 (in case of data frames with ETH2), reset value: 0xaaaa, access type: RW
		uint32 llcSnapPrefix47To323 : 16; //Bits 47:32 of llc_snap_prefix_3, which is the LLC/SNAP header that is inserted in case ethertype matches ethertype_for_snap_3 (in case of data frames with ETH2), reset value: 0xaaaa, access type: RW
	} bitFields;
} RegMacTxHcLlcSnapPrefix47To3232_u;

/*REG_MAC_TX_HC_LLC_SNAP_PREFIX_31TO0_4 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 llcSnapPrefix31To04 : 32; //Bits 31:0 of llc_snap_prefix_4, which is the LLC/SNAP header that is inserted in case ethertype matches ethertype_for_snap_4 (in case of data frames with ETH2), reset value: 0x30000f8, access type: RW
	} bitFields;
} RegMacTxHcLlcSnapPrefix31To04_u;

/*REG_MAC_TX_HC_LLC_SNAP_PREFIX_47TO32_4 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 llcSnapPrefix47To324 : 16; //Bits 47:32 of llc_snap_prefix_4, which is the LLC/SNAP header that is inserted in case ethertype matches ethertype_for_snap_4 (in case of data frames with ETH2), reset value: 0xaaaa, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacTxHcLlcSnapPrefix47To324_u;

/*REG_MAC_TX_HC_LLC_SNAP_DEFAULT_PREFIX_31TO0 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 llcSnapDefaultPrefix31To0 : 32; //Bits 31:0 of llc_snap_default_prefix. This is the LLC/SNAP header which is inserted (in case of data frames with ETH2) if the frame's ethertype does not match any of the configured ethertype_for_snap_4:0, reset value: 0x3000000, access type: RW
	} bitFields;
} RegMacTxHcLlcSnapDefaultPrefix31To0_u;

/*REG_MAC_TX_HC_LLC_SNAP_DEFAULT_PREFIX_47TO32 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 llcSnapDefaultPrefix47To32 : 16; //Bits 47:32 of llc_snap_default_prefix. This is the LLC/SNAP header which is inserted (in case of data frames with ETH2) if the frame's ethertype does not match any of the configured ethertype_for_snap_4:0, reset value: 0xaaaa, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacTxHcLlcSnapDefaultPrefix47To32_u;

/*REG_MAC_TX_HC_TX_HC_FLOW_CONTROL_CMD 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txHcFlowControlWrPtr : 14; //Write Pointer value. When this register is written, the configured user_id entry of the output flow_control array is updated with the value of the wr_ptr , reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 txHcFlowControlUserId : 7; //User ID value. When this register is written, the configured user_id entry of the output flow_control array is updated with the value of the wr_ptr , reset value: 0x0, access type: RW
		uint32 reserved1 : 9;
	} bitFields;
} RegMacTxHcTxHcFlowControlCmd_u;

/*REG_MAC_TX_HC_PPDU_STA_QUEUE_MODE 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ppduStaQueueMode : 1; //Station queue indication. This field is configured before each PPDU in order to indicate whether the transmission is from Station or Non-Station queues. 0 - Non-Sta queue ;  1 - Sta queue, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacTxHcPpduStaQueueMode_u;

/*REG_MAC_TX_HC_VAP_DB_PARAMS 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapDbBssidOffset : 8; //DW offset to BSSID location within a VAP_DB entry, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacTxHcVapDbParams_u;

/*REG_MAC_TX_HC_WAPI_CONTROL 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiConstant : 16; //This field is the constant portion of the WAPI security header. This constant is repaeated 5 times within the WAPI header, reset value: 0x5c36, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacTxHcWapiControl_u;

/*REG_MAC_TX_HC_HC_SENDER_DESC_FIFO_PARAMS 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hcSenderDescFifoBaseAddr : 10; //Base Address of the HC_SENDER_DESC_FIFO within the RAM. This is a DW address., reset value: 0x0, access type: RW
		uint32 reserved0 : 6;
		uint32 hcSenderDescFifoDepthMinusOne : 9; //Depth of the FIFO minus one. Each unit designates a QW (since it designates a descriptor, which is 64 bits long), reset value: 0x7f, access type: RW
		uint32 reserved1 : 7;
	} bitFields;
} RegMacTxHcHcSenderDescFifoParams_u;

/*REG_MAC_TX_HC_HC_SENDER_DESC_RD_ENTRIES_NUM 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hcSenderDescRdEntriesNum : 10; //Number of entries to decrement. Each unit designates a QW (since it designates a descriptor, which is 64 bits long), reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegMacTxHcHcSenderDescRdEntriesNum_u;

/*REG_MAC_TX_HC_HC_SENDER_DESC_FIFO_STATUS 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hcSenderDescNumEntriesCount : 10; //Number of entries available for read in the FIFO. Each unit designates a QW (since it designates a descriptor, which is 64 bits long), reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
		uint32 hcSenderDescFifoWrPtr : 9; //FIFO Write pointer. Each unit designates a QW (since it designates a descriptor, which is 64 bits long), reset value: 0x0, access type: RO
		uint32 reserved1 : 3;
		uint32 hcSenderDescFifoNotEmpty : 1; //FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 hcSenderDescFifoDecrementLessThanZero : 1; //Number of entries decremented to less than zero (error indication). , Locked until cleared by hc_sender_desc_fifo_clear_dec_less_than_zero, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
	} bitFields;
} RegMacTxHcHcSenderDescFifoStatus_u;

/*REG_MAC_TX_HC_TX_HC_CONTROL_BITS 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hcSenderDescFifoClear : 1; //Write 1 to this field in order to clear the HC_SENDER_DESC_FIFO pointers, reset value: 0x0, access type: WO
		uint32 genriscFifoBypass : 1; //When set, fullness of genrisc_fifo is ignored and any pending writes to the memory shall always be written, reset value: 0x0, access type: RW
		uint32 headerMonitorBypass : 1; //When set, the header monitor is bypassed and the input flow control vector is not used, reset value: 0x0, access type: RW
		uint32 hcJobFifoClear : 1; //Write 1 to this field in order to clear the HC_JOB_FIFO, reset value: 0x0, access type: WO
		uint32 blockDmaJobs : 1; //When set, any incoming jobs shall be masked and not written to the HC_JOB_FIFO., reset value: 0x0, access type: RW
		uint32 blockHcJobFifoRead : 1; //When set, the HC_JOB_FIFO will not be read (but writing to it is still possible), reset value: 0x0, access type: RW
		uint32 txHcFlowControlClear : 1; //Write 1 to this field in order to zero the TX_HC output flow control array (all user_ids shall be zeroed, reset value: 0x0, access type: WO
		uint32 reserved0 : 1;
		uint32 hcSenderDescFifoClearDecLessThanZero : 1; //Write 1 to this field in order to clear hc_sender_desc_fifo_decrement_less_than_zero error, reset value: 0x0, access type: WO
		uint32 hcSenderDescFifoOccupancyMaxClear : 1; //Write 1 to this field in order to clear hc_sender_desc_fifo_occupancy_max, reset value: 0x0, access type: WO
		uint32 hcJobFifoOccupancyMaxClear : 1; //Write 1 to this field in order to clear hc_job_fifo_occupancy_max, reset value: 0x0, access type: WO
		uint32 reserved1 : 21;
	} bitFields;
} RegMacTxHcTxHcControlBits_u;

/*REG_MAC_TX_HC_TX_HC_STATUS_0 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hcJobFifoOccupancyLive : 6; //Live Occupancy of HC_JOB_FIFO, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 hcJobFifoOccupancyMax : 6; //Maximal Occupancy of HC_JOB_FIFO (Cleared by writing to hc_job_fifo_occupancy_max_clear), reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 hcSenderDescFifoOccupancyMax : 10; //Maximal Occupancy of HC_SENDER_DESC_FIFO (Cleared by writing to hc_sender_desc_fifo_occupancy_max_clear), reset value: 0x0, access type: RO
		uint32 reserved2 : 6;
	} bitFields;
} RegMacTxHcTxHcStatus0_u;

/*REG_MAC_TX_HC_TX_HC_STATUS_1 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txHcIsIdle : 1; //Indication that TX_HC block is Idle. Used as an indication for end of clear process, reset value: 0x1, access type: RO
		uint32 reserved0 : 3;
		uint32 txHcPipe0Sm : 3; //State of PIPE_0 State Machine, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 txHcPipe1RdSm : 3; //State of PIPE_1_RD State Machine, reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 txHcPipe1WrSm : 4; //State of PIPE_1_WR State Machine, reset value: 0x0, access type: RO
		uint32 txHcPipe2Sm : 1; //State of PIPE_2 State Machine, reset value: 0x0, access type: RO
		uint32 reserved3 : 3;
		uint32 txHcGenriscFifoSm : 2; //State of GENRISC_FIFO State Machine, reset value: 0x0, access type: RO
		uint32 reserved4 : 2;
		uint32 txHcStdSm : 3; //State of STD State Machine, reset value: 0x0, access type: RO
		uint32 reserved5 : 5;
	} bitFields;
} RegMacTxHcTxHcStatus1_u;

/*REG_MAC_TX_HC_TX_HC_LOGGER_CONTROL 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txHcLoggerEn : 1; //Logger Enable, reset value: 0x0, access type: RW
		uint32 txHcLoggerMaskMsdu : 1; //When this bit is set, any job with the 'Add MSDU Hdr' set, shall not be sent to the logger., reset value: 0x0, access type: RW
		uint32 txHcLoggerPriority : 2; //TX_HC Logger priority, reset value: 0x0, access type: RW
		uint32 txHcLoggerNumOfMsgsMinusOne : 5; //The maximal number of messages which will comprise a logger packet. If timeout (or logger disable) occurs before reaching this threshold, then the number of messages within a logger packet may be less than this threshold, reset value: 0xf, access type: RW
		uint32 reserved0 : 3;
		uint32 txHcLoggerTimeoutMinusOne : 13; //timeout value for a logger packet. It is measured in units of 1us. , If a logger packet is opened and does not reach the configured maximal number of messages, then timeout shall force the logger packet to be closed and to be sent. The timer starts once the first message within a logger packet is created., reset value: 0x3e7, access type: RW
		uint32 reserved1 : 7;
	} bitFields;
} RegMacTxHcTxHcLoggerControl_u;

/*REG_MAC_TX_HC_TX_HC_LOGGER_STATUS 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txHcLoggerIdle : 1; //Indication that the logger is IDLE. Use this indication after the logger is disabled, in order to verify that it has finished any work in progress., reset value: 0x1, access type: RO
		uint32 txHcLoggerSm : 3; //State of the logger state machine, reset value: 0x0, access type: RO
		uint32 txHcLoggerPacketOpen : 1; //Indication whether or not the logger holds an open packet. An open packet designates that some messages were already output by the logger, but the packet was not closed and sent yet., reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegMacTxHcTxHcLoggerStatus_u;

/*REG_MAC_TX_HC_TX_HC_INT_ERROR_STATUS 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txHcDmaLengthMismatchStatus : 1; //DMA Length mismatch occurs when bit 14 of the dma_length field is set within an incoming job, reset value: 0x0, access type: RO
		uint32 txHcMsduNonDataMismatchStatus : 1; //MSDU non-data mismatch occurs when a job instructs to add an MSDU header, but the frame_control field indicates that the frame is not a Data frame, reset value: 0x0, access type: RO
		uint32 txHcMpduNotFirstMsduMismatchStatus : 1; //MPDU not first MSDU mismatch occurs when a job instructs to add an MPDU header, but 'first MSDU in MPDU' flag is inactive , (this is not possible, since in case of MPDU insertion, the 'First MSDU' flag must be set), reset value: 0x0, access type: RO
		uint32 txHcJobFifoPushWhileFullStatus : 1; //HC_JOB_FIFO Push while full event, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacTxHcTxHcIntErrorStatus_u;

/*REG_MAC_TX_HC_TX_HC_INT_ERROR_EN 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txHcDmaLengthMismatchEn : 1; //DMA Length mismatch interrupt enable, reset value: 0x0, access type: RW
		uint32 txHcMsduNonDataMismatchEn : 1; //MSDU non-Data mismatch interrupt enable, reset value: 0x0, access type: RW
		uint32 txHcMpduNotFirstMsduMismatchEn : 1; //MPDU not first MSDU mismatch interrupt enable, reset value: 0x0, access type: RW
		uint32 txHcJobFifoPushWhileFullEn : 1; //Job FIFO Push while Full interrupt enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegMacTxHcTxHcIntErrorEn_u;

/*REG_MAC_TX_HC_TX_HC_INT_ERROR_CLEAR 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txHcDmaLengthMismatchClr : 1; //DMA Length mismatch interrupt clear, reset value: 0x0, access type: WO
		uint32 txHcMsduNonDataMismatchClr : 1; //MSDU non-Data mismatch interrupt clear, reset value: 0x0, access type: WO
		uint32 txHcMpduNotFirstMsduMismatchClr : 1; //MPDU not first MSDU mismatch interrupt clear, reset value: 0x0, access type: WO
		uint32 txHcJobFifoPushWhileFullClr : 1; //Job FIFO Push while Full interrupt clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacTxHcTxHcIntErrorClear_u;

/*REG_MAC_TX_HC_TX_HC_SPARE_REGISTER 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareRegister : 16; //Spare Register, reset value: 0x8888, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacTxHcTxHcSpareRegister_u;



#endif // _MAC_TX_HC_REGS_H_
