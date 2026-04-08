
/***********************************************************************************
File:				AggBuilderRegs.h
Module:				aggBuilder
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _AGG_BUILDER_REGS_H_
#define _AGG_BUILDER_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define AGG_BUILDER_BASE_ADDRESS                             MEMORY_MAP_UNIT_21_BASE_ADDRESS
#define	REG_AGG_BUILDER_PACKET_DENSITY_BYTE_OVERHEAD               (AGG_BUILDER_BASE_ADDRESS + 0x0)
#define	REG_AGG_BUILDER_TAIL_BIT_LENGTH_OVERHEAD1                  (AGG_BUILDER_BASE_ADDRESS + 0x4)
#define	REG_AGG_BUILDER_TAIL_BIT_LENGTH_OVERHEAD2                  (AGG_BUILDER_BASE_ADDRESS + 0x8)
#define	REG_AGG_BUILDER_PHY_PREAMBLE_DURATION_OVERHEAD             (AGG_BUILDER_BASE_ADDRESS + 0xC)
#define	REG_AGG_BUILDER_PHY_PREAMBLE_DURATION_OVERHEAD_11B         (AGG_BUILDER_BASE_ADDRESS + 0x10)
#define	REG_AGG_BUILDER_RTS_CTS2SELF_METHOD                        (AGG_BUILDER_BASE_ADDRESS + 0x14)
#define	REG_AGG_BUILDER_ETH_TYPE_OVERHEAD                          (AGG_BUILDER_BASE_ADDRESS + 0x18)
#define	REG_AGG_BUILDER_PSDU_CONSTRUCT_ENABLE_BITS                 (AGG_BUILDER_BASE_ADDRESS + 0x1C)
#define	REG_AGG_BUILDER_MAX_MSDU_SIZE1                             (AGG_BUILDER_BASE_ADDRESS + 0x20)
#define	REG_AGG_BUILDER_MAX_MSDU_SIZE2                             (AGG_BUILDER_BASE_ADDRESS + 0x24)
#define	REG_AGG_BUILDER_MAX_MPDU_SIZE1                             (AGG_BUILDER_BASE_ADDRESS + 0x28)
#define	REG_AGG_BUILDER_MAX_MPDU_SIZE2                             (AGG_BUILDER_BASE_ADDRESS + 0x2C)
#define	REG_AGG_BUILDER_MAX_MPDU_SIZE3                             (AGG_BUILDER_BASE_ADDRESS + 0x30)
#define	REG_AGG_BUILDER_MAX_AMPDU_SIZE_11N                         (AGG_BUILDER_BASE_ADDRESS + 0x34)
#define	REG_AGG_BUILDER_MAX_AMPDU_SIZE_11AC                        (AGG_BUILDER_BASE_ADDRESS + 0x38)
#define	REG_AGG_BUILDER_AGG_CONFIG_DB                              (AGG_BUILDER_BASE_ADDRESS + 0x3C)
#define	REG_AGG_BUILDER_AGG_CONFIG_DB_4B_OFFSET                    (AGG_BUILDER_BASE_ADDRESS + 0x40)
#define	REG_AGG_BUILDER_AGG_BUILDER_START_PULSE                    (AGG_BUILDER_BASE_ADDRESS + 0x44)
#define	REG_AGG_BUILDER_AGG_BUILDER_DEBUG_SM1                      (AGG_BUILDER_BASE_ADDRESS + 0x4C)
#define	REG_AGG_BUILDER_AGG_BUILDER_DEBUG_SM2                      (AGG_BUILDER_BASE_ADDRESS + 0x50)
#define	REG_AGG_BUILDER_AGG_BUILDER_DEBUG_SM3                      (AGG_BUILDER_BASE_ADDRESS + 0x54)
#define	REG_AGG_BUILDER_AGG_BUILDER_TIMER_MIN_MAX                  (AGG_BUILDER_BASE_ADDRESS + 0x58)
#define	REG_AGG_BUILDER_AGG_BUILDER_TIMER                          (AGG_BUILDER_BASE_ADDRESS + 0x5C)
#define	REG_AGG_BUILDER_ZERO_TIMER_THRESHOLD                       (AGG_BUILDER_BASE_ADDRESS + 0x60)
#define	REG_AGG_BUILDER_BEACON_TIM_LENGTH_OVERHEAD                 (AGG_BUILDER_BASE_ADDRESS + 0x64)
#define	REG_AGG_BUILDER_START_OPERATION_CONFIG_PD_TYPES            (AGG_BUILDER_BASE_ADDRESS + 0x68)
#define	REG_AGG_BUILDER_TX_STD_FORBIDDEN_GENRISC_ACCESS            (AGG_BUILDER_BASE_ADDRESS + 0x6C)
#define	REG_AGG_BUILDER_AGG_BUILDER_STATUS_IRQ                     (AGG_BUILDER_BASE_ADDRESS + 0x70)
#define	REG_AGG_BUILDER_AGG_BUILDER_STATUS_IRQ_CLR                 (AGG_BUILDER_BASE_ADDRESS + 0x74)
#define	REG_AGG_BUILDER_AGG_BUILDER_STATUS_IRQ_MASK                (AGG_BUILDER_BASE_ADDRESS + 0x78)
#define	REG_AGG_BUILDER_DATA_PDS_TOTAL_RD_COUNT_MIN_LIMIT          (AGG_BUILDER_BASE_ADDRESS + 0x7C)
#define	REG_AGG_BUILDER_DATA_PDS_TOTAL_RD_COUNT_USP0_1             (AGG_BUILDER_BASE_ADDRESS + 0x80)
#define	REG_AGG_BUILDER_DATA_PDS_TOTAL_RD_COUNT_USP2_3             (AGG_BUILDER_BASE_ADDRESS + 0x84)
#define	REG_AGG_BUILDER_STOP_USP0_PSDU_DATA_REASON                 (AGG_BUILDER_BASE_ADDRESS + 0x88)
#define	REG_AGG_BUILDER_STOP_USP1_PSDU_DATA_REASON                 (AGG_BUILDER_BASE_ADDRESS + 0x8C)
#define	REG_AGG_BUILDER_STOP_USP2_PSDU_DATA_REASON                 (AGG_BUILDER_BASE_ADDRESS + 0x90)
#define	REG_AGG_BUILDER_STOP_USP3_PSDU_DATA_REASON                 (AGG_BUILDER_BASE_ADDRESS + 0x94)
#define	REG_AGG_BUILDER_USP0_DATA_CONSTRUCT_ERROR_STATUS           (AGG_BUILDER_BASE_ADDRESS + 0x98)
#define	REG_AGG_BUILDER_USP1_DATA_CONSTRUCT_ERROR_STATUS           (AGG_BUILDER_BASE_ADDRESS + 0x9C)
#define	REG_AGG_BUILDER_USP2_DATA_CONSTRUCT_ERROR_STATUS           (AGG_BUILDER_BASE_ADDRESS + 0xA0)
#define	REG_AGG_BUILDER_USP3_DATA_CONSTRUCT_ERROR_STATUS           (AGG_BUILDER_BASE_ADDRESS + 0xA4)
#define	REG_AGG_BUILDER_DATA_PDS_TOTAL_TX_COUNT_USP0_1             (AGG_BUILDER_BASE_ADDRESS + 0xA8)
#define	REG_AGG_BUILDER_DATA_PDS_TOTAL_TX_COUNT_USP2_3             (AGG_BUILDER_BASE_ADDRESS + 0xAC)
#define	REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR0_VALUE                (AGG_BUILDER_BASE_ADDRESS + 0xB0)
#define	REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR0_MASK                 (AGG_BUILDER_BASE_ADDRESS + 0xB4)
#define	REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR1_VALUE                (AGG_BUILDER_BASE_ADDRESS + 0xB8)
#define	REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR1_MASK                 (AGG_BUILDER_BASE_ADDRESS + 0xBC)
#define	REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR3_VALUE                (AGG_BUILDER_BASE_ADDRESS + 0xC0)
#define	REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR3_MASK                 (AGG_BUILDER_BASE_ADDRESS + 0xC4)
#define	REG_AGG_BUILDER_OVERRIDE_DB_PARAMETERS                     (AGG_BUILDER_BASE_ADDRESS + 0xC8)
#define	REG_AGG_BUILDER_ARRIVAL_SETUP_TIME                         (AGG_BUILDER_BASE_ADDRESS + 0xCC)
#define	REG_AGG_BUILDER_ARRIVAL_PROCESS_TIME                       (AGG_BUILDER_BASE_ADDRESS + 0xD0)
#define	REG_AGG_BUILDER_ARRIVAL_128BYTE_TIME                       (AGG_BUILDER_BASE_ADDRESS + 0xD4)
#define	REG_AGG_BUILDER_DEPARTURE_INIT_TIME                        (AGG_BUILDER_BASE_ADDRESS + 0xD8)
#define	REG_AGG_BUILDER_ZLD_EXTENSION_TIME                         (AGG_BUILDER_BASE_ADDRESS + 0xDC)
#define	REG_AGG_BUILDER_MU_MAX_NSTS                                (AGG_BUILDER_BASE_ADDRESS + 0xE0)
#define	REG_AGG_BUILDER_FORCE_RTS_CTS2SELF_VAP                     (AGG_BUILDER_BASE_ADDRESS + 0xE4)
#define	REG_AGG_BUILDER_RTS_CTS2SELF_CONTROL                       (AGG_BUILDER_BASE_ADDRESS + 0xE8)
#define	REG_AGG_BUILDER_BF_IMPLICIT_FILLER_PHY_MODE                (AGG_BUILDER_BASE_ADDRESS + 0xEC)
#define	REG_AGG_BUILDER_PREAMBLE_11B_TYPE_CONTROL                  (AGG_BUILDER_BASE_ADDRESS + 0xF0)
#define	REG_AGG_BUILDER_PREAMBLE_11B_TYPE_VAP                      (AGG_BUILDER_BASE_ADDRESS + 0xF4)
#define	REG_AGG_BUILDER_USE_ONLY_CCA_20S_PWR_PHY_MODE_HT           (AGG_BUILDER_BASE_ADDRESS + 0xF8)
#define	REG_AGG_BUILDER_TX_STICKY_CCA_FREE                         (AGG_BUILDER_BASE_ADDRESS + 0xFC)
#define	REG_AGG_BUILDER_TXOP_AIR_TIME_DURATION                     (AGG_BUILDER_BASE_ADDRESS + 0x100)
#define	REG_AGG_BUILDER_BF_SEQ_AIR_TIME_DURATION                   (AGG_BUILDER_BASE_ADDRESS + 0x104)
#define	REG_AGG_BUILDER_TX_DUR_CALC_CONTROL                        (AGG_BUILDER_BASE_ADDRESS + 0x108)
#define	REG_AGG_BUILDER_TX_SEQ_AIR_TIME_DUR                        (AGG_BUILDER_BASE_ADDRESS + 0x10C)
#define	REG_AGG_BUILDER_NO_PSDU_TIME_LIMIT_11B_EN                  (AGG_BUILDER_BASE_ADDRESS + 0x110)
#define	REG_AGG_BUILDER_BF_RPT_ANALYZER_DELTA_MCS_NSS              (AGG_BUILDER_BASE_ADDRESS + 0x114)
#define	REG_AGG_BUILDER_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS0TO3      (AGG_BUILDER_BASE_ADDRESS + 0x118)
#define	REG_AGG_BUILDER_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS4TO7      (AGG_BUILDER_BASE_ADDRESS + 0x11C)
#define	REG_AGG_BUILDER_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS8TO9      (AGG_BUILDER_BASE_ADDRESS + 0x120)
#define	REG_AGG_BUILDER_RX_BF_RPT_MCS_NSS_PARAMETERS               (AGG_BUILDER_BASE_ADDRESS + 0x124)
#define	REG_AGG_BUILDER_RX_BF_RPT_SNR                              (AGG_BUILDER_BASE_ADDRESS + 0x128)
#define	REG_AGG_BUILDER_RD_DB_TCRS_DATA_802_11_PD_TYPE             (AGG_BUILDER_BASE_ADDRESS + 0x12C)
#define	REG_AGG_BUILDER_AGG_BUILDER_ENABLE                         (AGG_BUILDER_BASE_ADDRESS + 0x130)
#define	REG_AGG_BUILDER_AGG_BUILDER_LOGGER                         (AGG_BUILDER_BASE_ADDRESS + 0x134)
#define	REG_AGG_BUILDER_AGG_BUILDER_LOGGER_ACTIVE                  (AGG_BUILDER_BASE_ADDRESS + 0x138)
#define	REG_AGG_BUILDER_PRE_TX_CALIBRATION_IND_EN                  (AGG_BUILDER_BASE_ADDRESS + 0x13C)
#define	REG_AGG_BUILDER_PRE_TX_CALIBRATION_IND_SET                 (AGG_BUILDER_BASE_ADDRESS + 0x140)
#define	REG_AGG_BUILDER_RX_BF_RPT_SNR_STREAM0                      (AGG_BUILDER_BASE_ADDRESS + 0x144)
#define	REG_AGG_BUILDER_RX_BF_RPT_SNR_STREAM1                      (AGG_BUILDER_BASE_ADDRESS + 0x148)
#define	REG_AGG_BUILDER_RX_BF_RPT_SNR_STREAM2                      (AGG_BUILDER_BASE_ADDRESS + 0x14C)
#define	REG_AGG_BUILDER_RX_BF_RPT_SNR_STREAM3                      (AGG_BUILDER_BASE_ADDRESS + 0x150)
#define	REG_AGG_BUILDER_DEBUG_MAC_TX_DUR_AIR_TIME_CALC_IN1         (AGG_BUILDER_BASE_ADDRESS + 0x154)
#define	REG_AGG_BUILDER_DEBUG_MAC_TX_DUR_AIR_TIME_CALC_IN2         (AGG_BUILDER_BASE_ADDRESS + 0x158)
#define	REG_AGG_BUILDER_DEBUG_MAC_TX_DUR_AIR_TIME_CALC_OUT         (AGG_BUILDER_BASE_ADDRESS + 0x15C)
#define	REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_RESULT_EN             (AGG_BUILDER_BASE_ADDRESS + 0x160)
#define	REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_RESULT                (AGG_BUILDER_BASE_ADDRESS + 0x164)
#define	REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP0_RESULT1          (AGG_BUILDER_BASE_ADDRESS + 0x168)
#define	REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP0_RESULT2          (AGG_BUILDER_BASE_ADDRESS + 0x16C)
#define	REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP1_RESULT1          (AGG_BUILDER_BASE_ADDRESS + 0x170)
#define	REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP1_RESULT2          (AGG_BUILDER_BASE_ADDRESS + 0x174)
#define	REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP2_RESULT1          (AGG_BUILDER_BASE_ADDRESS + 0x178)
#define	REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP2_RESULT2          (AGG_BUILDER_BASE_ADDRESS + 0x17C)
#define	REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP3_RESULT1          (AGG_BUILDER_BASE_ADDRESS + 0x180)
#define	REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP3_RESULT2          (AGG_BUILDER_BASE_ADDRESS + 0x184)
#define	REG_AGG_BUILDER_MU_MULT_USRS_TRAINING_CONTROL              (AGG_BUILDER_BASE_ADDRESS + 0x188)
#define	REG_AGG_BUILDER_MU_BAR_BACK_AIR_TIME_DURATION              (AGG_BUILDER_BASE_ADDRESS + 0x18C)
#define	REG_AGG_BUILDER_MAX_SECONDARY_DURATION_PRIMARY_END         (AGG_BUILDER_BASE_ADDRESS + 0x190)
#define	REG_AGG_BUILDER_MU_GRP_DB_ADDR                             (AGG_BUILDER_BASE_ADDRESS + 0x194)
#define	REG_AGG_BUILDER_FIRST_PD_POINTER_USP0_1                    (AGG_BUILDER_BASE_ADDRESS + 0x198)
#define	REG_AGG_BUILDER_FIRST_PD_POINTER_USP2_3                    (AGG_BUILDER_BASE_ADDRESS + 0x19C)
#define	REG_AGG_BUILDER_MAX_MPDU_SIZE_ESTIMATED_USP0_1             (AGG_BUILDER_BASE_ADDRESS + 0x1A0)
#define	REG_AGG_BUILDER_MAX_MPDU_SIZE_ESTIMATED_USP2_3             (AGG_BUILDER_BASE_ADDRESS + 0x1A4)
#define	REG_AGG_BUILDER_PPDU_AIR_TIME_DURATION_ESTIMATED_USP0_1    (AGG_BUILDER_BASE_ADDRESS + 0x1A8)
#define	REG_AGG_BUILDER_PPDU_AIR_TIME_DURATION_ESTIMATED_USP2_3    (AGG_BUILDER_BASE_ADDRESS + 0x1AC)
#define	REG_AGG_BUILDER_MAX_PSDU_SIZE_ESTIMATED_USP0               (AGG_BUILDER_BASE_ADDRESS + 0x1B0)
#define	REG_AGG_BUILDER_MAX_PSDU_SIZE_ESTIMATED_USP1               (AGG_BUILDER_BASE_ADDRESS + 0x1B4)
#define	REG_AGG_BUILDER_MAX_PSDU_SIZE_ESTIMATED_USP2               (AGG_BUILDER_BASE_ADDRESS + 0x1B8)
#define	REG_AGG_BUILDER_MAX_PSDU_SIZE_ESTIMATED_USP3               (AGG_BUILDER_BASE_ADDRESS + 0x1BC)
#define	REG_AGG_BUILDER_MIN_MPDU_SIZE_USP0_1                       (AGG_BUILDER_BASE_ADDRESS + 0x1C0)
#define	REG_AGG_BUILDER_MIN_MPDU_SIZE_USP2_3                       (AGG_BUILDER_BASE_ADDRESS + 0x1C4)
#define	REG_AGG_BUILDER_TX_SEL_RESULT                              (AGG_BUILDER_BASE_ADDRESS + 0x1C8)
#define	REG_AGG_BUILDER_TX_SEL_USP0_RESULT1                        (AGG_BUILDER_BASE_ADDRESS + 0x1CC)
#define	REG_AGG_BUILDER_TX_SEL_USP0_RESULT2                        (AGG_BUILDER_BASE_ADDRESS + 0x1D0)
#define	REG_AGG_BUILDER_TX_SEL_USP1_RESULT1                        (AGG_BUILDER_BASE_ADDRESS + 0x1D4)
#define	REG_AGG_BUILDER_TX_SEL_USP1_RESULT2                        (AGG_BUILDER_BASE_ADDRESS + 0x1D8)
#define	REG_AGG_BUILDER_TX_SEL_USP2_RESULT1                        (AGG_BUILDER_BASE_ADDRESS + 0x1DC)
#define	REG_AGG_BUILDER_TX_SEL_USP2_RESULT2                        (AGG_BUILDER_BASE_ADDRESS + 0x1F0)
#define	REG_AGG_BUILDER_TX_SEL_USP3_RESULT1                        (AGG_BUILDER_BASE_ADDRESS + 0x1F4)
#define	REG_AGG_BUILDER_TX_SEL_USP3_RESULT2                        (AGG_BUILDER_BASE_ADDRESS + 0x1F8)
#define	REG_AGG_BUILDER_BW_LIMIT_VAP                               (AGG_BUILDER_BASE_ADDRESS + 0x200)
#define	REG_AGG_BUILDER_MU_USP_SECONDARY_NO_TX_REASON              (AGG_BUILDER_BASE_ADDRESS + 0x204)
#define	REG_AGG_BUILDER_MAX_PSDU_TX_TIME_LIMIT                     (AGG_BUILDER_BASE_ADDRESS + 0x208)
#define	REG_AGG_BUILDER_AGG_BUILDER_SPARE                          (AGG_BUILDER_BASE_ADDRESS + 0x3FC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_AGG_BUILDER_PACKET_DENSITY_BYTE_OVERHEAD 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetDensityByteOverhead:6;	// packet density byte overhead
		uint32 reserved0:26;
	} bitFields;
} RegAggBuilderPacketDensityByteOverhead_u;

/*REG_AGG_BUILDER_TAIL_BIT_LENGTH_OVERHEAD1 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tailBitLengthOverhead111Ag:6;	// tail bit length overhead1 11ag
		uint32 reserved0:2;
		uint32 tailBitLengthOverhead111B:6;	// tail bit length overhead1 11b
		uint32 reserved1:2;
		uint32 tailBitLengthOverhead111N:6;	// tail bit length overhead1 11n
		uint32 reserved2:2;
		uint32 tailBitLengthOverhead111Ac:6;	// tail bit length overhead1 11ac
		uint32 reserved3:2;
	} bitFields;
} RegAggBuilderTailBitLengthOverhead1_u;

/*REG_AGG_BUILDER_TAIL_BIT_LENGTH_OVERHEAD2 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tailBitLengthOverhead211Ag:6;	// tail bit length overhead2 11ag
		uint32 reserved0:2;
		uint32 tailBitLengthOverhead211B:6;	// tail bit length overhead2 11b
		uint32 reserved1:2;
		uint32 tailBitLengthOverhead211N:6;	// tail bit length overhead2 11n
		uint32 reserved2:2;
		uint32 tailBitLengthOverhead211Ac:6;	// tail bit length overhead2 11ac
		uint32 reserved3:2;
	} bitFields;
} RegAggBuilderTailBitLengthOverhead2_u;

/*REG_AGG_BUILDER_PHY_PREAMBLE_DURATION_OVERHEAD 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPreambleOverhead11Ag:8;	// phy preamble duration overhead 11ag
		uint32 phyPreambleOverhead11N:8;	// phy preamble duration overhead 11n
		uint32 phyPreambleOverhead11Ac:8;	// phy preamble duration overhead 11ac
		uint32 reserved0:8;
	} bitFields;
} RegAggBuilderPhyPreambleDurationOverhead_u;

/*REG_AGG_BUILDER_PHY_PREAMBLE_DURATION_OVERHEAD_11B 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPreambleOverhead11BShort:8;	// phy preamble duration overhead 11b short
		uint32 phyPreambleOverhead11BLong:8;	// phy preamble duration overhead 11b long
		uint32 reserved0:16;
	} bitFields;
} RegAggBuilderPhyPreambleDurationOverhead11B_u;

/*REG_AGG_BUILDER_RTS_CTS2SELF_METHOD 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtsCtsThresholdMethod:2;	// RTS-CTS threshold method
		uint32 cts2SelfThresholdMethod:2;	// CTS-to-Self threshold method
		uint32 reserved0:28;
	} bitFields;
} RegAggBuilderRtsCts2SelfMethod_u;

/*REG_AGG_BUILDER_ETH_TYPE_OVERHEAD 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ethType0Overhead:6;	// Eth. Type0 overhead
		uint32 reserved0:2;
		uint32 ethType1Overhead:6;	// Eth. Type1 overhead
		uint32 reserved1:2;
		uint32 ethType2Overhead:6;	// Eth. Type2 overhead
		uint32 reserved2:2;
		uint32 ethType3Overhead:6;	// Eth. Type3 overhead
		uint32 reserved3:2;
	} bitFields;
} RegAggBuilderEthTypeOverhead_u;

/*REG_AGG_BUILDER_PSDU_CONSTRUCT_ENABLE_BITS 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 allowed1MpduInAMpduHt:1;	// allowed 1mpdu in A-MPDU HT
		uint32 suAllowed1MpduInAMpduVht:1;	// SU allowed 1mpdu in A-MPDU VHT
		uint32 reserved0:2;
		uint32 retryDbMaxMsdusAtAmsduLimitCheckEn:1;	// Enable verify max MSDUs at A-MSDU count (read from DB) in case of retransmission PD
		uint32 retryDbMaxMpduLengthLimitCheckEn:1;	// Enable verify max MPDU length limit (read from DB) in case of retransmission PD
		uint32 muAllowed1MpduInAMpduVht:1;	// MU allowed 1mpdu in A-MPDU VHT
		uint32 reserved1:25;
	} bitFields;
} RegAggBuilderPsduConstructEnableBits_u;

/*REG_AGG_BUILDER_MAX_MSDU_SIZE1 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMsduSize11B:14;	// max msdu size 11b
		uint32 reserved0:2;
		uint32 maxMsduSize11Ag:14;	// max msdu size 11a/g
		uint32 reserved1:2;
	} bitFields;
} RegAggBuilderMaxMsduSize1_u;

/*REG_AGG_BUILDER_MAX_MSDU_SIZE2 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMsduSize11N:14;	// max msdu size 11n
		uint32 reserved0:2;
		uint32 maxMsduSize11Ac:14;	// max msdu size 11ac
		uint32 reserved1:2;
	} bitFields;
} RegAggBuilderMaxMsduSize2_u;

/*REG_AGG_BUILDER_MAX_MPDU_SIZE1 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduSize11B:14;	// max MPDU size 11b
		uint32 reserved0:2;
		uint32 maxMpduSize11Ag:14;	// max MPDU size 11ag
		uint32 reserved1:2;
	} bitFields;
} RegAggBuilderMaxMpduSize1_u;

/*REG_AGG_BUILDER_MAX_MPDU_SIZE2 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduSize11NAmpdu:14;	// max MPDU size 11n in A-MPDU
		uint32 reserved0:2;
		uint32 maxMpduSize11NNotAmpdu:14;	// max MPDU size 11n in not A-MPDU
		uint32 reserved1:2;
	} bitFields;
} RegAggBuilderMaxMpduSize2_u;

/*REG_AGG_BUILDER_MAX_MPDU_SIZE3 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduSize11Ac:14;	// max MPDU size 11ac
		uint32 reserved0:18;
	} bitFields;
} RegAggBuilderMaxMpduSize3_u;

/*REG_AGG_BUILDER_MAX_AMPDU_SIZE_11N 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxAmpduSize11N:20;	// max A-MPDU size 11n
		uint32 reserved0:12;
	} bitFields;
} RegAggBuilderMaxAmpduSize11N_u;

/*REG_AGG_BUILDER_MAX_AMPDU_SIZE_11AC 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxAmpduSize11Ac:20;	// max A-MPDU size 11ac
		uint32 reserved0:12;
	} bitFields;
} RegAggBuilderMaxAmpduSize11Ac_u;

/*REG_AGG_BUILDER_AGG_CONFIG_DB 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggConfigDbBaseAddr:24;	// AGG config DB base addr
		uint32 aggConfigDbEntry4BSize:8;	// AGG config DB word size
	} bitFields;
} RegAggBuilderAggConfigDb_u;

/*REG_AGG_BUILDER_AGG_CONFIG_DB_4B_OFFSET 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggConfigDb4BOffset:8;	// AGG config DB word offset
		uint32 reserved0:24;
	} bitFields;
} RegAggBuilderAggConfigDb4BOffset_u;

/*REG_AGG_BUILDER_AGG_BUILDER_START_PULSE 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggBuilderStartPulse:1;	// AGG builder start pulse - write '1'
		uint32 aggBuilderAbortPulse:1;	// AGG builder abort pulse - write '1'
		uint32 reserved0:30;
	} bitFields;
} RegAggBuilderAggBuilderStartPulse_u;

/*REG_AGG_BUILDER_AGG_BUILDER_DEBUG_SM1 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggBuilderMainSm:5;	// AGG builder main SM
		uint32 aggBuilderRtsCts2SelfSm:3;	// AGG builder RTS CTS2Self SM
		uint32 aggBuilderRdDbSm:4;	// AGG builder Read DB SM
		uint32 txDurSm:4;	// Tx duration State machine
		uint32 aggBuilderUsp0EstSm:4;	// AGG builder estimation SM
		uint32 aggBuilderUsp1EstSm:4;	// AGG builder estimation SM
		uint32 aggBuilderUsp2EstSm:4;	// AGG builder estimation SM
		uint32 aggBuilderUsp3EstSm:4;	// AGG builder estimation SM
	} bitFields;
} RegAggBuilderAggBuilderDebugSm1_u;

/*REG_AGG_BUILDER_AGG_BUILDER_DEBUG_SM2 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggBuilderBeaconSm:3;	// AGG builder beacon SM
		uint32 ccaBwSm:2;	// BW based CCA state machine
		uint32 bfRptAnalyzerSm:3;	// BF report analyzer state machine
		uint32 nxtUspSm:3;	// Next USP state machine
		uint32 qMngIfSm:3;	// Q manager I/F state machine
		uint32 txselUnlockIfSm:4;	// Tx Selector unlock I/F state machine
		uint32 reserved0:14;
	} bitFields;
} RegAggBuilderAggBuilderDebugSm2_u;

/*REG_AGG_BUILDER_AGG_BUILDER_DEBUG_SM3 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggUsp0DataConstructSm:3;	// AGG builder data construction state machine
		uint32 aggUsp1DataConstructSm:3;	// AGG builder data construction state machine
		uint32 aggUsp2DataConstructSm:3;	// AGG builder data construction state machine
		uint32 aggUsp3DataConstructSm:3;	// AGG builder data construction state machine
		uint32 usp0RdPdsSm:4;	// Read PDs state machine
		uint32 usp1RdPdsSm:4;	// Read PDs state machine
		uint32 usp2RdPdsSm:4;	// Read PDs state machine
		uint32 usp3RdPdsSm:4;	// Read PDs state machine
		uint32 reserved0:4;
	} bitFields;
} RegAggBuilderAggBuilderDebugSm3_u;

/*REG_AGG_BUILDER_AGG_BUILDER_TIMER_MIN_MAX 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggBuilderTimerMin:12;	// AGG builder timer minimum
		uint32 reserved0:3;
		uint32 aggBuilderTimerMinEn:1;	// AGG builder timer minimum enable
		uint32 aggBuilderTimerMax:12;	// AGG builder timer maximum
		uint32 reserved1:3;
		uint32 aggBuilderTimerMaxEn:1;	// AGG builder timer maximum enable
	} bitFields;
} RegAggBuilderAggBuilderTimerMinMax_u;

/*REG_AGG_BUILDER_AGG_BUILDER_TIMER 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggBuilderTimer:12;	// AGG builder timer
		uint32 reserved0:20;
	} bitFields;
} RegAggBuilderAggBuilderTimer_u;

/*REG_AGG_BUILDER_ZERO_TIMER_THRESHOLD 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 zeroTimerThreshold:8;	// zero timer threshold
		uint32 reserved0:7;
		uint32 zeroTimerThresholdEn:1;	// zero timer threshold enable
		uint32 reserved1:16;
	} bitFields;
} RegAggBuilderZeroTimerThreshold_u;

/*REG_AGG_BUILDER_BEACON_TIM_LENGTH_OVERHEAD 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beaconTimLengthOverhead:8;	// beacon TIM length overhead
		uint32 reserved0:24;
	} bitFields;
} RegAggBuilderBeaconTimLengthOverhead_u;

/*REG_AGG_BUILDER_START_OPERATION_CONFIG_PD_TYPES 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startOperationConfigPdType0:2;	// Start operation configuration PD type 0
		uint32 startOperationConfigPdType1:2;	// Start operation configuration PD type 1
		uint32 startOperationConfigPdType2:2;	// Start operation configuration PD type 2
		uint32 startOperationConfigPdType3:2;	// Start operation configuration PD type 3
		uint32 startOperationConfigPdType4:2;	// Start operation configuration PD type 4
		uint32 startOperationConfigPdType5:2;	// Start operation configuration PD type 5
		uint32 startOperationConfigPdType6:2;	// Start operation configuration PD type 6
		uint32 startOperationConfigPdType7:2;	// Start operation configuration PD type 7
		uint32 startOperationConfigPdType8:2;	// Start operation configuration PD type 8
		uint32 startOperationConfigPdType9:2;	// Start operation configuration PD type 9
		uint32 startOperationConfigPdType10:2;	// Start operation configuration PD type 10
		uint32 startOperationConfigPdType11:2;	// Start operation configuration PD type 11
		uint32 startOperationConfigPdType12:2;	// Start operation configuration PD type 12
		uint32 startOperationConfigPdType13:2;	// Start operation configuration PD type 13
		uint32 startOperationConfigPdType14:2;	// Start operation configuration PD type 14
		uint32 startOperationConfigPdType15:2;	// Start operation configuration PD type 15
	} bitFields;
} RegAggBuilderStartOperationConfigPdTypes_u;

/*REG_AGG_BUILDER_TX_STD_FORBIDDEN_GENRISC_ACCESS 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txStdAddrForbiddenGenriscAccess:10;	// tx STD address forbidden GenRisc access
		uint32 reserved0:22;
	} bitFields;
} RegAggBuilderTxStdForbiddenGenriscAccess_u;

/*REG_AGG_BUILDER_AGG_BUILDER_STATUS_IRQ 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 firstDataPdAMsduCtrErrorIrq:1;	// first_data_pd_a_msdu_ctr_error_irq
		uint32 firstDataPdMsduSizeErrorIrq:1;	// first_data_pd_msdu_size_error_irq
		uint32 firstDataPdMpduSizeDbErrorIrq:1;	// first_data_pd_mpdu_size_db_error_irq
		uint32 firstDataPdPsduSizeErrorIrq:1;	// first_data_pd_psdu_size_error_irq
		uint32 firstDataPdPsduSizeEstimatedErrorIrq:1;	// first_data_pd_psdu_size_estimated_error_irq
		uint32 firstDataPdNotRetriesMpdusCountErrorIrq:1;	// first_data_pd_not_retries_mpdus_count_error_irq
		uint32 startOperationIrq:1;	// start_operation_irq
		uint32 doneOperationIrq:1;	// done_operation_irq
		uint32 operationTimerErrorIrq:1;	// operation_timer_error_irq
		uint32 reserved0:1;
		uint32 txhdStartDuringOperationIrq:1;	// txhd_start_during_operation_irq
		uint32 cpuStartDuringOperationIrq:1;	// cpu_start_during_operation_irq
		uint32 txStdForbiddenGenriscAccessIrq:1;	// tx_std_forbidden_genrisc_access_irq
		uint32 dataPdsRdCountLessMinLimitIrq:1;	// data_pds_rd_count_less_min_limit_irq
		uint32 reserved1:1;
		uint32 firstPdPointerNullIrq:1;	// first_pd_pointer_null_irq
		uint32 firstDataPdMpduSizeConfigErrorIrq:1;	// first_data_pd_mpdu_size_config_error_irq
		uint32 firstDataPdPsduSizeSecondaryPrimaryEndErrorIrq:1;	// first_data_pd_psdu_size_secondary_primary_end_error_irq
		uint32 txSelPrimaryNotValidErrIrq:1;	// tx_sel_primary_not_valid_err_irq
		uint32 txSelNoUspValidErrIrq:1;	// tx_sel_no_usp_valid_err_irq
		uint32 reserved2:12;
	} bitFields;
} RegAggBuilderAggBuilderStatusIrq_u;

/*REG_AGG_BUILDER_AGG_BUILDER_STATUS_IRQ_CLR 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 firstDataPdAMsduCtrErrorIrqClr:1;	// first_data_pd_a_msdu_ctr_error_irq_clr
		uint32 firstDataPdMsduSizeErrorIrqClr:1;	// first_data_pd_msdu_size_error_irq_clr
		uint32 firstDataPdMpduSizeDbErrorIrqClr:1;	// first_data_pd_mpdu_size_db_error_irq_clr
		uint32 firstDataPdPsduSizeErrorIrqClr:1;	// first_data_pd_psdu_size_error_irq_clr
		uint32 firstDataPdPsduSizeEstimatedErrorIrqClr:1;	// first_data_pd_psdu_size_estimated_error_irq_clr
		uint32 firstDataPdNotRetriesMpdusCountErrorIrqClr:1;	// first_data_pd_not_retries_mpdus_count_error_irq_clr
		uint32 startOperationIrqClr:1;	// start_operation_irq_clr
		uint32 doneOperationIrqClr:1;	// done_operation_irq_clr
		uint32 operationTimerErrorIrqClr:1;	// operation_timer_error_irq_clr
		uint32 reserved0:1;
		uint32 txhdStartDuringOperationIrqClr:1;	// txhd_start_during_operation_irq_clr
		uint32 cpuStartDuringOperationIrqClr:1;	// cpu_start_during_operation_irq_clr
		uint32 txStdForbiddenGenriscAccessIrqClr:1;	// tx_std_forbidden_genrisc_access_irq_clr
		uint32 dataPdsRdCountLessMinLimitIrqClr:1;	// data_pds_rd_count_less_min_limit_irq_clr
		uint32 reserved1:1;
		uint32 firstPdPointerNullIrqClr:1;	// first_pd_pointer_null_irq_clr
		uint32 firstDataPdMpduSizeConfigErrorIrqClr:1;	// first_data_pd_mpdu_size_config_error_irq_clr
		uint32 firstDataPdPsduSizeSecondaryPrimaryEndErrorIrqClr:1;	// first_data_pd_psdu_size_secondary_primary_end_error_irq_clr
		uint32 txSelPrimaryNotValidErrIrqClr:1;	// tx_sel_primary_not_valid_err_irq_clr
		uint32 txSelNoUspValidErrIrqClr:1;	// tx_sel_no_usp_valid_err_irq_clr
		uint32 reserved2:12;
	} bitFields;
} RegAggBuilderAggBuilderStatusIrqClr_u;

/*REG_AGG_BUILDER_AGG_BUILDER_STATUS_IRQ_MASK 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 firstDataPdAMsduCtrErrorIrqMask:1;	// first_data_pd_a_msdu_ctr_error_irq_mask
		uint32 firstDataPdMsduSizeErrorIrqMask:1;	// first_data_pd_msdu_size_error_irq_mask
		uint32 firstDataPdMpduSizeDbErrorIrqMask:1;	// first_data_pd_mpdu_size_db_error_irq_mask
		uint32 firstDataPdPsduSizeErrorIrqMask:1;	// first_data_pd_psdu_size_error_irq_mask
		uint32 firstDataPdPsduSizeEstimatedErrorIrqMask:1;	// first_data_pd_psdu_size_estimated_error_irq_mask
		uint32 firstDataPdNotRetriesMpdusCountErrorIrqMask:1;	// first_data_pd_not_retries_mpdus_count_error_irq_mask
		uint32 startOperationIrqMask:1;	// start_operation_irq_mask
		uint32 doneOperationIrqMask:1;	// done_operation_irq_mask
		uint32 operationTimerErrorIrqMask:1;	// operation_timer_error_irq_mask
		uint32 reserved0:1;
		uint32 txhdStartDuringOperationIrqMask:1;	// txhd_start_during_operation_irq_mask
		uint32 cpuStartDuringOperationIrqMask:1;	// cpu_start_during_operation_irq_mask
		uint32 txStdForbiddenGenriscAccessIrqMask:1;	// tx_std_forbidden_genrisc_access_irq_mask
		uint32 dataPdsRdCountLessMinLimitIrqMask:1;	// data_pds_rd_count_less_min_limit_irq_mask
		uint32 reserved1:1;
		uint32 firstPdPointerNullIrqMask:1;	// first_pd_pointer_null_irq_mask
		uint32 firstDataPdMpduSizeConfigErrorIrqMask:1;	// first_data_pd_mpdu_size_config_error_irq_mask
		uint32 firstDataPdPsduSizeSecondaryPrimaryEndErrorIrqMask:1;	// first_data_pd_psdu_size_secondary_primary_end_error_irq_mask
		uint32 txSelPrimaryNotValidErrIrqMask:1;	// tx_sel_primary_not_valid_err_irq_mask
		uint32 txSelNoUspValidErrIrqMask:1;	// tx_sel_no_usp_valid_err_irq_mask
		uint32 reserved2:12;
	} bitFields;
} RegAggBuilderAggBuilderStatusIrqMask_u;

/*REG_AGG_BUILDER_DATA_PDS_TOTAL_RD_COUNT_MIN_LIMIT 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataPdsTotalRdCountMinLimit:9;	// Data PDs total read count minimum limit
		uint32 reserved0:23;
	} bitFields;
} RegAggBuilderDataPdsTotalRdCountMinLimit_u;

/*REG_AGG_BUILDER_DATA_PDS_TOTAL_RD_COUNT_USP0_1 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataPdsTotalRdCountUsp0:9;	// Data PDs total read count
		uint32 reserved0:7;
		uint32 dataPdsTotalRdCountUsp1:9;	// Data PDs total read count
		uint32 reserved1:7;
	} bitFields;
} RegAggBuilderDataPdsTotalRdCountUsp01_u;

/*REG_AGG_BUILDER_DATA_PDS_TOTAL_RD_COUNT_USP2_3 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataPdsTotalRdCountUsp2:9;	// Data PDs total read count
		uint32 reserved0:7;
		uint32 dataPdsTotalRdCountUsp3:9;	// Data PDs total read count
		uint32 reserved1:7;
	} bitFields;
} RegAggBuilderDataPdsTotalRdCountUsp23_u;

/*REG_AGG_BUILDER_STOP_USP0_PSDU_DATA_REASON 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopUsp0PsduAMsduCtrLimit:1;	// Stop due Maximum MSDUs count in A-MSDU limitation 
		uint32 stopUsp0PsduMsduSizeLimit:1;	// Stop due Maximum MSDU (PD) byte length limitation
		uint32 stopUsp0PsduMpduSizeDbLimit:1;	// Stop due Maximum MPDU byte length limitation. The limit value is based on DB 
		uint32 stopUsp0PsduMpduSizeConfigLimit:1;	// Stop due Maximum MPDU byte length limitation. The limit value is based on configuration per PHY mode
		uint32 stopUsp0PsduSizeLimit:1;	// Stop due Maximum PSDU byte length limitation
		uint32 stopUsp0PsduEstimatedSizeLimit:1;	// Stop due Maximum PSDU byte length estimated limitation
		uint32 stopUsp0PsduSizeSecondaryPrimaryEndLimit:1;	// Stop due Maximum Secondary PSDU byte length estimated limitation,  , Valid only to the Secondary USPs and when Primary USP has been completed.
		uint32 stopUsp0PsduNotRetriesMpdusCountLimit:1;	// Stop due Number of MPDUs with “fresh” Sequence number limitation.
		uint32 stopUsp0PsduMpdusCountLimit:1;	// Stop due Max MPDUs count in A-MPDU limitation.
		uint32 stopUsp0PsduNextPdNotData:1;	// Stop due Tx PD with frame type different then data (PD type 0000/0001).
		uint32 stopUsp0PsduExternalComplete:1;	// Stop due external complete indication
		uint32 stopUsp0PsduPdPointsNull:1;	// Stop due PD next pointer is NULL
		uint32 reserved0:20;
	} bitFields;
} RegAggBuilderStopUsp0PsduDataReason_u;

/*REG_AGG_BUILDER_STOP_USP1_PSDU_DATA_REASON 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopUsp1PsduAMsduCtrLimit:1;	// Stop due Maximum MSDUs count in A-MSDU limitation 
		uint32 stopUsp1PsduMsduSizeLimit:1;	// Stop due Maximum MSDU (PD) byte length limitation
		uint32 stopUsp1PsduMpduSizeDbLimit:1;	// Stop due Maximum MPDU byte length limitation. The limit value is based on DB 
		uint32 stopUsp1PsduMpduSizeConfigLimit:1;	// Stop due Maximum MPDU byte length limitation. The limit value is based on configuration per PHY mode
		uint32 stopUsp1PsduSizeLimit:1;	// Stop due Maximum PSDU byte length limitation
		uint32 stopUsp1PsduEstimatedSizeLimit:1;	// Stop due Maximum PSDU byte length estimated limitation
		uint32 stopUsp1PsduSizeSecondaryPrimaryEndLimit:1;	// Stop due Maximum Secondary PSDU byte length estimated limitation,  , Valid only to the Secondary USPs and when Primary USP has been completed.
		uint32 stopUsp1PsduNotRetriesMpdusCountLimit:1;	// Stop due Number of MPDUs with “fresh” Sequence number limitation.
		uint32 stopUsp1PsduMpdusCountLimit:1;	// Stop due Max MPDUs count in A-MPDU limitation.
		uint32 stopUsp1PsduNextPdNotData:1;	// Stop due Tx PD with frame type different then data (PD type 0000/0001).
		uint32 stopUsp1PsduExternalComplete:1;	// Stop due external complete indication
		uint32 stopUsp1PsduPdPointsNull:1;	// Stop due PD next pointer is NULL
		uint32 reserved0:20;
	} bitFields;
} RegAggBuilderStopUsp1PsduDataReason_u;

/*REG_AGG_BUILDER_STOP_USP2_PSDU_DATA_REASON 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopUsp2PsduAMsduCtrLimit:1;	// Stop due Maximum MSDUs count in A-MSDU limitation 
		uint32 stopUsp2PsduMsduSizeLimit:1;	// Stop due Maximum MSDU (PD) byte length limitation
		uint32 stopUsp2PsduMpduSizeDbLimit:1;	// Stop due Maximum MPDU byte length limitation. The limit value is based on DB 
		uint32 stopUsp2PsduMpduSizeConfigLimit:1;	// Stop due Maximum MPDU byte length limitation. The limit value is based on configuration per PHY mode
		uint32 stopUsp2PsduSizeLimit:1;	// Stop due Maximum PSDU byte length limitation
		uint32 stopUsp2PsduEstimatedSizeLimit:1;	// Stop due Maximum PSDU byte length estimated limitation
		uint32 stopUsp2PsduSizeSecondaryPrimaryEndLimit:1;	// Stop due Maximum Secondary PSDU byte length estimated limitation,  , Valid only to the Secondary USPs and when Primary USP has been completed.
		uint32 stopUsp2PsduNotRetriesMpdusCountLimit:1;	// Stop due Number of MPDUs with “fresh” Sequence number limitation.
		uint32 stopUsp2PsduMpdusCountLimit:1;	// Stop due Max MPDUs count in A-MPDU limitation.
		uint32 stopUsp2PsduNextPdNotData:1;	// Stop due Tx PD with frame type different then data (PD type 0000/0001).
		uint32 stopUsp2PsduExternalComplete:1;	// Stop due external complete indication
		uint32 stopUsp2PsduPdPointsNull:1;	// Stop due PD next pointer is NULL
		uint32 reserved0:20;
	} bitFields;
} RegAggBuilderStopUsp2PsduDataReason_u;

/*REG_AGG_BUILDER_STOP_USP3_PSDU_DATA_REASON 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopUsp3PsduAMsduCtrLimit:1;	// Stop due Maximum MSDUs count in A-MSDU limitation 
		uint32 stopUsp3PsduMsduSizeLimit:1;	// Stop due Maximum MSDU (PD) byte length limitation
		uint32 stopUsp3PsduMpduSizeDbLimit:1;	// Stop due Maximum MPDU byte length limitation. The limit value is based on DB 
		uint32 stopUsp3PsduMpduSizeConfigLimit:1;	// Stop due Maximum MPDU byte length limitation. The limit value is based on configuration per PHY mode
		uint32 stopUsp3PsduSizeLimit:1;	// Stop due Maximum PSDU byte length limitation
		uint32 stopUsp3PsduEstimatedSizeLimit:1;	// Stop due Maximum PSDU byte length estimated limitation
		uint32 stopUsp3PsduSizeSecondaryPrimaryEndLimit:1;	// Stop due Maximum Secondary PSDU byte length estimated limitation,  , Valid only to the Secondary USPs and when Primary USP has been completed.
		uint32 stopUsp3PsduNotRetriesMpdusCountLimit:1;	// Stop due Number of MPDUs with “fresh” Sequence number limitation.
		uint32 stopUsp3PsduMpdusCountLimit:1;	// Stop due Max MPDUs count in A-MPDU limitation.
		uint32 stopUsp3PsduNextPdNotData:1;	// Stop due Tx PD with frame type different then data (PD type 0000/0001).
		uint32 stopUsp3PsduExternalComplete:1;	// Stop due external complete indication
		uint32 stopUsp3PsduPdPointsNull:1;	// Stop due PD next pointer is NULL
		uint32 reserved0:20;
	} bitFields;
} RegAggBuilderStopUsp3PsduDataReason_u;

/*REG_AGG_BUILDER_USP0_DATA_CONSTRUCT_ERROR_STATUS 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp0FirstDataPdAMsduCtrError:1;	// First data PD can't be added due to Maximum MSDUs count in A-MSDU limitation 
		uint32 usp0FirstDataPdMsduSizeError:1;	// First data PD can't be added due to Maximum MSDU (PD) byte length limitation
		uint32 usp0FirstDataPdMpduSizeDbError:1;	// First data PD can't be added due to Maximum MPDU byte length limitation. The limit value is based on DB 
		uint32 usp0FirstDataPdPsduSizeError:1;	// First data PD can't be added due to Maximum PSDU byte length limitation
		uint32 usp0FirstDataPdPsduSizeEstimatedError:1;	// First data PD can't be added due to Maximum PSDU byte length estimated limitation
		uint32 usp0FirstDataPdNotRetriesMpdusCountError:1;	// First data PD can't be added due to Number of MPDUs with “fresh” Sequence number limitation
		uint32 usp0FirstDataPdMpduSizeConfigError:1;	// First data PD can't be added due to Maximum MPDU byte length limitation. The limit value is based on configuration per PHY mode
		uint32 usp0FirstDataPdPsduSizeSecondaryPrimaryEndError:1;	// First data PD can't be added due to Maximum Secondary PSDU byte length estimated limitation. Valid only to the Secondary USPs and when Primary USP has been completed. 
		uint32 reserved0:24;
	} bitFields;
} RegAggBuilderUsp0DataConstructErrorStatus_u;

/*REG_AGG_BUILDER_USP1_DATA_CONSTRUCT_ERROR_STATUS 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp1FirstDataPdAMsduCtrError:1;	// First data PD can't be added due to Maximum MSDUs count in A-MSDU limitation 
		uint32 usp1FirstDataPdMsduSizeError:1;	// First data PD can't be added due to Maximum MSDU (PD) byte length limitation
		uint32 usp1FirstDataPdMpduSizeDbError:1;	// First data PD can't be added due to Maximum MPDU byte length limitation. The limit value is based on DB 
		uint32 usp1FirstDataPdPsduSizeError:1;	// First data PD can't be added due to Maximum PSDU byte length limitation
		uint32 usp1FirstDataPdPsduSizeEstimatedError:1;	// First data PD can't be added due to Maximum PSDU byte length estimated limitation
		uint32 usp1FirstDataPdNotRetriesMpdusCountError:1;	// First data PD can't be added due to Number of MPDUs with “fresh” Sequence number limitation
		uint32 usp1FirstDataPdMpduSizeConfigError:1;	// First data PD can't be added due to Maximum MPDU byte length limitation. The limit value is based on configuration per PHY mode
		uint32 usp1FirstDataPdPsduSizeSecondaryPrimaryEndError:1;	// First data PD can't be added due to Maximum Secondary PSDU byte length estimated limitation. Valid only to the Secondary USPs and when Primary USP has been completed. 
		uint32 reserved0:24;
	} bitFields;
} RegAggBuilderUsp1DataConstructErrorStatus_u;

/*REG_AGG_BUILDER_USP2_DATA_CONSTRUCT_ERROR_STATUS 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp2FirstDataPdAMsduCtrError:1;	// First data PD can't be added due to Maximum MSDUs count in A-MSDU limitation 
		uint32 usp2FirstDataPdMsduSizeError:1;	// First data PD can't be added due to Maximum MSDU (PD) byte length limitation
		uint32 usp2FirstDataPdMpduSizeDbError:1;	// First data PD can't be added due to Maximum MPDU byte length limitation. The limit value is based on DB 
		uint32 usp2FirstDataPdPsduSizeError:1;	// First data PD can't be added due to Maximum PSDU byte length limitation
		uint32 usp2FirstDataPdPsduSizeEstimatedError:1;	// First data PD can't be added due to Maximum PSDU byte length estimated limitation
		uint32 usp2FirstDataPdNotRetriesMpdusCountError:1;	// First data PD can't be added due to Number of MPDUs with “fresh” Sequence number limitation
		uint32 usp2FirstDataPdMpduSizeConfigError:1;	// First data PD can't be added due to Maximum MPDU byte length limitation. The limit value is based on configuration per PHY mode
		uint32 usp2FirstDataPdPsduSizeSecondaryPrimaryEndError:1;	// First data PD can't be added due to Maximum Secondary PSDU byte length estimated limitation. Valid only to the Secondary USPs and when Primary USP has been completed. 
		uint32 reserved0:24;
	} bitFields;
} RegAggBuilderUsp2DataConstructErrorStatus_u;

/*REG_AGG_BUILDER_USP3_DATA_CONSTRUCT_ERROR_STATUS 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp3FirstDataPdAMsduCtrError:1;	// First data PD can't be added due to Maximum MSDUs count in A-MSDU limitation 
		uint32 usp3FirstDataPdMsduSizeError:1;	// First data PD can't be added due to Maximum MSDU (PD) byte length limitation
		uint32 usp3FirstDataPdMpduSizeDbError:1;	// First data PD can't be added due to Maximum MPDU byte length limitation. The limit value is based on DB 
		uint32 usp3FirstDataPdPsduSizeError:1;	// First data PD can't be added due to Maximum PSDU byte length limitation
		uint32 usp3FirstDataPdPsduSizeEstimatedError:1;	// First data PD can't be added due to Maximum PSDU byte length estimated limitation
		uint32 usp3FirstDataPdNotRetriesMpdusCountError:1;	// First data PD can't be added due to Number of MPDUs with “fresh” Sequence number limitation
		uint32 usp3FirstDataPdMpduSizeConfigError:1;	// First data PD can't be added due to Maximum MPDU byte length limitation. The limit value is based on configuration per PHY mode
		uint32 usp3FirstDataPdPsduSizeSecondaryPrimaryEndError:1;	// First data PD can't be added due to Maximum Secondary PSDU byte length estimated limitation. Valid only to the Secondary USPs and when Primary USP has been completed. 
		uint32 reserved0:24;
	} bitFields;
} RegAggBuilderUsp3DataConstructErrorStatus_u;

/*REG_AGG_BUILDER_DATA_PDS_TOTAL_TX_COUNT_USP0_1 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataPdsTotalTxCountUsp0:16;	// USP0 Data PDs total Tx count
		uint32 dataPdsTotalTxCountUsp1:16;	// USP1 Data PDs total Tx count
	} bitFields;
} RegAggBuilderDataPdsTotalTxCountUsp01_u;

/*REG_AGG_BUILDER_DATA_PDS_TOTAL_TX_COUNT_USP2_3 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataPdsTotalTxCountUsp2:16;	// USP2 Data PDs total Tx count
		uint32 dataPdsTotalTxCountUsp3:16;	// USP3 Data PDs total Tx count
	} bitFields;
} RegAggBuilderDataPdsTotalTxCountUsp23_u;

/*REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR0_VALUE 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideDbDataTcr0Value:24;	// Override DB Data TCR0 value
		uint32 reserved0:8;
	} bitFields;
} RegAggBuilderOverrideDbDataTcr0Value_u;

/*REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR0_MASK 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideDbDataTcr0Mask:24;	// Override DB Data TCR0 mask
		uint32 reserved0:8;
	} bitFields;
} RegAggBuilderOverrideDbDataTcr0Mask_u;

/*REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR1_VALUE 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideDbDataTcr1Value:24;	// Override DB Data TCR1 value
		uint32 reserved0:8;
	} bitFields;
} RegAggBuilderOverrideDbDataTcr1Value_u;

/*REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR1_MASK 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideDbDataTcr1Mask:24;	// Override DB Data TCR1 mask
		uint32 reserved0:8;
	} bitFields;
} RegAggBuilderOverrideDbDataTcr1Mask_u;

/*REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR3_VALUE 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideDbDataTcr3Value:24;	// Override DB Data TCR3 value
		uint32 reserved0:8;
	} bitFields;
} RegAggBuilderOverrideDbDataTcr3Value_u;

/*REG_AGG_BUILDER_OVERRIDE_DB_DATA_TCR3_MASK 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideDbDataTcr3Mask:24;	// Override DB Data TCR3 mask
		uint32 reserved0:8;
	} bitFields;
} RegAggBuilderOverrideDbDataTcr3Mask_u;

/*REG_AGG_BUILDER_OVERRIDE_DB_PARAMETERS 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableOverrideDbProtectionParams:1;	// Enable override DB protection parameters
		uint32 overrideDbRtsCtsTxMethod:2;	// override DB rts_cts_tx_method
		uint32 overrideDbCts2SelfTxMethod:2;	// override DB cts2self_tx_method
		uint32 overrideDbStaticBwEn:1;	// override DB static_bw_en
		uint32 overrideDbDynamicBwEn:1;	// override DB dynamic_bw_en
		uint32 reserved0:1;
		uint32 enableOverrideDbAggregationParams:1;	// enable override DB aggregation parameters
		uint32 overrideDbBaEnable:1;	// override DB ba_enable
		uint32 overrideDbPpduTxMode:2;	// override DB ppdu_tx_mode
		uint32 reserved1:20;
	} bitFields;
} RegAggBuilderOverrideDbParameters_u;

/*REG_AGG_BUILDER_ARRIVAL_SETUP_TIME 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 arrivalPdSetupTime:9;	// The minimum penalty of each PD by the DMA/Sender in case of no A-MSDU, in 1/64[us]
		uint32 reserved0:7;
		uint32 arrivalAMsduSetupTime:9;	// The minimum penalty of each PD by the DMA/Sender in case of A-MSDU, in 1/64[us]
		uint32 reserved1:7;
	} bitFields;
} RegAggBuilderArrivalSetupTime_u;

/*REG_AGG_BUILDER_ARRIVAL_PROCESS_TIME 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 arrivalProcessTime:9;	// The penalty for each PD, in 1/64[us]
		uint32 reserved0:23;
	} bitFields;
} RegAggBuilderArrivalProcessTime_u;

/*REG_AGG_BUILDER_ARRIVAL_128BYTE_TIME 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 arrival128ByteTime:7;	// The penalty of each 128Bytes by the DMA, in 1/64[us]
		uint32 reserved0:25;
	} bitFields;
} RegAggBuilderArrival128ByteTime_u;

/*REG_AGG_BUILDER_DEPARTURE_INIT_TIME 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 departureInitTime:13;	// The starting value of the "departure_length", in 1/64[us]
		uint32 reserved0:19;
	} bitFields;
} RegAggBuilderDepartureInitTime_u;

/*REG_AGG_BUILDER_ZLD_EXTENSION_TIME 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 zldExtensionTime:13;	// The extra ZLDs which added when required to add ZLDs due to departure/arrival, in 1/64[us]
		uint32 reserved0:19;
	} bitFields;
} RegAggBuilderZldExtensionTime_u;

/*REG_AGG_BUILDER_MU_MAX_NSTS 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muMaxNsts:3;	// Max number of allocated Nsts (minus one) for all USPs, when MU-MIMO transmission is selected.
		uint32 reserved0:29;
	} bitFields;
} RegAggBuilderMuMaxNsts_u;

/*REG_AGG_BUILDER_FORCE_RTS_CTS2SELF_VAP 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 forceRtsCtsVap:16;	// Bit per VAP: Force RTS-CTS Transmission (relevant only to STA Q_ID)
		uint32 forceCts2SelfVap:16;	// Bit per VAP: Force CTS-to-Self Transmission (relevant only to STA Q_ID)
	} bitFields;
} RegAggBuilderForceRtsCts2SelfVap_u;

/*REG_AGG_BUILDER_RTS_CTS2SELF_CONTROL 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ignoreDynamicBw20Mhz:1;	// Ignore dynamic enable bit from DB In case BW == 20MHz
		uint32 muPsduLengthRtsCts2SelfThrSumMaxN:1;	// The PSDU length which used for RTS-CTS/CTS2Self based on threshold: , 0: Maximum of the PSDU length of the active USPs. , 1: SUM of the PSDU length of the active USPs.
		uint32 reserved0:30;
	} bitFields;
} RegAggBuilderRtsCts2SelfControl_u;

/*REG_AGG_BUILDER_BF_IMPLICIT_FILLER_PHY_MODE 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfImplicitFillerPhyMode11B:1;	// Enable setting the BF implicit filler when PHY mode is 11b
		uint32 bfImplicitFillerPhyMode11Ag:1;	// Enable setting the BF implicit filler when PHY mode is 11a/g
		uint32 bfImplicitFillerPhyMode11N:1;	// Enable setting the BF implicit filler when PHY mode is 11n
		uint32 bfImplicitFillerPhyMode11Ac:1;	// Enable setting the BF implicit filler when PHY mode is 11ac
		uint32 reserved0:28;
	} bitFields;
} RegAggBuilderBfImplicitFillerPhyMode_u;

/*REG_AGG_BUILDER_PREAMBLE_11B_TYPE_CONTROL 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preamble11BTypeEn:1;	// Enable 11B preamble long/short setting based configuration instead of DB
		uint32 reserved0:3;
		uint32 preamble11BMcs000Replacement:3;	// MCS replacement in case of 11B MCS value of 000, i.e. short preamable 1Mb/s
		uint32 reserved1:25;
	} bitFields;
} RegAggBuilderPreamble11BTypeControl_u;

/*REG_AGG_BUILDER_PREAMBLE_11B_TYPE_VAP 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preamble11BTypeVap:16;	// 11B Preamble type, 1 bit per VAP: , 0 - short. , 1 - long.
		uint32 reserved0:16;
	} bitFields;
} RegAggBuilderPreamble11BTypeVap_u;

/*REG_AGG_BUILDER_USE_ONLY_CCA_20S_PWR_PHY_MODE_HT 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 useOnlyCca20SPwrPhyModeHt:16;	// Bit per VAP: Use only CCA 20MHz secondary power free for selection BW 40Mhz in PHY mdoe HT
		uint32 reserved0:16;
	} bitFields;
} RegAggBuilderUseOnlyCca20SPwrPhyModeHt_u;

/*REG_AGG_BUILDER_TX_STICKY_CCA_FREE 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txStickyCca20PFree:1;	// Tx sticky bit of CCA 20MHz primary free
		uint32 txStickyCca20SFree:1;	// Tx sticky bit of CCA 20MHz secondary free
		uint32 txStickyCca40SFree:1;	// Tx sticky bit of CCA 40MHz secondary free
		uint32 txStickyCca20PPwrFree:1;	// Tx sticky bit of CCA 20MHz primary power free
		uint32 txStickyCca20SPwrFree:1;	// Tx sticky bit of CCA 20MHz secondary power free
		uint32 txStickyCca40SPwrFree:1;	// Tx sticky bit of CCA 40MHz secondary power free
		uint32 reserved0:26;
	} bitFields;
} RegAggBuilderTxStickyCcaFree_u;

/*REG_AGG_BUILDER_TXOP_AIR_TIME_DURATION 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txopAirTimeDuration:15;	// TXOP multiple data transmissions air time duration [micro seconds]
		uint32 reserved0:17;
	} bitFields;
} RegAggBuilderTxopAirTimeDuration_u;

/*REG_AGG_BUILDER_BF_SEQ_AIR_TIME_DURATION 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfSeqAirTimeDuration:15;	// Air time duration of BF explicit sequence, [micro seconds]
		uint32 reserved0:17;
	} bitFields;
} RegAggBuilderBfSeqAirTimeDuration_u;

/*REG_AGG_BUILDER_TX_DUR_CALC_CONTROL 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDurAutoReplyDegradationFactor:4;	// Degradation factor for the Tx duration calculation for the auto reply of the main Tx
		uint32 reserved0:4;
		uint32 txSeqDurMainOfdmAdd6MicroEn:1;	// Enable adding 0x6 [us] when the PHY mode of the main Tx frame is OFDM (11a/g/n/ac) during Tx sequence air time calculation
		uint32 txMpduDurFieldOfdmDec6MicroEn:1;	// Enable decrease 0x6 [us] when the PHY mode of the Tx frame is OFDM (11a/g/n/ac) during Tx MPDU duration field calculation
		uint32 reserved1:22;
	} bitFields;
} RegAggBuilderTxDurCalcControl_u;

/*REG_AGG_BUILDER_TX_SEQ_AIR_TIME_DUR 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSeqAirTimeDur:18;	// Total Tx sequence air time duration [micro seconds]
		uint32 reserved0:14;
	} bitFields;
} RegAggBuilderTxSeqAirTimeDur_u;

/*REG_AGG_BUILDER_NO_PSDU_TIME_LIMIT_11B_EN 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noPpduTimeLimit11BEn:1;	// Enable ignore Max PPDU time limit DB parameter in PHY mode 11B
		uint32 reserved0:31;
	} bitFields;
} RegAggBuilderNoPsduTimeLimit11BEn_u;

/*REG_AGG_BUILDER_BF_RPT_ANALYZER_DELTA_MCS_NSS 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptDeltaMcsCpuRaFillerLow:4;	// Delta MCS compare value to select CPU RA filler when BF report MCS is lower than DB MCS
		uint32 bfRptDeltaMcsCpuRaFillerHigh:4;	// Delta MCS compare value to select CPU RA filler when BF report MCS is higher than DB MCS
		uint32 bfRptDeltaMcsDoNothingLow:4;	// Delta MCS compare value to select HW fast RA when BF report MCS is lower than DB MCS
		uint32 bfRptDeltaMcsDoNothingHigh:4;	// Delta MCS compare value to select HW fast RA when BF report MCS is higher than DB MCS
		uint32 bfRptDeltaNssCpuRaFillerLow:2;	// Delta Nss compare value to select CPU RA filler when BF report Nss is lower than DB Nss
		uint32 bfRptDeltaNssCpuRaFillerHigh:2;	// Delta Nss compare value to select CPU RA filler when BF report Nss is higher than DB Nss
		uint32 bfRptDeltaNssDoNothingLow:2;	// Delta Nss compare value to select HW fast RA when BF report Nss is lower than DB Nss
		uint32 bfRptDeltaNssDoNothingHigh:2;	// Delta Nss compare value to select HW fast RA when BF report Nss is higher than DB Nss
		uint32 reserved0:8;
	} bitFields;
} RegAggBuilderBfRptAnalyzerDeltaMcsNss_u;

/*REG_AGG_BUILDER_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS0TO3 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAnalyzerSnrThresholdMcs0To3:32;	// BF report analyzer SNR threshold, 8bits signed value per MCS
	} bitFields;
} RegAggBuilderBfRptAnalyzerSnrThresholdMcs0To3_u;

/*REG_AGG_BUILDER_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS4TO7 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAnalyzerSnrThresholdMcs4To7:32;	// BF report analyzer SNR threshold, 8bits signed value per MCS
	} bitFields;
} RegAggBuilderBfRptAnalyzerSnrThresholdMcs4To7_u;

/*REG_AGG_BUILDER_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS8TO9 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAnalyzerSnrThresholdMcs8To9:16;	// BF report analyzer SNR threshold, 8bits signed value per MCS
		uint32 reserved0:16;
	} bitFields;
} RegAggBuilderBfRptAnalyzerSnrThresholdMcs8To9_u;

/*REG_AGG_BUILDER_RX_BF_RPT_MCS_NSS_PARAMETERS 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptVhtHtN:1;	// BF report PHY mode based on Action code and category: 1-VHT, 0-HT
		uint32 rxBfRptMimoNc:2;	// Nc from BF report MIMO control
		uint32 rxBfRptMfb:7;	// BF report MCS feedback
		uint32 rxBfRptMfbValid:1;	// BF report MCS feeback valid indication
		uint32 reserved0:21;
	} bitFields;
} RegAggBuilderRxBfRptMcsNssParameters_u;

/*REG_AGG_BUILDER_RX_BF_RPT_SNR 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptSnr:32;	// BF report SNR 0-3
	} bitFields;
} RegAggBuilderRxBfRptSnr_u;

/*REG_AGG_BUILDER_RD_DB_TCRS_DATA_802_11_PD_TYPE 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdDbTcrsData80211PdType:16;	// Select read TCRs and BW limit of data or 802.11, 1 bit per PD type: , 0 - Read 802.11 TCRs. , 1 - Read Data TCRs.
		uint32 reserved0:16;
	} bitFields;
} RegAggBuilderRdDbTcrsData80211PdType_u;

/*REG_AGG_BUILDER_AGG_BUILDER_ENABLE 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggBuilderEnable:1;	// AGG builder enable
		uint32 aggBuilderMuEnable:1;	// AGG builder MU transmission enable
		uint32 reserved0:30;
	} bitFields;
} RegAggBuilderAggBuilderEnable_u;

/*REG_AGG_BUILDER_AGG_BUILDER_LOGGER 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggBuilderLoggerEn:1;	// AGG builder logger enable
		uint32 reserved0:7;
		uint32 aggBuilderLoggerPriority:2;	// AGG builder logger priority
		uint32 reserved1:22;
	} bitFields;
} RegAggBuilderAggBuilderLogger_u;

/*REG_AGG_BUILDER_AGG_BUILDER_LOGGER_ACTIVE 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggBuilderLoggerActive:1;	// AGG Builder logger active
		uint32 reserved0:31;
	} bitFields;
} RegAggBuilderAggBuilderLoggerActive_u;

/*REG_AGG_BUILDER_PRE_TX_CALIBRATION_IND_EN 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macPhyPreTxCalibrationIndEn:1;	// Enable MAC-PHY pre Tx calibration indication
		uint32 reserved0:31;
	} bitFields;
} RegAggBuilderPreTxCalibrationIndEn_u;

/*REG_AGG_BUILDER_PRE_TX_CALIBRATION_IND_SET 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preTxCalibrationIndSet:1;	// Generate the Pre Tx calibration indication, by write '1' - for Debug purpose
		uint32 reserved0:31;
	} bitFields;
} RegAggBuilderPreTxCalibrationIndSet_u;

/*REG_AGG_BUILDER_RX_BF_RPT_SNR_STREAM0 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptSnrStream0:32;	// BF report SNR of stream 0, with signed extension
	} bitFields;
} RegAggBuilderRxBfRptSnrStream0_u;

/*REG_AGG_BUILDER_RX_BF_RPT_SNR_STREAM1 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptSnrStream1:32;	// BF report SNR of stream 1, with signed extension
	} bitFields;
} RegAggBuilderRxBfRptSnrStream1_u;

/*REG_AGG_BUILDER_RX_BF_RPT_SNR_STREAM2 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptSnrStream2:32;	// BF report SNR of stream 2, with signed extension
	} bitFields;
} RegAggBuilderRxBfRptSnrStream2_u;

/*REG_AGG_BUILDER_RX_BF_RPT_SNR_STREAM3 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptSnrStream3:32;	// BF report SNR of stream 3, with signed extension
	} bitFields;
} RegAggBuilderRxBfRptSnrStream3_u;

/*REG_AGG_BUILDER_DEBUG_MAC_TX_DUR_AIR_TIME_CALC_IN1 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugTxDurAirTimeCalcMcsIn:6;	// Debug Tx duration Air time calculator MCS in
		uint32 debugTxDurAirTimeCalcBwIn:2;	// Debug Tx duration Air time calculator BW in
		uint32 debugTxDurAirTimeCalcPhyModeIn:2;	// Debug Tx duration Air time calculator PHY mode in
		uint32 debugTxDurAirTimeCalcStbcIn:2;	// Debug Tx duration Air time calculator STBC in
		uint32 debugTxDurAirTimeCalcScpIn:1;	// Debug Tx duration Air time calculator SCP in
		uint32 debugTxDurAirTimeCalcLdpcIn:1;	// Debug Tx duration Air time calculator LDPC in
		uint32 reserved0:18;
	} bitFields;
} RegAggBuilderDebugMacTxDurAirTimeCalcIn1_u;

/*REG_AGG_BUILDER_DEBUG_MAC_TX_DUR_AIR_TIME_CALC_IN2 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugTxDurAirTimeCalcLengthIn:20;	// Debug Tx duration Air time calculator length in
		uint32 reserved0:11;
		uint32 debugTxDurAirTimeCalcStartStrbIn:1;	// Debug Tx duration Air time calculator start strobe in
	} bitFields;
} RegAggBuilderDebugMacTxDurAirTimeCalcIn2_u;

/*REG_AGG_BUILDER_DEBUG_MAC_TX_DUR_AIR_TIME_CALC_OUT 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugTxDurAirTimeCalcResultOut:17;	// Debug Tx duration Air time calculator result out
		uint32 reserved0:13;
		uint32 debugTxDurAirTimeCalcAccessPending:1;	// Debug Tx duration Air time calculator access pending
		uint32 debugTxDurAirTimeCalcResultValid:1;	// Debug Tx duration Air time calculator result valid
	} bitFields;
} RegAggBuilderDebugMacTxDurAirTimeCalcOut_u;

/*REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_RESULT_EN 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideTxSelectorResultEn:1;	// override Tx selector result enanle
		uint32 reserved0:31;
	} bitFields;
} RegAggBuilderOverrideTxSelectorResultEn_u;

/*REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_RESULT 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideTxSelectorMuSuN:1;	// override Tx selector SU/MU selection, 1 - MU, 0 - SU
		uint32 overrideTxSelectorMuPrimaryUsp:2;	// override Tx selector Primary USP
		uint32 overrideTxSelectorMuGrpTxMode:2;	// override Tx selector MU Group Tx mode
		uint32 overrideTxSelectorUspValid:4;	// override Tx selector USP valid results
		uint32 overrideTxSelectorMuGrpEntry:7;	// override Tx selector MU Group entry index
		uint32 overrideTxSelectorUspLock:4;	// override Tx selector USP lock
		uint32 overrideTxSelectorGroupLock:1;	// override Tx selector MU group lock
		uint32 reserved0:11;
	} bitFields;
} RegAggBuilderOverrideTxSelectorResult_u;

/*REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP0_RESULT1 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideTxSelectorUsp0TxqId:18;	// override Tx selector USP0 TXQ_ID
		uint32 overrideTxSelectorUsp0AcId:2;	// override Tx selector USP0 AC_ID
		uint32 overrideTxSelectorUsp0SuBfSeq:2;	// override Tx selector USP0 SU BF sequence
		uint32 reserved0:10;
	} bitFields;
} RegAggBuilderOverrideTxSelectorUsp0Result1_u;

/*REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP0_RESULT2 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideTxSelectorUsp0BitmapStatus:32;	// override Tx selector USP0 BitMAP Status
	} bitFields;
} RegAggBuilderOverrideTxSelectorUsp0Result2_u;

/*REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP1_RESULT1 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideTxSelectorUsp1TxqId:18;	// override Tx selector USP1 TXQ_ID
		uint32 overrideTxSelectorUsp1AcId:2;	// override Tx selector USP1 AC_ID
		uint32 overrideTxSelectorUsp1SuBfSeq:2;	// override Tx selector USP1 SU BF sequence
		uint32 reserved0:10;
	} bitFields;
} RegAggBuilderOverrideTxSelectorUsp1Result1_u;

/*REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP1_RESULT2 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideTxSelectorUsp1BitmapStatus:32;	// override Tx selector USP1 BitMAP Status
	} bitFields;
} RegAggBuilderOverrideTxSelectorUsp1Result2_u;

/*REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP2_RESULT1 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideTxSelectorUsp2TxqId:18;	// override Tx selector USP2 TXQ_ID
		uint32 overrideTxSelectorUsp2AcId:2;	// override Tx selector USP2 AC_ID
		uint32 overrideTxSelectorUsp2SuBfSeq:2;	// override Tx selector USP2 SU BF sequence
		uint32 reserved0:10;
	} bitFields;
} RegAggBuilderOverrideTxSelectorUsp2Result1_u;

/*REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP2_RESULT2 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideTxSelectorUsp2BitmapStatus:32;	// override Tx selector USP2 BitMAP Status
	} bitFields;
} RegAggBuilderOverrideTxSelectorUsp2Result2_u;

/*REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP3_RESULT1 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideTxSelectorUsp3TxqId:18;	// override Tx selector USP3 TXQ_ID
		uint32 overrideTxSelectorUsp3AcId:2;	// override Tx selector USP3 AC_ID
		uint32 overrideTxSelectorUsp3SuBfSeq:2;	// override Tx selector USP3 SU BF sequence
		uint32 reserved0:10;
	} bitFields;
} RegAggBuilderOverrideTxSelectorUsp3Result1_u;

/*REG_AGG_BUILDER_OVERRIDE_TX_SELECTOR_USP3_RESULT2 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overrideTxSelectorUsp3BitmapStatus:32;	// override Tx selector USP3 BitMAP Status
	} bitFields;
} RegAggBuilderOverrideTxSelectorUsp3Result2_u;

/*REG_AGG_BUILDER_MU_MULT_USRS_TRAINING_CONTROL 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muMultUsrsTrainingAirTimeDuration:15;	// Air time duration of MU multiple users BF training sequence, [micro seconds]
		uint32 reserved0:1;
		uint32 muMultUsrsTrainingBwLimit:2;	// BW limit of MU multiple users BF training sequence: , 00: 20MHz , 01: 40MHz , 10: 80MHz
		uint32 muMultUsrsTrainingFillerType:2;	// Filler type of MU multiple users BF training sequence: , 00: Filler Disabled ( NA )   , 01: Filler using CTS-to-Self  , 10: Filler using RTS-CTS to the primary STA/TID    , 11: Filler using MAC NDP+ACK to the primary STA/TID
		uint32 muMultUsrsTrainingProtection:2;	// Protection setting of MU multiple users BF training sequence: , 00: None , 01: RTS-CTS , 10: CTS-to-Self
		uint32 muMultUsrsTrainingRtsCtsFormat:2;	// RTS-CTS format setting of MU multiple users BF training sequence: , 00: Legacy RTS. , 10: Static BW. , 11: Dynamic BW.
		uint32 reserved1:8;
	} bitFields;
} RegAggBuilderMuMultUsrsTrainingControl_u;

/*REG_AGG_BUILDER_MU_BAR_BACK_AIR_TIME_DURATION 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muBarBackAirTimeDuration:10;	// Air time duration of SIFS+BAR+SIFS+BACK for MU sequence, [micro seconds]
		uint32 reserved0:22;
	} bitFields;
} RegAggBuilderMuBarBackAirTimeDuration_u;

/*REG_AGG_BUILDER_MAX_SECONDARY_DURATION_PRIMARY_END 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxSecondaryDurationPrimaryEnd:14;	// Maximum PPDU air time duration [us] for the secondary USPs when Primary USP has been completed 
		uint32 reserved0:18;
	} bitFields;
} RegAggBuilderMaxSecondaryDurationPrimaryEnd_u;

/*REG_AGG_BUILDER_MU_GRP_DB_ADDR 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muGrpDbBaseAddr:24;	// MU Group DB base byte address
		uint32 muGrpDbEntry4BSize:8;	// MU Group DB entry size, reolution of 4 Bytes
	} bitFields;
} RegAggBuilderMuGrpDbAddr_u;

/*REG_AGG_BUILDER_FIRST_PD_POINTER_USP0_1 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 firstPdPointerUsp0:16;	// First PD pointer
		uint32 firstPdPointerUsp1:16;	// First PD pointer
	} bitFields;
} RegAggBuilderFirstPdPointerUsp01_u;

/*REG_AGG_BUILDER_FIRST_PD_POINTER_USP2_3 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 firstPdPointerUsp2:16;	// First PD pointer
		uint32 firstPdPointerUsp3:16;	// First PD pointer
	} bitFields;
} RegAggBuilderFirstPdPointerUsp23_u;

/*REG_AGG_BUILDER_MAX_MPDU_SIZE_ESTIMATED_USP0_1 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduSizeEstimatedUsp0:14;	// max MPDU size estimated
		uint32 reserved0:2;
		uint32 maxMpduSizeEstimatedUsp1:14;	// max MPDU size estimated
		uint32 reserved1:2;
	} bitFields;
} RegAggBuilderMaxMpduSizeEstimatedUsp01_u;

/*REG_AGG_BUILDER_MAX_MPDU_SIZE_ESTIMATED_USP2_3 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduSizeEstimatedUsp2:14;	// max MPDU size estimated
		uint32 reserved0:2;
		uint32 maxMpduSizeEstimatedUsp3:14;	// max MPDU size estimated
		uint32 reserved1:2;
	} bitFields;
} RegAggBuilderMaxMpduSizeEstimatedUsp23_u;

/*REG_AGG_BUILDER_PPDU_AIR_TIME_DURATION_ESTIMATED_USP0_1 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ppduAirTimeDurationEstimatedUsp0:14;	// PPDU air time duration estimated
		uint32 reserved0:2;
		uint32 ppduAirTimeDurationEstimatedUsp1:14;	// PPDU air time duration estimated
		uint32 reserved1:2;
	} bitFields;
} RegAggBuilderPpduAirTimeDurationEstimatedUsp01_u;

/*REG_AGG_BUILDER_PPDU_AIR_TIME_DURATION_ESTIMATED_USP2_3 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ppduAirTimeDurationEstimatedUsp2:14;	// PPDU air time duration estimated
		uint32 reserved0:2;
		uint32 ppduAirTimeDurationEstimatedUsp3:14;	// PPDU air time duration estimated
		uint32 reserved1:2;
	} bitFields;
} RegAggBuilderPpduAirTimeDurationEstimatedUsp23_u;

/*REG_AGG_BUILDER_MAX_PSDU_SIZE_ESTIMATED_USP0 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPsduSizeEstimatedUsp0:20;	// max PSDU size estimated
		uint32 reserved0:12;
	} bitFields;
} RegAggBuilderMaxPsduSizeEstimatedUsp0_u;

/*REG_AGG_BUILDER_MAX_PSDU_SIZE_ESTIMATED_USP1 0x1B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPsduSizeEstimatedUsp1:20;	// max PSDU size estimated
		uint32 reserved0:12;
	} bitFields;
} RegAggBuilderMaxPsduSizeEstimatedUsp1_u;

/*REG_AGG_BUILDER_MAX_PSDU_SIZE_ESTIMATED_USP2 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPsduSizeEstimatedUsp2:20;	// max PSDU size estimated
		uint32 reserved0:12;
	} bitFields;
} RegAggBuilderMaxPsduSizeEstimatedUsp2_u;

/*REG_AGG_BUILDER_MAX_PSDU_SIZE_ESTIMATED_USP3 0x1BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPsduSizeEstimatedUsp3:20;	// max PSDU size estimated
		uint32 reserved0:12;
	} bitFields;
} RegAggBuilderMaxPsduSizeEstimatedUsp3_u;

/*REG_AGG_BUILDER_MIN_MPDU_SIZE_USP0_1 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minMpduSizeUsp0:12;	// min MPDU size
		uint32 reserved0:4;
		uint32 minMpduSizeUsp1:12;	// min MPDU size
		uint32 reserved1:4;
	} bitFields;
} RegAggBuilderMinMpduSizeUsp01_u;

/*REG_AGG_BUILDER_MIN_MPDU_SIZE_USP2_3 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minMpduSizeUsp2:12;	// min MPDU size
		uint32 reserved0:4;
		uint32 minMpduSizeUsp3:12;	// min MPDU size
		uint32 reserved1:4;
	} bitFields;
} RegAggBuilderMinMpduSizeUsp23_u;

/*REG_AGG_BUILDER_TX_SEL_RESULT 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelMuSuN:1;	// Tx selector SU/MU selection, 1 - MU, 0 - SU
		uint32 txSelMuPrimaryUsp:2;	// Tx selector Primary USP
		uint32 txSelMuGrpTxMode:2;	// Tx selector MU Group Tx mode
		uint32 txSelUspValid:4;	// Tx selector USP valid results
		uint32 txSelMuGrpEntry:7;	// Tx selector MU Group entry index
		uint32 txSelUspLock:4;	// Tx selector USP lock
		uint32 txSelMuGroupLock:1;	// Tx selector MU group lock
		uint32 aggSwitchMuToSu:1;	// AGG modified Tx selector results from MU to SU
		uint32 reserved0:10;
	} bitFields;
} RegAggBuilderTxSelResult_u;

/*REG_AGG_BUILDER_TX_SEL_USP0_RESULT1 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp0TxqTidId:3;	// Tx selector usp0 TXQ TID id
		uint32 txSelUsp0TxqStaId:8;	// Tx selector usp0 TXQ STA id
		uint32 txSelUsp0TxqVapId:4;	// Tx selector usp0 TXQ VAP id
		uint32 txSelUsp0TxqQueueId:3;	// Tx selector usp0 TXQ Queue id
		uint32 txSelUsp0AcId:2;	// Tx selector usp0 AC_ID
		uint32 txSelUsp0SuBfSeq:2;	// Tx selector usp0 SU BF sequence
		uint32 reserved0:10;
	} bitFields;
} RegAggBuilderTxSelUsp0Result1_u;

/*REG_AGG_BUILDER_TX_SEL_USP0_RESULT2 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp0BitmapStatus:32;	// Tx selector usp0 BitMAP Status
	} bitFields;
} RegAggBuilderTxSelUsp0Result2_u;

/*REG_AGG_BUILDER_TX_SEL_USP1_RESULT1 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp1TxqTidId:3;	// Tx selector usp1 TXQ TID id
		uint32 txSelUsp1TxqStaId:8;	// Tx selector usp1 TXQ STA id
		uint32 txSelUsp1TxqVapId:4;	// Tx selector usp1 TXQ VAP id
		uint32 txSelUsp1TxqQueueId:3;	// Tx selector usp1 TXQ Queue id
		uint32 txSelUsp1AcId:2;	// Tx selector usp1 AC_ID
		uint32 txSelUsp1SuBfSeq:2;	// Tx selector usp1 SU BF sequence
		uint32 reserved0:10;
	} bitFields;
} RegAggBuilderTxSelUsp1Result1_u;

/*REG_AGG_BUILDER_TX_SEL_USP1_RESULT2 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp1BitmapStatus:32;	// Tx selector usp1 BitMAP Status
	} bitFields;
} RegAggBuilderTxSelUsp1Result2_u;

/*REG_AGG_BUILDER_TX_SEL_USP2_RESULT1 0x1DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp2TxqTidId:3;	// Tx selector usp2 TXQ TID id
		uint32 txSelUsp2TxqStaId:8;	// Tx selector usp2 TXQ STA id
		uint32 txSelUsp2TxqVapId:4;	// Tx selector usp2 TXQ VAP id
		uint32 txSelUsp2TxqQueueId:3;	// Tx selector usp2 TXQ Queue id
		uint32 txSelUsp2AcId:2;	// Tx selector usp2 AC_ID
		uint32 txSelUsp2SuBfSeq:2;	// Tx selector usp2 SU BF sequence
		uint32 reserved0:10;
	} bitFields;
} RegAggBuilderTxSelUsp2Result1_u;

/*REG_AGG_BUILDER_TX_SEL_USP2_RESULT2 0x1F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp2BitmapStatus:32;	// Tx selector usp2 BitMAP Status
	} bitFields;
} RegAggBuilderTxSelUsp2Result2_u;

/*REG_AGG_BUILDER_TX_SEL_USP3_RESULT1 0x1F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp3TxqTidId:3;	// Tx selector usp3 TXQ TID id
		uint32 txSelUsp3TxqStaId:8;	// Tx selector usp3 TXQ STA id
		uint32 txSelUsp3TxqVapId:4;	// Tx selector usp3 TXQ VAP id
		uint32 txSelUsp3TxqQueueId:3;	// Tx selector usp3 TXQ Queue id
		uint32 txSelUsp3AcId:2;	// Tx selector usp3 AC_ID
		uint32 txSelUsp3SuBfSeq:2;	// Tx selector usp3 SU BF sequence
		uint32 reserved0:10;
	} bitFields;
} RegAggBuilderTxSelUsp3Result1_u;

/*REG_AGG_BUILDER_TX_SEL_USP3_RESULT2 0x1F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp3BitmapStatus:32;	// Tx selector usp3 BitMAP Status
	} bitFields;
} RegAggBuilderTxSelUsp3Result2_u;

/*REG_AGG_BUILDER_BW_LIMIT_VAP 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bwLimitVap:32;	// BW limit per VAP, 2bits for each VAP. , 00: 20MHz , 01: 40MHz , 10: 80MHz , 11: 160MHz 
	} bitFields;
} RegAggBuilderBwLimitVap_u;

/*REG_AGG_BUILDER_MU_USP_SECONDARY_NO_TX_REASON 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUsp0SecondaryNoTxReason:4;	// MU USP Secondary no Tx reason: , 8- PD type not data,  , 9- Cannot aggregate,  , 10- Max Nsts reached
		uint32 muUsp1SecondaryNoTxReason:4;	// MU USP Secondary no Tx reason: , 8- PD type not data,  , 9- Cannot aggregate,  , 10- Max Nsts reached
		uint32 muUsp2SecondaryNoTxReason:4;	// MU USP Secondary no Tx reason: , 8- PD type not data,  , 9- Cannot aggregate,  , 10- Max Nsts reached
		uint32 muUsp3SecondaryNoTxReason:4;	// MU USP Secondary no Tx reason: , 8- PD type not data,  , 9- Cannot aggregate,  , 10- Max Nsts reached
		uint32 reserved0:16;
	} bitFields;
} RegAggBuilderMuUspSecondaryNoTxReason_u;

/*REG_AGG_BUILDER_MAX_PSDU_TX_TIME_LIMIT 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPsduTxTimeLimit:13;	// Max PSDU Tx Time limit, resolution of 2[us]
		uint32 reserved0:19;
	} bitFields;
} RegAggBuilderMaxPsduTxTimeLimit_u;

/*REG_AGG_BUILDER_AGG_BUILDER_SPARE 0x3FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggBuilderSpare:32;	// Spare registers
	} bitFields;
} RegAggBuilderAggBuilderSpare_u;



#endif // _AGG_BUILDER_REGS_H_
