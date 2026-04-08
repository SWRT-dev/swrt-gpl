
/***********************************************************************************
File:				BsrcRegsRegs.h
Module:				BsrcRegs
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _BSRC_REGS_REGS_H_
#define _BSRC_REGS_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define BSRC_REGS_BASE_ADDRESS                             MEMORY_MAP_UNIT_16_BASE_ADDRESS
#define	REG_BSRC_REGS_GLOBAL                            (BSRC_REGS_BASE_ADDRESS + 0x0)
#define	REG_BSRC_REGS_INT_STATUS                        (BSRC_REGS_BASE_ADDRESS + 0x4)
#define	REG_BSRC_REGS_INT_EN                            (BSRC_REGS_BASE_ADDRESS + 0x8)
#define	REG_BSRC_REGS_CLEAR_INT                         (BSRC_REGS_BASE_ADDRESS + 0xC)
#define	REG_BSRC_REGS_INT_ERROR_STATUS                  (BSRC_REGS_BASE_ADDRESS + 0x10)
#define	REG_BSRC_REGS_INT_ERROR_EN                      (BSRC_REGS_BASE_ADDRESS + 0x14)
#define	REG_BSRC_REGS_CLEAR_INT_ERROR                   (BSRC_REGS_BASE_ADDRESS + 0x18)
#define	REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_31_0       (BSRC_REGS_BASE_ADDRESS + 0x1C)
#define	REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_63_32      (BSRC_REGS_BASE_ADDRESS + 0x20)
#define	REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_95_64      (BSRC_REGS_BASE_ADDRESS + 0x24)
#define	REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_127_96     (BSRC_REGS_BASE_ADDRESS + 0x28)
#define	REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_159_128    (BSRC_REGS_BASE_ADDRESS + 0x2C)
#define	REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_191_160    (BSRC_REGS_BASE_ADDRESS + 0x30)
#define	REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_223_192    (BSRC_REGS_BASE_ADDRESS + 0x34)
#define	REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_255_224    (BSRC_REGS_BASE_ADDRESS + 0x38)
#define	REG_BSRC_REGS_AGER_CFG                          (BSRC_REGS_BASE_ADDRESS + 0x3C)
#define	REG_BSRC_REGS_FW_INDIRECT_ACCESS                (BSRC_REGS_BASE_ADDRESS + 0x40)
#define	REG_BSRC_REGS_FW_AGER_PERIOD                    (BSRC_REGS_BASE_ADDRESS + 0x44)
#define	REG_BSRC_REGS_FW_BUF_STS_CNT_1_0                (BSRC_REGS_BASE_ADDRESS + 0x48)
#define	REG_BSRC_REGS_FW_BUF_STS_CNT_3_2                (BSRC_REGS_BASE_ADDRESS + 0x4C)
#define	REG_BSRC_REGS_FW_BUF_STS_CNT_5_4                (BSRC_REGS_BASE_ADDRESS + 0x50)
#define	REG_BSRC_REGS_FW_BUF_STS_CNT_7_6                (BSRC_REGS_BASE_ADDRESS + 0x54)
#define	REG_BSRC_REGS_LOGGER_REG                        (BSRC_REGS_BASE_ADDRESS + 0x58)
#define	REG_BSRC_REGS_LOGGER_FILTER                     (BSRC_REGS_BASE_ADDRESS + 0x5C)
#define	REG_BSRC_REGS_FIFO_STATUS                       (BSRC_REGS_BASE_ADDRESS + 0x60)
#define	REG_BSRC_REGS_FSM_STATUS                        (BSRC_REGS_BASE_ADDRESS + 0x64)
#define	REG_BSRC_REGS_SPARE_REG                         (BSRC_REGS_BASE_ADDRESS + 0x70)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_BSRC_REGS_GLOBAL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBlockEn : 1; //BSRC block enable., reset value: 0x1, access type: RW
		uint32 crBufStsCompensationEn : 1; //Buffer Status compensation enable., reset value: 0x1, access type: RW
		uint32 crMaxMpduHdrLength : 8; //Max MPDU header length, to be decremented from MPDU full length received from RxC, for Buffer Status counters update., reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegBsrcRegsGlobal_u;

/*REG_BSRC_REGS_INT_STATUS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srFwDoneInt : 1; //FW indirect access has completed., reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegBsrcRegsIntStatus_u;

/*REG_BSRC_REGS_INT_EN 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwDoneIntEn : 1; //Enable fw_done_int., reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegBsrcRegsIntEn_u;

/*REG_BSRC_REGS_CLEAR_INT 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crClrFwDoneInt : 1; //Clear fw_done_int., reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegBsrcRegsClearInt_u;

/*REG_BSRC_REGS_INT_ERROR_STATUS 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srStartWhileRxcFifoNotEmptyInt : 1; //Got bsrc_start pulse from RxC while RxC FIFO was not empty., reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegBsrcRegsIntErrorStatus_u;

/*REG_BSRC_REGS_INT_ERROR_EN 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crStartWhileRxcFifoNotEmptyIntEn : 1; //Enable start_while_rxc_fifo_not_empty_int., reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegBsrcRegsIntErrorEn_u;

/*REG_BSRC_REGS_CLEAR_INT_ERROR 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crClrStartWhileRxcFifoNotEmptyInt : 1; //Clear start_while_rxc_fifo_not_empty_int., reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegBsrcRegsClearIntError_u;

/*REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_31_0 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBufStsSwInitUpdate310 : 32; //Bitmap per station. If set init is done by SW and not by first Queue Size encountered per Rx session., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsBufStsSwInitUpdate310_u;

/*REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_63_32 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBufStsSwInitUpdate6332 : 32; //Bitmap per station. If set init is done by SW and not by first Queue Size encountered per Rx session., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsBufStsSwInitUpdate6332_u;

/*REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_95_64 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBufStsSwInitUpdate9564 : 32; //Bitmap per station. If set init is done by SW and not by first Queue Size encountered per Rx session., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsBufStsSwInitUpdate9564_u;

/*REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_127_96 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBufStsSwInitUpdate12796 : 32; //Bitmap per station. If set init is done by SW and not by first Queue Size encountered per Rx session., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsBufStsSwInitUpdate12796_u;

/*REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_159_128 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBufStsSwInitUpdate159128 : 32; //Bitmap per station. If set init is done by SW and not by first Queue Size encountered per Rx session., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsBufStsSwInitUpdate159128_u;

/*REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_191_160 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBufStsSwInitUpdate191160 : 32; //Bitmap per station. If set init is done by SW and not by first Queue Size encountered per Rx session., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsBufStsSwInitUpdate191160_u;

/*REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_223_192 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBufStsSwInitUpdate223192 : 32; //Bitmap per station. If set init is done by SW and not by first Queue Size encountered per Rx session., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsBufStsSwInitUpdate223192_u;

/*REG_BSRC_REGS_BUF_STS_SW_INIT_UPDATE_255_224 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBufStsSwInitUpdate255224 : 32; //Bitmap per station. If set init is done by SW and not by first Queue Size encountered per Rx session., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsBufStsSwInitUpdate255224_u;

/*REG_BSRC_REGS_AGER_CFG 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crAgerCntEn : 1; //Ager counters enable., reset value: 0x1, access type: RW
		uint32 crAgerStrobeCnt : 15; //Ager strobe counter value. Resolution is 1usec. Default is 9999, meaning once per 10msec Ager counters are decremented by 1., reset value: 0x270F, access type: RW
		uint32 crAgerPeriod : 16; //Ager period in 10msec resolution., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsAgerCfg_u;

/*REG_BSRC_REGS_FW_INDIRECT_ACCESS 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fwRxTidBitmap : 8; //Rx TID bitmap in FW indirect access., reset value: 0x0, access type: WO
		uint32 fwRxStaid : 9; //Rx Station ID in FW indirect access., reset value: 0x0, access type: WO
		uint32 fwCommand : 3; //Command in FW indirect access: , 0 - Mask , 1 - Unmask , 2 - Clear , 3 - Set , 4 - Init, reset value: 0x0, access type: WO
		uint32 reserved0 : 12;
	} bitFields;
} RegBsrcRegsFwIndirectAccess_u;

/*REG_BSRC_REGS_FW_AGER_PERIOD 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwClrAgerPeriod : 16; //Ager clear period in 10msec resolution., reset value: 0x0, access type: RW
		uint32 crFwSetAgerPeriod : 16; //Ager set period in 10msec resolution., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsFwAgerPeriod_u;

/*REG_BSRC_REGS_FW_BUF_STS_CNT_1_0 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwBufStsCnt0 : 16; //Buffer Status counter for FW init command for TID #0., reset value: 0x0, access type: RW
		uint32 crFwBufStsCnt1 : 16; //Buffer Status counter for FW init command for TID #1., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsFwBufStsCnt10_u;

/*REG_BSRC_REGS_FW_BUF_STS_CNT_3_2 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwBufStsCnt2 : 16; //Buffer Status counter for FW init command for TID #2., reset value: 0x0, access type: RW
		uint32 crFwBufStsCnt3 : 16; //Buffer Status counter for FW init command for TID #3., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsFwBufStsCnt32_u;

/*REG_BSRC_REGS_FW_BUF_STS_CNT_5_4 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwBufStsCnt4 : 16; //Buffer Status counter for FW init command for TID #4., reset value: 0x0, access type: RW
		uint32 crFwBufStsCnt5 : 16; //Buffer Status counter for FW init command for TID #5., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsFwBufStsCnt54_u;

/*REG_BSRC_REGS_FW_BUF_STS_CNT_7_6 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwBufStsCnt6 : 16; //Buffer Status counter for FW init command for TID #6., reset value: 0x0, access type: RW
		uint32 crFwBufStsCnt7 : 16; //Buffer Status counter for FW init command for TID #7., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsFwBufStsCnt76_u;

/*REG_BSRC_REGS_LOGGER_REG 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bsrcLoggerRegEn : 1; //BSRC logger enable, reset value: 0x0, access type: RW
		uint32 bsrcLoggerRegPriority : 2; //BSRC logger priority, reset value: 0x0, access type: RW
		uint32 bsrcLoggerRegActive : 1; //BSRC logger active, reset value: 0x0, access type: RO
		uint32 crLogMaxNumOfMsgs : 8; //Max number of messages in logger per report, while there are two words of 32 bits per message., reset value: 0x20, access type: RW
		uint32 crLogTimeoutVal : 16; //Timeout value in 1 usec resolution. When there was no messages sent within that period logger report will be closed., reset value: 0x3ff, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegBsrcRegsLoggerReg_u;

/*REG_BSRC_REGS_LOGGER_FILTER 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crLogStaid : 9; //Filter only Stations with Station ID equals cr_log_staid, reset value: 0x0, access type: RW
		uint32 crLogAllStaid : 1; //Log all Station IDs, reset value: 0x1, access type: RW
		uint32 crLogTidBitmap : 9; //Filter only TIDs set in the bitmap, reset value: 0x1ff, access type: RW
		uint32 crLogRxcStrbOnly : 1; //Filter only Buffer status counters update origin from RxC, reset value: 0x1, access type: RW
		uint32 crLogFwCmdOnly : 1; //Filter only Buffer status counters update origin from FW, reset value: 0x1, access type: RW
		uint32 crLogAgerClrOnly : 1; //Filter only Buffer status counters update origin from Ager, reset value: 0x1, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegBsrcRegsLoggerFilter_u;

/*REG_BSRC_REGS_FIFO_STATUS 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srRxcFifoEmpty : 1; //RxC FIFO empty indication., reset value: 0x1, access type: RO
		uint32 srRxcFifoFull : 1; //RxC FIFO full indication., reset value: 0x0, access type: RO
		uint32 srRxcFifoBytes : 3; //Valid entries in RxC FIFO., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 srFwFifoEmpty : 1; //FW FIFO empty indication., reset value: 0x1, access type: RO
		uint32 srFwFifoFull : 1; //FW FIFO full indication., reset value: 0x0, access type: RO
		uint32 srFwFifoBytes : 3; //Valid entries in FW FIFO., reset value: 0x0, access type: RO
		uint32 reserved1 : 3;
		uint32 srAgerFifoEmpty : 1; //Ager FIFO empty indication., reset value: 0x1, access type: RO
		uint32 srAgerFifoFull : 1; //Ager FIFO full indication., reset value: 0x0, access type: RO
		uint32 srAgerFifoBytes : 2; //Valid entries in Ager FIFO., reset value: 0x0, access type: RO
		uint32 reserved2 : 4;
		uint32 srBitselFifoEmpty : 1; //Bit Selector FIFO empty indication., reset value: 0x1, access type: RO
		uint32 srBitselFifoFull : 1; //Bit Selector FIFO full indication., reset value: 0x0, access type: RO
		uint32 srBitselFifoBytes : 2; //Valid entries in Bit Selector FIFO., reset value: 0x0, access type: RO
		uint32 reserved3 : 4;
	} bitFields;
} RegBsrcRegsFifoStatus_u;

/*REG_BSRC_REGS_FSM_STATUS 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srMainFsmState : 3; //Main FSM state., reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 srAgerFsmState : 2; //Ager FSM state., reset value: 0x0, access type: RO
		uint32 reserved1 : 26;
	} bitFields;
} RegBsrcRegsFsmStatus_u;

/*REG_BSRC_REGS_SPARE_REG 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crSpareReg : 32; //Spare register., reset value: 0x88888888, access type: RW
	} bitFields;
} RegBsrcRegsSpareReg_u;



#endif // _BSRC_REGS_REGS_H_
