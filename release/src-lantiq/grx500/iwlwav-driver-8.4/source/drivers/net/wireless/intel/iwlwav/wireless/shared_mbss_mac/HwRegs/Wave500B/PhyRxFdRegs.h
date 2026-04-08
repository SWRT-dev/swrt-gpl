
/***********************************************************************************
File:				PhyRxFdRegs.h
Module:				phyRxFd
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_RX_FD_REGS_H_
#define _PHY_RX_FD_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_RX_FD_BASE_ADDRESS                             MEMORY_MAP_UNIT_51_BASE_ADDRESS
#define	REG_PHY_RX_FD_PHY_RXFD_REG06    (PHY_RX_FD_BASE_ADDRESS + 0x18)
#define	REG_PHY_RX_FD_PHY_RXFD_REG07    (PHY_RX_FD_BASE_ADDRESS + 0x1C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG0A    (PHY_RX_FD_BASE_ADDRESS + 0x28)
#define	REG_PHY_RX_FD_PHY_RXFD_REG0B    (PHY_RX_FD_BASE_ADDRESS + 0x2C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG0C    (PHY_RX_FD_BASE_ADDRESS + 0x30)
#define	REG_PHY_RX_FD_PHY_RXFD_REG0D    (PHY_RX_FD_BASE_ADDRESS + 0x34)
#define	REG_PHY_RX_FD_PHY_RXFD_REG0E    (PHY_RX_FD_BASE_ADDRESS + 0x38)
#define	REG_PHY_RX_FD_PHY_RXFD_REG0F    (PHY_RX_FD_BASE_ADDRESS + 0x3C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG11    (PHY_RX_FD_BASE_ADDRESS + 0x44)
#define	REG_PHY_RX_FD_PHY_RXFD_REG12    (PHY_RX_FD_BASE_ADDRESS + 0x48)
#define	REG_PHY_RX_FD_PHY_RXFD_REG13    (PHY_RX_FD_BASE_ADDRESS + 0x4C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG14    (PHY_RX_FD_BASE_ADDRESS + 0x50)
#define	REG_PHY_RX_FD_PHY_RXFD_REG15    (PHY_RX_FD_BASE_ADDRESS + 0x54)
#define	REG_PHY_RX_FD_PHY_RXFD_REG16    (PHY_RX_FD_BASE_ADDRESS + 0x58)
#define	REG_PHY_RX_FD_PHY_RXFD_REG1B    (PHY_RX_FD_BASE_ADDRESS + 0x6C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG1C    (PHY_RX_FD_BASE_ADDRESS + 0x70)
#define	REG_PHY_RX_FD_PHY_RXFD_REG1D    (PHY_RX_FD_BASE_ADDRESS + 0x74)
#define	REG_PHY_RX_FD_PHY_RXFD_REG1E    (PHY_RX_FD_BASE_ADDRESS + 0x78)
#define	REG_PHY_RX_FD_PHY_RXFD_REG34    (PHY_RX_FD_BASE_ADDRESS + 0xD0)
#define	REG_PHY_RX_FD_PHY_RXFD_REG35    (PHY_RX_FD_BASE_ADDRESS + 0xD4)
#define	REG_PHY_RX_FD_PHY_RXFD_REG36    (PHY_RX_FD_BASE_ADDRESS + 0xD8)
#define	REG_PHY_RX_FD_PHY_RXFD_REG37    (PHY_RX_FD_BASE_ADDRESS + 0xDC)
#define	REG_PHY_RX_FD_PHY_RXFD_REG38    (PHY_RX_FD_BASE_ADDRESS + 0xE0)
#define	REG_PHY_RX_FD_PHY_RXFD_REG3A    (PHY_RX_FD_BASE_ADDRESS + 0xE8)
#define	REG_PHY_RX_FD_PHY_RXFD_REG3B    (PHY_RX_FD_BASE_ADDRESS + 0xEC)
#define	REG_PHY_RX_FD_PHY_RXFD_REG3C    (PHY_RX_FD_BASE_ADDRESS + 0xF0)
#define	REG_PHY_RX_FD_PHY_RXFD_REG3E    (PHY_RX_FD_BASE_ADDRESS + 0xF8)
#define	REG_PHY_RX_FD_PHY_RXFD_REG3F    (PHY_RX_FD_BASE_ADDRESS + 0xFC)
#define	REG_PHY_RX_FD_PHY_RXFD_REG40    (PHY_RX_FD_BASE_ADDRESS + 0x100)
#define	REG_PHY_RX_FD_PHY_RXFD_REG4E    (PHY_RX_FD_BASE_ADDRESS + 0x138)
#define	REG_PHY_RX_FD_PHY_RXFD_REG50    (PHY_RX_FD_BASE_ADDRESS + 0x140)
#define	REG_PHY_RX_FD_PHY_RXFD_REG51    (PHY_RX_FD_BASE_ADDRESS + 0x144)
#define	REG_PHY_RX_FD_PHY_RXFD_REG57    (PHY_RX_FD_BASE_ADDRESS + 0x15C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG68    (PHY_RX_FD_BASE_ADDRESS + 0x1A0)
#define	REG_PHY_RX_FD_PHY_RXFD_REG73    (PHY_RX_FD_BASE_ADDRESS + 0x1CC)
#define	REG_PHY_RX_FD_PHY_RXFD_REG74    (PHY_RX_FD_BASE_ADDRESS + 0x1D0)
#define	REG_PHY_RX_FD_PHY_RXFD_REG75    (PHY_RX_FD_BASE_ADDRESS + 0x1D4)
#define	REG_PHY_RX_FD_PHY_RXFD_REG76    (PHY_RX_FD_BASE_ADDRESS + 0x1D8)
#define	REG_PHY_RX_FD_PHY_RXFD_REG77    (PHY_RX_FD_BASE_ADDRESS + 0x1DC)
#define	REG_PHY_RX_FD_PHY_RXFD_REG78    (PHY_RX_FD_BASE_ADDRESS + 0x1E0)
#define	REG_PHY_RX_FD_PHY_RXFD_REG79    (PHY_RX_FD_BASE_ADDRESS + 0x1E4)
#define	REG_PHY_RX_FD_PHY_RXFD_REG7A    (PHY_RX_FD_BASE_ADDRESS + 0x1E8)
#define	REG_PHY_RX_FD_PHY_RXFD_REG7B    (PHY_RX_FD_BASE_ADDRESS + 0x1EC)
#define	REG_PHY_RX_FD_PHY_RXFD_REG7C    (PHY_RX_FD_BASE_ADDRESS + 0x1F0)
#define	REG_PHY_RX_FD_PHY_RXFD_REG7D    (PHY_RX_FD_BASE_ADDRESS + 0x1F4)
#define	REG_PHY_RX_FD_PHY_RXFD_REG7E    (PHY_RX_FD_BASE_ADDRESS + 0x1F8)
#define	REG_PHY_RX_FD_PHY_RXFD_REG7F    (PHY_RX_FD_BASE_ADDRESS + 0x1FC)
#define	REG_PHY_RX_FD_PHY_RXFD_REG80    (PHY_RX_FD_BASE_ADDRESS + 0x200)
#define	REG_PHY_RX_FD_PHY_RXFD_REG81    (PHY_RX_FD_BASE_ADDRESS + 0x204)
#define	REG_PHY_RX_FD_PHY_RXFD_REG82    (PHY_RX_FD_BASE_ADDRESS + 0x208)
#define	REG_PHY_RX_FD_PHY_RXFD_REG83    (PHY_RX_FD_BASE_ADDRESS + 0x20C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG84    (PHY_RX_FD_BASE_ADDRESS + 0x210)
#define	REG_PHY_RX_FD_PHY_RXFD_REG86    (PHY_RX_FD_BASE_ADDRESS + 0x218)
#define	REG_PHY_RX_FD_PHY_RXFD_REG87    (PHY_RX_FD_BASE_ADDRESS + 0x21C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG88    (PHY_RX_FD_BASE_ADDRESS + 0x220)
#define	REG_PHY_RX_FD_PHY_RXFD_REG89    (PHY_RX_FD_BASE_ADDRESS + 0x224)
#define	REG_PHY_RX_FD_PHY_RXFD_REG8A    (PHY_RX_FD_BASE_ADDRESS + 0x228)
#define	REG_PHY_RX_FD_PHY_RXFD_REG8B    (PHY_RX_FD_BASE_ADDRESS + 0x22C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG8C    (PHY_RX_FD_BASE_ADDRESS + 0x230)
#define	REG_PHY_RX_FD_PHY_RXFD_REG8D    (PHY_RX_FD_BASE_ADDRESS + 0x234)
#define	REG_PHY_RX_FD_PHY_RXFD_IF08E    (PHY_RX_FD_BASE_ADDRESS + 0x238)
#define	REG_PHY_RX_FD_PHY_RXFD_REG8F    (PHY_RX_FD_BASE_ADDRESS + 0x23C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG90    (PHY_RX_FD_BASE_ADDRESS + 0x240)
#define	REG_PHY_RX_FD_PHY_RXFD_REG91    (PHY_RX_FD_BASE_ADDRESS + 0x244)
#define	REG_PHY_RX_FD_PHY_RXFD_REG92    (PHY_RX_FD_BASE_ADDRESS + 0x248)
#define	REG_PHY_RX_FD_PHY_RXFD_REG93    (PHY_RX_FD_BASE_ADDRESS + 0x24C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG94    (PHY_RX_FD_BASE_ADDRESS + 0x250)
#define	REG_PHY_RX_FD_PHY_RXFD_REG95    (PHY_RX_FD_BASE_ADDRESS + 0x254)
#define	REG_PHY_RX_FD_PHY_RXFD_REG96    (PHY_RX_FD_BASE_ADDRESS + 0x258)
#define	REG_PHY_RX_FD_PHY_RXFD_REG97    (PHY_RX_FD_BASE_ADDRESS + 0x25C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG98    (PHY_RX_FD_BASE_ADDRESS + 0x260)
#define	REG_PHY_RX_FD_PHY_RXFD_REG99    (PHY_RX_FD_BASE_ADDRESS + 0x264)
#define	REG_PHY_RX_FD_PHY_RXFD_REG9A    (PHY_RX_FD_BASE_ADDRESS + 0x268)
#define	REG_PHY_RX_FD_PHY_RXFD_REG9B    (PHY_RX_FD_BASE_ADDRESS + 0x26C)
#define	REG_PHY_RX_FD_PHY_RXFD_REG9C    (PHY_RX_FD_BASE_ADDRESS + 0x270)
#define	REG_PHY_RX_FD_PHY_RXFD_REG9D    (PHY_RX_FD_BASE_ADDRESS + 0x274)
#define	REG_PHY_RX_FD_PHY_RXFD_REG9E    (PHY_RX_FD_BASE_ADDRESS + 0x278)
#define	REG_PHY_RX_FD_PHY_RXFD_REG9F    (PHY_RX_FD_BASE_ADDRESS + 0x27C)
#define	REG_PHY_RX_FD_PHY_RXFD_REGA0    (PHY_RX_FD_BASE_ADDRESS + 0x280)
#define	REG_PHY_RX_FD_PHY_RXFD_REGA1    (PHY_RX_FD_BASE_ADDRESS + 0x284)
#define	REG_PHY_RX_FD_PHY_RXFD_REGA2    (PHY_RX_FD_BASE_ADDRESS + 0x288)
#define	REG_PHY_RX_FD_PHY_RXFD_REGA3    (PHY_RX_FD_BASE_ADDRESS + 0x28C)
#define	REG_PHY_RX_FD_PHY_RXFD_REGA4    (PHY_RX_FD_BASE_ADDRESS + 0x290)
#define	REG_PHY_RX_FD_PHY_RXFD_REGA5    (PHY_RX_FD_BASE_ADDRESS + 0x294)
#define	REG_PHY_RX_FD_PHY_RXFD_REGA6    (PHY_RX_FD_BASE_ADDRESS + 0x298)
#define	REG_PHY_RX_FD_PHY_RXFD_REGA7    (PHY_RX_FD_BASE_ADDRESS + 0x29C)
#define	REG_PHY_RX_FD_PHY_RXFD_REGA8    (PHY_RX_FD_BASE_ADDRESS + 0x2A0)
#define	REG_PHY_RX_FD_PHY_RXFD_REGA9    (PHY_RX_FD_BASE_ADDRESS + 0x2A4)
#define	REG_PHY_RX_FD_PHY_RXFD_REGAA    (PHY_RX_FD_BASE_ADDRESS + 0x2A8)
#define	REG_PHY_RX_FD_PHY_RXFD_REGAB    (PHY_RX_FD_BASE_ADDRESS + 0x2AC)
#define	REG_PHY_RX_FD_PHY_RXFD_REGBE    (PHY_RX_FD_BASE_ADDRESS + 0x2F8)
#define	REG_PHY_RX_FD_PHY_RXFD_REGBF    (PHY_RX_FD_BASE_ADDRESS + 0x2FC)
#define	REG_PHY_RX_FD_PHY_RXFD_REGC0    (PHY_RX_FD_BASE_ADDRESS + 0x300)
#define	REG_PHY_RX_FD_PHY_RXFD_REGC1    (PHY_RX_FD_BASE_ADDRESS + 0x304)
#define	REG_PHY_RX_FD_PHY_RXFD_REGC2    (PHY_RX_FD_BASE_ADDRESS + 0x308)
#define	REG_PHY_RX_FD_PHY_RXFD_REGC3    (PHY_RX_FD_BASE_ADDRESS + 0x30C)
#define	REG_PHY_RX_FD_PHY_RXFD_REGC4    (PHY_RX_FD_BASE_ADDRESS + 0x310)
#define	REG_PHY_RX_FD_PHY_RXFD_REGC5    (PHY_RX_FD_BASE_ADDRESS + 0x314)
#define	REG_PHY_RX_FD_PHY_RXFD_REGC6    (PHY_RX_FD_BASE_ADDRESS + 0x318)
#define	REG_PHY_RX_FD_PHY_RXFD_REGC7    (PHY_RX_FD_BASE_ADDRESS + 0x31C)
#define	REG_PHY_RX_FD_PHY_RXFD_REGC8    (PHY_RX_FD_BASE_ADDRESS + 0x320)
#define	REG_PHY_RX_FD_PHY_RXFD_REGC9    (PHY_RX_FD_BASE_ADDRESS + 0x324)
#define	REG_PHY_RX_FD_PHY_RXFD_REGCA    (PHY_RX_FD_BASE_ADDRESS + 0x328)
#define	REG_PHY_RX_FD_PHY_RXFD_REGCB    (PHY_RX_FD_BASE_ADDRESS + 0x32C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RX_FD_PHY_RXFD_REG06 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ptBypass:1;	// phase tracking bypass - zero outputs
		uint32 ncoFuncBypass:1;	// nco functional bypass (force phase addr to zero)
		uint32 reserved0:3;
		uint32 upChModeUpChEn:1;	// enable update ch in update mode
		uint32 indpndtNeSwBypass:1;	// independent noise estimation sw bypass
		uint32 dpndtNeSwBypass:1;	// dependent noise estimation sw bypass
		uint32 reserved1:1;
		uint32 rxEnableOverride:1;	// GSM - test mode
		uint32 dpndtNeSymmetricMode:1;	// dpndt_ne_symmetric_mode
		uint32 reserved2:21;
	} bitFields;
} RegPhyRxFdPhyRxfdReg06_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG07 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fftOlaSelect:4;	// fft_ola_select[3],fft_ola_select[2],fft_ola_select[1],fft_ola_select[0]
		uint32 fft0GclkBypass:1;	// fft 0 gated clock bypass
		uint32 fft1GclkBypass:1;	// fft 1 gated clock bypass
		uint32 fft2GclkBypass:1;	// fft 2 gated clock bypass
		uint32 fft3GclkBypass:1;	// fft 3 gated clock bypass
		uint32 fftRnd:2;	// fft output round bit selection in CB80
		uint32 reserved0:2;
		uint32 fftStartWaddr:8;	// fft_start_waddr
		uint32 fcbShortCpTimShift:5;	// short cyclic prefix timing shift - 40M
		uint32 reserved1:2;
		uint32 ncbShortCpTimShift:5;	// short cyclic prefix timing shift - 20M
	} bitFields;
} RegPhyRxFdPhyRxfdReg07_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG0A 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ptSmoothingVhtInitSym:1;	// Phase Tracking smoothing VHT init symbol , 0-second VHT_lLTF, 1-data_symbol
		uint32 reserved0:3;
		uint32 ptSmoothingPhaseInitMode:2;	// Phase Tracking smoothing - phase init mode
		uint32 reserved1:2;
		uint32 ptSmoothingTimingInitMode:2;	// Phase Tracking smoothing - timing init mode
		uint32 reserved2:2;
		uint32 ftEnable:1;	// frequency tracking enable
		uint32 reserved3:7;
		uint32 numOfSymMask:11;	// frequency tracking symbol mask
		uint32 rfDataType:1;	// frequency tracking data type
	} bitFields;
} RegPhyRxFdPhyRxfdReg0A_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG0B 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctGclkDis:1;	// channel tracking gated clock disable
		uint32 reserved0:4;
		uint32 ct2TbEn:1;	// channel (pilot & data) to test bus en (at end of packet)
		uint32 ctForceLlrValidChTrk:1;	// ct_force_llr_valid_ch_trk
		uint32 ctForceLlrValidNoiseTrk:1;	// ct_force_llr_valid_noise_trk
		uint32 ctTxEqualNormMode:1;	// ct_tx_equal_norm_mode
		uint32 toneRotationEn:1;	// tone_rotation_en
		uint32 reserved1:22;
	} bitFields;
} RegPhyRxFdPhyRxfdReg0B_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG0C 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ceTable1Start:10;	// channel estimation (smoothing) HT and VHT 1SS 40M table start address
		uint32 ceTable2Start:10;	// channel estimation (smoothing) VHT more than 1SS 80M table start address
		uint32 ceTable3Start:10;	// channel estimation (smoothing) VHT 1SS 80M table start address
		uint32 reserved0:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg0C_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG0D 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ceH0Set:10;	// channel estimation (smoothing) ho coefficients
		uint32 reserved0:6;
		uint32 ceH1Set:10;	// channel estimation (smoothing) h1 coefficients
		uint32 reserved1:6;
	} bitFields;
} RegPhyRxFdPhyRxfdReg0D_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG0E 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ceH2Set:10;	// channel estimation (smoothing) h2 coefficients
		uint32 reserved0:6;
		uint32 ceH3Set:10;	// channel estimation (smoothing) h3 coefficients
		uint32 reserved1:6;
	} bitFields;
} RegPhyRxFdPhyRxfdReg0E_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG0F 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ceH4Set:10;	// channel estimation (smoothing) h4 coefficients
		uint32 reserved0:6;
		uint32 ceH5Set:10;	// channel estimation (smoothing) h5 coefficients
		uint32 reserved1:6;
	} bitFields;
} RegPhyRxFdPhyRxfdReg0F_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG11 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 long1ScrmInitVal:7;	// long1_scrm_init_val
		uint32 reserved0:1;
		uint32 fcbPrank3MaskAn0:8;	// 40M-scrambler pilots mask rank 3 antenna0
		uint32 fcbPrank3MaskAn1:8;	// 40M-scrambler pilots mask rank 3 antenna1
		uint32 fcbPrank3MaskAn2:8;	// 40M-scrambler pilots mask rank 3 antenna2
	} bitFields;
} RegPhyRxFdPhyRxfdReg11_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG12 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twoBandPrank1Mask:8;	// two_band-scrambler pilots mask rank 1
		uint32 long3FcbPrank1Mask:8;	// long3 40M-scrambler pilots mask rank 1
		uint32 long3NcbPrank1Mask:8;	// long3 20M-scrambler pilots mask rank 1
		uint32 ncbPrank3MaskAn0:8;	// 20M-scrambler pilots mask rank 3 antenna0
	} bitFields;
} RegPhyRxFdPhyRxfdReg12_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG13 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcbPrank1Mask:8;	// 40M-scrambler pilots mask rank 1
		uint32 ncbPrank1Mask:8;	// 20M-scrambler pilots mask rank 1
		uint32 ncbPrank3MaskAn1:8;	// 20M-scrambler pilots mask rank 3 antenna1
		uint32 ncbPrank3MaskAn2:8;	// 20M-scrambler pilots mask rank 3 antenna2
	} bitFields;
} RegPhyRxFdPhyRxfdReg13_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG14 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcbPrank2MaskAn0:8;	// 40M-scrambler pilots mask rank 2 antenna0
		uint32 fcbPrank2MaskAn1:8;	// 40M-scrambler pilots mask rank 2 antenna1
		uint32 ncbPrank2MaskAn0:8;	// 20M-scrambler pilots mask rank 2 antenna0
		uint32 ncbPrank2MaskAn1:8;	// 20M-scrambler pilots mask rank 2 antenna1
	} bitFields;
} RegPhyRxFdPhyRxfdReg14_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG15 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdHtModeJumpAdd:4;	// GSM td_ht_mode_jump_address
		uint32 tdHtLtf2JumpAdd:4;	// GSM td_ht_ltf2_jump_address
		uint32 tdHtCp1JumpAdd:4;	// GSM td_ht_cp_1_jump_address
		uint32 tdHtCp0JumpAdd:4;	// GSM td_ht_cp_0_jump_address
		uint32 tdHtLtf3JumpAdd:4;	// GSM td_ht_ltf3_jump_address
		uint32 tdHtEltfSkipAdd:4;	// GSM td_ht_eltf_skip_addr 
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxFdPhyRxfdReg15_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG16 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fdHtModeJumpAdd:4;	// GSM fd_ht_mode_jump_address
		uint32 fdHtLtf2JumpAdd:4;	// GSM fd_ht_ltf2_jump_address
		uint32 fdHtBfLtfJumpAdd:4;	// GSM - fd extra ltf jump address
		uint32 fdHtLtf3JumpAdd:4;	// GSM fd_ht_ltf3_jump_address
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxFdPhyRxfdReg16_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG1B 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmAbortThr:16;	// gsm_abort_threshold
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxFdPhyRxfdReg1B_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG1C 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lenTp0:3;	// enable data to test_bus {bf_sm,csm,gsm}
		uint32 reserved0:1;
		uint32 lenTp1:3;	// enable data to test_bus {QR R digonal/Non diagonal data,QR R digonal/Non diagonal data, SVD S diagonal data}
		uint32 reserved1:1;
		uint32 lenTp2:3;	// enable data to test_bus {timing_fix, noise gain, phase/frequency tracking}
		uint32 reserved2:1;
		uint32 lenTp3:3;	// bit #12- enable v_compressed to test bus (through bf_rx_parser) , bits 14:13 enable channel to test bus
		uint32 reserved3:13;
		uint32 gsm2TbRate:4;	// gsm information strobe rate x4  to the test bus
	} bitFields;
} RegPhyRxFdPhyRxfdReg1C_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG1D 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareReg:16;	// spare register
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxFdPhyRxfdReg1D_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG1E 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:3;
		uint32 tbRNonDiagonal:1;	// tb_r_non_diagonal
		uint32 reserved1:28;
	} bitFields;
} RegPhyRxFdPhyRxfdReg1E_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG34 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 swHtDetectorEn:1;	// 1 - force detector output to be as sw_ht_detector_11n
		uint32 swHtDetector11N:1;	// force ht_detector output: , 0 - 11a, , 1- 11n
		uint32 cddPolarity:2;	// tim_est CDD polarity: , 00 - bypass , 01 - positive , 10- negative
		uint32 reserved1:1;
		uint32 ceUpdateCh:1;	// channel estimation - write back to channel
		uint32 olaMode:1;	// ola mode
		uint32 olaFftDone:1;	// fft done in ola mode
		uint32 olaSwFftP:1;	// ola  mode - switch between the fft
		uint32 greenfieldDtctrEn:1;	// enable green field detector
		uint32 reserved2:1;
		uint32 enableErrorEvent:1;	// GSM enable error event
		uint32 reserved3:4;
	} bitFields;
} RegPhyRxFdPhyRxfdReg34_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG35 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ceTable4Start:10;	// channel estimation VHT more than 1SS 40M table start address
		uint32 ceTable5Start:10;	// channel estimation VHT more than 1SS 20M table start address
		uint32 reserved0:12;
	} bitFields;
} RegPhyRxFdPhyRxfdReg35_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG36 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:20;
		uint32 swBistStart:1;	// sw_bist_start
		uint32 clearRamMode:1;	// clear_ram_mode
		uint32 testBusLowMuxCtrl:2;	// test_bus_low_mux_ctrl
		uint32 testBusMidMuxCtrl:2;	// test_bus_mid_mux_ctrl
		uint32 testBusHighMuxCtrl:2;	// test_bus_high_mux_ctrl
		uint32 testBusFourMuxCtrl:2;	// test_bus_four_mux_ctrl
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg36_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG37 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:28;
		uint32 slicerGclkBypass:4;	// slicer_gclk_bypass
	} bitFields;
} RegPhyRxFdPhyRxfdReg37_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG38 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 fdGfLsigModeJumpAdd:4;	// GSM - fd (lsig) next address in case of greenfield (default htsig1)
		uint32 fdGfHtsig2ModeJumpAdd:4;	// GSM - fd (htsig2) next address in case of greenfield (default data)
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxFdPhyRxfdReg38_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG3A 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 hypRxfdNeRamRm:3;	// hyp_rxfd_ne_ram_rm
		uint32 reserved1:1;
		uint32 hypRxfdPhaseRamRm:3;	// hyp_rxfd_phase_ram_rm
		uint32 reserved2:1;
		uint32 hypRxfdPhaseSmoothingRamRm:3;	// hyp_rxfd_phase_smoothing_ram_rm
		uint32 reserved3:1;
		uint32 hypRxfdTimingSmoothingRamRm:3;	// hyp_rxfd_timing_smoothing_ram_rm
		uint32 reserved4:1;
	} bitFields;
} RegPhyRxFdPhyRxfdReg3A_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG3B 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdData0ChRamRm:3;	// hyp_rxfd_data_0_ch_ram_rm
		uint32 reserved0:1;
		uint32 hypRxfdData1ChRamRm:3;	// hyp_rxfd_data_1_ch_ram_rm
		uint32 reserved1:1;
		uint32 hypRxfdPilot0ChRamRm:3;	// hyp_rxfd_pilot_0_ch_ram_rm
		uint32 reserved2:1;
		uint32 hypRxfdPilot1ChRamRm:3;	// hyp_rxfd_pilot_1_ch_ram_rm
		uint32 reserved3:1;
		uint32 hypRxfdFft0ImageRamRm:3;	// hyp_rxfd_fft_0_image_ram_rm
		uint32 reserved4:5;
		uint32 hypRxfdPilot2ChRamRm:3;	// hyp_rxfd_pilot_2_ch_ram_rm
		uint32 reserved5:1;
		uint32 hypRxfdPilot3ChRamRm:3;	// hyp_rxfd_pilot_3_ch_ram_rm
		uint32 reserved6:1;
	} bitFields;
} RegPhyRxFdPhyRxfdReg3B_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG3C 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGsmRamStyleRm:3;	// hyp_rxfd_gsm_ram_style_rm
		uint32 reserved0:1;
		uint32 hypRxfdESnrThrRamRm:3;	// hyp_rxfd_e_snr_thr_ram_rm
		uint32 reserved1:25;
	} bitFields;
} RegPhyRxFdPhyRxfdReg3C_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG3E 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 hypRxfdData2ChRamRm:3;	// hyp_rxfd_data_2_ch_ram_rm
		uint32 reserved1:1;
		uint32 hypRxfdData3ChRamRm:3;	// hyp_rxfd_data_3_ch_ram_rm
		uint32 reserved2:1;
		uint32 hypRxfdImplChannPrePhaseRamRm:3;	// hyp_rxfd_impl_chann_pre_phase_ram_rm
		uint32 reserved3:1;
		uint32 hypRxfdImplChannPreTxRxRamRm:3;	// hyp_rxfd_impl_chann_pre_tx_rx_ram_rm
		uint32 reserved4:1;
	} bitFields;
} RegPhyRxFdPhyRxfdReg3E_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG3F 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 effectiveSnrBetaInv1:9;	// effective_snr_beta_inv_1
		uint32 effectiveSnrBetaInv23:9;	// effective_snr_beta_inv_2_3
		uint32 effectiveSnrBetaInv45:9;	// effective_snr_beta_inv_4_5
		uint32 reserved0:5;
	} bitFields;
} RegPhyRxFdPhyRxfdReg3F_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG40 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 effectiveSnrBetaInv678:9;	// effective_snr_beta_inv_6_7_8
		uint32 effectiveSnrBetaInv910:9;	// effective_snr_beta_inv_9_10
		uint32 effectiveSnrBetaForAvrReport:3;	// effective_snr_beta_for_avr_report
		uint32 reserved0:11;
	} bitFields;
} RegPhyRxFdPhyRxfdReg40_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG4E 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 effectiveSnrGclkBypass:1;	// effective_snr_gclk_bypass
		uint32 bfRxParserGclkBypass:1;	// bf_rx_parser_gclk_bypass
		uint32 guessUnitGclkBypass:1;	// guess_unit_gclk_bypass
		uint32 reserved0:28;
		uint32 implChannPreGclkBypass:1;	// impl_chann_pre_gclk_bypass
	} bitFields;
} RegPhyRxFdPhyRxfdReg4E_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG50 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compressedFourierPhase2:14;	// compressed_fourier_phase_2
		uint32 reserved0:2;
		uint32 compressedFourierPhase3:14;	// compressed_fourier_phase_3
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg50_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG51 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compressedFourierPhase4:14;	// compressed_fourier_phase_4
		uint32 reserved0:2;
		uint32 compressedFourierPhase5:14;	// compressed_fourier_phase_5
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg51_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG57 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 channelEstimationUse3Ltfs:1;	// ignore stream #4 in channel estimation (orgnanizer) in HT 3ss
		uint32 reserved1:29;
	} bitFields;
} RegPhyRxFdPhyRxfdReg57_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG68 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 numOfPgc1Values:6;	// Implicit channel preperation - number of PGC1 values
		uint32 numOfPgc2Values:6;	// Implicit channel preperation - number of PGC2 values
		uint32 numOfPgc3Values:6;	// Implicit channel preperation - number of PGC3 values
		uint32 numOfLnaValues:5;	// Implicit channel preperation - number of lna values
		uint32 reserved1:7;
	} bitFields;
} RegPhyRxFdPhyRxfdReg68_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG73 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qrInternalGclkBypass:1;	// qr_gclk_disable
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxFdPhyRxfdReg73_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG74 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 hypRxfdGclkDisable:18;	// hyp_rxfd_gclk_disable
		uint32 reserved1:3;
		uint32 memGlobalRm:2;	// mem_global_rm
		uint32 memGlobalBistScrBypass:1;	// mem_global_bist_scr_bypass
		uint32 memGlobalRamTestMode:1;	// mem_global_ram_test_mode
		uint32 reserved2:6;
	} bitFields;
} RegPhyRxFdPhyRxfdReg74_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG75 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseTrkAlphaFirst:3;	// noise_trk_alpha_first
		uint32 reserved0:1;
		uint32 noiseTrkAlphaSecond:3;	// noise_trk_alpha_second
		uint32 reserved1:11;
		uint32 noiseTrkBypassData:1;	// noise_trk_bypass_data
		uint32 noiseTrkBypassSig:1;	// noise_trk_bypass_sig
		uint32 noiseTrkNoiseGainLimit:4;	// Noise_trk_noise_gain_limit
		uint32 reserved2:8;
	} bitFields;
} RegPhyRxFdPhyRxfdReg75_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG76 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qrGc0PmRmValue:3;	// qr_gc0_pm_rm_value
		uint32 reserved0:1;
		uint32 qrGc1PmRmValue:3;	// qr_gc1_pm_rm_value
		uint32 reserved1:1;
		uint32 qrGc2PmRmValue:3;	// qr_gc2_pm_rm_value
		uint32 reserved2:1;
		uint32 qrGc0Unit0HSMemRmValue:3;	// qr_gc0_unit0_h_s_mem_rm_value
		uint32 reserved3:1;
		uint32 qrGc0Unit0QVMemRmValue:3;	// qr_gc0_unit0_q_v_mem_rm_value
		uint32 reserved4:1;
		uint32 qrGc1Unit0HSMemRmValue:3;	// qr_gc1_unit0_h_s_mem_rm_value
		uint32 reserved5:1;
		uint32 qrGc1Unit0QVMemRmValue:3;	// qr_gc1_unit0_q_v_mem_rm_value
		uint32 reserved6:1;
		uint32 qrGc2Unit0HSMemRmValue:3;	// qr_gc2_unit0_h_s_mem_rm_value
		uint32 reserved7:1;
	} bitFields;
} RegPhyRxFdPhyRxfdReg76_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG77 0x1DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qrGc2Unit0QVMemRmValue:3;	// qr_gc2_unit0_q_v_mem_rm_value
		uint32 reserved0:1;
		uint32 qrGc0Unit1HSMemRmValue:3;	// qr_gc0_unit1_h_s_mem_rm_value
		uint32 reserved1:1;
		uint32 qrGc0Unit1QVMemRmValue:3;	// qr_gc0_unit1_q_v_mem_rm_value
		uint32 reserved2:1;
		uint32 qrGc1Unit1HSMemRmValue:3;	// qr_gc1_unit1_h_s_mem_rm_value
		uint32 reserved3:1;
		uint32 qrGc1Unit1QVMemRmValue:3;	// qr_gc1_unit1_q_v_mem_rm_value
		uint32 reserved4:1;
		uint32 qrGc2Unit1HSMemRmValue:3;	// qr_gc2_unit1_h_s_mem_rm_value
		uint32 reserved5:1;
		uint32 qrGc2Unit1QVMemRmValue:3;	// qr_gc2_unit1_q_v_mem_rm_value
		uint32 reserved6:1;
		uint32 qrGc0Unit2HSMemRmValue:3;	// qr_gc0_unit2_h_s_mem_rm_value
		uint32 reserved7:1;
	} bitFields;
} RegPhyRxFdPhyRxfdReg77_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG78 0x1E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qrGc0Unit2QVMemRmValue:3;	// qr_gc0_unit2_q_v_mem_rm_value
		uint32 reserved0:1;
		uint32 qrGc1Unit2HSMemRmValue:3;	// qr_gc1_unit2_h_s_mem_rm_value
		uint32 reserved1:1;
		uint32 qrGc1Unit2QVMemRmValue:3;	// qr_gc1_unit2_q_v_mem_rm_value
		uint32 reserved2:1;
		uint32 qrGc2Unit2HSMemRmValue:3;	// qr_gc2_unit2_h_s_mem_rm_value
		uint32 reserved3:1;
		uint32 qrGc2Unit2QVMemRmValue:3;	// qr_gc2_unit2_q_v_mem_rm_value
		uint32 reserved4:1;
		uint32 qrGc0Unit3HSMemRmValue:3;	// qr_gc0_unit3_h_s_mem_rm_value
		uint32 reserved5:1;
		uint32 qrGc0Unit3QVMemRmValue:3;	// qr_gc0_unit3_q_v_mem_rm_value
		uint32 reserved6:1;
		uint32 qrGc1Unit3HSMemRmValue:3;	// qr_gc1_unit3_h_s_mem_rm_value
		uint32 reserved7:1;
	} bitFields;
} RegPhyRxFdPhyRxfdReg78_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG79 0x1E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qrGc1Unit3QVMemRmValue:3;	// qr_gc1_unit3_q_v_mem_rm_value
		uint32 reserved0:1;
		uint32 qrGc2Unit3HSMemRmValue:3;	// qr_gc2_unit3_h_s_mem_rm_value
		uint32 reserved1:1;
		uint32 qrGc2Unit3QVMemRmValue:3;	// qr_gc2_unit3_q_v_mem_rm_value
		uint32 reserved2:1;
		uint32 qrSphereIfQapplyMem0RmValue:3;	// qr_sphere_if_qapply_mem_0_rm_value
		uint32 reserved3:1;
		uint32 qrSphereIfQapplyMem1RmValue:3;	// qr_sphere_if_qapply_mem_1_rm_value
		uint32 reserved4:1;
		uint32 qrSphereIfQapplyMem2RmValue:3;	// qr_sphere_if_qapply_mem_2_rm_value
		uint32 reserved5:1;
		uint32 qrSphereIfQapplyMem3RmValue:3;	// qr_sphere_if_qapply_mem_3_rm_value
		uint32 reserved6:1;
		uint32 qrSphereIfQrMem0RmValue:3;	// qr_sphere_if_qr_mem_0_rm_value
		uint32 reserved7:1;
	} bitFields;
} RegPhyRxFdPhyRxfdReg79_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG7A 0x1E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qrSphereIfQrMem1RmValue:3;	// qr_sphere_if_qr_mem_1_rm_value
		uint32 reserved0:1;
		uint32 qrSphereIfQrMem2RmValue:3;	// qr_sphere_if_qr_mem_2_rm_value
		uint32 reserved1:1;
		uint32 qrSphereIfQrMem3RmValue:3;	// qr_sphere_if_qr_mem_3_rm_value
		uint32 reserved2:1;
		uint32 qrSortMemRmValue:3;	// qr_sort_mem_rm_value
		uint32 reserved3:1;
		uint32 qrSortMemSphereRmValue:3;	// qr_sort_mem_rm_value
		uint32 reserved4:1;
		uint32 qrNcoMemRmValue:3;	// qr_nco_mem_rm_value
		uint32 reserved5:9;
	} bitFields;
} RegPhyRxFdPhyRxfdReg7A_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG7B 0x1EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qrStartAddr:10;	// qr_start_addr
		uint32 qrEndAddr:10;	// qr_end_addr
		uint32 qrStartAddr1Ss:10;	// qr_start_addr_1ss
		uint32 reserved0:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg7B_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG7C 0x1F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qrEndAddr1Ss:10;	// qr_end_addr_1ss
		uint32 svdCsdStartAddr:10;	// svd_csd_start_addr
		uint32 svdCsdEndAddr:10;	// svd_csd_end_addr
		uint32 reserved0:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg7C_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG7D 0x1F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 svdTransCsdStartAddr:10;	// svd_trans_csd_start_addr
		uint32 svdTransCsdEndAddr:10;	// svd_trans_csd_end_addr
		uint32 svdQrStartAddr:10;	// svd_qr_start_addr
		uint32 reserved0:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg7D_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG7E 0x1F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 svdQrEndAddr:10;	// svd_qr_end_addr
		uint32 svdStartAddrImplicit:10;	// svd_start_addr_implicit
		uint32 svdEndAddrImplicit:10;	// svd_end_addr_implicit
		uint32 reserved0:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg7E_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG7F 0x1FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 svdMultByQEndAddr:10;	// svd_mult_by_q_end_addr
		uint32 svdAddrForIndication:10;	// svd_addr_for_indication
		uint32 compress4AntStartAddr:10;	// compress_4ant_start_addr
		uint32 reserved0:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg7F_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG80 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compress4AntEndAddr:10;	// compress_4ant_end_addr
		uint32 compress3AntStartAddr:10;	// compress_3ant_start_addr
		uint32 compress3AntEndAddr:10;	// compress_3ant_end_addr
		uint32 reserved0:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg80_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG81 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compress2AntStartAddr:10;	// compress_2ant_start_addr
		uint32 compress2AntEndAddr:10;	// compress_2ant_end_addr
		uint32 decompress4AntStartAddr:10;	// decompress_4ant_start_addr
		uint32 reserved0:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg81_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG82 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decompress4AntEndAddr:10;	// decompress_4ant_end_addr
		uint32 decompress3AntStartAddr:10;	// decompress_3ant_start_addr
		uint32 decompress3AntEndAddr:10;	// decompress_3ant_end_addr
		uint32 reserved0:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg82_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG83 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decompress2AntStartAddr:10;	// decompress_2ant_start_addr
		uint32 decompress2AntEndAddr:10;	// decompress_2ant_end_addr
		uint32 stbcStartAddr:10;	// stbc_start_addr
		uint32 reserved0:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg83_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG84 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stbcEndAddr:10;	// stbc_end_addr
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxFdPhyRxfdReg84_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG86 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compressedFourierPhase0:14;	// compressed_fourier_phase_0
		uint32 reserved0:2;
		uint32 compressedFourierPhase1:14;	// compressed_fourier_phase_1
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg86_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG87 0x21C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compressedFourierPhase6:14;	// compressed_fourier_phase_6
		uint32 reserved0:2;
		uint32 compressedFourierPhase7:14;	// compressed_fourier_phase_7
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg87_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG88 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compressedFourierPhase8:14;	// compressed_fourier_phase_8
		uint32 reserved0:2;
		uint32 compressedFourierPhase9:14;	// compressed_fourier_phase_9
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg88_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG89 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compressedFourierPhase10:14;	// compressed_fourier_phase_10
		uint32 reserved0:2;
		uint32 compressedFourierPhase11:14;	// compressed_fourier_phase_11
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxFdPhyRxfdReg89_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG8A 0x228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ceTable6Start:10;	// channel estimation (smoothing) HT and VHT 1SS 20M table start address
		uint32 reserved0:2;
		uint32 ceTableEndAddr:10;	// channel estimation (smoothing) table end address
		uint32 reserved1:10;
	} bitFields;
} RegPhyRxFdPhyRxfdReg8A_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG8B 0x22C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ncbPrank4MaskAn0:8;	// 20M-scrambler pilots mask rank 4 antenna0
		uint32 ncbPrank4MaskAn1:8;	// 20M-scrambler pilots mask rank 4 antenna1
		uint32 ncbPrank4MaskAn2:8;	// 20M-scrambler pilots mask rank 4 antenna2
		uint32 ncbPrank4MaskAn3:8;	// 20M-scrambler pilots mask rank 4 antenna3
	} bitFields;
} RegPhyRxFdPhyRxfdReg8B_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG8C 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcbPrank4MaskAn0:8;	// 40M-scrambler pilots mask rank 4 antenna0
		uint32 fcbPrank4MaskAn1:8;	// 40M-scrambler pilots mask rank 4 antenna1
		uint32 fcbPrank4MaskAn2:8;	// 40M-scrambler pilots mask rank 4 antenna2
		uint32 fcbPrank4MaskAn3:8;	// 40M-scrambler pilots mask rank 4 antenna3
	} bitFields;
} RegPhyRxFdPhyRxfdReg8C_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG8D 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cb80Prank1Mask:8;	// 80M-scrambler pilots mask rank 1
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxFdPhyRxfdReg8D_u;

/*REG_PHY_RX_FD_PHY_RXFD_IF08E 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qrMaxPrescale1SsQr:3;	// qr_max_prescale_1ss_qr
		uint32 reserved0:1;
		uint32 qrMaxPrescale2SsQr:3;	// qr_max_prescale_2ss_qr
		uint32 reserved1:1;
		uint32 qrMaxPrescale3SsQr:3;	// qr_max_prescale_3ss_qr
		uint32 reserved2:1;
		uint32 qrMaxPrescale4SsQr:3;	// qr_max_prescale_4ss_qr
		uint32 reserved3:1;
		uint32 qrMaxPrescale1SsQapply:3;	// qr_max_prescale_1ss_qapply
		uint32 reserved4:1;
		uint32 qrMaxPrescale2SsQapply:3;	// qr_max_prescale_2ss_qapply
		uint32 reserved5:1;
		uint32 qrMaxPrescale3SsQapply:3;	// qr_max_prescale_3ss_qapply
		uint32 reserved6:1;
		uint32 qrMaxPrescale4SsQapply:3;	// qr_max_prescale_4ss_qapply
		uint32 reserved7:1;
	} bitFields;
} RegPhyRxFdPhyRxfdIf08E_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG8F 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 linearFifoRamRm:3;	// linear_fifo_ram_rm
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxFdPhyRxfdReg8F_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG90 0x240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fdVhtSigbJumpAdd:4;	// GSM - fd_vht_sigb_jump_add
		uint32 tdVhtSigbJumpAdd:4;	// GSM - td_vht_sigb_jump_add
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxFdPhyRxfdReg90_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG91 0x244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cb80ShortCpTimShift:5;	// short cyclic prefix timing shift - cb80
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxFdPhyRxfdReg91_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG92 0x248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 chEstiSmoothRmValue:3;	// ch_esti_smooth_rm_value
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxFdPhyRxfdReg92_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG93 0x24C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 long3Cb80Prank1Mask:8;	// 80M long3 - scrambler pilots mask rank 1
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxFdPhyRxfdReg93_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG94 0x250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGpr1:32;	// RXFD spare reserved register
	} bitFields;
} RegPhyRxFdPhyRxfdReg94_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG95 0x254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGpr2:32;	// RXFD spare reserved register
	} bitFields;
} RegPhyRxFdPhyRxfdReg95_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG96 0x258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGpr3:32;	// RXFD spare reserved register
	} bitFields;
} RegPhyRxFdPhyRxfdReg96_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG97 0x25C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGpr4:32;	// RXFD spare reserved register
	} bitFields;
} RegPhyRxFdPhyRxfdReg97_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG98 0x260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom12Ss0I:8;	// tx_ct_sm_matrix_custom1_2ss_0_i
		uint32 txCtSmMatrixCustom12Ss0Q:8;	// tx_ct_sm_matrix_custom1_2ss_0_q
		uint32 txCtSmMatrixCustom12Ss1I:8;	// tx_ct_sm_matrix_custom1_2ss_1_i
		uint32 txCtSmMatrixCustom12Ss1Q:8;	// tx_ct_sm_matrix_custom1_2ss_1_q
	} bitFields;
} RegPhyRxFdPhyRxfdReg98_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG99 0x264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom12Ss2I:8;	// tx_ct_sm_matrix_custom1_2ss_2_i
		uint32 txCtSmMatrixCustom12Ss2Q:8;	// tx_ct_sm_matrix_custom1_2ss_2_q
		uint32 txCtSmMatrixCustom12Ss3I:8;	// tx_ct_sm_matrix_custom1_2ss_3_i
		uint32 txCtSmMatrixCustom12Ss3Q:8;	// tx_ct_sm_matrix_custom1_2ss_3_q
	} bitFields;
} RegPhyRxFdPhyRxfdReg99_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG9A 0x268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom12Ss4I:8;	// tx_ct_sm_matrix_custom1_2ss_4_i
		uint32 txCtSmMatrixCustom12Ss4Q:8;	// tx_ct_sm_matrix_custom1_2ss_4_q
		uint32 txCtSmMatrixCustom12Ss5I:8;	// tx_ct_sm_matrix_custom1_2ss_5_i
		uint32 txCtSmMatrixCustom12Ss5Q:8;	// tx_ct_sm_matrix_custom1_2ss_5_q
	} bitFields;
} RegPhyRxFdPhyRxfdReg9A_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG9B 0x26C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom12Ss6I:8;	// tx_ct_sm_matrix_custom1_2ss_6_i
		uint32 txCtSmMatrixCustom12Ss6Q:8;	// tx_ct_sm_matrix_custom1_2ss_6_q
		uint32 txCtSmMatrixCustom12Ss7I:8;	// tx_ct_sm_matrix_custom1_2ss_7_i
		uint32 txCtSmMatrixCustom12Ss7Q:8;	// tx_ct_sm_matrix_custom1_2ss_7_q
	} bitFields;
} RegPhyRxFdPhyRxfdReg9B_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG9C 0x270 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom22Ss0I:8;	// tx_ct_sm_matrix_custom2_2ss_0_i
		uint32 txCtSmMatrixCustom22Ss0Q:8;	// tx_ct_sm_matrix_custom2_2ss_0_q
		uint32 txCtSmMatrixCustom22Ss1I:8;	// tx_ct_sm_matrix_custom2_2ss_1_i
		uint32 txCtSmMatrixCustom22Ss1Q:8;	// tx_ct_sm_matrix_custom2_2ss_1_q
	} bitFields;
} RegPhyRxFdPhyRxfdReg9C_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG9D 0x274 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom22Ss2I:8;	// tx_ct_sm_matrix_custom2_2ss_2_i
		uint32 txCtSmMatrixCustom22Ss2Q:8;	// tx_ct_sm_matrix_custom2_2ss_2_q
		uint32 txCtSmMatrixCustom22Ss3I:8;	// tx_ct_sm_matrix_custom2_2ss_3_i
		uint32 txCtSmMatrixCustom22Ss3Q:8;	// tx_ct_sm_matrix_custom2_2ss_3_q
	} bitFields;
} RegPhyRxFdPhyRxfdReg9D_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG9E 0x278 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom22Ss4I:8;	// tx_ct_sm_matrix_custom2_2ss_4_i
		uint32 txCtSmMatrixCustom22Ss4Q:8;	// tx_ct_sm_matrix_custom2_2ss_4_q
		uint32 txCtSmMatrixCustom22Ss5I:8;	// tx_ct_sm_matrix_custom2_2ss_5_i
		uint32 txCtSmMatrixCustom22Ss5Q:8;	// tx_ct_sm_matrix_custom2_2ss_5_q
	} bitFields;
} RegPhyRxFdPhyRxfdReg9E_u;

/*REG_PHY_RX_FD_PHY_RXFD_REG9F 0x27C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom22Ss6I:8;	// tx_ct_sm_matrix_custom2_2ss_6_i
		uint32 txCtSmMatrixCustom22Ss6Q:8;	// tx_ct_sm_matrix_custom2_2ss_6_q
		uint32 txCtSmMatrixCustom22Ss7I:8;	// tx_ct_sm_matrix_custom2_2ss_7_i
		uint32 txCtSmMatrixCustom22Ss7Q:8;	// tx_ct_sm_matrix_custom2_2ss_7_q
	} bitFields;
} RegPhyRxFdPhyRxfdReg9F_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGA0 0x280 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom13Ss0I:8;	// tx_ct_sm_matrix_custom1_3ss_0_i
		uint32 txCtSmMatrixCustom13Ss0Q:8;	// tx_ct_sm_matrix_custom1_3ss_0_q
		uint32 txCtSmMatrixCustom13Ss1I:8;	// tx_ct_sm_matrix_custom1_3ss_1_i
		uint32 txCtSmMatrixCustom13Ss1Q:8;	// tx_ct_sm_matrix_custom1_3ss_1_q
	} bitFields;
} RegPhyRxFdPhyRxfdRega0_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGA1 0x284 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom13Ss2I:8;	// tx_ct_sm_matrix_custom1_3ss_2_i
		uint32 txCtSmMatrixCustom13Ss2Q:8;	// tx_ct_sm_matrix_custom1_3ss_2_q
		uint32 txCtSmMatrixCustom13Ss3I:8;	// tx_ct_sm_matrix_custom1_3ss_3_i
		uint32 txCtSmMatrixCustom13Ss3Q:8;	// tx_ct_sm_matrix_custom1_3ss_3_q
	} bitFields;
} RegPhyRxFdPhyRxfdRega1_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGA2 0x288 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom13Ss4I:8;	// tx_ct_sm_matrix_custom1_3ss_4_i
		uint32 txCtSmMatrixCustom13Ss4Q:8;	// tx_ct_sm_matrix_custom1_3ss_4_q
		uint32 txCtSmMatrixCustom13Ss5I:8;	// tx_ct_sm_matrix_custom1_3ss_5_i
		uint32 txCtSmMatrixCustom13Ss5Q:8;	// tx_ct_sm_matrix_custom1_3ss_5_q
	} bitFields;
} RegPhyRxFdPhyRxfdRega2_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGA3 0x28C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom13Ss6I:8;	// tx_ct_sm_matrix_custom1_3ss_6_i
		uint32 txCtSmMatrixCustom13Ss6Q:8;	// tx_ct_sm_matrix_custom1_3ss_6_q
		uint32 txCtSmMatrixCustom13Ss7I:8;	// tx_ct_sm_matrix_custom1_3ss_7_i
		uint32 txCtSmMatrixCustom13Ss7Q:8;	// tx_ct_sm_matrix_custom1_3ss_7_q
	} bitFields;
} RegPhyRxFdPhyRxfdRega3_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGA4 0x290 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom13Ss8I:8;	// tx_ct_sm_matrix_custom1_3ss_8_i
		uint32 txCtSmMatrixCustom13Ss8Q:8;	// tx_ct_sm_matrix_custom1_3ss_8_q
		uint32 txCtSmMatrixCustom13Ss9I:8;	// tx_ct_sm_matrix_custom1_3ss_9_i
		uint32 txCtSmMatrixCustom13Ss9Q:8;	// tx_ct_sm_matrix_custom1_3ss_9_q
	} bitFields;
} RegPhyRxFdPhyRxfdRega4_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGA5 0x294 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom13Ss10I:8;	// tx_ct_sm_matrix_custom1_3ss_10_i
		uint32 txCtSmMatrixCustom13Ss10Q:8;	// tx_ct_sm_matrix_custom1_3ss_10_q
		uint32 txCtSmMatrixCustom13Ss11I:8;	// tx_ct_sm_matrix_custom1_3ss_11_i
		uint32 txCtSmMatrixCustom13Ss11Q:8;	// tx_ct_sm_matrix_custom1_3ss_11_q
	} bitFields;
} RegPhyRxFdPhyRxfdRega5_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGA6 0x298 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom23Ss0I:8;	// tx_ct_sm_matrix_custom2_3ss_0_i
		uint32 txCtSmMatrixCustom23Ss0Q:8;	// tx_ct_sm_matrix_custom2_3ss_0_q
		uint32 txCtSmMatrixCustom23Ss1I:8;	// tx_ct_sm_matrix_custom2_3ss_1_i
		uint32 txCtSmMatrixCustom23Ss1Q:8;	// tx_ct_sm_matrix_custom2_3ss_1_q
	} bitFields;
} RegPhyRxFdPhyRxfdRega6_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGA7 0x29C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom23Ss2I:8;	// tx_ct_sm_matrix_custom2_3ss_2_i
		uint32 txCtSmMatrixCustom23Ss2Q:8;	// tx_ct_sm_matrix_custom2_3ss_2_q
		uint32 txCtSmMatrixCustom23Ss3I:8;	// tx_ct_sm_matrix_custom2_3ss_3_i
		uint32 txCtSmMatrixCustom23Ss3Q:8;	// tx_ct_sm_matrix_custom2_3ss_3_q
	} bitFields;
} RegPhyRxFdPhyRxfdRega7_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGA8 0x2A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom23Ss4I:8;	// tx_ct_sm_matrix_custom2_3ss_4_i
		uint32 txCtSmMatrixCustom23Ss4Q:8;	// tx_ct_sm_matrix_custom2_3ss_4_q
		uint32 txCtSmMatrixCustom23Ss5I:8;	// tx_ct_sm_matrix_custom2_3ss_5_i
		uint32 txCtSmMatrixCustom23Ss5Q:8;	// tx_ct_sm_matrix_custom2_3ss_5_q
	} bitFields;
} RegPhyRxFdPhyRxfdRega8_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGA9 0x2A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom23Ss6I:8;	// tx_ct_sm_matrix_custom2_3ss_6_i
		uint32 txCtSmMatrixCustom23Ss6Q:8;	// tx_ct_sm_matrix_custom2_3ss_6_q
		uint32 txCtSmMatrixCustom23Ss7I:8;	// tx_ct_sm_matrix_custom2_3ss_7_i
		uint32 txCtSmMatrixCustom23Ss7Q:8;	// tx_ct_sm_matrix_custom2_3ss_7_q
	} bitFields;
} RegPhyRxFdPhyRxfdRega9_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGAA 0x2A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom23Ss8I:8;	// tx_ct_sm_matrix_custom2_3ss_8_i
		uint32 txCtSmMatrixCustom23Ss8Q:8;	// tx_ct_sm_matrix_custom2_3ss_8_q
		uint32 txCtSmMatrixCustom23Ss9I:8;	// tx_ct_sm_matrix_custom2_3ss_9_i
		uint32 txCtSmMatrixCustom23Ss9Q:8;	// tx_ct_sm_matrix_custom2_3ss_9_q
	} bitFields;
} RegPhyRxFdPhyRxfdRegaa_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGAB 0x2AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtSmMatrixCustom23Ss10I:8;	// tx_ct_sm_matrix_custom2_3ss_10_i
		uint32 txCtSmMatrixCustom23Ss10Q:8;	// tx_ct_sm_matrix_custom2_3ss_10_q
		uint32 txCtSmMatrixCustom23Ss11I:8;	// tx_ct_sm_matrix_custom2_3ss_11_i
		uint32 txCtSmMatrixCustom23Ss11Q:8;	// tx_ct_sm_matrix_custom2_3ss_11_q
	} bitFields;
} RegPhyRxFdPhyRxfdRegab_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGBE 0x2F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 avgSnrAddFactor:12;	// avg_snr_add_factor
		uint32 avgSnrMultFactor30:15;	// avg_snr_mult_factor for 30 groups
		uint32 reserved0:5;
	} bitFields;
} RegPhyRxFdPhyRxfdRegbe_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGBF 0x2FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deltaSnrAddFactor:12;	// avg_snr_add_factor
		uint32 avgSnrMultFactor62:15;	// avg_snr_mult_factor for 62 groups
		uint32 reserved0:5;
	} bitFields;
} RegPhyRxFdPhyRxfdRegbf_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGC0 0x300 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fftRndHb2080:2;	// fft output round bit selection in HB2080
		uint32 fftRndHb4080:2;	// fft output round bit selection in HB4080
		uint32 reserved0:1;
		uint32 bypassHaltSphere:1;	// sphere will not be halted during operation (in rxfd_csm)
		uint32 reserved1:26;
	} bitFields;
} RegPhyRxFdPhyRxfdRegc0_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGC1 0x304 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muQrDecompress4AntStartAddr:10;	// mu_qr_decompress_4ant_start_addr
		uint32 reserved0:2;
		uint32 muQrDecompress4AntEndAddr:10;	// mu_qr_decompress_4ant_end_addr
		uint32 reserved1:10;
	} bitFields;
} RegPhyRxFdPhyRxfdRegc1_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGC2 0x308 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muQrDecompress3AntStartAddr:10;	// mu_qr_decompress_3ant_start_addr
		uint32 reserved0:2;
		uint32 muQrDecompress3AntEndAddr:10;	// mu_qr_decompress_3ant_end_addr
		uint32 reserved1:10;
	} bitFields;
} RegPhyRxFdPhyRxfdRegc2_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGC3 0x30C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muQrDecompress2AntStartAddr:10;	// mu_qr_decompress_2ant_start_addr
		uint32 reserved0:2;
		uint32 muQrDecompress2AntEndAddr:10;	// mu_qr_decompress_2ant_end_addr
		uint32 reserved1:10;
	} bitFields;
} RegPhyRxFdPhyRxfdRegc3_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGC4 0x310 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muQrQapplyStartAddr:10;	// mu_qr_decompress_2ant_start_addr
		uint32 reserved0:2;
		uint32 muQrQapplyEndAddr:10;	// mu_qr_decompress_2ant_end_addr
		uint32 reserved1:10;
	} bitFields;
} RegPhyRxFdPhyRxfdRegc4_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGC5 0x314 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muPhasesRmValue:3;	// mu_phases_rm_value
		uint32 reserved0:1;
		uint32 muTrainEffRateRmValue:3;	// mu_train_eff_rate_rm_value
		uint32 reserved1:1;
		uint32 muDeltaSnrRmValue:3;	// mu_delta_snr_rm_value
		uint32 reserved2:1;
		uint32 hypRxfdESnrGamaRamRm:3;	// hyp_rxfd_e_snr_gama_ram_rm
		uint32 reserved3:17;
	} bitFields;
} RegPhyRxFdPhyRxfdRegc5_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGC6 0x318 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muQr4X2StartAddr:10;	// mu_qr_qapply_4x2_start_addr
		uint32 reserved0:2;
		uint32 muQr4X2EndAddr:10;	// mu_qr_qapply_4x2_end_addr
		uint32 reserved1:10;
	} bitFields;
} RegPhyRxFdPhyRxfdRegc6_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGC7 0x31C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deltaSnrMultFactor:15;	// delta_snr_mult_factor
		uint32 reserved0:1;
		uint32 avgSnrMultFactor122:15;	// avg_snr_mult_factor122
		uint32 reserved1:1;
	} bitFields;
} RegPhyRxFdPhyRxfdRegc7_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGC8 0x320 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 avgSnrMultFactor52:15;	// avg_snr_mult_factor32
		uint32 reserved0:1;
		uint32 avgSnrMultFactor58:15;	// avg_snr_mult_factor58
		uint32 reserved1:1;
	} bitFields;
} RegPhyRxFdPhyRxfdRegc8_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGC9 0x324 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eSnrLog10BetaFxp1:8;	// e_snr_log10_beta_fxp_1
		uint32 eSnrLog10BetaFxp23:8;	// e_snr_log10_beta_fxp_2_3
		uint32 eSnrLog10BetaFxp45:8;	// e_snr_log10_beta_fxp_4_5
		uint32 eSnrLog10BetaFxp678:8;	// e_snr_log10_beta_fxp_6_7_8
	} bitFields;
} RegPhyRxFdPhyRxfdRegc9_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGCA 0x328 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eSnrLog10BetaFxp910:8;	// e_snr_log10_beta_fxp_9_10
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxFdPhyRxfdRegca_u;

/*REG_PHY_RX_FD_PHY_RXFD_REGCB 0x32C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfHtCodebookInfo:2;	// bf_ht_codebook_info
		uint32 bfVhtCodebookInfo:1;	// bf_ht_codebook_info
		uint32 bfVhtMuCodebookInfo:1;	// bf_ht_codebook_info
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxFdPhyRxfdRegcb_u;



#endif // _PHY_RX_FD_REGS_H_
