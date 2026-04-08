
/***********************************************************************************
File:				TxhTxcRegs.h
Module:				txhTxc
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TXH_TXC_REGS_H_
#define _TXH_TXC_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TXH_TXC_BASE_ADDRESS                             MEMORY_MAP_UNIT_53_BASE_ADDRESS
#define	REG_TXH_TXC_TXH_TXC_ENABLE               (TXH_TXC_BASE_ADDRESS + 0x0)
#define	REG_TXH_TXC_ERROR_IFS1                   (TXH_TXC_BASE_ADDRESS + 0x4)
#define	REG_TXH_TXC_ERROR_IFS2                   (TXH_TXC_BASE_ADDRESS + 0x8)
#define	REG_TXH_TXC_ERROR_IFS3                   (TXH_TXC_BASE_ADDRESS + 0xC)
#define	REG_TXH_TXC_SIFS_ELEMENT                 (TXH_TXC_BASE_ADDRESS + 0x10)
#define	REG_TXH_TXC_TXC_SENSITIVE_DEFAULT        (TXH_TXC_BASE_ADDRESS + 0x20)
#define	REG_TXH_TXC_RESP_TIMEOUT_VALUE_SET       (TXH_TXC_BASE_ADDRESS + 0x24)
#define	REG_TXH_TXC_RESP_TIMEOUT_RD              (TXH_TXC_BASE_ADDRESS + 0x28)
#define	REG_TXH_TXC_TXC_DEBUG                    (TXH_TXC_BASE_ADDRESS + 0x2C)
#define	REG_TXH_TXC_START_SLOT_AFTER_SIFS_CTR    (TXH_TXC_BASE_ADDRESS + 0x30)
#define	REG_TXH_TXC_START_SLOT_AFTER_SLOT_CTR    (TXH_TXC_BASE_ADDRESS + 0x34)
#define	REG_TXH_TXC_SET_SMC_USP_CIR_BUF_AVAIL    (TXH_TXC_BASE_ADDRESS + 0x38)
#define	REG_TXH_TXC_TXC_SMC_USP_CIR_BUF_AVAIL    (TXH_TXC_BASE_ADDRESS + 0x3C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TXH_TXC_TXH_TXC_ENABLE 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhTxcEnable:1;	// TXH TXC enable
		uint32 reserved0:31;
	} bitFields;
} RegTxhTxcTxhTxcEnable_u;

/*REG_TXH_TXC_ERROR_IFS1 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errorIfs11BShort:13;	// error IFS 11b short
		uint32 reserved0:3;
		uint32 errorIfs11BLong:13;	// error IFS 11b long
		uint32 reserved1:3;
	} bitFields;
} RegTxhTxcErrorIfs1_u;

/*REG_TXH_TXC_ERROR_IFS2 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errorIfs11Ag:13;	// error IFS 11a/g
		uint32 reserved0:3;
		uint32 errorIfs11N:13;	// error IFS 11n
		uint32 reserved1:3;
	} bitFields;
} RegTxhTxcErrorIfs2_u;

/*REG_TXH_TXC_ERROR_IFS3 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errorIfs11Ac:13;	// error IFS 11ac
		uint32 reserved0:19;
	} bitFields;
} RegTxhTxcErrorIfs3_u;

/*REG_TXH_TXC_SIFS_ELEMENT 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sifsElement:8;	// SIFS element
		uint32 reserved0:24;
	} bitFields;
} RegTxhTxcSifsElement_u;

/*REG_TXH_TXC_TXC_SENSITIVE_DEFAULT 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcCcaSensitiveDefault:1;	// CCA sensitive default
		uint32 txcRxNavSensitiveDefault:1;	// RX NAV sensitive default
		uint32 txcTxNavSensitiveDefault:1;	// TX NAV sensitive default
		uint32 reserved0:29;
	} bitFields;
} RegTxhTxcTxcSensitiveDefault_u;

/*REG_TXH_TXC_RESP_TIMEOUT_VALUE_SET 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 respTimeoutValueSet:13;	// Response timeout set
		uint32 reserved0:19;
	} bitFields;
} RegTxhTxcRespTimeoutValueSet_u;

/*REG_TXH_TXC_RESP_TIMEOUT_RD 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 respTimeoutRd:13;	// Response timeout read
		uint32 reserved0:3;
		uint32 respTimeoutEnRd:1;	// Response timeout enable read
		uint32 reserved1:15;
	} bitFields;
} RegTxhTxcRespTimeoutRd_u;

/*REG_TXH_TXC_TXC_DEBUG 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSm:4;	// TXC state machine
		uint32 reserved0:28;
	} bitFields;
} RegTxhTxcTxcDebug_u;

/*REG_TXH_TXC_START_SLOT_AFTER_SIFS_CTR 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startSlotAfterSifsCtr:16;	// Counter of Start SLOT after SIFS events from TXC to TxH
		uint32 reserved0:16;
	} bitFields;
} RegTxhTxcStartSlotAfterSifsCtr_u;

/*REG_TXH_TXC_START_SLOT_AFTER_SLOT_CTR 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startSlotAfterSlotCtr:24;	// Counter of Start SLOT after SLOT events from TXC to TxH
		uint32 reserved0:8;
	} bitFields;
} RegTxhTxcStartSlotAfterSlotCtr_u;

/*REG_TXH_TXC_SET_SMC_USP_CIR_BUF_AVAIL 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setSmcUspCirBufAvail:4;	// Set which Tx circular buffers are available
		uint32 reserved0:28;
	} bitFields;
} RegTxhTxcSetSmcUspCirBufAvail_u;

/*REG_TXH_TXC_TXC_SMC_USP_CIR_BUF_AVAIL 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSmcUspCirBufAvail:4;	// User positions Tx circular buffers available
		uint32 reserved0:28;
	} bitFields;
} RegTxhTxcTxcSmcUspCirBufAvail_u;



#endif // _TXH_TXC_REGS_H_
