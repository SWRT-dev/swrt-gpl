
/***********************************************************************************
File:				PhyRxTdIfRiscPage0FdRegs.h
Module:				phyRxTdIfRiscPage0Fd
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_RX_TD_IF_RISC_PAGE_0_FD_REGS_H_
#define _PHY_RX_TD_IF_RISC_PAGE_0_FD_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS                             MEMORY_MAP_UNIT_51_BASE_ADDRESS
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF100                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10400)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF101                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10404)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF102                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10408)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF103                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1040C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF104                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10410)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF_RISC_CAUSE_INT    (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10414)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF106                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10418)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF107                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1041C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF108                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10420)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF109                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10424)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF10B                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1042C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF10C                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10430)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF10D                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10434)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF10E                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10438)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF10F                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1043C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF110                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10440)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF111                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10444)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF112                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10448)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF113                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1044C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF115                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10454)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF116                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10458)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF117                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1045C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF118                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10460)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF119                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10464)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF11A                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10468)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF11B                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1046C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF11D                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10474)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF11E                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10478)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF11F                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1047C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF120                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10480)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF121                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10484)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF122                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10488)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF123                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1048C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF124                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10490)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF125                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10494)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF126                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10498)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF127                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1049C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF128                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104A0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF129                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104A4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12A                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104A8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12B                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104AC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12C                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104B0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12D                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104B4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12E                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104B8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12F                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104BC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF130                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104C0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF131                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104C4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF132                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104C8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF133                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104CC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF134                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104D0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF135                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104D4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF136                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104D8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF137                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104DC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF138                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104E0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF139                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104E4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13A                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104E8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13B                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104EC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13C                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104F0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13D                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104F4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13E                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104F8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13F                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x104FC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF140                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10500)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF141                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10504)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF142                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10508)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF143                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1050C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF144                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10510)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF145                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10514)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF146                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10518)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF147                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1051C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF148                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10520)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF149                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10524)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14A                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10528)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14B                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1052C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14C                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10530)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14D                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10534)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14E                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10538)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14F                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1053C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF150                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10540)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF151                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10544)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF152                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10548)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF153                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1054C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF154                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10550)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF155                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10554)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF156                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10558)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF157                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1055C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF158                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10560)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF159                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10564)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF15A                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10568)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF15B                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x1056C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF15C                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10570)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF15D                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10574)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF15E                (PHY_RX_TD_IF_RISC_PAGE_0_FD_BASE_ADDRESS + 0x10578)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF100 0x10400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg:16;	// FD sub blocks sw reset , One bit per sub block. Resets only the core logic. Programming model is unchaged. Writing "0" resets the sub block and holds it reset. Writing "1" resets the sub block and releases the resets autumaticaly. , [0]- fd csm , [1]- fft_0 , [2]- fft_1 , [3]- fft_2 , [4]-fft_3 , [5]- bin chooser , [6]- nco , [7]- pilot scrambler , [8]- phase tracking , [8]- organizer , [9]- channel tracking , [10]- channel smoothing , [11]- timing estimation , [12]- gsm , [13]- slicer
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf100_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF101 0x10404 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blocksEn:16;	// FD_sub blocks en. One bit per sub block. Enables or disables the sub block  , [0]- fd csm , [1]- fft_0 , [2]- fft_1 , [3]- fft_2 , [4]- fft_2 , [5]- bin chooser , [6]- nco , [7]- pilot scrambler , [8]- phase tracking , [9]- organizer , [10]- qr , [11]- channel smoothing , [12]- timing estimation , [13]- gsm , [14]- slicer
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf101_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF102 0x10408 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate:16;	// A SW reset procedure will be initiated by writing to this register to the bits that equal 1
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf102_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF103 0x1040C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmSwResetMask:16;	// Which block to reset at the falling edge of the global enable. One bit per sub block - same mapping as in sw_reset_n_reg register
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf103_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF104 0x10410 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscResetModemP:1;	// Reset Modem Command From Risc. pulse
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf104_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF_RISC_CAUSE_INT 0x10414 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfdRiscCauseIntClear_rxfdRiscCauseInt:9;	// RXFD Risc Cause int - , bit[7] - effective snr , bit[6] - last symbol , bit[5] - fd_stm_aborted , bit[4] - td_stm_aborted , bit[3] - fd symbol ready , bit[2] - sym_strobe to td , bit[1] - green field packet , bit[0] - high throughput packet
		uint32 reserved0:23;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIfRiscCauseInt_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF106 0x10418 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfdRiscIntEnable:9;	// RXFD Risc Int Enable
		uint32 reserved0:23;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf106_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF107 0x1041C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmTdNextState:4;	// GSM Td Next State
		uint32 gsmFdNextState:4;	// GSM Fd Next State
		uint32 rxfdCsm:5;	// RXFD control_stm
		uint32 reserved0:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf107_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF108 0x10420 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 antennaEn:4;	// Sanity Risc Antenna Enable , Risc Decision , (bit per antenna) 
		uint32 antennaEnValid:1;	// Sanity Risc Antenna Enable Valid
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf108_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF109 0x10424 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctDataMu:4;	// Channel Tracking data Coefficient Mu
		uint32 ctPilotMu:4;	// Channel Tracking pilot Coefficient Mu
		uint32 ftShifter:4;	// Frequency Tracking shifter
		uint32 lltfChSmoothBypass:1;	// L-LTF Channel smoothing Bypass
		uint32 lltfTeBypass:1;	// L-LTF timing estimation bypass
		uint32 htltfTeBypass:1;	// HT-LTF timing estimation bypass
		uint32 htltfChSmoothBypass:1;	// HT-LTF Channel smoothing Bypass
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf109_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF10B 0x1042C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phaseSmoothingSize:5;	// Phase Smoothing Size
		uint32 reserved0:3;
		uint32 timingSmoothingSize:5;	// Timing Smoothing Size
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf10B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF10C 0x10430 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 hModeCntRisc:3;	// h_mode counter
		uint32 reserved1:25;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf10C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF10D 0x10434 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscTdGlobalEn:1;	// risc override the td global enable
		uint32 riscFdGlobalEn:1;	// risc override the fd global enable
		uint32 riscBeGlobalEn:1;	// risc override the be global enable
		uint32 reserved0:1;
		uint32 riscTdGlobalEnSel:1;	// TD global enable selection: , 0- hw , 1-sw risc
		uint32 riscFdGlobalEnSel:1;	// FD global enable selection: , 0- hw , 1-sw risc
		uint32 riscBeGlobalEnSel:1;	// BE global enable selection: , 0- hw , 1-sw risc
		uint32 reserved1:25;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf10D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF10E 0x10438 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htMode:1;	// ht mode: , 0-NOT ht , 1-HT mode
		uint32 gfDetected:1;	// green field detection result: , 1- green field detected
		uint32 vhtMode:1;	// vht mode
		uint32 reserved0:1;
		uint32 htMetrics:11;	// HT detector metric information
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf10E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF10F 0x1043C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mcs32Rev0:1;	// mcs32 backward compatible for rev_0
		uint32 gfNcoBpEn:1;	// enable nco_bypass for ht_detector input data in L_sig (do not rotate the data_bins for the ht detector) 
		uint32 htNcoBpEn:1;	// enable nco_bypass for ht_detector input data in ht_sig1 (do not rotate the data_bins for the ht detector) 
		uint32 vhtNcoBpEn:1;	// enable nco_bypass for ht_detector input data in ht_sig2 (do not rotate the data_bins for the ht detector) 
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf10F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF110 0x10440 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGpo:16;	// gpo , for general use at the top level
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf110_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF111 0x10444 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscNumSymbols:15;	// number of symbols by RISC (override air time calculation)
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf111_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF112 0x10448 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:12;
		uint32 riscNumSymbolsValid:1;	// number of symbols by RISC valid
		uint32 riscNumSymbolsSel:1;	// Select number of symbols source , 0-Hw , 1-Risc
		uint32 fdTimRevertEn:1;	// FD Timing revert enable
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf112_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF113 0x1044C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 teOffset:4;	// timing estimation offset
		uint32 reserved1:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf113_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF115 0x10454 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGpi:16;	// gpi for read only 
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf115_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF116 0x10458 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGpi2:16;	// gpi2 for read only 
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf116_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF117 0x1045C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdLowGpr:16;	// hyp_rxfd_low_gpr
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf117_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF118 0x10460 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdHighGpr:16;	// hyp_rxfd_high_gpr
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf118_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF119 0x10464 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timFixAn0:7;	// fd_timing result antenna_0
		uint32 reserved0:1;
		uint32 timFixAn1:7;	// fd_timing result antenna_1
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf119_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF11A 0x10468 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timFixAn2:7;	// fd_timing result antenna_2
		uint32 reserved0:1;
		uint32 timFixAn3:7;	// fd_timing result antenna_3
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf11A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF11B 0x1046C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:14;
		uint32 freqTrckGclkDebugBypass:1;	// freq_trck_gclk_debug_bypass
		uint32 riscIf2BiuGclkBypass:1;	// risc_if2biu gclk bypass
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf11B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF11D 0x10474 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkDebugBypass:16;	// One bit per gator. , [0]- csm gator , [1]- fft_0 gator , [2]- fft_1 gator , [3]- fft_2 gator , [4]- fft_2 gator , [5]- bin chooser gator , [6]- nco gator , [7]- pilot scrambler gator , [8]- phase tracking  gator , [9]- organizer gator , [10]- QR gator , [11] - channel estimation gator , [12] - timing estimation gator , [13] - gsm gator , [14] - avg_4_2band , [15] - slicer 
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf11D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF11E 0x10478 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timingShiftAccum:7;	// timing shift accumulator - for metrics
		uint32 reserved0:25;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf11E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF11F 0x1047C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 an0Magnitude:16;	// fd_timing - an0 magnitude value
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf11F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF120 0x10480 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 an1Magnitude:16;	// fd_timing - an1 magnitude value
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf120_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF121 0x10484 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 an2Magnitude:16;	// fd_timing - an2 magnitude value
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf121_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF122 0x10488 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 isHtGfMagnitude:16;	// gf/ht/vht detection magnitude value
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf122_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF123 0x1048C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseInv:15;	// value of 1/noise to the effective_snr module
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf123_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF124 0x10490 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 meanExpResult:9;	// effective snr module result - mean_exp_result
		uint32 effectiveSnrMcs:7;	// effective snr module result - effective_snr_mcs withput the msb (rank bit)
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf124_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF125 0x10494 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 pgc1Ant0:6;	// agc calculation - pgc1 ant0 
		uint32 pgc2Ant0:6;	// agc calculation - pgc2 ant0 
		uint32 reserved1:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf125_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF126 0x10498 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pgc3Ant0:6;	// agc calculation - pgc3 ant0 
		uint32 reserved0:2;
		uint32 lnaAnt0:5;	// agc calculation - lna ant0 
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf126_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF127 0x1049C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 pgc1Ant1:6;	// agc calculation - pgc1 ant1
		uint32 pgc2Ant1:6;	// agc calculation - pgc2 ant1
		uint32 reserved1:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf127_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF128 0x104A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pgc3Ant1:6;	// agc calculation - pgc3 ant1
		uint32 reserved0:2;
		uint32 lnaAnt1:5;	// agc calculation - lna ant1
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf128_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF129 0x104A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 pgc1Ant2:6;	// agc calculation - pgc1 ant2
		uint32 pgc2Ant2:6;	// agc calculation - pgc2 ant2
		uint32 reserved1:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf129_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12A 0x104A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pgc3Ant2:6;	// agc calculation - pgc3 ant2
		uint32 reserved0:2;
		uint32 lnaAnt2:5;	// agc calculation - lna ant2
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf12A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12B 0x104AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 implChannPreFlatCorrectionAnt0:8;	// flat correction calculation for implicit channel preparation antenna0
		uint32 implChannPreFlatCorrectionAnt1:8;	// flat correction calculation for implicit channel preparation antenna1
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf12B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12C 0x104B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 implChannPreFlatCorrectionAnt2:8;	// flat correction calculation for implicit channel preparation antenna2
		uint32 implChannPreFlatCorrectionAnt3:8;	// flat correction calculation for implicit channel preparation antenna3
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf12C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12D 0x104B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 flatCorrectionCalcChPreUnitDone:1;	// this signal rise when genrisc finnish correction calculation 
		uint32 explImplNMode:1;	// 0 - implicit bf mode , 1- explicit bf mode
		uint32 genriscBfEn:1;	// bf enable
		uint32 genriscUploadChEn:1;	// bf_rx_parser upload the channel
		uint32 mcsFeedbackDisable:1;	// 1 - ignore e_snr mcs decision
		uint32 reserved1:25;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf12D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12E 0x104B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sphereMaxLlr:10;	// sphere_max_llr
		uint32 reserved0:2;
		uint32 sphereNoisePowerProgModel:3;	// sphere_noise_power_prog_model
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf12E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF12F 0x104BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sphereTreePartThr:3;	// sphere_tree_part_thr
		uint32 reserved0:1;
		uint32 sphereLlrThreshold:5;	// sphere_llr_threshold
		uint32 sphereNoisePowerTreePartThr:1;	// sphere_noise_power_tree_part_thr
		uint32 reserved1:1;
		uint32 noiseTrkGainInMin:5;	// noise_trk_gain_in_min
		uint32 reserved2:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf12F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF130 0x104C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctDataE4InForIir0:16;	// ct_data_e4_in_for_iir0
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf130_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF131 0x104C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctDataE4InForIir1:16;	// ct_data_e4_in_for_iir1
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf131_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF132 0x104C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctDataE4InForIir2:16;	// ct_data_e4_in_for_iir2
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf132_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF133 0x104CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctDataE4InForIir3:16;	// ct_data_e4_in_for_iir3
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf133_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF134 0x104D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseTrkNoiseIdentityReg:6;	// noise_trk_noise_identity_reg
		uint32 reserved0:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf134_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF135 0x104D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 an3Magnitude:16;	// fd_timing - an3 magnitude value
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf135_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF136 0x104D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rfGclkDebugBypass:1;	// Register File gclk_debug_bypass
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf136_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF137 0x104DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timFixLltfValid:1;	// fd_timing l_ltf result valid 
		uint32 timFixHtltfValid:1;	// fd_timing ht_ltf result valid 
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf137_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF138 0x104E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscEnableSphere:1;	// enable sphere
		uint32 riscEnableLinear:1;	// enable linear
		uint32 riscSumLlr:1;	// sum LLRs from linear and sphere
		uint32 reserved0:1;
		uint32 riscForceNtGainCalc:1;	// force noise tracking gain calculation
		uint32 reserved1:5;
		uint32 sphereMode:2;	// sphere_mode 0:regular,1:fastmode1,2:fastmode2
		uint32 reserved2:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf138_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF139 0x104E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 linearViterbiShift:10;	// linear_viterbi_shift
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf139_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13A 0x104E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerLoadGainStream0:12;	// Power load gain for adaptive filter (Tx) stream 0 
		uint32 powerLoadGainReady:1;	// Power load gain for adaptive filter is ready in registers for all streams
		uint32 reserved0:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf13A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13B 0x104EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerLoadGainStream1:12;	// Power load gain for adaptive filter (Tx) stream 1 
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf13B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13C 0x104F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerLoadGainStream2:12;	// Power load gain for adaptive filter (Tx) stream 2 
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf13C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13D 0x104F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerLoadGainStream3:12;	// Power load gain for adaptive filter (Tx) stream 3 
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf13D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13E 0x104F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sigbMuData1Ss:4;	// Channel Tracking Coefficient Mu for SIGB 1ss
		uint32 sigbMuData2Ss:4;	// Channel Tracking Coefficient Mu for SIGB 2ss
		uint32 sigbMuData3Ss:4;	// Channel Tracking Coefficient Mu for SIGB 3ss
		uint32 sigbMuData4Ss:4;	// Channel Tracking Coefficient Mu for SIGB 4ss
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf13E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF13F 0x104FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sphereViterbiShift:10;	// sphere_viterbi_shift
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf13F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF140 0x10500 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bypassSmoothing:4;	// Bypass smoothing - bit per Spatial Stream
		uint32 chEstiSmoothingCsd:1;	// CSD compensation in smoothing unit -remove CSD before smoothing and return value (reverse) after smoothing
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf140_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF141 0x10504 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 pgc1Ant3:6;	// agc calculation - pgc1 ant3 
		uint32 pgc2Ant3:6;	// agc calculation - pgc2 ant3 
		uint32 reserved1:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf141_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF142 0x10508 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pgc3Ant3:6;	// agc calculation - pgc3 ant3 
		uint32 reserved0:2;
		uint32 lnaAnt3:5;	// agc calculation - lna ant3 
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf142_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF143 0x1050C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscStartOrganizer:1;	// risc_start_organizer (set when 'fine' is ready)
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf143_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF144 0x10510 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 payloadSymbolNum:15;	// payload_symbol_number
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf144_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF145 0x10514 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSwResetMask:16;	// Which units to software reset at at the beginning of Tx , [0]- fd csm , [1]- fft_0 , [2]- fft_1 , [3]- fft_2 , [4]-fft_3 , [5]- bin chooser , [6]- nco , [7]- pilot scrambler , [8]- phase tracking , [9]- organizer , [10]- channel tracking , [11]- channel smoothing , [12]- timing estimation , [13]- gsm , [14]- slicer/qr
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf145_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF146 0x10518 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gearShiftSymNum:10;	// The symbol number for changing the mu (Start counting from first DATA symbol)
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf146_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF147 0x1051C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctDataMuGearShift:4;	// Channel Tracking Mu value for data to change after reaching the gear shift symbol
		uint32 ctPilotMuGearShift:4;	// Channel Tracking Mu value for pilots to change after reaching the gear shift symbol
		uint32 ftShifterGearShift:4;	// Frequency Tracking shift value when reaching the gear shift symbol
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf147_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF148 0x10520 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fdSymbolType:4;	// FD symbol type (from GSM)
		uint32 fdBw:2;	// FD bandwidth , 1- 20M , 2 - 40M , 3 - 80M
		uint32 reserved0:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf148_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF149 0x10524 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 avrSnrReportLow:16;	// avr_snr_report_low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf149_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14A 0x10528 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 avrSnrReportHigh:16;	// avr_snr_report_high
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf14A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14B 0x1052C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTrainingMode:1;	// MU training mode
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf14B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14C 0x10530 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muStartTrainingMode:1;	// start MU training operation
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf14C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14D 0x10534 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTrainingDone:1;	// mu training done
		uint32 muTrainingIndex:10;	// mu training index (effective rate memory address)
		uint32 rxEffectiveSnrDoneSticky:1;	// rx_effective_snr_done_sticky
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf14D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14E 0x10538 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUsersValidityReg:16;	// Reflecting the users validity in training. Bit per user
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf14E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF14F 0x1053C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muRiscUsersValidityReg:16;	// The GenRisc reads the mu_users_validity_reg and write back to this register with option of modifying
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf14F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF150 0x10540 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTrainingBw:2;	// mu_training_bw
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf150_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF151 0x10544 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtPwrLDb0:5;	// tx_ct_pwr_l_db0
		uint32 reserved0:3;
		uint32 txCtPwrLDb1:5;	// tx_ct_pwr_l_db1
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf151_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF152 0x10548 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtPwrLDb2:5;	// tx_ct_pwr_l_db2
		uint32 reserved0:3;
		uint32 txCtPwrLDb3:5;	// tx_ct_pwr_l_db3
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf152_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF153 0x1054C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg2:3;	// FD sub blocks sw reset , One bit per sub block. Resets only the core logic. Programming model is unchaged. Writing "0" resets the sub block and holds it reset. Writing "1" resets the sub block and releases the resets autumaticaly. , [0]- fd mu csm , [1]- phases db , [2]- fdelta snr db
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf153_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF154 0x10550 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blocksEn2:3;	// FD_sub blocks en. One bit per sub block. Enables or disables the sub block  , [0]- fd mu csm , [1]- phases db , [2]- fdelta snr db
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf154_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF155 0x10554 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate2:3;	// A SW reset procedure will be initiated by writing to this register to the bits that equal 1
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf155_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF156 0x10558 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmSwResetMask2:3;	// Which block to reset at the falling edge of the global enable. One bit per sub block - same mapping as in sw_reset_n_reg2 register
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf156_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF157 0x1055C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSwResetMask2:3;	// Which units to software reset at at the beginning of Tx , [0]- fd mu csm , [1]- phases db , [2]- delta snr db
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf157_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF158 0x10560 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkDebugBypass2:3;	// One bit per gator. , [0]- fd mu csm , [1]- phases db , [2]- delta snr db
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf158_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF159 0x10564 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eSnrUp0Mcs:4;	// e_snr_up0_mcs
		uint32 eSnrUp1Mcs:4;	// e_snr_up1_mcs
		uint32 eSnrUp2Mcs:4;	// e_snr_up2_mcs
		uint32 eSnrUp3Mcs:4;	// e_snr_up3_mcs
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf159_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF15A 0x10568 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfdMuNdp:1;	// rxfd_mu_ndp
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf15A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF15B 0x1056C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sphereActivationSymNum:10;	// The symbol number for activating sphere (Start counting from first DATA symbol)
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf15B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF15C 0x10570 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muSphereActivationSymNum:10;	// MU - the symbol number for activating sphere (Start counting from first DATA symbol)
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf15C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF15D 0x10574 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfdMuCsm:3;	// RXFD MU control stm
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf15D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_FD_PHY_RXFD_IF15E 0x10578 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTxErrorCause:8;	// Cause indication to MU Tx error - , {user_3_nr_error ,user_2_nr_error ,user_1_nr_error ,user_0_nr_error, user_3_nc_error ,user_2_nc_error ,user_1_nc_error ,user_0_nc_error}
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0FdPhyRxfdIf15E_u;



#endif // _PHY_RX_TD_IF_RISC_PAGE_0_FD_REGS_H_
