/***********************************************************************************
File:				PacTxcRegs.h
Module:				PacTxc
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PAC_TXC_REGS_H_
#define _PAC_TXC_REGS_H_

#include "HwMemoryMap.h"

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#define PAC_TXC_BASE_ADDRESS                             MEMORY_MAP_UNIT__BASE_ADDRESS
#define	REG_PAC_TXC_PTCS_POINTER_TABLE_BASE       (PAC_TXC_BASE_ADDRESS + 0x0)
#define	REG_PAC_TXC_STATUS                        (PAC_TXC_BASE_ADDRESS + 0x4)
#define	REG_PAC_TXC_EDCF_PRIORITY_ENABLE          (PAC_TXC_BASE_ADDRESS + 0x8)
#define	REG_PAC_TXC_ALT_BACKOFF_0                 (PAC_TXC_BASE_ADDRESS + 0xC)
#define	REG_PAC_TXC_ALT_BACKOFF_1                 (PAC_TXC_BASE_ADDRESS + 0x10)
#define	REG_PAC_TXC_ALT_BACKOFF_2                 (PAC_TXC_BASE_ADDRESS + 0x14)
#define	REG_PAC_TXC_EDCF_CONTEXT                  (PAC_TXC_BASE_ADDRESS + 0x18)
#define	REG_PAC_TXC_EDCF_PRIORITY_ENABLE_SET      (PAC_TXC_BASE_ADDRESS + 0x1C)
#define	REG_PAC_TXC_EDCF_PRIORITY_ENABLE_CLEAR    (PAC_TXC_BASE_ADDRESS + 0x20)
#define	REG_PAC_TXC_WME_REVISION                  (PAC_TXC_BASE_ADDRESS + 0x2C)
#define	REG_PAC_TXC_RXC_DEBUG_OVERRIDE_TXC        (PAC_TXC_BASE_ADDRESS + 0x30)
#define	REG_PAC_TXC_ENABLE_MODIFICATIONS          (PAC_TXC_BASE_ADDRESS + 0x34)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PAC_TXC_PTCS_POINTER_TABLE_BASE 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 ptcsPointerTableBase:14;	// TXC  ptcs pointer table base
		uint32 reserved1:10;
	} bitFields;
} RegPacTxcPtcsPointerTableBase_u;

/*REG_PAC_TXC_STATUS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcState:5;	// TXC  txc state
		uint32 reserved0:1;
		uint32 ifsComplete:1;	// TXC  ifs complete
		uint32 backoffComplete:1;	// TXC  backoff complete
		uint32 ptcsIndex:6;	// TXC  ptcs index
		uint32 lastRxCrcError:1;	// TXC  last rx crc error
		uint32 macClearChanAssess:1;	// TXC  mac clear chan assess
		uint32 txcPriority:2;	// TXC  priority
		uint32 reserved1:2;
		uint32 edcfBackoffComplete:4;	// TXC  edcf backoff complete
		uint32 reserved2:2;
		uint32 aifsComplete:4;	// TXC  aifs complete
		uint32 reserved3:2;
	} bitFields;
} RegPacTxcStatus_u;

/*REG_PAC_TXC_EDCF_PRIORITY_ENABLE 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 edcfPriorityEnable:4;	// TXC  edcf priority enable
		uint32 reserved0:28;
	} bitFields;
} RegPacTxcEdcfPriorityEnable_u;

/*REG_PAC_TXC_ALT_BACKOFF_0 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 altBackoff0:10;	// TXC  alt backoff 0
		uint32 reserved0:22;
	} bitFields;
} RegPacTxcAltBackoff0_u;

/*REG_PAC_TXC_ALT_BACKOFF_1 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 altBackoff1:10;	// TXC  alt backoff 1
		uint32 reserved0:22;
	} bitFields;
} RegPacTxcAltBackoff1_u;

/*REG_PAC_TXC_ALT_BACKOFF_2 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 altBackoff2:10;	// TXC  alt backoff 2
		uint32 reserved0:22;
	} bitFields;
} RegPacTxcAltBackoff2_u;

/*REG_PAC_TXC_EDCF_CONTEXT 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 edcfContext:4;	// TXC  edcf context
		uint32 coincidentPtcs:4;	// coincident ptcs
		uint32 reserved0:24;
	} bitFields;
} RegPacTxcEdcfContext_u;

/*REG_PAC_TXC_EDCF_PRIORITY_ENABLE_SET 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 edcfPriorityEnableSet:4;	// edcf priority enable set
		uint32 reserved0:28;
	} bitFields;
} RegPacTxcEdcfPriorityEnableSet_u;

/*REG_PAC_TXC_EDCF_PRIORITY_ENABLE_CLEAR 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 edcfPriorityEnableClear:4;	// edcf priority enable clear
		uint32 reserved0:28;
	} bitFields;
} RegPacTxcEdcfPriorityEnableClear_u;

/*REG_PAC_TXC_WME_REVISION 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wmeRevisionMinor:4;	// wme revision minor
		uint32 wmeRevisionMajor:4;	// wme revision major
		uint32 reserved0:24;
	} bitFields;
} RegPacTxcWmeRevision_u;

/*REG_PAC_TXC_RXC_DEBUG_OVERRIDE_TXC 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcDebugOverrideTxc:2;	// rxc debug override txc
		uint32 reserved0:30;
	} bitFields;
} RegPacTxcRxcDebugOverrideTxc_u;

/*REG_PAC_TXC_ENABLE_MODIFICATIONS 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ifsCrcErrFixEnable:1;	// IFS CRC error fix enable
		uint32 reserved0:31;
	} bitFields;
} RegPacTxcEnableModifications_u;



#endif // _PAC_TXC_REGS_H_
