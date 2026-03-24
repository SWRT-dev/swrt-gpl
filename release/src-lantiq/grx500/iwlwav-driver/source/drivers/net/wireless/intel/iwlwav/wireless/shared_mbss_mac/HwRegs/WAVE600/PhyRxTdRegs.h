
/***********************************************************************************
File:				PhyRxTdRegs.h
Module:				PhyRxTd
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_RX_TD_REGS_H_
#define _PHY_RX_TD_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_RX_TD_BASE_ADDRESS                             MEMORY_MAP_UNIT_50_BASE_ADDRESS
#define	REG_PHY_RX_TD_PHY_RXTD_REG00                      (PHY_RX_TD_BASE_ADDRESS + 0x0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG01                      (PHY_RX_TD_BASE_ADDRESS + 0x4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG02                      (PHY_RX_TD_BASE_ADDRESS + 0x8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG03                      (PHY_RX_TD_BASE_ADDRESS + 0xC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG04                      (PHY_RX_TD_BASE_ADDRESS + 0x10)
#define	REG_PHY_RX_TD_PHY_RXTD_REG05                      (PHY_RX_TD_BASE_ADDRESS + 0x14)
#define	REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_LOW         (PHY_RX_TD_BASE_ADDRESS + 0x18)
#define	REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_MID         (PHY_RX_TD_BASE_ADDRESS + 0x1C)
#define	REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_HIGH        (PHY_RX_TD_BASE_ADDRESS + 0x20)
#define	REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_FOUR        (PHY_RX_TD_BASE_ADDRESS + 0x24)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0A                      (PHY_RX_TD_BASE_ADDRESS + 0x28)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0B                      (PHY_RX_TD_BASE_ADDRESS + 0x2C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0C                      (PHY_RX_TD_BASE_ADDRESS + 0x30)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0D                      (PHY_RX_TD_BASE_ADDRESS + 0x34)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0E                      (PHY_RX_TD_BASE_ADDRESS + 0x38)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0F                      (PHY_RX_TD_BASE_ADDRESS + 0x3C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG10                      (PHY_RX_TD_BASE_ADDRESS + 0x40)
#define	REG_PHY_RX_TD_PHY_RXTD_REG11                      (PHY_RX_TD_BASE_ADDRESS + 0x44)
#define	REG_PHY_RX_TD_PHY_RXTD_REG12                      (PHY_RX_TD_BASE_ADDRESS + 0x48)
#define	REG_PHY_RX_TD_PHY_RXTD_REG13                      (PHY_RX_TD_BASE_ADDRESS + 0x4C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG15                      (PHY_RX_TD_BASE_ADDRESS + 0x54)
#define	REG_PHY_RX_TD_PHY_RXTD_REG16                      (PHY_RX_TD_BASE_ADDRESS + 0x58)
#define	REG_PHY_RX_TD_PHY_RXTD_REG17                      (PHY_RX_TD_BASE_ADDRESS + 0x5C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG18                      (PHY_RX_TD_BASE_ADDRESS + 0x60)
#define	REG_PHY_RX_TD_PHY_RXTD_REG19                      (PHY_RX_TD_BASE_ADDRESS + 0x64)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1A                      (PHY_RX_TD_BASE_ADDRESS + 0x68)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1B                      (PHY_RX_TD_BASE_ADDRESS + 0x6C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1C                      (PHY_RX_TD_BASE_ADDRESS + 0x70)
#define	REG_PHY_RX_TD_PHY_RXTD_REG01D                     (PHY_RX_TD_BASE_ADDRESS + 0x74)
#define	REG_PHY_RX_TD_PHY_RXTD_REG01E                     (PHY_RX_TD_BASE_ADDRESS + 0x78)
#define	REG_PHY_RX_TD_PHY_RXTD_REG01F                     (PHY_RX_TD_BASE_ADDRESS + 0x7C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG021                     (PHY_RX_TD_BASE_ADDRESS + 0x84)
#define	REG_PHY_RX_TD_PHY_RXTD_REG022                     (PHY_RX_TD_BASE_ADDRESS + 0x88)
#define	REG_PHY_RX_TD_PHY_RXTD_REG023                     (PHY_RX_TD_BASE_ADDRESS + 0x8C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG024                     (PHY_RX_TD_BASE_ADDRESS + 0x90)
#define	REG_PHY_RX_TD_PHY_RXTD_REG025                     (PHY_RX_TD_BASE_ADDRESS + 0x94)
#define	REG_PHY_RX_TD_PHY_RXTD_REG026                     (PHY_RX_TD_BASE_ADDRESS + 0x98)
#define	REG_PHY_RX_TD_PHY_RXTD_REG027                     (PHY_RX_TD_BASE_ADDRESS + 0x9C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG028                     (PHY_RX_TD_BASE_ADDRESS + 0xA0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG029                     (PHY_RX_TD_BASE_ADDRESS + 0xA4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG02A                     (PHY_RX_TD_BASE_ADDRESS + 0xA8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG02B                     (PHY_RX_TD_BASE_ADDRESS + 0xAC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG02C                     (PHY_RX_TD_BASE_ADDRESS + 0xB0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG02D                     (PHY_RX_TD_BASE_ADDRESS + 0xB4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2E                      (PHY_RX_TD_BASE_ADDRESS + 0xB8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2F                      (PHY_RX_TD_BASE_ADDRESS + 0xBC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG30                      (PHY_RX_TD_BASE_ADDRESS + 0xC0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG31                      (PHY_RX_TD_BASE_ADDRESS + 0xC4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG032                     (PHY_RX_TD_BASE_ADDRESS + 0xC8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG033                     (PHY_RX_TD_BASE_ADDRESS + 0xCC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG034                     (PHY_RX_TD_BASE_ADDRESS + 0xD0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG035                     (PHY_RX_TD_BASE_ADDRESS + 0xD4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG036                     (PHY_RX_TD_BASE_ADDRESS + 0xD8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG037                     (PHY_RX_TD_BASE_ADDRESS + 0xDC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG038                     (PHY_RX_TD_BASE_ADDRESS + 0xE0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG039                     (PHY_RX_TD_BASE_ADDRESS + 0xE4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG03A                     (PHY_RX_TD_BASE_ADDRESS + 0xE8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG03B                     (PHY_RX_TD_BASE_ADDRESS + 0xEC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG03C                     (PHY_RX_TD_BASE_ADDRESS + 0xF0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG03D                     (PHY_RX_TD_BASE_ADDRESS + 0xF4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG03E                     (PHY_RX_TD_BASE_ADDRESS + 0xF8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG03F                     (PHY_RX_TD_BASE_ADDRESS + 0xFC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG040                     (PHY_RX_TD_BASE_ADDRESS + 0x100)
#define	REG_PHY_RX_TD_PHY_RXTD_REG041                     (PHY_RX_TD_BASE_ADDRESS + 0x104)
#define	REG_PHY_RX_TD_PHY_RXTD_REG042                     (PHY_RX_TD_BASE_ADDRESS + 0x108)
#define	REG_PHY_RX_TD_PHY_RXTD_REG043                     (PHY_RX_TD_BASE_ADDRESS + 0x10C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG044                     (PHY_RX_TD_BASE_ADDRESS + 0x110)
#define	REG_PHY_RX_TD_PHY_RXTD_REG045                     (PHY_RX_TD_BASE_ADDRESS + 0x114)
#define	REG_PHY_RX_TD_PHY_RXTD_REG046                     (PHY_RX_TD_BASE_ADDRESS + 0x118)
#define	REG_PHY_RX_TD_PHY_RXTD_REG047                     (PHY_RX_TD_BASE_ADDRESS + 0x11C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG048                     (PHY_RX_TD_BASE_ADDRESS + 0x120)
#define	REG_PHY_RX_TD_PHY_RXTD_REG049                     (PHY_RX_TD_BASE_ADDRESS + 0x124)
#define	REG_PHY_RX_TD_PHY_RXTD_REG04A                     (PHY_RX_TD_BASE_ADDRESS + 0x128)
#define	REG_PHY_RX_TD_PHY_RXTD_REG04B                     (PHY_RX_TD_BASE_ADDRESS + 0x12C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG04C                     (PHY_RX_TD_BASE_ADDRESS + 0x130)
#define	REG_PHY_RX_TD_PHY_RXTD_REG04D                     (PHY_RX_TD_BASE_ADDRESS + 0x134)
#define	REG_PHY_RX_TD_PHY_RXTD_REG04E                     (PHY_RX_TD_BASE_ADDRESS + 0x138)
#define	REG_PHY_RX_TD_PHY_RXTD_REG04F                     (PHY_RX_TD_BASE_ADDRESS + 0x13C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG050                     (PHY_RX_TD_BASE_ADDRESS + 0x140)
#define	REG_PHY_RX_TD_PHY_RXTD_REG051                     (PHY_RX_TD_BASE_ADDRESS + 0x144)
#define	REG_PHY_RX_TD_PHY_RXTD_REG052                     (PHY_RX_TD_BASE_ADDRESS + 0x148)
#define	REG_PHY_RX_TD_PHY_RXTD_REG053                     (PHY_RX_TD_BASE_ADDRESS + 0x14C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG054                     (PHY_RX_TD_BASE_ADDRESS + 0x150)
#define	REG_PHY_RX_TD_PHY_RXTD_REG055                     (PHY_RX_TD_BASE_ADDRESS + 0x154)
#define	REG_PHY_RX_TD_PHY_RXTD_REG056                     (PHY_RX_TD_BASE_ADDRESS + 0x158)
#define	REG_PHY_RX_TD_PHY_RXTD_REG057                     (PHY_RX_TD_BASE_ADDRESS + 0x15C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG058                     (PHY_RX_TD_BASE_ADDRESS + 0x160)
#define	REG_PHY_RX_TD_PHY_RXTD_REG059                     (PHY_RX_TD_BASE_ADDRESS + 0x164)
#define	REG_PHY_RX_TD_PHY_RXTD_REG05A                     (PHY_RX_TD_BASE_ADDRESS + 0x168)
#define	REG_PHY_RX_TD_PHY_RXTD_REG05B                     (PHY_RX_TD_BASE_ADDRESS + 0x16C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG05C                     (PHY_RX_TD_BASE_ADDRESS + 0x170)
#define	REG_PHY_RX_TD_PHY_RXTD_REG05D                     (PHY_RX_TD_BASE_ADDRESS + 0x174)
#define	REG_PHY_RX_TD_PHY_RXTD_REG05E                     (PHY_RX_TD_BASE_ADDRESS + 0x178)
#define	REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_MSG         (PHY_RX_TD_BASE_ADDRESS + 0x17C)
#define	REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_MSG_DONE    (PHY_RX_TD_BASE_ADDRESS + 0x180)
#define	REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_FAST_MSG    (PHY_RX_TD_BASE_ADDRESS + 0x184)
#define	REG_PHY_RX_TD_PHY_RXTD_REG062                     (PHY_RX_TD_BASE_ADDRESS + 0x188)
#define	REG_PHY_RX_TD_PHY_RXTD_REG063                     (PHY_RX_TD_BASE_ADDRESS + 0x18C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG064                     (PHY_RX_TD_BASE_ADDRESS + 0x190)
#define	REG_PHY_RX_TD_PHY_RXTD_REG065                     (PHY_RX_TD_BASE_ADDRESS + 0x194)
#define	REG_PHY_RX_TD_PHY_RXTD_REG066                     (PHY_RX_TD_BASE_ADDRESS + 0x198)
#define	REG_PHY_RX_TD_PHY_RXTD_REG067                     (PHY_RX_TD_BASE_ADDRESS + 0x19C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG068                     (PHY_RX_TD_BASE_ADDRESS + 0x1A0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG069                     (PHY_RX_TD_BASE_ADDRESS + 0x1A4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG06A                     (PHY_RX_TD_BASE_ADDRESS + 0x1A8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG06B                     (PHY_RX_TD_BASE_ADDRESS + 0x1AC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG06C                     (PHY_RX_TD_BASE_ADDRESS + 0x1B0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG_LOOPBACK_CTRL          (PHY_RX_TD_BASE_ADDRESS + 0x1B4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG06E                     (PHY_RX_TD_BASE_ADDRESS + 0x1B8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG06F                     (PHY_RX_TD_BASE_ADDRESS + 0x1BC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG070                     (PHY_RX_TD_BASE_ADDRESS + 0x1C0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG071                     (PHY_RX_TD_BASE_ADDRESS + 0x1C4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG072                     (PHY_RX_TD_BASE_ADDRESS + 0x1C8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG076                     (PHY_RX_TD_BASE_ADDRESS + 0x1D8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG077                     (PHY_RX_TD_BASE_ADDRESS + 0x1DC)
#define	REG_PHY_RX_TD_PHY_TSF_TIMER_CONTROL               (PHY_RX_TD_BASE_ADDRESS + 0x1E0)
#define	REG_PHY_RX_TD_PHY_TSF_TIMER                       (PHY_RX_TD_BASE_ADDRESS + 0x1E4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG07A                     (PHY_RX_TD_BASE_ADDRESS + 0x1E8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG07B                     (PHY_RX_TD_BASE_ADDRESS + 0x1EC)
#define	REG_PHY_RX_TD_PHY_TSF_TIMER_LOAD_VAL              (PHY_RX_TD_BASE_ADDRESS + 0x1F0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG07D                     (PHY_RX_TD_BASE_ADDRESS + 0x1F4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG07E                     (PHY_RX_TD_BASE_ADDRESS + 0x1F8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG07F                     (PHY_RX_TD_BASE_ADDRESS + 0x1FC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG080                     (PHY_RX_TD_BASE_ADDRESS + 0x200)
#define	REG_PHY_RX_TD_PHY_RXTD_REG081                     (PHY_RX_TD_BASE_ADDRESS + 0x204)
#define	REG_PHY_RX_TD_PHY_RXTD_REG082                     (PHY_RX_TD_BASE_ADDRESS + 0x208)
#define	REG_PHY_RX_TD_PHY_RXTD_REG083                     (PHY_RX_TD_BASE_ADDRESS + 0x20C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG084                     (PHY_RX_TD_BASE_ADDRESS + 0x210)
#define	REG_PHY_RX_TD_PHY_RXTD_REG085                     (PHY_RX_TD_BASE_ADDRESS + 0x214)
#define	REG_PHY_RX_TD_PHY_RXTD_REG086                     (PHY_RX_TD_BASE_ADDRESS + 0x218)
#define	REG_PHY_RX_TD_PHY_RXTD_REG088                     (PHY_RX_TD_BASE_ADDRESS + 0x220)
#define	REG_PHY_RX_TD_PHY_RXTD_REG089                     (PHY_RX_TD_BASE_ADDRESS + 0x224)
#define	REG_PHY_RX_TD_PHY_RXTD_REG08A                     (PHY_RX_TD_BASE_ADDRESS + 0x228)
#define	REG_PHY_RX_TD_PHY_RXTD_REG08B                     (PHY_RX_TD_BASE_ADDRESS + 0x22C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG08C                     (PHY_RX_TD_BASE_ADDRESS + 0x230)
#define	REG_PHY_RX_TD_PHY_RXTD_REG08D                     (PHY_RX_TD_BASE_ADDRESS + 0x234)
#define	REG_PHY_RX_TD_PHY_RXTD_REG08E                     (PHY_RX_TD_BASE_ADDRESS + 0x238)
#define	REG_PHY_RX_TD_PHY_RXTD_REG08F                     (PHY_RX_TD_BASE_ADDRESS + 0x23C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG090                     (PHY_RX_TD_BASE_ADDRESS + 0x240)
#define	REG_PHY_RX_TD_PHY_RXTD_REG091                     (PHY_RX_TD_BASE_ADDRESS + 0x244)
#define	REG_PHY_RX_TD_PHY_RXTD_REG092                     (PHY_RX_TD_BASE_ADDRESS + 0x248)
#define	REG_PHY_RX_TD_PHY_RXTD_REG093                     (PHY_RX_TD_BASE_ADDRESS + 0x24C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG094                     (PHY_RX_TD_BASE_ADDRESS + 0x250)
#define	REG_PHY_RX_TD_PHY_RXTD_REG095                     (PHY_RX_TD_BASE_ADDRESS + 0x254)
#define	REG_PHY_RX_TD_PHY_RXTD_REG096                     (PHY_RX_TD_BASE_ADDRESS + 0x258)
#define	REG_PHY_RX_TD_PHY_RXTD_REG097                     (PHY_RX_TD_BASE_ADDRESS + 0x25C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG098                     (PHY_RX_TD_BASE_ADDRESS + 0x260)
#define	REG_PHY_RX_TD_PHY_RXTD_REG099                     (PHY_RX_TD_BASE_ADDRESS + 0x264)
#define	REG_PHY_RX_TD_PHY_RXTD_REG09A                     (PHY_RX_TD_BASE_ADDRESS + 0x268)
#define	REG_PHY_RX_TD_PHY_RXTD_REG09B                     (PHY_RX_TD_BASE_ADDRESS + 0x26C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG09C                     (PHY_RX_TD_BASE_ADDRESS + 0x270)
#define	REG_PHY_RX_TD_PHY_RXTD_REG09D                     (PHY_RX_TD_BASE_ADDRESS + 0x274)
#define	REG_PHY_RX_TD_PHY_RXTD_REG09E                     (PHY_RX_TD_BASE_ADDRESS + 0x278)
#define	REG_PHY_RX_TD_PHY_RXTD_REG09F                     (PHY_RX_TD_BASE_ADDRESS + 0x27C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0A0                     (PHY_RX_TD_BASE_ADDRESS + 0x280)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0A1                     (PHY_RX_TD_BASE_ADDRESS + 0x284)
#define	REG_PHY_RX_TD_PHY_TSF_TIMER_HIGH_LOAD_VAL         (PHY_RX_TD_BASE_ADDRESS + 0x288)
#define	REG_PHY_RX_TD_PHY_TSF_TIMER_HIGH                  (PHY_RX_TD_BASE_ADDRESS + 0x28C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0A4                     (PHY_RX_TD_BASE_ADDRESS + 0x290)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0A5                     (PHY_RX_TD_BASE_ADDRESS + 0x294)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0A6                     (PHY_RX_TD_BASE_ADDRESS + 0x298)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RX_TD_PHY_RXTD_REG00 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg : 32; //Sub blocks SW Reset, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg00_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG01 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockEn : 32; //Sub blocks Enable, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg01_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG02 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reg02 : 32; //Sub blocks SW Reset, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg02_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG03 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reg03 : 32; //RxTD Hyperion Control Reg, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg03_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 24;
		uint32 scCyclicPeriod : 3; //sc cyclic period, reset value: 0x5, access type: RW
		uint32 reserved1 : 5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg04_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG05 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bwMode : 2; //bw_mode, reset value: 0x2, access type: CONST
		uint32 hbMode : 2; //hb_mode, reset value: 0x0, access type: RW
		uint32 pmDetectorTriggeredMode : 1; //Detector Triggered based mode on, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 pmAdjecentAnt : 2; //The adjecent channels choose from antenna ID, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmDetectorTriggeredMode2 : 1; //Detector Triggered based mode 2 on, reset value: 0x0, access type: RW
		uint32 reserved2 : 19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg05_u;

/*REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_LOW 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mipsTestPlugLow : 28; //mips test plug low data, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdMipsTestPlugLow_u;

/*REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_MID 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mipsTestPlugMid : 28; //mips test plug mid data, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdMipsTestPlugMid_u;

/*REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_HIGH 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mipsTestPlugHigh : 28; //mips test plug high data, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdMipsTestPlugHigh_u;

/*REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_FOUR 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mipsTestPlugFour : 28; //mips test plug four data, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdMipsTestPlugFour_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0A 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstEnResetAcc : 1; //Rx TD Noise Est Enable Reset Acc, reset value: 0x0, access type: CONST
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0B 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaTimingModeDelayOfdm : 16; //delay in clocks for an OFDM packet, reset value: 0xa00, access type: RW
		uint32 ccaTimingModeDelay11B : 16; //delay in clocks for a 11b packet, reset value: 0xa00, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg0B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0C 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detBypassMeanRemoval : 1; //Detection Mean Removal Bypass., reset value: 0x0, access type: RW
		uint32 detMrbDelayLineLength : 1; //Detection Mean Removal delay line length., reset value: 0x0, access type: RW
		uint32 detMrbMiddleTap : 3; //Detection Mean Removal middle tap., reset value: 0x0, access type: RW
		uint32 detMrbBpfMode : 1; //Detection Mean Removal BPF mode, reset value: 0x1, access type: RW
		uint32 reserved0 : 16;
		uint32 detCorrDL : 4; //Detection correlation-D low tap., reset value: 0x4, access type: RW
		uint32 detCorrDH : 4; //Detection correlation-D high tap., reset value: 0xb, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0D 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 detDiffMode : 1; //Detection correlation-64 differentiators Mode., reset value: 0x0, access type: RW
		uint32 detCntThForCorr16Size : 8; //Detection threshold for correlation-16 size., reset value: 0x0, access type: RW
		uint32 reserved1 : 9;
		uint32 detFineErrCntInit : 11; //Detection fine Error Cnt Init., reset value: 0x0, access type: RW
		uint32 reserved2 : 1;
		uint32 detControlImd : 1; //Use common coarse rdy, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg0D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0E 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detFinePeakCntInit : 8; //Detection Fine timing Peak Cnt Init., reset value: 0x0, access type: RW
		uint32 detFineEnableDelay : 8; //Detection Fine timing Enable Delay., reset value: 0x0, access type: RW
		uint32 detFineDelaySize : 4; //Detection Fine frequency Delay Size., reset value: 0x0, access type: RW
		uint32 detFinePeakHeTrigMode2 : 8; //Detection Fine timing Peak Cnt Init in HE trigger based mode 2, reset value: 0x7, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0F 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detCoarsePeakCntInit : 8; //Detection Coarse timing Peak Cnt Init., reset value: 0x0, access type: RW
		uint32 detCoarseEnableDelay : 8; //Detection Coarse timing Enable Delay., reset value: 0x0, access type: RW
		uint32 detCoarseDelaySize : 5; //Detection Coarse frequency Delay Size., reset value: 0x0, access type: RW
		uint32 detCoarseErrCntInit : 11; //Detection coarse Error Cnt Init., reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg0F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG10 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTopCnt : 6; //Top Detection thershold register 2., reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 detTopTh3 : 15; //Top Detection thershold register 3., reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 detTopCntPri : 6; //Top Detection counter initialization., reset value: 0x0, access type: RW
		uint32 detTopMode : 2; //Top Detection mode., reset value: 0x3, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg10_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG11 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTopBlockCnt : 12; //Top Detection block counter., reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 detMaDec : 4; //Rx TD ma_dec., reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg11_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG12 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htShortOffset : 16; //ht short offset, reset value: 0x4600, access type: RW
		uint32 lastSampleOffset : 16; //last sample offset, reset value: 0x2800, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg12_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG13 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmCcaEnergy01Delay : 12; //delay in clocks of CCA rise event, reset value: 0x0, access type: RW
		uint32 pmCcaEnergy10Delay : 12; //delay in clocks of CCA fall event, reset value: 0x0, access type: RW
		uint32 pmCcaPifsHistClr : 1; //indicates wether LSB is the control channel (1 - LSB,0 - USB), reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegPhyRxTdPhyRxtdReg13_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG15 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctrlChannelCnt : 32; //counter for the control channel, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxTdPhyRxtdReg15_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG16 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extChannelCnt : 32; //counter for the extension channel, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxTdPhyRxtdReg16_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG17 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extChannelCnt40 : 32; //cca statistics counter: saturated count of secondary 40MHz cca activation cycles since last stats_reset event, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxTdPhyRxtdReg17_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG18 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 adcActiveReg : 1; //Rx TD Adc Active., reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg18_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG19 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockAuxChannelSec : 1; //block_aux_channel_sec, reset value: 0x0, access type: RW
		uint32 lsbIsCtrlChannelSec : 1; //lsb_is_ctrl_channel_sec, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg19_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1A 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bandSelectDelay : 8; //MU Antenna Enable. , Host antenna enable bits , can be changed after risc processing., reset value: 0x0, access type: RW
		uint32 bandSelectDelay2B : 8; //Antenna Enable. , Host antenna enable bits , can be changed after risc processing., reset value: 0x0, access type: RW
		uint32 bandSelectDelay4B : 8; //Band Select Delay, reset value: 0x0, access type: RW
		uint32 bandSelectDelay8B : 8; //Band Select Delay 2 Band, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg1A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1B 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stmAbortThr : 16; //Stm Abort Threshold, reset value: 0xfff0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1C 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 pmForceDcEstMode : 2; //pm_force_dc_est_mode, reset value: 0x0, access type: RW
		uint32 pmAllowFallbackToDcAvgIfHtstfNotValid : 1; //pm_allow_fallback_to_dc_avg_if_htstf_not_valid, reset value: 0x0, access type: RW
		uint32 pmEnableBinErasing : 1; //pm_enable_bin_erasing, reset value: 0x0, access type: RW
		uint32 bypassDcAll : 1; //Bypass Dc Removal, reset value: 0x0, access type: RW
		uint32 pmAllowFallbackToDcAvgIfHtstfValid : 1; //pm_allow_fallback_to_dc_avg_if_htstf_valid, reset value: 0x0, access type: RW
		uint32 pmAllowFallbackToDcGzlIfHtstfValid : 1; //pm_allow_fallback_to_dc_gzl_if_htstf_valid, reset value: 0x0, access type: RW
		uint32 pmAllowFallbackToDcGzlIfHtstfNotValid : 1; //pm_allow_fallback_to_dc_gzl_if_htstf_not_valid, reset value: 0x0, access type: RW
		uint32 reserved1 : 23;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG01D 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rssiSamplePolarity : 1; //Rssi Sample Polarity., reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 rssiStrbRate : 1; //ESSI strobe threshold: , 0 : for 40Mhz , 1: for 20Mhz, reset value: 0x0, access type: RW
		uint32 reserved1 : 27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg01D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG01E 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 parallelMode : 1; //Rx TD Hyp Pif Parallel Mode., reset value: 0x0, access type: CONST
		uint32 reserved1 : 27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg01E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG01F 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deltaFCmpEn : 1; //Delta F Cmp En., reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 deltaFFineErrEnable : 1; //Delta F fine error enable., reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 coarseFreqWeight : 4; //coarse_freq_weight, reset value: 0x0, access type: RW
		uint32 reserved2 : 20;
	} bitFields;
} RegPhyRxTdPhyRxtdReg01F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG021 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 riscPage : 1; //RIsc Page Address., reset value: 0x0, access type: RW
		uint32 reserved1 : 27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg021_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG022 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscStartOp : 1; //Rx TD RIsc Start Opcode., reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg022_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG023 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdTestPlugLenAn0 : 25; //Test Plug Local Enable., reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegPhyRxTdPhyRxtdReg023_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG024 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdTestPlugLenAn1 : 25; //Test Plug Local Enable., reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegPhyRxTdPhyRxtdReg024_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG025 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdTestPlugLenAn2 : 25; //Test Plug Local Enable., reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegPhyRxTdPhyRxtdReg025_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG026 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdTestPlugLenAn3 : 25; //Test Plug Local Enable., reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegPhyRxTdPhyRxtdReg026_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG027 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate : 32; //Rx TD Sw Reset Generate, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg027_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG028 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmSwResetMask : 32; //Rx TD Gsm Sw Reset., reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg028_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG029 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxRdyRiseDelay11A : 14; //rx_rdy_rise_delay_11a, reset value: 0x1180, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegPhyRxTdPhyRxtdReg029_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG02A 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxRdyRiseDelay11N : 14; //rx_rdy_rise_delay_11n, reset value: 0x2A80, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegPhyRxTdPhyRxtdReg02A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG02B 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxRdyFallDelay : 14; //rx_rdy_fall_delay, reset value: 0x2080, access type: RW
		uint32 reserved0 : 2;
		uint32 rxRdyFallDelay11B : 14; //rx_rdy_fall_delay, reset value: 0x1180, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegPhyRxTdPhyRxtdReg02B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG02C 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopSignalTh : 13; //stop_signal_th, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg02C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG02D 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extendAirTimeOffset : 16; //extend_air_time_offset, reset value: 0x0, access type: RW
		uint32 extendAirTimeOffsetEn : 1; //extend_air_time_offset_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 15;
	} bitFields;
} RegPhyRxTdPhyRxtdReg02D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2E 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdTestBusGclkEn : 1; //td_test_bus_gclk_en , reset value: 0x0, access type: RW
		uint32 beTestBusGclkEn : 1; //be_test_bus_gclk_en , reset value: 0x0, access type: RW
		uint32 slicerTestBusGclkEn : 1; //slicer_test_bus_gclk_en , reset value: 0x0, access type: RW
		uint32 fdTestBusGclkEn : 1; //fd_test_bus_gclk_en , reset value: 0x0, access type: RW
		uint32 txTestBusGclkEn : 1; //tx_test_bus_gclk_en , reset value: 0x0, access type: RW
		uint32 loggerTestBusGclkEn : 1; //logger_test_bus_gclk_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 swBistStart : 1; //sw_bist_start, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 clearRamMode : 1; //clear_ram_mode, reset value: 0x0, access type: RW
		uint32 reserved2 : 19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2F 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkBypassEn : 32; //gclk bypass - when '1' clocks will be always active, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg2F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG30 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 tbFifoClear : 1; //Test Bus FIFO Clear, reset value: 0x0, access type: WO
		uint32 reserved1 : 27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg30_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG31 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testFifoCaptureMode : 1; //test_fifo_capture_mode, reset value: 0x0, access type: RW
		uint32 testFifoCyclicMode : 1; //test_fifo_cyclic_mode, reset value: 0x0, access type: RW
		uint32 testFifoExternalTrigEn : 1; //test_fifo_external_trig_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 testBusLowMuxCtrl : 2; //test_bus_low_mux_ctrl, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 testBusMidMuxCtrl : 2; //test_bus_mid_mux_ctrl, reset value: 0x1, access type: RW
		uint32 reserved2 : 2;
		uint32 testBusHighMuxCtrl : 2; //test_bus_high_mux_ctrl, reset value: 0x2, access type: RW
		uint32 reserved3 : 2;
		uint32 testBusFourMuxCtrl : 2; //test_bus_four_mux_ctrl, reset value: 0x3, access type: RW
		uint32 reserved4 : 14;
	} bitFields;
} RegPhyRxTdPhyRxtdReg31_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG032 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscSramRm : 3; //gen_risc_sram_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 genRiscIramRm : 3; //gen_risc_iram_rm, reset value: 0x3, access type: RW
		uint32 reserved1 : 17;
		uint32 memConverterRm : 3; //mem_converter_rm, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 testFifoRamRm : 3; //test_fifo_large_ram_low_rm, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg032_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG033 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcTableRm : 3; //dc_table_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 dcTable2Rm : 3; //dc_table2_rm, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 agcTableRm : 3; //agc_table_rm, reset value: 0x3, access type: RW
		uint32 reserved2 : 21;
	} bitFields;
} RegPhyRxTdPhyRxtdReg033_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG034 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 switch11BCounterLimit : 21; //switch_11b_counter_limit, reset value: 0x40000, access type: RW
		uint32 reserved0 : 11;
	} bitFields;
} RegPhyRxTdPhyRxtdReg034_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG035 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcCalcDecisionParams : 32; //dc_calc_decision_params, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxTdPhyRxtdReg035_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG036 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bistScrBypass : 1; //bist_scr_bypass, reset value: 0x0, access type: RW
		uint32 ramTestMode : 1; //ram_test_mode, reset value: 0x0, access type: RW
		uint32 memGlobalRm : 2; //mem_global_rm, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdPhyRxtdReg036_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG037 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectorMrbRm : 3; //detector_mrb_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 detectorC16Rm : 3; //detector_c16_rm, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 detectorC64Rm : 3; //detector_c64_rm, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 detectorCfbRm : 3; //detector_cfb_rm, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 detectorFfbRm : 3; //detector_ffb_rm, reset value: 0x3, access type: RW
		uint32 reserved4 : 13;
	} bitFields;
} RegPhyRxTdPhyRxtdReg037_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG038 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 disableSecDetection : 1; //disable_sec_detection, reset value: 0x0, access type: RW
		uint32 disablePrimaryDetection : 1; //disable_primary_detection, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 detectionOnLltfBackoff : 4; //detection_on_lltf_backoff, reset value: 0x4, access type: RW
		uint32 pmDetectOnLtfCorrDDisable : 1; //pm_detect_on_ltf_corr_d_disable, reset value: 0x1, access type: RW
		uint32 reserved1 : 23;
	} bitFields;
} RegPhyRxTdPhyRxtdReg038_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG039 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 watchDogLimit : 32; //watch dog limit, reset value: 0xffffff, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg039_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG03A 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 adjacentRejOnSecEn : 1; //adjacent_rej_on_sec_en, reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 adjacentRejOnOobEn : 1; //adjacent_rej_on_oob_en, reset value: 0x1, access type: RW
		uint32 reserved1 : 3;
		uint32 powerDivOobTh : 11; //power_div_oob_th, reset value: 0x10, access type: RW
		uint32 reserved2 : 1;
		uint32 powerDivIbTh : 11; //power_div_ib_th, reset value: 0x64, access type: RW
		uint32 reserved3 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg03A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG03B 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 midPacketEn : 1; //cca_mid_long_dec, reset value: 0x1, access type: RW
		uint32 reserved0 : 7;
		uint32 midPacketAnt : 2; //mid_packet_ant, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmCcaCombineSec20 : 1; //cca combine Secondary 20 in Primary, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 pmCcaMidDetThTime : 14; //cca_mid_det_energy_pass_th debounce time, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxTdPhyRxtdReg03B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG03C 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 midDcIirIsBypassed : 1; //mid_dc_iir_is_bypassed , reset value: 0x0, access type: RW
		uint32 midOobDcIirIsBypassed : 1; //mid_oob_dc_iir_is_bypassed , reset value: 0x1, access type: RW
		uint32 reserved0 : 2;
		uint32 midDcIirAlpha : 4; //mid_dc_iir_alpha , reset value: 0x7, access type: RW
		uint32 ccaSecMidPowerTh : 16; //cca_sec_mid_power_th, reset value: 0x19, access type: RW
		uint32 ccaMidMaxThPasses : 7; //cca_mid_max_th_passes, reset value: 0x7f, access type: RW
		uint32 reserved1 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg03C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG03D 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmCcaMidTh20Sec : 8; //cca mid power threshold Secondary 20MHz , reset value: 0xB8, access type: RW
		uint32 reserved0 : 4;
		uint32 pmCcaMidTh40 : 8; //cca mid power threshold Secondary 40MHz , reset value: 0xB8, access type: RW
		uint32 reserved1 : 4;
		uint32 pmCcaMidTh20Prim : 8; //cca mid power threshold Primary 20MHz , reset value: 0xB8, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg03D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG03E 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmCcaPifs1TimeLen : 15; //cca PIFS1 time length (clock cycles), reset value: 0x1f40, access type: RW
		uint32 reserved0 : 1;
		uint32 pmCcaWifiDetSecTime : 15; //CCA Secondary WiFi Detector Fall Hysteresis (on-shot)  of 24us, reset value: 0x1e00, access type: RW
		uint32 pmCcaWifiDetSecEnable : 1; //CCA enable OFDM WiFi Detector of the secondary bands, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg03E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG03F 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaMpdDramRm : 3; //cca_mpd_dram_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 ccaMpdSramRm : 3; //cca_mpd_sram_rm, reset value: 0x3, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegPhyRxTdPhyRxtdReg03F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG040 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fftExtraGain20 : 2; //fft_extra_gain, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 fftExtraGain40 : 2; //fft_extra_gain, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 fftExtraGain80 : 2; //fft_extra_gain, reset value: 0x0, access type: RW
		uint32 fftExtraGain160 : 2; //no description, reset value: 0x0, access type: RW
		uint32 reserved2 : 20;
	} bitFields;
} RegPhyRxTdPhyRxtdReg040_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG041 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmCcaPifs2Start : 4; //CCA PIFS2 detection time start (us), reset value: 0x8, access type: RW
		uint32 pmCcaPifs2End : 6; //CCA PIFS2 detection time n end (us), reset value: 0x21, access type: RW
		uint32 reserved0 : 2;
		uint32 pmCcaReleasePifs2 : 1; //release pifs2 freeze, reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg041_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG042 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaEnergy10DelayPrim : 14; //cca_energy_1_0_delay_prim, reset value: 0x0, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegPhyRxTdPhyRxtdReg042_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG043 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decisionMask : 8; //det_th_1ant_pri, reset value: 0xf, access type: RW
		uint32 detCombTh : 2; //det_th_1ant_pri, reset value: 0x3, access type: RW
		uint32 coChMode : 1; //det_th_1ant_pri, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 coChRestartLen : 10; //det_th_1ant_pri, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 detCombTh8Bands : 3; //det_th_1ant_pri, reset value: 0x3, access type: RW
		uint32 reserved2 : 5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg043_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG044 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectionMask0 : 4; //detecttiom mask, reset value: 0xf, access type: RW
		uint32 detectionMask1 : 4; //detecttiom mask, reset value: 0x0, access type: RW
		uint32 detectionMask2 : 4; //detecttiom mask, reset value: 0x0, access type: RW
		uint32 detectionMask3 : 4; //detecttiom mask, reset value: 0x0, access type: RW
		uint32 detectionMask4 : 4; //detecttiom mask, reset value: 0x0, access type: RW
		uint32 detectionMask5 : 4; //detecttiom mask, reset value: 0x0, access type: RW
		uint32 detectionMask6 : 4; //detecttiom mask, reset value: 0x0, access type: RW
		uint32 detectionMask7 : 4; //detecttiom mask, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg044_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG045 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTh1AntPri : 8; //det_th_1ant_pri, reset value: 0x0, access type: RW
		uint32 detTh2AntPri : 8; //det_th_2ant_pri, reset value: 0x0, access type: RW
		uint32 detTh3AntPri : 8; //det_th_3ant_pri, reset value: 0x0, access type: RW
		uint32 detTh4AntPri : 8; //det_th_4ant_pri, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg045_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG046 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTh1AntSec : 8; //det_th_1ant_sec, reset value: 0x0, access type: RW
		uint32 detTh2AntSec : 8; //det_th_2ant_sec, reset value: 0x0, access type: RW
		uint32 detTh3AntSec : 8; //det_th_3ant_sec, reset value: 0x0, access type: RW
		uint32 detTh4AntSec : 8; //det_th_4ant_sec, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg046_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG047 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaSecMidDetTh : 8; //cca_sec_mid_det_th, reset value: 0xc0, access type: RW
		uint32 ccaSecMidDetShortTh : 8; //cca_sec_mid_det_short_th, reset value: 0xcd, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg047_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG048 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr0 : 32; //spare_gpr_0, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg048_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG049 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr1 : 32; //spare_gpr_1, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg049_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04A 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr2 : 32; //spare_gpr_2, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg04A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04B 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr3 : 32; //spare_gpr_3, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg04B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04C 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detCoarseCI : 22; //det_coarse_c_i, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPhyRxTdPhyRxtdReg04C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04D 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detCoarseCQ : 22; //det_coarse_c_q, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPhyRxTdPhyRxtdReg04D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04E 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstAccOutAn0 : 31; //Rx TD Noise Estimation Accumulator Data An0, reset value: 0x2, access type: RO
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg04E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04F 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstAccOutAn1 : 31; //Rx TD Noise Estimation Accumulator Data An1, reset value: 0x2, access type: RO
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg04F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG050 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstAccOutAn2 : 31; //Rx TD Noise Estimation Accumulator Data An2, reset value: 0x2, access type: RO
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg050_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG051 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstAccOutAn3 : 31; //Rx TD Noise Estimation Accumulator Data An3, reset value: 0x2, access type: RO
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg051_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG052 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSumIn0 : 31; //short_sum_in0, reset value: 0x7fffffff, access type: RW
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg052_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG053 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSumIn1 : 31; //short_sum_in1, reset value: 0x7fffffff, access type: RW
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg053_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG054 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSumIn2 : 31; //short_sum_in2, reset value: 0x7fffffff, access type: RW
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg054_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG055 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSumIn3 : 31; //short_sum_in3, reset value: 0x7fffffff, access type: RW
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg055_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG056 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 thresholdMaxCorr : 12; //threshold_max_corr, reset value: 0x1, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdPhyRxtdReg056_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG057 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 thresholdMaxCorrAdj160 : 12; //threshold_max_corr_adj_160, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
		uint32 thresholdMaxCorrAdj80 : 12; //threshold_max_corr_adj_80, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg057_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG058 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 thresholdMaxCorrAdj20 : 12; //threshold_max_corr_adj_20, reset value: 0x1, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdPhyRxtdReg058_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG059 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 12;
		uint32 thresholdMaxCorrAdj40 : 12; //threshold_max_corr_adj_40, reset value: 0x1, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg059_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG05A 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 24;
		uint32 detFineTimingContinueAfterRdy : 8; //det_fine_timing_continue_after_rdy, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg05A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG05B 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTh1AntCoCh : 8; //det_th_1ant_co_ch, reset value: 0x0, access type: RW
		uint32 detTh2AntCoCh : 8; //det_th_2ant_co_ch, reset value: 0x0, access type: RW
		uint32 detTh3AntCoCh : 8; //det_th_3ant_co_ch, reset value: 0x0, access type: RW
		uint32 detTh4AntCoCh : 8; //det_th_4ant_co_ch, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg05B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG05C 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sig3ReadyOffset : 16; //offset in clk from ht short to sig3 ready, reset value: 0x1f5e, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg05C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG05D 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCnt : 32; //free_running_cnt, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxTdPhyRxtdReg05D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG05E 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmCcaValidDetTimer : 8; //timer of 1.25us in 20Mhz from sec detect till primary, reset value: 0x32, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdPhyRxtdReg05E_u;

/*REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_MSG 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscTestPlugMsg : 32; //risc data to logger, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdRiscTestPlugMsg_u;

/*REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_MSG_DONE 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscTestPlugMsgDone : 1; //risc data to logger, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdPhyRxtdRiscTestPlugMsgDone_u;

/*REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_FAST_MSG 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscTestPlugFastMsg : 32; //risc data to logger, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdRiscTestPlugFastMsg_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG062 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 forceDetectionOnBands : 8; //force_detection_on_bands, reset value: 0x0, access type: RW
		uint32 blockDetectionOnBands : 8; //block_detection_on_bands, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg062_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG063 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn0Corr64AtCommonFineT : 28; //Detector an0 Abs corr64 smoothed at fine timing, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg063_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG064 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn1Corr64AtCommonFineT : 28; //Detector an1 Abs corr64 smoothed at fine timing, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg064_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG065 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn2Corr64AtCommonFineT : 28; //Detector an2 Abs corr64 smoothed at fine timing, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg065_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG066 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn3Corr64AtCommonFineT : 28; //Detector an3 Abs corr64 smoothed at fine timing, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg066_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG067 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsCommonCorr64AtCommonFineT : 28; //Detector common Abs corr64 smoothed at fine timing, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg067_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG068 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn0Corr16AtLocalCoarseT : 20; //Detector an0 Abs corr64 smoothed at fine timing, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg068_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG069 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn1Corr16AtLocalCoarseT : 20; //Detector an1 Abs corr64 smoothed at fine timing, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg069_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06A 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn2Corr16AtLocalCoarseT : 20; //Detector an2 Abs corr64 smoothed at fine timing, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06B 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn3Corr16AtLocalCoarseT : 20; //Detector an3 Abs corr64 smoothed at fine timing, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06C 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsCommonCorr16AtCommonCoarseT : 20; //Detector common Abs corr64 smoothed at fine timing, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG_LOOPBACK_CTRL 0x1B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 digitalLpbk : 1; //digital LoopBack Mode, reset value: 0x0, access type: RW
		uint32 reversedDigitalLpbk : 1; //reveresed digital LoopBack Mode, reset value: 0x0, access type: RW
		uint32 reversedDigitalLpbkRssi : 1; //reversed LoopBack Mode rssi, reset value: 0x0, access type: RW
		uint32 tx0LpbkSelect : 1; //Tx0 LoopBack Select, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdPhyRxtdRegLoopbackCtrl_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06E 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpTimingEnable : 1; //cp_timing_enable, reset value: 0x1, access type: RW
		uint32 cpTimingSkipTimingFromSig : 2; //cp_timing_skip_timing_from_sig, reset value: 0x1, access type: RW
		uint32 cpTimingSkipTimingFromLtfs : 2; //cp_timing_skip_timing_from_ltfs, reset value: 0x2, access type: RW
		uint32 cpTimingSkipTimingFromData : 2; //cp_timing_skip_timing_from_data, reset value: 0x2, access type: RW
		uint32 reserved0 : 1;
		uint32 cpTimingNSymToAvg : 7; //cp_timing_n_sym_to_avg, reset value: 0xa, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06F 0x1BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpTimingWindowSize : 5; //cp_timing_window_size, reset value: 0x14, access type: RW
		uint32 cpTimingMinimalLtfsForEstimation : 3; //cp_timing)minimal_ltfs_for_estimation, reset value: 0x0, access type: RW
		uint32 cpTimingSync : 6; //cp_timing_sync, reset value: 0x1a, access type: RW
		uint32 reserved0 : 2;
		uint32 cpTimingWindowSizeForSigEstimation : 5; //cp_timing_window_size_for_sig_estimation, reset value: 0x14, access type: RW
		uint32 reserved1 : 11;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG070 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpTimingShiftFromLookahead : 6; //cp_timing_shift_from_lookahead, reset value: 0x38, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyRxTdPhyRxtdReg070_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG071 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 m11BBLongOffset : 20; //m11b_b_long_offset, reset value: 0x1E6C0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg071_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG072 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTopTh3ForLtf : 15; //Top Detection thershold register 3 for ltf detection., reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyRxTdPhyRxtdReg072_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG076 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPacketCounter : 27; //cca_packet_counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg076_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG077 0x1DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detCoarseGclkEnCb : 1; //det_coarse_gclk_en_cb, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg077_u;

/*REG_PHY_RX_TD_PHY_TSF_TIMER_CONTROL 0x1E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyTsfTimerEn : 1; //phy_tsf_timer_en, reset value: 0x0, access type: RW
		uint32 phyTsfTimerClear : 1; //phy_tsf_timer_clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 2;
		uint32 phyTsfTimerUnit : 10; //phy_tsf_timer_unit, reset value: 0x280, access type: RW
		uint32 reserved1 : 2;
		uint32 phyTsfTimerLoad : 1; //phy_tsf_timer_load, reset value: 0x0, access type: WO
		uint32 reserved2 : 15;
	} bitFields;
} RegPhyRxTdPhyTsfTimerControl_u;

/*REG_PHY_RX_TD_PHY_TSF_TIMER 0x1E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyTsfTimer : 32; //phy_tsf_timer, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxTdPhyTsfTimer_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG07A 0x1E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmCcaPdTh20Prim : 8; //CCA primary 20MHz power threshold value, reset value: 0xC2, access type: RW
		uint32 pmCcaPdTh20Sec : 8; //CCA Secondary 20MHz power threshold value, reset value: 0xC2, access type: RW
		uint32 pmCcaPdTh40 : 8; //CCA Secondary 40MHz power threshold value, reset value: 0xC5, access type: RW
		uint32 pmCcaPdTh80 : 8; //CCA Secondary 80MHz power threshold value, reset value: 0xC5, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg07A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG07B 0x1EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 hostAntennaEn : 4; //Antenna Enable. , Host antenna enable bits , can be changed after risc processing., reset value: 0x0, access type: RW
		uint32 reserved1 : 24;
	} bitFields;
} RegPhyRxTdPhyRxtdReg07B_u;

/*REG_PHY_RX_TD_PHY_TSF_TIMER_LOAD_VAL 0x1F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyTsfTimerLoadVal : 32; //init value, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxTdPhyTsfTimerLoadVal_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG07D 0x1F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lsbIsCtrlChannel : 1; //delay in clocks of CCA rise event, reset value: 0x0, access type: RW
		uint32 lsb40IsCtrlChannel : 1; //delay in clocks of CCA fall event, reset value: 0x0, access type: RW
		uint32 lsb80IsCtrlChannel : 1; //indicates wether LSB is the control channel (1 - LSB,0 - USB), reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 blockAuxChannel : 1; //determines which 40m band is the primary (1 - LSB,0 - USB), reset value: 0x0, access type: RW
		uint32 blockAuxChannel40 : 1; //block_aux_channel, reset value: 0x0, access type: RW
		uint32 blockAuxChannel80 : 1; //block_aux_channel40, reset value: 0x0, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegPhyRxTdPhyRxtdReg07D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG07E 0x1F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDetSorpDelay : 8; //detecttion_s_or_p delay indication - starts the C64 process. For CSIm calibration, reset value: 0x0, access type: RW
		uint32 pmDetCt2FtDelay : 8; // coarse-timing to start fine-timing delay. For CSIM calibration, reset value: 0x0, access type: RW
		uint32 pmDetC16RestartDelay : 8; //delay from coarse-timing to C16 restart. For CSIM calibration, reset value: 0x0, access type: RW
		uint32 pmDetCnt2DetCalibrate : 4; //delay the detect-ready assert for CSIM vs RTL bit-acc, reset value: 0x0, access type: RW
		uint32 pmDetDisableMaskChAfterRdy : 1; //disable the feature that mask the channels after detection_ready., reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
	} bitFields;
} RegPhyRxTdPhyRxtdReg07E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG07F 0x1FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 thresholdMaxCorrCoChannel20 : 12; //threshold_max_corr_co_channel_20, reset value: 0x1, access type: RW
		uint32 thresholdMaxCorrCoChannel40 : 12; //threshold_max_corr_co_channel_40, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg07F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG080 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muHostAntennaEn : 4; //MU Antenna Enable. , Host antenna enable bits , can be changed after risc processing., reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdPhyRxtdReg080_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG081 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDoneDft : 1; //end of trigger frame transmit, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg081_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG082 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 triggerBasedCnt : 16; //count from end of tx trigger frame to start of he mu rx, reset value: 0x2800, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg082_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG083 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delayCoarseReadyForDcStart : 6; //delay coarse indication for dc calculation, reset value: 0xD, access type: RW
		uint32 reserved0 : 2;
		uint32 detCoarseOffsetHeTrigMode2 : 6; //det_coarse_offset_he_trig_mode_2, reset value: 0x7, access type: RW
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyRxTdPhyRxtdReg083_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG084 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minNoiseEst : 31; //Rx TD Min Noise Estimation, reset value: 0x2, access type: RO
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg084_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG085 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcCfoThresholdRm : 3; //dc_cfo_threshold_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxTdPhyRxtdReg085_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG086 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmSnrThrForDc0 : 8; //snr threshold for dc, reset value: 0x40, access type: RW
		uint32 pmSnrThrForDc1 : 8; //snr threshold for dc, reset value: 0x60, access type: RW
		uint32 pmSnrThrForDc2 : 8; //snr threshold for dc, reset value: 0x85, access type: RW
		uint32 pmSnrThrForDc3 : 8; //snr threshold for dc, reset value: 0xBA, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg086_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG088 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cdbAntSel : 4; //cdb_ant_sel, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdPhyRxtdReg088_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG089 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 snrEstAn0 : 10; //Rx TD SNR Estimation Data An0, reset value: 0x0, access type: RO
		uint32 snrEstAn1 : 10; //Rx TD SNR Estimation Data An1, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg089_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG08A 0x228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 snrEstAn2 : 10; //Rx TD SNR Estimation Data An2, reset value: 0x0, access type: RO
		uint32 snrEstAn3 : 10; //Rx TD SNR Estimation Data An3, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg08A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG08B 0x22C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deltaFDataReg : 20; //Sanity Delta F Data, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg08B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG08C 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 accumulatorResetValue : 18; //accumulator_reset_value, reset value: 0x0, access type: RW
		uint32 reserved0 : 14;
	} bitFields;
} RegPhyRxTdPhyRxtdReg08C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG08D 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSnrIn0 : 10; //short_snr_in0, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg08D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG08E 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSnrIn1 : 10; //short_snr_in1, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg08E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG08F 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSnrIn2 : 10; //short_snr_in2, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg08F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG090 0x240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSnrIn3 : 10; //short_snr_in3, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg090_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG091 0x244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testBusFifoConfig : 4; //‘1’ = Use memory , ‘0’ = memory disconnected ,  , [0] - FIFO 0 , [1] - FIFO 1 , [2] - SU BF 0 , [3] - SU BF 1, reset value: 0x0, access type: RW
		uint32 testBusFifoConfigError : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg091_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG092 0x248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 m11BBShortOffset : 20; //m11b_b_short_offset, reset value: 0xF6C0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg092_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG093 0x24C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sig3ReadyOffsetHe : 16; //offset in clk from ht short to he sig3 ready, reset value: 0x295e, access type: RW
		uint32 sig3ReadyOffsetHeExt : 16; //offset in clk from ht short to he sig3 ready ext, reset value: 0x3d5e, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg093_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG094 0x250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heShortOffset : 16; //he short offset, reset value: 0x5000, access type: RW
		uint32 heShortOffsetExt : 16; //he short offset ext, reset value: 0x6400, access type: RW
	} bitFields;
} RegPhyRxTdPhyRxtdReg094_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG095 0x254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmAcidetAciAntenna : 2; //pm_acidet_aci_antenna, reset value: 0x0, access type: RW
		uint32 pmAcidetAciLocal : 1; //pm_acidet_aci_local, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxTdPhyRxtdReg095_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG096 0x258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tbBytesInFifo : 17; //no description, reset value: 0x0, access type: RO
		uint32 loggerTdNextSeqAlowed : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegPhyRxTdPhyRxtdReg096_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG097 0x25C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmCcaBusySwPrim20 : 1; //pm_cca_busy_sw_prim20, reset value: 0x0, access type: RW
		uint32 pmCcaBusySwSec20 : 1; //pm_cca_busy_sw_sec20, reset value: 0x0, access type: RW
		uint32 pmCcaBusySwSec40 : 1; //pm_cca_busy_sw_sec40 , reset value: 0x0, access type: RW
		uint32 pmCcaBusySwSec80 : 1; //pm_cca_busy_sw_sec80 , reset value: 0x0, access type: RW
		uint32 pmCcaBusySw20Sb : 8; //pm_cca_busy_sw_20sb , reset value: 0x0, access type: RW
		uint32 pmCcaSelBusyHw : 1; //pm_cca_sel_busy_hw� , reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg097_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG098 0x260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmCcaClearSw : 1; //pm_cca_clear_sw, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 pmCcaClearHwEn : 1; //clear hw enable, reset value: 0x0, access type: RW
		uint32 reserved1 : 27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg098_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG099 0x264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testBusFifoDumpLast : 1; //write last samples to the fifo, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg099_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG09A 0x268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDetTxShortPreamCnt : 16; //The count from reset till TX-Preamble internal generation, reset value: 0x1e8, access type: RW
		uint32 pmDetTxShortPreamByp : 1; //bypass TX-Preamble and generate it internally , reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 pmDetRcrBwBypass : 1; //pm_det_rcr_bw_bypass� , reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 pmDetRcrBwValue : 2; //pm_det_rcr_bw_value , reset value: 0x0, access type: RW
		uint32 reserved2 : 6;
	} bitFields;
} RegPhyRxTdPhyRxtdReg09A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG09B 0x26C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmSnrUpdate : 1; //pm_snr_update, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg09B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG09C 0x270 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 disableFreqEst : 1; //disable_freq_est, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg09C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG09D 0x274 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fftFifoHostAntSel : 2; //fft_fifo_host_ant_sel, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg09D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG09E 0x278 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugSdSel : 2; //fft_fifo_host_ant_sel, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg09E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG09F 0x27C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugFifoSel : 2; //fft_fifo_host_ant_sel, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg09F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0A0 0x280 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearHeTrigIndication : 1; //clear_he_trig_indication, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0A0_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0A1 0x284 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hestfFftGainOptStartPoint : 11; //hestf_fft_gain_opt_start_point, reset value: 0x280, access type: RW
		uint32 reserved0 : 1;
		uint32 hestfFftGainOptEndPoint : 11; //hestf_fft_gain_opt_end_point, reset value: 0x300, access type: RW
		uint32 reserved1 : 9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0A1_u;

/*REG_PHY_RX_TD_PHY_TSF_TIMER_HIGH_LOAD_VAL 0x288 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyTsfTimerHighLoadVal : 8; //init high value, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdPhyTsfTimerHighLoadVal_u;

/*REG_PHY_RX_TD_PHY_TSF_TIMER_HIGH 0x28C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyTsfTimerHigh : 8; //tsf timer bits 39:32, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdPhyTsfTimerHigh_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0A4 0x290 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSig30 : 26; //he_sig3_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 heSig3CrcErrCalcStart : 1; //he_sig3_crc_err_calc_start, reset value: 0x0, access type: WO
	} bitFields;
} RegPhyRxTdPhyRxtdReg0A4_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0A5 0x294 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSig31 : 26; //he_sig3_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 6;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0A5_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0A6 0x298 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSig3CrcErrCalcDone : 1; //he_sig3_crc_err_calc_done, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 heSig3CrcErrVal : 1; //he_sig3_crc_err_val, reset value: 0x0, access type: RO
		uint32 reserved1 : 3;
		uint32 heSig3CrcCalcVal : 8; //he_sig3_crc_calc_val, reset value: 0x0, access type: RO
		uint32 reserved2 : 16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0A6_u;



#endif // _PHY_RX_TD_REGS_H_
