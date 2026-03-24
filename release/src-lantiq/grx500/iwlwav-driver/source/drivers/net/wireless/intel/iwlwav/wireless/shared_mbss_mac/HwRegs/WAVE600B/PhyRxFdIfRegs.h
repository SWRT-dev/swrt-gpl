
/***********************************************************************************
File:				PhyRxFdIfRegs.h
Module:				PhyRxFdIf
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_RX_FD_IF_REGS_H_
#define _PHY_RX_FD_IF_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_RX_FD_IF_BASE_ADDRESS                             MEMORY_MAP_UNIT_51_BASE_ADDRESS
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF100                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1400)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF101                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1404)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF102                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1408)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF103                (PHY_RX_FD_IF_BASE_ADDRESS + 0x140C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF104                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1410)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF_RISC_CAUSE_INT    (PHY_RX_FD_IF_BASE_ADDRESS + 0x1414)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF106                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1418)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF107                (PHY_RX_FD_IF_BASE_ADDRESS + 0x141C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF108                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1420)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF109                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1424)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF10B                (PHY_RX_FD_IF_BASE_ADDRESS + 0x142C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF10C                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1430)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF10D                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1434)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF10E                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1438)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF10F                (PHY_RX_FD_IF_BASE_ADDRESS + 0x143C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF110                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1440)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF111                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1444)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF112                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1448)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF113                (PHY_RX_FD_IF_BASE_ADDRESS + 0x144C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF115                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1454)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF116                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1458)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF117                (PHY_RX_FD_IF_BASE_ADDRESS + 0x145C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF118                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1460)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF11B                (PHY_RX_FD_IF_BASE_ADDRESS + 0x146C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF11D                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1474)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF11E                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1478)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF11F                (PHY_RX_FD_IF_BASE_ADDRESS + 0x147C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF120                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1480)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF121                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1484)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF122                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1488)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF124                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1490)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF125                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1494)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF126                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1498)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF127                (PHY_RX_FD_IF_BASE_ADDRESS + 0x149C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF128                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14A0)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF129                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14A4)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF12A                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14A8)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF12B                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14AC)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF12C                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14B0)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF12D                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14B4)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF12F                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14BC)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF134                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14D0)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF135                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14D4)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF137                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14DC)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF138                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14E0)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF139                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14E4)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF13A                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14E8)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF13B                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14EC)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF13C                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14F0)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF13D                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14F4)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF13E                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14F8)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF13F                (PHY_RX_FD_IF_BASE_ADDRESS + 0x14FC)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF140                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1500)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF141                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1504)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF142                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1508)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF143                (PHY_RX_FD_IF_BASE_ADDRESS + 0x150C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF144                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1510)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF145                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1514)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF146                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1518)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF147                (PHY_RX_FD_IF_BASE_ADDRESS + 0x151C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF148                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1520)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF14B                (PHY_RX_FD_IF_BASE_ADDRESS + 0x152C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF14C                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1530)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF14D                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1534)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF14E                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1538)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF14F                (PHY_RX_FD_IF_BASE_ADDRESS + 0x153C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF150                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1540)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF151                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1544)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF152                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1548)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF153                (PHY_RX_FD_IF_BASE_ADDRESS + 0x154C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF154                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1550)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF155                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1554)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF156                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1558)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF157                (PHY_RX_FD_IF_BASE_ADDRESS + 0x155C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF15A                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1568)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF15B                (PHY_RX_FD_IF_BASE_ADDRESS + 0x156C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF15C                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1570)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF15D                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1574)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF15E                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1578)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF15F                (PHY_RX_FD_IF_BASE_ADDRESS + 0x157C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF160                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1580)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF161                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1584)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF162                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1588)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF163                (PHY_RX_FD_IF_BASE_ADDRESS + 0x158C)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF164                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1590)
#define	REG_PHY_RX_FD_IF_PHY_RXFD_IF165                (PHY_RX_FD_IF_BASE_ADDRESS + 0x1594)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RX_FD_IF_PHY_RXFD_IF100 0x1400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg : 16; //FD sub blocks sw reset , One bit per sub block. Resets only the core logic. Programming model is unchaged. Writing "0" resets the sub block and holds it reset. Writing "1" resets the sub block and releases the resets autumaticaly. , [0]- fd csm , [1]- fft_0 , [2]- fft_1 , [3]- fft_2 , [4]-fft_3 , [5]- bin chooser , [6]- nco , [7]- pilot scrambler , [8]- phase tracking , [9]- organizer , [10]- channel tracking , [11]- channel smoothing , [12]- timing estimation , [13]- gsm , [14]- slicer , [15]- qr, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf100_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF101 0x1404 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blocksEn : 16; //FD_sub blocks en. One bit per sub block. Enables or disables the sub block  , [0]- fd csm , [1]- fft_0 , [2]- fft_1 , [3]- fft_2 , [4]- fft_2 , [5]- bin chooser , [6]- nco , [7]- pilot scrambler , [8]- phase tracking , [9]- organizer , [10]- qr , [11]- channel smoothing , [12]- timing estimation , [13]- gsm , [14]- slicer , [15]- qr, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf101_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF102 0x1408 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate : 16; //A SW reset procedure will be initiated by writing to this register to the bits that equal 1, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf102_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF103 0x140C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmSwResetMask : 16; //Which block to reset at the falling edge of the global enable. One bit per sub block - same mapping as in sw_reset_n_reg register, reset value: 0x5FFF, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf103_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF104 0x1410 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscResetModemP : 1; //Reset Modem Command From Risc. pulse, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf104_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF_RISC_CAUSE_INT 0x1414 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfdRiscCauseInt_rxfdRiscCauseIntClear : 10; //clear RXFD Risc Cause intterupt - , bit[9] - he packet , bit[8] - vht packet , bit[7] - effective snr , bit[6] - last symbol , bit[5] - fd_stm_aborted , bit[4] - td_stm_aborted , bit[3] - fd symbol ready , bit[2] - sym_strobe to td , bit[1] - green field packet , bit[0] - high throughput packet, reset value: 0x0, access type: WO
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIfRiscCauseInt_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF106 0x1418 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfdRiscIntEnable : 10; //RXFD Risc intterupt enable , bit[9] - he packet , bit[8] - vht packet , bit[7] - effective snr , bit[6] - last symbol , bit[5] - fd_stm_aborted , bit[4] - td_stm_aborted , bit[3] - fd symbol ready , bit[2] - sym_strobe to td , bit[1] - green field packet , bit[0] - high throughput packet, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf106_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF107 0x141C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmTdNextState : 4; //GSM Td Next State, reset value: 0x0, access type: RO
		uint32 gsmFdNextState : 4; //GSM Fd Next State, reset value: 0x0, access type: RO
		uint32 rxfdCsm : 5; //RXFD control_stm, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf107_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF108 0x1420 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 antennaEn : 4; //Sanity Risc Antenna Enable , Risc Decision , (bit per antenna) , reset value: 0x0, access type: RW
		uint32 antennaEnValid : 1; //Sanity Risc Antenna Enable Valid, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf108_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF109 0x1424 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctDataMu : 4; //Channel Tracking data Coefficient Mu, reset value: 0x0, access type: RW
		uint32 ctPilotMu : 4; //Channel Tracking pilot Coefficient Mu, reset value: 0x0, access type: RW
		uint32 ftShifter : 4; //Frequency Tracking shifter, reset value: 0x0, access type: RW
		uint32 lltfChSmoothBypass : 1; //L-LTF Channel smoothing Bypass, reset value: 0x1, access type: RW
		uint32 lltfTeBypass : 1; //L-LTF timing estimation bypass, reset value: 0x0, access type: RW
		uint32 htltfTeBypass : 1; //HT-LTF timing estimation bypass, reset value: 0x0, access type: RW
		uint32 htltfChSmoothBypass : 1; //HT-LTF Channel smoothing Bypass, reset value: 0x1, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf109_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF10B 0x142C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phaseSmoothingSize : 5; //Phase Smoothing Size, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 timingSmoothingSize : 5; //Timing Smoothing Size, reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf10B_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF10C 0x1430 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 hModeCntRisc : 3; //h_mode counter, reset value: 0x0, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf10C_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF10D 0x1434 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscTdGlobalEn : 1; //risc override the td global enable, reset value: 0x0, access type: RW
		uint32 riscFdGlobalEn : 1; //risc override the fd global enable, reset value: 0x0, access type: RW
		uint32 riscBeGlobalEn : 1; //risc override the be global enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 riscTdGlobalEnSel : 1; //TD global enable selection: , 0- hw , 1-sw risc, reset value: 0x0, access type: RW
		uint32 riscFdGlobalEnSel : 1; //FD global enable selection: , 0- hw , 1-sw risc, reset value: 0x0, access type: RW
		uint32 riscBeGlobalEnSel : 1; //BE global enable selection: , 0- hw , 1-sw risc, reset value: 0x0, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf10D_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF10E 0x1438 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htMode : 1; //ht mode: , 0-NOT ht , 1-HT mode, reset value: 0x0, access type: RO
		uint32 gfDetected : 1; //green field detection result: , 1- green field detected, reset value: 0x0, access type: RO
		uint32 vhtMode : 1; //vht mode, reset value: 0x0, access type: RO
		uint32 heMode : 1; //he mode, reset value: 0x0, access type: RO
		uint32 htMetrics : 11; //HT detector metric information, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf10E_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF10F 0x143C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mcs32Rev0 : 1; //mcs32 backward compatible for rev_0, reset value: 0x0, access type: RW
		uint32 gfNcoBpEn : 1; //enable nco_bypass for ht_detector input data in L_sig (do not rotate the data_bins for the ht detector) , reset value: 0x0, access type: RW
		uint32 htNcoBpEn : 1; //enable nco_bypass for ht_detector input data in ht_sig1 (do not rotate the data_bins for the ht detector) , reset value: 0x0, access type: RW
		uint32 vhtNcoBpEn : 1; //enable nco_bypass for ht_detector input data in ht_sig2 (do not rotate the data_bins for the ht detector) , reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf10F_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF110 0x1440 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGpo : 16; //gpo , for general use at the top level, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf110_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF111 0x1444 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscNumSymbols : 15; //number of symbols by RISC (override air time calculation), reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf111_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF112 0x1448 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 12;
		uint32 riscNumSymbolsValid : 1; //number of symbols by RISC valid, reset value: 0x0, access type: RW
		uint32 riscNumSymbolsSel : 1; //Select number of symbols source , 0-Hw , 1-Risc, reset value: 0x0, access type: RW
		uint32 fdTimRevertEn : 1; //FD Timing revert enable, reset value: 0x0, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf112_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF113 0x144C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 teOffset : 4; //timing estimation offset, reset value: 0x0, access type: RW
		uint32 reserved1 : 24;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf113_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF115 0x1454 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGpi : 16; //gpi for read only , reset value: 0xa002, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf115_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF116 0x1458 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdGpi2 : 16; //gpi2 for read only , reset value: 0xa002, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf116_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF117 0x145C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdLowGpr : 16; //hyp_rxfd_low_gpr, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf117_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF118 0x1460 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdHighGpr : 16; //hyp_rxfd_high_gpr, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf118_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF11B 0x146C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkDebugBypass2 : 9; //One bit per gator. , [0]- fd mu csm , [1]- mu phases db , [2]- fdelta snr db , [3]- su phases db , [4]- tcr , [5]- phase align , [6]- impl channel prep , [7]- average snr calc , [8]- bf rx parser , , reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 rfGclkDebugBypass : 1; //Register File gclk_debug_bypass, reset value: 0x1, access type: RW
		uint32 freqTrckGclkDebugBypass : 1; //freq_trck_gclk_debug_bypass, reset value: 0x1, access type: RW
		uint32 riscIf2BiuGclkBypass : 1; //risc_if2biu gclk bypass, reset value: 0x0, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf11B_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF11D 0x1474 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkDebugBypass : 16; //One bit per gator. , [0]- fd csm , [1]- fft_0 , [2]- fft_1 , [3]- fft_2 , [4]-fft_3 , [5]- bin chooser , [6]- nco , [7]- pilot scrambler , [8]- phase tracking , [9]- organizer , [10]- channel tracking , [11]- channel smoothing , [12]- timing estimation , [13]- gsm , [14]- slicer , [15]- qr, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf11D_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF11E 0x1478 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timingShiftAccum : 7; //timing shift accumulator - for metrics, reset value: 0x0, access type: RO
		uint32 reserved0 : 25;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf11E_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF11F 0x147C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 an0Magnitude : 16; //fd_timing - an0 magnitude value, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf11F_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF120 0x1480 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 an1Magnitude : 16; //fd_timing - an1 magnitude value, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf120_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF121 0x1484 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 an2Magnitude : 16; //fd_timing - an2 magnitude value, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf121_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF122 0x1488 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 isHtGfMagnitude : 16; //gf/ht/vht detection magnitude value, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf122_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF124 0x1490 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 9;
		uint32 effectiveSnrMcs : 7; //effective snr module result - effective_snr_mcs withput the msb (rank bit), reset value: 0x0, access type: RO
		uint32 reserved1 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf124_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF125 0x1494 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 2;
		uint32 pgc1Ant0 : 6; //agc calculation - pgc1 ant0 , reset value: 0x0, access type: RW
		uint32 pgc2Ant0 : 6; //agc calculation - pgc2 ant0 , reset value: 0x0, access type: RW
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf125_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF126 0x1498 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pgc3Ant0 : 6; //agc calculation - pgc3 ant0 , reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 lnaAnt0 : 5; //agc calculation - lna ant0 , reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf126_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF127 0x149C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 2;
		uint32 pgc1Ant1 : 6; //agc calculation - pgc1 ant1, reset value: 0x0, access type: RW
		uint32 pgc2Ant1 : 6; //agc calculation - pgc2 ant1, reset value: 0x0, access type: RW
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf127_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF128 0x14A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pgc3Ant1 : 6; //agc calculation - pgc3 ant1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 lnaAnt1 : 5; //agc calculation - lna ant1, reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf128_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF129 0x14A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 2;
		uint32 pgc1Ant2 : 6; //agc calculation - pgc1 ant2, reset value: 0x0, access type: RW
		uint32 pgc2Ant2 : 6; //agc calculation - pgc2 ant2, reset value: 0x0, access type: RW
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf129_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF12A 0x14A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pgc3Ant2 : 6; //agc calculation - pgc3 ant2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 lnaAnt2 : 5; //agc calculation - lna ant2, reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf12A_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF12B 0x14AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 implChannPreFlatCorrectionAnt0 : 8; //flat correction calculation for implicit channel preparation antenna0, reset value: 0x0, access type: RW
		uint32 implChannPreFlatCorrectionAnt1 : 8; //flat correction calculation for implicit channel preparation antenna1, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf12B_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF12C 0x14B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 implChannPreFlatCorrectionAnt2 : 8; //flat correction calculation for implicit channel preparation antenna2, reset value: 0x0, access type: RW
		uint32 implChannPreFlatCorrectionAnt3 : 8; //flat correction calculation for implicit channel preparation antenna3, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf12C_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF12D 0x14B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 2;
		uint32 flatCorrectionCalcChPreUnitDone : 1; //this signal rise when genrisc finnish correction calculation , reset value: 0x0, access type: RW
		uint32 explImplNMode : 1; //0 - implicit bf mode , 1- explicit bf mode, reset value: 0x0, access type: RW
		uint32 genriscBfEn : 1; //bf enable, reset value: 0x1, access type: RW
		uint32 genriscUploadChEn : 1; //bf_rx_parser upload the channel, reset value: 0x0, access type: RW
		uint32 mcsFeedbackDisable : 1; //1 - ignore e_snr mcs decision, reset value: 0x0, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf12D_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF12F 0x14BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 11;
		uint32 noiseTrkGainInMin : 5; //noise_trk_gain_in_min, reset value: 0x5, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf12F_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF134 0x14D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseTrkNoiseIdentityReg : 8; //noise_trk_noise_identity_reg, reset value: 0x10, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf134_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF135 0x14D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 an3Magnitude : 16; //fd_timing - an3 magnitude value, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf135_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF137 0x14DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timFixLltfValid : 1; //fd_timing l_ltf result valid , reset value: 0x0, access type: RO
		uint32 timFixHtltfValid : 1; //fd_timing ht_ltf result valid , reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf137_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF138 0x14E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscEnableSphere : 1; //enable sphere, reset value: 0x1, access type: RW
		uint32 riscEnableLinear : 1; //enable linear, reset value: 0x1, access type: RW
		uint32 riscSumLlr : 1; //sum LLRs from linear and sphere, reset value: 0x1, access type: RW
		uint32 reserved0 : 1;
		uint32 riscForceNtGainCalc : 1; //force noise tracking gain calculation, reset value: 0x0, access type: RW
		uint32 reserved1 : 5;
		uint32 sphereMode : 2; //sphere_mode 0:regular,1:fastmode1,2:fastmode2, reset value: 0x0, access type: RW
		uint32 reserved2 : 20;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf138_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF139 0x14E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 linearViterbiShift : 10; //linear_viterbi_shift, reset value: 0xe, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf139_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF13A 0x14E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerLoadGainStream0 : 12; //Power load gain for adaptive filter (Tx) stream 0 , reset value: 0x400, access type: RW
		uint32 powerLoadGainReady : 1; //Power load gain for adaptive filter is ready in registers for all streams, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf13A_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF13B 0x14EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerLoadGainStream1 : 12; //Power load gain for adaptive filter (Tx) stream 1 , reset value: 0x400, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf13B_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF13C 0x14F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerLoadGainStream2 : 12; //Power load gain for adaptive filter (Tx) stream 2 , reset value: 0x400, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf13C_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF13D 0x14F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerLoadGainStream3 : 12; //Power load gain for adaptive filter (Tx) stream 3 , reset value: 0x400, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf13D_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF13E 0x14F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sigbMuData1Ss : 4; //Channel Tracking Coefficient Mu for SIGB 1ss, reset value: 0x1, access type: RW
		uint32 sigbMuData2Ss : 4; //Channel Tracking Coefficient Mu for SIGB 2ss, reset value: 0x2, access type: RW
		uint32 sigbMuData3Ss : 4; //Channel Tracking Coefficient Mu for SIGB 3ss, reset value: 0x3, access type: RW
		uint32 sigbMuData4Ss : 4; //Channel Tracking Coefficient Mu for SIGB 4ss, reset value: 0x3, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf13E_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF13F 0x14FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sphereViterbiShift : 10; //sphere_viterbi_shift, reset value: 0xe, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf13F_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF140 0x1500 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bypassSmoothing : 4; //Bypass smoothing - bit per Spatial Stream, reset value: 0x0, access type: RW
		uint32 chEstiSmoothingCsd : 1; //CSD compensation in smoothing unit -remove CSD before smoothing and return value (reverse) after smoothing, reset value: 0x1, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf140_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF141 0x1504 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 2;
		uint32 pgc1Ant3 : 6; //agc calculation - pgc1 ant3 , reset value: 0x0, access type: RW
		uint32 pgc2Ant3 : 6; //agc calculation - pgc2 ant3 , reset value: 0x0, access type: RW
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf141_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF142 0x1508 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pgc3Ant3 : 6; //agc calculation - pgc3 ant3 , reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 lnaAnt3 : 5; //agc calculation - lna ant3 , reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf142_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF143 0x150C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscStartOrganizer : 1; //risc_start_organizer (set when 'fine' is ready), reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf143_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF144 0x1510 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 payloadSymbolNum : 15; //payload_symbol_number, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf144_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF145 0x1514 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSwResetMask : 16; //Which units to software reset at at the beginning of Tx , [0]- fd csm , [1]- fft_0 , [2]- fft_1 , [3]- fft_2 , [4]-fft_3 , [5]- bin chooser , [6]- nco , [7]- pilot scrambler , [8]- phase tracking , [9]- organizer , [10]- channel tracking , [11]- channel smoothing , [12]- timing estimation , [13]- gsm , [14]- slicer/qr, reset value: 0x7FE1, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf145_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF146 0x1518 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gearShiftSymNum : 10; //The symbol number for changing the mu (Start counting from first DATA symbol), reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf146_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF147 0x151C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctDataMuGearShift : 4; //Channel Tracking Mu value for data to change after reaching the gear shift symbol, reset value: 0x0, access type: RW
		uint32 ctPilotMuGearShift : 4; //Channel Tracking Mu value for pilots to change after reaching the gear shift symbol, reset value: 0x0, access type: RW
		uint32 ftShifterGearShift : 4; //Frequency Tracking shift value when reaching the gear shift symbol, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf147_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF148 0x1520 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fdSymbolType : 5; //FD symbol type (from GSM), reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 fdBw : 3; //FD bandwidth , 1- 20M , 2 - 40M , 3 - 80M, reset value: 0x0, access type: RO
		uint32 reserved1 : 21;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf148_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF14B 0x152C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTrainingMode : 1; //MU training mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf14B_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF14C 0x1530 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muStartTrainingMode : 1; //start MU training operation, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf14C_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF14D 0x1534 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTrainingDone : 1; //mu training done, reset value: 0x1, access type: RO
		uint32 muTrainingIndex : 10; //mu training index (effective rate memory address), reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf14D_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF14E 0x1538 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUsersValidityReg : 16; //Reflecting the users validity in training. Bit per user, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf14E_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF14F 0x153C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muRiscUsersValidityReg : 16; //The GenRisc reads the mu_users_validity_reg and write back to this register with option of modifying, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf14F_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF150 0x1540 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTrainingBw : 2; //mu_training_bw, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf150_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF151 0x1544 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtPwrLDb0 : 5; //tx_ct_pwr_l_db0, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 txCtPwrLDb1 : 5; //tx_ct_pwr_l_db1, reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf151_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF152 0x1548 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCtPwrLDb2 : 5; //tx_ct_pwr_l_db2, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 txCtPwrLDb3 : 5; //tx_ct_pwr_l_db3, reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf152_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF153 0x154C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg2 : 16; //FD sub blocks sw reset , One bit per sub block. Resets only the core logic. Programming model is unchaged. Writing "0" resets the sub block and holds it reset. Writing "1" resets the sub block and releases the resets autumaticaly. , [0]- fd mu csm , [1]- mu phases db , [2]- fdelta snr db , [3]- su phases db , [4]- tcr , [5]- phase align , [6]- impl channel prep , [7]- average snr calc , [8]- bf rx parser , , reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf153_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF154 0x1550 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blocksEn2 : 16; //FD_sub blocks en. One bit per sub block. Enables or disables the sub block  , [0]- fd mu csm , [1]- mu phases db , [2]- fdelta snr db , [3]- su phases db , [4]- tcr , [5]- phase align , [6]- impl channel prep , [7]- average snr calc , [8]- bf rx parser , , reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf154_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF155 0x1554 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate2 : 16; //A SW reset procedure will be initiated by writing to this register to the bits that equal 1, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf155_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF156 0x1558 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmSwResetMask2 : 16; //Which block to reset at the falling edge of the global enable. One bit per sub block - same mapping as in sw_reset_n_reg2 register, reset value: 0x7, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf156_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF157 0x155C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSwResetMask2 : 16; //Which units to software reset at at the beginning of Tx , [0]- fd mu csm , [1]- phases db , [2]- delta snr db, reset value: 0x7, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf157_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF15A 0x1568 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfdMuNdp : 1; //rxfd_mu_ndp, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf15A_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF15B 0x156C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sphereActivationSymNum : 10; //The symbol number for activating sphere (Start counting from first DATA symbol), reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf15B_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF15C 0x1570 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muSphereActivationSymNum : 10; //MU - the symbol number for activating sphere (Start counting from first DATA symbol), reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf15C_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF15D 0x1574 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfdMuCsm : 3; //RXFD MU control stm, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf15D_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF15E 0x1578 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTxErrorCause : 8; //Cause indication to MU Tx error - , {user_3_nr_error ,user_2_nr_error ,user_1_nr_error ,user_0_nr_error, user_3_nc_error ,user_2_nc_error ,user_1_nc_error ,user_0_nc_error}, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf15E_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF15F 0x157C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscSuClearAll : 1; //clear su db valid, reset value: 0x0, access type: RW
		uint32 riscMuClearAll : 1; //clear mu db valid, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf15F_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF160 0x1580 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timFixAn0 : 10; //fd_timing result antenna_0, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf160_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF161 0x1584 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timFixAn1 : 10; //fd_timing result antenna_1, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf161_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF162 0x1588 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timFixAn2 : 10; //fd_timing result antenna_2, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf162_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF163 0x158C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timFixAn3 : 10; //fd_timing result antenna_3, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf163_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF164 0x1590 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxfdClrRu : 1; //clear smooth ru ram, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf164_u;

/*REG_PHY_RX_FD_IF_PHY_RXFD_IF165 0x1594 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTxErrorCause2 : 9; //Cause indication to MU Tx error 2nd : {user_bw_error_8       user_3_bw_error_6  ,   user_2_bw_error_6 , user_1_bw_error_6 , user_0_bw_error_6 ,user_3_bw_error_4  ,   user_2_bw_error_4 , user_1_bw_error_4 , user_0_bw_error_4 }, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegPhyRxFdIfPhyRxfdIf165_u;



#endif // _PHY_RX_FD_IF_REGS_H_
