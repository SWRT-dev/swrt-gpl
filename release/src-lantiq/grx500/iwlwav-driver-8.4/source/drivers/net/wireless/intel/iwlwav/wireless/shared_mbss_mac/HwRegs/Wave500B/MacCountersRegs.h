
/***********************************************************************************
File:				MacCountersRegs.h
Module:				macCounters
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_COUNTERS_REGS_H_
#define _MAC_COUNTERS_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_COUNTERS_BASE_ADDRESS                             MEMORY_MAP_UNIT_56_BASE_ADDRESS
#define	REG_MAC_COUNTERS_CLEAR_STAT_STRB                    (MAC_COUNTERS_BASE_ADDRESS + 0x0)
#define	REG_MAC_COUNTERS_COUNTERS_ENABLE                    (MAC_COUNTERS_BASE_ADDRESS + 0x4)
#define	REG_MAC_COUNTERS_TXH_AC0_CTR                        (MAC_COUNTERS_BASE_ADDRESS + 0x10)
#define	REG_MAC_COUNTERS_TXH_AC1_CTR                        (MAC_COUNTERS_BASE_ADDRESS + 0x14)
#define	REG_MAC_COUNTERS_TXH_AC2_CTR                        (MAC_COUNTERS_BASE_ADDRESS + 0x18)
#define	REG_MAC_COUNTERS_TXH_AC3_CTR                        (MAC_COUNTERS_BASE_ADDRESS + 0x1C)
#define	REG_MAC_COUNTERS_TXH_AC5_CTR                        (MAC_COUNTERS_BASE_ADDRESS + 0x20)
#define	REG_MAC_COUNTERS_TXH_AC6_CTR                        (MAC_COUNTERS_BASE_ADDRESS + 0x28)
#define	REG_MAC_COUNTERS_TXH_AC7_CTR                        (MAC_COUNTERS_BASE_ADDRESS + 0x2C)
#define	REG_MAC_COUNTERS_AGG_START_CTR                      (MAC_COUNTERS_BASE_ADDRESS + 0x100)
#define	REG_MAC_COUNTERS_AGG_END_CTR                        (MAC_COUNTERS_BASE_ADDRESS + 0x104)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE0_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x108)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE1_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x10C)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE2_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x110)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE3_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x114)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE4_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x118)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE5_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x11C)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE6_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x120)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE7_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x124)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE8_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x128)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE9_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x12C)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE10_CTR                  (MAC_COUNTERS_BASE_ADDRESS + 0x130)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE11_CTR                  (MAC_COUNTERS_BASE_ADDRESS + 0x134)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE12_CTR                  (MAC_COUNTERS_BASE_ADDRESS + 0x138)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE13_CTR                  (MAC_COUNTERS_BASE_ADDRESS + 0x13C)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE14_CTR                  (MAC_COUNTERS_BASE_ADDRESS + 0x140)
#define	REG_MAC_COUNTERS_AGG_PD_TYPE15_CTR                  (MAC_COUNTERS_BASE_ADDRESS + 0x144)
#define	REG_MAC_COUNTERS_AGG_Q_STA_TID0_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x148)
#define	REG_MAC_COUNTERS_AGG_Q_STA_TID1_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x14C)
#define	REG_MAC_COUNTERS_AGG_Q_STA_TID2_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x150)
#define	REG_MAC_COUNTERS_AGG_Q_STA_TID3_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x154)
#define	REG_MAC_COUNTERS_AGG_Q_STA_TID4_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x158)
#define	REG_MAC_COUNTERS_AGG_Q_STA_TID5_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x15C)
#define	REG_MAC_COUNTERS_AGG_Q_STA_TID6_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x160)
#define	REG_MAC_COUNTERS_AGG_Q_STA_TID7_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x164)
#define	REG_MAC_COUNTERS_AGG_Q_VAP_TID0_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x168)
#define	REG_MAC_COUNTERS_AGG_Q_VAP_TID1_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x16C)
#define	REG_MAC_COUNTERS_AGG_Q_VAP_TID2_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x170)
#define	REG_MAC_COUNTERS_AGG_Q_VAP_TID3_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x174)
#define	REG_MAC_COUNTERS_AGG_Q_VAP_TID4_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x178)
#define	REG_MAC_COUNTERS_AGG_Q_VAP_TID5_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x17C)
#define	REG_MAC_COUNTERS_AGG_Q_VAP_TID6_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x180)
#define	REG_MAC_COUNTERS_AGG_Q_VAP_TID7_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x184)
#define	REG_MAC_COUNTERS_AGG_Q_GLOBAL_AC0_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x188)
#define	REG_MAC_COUNTERS_AGG_Q_GLOBAL_AC1_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x18C)
#define	REG_MAC_COUNTERS_AGG_Q_GLOBAL_AC2_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x190)
#define	REG_MAC_COUNTERS_AGG_Q_GLOBAL_AC3_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x194)
#define	REG_MAC_COUNTERS_AGG_Q_GPLP_CTR                     (MAC_COUNTERS_BASE_ADDRESS + 0x198)
#define	REG_MAC_COUNTERS_AGG_Q_GPHP_CTR                     (MAC_COUNTERS_BASE_ADDRESS + 0x19C)
#define	REG_MAC_COUNTERS_AGG_Q_BEACON_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x1A0)
#define	REG_MAC_COUNTERS_BAA_Q_STA_TID0_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x200)
#define	REG_MAC_COUNTERS_BAA_Q_STA_TID1_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x204)
#define	REG_MAC_COUNTERS_BAA_Q_STA_TID2_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x208)
#define	REG_MAC_COUNTERS_BAA_Q_STA_TID3_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x20C)
#define	REG_MAC_COUNTERS_BAA_Q_STA_TID4_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x210)
#define	REG_MAC_COUNTERS_BAA_Q_STA_TID5_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x214)
#define	REG_MAC_COUNTERS_BAA_Q_STA_TID6_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x218)
#define	REG_MAC_COUNTERS_BAA_Q_STA_TID7_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x21C)
#define	REG_MAC_COUNTERS_BAA_Q_VAP_TID0_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x220)
#define	REG_MAC_COUNTERS_BAA_Q_VAP_TID1_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x224)
#define	REG_MAC_COUNTERS_BAA_Q_VAP_TID2_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x228)
#define	REG_MAC_COUNTERS_BAA_Q_VAP_TID3_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x22C)
#define	REG_MAC_COUNTERS_BAA_Q_VAP_TID4_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x230)
#define	REG_MAC_COUNTERS_BAA_Q_VAP_TID5_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x234)
#define	REG_MAC_COUNTERS_BAA_Q_VAP_TID6_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x238)
#define	REG_MAC_COUNTERS_BAA_Q_VAP_TID7_CTR                 (MAC_COUNTERS_BASE_ADDRESS + 0x23C)
#define	REG_MAC_COUNTERS_BAA_Q_GLOBAL_AC0_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x240)
#define	REG_MAC_COUNTERS_BAA_Q_GLOBAL_AC1_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x244)
#define	REG_MAC_COUNTERS_BAA_Q_GLOBAL_AC2_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x248)
#define	REG_MAC_COUNTERS_BAA_Q_GLOBAL_AC3_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x24C)
#define	REG_MAC_COUNTERS_BAA_Q_GPLP_CTR                     (MAC_COUNTERS_BASE_ADDRESS + 0x250)
#define	REG_MAC_COUNTERS_BAA_Q_GPHP_CTR                     (MAC_COUNTERS_BASE_ADDRESS + 0x254)
#define	REG_MAC_COUNTERS_BAA_Q_BEACON_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x258)
#define	REG_MAC_COUNTERS_BAA_START_COUNTER                  (MAC_COUNTERS_BASE_ADDRESS + 0x25C)
#define	REG_MAC_COUNTERS_BAA_END_COUNTER                    (MAC_COUNTERS_BASE_ADDRESS + 0x260)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE0_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x264)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE1_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x268)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE2_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x26C)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE3_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x270)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE4_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x274)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE5_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x278)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE6_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x27C)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE7_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x280)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE8_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x284)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE9_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x288)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE10_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x28C)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE11_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x290)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE12_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x294)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE13_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x298)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE14_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x29C)
#define	REG_MAC_COUNTERS_BAA_PD_TYPE15_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x2A0)
#define	REG_MAC_COUNTERS_SUCCESS_BAA_TID0_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2A4)
#define	REG_MAC_COUNTERS_SUCCESS_BAA_TID1_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2A8)
#define	REG_MAC_COUNTERS_SUCCESS_BAA_TID2_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2AC)
#define	REG_MAC_COUNTERS_SUCCESS_BAA_TID3_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2B0)
#define	REG_MAC_COUNTERS_SUCCESS_BAA_TID4_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2B4)
#define	REG_MAC_COUNTERS_SUCCESS_BAA_TID5_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2B8)
#define	REG_MAC_COUNTERS_SUCCESS_BAA_TID6_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2BC)
#define	REG_MAC_COUNTERS_SUCCESS_BAA_TID7_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2C0)
#define	REG_MAC_COUNTERS_RETRY_BAA_TID0_COUNTER             (MAC_COUNTERS_BASE_ADDRESS + 0x2C4)
#define	REG_MAC_COUNTERS_RETRY_BAA_TID1_COUNTER             (MAC_COUNTERS_BASE_ADDRESS + 0x2C8)
#define	REG_MAC_COUNTERS_RETRY_BAA_TID2_COUNTER             (MAC_COUNTERS_BASE_ADDRESS + 0x2CC)
#define	REG_MAC_COUNTERS_RETRY_BAA_TID3_COUNTER             (MAC_COUNTERS_BASE_ADDRESS + 0x2D0)
#define	REG_MAC_COUNTERS_RETRY_BAA_TID4_COUNTER             (MAC_COUNTERS_BASE_ADDRESS + 0x2D4)
#define	REG_MAC_COUNTERS_RETRY_BAA_TID5_COUNTER             (MAC_COUNTERS_BASE_ADDRESS + 0x2D8)
#define	REG_MAC_COUNTERS_RETRY_BAA_TID6_COUNTER             (MAC_COUNTERS_BASE_ADDRESS + 0x2DC)
#define	REG_MAC_COUNTERS_RETRY_BAA_TID7_COUNTER             (MAC_COUNTERS_BASE_ADDRESS + 0x2E0)
#define	REG_MAC_COUNTERS_DISCARD_BAA_TID0_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2E4)
#define	REG_MAC_COUNTERS_DISCARD_BAA_TID1_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2E8)
#define	REG_MAC_COUNTERS_DISCARD_BAA_TID2_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2EC)
#define	REG_MAC_COUNTERS_DISCARD_BAA_TID3_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2F0)
#define	REG_MAC_COUNTERS_DISCARD_BAA_TID4_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2F4)
#define	REG_MAC_COUNTERS_DISCARD_BAA_TID5_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2F8)
#define	REG_MAC_COUNTERS_DISCARD_BAA_TID6_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x2FC)
#define	REG_MAC_COUNTERS_DISCARD_BAA_TID7_COUNTER           (MAC_COUNTERS_BASE_ADDRESS + 0x300)
#define	REG_MAC_COUNTERS_ACK_BAA_TID0_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x304)
#define	REG_MAC_COUNTERS_ACK_BAA_TID1_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x308)
#define	REG_MAC_COUNTERS_ACK_BAA_TID2_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x30C)
#define	REG_MAC_COUNTERS_ACK_BAA_TID3_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x310)
#define	REG_MAC_COUNTERS_ACK_BAA_TID4_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x314)
#define	REG_MAC_COUNTERS_ACK_BAA_TID5_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x318)
#define	REG_MAC_COUNTERS_ACK_BAA_TID6_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x31C)
#define	REG_MAC_COUNTERS_ACK_BAA_TID7_COUNTER               (MAC_COUNTERS_BASE_ADDRESS + 0x320)
#define	REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID0_COUNTER         (MAC_COUNTERS_BASE_ADDRESS + 0x324)
#define	REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID1_COUNTER         (MAC_COUNTERS_BASE_ADDRESS + 0x328)
#define	REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID2_COUNTER         (MAC_COUNTERS_BASE_ADDRESS + 0x32C)
#define	REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID3_COUNTER         (MAC_COUNTERS_BASE_ADDRESS + 0x330)
#define	REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID4_COUNTER         (MAC_COUNTERS_BASE_ADDRESS + 0x334)
#define	REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID5_COUNTER         (MAC_COUNTERS_BASE_ADDRESS + 0x338)
#define	REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID6_COUNTER         (MAC_COUNTERS_BASE_ADDRESS + 0x33C)
#define	REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID7_COUNTER         (MAC_COUNTERS_BASE_ADDRESS + 0x340)
#define	REG_MAC_COUNTERS_NACK_BAA_TID0_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x344)
#define	REG_MAC_COUNTERS_NACK_BAA_TID1_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x348)
#define	REG_MAC_COUNTERS_NACK_BAA_TID2_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x34C)
#define	REG_MAC_COUNTERS_NACK_BAA_TID3_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x350)
#define	REG_MAC_COUNTERS_NACK_BAA_TID4_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x354)
#define	REG_MAC_COUNTERS_NACK_BAA_TID5_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x358)
#define	REG_MAC_COUNTERS_NACK_BAA_TID6_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x35C)
#define	REG_MAC_COUNTERS_NACK_BAA_TID7_COUNTER              (MAC_COUNTERS_BASE_ADDRESS + 0x360)
#define	REG_MAC_COUNTERS_TX_SEL_START_CTR                   (MAC_COUNTERS_BASE_ADDRESS + 0x400)
#define	REG_MAC_COUNTERS_TX_SEL_END_CTR                     (MAC_COUNTERS_BASE_ADDRESS + 0x404)
#define	REG_MAC_COUNTERS_TX_SEL_STA_TID0_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x408)
#define	REG_MAC_COUNTERS_TX_SEL_STA_TID1_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x40C)
#define	REG_MAC_COUNTERS_TX_SEL_STA_TID2_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x410)
#define	REG_MAC_COUNTERS_TX_SEL_STA_TID3_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x414)
#define	REG_MAC_COUNTERS_TX_SEL_STA_TID4_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x418)
#define	REG_MAC_COUNTERS_TX_SEL_STA_TID5_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x41C)
#define	REG_MAC_COUNTERS_TX_SEL_STA_TID6_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x420)
#define	REG_MAC_COUNTERS_TX_SEL_STA_TID7_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x424)
#define	REG_MAC_COUNTERS_TX_SEL_VAP_TID0_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x428)
#define	REG_MAC_COUNTERS_TX_SEL_VAP_TID1_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x42C)
#define	REG_MAC_COUNTERS_TX_SEL_VAP_TID2_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x430)
#define	REG_MAC_COUNTERS_TX_SEL_VAP_TID3_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x434)
#define	REG_MAC_COUNTERS_TX_SEL_VAP_TID4_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x438)
#define	REG_MAC_COUNTERS_TX_SEL_VAP_TID5_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x43C)
#define	REG_MAC_COUNTERS_TX_SEL_VAP_TID6_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x440)
#define	REG_MAC_COUNTERS_TX_SEL_VAP_TID7_CTR                (MAC_COUNTERS_BASE_ADDRESS + 0x444)
#define	REG_MAC_COUNTERS_TX_SEL_GLOBAL_BE_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x448)
#define	REG_MAC_COUNTERS_TX_SEL_GLOBAL_BK_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x44C)
#define	REG_MAC_COUNTERS_TX_SEL_GLOBAL_VI_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x450)
#define	REG_MAC_COUNTERS_TX_SEL_GLOBAL_VO_CTR               (MAC_COUNTERS_BASE_ADDRESS + 0x454)
#define	REG_MAC_COUNTERS_TX_SEL_GPLP_CTR                    (MAC_COUNTERS_BASE_ADDRESS + 0x458)
#define	REG_MAC_COUNTERS_TX_SEL_GPHP_CTR                    (MAC_COUNTERS_BASE_ADDRESS + 0x45C)
#define	REG_MAC_COUNTERS_TX_SEL_BEACON_CTR                  (MAC_COUNTERS_BASE_ADDRESS + 0x460)
#define	REG_MAC_COUNTERS_TX_IN_HOST_IF_HD_COUNT_CTR         (MAC_COUNTERS_BASE_ADDRESS + 0x464)
#define	REG_MAC_COUNTERS_TX_OUT_HOST_IF_HD_COUNT_CTR        (MAC_COUNTERS_BASE_ADDRESS + 0x468)
#define	REG_MAC_COUNTERS_RX_OUT_HOST_IF_HD_COUNT_CTR        (MAC_COUNTERS_BASE_ADDRESS + 0x46C)
#define	REG_MAC_COUNTERS_TX_IN_HOST_IF_BUFFER_COUNT_CTR     (MAC_COUNTERS_BASE_ADDRESS + 0x470)
#define	REG_MAC_COUNTERS_TX_OUT_HOST_IF_BUFFER_COUNT_CTR    (MAC_COUNTERS_BASE_ADDRESS + 0x474)
#define	REG_MAC_COUNTERS_RX_OUT_HOST_IF_BUFFER_COUNT_CTR    (MAC_COUNTERS_BASE_ADDRESS + 0x478)
#define	REG_MAC_COUNTERS_LOGGER_EVENT_ENABLE                (MAC_COUNTERS_BASE_ADDRESS + 0x500)
#define	REG_MAC_COUNTERS_SENDER_EVENT                       (MAC_COUNTERS_BASE_ADDRESS + 0x504)
#define	REG_MAC_COUNTERS_ENTRIES_IN_LOGGER                  (MAC_COUNTERS_BASE_ADDRESS + 0x508)
#define	REG_MAC_COUNTERS_LOGGER_RESET                       (MAC_COUNTERS_BASE_ADDRESS + 0x50C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_COUNTERS_CLEAR_STAT_STRB 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearStatStrb:1;	// Clear statistics
		uint32 reserved0:31;
	} bitFields;
} RegMacCountersClearStatStrb_u;

/*REG_MAC_COUNTERS_COUNTERS_ENABLE 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhCtrEnable:1;	// TxH counters enable
		uint32 aggCtrEnable:1;	// Aggregator counters enable
		uint32 baaCtrEnable:1;	// BAA counters enable
		uint32 txSelCtrEnable:1;	// TX Selector counters enable
		uint32 hostIfAccCtrEnable:1;	// Host IF Acc counters enable
		uint32 reserved0:27;
	} bitFields;
} RegMacCountersCountersEnable_u;

/*REG_MAC_COUNTERS_TXH_AC0_CTR 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhAc0Ctr:32;	// txh_ac0_ctr
	} bitFields;
} RegMacCountersTxhAc0Ctr_u;

/*REG_MAC_COUNTERS_TXH_AC1_CTR 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhAc1Ctr:32;	// txh_ac1_ctr
	} bitFields;
} RegMacCountersTxhAc1Ctr_u;

/*REG_MAC_COUNTERS_TXH_AC2_CTR 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhAc2Ctr:32;	// txh_ac2_ctr
	} bitFields;
} RegMacCountersTxhAc2Ctr_u;

/*REG_MAC_COUNTERS_TXH_AC3_CTR 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhAc3Ctr:32;	// txh_ac3_ctr
	} bitFields;
} RegMacCountersTxhAc3Ctr_u;

/*REG_MAC_COUNTERS_TXH_AC5_CTR 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhAc5Ctr:32;	// txh_ac5_ctr
	} bitFields;
} RegMacCountersTxhAc5Ctr_u;

/*REG_MAC_COUNTERS_TXH_AC6_CTR 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhAc6Ctr:32;	// txh_ac6_ctr
	} bitFields;
} RegMacCountersTxhAc6Ctr_u;

/*REG_MAC_COUNTERS_TXH_AC7_CTR 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhAc7Ctr:32;	// txh_ac7_ctr
	} bitFields;
} RegMacCountersTxhAc7Ctr_u;

/*REG_MAC_COUNTERS_AGG_START_CTR 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggStartCtr:32;	// agg_start_ctr
	} bitFields;
} RegMacCountersAggStartCtr_u;

/*REG_MAC_COUNTERS_AGG_END_CTR 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggEndCtr:32;	// agg_end_ctr
	} bitFields;
} RegMacCountersAggEndCtr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE0_CTR 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType0Ctr:32;	// agg_pd_type0_ctr
	} bitFields;
} RegMacCountersAggPdType0Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE1_CTR 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType1Ctr:32;	// agg_pd_type1_ctr
	} bitFields;
} RegMacCountersAggPdType1Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE2_CTR 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType2Ctr:32;	// agg_pd_type2_ctr
	} bitFields;
} RegMacCountersAggPdType2Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE3_CTR 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType3Ctr:32;	// agg_pd_type3_ctr
	} bitFields;
} RegMacCountersAggPdType3Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE4_CTR 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType4Ctr:32;	// agg_pd_type4_ctr
	} bitFields;
} RegMacCountersAggPdType4Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE5_CTR 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType5Ctr:32;	// agg_pd_type5_ctr
	} bitFields;
} RegMacCountersAggPdType5Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE6_CTR 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType6Ctr:32;	// agg_pd_type6_ctr
	} bitFields;
} RegMacCountersAggPdType6Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE7_CTR 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType7Ctr:32;	// agg_pd_type7_ctr
	} bitFields;
} RegMacCountersAggPdType7Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE8_CTR 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType8Ctr:32;	// agg_pd_type8_ctr
	} bitFields;
} RegMacCountersAggPdType8Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE9_CTR 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType9Ctr:32;	// agg_pd_type9_ctr
	} bitFields;
} RegMacCountersAggPdType9Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE10_CTR 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType10Ctr:32;	// agg_pd_type10_ctr
	} bitFields;
} RegMacCountersAggPdType10Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE11_CTR 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType11Ctr:32;	// agg_pd_type11_ctr
	} bitFields;
} RegMacCountersAggPdType11Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE12_CTR 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType12Ctr:32;	// agg_pd_type12_ctr
	} bitFields;
} RegMacCountersAggPdType12Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE13_CTR 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType13Ctr:32;	// agg_pd_type13_ctr
	} bitFields;
} RegMacCountersAggPdType13Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE14_CTR 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType14Ctr:32;	// agg_pd_type14_ctr
	} bitFields;
} RegMacCountersAggPdType14Ctr_u;

/*REG_MAC_COUNTERS_AGG_PD_TYPE15_CTR 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggPdType15Ctr:32;	// agg_pd_type15_ctr
	} bitFields;
} RegMacCountersAggPdType15Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_STA_TID0_CTR 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQStaTid0Ctr:32;	// agg_q_sta_tid0_ctr
	} bitFields;
} RegMacCountersAggQStaTid0Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_STA_TID1_CTR 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQStaTid1Ctr:32;	// agg_q_sta_tid1_ctr
	} bitFields;
} RegMacCountersAggQStaTid1Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_STA_TID2_CTR 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQStaTid2Ctr:32;	// agg_q_sta_tid2_ctr
	} bitFields;
} RegMacCountersAggQStaTid2Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_STA_TID3_CTR 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQStaTid3Ctr:32;	// agg_q_sta_tid3_ctr
	} bitFields;
} RegMacCountersAggQStaTid3Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_STA_TID4_CTR 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQStaTid4Ctr:32;	// agg_q_sta_tid4_ctr
	} bitFields;
} RegMacCountersAggQStaTid4Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_STA_TID5_CTR 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQStaTid5Ctr:32;	// agg_q_sta_tid5_ctr
	} bitFields;
} RegMacCountersAggQStaTid5Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_STA_TID6_CTR 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQStaTid6Ctr:32;	// agg_q_sta_tid6_ctr
	} bitFields;
} RegMacCountersAggQStaTid6Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_STA_TID7_CTR 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQStaTid7Ctr:32;	// agg_q_sta_tid7_ctr
	} bitFields;
} RegMacCountersAggQStaTid7Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_VAP_TID0_CTR 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQVapTid0Ctr:32;	// agg_q_vap_tid0_ctr
	} bitFields;
} RegMacCountersAggQVapTid0Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_VAP_TID1_CTR 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQVapTid1Ctr:32;	// agg_q_vap_tid1_ctr
	} bitFields;
} RegMacCountersAggQVapTid1Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_VAP_TID2_CTR 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQVapTid2Ctr:32;	// agg_q_vap_tid2_ctr
	} bitFields;
} RegMacCountersAggQVapTid2Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_VAP_TID3_CTR 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQVapTid3Ctr:32;	// agg_q_vap_tid3_ctr
	} bitFields;
} RegMacCountersAggQVapTid3Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_VAP_TID4_CTR 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQVapTid4Ctr:32;	// agg_q_vap_tid4_ctr
	} bitFields;
} RegMacCountersAggQVapTid4Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_VAP_TID5_CTR 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQVapTid5Ctr:32;	// agg_q_vap_tid5_ctr
	} bitFields;
} RegMacCountersAggQVapTid5Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_VAP_TID6_CTR 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQVapTid6Ctr:32;	// agg_q_vap_tid6_ctr
	} bitFields;
} RegMacCountersAggQVapTid6Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_VAP_TID7_CTR 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQVapTid7Ctr:32;	// agg_q_vap_tid7_ctr
	} bitFields;
} RegMacCountersAggQVapTid7Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_GLOBAL_AC0_CTR 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQGlobalAc0Ctr:32;	// agg_q_global_ac0_ctr
	} bitFields;
} RegMacCountersAggQGlobalAc0Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_GLOBAL_AC1_CTR 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQGlobalAc1Ctr:32;	// agg_q_global_ac1_ctr
	} bitFields;
} RegMacCountersAggQGlobalAc1Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_GLOBAL_AC2_CTR 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQGlobalAc2Ctr:32;	// agg_q_global_ac2_ctr
	} bitFields;
} RegMacCountersAggQGlobalAc2Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_GLOBAL_AC3_CTR 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQGlobalAc3Ctr:32;	// agg_q_global_ac3_ctr
	} bitFields;
} RegMacCountersAggQGlobalAc3Ctr_u;

/*REG_MAC_COUNTERS_AGG_Q_GPLP_CTR 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQGplpCtr:32;	// agg_q_gplp_ctr
	} bitFields;
} RegMacCountersAggQGplpCtr_u;

/*REG_MAC_COUNTERS_AGG_Q_GPHP_CTR 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQGphpCtr:32;	// agg_q_gphp_ctr
	} bitFields;
} RegMacCountersAggQGphpCtr_u;

/*REG_MAC_COUNTERS_AGG_Q_BEACON_CTR 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggQBeaconCtr:32;	// agg_q_beacon_ctr
	} bitFields;
} RegMacCountersAggQBeaconCtr_u;

/*REG_MAC_COUNTERS_BAA_Q_STA_TID0_CTR 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQStaTid0Ctr:32;	// baa_q_sta_tid0_ctr
	} bitFields;
} RegMacCountersBaaQStaTid0Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_STA_TID1_CTR 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQStaTid1Ctr:32;	// baa_q_sta_tid1_ctr
	} bitFields;
} RegMacCountersBaaQStaTid1Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_STA_TID2_CTR 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQStaTid2Ctr:32;	// baa_q_sta_tid2_ctr
	} bitFields;
} RegMacCountersBaaQStaTid2Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_STA_TID3_CTR 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQStaTid3Ctr:32;	// baa_q_sta_tid3_ctr
	} bitFields;
} RegMacCountersBaaQStaTid3Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_STA_TID4_CTR 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQStaTid4Ctr:32;	// baa_q_sta_tid4_ctr
	} bitFields;
} RegMacCountersBaaQStaTid4Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_STA_TID5_CTR 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQStaTid5Ctr:32;	// baa_q_sta_tid5_ctr
	} bitFields;
} RegMacCountersBaaQStaTid5Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_STA_TID6_CTR 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQStaTid6Ctr:32;	// baa_q_sta_tid6_ctr
	} bitFields;
} RegMacCountersBaaQStaTid6Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_STA_TID7_CTR 0x21C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQStaTid7Ctr:32;	// baa_q_sta_tid7_ctr
	} bitFields;
} RegMacCountersBaaQStaTid7Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_VAP_TID0_CTR 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQVapTid0Ctr:32;	// baa_q_vap_tid0_ctr
	} bitFields;
} RegMacCountersBaaQVapTid0Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_VAP_TID1_CTR 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQVapTid1Ctr:32;	// baa_q_vap_tid1_ctr
	} bitFields;
} RegMacCountersBaaQVapTid1Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_VAP_TID2_CTR 0x228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQVapTid2Ctr:32;	// baa_q_vap_tid2_ctr
	} bitFields;
} RegMacCountersBaaQVapTid2Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_VAP_TID3_CTR 0x22C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQVapTid3Ctr:32;	// baa_q_vap_tid3_ctr
	} bitFields;
} RegMacCountersBaaQVapTid3Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_VAP_TID4_CTR 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQVapTid4Ctr:32;	// baa_q_vap_tid4_ctr
	} bitFields;
} RegMacCountersBaaQVapTid4Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_VAP_TID5_CTR 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQVapTid5Ctr:32;	// baa_q_vap_tid5_ctr
	} bitFields;
} RegMacCountersBaaQVapTid5Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_VAP_TID6_CTR 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQVapTid6Ctr:32;	// baa_q_vap_tid6_ctr
	} bitFields;
} RegMacCountersBaaQVapTid6Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_VAP_TID7_CTR 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQVapTid7Ctr:32;	// baa_q_vap_tid7_ctr
	} bitFields;
} RegMacCountersBaaQVapTid7Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_GLOBAL_AC0_CTR 0x240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQGlobalAc0Ctr:32;	// baa_q_global_ac0_ctr
	} bitFields;
} RegMacCountersBaaQGlobalAc0Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_GLOBAL_AC1_CTR 0x244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQGlobalAc1Ctr:32;	// baa_q_global_ac1_ctr
	} bitFields;
} RegMacCountersBaaQGlobalAc1Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_GLOBAL_AC2_CTR 0x248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQGlobalAc2Ctr:32;	// baa_q_global_ac2_ctr
	} bitFields;
} RegMacCountersBaaQGlobalAc2Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_GLOBAL_AC3_CTR 0x24C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQGlobalAc3Ctr:32;	// baa_q_global_ac3_ctr
	} bitFields;
} RegMacCountersBaaQGlobalAc3Ctr_u;

/*REG_MAC_COUNTERS_BAA_Q_GPLP_CTR 0x250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQGplpCtr:32;	// baa_q_gplp_ctr
	} bitFields;
} RegMacCountersBaaQGplpCtr_u;

/*REG_MAC_COUNTERS_BAA_Q_GPHP_CTR 0x254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQGphpCtr:32;	// baa_q_gphp_ctr
	} bitFields;
} RegMacCountersBaaQGphpCtr_u;

/*REG_MAC_COUNTERS_BAA_Q_BEACON_CTR 0x258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaQBeaconCtr:32;	// baa_q_beacon_ctr
	} bitFields;
} RegMacCountersBaaQBeaconCtr_u;

/*REG_MAC_COUNTERS_BAA_START_COUNTER 0x25C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaStartCounter:32;	// baa_start_counter
	} bitFields;
} RegMacCountersBaaStartCounter_u;

/*REG_MAC_COUNTERS_BAA_END_COUNTER 0x260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaEndCounter:32;	// baa_end_counter
	} bitFields;
} RegMacCountersBaaEndCounter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE0_COUNTER 0x264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType0Counter:32;	// baa_pd_type0_counter
	} bitFields;
} RegMacCountersBaaPdType0Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE1_COUNTER 0x268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType1Counter:32;	// baa_pd_type1_counter
	} bitFields;
} RegMacCountersBaaPdType1Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE2_COUNTER 0x26C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType2Counter:32;	// baa_pd_type2_counter
	} bitFields;
} RegMacCountersBaaPdType2Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE3_COUNTER 0x270 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType3Counter:32;	// baa_pd_type3_counter
	} bitFields;
} RegMacCountersBaaPdType3Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE4_COUNTER 0x274 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType4Counter:32;	// baa_pd_type4_counter
	} bitFields;
} RegMacCountersBaaPdType4Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE5_COUNTER 0x278 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType5Counter:32;	// baa_pd_type5_counter
	} bitFields;
} RegMacCountersBaaPdType5Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE6_COUNTER 0x27C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType6Counter:32;	// baa_pd_type6_counter
	} bitFields;
} RegMacCountersBaaPdType6Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE7_COUNTER 0x280 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType7Counter:32;	// baa_pd_type7_counter
	} bitFields;
} RegMacCountersBaaPdType7Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE8_COUNTER 0x284 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType8Counter:32;	// baa_pd_type8_counter
	} bitFields;
} RegMacCountersBaaPdType8Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE9_COUNTER 0x288 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType9Counter:32;	// baa_pd_type9_counter
	} bitFields;
} RegMacCountersBaaPdType9Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE10_COUNTER 0x28C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType10Counter:32;	// baa_pd_type10_counter
	} bitFields;
} RegMacCountersBaaPdType10Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE11_COUNTER 0x290 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType11Counter:32;	// baa_pd_type11_counter
	} bitFields;
} RegMacCountersBaaPdType11Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE12_COUNTER 0x294 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType12Counter:32;	// baa_pd_type12_counter
	} bitFields;
} RegMacCountersBaaPdType12Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE13_COUNTER 0x298 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType13Counter:32;	// baa_pd_type13_counter
	} bitFields;
} RegMacCountersBaaPdType13Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE14_COUNTER 0x29C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType14Counter:32;	// baa_pd_type14_counter
	} bitFields;
} RegMacCountersBaaPdType14Counter_u;

/*REG_MAC_COUNTERS_BAA_PD_TYPE15_COUNTER 0x2A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaPdType15Counter:32;	// baa_pd_type15_counter
	} bitFields;
} RegMacCountersBaaPdType15Counter_u;

/*REG_MAC_COUNTERS_SUCCESS_BAA_TID0_COUNTER 0x2A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successBaaTid0Counter:32;	// success_baa_tid0_counter
	} bitFields;
} RegMacCountersSuccessBaaTid0Counter_u;

/*REG_MAC_COUNTERS_SUCCESS_BAA_TID1_COUNTER 0x2A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successBaaTid1Counter:32;	// success_baa_tid1_counter
	} bitFields;
} RegMacCountersSuccessBaaTid1Counter_u;

/*REG_MAC_COUNTERS_SUCCESS_BAA_TID2_COUNTER 0x2AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successBaaTid2Counter:32;	// success_baa_tid2_counter
	} bitFields;
} RegMacCountersSuccessBaaTid2Counter_u;

/*REG_MAC_COUNTERS_SUCCESS_BAA_TID3_COUNTER 0x2B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successBaaTid3Counter:32;	// success_baa_tid3_counter
	} bitFields;
} RegMacCountersSuccessBaaTid3Counter_u;

/*REG_MAC_COUNTERS_SUCCESS_BAA_TID4_COUNTER 0x2B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successBaaTid4Counter:32;	// success_baa_tid4_counter
	} bitFields;
} RegMacCountersSuccessBaaTid4Counter_u;

/*REG_MAC_COUNTERS_SUCCESS_BAA_TID5_COUNTER 0x2B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successBaaTid5Counter:32;	// success_baa_tid5_counter
	} bitFields;
} RegMacCountersSuccessBaaTid5Counter_u;

/*REG_MAC_COUNTERS_SUCCESS_BAA_TID6_COUNTER 0x2BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successBaaTid6Counter:32;	// success_baa_tid6_counter
	} bitFields;
} RegMacCountersSuccessBaaTid6Counter_u;

/*REG_MAC_COUNTERS_SUCCESS_BAA_TID7_COUNTER 0x2C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 successBaaTid7Counter:32;	// success_baa_tid7_counter
	} bitFields;
} RegMacCountersSuccessBaaTid7Counter_u;

/*REG_MAC_COUNTERS_RETRY_BAA_TID0_COUNTER 0x2C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryBaaTid0Counter:32;	// retry_baa_tid0_counter
	} bitFields;
} RegMacCountersRetryBaaTid0Counter_u;

/*REG_MAC_COUNTERS_RETRY_BAA_TID1_COUNTER 0x2C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryBaaTid1Counter:32;	// retry_baa_tid1_counter
	} bitFields;
} RegMacCountersRetryBaaTid1Counter_u;

/*REG_MAC_COUNTERS_RETRY_BAA_TID2_COUNTER 0x2CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryBaaTid2Counter:32;	// retry_baa_tid2_counter
	} bitFields;
} RegMacCountersRetryBaaTid2Counter_u;

/*REG_MAC_COUNTERS_RETRY_BAA_TID3_COUNTER 0x2D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryBaaTid3Counter:32;	// retry_baa_tid3_counter
	} bitFields;
} RegMacCountersRetryBaaTid3Counter_u;

/*REG_MAC_COUNTERS_RETRY_BAA_TID4_COUNTER 0x2D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryBaaTid4Counter:32;	// retry_baa_tid4_counter
	} bitFields;
} RegMacCountersRetryBaaTid4Counter_u;

/*REG_MAC_COUNTERS_RETRY_BAA_TID5_COUNTER 0x2D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryBaaTid5Counter:32;	// retry_baa_tid5_counter
	} bitFields;
} RegMacCountersRetryBaaTid5Counter_u;

/*REG_MAC_COUNTERS_RETRY_BAA_TID6_COUNTER 0x2DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryBaaTid6Counter:32;	// retry_baa_tid6_counter
	} bitFields;
} RegMacCountersRetryBaaTid6Counter_u;

/*REG_MAC_COUNTERS_RETRY_BAA_TID7_COUNTER 0x2E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryBaaTid7Counter:32;	// retry_baa_tid7_counter
	} bitFields;
} RegMacCountersRetryBaaTid7Counter_u;

/*REG_MAC_COUNTERS_DISCARD_BAA_TID0_COUNTER 0x2E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 discardBaaTid0Counter:32;	// discard_baa_tid0_counter
	} bitFields;
} RegMacCountersDiscardBaaTid0Counter_u;

/*REG_MAC_COUNTERS_DISCARD_BAA_TID1_COUNTER 0x2E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 discardBaaTid1Counter:32;	// discard_baa_tid1_counter
	} bitFields;
} RegMacCountersDiscardBaaTid1Counter_u;

/*REG_MAC_COUNTERS_DISCARD_BAA_TID2_COUNTER 0x2EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 discardBaaTid2Counter:32;	// discard_baa_tid2_counter
	} bitFields;
} RegMacCountersDiscardBaaTid2Counter_u;

/*REG_MAC_COUNTERS_DISCARD_BAA_TID3_COUNTER 0x2F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 discardBaaTid3Counter:32;	// discard_baa_tid3_counter
	} bitFields;
} RegMacCountersDiscardBaaTid3Counter_u;

/*REG_MAC_COUNTERS_DISCARD_BAA_TID4_COUNTER 0x2F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 discardBaaTid4Counter:32;	// discard_baa_tid4_counter
	} bitFields;
} RegMacCountersDiscardBaaTid4Counter_u;

/*REG_MAC_COUNTERS_DISCARD_BAA_TID5_COUNTER 0x2F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 discardBaaTid5Counter:32;	// discard_baa_tid5_counter
	} bitFields;
} RegMacCountersDiscardBaaTid5Counter_u;

/*REG_MAC_COUNTERS_DISCARD_BAA_TID6_COUNTER 0x2FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 discardBaaTid6Counter:32;	// discard_baa_tid6_counter
	} bitFields;
} RegMacCountersDiscardBaaTid6Counter_u;

/*REG_MAC_COUNTERS_DISCARD_BAA_TID7_COUNTER 0x300 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 discardBaaTid7Counter:32;	// discard_baa_tid7_counter
	} bitFields;
} RegMacCountersDiscardBaaTid7Counter_u;

/*REG_MAC_COUNTERS_ACK_BAA_TID0_COUNTER 0x304 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ackBaaTid0Counter:32;	// ack_baa_tid0_counter
	} bitFields;
} RegMacCountersAckBaaTid0Counter_u;

/*REG_MAC_COUNTERS_ACK_BAA_TID1_COUNTER 0x308 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ackBaaTid1Counter:32;	// ack_baa_tid1_counter
	} bitFields;
} RegMacCountersAckBaaTid1Counter_u;

/*REG_MAC_COUNTERS_ACK_BAA_TID2_COUNTER 0x30C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ackBaaTid2Counter:32;	// ack_baa_tid2_counter
	} bitFields;
} RegMacCountersAckBaaTid2Counter_u;

/*REG_MAC_COUNTERS_ACK_BAA_TID3_COUNTER 0x310 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ackBaaTid3Counter:32;	// ack_baa_tid3_counter
	} bitFields;
} RegMacCountersAckBaaTid3Counter_u;

/*REG_MAC_COUNTERS_ACK_BAA_TID4_COUNTER 0x314 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ackBaaTid4Counter:32;	// ack_baa_tid4_counter
	} bitFields;
} RegMacCountersAckBaaTid4Counter_u;

/*REG_MAC_COUNTERS_ACK_BAA_TID5_COUNTER 0x318 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ackBaaTid5Counter:32;	// ack_baa_tid5_counter
	} bitFields;
} RegMacCountersAckBaaTid5Counter_u;

/*REG_MAC_COUNTERS_ACK_BAA_TID6_COUNTER 0x31C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ackBaaTid6Counter:32;	// ack_baa_tid6_counter
	} bitFields;
} RegMacCountersAckBaaTid6Counter_u;

/*REG_MAC_COUNTERS_ACK_BAA_TID7_COUNTER 0x320 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ackBaaTid7Counter:32;	// ack_baa_tid7_counter
	} bitFields;
} RegMacCountersAckBaaTid7Counter_u;

/*REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID0_COUNTER 0x324 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockAckBaaTid0Counter:32;	// block_ack_baa_tid0_counter
	} bitFields;
} RegMacCountersBlockAckBaaTid0Counter_u;

/*REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID1_COUNTER 0x328 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockAckBaaTid1Counter:32;	// block_ack_baa_tid1_counter
	} bitFields;
} RegMacCountersBlockAckBaaTid1Counter_u;

/*REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID2_COUNTER 0x32C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockAckBaaTid2Counter:32;	// block_ack_baa_tid2_counter
	} bitFields;
} RegMacCountersBlockAckBaaTid2Counter_u;

/*REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID3_COUNTER 0x330 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockAckBaaTid3Counter:32;	// block_ack_baa_tid3_counter
	} bitFields;
} RegMacCountersBlockAckBaaTid3Counter_u;

/*REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID4_COUNTER 0x334 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockAckBaaTid4Counter:32;	// block_ack_baa_tid4_counter
	} bitFields;
} RegMacCountersBlockAckBaaTid4Counter_u;

/*REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID5_COUNTER 0x338 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockAckBaaTid5Counter:32;	// block_ack_baa_tid5_counter
	} bitFields;
} RegMacCountersBlockAckBaaTid5Counter_u;

/*REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID6_COUNTER 0x33C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockAckBaaTid6Counter:32;	// block_ack_baa_tid6_counter
	} bitFields;
} RegMacCountersBlockAckBaaTid6Counter_u;

/*REG_MAC_COUNTERS_BLOCK_ACK_BAA_TID7_COUNTER 0x340 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockAckBaaTid7Counter:32;	// block_ack_baa_tid7_counter
	} bitFields;
} RegMacCountersBlockAckBaaTid7Counter_u;

/*REG_MAC_COUNTERS_NACK_BAA_TID0_COUNTER 0x344 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nackBaaTid0Counter:32;	// nack_baa_tid0_counter
	} bitFields;
} RegMacCountersNackBaaTid0Counter_u;

/*REG_MAC_COUNTERS_NACK_BAA_TID1_COUNTER 0x348 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nackBaaTid1Counter:32;	// nack_baa_tid1_counter
	} bitFields;
} RegMacCountersNackBaaTid1Counter_u;

/*REG_MAC_COUNTERS_NACK_BAA_TID2_COUNTER 0x34C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nackBaaTid2Counter:32;	// nack_baa_tid2_counter
	} bitFields;
} RegMacCountersNackBaaTid2Counter_u;

/*REG_MAC_COUNTERS_NACK_BAA_TID3_COUNTER 0x350 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nackBaaTid3Counter:32;	// nack_baa_tid3_counter
	} bitFields;
} RegMacCountersNackBaaTid3Counter_u;

/*REG_MAC_COUNTERS_NACK_BAA_TID4_COUNTER 0x354 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nackBaaTid4Counter:32;	// nack_baa_tid4_counter
	} bitFields;
} RegMacCountersNackBaaTid4Counter_u;

/*REG_MAC_COUNTERS_NACK_BAA_TID5_COUNTER 0x358 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nackBaaTid5Counter:32;	// nack_baa_tid5_counter
	} bitFields;
} RegMacCountersNackBaaTid5Counter_u;

/*REG_MAC_COUNTERS_NACK_BAA_TID6_COUNTER 0x35C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nackBaaTid6Counter:32;	// nack_baa_tid6_counter
	} bitFields;
} RegMacCountersNackBaaTid6Counter_u;

/*REG_MAC_COUNTERS_NACK_BAA_TID7_COUNTER 0x360 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nackBaaTid7Counter:32;	// nack_baa_tid7_counter
	} bitFields;
} RegMacCountersNackBaaTid7Counter_u;

/*REG_MAC_COUNTERS_TX_SEL_START_CTR 0x400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelStartCtr:32;	// tx_sel_start_ctr
	} bitFields;
} RegMacCountersTxSelStartCtr_u;

/*REG_MAC_COUNTERS_TX_SEL_END_CTR 0x404 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelEndCtr:32;	// tx_sel_end_ctr
	} bitFields;
} RegMacCountersTxSelEndCtr_u;

/*REG_MAC_COUNTERS_TX_SEL_STA_TID0_CTR 0x408 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelStaTid0Ctr:32;	// tx_sel_sta_tid0_ctr
	} bitFields;
} RegMacCountersTxSelStaTid0Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_STA_TID1_CTR 0x40C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelStaTid1Ctr:32;	// tx_sel_sta_tid1_ctr
	} bitFields;
} RegMacCountersTxSelStaTid1Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_STA_TID2_CTR 0x410 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelStaTid2Ctr:32;	// tx_sel_sta_tid2_ctr
	} bitFields;
} RegMacCountersTxSelStaTid2Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_STA_TID3_CTR 0x414 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelStaTid3Ctr:32;	// tx_sel_sta_tid3_ctr
	} bitFields;
} RegMacCountersTxSelStaTid3Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_STA_TID4_CTR 0x418 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelStaTid4Ctr:32;	// tx_sel_sta_tid4_ctr
	} bitFields;
} RegMacCountersTxSelStaTid4Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_STA_TID5_CTR 0x41C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelStaTid5Ctr:32;	// tx_sel_sta_tid5_ctr
	} bitFields;
} RegMacCountersTxSelStaTid5Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_STA_TID6_CTR 0x420 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelStaTid6Ctr:32;	// tx_sel_sta_tid6_ctr
	} bitFields;
} RegMacCountersTxSelStaTid6Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_STA_TID7_CTR 0x424 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelStaTid7Ctr:32;	// tx_sel_sta_tid7_ctr
	} bitFields;
} RegMacCountersTxSelStaTid7Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_VAP_TID0_CTR 0x428 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelVapTid0Ctr:32;	// tx_sel_vap_tid0_ctr
	} bitFields;
} RegMacCountersTxSelVapTid0Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_VAP_TID1_CTR 0x42C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelVapTid1Ctr:32;	// tx_sel_vap_tid1_ctr
	} bitFields;
} RegMacCountersTxSelVapTid1Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_VAP_TID2_CTR 0x430 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelVapTid2Ctr:32;	// tx_sel_vap_tid2_ctr
	} bitFields;
} RegMacCountersTxSelVapTid2Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_VAP_TID3_CTR 0x434 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelVapTid3Ctr:32;	// tx_sel_vap_tid3_ctr
	} bitFields;
} RegMacCountersTxSelVapTid3Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_VAP_TID4_CTR 0x438 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelVapTid4Ctr:32;	// tx_sel_vap_tid4_ctr
	} bitFields;
} RegMacCountersTxSelVapTid4Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_VAP_TID5_CTR 0x43C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelVapTid5Ctr:32;	// tx_sel_vap_tid5_ctr
	} bitFields;
} RegMacCountersTxSelVapTid5Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_VAP_TID6_CTR 0x440 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelVapTid6Ctr:32;	// tx_sel_vap_tid6_ctr
	} bitFields;
} RegMacCountersTxSelVapTid6Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_VAP_TID7_CTR 0x444 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelVapTid7Ctr:32;	// tx_sel_vap_tid7_ctr
	} bitFields;
} RegMacCountersTxSelVapTid7Ctr_u;

/*REG_MAC_COUNTERS_TX_SEL_GLOBAL_BE_CTR 0x448 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelGlobalBeCtr:32;	// tx_sel_global_be_ctr
	} bitFields;
} RegMacCountersTxSelGlobalBeCtr_u;

/*REG_MAC_COUNTERS_TX_SEL_GLOBAL_BK_CTR 0x44C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelGlobalBkCtr:32;	// tx_sel_global_bk_ctr
	} bitFields;
} RegMacCountersTxSelGlobalBkCtr_u;

/*REG_MAC_COUNTERS_TX_SEL_GLOBAL_VI_CTR 0x450 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelGlobalViCtr:32;	// tx_sel_global_vi_ctr
	} bitFields;
} RegMacCountersTxSelGlobalViCtr_u;

/*REG_MAC_COUNTERS_TX_SEL_GLOBAL_VO_CTR 0x454 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelGlobalVoCtr:32;	// tx_sel_global_vo_ctr
	} bitFields;
} RegMacCountersTxSelGlobalVoCtr_u;

/*REG_MAC_COUNTERS_TX_SEL_GPLP_CTR 0x458 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelGplpCtr:32;	// tx_sel_gplp_ctr
	} bitFields;
} RegMacCountersTxSelGplpCtr_u;

/*REG_MAC_COUNTERS_TX_SEL_GPHP_CTR 0x45C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelGphpCtr:32;	// tx_sel_gphp_ctr
	} bitFields;
} RegMacCountersTxSelGphpCtr_u;

/*REG_MAC_COUNTERS_TX_SEL_BEACON_CTR 0x460 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelBeaconCtr:32;	// tx_sel_beacon_ctr
	} bitFields;
} RegMacCountersTxSelBeaconCtr_u;

/*REG_MAC_COUNTERS_TX_IN_HOST_IF_HD_COUNT_CTR 0x464 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHostIfHdCountCtr:32;	// tx_in_host_if_hd_count_ctr
	} bitFields;
} RegMacCountersTxInHostIfHdCountCtr_u;

/*REG_MAC_COUNTERS_TX_OUT_HOST_IF_HD_COUNT_CTR 0x468 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHostIfHdCountCtr:32;	// tx_out_host_if_hd_count_ctr
	} bitFields;
} RegMacCountersTxOutHostIfHdCountCtr_u;

/*REG_MAC_COUNTERS_RX_OUT_HOST_IF_HD_COUNT_CTR 0x46C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHostIfHdCountCtr:32;	// rx_out_host_if_hd_count_ctr
	} bitFields;
} RegMacCountersRxOutHostIfHdCountCtr_u;

/*REG_MAC_COUNTERS_TX_IN_HOST_IF_BUFFER_COUNT_CTR 0x470 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHostIfBufferCountCtr:32;	// tx_in_host_if_buffer_count_ctr
	} bitFields;
} RegMacCountersTxInHostIfBufferCountCtr_u;

/*REG_MAC_COUNTERS_TX_OUT_HOST_IF_BUFFER_COUNT_CTR 0x474 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHostIfBufferCountCtr:32;	// tx_out_host_if_buffer_count_ctr
	} bitFields;
} RegMacCountersTxOutHostIfBufferCountCtr_u;

/*REG_MAC_COUNTERS_RX_OUT_HOST_IF_BUFFER_COUNT_CTR 0x478 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHostIfBufferCountCtr:32;	// rx_out_host_if_buffer_count_ctr
	} bitFields;
} RegMacCountersRxOutHostIfBufferCountCtr_u;

/*REG_MAC_COUNTERS_LOGGER_EVENT_ENABLE 0x500 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectionStartPulseEna:1;
		uint32 selectionEndPulseEna:1;
		uint32 dbgAggBuilderMainStartPulseEna:1;
		uint32 dbgAggBuilderMainEndPulseEna:1;
		uint32 senderStartStbEna:1;
		uint32 senderEndStbEna:1;
		uint32 delStartStbEna:1;
		uint32 delEndStbEna:1;
		uint32 txhNtdBaaStartPulseEna:1;
		uint32 baaBaaDonePulseEna:1;
		uint32 reserved0:22;
	} bitFields;
} RegMacCountersLoggerEventEnable_u;

/*REG_MAC_COUNTERS_SENDER_EVENT 0x504 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderStartStb:1;
		uint32 senderEndStb:1;
		uint32 reserved0:30;
	} bitFields;
} RegMacCountersSenderEvent_u;

/*REG_MAC_COUNTERS_ENTRIES_IN_LOGGER 0x508 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entriesInLogger:9;
		uint32 reserved0:23;
	} bitFields;
} RegMacCountersEntriesInLogger_u;

/*REG_MAC_COUNTERS_LOGGER_RESET 0x50C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerReset:1;
		uint32 reserved0:31;
	} bitFields;
} RegMacCountersLoggerReset_u;



#endif // _MAC_COUNTERS_REGS_H_
