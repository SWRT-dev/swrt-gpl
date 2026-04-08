
/***********************************************************************************
File:				DummyPhyRegs.h
Module:				dummyPhy
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _DUMMY_PHY_REGS_H_
#define _DUMMY_PHY_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define DUMMY_PHY_BASE_ADDRESS                             MEMORY_MAP_UNIT_69_BASE_ADDRESS
#define	REG_DUMMY_PHY_DPHY_REG000            (DUMMY_PHY_BASE_ADDRESS + 0x0)
#define	REG_DUMMY_PHY_DPHY_REG001            (DUMMY_PHY_BASE_ADDRESS + 0x4)
#define	REG_DUMMY_PHY_DPHY_REG002            (DUMMY_PHY_BASE_ADDRESS + 0x8)
#define	REG_DUMMY_PHY_DPHY_REG003            (DUMMY_PHY_BASE_ADDRESS + 0xC)
#define	REG_DUMMY_PHY_DPHY_REG004            (DUMMY_PHY_BASE_ADDRESS + 0x10)
#define	REG_DUMMY_PHY_DPHY_REG005            (DUMMY_PHY_BASE_ADDRESS + 0x14)
#define	REG_DUMMY_PHY_DPHY_REG006            (DUMMY_PHY_BASE_ADDRESS + 0x18)
#define	REG_DUMMY_PHY_DPHY_REG007            (DUMMY_PHY_BASE_ADDRESS + 0x1C)
#define	REG_DUMMY_PHY_DPHY_REG008            (DUMMY_PHY_BASE_ADDRESS + 0x20)
#define	REG_DUMMY_PHY_DPHY_REG009            (DUMMY_PHY_BASE_ADDRESS + 0x24)
#define	REG_DUMMY_PHY_DPHY_REG00A            (DUMMY_PHY_BASE_ADDRESS + 0x28)
#define	REG_DUMMY_PHY_DPHY_REG00B            (DUMMY_PHY_BASE_ADDRESS + 0x2C)
#define	REG_DUMMY_PHY_DPHY_REG00C            (DUMMY_PHY_BASE_ADDRESS + 0x30)
#define	REG_DUMMY_PHY_DPHY_REG00D            (DUMMY_PHY_BASE_ADDRESS + 0x34)
#define	REG_DUMMY_PHY_DPHY_REG00E            (DUMMY_PHY_BASE_ADDRESS + 0x38)
#define	REG_DUMMY_PHY_DPHY_REG00F            (DUMMY_PHY_BASE_ADDRESS + 0x3C)
#define	REG_DUMMY_PHY_DPHY_REG010            (DUMMY_PHY_BASE_ADDRESS + 0x40)
#define	REG_DUMMY_PHY_DPHY_REG011            (DUMMY_PHY_BASE_ADDRESS + 0x44)
#define	REG_DUMMY_PHY_DPHY_REG012            (DUMMY_PHY_BASE_ADDRESS + 0x48)
#define	REG_DUMMY_PHY_DPHY_REG013            (DUMMY_PHY_BASE_ADDRESS + 0x4C)
#define	REG_DUMMY_PHY_DPHY_REG01A            (DUMMY_PHY_BASE_ADDRESS + 0x68)
#define	REG_DUMMY_PHY_DPHY_REG01B            (DUMMY_PHY_BASE_ADDRESS + 0x6C)
#define	REG_DUMMY_PHY_DPHY_REG01C            (DUMMY_PHY_BASE_ADDRESS + 0x70)
#define	REG_DUMMY_PHY_DPHY_REG01D            (DUMMY_PHY_BASE_ADDRESS + 0x74)
#define	REG_DUMMY_PHY_DPHY_REG_AC_CAPABLE    (DUMMY_PHY_BASE_ADDRESS + 0x78)
#define	REG_DUMMY_PHY_DPHY_REG01F            (DUMMY_PHY_BASE_ADDRESS + 0x7C)
#define	REG_DUMMY_PHY_DPHY_REG020            (DUMMY_PHY_BASE_ADDRESS + 0x80)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_DUMMY_PHY_DPHY_REG000 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 byteErrorRate:32;	// byte_error_rate
	} bitFields;
} RegDummyPhyDphyReg000_u;

/*REG_DUMMY_PHY_DPHY_REG001 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beErrorRate:32;	// be_error_rate
	} bitFields;
} RegDummyPhyDphyReg001_u;

/*REG_DUMMY_PHY_DPHY_REG002 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beErrorNumOfBytes:20;	// be_error_num_of_bytes
		uint32 reserved0:12;
	} bitFields;
} RegDummyPhyDphyReg002_u;

/*REG_DUMMY_PHY_DPHY_REG003 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetErrorRate:32;	// packet_error_rate
	} bitFields;
} RegDummyPhyDphyReg003_u;

/*REG_DUMMY_PHY_DPHY_REG004 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfFeedbackType:2;	// bf_feedback_type
		uint32 bfImplicitEnable:1;	// bf_implicit_enable
		uint32 reserved0:29;
	} bitFields;
} RegDummyPhyDphyReg004_u;

/*REG_DUMMY_PHY_DPHY_REG005 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearCnt:1;	// clear_cnt
		uint32 latchCnt:1;	// latch_cnt
		uint32 reserved0:30;
	} bitFields;
} RegDummyPhyDphyReg005_u;

/*REG_DUMMY_PHY_DPHY_REG006 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 transmittedPktsCnt:32;	// transmitted_pkts_cnt
	} bitFields;
} RegDummyPhyDphyReg006_u;

/*REG_DUMMY_PHY_DPHY_REG007 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedTotalPktsCnt:32;	// recieved_total_pkts_cnt
	} bitFields;
} RegDummyPhyDphyReg007_u;

/*REG_DUMMY_PHY_DPHY_REG008 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedGoodPktsCnt:32;	// recieved_good_pkts_cnt
	} bitFields;
} RegDummyPhyDphyReg008_u;

/*REG_DUMMY_PHY_DPHY_REG009 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedUnsupportedPktsCnt:32;	// recieved_unsupported_pkts_cnt
	} bitFields;
} RegDummyPhyDphyReg009_u;

/*REG_DUMMY_PHY_DPHY_REG00A 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedCollisionPktsCnt:32;	// recieved_collision_pkts_cnt
	} bitFields;
} RegDummyPhyDphyReg00A_u;

/*REG_DUMMY_PHY_DPHY_REG00B 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedErrorPktsCnt:32;	// recieved_error_pkts_cnt
	} bitFields;
} RegDummyPhyDphyReg00B_u;

/*REG_DUMMY_PHY_DPHY_REG00C 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedByteCnt:32;	// recieved_byte_cnt
	} bitFields;
} RegDummyPhyDphyReg00C_u;

/*REG_DUMMY_PHY_DPHY_REG00D 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 transmittedPktsCntLatch:32;	// transmitted_pkts_cnt_latch
	} bitFields;
} RegDummyPhyDphyReg00D_u;

/*REG_DUMMY_PHY_DPHY_REG00E 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedTotalPktsCntLatch:32;	// recieved_total_pkts_cnt_latch
	} bitFields;
} RegDummyPhyDphyReg00E_u;

/*REG_DUMMY_PHY_DPHY_REG00F 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedGoodPktsCntLatch:32;	// recieved_good_pkts_cnt_latch
	} bitFields;
} RegDummyPhyDphyReg00F_u;

/*REG_DUMMY_PHY_DPHY_REG010 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedUnsupportedPktsCntLatch:32;	// recieved_unsupported_pkts_cnt_latch
	} bitFields;
} RegDummyPhyDphyReg010_u;

/*REG_DUMMY_PHY_DPHY_REG011 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedCollisionPktsCntLatch:32;	// recieved_collision_pkts_cnt_latch
	} bitFields;
} RegDummyPhyDphyReg011_u;

/*REG_DUMMY_PHY_DPHY_REG012 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedErrorPktsCntLatch:32;	// recieved_error_pkts_cnt_latch
	} bitFields;
} RegDummyPhyDphyReg012_u;

/*REG_DUMMY_PHY_DPHY_REG013 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recievedByteCntLatch:32;	// recieved_byte_cnt_latch
	} bitFields;
} RegDummyPhyDphyReg013_u;

/*REG_DUMMY_PHY_DPHY_REG01A 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsInit:32;	// prbs_init
	} bitFields;
} RegDummyPhyDphyReg01A_u;

/*REG_DUMMY_PHY_DPHY_REG01B 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsType:32;	// prbs_type
	} bitFields;
} RegDummyPhyDphyReg01B_u;

/*REG_DUMMY_PHY_DPHY_REG01C 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsClear:1;	// prbs_clear
		uint32 reserved0:31;
	} bitFields;
} RegDummyPhyDphyReg01C_u;

/*REG_DUMMY_PHY_DPHY_REG01D 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 legacyBwErrorRate:32;	// legacy_bw_error_rate
	} bitFields;
} RegDummyPhyDphyReg01D_u;

/*REG_DUMMY_PHY_DPHY_REG_AC_CAPABLE 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 acCapable:1;	// ac_capable
		uint32 reserved0:31;
	} bitFields;
} RegDummyPhyDphyRegAcCapable_u;

/*REG_DUMMY_PHY_DPHY_REG01F 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 channelPrimary:4;	// channel_primary
		uint32 reserved0:28;
	} bitFields;
} RegDummyPhyDphyReg01F_u;

/*REG_DUMMY_PHY_DPHY_REG020 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwrThr:8;	// cca_pwr_thr
		uint32 ccaMpdPwrThr:8;	// cca_mpd_pwr_thr
		uint32 detectionPwrThr:8;	// detection_pwr_thr
		uint32 reserved0:8;
	} bitFields;
} RegDummyPhyDphyReg020_u;



#endif // _DUMMY_PHY_REGS_H_
