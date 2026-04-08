
/***********************************************************************************
File:				Modem11BRegs.h
Module:				modem11B
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MODEM_11B_REGS_H_
#define _MODEM_11B_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MODEM_11B_BASE_ADDRESS                             MEMORY_MAP_UNIT_52_BASE_ADDRESS
#define	REG_MODEM_11B_MDMBCNTL       (MODEM_11B_BASE_ADDRESS + 0xC000)
#define	REG_MODEM_11B_MDMBPRMINIT    (MODEM_11B_BASE_ADDRESS + 0xC020)
#define	REG_MODEM_11B_MDMBTALPHA     (MODEM_11B_BASE_ADDRESS + 0xC030)
#define	REG_MODEM_11B_MDMBTBETA      (MODEM_11B_BASE_ADDRESS + 0xC040)
#define	REG_MODEM_11B_MDMBTMU        (MODEM_11B_BASE_ADDRESS + 0xC050)
#define	REG_MODEM_11B_MDMBCNTL1      (MODEM_11B_BASE_ADDRESS + 0xC060)
#define	REG_MODEM_11B_MDMBRFCNTL     (MODEM_11B_BASE_ADDRESS + 0xC070)
#define	REG_MODEM_11B_MDMBCCA        (MODEM_11B_BASE_ADDRESS + 0xC080)
#define	REG_MODEM_11B_MDMBEQCNTL     (MODEM_11B_BASE_ADDRESS + 0xC090)
#define	REG_MODEM_11B_MDMBCNTL2      (MODEM_11B_BASE_ADDRESS + 0xC0A0)
#define	REG_MODEM_11B_MDMBSTAT0      (MODEM_11B_BASE_ADDRESS + 0xC0B0)
#define	REG_MODEM_11B_MDMBSTAT1      (MODEM_11B_BASE_ADDRESS + 0xC0C0)
#define	REG_MODEM_11B_MDMBVERSION    (MODEM_11B_BASE_ADDRESS + 0xC0D0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MODEM_11B_MDMBCNTL 0xC000 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intPrepre:6;	// pre-preamble cycles
		uint32 reserved0:2;
		uint32 intSfdlen:3;	// short SFD preamble bits
		uint32 reserved1:1;
		uint32 intSfderr:3;	// SFD error num
		uint32 reserved2:5;
		uint32 intScrambdisb:1;	// Scrambeling disable
		uint32 intSpreaddisb:1;	// Spreading disable
		uint32 intFirdisb:1;	// Tx and Rx filters disable
		uint32 intEqdisb:1;	// Equalizer bypass
		uint32 intCompdisb:1;	// Frequency offset compensation bypass
		uint32 intDcoffdisb:1;	// DC Offset compensation bypass
		uint32 intPrecompdisb:1;	// Frequency pre-compensation bypass
		uint32 intGaindisb:1;	// Gain compensation bypass
		uint32 intIqmmdisb:1;	// I/Q Mismatch compensation bypass
		uint32 intInterpdisb:1;	// Timing interpolator bypass
		uint32 intRxc2Disb:1;	// RX 2’s complement conversion bypass
		uint32 intTlockdisb:1;	// Timing lock mode
	} bitFields;
} RegModem11BMdmbcntl_u;

/*REG_MODEM_11B_MDMBPRMINIT 0xC020 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intAlpha:2;	// Equalizer initial ? parameter
		uint32 reserved0:2;
		uint32 intBeta:2;	// Equalizer initial  ? parameter
		uint32 reserved1:10;
		uint32 intMu:2;	// Phase and Carrier  ? parameter
		uint32 reserved2:2;
		uint32 intRho:2;	// Phase and Carrier ? parameter
		uint32 reserved3:10;
	} bitFields;
} RegModem11BMdmbprminit_u;

/*REG_MODEM_11B_MDMBTALPHA 0xC030 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intTalpha0:4;	// Equalizer ? parameter update interva ??0
		uint32 intTalpha1:4;	// Equalizer ? parameter update interva ??1
		uint32 intTalpha2:4;	// Equalizer ? parameter update interva ??2
		uint32 intTalpha3:4;	// Equalizer ? parameter update interva ??2
		uint32 reserved0:16;
	} bitFields;
} RegModem11BMdmbtalpha_u;

/*REG_MODEM_11B_MDMBTBETA 0xC040 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intTbeta0:4;	// Equalizer ? parameter update interva ??0
		uint32 intTbeta1:4;	// Equalizer ? parameter update interva ??1
		uint32 intTbeta2:4;	// Equalizer ? parameter update interva ??2
		uint32 intTbeta3:4;	// Equalizer ? parameter update interva ??3
		uint32 reserved0:16;
	} bitFields;
} RegModem11BMdmbtbeta_u;

/*REG_MODEM_11B_MDMBTMU 0xC050 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intTmu0:4;	// Equalizer ? parameter update interva ??0
		uint32 intTmu1:4;	// Equalizer ? parameter update interva ??1
		uint32 intTmu2:4;	// Equalizer ? parameter update interva ??2
		uint32 intTmu3:4;	// Equalizer ? parameter update interva ??3
		uint32 reserved0:16;
	} bitFields;
} RegModem11BMdmbtmu_u;

/*REG_MODEM_11B_MDMBCNTL1 0xC060 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intRxmaxlength:12;	// Max accepted received length
		uint32 intRxlenchken:1;	// Rx length check mode
		uint32 intDscrmodesel:1;	// Selects dscr_mode in decode path
		uint32 reserved0:18;
	} bitFields;
} RegModem11BMdmbcntl1_u;

/*REG_MODEM_11B_MDMBRFCNTL 0xC070 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 intTxconst:8;	// I data DAC valuue before Tx packets
		uint32 intTxenddel:8;	// Tx front-end delay
		uint32 reserved1:8;
	} bitFields;
} RegModem11BMdmbrfcntl_u;

/*REG_MODEM_11B_MDMBCCA 0xC080 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 intCcamode:3;	// CCA mode
		uint32 reserved1:21;
	} bitFields;
} RegModem11BMdmbcca_u;

/*REG_MODEM_11B_MDMBEQCNTL 0xC090 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intEqtime:4;	// Equalizer switch from CCA delay
		uint32 reserved0:1;
		uint32 intEsttime:5;	// Equalizer en to estimation delay
		uint32 intComptime:5;	// Estimation start to compensation start
		uint32 reserved1:1;
		uint32 intEqhold:12;	// Last param update to equalizer stop
		uint32 reserved2:4;
	} bitFields;
} RegModem11BMdmbeqcntl_u;

/*REG_MODEM_11B_MDMBCNTL2 0xC0A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intLooptime:4;	// AGC/CCA to phase and carrier offset
		uint32 reserved0:4;
		uint32 intSynctime:6;	// Peak detector sync duration
		uint32 reserved1:2;
		uint32 intPrecomp:6;	// Energey detection to frequency pre-compensation
		uint32 reserved2:2;
		uint32 intMaxstage:6;	// timing offset compensation DSSS/CCK interpolator stages
		uint32 reserved3:2;
	} bitFields;
} RegModem11BMdmbcntl2_u;

/*REG_MODEM_11B_MDMBSTAT0 0xC0B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 regDcoffseti:6;	// DC Offset Estimation I
		uint32 reserved0:2;
		uint32 regDcoffsetq:6;	// DC Offset Estimation Q
		uint32 reserved1:2;
		uint32 regEqsumi:8;	// Equalizer coefficients sum I
		uint32 regEqsumq:8;	// Equalizer coefficients sum Q
	} bitFields;
} RegModem11BMdmbstat0_u;

/*REG_MODEM_11B_MDMBSTAT1 0xC0C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 regFreqoffestim:8;	// Last Packet Frequency Offset Estimation
		uint32 regIqgainestim:7;	// Last Packet Gain Estimation
		uint32 reserved0:17;
	} bitFields;
} RegModem11BMdmbstat1_u;

/*REG_MODEM_11B_MDMBVERSION 0xC0D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intUpg:8;	// Modem version upgrade number
		uint32 intRel:8;	// Modem version release number
		uint32 intBuild:16;	// Modem version build number
	} bitFields;
} RegModem11BMdmbversion_u;



#endif // _MODEM_11B_REGS_H_
