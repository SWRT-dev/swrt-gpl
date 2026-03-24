
/***********************************************************************************
File:				MacPhyMonitorRegs.h
Module:				macPhyMonitor
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_PHY_MONITOR_REGS_H_
#define _MAC_PHY_MONITOR_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_PHY_MONITOR_BASE_ADDRESS                             MEMORY_MAP_UNIT_55_BASE_ADDRESS
#define	REG_MAC_PHY_MONITOR_TX_RX_BUFFER_CONTROLS    (MAC_PHY_MONITOR_BASE_ADDRESS + 0x0)
#define	REG_MAC_PHY_MONITOR_CLEAR_CTR                (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4)
#define	REG_MAC_PHY_MONITOR_TCR_CTR                  (MAC_PHY_MONITOR_BASE_ADDRESS + 0x1C)
#define	REG_MAC_PHY_MONITOR_TCR0                     (MAC_PHY_MONITOR_BASE_ADDRESS + 0x20)
#define	REG_MAC_PHY_MONITOR_TCR1                     (MAC_PHY_MONITOR_BASE_ADDRESS + 0x24)
#define	REG_MAC_PHY_MONITOR_TCR2                     (MAC_PHY_MONITOR_BASE_ADDRESS + 0x28)
#define	REG_MAC_PHY_MONITOR_TCR3                     (MAC_PHY_MONITOR_BASE_ADDRESS + 0x2C)
#define	REG_MAC_PHY_MONITOR_TCR4                     (MAC_PHY_MONITOR_BASE_ADDRESS + 0x30)
#define	REG_MAC_PHY_MONITOR_TX_TRACE_ADDR            (MAC_PHY_MONITOR_BASE_ADDRESS + 0x34)
#define	REG_MAC_PHY_MONITOR_TX_TRACE_ADDR_START      (MAC_PHY_MONITOR_BASE_ADDRESS + 0x38)
#define	REG_MAC_PHY_MONITOR_TX_PACKET_CTR            (MAC_PHY_MONITOR_BASE_ADDRESS + 0x3C)
#define	REG_MAC_PHY_MONITOR_STATUS0_LOW              (MAC_PHY_MONITOR_BASE_ADDRESS + 0x40)
#define	REG_MAC_PHY_MONITOR_STATUS0_HIGH             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x44)
#define	REG_MAC_PHY_MONITOR_STATUS1_LOW              (MAC_PHY_MONITOR_BASE_ADDRESS + 0x48)
#define	REG_MAC_PHY_MONITOR_STATUS1_HIGH             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4C)
#define	REG_MAC_PHY_MONITOR_STATUS2_LOW              (MAC_PHY_MONITOR_BASE_ADDRESS + 0x50)
#define	REG_MAC_PHY_MONITOR_STATUS2_HIGH             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x54)
#define	REG_MAC_PHY_MONITOR_RX_TRACE_ADDR            (MAC_PHY_MONITOR_BASE_ADDRESS + 0x58)
#define	REG_MAC_PHY_MONITOR_RX_TRACE_ADDR_START      (MAC_PHY_MONITOR_BASE_ADDRESS + 0x5C)
#define	REG_MAC_PHY_MONITOR_RX_PACKET_CTR            (MAC_PHY_MONITOR_BASE_ADDRESS + 0x60)
#define	REG_MAC_PHY_MONITOR_STATUS_CTR               (MAC_PHY_MONITOR_BASE_ADDRESS + 0x64)
#define	REG_MAC_PHY_MONITOR_TX_MODE0_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x400)
#define	REG_MAC_PHY_MONITOR_TX_MODE1_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x404)
#define	REG_MAC_PHY_MONITOR_TX_MODE2_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x408)
#define	REG_MAC_PHY_MONITOR_TX_MODE3_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x40C)
#define	REG_MAC_PHY_MONITOR_TX_CBW20_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x410)
#define	REG_MAC_PHY_MONITOR_TX_CBW40_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x414)
#define	REG_MAC_PHY_MONITOR_TX_AGG_CTR               (MAC_PHY_MONITOR_BASE_ADDRESS + 0x420)
#define	REG_MAC_PHY_MONITOR_TX_NON_AGG_CTR           (MAC_PHY_MONITOR_BASE_ADDRESS + 0x424)
#define	REG_MAC_PHY_MONITOR_TX_MCS0_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x430)
#define	REG_MAC_PHY_MONITOR_TX_MCS1_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x434)
#define	REG_MAC_PHY_MONITOR_TX_MCS2_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x438)
#define	REG_MAC_PHY_MONITOR_TX_MCS3_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x43C)
#define	REG_MAC_PHY_MONITOR_TX_MCS4_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x440)
#define	REG_MAC_PHY_MONITOR_TX_MCS5_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x444)
#define	REG_MAC_PHY_MONITOR_TX_MCS6_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x448)
#define	REG_MAC_PHY_MONITOR_TX_MCS7_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x44C)
#define	REG_MAC_PHY_MONITOR_TX_MCS1_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x450)
#define	REG_MAC_PHY_MONITOR_TX_MCS2_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x454)
#define	REG_MAC_PHY_MONITOR_TX_MCS3_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x458)
#define	REG_MAC_PHY_MONITOR_TX_MCS4_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x45C)
#define	REG_MAC_PHY_MONITOR_TX_MCS5_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x460)
#define	REG_MAC_PHY_MONITOR_TX_MCS6_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x464)
#define	REG_MAC_PHY_MONITOR_TX_MCS7_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x468)
#define	REG_MAC_PHY_MONITOR_TX_MCS0_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x46C)
#define	REG_MAC_PHY_MONITOR_TX_MCS1_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x470)
#define	REG_MAC_PHY_MONITOR_TX_MCS2_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x474)
#define	REG_MAC_PHY_MONITOR_TX_MCS3_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x478)
#define	REG_MAC_PHY_MONITOR_TX_MCS4_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x47C)
#define	REG_MAC_PHY_MONITOR_TX_MCS5_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x480)
#define	REG_MAC_PHY_MONITOR_TX_MCS6_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x484)
#define	REG_MAC_PHY_MONITOR_TX_MCS7_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x488)
#define	REG_MAC_PHY_MONITOR_TX_MCS8_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x48C)
#define	REG_MAC_PHY_MONITOR_TX_MCS9_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x490)
#define	REG_MAC_PHY_MONITOR_TX_MCS10_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x494)
#define	REG_MAC_PHY_MONITOR_TX_MCS11_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x498)
#define	REG_MAC_PHY_MONITOR_TX_MCS12_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x49C)
#define	REG_MAC_PHY_MONITOR_TX_MCS13_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4A0)
#define	REG_MAC_PHY_MONITOR_TX_MCS14_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4A4)
#define	REG_MAC_PHY_MONITOR_TX_MCS15_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4A8)
#define	REG_MAC_PHY_MONITOR_TX_MCS16_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4AC)
#define	REG_MAC_PHY_MONITOR_TX_MCS17_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4B0)
#define	REG_MAC_PHY_MONITOR_TX_MCS18_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4B4)
#define	REG_MAC_PHY_MONITOR_TX_MCS19_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4B8)
#define	REG_MAC_PHY_MONITOR_TX_MCS20_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4BC)
#define	REG_MAC_PHY_MONITOR_TX_MCS21_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4C0)
#define	REG_MAC_PHY_MONITOR_TX_MCS22_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4C4)
#define	REG_MAC_PHY_MONITOR_TX_MCS23_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4C8)
#define	REG_MAC_PHY_MONITOR_TX_MCS32_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x4CC)
#define	REG_MAC_PHY_MONITOR_RX_MODE0_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x800)
#define	REG_MAC_PHY_MONITOR_RX_MODE1_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x804)
#define	REG_MAC_PHY_MONITOR_RX_MODE2_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x808)
#define	REG_MAC_PHY_MONITOR_RX_MODE3_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x80C)
#define	REG_MAC_PHY_MONITOR_RX_CBW20_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x810)
#define	REG_MAC_PHY_MONITOR_RX_CBW40_CTR             (MAC_PHY_MONITOR_BASE_ADDRESS + 0x814)
#define	REG_MAC_PHY_MONITOR_RX_AGG_CTR               (MAC_PHY_MONITOR_BASE_ADDRESS + 0x820)
#define	REG_MAC_PHY_MONITOR_RX_NON_AGG_CTR           (MAC_PHY_MONITOR_BASE_ADDRESS + 0x824)
#define	REG_MAC_PHY_MONITOR_RX_CRC_ERR_CTR           (MAC_PHY_MONITOR_BASE_ADDRESS + 0x828)
#define	REG_MAC_PHY_MONITOR_RX_CRC_OK_CTR            (MAC_PHY_MONITOR_BASE_ADDRESS + 0x82C)
#define	REG_MAC_PHY_MONITOR_RX_MCS0_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x830)
#define	REG_MAC_PHY_MONITOR_RX_MCS1_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x834)
#define	REG_MAC_PHY_MONITOR_RX_MCS2_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x838)
#define	REG_MAC_PHY_MONITOR_RX_MCS3_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x83C)
#define	REG_MAC_PHY_MONITOR_RX_MCS4_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x840)
#define	REG_MAC_PHY_MONITOR_RX_MCS5_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x844)
#define	REG_MAC_PHY_MONITOR_RX_MCS6_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x848)
#define	REG_MAC_PHY_MONITOR_RX_MCS7_MODE0_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x84C)
#define	REG_MAC_PHY_MONITOR_RX_MCS1_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x850)
#define	REG_MAC_PHY_MONITOR_RX_MCS2_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x854)
#define	REG_MAC_PHY_MONITOR_RX_MCS3_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x858)
#define	REG_MAC_PHY_MONITOR_RX_MCS4_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x85C)
#define	REG_MAC_PHY_MONITOR_RX_MCS5_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x860)
#define	REG_MAC_PHY_MONITOR_RX_MCS6_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x864)
#define	REG_MAC_PHY_MONITOR_RX_MCS7_MODE1_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x868)
#define	REG_MAC_PHY_MONITOR_RX_MCS0_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x86C)
#define	REG_MAC_PHY_MONITOR_RX_MCS1_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x870)
#define	REG_MAC_PHY_MONITOR_RX_MCS2_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x874)
#define	REG_MAC_PHY_MONITOR_RX_MCS3_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x878)
#define	REG_MAC_PHY_MONITOR_RX_MCS4_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x87C)
#define	REG_MAC_PHY_MONITOR_RX_MCS5_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x880)
#define	REG_MAC_PHY_MONITOR_RX_MCS6_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x884)
#define	REG_MAC_PHY_MONITOR_RX_MCS7_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x888)
#define	REG_MAC_PHY_MONITOR_RX_MCS8_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x88C)
#define	REG_MAC_PHY_MONITOR_RX_MCS9_MODE2_CTR        (MAC_PHY_MONITOR_BASE_ADDRESS + 0x890)
#define	REG_MAC_PHY_MONITOR_RX_MCS10_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x894)
#define	REG_MAC_PHY_MONITOR_RX_MCS11_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x898)
#define	REG_MAC_PHY_MONITOR_RX_MCS12_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x89C)
#define	REG_MAC_PHY_MONITOR_RX_MCS13_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8A0)
#define	REG_MAC_PHY_MONITOR_RX_MCS14_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8A4)
#define	REG_MAC_PHY_MONITOR_RX_MCS15_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8A8)
#define	REG_MAC_PHY_MONITOR_RX_MCS16_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8AC)
#define	REG_MAC_PHY_MONITOR_RX_MCS17_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8B0)
#define	REG_MAC_PHY_MONITOR_RX_MCS18_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8B4)
#define	REG_MAC_PHY_MONITOR_RX_MCS19_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8B8)
#define	REG_MAC_PHY_MONITOR_RX_MCS20_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8BC)
#define	REG_MAC_PHY_MONITOR_RX_MCS21_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8C0)
#define	REG_MAC_PHY_MONITOR_RX_MCS22_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8C4)
#define	REG_MAC_PHY_MONITOR_RX_MCS23_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8C8)
#define	REG_MAC_PHY_MONITOR_RX_MCS32_MODE2_CTR       (MAC_PHY_MONITOR_BASE_ADDRESS + 0x8CC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_PHY_MONITOR_TX_RX_BUFFER_CONTROLS 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxTxnBufferSel : 1; //Tx Rx Buffer Sellect, reset value: 0x0, access type: RW
		uint32 resetTxAddressLocation : 1; //Reset Tx Address Location, reset value: 0x0, access type: WO
		uint32 resetRxAddressLocation : 1; //Reset Rx Address Location, reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegMacPhyMonitorTxRxBufferControls_u;

/*REG_MAC_PHY_MONITOR_CLEAR_CTR 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearStat : 1; //Clear Statistics Counters, reset value: 0x0, access type: WO
		uint32 clearPacketCtr : 1; //Clear Packet Counters, reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacPhyMonitorClearCtr_u;

/*REG_MAC_PHY_MONITOR_TCR_CTR 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCtr : 3; //TCR Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegMacPhyMonitorTcrCtr_u;

/*REG_MAC_PHY_MONITOR_TCR0 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr0 : 24; //TCR0, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacPhyMonitorTcr0_u;

/*REG_MAC_PHY_MONITOR_TCR1 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr1 : 24; //TCR1, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacPhyMonitorTcr1_u;

/*REG_MAC_PHY_MONITOR_TCR2 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr2 : 24; //TCR2, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacPhyMonitorTcr2_u;

/*REG_MAC_PHY_MONITOR_TCR3 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr3 : 24; //TCR3, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacPhyMonitorTcr3_u;

/*REG_MAC_PHY_MONITOR_TCR4 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr4 : 24; //TCR4, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacPhyMonitorTcr4_u;

/*REG_MAC_PHY_MONITOR_TX_TRACE_ADDR 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTraceAddr : 13; //Next Tx Write Address, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegMacPhyMonitorTxTraceAddr_u;

/*REG_MAC_PHY_MONITOR_TX_TRACE_ADDR_START 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTraceAddrStart : 13; //Last Tx Start Address, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegMacPhyMonitorTxTraceAddrStart_u;

/*REG_MAC_PHY_MONITOR_TX_PACKET_CTR 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPacketCtr : 32; //Tx Total Packet Counter, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxPacketCtr_u;

/*REG_MAC_PHY_MONITOR_STATUS0_LOW 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 status0Low : 32; //Status0 Low, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorStatus0Low_u;

/*REG_MAC_PHY_MONITOR_STATUS0_HIGH 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 status0High : 32; //Status0 High, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorStatus0High_u;

/*REG_MAC_PHY_MONITOR_STATUS1_LOW 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 status1Low : 32; //Status1 Low, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorStatus1Low_u;

/*REG_MAC_PHY_MONITOR_STATUS1_HIGH 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 status1High : 32; //Status1 High, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorStatus1High_u;

/*REG_MAC_PHY_MONITOR_STATUS2_LOW 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 status2Low : 32; //Status2 Low, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorStatus2Low_u;

/*REG_MAC_PHY_MONITOR_STATUS2_HIGH 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 status2High : 32; //Status2 High, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorStatus2High_u;

/*REG_MAC_PHY_MONITOR_RX_TRACE_ADDR 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxTraceAddr : 13; //Next Rx Write Address, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegMacPhyMonitorRxTraceAddr_u;

/*REG_MAC_PHY_MONITOR_RX_TRACE_ADDR_START 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxTraceAddrStart : 13; //Last Rx Start Address, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegMacPhyMonitorRxTraceAddrStart_u;

/*REG_MAC_PHY_MONITOR_RX_PACKET_CTR 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxPacketCtr : 32; //Rx Total Packet Counter, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxPacketCtr_u;

/*REG_MAC_PHY_MONITOR_STATUS_CTR 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 statusCtr : 2; //Status Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacPhyMonitorStatusCtr_u;

/*REG_MAC_PHY_MONITOR_TX_MODE0_CTR 0x400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMode0Ctr : 32; //Tx Packet Counter Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMode0Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MODE1_CTR 0x404 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMode1Ctr : 32; //Tx Packet Counter Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMode1Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MODE2_CTR 0x408 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMode2Ctr : 32; //Tx Packet Counter Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MODE3_CTR 0x40C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMode3Ctr : 32; //Tx Packet Counter Mode AC, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMode3Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_CBW20_CTR 0x410 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCbw20Ctr : 32; //Tx Packet Counter CBW 20, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxCbw20Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_CBW40_CTR 0x414 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCbw40Ctr : 32; //Tx Packet Counter CBW 40, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxCbw40Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_AGG_CTR 0x420 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txAggCtr : 32; //Tx Packet Counter Aggregate, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxAggCtr_u;

/*REG_MAC_PHY_MONITOR_TX_NON_AGG_CTR 0x424 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNonAggCtr : 32; //Tx Packet Counter Non Aggregate, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxNonAggCtr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS0_MODE0_CTR 0x430 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs0Mode0Ctr : 32; //Tx Packet Counter MCS 0 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs0Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS1_MODE0_CTR 0x434 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs1Mode0Ctr : 32; //Tx Packet Counter MCS 1 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs1Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS2_MODE0_CTR 0x438 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs2Mode0Ctr : 32; //Tx Packet Counter MCS 2 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs2Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS3_MODE0_CTR 0x43C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs3Mode0Ctr : 32; //Tx Packet Counter MCS 3 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs3Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS4_MODE0_CTR 0x440 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs4Mode0Ctr : 32; //Tx Packet Counter MCS 4 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs4Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS5_MODE0_CTR 0x444 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs5Mode0Ctr : 32; //Tx Packet Counter MCS 5 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs5Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS6_MODE0_CTR 0x448 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs6Mode0Ctr : 32; //Tx Packet Counter MCS 6 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs6Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS7_MODE0_CTR 0x44C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs7Mode0Ctr : 32; //Tx Packet Counter MCS 7 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs7Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS1_MODE1_CTR 0x450 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs1Mode1Ctr : 32; //Tx Packet Counter MCS 1 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs1Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS2_MODE1_CTR 0x454 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs2Mode1Ctr : 32; //Tx Packet Counter MCS 2 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs2Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS3_MODE1_CTR 0x458 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs3Mode1Ctr : 32; //Tx Packet Counter MCS 3 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs3Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS4_MODE1_CTR 0x45C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs4Mode1Ctr : 32; //Tx Packet Counter MCS 4 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs4Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS5_MODE1_CTR 0x460 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs5Mode1Ctr : 32; //Tx Packet Counter MCS 5 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs5Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS6_MODE1_CTR 0x464 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs6Mode1Ctr : 32; //Tx Packet Counter MCS 6 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs6Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS7_MODE1_CTR 0x468 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs7Mode1Ctr : 32; //Tx Packet Counter MCS 7 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs7Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS0_MODE2_CTR 0x46C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs0Mode2Ctr : 32; //Tx Packet Counter MCS 0 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs0Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS1_MODE2_CTR 0x470 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs1Mode2Ctr : 32; //Tx Packet Counter MCS 1 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs1Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS2_MODE2_CTR 0x474 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs2Mode2Ctr : 32; //Tx Packet Counter MCS 2 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs2Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS3_MODE2_CTR 0x478 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs3Mode2Ctr : 32; //Tx Packet Counter MCS 3 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs3Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS4_MODE2_CTR 0x47C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs4Mode2Ctr : 32; //Tx Packet Counter MCS 4 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs4Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS5_MODE2_CTR 0x480 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs5Mode2Ctr : 32; //Tx Packet Counter MCS 5 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs5Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS6_MODE2_CTR 0x484 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs6Mode2Ctr : 32; //Tx Packet Counter MCS 6 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs6Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS7_MODE2_CTR 0x488 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs7Mode2Ctr : 32; //Tx Packet Counter MCS 7 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs7Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS8_MODE2_CTR 0x48C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs8Mode2Ctr : 32; //Tx Packet Counter MCS 8 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs8Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS9_MODE2_CTR 0x490 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs9Mode2Ctr : 32; //Tx Packet Counter MCS 9 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs9Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS10_MODE2_CTR 0x494 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs10Mode2Ctr : 32; //Tx Packet Counter MCS 10 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs10Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS11_MODE2_CTR 0x498 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs11Mode2Ctr : 32; //Tx Packet Counter MCS 11 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs11Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS12_MODE2_CTR 0x49C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs12Mode2Ctr : 32; //Tx Packet Counter MCS 12 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs12Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS13_MODE2_CTR 0x4A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs13Mode2Ctr : 32; //Tx Packet Counter MCS 13 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs13Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS14_MODE2_CTR 0x4A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs14Mode2Ctr : 32; //Tx Packet Counter MCS 14 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs14Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS15_MODE2_CTR 0x4A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs15Mode2Ctr : 32; //Tx Packet Counter MCS 15 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs15Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS16_MODE2_CTR 0x4AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs16Mode2Ctr : 32; //Tx Packet Counter MCS 16 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs16Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS17_MODE2_CTR 0x4B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs17Mode2Ctr : 32; //Tx Packet Counter MCS 17 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs17Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS18_MODE2_CTR 0x4B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs18Mode2Ctr : 32; //Tx Packet Counter MCS 18 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs18Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS19_MODE2_CTR 0x4B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs19Mode2Ctr : 32; //Tx Packet Counter MCS 19 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs19Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS20_MODE2_CTR 0x4BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs20Mode2Ctr : 32; //Tx Packet Counter MCS 20 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs20Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS21_MODE2_CTR 0x4C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs21Mode2Ctr : 32; //Tx Packet Counter MCS 21 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs21Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS22_MODE2_CTR 0x4C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs22Mode2Ctr : 32; //Tx Packet Counter MCS 22 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs22Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS23_MODE2_CTR 0x4C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs23Mode2Ctr : 32; //Tx Packet Counter MCS 23 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs23Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_TX_MCS32_MODE2_CTR 0x4CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMcs32Mode2Ctr : 32; //Tx Packet Counter MCS 32 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorTxMcs32Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MODE0_CTR 0x800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMode0Ctr : 32; //Rx Packet Counter Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMode0Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MODE1_CTR 0x804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMode1Ctr : 32; //Rx Packet Counter Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMode1Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MODE2_CTR 0x808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMode2Ctr : 32; //Rx Packet Counter Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MODE3_CTR 0x80C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMode3Ctr : 32; //Rx Packet Counter Mode AC, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMode3Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_CBW20_CTR 0x810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxCbw20Ctr : 32; //Rx Packet Counter CBW 20, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxCbw20Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_CBW40_CTR 0x814 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxCbw40Ctr : 32; //Rx Packet Counter CBW 40, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxCbw40Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_AGG_CTR 0x820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxAggCtr : 32; //Rx Packet Counter Aggregate, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxAggCtr_u;

/*REG_MAC_PHY_MONITOR_RX_NON_AGG_CTR 0x824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxNonAggCtr : 32; //Rx Packet Counter Non Aggregate, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxNonAggCtr_u;

/*REG_MAC_PHY_MONITOR_RX_CRC_ERR_CTR 0x828 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxCrcErrCtr : 32; //Rx Packet Counter CRC Error, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxCrcErrCtr_u;

/*REG_MAC_PHY_MONITOR_RX_CRC_OK_CTR 0x82C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxCrcOkCtr : 32; //Rx Packet Counter CRC OK, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxCrcOkCtr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS0_MODE0_CTR 0x830 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs0Mode0Ctr : 32; //Rx Packet Counter MCS 0 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs0Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS1_MODE0_CTR 0x834 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs1Mode0Ctr : 32; //Rx Packet Counter MCS 1 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs1Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS2_MODE0_CTR 0x838 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs2Mode0Ctr : 32; //Rx Packet Counter MCS 2 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs2Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS3_MODE0_CTR 0x83C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs3Mode0Ctr : 32; //Rx Packet Counter MCS 3 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs3Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS4_MODE0_CTR 0x840 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs4Mode0Ctr : 32; //Rx Packet Counter MCS 4 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs4Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS5_MODE0_CTR 0x844 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs5Mode0Ctr : 32; //Rx Packet Counter MCS 5 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs5Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS6_MODE0_CTR 0x848 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs6Mode0Ctr : 32; //Rx Packet Counter MCS 6 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs6Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS7_MODE0_CTR 0x84C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs7Mode0Ctr : 32; //Rx Packet Counter MCS 7 Mode A/G, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs7Mode0Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS1_MODE1_CTR 0x850 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs1Mode1Ctr : 32; //Rx Packet Counter MCS 1 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs1Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS2_MODE1_CTR 0x854 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs2Mode1Ctr : 32; //Rx Packet Counter MCS 2 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs2Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS3_MODE1_CTR 0x858 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs3Mode1Ctr : 32; //Rx Packet Counter MCS 3 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs3Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS4_MODE1_CTR 0x85C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs4Mode1Ctr : 32; //Rx Packet Counter MCS 4 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs4Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS5_MODE1_CTR 0x860 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs5Mode1Ctr : 32; //Rx Packet Counter MCS 5 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs5Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS6_MODE1_CTR 0x864 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs6Mode1Ctr : 32; //Rx Packet Counter MCS 6 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs6Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS7_MODE1_CTR 0x868 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs7Mode1Ctr : 32; //Rx Packet Counter MCS 7 Mode B, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs7Mode1Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS0_MODE2_CTR 0x86C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs0Mode2Ctr : 32; //Rx Packet Counter MCS 0 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs0Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS1_MODE2_CTR 0x870 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs1Mode2Ctr : 32; //Rx Packet Counter MCS 1 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs1Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS2_MODE2_CTR 0x874 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs2Mode2Ctr : 32; //Rx Packet Counter MCS 2 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs2Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS3_MODE2_CTR 0x878 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs3Mode2Ctr : 32; //Rx Packet Counter MCS 3 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs3Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS4_MODE2_CTR 0x87C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs4Mode2Ctr : 32; //Rx Packet Counter MCS 4 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs4Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS5_MODE2_CTR 0x880 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs5Mode2Ctr : 32; //Rx Packet Counter MCS 5 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs5Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS6_MODE2_CTR 0x884 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs6Mode2Ctr : 32; //Rx Packet Counter MCS 6 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs6Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS7_MODE2_CTR 0x888 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs7Mode2Ctr : 32; //Rx Packet Counter MCS 7 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs7Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS8_MODE2_CTR 0x88C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs8Mode2Ctr : 32; //Rx Packet Counter MCS 8 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs8Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS9_MODE2_CTR 0x890 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs9Mode2Ctr : 32; //Rx Packet Counter MCS 9 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs9Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS10_MODE2_CTR 0x894 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs10Mode2Ctr : 32; //Rx Packet Counter MCS 10 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs10Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS11_MODE2_CTR 0x898 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs11Mode2Ctr : 32; //Rx Packet Counter MCS 11 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs11Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS12_MODE2_CTR 0x89C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs12Mode2Ctr : 32; //Rx Packet Counter MCS 12 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs12Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS13_MODE2_CTR 0x8A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs13Mode2Ctr : 32; //Rx Packet Counter MCS 13 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs13Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS14_MODE2_CTR 0x8A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs14Mode2Ctr : 32; //Rx Packet Counter MCS 14 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs14Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS15_MODE2_CTR 0x8A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs15Mode2Ctr : 32; //Rx Packet Counter MCS 15 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs15Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS16_MODE2_CTR 0x8AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs16Mode2Ctr : 32; //Rx Packet Counter MCS 16 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs16Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS17_MODE2_CTR 0x8B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs17Mode2Ctr : 32; //Rx Packet Counter MCS 17 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs17Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS18_MODE2_CTR 0x8B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs18Mode2Ctr : 32; //Rx Packet Counter MCS 18 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs18Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS19_MODE2_CTR 0x8B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs19Mode2Ctr : 32; //Rx Packet Counter MCS 19 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs19Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS20_MODE2_CTR 0x8BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs20Mode2Ctr : 32; //Rx Packet Counter MCS 20 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs20Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS21_MODE2_CTR 0x8C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs21Mode2Ctr : 32; //Rx Packet Counter MCS 21 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs21Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS22_MODE2_CTR 0x8C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs22Mode2Ctr : 32; //Rx Packet Counter MCS 22 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs22Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS23_MODE2_CTR 0x8C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs23Mode2Ctr : 32; //Rx Packet Counter MCS 23 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs23Mode2Ctr_u;

/*REG_MAC_PHY_MONITOR_RX_MCS32_MODE2_CTR 0x8CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMcs32Mode2Ctr : 32; //Rx Packet Counter MCS 32 Mode N, reset value: 0x0, access type: RO
	} bitFields;
} RegMacPhyMonitorRxMcs32Mode2Ctr_u;



#endif // _MAC_PHY_MONITOR_REGS_H_
