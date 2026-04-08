
/***********************************************************************************
File:				PacRxcRegs.h
Module:				pacRxc
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PAC_RXC_REGS_H_
#define _PAC_RXC_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PAC_RXC_BASE_ADDRESS                             MEMORY_MAP_UNIT_9_BASE_ADDRESS
#define	REG_PAC_RXC_RX_CONTROL                          (PAC_RXC_BASE_ADDRESS + 0x0)
#define	REG_PAC_RXC_RX_BUFFER_IN_POINTER_WR             (PAC_RXC_BASE_ADDRESS + 0x4)
#define	REG_PAC_RXC_RX_BUFFER_OUT_POINTER_WR            (PAC_RXC_BASE_ADDRESS + 0x8)
#define	REG_PAC_RXC_NEAR_FULL_THRESHOLD                 (PAC_RXC_BASE_ADDRESS + 0xC)
#define	REG_PAC_RXC_CCA_IFS_COMPENSATION                (PAC_RXC_BASE_ADDRESS + 0x14)
#define	REG_PAC_RXC_RXRDY_IFS_COMPENSATION              (PAC_RXC_BASE_ADDRESS + 0x18)
#define	REG_PAC_RXC_RXC_STATE                           (PAC_RXC_BASE_ADDRESS + 0x1C)
#define	REG_PAC_RXC_MT_FORCE_CCA                        (PAC_RXC_BASE_ADDRESS + 0x24)
#define	REG_PAC_RXC_RCC_CRC_ADDR                        (PAC_RXC_BASE_ADDRESS + 0x28)
#define	REG_PAC_RXC_RX_BUFFER_IN_POINTER_RD             (PAC_RXC_BASE_ADDRESS + 0x2C)
#define	REG_PAC_RXC_RX_BUFFER_OUT_POINTER_RD            (PAC_RXC_BASE_ADDRESS + 0x30)
#define	REG_PAC_RXC_MINIMAL_RSSI_THRESHOLD              (PAC_RXC_BASE_ADDRESS + 0x34)
#define	REG_PAC_RXC_TRAFFIC_INDICATOR_EN                (PAC_RXC_BASE_ADDRESS + 0x38)
#define	REG_PAC_RXC_TRAFFIC_BA_RECIPIENT_RXD_LINES_A    (PAC_RXC_BASE_ADDRESS + 0x3C)
#define	REG_PAC_RXC_TRAFFIC_BA_RECIPIENT_RXD_LINES_B    (PAC_RXC_BASE_ADDRESS + 0x40)
#define	REG_PAC_RXC_CRC_CALC_INIT                       (PAC_RXC_BASE_ADDRESS + 0x98)
#define	REG_PAC_RXC_CRC_CALC_CTL                        (PAC_RXC_BASE_ADDRESS + 0x9C)
#define	REG_PAC_RXC_CRC_CALC_DATA                       (PAC_RXC_BASE_ADDRESS + 0xA0)
#define	REG_PAC_RXC_CRC_CALC_STATUS                     (PAC_RXC_BASE_ADDRESS + 0xA4)
#define	REG_PAC_RXC_CRC_CALC_RESULT_ADDR                (PAC_RXC_BASE_ADDRESS + 0xA8)
#define	REG_PAC_RXC_BUFFER_OVERFLOW_CTR                 (PAC_RXC_BASE_ADDRESS + 0xAC)
#define	REG_PAC_RXC_BUFFER_OVERFLOW_CTR_CLEAR           (PAC_RXC_BASE_ADDRESS + 0xB0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PAC_RXC_RX_CONTROL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcEnable:1;	// RXC  rx control
		uint32 earlyEnableDeassert:1;	// RXC  early enable deassert
		uint32 rxBufOutPtrRxDmaIncEn:1;	// Enable update RXC buffer out based on Rx DMA monitor address
		uint32 phyRxAbortRssiEn:1;	// Enable abort PHY-MAC Rx session based on RSSI threshold
		uint32 reserved0:4;
		uint32 reservedStorageSize:6;	// RXC  reserved storage size
		uint32 reserved1:1;
		uint32 rxcKeepBadFrames:1;	// RXC  keep bad frames
		uint32 rxcAutoDiscard:1;	// RXC  auto discard
		uint32 rxcAutoKeep:1;	// RXC  auto keep
		uint32 cca:1;	// RXC  cca
		uint32 reserved2:1;
		uint32 rxBufferNearFull:1;	// RXC  rx buffer near full
		uint32 rxBufferOverflowWpulse:1;	// RXC  rx buffer overflow
		uint32 rxFifoOverflowWpulse:1;	// RXC  rx fifo overflow
		uint32 receiveBusy:1;	// RXC  receive busy
		uint32 reserved3:2;
		uint32 rxBufferOverflow:1;	// RXC  rx buffer overflow RD
		uint32 rxFifoOverflow:1;	// RXC  rx fifo overflow RD
		uint32 phyRxAbortRssiStatus:1;	// Abort PHY-MAC Rx session based on RSSI threshold indication from RXC to Delineator
		uint32 reserved4:3;
	} bitFields;
} RegPacRxcRxControl_u;

/*REG_PAC_RXC_RX_BUFFER_IN_POINTER_WR 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 rxBufferInPointerWr:20;	// RXC rx buffer in pointer WR
		uint32 reserved1:10;
	} bitFields;
} RegPacRxcRxBufferInPointerWr_u;

/*REG_PAC_RXC_RX_BUFFER_OUT_POINTER_WR 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 rxBufferOutPointerWr:20;	// RXC rx buffer out pointer WR
		uint32 reserved1:10;
	} bitFields;
} RegPacRxcRxBufferOutPointerWr_u;

/*REG_PAC_RXC_NEAR_FULL_THRESHOLD 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 nearFullThreshold:7;	// RXC  near full threshold
		uint32 reserved1:23;
	} bitFields;
} RegPacRxcNearFullThreshold_u;

/*REG_PAC_RXC_CCA_IFS_COMPENSATION 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaIfsCompensation:16;	// RXC  cca ifs compensation
		uint32 reducedFreqCcaIfsCompensation:16;	// RXC  cca ifs compensation
	} bitFields;
} RegPacRxcCcaIfsCompensation_u;

/*REG_PAC_RXC_RXRDY_IFS_COMPENSATION 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxrdyIfsCompensation:16;	// RXC  rxrdy ifs compensation
		uint32 reducedFreqRxrdyIfsCompensation:16;	// RXC  rxrdy ifs compensation
	} bitFields;
} RegPacRxcRxrdyIfsCompensation_u;

/*REG_PAC_RXC_RXC_STATE 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcState:4;	// RxC state
		uint32 reserved0:4;
		uint32 rxcRdInfoSm:3;	// RxC Read additional info state machine
		uint32 reserved1:21;
	} bitFields;
} RegPacRxcRxcState_u;

/*REG_PAC_RXC_MT_FORCE_CCA 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 mtForceSwCcaPrimary:1;	// MT Force SW CCA primary
		uint32 mtForcedSwCcaPrimary:1;	// MT SW Forced CCA primary value
		uint32 reserved1:5;
		uint32 forceCca20PFree:1;	// Force CCA 20MHz primary to be free
		uint32 forceCca20SFree:1;	// Force CCA 20MHz secondary to be free
		uint32 forceCca40SFree:1;	// Force CCA 40MHz secondary to be free
		uint32 reserved2:1;
		uint32 forceCca20PPwrFree:1;	// Force CCA 20MHz primary power to be free
		uint32 forceCca20SPwrFree:1;	// Force CCA 20MHz secondary power to be free
		uint32 forceCca40SPwrFree:1;	// Force CCA 40MHz secondary power to be free
		uint32 reserved3:1;
		uint32 cca20PAdaptMode:2;	// CCA 20MHz primary is adapted according to the following: , 00: Only 20MHz primary CCA signal. , 01: AND between 20MHz primary and 20MHz secondary CCA signals. , 10: AND between 20MHz primary, 20MHz secondary and 40MHz secondary CCA signals.
		uint32 reserved4:14;
	} bitFields;
} RegPacRxcMtForceCca_u;

/*REG_PAC_RXC_RCC_CRC_ADDR 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcCrc:32;	// RXC crc
	} bitFields;
} RegPacRxcRccCrcAddr_u;

/*REG_PAC_RXC_RX_BUFFER_IN_POINTER_RD 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 rxBufferInPointerRd:20;	// RXC rx buffer in pointer RD
		uint32 reserved1:10;
	} bitFields;
} RegPacRxcRxBufferInPointerRd_u;

/*REG_PAC_RXC_RX_BUFFER_OUT_POINTER_RD 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 rxBufferOutPointerRd:20;	// RXC rx buffer out pointer RD
		uint32 reserved1:10;
	} bitFields;
} RegPacRxcRxBufferOutPointerRd_u;

/*REG_PAC_RXC_MINIMAL_RSSI_THRESHOLD 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minimalRssiThreshold:8;	// Minimal RSSI threshold. , Signed value. , Used for setting PHY abort and NAV update.
		uint32 reserved0:24;
	} bitFields;
} RegPacRxcMinimalRssiThreshold_u;

/*REG_PAC_RXC_TRAFFIC_INDICATOR_EN 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 trafficStaActivityEn:1;	// Enable setting STA activity BitMAP indications
		uint32 trafficBaAgreementRecipientEn:1;	// Enable setting BA agreement recipient BitMAP indications
		uint32 trafficBaAgreementOriginatorEn:1;	// Enable setting BA agreement originator BitMAP indications
		uint32 ignoreMultProbeReqTrafficStaActivityEn:1;	// Enable ignore Multicat Probe request frame from setting STA activity BitMAP indications
		uint32 reserved0:28;
	} bitFields;
} RegPacRxcTrafficIndicatorEn_u;

/*REG_PAC_RXC_TRAFFIC_BA_RECIPIENT_RXD_LINES_A 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 trafficBaRecipientRxdLinesA:32;	// Each bit per RXD line (lines 0to31), , Indicates to update BA Agreement recipient bitmap due to BA transmission
	} bitFields;
} RegPacRxcTrafficBaRecipientRxdLinesA_u;

/*REG_PAC_RXC_TRAFFIC_BA_RECIPIENT_RXD_LINES_B 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 trafficBaRecipientRxdLinesB:2;	// Each bit per RXD line (lines 32to33), , Indicates to update BA Agreement recipient bitmap due to BA transmission
		uint32 reserved0:30;
	} bitFields;
} RegPacRxcTrafficBaRecipientRxdLinesB_u;

/*REG_PAC_RXC_CRC_CALC_INIT 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcCalcInit:1;
		uint32 reserved0:31;
	} bitFields;
} RegPacRxcCrcCalcInit_u;

/*REG_PAC_RXC_CRC_CALC_CTL 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcByteEnable:4;	// CRC byte enables
		uint32 reserved0:28;
	} bitFields;
} RegPacRxcCrcCalcCtl_u;

/*REG_PAC_RXC_CRC_CALC_DATA 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcDataInOut:32;	// CRC read/write data
	} bitFields;
} RegPacRxcCrcCalcData_u;

/*REG_PAC_RXC_CRC_CALC_STATUS 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcCalcError:1;	// CRC error
		uint32 reserved0:31;
	} bitFields;
} RegPacRxcCrcCalcStatus_u;

/*REG_PAC_RXC_CRC_CALC_RESULT_ADDR 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcCalcResult:32;	// CRC result
	} bitFields;
} RegPacRxcCrcCalcResultAddr_u;

/*REG_PAC_RXC_BUFFER_OVERFLOW_CTR 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bufferOverflowCtr:16;	// Buffer overflow counter
		uint32 reserved0:16;
	} bitFields;
} RegPacRxcBufferOverflowCtr_u;

/*REG_PAC_RXC_BUFFER_OVERFLOW_CTR_CLEAR 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bufferOverflowCtrClear:1;	// Buffer overflow counter clear, by write '1'
		uint32 reserved0:31;
	} bitFields;
} RegPacRxcBufferOverflowCtrClear_u;



#endif // _PAC_RXC_REGS_H_
