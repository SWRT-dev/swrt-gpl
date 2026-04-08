
/***********************************************************************************
File:				MacAddr2IndexRegs.h
Module:				macAddr2Index
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_ADDR2INDEX_REGS_H_
#define _MAC_ADDR2INDEX_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_ADDR2INDEX_BASE_ADDRESS                             MEMORY_MAP_UNIT_22_BASE_ADDRESS
#define	REG_MAC_ADDR2INDEX_QUEUE_STA_DB_BASE_ADDR              (MAC_ADDR2INDEX_BASE_ADDRESS + 0x0)
#define	REG_MAC_ADDR2INDEX_QUEUE_VAP_DB_BASE_ADDR              (MAC_ADDR2INDEX_BASE_ADDRESS + 0x4)
#define	REG_MAC_ADDR2INDEX_QUEUE_GLOBAL_DB_BASE_ADDR           (MAC_ADDR2INDEX_BASE_ADDRESS + 0x8)
#define	REG_MAC_ADDR2INDEX_QUEUE_GPLP_DB_BASE_ADDR             (MAC_ADDR2INDEX_BASE_ADDRESS + 0xC)
#define	REG_MAC_ADDR2INDEX_QUEUE_GPHP_DB_BASE_ADDR             (MAC_ADDR2INDEX_BASE_ADDRESS + 0x10)
#define	REG_MAC_ADDR2INDEX_QUEUE_BEACON_DB_BASE_ADDR           (MAC_ADDR2INDEX_BASE_ADDRESS + 0x14)
#define	REG_MAC_ADDR2INDEX_QUEUE_STA_DB_SIZE                   (MAC_ADDR2INDEX_BASE_ADDRESS + 0x18)
#define	REG_MAC_ADDR2INDEX_QUEUE_VAP_DB_SIZE                   (MAC_ADDR2INDEX_BASE_ADDRESS + 0x20)
#define	REG_MAC_ADDR2INDEX_QUEUE_GLOBAL_DB_SIZE                (MAC_ADDR2INDEX_BASE_ADDRESS + 0x24)
#define	REG_MAC_ADDR2INDEX_QUEUE_GPLP_DB_SIZE                  (MAC_ADDR2INDEX_BASE_ADDRESS + 0x28)
#define	REG_MAC_ADDR2INDEX_QUEUE_GPHP_DB_SIZE                  (MAC_ADDR2INDEX_BASE_ADDRESS + 0x2C)
#define	REG_MAC_ADDR2INDEX_QUEUE_BEACON_DB_SIZE                (MAC_ADDR2INDEX_BASE_ADDRESS + 0x30)
#define	REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_COMMAND              (MAC_ADDR2INDEX_BASE_ADDRESS + 0x34)
#define	REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_ADDR_0TO31           (MAC_ADDR2INDEX_BASE_ADDRESS + 0x38)
#define	REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_ADDR_32TO47          (MAC_ADDR2INDEX_BASE_ADDRESS + 0x3C)
#define	REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_STATUS               (MAC_ADDR2INDEX_BASE_ADDRESS + 0x40)
#define	REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_VALID_STA_0TO31      (MAC_ADDR2INDEX_BASE_ADDRESS + 0x44)
#define	REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_VALID_STA_32TO63     (MAC_ADDR2INDEX_BASE_ADDRESS + 0x48)
#define	REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_VALID_STA_64TO95     (MAC_ADDR2INDEX_BASE_ADDRESS + 0x4C)
#define	REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_VALID_STA_96TO127    (MAC_ADDR2INDEX_BASE_ADDRESS + 0x50)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_ADDR2INDEX_QUEUE_STA_DB_BASE_ADDR 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueStaDbBaseAddr:24;	// Queue STA DB base addr
		uint32 reserved0:8;
	} bitFields;
} RegMacAddr2IndexQueueStaDbBaseAddr_u;

/*REG_MAC_ADDR2INDEX_QUEUE_VAP_DB_BASE_ADDR 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueVapDbBaseAddr:24;	// Queue VAP DB base addr
		uint32 reserved0:8;
	} bitFields;
} RegMacAddr2IndexQueueVapDbBaseAddr_u;

/*REG_MAC_ADDR2INDEX_QUEUE_GLOBAL_DB_BASE_ADDR 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueGlobalDbBaseAddr:24;	// Queue Global DB base addr
		uint32 reserved0:8;
	} bitFields;
} RegMacAddr2IndexQueueGlobalDbBaseAddr_u;

/*REG_MAC_ADDR2INDEX_QUEUE_GPLP_DB_BASE_ADDR 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueGplpDbBaseAddr:24;	// Queue GPLP DB base addr
		uint32 reserved0:8;
	} bitFields;
} RegMacAddr2IndexQueueGplpDbBaseAddr_u;

/*REG_MAC_ADDR2INDEX_QUEUE_GPHP_DB_BASE_ADDR 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueGphpDbBaseAddr:24;	// Queue GPHP DB base addr
		uint32 reserved0:8;
	} bitFields;
} RegMacAddr2IndexQueueGphpDbBaseAddr_u;

/*REG_MAC_ADDR2INDEX_QUEUE_BEACON_DB_BASE_ADDR 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueBeaconDbBaseAddr:24;	// Queue Beacon DB base addr
		uint32 reserved0:8;
	} bitFields;
} RegMacAddr2IndexQueueBeaconDbBaseAddr_u;

/*REG_MAC_ADDR2INDEX_QUEUE_STA_DB_SIZE 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueStaDbEntry4BSize:8;	// Queue STA DB entry 4Bytes size
		uint32 queueStaSecondaryDbEntry4BOffset:8;	// Queue STA secondary DB entry 4Bytes offset
		uint32 queueStaSecondaryDbEntry4BSize:6;	// Queue STA secondary DB entry 4Bytes size
		uint32 reserved0:10;
	} bitFields;
} RegMacAddr2IndexQueueStaDbSize_u;

/*REG_MAC_ADDR2INDEX_QUEUE_VAP_DB_SIZE 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueVapDbEntry4BSize:8;	// Queue VAP DB entry 4Bytes size
		uint32 queueVapSecondaryDbEntry4BOffset:8;	// Queue VAP secondary DB entry 4Bytes offset
		uint32 queueVapSecondaryDbEntry4BSize:6;	// Queue VAP secondary DB entry 4Bytes size
		uint32 reserved0:10;
	} bitFields;
} RegMacAddr2IndexQueueVapDbSize_u;

/*REG_MAC_ADDR2INDEX_QUEUE_GLOBAL_DB_SIZE 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueGlobalDbEntry4BSize:8;	// Queue Global DB entry 4Bytes size
		uint32 queueGlobalSecondaryDbEntry4BOffset:8;	// Queue Global secondary DB entry 4Bytes offset
		uint32 queueGlobalSecondaryDbEntry4BSize:6;	// Queue Global secondary DB entry 4Bytes size
		uint32 reserved0:10;
	} bitFields;
} RegMacAddr2IndexQueueGlobalDbSize_u;

/*REG_MAC_ADDR2INDEX_QUEUE_GPLP_DB_SIZE 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueGplpDbEntry4BSize:8;	// Queue GPLP DB entry 4Bytes size
		uint32 queueGplpSecondaryDbEntry4BOffset:8;	// Queue GPLP secondary DB entry 4Bytes offset
		uint32 reserved0:16;
	} bitFields;
} RegMacAddr2IndexQueueGplpDbSize_u;

/*REG_MAC_ADDR2INDEX_QUEUE_GPHP_DB_SIZE 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueGphpDbEntry4BSize:8;	// Queue GPHP DB entry 4Bytes size
		uint32 queueGphpSecondaryDbEntry4BOffset:8;	// Queue GPHP secondary DB entry 4Bytes offset
		uint32 reserved0:16;
	} bitFields;
} RegMacAddr2IndexQueueGphpDbSize_u;

/*REG_MAC_ADDR2INDEX_QUEUE_BEACON_DB_SIZE 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 queueBeaconDbEntry4BSize:8;	// Queue Beacon DB entry 4Bytes size
		uint32 queueBeaconSecondaryDbEntry4BOffset:8;	// Queue Beacon secondary DB entry 4Bytes offset
		uint32 reserved0:16;
	} bitFields;
} RegMacAddr2IndexQueueBeaconDbSize_u;

/*REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_COMMAND 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAddr2IndexCommand:3;	// MAC addr2index command
		uint32 reserved0:29;
	} bitFields;
} RegMacAddr2IndexMacAddr2IndexCommand_u;

/*REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_ADDR_0TO31 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAddr2IndexAddr0To31:32;	// MAC addr2index addr 0to31
	} bitFields;
} RegMacAddr2IndexMacAddr2IndexAddr0To31_u;

/*REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_ADDR_32TO47 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAddr2IndexAddr32To47:16;	// MAC addr2index addr 32to47
		uint32 removeDbIndex:7;	// MAC addr2index remove DB index
		uint32 reserved0:9;
	} bitFields;
} RegMacAddr2IndexMacAddr2IndexAddr32To47_u;

/*REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_STATUS 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAddr2IndexDone:1;	// MAC addr2index done
		uint32 macAddr2IndexFail:3;	// MAC addr2index fail status
		uint32 reserved0:4;
		uint32 macAddr2IndexDbIndex:7;	// MAC addr2index DB index
		uint32 reserved1:1;
		uint32 macAddr2IndexNumValidIndexes:8;	// MAC addr2index num valid indexes
		uint32 macAddr2IndexSm:3;	// MAC addr2index state machine
		uint32 macAddr2IndexArbSm:2;	// MAC addr2index arbiter state machine
		uint32 reserved2:3;
	} bitFields;
} RegMacAddr2IndexMacAddr2IndexStatus_u;

/*REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_VALID_STA_0TO31 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAddr2IndexValidSta0To31:32;	// MAC addr2index valid STAs 0to31
	} bitFields;
} RegMacAddr2IndexMacAddr2IndexValidSta0To31_u;

/*REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_VALID_STA_32TO63 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAddr2IndexValidSta32To63:32;	// MAC addr2index valid STAs 32to63
	} bitFields;
} RegMacAddr2IndexMacAddr2IndexValidSta32To63_u;

/*REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_VALID_STA_64TO95 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAddr2IndexValidSta64To95:32;	// MAC addr2index valid STAs 64to95
	} bitFields;
} RegMacAddr2IndexMacAddr2IndexValidSta64To95_u;

/*REG_MAC_ADDR2INDEX_MAC_ADDR2INDEX_VALID_STA_96TO127 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAddr2IndexValidSta96To127:32;	// MAC addr2index valid STAs 96to127
	} bitFields;
} RegMacAddr2IndexMacAddr2IndexValidSta96To127_u;



#endif // _MAC_ADDR2INDEX_REGS_H_
