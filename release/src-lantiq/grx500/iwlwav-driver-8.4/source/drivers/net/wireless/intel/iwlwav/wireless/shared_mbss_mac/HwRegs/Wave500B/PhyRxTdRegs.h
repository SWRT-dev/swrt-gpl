
/***********************************************************************************
File:				PhyRxTdRegs.h
Module:				phyRxTd
SOC Revision:		843
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
#define	REG_PHY_RX_TD_PHY_RXTD_REG06                      (PHY_RX_TD_BASE_ADDRESS + 0x18)
#define	REG_PHY_RX_TD_PHY_RXTD_REG07                      (PHY_RX_TD_BASE_ADDRESS + 0x1C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG09                      (PHY_RX_TD_BASE_ADDRESS + 0x24)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0A                      (PHY_RX_TD_BASE_ADDRESS + 0x28)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0B                      (PHY_RX_TD_BASE_ADDRESS + 0x2C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0C                      (PHY_RX_TD_BASE_ADDRESS + 0x30)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0D                      (PHY_RX_TD_BASE_ADDRESS + 0x34)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0F                      (PHY_RX_TD_BASE_ADDRESS + 0x3C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG11                      (PHY_RX_TD_BASE_ADDRESS + 0x44)
#define	REG_PHY_RX_TD_PHY_RXTD_REG13                      (PHY_RX_TD_BASE_ADDRESS + 0x4C)
#define	REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_LOW         (PHY_RX_TD_BASE_ADDRESS + 0x50)
#define	REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_MID         (PHY_RX_TD_BASE_ADDRESS + 0x54)
#define	REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_HIGH        (PHY_RX_TD_BASE_ADDRESS + 0x58)
#define	REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_FOUR        (PHY_RX_TD_BASE_ADDRESS + 0x5C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG22                      (PHY_RX_TD_BASE_ADDRESS + 0x88)
#define	REG_PHY_RX_TD_PHY_RXTD_REG24                      (PHY_RX_TD_BASE_ADDRESS + 0x90)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A                      (PHY_RX_TD_BASE_ADDRESS + 0xA8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2C                      (PHY_RX_TD_BASE_ADDRESS + 0xB0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2E                      (PHY_RX_TD_BASE_ADDRESS + 0xB8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2F                      (PHY_RX_TD_BASE_ADDRESS + 0xBC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG30                      (PHY_RX_TD_BASE_ADDRESS + 0xC0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG31                      (PHY_RX_TD_BASE_ADDRESS + 0xC4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG32                      (PHY_RX_TD_BASE_ADDRESS + 0xC8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG33                      (PHY_RX_TD_BASE_ADDRESS + 0xCC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG34                      (PHY_RX_TD_BASE_ADDRESS + 0xD0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG35                      (PHY_RX_TD_BASE_ADDRESS + 0xD4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG36                      (PHY_RX_TD_BASE_ADDRESS + 0xD8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG37                      (PHY_RX_TD_BASE_ADDRESS + 0xDC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG38                      (PHY_RX_TD_BASE_ADDRESS + 0xE0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG39                      (PHY_RX_TD_BASE_ADDRESS + 0xE4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG3A                      (PHY_RX_TD_BASE_ADDRESS + 0xE8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG3B                      (PHY_RX_TD_BASE_ADDRESS + 0xEC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG3C                      (PHY_RX_TD_BASE_ADDRESS + 0xF0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG3D                      (PHY_RX_TD_BASE_ADDRESS + 0xF4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG3E                      (PHY_RX_TD_BASE_ADDRESS + 0xF8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG041                     (PHY_RX_TD_BASE_ADDRESS + 0x104)
#define	REG_PHY_RX_TD_PHY_RXTD_REG04B                     (PHY_RX_TD_BASE_ADDRESS + 0x12C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG04C                     (PHY_RX_TD_BASE_ADDRESS + 0x130)
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
#define	REG_PHY_RX_TD_PHY_RXTD_REG05E                     (PHY_RX_TD_BASE_ADDRESS + 0x178)
#define	REG_PHY_RX_TD_PHY_RXTD_REG05F                     (PHY_RX_TD_BASE_ADDRESS + 0x17C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG060                     (PHY_RX_TD_BASE_ADDRESS + 0x180)
#define	REG_PHY_RX_TD_PHY_RXTD_REG061                     (PHY_RX_TD_BASE_ADDRESS + 0x184)
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
#define	REG_PHY_RX_TD_PHY_RXTD_REG06D                     (PHY_RX_TD_BASE_ADDRESS + 0x1B4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG06E                     (PHY_RX_TD_BASE_ADDRESS + 0x1B8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG06F                     (PHY_RX_TD_BASE_ADDRESS + 0x1BC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG073                     (PHY_RX_TD_BASE_ADDRESS + 0x1CC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG074                     (PHY_RX_TD_BASE_ADDRESS + 0x1D0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG075                     (PHY_RX_TD_BASE_ADDRESS + 0x1D4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG076                     (PHY_RX_TD_BASE_ADDRESS + 0x1D8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG078                     (PHY_RX_TD_BASE_ADDRESS + 0x1E0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG079                     (PHY_RX_TD_BASE_ADDRESS + 0x1E4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG07A                     (PHY_RX_TD_BASE_ADDRESS + 0x1E8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG07B                     (PHY_RX_TD_BASE_ADDRESS + 0x1EC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG07C                     (PHY_RX_TD_BASE_ADDRESS + 0x1F0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG7F                      (PHY_RX_TD_BASE_ADDRESS + 0x1FC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG80                      (PHY_RX_TD_BASE_ADDRESS + 0x200)
#define	REG_PHY_RX_TD_PHY_RXTD_REG81                      (PHY_RX_TD_BASE_ADDRESS + 0x204)
#define	REG_PHY_RX_TD_PHY_RXTD_REG84                      (PHY_RX_TD_BASE_ADDRESS + 0x210)
#define	REG_PHY_RX_TD_PHY_RXTD_REG085                     (PHY_RX_TD_BASE_ADDRESS + 0x214)
#define	REG_PHY_RX_TD_PHY_RXTD_REG086                     (PHY_RX_TD_BASE_ADDRESS + 0x218)
#define	REG_PHY_RX_TD_PHY_RXTD_REG087                     (PHY_RX_TD_BASE_ADDRESS + 0x21C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG08C                     (PHY_RX_TD_BASE_ADDRESS + 0x230)
#define	REG_PHY_RX_TD_PHY_RXTD_REG08D                     (PHY_RX_TD_BASE_ADDRESS + 0x234)
#define	REG_PHY_RX_TD_PHY_RXTD_REG08F                     (PHY_RX_TD_BASE_ADDRESS + 0x23C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG091                     (PHY_RX_TD_BASE_ADDRESS + 0x244)
#define	REG_PHY_RX_TD_PHY_RXTD_REG093                     (PHY_RX_TD_BASE_ADDRESS + 0x24C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0E4                     (PHY_RX_TD_BASE_ADDRESS + 0x390)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0E5                     (PHY_RX_TD_BASE_ADDRESS + 0x394)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0E6                     (PHY_RX_TD_BASE_ADDRESS + 0x398)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0E7                     (PHY_RX_TD_BASE_ADDRESS + 0x39C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0E8                     (PHY_RX_TD_BASE_ADDRESS + 0x3A0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0E9                     (PHY_RX_TD_BASE_ADDRESS + 0x3A4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0EA                     (PHY_RX_TD_BASE_ADDRESS + 0x3A8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0EB                     (PHY_RX_TD_BASE_ADDRESS + 0x3AC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0EC                     (PHY_RX_TD_BASE_ADDRESS + 0x3B0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0ED                     (PHY_RX_TD_BASE_ADDRESS + 0x3B4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0EE                     (PHY_RX_TD_BASE_ADDRESS + 0x3B8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG0EF                     (PHY_RX_TD_BASE_ADDRESS + 0x3BC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG105                     (PHY_RX_TD_BASE_ADDRESS + 0x414)
#define	REG_PHY_RX_TD_PHY_RXTD_REG106                     (PHY_RX_TD_BASE_ADDRESS + 0x418)
#define	REG_PHY_RX_TD_PHY_RXTD_REG107                     (PHY_RX_TD_BASE_ADDRESS + 0x41C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG10C                     (PHY_RX_TD_BASE_ADDRESS + 0x430)
#define	REG_PHY_RX_TD_PHY_RXTD_REG10D                     (PHY_RX_TD_BASE_ADDRESS + 0x434)
#define	REG_PHY_RX_TD_PHY_RXTD_REG10F                     (PHY_RX_TD_BASE_ADDRESS + 0x43C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG111                     (PHY_RX_TD_BASE_ADDRESS + 0x444)
#define	REG_PHY_RX_TD_PHY_RXTD_REG113                     (PHY_RX_TD_BASE_ADDRESS + 0x44C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG164                     (PHY_RX_TD_BASE_ADDRESS + 0x590)
#define	REG_PHY_RX_TD_PHY_RXTD_REG165                     (PHY_RX_TD_BASE_ADDRESS + 0x594)
#define	REG_PHY_RX_TD_PHY_RXTD_REG166                     (PHY_RX_TD_BASE_ADDRESS + 0x598)
#define	REG_PHY_RX_TD_PHY_RXTD_REG167                     (PHY_RX_TD_BASE_ADDRESS + 0x59C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG168                     (PHY_RX_TD_BASE_ADDRESS + 0x5A0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG169                     (PHY_RX_TD_BASE_ADDRESS + 0x5A4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG16A                     (PHY_RX_TD_BASE_ADDRESS + 0x5A8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG16B                     (PHY_RX_TD_BASE_ADDRESS + 0x5AC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG16C                     (PHY_RX_TD_BASE_ADDRESS + 0x5B0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG16D                     (PHY_RX_TD_BASE_ADDRESS + 0x5B4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG16E                     (PHY_RX_TD_BASE_ADDRESS + 0x5B8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG16F                     (PHY_RX_TD_BASE_ADDRESS + 0x5BC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG175                     (PHY_RX_TD_BASE_ADDRESS + 0x5D4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG179                     (PHY_RX_TD_BASE_ADDRESS + 0x5E4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG17B                     (PHY_RX_TD_BASE_ADDRESS + 0x5EC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG17C                     (PHY_RX_TD_BASE_ADDRESS + 0x5F0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG17D                     (PHY_RX_TD_BASE_ADDRESS + 0x5F4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG17E                     (PHY_RX_TD_BASE_ADDRESS + 0x5F8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG17F                     (PHY_RX_TD_BASE_ADDRESS + 0x5FC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG185                     (PHY_RX_TD_BASE_ADDRESS + 0x614)
#define	REG_PHY_RX_TD_PHY_RXTD_REG186                     (PHY_RX_TD_BASE_ADDRESS + 0x618)
#define	REG_PHY_RX_TD_PHY_RXTD_REG187                     (PHY_RX_TD_BASE_ADDRESS + 0x61C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG18C                     (PHY_RX_TD_BASE_ADDRESS + 0x630)
#define	REG_PHY_RX_TD_PHY_RXTD_REG18D                     (PHY_RX_TD_BASE_ADDRESS + 0x634)
#define	REG_PHY_RX_TD_PHY_RXTD_REG18F                     (PHY_RX_TD_BASE_ADDRESS + 0x63C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG191                     (PHY_RX_TD_BASE_ADDRESS + 0x644)
#define	REG_PHY_RX_TD_PHY_RXTD_REG193                     (PHY_RX_TD_BASE_ADDRESS + 0x64C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1E4                     (PHY_RX_TD_BASE_ADDRESS + 0x790)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1E5                     (PHY_RX_TD_BASE_ADDRESS + 0x794)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1E6                     (PHY_RX_TD_BASE_ADDRESS + 0x798)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1E7                     (PHY_RX_TD_BASE_ADDRESS + 0x79C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1E8                     (PHY_RX_TD_BASE_ADDRESS + 0x7A0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1E9                     (PHY_RX_TD_BASE_ADDRESS + 0x7A4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1EA                     (PHY_RX_TD_BASE_ADDRESS + 0x7A8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1EB                     (PHY_RX_TD_BASE_ADDRESS + 0x7AC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1EC                     (PHY_RX_TD_BASE_ADDRESS + 0x7B0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1ED                     (PHY_RX_TD_BASE_ADDRESS + 0x7B4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1EE                     (PHY_RX_TD_BASE_ADDRESS + 0x7B8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1EF                     (PHY_RX_TD_BASE_ADDRESS + 0x7BC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1F8                     (PHY_RX_TD_BASE_ADDRESS + 0x7E0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1F9                     (PHY_RX_TD_BASE_ADDRESS + 0x7E4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1FA                     (PHY_RX_TD_BASE_ADDRESS + 0x7E8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1FB                     (PHY_RX_TD_BASE_ADDRESS + 0x7EC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1FC                     (PHY_RX_TD_BASE_ADDRESS + 0x7F0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1FD                     (PHY_RX_TD_BASE_ADDRESS + 0x7F4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG1FE                     (PHY_RX_TD_BASE_ADDRESS + 0x7F8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG200                     (PHY_RX_TD_BASE_ADDRESS + 0x800)
#define	REG_PHY_RX_TD_PHY_RXTD_REG201                     (PHY_RX_TD_BASE_ADDRESS + 0x804)
#define	REG_PHY_RX_TD_PHY_RXTD_REG202                     (PHY_RX_TD_BASE_ADDRESS + 0x808)
#define	REG_PHY_RX_TD_PHY_RXTD_REG203                     (PHY_RX_TD_BASE_ADDRESS + 0x80C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG204                     (PHY_RX_TD_BASE_ADDRESS + 0x810)
#define	REG_PHY_RX_TD_PHY_RXTD_REG205                     (PHY_RX_TD_BASE_ADDRESS + 0x814)
#define	REG_PHY_RX_TD_PHY_RXTD_REG206                     (PHY_RX_TD_BASE_ADDRESS + 0x818)
#define	REG_PHY_RX_TD_PHY_RXTD_REG207                     (PHY_RX_TD_BASE_ADDRESS + 0x81C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG208                     (PHY_RX_TD_BASE_ADDRESS + 0x820)
#define	REG_PHY_RX_TD_PHY_RXTD_REG209                     (PHY_RX_TD_BASE_ADDRESS + 0x824)
#define	REG_PHY_RX_TD_PHY_RXTD_REG20A                     (PHY_RX_TD_BASE_ADDRESS + 0x828)
#define	REG_PHY_RX_TD_PHY_RXTD_REG20B                     (PHY_RX_TD_BASE_ADDRESS + 0x82C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG20C                     (PHY_RX_TD_BASE_ADDRESS + 0x830)
#define	REG_PHY_RX_TD_PHY_RXTD_REG20D                     (PHY_RX_TD_BASE_ADDRESS + 0x834)
#define	REG_PHY_RX_TD_PHY_RXTD_REG20E                     (PHY_RX_TD_BASE_ADDRESS + 0x838)
#define	REG_PHY_RX_TD_PHY_RXTD_REG20F                     (PHY_RX_TD_BASE_ADDRESS + 0x83C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG210                     (PHY_RX_TD_BASE_ADDRESS + 0x840)
#define	REG_PHY_RX_TD_PHY_RXTD_REG211                     (PHY_RX_TD_BASE_ADDRESS + 0x844)
#define	REG_PHY_RX_TD_PHY_RXTD_REG212                     (PHY_RX_TD_BASE_ADDRESS + 0x848)
#define	REG_PHY_RX_TD_PHY_RXTD_REG213                     (PHY_RX_TD_BASE_ADDRESS + 0x84C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG214                     (PHY_RX_TD_BASE_ADDRESS + 0x850)
#define	REG_PHY_RX_TD_PHY_RXTD_REG215                     (PHY_RX_TD_BASE_ADDRESS + 0x854)
#define	REG_PHY_RX_TD_PHY_RXTD_REG216                     (PHY_RX_TD_BASE_ADDRESS + 0x858)
#define	REG_PHY_RX_TD_PHY_RXTD_REG217                     (PHY_RX_TD_BASE_ADDRESS + 0x85C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG218                     (PHY_RX_TD_BASE_ADDRESS + 0x860)
#define	REG_PHY_RX_TD_PHY_RXTD_REG219                     (PHY_RX_TD_BASE_ADDRESS + 0x864)
#define	REG_PHY_RX_TD_PHY_RXTD_REG21A                     (PHY_RX_TD_BASE_ADDRESS + 0x868)
#define	REG_PHY_RX_TD_PHY_RXTD_REG21B                     (PHY_RX_TD_BASE_ADDRESS + 0x86C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG21C                     (PHY_RX_TD_BASE_ADDRESS + 0x870)
#define	REG_PHY_RX_TD_PHY_RXTD_REG21D                     (PHY_RX_TD_BASE_ADDRESS + 0x874)
#define	REG_PHY_RX_TD_PHY_RXTD_REG21E                     (PHY_RX_TD_BASE_ADDRESS + 0x878)
#define	REG_PHY_RX_TD_PHY_RXTD_REG21F                     (PHY_RX_TD_BASE_ADDRESS + 0x87C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG220                     (PHY_RX_TD_BASE_ADDRESS + 0x880)
#define	REG_PHY_RX_TD_PHY_RXTD_REG221                     (PHY_RX_TD_BASE_ADDRESS + 0x884)
#define	REG_PHY_RX_TD_PHY_RXTD_REG222                     (PHY_RX_TD_BASE_ADDRESS + 0x888)
#define	REG_PHY_RX_TD_PHY_RXTD_REG223                     (PHY_RX_TD_BASE_ADDRESS + 0x88C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG224                     (PHY_RX_TD_BASE_ADDRESS + 0x890)
#define	REG_PHY_RX_TD_PHY_RXTD_REG225                     (PHY_RX_TD_BASE_ADDRESS + 0x894)
#define	REG_PHY_RX_TD_PHY_RXTD_REG226                     (PHY_RX_TD_BASE_ADDRESS + 0x898)
#define	REG_PHY_RX_TD_PHY_RXTD_REG227                     (PHY_RX_TD_BASE_ADDRESS + 0x89C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG228                     (PHY_RX_TD_BASE_ADDRESS + 0x8A0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG229                     (PHY_RX_TD_BASE_ADDRESS + 0x8A4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG22A                     (PHY_RX_TD_BASE_ADDRESS + 0x8A8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG22B                     (PHY_RX_TD_BASE_ADDRESS + 0x8AC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG22C                     (PHY_RX_TD_BASE_ADDRESS + 0x8B0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG22D                     (PHY_RX_TD_BASE_ADDRESS + 0x8B4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG22E                     (PHY_RX_TD_BASE_ADDRESS + 0x8B8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG22F                     (PHY_RX_TD_BASE_ADDRESS + 0x8BC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG230                     (PHY_RX_TD_BASE_ADDRESS + 0x8C0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG231                     (PHY_RX_TD_BASE_ADDRESS + 0x8C4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG232                     (PHY_RX_TD_BASE_ADDRESS + 0x8C8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG233                     (PHY_RX_TD_BASE_ADDRESS + 0x8CC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG234                     (PHY_RX_TD_BASE_ADDRESS + 0x8D0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG235                     (PHY_RX_TD_BASE_ADDRESS + 0x8D4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG236                     (PHY_RX_TD_BASE_ADDRESS + 0x8D8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG237                     (PHY_RX_TD_BASE_ADDRESS + 0x8DC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG238                     (PHY_RX_TD_BASE_ADDRESS + 0x8E0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG239                     (PHY_RX_TD_BASE_ADDRESS + 0x8E4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG23A                     (PHY_RX_TD_BASE_ADDRESS + 0x8E8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG23B                     (PHY_RX_TD_BASE_ADDRESS + 0x8EC)
#define	REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_MSG         (PHY_RX_TD_BASE_ADDRESS + 0x8F0)
#define	REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_MSG_DONE    (PHY_RX_TD_BASE_ADDRESS + 0x8F4)
#define	REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_FAST_MSG    (PHY_RX_TD_BASE_ADDRESS + 0x8F8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG23F                     (PHY_RX_TD_BASE_ADDRESS + 0x8FC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG240                     (PHY_RX_TD_BASE_ADDRESS + 0x900)
#define	REG_PHY_RX_TD_PHY_RXTD_REG241                     (PHY_RX_TD_BASE_ADDRESS + 0x904)
#define	REG_PHY_RX_TD_PHY_RXTD_REG242                     (PHY_RX_TD_BASE_ADDRESS + 0x908)
#define	REG_PHY_RX_TD_PHY_RXTD_REG243                     (PHY_RX_TD_BASE_ADDRESS + 0x90C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG244                     (PHY_RX_TD_BASE_ADDRESS + 0x910)
#define	REG_PHY_RX_TD_PHY_RXTD_REG245                     (PHY_RX_TD_BASE_ADDRESS + 0x914)
#define	REG_PHY_RX_TD_PHY_RXTD_REG246                     (PHY_RX_TD_BASE_ADDRESS + 0x918)
#define	REG_PHY_RX_TD_PHY_RXTD_REG247                     (PHY_RX_TD_BASE_ADDRESS + 0x91C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG248                     (PHY_RX_TD_BASE_ADDRESS + 0x920)
#define	REG_PHY_RX_TD_PHY_RXTD_REG249                     (PHY_RX_TD_BASE_ADDRESS + 0x924)
#define	REG_PHY_RX_TD_PHY_RXTD_REG_LOOPBACK_CTRL          (PHY_RX_TD_BASE_ADDRESS + 0x928)
#define	REG_PHY_RX_TD_PHY_RXTD_REG24B                     (PHY_RX_TD_BASE_ADDRESS + 0x92C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG24C                     (PHY_RX_TD_BASE_ADDRESS + 0x930)
#define	REG_PHY_RX_TD_PHY_RXTD_REG24D                     (PHY_RX_TD_BASE_ADDRESS + 0x934)
#define	REG_PHY_RX_TD_PHY_RXTD_REG24E                     (PHY_RX_TD_BASE_ADDRESS + 0x938)
#define	REG_PHY_RX_TD_PHY_RXTD_REG24F                     (PHY_RX_TD_BASE_ADDRESS + 0x93C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG250                     (PHY_RX_TD_BASE_ADDRESS + 0x940)
#define	REG_PHY_RX_TD_PHY_RXTD_REG251                     (PHY_RX_TD_BASE_ADDRESS + 0x944)
#define	REG_PHY_RX_TD_PHY_RXTD_REG252                     (PHY_RX_TD_BASE_ADDRESS + 0x948)
#define	REG_PHY_RX_TD_PHY_RXTD_REG253                     (PHY_RX_TD_BASE_ADDRESS + 0x94C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG254                     (PHY_RX_TD_BASE_ADDRESS + 0x950)
#define	REG_PHY_RX_TD_PHY_RXTD_REG255                     (PHY_RX_TD_BASE_ADDRESS + 0x954)
#define	REG_PHY_RX_TD_PHY_RXTD_REG256                     (PHY_RX_TD_BASE_ADDRESS + 0x958)
#define	REG_PHY_RX_TD_PHY_RXTD_REG257                     (PHY_RX_TD_BASE_ADDRESS + 0x95C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG258                     (PHY_RX_TD_BASE_ADDRESS + 0x960)
#define	REG_PHY_RX_TD_PHY_RXTD_REG259                     (PHY_RX_TD_BASE_ADDRESS + 0x964)
#define	REG_PHY_RX_TD_PHY_RXTD_REG25A                     (PHY_RX_TD_BASE_ADDRESS + 0x968)
#define	REG_PHY_RX_TD_PHY_RXTD_REG25B                     (PHY_RX_TD_BASE_ADDRESS + 0x96C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG25C                     (PHY_RX_TD_BASE_ADDRESS + 0x970)
#define	REG_PHY_RX_TD_PHY_RXTD_REG25D                     (PHY_RX_TD_BASE_ADDRESS + 0x974)
#define	REG_PHY_RX_TD_PHY_RXTD_REG25E                     (PHY_RX_TD_BASE_ADDRESS + 0x978)
#define	REG_PHY_RX_TD_PHY_RXTD_REG25F                     (PHY_RX_TD_BASE_ADDRESS + 0x97C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG260                     (PHY_RX_TD_BASE_ADDRESS + 0x980)
#define	REG_PHY_RX_TD_PHY_RXTD_REG261                     (PHY_RX_TD_BASE_ADDRESS + 0x984)
#define	REG_PHY_RX_TD_PHY_RXTD_REG262                     (PHY_RX_TD_BASE_ADDRESS + 0x988)
#define	REG_PHY_RX_TD_PHY_RXTD_REG263                     (PHY_RX_TD_BASE_ADDRESS + 0x98C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG264                     (PHY_RX_TD_BASE_ADDRESS + 0x990)
#define	REG_PHY_RX_TD_PHY_RXTD_REG265                     (PHY_RX_TD_BASE_ADDRESS + 0x994)
#define	REG_PHY_RX_TD_PHY_RXTD_REG266                     (PHY_RX_TD_BASE_ADDRESS + 0x998)
#define	REG_PHY_RX_TD_PHY_RXTD_REG267                     (PHY_RX_TD_BASE_ADDRESS + 0x99C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG268                     (PHY_RX_TD_BASE_ADDRESS + 0x9A0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG269                     (PHY_RX_TD_BASE_ADDRESS + 0x9A4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG26A                     (PHY_RX_TD_BASE_ADDRESS + 0x9A8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG26B                     (PHY_RX_TD_BASE_ADDRESS + 0x9AC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG26C                     (PHY_RX_TD_BASE_ADDRESS + 0x9B0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG26D                     (PHY_RX_TD_BASE_ADDRESS + 0x9B4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG26E                     (PHY_RX_TD_BASE_ADDRESS + 0x9B8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG26F                     (PHY_RX_TD_BASE_ADDRESS + 0x9BC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG270                     (PHY_RX_TD_BASE_ADDRESS + 0x9C0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG271                     (PHY_RX_TD_BASE_ADDRESS + 0x9C4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG272                     (PHY_RX_TD_BASE_ADDRESS + 0x9C8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG273                     (PHY_RX_TD_BASE_ADDRESS + 0x9CC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG274                     (PHY_RX_TD_BASE_ADDRESS + 0x9D0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG275                     (PHY_RX_TD_BASE_ADDRESS + 0x9D4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG276                     (PHY_RX_TD_BASE_ADDRESS + 0x9D8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG277                     (PHY_RX_TD_BASE_ADDRESS + 0x9DC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG278                     (PHY_RX_TD_BASE_ADDRESS + 0x9E0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG279                     (PHY_RX_TD_BASE_ADDRESS + 0x9E4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG27A                     (PHY_RX_TD_BASE_ADDRESS + 0x9E8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG27B                     (PHY_RX_TD_BASE_ADDRESS + 0x9EC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG27C                     (PHY_RX_TD_BASE_ADDRESS + 0x9F0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG27D                     (PHY_RX_TD_BASE_ADDRESS + 0x9F4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG27E                     (PHY_RX_TD_BASE_ADDRESS + 0x9F8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG27F                     (PHY_RX_TD_BASE_ADDRESS + 0x9FC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG280                     (PHY_RX_TD_BASE_ADDRESS + 0xA00)
#define	REG_PHY_RX_TD_PHY_RXTD_REG281                     (PHY_RX_TD_BASE_ADDRESS + 0xA04)
#define	REG_PHY_RX_TD_PHY_RXTD_REG282                     (PHY_RX_TD_BASE_ADDRESS + 0xA08)
#define	REG_PHY_RX_TD_PHY_RXTD_REG283                     (PHY_RX_TD_BASE_ADDRESS + 0xA0C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG284                     (PHY_RX_TD_BASE_ADDRESS + 0xA10)
#define	REG_PHY_RX_TD_PHY_RXTD_REG285                     (PHY_RX_TD_BASE_ADDRESS + 0xA14)
#define	REG_PHY_RX_TD_PHY_RXTD_REG286                     (PHY_RX_TD_BASE_ADDRESS + 0xA18)
#define	REG_PHY_RX_TD_PHY_RXTD_REG287                     (PHY_RX_TD_BASE_ADDRESS + 0xA1C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG288                     (PHY_RX_TD_BASE_ADDRESS + 0xA20)
#define	REG_PHY_RX_TD_PHY_RXTD_REG289                     (PHY_RX_TD_BASE_ADDRESS + 0xA24)
#define	REG_PHY_RX_TD_PHY_RXTD_REG28A                     (PHY_RX_TD_BASE_ADDRESS + 0xA28)
#define	REG_PHY_RX_TD_PHY_RXTD_REG28B                     (PHY_RX_TD_BASE_ADDRESS + 0xA2C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG28C                     (PHY_RX_TD_BASE_ADDRESS + 0xA30)
#define	REG_PHY_RX_TD_PHY_RXTD_REG28D                     (PHY_RX_TD_BASE_ADDRESS + 0xA34)
#define	REG_PHY_RX_TD_PHY_RXTD_REG28E                     (PHY_RX_TD_BASE_ADDRESS + 0xA38)
#define	REG_PHY_RX_TD_PHY_RXTD_REG28F                     (PHY_RX_TD_BASE_ADDRESS + 0xA3C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG290                     (PHY_RX_TD_BASE_ADDRESS + 0xA40)
#define	REG_PHY_RX_TD_PHY_RXTD_REG291                     (PHY_RX_TD_BASE_ADDRESS + 0xA44)
#define	REG_PHY_RX_TD_PHY_RXTD_REG292                     (PHY_RX_TD_BASE_ADDRESS + 0xA48)
#define	REG_PHY_RX_TD_PHY_RXTD_REG293                     (PHY_RX_TD_BASE_ADDRESS + 0xA4C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG294                     (PHY_RX_TD_BASE_ADDRESS + 0xA50)
#define	REG_PHY_RX_TD_PHY_RXTD_REG295                     (PHY_RX_TD_BASE_ADDRESS + 0xA54)
#define	REG_PHY_RX_TD_PHY_RXTD_REG296                     (PHY_RX_TD_BASE_ADDRESS + 0xA58)
#define	REG_PHY_RX_TD_PHY_RXTD_REG297                     (PHY_RX_TD_BASE_ADDRESS + 0xA5C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG298                     (PHY_RX_TD_BASE_ADDRESS + 0xA60)
#define	REG_PHY_RX_TD_PHY_RXTD_REG299                     (PHY_RX_TD_BASE_ADDRESS + 0xA64)
#define	REG_PHY_RX_TD_PHY_RXTD_REG29A                     (PHY_RX_TD_BASE_ADDRESS + 0xA68)
#define	REG_PHY_RX_TD_PHY_RXTD_REG29B                     (PHY_RX_TD_BASE_ADDRESS + 0xA6C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG29C                     (PHY_RX_TD_BASE_ADDRESS + 0xA70)
#define	REG_PHY_RX_TD_PHY_RXTD_REG29E                     (PHY_RX_TD_BASE_ADDRESS + 0xA78)
#define	REG_PHY_RX_TD_PHY_RXTD_REG29F                     (PHY_RX_TD_BASE_ADDRESS + 0xA7C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A0                     (PHY_RX_TD_BASE_ADDRESS + 0xA80)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A1                     (PHY_RX_TD_BASE_ADDRESS + 0xA84)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A2                     (PHY_RX_TD_BASE_ADDRESS + 0xA88)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A3                     (PHY_RX_TD_BASE_ADDRESS + 0xA8C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A4                     (PHY_RX_TD_BASE_ADDRESS + 0xA90)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A5                     (PHY_RX_TD_BASE_ADDRESS + 0xA94)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A6                     (PHY_RX_TD_BASE_ADDRESS + 0xA98)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A7                     (PHY_RX_TD_BASE_ADDRESS + 0xA9C)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A8                     (PHY_RX_TD_BASE_ADDRESS + 0xAA0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2A9                     (PHY_RX_TD_BASE_ADDRESS + 0xAA4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2AA                     (PHY_RX_TD_BASE_ADDRESS + 0xAA8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2AB                     (PHY_RX_TD_BASE_ADDRESS + 0xAAC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2AC                     (PHY_RX_TD_BASE_ADDRESS + 0xAB0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2AD                     (PHY_RX_TD_BASE_ADDRESS + 0xAB4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2AE                     (PHY_RX_TD_BASE_ADDRESS + 0xAB8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2AF                     (PHY_RX_TD_BASE_ADDRESS + 0xABC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2B0                     (PHY_RX_TD_BASE_ADDRESS + 0xAC0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2B1                     (PHY_RX_TD_BASE_ADDRESS + 0xAC4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2B2                     (PHY_RX_TD_BASE_ADDRESS + 0xAC8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2B3                     (PHY_RX_TD_BASE_ADDRESS + 0xACC)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2B4                     (PHY_RX_TD_BASE_ADDRESS + 0xAD0)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2B5                     (PHY_RX_TD_BASE_ADDRESS + 0xAD4)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2B6                     (PHY_RX_TD_BASE_ADDRESS + 0xAD8)
#define	REG_PHY_RX_TD_PHY_RXTD_REG2B7                     (PHY_RX_TD_BASE_ADDRESS + 0xADC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RX_TD_PHY_RXTD_REG00 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg:16;	// Sub blocks SW Reset
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg00_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG01 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockEn:16;	// Sub blocks Enable
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg01_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG02 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reg02:32;	// Sub blocks SW Reset
	} bitFields;
} RegPhyRxTdPhyRxtdReg02_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG03 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reg03:32;	// RxTD Hyperion Control Reg
	} bitFields;
} RegPhyRxTdPhyRxtdReg03_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 adcSamplingPolarity:1;	// ADC Sampling Polarity
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg04_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG05 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchBypassIirAn0:1;	// IQ Mismatch IIR Bypass
		uint32 iqMismatchBypassEqAn0:1;	// IQ Mismatch Equalizer Bypass
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg05_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchIirMuIAn0:4;	// IQ Mismatch IIR Mu I
		uint32 iqMismatchIirMuQAn0:4;	// IQ Mismatch IIR Mu Q
		uint32 reserved0:16;
		uint32 iqMismatchEstRegularISumAn0:1;	// Rx TD IQ Mismatch I regular sum (not sqr)
		uint32 reserved1:3;
		uint32 iqMismatchEstRegularQSumAn0:1;	// Rx TD IQ Mismatch Q regular sum (not sqr)
		uint32 reserved2:3;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG07 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 iqMismatchEqLengthQAn0:3;	// IQ Mismatch EQ Length Q
		uint32 reserved1:25;
	} bitFields;
} RegPhyRxTdPhyRxtdReg07_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG09 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:24;
		uint32 scCyclicPeriod:3;	// sc cyclic period
		uint32 reserved1:5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg09_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0A 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fixArbiterEnable:1;	// sc cyclic period
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0B 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bwMode:2;	// bw_mode
		uint32 hbMode:2;	// hb_mode
		uint32 fftInShift:2;	// fft in shift
		uint32 fftInShiftHb2080:2;	// Rx TD fft in shift for hb2080
		uint32 fftInShiftHb4080:2;	// Rx TD fft in shift for hb4080
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0C 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstAccCounterAn0:24;	// IQ Mismatch Est Acc Counter
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0D 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 iqMismatchEstStartWorkAn0:1;	// IQ Mismatch Est Start Work
		uint32 reserved1:3;
		uint32 iqMismatchEstResetRegsAn0:1;	// IQ Mismatch Est Reset Regs
		uint32 reserved2:3;
		uint32 iqMismatchEstValidIqAn0:1;	// Rx TD IQ Mismatch Est valid
		uint32 reserved3:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0F 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegIiAn0:32;	// IQ Mismatch Est Reg II low
	} bitFields;
} RegPhyRxTdPhyRxtdReg0F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG11 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegQqAn0:32;	// IQ Mismatch Est Reg QQ low
	} bitFields;
} RegPhyRxTdPhyRxtdReg11_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG13 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegIqAn0:32;	// IQ Mismatch Est Reg IQ low
	} bitFields;
} RegPhyRxTdPhyRxtdReg13_u;

/*REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_LOW 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mipsTestPlugLow:24;	// mips test plug low data
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdMipsTestPlugLow_u;

/*REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_MID 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mipsTestPlugMid:24;	// mips test plug mid data
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdMipsTestPlugMid_u;

/*REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_HIGH 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mipsTestPlugHigh:24;	// mips test plug high data
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdMipsTestPlugHigh_u;

/*REG_PHY_RX_TD_PHY_RXTD_MIPS_TEST_PLUG_FOUR 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mipsTestPlugFour:24;	// mips test plug four data
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdMipsTestPlugFour_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG22 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstEnResetAcc:1;	// Rx TD Noise Est Enable Reset Acc
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg22_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG24 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxIqSwap:4;	// IQ Swap
		uint32 rxSpectromFlip:4;	// Spectrum flip
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdPhyRxtdReg24_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 bsGclkDis:1;	// Rx TD Band Select Gated Clock Disable
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2C 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaTimingModeDelayOfdm:16;	// delay in clocks for an OFDM packet
		uint32 ccaTimingModeDelay11B:16;	// delay in clocks for a 11b packet
	} bitFields;
} RegPhyRxTdPhyRxtdReg2C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2E 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detBypassMeanRemoval:1;	// Detection Mean Removal Bypass.
		uint32 detMrbDelayLineLength:1;	// Detection Mean Removal delay line length.
		uint32 detMrbMiddleTap:3;	// Detection Mean Removal middle tap.
		uint32 detMrbBpfMode:1;	// Detection Mean Removal BPF mode
		uint32 reserved0:16;
		uint32 detCorrDL:4;	// Detection correlation-D low tap.
		uint32 detCorrDH:4;	// Detection correlation-D high tap.
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2F 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 detDiffMode:1;	// Detection correlation-64 differentiators Mode.
		uint32 detCntThForCorr16Size:8;	// Detection threshold for correlation-16 size.
		uint32 reserved1:9;
		uint32 detFineErrCntInit:11;	// Detection fine Error Cnt Init.
		uint32 reserved2:1;
		uint32 detControlImd:1;	// Use common coarse rdy
	} bitFields;
} RegPhyRxTdPhyRxtdReg2F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG30 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detFinePeakCntInit:8;	// Detection Fine timing Peak Cnt Init.
		uint32 detFineEnableDelay:8;	// Detection Fine timing Enable Delay.
		uint32 detFineDelaySize:4;	// Detection Fine frequency Delay Size.
		uint32 reserved0:12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg30_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG31 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detCoarsePeakCntInit:8;	// Detection Coarse timing Peak Cnt Init.
		uint32 detCoarseEnableDelay:8;	// Detection Coarse timing Enable Delay.
		uint32 detCoarseDelaySize:5;	// Detection Coarse frequency Delay Size.
		uint32 detCoarseErrCntInit:11;	// Detection coarse Error Cnt Init.
	} bitFields;
} RegPhyRxTdPhyRxtdReg31_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG32 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTopCnt:6;	// Top Detection thershold register 2.
		uint32 reserved0:2;
		uint32 detTopTh3:15;	// Top Detection thershold register 3.
		uint32 reserved1:1;
		uint32 detTopCntPri:6;	// Top Detection counter initialization.
		uint32 detTopMode:2;	// Top Detection mode.
	} bitFields;
} RegPhyRxTdPhyRxtdReg32_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG33 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTopBlockCnt:12;	// Top Detection block counter.
		uint32 reserved0:12;
		uint32 detMaDec:4;	// Rx TD ma_dec.
		uint32 reserved1:4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg33_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG34 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htShortOffset:16;	// CCA ht short offset
		uint32 lastSampleOffset:16;	// CCA last sample offset
	} bitFields;
} RegPhyRxTdPhyRxtdReg34_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG35 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaEnergy01Delay:11;	// delay in clocks of CCA rise event
		uint32 reserved0:1;
		uint32 ccaEnergy10Delay:11;	// delay in clocks of CCA fall event
		uint32 lsbIsCtrlChannel:1;	// indicates wether LSB is the control channel (1 - LSB,0 - USB)
		uint32 lsb40IsCtrlChannel:1;	// determines which 40m band is the primary (1 - LSB,0 - USB)
		uint32 blockAuxChannel:1;	// block_aux_channel
		uint32 blockAuxChannel40:1;	// block_aux_channel40
		uint32 resetCcaCounters:1;	// reset cca counters
		uint32 ccaAccSamples:4;	// number of samples to use for energy estimation (number + 1)
	} bitFields;
} RegPhyRxTdPhyRxtdReg35_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG36 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaEdThreshold:8;	// threshold value
		uint32 ccaEdThreshold40:8;	// threshold value
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg36_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG37 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctrlChannelCnt:32;	// counter for the control channel
	} bitFields;
} RegPhyRxTdPhyRxtdReg37_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG38 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extChannelCnt:32;	// counter for the extension channel
	} bitFields;
} RegPhyRxTdPhyRxtdReg38_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG39 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extChannelCnt40:32;	// cca statistics counter: saturated count of secondary 40MHz cca activation cycles since last stats_reset event
	} bitFields;
} RegPhyRxTdPhyRxtdReg39_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG3A 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 adcActiveReg:1;	// Rx TD Adc Active.
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg3A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG3B 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockAuxChannelSec:1;	// block_aux_channel_sec
		uint32 lsbIsCtrlChannelSec:1;	// lsb_is_ctrl_channel_sec
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg3B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG3C 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muHostAntennaEn:4;	// MU Antenna Enable. , Host antenna enable bits , can be changed after risc processing.
		uint32 hostAntennaEn:4;	// Antenna Enable. , Host antenna enable bits , can be changed after risc processing.
		uint32 bandSelectDelay:8;	// Band Select Delay
		uint32 bandSelectDelay2B:8;	// Band Select Delay 2 Band
		uint32 bandSelectDelay4B:8;	// Band Select Delay 4 Band
	} bitFields;
} RegPhyRxTdPhyRxtdReg3C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG3D 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stmAbortThr:16;	// Stm Abort Threshold
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg3D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG3E 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 bypassDecAll:1;	// Bypass Decimator Filter
		uint32 reserved1:3;
		uint32 bypassDcAll:1;	// Bypass Dc Removal
		uint32 reserved2:26;
	} bitFields;
} RegPhyRxTdPhyRxtdReg3E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG041 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rssiSamplePolarity:1;	// Rssi Sample Polarity.
		uint32 reserved0:3;
		uint32 rssiStrbRate:1;	// ESSI strobe threshold: , 0 : for 40Mhz , 1: for 20Mhz
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg041_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04B 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 parallelMode:1;	// Rx TD Hyp Pif Parallel Mode.
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg04B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04C 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deltaFCmpEn:1;	// Delta F Cmp En.
		uint32 reserved0:3;
		uint32 deltaFFineErrEnable:1;	// Delta F fine error enable.
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg04C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG04F 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscArbiterGclkBypass:1;	// RIsc Arbiter Gclk Bypass.
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg04F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG050 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 riscPage:1;	// RIsc Page Address.
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg050_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG051 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscStartOp:1;	// Rx TD RIsc Start Opcode.
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg051_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG052 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzelResetAcum:4;	// Geortzel Reset Acumulator.
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdPhyRxtdReg052_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG053 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 geortzel0InputSelect:4;	// Rx TD Geortzel 0 input Select.
		uint32 geortzel1InputSelect:4;	// Rx TD Geortzel 1 input Select.
		uint32 geortzel2InputSelect:4;	// Rx TD Geortzel 2 input Select.
		uint32 geortzel3InputSelect:4;	// Rx TD Geortzel 3 input Select.
		uint32 reserved1:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg053_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG054 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel0MulI:14;	// Geortzel 0 Mult I Data.
		uint32 reserved0:2;
		uint32 geortzel0MulQ:14;	// Geortzel 0 Mult Q Data.
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxTdPhyRxtdReg054_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG055 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel1MulI:14;	// Geortzel 1 Mult I Data.
		uint32 reserved0:2;
		uint32 geortzel1MulQ:14;	// Geortzel 1 Mult Q Data.
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxTdPhyRxtdReg055_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG056 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel2MulI:14;	// Geortzel 2 Mult I Data.
		uint32 reserved0:2;
		uint32 geortzel2MulQ:14;	// Geortzel 2 Mult Q Data.
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxTdPhyRxtdReg056_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG057 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel3MulI:14;	// Geortzel 1 Mult I Data.
		uint32 reserved0:2;
		uint32 geortzel3MulQ:14;	// Geortzel 1 Mult Q Data.
		uint32 reserved1:2;
	} bitFields;
} RegPhyRxTdPhyRxtdReg057_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG058 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel0ValidThr:7;	// Geortzel 0 Valid Threshold.
		uint32 reserved0:1;
		uint32 geortzel1ValidThr:7;	// Geortzel 1 Valid Threshold.
		uint32 reserved1:1;
		uint32 geortzel2ValidThr:7;	// Geortzel 2 Valid Threshold.
		uint32 reserved2:1;
		uint32 geortzel3ValidThr:7;	// Geortzel 3 Valid Threshold.
		uint32 reserved3:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg058_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG05E 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdTestPlugLenAn0:25;	// Test Plug Local Enable.
		uint32 reserved0:7;
	} bitFields;
} RegPhyRxTdPhyRxtdReg05E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG05F 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdTestPlugLenAn1:25;	// Test Plug Local Enable.
		uint32 reserved0:7;
	} bitFields;
} RegPhyRxTdPhyRxtdReg05F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG060 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdTestPlugLenAn2:25;	// Test Plug Local Enable.
		uint32 reserved0:7;
	} bitFields;
} RegPhyRxTdPhyRxtdReg060_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG061 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdTestPlugLenAn3:25;	// Test Plug Local Enable.
		uint32 reserved0:7;
	} bitFields;
} RegPhyRxTdPhyRxtdReg061_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG062 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate:16;	// Rx TD Sw Reset Generate
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg062_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG063 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmSwResetMask:16;	// Rx TD Gsm Sw Reset.
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg063_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG064 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap0Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 0.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg064_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG065 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap1Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 1.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg065_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG066 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap2Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 2.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg066_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG067 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap3Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 3.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg067_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG068 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap4Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 4.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg068_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG069 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap5Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 5.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg069_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06A 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap6Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 6.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06B 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap7Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 7.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06C 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap8Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 8.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06D 0x1B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap9Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 9.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06E 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap10Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 10.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG06F 0x1BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap11Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 11.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg06F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG073 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 olaSamplePerWindow:8;	// ola number of samples per window
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdPhyRxtdReg073_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG074 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 olaNumOfWindows:5;	// 2^(ola number of windows )
		uint32 reserved0:3;
		uint32 olaZeroInput:2;	// ola zero input: , 00- regular mode , 01- Q-part zero , 10- I part zero
		uint32 reserved1:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg074_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG075 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 olaRound:5;	// ola round parameter
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg075_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG076 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rfCalMode:1;	// rf calibration mode
		uint32 reserved0:3;
		uint32 olaStartAn0:1;	// ola_start_an0 pulse
		uint32 reserved1:3;
		uint32 olaStartAn1:1;	// ola_start_an1 pulse 
		uint32 reserved2:3;
		uint32 olaStartAn2:1;	// ola_start_an2 pulse 
		uint32 reserved3:3;
		uint32 olaStartAn3:1;	// ola_start_an3 pulse 
		uint32 reserved4:3;
		uint32 olaDoneAn0:1;	// ola_done_an0 pulse 
		uint32 olaDoneAn1:1;	// ola_done_an1 pulse 
		uint32 olaDoneAn2:1;	// ola_done_an2 pulse 
		uint32 olaDoneAn3:1;	// ola_done_an3 pulse 
		uint32 olaRssiMask:4;	// ola rssi mask : 1bit for each antenna
		uint32 reserved5:4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg076_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG078 0x1E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxRdyRiseDelay11A:13;	// rx_rdy_rise_delay_11a
		uint32 reserved0:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg078_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG079 0x1E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxRdyRiseDelay11N:13;	// rx_rdy_rise_delay_11n
		uint32 reserved0:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg079_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG07A 0x1E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxRdyFallDelay:13;	// rx_rdy_fall_delay
		uint32 reserved0:3;
		uint32 rxRdyFallDelay11B:13;	// rx_rdy_fall_delay
		uint32 reserved1:3;
	} bitFields;
} RegPhyRxTdPhyRxtdReg07A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG07B 0x1EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopSignalTh:13;	// stop_signal_th
		uint32 reserved0:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg07B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG07C 0x1F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extendAirTimeOffset:8;	// extend_air_time_offset
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdPhyRxtdReg07C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG7F 0x1FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdTestBusGclkEn:1;	// td_test_bus_gclk_en 
		uint32 beTestBusGclkEn:1;	// be_test_bus_gclk_en 
		uint32 slicerTestBusGclkEn:1;	// slicer_test_bus_gclk_en 
		uint32 fdTestBusGclkEn:1;	// fd_test_bus_gclk_en 
		uint32 txTestBusGclkEn:1;	// tx_test_bus_gclk_en 
		uint32 loggerTestBusGclkEn:1;	// logger_test_bus_gclk_en
		uint32 reserved0:2;
		uint32 swBistStart:1;	// sw_bist_start
		uint32 reserved1:3;
		uint32 clearRamMode:1;	// clear_ram_mode
		uint32 reserved2:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg7F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG80 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkBypassEn:32;	// gclk bypass - when '1' clocks will be always active
	} bitFields;
} RegPhyRxTdPhyRxtdReg80_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG81 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 tbFifoClear:1;	// Test Bus FIFO Clear
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg81_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG84 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testFifoCaptureMode:1;	// test_fifo_capture_mode
		uint32 testFifoCyclicMode:1;	// test_fifo_cyclic_mode
		uint32 testFifoExternalTrigEn:1;	// test_fifo_external_trig_en
		uint32 reserved0:1;
		uint32 testBusLowMuxCtrl:2;	// test_bus_low_mux_ctrl
		uint32 reserved1:2;
		uint32 testBusMidMuxCtrl:2;	// test_bus_mid_mux_ctrl
		uint32 reserved2:2;
		uint32 testBusHighMuxCtrl:2;	// test_bus_high_mux_ctrl
		uint32 reserved3:2;
		uint32 testBusFourMuxCtrl:2;	// test_bus_four_mux_ctrl
		uint32 reserved4:14;
	} bitFields;
} RegPhyRxTdPhyRxtdReg84_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG085 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchBypassIirAn1:1;	// IQ Mismatch IIR Bypass
		uint32 iqMismatchBypassEqAn1:1;	// IQ Mismatch Equalizer Bypass
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg085_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG086 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchIirMuIAn1:4;	// IQ Mismatch IIR Mu I
		uint32 iqMismatchIirMuQAn1:4;	// IQ Mismatch IIR Mu Q
		uint32 reserved0:16;
		uint32 iqMismatchEstRegularISumAn1:1;	// Rx TD IQ Mismatch I regular sum (not sqr)
		uint32 reserved1:3;
		uint32 iqMismatchEstRegularQSumAn1:1;	// Rx TD IQ Mismatch Q regular sum (not sqr)
		uint32 reserved2:3;
	} bitFields;
} RegPhyRxTdPhyRxtdReg086_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG087 0x21C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 iqMismatchEqLengthQAn1:3;	// IQ Mismatch EQ Length Q
		uint32 reserved1:25;
	} bitFields;
} RegPhyRxTdPhyRxtdReg087_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG08C 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstAccCounterAn1:24;	// IQ Mismatch Est Acc Counter
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg08C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG08D 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 iqMismatchEstStartWorkAn1:1;	// IQ Mismatch Est Start Work
		uint32 reserved1:3;
		uint32 iqMismatchEstResetRegsAn1:1;	// IQ Mismatch Est Reset Regs
		uint32 reserved2:3;
		uint32 iqMismatchEstValidIqAn1:1;	// Rx TD IQ Mismatch Est valid
		uint32 reserved3:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg08D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG08F 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegIiAn1:32;	// IQ Mismatch Est Reg II low
	} bitFields;
} RegPhyRxTdPhyRxtdReg08F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG091 0x244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegQqAn1:32;	// IQ Mismatch Est Reg QQ low
	} bitFields;
} RegPhyRxTdPhyRxtdReg091_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG093 0x24C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegIqAn1:32;	// IQ Mismatch Est Reg IQ low
	} bitFields;
} RegPhyRxTdPhyRxtdReg093_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0E4 0x390 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap0Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 0.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0E4_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0E5 0x394 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap1Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 1.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0E5_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0E6 0x398 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap2Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 2.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0E6_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0E7 0x39C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap3Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 3.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0E7_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0E8 0x3A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap4Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 4.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0E8_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0E9 0x3A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap5Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 5.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0E9_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0EA 0x3A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap6Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 6.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0Ea_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0EB 0x3AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap7Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 7.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0Eb_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0EC 0x3B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap8Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 8.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0Ec_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0ED 0x3B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap9Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 9.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0Ed_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0EE 0x3B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap10Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 10.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0Ee_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG0EF 0x3BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap11Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 11.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg0Ef_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG105 0x414 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchBypassIirAn2:1;	// IQ Mismatch IIR Bypass
		uint32 iqMismatchBypassEqAn2:1;	// IQ Mismatch Equalizer Bypass
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg105_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG106 0x418 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchIirMuIAn2:4;	// IQ Mismatch IIR Mu I
		uint32 iqMismatchIirMuQAn2:4;	// IQ Mismatch IIR Mu Q
		uint32 reserved0:16;
		uint32 iqMismatchEstRegularISumAn2:1;	// Rx TD IQ Mismatch I regular sum (not sqr)
		uint32 reserved1:3;
		uint32 iqMismatchEstRegularQSumAn2:1;	// Rx TD IQ Mismatch Q regular sum (not sqr)
		uint32 reserved2:3;
	} bitFields;
} RegPhyRxTdPhyRxtdReg106_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG107 0x41C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 iqMismatchEqLengthQAn2:3;	// IQ Mismatch EQ Length Q
		uint32 reserved1:25;
	} bitFields;
} RegPhyRxTdPhyRxtdReg107_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG10C 0x430 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstAccCounterAn2:24;	// IQ Mismatch Est Acc Counter
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg10C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG10D 0x434 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 iqMismatchEstStartWorkAn2:1;	// IQ Mismatch Est Start Work
		uint32 reserved1:3;
		uint32 iqMismatchEstResetRegsAn2:1;	// IQ Mismatch Est Reset Regs
		uint32 reserved2:3;
		uint32 iqMismatchEstValidIqAn2:1;	// Rx TD IQ Mismatch Est valid
		uint32 reserved3:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg10D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG10F 0x43C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegIiAn2:32;	// IQ Mismatch Est Reg II low
	} bitFields;
} RegPhyRxTdPhyRxtdReg10F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG111 0x444 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegQqAn2:32;	// IQ Mismatch Est Reg QQ low
	} bitFields;
} RegPhyRxTdPhyRxtdReg111_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG113 0x44C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegIqAn2:32;	// IQ Mismatch Est Reg IQ low
	} bitFields;
} RegPhyRxTdPhyRxtdReg113_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG164 0x590 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap0Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 0.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg164_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG165 0x594 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap1Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 1.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg165_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG166 0x598 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap2Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 2.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg166_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG167 0x59C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap3Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 3.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg167_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG168 0x5A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap4Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 4.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg168_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG169 0x5A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap5Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 5.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg169_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG16A 0x5A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap6Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 6.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg16A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG16B 0x5AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap7Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 7.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg16B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG16C 0x5B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap8Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 8.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg16C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG16D 0x5B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap9Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 9.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg16D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG16E 0x5B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap10Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 10.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg16E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG16F 0x5BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap11Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 11.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg16F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG175 0x5D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscSramRm:3;	// gen_risc_sram_rm
		uint32 reserved0:1;
		uint32 genRiscIramRm:3;	// gen_risc_iram_rm
		uint32 reserved1:13;
		uint32 iqCoefRm:3;	// iq_coef_rm
		uint32 reserved2:1;
		uint32 memConverterRm:3;	// mem_converter_rm
		uint32 reserved3:1;
		uint32 testFifoRamRm:3;	// test_fifo_large_ram_low_rm
		uint32 reserved4:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg175_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG179 0x5E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcTableRm:3;	// dc_table_rm
		uint32 reserved0:1;
		uint32 dcTable2Rm:3;	// dc_table2_rm
		uint32 reserved1:1;
		uint32 agcTableRm:3;	// agc_table_rm
		uint32 reserved2:21;
	} bitFields;
} RegPhyRxTdPhyRxtdReg179_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG17B 0x5EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 switch11BCounterLimit:21;	// switch_11b_counter_limit
		uint32 reserved0:11;
	} bitFields;
} RegPhyRxTdPhyRxtdReg17B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG17C 0x5F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkShutdownEn:32;	// gclk shutdown - when '1' clocks will be always deactive
	} bitFields;
} RegPhyRxTdPhyRxtdReg17C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG17D 0x5F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bistScrBypass:1;	// bist_scr_bypass
		uint32 ramTestMode:1;	// ram_test_mode
		uint32 memGlobalRm:2;	// mem_global_rm
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdPhyRxtdReg17D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG17E 0x5F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxFifoAn0Rm:3;	// rx_fifo_an0_rm
		uint32 reserved0:1;
		uint32 rxFifoAn1Rm:3;	// rx_fifo_an1_rm
		uint32 reserved1:1;
		uint32 rxFifoAn2Rm:3;	// rx_fifo_an2_rm
		uint32 reserved2:1;
		uint32 rxFifoAn3Rm:3;	// rx_fifo_an3_rm
		uint32 reserved3:17;
	} bitFields;
} RegPhyRxTdPhyRxtdReg17E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG17F 0x5FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectorMrbRm:3;	// detector_mrb_rm
		uint32 reserved0:1;
		uint32 detectorC16Rm:3;	// detector_c16_rm
		uint32 reserved1:1;
		uint32 detectorC64Rm:3;	// detector_c64_rm
		uint32 reserved2:1;
		uint32 detectorCfbRm:3;	// detector_cfb_rm
		uint32 reserved3:1;
		uint32 detectorFfbRm:3;	// detector_ffb_rm
		uint32 reserved4:13;
	} bitFields;
} RegPhyRxTdPhyRxtdReg17F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG185 0x614 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchBypassIirAn3:1;	// IQ Mismatch IIR Bypass
		uint32 iqMismatchBypassEqAn3:1;	// IQ Mismatch Equalizer Bypass
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg185_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG186 0x618 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchIirMuIAn3:4;	// IQ Mismatch IIR Mu I
		uint32 iqMismatchIirMuQAn3:4;	// IQ Mismatch IIR Mu Q
		uint32 reserved0:16;
		uint32 iqMismatchEstRegularISumAn3:1;	// Rx TD IQ Mismatch I regular sum (not sqr)
		uint32 reserved1:3;
		uint32 iqMismatchEstRegularQSumAn3:1;	// Rx TD IQ Mismatch Q regular sum (not sqr)
		uint32 reserved2:3;
	} bitFields;
} RegPhyRxTdPhyRxtdReg186_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG187 0x61C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 iqMismatchEqLengthQAn3:3;	// IQ Mismatch EQ Length Q
		uint32 reserved1:25;
	} bitFields;
} RegPhyRxTdPhyRxtdReg187_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG18C 0x630 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstAccCounterAn3:24;	// IQ Mismatch Est Acc Counter
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg18C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG18D 0x634 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 iqMismatchEstStartWorkAn3:1;	// IQ Mismatch Est Start Work
		uint32 reserved1:3;
		uint32 iqMismatchEstResetRegsAn3:1;	// IQ Mismatch Est Reset Regs
		uint32 reserved2:3;
		uint32 iqMismatchEstValidIqAn3:1;	// Rx TD IQ Mismatch Est valid
		uint32 reserved3:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg18D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG18F 0x63C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegIiAn3:32;	// IQ Mismatch Est Reg II low
	} bitFields;
} RegPhyRxTdPhyRxtdReg18F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG191 0x644 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegQqAn3:32;	// IQ Mismatch Est Reg QQ low
	} bitFields;
} RegPhyRxTdPhyRxtdReg191_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG193 0x64C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstRegIqAn3:32;	// IQ Mismatch Est Reg IQ low
	} bitFields;
} RegPhyRxTdPhyRxtdReg193_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1E4 0x790 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap0Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 0.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1E4_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1E5 0x794 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap1Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 1.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1E5_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1E6 0x798 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap2Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 2.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1E6_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1E7 0x79C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap3Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 3.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1E7_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1E8 0x7A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap4Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 4.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1E8_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1E9 0x7A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap5Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 5.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1E9_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1EA 0x7A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap6Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 6.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Ea_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1EB 0x7AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap7Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 7.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Eb_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1EC 0x7B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap8Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 8.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Ec_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1ED 0x7B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap9Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 9.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Ed_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1EE 0x7B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap10Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 10.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Ee_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1EF 0x7BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqTap11Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 11.
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Ef_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1F8 0x7E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 disableSecDetection:1;	// disable_sec_detection
		uint32 disablePrimaryDetection:1;	// disable_primary_detection
		uint32 reserved0:2;
		uint32 detectionOnLltfBackoff:4;	// detection_on_lltf_backoff
		uint32 reserved1:24;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1F8_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1F9 0x7E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 watchDogLimit:32;	// watch dog limit
	} bitFields;
} RegPhyRxTdPhyRxtdReg1F9_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1FA 0x7E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 adjacentRejOnSecEn:1;	// adjacent_rej_on_sec_en
		uint32 reserved0:3;
		uint32 adjacentRejOnOobEn:1;	// adjacent_rej_on_oob_en
		uint32 reserved1:3;
		uint32 powerDivOobTh:11;	// power_div_oob_th
		uint32 reserved2:1;
		uint32 powerDivIbTh:11;	// power_div_ib_th
		uint32 reserved3:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Fa_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1FB 0x7EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 midPacketEn:1;	// cca_mid_long_dec
		uint32 reserved0:3;
		uint32 ccaMacBw:2;	// cca_mac_bw
		uint32 reserved1:2;
		uint32 midPacketAnt:2;	// mid_packet_ant
		uint32 reserved2:2;
		uint32 ccaPrim20OrSec20:1;	// cca_prim20_or_sec20
		uint32 ccaPrim20OrSec40:1;	// cca_prim20_or_sec40
		uint32 reserved3:2;
		uint32 ccaMidDetEnergyPassThTime:14;	// cca_mid_det_energy_pass_th_time
		uint32 reserved4:2;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Fb_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1FC 0x7F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 midDcIirIsBypassed:1;	// mid_dc_iir_is_bypassed 
		uint32 midOobDcIirIsBypassed:1;	// mid_oob_dc_iir_is_bypassed 
		uint32 reserved0:2;
		uint32 midDcIirAlpha:4;	// mid_dc_iir_alpha 
		uint32 ccaSecMidPowerTh:16;	// cca_sec_mid_power_th
		uint32 ccaMidMaxThPasses:7;	// cca_mid_max_th_passes
		uint32 reserved1:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Fc_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1FD 0x7F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaSecMidTh20:8;	// cca_sec_mid_th20
		uint32 reserved0:4;
		uint32 ccaSecMidTh40:8;	// cca_sec_mid_th40
		uint32 reserved1:4;
		uint32 ccaPrimMidTh:8;	// cca_prim_mid_th
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Fd_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG1FE 0x7F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPifsTime:14;	// cca_pifs_time
		uint32 reserved0:2;
		uint32 ccaWifiDetOnSecTime:14;	// cca_wifi_det_on_sec_period
		uint32 reserved1:1;
		uint32 ccaWifiDetOnSecEnable:1;	// cca_wifi_det_on_sec_enable
	} bitFields;
} RegPhyRxTdPhyRxtdReg1Fe_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG200 0x800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaMpdDramRm:3;	// cca_mpd_dram_rm
		uint32 reserved0:1;
		uint32 ccaMpdSramRm:3;	// cca_mpd_sram_rm
		uint32 reserved1:25;
	} bitFields;
} RegPhyRxTdPhyRxtdReg200_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG201 0x804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fftExtraGain20:2;	// fft_extra_gain
		uint32 reserved0:2;
		uint32 fftExtraGain40:2;	// fft_extra_gain
		uint32 reserved1:2;
		uint32 fftExtraGain80:2;	// fft_extra_gain
		uint32 reserved2:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg201_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG202 0x808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaDetectionTime:4;	// cca_detection_time
		uint32 ccaDetectionNPifsTime:6;	// cca_detection_n_pifs_time
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg202_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG203 0x80C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaEnergy10DelayPrim:14;	// cca_energy_1_0_delay_prim
		uint32 reserved0:18;
	} bitFields;
} RegPhyRxTdPhyRxtdReg203_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG204 0x810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freqShiftBypass:1;	// freq_shift_bypass
		uint32 freqShiftNcoBypass:1;	// freq_shift_nco_bypass
		uint32 freqShift11BBypass:1;	// freq_shift_11b_bypass
		uint32 freqShiftSig3Bypass:1;	// freq_shift_sig3_bypass
		uint32 freqShiftDummyEn:1;	// freq_shift_functional_bypass
		uint32 setHighEvmDecAtDetect:1;	// set_high_evm_dec_at_detect
		uint32 reserved0:2;
		uint32 freqShiftBOffset:8;	// freq_shift_b_offset
		uint32 freqShiftOfdmOffset:8;	// freq_shift_ofdm_offset
		uint32 freqShiftSig3Offset:8;	// freq_shift_sig3_offset
	} bitFields;
} RegPhyRxTdPhyRxtdReg204_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG205 0x814 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decisionMask:4;	// det_th_1ant_pri
		uint32 detCombTh:2;	// det_th_1ant_pri
		uint32 reserved0:2;
		uint32 coChMode:1;	// det_th_1ant_pri
		uint32 reserved1:3;
		uint32 coChRestartLen:10;	// det_th_1ant_pri
		uint32 reserved2:10;
	} bitFields;
} RegPhyRxTdPhyRxtdReg205_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG206 0x818 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectionMask0:4;	// detecttiom mask
		uint32 detectionMask1:4;	// detecttiom mask
		uint32 detectionMask2:4;	// detecttiom mask
		uint32 detectionMask3:4;	// detecttiom mask
		uint32 detectionMask4:4;	// detecttiom mask
		uint32 detectionMask5:4;	// detecttiom mask
		uint32 detectionMask6:4;	// detecttiom mask
		uint32 detectionMask7:4;	// detecttiom mask
	} bitFields;
} RegPhyRxTdPhyRxtdReg206_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG207 0x81C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTh1AntPri:8;	// det_th_1ant_pri
		uint32 detTh2AntPri:8;	// det_th_2ant_pri
		uint32 detTh3AntPri:8;	// det_th_3ant_pri
		uint32 detTh4AntPri:8;	// det_th_4ant_pri
	} bitFields;
} RegPhyRxTdPhyRxtdReg207_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG208 0x820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTh1AntSec:8;	// det_th_1ant_sec
		uint32 detTh2AntSec:8;	// det_th_2ant_sec
		uint32 detTh3AntSec:8;	// det_th_3ant_sec
		uint32 detTh4AntSec:8;	// det_th_4ant_sec
	} bitFields;
} RegPhyRxTdPhyRxtdReg208_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG209 0x824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaSecMidDetTh:8;	// cca_sec_mid_det_th
		uint32 ccaSecMidDetShortTh:8;	// cca_sec_mid_det_short_th
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg209_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG20A 0x828 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcRegIAn0:17;	// dc_reg_i_an0
		uint32 reserved0:15;
	} bitFields;
} RegPhyRxTdPhyRxtdReg20A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG20B 0x82C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcRegQAn0:17;	// dc_reg_q_an0
		uint32 reserved0:15;
	} bitFields;
} RegPhyRxTdPhyRxtdReg20B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG20C 0x830 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcRegIAn1:17;	// dc_reg_i_an1
		uint32 reserved0:15;
	} bitFields;
} RegPhyRxTdPhyRxtdReg20C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG20D 0x834 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcRegQAn1:17;	// dc_reg_q_an1
		uint32 reserved0:15;
	} bitFields;
} RegPhyRxTdPhyRxtdReg20D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG20E 0x838 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcRegIAn2:17;	// dc_reg_i_an2
		uint32 reserved0:15;
	} bitFields;
} RegPhyRxTdPhyRxtdReg20E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG20F 0x83C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcRegQAn2:17;	// dc_reg_q_an2
		uint32 reserved0:15;
	} bitFields;
} RegPhyRxTdPhyRxtdReg20F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG210 0x840 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcRegIAn3:17;	// dc_reg_i_an3
		uint32 reserved0:15;
	} bitFields;
} RegPhyRxTdPhyRxtdReg210_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG211 0x844 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcRegQAn3:17;	// dc_reg_q_an3
		uint32 reserved0:15;
	} bitFields;
} RegPhyRxTdPhyRxtdReg211_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG212 0x848 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bsCoeff1:9;	// bs_coeff_1
		uint32 reserved0:3;
		uint32 bsCoeff2:7;	// bs_coeff_2
		uint32 reserved1:1;
		uint32 bsCoeff3:7;	// bs_coeff_3
		uint32 reserved2:5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg212_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG213 0x84C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bsCoeff4:7;	// bs_coeff_4
		uint32 reserved0:1;
		uint32 bsCoeff5:5;	// bs_coeff_5
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg213_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG214 0x850 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bs80Coeff1:9;	// bs80_coeff_1
		uint32 reserved0:3;
		uint32 bs80Coeff2:7;	// bs80_coeff_2
		uint32 reserved1:1;
		uint32 bs80Coeff3:7;	// bs80_coeff_3
		uint32 reserved2:5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg214_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG215 0x854 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bs80Coeff4:7;	// bs80_coeff_4
		uint32 reserved0:1;
		uint32 bs80Coeff5:5;	// bs80_coeff_5
		uint32 reserved1:3;
		uint32 bs80Coeff6:5;	// bs80_coeff_6
		uint32 reserved2:3;
		uint32 bs80Coeff7:5;	// bs80_coeff_7
		uint32 reserved3:3;
	} bitFields;
} RegPhyRxTdPhyRxtdReg215_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG216 0x858 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bs80Coeff8:5;	// bs80_coeff_8
		uint32 reserved0:3;
		uint32 bs80Coeff9:5;	// bs80_coeff_9
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg216_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG217 0x85C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr0:32;	// spare_gpr_0
	} bitFields;
} RegPhyRxTdPhyRxtdReg217_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG218 0x860 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr1:32;	// spare_gpr_1
	} bitFields;
} RegPhyRxTdPhyRxtdReg218_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG219 0x864 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr2:32;	// spare_gpr_2
	} bitFields;
} RegPhyRxTdPhyRxtdReg219_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG21A 0x868 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr3:32;	// spare_gpr_3
	} bitFields;
} RegPhyRxTdPhyRxtdReg21A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG21B 0x86C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detCoarseCI:22;	// det_coarse_c_i
		uint32 reserved0:10;
	} bitFields;
} RegPhyRxTdPhyRxtdReg21B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG21C 0x870 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detCoarseCQ:22;	// det_coarse_c_q
		uint32 reserved0:10;
	} bitFields;
} RegPhyRxTdPhyRxtdReg21C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG21D 0x874 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstAccOutAn0:31;	// Rx TD Noise Estimation Accumulator Data An0
		uint32 reserved0:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg21D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG21E 0x878 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstAccOutAn1:31;	// Rx TD Noise Estimation Accumulator Data An1
		uint32 reserved0:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg21E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG21F 0x87C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstAccOutAn2:31;	// Rx TD Noise Estimation Accumulator Data An2
		uint32 reserved0:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg21F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG220 0x880 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstAccOutAn3:31;	// Rx TD Noise Estimation Accumulator Data An3
		uint32 reserved0:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg220_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG221 0x884 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSumIn0:31;	// short_sum_in0
		uint32 reserved0:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg221_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG222 0x888 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSumIn1:31;	// short_sum_in1
		uint32 reserved0:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg222_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG223 0x88C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSumIn2:31;	// short_sum_in2
		uint32 reserved0:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg223_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG224 0x890 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortSumIn3:31;	// short_sum_in3
		uint32 reserved0:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg224_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG225 0x894 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC0:11;	// difi_c0
		uint32 reserved0:1;
		uint32 difiC1:11;	// difi_c1
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg225_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG226 0x898 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC2:11;	// difi_c2
		uint32 reserved0:1;
		uint32 difiC3:11;	// difi_c3
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg226_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG227 0x89C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC4:11;	// difi_c4
		uint32 reserved0:1;
		uint32 difiC5:11;	// difi_c5
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg227_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG228 0x8A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC6:11;	// difi_c6
		uint32 reserved0:1;
		uint32 difiC7:11;	// difi_c7
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg228_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG229 0x8A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC8:11;	// difi_c8
		uint32 reserved0:1;
		uint32 difiC9:11;	// difi_c9
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg229_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG22A 0x8A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC10:11;	// difi_c10
		uint32 reserved0:1;
		uint32 difiC11:11;	// difi_c11
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg22A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG22B 0x8AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC12:11;	// difi_c12
		uint32 reserved0:1;
		uint32 difiC13:11;	// difi_c13
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg22B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG22C 0x8B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC14:11;	// difi_c14
		uint32 reserved0:1;
		uint32 difiC15:11;	// difi_c15
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg22C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG22D 0x8B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC16:11;	// difi_c16
		uint32 reserved0:1;
		uint32 difiC17:11;	// difi_c17
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg22D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG22E 0x8B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC18:11;	// difi_c18
		uint32 reserved0:1;
		uint32 difiC19:11;	// difi_c19
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg22E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG22F 0x8BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC20:11;	// difi_c20
		uint32 reserved0:1;
		uint32 difiC21:11;	// difi_c21
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg22F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG230 0x8C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC22:11;	// difi_c22
		uint32 reserved0:1;
		uint32 difiC23:11;	// difi_c23
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg230_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG231 0x8C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC24:11;	// difi_c24
		uint32 reserved0:1;
		uint32 difiC25:11;	// difi_c25
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg231_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG232 0x8C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiC26:11;	// difi_c26
		uint32 reserved0:1;
		uint32 difiC27:11;	// difi_c27
		uint32 reserved1:9;
	} bitFields;
} RegPhyRxTdPhyRxtdReg232_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG233 0x8CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 difiF8To1DecShifter:3;	// difi_f8to1_dec_shifter
		uint32 reserved0:1;
		uint32 difiInOffst:15;	// difi_in_offst
		uint32 reserved1:1;
		uint32 difiMulBy3:1;	// difi_mul_by_3
		uint32 reserved2:10;
		uint32 difiBypass:1;	// difi_bypass
	} bitFields;
} RegPhyRxTdPhyRxtdReg233_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG234 0x8D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 thresholdMaxCorr:12;	// threshold_max_corr
		uint32 reserved0:8;
		uint32 thresholdMaxCorrAdj80:12;	// threshold_max_corr_adj_80
	} bitFields;
} RegPhyRxTdPhyRxtdReg234_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG235 0x8D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 thresholdMaxCorrAdj20:12;	// threshold_max_corr_adj_20
		uint32 thresholdMaxCorrAdj40:12;	// threshold_max_corr_adj_40
		uint32 detFineTimingContinueAfterRdy:8;	// det_fine_timing_continue_after_rdy
	} bitFields;
} RegPhyRxTdPhyRxtdReg235_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG236 0x8D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTh1AntCoCh:8;	// det_th_1ant_co_ch
		uint32 detTh2AntCoCh:8;	// det_th_2ant_co_ch
		uint32 detTh3AntCoCh:8;	// det_th_3ant_co_ch
		uint32 detTh4AntCoCh:8;	// det_th_4ant_co_ch
	} bitFields;
} RegPhyRxTdPhyRxtdReg236_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG237 0x8DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sig3ReadyOffset:12;	// offset in clk from ht short to sig3 ready
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdPhyRxtdReg237_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG238 0x8E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decDcIirAlpha:4;	// decimator iir filter alpha
		uint32 decDcIirIsBypassed:1;	// decimator iir filter bypass
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg238_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG239 0x8E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCnt:32;	// free_running_cnt
	} bitFields;
} RegPhyRxTdPhyRxtdReg239_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG23A 0x8E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaValidDetTimer:8;	// timer in 20Mhz from sec detect till primary
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdPhyRxtdReg23A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG23B 0x8EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 afeTpControl:2;	// timer in 20Mhz from sec detect till primary
		uint32 reserved0:2;
		uint32 decimatorTpControl:1;	// decimator hb sensitive:  , 1'b0 all strobe , 1'b1: case hb_mode , CB80: all strobes , HB4080:one every two samples , HB2080:one every four samples
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdPhyRxtdReg23B_u;

/*REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_MSG 0x8F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscTestPlugMsg:32;	// risc data to logger
	} bitFields;
} RegPhyRxTdPhyRxtdRiscTestPlugMsg_u;

/*REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_MSG_DONE 0x8F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscTestPlugMsgDone:1;	// risc data to logger
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdPhyRxtdRiscTestPlugMsgDone_u;

/*REG_PHY_RX_TD_PHY_RXTD_RISC_TEST_PLUG_FAST_MSG 0x8F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscTestPlugFastMsg:32;	// risc data to logger
	} bitFields;
} RegPhyRxTdPhyRxtdRiscTestPlugFastMsg_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG23F 0x8FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 forceDetectionOnBands:4;	// force_detection_on_bands
		uint32 blockDetectionOnBands:4;	// block_detection_on_bands
		uint32 thresholdMaxCorrCoChannel20:12;	// threshold_max_corr_co_channel_20
		uint32 thresholdMaxCorrCoChannel40:12;	// threshold_max_corr_co_channel_40
	} bitFields;
} RegPhyRxTdPhyRxtdReg23F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG240 0x900 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn0Corr64AtCommonFineT:27;	// Detector an0 Abs corr64 smoothed at fine timing
		uint32 reserved0:5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg240_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG241 0x904 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn1Corr64AtCommonFineT:27;	// Detector an1 Abs corr64 smoothed at fine timing
		uint32 reserved0:5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg241_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG242 0x908 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn2Corr64AtCommonFineT:27;	// Detector an2 Abs corr64 smoothed at fine timing
		uint32 reserved0:5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg242_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG243 0x90C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn3Corr64AtCommonFineT:27;	// Detector an3 Abs corr64 smoothed at fine timing
		uint32 reserved0:5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg243_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG244 0x910 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsCommonCorr64AtCommonFineT:27;	// Detector common Abs corr64 smoothed at fine timing
		uint32 reserved0:5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg244_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG245 0x914 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn0Corr16AtLocalCoarseT:20;	// Detector an0 Abs corr64 smoothed at fine timing
		uint32 reserved0:12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg245_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG246 0x918 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn1Corr16AtLocalCoarseT:20;	// Detector an1 Abs corr64 smoothed at fine timing
		uint32 reserved0:12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg246_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG247 0x91C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn2Corr16AtLocalCoarseT:20;	// Detector an2 Abs corr64 smoothed at fine timing
		uint32 reserved0:12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg247_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG248 0x920 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsAn3Corr16AtLocalCoarseT:20;	// Detector an3 Abs corr64 smoothed at fine timing
		uint32 reserved0:12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg248_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG249 0x924 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAbsCommonCorr16AtCommonCoarseT:20;	// Detector common Abs corr64 smoothed at fine timing
		uint32 reserved0:12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg249_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG_LOOPBACK_CTRL 0x928 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 digitalLpbk:1;	// digital LoopBack Mode
		uint32 reversedDigitalLpbk:1;	// reveresed digital LoopBack Mode
		uint32 reversedDigitalLpbkRssi:1;	// reversed LoopBack Mode rssi
		uint32 tx0LpbkSelect:1;	// Tx0 LoopBack Select
		uint32 rx0LpbkSelect:2;	// Rx0 LoopBack Select
		uint32 rx1LpbkSelect:2;	// Rx1 LoopBack Select
		uint32 rx2LpbkSelect:2;	// Rx2 LoopBack Select
		uint32 rx3LpbkSelect:2;	// Rx3 LoopBack Select
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdPhyRxtdRegLoopbackCtrl_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG24B 0x92C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpTimingEnable:1;	// cp_timing_enable
		uint32 cpTimingSkipTimingFromSig:2;	// cp_timing_skip_timing_from_sig
		uint32 cpTimingSkipTimingFromLtfs:2;	// cp_timing_skip_timing_from_ltfs
		uint32 cpTimingSkipTimingFromData:2;	// cp_timing_skip_timing_from_data
		uint32 reserved0:1;
		uint32 cpTimingNSymToAvg:7;	// cp_timing_n_sym_to_avg
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdPhyRxtdReg24B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG24C 0x930 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpTimingWindowSize:5;	// cp_timing_window_size
		uint32 cpTimingMinimalLtfsForEstimation:3;	// cp_timing)minimal_ltfs_for_estimation
		uint32 cpTimingSync:6;	// cp_timing_sync
		uint32 reserved0:2;
		uint32 cpTimingWindowSizeForSigEstimation:5;	// cp_timing_window_size_for_sig_estimation
		uint32 reserved1:11;
	} bitFields;
} RegPhyRxTdPhyRxtdReg24C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG24D 0x934 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpTimingShiftFromLookahead:6;	// cp_timing_shift_from_lookahead
		uint32 reserved0:26;
	} bitFields;
} RegPhyRxTdPhyRxtdReg24D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG24E 0x938 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freqShiftDummyDetectMask:4;	// dummy detect (ofdm/11b) mask
		uint32 freqShiftDummyDetect11B:1;	// dummy detect 11b
		uint32 reserved0:3;
		uint32 freqShiftDummyDetectJump:16;	// dummy detect (ofdm/11b) jump iq sample counter limit
		uint32 reserved1:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg24E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG24F 0x93C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freqShiftDummySig3Bw:2;	// dummy sig3 bw
		uint32 reserved0:2;
		uint32 freqShiftDummySig3Qam256:1;	// dummy sig3 qam256
		uint32 reserved1:3;
		uint32 freqShiftDummySig3Jump:16;	// dummy sig3 jump iq sample counter limit
		uint32 reserved2:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg24F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG250 0x940 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 m11BBLongOffset:16;	// m11b_b_long_offset
		uint32 m11BBShortOffset:16;	// m11b_b_short_offset
	} bitFields;
} RegPhyRxTdPhyRxtdReg250_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG251 0x944 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detTopTh3ForLtf:15;	// Top Detection thershold register 3 for ltf detection.
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdPhyRxtdReg251_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG252 0x948 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqRamMode:1;	// iq_ram_mode , 0-FIR equalizer coefs are saved in memory and FDL coefs are taken from prog model registers
		uint32 iqFdlBypass:1;	// iq_fdl_bypass
		uint32 iqFdlBypassRam:1;	// iq_fdl_bypass_ram
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdPhyRxtdReg252_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG253 0x94C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqFdlDBypassRamAnt0:8;	// antenna #0 d (=d1+d2) value when (iq_fdl_bypass_ram=1)
		uint32 iqFdlDBypassRamAnt1:8;	// antenna #1 d (=d1+d2) value when (iq_fdl_bypass_ram=1)
		uint32 iqFdlDBypassRamAnt2:8;	// antenna #2 d (=d1+d2) value when (iq_fdl_bypass_ram=1)
		uint32 iqFdlDBypassRamAnt3:8;	// antenna #3 d (=d1+d2) value when (iq_fdl_bypass_ram=1)
	} bitFields;
} RegPhyRxTdPhyRxtdReg253_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG254 0x950 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqPgc1Ant0:5;	// pgc1 value to be used before AGC accelerators are ready antenna #0
		uint32 reserved0:3;
		uint32 iqPgc1Ant1:5;	// pgc1 value to be used before AGC accelerators are ready antenna #1
		uint32 reserved1:3;
		uint32 iqPgc1Ant2:5;	// pgc1 value to be used before AGC accelerators are ready antenna #2
		uint32 reserved2:3;
		uint32 iqPgc1Ant3:5;	// pgc1 value to be used before AGC accelerators are ready antenna #3
		uint32 reserved3:3;
	} bitFields;
} RegPhyRxTdPhyRxtdReg254_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG255 0x954 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqPgc2Ant0:4;	// pgc2 value to be used before AGC accelerators are ready antenna #0
		uint32 iqPgc2Ant1:4;	// pgc2 value to be used before AGC accelerators are ready antenna #1
		uint32 iqPgc2Ant2:4;	// pgc2 value to be used before AGC accelerators are ready antenna #2
		uint32 iqPgc2Ant3:4;	// pgc2 value to be used before AGC accelerators are ready antenna #3
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg255_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG256 0x958 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqCalMode:1;	// IQ equalizer calibration mode
		uint32 iqEqBypassRam:1;	// IQ equalizer bypass RAM. Use cofficients from prog model
		uint32 iqW12Bypass:1;	// IQ W1, W2 De-correlator bypass
		uint32 iqPgcFromAgcAcc:1;	// select PGC1,PGC2 source: , 0-progmodels, 1-AGC accelerator
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdPhyRxtdReg256_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG257 0x95C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Hb2080An0:14;	// iq_gain_w1_reg_an0 in hb2080 mode
		uint32 reserved0:6;
		uint32 iqW2Hb2080An0:12;	// iq_gain_w2_reg_an0 in hb2080 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg257_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG258 0x960 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Hb2080An1:14;	// iq_gain_w1_reg_an1 in hb2080 mode
		uint32 reserved0:6;
		uint32 iqW2Hb2080An1:12;	// iq_gain_w2_reg_an1 in hb2080 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg258_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG259 0x964 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Hb2080An2:14;	// iq_gain_w1_reg_an2 in hb2080 mode
		uint32 reserved0:6;
		uint32 iqW2Hb2080An2:12;	// iq_gain_w2_reg_an2 in hb2080 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg259_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG25A 0x968 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Hb2080An3:14;	// iq_gain_w1_reg_an3 in hb2080 mode
		uint32 reserved0:6;
		uint32 iqW2Hb2080An3:12;	// iq_gain_w2_reg_an3 in hb2080 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg25A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG25B 0x96C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Hb4080An0:14;	// iq_gain_w1_reg_an0 in hb4080 mode
		uint32 reserved0:6;
		uint32 iqW2Hb4080An0:12;	// iq_gain_w2_reg_an0 in hb4080 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg25B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG25C 0x970 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Hb4080An1:14;	// iq_gain_w1_reg_an1 in hb4080 mode
		uint32 reserved0:6;
		uint32 iqW2Hb4080An1:12;	// iq_gain_w2_reg_an1 in hb4080 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg25C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG25D 0x974 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Hb4080An2:14;	// iq_gain_w1_reg_an2 in hb4080 mode
		uint32 reserved0:6;
		uint32 iqW2Hb4080An2:12;	// iq_gain_w2_reg_an2 in hb4080 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg25D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG25E 0x978 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Hb4080An3:14;	// iq_gain_w1_reg_an3 in hb4080 mode
		uint32 reserved0:6;
		uint32 iqW2Hb4080An3:12;	// iq_gain_w2_reg_an3 in hb4080 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg25E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG25F 0x97C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Cb80An0:14;	// iq_gain_w1_reg_an0 in cb80 mode
		uint32 reserved0:6;
		uint32 iqW2Cb80An0:12;	// iq_gain_w2_reg_an0 in cb80 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg25F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG260 0x980 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Cb80An1:14;	// iq_gain_w1_reg_an1 in cb80 mode
		uint32 reserved0:6;
		uint32 iqW2Cb80An1:12;	// iq_gain_w2_reg_an1 in cb80 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg260_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG261 0x984 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Cb80An2:14;	// iq_gain_w1_reg_an2 in cb80 mode
		uint32 reserved0:6;
		uint32 iqW2Cb80An2:12;	// iq_gain_w2_reg_an2 in cb80 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg261_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG262 0x988 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqW1Cb80An3:14;	// iq_gain_w1_reg_an3 in cb80 mode
		uint32 reserved0:6;
		uint32 iqW2Cb80An3:12;	// iq_gain_w2_reg_an3 in cb80 mode
	} bitFields;
} RegPhyRxTdPhyRxtdReg262_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG263 0x98C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEst160Mhz:1;	// w1,w2 estimator work after w1,w2 de-correlator (@80MHz) or after equalizer (@160MHZ)
		uint32 iqMismatchAccumRssi:1;	// use iq_mismatch w12_estimator's accumulator to accumulate rssi_data
		uint32 reserved0:2;
		uint32 iqMismatchEstShiftAnt0:4;	// w1,w2 estimator shift value antenna 0
		uint32 iqMismatchEstShiftAnt1:4;	// w1,w2 estimator shift value antenna 1
		uint32 iqMismatchEstShiftAnt2:4;	// w1,w2 estimator shift value antenna 2
		uint32 iqMismatchEstShiftAnt3:4;	// w1,w2 estimator shift value antenna 3
		uint32 reserved1:12;
	} bitFields;
} RegPhyRxTdPhyRxtdReg263_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG264 0x990 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqMismatchEstOfIiAnt0:1;	// OverFlow indication on i^2 32-bit accumulation antenna 0
		uint32 iqMismatchEstOfQqAnt0:1;	// OverFlow indication on q^2 32-bit accumulation antenna 0
		uint32 iqMismatchEstOfIqAnt0:1;	// OverFlow indication on i*q 32-bit accumulation antenna 0
		uint32 reserved0:1;
		uint32 iqMismatchEstOfIiAnt1:1;	// OverFlow indication on i^2 32-bit accumulation antenna 1
		uint32 iqMismatchEstOfQqAnt1:1;	// OverFlow indication on q^2 32-bit accumulation antenna 1
		uint32 iqMismatchEstOfIqAnt1:1;	// OverFlow indication on i*q 32-bit accumulation antenna 1
		uint32 reserved1:1;
		uint32 iqMismatchEstOfIiAnt2:1;	// OverFlow indication on i^2 32-bit accumulation antenna 2
		uint32 iqMismatchEstOfQqAnt2:1;	// OverFlow indication on q^2 32-bit accumulation antenna 2
		uint32 iqMismatchEstOfIqAnt2:1;	// OverFlow indication on i*q 32-bit accumulation antenna 2
		uint32 reserved2:1;
		uint32 iqMismatchEstOfIiAnt3:1;	// OverFlow indication on i^2 32-bit accumulation antenna 3
		uint32 iqMismatchEstOfQqAnt3:1;	// OverFlow indication on q^2 32-bit accumulation antenna 3
		uint32 iqMismatchEstOfIqAnt3:1;	// OverFlow indication on i*q 32-bit accumulation antenna 3
		uint32 reserved3:17;
	} bitFields;
} RegPhyRxTdPhyRxtdReg264_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG265 0x994 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqRamReadTrigger:1;	// Upon writing '1' a RAM read is performed into IQ coefficients
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg265_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG266 0x998 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap0Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 0 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg266_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG267 0x99C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap1Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 1 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg267_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG268 0x9A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap2Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 2 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg268_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG269 0x9A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap3Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 3 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg269_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG26A 0x9A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap4Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 4 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg26A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG26B 0x9AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap5Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 5 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg26B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG26C 0x9B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap6Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 6 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg26C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG26D 0x9B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap7Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 7 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg26D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG26E 0x9B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap8Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 8 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg26E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG26F 0x9BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap9Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 9 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg26F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG270 0x9C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap10Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 10 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg270_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG271 0x9C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap11Ant0:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 11 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg271_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG272 0x9C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap0Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 0 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg272_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG273 0x9CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap1Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 1 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg273_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG274 0x9D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap2Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 2 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg274_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG275 0x9D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap3Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 3 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg275_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG276 0x9D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap4Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 4 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg276_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG277 0x9DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap5Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 5 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg277_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG278 0x9E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap6Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 6 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg278_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG279 0x9E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap7Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 7 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg279_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG27A 0x9E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap8Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 8 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg27A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG27B 0x9EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap9Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 9 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg27B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG27C 0x9F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap10Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 10 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg27C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG27D 0x9F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap11Ant1:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 11 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg27D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG27E 0x9F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap0Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 0 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg27E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG27F 0x9FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap1Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 1 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg27F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG280 0xA00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap2Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 2 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg280_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG281 0xA04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap3Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 3 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg281_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG282 0xA08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap4Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 4 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg282_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG283 0xA0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap5Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 5 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg283_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG284 0xA10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap6Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 6 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg284_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG285 0xA14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap7Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 7 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg285_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG286 0xA18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap8Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 8 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg286_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG287 0xA1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap9Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 9 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg287_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG288 0xA20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap10Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 10 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg288_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG289 0xA24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap11Ant2:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 11 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg289_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG28A 0xA28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap0Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 0 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg28A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG28B 0xA2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap1Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 1 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg28B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG28C 0xA30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap2Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 2 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg28C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG28D 0xA34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap3Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 3 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg28D_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG28E 0xA38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap4Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 4 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg28E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG28F 0xA3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap5Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 5 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg28F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG290 0xA40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap6Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 6 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg290_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG291 0xA44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap7Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 7 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg291_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG292 0xA48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap8Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 8 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg292_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG293 0xA4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap9Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 9 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg293_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG294 0xA50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap10Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 10 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg294_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG295 0xA54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqEqUsedTap11Ant3:10;	// IQ Mismatch Equalizer Calibration Coefficient Num 11 used in filter
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdPhyRxtdReg295_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG296 0xA58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freqThrForDc0:7;	// dc frequency threshold
		uint32 reserved0:1;
		uint32 freqThrForDc1:7;	// dc frequency threshold
		uint32 reserved1:1;
		uint32 freqThrForDc2:7;	// dc frequency threshold
		uint32 reserved2:1;
		uint32 freqThrForDc3:7;	// dc frequency threshold
		uint32 reserved3:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg296_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG297 0xA5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseThrForDc0:15;	// noise threshold for dc
		uint32 reserved0:1;
		uint32 noiseThrForDc1:15;	// noise threshold for dc
		uint32 reserved1:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg297_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG298 0xA60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseThrForDc2:15;	// noise threshold for dc
		uint32 reserved0:1;
		uint32 noiseThrForDc3:15;	// noise threshold for dc
		uint32 reserved1:1;
	} bitFields;
} RegPhyRxTdPhyRxtdReg298_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG299 0xA64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsMuShiftAnt0:4;	// IQ NLMS mu_shift value antenna 0
		uint32 iqLmsMuShiftAnt1:4;	// IQ NLMS mu_shift value antenna 1
		uint32 iqLmsMuShiftAnt2:4;	// IQ NLMS mu_shift value antenna 2
		uint32 iqLmsMuShiftAnt3:4;	// IQ NLMS mu_shift value antenna 3
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg299_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG29A 0xA68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsAlpha:2;	// IQ LMS alpha - value is summed with 8, so final Alpha value can reach 8:10
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg29A_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG29B 0xA6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsCoeffEn0:1;	// If 0 coefficient #0 is not updated after the LMS step 
		uint32 iqLmsCoeffEn1:1;	// If 0 coefficient #1 is not updated after the LMS step 
		uint32 iqLmsCoeffEn2:1;	// If 0 coefficient #2 is not updated after the LMS step 
		uint32 iqLmsCoeffEn3:1;	// If 0 coefficient #3 is not updated after the LMS step 
		uint32 iqLmsCoeffEn4:1;	// If 0 coefficient #4 is not updated after the LMS step 
		uint32 iqLmsCoeffEn5:1;	// If 0 coefficient #5 is not updated after the LMS step 
		uint32 iqLmsCoeffEn6:1;	// If 0 coefficient #6 is not updated after the LMS step 
		uint32 iqLmsCoeffEn7:1;	// If 0 coefficient #7 is not updated after the LMS step 
		uint32 iqLmsCoeffEn8:1;	// If 0 coefficient #8 is not updated after the LMS step 
		uint32 iqLmsCoeffEn9:1;	// If 0 coefficient #9 is not updated after the LMS step 
		uint32 iqLmsCoeffEn10:1;	// If 0 coefficient #10 is not updated after the LMS step 
		uint32 iqLmsCoeffEn11:1;	// If 0 coefficient #11 is not updated after the LMS step 
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdPhyRxtdReg29B_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG29C 0xA70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsStepsToRunAnt0:16;	// Number od steps to run the LMS antenna 0
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg29C_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG29E 0xA78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsStepsToRunAnt1:16;	// Number od steps to run the LMS antenna 1
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg29E_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG29F 0xA7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsStepsToRunAnt2:16;	// Number od steps to run the LMS antenna 2
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg29F_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A0 0xA80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsStepsToRunAnt3:16;	// Number od steps to run the LMS antenna 3
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A0_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A1 0xA84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsStartWorkAnt0:1;	// start updating the FIR coefficients antenna 0
		uint32 iqLmsStartWorkAnt1:1;	// start updating the FIR coefficients antenna 1
		uint32 iqLmsStartWorkAnt2:1;	// start updating the FIR coefficients antenna 2
		uint32 iqLmsStartWorkAnt3:1;	// start updating the FIR coefficients antenna 3
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A1_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A2 0xA88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsErrorIndicatorAnt0:20;	// RO register holding the current Error_Indicator value for antenna 0
		uint32 iqLmsErrorValidAnt0:1;	// IQ NLMS Error valid antenna 0
		uint32 reserved0:11;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A2_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A3 0xA8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsErrorIndicatorAnt1:20;	// RO register holding the current Error_Indicator value for antenna 1
		uint32 iqLmsErrorValidAnt1:1;	// IQ NLMS Error valid antenna 1
		uint32 reserved0:11;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A3_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A4 0xA90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsErrorIndicatorAnt2:20;	// RO register holding the current Error_Indicator value for antenna 2
		uint32 iqLmsErrorValidAnt2:1;	// IQ NLMS Error valid antenna 2
		uint32 reserved0:11;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A4_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A5 0xA94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 iqLmsErrorIndicatorAnt3:20;	// RO register holding the current Error_Indicator value for antenna 3
		uint32 iqLmsErrorValidAnt3:1;	// IQ NLMS Error valid antenna 3
		uint32 reserved0:11;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A5_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A6 0xA98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDcRadarEnable:1;	// DC Cancellation Enable
		uint32 rxDcRadarHwSelect:1;	// Progmodel or Accelerator
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A6_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A7 0xA9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 radarDcOffsetIAnt0:12;	// DC Offset I Ant 1
		uint32 radarDcOffsetQAnt0:12;	// DC Offset Q Ant 1
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A7_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A8 0xAA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 radarDcOffsetIAnt1:12;	// DC Offset I Ant 2
		uint32 radarDcOffsetQAnt1:12;	// DC Offset Q Ant 2
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A8_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2A9 0xAA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 radarDcOffsetIAnt2:12;	// DC Offset I Ant 3
		uint32 radarDcOffsetQAnt2:12;	// DC Offset Q Ant 3
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2A9_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2AA 0xAA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 radarDcOffsetIAnt3:12;	// DC Offset I Ant 4
		uint32 radarDcOffsetQAnt3:12;	// DC Offset Q Ant 4
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2Aa_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2AB 0xAAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDigitalGainEnable:1;	// Digital Gain Enable
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2Ab_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2AC 0xAB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPacketCounter:27;	// cca_packet_counter
		uint32 reserved0:5;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2Ac_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2AD 0xAB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detCoarseGclkEnCb:1;	// det_coarse_gclk_en_cb
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2Ad_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2AE 0xAB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel0IResult:28;	// Geortzel 0 I Result.
		uint32 reserved0:4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2Ae_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2AF 0xABC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel0QResult:28;	// Geortzel 0 Q Result.
		uint32 reserved0:3;
		uint32 geortzel0DataValid:1;	// Geortzel 0 Data Valid.
	} bitFields;
} RegPhyRxTdPhyRxtdReg2Af_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2B0 0xAC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel1IResult:28;	// Geortzel 1 I Result.
		uint32 reserved0:4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2B0_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2B1 0xAC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel1QResult:28;	// Geortzel 1 Q Result.
		uint32 reserved0:3;
		uint32 geortzel1DataValid:1;	// Geortzel 1 Data Valid.
	} bitFields;
} RegPhyRxTdPhyRxtdReg2B1_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2B2 0xAC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel2IResult:28;	// Geortzel 2 I Result.
		uint32 reserved0:4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2B2_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2B3 0xACC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel2QResult:28;	// Geortzel 2 Q Result.
		uint32 reserved0:3;
		uint32 geortzel2DataValid:1;	// Geortzel 2 Data Valid.
	} bitFields;
} RegPhyRxTdPhyRxtdReg2B3_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2B4 0xAD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel3IResult:28;	// Geortzel 3 I Result.
		uint32 reserved0:4;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2B4_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2B5 0xAD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 geortzel3QResult:28;	// Geortzel 3 Q Result.
		uint32 reserved0:3;
		uint32 geortzel3DataValid:1;	// Geortzel 3 Data Valid.
	} bitFields;
} RegPhyRxTdPhyRxtdReg2B5_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2B6 0xAD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyTsfTimerEn:1;	// phy_tsf_timer_en
		uint32 phyTsfTimerClear:1;	// phy_tsf_timer_clear
		uint32 reserved0:2;
		uint32 phyTsfTimerUnit:9;	// phy_tsf_timer_unit
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdPhyRxtdReg2B6_u;

/*REG_PHY_RX_TD_PHY_RXTD_REG2B7 0xADC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyTsfTimer:32;	// phy_tsf_timer
	} bitFields;
} RegPhyRxTdPhyRxtdReg2B7_u;



#endif // _PHY_RX_TD_REGS_H_
